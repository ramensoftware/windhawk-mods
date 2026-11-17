## 1.0.2 ([Nov 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8f087f9652d831e96376083d1f795e987f0c2662/mods/taskbar-ai-usage.wh.cpp))

* Claude rate limits (HTTP 429): the first two without a usable `Retry-After` retry at the selected interval, then the wait grows in five-minute steps up to 15 minutes. Waits sent by the server are honored, the streak survives reloads, and a successful reading resets it.
* Default flyout material in dark mode: the wallpaper color is now blended over a dark base, so the card keeps a steady dark tone as the wallpaper changes. Light mode is unchanged.
* The "+" setup tile has an accessible name ("Add an AI account").
* Removed dead code, unused strings and legacy paths.
* Fixed a Spanish string.

## 1.0.1 ([Nov 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f4350f743d2adcf3fa2efe4c869ad6d9feaf0126/mods/taskbar-ai-usage.wh.cpp))

Initial release.
