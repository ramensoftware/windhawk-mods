// ==WindhawkMod==
// @id              shrink-address-bar-height
// @name            Shrink Address Bar Height
// @description     Shrinks the address bar height in file explorer
// @version         1.0.1
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shrink Address Bar Height
This mod shrinks the address bar height in file explorer.

Note: This mod applies to explorer folders as well as save/open dialogs in all programs.

The code is based on the implementation in [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher).

![Screenshot](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/shrink-address-bar-height.png)
*/
// ==/WindhawkModReadme==


int (WINAPI *GetSystemMetricsForDpiOrig)(int nIndex, UINT dpi);
int WINAPI GetSystemMetricsForDpiHook(int nIndex, UINT dpi)
{
    if (nIndex == SM_CYFIXEDFRAME) return 0;
    
    return GetSystemMetricsForDpiOrig(nIndex, dpi);
}


BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* origFunc = (void*)GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    Wh_SetFunctionHook(origFunc, (void*)GetSystemMetricsForDpiHook, (void**)&GetSystemMetricsForDpiOrig);

    return TRUE;
}
