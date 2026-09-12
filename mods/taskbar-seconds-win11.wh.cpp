// ==WindhawkMod==
// @id              taskbar-seconds-win11
// @name            Taskbar Seconds (Windows 11)
// @description     Forces display of seconds in Windows 11 taskbar clock
// @version         1.0
// @author          RussianPlugins
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod enforces the 'ShowSecondsInSystemClock' setting for the Windows 11 XAML taskbar.
It hooks registry queries in explorer.exe to return 1 (enabled) and updates the registry key.
*/
// ==/WindhawkModReadme==

#include <windows.h>

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t pOriginalRegQueryValueExW;

// Hook to intercept Explorer reading the clock seconds setting
LSTATUS WINAPI RegQueryValueExW_Hook(
    HKEY    hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE  lpData,
    LPDWORD lpcbData
) {
    if (lpValueName && _wcsicmp(lpValueName, L"ShowSecondsInSystemClock") == 0) {
        if (lpType) {
            *lpType = REG_DWORD;
        }
        if (lpData && lpcbData && *lpcbData >= sizeof(DWORD)) {
            *(DWORD*)lpData = 1; // 1 = Show seconds
            *lpcbData = sizeof(DWORD);
            return ERROR_SUCCESS;
        }
    }

    return pOriginalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

void SetClockRegistryValue(DWORD value) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, L"ShowSecondsInSystemClock", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Seconds (Win11) loaded");

    // 1. Ensure registry value is physically written
    SetClockRegistryValue(1);

    // 2. Hook RegQueryValueExW so Explorer always reads it as enabled
    HMODULE hAdvapi32 = GetModuleHandleW(L"advapi32.dll");
    if (hAdvapi32) {
        FARPROC pFunc = GetProcAddress(hAdvapi32, "RegQueryValueExW");
        if (pFunc) {
            Wh_SetFunctionHook((void*)pFunc, (void*)RegQueryValueExW_Hook, (void**)&pOriginalRegQueryValueExW);
        }
    }

    // 3. Notify Explorer to refresh its settings
    DWORD_PTR dwResult;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings", SMTO_ABORTIFHUNG, 1000, &dwResult);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Taskbar Seconds (Win11) unloaded");

    // Revert registry change when mod is disabled
    SetClockRegistryValue(0);

    DWORD_PTR dwResult;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings", SMTO_ABORTIFHUNG, 1000, &dwResult);
}
