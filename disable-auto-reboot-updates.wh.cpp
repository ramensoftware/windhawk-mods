// ==WindhawkMod==
// @id              disable-auto-reboot-updates
// @name            Disable Auto Reboot for Windows Updates
// @description     Prevents Windows from automatically rebooting when users are logged in
// @version         1.4.0
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

This mod uses regedit to modify the Windows Update registry settings. It will prompt
for UAC elevation when enabling or disabling the mod.

## Settings

- **Registry Path**: Choose which registry path to use
- **Value when enabled**: Set to 1 (disable auto reboot) or 0 (allow auto reboot)
- **Delete on disable**: Remove the value entirely or just set it to 0

## Registry Paths

- **Group Policy AU** (recommended): `HKLM\SOFTWARE\Policies\Microsoft\Windows\WindowsUpdate\AU`
- **Windows Update Auto Update**: `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate\Auto Update`

**Note:** Will prompt for UAC elevation to make registry changes.

## About

Developed by [Self Edit Technologies Inc](https://www.self-edit.com) - Web Development & SEO Services based in Abbotsford, BC.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- registryPath: policiesAU
  $name: Registry Path
  $description: Select which registry path to modify
  $options:
  - policiesAU: Group Policy WindowsUpdate AU (recommended)
  - autoUpdate: Windows Update Auto Update
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
#include <shlobj.h>
#include <stdio.h>

struct {
    PCWSTR registryPath;
    DWORD enabledValue;
    BOOL deleteOnDisable;
} settings;

const WCHAR* GetRegistryPath(PCWSTR pathSetting) {
    if (pathSetting[0] == L'\0' || wcscmp(pathSetting, L"policiesAU") == 0) {
        return L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU";
    } else if (wcscmp(pathSetting, L"autoUpdate") == 0) {
        return L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update";
    } else {
        return L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU";
    }
}

void LoadSettings() {
    settings.registryPath = Wh_GetStringSetting(L"registryPath");
    settings.enabledValue = Wh_GetIntSetting(L"enabledValue");
    settings.deleteOnDisable = Wh_GetIntSetting(L"deleteOnDisable");
}

BOOL RunRegFile(const WCHAR* regContent) {
    // Get temp path
    WCHAR tempPath[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempPath) == 0) {
        Wh_Log(L"GetTempPath failed");
        return FALSE;
    }

    // Create temp .reg file path
    WCHAR regFilePath[MAX_PATH];
    wsprintfW(regFilePath, L"%s\\wh_autoreboot.reg", tempPath);

    // Write the .reg file
    HANDLE hFile = CreateFileW(regFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateFile failed: %d", GetLastError());
        return FALSE;
    }

    // Convert wide string to UTF-16 LE with BOM for .reg file
    BYTE bom[] = {0xFF, 0xFE}; // UTF-16 LE BOM
    DWORD written;
    WriteFile(hFile, bom, sizeof(bom), &written, NULL);
    
    DWORD contentLen = (DWORD)(wcslen(regContent) * sizeof(WCHAR));
    WriteFile(hFile, regContent, contentLen, &written, NULL);
    CloseHandle(hFile);

    Wh_Log(L"Created reg file: %s", regFilePath);

    // Run regedit /s with elevation
    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = L"regedit.exe";
    
    WCHAR params[MAX_PATH + 10];
    wsprintfW(params, L"/s \"%s\"", regFilePath);
    sei.lpParameters = params;
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"ShellExecuteEx failed: %d", GetLastError());
        DeleteFileW(regFilePath);
        return FALSE;
    }

    // Wait for regedit to finish
    if (sei.hProcess) {
        WaitForSingleObject(sei.hProcess, 10000);
        CloseHandle(sei.hProcess);
    }

    // Clean up temp file
    DeleteFileW(regFilePath);
    
    Wh_Log(L"Registry updated successfully");
    return TRUE;
}

BOOL SetRegistryValue(DWORD value) {
    const WCHAR* regPath = GetRegistryPath(settings.registryPath);
    
    WCHAR regContent[512];
    wsprintfW(regContent, 
        L"Windows Registry Editor Version 5.00\r\n\r\n"
        L"[%s]\r\n"
        L"\"NoAutoRebootWithLoggedOnUsers\"=dword:%08x\r\n",
        regPath, value);

    Wh_Log(L"Setting registry value to %d at %s", value, regPath);
    return RunRegFile(regContent);
}

BOOL DeleteRegistryValue() {
    const WCHAR* regPath = GetRegistryPath(settings.registryPath);
    
    WCHAR regContent[512];
    wsprintfW(regContent, 
        L"Windows Registry Editor Version 5.00\r\n\r\n"
        L"[%s]\r\n"
        L"\"NoAutoRebootWithLoggedOnUsers\"=-\r\n",
        regPath);

    Wh_Log(L"Deleting registry value at %s", regPath);
    return RunRegFile(regContent);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init - Disable Auto Reboot mod");
    LoadSettings();
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
    SetRegistryValue(settings.enabledValue);
}
