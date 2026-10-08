## 2.1 ([Oct 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f9b3bde0d49096c5c76a161f2e3ca08ec5d7f976/mods/taskbar-vd-switcher.wh.cpp))

* Added grouped settings and one nestable Arrangement expression; old renamed keys reset once
* Added native side-taskbar layouts and literal written arrangements
* Added experimental Windows 10 tray-window placement and compact automatic grids
* The Windows 10 backend waits on registry change notifications instead of polling Explorer, and keeps a hidden bar while its anchor is missing instead of forcing taskbar relayouts
* Hardened rebuild, settings, teardown and retry lifecycle handling
* Added DPI-aware preview fonts and bounded same-process caption reads
* Retained configurable labels/symbols/fonts, native checked states and hover previews

## 1.7 ([Jul 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/64411041fc07359947b0b6a3f85ff5b83361df57/mods/taskbar-vd-switcher.wh.cpp))

* Fixed the active-desktop highlight sticking to the startup desktop: every desktop notification now fully rebuilds the button grid (lightweight-styling resources resolve once at template application, so in-place swaps never took effect). This may also resolve the hover/hit-test report in #4784
* Fixed inactive buttons becoming partially unclickable after switching desktops
* Windows accent color support: color settings accept `accent`, `accentLight`, `accentDark`, and `transparent` in addition to hex; the active desktop now uses the Windows accent color by default (empty = plain native surface)
* Experimental "Show on all taskbars" option: also injects the switcher into secondary monitors' taskbars (tray positions only; requested in a comment on #4785)
* Renamed "Master button" to "Task View button" in settings and readme
* Settings page reordered so color/state options follow a consistent order; expanded readme with screenshot gallery, feature summary, and full settings table

## 1.5 ([Jun 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5e4d697db70f4e4ab70fabb8a82bde7abf198f58/mods/taskbar-vd-switcher.wh.cpp))

Initial release.
