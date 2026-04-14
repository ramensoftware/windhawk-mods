## 1.1.0 ([Apr 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/849442c1f7f0e050615851eedfe35ffbfac0a4b0/mods/auto-custom-titlebar-colors.wh.cpp))

Fixed a major issue with third-party apps like GlazeWM, Flow Launcher etc

Issues fixed:
- GlazeWM - Chromium-based browsers and apps simply don't play well with running glazewm and having this mod enabled as they aggressively grab the focus and not giving the room to shift workspaces. Also, this mod seems to have caused a regression with it where when toggling so theme from dark to light and vice versa, the workspaces acts weird everytime i.e it randomly shifts workspace focus. 

- Flow Launcher - Closes instantly on an input event. Simple fix by adding `Flow.Launcher.exe` to `@exclude`

## 1.0.0 ([Apr 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/03eb122ec8fd09c77818492144ff779102f0aec7/mods/auto-custom-titlebar-colors.wh.cpp))

Initial release.
