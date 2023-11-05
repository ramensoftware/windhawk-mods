// ==WindhawkMod==
// @id              explorer-32px-icons
// @name            Explorer 32px Icons
// @description     Makes the "Medium icons" view in Explorer 32px again
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer 32px Icons
Beginning with Windows Vista, the "Medium icons" view (previous just "Icons") uses
icons that are 48 pixels in size, instead of the previous 32 pixel size. This mod
restores the 32 pixel size.

**This mod will only work on Windhawk v1.4 and greater.**

# IMPORTANT
You **must** be using Explorer with the SysListView32 view enabled for this mod to work.
The default DirectUI view will not work.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/explorer-32px-icons-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/explorer-32px-icons-after.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

/* FOLDERVIEWMODE enum */
#include <shobjidl.h>

/* All of these symbols use the same calling convention,
   but it varies between x86 and x86-64 */
#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __thiscall
#define SCALCON L"__thiscall"
#endif

/* Makes "Medium icons" set 32px icons. */
typedef void (* CALCON CDefView__SwitchToViewModeAndIconSize_t)(void *, FOLDERVIEWMODE, int);
CDefView__SwitchToViewModeAndIconSize_t CDefView__SwitchToViewModeAndIconSize_orig;
void CALCON CDefView__SwitchToViewModeAndIconSize_hook(
    void           *pThis,
    FOLDERVIEWMODE  mode,
    int             size
)
{
    if (mode == FVM_THUMBNAIL && size == 48)
    {
        size = 32;
    }

    return CDefView__SwitchToViewModeAndIconSize_orig(
        pThis, mode, size
    );
}

/* Make 32px icons display labels below. */
typedef void (* CALCON CListViewHost__SwitchToViewModeAndIconSizeWorker_t)(void *, FOLDERVIEWMODE, FOLDERVIEWMODE, UINT);
CListViewHost__SwitchToViewModeAndIconSizeWorker_t CListViewHost__SwitchToViewModeAndIconSizeWorker_orig;
void CALCON CListViewHost__SwitchToViewModeAndIconSizeWorker_hook(
    void           *pThis,
    FOLDERVIEWMODE  prevMode,
    FOLDERVIEWMODE  newMode,
    UINT            size
)
{
    if (newMode == FVM_SMALLICON && size == 32)
    {
        newMode = FVM_ICON;
    }

    return CListViewHost__SwitchToViewModeAndIconSizeWorker_orig(
        pThis, prevMode, newMode, size
    );
}

/* Make "Medium icons" selected when icons are 32px */
typedef UINT (*CALCON CDefView_GetMenuIDFromViewModeAndIconSize_t)(void *, FOLDERVIEWMODE, UINT, int);
CDefView_GetMenuIDFromViewModeAndIconSize_t CDefView_GetMenuIDFromViewModeAndIconSize_orig;
UINT CALCON CDefView_GetMenuIDFromViewModeAndIconSize_hook(
    void           *pThis,
    FOLDERVIEWMODE  mode,
    UINT            size,
    int             i1
)
{
    if (mode == FVM_SMALLICON && size == 32)
    {
        return 28750;
    }

    return CDefView_GetMenuIDFromViewModeAndIconSize_orig(
        pThis, mode, size, i1
    );
}

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
                L"private: void "
                SCALCON
                L" CDefView::_SwitchToViewModeAndIconSize(enum FOLDERVIEWMODE,int)"
            },
            &CDefView__SwitchToViewModeAndIconSize_orig,
            CDefView__SwitchToViewModeAndIconSize_hook,
            false
        },
        {
            {
                L"private: void "
                SCALCON
                L" CListViewHost::_SwitchToViewModeAndIconSizeWorker(enum FOLDERVIEWMODE,enum FOLDERVIEWMODE,unsigned int)"
            },
            &CListViewHost__SwitchToViewModeAndIconSizeWorker_orig,
            CListViewHost__SwitchToViewModeAndIconSizeWorker_hook,
            false
        },
        {
            {
                L"private: unsigned int "
                SCALCON
                L" CDefView::GetMenuIDFromViewModeAndIconSize(unsigned int,unsigned int,int)"
            },
            &CDefView_GetMenuIDFromViewModeAndIconSize_orig,
            CDefView_GetMenuIDFromViewModeAndIconSize_hook,
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