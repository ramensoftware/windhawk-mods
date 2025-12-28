## 1.6 ([Dec 28, 2025](https://github.com/ramensoftware/windhawk-mods/blob/b10d6bd9375f96e105e8b13d2db6a2a75b4868bf/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.76 to 1.2.80
* Add a 'transparent mode', which allows full transparency when the transparent rendering is enabled, and native frames are disabled
  * To activate, call the `setTransparent(true)` function of the mod's JavaScript API
  * This mode conflicts with DWM effect applicators like MicaForEveryone and Translucent Windows, and the Basic Themer Windhawk mod that has the Spotify window excluded by the exclusion options or the 'secure desktop only' mode.
  * Please exclude Spotify from MicaForEveryone/Translucent Windows, and for the Basic Themer mod, Spotify must be included in the mod's basic theme apply list, or the mod must be fully disabled.
* Add support for removing DWM effects with the JS API via `setBackdrop('none')`.
* Update the readme and add documentation for the mod's JavaScript API.

## 1.5 ([Oct 20, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c09f0f7e253100a65cff60ea5399a8d5074623c9/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.75
* Fix Basic/Classic frames being invisible on Spotify 1.2.71+ / CEF 139+

## 1.4 ([Oct 5, 2025](https://github.com/ramensoftware/windhawk-mods/blob/3e8b6dcf0692df8cdba401d7d6ef4ee54ce5bdbe/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.72 to 1.2.74
* Add an option to block the Spotify client from updating itself
* Prevent Basic frames from being wrongly drawn when entering and exiting the full-screen mode

## 1.3 ([Aug 24, 2025](https://github.com/ramensoftware/windhawk-mods/blob/bdd9ada4189479dd475c3db41ee033f5ef190a17/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.66-1.2.71
* Changing the playback speed is no longer supported on Spotify 1.2.67 and higher due to Spotify blocking the way this mod changed it
* Fix basic frames being drawn wrongly when entering and exiting the full-screen mode, when the native frames are disabled

## 1.2 ([Jun 5, 2025](https://github.com/ramensoftware/windhawk-mods/blob/4341502030118d63157c4bc43d74f199aa667e19/mods/cef-titlebar-enabler-universal.wh.cpp))

* Fixed compatibility with the Translucent Windows mod
    * At least version 1.5 of the Translucent Windows mod is required. 1.4 and below are still not supported when the native frames are disabled

## 1.1 ([Jun 3, 2025](https://github.com/ramensoftware/windhawk-mods/blob/5a9dafdec518bcd45686bc0e87f61858f7ce5d01/mods/cef-titlebar-enabler-universal.wh.cpp))

* Finally implemented proper transparency support for frameless windows
    * In frameless mode, this mod is not compatible with the [Translucent Windows](https://windhawk.net/mods/translucent-windows) mod, due to the mod interfering with how this mod works around [Chromium's weird code](https://source.chromium.org/chromium/chromium/src/+/main:ui/views/win/hwnd_message_handler.cc;drc=339fea7fafdc1ba5b16e7b2fa6f9d996b65348a3;l=616)
    * Please use [MicaForEveryone](https://github.com/MicaForEveryone/MicaForEveryone) instead in frameless mode
    * This does not apply when the native frame option is enabled. Translucent Windows will work fine in this mode
* Prevent bad memory scan results with zero match persisting as a cached count value
* Prevent various time-consuming memory patch operations from running on known incompatible versions
* Added support for disabling the forced dark mode in Spotify versions between 1.2.21 and 1.2.25

## 1.0 ([May 29, 2025](https://github.com/ramensoftware/windhawk-mods/blob/363a9ab160fe2975c2fc1a7c359b488bbd43cc88/mods/cef-titlebar-enabler-universal.wh.cpp))

* Added a standalone playback speed changer in the Windhawk mod settings page
  * Previously, this feature was only available via the JavaScript API
* Added support for Spotify 1.2.63 and 1.2.64, including the playback speed mod
* Added caching for memory patch offsets to improve the startup speed
* Added support for Windows 7 running older versions of the Spotify client (1.2.5 and below)
  * `--no-sandbox` is not required when running those older versions
  * Note that non-DWM mode might look broken due to Chromium's classic custom frames. It is recommended to enable Aero and then use a basic themer for using the Basic/Classic frames
  * You'll still need `--no-sandbox` when running newer versions with VxKex
* Fixed memory leaks in the renderer process caused by improperly managed V8 value memories
* Fixed the GPU acceleration status being detected wrongly and causing rendering issues when the hardware acceleration is enabled but unavailable
* Improved security by limiting the IPC pipe connection to the most recently spawned renderer

## 0.9 ([Apr 20, 2025](https://github.com/ramensoftware/windhawk-mods/blob/1d248fdd4964e037fc00cb9799cd5fb55fe92f77/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.62
* Fix the `query()` API function not getting the latest option changes
* Now this mod uses a more proper way of changing the minimum window size
  * JavaScript `window.resizeTo()` should work fine for smaller sizes, and the resize cursor should always be shown when hovering over window borders
  * As a result, this feature no longer works on Spotify versions older than 1.1.71

## 0.8 ([Mar 26, 2025](https://github.com/ramensoftware/windhawk-mods/blob/e3cf3ac434db56bfb98c39e23f2215a8c4106600/mods/cef-titlebar-enabler-universal.wh.cpp))

* Fixed Aero Glass not working and showing a white background instead in some cases

## 0.7 ([Feb 10, 2025](https://github.com/ramensoftware/windhawk-mods/blob/da2e72abc1f05e34d0737578eb8fcc8a17780a59/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.57
* Fix JS API being unavailable on Spotify 1.2.4-1.2.32: use `window._getSpotifyModule('ctewh')` instead on these versions
* The transparent rendering, anti-forced dark mode, and Chrome extension enabler mods can now be applied even if the mod is loaded a bit late

## 0.6 ([Jan 30, 2025](https://github.com/ramensoftware/windhawk-mods/blob/1a126cbd6d9e3d268ccfe23eb3a5774acb456102/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.55 and 1.2.56
* Support directly enabling transparent web content rendering without having to patch `Spotify.exe` manually
* Add options to disable forced dark mode or force-enable Chrome extension support
* Add a JavaScript API for interacting with this mod or the main window that can be used within Spicetify extensions and themes

## 0.5 ([Dec 25, 2024](https://github.com/ramensoftware/windhawk-mods/blob/d25beeb3c894c97d36c4fe7b52e67a3505ad78e2/mods/cef-titlebar-enabler-universal.wh.cpp))

* Support making Spotify's custom window controls transparent
* Try to avoid crashes on untested newer releases of CEF/Spotify
* Support using DWM backgrounds with MicaForEveryone (requires hex patching `Spotify.exe`; see the mod details for instructions)

## 0.4 ([Dec 24, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b8b4b7aaa97eae0a573b4cde7bcd95b869448bd9/mods/cef-titlebar-enabler-universal.wh.cpp))

* The leftover space from hiding Spotify's custom window controls and the menu button is now clickable (mouse events are sent to the CEF browser)

## 0.3 ([Dec 21, 2024](https://github.com/ramensoftware/windhawk-mods/blob/e1dedbdcf972be80fa02d122d7379f9ca91e7582/mods/cef-titlebar-enabler-universal.wh.cpp))

* Add support for Spotify 1.2.53
* The ignore minimum size option now works without having to enable the native frames option
* Add an option to enable native frames on non-main windows (miniplayer, DevTools, etc.)
* WM_NCPAINT fix now works on non-Spotify CEF applications

## 0.2 ([Dec 18, 2024](https://github.com/ramensoftware/windhawk-mods/blob/d9e7075dc68171e319427778e094cf9945dbe4a9/mods/cef-titlebar-enabler-universal.wh.cpp))

* Now supports proper native window controls on all supported versions. DWM, Basic, and Classic are all supported.
* Fix show menu option not working properly when logging is disabled
* Fix Basic/Classic frames turning black in some cases (upstream Chromium issue)
* Add an option to ignore the minimum size set by Spotify

## 0.1 ([Dec 13, 2024](https://github.com/ramensoftware/windhawk-mods/blob/1c42a261bb552580949476c51ff569b15070ad6e/mods/cef-titlebar-enabler-universal.wh.cpp))

Initial release.
