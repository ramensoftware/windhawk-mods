// ==WindhawkMod==
// @id              spoof-light-dark-theme
// @name            Spoof Light/Dark Theme
// @description     Use light/dark theme on an application basis
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lntdll
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- spoofs:
  - - path: explorer.exe
      $name: Application name or path
    - dark: false
      $name: Dark theme
  $name: Spoofs
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Spoof Light/Dark Theme
This mod lets you make specific applications believe you are using either light or dark theme,
regardless of your actual Windows setting.
*/
// ==/WindhawkModReadme==

#include <ntdef.h>
#include <ntstatus.h>

// Spoof?
bool g_fSpoofTheme = false;
// If spoofing, this dictates whether we're spoofing light or dark.
bool g_fDarkTheme = false;

typedef enum _KEY_INFORMATION_CLASS
{
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

typedef struct _KEY_NAME_INFORMATION
{
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

bool EndsWith(const wchar_t *str, const wchar_t *suffix)
{
    if (!str || !suffix)
        return false;
    size_t lenstr = wcslen(str);
    size_t lensuffix = wcslen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return 0 == wcsnicmp(str + lenstr - lensuffix, suffix, lensuffix);
}

/**
  * Applies the dark mode spoof to the arguments of a registry call.
  * 
  * If fAnsi is set, lpSubKey and lpValueName will be treated as ANSI
  * strings (LPCSTR)
  */
void ApplyDarkModeSpoof(
    LSTATUS lStatus,
    HKEY    hkey,
    LPCWSTR lpSubKey,
    LPCWSTR lpValueName,
    LPDWORD lpcbData,
    bool    fAnsi,
    LPDWORD lpData
)
{
    ULONG ulAlloc = 0;
    NTSTATUS status;
    KEY_NAME_INFORMATION *pNameInfo = nullptr;
    HKEY hkeyQuery = hkey;
    bool fOpenedSubKey = false;

    if (!g_fSpoofTheme || lStatus != ERROR_SUCCESS || !hkey || !lpValueName || !lpcbData || !lpData)
        return;

    // Check value name
    bool fIsLightThemeValue;
    if (fAnsi)
        fIsLightThemeValue = !stricmp((LPCSTR)lpValueName, "AppsUseLightTheme");
    else
        fIsLightThemeValue = !wcsicmp(lpValueName, L"AppsUseLightTheme");

    if (!fIsLightThemeValue)
        goto cleanup;

    if (lpSubKey && (fAnsi ? *(LPCSTR)lpSubKey : *lpSubKey))
    {
        hkeyQuery = NULL;
        if (fAnsi)
            RegOpenKeyExA(hkey, (LPCSTR)lpSubKey, 0, KEY_READ, &hkeyQuery);
        else
            RegOpenKeyExW(hkey, lpSubKey, 0, KEY_READ, &hkeyQuery);
        if (!hkeyQuery)
            goto cleanup;
        fOpenedSubKey = true;
    }

    // Check key name
    status = NtQueryKey(hkeyQuery, KeyNameInformation, nullptr, 0, &ulAlloc);
    if (status != STATUS_BUFFER_TOO_SMALL)
        goto cleanup;

    // Add space for null terminator and allocate
    ulAlloc += sizeof(WCHAR);
    pNameInfo = (KEY_NAME_INFORMATION *)LocalAlloc(LPTR, ulAlloc);
    if (!pNameInfo)
        goto cleanup;
    ZeroMemory(pNameInfo, ulAlloc);

    status = NtQueryKey(hkeyQuery, KeyNameInformation, pNameInfo, ulAlloc, &ulAlloc);
    if (status != STATUS_SUCCESS)
        goto cleanup;

    if (!EndsWith(pNameInfo->Name, L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"))
        goto cleanup;

    *lpData = !g_fDarkTheme;

cleanup:
    if (pNameInfo)
        LocalFree(pNameInfo);

    if (fOpenedSubKey)
        RegCloseKey(hkeyQuery);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_orig;
LSTATUS WINAPI RegQueryValueExW_hook(
    HKEY    hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE  lpData,
    LPDWORD lpcbData
)
{
    LSTATUS lStatus = RegQueryValueExW_orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    ApplyDarkModeSpoof(lStatus, hKey, nullptr, lpValueName, lpcbData, false, (LPDWORD)lpData);
    return lStatus;
}

using RegQueryValueExA_t = decltype(&RegQueryValueExA);
RegQueryValueExA_t RegQueryValueExA_orig;
LSTATUS WINAPI RegQueryValueExA_hook(
    HKEY    hKey,
    LPCSTR  lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE  lpData,
    LPDWORD lpcbData
)
{
    LSTATUS lStatus = RegQueryValueExA_orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    ApplyDarkModeSpoof(lStatus, hKey, nullptr, (LPCWSTR)lpValueName, lpcbData, true, (LPDWORD)lpData);
    return lStatus;
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_orig;
LSTATUS WINAPI RegGetValueW_hook(
    HKEY    hkey,
    LPCWSTR lpSubKey,
    LPCWSTR lpValue,
    DWORD   dwFlags,
    LPDWORD pdwType,
    PVOID   pvData,
    LPDWORD pcbData
)
{
    LSTATUS lStatus = RegGetValueW_orig(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    ApplyDarkModeSpoof(lStatus, hkey, lpSubKey, lpValue, pcbData, false, (LPDWORD)pvData);
    return lStatus;
}

using RegGetValueA_t = decltype(&RegGetValueA);
RegGetValueA_t RegGetValueA_orig;
LSTATUS WINAPI RegGetValueA_hook(
    HKEY    hkey,
    LPCSTR  lpSubKey,
    LPCSTR  lpValue,
    DWORD   dwFlags,
    LPDWORD pdwType,
    PVOID   pvData,
    LPDWORD pcbData
)
{
    LSTATUS lStatus = RegGetValueA_orig(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    ApplyDarkModeSpoof(lStatus, hkey, (LPCWSTR)lpSubKey, (LPCWSTR)lpValue, pcbData, true, (LPDWORD)pvData);
    return lStatus;
}

bool (WINAPI *ShouldAppsUseDarkMode_orig)(void);
bool WINAPI ShouldAppsUseDarkMode_hook(void)
{
    return g_fSpoofTheme ? g_fDarkTheme : ShouldAppsUseDarkMode_orig();
}

void UpdateSpoofInfo(void)
{
    g_fSpoofTheme = false;
    g_fDarkTheme = false;

    WCHAR szAppPath[MAX_PATH];
    GetModuleFileNameW(GetModuleHandleW(NULL), szAppPath, MAX_PATH);

    for (int i = 0;; i++)
    {
        LPCWSTR szPath = Wh_GetStringSetting(L"spoofs[%d].path", i);
        // Stop enumerating at an empty string
        if (!*szPath)
        {
            Wh_FreeStringSetting(szPath);
            break;
        }

        WCHAR szExpandedPath[MAX_PATH];
        ExpandEnvironmentStringsW(szPath, szExpandedPath, ARRAYSIZE(szExpandedPath));
        Wh_FreeStringSetting(szPath);

        // Does the current application path or name match the spoof?
        WCHAR *pBackslash = wcsrchr(szAppPath, L'\\');
        if (0 == wcsicmp(szAppPath, szExpandedPath)
        || (pBackslash && 0 == wcsicmp(pBackslash + 1, szExpandedPath)))
        {
            g_fSpoofTheme = true;
            g_fDarkTheme = Wh_GetIntSetting(L"spoofs[%d].dark", i);
        }

        if (g_fSpoofTheme)
        {
            Wh_Log(
                L"Application: %s, spoofing as %s theme",
                szAppPath, g_fDarkTheme ? L"dark" : L"light"
            );
            break;
        }
    }
}

void Wh_ModSettingsChanged(void)
{
    UpdateSpoofInfo();
}

#define HOOK(func)                                                                         \
    if (!Wh_SetFunctionHook((void *)func, (void *)func ## _hook, (void **)&func ## _orig)) \
    {                                                                                      \
        Wh_Log(L"Failed to hook %s", L ## #func);                                          \
        return FALSE;                                                                      \
    }

BOOL Wh_ModInit(void)
{
    UpdateSpoofInfo();

    HOOK(RegQueryValueExW);
    HOOK(RegQueryValueExA);
    HOOK(RegGetValueW);
    HOOK(RegGetValueA);

    HMODULE hUxTheme = LoadLibraryExW(L"UxTheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hUxTheme)
    {
        Wh_Log(L"Failed to load UxTheme.dll");
        return FALSE;
    }

    void *ShouldAppsUseDarkMode = (void *)GetProcAddress(hUxTheme, (LPCSTR)132);
    if (ShouldAppsUseDarkMode)
    {
        HOOK(ShouldAppsUseDarkMode);
    }
    else
    {
        Wh_Log(L"Failed to get address of ShouldAppsUseDarkMode");
        return FALSE;
    }

    return TRUE;
}