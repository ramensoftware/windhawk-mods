// ==WindhawkMod==
// @id              double-click-toggle-desktop-icons
// @name            Double Click to Toggle Desktop Icons
// @description     Hides or shows desktop icons when double-clicking on an empty space on the desktop.
// @version         1.2
// @author          YAS221D
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
# Double Click to Toggle Desktop Icons

Hides or shows your desktop icons when you double-click on any empty space on the desktop. 

- **Smart Hit Testing:** Double-clicking on actual files, folders, or shortcut icons will still open them normally.
- **Bi-directional:** Works seamlessly both ways—hiding the icons and bringing them back on a double-click.
// ==WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdlib.h>

HHOOK g_hMouseHook = NULL;

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

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MOUSEHOOKSTRUCT* mhs = (MOUSEHOOKSTRUCT*)lParam;
        wchar_t className[256];
        GetClassNameW(mhs->hwnd, className, 256);

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
                    
                    if (index == -1) {
                        SendMessage(hParent, WM_COMMAND, 0x7402, 0);
                        return 1;
                    }
                }
            }
            else if (wcscmp(className, L"WorkerW") == 0 || 
                     wcscmp(className, L"Progman") == 0 || 
                     wcscmp(className, L"SHELLDLL_DefView") == 0) {
                HWND hDefView = FindDesktopDefView();
                if (hDefView) SendMessage(hDefView, WM_COMMAND, 0x7402, 0);
                return 1;
            }
        } 
        else if (wParam == WM_LBUTTONDOWN) {
            if (wcscmp(className, L"WorkerW") == 0 || 
                wcscmp(className, L"Progman") == 0 || 
                wcscmp(className, L"SHELLDLL_DefView") == 0) {
                
                static DWORD lastClickTime = 0;
                static POINT lastClickPt = {0, 0};
                
                DWORD currentTime = GetTickCount();
                POINT currentPt = mhs->pt;
                
                if (currentTime - lastClickTime <= GetDoubleClickTime()) {
                    int cx = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
                    int cy = GetSystemMetrics(SM_CYDOUBLECLK) / 2;
                    
                    if (abs(currentPt.x - lastClickPt.x) <= cx && 
                        abs(currentPt.y - lastClickPt.y) <= cy) {
                        
                        HWND hDefView = FindDesktopDefView();
                        if (hDefView) SendMessage(hDefView, WM_COMMAND, 0x7402, 0);
                        
                        lastClickTime = 0;
                        return 1;
                    }
                }
                
                lastClickTime = currentTime;
                lastClickPt = currentPt;
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Double Click Toggle Desktop Icons v1.2...");
    
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
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
    }
}
