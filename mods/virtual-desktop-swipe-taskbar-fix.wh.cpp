// ==WindhawkMod==
// @id              virtual-desktop-swipe-taskbar-fix
// @name            Virtual Desktop Swipe Taskbar Fix
// @description     Restores taskbar presence in virtual desktop swipe previews.
// @version         1.0.4
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
thumbnails taken at that time inevitably look the same as on the outgoing
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
void* g_switchThumbnailFactoryVftable;
void* g_createCustomPaintThumbnail;

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

bool DiscoverGestureLayout(void* bitmapResult, void* initialize, void* addThumbnail,
                           GestureLayout& layout) {
    std::string code[3];
    void* functions[]{bitmapResult, initialize, addThumbnail};
    const wchar_t* methodNames[]{L"GetTrayBitmapResult", L"Initialize", L"AddThumbnailToThumbnailCollection"};
    const int limits[]{220, 64, 180};
    for (size_t index = 0; index < ARRAYSIZE(functions); ++index) {
        auto* instruction = static_cast<std::byte*>(functions[index]);
        for (int count = 0; count < limits[index]; ++count) {
            WH_DISASM_RESULT decoded{};
            if (!Wh_Disasm(instruction, &decoded) || !decoded.length || decoded.length > 15) {
                Wh_Log(L"Failed to disassemble %s at %p", methodNames[index], instruction);
                return false;
            }
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
        std::smatch match;
        if (!std::regex_search(instructions, match, std::regex(pattern))) return std::nullopt;
        size_t offset = std::stoull(match[group], nullptr, 0);
        if (offset >= 0x10000) return std::nullopt;
        return offset;
    };

    std::smatch self;
    // Example: mov rsi, rcx. rsi holds the gesture object in subsequent accesses.
    if (!std::regex_search(code[0], self, std::regex(R"(mov (r\w+), rcx;)"))) {
        Wh_Log(L"Gesture object register not found");
        return false;
    }
    const std::string owner = self[1];
    const std::string ownerMember = R"(\[)" + owner + R"(\+(0x[0-9a-f]+)\])";

    // Example: mov rcx, [rdx+0x10]; xor ebx, ebx; test rcx, rcx.
    // The bitmap load and null check must use the same register.
    auto bitmap = offsetFrom(code[0],
        R"(mov (r\w+), (?:qword ptr )?\[rdx\+(0x[0-9a-f]+)\];(?:xor \w+, \w+;)?test \1, \1;)", 2);
    // Example: cmp [rsi+0x08], rbx. This checks the destination HWND for null.
    auto window = offsetFrom(code[0], R"(cmp (?:qword ptr )?)" + ownerMember + R"(, \w+;)");
    // Example: mov [rcx+0xE4], edx. Initialize stores the context-type argument.
    auto contextType = offsetFrom(code[1], R"(mov (?:dword ptr )?\[rcx\+(0x[0-9a-f]+)\], edx;)");
    if (!bitmap) {
        Wh_Log(L"Bitmap handle offset not found");
        return false;
    }
    if (!window) {
        Wh_Log(L"Gesture window offset not found");
        return false;
    }
    if (!contextType) {
        Wh_Log(L"Context-type offset not found");
        return false;
    }

    // Example: mov rdx, [rsi+0xA0]; mov rcx, rdi; mov rax, [rbx+0x48]; call ...
    // These are the parent, factory, and method used to create the bitmap layer.
    // Verify the method against its symbol before accepting the member offsets.
    std::optional<size_t> factory;
    std::optional<size_t> parent;
    std::regex creationCall(R"(mov rdx, (?:qword ptr )?)" + ownerMember +
        R"(;mov rcx, (r\w+);mov rax, ([^;]+);call (?:0x[0-9a-f]+|\[0x[0-9a-f]+\]);)");
    for (auto call = std::sregex_iterator(code[0].begin(), code[0].end(), creationCall);
         call != std::sregex_iterator(); ++call) {
        std::string beforeCall = code[0].substr(0, call->position());
        std::string targetOperand = (*call)[3];
        std::smatch slotLoad;
        if (!std::regex_match(targetOperand, slotLoad,
                std::regex(R"((?:qword ptr )?\[(r\w+)\+(0x[0-9a-f]+)\])"))) {
            if (!std::regex_match(targetOperand, std::regex(R"(r\w+)"))) continue;
            // Older builds load the method earlier, then use mov rax, rbx here.
            std::regex savedMethodLoad("mov " + targetOperand + R"(, ((?:qword ptr )?\[r\w+\+0x[0-9a-f]+\]);)");
            std::optional<size_t> methodPosition;
            for (auto load = std::sregex_iterator(beforeCall.begin(), beforeCall.end(), savedMethodLoad);
                 load != std::sregex_iterator(); ++load) {
                methodPosition = load->position();
                targetOperand = (*load)[1];
            }
            if (!methodPosition || !std::regex_match(targetOperand, slotLoad,
                    std::regex(R"((?:qword ptr )?\[(r\w+)\+(0x[0-9a-f]+)\])"))) continue;
            beforeCall.resize(*methodPosition);
        }
        size_t slotOffset = std::stoull(slotLoad[2], nullptr, 0);
        void* slotMethod = nullptr;
        if (slotOffset >= 0x10000 || slotOffset % alignof(void*) ||
            !ReadProcessMemory(GetCurrentProcess(),
                static_cast<const std::byte*>(g_switchThumbnailFactoryVftable) + slotOffset,
                &slotMethod, sizeof(slotMethod), nullptr) ||
            slotMethod != g_createCustomPaintThumbnail) continue;

        // Example: mov rdi, [rsi+0x38]; mov rbx, [rdi].
        // Trace the call's vtable back to the factory member.
        std::regex vtableLoad("mov " + slotLoad[1].str() + R"(, (?:qword ptr )?\[)" + (*call)[2].str() + R"(\];)");
        std::optional<size_t> vtablePosition;
        for (auto load = std::sregex_iterator(beforeCall.begin(), beforeCall.end(), vtableLoad);
             load != std::sregex_iterator(); ++load) vtablePosition = load->position();
        if (!vtablePosition) continue;
        const std::string beforeVtable = beforeCall.substr(0, *vtablePosition);
        std::regex factoryLoad("mov " + (*call)[2].str() + R"(, (?:qword ptr )?)" + ownerMember + ';');
        std::optional<size_t> factoryOffset;
        for (auto load = std::sregex_iterator(beforeVtable.begin(), beforeVtable.end(), factoryLoad);
             load != std::sregex_iterator(); ++load) factoryOffset = std::stoull((*load)[1], nullptr, 0);
        if (!factoryOffset) continue;
        size_t parentOffset = std::stoull((*call)[1], nullptr, 0);
        if ((factory && *factory != *factoryOffset) || (parent && *parent != parentOffset)) {
            Wh_Log(L"Conflicting factory or parent offsets in thumbnail creation calls");
            return false;
        }
        factory = factoryOffset;
        parent = parentOffset;
    }
    if (!factory || !parent) {
        Wh_Log(L"Thumbnail creation call or factory member load not recognized");
        return false;
    }
    size_t customPaint = SIZE_MAX;
    std::smatch flag;
    // Example: lea r13, [rsi+0xE0]; ...; cmp [r13], bl.
    // The flag address must be followed by a byte comparison through it.
    if (std::regex_search(code[0], flag,
            std::regex(R"(lea (r\w+), \[)" + owner + R"(\+(0x[0-9a-f]+)\];)"))) {
        if (!std::regex_search(code[0],
                std::regex(R"(cmp (?:byte ptr )?\[)" + flag[1].str() + R"(\], (?:[abcd]l|r\d+b);)"))) {
            Wh_Log(L"Custom-paint flag comparison not found");
            return false;
        }
        customPaint = std::stoull(flag[2], nullptr, 0);
        if (customPaint >= 0x10000) {
            Wh_Log(L"Invalid custom-paint offset: 0x%zx", customPaint);
            return false;
        }
    } else {
        // Older builds have no custom-paint flag, only the context-type checks.
        if (std::regex_search(code[0], std::regex("byte ptr \\[" + owner))) {
            Wh_Log(L"Unrecognized custom-paint flag access");
            return false;
        }
        char typeOffset[32]{};
        snprintf(typeOffset, sizeof(typeOffset), "%zx", *contextType);
        std::regex typeBranch("cmp dword ptr \\[" + owner + "\\+0x0*" +
                              typeOffset + R"(\], 0x0*2;)");
        // Example: cmp dword ptr [r14+0xE0], 0x02. Require both mode checks.
        auto branches = std::distance(std::sregex_iterator(code[0].begin(), code[0].end(), typeBranch), std::sregex_iterator());
        if (branches != 2) {
            Wh_Log(L"Expected two context-type checks, found %td", branches);
            return false;
        }
    }

    // Windows owns both thumbnail arrays. Either can receive the adjacent desktop.
    std::smatch collectionOwner;
    std::smatch selectorRegister;
    // Example: mov rsi, rcx; mov ebx, edx. Some builds keep rcx and edx instead.
    const std::string collectionBase = std::regex_search(code[2], collectionOwner,
        std::regex(R"(mov (r\w+), rcx;)")) ? collectionOwner[1].str() : "rcx";
    const std::string selector = std::regex_search(code[2], selectorRegister,
        std::regex(R"(mov (e\w+), edx;)")) ? selectorRegister[1].str() : "edx";
    // Example: cmp ebx, 3; jnz ... . Equality selects the first array.
    auto firstCollectionSelector = offsetFrom(code[2], "cmp " + selector +
        R"(, (0x[0-9a-f]+|[0-9]+);(?:jne|jnz) )");
    if (!firstCollectionSelector) {
        Wh_Log(L"Thumbnail collection selector not found");
        return false;
    }
    std::smatch collectionTarget;
    // Example: lea rcx, [rsi+0x40] / lea rcx, [rsi+0x60].
    // Both branches use the same destination register for the array address.
    if (!std::regex_search(code[2], collectionTarget,
            std::regex(R"(lea (r\w+), \[)" + collectionBase + R"(\+0x[0-9a-f]+\];)"))) {
        Wh_Log(L"Thumbnail collection address calculation not found");
        return false;
    }
    std::regex collectionPattern("lea " + collectionTarget[1].str() + R"(, \[)" + collectionBase +
                                 R"(\+(0x[0-9a-f]+)\];)");
    std::vector<size_t> collections;
    for (auto it = std::sregex_iterator(code[2].begin(), code[2].end(), collectionPattern);
         it != std::sregex_iterator(); ++it) {
        size_t offset = std::stoull((*it)[1], nullptr, 0);
        if (collections.empty() || collections.back() != offset) collections.push_back(offset);
    }
    if (collections.size() != 2) {
        Wh_Log(L"Expected two thumbnail collection offsets, found %zu", collections.size());
        return false;
    }
    for (size_t offset : {*bitmap, *window, *factory, *parent, collections[0], collections[1]}) {
        if (!offset || offset >= 0x10000 || offset % alignof(void*)) {
            Wh_Log(L"Invalid pointer member offset: 0x%zx", offset);
            return false;
        }
    }
    if (*factory == *parent || collections[0] == collections[1] || customPaint == *contextType) {
        Wh_Log(L"Discovered member offsets overlap");
        return false;
    }
    layout = {*window, *bitmap, customPaint, *contextType, *factory, *parent,
              collections[0], collections[1], static_cast<unsigned int>(*firstCollectionSelector)};
    return true;
}

HRESULT GetTrayBitmapResultHook(void* gestureWindow, const void* bitmapData,
                               int desktopOffset, const RECT& bitmapRect,
                               void* thumbnails, IUnknown* adjacentDesktop) {
    auto* windowBytes = static_cast<std::byte*>(gestureWindow);
    HWND destination = *reinterpret_cast<HWND*>(windowBytes + g_gestureLayout.window);
    wchar_t destinationClass[64]{};
    GetClassNameW(destination, destinationClass, ARRAYSIZE(destinationClass));
    if (wcscmp(destinationClass, L"VirtualDesktopGestureSwitcher") != 0) {
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
        return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                            bitmapRect, thumbnails, adjacentDesktop);
    }

    auto* factory = *reinterpret_cast<void**>(windowBytes + g_gestureLayout.factory);
    IUnknown* parent = *reinterpret_cast<IUnknown**>(windowBytes + g_gestureLayout.parent);
    // Check the concrete factory type without dereferencing an invalid candidate.
    void* factoryVftable = nullptr;
    bool verifiedFactory = factory && ReadProcessMemory(GetCurrentProcess(), factory,
        &factoryVftable, sizeof(factoryVftable), nullptr) &&
        factoryVftable == g_switchThumbnailFactoryVftable;
    HWND taskbar = g_getTrayWindowForMonitor(MonitorFromWindow(destination, MONITOR_DEFAULTTONEAREST));
    IUnknown* taskbarThumbnail = nullptr;
    RECT destinationRect{};
    HRESULT replacementResult = E_NOTIMPL;
    if (verifiedFactory && parent && taskbar && IsWindow(destination) && GetWindowRect(taskbar, &destinationRect)) {
        RECT sourceRect{0, 0, destinationRect.right - destinationRect.left,
                       destinationRect.bottom - destinationRect.top};
        if (sourceRect.right > 0 && sourceRect.bottom > 0) {
            // MapWindowPoints can return zero on success. Check last error then.
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
                        // Append to the callback's array, regardless of swipe direction.
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
    // The native append AddRefs the thumbnail. Release our factory reference.
    if (taskbarThumbnail) taskbarThumbnail->Release();
    if (SUCCEEDED(replacementResult)) {
        Wh_Log(L"Replaced taskbar=%p collection=0x%zx offset=%d rect=(%ld,%ld,%ld,%ld)", taskbar,
               thumbnails == windowBytes + g_gestureLayout.firstCollection ?
                   g_gestureLayout.firstCollection : g_gestureLayout.secondCollection, desktopOffset,
               destinationRect.left, destinationRect.top, destinationRect.right, destinationRect.bottom);
        return S_OK;
    }
    Wh_Log(L"Thumbnail replacement failed: 0x%08lX", static_cast<unsigned long>(replacementResult));
    return g_getTrayBitmapResultOriginal(gestureWindow, bitmapData, desktopOffset,
                                        bitmapRect, thumbnails, adjacentDesktop);
}

BOOL Wh_ModInit() {
    HMODULE shell = GetModuleHandleW(L"twinui.pcshell.dll");
    HMODULE thumbnails = GetModuleHandleW(L"twinui.dll");
    // Load missing shell modules during early Explorer startup.
    // Keep the acquired references for the process lifetime; reuse on mod reload.
    if (!shell) shell = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!thumbnails) thumbnails = LoadLibraryExW(L"twinui.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!shell || !thumbnails) {
        Wh_Log(L"Shell modules unavailable: twinui.pcshell=%p twinui=%p", shell, thumbnails);
        return FALSE;
    }
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
        {{L"const CSwitchThumbnailFactory::`vftable'"}, &g_switchThumbnailFactoryVftable},
        {{L"public: virtual long __cdecl CSwitchThumbnailFactory::CreateCustomPaintThumbnail(struct IDCompThumbnail *,struct tagSIZE,enum CustomPaintThumbnailFlags,struct _GUID const &,void * *)"}, &g_createCustomPaintThumbnail},
        {{L"public: virtual long __cdecl CSwitchThumbnailFactory::CreateThumbnail(struct IDCompThumbnail *,struct HWND__ *,unsigned long,struct tagSIZE const *,struct tagRECT *,struct _GUID const &,void * *)"}, &g_createWindowThumbnail},
        {{L"protected: virtual long __cdecl CDCompAbstractThumbnail::SetRect(struct tagRECT const &,enum DCompThumbnailAnimation,struct IUIAnimationStoryboardEventHandler2 *)"}, &g_setThumbnailRect},
    };
    if (!WindhawkUtils::HookSymbols(shell, shellSymbols, ARRAYSIZE(shellSymbols)) ||
        !WindhawkUtils::HookSymbols(thumbnails, thumbnailSymbols, ARRAYSIZE(thumbnailSymbols))) return FALSE;
    try {
        if (!DiscoverGestureLayout(reinterpret_cast<void*>(g_getTrayBitmapResultOriginal),
                reinterpret_cast<void*>(g_initializeGestureWindow),
                reinterpret_cast<void*>(g_addThumbnailToCollection), g_gestureLayout)) {
            return FALSE;
        }
    } catch (const std::exception& exception) {
        Wh_Log(L"Layout discovery failed: %S", exception.what());
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(g_getTrayBitmapResultOriginal,
            GetTrayBitmapResultHook, &g_getTrayBitmapResultOriginal)) return FALSE;
    Wh_Log(L"Initialized: window=0x%zx bitmap=0x%zx customPaint=0x%zx type=0x%zx factory=0x%zx parent=0x%zx collections=(0x%zx,0x%zx) firstSelector=%u",
           g_gestureLayout.window, g_gestureLayout.bitmap, g_gestureLayout.customPaint,
           g_gestureLayout.contextType, g_gestureLayout.factory, g_gestureLayout.parent,
           g_gestureLayout.firstCollection, g_gestureLayout.secondCollection, g_gestureLayout.firstCollectionSelector);
    return TRUE;
}
