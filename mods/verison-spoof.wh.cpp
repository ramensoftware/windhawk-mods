// ==WindhawkMod==
// @id              version-spoof
// @name            Version Spoof
// @description     Fakes the Windows version reported to applications
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @exclude         winlogon.exe
// @exclude         dwm.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Version Spoof
This mod fakes the Windows version reported to applications. This is mainly useful
if you want an application to look how it did on an earlier Windows version, such as
using a native styled titlebar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- spoofs:
  - - path: winver.exe
      $name: Application name or path
    - major: 6
      $name: Major version
    - minor: 1
      $name: Minor version
    - build: 7601
      $name: Build number
    - sp: 1
      $name: Service pack
*/
// ==/WindhawkModSettings==

#include <ntdef.h>
#include <windhawk_utils.h>

struct OSSPOOFINFO {
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwServicePack;
} g_osSpoofInfo = { 0 };
bool g_bSpoofVersion = false;

BOOL (WINAPI *GetVersionExW_orig)(LPOSVERSIONINFOW);
BOOL WINAPI GetVersionExW_hook(
    LPOSVERSIONINFOW lpVersionInformation
)
{
    BOOL bResult = GetVersionExW_orig(lpVersionInformation);
    if (bResult && g_bSpoofVersion)
    {
        Wh_Log(L"Successful GetVersionExW call, spoofing");
        lpVersionInformation->dwMajorVersion = g_osSpoofInfo.dwMajorVersion;
        lpVersionInformation->dwMinorVersion = g_osSpoofInfo.dwMinorVersion;
        lpVersionInformation->dwBuildNumber  = g_osSpoofInfo.dwBuildNumber;
        if (lpVersionInformation->dwOSVersionInfoSize >= sizeof(OSVERSIONINFOEXW))
        {
            ((LPOSVERSIONINFOEXW)lpVersionInformation)->wServicePackMajor
                = g_osSpoofInfo.dwServicePack;
        }
    }
    return bResult;
}

BOOL (WINAPI *GetVersionExA_orig)(LPOSVERSIONINFOA);
BOOL WINAPI GetVersionExA_hook(
    LPOSVERSIONINFOA lpVersionInformation
)
{
    BOOL bResult = GetVersionExA_orig(lpVersionInformation);
    if (bResult && g_bSpoofVersion)
    {
        Wh_Log(L"Successful GetVersionExA call, spoofing");
        lpVersionInformation->dwMajorVersion = g_osSpoofInfo.dwMajorVersion;
        lpVersionInformation->dwMinorVersion = g_osSpoofInfo.dwMinorVersion;
        lpVersionInformation->dwBuildNumber  = g_osSpoofInfo.dwBuildNumber;
        if (lpVersionInformation->dwOSVersionInfoSize >= sizeof(OSVERSIONINFOEXA))
        {
            ((LPOSVERSIONINFOEXA)lpVersionInformation)->wServicePackMajor
                = g_osSpoofInfo.dwServicePack;
        }
    }
    return bResult;
}

DWORD (WINAPI *GetVersion_orig)(void);
DWORD WINAPI GetVersion_hook(void)
{
    DWORD dwVersion = GetVersion_orig();
    if (dwVersion && g_bSpoofVersion)
    {
        Wh_Log(L"Successful GetVersion call, spoofing");
        dwVersion = MAKELONG(
            MAKEWORD(g_osSpoofInfo.dwMajorVersion, g_osSpoofInfo.dwMinorVersion),
            g_osSpoofInfo.dwBuildNumber
        );
    }
    return dwVersion;
}

NTSTATUS (NTAPI *RtlGetVersion_orig)(PRTL_OSVERSIONINFOW);
NTSTATUS NTAPI RtlGetVersion_hook(
    PRTL_OSVERSIONINFOW lpVersionInformation
)
{
    NTSTATUS lStatus = RtlGetVersion_orig(lpVersionInformation);
    if (NT_SUCCESS(lStatus) && g_bSpoofVersion)
    {
        Wh_Log(L"Successful RtlGetVersion call, spoofing");
        lpVersionInformation->dwMajorVersion = g_osSpoofInfo.dwMajorVersion;
        lpVersionInformation->dwMinorVersion = g_osSpoofInfo.dwMinorVersion;
        lpVersionInformation->dwBuildNumber  = g_osSpoofInfo.dwBuildNumber;
        if (lpVersionInformation->dwOSVersionInfoSize >= sizeof(RTL_OSVERSIONINFOEXW))
        {
            ((PRTL_OSVERSIONINFOEXW)lpVersionInformation)->wServicePackMajor
                = g_osSpoofInfo.dwServicePack;
        }
    }
    return lStatus;
}

/**
  * This function provides the fake manifested version for applications
  * (6.2.9200 when unmanifested).
  * 
  * VerifyVersionInfo(W|A) call into it to get the version to compare against.
  * Hooking them is not necessary with this hook in place.
  *
  * This function seems to get compiled into __thiscall on x86, so that's
  * why the calling convention is set as such.
  */
NTSTATUS (__thiscall *SwitchedRtlGetVersion_orig)(PRTL_OSVERSIONINFOW);
NTSTATUS __thiscall SwitchedRtlGetVersion_hook(
    PRTL_OSVERSIONINFOW lpVersionInformation
)
{
    NTSTATUS lStatus = SwitchedRtlGetVersion_orig(lpVersionInformation);
    if (NT_SUCCESS(lStatus) && g_bSpoofVersion)
    {
        Wh_Log(L"Successful SwitchRtlGetVersion call, spoofing");
        lpVersionInformation->dwMajorVersion = g_osSpoofInfo.dwMajorVersion;
        lpVersionInformation->dwMinorVersion = g_osSpoofInfo.dwMinorVersion;
        lpVersionInformation->dwBuildNumber  = g_osSpoofInfo.dwBuildNumber;
        if (lpVersionInformation->dwOSVersionInfoSize >= sizeof(RTL_OSVERSIONINFOEXW))
        {
            ((PRTL_OSVERSIONINFOEXW)lpVersionInformation)->wServicePackMajor
                = g_osSpoofInfo.dwServicePack;
        }
    }
    return lStatus;
}

void UpdateSpoofInfo(void)
{
    g_bSpoofVersion = false;
    ZeroMemory(&g_osSpoofInfo, sizeof(OSSPOOFINFO));

    WCHAR szAppPath[MAX_PATH];
    GetModuleFileNameW(GetModuleHandleW(NULL), szAppPath, MAX_PATH);

    // 1000 entries maximum.
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
            g_bSpoofVersion = true;
            g_osSpoofInfo.dwMajorVersion = Wh_GetIntSetting(L"spoofs[%d].major", i);
            g_osSpoofInfo.dwMinorVersion = Wh_GetIntSetting(L"spoofs[%d].minor", i);
            g_osSpoofInfo.dwBuildNumber  = Wh_GetIntSetting(L"spoofs[%d].build", i);
            g_osSpoofInfo.dwServicePack  = Wh_GetIntSetting(L"spoofs[%d].sp", i);
        }
        Wh_FreeStringSetting(szPath);

        if (g_bSpoofVersion)
        {
            Wh_Log(
                L"Application: %s, spoofing as %d.%d.%d Service Pack %d",
                szAppPath,
                g_osSpoofInfo.dwMajorVersion,
                g_osSpoofInfo.dwMinorVersion,
                g_osSpoofInfo.dwBuildNumber,
                g_osSpoofInfo.dwServicePack
            );
            return;
        }
    }
}

void Wh_ModSettingsChanged(void)
{
    UpdateSpoofInfo();
}

const WindhawkUtils::SYMBOL_HOOK ntdllDllHooks[] = {
    {
        {
#ifdef _WIN64
            L"SwitchedRtlGetVersion"
#else
            L"_SwitchedRtlGetVersion@4"
#endif
        },
        &SwitchedRtlGetVersion_orig,
        SwitchedRtlGetVersion_hook,
        false
    }
};

#define HookFunction(func)            \
if (!Wh_SetFunctionHook(              \
    (void *)func,                     \
    (void *)func ## _hook,            \
    (void **)&func ## _orig           \
))                                    \
{                                     \
    Wh_Log(L"Failed to hook " #func); \
    return FALSE;                     \
}

BOOL Wh_ModInit(void)
{
    UpdateSpoofInfo();

    //Wh_SetFunctionHook((void *)GetVersionExW, (void *)GetVersionExW_hook, (void **)&GetVersionExW_orig);
    HookFunction(GetVersionExW);
    HookFunction(GetVersionExA);
    HookFunction(GetVersion);

    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    void *RtlGetVersion = (void *)GetProcAddress(hNtDll, "RtlGetVersion");
    HookFunction(RtlGetVersion);

    if (!WindhawkUtils::HookSymbols(
        hNtDll,
        ntdllDllHooks,
        ARRAYSIZE(ntdllDllHooks))
    )
    {
        Wh_Log(L"Failed to hook SwitchedRtlGetVersion");
        return FALSE;
    }

    return TRUE;
}