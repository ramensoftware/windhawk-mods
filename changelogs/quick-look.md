## 1.1.0 ([Nov 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1be2170523d417ed1db73f2b341e6e52d2de5867/mods/quick-look.wh.cpp))

* **Glass effects are supported.** With ExplorerBlurMica, DWMBlurGlass or similar, the panel follows
  the effect like the rest of the window, in dark and in light mode. Its text is drawn solid instead of
  being added to the blur, and an image preview is never tinted by what is behind Explorer. Without an
  effect the panel is drawn exactly as before.
* **Zoom and pan for images.** Double click an image to switch between fit and 100%, drag a zoomed
  image to pan it, and use the wheel (Shift for sideways) to scroll it.
* **Colors follow the Windows app mode.** The text color now depends only on whether Windows is light
  or dark, so it no longer turns gray or white from one preview to the next. The background stays
  Explorer's own surface. The accent still comes from Windows.
* **Archives show the shell's own view without a name filter.** A results folder cannot resolve the
  name of an item inside an archive, so the filter box is hidden there. Folders keep the filter.
* **Typing in the filter no longer opens an Explorer window.** The panel's own results view is no
  longer treated as a navigation, and a navigation outside the previewed folder is cancelled silently.
* **The filter shows the right items.** Only the matching direct children of the previewed folder are
  listed, with no duplicates and no stray file.
* **The icon in the properties view** is taken from a frame the shell did not pad, so it fills its box
  instead of sitting tiny in the corner.
* **A folder inside an archive** shows an empty date instead of the 1601 epoch.
* **Opaque images scale sharply.** Only an image with real transparency is blended; a photo or a
  screenshot is scaled with a crisp filter.

## 1.0.0 ([Nov 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a2c3b3bf6c8b6e52f87efbd3e6b977fcb23c3b25/mods/quick-look.wh.cpp))

Initial release.
