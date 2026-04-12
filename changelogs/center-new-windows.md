## 1.0.1 ([Apr 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a8b6141258658b1fbae949d1902f4ae76e55c0fc/mods/center-new-windows.wh.cpp))

* SetWindowPos grace period — after a window is first centred, a 2-second grace period blocks any SetWindowPos call that tries to move it elsewhere. This fixes a known issue with Firefox-based browsers (Firefox, Floorp, Zen, LibreWolf and others) that restore their saved session position via SetWindowPos ~700 ms after ShowWindow, overriding the centred position.
* Vertical offset setting — shifts the centred position up or down by a fixed number of pixels. Useful for third-party docks or status bars that are not reflected in the system work area.
* DPI-unaware app fix — windows and dialogs that belong to processes using system scaling (e.g. Audacity, older Win32 apps) are now correctly centered at all scale levels. The previous calculation mixed physical monitor coordinates with virtual window coordinates, placing windows above and to the left of true center.

## 1.0.0 ([Mar 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7d0b212e3305a746d709da156f06012b3713acef/mods/center-new-windows.wh.cpp))

Initial release.
