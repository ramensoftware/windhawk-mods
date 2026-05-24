## 2.1 ([May 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/75eb49df7dd3d7ada526f7592d1f164290950150/mods/ultimate-custom-tray.wh.cpp))

New Features:
- Menu item states: Enabled / Disabled / Hidden
- Menu descriptions: Add text on the right (e.g., "Win+E")
- Menu separators: Add lines above/below items
- New folder shortcuts: ~network, ~personal, ~sounds

## 2.0 ([May 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d1eda85043c68ec6afa97504e8dd438f82916916/mods/ultimate-custom-tray.wh.cpp))

New Features:
- Added ability to swap left/right click actions for each tray icon individually. Enable "Swap mouse buttons" in settings to make left-click open context menu and right-click run the action.
- Icons can now be extracted directly from .exe and .dll files. Simply specify the full path to an application in the Icon field (e.g., C:\Program Files\Windhawk\windhawk.exe).
- Icon color now supports "Auto" mode that automatically detects Windows light/dark theme and adjusts icon colors accordingly.
- Context menus now support "Auto" theme mode that follows Windows system theme, plus manual Light/Dark options.
- Added press: prefix for keyboard shortcuts (e.g., press:Win+E), plus modifier signs - (run as admin) and * (show terminal window).
- Added Open menu near tray icon option — context menu can now open near the tray icon instead of at cursor position
- Added Menu direction setting — choose where menu appears (above/below/left/right of icon)
- Added Menu alignment setting — control menu alignment relative to icon (center/left/right/top/bottom)

Improvements:
- Improved icon rendering quality with better antialiasing
- Better handling of icon extraction and resizing
- Enhanced context menu appearance with proper dark mode support
- Fixed pre-multiplied alpha rendering for menu icons
- Added support for more image formats in context menus

Technical:
- Added uxtheme.dll integration for native dark mode menus
- Improved GDI+ icon scaling with high-quality interpolation
- Better memory management for icon resources
- Enhanced thread safety for icon operations

## 1.0 ([May 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7f92885de8e49e5169175fd8adcdc46854a938e4/mods/ultimate-custom-tray.wh.cpp))

Initial release.
