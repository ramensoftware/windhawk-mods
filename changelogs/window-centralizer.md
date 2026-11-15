## 1.2.0 ([Nov 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/55d92d44a3b17f6a8309e1acb201173dcdbca218/mods/window-centralizer.wh.cpp))

* Windows shown with `SetWindowPlacement` are now centered. Programs that restore a saved position this way (7-Zip's file manager) were left where they were, whatever the size limit.
* New option, **Center on the unrelated launcher window** (Parent mode only): a program started from another one, such as from Explorer's context menu or an archive opened from a folder, opens centered on the window it was started from (if inside the window size limit target). That window is the one in the foreground when the new process starts, and it must belong to the launching program (any Explorer process counts), so programs started from Start or the taskbar still open at the center of the monitor.
* Hotkeys now require Win, at least one more modifier (Ctrl, Alt or Shift) and a key; other combinations are rejected, so a hotkey such as Shift+letter can't catch ordinary typing.

## 1.1.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7a79ece1171c8f82c517e577bdcd2ac5466e63ed/mods/window-centralizer.wh.cpp))

* New toggle to show windows only after complete UI load, avoiding the native elements flicker of some interfaces.

## 1.0.0 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/877a07f2d63957f5bd9ce4329762e847731fdf8c/mods/window-centralizer.wh.cpp))

Initial release.
