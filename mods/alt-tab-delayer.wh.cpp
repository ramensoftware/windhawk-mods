// ==WindhawkMod==
// @id              alt-tab-delayer
// @name            Alt+Tab window delayer
// @description     Delays the appearance of the Alt+Tab window, preventing flickering and reducing distractions during fast app switching
// @version         1.0.0
// @author          L3r0y
// @github          https://github.com/L3r0yThingz
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt tab window delayer

This mod improves the Alt+Tab behavior by introducing a short delay before
displaying the tasks window. It helps reduce visual distractions by preventing
brief flickers when quickly switching between apps, similar to how macOS and
Ubuntu handle fast app switching.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delay: 150
  $name: Delay
  $description: The number of milliseconds to delay the switcher.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

std::atomic<DWORD> g_threadIdForAltTabShowWindow;
HWND g_taskSwitcherHwnd;
UINT_PTR g_timerId;
int g_nCmdShow;
int g_delayMilliseconds;

void ClearState() {
    if (g_timerId != 0) {
        KillTimer(nullptr, g_timerId);
        g_timerId = 0;
    }
    g_taskSwitcherHwnd = 0;
}

using XamlAltTabViewHost_DisplayAltTab_t = void(WINAPI*)(void* pThis);
XamlAltTabViewHost_DisplayAltTab_t XamlAltTabViewHost_DisplayAltTab_Original;
void XamlAltTabViewHost_DisplayAltTab_Hook(void* pThis) {
    Wh_Log(L">");
    ClearState();
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    XamlAltTabViewHost_DisplayAltTab_Original(pThis);
    g_threadIdForAltTabShowWindow = 0;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idTimer, DWORD dwTime) {
    KillTimer(nullptr, g_timerId);
    g_timerId = 0;
    Wh_Log(L"Timer proc %d", g_nCmdShow);
    ShowWindow_Original(g_taskSwitcherHwnd, g_nCmdShow);
}

BOOL ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (hWnd == g_taskSwitcherHwnd && nCmdShow == SW_HIDE) {
        Wh_Log(L"> Resetting state");
        ClearState();
        return ShowWindow_Original(hWnd, nCmdShow);
    }
    if (g_threadIdForAltTabShowWindow != GetCurrentThreadId()) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    Wh_Log(L">");
    if (nCmdShow != SW_HIDE) {
        g_taskSwitcherHwnd = hWnd;
        g_nCmdShow = nCmdShow;
        g_timerId =
            SetTimer(nullptr, g_timerId, g_delayMilliseconds, TimerProc);
        return TRUE;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

void LoadSettings() {
    g_delayMilliseconds = Wh_GetIntSetting(L"delay");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
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

void Wh_ModSettingsChanged() {
    LoadSettings();
}
