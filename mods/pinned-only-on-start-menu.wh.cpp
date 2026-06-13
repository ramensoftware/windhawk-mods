// ==WindhawkMod==
// @id              pinned-only-on-start-menu
// @name            Pinned Only on Start Menu
// @description     Hides 'All apps' and 'Recommended' sections from the Start Menu.
// @version         1.0.1
// @author          Amat3rassu
// @github          https://github.com/Amat3rassu
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Pinned Only on Start Menu
This mod simplifies the Start Menu by removing secondary sections using native system policies.

### Features:
* **Hides "All apps":** Removes the button and the programs list.
* **Hides "Recommended":** Removes the section with recent files.

### Instructions:
* Enable the mod in Windhawk.
* **Restart explorer.exe** or your computer to apply changes.

---
**Author:** Amat3rassu
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;

/**
 * HOOK: Registry policy enforcement for the Start Menu.
 */
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    if (lpValueName) {
        // Force hide "All apps"
        if (lstrcmpiW(lpValueName, L"NoStartMenuMorePrograms") == 0) {
            if (lpType) *lpType = REG_DWORD;
            if (lpData && lpcbData && *lpcbData >= sizeof(DWORD)) {
                *(DWORD*)lpData = 1;
                *lpcbData = sizeof(DWORD);
            }
            return ERROR_SUCCESS;
        }
        
        // Force hide "Recommended"
        if (lstrcmpiW(lpValueName, L"HideRecommendedSection") == 0) {
            if (lpType) *lpType = REG_DWORD;
            if (lpData && lpcbData && *lpcbData >= sizeof(DWORD)) {
                *(DWORD*)lpData = 1;
                *lpcbData = sizeof(DWORD);
            }
            return ERROR_SUCCESS;
        }
    }
    return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

BOOL Wh_ModInit() {
    HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
    if (kernelBase) {
        Wh_SetFunctionHook(
            (void*)GetProcAddress(kernelBase, "RegQueryValueExW"),
            (void*)RegQueryValueExW_Hook,
            (void**)&RegQueryValueExW_Original
        );
    }
    return TRUE;
}

void Wh_ModUninit() {}
