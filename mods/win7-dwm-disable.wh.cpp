// ==WindhawkMod==
// @id              win7-dwm-disable
// @name            Win7 Disable Composition for process
// @description     Disables desktop composition when a certain process is running
// @version         1.0
// @author          Jevil7452
// @github          https://github.com/Jevil7452
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod lets you disable DWM desktop composition when you run a certain program.

It acts the same as the "Disable desktop composition" option in the Compatibility tab, however this can be used for 64-bit programs as well.

To include a program to be affected by the mod, use the process inclusion option on the Advanced tab.

NOTE: If your program re-enables DWM composition by itself (for example, OBS versions that support turning it off via the program), it will not work

Example with Notepad:

![Notepad](https://i.imgur.com/4O06W7H.png)

IMPORTANT: This mod is only for actual Windows 7, it will not work on newer versions
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    DwmEnableComposition(0);
    return TRUE;
}
