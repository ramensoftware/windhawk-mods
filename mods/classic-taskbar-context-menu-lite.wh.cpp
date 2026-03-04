// ==WindhawkMod==
// @id              classic-taskbar-context-menu-lite
// @name            Non-Immersive Taskbar Context Menu Lite
// @description     Restores the classic (non-immersive) taskbar context menus
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
SetMenuItemInfoW_t pOriginalSetMenuItemInfoW;

BOOL WINAPI SetMenuItemInfoW_Hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmii) {
    if (lpmii && (lpmii->fMask & MIIM_FTYPE) && (lpmii->fType & MFT_OWNERDRAW)) {
        MENUITEMINFOW copy = *lpmii;
        copy.fType &= ~MFT_OWNERDRAW;
        return pOriginalSetMenuItemInfoW(hMenu, item, fByPosition, &copy);
    }
    return pOriginalSetMenuItemInfoW(hMenu, item, fByPosition, lpmii);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    Wh_SetFunctionHook((void*)SetMenuItemInfoW, (void*)SetMenuItemInfoW_Hook, (void**)&pOriginalSetMenuItemInfoW);

    return TRUE;
}
