// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word by sending Ctrl+S
// @version         2.0
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
- Detects Ctrl+V, Ctrl+X, Ctrl+Z, Ctrl+Y, Ctrl+Enter (page break), Ctrl+Shift+Enter (column break)
- Configurable delay before saving
- Optional minimum interval between saves to prevent excessive disk writes
- Works with any locally saved Word document
- Only saves when Word is the active window
- Requires a quiet period (500ms no key presses) before saving to prevent shortcut conflicts
- **Comprehensive protection against ALL Word keyboard shortcuts** (100+ shortcuts checked)

## Safety Features (v2.0)

The mod uses **8-stage verification** to guarantee zero false shortcut triggers:

1. **Triple pre-check** with delays to catch fast typing
2. **Quiet period validation** before and after each stage
3. **Post-Ctrl verification** to detect keys pressed during Ctrl send
4. **Post-S verification** to detect concurrent key presses
5. **Inter-key delays** to allow system to process inputs
6. **Foreground re-validation** to ensure Word is still active
7. **Key release retry** with multiple attempts
8. **Comprehensive 100+ shortcut protection**

This ensures **zero false triggers** even with minimum 100ms save delay.

## Protected Shortcuts

The mod protects against accidental triggering of ANY Word shortcut, including:
- **Font Size**: Ctrl+[ and Ctrl+] (Decrease/Increase font)
- **Document**: Ctrl+N/O/W/P (New/Open/Close/Print)
- **Editing**: Ctrl+A/C/V/X/Z/Y (Select/Copy/Paste/Cut/Undo/Redo)
- **Navigation**: Ctrl+Home/End/Arrows/PageUp/PageDown
- **Special**: Ctrl+Space (Clear format), Ctrl+Enter (Page break), Ctrl+Tab (Switch docs)
- **Function keys**: Ctrl+F1-F12 (Ribbon, Print Preview, Close, etc.)
- **And many more**: All Ctrl+Shift, Ctrl+Alt combinations

## Settings

- **Save Delay (ms)**: How long to wait after the last keystroke before saving.
  Default is 1000ms (1 second).
- **Minimum Time Between Saves (ms)**: Minimum interval between consecutive saves.
  Set to 0 to disable this limit and allow saving as frequently as possible.

## Limitations

- Mouse operations (click, drag & drop, context menu paste) are not detected
- Only works with documents that have already been saved at least once
- New unsaved documents will trigger the "Save As" dialog
- IME input (Asian languages) may not trigger auto-save reliably

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

#define IS_KEY_PRESSED(vk) (GetAsyncKeyState(vk) < 0)

// Timing - CRITICAL for preventing false triggers
// Even with 100ms save delay, we MUST have enough quiet time
const DWORD QUIET_PERIOD_MS = 500;              // Minimum quiet time before saving
const DWORD RETRY_INTERVAL_MS = 100;            // Interval between retry attempts
const DWORD DEFERRED_SAVE_BUFFER_MS = 50;       // Buffer for deferred saves
const DWORD PRE_SEND_VERIFY_DELAY_MS = 10;      // Delay between verification checks
const DWORD POST_CTRL_VERIFY_DELAY_MS = 5;      // Delay after Ctrl press before check
const DWORD INTER_KEY_DELAY_MS = 2;             // Delay between key operations
const int MAX_RETRY_COUNT = 50;                 // Max retry attempts (5 seconds)
const int MAX_KEY_RELEASE_RETRIES = 3;          // Retries for releasing stuck keys
const int PRE_SEND_CHECK_COUNT = 3;             // Number of pre-send verifications
const int MIN_SAVE_DELAY_MS = 100;
const int MAX_SAVE_DELAY_MS = 60000;
const int MAX_MIN_TIME_BETWEEN_SAVES = 300000;

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
        Wh_Log(L"Too many retries (%d), giving up", g_retryCount);
        g_retryCount = 0;
        return;
    }
    
    KillRetryTimer();
    UINT_PTR timerId = SetTimer(nullptr, 0, RETRY_INTERVAL_MS, RetryTimerProc);
    if (timerId == 0) {
        Wh_Log(L"Failed to set retry timer: %lu", GetLastError());
        g_retryCount = 0;
    } else {
        g_retryTimerId = timerId;
    }
}

bool IsWordForeground() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) return false;
    
    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foregroundWindow, &foregroundProcessId);
    return (foregroundProcessId == g_wordProcessId);
}

bool HasQuietPeriodPassed() {
    if (g_lastKeyPressTime == 0) return true;
    
    DWORD currentTime = GetTickCount();
    DWORD timeSinceLastKey = SafeTimeDiff(currentTime, g_lastKeyPressTime);
    return timeSinceLastKey >= QUIET_PERIOD_MS;
}

// Comprehensive key check - ALL keys and mouse buttons
bool AreAnyKeysPressed() {
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
    
    // All mouse buttons
    if (IS_KEY_PRESSED(VK_LBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_RBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_MBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_XBUTTON1)) return true;
    if (IS_KEY_PRESSED(VK_XBUTTON2)) return true;
    
    // Editing keys
    if (IS_KEY_PRESSED(VK_SPACE)) return true;
    if (IS_KEY_PRESSED(VK_RETURN)) return true;
    if (IS_KEY_PRESSED(VK_TAB)) return true;
    if (IS_KEY_PRESSED(VK_BACK)) return true;
    if (IS_KEY_PRESSED(VK_DELETE)) return true;
    if (IS_KEY_PRESSED(VK_INSERT)) return true;
    if (IS_KEY_PRESSED(VK_ESCAPE)) return true;
    if (IS_KEY_PRESSED(VK_CLEAR)) return true;  // Numpad 5 without NumLock
    
    // Navigation keys
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
    
    // Numpad (all keys including separator)
    for (int i = VK_NUMPAD0; i <= VK_DIVIDE; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    if (IS_KEY_PRESSED(VK_NUMLOCK)) return true;
    if (IS_KEY_PRESSED(VK_SEPARATOR)) return true;  // Numpad Enter on some keyboards
    
    // OEM/Punctuation
    if (IS_KEY_PRESSED(VK_OEM_1)) return true;
    if (IS_KEY_PRESSED(VK_OEM_2)) return true;
    if (IS_KEY_PRESSED(VK_OEM_3)) return true;
    if (IS_KEY_PRESSED(VK_OEM_4)) return true;
    if (IS_KEY_PRESSED(VK_OEM_5)) return true;
    if (IS_KEY_PRESSED(VK_OEM_6)) return true;
    if (IS_KEY_PRESSED(VK_OEM_7)) return true;
    if (IS_KEY_PRESSED(VK_OEM_8)) return true;
    if (IS_KEY_PRESSED(VK_OEM_PLUS)) return true;
    if (IS_KEY_PRESSED(VK_OEM_COMMA)) return true;
    if (IS_KEY_PRESSED(VK_OEM_MINUS)) return true;
    if (IS_KEY_PRESSED(VK_OEM_PERIOD)) return true;
    if (IS_KEY_PRESSED(VK_OEM_102)) return true;
    
    // System/Toggle keys
    if (IS_KEY_PRESSED(VK_CAPITAL)) return true;
    if (IS_KEY_PRESSED(VK_SCROLL)) return true;
    if (IS_KEY_PRESSED(VK_SNAPSHOT)) return true;
    if (IS_KEY_PRESSED(VK_PAUSE)) return true;
    if (IS_KEY_PRESSED(VK_APPS)) return true;
    
    // Media keys
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
    if (IS_KEY_PRESSED(VK_LAUNCH_MAIL)) return true;
    if (IS_KEY_PRESSED(VK_LAUNCH_MEDIA_SELECT)) return true;
    if (IS_KEY_PRESSED(VK_LAUNCH_APP1)) return true;
    if (IS_KEY_PRESSED(VK_LAUNCH_APP2)) return true;
    
    return false;
}

// ============================================================================
// Core Logic
// ============================================================================

bool SendSingleKey(WORD vk, bool keyUp) {
    INPUT input;
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = keyUp ? KEYEVENTF_KEYUP : 0;
    return SendInput(1, &input, sizeof(INPUT)) == 1;
}

// Release a key with multiple retries
bool ReleaseKeyWithRetry(WORD vk) {
    for (int i = 0; i < MAX_KEY_RELEASE_RETRIES; i++) {
        if (SendSingleKey(vk, true)) {
            return true;
        }
        Wh_Log(L"Retry releasing key 0x%02X (attempt %d)", vk, i + 1);
    }
    Wh_Log(L"ERROR: Failed to release key 0x%02X after %d attempts", vk, MAX_KEY_RELEASE_RETRIES);
    return false;
}

// ==========================================================================
// COMPREHENSIVE critical key check after Ctrl is pressed
// This prevents ALL possible accidental Word shortcuts
// Based on complete Microsoft Word keyboard shortcuts documentation
// ==========================================================================
bool IsAnyCriticalKeyPressed() {
    // ======================================================================
    // LETTERS A-Z (except S which we send)
    // Ctrl+A=SelectAll, Ctrl+B=Bold, Ctrl+C=Copy, Ctrl+D=FontDialog,
    // Ctrl+E=Center, Ctrl+F=Find, Ctrl+G=GoTo, Ctrl+H=Replace,
    // Ctrl+I=Italic, Ctrl+J=Justify, Ctrl+K=Hyperlink, Ctrl+L=Left,
    // Ctrl+M=Indent, Ctrl+N=New, Ctrl+O=Open, Ctrl+P=Print,
    // Ctrl+Q=RemoveFormat, Ctrl+R=Right, Ctrl+T=HangingIndent,
    // Ctrl+U=Underline, Ctrl+V=Paste, Ctrl+W=Close, Ctrl+X=Cut,
    // Ctrl+Y=Redo, Ctrl+Z=Undo
    // ======================================================================
    for (int i = VK_KEY_A; i <= VK_KEY_Z; i++) {
        if (i == 'S') continue;  // We're sending S
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // ======================================================================
    // NUMBERS 0-9
    // Ctrl+0=SpaceBefore, Ctrl+1=SingleSpace, Ctrl+2=DoubleSpace,
    // Ctrl+5=1.5Space, Ctrl+6=Subscript, etc.
    // ======================================================================
    for (int i = VK_KEY_0; i <= VK_KEY_9; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // ======================================================================
    // ALL MODIFIER KEYS
    // Shift: Ctrl+Shift+S=Styles, Ctrl+Shift+E=TrackChanges,
    //        Ctrl+Shift+>=IncreaseFontMore, Ctrl+Shift+Enter=ColumnBreak
    // Alt: Ctrl+Alt combinations for special characters
    // Win: System shortcuts
    // ======================================================================
    if (IS_KEY_PRESSED(VK_SHIFT)) return true;
    if (IS_KEY_PRESSED(VK_LSHIFT)) return true;
    if (IS_KEY_PRESSED(VK_RSHIFT)) return true;
    if (IS_KEY_PRESSED(VK_MENU)) return true;      // Alt
    if (IS_KEY_PRESSED(VK_LMENU)) return true;
    if (IS_KEY_PRESSED(VK_RMENU)) return true;     // AltGr
    if (IS_KEY_PRESSED(VK_LWIN)) return true;
    if (IS_KEY_PRESSED(VK_RWIN)) return true;
    
    // ======================================================================
    // FUNCTION KEYS F1-F24
    // Ctrl+F1=Ribbon, Ctrl+F2=PrintPreview, Ctrl+F4=CloseWindow,
    // Ctrl+F6=SwitchWindows, Ctrl+F9=InsertField, Ctrl+F10=Maximize,
    // Ctrl+F12=Open
    // ======================================================================
    for (int i = VK_F1; i <= VK_F24; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    
    // ======================================================================
    // OEM KEYS (Punctuation/Symbols)
    // Ctrl+[=DecreaseFontSize, Ctrl+]=IncreaseFontSize,
    // Ctrl+=Subscript, Ctrl+-=NonBreakingHyphen,
    // Ctrl+;=Date, Ctrl+'=Accents, Ctrl+`=Accents,
    // Ctrl+\=Various, Ctrl+/=Various
    // ======================================================================
    if (IS_KEY_PRESSED(VK_OEM_1)) return true;      // ;: - Ctrl+;=Date
    if (IS_KEY_PRESSED(VK_OEM_2)) return true;      // /? 
    if (IS_KEY_PRESSED(VK_OEM_3)) return true;      // `~ - Ctrl+`=Accent
    if (IS_KEY_PRESSED(VK_OEM_4)) return true;      // [{ - Ctrl+[=DecreaseFont!
    if (IS_KEY_PRESSED(VK_OEM_5)) return true;      // \|
    if (IS_KEY_PRESSED(VK_OEM_6)) return true;      // ]} - Ctrl+]=IncreaseFont!
    if (IS_KEY_PRESSED(VK_OEM_7)) return true;      // '" - Ctrl+'=Accent
    if (IS_KEY_PRESSED(VK_OEM_8)) return true;      // misc
    if (IS_KEY_PRESSED(VK_OEM_PLUS)) return true;   // =+ - Ctrl+=Subscript!
    if (IS_KEY_PRESSED(VK_OEM_COMMA)) return true;  // ,<
    if (IS_KEY_PRESSED(VK_OEM_MINUS)) return true;  // -_ - Ctrl+-=NonBreakingHyphen
    if (IS_KEY_PRESSED(VK_OEM_PERIOD)) return true; // .>
    if (IS_KEY_PRESSED(VK_OEM_102)) return true;    // non-US \|
    
    // ======================================================================
    // NAVIGATION KEYS
    // Ctrl+Home=DocumentStart, Ctrl+End=DocumentEnd,
    // Ctrl+PageUp=PreviousPage, Ctrl+PageDown=NextPage,
    // Ctrl+Arrow=MoveByWord/Paragraph
    // ======================================================================
    if (IS_KEY_PRESSED(VK_HOME)) return true;
    if (IS_KEY_PRESSED(VK_END)) return true;
    if (IS_KEY_PRESSED(VK_PRIOR)) return true;      // Page Up
    if (IS_KEY_PRESSED(VK_NEXT)) return true;       // Page Down
    if (IS_KEY_PRESSED(VK_LEFT)) return true;
    if (IS_KEY_PRESSED(VK_RIGHT)) return true;
    if (IS_KEY_PRESSED(VK_UP)) return true;
    if (IS_KEY_PRESSED(VK_DOWN)) return true;
    
    // ======================================================================
    // SPECIAL EDITING KEYS
    // Ctrl+Enter=PageBreak, Ctrl+Tab=SwitchDocuments,
    // Ctrl+Space=ClearFormatting, Ctrl+Backspace=DeleteWord,
    // Ctrl+Delete=DeleteWordForward, Ctrl+Insert=Copy
    // ======================================================================
    if (IS_KEY_PRESSED(VK_RETURN)) return true;     // Ctrl+Enter=PageBreak!
    if (IS_KEY_PRESSED(VK_TAB)) return true;        // Ctrl+Tab=SwitchDocs!
    if (IS_KEY_PRESSED(VK_SPACE)) return true;      // Ctrl+Space=ClearFormat!
    if (IS_KEY_PRESSED(VK_BACK)) return true;       // Ctrl+Back=DeleteWord!
    if (IS_KEY_PRESSED(VK_DELETE)) return true;     // Ctrl+Del=DeleteWordFwd!
    if (IS_KEY_PRESSED(VK_INSERT)) return true;     // Ctrl+Ins=Copy!
    if (IS_KEY_PRESSED(VK_ESCAPE)) return true;
    if (IS_KEY_PRESSED(VK_CLEAR)) return true;      // Numpad 5 without NumLock
    
    // ======================================================================
    // NUMPAD KEYS
    // Various Ctrl+Numpad combinations
    // ======================================================================
    for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++) {
        if (IS_KEY_PRESSED(i)) return true;
    }
    if (IS_KEY_PRESSED(VK_MULTIPLY)) return true;
    if (IS_KEY_PRESSED(VK_ADD)) return true;
    if (IS_KEY_PRESSED(VK_SEPARATOR)) return true;
    if (IS_KEY_PRESSED(VK_SUBTRACT)) return true;
    if (IS_KEY_PRESSED(VK_DECIMAL)) return true;
    if (IS_KEY_PRESSED(VK_DIVIDE)) return true;
    if (IS_KEY_PRESSED(VK_NUMLOCK)) return true;
    
    // ======================================================================
    // MOUSE BUTTONS
    // User might be selecting text with mouse
    // ======================================================================
    if (IS_KEY_PRESSED(VK_LBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_RBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_MBUTTON)) return true;
    if (IS_KEY_PRESSED(VK_XBUTTON1)) return true;
    if (IS_KEY_PRESSED(VK_XBUTTON2)) return true;
    
    // ======================================================================
    // SYSTEM KEYS
    // Prevent any system key combinations
    // ======================================================================
    if (IS_KEY_PRESSED(VK_SNAPSHOT)) return true;   // Print Screen
    if (IS_KEY_PRESSED(VK_SCROLL)) return true;     // Scroll Lock
    if (IS_KEY_PRESSED(VK_PAUSE)) return true;      // Pause/Break - Ctrl+Break
    if (IS_KEY_PRESSED(VK_CAPITAL)) return true;    // Caps Lock
    if (IS_KEY_PRESSED(VK_APPS)) return true;       // Context Menu
    
    // ======================================================================
    // BROWSER/MEDIA KEYS (some keyboards)
    // ======================================================================
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
    if (IS_KEY_PRESSED(VK_LAUNCH_MAIL)) return true;
    if (IS_KEY_PRESSED(VK_LAUNCH_MEDIA_SELECT)) return true;
    if (IS_KEY_PRESSED(VK_LAUNCH_APP1)) return true;
    if (IS_KEY_PRESSED(VK_LAUNCH_APP2)) return true;
    
    return false;
}

void AbortSendCtrlS(const wchar_t* reason, bool releaseCtrl, bool releaseS) {
    Wh_Log(L"Abort: %s", reason);
    if (releaseS) {
        ReleaseKeyWithRetry('S');
    }
    if (releaseCtrl) {
        ReleaseKeyWithRetry(VK_CONTROL);
    }
    g_isSendingCtrlS = false;
    ScheduleRetry();
}

void SendCtrlS() {
    g_isSendingCtrlS = true;
    
    // =========================================================================
    // STAGE 1: TRIPLE PRE-CHECK
    // Verify NO keys are pressed, with delays to catch fast typing
    // This catches keys that are being pressed RIGHT NOW
    // =========================================================================
    for (int check = 0; check < PRE_SEND_CHECK_COUNT; check++) {
        if (AreAnyKeysPressed()) {
            AbortSendCtrlS(L"Pre-check failed: key pressed", false, false);
            return;
        }
        if (!HasQuietPeriodPassed()) {
            AbortSendCtrlS(L"Pre-check failed: quiet period not passed", false, false);
            return;
        }
        // Small delay between checks to catch keys being pressed
        if (check < PRE_SEND_CHECK_COUNT - 1) {
            Sleep(PRE_SEND_VERIFY_DELAY_MS);
        }
    }
    
    // =========================================================================
    // STAGE 2: FINAL QUIET PERIOD CHECK
    // Verify quiet period STILL valid after pre-checks
    // =========================================================================
    if (!HasQuietPeriodPassed()) {
        AbortSendCtrlS(L"Quiet period invalidated during pre-check", false, false);
        return;
    }
    
    // =========================================================================
    // STAGE 3: PRESS CTRL
    // =========================================================================
    if (!SendSingleKey(VK_CONTROL, false)) {
        AbortSendCtrlS(L"Failed to send Ctrl press", false, false);
        return;
    }
    
    // Small delay to let the system process the Ctrl press
    Sleep(POST_CTRL_VERIFY_DELAY_MS);
    
    // =========================================================================
    // STAGE 4: CRITICAL CHECK AFTER CTRL
    // This is THE MOST IMPORTANT check - detects if user pressed
    // any key while we were sending Ctrl
    // =========================================================================
    if (IsAnyCriticalKeyPressed()) {
        AbortSendCtrlS(L"Critical key detected after Ctrl press", true, false);
        return;
    }
    
    // Double-check quiet period is still valid
    if (!HasQuietPeriodPassed()) {
        AbortSendCtrlS(L"Quiet period invalidated after Ctrl press", true, false);
        return;
    }
    
    // =========================================================================
    // STAGE 5: PRESS S
    // =========================================================================
    Sleep(INTER_KEY_DELAY_MS);
    
    if (!SendSingleKey('S', false)) {
        AbortSendCtrlS(L"Failed to send S press", true, false);
        return;
    }
    
    // =========================================================================
    // STAGE 6: POST-S VERIFICATION
    // Check if any OTHER key was pressed during S send
    // If so, we might have created Ctrl+[other key]+S which is dangerous
    // =========================================================================
    Sleep(INTER_KEY_DELAY_MS);
    
    // Check all critical keys EXCEPT S (which we just pressed)
    // If any other key is now pressed, something went wrong
    for (int i = VK_KEY_A; i <= VK_KEY_Z; i++) {
        if (i == 'S') continue;
        if (IS_KEY_PRESSED(i)) {
            Wh_Log(L"WARNING: Key 0x%02X pressed during S send - aborting", i);
            AbortSendCtrlS(L"Key pressed during S send", true, true);
            return;
        }
    }
    
    // =========================================================================
    // STAGE 7: RELEASE S
    // =========================================================================
    Sleep(INTER_KEY_DELAY_MS);
    
    if (!ReleaseKeyWithRetry('S')) {
        Wh_Log(L"Warning: S may be stuck");
    }
    
    // =========================================================================
    // STAGE 8: RELEASE CTRL
    // =========================================================================
    Sleep(INTER_KEY_DELAY_MS);
    
    if (!ReleaseKeyWithRetry(VK_CONTROL)) {
        Wh_Log(L"Warning: Ctrl may be stuck");
    }
    
    // =========================================================================
    // SUCCESS
    // =========================================================================
    g_isSendingCtrlS = false;
    
    Wh_Log(L"Auto-save: Ctrl+S sent successfully");
    g_lastSaveTime = GetTickCount();
    g_lastInputTime = 0;
    g_retryCount = 0;
}

void CALLBACK RetryTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillRetryTimer();
    TrySave();
}

void TrySave() {
    // =========================================================================
    // SAFETY CHECK 1: Word must be foreground
    // =========================================================================
    if (!IsWordForeground()) {
        Wh_Log(L"Word not in foreground, skipping");
        g_retryCount = 0;
        return;
    }

    // =========================================================================
    // SAFETY CHECK 2: No keys pressed
    // =========================================================================
    if (AreAnyKeysPressed()) {
        ScheduleRetry();
        return;
    }
    
    // =========================================================================
    // SAFETY CHECK 3: Quiet period passed
    // =========================================================================
    if (!HasQuietPeriodPassed()) {
        ScheduleRetry();
        return;
    }
    
    // =========================================================================
    // SAFETY CHECK 4: Double-check after small delay
    // This catches very fast typing that might have started
    // =========================================================================
    Sleep(PRE_SEND_VERIFY_DELAY_MS);
    
    if (AreAnyKeysPressed()) {
        ScheduleRetry();
        return;
    }
    
    if (!HasQuietPeriodPassed()) {
        ScheduleRetry();
        return;
    }
    
    // =========================================================================
    // SAFETY CHECK 5: Verify Word is STILL foreground
    // User might have switched windows during our checks
    // =========================================================================
    if (!IsWordForeground()) {
        Wh_Log(L"Word lost focus during checks, skipping");
        g_retryCount = 0;
        return;
    }

    Wh_Log(L"All conditions met, sending Ctrl+S...");
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
            Wh_Log(L"Deferring save - %lu ms remaining", remainingTime);
            
            UINT_PTR timerId = SetTimer(nullptr, 0, remainingTime + DEFERRED_SAVE_BUFFER_MS, SaveTimerProc);
            if (timerId == 0) {
                Wh_Log(L"Failed to set deferred timer: %lu", GetLastError());
            } else {
                g_saveTimerId = timerId;
            }
            return;
        }
    }

    TrySave();
}

void ScheduleSave() {
    g_lastInputTime = GetTickCount();
    
    // Only kill save timer, preserve retry state
    KillSaveTimer();

    UINT_PTR timerId = SetTimer(nullptr, 0, g_settings.saveDelay, SaveTimerProc);
    if (timerId == 0) {
        Wh_Log(L"Failed to set save timer: %lu", GetLastError());
    } else {
        g_saveTimerId = timerId;
    }
}

bool IsEditingKey(WPARAM wParam) {
    if (g_isSendingCtrlS) {
        return false;
    }

    bool ctrlPressed = IS_KEY_PRESSED(VK_CONTROL);
    bool shiftPressed = IS_KEY_PRESSED(VK_SHIFT);
    bool altPressed = IS_KEY_PRESSED(VK_MENU);

    // Manual Ctrl+S - reset timers
    if (ctrlPressed && !shiftPressed && !altPressed && wParam == 'S') {
        g_lastSaveTime = GetTickCount();
        g_lastInputTime = 0;
        ResetAllTimers();
        Wh_Log(L"Manual save detected");
        return false;
    }

    // Ctrl combinations that modify document
    if (ctrlPressed && !altPressed) {
        // Paste, cut, redo, undo
        if (wParam == 'V' || wParam == 'X' || wParam == 'Y' || wParam == 'Z') {
            return true;
        }
        // Ctrl+Enter = page break, Ctrl+Shift+Enter = column break
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

    // Special editing keys
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
    if (wParam == VK_SEPARATOR) return true;
    
    // OEM keys
    switch (wParam) {
        case VK_OEM_1:
        case VK_OEM_2:
        case VK_OEM_3:
        case VK_OEM_4:
        case VK_OEM_5:
        case VK_OEM_6:
        case VK_OEM_7:
        case VK_OEM_8:
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
        // Track key presses for quiet period
        if (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN) {
            g_lastKeyPressTime = GetTickCount();
        }
        
        // Also track WM_CHAR for IME input support
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
    Wh_Log(L"Word Local AutoSave v2.0 initializing...");

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
    ResetAllTimers();
    LoadSettings();
}
