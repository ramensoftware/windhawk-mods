## 1.0.7 ([Nov 7, 2024](https://github.com/ramensoftware/windhawk-mods/blob/88f7a3c83a0d67d032d94c18d438fa0f0f407a30/mods/taskbar-button-scroll.wh.cpp))

* Fixed all group items minimizing/restoring on scroll even when the group is not combined.
* Fixed compatibility with the `ExtendedUIXamlRefresh` Windows feature flag.
* Fixed thumbnails sometimes being stuck in an inconsistent state when scrolling over them.

## 1.0.6 ([May 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3a87f5e12b5f2c6778b4256c45c8124c38fad2cc/mods/taskbar-button-scroll.wh.cpp))

* Added an online symbol cache mechanism as a temporary workaround for the unavailable Microsoft symbols. Currently, this makes the mod work on Windows 11 versions 22631.3447 and 22631.3527. For more details, refer to [the relevant blog post](https://ramensoftware.com/windhawk-and-symbol-download-errors).

## 1.0.5 ([Sep 30, 2023](https://github.com/ramensoftware/windhawk-mods/blob/eb081300de3950ca47306e7c7ebd73c18d1ee6cf/mods/taskbar-button-scroll.wh.cpp))

* Improved compatiblity with other mods such as: Cycle taskbar buttons with mouse wheel.
* Fixed command invoking multiple times on rapid scrolling, which could cause a window to be un-maximized when restored.

## 1.0.4 ([Sep 30, 2023](https://github.com/ramensoftware/windhawk-mods/blob/bda321aa7a4e17c6e5f5df9e6df406a72fdd94b6/mods/taskbar-button-scroll.wh.cpp))

* Fixed the incompatibility with Windows 11 22H2 update KB5030310 ("Moment 4" update).

## 1.0.3 ([Apr 26, 2023](https://github.com/ramensoftware/windhawk-mods/blob/ba27a96436b8808fca3fc051ddc80c10d5094c10/mods/taskbar-button-scroll.wh.cpp))

* Improved switching to windows with additional popup windows.

## 1.0.2 ([Apr 26, 2023](https://github.com/ramensoftware/windhawk-mods/blob/a3300d6fe7a987529fa3e2522d4ac2db2b192eee/mods/taskbar-button-scroll.wh.cpp))

* Changed the way taskbar button scroll events are captured, making the mod more robust.
* Improved focus grabbing to bring other apps to front.

## 1.0.1 ([Apr 24, 2023](https://github.com/ramensoftware/windhawk-mods/blob/4e3c50df072814f5bd007c52f52184bb2ec128c6/mods/taskbar-button-scroll.wh.cpp))

* Fixed a bug which could cause the taskbar to freeze.

## 1.0 ([Apr 23, 2023](https://github.com/ramensoftware/windhawk-mods/blob/120b7b15054aea462b3c12d03ee4b080eb39d5ac/mods/taskbar-button-scroll.wh.cpp))

Initial release.
