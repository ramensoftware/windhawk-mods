## 1.3.2 ([Oct 1, 2023](https://github.com/ramensoftware/windhawk-mods/blob/5d0c4eeb53c4888499420595c3f4a4658569be60/mods/taskbar-clock-customization.wh.cpp))

* Fixed clock with seconds precision issue for some Windows 11 versions.

## 1.3.1 ([Sep 29, 2023](https://github.com/ramensoftware/windhawk-mods/blob/9422e42496cbf69949fb5cedce029b6bf3f0ddb8/mods/taskbar-clock-customization.wh.cpp))

* Fixed the incompatibility with Windows 11 22H2 update KB5030310 ("Moment 4" update).

## 1.3 ([Sep 17, 2023](https://github.com/ramensoftware/windhawk-mods/blob/18081443a8c4f967d07c82997d3096f3b9faba6c/mods/taskbar-clock-customization.wh.cpp))

Taskbar Clock Customization v1.3

* Added text alignment customization option (Windows 11).
* Styles are applied automatically without having to hover over the clock, and are restored when the mod is disabled (Windows 11).
* Text spacing can now be customized on Windows 11, not only on Windows 10.

## 1.2.1 ([Aug 29, 2023](https://github.com/ramensoftware/windhawk-mods/blob/122aa5caae18036ac97f187119f589b70a76c917/mods/taskbar-clock-customization.wh.cpp))

* Fixed a crash for new mod installations.

## 1.2 ([Aug 29, 2023](https://github.com/ramensoftware/windhawk-mods/blob/8ab7ca1f13850e0ebb9ac2a9aee7ac75cc947d81/mods/taskbar-clock-customization.wh.cpp))

* Added support for multiple web content sources.
* Fixed the number of seconds updating with a slight delay, finally causing one second to be skipped.

## 1.1.1 ([May 20, 2023](https://github.com/ramensoftware/windhawk-mods/blob/9c814a48137a68e5e1262b98d84be86b08ab96e9/mods/taskbar-clock-customization.wh.cpp))

* Fixed the first clock line flashing on hover on Windows 11 version 22H2 and newer.

## 1.1 ([May 20, 2023](https://github.com/ramensoftware/windhawk-mods/blob/0718eb23fbfa530f9a7bc1c1892f071db8b4fb41/mods/taskbar-clock-customization.wh.cpp))

* Added settings for text styles, such as the font color and size, for Windows 11 version 22H2 and newer.
* Added the `%timezone%` pattern for the time zone in ISO 8601 format.
* Fixed handling of the "Web content end" value.

## 1.0.9 ([Apr 4, 2023](https://github.com/ramensoftware/windhawk-mods/blob/f053e22f61b37e9561a8de8786541dd79be955f1/mods/taskbar-clock-customization.wh.cpp))

* Added a feature that allows the user to customize the spacing between each line in the clock. Along with supporting custom text spacing, no spacing will be used if the user specifies no text for the bottom and middle lines, allowing one line on a large taskbar. Contributed by Nightlinbit.

## 1.0.8 ([Mar 4, 2023](https://github.com/ramensoftware/windhawk-mods/blob/0a2d75864aab7e62da9262f5e054cba6e37baba7/mods/taskbar-clock-customization.wh.cpp))

* Added the ISO week number as `%weeknum_iso%`.

## 1.0.7 ([Mar 3, 2023](https://github.com/ramensoftware/windhawk-mods/blob/0dbb93c0ff0d66038a539dd3d6c97690701d4757/mods/taskbar-clock-customization.wh.cpp))

* Renamed `%weeknum%` to `%weekday_num%`.
* Improved `%weekday_num%` to take the configured first day of the week into account.
* Added the week number of the year as `%weeknum%`.
* Reduced news fetching retry time when the request fails.
* Improved Windows version detection.

## 1.0.6 ([Jan 4, 2023](https://github.com/ramensoftware/windhawk-mods/blob/73cb003a7158fe622a169362a6669ccc9bfbd95e/mods/taskbar-clock-customization.wh.cpp))

* Fix tooltip when secondary clocks are used.
* Add option to add an extra line to the tooltip.
* Add week number as %weeknum%.
* Add support for the old taskbar on Windows 11 (e.g. with Explorer Patcher).
* Improve loading speed by caching symbols.

## 1.0.5 ([Oct 14, 2022](https://github.com/ramensoftware/windhawk-mods/blob/18343f1646f86232bfa309f20ab19857cbbd9cb1/mods/taskbar-clock-customization.wh.cpp))

* Fix an explorer crash when the monitor turns off.

## 1.0.4 ([Aug 24, 2022](https://github.com/ramensoftware/windhawk-mods/blob/e2ca051a501e542dc5e4a3ad6e2945fb4d1b3b35/mods/taskbar-clock-customization.wh.cpp))

* Add support for new Windows 11 builds.

## 1.0.3 ([Jun 20, 2022](https://github.com/ramensoftware/windhawk-mods/blob/c450034bab997a7833c0843c9e35d5506e3e8899/mods/taskbar-clock-customization.wh.cpp))

* Fix mod not working on system start.

## 1.0.2 ([Jun 17, 2022](https://github.com/ramensoftware/windhawk-mods/blob/e39485cf274daba4c6ec76329b9f8112ecf973ea/mods/taskbar-clock-customization.wh.cpp))

* Add support for Windows 11 version 22H2.

## 1.0.1 ([Jun 16, 2022](https://github.com/ramensoftware/windhawk-mods/blob/972c27bbbcdf39a2b8faf02cbfe5da2bc6080ee5/mods/taskbar-clock-customization.wh.cpp))

* Fix finding symbols for new Win11 versions.

* Fix crash in case of empty strings in Win11.

## 1.0 ([Mar 6, 2022](https://github.com/ramensoftware/windhawk-mods/blob/85322d8095db39e00abcd70168b490c9602c43d4/mods/taskbar-clock-customization.wh.cpp))

Initial release.
