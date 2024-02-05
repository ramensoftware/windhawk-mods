// ==WindhawkMod==
// @id              syslistview32-enabler
// @name            Enable SyslistView32
// @description     Enables SysListView32 folder layout in Explorer
// @version         1.0.1
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Enables the SysListView32 control in File Explorer folder windows.
This makes the view more compact and allows icon rearrangement.
SysListView32 control has been used by default before Windows 7.

Before:
![Default view](https://i.imgur.com/rPpiFEU.png)

After:
![SysListView32 mode](https://i.imgur.com/oqYf1YW.png)

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

/* SysListView32 */
typedef BOOL (* CALCON CDefView__UseItemsView_t)(void *);
CDefView__UseItemsView_t CDefView__UseItemsView_orig;
BOOL CALCON CDefView__UseItemsView_hook(void *pThis)
{return FALSE;}


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
                L"private: int "
                SCALCON
                L" CDefView::_UseItemsView(void)"
            },
            &CDefView__UseItemsView_orig,
            CDefView__UseItemsView_hook,
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
