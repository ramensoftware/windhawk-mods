// ==WindhawkMod==
// @id              taskbar-notification-icons-show-all
// @name            Always show all taskbar tray icons
// @description     Restore the missing Windows option to always show all notification (tray) icons (Windows 11 only)
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
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
# Always show all taskbar tray icons

Restore the missing Windows option to always show all notification (tray) icons.

Alternatively, just make sure that new icons are shown by default.

Only Windows 11 is supported.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: showAll
  $name: Mode
  $options:
  - showAll: All icons are shown
  - showNew: New icons are shown, existing icons are unaffected
*/
// ==/WindhawkModSettings==

#include <ntstatus.h>

#include <string>
#include <string_view>
#include <vector>

enum class Mode {
    showAll,
    showNew,
};

struct {
    Mode mode;
} g_settings;

// https://github.com/valinet/wh-mods/blob/61319815c7e018e392a08077dc364559548ade02/mods/valinet-unserver.wh.cpp#L95
// https://stackoverflow.com/questions/937044/determine-path-to-registry-key-from-hkey-handle-in-c
std::wstring GetPathFromHKEY(HKEY key) {
    std::wstring keyPath;
    if (key) {
        HMODULE dll = GetModuleHandleW(L"ntdll.dll");
        if (dll) {
            typedef NTSTATUS(__stdcall * NtQueryKeyType)(
                HANDLE KeyHandle, int KeyInformationClass, PVOID KeyInformation,
                ULONG Length, PULONG ResultLength);
            NtQueryKeyType func = reinterpret_cast<NtQueryKeyType>(
                GetProcAddress(dll, "NtQueryKey"));
            if (func) {
                DWORD size = 0;
                NTSTATUS result = func(key, 3, 0, 0, &size);
                if (result == STATUS_BUFFER_TOO_SMALL) {
                    size = size + 2;
                    wchar_t* buffer = new (std::nothrow)
                        wchar_t[size / sizeof(wchar_t)];  // size is in bytes
                    if (buffer) {
                        result = func(key, 3, buffer, size, &size);
                        if (result == STATUS_SUCCESS) {
                            buffer[size / sizeof(wchar_t)] = L'\0';
                            keyPath = std::wstring(buffer + 2);
                        }
                        delete[] buffer;
                    }
                }
            }
        }
    }
    return keyPath;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring> SplitStringView(std::wstring_view s,
                                          WCHAR delimiter) {
    size_t pos_start = 0, pos_end;
    std::wstring token;
    std::vector<std::wstring> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + 1;
        res.push_back(std::move(token));
    }

    token = s.substr(pos_start);
    res.push_back(std::move(token));
    return res;
}

std::wstring GetNotifyIconSettingsNameFromRegKey(HKEY hKey) {
    // Example path:
    // \REGISTRY\USER\S-1-5-21-3623811015-3361044348-30300820-1013
    // \Control Panel\NotifyIconSettings\12345678901234567890
    auto path = SplitStringView(GetPathFromHKEY(hKey), L'\\');
    if (path.size() == 7 && path[0].size() == 0 &&
        _wcsicmp(path[1].c_str(), L"REGISTRY") == 0 &&
        _wcsicmp(path[2].c_str(), L"USER") == 0 &&
        _wcsicmp(path[4].c_str(), L"Control Panel") == 0 &&
        _wcsicmp(path[5].c_str(), L"NotifyIconSettings") == 0) {
        return path[6];
    }

    return L"";
}

using RegSetValueExW_t = decltype(&RegSetValueExW);
RegSetValueExW_t RegSetValueExW_Original;
LONG WINAPI RegSetValueExW_Hook(HKEY hKey,
                                LPCWSTR lpValueName,
                                DWORD Reserved,
                                DWORD dwType,
                                CONST BYTE* lpData,
                                DWORD cbData) {
    if (g_settings.mode == Mode::showAll && lpValueName &&
        _wcsicmp(lpValueName, L"IsPromoted") == 0) {
        auto entry = GetNotifyIconSettingsNameFromRegKey(hKey);
        if (!entry.empty()) {
            Wh_Log(L"Setting IsPromoted for %s", entry.c_str());
            return ERROR_SUCCESS;
        }
    }

    return RegSetValueExW_Original(hKey, lpValueName, Reserved, dwType, lpData,
                                   cbData);
}

bool SetIsPromoted(PCWSTR entry) {
    Wh_Log(L"Setting IsPromoted for %s", entry);

    std::wstring subKey = L"Control Panel\\NotifyIconSettings\\";
    subKey += entry;

    HKEY key;
    LONG result =
        RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_SET_VALUE, &key);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open %s: %d", subKey.c_str(), result);
        return false;
    }

    DWORD dw = 1;
    result = RegSetValueExW_Original(key, L"IsPromoted", 0, REG_DWORD,
                                     (const BYTE*)&dw, sizeof(dw));
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"Failed to write to %s: %d", subKey.c_str(), result);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LONG WINAPI RegGetValueW_Hook(HKEY hkey,
                              LPCWSTR lpSubKey,
                              LPCWSTR lpValue,
                              DWORD dwFlags,
                              LPDWORD pdwType,
                              PVOID pvData,
                              LPDWORD pcbData) {
    if (!lpSubKey && lpValue && (dwFlags & RRF_RT_REG_DWORD) && pvData &&
        pcbData && *pcbData >= sizeof(DWORD) &&
        _wcsicmp(lpValue, L"IsPromoted") == 0) {
        auto entry = GetNotifyIconSettingsNameFromRegKey(hkey);
        if (!entry.empty()) {
            Wh_Log(L"Getting IsPromoted for %s", entry.c_str());

            if (g_settings.mode != Mode::showAll) {
                LONG result = RegGetValueW_Original(
                    hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
                if (result != ERROR_FILE_NOT_FOUND) {
                    return result;
                }

                Wh_Log(L"Result of reading original value: %d", result);

                SetIsPromoted(entry.c_str());
            }

            if (pdwType) {
                *pdwType = REG_DWORD;
            }

            *(DWORD*)pvData = 1;
            *pcbData = sizeof(DWORD);
            return ERROR_SUCCESS;
        }
    }

    return RegGetValueW_Original(hkey, lpSubKey, lpValue, dwFlags, pdwType,
                                 pvData, pcbData);
}

// Explorer has a registry watcher for each notify icon item. Trigger the
// watcher by writing and then removing a temporary value.
void TouchAllNotifyIconSettings() {
    constexpr WCHAR kBaseKeyPath[] = L"Control Panel\\NotifyIconSettings";
    constexpr WCHAR kTempValueName[] =
        L"_temp_windhawk_taskbar-notification-icons-show-all";

    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, kBaseKeyPath, 0,
                               KEY_READ | KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open base key: %d", result);
        return;
    }

    DWORD index = 0;
    WCHAR subKeyName[MAX_PATH];
    DWORD subKeyNameSize = MAX_PATH;

    while (RegEnumKeyEx(hKey, index, subKeyName, &subKeyNameSize, nullptr,
                        nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
        HKEY hSubKey;
        result = RegOpenKeyEx(hKey, subKeyName, 0, KEY_WRITE, &hSubKey);
        if (result == ERROR_SUCCESS) {
            if (RegSetValueEx(hSubKey, kTempValueName, 0, REG_SZ,
                              (const BYTE*)L"",
                              sizeof(WCHAR)) != ERROR_SUCCESS) {
                Wh_Log(L"Failed to create temp value: %s", subKeyName);
            } else if (RegDeleteValue(hSubKey, kTempValueName) !=
                       ERROR_SUCCESS) {
                Wh_Log(L"Failed to remove temp value: %s", subKeyName);
            }

            RegCloseKey(hSubKey);
        } else {
            Wh_Log(L"Failed to open subkey: %d", result);
        }

        subKeyNameSize = MAX_PATH;
        index++;
    }

    RegCloseKey(hKey);
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = Mode::showAll;
    if (wcscmp(mode, L"showNew") == 0) {
        g_settings.mode = Mode::showNew;
    }
    Wh_FreeStringSetting(mode);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelBaseModule) {
        Wh_Log(L"kernelBaseModule not found");
        return FALSE;
    }

    FARPROC pRegGetValueW = GetProcAddress(kernelBaseModule, "RegGetValueW");
    if (!pRegGetValueW) {
        Wh_Log(L"RegGetValueW not found");
        return FALSE;
    }

    FARPROC pRegSetValueExW =
        GetProcAddress(kernelBaseModule, "RegSetValueExW");
    if (!pRegSetValueExW) {
        Wh_Log(L"RegSetValueExW not found");
        return FALSE;
    }

    Wh_SetFunctionHook((void*)pRegSetValueExW, (void*)RegSetValueExW_Hook,
                       (void**)&RegSetValueExW_Original);

    Wh_SetFunctionHook((void*)pRegGetValueW, (void*)RegGetValueW_Hook,
                       (void**)&RegGetValueW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    TouchAllNotifyIconSettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    TouchAllNotifyIconSettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    TouchAllNotifyIconSettings();
}
