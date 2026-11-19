## 1.1.1 ([Nov 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/065be613a1ee59d4b039f1e93a0b8bdaf60808a2/mods/gpu-accelerated-explorer-view.wh.cpp))

### New
- **Custom themes:** Customize backgrounds, gradients, borders, scrollbars, selection effects, tooltips, and storage bars, with light and dark mode support. Check the guide for details and examples. 
- **More layouts:** Added Tiles, Content, and Compact views.
- **Better renaming:** Batch rename files, click to rename, and automatically rename newly created items.
- **Expanded folder support:** Libraries, search results, Applications, and Printers now use the GPU-rendered view.
- **Smarter search:** Highlight matching filenames and smoothly update results as you refine searches.
- **Edge scrolling:** Automatically scroll when dragging files toward the edge of a folder.

### Improved
- **Better file interactions:** More natural dragging, keyboard navigation, sorting, and folder-size updates.
- **Improved cloud support:** Faster OneDrive status updates and correctly displayed sync icons (PR @dariy-vel)

### Fixed
- Fixed blank views, navigation glitches, and an Explorer crash when using Content view across multiple windows.
- Fixed renaming issues, including flickering, batch renaming, and international text input.
- Fixed incorrect file sizes, sorting, and display issues on removable drives and in Libraries.
- Fixed various animation, touch, selection, and display-scaling issues.

## 1.0.4 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5a52fd297ff23b70877e27fb8d5860b53c3e5002/mods/gpu-accelerated-explorer-view.wh.cpp))

* Added full touchscreen support, including scrolling, pinch-to-zoom, gestures, and drag-and-drop.
* Added mouse and touch shortcuts: double-click empty space to go up a folder, middle-click folders to open new tabs, and two-finger tap to open context menus.
* Improved file tooltips with detailed metadata, including file sizes, dates, dimensions, and localized relative dates.
* Added selection checkboxes, including a Select All checkbox.
* Redesigned drag-and-drop previews with stacked icons, thumbnails, and smooth animations.
* Added support for native Windows 11 context menus.
* Added persistent folder sorting, synchronized with Explorer's native view.
* Fixed blurry interface elements and inconsistent DPI scaling.
* Fixed cloud, network, and virtual folders incorrectly appearing empty.
* Fixed folders occasionally failing to open or losing selections.
* Fixed Select All occasionally clearing the selection instead.
* Fixed missing icons and incorrect shortcut or cloud status overlays.
* Fixed flickering during window resizing, file renaming, and folder navigation.
* Fixed Explorer's native view not restoring correctly when switching views.

## 0.9.240 ([Oct 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6937b85477825dcbee84bf84d05718f0e68436dd/mods/gpu-accelerated-explorer-view.wh.cpp))

Initial release.
