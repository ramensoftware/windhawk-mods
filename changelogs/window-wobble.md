## 1.0.1 ([Nov 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/14b4d0fca3efb5f5ae23a9f898f97aa60a9ccb4d/mods/window-wobble.wh.cpp))

* Fixed the window stretching off screen and disappearing for a moment when the mouse button was released while the window was still moving. The settle animation was numerically unstable for frame times longer than 7.8 ms, so it affected 60 Hz and 120 Hz displays and any frame hitch. It is now sub-stepped and stable at any refresh rate.

## 1.0.0 ([Oct 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9674ac4e7339530fdd264f1ee8424e8afabc249f/mods/window-wobble.wh.cpp))

Initial release.
