// ==WindhawkMod==
// @id              use-windows-10-right-click-context-menu
// @name            Switch Windows 11 right-click context menu to Windows 10 style
// @description     This simplest mod will switch Win11 style right-click context menu in explorer into classic (Win 10) style
// @version         0.1
// @author          Puvox Software
// @github          https://github.com/puvox
// @homepage        https://puvox.software/
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Win11 new context menu in explorer
You need to enter the "Settings" page of this mod to enable it.

# Getting started
![screenshot](https://i.imgur.com/TouCt1Q.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- disableWin11ContextMenu: false
  $name: Disable Windows 11 right click context menu
  $description: Note, you will need to click SAVE SETTINGS and explorer will restart.
*/
// ==/WindhawkModSettings==


#include <gdiplus.h>
#include <cstdlib>
#include <windows.h>
#include <cstdlib>
#include <unistd.h> // For sleep() function on Unix-like systems

struct {
    bool disableWin11ContextMenu;
} settings;


void LoadSettings() {
    settings.disableWin11ContextMenu = Wh_GetIntSetting(L"disableWin11ContextMenu");
}

void restart ()
{
    system("taskkill /f /im explorer.exe & start explorer");
}

// The mod is being initialized, load settings, hook functions, and do other (required).
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    LoadSettings();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"SettingsChanged !!!");
    LoadSettings();
    if (settings.disableWin11ContextMenu) {
        system("reg add \"HKCU\\Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}\\InprocServer32\" /f /ve");
    } else {
        system("reg delete \"HKCU\\Software\\Classes\\CLSID\\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}\" /f");
    }
    restart ();
    return true;
}

