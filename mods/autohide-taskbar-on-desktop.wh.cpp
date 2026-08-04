// ==WindhawkMod==
// @id              autohide-taskbar-on-desktop
// @name            autohide-taskbar-on-desktop
// @version         1.0.0
// @author          qwertyuiop00-art
// @github          https://github.com/qwertyuiop00-art
// @description     Hides taskbar on desktop, shows when an app is active.
// @architecture    x86-64
// @include         explorer.exe
// ==/WindhawkMod==

#include <windows.h>

void SetTaskbarAutoHide(bool enable) {
    APPBARDATA abd = { sizeof(APPBARDATA) };
    abd.hWnd = FindWindow(L"Shell_TrayWnd", NULL);
    abd.lParam = enable ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}

// Foreground Window change hook to detect active apps vs desktop
HWINEVENTHOOK g_hook = NULL;

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, 
                            LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND && hwnd) {
        WCHAR className[256];
        GetClassName(hwnd, className, 256);
        
        // Check if current active window is Desktop (Progman or WorkerW)
        if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0) {
            SetTaskbarAutoHide(true);  // Hide on Desktop
        } else {
            SetTaskbarAutoHide(false); // Show on App Open
        }
    }
}

BOOL Wh_ModInit() {
    // Hook event for active window change
    g_hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, 
                             NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hook) UnhookWinEvent(g_hook);
    SetTaskbarAutoHide(false); // Restore default
}

// ==WindhawkModReadme==
/*
# Hide Taskbar on Desktop Only

This mod automatically hides the Windows taskbar when you are viewing the desktop and restores it when applications are active/focused.

## Features
- Keeps desktop clean by auto-hiding taskbar.
- Restores taskbar instantly when switching to open applications.
*/
// ==/WindhawkModReadme==
