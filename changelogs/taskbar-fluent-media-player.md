## 1.6.0 ([Aug 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/92c7152155eea8079f5a6d52dfd8013b06b27752/mods/taskbar-fluent-media-player.wh.cpp))

### Added
- **Mini Player Flyout Menu** — new compact popup menu that appears above the taskbar when clicking the media player.
  - Large centered album art
  - Current track title and artist displayed below album art
  - Large control buttons for Previous/Play-Pause/Next/Shuffle/Repeat
  - Session list showing all active media sessions with thumbnails, titles, and artists
  - Current session highlighting with animated pill indicator
  - Click session rows to switch between apps
  - Smooth slide-up animation from taskbar
  - Bottom settings bar with Windhawk button
  - Theme-aware styling with acrylic background
  - **Flexible placement** — menu can be positioned anywhere on the screen via Player Menu Settings: choose any corner or edge with configurable distance offsets.
  - **Show session list** — option to hide the media session list in the mini player menu, showing only the large controls and current track info.

- **"Auto-switch to active media session"** — when enabled, the player switches to a different session that is actively Playing when the current session pauses or stops. If no other session is playing, the player stays on the current session — so pressing Pause no longer causes an unwanted session change.
- **Full-height invisible hit area** for the taskbar media player — the entire taskbar height now responds to hover and click events even if the player itself is shorter than the taskbar, making it easier to interact with.
- **Monitor setting** in Main Settings → Media player — choose which monitor's taskbar the player is injected into (1, 2, 3...). Defaults to 1.

### Changed
- **Smooth hover animation** — player button and all media control buttons now fade in/out on hover and press using a native `BrushTransition`. Background changes are applied directly to the template border so WinRT can interpolate them, replacing the previous instantaneous `DiscreteObjectAnimation`.
- **"Switch tracks (inverted)"** option in Mouse Wheel Actions — scrolling up goes to the next track and scrolling down goes to the previous one, the inverse of the default behavior. Available for both the player area and the album art area.
- **Album art color contrast correction for the visualizer** — album colors that lack sufficient contrast against the taskbar are now tinted with the theme's base color (brightened on dark theme, darkened on light theme) so the visualizer bars remain visible when using Dynamic Album or Dynamic Gradient color modes.
- **Improved hover tooltip** — previously the track title and artist name each had their own separate tooltip; now a single tooltip appears over the entire player showing the track title and artist together.
- **Visualizer color caching** — palette and color-string values are now cached and only recomputed when the album art or system theme changes, eliminating ~180–240 registry reads per second while the visualizer is visible.

### Fixed
- **Visualizer idle bar height 0** now fully hides bars at rest — previously setting the second value of "Bar size" to 0 still showed bars at the bar-width height; bars now grow from zero when idle height is 0. [#14](https://github.com/Salyts/Taskbar-Fluent-Media-Player/issues/14)
- **Play/Pause button unreliable on some browsers** (e.g. Zen/Firefox) — replaced `TryTogglePlayPauseAsync` with an explicit `TryPauseAsync` / `TryPlayAsync` based on the current playback status, eliminating the race condition that caused ~50% miss rate. [#26](https://github.com/Salyts/Taskbar-Fluent-Media-Player/issues/26)
- **Widget flickers to another player during track changes** — when the active session briefly leaves the Playing state while switching tracks (e.g. Spotify), the player no longer jumps to a paused secondary session. [#34](https://github.com/Salyts/Taskbar-Fluent-Media-Player/issues/34) [#22](https://github.com/Salyts/Taskbar-Fluent-Media-Player/issues/22)
- **Visualizer capture thread no longer blocks the taskbar UI thread** on show/hide transitions — `StopVizCaptureThread` is now called from a background worker, preventing potential freezes when the audio service takes time to release the endpoint.
- **Right double-click and middle double-click actions not firing** — `right_double_click` and `middle_double_click` bindings were configurable in settings but silently did nothing. All six click types (left/right/middle × single/double) for both the player area and album art now work correctly.
- **Stale album art shown after switching to a track with no artwork in Chromium-based browsers** — Chrome/Edge/Brave/Opera/Vivaldi do not clear the SMTC thumbnail when the new track has no artwork, causing the previous track's cover to persist indefinitely. The player now detects this case: on every title/artist change from a browser session the previous thumbnail is marked as suspect, and if the next poll returns the same image it is discarded and the placeholder is shown instead. [#33](https://github.com/Salyts/Taskbar-Fluent-Media-Player/issues/33)
- **Removed non-functional `mouse_wheel_up` and `mouse_wheel_down` options** from Mouse Wheel Actions — these options appeared in the settings UI but never triggered any action. Scroll direction is already handled per-action (e.g. `switch_tracks` scrolls forward on wheel-up and backward on wheel-down).

## 1.5.0 ([Jul 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/145b63f3d04a7f9c04353a6ac87a2d9a4314103a/mods/taskbar-fluent-media-player.wh.cpp))

### Added
- **Context Menu Settings** section
  - Configurable item list — choose which items appear in the context menu and in what order
  - **Repeat style** — submenu (Repeat off / Repeat all / Repeat one) or a single toggle button that cycles through modes and reflects the current state in its label
  - **Shuffle style** — submenu (Shuffle off / Shuffle on) or a toggle button with live on/off label
  - **Context menu icon style** — pick Fluent Icons or MDL2 Assets (Outline/Filled), or inherit from Media Buttons Style
  - **Context menu icon color and opacity** — independent from player button color settings
  - **"Show Open Windhawk button"** toggle — the separator above it hides automatically when both it and Restart Player are off
- **"Nothing" option** in Media buttons order and Context menu items lists — useful as an empty placeholder slot

### Fixed
- Idle auto-hide timer now ticks independently of media events — timeout values above 1 second now work correctly [#5](https://github.com/Salyts/Taskbar-Fluent-Media-Player/issues/5)
- Player no longer disappears when **Hide when no media** is disabled and **Idle auto-hide** is set to 0
- When title or artist placeholder text is empty, the scroll-view container now collapses properly — a single visible line is displayed without the hidden element reserving space

### Changed
- **Visualizer idle bar height** is now specified in pixels (0–15 px) instead of percentage — default changed from 15% to 3 px
- Bar width minimum is now 0 (hides bars entirely) instead of 1
- `ExtractAlbumPalette` now uses the internal `DecodeImageToBGRA` helper instead of the WinRT `BitmapDecoder` pipeline

## 1.4.0 ([Jun 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1fa4d7fa3995d334df2bba31d035d361b851414a/mods/taskbar-fluent-media-player.wh.cpp))

### Added
- **Real-time audio visualizer** (WASAPI loopback + FFT)
  - 5 bar shapes: Stereo, Mountain, Mirror, Wave, Breathe
  - 5 color modes: Solid, Dynamic Album, Dynamic Gradient, Custom Gradient, Acrylic
  - EQ presets: Balanced, Bass, Rock, Pop, Jazz, Electronic
  - Configurable position (left/right of player), vertical anchor (top/middle/bottom), bar count/width/gap, idle bar size, padding, and sensitivity
- **Right-click context menu on the player**: Toggle Repeat/Shuffle, Rewind/Forward 5s, Next/Previous Track, Switch Sessions, Open media app, optional Restart Player, Open Windhawk
- **"Switch Sessions" button/action** — new media button type, also available as a mouse/context-menu action
- **Custom empty album art icon** — replaces the fixed "question mark"/"music note" presets with a configurable glyph (hex code), font, size, color, and opacity
- **Per-theme color support** — almost every color field now accepts two values separated by `$` (e.g. `0 0 0$255 255 255`) for light/dark theme: background, button icons, title, artist, empty-art icon, visualizer
- Separate pause-overlay icon size control
- Dedicated second color for the gradient background (`gradientColor2`), independent from the two-tone solid color
- Debug setting to toggle the "Restart Player" entry in the context menu
- Customizable placeholder texts for the title/artist fields — separate, editable strings for "no title on the current track", "no artist on the current track", "no media session at all" (title and artist), each can be left empty to hide that field entirely in that state

### Changed
- "Spacing between buttons" and "Button icon size" moved from Main Settings to Appearance → Media Buttons Style
- Reworked player hover/press visuals — new gradient "elevation" border instead of manual brush swapping per state
- Buttons renamed for clarity: Previous → Previous Track, Next → Next Track, Shuffle → Toggle Shuffle, Repeat → Toggle Repeat
- Removed the standalone "Auto theme" toggle — replaced by the universal light$dark color-pair support across all elements
- The title/artist text container now collapses entirely (no reserved space) when both fields end up empty, instead of just hiding the individual text blocks

## 1.3.0 ([Jun 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5a2f0ea730d8c70bfd9a4964909c0b6f29bfcf17/mods/taskbar-fluent-media-player.wh.cpp))

### Added
- **Text scrolling for long track titles and artist names**
  - Enable/disable scrolling for title and artist separately
  - Adjustable scroll speed (1-10)
  - Two scroll modes: bounce (back and forth) and loop (continuous)
  - Configurable pause duration and spacing between repeats
- **Individual corner radius control** for player, buttons, and album art
  - Use single value (e.g., '4') for uniform corners
  - Use four values (e.g., '4 2 4 2') for individual corner customization
 
### Changed
- More compact spacing between title and artist by default
- Improved visual feedback when clicking on the player

## 1.2.0 ([Jun 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f0bc654c79f5fc11c5509aba5c2428922694388a/mods/taskbar-fluent-media-player.wh.cpp))

Initial release.
