## 2.1 ([Oct 14, 2025](https://github.com/ramensoftware/windhawk-mods/blob/9f806706ffd5af8ef7970ba315b4999c42682c35/mods/explorer-double-f2-rename-extension.wh.cpp))

- allow swapping double/triple F2 behavior (extension and full name)
- added descriptors to settings
- now caching settings in memory
- now hooking keydown instead of keyup for faster and more consistent (with Explorer) response

## 2 ([Oct 12, 2025](https://github.com/ramensoftware/windhawk-mods/blob/4f96cfe6b97163c9d9c722d27f25e2cca04666f3/mods/explorer-double-f2-rename-extension.wh.cpp))

* Added triple F2 shortcut to select full name.
* Added cycling support to selection logic.
* Removed explicit desktop hooking (now handled via window enumeration).
* Internal refactor: separated API integration from core logic and encapsulated callbacks, timing, and selection for clarity.

## 1 ([Sep 24, 2025](https://github.com/ramensoftware/windhawk-mods/blob/a4e2bd7298358892902bf017b51a62de9037121b/mods/explorer-double-f2-rename-extension.wh.cpp))

Initial release.
