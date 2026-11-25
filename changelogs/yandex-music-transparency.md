## 1.2.8 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5b2330146733bd5c376685c8caa8e90b84920731/mods/yandex-music-transparency.wh.cpp))

- Added configurable opacity for active and inactive windows
  (`Alpha` / `InactiveAlpha`).
- Added "fully opaque on hover" option (`OpaqueOnHover`).
- Added "fully opaque when maximized/fullscreen" option
  (`OpaqueWhenMaximized`), applied immediately to avoid clashing with the
  system resize animation.
- Added smooth animated transitions between opacity levels
  (`SmoothTransitions`).
- Defaults: active opacity 220, inactive opacity 170 (inactive windows are now
  more transparent than the active value), smooth transitions on, fully
  opaque on hover off.
- Fixed restoring the original alpha on unload (the window's true original
  opacity is now captured once).

## 1.1 ([Nov 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8f4eaeef62ea9804e81a529e792d7423b45d5c04/mods/yandex-music-transparency.wh.cpp))

Initial release.
