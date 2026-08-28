## 0.18.8 ([Aug 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d2af22192847759b3de9d7df43f2364b75331c58/mods/snap-sentry.wh.cpp))

- When a multi-page or animated image is kept instead of deleted, a notice now says so. Before, that only appeared in the log.
- More reliable folder watching on network or sync-backed locations that don't deliver change notifications.
- Copies large images using less memory.
- Deleting to the Recycle Bin works again for a watched folder whose path ends in a separator. The composed path was not in the form the shell accepts, so the delete failed and the file was kept.

## 0.17.3 ([Aug 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fa893122b9e544bd3605075f8fed427f84e6141a/mods/snap-sentry.wh.cpp))

* Recognizes .tif, .tiff, and .jfif images in a watched folder, alongside the PNG, JPEG, BMP, GIF, and WebP it already handled.

## 0.17.2 ([Aug 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f863c7afea97a688a83ef22cc951a99b9d5111d0/mods/snap-sentry.wh.cpp))

* Say that the watched folder can be your own, in the mod description, the readme, and the folder setting itself
* Shorten the settings descriptions
* New readme screenshot showing the notification with renaming turned on
* Fix a colon in a setting description that broke the settings YAML
* Say what happens when SnapSentry's notifications are turned off in Windows
* Make clear the Recycle Bin covers automatic deletions, not just the popup buttons
* Name the Start Menu shortcut and registry entry the popup leaves, instead of describing them vaguely

## 0.16.0 ([Aug 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e9237a6f4eb990817987ed34d41c00842f43dadd/mods/snap-sentry.wh.cpp))

Initial release.
