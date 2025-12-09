## 1.0.9 ([Dec 9, 2025](https://github.com/ramensoftware/windhawk-mods/blob/b43686cacbdb524ba6ddb1776332ecd32bfd4e3f/mods/taskbar-button-click.wh.cpp))

* Ending task (i.e. terminating the process) is no longer applied for store (modern/UWP) apps, since several apps might share the same process.
* Fixed a crash due to the removal of unused taskbar functions in recent Windows 11 builds. Only relevant if the "Multiple items behavior" option is set to "Close foreground window".

## 1.0.8 ([Jun 3, 2025](https://github.com/ramensoftware/windhawk-mods/blob/78fc318a181cfb5299fab5beaf42a8726ebe5f9e/mods/taskbar-button-click.wh.cpp))

* Added support for ARM64.
* The "Customize the old taskbar on Windows 11" option no longer needs to be enabled for the mod to work with the ExplorerPatcher taskbar.

## 1.0.7 ([Oct 11, 2024](https://github.com/ramensoftware/windhawk-mods/blob/d190ad2c0e8b24bd6b46292507fceaad45472d8f/mods/taskbar-button-click.wh.cpp))

* Holding Ctrl while middle clicking will end the running task. The key combination can be configured or disabled in the mod settings.
* Added support for the ExplorerPatcher taskbar. Versions newer than 22621.3880.66.6 are supported. The "Customize the old taskbar on Windows 11" option must be enabled for the mod to work with the ExplorerPatcher taskbar.

## 1.0.6 ([May 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/f49525eaab8fbdf923f203c514c42b215eeab10e/mods/taskbar-button-click.wh.cpp))

* Added an online symbol cache mechanism as a temporary workaround for the unavailable Microsoft symbols. Currently, this makes the mod work on Windows 11 versions 22631.3447 and 22631.3527. For more details, refer to [the relevant blog post](https://ramensoftware.com/windhawk-and-symbol-download-errors).

## 1.0.5 ([Nov 24, 2023](https://github.com/ramensoftware/windhawk-mods/blob/1f28d4e873e7790ddb44cf727fbf6d90c0a8167a/mods/taskbar-button-click.wh.cpp))

* Added support for the new Windows 11 taskbar labels option.

## 1.0.4 ([Apr 24, 2023](https://github.com/ramensoftware/windhawk-mods/blob/0ff37a61a6d89141553c7109ffd4170e5a0d5009/mods/taskbar-button-click.wh.cpp))

* Improved compatibility with the taskbar-grouping mod. Previously, using both mods could lead to a middle click closing the wrong window.

## 1.0.3 ([Jan 4, 2023](https://github.com/ramensoftware/windhawk-mods/blob/5f8922e7da38b162738fc5b7e4c86528654a6e6a/mods/taskbar-button-click.wh.cpp))

* Fix support for the Never Combine configuration for the old taskbar in Windows 11.

## 1.0.2 ([Dec 31, 2022](https://github.com/ramensoftware/windhawk-mods/blob/2bf09d3193530d74167e7123f7ff4d0412bdda4f/mods/taskbar-button-click.wh.cpp))

* Add support for the Never Combine configuration in Windows 10.
* Add support for the "Close foreground window" option for multiple windows in Windows 10.
* Add support for the old taskbar on Windows 11 (e.g. with Explorer Patcher).
* Improve loading speed by caching symbols.

## 1.0.1 ([Jul 16, 2022](https://github.com/ramensoftware/windhawk-mods/blob/ee201c1c1ae87d057978fc4b6a315be4a9382f90/mods/taskbar-button-click.wh.cpp))

* Added options regarding closing of multiple items.

* Fixed middle click on pinned items.

## 1.0 ([Mar 6, 2022](https://github.com/ramensoftware/windhawk-mods/blob/85322d8095db39e00abcd70168b490c9602c43d4/mods/taskbar-button-click.wh.cpp))

Initial release.
