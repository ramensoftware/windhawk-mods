## 1.0.2 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1a497a6d23a99e14899d4c101d7e0f86ea5f457e/mods/altbinz-dark.wh.cpp))

* New setting "Title bar color" (default black, Windows 11 only): colors the window title bars of Alt.Binz. Leave it empty to keep the color of Windows.
* New setting "List text color" for the text and lines in the download queue and the log. Without it the text color is used.
* New setting "Column text color" for the text of the column headers.
* The text colors are applied live, without restarting Alt.Binz, and can now be dark (they were limited to at least 50% lightness before).

## 1.0.1 ([Oct 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2b6819a228e11153d60b8f32314caa053e7aa2a5/mods/altbinz-dark.wh.cpp))

* Faster window resizing: the per-pixel color mapping is cached, skin tinting uses a lookup table, and the temporary bitmap for blits is reused instead of being created for every blit (about 2.4 times faster in a resize benchmark, 333 ms to 140 ms per step).
* The classification of the calling module is cached per address.

## 1.0.0 ([Oct 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4028990b798d125c7ee88e143561bdb17b3eb643/mods/altbinz-dark.wh.cpp))

Initial release.
