// ==WindhawkMod==
// @id              alt-tab-delayer
// @name            Alt+Tab window delayer
// @description     Delays the appearance of the Alt+Tab window, preventing flickering and reducing distractions during fast app switching
// @version         1.2.0
// @author          L3r0y
// @github          https://github.com/L3r0yThingz
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
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

#include <atomic>

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

std::atomic<DWORD> g_threadIdForAltTabShowWindow;
HWND g_taskSwitcherHwnd;
UINT_PTR g_timerId;
int g_nCmdShow;
int g_delayMilliseconds;

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetWindowsVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
}

void ClearState() {
    if (g_timerId != 0) {
        KillTimer(nullptr, g_timerId);
        g_timerId = 0;
    }
    g_taskSwitcherHwnd = 0;
}

using XamlAltTabViewHost_ViewLoaded_t = void(WINAPI*)(void* pThis);
XamlAltTabViewHost_ViewLoaded_t XamlAltTabViewHost_ViewLoaded_Original;
void WINAPI XamlAltTabViewHost_ViewLoaded_Hook(void* pThis) {
    Wh_Log(L">");
    ClearState();
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    XamlAltTabViewHost_ViewLoaded_Original(pThis);
    g_threadIdForAltTabShowWindow = 0;
}

using XamlAltTabViewHost_DisplayAltTab_t = void(WINAPI*)(void* pThis);
XamlAltTabViewHost_DisplayAltTab_t XamlAltTabViewHost_DisplayAltTab_Original;
void WINAPI XamlAltTabViewHost_DisplayAltTab_Hook(void* pThis) {
    Wh_Log(L">");

    if (g_threadIdForAltTabShowWindow) {
        // Likely called from ViewLoaded.
        return XamlAltTabViewHost_DisplayAltTab_Original(pThis);
    }

    ClearState();
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    XamlAltTabViewHost_DisplayAltTab_Original(pThis);
    g_threadIdForAltTabShowWindow = 0;
}

using CAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                void* param1,
                                                void* param2,
                                                void* param3);
CAltTabViewHost_Show_t CAltTabViewHost_Show_Original;
HRESULT WINAPI CAltTabViewHost_Show_Hook(void* pThis,
                                         void* param1,
                                         void* param2,
                                         void* param3) {
    Wh_Log(L">");
    ClearState();
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    HRESULT ret = CAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    g_threadIdForAltTabShowWindow = 0;
    return ret;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idTimer, DWORD dwTime) {
    KillTimer(nullptr, g_timerId);
    g_timerId = 0;
    Wh_Log(L"Timer proc %d", g_nCmdShow);
    ShowWindow_Original(g_taskSwitcherHwnd, g_nCmdShow);
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
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

    g_winVersion = GetWindowsVersion();

    LoadSettings();

    HMODULE twinuiPcshellModule = LoadLibrary(L"twinui.pcshell.dll");
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (g_winVersion == WinVersion::Win11) {
        // twinui.pcshell.dll
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                // On ARM64, DisplayAltTab doesn't exist, and ViewLoaded
                // contains identical code to DisplayAltTab on x64. On x64,
                // ViewLoaded either calls DisplayAltTab, or queues it to be
                // called later. Therefore, for x64, hooking only ViewLoaded
                // isn't enough.
                {LR"(public: virtual long __cdecl XamlAltTabViewHost::ViewLoaded(void))"},
                &XamlAltTabViewHost_ViewLoaded_Original,
                XamlAltTabViewHost_ViewLoaded_Hook,
            },
            {
                {LR"(private: void __cdecl XamlAltTabViewHost::DisplayAltTab(void))"},
                &XamlAltTabViewHost_DisplayAltTab_Original,
                XamlAltTabViewHost_DisplayAltTab_Hook,
                true,  // Doesn't exist in ARM64
            },
            // For the old Win10 (non-XAML) Alt+Tab (can be enabled with
            // ExplorerPatcher):
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
                true,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            return FALSE;
        }
    } else if (g_winVersion == WinVersion::Win10) {
        // twinui.pcshell.dll
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(ShowWindow, ShowWindow_Hook,
                                       &ShowWindow_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
