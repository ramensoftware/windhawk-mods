// ==WindhawkMod==
// @id              confirm-closing-multiple-explorer-tabs
// @name            Confirm Closing Multiple Tabs in File Explorer
// @description     Shows a confirmation dialog when closing a File Explorer window with multiple tabs open
// @version         1.0.1
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

## Configuration
**Default button**: Choose whether "Close Tabs" or "Cancel" is the default button in the confirmation dialog.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultButton: cancel
  $name: Default button
  $description: The button that is selected by default in the confirmation dialog
  $options:
    - closeTabs: Close Tabs
    - cancel: Cancel
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <cstdio>

// Window class definitions
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
LRESULT CALLBACK ExplorerSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_CLOSE) {
        int tabCount = GetTabCount(hWnd);

        // Only trigger if 2 or more tabs are open
        if (tabCount >= 2) {
            // Prepare the dynamic Main Instruction text
            wchar_t mainInstruction[64];
            swprintf_s(mainInstruction, L"Close %d tabs?", tabCount);

            // Define custom buttons
            const TASKDIALOG_BUTTON buttons[] = {
                { IDOK, L"Close Tabs" },
                { IDCANCEL, L"Cancel" }
            };

            // Determine default button from settings
            int nDefaultButtonId = IDCANCEL; // Default button fallback
            PCWSTR defaultButtonSetting = Wh_GetStringSetting(L"defaultButton");

            if (defaultButtonSetting) {
                if (wcscmp(defaultButtonSetting, L"closeTabs") == 0) {
                    nDefaultButtonId = IDOK;
                }
                Wh_FreeStringSetting(defaultButtonSetting);
            }

            // Dialog configuration
            TASKDIALOGCONFIG config = { 0 };
            config.cbSize = sizeof(config);
            config.hwndParent = hWnd;
            config.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW; // Center over the File Explorer window
            config.pszWindowTitle = L"File Explorer";         // Title
            config.pszMainIcon = TD_INFORMATION_ICON;         // Icon
            config.pszMainInstruction = mainInstruction;      // Main instruction (big blue text)

            // Button configuration
            config.pButtons = buttons;
            config.cButtons = ARRAYSIZE(buttons);
            config.nDefaultButton = nDefaultButtonId; // Default button based on setting

            int nButton = 0;
            HRESULT hr = TaskDialogIndirect(&config, &nButton, NULL, NULL);

            if (SUCCEEDED(hr)) {
                if (nButton != IDOK) {
                    // User clicked "Cancel" or "X", then don't close the File Explorer window
                    return 0;
                }
                // User clicked "Close Tabs", then fall through to DefSubclassProc to close the File Explorer window
            }
        }
    }

    if (uMsg == WM_NCDESTROY) {
        // Remove the subclass when the File Explorer window is destroyed
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ExplorerSubclassProc);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Hook logic to attach the subclass to File Explorer windows
void AttachSubclass(HWND hWnd) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ExplorerSubclassProc, 0);
}

// Hook logic to detach the subclass
void DetachSubclass(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ExplorerSubclassProc);
}

// Hook CreateWindowExW to catch new File Explorer windows
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

// Enumeration callback for existing File Explorer windows
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
    Wh_Log(L"Init");

    // Hook creation of new File Explorer windows
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);

    // Attach to any currently open File Explorer windows
    EnumWindows(EnumWindowsCallback, 0);

    return TRUE;
}

// Mod uninitialization
void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Clean-up: Remove subclass from all File Explorer windows to prevent dangling pointers (aka crash)
    EnumWindows(EnumWindowsRemoveCallback, 0);
}
