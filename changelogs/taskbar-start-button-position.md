## 1.2.4 ([Oct 25, 2025](https://github.com/ramensoftware/windhawk-mods/blob/862ba36c9284acfd3cd617faffe37aea501db928/mods/taskbar-start-button-position.wh.cpp))

* Reworked Start menu positioning:
  * The [redesigned Windows 11 Start menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/) is now supported.
  * The "Start menu width" option has been removed, as it's no longer necessary.
* Fixed missing Start button padding on the left in some cases. For details, see [this comment](https://github.com/ramensoftware/windhawk-mods/issues/1818#issuecomment-3446863789).
* Fixed the mod not working on some Windows 11 builds which have one of the hooked functions inlined.

## 1.2.3 ([Jul 23, 2025](https://github.com/ramensoftware/windhawk-mods/blob/5c7369abff574388214084b4d8ac46fa78e11279/mods/taskbar-start-button-position.wh.cpp))

* Fixed the mod options not applying unless explorer is restarted in Windows 11 build 26100.4770 or newer.
* Improved compatibility with status bar programs such as YASB and Zebar.
* Fixed start menu layout becoming broken when settings are changed in some cases.

## 1.2.2 ([Mar 22, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c57c0a2cb008722765cf652cd24c207fe8818e82/mods/taskbar-start-button-position.wh.cpp))

* Fixed an incompatibility with recent Windows 11 preview builds.

## 1.2.1 ([Feb 8, 2025](https://github.com/ramensoftware/windhawk-mods/blob/65167d91ff632f8defcde31939d90f391e43c155/mods/taskbar-start-button-position.wh.cpp))

* Improved start menu positioning.
* Fixed the start menu width option not applying until restart in some cases.

## 1.2 ([Nov 1, 2024](https://github.com/ramensoftware/windhawk-mods/blob/f44455ff44bcaa2a31f0d6aca46b8bd22fe2dae1/mods/taskbar-start-button-position.wh.cpp))

* Fixed the jumpy animation that occurred when the taskbar is animated.
* Fixed taskbar items overlapping with the start button when the taskbar is full.
* Fixed the mod when the "Start menu on the left" option is disabled.

## 1.1.2 ([Nov 1, 2024](https://github.com/ramensoftware/windhawk-mods/blob/5b7ec49beb4c159ddfdef8481427a56992c0cb05/mods/taskbar-start-button-position.wh.cpp))

* Fixed the widget overlapping with the start button.

## 1.1.1 ([Sep 30, 2024](https://github.com/ramensoftware/windhawk-mods/blob/d4c63c187231bcd43dc7c476269b6377a89f9827/mods/taskbar-start-button-position.wh.cpp))

* Fixed taskbar buttons not being completely centered.
* Fixed restoring styles on unload.

## 1.1 ([Sep 21, 2024](https://github.com/ramensoftware/windhawk-mods/blob/e522fbec2bacd951c355649004aea1927462851f/mods/taskbar-start-button-position.wh.cpp))

* The start menu is now positioned on the left too. Can be disabled in the mod settings.
* Added support for multiple monitors.

## 1.0 ([Aug 24, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3a19d101477ac04951f6c533f92f137d0fdd048b/mods/taskbar-start-button-position.wh.cpp))

Initial release.
