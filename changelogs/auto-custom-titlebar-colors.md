## 1.2.0 ([Aug 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8ca7c059ad7d5e8d10a9f0ce54cdd6cff1725d9a/mods/auto-custom-titlebar-colors.wh.cpp))

- Added a new method to detect and ignore UWP apps by default
- Added classes to ignore browser windows (conflict with Zen Browser which is a special case)
- Added a toggle setting to exclude Zen related browser windows

**Note:** Custom colours are now enabled by default in v1.2.0 with default dark mode colors updated to `#000000` (active) and `#202020` (inactive). Make sure to update your colors if not using default colors

## 1.1.2 ([Apr 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/69a1563ab829620af049efd27a911655aee6195d/mods/auto-custom-titlebar-colors.wh.cpp))

- Remove Flow Launcher from exclusion list as it doesn't cause the issue anymore with the previous fixes

## 1.1.1 ([Apr 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/61ba5925653bc3f666213d1aef8d12a5f811bcd8/mods/auto-custom-titlebar-colors.wh.cpp))

Issue Fixed:

- Input focus loss for every new window. Root cause was `SetWindowPos` with `SWP_FRAMECHANGED` flag in `CreateWindowEx` hooks was stealing focus from newly created windows.

## 1.1.0 ([Apr 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/849442c1f7f0e050615851eedfe35ffbfac0a4b0/mods/auto-custom-titlebar-colors.wh.cpp))

Fixed a major issue with third-party apps like GlazeWM, Flow Launcher etc

Issues fixed:
- GlazeWM - Chromium-based browsers and apps simply don't play well with running glazewm and having this mod enabled as they aggressively grab the focus and not giving the room to shift workspaces. Also, this mod seems to have caused a regression with it where when toggling so theme from dark to light and vice versa, the workspaces acts weird everytime i.e it randomly shifts workspace focus. 

- Flow Launcher - Closes instantly on an input event. Simple fix by adding `Flow.Launcher.exe` to `@exclude`

## 1.0.0 ([Apr 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/03eb122ec8fd09c77818492144ff779102f0aec7/mods/auto-custom-titlebar-colors.wh.cpp))

Initial release.
