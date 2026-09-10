// ==WindhawkMod==
// @id              taskbar-auto-show-on-flash
// @name            Taskbar auto-show on notification flash
// @description     Auto-hide the taskbar after a configurable delay when a notification flash is detected; new notifications reset the timer
// @version         1.1
// @author          yunhanbian6-cmyk
// @github          https://github.com/yunhanbian6-cmyk
// @include         explorer.exe
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar auto-show on notification flash

当 Windows 任务栏启用了自动隐藏时，应用通知闪烁会强制任务栏显示。
本 Mod 让任务栏在通知后延迟一段时间自动隐藏，新通知会重置计时器。

- 支持 Windows 10 和 Windows 11
- 可配置延迟时间（默认 5 秒）
- 可启用/禁用 Mod

## 使用前提

必须在 Windows 设置中启用任务栏自动隐藏。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delay: 5000
  $name: 自动隐藏延迟（毫秒）
  $description: 最后一次通知后等待多少毫秒再隐藏任务栏。设为 0 则使用 Windows 默认行为。
- enabled: true
  $name: 启用通知自动隐藏
  $description: 禁用后，任务栏将在通知期间保持显示（Windows 默认行为）。
- compatAutoHideSpeed: false
  $name: 兼容 Taskbar auto-hide speed
  $description: 如果同时使用了 Taskbar auto-hide speed mod，请启用此项以避免冲突。
*/
// ==/WindhawkModSettings==

#include <minwindef.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <cstdint>

// Hook function types (using __thiscall to match MSVC ABI)
using WndProc_t = LRESULT(__thiscall*)(void* pThis,
                                       HWND hWnd,
                                       UINT msg,
                                       WPARAM wParam,
                                       LPARAM lParam);
using PermitAutoHide_t = BOOL(__thiscall*)(void* pThis);

// Original function pointers
WndProc_t WndProc_o = nullptr;
PermitAutoHide_t PermitAutoHide_o = nullptr;

// State
FILETIME lastNotifyTime = {0};
uint64_t delayFiletime;
bool g_enabled;
bool g_hooksInstalled = false;
bool g_compatAutoHideSpeed = false;

enum class ExplorerVersion {
    Unsupported,
    Win10,
    Win11,
};

ExplorerVersion explorerVer;

// Forward declarations
bool TryInstallHooks();
LRESULT __thiscall WndProc_hook(void* pThis, HWND hWnd, UINT msg,
                                WPARAM wParam, LPARAM lParam);
BOOL __thiscall PermitAutoHide_hook(void* pThis);

// Version detection

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

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

ExplorerVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo)
        return ExplorerVersion::Unsupported;

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Explorer version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000)
                return ExplorerVersion::Win10;
            else
                return ExplorerVersion::Win11;
            break;
    }

    return ExplorerVersion::Unsupported;
}

// Settings

void LoadSettings() {
    int delayMs = Wh_GetIntSetting(L"delay");
    delayFiletime = (uint64_t)delayMs * 10 * 1000;  // ms -> 100-nanosecond
    g_enabled = Wh_GetIntSetting(L"enabled");
    g_compatAutoHideSpeed = Wh_GetIntSetting(L"compatAutoHideSpeed");
}

// Try to install hooks - returns true if successful
bool TryInstallHooks() {
    if (g_hooksInstalled) {
        return true;
    }

    HMODULE hModule = (explorerVer == ExplorerVersion::Win10)
                          ? GetModuleHandle(L"explorer.exe")
                          : GetModuleHandle(L"taskbar.dll");

    if (!hModule) {
        hModule = (explorerVer == ExplorerVersion::Win10)
                      ? LoadLibrary(L"explorer.exe")
                      : LoadLibrary(L"taskbar.dll");
    }

    if (!hModule) {
        Wh_Log(L"Failed to get taskbar module");
        return false;
    }

    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK explorerTaskbarHooks[] = {
        {{
            // Win11 / Win10 x64
            L"protected: virtual __int64 __cdecl CTaskBand::v_WndProc("
            L"struct HWND__ *,unsigned int,unsigned __int64,__int64)",
            // Win10 x86 (fallback)
            L"protected: virtual long __thiscall CTaskBand::v_WndProc("
            L"struct HWND__ *,unsigned int,unsigned int,long)",
        },
         (void**)&WndProc_o,
         (void*)WndProc_hook},
        {{
            // Win11 / Win10 x64
            L"public: virtual int __cdecl CTaskListWnd::PermitAutoHide(void)",
            // Win10 x86 (fallback)
            L"public: virtual int __stdcall CTaskListWnd::PermitAutoHide(void)",
        },
         (void**)&PermitAutoHide_o,
         (void*)PermitAutoHide_hook},
    };

    if (!HookSymbols(hModule, explorerTaskbarHooks, ARRAYSIZE(explorerTaskbarHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    g_hooksInstalled = true;
    return true;
}

// Notification detection hook
// CTaskBand::v_WndProc receives internal notification messages:
//   Win10: msg=0xC028, Win11: msg=0xC029
//   wParam==0x8006 indicates a flash/notification event

LRESULT __thiscall WndProc_hook(void* pThis,
                                HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    static UINT notificationMsgId =
        (explorerVer == ExplorerVersion::Win10) ? 0xC028 : 0xC029;

    if (g_enabled && msg == notificationMsgId && wParam == 0x8006) {
        GetSystemTimeAsFileTime(&lastNotifyTime);
        Wh_Log(L"Notification detected, timestamp updated");
    }

    return WndProc_o(pThis, hWnd, msg, wParam, lParam);
}

// Auto-hide control hook
// CTaskListWnd::PermitAutoHide decides whether the taskbar is allowed to
// auto-hide. Windows returns FALSE during active notifications. We override
// this to return TRUE after our configured delay has elapsed.

BOOL __thiscall PermitAutoHide_hook(void* pThis) {
    if (!g_enabled || delayFiletime == 0) {
        return PermitAutoHide_o(pThis);
    }

    FILETIME currentTime;
    GetSystemTimeAsFileTime(&currentTime);

    uint64_t now = ((uint64_t)currentTime.dwHighDateTime << 32) |
                   currentTime.dwLowDateTime;
    uint64_t last = ((uint64_t)lastNotifyTime.dwHighDateTime << 32) |
                    lastNotifyTime.dwLowDateTime;

    // If enough time has passed since the last notification, allow auto-hide
    // When Taskbar auto-hide speed mod is enabled, we use a slightly longer
    // delay to account for the accelerated animation
    uint64_t adjustedDelay = delayFiletime;
    if (g_compatAutoHideSpeed) {
        // Add 100ms buffer when animation speed mod is present
        adjustedDelay += 100 * 10 * 1000;
    }

    if (now - last > adjustedDelay) {
        return TRUE;
    }

    return PermitAutoHide_o(pThis);
}

// Mod entry point

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    explorerVer = GetExplorerVersion();
    if (explorerVer == ExplorerVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    // Try to install hooks immediately
    if (!TryInstallHooks()) {
        Wh_Log(L"Hooks not installed yet, will retry in Wh_ModAfterInit");
        // Return TRUE to keep the mod loaded - hooks will be installed later
    }

    Wh_Log(L"Mod initialized: delay=%lldms, enabled=%d",
           delayFiletime / (10 * 1000), g_enabled);

    return TRUE;
}

// Called after mod initialization - retry hook installation if needed
void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_hooksInstalled) {
        Wh_Log(L"Retrying hook installation...");

        // Retry with delays for startup timing issues
        for (int i = 0; i < 5; i++) {
            if (TryInstallHooks()) {
                Wh_Log(L"Hooks installed successfully on retry %d", i + 1);
                return;
            }
            if (i < 4) {
                Sleep(1000);  // Wait 1 second between retries
            }
        }

        Wh_Log(L"Failed to install hooks after retries");
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    LoadSettings();
    Wh_Log(L"Settings updated: delay=%lldms, enabled=%d, hooks=%s, "
           L"compat-autohide-speed=%s",
           delayFiletime / (10 * 1000), g_enabled,
           g_hooksInstalled ? L"installed" : L"not installed",
           g_compatAutoHideSpeed ? L"on" : L"off");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
