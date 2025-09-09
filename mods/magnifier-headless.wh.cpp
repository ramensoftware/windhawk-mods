// ==WindhawkMod==
// @id              magnifier-headless
// @name            Magnifier Headless Mode
// @description     Blocks the Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
// @version         0.1.3
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         magnify.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Blocks the Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
*/
// ==/WindhawkModReadme==

#include <windows.h>

// ShowWindow hook to catch magnifier show attempts
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // Check if it's a magnifier window by class name
    wchar_t className[256] = {0};
    if (GetClassNameW(hWnd, className, 256)) {
        if (wcscmp(className, L"MagUIClass") == 0) {
            // If magnifier trying to show, hide it instead
            if (nCmdShow == SW_SHOW || nCmdShow == SW_RESTORE || nCmdShow == SW_MAXIMIZE) {
                return ShowWindow_Original(hWnd, SW_HIDE);
            }
        }
    }
    
    return ShowWindow_Original(hWnd, nCmdShow);
}

// CreateWindowExW hook to catch magnifier window creation
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
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
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );
    
    // Check if lpClassName is a valid string pointer
    if (hwnd && lpClassName) {
        BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
        if (bTextualClassName && wcscmp(lpClassName, L"MagUIClass") == 0) {
            ShowWindow(hwnd, SW_HIDE);
        }
    }
    
    return hwnd;
}

// Mod initialization
BOOL Wh_ModInit() {
    // Set up hooks using Windhawk API
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "ShowWindow"),
        (void*)ShowWindow_Hook,
        (void**)&ShowWindow_Original
    );
    
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExW"),
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original
    );
    
    // Initial hide of any existing magnifier windows
    HWND hwnd = FindWindowW(L"MagUIClass", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }
    
    return TRUE;
}

// Cleanup
void Wh_ModUninit() {
    // Restore magnifier window if it exists
    HWND hwnd = FindWindowW(L"MagUIClass", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
    }
}