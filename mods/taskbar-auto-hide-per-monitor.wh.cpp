// ==WindhawkMod==
// @id              taskbar-auto-hide-per-monitor
// @name            Taskbar auto-hide per monitor
// @description     By default, Windows uses the same auto-hide setting for all monitors. This mod allows setting different auto-hide settings for each monitor.
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar auto-hide per monitor

By default, Windows uses the same auto-hide setting for all monitors. This mod
allows setting different auto-hide settings for each monitor.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitors:
  - - monitor: 1
      $name: Monitor
      $description: >-
        The monitor number (1-based). Used only if monitor interface name is
        empty.
    - monitorInterfaceName: ""
      $name: Monitor interface name
      $description: >-
        If not empty, the given monitor interface name (can also be an interface
        name substring) will be used instead of the monitor number. Can be
        useful if the monitor numbers change often. To see all available
        interface names, set this field to any non-empty string, enable mod
        logs, and look for "Found display device" messages.
    - autoHideDisabled: false
      $name: Auto-hide disabled
      $description: >-
        Whether auto-hide should be disabled for this monitor.
  $name: Monitors
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>
#include <shellapi.h>

#include <atomic>
#include <optional>
#include <vector>

struct MonitorConfig {
    int monitor;
    WindhawkUtils::StringSetting monitorInterfaceName;
    bool autoHideDisabled;
};

struct {
    std::vector<MonitorConfig> monitors;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool g_autoHideDisabledForMonitor;

void ToggleAutoHideToApplySettings() {
    APPBARDATA abd = {sizeof(abd)};
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);

    if (state & ABS_AUTOHIDE) {
        Wh_Log(L"Auto-hide is enabled, toggling to apply settings");

        // Disable auto-hide
        abd.lParam = state & ~ABS_AUTOHIDE;
        SHAppBarMessage(ABM_SETSTATE, &abd);

        // Re-enable auto-hide
        abd.lParam = state;
        SHAppBarMessage(ABM_SETSTATE, &abd);
    }
}

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR monitorResult = nullptr;
    int currentMonitorId = 0;

    auto monitorEnumProc = [&](HMONITOR hMonitor) -> BOOL {
        if (currentMonitorId == monitorId) {
            monitorResult = hMonitor;
            return FALSE;
        }
        currentMonitorId++;
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

HMONITOR GetMonitorByInterfaceNameSubstr(PCWSTR interfaceNameSubstr) {
    HMONITOR monitorResult = nullptr;

    auto monitorEnumProc = [&](HMONITOR hMonitor) -> BOOL {
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (GetMonitorInfo(hMonitor, &monitorInfo)) {
            DISPLAY_DEVICE displayDevice = {
                .cb = sizeof(displayDevice),
            };

            if (EnumDisplayDevices(monitorInfo.szDevice, 0, &displayDevice,
                                   EDD_GET_DEVICE_INTERFACE_NAME)) {
                Wh_Log(L"Found display device %s, interface name: %s",
                       monitorInfo.szDevice, displayDevice.DeviceID);

                if (wcsstr(displayDevice.DeviceID, interfaceNameSubstr)) {
                    Wh_Log(L"Matched display device");
                    monitorResult = hMonitor;
                    return FALSE;
                }
            }
        }
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

std::optional<bool> GetAutoHideDisabledForMonitor(HMONITOR monitor) {
    for (const auto& config : g_settings.monitors) {
        HMONITOR configMonitor = nullptr;

        if (*config.monitorInterfaceName.get()) {
            configMonitor = GetMonitorByInterfaceNameSubstr(
                config.monitorInterfaceName.get());
        } else if (config.monitor >= 1) {
            configMonitor = GetMonitorById(config.monitor - 1);
        }

        if (configMonitor == monitor) {
            return config.autoHideDisabled;
        }
    }

    return std::nullopt;
}

using CSecondaryTray_GetMonitor_t = HMONITOR(WINAPI*)(void* pThis);
CSecondaryTray_GetMonitor_t CSecondaryTray_GetMonitor_Original;

using TrayUI_GetAutoHideFlags_t = DWORD(WINAPI*)(void* pThis);
TrayUI_GetAutoHideFlags_t TrayUI_GetAutoHideFlags_Original;
DWORD WINAPI TrayUI_GetAutoHideFlags_Hook(void* pThis) {
    Wh_Log(L">");

    if (g_autoHideDisabledForMonitor) {
        return 0;
    }

    DWORD ret = TrayUI_GetAutoHideFlags_Original(pThis);

    return ret;
}

enum {
    kTrayUITimerHide = 2,
};

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    Wh_Log(L">");

    // Check if auto-hide is disabled for the primary monitor.
    const POINT ptZero = {0, 0};
    HMONITOR primaryMonitor =
        MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
    auto autoHideDisabled = GetAutoHideDisabledForMonitor(primaryMonitor);
    if (autoHideDisabled && *autoHideDisabled) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            KillTimer(hTaskbarWnd, kTrayUITimerHide);
        }
        return;
    }

    TrayUI__Hide_Original(pThis);
}

using CSecondaryTray__LoadSettings_t = HRESULT(WINAPI*)(void* pThis);
CSecondaryTray__LoadSettings_t CSecondaryTray__LoadSettings_Original;
HRESULT WINAPI CSecondaryTray__LoadSettings_Hook(void* pThis) {
    Wh_Log(L">");

    HMONITOR monitor = CSecondaryTray_GetMonitor_Original(pThis);
    auto autoHideDisabled = GetAutoHideDisabledForMonitor(monitor);
    if (autoHideDisabled && *autoHideDisabled) {
        g_autoHideDisabledForMonitor = true;
    }

    HRESULT ret = CSecondaryTray__LoadSettings_Original(pThis);

    g_autoHideDisabledForMonitor = false;

    return ret;
}

using CSecondaryTray_CheckSize_t = void(WINAPI*)(void* pThis, int param1);
CSecondaryTray_CheckSize_t CSecondaryTray_CheckSize_Original;
void WINAPI CSecondaryTray_CheckSize_Hook(void* pThis, int param1) {
    Wh_Log(L">");

    HMONITOR monitor = CSecondaryTray_GetMonitor_Original(pThis);
    auto autoHideDisabled = GetAutoHideDisabledForMonitor(monitor);
    if (autoHideDisabled && *autoHideDisabled) {
        g_autoHideDisabledForMonitor = true;
    }

    CSecondaryTray_CheckSize_Original(pThis, param1);

    g_autoHideDisabledForMonitor = false;
}

using CTray_RecomputeWorkArea_t = int(WINAPI*)(void* pThis,
                                               HMONITOR monitor,
                                               RECT* rect);
CTray_RecomputeWorkArea_t CTray_RecomputeWorkArea_Original;
int WINAPI CTray_RecomputeWorkArea_Hook(void* pThis,
                                        HMONITOR monitor,
                                        RECT* rect) {
    Wh_Log(L">");

    auto autoHideDisabled = GetAutoHideDisabledForMonitor(monitor);
    if (autoHideDisabled && *autoHideDisabled) {
        g_autoHideDisabledForMonitor = true;
    }

    int ret = CTray_RecomputeWorkArea_Original(pThis, monitor, rect);

    g_autoHideDisabledForMonitor = false;

    return ret;
}

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

WinVersion GetExplorerVersion() {
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
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?GetMonitor@CSecondaryTray@@UEAAPEAUHMONITOR__@@XZ)",
         &CSecondaryTray_GetMonitor_Original},
        {R"(?GetAutoHideFlags@TrayUI@@UEAAIXZ)",
         &TrayUI_GetAutoHideFlags_Original, TrayUI_GetAutoHideFlags_Hook},
        {R"(?_Hide@TrayUI@@QEAAXXZ)", &TrayUI__Hide_Original,
         TrayUI__Hide_Hook},
        {R"(?_LoadSettings@CSecondaryTray@@AEAAJXZ)",
         &CSecondaryTray__LoadSettings_Original,
         CSecondaryTray__LoadSettings_Hook},
        {R"(?CheckSize@CSecondaryTray@@UEAAXH@Z)",
         &CSecondaryTray_CheckSize_Original, CSecondaryTray_CheckSize_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

bool HookTaskbarSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))"},
            &CSecondaryTray_GetMonitor_Original,
        },
        {
            {LR"(public: virtual unsigned int __cdecl TrayUI::GetAutoHideFlags(void))"},
            &TrayUI_GetAutoHideFlags_Original,
            TrayUI_GetAutoHideFlags_Hook,
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
        },
        {
            {LR"(private: long __cdecl CSecondaryTray::_LoadSettings(void))"},
            &CSecondaryTray__LoadSettings_Original,
            CSecondaryTray__LoadSettings_Hook,
        },
        {
            {LR"(public: virtual void __cdecl CSecondaryTray::CheckSize(int))"},
            &CSecondaryTray_CheckSize_Original,
            CSecondaryTray_CheckSize_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookExplorerExeSymbols() {
    HMODULE module = GetModuleHandle(nullptr);

    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(private: int __cdecl CTray::RecomputeWorkArea(struct HMONITOR__ *,struct tagRECT *))"},
            &CTray_RecomputeWorkArea_Original,
            CTray_RecomputeWorkArea_Hook,
        },
    };

    if (!HookSymbols(module, explorerExeHooks, ARRAYSIZE(explorerExeHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    g_settings.monitors.clear();

    for (int i = 0;; i++) {
        auto monitorInterfaceName = WindhawkUtils::StringSetting::make(
            L"monitors[%d].monitorInterfaceName", i);
        int monitor = Wh_GetIntSetting(L"monitors[%d].monitor", i);
        bool hasValue = *monitorInterfaceName.get() || monitor;

        if (!hasValue) {
            break;
        }

        g_settings.monitors.push_back(MonitorConfig{
            .monitor = monitor,
            .monitorInterfaceName = std::move(monitorInterfaceName),
            .autoHideDisabled =
                !!Wh_GetIntSetting(L"monitors[%d].autoHideDisabled", i),
        });
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    } else if (g_winVersion == WinVersion::Win10) {
        Wh_Log(L"Win10 is unsupported");
        return FALSE;
    }

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HookExplorerExeSymbols()) {
        return FALSE;
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(nullptr), L"Shell_TrayWnd", &wndclass)) {
        ToggleAutoHideToApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    ToggleAutoHideToApplySettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ToggleAutoHideToApplySettings();
}
