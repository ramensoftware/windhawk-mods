// ==WindhawkMod==
// @id              chrome-top-resize-blocker
// @name            Chrome Top Resize Blocker
// @description     Prevents resizing and blocks the mouse cursor in a small zone at the top edge of a non-maximized Chrome window when it's positioned at the top of the screen. Useful for easily accessing browser tabs without accidentally resizing the window.
// @version         1.0
// @author          r3con
// @github          https://github.com/r3conx
// @twitter         https://twitter.com/x_r3con
// @include         chrome.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- protectedZonePixels: 7
  $name: Protected Top Zone (pixels)
  $description: The height of the protected zone at the top of the window. Resizing and mouse input will be blocked in this zone when the window is positioned at the top of the screen. A full restart of Chrome is required for changes to this setting to take effect.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>

// Standard macros for extracting coordinates from LPARAM, defined for compatibility.
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

// Global struct to hold the mod's settings.
struct {
    int protectedZonePixels;
} settings;

#define SUBCLASS_ID 1001

// Checks if the conditions for blocking are met for a given window.
BOOL ShouldApplyBlocking(HWND hwnd) {
    // First, check the window's placement.
    WINDOWPLACEMENT placement = {0};
    placement.length = sizeof(WINDOWPLACEMENT);
    if (!GetWindowPlacement(hwnd, &placement)) {
        return FALSE;
    }
    
    // If the window is maximized, we should not block anything.
    if (placement.showCmd == SW_SHOWMAXIMIZED) {
        return FALSE;
    }
    
    // Next, check if the window is positioned at the top of the screen.
    RECT winRect;
    if (!GetWindowRect(hwnd, &winRect)) {
        return FALSE;
    }
    
    // Find the monitor the window is currently on to get accurate work area coordinates.
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    
    if (GetMonitorInfo(hMonitor, &monitorInfo)) {
        // Check if the window's top edge is within the activation threshold of the monitor's work area top.
        int distanceFromTop = winRect.top - monitorInfo.rcWork.top;
        if (distanceFromTop > settings.protectedZonePixels) {
            return FALSE; // The window is not close enough to the top, so no blocking is needed.
        }
    }
    
    // The window is not maximized and is at the top of the screen, so blocking should be applied.
    return TRUE;
}

// Subclass procedure for the main Chrome window to intercept messages.
LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_NCHITTEST: {
            // Handle non-client hit testing to prevent resizing from the top.
            if (ShouldApplyBlocking(hwnd)) {
                // First, let the default procedure determine the hit-test result.
                LRESULT result = DefSubclassProc(hwnd, uMsg, wParam, lParam);
                
                // If the result indicates the top resize handles...
                if (result == HTTOP || result == HTTOPLEFT || result == HTTOPRIGHT) {
                    POINT cursor;
                    GetCursorPos(&cursor);
                    RECT winRect;
                    GetWindowRect(hwnd, &winRect);
                    
                    // ...and the cursor is within the protected pixel zone...
                    if (cursor.y < winRect.top + settings.protectedZonePixels) {
                        // ...then change the result to HTCLIENT, treating it as the client area.
                        // This effectively disables resizing from the top edge.
                        return HTCLIENT;
                    }
                }
                return result;
            }
            break;
        }
        
        case WM_MOUSEMOVE: {
            // When the mouse moves within the client area...
            if (ShouldApplyBlocking(hwnd)) {
                POINT pt;
                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);
                
                // ...check if the cursor is in the protected zone.
                if (pt.y < settings.protectedZonePixels) {
                    // Convert the client coordinates to screen coordinates.
                    ClientToScreen(hwnd, &pt);
                    
                    RECT winRect;
                    GetWindowRect(hwnd, &winRect);
                    
                    // Move the cursor just below the protected zone.
                    int newY = winRect.top + settings.protectedZonePixels;
                    SetCursorPos(pt.x, newY);
                    return 0; // Consume the message.
                }
            }
            break;
        }
        
        case WM_NCMOUSEMOVE: {
            // Also handle mouse movement in the non-client area (e.g., title bar).
            if (ShouldApplyBlocking(hwnd)) {
                POINT cursor;
                GetCursorPos(&cursor);
                RECT winRect;
                GetWindowRect(hwnd, &winRect);
                
                // If the cursor enters the protected zone from the non-client area...
                if (cursor.y < winRect.top + settings.protectedZonePixels) {
                    // ...force it to a position just below the zone.
                    SetCursorPos(cursor.x, winRect.top + settings.protectedZonePixels);
                    return 0; // Consume the message.
                }
            }
            break;
        }
        
        case WM_SETCURSOR: {
            // A fallback mechanism to ensure the cursor is repositioned if other messages fail.
            if (ShouldApplyBlocking(hwnd)) {
                POINT cursor;
                GetCursorPos(&cursor);
                RECT winRect;
                GetWindowRect(hwnd, &winRect);
                
                if (cursor.y < winRect.top + settings.protectedZonePixels) {
                    SetCursorPos(cursor.x, winRect.top + settings.protectedZonePixels);
                }
            }
            break;
        }
        
        case WM_NCDESTROY:
            // Clean up by removing the subclass when the window is destroyed.
            RemoveWindowSubclass(hwnd, SubclassProc, uIdSubclass);
            break;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Define a type for the original CreateWindowExW function pointer.
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

// We hook CreateWindowExW to find and subclass the main Chrome window.
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
    // Call the original function to create the window first.
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    // We are interested in top-level (no parent) and resizable (WS_THICKFRAME) windows.
    if (hwnd && !hWndParent && (dwStyle & WS_THICKFRAME)) {
        WCHAR className[256];
        // Check if the class name contains "Chrome" to identify the correct window.
        if (GetClassNameW(hwnd, className, 256) > 0 && wcsstr(className, L"Chrome")) {
            // If all conditions match, apply our subclass.
            SetWindowSubclass(hwnd, SubclassProc, SUBCLASS_ID, 0);
            Wh_Log(L"Subclassed Chrome window: %s", className);
        }
    }
    return hwnd;
}

// Load settings from Windhawk.
void LoadSettings() {
    settings.protectedZonePixels = Wh_GetIntSetting(L"protectedZonePixels");
    // Provide a safe default if the setting is missing or invalid.
    if (settings.protectedZonePixels <= 0) settings.protectedZonePixels = 7;
    
    Wh_Log(L"Settings loaded: protectedZonePixels=%d", settings.protectedZonePixels);
}

// The main entry point for the mod.
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Chrome Top Resize Blocker...");
    LoadSettings();
    // Hook the window creation function.
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);
    return TRUE;
}

// Called when the mod is unloaded.
void Wh_ModUninit() {
    Wh_Log(L"Unloading Chrome Top Resize Blocker...");
    // The hook is automatically removed by Windhawk.
}

// Called when the user changes settings for the mod.
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading for new windows...");
    LoadSettings();
}
