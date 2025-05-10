## 1.4.1 ([May 10, 2025](https://github.com/ramensoftware/windhawk-mods/blob/5e41b17208d957cf8b3dbfcc837c24ceb2e2eaa7/mods/taskbar-labels.wh.cpp))

* Added an option for customizing text trimming (ellipsis/clip).
* Added an option to hide the running indicator by setting the running indicator height to -1.
* Improved compatibility with the "Taskbar height and icon size" mod.
* Fixed the mod not applying on startup in some cases.
* Improved compatibility with status bar programs such as YASB and Zebar.

## 1.4 ([Mar 21, 2025](https://github.com/ramensoftware/windhawk-mods/blob/f235abdd918c4639a7631e9e228399b22c0b734b/mods/taskbar-labels.wh.cpp))

* Added an option to customize the font family of the labels.
* Added options to customize the running indicator height and vertical offset.
* Fixed an incompatibility with recent Windows 11 preview builds.
* Fixed misaligned running indicators when custom icon sizes are used (e.g. with the Taskbar height and icon size mod).

## 1.3.5 ([Dec 13, 2024](https://github.com/ramensoftware/windhawk-mods/blob/37fce381dc197fbd1858b833d6ca0475acb40a54/mods/taskbar-labels.wh.cpp))

* Undid the fix of thumbnail labels incorrectly showing or not showing when using some modes, since the fix had unintended side effects.
* Reimplemented an option to always show thumbnail labels, so that it can still be used as a workaround.

## 1.3.4 ([Dec 13, 2024](https://github.com/ramensoftware/windhawk-mods/blob/17e0c80c827a71f9a6a66079eaaae4dd289fecf9/mods/taskbar-labels.wh.cpp))

* Fixed thumbnail labels incorrectly showing or not showing when using some modes.
* Added an option to always show thumbnail labels. By default, thumbnail labels are shown on hover only if taskbar labels are hidden, but that might not be applied for all customizations that this mod offers.
* Fixed restoring labels on mod unload.

## 1.3.3 ([Dec 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b0179b2e31ee74e302077bfb2de1b7cdfc6092d7/mods/taskbar-labels.wh.cpp))

* The "Minimum taskbar item width" option can now be set to any value. Only 44 or larger values were previously supported.
* Fixed a crash which could occur in some cases due to an incorrect running indicator width calculation.
* Fixed taskbar settings (the "Combine taskbar buttons and hide labels" option) affecting some of the taskbar behavior, such as left click on combined items.

## 1.3.2 ([Oct 20, 2024](https://github.com/ramensoftware/windhawk-mods/blob/0951986351d82332cd99a2335f16f6fbcb22033b/mods/taskbar-labels.wh.cpp))

* Added an option exclude programs by application ids.
* Fixed the "Minimum taskbar item width" option that stopped working after a recent update of Windows 11 version 23H2.

## 1.3.1 ([Sep 29, 2024](https://github.com/ramensoftware/windhawk-mods/blob/984a75a743bba52a247387158c47db2ede70f31a/mods/taskbar-labels.wh.cpp))

* Improved badge placement (e.g. unread counters) in some cases.
* Fixed the "Centered, fixed size" indicator not being always centered.

## 1.3 ([Sep 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/18d85f2bce2e5fbb27499f93281a2d79b13c6670/mods/taskbar-labels.wh.cpp))

* Added four modes, last two of which weren't possible before:
  * Show labels, don't combine taskbar buttons
  * Hide labels, combine taskbar buttons
  * Show labels, combine taskbar buttons
  * Hide labels, don't combine taskbar buttons
* Added an option to show or hide labels for specific programs.
* Improved custom indicator compatibility with adaptive width labels. Previously, it could break the layout or in rare cases cause a crash.
* Fixed the "Minimum taskbar item width" option in newer Windows 11 versions.

## 1.2.5 ([May 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/386a0d9c151361b9c9695a67658d9ffadc2df51c/mods/taskbar-labels.wh.cpp))

* Added an online symbol cache mechanism as a temporary workaround for the unavailable Microsoft symbols. Currently, this makes the mod work on Windows 11 versions 22631.3447 and 22631.3527. For more details, refer to [the relevant blog post](https://ramensoftware.com/windhawk-and-symbol-download-errors).
* Adjusted detection of native taskbar labels being turned off for new Windows 11 builds.
* Fixed restoring maximum label width on mod unload.

## 1.2.4 ([Apr 10, 2024](https://github.com/ramensoftware/windhawk-mods/blob/52fb2fdb8d52bcee8b05e4e7f89ac3027f633954/mods/taskbar-labels.wh.cpp))

* Added an option to set the maximum taskbar item width (only used for the Windows adaptive width).

## 1.2.3 ([Apr 7, 2024](https://github.com/ramensoftware/windhawk-mods/blob/37d87af5cad91032a99660648134df05b1201b2c/mods/taskbar-labels.wh.cpp))

* Fixed width of pinned items with recent Windows 11 updates.

## 1.2.2 ([Dec 8, 2023](https://github.com/ramensoftware/windhawk-mods/blob/098b5d81a22328110d49566ffbcd9ea1a3da4243/mods/taskbar-labels.wh.cpp))

* Added an option to hide labels and still use the other customizations.
* Added logic to avoid crashing when a non-positive font size is set.
* Fixed left indicator style position with custom left and right padding size value.
* Fixed truncated labels for wide taskbar buttons.

## 1.2.1 ([Nov 29, 2023](https://github.com/ramensoftware/windhawk-mods/blob/b9c4fefe6754408557a92715d6f4e11465f8ae3a/mods/taskbar-labels.wh.cpp))

* Added a new option: Minimum taskbar item width.
* Fixed label texts and icons to refer to the window and not the app. The downside of the fix is that combining can no longer be enabled ("When taskbar is full" still works). This may be addressed in the future.
* Fixed "Space between icon and label" calculation in some cases.
* Fixed label disappearing when dragging.
* Fixed settings applying in some cases.
* Fixed for new builds with the labels feature turned off.

## 1.2 ([Nov 29, 2023](https://github.com/ramensoftware/windhawk-mods/blob/55cf8e1768150ba51f8b822a44f19de3cf0cc9fa/mods/taskbar-labels.wh.cpp))

* A new, more robust implementation for newer Windows 11 versions with a native taskbar labels implementation.

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
