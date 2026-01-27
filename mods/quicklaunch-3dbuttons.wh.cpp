// ==WindhawkMod==
// @id              quicklaunch-3dbuttons
// @name            3D buttons on Quick Launch bar
// @description     Makes the buttons on the Quick Launch bar 3-dimensional
// @version         1.0.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==

/*
Makes the buttons on the Quick Launch bar 3-dimensional, as it was the case in Windows 95 Beta.

Before:

![Before](https://i.imgur.com/4ccDO6y.png)

After:

![After](https://i.imgur.com/LbrokEc.png)

*/

// ==/WindhawkModReadme==

#include <commctrl.h>

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,
DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {

    WCHAR wszClassName[256];
    ZeroMemory(wszClassName, 256);

    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"ToolbarWindow32")) { 
        GetClassNameW(hWndParent, wszClassName, 256);
        if (!wcscmp(wszClassName, L"ReBarWindow32")) { 
            GetClassNameW(GetParent(hWndParent), wszClassName, 256);
            if (!wcscmp(wszClassName, L"Shell_TrayWnd")) {
                dwStyle &= ~TBSTYLE_FLAT;
            }
        }
    }

    return CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

BOOL Wh_ModInit(void)
{
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);

    return TRUE;
}
