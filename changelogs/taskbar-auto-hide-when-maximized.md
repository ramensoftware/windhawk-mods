## 1.1.1 ([Nov 3, 2024](https://github.com/ramensoftware/windhawk-mods/blob/1819258d37a06f57310f9ab3c9f6538d808317f4/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Fixed spanned windows (e.g. Win+right) affecting the sibling monitor taskbar.

## 1.1 ([Nov 3, 2024](https://github.com/ramensoftware/windhawk-mods/blob/350f82b7f5076ccb9a1070a8da3f149cfb900af9/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Most code was rewritten, and the mod works in a slightly different way now - instead of switching auto-hide on and off, it's always left on, but the mod prevents the taskbar from hiding in certain conditions. That has benefits like a smoother transition between the states.
* The taskbar now auto-hides when a window intersects it. This can be disabled in the options.
* Auto-hiding can be suppressed altogether in the settings. In this case, maximized windows will be covered by the taskbar. This might be desirable if the taskbar is mostly transparent.

## 1.0 ([Oct 25, 2024](https://github.com/ramensoftware/windhawk-mods/blob/df4eb62db86a415c8ee319727521a15a36216661/mods/taskbar-auto-hide-when-maximized.wh.cpp))

Initial release.
