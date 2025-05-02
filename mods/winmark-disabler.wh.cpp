// ==WindhawkMod==
// @id              winmark-disabler
// @name            Winmark Disabler
// @description     Disables "Activate Windows" and unsupported hardware watermarks via registry tweaks
// @version         1.0
// @author          Ömer Doğan, Adapted by NukedRust
// @github          https://github.com/nukedrust-pixel
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Winmark Disabler

Removes "Activate Windows" and "System does not meet requirements" watermarks by tweaking registry values.
- Disables the Software Protection service (svsvc)
- Hides unsupported hardware notifications

**Note:**  
- Changes take effect after a restart.
- The watermark may return after updates or system checks.
- This does not activate Windows, only hides the watermark.

Original script by [Ömer Doğan](https://github.com/owerdogan/winmark-disabler).
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlwapi.h>

BOOL SetRegistryDword(HKEY hRoot, LPCWSTR subkey, LPCWSTR value, DWORD data) {
    HKEY hKey;
    if (RegCreateKeyExW(hRoot, subkey, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        LONG res = RegSetValueExW(hKey, value, 0, REG_DWORD, (BYTE*)&data, sizeof(DWORD));
        RegCloseKey(hKey);
        return res == ERROR_SUCCESS;
    }
    return FALSE;
}

BOOL RegistryKeyExists(HKEY hRoot, LPCWSTR subkey) {
    HKEY hKey;
    BOOL exists = RegOpenKeyExW(hRoot, subkey, 0, KEY_READ, &hKey) == ERROR_SUCCESS;
    if (exists) RegCloseKey(hKey);
    return exists;
}

void ApplyWinmarkDisabler() {
    // Disable svsvc (Software Protection service)
    SetRegistryDword(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\svsvc", L"Start", 4);

    // Hide unsupported hardware notification, if key exists
    if (RegistryKeyExists(HKEY_CURRENT_USER, L"Control Panel\\UnsupportedHardwareNotificationCache")) {
        SetRegistryDword(HKEY_CURRENT_USER, L"Control Panel\\UnsupportedHardwareNotificationCache", L"SV2", 0);
    }
}

BOOL Wh_ModInit() {
    ApplyWinmarkDisabler();

    MessageBoxW(NULL,
        L"Winmark Disabler has applied registry tweaks.\n\n"
        L"Please restart your computer for the watermark removal to take effect.",
        L"Winmark Disabler", MB_OK | MB_ICONINFORMATION);

    return TRUE;
}

void Wh_ModUninit() {
    // No cleanup necessary; registry changes persist until reverted or Windows restores them
}

void Wh_ModSettingsChanged() {
}
