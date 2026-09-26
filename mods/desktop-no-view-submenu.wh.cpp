// ==WindhawkMod==
// @id              desktop-no-view-submenu
// @name            Desktop No View Submenu
// @description     Removes the View submenu from the desktop context menu and moves "Show desktop icons" into Sort by, as in Windows XP and earlier; works on Windows 11 24H2
// @name:ru         Без подменю «Вид» на рабочем столе
// @description:ru  Убирает подменю «Вид» из контекстного меню рабочего стола и переносит пункт «Отображать значки рабочего стола» в «Сортировка», как в Windows XP и более ранних; работает в Windows 11 24H2
// @version         2.0
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @architecture    x86-64
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop No View Submenu

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

Windows XP and before did not let you change the view of the desktop and had no
"View" submenu in the desktop's context menu. This mod removes that submenu and,
like Windows XP, keeps the "Show desktop icons" switch by moving it into the
"Sort by" ("Arrange Icons By") submenu.

Folder windows are not touched - only the desktop's own context menu.

## Notes

This is a rewrite of [Desktop No View Menu](https://windhawk.net/mods/desktop-no-view-menu)
by aubymori. The original recognises the desktop by reading a flag at a fixed
offset inside `CDefView`, and that offset moved in Windows 11 24H2 (26100), so
on those builds the mod loads but does nothing.

This version does not read any `CDefView` fields. It edits the menu after
shell32 has finished building it, and recognises the desktop by the one item
that only ever appears in the desktop's background menu - "Show desktop icons".

---

## По-русски

В Windows XP и более ранних версиях вид рабочего стола нельзя было менять, и в
его контекстном меню не было подменю «Вид». Мод убирает это подменю и, как в
Windows XP, сохраняет переключатель «Отображать значки рабочего стола»,
переставляя его в подменю «Сортировка» («Упорядочить значки»). Куда поместить
этот пункт, можно выбрать в настройках.

Окна папок не затрагиваются - меняется только контекстное меню самого рабочего
стола.

Это переписанная версия мода
[Desktop No View Menu](https://windhawk.net/mods/desktop-no-view-menu) от
aubymori. Оригинал узнаёт рабочий стол по флагу по фиксированному смещению
внутри `CDefView`, а в Windows 11 24H2 (26100) это смещение изменилось, и мод
загружается, но ничего не делает. Эта версия не читает поля `CDefView`: она
правит меню после того, как shell32 закончил его строить, и узнаёт рабочий стол
по пункту, который бывает только в его меню, - «Отображать значки рабочего
стола».

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showDesktopIcons: sortby
  $name: 'The "Show desktop icons" item'
  $name:ru: Пункт «Отображать значки рабочего стола»
  $description: >-
    The switch lives inside the "View" submenu, so it disappears along with it.
    Choose where it should go instead.
  $description:ru: >-
    Этот пункт находится внутри меню «Вид» и пропадает вместе с ним.
    Выберите, куда его перенести.
  $options:
  - sortby: Move into the "Sort by" submenu (like Windows XP)
  - menu: Keep it in the context menu itself
  - remove: Remove it as well
  $options:ru:
  - sortby: Перенести в подменю «Сортировка» (как в Windows XP)
  - menu: Оставить в самом контекстном меню
  - remove: Тоже убрать
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <windhawk_utils.h>

/* Menu template in shell32.dll used for a shell view's background menu. */
#define POPUP_SFV_BACKGROUND 215

/* Command IDs of that template (shell32 private, unchanged since Windows XP). */
#define FCIDM_SHVIEWLAST         0x7fff
#define SFVIDM_FIRST             (FCIDM_SHVIEWLAST - 0x0fff)
#define SFVIDM_MENU_ARRANGE      (SFVIDM_FIRST + 0x0001)
#define SFVIDM_MENU_VIEW         (SFVIDM_FIRST + 0x0002)
#define SFVIDM_DESKTOP_FIRST     (SFVIDM_FIRST + 0x0400)
#define SFVIDM_DESKTOPHTML_ICONS (SFVIDM_DESKTOP_FIRST + 0x0002)

enum class IconsItemPlacement {
    sortBy,
    menu,
    remove,
};

struct {
    IconsItemPlacement showDesktopIcons;
} g_settings;

/* Only set while CDefView::_Create_BackgrndHMENU runs on this thread. */
thread_local int g_inCreateBackgrndMenu;
thread_local HMENU g_hmenuBackground;

static HMENU GetSubMenuByCommand(HMENU hMenu, UINT uId) {
    MENUITEMINFOW mii = {sizeof(MENUITEMINFOW)};
    mii.fMask = MIIM_SUBMENU;
    if (!GetMenuItemInfoW(hMenu, uId, FALSE, &mii)) {
        return NULL;
    }
    return mii.hSubMenu;
}

static int GetPositionByCommand(HMENU hMenu, UINT uId) {
    int cItems = GetMenuItemCount(hMenu);
    for (int i = 0; i < cItems; i++) {
        if (GetMenuItemID(hMenu, i) == uId) {
            return i;
        }
    }
    return -1;
}

/* Fallback for the restricted case where shell32 does not merge the
   "Show desktop icons" item in: the desktop's shell view has a thread of its
   own, folder windows get one each. */
static bool IsDesktopThread(void) {
    HWND hDefView = NULL;

    HWND hProgman = FindWindowW(L"Progman", NULL);
    if (hProgman) {
        hDefView = FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);
    }

    if (!hDefView) {
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL))) {
            hDefView = FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDefView) {
                break;
            }
        }
    }

    return hDefView &&
           GetWindowThreadProcessId(hDefView, NULL) == GetCurrentThreadId();
}

static void RemoveViewMenu(HMENU hMenu) {
    HMENU hmenuView = GetSubMenuByCommand(hMenu, SFVIDM_MENU_VIEW);
    if (!hmenuView) {
        Wh_Log(L"No View submenu, nothing to do");
        return;
    }

    WCHAR szIcons[128] = L"";
    MENUITEMINFOW miiIcons = {sizeof(MENUITEMINFOW)};
    miiIcons.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STATE | MIIM_STRING;
    miiIcons.dwTypeData = szIcons;
    miiIcons.cch = ARRAYSIZE(szIcons);
    BOOL fHasIconsItem =
        GetMenuItemInfoW(hmenuView, SFVIDM_DESKTOPHTML_ICONS, FALSE, &miiIcons);

    /* "Show desktop icons" is merged in for the desktop only, so it tells the
       desktop's background menu apart from a folder's. */
    if (!fHasIconsItem && !IsDesktopThread()) {
        Wh_Log(L"Folder background menu, leaving it alone");
        return;
    }

    int posView = GetPositionByCommand(hMenu, SFVIDM_MENU_VIEW);

    if (fHasIconsItem &&
        g_settings.showDesktopIcons != IconsItemPlacement::remove) {
        miiIcons.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STATE | MIIM_STRING;
        miiIcons.dwTypeData = szIcons;

        HMENU hmenuArrange =
            g_settings.showDesktopIcons == IconsItemPlacement::sortBy
                ? GetSubMenuByCommand(hMenu, SFVIDM_MENU_ARRANGE)
                : NULL;

        if (hmenuArrange) {
            AppendMenuW(hmenuArrange, MF_SEPARATOR, 0, NULL);
            InsertMenuItemW(hmenuArrange, (UINT)-1, TRUE, &miiIcons);
        } else {
            InsertMenuItemW(hMenu, posView >= 0 ? (UINT)posView : 0, TRUE,
                            &miiIcons);
        }
    }

    DeleteMenu(hMenu, SFVIDM_MENU_VIEW, MF_BYCOMMAND);
    Wh_Log(L"View submenu removed");
}

using SHLoadPopupMenu_t = HMENU(__cdecl*)(HINSTANCE, UINT);
SHLoadPopupMenu_t SHLoadPopupMenu_orig;
HMENU __cdecl SHLoadPopupMenu_hook(HINSTANCE hinst, UINT id) {
    HMENU hMenu = SHLoadPopupMenu_orig(hinst, id);

    if (hMenu && id == POPUP_SFV_BACKGROUND && g_inCreateBackgrndMenu) {
        g_hmenuBackground = hMenu;
    }

    return hMenu;
}

using CDefView__Create_BackgrndHMENU_t = HRESULT(__cdecl*)(void*, UINT, REFIID,
                                                           void**);
CDefView__Create_BackgrndHMENU_t CDefView__Create_BackgrndHMENU_orig;
HRESULT __cdecl CDefView__Create_BackgrndHMENU_hook(void* pThis,
                                                    UINT idMenuToKeep,
                                                    REFIID riid, void** ppv) {
    HMENU hmenuOuter = g_hmenuBackground;
    g_hmenuBackground = NULL;
    g_inCreateBackgrndMenu++;

    HRESULT hr =
        CDefView__Create_BackgrndHMENU_orig(pThis, idMenuToKeep, riid, ppv);

    g_inCreateBackgrndMenu--;
    HMENU hMenu = g_hmenuBackground;
    g_hmenuBackground = hmenuOuter;

    /* shell32 is done with the menu here: the desktop items have been merged in
       and the IDs are still the ones from the menu template. */
    if (SUCCEEDED(hr) && hMenu) {
        RemoveViewMenu(hMenu);
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

static void LoadSettings(void) {
    PCWSTR placement = Wh_GetStringSetting(L"showDesktopIcons");

    g_settings.showDesktopIcons = IconsItemPlacement::sortBy;
    if (0 == wcscmp(placement, L"menu")) {
        g_settings.showDesktopIcons = IconsItemPlacement::menu;
    } else if (0 == wcscmp(placement, L"remove")) {
        g_settings.showDesktopIcons = IconsItemPlacement::remove;
    }

    Wh_FreeStringSetting(placement);
}

BOOL Wh_ModInit(void) {
    LoadSettings();

    HMODULE hShell32 =
        LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks,
                                    ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to hook shell32.dll symbols");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
}
