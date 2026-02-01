## 1.1.3 ([Feb 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ddaa47d5ea629338f30ed0ed41d49774f9487a90/mods/taskbar-button-scroll.wh.cpp))

* Improved window minimize/restore robustness on Windows 11.

## 1.1.2 ([Mar 22, 2025](https://github.com/ramensoftware/windhawk-mods/blob/45104fb3435b9364f9905ad4e1bd8053739ec139/mods/taskbar-button-scroll.wh.cpp))

* Fixed an incompatibility with recent Windows 11 preview builds.

## 1.1.1 ([Feb 14, 2025](https://github.com/ramensoftware/windhawk-mods/blob/b6614250291822a82219a5eacb2d5bd68959936b/mods/taskbar-button-scroll.wh.cpp))

* The aero peek effect is now canceled when scrolling over thumbnails (for the new thumbnail previews implementation in Windows 11).

## 1.1 ([Feb 12, 2025](https://github.com/ramensoftware/windhawk-mods/blob/ceb0321bf91e11a795aa5c846bfb84864d73cb90/mods/taskbar-button-scroll.wh.cpp))

* Added support for the new thumbnail previews implementation in Windows 11.
* Added support for Windows 10.
* Added support for the Windows 10 taskbar on Windows 11 (the relevant option has to be enabled in the mod settings).
* Added support for the ExplorerPatcher taskbar. A version newer than ExplorerPatcher 67.1 is required. Currently, ExplorerPatcher 67.1 is the latest version, meaning that you'll have to wait for the next update. If you can't wait to try it, according to the maintainers of ExplorerPatcher, you can use [this pre-release version of the taskbar](https://github.com/ExplorerPatcher/ep_taskbar_releases/releases/tag/860073b).
* Fixed the taskbar becoming hung when scrolling over a window which isn't responding.

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
