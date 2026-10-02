## 1.5.0 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/efe709cf5a54c20c661b4322a8367d278a95f9f1/mods/explorer-custom-shortcuts.wh.cpp))

* Added `internal:showProperties` command to open native properties dialogs for selected items (capped at 15 windows to prevent Explorer hangs).
* Added `internal:openCmd` and `internal:openCmdAdmin` commands to launch Command Prompt with `pushd` directory resolution.
* Added `internal:createShortcut` command to generate `.lnk` files using the `IShellLink` interface with native collision renaming.
* Added `internal:packIntoFolder` command to move selected items into a new group folder with native `Ctrl+Z` undo support.
* Added `internal:bulkDuplicate` command to duplicate selected items in-place.
* Added `internal:openTerminal` and `internal:openTerminalAdmin` commands with automatic PowerShell fallback.
* Added `internal:openWithNotepad` command to open all selected files simultaneously in Notepad.
* Added `internal:toggleCheckboxes` command to toggle the native Windows item checkboxes setting globally.
* Rebuilt the toast notification engine with distinct visual states (**Info**, **Success**, **Error**) and colorblind-friendly error indicators.
* Added customizable screen positioning (9 anchor points) and display duration settings for notifications.
* Implemented multi-monitor dynamic anchoring and per-monitor DPI scaling for toast notifications.
* Added the `%d_name` token to extract only the active tab's folder name.
* Added `Ctrl+Alt+Enter` mapped to `internal:showProperties` in the default settings configuration.
* Improved notification fade-out animations by calculating dynamic timing instead of using hardcoded sleep loops.

## 1.4.0 ([Sep 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/df7220d83c639d849596a50f9d074f8576bcce74/mods/explorer-custom-shortcuts.wh.cpp))

- **Added Action Toast Notifications**: Displays a brief floating OSD notification upon completing actions like copying paths/names or emptying the Recycle Bin. Automatically adapts to Windows light/dark mode, active system accent colors, and per-monitor DPI scaling.
- **Added Notification Toggle Setting**: Added the `showActionToasts` option in mod settings to enable or disable toast notifications.
- **Improved `internal:copyPath`**: Automatically falls back to copying the active folder path when no items are selected.
- **Aligned Recycle Bin Confirmation**: Switched confirmation dialog default focus to OK (`MB_DEFBUTTON1`) to align with native Windows Explorer deletion behavior.
- **Robust Mod Lifecycle & Safety**: Runs notifications directly on background worker threads to avoid unjoined threads during mod unloading, properly scopes GDI+ measurement objects, and ensures valid hook verification during initialization.
- **Enhanced Logging**: Added structured diagnostic logging for shortcut matching, command execution, and internal operations.

## 1.3.0 ([Sep 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7146805e48b73f481404dd1d80808c88f0fc7dc0/mods/explorer-custom-shortcuts.wh.cpp))

* Added `internal:openParentFolder` command to navigate the active Explorer tab to its parent folder.
* Added `internal:copyPath` command to copy absolute path(s) of selected items without quotes.
* Added `internal:copyName` command to copy filename(s) with extension without quotes.
* Added `internal:openWith` command to trigger the native Windows "Open with" dialog.
* Expanded key parser (`ParseKey`) to support `Backspace`, `Insert`, `Home`, `End`, `PageUp`, and `PageDown`.
* Updated README documentation and settings schema for the new commands and supported keys.

## 1.0 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3c1bddd5acf554972350bb9186c823c6a65126fb/mods/explorer-custom-shortcuts.wh.cpp))

Initial release.
