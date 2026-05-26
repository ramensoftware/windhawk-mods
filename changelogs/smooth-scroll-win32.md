## 1.0.3 ([May 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8a4b44b30b8e194e4e7d7e4651ef8a21fecc57db/mods/smooth-scroll-win32.wh.cpp))

- **Consistent scroll speed across folder sizes** — the UIA percent-per-line
  estimate now uses a multi-range curve (quadratic for view sizes 25–70,
  power-5 for 70–92, power-20 for 92–100) that keeps scroll distance
  consistent regardless of how many items a folder contains.
  Credit: **@JTFA** (github.com/JTFA).
- **Smooth-only mode** — a new setting disables spring physics entirely.
  Scrolling uses linear easing like a touchscreen: no bounce, no overshoot,
  just a clean deceleration. Tune the speed with **Smooth Speed**.
- **Improved spring physics** — progressive damping near the target
  eliminates micro-oscillation at rest without affecting the feel of
  the main animation.
- **Control Panel scroll no longer blocked** — legacy `DirectUIHWND` containers
  (Control Panel) are now correctly identified via the root window's WndProc
  module and passed through to the native scroll handler. Detection is stable
  from the first scroll, eliminating the intermittent first-open failure.
- **Overshoot** — new setting that underdamps the spring for a subtle
  mobile-like bounce. The progressive near-target damping still prevents
  endless oscillation. No effect in Smooth-only mode.
- **Automatic refresh rate** — Animation Interval and V-Sync settings removed.
  The timer interval is now detected per window from the actual monitor's
  refresh rate and updates automatically when the window moves to another
  display.

## 1.0.2 ([Apr 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/466acb75de958bc766e59c85b639c56a759a99a4/mods/smooth-scroll-win32.wh.cpp))

- Fixed scrolling on SysListView32 horizontal-only views (Icon, Table, Tile)
- Fixed scrolling not working on Task Manager WinUI tabs
- Fixed possible overscroll past top/bottom boundaries
- Fixed inconsistent item scrolling in folders with different item counts
- Added V-Sync option to match animation interval to display refresh rate
- Fixed Ctrl+scroll not working when changing view modes
- Smoother spring effect for a more natural scrolling experience

## 1.0.1 ([Apr 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/22be6a2051b29dfeb12a0ff4ebf3cd47c0caa41b/mods/smooth-scroll-win32.wh.cpp))

Fixed horizontal scrolling on listview mode

## 1.0.0 ([Mar 31, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6d6c2dd24ba263b22d599c4890233c2ae2f6a262/mods/smooth-scroll-win32.wh.cpp))

Initial release.
