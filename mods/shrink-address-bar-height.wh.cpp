// ==WindhawkMod==
// @id              shrink-address-bar-height
// @name            Shrink Address Bar Height
// @description     Shrinks the address bar height in file explorer
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shrink Address Bar Height
This mod shrinks the address bar height in file explorer.

The code is based on the implementation in [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher).

![Screenshot](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/win7-command-bar.png)
*/
// ==/WindhawkModReadme==

int (*ExplorerFrame_GetSystemMetricsForDpiOrig)(int nIndex, UINT dpi);
int ExplorerFrame_GetSystemMetricsForDpiHook(int nIndex, UINT dpi)
{
    if (nIndex == SM_CYFIXEDFRAME) return 0;
    
    return ExplorerFrame_GetSystemMetricsForDpiOrig(nIndex, dpi);
}


BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* origFunc = (void*)GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    Wh_SetFunctionHook(origFunc, (void*)ExplorerFrame_GetSystemMetricsForDpiHook, (void**)&ExplorerFrame_GetSystemMetricsForDpiOrig);

    return TRUE;
}
