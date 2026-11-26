## 1.1.0 ([Nov 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1b6f2dfd4cff71c795209e87a86fbf1617620f4f/mods/start-menu-groups.wh.cpp))

- Rearrange groups and apps by dragging them right in Start, with no settings window and no administrator prompt: move groups, reorder apps or move them to another group, drop an app outside the tiles to make it an app on its own, and drop an app onto another app to put both in a new folder.
- New folders: put any number of apps in one cell of a tile, shown as a preview of their icons, and open them like a group. Supported in the settings, the settings window and Start.
- Opened groups and folders are centered over Start like Start's own folders, with grow and shrink animations, rounded corners and room for at least four rows.
- Tiles are stretched or shrunk slightly so each row of groups fills Start's width, with new left and right spacing settings.
- Works alongside Windows 11 Start Menu Styler and other mods that use XAML diagnostics.
- The groups show in any display language.
- The overflow cluster opens only the apps that didn't fit in the tile.
- The default horizontal gap between tiles is now 25 (was 30).
- The default websites no longer download favicons, so the default settings don't contact any server.
- Saving from the settings window checks the settings file before importing it and asks before overwriting changes made in Windhawk.
- Launched apps come to the front, and Start hides right after a launch.
- Settings window pages stretch to the full window width.
- Cleaner unloading of the mod.

## 1.0.0 ([Nov 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0d9d966af10c36b77bb8960c82ad9ae89bdcccd2/mods/start-menu-groups.wh.cpp))

Initial release.
