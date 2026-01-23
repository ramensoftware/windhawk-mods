// ==WindhawkMod==
// @id              explorer-tab-utility-window-hider
// @name            Explorer Tab Utility Window Hider
// @description     Hides the Explorer Tab Utility window completely, allowing it to run only in the background
// @version         1.0.1
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         ExplorerTabUtility.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Tab Utility Window Hider

This mod prevents w4po's Explorer Tab Utility (https://github.com/w4po/ExplorerTabUtility) from showing its window at startup and all times. The program will continue to run in the background with all its functionality, but the configuration window will never appear.

## How it works

The mod hooks the following Windows API functions:
- `ShowWindow` / `ShowWindowAsync` - Blocks any attempt to show the window
- `CreateWindowExW` - Hides the main window immediately after creation
- `SetWindowPos` - Prevents the window from being made visible through positioning

## Target Process

This mod only targets `ExplorerTabUtility.exe` and has no effect on other programs.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

// Flag to track if we've found the main window
HWND g_mainWindow = NULL;

// Check if a window belongs to the main application window class
bool IsMainAppWindow(HWND hWnd) {
    if (!hWnd) return false;

    // Check window class name
    WCHAR className[256];
    if (GetClassNameW(hWnd, className, 256) > 0) {
        // Common WPF/WinForms window class names
        if (wcsstr(className, L"HwndWrapper") ||      // WPF
            wcsstr(className, L"WindowsForms") ||     // WinForms
            wcsstr(className, L"Window") ||
            wcscmp(className, L"#32770") == 0) {      // Dialog
            return true;
        }
    }

    // Check if it's a top-level window with a title
    HWND parent = GetParent(hWnd);
    if (parent == NULL || parent == GetDesktopWindow()) {
        WCHAR title[256];
        if (GetWindowTextW(hWnd, title, 256) > 0) {
            // Check for Explorer Tab Utility in the title
            if (wcsstr(title, L"Explorer Tab") ||
                wcsstr(title, L"ExplorerTab")) {
                return true;
            }
        }

        // Also check by window style - main windows typically have these
        LONG style = GetWindowLong(hWnd, GWL_STYLE);
        if ((style & WS_CAPTION) && (style & WS_SYSMENU)) {
            return true;
        }
    }

    return false;
}

// Hook for ShowWindow
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // Block showing the main window
    if (IsMainAppWindow(hWnd)) {
        // If trying to show the window, hide it instead
        if (nCmdShow != SW_HIDE && nCmdShow != SW_MINIMIZE) {
            Wh_Log(L"Blocked ShowWindow for main window (nCmdShow=%d)", nCmdShow);
            // Store the main window handle for later use
            g_mainWindow = hWnd;
            // Hide the window instead
            return ShowWindow_Original(hWnd, SW_HIDE);
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

// Hook for ShowWindowAsync
using ShowWindowAsync_t = decltype(&ShowWindowAsync);
ShowWindowAsync_t ShowWindowAsync_Original;

BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int nCmdShow) {
    if (IsMainAppWindow(hWnd)) {
        if (nCmdShow != SW_HIDE && nCmdShow != SW_MINIMIZE) {
            Wh_Log(L"Blocked ShowWindowAsync for main window (nCmdShow=%d)", nCmdShow);
            g_mainWindow = hWnd;
            return ShowWindowAsync_Original(hWnd, SW_HIDE);
        }
    }
    return ShowWindowAsync_Original(hWnd, nCmdShow);
}

// Hook for CreateWindowExW
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X, int Y,
    int nWidth, int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    bool isMainWindow = false;

    // Check if this looks like a main application window
    if (lpWindowName && (wcsstr(lpWindowName, L"Explorer Tab") ||
                         wcsstr(lpWindowName, L"ExplorerTab"))) {
        isMainWindow = true;
    }

    // Check if it's a top-level window with standard window styles
    if (!hWndParent && (dwStyle & WS_CAPTION) && (dwStyle & WS_SYSMENU)) {
        // Check class name for WPF/WinForms
        if (lpClassName && !IS_INTRESOURCE(lpClassName)) {
            if (wcsstr(lpClassName, L"HwndWrapper") ||
                wcsstr(lpClassName, L"WindowsForms")) {
                isMainWindow = true;
            }
        }
    }

    if (isMainWindow) {
        Wh_Log(L"Creating main window with WS_EX_TOOLWINDOW and removing WS_VISIBLE");
        // Remove visible style and add tool window style (won't show in taskbar)
        dwStyle &= ~WS_VISIBLE;
        dwExStyle |= WS_EX_TOOLWINDOW;
    }

    HWND hWnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    if (hWnd && isMainWindow) {
        g_mainWindow = hWnd;
        // Ensure the window stays hidden
        ShowWindow_Original(hWnd, SW_HIDE);
        Wh_Log(L"Main window created and hidden: %p", hWnd);
    }

    return hWnd;
}

// Hook for SetWindowPos to prevent window from being made visible
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

BOOL WINAPI SetWindowPos_Hook(
    HWND hWnd,
    HWND hWndInsertAfter,
    int X, int Y,
    int cx, int cy,
    UINT uFlags
) {
    if (IsMainAppWindow(hWnd)) {
        // If trying to show the window, add the NOACTIVATE and HIDEWINDOW flags
        if (!(uFlags & SWP_HIDEWINDOW)) {
            Wh_Log(L"Blocked SetWindowPos show for main window");
            uFlags |= SWP_HIDEWINDOW;
            uFlags &= ~SWP_SHOWWINDOW;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// Hook for SetForegroundWindow to prevent window from being brought to front
using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Original;

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (IsMainAppWindow(hWnd)) {
        Wh_Log(L"Blocked SetForegroundWindow for main window");
        return FALSE;
    }
    return SetForegroundWindow_Original(hWnd);
}

// Hook for BringWindowToTop
using BringWindowToTop_t = decltype(&BringWindowToTop);
BringWindowToTop_t BringWindowToTop_Original;

BOOL WINAPI BringWindowToTop_Hook(HWND hWnd) {
    if (IsMainAppWindow(hWnd)) {
        Wh_Log(L"Blocked BringWindowToTop for main window");
        return FALSE;
    }
    return BringWindowToTop_Original(hWnd);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Explorer Tab Utility Window Hider initializing...");

    // Hook ShowWindow - using type-safe WindhawkUtils wrapper
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);

    // Hook ShowWindowAsync
    WindhawkUtils::SetFunctionHook(ShowWindowAsync, ShowWindowAsync_Hook, &ShowWindowAsync_Original);

    // Hook CreateWindowExW
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);

    // Hook SetWindowPos
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original);

    // Hook SetForegroundWindow
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook, &SetForegroundWindow_Original);

    // Hook BringWindowToTop
    WindhawkUtils::SetFunctionHook(BringWindowToTop, BringWindowToTop_Hook, &BringWindowToTop_Original);

    Wh_Log(L"Explorer Tab Utility Window Hider initialized successfully");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Explorer Tab Utility Window Hider unloading...");

    // Note: Windhawk automatically unhooks all function hooks when the mod is unloaded
}
