## 1.1 ([Aug 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8be5d442cd3d958f58b91428d3cf9039536924bd/mods/windows-11-settings-styler.wh.cpp))

* Added the following themes: Translucent Settings11, WindowGlass, OLED (Modirinth Green), OLED (System Ascent).
* Updated the StoreFrame11 theme.
* Several target controls can now be specified for the same styles by separating them
with commas, for example: `ParentClass > Class#Name1, ParentClass >
Class#Name2`.
* Added string literals support for substitution expressions, for example: `` Text={{var == `a` ? `b` : `c`}} ``.
* Style variables now get the value from whichever capturing control is closest. Previously, the value was undefined if there was more than one control publishing the same variable.
* Improved handling for remote images.
* Fixed crashes that could occur in some cases during navigation.

## 1.0.1 ([Jul 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/44a6a55652401b83df91d79aa04c6091ddfbe55a/mods/windows-11-settings-styler.wh.cpp))

* Added the following themes: ClassicSearchBar, StoreFrame11, Blue.

## 1.0 ([Jun 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6ae9513b383f5e9a7870a5d22696203d4657bcec/mods/windows-11-settings-styler.wh.cpp))

Initial release.
