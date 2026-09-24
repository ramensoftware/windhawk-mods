## 1.3 ([Sep 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0b31194932e041ef6da46e6bd165567d4b400099/mods/win7-bsdr.wh.cpp))

* Hardcode Windows Vista style resources, as well as 35 languages from every available Vista/7 MUIs.
* External DLL is no longer required for using the Vista style or localization. It is now only left for advanced users who want to customize the dialog with Resource Hacker.
    * If you have been using an external DLL, please set the dialog style to either Custom or your desired one.

## 1.2 ([Sep 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/58d5161becfeb9a770c8aa4372a32c02f5f39206/mods/win7-bsdr.wh.cpp))

* Fix a critical LogonUI crash occurring in Windhawk 2.0 alpha 4 and newer
* Add support for runtime DPI changes, which might occur if the monitor topology changes during a logoff (Windows 10 1703+ only)
* Reorder the mod README to be clearer about the necessary setting change

## 1.1 ([Sep 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d7f29f93f17b01ab03c53367f421bdc0fc339ffe/mods/win7-bsdr.wh.cpp))

* Add support for loading `winsrv.dll` from Windows Vista
    * Resources for Vista BSDR are not included in the mod source. You must provide the resource DLL if you want to use the Vista style. See the mod readme for the instructions.
* Improve the accuracy of app tile layout, image rendering, and scrolling behavior
* Refactor the safety checks to be less janky

## 1.0 ([Sep 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b71ad7e072474d28c82dc126b48917cb26fa3c44/mods/win7-bsdr.wh.cpp))

Initial release.
