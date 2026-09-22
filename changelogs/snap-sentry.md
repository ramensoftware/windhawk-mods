## 0.21.2 ([Sep 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b69a191a8b6059d3f3a929a4e4e7526c9bc06f45/mods/snap-sentry.wh.cpp))

* Bug reports and feature requests now go to the SnapSentry issues page on GitHub.

## 0.21.1 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e79dd48287abe987ba9e3869db2ca45983891469/mods/snap-sentry.wh.cpp))

* Added an optional setting to remove duplicate screenshots. When the same image is captured again, the extra copy is moved to the Recycle Bin so the folder does not fill up with identical shots. It is off by default.
* Only exact, byte-for-byte duplicates taken during the current session are removed, and only while SnapSentry is set to copy the image itself. Files that were already in the folder are left alone.
* A removed duplicate always goes to the Recycle Bin so it can be restored, even when normal deletion is set to permanent. The earlier copy is kept, and SnapSentry confirms it is still there before removing anything.
* The log now records the outcome for each screenshot: copied, kept, recycled, skipped, or deleted.
* Screenshots are still copied to the clipboard when Windows cannot show a notification.
* Fixed a watched folder being ignored when its path had a stray space at the start or end.

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
