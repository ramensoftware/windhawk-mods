// ==WindhawkMod==
// @id              notepad-transparent-bg
// @name            Notepad Transparent Background
// @description     Makes the Notepad background/window transparent.
// @version         1.0.0
// @author          SenkuTheGreat
// @github          https://github.com/senkuthegreat
// @include         Notepad.exe
// @compilerOptions -luser32 -ldwmapi
// ==/WindhawkMod==

#include <windows.h>
#include <dwmapi.h>

// Set the desired opacity level here (0 = totally transparent, 255 = fully opaque)
const int OPACITY_LEVEL = 200; 

// Keep track of the original CreateWindowExW function
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

// The hook function replacing CreateWindowExW
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
    LPVOID lpParam)
{
    // Call the original function to actually create the window
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, 
                                         X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    // If successful and it's a top-level window
    if (hWnd && hWndParent == NULL) 
    {
        // Add WS_EX_LAYERED to the window's extended style to allow transparency
        LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        if (!(exStyle & WS_EX_LAYERED))
        {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        }
        
        // Set the opacity using LWA_ALPHA
        SetLayeredWindowAttributes(hWnd, 0, OPACITY_LEVEL, LWA_ALPHA);
    }
    
    return hWnd;
}

// Windhawk initialization function
BOOL Wh_ModInit()
{
    // Apply the hook to CreateWindowExW
    Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original
    );
    
    return TRUE;
}

// Windhawk uninitialization function
void Wh_ModUninit()
{
    // Remove the hook when the mod is unloaded
    Wh_RemoveFunctionHook((void*)CreateWindowExW);
}
