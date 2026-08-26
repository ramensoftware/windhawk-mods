// ==WindhawkMod==
// @id              control-panel-revival
// @name            Control Panel Revival
// @description     control panel unhider designed to work smoothly alongside ExplorerPatcher
// @version         1.0.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @include         control.exe
// @compilerOptions -lpsapi -lcomctl32 -lshlwapi
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Control Panel Revival

Control Panel Revival is a lightweight Windhawk mod designed to restore legacy Control Panel applets in Windows 11 by bypassing modern Settings app redirection

**Important! Installing ExplorerPatcher is necessary for this mod to work.**

*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <psapi.h>
#include <shlwapi.h>
#include <windhawk_utils.h>

//GUID list of applets you want to revert to Control Panel

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

bool (*COpenControlPanel__MapLegacyName_orig)(void *, LPCWSTR, LPWSTR, UINT, bool *);
bool COpenControlPanel__MapLegacyName_hook(void *pThis, LPCWSTR pszLegacyName, LPWSTR pszNewName, UINT uUnused, bool *nameChanged) {
    if (nameChanged) *nameChanged = false;
    if (pszNewName) *pszNewName = L'\0';
    return false;
}

using CompareStringOrdinal_t = decltype(&CompareStringOrdinal);
CompareStringOrdinal_t CompareStringOrdinal_orig;
int WINAPI CompareStringOrdinal_hook(LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, BOOL bIgnoreCase) {
    if (!lpString1 || !lpString2) return ERROR_INVALID_PARAMETER;

    auto pCompFunc = bIgnoreCase ? wcsicmp : wcscmp;
    for (UINT i = 0; i < ARRAYSIZE(g_szAppletsToUnhide); i++) {
        if (0 == pCompFunc(lpString1, g_szAppletsToUnhide[i]) || 0 == pCompFunc(lpString2, g_szAppletsToUnhide[i])) {
            return CSTR_LESS_THAN; 
        }
    }
    for (UINT i = 0; i < ARRAYSIZE(g_szCanonicalNames); i++) {
        if (0 == pCompFunc(lpString1, g_szCanonicalNames[i]) || 0 == pCompFunc(lpString2, g_szCanonicalNames[i])) {
            return CSTR_LESS_THAN;
        }
    }
    return CompareStringOrdinal_orig(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
}

void KillStringInModule(HMODULE hModule, LPCWSTR lpSearch) {
    if (!hModule || !lpSearch) return;

    MODULEINFO info = { 0 };
    GetModuleInformation(GetCurrentProcess(), hModule, &info, sizeof(MODULEINFO));

    DWORD_PTR base = (size_t)info.lpBaseOfDll;
    size_t size = (size_t)info.SizeOfImage;
    size_t patternLen = wcslen(lpSearch) * 2;

    for (size_t i = 0; i < size - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (*((char *)lpSearch + j) != *(char *)(base + i + j)) {
                found = false;
                break;
            }
        }

        if (found) {
            size_t ptr = base + i;
            MEMORY_BASIC_INFORMATION mbi;
            VirtualQuery((wchar_t *)ptr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
            if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect)) {
                ZeroMemory((void *)ptr, patternLen); 
                DWORD dwOldProtect;
                VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);
                return;
            }
        }
    }
}

// explorer.exe, control.exe
const WindhawkUtils::SYMBOL_HOOK explorerControlHooks[] = {
    {
        { L"private: bool __cdecl COpenControlPanel::_MapLegacyName(unsigned short const *,unsigned short *,unsigned int,bool *)" },
    &COpenControlPanel__MapLegacyName_orig,
    COpenControlPanel__Name_hook,
    false
    }
};

BOOL Wh_ModInit(void) {
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    HMODULE hWinStorage = LoadLibraryW(L"windows.storage.dll"); 
    
    for (size_t i = 50; i < ARRAYSIZE(g_szAppletsToUnhide); i++) { 
        // Loop placeholder check handled safely below
    }

    for (size_t i = 0; i < ARRAYSIZE(g_szAppletsToUnhide); i++) {
        if (hShell32) KillStringInModule(hShell32, g_szAppletsToUnhide[i]);
        if (hWinStorage) KillStringInModule(hWinStorage, g_szAppletsToUnhide[i]);
    }

    Wh_SetFunctionHook(
        (void *)CompareStringOrdinal,
        (void *)CompareStringOrdinal_hook,
        (void **)&CompareStringOrdinal_orig
    );

    if (hShell32) {
        WindhawkUtils::HookSymbols(hShell32, explorerControlHooks, ARRAYSIZE(explorerControlHooks));
    }

    return TRUE;
}
