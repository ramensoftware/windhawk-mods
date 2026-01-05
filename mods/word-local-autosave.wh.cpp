// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word by sending Ctrl+S
// @version         1.3
// @author          communism420
// @github          https://github.com/communism420
// @include         WINWORD.EXE
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Word Local AutoSave

This mod enables automatic saving for locally stored Word documents, similar to
how AutoSave works with OneDrive files.

## How it works

The mod monitors keyboard input in Microsoft Word. When you type, delete, paste,
or make any changes to your document, it automatically triggers a save after a
short delay.

## Features

- Detects typing, backspace, delete, enter, and clipboard operations (Ctrl+V, Ctrl+X, Ctrl+Z, Ctrl+Y)
- Configurable delay before saving
- Optional minimum interval between saves to prevent excessive disk writes
- Works with any locally saved Word document
- Only saves when Word is the active window
- Waits for ALL keys to be released before saving to prevent shortcut conflicts

## Settings

- **Save Delay (ms)**: How long to wait after the last keystroke before saving.
  Default is 1000ms (1 second).
- **Minimum Time Between Saves (ms)**: Minimum interval between consecutive saves.
  Set to 0 to disable this limit and allow saving as frequently as possible.

## Notes

- The mod only works with documents that have already been saved at least once.
  New unsaved documents will trigger the "Save As" dialog.
- The mod simulates pressing Ctrl+S, so it behaves exactly like manual saving.
- Manual Ctrl+S presses are detected and reset the auto-save timer.
- Auto-save only triggers when Microsoft Word is the foreground window.
- Auto-save waits for all keys to be released to avoid triggering wrong shortcuts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- saveDelay: 1000
  $name: Save Delay (ms)
  $description: Delay in milliseconds before auto-saving after a change is detected (minimum 100ms)
- minTimeBetweenSaves: 0
  $name: Minimum Time Between Saves (ms)
  $description: Minimum time between consecutive saves. Set to 0 to disable this limit.
*/
// ==/WindhawkModSettings==

#include <windows.h>

// Settings
struct {
    int saveDelay;
    int minTimeBetweenSaves;
} g_settings;

// Global state
UINT_PTR g_saveTimerId = 0;
UINT_PTR g_retryTimerId = 0;
DWORD g_lastSaveTime = 0;
DWORD g_lastInputTime = 0;
bool g_isSendingCtrlS = false;
DWORD g_wordProcessId = 0;
int g_retryCount = 0;

// Original function pointer
typedef BOOL (WINAPI *TranslateMessage_t)(const MSG*);
TranslateMessage_t g_originalTranslateMessage = nullptr;

// Forward declarations
void ScheduleSave();
void SendCtrlS();
void TrySave();

// Check if Word is the foreground window
bool IsWordForeground() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return false;
    }

    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foregroundWindow, &foregroundProcessId);

    return (foregroundProcessId == g_wordProcessId);
}

// Check if any keys are currently pressed that could interfere with Ctrl+S
bool AreAnyKeysPressed() {
    BYTE keyState[256];
    if (!GetKeyboardState(keyState)) {
        return false;  // If we can't get state, assume no keys pressed
    }
    
    // Check letters A-Z (0x41-0x5A) - if any letter is held, wait
    for (int i = 0x41; i <= 0x5A; i++) {
        if (keyState[i] & 0x80) {
            Wh_Log(L"Key 0x%02X is pressed, delaying save", i);
            return true;
        }
    }
    
    // Check numbers 0-9 (0x30-0x39)
    for (int i = 0x30; i <= 0x39; i++) {
        if (keyState[i] & 0x80) {
            Wh_Log(L"Key 0x%02X is pressed, delaying save", i);
            return true;
        }
    }
    
    // Check Shift and Alt (we don't want Ctrl+Shift+S or Ctrl+Alt+S)
    if (keyState[VK_SHIFT] & 0x80) {
        Wh_Log(L"Shift is pressed, delaying save");
        return true;
    }
    if (keyState[VK_LSHIFT] & 0x80) return true;
    if (keyState[VK_RSHIFT] & 0x80) return true;
    
    if (keyState[VK_MENU] & 0x80) {  // Alt
        Wh_Log(L"Alt is pressed, delaying save");
        return true;
    }
    if (keyState[VK_LMENU] & 0x80) return true;
    if (keyState[VK_RMENU] & 0x80) return true;
    
    // Check common editing keys
    if (keyState[VK_SPACE] & 0x80) return true;
    if (keyState[VK_RETURN] & 0x80) return true;
    if (keyState[VK_TAB] & 0x80) return true;
    if (keyState[VK_BACK] & 0x80) return true;
    if (keyState[VK_DELETE] & 0x80) return true;
    
    // Check numpad keys
    for (int i = VK_NUMPAD0; i <= VK_DIVIDE; i++) {
        if (keyState[i] & 0x80) return true;
    }
    
    // Check OEM keys (punctuation, brackets, etc.)
    int oemKeys[] = {
        VK_OEM_1, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7, VK_OEM_8,
        VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD
    };
    for (int key : oemKeys) {
        if (keyState[key] & 0x80) return true;
    }
    
    return false;
}

// Send Ctrl+S keystroke
void SendCtrlS() {
    g_isSendingCtrlS = true;

    INPUT inputs[4] = {};

    // Press Ctrl
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    inputs[0].ki.dwFlags = 0;

    // Press S
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'S';
    inputs[1].ki.dwFlags = 0;

    // Release S
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'S';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    // Release Ctrl
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT sent = SendInput(4, inputs, sizeof(INPUT));

    g_isSendingCtrlS = false;

    Wh_Log(L"Sent Ctrl+S for auto-save (sent %u inputs)", sent);
}

// Retry timer callback - checks if all keys are released
void CALLBACK RetryTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(nullptr, g_retryTimerId);
    g_retryTimerId = 0;
    
    TrySave();
}

// Try to perform save, retry if any keys are pressed
void TrySave() {
    // Verify Word is still the foreground window
    if (!IsWordForeground()) {
        Wh_Log(L"Word is not the foreground window, skipping auto-save");
        g_retryCount = 0;
        return;
    }

    // Check if ANY keys are currently pressed
    if (AreAnyKeysPressed()) {
        g_retryCount++;
        
        // Retry up to 50 times (5 seconds total with 100ms intervals)
        if (g_retryCount < 50) {
            // Try again in 100ms
            g_retryTimerId = SetTimer(nullptr, 0, 100, RetryTimerProc);
            return;
        } else {
            Wh_Log(L"Too many retries, giving up on this save");
            g_retryCount = 0;
            return;
        }
    }

    g_retryCount = 0;
    
    // All keys released - safe to send Ctrl+S
    SendCtrlS();
    
    g_lastSaveTime = GetTickCount();
    g_lastInputTime = 0;
}

// Timer callback for delayed save
void CALLBACK SaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(nullptr, g_saveTimerId);
    g_saveTimerId = 0;

    DWORD currentTime = GetTickCount();

    // Check minimum time between saves (only if enabled, i.e. > 0)
    if (g_settings.minTimeBetweenSaves > 0 && g_lastSaveTime > 0) {
        if ((currentTime - g_lastSaveTime) < static_cast<DWORD>(g_settings.minTimeBetweenSaves)) {
            Wh_Log(L"Skipping save - too soon since last save");
            return;
        }
    }

    // Check if there was recent input
    if (g_lastInputTime == 0) {
        return;
    }

    Wh_Log(L"Performing auto-save...");

    // Try to save (will retry if any keys are pressed)
    TrySave();
}

// Schedule a save operation
void ScheduleSave() {
    g_lastInputTime = GetTickCount();

    // Kill existing timers
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }
    if (g_retryTimerId != 0) {
        KillTimer(nullptr, g_retryTimerId);
        g_retryTimerId = 0;
    }
    g_retryCount = 0;

    // Set new timer
    g_saveTimerId = SetTimer(nullptr, 0, g_settings.saveDelay, SaveTimerProc);

    if (g_saveTimerId == 0) {
        Wh_Log(L"Failed to set timer: %lu", GetLastError());
    }
}

// Check if a key is an editing key that modifies the document
bool IsEditingKey(WPARAM wParam) {
    // Ignore if we're sending Ctrl+S ourselves
    if (g_isSendingCtrlS) {
        return false;
    }

    bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

    // Ignore Ctrl+S (manual save) - update last save time
    if (ctrlPressed && wParam == 'S') {
        g_lastSaveTime = GetTickCount();
        g_lastInputTime = 0;
        if (g_saveTimerId != 0) {
            KillTimer(nullptr, g_saveTimerId);
            g_saveTimerId = 0;
        }
        if (g_retryTimerId != 0) {
            KillTimer(nullptr, g_retryTimerId);
            g_retryTimerId = 0;
        }
        Wh_Log(L"Manual save detected, resetting timer");
        return false;
    }

    // Ctrl combinations that modify document
    if (ctrlPressed) {
        if (wParam == 'V' || wParam == 'X' || wParam == 'Z' || wParam == 'Y') {
            return true;
        }
        return false;
    }

    // Ignore Alt combinations
    if (altPressed) {
        return false;
    }

    // Printable characters
    if (wParam >= 0x20 && wParam <= 0x7E) {
        return true;
    }

    // Special editing keys
    switch (wParam) {
        case VK_BACK:
        case VK_DELETE:
        case VK_RETURN:
        case VK_TAB:
            return true;
    }

    return false;
}

// Hooked TranslateMessage
BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (lpMsg && lpMsg->message == WM_KEYDOWN) {
        if (IsEditingKey(lpMsg->wParam)) {
            ScheduleSave();
        }
    }

    return g_originalTranslateMessage(lpMsg);
}

// Load settings
void LoadSettings() {
    g_settings.saveDelay = Wh_GetIntSetting(L"saveDelay");
    g_settings.minTimeBetweenSaves = Wh_GetIntSetting(L"minTimeBetweenSaves");

    // Minimal validation - just prevent negative values and too small delay
    if (g_settings.saveDelay < 100) {
        g_settings.saveDelay = 100;
    }
    if (g_settings.minTimeBetweenSaves < 0) {
        g_settings.minTimeBetweenSaves = 0;
    }

    Wh_Log(L"Settings loaded: saveDelay=%d, minTimeBetweenSaves=%d (0=disabled)",
           g_settings.saveDelay, g_settings.minTimeBetweenSaves);
}

// Mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Word Local AutoSave mod v1.3 initializing...");

    // Store current process ID for foreground window check
    g_wordProcessId = GetCurrentProcessId();

    LoadSettings();

    // Hook TranslateMessage
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"Failed to get user32.dll handle");
        return FALSE;
    }

    void* translateMessageAddr = reinterpret_cast<void*>(
        GetProcAddress(user32, "TranslateMessage"));
    if (!translateMessageAddr) {
        Wh_Log(L"Failed to get TranslateMessage address");
        return FALSE;
    }

    Wh_Log(L"TranslateMessage found at %p", translateMessageAddr);

    if (!Wh_SetFunctionHook(translateMessageAddr,
                            reinterpret_cast<void*>(TranslateMessage_Hook),
                            reinterpret_cast<void**>(&g_originalTranslateMessage))) {
        Wh_Log(L"Failed to hook TranslateMessage");
        return FALSE;
    }

    Wh_Log(L"Word Local AutoSave mod initialized successfully!");

    return TRUE;
}

// Mod uninitialization
void Wh_ModUninit() {
    Wh_Log(L"Word Local AutoSave mod uninitializing...");

    // Kill timers
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }
    if (g_retryTimerId != 0) {
        KillTimer(nullptr, g_retryTimerId);
        g_retryTimerId = 0;
    }

    Wh_Log(L"Word Local AutoSave mod uninitialized");
}

// Settings changed callback
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}
