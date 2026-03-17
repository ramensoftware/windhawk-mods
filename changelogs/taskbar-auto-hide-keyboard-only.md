## 2.0 ([Mar 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b99b0e5e0cfbe608e0657ef8ab21a8f3c555ca8a/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

* Renamed from "Taskbar keyboard-only auto-hide" to "Taskbar auto-hide fine tuning".
* Added multiple auto-hide modes: Keyboard or mouse click, Keyboard only (previously "Fully hide"), and Never (blocks all unhiding including notifications and Win key).
* Added configurable hotkeys for showing the taskbar temporarily and toggling permanent visibility (always-show mode).
* Added a mouse event option (middle click or double click) on the Win11 taskbar to toggle always-show mode.

## 1.1.2 ([Jul 19, 2025](https://github.com/ramensoftware/windhawk-mods/blob/e857f64901909ed4e2aa2bc83ab06898effdb4a1/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

* Fixed the taskbar unhiding in some cases when the "Fully hide" option is enabled.
* Improved compatibility with status bar programs such as YASB and Zebar.

## 1.1.1 ([Mar 23, 2025](https://github.com/ramensoftware/windhawk-mods/blob/20ab961f2e9e81c99927394bd0a6d1dcb3987de9/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

* Fixed Win+num hotkeys not working when the "Fully hide" option is enabled.

## 1.1 ([Mar 22, 2025](https://github.com/ramensoftware/windhawk-mods/blob/9e22c833002db2e8c8c002ea098d25805d09167c/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

* Added support for secondary taskbars.
* Added the "Fully hide" option. Normally, the taskbar is hidden to a thin line which can be clicked to unhide it. The new option makes it so that the taskbar is fully hidden on auto-hide, leaving no traces at all. With this option, the taskbar can only be unhidden via the keyboard.

## 1.0 ([Feb 24, 2025](https://github.com/ramensoftware/windhawk-mods/blob/a964360a62a40333fb917ccea2c5513fe7e77b3b/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

Initial release.
