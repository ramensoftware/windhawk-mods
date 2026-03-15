## 2.0.0 ([Mar 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b25cbfb1fa555527b71e6c68a5ddedd4816e51b5/mods/tiling-helper.wh.cpp))

* Reworked Tiling Helper around a simpler always-on `Master+Stack` workflow. After tiling a monitor, windows stay managed until automatic tiling is paused.
* Dragging a tiled window now untile it and removes it from the current layout, while the remaining tiled windows are rearranged automatically.
* Removed layout cycling and the extra layouts, and focused the settings on a single configurable `Master+Stack` layout with horizontal/vertical orientation and adjustable master size.
* Updated hotkeys and controls: the retile toggle now pauses/resumes automatic tiling without disabling manual tiling, and a new shortcut can switch which window is the master.

Credit to @Meteony for the majority of code changes in this release.

## 1.0.0 ([Feb 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/85e4a6e8b5ce29cc1c26d9da6f343b03e20927ae/mods/tiling-helper.wh.cpp))

Initial release.
