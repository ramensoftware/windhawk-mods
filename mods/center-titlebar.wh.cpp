// ==WindhawkMod==
// @id              center-titlebar
// @name            Center Titlebar
// @description     Center align the text in titlebar
// @version         1.0
// @author          rounk-ctrl
// @github          https://github.com/rounk-ctrl
// @include         dwm.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Center Titlebar
A port of valinet's [WinCenterTitle](https://github.com/valinet/WinCenterTitle).
Only has the non-aero path for now.
*/
// ==/WindhawkModReadme==

#include <Windows.h>

#define ENTIRE_TITLEBAR 10000

// DrawTextW
using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Orig;
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    int ret;

    if (!(format & DT_CALCRECT))
    {
        // fixes title label sometimes overlapping right margin of window
        lprc->right -= 2;
    }
    ret = DrawTextW_Orig(
        hdc,
        lpchText,
        cchText,
        lprc,
        (format & (~DT_LEFT)) | DT_CENTER
    );
    if (format & DT_CALCRECT) lprc->right = ENTIRE_TITLEBAR;
    return ret;
}

// init
BOOL Wh_ModInit() {
    // hook DrawTextW
    Wh_SetFunctionHook((void*)DrawTextW, (void*)DrawTextW_Hook,
                       (void**)&DrawTextW_Orig);
    return TRUE;
}

// uninit
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
