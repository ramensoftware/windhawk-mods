## 1.1.5 ([Oct 3, 2023](https://github.com/ramensoftware/windhawk-mods/blob/4a1778822b1ce783729ec3fde550d59fdc7ce547/mods/taskbar-labels.wh.cpp))

* Fixed an overflow regression for a centered taskbar for Windows 11 22H2 before update KB5030310 ("Moment 4" update). The bug was introduced in version 1.1.4.

## 1.1.4 ([Sep 30, 2023](https://github.com/ramensoftware/windhawk-mods/blob/6a427c77a7a4c23efa5db4274c1c7ef5369b3eca/mods/taskbar-labels.wh.cpp))

* Initial fixes for the incompatibility with Windows 11 22H2 update KB5030310 ("Moment 4" update). There are still known bugs, but it's mostly usable.

## 1.1.3 ([Apr 3, 2023](https://github.com/ramensoftware/windhawk-mods/blob/d655abb41851b0ad352af334461da59b45a1867d/mods/taskbar-labels.wh.cpp))

* Undid the fix of the pinned items sometimes being too wide, since it caused more problems.
* Fixed compatibility with some insider builds (wasn't included in v1.1.2 by mistake).

## 1.1.2 ([Apr 1, 2023](https://github.com/ramensoftware/windhawk-mods/blob/e75309dae64419bf625a4aa116471d2715b4e9b4/mods/taskbar-labels.wh.cpp))

* Added a separate option for progress indicator style.
* Improved badge handling and fixed badge position for UWP apps.
* Improved support for small item width.
* Fixed pinned items sometimes being too wide.
* Fixed mod applying before explorer starts running (e.g. on system startup) on some Windows 11 21H2 versions.
* Fixed compatibility with some insider builds.

## 1.1.1 ([Mar 13, 2023](https://github.com/ramensoftware/windhawk-mods/blob/944eed5252e88f92ec24a738af705c092f3ac0c4/mods/taskbar-labels.wh.cpp))

* Fixed overflow handling for high DPI setups.

## 1.1 ([Mar 11, 2023](https://github.com/ramensoftware/windhawk-mods/blob/2192e42078b228d8f7995c07bb63644765f8e57d/mods/taskbar-labels.wh.cpp))

* Implemented adaptive sizing when there are too many items.
* Fixed missing labels when switching between virtual desktops or moving windows between monitors.
* Fixed wrong badge placement.
* Added several customization options such as running indicator style, font size and label string format.
* Improved compatibility with the Large Taskbar Icons mod.
* Improved loading speed by caching symbols.

## 1.0.3 ([Mar 3, 2023](https://github.com/ramensoftware/windhawk-mods/blob/c0b092ca7a1fb39f131ce0ae0c201184b006dbd3/mods/taskbar-labels.wh.cpp))

* Fix compatibility with Windows 11 update KB5022913.

## 1.0.2 ([Oct 26, 2022](https://github.com/ramensoftware/windhawk-mods/blob/d84761e1bf6fe6bc43c1eb03a3ee10b5d758b743/mods/taskbar-labels.wh.cpp))

* Fix slow loading of the explorer process.

## 1.0.1 ([Aug 25, 2022](https://github.com/ramensoftware/windhawk-mods/blob/fad5047cda95693be381808bb53b00ec0e4e9225/mods/taskbar-labels.wh.cpp))

* Fix for small pinned items on explorer launch.

## 1.0 ([Aug 22, 2022](https://github.com/ramensoftware/windhawk-mods/blob/a62a9b7306cdd66a8bac68c2d6448fb2111e66c3/mods/taskbar-labels.wh.cpp))

Initial release.
