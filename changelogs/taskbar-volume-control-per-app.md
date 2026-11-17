## 1.2 ([Nov 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/afd824bf1fddd9a2e59d85a559390e9ba3e77188/mods/taskbar-volume-control-per-app.wh.cpp))

* Middle-clicking a taskbar button toggles mute for that app. It can be turned off in the mod settings. The implementation was contributed by [flxcraft](https://github.com/flxcraft).
* Added an option for the tooltip color.
* Added the "Touchpad scrolling" option, which changes the volume with two-finger scrolling on precision touchpads whose scrolling doesn't work over the taskbar.
* Fixed the tooltip being cut off with a vertical taskbar, set in the Windows settings or by the "Vertical Taskbar for Windows 11" mod. The tooltip is shown at the screen edge, above thumbnail previews.
* Fixed compatibility with newer Windows 11 versions, where the system tray is part of the taskbar frame.

## 1.1.4 ([Jan 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7a831cf22a819dd2549b70aa3de1fd62394d7097/mods/taskbar-volume-control-per-app.wh.cpp))

* Added support for some Chromium-based programs.
* Updated the mod description to mention a workaround for some laptop touchpads.
* Fixed the tooltip being misplaced while only half of the content is displayed at the top of the taskbar (the fix in v1.1.2 didn't work in all cases).

## 1.1.3 ([Jan 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1d32aa72d3460c7487ffe590ade4ba24b84a80af/mods/taskbar-volume-control-per-app.wh.cpp))

* Fixed compatibility with the vertical taskbar.

## 1.1.2 ([Jan 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/44661b1433d48855610c53c44e7a936d3f584ae9/mods/taskbar-volume-control-per-app.wh.cpp))

* Fixed the tooltip being misplaced while only half of the content is displayed at the top of the taskbar.
* Fixed the tooltip being horizontally misplaced with some custom taskbar themes such as the DockLike taskbar theme.
* Added support for older Windows 11 builds.

## 1.1.1 ([Jan 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0c5ede9a9249b0e88c3d82b48c263f6b24f03560/mods/taskbar-volume-control-per-app.wh.cpp))

* Reverted to the in-taskbar tooltips, since the above-taskbar tooltips are hovered by thumbnails.

## 1.1 ([Jan 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b01400a9758db6ea150ee7d45a28863fbcaa28c8/mods/taskbar-volume-control-per-app.wh.cpp))

* Added an option mute apps with Ctrl+click.
* Added an option for a terse format, e.g., "🔊 64%" instead of "Volume: 64%".
* Improved the tooltips to be shown above the taskbar items. This also adds support for the vertical taskbar.
* Added support for Chromium browsers and Electron apps.
* Changed the default volume change step from 2 to 10.

## 1.0 ([Jan 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7355e2851f5a14f838c11dac1ec707b01945eee8/mods/taskbar-volume-control-per-app.wh.cpp))

Initial release.
