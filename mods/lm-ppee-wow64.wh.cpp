// ==WindhawkMod==
// @id              lm-ppee-wow64
// @name            Fix PPEE/ResourceHacker Wow64 filesystem redirection
// @description     Disables Wow64 filesystem redirection when loading a file in PPEE or ResourceHacker
// @version         1.2
// @author          Mark Jansen
// @github          https://github.com/learn-more
// @twitter         https://twitter.com/learn_more
// @include         ppee.exe
// @include         resourcehacker.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix Wow64 filesystem redirection for PPEE and ResourceHacker

When loading a file from `C:\Windows\System32`, PPEE would automatically be redirected to `C:\Windows\SysWOW64`.

This mod disables that redirection, so that the correct file is loaded.

## Before:
![before](https://i.imgur.com/pJx8oPy.png)

## After:
![after](https://i.imgur.com/DBmkYJe.png)


*/
// ==/WindhawkModReadme==

static BOOL g_fRevert = FALSE;
static PVOID g_Cookie = NULL;

BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    g_fRevert = Wow64DisableWow64FsRedirection(&g_Cookie);
    Wh_Log(L" => Disable FS redirection: %d", g_fRevert);

    return TRUE;
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Uninit " WH_MOD_ID );
    if (g_fRevert)
        Wow64RevertWow64FsRedirection(g_Cookie);
}
