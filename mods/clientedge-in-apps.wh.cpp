// ==WindhawkMod==
// @id              clientedge-in-apps
// @name            Clientedge (3D border) in choosen apps
// @description     Adds WS_EX_CLIENTEDGE style to some apps to look better in Classic theme.
// @version         1.0.0
// @author          anixx
// @github          https://github.com/Anixx
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds WS_EX_CLIENTEDGE (3D border) to some apps to look better in Classic theme. Includes SysListView32 control in 
File Explorer, legacy (XP-like) file picker, Classic Notepad and Wolfram Mathematica.

Before:

![Before](https://i.imgur.com/dJBOnvP.png)

After:

![After](https://i.imgur.com/42jJDs6.png)
*/
// ==/WindhawkModReadme==

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,
DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam) {

    WCHAR wszClassName[256];
    ZeroMemory(wszClassName, 256);

    // Explorer and XP-like File Picker

    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"SysListView32")) {
        GetClassNameW(hWndParent, wszClassName, 256);
        if (!wcscmp(wszClassName, L"SHELLDLL_DefView")) {
            GetClassNameW(GetParent(hWndParent), wszClassName, 256);
            if (wcscmp(wszClassName, L"Progman")) {
                dwExStyle |= WS_EX_CLIENTEDGE;
            }
        }
    }

    // Classic Notepad

    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"Edit")) {
        GetClassNameW(hWndParent, wszClassName, 256);
        if (!wcscmp(wszClassName, L"Notepad")) {
            dwExStyle |= WS_EX_CLIENTEDGE;
        }
    }

    // Wolfram Mathematica

    if ((((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) && !wcscmp(lpClassName, L"NotebookContent")) {
        GetClassNameW(hWndParent, wszClassName, 256);
        if (!wcscmp(wszClassName, L"NotebookFrame")) {
            dwExStyle |= WS_EX_CLIENTEDGE;
        }
    }

    return CreateWindowExW_Orig(dwExStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

BOOL Wh_ModInit(void)
{
        Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);

    return TRUE;
}
