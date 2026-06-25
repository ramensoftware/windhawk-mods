## 0.3.1 ([Jun 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/abf43cf5f55c1d34f2ca6fcd7da315e8beaacd54/mods/taskmgr-tab-slide-animation.wh.cpp))

- **New "Change threshold" setting** (default 18%) with pixel-sampling comparison, so clicking a process row, selecting, or live-graph ticks no longer trigger a spurious slide.
- **Performance sub-pages** (CPU, Memory, Disk, Network/Ethernet, Wi-Fi, GPU) now slide too, not just the main left navigation.
- **Poll-until-rendered capture** of the new view (up to 6 attempts) so tabs that draw their first frame late (Disk/Network/GPU) are reliably animated.

## 0.2.0 ([Jun 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ca4b673744a580a088fd58c21a6b644dc7811636/mods/taskmgr-tab-slide-animation.wh.cpp))

Initial release.
