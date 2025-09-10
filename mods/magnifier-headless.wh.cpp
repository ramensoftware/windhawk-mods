// ==WindhawkMod==
// @id              magnifier-headless
// @name            Magnifier Headless Mode
// @description     Blocks the Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
// @version         0.6.0
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
#include <windhawk_api.h>


// Function to check if a window is the Magnifier window by its class name.
BOOL IsMagnifierWindow(HWND hwnd) {
    wchar_t className[256] = {0};
    if (!GetClassNameW(hwnd, className, sizeof(className) / sizeof(wchar_t))) {
        return FALSE;
    }

    // Check for known Magnifier class names. Both "MagUIClass" and "ScreenMagnifierUIWnd"
    // are checked to support different versions of Windows where the class name for
    // the Magnifier window may vary.
    return (wcscmp(className, L"MagUIClass") == 0 ||
            wcscmp(className, L"ScreenMagnifierUIWnd") == 0);
}

// ShowWindow hook to catch attempts to show the Magnifier window.
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // If it's a Magnifier window and the command is to show it, hide it instead.
    if (IsMagnifierWindow(hWnd) && nCmdShow != SW_HIDE) {
        return ShowWindow_Original(hWnd, SW_HIDE);
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

// SetWindowPos hook to catch attempts to show the Magnifier window via position changes.
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    // If it's a Magnifier window and the command is to show it, hide it instead.
    if (IsMagnifierWindow(hWnd)) {
        if (uFlags & SWP_SHOWWINDOW) {
            uFlags &= ~SWP_SHOWWINDOW;
            uFlags |= SWP_HIDEWINDOW;
        }
    }

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// SetWindowLongPtrW hook to catch attempts to make the window visible or add it to the taskbar.
using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
SetWindowLongPtrW_t SetWindowLongPtrW_Original;
LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (IsMagnifierWindow(hWnd)) {
        // When changing the standard window style, ensure WS_VISIBLE is removed.
        if (nIndex == GWL_STYLE) {
            if (dwNewLong & WS_VISIBLE) {
                dwNewLong &= ~WS_VISIBLE;
            }
        }
        // When changing the extended window style, ensure WS_EX_APPWINDOW is removed.
        if (nIndex == GWL_EXSTYLE) {
            if (dwNewLong & WS_EX_APPWINDOW) {
                dwNewLong &= ~WS_EX_APPWINDOW;
            }
        }
    }
    return SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong);
}

// CreateWindowExW hook to catch Magnifier window creation.
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
    // Check if the class name indicates a Magnifier window.
    // We must also check if lpClassName is a string pointer, not an atom.
    BOOL isMagnifierClass = FALSE;
    if (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) {
        // Both "MagUIClass" and "ScreenMagnifierUIWnd" are checked to support
        // different versions of Windows.
        if (wcscmp(lpClassName, L"MagUIClass") == 0 ||
            wcscmp(lpClassName, L"ScreenMagnifierUIWnd") == 0) {
            isMagnifierClass = TRUE;
        }
    }

    // If it is a Magnifier window, create it initially hidden and without the taskbar icon.
    if (isMagnifierClass) {
        dwStyle &= ~WS_VISIBLE;
        dwExStyle &= ~WS_EX_APPWINDOW;
    }

    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    // As a fallback, if a Magnifier window was created, hide it.
    // This handles cases where the window is created via other means or if the style modification fails.
    if (hwnd && (isMagnifierClass || IsMagnifierWindow(hwnd))) {
        ShowWindow(hwnd, SW_HIDE);
    }

    return hwnd;
}

// Mod initialization
BOOL Wh_ModInit() {
    // Hide any Magnifier windows that might already be open when the mod is loaded.
    HWND hwnd = FindWindowW(L"MagUIClass", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }

    hwnd = FindWindowW(L"ScreenMagnifierUIWnd", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }

    return TRUE;
}

// Mod uninitialization
void Wh_ModUninit() {
}

// Set up hooks before symbol loading.
BOOL Wh_ModBeforeSymbolLoading() {
    // Hook ShowWindow to prevent the Magnifier window from being shown.
    if (!Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "ShowWindow"),
        (void*)ShowWindow_Hook,
        (void**)&ShowWindow_Original)) {
        Wh_Log(L"Failed to hook ShowWindow");
        return FALSE;
    }

    // Hook SetWindowPos as it can also be used to show windows.
    if (!Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowPos"),
        (void*)SetWindowPos_Hook,
        (void**)&SetWindowPos_Original)) {
        Wh_Log(L"Failed to hook SetWindowPos");
        return FALSE;
    }

    // Hook SetWindowLongPtrW to prevent style changes from making the window visible.
    if (!Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowLongPtrW"),
        (void*)SetWindowLongPtrW_Hook,
        (void**)&SetWindowLongPtrW_Original)) {
        Wh_Log(L"Failed to hook SetWindowLongPtrW");
        return FALSE;
    }

    // Hook CreateWindowExW to prevent the Magnifier window from being created visible.
    if (!Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExW"),
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}