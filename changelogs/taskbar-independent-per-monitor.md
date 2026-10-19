## 1.6.0 ([Oct 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b0f5bd82a63baa016bb54b8604848c0d287ed9ae/mods/taskbar-independent-per-monitor.wh.cpp))

* Windows 10 support (tested on build 19044 with 4 monitors).
* Each taskbar keeps its own order of buttons, also after a restart. New window buttons are added at the end; buttons moved by the user keep their place.
* Drag a pin to another taskbar to move it there, to the place where it's dropped. A shortcut dropped on a taskbar is pinned where it's dropped.
* New option: open a clicked pin's app on the monitor of that taskbar.
* Fixed: the window-move thread could still run while the mod was unloaded.

## 1.5.0 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/004342fdf226b83a905abffd4a880249e2c47679/mods/taskbar-independent-per-monitor.wh.cpp))

Initial release.
