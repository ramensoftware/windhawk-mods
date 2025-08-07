// ==WindhawkMod==
// @id              explorer-no-toolbars-bottom-gripper
// @name            Explorer Unlocked Toolbars Fix (WINAPI)
// @description     Removes bottom gripper from unlocked Explorer toolbars without the need to download symbols
// @version         1.0.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod does the same as the mod `Explorer Unlocked Toolbars Fix` but does so by hooking only a WINAPI function, 
so does not need the download of the debugging symbols.
*/
// ==/WindhawkModReadme==

#include <CommCtrl.h>

LRESULT CALLBACK RebarSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData
)
{
    if (uMsg == WM_NCDESTROY)
    {
        RemoveWindowSubclass(hWnd, RebarSubclassProc, 1);
    }
    else if (uMsg == RB_SETEXTENDEDSTYLE) 
	{
		return 0;
	}

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,
DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {

    HWND hWnd = CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && (!wcscmp(lpClassName, L"ReBarWindow32"))) {
		SetWindowSubclass(hWnd, RebarSubclassProc, 1, 0);
    }

     return hWnd;
}

BOOL Wh_ModInit(void)
{
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);
    return TRUE;
}
