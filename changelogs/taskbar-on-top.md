## 1.1.6 ([Dec 20, 2025](https://github.com/ramensoftware/windhawk-mods/blob/de4cfd5ea752b671743eb99a1dd9abeba39e413f/mods/taskbar-on-top.wh.cpp))

* Implemented basic support for taskbar auto-hide.
* Fixed incorrect Alt+Tab window position with multiple monitors in some cases.

## 1.1.5 ([Oct 26, 2025](https://github.com/ramensoftware/windhawk-mods/blob/6a179f7c2bc4de25e0d72167534b5583c43db03e/mods/taskbar-on-top.wh.cpp))

* Reworked Start menu positioning:
  * The [redesigned Windows 11 Start menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/) is now supported.
  * Added an option for the Start menu to animate from the top using a technique discovered by [SandTechStuff](https://github.com/SandTechStuff) (not available for the redesigned Start menu).
  * The account card at the bottom of the Start menu is no longer truncated.
* Added a workaround for the edges of the taskbar being non-clickable in some cases.
* Added a workaround for compatibility with ExplorerPatcher. Previously, if the mod was used together with ExplorerPatcher, the Start menu could be misplaced at the edge of the screen.

## 1.1.4 ([Jul 17, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c094b860efa76d6d481a26237c089695d6f78406/mods/taskbar-on-top.wh.cpp))

* Fixed a crash when hovering over taskbar items in recent Windows 11 builds.
* Fixed [the redesigned start menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/) being cut off. Currently it opens in the original location. Better support is planned for follow-up updates.

## 1.1.3 ([May 31, 2025](https://github.com/ramensoftware/windhawk-mods/blob/f696c21820a69cca5815ca7deca1f0a91675b5c0/mods/taskbar-on-top.wh.cpp))

* Fixed tooltips covering the tray icons in the first row of the overflow popup.
* Fixed the placement of thumbnail previews in recent Windows builds.
* Improved the placement of thumbnail preview tooltips.
* The task view popup is now shown at the top of the screen and not no the bottom.
* Improved compatibility with status bar programs such as YASB and Zebar.

## 1.1.2 ([Mar 22, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7a9a61a0563a031b366c70f4e4620f7c97a371b4/mods/taskbar-on-top.wh.cpp))

* Fixed an incompatibility with recent Windows 11 preview builds.
* Fixed context menus of taskbar items being covered by the overflow popup.
* Fixed thumbnail preview tooltips covering the close button on hover.
* Fixed thumbnail preview positions when a touch screen is used.

## 1.1.1 ([Feb 8, 2025](https://github.com/ramensoftware/windhawk-mods/blob/33d869f823bca2613379a3ce5664cc27b0e9471d/mods/taskbar-on-top.wh.cpp))

* Added support for the new thumbnail previews implementation in Windows.

## 1.1 ([Jan 29, 2025](https://github.com/ramensoftware/windhawk-mods/blob/124523bc261918872e9ca412c587f832519d7a01/mods/taskbar-on-top.wh.cpp))

* Added an option to show running indicators above the taskbar icons.
* Fixed context menus not appearing with monitors that are arranged vertically, one above the other.

## 1.0.5 ([Nov 3, 2024](https://github.com/ramensoftware/windhawk-mods/blob/25345e441de9f8b292d8a2d316c06ac6869e3ced/mods/taskbar-on-top.wh.cpp))

* Fixed an incompatibility with the following mods which could lead to a crash: Taskbar tray icon spacing, Taskbar tray system icon tweaks.

## 1.0.4 ([Oct 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/8195a025df8ce6bfbd9a3dab732811619d75d205/mods/taskbar-on-top.wh.cpp))

* Fixed the start menu opening on the left side of the screen, even if the icons on the taskbar are centered. The bug was caused by the attempt to change the incorrect jump list animation in version 1.0.3. As a result, if icons are centered, jump list animation will be incorrect again. This affects only Windows 11 23H2, there's no such issue on Windows 11 24H2.

## 1.0.3 ([Oct 19, 2024](https://github.com/ramensoftware/windhawk-mods/blob/ab4920afd69b029af1091d6f9598dd1c1c90eed8/mods/taskbar-on-top.wh.cpp))

* Improved compatibility with some programs which query the taskbar location, such as EarTrumpet.
* Adjusted jump list menu animations.

## 1.0.2 ([Oct 19, 2024](https://github.com/ramensoftware/windhawk-mods/blob/8d4c428d099ef834d8f616a0c78157a58b4ac458/mods/taskbar-on-top.wh.cpp))

* Fixed tray icon context menu not being clickable.
* Improved compatibility with some devices, mostly tablets and touchscreen devices.
* Fixed jump list position for multiple monitors in some cases.

## 1.0.1 ([Oct 5, 2024](https://github.com/ramensoftware/windhawk-mods/blob/2fb9f53e0e636376c07c33fa9a861345c4572e1a/mods/taskbar-on-top.wh.cpp))

* Improved jump list size and position handling.
* Adjusted the 1px taskbar border to be at the bottom of the taskbar.
* Fixed the start menu disappearing or being positioned incorrectly sometimes.

## 1.0 ([Oct 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b43269d44eb047e3f27c015faca6fd365b0960d1/mods/taskbar-on-top.wh.cpp))

Initial release.
