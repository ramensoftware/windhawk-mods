## 1.3.0 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e8116310d764553d91ec584d0d34f2acbc9b0c28/mods/win81-start-screen-recreation.wh.cpp))

- Added best-effort live tiles
- Added support for up to eight custom tiles, each with a target, color, icon, and separate images for the four tile sizes
- Added best-effort touchscreen support (not tested by the author due to hardware limitations)
- Reintroduced the optional setting to sample desktop tile colors from application icons (Color sampling is best-effort)
- Added best-effort integration with Windows 8.1 Taskbar Recreation: when detected on the current monitor, it is prioritized on bottom-edge hover. The native taskbar remains the fallback when the companion is not detected
- Preserved the existing layout format. Custom images are read from local paths and are not copied by the mod
- Standardized and translated the new settings’ descriptions
- Applied minor graphic improvements

## 1.2.0 ([Nov 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/208aff70c74897d4deff49304a06090ca069e814/mods/win81-start-screen-recreation.wh.cpp))

- Improved the context menus to match the Windows 8/8.1 interface
- Improved the animations to be more accurate to the original Windows 8.1
- Improved the arrow that switches from "All apps" to Start (and viceversa)
- Implemented a best-effort mechanism to display the taskbar when hovering at the bottom of the screen, replicating the behavior introduced in Windows 8.1 Update 1
- Applied minor improvements to the tile logic

## 1.1.0 ([Oct 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bdb2b10f1c0ee2e8a636ed05244e6ed0d846cad5/mods/win81-start-screen-recreation.wh.cpp))

- Improved the compatibility with StartAllBack (best-effort)
- Improved the graphics of the tiles
- Improved the animations of the Start Screen
- Fixed a defect with the search charm 
- Fixed a minor defect with the names of the tiles going off-screen
- Improved the search's algorithm 
- Improved the animation of the search charm

## 1.0.0 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5f736702f73799aea88466b725d4aa031003066b/mods/win81-start-screen-recreation.wh.cpp))

Initial release.
