## 1.8 ([Feb 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/95b4e5915b916a947210f4b89c88e15674d81f25/mods/word-local-autosave.wh.cpp))

- Version bumped to 1.8
- **Fixed dead code**: Ctrl+Shift+Z check was unreachable - now Ctrl+Z handles both undo and redo (with/without Shift)
- **Fixed data loss**: `minTimeBetweenSaves` skip no longer loses changes - now calculates remaining time and schedules deferred save
- **Fixed crash risk**: Hook now returns TRUE instead of FALSE when `g_originalTranslateMessage` is null
- **Fixed race condition**: `Wh_ModSettingsChanged()` now calls `ResetAllTimers()` before loading new settings
- **Removed unused variable**: `g_initialized` was declared but never used
- **Added navigation keys check**: Home, End, Page Up (VK_PRIOR), Page Down (VK_NEXT)
- **Removed redundant checks**: Eliminated double `AreAnyKeysPressed()` calls in TrySave → SendCtrlS flow
- **Added upper limit for `minTimeBetweenSaves`**: MAX_MIN_TIME_BETWEEN_SAVES = 300000 (5 minutes)
- **Changed `ScheduleRetry()` return type**: Now returns void instead of ignored bool
- **Fixed `Wh_ModUninit()`**: Now properly resets `g_retryCount` via `ResetAllTimers()`
- **Added `IS_KEY_PRESSED()` macro**: Replaced 40+ occurrences of `0x8000` magic number
- **Added `ResetAllTimers()` helper**: Unified function for killing all timers and resetting state
- **Increased quiet period**: 400ms → 500ms for better safety margin
- **Implemented atomic Ctrl+S sending**: Safety checks between each keystroke to prevent race conditions
- **Added critical key check after Ctrl press**:
  - All letters A-Z (prevents Ctrl+B/I/U/E/L/R/J/A/N/O/P/W, etc.)
  - All numbers 0-9 (prevents Ctrl+1/2/5 line spacing)
  - Shift key (prevents Ctrl+Shift+S Apply Styles)
  - Alt key (prevents Ctrl+Alt combinations)
  - Bracket keys [ ] (prevents Ctrl+[/] font size change)
  - Function keys F1-F12
- **Added `SendSingleKey()` helper**: Granular keystroke control
- **Added `ReleaseCtrl()` cleanup**: Properly releases Ctrl on abort
- **Expanded `AreAnyKeysPressed()` with 85+ keys**:
  - Left/Right Shift, Ctrl, Alt variants
  - Caps Lock, Scroll Lock, Num Lock
  - Print Screen, Pause/Break, Context Menu
  - F13-F24 extended function keys
  - All media keys (Volume, Play/Pause, etc.)
  - All browser keys (Back, Forward, Refresh, etc.)

## 1.7 ([Jan 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2e20ba2ff622362ea8165a3ec4852aeffdfaf51f/mods/word-local-autosave.wh.cpp))

- Fixed accidental text alignment changes (Ctrl+L/E/R/J) caused by race condition
- Increased quiet period from 250ms to 400ms for better shortcut conflict prevention
- Added double key-state verification with 20ms delay before sending Ctrl+S
- Added final quiet period check right before sending keystrokes
- Fixed Ctrl key not being checked in AreAnyKeysPressed() - could cause Ctrl+S to overlap other Ctrl shortcuts
- Added Windows key (Win) detection to prevent Win+S (Windows Search) conflicts
- Added numpad number keys (0-9) and operators to trigger auto-save
- Fixed potential timer leak in SendCtrlS() retry logic
- Replaced range-based for loop with explicit checks for better compiler compatibility
- Version bumped to 1.7

## 1.6 ([Jan 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/910f99823709334f1f4d6a886d50f323d2b87f39/mods/word-local-autosave.wh.cpp))

## Add support for punctuation keys

### Problem

Typing punctuation marks (period, comma, brackets, quotes, etc.) alone without any other characters would not trigger auto-save. While it's unclear who might need to save a document containing nothing but a lonely period contemplating its existence, it should still work.

### Root Cause

The `IsEditingKey()` function only checked for:
- ASCII printable characters in range 0x20-0x7E
- Special keys (Backspace, Delete, Enter, Tab)

However, punctuation keys on most keyboards are OEM keys with virtual key codes outside the ASCII range:
- `VK_OEM_PERIOD` (0xBE) - period/full stop
- `VK_OEM_COMMA` (0xBC) - comma
- And other punctuation keys (0xBA-0xDF range)

### Solution

Added explicit handling for all OEM punctuation keys in `IsEditingKey()`.

### Changes

- **Added OEM key detection in `IsEditingKey()`** — now recognizes all standard punctuation keys:
  - `VK_OEM_1` (;:)
  - `VK_OEM_2` (/?)
  - `VK_OEM_3` (`` `~ ``)
  - `VK_OEM_4` ([{)
  - `VK_OEM_5` (\\|)
  - `VK_OEM_6` (]})
  - `VK_OEM_7` ('")
  - `VK_OEM_8` (misc)
  - `VK_OEM_PLUS` (=+)
  - `VK_OEM_COMMA` (,<)
  - `VK_OEM_MINUS` (-_)
  - `VK_OEM_PERIOD` (.>)
  - `VK_OEM_102` (extra key on non-US keyboards)
- **Updated readme** — added "punctuation" to the list of detected inputs
- Version bumped to 1.6

## 1.5 ([Jan 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/df9a07b7e0359a0c3074b69cd0be6353df6a09c8/mods/word-local-autosave.wh.cpp))

## Fix race condition with low save delay values

### Problem

Users with very low save delay settings (e.g., 100ms) still experienced random shortcut triggers (Ctrl+K, Ctrl+H, etc.). The 100ms quiet period was not sufficient because:
- Fast typing produces keystrokes every 50-100ms
- With save delay = 100ms and quiet period = 100ms, saves would trigger during active typing
- A key could be pressed in the microseconds between the final check and SendInput call

### Solution

1. Increased quiet period from 100ms to 250ms (longer than typical fast typing interval)
2. Added a final safety check immediately before SendInput

### Changes

- **Increased `QUIET_PERIOD_MS` from 100 to 250** — ensures save only triggers during actual pauses in typing, not between keystrokes
- **Added final safety check in `SendCtrlS()`** — checks `AreAnyKeysPressed()` one more time right before calling `SendInput()`, aborts and reschedules if any key is detected
- **Added forward declaration for `RetryTimerProc`** — needed because `SendCtrlS()` now references it
- Version bumped to 1.5

## 1.4 ([Jan 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fb96bf6b44b3dbabe393a42bcd5464ce15688618/mods/word-local-autosave.wh.cpp))

## Fix race condition causing random shortcuts to trigger

### Problem

Despite checking if keys were pressed before sending Ctrl+S, random Word shortcuts would still trigger:
- Ctrl+H (Find and Replace)
- Ctrl+F (Navigation)
- Ctrl+Shift+S (Apply Styles)

This happened because of a **race condition**: between the moment the mod checked keyboard state and the moment it sent the Ctrl key, the user could press a new key. Word would then receive Ctrl+[that key] instead of Ctrl+S.

### Root Cause

`GetKeyboardState()` returns keyboard state from the message queue, not the actual physical state. The check and send operations were not atomic, allowing new keypresses to slip in between.

### Solution

Implemented a **quiet period** mechanism: the mod now tracks the timestamp of every keypress and only saves after 100ms of complete keyboard inactivity.

### Changes

- **Added `g_lastKeyPressTime`** — tracks the timestamp of every key press (including non-editing keys)
- **Added `QUIET_PERIOD_MS` constant** — 100ms minimum quiet time before saving
- **Added `HasQuietPeriodPassed()` function** — checks if enough time has passed since last keypress
- **Track ALL keypresses** — now monitors `WM_KEYDOWN` and `WM_SYSKEYDOWN` for quiet period, not just editing keys
- **Switched to `GetAsyncKeyState()`** — checks physical key state in real-time instead of message queue state
- **Dual safety check** — save only triggers when BOTH conditions are met:
  1. No keys physically pressed (`AreAnyKeysPressed() == false`)
  2. 100ms passed since last keypress (`HasQuietPeriodPassed() == true`)
- Version bumped to 1.4

## 1.3 ([Jan 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/66bf7d5b8752a2b92bf976b8ee2c779386a758eb/mods/word-local-autosave.wh.cpp))

Problem
When typing quickly, the mod would send Ctrl key at the exact moment user was pressing another letter (like K or F). Word interpreted this as Ctrl+K (Insert Hyperlink) or Ctrl+F (Navigation) instead of Ctrl+S (Save).
Root Cause
Version 1.2 only checked if Shift and Alt were pressed. It did not check if regular letter keys were being held down.
Solution
Added AreAnyKeysPressed() function that uses GetKeyboardState() to check if any key is currently pressed before sending Ctrl+S.
Changes

New function AreAnyKeysPressed() — checks all keyboard keys, not just modifiers
Checks letters A-Z (virtual keys 0x41-0x5A)
Checks numbers 0-9 (virtual keys 0x30-0x39)
Checks Shift/Alt (both left and right variants)
Checks common keys — Space, Enter, Tab, Backspace, Delete
Checks numpad keys — NumPad0-9, operators
Checks OEM keys — punctuation, brackets, comma, period, etc.
Increased retry limit — from 20 to 50 attempts (5 seconds max wait)
Better logging — logs which specific key is blocking the save
Version bumped to 1.3

Behavior
The mod now waits until the keyboard is completely idle before sending Ctrl+S. This eliminates all shortcut conflicts.

## 1.2 ([Jan 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3c7c9ac8e40174aaff1620d925bdceaaea56f1d4/mods/word-local-autosave.wh.cpp))

Fix Shift key interference during typing

Problem:
Version 1.1 forcibly released the Shift key before sending Ctrl+S. This caused issues when typing:
- Uppercase letters would randomly become lowercase
- Comma (Shift+period on some layouts) would become a period
- Any Shift+key combination could be interrupted mid-keystroke

Solution:
Instead of forcing modifier keys to release, the mod now **waits** for the user to naturally release Shift/Alt before triggering auto-save.

### Changes

- **Removed forced key release** — no longer sends synthetic Shift/Alt key-up events
- **Added retry mechanism** — if Shift or Alt is pressed when save triggers, the mod waits and retries
- **Retry polling** — checks every 100ms whether modifiers are released (up to 20 attempts = 2 seconds)
- **Graceful timeout** — if modifiers are held for more than 2 seconds, skip this save (will save on next edit)
- Added `g_retryTimerId` and `g_retryCount` for retry state management
- Added `AreModifiersPressed()` helper function
- Added `TrySave()` function that handles retry logic
- Version bumped to 1.2

## 1.1 ([Jan 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f9ec9f9313b098032de9129f9bb7e96059794fdc/mods/word-local-autosave.wh.cpp))

Fix Apply Styles dialog opening during auto-save

Problem:
When typing uppercase letters (holding Shift) and auto-save triggers at the same time, Word receives Ctrl+Shift+S instead of Ctrl+S. This opens the "Apply Styles" dialog, interrupting the user's workflow.

Solution:
Before sending Ctrl+S, the mod now checks if Shift or Alt keys are currently pressed. If so, it sends key-up events for these modifiers first, ensuring Word receives a clean Ctrl+S command.

Changes:
1) Added detection for pressed Shift and Alt keys before auto-save
2) Send key-up events for modifier keys before Ctrl+S to prevent unwanted shortcuts
3) Version bumped to 1.1

## 1.0 ([Jan 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e3e6960a8a2ef064cc7f3391486c7607413ed2b6/mods/word-local-autosave.wh.cpp))

Initial release.
