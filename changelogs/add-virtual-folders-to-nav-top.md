## 1.3.0 ([Jun 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ca3145f0cb1e292fead3f7c74c2eda168c686799/mods/add-virtual-folders-to-nav-top.wh.cpp))

Changed the injection strategy to hook kernelbase!LoadLibraryExW instead of force-loading ExplorerFrame.dll in Wh_ModInit. Processes that don't use ExplorerFrame.dll no longer get it loaded at startup.

A user reported instability on 25H2 in #4280 (browser tabs crashing, Explorer becoming unresponsive after restart). I haven't been able to reproduce it, but the previous approach loaded ExplorerFrame.dll into every injected process at startup, which is unnecessarily aggressive. Deferring to the kernelbase hook (per @m417z's suggestion in #4159) avoids that entirely.

## 1.2.1 ([Jun 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/da0605c6e036ab19f40fecfa7a367a1084daa6e4/mods/add-virtual-folders-to-nav-top.wh.cpp))

Initial release.
