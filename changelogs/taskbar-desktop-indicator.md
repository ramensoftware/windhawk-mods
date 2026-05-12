## 1.2.3 ([May 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b02a1ae38487f5c013e9197b2c7ba8098c8d3e0e/mods/taskbar-desktop-indicator.wh.cpp))

* Added an option to hide the indicator when only one virtual desktop exists, closes [#3634](https://github.com/ramensoftware/windhawk-mods/issues/3634)

## 1.2.2 ([May 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c76d030cf7c5848b97696ce764c1f5ec7db9d049/mods/taskbar-desktop-indicator.wh.cpp))

* Added support for newer Windows 11 builds where taskbar SystemTray symbols are in SystemTray.dll.
* Kept fallback support for older Taskbar.View.dll and ExplorerExtensions.dll builds.

## 1.2.1 ([Mar 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d205b0c73e8dacf528c684eac03b444da31b803a/mods/taskbar-desktop-indicator.wh.cpp))

Fixes the case where taskbar-desktop-indicator can stop updating after explorer.exe restarts if notification registration initially fails and polling fallback is disabled.

This keeps the notification thread alive and retries registration until the virtual desktop notification service becomes available, instead of attempting registration only once during startup.

Related issue: [#3520](https://github.com/ramensoftware/windhawk-mods/issues/3520)

## 1.2.0 ([Mar 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f00beff69e901b08bbfd500d8fe2f59c2a861720/mods/taskbar-desktop-indicator.wh.cpp))

Adds optional custom color settings for the taskbar desktop indicator:

- custom color for the active indicator / number
- custom color for inactive workspace markers
- inactive marker opacity now also applies when a custom inactive color is set
- updates the settings descriptions accordingly
- bumps the mod version to 1.2.0

## 1.1.0 ([Mar 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b55674f7fd91fa6e0c83f02d2993d9969d0dbc00/mods/taskbar-desktop-indicator.wh.cpp))

Initial release.
