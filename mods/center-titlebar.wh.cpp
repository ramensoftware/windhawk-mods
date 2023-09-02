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

![Screenshot](https://i.imgur.com/ULXuIQn.png)

Only has the non-aero path for now.

# ⚠ Important usage note ⚠ 
  
 In order to use this mod, you must enable Windhawk to inject into system processes in its advanced settings. 
 If you do not do this, it will silently fail to inject. **Changing the Windhawk advanced settings will also 
 affect any other mod you have installed, and may cause instability as any other mod that injects into all 
 processes will now inject into system processes too.** 
  
 This mod will not work on portable versions of Windhawk because DWM is a protected process and can only be 
 modified by a system account. Since the portable version of Windhawk only runs as administrator under your 
 own user account, it will not have the privilege required to inject into DWM. (You may not be so lucky with 
 forcing the portable version to run as `NT AUTHORITY\SYSTEM` either, as this didn't work in my testing.) 
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
