## 1.2.12 ([Aug 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7ad7264eae826e7b96ebb07e04770e175f5fee9e/mods/taskbar-icon-size.wh.cpp))

* Fixed the weather/news widget content sometimes disappearing when large icons are used.

## 1.2.11 ([Aug 18, 2024](https://github.com/ramensoftware/windhawk-mods/blob/68b57c0b24b7d2ef165ac7a4698ee8f2591fc445/mods/taskbar-icon-size.wh.cpp))

* Fixed the taskbar getting pushed offscreen on some devices with touchscreens.

## 1.2.10 ([Aug 10, 2024](https://github.com/ramensoftware/windhawk-mods/blob/e3b2fcf725fb4688a56af83dd299f2605182cdcc/mods/taskbar-icon-size.wh.cpp))

* Added support for Windows 11 version 24H2.

## 1.2.9 ([Jul 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7e0bd27ae1d12ae639497fbc9b48bb791f98b078/mods/taskbar-icon-size.wh.cpp))

* Fixed compatibility with update KB5040527 for Windows 11 version 23H2.

## 1.2.8 ([May 31, 2024](https://github.com/ramensoftware/windhawk-mods/blob/09d81d6358f4f0fc82e59541b4f0e6daaaad19fd/mods/taskbar-icon-size.wh.cpp))

* Fixed the weather icon disappearing if taskbar icons are centered.
* Added a restart prompt when the taskbar button width is changed in settings.

## 1.2.7 ([May 10, 2024](https://github.com/ramensoftware/windhawk-mods/blob/16879e75ab4846ac6eaf63c32539abc516850756/mods/taskbar-icon-size.wh.cpp))

* Fixed the weather icon disappearing with the new KB5036980 update.

## 1.2.6 ([May 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/76eac50ecafbe606e0840cb0c250afb8cfb7eb00/mods/taskbar-icon-size.wh.cpp))

* Added an online symbol cache mechanism as a temporary workaround for the unavailable Microsoft symbols. Currently, this makes the mod work on Windows 11 versions 22631.3447 and 22631.3527. For more details, refer to [the relevant blog post](https://ramensoftware.com/windhawk-and-symbol-download-errors).

## 1.2.5 ([Apr 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/159dc6e1497d197eb5613eb190f4909ef8ab667b/mods/taskbar-icon-size.wh.cpp))

* Fixed incorrect search icon size, started happening after update KB5036980.

## 1.2.4 ([Mar 7, 2024](https://github.com/ramensoftware/windhawk-mods/blob/23514c6d6153a7fe6a79382b37c9b2c3fbf4003c/mods/taskbar-icon-size.wh.cpp))

* Fixed a possible crash caused by the weather icon resizing code.

## 1.2.3 ([Mar 6, 2024](https://github.com/ramensoftware/windhawk-mods/blob/ccfedcb3f0ad1013cb821d360c6375c5cb5e3626/mods/taskbar-icon-size.wh.cpp))

* Fixed taskbar height not affecting maximized windows in some cases.
* Improved applying settings, hopefully fixing bugs where not all of the taskbar elements are updated.
* The weather widget icon size is now adjusted as well. In previous versions, the weather icon's size wasn't affected by the mod.

## 1.2.2 ([Nov 24, 2023](https://github.com/ramensoftware/windhawk-mods/blob/d1b9a9260c59f7f721e3439496eee9d0dafc5c2b/mods/taskbar-icon-size.wh.cpp))

* Fixed displaced secondary taskbar when auto-hide is enabled.

## 1.2.1 ([Nov 10, 2023](https://github.com/ramensoftware/windhawk-mods/blob/d28ad7f818078e99a4ccc492486bfcf534bf016e/mods/taskbar-icon-size.wh.cpp))

* Fixed displaced clock on secondary monitors.

## 1.2 ([May 6, 2023](https://github.com/ramensoftware/windhawk-mods/blob/3c6652f30f4745c6ab50521fb879edd22ed2aba2/mods/taskbar-icon-size.wh.cpp))

* Added an option to control the width of taskbar buttons.

## 1.1 ([Mar 18, 2023](https://github.com/ramensoftware/windhawk-mods/blob/a4f9caa90467d6d6f733dafc41ec9d078b6091cb/mods/taskbar-icon-size.wh.cpp))

* Renamed from "Large Taskbar Icons" to "Taskbar height and icon size".
* Fixed support for secondary taskbars.
* Fixed sizes of widget icons (start button and others) in some cases.
* Fixed notification area misalignment on Windows 11 22H2 (known issue on Windows 11 21H2 with small icons).
* Fixed mod applying before explorer starts running (e.g. on system startup) on new Windows 11 21H2 versions.
* Improved loading speed by caching symbols.

## 1.0.2 ([Oct 26, 2022](https://github.com/ramensoftware/windhawk-mods/blob/63a96e571a9ba6b6046b53a95c1ad52a22a0b9ef/mods/taskbar-icon-size.wh.cpp))

* Fix slow loading of the explorer process.

## 1.0.1 ([Jul 16, 2022](https://github.com/ramensoftware/windhawk-mods/blob/48e7136058f779e6e377fd36d37111d13599ea01/mods/taskbar-icon-size.wh.cpp))

* Fix applying settings on Windows startup.

## 1.0 ([Jul 16, 2022](https://github.com/ramensoftware/windhawk-mods/blob/af745c3a60a4837818ddbcec7de5cbdf8ecfd0bf/mods/taskbar-icon-size.wh.cpp))

Initial release.
