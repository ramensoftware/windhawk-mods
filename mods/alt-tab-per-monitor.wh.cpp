// ==WindhawkMod==
// @id              alt-tab-per-monitor
// @name            Alt+Tab per monitor
// @description     Pressing Alt+Tab shows all open windows on the primary display. This mod shows only the windows on the monitor where the cursor is.
// @version         1.0.0
// @author          L3r0y
// @github          https://github.com/L3r0yThingz
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32
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

#include <windhawk_utils.h>

#include <winrt/windows.foundation.collections.h>

std::atomic<DWORD> g_threadIdForAltTabShowWindow;
std::atomic<DWORD> g_lastThreadIdForXamlAltTabViewHost_CreateInstance;
std::atomic<DWORD> g_threadIdForXamlAltTabViewHost_CreateInstance;
ULONGLONG g_CreateInstance_TickCount;
constexpr ULONGLONG kDeltaThreshold = 200;

bool HandleAltTabWindow(winrt::Windows::Foundation::Rect* rect) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monInfo;
    monInfo.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfo(hMon, &monInfo)) {
        return false;
    }

    rect->Height = monInfo.rcWork.bottom - monInfo.rcWork.top;
    rect->Width = monInfo.rcWork.right - monInfo.rcWork.left;
    rect->X = monInfo.rcWork.left;
    rect->Y = monInfo.rcWork.top;

    return true;
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

HRESULT GetWindowHandleFromApplicationView(void* applicationView,
                                           HWND* windowHandle) {
    *windowHandle = nullptr;
    void* vtable = *(void**)applicationView;
    HRESULT hr = E_FAIL;

    if (vtable == CWin32ApplicationView_vtable) {
        hr = CWin32ApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    } else if (vtable == CWinRTApplicationView_vtable) {
        hr = CWinRTApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    }

    return hr;
}

bool IsWindowOnCursorMonitor(HWND windowHandle) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    auto hMonFromWindow =
        MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);

    return hMon == hMonFromWindow;
}

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

    if (g_threadIdForXamlAltTabViewHost_CreateInstance !=
        GetCurrentThreadId()) {
        // A focused window might be added after a short period. Filter windows
        // using our monitor rules if the alt tab window was just opened.
        // Otherwise, don't play with the filter anymore, as it's also used by
        // other components such as Win+Tab and the taskbar.
        if ((GetTickCount64() - g_CreateInstance_TickCount) > kDeltaThreshold ||
            g_lastThreadIdForXamlAltTabViewHost_CreateInstance !=
                GetCurrentThreadId()) {
            return ret;
        }
    }

    if (!*isVisible) {
        return ret;
    }

    HWND windowHandle;
    HRESULT hr =
        GetWindowHandleFromApplicationView(applicationView, &windowHandle);

    if (FAILED(hr) || !windowHandle) {
        return ret;
    }

    if (!IsWindowOnCursorMonitor(windowHandle)) {
        *isVisible = FALSE;
    }

    return ret;
}

using XamlAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                   void* param1,
                                                   int param2,
                                                   void* param3);
XamlAltTabViewHost_Show_t XamlAltTabViewHost_Show_Original;
HRESULT XamlAltTabViewHost_Show_Hook(void* pThis,
                                     void* param1,
                                     int param2,
                                     void* param3) {
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    HRESULT ret =
        XamlAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    g_threadIdForAltTabShowWindow = 0;
    return ret;
}

using ITaskGroupWindowInformation_Position_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect* rect);
ITaskGroupWindowInformation_Position_t
    ITaskGroupWindowInformation_Position_Original;
HRESULT ITaskGroupWindowInformation_Position_Hook(
    void* pThis,
    winrt::Windows::Foundation::Rect* rect) {
    if (g_threadIdForAltTabShowWindow != GetCurrentThreadId()) {
        return ITaskGroupWindowInformation_Position_Original(pThis, rect);
    }
    g_threadIdForAltTabShowWindow = 0;

    winrt::Windows::Foundation::Rect newRect;
    if (!HandleAltTabWindow(&newRect)) {
        return ITaskGroupWindowInformation_Position_Original(pThis, rect);
    }

    HRESULT ret =
        ITaskGroupWindowInformation_Position_Original(pThis, &newRect);

    return ret;
}

using XamlAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* pThis,
                                                             void* param1,
                                                             void* param2,
                                                             void* param3);
XamlAltTabViewHost_CreateInstance_t XamlAltTabViewHost_CreateInstance_Original;
HRESULT XamlAltTabViewHost_CreateInstance_Hook(void* pThis,
                                               void* param1,
                                               void* param2,
                                               void* param3) {
    g_threadIdForXamlAltTabViewHost_CreateInstance = GetCurrentThreadId();
    g_lastThreadIdForXamlAltTabViewHost_CreateInstance = GetCurrentThreadId();
    g_CreateInstance_TickCount = GetTickCount64();
    HRESULT ret = XamlAltTabViewHost_CreateInstance_Original(pThis, param1,
                                                             param2, param3);
    g_threadIdForXamlAltTabViewHost_CreateInstance = 0;
    return ret;
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
            {LR"(public: virtual long __cdecl XamlAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
            &XamlAltTabViewHost_Show_Original,
            XamlAltTabViewHost_Show_Hook,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_Internal_Shell_TaskGroups_ITaskGroupWindowInformation<struct winrt::Windows::Internal::Shell::TaskGroups::ITaskGroupWindowInformation>::Position(struct winrt::Windows::Foundation::Rect const &)const )"},
            &ITaskGroupWindowInformation_Position_Original,
            ITaskGroupWindowInformation_Position_Hook,
        },
        {
            {LR"(long __cdecl XamlAltTabViewHost_CreateInstance(struct XamlViewHostInitializeArgs const &,struct _GUID const &,void * *))"},
            &XamlAltTabViewHost_CreateInstance_Original,
            XamlAltTabViewHost_CreateInstance_Hook,
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

    return TRUE;
}
