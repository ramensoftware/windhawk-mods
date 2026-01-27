// ==WindhawkMod==
// @id              explorer-toolbars-separators
// @name            Separators around File Explorer toolbars
// @description     Adds separators around File Explorer toolbars
// @version         1.0.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==

/* 

Adds separators around File Explorer toolbars, as in Windows 98 or Windows 2000 

Before:

![Before](https://i.imgur.com/MqcVeUW.png)

After:

![After](https://i.imgur.com/Xyzj4X2.png)

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
            if (!wcscmp(wszClassName, L"WorkerW")) {
                LONG style = GetWindowLongPtrW(hWndParent, GWL_STYLE);
                style |= RBS_BANDBORDERS;
                style |= WS_BORDER;
                SetWindowLongPtrW(hWndParent, GWL_STYLE, style);
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
