## 1.3.0 ([Sep 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bd465cf91976360394dd11f5b58ed083080b1c33/mods/disable-windows-shortcuts.wh.cpp))

* Added new shortcuts: `Win+Shift+C` (Charms Menu in Windows 10) and `Win+Shift+R` (Snipping Tool screen recording).
* Added numpad `+` and `-` support for Magnifier zoom (`Win+Plus` / `Win+Minus`).
* Fixed modifier keys occasionally getting stuck when keys are held during workstation lock or desktop switch.
* Fixed premature/redundant Explorer restart prompts by tracking blocked state per Explorer PID in Windhawk storage.
* Fixed race condition in uninit prompt thread cleanup and bounded prompt wait timeout.
* Dynamically load `TaskDialogIndirect` from `comctl32.dll`, removing the `-lcomctl32` compiler dependency.
* Preserved the 3-tier options for Special Shortcuts (`Off`, `Disable hotkey`, `Block hotkey`) to keep `dwm.exe` optional for standard users.

## 1.2.1 ([Jul 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bc9c9d57104d5081e9e70a507664872a0d4378e4/mods/disable-windows-shortcuts.wh.cpp))

- Add `Win+F1` shortcut to block windows help from opening in default browser

## 1.2.0 ([May 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4960f73fdd2c7d829d7d160372887ad73b46f0be/mods/disable-windows-shortcuts.wh.cpp))

- Fixed an annoying issue for users with PowerToys installed where it triggers the explorer restart prompt every time during startup. Addresses [#3973](https://github.com/ramensoftware/windhawk-mods/issues/3973)

## 1.1.1 ([Apr 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/00f99498b90b44630ab4e46c10038c3e3000a663/mods/disable-windows-shortcuts.wh.cpp))

- Added new shortcuts ([#3880](https://github.com/ramensoftware/windhawk-mods/issues/3880))
  - Standard: `Win+Q`, `Win+Ctrl+F`, `Alt+Shift`, `Ctrl+Esc`, `Win`
  - Special: `Win+/` 
- Fixed an issue where after re-enabling the mod, it won't apply the settings. It shows an explorer restart prompt to re-apply the settings correctly now
- Fixed mod un-initialization and re-initialization delay
- Fixed `Win+Space` which refused to get blocked in the earlier versions

## 1.1.0 ([Apr 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7c82186f741a488c1fa6ac78b2247bb5399c9411/mods/disable-windows-shortcuts.wh.cpp))

- Fixes few shortcut keys that are hard-coded into windows with a new approach
- Refactored settings into two separate groups for the updated shortcuts

## 1.0.0 ([Apr 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bf450c23f6810c142a3e5a4445b9afa296df24b3/mods/disable-windows-shortcuts.wh.cpp))

Initial release.
