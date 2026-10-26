## 1.6 ([Oct 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3f4ace0fe0c3f6f9c48882fe50b4b2e351e4f1e7/mods/cursor-trail.wh.cpp))

* Added the "Always on top" setting, enabled by default. The trail now draws above the taskbar, Start menu and other shell surfaces. Explorer hosts a layered, click-through window in the UIAccess Z-order band (falling back to the system-tools band), and the trail is drawn into it; if that is not possible, the trail falls back to its own overlay window.
* Turning the setting off unloads the `explorer.exe` side and keeps the trail in the normal desktop band.

## 1.2 ([Oct 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/11fb8bda590c23f2db86e393c419992247a817eb/mods/cursor-trail.wh.cpp))

Initial release.
