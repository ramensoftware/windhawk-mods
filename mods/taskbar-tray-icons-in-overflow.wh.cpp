// ==WindhawkMod==
// @id              taskbar-tray-icons-in-overflow
// @name            Always hide all taskbar tray icons in overflow
// @description     Forces all notification area (tray) icons inside the overflow chevron arrow (Windows 11 only)
// @version         1.0
// @author          Hubert
// @github          https://github.com/hlsitechio
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always hide all taskbar tray icons in overflow

Keep your Windows 11 taskbar minimal, clean, and distraction-free by automatically collapsing
all notification area (system tray) icons into the overflow arrow (the `^` chevron flyout).

![Demonstration](https://raw.githubusercontent.com/hlsitechio/windhawk-tray-icons-in-overflow/main/assets/tray-icons-after.png)

### Features
- **All icons inside the arrow**: Ensures every third-party notification tray icon stays inside the overflow flyout rather than cluttering the main taskbar.
- **Configurable modes**:
  - `hideAll` (Default): All notification icons are forced into the overflow flyout.
  - `hideNew`: New notification icons are sent to the overflow flyout by default, leaving existing configurations intact.
- **Zero background overhead**: Hooks the Windows registry queries (`NotifyIconSettings\IsPromoted`) directly inside Explorer without running persistent polling loops.

### Supported Windows Versions
- Windows 11 only (including 22H2, 23H2, and 24H2).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: hideAll
  $name: Mode
  $options:
  - hideAll: All icons are hidden inside the overflow arrow
  - hideNew: New icons are hidden inside the overflow arrow, existing icons are unaffected
*/
// ==/WindhawkModSettings==

#include <ntstatus.h>

#include <string>
#include <string_view>
#include <vector>

enum class Mode {
    hideAll,
    hideNew,
};

struct {
    Mode mode;
} g_settings;

std::wstring GetPathFromHKEY(HKEY key) {
    std::wstring keyPath;
    if (key) {
        HMODULE dll = GetModuleHandleW(L"ntdll.dll");
        if (dll) {
            typedef NTSTATUS(__stdcall * NtQueryKeyType)(
                HANDLE KeyHandle, int KeyInformationClass, PVOID KeyInformation,
                ULONG Length, PULONG ResultLength);
            NtQueryKeyType func =
                (NtQueryKeyType)GetProcAddress(dll, "NtQueryKey");
            if (func) {
                ULONG size = 0;
                NTSTATUS result = func(key, 3, nullptr, 0, &size);
                if (result == STATUS_BUFFER_TOO_SMALL) {
                    size = size + 2;
                    wchar_t* buffer = new (std::nothrow)
                        wchar_t[size / sizeof(wchar_t)];
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
    // Expected path:
    // \REGISTRY\USER\S-1-5-...\Control Panel\NotifyIconSettings\<entry_id>
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
    if (g_settings.mode == Mode::hideAll && lpValueName &&
        _wcsicmp(lpValueName, L"IsPromoted") == 0) {
        auto entry = GetNotifyIconSettingsNameFromRegKey(hKey);
        if (!entry.empty()) {
            Wh_Log(L"Suppressing promotion for %s, keeping inside overflow", entry.c_str());
            return ERROR_SUCCESS;
        }
    }

    return RegSetValueExW_Original(hKey, lpValueName, Reserved, dwType, lpData,
                                   cbData);
}

bool SetIsPromoted(PCWSTR entry, DWORD isPromoted) {
    Wh_Log(L"Writing IsPromoted=%u for %s", isPromoted, entry);

    std::wstring subKey = L"Control Panel\\NotifyIconSettings\\";
    subKey += entry;

    HKEY key;
    LONG result =
        RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_SET_VALUE, &key);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open %s: %d", subKey.c_str(), result);
        return false;
    }

    result = RegSetValueExW_Original(key, L"IsPromoted", 0, REG_DWORD,
                                     (const BYTE*)&isPromoted, sizeof(isPromoted));
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
            Wh_Log(L"Checking IsPromoted for %s", entry.c_str());

            if (g_settings.mode != Mode::hideAll) {
                LONG result = RegGetValueW_Original(
                    hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
                if (result != ERROR_FILE_NOT_FOUND) {
                    return result;
                }

                Wh_Log(L"No existing IsPromoted value found for %s, setting default 0", entry.c_str());
                SetIsPromoted(entry.c_str(), 0);
            }

            if (pdwType) {
                *pdwType = REG_DWORD;
            }

            // 0 = Not promoted, keeps icon inside the overflow chevron arrow
            *(DWORD*)pvData = 0;
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
        L"_temp_windhawk_taskbar-tray-icons-in-overflow";

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
    g_settings.mode = Mode::hideAll;
    if (mode && wcscmp(mode, L"hideNew") == 0) {
        g_settings.mode = Mode::hideNew;
    }
    Wh_FreeStringSetting(mode);
}

BOOL Wh_ModInit() {
    Wh_Log(L"> Init");

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
    Wh_Log(L"> AfterInit");
    TouchAllNotifyIconSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"> Uninit");
    TouchAllNotifyIconSettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> SettingsChanged");
    LoadSettings();
    TouchAllNotifyIconSettings();
}
