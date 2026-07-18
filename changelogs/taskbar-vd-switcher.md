## 1.7 ([Jul 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/64411041fc07359947b0b6a3f85ff5b83361df57/mods/taskbar-vd-switcher.wh.cpp))

* Fixed the active-desktop highlight sticking to the startup desktop: every desktop notification now fully rebuilds the button grid (lightweight-styling resources resolve once at template application, so in-place swaps never took effect). This may also resolve the hover/hit-test report in #4784
* Fixed inactive buttons becoming partially unclickable after switching desktops
* Windows accent color support: color settings accept `accent`, `accentLight`, `accentDark`, and `transparent` in addition to hex; the active desktop now uses the Windows accent color by default (empty = plain native surface)
* Experimental "Show on all taskbars" option: also injects the switcher into secondary monitors' taskbars (tray positions only; requested in a comment on #4785)
* Renamed "Master button" to "Task View button" in settings and readme
* Settings page reordered so color/state options follow a consistent order; expanded readme with screenshot gallery, feature summary, and full settings table

## 1.5 ([Jun 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5e4d697db70f4e4ab70fabb8a82bde7abf198f58/mods/taskbar-vd-switcher.wh.cpp))

Initial release.
