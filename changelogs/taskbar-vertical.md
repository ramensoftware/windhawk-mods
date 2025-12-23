## 1.3.9 ([Dec 23, 2025](https://github.com/ramensoftware/windhawk-mods/blob/8d55be3283a54c65186cc221e3502adb27413d04/mods/taskbar-vertical.wh.cpp))

* Fixed gap between the taskbar and the Start menu when the taskbar is on the right.
* Improved Start menu handling, especially when opened on a different monitor.

## 1.3.8 ([Dec 19, 2025](https://github.com/ramensoftware/windhawk-mods/blob/bfba1b55ec10ecd3f7b78009533838598191141d/mods/taskbar-vertical.wh.cpp))

* Fixed drag and drop for tray icons.
* Fixed secondary taskbar clock rotation on startup.
* Fixed badge orientation for taskbar icons (contribution by Lubomir Stanko).
* Fixed notification center being shown on the primary monitor when clicking on the secondary monitor clock.
* Fixed labels vertical alignment when using small icons.
* Fixed the start menu sliding over the taskbar with the "Adjust Start menu animation" option.
* Improved start menu handling with multiple monitors.
* Fixed broken share picker UI layout.

## 1.3.7 ([Oct 25, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7f2f2a00bede1e5e55dc49331996d18ed88547cd/mods/taskbar-vertical.wh.cpp))

* Made the start menu animation adjustment optional and off by default, since it's incompatible with the Phone Link sidebar and with some Start Menu Styler themes.

## 1.3.6 ([Oct 25, 2025](https://github.com/ramensoftware/windhawk-mods/blob/150cfc38691f0f7f36e548fd8bc6e41e5c74dcf0/mods/taskbar-vertical.wh.cpp))

* Reworked Start menu positioning:
  * The [redesigned Windows 11 Start menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/) is now supported.
  * The "Start menu width" option has been removed, as it's no longer necessary.
  * The Start menu now animates from the taskbar side using a technique discovered by [SandTechStuff](https://github.com/SandTechStuff) (not available for the redesigned Start menu).
  * The account card at the bottom of the Start menu is no longer truncated.
* Improved support for the new battery percentage option.
* Settings now apply to tray elements automatically. Previously, the mod had to be reloaded in some cases.

## 1.3.5 ([Jul 16, 2025](https://github.com/ramensoftware/windhawk-mods/blob/97c72fa2c85a13393590bdf73464db7c6d19e8ae/mods/taskbar-vertical.wh.cpp))

* Fixed misplaced thumbnail previews after a recent Windows 11 update.
* Thumbnail previews in list mode are now aligned to bottom. List mode is used when there are too many items, or when the "Disable Taskbar Thumbnails" mod is used in list mode.
* Fixed taskbar elements being unclickable on the monitor edges sometimes.
* Fixed [the redesigned start menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/) being cut off. Currently it opens in the original location. Better support is planned for follow-up updates.
* Improved notifications placement when the taskbar is on the left. The mod tried to keep notifications on the left in this case, but they sometimes still opened on the right.
* Improved compatibility with status bar programs such as YASB and Zebar.

## 1.3.4 ([Mar 21, 2025](https://github.com/ramensoftware/windhawk-mods/blob/787a8c303d89449e75abd3f2a4677f26b7d16b0a/mods/taskbar-vertical.wh.cpp))

* Added an option to customize the start menu vertical alignment.
* Added an option to customize the clock container height.
* Fixed an incompatibility with recent Windows 11 preview builds.
* Fixed the new colored battery icon not being rotated.
* Fixed a bug which could cause the taskbar to become hidden on system startup.

## 1.3.3 ([Feb 8, 2025](https://github.com/ramensoftware/windhawk-mods/blob/cf9709ca613ae552c8f1d4a3ed9a029f916a9344/mods/taskbar-vertical.wh.cpp))

* Added support for the new thumbnail previews implementation in Windows.
* Improved start menu positioning.
* Fixed crashes in recent Windows 11 builds when the taskbar overflow is used.
* Fixed restoring jump list animation when the mod is disabled.

## 1.3.2 ([Jan 29, 2025](https://github.com/ramensoftware/windhawk-mods/blob/2d855bfc1ee92c213a681996b36cb7997fb1594d/mods/taskbar-vertical.wh.cpp))

* Show the running indicator on the right instead of the left when the taskbar is on the right.

## 1.3.1 ([Jan 29, 2025](https://github.com/ramensoftware/windhawk-mods/blob/b4dba0e9ddf0dbcc0b0668b079cf7144ec54c298/mods/taskbar-vertical.wh.cpp))

* Fixed the start menu width option not applying until restart in some cases.
* Fixed context menus not appearing with monitors that are arranged vertically, one above the other.
* Fixed compatibility with recent preview builds of Windows 11.

## 1.3 ([Oct 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b43bd5d00887601ed297b2366631494080f0dd3e/mods/taskbar-vertical.wh.cpp))

* Fixed jump lists being cut off when opened near the edge of the screen.
* Added a vertical alignment option for jump lists.
* Adjusted jump list animations to slide from the correct direction.
* Fixed tooltips overlapping with the taskbar in Windows 11 version 24H2.
* Improved support for some devices, mostly tablets and devices with a touchscreen.
* Improved compatibility with some programs which query the taskbar location, such as EarTrumpet.
* This update requires Windhawk 1.5 or newer. If you're using an older Windhawk version, please update.

## 1.2.5 ([Oct 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/31cb0bda3c0d239f220fad072bd4a2be53c6ff05/mods/taskbar-vertical.wh.cpp))

* Improved tray overflow position, now it's vertically centered to the mouse position and isn't shown far away from the taskbar.
* Fixed right click menu location for some tray icons.
* Fixed jump list width on high DPIs.

## 1.2.4 ([Sep 30, 2024](https://github.com/ramensoftware/windhawk-mods/blob/056d7c496f105eecebcf9527d06841b5a923e384/mods/taskbar-vertical.wh.cpp))

* Fixed a single pixel of the background not aligning with maximized windows.
* Fixed notifications being moved to the top of the screen.

## 1.2.3 ([Sep 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3065efada7b6b5dd413a165287b76560ff0c0210/mods/taskbar-vertical.wh.cpp))

* Fixed rotated badges (e.g. unread counters) for some programs.
* Fixed the edge of the taskbar being non-clickable in some cases.
* Fixed the taskbar not showing up at startup in some cases.
* Fixed the tray icons flyout arrow being reversed when the taskbar is on the right.
* Fixed taskbar overflow flyout opening on the left of the monitor when the taskbar is on the right.

## 1.2.2 ([Sep 20, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a90c4ab676b14803b36ae376b63928dc6b483b51/mods/taskbar-vertical.wh.cpp))

* Fixed a possible crash loop on mod initialization with multiple monitors.
* Fixed the taskbar becoming blank after changing the settings.
* Fixed all taskbar items hidden in an overflow view after enabling the mod.
* Improved handling of the right click menu on the top of the taskbar, reducing the chance of the menu being cut off.

## 1.2.1 ([Sep 20, 2024](https://github.com/ramensoftware/windhawk-mods/blob/824532afd6f3568f26b5676f950ef0557602df99/mods/taskbar-vertical.wh.cpp))

* Improved core implementation, which improves compatibility with other mods and programs.
* Fixed overflow tray icons cut off when the taskbar is on the right.
* Fixed copilot opening behind the taskbar when it's on the right.

## 1.2 ([Sep 19, 2024](https://github.com/ramensoftware/windhawk-mods/blob/0b018deaa9febe57c9ed85c1ad65aac49966d743/mods/taskbar-vertical.wh.cpp))

* Added support for Windows 11 version 24H2.
* Added an option to move the taskbar to the right.
* Added an option for a custom start menu width, can be useful for custom start menu themes.
* If taskbar labels are enabled, they're no longer rotated sideways.
* The Copilot icon and widget icons are no longer rotated sideways.
* Fixed thumbnails being cut off after closing an item.

## 1.1 ([Aug 31, 2024](https://github.com/ramensoftware/windhawk-mods/blob/387771ec375561f49fffc1429b8b185f54feb5e4/mods/taskbar-vertical.wh.cpp))

* Fixed the start menu being covered by the taskbar, adjusted its position to
  always open near the start button.
* Adjusted the position of the notification flyout and quick settings flyout to
  open near the tray area and not on the opposite side of the screen.
* The clock is no longer rotated sideways.
* Some (hopefully all) system tray icons are no longer rotated sideways.
* The default taskbar width was increased to account for the rotated clock
  width.
* Fixed some cases of disappearing taskbar icons.
* Fixed the position of the Win+X menu (start button right click menu).
* Other small fixes and improvements.
* Fixed the start button, search button and tasks button becoming larger when
  the taskbar width is increased.

## 1.0 ([Jun 12, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3fe4bb2e2b2b6ea124b622c91bfee9350b7ab2c7/mods/taskbar-vertical.wh.cpp))

Initial release.
