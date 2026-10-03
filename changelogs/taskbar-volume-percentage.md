## 1.8.1 ([Oct 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7046e9ed96c91e9ce073fb74d94fac13b9c4895a/mods/taskbar-volume-percentage.wh.cpp))

- feat: add independent fontSize, prefixSize, and iconSize settings with native 16px defaults
- feat: support customFontFamily with native DirectWrite fallback
- feat: measure container width proactively on initialization to prevent initial truncation
- feat: add official Ko-fi donation link and Support section in documentation
- fix: skip secondary Explorer COM factory processes to eliminate launch failures
- fix: save and restore native icon FontSize and MinWidth to ensure clean reversibility
- fix: preserve Taskbar Styler font inheritance and decouple volume text from iconSize
- fix: ensure symmetrical layer reset and clean property unbinding on unload

## 1.7.4 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/33a028cd441387b8983f5e79142c09c6e476ae93/mods/taskbar-volume-percentage.wh.cpp))

- Add native icon and text dual display options for mute state ("Native mute icon and text" and "Native mute icon and 0%")
- Render text-only display styles in a dedicated text block using the same font as other tray items, fixing vertical misalignment
- Make the volume text color dynamically follow the taskbar theme
- Fix the original icon layout being lost after changing settings, which could hide the volume icon when the mod was disabled
- Manage icon-to-text spacing via native Grid column spacing, preserving custom margins set by Windows 11 Taskbar Styler
- Fix the icon spacing setting being ignored when using custom or automatic values
- Dynamically detect and mirror custom font sizes applied to the tray text block by Taskbar Styler themes

## 1.6.3 ([Sep 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d716eb9c201b51aaa77af303bfd64272e4ea2ab5/mods/taskbar-volume-percentage.wh.cpp))

Initial release.
