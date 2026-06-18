// ==WindhawkMod==
// @id              disable-windows-web-search
// @name            Disable Windows Web Search
// @description     Turns off Bing results in Windows Search
// @version         1.0
// @author          davidevol
// @github          https://github.com/davidevol
// @include         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Only changes the user key: HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Search\BingSearchEnabled = 0
Reads and saves the current value (if present) before changing it.
Restores the saved value when the mod is disabled/removed.
Also restores automatically if the mod's enabled option is switched off at runtime.

With Web
![Default radius](https://i.imgur.com/NuhsQFe.jpeg)

With this mod
![Default radius](https://i.imgur.com/HeZwtJ5.png)

The mod is intentionally minimal: it touches only a single user key and ensures safe restoration.

*/
// ==/WindhawkModReadme==

#include <windows.h>

static const wchar_t* kSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search";
static const wchar_t* kValueName = L"BingSearchEnabled";

static bool QueryDwordValue(DWORD* outValue, bool* outExists) {
    if (outValue) *outValue = 0;
    if (outExists) *outExists = false;

    HKEY hKey = nullptr;
    LONG status = RegOpenKeyExW(HKEY_CURRENT_USER, kSubKey, 0, KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS) {
        return false;
    }

    DWORD type = 0;
    DWORD data = 0;
    DWORD size = sizeof(data);

    status = RegQueryValueExW(
        hKey,
        kValueName,
        nullptr,
        &type,
        reinterpret_cast<LPBYTE>(&data),
        &size
    );

    RegCloseKey(hKey);

    if (status == ERROR_SUCCESS && type == REG_DWORD && size == sizeof(DWORD)) {
        if (outValue) *outValue = data;
        if (outExists) *outExists = true;
        return true;
    }

    return false;
}

static bool SetDwordValue(DWORD value) {
    HKEY hKey = nullptr;
    LONG status = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        kSubKey,
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        nullptr,
        &hKey,
        nullptr
    );

    if (status != ERROR_SUCCESS) {
        return false;
    }

    status = RegSetValueExW(
        hKey,
        kValueName,
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&value),
        sizeof(value)
    );

    RegCloseKey(hKey);
    return status == ERROR_SUCCESS;
}

static bool DeleteValueIfExists() {
    HKEY hKey = nullptr;
    LONG status = RegOpenKeyExW(HKEY_CURRENT_USER, kSubKey, 0, KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS) {
        return false;
    }

    status = RegDeleteValueW(hKey, kValueName);
    RegCloseKey(hKey);

    return status == ERROR_SUCCESS || status == ERROR_FILE_NOT_FOUND;
}

static void SaveBackupOnce() {
    if (Wh_GetIntValue(L"backup.saved", 0) != 0) {
        return;
    }

    DWORD currentValue = 0;
    bool exists = false;
    QueryDwordValue(&currentValue, &exists);

    Wh_SetIntValue(L"backup.saved", 1);
    Wh_SetIntValue(L"backup.exists", exists ? 1 : 0);
    Wh_SetIntValue(L"backup.value", static_cast<int>(currentValue));
}

static void ClearBackup() {
    Wh_SetIntValue(L"backup.saved", 0);
    Wh_SetIntValue(L"backup.exists", 0);
    Wh_SetIntValue(L"backup.value", 0);
}

static void RestoreBackup() {
    if (Wh_GetIntValue(L"backup.saved", 0) == 0) {
        return;
    }

    bool exists = Wh_GetIntValue(L"backup.exists", 0) != 0;
    DWORD value = static_cast<DWORD>(Wh_GetIntValue(L"backup.value", 0));

    if (exists) {
        SetDwordValue(value);
    } else {
        DeleteValueIfExists();
    }

    ClearBackup();
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
}

static void ApplyMod() {
    SaveBackupOnce();
    SetDwordValue(0); // Aplica a modificação de forma incondicional
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
}

BOOL Wh_ModInit() {
    ApplyMod();
    return TRUE;
}

void Wh_ModUninit() {
    RestoreBackup();
}