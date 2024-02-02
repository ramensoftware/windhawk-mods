// ==WindhawkMod==
// @id              classic-browser-fix
// @name            Fix browsers for Windows Classic theme
// @description     Forces the border from Aero Light theme to fix glitched border in Classic theme
// @version         1.0
// @author 			Anixx
// @github 			https://github.com/Anixx
// @include         msedge.exe
// @include         chrome.exe
// @include         chromium.exe
// @include         steam.exe

// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod forces Aero Lite window borders on browsers MS Edge, Chrome, Chromium and Steam app,
which fixes their glitches when using Windows Classic theme.

![Aero Lite](https://i.imgur.com/5enqSD8.png)

*/
// ==/WindhawkModReadme==

#include <uxtheme.h>

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    SetThemeAppProperties(0);
    return TRUE;
}
