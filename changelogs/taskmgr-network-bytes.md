## 1.3 ([Nov 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/57b6069a465ecca327e085d3d0b2057fad533a43/mods/taskmgr-network-bytes.wh.cpp))

* Fixed: the mod failed to load when Microsoft's symbol server had no PDB for the installed Task Manager build. Added a byte-pattern fallback for all seven hooks.
* Improved: pattern matching anchors to function starts from the executable's `.pdata` table, requires an exact and unique match, and refuses ambiguous matches instead of hooking the wrong function.
* Symbol-based hooking is still tried first, and every hook remains optional. No settings changed.

## 1.1 ([Oct 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2995b1cd1f98d682ad5b13863523536d0bd58f6d/mods/taskmgr-network-bytes.wh.cpp))

Initial release.
