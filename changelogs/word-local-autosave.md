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
