## 1.3 ([Nov 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ab22480f9d59785b176354b0441c4b4da4945d74/mods/taskbar-system-utilities.wh.cpp))

- **Custom Menu Items**: Added support for launching custom programs, files, folders, scripts, URLs, `ms-settings:` pages, and `shell:` locations directly from the Windows 11 taskbar context menu (includes **Terminal** and **Control Panel** by default).
- **Rich Item Customization**: Each custom entry supports environment variable expansion (e.g., `%USERPROFILE%`), optional command-line arguments, Segoe Fluent Icons hex glyphs or emojis, **Run as administrator** (UAC elevation), **Hide window**, separator lines (`-`), and disabled section headings (items without a command).
- **Flexible Menu Layout & Submenus**: Added options to place custom items below built-in items (`bottom`), above them (`top`), or inside the **System Utilities** submenu (`submenu`), group custom items into their own dedicated submenu with a custom name and icon, and choose top or bottom placement for System Utilities.
- **Improved Unload Lifecycle**: Added weak-reference tracking for injected menu items and click handlers to cleanly detach and hide elements when the mod is unloaded or updated.

## 1.2 ([Nov 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/15d44b0108ad149dc9432402390eff99e05a1950/mods/taskbar-system-utilities.wh.cpp))

* Added support for grouping taskbar utilities into a customizable dropdown submenu (`MenuFlyoutSubItem`) to reduce root menu clutter.
* Added settings to configure submenu visibility, label text, and icon glyph.
* Added individual `inSubmenu` toggles for each utility in the settings.

## 1.1 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/86a896f7ddd20f2bf60d0798e487701f009f6595/mods/taskbar-system-utilities.wh.cpp))

- Added **Reduce Memory** tool to trim working sets of non-elevated processes.
- Added **Rebuild Icon & Thumbnail Cache** tool (`thumbcache_*.db` cleanup + `ie4uinit.exe -show`).
- Added optional **Public IP** lookup in Get IP Address (`getIp.queryPublicIp`, disabled by default for privacy).
- Reorganized settings into structured groups for better UX (**Note:** existing settings are reset to defaults due to the new grouped key structure and need to be reconfigured).
- Filtered out non-preferred and APIPA (`169.254.x.x`) local network addresses.

## 1.0 ([Nov 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/48e23bd6b1d7ceb1fb47f261ffdd36ec211a7dd3/mods/taskbar-system-utilities.wh.cpp))

Initial release.
