## 2.1.15 ([Nov 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cb956cc0a64236fa2498a8ea08c095855b986434/mods/floating-taskbar.wh.cpp))

- Pass a valid timer handle to `CreateTimerQueueTimer` during the initial taskbar layout calibration.
- Skip applying the floating taskbar until `TaskbarFrame` is loaded and has an `XamlRoot`.
- Harden Explorer restart initialization against invalid timer output and pre-load taskbar application.

## 2.1.14 ([Nov 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/68650cb524e6e9976dcf9e2cf602fff41c13ec40/mods/floating-taskbar.wh.cpp))

* Add hover-radius support for taskbar and tray controls that expose `CornerRadius` directly.
* Cover SystemTray icon, notification icon, quick-settings, and tray-frame hover hosts.
* Lower the default hover corner radius from 20 to 10 pixels.

## 2.1.13 ([Nov 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3631c69d3cd1f3e77c840781f48155f42210fd12/mods/floating-taskbar.wh.cpp))

Initial release.
