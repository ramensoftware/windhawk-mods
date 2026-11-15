## 1.1.0 ([Nov 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8f97906d3b28a9424b1558a18768c1c739d8f4fa/mods/task-manager-pause-button.wh.cpp))

* Keeps Pause active when switching between Processes and Details, and resumes Task Manager updates when opening other pages.
* Restores direct support for opening Task Manager on Details, using the available navigation symbols while avoiding duplicate hooks for shared function addresses.
* Adds `Frame.Navigated` handling as a fallback for page detection and refresh-state changes.
* Improves command-bar detection and retries button insertion when the toolbar is not ready.
* Keeps the Pause/Resume button usable with its text label if the native icon cannot be created.
* Improves cleanup of button and navigation event handlers when disabling the mod.

## 1.0.0 ([Nov 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b8a64f42f5edc941234424f16894123290e733b4/mods/task-manager-pause-button.wh.cpp))

Initial release.
