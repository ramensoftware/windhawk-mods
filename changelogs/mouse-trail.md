## 3.4.5.3 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8630d6bf9b605f1cd3dff532975d62a8deeec28f/mods/mouse-trail.wh.cpp))

- Add：4 new color modes with optimizations to existing modes
- Add：3 new trail shape types
- Add：More click feedback effects
- Change：Refactored API abstraction layer for D3D11/D2D fallback, reduced maintenance cost
- Change：Improved particle cursor attraction — particles now follow the trail path toward the cursor
- Change：Improved physics clustering behavior
- Change：Improved performance mode visibility
- Fix：Color overflow issue
- Fix：Trail delay offset error
- Fix：Bezier smoothing not taking effect

## 3.4.4.0 ([Sep 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bfcbf010b49005e0b230d4dee351ccc4975d1143/mods/mouse-trail.wh.cpp))

- Change：Reorganized settings into collapsible groups (will reset to defaults)
- Fix：10 shader issues
- Fix：Particle mode accidentally spawning particles due to aggressive compensation mechanism
- Change：Improved debug rendering
- Change：Improved vortex generation, merging, and spawning frequency around cursor

## 3.4.2.2 ([Sep 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/78e72a6fb982d2956daee2c013a1b355854ef9a1/mods/mouse-trail.wh.cpp))

- Add：Frame sync toggle for tear-free presentation
- Add：Trail stroke with configurable width
- Add：Particle stroke with configurable width, alpha and color
- Add：Device loss recovery (GPU TDR, driver update, sleep/resume)
- Add：Global hotkeys for trail, particles and master toggle
- Fix：Overlay window z-order after system restart
- Fix：Settings hot-reload callback registration

## 3.4.1 ([Sep 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/661af3e4a08ace7fd48652ca56e1a7d7df712da4/mods/mouse-trail.wh.cpp))

Initial release.
