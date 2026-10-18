## 3.2.1 ([Oct 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/47a5170a682da083cfc0bd7cbb10ac410ef79cf4/mods/remove-clock-cutout-background.wh.cpp))

* Fixed transparent PNG and GIF background bleeding by clearing the background with the parent color.
* Removed leftover window border lines (`WS_BORDER`) on clock controls.

## 3.2.0 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/036c1fb164596b01650218b075affec3bf65e88f/mods/remove-clock-cutout-background.wh.cpp))

* Added `customImagePath` setting to replace the default analog clock in StartAllBack and Date/Time dialogs with a custom static image (PNG, JPEG, BMP) or animated GIF.
* Implemented safe memory-stream image loading to prevent file locking while the mod is enabled.
* Added robust GDI+ lifecycle management during process shutdown.

## 3.1.0 ([Oct 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4e183d31eb36e9e1eb2dc144291f6e94d5981daa/mods/remove-clock-cutout-background.wh.cpp))

Initial release.
