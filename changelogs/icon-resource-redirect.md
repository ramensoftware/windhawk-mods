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
