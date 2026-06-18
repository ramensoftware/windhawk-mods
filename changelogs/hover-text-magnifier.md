## 1.3.4 ([Jun 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c38ae45e176d59ab0176e9ae39de93e3bf1a8400/mods/hover-text-magnifier.wh.cpp))

- add shell32 linkage for CommandLineToArgvW
- pass immutable UIA request snapshots instead of reading live settings from the UIA thread
- reject stale UIA results by sequence, point, settings generation, and age
- remove the unload-time worker detach path and join the worker to avoid unloading the DLL under a live thread
- reduce repeated magnifier filter setup and non-erasing repaint invalidation

## 1.3.2 ([Jan 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/608636eae3c24e951825d546457ea9eb09050fe1/mods/hover-text-magnifier.wh.cpp))

Initial release.
