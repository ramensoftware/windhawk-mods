// ==WindhawkMod==
// @id              taskbar-auto-hide-after-flash
// @name            Auto-hide taskbar after notification flash
// @description     Re-hides the taskbar after a configurable delay when a notification flash keeps it shown; new flashes reset the timer
// @description:zh-CN  通知闪烁导致任务栏弹出后，延迟一段时间自动隐藏；新通知会重置计时器
// @version         2.0
// @author          yunhanbian6-cmyk
// @github          https://github.com/yunhanbian6-cmyk
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto-hide taskbar after notification flash

When the Windows taskbar has auto-hide enabled, application notification
flashes force the taskbar to stay visible. This mod re-permits auto-hide
after a configurable delay so the taskbar hides again, and new
notifications reset the timer.

Supports Windows 10 and Windows 11. Note that on Windows 11, hiding the
taskbar during a notification can make it completely invisible (Windows 10
at least shows an orange line at the screen edge), so choose a delay that
gives you enough time to notice.

---

当 Windows 任务栏启用了自动隐藏时，应用通知闪烁会强制任务栏保持显示。
本 Mod 让任务栏在通知后延迟一段时间自动重新隐藏，新通知会重置计时器。

支持 Windows 10 和 Windows 11。注意在 Windows 11 上，通知期间隐藏任务栏
可能使通知完全不可见（Windows 10 至少会在屏幕边缘显示一条橙色线），
请根据需要选择合适的延迟时间。

**Prerequisite:** Windows taskbar auto-hide must be enabled in Settings.

**前提：** 必须在 Windows 设置中启用任务栏自动隐藏。

## Credits

Based on [Better Taskbar Autohide](https://windhawk.net/mods/taskbar-autohide-better)
by Cirn09. This mod improves upon it with a more reliable notification
detection mechanism, a monotonic clock, and safer hook installation.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- delay: 5000
  $name: Auto-hide delay (ms)
  $name:zh-CN: 自动隐藏延迟（毫秒）
  $description: How long to wait after the last notification before hiding the taskbar. Set to 0 to use the Windows default behavior.
  $description:zh-CN: 最后一次通知后等待多少毫秒再隐藏任务栏。设为 0 则使用 Windows 默认行为。
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <cstdint>

// Hook function types
using WndProc_t = LRESULT(__thiscall*)(void* pThis,
                                       HWND hWnd,
                                       UINT msg,
                                       WPARAM wParam,
                                       LPARAM lParam);
using PermitAutoHide_t = BOOL(__thiscall*)(void* pThis);

// Original function pointers
WndProc_t g_wndProcOriginal = nullptr;
PermitAutoHide_t g_permitAutoHideOriginal = nullptr;

// State
uint64_t g_lastNotifyTick = 0;
uint64_t g_delayMs = 5000;
bool g_hooksInstalled = false;

enum class ExplorerVersion {
    Unsupported,
    Win10,
    Win11,
};

ExplorerVersion g_explorerVer;

// Shell hook message ID — resolved dynamically, not a fixed constant.
UINT g_shellHookMsg = 0;

// Forward declarations
bool TryInstallHooks();
LRESULT __thiscall WndProc_hook(void* pThis,
                                HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam);
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
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Explorer version: %u.%u.%u.%u", major,
           LOWORD(fixedFileInfo->dwFileVersionMS), build,
           LOWORD(fixedFileInfo->dwFileVersionLS));

    if (major != 10)
        return ExplorerVersion::Unsupported;

    return build < 22000 ? ExplorerVersion::Win10 : ExplorerVersion::Win11;
}

// Settings

void LoadSettings() {
    int delayMs = Wh_GetIntSetting(L"delay");
    g_delayMs = static_cast<uint64_t>(std::max(0, delayMs));
}

// Try to install hooks - returns true if successful
bool TryInstallHooks() {
    if (g_hooksInstalled) {
        return true;
    }

    HMODULE hModule = (g_explorerVer == ExplorerVersion::Win10)
                          ? GetModuleHandle(L"explorer.exe")
                          : GetModuleHandle(L"taskbar.dll");

    if (!hModule && g_explorerVer == ExplorerVersion::Win11) {
        hModule =
            LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    if (!hModule) {
        Wh_Log(L"Failed to get taskbar module");
        return false;
    }

    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK explorerTaskbarHooks[] = {
        {{
             L"protected: virtual __int64 __cdecl CTaskBand::v_WndProc("
             L"struct HWND__ *,unsigned int,unsigned __int64,__int64)",
         },
         (void**)&g_wndProcOriginal,
         (void*)WndProc_hook},
        {{
             L"public: virtual int __cdecl CTaskListWnd::PermitAutoHide(void)",
         },
         (void**)&g_permitAutoHideOriginal,
         (void*)PermitAutoHide_hook},
    };

    if (!WindhawkUtils::HookSymbols(hModule, explorerTaskbarHooks,
                                    ARRAYSIZE(explorerTaskbarHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    g_hooksInstalled = true;
    return true;
}

// Notification detection hook
// CTaskBand::v_WndProc receives shell hook messages. HSHELL_FLASH (0x8006)
// indicates a window flash/notification event. The shell hook message ID is
// resolved via RegisterWindowMessage for version independence.

LRESULT __thiscall WndProc_hook(void* pThis,
                                HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    if (g_delayMs > 0 && msg == g_shellHookMsg && wParam == HSHELL_FLASH) {
        g_lastNotifyTick = GetTickCount64();
        Wh_Log(L"Flash notification detected, timer reset");
    }

    return g_wndProcOriginal(pThis, hWnd, msg, wParam, lParam);
}

// Auto-hide control hook
// CTaskListWnd::PermitAutoHide decides whether the taskbar is allowed to
// auto-hide. Windows returns FALSE during active notifications. We override
// this to return TRUE after our configured delay has elapsed.

BOOL __thiscall PermitAutoHide_hook(void* pThis) {
    if (g_delayMs == 0) {
        return g_permitAutoHideOriginal(pThis);
    }

    uint64_t now = GetTickCount64();
    if (now - g_lastNotifyTick > g_delayMs) {
        return TRUE;
    }

    return g_permitAutoHideOriginal(pThis);
}

// Mod entry point

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    if (g_shellHookMsg == 0) {
        Wh_Log(L"Failed to register SHELLHOOK message");
        return FALSE;
    }

    g_explorerVer = GetExplorerVersion();
    if (g_explorerVer == ExplorerVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (!TryInstallHooks()) {
        Wh_Log(L"Failed to install hooks");
        return FALSE;
    }

    Wh_Log(L"Mod initialized: delay=%llu ms", g_delayMs);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings updated: delay=%llu ms", g_delayMs);
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
