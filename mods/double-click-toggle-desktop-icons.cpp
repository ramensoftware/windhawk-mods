// ==WindhawkMod==
// @id              double-click-toggle-desktop-icons-fork
// @name            Double Click to Toggle Desktop Icons - Fork
// @description     Hides or shows desktop icons when double-clicking on an empty space on the desktop.
// @version         1.2
// @author          [YAS]
// @github          [YAS221D]
// @include         explorer.exe
// ==/WindhawkMod==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdlib.h>

HHOOK g_hMouseHook = NULL;

// Helper to find the desktop background layer dynamically
HWND FindDesktopDefView() {
    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    HWND hDefView = FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);
    if (!hDefView) {
        HWND hWorkerW = NULL;
        do {
            hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL);
            hDefView = FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
        } while (hWorkerW && !hDefView);
    }
    return hDefView;
}

// Global thread hook to catch all mouse events before Windows processes them
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MOUSEHOOKSTRUCT* mhs = (MOUSEHOOKSTRUCT*)lParam;
        wchar_t className[256];
        GetClassNameW(mhs->hwnd, className, 256);

        // 1. NATIVE DOUBLE CLICK (Usually fires when icons are visible)
        if (wParam == WM_LBUTTONDBLCLK) {
            if (wcscmp(className, L"SysListView32") == 0) {
                HWND hParent = GetParent(mhs->hwnd);
                wchar_t parentClass[256];
                GetClassNameW(hParent, parentClass, 256);
                if (wcscmp(parentClass, L"SHELLDLL_DefView") == 0) {
                    LVHITTESTINFO lvhti = {0};
                    POINT pt = mhs->pt;
                    ScreenToClient(mhs->hwnd, &pt);
                    lvhti.pt = pt;
                    int index = (int)SendMessage(mhs->hwnd, LVM_HITTEST, 0, (LPARAM)&lvhti);
                    
                    if (index == -1) { // -1 means you clicked empty space
                        SendMessage(hParent, WM_COMMAND, 0x7402, 0);
                        return 1; // Block default Windows action
                    }
                }
            }
            // Just in case Windows natively allows a double click on the wallpaper layer
            else if (wcscmp(className, L"WorkerW") == 0 || 
                     wcscmp(className, L"Progman") == 0 || 
                     wcscmp(className, L"SHELLDLL_DefView") == 0) {
                HWND hDefView = FindDesktopDefView();
                if (hDefView) SendMessage(hDefView, WM_COMMAND, 0x7402, 0);
                return 1;
            }
        } 
        
        // 2. MANUAL DOUBLE CLICK (Catches clicks when icons are hidden and Windows stops sending Double Clicks)
        else if (wParam == WM_LBUTTONDOWN) {
            if (wcscmp(className, L"WorkerW") == 0 || 
                wcscmp(className, L"Progman") == 0 || 
                wcscmp(className, L"SHELLDLL_DefView") == 0) {
                
                static DWORD lastClickTime = 0;
                static POINT lastClickPt = {0, 0};
                
                DWORD currentTime = GetTickCount();
                POINT currentPt = mhs->pt;
                
                // If clicked within the Windows double-click time limit...
                if (currentTime - lastClickTime <= GetDoubleClickTime()) {
                    int cx = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
                    int cy = GetSystemMetrics(SM_CYDOUBLECLK) / 2;
                    
                    // ...and the mouse didn't move too far between the two clicks
                    if (abs(currentPt.x - lastClickPt.x) <= cx && 
                        abs(currentPt.y - lastClickPt.y) <= cy) {
                        
                        // We caught a manual double click! Toggle the icons back on.
                        HWND hDefView = FindDesktopDefView();
                        if (hDefView) SendMessage(hDefView, WM_COMMAND, 0x7402, 0);
                        
                        lastClickTime = 0; // Reset to prevent rapid clicking glitches
                        return 1; // Block default Windows action
                    }
                }
                
                // Save click info for the next check
                lastClickTime = currentTime;
                lastClickPt = currentPt;
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Double Click Toggle Desktop Icons v1.2...");
    
    // Find the desktop and hook the specific thread handling it
    HWND hDefView = FindDesktopDefView();
    if (hDefView) {
        DWORD threadId = GetWindowThreadProcessId(hDefView, NULL);
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, NULL, threadId);
        if (g_hMouseHook) {
            Wh_Log(L"Successfully hooked desktop thread.");
            return TRUE;
        }
    }
    Wh_Log(L"Failed to hook desktop thread.");
    return FALSE;
}

void Wh_ModUninit() {
    // Remove the hook cleanly when disabling the mod
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
    }
}
