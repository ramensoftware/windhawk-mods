## 1.5.2 ([Dec 16, 2025](https://github.com/ramensoftware/windhawk-mods/blob/9345ef955e6053e4e564c66a511146e60e4470dc/mods/windows-11-taskbar-styler.wh.cpp))

* Added new themes: SunValley, Surface, Oversimplified&Accentuated, Luminosity.
* Updated themes: SimplyTransparent, Squircle, Matter, WinXP, TaskbarXII, Windows7, WindowGlass.
* Renamed the 21996Taskbar theme to SunValley (Legacy).
* For themes using images from http(s) sources, the mod now retries loading once an internet connection is available.

## 1.5.1 ([Sep 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/423d9610b97bf4c6b11590948ca171b9b56a3c81/mods/windows-11-taskbar-styler.wh.cpp))

* Updated themes: TranslucentTaskbar, Windows7.
* Added themes: Plasma, WindowGlass, BottomDensy (NoInd).
* Added ThemeResource support to WindhawkBlur.
* Fixed the mod having no effect on the keyboard layout popup dialog.

## 1.5 ([Jul 11, 2025](https://github.com/ramensoftware/windhawk-mods/blob/fbce0bace35249e6ec4996e0b4a9b2bb07017352/mods/windows-11-taskbar-styler.wh.cpp))

* The mod now handles all explorer.exe threads, which allows styling additional components: the volume popup, Alt+Tab, Win+Tab, the language popup, snap layout popups, and maybe other elements.
* In new Windows 11 builds, fixed a crash with some themes when the "Icon and label"-style search button is used.
* Added new themes: WinXP (Zune), Matter, SimplyTransparent, Windows7, and Aeris.
* Update themes: Bubbles, TranslucentTaskbar, CleanSlate, Lucent, TaskbarXII.

## 1.4 ([May 9, 2025](https://github.com/ramensoftware/windhawk-mods/blob/29917c376199d3312e355f1b001dc3108040b7ca/mods/windows-11-taskbar-styler.wh.cpp))

* Added style constants which can be defined once and used in multiple styles.
* Added a built-in blur brush object, `WindhawkBlur`, which supports the `BlurAmount` and `TintColor` properties. For example: `Fill:=<WindhawkBlur BlurAmount="10" TintColor="#80FF00FF"/>`.
* Improved handling of commented targets and styles (starting with `//`), and added details about this in the mod's description.
* Added themes: BottomDensy, TaskbarXII, xdark.
* Updated themes: DockLike, 21996Taskbar.
  * The DockLike theme now applies correctly when a new monitor is attached.
* Improved compatibility with status bar programs such as YASB and Zebar.

## 1.3.10 ([Feb 18, 2025](https://github.com/ramensoftware/windhawk-mods/blob/969985c9efc19788571d93e85240a260c5ab0fc7/mods/windows-11-taskbar-styler.wh.cpp))

* Improved support for the new thumbnail previews implementation in Windows 11 for the following themes: WinXP, TranslucentTaskbar, Squircle, RosePine, DockLike, CleanSlate, Lucent.
* Fixed customizing the FontWeight property for some controls.

## 1.3.9 ([Feb 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/42d2709e22df203d6f5765b796909dca2a286cf1/mods/windows-11-taskbar-styler.wh.cpp))

* Added the 21996Taskbar theme.
* Updated themes: WinXP, Lucent.
* Empty visual states can now be targeted, can be useful in cases when there's no active visual state.
* Fixed error 80070490 which sometime prevented the mod from loading.

## 1.3.8 ([Oct 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/fde2dd9c8dafbf68a126fb25ec1b3f9587efd18b/mods/windows-11-taskbar-styler.wh.cpp))

* Added themes: CleanSlate, Lucent.
* Updated existing themes.

## 1.3.7 ([Jul 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/5ceffe5133b88ab09b00e41a846cd7a4bcb4e4c0/mods/windows-11-taskbar-styler.wh.cpp))

* Added themes: DockLike, WinVista.
* Updated themes: TranslucentTaskbar, Squircle, RosePine, Bubbles.

## 1.3.6 ([Jul 19, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b16412c8ab6de3303ecb179ddcb39a2fe8b28e25/mods/windows-11-taskbar-styler.wh.cpp))

* Added the RosePine theme.
* Updated the Squircle theme.

## 1.3.5 ([Jun 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/d02494284cfba2bb2c9fcb54ad499c0489ac45dd/mods/windows-11-taskbar-styler.wh.cpp))

* Added a new theme: Squircle.
* Fixed a crash with some themes when the "Icon and label"-style search button is used.

## 1.3.4 ([May 31, 2024](https://github.com/ramensoftware/windhawk-mods/blob/ff1acfae87ae906f670c3b0d43f7fbb8c3dcfd03/mods/windows-11-taskbar-styler.wh.cpp))

* Fixed taskbar background customization when there's more than one monitor.
* TranslucentTaskbar theme: Fixed menu flyout padding.

## 1.3.3 ([May 30, 2024](https://github.com/ramensoftware/windhawk-mods/blob/7cb5b296f3dd8385719119f7693a9037e5d9e46f/mods/windows-11-taskbar-styler.wh.cpp))

* Added the TranslucentTaskbar theme.
* Made styles defined later override styles defined earlier.
* Empty values are now allowed. Setting an empty value with the XAML syntax clears the property value.
* Added a workaround for setting FontWeight which didn't work.
* Fixed null property value handling which could cause a crash.
* Fixed background customization which was sometimes not applied on launch.

## 1.3.2 ([May 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/6fcd66105c7419656b3d34947dacf56a3b06927c/mods/windows-11-taskbar-styler.wh.cpp))

* Added an online symbol cache mechanism as a temporary workaround for the unavailable Microsoft symbols. Currently, this makes the mod work on Windows 11 versions 22631.3447 and 22631.3527. For more details, refer to [the relevant blog post](https://ramensoftware.com/windhawk-and-symbol-download-errors).

## 1.3.1 ([Apr 23, 2024](https://github.com/ramensoftware/windhawk-mods/blob/72258fe7830a25a42dd597bd6b8588f02f4b6152/mods/windows-11-taskbar-styler.wh.cpp))

* Having more than one target applied for the same element is now supported.
* Added the Bubbles theme.
* Fixed some styles not being restored when the mod is unloaded.

## 1.3 ([Apr 20, 2024](https://github.com/ramensoftware/windhawk-mods/blob/085a17c3ddc3084feb9f607b971d4873076d6fb7/mods/windows-11-taskbar-styler.wh.cpp))

* Added themes! Currently, only the sample WinXP theme is integrated into the mod and can be selected in the settings.
* Added support for customizing some elements which weren't customizable before.
* Fixed: Only parse styles when the target element is available. Previously styling some elements could cause a crash on startup.

## 1.2.3 ([Dec 26, 2023](https://github.com/ramensoftware/windhawk-mods/blob/dd6a77e99fd98816a7800be14a834ad988a6cd58/mods/windows-11-taskbar-styler.wh.cpp))

* Added support for unnamed visual state groups.
* Implemented generic handling for any target namespace.

## 1.2.2 ([Dec 21, 2023](https://github.com/ramensoftware/windhawk-mods/blob/955ecfce747cec506a89bdee69c706aea050487a/mods/windows-11-taskbar-styler.wh.cpp))

* Added a workaround for a crash on startup if a custom background is set.

## 1.2.1 ([Dec 16, 2023](https://github.com/ramensoftware/windhawk-mods/blob/881a5daee8bdcf6cdc0e8f6a7ca99ebbeab1f569/mods/windows-11-taskbar-styler.wh.cpp))

* Fixed a possible crash and style applying bugs when settings are changed.
* Added link to the Windows 11 taskbar styling guide.

## 1.2 ([Dec 11, 2023](https://github.com/ramensoftware/windhawk-mods/blob/23129dc99634ad8037cd80cd3f01a7e79b63bcfc/mods/windows-11-taskbar-styler.wh.cpp))

* Index, properties and a visual state can now be specified for a target. Refer to the mod description for more information and examples.
* Styles can now be specified per visual state.
* Styles can now be specified in XAML form.
* Customized styles are now restored on mod unload.
* Fixed a possible crash on mod unload.

## 1.1.3 ([Sep 21, 2023](https://github.com/ramensoftware/windhawk-mods/blob/7eb55c75224aa0dfff6a524fa3d993ece8a41704/mods/windows-11-taskbar-styler.wh.cpp))

* Fixed mod not unloading when disabled.

## 1.1.2 ([Sep 16, 2023](https://github.com/ramensoftware/windhawk-mods/blob/3ab1f3e504481a1a704e331709a3508fe1980eec/mods/windows-11-taskbar-styler.wh.cpp))

* Fixed a crash loop when some styles are used.

## 1.1.1 ([Sep 15, 2023](https://github.com/ramensoftware/windhawk-mods/blob/c8bd45d3c8292364a173776354e9909934fb3817/mods/windows-11-taskbar-styler.wh.cpp))

* Improved parent control matching.

## 1.1 ([Jul 27, 2023](https://github.com/ramensoftware/windhawk-mods/blob/945959053f99f73a7babdd46a097e905f0f1f4e2/mods/windows-11-taskbar-styler.wh.cpp))

* Added the ability to target control child elements
* New styles are now applied without restarting Explorer (but previously set styles aren't cleared)
* Fixed showing the Explorer restart prompt message on startup
* Added an option to completely disable the Explorer restart prompt message

## 1.0 ([May 19, 2023](https://github.com/ramensoftware/windhawk-mods/blob/dd94ba0dad734db104daf98094a6497bb2911039/mods/windows-11-taskbar-styler.wh.cpp))

Initial release.
