## 2.3 ([Apr 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9f6a018d93adf561e55dc5318babf4428073c11c/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

* Fixed a crash on system startup when the "Never" auto-hide mode is used.
* Improved support for the new taskbar auto-hide animation in Windows 11 preview builds (ViVeTool ID: 41356296).

## 2.2 ([Mar 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ccbf8d62b852558521be1be12194cb2f2df85a56/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

* Added an option to configure the time the taskbar stays visible after being shown temporarily.
* Improved "Show taskbar, open Start menu if already shown" Win key action to always trigger the primary taskbar, and to open the Start menu even if the taskbar wasn't shown by the mod (e.g. by hovering).

## 2.1 ([Mar 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/20436b3a478ab7a2f7ba4d5e5414bf3abe232de0/mods/taskbar-auto-hide-keyboard-only.wh.cpp))

* Added an option to customize the action to perform when the Win key is pressed: show the taskbar without showing the Start menu, or toggle permanent taskbar visibility.
* Fixed hotkeys not working on system startup (they only worked if the mod is enabled later on).

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
