## 2.0 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/af95847c8bb8beaf8f6c118ab2d15f53c7099db0/mods/adaptive-microphone-icon-visibility.wh.cpp))

* Detect microphone use via audio sessions, since the registry data is no longer updated.
* Whitelist entries are now executable names (e.g. `obs64.exe`), including for Store apps.
* Show the icon if monitoring fails to start.
* Fixed loading on builds where the tray lives in `ExplorerExtensions.dll`.
* Improved unloading and logging.

## 1.0 ([Jun 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0ba128210751d519899c9c5564c47a9aaf8460cb/mods/adaptive-microphone-icon-visibility.wh.cpp))

Initial release.
