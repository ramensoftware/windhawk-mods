// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Toggle desktop icons visibility with a configurable hotkey (default: Ctrl+Alt+D)
// @version         1.3.4
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

// Global state structure
struct ModState {
    HWND hDesktopListView;
    HWND hShellViewWindow;
    BOOL bIconsVisible;
    
    // Settings
    BOOL bUseCtrl;
    BOOL bUseAlt;
    WCHAR cHotkeyChar;
} g_state = {0};

// Original function pointers
using TranslateMessage_t = BOOL(WINAPI*)(const MSG*);
TranslateMessage_t TranslateMessage_Original;

// Forward declarations
HWND FindDesktopListView();
void ToggleDesktopIcons();
void RestoreIconsToVisible();
BOOL IsWindowInCurrentProcess(HWND hwnd);

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
    if (hShellView && !IsWindowInCurrentProcess(hShellView)) {
        Wh_Log(L"SHELLDLL_DefView under Progman not in current process, ignoring");
        hShellView = nullptr;
    }
    
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
    
    // Force a targeted desktop refresh
    if (g_state.hShellViewWindow && IsWindow(g_state.hShellViewWindow)) {
        RedrawWindow(
            g_state.hShellViewWindow,
            nullptr,
            nullptr,
            RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    } else {
        InvalidateRect(GetDesktopWindow(), nullptr, TRUE);
        UpdateWindow(GetDesktopWindow());
    }
    
    Wh_Log(L"Desktop icons %s", g_state.bIconsVisible ? L"shown" : L"hidden");
}

// Hook for TranslateMessage to intercept keyboard messages
BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (lpMsg && (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN)) {
        if (IsHotkeyMatch(lpMsg->wParam)) {
            Wh_Log(L"Hotkey detected - toggling desktop icons");
            ToggleDesktopIcons();
            // Return TRUE to skip translating this message
            return TRUE;
        }
    }
    
    return TranslateMessage_Original(lpMsg);
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
            if (g_state.hShellViewWindow && IsWindow(g_state.hShellViewWindow)) {
                RedrawWindow(
                    g_state.hShellViewWindow,
                    nullptr,
                    nullptr,
                    RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
            } else {
                InvalidateRect(GetDesktopWindow(), nullptr, TRUE);
                UpdateWindow(GetDesktopWindow());
            }
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
        Wh_Log(L"Desktop ListView not found during initialization (will retry after init)");
    }
    
    // Hook TranslateMessage to intercept keyboard input
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"Failed to get handle for user32.dll");
        return FALSE;
    }
    
    void* pTranslateMessage = (void*)GetProcAddress(hUser32, "TranslateMessage");
    if (!pTranslateMessage) {
        Wh_Log(L"Failed to get TranslateMessage address");
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook(pTranslateMessage, (void*)TranslateMessage_Hook, (void**)&TranslateMessage_Original)) {
        Wh_Log(L"Failed to hook TranslateMessage");
        return FALSE;
    }
    
    Wh_Log(L"Hooked TranslateMessage for hotkey detection");
    Wh_Log(L"Desktop Icons Toggle mod initialized successfully");
    return TRUE;
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"Desktop Icons Toggle mod uninitializing...");
    
    // Restore icons before cleanup
    RestoreIconsToVisible();
    
    // Clear state
    ZeroMemory(&g_state, sizeof(g_state));
    
    Wh_Log(L"Desktop Icons Toggle mod uninitialized");
}

// Called after initialization is complete
void Wh_ModAfterInit() {
    // Try to find ListView again if not found initially
    if (!g_state.hDesktopListView) {
        Wh_Log(L"Retrying desktop ListView discovery after initialization...");
        if (EnsureValidListView()) {
            Wh_Log(L"Successfully found desktop ListView on retry");
        } else {
            Wh_Log(L"Desktop ListView still not found (will retry on first toggle attempt)");
        }
    }
}

// Called when settings are changed
void Wh_ModSettingsChanged() {
    // Reload settings
    LoadSettings();

    // Refresh handles after settings change
    g_state.hDesktopListView = FindDesktopListView();
    if (g_state.hDesktopListView && !IsWindowInCurrentProcess(g_state.hDesktopListView)) {
        Wh_Log(L"Desktop ListView belongs to a different process after settings change, ignoring");
        g_state.hDesktopListView = nullptr;
    }
    if (g_state.hDesktopListView) {
        g_state.bIconsVisible = IsWindowVisible(g_state.hDesktopListView);
    }
}
