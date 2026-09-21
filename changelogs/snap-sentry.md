## 0.21.1 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e79dd48287abe987ba9e3869db2ca45983891469/mods/snap-sentry.wh.cpp))

adds an option to clean up identical screenshots after it copies the image. off by default, and it only looks at recent captures from this run, not the older files already sitting in the folder. a duplicate goes to the recycle bin even when normal deletion is set to permanent, so it's always recoverable.

it rechecks the earlier copy before recycling anything. if that one's gone or changed, or the two don't still match byte for byte, the new shot stays. changing a setting cancels a pending cleanup. each capture gets a log line for what happened to it.

## 0.18.8 ([Aug 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d2af22192847759b3de9d7df43f2364b75331c58/mods/snap-sentry.wh.cpp))

- When a multi-page or animated image is kept instead of deleted, a notice now says so. Before, that only appeared in the log.
- More reliable folder watching on network or sync-backed locations that don't deliver change notifications.
- Copies large images using less memory.
- Deleting to the Recycle Bin works again for a watched folder whose path ends in a separator. The composed path was not in the form the shell accepts, so the delete failed and the file was kept.

## 0.17.3 ([Aug 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fa893122b9e544bd3605075f8fed427f84e6141a/mods/snap-sentry.wh.cpp))

* Recognizes .tif, .tiff, and .jfif images in a watched folder, alongside the PNG, JPEG, BMP, GIF, and WebP it already handled.

## 0.17.2 ([Aug 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f863c7afea97a688a83ef22cc951a99b9d5111d0/mods/snap-sentry.wh.cpp))

* The watched folder is described as your own to pick, in the mod description, the readme, and the folder setting.
* Shorter settings descriptions.
* New readme screenshot showing a renamed capture in the notification.
* Fixed a stray colon in a setting description that broke the settings YAML.
* Says what happens when SnapSentry's notifications are turned off in Windows.
* The Recycle Bin note covers automatic deletions, not just the popup buttons.
* Names the Start Menu shortcut and registry entry the popup leaves behind.

## 0.16.0 ([Aug 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e9237a6f4eb990817987ed34d41c00842f43dadd/mods/snap-sentry.wh.cpp))

Initial release.
