## 1.1.5 ([Jul 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b6c907a723f18b738c615933ec1834ef9009f18d/mods/windows-11-start-menu-styler.wh.cpp))

* Updated SideBySide2 theme with several fixes.

## 1.1.4 ([Jul 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/2007927c9158132859b6d559104a7539742c74d7/mods/windows-11-start-menu-styler.wh.cpp))

* Added themes: Fluent2Inspired, Windows10, Windows11_Metro10.
* Updated themes: SideBySide, SideBySide2, TranslucentStartMenu.

## 1.1.3 ([May 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/09a45ac4d6daf121bc16d69a7c961f8be0405d07/mods/windows-11-start-menu-styler.wh.cpp))

* Added the TranslucentStartMenu theme.
* Made styles defined later override styles defined earlier.
* Empty values are now allowed. Setting an empty value with the XAML syntax clears the property value.
* Added a workaround for setting FontWeight which didn't work.
* Fixed null property value handling which could cause a crash.

## 1.1.2 ([Apr 30, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a36ce2c5a985dcc2822d344ccf8aacd4650fb59a/mods/windows-11-start-menu-styler.wh.cpp))

* Having more than one target applied for the same element is now supported.
* Fixed some styles not being restored when the mod is unloaded.

## 1.1.1 ([Apr 20, 2024](https://github.com/ramensoftware/windhawk-mods/blob/ea404918c73f63626fee81883879365498d2cdd9/mods/windows-11-start-menu-styler.wh.cpp))

* Fixed incorrect class name replacements.

## 1.1 ([Apr 20, 2024](https://github.com/ramensoftware/windhawk-mods/blob/088c43ae0fea274c39f5aab12168e9a3e792c579/mods/windows-11-start-menu-styler.wh.cpp))

* Added themes! Currently, four themes are integrated into the mod and can be selected in the settings.
* Added support for customizing some elements which weren't customizable before.
* Fixed: Only parse styles when the target element is available. Previously styling some elements could cause a crash on startup.

## 1.0.2 ([Feb 12, 2024](https://github.com/ramensoftware/windhawk-mods/blob/6f22a17e1b26a3a7ee513c0589dc40cd555d61fb/mods/windows-11-start-menu-styler.wh.cpp))

* Fixed styles not applying on system startup.

## 1.0.1 ([Dec 26, 2023](https://github.com/ramensoftware/windhawk-mods/blob/a962cd86b636e62652f9a903d0a064190d638cdf/mods/windows-11-start-menu-styler.wh.cpp))

* Added support for unnamed visual state groups.
* Implemented generic handling for any target namespace.

## 1.0 ([Dec 19, 2023](https://github.com/ramensoftware/windhawk-mods/blob/af135759094c6dc9558b926435bb1a9f597bf30f/mods/windows-11-start-menu-styler.wh.cpp))

Initial release.
