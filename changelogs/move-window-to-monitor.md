## 1.1.1 ([Nov 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8e9155da316053d27d39d343a54e2b0bd12be00a/mods/move-window-to-monitor.wh.cpp))

* Fixed maximized windows ending up on the old monitor or flickering when moved
* Borderless full-screen windows now cover the destination monitor instead of being scaled to its work area
* If a configured target monitor no longer exists (e.g. unplugged, wrong index), the hotkey now falls back to the automatic direction-based move
(Thanks to DavidHiFi for the report and the proposed fix)

## 1.1.0 ([May 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f0ef9faeba5aac55e931e6603bf30066d2e4a7f3/mods/move-window-to-monitor.wh.cpp))

Initial release.
