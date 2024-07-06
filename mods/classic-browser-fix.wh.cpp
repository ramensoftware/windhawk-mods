// ==WindhawkMod==
// @id              classic-browser-fix
// @name            Fix browsers for Windows Classic theme
// @description     Forces the border from Aero Light theme to fix glitched border in Classic theme
// @version         1.2
// @author 			Anixx
// @github 			https://github.com/Anixx
// @include         msedge.exe
// @include         brave.exe
// @include         chrome.exe
// @include         chromium.exe
// @include         Skype.exe
// @include         steam.exe

// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod forces Aero Lite window borders on Chromium-based browsers and applications
MS Edge, Chrome, Chromium, Skype, and Steam app,
which fixes their glitches when using Windows Classic theme.

![Aero Lite](https://i.imgur.com/5enqSD8.png)

Authors: Anixx, levitation
*/
// ==/WindhawkModReadme==

#include <uxtheme.h>

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    SetThemeAppProperties(STAP_ALLOW_CONTROLS);       //Without the STAP_ALLOW_CONTROLS flag the file save dialog fails to open.
    return TRUE;
}
