// ==WindhawkMod==
// @id              declutter-alt-tab
// @name            Declutter Alt+Tab
// @description     Hides windows inside Snap Groups from Alt+Tab
// @version         1.0
// @author          Arthur Cuesta
// @github          https://github.com/cuesta4
// @homepage        https://github.com/cuesta4/declutter-alt-tab
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Declutter Alt+Tab
Windows 11 lists a Snap Group in Alt+Tab, but it also keeps listing every
window inside it. Picking one of those windows brings only that window forward
and breaks the group flow.

With this mod, windows inside a Snap Group are not listed in Alt+Tab; only the
group is. Once the group is broken, its windows are listed again. The taskbar
and Task View (Win+Tab) are not affected.

Tested on Windows 11 build 26200.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <inspectable.h>
#include <roapi.h>
#include <winstring.h>

#include <atomic>

namespace {

// Alt+Tab evaluates its window filter while the view host is being created;
// a focused window may be added shortly after, on the same thread.
constexpr ULONGLONG kAltTabScopeMs = 200;

std::atomic<DWORD> g_altTabThread;
std::atomic<DWORD> g_lastAltTabThread;
std::atomic<ULONGLONG> g_altTabTick;

// A group's visibility is computed from its members' IsViewVisible results,
// so members must not be filtered while that runs.
thread_local int g_groupVisibilityDepth;

void* g_win32ViewVtable;
void* g_winrtViewVtable;
using GetNativeWindow_t = HRESULT(WINAPI*)(void* view, HWND* window);
GetNativeWindow_t g_win32GetNativeWindow;
GetNativeWindow_t g_winrtGetNativeWindow;

// Windows.Internal.Shell.TaskGroups ABI, taken from the C++/WinRT consume
// wrappers in twinui.pcshell.dll (Current: slot 6, TryFindGroupWithWindow:
// slot 9, WindowId is a 32-bit window handle).
constexpr IID kIID_ITaskGroupDataModelStatics = {
    0xB9215ACB, 0x5441, 0x5728, {0x93, 0x03, 0xED, 0xA3, 0xF4, 0x96, 0x83, 0xE6}};

struct ITaskGroupDataModel : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Slot6() = 0;
    virtual HRESULT STDMETHODCALLTYPE Slot7() = 0;
    virtual HRESULT STDMETHODCALLTYPE Slot8() = 0;
    virtual HRESULT STDMETHODCALLTYPE TryFindGroupWithWindow(UINT32 windowId, IInspectable** group) = 0;
};

struct ITaskGroupDataModelStatics : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Current(ITaskGroupDataModel** model) = 0;
};

bool IsInSnapGroup(HWND window) {
    static constexpr wchar_t kClass[] = L"Windows.Internal.Shell.TaskGroups.TaskGroupDataModel";
    HSTRING_HEADER header;
    HSTRING className;
    ITaskGroupDataModelStatics* statics = nullptr;
    if (FAILED(WindowsCreateStringReference(kClass, ARRAYSIZE(kClass) - 1, &header, &className)) ||
        FAILED(RoGetActivationFactory(className, kIID_ITaskGroupDataModelStatics,
                                      reinterpret_cast<void**>(&statics)))) {
        return false;
    }
    ITaskGroupDataModel* model = nullptr;
    IInspectable* group = nullptr;
    if (SUCCEEDED(statics->get_Current(&model)) && model) {
        model->TryFindGroupWithWindow(static_cast<UINT32>(reinterpret_cast<UINT_PTR>(window)), &group);
        model->Release();
    }
    statics->Release();
    if (!group) {
        return false;
    }
    group->Release();
    return true;
}

HWND GetViewWindow(void* view) {
    HWND window = nullptr;
    void* vtable = *static_cast<void**>(view);
    GetNativeWindow_t getter = vtable == g_win32ViewVtable   ? g_win32GetNativeWindow
                               : vtable == g_winrtViewVtable ? g_winrtGetNativeWindow
                                                             : nullptr;
    return getter && SUCCEEDED(getter(view, &window)) ? window : nullptr;
}

bool InAltTabScope() {
    const DWORD thread = GetCurrentThreadId();
    return g_altTabThread == thread ||
           (g_lastAltTabThread == thread && GetTickCount64() - g_altTabTick <= kAltTabScopeMs);
}

using IsViewVisible_t = HRESULT(WINAPI*)(void* self, void* view, BOOL* visible);
IsViewVisible_t IsViewVisible_Original;
HRESULT WINAPI IsViewVisible_Hook(void* self, void* view, BOOL* visible) {
    const HRESULT hr = IsViewVisible_Original(self, view, visible);
    if (FAILED(hr) || !*visible || g_groupVisibilityDepth || !InAltTabScope()) {
        return hr;
    }
    if (HWND window = GetViewWindow(view); window && IsInSnapGroup(window)) {
        Wh_Log(L"Hiding grouped window %p from Alt+Tab", window);
        *visible = FALSE;
    }
    return hr;
}

using IsGroupVisibleOnDesktop_t = bool(WINAPI*)(void* self, void* group, void* desktop);
IsGroupVisibleOnDesktop_t IsGroupVisibleOnDesktop_Original;
bool WINAPI IsGroupVisibleOnDesktop_Hook(void* self, void* group, void* desktop) {
    ++g_groupVisibilityDepth;
    const bool result = IsGroupVisibleOnDesktop_Original(self, group, desktop);
    --g_groupVisibilityDepth;
    return result;
}

using XamlAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* args, const GUID& iid, void** result);
XamlAltTabViewHost_CreateInstance_t XamlAltTabViewHost_CreateInstance_Original;
HRESULT WINAPI XamlAltTabViewHost_CreateInstance_Hook(void* args, const GUID& iid, void** result) {
    const DWORD thread = GetCurrentThreadId();
    g_altTabThread = thread;
    g_lastAltTabThread = thread;
    g_altTabTick = GetTickCount64();
    const HRESULT hr = XamlAltTabViewHost_CreateInstance_Original(args, iid, result);
    g_altTabThread = 0;
    return hr;
}

}  // namespace

BOOL Wh_ModInit() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"twinui.pcshell.dll unavailable");
        return FALSE;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
         &IsViewVisible_Original,
         IsViewVisible_Hook},
        {{LR"(private: bool __cdecl winrt::Windows::Internal::Shell::DesktopTaskGroups::implementation::DesktopTaskGroupsSwitchItemController::IsGroupVisibleOnDesktop(struct winrt::Windows::Internal::Shell::TaskGroups::TaskGroup const &,struct IVirtualDesktop *))"},
         &IsGroupVisibleOnDesktop_Original,
         IsGroupVisibleOnDesktop_Hook},
        {{LR"(long __cdecl XamlAltTabViewHost_CreateInstance(struct XamlViewHostInitializeArgs const &,struct _GUID const &,void * *))"},
         &XamlAltTabViewHost_CreateInstance_Original,
         XamlAltTabViewHost_CreateInstance_Hook},
        {{LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"}, &g_win32ViewVtable},
        {{LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
         &g_win32GetNativeWindow},
        {{LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"}, &g_winrtViewVtable},
        {{LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
         &g_winrtGetNativeWindow},
    };
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }
    return TRUE;
}
