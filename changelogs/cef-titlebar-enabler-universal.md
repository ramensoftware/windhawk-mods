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
