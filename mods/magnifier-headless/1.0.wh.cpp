// ==WindhawkMod==
// @id              magnifier-headless
// @name            Magnifier Headless Mode
// @description     Blocks the Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
// @version         1.0
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         magnify.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Magnifier Headless Mode

![Screenshot](https://i.imgur.com/m5w78pe.png)

Hides the Windows Magnifier window while keeping zoom functionality fully
working with Win + Plus and Win + Minus keyboard shortcuts.

## Features
- Completely hides the Magnifier UI window
- Removes the Magnifier icon from the taskbar and Alt+Tab
- Fixes the 5-6 second mouse cursor freeze on Magnifier startup (a known
  Windows bug caused by touch controls appearing, even on non-touch devices)
- Hides Magnifier touch overlay controls (semi-transparent squares at screen
  corners)
- Zoom keyboard shortcuts continue to work normally

## Usage
1. Install the mod in Windhawk
2. Launch Windows Magnifier (Win + Plus)
3. Use magnification as normal -- the UI window will not appear

### Keyboard Shortcuts
- **Win + Plus**: Zoom in
- **Win + Minus**: Zoom out
- **Win + Esc**: Exit Magnifier

## Compatibility
- Windows 10 and Windows 11
- Targets magnify.exe only
*/
// ==/WindhawkModReadme==

#include <windows.h>

// Global state
HWND g_hHostWnd = NULL;
BOOL g_bInitialized = FALSE;

// Check if a window belongs to the Magnifier UI
inline BOOL IsMagnifierWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return FALSE;
    }

    wchar_t className[64] = {0};
    if (GetClassNameW(hwnd, className, 64) == 0) {
        return FALSE;
    }

    return (wcscmp(className, L"MagUIClass") == 0 ||
            wcscmp(className, L"ScreenMagnifierUIWnd") == 0);
}

// Check if a window is the Magnifier Touch overlay
inline BOOL IsTouchOverlayWindow(HWND hwnd) {
    if (!hwnd) {
        return FALSE;
    }
    wchar_t title[64] = {0};
    return (GetWindowTextW(hwnd, title, 64) > 0 &&
            wcsstr(title, L"Magnifier Touch") != NULL);
}

// --- HOOKS ---

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original = nullptr;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (!g_bInitialized) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if ((IsMagnifierWindow(hWnd) || IsTouchOverlayWindow(hWnd)) && nCmdShow != SW_HIDE) {
        return TRUE;
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original = nullptr;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                               int cx, int cy, UINT uFlags) {
    if (!g_bInitialized) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    // Move touch overlay off-screen with zero size and force hidden
    if (IsTouchOverlayWindow(hWnd)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, -32000, -32000, 0, 0,
                                      uFlags | SWP_HIDEWINDOW);
    }

    if (IsMagnifierWindow(hWnd)) {
        uFlags &= ~SWP_SHOWWINDOW;
        uFlags |= SWP_HIDEWINDOW;
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
SetWindowLongPtrW_t SetWindowLongPtrW_Original = nullptr;
LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (!g_bInitialized) {
        return SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong);
    }

    // Hide touch overlay from Alt+Tab
    if (IsTouchOverlayWindow(hWnd) && nIndex == GWL_EXSTYLE) {
        dwNewLong &= ~WS_EX_APPWINDOW;
        dwNewLong |= WS_EX_TOOLWINDOW;
    }

    if (IsMagnifierWindow(hWnd)) {
        if (nIndex == GWL_STYLE) {
            dwNewLong &= ~WS_VISIBLE;
        } else if (nIndex == GWL_EXSTYLE) {
            dwNewLong &= ~WS_EX_APPWINDOW;
            dwNewLong |= WS_EX_TOOLWINDOW;
        }
    }
    return SetWindowLongPtrW_Original(hWnd, nIndex, dwNewLong);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;
HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam) {

    if (!g_bInitialized) {
        return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
            dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    BOOL isMagnifierClass = FALSE;
    BOOL isTouchOverlay = FALSE;

    // Check for touch overlay by window title
    if (lpWindowName && wcsstr(lpWindowName, L"Magnifier Touch") != NULL) {
        isTouchOverlay = TRUE;
        dwStyle &= ~WS_VISIBLE;
        dwExStyle &= ~WS_EX_APPWINDOW;
        dwExStyle |= WS_EX_TOOLWINDOW;
        Wh_Log(L"Magnifier Headless: Detected Magnifier Touch window");
    }

    // Check for magnifier window classes (only if class name is a string, not an atom)
    if (!isTouchOverlay && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0) {
        if (wcscmp(lpClassName, L"MagUIClass") == 0 ||
            wcscmp(lpClassName, L"ScreenMagnifierUIWnd") == 0) {
            isMagnifierClass = TRUE;
            dwStyle &= ~WS_VISIBLE;
            dwExStyle &= ~WS_EX_APPWINDOW;
            dwExStyle |= WS_EX_TOOLWINDOW;
            Wh_Log(L"Magnifier Headless: Intercepting window creation (class: %ls)",
                   lpClassName);
        }
    }

    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hwnd && isTouchOverlay) {
        // Force off-screen position, zero size, and hidden
        if (SetWindowPos_Original) {
            SetWindowPos_Original(hwnd, NULL, -32000, -32000, 0, 0,
                                  SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (ShowWindow_Original) {
            ShowWindow_Original(hwnd, SW_HIDE);
        }
    } else if (hwnd && isMagnifierClass) {
        // Reparent to hidden host window and hide
        if (g_hHostWnd) {
            SetParent(hwnd, g_hHostWnd);
        }
        if (ShowWindow_Original) {
            ShowWindow_Original(hwnd, SW_HIDE);
        }
    }

    return hwnd;
}

// Callback for hiding existing magnifier windows (scoped to current process)
BOOL CALLBACK EnumWindowsProc_HideMagnifier(HWND hwnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (IsMagnifierWindow(hwnd)) {
        Wh_Log(L"Magnifier Headless: Found existing magnifier window (HWND: 0x%p), hiding",
               hwnd);
        if (ShowWindow_Original) {
            ShowWindow_Original(hwnd, SW_HIDE);
        }
    }
    return TRUE;
}

// --- MOD LIFECYCLE ---

BOOL Wh_ModInit() {
    Wh_Log(L"Magnifier Headless: Initializing...");

    // Set up hooks
    if (!Wh_SetFunctionHook((void*)CreateWindowExW,
                             (void*)CreateWindowExW_Hook,
                             (void**)&CreateWindowExW_Original) ||
        !Wh_SetFunctionHook((void*)ShowWindow,
                             (void*)ShowWindow_Hook,
                             (void**)&ShowWindow_Original) ||
        !Wh_SetFunctionHook((void*)SetWindowPos,
                             (void*)SetWindowPos_Hook,
                             (void**)&SetWindowPos_Original) ||
        !Wh_SetFunctionHook((void*)SetWindowLongPtrW,
                             (void*)SetWindowLongPtrW_Hook,
                             (void**)&SetWindowLongPtrW_Original)) {
        Wh_Log(L"Magnifier Headless: Failed to set up hooks");
        return FALSE;
    }

    // Create hidden message-only host window
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = L"MagnifierHeadlessHost";
    wc.hInstance = GetModuleHandle(NULL);

    if (!RegisterClassW(&wc)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Magnifier Headless: Failed to register window class");
            return FALSE;
        }
    }

    g_hHostWnd = CreateWindowExW(0, wc.lpszClassName, NULL, 0,
        0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
    if (!g_hHostWnd) {
        Wh_Log(L"Magnifier Headless: Failed to create host window");
        return FALSE;
    }

    g_bInitialized = TRUE;

    Wh_Log(L"Magnifier Headless: Initialization complete");
    return TRUE;
}

// Called by Windhawk after all hooks are activated and original function
// pointers are valid. Safe to call ShowWindow_Original here.
void Wh_ModAfterInit() {
    Wh_Log(L"Magnifier Headless: Hiding existing magnifier windows...");
    EnumWindows(EnumWindowsProc_HideMagnifier, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Magnifier Headless: Uninitializing - terminating process for clean state");
    // Restoring state (unhiding windows, unregistering class, destroying the
    // host window from the correct thread) is not feasible. Terminate so that
    // magnify.exe can be restarted cleanly without the mod.
    TerminateProcess(GetCurrentProcess(), 0);
}
