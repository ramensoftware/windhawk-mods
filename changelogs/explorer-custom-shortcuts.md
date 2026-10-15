## 1.7.0 ([Oct 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a62f085f428edfe603914cee262a1088ea22da03/mods/explorer-custom-shortcuts.wh.cpp))

* Added `%base` and `%basename` tokens to extract filenames without extensions.
* Added `internal:openWithCode` (selection-aware with 10-folder/50-file caps) and `internal:openDirInCode` for VS Code integration.
* Added support for shorthand `int:` prefix alongside `internal:`.
* Added full-word parameter aliases (`%name`, `%dir`, `%count`, `%parent`) and literal percent escape (`%%`).
* Added on-screen error notifications for invalid internal commands.
* Consolidated internal command documentation with a single tip callout.
* Updated settings descriptions and streamlined documentation sections.

## 1.6.0 ([Oct 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b60ad5890be359ed15f9b610f1649afba7bb4e05/mods/explorer-custom-shortcuts.wh.cpp))

* **Feature (Internal Commands)**: Added `internal:newSmartFile` to create a blank, extensionless file that enters inline rename directly for quick creation of code and config files (`.js`, `.py`, `.json`, `.cpp`, `.md`, etc.).
* **Feature (Internal Commands)**: Added `internal:touchFile` to update the `Last Modified` timestamp of selected files or folders to the current time.
* **Feature (Internal Commands)**: Added `internal:copyUnixPath` to copy path(s) using forward slashes (`/`) for Git, Python, and terminal environments.
* **Feature (Internal Commands)**: Added `internal:copyFolderPath` to explicitly copy the current folder path directly, ignoring any active item selections.
* **Improvement (Toasts)**: Registered the toast notification window class once in `Wh_ModInit` with the mod's own module handle and clean it up in `Wh_ModUninit`.
* **Improvement (Settings & Docs)**: Clarified the `toasts.position` setting description to specify anchor placement relative to the active Explorer window.
* **Improvement (Documentation)**: Reorganized all internal commands in `ExecuteInternalCommand` to mirror the README documentation tables 1:1, and added a preview screenshot of action toasts.

## 1.5.1 ([Oct 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4e4b6b8a851ea95542ae6d1cd07efb05ff07ab07/mods/explorer-custom-shortcuts.wh.cpp))

* **Fix (Keyboard Shortcuts):** Resolved an issue where invoking shortcuts with the `Alt` key (e.g., `Alt+C`) caused Explorer to get stuck in its menu/keytip accelerator loop, blocking standalone keys like `Delete` and `Backspace`.
* **Improvement (Settings):** Cleaned up notification settings parsing with proper bounds clamping and safe default fallbacks.

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
