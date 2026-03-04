// ==WindhawkMod==
// @id              classic-taskbar-context-menu-lite
// @name            Non-Immersive Taskbar Context Menu Lite
// @description     Restores the classic (non-immersive) taskbar context menus in Win10 taskbar
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Compared to the other mod `Non-Immersive Taskbar Context Menu` this one has simplier code and supports
the second-level menus.

Unlike `Eradicate immersive menus` it does not download symbols.

Intended for Win10 taskbar running either under Win10 or Win11.

*/
// ==/WindhawkModReadme==

using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
SetMenuItemInfoW_t pOriginalSetMenuItemInfoW;

BOOL WINAPI SetMenuItemInfoW_Hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmii) {
    if (lpmii && (lpmii->fMask & MIIM_FTYPE) && (lpmii->fType & MFT_OWNERDRAW)) {
        alignas(MENUITEMINFOW) BYTE buffer[256];
        
        if (lpmii->cbSize > sizeof(buffer)) {
            return pOriginalSetMenuItemInfoW(hMenu, item, fByPosition, lpmii);
        }
        
        memcpy(buffer, lpmii, lpmii->cbSize);
        auto* copy = reinterpret_cast<LPMENUITEMINFOW>(buffer);
        
        copy->fType &= ~MFT_OWNERDRAW;
        
        return pOriginalSetMenuItemInfoW(hMenu, item, fByPosition, copy);
    }
    return pOriginalSetMenuItemInfoW(hMenu, item, fByPosition, lpmii);
}

BOOL Wh_ModInit() {

    return Wh_SetFunctionHook((void*)SetMenuItemInfoW, (void*)SetMenuItemInfoW_Hook, (void**)&pOriginalSetMenuItemInfoW);

}
