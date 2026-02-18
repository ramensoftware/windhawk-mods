## 1.0 ([Feb 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/90157decb7b9647ae233e9e2994a95c8ad417bb3/mods/magnifier-headless.wh.cpp))

refactor: Remove over-engineering and fix Windhawk compliance issues

- Remove 7 unnecessary API hooks (`AnimateWindow`, `BringWindowToTop`,
  `SetForegroundWindow`, `UpdateLayeredWindow`, `SetLayeredWindowAttributes`,
  `SetWindowRgn`, `DwmSetWindowAttribute`) — the 4 core hooks
  (`CreateWindowExW`, `ShowWindow`, `SetWindowPos`, `SetWindowLongPtrW`) fully
  cover all window visibility paths for Magnify.exe
- Remove `CallWndProc` hook + window subclassing infrastructure (150+ lines)
  that duplicated what the API hooks already handle
- Remove LRU cache (16-entry tick-based eviction for a process with ~5
  windows)
- Remove Safe wrapper functions with retry mechanisms (`SafeSetParent Sleep+retry`, `SafeSetWindowLongPtrW`, `SafeGetClassName`, `SafeIsWindow`)
- Remove `AutoCriticalSection` class and CRITICAL_SECTION — no longer needed without subclassing or cache
- Remove `CSpNotify` and `MSCTFIME` from `IsMagnifierWindow` detection — these are system-wide accessibility/IME windows, hiding them breaks accessibility features in an accessibility tool
- Remove GDI+ helper window detection — not Magnifier-specific
- Remove MSVC-only #pragma comment(lib) — Windhawk uses Clang/mingw-w64
- Remove redundant #include directives (`windhawk_api.h` is auto-included)
- Remove `@compilerOptions -lcomctl32` (only needed for removed subclassing)
- Fix `EnumWindows` to scope to current process only via `GetCurrentProcessId`
- Rewrite `WindhawkModReadme` section to be user-focused instead of implementation docs
- Bump version to 1.0

fix: Hide Magnifier Touch window from Alt+Tab
The touch overlay was still appearing in Alt+Tab because `ShowWindow_Hook` was letting show commands pass through for touch overlay windows.
- `ShowWindow_Hook` blocks show commands for touch overlays (same as magnifier)
- `SetWindowPos_Hook` adds `SWP_HIDEWINDOW` flag for touch overlays
- `CreateWindowExW_Hook` calls `ShowWindow` (`SW_HIDE`) after creating touch overlay
- `CreateWindowExW_Hook` strips `WS_VISIBLE` from touch overlay before creation

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
