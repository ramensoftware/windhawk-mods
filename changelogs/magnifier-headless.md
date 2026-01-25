## 0.9.5.4 ([Jan 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a0e7724e823c3e7f5c2643133115e7d966753652/mods/magnifier-headless.wh.cpp))

This change ensures that the Magnifier touch overlay window is properly hidden from Alt+Tab switcher and Task Manager by enforcing the `WS_EX_TOOLWINDOW` extended window style. Also, a screenshot of the mod has been added to Windhawk Mod Readme.

## 0.9.5.2 ([Jan 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1f5a98f4619528de6c38e30571fcd85a6c0e8bbe/mods/magnifier-headless.wh.cpp))

**New Features:**

- Add thread-safe implementation and race condition fixes.
- Add comprehensive API hooks for complete window visibility control.
- Add window procedure interception for comprehensive message handling.
- Add comprehensive performance optimizations.
- Add comprehensive error handling and robustness improvements.
- fix compilation errors by replacing SEH with portable error handling.
- Eliminate mouse freeze and add ultra-fast cleanup.
- Block touch controls and document mouse freeze fix.
- Block Magnifier Touch window by title.
- Make Magnifier Touch overlay transparent instead of hiding.
- Hide touch overlay using off-screen position + 0x0 size.
- The appropriate version number has been assigned.

**Bug Fixes:**
- Hide GDI+ helper windows.
- Hide CSPNotify and MSCTFIME helper windows.
- Correct CSpNotify case sensitivity.
- Revert broad window class patterns that broke zoom.

## 0.7.0 ([Sep 22, 2025](https://github.com/ramensoftware/windhawk-mods/blob/4b4007eafe60bd25a02d29acac4dd03eca7aaba5/mods/magnifier-headless.wh.cpp))

Initial release.
