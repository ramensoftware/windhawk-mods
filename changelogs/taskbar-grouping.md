## 1.3.1 ([Dec 27, 2023](https://github.com/ramensoftware/windhawk-mods/blob/a378597db91ccbf3a5bdbfbee7f3c3dd437010d7/mods/taskbar-grouping.wh.cpp))

* Fixed a regression which caused pinned items to be separated when they shouldn't be (introduced in v1.3).

## 1.3 ([Dec 24, 2023](https://github.com/ramensoftware/windhawk-mods/blob/f80c31b2a02a858d585c01b85fb3d5d5f1789555/mods/taskbar-grouping.wh.cpp))

* Added a new mode: Pinned items remain in place, group running instances.
* Added an option to only disable grouping for listed programs.
* Custom groups and excluded programs can now be specified with an application ID.
* Fixed some cases of taskbar item recreation.

## 1.2.2 ([May 8, 2023](https://github.com/ramensoftware/windhawk-mods/blob/fe7c3b4be6113f48dcdd876ed401e5ce104335da/mods/taskbar-grouping.wh.cpp))

* Fixed a rare crash.

## 1.2.1 ([Apr 24, 2023](https://github.com/ramensoftware/windhawk-mods/blob/59bcbf7c877ecdd7a5b70216e989489f899e7a80/mods/taskbar-grouping.wh.cpp))

* Improved icon handling.
* Added the option to use window icons instead of application icons.
* Made unpinning a separated pinned item slightly less buggy.

## 1.2 ([Apr 22, 2023](https://github.com/ramensoftware/windhawk-mods/blob/f472d07a9c4fc0cf94c18fde2a52570b6fbd82f2/mods/taskbar-grouping.wh.cpp))

* Pinned items, if not separated, are now shown only when the last window closes.
* Jump lists always show the relevant items, such as recent files.
* Improved icon consistency.
* New option: Place ungrouped items together.

## 1.1.1 ([Apr 18, 2023](https://github.com/ramensoftware/windhawk-mods/blob/3ba4830923ea4d304c20a50f41b49a8bff6f47d8/mods/taskbar-grouping.wh.cpp))

* Fixed a crash on window snapping by disabling the snapping groups in taskbar previews.

## 1.1 ([Apr 15, 2023](https://github.com/ramensoftware/windhawk-mods/blob/2931b75955e2e4db928760c5c29fb3048c4e6f7e/mods/taskbar-grouping.wh.cpp))

* Reimplemented and made to work in more cases.
* Improved launching a new instance with middle click or shift+left click to work in more cases.
* Added an option for pinned items to be replaced with the first running instance (and not kept separated).
* Added an option for configuring custom groups.
* Added an option for configuring programs to be excluded by the mod.
* Added support for the old taskbar on Windows 11 (e.g. with Explorer Patcher).

## 1.0 ([Mar 6, 2022](https://github.com/ramensoftware/windhawk-mods/blob/85322d8095db39e00abcd70168b490c9602c43d4/mods/taskbar-grouping.wh.cpp))

Initial release.
