// ==WindhawkMod==
// @id              remove-fullscreen-popup-chrome
// @name            Remove "To Exit Fullscreen Press Esc" popup - Chrome/Opera
// @description     Remove “TO EXIT FULLSCREEN PRESS ESC/F11” popup in Chrome/Opera/OperaGX
// @version         1.0
// @author          lorenzoc01
// @github          https://github.com/lorenzoc01
// @include         opera.exe
// @include         chrome.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove "To Exit Fullscreen Press Esc" popup – Chrome/Opera/OperaGX

A simple mod to remove the annoying “TO EXIT FULLSCREEN PRESS ESC/F11” popup in Chrome, Opera and OperaGX.

![Chrome](https://raw.githubusercontent.com/lorenzoc01/remove-chrome-fullscreen-popup/main/screenshots/chrome.png)
![OperaGX](https://raw.githubusercontent.com/lorenzoc01/remove-chrome-fullscreen-popup/main/screenshots/operagx.png)

## ⚠ Important

Based on testing, the mod works as intended. However, it **may also hide other windows** that share similar properties with the popup.

Check the [Github Repo](https://github.com/lorenzoc01/remove-chrome-fullscreen-popup) for more details.

*/
// ==/WindhawkModReadme==

#include <commctrl.h>
#include <windhawk_api.h>
#include <cstddef>

HWND popupWindowhandle = nullptr;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{

    HWND hWnd = pOriginalCreateWindowExW(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );

    //According to my testings, the popup has
    // hWndParent == nullptr
    // lpWindowName == nullptr
    // className == "Chrome_WidgetWin_1"
    // height == 0

    if (hWndParent == nullptr && lpWindowName == nullptr){ 
        wchar_t className[256] = {0};
        if (GetClassNameW(hWnd, className, _countof(className)) > 0) {
            if (wcscmp(className, L"Chrome_WidgetWin_1") == 0) {
                RECT rect;
                if (GetWindowRect(hWnd, &rect)) {
                    if (rect.bottom-rect.top == 0) {
                        Wh_Log(L"Found handle of popup window: %llX", (unsigned long long)hWnd);
                        popupWindowhandle = hWnd;
                    }
                }
            }
        }
    }

    return hWnd;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t pOriginalShowWindow;
BOOL WINAPI ShowWindowHook(
    HWND hWnd,
    int nCmdShow)
{                    
    if (hWnd == popupWindowhandle) {
        Wh_Log(L"Not showing window %llX", (unsigned long long)hWnd);
        return TRUE;
    }
    return pOriginalShowWindow(hWnd, nCmdShow);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Mod Init");

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindowHook, (void**)&pOriginalShowWindow);

    return TRUE;
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Mod Uninit");
    Wh_RemoveFunctionHook((void *)CreateWindowExW);
    Wh_RemoveFunctionHook((void*)ShowWindow);
}
