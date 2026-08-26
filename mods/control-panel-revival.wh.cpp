// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival 
// @description     control panel unhider designed to work smoothly alongside ExplorerPatcher
// @version         1.0.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         control.exe
// @compilerOptions -lpsapi
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Control Panel Revival

Control Panel Revival is a lightweight Windhawk mod designed to restore legacy Control Panel applets in Windows 11 by bypassing modern Settings app redirection.

**Important! Installing ExplorerPatcher is necessary for this mod to work.**
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <psapi.h>
#include <windhawk_utils.h>

// GUID list of applets you want to revert to Control Panel
LPCWSTR g_szAppletsToUnhide[] = {
    L"::{BF782CC9-5A52-4A17-806C-2A894FFEEAC5}", // Language Settings
    L"::{ED834ED6-4B5A-4BFE-8F11-A626DCB6A921}", // Personalization
    L"::{D9EF8727-CAC2-4e60-809E-86F80A666C91}", // BitLocker Drive Encryption
    L"::{D17D1D6D-CC3F-4815-8FE3-607E7D5D10B3}", // Text to Speech
    L"::{96AE8D84-A250-4520-95A5-A47A7E3C548B}", // Family Safety
    L"::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}", // System
    L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", // Devices and Printers
    L"::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}", // Installed Updates 
    L"::{17cd9488-1228-4b2f-88ce-4298e93e0966}", // Default Programs 
    L"::{7B81BE6A-CE2B-4676-A29E-EB907A5126C5}", // Programs and Features
    L"::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}", // Troubleshooting
    L"::{BD84B380-8CA2-1069-AB1D-08000948F534}", // Fonts
    L"::{C555438B-3C23-4769-A71F-B6D3D9B6053A}", // Display
    L"::{67CA7650-96E6-4FDD-BB43-A8E774F73A57}"  // Home group
};

LPCWSTR g_szCanonicalNames[] = {
    L"Microsoft.Display",
    L"Microsoft.Personalization",
    L"Microsoft.Language",
    L"Microsoft.LocationAndOtherSensors",
    L"Microsoft.LocationSettings",
    L"Microsoft.WindowsUpdate",
    L"Microsoft.Troubleshooting", 
    L"Microsoft.DevicesAndPrinters",
    L"Microsoft.RegionalAndLanguageOptions",
    L"Microsoft.System",
    L"Microsoft.Printers",
    L"Microsoft.InstalledUpdates",
    L"Microsoft.DefaultPrograms",
    L"Microsoft.Fonts"
};

HMODULE g_hWinStorage = nullptr;

// Safe MapLegacyName hook
bool (*COpenControlPanel__MapLegacyName_orig)(void *, LPCWSTR, LPWSTR, UINT, bool *) = nullptr;
bool COpenControlPanel__MapLegacyName_hook(void *pThis, LPCWSTR pszLegacyName, LPWSTR pszNewName, UINT uUnused, bool *nameChanged) {
    if (pszLegacyName) {
        for (size_t i = 0; i < ARRAYSIZE(g_szCanonicalNames); i++) {
            if (g_szCanonicalNames[i] && _wcsicmp(pszLegacyName, g_szCanonicalNames[i]) == 0) {
                if (nameChanged) *nameChanged = false;
                if (pszNewName && uUnused > 0) {
                    *pszNewName = L'\0';
                }
                return false;
            }
        }
    }
    return COpenControlPanel__MapLegacyName_orig(pThis, pszLegacyName, pszNewName, uUnused, nameChanged);
}

// Safe CompareStringOrdinal hook targeting kernelbase.dll
decltype(&CompareStringOrdinal) CompareStringOrdinal_orig = nullptr;
int WINAPI CompareStringOrdinal_hook(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase) {
    if (!lpString1 || !lpString2) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (cchCount1 == -1 && cchCount2 == -1) {
        auto pCompFunc = bIgnoreCase ? _wcsicmp : wcscmp;
        for (size_t i = 0; i < ARRAYSIZE(g_szAppletsToUnhide); i++) {
            if (g_szAppletsToUnhide[i] && (0 == pCompFunc(lpString1, g_szAppletsToUnhide[i]) || 0 == pCompFunc(lpString2, g_szAppletsToUnhide[i]))) {
                return CSTR_LESS_THAN; 
            }
        }
        for (size_t i = 0; i < ARRAYSIZE(g_szCanonicalNames); i++) {
            // Fixed parenthesis error here
            if (g_szCanonicalNames[i] && (0 == pCompFunc(lpString1, g_szCanonicalNames[i]) || 0 == pCompFunc(lpString2, g_szCanonicalNames[i]))) {
                return CSTR_LESS_THAN;
            }
        }
    }

    if (!CompareStringOrdinal_orig) {
        return 0;
    }

    return CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        { L"private: bool __cdecl COpenControlPanel::_MapLegacyName(unsigned short const *,unsigned short *,unsigned int,bool *)" },
        (void**)&COpenControlPanel__MapLegacyName_orig,
        (void*)COpenControlPanel__MapLegacyName_hook,
        false
    }
};

BOOL Wh_ModInit(void) {
    Wh_Log(L"Initializing Control Panel Revival mod");

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"Failed to get handle for shell32.dll");
        return FALSE;
    }

    g_hWinStorage = LoadLibraryExW(L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBaseModule) {
        Wh_Log(L"Failed to get handle for kernelbase.dll");
        return FALSE;
    }

    auto pCompareStringOrdinal = (decltype(&CompareStringOrdinal))GetProcAddress(kernelBaseModule, "CompareStringOrdinal");
    if (!pCompareStringOrdinal) {
        Wh_Log(L"Failed to resolve CompareStringOrdinal address");
        return FALSE;
    }

    // Fixed SetFunctionHook types without casting to void*
    if (!WindhawkUtils::SetFunctionHook(pCompareStringOrdinal, CompareStringOrdinal_hook, &CompareStringOrdinal_orig)) {
        Wh_Log(L"Failed to hook CompareStringOrdinal");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to hook shell32 symbols");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit(void) {
    Wh_Log(L"Uninitializing Control Panel Revival mod");
    if (g_hWinStorage) {
        FreeLibrary(g_hWinStorage);
        g_hWinStorage = nullptr;
    }
}
