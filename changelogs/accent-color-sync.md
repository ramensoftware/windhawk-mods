## 1.53 ([Oct 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/0bf9b7f52bcb88beaf0efa0e464fa4e53863d032/mods/accent-color-sync.wh.cpp))

* Re-added 'older OpenGlass' option to DWM modification software selection.
* Updated readme and made instructions clearer.

## 1.52 ([Oct 16, 2024](https://github.com/ramensoftware/windhawk-mods/blob/eb6dedd8df15020401723ccc9e1a82ed0246c9ce/mods/accent-color-sync.wh.cpp))

* OpenGlass: Removed old registry name option as it's redundant for the latest versions.
* OpenGlass: Fixed incompatibility with OpenGlass DComp (a registry value had been fixed at zero, which caused a visual bug on that version).
* Fixed compatibility for legacy versions of Windows 10 (1709 and below).

## 1.51 ([Oct 13, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b4fd17a79d7d27a688c194d2e85d2df6da981df4/mods/accent-color-sync.wh.cpp))

* **Hotfixes**
  * Fixed a minor bug in which the "Fixed opacity" option in the mod settings was accidentally left enabled. Please disable the option manually if you are not planning on using the feature.
  * Fixed a bug in which the OpenGlass options were incorrectly swapped.
  * Updated readme to correct some things.

## 1.5 ([Oct 13, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b788c556e80d06c9ae315e53830396ff92ee1d59/mods/accent-color-sync.wh.cpp))

* Fixed margins of error with the DWM colorization calculator formula, making it more accurate to ALTaleX's original Python code.
* OpenGlass: Add option to toggle between using the old registry value name (i.e. "og_{xxx}") and the current one ("{xxx}Override") in mod settings
* Add (experimental) support for [legacy Glass8](https://archive.org/details/full-glass8-archive)
* Add option to set permanent fixed opacity (useful for automatic desktop background color!)
* Add Japanese translation
* Updated readme, settings and mod description

## 1.41 ([Oct 2, 2024](https://github.com/ramensoftware/windhawk-mods/blob/4ab0994ab79cb76e217a4b658639f98875959b4a/mods/accent-color-sync.wh.cpp))

* Hotfix to account for new OpenGlass registry values as announced in the [latest update](https://github.com/ALTaleX531/OpenGlass/releases/tag/v1.2-legacy).
* Updated readme.

## 1.4 ([Sep 7, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7292f722244cd19dc93c4438270bcc600b4bf2ec/mods/accent-color-sync.wh.cpp))

* Replaced DWM Colorization Parameters calculation formula with [ALTaleX's method](https://github.com/ALTaleX531/dwm_colorization_calculator).
* Compatibility check now returns the actual Windows version instead of the version returned from Explorer. This fixes the issue of the mod not being compatible with [explorer7](https://github.com/Erizur/explorer7-releases/).
* Updated readme.

## 1.31 ([Jul 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/940c28ba0fb4ccebb782988516dd6bb5a7f23de0/mods/accent-color-sync.wh.cpp))

* Set *Sync with DWM* option to enabled by default, and added notice for updating regarding this option in the readme.
* Update French and Spanish translation.

## 1.3 ([Jul 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7c4df7d217e3e09907079ed0d5a13df0cf64d5f0/mods/accent-color-sync.wh.cpp))

* Fixed bugs in which OpenGlass opacity did not update when changing theme or clicking "Cancel" on the 'Color & Appearance' page.
* Tweaked color balance calculation to be more accurate, including adding hotfixes for forcing certain values to match Windows 7's.
* Fixed crash bug from the default opacity value using the wrong calculation.
* Removed transparency option as OpenGlass-legacy still does not yet support opaque rendering.
* Added Spanish & French translation (partial).

## 1.2 ([Jul 21, 2024](https://github.com/ramensoftware/windhawk-mods/blob/120bd157b2a8eb1c99a1ebd8363686cbc40de906/mods/accent-color-sync.wh.cpp))

* Add option to sync with DWM's opacity value.
* Add support for x86 Windows.

## 1.1 ([Jul 21, 2024](https://github.com/ramensoftware/windhawk-mods/blob/45b792a63461caa9ac8c5864b83fcf26d724e549/mods/accent-color-sync.wh.cpp))

* Add files via upload

* Update fix

* Add files via upload

## 1.0 ([Jul 19, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7bc74ba3694b934452c45f010dda182d70c1e222/mods/accent-color-sync.wh.cpp))

Initial release.
