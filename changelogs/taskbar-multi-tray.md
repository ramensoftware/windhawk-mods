## 1.3.0 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a7cd9de73db60e1d68de5db6008eae71c09eed91/mods/taskbar-multi-tray.wh.cpp))

### Bug fixes & improvements
* Graphical issues shouldn't be present anymore, including :
  * Empty space(s) on some monitors
  * Light flickering on tray icons right-click
  * Hidden tray icon menu spawning with the wrong position
* Right click menus should appear on the correct monitor
* Right-clicking a non-hidden tray icon on another monitor no longer requires an extra click (& doesn't refresh the chevron menu)
* Move icons anywhere ! No need to move them back to the primary screen's area to drop them, works cross-screen too !
* As a consequence, the hidden tray flyout no longer teleports when there's a line added/removed
* Some apps with custom right-click menus (like Raycast) no longer appear on the wrong screen, although they might have positioning/scaling issues. This is out of scope for this mod & requires the app itself to fix this.
* Better logs

### Issues remaining
* Support for the new taskbar layouts in Windows 11 (left/right/top) has NOT been tested, tho nice if it works
* Extra items like emoji/language selector aren't cloned YET (progress is underway)
* As always compatibility with other mods that change the taskbar is NOT guaranteed

## 1.2.2 ([Jun 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/dc839cd068c2485f5339538d4c6a64fe809e4282/mods/taskbar-multi-tray.wh.cpp))

Initial release.
