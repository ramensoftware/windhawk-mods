// ==WindhawkMod==
// @id              auto-hide-taskbar-desktop-only
// @name            Hide Taskbar on Desktop Only
// @version         1.0
// @author          qwertyuiop00-art
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
