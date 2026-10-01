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
