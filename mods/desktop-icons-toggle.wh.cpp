// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Toggle desktop icons visibility with a configurable hotkey (default: Ctrl+Alt+D)
// @version         1.3.1
// @author          Cinabutts
// @github          https://github.com/Cinabutts
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lkernel32 -lshell32 -lcomctl32 --optimize=0 --debug
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
- **Modifier Keys**: Configure which modifier keys (Ctrl, Alt) are required for the hotkey
  - **Ctrl**: Enable/disable Ctrl key requirement
  - **Alt**: Enable/disable Alt key requirement  
- **Hotkey Character**: The character key to use (A-Z, 0-9)

**Note**: At least one modifier key must be selected for security and to prevent accidental activation.

## Compatibility
- Windows 10 (1903+)
- Windows 11 (all versions including 24H2)
- 64-bit architecture
- Requires Windhawk 1.3+
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifierKeys:
  - Ctrl: true
  - Alt: true
  $name: Modifier keys
  $description: >-
    A combination of modifier keys that must be pressed along with the hotkey
    character. At least one modifier must be selected.
- HotkeyChar: D
  $name: Hotkey Character
  $description: The character key to use (A-Z, 0-9)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

// Constants
static const UINT HOTKEY_ID = 0x1001;


// Global state structure
struct ModState {
    HWND hDesktopListView;
    HWND hDesktopWindow;
    HWND hShellViewWindow;
    BOOL bIconsVisible;
    
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
BOOL IsWindowInCurrentProcess(HWND hwnd);
LRESULT CALLBACK CustomShellViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass);
LRESULT CALLBACK CustomListViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass);
LRESULT CALLBACK CustomProgmanWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass);

// Helper function to convert character to uppercase
WCHAR ToUpperCase(WCHAR ch) {
    if (ch >= L'a' && ch <= L'z') {
        return ch - L'a' + L'A';
    }
    return ch;
}

// Helper function to validate and refresh ListView handle if needed
BOOL EnsureValidListView() {
    if (g_state.hDesktopListView && IsWindow(g_state.hDesktopListView)) {
        return TRUE;
    }
    
    Wh_Log(L"Desktop ListView invalid, searching again...");
    g_state.hDesktopListView = FindDesktopListView();
    if (!g_state.hDesktopListView) {
        Wh_Log(L"Failed to find valid desktop ListView");
        return FALSE;
    }
    
    return TRUE;
}

// Utility function to check if hotkey combination matches settings
BOOL IsHotkeyMatch(WPARAM wParam) {
    // Convert both keys to uppercase for consistent comparison
    WCHAR upperKey = ToUpperCase((WCHAR)wParam);
    WCHAR configKey = ToUpperCase(g_state.cHotkeyChar);
    
    // Check if the pressed key matches our configured character
    if (upperKey != configKey) {
        return FALSE;
    }
    
    // Check modifier keys
    SHORT ctrlState = GetAsyncKeyState(VK_CONTROL);
    SHORT altState = GetAsyncKeyState(VK_MENU);
    BOOL ctrlPressed = (ctrlState & 0x8000) != 0;
    BOOL altPressed = (altState & 0x8000) != 0;
    
    // Must match the configured modifier combination exactly
    BOOL modifiersMatch = (ctrlPressed == g_state.bUseCtrl) && (altPressed == g_state.bUseAlt);
    
    if (modifiersMatch) {
        Wh_Log(L"Hotkey match: Key=%lc, Ctrl=%d, Alt=%d", (wchar_t)wParam, ctrlPressed, altPressed);
    }
    
    return modifiersMatch;
}

// Check if a window belongs to the current process
BOOL IsWindowInCurrentProcess(HWND hwnd) {
    if (!hwnd) {
        return FALSE;
    }
    
    DWORD currentProcessId = GetCurrentProcessId();
    DWORD windowProcessId;
    GetWindowThreadProcessId(hwnd, &windowProcessId);
    
    return (currentProcessId == windowProcessId);
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
            if (hShellView && IsWindowInCurrentProcess(hShellView)) {
                Wh_Log(L"Found SHELLDLL_DefView under WorkerW: %p", hShellView);
                break;
            }
            hShellView = nullptr; // Reset if not in current process
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
        // Verify this ListView belongs to the current process
        if (IsWindowInCurrentProcess(hListView)) {
            Wh_Log(L"Found desktop ListView: %p", hListView);
            return hListView;
        } else {
            Wh_Log(L"Found ListView but it belongs to a different process, skipping");
        }
    }
    
    Wh_Log(L"SysListView32 control not found");
    return nullptr;
}

// Toggle desktop icons visibility
void ToggleDesktopIcons() {
    Wh_Log(L"ToggleDesktopIcons called");
    
    if (!EnsureValidListView()) {
        return;
    }
    
    // Check current visibility state to ensure consistency
    BOOL currentlyVisible = IsWindowVisible(g_state.hDesktopListView);
    
    // Toggle the visibility state
    g_state.bIconsVisible = !currentlyVisible;
    
    // Apply the visibility change
    int showCommand = g_state.bIconsVisible ? SW_SHOW : SW_HIDE;
    ShowWindow(g_state.hDesktopListView, showCommand);
    
    // Verify the operation worked by checking actual window visibility
    BOOL actuallyVisible = IsWindowVisible(g_state.hDesktopListView);
    if (actuallyVisible == g_state.bIconsVisible) {
        Wh_Log(L"Successfully changed desktop icons visibility");
    } else {
        Wh_Log(L"Failed to change desktop icons visibility");
        // Revert state on failure
        g_state.bIconsVisible = currentlyVisible;
        return;
    }
    
    // Force desktop refresh
    InvalidateRect(nullptr, nullptr, TRUE);
    UpdateWindow(GetDesktopWindow());
    
    Wh_Log(L"Desktop icons %s", g_state.bIconsVisible ? L"shown" : L"hidden");
}

// Custom window procedure for SHELLDLL_DefView
LRESULT CALLBACK CustomShellViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
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
                WCHAR expectedChar = (WCHAR)(wParam + 64); // Convert control code back to character
                WCHAR upperExpected = ToUpperCase(expectedChar);
                WCHAR upperConfig = ToUpperCase(g_state.cHotkeyChar);
                
                if (upperExpected == upperConfig) {
                    SHORT altState = GetAsyncKeyState(VK_MENU);
                    if ((altState & 0x8000) && g_state.bUseAlt) {
                        Wh_Log(L"Hotkey detected via WM_CHAR in ShellView window");
                        ToggleDesktopIcons();
                        return 0;
                    }
                }
            }
            break;
            
        case WM_NCDESTROY:
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, CustomShellViewWndProc);
            break;
    }
    
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Custom window procedure for ListView
LRESULT CALLBACK CustomListViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
    switch (uMsg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (IsHotkeyMatch(wParam)) {
                Wh_Log(L"Hotkey detected in ListView window");
                ToggleDesktopIcons();
                return 0;
            }
            break;
            
        case WM_NCDESTROY:
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, CustomListViewWndProc);
            break;
    }
    
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Custom window procedure for Program Manager (for global hotkey handling)
LRESULT CALLBACK CustomProgmanWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass) {
    switch (uMsg) {
        case WM_HOTKEY:
            if (wParam == HOTKEY_ID) {
                Wh_Log(L"Global hotkey detected (ID: %d)", (int)wParam);
                ToggleDesktopIcons();
                return 0;
            }
            break;
            
        case WM_NCDESTROY:
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, CustomProgmanWndProc);
            break;
    }
    
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
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
    
    // Try to register the global hotkey
    UINT modifiers = 0;
    if (g_state.bUseCtrl) modifiers |= MOD_CONTROL;
    if (g_state.bUseAlt) modifiers |= MOD_ALT;
    
    if (RegisterHotKey(g_state.hDesktopWindow, HOTKEY_ID, modifiers, g_state.cHotkeyChar)) {
        Wh_Log(L"Successfully registered global hotkey");
        
        // Subclass the Program Manager window to handle WM_HOTKEY messages
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            g_state.hDesktopWindow, 
            CustomProgmanWndProc, 
            0)) {
            Wh_Log(L"Successfully subclassed Program Manager window: %p", g_state.hDesktopWindow);
        } else {
            Wh_Log(L"Failed to subclass Program Manager window");
        }
    } else {
        DWORD error = GetLastError();
        if (error == 1408) { // ERROR_HOTKEY_ALREADY_REGISTERED
            Wh_Log(L"Global hotkey already in use by another application (error: %lu) - using window subclassing only", error);
        } else {
            Wh_Log(L"Failed to register global hotkey (error: %lu), using window subclassing only", error);
        }
    }
    
    // Subclass the SHELLDLL_DefView window for keyboard input
    if (g_state.hShellViewWindow) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            g_state.hShellViewWindow, 
            CustomShellViewWndProc, 
            0)) {
            Wh_Log(L"Successfully subclassed SHELLDLL_DefView window: %p", g_state.hShellViewWindow);
        } else {
            Wh_Log(L"Failed to subclass SHELLDLL_DefView window");
        }
    }
    
    // Also subclass the ListView for additional hotkey detection
    if (g_state.hDesktopListView) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            g_state.hDesktopListView, 
            CustomListViewWndProc, 
            0)) {
            Wh_Log(L"Successfully subclassed ListView window: %p", g_state.hDesktopListView);
        } else {
            Wh_Log(L"Failed to subclass ListView window");
        }
    }
    
    return TRUE;
}

// Cleanup hotkey handling and remove window subclasses
void CleanupHotkeyHandling() {
    // Unregister global hotkey
    if (g_state.hDesktopWindow) {
        UnregisterHotKey(g_state.hDesktopWindow, HOTKEY_ID);
        Wh_Log(L"Unregistered global hotkey");
    }
    
    // Remove window subclasses (the WindhawkUtils system handles cleanup automatically)
    if (g_state.hDesktopWindow) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_state.hDesktopWindow, CustomProgmanWndProc);
        Wh_Log(L"Removed Program Manager window subclass");
        g_state.hDesktopWindow = nullptr;
    }
    
    if (g_state.hShellViewWindow) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_state.hShellViewWindow, CustomShellViewWndProc);
        Wh_Log(L"Removed SHELLDLL_DefView window subclass");
        g_state.hShellViewWindow = nullptr;
    }
    
    if (g_state.hDesktopListView) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_state.hDesktopListView, CustomListViewWndProc);
        Wh_Log(L"Removed ListView window subclass");
    }
}

// Restore icons to visible state
void RestoreIconsToVisible() {
    if (!EnsureValidListView()) {
        Wh_Log(L"Cannot restore icons - desktop ListView not found");
        return;
    }
    
    // Only restore if currently hidden
    if (!IsWindowVisible(g_state.hDesktopListView)) {
        ShowWindow(g_state.hDesktopListView, SW_SHOW);
        
        // Verify the operation worked
        if (IsWindowVisible(g_state.hDesktopListView)) {
            g_state.bIconsVisible = TRUE;
            InvalidateRect(nullptr, nullptr, TRUE);
            UpdateWindow(GetDesktopWindow());
            Wh_Log(L"Desktop icons restored to visible state");
        } else {
            Wh_Log(L"Failed to restore desktop icons visibility");
        }
    } else {
        g_state.bIconsVisible = TRUE;
        Wh_Log(L"Desktop icons were already visible");
    }
}

// Load settings from Windhawk configuration
void LoadSettings() {
    // Load settings from Windhawk using the nested structure like taskbar-button-click
    g_state.bUseCtrl = (BOOL)Wh_GetIntSetting(L"modifierKeys.Ctrl");
    g_state.bUseAlt = (BOOL)Wh_GetIntSetting(L"modifierKeys.Alt");
    PCWSTR hotkeyCharStr = Wh_GetStringSetting(L"HotkeyChar");
    if (hotkeyCharStr && wcslen(hotkeyCharStr) > 0) {
        // Convert to uppercase for consistency
        g_state.cHotkeyChar = ToUpperCase(hotkeyCharStr[0]);
    } else {
        g_state.cHotkeyChar = L'D'; // Default fallback
    }
    if (hotkeyCharStr) {
        Wh_FreeStringSetting(hotkeyCharStr);
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
    if (g_state.hDesktopListView && !IsWindowInCurrentProcess(g_state.hDesktopListView)) {
        Wh_Log(L"Desktop ListView belongs to a different process, ignoring");
        g_state.hDesktopListView = nullptr;
    }
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
    
    Wh_Log(L"Desktop Icons Toggle mod initialized successfully");
    return TRUE;
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"Desktop Icons Toggle mod uninitializing...");
    
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
    // Try to find ListView again if not found initially
    if (!g_state.hDesktopListView) {
        Wh_Log(L"Desktop ListView not found during init, retrying...");
        EnsureValidListView();
    }
    
    // Setup or re-setup hotkey handling if needed
    if (!g_state.hDesktopWindow) {
        SetupHotkeyHandling();
    }
}

// Called before uninitialization
void Wh_ModBeforeUninit() {
    // This function is called before Wh_ModUninit
}

// Called when settings are changed
void Wh_ModSettingsChanged() {
    // Cleanup current hotkey handling
    CleanupHotkeyHandling();
    
    // Reload settings
    LoadSettings();
    
    // Setup hotkey handling with new settings
    SetupHotkeyHandling();
}
