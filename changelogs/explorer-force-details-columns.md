## 1.5 ([Nov 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8a7c48cc3b4dd6c4ade9ac3a969cd35f36dc7dd8/mods/explorer-force-details-columns.wh.cpp))

* Added the "Minimum Column Width" setting, which lowers the minimum column width enforced by Windows (about 90 logical pixels), so that smaller forced widths take effect and columns can be dragged narrower.
* Fixed columns with width -1 being auto-sized before the folder's items were displayed, which made them only as wide as their header, or as narrow as the minimum width.
* Fixed a possible Explorer crash when settings are changed while a folder is opening.
* Changing settings no longer refreshes open folders, which reset their scroll position and selection, unless their columns or sorting change.
* Removed the note about the minimum column width known issue.
* Added a note that Explorer keeps the forced columns, widths and sorting of folders after the mod is disabled.

## 1.4 ([Aug 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/da5a7e59605f811c25d98fb3e8c399b34906272f/mods/explorer-force-details-columns.wh.cpp))

- Fixed normal folders sometimes being treated as virtual folders and left unchanged, which also affected their subfolders.
- Added a note about a known issue of Windows 11 using an unreasonable minimum column width when a non-custom scaling is set for the display instead of a custom scaling.
- Columns without "Force Width" now keep their current width instead of being reset when the set of columns changes.
- Forced widths are now also applied as the column's default and ideal width, so auto-sizing doesn't override them.

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
