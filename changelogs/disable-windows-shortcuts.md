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
