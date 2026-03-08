// ==WindhawkMod==
// @id              photos-fullscreen
// @name            Photos Fullscreen Mode
// @description     Forces Windows Photos app to open in fullscreen mode instead of windowed mode
// @version         1.0.2
// @author          mak7im01
// @github          https://github.com/mak7im01
// @include         Microsoft.Photos.exe
// @include         Photos.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Photos Fullscreen Mode

This mod automatically opens the Windows Photos app in fullscreen (maximized) mode.

## How it works

The mod intercepts the creation of the Photos app main window and automatically applies the maximized window style, forcing the application to open in fullscreen.

## Compatibility

- Windows 10
- Windows 11
- Photos app (Microsoft.Photos.exe / Photos.exe)

## Notes

After installing this mod, the Photos app will automatically open in fullscreen mode. You can still manually resize or minimize the window if needed.
*/
// ==/WindhawkModReadme==



#include <windows.h>

// Original CreateWindowExW function
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

// Original ShowWindow function
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;



// Hook for CreateWindowExW - intercept window creation
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
    // If this is a top-level window (no parent)
    if (hWndParent == NULL) {
        // Check that this is not a system window
        if (lpClassName && lpWindowName) {
            // Add WS_MAXIMIZE style to maximize the window
            dwStyle |= WS_MAXIMIZE;
            
            Wh_Log(L"Intercepted window creation: %s", lpWindowName);
        }
    }
    
    // Call the original function with modified parameters
    return CreateWindowExW_Original(
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

// Hook for ShowWindow - additional maximization when showing window
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // Check that this is a top-level application window
    if (hWnd && GetParent(hWnd) == NULL) {
        WCHAR className[256];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0) {
            // Maximize window on first show
            if (nCmdShow == SW_SHOW || nCmdShow == SW_SHOWNORMAL) {
                nCmdShow = SW_SHOWMAXIMIZED;
                Wh_Log(L"Maximizing window: %s", className);
            }
        }
    }
    
    // Call the original function
    return ShowWindow_Original(hWnd, nCmdShow);
}

// Mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Photos Fullscreen Mode - Initializing");
    
    // Set hook for CreateWindowExW
    Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original
    );
    
    // Set hook for ShowWindow
    Wh_SetFunctionHook(
        (void*)ShowWindow,
        (void*)ShowWindow_Hook,
        (void**)&ShowWindow_Original
    );
    
    Wh_Log(L"Photos Fullscreen Mode - Initialized successfully");
    return TRUE;
}

// Called after initialization
void Wh_ModAfterInit() {
    Wh_Log(L"Photos Fullscreen Mode - After init");
    
    // Maximize all existing application windows
    HWND hWnd = GetTopWindow(NULL);
    while (hWnd) {
        DWORD processId;
        GetWindowThreadProcessId(hWnd, &processId);
        
        if (processId == GetCurrentProcessId() && GetParent(hWnd) == NULL) {
            if (IsWindowVisible(hWnd)) {
                ShowWindow(hWnd, SW_MAXIMIZE);
                Wh_Log(L"Maximized existing window");
            }
        }
        
        hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
    }
}

// Mod uninitialization
void Wh_ModUninit() {
    Wh_Log(L"Photos Fullscreen Mode - Uninitializing");
}
