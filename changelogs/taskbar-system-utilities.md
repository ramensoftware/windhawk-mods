## 1.1 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/86a896f7ddd20f2bf60d0798e487701f009f6595/mods/taskbar-system-utilities.wh.cpp))

- Added **Reduce Memory** tool to trim working sets of non-elevated processes.
- Added **Rebuild Icon & Thumbnail Cache** tool (`thumbcache_*.db` cleanup + `ie4uinit.exe -show`).
- Added optional **Public IP** lookup in Get IP Address (`getIp.queryPublicIp`, disabled by default for privacy).
- Reorganized settings into structured groups for better UX (**Note:** existing settings are reset to defaults due to the new grouped key structure and need to be reconfigured).
- Filtered out non-preferred and APIPA (`169.254.x.x`) local network addresses.

## 1.0 ([Nov 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/48e23bd6b1d7ceb1fb47f261ffdd36ec211a7dd3/mods/taskbar-system-utilities.wh.cpp))

Initial release.
