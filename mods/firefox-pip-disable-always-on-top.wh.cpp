// ==WindhawkMod==
// @id              firefox-pip-disable-always-on-top
// @name            Disable Firefox Picture-in-Picture Always on Top
// @description     Removes the always-on-top behavior from Firefox PiP windows
// @version         1.0
// @author          ViNSaNiTY24
// @github          https://github.com/ViNSaNiTY24
// @include         firefox.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Firefox PiP - Disable Always on Top

Prevents ONLY Firefox Picture-in-Picture windows from staying always on top.
Uses hook + async verification for precision without blocking.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

// Thread function that waits and then checks if window is PiP
DWORD WINAPI DelayedPiPCheck(LPVOID lpParam) {
    HWND hWnd = (HWND)lpParam;
    
    // Wait for title to appear (check every 50ms, up to 500ms)
    for (int i = 0; i < 10; i++) {
        Sleep(50);
        
        if (!IsWindow(hWnd)) {
            return 0;  // Window closed
        }
        
        wchar_t windowTitle[128];
        int titleLen = GetWindowTextW(hWnd, windowTitle, 128);
        
        if (titleLen > 0) {
            // Title appeared - check if PiP
            if (wcsstr(windowTitle, L"Picture-in-Picture") != NULL) {
                // It's PiP - remove topmost
                LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
                if (exStyle & WS_EX_TOPMOST) {
                    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOPMOST);
                    SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, 
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    Wh_Log(L"Removed topmost from PiP window");
                }
            }
            return 0;  // Done - title appeared
        }
    }
    
    return 0;  // Timeout - no title, not PiP
}

BOOL WINAPI SetWindowPos_Hook(
    HWND hWnd,
    HWND hWndInsertAfter,
    int X,
    int Y,
    int cx,
    int cy,
    UINT uFlags
) {
    if (hWndInsertAfter == HWND_TOPMOST) {
        wchar_t className[32];
        if (GetClassNameW(hWnd, className, 32) > 0 && 
            wcscmp(className, L"MozillaDialogClass") == 0) {
            
            wchar_t windowTitle[128];
            int titleLen = GetWindowTextW(hWnd, windowTitle, 128);
            
            if (titleLen > 0) {
                // Already has title - check immediately
                if (wcsstr(windowTitle, L"Picture-in-Picture") != NULL) {
                    Wh_Log(L"Blocking PiP window from topmost");
                    hWndInsertAfter = HWND_NOTOPMOST;
                }
            } else {
                // No title yet - spawn thread to check later
                Wh_Log(L"MozillaDialogClass with no title, spawning check thread");
                HANDLE hThread = CreateThread(NULL, 0, DelayedPiPCheck, (LPVOID)hWnd, 0, NULL);
                if (hThread) {
                    CloseHandle(hThread);  // Let it run independently
                }
            }
        }
    }
    
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    
    if (!Wh_SetFunctionHook(
        (void*)SetWindowPos,
        (void*)SetWindowPos_Hook,
        (void**)&SetWindowPos_Original
    )) {
        Wh_Log(L"Failed to hook SetWindowPos");
        return FALSE;
    }
    
    Wh_Log(L"Initialized");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}