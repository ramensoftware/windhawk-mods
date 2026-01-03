// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word by sending Ctrl+S
// @version         1.1
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
DWORD g_lastSaveTime = 0;
DWORD g_lastInputTime = 0;
bool g_isSendingCtrlS = false;
DWORD g_wordProcessId = 0;

// Original function pointer
typedef BOOL (WINAPI *TranslateMessage_t)(const MSG*);
TranslateMessage_t g_originalTranslateMessage = nullptr;

// Forward declarations
void ScheduleSave();
void SendCtrlS();

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

// Send Ctrl+S keystroke
void SendCtrlS() {
    // Verify Word is still the foreground window before sending
    if (!IsWordForeground()) {
        Wh_Log(L"Word is not the foreground window, skipping auto-save");
        return;
    }

    g_isSendingCtrlS = true;

    // Check if Shift or Alt are currently pressed
    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

    // Calculate how many inputs we need
    // Base: Ctrl down, S down, S up, Ctrl up = 4
    // Plus: Shift up (if pressed), Alt up (if pressed)
    int inputCount = 4;
    if (shiftPressed) inputCount++;
    if (altPressed) inputCount++;

    INPUT inputs[6] = {};  // Max 6 inputs
    int idx = 0;

    // Release Shift first if pressed (to avoid Ctrl+Shift+S = Apply Styles)
    if (shiftPressed) {
        inputs[idx].type = INPUT_KEYBOARD;
        inputs[idx].ki.wVk = VK_SHIFT;
        inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP;
        idx++;
        Wh_Log(L"Releasing Shift key before Ctrl+S");
    }

    // Release Alt if pressed (to avoid Alt+Ctrl+S combinations)
    if (altPressed) {
        inputs[idx].type = INPUT_KEYBOARD;
        inputs[idx].ki.wVk = VK_MENU;
        inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP;
        idx++;
        Wh_Log(L"Releasing Alt key before Ctrl+S");
    }

    // Press Ctrl
    inputs[idx].type = INPUT_KEYBOARD;
    inputs[idx].ki.wVk = VK_CONTROL;
    inputs[idx].ki.dwFlags = 0;
    idx++;

    // Press S
    inputs[idx].type = INPUT_KEYBOARD;
    inputs[idx].ki.wVk = 'S';
    inputs[idx].ki.dwFlags = 0;
    idx++;

    // Release S
    inputs[idx].type = INPUT_KEYBOARD;
    inputs[idx].ki.wVk = 'S';
    inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP;
    idx++;

    // Release Ctrl
    inputs[idx].type = INPUT_KEYBOARD;
    inputs[idx].ki.wVk = VK_CONTROL;
    inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP;
    idx++;

    UINT sent = SendInput(idx, inputs, sizeof(INPUT));

    g_isSendingCtrlS = false;

    Wh_Log(L"Sent Ctrl+S for auto-save (sent %u inputs)", sent);
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

    // Send Ctrl+S (function will verify Word is foreground)
    SendCtrlS();

    g_lastSaveTime = currentTime;
    g_lastInputTime = 0;
}

// Schedule a save operation
void ScheduleSave() {
    g_lastInputTime = GetTickCount();

    // Kill existing timer if any
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }

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
        Wh_Log(L"Manual save detected, resetting timer");
        return false;
    }

    // Ctrl combinations that modify document
    if (ctrlPressed) {
        if (wParam == 'V' || wParam == 'X' || wParam == 'Z' || wParam == 'Y') {
            Wh_Log(L"Edit key detected: Ctrl+%c", static_cast<char>(wParam));
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
            Wh_Log(L"Edit key detected: VK=%zu", static_cast<size_t>(wParam));
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
    Wh_Log(L"Word Local AutoSave mod initializing...");

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

    // Kill timer
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }

    Wh_Log(L"Word Local AutoSave mod uninitialized");
}

// Settings changed callback
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}
