## 1.1.1 ([Nov 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7e4ed06f199782d1e3c512e259ba5866b8590fb6/mods/task-manager-pause-button.wh.cpp))

- The previous version relied on Task Manager’s Ctrl-key pause, which only worked while the window had focus. Clicking the taskbar or choosing **Open file location** could make the process list start updating again while **Pause** was still active.
- **Pause** now controls list updates directly, so the list stays frozen even when Task Manager loses focus.
- Thanks to @F404X for reporting the **Open file location** case in [issue #1](https://github.com/luizbizzio/task-manager-pause-button/issues/1).

## 1.1.0 ([Nov 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8f97906d3b28a9424b1558a18768c1c739d8f4fa/mods/task-manager-pause-button.wh.cpp))

* Keeps Pause active when switching between Processes and Details, and resumes Task Manager updates when opening other pages.
* Restores direct support for opening Task Manager on Details, using the available navigation symbols while avoiding duplicate hooks for shared function addresses.
* Adds `Frame.Navigated` handling as a fallback for page detection and refresh-state changes.
* Improves command-bar detection and retries button insertion when the toolbar is not ready.
* Keeps the Pause/Resume button usable with its text label if the native icon cannot be created.
* Improves cleanup of button and navigation event handlers when disabling the mod.

## 1.0.0 ([Nov 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b8a64f42f5edc941234424f16894123290e733b4/mods/task-manager-pause-button.wh.cpp))

Initial release.
