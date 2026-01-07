// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Toggle desktop icons visibility with a configurable hotkey (default: Ctrl+Alt+D)
// @version         1.3.0
// @author          Cinabutts
// @github          https://github.com/Cinabutts
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lkernel32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icons Toggle

This mod allows you to toggle the visibility of desktop icons using a configurable hotkey.

## Features
- Configurable hotkey support (default: Ctrl+Alt+D)
- Works with Windows 11 24H2 and Windows 10
- Uses efficient window message hooking for reliable hotkey detection
- Automatically restores icons when the mod is disabled
- Clean and robust implementation

## Usage
1. Install and enable the mod
2. Configure your preferred hotkey in the mod settings (optional)
3. Press the configured hotkey to toggle desktop icons visibility
4. Icons will be restored when you toggle again or disable the mod

## Settings
- **Ctrl Key**: Enable/disable Ctrl key requirement for the hotkey
- **Alt Key**: Enable/disable Alt key requirement for the hotkey  
- **Hotkey Character**: The character key to use (A-Z, 0-9)

## Compatibility
- Windows 10 (1903+)
- Windows 11 (all versions including 24H2)
- 64-bit architecture
- Requires Windhawk 1.3+
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- UseCtrl: true
  $name: Use Ctrl key
  $description: Enable or disable Ctrl key requirement for the hotkey - Atleast ONE modifier must be active
- UseAlt: true
  $name: Use Alt key
  $description: Enable or disable Alt key requirement for the hotkey
- HotkeyChar: D
  $name: Hotkey Character
  $description: The character key to use (Capital A-Z, 0-9)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>

// Constants
static const UINT HOTKEY_ID = 0x1001;


// Global state structure
struct ModState {
    HWND hDesktopListView;
    HWND hDesktopWindow;
    HWND hShellViewWindow;
    WNDPROC pOriginalShellViewWndProc;
    WNDPROC pOriginalListViewWndProc;
    BOOL bIconsVisible;
    BOOL bInitialized;
    
    // Settings
    BOOL bUseCtrl;
    BOOL bUseAlt;
    WCHAR cHotkeyChar;
} g_state = {0};

// Forward declarations
HWND FindDesktopListView();
void ToggleDesktopIcons();
BOOL SetupHotkeyHandling();
void CleanupHotkeyHandling();
void RestoreIconsToVisible();
LRESULT CALLBACK CustomShellViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CustomListViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Utility function to check if hotkey combination matches settings
BOOL IsHotkeyMatch(WPARAM wParam) {
    // Check if the pressed key matches our configured character
    if (wParam != g_state.cHotkeyChar && wParam != (g_state.cHotkeyChar + 32)) { // Handle both cases
        return FALSE;
    }
    
    // Check modifier keys
    SHORT ctrlState = GetAsyncKeyState(VK_CONTROL);
    SHORT altState = GetAsyncKeyState(VK_MENU);
    BOOL ctrlPressed = (ctrlState & 0x8000) != 0;
    BOOL altPressed = (altState & 0x8000) != 0;
    
    return (ctrlPressed == g_state.bUseCtrl) && (altPressed == g_state.bUseAlt);
}

// Find the desktop ListView window
HWND FindDesktopListView() {
    Wh_Log(L"Searching for desktop ListView...");
    
    // Find Program Manager window
    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    if (!hProgman) {
        Wh_Log(L"Program Manager window not found");
        return nullptr;
    }
    
    Wh_Log(L"Found Program Manager: %p", hProgman);
    
    // Find SHELLDLL_DefView under Program Manager
    HWND hShellView = FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
    
    // If not found under Progman, try WorkerW windows
    if (!hShellView) {
        Wh_Log(L"SHELLDLL_DefView not found under Progman, searching WorkerW windows...");
        HWND hWorkerW = nullptr;
        while ((hWorkerW = FindWindowExW(nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
            hShellView = FindWindowExW(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (hShellView) {
                Wh_Log(L"Found SHELLDLL_DefView under WorkerW: %p", hShellView);
                break;
            }
        }
    } else {
        Wh_Log(L"Found SHELLDLL_DefView under Progman: %p", hShellView);
    }
    
    if (!hShellView) {
        Wh_Log(L"SHELLDLL_DefView window not found");
        return nullptr;
    }
    
    // Store the SHELLDLL_DefView window for later use
    g_state.hShellViewWindow = hShellView;
    
    // Find the ListView control
    HWND hListView = FindWindowExW(hShellView, nullptr, L"SysListView32", L"FolderView");
    if (hListView) {
        Wh_Log(L"Found desktop ListView: %p", hListView);
        return hListView;
    }
    
    Wh_Log(L"SysListView32 control not found");
    return nullptr;
}

// Toggle desktop icons visibility
void ToggleDesktopIcons() {
    Wh_Log(L"ToggleDesktopIcons called");
    
    if (!g_state.hDesktopListView) {
        g_state.hDesktopListView = FindDesktopListView();
        if (!g_state.hDesktopListView) {
            Wh_Log(L"Failed to find desktop ListView for toggle operation");
            return;
        }
    }
    
    // Toggle the visibility state
    g_state.bIconsVisible = !g_state.bIconsVisible;
    
    // Apply the visibility change
    int showCommand = g_state.bIconsVisible ? SW_SHOW : SW_HIDE;
    ShowWindow(g_state.hDesktopListView, showCommand);
    
    // Force desktop refresh
    InvalidateRect(nullptr, nullptr, TRUE);
    
    Wh_Log(L"Desktop icons %s", g_state.bIconsVisible ? L"shown" : L"hidden");
}

// Custom window procedure for SHELLDLL_DefView
LRESULT CALLBACK CustomShellViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (IsHotkeyMatch(wParam)) {
                Wh_Log(L"Hotkey detected in ShellView window");
                ToggleDesktopIcons();
                return 0;
            }
            break;
            
        case WM_CHAR:
            // Handle Ctrl+character combinations that produce control codes
            if (wParam > 0 && wParam < 32) {
                WCHAR expectedChar = wParam + 64; // Convert control code back to character
                if (expectedChar == g_state.cHotkeyChar) {
                    SHORT altState = GetAsyncKeyState(VK_MENU);
                    if ((altState & 0x8000) && g_state.bUseAlt) {
                        Wh_Log(L"Hotkey detected via WM_CHAR in ShellView window");
                        ToggleDesktopIcons();
                        return 0;
                    }
                }
            }
            break;
    }
    
    return CallWindowProcW(g_state.pOriginalShellViewWndProc, hwnd, uMsg, wParam, lParam);
}

// Custom window procedure for ListView (fallback)
LRESULT CALLBACK CustomListViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (IsHotkeyMatch(wParam)) {
                Wh_Log(L"Hotkey detected in ListView window");
                ToggleDesktopIcons();
                return 0;
            }
            break;
    }
    
    return CallWindowProcW(g_state.pOriginalListViewWndProc, hwnd, uMsg, wParam, lParam);
}

// Setup hotkey handling by subclassing windows
BOOL SetupHotkeyHandling() {
    // Find Program Manager window for hotkey registration
    g_state.hDesktopWindow = FindWindowW(L"Progman", L"Program Manager");
    if (!g_state.hDesktopWindow) {
        Wh_Log(L"Could not find Program Manager window for hotkey setup");
        return FALSE;
    }
    
    Wh_Log(L"Found Program Manager window: %p", g_state.hDesktopWindow);
    
    // Try to register the global hotkey (optional, as we also use window subclassing)
    UINT modifiers = 0;
    if (g_state.bUseCtrl) modifiers |= MOD_CONTROL;
    if (g_state.bUseAlt) modifiers |= MOD_ALT;
    
    if (RegisterHotKey(g_state.hDesktopWindow, HOTKEY_ID, modifiers, g_state.cHotkeyChar)) {
        Wh_Log(L"Successfully registered global hotkey");
    } else {
        DWORD error = GetLastError();
        Wh_Log(L"Failed to register global hotkey (error: %lu), using window subclassing only", error);
    }
    
    // Subclass the SHELLDLL_DefView window for keyboard input
    if (g_state.hShellViewWindow) {
        g_state.pOriginalShellViewWndProc = (WNDPROC)SetWindowLongPtrW(
            g_state.hShellViewWindow, 
            GWLP_WNDPROC, 
            (LONG_PTR)CustomShellViewWndProc
        );
        
        if (g_state.pOriginalShellViewWndProc) {
            Wh_Log(L"Successfully subclassed SHELLDLL_DefView window: %p", g_state.hShellViewWindow);
        } else {
            Wh_Log(L"Failed to subclass SHELLDLL_DefView window");
        }
    }
    
    // Also subclass the ListView as a fallback
    if (g_state.hDesktopListView) {
        g_state.pOriginalListViewWndProc = (WNDPROC)SetWindowLongPtrW(
            g_state.hDesktopListView, 
            GWLP_WNDPROC, 
            (LONG_PTR)CustomListViewWndProc
        );
        
        if (g_state.pOriginalListViewWndProc) {
            Wh_Log(L"Successfully subclassed ListView window: %p", g_state.hDesktopListView);
        } else {
            Wh_Log(L"Failed to subclass ListView window");
        }
    }
    
    return TRUE;
}

// Cleanup hotkey handling and restore original window procedures
void CleanupHotkeyHandling() {
    // Unregister global hotkey
    if (g_state.hDesktopWindow) {
        UnregisterHotKey(g_state.hDesktopWindow, HOTKEY_ID);
        Wh_Log(L"Unregistered global hotkey");
        g_state.hDesktopWindow = nullptr;
    }
    
    // Restore SHELLDLL_DefView window procedure
    if (g_state.hShellViewWindow && g_state.pOriginalShellViewWndProc) {
        SetWindowLongPtrW(g_state.hShellViewWindow, GWLP_WNDPROC, (LONG_PTR)g_state.pOriginalShellViewWndProc);
        Wh_Log(L"Restored SHELLDLL_DefView window procedure");
        g_state.pOriginalShellViewWndProc = nullptr;
    }
    
    // Restore ListView window procedure
    if (g_state.hDesktopListView && g_state.pOriginalListViewWndProc) {
        SetWindowLongPtrW(g_state.hDesktopListView, GWLP_WNDPROC, (LONG_PTR)g_state.pOriginalListViewWndProc);
        Wh_Log(L"Restored ListView window procedure");
        g_state.pOriginalListViewWndProc = nullptr;
    }
}

// Restore icons to visible state
void RestoreIconsToVisible() {
    if (!g_state.bIconsVisible && g_state.hDesktopListView) {
        ShowWindow(g_state.hDesktopListView, SW_SHOW);
        InvalidateRect(nullptr, nullptr, TRUE);
        g_state.bIconsVisible = TRUE;
        Wh_Log(L"Desktop icons restored to visible state");
    }
}

// Load settings from Windhawk configuration
void LoadSettings() {
    // Load settings from Windhawk
    g_state.bUseCtrl = (BOOL)Wh_GetIntSetting(L"UseCtrl", TRUE);
    g_state.bUseAlt = (BOOL)Wh_GetIntSetting(L"UseAlt", TRUE);
    PCWSTR hotkeyCharStr = Wh_GetStringSetting(L"HotkeyChar");
    if (hotkeyCharStr && wcslen(hotkeyCharStr) > 0) {
        g_state.cHotkeyChar = hotkeyCharStr[0];
    } else {
        g_state.cHotkeyChar = L'D'; // Default fallback
    }
    
    // Ensure we have at least one modifier key
    if (!g_state.bUseCtrl && !g_state.bUseAlt) {
        Wh_Log(L"Warning: No modifier keys selected, defaulting to Ctrl+Alt");
        g_state.bUseCtrl = TRUE;
        g_state.bUseAlt = TRUE;
    }
    
    Wh_Log(L"Settings loaded - Ctrl: %d, Alt: %d, Key: %c", 
           g_state.bUseCtrl, g_state.bUseAlt, g_state.cHotkeyChar);
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Desktop Icons Toggle mod initializing...");
    
    // Initialize state
    ZeroMemory(&g_state, sizeof(g_state));
    
    // Load settings
    LoadSettings();
    
    // Find desktop ListView
    g_state.hDesktopListView = FindDesktopListView();
    if (g_state.hDesktopListView) {
        g_state.bIconsVisible = IsWindowVisible(g_state.hDesktopListView);
        Wh_Log(L"Initial icons state: %s", g_state.bIconsVisible ? L"visible" : L"hidden");
    } else {
        Wh_Log(L"Could not find desktop ListView during initialization");
    }
    
    // Setup hotkey handling
    if (!SetupHotkeyHandling()) {
        Wh_Log(L"Failed to setup hotkey handling, but continuing...");
    }
    
    g_state.bInitialized = TRUE;
    Wh_Log(L"Desktop Icons Toggle mod initialized successfully");
    return TRUE;
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"Desktop Icons Toggle mod uninitializing...");
    
    if (!g_state.bInitialized) {
        return;
    }
    
    // Restore icons before cleanup
    RestoreIconsToVisible();
    
    // Cleanup hotkey handling
    CleanupHotkeyHandling();
    
    // Clear state
    ZeroMemory(&g_state, sizeof(g_state));
    
    Wh_Log(L"Desktop Icons Toggle mod uninitialized");
}

// Called after initialization is complete
void Wh_ModAfterInit() {
    Wh_Log(L"Desktop Icons Toggle mod after init");
    
    // Try to find ListView again if not found initially
    if (!g_state.hDesktopListView) {
        Sleep(2000); // Give explorer more time to fully load
        g_state.hDesktopListView = FindDesktopListView();
        if (g_state.hDesktopListView) {
            g_state.bIconsVisible = IsWindowVisible(g_state.hDesktopListView);
            Wh_Log(L"ListView found in after init: %p (visible: %d)", 
                   g_state.hDesktopListView, g_state.bIconsVisible);
            
            // Setup hotkey handling if it wasn't done before
            if (!g_state.hDesktopWindow) {
                SetupHotkeyHandling();
            }
        } else {
            Wh_Log(L"Still could not find ListView in after init");
        }
    }
}

// Called before uninitialization
void Wh_ModBeforeUninit() {
    Wh_Log(L"Desktop Icons Toggle mod before uninit");
    RestoreIconsToVisible();
}

// Called when settings are changed
void Wh_ModSettingsChanged() {
    Wh_Log(L"Desktop Icons Toggle mod settings changed");
    
    // Cleanup current hotkey handling
    CleanupHotkeyHandling();
    
    // Reload settings
    LoadSettings();
    
    // Setup hotkey handling with new settings
    SetupHotkeyHandling();
}
