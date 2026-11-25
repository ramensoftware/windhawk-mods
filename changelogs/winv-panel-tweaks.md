## 1.1.0 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ae0ed6f714070b28fc188653813e667190c0fccf/mods/winv-panel-tweaks.wh.cpp))

* Adds a "Pinned" button next to "Clear all" that shows only your pinned clipboard items. Click it again to show everything. "Clear all" is hidden while only pinned items are shown.
* Adds a Win+Alt+V shortcut that opens the panel showing only pinned items.
* The button and the shortcut can be turned off or changed in the new "Pinned items" settings group.
* The shortcut runs in a small separate Windhawk process.

## 1.0.1 ([Oct 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fa237ad2838057507501b6cb820b423eeda333ad/mods/winv-panel-tweaks.wh.cpp))

* Fixes the panel size setting having no effect above 100%: the limit Windows reports for the panel is now raised to the requested size.
* The panel never grows past the usable area of the screen it opens on.
* The mod now also hooks `TextInput.dll`, so the first start after installing or after a Windows update may take a moment while symbols download.

## 1.0.0 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d7afc202adc5a0b927f4fc78e67e1a3e4230afaa/mods/winv-panel-tweaks.wh.cpp))

Initial release.
