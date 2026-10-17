## 1.4.1 ([Oct 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/564e574c0812bbae205023f0b5ff4c6ed4a1a8f8/mods/shake-to-find-cursor.wh.cpp))

* Fixed flickering and stutter with larger pointer sizes (Settings > Accessibility > Mouse pointer and touch > Size). Thanks to @winchestercanal for tracking this down in #5595.
* Fixed the cursor briefly appearing at the wrong size when it changes type (e.g. arrow to text cursor) during or after the animation.
* Fixed the cursor not updating while the mouse is held still.

## 1.4 ([Oct 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7b37aa5a92ca5ae076d008a97eed67334d80458f/mods/shake-to-find-cursor.wh.cpp))

* Animated cursors (.ani) now grow and shrink smoothly instead of jumping straight to the final size, and keep animating while enlarged: the mod plays the animation itself, one frame at a time at the current size, and the real animated cursor comes back once the cursor has shrunk back. Fixes the issue reported by @winchestercanal in #5595.

## 1.3 ([Sep 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7cad60cca1a0a2ad2dd405bf9d4a4bea65a941bf/mods/shake-to-find-cursor.wh.cpp))

Initial release.
