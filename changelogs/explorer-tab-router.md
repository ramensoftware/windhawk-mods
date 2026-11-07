## 1.1.2 ([Nov 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6a2eda932f27a788ca5627e4e817b0a6f9f9c568/mods/explorer-tab-router.wh.cpp))

* Fix brief switches through unrelated tabs when activating an existing folder tab from an external open or file-reveal request.
* Resolve the exact target HWND on the owning frame thread and issue one verified selection command, including after tab reordering.
* Remove sequential ordinal probing; retain file selection and fail safely if direct activation is unavailable.

## 1.1.1 ([Nov 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/62118eece92a009ea27315a1ba22c68555709bd9/mods/explorer-tab-router.wh.cpp))

Initial release.
