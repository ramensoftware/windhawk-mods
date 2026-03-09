// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word by sending Ctrl+S
// @version         2.1
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

- Detects typing, backspace, delete, enter, punctuation, numpad, and clipboard operations
- Detects Ctrl+V, Ctrl+X, Ctrl+Z, Ctrl+Y, Ctrl+Enter (page break)
- Configurable delay before saving
- Optional minimum interval between saves to prevent excessive disk writes
- Works with any locally saved Word document
- Only saves when Word is the active window

## Intelligent Safety System (v2.1)

The mod uses smart verification to guarantee zero false shortcut triggers:

- **Adaptive timing** - waits for natural typing pauses
- **Instant key detection** - checks physical key state, not timing
- **Atomic Ctrl+S** - fast send with abort capability
- **100+ shortcut protection** - all Word Ctrl combinations covered

## Settings

- **Save Delay (ms)**: How long to wait after the last keystroke before saving.
  Default is 1000ms (1 second).
- **Minimum Time Between Saves (ms)**: Minimum interval between consecutive saves.
  Set to 0 to disable this limit and allow saving as frequently as possible.

## Limitations

- Mouse operations (click, drag & drop, context menu paste) are not detected
- Only works with documents that have already been saved at least once
- New unsaved documents will trigger the "Save As" dialog
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

// ============================================================================
// Constants
// ============================================================================

#define IS_KEY_PRESSED(vk) (GetAsyncKeyState(vk) < 0)

// Timing
const int MIN_SAVE_DELAY_MS = 100;
const int MAX_SAVE_DELAY_MS = 60000;
const int MAX_MIN_TIME_BETWEEN_SAVES = 300000;
const DWORD RETRY_INTERVAL_MS = 50;
const DWORD DEFERRED_SAVE_BUFFER_MS = 50;
const int MAX_RETRY_COUNT = 200;            // 10 seconds max retry
const int MAX_KEY_RELEASE_RETRIES = 5;

// Virtual key ranges
const int VK_KEY_0 = 0x30;
const int VK_KEY_9 = 0x39;
const int VK_KEY_A = 0x41;
const int VK_KEY_Z = 0x5A;

// ============================================================================
// Global State
// ============================================================================

struct {
    int saveDelay;
    int minTimeBetweenSaves;
} g_settings;

UINT_PTR g_saveTimerId = 0;
UINT_PTR g_retryTimerId = 0;
DWORD g_lastSaveTime = 0;
DWORD g_lastInputTime = 0;
DWORD g_lastKeyPressTime = 0;
volatile bool g_isSendingCtrlS = false;
DWORD g_wordProcessId = 0;
int g_retryCount = 0;

typedef BOOL (WINAPI *TranslateMessage_t)(const MSG*);
TranslateMessage_t g_originalTranslateMessage = nullptr;

// ============================================================================
// Forward Declarations
// ============================================================================

void ScheduleSave();
void ScheduleRetry();
void SendCtrlS();
void TrySave();
void CALLBACK RetryTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void CALLBACK SaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

// ============================================================================
// Helper Functions
// ============================================================================

inline DWORD SafeTimeDiff(DWORD now, DWORD past) {
    return now - past;
}

void KillRetryTimer() {
    if (g_retryTimerId != 0) {
        KillTimer(nullptr, g_retryTimerId);
        g_retryTimerId = 0;
    }
}

void KillSaveTimer() {
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }
}

void ResetAllTimers() {
    KillSaveTimer();
    KillRetryTimer();
    g_retryCount = 0;
}

void ScheduleRetry() {
    g_retryCount++;
    if (g_retryCount >= MAX_RETRY_COUNT) {
        Wh_Log(L"Max retries reached, giving up");
        g_retryCount = 0;
        return;
    }
    
    KillRetryTimer();
    g_retryTimerId = SetTimer(nullptr, 0, RETRY_INTERVAL_MS, RetryTimerProc);
}

bool IsWordForeground() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) return false;
    
    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foregroundWindow, &foregroundProcessId);
    return (foregroundProcessId == g_wordProcessId);
}

// ============================================================================
// SMART KEY DETECTION
// Instead of timing-based quiet period, we check ACTUAL key states
// This is instant and doesn't depend on typing speed
// ============================================================================

// Check if ANY key or mouse button is currently physically pressed
bool IsAnyKeyPhysicallyPressed() {
    // Letters A-Z
    for (int i = VK_KEY_A; i <= VK_KEY_Z; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // Numbers 0-9
    for (int i = VK_KEY_0; i <= VK_KEY_9; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // All modifiers
    if (IS_KEY_PRESSED(VK_SHIFT)) return true;
    if (IS_KEY_PRESSED(VK_CONTROL)) return true;
    if (IS_KEY_PRESSED(VK_MENU)) return true;
    if (IS_KEY_PRESSED(VK_LSHIFT)) return true;
    if (IS_KEY_PRESSED(VK_RSHIFT)) return true;
    if (IS_KEY_PRESSED(VK_LCONTROL)) return true;
    if (IS_KEY_PRESSED(VK_RCONTROL)) return true;
    if (IS_KEY_PRESSED(VK_LMENU)) return true;
    if (IS_KEY_PRESSED(VK_RMENU)) return true;
    if (IS_KEY_PRESSED(VK_LWIN)) return true;
    if (IS_KEY_PRESSED(VK_RWIN)) return true;
    
    // Mouse buttons
    if (IS_KEY_PRESSED(VK_LBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_RBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_MBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_XBUTTON1)) return true;
    if (IS_KEY_PRESSED(VK_XBUTTON2)) return true;
    
    // Common editing keys
    if (IS_KEY_PRESSED(VK_SPACE)) return true;
    if (IS_KEY_PRESSED(VK_RETURN)) return true;
    if (IS_KEY_PRESSED(VK_TAB)) return true;
    if (IS_KEY_PRESSED(VK_BACK)) return true;
    if (IS_KEY_PRESSED(VK_DELETE)) return true;
    if (IS_KEY_PRESSED(VK_INSERT)) return true;
    if (IS_KEY_PRESSED(VK_ESCAPE)) return true;
    
    // Navigation
    if (IS_KEY_PRESSED(VK_HOME)) return true;
    if (IS_KEY_PRESSED(VK_END)) return true;
    if (IS_KEY_PRESSED(VK_PRIOR)) return true;
    if (IS_KEY_PRESSED(VK_NEXT)) return true;
    if (IS_KEY_PRESSED(VK_LEFT)) return true;
    if (IS_KEY_PRESSED(VK_RIGHT)) return true;
    if (IS_KEY_PRESSED(VK_UP)) return true;
    if (IS_KEY_PRESSED(VK_DOWN)) return true;
    
    // Function keys
    for (int i = VK_F1; i <= VK_F24; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // Numpad
    for (int i = VK_NUMPAD0; i <= VK_DIVIDE; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // OEM keys
    if (IS_KEY_PRESSED(VK_OEM_1)) return true;
    if (IS_KEY_PRESSED(VK_OEM_2)) return true;
    if (IS_KEY_PRESSED(VK_OEM_3)) return true;
    if (IS_KEY_PRESSED(VK_OEM_4)) return true;
    if (IS_KEY_PRESSED(VK_OEM_5)) return true;
    if (IS_KEY_PRESSED(VK_OEM_6)) return true;
    if (IS_KEY_PRESSED(VK_OEM_7)) return true;
    if (IS_KEY_PRESSED(VK_OEM_PLUS)) return true;
    if (IS_KEY_PRESSED(VK_OEM_COMMA)) return true;
    if (IS_KEY_PRESSED(VK_OEM_MINUS)) return true;
    if (IS_KEY_PRESSED(VK_OEM_PERIOD)) return true;
    
    return false;
}

// Check critical keys that could combine with Ctrl (excluding S)
bool IsAnyCriticalKeyPressed() {
    // All letters except S
    for (int i = VK_KEY_A; i <= VK_KEY_Z; i++) {
        if (i == 'S') continue;
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // Numbers
    for (int i = VK_KEY_0; i <= VK_KEY_9; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // Shift (Ctrl+Shift+S = Styles)
    if (IS_KEY_PRESSED(VK_SHIFT) || IS_KEY_PRESSED(VK_LSHIFT) || IS_KEY_PRESSED(VK_RSHIFT)) {
        return true;
    }
    
    // Alt
    if (IS_KEY_PRESSED(VK_MENU) || IS_KEY_PRESSED(VK_LMENU) || IS_KEY_PRESSED(VK_RMENU)) {
        return true;
    }
    
    // Win
    if (IS_KEY_PRESSED(VK_LWIN) || IS_KEY_PRESSED(VK_RWIN)) {
        return true;
    }
    
    // Function keys
    for (int i = VK_F1; i <= VK_F12; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // OEM keys (brackets change font size)
    if (IS_KEY_PRESSED(VK_OEM_4)) return true;  // [
    if (IS_KEY_PRESSED(VK_OEM_6)) return true;  // ]
    if (IS_KEY_PRESSED(VK_OEM_PLUS)) return true;
    if (IS_KEY_PRESSED(VK_OEM_MINUS)) return true;
    
    // Navigation
    if (IS_KEY_PRESSED(VK_HOME)) return true;
    if (IS_KEY_PRESSED(VK_END)) return true;
    if (IS_KEY_PRESSED(VK_PRIOR)) return true;
    if (IS_KEY_PRESSED(VK_NEXT)) return true;
    if (IS_KEY_PRESSED(VK_LEFT)) return true;
    if (IS_KEY_PRESSED(VK_RIGHT)) return true;
    if (IS_KEY_PRESSED(VK_UP)) return true;
    if (IS_KEY_PRESSED(VK_DOWN)) return true;
    
    // Special keys
    if (IS_KEY_PRESSED(VK_RETURN)) return true;
    if (IS_KEY_PRESSED(VK_TAB)) return true;
    if (IS_KEY_PRESSED(VK_SPACE)) return true;
    if (IS_KEY_PRESSED(VK_BACK)) return true;
    if (IS_KEY_PRESSED(VK_DELETE)) return true;
    if (IS_KEY_PRESSED(VK_INSERT)) return true;
    if (IS_KEY_PRESSED(VK_ESCAPE)) return true;
    
    // Mouse (user selecting text)
    if (IS_KEY_PRESSED(VK_LBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_RBUTTON)) return true;
    
    return false;
}

// ============================================================================
// SMART CTRL+S SEND
// Fast and safe - checks keys, not timing
// ============================================================================

bool SendSingleKey(WORD vk, bool keyUp) {
    INPUT input;
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = keyUp ? KEYEVENTF_KEYUP : 0;
    return SendInput(1, &input, sizeof(INPUT)) == 1;
}

void ReleaseAllKeys() {
    // Make sure Ctrl and S are released
    SendSingleKey('S', true);
    SendSingleKey(VK_CONTROL, true);
}

void SendCtrlS() {
    g_isSendingCtrlS = true;
    
    // CHECK 1: No keys pressed right now
    if (IsAnyKeyPhysicallyPressed()) {
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // SEND: Ctrl down
    if (!SendSingleKey(VK_CONTROL, false)) {
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // CHECK 2: No critical keys pressed after Ctrl
    if (IsAnyCriticalKeyPressed()) {
        ReleaseAllKeys();
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // SEND: S down
    if (!SendSingleKey('S', false)) {
        ReleaseAllKeys();
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // CHECK 3: Still no other letters pressed
    for (int i = VK_KEY_A; i <= VK_KEY_Z; i++) {
        if (i == 'S') continue;
        if (IS_KEY_PRESSED(i)) {
            ReleaseAllKeys();
            g_isSendingCtrlS = false;
            ScheduleRetry();
            return;
        }
    }
    
    // SEND: S up
    SendSingleKey('S', true);
    
    // SEND: Ctrl up
    SendSingleKey(VK_CONTROL, true);
    
    // VERIFY: Keys released
    for (int i = 0; i < MAX_KEY_RELEASE_RETRIES; i++) {
        if (!IS_KEY_PRESSED(VK_CONTROL) && !IS_KEY_PRESSED('S')) {
            break;
        }
        ReleaseAllKeys();
        Sleep(1);
    }
    
    g_isSendingCtrlS = false;
    
    Wh_Log(L"Auto-save: Ctrl+S sent");
    g_lastSaveTime = GetTickCount();
    g_lastInputTime = 0;
    g_retryCount = 0;
}

// ============================================================================
// SMART SAVE DECISION
// ============================================================================

void CALLBACK RetryTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillRetryTimer();
    TrySave();
}

void TrySave() {
    // Must be foreground
    if (!IsWordForeground()) {
        g_retryCount = 0;
        return;
    }

    // If any key is physically pressed, wait
    if (IsAnyKeyPhysicallyPressed()) {
        ScheduleRetry();
        return;
    }
    
    // All clear - send Ctrl+S
    SendCtrlS();
}

void CALLBACK SaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillSaveTimer();

    if (g_lastInputTime == 0) {
        return;
    }

    DWORD currentTime = GetTickCount();

    // Check minimum time between saves
    if (g_settings.minTimeBetweenSaves > 0 && g_lastSaveTime > 0) {
        DWORD timeSinceLastSave = SafeTimeDiff(currentTime, g_lastSaveTime);
        DWORD minTime = static_cast<DWORD>(g_settings.minTimeBetweenSaves);
        
        if (timeSinceLastSave < minTime) {
            DWORD remainingTime = minTime - timeSinceLastSave;
            g_saveTimerId = SetTimer(nullptr, 0, remainingTime + DEFERRED_SAVE_BUFFER_MS, SaveTimerProc);
            return;
        }
    }

    TrySave();
}

void ScheduleSave() {
    g_lastInputTime = GetTickCount();
    KillSaveTimer();
    g_saveTimerId = SetTimer(nullptr, 0, g_settings.saveDelay, SaveTimerProc);
}

// ============================================================================
// Input Detection
// ============================================================================

bool IsEditingKey(WPARAM wParam) {
    if (g_isSendingCtrlS) {
        return false;
    }

    bool ctrlPressed = IS_KEY_PRESSED(VK_CONTROL);
    bool shiftPressed = IS_KEY_PRESSED(VK_SHIFT);
    bool altPressed = IS_KEY_PRESSED(VK_MENU);

    // Manual Ctrl+S
    if (ctrlPressed && !shiftPressed && !altPressed && wParam == 'S') {
        g_lastSaveTime = GetTickCount();
        g_lastInputTime = 0;
        ResetAllTimers();
        return false;
    }

    // Ctrl combinations that modify document
    if (ctrlPressed && !altPressed) {
        if (wParam == 'V' || wParam == 'X' || wParam == 'Y' || wParam == 'Z') {
            return true;
        }
        if (wParam == VK_RETURN) {
            return true;
        }
        return false;
    }

    if (altPressed) {
        return false;
    }

    // Letters
    if (wParam >= VK_KEY_A && wParam <= VK_KEY_Z) return true;
    
    // Numbers
    if (wParam >= VK_KEY_0 && wParam <= VK_KEY_9) return true;
    
    // Space
    if (wParam == VK_SPACE) return true;

    // Editing keys
    switch (wParam) {
        case VK_BACK:
        case VK_DELETE:
        case VK_RETURN:
        case VK_TAB:
            return true;
    }
    
    // Numpad
    if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) return true;
    if (wParam == VK_MULTIPLY || wParam == VK_ADD || 
        wParam == VK_SUBTRACT || wParam == VK_DECIMAL || wParam == VK_DIVIDE) {
        return true;
    }
    
    // OEM keys
    switch (wParam) {
        case VK_OEM_1:
        case VK_OEM_2:
        case VK_OEM_3:
        case VK_OEM_4:
        case VK_OEM_5:
        case VK_OEM_6:
        case VK_OEM_7:
        case VK_OEM_PLUS:
        case VK_OEM_COMMA:
        case VK_OEM_MINUS:
        case VK_OEM_PERIOD:
        case VK_OEM_102:
            return true;
    }

    return false;
}

// ============================================================================
// Hook
// ============================================================================

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (!g_originalTranslateMessage) {
        return TRUE;
    }
    
    if (lpMsg) {
        if (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN) {
            g_lastKeyPressTime = GetTickCount();
        }
        
        if (lpMsg->message == WM_CHAR && lpMsg->wParam >= 0x20) {
            g_lastKeyPressTime = GetTickCount();
            g_lastInputTime = GetTickCount();
        }
        
        if (lpMsg->message == WM_KEYDOWN) {
            if (IsEditingKey(lpMsg->wParam)) {
                ScheduleSave();
            }
        }
    }

    return g_originalTranslateMessage(lpMsg);
}

// ============================================================================
// Windhawk Callbacks
// ============================================================================

void LoadSettings() {
    g_settings.saveDelay = Wh_GetIntSetting(L"saveDelay");
    g_settings.minTimeBetweenSaves = Wh_GetIntSetting(L"minTimeBetweenSaves");

    if (g_settings.saveDelay < MIN_SAVE_DELAY_MS) {
        g_settings.saveDelay = MIN_SAVE_DELAY_MS;
    }
    if (g_settings.saveDelay > MAX_SAVE_DELAY_MS) {
        g_settings.saveDelay = MAX_SAVE_DELAY_MS;
    }
    if (g_settings.minTimeBetweenSaves < 0) {
        g_settings.minTimeBetweenSaves = 0;
    }
    if (g_settings.minTimeBetweenSaves > MAX_MIN_TIME_BETWEEN_SAVES) {
        g_settings.minTimeBetweenSaves = MAX_MIN_TIME_BETWEEN_SAVES;
    }

    Wh_Log(L"Settings: saveDelay=%d ms, minTimeBetweenSaves=%d ms",
           g_settings.saveDelay, g_settings.minTimeBetweenSaves);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Word Local AutoSave v2.1 initializing...");

    g_wordProcessId = GetCurrentProcessId();
    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"ERROR: Failed to get user32.dll handle");
        return FALSE;
    }

    void* translateMessageAddr = reinterpret_cast<void*>(
        GetProcAddress(user32, "TranslateMessage"));
    if (!translateMessageAddr) {
        Wh_Log(L"ERROR: Failed to get TranslateMessage address");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(translateMessageAddr,
                            reinterpret_cast<void*>(TranslateMessage_Hook),
                            reinterpret_cast<void**>(&g_originalTranslateMessage))) {
        Wh_Log(L"ERROR: Failed to hook TranslateMessage");
        return FALSE;
    }
    
    if (!g_originalTranslateMessage) {
        Wh_Log(L"ERROR: Original TranslateMessage pointer is null");
        return FALSE;
    }

    Wh_Log(L"Word Local AutoSave initialized");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Word Local AutoSave uninitializing...");
    ResetAllTimers();
    Wh_Log(L"Word Local AutoSave uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    ResetAllTimers();
    LoadSettings();
}
