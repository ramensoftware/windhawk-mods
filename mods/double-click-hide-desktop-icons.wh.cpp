// ==WindhawkMod==
// @id				double-click-hide-desktop-icons
// @name			Double Click hide Desktop Icons
// @name:de-DE		Doppelklick Desktop Icons verstecken/einblenden
// @description		Hides or shows the desktop icons by double-clicking on an empty space.
// @description:de-DE	Blendet die Desktop-Icon mit einem Doppelklick auf den Desktop aus.
// @version			1.0.4
// @author			M4RC-XX
// @github			https://github.com/M4RC-XX
// @license			GPL-3.0-or-later
// @include			explorer.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdlib.h>

HHOOK g_hMouseHook = NULL;
HWND g_hwndDefView = NULL;
HWND g_hwndDesktopList = NULL;
HANDLE g_hInitThread = NULL;
HANDLE g_hStopEvent = NULL;

// Explorer shell command to toggle desktop icon visibility
const int TOGGLE_ICONS_COMMAND = 0x7402;

ULONGLONG g_lastClickTime = 0;
POINT g_lastClickPos = {0, 0};

// Traverses parent and child hierarchies to locate SHELLDLL_DefView
HWND GetDefView(HWND clickedHwnd) {
    WCHAR szClass[256];
    HWND curr = clickedHwnd;
    while (curr) {
        GetClassName(curr, szClass, 256);
        if (wcscmp(szClass, L"SHELLDLL_DefView") == 0) {
            return curr;
        }
        HWND child = FindWindowEx(curr, NULL, L"SHELLDLL_DefView", NULL);
        if (child) {
            return child;
        }
        curr = GetParent(curr);
    }
    return g_hwndDefView;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONDBLCLK)) {
        MOUSEHOOKSTRUCT* mhs = (MOUSEHOOKSTRUCT*)lParam;
        bool isDoubleClick = (wParam == WM_LBUTTONDBLCLK);
        
        // Manual double-click detection in case WM_LBUTTONDBLCLK is not dispatched
        if (wParam == WM_LBUTTONDOWN) {
            ULONGLONG currentTime = GetTickCount64();
            int doubleClickTime = GetDoubleClickTime();
            int sysX = GetSystemMetrics(SM_CXDOUBLECLK);
            int sysY = GetSystemMetrics(SM_CYDOUBLECLK);
            
            if ((currentTime - g_lastClickTime) <= (ULONGLONG)doubleClickTime &&
                abs(mhs->pt.x - g_lastClickPos.x) <= (sysX / 2) &&
                abs(mhs->pt.y - g_lastClickPos.y) <= (sysY / 2)) {
                isDoubleClick = true;
            }
            
            g_lastClickTime = currentTime;
            g_lastClickPos = mhs->pt;
        } else {
            g_lastClickTime = 0; 
        }

        if (isDoubleClick) {
            WCHAR szClass[256];
            GetClassName(mhs->hwnd, szClass, 256);
            HWND targetDefView = GetDefView(mhs->hwnd);
            
            // Check if click occurred on an empty area of the desktop list view
            if (wcscmp(szClass, L"SysListView32") == 0) {
                POINT pt = mhs->pt;
                ScreenToClient(mhs->hwnd, &pt);
                LVHITTESTINFO info;
                ZeroMemory(&info, sizeof(info));
                info.pt = pt;
                int index = (int)SendMessage(mhs->hwnd, LVM_HITTEST, 0, (LPARAM)&info);
                
                // index == -1 indicates an empty spot with no item under the cursor
                if (index == -1 && targetDefView) {
                    SendMessage(targetDefView, WM_COMMAND, TOGGLE_ICONS_COMMAND, 0);
                    g_lastClickTime = 0; 
                }
            } 
            // Fallback for background desktop container windows
            else if (wcscmp(szClass, L"SHELLDLL_DefView") == 0 || 
                     wcscmp(szClass, L"WorkerW") == 0 || 
                     wcscmp(szClass, L"Progman") == 0) {
                if (targetDefView) {
                    SendMessage(targetDefView, WM_COMMAND, TOGGLE_ICONS_COMMAND, 0);
                    g_lastClickTime = 0; 
                }
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HWND defView = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
    if (defView) {
        g_hwndDefView = defView;
        HWND listView = FindWindowEx(defView, NULL, L"SysListView32", L"FolderView");
        HWND* ret = (HWND*)lParam;
        *ret = listView ? listView : defView;
        return FALSE;
    }
    return TRUE;
}

// Locates the desktop ListView across Progman or WorkerW hierarchies
HWND FindDesktopListView() {
    HWND hwndTarget = NULL;
    HWND hwndProgman = FindWindow(L"Progman", L"Program Manager");
    if (hwndProgman) {
        HWND defView = FindWindowEx(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
        if (defView) {
            g_hwndDefView = defView;
            HWND listView = FindWindowEx(defView, NULL, L"SysListView32", L"FolderView");
            return listView ? listView : defView;
        }
    }
    EnumWindows(EnumWindowsProc, (LPARAM)&hwndTarget);
    return hwndTarget;
}

DWORD WINAPI InitHookThread(LPVOID lpParam) {
    HWND hwndDesktop = NULL;

    // Poll until Explorer finishes creating desktop windows during startup
    while (WaitForSingleObject(g_hStopEvent, 250) == WAIT_TIMEOUT) {
        hwndDesktop = FindDesktopListView();
        if (hwndDesktop && g_hwndDefView) {
            break;
        }
    }

    if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
        return 0;
    }

    DWORD threadId = GetWindowThreadProcessId(hwndDesktop, NULL);
    if (threadId != 0) {
        g_hwndDesktopList = hwndDesktop;
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, NULL, threadId);
    }

    // Keep thread alive: Win32 unhooks automatically when the installer thread exits
    if (g_hMouseHook) {
        WaitForSingleObject(g_hStopEvent, INFINITE);
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = NULL;
    }

    return 0;
}

BOOL Wh_ModInit() {
    g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) return FALSE;

    // Run initialization in a separate thread to prevent blocking Explorer startup
    g_hInitThread = CreateThread(NULL, 0, InitHookThread, NULL, 0, NULL);
    if (!g_hInitThread) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }
    if (g_hInitThread) {
        WaitForSingleObject(g_hInitThread, 2000);
        CloseHandle(g_hInitThread);
        g_hInitThread = NULL;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }
}