// ==WindhawkMod==
// @id              fix-strips
// @name            FixStrips WH Port
// @description     Port of the fixstrips AHK script that fixes some classic theme issues with Explorer7
// @version         1.0
// @author          OliveIsTyping + Anixx for the original script
// @github          https://github.com/OliviaIsTyping
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## NOTE:
You might need to restart explorer for changes to take affect
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==

// ==/WindhawkModSettings==
#include <windows.h>

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,
    HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {
        
    wchar_t wszClassName[200];
    ZeroMemory(wszClassName, 200);
    //Add WS_DLGFRAME to Shell_TrayWnd
    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"Shell_TrayWnd"))
    {
        dwStyle |= WS_DLGFRAME;
    }
    HWND hWnd = CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

    //Remove WS_DLGFRAME from Shell_TrayWnd (dont ask why it works but it does :D)
    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"Shell_TrayWnd"))
    {
       SetWindowLongPtrW(hWnd,GWL_STYLE,GetWindowLongPtrW(hWnd, GWL_STYLE) & ~WS_DLGFRAME);
    }

    return hWnd;
}


// The mod is being initialized, load settings, hook functions, and do other initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Orig);
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
