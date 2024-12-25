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
