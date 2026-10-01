## 0.9.4 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9a06ff2e9d46c9745918d9b52743e6f2f87077d9/mods/native-shadow-tuner.wh.cpp))

## Windows Shadows Tuner 0.9.4

### New
- Dynamic shadows (experimental): each window's shadow fades based on its
  distance from a virtual light source, updating live while windows are
  dragged. Settings:
  - Light position: four screen edges (top, bottom, left, right), four
    corners, top/bottom center or middle left/right.
  - Dimming strength: how much the shadow fades.
  - Fade distance: how far from the light the shadow reaches its minimum.
  - Fade curve: linear, fast, smooth or late.

### Safety
- Dynamic shadows read four internal uDWM field offsets. At startup the mod
  checks the uDWM code that uses them and, if anything doesn't match, disables
  only dynamic shadows and keeps the intensity and size controls working.
- When the mod is unloaded, every shadow is reset to full opacity on DWM's own
  thread before the hooks are removed.

### Other changes
- Removed per-call logging from the shadow parameters hook.
- README: documented all settings and limitations, added a demo GIF, and
  corrected the note about stored data (the mod stores the time of its last
  initialization for crash loop detection).

### Testing
- Tested with uDWM.dll 10.0.26100.9549 on Windows build 26300.9550,
  Windhawk 1.7.3.
- The uDWM symbols and field offsets used by dynamic shadows were also
  verified by disassembly on uDWM.dll 10.0.26100.9278.

## 0.6.4 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5ba5889e229443d62d6b13a03419c11dcbd6e01c/mods/native-shadow-tuner.wh.cpp))

Initial release.
