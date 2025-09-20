// ==WindhawkMod==
// @id              remove-fullscreen-popup-chrome
// @name            Remove "Exit Fullscreen" popup in Chrome
// @description     Removes “TO EXIT FULLSCREEN PRESS ESC/F11” popup in Chrome/Opera/OperaGX/Edge/Brave
// @version         1.2
// @author          lorenzoc01
// @github          https://github.com/lorenzoc01
// @include         opera.exe
// @include         chrome.exe
// @include         msedge.exe
// @include         brave.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove "Exit Fullscreen" popup in Chrome/Opera/OperaGX/Edge/Brave

A simple mod to remove the annoying “TO EXIT FULLSCREEN PRESS ESC/F11” popup in Chromium-based browsers like Chrome, Opera, OperaGX, Edge and Brave.

![Chrome](https://raw.githubusercontent.com/lorenzoc01/remove-chrome-fullscreen-popup/main/screenshots/chrome.png)
![OperaGX](https://raw.githubusercontent.com/lorenzoc01/remove-chrome-fullscreen-popup/main/screenshots/operagx.png)

## ⚠ Important

Based on testing, the mod works as intended. However, it **may also hide other windows** that share similar properties with the popup.

Check the [Github Repo](https://github.com/lorenzoc01/remove-chrome-fullscreen-popup) for more details.

*/
// ==/WindhawkModReadme==

#include <commctrl.h>
#include <windhawk_api.h>
#include <windows.h>
#include <cstddef>


using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t pOriginalShowWindow;
BOOL WINAPI ShowWindowHook(
    HWND hWnd,
    int nCmdShow)
{

    //According to my testings, the popup has
    // className == "Chrome_WidgetWin_1"
    // GWL_STYLE: WS_POPUP  (GWL_STYLE value: 0x86000000)
    // GWL_EXSTYLE: WS_EX_TOOLWINDOW, WS_EX_NOACTIVATE, WS_EX_TOPMOST, WS_EX_TRANSPARENT  (GWL_EXSTYLE value: 0x80000a8)
    
    // It is quite unlikely but if another window has the same properties, it will be hidden as well
    
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    if ((exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT)) == (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT)) {

        LONG style = GetWindowLong(hWnd, GWL_STYLE);
        if (style & WS_POPUP && !(style & WS_SYSMENU)) {

            wchar_t className[256] = {0};
            if (GetClassNameW(hWnd, className, _countof(className)) > 0) {
                if (wcscmp(className, L"Chrome_WidgetWin_1") == 0) {

                    Wh_Log(L"Not showing window: %llX", (unsigned long long)hWnd);
                    return TRUE;
                }
            }
        }
    }

    return pOriginalShowWindow(hWnd, nCmdShow);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Mod Init");

    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindowHook, (void**)&pOriginalShowWindow);

    return TRUE;
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Mod Uninit");
}

