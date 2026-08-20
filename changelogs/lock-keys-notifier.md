## 1.2.0 ([Aug 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1bd07309c86154278e3a2df58acc948fcc9a8cba/mods/lock-keys-notifier.wh.cpp))

* Add sound trigger condition and per-state WAVs
* Give Insert key its own sound toggle
* Move the keyboard hook off the render thread onto a dedicated thread
* Make the worker thread DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2

## 1.1.0 ([Jun 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/00056305e81658b3ca6dda4c76f391593f3a17c1/mods/lock-keys-notifier.wh.cpp))

* NEW: show notifications even if an elevated application is currently focused via a poll-timer (toggleable)

## 1.0.1 ([Jun 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/be4c114ae4a7801eabd67c9cbf851c0754a1b8c0/mods/lock-keys-notifier.wh.cpp))

* Pre-warm GDI+ text pipeline at startup to fix cold-boot first-toast delay

## 1.0.0 ([Jun 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0d25f36c6bece76b384de92bf08e6c7bc2e10061/mods/lock-keys-notifier.wh.cpp))

Initial release.
