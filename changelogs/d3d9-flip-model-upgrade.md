## 1.1 ([Oct 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/feb623294a194eecdb29c8459f4bdcd941466458/mods/d3d9-flip-model-upgrade.wh.cpp))

* Fixed the flip model conflicting with in-game overlays, such as the Steam overlay, which made games render incorrectly.
* Fixed managed vertex and index buffers not working with software vertex processing.
* Fixed the borderless conversion blocking on the thread the window belongs to.
* Added the SafeHook option, for games whose anti-tamper protection hangs or closes them with the mod enabled. It detects d3d9.dll loading with a Windows DLL load notification instead of hooking `LoadLibraryExW` in kernelbase.dll.

## 1.0 ([Sep 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/68ea89198430663374733ef5f9ce360c04ef5695/mods/d3d9-flip-model-upgrade.wh.cpp))

Initial release.
