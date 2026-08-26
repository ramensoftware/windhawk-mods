## 10.0.36 ([Aug 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/65a796cb19f248d5d5cd06c3ad76619efed32102/mods/settings-to-control-panel.wh.cpp))

- Fixed a bug where some hooks were registered but never actually turned on, so a few features could silently stop working after Explorer restarted or after changing a setting
- Cleaned up some unnecessary error-handling code that wasn't actually protecting against anything
- Fixed the "Display" settings redirect being removed by accident, and added a toggle so this mod works together with the Classic Display Control Panel Restorer mod instead of conflicting with it
- Updated the README to be more accurate about what the mod actually protects against

## 10.0.35 ([Jul 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/639e8f6b8bde4a0009f039b4aa3dafa7922d5c4b/mods/settings-to-control-panel.wh.cpp))

- Made system tray redirect more robust in some edge cases
- Enhanced Windows 11 compatibility
- Fixed menu items appearing blank in system tray context menus
- Enhanced stability
- Added Legacy Name Mapping Fix

## 10.0.20 ([Jul 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f31ffa06985377849e7466a266663e70541c6939/mods/settings-to-control-panel.wh.cpp))

- added new mappings
- added redirects to the control panel for the system tray

## 10.0.1 ([Jun 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1ac6e9fec2eed8fc9df2082f410e528b70dc1720/mods/settings-to-control-panel.wh.cpp))

Initial release.
