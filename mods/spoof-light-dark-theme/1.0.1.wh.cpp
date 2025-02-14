// ==WindhawkMod==
// @id              spoof-light-dark-theme
// @name            Spoof Light/Dark Theme
// @description     Use light/dark theme on an application basis
// @version         1.0.1
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
    if (g_fSpoofTheme && ERROR_SUCCESS == lStatus && lpData && 0 == wcsicmp(lpValueName, L"AppsUseLightTheme"))
    {
        ULONG ulSize = 0;
        NTSTATUS status = NtQueryKey(hKey, KeyNameInformation, nullptr, 0, &ulSize);
        if (status == STATUS_BUFFER_TOO_SMALL)
        {
            ulSize += 2;
            LPWSTR lpBuffer = new WCHAR[ulSize / sizeof(WCHAR)];
            if (lpBuffer)
            {
                status = NtQueryKey(hKey, KeyNameInformation, lpBuffer, ulSize, &ulSize);
                if (status == STATUS_SUCCESS)
                {
                    LPCWSTR lpKeyName = lpBuffer + 2;
                    lpBuffer[ulSize / sizeof(WCHAR)] = L'\0';
                    if (EndsWith(lpKeyName, L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"))
                    {
                        *(LPDWORD)lpData = !g_fDarkTheme;
                    }
                }
                delete[] lpBuffer;
            }
        }
    }
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

        // Does the current application path or name match the spoof?
        WCHAR *pBackslash = wcsrchr(szAppPath, L'\\');
        if (0 == wcsicmp(szAppPath, szPath)
        || (pBackslash && 0 == wcsicmp(pBackslash + 1, szPath)))
        {
            g_fSpoofTheme = true;
            g_fDarkTheme = Wh_GetIntSetting(L"spoofs[%d].dark", i);
        }
        Wh_FreeStringSetting(szPath);

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

BOOL Wh_ModInit(void)
{
    UpdateSpoofInfo();

    Wh_SetFunctionHook(
        (void *)RegQueryValueExW,
        (void *)RegQueryValueExW_hook,
        (void **)&RegQueryValueExW_orig
    );
    void *ShouldAppsUseDarkMode = (void *)GetProcAddress(LoadLibraryW(L"uxtheme.dll"), (LPCSTR)132);
    if (ShouldAppsUseDarkMode)
    {
        Wh_SetFunctionHook(
            (void *)ShouldAppsUseDarkMode,
            (void *)ShouldAppsUseDarkMode_hook,
            (void **)&ShouldAppsUseDarkMode_orig
        );
    }

    return TRUE;
}