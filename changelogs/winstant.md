## 1.6.1 ([Nov 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3f02b6d1cf2da9000ca24027f44145eda5eab1ea/mods/winstant.wh.cpp))

- **Ctrl+Backspace** now deletes the previous word in the panel's search box.
  Windows sends Ctrl+Backspace as the control character `0x7F` (DEL), which
  the panel didn't filter out: it was added to the search text and shown as a
  little box.
- The same now works in the text box used to rename a window (F2) and to
  name a saved layout. It's a standard EDIT control, which inserts `0x7F` on
  Ctrl+Backspace too; the subclass now handles it.

## 1.6 ([Nov 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/dc8ad91cc70df48de6f92ab75023be43feba1721/mods/winstant.wh.cpp))

-there’s a new setting, “Open the panel by hovering the minimize button” (Panel group). Turn it off and the panel opens only with the hotkey or key taps. The mouse wheel on the minimize button keeps its own option.

## 1.5 ([Oct 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/83d9b317497843f42cc0dbc229e0aab85b412a2a/mods/winstant.wh.cpp))

Initial release.
