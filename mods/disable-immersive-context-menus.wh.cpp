// ==WindhawkMod==
// @id              disable-immersive-context-menus
// @name            Disable Immersive Context Menus
// @description     Disables immersive context menus in File Explorer
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Immersive Context Menus
This mod disables immersive context menus in file explorer.

This mod applies to explorer folders as well as save/open dialogs in all programs.

Note: This mod disables immersive context menus for *file explorer*, *desktop*, etc.  
For disabling immersive context menus on the *taskbar*, use my [Non Immersive Taskbar Context Menu](https://windhawk.net/mods/classic-taskbar-context-menu) mod.

**Without** this mod:
![Screenshot](https://i.imgur.com/0iRYrCJ.png)

**With** this mod:
![Screenshot](https://i.imgur.com/1bkNHyT.png)
*/
// ==/WindhawkModReadme==

using SystemParametersInfoW_t = decltype(&SystemParametersInfoW);
SystemParametersInfoW_t pOriginalSystemParametersInfoW;
BOOL WINAPI SystemParametersInfoW_Hook(
    UINT  uiAction,
    UINT  uiParam,
    PVOID pvParam,
    UINT  fWinIni
)
{
    if (uiAction == SPI_GETSCREENREADER)
    {
        *(BOOL*)pvParam = TRUE;
        return TRUE;
    }

    return pOriginalSystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
}


BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)SystemParametersInfoW, (void*)SystemParametersInfoW_Hook, (void**)&pOriginalSystemParametersInfoW);
    return TRUE;
}
