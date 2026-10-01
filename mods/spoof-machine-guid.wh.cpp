// ==WindhawkMod==
// @id              spoof-machine-guid
// @name            Spoof MachineGuid
// @name:zh-CN      伪造 MachineGuid
// @description     Replaces MachineGuid returned by registry queries for the target process.
// @description:zh-CN 替换目标进程查询注册表时返回的 MachineGuid。
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @include         mspaint.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Spoof MachineGuid

Replaces the `MachineGuid` value returned by registry queries, for the target
process only. The real value on the system is left untouched.

## Choosing the target process

The target process is set by the mod's `@include` metadata field, which
Windhawk uses to decide which processes to inject into. It is set to
`mspaint.exe` (Paint) as a placeholder, so no program is affected until you
change it.

You do not need to edit the source code to change it. Open the mod in Windhawk,
go to **Details** → **Advanced settings**, and put the executable name of your
target in the **process inclusion list** there. The change takes effect the next
time the program starts.

## Settings

- **Fake MachineGuid**: the GUID to return in place of the real one.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fakeGuid: "11111111-2222-3333-4444-555555555555"
  $name: Fake MachineGuid
  $name:zh-CN: 伪造的 MachineGuid
  $description: The GUID to return in place of the real one.
  $description:zh-CN: 填入你想伪造的 GUID
*/
// ==/WindhawkModSettings==


#include <string>

typedef LONG (WINAPI *RegQueryValueExW_t)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
RegQueryValueExW_t originalRegQueryValueExW;

std::wstring g_fakeGuid;

LONG WINAPI HookedRegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    LONG result = originalRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

    if (lpValueName && wcscmp(lpValueName, L"MachineGuid") == 0 && result == ERROR_SUCCESS && lpData && !g_fakeGuid.empty()) {
        size_t len = (g_fakeGuid.size() + 1) * sizeof(wchar_t);
        if (lpcbData && *lpcbData >= len) {
            memcpy(lpData, g_fakeGuid.c_str(), len);
            *lpcbData = (DWORD)len;
        }
    }
    return result;
}

void LoadSettings() {
    PCWSTR fakeGuid = Wh_GetStringSetting(L"fakeGuid");
    g_fakeGuid = fakeGuid ? fakeGuid : L"";
    Wh_FreeStringSetting(fakeGuid);
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_SetFunctionHook((void*)RegQueryValueExW, (void*)HookedRegQueryValueExW, (void**)&originalRegQueryValueExW);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}