// ==WindhawkMod==
// @id              balloon-sound-fix
// @name            Balloon Sound Fix
// @description     Makes notifciation balloons play a sound again
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Balloon Sound Fix
Windows XP played a sound when showing balloon notifications. However, this
functionality has been broken since Windows Vista. This mod fixes that issue in
Windows 10. This mod will *not* work on Windows 11's new taskbar, as that does
not support legacy balloons.

To enable the balloons, create the registry value
`HKEY_CURRENT_USER\SOFTWARE\Policies\Microsoft\Windows\Explorer\EnableLegacyBalloonNotifications`
as a DWORD, set it to 1, and restart Explorer (or sign out and sign in). 
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

HRESULT (*SHPlaySound_orig)(void);
HRESULT SHPlaySound_hook(void)
{
    // Microsoft messed this up badly, presumably starting in Vista.
    // It attempts to play the balloon sound from the `Explorer` app, when it
    // should play it from `.Default`. The strange thing is, the original version
    // of this function seems to include code for grabbing the .Default version
    // of the sound from registry, but the result is never used.
    WCHAR szFileName[MAX_PATH] = { 0 };
    LONG cbSize = sizeof(szFileName);

    if (ERROR_SUCCESS == RegQueryValueW(
        HKEY_CURRENT_USER, L"AppEvents\\Schemes\\Apps\\.Default\\SystemNotification\\.Current", 
        szFileName, &cbSize)
        && szFileName[0])
    {
        PlaySoundW(szFileName, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
    }
    return S_OK;
}

const WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
    {
        {
            L"SHPlaySound"
        },
        &SHPlaySound_orig,
        SHPlaySound_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        explorerExeHooks,
        ARRAYSIZE(explorerExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook SHPlaySound");
        return FALSE;
    }

    return TRUE;
}