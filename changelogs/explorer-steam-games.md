## 1.2 ([Nov 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/60313640913ae0898aa64c4085841b4712fed91f/mods/explorer-steam-games.wh.cpp))

- added an optional “Navigation pane icons” setting, so you can use your own icon for each navigation pane entry (an .ico file or an .exe/.dll with an index). If the field is empty or the file doesn’t exist, the default icon is used. Like before, it only writes to the private hive, never the real registry.

## 1.1 ([Oct 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e86ece4b39234ad53c9e32916d196c1c932e3836/mods/explorer-steam-games.wh.cpp))

Added a “Position in the navigation pane” option. The entries can go above This PC (the default, same as before), at the top above Home, or at the bottom below Network. It only sets SortOrderIndex in the private hive, so still no writes to the real registry.

## 1.0 ([Oct 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/06f757037dab9ee0cd3ed270f4546ed5cde55c02/mods/explorer-steam-games.wh.cpp))

Initial release.
