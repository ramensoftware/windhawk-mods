## 1.1 ([Nov 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a44144776438bb944427d5cb39f50f57eafe74ce/mods/desktop-snowfall.wh.cpp))

* New "Where the snow falls" setting: over all windows, or behind them on the desktop. Behind windows, the falling snow shows only where the desktop is uncovered, and the snow still settles on top of the windows and the taskbar.
* Much lower power use. Calm snowfall is drawn at about 30 fps, and the full frame rate is used only while something moves. Snow that lies still isn't redrawn. The frame rate is capped on battery and lowered in energy saver mode. The snow pauses while the screen is locked or off.
* Each monitor gets its own snow, sized for its display scale. A fullscreen app hides the snow only on its own monitor.
* Shake a window to throw its snow off. Closing or minimizing a window drops its snow in lumps that burst into powder.
* Snow piles up along the bottom of the screen where the taskbar doesn't cover it (new setting, on by default).
* New "Snow slowly melts" setting.
* New "Hide from screenshots and screen sharing" setting.
* Menus, tooltips and the shell's flyouts now stay in front of the snow, and popup windows hide the snow lying behind them.

## 1.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/be5856fcd480efbfe0843219dea54843673763fe/mods/desktop-snowfall.wh.cpp))

Initial release.
