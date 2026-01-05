## 1.2.4 ([Jan 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e1771115d4c8ce364c0413eb79e6cd268b98000c/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Excluded Task Switcher view (Win+Tab), now the taskbar is always visible when Task Switcher is open.
* Added support for the new taskbar auto-hide animation in Windows 11 preview builds (ViVeTool ID: 41356296).

## 1.2.3 ([May 9, 2025](https://github.com/ramensoftware/windhawk-mods/blob/f8c259486b46a48a4fd3d8ddc0dee2ee98a4b4e1/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* The mod no longer permanently changes the system auto-hide settings.
* Improved compatibility with status bar programs such as YASB and Zebar.
* Fixed a possible crash on Windows startup (only observed in Windows 10).

## 1.2.2 ([Mar 22, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c2a94e3b3f0465445cf6d789a48b2cd561fb8242/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Hide the taskbar for fullscreen windows for the "Auto-hide only when a window is maximized" mode.
* Fixed taskbar thumbnails causing the taskbar to hide and then show up again right away (the fix in v1.2.1 wasn't complete).

## 1.2.1 ([Mar 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/2a980f110766e76fe0095140c9010f8933f3918d/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Fixed taskbar thumbnails causing the taskbar to hide and then show up again right away.
* Made the "Never auto-hide" mode work with the "Primary monitor only" option.
* Improved performance and fixed the taskbar becoming stuck in some cases.

## 1.2 ([Feb 19, 2025](https://github.com/ramensoftware/windhawk-mods/blob/be73d3a4f2e28c78c621a1bb6f3de212d01c9514/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* It's no longer necessary to enable auto-hide in the taskbar properties for the mod to work, the mod does that automatically.
* Added an option to apply the auto-hide logic only to foreground window.
* Added an option to exclude programs from the auto-hide logic, can be useful for programs which act as taskbar widgets or for buggy programs.
* Added an option to apply the mod only to the primary monitor taskbar.
* The "Auto-hide only when a window is maximized" mode was improved to hide the taskbar for fullscreen windows and snapped windows as well.

## 1.1.2 ([Nov 9, 2024](https://github.com/ramensoftware/windhawk-mods/blob/699e3b8e67452ab1f163ddc7bd9376549364e4db/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Fixed the mod not applying on startup in some cases.
* ExplorerPatcher taskbar: Added support for secondary taskbars for versions newer than 67.1.

## 1.1.1 ([Nov 3, 2024](https://github.com/ramensoftware/windhawk-mods/blob/1819258d37a06f57310f9ab3c9f6538d808317f4/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Fixed spanned windows (e.g. Win+right) affecting the sibling monitor taskbar.

## 1.1 ([Nov 3, 2024](https://github.com/ramensoftware/windhawk-mods/blob/350f82b7f5076ccb9a1070a8da3f149cfb900af9/mods/taskbar-auto-hide-when-maximized.wh.cpp))

* Most code was rewritten, and the mod works in a slightly different way now - instead of switching auto-hide on and off, it's always left on, but the mod prevents the taskbar from hiding in certain conditions. That has benefits like a smoother transition between the states.
* The taskbar now auto-hides when a window intersects it. This can be disabled in the options.
* Auto-hiding can be suppressed altogether in the settings. In this case, maximized windows will be covered by the taskbar. This might be desirable if the taskbar is mostly transparent.

## 1.0 ([Oct 25, 2024](https://github.com/ramensoftware/windhawk-mods/blob/df4eb62db86a415c8ee319727521a15a36216661/mods/taskbar-auto-hide-when-maximized.wh.cpp))

Initial release.
