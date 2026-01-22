// ==WindhawkMod==
// @id              disable-auto-reboot-updates
// @name            Disable Auto Reboot for Windows Updates
// @description     Prevents Windows from automatically rebooting when users are logged in
// @version         1.2.1
// @author          Self Edit Technologies Inc
// @github          https://github.com/selfedit
// @homepage        https://www.self-edit.com
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Auto Reboot for Windows Updates

When enabled, this mod sets the `NoAutoRebootWithLoggedOnUsers` registry value to 1,
preventing Windows from automatically rebooting for updates when users are logged in.

When disabled, the registry value is removed (or set to 0, depending on settings).

## How It Works

This mod modifies the Windows Update registry settings when loaded. It automatically
detects which registry path exists on your system, or you can manually select one.

## Settings

- **Registry Path**: Auto-detect (recommended) or manually choose a specific path
- **Value when enabled**: Set to 1 (disable auto reboot) or 0 (allow auto reboot)
- **Delete on disable**: Remove the value entirely or just set it to 0

## Registry Paths

The mod can auto-detect which path exists on your system, or you can manually select:

- **Group Policy AU**: `HKLM\SOFTWARE\Policies\Microsoft\Windows\WindowsUpdate\AU`
- **Windows Update Auto Update**: `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate\Auto Update`
- **Legacy AU path**: `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Windows Update\Auto Update`

**Note:** Requires Windhawk to run with administrator privileges.

## About

Developed by [Self Edit Technologies Inc](https://www.self-edit.com) - Web Development & SEO Services based in Abbotsford, BC.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- registryPath: auto
  $name: Registry Path
  $description: Auto-detect will check which path exists on your system
  $options:
  - auto: Auto-detect (recommended)
  - policiesAU: Group Policy WindowsUpdate AU
  - autoUpdate: Windows Update Auto Update
  - legacyAU: Legacy AU path
- enabledValue: 1
  $name: Value when enabled
  $description: Registry value to set (1 = prevent auto reboot, 0 = allow)
  $options:
  - 1: 1 (Prevent auto reboot)
  - 0: 0 (Allow auto reboot)
- deleteOnDisable: true
  $name: Delete value when mod disabled
  $description: If true, removes the registry value entirely. If false, sets it to 0.
*/
// ==/WindhawkModSettings==

#include <windows.h>

// Registry paths to check
const WCHAR* PATHS[] = {
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU",
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update",
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Windows Update\\Auto Update"
};
const int PATH_COUNT = 3;

struct {
    PCWSTR registryPath;
    DWORD enabledValue;
    BOOL deleteOnDisable;
    const WCHAR* detectedPath;
} settings;

BOOL CheckPathExists(const WCHAR* path) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        path,
        0,
        KEY_READ,
        &hKey
    );
    
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return TRUE;
    }
    return FALSE;
}

const WCHAR* DetectRegistryPath() {
    for (int i = 0; i < PATH_COUNT; i++) {
        if (CheckPathExists(PATHS[i])) {
            Wh_Log(L"Auto-detected registry path: %s", PATHS[i]);
            return PATHS[i];
        }
    }
    
    // Default to Group Policy path if none exist (will be created)
    Wh_Log(L"No existing path found, will create: %s", PATHS[0]);
    return PATHS[0];
}

const WCHAR* GetRegistryPath(PCWSTR pathSetting) {
    // Handle empty string (first load) or explicit "auto" selection
    if (pathSetting[0] == L'\0' || wcscmp(pathSetting, L"auto") == 0) {
        return DetectRegistryPath();
    } else if (wcscmp(pathSetting, L"policiesAU") == 0) {
        return PATHS[0];
    } else if (wcscmp(pathSetting, L"autoUpdate") == 0) {
        return PATHS[1];
    } else if (wcscmp(pathSetting, L"legacyAU") == 0) {
        return PATHS[2];
    } else {
        return DetectRegistryPath();
    }
}

void LoadSettings() {
    settings.registryPath = Wh_GetStringSetting(L"registryPath");
    settings.enabledValue = Wh_GetIntSetting(L"enabledValue");
    settings.deleteOnDisable = Wh_GetIntSetting(L"deleteOnDisable");
    settings.detectedPath = GetRegistryPath(settings.registryPath);
}

BOOL SetRegistryValue(DWORD value) {
    const WCHAR* regPath = settings.detectedPath;
    HKEY hKey;
    
    LONG result = RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        regPath,
        0,
        KEY_SET_VALUE,
        &hKey
    );
    
    if (result == ERROR_FILE_NOT_FOUND) {
        result = RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            regPath,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_SET_VALUE,
            NULL,
            &hKey,
            NULL
        );
    }
    
    if (result == ERROR_SUCCESS) {
        result = RegSetValueExW(
            hKey,
            L"NoAutoRebootWithLoggedOnUsers",
            0,
            REG_DWORD,
            (BYTE*)&value,
            sizeof(value)
        );
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS) {
            Wh_Log(L"Registry set: %s\\NoAutoRebootWithLoggedOnUsers = %d", regPath, value);
            return TRUE;
        } else {
            Wh_Log(L"Failed to set registry value. Error: %d", result);
        }
    } else {
        Wh_Log(L"Failed to open/create registry key: %s. Error: %d", regPath, result);
    }
    
    return FALSE;
}

BOOL DeleteRegistryValue() {
    const WCHAR* regPath = settings.detectedPath;
    HKEY hKey;
    
    LONG result = RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        regPath,
        0,
        KEY_SET_VALUE,
        &hKey
    );
    
    if (result == ERROR_SUCCESS) {
        result = RegDeleteValueW(hKey, L"NoAutoRebootWithLoggedOnUsers");
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) {
            Wh_Log(L"Registry value deleted: %s\\NoAutoRebootWithLoggedOnUsers", regPath);
            return TRUE;
        } else {
            Wh_Log(L"Failed to delete registry value. Error: %d", result);
        }
    } else {
        Wh_Log(L"Failed to open registry key for deletion: %s. Error: %d", regPath, result);
    }
    
    return FALSE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init - Disable Auto Reboot mod");
    LoadSettings();
    Wh_Log(L"Using registry path: %s", settings.detectedPath);
    return SetRegistryValue(settings.enabledValue);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit - Disable Auto Reboot mod");
    
    if (settings.deleteOnDisable) {
        DeleteRegistryValue();
    } else {
        SetRegistryValue(0);
    }
    
    Wh_FreeStringSetting(settings.registryPath);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    Wh_FreeStringSetting(settings.registryPath);
    LoadSettings();
    Wh_Log(L"Using registry path: %s", settings.detectedPath);
    SetRegistryValue(settings.enabledValue);
}
