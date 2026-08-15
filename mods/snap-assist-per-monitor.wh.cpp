// ==WindhawkMod==
// @id              snap-assist-per-monitor
// @name            Snap Assist per monitor
// @description     Snap Assist suggests windows from every display. This shows only the windows on the monitor you snapped on.
// @version         1.0
// @author          zunxo7
// @github          https://github.com/zunxo7
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snap Assist per monitor

When you snap a window, Windows offers to fill the remaining space with one of
your other windows. On a multi-monitor setup that list includes windows from
**every** display — snap something on your laptop and it offers windows sitting
on your other monitor, which is never what you want.

This mod filters that list down to the display you actually snapped on.

It pairs naturally with the *Alt+Tab per monitor* mod: same idea, applied to the
other place Windows ignores which screen you are working on.

## Settings

- **Windows to show**
  - *Windows from the monitor you snapped on* (default) — the display is taken
    from the window you just snapped, so it stays correct even if your mouse has
    already moved to another screen.
  - *Windows from the monitor where the cursor is* — follows the pointer instead.
  - *Windows from all monitors* — disables filtering, i.e. stock Windows
    behaviour, without having to disable the mod.

- **Filter duration (ms)** — how long after Snap Assist starts that filtering
  stays active. The default of 3000 is comfortable; only worth changing if your
  window list is built unusually late.

## Requirements

Windows 11. Earlier versions do not have the XAML Snap Assist host this hooks.

## How it works

Snap Assist and Alt+Tab share the same window-visibility filter inside
`twinui.pcshell.dll`: `CVirtualDesktop::IsViewVisible`. Windows asks it whether
each window should be listed, so hiding windows from other displays is a matter
of answering that question differently.

The catch is that this filter is also used by Win+Tab and the taskbar. Filtering
unconditionally would break them, so it is armed only while Snap Assist is
actually building its list — scoped to that thread, and for a bounded time.

The arming point matters. Snap Assist collects its candidates *before* any UI is
shown, so hooking the moment the window appears is too late and filters nothing.
Instead the mod hooks `CSnapAssistControllerBase::TriggerSnapAssist*`, which is
where Snap Assist begins. Those functions also receive the `IApplicationView` of
the window you just snapped, which is what makes the "monitor you snapped on"
option exact rather than a guess based on cursor position.

## Troubleshooting

If the list is still unfiltered, enable **Enable logging** in the mod's Advanced
settings, snap a window, and open the log:

- `Armed from ...` — Snap Assist was caught starting, as intended.
- `Hiding hwnd=...` — filtering is working.
- Only `Show on rect ...` and no `Armed from` — none of the trigger functions
  fired on your build; please report the log.
- Nothing at all — Snap Assist is not going through `twinui.pcshell.dll` on your
  build; please report your Windows version.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- windowsToShow: snapMonitor
  $name: Windows to show
  $description: Which windows Snap Assist should offer
  $options:
  - snapMonitor: Windows from the monitor you snapped on
  - cursorMonitor: Windows from the monitor where the cursor is
  - allMonitors: Windows from all monitors (no filtering)
- filterWindowMs: 3000
  $name: Filter duration (ms)
  $description: >-
    How long after Snap Assist starts that filtering stays active. Raise this if
    the list is populated late and unfiltered windows still appear.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>

enum class WindowsToShow {
    snapMonitor,
    cursorMonitor,
    allMonitors,
};

struct {
    WindowsToShow windowsToShow;
    int filterWindowMs;
} g_settings;

// CVirtualDesktop::IsViewVisible is shared with Alt+Tab, Win+Tab and the taskbar,
// so filtering is confined to the thread Snap Assist runs on, and only for a
// bounded period after it starts. Outside that, calls pass through untouched.
std::atomic<DWORD> g_snapThreadId;
std::atomic<ULONGLONG> g_snapTickCount;
std::atomic<HMONITOR> g_snapMonitor;

void* CWin32ApplicationView_vtable;
void* CWinRTApplicationView_vtable;

using CWin32ApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWin32ApplicationView_v_GetNativeWindow_t
    CWin32ApplicationView_v_GetNativeWindow;

using CWinRTApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWinRTApplicationView_v_GetNativeWindow_t
    CWinRTApplicationView_v_GetNativeWindow;

// An IApplicationView is either a classic Win32 window or a WinRT one, and the
// call to reach the underlying HWND differs. The vtable pointer says which.
HRESULT GetWindowHandleFromApplicationView(void* applicationView,
                                           HWND* windowHandle) {
    *windowHandle = nullptr;
    if (!applicationView) {
        return E_FAIL;
    }

    void* vtable = *(void**)applicationView;

    if (vtable == CWin32ApplicationView_vtable &&
        CWin32ApplicationView_v_GetNativeWindow) {
        return CWin32ApplicationView_v_GetNativeWindow(applicationView,
                                                       windowHandle);
    }

    if (vtable == CWinRTApplicationView_vtable &&
        CWinRTApplicationView_v_GetNativeWindow) {
        return CWinRTApplicationView_v_GetNativeWindow(applicationView,
                                                       windowHandle);
    }

    return E_FAIL;
}

// Arm the filter. Called as Snap Assist starts, with the view that was snapped.
void ArmFilter(void* snappedView, PCWSTR from) {
    HMONITOR monitor = nullptr;

    HWND snappedWindow = nullptr;
    if (snappedView &&
        SUCCEEDED(GetWindowHandleFromApplicationView(snappedView,
                                                     &snappedWindow)) &&
        snappedWindow) {
        monitor = MonitorFromWindow(snappedWindow, MONITOR_DEFAULTTONEAREST);
    }

    if (!monitor) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        }
    }

    g_snapMonitor = monitor;
    g_snapThreadId = GetCurrentThreadId();
    g_snapTickCount = GetTickCount64();

    Wh_Log(L"Armed from %s: snapped hwnd=%08X monitor=%p", from,
           (DWORD)(ULONG_PTR)snappedWindow, monitor);
}

HMONITOR GetTargetMonitor() {
    if (g_settings.windowsToShow == WindowsToShow::cursorMonitor) {
        POINT pt;
        if (!GetCursorPos(&pt)) {
            return nullptr;
        }
        return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }

    return g_snapMonitor.load();
}

bool ShouldFilterNow() {
    if (g_settings.windowsToShow == WindowsToShow::allMonitors) {
        return false;
    }

    if (g_snapThreadId.load() != GetCurrentThreadId()) {
        return false;
    }

    ULONGLONG elapsed = GetTickCount64() - g_snapTickCount.load();
    return elapsed <= (ULONGLONG)g_settings.filterWindowMs;
}

using CVirtualDesktop_IsViewVisible_t = HRESULT(WINAPI*)(void* pThis,
                                                         void* applicationView,
                                                         BOOL* isVisible);
CVirtualDesktop_IsViewVisible_t CVirtualDesktop_IsViewVisible_Original;
HRESULT WINAPI CVirtualDesktop_IsViewVisible_Hook(void* pThis,
                                                  void* applicationView,
                                                  BOOL* isVisible) {
    HRESULT ret = CVirtualDesktop_IsViewVisible_Original(pThis, applicationView,
                                                         isVisible);
    if (FAILED(ret) || !isVisible || !*isVisible) {
        return ret;
    }

    if (!ShouldFilterNow()) {
        return ret;
    }

    HMONITOR target = GetTargetMonitor();
    if (!target) {
        return ret;
    }

    HWND windowHandle = nullptr;
    if (FAILED(GetWindowHandleFromApplicationView(applicationView,
                                                  &windowHandle)) ||
        !windowHandle) {
        return ret;
    }

    HMONITOR windowMonitor =
        MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);

    if (windowMonitor != target) {
        Wh_Log(L"Hiding hwnd=%08X (monitor %p, want %p)",
               (DWORD)(ULONG_PTR)windowHandle, windowMonitor, target);
        *isVisible = FALSE;
    }

    return ret;
}

// --- Snap Assist entry points -------------------------------------------------
// These are where Snap Assist begins, and both receive the view that was just
// snapped. Arming here rather than at Show matters: the candidate list is built
// before Show is called, so arming at Show is too late.

using TriggerFromArranged_t = HRESULT(WINAPI*)(void* pThis,
                                               void* applicationView,
                                               int snappingMode,
                                               int viewFlags);
TriggerFromArranged_t TriggerFromArranged_Original;
HRESULT WINAPI TriggerFromArranged_Hook(void* pThis,
                                        void* applicationView,
                                        int snappingMode,
                                        int viewFlags) {
    ArmFilter(applicationView, L"ArrangedNotification");
    return TriggerFromArranged_Original(pThis, applicationView, snappingMode,
                                        viewFlags);
}

using TriggerWithRegions_t = HRESULT(WINAPI*)(void* pThis,
                                              void* applicationView,
                                              int viewFlags,
                                              const RECT* regions,
                                              unsigned int regionCount);
TriggerWithRegions_t TriggerWithRegions_Original;
HRESULT WINAPI TriggerWithRegions_Hook(void* pThis,
                                       void* applicationView,
                                       int viewFlags,
                                       const RECT* regions,
                                       unsigned int regionCount) {
    ArmFilter(applicationView, L"WithRegions");
    return TriggerWithRegions_Original(pThis, applicationView, viewFlags,
                                       regions, regionCount);
}

using TriggerFromAppLayout_t = HRESULT(WINAPI*)(void* pThis,
                                                void* immersiveMonitor,
                                                void* appLayout);
TriggerFromAppLayout_t TriggerFromAppLayout_Original;
HRESULT WINAPI TriggerFromAppLayout_Hook(void* pThis,
                                         void* immersiveMonitor,
                                         void* appLayout) {
    // No application view here, so the monitor falls back to the cursor.
    ArmFilter(nullptr, L"AppLayout");
    return TriggerFromAppLayout_Original(pThis, immersiveMonitor, appLayout);
}

// Secondary arming point, and useful in the log for confirming Snap Assist is
// reaching the XAML host at all.
using XamlSnapAssistViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                       RECT* rect,
                                                       int flags);
XamlSnapAssistViewHost_Show_t XamlSnapAssistViewHost_Show_Original;
HRESULT WINAPI XamlSnapAssistViewHost_Show_Hook(void* pThis,
                                                RECT* rect,
                                                int flags) {
    if (rect) {
        Wh_Log(L"Show on rect (%d,%d)-(%d,%d)", rect->left, rect->top,
               rect->right, rect->bottom);

        // Only trust this monitor if nothing armed us already: the rectangle is
        // where Snap Assist is being drawn, which is the display we want.
        if (!g_snapMonitor.load()) {
            g_snapMonitor = MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
        }
    } else {
        Wh_Log(L"Show (no rect)");
    }

    // Refresh the timer so late list-building is still covered.
    g_snapThreadId = GetCurrentThreadId();
    g_snapTickCount = GetTickCount64();

    return XamlSnapAssistViewHost_Show_Original(pThis, rect, flags);
}

void LoadSettings() {
    PCWSTR value = Wh_GetStringSetting(L"windowsToShow");

    g_settings.windowsToShow = WindowsToShow::snapMonitor;
    if (wcscmp(value, L"cursorMonitor") == 0) {
        g_settings.windowsToShow = WindowsToShow::cursorMonitor;
    } else if (wcscmp(value, L"allMonitors") == 0) {
        g_settings.windowsToShow = WindowsToShow::allMonitors;
    }

    Wh_FreeStringSetting(value);

    g_settings.filterWindowMs = Wh_GetIntSetting(L"filterWindowMs");
    if (g_settings.filterWindowMs <= 0) {
        g_settings.filterWindowMs = 3000;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE twinuiPcshellModule = LoadLibrary(L"twinui.pcshell.dll");
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
        // The shared window filter. Also used by Alt+Tab, Win+Tab and the
        // taskbar, which is why filtering is scoped by thread and time.
        {
            {LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
            &CVirtualDesktop_IsViewVisible_Original,
            CVirtualDesktop_IsViewVisible_Hook,
        },

        // Resolving an IApplicationView to its HWND.
        {
            {LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"},
            &CWin32ApplicationView_vtable,
        },
        {
            {LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
            &CWin32ApplicationView_v_GetNativeWindow,
        },
        {
            {LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"},
            &CWinRTApplicationView_vtable,
        },
        {
            {LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
            &CWinRTApplicationView_v_GetNativeWindow,
        },

        // Snap Assist entry points. Marked optional individually because which
        // one fires depends on how the snap was performed; at least one will.
        {
            {LR"(public: virtual long __cdecl CSnapAssistControllerBase::TriggerSnapAssistFromApplicationArrangedNotification(struct IApplicationView *,enum WINDOW_SNAPPING_MODE,enum SNAP_ASSIST_VIEW_FLAGS))"},
            &TriggerFromArranged_Original,
            TriggerFromArranged_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CSnapAssistControllerBase::TriggerSnapAssistWithRegions(struct IApplicationView *,enum SNAP_ASSIST_VIEW_FLAGS,struct tagRECT const *,unsigned int))"},
            &TriggerWithRegions_Original,
            TriggerWithRegions_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CSnapAssistControllerBase::TriggerSnapAssistFromAppLayout(struct IImmersiveMonitor *,struct IAppLayout *))"},
            &TriggerFromAppLayout_Original,
            TriggerFromAppLayout_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl XamlSnapAssistViewHost::Show(struct tagRECT *,enum SNAP_ASSIST_VIEW_FLAGS))"},
            &XamlSnapAssistViewHost_Show_Original,
            XamlSnapAssistViewHost_Show_Hook,
            true,
        },
    };

    if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                     ARRAYSIZE(twinuiPcshellSymbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}
