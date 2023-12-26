## 2.1 ([Dec 26, 2023](https://github.com/ramensoftware/windhawk-mods/blob/71368b0ce9133d13dbe076d0a045cfbfc1762401/mods/classic-maximized-windows-fix.wh.cpp))

- Improved compatibility with Aero Snap. This was done by migrating to the undocumented `SetWindowRgnEx` function rather than the older public function.
- Rewrote the symbol hook wrapper CmwfHookSymbols for initial load with multiple symbol hooks.

## 2.0 ([Dec 25, 2023](https://github.com/ramensoftware/windhawk-mods/blob/a7051cb16cec7d50699a66aa4c47ab939c48f28c/mods/classic-maximized-windows-fix.wh.cpp))

- Moved global window procedure hooking to leverage the availability of the theme engine (uxtheme), rather than using SetWindowsHookEx.
- Improved compatibility with many apps, such as pre-Windows 8 versions of Task Manager.

## 1.2 ([Oct 7, 2023](https://github.com/ramensoftware/windhawk-mods/blob/0c2abf98ba3649753230cc5067fe8833cecf0723/mods/classic-maximized-windows-fix.wh.cpp))

Fix non-fully maximized windows. Namely open/save dialogs, which maximize but do not fill screen.

## 1.1 ([Oct 5, 2023](https://github.com/ramensoftware/windhawk-mods/blob/dd1d6601678911e85086109e88e21ca47785feb5/mods/classic-maximized-windows-fix.wh.cpp))

- Fixed bugs with DWM injection by excluding the mod from injecting into system processes.
- Fixed bugs with switching maximised applications to fullscreen mode (i.e. YouTube videos in web browser).
- Improved mod uninit routine to remove the clip from all affected windows.

## 1.0 ([Sep 9, 2023](https://github.com/ramensoftware/windhawk-mods/blob/59d4fcd662a854e70f166ea1756cf92ca940b02e/mods/classic-maximized-windows-fix.wh.cpp))

Initial release.
