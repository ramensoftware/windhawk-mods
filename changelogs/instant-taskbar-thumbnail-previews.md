## 1.2.0 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bd41dcabf387671a682e2083042fdfd54188ecca/mods/instant-taskbar-thumbnail-previews.wh.cpp))

* Added "aim boundary" feature.
  * Off by default. While thumbnail previews are open and the pointer moves from their taskbar button toward them, crossing other taskbar buttons no longer switches the previews, and leaving the taskbar no longer starts closing them. 
  * A movement counts as heading toward the previews when its direction lies between two boundary lines: one from the button's left edge to the previews' bottom-left corner, and one from the button's right edge to the previews' bottom-right corner.
  * Hover changes held back during the movement are applied in order once the pointer reaches the previews, heads outside the boundary lines, or a mouse button is pressed. Buttons that were only crossed on the way are skipped.
  * "Aim boundary height on the button (%)" setting, default 100. It sets where the boundary lines meet the button's edges, measured down from the button's top edge (0 is the top edge, 100 the bottom edge).
  * The aim boundary feature doesn't work with vertical taskbars.
* Cleaned up README section

## 1.1.0 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0796806625b29f982264f16488f7e9b4d9f956dc/mods/instant-taskbar-thumbnail-previews.wh.cpp))

* New "Delay after thumbnail removal" setting (default 500 ms). When a thumbnail is removed and others remain (close button, middle-click, keyboard, or the window closing externally), the remaining previews stay open for this long. The normal close delay applies when the last thumbnail is removed.
* The default "Thumbnail close delay" is now 200 ms instead of 1 ms. I find this provides optimal expected behavior; set it to 1 for the previous behavior.
* Fixed hook installation when the taskbar loads after the mod.
* Added a demo to the README.

## 1.0.0 ([Jul 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/06f19979b14e80d76a5ca4508def5ec55767a251/mods/instant-taskbar-thumbnail-previews.wh.cpp))

Initial release.
