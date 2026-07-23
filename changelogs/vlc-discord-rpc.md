## 1.2.0 ([Jul 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/166d4f15809125b61e7e9f556c4d3b14ae78a52c/mods/vlc-discord-rpc.wh.cpp))

## ✨ Added

- Added a **multi-engine metadata scraper** for more accurate movie and TV information.
- Added **TVMaze support** for TV shows, providing faster lookups and improving metadata accuracy.
- Added richer metadata, including:
  - Official title
  - Release year
  - ⭐ Rating
  - Genres
  - Runtime
  - High-quality poster artwork
- Added a **30-day local metadata cache** to speed up repeat lookups and reduce unnecessary network requests.
- Reorganized mod settings into clear categories for easier navigation.

## 🚀 Improved

- Improved metadata lookup speed with faster loading times.
- Improved title matching for files containing scene names, quality tags, release groups, and other unnecessary text.
- Improved overall metadata accuracy for both movies and TV shows.
- Improved reliability when fetching metadata and artwork.
- Automatically removes expired cached metadata to keep the local cache clean and up to date.
- Improved compatibility with existing settings so previous configurations continue to work after updating.
- Improved overall stability when enabling, disabling, or reloading the mod.

## 🛠️ Fixed

- Fixed incorrect posters being displayed for movies that are part of a collection (box sets). The correct movie poster is now shown.
- Fixed metadata matching issues caused by noisy or poorly formatted filenames.
- Fixed cases where title cleaning could accidentally remove parts of valid movie or TV show names.
- Fixed duplicate metadata notifications in some situations.

> **Note:** This is the **final release supporting Windhawk v1.x.x**.

## 1.1.5 ([May 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b2662eeac6b52ea789b3647dd224b0c42a19b05a/mods/vlc-discord-rpc.wh.cpp))

### New
- Music activity on Discord now shows:  
  - **Listening to [Song title]**  
  - **Listening to [Artist]**  
  - **Listening to [Album]**  
- VLC port is detected in most setups, so you don’t need to enter the port number manually.  
- The mod now reads your VLC Lua HTTP password directly, so you can use any password without extra setup.  

### Fixes
- Toast notifications no longer repeat or display the wrong track.  
- The image scraper now provides more accurate artwork instead of random results.  
- Port detection works correctly across different VLC configurations, including non‑default ports.  

### Improvements
- Added `wh_log()` entries to make debugging and tracking media simpler.  
- Artwork handling has been refined for sharper and more consistent images.  

Special thanks to **@josephct** for suggesting the music activity feature and for identifying the VLC port issue that could stop Rich Presence from working.

## 1.1.4 ([Apr 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7620480f8843bd0499f3141edd9bb7eae8f8484d/mods/vlc-discord-rpc.wh.cpp))

* **Cleaner TV/Movie Status:** Added toggles to show or hide the *Chapter* and *Audio Language*. You can now set your status to simply show `S01 E01` for a perfectly minimal look!
* **Native Desktop Notifications:** You can now optionally enable sleek Windows Toasts that pop up when a new track plays, complete with automatically cropped, high-res web thumbnails of your movie or album posters!

## 1.1.3 ([Mar 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9f9bae05d7e71f4194161e0eafabfe182458ff1c/mods/vlc-discord-rpc.wh.cpp))

**Fixed slow Discord status updates:**
- Your status now updates instantly when a song or video changes. Cover art uploading now runs in the background, so Discord no longer freezes while waiting for images.

**Fixed local album art not showing:**
- Some users could not upload local artwork because our previous image host (~~`0x0.st`~~) was silently blocking certain connections. We have switched to `uguu.se`, which is more reliable and automatically deletes images after 24 hours.

**New "Minimal Mode" toggle:**
- You can now enable Minimal Mode in the settings. Instead of choosing a separate theme, this toggle hides the play/pause/stop badges while keeping your current theme (such as Dark Mode) active. This allows the cover art to fill the entire square.

**Formatting and search improvements:**
- Episode formatting now uses proper spacing (e.g., S01 E01 instead of S01E01). This improves readability and makes the Search This button results more accurate.

## 1.1.2 ([Feb 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e07ad9a662659b593b03547496fb6930809acbab/mods/vlc-discord-rpc.wh.cpp))

_Fixes & Performance Improvements_

* High CPU Usage Fix: Resolved a performance issue introduced in v1.1.1 where the metadata cleaner caused high CPU load. Implemented a state-change cache so the heavy metadata scrubber only runs when the source media actually changes.

_Advanced Filtering & Processing_

* Dynamic Filter Syncing: The metadata cleaner now remotely fetches and locally caches community-maintained junk word and tag filters. This ensures the cleaner stays highly accurate and up-to-date over time without requiring mod updates.

_Enhanced Customization_

* Modular Mod Settings: Added dedicated toggles in the Windhawk settings to individually enable or disable Quality Tags (`ShowQualityTags`) and the Metadata Cleaner (`clean media titles`) based on user preference.

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
