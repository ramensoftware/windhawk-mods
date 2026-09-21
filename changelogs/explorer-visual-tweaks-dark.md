## 1.0.1 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5185fb6ac35cff1f84cb447f83e649f9ce17cf0f/mods/explorer-visual-tweaks-dark.wh.cpp))

* Adds independent switches for file list selection, Navigation Pane customization, and progress indicators, alongside the existing Preview Pane switch.
* Organizes settings into named groups, with clearly labeled shared selection appearance settings.
* Disabled Selection and Progress blocks pass drawing calls through unchanged, without custom rendering or selection-state suppression.
* Applies Selection switches independently of graphics resource creation, so resource creation failures cannot prevent disabling a block.
* Adds guidance on disabling overlapping features when combining mods.

Preview Pane changes still require restarting Explorer and affected host applications.

Settings now use grouped keys. Previously customized values need to be reapplied after updating.

## 1.0.0 ([Sep 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0e6b56702ef4219f8b01cfce98cd8a4f7d4adcfb/mods/explorer-visual-tweaks-dark.wh.cpp))

Initial release.
