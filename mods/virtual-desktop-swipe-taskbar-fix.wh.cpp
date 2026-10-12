// ==WindhawkMod==
// @id              virtual-desktop-swipe-taskbar-fix
// @name            Virtual Desktop Swipe Taskbar Fix
// @description     Restores taskbar presence in virtual desktop swipe previews.
// @version         1.0.2
// @author          meteoni
// @github          https://github.com/Meteoni
// @license         MIT
// @include         explorer.exe
// @architecture    amd64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Virtual Desktop Swipe Taskbar Fix

Restores the missing incoming taskbar bitmap during a touchpad virtual desktop swipe
on Windows 11.

![The issue](https://raw.githubusercontent.com/Meteoni/meteoni-assets/main/virtual-desktop-swipe-taskbar-fix/virtual-desktop-swipe-taskbar-fix.png)
_The issue_

## Limitation

While a swipe is held, both previews can show the current desktop's taskbar
buttons, instead of reflecting the incoming taskbar's current state. This is
because Windows has not yet committed the switch during a swipe gesture, and
thumbnails taken at that time inevitably looks the same as on the outgoing
desktop. 

Keyboard switching avoids this because it freezes the outgoing taskbar preview
and updates the actual taskbar during the transition, allowing the live incoming
preview to show the destination desktop's buttons.

ARM64 is not supported: native methods are resolved through symbols, but internal
member offsets are discovered from those methods at initialization. Initialization
stops if the required symbols or layout cannot be recognized. 
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <unknwn.h>
#include <cstddef>
#include <cstdio>
#include <cwchar>
#include <optional>
#include <regex>
#include <string>
#include <vector>

using GetTrayBitmapResult = HRESULT (*)(void*, const void*, int, const RECT&, void*, IUnknown*);
using InitializeGestureWindow = HRESULT (*)(void*, unsigned long);
using GetTrayWindowForMonitor = HWND (*)(HMONITOR);
using AddThumbnailToCollection = HRESULT (*)(void*, unsigned int, IUnknown*);
using CreateWindowThumbnail = HRESULT (*)(void*, IUnknown*, HWND, DWORD,
                                        const SIZE*, RECT*, const GUID&, void**);
using SetThumbnailRect = HRESULT (*)(IUnknown*, const RECT&, int, void*);

GetTrayBitmapResult g_getTrayBitmapResultOriginal;
InitializeGestureWindow g_initializeGestureWindow;
GetTrayWindowForMonitor g_getTrayWindowForMonitor;
AddThumbnailToCollection g_addThumbnailToCollection;
CreateWindowThumbnail g_createWindowThumbnail;
SetThumbnailRect g_setThumbnailRect;

struct GestureLayout {
    size_t window;
    size_t bitmap;
    size_t customPaint;
    size_t contextType;
    size_t factory;
    size_t parent;
    size_t firstCollection;
    size_t secondCollection;
    unsigned int firstCollectionSelector;
} g_gestureLayout;

// Like existing shell mods, inspect symbol-resolved methods with Wh_Disasm.
// No guessed/default offsets are used when an instruction pattern is missing.
bool DiscoverGestureLayout(void* bitmapResult, void* initialize, void* addThumbnail,
                           GestureLayout& layout) {
    std::string code[3];
    void* functions[]{bitmapResult, initialize, addThumbnail};
    const int limits[]{220, 64, 180};
    for (size_t index = 0; index < ARRAYSIZE(functions); ++index) {
        auto* instruction = static_cast<std::byte*>(functions[index]);
        for (int count = 0; count < limits[index]; ++count) {
            WH_DISASM_RESULT decoded{};
            if (!Wh_Disasm(instruction, &decoded) || !decoded.length || decoded.length > 15) return false;
            std::string text(decoded.text);
            for (char& character : text) {
                if (character >= 'A' && character <= 'Z') character += 'a' - 'A';
            }
            code[index] += text + ';';
            instruction += decoded.length;
            if (text == "ret") break;
        }
    }
    auto offsetFrom = [](const std::string& instructions, const std::string& pattern,
                         size_t group = 1) -> std::optional<size_t> {
        // Read the chosen capture group as a member offset or selector constant.
        // A missing instruction is a discovery failure, never a default value.
        std::smatch match;
        if (!std::regex_search(instructions, match, std::regex(pattern))) return std::nullopt;
        size_t offset = std::stoull(match[group], nullptr, 0);
        if (offset >= 0x10000) return std::nullopt;
        return offset;
    };

    std::smatch self;
    // At entry rcx is the gesture object. "mov rsi, rcx", for example, tells
    // us which register the method uses for later object-member accesses.
    if (!std::regex_search(code[0], self, std::regex(R"(mov (r\w+), rcx;)"))) return false;
    const std::string owner = self[1];
    const std::string ownerMember = R"(\[)" + owner + R"(\+(0x[0-9a-f]+)\])";

    // Recognize "mov rcx, [rdx+offset]; test rcx, rcx": rdx holds bitmapData,
    // so this offset locates its bitmap handle. The backreference requires the
    // load and null test to use the same register; an intervening xor is allowed.
    auto bitmap = offsetFrom(code[0],
        R"(mov (r\w+), (?:qword ptr )?\[rdx\+(0x[0-9a-f]+)\];(?:xor \w+, \w+;)?test \1, \1;)", 2);
    // The first pointer-sized "cmp [owner+offset], register" in this method
    // checks whether the destination HWND exists before using the bitmap.
    auto window = offsetFrom(code[0], R"(cmp (?:qword ptr )?)" + ownerMember + R"(, \w+;)");
    // Initialize writes its context-type argument (edx) into [rcx+offset].
    auto contextType = offsetFrom(code[1], R"(mov (?:dword ptr )?\[rcx\+(0x[0-9a-f]+)\], edx;)");

    // Before creating the bitmap thumbnail, the first pointer load from the
    // owner obtains the factory. The load into rdx supplies the parent thumbnail
    // as the factory call's second argument.
    auto factory = offsetFrom(code[0], R"(mov r\w+, (?:qword ptr )?)" + ownerMember + ';');
    auto parent = offsetFrom(code[0], R"(mov rdx, (?:qword ptr )?)" + ownerMember + ';');
    size_t customPaint = SIZE_MAX;
    std::smatch flag;
    // "lea register, [owner+offset]" takes the custom-paint flag's address.
    // Require a byte comparison through that same register before trusting it.
    if (std::regex_search(code[0], flag,
            std::regex(R"(lea (r\w+), \[)" + owner + R"(\+(0x[0-9a-f]+)\];)"))) {
        if (!std::regex_search(code[0],
                std::regex(R"(cmp (?:byte ptr )?\[)" + flag[1].str() + R"(\], (?:[abcd]l|r\d+b);)"))) return false;
        customPaint = std::stoull(flag[2], nullptr, 0);
        if (customPaint >= 0x10000) return false;
    } else {
        // Older implementations have only the context-type branch, with no
        // custom-paint member. Recognize that form rather than invent an offset.
        if (!contextType || std::regex_search(code[0], std::regex("byte ptr \\[" + owner))) return false;
        char typeOffset[32]{};
        snprintf(typeOffset, sizeof(typeOffset), "%zx", *contextType);
        std::regex typeBranch("cmp dword ptr \\[" + owner + "\\+0x0*" +
                              typeOffset + R"(\], 0x0*2;)");
        // In the older method, both relevant branches compare this context
        // member with 2. Require both checks to recognize the mode-only form.
        auto branches = std::distance(std::sregex_iterator(code[0].begin(), code[0].end(), typeBranch), std::sregex_iterator());
        if (branches != 2) return false;
    }

    // Let Windows own its array layout and AddRef/append operations. Recover
    // both collection addresses to match the array passed to this callback.
    // Either collection can receive the adjacent desktop, depending on direction.
    std::smatch collectionOwner;
    std::smatch selectorRegister;
    // The append method may copy rcx (object) and edx (selector) into saved
    // registers. If it keeps the arguments in place, use rcx and edx directly.
    const std::string collectionBase = std::regex_search(code[2], collectionOwner,
        std::regex(R"(mov (r\w+), rcx;)")) ? collectionOwner[1].str() : "rcx";
    const std::string selector = std::regex_search(code[2], selectorRegister,
        std::regex(R"(mov (e\w+), edx;)")) ? selectorRegister[1].str() : "edx";
    // "cmp selector, value; jne secondBranch" means equality chooses the
    // first collection; every other selector chooses the second collection.
    auto firstCollectionSelector = offsetFrom(code[2], "cmp " + selector +
        R"(, (0x[0-9a-f]+|[0-9]+);(?:jne|jnz) )");
    std::smatch collectionTarget;
    // Each branch takes its array address with "lea target, [object+offset]".
    // Find the first target register, then gather only addresses loaded into
    // that same register from the object. This excludes unrelated count math.
    if (!std::regex_search(code[2], collectionTarget,
            std::regex(R"(lea (r\w+), \[)" + collectionBase + R"(\+0x[0-9a-f]+\];)"))) return false;
    std::regex collectionPattern("lea " + collectionTarget[1].str() + R"(, \[)" + collectionBase +
                                 R"(\+(0x[0-9a-f]+)\];)");
    std::vector<size_t> collections;
    for (auto it = std::sregex_iterator(code[2].begin(), code[2].end(), collectionPattern);
         it != std::sregex_iterator(); ++it) {
        size_t offset = std::stoull((*it)[1], nullptr, 0);
        if (collections.empty() || collections.back() != offset) collections.push_back(offset);
    }
    if (!bitmap || !window || !contextType || !factory || !parent || !firstCollectionSelector ||
        collections.size() != 2) return false;
    // Publish the layout only after every required member has been found and
    // the pointer fields are aligned, distinct where required, and bounded.
    for (size_t offset : {*bitmap, *window, *factory, *parent, collections[0], collections[1]}) {
        if (!offset || offset >= 0x10000 || offset % alignof(void*)) return false;
    }
    if (*factory == *parent || collections[0] == collections[1] || customPaint == *contextType) return false;
    layout = {*window, *bitmap, customPaint, *contextType, *factory, *parent,
              collections[0], collections[1], static_cast<unsigned int>(*firstCollectionSelector)};
    return true;
}

HRESULT GetTrayBitmapResultHook(void* gestureWindow, const void* bitmapData,
                               int desktopOffset, const RECT& bitmapRect,
                               void* thumbnails, IUnknown* adjacentDesktop) {
    DWORD incomingLastError = GetLastError();
    auto* windowBytes = static_cast<std::byte*>(gestureWindow);
    HWND destination = *reinterpret_cast<HWND*>(windowBytes + g_gestureLayout.window);
    wchar_t destinationClass[64]{};
    GetClassNameW(destination, destinationClass, ARRAYSIZE(destinationClass));
    if (wcscmp(destinationClass, L"VirtualDesktopGestureSwitcher") != 0) {
        SetLastError(incomingLastError);
        return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                            bitmapRect, thumbnails, adjacentDesktop);
    }
    HBITMAP bitmap = *reinterpret_cast<HBITMAP const*>(
        static_cast<const std::byte*>(bitmapData) + g_gestureLayout.bitmap);
    if (bitmap ||
        (thumbnails != windowBytes + g_gestureLayout.firstCollection &&
         thumbnails != windowBytes + g_gestureLayout.secondCollection) ||
        (g_gestureLayout.customPaint != SIZE_MAX &&
         *reinterpret_cast<const bool*>(windowBytes + g_gestureLayout.customPaint)) ||
        *reinterpret_cast<const int*>(windowBytes + g_gestureLayout.contextType) == 2) {
        SetLastError(incomingLastError);
        return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                            bitmapRect, thumbnails, adjacentDesktop);
    }

    auto* factory = *reinterpret_cast<void**>(windowBytes + g_gestureLayout.factory);
    IUnknown* parent = *reinterpret_cast<IUnknown**>(windowBytes + g_gestureLayout.parent);
    HWND taskbar = g_getTrayWindowForMonitor(MonitorFromWindow(destination, MONITOR_DEFAULTTONEAREST));
    IUnknown* taskbarThumbnail = nullptr;
    RECT destinationRect{};
    HRESULT replacementResult = E_NOTIMPL;
    if (factory && parent && taskbar && IsWindow(destination) && GetWindowRect(taskbar, &destinationRect)) {
        RECT sourceRect{0, 0, destinationRect.right - destinationRect.left,
                       destinationRect.bottom - destinationRect.top};
        if (sourceRect.right > 0 && sourceRect.bottom > 0) {
            SetLastError(ERROR_SUCCESS);
            int mapped = MapWindowPoints(nullptr, destination,
                                         reinterpret_cast<POINT*>(&destinationRect), 2);
            if (mapped != 0 || GetLastError() == ERROR_SUCCESS) {
                const GUID thumbnailInterface{0x47267895, 0xA4F8, 0x4012,
                                              {0x9E, 0x98, 0x08, 0x7C, 0x61, 0x9E, 0x7C, 0xFC}};
                replacementResult = g_createWindowThumbnail(
                    factory, parent, taskbar, 0, nullptr, &sourceRect, thumbnailInterface,
                    reinterpret_cast<void**>(&taskbarThumbnail));
                if (SUCCEEDED(replacementResult) && taskbarThumbnail) {
                    destinationRect.left += desktopOffset;
                    destinationRect.right += desktopOffset;
                    replacementResult = g_setThumbnailRect(taskbarThumbnail, destinationRect, 0, nullptr);
                    if (SUCCEEDED(replacementResult)) {
                        // The native method selects the first collection on equality
                        // and the second otherwise. Match the callback's actual array.
                        unsigned int collectionSelector = g_gestureLayout.firstCollectionSelector;
                        if (thumbnails == windowBytes + g_gestureLayout.secondCollection) ++collectionSelector;
                        replacementResult = g_addThumbnailToCollection(
                            gestureWindow, collectionSelector, taskbarThumbnail);
                    }
                } else if (SUCCEEDED(replacementResult)) {
                    replacementResult = E_UNEXPECTED;
                }
            } else {
                replacementResult = HRESULT_FROM_WIN32(GetLastError());
            }
        }
    }
    // The native append retains its own reference, matching normal shell callers.
    if (taskbarThumbnail) taskbarThumbnail->Release();
    if (SUCCEEDED(replacementResult)) {
        Wh_Log(L"VDF REPLACED taskbar=%p collection=0x%zx offset=%d rect=(%ld,%ld,%ld,%ld)", taskbar,
               thumbnails == windowBytes + g_gestureLayout.firstCollection ?
                   g_gestureLayout.firstCollection : g_gestureLayout.secondCollection, desktopOffset,
               destinationRect.left, destinationRect.top, destinationRect.right, destinationRect.bottom);
        SetLastError(incomingLastError);
        return S_OK;
    }
    Wh_Log(L"VDF FALLBACK hr=0x%08lX", static_cast<unsigned long>(replacementResult));
    SetLastError(incomingLastError);
    return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                        bitmapRect, thumbnails, adjacentDesktop);
}

BOOL Wh_ModInit() {
    HMODULE shell = GetModuleHandleW(L"twinui.pcshell.dll");
    HMODULE thumbnails = GetModuleHandleW(L"twinui.dll");
    if (!shell || !thumbnails) return FALSE;
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK shellSymbols[]{
        {{L"protected: long __cdecl VirtualDesktopGestureWindow::GetTrayBitmapResult(struct TrayBitmapForVDThumbnailData const &,int,struct Geometry::CRect const &,class CCoSimpleArray<class Microsoft::WRL::ComPtr<struct IDCompThumbnail>,4294967294,class CSimpleArrayStandardCompareHelper<class Microsoft::WRL::ComPtr<struct IDCompThumbnail> > > &,struct IDCompThumbnail *)",
          L"private: long __cdecl VirtualDesktopGestureWindow::GetTrayBitmapResult(struct TrayBitmapForVDThumbnailData const &,int,struct Geometry::CRect const &,class CCoSimpleArray<class Microsoft::WRL::ComPtr<struct IDCompThumbnail>,4294967294,class CSimpleArrayStandardCompareHelper<class Microsoft::WRL::ComPtr<struct IDCompThumbnail> > > &,struct IDCompThumbnail *)"}, &g_getTrayBitmapResultOriginal},
        {{L"public: long __cdecl VirtualDesktopGestureWindow::Initialize(unsigned long)"}, &g_initializeGestureWindow},
        {{L"public: long __cdecl VirtualDesktopGestureWindow::AddThumbnailToThumbnailCollection(unsigned int,struct IDCompThumbnail *)"}, &g_addThumbnailToCollection},
        {{L"struct HWND__ * __cdecl TaskbarHelpers::GetTrayWindowForMonitor(struct HMONITOR__ *)"}, &g_getTrayWindowForMonitor},
    };
    // twinui.dll
    WindhawkUtils::SYMBOL_HOOK thumbnailSymbols[]{
        {{L"public: virtual long __cdecl CSwitchThumbnailFactory::CreateThumbnail(struct IDCompThumbnail *,struct HWND__ *,unsigned long,struct tagSIZE const *,struct tagRECT *,struct _GUID const &,void * *)"}, &g_createWindowThumbnail},
        {{L"protected: virtual long __cdecl CDCompAbstractThumbnail::SetRect(struct tagRECT const &,enum DCompThumbnailAnimation,struct IUIAnimationStoryboardEventHandler2 *)"}, &g_setThumbnailRect},
    };
    if (!WindhawkUtils::HookSymbols(shell, shellSymbols, ARRAYSIZE(shellSymbols)) ||
        !WindhawkUtils::HookSymbols(thumbnails, thumbnailSymbols, ARRAYSIZE(thumbnailSymbols))) return FALSE;
    try {
        if (!DiscoverGestureLayout(reinterpret_cast<void*>(g_getTrayBitmapResultOriginal),
                reinterpret_cast<void*>(g_initializeGestureWindow),
                reinterpret_cast<void*>(g_addThumbnailToCollection), g_gestureLayout)) {
            Wh_Log(L"VDF unsupported: native layout could not be recognized");
            return FALSE;
        }
    } catch (const std::exception&) {
        Wh_Log(L"VDF unsupported: layout discovery failed");
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(g_getTrayBitmapResultOriginal,
            GetTrayBitmapResultHook, &g_getTrayBitmapResultOriginal)) return FALSE;
    Wh_Log(L"VDF READY offsets window=0x%zx bitmap=0x%zx customPaint=0x%zx type=0x%zx factory=0x%zx parent=0x%zx collections=(0x%zx,0x%zx) firstSelector=%u",
           g_gestureLayout.window, g_gestureLayout.bitmap, g_gestureLayout.customPaint,
           g_gestureLayout.contextType, g_gestureLayout.factory, g_gestureLayout.parent,
           g_gestureLayout.firstCollection, g_gestureLayout.secondCollection, g_gestureLayout.firstCollectionSelector);
    return TRUE;
}
