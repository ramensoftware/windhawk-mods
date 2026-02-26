// ==WindhawkMod==
// @id              taskbar-split-by-monitor
// @name            Taskbar: Show windows only on their monitor
// @name:ja         タスクバー: モニターごとにウィンドウを表示
// @description     Each monitor's taskbar shows only the windows on that monitor
// @description:ja  各モニターのタスクバーにそのモニター上のウィンドウのみを表示します
// @version         1.1.0
// @author          ajisaiflow
// @github          https://github.com/lighfu
// @include         explorer.exe
// @include         ShellHost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar: Show windows only on their monitor

In a multi-monitor setup, this mod forces each monitor's taskbar to display
only the windows located on that specific monitor.

## How it works

The mod hooks Windows Explorer's registry reads for multi-monitor taskbar
settings, enforcing the following:

- **"Show my taskbar on all displays"** is always enabled
- **"Show taskbar buttons on"** is set to **"Taskbar where window is open"**

## Modes

- **Per-monitor only (default)**: Each taskbar shows only the windows on
  its own monitor
- **Main + per-monitor**: The primary taskbar shows all windows; secondary
  taskbars show only windows on their monitor

## Supported OS

- Windows 10
- Windows 11
- Windows 11 24H2 (ShellHost.exe supported)

---

# タスクバー: モニターごとにウィンドウを表示

マルチモニター環境で、各モニターのタスクバーにそのモニター上にあるウィンドウのみを
表示するMODです。

## 動作原理

Windows Explorer がマルチモニタータスクバーの設定を読み取る際にフックし、
以下の設定を強制的に適用します：

- 「すべてのディスプレイにタスクバーを表示する」を有効化
- 「タスクバーボタンの表示先」を「ウィンドウが開いているタスクバー」に設定

## モード

- **モニターごとのみ（デフォルト）**: 各タスクバーにそのモニター上の
  ウィンドウのみを表示
- **メイン＋モニターごと**: メインタスクバーにはすべてのウィンドウを表示し、
  セカンダリタスクバーにはそのモニター上のウィンドウのみを表示

## 対応OS

- Windows 10
- Windows 11
- Windows 11 24H2（ShellHost.exe 対応）
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FilterMode: perMonitorOnly
  $name: Filter Mode
  $name:ja: フィルターモード
  $description: How to filter windows across taskbars
  $description:ja: タスクバー間でのウィンドウフィルタリング方法
  $options:
  - perMonitorOnly: Per-monitor only
  - mainAndPerMonitor: Main shows all + per-monitor on secondary
*/
// ==/WindhawkModSettings==

#include <windows.h>

// Registry path for taskbar settings
static const WCHAR kRegPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

// Settings
struct {
    // 0 = per-monitor only (MMTaskbarMode=2)
    // 1 = main + per-monitor (MMTaskbarMode=1)
    int filterMode;
} g_settings;

// Saved original registry values for restoration on unload
static DWORD g_origMMTaskbarMode = 0;
static DWORD g_origMMTaskbarEnabled = 0;
static bool g_origSaved = false;

// Desired MMTaskbarMode based on current settings
static DWORD GetDesiredMMTaskbarMode() {
    return (g_settings.filterMode == 0) ? 2 : 1;
}

// ============================================================
// Hook: RegQueryValueExW (from kernelbase.dll)
// ============================================================
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
static RegQueryValueExW_t g_origRegQueryValueExW = nullptr;

LSTATUS WINAPI Hook_RegQueryValueExW(
    HKEY hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData)
{
    LSTATUS result = g_origRegQueryValueExW(
        hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

    if (result != ERROR_SUCCESS || !lpValueName || !lpData || !lpcbData) {
        return result;
    }
    if (*lpcbData < sizeof(DWORD)) {
        return result;
    }
    if (lpType && *lpType != REG_DWORD) {
        return result;
    }

    if (_wcsicmp(lpValueName, L"MMTaskbarMode") == 0) {
        DWORD desired = GetDesiredMMTaskbarMode();
        Wh_Log(L"[RegQueryValueExW] MMTaskbarMode: %d -> %d",
               *(DWORD*)lpData, desired);
        *(DWORD*)lpData = desired;
    }
    else if (_wcsicmp(lpValueName, L"MMTaskbarEnabled") == 0) {
        Wh_Log(L"[RegQueryValueExW] MMTaskbarEnabled: %d -> 1",
               *(DWORD*)lpData);
        *(DWORD*)lpData = 1;
    }

    return result;
}

// ============================================================
// Hook: RegGetValueW (from kernelbase.dll)
//   Some shell components use this instead of RegQueryValueExW.
//   RegGetValueW may call NtQueryValueKey directly, bypassing
//   RegQueryValueExW, so we need to hook it separately.
// ============================================================
using RegGetValueW_t = LSTATUS(WINAPI*)(
    HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue,
    DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);
static RegGetValueW_t g_origRegGetValueW = nullptr;

LSTATUS WINAPI Hook_RegGetValueW(
    HKEY hkey,
    LPCWSTR lpSubKey,
    LPCWSTR lpValue,
    DWORD dwFlags,
    LPDWORD pdwType,
    PVOID pvData,
    LPDWORD pcbData)
{
    LSTATUS result = g_origRegGetValueW(
        hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);

    if (result != ERROR_SUCCESS || !lpValue || !pvData || !pcbData) {
        return result;
    }
    if (*pcbData < sizeof(DWORD)) {
        return result;
    }
    if (pdwType && *pdwType != REG_DWORD) {
        return result;
    }

    if (_wcsicmp(lpValue, L"MMTaskbarMode") == 0) {
        DWORD desired = GetDesiredMMTaskbarMode();
        Wh_Log(L"[RegGetValueW] MMTaskbarMode: %d -> %d",
               *(DWORD*)pvData, desired);
        *(DWORD*)pvData = desired;
    }
    else if (_wcsicmp(lpValue, L"MMTaskbarEnabled") == 0) {
        Wh_Log(L"[RegGetValueW] MMTaskbarEnabled: %d -> 1",
               *(DWORD*)pvData);
        *(DWORD*)pvData = 1;
    }

    return result;
}

// ============================================================
// Registry helpers
// ============================================================

// Save current registry values so we can restore them on unload
static void SaveOriginalValues() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegPath, 0, KEY_READ, &hKey)
        != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open registry key for reading originals");
        return;
    }

    DWORD size = sizeof(DWORD);
    if (RegQueryValueExW(hKey, L"MMTaskbarMode", nullptr, nullptr,
                         (LPBYTE)&g_origMMTaskbarMode, &size) != ERROR_SUCCESS) {
        g_origMMTaskbarMode = 0;
    }
    size = sizeof(DWORD);
    if (RegQueryValueExW(hKey, L"MMTaskbarEnabled", nullptr, nullptr,
                         (LPBYTE)&g_origMMTaskbarEnabled, &size) != ERROR_SUCCESS) {
        g_origMMTaskbarEnabled = 1;
    }

    RegCloseKey(hKey);
    g_origSaved = true;
    Wh_Log(L"Saved originals: MMTaskbarMode=%d, MMTaskbarEnabled=%d",
           g_origMMTaskbarMode, g_origMMTaskbarEnabled);
}

// Write our desired values to the registry.
// This triggers RegNotifyChangeKeyValue watchers in the taskbar,
// causing it to re-read settings (where our hooks intercept).
static void WriteDesiredValues() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegPath, 0, KEY_SET_VALUE, &hKey)
        != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open registry key for writing");
        return;
    }

    DWORD mode = GetDesiredMMTaskbarMode();
    DWORD enabled = 1;
    RegSetValueExW(hKey, L"MMTaskbarMode", 0, REG_DWORD,
                   (const BYTE*)&mode, sizeof(DWORD));
    RegSetValueExW(hKey, L"MMTaskbarEnabled", 0, REG_DWORD,
                   (const BYTE*)&enabled, sizeof(DWORD));

    RegCloseKey(hKey);
    Wh_Log(L"Wrote registry: MMTaskbarMode=%d, MMTaskbarEnabled=%d",
           mode, enabled);
}

// Restore original registry values
static void RestoreOriginalValues() {
    if (!g_origSaved) return;

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRegPath, 0, KEY_SET_VALUE, &hKey)
        != ERROR_SUCCESS) {
        return;
    }

    RegSetValueExW(hKey, L"MMTaskbarMode", 0, REG_DWORD,
                   (const BYTE*)&g_origMMTaskbarMode, sizeof(DWORD));
    RegSetValueExW(hKey, L"MMTaskbarEnabled", 0, REG_DWORD,
                   (const BYTE*)&g_origMMTaskbarEnabled, sizeof(DWORD));

    RegCloseKey(hKey);
    Wh_Log(L"Restored originals: MMTaskbarMode=%d, MMTaskbarEnabled=%d",
           g_origMMTaskbarMode, g_origMMTaskbarEnabled);
}

// Notify the taskbar to re-read its settings
static void NotifySettingsChange() {
    // Method 1: Broadcast WM_SETTINGCHANGE with "TraySettings" lParam.
    // This is what the Windows Settings app sends when changing taskbar options.
    DWORD_PTR dwResult = 0;
    SendMessageTimeoutW(
        HWND_BROADCAST,
        WM_SETTINGCHANGE,
        0,
        (LPARAM)L"TraySettings",
        SMTO_ABORTIFHUNG,
        5000,
        &dwResult);

    // Method 2: Also send directly to Shell_TrayWnd for good measure
    HWND hTrayWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTrayWnd) {
        SendMessageTimeoutW(
            hTrayWnd,
            WM_SETTINGCHANGE,
            0,
            (LPARAM)L"TraySettings",
            SMTO_ABORTIFHUNG,
            5000,
            &dwResult);
    }

    Wh_Log(L"Settings change notification sent");
}

// ============================================================
// Settings
// ============================================================
static void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"FilterMode");
    g_settings.filterMode =
        (mode && _wcsicmp(mode, L"mainAndPerMonitor") == 0) ? 1 : 0;
    if (mode) {
        Wh_FreeStringSetting(mode);
    }
    Wh_Log(L"Settings loaded: filterMode=%d", g_settings.filterMode);
}

// ============================================================
// Windhawk mod lifecycle
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"=== taskbar-split-by-monitor v1.1.0 initializing ===");

    LoadSettings();

    // Save original registry values BEFORE hooks are applied
    SaveOriginalValues();

    // Write desired values to registry to trigger RegNotifyChangeKeyValue
    WriteDesiredValues();

    // Hook RegQueryValueExW from kernelbase.dll
    HMODULE hKernelbase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelbase) {
        void* pRegQueryValueExW =
            (void*)GetProcAddress(hKernelbase, "RegQueryValueExW");
        if (pRegQueryValueExW) {
            Wh_Log(L"kernelbase!RegQueryValueExW @ %p", pRegQueryValueExW);
            if (Wh_SetFunctionHook(pRegQueryValueExW,
                                   (void*)Hook_RegQueryValueExW,
                                   (void**)&g_origRegQueryValueExW)) {
                Wh_Log(L"Hooked RegQueryValueExW OK");
            } else {
                Wh_Log(L"FAILED to hook RegQueryValueExW");
            }
        }

        // Hook RegGetValueW (may bypass RegQueryValueExW internally)
        void* pRegGetValueW =
            (void*)GetProcAddress(hKernelbase, "RegGetValueW");
        if (pRegGetValueW) {
            Wh_Log(L"kernelbase!RegGetValueW @ %p", pRegGetValueW);
            if (Wh_SetFunctionHook(pRegGetValueW,
                                   (void*)Hook_RegGetValueW,
                                   (void**)&g_origRegGetValueW)) {
                Wh_Log(L"Hooked RegGetValueW OK");
            } else {
                Wh_Log(L"FAILED to hook RegGetValueW");
            }
        }
    } else {
        Wh_Log(L"ERROR: kernelbase.dll not found");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // Hooks are now active. Notify the taskbar to re-read settings.
    NotifySettingsChange();
    Wh_Log(L"Init complete, notification sent (filterMode=%d)",
           g_settings.filterMode);
}

void Wh_ModBeforeUninit() {
    // Hooks are still active here. Restore the original registry values
    // so that after hooks are removed, the taskbar reads the real values.
    RestoreOriginalValues();
}

void Wh_ModUninit() {
    Wh_Log(L"=== taskbar-split-by-monitor uninitializing ===");
    // Hooks are now removed. Notify the taskbar to re-read the restored values.
    NotifySettingsChange();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
    WriteDesiredValues();
    NotifySettingsChange();
}
