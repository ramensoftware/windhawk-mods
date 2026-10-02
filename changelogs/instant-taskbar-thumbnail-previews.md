## 1.1.0 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0796806625b29f982264f16488f7e9b4d9f956dc/mods/instant-taskbar-thumbnail-previews.wh.cpp))

* New "Delay after thumbnail removal" setting (default 500 ms). When a thumbnail is removed and others remain (close button, middle-click, keyboard, or the window closing externally), the remaining previews stay open for this long. The normal close delay applies when the last thumbnail is removed.
* The default "Thumbnail close delay" is now 200 ms instead of 1 ms. I find this provides optimal expected behavior; set it to 1 for the previous behavior.
* Fixed hook installation when the taskbar loads after the mod.
* Added a demo to the README.

## 1.0.0 ([Jul 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/06f19979b14e80d76a5ca4508def5ec55767a251/mods/instant-taskbar-thumbnail-previews.wh.cpp))

Initial release.
