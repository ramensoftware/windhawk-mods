## 1.1.1 ([Feb 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/77a37e151ece2d18a73ac9e1b3cfab3abac18a6e/mods/vlc-discord-rpc.wh.cpp))

Metadata & Visuals

* External Artwork Fetching (new): If a media file lacks embedded artwork, the mod now seamlessly attempts to find and display relevant cover art or movie posters online.

* Advanced Metadata Cleaning: Introduced aggressive filtering to remove junk text (like website names, resolutions, or release group tags) from titles and artist names, resulting in a much cleaner Rich Presence.

* Release Year Parsing: Added support for extracting the release year from filenames to improve metadata accuracy and artwork matching.

## 1.1.0 ([Feb 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4661e78857e35dcac8fa19ae55234b73cd322d6d/mods/vlc-discord-rpc.wh.cpp))

_Visual Enhancements_

* Cover Art Integration **(new)** : Discord now displays actual album art and movie posters when available (for files with embedded artwork).
* Privacy Options: Added a new setting to disable cover art display and revert to the classic VLC icon.
* Progress Bar: Replaced plain timestamps with a functional progress bar.

_Smarter Status Updates_

* Contextual Status: Activity status now adapts automatically **(new)** :

  * Listening to [Song Title] for music.
  * Watching [Movie Title] for videos.

* Audio Language: Active audio track language (e.g., English, Japanese) is now shown for video files.

_Improvements & Fixes_

* Improved Layout:

  * Music: Displays Song, Artist, and Album (album shown on hover).
  * TV Shows: Automatically detects and shows Season/Episode format (e.g., S01E05).

* Technical Fixes:

  * Quality Indicators: Fixed display of tags for 4K, HDR, 10-bit, and 1080p content.

## 1.0.3 ([Jan 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fad163cf3c8246732c75ac49aa098666cbebd786/mods/vlc-discord-rpc.wh.cpp))

Initial release.
