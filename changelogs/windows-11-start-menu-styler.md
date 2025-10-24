## 1.3.2 ([Oct 24, 2025](https://github.com/ramensoftware/windhawk-mods/blob/1f6c3b0396e9f2e89a01266c7207fc847c6679ff/mods/windows-11-start-menu-styler.wh.cpp))

* Updated most themes for greater compatibility with [the redesigned Start menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/). The excellent adaptation work was done by [bbmaster123](https://github.com/bbmaster123).
* Added themes: WindowGlass, WindowGlass (Minimal), Fluid, Oversimplified&Accentuated.
* Images from online sources are now fetched once internet becomes available. Previously, if images didn't load successfully, disabling and re-enabling the mod was required.
* WindhawkBlur brushes that use ThemeResource colors are now updated automatically on theme change. Previously, disabling and re-enabling the mod was required to apply the new colors.

## 1.3.1 ([Jul 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/51c2fd314812aaa6a4596b58dc12c6aa09d4165e/mods/windows-11-start-menu-styler.wh.cpp))

* Updated themes: Windows10 (Minimal), TranslucentStartMenu, Fluent2Inspired.
* Initial adaptation of themes to [the redesigned start menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/): NoRecommendedSection, Windows10, Windows10 (Minimal), TranslucentStartMenu, Fluent2Inspired, RosePine.
* The "Disable the new start menu layout" option now disables the redesigned start menu too. Previously, only layout changes related to the Phone Link pane were disabled.
* Added ThemeResource support to WindhawkBlur.

## 1.3 ([Jun 7, 2025](https://github.com/ramensoftware/windhawk-mods/blob/4579380d42e30aea8898e502106df234cade7b3c/mods/windows-11-start-menu-styler.wh.cpp))

* Added an option to disable the new start menu layout. Recent Windows updates introduced some changes which broke some of the themes. The themes will be adapted to these changes in the future, but for now, this option can be used to temporarily revert them.
* Added themes: Windows10 (Minimal), LegacyFluent, OnlySearch.
* Updated themes: SideBySideMinimal, TranslucentStartMenu.
* Added style constants which can be defined once and used in multiple styles.
* Improved handling of commented targets and styles (starting with `//`), and added details about this in the mod's description.
* Added a built-in blur brush object, `WindhawkBlur`, which supports the `BlurAmount` and `TintColor` properties. For example: `Fill:=<WindhawkBlur BlurAmount="10" TintColor="#80FF00FF"/>`.
* Added support for the new WebView2 search menu.

## 1.2.1 ([Feb 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/8e8581b30c2d1d64c641b8fdfe8ee417df35fdcb/mods/windows-11-start-menu-styler.wh.cpp))

* Added themes: 21996, Down Aero, UniMenu.
* Updated themes: NoRecommendedSection, TranslucentStartMenu.
* Added support for styling the Windows 10 search menu.
* Added support for styling the offline variant of the search menu (when the DisableSearchBoxSuggestions registry option is enabled).
* Fixed the All Apps list sometimes becoming truncated.
* Empty visual states can now be targeted, can be useful in cases when there's no active visual state.
* Fixed not being able to style some elements due to error 802B000A (failed to create a 'System.Type' from the text).

## 1.2 ([Dec 10, 2024](https://github.com/ramensoftware/windhawk-mods/blob/8db53b86b7fb576eaf44aaa772c00a53a8933590/mods/windows-11-start-menu-styler.wh.cpp))

* The start menu search (what you see when you click on the start menu search bar) is now styled as well. Most of the search content is a WebView element, and the mod now allows to style it with CSS targets and styles. Refer to the mod description for more details.
* Added the Everblush theme.
* Updated themes: SideBySide2, Fluent2Inspired, Metro10Minimal.

## 1.1.6 ([Oct 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/31cc7d1150a2953dfb1897f13678dcd70f74fa47/mods/windows-11-start-menu-styler.wh.cpp))

* Added themes: RosePine, Windows11_Metro10Minimal.
* Updated existing themes.
* Added a workaround for hanging the start menu process on Windows 10. Although the available themes and example styles are designed for Windows 11, it should now be possible to use the mod on Windows 10 too.

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
