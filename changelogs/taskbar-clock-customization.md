## 1.7.1 ([Jan 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/273b8d750fd41ccabba8d53a70d0d8ddb7d6fafd/mods/taskbar-clock-customization.wh.cpp))

* Added an option to specify text for `%media_info%` when no media is playing.
* Added the `%n%` pattern as a shorter alternative to `%newline%`.
* Fixed `%battery%` to show 100% when fully charged. 99% was previously displayed in this case.
* Fixed network and GPU metrics not accounting for new adapters or processes.
* Fixed the mod preventing explorer.exe from exiting if media info patterns are used.

## 1.7 ([Dec 26, 2025](https://github.com/ramensoftware/windhawk-mods/blob/f5c9e38460918ba1fa4b643dedab4ecf42faf03c/mods/taskbar-clock-customization.wh.cpp))

- Added new system performance metrics patterns:
  - `%total_speed%` - combined upload and download transfer rate.
  - `%disk_read%` - disk read speed.
  - `%disk_write%` - disk write speed.
  - `%disk_total%` - combined disk read and write speed.
  - `%gpu%` - GPU usage.
  - `%battery%` - battery level percentage.
  - `%battery_time%` - battery time remaining (charging/discharging time left).
  - `%power%` - battery power in watts (negative when discharging, positive when charging).
- Added media player info patterns, based on the work contributed by trashpanda ([kaoshipaws](https://github.com/kaoshipaws)):
  - `%media_title%` - currently playing media title.
  - `%media_artist%` - currently playing media artist.
  - `%media_album%` - currently playing media album.
  - `%media_status%` - media playback status icon (⏯, ⏸, ⏹).
  - `%media_info%` - combined media info (Artist - Title), truncated with ellipsis.
- Added tooltip line mode option to replace the default tooltip instead of appending to it.
- Added network adapter name filter for upload/download metrics.
- Added "single space padding" option for percentage format (useful for monospaced fonts).

## 1.6.3 ([Aug 26, 2025](https://github.com/ramensoftware/windhawk-mods/blob/a1e5471c16c55ce3827e8f85367aafa200d0e69a/mods/taskbar-clock-customization.wh.cpp))

* Added more settings for system performance metrics:
  * Network metrics format: MB/s or MBit/s, with or without KB/s or KBit/s, optionally use a number only.
  * Fixed decimal places for network metrics (e.g. always show a single digit after the period).
  * Percentage format for CPU/RAM: Custom or no padding for a single digit.
* Use "Processor Utility" metric instead of "Processor Time" for a more accurate CPU usage calculation.
* Fixed compatibility with Windows feature flag 38762814, which makes the clock clickable on all taskbars.

## 1.6.2 ([Aug 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/725fd582986a072d0dc02c11281a56d30c3c2576/mods/taskbar-clock-customization.wh.cpp))

* Fixed performance metrics (upload/download speed) always showing zero values on non-English systems. The fix in version 1.6.1 was incomplete.

## 1.6.1 ([Aug 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7068e6957690dc8bb1c1d1449041593fc34ff51e/mods/taskbar-clock-customization.wh.cpp))

* Added an option to customize the weather units (USCS/SI).
* Improved performance metrics formatting to reduce text width changes:
  * CPU and RAM show 0-99 values, and single digit values are padded with spaces.
  * Upload/download speeds always show MB/s in a way that keeps the width constant for most values. Also, the current locale decimal separator is used (e.g. 12.3 or 12,3).
* Switched to a more accurate RAM usage calculation method.
* Fixed performance metrics (upload/download speed, CPU, RAM) always showing zero values on non-English systems.

## 1.6 ([Aug 9, 2025](https://github.com/ramensoftware/windhawk-mods/blob/8eb638d7f86c52bde93c6050ca3232bdf7baa5ef/mods/taskbar-clock-customization.wh.cpp))

* Added patterns for realtime system performance metrics:
  * `%upload_speed%` - system-wide upload transfer rate.
  * `%download_speed%` - system-wide download transfer rate.
  * `%cpu%` - CPU usage.
  * `%ram%` - RAM usage.
* Added the `%weather%` pattern for weather information, powered by [wttr.in](https://wttr.in/).
* Added the content mode option to web content items, which can be used to strip/decode HTML and XML tags.
* Added the content search/replace option to web content items to apply regular expression-based search and replace operations to the extracted content.
* Improved compatibility with status bar programs such as YASB and Zebar.

## 1.5.2 ([Mar 21, 2025](https://github.com/ramensoftware/windhawk-mods/blob/a2cd3cd75b7491b7f38a2ea3cf51ae71f164f119/mods/taskbar-clock-customization.wh.cpp))

* Fixed an incompatibility with recent Windows 11 preview builds.
* Added an option to specify a custom list of week days.

## 1.5.1 ([Mar 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/a2c63362113ef44916a06a798fd2424a44c17fd6/mods/taskbar-clock-customization.wh.cpp))

* Change the Visible option for the top/bottom clock line to Hidden, to prevent the bottom line from being hidden by default after updating to version 1.5.

## 1.5 ([Mar 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/67c1039afeda68ee5efd3c6a0e5cabe2de082557/mods/taskbar-clock-customization.wh.cpp))

* Added patterns for displaying additional time zones.
* Added a visibility option for the bottom clock line (Windows 11).

## 1.4 ([Dec 1, 2024](https://github.com/ramensoftware/windhawk-mods/blob/6082c60100929675fc56d995c9f9a6ae7b221c9e/mods/taskbar-clock-customization.wh.cpp))

* Added support for the new "Show abbreviated time and date" Windows 11 option.
* Added a way to specify multiple date/time formats. It can be useful for things such as showing hours and minutes in different lines.
* Added an option for the clock max width (Windows 11 only).
* Added the `%dayofyear%` pattern - the day of year starting from January 1st.
* Added the `%newline%` pattern - a newline.
* Web content is now updated right away after resuming the computer from sleep.
* Added support for the ExplorerPatcher taskbar.

## 1.3.3 ([May 4, 2024](https://github.com/ramensoftware/windhawk-mods/blob/058fa8b81165b566fd988e19e42052771a59b120/mods/taskbar-clock-customization.wh.cpp))

* Added an online symbol cache mechanism as a temporary workaround for the unavailable Microsoft symbols. Currently, this makes the mod work on Windows 11 versions 22631.3447 and 22631.3527. For more details, refer to [the relevant blog post](https://ramensoftware.com/windhawk-and-symbol-download-errors).
* Fixed clock with seconds precision issue for some earlier Windows 11 versions.

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
