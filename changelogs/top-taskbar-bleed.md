## 1.1.0 ([Oct 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/115a3db9089965b3a37c711a88b166ef3fc27321/mods/top-taskbar-bleed.wh.cpp))

### Fixed
* After an Explorer restart the bleed didn't appear until the taskbar was hovered or a window clicked. The mod loads before the taskbar exists, and the `Taskbar.View.dll` layout hooks fire while the taskbar is still being built (band and XAML host not ready), so the early attach attempts failed and nothing re-triggered them. The tracker now retries the direct `taskbar.dll` attach while any taskbar window of the process lacks an island (1 s, then 10 s after the first minute) — which also covers a secondary taskbar that wasn't ready when the primary attached, and a taskbar rebuilt while another kept its island.
* Switching the Windows mode (light/dark) while a panel was showing left the taskbar in the old mode after the panel hid. The root's `RequestedTheme` is now watched: the taskbar's own write becomes the value to restore, the forced theme is re-asserted 500 ms later instead of synchronously (so the taskbar computes its background under the real mode), and on hide an explicit original is only trusted while the Windows mode is unchanged — otherwise the current mode is restored. Covers windows that stay dark and windows that follow the system theme, on primary and secondary taskbars.

### Added
* Experimental **Any taskbar position** option (off by default): with it on, taskbars docked at the bottom, left or right are painted as well. The color still comes from the maximized window's title bar, so a bottom taskbar takes the title bar's color. Vertical taskbars always get the flat color, since the panel rotates with the frame and a gradient would run along the bar. The strip's taskbar guard now applies only when the sampled rows actually overlap the taskbar, which a bottom or side taskbar never does.

### Changed
* Wording: the mod description, README, settings and comments say "title bar" rather than "top edge".

### Notes
* Version 1.1.0 (a new option on top of the fixes). Tested on Windows 11 26H2 (26300.9711), two monitors, Taskbar Styler enabled, Windows mode toggled both ways with dark-only and theme-following maximized windows; the any-position option tested with the taskbar at the bottom and, with vertical taskbar, at the left and right

## 1.0.0 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/734714f248424422007d996d861940948d4eb2e5/mods/top-taskbar-bleed.wh.cpp))

Initial release.
