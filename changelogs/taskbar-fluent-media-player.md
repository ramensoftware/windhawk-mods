## 1.7.0 ([Nov 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/de5105a2c7c6cfb971bd7e3646713506a961ef71/mods/taskbar-fluent-media-player.wh.cpp))

### The big ones

- **Its own settings window.** Right-click the player and pick Settings. It's built in the Windows 11 style with Mica, sorted into pages, and changes show on the player while you make them. It comes in 15 languages and follows your Windows language by default.
- **Settings live in the mod now.** The first time 1.7 runs, it copies everything from Windhawk's settings page, so you don't lose anything. Save keeps your changes, and closing only warns you if something really differs from what's saved.
- **Move any part of the player.** Album art, track info, buttons and the visualizer can each be detached and placed somewhere else on the taskbar or tray, with their own background and monitor.
- **Progress bar.** It can sit under any part of the player or the whole thing, on its own line or drawn on top. Click or drag it to seek.
- **Vertical taskbars.** If yours is docked left or right, the text and album art turn to read along it and the buttons stack up.
- **Monitor buttons.** Pick a monitor by clicking its button, numbered like Windows. Click the selected one again to hide the player. Monitors that aren't connected stay gray and become available when you plug them in.

### Visualizer

- "Only listen to the app in the player", on by default, so a Discord call or a game won't move the bars (Windows 11). (#93, #85)
- New: it can fill the background behind the whole player.
- New: Line style, and a Micro version that fits next to the artist text. (#16)
- Pick which audio device it listens to, including recording devices. (#17, #50)
- Hide it when nothing is playing. (#14)
- Tune each of the 7 frequency bands.

### Player and layout

- Set the order of album art, text, buttons and visualizer. (#32)
- "Media player offset" moves the player without making the taskbar wider, which helps with floating Taskbar Styler themes. (#88)
- Reorder the media buttons and right-click menu items.
- Hide the whole player while paused, after a delay you choose. (#14)
- Hide the Windows Widgets button.

### Text

- Show the album name instead of the artist, or only when there's no cover. (#38)
- Custom text formats with `%title%`, `%artist%`, `%album%` and `%artist_or_album%`. (#38)
- Show the playback time (like 2:02/4:24) instead of the artist.
- Hide the text when nothing is playing.

### Clicks and apps

- Choose an app to open when you click the player with nothing playing. (#23)
- New click actions: minimize, open or minimize, and close the media app. (#53)
- "Only show these apps" mode, and the app filter moved out of Debug into Behaviour. (#64)

### Mini player

- Seek bar, with its own color. (#62)
- Background options: Acrylic, solid color, album colors gradient or blurred cover. (#48)
- Corner radius setting, and the bottom icon row can be turned off. (#84)

### Settings window extras

- Choose the window's background: Mica, Mica Alt, Acrylic or Acrylic Thin. Acrylic shows what's behind the window, so it works with live wallpapers that Mica can't see, and has its own transparency and blur sliders.
- The title bar can match the window, stay as Windows draws it, or take any color.
- The page fades at its top and bottom edges while you scroll.
- Any color can follow the album, including the window's own accent, which fades to the new color when the song changes.
- Report a bug from inside the window. You choose whether to attach errors, crash info, the log and your settings. Reports also include your PC name and Windows user name, and the form says so.
- Reset to Windhawk's settings, in Other.
- An Animations switch, which also follows Windows' own setting.

### Fixes

**Windows updates**
- Works again after the updates that changed the system tray and the September taskbar update. (#69, #78, #79, #83, #89, #91)
- If the button the player sits next to doesn't exist on your build, the player goes to the start of the tray instead of disappearing.
- The player no longer vanishes when Do Not Disturb is on.

**Startup and stability**
- If the player doesn't make it onto the taskbar at startup, the mod tries again. (#67, #68, #73, #82)
- Media keys keep working after an Explorer restart. (#52)
- The player corrects itself if it ends up on the wrong side of its tray button.
- Security report: fixes for callback handling, the shell32 and windhawk.exe lookups, image handling and unloading. (#71)

**Which app it follows**
- The player stays on the app that's playing instead of a paused one. (#22)
- It no longer flashes another app when tracks change. (#34)
- Ignored apps stay out of session switching, and the ignore list also matches the browser's exe. (#66, #70, #77)

**Album art and colors**
- Covers up to 24 MB show up. (#81)
- Old art no longer stays when a browser track has none. (#33)
- The Album color works on the progress bar, the visualizer and everything else, including with album art turned off.
- With no album art, dynamic visualizer colors go back to your normal color. (#58)
- Solid backgrounds survive switching between light and dark. (#57)

**Visualizer**
- Keeps reacting when you switch the player to another app, like Clipchamp, Firefox or Snipping Tool.
- Much lower GPU use when nothing is playing. (#41)

**Monitors**
- Text scrolling and the visualizer keep working after the player moves to another monitor.

**Input and placement**
- Pause works reliably in Zen browser. (#26)
- Right and middle double-click actions fire again.
- "Hide when no media" no longer keeps taking up space. (#14)
- Fixed the gap with "Right of Widgets", and the player going under the icons with Taskbar Styler "Borderless". (#74)

### Not in this version

- Showing the player on several monitors at the same time is still planned. (#15, #90)

### Thanks

- Net-zen for the StackPanel tray patch (#87).
- Everyone who reported bugs and tested the alphas on the Windhawk Discord.

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
