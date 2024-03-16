// ==WindhawkMod==
// @id              hide-action-center-icon
// @name            Hide Action Center icon
// @description     Hides Action Center icon from Win10 taskbar
// @version         1.0.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/* 
Removes the Action Center icon (cogwheel) from Win10 taskbar. After application of the mod,
File Explorer should be restarted.
The code is based on the corresponding code from Explorer Patcher and can stop working in the future.
*/
// ==/WindhawkModReadme==

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,
DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {

	HWND hWnd = CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
	if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"ControlCenterButton"))
    {   
        BYTE* lpDisplayCCButton = (BYTE*)(GetWindowLongPtrW(hWnd, 0) + 120);
        *lpDisplayCCButton = FALSE;
    }
    return hWnd;
}

BOOL Wh_ModInit(void)
{
        Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);

    return TRUE;
}
