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
