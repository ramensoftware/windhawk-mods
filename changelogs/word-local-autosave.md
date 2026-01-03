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
