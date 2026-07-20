## 1.3 ([Jul 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/615068c3c4bf5f22dbd5f0ddd3af971c4ab9fac7/mods/explorer-force-details-columns.wh.cpp))

- Virtual folders with their own specialized columns (Recycle Bin, This PC, search results, etc.) are no longer affected by default; controlled by "Exclude virtual folders".
- Sorting can now be enforced with the "Sort By" / "Sort Descending" settings.
- Width -1 auto-sizes a column to its content.
- Column widths now update immediately when a window moves to a monitor with a different DPI.
- Switching an already open folder to Details view now applies the columns immediately.
- Changing settings now only refreshes windows whose view actually changed.

## 1.2 ([May 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ee00fe42ee1bf5e16382c2e2a8529336e86278ca/mods/explorer-force-details-columns.wh.cpp))

- Fixed the problem causing the settings to fallback to default during mod initiating.
- Refined width realization and DPI calculation, fixed the problem of minimum width being too wide.
- Duplicated property keys are handled only once if the user added same property multiple times in settings.
- Fixed some other problems.

## 1.1 ([Apr 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0e1772a08bdcdd2dbf87a949f7d1d2c136c2be57/mods/explorer-force-details-columns.wh.cpp))

- Width settings should be consistent now after changing monitors with different DPI.
- New changes take effect immediately now to all windows including opened ones.

## 1.0 ([Apr 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/188c9b7c5ac412d8f3fb43b53a7d4d1a0baa7792/mods/explorer-force-details-columns.wh.cpp))

Initial release.
