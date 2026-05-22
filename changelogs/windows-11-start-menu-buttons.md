## 2.1 ([May 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5cebaf3b9d51243548c53c9df5d14a54bfb1dd88/mods/windows-11-start-menu-buttons.wh.cpp))

New Features:
  - Added press: action prefix for keyboard shortcuts (e.g., press:Win+E or press:0x5B;0x45)
  - Added modifier signs for actions: - (run as admin) and * (show terminal window)
  - Added support for extracting icons from .exe and .dll files
  - Icon field now accepts 4-digit hex codes without \u prefix (e.g., E774)
  - Icon field now accepts /u as alternative to \u for Unicode escapes

  Bug Fixes:
  - Fixed icon normalization and parsing
  - Improved GDI+ initialization for icon extraction
  - Fixed window class registration error handling
  - Improved code stability and error handling

## 2.0 ([Apr 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/596a4eba99e22c1a8aefd4c960d33bb90879bdda/mods/windows-11-start-menu-buttons.wh.cpp))

- Preset system: Pre-made button templates have been added to simplify configuration.
- Custom icons: Added support for .png, .ico, .jpg, .bmp, and .webp formats as button icons (thanks to [@SharkIT-sys](https://github.com/SharkIT-sys)).
- Localization: Added support for Russian.
- Account button: New options to hide or reposition the user profile button.
- Action formats: You must now use the “web:” prefix to specify a link.
- Improved submenus: It is now easier to create submenus through Windhawk settings.
- Updated Readme file.
- Other improvements and fixes.

## 1.1 ([Mar 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9b7c94904e9d65cd9c1c24efe672f5cb5f2a6ee2/mods/windows-11-start-menu-buttons.wh.cpp))

Initial release.
