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
