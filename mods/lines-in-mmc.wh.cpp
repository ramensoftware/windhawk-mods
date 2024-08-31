// ==WindhawkMod==
// @id              lines-in-mmc
// @name            Lines in MMC and Device Manager
// @description     Adds dotted lines to the TreeView in Microsoft Management Console and Device Manager
// @version         1.0.1
// @author          anixx
// @github          https://github.com/Anixx
// @include         mmc.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds dotted lines to the TreeView in Microsoft Management Console and Device Manager

![Screenshot](https://i.imgur.com/3ZBR2HD.png)
*/
// ==/WindhawkModReadme==

#include <commctrl.h>

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,
DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {

    WCHAR wszClassName[256];
    ZeroMemory(wszClassName, 256);

    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"SysTreeView32")) {
        GetClassNameW(hWndParent, wszClassName, 256);
        if (!wcscmp(wszClassName, L"AfxOleControl42u") || !wcscmp(wszClassName, L"MMCViewWindow")) {
            dwStyle |= TVS_HASLINES;
        }
    }

    return CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

BOOL Wh_ModInit(void)
{
        Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);

    return TRUE;
}
