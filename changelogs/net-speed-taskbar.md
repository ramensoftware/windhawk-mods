## 1.7 ([Nov 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/806a8f3b7d33ca19ccf588fa38ed0281042f1f78/mods/net-speed-taskbar.wh.cpp))

* Added custom font family setting (`fontFamily`) with fallback to Segoe UI.
* Added bold text styling toggle (`isBold`).
* Added custom HEX color settings (`textColor` and `uploadColor`).
* Added dynamic text measurement to expand widget width and prevent font clipping.
* Disabled GDI+ word wrapping to avoid accidental multiline breaks.
* Clamped widget height and vertical offset to ensure the meter never overflows the taskbar.
* Added safe clamps to font size (6–36), vertical nudge, and update interval.
* Updated demo GIF.

## 1.6 ([Sep 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e7faca32a6b7a3166a709ac824ec060b54070fc6/mods/net-speed-taskbar.wh.cpp))

Initial release.
