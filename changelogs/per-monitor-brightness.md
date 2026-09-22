## 2.1 ([Sep 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/325e75600e4ed1367e8d118969097e64fbc3ef72/mods/per-monitor-brightness.wh.cpp))

- **Configurable Slider Placement**: Added an option to place brightness sliders inside the Quick Settings card (above or below native Windows sliders) instead of below the footer, matching native flyout dimensions, margins, and icon alignment.
- **Hide Unsupported Displays**: Added an optional setting to hide monitors that do not support brightness adjustment (DDC/CI).
- **Fixed Phantom Percentage**: Unsupported monitors no longer display a placeholder "50%" text above the "Brightness control not supported" status.
- **Improved Layout Cleanup**: Refined visual tree handling to ensure smooth enable/disable transitions with zero leftover gaps or layout shifts.

## 2.0 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1e9a228c451f2d5ceabd8b412c07fa726a4d583b/mods/per-monitor-brightness.wh.cpp))

Initial release.
