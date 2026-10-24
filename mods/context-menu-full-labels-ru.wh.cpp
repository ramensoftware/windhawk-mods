// ==WindhawkMod==
// @id              context-menu-full-labels-ru
// @name            Full labels in the context menu (RU)
// @name:ru-RU      Полные названия в контекстном меню (RU)
// @description     Replaces the abbreviated Russian items "Копир.", "Переим.", "Созд. ярлык" in the context menu (in Explorer and in file/folder pickers of any program) with full words
// @description:ru-RU Заменяет сокращения «Копир.», «Переим.», «Созд. ярлык» в контекстном меню (в Проводнике и в диалогах выбора файла/папки любых программ) на полные слова
// @version         1.3
// @author          limpompo
// @github          https://github.com/limpompo
// @homepage        https://github.com/limpompo/windhawk-mods-limpompo/tree/main/mods/context-menu-full-labels-ru
// @license         MIT
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Full labels in the context menu (RU)

Some Windows 11 builds ship a Russian `shell32.dll.mui` with abbreviated items
in the classic context menu: "Копир.", "Переим.", "Созд. ярлык".
This mod replaces them with "Копировать", "Переименовать", "Создать ярлык".
It works in all processes (including the Open/Save/Select Folder dialogs of
other programs). System files are not modified.

Only useful with the Russian Windows UI language.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <wchar.h>

struct Replacement {
    const wchar_t* from;
    const wchar_t* to;
};

const Replacement kReplacements[] = {
    {L"&Копир.", L"&Копировать"},
    {L"П&ереим.", L"П&ереименовать"},
    {L"Созд. &ярлык", L"Создать &ярлык"},
};

// Compare two menu strings ignoring '&' accelerator markers.
bool EqualsIgnoringAccel(const wchar_t* a, const wchar_t* b) {
    while (true) {
        while (*a == L'&') a++;
        while (*b == L'&') b++;
        if (*a != *b) return false;
        if (*a == L'\0') return true;
        a++;
        b++;
    }
}

const wchar_t* FindReplacement(const wchar_t* text) {
    if (!text || IS_INTRESOURCE(text)) return nullptr;
    for (const auto& r : kReplacements) {
        if (EqualsIgnoringAccel(text, r.from)) return r.to;
    }
    return nullptr;
}

// Explorer's context menu items are owner-drawn, but the owner-draw code
// still reads the item string, so replacing the string is enough.
void FixMenu(HMENU menu, int depth) {
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        wchar_t text[256] = L"";
        MENUITEMINFOW mii = {sizeof(mii)};
        mii.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_SUBMENU;
        mii.dwTypeData = text;
        mii.cch = ARRAYSIZE(text);
        if (!GetMenuItemInfoW(menu, i, TRUE, &mii)) continue;

        if (!(mii.fType & MFT_SEPARATOR) && mii.cch > 0) {
            if (const wchar_t* to = FindReplacement(text)) {
                MENUITEMINFOW set = {sizeof(set)};
                set.fMask = MIIM_STRING;
                set.dwTypeData = const_cast<LPWSTR>(to);
                SetMenuItemInfoW(menu, i, TRUE, &set);
            }
        }

        if (mii.hSubMenu && depth < 4) FixMenu(mii.hSubMenu, depth + 1);
    }
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                  HWND hwnd, LPTPMPARAMS lptpm) {
    FixMenu(hMenu, 0);
    return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hwnd, lptpm);
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                int nReserved, HWND hWnd, const RECT* prcRect) {
    FixMenu(hMenu, 0);
    return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd,
                                   prcRect);
}

using InsertMenuItemW_t = decltype(&InsertMenuItemW);
InsertMenuItemW_t InsertMenuItemW_Original;

BOOL WINAPI InsertMenuItemW_Hook(HMENU hmenu, UINT item, BOOL fByPosition,
                                 LPCMENUITEMINFOW lpmi) {
    if (lpmi && (lpmi->fMask & (MIIM_STRING | MIIM_TYPE)) &&
        !(lpmi->fType & (MFT_SEPARATOR | MFT_BITMAP | MFT_OWNERDRAW))) {
        if (const wchar_t* to = FindReplacement(lpmi->dwTypeData)) {
            MENUITEMINFOW copy = *lpmi;
            copy.dwTypeData = const_cast<LPWSTR>(to);
            copy.cch = (UINT)wcslen(to);
            return InsertMenuItemW_Original(hmenu, item, fByPosition, &copy);
        }
    }
    return InsertMenuItemW_Original(hmenu, item, fByPosition, lpmi);
}

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_Original;

HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    HMENU menu = LoadMenuW_Original(hInstance, lpMenuName);
    if (menu) FixMenu(menu, 0);
    return menu;
}

using LoadMenuIndirectW_t = decltype(&LoadMenuIndirectW);
LoadMenuIndirectW_t LoadMenuIndirectW_Original;

HMENU WINAPI LoadMenuIndirectW_Hook(const MENUTEMPLATEW* lpMenuTemplate) {
    HMENU menu = LoadMenuIndirectW_Original(lpMenuTemplate);
    if (menu) FixMenu(menu, 0);
    return menu;
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook,
                       (void**)&TrackPopupMenuEx_Original);
    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)TrackPopupMenu_Hook,
                       (void**)&TrackPopupMenu_Original);
    Wh_SetFunctionHook((void*)InsertMenuItemW, (void*)InsertMenuItemW_Hook,
                       (void**)&InsertMenuItemW_Original);
    Wh_SetFunctionHook((void*)LoadMenuW, (void*)LoadMenuW_Hook,
                       (void**)&LoadMenuW_Original);
    Wh_SetFunctionHook((void*)LoadMenuIndirectW, (void*)LoadMenuIndirectW_Hook,
                       (void**)&LoadMenuIndirectW_Original);
    return TRUE;
}
