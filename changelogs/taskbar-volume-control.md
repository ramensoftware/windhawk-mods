## 1.3 ([Feb 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7514df4a76170d6b3cd9075791227880f2068fb4/mods/taskbar-volume-control.wh.cpp))

* Added an option to scroll anywhere on the screen while holding a configurable combination of modifier keys to change the system volume.
* Added an option to scroll at the taskbar position even when a full screen window covers the taskbar.
* Added an option for custom regions along the taskbar where scrolling will control the system volume. For example, "100-200" (pixels) or "20%-50%" (percentage of taskbar length).
* Improved the volume change step option, which is now compatible with all volume indicators.
* Improved the "No automatic mute toggle" option, which is now compatible with all volume indicators.

## 1.2.2 ([Mar 21, 2025](https://github.com/ramensoftware/windhawk-mods/blob/066479c7768f6916417c24e9eea34a022182df0f/mods/taskbar-volume-control.wh.cpp))

* Fixed an incompatibility with recent Windows 11 preview builds.
* The "Customize the old taskbar on Windows 11" option no longer needs to be enabled for the mod to work with the ExplorerPatcher taskbar.

## 1.2.1 ([May 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/dbac51f369f38001f43dd37f3007ce7ebadcbd20/mods/taskbar-volume-control.wh.cpp))

* Added an option to activate volume scrolling only when holding the Ctrl key.
* Added a new scroll area option: The taskbar without the notification area.
* On new Windows 11 versions, consider the clock on secondary taskbars as a notification area.

## 1.2 ([Jul 15, 2023](https://github.com/ramensoftware/windhawk-mods/blob/16e1e6b2ef632628f27b36f035ada31cc904e97d/mods/taskbar-volume-control.wh.cpp))

* Added support for the old taskbar on Windows 11 (e.g. with Explorer Patcher).

## 1.1.1 ([Jul 15, 2023](https://github.com/ramensoftware/windhawk-mods/blob/976339b249645ff6874144f4037886085e0f1411/mods/taskbar-volume-control.wh.cpp))

* Fixed a crash in Windows 11 version 21H2.

## 1.1 ([Apr 2, 2023](https://github.com/ramensoftware/windhawk-mods/blob/504d0704581f14714990fe7c3271070711679b0e/mods/taskbar-volume-control.wh.cpp))

* Added the new Windows 11 volume indicator.
* Added middle click to mute.
* Removed mouse hooks and made the mod more robust and work in more scenarios, e.g. when an elevated program is focused.
* Removed the ugly border in the Windows 10 volume indicator.
* Removed the tooltip volume indicator which wasn't supported on Windows 11 and was buggy on Windows 10.

## 1.0 ([May 19, 2022](https://github.com/ramensoftware/windhawk-mods/blob/467699b935313e8785c14753b063c5fddc28ac87/mods/taskbar-volume-control.wh.cpp))

Initial release.
