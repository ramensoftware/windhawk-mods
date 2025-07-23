// ==WindhawkMod==
// @id              disable-tray-icons
// @name            Disable Tray Icons
// @description     Disable all tray icons for a specific application
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Tray Icons
This mod will disable all tray icons for a specific application.

## How to use
1. Go to the "Advanced" tab of this mod
2. Find the "Custom process inclusion list"
3. Add the name of the EXE you want to disable tray icons for.
   - If you don't know the name of the EXE, you can find it using 
     Task Manager.
4. Click Save, and if the program is open already, restart it.
*/
// ==/WindhawkModReadme==

#include <shellapi.h>

using Shell_NotifyIconW_t = decltype(&Shell_NotifyIconW);
Shell_NotifyIconW_t Shell_NotifyIconW_orig;
BOOL WINAPI Shell_NotifyIconW_hook(
    DWORD dwMessage, PNOTIFYICONDATAW lpData
)
{
    return TRUE;
}

using Shell_NotifyIconA_t = decltype(&Shell_NotifyIconA);
Shell_NotifyIconA_t Shell_NotifyIconA_orig;
BOOL WINAPI Shell_NotifyIconA_hook(
    DWORD dwMessage, PNOTIFYICONDATAA lpData
)
{
    return TRUE;
}

BOOL Wh_ModInit(void)
{
    return Wh_SetFunctionHook(
        (void *)Shell_NotifyIconW,
        (void *)Shell_NotifyIconW_hook,
        (void **)&Shell_NotifyIconW_orig
    ) && Wh_SetFunctionHook(
        (void *)Shell_NotifyIconA,
        (void *)Shell_NotifyIconA_hook,
        (void **)&Shell_NotifyIconA_orig
    );
}