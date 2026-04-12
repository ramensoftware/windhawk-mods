// ==WindhawkMod==
// @id              resizable-discord
// @name            Discord Transparency Resize Fix
// @description     Forces discord to be resizable with vencord window transparency
// @version         1.0.0
// @author          gameknight963
// @include         discord.exe
// @github          https://github.com/Gameknight963
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod forces resizing on Discord when using window transparency + native titlebar.

How it works: It enables WS_THICKFRAME for proper resizing behavior,
which Discord disables because it uses a custom titlebar, and Electron handles
the resize logic through software in this custom titlebar mode.

When Vencord turns on custom titlebar, it doesn't set WS_THICKFRAME, so
the window is not resizable. This mod fixes that.

*/
// ==/WindhawkModReadme==

#include <windows.h>

void HookWindow(HWND hWnd)
{
    wchar_t className[256];
    GetClassNameW(hWnd, className, 256);
    if (wcscmp(className, L"Chrome_WidgetWin_1") != 0)
        return;

    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    SetWindowLongPtr(hWnd, GWL_STYLE, style | WS_THICKFRAME);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM) { HookWindow(hWnd); return TRUE; }

BOOL Wh_ModInit()
{
    EnumWindows(EnumWindowsProc, 0);
    return TRUE;
}