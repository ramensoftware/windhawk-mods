// ==WindhawkMod==
// @id              desktop-no-view-menu
// @name            Desktop No View Menu
// @description     Removes the "View" menu from the desktop context menu
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop No View Menu
Windows XP and before did not allow you to change the view of the desktop and
as such did not have a "View" menu in the desktop's context menu. This mod
removes that menu and moves the "Show desktop icons" option to the "Sort by"
(formerly "Arrange Icons By") menu, like Windows XP and before.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/desktop-no-view-menu/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/desktop-no-view-menu/after.png)
*/
// ==/WindhawkModReadme==

#include <shlobj.h>
#include <windhawk_utils.h>

#define POPUP_SFV_BACKGROUND    215
#define POPUP_SFV_BACKGROUND_AD 218

#define FCIDM_SHVIEWLAST            0x7fff
#define SFVIDM_FIRST            (FCIDM_SHVIEWLAST-0x0fff)
#define SFVIDM_MENU_ARRANGE	    (SFVIDM_FIRST + 0x0001)
#define SFVIDM_MENU_VIEW        (SFVIDM_FIRST + 0x0002)

#define SFVIDM_DESKTOP_FIRST            (SFVIDM_FIRST + 0x0400)
#define SFVIDM_DESKTOPHTML_ICONS        (SFVIDM_DESKTOP_FIRST + 0x0002)

thread_local void *g_pCurrentDefView = nullptr;
thread_local HMENU g_hmenuArrange = NULL;
thread_local bool g_fDesktopIconsShown = false;

HMENU (*SHLoadPopupMenu_orig)(HINSTANCE, UINT);
HMENU SHLoadPopupMenu_hook(HINSTANCE hinst, UINT id)
{
    HMENU hmenu = SHLoadPopupMenu_orig(hinst, id);
    if (id == POPUP_SFV_BACKGROUND
    && g_pCurrentDefView
    && (*((DWORD *)g_pCurrentDefView + 200) & 0x10000000))
    {
        // Delete view menu.
        // The original XP code also deleted SFVIDM_VIEW_COLSETTINGS but
        // I won't since it doesn't exist in the menu template anymore.
        DeleteMenu(hmenu, SFVIDM_MENU_VIEW, MF_BYCOMMAND);

        if (!SHRestricted(REST_CLASSICSHELL))
        {
            HMENU hmenuAD = SHLoadPopupMenu_orig(hinst, POPUP_SFV_BACKGROUND_AD);
            if (hmenuAD)
            {
                MENUITEMINFOW mii = { 0 };
                mii.cbSize = sizeof(mii);
                mii.fMask = MIIM_SUBMENU;
                if (GetMenuItemInfoW(hmenu, SFVIDM_MENU_ARRANGE, FALSE, &mii))
                {
                    g_hmenuArrange = mii.hSubMenu;

                    SHELLSTATEW ss;
                    SHGetSetSettings(&ss, SSF_DESKTOPHTML | SSF_HIDEICONS, FALSE);

                    // See tail of CDefView__Create_BackgrndHMENU_hook.
                    if (!ss.fHideIcons)
                        g_fDesktopIconsShown = true;

                    Shell_MergeMenus(mii.hSubMenu, hmenuAD, (UINT)-1, 0, (UINT)-1, MM_ADDSEPARATOR);
                }
            }
        }
    }
    return hmenu;
}

HRESULT (*CDefView__Create_BackgrndHMENU_orig)(void *, UINT, REFIID, void **);
HRESULT CDefView__Create_BackgrndHMENU_hook(void *pThis, UINT idMenuToKeep, REFIID riid, void **ppv)
{
    g_pCurrentDefView = pThis;
    HRESULT hr = CDefView__Create_BackgrndHMENU_orig(pThis, idMenuToKeep, riid, ppv);
    g_pCurrentDefView = nullptr;
    
    // HACK: Something after the SHLoadPopupMenu call clears the
    // checkbox of the "Show desktop icons" item. Defer it to here
    // to stop whatever's doing that...
    if (g_fDesktopIconsShown)
    {
        CheckMenuItem(g_hmenuArrange, SFVIDM_DESKTOPHTML_ICONS, MF_BYCOMMAND | MF_CHECKED);
        g_fDesktopIconsShown = false;
        g_hmenuArrange = NULL;
    }

    return hr;
}

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        {
            L"struct HMENU__ * __cdecl SHLoadPopupMenu(struct HINSTANCE__ *,unsigned int)"
        },
        &SHLoadPopupMenu_orig,
        SHLoadPopupMenu_hook,
        false
    },
    {
        {
            L"private: long __cdecl CDefView::_Create_BackgrndHMENU(unsigned int,struct _GUID const &,void * *)"
        },
        &CDefView__Create_BackgrndHMENU_orig,
        CDefView__Create_BackgrndHMENU_hook,
        false
    },
};

BOOL Wh_ModInit(void)
{
    HMODULE hmodShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hmodShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hmodShell32,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    }

    return TRUE;
}