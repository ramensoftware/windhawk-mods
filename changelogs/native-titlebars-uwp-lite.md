## 2.1.0 ([Nov 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a4f6f237f3826e0cfe1bf568ba89aa7905c88727/mods/native-titlebars-uwp-lite.wh.cpp))

* [Merge changes by user o12r](https://github.com/ramensoftware/windhawk-mods/pull/7682) (show maximize and minimize buttons when the theme is active):

Fixes missing caption buttons when using this mod (only tested on Windows 11 25H2, build 26200). With 2.0.1 the native titlebar shows title and icon but no minimize/maximize/close.

### Root cause
1. **All buttons hidden:** ApplicationFrameHost calls `SetWindowThemeAttribute(WTA_NONCLIENT)` on the frame with flags `0x10007` / mask `0x1000F`. The undocumented bit `0x10000` makes DWM drop all caption buttons (stock UWP frames draw their own). Logged in AFH: DWM caption-button width goes from 49 → 0 on exactly that call.
2. **Only close returns after fixing (1):** frames are created with `CreateWindowInBandEx` type flags `0x3`. Bit `0x1` makes the window "immersive", and uDWM's `CTopLevelWindow::GetCurrentStyle` adds close but skips minimize/maximize when `CWindowData::IsImmersiveWindow()` is true. Changing the type flags in AFH is not an option: clearing either bit stops the app's `CoreWindow` from being hosted in the frame (tested all combinations).

### Changes (v2.1.0)
- **AFH:** hook `SetWindowThemeAttribute` and strip `0x10000` for `ApplicationFrameWindow`; on load, clear it on frames this mod already converted.
- **AFH:** `WM_NCHITTEST` / NC button messages for `HTMINBUTTON`/`HTMAXBUTTON`/`HTCLOSE` go to `DefWindowProc`, so hit-testing, hover and clicks follow DWM's actual button rects instead of the UWP layout.
- **dwm.exe** (new `@include`): hook `CTopLevelWindow::GetCurrentStyle` and add the min/max `NCAREA_FLAGS` bits (`0x100|0x200` + enabled `0x1000`/`0x2000`, same as the non-immersive path) only when the window already has a close button, is immersive, and has `WS_CAPTION|WS_SYSMENU` + min/max boxes. Stock UWP frames keep `0x10000`, never get the close bit, and are unaffected.

### Notes
- The dwm.exe part reads the style from `CWindowData+0x74` (as `GetCurrentStyle` does on uDWM 10.0.26100). It is sanity-checked, so on other builds it should be a no-op (close button only) rather than misbehave.
- Tested with OpenGlass (Win7 theme) and with OpenGlass stopped: Calculator and Settings show all three buttons, content is hosted normally, buttons respond.

## 2.0.1 ([Oct 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/90e067381451a3a716f239a37f3c0191f4c601a1/mods/native-titlebars-uwp-lite.wh.cpp))

* Update screenshot

## 2.0 ([Oct 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f2a007255a1fa993638d0b9f7130f3d1fafea1c9/mods/native-titlebars-uwp-lite.wh.cpp))

* Use another method
* Remove black lines under titlebars
* Improve compatibility with Windows 11 23H2 and Windows 10
* Improve compatibility with themed setups

## 1.4.0 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a4c44e19cf76a635b3ecddac529fcdf1e83615ea/mods/native-titlebars-uwp-lite.wh.cpp))

* Improve compatibility with 26H2

## 1.3.0 ([Sep 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/196fcdb727a79e52a624264506fdd1928e4ec565/mods/native-titlebars-uwp-lite.wh.cpp))

* Fix immersive Alt-Tab switcher

## 1.2.1 ([Jul 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a6331a0a1240ac02de1b2e73e2109755bf8c816d/mods/native-titlebars-uwp-lite.wh.cpp))

Updated version number and screenshot in the mod description.

## 1.2 ([Jul 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c3cfb1e122539801e373fb76a7c6c228ff6eaf30/mods/native-titlebars-uwp-lite.wh.cpp))

Improve the icon appearance and fix a bug when the icons were confused when two UWP apps were opened at the same time.

## 1.1.0 ([May 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/16941c0526e84fd525aae8bccb265ad89e2168e0/mods/native-titlebars-uwp-lite.wh.cpp))

* Add support for proper icons in titlebars

* Change screenshot link in native-titlebars mod

Updated the screenshot link in the mod readme.

* Change screenshot URL in native-titlebars mod

Updated the screenshot URL in the mod readme.

## 1.0.0 ([Mar 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f2218777d0edd317dcebe26e444176231202e103/mods/native-titlebars-uwp-lite.wh.cpp))

Initial release.
