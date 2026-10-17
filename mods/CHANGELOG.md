# 📜 Changelog & User Feedback Fixes

All notable changes, enhancements, and bug fixes for **Dynamic Island for Windows** are documented in this file.

---

## [1.4.0] — 2026-10-17

### ✨ New Features

#### 🎵 Live Lyrics (#103)
Real-time synchronized lyrics display with multiple viewing modes:
- **Inline on collapsed island** — current line scrolls directly on the media pill
- **Dedicated lyrics tab** — full scrolling lyrics view in the expanded dashboard
- **Quick Lookup** — right-click any word in the lyrics to search it instantly
- Smooth animations and automatic syncing with playback position

#### 🔄 Cross-Source Media Switching (#103)
Seamlessly switch between different media sources (Spotify, browsers, system audio) without the island losing context. Media state persists across source changes.

#### 💾 Persistent File Tray Storage (#103)
File Tray items now persist to disk storage. Dragged files survive mod restarts and system reboots, making the tray a true parking space rather than a session-only shelf.

#### 🔊 Event-Driven Volume Flyout (#101)
Volume flyout completely rebuilt to be event-driven instead of polled:
- Registers `IAudioEndpointVolumeCallback` for instant updates on every volume step
- Smooth spring animation on the volume bar (overdamped, cannot overshoot)
- `IMMNotificationClient` rebinds when default output device changes (switching from speakers to headphones now works correctly)
- No more 1-second polling lag or repeated nudges while adjusting
- Wakes parked islands on volume changes

#### 📅 Calendar Locale Support (#101)
Calendar now properly follows Windows display language and regional settings. Month names no longer incorrectly uppercased (fixes Turkish "i" → "İ" and preserves CJK characters).

### 🐛 Fixes

- Volume changes under 2% are no longer ignored
- Calendar locale rendering fixed for non-English languages
- Volume bar shows exact percentage while animating smoothly to target

---

## [1.3.0] — 2026-09-24

### ✅ Resolved Issues

Every reported issue addressed in this release, and where to find the change.
Issue numbers without a prefix are on this repository; `ramensoftware#NNNN`
refers to the [windhawk-mods](https://github.com/ramensoftware/windhawk-mods)
tracker the published mod is submitted through.

| Issue | Reported | Addressed by |
| --- | --- | --- |
| [#83](../../issues/83) | Separate Offset Y for collapsed vs expanded | Separate Offset Y For Collapsed And Expanded |
| [#82](../../issues/82) | Weather text not vertically centred | Weather Accuracy & Layout |
| [#81](../../issues/81) | Action Center notifications don't work | Notification Setup Clarified, Notification Listener Resiliency |
| [#79](../../issues/79) | Should disappear when nothing is playing | Auto-Hide Island Across All States |
| [#76](../../issues/76) | Want to disable the volume flyout | `Modules.Volume` toggle — Weather/Volume module toggles |
| [#74](../../issues/74) | Skip forward/back doesn't work | Media Controls Now Actually Respond |
| [#73](../../issues/73) | Shouldn't sit on top and block app tabs | `AlwaysOnTop` off, click-through, fullscreen auto-hide |
| [#72](../../issues/72) | Bottom-left / bottom-right positions | Position & Z-Order |
| [#71](../../issues/71) | 60 FPS not smooth on 144 Hz+ | High Refresh Rate (360Hz+) Support |
| [#70](../../issues/70) | Media buttons unresponsive | Media Controls Now Actually Respond |
| [#69](../../issues/69) | Auto-hide when something is fullscreen | `AutoHideFullscreen` — Auto-Hide Island Across All States |
| [#67](../../issues/67) | More background colour control | Custom Colours & Transparency, Eight New Themes |
| [#66](../../issues/66) | Album art clipping | Album Art Clipping |
| [#65](../../issues/65) | Disable mic/camera privacy lights | Privacy Indicator Customization & Toggles |
| [#64](../../issues/64) | FPS options, 60 FPS laggy on 360 Hz | High Refresh Rate (360Hz+) Support |
| [#63](../../issues/63) | Weather not working | Weather Accuracy & Layout |
| [#62](../../issues/62) | Expands constantly on short-form video | Media Auto-Expand Exclusions |
| [#61](../../issues/61) | Clock and date presentation | Typography & Clock Control |
| [#60](../../issues/60) | Doesn't disappear despite hide setting | Auto-Hide Island Across All States |
| [#59](../../issues/59) | Translucency and more customization | Real Blur & Acrylic Backdrop |
| [#57](../../issues/57) | Hide when untouched | Auto-Hide Island Across All States |
| [#49](../../issues/49) | Privacy dot stays on | Privacy Indicator No Longer Sticks On |
| [#45](../../issues/45) | VLC playback not detected | VLC Detection |
| [#44](../../issues/44) | Game overlay missing, idle-hide broken, wants an expand toggle | Game Overlay Options, Auto-Hide, `MediaAutoExpand` |
| [#41](../../issues/41) | Font face and size for clock/weather | Typography & Clock Control |
| [#35](../../issues/35) | Localization | Multi-Language Support |
| [#33](../../issues/33) | Border-merged mode, Bluetooth view, file tray | File Tray, Border-Merged Mode, Bluetooth View |
| [#31](../../issues/31) | PowerToys bar z-order conflict | Position & Z-Order |
| [#25](../../issues/25) | More game mode options | Game Overlay Options, Game Overlay Rebuilt |
| ramensoftware#4352 | Caps Lock toggle, wind arrows | Caps Lock Toggle & Wind Direction Arrows |
| ramensoftware#4738 | Media pins the island open | Auto-Hide Island Across All States |
| ramensoftware#5086 | Island wastes space, date formatting | Collapsed strip sized to content, Typography & Clock Control |

**Still open, deliberately.** [#27](../../issues/27) asks for local speech-to-text;
that needs a bundled ASR model, which is not something a Windhawk mod can
reasonably ship. [#20](../../issues/20) asks for Fahrenheit, which the
**Weather in Fahrenheit** setting already does — it switches the temperature,
wind speed and feels-like values together.

### 🎨 Visual Redesign

The island is no longer a flat coloured shape. It is now built as a layered material, and every surface shares one design language.

* **Real Depth:** A soft drop shadow now lifts the island off the desktop — previously the shadow routine was an empty stub, so there was none at all. Above it sits a single downward depth gradient: neutral at the top, gradually deeper toward the bottom.
* **No Edge Lighting:** Nothing traces a bright line along an edge any more. Three separate treatments used to: a specular hairline along the top arc, a second full highlight ring just inside the contour, and a white sheen opening the depth gradient. Together they read as a lit rim around the whole island. Inner cards lost their hairline outlines too — six of those side by side in the hardware grid turned into a mesh of bright edges competing with the content — and their fill was raised to carry the separation instead.
* **Live Accent Bloom:** A wide, very soft wash of your album art's accent colour bleeds in from the top of the island. Strongest on the media surface, a whisper elsewhere, so the whole UI feels connected to what is playing. Adjustable from 0 to 200%, or off.
* **One Design Language:** Introduced a small token set (surface, raised, hairline, three text tiers, accent) that every panel, chip, badge, divider and separator now draws from. Twenty-six hardcoded white fills were routed through it, so they all respond to the theme — and to background luminance, meaning a light background now gets dark separators instead of washed-out white ones.
* **Unified Progress:** The media scrubber, volume bar and other progress indicators now share one gradient accent track, so "progress" looks identical everywhere. The scrubber thumb became a white dot with an accent ring, which reads far more precisely against busy album art, and the bar thickens while you drag it.
* **Consistent Panels:** A shared card primitive backs every dashboard panel, giving matched corner radii and fills across the media, calendar, weather, hardware, game and File Tray surfaces.
* **Accent Stays Legible:** The accent is now contrast-checked against the island's own background. The check previously only ever brightened a colour, which silently assumed a dark island — on a light background it drove the accent toward white until it disappeared. It now moves away from the background in whichever direction actually helps, which is what makes the new light theme usable.

#### 🎨 Eight New Themes
* **Retired The Old Four:** OLED Black, Fluent, Midnight Blue and Deep Purple were tuned for a material that lit every edge. With the edge lighting gone a palette has to carry the look on flat fills alone, so the set was replaced: **Obsidian** (true black, OLED friendly), **Graphite** (neutral Windows 11 dark), **Slate** (cool blue-grey), **Nord**, **Evergreen**, **Espresso**, **Plum**, and **Porcelain** — a light theme, which the old set had none of. Secondary text holds roughly 4.5:1 against its own background in every one.
* **Theme Submenu:** Nine entries would have swamped the right-click menu, so themes moved into their own submenu. Menu labels, the settings dropdown and the resolved colours all come from one table now instead of three lists that had to be kept in step.
* **Migrated, Not Reset:** The active theme is stored as an index, and the new set renumbered those indexes. Your selection is migrated across once (Deep Purple lands on Plum, Fluent on Graphite, and so on), and a previously saved custom colour is still recognised as custom rather than being reinterpreted as whichever palette now sits at that number. Custom hex fields you never edited are still treated as untouched even though their defaults moved.

#### 🎮 Game Overlay Rebuilt
* **Frame Rate As The Hero:** FPS is the number you actually watch mid-game, so it gets a wider card and the 18px clock face while the percentages stay at the shared 13.5px.
* **Semantic Load Colour:** The per-metric rainbow is gone (cyan CPU, magenta RAM, green GPU, orange disk) — it encoded nothing and fought the album-art accent. Colour now means load: accent while comfortable, amber from 75%, red from 90%. Cards also lost a hairline border *and* a metric-coloured ring stacked on top of it.
* **One Icon Family:** The overlay had its own icon set drawn at different weights and proportions, so the same CPU appeared as two different symbols depending on the surface. Every card now draws from the shared glyph family, and a new gauge glyph backs FPS — which previously resolved to nothing and drew no icon at all.
* **Sizing Can't Drift:** The strip's animated width and its contents each kept their own copy of the card width and padding, so widening a card in one place left the other sizing the island for the old value and the last card fell off the "ran out of room" check. Both read one layout table now.

#### 📅 Calendar Rebuilt
* **Six-Row Months Fit:** Row height was fixed at 26px, so a month starting late in the week ran past the island's bottom edge and the last row was silently clipped. Row height is now derived from the space actually available.
* **Accent, Not Red:** Unless the accent mode was set to System the calendar fell back to a hardcoded red that answered to nothing and clashed with the accent on every other surface. It uses the real accent now.
* **Today Is The Only Marker:** Weekend columns were painted in the same accent as today, putting three competing marks in the grid and making the weekday headers look selected. Weekends now recede, leaving today as the single accented element, and its marker is sized from its cell instead of a fixed 12px disc that swallowed the digits.
* **Month Names Left Alone:** The month was uppercased character by character, which turns Turkish "i" into "I" rather than "İ" and does nothing at all for CJK.

#### 📐 Separate Offset Y For Collapsed And Expanded #83
* **Two Heights, No Compromise:** A single Offset Y had to serve both states, so a value that tucks the idle pill up near the screen edge left the expanded dashboard awkward to reach. Turning on the separate expanded offset makes Offset Y the collapsed value and adds a second one for expanded. The island eases between them in step with the expansion rather than snapping at a threshold.

#### 🪟 Real Blur & Acrylic Backdrop #59
* **Windows Translucency:** The island can now sit on genuine Windows blur or frosted acrylic instead of a solid fill, using the same composition path the shell uses for its own surfaces. Because that blur covers the window's full rectangle and ignores per-pixel alpha, the window is also shaped to a rounded region while a backdrop is active — otherwise a blurred rectangle would appear in the transparent padding around the island. The drop shadow steps aside in that mode, since the region would clip it anyway.
* **Graceful Degradation:** The entry point is resolved at runtime, so on builds without it the island simply stays opaque rather than failing to load.
* **Also Covered By Existing Settings:** The same report asked for a way to switch off the copy/clipboard flyout and the multi-second popup on every new song. Both already exist — the **Clipboard module** toggle and **Auto-expand on track change** — and the new media blocklist above gives finer control over the latter.

### ✨ New Features

#### 🌍 Multi-Language Support #35
* **12 Languages:** English, French, Spanish, German, Portuguese, Italian, Russian, Turkish, Hindi, Simplified Chinese, Japanese and Korean. Defaults to following your Windows display language, and regional tags resolve correctly (`pt-BR` uses Portuguese, `zh-Hans-CN` uses Chinese). Untranslated text falls back to readable English rather than a blank or an identifier.

#### 📎 File Tray #33
* **Drag & Drop Shelf:** Drag any file or folder onto the island to park it. The island jumps to the shelf to confirm the drop, shows each item's real Explorer icon, name and size, newest first, and highlights the row under your cursor. Click to open, right-click the island to clear. Re-dropping a file promotes it rather than duplicating it. Capacity is configurable from 1 to 25. Nothing is ever copied or moved — the shelf only holds references.

#### 🔠 Typography & Clock Control #41 #61
* **Independent Text Size:** Scale all island text from 70% to 160% without changing the island's dimensions. Previously the only option was the overall Size scale, which grew the whole island.
* **Clock Format:** Choose 12-hour, 24-hour, or follow your Windows locale. Optionally show seconds.
* **Custom Date Patterns:** Set your own date format, including CJK forms such as `yyyy年MM月dd日`. Any unrecognised character is printed literally, so patterns work as typed.
* **Date-First Layout:** Promote the date to the headline with the time beneath it, for people who care about the date more than the clock.

#### 🚫 Media Auto-Expand Exclusions #62
* **Per-App Blocklist:** Short-form video feeds change "track" every few seconds, which turned auto-expand into a constant popup. Name the offenders (for example `tiktok, youtube`) and the island updates quietly in the collapsed pill instead of expanding. Matched loosely against the app and the title, so one entry catches both a desktop app and a browser tab.

#### 🎮 Game Overlay Options #25
* **Choose Your Metrics:** FPS, CPU, GPU, RAM and disk can each be toggled individually. The strip now lays itself out from whichever metrics are enabled and resizes to match, so turning some off closes the gap instead of leaving a hole.
* **Compact Mode:** Narrows the overlay to fit neatly in the taskbar area.

### 🔧 Internal

* **Tab Loop Consolidated:** The dashboard tab arithmetic was copy-pasted in seven places, which is how the renderer and the input handlers drifted out of sync. It now comes from one source of truth, and the expensive GPU/network counters correctly track the Hardware Monitor card's real position rather than assuming it is the last tab.

### ✨ Fixes & Enhancements

#### 🎯 Media Controls Now Actually Respond #74 #70
* **Skip Buttons Fixed:** Clicking previous/next opened the source app instead of changing track. The buttons were drawn relative to the pill, but hit-tested as though the pill were centred in the overlay window — which it is not in macOS Notch or Border-Merged mode (22px offset), while hover-scaled, or whenever a second pill is showing alongside. Every missed click fell straight through to "focus the app". The renderer now publishes the exact rectangle it painted into and all hit-testing is derived from it, so the buttons, the scrubber and the album art can no longer drift away from their targets at any size scale, shape or layout.

#### 🌤️ Weather Accuracy & Layout #82 #63
* **No More Confident Wrong Readings:** A rate-limited or unrecognised-location response from wttr.in was committed as real data, so the island happily displayed "0°" with an empty description. Readings are now only accepted when the response actually parses, and the previous reading is kept otherwise.
* **City Names With Spaces & Accents:** Only spaces were escaped when building the request, so cities containing commas, accents or any non-ASCII character produced a malformed URL and silently failed. The city is now fully percent-encoded as UTF-8.
* **Failure Backoff:** A failed fetch now retries after 60s, then backs off (2m, 5m, 15m) instead of waiting a full refresh interval, without hammering a service that rate-limits.
* **Vertical Centring:** The weather dashboard's city, icon and temperature were pinned to the top of their layout boxes because the bold and huge text formats were the only centred formats missing a paragraph alignment. They are now centred inside explicit bands, which also reclaims the dead space under the dashboard.

#### 🎨 Custom Colours & Transparency #67
* **Background Colour Now Applies:** Editing the pill background hex did nothing unless the Theme preset also happened to be switched to Custom. Any colour field changed from its default now overrides the selected preset; restoring the default hands control back to the preset.
* **Translucent Backgrounds:** The hex parser rejected anything that was not exactly six digits, so an alpha channel was impossible, and the alpha was then discarded twice on the way to the screen. `#RGB`, `#RGBA`, `#RRGGBB` and `#RRGGBBAA` are all accepted now, so a translucent island can be requested while text and icons stay fully opaque.

#### 🔒 Privacy Indicator No Longer Sticks On #49 #65
* **Stuck Orange Dot:** The microphone dot could stay lit permanently, even with the mic unplugged and Windows itself reporting nothing in use. An entry was treated as "in use" whenever its stop timestamp was zero, which is also true of entries that had never been used at all. Both the start and stop timestamps are now required to agree, and the registry values are type- and size-checked. This also unblocks auto-hide, which deliberately refuses to hide the island while a privacy indicator is lit.

#### 🖼️ Album Art Clipping #66
* **Cropped Corner At Large Sizes:** The island's corner radius was scaled twice for the content mask, so from roughly 2× Size scale upwards the rounded corner cut across the album art. Visible mainly at 2.5× in the iPhone Pill shape.

#### 🎵 VLC Detection #45
* **VLC Playing But Island Silent:** Only the session Windows considered "current" was read, so a genuinely playing VLC was ignored whenever a stale or paused session from another app held that slot. A session that is actually playing is now preferred. The window-title fallback also tolerates skins and fullscreen, while still verifying the window really belongs to VLC.

#### 🪟 Position & Z-Order #31 #72
* **Bottom Left / Bottom Right:** Added both positions, completing the set.
* **Reclaiming The Top:** The island stayed buried when another always-on-top window (such as PowerToys' bar) was raised over it, because the z-order was only asserted on a resize. It now reclaims its place once a second — deliberately ignoring Windows' own shell surfaces, so the Start menu, notification centre, OSDs, menus and tooltips still correctly appear in front.

#### 🔔 Notification Setup Clarified #81
* **Missing Prerequisite Documented:** Adding `explorer.exe` to the process inclusion list is not sufficient on its own — Windows must also allow apps to read notifications. Both readmes now say so, and a denied permission is logged with the exact setting to change instead of a bare status code.

#### ⚙️ Hardware Monitor Redesign #42
* **Complete Overhaul:** The Hardware Monitor module was completely redesigned to feature a sleek 2-column grid layout utilizing Segoe Fluent Icons.
* **New Metrics:** Real-time metrics for Network Up/Down speeds (Mbps) and total absolute RAM usage (GB) were implemented using native PDH queries and `GlobalMemoryStatusEx`, giving much deeper insights right from the island!

#### ⚙️ Idle Dashboards (User Feedback)
* **Dynamic Tab Pagination:** The Idle dashboard tabs (Calendar, Weather, Hardware Monitor) now dynamically adapt to your enabled settings. You can seamlessly swipe or mouse-wheel scroll through all active dashboard combinations!
* **Fullscreen Auto-Hide Fix:** Fixed a bug where normal maximized windows (like a regular browser window) would falsely trigger the "Hide on full screen" logic and prevent the island from expanding on hover. The detector now correctly inspects the exact window client area, meaning true full-screen videos hide the island, but standard maximized windows allow it to function perfectly!

#### 🚀 Performance & Stability (Bug Fixes #40)
* **Notification System Rewrite:** Completely rewrote the internal notification tracking logic! Previously, the island tracked the "last seen ID", which caused it to randomly drop and miss new notifications if Windows happened to sort them unexpectedly (e.g. newest-first). It now uses an ultra-reliable memory set (`std::set`) to instantly detect and display any genuinely new notification, ignoring Windows' internal sort order. It also now cleanly skips blasting you with old notifications when starting up!
* **Notification Thread Loop:** Fixed a severe bug where the notification listener would enter an infinite, 0-millisecond tight loop on unsupported versions of Windows, causing high CPU usage and log spam. Additionally, a strict 30-second initialization delay has been added when injecting into `explorer.exe` on boot. This ensures the Windows Notification Service is fully loaded *before* the island connects to it, preventing the connection from becoming permanently corrupted until a manual restart!
* **Media Artwork Caching:** Fixed a major CPU sink where the background media thread was blindly downloading and re-decoding album artwork raw bytes unconditionally every 1.5 seconds. The thread now implements strict caching and validation logic, reusing the parsed image data unless the song title or artist actually changes. This drastically reduces background CPU usage and correctly allows the island to auto-hide as expected while playing music.

#### 🎵 VLC & Browser Media Focus Fixes (User Feedback / Bug Fixes #51 & #53)
* **Media Auto-Expand Toggle:** Added a new `Auto-expand on track change` toggle under the Mod Settings. Turning this off prevents the island from constantly popping open when you are rapidly scrolling through short videos (like Instagram Reels or YouTube Shorts), completely eliminating the frustration of constant popups while still keeping the background media state accurate!
* **Browser Window Focus:** Fixed an annoying bug where clicking the media pill to focus a playing browser (like Vivaldi, Chrome, or Edge) would launch a brand new empty window if you had switched tabs. The island will now smartly search for the existing window belonging to the browser process and bring it to the foreground instead of launching a new instance!
* **Missing Media Data:** Some versions of VLC fail to correctly pass the current media title to the Windows Media Transport Controls. The island will now automatically detect this missing data and fallback to safely extracting the playing video/song title directly from VLC's window title, ensuring media controls remain useful!

#### 📐 Shape Style Settings Consolidation (User Feedback / Refactor)
* **Combined Shape Settings:** The `Native Windows 11 style` and `macOS Notch style` settings have been combined into a single, clean `Island Shape Style` dropdown menu. This prevents mutually exclusive toggles from being active at the same time and makes customizing the island's shape much more intuitive.


#### ⛅ Weather Module Toggle (User Feedback / Feature Request)
* **Disable/Hide Weather Widget:** Added a `Weather module` master toggle under the Mod Settings. When disabled, the inactive Dynamic Island pill will perfectly shrink and center only the digital clock for a minimal setup. It also hides the weather widget completely from the expanded view (leaving only the calendar) and pauses all background network fetches to `wttr.in`, improving performance.

#### 👆 Touch Swipe Gestures (User Feedback / Feature Request #54)
* **Touch-Friendly Navigation:** For users on Surface Pro or other Windows tablets without a mouse wheel, the dynamic island is now fully touch-friendly! You can now physically swipe left or right on the island to effortlessly cycle between background apps (like swiping between Weather, the Clock, and active Media controls) just like on a smartphone.


#### ⌨️ Caps Lock Toggle & Wind Direction Arrows (User Feedback / Feature Request ramensoftware#4352)
* **Caps Lock Indicator Toggle:** Added a `Caps Lock module` toggle under the mod's Modules settings, allowing users to disable the Caps Lock / Num Lock island indicator completely if they find it distracting.
* **Wind Direction Arrows:** The weather dashboard now translates wind direction abbreviations (e.g., N, WSW, NE) into clear Unicode arrows (e.g., ↓, ↗, ↙) reflecting the actual meteorological wind flow direction for a cleaner visual layout.


#### ⏭️ Media Progress Bar Seeking & Consistent Album Art App Launching (User Feedback / Feature Request ramensoftware#4738)
* **Problem / Request:** Users suggested allowing clicks on the media progress bar to seek playback position (`TryChangePlaybackPositionAsync`), and noted that clicking anywhere on the media module (e.g. title text or background) opened the underlying media application, which was inconsistent and prone to accidental app launches.
* **Fix & Features Added:**
  * **Interactive Scrubber Seeking:** Clicking anywhere along the horizontal media progress bar in expanded media mode now instantly computes the track fraction and sends `session.TryChangePlaybackPositionAsync(targetTicks)` to the active WinRT media session. The local position state updates immediately (`positionTicks = targetTicks`) so the progress bar scrubber visually jumps right to the clicked location without delay.
  * **Restricted App Launching (`clickedAlbumArt`):** Clicking on the song title, artist, waveform, or background no longer triggers `OpenRelevantApp()`. The source application is now strictly opened ONLY when the user clicks directly on the square Album Art image (`clickedAlbumArt`) in either expanded or collapsed media mode.
  * **Unified Button Hit-Testing:** Standardized playback button hit-testing (`cmd 0, 1, 2`) across both `WM_LBUTTONDOWN` and `WM_LBUTTONUP` (`cy = 56.0f`), ensuring smooth and responsive interaction whether using floating Apple pill style or top-attached macOS Notch style.

#### 🎵 Community Fork Integration: Album Title Support, Weather Word Wrapping & Stability Enhancements (ChrisSch-dev Fork Analysis & Integration)
* **Overview:** Analyzed and integrated key features and stability fixes from `ChrisSch-dev`'s community fork (`dynamic-island-for-windows.wh-fork.cpp`), adding full credit to the mod readme (`// ==WindhawkModReadme==`).
* **Features & Improvements Added:**
  * **Album Title Display (`albumTitle`):** Extracted `properties.AlbumTitle()` from WinRT media sessions and integrated `albumTitle` into `MediaSnapshot`. When playing media with album information, the album title is now displayed cleanly with subtle opacity below the artist name in expanded media mode.
  * **Intelligent Weather Description Word Wrapping:** Upgraded `DrawWeatherDashboard()` description rendering from plain single-line `DrawTextW` to `IDWriteTextFormat` with word wrapping (`DWRITE_WORD_WRAPPING_WRAP`) and dynamic font sizing (`9.8f` to `13.5f` based on description character length), preventing long weather summaries from overflowing or clipping outside the expanded weather panel.
  * **Sleep & Power Resume Auto-Recovery (`WM_POWERBROADCAST`):** Added `WM_POWERBROADCAST` handling for `PBT_APMRESUMESUSPEND`, `PBT_APMRESUMEAUTOMATIC`, and `PBT_APMRESUMECRITICAL`. When the PC wakes from sleep or hibernation, the mod automatically signals settings refresh events, resets weather timestamps (`lastUpdated = 0.0`) to trigger immediate weather updates, and nudges the overlay to restore UI responsiveness instantly.
  * **Robust Keyboard Hook & CapsLock Synchronization (`WM_APP_CAPSLOCK`):** Fixed race conditions during startup where `KeyboardThreadProc` hooked keys before `g_hwnd` was fully initialized (`while (!g_hwnd) Sleep(10);`). Upgraded `LowLevelKeyboardProc` to capture exact synchronous key states (`GetKeyState`) and transmit them inside `WM_APP_CAPSLOCK` (`lParam`), guaranteeing accurate Caps Lock / Num Lock notification badges.
  * **GameOverlay Expansion & Movement Stability:** Added checks for `gameMetricsPresent` (`g_settings.gameOverlay` or `GameOverlayPinned`) inside `ChooseIslandActivity` (`WM_LBUTTONUP`), `UpdateLayout`, and `Renderer::Draw` (`hoverScale`) to prevent unwanted hover scaling, click expansions, or spring bounces when the thin game overlay pill is active. Also removed redundant `TriggerNudge()` calls from `CaptureClipboard` to eliminate unintended jitter or spring oscillations during background clipboard updates.


#### 🔔 Notification Listener & Shell Hook Resiliency Across PC Reboots (User Feedback / Bug Report #40)
* **Problem / Request:** Users reported that notifications (like WhatsApp) stopped working after restarting their PC unless they disabled and re-enabled the mod or removed/re-added `explorer.exe` to the process inclusion list on every reboot.
* **Fix & Features Added:**
  * **WinRT UWP Notification Listener Retry Loop:** When the mod initializes early during system boot (`windhawk.exe` service start), the UWP/WinRT notification platform (`wpnservice` / `UserNotificationListener`) is often not ready yet or access has not yet settled for the interactive user session. Previously, if `RequestAccessAsync()` returned denied or threw an exception right on startup, the notification listener thread permanently exited (`return 0;`). Implemented a resilient outer connection loop that retries connection (`WaitForSingleObject(g_stopEvent, 3000)`) instead of terminating, ensuring the WinRT listener connects automatically as soon as the user logs in and the notification subsystem comes online.
  * **Shell Hook Recovery on Explorer Start (`TaskbarCreated`):** Classic Shell Hook notifications (`CaptureShellNotification`) become disconnected whenever `explorer.exe` starts up after `windhawk.exe` during boot or when Windows Explorer restarts. Added listening and registration for the `TaskbarCreated` system message (`RegisterWindowMessageW(L"TaskbarCreated")`), so the overlay window automatically re-registers its shell hook (`RegisterShellHookWindow`) whenever the taskbar/shell initializes.
  * **UIPI Message Filtering (`ChangeWindowMessageFilterEx`):** Added explicit `ChangeWindowMessageFilterEx` (`MSGFLT_ALLOW`) rules for `SHELLHOOK`, `TaskbarCreated`, `WM_COPYDATA`, and `WM_COPYGLOBALDATA` when running under elevated or admin integrity levels, bypassing Windows User Interface Privilege Isolation (UIPI) so notifications broadcast from `explorer.exe` and `sihost.exe` reliably reach the overlay across all boot scenarios.

#### 🟠 / 🟢 Privacy Indicator Customization & Toggles (User Feedback / Feature Request #34, #36)
* **Problem / Request:** Users requested toggles to turn off the microphone and camera privacy dots (noting that background apps like Discord or OBS keep the mic dot permanently active), as well as controls to disable the pulsing animation and customize the dot colors.
* **Fix & Features Added:**
  * **Master & Individual Toggles:** Added `Show privacy indicators (Mic & Camera)` master toggle, along with independent `Show microphone indicator (Orange dot)` and `Show camera indicator (Green dot)` toggles under the **Modules & Features** settings block. Users can now turn off just the microphone dot while keeping the webcam alert active.
  * **Pulsing Animation Toggle:** Added `Privacy dots pulsing animation` (`PrivacyDotsPulse`). When disabled, privacy dots render as clean, static indicators without the breathing pulse.
  * **Custom Hex Colors:** Added `Microphone dot custom hex color` (`PrivacyDotsMicHex`) and `Camera dot custom hex color` (`PrivacyDotsCamHex`) so users can customize privacy dot colors beyond the default iOS Orange and Green.
  * **Optimized Background Queries:** When privacy indicators are disabled via settings, background registry polling (`IsMicrophoneActive` / `IsCameraActive`) is bypassed entirely, reducing background CPU and registry overhead.

#### ⏱️ Auto-Hide Island Across All States (User Feedback / Feature Request #43, #39, #32)
* **Problem / Request:** Users reported that the auto-hide setting (`AutoHideIdleSeconds`) only hid the island when it was completely idle, meaning background activities like playing media (Spotify, YouTube) or ongoing progress indicators would stay visible on screen continuously without ever auto-hiding after inactivity.
* **Fix & Features Added:**
  * **Universal Auto-Hide Support:** Updated `AutoHideIdleSeconds` to apply across all ongoing states (`Idle`, `Media`, and `Progress`).
  * **Intelligent Transient & Interaction Protection:** When a new event occurs (such as a song track change, clipboard copy, notification, volume adjustment, mouse hover, or pinned state), the island unhides immediately to show the update and stays awake.
  * **Smooth Auto-Hiding:** Once `AutoHideIdleSeconds` of inactivity elapse without further interaction or track changes, any active state smoothly collapses and hides until the next interaction or notification.
  * **Updated Settings Metadata:** Renamed option in Windhawk settings UI to `Auto-hide island (all states)` (`AutoHideIdleSeconds`) with clarified description explaining universal state support.

#### 🍏 Native macOS Notch Style Mode (User Feedback / Feature Request)
* **Problem / Request:** Users requested an option similar to the native Windows 11 style, but designed to resemble a MacBook notch where the island sticks flush to the top edge of the upper screen.
* **Fix & Features Added:**
  * **New Mod Setting (`MacOsNotchStyle`):** Added `macOS Notch style` toggle under the Appearance settings block.
  * **Top-Attached Screen Ceilings:** When enabled, the window position automatically anchors flush against the monitor top edge (`y = 0`) instead of floating 8 pixels below it, ensuring a true top-attached notch appearance.
  * **Custom Direct2D Notch Path Geometry:** Implemented `CreateNotchGeometry()` and `CreateIslandMaskGeometry()`, generating an authentic MacBook notch shape with flat top corners and smoothly curved bottom corners (`16.0f` scaled radius).
  * **Right-Click Context Menu Toggle:** Added a convenient right-click menu item ("Use macOS Notch Style" / "Disable macOS Notch Style") to switch seamlessly between Apple floating pill, Windows 11 flyout box, and macOS Notch styles.
  * **Precision Media Click Detection:** Updated window coordinate calculations in `WndProc` so interacting with media buttons and dashboards works with exact precision whether floating or attached in notch style.

#### ❌ Right-Click Menu Dismiss Button Functionality & Media Visibility Fix (Feedback / Issue #32)
* **Problem:** Clicking "Dismiss" from the right-click context menu would cause the Dynamic Island to disappear permanently, even if media (e.g. Spotify, YouTube) was currently playing in the background. The island would remain hidden until a new event (such as a media title change or clipboard copy) occurred.
* **Fix & Features Added:**
  * **Comprehensive Transient State Clearing:** Updated `DismissTransientState()` to reliably clear all transient popups (Clipboard, Notifications, Volume, Progress, Caps Lock, Device, and Battery low alerts) while keeping active media state intact.
  * **Immediate Layout Refresh & Nudge:** Added immediate `g_layoutDirty = true`, `g_clickExpanded = false`, and `TriggerNudge()` calls when the Dismiss menu command is executed.
  * **Instant Media Restoration:** When dismissing an overlaying notification or clipboard popup while media is playing, the island now smoothly and immediately animates back to the active Media pill or dashboard without ever hiding or freezing.

#### 📋 Customizable Clipboard Icon Background Style (Feedback / Issue #30)
* **Problem:** When copying text or images, the clipboard preview displayed a hardcoded gray background behind the icon that could not be modified or removed via settings.
* **Fix & Features Added:**
  * Added **`ClipboardIconBgStyle`** under the **Modules** settings block with 4 customizable modes:
    * `default`: Standard subtle gray background badge.
    * `transparent`: Eliminates the background box entirely for a clean, outline-only floating icon look.
    * `accent`: Uses the island's glowing accent color with soft opacity.
    * `custom`: Uses a custom hex color.
  * Added **`ClipboardIconBgHex`** setting to define the exact hex color when using Custom mode (default `#2E2E38`).
  * Updated `DrawClipboard` rendering logic so both the outer badge box and inner fallback icon plate respect the selected background style.

#### 🚀 High Refresh Rate (360Hz+) Support & Precise Animation Controls (Feedback / Issue #26)
* **Problem:** Animations felt stuttery or buggy on high refresh rate monitors (144Hz, 240Hz, 360Hz+), and users lacked precise control over animation physics.
* **Fix & Features Added:**
  * **Zero-Spin Frame Pacing:** Replaced hardcoded 16ms sleep (~60 FPS) with a CPU-efficient hybrid timing system using 1ms OS timer resolution (`timeBeginPeriod`) and thread yielding. Achieves buttery-smooth presentation on 360Hz/500Hz panels while maintaining **0.00% CPU usage at idle**.
  * **Automatic Monitor Hz Detection:** Implemented `GetMonitorRefreshRate(HWND hwnd)` to detect and match native display frequency automatically when set to `Auto`.
  * **2000Hz Deterministic Physics Sub-Stepping:** Upgraded `SpringValue::Step` to use a fixed timestep (`0.0005s`), ensuring identical spring behavior and stability regardless of framerate.
  * **New Mod Settings (`TargetFPS`, `AnimationStyle`, `AnimationSpeed`):** Added options for target FPS up to 500 FPS, spring styles (`smooth`, `default`, `bouncy`, `snappy`), and 6 granular animation speed levels (0.5x to 2.0x).
