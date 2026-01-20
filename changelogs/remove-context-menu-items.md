## 1.1 ([Jan 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9fa744387ad0d7a93c4d3804349fc37c177b8ae9/mods/remove-context-menu-items.wh.cpp))

- Added a **Custom Items** setting allowing users to manually enter specific menu item names for removal.
- Added a feature to automatically clean up duplicate or consecutive separator lines in the context menu.
- Added a mechanism to automatically remove trailing separators.
- Expanded the list of removable items to include Windows Media Player options, system actions like "Refresh" and "Copy as path," and image tools like "Rotate left/right."
- Added support for filtering common actions like "Open," "Edit," and "Play."
- The mod now ignores ampersands (`&`) in menu text to ensure items are caught regardless of keyboard shortcuts.
- Added logic to protect "Open with," "Open in Terminal," and "Edit with" from being accidentally removed.
- Added a hook for `TrackPopupMenu` to improve compatibility and implemented a safety limit for loading custom items.
- Added logging to track when custom items are successfully matched and removed.

## 1.0 ([Jan 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a4314662a227e624ff2784b29a48c7c99618a29c/mods/remove-context-menu-items.wh.cpp))

Initial release.
