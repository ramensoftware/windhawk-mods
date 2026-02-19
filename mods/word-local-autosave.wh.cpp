// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word by sending Ctrl+S
// @version         1.8
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

- Detects typing, backspace, delete, enter, punctuation, numpad, and clipboard operations (Ctrl+V, Ctrl+X, Ctrl+Z, Ctrl+Y)
- Configurable delay before saving
- Optional minimum interval between saves to prevent excessive disk writes
- Works with any locally saved Word document
- Only saves when Word is the active window
- Requires a quiet period (500ms no key presses) before saving to prevent shortcut conflicts

## Settings

- **Save Delay (ms)**: How long to wait after the last keystroke before saving.
  Default is 1000ms (1 second).
- **Minimum Time Between Saves (ms)**: Minimum interval between consecutive saves.
  Set to 0 to disable this limit and allow saving as frequently as possible.

## Limitations

- Mouse operations (click, drag & drop, context menu paste) are not detected
- Only works with documents that have already been saved at least once
- New unsaved documents will trigger the "Save As" dialog

## Notes

- The mod simulates pressing Ctrl+S, so it behaves exactly like manual saving.
- Manual Ctrl+S presses are detected and reset the auto-save timer.
- Auto-save only triggers when Microsoft Word is the foreground window.
- Auto-save requires 500ms of keyboard inactivity to prevent triggering wrong shortcuts.
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

// Key state check macro
#define IS_KEY_PRESSED(vk) ((GetAsyncKeyState(vk) & 0x8000) != 0)

// Timing constants
// QUIET_PERIOD_MS must be long enough to ensure user finished typing
// Average fast typing: 60-80 WPM = ~100-150ms between keystrokes
// We use 500ms to be safe - this is the minimum time since last key press
const DWORD QUIET_PERIOD_MS = 500;          // Minimum quiet time before saving
const DWORD RETRY_INTERVAL_MS = 100;        // Interval between retry attempts
const int MAX_RETRY_COUNT = 50;             // Maximum retry attempts (5 seconds total)
const int MIN_SAVE_DELAY_MS = 100;          // Minimum allowed save delay
const int MAX_SAVE_DELAY_MS = 60000;        // Maximum allowed save delay (1 minute)
const int MAX_MIN_TIME_BETWEEN_SAVES = 300000; // Maximum minTimeBetweenSaves (5 minutes)

// Virtual key code ranges
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
bool g_isSendingCtrlS = false;
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

// Safe time difference calculation (handles GetTickCount overflow)
inline DWORD SafeTimeDiff(DWORD now, DWORD past) {
    return now - past;  // Unsigned subtraction handles overflow correctly
}

// Kill retry timer safely
void KillRetryTimer() {
    if (g_retryTimerId != 0) {
        KillTimer(nullptr, g_retryTimerId);
        g_retryTimerId = 0;
    }
}

// Kill save timer safely
void KillSaveTimer() {
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }
}

// Kill all timers and reset state
void ResetAllTimers() {
    KillSaveTimer();
    KillRetryTimer();
    g_retryCount = 0;
}

// Schedule a retry attempt
void ScheduleRetry() {
    g_retryCount++;
    if (g_retryCount >= MAX_RETRY_COUNT) {
        Wh_Log(L"Too many retries (%d), giving up on this save", g_retryCount);
        g_retryCount = 0;
        return;
    }
    
    KillRetryTimer();
    g_retryTimerId = SetTimer(nullptr, 0, RETRY_INTERVAL_MS, RetryTimerProc);
}

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

// Check if enough quiet time has passed since last key press
bool HasQuietPeriodPassed() {
    if (g_lastKeyPressTime == 0) {
        return true;
    }
    
    DWORD currentTime = GetTickCount();
    DWORD timeSinceLastKey = SafeTimeDiff(currentTime, g_lastKeyPressTime);
    
    return timeSinceLastKey >= QUIET_PERIOD_MS;
}

// Check if any keys are physically pressed right now
// This is CRITICAL for preventing accidental Word shortcuts
// We check EVERY key that could combine with Ctrl to trigger a Word function
bool AreAnyKeysPressed() {
    // =========================================
    // LETTERS A-Z (Ctrl+A, Ctrl+B, etc.)
    // These are the most dangerous - almost all have Word shortcuts
    // =========================================
    for (int i = VK_KEY_A; i <= VK_KEY_Z; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // =========================================
    // NUMBERS 0-9 (Ctrl+1 = single spacing, etc.)
    // =========================================
    for (int i = VK_KEY_0; i <= VK_KEY_9; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // =========================================
    // ALL MODIFIER KEYS
    // If any modifier is held, we must wait
    // =========================================
    if (IS_KEY_PRESSED(VK_SHIFT)) return true;
    if (IS_KEY_PRESSED(VK_CONTROL)) return true;
    if (IS_KEY_PRESSED(VK_MENU)) return true;      // Alt
    if (IS_KEY_PRESSED(VK_LWIN)) return true;
    if (IS_KEY_PRESSED(VK_RWIN)) return true;
    if (IS_KEY_PRESSED(VK_LSHIFT)) return true;
    if (IS_KEY_PRESSED(VK_RSHIFT)) return true;
    if (IS_KEY_PRESSED(VK_LCONTROL)) return true;
    if (IS_KEY_PRESSED(VK_RCONTROL)) return true;
    if (IS_KEY_PRESSED(VK_LMENU)) return true;     // Left Alt
    if (IS_KEY_PRESSED(VK_RMENU)) return true;     // Right Alt (AltGr)
    
    // =========================================
    // EDITING KEYS
    // =========================================
    if (IS_KEY_PRESSED(VK_SPACE)) return true;
    if (IS_KEY_PRESSED(VK_RETURN)) return true;
    if (IS_KEY_PRESSED(VK_TAB)) return true;
    if (IS_KEY_PRESSED(VK_BACK)) return true;
    if (IS_KEY_PRESSED(VK_DELETE)) return true;
    if (IS_KEY_PRESSED(VK_INSERT)) return true;
    if (IS_KEY_PRESSED(VK_ESCAPE)) return true;
    
    // =========================================
    // NAVIGATION KEYS (Ctrl+Home = start of doc)
    // =========================================
    if (IS_KEY_PRESSED(VK_HOME)) return true;
    if (IS_KEY_PRESSED(VK_END)) return true;
    if (IS_KEY_PRESSED(VK_PRIOR)) return true;     // Page Up
    if (IS_KEY_PRESSED(VK_NEXT)) return true;      // Page Down
    if (IS_KEY_PRESSED(VK_LEFT)) return true;
    if (IS_KEY_PRESSED(VK_RIGHT)) return true;
    if (IS_KEY_PRESSED(VK_UP)) return true;
    if (IS_KEY_PRESSED(VK_DOWN)) return true;
    
    // =========================================
    // FUNCTION KEYS (F1=Help, F4=Close, F7=Spelling)
    // =========================================
    for (int i = VK_F1; i <= VK_F24; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // =========================================
    // NUMPAD (all keys)
    // =========================================
    for (int i = VK_NUMPAD0; i <= VK_DIVIDE; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    if (IS_KEY_PRESSED(VK_NUMLOCK)) return true;
    
    // =========================================
    // OEM/PUNCTUATION KEYS (Ctrl+] = font size)
    // =========================================
    if (IS_KEY_PRESSED(VK_OEM_1)) return true;      // ;:
    if (IS_KEY_PRESSED(VK_OEM_2)) return true;      // /?
    if (IS_KEY_PRESSED(VK_OEM_3)) return true;      // `~
    if (IS_KEY_PRESSED(VK_OEM_4)) return true;      // [{ - Ctrl+[ = decrease font!
    if (IS_KEY_PRESSED(VK_OEM_5)) return true;      // \|
    if (IS_KEY_PRESSED(VK_OEM_6)) return true;      // ]} - Ctrl+] = increase font!
    if (IS_KEY_PRESSED(VK_OEM_7)) return true;      // '"
    if (IS_KEY_PRESSED(VK_OEM_8)) return true;      // misc
    if (IS_KEY_PRESSED(VK_OEM_PLUS)) return true;   // =+
    if (IS_KEY_PRESSED(VK_OEM_COMMA)) return true;  // ,<
    if (IS_KEY_PRESSED(VK_OEM_MINUS)) return true;  // -_
    if (IS_KEY_PRESSED(VK_OEM_PERIOD)) return true; // .>
    if (IS_KEY_PRESSED(VK_OEM_102)) return true;    // non-US
    
    // =========================================
    // TOGGLE KEYS (check if being pressed, not toggled)
    // =========================================
    if (IS_KEY_PRESSED(VK_CAPITAL)) return true;    // Caps Lock
    if (IS_KEY_PRESSED(VK_SCROLL)) return true;     // Scroll Lock
    
    // =========================================
    // SYSTEM KEYS
    // =========================================
    if (IS_KEY_PRESSED(VK_SNAPSHOT)) return true;   // Print Screen
    if (IS_KEY_PRESSED(VK_PAUSE)) return true;      // Pause/Break
    if (IS_KEY_PRESSED(VK_APPS)) return true;       // Context Menu key
    
    // =========================================
    // BROWSER/MEDIA KEYS (some keyboards)
    // =========================================
    if (IS_KEY_PRESSED(VK_BROWSER_BACK)) return true;
    if (IS_KEY_PRESSED(VK_BROWSER_FORWARD)) return true;
    if (IS_KEY_PRESSED(VK_BROWSER_REFRESH)) return true;
    if (IS_KEY_PRESSED(VK_BROWSER_STOP)) return true;
    if (IS_KEY_PRESSED(VK_BROWSER_SEARCH)) return true;
    if (IS_KEY_PRESSED(VK_BROWSER_FAVORITES)) return true;
    if (IS_KEY_PRESSED(VK_BROWSER_HOME)) return true;
    if (IS_KEY_PRESSED(VK_VOLUME_MUTE)) return true;
    if (IS_KEY_PRESSED(VK_VOLUME_DOWN)) return true;
    if (IS_KEY_PRESSED(VK_VOLUME_UP)) return true;
    if (IS_KEY_PRESSED(VK_MEDIA_NEXT_TRACK)) return true;
    if (IS_KEY_PRESSED(VK_MEDIA_PREV_TRACK)) return true;
    if (IS_KEY_PRESSED(VK_MEDIA_STOP)) return true;
    if (IS_KEY_PRESSED(VK_MEDIA_PLAY_PAUSE)) return true;
    
    return false;
}

// ============================================================================
// Core Logic
// ============================================================================

// Helper: Send a single keyboard input
bool SendSingleKey(WORD vk, bool keyUp) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = keyUp ? KEYEVENTF_KEYUP : 0;
    return SendInput(1, &input, sizeof(INPUT)) == 1;
}

// Helper: Release Ctrl key (cleanup after abort)
void ReleaseCtrl() {
    SendSingleKey(VK_CONTROL, true);
}

// Send Ctrl+S keystroke with safety checks between each step
// This is the CRITICAL function - we must prevent ANY accidental shortcuts
void SendCtrlS() {
    g_isSendingCtrlS = true;
    
    // =========================================
    // STEP 1: Final safety check before starting
    // =========================================
    if (AreAnyKeysPressed()) {
        Wh_Log(L"Abort: Key pressed before Ctrl send");
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // =========================================
    // STEP 2: Press Ctrl
    // =========================================
    if (!SendSingleKey(VK_CONTROL, false)) {
        Wh_Log(L"Failed to send Ctrl press");
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // =========================================
    // STEP 3: CHECK - Did user press anything while we sent Ctrl?
    // This is the CRITICAL check that prevents Ctrl+[other key] combos
    // We check all keys EXCEPT Ctrl itself (which we just pressed)
    // =========================================
    
    // Check all letters (Ctrl+A, Ctrl+B, etc. - the most dangerous)
    for (int i = VK_KEY_A; i <= VK_KEY_Z; i++) {
        if (i == 'S') continue;  // We're about to press S ourselves
        if (IS_KEY_PRESSED(i)) {
            Wh_Log(L"Abort: Key 0x%02X pressed after Ctrl - preventing Ctrl+%c", i, (char)i);
            ReleaseCtrl();
            g_isSendingCtrlS = false;
            ScheduleRetry();
            return;
        }
    }
    
    // Check numbers (Ctrl+1 = single space, etc.)
    for (int i = VK_KEY_0; i <= VK_KEY_9; i++) {
        if (IS_KEY_PRESSED(i)) {
            Wh_Log(L"Abort: Number key pressed after Ctrl");
            ReleaseCtrl();
            g_isSendingCtrlS = false;
            ScheduleRetry();
            return;
        }
    }
    
    // Check other modifiers (Shift would make Ctrl+Shift+S = Styles)
    if (IS_KEY_PRESSED(VK_SHIFT) || IS_KEY_PRESSED(VK_LSHIFT) || IS_KEY_PRESSED(VK_RSHIFT)) {
        Wh_Log(L"Abort: Shift pressed after Ctrl - preventing Ctrl+Shift combo");
        ReleaseCtrl();
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    if (IS_KEY_PRESSED(VK_MENU) || IS_KEY_PRESSED(VK_LMENU) || IS_KEY_PRESSED(VK_RMENU)) {
        Wh_Log(L"Abort: Alt pressed after Ctrl");
        ReleaseCtrl();
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // Check bracket keys (Ctrl+[ and Ctrl+] change font size)
    if (IS_KEY_PRESSED(VK_OEM_4) || IS_KEY_PRESSED(VK_OEM_6)) {
        Wh_Log(L"Abort: Bracket key pressed after Ctrl - preventing font size change");
        ReleaseCtrl();
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // Check function keys
    for (int i = VK_F1; i <= VK_F12; i++) {
        if (IS_KEY_PRESSED(i)) {
            Wh_Log(L"Abort: F-key pressed after Ctrl");
            ReleaseCtrl();
            g_isSendingCtrlS = false;
            ScheduleRetry();
            return;
        }
    }
    
    // =========================================
    // STEP 4: Press S
    // =========================================
    if (!SendSingleKey('S', false)) {
        Wh_Log(L"Failed to send S press");
        ReleaseCtrl();
        g_isSendingCtrlS = false;
        ScheduleRetry();
        return;
    }
    
    // =========================================
    // STEP 5: Release S
    // =========================================
    if (!SendSingleKey('S', true)) {
        Wh_Log(L"Failed to send S release");
        // Continue anyway - S might be stuck but we need to release Ctrl
    }
    
    // =========================================
    // STEP 6: Release Ctrl
    // =========================================
    if (!SendSingleKey(VK_CONTROL, true)) {
        Wh_Log(L"Failed to send Ctrl release");
        // Try again
        SendSingleKey(VK_CONTROL, true);
    }
    
    g_isSendingCtrlS = false;
    
    Wh_Log(L"Auto-save: Ctrl+S sent successfully");
    g_lastSaveTime = GetTickCount();
    g_lastInputTime = 0;
    g_retryCount = 0;
}

// Retry timer callback
void CALLBACK RetryTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillRetryTimer();
    TrySave();
}

// Try to perform save
void TrySave() {
    // Verify Word is still the foreground window
    if (!IsWordForeground()) {
        Wh_Log(L"Word not in foreground, skipping auto-save");
        g_retryCount = 0;
        return;
    }

    // Check if any keys are currently pressed
    if (AreAnyKeysPressed()) {
        ScheduleRetry();
        return;
    }
    
    // Check quiet period
    if (!HasQuietPeriodPassed()) {
        ScheduleRetry();
        return;
    }

    // All conditions met - send Ctrl+S
    Wh_Log(L"Conditions met, sending Ctrl+S...");
    SendCtrlS();
}

// Timer callback for delayed save
void CALLBACK SaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillSaveTimer();

    // Check if there was recent input
    if (g_lastInputTime == 0) {
        return;
    }

    DWORD currentTime = GetTickCount();

    // Check minimum time between saves
    if (g_settings.minTimeBetweenSaves > 0 && g_lastSaveTime > 0) {
        DWORD timeSinceLastSave = SafeTimeDiff(currentTime, g_lastSaveTime);
        if (timeSinceLastSave < static_cast<DWORD>(g_settings.minTimeBetweenSaves)) {
            // Schedule deferred save instead of losing changes
            DWORD remainingTime = g_settings.minTimeBetweenSaves - timeSinceLastSave;
            Wh_Log(L"Deferring save - %lu ms until minTimeBetweenSaves", remainingTime);
            g_saveTimerId = SetTimer(nullptr, 0, remainingTime + 50, SaveTimerProc);
            return;
        }
    }

    TrySave();
}

// Schedule a save operation
void ScheduleSave() {
    g_lastInputTime = GetTickCount();

    // Kill existing timers
    ResetAllTimers();

    // Set new timer
    g_saveTimerId = SetTimer(nullptr, 0, g_settings.saveDelay, SaveTimerProc);

    if (g_saveTimerId == 0) {
        Wh_Log(L"Failed to set save timer: %lu", GetLastError());
    }
}

// Check if a key is an editing key that modifies the document
bool IsEditingKey(WPARAM wParam) {
    // Ignore our own Ctrl+S
    if (g_isSendingCtrlS) {
        return false;
    }

    bool ctrlPressed = IS_KEY_PRESSED(VK_CONTROL);
    bool shiftPressed = IS_KEY_PRESSED(VK_SHIFT);
    bool altPressed = IS_KEY_PRESSED(VK_MENU);

    // Handle manual Ctrl+S - reset timers
    if (ctrlPressed && !shiftPressed && !altPressed && wParam == 'S') {
        g_lastSaveTime = GetTickCount();
        g_lastInputTime = 0;
        ResetAllTimers();
        Wh_Log(L"Manual save detected");
        return false;
    }

    // Ctrl combinations that modify document
    if (ctrlPressed && !altPressed) {
        // Ctrl+V (paste), Ctrl+X (cut), Ctrl+Y (redo)
        if (wParam == 'V' || wParam == 'X' || wParam == 'Y') {
            return true;
        }
        // Ctrl+Z (undo) - with or without Shift (Ctrl+Shift+Z = redo in some apps)
        if (wParam == 'Z') {
            return true;
        }
        // All other Ctrl combinations are not editing keys
        return false;
    }

    // Ignore Alt combinations (menu shortcuts)
    if (altPressed) {
        return false;
    }

    // Letter keys A-Z
    if (wParam >= VK_KEY_A && wParam <= VK_KEY_Z) {
        return true;
    }
    
    // Number keys 0-9
    if (wParam >= VK_KEY_0 && wParam <= VK_KEY_9) {
        return true;
    }
    
    // Space
    if (wParam == VK_SPACE) {
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
    
    // Numpad numbers and operators
    if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) {
        return true;
    }
    if (wParam == VK_MULTIPLY || wParam == VK_ADD || 
        wParam == VK_SUBTRACT || wParam == VK_DECIMAL || wParam == VK_DIVIDE) {
        return true;
    }
    
    // OEM keys (punctuation)
    switch (wParam) {
        case VK_OEM_1:      // ;:
        case VK_OEM_2:      // /?
        case VK_OEM_3:      // `~
        case VK_OEM_4:      // [{
        case VK_OEM_5:      // \|
        case VK_OEM_6:      // ]}
        case VK_OEM_7:      // '"
        case VK_OEM_8:      // misc
        case VK_OEM_PLUS:   // =+
        case VK_OEM_COMMA:  // ,<
        case VK_OEM_MINUS:  // -_
        case VK_OEM_PERIOD: // .>
        case VK_OEM_102:    // non-US keyboards
            return true;
    }

    return false;
}

// ============================================================================
// Hook
// ============================================================================

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    // Safety check - call original if available, otherwise pass through
    if (!g_originalTranslateMessage) {
        // This should never happen, but don't crash Word
        return TRUE;
    }
    
    if (lpMsg) {
        // Track ALL key presses for quiet period detection
        if (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN) {
            g_lastKeyPressTime = GetTickCount();
        }
        
        // Schedule save only for editing keys
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

    // Validate settings
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
    Wh_Log(L"Word Local AutoSave v1.8 initializing...");

    g_wordProcessId = GetCurrentProcessId();

    LoadSettings();

    // Hook TranslateMessage
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

    Wh_Log(L"Word Local AutoSave initialized successfully");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Word Local AutoSave uninitializing...");

    ResetAllTimers();

    Wh_Log(L"Word Local AutoSave uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    
    // Reset timers before loading new settings
    // This ensures the new saveDelay takes effect immediately
    ResetAllTimers();
    
    LoadSettings();
}
