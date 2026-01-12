// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Toggle desktop icons visibility with a configurable hotkey (default: Ctrl+Alt+D)
// @version         1.3.5
// @author          Cinabutts
// @github          https://github.com/Cinabutts
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lkernel32 -lshell32 -lcomctl32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icons Toggle

This mod allows you to toggle the visibility of desktop icons using a configurable hotkey.

`Prerequisite: Must always click the Taskbar or Desktop to allow hotkey to become Live.`

In other words Click Taskbar/Desktop THEN configured hotkey to toggle desktop icons.

------------------------------------------

## Features
- **Persistent State**: Icons stay hidden/shown even after restarting Windows (Uses Registry).
- **Instant Toggle**: Uses direct window manipulation for immediate feedback.
- Configurable hotkey support (default: Ctrl+Alt+D).

## Settings
- **Modifier Keys**: Configure which modifier keys (Ctrl, Alt) are required for the hotkey.
- **Hotkey Character**: The character key to use (A-Z, 0-9).
- **Hide icons on startup**: Automatically hide desktop icons when the mod loads or Explorer restarts.

## Compatibility
- Tested on Windows 11 25H2 (26220.7523) | 64-bit architecture
- SHOULD work on Windows 10 and above(untested)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- HideOnStartup: true
  $name: Hide icons on startup
  $description: >-
    Automatically hide desktop icons when the mod loads or Explorer restarts. (The mod itself/this becomes the switch - Does not effect ↓ below)

     | Keep on for: On restart(Explorer or PC) icons are Hidden.
- modifierKeys:
  - Ctrl: true
  - Alt: true
  $name: Modifier keys
  $description: >-
    A combination of modifier keys that must be pressed along with the hotkey character.
    
     | At least one modifier must be selected. (Default: Ctrl+Alt)
- HotkeyChar: D
  $name: Hotkey Character
  $description: The character key to use (A-Z, 0-9)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>


// Registry paths for persistence
constexpr wchar_t kDesktopRegPath[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr wchar_t kHideIconsValueName[] = L"HideIcons";

// Global state structure
struct ModState {
    HWND hDesktopListView;
    HWND hShellViewWindow;
    BOOL bIconsVisible;
    BOOL bShouldHandleHotkey;
    
    // Settings
    BOOL bUseCtrl;
    BOOL bUseAlt;
    WCHAR cHotkeyChar;
    BOOL bHideOnStartup; 
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
    
    Wh_Log(L"[WARNING] Desktop ListView invalid, searching again...");
    g_state.hDesktopListView = FindDesktopListView();
    if (!g_state.hDesktopListView) {
        Wh_Log(L"[ERROR] Failed to find valid desktop ListView");
        return FALSE;
    }
    
    return TRUE;
}

// Utility function to check if hotkey combination matches settings
BOOL IsHotkeyMatch(WPARAM wParam) {
    if (!g_state.bShouldHandleHotkey) {
        return FALSE;
    }
    
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
        Wh_Log(L"[HOOK] Hotkey match: Key=%lc, Ctrl=%d, Alt=%d", (wchar_t)wParam, ctrlPressed, altPressed);
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
    Wh_Log(L"[INIT] Searching for desktop ListView...");
    
    // Find Program Manager window
    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    if (!hProgman) {
        Wh_Log(L"[ERROR] Program Manager window not found");
        return nullptr;
    }
    
    Wh_Log(L"[INIT] Found Program Manager: %p", hProgman);
    
    // Find SHELLDLL_DefView under Program Manager
    HWND hShellView = FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
    
    // Note: Removed IsWindowInCurrentProcess check here to allow Taskbar process handle access.
    
    // If not found under Progman, try WorkerW windows
    if (!hShellView) {
        Wh_Log(L"[INIT] SHELLDLL_DefView not found under Progman, searching WorkerW windows...");
        HWND hWorkerW = nullptr;
        while ((hWorkerW = FindWindowExW(nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
            hShellView = FindWindowExW(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (hShellView) {
                Wh_Log(L"[INIT] Found SHELLDLL_DefView under WorkerW: %p", hShellView);
                break;
            }
            hShellView = nullptr;
        }
    } else {
        Wh_Log(L"[INIT] Found SHELLDLL_DefView under Progman: %p", hShellView);
    }
    
    if (!hShellView) {
        Wh_Log(L"[ERROR] SHELLDLL_DefView window not found");
        return nullptr;
    }
    
    // Store the SHELLDLL_DefView window for later use
    g_state.hShellViewWindow = hShellView;
    
    // Find the ListView control
    HWND hListView = FindWindowExW(hShellView, nullptr, L"SysListView32", L"FolderView");
    if (hListView) {
        Wh_Log(L"[INIT] Found desktop ListView: %p", hListView);
        return hListView;
    }
    
    Wh_Log(L"[ERROR] SysListView32 control not found");
    return nullptr;
}

// Update Registry Setting (Persistent)
BOOL UpdateRegistryState(BOOL hide) {
    HKEY key = nullptr;
    LONG status = RegOpenKeyExW(HKEY_CURRENT_USER, kDesktopRegPath, 0, KEY_SET_VALUE, &key);
    if (status != ERROR_SUCCESS) return FALSE;

    // Registry: 1 = Hide, 0 = Show
    DWORD newValue = hide ? 1 : 0;
    status = RegSetValueExW(key, kHideIconsValueName, 0, REG_DWORD, 
                           reinterpret_cast<const BYTE*>(&newValue), sizeof(newValue));
    RegCloseKey(key);
    
    // Notify Explorer
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                        reinterpret_cast<LPARAM>(kDesktopRegPath),
                        SMTO_ABORTIFHUNG, 200, nullptr);
    return (status == ERROR_SUCCESS);
}

// Toggle desktop icons visibility
void ToggleDesktopIcons() {
    Wh_Log(L"[TOGGLE] ToggleDesktopIcons called");
    
    if (!EnsureValidListView()) {
        return;
    }
    
    // Check current visibility state to ensure consistency
    BOOL currentlyVisible = IsWindowVisible(g_state.hDesktopListView);
    
    // Toggle the visibility state
    g_state.bIconsVisible = !currentlyVisible;
    
    // Update Registry for Persistence
    UpdateRegistryState(!g_state.bIconsVisible);

    // Apply the visibility change to window (Instant)
    int showCommand = g_state.bIconsVisible ? SW_SHOW : SW_HIDE;
    ShowWindow(g_state.hDesktopListView, showCommand);
    
    // Verify the operation worked by checking actual window visibility
    BOOL actuallyVisible = IsWindowVisible(g_state.hDesktopListView);
    if (actuallyVisible == g_state.bIconsVisible) {
        Wh_Log(L"[TOGGLE] Successfully changed desktop icons visibility");
    } else {
        Wh_Log(L"[ERROR] Failed to change desktop icons visibility");
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
    
    Wh_Log(L"[TOGGLE] Desktop icons %s (Reg updated)", g_state.bIconsVisible ? L"shown" : L"hidden");
}

// Hook for TranslateMessage to intercept keyboard messages
BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (lpMsg && (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN)) {
        if (IsHotkeyMatch(lpMsg->wParam)) {
            Wh_Log(L"[HOOK] Hotkey detected - toggling desktop icons");
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
        Wh_Log(L"[ERROR] Cannot restore icons - desktop ListView not found");
        return;
    }
    
    // Ensure Registry matches "Visible" state (HideIcons = 0)
    UpdateRegistryState(FALSE);

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
            Wh_Log(L"[TOGGLE] Desktop icons restored to visible state");
        } else {
            Wh_Log(L"[ERROR] Failed to restore desktop icons visibility");
        }
    } else {
        g_state.bIconsVisible = TRUE;
        Wh_Log(L"[TOGGLE] Desktop icons were already visible");
    }
}

// Load settings from Windhawk configuration
void LoadSettings() {
    g_state.bUseCtrl = (BOOL)Wh_GetIntSetting(L"modifierKeys.Ctrl");
    g_state.bUseAlt = (BOOL)Wh_GetIntSetting(L"modifierKeys.Alt");
    g_state.bHideOnStartup = (BOOL)Wh_GetIntSetting(L"HideOnStartup");
    
    PCWSTR hotkeyCharStr = Wh_GetStringSetting(L"HotkeyChar");
    if (hotkeyCharStr && wcslen(hotkeyCharStr) > 0) {
        g_state.cHotkeyChar = ToUpperCase(hotkeyCharStr[0]);
    } else {
        g_state.cHotkeyChar = L'D';
    }
    if (hotkeyCharStr) {
        Wh_FreeStringSetting(hotkeyCharStr);
    }
    
    // Ensure we have at least one modifier key
    if (!g_state.bUseCtrl && !g_state.bUseAlt) {
        Wh_Log(L"[WARNING] No modifier keys selected, defaulting to Ctrl+Alt");
        g_state.bUseCtrl = TRUE;
        g_state.bUseAlt = TRUE;
    }
    
    Wh_Log(L"[SETTINGS] Settings loaded - Ctrl: %d, Alt: %d, Key: %c, HideOnStart: %d", 
           g_state.bUseCtrl, g_state.bUseAlt, g_state.cHotkeyChar, g_state.bHideOnStartup);
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"[INIT] Desktop Icons Toggle mod initializing...");
    
    // Initialize state
    ZeroMemory(&g_state, sizeof(g_state));
    
    // Load settings
    LoadSettings();
    
    // Find desktop ListView
    g_state.hDesktopListView = FindDesktopListView();
    
    // Set hotkey active if handle found
    if (g_state.hDesktopListView) {
        g_state.bShouldHandleHotkey = TRUE;
        g_state.bIconsVisible = IsWindowVisible(g_state.hDesktopListView);
        Wh_Log(L"[INIT] Desktop handle found - will handle hotkeys");
    } else {
        g_state.bShouldHandleHotkey = FALSE;
        Wh_Log(L"[INIT] Desktop handle not found - will NOT handle hotkeys");
    }
    
    // Hook TranslateMessage to intercept keyboard input
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"[CRITICAL] Failed to get handle for user32.dll");
        return FALSE;
    }
    
    void* pTranslateMessage = (void*)GetProcAddress(hUser32, "TranslateMessage");
    if (!pTranslateMessage) {
        Wh_Log(L"[CRITICAL] Failed to get TranslateMessage address");
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook(pTranslateMessage, (void*)TranslateMessage_Hook, (void**)&TranslateMessage_Original)) {
        Wh_Log(L"[CRITICAL] Failed to hook TranslateMessage");
        return FALSE;
    }
    
    Wh_Log(L"[HOOK] TranslateMessage hooked for hotkey detection");
    Wh_Log(L"[INIT] Desktop Icons Toggle mod initialized successfully");
    return TRUE;
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"[INIT] Desktop Icons Toggle mod uninitializing...");
    
    // Restore icons before cleanup (only if we have the handle)
    if (g_state.bShouldHandleHotkey) {
        RestoreIconsToVisible();
    }
    
    // Clear state
    ZeroMemory(&g_state, sizeof(g_state));
    
    Wh_Log(L"[INIT] Desktop Icons Toggle mod uninitialized");
}

// Called after initialization is complete
void Wh_ModAfterInit() {
    // Try to find ListView again if not found initially
    if (!g_state.hDesktopListView) {
        Wh_Log(L"[INIT] Retrying desktop ListView discovery after initialization...");
        g_state.hDesktopListView = FindDesktopListView();
        if (g_state.hDesktopListView) {
            g_state.bShouldHandleHotkey = TRUE;
            g_state.bIconsVisible = IsWindowVisible(g_state.hDesktopListView);
            Wh_Log(L"[INIT] Successfully found desktop ListView on retry - now handling hotkeys");
        }
    }

    // Hide on Startup
    // Only perform this check if we own the window to avoid race conditions.
    if (g_state.bHideOnStartup && g_state.bShouldHandleHotkey && IsWindowInCurrentProcess(g_state.hDesktopListView)) {
        if (IsWindowVisible(g_state.hDesktopListView)) {
            Wh_Log(L"[INIT] 'Hide on Startup' active. Hiding icons...");
            ToggleDesktopIcons();
        }
    }
}

// Called when settings are changed
void Wh_ModSettingsChanged() {
    // Reload settings
    LoadSettings();

    // Refresh handles logic
    if (!g_state.hDesktopListView || !IsWindow(g_state.hDesktopListView)) {
        g_state.hDesktopListView = FindDesktopListView();
        if (g_state.hDesktopListView) {
            g_state.bShouldHandleHotkey = TRUE;
            g_state.bIconsVisible = IsWindowVisible(g_state.hDesktopListView);
        } else {
            g_state.bShouldHandleHotkey = FALSE;
        }
    }

    // Handle immediate state toggle based on setting change
    // Only the process that owns the desktop should run this check to avoid double-toggling.
    if (g_state.bShouldHandleHotkey && IsWindowInCurrentProcess(g_state.hDesktopListView)) {
        BOOL areIconsVisible = IsWindowVisible(g_state.hDesktopListView);

        if (g_state.bHideOnStartup) {
            // User toggled ON -> If currently visible, hide them.
            if (areIconsVisible) {
                Wh_Log(L"[SETTINGS] Setting changed to Hide. Hiding icons...");
                ToggleDesktopIcons();
            }
        } else {
            // User toggled OFF -> If currently hidden, show them.
            if (!areIconsVisible) {
                Wh_Log(L"[SETTINGS] Setting changed to Show. Showing icons...");
                RestoreIconsToVisible();
            }
        }
    }
}