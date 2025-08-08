// ==WindhawkMod==
// @id              classic-explorer-dragdrop-lite
// @name            Classic Explorer Drag-n-Drop Lite
// @description     Restores Drag & Drop image from Win 95 or Win XP in Classic theme
// @version         1.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This is a maximally stripped down version of the mod "Classic Explorer Drag/Drop" by Isabella Lulamoon (@kawapure).
It is intended for restoring the Drag & Drop image of the items being dragged the way it was in Win 95 - Win XP.

The differences with the original mod are as follows:

* This mod does not download the debug symbols

* This mod hooks only a WinAPI function, thus architecture-independent (can work on ARM, for instance)

* From the user's perspective this mod enables effect of dragged items in cases where the original mod suppresses. 
So, expect visibility of the dragged items in more use cases.
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>

using OpenThemeData_t = decltype(&OpenThemeData);
OpenThemeData_t OpenThemeData_orig;
HTHEME WINAPI OpenThemeData_hook(HWND hwnd, LPCWSTR pszClassList)
{
    if (!wcscmp(pszClassList, L"DragDrop")) return (HTHEME)1;
    return OpenThemeData_orig(hwnd, pszClassList);
}

BOOL Wh_ModInit()
{
    Wh_SetFunctionHook((void *)OpenThemeData, (void *)OpenThemeData_hook, (void **)&OpenThemeData_orig);
    return TRUE;
}
