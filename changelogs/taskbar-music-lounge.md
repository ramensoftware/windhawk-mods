## 4.0.1 ([Mar 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/de1c1388c6aadf16637ba7ebe4051f310cb2605a/mods/taskbar-music-lounge.wh.cpp))

Fixes the widget appearing at top-left of screen on startup.

## 4.0 ([Mar 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fddd032c9c986776e98105654f3a7aa5fa3a43ec/mods/taskbar-music-lounge.wh.cpp))

This version addresses and incorporates multiple suggestions. A detailed diff table has been added to the README.

Addressed Issues & Fixes:
* [#3666395914](https://github.com/ramensoftware/windhawk-mods/pull/2746#issuecomment-3666395914) - Add AppID for compatibility with taskbar auto-hide mod
* [#3666527577](https://github.com/ramensoftware/windhawk-mods/pull/2746#issuecomment-3666527577) - Rounded corners, scalable icons for high-DPI, hide during fullscreen gaming
* [2eddf69](https://github.com/ramensoftware/windhawk-mods/commit/2eddf6953b7aaee9ad5eb050bb329e135f4c7798#commitcomment-173045991) - Fix widget lagging behind taskbar with smooth following
* [#3671818092](https://github.com/ramensoftware/windhawk-mods/pull/2746#issuecomment-3671818092) - Support for AIMP/MPC-BE players, larger 4K icons, fullscreen hide option
* [#2828](https://github.com/ramensoftware/windhawk-mods/issues/2828) - Event-driven positioning for smoother auto-hide and fullscreen sync
* [#3689038965](https://github.com/ramensoftware/windhawk-mods/pull/2746#issuecomment-3689038965) - Comprehensive rework fixing auto-hide lag, fullscreen detection, and high-DPI scaling
* [#3690241518](https://github.com/ramensoftware/windhawk-mods/pull/2746#issuecomment-3690241518) - Auto-hide widget when no media is playing

Credits:
Special thanks to @Cinabutts and @Deykii for code references and the community for testing.

Testing Note:
AIMP and MPC-BE integration is implemented but currently untested. Please report any issues with these players.

## 3.0 ([Dec 17, 2025](https://github.com/ramensoftware/windhawk-mods/blob/2eddf6953b7aaee9ad5eb050bb329e135f4c7798/mods/taskbar-music-lounge.wh.cpp))

Initial release.
