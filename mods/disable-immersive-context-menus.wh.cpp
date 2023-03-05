// ==WindhawkMod==
// @id              disable-immersive-context-menus
// @name            Disable Immersive Context Menus
// @description     Disables immersive context menus in File Explorer
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Immersive Context Menus
This mod disables immersive context menus in file explorer.

Note: This mod disables immersive context menus for *file explorer*, *desktop*, etc.  
For disabling immersive context menus on the *taskbar*, use my [Non Immersive Taskbar Context Menu](https://windhawk.net/mods/classic-taskbar-context-menu) mod.

**Without** this mod:
![Screenshot](https://i.imgur.com/0iRYrCJ.png)

**With** this mod:
![Screenshot](https://i.imgur.com/1bkNHyT.png)
*/
// ==/WindhawkModReadme==

BOOL(*pOriginalSystemParametersInfoW)(
    UINT  uiAction,
    UINT  uiParam,
    PVOID pvParam,
    UINT  fWinIni
);

BOOL WINAPI SystemParametersInfoWHook(
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
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* origFunc = (void*)GetProcAddress(hUser32, "SystemParametersInfoW");
    Wh_SetFunctionHook(origFunc, (void*)SystemParametersInfoWHook, (void**)&pOriginalSystemParametersInfoW);

    return TRUE;
}
