// ==WindhawkMod==
// @id              transparent-screensaver-fix
// @name            Transparent Screensaver Fix
// @description     Allows transparent screensavers to see your desktop again
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         winlogon.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Transparent Screensaver Fix
This mod allows transparent screensavers, such as the Bubbles screensaver, to see
your desktop again, rather than displaying a solid color.

# IMPORTANT: READ!
Windhawk needs to hook into `winlogon.exe` for this mod to work. Please navigate
to Windhawk's Settings, Advanced settings, More advanced settings, and make sure
that `winlogon.exe` is in the Process inclusion list.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/transparent-screensaver-fix-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/transparent-screensaver-fix-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

bool g_bStartingScreenSaver = false;

/* Only prevent desktop switch when screensaver is being started */
DWORD (*RunScreenSaver_orig)(struct _WLSM_GLOBAL_CONTEXT *, struct SCREEN_SAVER_DATA *, struct _WINLOGON_JOB **);
DWORD RunScreenSaver_hook(
    struct _WLSM_GLOBAL_CONTEXT *pContext,
    struct SCREEN_SAVER_DATA    *pData,
    struct _WINLOGON_JOB        **ppJob
)
{
    g_bStartingScreenSaver = true;
    DWORD result = RunScreenSaver_orig(
        pContext, pData, ppJob
    );
    g_bStartingScreenSaver = false;
    return result;
}

/* Prevent screensaver from switching desktop */
void (*CSession_SwitchDesktop_orig)(void *, UINT, int *, DWORD, DWORD);
void CSession_SwitchDesktop_hook(
    void  *pThis,
    UINT   a1,
    int   *a2,
    DWORD  a3,
    DWORD  a4
)
{
    if (!g_bStartingScreenSaver)
        return CSession_SwitchDesktop_orig(pThis, a1, a2, a3, a4);
}

/* Create screensaver on the user desktop */
DWORD (*CUser_CreateProcessW_orig)(void *, LPCWSTR, LPWSTR, int, STARTUPINFOW *, PROCESS_INFORMATION *, int);
DWORD CUser_CreateProcessW_hook(
    void                *pThis,
    LPCWSTR              a2,
    LPWSTR               a3,
    int                  a4,
    STARTUPINFOW        *psi,
    PROCESS_INFORMATION *a6,
    int                  a7
)
{
    if (psi && psi->lpDesktop && psi->lpDesktop[0]
    && 0 == wcsicmp(psi->lpDesktop, L"Winsta0\\Screen-saver"))
    {
        psi->lpDesktop = nullptr;
    }
    return CUser_CreateProcessW_orig(
        pThis, a2, a3, a4, psi, a6, a7
    );
}

const WindhawkUtils::SYMBOL_HOOK winlogonExeHooks[] = {
    {
        {
            L"unsigned long __cdecl RunScreenSaver(struct _WLSM_GLOBAL_CONTEXT *,struct SCREEN_SAVER_DATA *,struct _WINLOGON_JOB * *)"
        },
        &RunScreenSaver_orig,
        RunScreenSaver_hook,
        false
    },
    {
        {
            L"public: void __cdecl CSession::SwitchDesktop(enum _DESKTOPID,int *,unsigned long,unsigned long)"
        },
        &CSession_SwitchDesktop_orig,
        CSession_SwitchDesktop_hook,
        false
    },
    {
        {
            L"public: unsigned long __cdecl CUser::CreateProcessW(unsigned short const *,unsigned short *,unsigned long,struct _STARTUPINFOW *,struct _PROCESS_INFORMATION *,int)"
        },
        &CUser_CreateProcessW_orig,
        CUser_CreateProcessW_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        winlogonExeHooks,
        ARRAYSIZE(winlogonExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in winlogon.exe");
        return FALSE;
    }

    return TRUE;
}