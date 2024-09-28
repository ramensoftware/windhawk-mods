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
