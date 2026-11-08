## 4.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bb165c126f8428fa76189e2ae85861359379e2cb/mods/brave-acrylic.wh.cpp))

* Add independent Toolbar and Address bar settings. The Address bar option is on by default and removes Brave’s hover shadow on supported builds. If you previously disabled Toolbar to keep the address field opaque, turn Address bar off separately.
* Enable Hide duplicate Windows buttons by default on fresh installs so affected users get a single set of controls without another setup step. The option can affect Alt+Space and taskbar menus. Turn it off to restore the original Windows window style.
* Clear the backing around rounded webpage content while preserving its clipping and thin outline.
* Add acrylic to Site information, Recent downloads and Media controls. Clear the download quick-action strip while preserving its controls.
* Add adjustable popup tint opacity and use a short matching fade for supported bubbles and their backdrop.
* Recognize Chromium 157’s virtual popup opacity setter and check for new stable Brave builds every hour.
* Add an opt-in fallback to the official Brave symbol server. Windhawk checks the small cache first and can download the full PDB if needed. The fallback is off by default.

## 3.9 ([Nov 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/74caff8729dc6a0ea6c843341bd56ae1479ca2dd/mods/brave-acrylic.wh.cpp))

Initial release.
