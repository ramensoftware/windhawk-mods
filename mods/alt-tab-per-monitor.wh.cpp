// ==WindhawkMod==
// @id              alt-tab-per-monitor
// @name            Alt+Tab per monitor
// @description     Pressing Alt+Tab shows all open windows on the primary display. This mod shows only the windows on the monitor where the cursor is.
// @version         1.0.0
// @author          L3r0y
// @github          https://github.com/L3r0yThingz
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+Tab per monitor

When you press the Alt+Tab combination, the window switcher will appear on the
primary display, showing all open windows across all monitors. This mod
customizes the behavior to display the switcher on the monitor where the cursor
is currently located, showing only the windows present on that specific monitor.
*/
// ==/WindhawkModReadme==

#include <processthreadsapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <commctrl.h>
#include <windows.h>
#include <winerror.h>
#include <wininet.h>
#include <winnt.h>

std::atomic<DWORD> g_threadIdForAltTabShowWindow;

void HandleAltTabWindow(HWND hWnd) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monInfo;
    monInfo.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfo(hMon, &monInfo)) {
        return;
    }

    SetWindowPos(hWnd, nullptr, monInfo.rcWork.left, monInfo.rcWork.top,
                 monInfo.rcWork.right - monInfo.rcWork.left,
                 monInfo.rcWork.bottom - monInfo.rcWork.top,
                 SWP_NOACTIVATE | SWP_NOZORDER);
}

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

using CVirtualDesktop_IsViewVisible_t = HRESULT(WINAPI*)(void* pThis,
                                                         void* applicationView,
                                                         BOOL* isVisible);
CVirtualDesktop_IsViewVisible_t CVirtualDesktop_IsViewVisible_Original;
HRESULT WINAPI CVirtualDesktop_IsViewVisible_Hook(void* pThis,
                                                  void* applicationView,
                                                  BOOL* isVisible) {
    Wh_Log(L">");

    auto ret = CVirtualDesktop_IsViewVisible_Original(pThis, applicationView,
                                                      isVisible);
    if (FAILED(ret)) {
        return ret;
    }

    if (!*isVisible) {
        return ret;
    }

    void* vtable = *(void**)applicationView;

    HWND windowHandle = nullptr;
    HRESULT hr = E_FAIL;
    if (vtable == CWin32ApplicationView_vtable) {
        hr = CWin32ApplicationView_v_GetNativeWindow(applicationView,
                                                     &windowHandle);
    } else if (vtable == CWinRTApplicationView_vtable) {
        hr = CWinRTApplicationView_v_GetNativeWindow(applicationView,
                                                     &windowHandle);
    }
    if (FAILED(hr) || !windowHandle) {
        return ret;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return ret;
    }

    auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    auto hMonFromWindow =
        MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);

    if (hMon != hMonFromWindow) {
        *isVisible = FALSE;
    }

    return ret;
}

using XamlAltTabViewHost_DisplayAltTab_t = void(WINAPI*)(void* pThis);
XamlAltTabViewHost_DisplayAltTab_t XamlAltTabViewHost_DisplayAltTab_Original;
void XamlAltTabViewHost_DisplayAltTab_Hook(void* pThis) {
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    XamlAltTabViewHost_DisplayAltTab_Original(pThis);
    g_threadIdForAltTabShowWindow = 0;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_threadIdForAltTabShowWindow != GetCurrentThreadId()) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (nCmdShow != SW_HIDE) {
        HandleAltTabWindow(hWnd);
    }

    g_threadIdForAltTabShowWindow = 0;
    return ShowWindow_Original(hWnd, nCmdShow);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
        {
            {LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
            &CVirtualDesktop_IsViewVisible_Original,
            CVirtualDesktop_IsViewVisible_Hook,
        },
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
        {
            {LR"(private: void __cdecl XamlAltTabViewHost::DisplayAltTab(void))"},
            &XamlAltTabViewHost_DisplayAltTab_Original,
            XamlAltTabViewHost_DisplayAltTab_Hook,
        },

    };

    HMODULE twinuiPcshellModule = LoadLibrary(L"twinui.pcshell.dll");
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                     ARRAYSIZE(twinuiPcshellSymbolHooks))) {
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(ShowWindow, ShowWindow_Hook,
                                       &ShowWindow_Original);

    return TRUE;
}
