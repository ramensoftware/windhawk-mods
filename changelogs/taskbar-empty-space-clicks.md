## 1.4 ([Sep 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/24a59b390771c24436afc2ef3b64c145f8547082/mods/taskbar-empty-space-clicks.wh.cpp))

* Fixed build errors introduced with latest version of Windhawk 1.5x

* Use CoInitializeEx with COINIT_APARTMENTTHREADED (fixes issue #12: Can't Restart Explorer While this Mod is Enabled)

## 1.3 ([Feb 7, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7cf96eb29f7166f2587624eac5241b4eeb4a0016/mods/taskbar-empty-space-clicks.wh.cpp))

Features:
- added option to control "Combine Taskbar buttons" feature on secondary taskbars
- added feature to send custom key press
- added feature to run arbitrary command/process

**Dev:**
- implemented file logger in order to capture log after fresh Windows start (and debug hook) or to capture crashes
- implemented trace logger to log method entry/exit to debug execution order  
- implemented logging levels to make stuff really verbose if needed

## 1.2 ([Jan 14, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a61b96c8ecb6a86ce4c3af434ab3bade823b67a2/mods/taskbar-empty-space-clicks.wh.cpp))

* Fixed issue when mod was not hooked after Windows startup.

## 1.1 ([Jan 14, 2024](https://github.com/ramensoftware/windhawk-mods/blob/cc8981928018153636f20dfdef7ac8dacf5571e5/mods/taskbar-empty-space-clicks.wh.cpp))

- code reformatted by clang formatter - Visual Studio style (it makes the diff a pain in the ass I know and I am sorry for that - I will stick to this format from now on)
- removed 7+TT code used for Ctrl+Alt+Tab and Win+Tab (replaced by virtual keypress)
- implemented Open Start menu feature (sends Win keypress)
- implemented "Combine taskbar butons toggle" (works only on Windows 10 and newer Windows 11 versions) - closes #463
- prevent multiple double-clicks when tripple-clicking

## 1.0 ([Jan 1, 2024](https://github.com/ramensoftware/windhawk-mods/blob/46472b5d13df20650591a0456fd464daab9552a2/mods/taskbar-empty-space-clicks.wh.cpp))

Initial release.
