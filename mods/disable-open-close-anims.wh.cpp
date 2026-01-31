// ==WindhawkMod==
// @id              disable-open-close-anims
// @name            Disable Window Open/Close Animations
// @description     Disables Open/Close animations without affecting other window animations.
// @version         1.0
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         *
// @exclude         lsass.exe
// @exclude         csrss.exe
// @exclude         smss.exe
// @compilerOptions -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Window Open/Close Animations

This mod disables non-UWP app's window opening and closing animations without affecting other window animations, such as minimizing, restoring, maximizing, and snapping.

![](https://i.imgur.com/d9HvrJA.gif)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <dwmapi.h>

// ---------------------------------------------------------------------------
// CONFIGURATION
// ---------------------------------------------------------------------------
#define RESTORE_ANIM_DELAY_MS 50

// Using a unique timestamp-based ID to minimize collision risk with app timers.
// Original: 0xDEAD (potentially common). New: Epoch timestamp ~ Jan 2026.
#define TIMER_ID_RESTORE 1769817678 

#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif

// ---------------------------------------------------------------------------
// HELPERS
// ---------------------------------------------------------------------------

void DisableTransitions(HWND hWnd, BOOL disable) {
    if (IsWindow(hWnd)) {
        DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
    }
}

// CRITICAL: Prevents Explorer crashes by ignoring Shell windows
bool IsBlacklistedClass(HWND hWnd) {
    char className[256];
    if (GetClassNameA(hWnd, className, sizeof(className)) == 0) return false;

    if (strcmp(className, "Shell_TrayWnd") == 0) return true;      // Taskbar
    if (strcmp(className, "Progman") == 0) return true;            // Desktop
    if (strcmp(className, "WorkerW") == 0) return true;            // Desktop Worker
    if (strcmp(className, "Windows.UI.Core.CoreWindow") == 0) return true; // Start Menu / Modern Apps
    if (strcmp(className, "DV2ControlHost") == 0) return true;     // Start Menu (Classic)
    
    return false;
}

bool IsTargetWindow(HWND hWnd) {
    if (!IsWindow(hWnd)) return false;

    // 1. Filter by Style
    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    if ((style & WS_CHILD) || !(style & (WS_CAPTION | WS_THICKFRAME))) {
        return false;
    }

    // 2. Filter by Extended Style
    if (GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        return false;
    }

    // 3. Filter Blacklisted Shell Windows
    if (IsBlacklistedClass(hWnd)) {
        return false;
    }

    // 4. Root Check
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// TIMER CALLBACK
// ---------------------------------------------------------------------------
VOID CALLBACK RestoreAnimTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hWnd, idEvent);
    if (IsWindow(hWnd)) {
        DisableTransitions(hWnd, FALSE); 
    }
}

void ScheduleRestore(HWND hWnd) {
    SetTimer(hWnd, TIMER_ID_RESTORE, RESTORE_ANIM_DELAY_MS, RestoreAnimTimerProc);
}

// ---------------------------------------------------------------------------
// HOOKS
// ---------------------------------------------------------------------------

// 1. Hook SetWindowPos
// Catches changes that bypass ShowWindow (often used by modern apps)
typedef BOOL (WINAPI *SetWindowPos_t)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
SetWindowPos_t SetWindowPos_Original;

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    if (!(uFlags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW))) {
         return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    if (IsIconic(hWnd) || !IsTargetWindow(hWnd)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    // Hide
    if (uFlags & SWP_HIDEWINDOW) {
        if (IsWindowVisible(hWnd)) {
            DisableTransitions(hWnd, TRUE);
            BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
            ScheduleRestore(hWnd);
            return res;
        }
    }

    // Show
    if (uFlags & SWP_SHOWWINDOW) {
        if (!IsWindowVisible(hWnd)) {
            DisableTransitions(hWnd, TRUE);
            BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
            ScheduleRestore(hWnd);
            return res;
        } 
    }
    
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// 2. Hook ShowWindow
// Restored Logic: We MUST disable here to catch the intent before the OS processes it.
typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (!IsTargetWindow(hWnd)) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    // Restore/Minimize -> ENABLE Animations
    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || 
        nCmdShow == SW_RESTORE || nCmdShow == SW_MAXIMIZE) {
        KillTimer(hWnd, TIMER_ID_RESTORE);
        DisableTransitions(hWnd, FALSE); 
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    // Hide -> DISABLE Animations
    if (nCmdShow == SW_HIDE) {
         if (IsWindowVisible(hWnd)) {
            DisableTransitions(hWnd, TRUE);
         }
         BOOL res = ShowWindow_Original(hWnd, nCmdShow);
         ScheduleRestore(hWnd);
         return res;
    }

    // Show -> DISABLE Animations
    if (!IsWindowVisible(hWnd)) {
        DisableTransitions(hWnd, TRUE);
        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        ScheduleRestore(hWnd);
        return res;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

// 3. Hook DestroyWindow
// Restored: Necessary for catching the "Close" animation.
// Safe: Includes checks to ensure we don't crash on zombie windows.
typedef BOOL (WINAPI *DestroyWindow_t)(HWND hWnd);
DestroyWindow_t DestroyWindow_Original;

BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    if (IsWindowVisible(hWnd) && IsTargetWindow(hWnd)) {
        DisableTransitions(hWnd, TRUE);
    }
    return DestroyWindow_Original(hWnd);
}

// 4. Hook AnimateWindow
typedef BOOL (WINAPI *AnimateWindow_t)(HWND hWnd, DWORD dwTime, DWORD dwFlags);
AnimateWindow_t AnimateWindow_Original;

BOOL WINAPI AnimateWindow_Hook(HWND hWnd, DWORD dwTime, DWORD dwFlags) {
    if (IsTargetWindow(hWnd)) {
        if (dwFlags & AW_HIDE) {
            return ShowWindow_Original(hWnd, SW_HIDE);
        }
        return ShowWindow_Original(hWnd, SW_SHOW);
    }
    return AnimateWindow_Original(hWnd, dwTime, dwFlags);
}

// ---------------------------------------------------------------------------
// INITIALIZATION
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)DestroyWindow, (void*)DestroyWindow_Hook, (void**)&DestroyWindow_Original);
    Wh_SetFunctionHook((void*)AnimateWindow, (void*)AnimateWindow_Hook, (void**)&AnimateWindow_Original);
    return TRUE;
}
