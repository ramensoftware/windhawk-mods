// ==WindhawkMod==
// @id              zen-desktop-toggle-icons
// @name            ZenDesktop: Double Click to Hide Icons
// @description     A high-performance, native C++ Windhawk mod that lets you double-click empty desktop space to hide/show desktop icons. Zero lag, zero UI overhead.
// @version         1.2.0
// @author          Lanbo
// @github          https://github.com/Liset999
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZenDesktop: Double Click to Hide Icons

Have you ever wanted a clean, clutter-free desktop but found it annoying to right-click -> View -> Show desktop icons every time? 

**ZenDesktop** brings the ultimate native solution to Windows. It allows you to **double-click empty space on your desktop** to instantly hide or show your icons.

### 🌟 Key Features:
* **Zero UI Overhead**: Embedded natively inside `explorer.exe` process space. No background EXE running, no taskbar/tray icons, 0% CPU and virtually 0MB extra RAM.
* **Process-Native Hit-Testing**: When you double-click, it performs a real-time ListView hit-test. If you double-click a file, folder, or shortcut, the default "open" action is triggered normally. If you double-click empty space, it toggles your desktop icons.
* **Dynamic Hooking**: Automatically subclasses the shell views, meaning even if your Explorer crashes, restarts, or you plug/unplug monitors, the mod remains fully active and stable.
* **Completely Safe**: Uses native Windows `0x7402` (WM_COMMAND) toggle signals, ensuring Windows handles the fade animations and desktop state natively without breaking icon grid arrangements.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <windhawk_utils.h>

// Forward Declarations of Subclass Procedures
LRESULT CALLBACK DesktopListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);
LRESULT CALLBACK DesktopShellViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData);

// Real pointer to CreateWindowExW hook
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t Real_CreateWindowExW;

// Helper to find the current active SHELLDLL_DefView window
HWND FindDesktopShellView() {
    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman) {
        HWND hwndShell = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) return hwndShell;
    }
    
    // Check WorkerW instances (active when using wallpaper engines like Wallpaper Engine or slide shows)
    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL) {
        HWND hwndShell = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) return hwndShell;
    }
    return NULL;
}

// Subclasses active windows in explorer using SetWindowSubclassFromAnyThread
void SubclassExistingWindows() {
    HWND hwndShell = FindDesktopShellView();
    if (hwndShell) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc, 0);
        
        HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (hwndListView) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwndListView, DesktopListViewSubclassProc, 0);
        }
    }
}

// Unsubclasses active windows to allow a safe DLL unload
void UnsubclassWindows() {
    HWND hwndShell = FindDesktopShellView();
    if (hwndShell) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndShell, DesktopShellViewSubclassProc);
        
        HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (hwndListView) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndListView, DesktopListViewSubclassProc);
        }
    }
}

// Hook Window Creator to subclass desktop windows dynamically as they are initialized
HWND WINAPI Hook_CreateWindowExW(
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
    HWND hWnd = Real_CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    if (hWnd) {
        if (lpClassName && !IS_INTRESOURCE(lpClassName)) {
            if (wcscmp(lpClassName, L"SHELLDLL_DefView") == 0) {
                WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopShellViewSubclassProc, 0);
            }
            else if (wcscmp(lpClassName, L"SysListView32") == 0) {
                // Ensure this ListView is indeed the desktop listview
                WCHAR parentClass[256] = {0};
                if (hWndParent && GetClassNameW(hWndParent, parentClass, 256) && wcscmp(parentClass, L"SHELLDLL_DefView") == 0) {
                    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DesktopListViewSubclassProc, 0);
                }
            }
        }
    }

    return hWnd;
}

// Subclass Proc for the Desktop ListView (Handles click empty space detection)
LRESULT CALLBACK DesktopListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        static DWORD lastClickTime = 0;
        static POINT lastClickPt = {0, 0};
        
        bool isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        
        // Manual double-click detection for maximum robustness across custom class style combinations
        if (uMsg == WM_LBUTTONDOWN) {
            DWORD clickTime = GetTickCount();
            DWORD dblClickTime = GetDoubleClickTime();
            POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
            
            if (clickTime - lastClickTime <= dblClickTime &&
                abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2) {
                isDblClick = true;
            } else {
                lastClickTime = clickTime;
                lastClickPt = pt;
            }
        }
        
        if (isDblClick) {
            // Perform native process-local Hit-Test (LVM_HITTEST)
            LVHITTESTINFO htInfo = {0};
            htInfo.pt.x = (short)GET_X_LPARAM(lParam);
            htInfo.pt.y = (short)GET_Y_LPARAM(lParam);
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&htInfo);
            
            // iItem == -1 means double-click landed on completely empty space
            if (htInfo.iItem == -1) {
                HWND hwndParent = GetParent(hWnd);
                if (hwndParent) {
                    // Send toggle desktop icons command (0x7402) natively to SHELLDLL_DefView
                    SendMessageW(hwndParent, WM_COMMAND, 0x7402, 0);
                    return 0; // Handled, prevent default handling of this specific double click
                }
            }
        }
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Subclass Proc for the SHELLDLL_DefView (Handles double-click when icons are already hidden)
LRESULT CALLBACK DesktopShellViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        static DWORD lastClickTime = 0;
        static POINT lastClickPt = {0, 0};
        
        bool isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        
        if (uMsg == WM_LBUTTONDOWN) {
            DWORD clickTime = GetTickCount();
            DWORD dblClickTime = GetDoubleClickTime();
            POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
            
            if (clickTime - lastClickTime <= dblClickTime &&
                abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2) {
                isDblClick = true;
            } else {
                lastClickTime = clickTime;
                lastClickPt = pt;
            }
        }
        
        if (isDblClick) {
            // When icons are already hidden, any double click on SHELLDLL_DefView is a toggle (to restore icons)
            SendMessageW(hWnd, WM_COMMAND, 0x7402, 0);
            return 0; // Handled
        }
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Mod initialization (hook APIs and subclass existing desktop windows)
bool Wh_ModInit() {
    // Register hook for CreateWindowExW using the official Windhawk Wh_SetFunctionHook API
    if (!Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)Hook_CreateWindowExW,
        (void**)&Real_CreateWindowExW
    )) {
        return false;
    }
    
    // Subclass already existing desktop windows (in case of post-boot injection)
    SubclassExistingWindows();
    
    return true;
}

// Mod uninitialization (unsubclass windows and clean up hooks)
void Wh_ModUninit() {
    UnsubclassWindows();
}
