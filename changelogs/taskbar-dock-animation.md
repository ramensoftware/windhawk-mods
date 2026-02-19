## 1.9.2 ([Feb 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d23ad55f18703e56934a130555304dbdb78b4dee/mods/taskbar-dock-animation.wh.cpp))

Added an option to disable animation for system taskbar buttons (Start, Widgets, Task View, Search).

Added multilingual support.

Added a smoothing (lerp) setting for the animation (not to be confused with the animation curve types).

Added dirty-checking to prevent expensive repeated scans of the Windows UI element tree.

## 1.7 ([Dec 30, 2025](https://github.com/ramensoftware/windhawk-mods/blob/e02578f8da0413422830baa6b5c0934d6446b1f4/mods/taskbar-dock-animation.wh.cpp))

* Expanded Taskbar Support: Added support for vertical and top-aligned taskbars.
* Focus Animation: Added smooth fade-in/fade-out focus animation.
* Scale Range: Adjusted customization limits (Min: 101%, Max: 220%).
* Animation Curves: Added a selection of 3 animation curves (Cosine, Linear, Exponential).
* Labels Mode: Added support for "Taskbar Labels" (Icon + Text mode). Please report any visual glitches.
* Optimization: General code cleanup and performance improvements.

## 1.1 ([Nov 16, 2025](https://github.com/ramensoftware/windhawk-mods/blob/cbcbcc57ce2db3350ff804cf7bbdbee5d9c729d3/mods/taskbar-dock-animation.wh.cpp))

Initial release.
