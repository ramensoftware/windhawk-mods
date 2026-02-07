## 1.1.0 ([Feb 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/02e228b15957006012d2e47f825fb567b6e68361/mods/classic-min-max-animations.wh.cpp))

- Hook SystemParametersInfo to handle SPI_(GET|SET)ANIMATION
  - Enabling/disabling "Animate windows when minimizing and maximizing" with
    the mod enabled will now control the mod's animation, not the system's
- Call SystemParametersInfo to automatically disable the DWM window animations
  at init and to enable them at uninit if MinAnimate is still enabled

## 1.0.2 ([Oct 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/00c4f631eeebf04af95a6033d39d05a071fd67fc/mods/classic-min-max-animations.wh.cpp))

- Fix issues with per-monitor DPI-scaled windows (thanks @Ingan121)
- Properly clip animation for MDI windows (thanks @Ingan121)
- Properly destroy animation window and thread on mod uninitialization

## 1.0.1 ([Oct 13, 2025](https://github.com/ramensoftware/windhawk-mods/blob/f1f0d4e6ba11a02875e542aacd7cd3b164002b53/mods/classic-min-max-animations.wh.cpp))

- Remove title text from animation window to prevent it from showing up in Task Manager

## 1.0.0 ([Oct 13, 2025](https://github.com/ramensoftware/windhawk-mods/blob/63dbd5b8633b9a64ede8983e358347d3e0d17478/mods/classic-min-max-animations.wh.cpp))

Initial release.
