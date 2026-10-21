## 1.0.1 ([Oct 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fa237ad2838057507501b6cb820b423eeda333ad/mods/winv-panel-tweaks.wh.cpp))

* Fixes the panel size setting having no effect above 100%: the limit Windows reports for the panel is now raised to the requested size.
* The panel never grows past the usable area of the screen it opens on.
* The mod now also hooks `TextInput.dll`, so the first start after installing or after a Windows update may take a moment while symbols download.

## 1.0.0 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d7afc202adc5a0b927f4fc78e67e1a3e4230afaa/mods/winv-panel-tweaks.wh.cpp))

Initial release.
