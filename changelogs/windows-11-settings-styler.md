## 1.2.1 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ecb4b86d0e225b72c8b04ba636c260554129fefa/mods/windows-11-settings-styler.wh.cpp))

* Fixed a regression that causes a crash when navigating to some pages such as Personalization > Background.
* Improved navigation handling that fixes some minor quirks such as the Printers view not being centered and the sidebar losing its background effect.

## 1.2 ([Oct 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7986323bd6b7c28686248b4347550ea9df58d723/mods/windows-11-settings-styler.wh.cpp))

* Added the Translucent Seamless theme.
* Fixed the mod not detecting a newly created Settings window in some cases, requiring to re-enable the mod to apply the styling.
* Improved handling for remote images and added local caching. Internet connection is no longer needed once remote images are downloaded.
* Added `skip()` which leaves the style unapplied, so the property keeps (or returns to) its original value, e.g. `{{width > 0 ? width : skip()}}` applies only once `width` is positive.
* Improved handling for `NaN` and `Infinity` values in expressions.

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
