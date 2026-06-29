## 0.4.1 ([Jun 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1e8d5224d6c438602744da41174e6c0427d2f3da/mods/taskmgr-tab-slide-animation.wh.cpp))

Documentation-only fix: the two readme demo GIFs (crossfade and slide) were pointing at filenames whose cached copies on the image proxy showed the wrong animation. This points them at fresh `-v2` filenames so the correct crossfade and slide demos are shown. No code changes.

## 0.4.0 ([Jun 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a2dc42dae82c5bfdd6bebb8cef79b93f091b9664/mods/taskmgr-tab-slide-animation.wh.cpp))

This version adds a selectable animation style alongside the existing crossfade:

- **Animation type** setting - **Crossfade** (default, the previous behavior) or **Slide** (the old view slides out while the new one slides in; left/right, or up/down via *Slide vertically*).
- Both styles use the same no-visual-jump approach: the current view is frozen under a topmost, click-through overlay at mouse-down, so Task Manager switches the tab underneath it invisibly, and the animation always starts from the old frame.
- Robustness fix: if the window is moved or resized mid-animation, the overlay is now hidden immediately instead of lingering as a detached "ghost" copy at the old location.

The readme demo now shows both the crossfade and slide animations.

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
