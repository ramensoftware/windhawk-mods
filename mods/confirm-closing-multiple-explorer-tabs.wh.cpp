// ==WindhawkMod==
// @id              confirm-closing-multiple-explorer-tabs
// @name            Confirm Closing Multiple Tabs in File Explorer
// @description     Shows a confirmation dialog when closing a File Explorer window with multiple tabs open
// @version         1.0
// @author          Kitsune
// @github          https://github.com/AromaKitsune
// @include         explorer.exe
// @compilerOptions -lcomctl32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Confirm Closing Multiple Tabs in File Explorer
This mod adds a confirmation dialog that spawns when you attempt to close a File Explorer window
with multiple tabs open, preventing accidental closure of all tabs.

![Preview](https://raw.githubusercontent.com/AromaKitsune/My-Windhawk-Mods/main/screenshots/confirm-closing-multiple-explorer-tabs.png)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>
#include <cstdio>

// Window Class Definitions
#define WC_EXPLORER L"CabinetWClass"
#define WC_TAB      L"ShellTabWindowClass"

// Helper to count tabs
// We define a callback for EnumChildWindows to count instances of ShellTabWindowClass
BOOL CALLBACK CountTabsCallback(HWND hWnd, LPARAM lParam) {
    int* pCount = (int*)lParam;
    wchar_t className[256];
    if (GetClassNameW(hWnd, className, 256)) {
        if (wcscmp(className, WC_TAB) == 0) {
            (*pCount)++;
        }
    }
    return TRUE;
}

int GetTabCount(HWND hExplorer) {
    int count = 0;
    EnumChildWindows(hExplorer, CountTabsCallback, (LPARAM)&count);
    return count;
}

// Subclass procedure to intercept WM_CLOSE
LRESULT CALLBACK ExplorerSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_CLOSE) {
        int tabCount = GetTabCount(hWnd);

        // Only trigger if 2 or more tabs are open
        if (tabCount >= 2) {
            // Play Asterisk sound
            MessageBeep(MB_ICONASTERISK);

            // Prepare the dynamic Main Instruction text
            wchar_t mainInstruction[64];
            swprintf_s(mainInstruction, L"Close %d tabs?", tabCount);

            // Define Custom Buttons
            const TASKDIALOG_BUTTON buttons[] = {
                { IDOK, L"Close Tabs" },
                { IDCANCEL, L"Cancel" }
            };

            TASKDIALOGCONFIG config = { 0 };
            config.cbSize = sizeof(config);
            config.hwndParent = hWnd;
            config.hInstance = NULL;
            config.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW; // Center over File Explorer window
            
            // Configuration requested
            config.pszWindowTitle = L"File Explorer";
            config.pszMainIcon = TD_INFORMATION_ICON; // "i" icon
            config.pszMainInstruction = mainInstruction; // Big blue text
            config.pszContent = NULL; // No content text
            
            // Button Configuration
            config.pButtons = buttons;
            config.cButtons = ARRAYSIZE(buttons);
            config.nDefaultButton = IDCANCEL; // Default to "Cancel" button

            int nButton = 0;
            HRESULT hr = TaskDialogIndirect(&config, &nButton, NULL, NULL);

            if (SUCCEEDED(hr)) {
                if (nButton != IDOK) {
                    // User clicked Cancel (or X on dialog), block the close
                    return 0;
                }
                // If IDOK ("Close Tabs"), we fall through to DefSubclassProc to close the window
            }
        }
    }

    if (uMsg == WM_NCDESTROY) {
        // Remove the subclass when a window is destroyed
        RemoveWindowSubclass(hWnd, ExplorerSubclassProc, uIdSubclass);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Hook logic to attach the subclass to Explorer windows
void AttachSubclass(HWND hWnd) {
    // Check if we can attach. 0 is the subclass ID.
    SetWindowSubclass(hWnd, ExplorerSubclassProc, 0, 0);
}

// Hook logic to detach the subclass
void DetachSubclass(HWND hWnd) {
    RemoveWindowSubclass(hWnd, ExplorerSubclassProc, 0);
}

// Hook CreateWindowExW to catch new Explorer windows
using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_Original;

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
    LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_Original(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );

    if (hWnd && lpClassName && (UINT_PTR)lpClassName > 0xFFFF) {
        if (wcscmp(lpClassName, WC_EXPLORER) == 0) {
            AttachSubclass(hWnd);
        }
    }

    return hWnd;
}

// Enumeration callback for existing windows
BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    wchar_t className[256];
    if (GetClassNameW(hWnd, className, 256)) {
        if (wcscmp(className, WC_EXPLORER) == 0) {
            AttachSubclass(hWnd);
        }
    }
    return TRUE;
}

// Enumeration callback for removing subclass
BOOL CALLBACK EnumWindowsRemoveCallback(HWND hWnd, LPARAM lParam) {
    wchar_t className[256];
    if (GetClassNameW(hWnd, className, 256)) {
        if (wcscmp(className, WC_EXPLORER) == 0) {
            DetachSubclass(hWnd);
        }
    }
    return TRUE;
}

// Mod initialization
BOOL Wh_ModInit() {
    // Hook creation of new windows
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);

    // Attach to any currently open Explorer windows
    EnumWindows(EnumWindowsCallback, 0);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Clean-up: Remove subclass from all windows to prevent dangling pointers (aka crash)
    EnumWindows(EnumWindowsRemoveCallback, 0);
}
