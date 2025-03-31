## 1.1 ([Mar 31, 2025](https://github.com/ramensoftware/windhawk-mods/blob/bcc31d6dbb7a99877da5ddf191dbe9737f49e0f6/mods/disable-virtual-desktop-transition.wh.cpp))

This update adds support for Windows 11. I tested on `10.0.22631.2428`, but the Windows 11 approach is general enough that it should work for all versions.

In Windows 11, they moved the animation to reside in `TWinUI.PCShell.dll`, which is loaded into the main shell process `explorer.exe`. In Windows 10, it resided in `uDWM.dll` which ran in `dwm.exe`.

## 1.0 ([Dec 9, 2024](https://github.com/ramensoftware/windhawk-mods/blob/000110fb95288d15f121807167c826024d9e764e/mods/disable-virtual-desktop-transition.wh.cpp))

Initial release.
