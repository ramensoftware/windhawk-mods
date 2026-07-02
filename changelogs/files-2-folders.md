## 2.5 ([Jul 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/04683525522c0a20295ae6cbe1d6360c1ab1e80c/mods/files-2-folders.wh.cpp))

- Adds an **Operation** segmented control (Move - fast / Move - safe / Copy) to the dialog, above the mode list. Picking one changes the operation for just that run without touching the **Operation** setting.
- Adds two optional aids for building nested subfolder paths in **Fixed name** mode (each with its own settings toggle, on by default):
  - A live breadcrumb preview line under the box (e.g. typing `archive/incoming` shows `archive › incoming`).
  - An **Add level** (`+`) button that appends a `\` separator and refocuses the box.
- Two new settings: `showNestedPreview`, `showAddLevelButton`.

## 2.2 ([Jun 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fd63b735358811aff34f11c4ba5909f1b44688a4/mods/files-2-folders.wh.cpp))

* Add nested-subfolder support to the **Fixed name** mode. A `/` (or `\`) in the name is now treated as a path separator, so `test/1/2/3/name` creates `test\1\2\3\name` and moves the selection into the final `name` folder.
* Intermediate folders are always created/reused as a stable path; only the last segment follows the existing "Fixed name: reuse an existing folder" reuse/numbering rule.
* Empty segments (from leading, trailing, or doubled separators) are ignored. Other modes (by name, by extension, by date) are unchanged.

## 2.1 ([Jun 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b0dd6aa46438ea0f3e7c9130f92772831dcbd22c/mods/files-2-folders.wh.cpp))

- **Copy operation mode.** The old Fast/Slow move toggle becomes a single **Operation** setting with three choices: *Move - fast* (`MoveFileExW`), *Move - safe* (`IFileOperation::MoveItem`, with progress/undo/UAC), and *Copy* (`IFileOperation::CopyItem`, leaving the originals in place). The dialog radio labels read "Copy" vs "Move" to match the selected operation.
- **Context-menu positioning.** A new **Right-click menu position** setting: *Top of menu* (default) or *Between Cut and Copy* — inserted just after the shell Cut/Copy block (matched by command id, so it is locale-independent), falling back to the top of the menu on the modern Windows 11 flyout where Cut/Copy are not real menu items.

## 2.0 ([Jun 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ad19ef246bb802bc953cbd0d171ca2f8eeba204d/mods/files-2-folders.wh.cpp))

* Fixed: the "Files 2 Folder..." menu entry now appears and works on the desktop (previously only inside Explorer folder windows).
* Added Silent mode (right-click menu): skips the dialog and immediately moves the selection using the default mode; suppresses the mod's own dialogs/error popups. The hotkey workaround is unaffected.
* "Default selected mode" is now a dropdown showing the actual mode names instead of a raw 1-4 number box.
* Fixed: the dialog now correctly pre-selects the configured "Default selected mode" (it was always reverting to the first option).
* Also handles selections in library/search views by deriving the destination from the selected items.
* Fixed: no longer inserts a doubled separator when "Place menu item near Cut/Copy" is on and Explorer already has a separator there.
* The menu entry and default subfolder name now use Windows' own localized strings (shell32: "Move to a folder...", "New folder"), so they appear in the OS language.
* Fixed-name / date modes no longer silently reuse an existing folder of that name — they number duplicates like Explorer ("New folder (2)", "(3)", ...).
* The "Files 2 Folder..." entry now also appears in the classic menu bar's Edit dropdown (after Cut/Copy).
* Settings descriptions rewritten for consistency (On/Off states on their own lines, unified quoting/terminology).
* Workaround hotkey: Win-based combos now work via a low-level keyboard hook (the technique PowerToys uses) and suppress the OS default, instead of failing to register. Note: bare Win+<letter> shortcuts (Win+F, Win+L, ...) are reserved by Windows and cannot be intercepted by any hook — pair Win with another modifier (e.g. Win+Shift+F); the mod now warns when a bare Win combo is selected.

## 1.3 ([May 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/edd5a740b5d5c349acaf60598478c0abc5cfe994/mods/files-2-folders.wh.cpp))

* Auto-select a mode's radio button when its companion edit box receives focus (by mouse click or Tab). Focusing the subfolder-name box now selects the fixed-name mode; focusing the date box selects the date-named mode. Previously you could type a name into a box without its radio being checked, so the move would run under whichever mode happened to be selected. This makes the mode you're editing the mode that runs on OK.
tl;dr: Clicking on subfolder name box or date will choose that mod now.

## 1.2 ([May 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f97bd370462cef10e23fea9e699a2debfaf651ec/mods/files-2-folders.wh.cpp))

Initial release.
