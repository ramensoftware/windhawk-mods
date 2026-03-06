// ==WindhawkMod==
// @id              explorer-transparent-bg
// @name            File Explorer Transparent Background
// @description     Makes the File Explorer windows transparent (CabinetWClass).
// @version         1.0.0
// @author          SenkuTheGreat
// @github          https://github.com/senkuthegreat
// @include         explorer.exe
// @compilerOptions -luser32 -ldwmapi
// ==/WindhawkMod==

#include <windows.h>
#include <dwmapi.h>

// Set the desired opacity level here (0 = totally transparent, 255 = fully opaque)
const int OPACITY_LEVEL = 200; 

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

// Helper function to apply transparency to a specific window
void ApplyTransparency(HWND hWnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_LAYERED)) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }
    SetLayeredWindowAttributes(hWnd, 0, OPACITY_LEVEL, LWA_ALPHA);
}

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
        // Safely check if the window being created is a File Explorer window (CabinetWClass)
        // HIWORD check prevents crashing if lpClassName is passed as a resource ID instead of a string
        if (HIWORD(lpClassName) != 0 && wcscmp(lpClassName, L"CabinetWClass") == 0)
        {
            ApplyTransparency(hWnd);
        }
    }
    
    return hWnd;
}

// Callback to apply transparency to File Explorer windows that are ALREADY open
BOOL CALLBACK EnumWindowsProcInit(HWND hWnd, LPARAM lParam) {
    WCHAR className[256];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"CabinetWClass") == 0) {
            ApplyTransparency(hWnd);
        }
    }
    return TRUE;
}

// Callback to remove transparency if you disable the mod
BOOL CALLBACK EnumWindowsProcUninit(HWND hWnd, LPARAM lParam) {
    WCHAR className[256];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"CabinetWClass") == 0) {
            LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        }
    }
    return TRUE;
}

// Windhawk initialization function
BOOL Wh_ModInit()
{
    // Apply the hook to CreateWindowExW for newly opened windows
    Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original
    );
    
    // Apply transparency to any Explorer windows you already have open
    EnumWindows(EnumWindowsProcInit, 0);
    
    return TRUE;
}

// Windhawk uninitialization function
void Wh_ModUninit()
{
    // Remove the hook when the mod is unloaded
    Wh_RemoveFunctionHook((void*)CreateWindowExW);
    
    // Remove transparency from existing windows so they return to normal
    EnumWindows(EnumWindowsProcUninit, 0);
}
