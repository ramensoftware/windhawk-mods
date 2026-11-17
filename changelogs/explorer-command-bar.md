## 1.3.0 ([Nov 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/96f8ea0ad98a9612d5e0e5aa24f7ed7e8ca757df/mods/explorer-command-bar.wh.cpp))

- New
- Drag and drop to reorder items in the settings window preview: your own buttons and menus, and File Explorer's built-in buttons (New, Cut, Copy, Paste, Rename, Share, Delete, Sort, View) with their separators
- Drag and drop to reorder the entries of a menu
- Reset order button that brings back File Explorer's own order of the built-in buttons
- Built-in button order setting, e.g. `sort, view | new | cut, copy, paste, rename, share, delete`
- New placeholders: `%dir%`, `%files%`, `%folders%`, `%name%`, `%base%`, `%file_name%`, `%1%`, `%ext%`, `%count%`, `%d_name%`, `%d_smart%`, `%parent%` and more
- Live labels: button and menu entry labels can use `%name%`, `%base%`, `%ext%`, `%count%`, `%d_name%` and `%path%`, and update as the selection changes
- File extension filter setting to show a button only for chosen selections, e.g. `only txt`, `files + single` or `-zip`
- Setting to gray out buttons that don't match the filter instead of hiding them
- Run as administrator and Window style (normal, minimized, maximized, hidden) options for buttons and menu entries
- Ctrl+Shift+click runs any item as administrator
- Internal commands: `internal:CopyPath`, `internal:CopyName`, `internal:CopyDir`, `internal:NewFolder`, `internal:NewFile` and `internal:Refresh`
- Hide built-in button labels settings for New, Sort, View and the Details pane button
- Show chevrons on dropdowns setting for custom menu buttons

- Changed
- `%sel_each%` fills in the placeholders again for each selected item
- Long labels are shortened, with the full text in the tooltip
- Dropdown chevrons use the Fluent chevron instead of the ▾ arrow
- Buttons with no icon get a fallback icon, and the new internal commands have default icons
- Readme and setting descriptions updated with the new features

- Fixed
- A placeholder with no value no longer drops all parameters
- Commands started in locations that aren't folders on disk, such as This PC, now get a valid working directory

## 1.2.0 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8b1dc4b804cb693384afa25f56c06c120cf78492/mods/explorer-command-bar.wh.cpp))

### New
- Settings window: a cog button in the command bar opens a Windows 11 style settings window with a live preview of the command bar
- Toolbar items and menus are edited as a tree, with pickers for programs, icons, internal commands and placeholders
- Import and export settings as YAML or JSON
- Show Explorer Command Bar Settings Item and Settings Item Order settings
- Keyboard shortcuts for buttons and menu entries, e.g. `Ctrl+Shift+T`,`Alt+N` or `F9`
- Internal commands: `internal:TogglePreview`, `internal:ToggleDetails`,`internal:OpenWith` and `internal:FolderOptions` (`int:` also works)
- `%sel_each%` placeholder runs the command per selected item (up to 20)
- Show label and Custom Label Text settings show text next to a button's icon
- Place buttons on the right side setting docks the buttons next to the Details/Preview toggles
- Theme-aware icons with the `Light | Dark` syntax, e.g. `E706 | E708`
- Icons reload automatically when the Windows theme changes

### Changed
- `%sel%` now expands to all selected items, each in quotes, instead of only the first one
- Every setting now has a description
- Readme updated with the new features

### Fixed
- Buttons now appear in File Explorer windows that were already open when the mod loaded, without opening a new tab or navigating first
- Other bug fixes.

## 1.1.0 ([Aug 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/81a25073317418e4daedb4eba756326a9587dffc/mods/explorer-command-bar.wh.cpp))

- Add the New+ feature from PowerToys to Explorer Command Bar, with possibility to replace the default explorer's New.
- Add an optional command bar button for the selected item’s or current folder’s shell context menu, including shell extensions, nested menus, extended verbs, and Nilesoft Shell support.
- Add nested custom menus, improved icon handling, configurable button spacing, and more reliable command bar refreshes.
- Prevent crashes when unloading the mod by revoking XAML event handlers and safely cleaning up menus, timers, hooks, and other per-thread resources.
- Add Windhawk 2.0 settings compatibility for toolbar items without subitems.
- Bug fixes.

## 1.0.0 ([Jul 31, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9dc2bcf409eb82f0361d261ca6353d1ad0d60923/mods/explorer-command-bar.wh.cpp))

Initial release.
