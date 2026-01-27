// ==WindhawkMod==
// @id              app-theme-crash-fixer
// @name            App Theme Crash Fixer
// @description     Prevents applications from crashing when using 3rd party themes by forcing default Windows themes or disabling visual styles per-app.
// @version         1.0.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         *
// @architecture    x86-64
// @compilerOptions -lntdll -lshlwapi -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
 # * App Theme Crash Fixer

 This mod helps fix stability issues in specific applications when using third-party Windows themes (e.g., via SecureUXTheme or UltraUXThemePatcher).

 Some legacy or poorly written applications may crash if they try to load a custom `.msstyles` file that is missing specific resources or metrics. This mod provides two methods to fix these crashes without uninstalling your theme.

 ## Features

 1.  **Force Light Mode (Soft Fix):**
 Spoofs the Registry to tell the application that Windows is running in the default "Light" mode.
 Useful for apps that only crash when "Dark Mode" is active.

 2.  **Disable Visual Styles (Hard Fix):**
 Completely disables the theming engine for the target application.
 The application will look like Windows 98/2000 (Classic Theme), but it will stop crashing.
 Also spoofs `GetCurrentThemeName` to return the default Aero path, preventing the app from attempting to read corrupt theme files.

 ## Usage

 Go to the **Settings** tab and add a new entry for each problematic application:
 **Executable Name:** The process name (e.g., `program.exe`).
 **Disable Visual Styles:** Check this if the app crashes on startup.
 **Force Light Mode:** Check this if the app looks broken in Dark Mode.
 */
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
 - * apps:
 - - exeName: example.exe
 $name: Executable Name
 $description: The process name of the application (e.g., notepad.exe).
 - disableTheming: true
 $name: Disable Visual Styles
 $description: Forces the application to use Windows Classic styling. Recommended if the app crashes immediately with custom themes.
 - forceLight: true
 $name: Force Light Mode
 $description: Spoofs the registry to make the application believe Windows is in Light Mode.
 $name: Target Applications
 */
// ==/WindhawkModSettings==

#include <ntdef.h>
#include <ntstatus.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <strsafe.h>

// Global Settings
bool g_bIsTargetApp = false;
bool g_bForceLight = false;
bool g_bDisableTheming = false;

// ---------------------------------------------------------------------------
// Helpers and Definitions
// ---------------------------------------------------------------------------

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation,
    KeyCachedInformation,
    KeyFlagsInformation,
    KeyVirtualizationInformation,
    KeyHandleTagsInformation,
    KeyTrustInformation,
    KeyLayerInformation,
    MaxKeyInfoClass
} KEY_INFORMATION_CLASS;

typedef struct _KEY_NAME_INFORMATION {
    ULONG NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

EXTERN_C NTSYSAPI NTSTATUS NTAPI NtQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
);

bool EndsWith(const wchar_t *str, const wchar_t *suffix) {
    if (!str || !suffix) return false;
    size_t lenstr = wcslen(str);
    size_t lensuffix = wcslen(suffix);
    if (lensuffix > lenstr) return false;
    return 0 == _wcsnicmp(str + lenstr - lensuffix, suffix, lensuffix);
}

// ---------------------------------------------------------------------------
// Registry Hooks (Light/Dark Mode Spoofing)
// ---------------------------------------------------------------------------

void ApplyThemeSpoof(LSTATUS lStatus, HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValueName, LPDWORD lpcbData, bool fAnsi, LPDWORD lpData) {
    // Only proceed if it is the target app, Light Mode is forced, and the query was successful
    if (!g_bIsTargetApp || !g_bForceLight || lStatus != ERROR_SUCCESS || !hkey || !lpValueName || !lpcbData || !lpData)
        return;

    // Check if the query is about theme preference
    bool fIsTargetValue;
    if (fAnsi)
        fIsTargetValue = (0 == _stricmp((LPCSTR)lpValueName, "AppsUseLightTheme")) || (0 == _stricmp((LPCSTR)lpValueName, "SystemUsesLightTheme"));
    else
        fIsTargetValue = (0 == _wcsicmp(lpValueName, L"AppsUseLightTheme")) || (0 == _wcsicmp(lpValueName, L"SystemUsesLightTheme"));

    if (!fIsTargetValue) return;

    // Validate the key path to ensure we are only spoofing the personalization key
    ULONG ulAlloc = 0;
    NTSTATUS status;
    HKEY hkeyQuery = hkey;
    bool fOpenedSubKey = false;
    KEY_NAME_INFORMATION *pNameInfo = nullptr;

    if (lpSubKey && (fAnsi ? *(LPCSTR)lpSubKey : *lpSubKey)) {
        hkeyQuery = NULL;
        if (fAnsi) RegOpenKeyExA(hkey, (LPCSTR)lpSubKey, 0, KEY_READ, &hkeyQuery);
        else RegOpenKeyExW(hkey, lpSubKey, 0, KEY_READ, &hkeyQuery);
        if (!hkeyQuery) return;
        fOpenedSubKey = true;
    }

    status = NtQueryKey(hkeyQuery, KeyNameInformation, nullptr, 0, &ulAlloc);
    if (status == STATUS_BUFFER_TOO_SMALL) {
        ulAlloc += sizeof(WCHAR);
        pNameInfo = (KEY_NAME_INFORMATION *)LocalAlloc(LPTR, ulAlloc);
        if (pNameInfo) {
            status = NtQueryKey(hkeyQuery, KeyNameInformation, pNameInfo, ulAlloc, &ulAlloc);
            if (status == STATUS_SUCCESS) {
                if (EndsWith(pNameInfo->Name, L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize")) {
                    *lpData = 1; // Force value to 1 (Light Mode)
                }
            }
            LocalFree(pNameInfo);
        }
    }

    if (fOpenedSubKey) RegCloseKey(hkeyQuery);
}

// Hook definitions for Registry functions
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_orig;
LSTATUS WINAPI RegQueryValueExW_hook(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    LSTATUS lStatus = RegQueryValueExW_orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    ApplyThemeSpoof(lStatus, hKey, nullptr, lpValueName, lpcbData, false, (LPDWORD)lpData);
    return lStatus;
}

using RegQueryValueExA_t = decltype(&RegQueryValueExA);
RegQueryValueExA_t RegQueryValueExA_orig;
LSTATUS WINAPI RegQueryValueExA_hook(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    LSTATUS lStatus = RegQueryValueExA_orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    ApplyThemeSpoof(lStatus, hKey, nullptr, (LPCWSTR)lpValueName, lpcbData, true, (LPDWORD)lpData);
    return lStatus;
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_orig;
LSTATUS WINAPI RegGetValueW_hook(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    LSTATUS lStatus = RegGetValueW_orig(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    ApplyThemeSpoof(lStatus, hkey, lpSubKey, lpValue, pcbData, false, (LPDWORD)pvData);
    return lStatus;
}

using RegGetValueA_t = decltype(&RegGetValueA);
RegGetValueA_t RegGetValueA_orig;
LSTATUS WINAPI RegGetValueA_hook(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    LSTATUS lStatus = RegGetValueA_orig(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    ApplyThemeSpoof(lStatus, hkey, (LPCWSTR)lpSubKey, (LPCWSTR)lpValue, pcbData, true, (LPDWORD)pvData);
    return lStatus;
}

// ---------------------------------------------------------------------------
// Theme API Spoofing (GetCurrentThemeName)
// ---------------------------------------------------------------------------

// If the app asks "Which theme file am I using?", we lie and say "Aero.msstyles".
// This prevents the app from trying to load resources from a potentially corrupt custom theme.
using GetCurrentThemeName_t = decltype(&GetCurrentThemeName);
GetCurrentThemeName_t GetCurrentThemeName_orig;
HRESULT WINAPI GetCurrentThemeName_hook(LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars) {

    if (g_bIsTargetApp && g_bDisableTheming) {
        // Spoof the default Windows Aero theme path
        if (pszThemeFileName) {
            StringCchCopyW(pszThemeFileName, dwMaxNameChars, L"C:\\Windows\\Resources\\Themes\\Aero\\Aero.msstyles");
        }
        if (pszColorBuff) {
            StringCchCopyW(pszColorBuff, cchMaxColorChars, L"NormalColor");
        }
        if (pszSizeBuff) {
            StringCchCopyW(pszSizeBuff, cchMaxSizeChars, L"NormalSize");
        }
        return S_OK;
    }

    return GetCurrentThemeName_orig(pszThemeFileName, dwMaxNameChars, pszColorBuff, cchMaxColorChars, pszSizeBuff, cchMaxSizeChars);
}

// ---------------------------------------------------------------------------
// Initialization and Settings
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_bIsTargetApp = false;
    g_bForceLight = false;
    g_bDisableTheming = false;

    WCHAR szAppPath[MAX_PATH];
    GetModuleFileNameW(NULL, szAppPath, MAX_PATH);
    WCHAR *pExeName = wcsrchr(szAppPath, L'\\');
    if (pExeName) pExeName++; else pExeName = szAppPath;

    for (int i = 0;; i++) {
        LPCWSTR settingExeName = Wh_GetStringSetting(L"apps[%d].exeName", i);
        if (!*settingExeName) {
            Wh_FreeStringSetting(settingExeName);
            break;
        }

        // Check if the current process matches the setting (Case insensitive)
        if (_wcsicmp(pExeName, settingExeName) == 0) {
            g_bIsTargetApp = true;
            g_bDisableTheming = Wh_GetIntSetting(L"apps[%d].disableTheming", i);
            g_bForceLight = Wh_GetIntSetting(L"apps[%d].forceLight", i);

            Wh_Log(L"Target App Detected: %s | Theming Disabled: %d | Force Light: %d", pExeName, g_bDisableTheming, g_bForceLight);
        }

        Wh_FreeStringSetting(settingExeName);
        if (g_bIsTargetApp) break;
    }
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
    // Note: Disabling theming might require a restart of the target application to take full effect.
}

BOOL Wh_ModInit(void) {
    LoadSettings();

    // Optimization: If this is not the target app, do not install any hooks.
    // This is critical since we use @include *
    if (!g_bIsTargetApp) return FALSE;

    // STEP 1: DISABLE VISUAL STYLES (NUCLEAR OPTION)
    // If enabled, this completely disables the theme engine for this process.
    // The app will look like Windows 98/Classic, but it will be stable.
    if (g_bDisableTheming) {
        // SetThemeAppProperties(0) -> Disables all visual styles (STAP_ALLOW_NONCLIENT, etc.)
        // We load it dynamically to ensure safety.
        HMODULE hUxTheme = LoadLibraryExW(L"UxTheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hUxTheme) {
            typedef void (WINAPI *SetThemeAppProperties_t)(DWORD);
            SetThemeAppProperties_t pSetThemeAppProperties = (SetThemeAppProperties_t)GetProcAddress(hUxTheme, "SetThemeAppProperties");
            if (pSetThemeAppProperties) {
                pSetThemeAppProperties(0);
                Wh_Log(L"Visual styles disabled via SetThemeAppProperties(0).");
            }
        }
    }

    // STEP 2: REGISTRY HOOKS (Light Mode Spoof)
    // Only strictly necessary if forceLight is enabled, but good to keep hooked in case of dynamic changes.
    Wh_SetFunctionHook((void *)RegQueryValueExW, (void *)RegQueryValueExW_hook, (void **)&RegQueryValueExW_orig);
    Wh_SetFunctionHook((void *)RegQueryValueExA, (void *)RegQueryValueExA_hook, (void **)&RegQueryValueExA_orig);
    Wh_SetFunctionHook((void *)RegGetValueW, (void *)RegGetValueW_hook, (void **)&RegGetValueW_orig);
    Wh_SetFunctionHook((void *)RegGetValueA, (void *)RegGetValueA_hook, (void **)&RegGetValueA_orig);

    // STEP 3: THEME NAME SPOOF
    // Hook GetCurrentThemeName to prevent the app from discovering the custom .msstyles file path.
    HMODULE hUxTheme = LoadLibraryExW(L"UxTheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hUxTheme) {
        void *pGetCurrentThemeName = (void *)GetProcAddress(hUxTheme, "GetCurrentThemeName");
        if (pGetCurrentThemeName) {
            Wh_SetFunctionHook(pGetCurrentThemeName, (void *)GetCurrentThemeName_hook, (void **)&GetCurrentThemeName_orig);
        }
    }

    return TRUE;
}
