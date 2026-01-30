// ==WindhawkMod==
// @id              hide-taskbar
// @name            Hide Taskbar Completely
// @description     Hides the Windows taskbar entirely. No auto-hide. It completely disables. Restores it when the mod is disabled.
// @version         1.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         explorer.exe
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
 # Ghost Taskbar (No Pop-up) for Windhawk
 
 **Completely disable the Windows Taskbar, reclaim your screen space, and stop accidental triggers.**
 
 This mod is designed for **Windows "ricers"** and power users who use custom bars (like YASB, DockFinder) or Tiling Window Managers (GlazeWM, Komorebi) and have no need for the native Windows taskbar.
 
 Unlike the default Windows "Auto-Hide" feature, this mod ensures the taskbar **never** pops up, even when your mouse hits the edge of the screen.
 
 ## 🚀 Features
 
 * **True Fullscreen:** Forces the Windows "Work Area" to expand to the full screen resolution (using the Auto-Hide API).
 * **Ghost Mode:** Sets the taskbar opacity to 0 (Invisible) and enables `WS_EX_TRANSPARENT`.
 * **No Mouse Triggers:** Mouse clicks and movements pass strictly *through* the taskbar area to the window behind it. No more accidental pop-ups while gaming or coding.
 * **Resilience:** Includes a timer to fight against Windows Explorer restarts or resolution changes, ensuring the taskbar stays dead.
 * **Multi-Monitor Support:** Detects and hides `Shell_SecondaryTrayWnd` on all connected displays.
 
 ## 🎯 Target Audience
 
 This mod is essential if you are using:
 * **YASB** (Yet Another Status Bar)
 * **Komorebi**
 * **GlazeWM**
 * **DockFinder** / **Nexus Dock**
 * **Rainmeter** custom suites
 
 If you are building a clean, distraction-free setup and want the native taskbar gone for good, this is for you.
 
 ## ⚙️ How it Works
 
 Windows Explorer is stubborn. If we simply "hide" the window, the screen space is still reserved (docked). If we use "Auto-Hide", the space is freed, but the bar pops up on hover.
 
 **Ghost Taskbar combines both:**
 1.  It tells Windows to **Auto-Hide** the taskbar (to reclaim the pixels).
 2.  It applies **Layered Window Attributes** to make the taskbar 100% transparent.
 3.  It adds the `WS_EX_TRANSPARENT` style, making the taskbar "click-through."
 
 ## ⚠️ Restoration
 
 To restore your taskbar:
 1.  Open Windhawk.
 2.  **Disable** the mod.
 3.  The taskbar will reappear and reset to its default docked state automatically.

*/
// ==/WindhawkModReadme==

#include <shellapi.h>

// Global variables
UINT_PTR g_taskbarTimerId = 0;

// Function to set Auto-Hide (Needed for Fullscreen Work Area)
void SetTaskbarState(BOOL enableAutoHide) {
    APPBARDATA abd = {0};
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = FindWindow(L"Shell_TrayWnd", NULL);
    
    if (abd.hWnd) {
        abd.lParam = enableAutoHide ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
        SHAppBarMessage(ABM_SETSTATE, &abd);
    }
}

// Function to make a window "Ghosted" (Transparent + Click-through)
// or restore it to normal.
void SetGhostMode(HWND hwnd, BOOL enableGhost) {
    if (!hwnd) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    if (enableGhost) {
        // Add Layered and Transparent styles
        // WS_EX_LAYERED: Allows transparency
        // WS_EX_TRANSPARENT: Allows mouse clicks to pass through to the window behind
        // WS_EX_TOOLWINDOW: Hides it from Alt-Tab (optional but good)
        if (!(exStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
        }
        
        // Set Opacity to 0 (Invisible)
        // LWA_ALPHA = 2, 0 = Fully transparent
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
    } else {
        // Restore opacity first
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

        // Remove the styles we added
        // We carefully mask out the bits we added to restore original behavior
        exStyle &= ~(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
        
        // Force redraw
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

// Applies Ghost Mode to all taskbar elements
void ApplyGhosting(BOOL enable) {
    // 1. Main Taskbar
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    SetGhostMode(hTaskbar, enable);

    // 2. Secondary Taskbars (Multi-monitor)
    HWND hSecTaskbar = NULL;
    while ((hSecTaskbar = FindWindowEx(NULL, hSecTaskbar, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        SetGhostMode(hSecTaskbar, enable);
    }
    
    // 3. Start Button (If separate)
    HWND hStart = FindWindow(L"Button", L"Start");
    SetGhostMode(hStart, enable);
}

// Timer: Ensures settings persist against Explorer refreshes
VOID CALLBACK TaskbarTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    // Force Auto-Hide to keep Work Area correct
    SetTaskbarState(TRUE);
    
    // Force Ghost Mode to prevent pop-ups
    ApplyGhosting(TRUE);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Mod initialized. Engaging Ghost Mode...");

    // 1. Enable Auto-Hide (To expand screen space)
    SetTaskbarState(TRUE);

    // 2. Make it a Ghost (To hide the pop-up)
    ApplyGhosting(TRUE);

    // 3. Set Timer (2 seconds)
    g_taskbarTimerId = SetTimer(NULL, 0, 2000, TaskbarTimerProc);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Mod uninitializing. Restoring normal taskbar...");

    if (g_taskbarTimerId != 0) {
        KillTimer(NULL, g_taskbarTimerId);
        g_taskbarTimerId = 0;
    }

    // 1. Remove Ghost Mode (Make visible and clickable)
    ApplyGhosting(FALSE);

    // 2. Disable Auto-Hide (Restore docked mode)
    SetTaskbarState(FALSE);
    
    // 3. Force a refresh just in case
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        ShowWindow(hTaskbar, SW_SHOW);
        RedrawWindow(hTaskbar, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}