// ==WindhawkMod==
// @id              console-error-to-ding
// @name            Console Error to Ding
// @description     Replaces the error sound in console applications with the ding
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         conhost.exe
// @compilerOptions -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Console Error to Ding
Certain actions in console applications can play an error sound. This mod replaces
that with the ding sound, which is often more accurate in the context of the application.
*/
// ==/WindhawkModReadme==

using PlaySoundW_t = decltype(&PlaySoundW);
PlaySoundW_t PlaySoundW_orig;
BOOL WINAPI PlaySoundW_hook(
    LPCWSTR pszSound,
    HMODULE hmod,
    DWORD   fdwSound
)
{
    if (pszSound == (LPCWSTR)SND_ALIAS_SYSTEMHAND)
    {
        pszSound = (LPCWSTR)SND_ALIAS_SYSTEMDEFAULT;
    }
    return PlaySoundW_orig(pszSound, hmod, fdwSound);
}

BOOL Wh_ModInit(void)
{
    Wh_SetFunctionHook(
        (void *)PlaySoundW,
        (void *)PlaySoundW_hook,
        (void **)&PlaySoundW_orig
    );
    return TRUE;
}