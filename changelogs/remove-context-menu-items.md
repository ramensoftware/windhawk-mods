## 1.3 ([Jan 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6429f0bef3712e84409553361f9b661bb2cae674/mods/remove-context-menu-items.wh.cpp))

- Users can now add an asterisk (`*`) at the end of a custom item (e.g., `Open*`) to perform prefix matching. This allows removing multiple variations of a menu item with a single entry.
- Added a `NormalizeString` function to convert various Unicode apostrophe characters (e.g., \`, ‘, ’, ʼ, ´) to a standard straight apostrophe (`'`) for better matching reliability.
- Updated documentation to clarify that the mod currently targets the classic context menu and recommends the "Classic context menu on Windows 11" mod for Win11 users.
- Custom items now use exact matching by default for precision, with optional wildcard support handled via the new `MatchesCustomItem` helper.
- Updated `ShouldRemoveMenuItem` to utilize both the new normalization and ampersand removal simultaneously for more robust string comparisons.
- Improved handling of multi-language strings where special characters or non-standard punctuation previously caused matching to fail.
- Development is underway to implement support for the modern (fluent) Windows 11 context menu.
- Future updates will aim to expand the predefined toggle options to include most languages.

## 1.2 ([Jan 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e666b3f39fd817f40bf16fea5b3558bd4dedac0e/mods/remove-context-menu-items.wh.cpp))

- Improved the custom items engine to use substring matching. This allows users to remove items with dynamic suffixes (like "Move to OneDrive - [Name]") by simply entering the base text.
- Added specific strings to remove "Always keep on this device" and "Free up space" items that appear within OneDrive context submenus.
- Added a prominent warning to the Readme and Settings explaining that predefined toggles are optimized for English Windows.
- Added explicit examples (e.g., "Copy", "Cut", "Sort by") and tips in the Readme to help users understand how to use the Custom Items field for manual removal.
- Updated the GitHub link to point users to the official issues page for reporting spelling variations.
- Added a log message during mod initialization to remind users that predefined options are in English and to use Custom Items for other languages.
- Minor adjustments to the Readme structure and formatting for better readability within the Windhawk interface.

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
