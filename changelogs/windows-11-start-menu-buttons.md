## 2.2 ([Jul 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8df93b9c741196c6fa1eae0e4bf295cc080cb5c2/mods/windows-11-start-menu-buttons.wh.cpp))

New Features:

- Added nested submenus support - create submenus within submenus up to 3 levels deep for both left-click and right-click menus
- Added right-click menu support - buttons can now have separate menus for right-click actions (up to 3 levels)
- Submenu icons now support image files (.png, .ico, .jpg, .bmp, .webp) and app paths (.exe, .dll) for icon extraction
- Icon paths now support both quoted and unquoted formats (e.g. "C:\Path\icon.png" or C:\Path\icon.png)
- Added multiple commands support - execute multiple commands in sequence by adding multiple Action entries in Windhawk UI
- Preset buttons now ignore user-defined submenus and right-click menus to maintain consistent preset behavior
- **Added [Visual Configurator](https://salyts.github.io/Windows-11-Start-Menu-Buttons/)** — a web-based tool to configure buttons visually with drag & drop, icon browser, submenu builder, export/import and more

Bug Fixes:

- Fixed "Container left margin" and "Container right margin" not working for Account button
- Fixed quote handling in icon paths - now properly strips outer quotes before processing

Notes:

- When upgrading from v2.1, Action fields may appear empty in UI but will continue to work. To edit them or add multiple commands, re-enter the actions manually in Windhawk settings. See mod description for details.
- **Upgrading from v2.1?** Use the **[Configurator's Migrate tool](https://salyts.github.io/Windows-11-Start-Menu-Buttons/)** to convert your v2.1 config to v2.2 format automatically — paste your old config, click Migrate, and copy the result back to Windhawk.

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
