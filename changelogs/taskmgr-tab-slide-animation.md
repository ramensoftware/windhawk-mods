## 0.3.3 ([Jun 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cc21e7ca1840bb5de5d5a8f2b08d27312555d29c/mods/taskmgr-tab-slide-animation.wh.cpp))

This version reworks the tab transition into a smooth crossfade between the old and new views — for both the main left navigation (Processes, Performance, App history, …) and the Performance sub-pages (CPU, Memory, Disk, Network/Ethernet, Wi-Fi, GPU).

To avoid a visible jump, on mouse-down the current view is frozen under a topmost, click-through layered overlay; Task Manager switches the tab underneath it invisibly, and a worker thread then locates the content edge via UI Automation, captures the new view once it has rendered, and crossfades the overlay from the old frame to the new one.

The demo GIF has been updated to show the new animation.

## 0.3.2 ([Jun 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/78b0296b65dab926d11dbc5f063d7314ae204aaf/mods/taskmgr-tab-slide-animation.wh.cpp))

Lowers the default **Change threshold** from 18% to 2%, so more subtle tab switches animate out of the box. The setting remains user-adjustable (1–100%).

## 0.3.1 ([Jun 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/abf43cf5f55c1d34f2ca6fcd7da315e8beaacd54/mods/taskmgr-tab-slide-animation.wh.cpp))

- **New "Change threshold" setting** (default 18%) with pixel-sampling comparison, so clicking a process row, selecting, or live-graph ticks no longer trigger a spurious slide.
- **Performance sub-pages** (CPU, Memory, Disk, Network/Ethernet, Wi-Fi, GPU) now slide too, not just the main left navigation.
- **Poll-until-rendered capture** of the new view (up to 6 attempts) so tabs that draw their first frame late (Disk/Network/GPU) are reliably animated.

## 0.2.0 ([Jun 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ca4b673744a580a088fd58c21a6b644dc7811636/mods/taskmgr-tab-slide-animation.wh.cpp))

Initial release.
