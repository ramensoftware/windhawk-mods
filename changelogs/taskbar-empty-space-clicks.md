## 2.2 ([Dec 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7833da2a65bf74c639d33f1432664d552056e2de/mods/taskbar-empty-space-clicks.wh.cpp))

- Added `Troubleshooting` chapter to address recent issues with incorrect trigger settings after update from v1 to v2
- Added info about reserving empty space on the taskbar - resolves https://github.com/m1lhaus/windhawk-mods/issues/47

## 2.1 ([Nov 20, 2025](https://github.com/ramensoftware/windhawk-mods/blob/077c9d5ae5e80ce1c341354c4dc628516f2b5201/mods/taskbar-empty-space-clicks.wh.cpp))

- implemented "Eager trigger evaluation" option - resolves https://github.com/m1lhaus/windhawk-mods/issues/40
- Toggle Volume Mute feature now mutes all devices, not just the default one - resolves https://github.com/m1lhaus/windhawk-mods/issues/33

## 2.0 ([Nov 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/532a7d6e3c03d67efbd5d889b367b8185caf7fc1/mods/taskbar-empty-space-clicks.wh.cpp))

- concept of triggers completely reworked (incompatible with v1.x settings)
- user can add up to 50 triggers
- added keyboard modifiers for each trigger - resolves https://github.com/m1lhaus/windhawk-mods/issues/22 and resolves https://github.com/m1lhaus/windhawk-mods/issues/1
- added single left/right button click triggers - resolves https://github.com/m1lhaus/windhawk-mods/issues/27 and resolves https://github.com/m1lhaus/windhawk-mods/issues/24
- smart handling of taskbar context menu on right-click triggers
- media keys not working bug fixed - fixes https://github.com/m1lhaus/windhawk-mods/issues/30

## 1.9 ([Dec 15, 2024](https://github.com/ramensoftware/windhawk-mods/blob/c18f329132507d6ed1a7517e5c1906d194e2bb34/mods/taskbar-empty-space-clicks.wh.cpp))

Fixed explorer.exe freeze on Windows 24H2 when the combine taskbar buttons option changed - https://github.com/m1lhaus/windhawk-mods/issues/23.

## 1.8 ([Dec 2, 2024](https://github.com/ramensoftware/windhawk-mods/blob/10dd219bec0369013eeeb9a3c0bee7efc3987f41/mods/taskbar-empty-space-clicks.wh.cpp))

Added fallback method to open Start menu when Start button was not found on the taskbar (e.g. was hidden via other Windhawk mod) - temporary hotfix for https://github.com/m1lhaus/windhawk-mods/issues/21.

## 1.7 ([Nov 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/2e30970216e691be9103eb6693f557e63eb39e45/mods/taskbar-empty-space-clicks.wh.cpp))

- Start menu now opens on the same monitor where the taskbar was clicked - fixes https://github.com/m1lhaus/windhawk-mods/issues/19
- Show Desktop feature fixes for Explorer Patcher taskbar - fixes https://github.com/m1lhaus/windhawk-mods/issues/10

## 1.6 ([Nov 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/5e8a177134d706049012b8451736c782c753b1c0/mods/taskbar-empty-space-clicks.wh.cpp))

Double tap on touch screens is now much more benevolent in terms of click position on the screen. It takes into account screen DPI since in hi-res screens (like my 14'' 4k laptop) the position diffs between clicks when performing double tap can be significant. Also, since middle mouse button click is not possible with touch and detecting touch gesture is quite cumbersome (to work reliably across both Windows 10 and Windows 11 taskbars), I implemented support for triple tap which activates middle mouse click action. Fixes https://github.com/m1lhaus/windhawk-mods/issues/11

## 1.5 ([Oct 16, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7f0e516d77c2e3f4f8ce32e6d2eee126d5456af6/mods/taskbar-empty-space-clicks.wh.cpp))

* Fixed execution of task manager on Windows 11 24H2

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
