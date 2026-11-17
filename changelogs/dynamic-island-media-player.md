## 1.6.1 ([Nov 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/479c5a5aaa4179b90455640c2e9597457b72b6aa/mods/dynamic-island-media-player.wh.cpp))

- Improved rendering performance by limiting automatic rendering to 60 FPS and caching foreground application and monitor DPI information.
- Optimized overlay window positioning by avoiding redundant window-position updates during animations and hover effects.
- Improved album artwork loading by retrying more frequently when artwork is unavailable or delayed.
- Avoided redundant album artwork decoding when the thumbnail image has not changed.
- Improved global media hotkey compatibility in games and other applications by combining `RegisterHotKey` with a Raw Input fallback.
- Added duplicate-dispatch protection to prevent a single key combination from triggering the same action through both input mechanisms.
- Improved keyboard input lifecycle management, including Raw Input registration cleanup and synchronization with hotkey settings.
- Moved low-level keyboard-hook installation and removal for hotkey capture to a dedicated thread.

## 1.6.0 ([Nov 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1ae41698ee287ad9e577aa72fc23dc295d21d8b5/mods/dynamic-island-media-player.wh.cpp))

Initial release.
