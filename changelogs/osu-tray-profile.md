## 3.9.3 ([Sep 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6502faca91d032111ad13180057004cd3d289ace/mods/osu-tray-profile.wh.cpp))

* Set custom "User-Agent" (`osu-tray-profile/3.9.3`) to prevent Cloudflare blocks.
* Reverted "Content-Type" to `application/x-www-form-urlencoded` for token requests, as Cloudflare was rejecting `application/json`.
* Added URL encoding for spaces in usernames (`%20`) to correctly format GET requests to the API.
* Improved JSON parsing to safely handle `null` values (e.g. for players without a global rank).
* Increased the retry timeout on error to 60 seconds to avoid temporary IP bans for spamming requests.

## 3.9.2 ([May 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/681c473a5d25049e49fc88918ef94e51757e8d1d/mods/osu-tray-profile.wh.cpp))

* Fixed the display of information when opening startup on Windows 11. (@EmanDev, [#3945](https://github.com/ramensoftware/windhawk-mods/issues/3945))
* Fixed a bug where the widget might not appear on the taskbar the first time its launched, but only after a few times.

## 3.9.1 ([Apr 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/df4a9446ecc3ecb93ff8848e8d0d6aa8cc3c2605/mods/osu-tray-profile.wh.cpp))

* After restarting the computer, the mod displayed an "error" message, which was resolved by "enabling/disabling" the mod. Now, after restarting the PC, the mod works correctly.

## 3.9 ([Apr 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4355b0e4bb076792f8a6ea54649321a23b8f2856/mods/osu-tray-profile.wh.cpp))

Initial release.
