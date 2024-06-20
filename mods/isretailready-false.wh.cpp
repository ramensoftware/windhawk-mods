// ==WindhawkMod==
// @id              isretailready-false
// @name            SecureBoot Warning Watermark Enabler
// @description     Enables the normally hidden "SecureBoot isn't configured correctly" watermark
// @version         1
// @author          Jevil7452
// @github          https://github.com/Jevil7452
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Certain versions of Windows starting with 8 have code for a SecureBoot watermark, which is usually hidden in normal circumstances, even if SecureBoot is off. It can be forcefully enabled by making a function (CDesktopWatermark::s_IsRetailReady) return false.
![Watermark](https://i.imgur.com/k0giME6.png)

It may not work with all versions of Windows.

Tested on:
- 1703 (Windhawk Mod) - worked fine
- 1803 (shell32.dll patch) - worked fine
- 1809 (Windhawk Mod) - worked fine
- 21H2 (Windhawk Mod) - the watermark enabled but the warning wasn't there - missing text resources?
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __thiscall
#define SCALCON L"__thiscall"
#endif

typedef BOOL (* WINAPI CDesktopWatermark__s_IsRetailReady_t)(void *);
CDesktopWatermark__s_IsRetailReady_t CDesktopWatermark__s_IsRetailReady_orig;
BOOL WINAPI CDesktopWatermark__s_IsRetailReady_hook(void *pThis)
{
    //Wh_Log(L"Got here");
    return FALSE;
    }
//It seems like it's the same function that would control the Microsoft Confidential watermark in builds that have it

BOOL Wh_ModInit(void)
{   
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {

        {
            {
                L"private: static bool "
                SCALCON
                L" CDesktopWatermark::s_IsRetailReady(void)"
            },
            &CDesktopWatermark__s_IsRetailReady_orig,
            CDesktopWatermark__s_IsRetailReady_hook,
            false
        }
    };

    if (!HookSymbols(hShell32, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"Failed to hook one or more symbol functions");
        return FALSE;
    }
    return TRUE;
}
