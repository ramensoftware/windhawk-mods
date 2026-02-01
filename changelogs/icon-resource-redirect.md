## 1.2.4 ([Feb 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9e4a8e92b9c5f80d2abde061c88ee272d3c1ed9d/mods/icon-resource-redirect.wh.cpp))

* Fixed `PrivateExtractIconsW` redirection for textual (non-numerical) resource IDs.
* Fixed `PrivateExtractIconsW` redirection for non-executable files (e.g. .ico files).

## 1.2.3 ([Jan 31, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c647bf6a1706bf7fd3e03751a2976b0ad64a6f41/mods/icon-resource-redirect.wh.cpp))

* Fixed some redirection cases of icons extracted with the `PrivateExtractIconsW` function.
* Fixed redirection of icon/cursor groups in some cases with the "Redirect all loaded resources" option.
* Fixed icon themes ending up in a corrupted state after removing and re-installing the mod.

## 1.2.2 ([Jul 11, 2025](https://github.com/ramensoftware/windhawk-mods/blob/4f480eff9a5cecad737798bfafa461e1fb23c250/mods/icon-resource-redirect.wh.cpp))

* Added the Kripton Flatery, Pane7 icon themes.
* Fixed some of the icon themes. To apply the fix, you might need to uninstall and re-install the mod.
  * Fixed icon themes: All White Icons, Catppuccin Lavender, Catppuccin Maroon, Eyecandy, Fluent, Fluent Keys Night, Fluent Keys Day, Gruvbox, Gruvbox Plus Olive, Gruvbox Numix, Lumicons Folders, Lumicons Symbols, Somatic Rebirth, Super Remix Slate, Sweet Awesomeness, Sweetness Neutral, Sweetness Original.
* Libraries are now loaded with the `LOAD_LIBRARY_SEARCH_SYSTEM32` flag to prevent malicious or accidental dll hijacking.

## 1.2.1 ([Jun 27, 2025](https://github.com/ramensoftware/windhawk-mods/blob/88473fba37aafef3009f3b609ca35bd55218e9bc/mods/icon-resource-redirect.wh.cpp))

* Fixed an error when launching programs without a visual style manifest.

## 1.2 ([Jun 27, 2025](https://github.com/ramensoftware/windhawk-mods/blob/9ee749979a3dfa668f4a16432d449f77ffb4fce9/mods/icon-resource-redirect.wh.cpp))

* Icon themes can now be selected in the mod settings. A selected theme is automatically downloaded, extracted, and applied. A prompt is shown to optionally clear the icon cache and restart Explorer.
* A theme path can now specify the ini file itself, not only the theme folder with the theme.ini file.
* Multiple theme paths can now be specified.

Windhawk v1.6 or newer is required for this version of the mod.

## 1.1.9 ([Nov 10, 2024](https://github.com/ramensoftware/windhawk-mods/blob/adad08eed4819a37229592168641eb2753afa76e/mods/icon-resource-redirect.wh.cpp))

* Improved compatibility with some resources in some cases in the experimental mode.
* Fixed string redirection in some cases (non-experimental mode).
* Added support for theme.ini files larger than 32 KB (yes, somebody had a file larger than that).
* Added a workaround to the error message that says "Runtime Error - Not Enough space for thread Data". The error could occur when a process is shutting down due to a somewhat specific incompatibility between the Microsoft C++ runtime library and Windhawk's C++ library. It's not really harmful, as the process is shutting down anyway, but it can be annoying.

## 1.1.8 ([Oct 25, 2024](https://github.com/ramensoftware/windhawk-mods/blob/5c313117215b4ad4452f3c15a7e1378cd2eb3a00/mods/icon-resource-redirect.wh.cpp))

* "Redirect all loaded resources" mode: Fixed icon loading in case the redirection module has different icon formats.
* "Redirect all loaded resources" mode: String resources are now redirected in more cases.
* Fixed a rare crash that could occur if logging is enabled or if the "Redirect all loaded resources (experimental)" option is enabled.

## 1.1.7 ([Oct 17, 2024](https://github.com/ramensoftware/windhawk-mods/blob/951ca39188471a50c458d2ba5cded4f96a03e392/mods/icon-resource-redirect.wh.cpp))

* Improved DirectUI string redirection.
* Improved string redirection to apply in more cases.
* Added an experimental option to redirect all resources, not only the supported resources that are listed in the description.

## 1.1.6 ([Oct 12, 2024](https://github.com/ramensoftware/windhawk-mods/blob/1db804f1cdc96957d8e71132cf1378fc19d94ea2/mods/icon-resource-redirect.wh.cpp))

* Improved DirectUI resources redirection (usually `UIFILE` and `XML`) to not affect sub resources such as strings, which are now loaded from the original file.
* Improved icon cache invalidation when settings change and when the mod is unloaded.

## 1.1.5 ([Aug 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/84c3316ab38986a63b67f2dc50b44025989788ff/mods/icon-resource-redirect.wh.cpp))

* Added information about icon themes to the mod description.

## 1.1.4 ([Jun 8, 2024](https://github.com/ramensoftware/windhawk-mods/blob/5d1f12d1d984919a764556f139724d8eec004900/mods/icon-resource-redirect.wh.cpp))

* Added support for using patterns for redirection, where '\*' matches any number characters and '?' matches any single character, for example: `C:\Programs\Firefox-Version-*\firefox.exe`.

## 1.1.3 ([May 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/d70dd616505af63c736c1cad00feb5e2350cba3f/mods/icon-resource-redirect.wh.cpp))

* Hook ANSI variant functions: LoadImageA, LoadIconA, LoadCursorA, LoadBitmapA, LoadMenuA, DialogBoxParamA, CreateDialogParamA, LoadStringA.
* In case multiple icons are extracted and the custom resource only overrides some of them, return the original icons. We'd ideally like to return a combined result, but unfortunately that's not trivial to implement, and returning the original icons is better than returning a partial result.

## 1.1.2 ([May 8, 2024](https://github.com/ramensoftware/windhawk-mods/blob/fab76b768b1f821b066113a1334dc8bfcf4cbc43/mods/icon-resource-redirect.wh.cpp))

* Added handling for menus loaded with the `LoadMenuW` function.
* Added handling for dialogs loaded with the `DialogBoxParamW`, `CreateDialogParamW` functions.
* Added handling for images loaded from external files with the `LoadImageW` function.
* Improved handling for `PrivateExtractIconsW` when an exact icon size is requested.
* Improved logs.

## 1.1.1 ([Apr 30, 2024](https://github.com/ramensoftware/windhawk-mods/blob/597515446756cfaac51a302032e13992ce2ee32c/mods/icon-resource-redirect.wh.cpp))

* Added handling for bitmaps loaded with the `LoadBitmapW` function.

## 1.1 ([Apr 29, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a9c288908ae7264b588e9bcc266a786e92361af2/mods/icon-resource-redirect.wh.cpp))

* Renamed from Icon Resource Redirect to Resource Redirect.
* Added handling for icons loaded with the `LoadIconW` function.
* Added handling for cursors loaded with the `LoadCursorW` function.
* Added handling for DirectUI resources (usually `UIFILE` and `XML`) loaded with the `SetXMLFromResource` function.

## 1.0.5 ([Apr 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/cbc1433369c1ff664220cd31ee57cef2dbadf4f7/mods/icon-resource-redirect.wh.cpp))

* Added an option to select a theme folder. Refer to the mod description for details.

## 1.0.4 ([Apr 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/de61b82dd647c298c1f71c9b12f04320f012d4ff/mods/icon-resource-redirect.wh.cpp))

* Added string and GDI+ image redirection.

## 1.0.3 ([Apr 24, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3757ba003fd3b4550edf1b57b0af88182cff3789/mods/icon-resource-redirect.wh.cpp))

* Added redirection of bitmaps and cursors in addition to icons.
* Improved redirection to support more cases.
* Added detailed logging.

## 1.0.2 ([Apr 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3107e3fe77c403cd149536f9c648e9603e6b4057/mods/icon-resource-redirect.wh.cpp))

* Hook the LoadImageW function to have more icons replaced.

## 1.0.1 ([Apr 14, 2024](https://github.com/ramensoftware/windhawk-mods/blob/fd5c9631d7d4d73369b9b492edc8990c1cbfd32c/mods/icon-resource-redirect.wh.cpp))

* Hook the LoadIconWithScaleDown function to have more icons replaced.
* If an icon doesn't exist in the custom resource file, the original icon is used.

## 1.0 ([Apr 13, 2024](https://github.com/ramensoftware/windhawk-mods/blob/f2f6321492b1d47ac920e45cacd533f14484a59c/mods/icon-resource-redirect.wh.cpp))

Initial release.
