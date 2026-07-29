// ==WindhawkMod==
// @id              startmenu-from-top-fork
// @name            Start Menu From Top - Fork
// @description     Makes the Windows Start menu appear from the top of the screen instead of the bottom
// @version         1.0.0
// @author          froggy.codes       
// @github          computerprogrammingfrog
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -std=c++17
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu From Top

A Windhawk mod that makes the Windows Start menu appear from the top of the screen instead of the bottom.

## Features

- **Top-aligned Start menu** — Opens from the top of the screen
- **Automatic detection** — Works with Windows 10 and Windows 11
- **Seamless integration** — No performance impact

## How It Works

This mod intercepts window positioning calls and redirects the Start menu to appear at the top of your screen instead of the traditional bottom position.

## Compatibility

- Windows 10 ✓
- Windows 11 ✓
- Architecture: x86-64
*/
// ==/WindhawkModReadme==


#include <winapifamily.h>
#include <winuser.h>
#include <windows.h>

typedef BOOL (WINAPI *SetWindowPos_t)(
    HWND hWnd,
    HWND hWndInsertAfter,
    int X,
    int Y,
    int cx,
    int cy,
    UINT uFlags
);

SetWindowPos_t pOriginalSetWindowPos = nullptr;

BOOL WINAPI SetWindowPos_Hook(
    HWND hWnd,
    HWND hWndInsertAfter,
    int X,
    int Y,
    int cx,
    int cy,
    UINT uFlags
) {
    // Get the window class name
    WCHAR szClassName[256] = L"";
    GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName));

    // Check if this is the Start menu window
    if (wcscmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0 ||
        wcscmp(szClassName, L"ApplicationFrameWindow") == 0) {
        
        // Get screen dimensions
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        
        // If the menu is being positioned at the bottom (high Y value),
        // move it to the top instead
        if (Y > screenHeight / 2) {
            Y = 0;
        }
    }

    return pOriginalSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

void Mod_Initialize() {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    pOriginalSetWindowPos = (SetWindowPos_t)GetProcAddress(hUser32, "SetWindowPos");

    if (pOriginalSetWindowPos) {
        Wh_SetFunctionHook((void *)pOriginalSetWindowPos, (void *)SetWindowPos_Hook, 
                           (void **)&pOriginalSetWindowPos);
    }
}

void Mod_Uninitialize() {
    // Cleanup is handled automatically by Windhawk
}
