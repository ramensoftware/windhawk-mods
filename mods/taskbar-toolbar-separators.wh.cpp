// ==WindhawkMod==
// @id              taskbar-toolbar-separators
// @name            Taskbar Toolbar Separators
// @description     Enables seperators between taskbar toolbars
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Toolbar Separators
This mod enables seperators between taskbar toolbars.

**NOTE: YOU HAVE TO RESTART YOUR COMPUTER FOR THE MOD TO TAKE EFFECT!**

**Without** this mod:
![Screenshot](https://i.imgur.com/hcNVQgv.png)

**With** this mod:
![Screenshot](https://i.imgur.com/h2IfdOg.png)
*/
// ==/WindhawkModReadme==

#include "commctrl.h"

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND CreateWindowExWHook(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
)
{
    if ((*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd")) dwStyle |= RBS_BANDBORDERS;
    }

    return pOriginalCreateWindowExW(
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
}

using SetWindowLongW_t = decltype(&SetWindowLongW);
SetWindowLongW_t pOriginalSetWindowLongW;
LONG_PTR SetWindowLongWHook(
    HWND     hWnd,
    int      nIndex,
    LONG_PTR dwNewLong
)
{
    WCHAR lpClassName[200];
    ZeroMemory(lpClassName, 200);
    GetClassNameW(hWnd, lpClassName, 200);
    HWND hWndParent = GetParent(hWnd);

    if ((*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd"))
        {
            if (nIndex == GWL_STYLE) dwNewLong |= RBS_BANDBORDERS;
        }
    }

    return pOriginalSetWindowLongW(hWnd, nIndex, dwNewLong);
}



BOOL Wh_ModInit() {
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);
    Wh_SetFunctionHook((void*)SetWindowLongW, (void*)SetWindowLongWHook, (void**)&pOriginalSetWindowLongW);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
