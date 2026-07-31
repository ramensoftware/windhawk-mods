// ==WindhawkMod==
// @id              taskbar-media-presence
// @name            Taskbar Media Presence
// @description     Taskbar music controls with adaptive album art, per-app volume, safe multi-monitor placement, and Discord Rich Presence.
// @version         1.0.23
// @author          MrBoxik
// @github          https://github.com/MrBoxik
// @homepage        https://github.com/MrBoxik/Taskbar-Media-Presence
// @donateUrl       https://buymeacoffee.com/mrboxik
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lruntimeobject -luuid -luser32 -lwindowsapp -lshell32 -lgdi32 -lshlwapi -lwindowscodecs -ldwmapi -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Media Presence 1.0.23

A compact Windows 11 taskbar player for any application that publishes a Windows media session.

![Taskbar Media Presence showing playback controls, album artwork, track information, volume control, and a clickable progress bar](https://raw.githubusercontent.com/MrBoxik/Taskbar-Media-Presence/main/assets/taskbar-media-presence-preview.png)

## Highlights

- Album art, title, artist, stable clickable playback progress, Shuffle, Repeat, Previous, Play/Pause, Next, and per-app volume.
- A single optional MusicBee companion adds its Windows media session and exact internal-volume control.
- Selected-monitor or compatible all-monitor placement with reserved presets and custom overlay anchors.
- One-way title marquee, ellipsis, and bounce modes.
- Transparent widget and optional transparent Windows taskbar. Treat the taskbar option as a convenience setting; do not combine it with a dedicated taskbar background or transparency mod.
- Context-menu choices persist across Explorer and Windows restarts.
- Opt-in Discord Rich Presence with playback progress and a configurable static artwork asset.

> The widget is hidden by default until a compatible media session is active. Start playback before checking whether installation succeeded.

## Compatibility

The player must expose a GSMTC/SMTC media session. Shuffle and Repeat appear disabled when the media application does not support them. Most players work directly through their Windows media and per-application audio sessions. When using a dedicated taskbar styling or transparency mod, leave **Transparent taskbar** disabled so only one mod owns the taskbar opacity.

## MusicBee companion (optional)

MusicBee users can install the project-owned [`mb_TaskbarMediaPresence.dll`](https://github.com/MrBoxik/Taskbar-Media-Presence/releases/latest/download/mb_TaskbarMediaPresence.dll) to publish a reliable media session and expose MusicBee's exact internal volume.

1. Download `mb_TaskbarMediaPresence.dll`.
2. In MusicBee, press **Ctrl+O**, or open **Edit > Preferences**.
3. Open **Plugins**, choose **Add Plugin**, and select the downloaded DLL.
4. Restart MusicBee and confirm that **Taskbar Media Presence** is enabled in the Plugins list.

The Windhawk mod never downloads or launches the companion automatically; installation is a separate manual choice. The companion source and build project are available in [`MusicBeeVolumeBridge`](https://github.com/MrBoxik/Taskbar-Media-Presence/tree/main/MusicBeeVolumeBridge).

Discord publishing is disabled by default. When enabled, the mod sends the selected track, artist, playback state, and progress to the local Discord desktop client. Discord then broadcasts that activity to the user's profile according to their Discord activity-privacy settings. The default Discord Application ID belongs to this project and is owned by MrBoxik, so activity is attributed to **Taskbar Media Presence** unless the user replaces the ID with their own. The mod performs no external HTTP requests itself.

## Links

- **[Source code and documentation](https://github.com/MrBoxik/Taskbar-Media-Presence)**
- **[Report a bug or request a feature](https://github.com/MrBoxik/Taskbar-Media-Presence/issues)**
- **[Support the project](https://buymeacoffee.com/mrboxik)**

## Credits

Created and maintained by **[MrBoxik](https://github.com/MrBoxik)**. Based on **[Taskbar Fluent Media Player](https://github.com/Salyts/Taskbar-Fluent-Media-Player)** by Salyts and distributed under the MIT License.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- MainSettings:
  - PlayerSetting:
    - taskbarMode: "selected"
      $name: Taskbar monitor mode
      $description: Show one widget on the selected monitor, or mirror it on every Windows taskbar.
      $options:
      - "selected": "Only selected monitor"
      - "all": "All monitors"
    - taskbarNumber: 1
      $name: Taskbar monitor number
      $description: Used in Only selected monitor mode. 1 is primary; 2 and above are secondary taskbars ordered left to right.
    - positionPreset: "apps"
      $name: Position preset
      $description: Next to apps, Left, and Right reserve taskbar space. Middle uses the free far-left area when Windows buttons are centered, or a center overlay when Windows buttons are left aligned. Choose Custom for a detailed anchor and offset.
      $options:
      - "left": "Left side before Start (no app overlap)"
      - "apps": "Immediately after taskbar applications (no overlap)"
      - "middle": "Adaptive middle / free far-left"
      - "right": "Right side before system tray (no app overlap)"
      - "custom": "Custom anchor"
    - position: "taskbar_after_apps"
      $name: Custom position anchor
      $description: Used only when Position preset is Custom. Overlay choices can cover taskbar apps; tracked and tray choices reserve space.
      $options:
      - "taskbar_left_edge": "Taskbar - Left edge (Overlay)"
      - "taskbar_center_edge": "Taskbar - Center (Overlay)"
      - "taskbar_right_edge": "Taskbar - Right edge (Overlay)"
      - "taskbar_left_start": "Taskbar - Left of Start button"
      - "taskbar_right_start": "Taskbar - Right of Start button"
      - "taskbar_after_apps": "Taskbar - After application buttons"
      - "taskbar_after_search_left": "Taskbar - Left of Search button"
      - "taskbar_after_search_right": "Taskbar - Right of Search button"
      - "taskbar_after_taskview_left": "Taskbar - Left of Task View button"
      - "taskbar_after_taskview_right": "Taskbar - Right of Task View button"
      - "taskbar_after_widgets_left": "Taskbar - Left of Widgets button"
      - "taskbar_after_widgets_right": "Taskbar - Right of Widgets button"
      - "tray_left": "Tray - Far left"
      - "tray_right": "Tray - Far right"
      - "tray_before_clock": "Tray - Left of Clock"
      - "tray_after_clock": "Tray - Right of Clock"
      - "tray_before_omni_left": "Tray - Left of Network/Volume button"
      - "tray_before_omni_right": "Tray - Right of Network/Volume button"
      - "tray_language_left": "Tray - Left of Language button"
      - "tray_language_right": "Tray - Right of Language button"
      - "tray_icons_left": "Tray - Left of Tray Icons"
      - "tray_icons_right": "Tray - Right of Tray Icons"
      - "tray_hidden_icons_left": "Tray - Left of Hidden icons button"
      - "tray_hidden_icons_right": "Tray - Right of Hidden icons button"
      - "tray_after_showdesktop_left": "Tray - Left of Show Desktop"
      - "tray_after_showdesktop_right": "Tray - Right of Show Desktop"
    - customOffset: "0 0"
      $name: Custom offset (X Y)
      $description: Used with Custom position. Moves the widget horizontally and vertically in pixels after applying the selected anchor.
    - playerWidth: "460 460"
      $name: Media player width (min max)
      $description: The first number is the minimum size, and the second is the maximum. You can also set it to 0, which means no limit.
    - playerHeight: "40 40"
      $name: Media player height (min max)
    - playerMargin: "4 4"
      $name: Media player margin (left right)
      $description: The first number is the distance to the left, and the second is to the right.
    - mirrorLayout: false
      $name: Mirror layout
      $description: Album art, text, and buttons will be displayed on the opposite side
    $name: Media player

  - AlbumArtSetting:
    - showAlbumArt: true
      $name: Show album art
    - albumArtWidth: "36 36"
      $name: Album art width (min max)
    - albumArtHeight: "36 36"
      $name: Album art height (min max)
    - albumArtMargin: "0 0"
      $name: Album art margin (left right)
    $name: Album art

  - TextAreaSetting:
    - showTrackTitle: true
      $name: Show track title
    - showTrackArtist: true
      $name: Show artist name
    - textAreaWidth: "220 250"
      $name: Text area width (min max)
    - textAreaHeight: "0 0"
      $name: Text area height (min max)
    - textAreaMargin: "5 5"
      $name: Text area margin (left right)
    - textSpacing: -1
      $name: Spacing between title and artist
    - enableTitleScrolling: true
      $name: Enable track title scrolling
      $description: When enabled, the track title will scroll horizontally if it overflows the text area.
    - enableArtistScrolling: false
      $name: Enable artist name scrolling
      $description: When enabled, the artist name will scroll horizontally if it overflows the text area.
    - scrollSpeed: 1
      $name: Scroll speed (1-10)
      $description: Controls how fast the text scrolls. 1 = slowest, 10 = fastest.
    - scrollPauseDuration: 1000
      $name: Pause duration (ms)
      $description: How long the title remains still before a marquee pass, or at each edge in Bounce mode.
    - scrollMode: "marquee"
      $name: Scroll mode
      $description: One-way loop and Bounce use the scrolling toggles above. No movement overrides them and trims overflowing text with an ellipsis.
      $options:
      - "marquee": "One-way loop (exit left, enter right)"
      - "ellipsis": "No movement; end with ..."
      - "bounce": "Bounce back and forth"
    - loopGap: 40
      $name: Marquee gap (px)
      $description: Extra blank distance before the text re-enters from the right in One-way loop mode.
    - swapTitleArtist: false
      $name: Swap artist name and track title
    - emptyTitleText: "Untitled"
      $name: Title text when track has no name
      $description: Shown in the title field when a track is playing but has no title. Leave empty to hide the title text in this case.
    - noMediaTitleText: "Not Playing"
      $name: Title text when nothing is playing
      $description: Shown in the title field when there is no media session at all. Leave empty to hide the title text in this case.
    - emptyArtistText: ""
      $name: Artist text when track has no artist
      $description: Shown in the artist field when a track is playing but has no artist specified. Leave empty to hide the artist text in this case.
    - noMediaArtistText: ""
      $name: Artist text when nothing is playing
      $description: Shown in the artist field when there is no media session at all. Leave empty to hide the artist text in this case.
    $name: Text area

  - MediaButtonsSettings:
    - showMediaButtons: true
      $name: Show media buttons
    - mediaButtons: [shuffle, repeat, prev, play, next, volume]
      $name: Media buttons order
      $description: Select which media control buttons to display and their order. Duplicates are ignored.
      $options:
      - none: Nothing
      - prev: Previous Track
      - play: Play/Pause
      - next: Next Track
      - volume: Open media-app volume slider
      - rewind: Rewind 5s
      - forward: Forward 5s
      - shuffle: Toggle Shuffle
      - repeat: Toggle Repeat
    - mediaButtonsMargin: "2 2"
      $name: Media buttons margin (left right)
    - buttonSize: 28
      $name: Button size
    - hideUnsupportedButtons: false
      $name: Hide unsupported buttons
      $description: When enabled, buttons for actions not supported by the current media session are completely hidden instead of shown as disabled.
    $name: Media buttons

  - ProgressBarSettings:
    - showProgressBar: true
      $name: Show playback progress bar
      $description: Shows a thin playback-progress line along the bottom edge of the media widget. Live streams and media without a known duration do not show the bar.
    - enableSeeking: true
      $name: Allow click and drag seeking
      $description: Click or drag the progress bar to seek when the current media application supports playback-position changes.
    - progressBarHeight: 2
      $name: Progress bar height
      $description: Normal visible height in pixels.
    - progressBarHoverHeight: 5
      $name: Progress bar hover height
      $description: Visible height while the pointer is over the progress bar.
    - showTimeTooltip: true
      $name: Show time while hovering
      $description: Shows the time under the pointer while hovering or dragging the progress bar.
    - timeFormat: "elapsed_total"
      $name: Progress time format
      $options:
      - "elapsed_total": "Elapsed / total (1:42 / 3:58)"
      - "elapsed":       "Elapsed only (1:42)"
      - "remaining":     "Remaining (-2:16)"
    $name: Progress bar

  $name: Player

- AppearanceSettings:
  - BackgroundStyleSettings:
    - backgroundType: "acrylic"
      $name: Background type
      $options:
      - "none":           "None (transparent)"
      - "solid":          "Solid color"
      - "gradient":       "Gradient"
      - "acrylic":        "Acrylic"
      - "mica":           "Mica"
      - "mica_alt":       "Mica Alt"
      - "album_art_blur": "Blurred album cover"
    - solidColor: "35 35 35"
      $name: Background color (RGB)
      $description: "Use '-1 -1 -1' for the Windows accent color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
    - solidColor2: "35 35 35"
      $name: Gradient color 1 (RGB)
    - gradientColor2: "128 128 128"
      $name: Gradient color 2 (RGB)
    - solidOpacity: 100
      $name: Solid color opacity (0-100)
    - gradientAngle: 50
      $name: Gradient rotation angle (0-360)
    - gradientBalance: 50
      $name: Gradient color balance (0-100)
    - acrylicTintOpacity: 50
      $name: Acrylic tint opacity (0-100)
    - micaOpacity: 50
      $name: Mica/Mica Alt opacity (0-100)
    - blurOpacity: 65
      $name: Album art blur opacity (0-100)
    - blurRadius: 11
      $name: Album art blur strength (1-50)
    - cornerRadius: "4"
      $name: Media player corner radius
      $description: "Use single value (e.g., '4') for uniform corners, or four space-separated values (e.g., '4 2 4 2') for individual corners."
    - enablePlayerHoverEffect: "auto"
      $name: Player hover effect
      $options:
      - "auto":  "Auto (theme changes automatically)"
      - "black": "Black"
      - "white": "White"
      - "off":   "Disable hover effect"
    - enableMediaButtonsHoverEffect: "auto"
      $name: Media buttons hover effect
      $options:
      - "auto":  "Auto (theme changes automatically)"
      - "black": "Black"
      - "white": "White"
      - "off":   "Disable hover effect"
    $name: Background

  - TaskbarAppearanceSettings:
    - transparentTaskbar: false
      $name: Transparent taskbar
      $description: Makes the Windows taskbar background fully transparent on every monitor. This is independent from the media widget background and is restored when the option or mod is disabled. Leave it off when a dedicated taskbar background or transparency mod is enabled.
    $name: Windows taskbar

  - ProgressBarStyleSettings:
    - progressColorPreset: "cyan"
      $name: Played progress color
      $description: Select a noticeable preset or choose Custom to use the RGB value below. Cyan is the default because it remains distinct from loading, update, and download indicators that commonly use green.
      $options:
      - "cyan":      "Cyan / light blue"
      - "red":       "Red (YouTube style)"
      - "purple":    "Purple"
      - "orange":    "Orange"
      - "accent":    "Windows accent color"
      - "album_art": "Color derived from album art"
      - "custom":    "Custom RGB value below"
    - progressColor: "0 188 255"
      $name: Custom played progress color (RGB)
      $description: "Used only when Played progress color is Custom. Use '-1 -1 -1' for the Windows accent color and '-2 -2 -2' for the album art color. Light and dark colors can be separated with $ like the other color settings."
    - progressOpacity: 100
      $name: Played progress opacity (0-100)
    - trackColor: "0 0 0$255 255 255"
      $name: Remaining track color (RGB)
      $description: Uses translucent black in the light theme and translucent white in the dark theme by default. Supports the same accent, album-art, and custom color values as the played progress color.
    - trackOpacity: 28
      $name: Remaining track opacity (0-100)
    $name: Progress bar

  - MediaButtonsStyleSettings:
    - iconStyle: "fluent_outline"
      $name: Icon style
      $options:
      - "fluent_outline": "Segoe Fluent Icons (Outline)"
      - "fluent_filled":  "Segoe Fluent Icons (Filled)"
      - "mdl2_outline":   "Segoe MDL2 Assets (Outline)"
      - "mdl2_filled":    "Segoe MDL2 Assets (Filled)"
    - buttonSpacing: 0
      $name: Spacing between media buttons
    - buttonIconSize: 12
      $name: Button icon size
    - buttonCornerRadius: "4"
      $name: Media buttons corner radius
      $description: "Use single value (e.g., '4') for uniform corners, or four space-separated values (e.g., '4 2 4 2') for individual corners."
    - buttonColor: "0 0 0$255 255 255"
      $name: Media buttons icons color (RGB)
      $description: "Use '-1 -1 -1' for the Windows accent color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
    - buttonColorOpacity: 100
      $name: Media buttons icons opacity (0-100)
    $name: Media buttons

  - TitleTextStyleSettings:
    - titleColor: "0 0 0$255 255 255"
      $name: Title color (RGB)
      $description: "Use '-1 -1 -1' for the Windows accent color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
    - titleColorOpacity: 100
      $name: Title opacity (0-100)
    - titleFont: segoe_ui_variable
      $name: Title font
      $options:
      - segoe_ui_variable: Segoe UI Variable Display
      - segoe_ui:          Segoe UI
      - aptos:             Aptos
      - calibri:           Calibri
      - cambria:           Cambria
      - candara:           Candara
      - consolas:          Consolas
      - corbel:            Corbel
      - arial:             Arial
      - trebuchet:         Trebuchet MS
      - verdana:           Verdana
      - tahoma:            Tahoma
      - georgia:           Georgia
      - times_new_roman:   Times New Roman
      - custom:            Custom...
    - titleFontSize: 12
      $name: Title font size
    - titleFontFamily: ""
      $name: Title font family (for Custom option)
      $description: >-
        For a list of fonts that are shipped with Windows 11, refer to the
        following page:
        https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
    - titleFontWeight: ""
      $name: Title font weight
      $options:
      - "":          Default
      - Thin:        Thin
      - ExtraLight:  Extra light
      - Light:       Light
      - SemiLight:   Semi light
      - Normal:      Normal
      - Medium:      Medium
      - SemiBold:    Semi bold
      - Bold:        Bold
      - ExtraBold:   Extra bold
      - Black:       Black
      - ExtraBlack:  Extra black
    - titleFontStyle: ""
      $name: Title font style
      $options:
      - "":       Default
      - Normal:   Normal
      - Oblique:  Oblique
      - Italic:   Italic
    - titleCharacterSpacing: 0
      $name: Title character spacing
      $description: Can be a positive or a negative number.
    $name: Title text

  - ArtistTextStyleSettings:
    - artistColor: "0 0 0$255 255 255"
      $name: Artist color (RGB)
      $description: "Use '-1 -1 -1' for the Windows accent color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
    - artistColorOpacity: 80
      $name: Artist opacity (0-100)
    - artistFont: segoe_ui_variable
      $name: Artist font
      $options:
      - segoe_ui_variable: Segoe UI Variable Display
      - segoe_ui:          Segoe UI
      - aptos:             Aptos
      - calibri:           Calibri
      - cambria:           Cambria
      - candara:           Candara
      - consolas:          Consolas
      - corbel:            Corbel
      - arial:             Arial
      - trebuchet:         Trebuchet MS
      - verdana:           Verdana
      - tahoma:            Tahoma
      - georgia:           Georgia
      - times_new_roman:   Times New Roman
      - custom:            Custom...
    - artistFontSize: 11
      $name: Artist font size
    - artistFontFamily: ""
      $name: Artist font family (for Custom option)
      $description: >-
        For a list of fonts that are shipped with Windows 11, refer to the
        following page:
        https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
    - artistFontWeight: ""
      $name: Artist font weight
      $options:
      - "":          Default
      - Thin:        Thin
      - ExtraLight:  Extra light
      - Light:       Light
      - SemiLight:   Semi light
      - Normal:      Normal
      - Medium:      Medium
      - SemiBold:    Semi bold
      - Bold:        Bold
      - ExtraBold:   Extra bold
      - Black:       Black
      - ExtraBlack:  Extra black
    - artistFontStyle: ""
      $name: Artist font style
      $options:
      - "":       Default
      - Normal:   Normal
      - Oblique:  Oblique
      - Italic:   Italic
    - artistCharacterSpacing: 0
      $name: Artist character spacing
      $description: Can be a positive or a negative number.
    $name: Artist text

  - AlbumArtDisplaySettings:
    - albumArtEmptyBehavior: "show"
      $name: Album art behavior when no cover available
      $options:
      - "show":          "Show area"
      - "hide":          "Hide area"
      - "show_icon":     "Show area with icon"
    - emptyIconGlyph: "E189"
      $name: Icon glyph code (hex)
      $description: "Hex code of the glyph to show, e.g. 'E189' for music note. See https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font"
    - emptyIconSize: 16
      $name: Icon size
    - emptyIconFont: "segoe_fluent"
      $name: Icon font style
      $options:
      - "segoe_fluent": "Segoe Fluent Icons"
      - "segoe_mdl2":   "Segoe MDL2 Assets"
    - emptyIconColor: "140 140 140"
      $name: Icon color (RGB)
      $description: "Use '-1 -1 -1' for the system accent color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
    - emptyIconOpacity: 100
      $name: Icon opacity (0-100)
    - albumArtQuality: "medium"
      $name: Album art quality
      $options:
      - "low":    "Low (faster, less memory)"
      - "medium": "Medium (default)"
      - "high":   "High (best quality)"
    - albumArtFitMode: "adaptive"
      $name: Album art aspect-ratio behavior
      $description: Adaptive keeps square covers square and widens the album-art area for wide covers so the full image is visible. Crop always fills the configured album-art box. Fit shows the full image inside the configured box without widening it.
      $options:
      - "adaptive": "Adaptive: widen for wide covers"
      - "crop":     "Crop to configured box"
      - "fit":      "Fit inside configured box"
    - albumArtAdaptiveMaxWidth: 96
      $name: Maximum adaptive album-art width
      $description: Maximum width in pixels used by Adaptive mode. A 16:9 cover at the default 36 px height uses about 64 px.
    - showPauseOverlay: true
      $name: Show pause icon overlay on album art when paused
    - pauseOverlayIconSize: 16
      $name: Pause icon size
    - pauseOverlayOpacity: 60
      $name: Pause overlay background opacity (0-100)
    - albumArtOpacity: 100
      $name: Album art opacity (0-100)
    - albumArtCornerRadius: "4"
      $name: Album art corner radius
      $description: "Use single value (e.g., '4') for uniform corners, or four space-separated values (e.g., '4 2 4 2') for individual corners."
    - showAppIcon: false
      $name: Show media app icon overlay
    - appIconCorner: "bottom_right"
      $name: App icon corner
      $options:
      - "top_left":     "Top left"
      - "top_right":    "Top right"
      - "bottom_left":  "Bottom left"
      - "bottom_right": "Bottom right"
    - appIconSize: 12
      $name: App icon size
    $name: Album art

  $name: Appearance

- BehaviorSettings:
  - MediaSourceSettings:
    - preferredApp: ""
      $name: Preferred media application
      $description: Leave empty to follow the actively playing Windows media session. To pin the widget and Discord presence to one player, select it from the widget's right-click Media source menu, or enter its Windows media-session app ID here.
    - sessionSwitchGraceMs: 1800
      $name: Media-source switching grace (ms)
      $description: Keeps the current player selected through brief stop/pause states during track changes. This also prevents a newly opened browser tab from stealing the widget while the current player is still active.
    - mediaTransitionGraceMs: 2500
      $name: Track transition grace (ms)
      $description: Keeps the previous title and artwork visible while the player publishes the next track's metadata and cover, avoiding brief disappearing or gray-placeholder flashes.
    $name: Media source
  - disableAlbumArtClick: false
    $name: Let clicks pass through album art
    $description: When enabled, the album art itself does not capture mouse clicks.
  - widgetClickAction: "none"
    $name: Widget left-click action
    $description: Controls what happens when the empty player area or text area is left-clicked. Media buttons keep their own actions. The default is Nothing.
    $options:
    - none:              Nothing
    - play_pause:        Play/Pause
    - next_track:        Next track
    - prev_track:        Previous track
    - rewind_5s:         Rewind 5 seconds
    - forward_5s:        Forward 5 seconds
    - toggle_shuffle:    Toggle Shuffle
    - toggle_repeat:     Toggle Repeat
    - open_app:          Open media app
    - open_context_menu: Open context menu
  - ClickActionSettings:
      - - object: player
          $name: Object
          $options:
          - none:       Nothing
          - player:     Player area
          - album_art:  Album art area
        - click: right_click
          $name: Click type
          $options:
          - none:                Nothing
          - left_click:          Left click
          - left_double_click:   Left double click
          - right_click:         Right click
          - middle_click:        Middle click
        - action: open_context_menu
          $name: Action
          $options:
          - none:            Nothing
          - play_pause:      Play/Pause
          - next_track:      Next track
          - prev_track:      Previous track
          - rewind_5s:       Rewind 5s
          - forward_5s:      Forward 5s
          - toggle_shuffle:  Toggle Shuffle
          - toggle_repeat:   Toggle Repeat
          - open_app:        Open media app
          - open_context_menu: Open context menu
      - - object: album_art
        - click: left_double_click
        - action: open_app
    $name: Additional click actions
  - MouseWheelActionSettings:
      - - object: player
          $name: Object
          $options:
          - none:       Nothing
          - player:     Player area
          - album_art:  Album art area
        - click: mouse_wheel
          $name: Mouse type
          $options:
          - none:             Nothing
          - mouse_wheel:      Mouse wheel
          - mouse_wheel_up:   Mouse wheel up
          - mouse_wheel_down: Mouse wheel down
        - action: app_sound
          $name: Action
          $options:
          - none:                      Nothing
          - "switch_tracks":           "Switch tracks"
          - "switch_tracks_inverted":  "Switch tracks (inverted)"
          - "app_sound":               "Change app sound volume"
      - - object: album_art
        - click: mouse_wheel
        - action: none
    $name: Mouse-wheel actions
  - hideWhenNoMedia: true
    $name: Hide when no media is playing
  - hideFullscreen: true
    $name: Hide when a fullscreen app is running
  - idleHideSeconds: 0
    $name: Idle auto-hide timeout (seconds, 0 = disabled)
  - showFullTitleOnHover: true
    $name: Show full track title on hover (tooltip)
  $name: Behavior

- DiscordPresenceSettings:
  - enabled: false
    $name: Publish Discord Rich Presence
    $description: Disabled by default. When enabled, sends the selected track, artist, playback state, and progress to the running Discord desktop client, which broadcasts the activity on your Discord profile to people allowed by your Discord privacy settings. The included default Application ID belongs to Taskbar Media Presence and is owned by MrBoxik; replace it below to attribute activity to your own Discord application.
  - applicationId: "1528896038163710112"
    $name: Discord Application ID
    $description: The default ID belongs to the Taskbar Media Presence Discord application owned by MrBoxik. Activity published with it is attributed to Taskbar Media Presence. You can replace it with another numeric Application ID; this value is public, not a secret.
  - showPaused: true
    $name: Show paused songs
  - activityName: "{title}"
    $name: Song detail template
    $description: Main presence line using {title}, {artist}, {album}, or {app}. Discord controls the Listening to application name from the Developer Portal and doesn't let a local activity replace it.
  - largeImageKey: "taskbar_media_presence"
    $name: Rich Presence image asset key
    $description: Asset key uploaded under your Discord application's Rich Presence assets. Upload assets/taskbar_media_presence.png with this key.
  $name: Discord presence

- DebugSettings:
  - ignoredProcesses: ""
    $name: Ignore media from processes (separate with ; )
  - enableTreeDump: false
    $name: Log taskbar XAML tree
    $description: Debug option. Writes the discovered taskbar XAML tree to the Windhawk log during injection.
  - showDebugBorders: false
    $name: Show layout debug borders
    $description: Debug option. Draws borders around the player layout regions.
  - showLayoutAnchors: false
    $name: Show positioning anchors
    $description: Debug option. Displays the taskbar elements used as positioning anchors.
  $name: Advanced
*/
// ==/WindhawkModSettings==

/*
MIT License

Copyright (c) 2026 Salyts
Copyright (c) 2026 MrBoxik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <robuffer.h>
#include <shcore.h>
#include <windows.h>
#include <appmodel.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wincodec.h>
#include <propsys.h>
#include <dwmapi.h>
#include <windhawk_utils.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include <propkey.h>

#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <mutex>
#include <unordered_map>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <limits>
#include <cwctype>
#include <cstdio>
#include <cstring>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Storage::Streams;

struct ModSettings {
    std::wstring taskbarMode         = L"selected";
    int          taskbarNumber        = 1;
    std::wstring positionPreset       = L"apps";
    std::wstring position             = L"taskbar_after_apps";
    int          customOffsetX        = 0;
    int          customOffsetY        = 0;
    std::wstring albumArtLeftClick    = L"none";
    std::wstring albumArtRightClick   = L"none";
    std::wstring albumArtMiddleClick  = L"none";
    std::wstring albumArtLeftDoubleClick  = L"none";
    std::wstring albumArtWheelAction  = L"none";
    std::wstring albumArtWheelUpAction = L"none";
    std::wstring albumArtWheelDownAction = L"none";
    std::wstring widgetClickAction    = L"none";
    int          sessionSwitchGraceMs = 1800;
    int          mediaTransitionGraceMs = 2500;
    std::wstring playerLeftClick      = L"none";
    std::wstring playerRightClick     = L"none";
    std::wstring playerMiddleClick    = L"none";
    std::wstring playerLeftDoubleClick  = L"none";
    std::wstring playerWheelAction    = L"none";
    std::wstring playerWheelUpAction  = L"none";
    std::wstring playerWheelDownAction = L"none";
    bool         mirrorLayout         = false;
    bool         showMediaButtons     = true;
    bool         showProgressBar      = true;
    bool         progressBarSeekEnabled = true;
    int          progressBarHeight    = 2;
    int          progressBarHoverHeight = 5;
    bool         progressShowTimeTooltip = true;
    std::wstring progressTimeFormat   = L"elapsed_total";
    std::wstring progressBarColorPreset = L"cyan";
    std::wstring progressBarColor     = L"0 188 255";
    int          progressBarOpacity   = 100;
    std::wstring progressTrackColor   = L"0 0 0$255 255 255";
    int          progressTrackOpacity = 28;
    int          playerMinWidth       = 460;
    int          playerMaxWidth       = 460;
    int          playerMinHeight      = 40;
    int          playerMaxHeight      = 40;
    bool         showAlbumArt         = true;
    std::wstring albumArtEmptyBehavior = L"show";
    std::wstring emptyIconGlyph       = L"E189";
    int          emptyIconSize        = 16;
    std::wstring emptyIconFont        = L"segoe_fluent";
    std::wstring emptyIconColor       = L"140 140 140";
    int          emptyIconOpacity     = 100;
    std::wstring albumArtQuality      = L"medium";
    std::wstring albumArtFitMode      = L"adaptive";
    int          albumArtAdaptiveMaxWidth = 96;
    bool         showPauseOverlay     = true;
    int          pauseOverlayIconSize = 16;
    int          pauseOverlayOpacity  = 60;
    int          albumArtMinWidth     = 36;
    int          albumArtMaxWidth     = 36;
    int          albumArtMinHeight    = 36;
    int          albumArtMaxHeight    = 36;
    int          albumArtOpacity      = 100;
    int          albumArtLeftMargin   = 0;
    int          albumArtRightMargin  = 0;
    bool         showTrackTitle       = true;
    bool         showFullTitleOnHover = true;
    bool         showTrackArtist      = true;
    bool         swapTitleArtist      = false;
    std::wstring emptyTitleText       = L"Untitled";
    std::wstring noMediaTitleText     = L"Not Playing";
    std::wstring emptyArtistText      = L"";
    std::wstring noMediaArtistText    = L"";
    std::wstring iconStyle            = L"fluent_outline";
    bool         showAppIcon          = false;
    std::wstring appIconCorner        = L"bottom_right";
    int          appIconSize          = 12;
    bool         hideWhenNoMedia      = true;
    std::wstring playerHoverEffectMode = L"auto";
    std::wstring mediaButtonsHoverEffectMode = L"auto";
    int          playerMarginLeft     = 4;
    int          playerMarginRight    = 4;
    int          mediaButtonsLeftMargin  = 2;
    int          mediaButtonsRightMargin = 2;
    int          textAreaMinWidth     = 220;
    int          textAreaMaxWidth     = 250;
    int          textAreaMinHeight    = 0;
    int          textAreaMaxHeight    = 0;
    int          textAreaLeftMargin   = 5;
    int          textAreaRightMargin  = 5;
    bool         hideFullscreen       = true;
    int          idleHideSeconds      = 0;
    std::wstring backgroundType       = L"acrylic";
    int          blurOpacity          = 65;
    int          blurRadius           = 11;
    double       cornerRadiusTL       = 4;
    double       cornerRadiusTR       = 4;
    double       cornerRadiusBR       = 4;
    double       cornerRadiusBL       = 4;
    double       albumArtCornerRadiusTL = 4;
    double       albumArtCornerRadiusTR = 4;
    double       albumArtCornerRadiusBR = 4;
    double       albumArtCornerRadiusBL = 4;
    int          buttonSpacing        = 0;
    int          buttonSize           = 28;
    int          buttonIconSize       = 12;
    double       buttonCornerRadiusTL = 4;
    double       buttonCornerRadiusTR = 4;
    double       buttonCornerRadiusBR = 4;
    double       buttonCornerRadiusBL = 4;
    int          titleFontSize        = 12;
    int          artistFontSize       = 11;
    std::wstring titleFont            = L"segoe_ui_variable";
    std::wstring artistFont           = L"segoe_ui_variable";
    std::wstring titleFontFamily      = L"";
    std::wstring artistFontFamily     = L"";
    std::wstring titleFontWeight      = L"";
    std::wstring artistFontWeight     = L"";
    std::wstring titleFontStyle       = L"";
    std::wstring artistFontStyle      = L"";
    int          titleCharacterSpacing  = 0;
    int          artistCharacterSpacing = 0;
    int          textSpacing          = -1;
    bool         enableArtistScrolling = false;
    bool         enableTitleScrolling = true;
    int          scrollSpeed          = 1;
    int          scrollPauseDuration  = 1000;
    std::wstring scrollMode           = L"marquee";
    int          loopGap              = 40;
    std::wstring solidColor           = L"35 35 35";
    std::wstring solidColor2          = L"35 35 35";
    std::wstring gradientColor2       = L"128 128 128";
    int          solidOpacity         = 100;
    int          gradientAngle        = 50;
    int          gradientBalance      = 50;
    int          acrylicTintOpacity   = 50;
    int          micaOpacity          = 50;
    bool         transparentTaskbar   = false;
    std::wstring buttonColor          = L"0 0 0$255 255 255";
    int          buttonColorOpacity   = 100;
    std::wstring titleColor           = L"0 0 0$255 255 255";
    int          titleColorOpacity    = 100;
    std::wstring artistColor          = L"0 0 0$255 255 255";
    int          artistColorOpacity   = 80;
    std::wstring ignoredProcesses     = L"";
    bool         enableTreeDump       = false;
    bool         showDebugBorders     = false;
    bool         showLayoutAnchors    = false;
    bool         hideUnsupportedButtons  = false;
    bool         disableAlbumArtClick    = false;
    bool         discordPresenceEnabled   = false;
    std::wstring discordApplicationId     = L"1528896038163710112";
    bool         discordShowPaused        = true;
    std::wstring discordActivityName      = L"{title}";
    std::wstring discordLargeImageKey = L"taskbar_media_presence";
};
// Settings are published as immutable shared snapshots. Readers copy the
// current shared_ptr under a short mutex, keeping their snapshot alive without
// retaining every historical settings version for the lifetime of Explorer.
static std::mutex g_settingsSnapshotMtx;
static std::shared_ptr<const ModSettings>
    g_settingsSnapshot = std::make_shared<const ModSettings>();
static std::mutex g_mediaSourceMtx;
static std::wstring g_preferredMediaApp;

static std::wstring GetPreferredMediaApp() {
    std::lock_guard<std::mutex> lock(g_mediaSourceMtx);
    return g_preferredMediaApp;
}

static void StorePreferredMediaApp(const std::wstring& appId) {
    std::lock_guard<std::mutex> lock(g_mediaSourceMtx);
    g_preferredMediaApp = appId;
}

static void ParseTwoInts(const std::wstring& s, int& a, int& b) {
    size_t sp = s.find(L' ');
    if (sp == std::wstring::npos) return;
    try {
        a = std::stoi(s.substr(0, sp));
        b = std::stoi(s.substr(sp + 1));
    } catch (...) {}
}

enum class MediaButtonType {
    Previous = 1,
    PlayPause = 2,
    Next = 3,
    Rewind5s = 5,
    Forward5s = 6,
    Shuffle = 7,
    Repeat = 8,
    Volume = 10,
};

struct MediaButtonDefinition {
    std::wstring keyword;
    MediaButtonType type;
    int cmd;
};

static const std::vector<MediaButtonDefinition> g_mediaButtonDefinitions = {
    {L"prev", MediaButtonType::Previous, 1},
    {L"play", MediaButtonType::PlayPause, 2},
    {L"next", MediaButtonType::Next, 3},
    {L"rewind", MediaButtonType::Rewind5s, 5},
    {L"forward", MediaButtonType::Forward5s, 6},
    {L"shuffle", MediaButtonType::Shuffle, 7},
    {L"repeat", MediaButtonType::Repeat, 8},
    {L"volume", MediaButtonType::Volume, 13},
};

struct MediaButtonConfig {
    MediaButtonType type;
    int cmd;
};

static std::vector<MediaButtonConfig> g_mediaButtons;
static std::mutex g_mediaButtonsMutex;

static bool HasConfiguredMediaButtons() {
    std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
    return !g_mediaButtons.empty();
}

static bool HasConfiguredMediaButton(MediaButtonType type) {
    std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
    return std::any_of(
        g_mediaButtons.begin(), g_mediaButtons.end(),
        [type](const MediaButtonConfig& button) {
            return button.type == type;
        });
}

static std::wstring MapFontName(const std::wstring& key) {
    if (key == L"custom") return L"";
    if (key == L"segoe_ui_variable") return L"Segoe UI Variable Display";
    if (key == L"segoe_ui") return L"Segoe UI";
    if (key == L"segoe_ui_semibold") return L"Segoe UI Semibold";
    if (key == L"segoe_ui_bold") return L"Segoe UI Bold";
    if (key == L"segoe_ui_light") return L"Segoe UI Light";
    if (key == L"segoe_ui_semilight") return L"Segoe UI Semilight";
    if (key == L"aptos") return L"Aptos";
    if (key == L"calibri") return L"Calibri";
    if (key == L"cambria") return L"Cambria";
    if (key == L"candara") return L"Candara";
    if (key == L"consolas") return L"Consolas";
    if (key == L"corbel") return L"Corbel";
    if (key == L"arial") return L"Arial";
    if (key == L"trebuchet") return L"Trebuchet MS";
    if (key == L"verdana") return L"Verdana";
    if (key == L"tahoma") return L"Tahoma";
    if (key == L"georgia") return L"Georgia";
    if (key == L"times_new_roman") return L"Times New Roman";
    return L"Segoe UI Variable Display";
}

static std::atomic<int> g_taskbarCenteredState{-1};

static bool IsWindowsTaskbarCentered() {
    int cached = g_taskbarCenteredState.load();
    if (cached >= 0) return cached != 0;
    DWORD value = 1;
    DWORD size = sizeof(value);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &value, &size);
    // Centered is the Windows 11 default when TaskbarAl isn't present.
    bool centered = status != ERROR_SUCCESS || value != 0;
    g_taskbarCenteredState = centered ? 1 : 0;
    return centered;
}

static void LoadSettings() {
    ModSettings g_settings;
    g_taskbarCenteredState = -1;
    auto Str = [](const wchar_t* key, const wchar_t* def) -> std::wstring {
        auto setting = WindhawkUtils::StringSetting::make(key);
        PCWSTR value = setting.get();
        return (*value != L'\0') ? value : def;
    };
    auto StrAllowEmpty =
        [](const wchar_t* key, const wchar_t* /*def*/) -> std::wstring {
        auto setting = WindhawkUtils::StringSetting::make(key);
        return setting.get();
    };
    auto Int = [](const wchar_t* key, int lo, int hi, int /*def*/) -> int {
        return std::clamp(Wh_GetIntSetting(key), lo, hi);
    };
    auto ParseMargin = [&Str](const wchar_t* key, const wchar_t* def, int& left, int& right) {
        std::wstring val = Str(key, def);
        try {
            size_t space = val.find(L' ');
            if (space != std::wstring::npos) {
                left  = std::stoi(val.substr(0, space));
                right = std::stoi(val.substr(space + 1));
            } else if (!val.empty()) {
                left = right = std::stoi(val);
            }
        } catch (...) {
            std::wstring d(def);
            size_t space = d.find(L' ');
            try {
                if (space != std::wstring::npos) {
                    left  = std::stoi(d.substr(0, space));
                    right = std::stoi(d.substr(space + 1));
                } else {
                    left = right = std::stoi(d);
                }
            } catch (...) { left = right = 0; }
        }
    };
    auto ParseCornerRadius = [&Str](const wchar_t* key, const wchar_t* def, double& tl, double& tr, double& br, double& bl) {
        std::wstring val = Str(key, def);
        std::vector<double> values;
        try {
            size_t pos = 0;
            while (pos < val.length()) {
                size_t space = val.find(L' ', pos);
                if (space == std::wstring::npos) space = val.length();
                std::wstring part = val.substr(pos, space - pos);
                if (!part.empty()) {
                    double v = std::stod(part);
                    values.push_back(v < 0.0 ? 0.0 : v);
                }
                pos = space + 1;
            }
        } catch (...) {}

        if (values.empty()) {
            try {
                std::wstring d(def);
                double v = std::stod(d);
                values.push_back(v < 0.0 ? 0.0 : v);
            } catch (...) {
                values.push_back(4.0);
            }
        }

        if (values.size() == 1) {
            tl = tr = br = bl = values[0];
        } else if (values.size() == 4) {
            tl = values[0];
            tr = values[1];
            br = values[2];
            bl = values[3];
        } else {
            tl = tr = br = bl = values[0];
        }
    };
    auto HoverMode = [&Str](const wchar_t* key) -> std::wstring {
        std::wstring mode = Str(key, L"auto");
        if (mode == L"black") return L"black";
        if (mode == L"white") return L"white";
        if (mode == L"off")   return L"off";
        return L"auto";
    };

    g_settings.taskbarMode          = Str(L"MainSettings.PlayerSetting.taskbarMode", L"selected");
    g_settings.taskbarNumber        = Int(L"MainSettings.PlayerSetting.taskbarNumber", 1, 16, 1);
    g_settings.positionPreset       = Str(L"MainSettings.PlayerSetting.positionPreset", L"apps");
    g_settings.position             = Str(L"MainSettings.PlayerSetting.position", L"taskbar_after_apps");
    ParseTwoInts(Str(L"MainSettings.PlayerSetting.customOffset", L"0 0"),
                 g_settings.customOffsetX, g_settings.customOffsetY);
    g_settings.customOffsetX = std::clamp(g_settings.customOffsetX, -4000, 4000);
    g_settings.customOffsetY = std::clamp(g_settings.customOffsetY, -4000, 4000);

    ParseMargin(L"MainSettings.PlayerSetting.playerMargin", L"4 4", g_settings.playerMarginLeft, g_settings.playerMarginRight);
    ParseMargin(L"MainSettings.PlayerSetting.playerWidth", L"460 460", g_settings.playerMinWidth, g_settings.playerMaxWidth);
    ParseMargin(L"MainSettings.PlayerSetting.playerHeight", L"40 40", g_settings.playerMinHeight, g_settings.playerMaxHeight);

    ParseMargin(L"MainSettings.AlbumArtSetting.albumArtWidth", L"36 36", g_settings.albumArtMinWidth, g_settings.albumArtMaxWidth);
    ParseMargin(L"MainSettings.AlbumArtSetting.albumArtHeight", L"36 36", g_settings.albumArtMinHeight, g_settings.albumArtMaxHeight);
    ParseMargin(L"MainSettings.AlbumArtSetting.albumArtMargin", L"0 0", g_settings.albumArtLeftMargin, g_settings.albumArtRightMargin);

    ParseMargin(L"MainSettings.TextAreaSetting.textAreaWidth", L"220 250", g_settings.textAreaMinWidth, g_settings.textAreaMaxWidth);
    if (g_settings.textAreaMinWidth == 150 && g_settings.textAreaMaxWidth == 170) {
        // Migrate the previous compact default while preserving custom widths.
        g_settings.textAreaMinWidth = 220;
        g_settings.textAreaMaxWidth = 250;
    }
    ParseMargin(L"MainSettings.TextAreaSetting.textAreaHeight", L"0 0", g_settings.textAreaMinHeight, g_settings.textAreaMaxHeight);
    ParseMargin(L"MainSettings.TextAreaSetting.textAreaMargin", L"5 5", g_settings.textAreaLeftMargin, g_settings.textAreaRightMargin);

    g_settings.mirrorLayout         = Wh_GetIntSetting(L"MainSettings.PlayerSetting.mirrorLayout") != 0;
    g_settings.showMediaButtons     = Wh_GetIntSetting(L"MainSettings.MediaButtonsSettings.showMediaButtons") != 0;
    ParseMargin(L"MainSettings.MediaButtonsSettings.mediaButtonsMargin", L"2 2", g_settings.mediaButtonsLeftMargin, g_settings.mediaButtonsRightMargin);
    g_settings.showProgressBar =
        Wh_GetIntSetting(L"MainSettings.ProgressBarSettings.showProgressBar") != 0;
    g_settings.progressBarSeekEnabled =
        Wh_GetIntSetting(L"MainSettings.ProgressBarSettings.enableSeeking") != 0;
    g_settings.progressBarHeight = Int(
        L"MainSettings.ProgressBarSettings.progressBarHeight", 1, 12, 2);
    g_settings.progressBarHoverHeight = Int(
        L"MainSettings.ProgressBarSettings.progressBarHoverHeight", 1, 20, 5);
    g_settings.progressBarHoverHeight = std::max(
        g_settings.progressBarHeight,
        g_settings.progressBarHoverHeight);
    g_settings.progressShowTimeTooltip =
        Wh_GetIntSetting(L"MainSettings.ProgressBarSettings.showTimeTooltip") != 0;
    g_settings.progressTimeFormat = Str(
        L"MainSettings.ProgressBarSettings.timeFormat", L"elapsed_total");
    if (g_settings.progressTimeFormat != L"elapsed_total" &&
        g_settings.progressTimeFormat != L"elapsed" &&
        g_settings.progressTimeFormat != L"remaining") {
        g_settings.progressTimeFormat = L"elapsed_total";
    }
    g_settings.sessionSwitchGraceMs = Int(
        L"BehaviorSettings.MediaSourceSettings.sessionSwitchGraceMs",
        0, 10000, 1800);
    g_settings.mediaTransitionGraceMs = Int(
        L"BehaviorSettings.MediaSourceSettings.mediaTransitionGraceMs",
        0, 10000, 2500);
    g_settings.showTrackTitle       = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.showTrackTitle")    != 0;
    g_settings.showFullTitleOnHover = Wh_GetIntSetting(L"BehaviorSettings.showFullTitleOnHover") != 0;
    g_settings.showTrackArtist      = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.showTrackArtist")   != 0;
    g_settings.swapTitleArtist      = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.swapTitleArtist")   != 0;
    g_settings.emptyTitleText       = StrAllowEmpty(L"MainSettings.TextAreaSetting.emptyTitleText",    L"Untitled");
    g_settings.noMediaTitleText     = StrAllowEmpty(L"MainSettings.TextAreaSetting.noMediaTitleText",  L"Not Playing");
    g_settings.emptyArtistText      = StrAllowEmpty(L"MainSettings.TextAreaSetting.emptyArtistText",   L"");
    g_settings.noMediaArtistText    = StrAllowEmpty(L"MainSettings.TextAreaSetting.noMediaArtistText", L"");
    g_settings.showAlbumArt         = Wh_GetIntSetting(L"MainSettings.AlbumArtSetting.showAlbumArt")      != 0;
    g_settings.albumArtEmptyBehavior = Str(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtEmptyBehavior", L"show");
    g_settings.emptyIconGlyph       = Str(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconGlyph",       L"E189");
    g_settings.emptyIconSize        = Int(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconSize",          1, 256, 16);
    g_settings.emptyIconFont        = Str(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconFont",        L"segoe_fluent");
    g_settings.emptyIconColor       = Str(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconColor",       L"140 140 140");
    g_settings.emptyIconOpacity     = Int(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconOpacity",       0, 100, 100);
    g_settings.albumArtQuality      = Str(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtQuality", L"medium");
    g_settings.albumArtFitMode      = Str(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtFitMode", L"adaptive");
    if (g_settings.albumArtFitMode != L"adaptive" &&
        g_settings.albumArtFitMode != L"crop" &&
        g_settings.albumArtFitMode != L"fit") {
        g_settings.albumArtFitMode = L"adaptive";
    }
    g_settings.albumArtAdaptiveMaxWidth =
        Int(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtAdaptiveMaxWidth",
            16, 512, 96);
    g_settings.showPauseOverlay     = Wh_GetIntSetting(L"AppearanceSettings.AlbumArtDisplaySettings.showPauseOverlay")  != 0;
    g_settings.pauseOverlayIconSize = Int(L"AppearanceSettings.AlbumArtDisplaySettings.pauseOverlayIconSize",     1, 256, 16);
    g_settings.pauseOverlayOpacity  = Int(L"AppearanceSettings.AlbumArtDisplaySettings.pauseOverlayOpacity",     0, 100,  60);
    g_settings.iconStyle            = Str(L"AppearanceSettings.MediaButtonsStyleSettings.iconStyle", L"fluent_outline");
    g_settings.showAppIcon          = Wh_GetIntSetting(L"AppearanceSettings.AlbumArtDisplaySettings.showAppIcon")       != 0;
    g_settings.appIconCorner        = Str(L"AppearanceSettings.AlbumArtDisplaySettings.appIconCorner",  L"bottom_right");
    g_settings.appIconSize          = Int(L"AppearanceSettings.AlbumArtDisplaySettings.appIconSize",         8,  32,  12);

    g_settings.backgroundType       = Str(L"AppearanceSettings.BackgroundStyleSettings.backgroundType", L"acrylic");
    g_settings.blurOpacity          = Int(L"AppearanceSettings.BackgroundStyleSettings.blurOpacity",           0, 100, 65);
    g_settings.blurRadius           = Int(L"AppearanceSettings.BackgroundStyleSettings.blurRadius",            1,  50,  11);
    ParseCornerRadius(L"AppearanceSettings.BackgroundStyleSettings.cornerRadius", L"4",
                      g_settings.cornerRadiusTL, g_settings.cornerRadiusTR,
                      g_settings.cornerRadiusBR, g_settings.cornerRadiusBL);
    g_settings.albumArtOpacity      = Int(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtOpacity",       0, 100, 100);
    ParseCornerRadius(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtCornerRadius", L"4",
                      g_settings.albumArtCornerRadiusTL, g_settings.albumArtCornerRadiusTR,
                      g_settings.albumArtCornerRadiusBR, g_settings.albumArtCornerRadiusBL);
    g_settings.buttonSpacing        = Wh_GetIntSetting(L"AppearanceSettings.MediaButtonsStyleSettings.buttonSpacing");
    g_settings.buttonSize           = Int(L"MainSettings.MediaButtonsSettings.buttonSize",          16,  48,  28);
    g_settings.buttonIconSize       = Int(L"AppearanceSettings.MediaButtonsStyleSettings.buttonIconSize",       8,  32,  12);
    ParseCornerRadius(L"AppearanceSettings.MediaButtonsStyleSettings.buttonCornerRadius", L"4",
                      g_settings.buttonCornerRadiusTL, g_settings.buttonCornerRadiusTR,
                      g_settings.buttonCornerRadiusBR, g_settings.buttonCornerRadiusBL);
    g_settings.titleFontSize        = Int(L"AppearanceSettings.TitleTextStyleSettings.titleFontSize",         7,  24,  12);
    g_settings.titleFont            = MapFontName(Str(L"AppearanceSettings.TitleTextStyleSettings.titleFont", L"segoe_ui_variable"));
    g_settings.titleFontFamily      = Str(L"AppearanceSettings.TitleTextStyleSettings.titleFontFamily", L"");
    g_settings.titleFontWeight      = Str(L"AppearanceSettings.TitleTextStyleSettings.titleFontWeight", L"");
    g_settings.titleFontStyle       = Str(L"AppearanceSettings.TitleTextStyleSettings.titleFontStyle", L"");
    g_settings.titleCharacterSpacing = Wh_GetIntSetting(L"AppearanceSettings.TitleTextStyleSettings.titleCharacterSpacing");
    g_settings.artistFontSize       = Int(L"AppearanceSettings.ArtistTextStyleSettings.artistFontSize",        7,  24,  11);
    g_settings.artistFont           = MapFontName(Str(L"AppearanceSettings.ArtistTextStyleSettings.artistFont", L"segoe_ui_variable"));
    g_settings.artistFontFamily     = Str(L"AppearanceSettings.ArtistTextStyleSettings.artistFontFamily", L"");
    g_settings.artistFontWeight     = Str(L"AppearanceSettings.ArtistTextStyleSettings.artistFontWeight", L"");
    g_settings.artistFontStyle      = Str(L"AppearanceSettings.ArtistTextStyleSettings.artistFontStyle", L"");
    g_settings.artistCharacterSpacing = Wh_GetIntSetting(L"AppearanceSettings.ArtistTextStyleSettings.artistCharacterSpacing");
    g_settings.textSpacing          = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.textSpacing");
    g_settings.enableArtistScrolling = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.enableArtistScrolling") != 0;
    g_settings.enableTitleScrolling = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.enableTitleScrolling") != 0;
    g_settings.scrollSpeed          = Int(L"MainSettings.TextAreaSetting.scrollSpeed", 1, 10, 1);
    g_settings.scrollPauseDuration  = Int(L"MainSettings.TextAreaSetting.scrollPauseDuration", 0, 10000, 1000);
    g_settings.scrollMode           = Str(L"MainSettings.TextAreaSetting.scrollMode", L"marquee");
    // "loop" was the name used by earlier builds for the single-copy marquee.
    // Keep existing installations compatible while exposing the clearer name.
    if (g_settings.scrollMode == L"loop") {
        g_settings.scrollMode = L"marquee";
    } else if (g_settings.scrollMode != L"marquee" &&
               g_settings.scrollMode != L"bounce" &&
               g_settings.scrollMode != L"ellipsis") {
        g_settings.scrollMode = L"marquee";
    }
    // Ellipsis mode deliberately bypasses the Canvas-based scrolling views so
    // XAML can trim each line with CharacterEllipsis.
    if (g_settings.scrollMode == L"ellipsis") {
        g_settings.enableTitleScrolling = false;
        g_settings.enableArtistScrolling = false;
    }
    g_settings.loopGap              = Int(L"MainSettings.TextAreaSetting.loopGap", 0, 500, 40);
    g_settings.solidColor           = Str(L"AppearanceSettings.BackgroundStyleSettings.solidColor", L"35 35 35");
    g_settings.solidColor2          = Str(L"AppearanceSettings.BackgroundStyleSettings.solidColor2", L"35 35 35");
    g_settings.gradientColor2       = Str(L"AppearanceSettings.BackgroundStyleSettings.gradientColor2", L"128 128 128");
    g_settings.solidOpacity         = Int(L"AppearanceSettings.BackgroundStyleSettings.solidOpacity", 0, 100, 100);
    g_settings.gradientAngle        = Int(L"AppearanceSettings.BackgroundStyleSettings.gradientAngle", 0, 360, 50);
    g_settings.gradientBalance      = Int(L"AppearanceSettings.BackgroundStyleSettings.gradientBalance", 0, 100, 50);
    g_settings.acrylicTintOpacity   = Int(L"AppearanceSettings.BackgroundStyleSettings.acrylicTintOpacity", 0, 100, 50);
    g_settings.micaOpacity          = Int(L"AppearanceSettings.BackgroundStyleSettings.micaOpacity", 0, 100, 50);
    g_settings.transparentTaskbar =
        Wh_GetIntSetting(L"AppearanceSettings.TaskbarAppearanceSettings.transparentTaskbar") != 0;
    g_settings.progressBarColorPreset = Str(
        L"AppearanceSettings.ProgressBarStyleSettings.progressColorPreset",
        L"cyan");
    if (g_settings.progressBarColorPreset != L"cyan" &&
        g_settings.progressBarColorPreset != L"red" &&
        g_settings.progressBarColorPreset != L"purple" &&
        g_settings.progressBarColorPreset != L"orange" &&
        g_settings.progressBarColorPreset != L"accent" &&
        g_settings.progressBarColorPreset != L"album_art" &&
        g_settings.progressBarColorPreset != L"custom") {
        g_settings.progressBarColorPreset = L"cyan";
    }
    g_settings.progressBarColor = Str(
        L"AppearanceSettings.ProgressBarStyleSettings.progressColor",
        L"0 188 255");
    g_settings.progressBarOpacity = Int(
        L"AppearanceSettings.ProgressBarStyleSettings.progressOpacity",
        0, 100, 100);
    g_settings.progressTrackColor = Str(
        L"AppearanceSettings.ProgressBarStyleSettings.trackColor",
        L"0 0 0$255 255 255");
    g_settings.progressTrackOpacity = Int(
        L"AppearanceSettings.ProgressBarStyleSettings.trackOpacity",
        0, 100, 28);
    g_settings.buttonColor          = Str(L"AppearanceSettings.MediaButtonsStyleSettings.buttonColor", L"0 0 0$255 255 255");
    g_settings.buttonColorOpacity   = Int(L"AppearanceSettings.MediaButtonsStyleSettings.buttonColorOpacity", 0, 100, 100);
    g_settings.titleColor           = Str(L"AppearanceSettings.TitleTextStyleSettings.titleColor", L"0 0 0$255 255 255");
    g_settings.titleColorOpacity    = Int(L"AppearanceSettings.TitleTextStyleSettings.titleColorOpacity", 0, 100, 100);
    g_settings.artistColor          = Str(L"AppearanceSettings.ArtistTextStyleSettings.artistColor", L"0 0 0$255 255 255");
    g_settings.artistColorOpacity   = Int(L"AppearanceSettings.ArtistTextStyleSettings.artistColorOpacity", 0, 100, 80);
    g_settings.widgetClickAction = Str(
        L"BehaviorSettings.widgetClickAction", L"none");
    if (g_settings.widgetClickAction != L"none" &&
        g_settings.widgetClickAction != L"play_pause" &&
        g_settings.widgetClickAction != L"next_track" &&
        g_settings.widgetClickAction != L"prev_track" &&
        g_settings.widgetClickAction != L"rewind_5s" &&
        g_settings.widgetClickAction != L"forward_5s" &&
        g_settings.widgetClickAction != L"toggle_shuffle" &&
        g_settings.widgetClickAction != L"toggle_repeat" &&
        g_settings.widgetClickAction != L"open_app" &&
        g_settings.widgetClickAction != L"open_context_menu") {
        g_settings.widgetClickAction = L"none";
    }
    g_settings.albumArtLeftClick        = L"none";
    g_settings.albumArtRightClick       = L"none";
    g_settings.albumArtMiddleClick      = L"none";
    g_settings.albumArtLeftDoubleClick  = L"none";
    g_settings.playerLeftClick          = L"none";
    g_settings.playerRightClick         = L"none";
    g_settings.playerMiddleClick        = L"none";
    g_settings.playerLeftDoubleClick    = L"none";

    for (int i = 0; i < 20; i++) {
        auto objectSetting = WindhawkUtils::StringSetting::make(
            L"BehaviorSettings.ClickActionSettings[%d].object", i);
        auto clickSetting = WindhawkUtils::StringSetting::make(
            L"BehaviorSettings.ClickActionSettings[%d].click", i);
        auto actionSetting = WindhawkUtils::StringSetting::make(
            L"BehaviorSettings.ClickActionSettings[%d].action", i);
        PCWSTR objectStr = objectSetting.get();
        PCWSTR clickStr = clickSetting.get();
        PCWSTR actionStr = actionSetting.get();

        if (*objectStr == L'\0' || *clickStr == L'\0' ||
            *actionStr == L'\0') {
            break;
        }

        std::wstring object(objectStr);
        std::wstring click(clickStr);
        std::wstring action(actionStr);

        if (object.empty()) object = L"none";
        if (click.empty()) click = L"none";
        if (action.empty()) action = L"none";

        if (object == L"none" || click == L"none") {
            continue;
        }

        if (object == L"album_art") {
            if (click == L"left_click") g_settings.albumArtLeftClick = action;
            else if (click == L"right_click") g_settings.albumArtRightClick = action;
            else if (click == L"middle_click") g_settings.albumArtMiddleClick = action;
            else if (click == L"left_double_click") g_settings.albumArtLeftDoubleClick = action;
        } else if (object == L"player") {
            if (click == L"left_click") g_settings.playerLeftClick = action;
            else if (click == L"right_click") g_settings.playerRightClick = action;
            else if (click == L"middle_click") g_settings.playerMiddleClick = action;
            else if (click == L"left_double_click") g_settings.playerLeftDoubleClick = action;
        }
    }

    // A non-default dedicated dropdown overrides the array entry. Keeping
    // "Nothing" selected leaves an explicitly configured array action intact.
    if (g_settings.widgetClickAction != L"none") {
        g_settings.playerLeftClick = g_settings.widgetClickAction;
    }

    g_settings.albumArtWheelAction = L"none";
    g_settings.albumArtWheelUpAction = L"none";
    g_settings.albumArtWheelDownAction = L"none";
    g_settings.playerWheelAction   = L"none";
    g_settings.playerWheelUpAction = L"none";
    g_settings.playerWheelDownAction = L"none";

    for (int i = 0; i < 20; i++) {
        auto objectSetting = WindhawkUtils::StringSetting::make(
            L"BehaviorSettings.MouseWheelActionSettings[%d].object", i);
        auto clickSetting = WindhawkUtils::StringSetting::make(
            L"BehaviorSettings.MouseWheelActionSettings[%d].click", i);
        auto actionSetting = WindhawkUtils::StringSetting::make(
            L"BehaviorSettings.MouseWheelActionSettings[%d].action", i);
        PCWSTR objectStr = objectSetting.get();
        PCWSTR clickStr = clickSetting.get();
        PCWSTR actionStr = actionSetting.get();

        if (*objectStr == L'\0' || *clickStr == L'\0' ||
            *actionStr == L'\0') {
            break;
        }

        std::wstring object(objectStr);
        std::wstring click(clickStr);
        std::wstring action(actionStr);

        if (object.empty()) object = L"none";
        if (click.empty()) click = L"none";
        if (action.empty()) action = L"none";

        if (object == L"none" || click == L"none") {
            continue;
        }

        if (object == L"album_art") {
            if (click == L"mouse_wheel") {
                g_settings.albumArtWheelAction = action;
            } else if (click == L"mouse_wheel_up") {
                g_settings.albumArtWheelUpAction = action;
            } else if (click == L"mouse_wheel_down") {
                g_settings.albumArtWheelDownAction = action;
            }
        } else if (object == L"player") {
            if (click == L"mouse_wheel") {
                g_settings.playerWheelAction = action;
            } else if (click == L"mouse_wheel_up") {
                g_settings.playerWheelUpAction = action;
            } else if (click == L"mouse_wheel_down") {
                g_settings.playerWheelDownAction = action;
            }
        }
    }
    std::wstring preferredMediaApp = StrAllowEmpty(
        L"BehaviorSettings.MediaSourceSettings.preferredApp", L"");
    if (Wh_GetIntValue(L"quickMediaSourceOverride", -1) == 1) {
        wchar_t quickPreferredApp[1024]{};
        Wh_GetStringValue(L"quickPreferredMediaApp", quickPreferredApp,
                          ARRAYSIZE(quickPreferredApp));
        preferredMediaApp = quickPreferredApp;
    }
    StorePreferredMediaApp(preferredMediaApp);

    g_settings.hideWhenNoMedia      = Wh_GetIntSetting(L"BehaviorSettings.hideWhenNoMedia")   != 0;
    g_settings.hideFullscreen       = Wh_GetIntSetting(L"BehaviorSettings.hideFullscreen")    != 0;
    g_settings.idleHideSeconds      = std::max(Wh_GetIntSetting(L"BehaviorSettings.idleHideSeconds"), 0);
    g_settings.playerHoverEffectMode = HoverMode(L"AppearanceSettings.BackgroundStyleSettings.enablePlayerHoverEffect");
    g_settings.mediaButtonsHoverEffectMode = HoverMode(L"AppearanceSettings.BackgroundStyleSettings.enableMediaButtonsHoverEffect");


    g_settings.discordPresenceEnabled =
        Wh_GetIntSetting(L"DiscordPresenceSettings.enabled") != 0;
    g_settings.discordApplicationId = StrAllowEmpty(
        L"DiscordPresenceSettings.applicationId", L"1528896038163710112");
    g_settings.discordShowPaused =
        Wh_GetIntSetting(L"DiscordPresenceSettings.showPaused") != 0;
    g_settings.discordActivityName = StrAllowEmpty(
        L"DiscordPresenceSettings.activityName", L"{title}");
    g_settings.discordLargeImageKey = StrAllowEmpty(
        L"DiscordPresenceSettings.largeImageKey", L"taskbar_media_presence");

    g_settings.hideUnsupportedButtons  = Wh_GetIntSetting(L"MainSettings.MediaButtonsSettings.hideUnsupportedButtons") != 0;

    g_settings.disableAlbumArtClick    = Wh_GetIntSetting(L"BehaviorSettings.disableAlbumArtClick") != 0;

    g_settings.ignoredProcesses = Str(
        L"DebugSettings.ignoredProcesses", L"");
    g_settings.enableTreeDump =
        Wh_GetIntSetting(L"DebugSettings.enableTreeDump") != 0;
    g_settings.showDebugBorders =
        Wh_GetIntSetting(L"DebugSettings.showDebugBorders") != 0;
    g_settings.showLayoutAnchors =
        Wh_GetIntSetting(L"DebugSettings.showLayoutAnchors") != 0;
    try {
        std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
        g_mediaButtons.clear();
        std::set<MediaButtonType> seen;

        for (int i = 0; i < 10; i++) {
            try {
                auto itemSetting = WindhawkUtils::StringSetting::make(
                    L"MainSettings.MediaButtonsSettings.mediaButtons[%d]", i);
                PCWSTR itemStr = itemSetting.get();
                if (!*itemStr) {
                    break;
                }

                std::wstring keyword(itemStr);

                for (const auto& def : g_mediaButtonDefinitions) {
                    if (def.keyword == keyword && seen.insert(def.type).second) {
                        g_mediaButtons.push_back({def.type, def.cmd});
                        break;
                    }
                }
            } catch (...) {
                Wh_Log(L"LoadSettings: Exception parsing media button at index %d", i);
            }
        }

        // Preserve custom layouts, but upgrade the exact pre-0.10 default so
        // existing users receive the new playback-mode controls automatically.
        if (g_mediaButtons.size() == 4 &&
            g_mediaButtons[0].type == MediaButtonType::Previous &&
            g_mediaButtons[1].type == MediaButtonType::PlayPause &&
            g_mediaButtons[2].type == MediaButtonType::Next &&
            g_mediaButtons[3].type == MediaButtonType::Volume) {
            g_mediaButtons.insert(g_mediaButtons.begin(), {
                {MediaButtonType::Shuffle, 7},
                {MediaButtonType::Repeat, 8}
            });
        }

        if (g_mediaButtons.empty()) {
            Wh_Log(L"LoadSettings: No media buttons configured, showing none");
        }
    } catch (...) {
        Wh_Log(L"LoadSettings: Critical exception in media buttons parsing, using defaults");
        try {
            std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
            g_mediaButtons = {
                {MediaButtonType::Shuffle, 7},
                {MediaButtonType::Repeat, 8},
                {MediaButtonType::Previous, 1},
                {MediaButtonType::PlayPause, 2},
                {MediaButtonType::Next, 3},
                {MediaButtonType::Volume, 13}
            };
        } catch (...) {}
    }

    if (g_settings.position == L"taskbar_left")
        g_settings.position = L"taskbar_left_start";
    else if (g_settings.position == L"taskbar_right")
        g_settings.position = L"taskbar_right_start";
    else if (g_settings.position == L"taskbar_after_start")
        g_settings.position = L"taskbar_after_search_right";
    else if (g_settings.position == L"taskbar_after_search")
        g_settings.position = L"taskbar_after_search_right";
    else if (g_settings.position == L"tray_before_omni")
        g_settings.position = L"tray_before_omni_right";
    else if (g_settings.position == L"tray_after_showdesktop")
        g_settings.position = L"tray_after_showdesktop_right";

    int quickPositionPreset = Wh_GetIntValue(L"quickPositionPreset", -1);
    if (quickPositionPreset == 0) g_settings.positionPreset = L"apps";
    else if (quickPositionPreset == 1) g_settings.positionPreset = L"left";
    else if (quickPositionPreset == 2) g_settings.positionPreset = L"middle";
    else if (quickPositionPreset == 3) g_settings.positionPreset = L"right";
    else if (quickPositionPreset == 4) g_settings.positionPreset = L"custom";

    int quickTaskbarMode = Wh_GetIntValue(L"quickTaskbarMode", -1);
    if (quickTaskbarMode == 0) g_settings.taskbarMode = L"selected";
    else if (quickTaskbarMode == 1) g_settings.taskbarMode = L"all";
    int quickTaskbarNumber = Wh_GetIntValue(L"quickTaskbarNumber", -1);
    if (quickTaskbarNumber > 0) g_settings.taskbarNumber = quickTaskbarNumber;

    if (g_settings.positionPreset == L"left") {
        g_settings.position = L"taskbar_left_start";
    } else if (g_settings.positionPreset == L"apps") {
        g_settings.position = L"taskbar_after_apps";
    } else if (g_settings.positionPreset == L"middle") {
        // Center-on-center is covered by Windows' app buttons. Use the free
        // far-left area for a centered Windows taskbar, reserving that space
        // in the taskbar repeater so the player can't cover Widgets. Keep the
        // original middle overlay when Windows itself is left aligned.
        g_settings.position = IsWindowsTaskbarCentered()
            ? L"taskbar_far_left_reserved"
            : L"taskbar_center_edge";
    } else if (g_settings.positionPreset == L"right") {
        g_settings.position = L"tray_left";
    }

    int quickBackground = Wh_GetIntValue(L"quickBackground", -1);
    if (quickBackground == 0) g_settings.backgroundType = L"none";
    else if (quickBackground == 1) g_settings.backgroundType = L"acrylic";

    int quickTransparentTaskbar = Wh_GetIntValue(L"quickTransparentTaskbar", -1);
    if (quickTransparentTaskbar == 0) g_settings.transparentTaskbar = false;
    else if (quickTransparentTaskbar == 1) g_settings.transparentTaskbar = true;

    // The widget wheel never modifies Windows master volume.
    auto normalizeWheelAction = [](std::wstring& action) {
        if (action == L"system_sound") action = L"app_sound";
    };
    normalizeWheelAction(g_settings.playerWheelAction);
    normalizeWheelAction(g_settings.playerWheelUpAction);
    normalizeWheelAction(g_settings.playerWheelDownAction);
    normalizeWheelAction(g_settings.albumArtWheelAction);
    normalizeWheelAction(g_settings.albumArtWheelUpAction);
    normalizeWheelAction(g_settings.albumArtWheelDownAction);

    std::shared_ptr<const ModSettings> snapshot =
        std::make_shared<ModSettings>(std::move(g_settings));
    {
        std::lock_guard<std::mutex> lock(g_settingsSnapshotMtx);
        g_settingsSnapshot = std::move(snapshot);
    }
}

// Settings readers span the taskbar UI, media, timer, and Discord threads.
// Each full expression owns the loaded immutable snapshot, so a reload can
// retire the previous snapshot as soon as its last active reader finishes.
static std::shared_ptr<const ModSettings> Cfg() {
    std::lock_guard<std::mutex> lock(g_settingsSnapshotMtx);
    return g_settingsSnapshot;
}

static HWND FindCurrentProcessTaskbarWnd();
static HWND FindCurrentProcessTaskbarWndForThread(DWORD threadId);
static bool IsCurrentProcessTaskbarWindow(HWND window,
                                          DWORD* threadId = nullptr,
                                          bool* secondary = nullptr);
static void DispatchMediaUpdate();
static bool ApplySettings();
static bool ApplyTaskbarTransparencyToAll(
    bool shutdownCleanup = false,
    std::optional<bool> transparentOverride = std::nullopt);
static bool HasExternalTaskbarTransparencyProvider();

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_applyingSettings{false};
static std::atomic<bool> g_hookInjectionInProgress{false};
static std::atomic<bool> g_taskbarXamlCallbacksSuppressed{false};
static std::atomic<uint64_t> g_taskbarRestartGeneration{0};
static std::atomic<unsigned> g_taskbarStartInProgress{0};
static std::atomic<unsigned> g_taskbarOriginalsInProgress{0};
static bool              g_isShellExplorerProcess = false;
static std::mutex g_asyncTasksMtx;
static std::deque<std::function<void()>> g_asyncTaskQueue;
static HANDLE g_asyncTaskThread = nullptr;
static HANDLE g_asyncTaskAvailableEvent = nullptr;
static HANDLE g_asyncTaskStopEvent = nullptr;
static HANDLE g_asyncTaskIdleEvent = nullptr;
static std::atomic<DWORD> g_asyncTaskThreadId{0};
static bool g_asyncTaskRunning = false;
static bool g_acceptAsyncTasks = true;

template <typename TAsyncOperation>
static auto WaitForWinrtOperation(
    TAsyncOperation const& operation,
    DWORD timeoutMs,
    PCWSTR operationName = nullptr)
    -> std::optional<decltype(operation.GetResults())> {
    using winrt::Windows::Foundation::AsyncStatus;

    const ULONGLONG started = GetTickCount64();
    while (true) {
        AsyncStatus status = operation.Status();
        if (status == AsyncStatus::Completed) {
            return operation.GetResults();
        }
        if (status == AsyncStatus::Canceled) {
            return std::nullopt;
        }
        if (status == AsyncStatus::Error) {
            // GetResults preserves the original HRESULT for the caller's
            // surrounding exception boundary.
            return operation.GetResults();
        }

        bool timedOut =
            timeoutMs != INFINITE &&
            GetTickCount64() - started >= timeoutMs;
        if (g_unloading || g_applyingSettings || timedOut) {
            try {
                operation.Cancel();
            } catch (...) {}
            if (timedOut && operationName) {
                Wh_Log(L"%ls timed out after %u ms",
                       operationName, timeoutMs);
            }
            return std::nullopt;
        }
        Sleep(10);
    }
}

static void UpdateAsyncTaskIdleEventLocked() {
    if (!g_asyncTaskIdleEvent) return;
    if (!g_asyncTaskRunning && g_asyncTaskQueue.empty()) {
        SetEvent(g_asyncTaskIdleEvent);
    } else {
        ResetEvent(g_asyncTaskIdleEvent);
    }
}

static DWORD WINAPI AsyncTaskThreadProc(void*) noexcept {
    g_asyncTaskThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    HRESULT apartmentResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool callCancellationEnabled =
        SUCCEEDED(CoEnableCallCancellation(nullptr));

    while (true) {
        std::function<void()> task;
        {
            std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
            if (!g_asyncTaskQueue.empty()) {
                task = std::move(g_asyncTaskQueue.front());
                g_asyncTaskQueue.pop_front();
                g_asyncTaskRunning = true;
                UpdateAsyncTaskIdleEventLocked();
                if (g_asyncTaskQueue.empty() && g_asyncTaskAvailableEvent) {
                    ResetEvent(g_asyncTaskAvailableEvent);
                }
            }
        }

        if (task) {
            try {
                task();
            } catch (...) {
                Wh_Log(L"Background task: unhandled exception");
            }
            {
                std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
                g_asyncTaskRunning = false;
                UpdateAsyncTaskIdleEventLocked();
                if (!g_asyncTaskQueue.empty() && g_asyncTaskAvailableEvent) {
                    SetEvent(g_asyncTaskAvailableEvent);
                }
            }
            continue;
        }

        HANDLE handles[] = {g_asyncTaskStopEvent, g_asyncTaskAvailableEvent};
        DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) break;
        if (result != WAIT_OBJECT_0 + 1) break;
    }

    {
        std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
        g_asyncTaskQueue.clear();
        g_asyncTaskRunning = false;
        UpdateAsyncTaskIdleEventLocked();
    }
    if (callCancellationEnabled) CoDisableCallCancellation(nullptr);
    if (SUCCEEDED(apartmentResult)) CoUninitialize();
    g_asyncTaskThreadId.store(0, std::memory_order_release);
    return 0;
}

static void CloseAsyncTaskWorkerHandlesLocked() {
    if (g_asyncTaskThread) {
        CloseHandle(g_asyncTaskThread);
        g_asyncTaskThread = nullptr;
    }
    if (g_asyncTaskAvailableEvent) {
        CloseHandle(g_asyncTaskAvailableEvent);
        g_asyncTaskAvailableEvent = nullptr;
    }
    if (g_asyncTaskStopEvent) {
        CloseHandle(g_asyncTaskStopEvent);
        g_asyncTaskStopEvent = nullptr;
    }
    if (g_asyncTaskIdleEvent) {
        CloseHandle(g_asyncTaskIdleEvent);
        g_asyncTaskIdleEvent = nullptr;
    }
    g_asyncTaskThreadId.store(0, std::memory_order_release);
    g_asyncTaskRunning = false;
    g_asyncTaskQueue.clear();
}

static bool StartAsyncTaskWorker() {
    std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
    if (g_asyncTaskThread) {
        if (WaitForSingleObject(g_asyncTaskThread, 0) == WAIT_TIMEOUT) {
            return true;
        }
        CloseAsyncTaskWorkerHandlesLocked();
    }

    g_asyncTaskAvailableEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_asyncTaskStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_asyncTaskIdleEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    if (!g_asyncTaskAvailableEvent || !g_asyncTaskStopEvent ||
        !g_asyncTaskIdleEvent) {
        CloseAsyncTaskWorkerHandlesLocked();
        return false;
    }

    g_asyncTaskThread =
        CreateThread(nullptr, 0, AsyncTaskThreadProc, nullptr, 0, nullptr);
    if (!g_asyncTaskThread) {
        CloseAsyncTaskWorkerHandlesLocked();
        return false;
    }
    return true;
}

static bool QueueAsyncTask(std::function<void()> task) {
    if (!task || g_unloading) return false;
    if (!StartAsyncTaskWorker()) return false;

    std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
    if (!g_acceptAsyncTasks || g_unloading || !g_asyncTaskThread) {
        return false;
    }
    try {
        g_asyncTaskQueue.push_back(std::move(task));
    } catch (...) {
        return false;
    }
    UpdateAsyncTaskIdleEventLocked();
    SetEvent(g_asyncTaskAvailableEvent);
    return true;
}

static void PauseAsyncTasks(bool discardPending = false) {
    std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
    g_acceptAsyncTasks = false;
    if (discardPending) {
        g_asyncTaskQueue.clear();
        if (g_asyncTaskAvailableEvent) ResetEvent(g_asyncTaskAvailableEvent);
    }
    UpdateAsyncTaskIdleEventLocked();
}

static void ResumeAsyncTasks() {
    if (g_unloading || !StartAsyncTaskWorker()) return;
    std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
    g_acceptAsyncTasks = true;
    if (!g_asyncTaskQueue.empty() && g_asyncTaskAvailableEvent) {
        SetEvent(g_asyncTaskAvailableEvent);
    }
}

static bool WaitForThreadExit(HANDLE thread, DWORD timeoutMs = INFINITE) {
    if (!thread) return true;
    ULONGLONG deadline = timeoutMs == INFINITE
        ? 0
        : GetTickCount64() + timeoutMs;

    while (true) {
        DWORD remaining = INFINITE;
        if (timeoutMs != INFINITE) {
            ULONGLONG now = GetTickCount64();
            remaining = now < deadline
                ? static_cast<DWORD>(std::min<ULONGLONG>(
                      deadline - now, MAXDWORD))
                : 0;
        }

        DWORD result = MsgWaitForMultipleObjects(
            1, &thread, FALSE, remaining, QS_SENDMESSAGE);
        if (result == WAIT_OBJECT_0) return true;
        if (result == WAIT_OBJECT_0 + 1) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0,
                                PM_REMOVE | PM_QS_SENDMESSAGE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            if (timeoutMs != INFINITE && GetTickCount64() >= deadline) {
                return WaitForSingleObject(thread, 0) == WAIT_OBJECT_0;
            }
            continue;
        }
        if (result == WAIT_TIMEOUT) return false;
        return WaitForSingleObject(thread, 0) == WAIT_OBJECT_0;
    }
}

static void CancelAsyncTaskWorkerCall() {
    DWORD threadId =
        g_asyncTaskThreadId.load(std::memory_order_acquire);
    if (threadId) {
        CoCancelCall(threadId, 0);
    }
}

static bool WaitForAsyncTasks(DWORD timeoutMs) {
    HANDLE idleEvent = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
        UpdateAsyncTaskIdleEventLocked();
        idleEvent = g_asyncTaskIdleEvent;
    }
    return !idleEvent || WaitForThreadExit(idleEvent, timeoutMs);
}

static bool StopAsyncTaskWorker(DWORD timeoutMs = 3000) {
    HANDLE thread = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
        g_acceptAsyncTasks = false;
        g_asyncTaskQueue.clear();
        UpdateAsyncTaskIdleEventLocked();
        thread = g_asyncTaskThread;
        if (g_asyncTaskStopEvent) SetEvent(g_asyncTaskStopEvent);
        if (g_asyncTaskAvailableEvent) SetEvent(g_asyncTaskAvailableEvent);
    }
    CancelAsyncTaskWorkerCall();
    bool stopped = !thread || WaitForThreadExit(thread, timeoutMs);
    if (stopped) {
        std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
        CloseAsyncTaskWorkerHandlesLocked();
    }
    return stopped;
}

static const CLSID XIID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID XIID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID XIID_IAudioSessionManager2 = __uuidof(IAudioSessionManager2);

static std::atomic<HWND> g_taskbarWnd{nullptr};
static std::atomic<HWND> g_playerOwnerWindow{nullptr};
static std::atomic<DWORD> g_playerOwnerThreadId{0};
static std::atomic<HANDLE> g_playerOwnerThreadHandle{nullptr};
static std::atomic<ULONGLONG> g_taskbarRestartNotBeforeTick{0};
[[clang::no_destroy]] static Grid             g_playerGrid      = nullptr;
[[clang::no_destroy]] static FrameworkElement g_injectionParent = nullptr;
static int              g_playerColumn    = -1;
struct ShiftedColumnChild {
    FrameworkElement element{nullptr};
    int originalColumn = 0;
};
static bool g_playerColumnInserted = false;
static bool g_playerColumnShiftCommitted = false;
[[clang::no_destroy]] static std::vector<ShiftedColumnChild> g_playerColumnShiftedChildren;
static std::atomic<bool> g_needsUiUpdate{false};

static HANDLE OpenTaskbarOwnerThreadHandle(DWORD threadId) {
    if (!threadId) return nullptr;
    return OpenThread(
        SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION,
        FALSE, threadId);
}

static bool IsOriginalTaskbarThreadAlive(
    HANDLE threadHandle, DWORD threadId) {
    if (!threadHandle || !threadId) return false;
    return GetThreadId(threadHandle) == threadId &&
           WaitForSingleObject(threadHandle, 0) == WAIT_TIMEOUT;
}

static void CloseOwnedThreadHandle(std::atomic<HANDLE>& slot) {
    HANDLE handle = slot.exchange(nullptr, std::memory_order_acq_rel);
    if (handle) CloseHandle(handle);
}

struct TextScrollState {
    double offset    = 0.0;
    double textWidth = 0.0;
    double viewWidth = 0.0;
    bool   forward   = true;
    bool   active    = false;
    bool   pausing   = false;
    int    pauseTick = 0;
    int    tickMs    = 16;
};

struct BlurBgCache {
    std::vector<BYTE> blurredPixels;
    int               width = 0;
    int               height = 0;
    size_t            artHash = 0;

    void Invalidate() {
        blurredPixels.clear();
        width = height = 0;
        artHash = 0;
    }
};

struct AlbumPalette {
    winrt::Windows::UI::Color primary;
    winrt::Windows::UI::Color secondary;
};

struct PlayerVisualState {
    std::wstring cachedAlbumTitle;
    std::wstring cachedAlbumArtist;
    std::vector<BYTE> cachedThumbnailBytes;
    int cachedAlbumPixelWidth = 0;
    int cachedAlbumPixelHeight = 0;
    int cachedAppIconSize = -1;
    std::wstring scrollCachedTitle;
    std::wstring scrollCachedArtist;
    TextScrollState titleScroll;
    TextScrollState artistScroll;
    BlurBgCache blurBgCache;
    AlbumPalette cachedAlbumPalette{
        winrt::Windows::UI::Color{255, 18, 18, 18},
        winrt::Windows::UI::Color{255, 45, 45, 45}};
    size_t cachedPaletteHash = 0;
    winrt::Windows::Foundation::IAsyncAction albumArtDecodeAction{nullptr};
    Controls::ToolTip progressToolTip{nullptr};
    bool progressDragging = false;
    bool progressHovered = false;
    double progressDragFraction = 0.0;
    uint64_t progressDragGeneration = 0;
    int64_t progressDragDurationSeconds = 0;
    std::wstring progressDragTitle;
    std::wstring progressDragArtist;
    uint64_t progressDragThumbnailHash = 0;
    std::atomic<bool> xamlCallbacksActive{false};
    std::vector<std::function<bool()>> xamlSubscriptionRevokers;
};

static bool TaskbarXamlCallbacksSuppressed() {
    return g_unloading.load(std::memory_order_acquire) ||
           g_applyingSettings.load(std::memory_order_acquire) ||
           g_taskbarXamlCallbacksSuppressed.load(std::memory_order_acquire);
}

static bool TaskbarRestartSettleWindowActive() {
    if (g_taskbarOriginalsInProgress.load(std::memory_order_acquire)) {
        return true;
    }
    ULONGLONG deadline =
        g_taskbarRestartNotBeforeTick.load(std::memory_order_acquire);
    return deadline && GetTickCount64() < deadline;
}

static void ScheduleTaskbarRebuildRetry(DWORD delayMs) {
    ULONGLONG desired = GetTickCount64() + delayMs;
    ULONGLONG current =
        g_taskbarRestartNotBeforeTick.load(std::memory_order_acquire);
    while (current < desired &&
           !g_taskbarRestartNotBeforeTick.compare_exchange_weak(
               current, desired, std::memory_order_acq_rel)) {}
}

static bool PlayerXamlCallbacksAllowed(
    std::weak_ptr<PlayerVisualState> const& weakState) {
    auto state = weakState.lock();
    return state &&
           state->xamlCallbacksActive.load(std::memory_order_acquire) &&
           !TaskbarXamlCallbacksSuppressed();
}

template <typename TSource, typename TRevoke>
static void TrackPlayerXamlSubscription(
    std::shared_ptr<PlayerVisualState> const& state,
    TSource const& source,
    winrt::event_token token,
    TRevoke revoke) {
    if (!state || !source || !token.value) return;
    state->xamlSubscriptionRevokers.emplace_back(
        [source, token, revoke = std::move(revoke)]() mutable {
            try {
                revoke(source, token);
                return true;
            } catch (...) {
                return false;
            }
        });
}

static bool RevokePlayerXamlSubscriptions(
    std::shared_ptr<PlayerVisualState> const& state) {
    if (!state) return true;
    state->xamlCallbacksActive.store(false, std::memory_order_release);
    state->progressToolTip = nullptr;
    state->progressDragging = false;
    state->progressHovered = false;
    state->progressDragFraction = 0.0;
    state->progressDragGeneration = 0;
    state->progressDragDurationSeconds = 0;
    state->progressDragTitle.clear();
    state->progressDragArtist.clear();
    state->progressDragThumbnailHash = 0;
    try {
        if (state->albumArtDecodeAction) {
            state->albumArtDecodeAction.Cancel();
            state->albumArtDecodeAction = nullptr;
        }
    } catch (...) {
        state->albumArtDecodeAction = nullptr;
    }

    std::vector<std::function<bool()>> failed;
    failed.reserve(state->xamlSubscriptionRevokers.size());
    for (auto iterator = state->xamlSubscriptionRevokers.rbegin();
         iterator != state->xamlSubscriptionRevokers.rend(); ++iterator) {
        bool revoked = false;
        try {
            revoked = (*iterator)();
        } catch (...) {}
        if (!revoked) failed.push_back(std::move(*iterator));
    }
    std::reverse(failed.begin(), failed.end());
    state->xamlSubscriptionRevokers = std::move(failed);
    return state->xamlSubscriptionRevokers.empty();
}

[[clang::no_destroy]] static std::shared_ptr<PlayerVisualState> g_primaryVisualState =
    std::make_shared<PlayerVisualState>();

struct MirrorPlayerInstance {
    HWND taskbarWindow = nullptr;
    DWORD ownerThreadId = 0;
    HANDLE ownerThreadHandle = nullptr;
    Grid playerGrid{nullptr};
    Grid targetGrid{nullptr};
    int playerColumn = -1;
    bool playerColumnInserted = false;
    bool playerColumnShiftCommitted = false;
    std::vector<ShiftedColumnChild> playerColumnShiftedChildren;
    FrameworkElement trackedElement{nullptr};
    Thickness trackedOriginalMargin{};
    winrt::event_token layoutUpdatedToken{};
    std::shared_ptr<PlayerVisualState> visualState =
        std::make_shared<PlayerVisualState>();
};

[[clang::no_destroy]] static std::vector<std::shared_ptr<MirrorPlayerInstance>> g_mirrorPlayers;
static std::mutex g_mirrorPlayersMtx;

static std::vector<std::shared_ptr<MirrorPlayerInstance>>
SnapshotMirrorPlayers() {
    std::lock_guard<std::mutex> lock(g_mirrorPlayersMtx);
    return g_mirrorPlayers;
}

static bool MirrorPlayersEmpty() {
    std::lock_guard<std::mutex> lock(g_mirrorPlayersMtx);
    return g_mirrorPlayers.empty();
}

static void AddMirrorPlayer(
    std::shared_ptr<MirrorPlayerInstance> const& instance) {
    std::lock_guard<std::mutex> lock(g_mirrorPlayersMtx);
    g_mirrorPlayers.push_back(instance);
}

static void RemoveMirrorPlayerReference(
    std::shared_ptr<MirrorPlayerInstance> const& instance) {
    std::lock_guard<std::mutex> lock(g_mirrorPlayersMtx);
    std::erase(g_mirrorPlayers, instance);
}

static void RemoveMirrorPlayerReferences(
    std::vector<std::shared_ptr<MirrorPlayerInstance>> const& instances) {
    if (instances.empty()) return;
    std::lock_guard<std::mutex> lock(g_mirrorPlayersMtx);
    std::erase_if(
        g_mirrorPlayers,
        [&](std::shared_ptr<MirrorPlayerInstance> const& candidate) {
            return std::find(instances.begin(), instances.end(), candidate) !=
                   instances.end();
        });
}

static void ClearMirrorPlayers() {
    std::lock_guard<std::mutex> lock(g_mirrorPlayersMtx);
    g_mirrorPlayers.clear();
}

static void ReleaseMirrorPlayersStorage() {
    std::lock_guard<std::mutex> lock(g_mirrorPlayersMtx);
    std::vector<std::shared_ptr<MirrorPlayerInstance>>().swap(
        g_mirrorPlayers);
}
[[clang::no_destroy]] static FrameworkElement g_trackedElement = nullptr;
static Thickness g_trackedElementOriginalMargin{};
static bool g_hasTrackedElementOriginalMargin = false;
static std::wstring g_trackPosition = L"";
static winrt::event_token g_layoutUpdateToken{};

using CTaskBand_GetTaskbarHost_t  = void*(WINAPI*)(void*, void*);
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t   = int  (WINAPI*)(void*);
using Std_Ref_Decref_t            = void (WINAPI*)(void*);

static CTaskBand_GetTaskbarHost_t  CTaskBand_GetTaskbarHost_Original  = nullptr;
static CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
static TaskbarHost_FrameHeight_t   TaskbarHost_FrameHeight_Original   = nullptr;
static Std_Ref_Decref_t            Std_Ref_Decref_Original            = nullptr;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;
static void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

using WindowThreadProc = void(*)(void*);

enum class WindowDispatchPhase : LONG {
    Pending,
    Running,
    Completed,
    Cancelled,
};

struct WindowDispatchRequest {
    WindowThreadProc proc = nullptr;
    void* param = nullptr;
    bool allowDuringShutdown = false;
    std::atomic<WindowDispatchPhase> phase{WindowDispatchPhase::Pending};
    std::atomic<bool> succeeded{false};
    HANDLE completedEvent = nullptr;

    ~WindowDispatchRequest() {
        if (completedEvent) CloseHandle(completedEvent);
    }
};

static std::mutex g_windowDispatchRequestsMtx;
static std::unordered_map<ULONG_PTR, std::shared_ptr<WindowDispatchRequest>>
    g_windowDispatchRequests;
static std::atomic<ULONG_PTR> g_nextWindowDispatchId{1};
static std::mutex g_failedWindowDispatchHooksMtx;
static std::vector<HHOOK> g_failedWindowDispatchHooks;

static SRWLOCK g_windowDispatchActivityLock = SRWLOCK_INIT;
static CONDITION_VARIABLE g_windowDispatchActivityChanged =
    CONDITION_VARIABLE_INIT;
static LONG g_activeWindowDispatchCalls = 0;
static LONG g_activeWindowDispatchHookCallbacks = 0;
static bool g_windowDispatchShuttingDown = false;
static thread_local bool g_allowWindowDispatchDuringShutdown = false;

class WindowDispatchActivityGuard {
public:
    enum class Kind { Call, HookCallback };

    WindowDispatchActivityGuard(Kind kind, bool allowDuringShutdown = false)
        : m_kind(kind) {
        AcquireSRWLockExclusive(&g_windowDispatchActivityLock);
        if (kind == Kind::Call &&
            g_windowDispatchShuttingDown &&
            !allowDuringShutdown &&
            !g_allowWindowDispatchDuringShutdown) {
            ReleaseSRWLockExclusive(&g_windowDispatchActivityLock);
            return;
        }

        if (kind == Kind::Call) {
            ++g_activeWindowDispatchCalls;
        } else {
            ++g_activeWindowDispatchHookCallbacks;
        }
        m_entered = true;
        ReleaseSRWLockExclusive(&g_windowDispatchActivityLock);
    }

    ~WindowDispatchActivityGuard() {
        if (!m_entered) return;
        AcquireSRWLockExclusive(&g_windowDispatchActivityLock);
        if (m_kind == Kind::Call) {
            --g_activeWindowDispatchCalls;
        } else {
            --g_activeWindowDispatchHookCallbacks;
        }
        WakeAllConditionVariable(&g_windowDispatchActivityChanged);
        ReleaseSRWLockExclusive(&g_windowDispatchActivityLock);
    }

    explicit operator bool() const {
        return m_entered;
    }

private:
    Kind m_kind;
    bool m_entered = false;
};

class WindowDispatchShutdownScope {
public:
    explicit WindowDispatchShutdownScope(bool enable)
        : m_previous(g_allowWindowDispatchDuringShutdown) {
        if (enable) g_allowWindowDispatchDuringShutdown = true;
    }

    ~WindowDispatchShutdownScope() {
        g_allowWindowDispatchDuringShutdown = m_previous;
    }

private:
    bool m_previous;
};

static bool InvokeWindowThreadProcWithCppExceptionBoundary(
    WindowThreadProc proc, void* param) {
    try {
        proc(param);
        return true;
    } catch (...) {
        Wh_Log(L"RunFromWindowThread: dispatched procedure threw an exception");
        return false;
    }
}

static bool InvokeWindowThreadProcSafely(WindowThreadProc proc, void* param) {
    if (!proc) return false;
    return InvokeWindowThreadProcWithCppExceptionBoundary(proc, param);
}

static std::shared_ptr<WindowDispatchRequest> FindWindowDispatchRequest(
    ULONG_PTR id) {
    std::lock_guard<std::mutex> lock(g_windowDispatchRequestsMtx);
    auto iterator = g_windowDispatchRequests.find(id);
    return iterator == g_windowDispatchRequests.end()
        ? nullptr
        : iterator->second;
}

static LRESULT CALLBACK WindowThreadDispatchHookProc(
    int code, WPARAM wParam, LPARAM lParam) {
    WindowDispatchActivityGuard activity(
        WindowDispatchActivityGuard::Kind::HookCallback);

    if (code == HC_ACTION) {
        try {
            auto* call = reinterpret_cast<const CWPSTRUCT*>(lParam);
            static const UINT message = RegisterWindowMessage(
                L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
            if (call && message && call->message == message) {
                auto request = FindWindowDispatchRequest(
                    static_cast<ULONG_PTR>(call->lParam));
                if (request) {
                    WindowDispatchPhase expected =
                        WindowDispatchPhase::Pending;
                    if (request->phase.compare_exchange_strong(
                            expected, WindowDispatchPhase::Running,
                            std::memory_order_acq_rel)) {
                        WindowDispatchShutdownScope shutdownScope(
                            request->allowDuringShutdown);
                        request->succeeded.store(
                            InvokeWindowThreadProcSafely(
                                request->proc, request->param),
                            std::memory_order_release);
                        request->phase.store(
                            WindowDispatchPhase::Completed,
                            std::memory_order_release);
                        SetEvent(request->completedEvent);
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"RunFromWindowThread: hook callback exception");
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static void WaitForWindowDispatchCompletion(HANDLE completedEvent) {
    if (!completedEvent) return;
    while (true) {
        DWORD result = MsgWaitForMultipleObjects(
            1, &completedEvent, FALSE, INFINITE, QS_SENDMESSAGE);
        if (result == WAIT_OBJECT_0) return;
        if (result == WAIT_OBJECT_0 + 1) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0,
                                PM_REMOVE | PM_QS_SENDMESSAGE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            continue;
        }

        // The event handle belongs to a live request, so WAIT_FAILED is not
        // expected. A direct wait is the safest fallback because returning
        // while the callback is running could invalidate a caller's stack
        // parameter.
        WaitForSingleObject(completedEvent, INFINITE);
        return;
    }
}

static bool UnhookWindowDispatchHookConfirmed(
    HHOOK hook, DWORD* errorOut = nullptr) {
    if (errorOut) *errorOut = ERROR_SUCCESS;
    if (!hook) return true;

    if (UnhookWindowsHookEx(hook)) return true;

    DWORD error = GetLastError();
    if (errorOut) *errorOut = error;

    // An invalid hook handle means the hook is no longer registered, so there
    // is no callback target left to keep this module alive for.
    return error == ERROR_INVALID_HOOK_HANDLE;
}

static bool RunFromWindowThreadImpl(
    HWND hWnd, WindowThreadProc proc, void* param,
    bool allowDuringShutdown, DWORD timeoutMs) {
    WindowDispatchActivityGuard activity(
        WindowDispatchActivityGuard::Kind::Call, allowDuringShutdown);
    if (!activity || !hWnd || !proc || !IsWindow(hWnd)) return false;

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid || IsHungAppWindow(hWnd)) return false;

    if (tid == GetCurrentThreadId()) {
        WindowDispatchShutdownScope shutdownScope(allowDuringShutdown);
        return InvokeWindowThreadProcSafely(proc, param);
    }

    static const UINT message = RegisterWindowMessage(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    if (!message) return false;

    auto request = std::make_shared<WindowDispatchRequest>();
    request->proc = proc;
    request->param = param;
    request->allowDuringShutdown = allowDuringShutdown;
    request->completedEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!request->completedEvent) return false;

    ULONG_PTR requestId = g_nextWindowDispatchId.fetch_add(
        1, std::memory_order_relaxed);
    if (!requestId) {
        requestId = g_nextWindowDispatchId.fetch_add(
            1, std::memory_order_relaxed);
    }
    {
        std::lock_guard<std::mutex> lock(g_windowDispatchRequestsMtx);
        while (!requestId ||
               g_windowDispatchRequests.find(requestId) !=
                   g_windowDispatchRequests.end()) {
            requestId = g_nextWindowDispatchId.fetch_add(
                1, std::memory_order_relaxed);
        }
        g_windowDispatchRequests.emplace(requestId, request);
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC, WindowThreadDispatchHookProc, nullptr, tid);
    if (!hook) {
        std::lock_guard<std::mutex> lock(g_windowDispatchRequestsMtx);
        g_windowDispatchRequests.erase(requestId);
        return false;
    }

    DWORD_PTR ignoredResult = 0;
    LRESULT sent = SendMessageTimeoutW(
        hWnd, message, 0, static_cast<LPARAM>(requestId),
        SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT,
        timeoutMs, &ignoredResult);

    WindowDispatchPhase phase = request->phase.load(
        std::memory_order_acquire);
    if (!sent && phase == WindowDispatchPhase::Pending) {
        request->phase.compare_exchange_strong(
            phase, WindowDispatchPhase::Cancelled,
            std::memory_order_acq_rel);
    } else if (sent && phase == WindowDispatchPhase::Pending) {
        // A successful send without our hook seeing the message must never
        // leave a live request that can run after this function returns.
        request->phase.compare_exchange_strong(
            phase, WindowDispatchPhase::Cancelled,
            std::memory_order_acq_rel);
    }

    DWORD unhookError = ERROR_SUCCESS;
    if (!UnhookWindowDispatchHookConfirmed(hook, &unhookError)) {
        std::lock_guard<std::mutex> lock(g_failedWindowDispatchHooksMtx);
        g_failedWindowDispatchHooks.push_back(hook);
        Wh_Log(L"RunFromWindowThread: UnhookWindowsHookEx failed (%u)",
               unhookError);
    }

    {
        std::lock_guard<std::mutex> lock(g_windowDispatchRequestsMtx);
        auto iterator = g_windowDispatchRequests.find(requestId);
        if (iterator != g_windowDispatchRequests.end() &&
            iterator->second == request) {
            g_windowDispatchRequests.erase(iterator);
        }
    }

    phase = request->phase.load(std::memory_order_acquire);
    if (phase == WindowDispatchPhase::Running) {
        // Once execution has begun, wait for it to finish. This is the only
        // way to preserve the lifetime of arbitrary caller-owned parameters.
        WaitForWindowDispatchCompletion(request->completedEvent);
        phase = request->phase.load(std::memory_order_acquire);
    }

    return phase == WindowDispatchPhase::Completed &&
           request->succeeded.load(std::memory_order_acquire);
}

static bool RunFromWindowThread(
    HWND hWnd, WindowThreadProc proc, void* param) {
    return RunFromWindowThreadImpl(hWnd, proc, param, false, 2500);
}

static bool RunFromWindowThreadForCleanup(
    HWND hWnd, WindowThreadProc proc, void* param) {
    return RunFromWindowThreadImpl(hWnd, proc, param, true, 4000);
}

static void BeginWindowDispatchShutdown() {
    AcquireSRWLockExclusive(&g_windowDispatchActivityLock);
    g_windowDispatchShuttingDown = true;
    ReleaseSRWLockExclusive(&g_windowDispatchActivityLock);
}

static void ResetWindowDispatchShutdown() {
    AcquireSRWLockExclusive(&g_windowDispatchActivityLock);
    g_windowDispatchShuttingDown = false;
    ReleaseSRWLockExclusive(&g_windowDispatchActivityLock);
}

static bool WaitForWindowDispatchActivityIdle(DWORD timeoutMs) {
    ULONGLONG deadline = timeoutMs == INFINITE
        ? 0
        : GetTickCount64() + timeoutMs;
    bool idle = true;

    AcquireSRWLockExclusive(&g_windowDispatchActivityLock);
    while (g_activeWindowDispatchCalls ||
           g_activeWindowDispatchHookCallbacks) {
        DWORD remaining = INFINITE;
        if (timeoutMs != INFINITE) {
            ULONGLONG now = GetTickCount64();
            if (now >= deadline) {
                idle = false;
                break;
            }
            remaining = static_cast<DWORD>(std::min<ULONGLONG>(
                deadline - now, MAXDWORD));
        }
        if (!SleepConditionVariableSRW(
                &g_windowDispatchActivityChanged,
                &g_windowDispatchActivityLock, remaining, 0) &&
            GetLastError() == ERROR_TIMEOUT) {
            idle = false;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_windowDispatchActivityLock);
    return idle;
}

static bool RetryFailedWindowDispatchHooks() {
    std::vector<HHOOK> hooks;
    {
        std::lock_guard<std::mutex> lock(g_failedWindowDispatchHooksMtx);
        hooks.swap(g_failedWindowDispatchHooks);
    }

    std::vector<HHOOK> failedAgain;
    for (HHOOK hook : hooks) {
        DWORD error = ERROR_SUCCESS;
        if (!UnhookWindowDispatchHookConfirmed(hook, &error)) {
            failedAgain.push_back(hook);
        }
    }

    if (!failedAgain.empty()) {
        std::lock_guard<std::mutex> lock(g_failedWindowDispatchHooksMtx);
        g_failedWindowDispatchHooks.insert(
            g_failedWindowDispatchHooks.end(),
            failedAgain.begin(), failedAgain.end());
    }
    return failedAgain.empty();
}

static bool WaitForFailedWindowDispatchHooksRemoved(DWORD timeoutMs) {
    ULONGLONG deadline = timeoutMs == INFINITE
        ? 0
        : GetTickCount64() + timeoutMs;
    while (!RetryFailedWindowDispatchHooks()) {
        if (timeoutMs != INFINITE && GetTickCount64() >= deadline) {
            Wh_Log(
                L"Wh_ModUninit: timed out unregistering a "
                L"window-dispatch hook");
            return false;
        }

        MsgWaitForMultipleObjectsEx(
            0, nullptr, 10, QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
        MSG message{};
        while (PeekMessageW(
            &message, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    return true;
}

struct MediaState {
    std::wstring      title;
    std::wstring      artist;
    std::wstring      albumTitle;
    std::wstring      appUserModelId;
    bool              isPlaying     = false;
    bool              playbackStateKnown = false;
    bool              hasMedia      = false;
    int64_t           positionSeconds = 0;
    int64_t           durationSeconds = 0;
    ULONGLONG         timelineSampleTick = 0;
    std::vector<BYTE> thumbnailBytes;
    uint64_t          thumbnailHash = 0;
    std::vector<BYTE> appIconBytes;
    std::wstring      appIconKey;
    bool              canSkipPrevious  = true;
    bool              canSkipNext      = true;
    bool              canShuffle       = true;
    bool              canRepeat        = true;
    bool              canSeek          = true;
};

static MediaState g_media;
static std::mutex g_mediaMtx;

// Worker-event slots are read by several producer threads and closed by the
// settings/unload path. Keep signal/create/close atomic with respect to one
// another so a stale producer can never SetEvent on a closed, reused handle.
static std::mutex g_workerEventHandlesMtx;

static bool CreateWorkerEventHandle(
    HANDLE& slot, bool manualReset, bool initialState) {
    HANDLE created = CreateEventW(
        nullptr, manualReset ? TRUE : FALSE,
        initialState ? TRUE : FALSE, nullptr);
    if (!created) return false;

    std::lock_guard<std::mutex> lock(g_workerEventHandlesMtx);
    if (slot) {
        CloseHandle(created);
        return false;
    }
    slot = created;
    return true;
}

static HANDLE SnapshotWorkerEventHandle(HANDLE& slot) {
    std::lock_guard<std::mutex> lock(g_workerEventHandlesMtx);
    return slot;
}

static bool SignalWorkerEventHandle(HANDLE& slot) {
    std::lock_guard<std::mutex> lock(g_workerEventHandlesMtx);
    return slot && SetEvent(slot);
}

static bool IsWorkerEventSignaled(HANDLE& slot) {
    std::lock_guard<std::mutex> lock(g_workerEventHandlesMtx);
    return slot &&
           WaitForSingleObject(slot, 0) == WAIT_OBJECT_0;
}

static void CloseWorkerEventHandle(HANDLE& slot) {
    std::lock_guard<std::mutex> lock(g_workerEventHandlesMtx);
    HANDLE handle = slot;
    slot = nullptr;
    if (handle) CloseHandle(handle);
}

static void ReportOutstandingCallbackRisk(PCWSTR reason);

// Discord's desktop Rich Presence RPC requires a registered application ID.
// This is an original, minimal local IPC client: it publishes the current
// title, artist, play state, timeline, and a configured Rich Presence
// asset key. It never authenticates as the user or performs HTTP requests.
static HANDLE g_discordPresenceThread = nullptr;
static HANDLE g_discordPresenceStopEvent = nullptr;
static HANDLE g_discordPresenceUpdateEvent = nullptr;
static std::atomic<uint64_t> g_discordNonce{1};

struct DiscordIpcHeader {
    uint32_t opcode;
    uint32_t length;
};

static bool IsValidDiscordApplicationId(const std::wstring& value) {
    if (value.size() < 5 || value.size() > 32) return false;
    return std::all_of(value.begin(), value.end(), [](wchar_t c) {
        return c >= L'0' && c <= L'9';
    });
}

static std::string ToUtf8(const std::wstring& value) {
    try {
        return winrt::to_string(winrt::hstring(value));
    } catch (...) {
        return {};
    }
}

static std::string LimitUtf8(std::string value, size_t limit = 120) {
    if (value.size() <= limit) return value;
    size_t cut = limit;
    while (cut > 0 &&
           (static_cast<unsigned char>(value[cut]) & 0xC0) == 0x80) {
        --cut;
    }
    value.resize(cut);
    return value;
}

static std::string JsonEscapeUtf8(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size() + 16);
    static const char kHex[] = "0123456789abcdef";
    for (unsigned char c : value) {
        switch (c) {
            case '\"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < 0x20) {
                    escaped += "\\u00";
                    escaped += kHex[(c >> 4) & 0x0F];
                    escaped += kHex[c & 0x0F];
                } else {
                    escaped.push_back(static_cast<char>(c));
                }
                break;
        }
    }
    return escaped;
}

static void ReplaceAll(std::wstring& value, const std::wstring& marker,
                       const std::wstring& replacement) {
    if (marker.empty()) return;
    size_t position = 0;
    while ((position = value.find(marker, position)) != std::wstring::npos) {
        value.replace(position, marker.size(), replacement);
        position += replacement.size();
    }
}

static std::wstring ResolveDiscordActivityName(
    const std::wstring& nameTemplate, const MediaState& media) {
    std::wstring result = nameTemplate;
    ReplaceAll(result, L"{title}", media.title);
    ReplaceAll(result, L"{artist}", media.artist);
    ReplaceAll(result, L"{album}", media.albumTitle);
    ReplaceAll(result, L"{app}", media.appUserModelId);
    return result;
}

struct DiscordActivityTiming {
    bool valid = false;
    int64_t startEpochSeconds = 0;
    int64_t endEpochSeconds = 0;
};

static DiscordActivityTiming GetDiscordActivityTiming(
    const MediaState& media) {
    DiscordActivityTiming result;
    if (!media.isPlaying || media.durationSeconds <= 0) return result;

    int64_t position = std::clamp<int64_t>(
        media.positionSeconds, 0, media.durationSeconds);
    ULONGLONG nowTick = GetTickCount64();
    if (media.timelineSampleTick && nowTick > media.timelineSampleTick) {
        position = std::min<int64_t>(
            media.durationSeconds,
            position + static_cast<int64_t>(
                (nowTick - media.timelineSampleTick) / 1000));
    }

    int64_t nowEpoch = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    result.valid = true;
    result.startEpochSeconds = nowEpoch - position;
    result.endEpochSeconds = result.startEpochSeconds + media.durationSeconds;
    return result;
}

static bool DiscordPipeTransfer(HANDLE pipe, bool write, void* buffer,
                                DWORD size, DWORD timeoutMs) {
    if (pipe == INVALID_HANDLE_VALUE || (!buffer && size)) return false;

    BYTE* cursor = static_cast<BYTE*>(buffer);
    const ULONGLONG deadline = timeoutMs == INFINITE
        ? 0
        : GetTickCount64() + timeoutMs;
    HANDLE stopEvent =
        SnapshotWorkerEventHandle(g_discordPresenceStopEvent);

    while (size > 0) {
        OVERLAPPED overlapped{};
        overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!overlapped.hEvent) return false;

        DWORD transferred = 0;
        BOOL completed = write
            ? WriteFile(pipe, cursor, size, &transferred, &overlapped)
            : ReadFile(pipe, cursor, size, &transferred, &overlapped);
        bool success = completed != FALSE;

        if (!success && GetLastError() == ERROR_IO_PENDING) {
            DWORD remaining = INFINITE;
            if (timeoutMs != INFINITE) {
                ULONGLONG now = GetTickCount64();
                remaining = now < deadline
                    ? static_cast<DWORD>(std::min<ULONGLONG>(
                          deadline - now, MAXDWORD))
                    : 0;
            }

            HANDLE waits[] = {overlapped.hEvent, stopEvent};
            DWORD waitCount = stopEvent ? 2 : 1;
            DWORD waitResult = remaining
                ? WaitForMultipleObjects(
                      waitCount, waits, FALSE, remaining)
                : WAIT_TIMEOUT;
            if (waitResult == WAIT_OBJECT_0) {
                success = GetOverlappedResult(
                    pipe, &overlapped, &transferred, FALSE) != FALSE;
            } else {
                CancelIoEx(pipe, &overlapped);
                GetOverlappedResult(
                    pipe, &overlapped, &transferred, TRUE);
                success = false;
            }
        }

        CloseHandle(overlapped.hEvent);
        if (!success || !transferred) return false;

        cursor += transferred;
        size -= transferred;
    }
    return true;
}

static bool DiscordPipeWriteAll(HANDLE pipe, const void* data, DWORD size,
                                DWORD timeoutMs) {
    return DiscordPipeTransfer(
        pipe, true, const_cast<void*>(data), size, timeoutMs);
}

static bool DiscordPipeReadAll(HANDLE pipe, void* data, DWORD size,
                               DWORD timeoutMs) {
    return DiscordPipeTransfer(pipe, false, data, size, timeoutMs);
}

static bool DiscordPipeWriteFrame(HANDLE pipe, uint32_t opcode,
                                  const std::string& payload) {
    if (payload.size() > 1024 * 1024) return false;
    DiscordIpcHeader header{opcode, static_cast<uint32_t>(payload.size())};
    return DiscordPipeWriteAll(pipe, &header, sizeof(header), 1000) &&
           DiscordPipeWriteAll(
               pipe, payload.data(), header.length, 1000);
}

static bool DiscordPipeReadFrame(HANDLE pipe, uint32_t& opcode,
                                 std::string& payload, DWORD timeoutMs) {
    DiscordIpcHeader header{};
    if (!DiscordPipeReadAll(pipe, &header, sizeof(header), timeoutMs) ||
        header.length > 1024 * 1024) {
        return false;
    }
    payload.assign(header.length, '\0');
    if (header.length &&
        !DiscordPipeReadAll(pipe, payload.data(), header.length, timeoutMs)) {
        return false;
    }
    opcode = header.opcode;
    return true;
}

static HANDLE ConnectDiscordPresencePipe(const std::wstring& applicationId) {
    HANDLE pipe = INVALID_HANDLE_VALUE;
    for (int i = 0; i < 10 && pipe == INVALID_HANDLE_VALUE; ++i) {
        std::wstring path = L"\\\\.\\pipe\\discord-ipc-" +
                            std::to_wstring(i);
        pipe = CreateFileW(
            path.c_str(), GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
    }
    if (pipe == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;

    std::string handshake = "{\"v\":1,\"client_id\":\"" +
        JsonEscapeUtf8(ToUtf8(applicationId)) + "\"}";
    if (!DiscordPipeWriteFrame(pipe, 0, handshake)) {
        CloseHandle(pipe);
        return INVALID_HANDLE_VALUE;
    }

    for (int attempt = 0; attempt < 4; ++attempt) {
        uint32_t opcode = 0;
        std::string response;
        if (!DiscordPipeReadFrame(pipe, opcode, response, 1200)) break;
        if (opcode == 3) {
            if (!DiscordPipeWriteFrame(pipe, 4, response)) break;
            continue;
        }
        if (opcode == 1 &&
            response.find("\"evt\":\"READY\"") != std::string::npos) {
            Wh_Log(L"Discord presence: connected");
            return pipe;
        }
        if (opcode == 2 || response.find("ERROR") != std::string::npos) break;
    }

    CloseHandle(pipe);
    return INVALID_HANDLE_VALUE;
}

static std::string MakeDiscordActivityCommand(
    const MediaState& media, bool publish,
    const DiscordActivityTiming& timing,
    const std::wstring& largeImage,
    const std::wstring& activityName) {
    uint64_t nonce = g_discordNonce.fetch_add(1);
    std::string command = "{\"cmd\":\"SET_ACTIVITY\",\"args\":{";
    command += "\"pid\":" + std::to_string(GetCurrentProcessId()) +
               ",\"activity\":";

    if (!publish) {
        command += "null";
    } else {
        std::wstring detailText =
            activityName.empty() ? media.title : activityName;
        std::string title =
            JsonEscapeUtf8(LimitUtf8(ToUtf8(detailText)));
        std::wstring stateText = media.artist.empty()
            ? L"Unknown artist"
            : media.artist;
        if (!media.isPlaying) stateText = L"Paused - " + stateText;
        std::string state = JsonEscapeUtf8(LimitUtf8(ToUtf8(stateText)));
        command += "{\"type\":2";
        command += ",\"details\":\"" + title +
                   "\",\"state\":\"" + state + "\"";
        if (timing.valid) {
            command += ",\"timestamps\":{\"start\":" +
                std::to_string(timing.startEpochSeconds) +
                ",\"end\":" + std::to_string(timing.endEpochSeconds) + "}";
        }
        if (!largeImage.empty()) {
            command += ",\"assets\":{\"large_image\":\"" +
                JsonEscapeUtf8(LimitUtf8(ToUtf8(largeImage), 512)) +
                "\"}";
        }
        command += ",\"instance\":true}";
    }

    command += "},\"nonce\":\"" + std::to_string(nonce) + "\"}";
    return command;
}

static bool SendDiscordActivity(HANDLE pipe, const MediaState& media,
                                bool publish,
                                const DiscordActivityTiming& timing,
                                const std::wstring& largeImage,
                                const std::wstring& activityName) {
    std::string command = MakeDiscordActivityCommand(
        media, publish, timing, largeImage, activityName);
    if (!DiscordPipeWriteFrame(pipe, 1, command)) return false;

    for (int attempt = 0; attempt < 4; ++attempt) {
        uint32_t opcode = 0;
        std::string response;
        if (!DiscordPipeReadFrame(pipe, opcode, response, 1000)) return false;
        if (opcode == 3) {
            if (!DiscordPipeWriteFrame(pipe, 4, response)) return false;
            continue;
        }
        return opcode == 1 &&
               response.find("\"evt\":\"ERROR\"") == std::string::npos;
    }
    return false;
}

static DWORD DiscordPresenceThreadMain() {
    const auto cfg = Cfg();
    const std::wstring applicationId = cfg->discordApplicationId;
    const bool showPaused = cfg->discordShowPaused;
    const std::wstring activityNameTemplate = cfg->discordActivityName;
    const std::wstring fallbackImageKey = cfg->discordLargeImageKey;
    HANDLE pipe = INVALID_HANDLE_VALUE;
    std::wstring lastStateKey;
    bool published = false;
    struct DiscordPipeCleanup {
        HANDLE& pipe;
        bool& published;
        ~DiscordPipeCleanup() noexcept {
            if (pipe == INVALID_HANDLE_VALUE) return;
            if (published) {
                try {
                    MediaState empty;
                    SendDiscordActivity(
                        pipe, empty, false, DiscordActivityTiming{}, L"", L"");
                } catch (...) {}
            }
            CloseHandle(pipe);
            pipe = INVALID_HANDLE_VALUE;
        }
    } pipeCleanup{pipe, published};
    HANDLE stopEvent =
        SnapshotWorkerEventHandle(g_discordPresenceStopEvent);
    HANDLE updateEvent =
        SnapshotWorkerEventHandle(g_discordPresenceUpdateEvent);
    if (!stopEvent || !updateEvent) return 0;

    while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {
        MediaState media;
        {
            std::lock_guard<std::mutex> lock(g_mediaMtx);
            media = g_media;
        }
        bool shouldPublish = media.hasMedia && !media.title.empty() &&
                             (media.isPlaying || showPaused);
        const std::wstring& largeImage = fallbackImageKey;
        std::wstring activityName = ResolveDiscordActivityName(
            activityNameTemplate, media);
        DiscordActivityTiming timing = GetDiscordActivityTiming(media);
        auto quantizeTimestamp = [](int64_t value) {
            constexpr int64_t kQuantumSeconds = 5;
            return value - value % kQuantumSeconds;
        };
        std::wstring stateKey = shouldPublish
            ? media.title + L"\n" + media.artist + L"\n" + media.albumTitle +
                L"\n" + (media.isPlaying ? L"playing" : L"paused") +
                L"\n" + largeImage + L"\n" + activityName + L"\n" +
                (timing.valid
                    ? std::to_wstring(
                          quantizeTimestamp(timing.startEpochSeconds)) +
                        L":" +
                        std::to_wstring(
                          quantizeTimestamp(timing.endEpochSeconds))
                    : L"no-timeline")
            : L"<clear>";

        if (pipe == INVALID_HANDLE_VALUE) {
            pipe = ConnectDiscordPresencePipe(applicationId);
            lastStateKey.clear();
        }

        if (pipe != INVALID_HANDLE_VALUE && stateKey != lastStateKey) {
            if (SendDiscordActivity(pipe, media, shouldPublish,
                                    timing, largeImage, activityName)) {
                lastStateKey = stateKey;
                published = shouldPublish;
                Wh_Log(L"Discord presence: %ls",
                       shouldPublish ? L"activity updated" : L"activity cleared");
            } else {
                CloseHandle(pipe);
                pipe = INVALID_HANDLE_VALUE;
                lastStateKey.clear();
            }
        }

        HANDLE waits[] = {stopEvent, updateEvent};
        DWORD wait = WaitForMultipleObjects(2, waits, FALSE, 5000);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_TIMEOUT && pipe != INVALID_HANDLE_VALUE) {
            DWORD available = 0;
            if (!PeekNamedPipe(pipe, nullptr, 0, nullptr, &available, nullptr)) {
                CloseHandle(pipe);
                pipe = INVALID_HANDLE_VALUE;
                lastStateKey.clear();
            }
        }
    }

    if (pipe != INVALID_HANDLE_VALUE) {
        if (published) {
            MediaState empty;
            SendDiscordActivity(pipe, empty, false,
                                DiscordActivityTiming{}, L"", L"");
        }
        CloseHandle(pipe);
        pipe = INVALID_HANDLE_VALUE;
    }
    return 0;
}

static DWORD WINAPI DiscordPresenceThreadProc(void*) noexcept {
    try {
        return DiscordPresenceThreadMain();
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Discord presence worker failed: 0x%08X",
               static_cast<uint32_t>(error.code()));
    } catch (...) {
        Wh_Log(L"Discord presence worker failed with an unexpected exception");
    }
    return 0;
}

static void SignalDiscordPresenceUpdate() {
    SignalWorkerEventHandle(g_discordPresenceUpdateEvent);
}

static bool ReapStoppedDiscordPresenceThread() {
    if (!g_discordPresenceThread) return true;
    if (WaitForSingleObject(g_discordPresenceThread, 0) != WAIT_OBJECT_0) {
        return false;
    }

    CloseHandle(g_discordPresenceThread);
    g_discordPresenceThread = nullptr;
    CloseWorkerEventHandle(g_discordPresenceStopEvent);
    CloseWorkerEventHandle(g_discordPresenceUpdateEvent);
    return true;
}

static void StartDiscordPresenceThread() {
    if (!ReapStoppedDiscordPresenceThread()) {
        Wh_Log(L"Discord presence: previous worker is still stopping");
        return;
    }
    const auto cfg = Cfg();
    if (!cfg->discordPresenceEnabled) return;
    if (!IsValidDiscordApplicationId(cfg->discordApplicationId)) {
        if (!cfg->discordApplicationId.empty()) {
            Wh_Log(L"Discord presence: Application ID must contain only digits");
        } else {
            Wh_Log(L"Discord presence: waiting for an Application ID in settings");
        }
        return;
    }

    bool stopEventCreated = CreateWorkerEventHandle(
        g_discordPresenceStopEvent, true, false);
    bool updateEventCreated = CreateWorkerEventHandle(
        g_discordPresenceUpdateEvent, false, true);
    if (!stopEventCreated || !updateEventCreated) {
        Wh_Log(L"Discord presence: failed to create worker events");
        CloseWorkerEventHandle(g_discordPresenceStopEvent);
        CloseWorkerEventHandle(g_discordPresenceUpdateEvent);
        return;
    }
    g_discordPresenceThread = CreateThread(
        nullptr, 0, DiscordPresenceThreadProc, nullptr, 0, nullptr);
    if (!g_discordPresenceThread) {
        CloseWorkerEventHandle(g_discordPresenceStopEvent);
        CloseWorkerEventHandle(g_discordPresenceUpdateEvent);
    }
}

static bool StopDiscordPresenceThread(bool shutdownCleanup = false) {
    SignalWorkerEventHandle(g_discordPresenceStopEvent);
    if (g_discordPresenceThread) {
        constexpr DWORD timeoutMs = 3000;
        bool stopped =
            WaitForThreadExit(g_discordPresenceThread, timeoutMs);
        if (!stopped) {
            if (shutdownCleanup) {
                Wh_Log(
                    L"Discord presence: worker exceeded the unload deadline; "
                    L"retaining the module if it remains active");
            } else {
                Wh_Log(
                    L"Discord presence: worker didn't stop within the "
                    L"settings-change deadline");
            }
            return false;
        }
    }
    return ReapStoppedDiscordPresenceThread();
}

enum class RepeatMode {
    Off = 0,
    All = 1,
    One = 2,
};

static std::atomic<bool> g_shuffleEnabled{false};
static std::atomic<RepeatMode> g_repeatMode{RepeatMode::Off};

static std::atomic<int> g_cachedAppIconSize{-1};


[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSessionManager g_sessionMgr     = nullptr;
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSession        g_currentSession = nullptr;
static std::mutex  g_sessionMtx;
static bool g_userSwitchedSession = false;
static std::atomic<bool> g_forceSessionRefresh{false};
static std::atomic<uint64_t> g_sessionGeneration{0};
static std::atomic<ULONGLONG> g_metadataRetryUntilTick{0};
static std::atomic<ULONGLONG> g_mediaTransitionUntilTick{0};
static std::atomic<ULONGLONG> g_currentSessionLastPlayingTick{0};

struct PendingSeekState {
    bool active = false;
    uint64_t requestId = 0;
    uint64_t generation = 0;
    int64_t targetSeconds = 0;
    int64_t durationSeconds = 0;
    ULONGLONG requestedTick = 0;
    std::wstring title;
    std::wstring artist;
    uint64_t thumbnailHash = 0;
};

static std::mutex g_pendingSeekMtx;
static PendingSeekState g_pendingSeek;
static std::atomic<uint64_t> g_nextSeekRequestId{1};
static constexpr DWORD kPendingSeekHoldMs = 3500;
static std::atomic<bool> g_mediaPropertiesFetchActive{false};
static std::atomic<bool> g_playbackInfoFetchActive{false};
static std::atomic<bool> g_mediaPropertiesFetchPending{false};
static std::atomic<bool> g_playbackInfoFetchPending{false};

static winrt::event_token g_evSessionsChanged{};
static winrt::event_token g_evCurrentChanged{};
static winrt::event_token g_evMediaProps{};
static winrt::event_token g_evPlayback{};
static winrt::event_token g_evTimeline{};
static std::atomic<bool> g_mediaEventUnsubscribeFailed{false};

static void RecordMediaEventUnsubscribeFailure(PCWSTR source) noexcept {
    bool firstFailure = !g_mediaEventUnsubscribeFailed.exchange(
        true, std::memory_order_acq_rel);
    if (firstFailure) {
        Wh_Log(L"Media event cleanup failed for %ls",
               source ? source : L"an unknown source");
    }
    ReportOutstandingCallbackRisk(
        source ? source : L"failed media event unsubscription");
}

static HANDLE g_mediaThread    = nullptr;
static std::atomic<DWORD> g_mediaThreadId{0};
static HANDLE g_mediaStopEvent = nullptr;
static HANDLE g_mediaRefreshEvent = nullptr;

static SRWLOCK g_mediaEventCallbackLock = SRWLOCK_INIT;
static CONDITION_VARIABLE g_mediaEventCallbackChanged =
    CONDITION_VARIABLE_INIT;
static LONG g_activeMediaEventCallbacks = 0;

class MediaEventCallbackGuard {
public:
    MediaEventCallbackGuard() {
        AcquireSRWLockExclusive(&g_mediaEventCallbackLock);
        ++g_activeMediaEventCallbacks;
        ReleaseSRWLockExclusive(&g_mediaEventCallbackLock);
    }

    ~MediaEventCallbackGuard() {
        AcquireSRWLockExclusive(&g_mediaEventCallbackLock);
        --g_activeMediaEventCallbacks;
        WakeAllConditionVariable(&g_mediaEventCallbackChanged);
        ReleaseSRWLockExclusive(&g_mediaEventCallbackLock);
    }
};

static bool WaitForMediaEventCallbacksIdle(DWORD timeoutMs = INFINITE) {
    ULONGLONG deadline = timeoutMs == INFINITE
        ? 0
        : GetTickCount64() + timeoutMs;
    bool idle = true;

    AcquireSRWLockExclusive(&g_mediaEventCallbackLock);
    while (g_activeMediaEventCallbacks) {
        DWORD remaining = INFINITE;
        if (timeoutMs != INFINITE) {
            ULONGLONG now = GetTickCount64();
            if (now >= deadline) {
                idle = false;
                break;
            }
            remaining = static_cast<DWORD>(std::min<ULONGLONG>(
                deadline - now, MAXDWORD));
        }
        if (!SleepConditionVariableSRW(
                &g_mediaEventCallbackChanged, &g_mediaEventCallbackLock,
                remaining, 0) &&
            GetLastError() == ERROR_TIMEOUT) {
            idle = false;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_mediaEventCallbackLock);
    return idle;
}

static void RequestMediaSessionRefresh() {
    SignalWorkerEventHandle(g_mediaRefreshEvent);
}

static void ArmMetadataRetry(DWORD durationMs = 15000) {
    g_metadataRetryUntilTick = GetTickCount64() + durationMs;
}

static void ArmMediaVisualTransition(DWORD durationMs = 0) {
    if (!durationMs) {
        durationMs = static_cast<DWORD>(
            std::max(0, Cfg()->mediaTransitionGraceMs));
    }
    if (!durationMs) {
        g_mediaTransitionUntilTick = 0;
        return;
    }

    ULONGLONG deadline = GetTickCount64() + durationMs;
    ULONGLONG current = g_mediaTransitionUntilTick.load(
        std::memory_order_acquire);
    while (current < deadline &&
           !g_mediaTransitionUntilTick.compare_exchange_weak(
               current, deadline, std::memory_order_acq_rel,
               std::memory_order_acquire)) {
    }
}

static bool MediaVisualTransitionActive() {
    ULONGLONG deadline = g_mediaTransitionUntilTick.load(
        std::memory_order_acquire);
    return deadline && GetTickCount64() < deadline;
}

static void ClearPendingSeek() {
    std::lock_guard<std::mutex> lock(g_pendingSeekMtx);
    g_pendingSeek = {};
}

static void ClearPendingSeekIfRequest(uint64_t requestId) {
    if (!requestId) return;
    std::lock_guard<std::mutex> lock(g_pendingSeekMtx);
    if (g_pendingSeek.active && g_pendingSeek.requestId == requestId) {
        g_pendingSeek = {};
    }
}

static uint64_t BeginPendingSeek(
    int64_t targetSeconds,
    uint64_t generation,
    const std::wstring& title,
    const std::wstring& artist,
    uint64_t thumbnailHash,
    int64_t durationSeconds) {
    uint64_t requestId = g_nextSeekRequestId.fetch_add(
        1, std::memory_order_relaxed);
    if (!requestId) {
        requestId = g_nextSeekRequestId.fetch_add(
            1, std::memory_order_relaxed);
    }

    std::lock_guard<std::mutex> lock(g_pendingSeekMtx);
    g_pendingSeek.active = true;
    g_pendingSeek.requestId = requestId;
    g_pendingSeek.generation = generation;
    g_pendingSeek.targetSeconds = targetSeconds;
    g_pendingSeek.durationSeconds = durationSeconds;
    g_pendingSeek.requestedTick = GetTickCount64();
    g_pendingSeek.title = title;
    g_pendingSeek.artist = artist;
    g_pendingSeek.thumbnailHash = thumbnailHash;
    return requestId;
}

static bool ShouldIgnoreTimelineSampleAfterSeek(
    uint64_t generation,
    int64_t reportedPositionSeconds,
    int64_t reportedDurationSeconds) {
    PendingSeekState pending;
    {
        std::lock_guard<std::mutex> lock(g_pendingSeekMtx);
        if (!g_pendingSeek.active) return false;
        pending = g_pendingSeek;
    }

    if (pending.generation != generation) {
        ClearPendingSeekIfRequest(pending.requestId);
        return false;
    }

    bool identityMatches = false;
    {
        std::lock_guard<std::mutex> lock(g_mediaMtx);
        int64_t durationDifference =
            g_media.durationSeconds > pending.durationSeconds
                ? g_media.durationSeconds - pending.durationSeconds
                : pending.durationSeconds - g_media.durationSeconds;
        identityMatches =
            g_media.title == pending.title &&
            g_media.artist == pending.artist &&
            durationDifference <= 2 &&
            (!pending.thumbnailHash || !g_media.thumbnailHash ||
             g_media.thumbnailHash == pending.thumbnailHash);
    }
    if (!identityMatches) {
        ClearPendingSeekIfRequest(pending.requestId);
        return false;
    }

    int64_t durationDifference =
        reportedDurationSeconds > pending.durationSeconds
            ? reportedDurationSeconds - pending.durationSeconds
            : pending.durationSeconds - reportedDurationSeconds;
    if (durationDifference > 2) {
        ClearPendingSeekIfRequest(pending.requestId);
        return false;
    }

    int64_t positionDifference =
        reportedPositionSeconds > pending.targetSeconds
            ? reportedPositionSeconds - pending.targetSeconds
            : pending.targetSeconds - reportedPositionSeconds;
    if (positionDifference <= 2) {
        // The player has acknowledged the new position. Future samples are
        // authoritative again.
        ClearPendingSeekIfRequest(pending.requestId);
        return false;
    }

    ULONGLONG nowTick = GetTickCount64();
    if (nowTick >= pending.requestedTick &&
        nowTick - pending.requestedTick < kPendingSeekHoldMs) {
        // Some players emit one or more old timeline events after accepting a
        // seek. Ignore those stale samples so the bar never snaps backward.
        return true;
    }

    ClearPendingSeekIfRequest(pending.requestId);
    return false;
}

static bool IsSystemLightTheme() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS) {
        return v != 0;
    }
    v = 0; sz = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_DWORD, nullptr, &v, &sz);
    return v != 0;
}

static SolidColorBrush MakeBrush(winrt::Windows::UI::Color c) {
    SolidColorBrush b; b.Color(c); return b;
}

static bool IsHoverEffectEnabled(std::wstring const& mode) {
    return mode != L"off";
}

static bool IsHoverLightTheme(std::wstring const& mode) {
    if (mode == L"white") return true;
    if (mode == L"black") return false;
    return IsSystemLightTheme();
}

static winrt::Windows::UI::Color GetSystemButtonHoverColor(std::wstring const& mode) {
    if (IsHoverLightTheme(mode)) {
        return winrt::Windows::UI::Color{0x99, 0xFF, 0xFF, 0xFF};
    }
    return winrt::Windows::UI::Color{0x0F, 0xFF, 0xFF, 0xFF};
}

static winrt::Windows::UI::Color GetSystemButtonPressedColor(std::wstring const& mode) {
    if (IsHoverLightTheme(mode)) {
        return winrt::Windows::UI::Color{0x4D, 0xFF, 0xFF, 0xFF};
    }
    return winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF};
}

static winrt::Windows::UI::Color GetSystemButtonBorderPressedColor(std::wstring const& mode) {
    if (IsHoverLightTheme(mode)) {
        return winrt::Windows::UI::Color{0x05, 0x00, 0x00, 0x00};
    }
    return winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF};
}

static Style CreateFluentMediaButtonStyle() {
    static const wchar_t kStyleXaml[] = LR"(<Style TargetType="Button"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">
  <Setter Property="Background" Value="Transparent"/>
  <Setter Property="BorderBrush" Value="Transparent"/>
  <Setter Property="BorderThickness" Value="0"/>
  <Setter Property="UseSystemFocusVisuals" Value="False"/>
  <Setter Property="Template">
    <Setter.Value>
      <ControlTemplate TargetType="Button">
        <Border x:Name="Root"
                Background="{TemplateBinding Background}"
                BorderBrush="{TemplateBinding BorderBrush}"
                BorderThickness="{TemplateBinding BorderThickness}"
                CornerRadius="{TemplateBinding CornerRadius}"
                Padding="{TemplateBinding Padding}">
          <ContentPresenter Content="{TemplateBinding Content}"
                            ContentTransitions="{TemplateBinding ContentTransitions}"
                            ContentTemplate="{TemplateBinding ContentTemplate}"
                            HorizontalContentAlignment="{TemplateBinding HorizontalContentAlignment}"
                            VerticalContentAlignment="{TemplateBinding VerticalContentAlignment}"/>
        </Border>
      </ControlTemplate>
    </Setter.Value>
  </Setter>
</Style>)";

    try {
        return winrt::Windows::UI::Xaml::Markup::XamlReader::Load(
            winrt::hstring(kStyleXaml)).as<Style>();
    } catch (...) {
        return nullptr;
    }
}

static Style GetFluentMediaButtonStyle() {
    return CreateFluentMediaButtonStyle();
}

static void ApplyFluentMediaButtonStyle(Button const& btn) {
    if (auto style = GetFluentMediaButtonStyle()) {
        btn.Style(style);
    }
}

static void SetButtonChrome(Button const& btn,
                            Brush const& background,
                            Brush const& borderBrush) {
    if (!btn) return;
    try {
        btn.Background(background);
        btn.BorderBrush(borderBrush);
    } catch (...) {}
}

static void ApplyMediaButtonState(Button const& btn,
                                  bool hovered,
                                  bool pressed) {
    if (!btn) return;
    auto transparent = MakeBrush({0x00, 0, 0, 0});
    Brush background = transparent;
    if (IsHoverEffectEnabled(Cfg()->mediaButtonsHoverEffectMode)) {
        if (pressed) {
            background = MakeBrush(GetSystemButtonPressedColor(
                Cfg()->mediaButtonsHoverEffectMode));
        } else if (hovered) {
            background = MakeBrush(GetSystemButtonHoverColor(
                Cfg()->mediaButtonsHoverEffectMode));
        }
    }
    SetButtonChrome(btn, background, transparent);
}

static Brush MakeElevationBorderBrush(std::wstring const& mode) {
    bool light = IsHoverLightTheme(mode);
    winrt::Windows::UI::Color topColor, bottomColor;
    if (light) {
        topColor    = winrt::Windows::UI::Color{0x08, 0x00, 0x00, 0x00};
        bottomColor = winrt::Windows::UI::Color{0x10, 0x00, 0x00, 0x00};
    } else {
        topColor    = winrt::Windows::UI::Color{0x28, 0xFF, 0xFF, 0xFF};
        bottomColor = winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF};
    }
    try {
        winrt::Windows::UI::Xaml::Media::LinearGradientBrush brush;
        brush.StartPoint(winrt::Windows::Foundation::Point(0.5f, 0.0f));
        brush.EndPoint(winrt::Windows::Foundation::Point(0.5f, 1.0f));
        winrt::Windows::UI::Xaml::Media::GradientStop s1, s2;
        s1.Color(topColor);    s1.Offset(0.0);
        s2.Color(bottomColor); s2.Offset(1.0);
        brush.GradientStops().Append(s1);
        brush.GradientStops().Append(s2);
        return brush;
    } catch (...) {
        return MakeBrush(topColor);
    }
}

static void ApplyPlayerButtonState(Button const& btn,
                                   Brush const& normalBackground,
                                   bool hovered,
                                   bool pressed) {
    if (!btn) return;
    auto transparent = MakeBrush({0x00, 0, 0, 0});
    Brush background = normalBackground;
    Brush border = transparent;
    if (IsHoverEffectEnabled(Cfg()->playerHoverEffectMode)) {
        if (pressed) {
            background = MakeBrush(GetSystemButtonPressedColor(
                Cfg()->playerHoverEffectMode));
            border = MakeBrush(GetSystemButtonBorderPressedColor(
                Cfg()->playerHoverEffectMode));
        } else if (hovered) {
            background = MakeBrush(GetSystemButtonHoverColor(
                Cfg()->playerHoverEffectMode));
            border = MakeElevationBorderBrush(
                Cfg()->playerHoverEffectMode);
        }
    }
    SetButtonChrome(btn, background, border);
}

static bool DecodeImageToBGRA(const std::vector<BYTE>& imgBytes,
                               std::vector<BYTE>& outPixels,
                               int& outW, int& outH)
{
    if (imgBytes.empty() || imgBytes.size() > UINT_MAX) return false;
    IWICImagingFactory* pFactory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&pFactory))) || !pFactory)
        return false;
    IStream* pStream = SHCreateMemStream(imgBytes.data(), (UINT)imgBytes.size());
    if (!pStream) { pFactory->Release(); return false; }
    bool ok = false;
    IWICBitmapDecoder* pDecoder = nullptr;
    if (SUCCEEDED(pFactory->CreateDecoderFromStream(
            pStream, nullptr, WICDecodeMetadataCacheOnDemand, &pDecoder))) {
        IWICBitmapFrameDecode* pFrame = nullptr;
        if (SUCCEEDED(pDecoder->GetFrame(0, &pFrame))) {
            UINT sourceWidth = 0;
            UINT sourceHeight = 0;
            IWICBitmapScaler* pScaler = nullptr;
            IWICBitmapSource* source = pFrame;
            if (SUCCEEDED(pFrame->GetSize(
                    &sourceWidth, &sourceHeight)) &&
                sourceWidth > 0 && sourceHeight > 0) {
                constexpr UINT kDecodeLimit = 512;
                if (sourceWidth > kDecodeLimit ||
                    sourceHeight > kDecodeLimit) {
                    double scale = std::min(
                        static_cast<double>(kDecodeLimit) / sourceWidth,
                        static_cast<double>(kDecodeLimit) / sourceHeight);
                    UINT scaledWidth = std::max<UINT>(
                        1, static_cast<UINT>(
                               std::lround(sourceWidth * scale)));
                    UINT scaledHeight = std::max<UINT>(
                        1, static_cast<UINT>(
                               std::lround(sourceHeight * scale)));
                    if (SUCCEEDED(pFactory->CreateBitmapScaler(&pScaler)) &&
                        SUCCEEDED(pScaler->Initialize(
                            pFrame, scaledWidth, scaledHeight,
                            WICBitmapInterpolationModeFant))) {
                        source = pScaler;
                    } else if (pScaler) {
                        pScaler->Release();
                        pScaler = nullptr;
                    }
                }
            }

            IWICFormatConverter* pConv = nullptr;
            if (sourceWidth > 0 && sourceHeight > 0 &&
                SUCCEEDED(pFactory->CreateFormatConverter(&pConv))) {
                if (SUCCEEDED(pConv->Initialize(
                        source, GUID_WICPixelFormat32bppBGRA,
                        WICBitmapDitherTypeNone, nullptr, 0.0,
                        WICBitmapPaletteTypeMedianCut))) {
                    UINT w = 0, h = 0;
                    if (SUCCEEDED(pConv->GetSize(&w, &h)) &&
                        w > 0 && h > 0 &&
                        w <= 512 && h <= 512 &&
                        static_cast<uint64_t>(w) * h <= 512ull * 512ull) {
                        size_t byteCount =
                            static_cast<size_t>(w) * h * 4;
                        outPixels.resize(byteCount);
                        if (SUCCEEDED(pConv->CopyPixels(nullptr, w * 4,
                                static_cast<UINT>(byteCount),
                                outPixels.data()))) {
                            outW = (int)w; outH = (int)h; ok = true;
                        }
                    }
                }
                pConv->Release();
            }
            if (pScaler) pScaler->Release();
            pFrame->Release();
        }
        pDecoder->Release();
    }
    pStream->Release();
    pFactory->Release();
    return ok;
}

static bool GetEncodedImageDimensions(
    const std::vector<BYTE>& imageBytes, int& outWidth, int& outHeight) {
    outWidth = 0;
    outHeight = 0;
    if (imageBytes.empty() || imageBytes.size() > UINT_MAX) return false;

    winrt::com_ptr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(
            CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory), factory.put_void()))) {
        return false;
    }

    IStream* rawStream = SHCreateMemStream(
        imageBytes.data(), static_cast<UINT>(imageBytes.size()));
    if (!rawStream) return false;
    winrt::com_ptr<IStream> stream;
    stream.attach(rawStream);

    winrt::com_ptr<IWICBitmapDecoder> decoder;
    if (FAILED(factory->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnDemand,
            decoder.put()))) {
        return false;
    }

    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    if (FAILED(decoder->GetFrame(0, frame.put()))) return false;

    UINT width = 0;
    UINT height = 0;
    if (FAILED(frame->GetSize(&width, &height)) ||
        width == 0 || height == 0 ||
        width > static_cast<UINT>(std::numeric_limits<int>::max()) ||
        height > static_cast<UINT>(std::numeric_limits<int>::max())) {
        return false;
    }

    outWidth = static_cast<int>(width);
    outHeight = static_cast<int>(height);
    return true;
}

static void DownsampleBGRA(const std::vector<BYTE>& src, int srcW, int srcH,
                            std::vector<BYTE>& dst, int dstW, int dstH)
{
    dst.resize((size_t)dstW * dstH * 4);
    float xr = (float)srcW / dstW, yr = (float)srcH / dstH;
    for (int dy = 0; dy < dstH; ++dy) {
        for (int dx = 0; dx < dstW; ++dx) {
            float sx = (dx + 0.5f) * xr - 0.5f;
            float sy = (dy + 0.5f) * yr - 0.5f;
            int x0 = (int)sx; if (x0 < 0) x0 = 0;
            int y0 = (int)sy; if (y0 < 0) y0 = 0;
            int x1 = x0 + 1; if (x1 >= srcW) x1 = srcW - 1;
            int y1 = y0 + 1; if (y1 >= srcH) y1 = srcH - 1;
            float fx = sx - (float)x0; if (fx < 0) fx = 0;
            float fy = sy - (float)y0; if (fy < 0) fy = 0;
            const BYTE* p00 = &src[((size_t)y0 * srcW + x0) * 4];
            const BYTE* p10 = &src[((size_t)y0 * srcW + x1) * 4];
            const BYTE* p01 = &src[((size_t)y1 * srcW + x0) * 4];
            const BYTE* p11 = &src[((size_t)y1 * srcW + x1) * 4];
            BYTE* d = &dst[((size_t)dy * dstW + dx) * 4];
            for (int c = 0; c < 4; ++c) {
                float v = p00[c]*(1-fx)*(1-fy) + p10[c]*fx*(1-fy)
                        + p01[c]*(1-fx)*fy     + p11[c]*fx*fy;
                d[c] = (BYTE)(v < 0 ? 0 : v > 255 ? 255 : (int)v);
            }
        }
    }
}

static void ApplyBoxBlurBGRA(std::vector<BYTE>& pixels, int w, int h, int radius)
{
    if (radius < 1 || w < 1 || h < 1) return;
    std::vector<BYTE> temp(pixels.size());

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int r = 0, g = 0, b = 0, a = 0, count = 0;
            for (int dx = -radius; dx <= radius; ++dx) {
                int sx = x + dx;
                if (sx >= 0 && sx < w) {
                    const BYTE* p = &pixels[((size_t)y * w + sx) * 4];
                    b += p[0]; g += p[1]; r += p[2]; a += p[3];
                    count++;
                }
            }
            BYTE* d = &temp[((size_t)y * w + x) * 4];
            d[0] = (BYTE)(b / count);
            d[1] = (BYTE)(g / count);
            d[2] = (BYTE)(r / count);
            d[3] = (BYTE)(a / count);
        }
    }

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int r = 0, g = 0, b = 0, a = 0, count = 0;
            for (int dy = -radius; dy <= radius; ++dy) {
                int sy = y + dy;
                if (sy >= 0 && sy < h) {
                    const BYTE* p = &temp[((size_t)sy * w + x) * 4];
                    b += p[0]; g += p[1]; r += p[2]; a += p[3];
                    count++;
                }
            }
            BYTE* d = &pixels[((size_t)y * w + x) * 4];
            d[0] = (BYTE)(b / count);
            d[1] = (BYTE)(g / count);
            d[2] = (BYTE)(r / count);
            d[3] = (BYTE)(a / count);
        }
    }
}

static bool UpdateAlbumBlurBgCache(BlurBgCache& cache,
                                   const std::vector<BYTE>& thumbBytes,
                                   int targetW, int targetH)
{
    if (thumbBytes.empty() || targetW <= 0 || targetH <= 0) {
        cache.Invalidate();
        return false;
    }
    size_t artHash = 0;
    for (size_t i = 0; i < thumbBytes.size(); i += 512)
        artHash = artHash * 31 + thumbBytes[i];
    if (cache.artHash == artHash && cache.width == targetW &&
        cache.height == targetH && !cache.blurredPixels.empty())
        return true;

    std::vector<BYTE> srcPixels; int srcW = 0, srcH = 0;
    if (!DecodeImageToBGRA(thumbBytes, srcPixels, srcW, srcH)) return false;

    int blurDiv = 8;
    int smallW = srcW / blurDiv; if (smallW < 1) smallW = 1;
    int smallH = srcH / blurDiv; if (smallH < 1) smallH = 1;

    std::vector<BYTE> small;
    DownsampleBGRA(srcPixels, srcW, srcH, small, smallW, smallH);

    int blurRadius = std::clamp(Cfg()->blurRadius, 1, 50);
    for (int i = 0; i < 3; ++i) {
        ApplyBoxBlurBGRA(small, smallW, smallH, blurRadius);
    }

    std::vector<BYTE> blurred;
    DownsampleBGRA(small, smallW, smallH, blurred, targetW, targetH);

    cache.blurredPixels = std::move(blurred);
    cache.width = targetW;
    cache.height = targetH;
    cache.artHash = artHash;
    return true;
}

static AlbumPalette ExtractAlbumPalette(const std::vector<BYTE>& thumbBytes) {
    const winrt::Windows::UI::Color fallbackPrimary{255, 18, 18, 18};
    const winrt::Windows::UI::Color fallbackSecondary{255, 45, 45, 45};

    if (thumbBytes.empty())
        return {fallbackPrimary, fallbackSecondary};

    try {
        std::vector<BYTE> pixels;
        int w = 0, h = 0;
        if (!DecodeImageToBGRA(thumbBytes, pixels, w, h) || w <= 0 || h <= 0 ||
            pixels.size() < (size_t)w * h * 4)
            return {fallbackPrimary, fallbackSecondary};

        struct Bucket { uint32_t r=0, g=0, b=0, n=0; };
        Bucket buckets[16][16][16]{};

        for (int y = 0; y < h; y += 4) {
            for (int x = 0; x < w; x += 4) {
                size_t idx = ((size_t)y * w + x) * 4;
                if (idx + 4 > pixels.size()) continue;

                BYTE pb = pixels[idx];
                BYTE pg = pixels[idx + 1];
                BYTE pr = pixels[idx + 2];

                int luma = (pr * 299 + pg * 587 + pb * 114) / 1000;
                if (luma < 24 || luma > 235) continue;

                auto& bk = buckets[pr >> 4][pg >> 4][pb >> 4];
                bk.r += pr; bk.g += pg; bk.b += pb; bk.n++;
            }
        }

        struct Cand { float w; BYTE r, g, b; };
        std::vector<Cand> cands;
        cands.reserve(64);

        for (int R = 0; R < 16; R++)
            for (int G = 0; G < 16; G++)
                for (int B = 0; B < 16; B++) {
                    auto& bk = buckets[R][G][B];
                    if (bk.n < 8) continue;
                    float fr = bk.r / (float)bk.n / 255.f;
                    float fg = bk.g / (float)bk.n / 255.f;
                    float fb = bk.b / (float)bk.n / 255.f;
                    float mx = std::max({fr, fg, fb});
                    float mn = std::min({fr, fg, fb});
                    float sat = mx > 0 ? (mx - mn) / mx : 0;
                    cands.push_back({bk.n * (0.3f + sat),
                                     (BYTE)(fr * 255), (BYTE)(fg * 255), (BYTE)(fb * 255)});
                }

        if (cands.empty())
            return {fallbackPrimary, fallbackSecondary};

        std::sort(cands.begin(), cands.end(),
                  [](const Cand& a, const Cand& b){ return a.w > b.w; });

        winrt::Windows::UI::Color primary{255, cands[0].r, cands[0].g, cands[0].b};
        winrt::Windows::UI::Color secondary = primary;

        for (auto& c : cands) {
            int dr = (int)c.r - (int)cands[0].r;
            int dg = (int)c.g - (int)cands[0].g;
            int db = (int)c.b - (int)cands[0].b;
            if (dr*dr + dg*dg + db*db > 3264) {
                secondary = winrt::Windows::UI::Color{255, c.r, c.g, c.b};
                break;
            }
        }

        return {primary, secondary};
    } catch (...) {
        return {fallbackPrimary, fallbackSecondary};
    }
}

static DWORD GetWindowsAccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque)))
        return 0xFF000000 | (color & 0x00FFFFFF);
    return 0xFF0078D4;
}

static winrt::Windows::UI::Color ParseColorWithSpecialValues(const std::wstring& colorStr, BYTE alpha = 255) {
    int r = 255, g = 255, b = 255;
    size_t pos1 = colorStr.find(L' ');
    size_t pos2 = colorStr.find(L' ', pos1 + 1);

    if (pos1 != std::wstring::npos && pos2 != std::wstring::npos) {
        try {
            r = std::stoi(colorStr.substr(0, pos1));
            g = std::stoi(colorStr.substr(pos1 + 1, pos2 - pos1 - 1));
            b = std::stoi(colorStr.substr(pos2 + 1));

            if (r == -1 && g == -1 && b == -1) {
                DWORD accentColor = GetWindowsAccentColor();
                return winrt::Windows::UI::Color{alpha,
                    (BYTE)((accentColor >> 16) & 0xFF),
                    (BYTE)((accentColor >> 8) & 0xFF),
                    (BYTE)(accentColor & 0xFF)};
            }

            if (r == -2 && g == -2 && b == -2) {
                if (g_primaryVisualState->cachedPaletteHash == 0) {
                    return winrt::Windows::UI::Color{0, 255, 255, 255};
                }
                return winrt::Windows::UI::Color{alpha,
                    g_primaryVisualState->cachedAlbumPalette.primary.R,
                    g_primaryVisualState->cachedAlbumPalette.primary.G,
                    g_primaryVisualState->cachedAlbumPalette.primary.B};
            }

            r = std::clamp(r, 0, 255);
            g = std::clamp(g, 0, 255);
            b = std::clamp(b, 0, 255);
        } catch (...) {}
    }

    return winrt::Windows::UI::Color{alpha, (BYTE)r, (BYTE)g, (BYTE)b};
}

static winrt::Windows::UI::Color ParseColorWithThemeSupport(const std::wstring& colorStr, BYTE alpha) {
    size_t dollarPos = colorStr.find(L'$');

    if (dollarPos != std::wstring::npos) {
        std::wstring lightColorStr = colorStr.substr(0, dollarPos);
        std::wstring darkColorStr = colorStr.substr(dollarPos + 1);

        lightColorStr.erase(0, lightColorStr.find_first_not_of(L" \t"));
        lightColorStr.erase(lightColorStr.find_last_not_of(L" \t") + 1);
        darkColorStr.erase(0, darkColorStr.find_first_not_of(L" \t"));
        darkColorStr.erase(darkColorStr.find_last_not_of(L" \t") + 1);

        if (IsSystemLightTheme()) {
            return ParseColorWithSpecialValues(lightColorStr, alpha);
        } else {
            return ParseColorWithSpecialValues(darkColorStr, alpha);
        }
    } else {
        return ParseColorWithSpecialValues(colorStr, alpha);
    }
}

static winrt::Windows::UI::Color TextColor() {
    BYTE alpha = (BYTE)((Cfg()->titleColorOpacity / 100.0) * 255);
    return ParseColorWithThemeSupport(Cfg()->titleColor, alpha);
}

static winrt::Windows::UI::Color ArtistColor() {
    BYTE alpha = (BYTE)((Cfg()->artistColorOpacity / 100.0) * 255);
    return ParseColorWithThemeSupport(Cfg()->artistColor, alpha);
}

static winrt::Windows::UI::Color ButtonColor() {
    BYTE alpha = (BYTE)((Cfg()->buttonColorOpacity / 100.0) * 255);
    return ParseColorWithThemeSupport(Cfg()->buttonColor, alpha);
}

static winrt::Windows::UI::Color ContextMenuIconColor() {
    return ButtonColor();
}

static const std::wstring& ContextMenuIconStyle() {
    return Cfg()->iconStyle;
}

static Brush MakeAlbumBlurBrush(BlurBgCache& cache,
                                const std::vector<BYTE>& thumbBytes,
                                int panelW, int panelH)
{
    if (!UpdateAlbumBlurBgCache(cache, thumbBytes, panelW, panelH) ||
        cache.blurredPixels.empty())
        return MakeBrush({0x00, 0x00, 0x00, 0x00});
    try {
        size_t bytesNeeded = (size_t)panelW * panelH * 4;
        WriteableBitmap wb(panelW, panelH);
        auto buf = wb.PixelBuffer();
        auto byteAccess = buf.as<Windows::Storage::Streams::IBufferByteAccess>();
        BYTE* pixels = nullptr;
        byteAccess->Buffer(&pixels);
        if (pixels && cache.blurredPixels.size() >= bytesNeeded)
            memcpy(pixels, cache.blurredPixels.data(), bytesNeeded);
        buf.Length(static_cast<uint32_t>(bytesNeeded));
        wb.Invalidate();
        ImageBrush brush;
        brush.ImageSource(wb);
        brush.Stretch(Stretch::UniformToFill);
        return brush;
    } catch (...) {}
    return MakeBrush({0x00, 0x00, 0x00, 0x00});
}
static Brush MakeBackgroundBrush() {
    auto t = Cfg()->backgroundType;

    BYTE opacity = (BYTE)((Cfg()->solidOpacity / 100.0) * 255);
    auto color1 = ParseColorWithThemeSupport(Cfg()->solidColor, opacity);
    auto color2 = ParseColorWithSpecialValues(Cfg()->solidColor2, opacity);
    auto gradientColor2 = ParseColorWithSpecialValues(Cfg()->gradientColor2, opacity);

    if (t == L"gradient") {
        try {
            winrt::Windows::UI::Xaml::Media::LinearGradientBrush brush;

            double angleRad = (Cfg()->gradientAngle % 360) * 3.14159265358979323846 / 180.0;
            double startX = 0.5 - 0.5 * std::cos(angleRad);
            double startY = 0.5 - 0.5 * std::sin(angleRad);
            double endX = 0.5 + 0.5 * std::cos(angleRad);
            double endY = 0.5 + 0.5 * std::sin(angleRad);

            brush.StartPoint(winrt::Windows::Foundation::Point((float)startX, (float)startY));
            brush.EndPoint(winrt::Windows::Foundation::Point((float)endX, (float)endY));

            double balancePoint = std::clamp(Cfg()->gradientBalance, 0, 100) / 100.0;

            winrt::Windows::UI::Xaml::Media::GradientStop stop1;
            stop1.Color(color2);
            stop1.Offset(0.0);

            winrt::Windows::UI::Xaml::Media::GradientStop stop2;
            stop2.Color(gradientColor2);
            stop2.Offset(balancePoint);

            winrt::Windows::UI::Xaml::Media::GradientStop stop3;
            stop3.Color(gradientColor2);
            stop3.Offset(1.0);

            brush.GradientStops().Append(stop1);
            brush.GradientStops().Append(stop2);
            brush.GradientStops().Append(stop3);

            return brush;
        } catch (...) {}
    }

    if (t == L"acrylic") {
        try {
            winrt::Windows::UI::Xaml::Media::AcrylicBrush brush;
            brush.BackgroundSource(winrt::Windows::UI::Xaml::Media::AcrylicBackgroundSource::HostBackdrop);
            auto col = winrt::Windows::UI::Color{0xFF, color1.R, color1.G, color1.B};
            brush.TintColor(col);
            brush.TintOpacity(Cfg()->acrylicTintOpacity / 100.0);
            brush.FallbackColor(winrt::Windows::UI::Color{0xCC, color1.R, color1.G, color1.B});
            return brush;
        } catch (...) {}
    }

    if (t == L"mica" || t == L"mica_alt") {
        BYTE micaAlpha = (BYTE)((Cfg()->micaOpacity / 100.0) * 255);
        auto col = winrt::Windows::UI::Color{micaAlpha, color1.R, color1.G, color1.B};
        return MakeBrush(col);
    }

    if (t == L"solid") {
        return MakeBrush(color1);
    }

    if (t == L"album_art_blur") {
        return MakeBrush({0x00, 0x00, 0x00, 0x00});
    }

    return MakeBrush({0x00, 0x00, 0x00, 0x00});
}

static FrameworkElement FindChildByName(FrameworkElement const& root, std::wstring_view name, int depth = 32) {
    if (!root || depth == 0) return nullptr;
    int n = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (child.Name() == name) return child;
        if (auto found = FindChildByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}

static void DumpXamlTree(DependencyObject const& node, int depth, int maxDepth) {
    if (!node || depth > maxDepth) return;
    std::wstring indent(depth * 2, L' ');
    auto fe = node.try_as<FrameworkElement>();
    std::wstring name  = fe ? std::wstring(fe.Name()) : L"";
    winrt::hstring typeHstr = winrt::get_class_name(node);
    std::wstring type  = std::wstring(typeHstr);
    auto dot = type.rfind(L'.');
    if (dot != std::wstring::npos) type = type.substr(dot + 1);

    int col = fe ? Grid::GetColumn(fe) : -1;
    if (!name.empty()) Wh_Log(L"%ls[%ls] name='%ls' col=%d", indent.c_str(), type.c_str(), name.c_str(), col);
    else Wh_Log(L"%ls[%ls]", indent.c_str(), type.c_str());

    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; ++i) {
        auto child = VisualTreeHelper::GetChild(node, i);
        if (child) DumpXamlTree(child, depth + 1, maxDepth);
    }
}

static constexpr wchar_t kGridName[]        = L"TaskbarMediaPresenceBar";
static constexpr wchar_t kArtImageName[]    = L"TaskbarMediaPresence_Art";
static constexpr wchar_t kArtContainerName[]= L"TaskbarMediaPresence_ArtContainer";
static constexpr wchar_t kAppIconImageName[]= L"TaskbarMediaPresence_AppIcon";
static constexpr wchar_t kAnchorOverlayName[]= L"TaskbarMediaPresence_DebugAnchorTarget";
static constexpr wchar_t kTextStackName[]   = L"TaskbarMediaPresence_TextStack";
static constexpr wchar_t kTitleBlockName[]  = L"TaskbarMediaPresence_Title";
static constexpr wchar_t kArtistBlockName[] = L"TaskbarMediaPresence_Artist";
static constexpr wchar_t kPlayBtnName[]     = L"TaskbarMediaPresence_Play";
static constexpr wchar_t kPrevBtnName[]     = L"TaskbarMediaPresence_Prev";
static constexpr wchar_t kNextBtnName[]     = L"TaskbarMediaPresence_Next";
static constexpr wchar_t kVolumeBtnName[]   = L"TaskbarMediaPresence_Volume";
static constexpr wchar_t kRewindBtnName[]   = L"TaskbarMediaPresence_Rewind";
static constexpr wchar_t kForwardBtnName[]  = L"TaskbarMediaPresence_Forward";
static constexpr wchar_t kShuffleBtnName[]  = L"TaskbarMediaPresence_Shuffle";
static constexpr wchar_t kRepeatBtnName[]   = L"TaskbarMediaPresence_Repeat";
static constexpr wchar_t kProgressHitName[] = L"TaskbarMediaPresence_ProgressHit";
static constexpr wchar_t kProgressTrackName[] = L"TaskbarMediaPresence_ProgressTrack";
static constexpr wchar_t kProgressFillName[] = L"TaskbarMediaPresence_ProgressFill";

static std::atomic<int>  g_idleSeconds{0};
static std::atomic<bool> g_hiddenByIdle{false};
static std::chrono::steady_clock::time_point g_lastMediaTime = std::chrono::steady_clock::now();

static void ShowAppVolumeFlyout(FrameworkElement const& target);

static bool MediaIdentityMatchesForSeek(
    uint64_t expectedGeneration,
    const std::wstring& expectedTitle,
    const std::wstring& expectedArtist,
    uint64_t expectedThumbnailHash,
    int64_t expectedDurationSeconds) {
    if (g_sessionGeneration.load(std::memory_order_acquire) !=
        expectedGeneration) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_mediaMtx);
    if (g_sessionGeneration.load(std::memory_order_acquire) !=
        expectedGeneration) {
        return false;
    }
    if (g_media.title != expectedTitle ||
        g_media.artist != expectedArtist ||
        g_media.durationSeconds != expectedDurationSeconds) {
        return false;
    }
    return !expectedThumbnailHash || !g_media.thumbnailHash ||
           g_media.thumbnailHash == expectedThumbnailHash;
}

static void SendMediaSeekAsync(
    int64_t targetSeconds,
    uint64_t expectedGeneration,
    std::wstring expectedTitle,
    std::wstring expectedArtist,
    uint64_t expectedThumbnailHash,
    int64_t expectedDurationSeconds) {
    if (expectedDurationSeconds <= 0 ||
        !MediaIdentityMatchesForSeek(
            expectedGeneration, expectedTitle, expectedArtist,
            expectedThumbnailHash, expectedDurationSeconds)) {
        return;
    }

    targetSeconds = std::clamp<int64_t>(
        targetSeconds, 0, expectedDurationSeconds);
    uint64_t seekRequestId = BeginPendingSeek(
        targetSeconds, expectedGeneration, expectedTitle, expectedArtist,
        expectedThumbnailHash, expectedDurationSeconds);

    // Update the local timeline immediately so the bar doesn't jump back while
    // the media application processes the asynchronous seek request.
    {
        std::lock_guard<std::mutex> lock(g_mediaMtx);
        if (g_sessionGeneration.load(std::memory_order_acquire) ==
                expectedGeneration &&
            g_media.title == expectedTitle &&
            g_media.artist == expectedArtist &&
            g_media.durationSeconds == expectedDurationSeconds &&
            (!expectedThumbnailHash || !g_media.thumbnailHash ||
             g_media.thumbnailHash == expectedThumbnailHash)) {
            g_media.positionSeconds = targetSeconds;
            g_media.timelineSampleTick = GetTickCount64();
        }
    }
    DispatchMediaUpdate();

    bool queued = QueueAsyncTask([
        targetSeconds, expectedGeneration,
        expectedTitle = std::move(expectedTitle),
        expectedArtist = std::move(expectedArtist),
        expectedThumbnailHash, expectedDurationSeconds,
        seekRequestId]() {
        if (g_unloading ||
            !MediaIdentityMatchesForSeek(
                expectedGeneration, expectedTitle, expectedArtist,
                expectedThumbnailHash, expectedDurationSeconds)) {
            ClearPendingSeekIfRequest(seekRequestId);
            return;
        }

        struct ApartmentGuard {
            ApartmentGuard() {
                winrt::init_apartment(
                    winrt::apartment_type::multi_threaded);
            }
            ~ApartmentGuard() noexcept {
                winrt::uninit_apartment();
            }
        } apartmentGuard;

        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                if (g_sessionGeneration.load(std::memory_order_acquire) !=
                    expectedGeneration) {
                    ClearPendingSeekIfRequest(seekRequestId);
                    return;
                }
                session = g_currentSession;
            }
            if (!session) {
                ClearPendingSeekIfRequest(seekRequestId);
                return;
            }

            auto timeline = session.GetTimelineProperties();
            auto startTime = timeline.StartTime();
            auto endTime = timeline.EndTime();
            auto requestedTime =
                startTime + std::chrono::seconds(targetSeconds);
            if (requestedTime < startTime) requestedTime = startTime;
            if (requestedTime > endTime) requestedTime = endTime;

            auto changed = WaitForWinrtOperation(
                session.TryChangePlaybackPositionAsync(
                    requestedTime.count()),
                3000, L"Progress-bar seek command");
            if (!changed.value_or(false)) {
                ClearPendingSeekIfRequest(seekRequestId);
                Wh_Log(L"Progress bar: player rejected seek to %lld seconds",
                       static_cast<long long>(targetSeconds));
            }
        } catch (const winrt::hresult_error& error) {
            ClearPendingSeekIfRequest(seekRequestId);
            Wh_Log(L"Progress bar: seek failed (0x%08X)",
                   static_cast<unsigned int>(error.code().value));
        } catch (...) {
            ClearPendingSeekIfRequest(seekRequestId);
            Wh_Log(L"Progress bar: seek failed");
        }

        // Refresh immediately. The pending-seek guard rejects stale timeline
        // samples while the player publishes the accepted position.
        RequestMediaSessionRefresh();
    });
    if (!queued) {
        ClearPendingSeekIfRequest(seekRequestId);
        Wh_Log(L"Progress bar: seek request couldn't be queued");
        RequestMediaSessionRefresh();
    }
}

static std::mutex g_pendingMediaCommandsMtx;
static std::vector<int> g_pendingMediaCommands;
static bool g_mediaCommandWorkerRunning = false;

static void ExecuteMediaCommandOnWorker(int cmd) {
    if (g_unloading) return;
    try {
        GlobalSystemMediaTransportControlsSession session{nullptr};
        {
            std::lock_guard<std::mutex> lock(g_sessionMtx);
            session = g_currentSession;
        }
        if (!session) return;

        switch (cmd) {
            case 1:
                WaitForWinrtOperation(
                    session.TrySkipPreviousAsync(), 3000,
                    L"Previous-track command");
                break;
            case 2:
                WaitForWinrtOperation(
                    session.TryTogglePlayPauseAsync(), 3000,
                    L"Play/pause command");
                break;
            case 3:
                WaitForWinrtOperation(
                    session.TrySkipNextAsync(), 3000,
                    L"Next-track command");
                break;
            case 5:
                try {
                    auto timeline = session.GetTimelineProperties();
                    auto currentPos = timeline.Position();
                    auto newPos = currentPos - std::chrono::seconds(5);
                    if (newPos.count() < 0) {
                        newPos = std::chrono::seconds(0);
                    }
                    WaitForWinrtOperation(
                        session.TryChangePlaybackPositionAsync(
                            newPos.count()),
                        3000, L"Rewind command");
                } catch (...) {}
                break;
            case 6:
                try {
                    auto timeline = session.GetTimelineProperties();
                    auto currentPos = timeline.Position();
                    auto endTime = timeline.EndTime();
                    auto newPos = currentPos + std::chrono::seconds(5);
                    if (newPos > endTime) newPos = endTime;
                    WaitForWinrtOperation(
                        session.TryChangePlaybackPositionAsync(
                            newPos.count()),
                        3000, L"Forward command");
                } catch (...) {}
                break;
            case 7:
                try {
                    bool currentShuffle = g_shuffleEnabled.load();
                    auto changed = WaitForWinrtOperation(
                        session.TryChangeShuffleActiveAsync(
                            !currentShuffle),
                        3000, L"Shuffle command");
                    if (changed.value_or(false)) {
                        g_shuffleEnabled = !currentShuffle;
                        DispatchMediaUpdate();
                    }
                } catch (...) {}
                break;
            case 8:
                try {
                    RepeatMode current = g_repeatMode.load();
                    winrt::Windows::Media::MediaPlaybackAutoRepeatMode mode;
                    RepeatMode next;
                    switch (current) {
                        case RepeatMode::Off:
                            mode = winrt::Windows::Media::
                                MediaPlaybackAutoRepeatMode::List;
                            next = RepeatMode::All;
                            break;
                        case RepeatMode::All:
                            mode = winrt::Windows::Media::
                                MediaPlaybackAutoRepeatMode::Track;
                            next = RepeatMode::One;
                            break;
                        case RepeatMode::One:
                        default:
                            mode = winrt::Windows::Media::
                                MediaPlaybackAutoRepeatMode::None;
                            next = RepeatMode::Off;
                            break;
                    }
                    auto changed = WaitForWinrtOperation(
                        session.TryChangeAutoRepeatModeAsync(mode),
                        3000, L"Repeat command");
                    if (changed.value_or(false)) {
                        g_repeatMode = next;
                        DispatchMediaUpdate();
                    }
                } catch (...) {}
                break;
        }
    } catch (...) {}
}

static void SendMediaCommandAsync(int cmd) {
    {
        std::lock_guard<std::mutex> lock(g_pendingMediaCommandsMtx);
        // Bound abusive input while preserving a normal wheel flick or click
        // sequence. Commands are still processed in order by one worker.
        if (g_pendingMediaCommands.size() >= 64) {
            Wh_Log(L"Media command queue full; dropping command %d", cmd);
            return;
        }
        g_pendingMediaCommands.push_back(cmd);
        if (g_mediaCommandWorkerRunning) return;
        g_mediaCommandWorkerRunning = true;
    }

    bool queued = QueueAsyncTask([]() {
        HRESULT apartmentResult =
            CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        while (!g_unloading) {
            std::vector<int> commands;
            {
                std::lock_guard<std::mutex> lock(g_pendingMediaCommandsMtx);
                commands.swap(g_pendingMediaCommands);
                if (commands.empty()) {
                    g_mediaCommandWorkerRunning = false;
                    break;
                }
            }

            for (int command : commands) {
                if (g_unloading) break;
                ExecuteMediaCommandOnWorker(command);
            }

            std::lock_guard<std::mutex> lock(g_pendingMediaCommandsMtx);
            if (g_pendingMediaCommands.empty()) {
                g_mediaCommandWorkerRunning = false;
                break;
            }
        }
        if (g_unloading) {
            std::lock_guard<std::mutex> lock(g_pendingMediaCommandsMtx);
            g_pendingMediaCommands.clear();
            g_mediaCommandWorkerRunning = false;
        }
        if (SUCCEEDED(apartmentResult)) CoUninitialize();
    });

    if (!queued) {
        std::lock_guard<std::mutex> lock(g_pendingMediaCommandsMtx);
        g_mediaCommandWorkerRunning = false;
        Wh_Log(L"Media command worker couldn't be queued");
    }
}

static std::atomic<bool> g_scrollResetRequested{false};

static void ResetScrollState(TextScrollState& s);

static void FetchMediaPropertiesAsync();
static void FetchPlaybackInfoAsync();
static void OnSessionsChanged();
static void AttachToSession(GlobalSystemMediaTransportControlsSession session);
static bool IsIgnoredMediaApp(const std::wstring& appUserModelId);
static std::wstring ToLowerCopy(std::wstring value);

static void SelectMediaSource(const std::wstring& appId, bool persist) {
    StorePreferredMediaApp(appId);
    if (persist) {
        Wh_SetStringValue(L"quickPreferredMediaApp", appId.c_str());
        Wh_SetIntValue(L"quickMediaSourceOverride", 1);
    }

    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        g_userSwitchedSession = !appId.empty();
    }
    g_forceSessionRefresh = true;
    RequestMediaSessionRefresh();
}

static std::wstring ToLowerCopy(std::wstring value);
static std::wstring PathFileStem(std::wstring path);

static std::wstring ProcessApplicationUserModelId(HANDLE process) {
    if (!process) return L"";

    UINT32 length = 0;
    LONG result = GetApplicationUserModelId(process, &length, nullptr);
    if (result != ERROR_INSUFFICIENT_BUFFER || length == 0) return L"";

    std::vector<wchar_t> buffer(length);
    result = GetApplicationUserModelId(process, &length, buffer.data());
    if (result != ERROR_SUCCESS || buffer.empty() || !buffer[0]) return L"";
    return std::wstring(buffer.data());
}

static std::wstring QueryProcessImagePath(HANDLE process) {
    if (!process) return L"";

    DWORD capacity = 512;
    while (capacity <= 32768) {
        std::wstring path(capacity, L'\0');
        DWORD length = capacity;
        if (QueryFullProcessImageNameW(process, 0, path.data(), &length)) {
            path.resize(length);
            return path;
        }
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) break;
        capacity *= 2;
    }

    return L"";
}

static bool ContainsIdentifierToken(const std::wstring& value,
                                    const std::wstring& token) {
    if (token.size() < 5) return false;

    auto isTokenCharacter = [](wchar_t character) {
        return iswalnum(character) || character == L'.' ||
               character == L'_' || character == L'-';
    };

    size_t position = 0;
    while ((position = value.find(token, position)) != std::wstring::npos) {
        bool leftBoundary =
            position == 0 || !isTokenCharacter(value[position - 1]);
        size_t end = position + token.size();
        bool rightBoundary =
            end == value.size() || !isTokenCharacter(value[end]);
        if (leftBoundary && rightBoundary) return true;
        position = end;
    }

    return false;
}

static bool SamePackagedAppIdentity(const std::wstring& first,
                                    const std::wstring& second) {
    if (first.empty() || second.empty()) return false;

    std::wstring a = ToLowerCopy(first);
    std::wstring b = ToLowerCopy(second);
    if (a == b) return true;

    auto packagePart = [](const std::wstring& value) {
        size_t separator = value.find(L'!');
        return value.substr(0, separator);
    };
    std::wstring packageA = packagePart(a);
    std::wstring packageB = packagePart(b);
    return !packageA.empty() && packageA == packageB;
}

static bool KnownMediaPlayerProcessAlias(const std::wstring& appLower,
                                         const std::wstring& processStemLower) {
    // The current Microsoft Media Player publishes the historic ZuneMusic
    // package identity while its executable is Microsoft.Media.Player.exe.
    if ((appLower.find(L"microsoft.zunemusic") != std::wstring::npos ||
         appLower.find(L"windowsmediaplayer") != std::wstring::npos) &&
        (processStemLower == L"microsoft.media.player" ||
         processStemLower == L"music.ui" ||
         processStemLower == L"wmplayer")) {
        return true;
    }

    return false;
}

static bool AudioSessionMatchesApp(IAudioSessionControl2* sessionControl2,
                                   const std::wstring& appUserModelId) {
    if (!sessionControl2 || appUserModelId.empty()) return false;

    const std::wstring appLower = ToLowerCopy(appUserModelId);
    std::wstring appStem = ToLowerCopy(PathFileStem(appUserModelId));
    if (appStem == L"applicationframehost" || appStem == L"explorer") {
        appStem.clear();
    }

    DWORD processId = 0;
    if (SUCCEEDED(sessionControl2->GetProcessId(&processId)) && processId) {
        HANDLE process = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (process) {
            const std::wstring processAumid =
                ProcessApplicationUserModelId(process);
            const std::wstring processPath =
                QueryProcessImagePath(process);
            CloseHandle(process);

            if (SamePackagedAppIdentity(appUserModelId, processAumid)) {
                return true;
            }

            if (!processPath.empty()) {
                const std::wstring processLower =
                    ToLowerCopy(processPath);
                const std::wstring processStem =
                    ToLowerCopy(PathFileStem(processPath));
                if (processLower == appLower ||
                    (!appStem.empty() && processStem == appStem) ||
                    KnownMediaPlayerProcessAlias(appLower, processStem)) {
                    return true;
                }
            }
        }
    }

    // Session identifiers vary between desktop and packaged applications.
    // Accept the complete app identity, or a reasonably long stem delimited as
    // an identifier token. Avoid unrestricted substring matches such as "vlc"
    // or "mpc", which can select an unrelated audio session.
    LPWSTR rawSessionId = nullptr;
    if (SUCCEEDED(sessionControl2->GetSessionIdentifier(&rawSessionId)) &&
        rawSessionId) {
        const std::wstring sessionIdLower = ToLowerCopy(rawSessionId);
        CoTaskMemFree(rawSessionId);
        if (sessionIdLower == appLower ||
            (appLower.size() >= 8 &&
             sessionIdLower.find(appLower) != std::wstring::npos) ||
            (!appStem.empty() &&
             ContainsIdentifierToken(sessionIdLower, appStem))) {
            return true;
        }
    }

    return false;
}

static bool ForEachMatchingAudioSession(
    const std::wstring& appUserModelId,
    IMMDeviceEnumerator* deviceEnumerator,
    const std::function<void(IAudioSessionControl*, IAudioSessionControl2*)>& visitor) {
    if (appUserModelId.empty()) return false;
    winrt::com_ptr<IMMDeviceEnumerator> localDeviceEnumerator;
    if (!deviceEnumerator) {
        if (FAILED(CoCreateInstance(XIID_MMDeviceEnumerator, nullptr, CLSCTX_INPROC_SERVER,
                                    XIID_IMMDeviceEnumerator,
                                    localDeviceEnumerator.put_void()))) {
            return false;
        }
        deviceEnumerator = localDeviceEnumerator.get();
    }
    if (!deviceEnumerator) return false;

    bool found = false;
    winrt::com_ptr<IMMDeviceCollection> devices;
    HRESULT hr = deviceEnumerator->EnumAudioEndpoints(
        eRender, DEVICE_STATE_ACTIVE, devices.put());
    if (FAILED(hr) || !devices) return false;

    UINT deviceCount = 0;
    if (FAILED(devices->GetCount(&deviceCount))) return false;

    for (UINT deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex) {
        winrt::com_ptr<IMMDevice> device;
        if (FAILED(devices->Item(deviceIndex, device.put())) || !device) continue;

        winrt::com_ptr<IAudioSessionManager2> sessionManager;
        if (FAILED(device->Activate(XIID_IAudioSessionManager2, CLSCTX_ALL,
                                    nullptr, sessionManager.put_void())) ||
            !sessionManager) {
            continue;
        }

        winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
        if (FAILED(sessionManager->GetSessionEnumerator(sessionEnumerator.put())) ||
            !sessionEnumerator) {
            continue;
        }

        int sessionCount = 0;
        if (FAILED(sessionEnumerator->GetCount(&sessionCount))) continue;

        for (int sessionIndex = 0; sessionIndex < sessionCount; ++sessionIndex) {
            winrt::com_ptr<IAudioSessionControl> sessionControl;
            if (FAILED(sessionEnumerator->GetSession(sessionIndex,
                                                     sessionControl.put())) ||
                !sessionControl) {
                continue;
            }

            winrt::com_ptr<IAudioSessionControl2> sessionControl2;
            if (FAILED(sessionControl->QueryInterface(
                    __uuidof(IAudioSessionControl2), sessionControl2.put_void())) ||
                !sessionControl2) {
                continue;
            }

            if (AudioSessionMatchesApp(sessionControl2.get(), appUserModelId)) {
                found = true;
                visitor(sessionControl.get(), sessionControl2.get());
            }
        }
    }

    return found;
}

static bool GetAppVolumeStateByAUMID(
    const std::wstring& appUserModelId, float& volume, bool& muted) {
    bool stateRead = false;
    ForEachMatchingAudioSession(
        appUserModelId, nullptr,
        [&](IAudioSessionControl* sessionControl, IAudioSessionControl2*) {
            if (stateRead) return;

            winrt::com_ptr<ISimpleAudioVolume> appVolume;
            if (FAILED(sessionControl->QueryInterface(
                    __uuidof(ISimpleAudioVolume), appVolume.put_void())) ||
                !appVolume) {
                return;
            }

            float sessionVolume = 0.0f;
            BOOL sessionMuted = FALSE;
            if (SUCCEEDED(appVolume->GetMasterVolume(&sessionVolume))) {
                volume = sessionVolume;
                muted = SUCCEEDED(appVolume->GetMute(&sessionMuted)) &&
                        sessionMuted != FALSE;
                stateRead = true;
            }
        });
    return stateRead;
}

static bool SetAppVolumeStateByAUMID(
    const std::wstring& appUserModelId, float volume, bool unmute) {
    bool changed = false;
    volume = std::clamp(volume, 0.0f, 1.0f);
    ForEachMatchingAudioSession(
        appUserModelId, nullptr,
        [&](IAudioSessionControl* sessionControl, IAudioSessionControl2*) {
            winrt::com_ptr<ISimpleAudioVolume> appVolume;
            if (FAILED(sessionControl->QueryInterface(
                    __uuidof(ISimpleAudioVolume), appVolume.put_void())) ||
                !appVolume) {
                return;
            }

            if (SUCCEEDED(appVolume->SetMasterVolume(volume, nullptr))) {
                changed = true;
                if (unmute) {
                    appVolume->SetMute(FALSE, nullptr);
                }
            }
        });
    return changed;
}

static std::wstring GetWindowAppUserModelId(HWND hWnd);
static void ShowMediaContextMenu(FrameworkElement const& target);

static void ExecuteMediaAction(const std::wstring& action, FrameworkElement const& sourceElement = nullptr) {
    if (action == L"none") {
        return;
    } else if (action == L"open_context_menu") {
        if (sourceElement) {
            ShowMediaContextMenu(sourceElement);
        }
        return;
    } else if (action == L"play_pause") {
        SendMediaCommandAsync(2);
        DispatchMediaUpdate();
    } else if (action == L"next_track") {
        SendMediaCommandAsync(3);
        DispatchMediaUpdate();
    } else if (action == L"prev_track") {
        SendMediaCommandAsync(1);
        DispatchMediaUpdate();
    } else if (action == L"rewind_5s") {
        SendMediaCommandAsync(5);
        DispatchMediaUpdate();
    } else if (action == L"forward_5s") {
        SendMediaCommandAsync(6);
        DispatchMediaUpdate();
    } else if (action == L"toggle_shuffle") {
        SendMediaCommandAsync(7);
        DispatchMediaUpdate();
    } else if (action == L"toggle_repeat") {
        SendMediaCommandAsync(8);
        DispatchMediaUpdate();
    } else if (action == L"open_app") {
        bool queued = QueueAsyncTask([]() {
            HRESULT apartmentResult =
                CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            struct ApartmentCleanup {
                HRESULT result;
                ~ApartmentCleanup() {
                    if (SUCCEEDED(result)) CoUninitialize();
                }
            } apartmentCleanup{apartmentResult};
            bool comAvailable =
                SUCCEEDED(apartmentResult) ||
                apartmentResult == RPC_E_CHANGED_MODE;
            if (!comAvailable) {
                Wh_Log(L"Open-media-app action: COM initialization failed "
                       L"(0x%08X)", apartmentResult);
            }

            std::wstring appAumid;
            {
                std::lock_guard<std::mutex> lk(g_sessionMtx);
                if (g_currentSession) {
                    try {
                        appAumid = std::wstring(g_currentSession.SourceAppUserModelId());
                    } catch (...) {}
                }
            }

            struct WindowSearch {
                std::wstring targetAumid;
                std::set<DWORD> matchingProcessIds;
                bool comAvailable = false;
                HWND aumidHwnd = nullptr;
                HWND processHwnd = nullptr;
            };
            WindowSearch search;
            search.comAvailable = comAvailable;
            search.targetAumid = ToLowerCopy(appAumid);

            // Reuse the strict audio-session identity matcher to resolve the
            // process IDs that actually belong to the selected media source.
            // This avoids foregrounding an unrelated process merely because a
            // short executable name appears inside an AUMID.
            if (!appAumid.empty()) {
                ForEachMatchingAudioSession(
                    appAumid, nullptr,
                    [&](IAudioSessionControl*,
                        IAudioSessionControl2* sessionControl2) {
                        DWORD processId = 0;
                        if (sessionControl2 &&
                            SUCCEEDED(sessionControl2->GetProcessId(&processId)) &&
                            processId) {
                            search.matchingProcessIds.insert(processId);
                        }
                    });
            }

            EnumWindows([](HWND hwnd, LPARAM lParam) CALLBACK -> BOOL {
                if (!IsWindowVisible(hwnd)) return TRUE;
                WINDOWINFO wi{};
                wi.cbSize = sizeof(wi);
                if (!GetWindowInfo(hwnd, &wi)) return TRUE;
                if ((wi.dwStyle & WS_CHILD) != 0 ||
                    (wi.dwExStyle & WS_EX_TOOLWINDOW) != 0) {
                    return TRUE;
                }

                auto* s = reinterpret_cast<WindowSearch*>(lParam);

                if (s->comAvailable && !s->aumidHwnd &&
                    !s->targetAumid.empty()) {
                    IPropertyStore* pps = nullptr;
                    if (SUCCEEDED(SHGetPropertyStoreForWindow(
                            hwnd, IID_PPV_ARGS(&pps))) && pps) {
                        PROPVARIANT var;
                        PropVariantInit(&var);
                        if (SUCCEEDED(pps->GetValue(
                                PKEY_AppUserModel_ID, &var)) &&
                            var.vt == VT_LPWSTR && var.pwszVal &&
                            SamePackagedAppIdentity(
                                s->targetAumid, var.pwszVal)) {
                            s->aumidHwnd = hwnd;
                        }
                        PropVariantClear(&var);
                        pps->Release();
                    }
                }

                if (!s->processHwnd && !s->targetAumid.empty()) {
                    DWORD pid = 0;
                    GetWindowThreadProcessId(hwnd, &pid);
                    if (!pid) return TRUE;

                    if (s->matchingProcessIds.find(pid) !=
                        s->matchingProcessIds.end()) {
                        s->processHwnd = hwnd;
                        return TRUE;
                    }

                    // Some players don't expose a normal audio session. Keep a
                    // conservative process-name fallback, but require a proper
                    // identifier token or a known explicit alias. Short loose
                    // substring matches such as "vlc", "mpc", or "tv" are
                    // intentionally rejected.
                    const std::wstring processPath = [&]() {
                        HANDLE process = OpenProcess(
                            PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                        if (!process) return std::wstring{};
                        std::wstring path = QueryProcessImagePath(process);
                        CloseHandle(process);
                        return path;
                    }();
                    const std::wstring processStem =
                        ToLowerCopy(PathFileStem(processPath));
                    if (!processStem.empty() &&
                        (KnownMediaPlayerProcessAlias(
                             s->targetAumid, processStem) ||
                         (s->targetAumid.size() >= 8 &&
                          ContainsIdentifierToken(
                              s->targetAumid, processStem)))) {
                        s->processHwnd = hwnd;
                    }
                }

                return TRUE;
            }, reinterpret_cast<LPARAM>(&search));

            HWND targetWindow =
                search.aumidHwnd ? search.aumidHwnd : search.processHwnd;

            if (targetWindow) {
                if (IsIconic(targetWindow)) {
                    ShowWindow(targetWindow, SW_RESTORE);
                }

                HWND hCurWnd = GetForegroundWindow();
                if (hCurWnd && hCurWnd != targetWindow) {
                    DWORD dwMyID = GetCurrentThreadId();
                    DWORD dwCurID = GetWindowThreadProcessId(hCurWnd, NULL);
                    AttachThreadInput(dwCurID, dwMyID, TRUE);
                    SetForegroundWindow(targetWindow);
                    BringWindowToTop(targetWindow);
                    AttachThreadInput(dwCurID, dwMyID, FALSE);
                } else {
                    SetForegroundWindow(targetWindow);
                    BringWindowToTop(targetWindow);
                }
                return;
            }

            if (!appAumid.empty()) {
                std::wstring shellPath = L"shell:AppsFolder\\" + appAumid;
                ShellExecuteW(nullptr, L"open", shellPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }

        });
        if (!queued) {
            Wh_Log(L"Open-media-app action couldn't be queued");
        }
    }
}

static std::wstring ToLowerCopy(std::wstring value) {
    for (auto& c : value) c = towlower(c);
    return value;
}

static std::wstring PathFileStem(std::wstring path) {
    auto slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) path = path.substr(slash + 1);
    auto dot = path.rfind(L'.');
    if (dot != std::wstring::npos) path = path.substr(0, dot);
    return path;
}

static std::atomic<HWND> g_volumePopupWindow{nullptr};
static HWND g_volumePopupTitle = nullptr;
static HWND g_volumePopupPercent = nullptr;
static std::wstring g_volumePopupAppUserModelId;
static HBRUSH g_volumePopupBrush = nullptr;
static COLORREF g_volumePopupTextColor = RGB(245, 245, 245);
static COLORREF g_volumePopupTrackColor = RGB(96, 96, 102);
static COLORREF g_volumePopupFillColor = RGB(0, 120, 215);
static COLORREF g_volumePopupDisabledFillColor = RGB(100, 100, 104);
static HFONT g_volumePopupFont = nullptr;
static UINT g_volumePopupDpi = 96;
static HMODULE g_modModuleHandle = nullptr;
static bool g_volumePopupClassRegistered = false;
static constexpr wchar_t kVolumePopupClass[] =
    L"TaskbarMediaPresenceVolumePopup";
static constexpr UINT kVolumeStateChangedMessage = WM_APP + 0x431;
static constexpr int kVolumePopupLogicalWidth = 84;
static constexpr int kVolumePopupLogicalHeight = 184;
static constexpr int kVolumeTrackLogicalCenterX = 42;
static constexpr int kVolumeTrackLogicalTop = 34;
static constexpr int kVolumeTrackLogicalBottom = 146;
static int g_volumePopupValue = 0;
static bool g_volumePopupDragging = false;
static bool g_volumePopupEnabled = false;
static bool g_volumePopupClosing = false;
static std::atomic<int> g_currentVolumePercent{-1};
static std::atomic<bool> g_currentVolumeMuted{false};

static UINT GetDpiForMonitorWithFallback(HMONITOR monitor) {
    UINT dpiX = 96;
    UINT dpiY = 96;
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    HMODULE shcore = GetModuleHandleW(L"shcore.dll");
    auto getDpiForMonitor = shcore
        ? reinterpret_cast<GetDpiForMonitor_t>(
              GetProcAddress(shcore, "GetDpiForMonitor"))
        : nullptr;
    if (monitor && getDpiForMonitor &&
        SUCCEEDED(getDpiForMonitor(monitor, 0, &dpiX, &dpiY)) && dpiX) {
        return dpiX;
    }
    return 96;
}

static UINT GetDpiForWindowWithFallback(HWND window) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto getDpiForWindow = user32
        ? reinterpret_cast<GetDpiForWindow_t>(
              GetProcAddress(user32, "GetDpiForWindow"))
        : nullptr;
    if (window && getDpiForWindow) {
        UINT dpi = getDpiForWindow(window);
        if (dpi) return dpi;
    }
    return GetDpiForMonitorWithFallback(
        MonitorFromWindow(window ? window : g_taskbarWnd.load(),
                          MONITOR_DEFAULTTONEAREST));
}

static int ScaleVolumePopupMetric(int value) {
    return MulDiv(value, static_cast<int>(g_volumePopupDpi), 96);
}

static int VolumePopupWidth() {
    return ScaleVolumePopupMetric(kVolumePopupLogicalWidth);
}

static int VolumePopupHeight() {
    return ScaleVolumePopupMetric(kVolumePopupLogicalHeight);
}

static int VolumeTrackCenterX() {
    return ScaleVolumePopupMetric(kVolumeTrackLogicalCenterX);
}

static int VolumeTrackTop() {
    return ScaleVolumePopupMetric(kVolumeTrackLogicalTop);
}

static int VolumeTrackBottom() {
    return ScaleVolumePopupMetric(kVolumeTrackLogicalBottom);
}

static void RecreateVolumePopupFont() {
    if (g_volumePopupFont) {
        DeleteObject(g_volumePopupFont);
        g_volumePopupFont = nullptr;
    }
    int fontHeight = -MulDiv(9, static_cast<int>(g_volumePopupDpi), 72);
    g_volumePopupFont = CreateFontW(
        fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

static void ApplyVolumePopupDpiLayout(HWND window, UINT dpi) {
    g_volumePopupDpi = dpi ? dpi : 96;
    RecreateVolumePopupFont();
    HFONT font = g_volumePopupFont
        ? g_volumePopupFont
        : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

    if (g_volumePopupTitle) {
        SetWindowPos(
            g_volumePopupTitle, nullptr,
            ScaleVolumePopupMetric(6), ScaleVolumePopupMetric(8),
            ScaleVolumePopupMetric(72), ScaleVolumePopupMetric(20),
            SWP_NOZORDER | SWP_NOACTIVATE);
        SendMessageW(g_volumePopupTitle, WM_SETFONT,
                     reinterpret_cast<WPARAM>(font), TRUE);
    }
    if (g_volumePopupPercent) {
        SetWindowPos(
            g_volumePopupPercent, nullptr,
            ScaleVolumePopupMetric(10), ScaleVolumePopupMetric(154),
            ScaleVolumePopupMetric(64), ScaleVolumePopupMetric(22),
            SWP_NOZORDER | SWP_NOACTIVATE);
        SendMessageW(g_volumePopupPercent, WM_SETFONT,
                     reinterpret_cast<WPARAM>(font), TRUE);
    }
    if (window) InvalidateRect(window, nullptr, TRUE);
}

static std::atomic<bool> g_moduleSafetyPinned{false};
static HMODULE g_moduleSafetyPinHandle = nullptr;

static void RetainModuleForSafety(PCWSTR reason) {
    bool expected = false;
    if (!g_moduleSafetyPinned.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel)) {
        return;
    }

    HMODULE module = nullptr;
    if (GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR>(&RetainModuleForSafety),
            &module) && module) {
        g_moduleSafetyPinHandle = module;
        Wh_Log(
            L"Lifecycle safety: cleanup left %ls; retaining the module until "
            L"Explorer restarts instead of hanging or unloading live code",
            reason ? reason : L"an outstanding callback");
        return;
    }

    g_moduleSafetyPinned.store(false, std::memory_order_release);
    Wh_Log(
        L"Lifecycle safety: cleanup left %ls and the emergency module "
        L"retention failed",
        reason ? reason : L"an outstanding callback");
}

static void ReportOutstandingCallbackRisk(PCWSTR reason) {
    if (g_unloading.load(std::memory_order_acquire)) {
        RetainModuleForSafety(reason);
    } else {
        Wh_Log(
            L"Lifecycle safety: cleanup left %ls; the module will be retained "
            L"if the condition is still present during unload",
            reason ? reason : L"an outstanding callback");
    }
}

static std::wstring CurrentMediaAppUserModelId() {
    std::wstring appUserModelId;
    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        appUserModelId = g_media.appUserModelId;
    }
    if (appUserModelId.empty()) {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        if (g_currentSession) {
            try {
                appUserModelId = std::wstring(g_currentSession.SourceAppUserModelId());
            } catch (...) {}
        }
    }
    return appUserModelId;
}

static std::wstring FriendlyMediaAppName(const std::wstring& appUserModelId) {
    if (appUserModelId.empty()) return L"Media app";

    if (ToLowerCopy(appUserModelId).find(L"musicbee") != std::wstring::npos) {
        return L"MusicBee";
    }

    size_t separator = appUserModelId.rfind(L'!');
    if (separator != std::wstring::npos && separator + 1 < appUserModelId.size()) {
        std::wstring suffix = appUserModelId.substr(separator + 1);
        if (_wcsicmp(suffix.c_str(), L"app") != 0) return suffix;
    }

    std::wstring package = separator == std::wstring::npos
        ? appUserModelId
        : appUserModelId.substr(0, separator);
    size_t dot = package.rfind(L'.');
    if (dot != std::wstring::npos && dot + 1 < package.size()) {
        std::wstring name = package.substr(dot + 1);
        size_t underscore = name.find(L'_');
        if (underscore != std::wstring::npos) name.resize(underscore);
        if (!name.empty()) return name;
    }

    std::wstring stem = PathFileStem(appUserModelId);
    return stem.empty() ? L"Media app" : stem;
}

static bool IsMusicBeeAppId(const std::wstring& appUserModelId) {
    return ToLowerCopy(appUserModelId).find(L"musicbee") != std::wstring::npos;
}

static const wchar_t* VolumeGlyphForState(int percent, bool muted) {
    if (muted || percent <= 0) return L"\uE74F"; // muted
    if (percent <= 45) return L"\uE993";         // one wave
    return L"\uE995";                            // full waves
}

static void PublishVolumeState(int percent, bool muted) {
    percent = std::clamp(percent, 0, 100);
    int oldPercent = g_currentVolumePercent.exchange(percent);
    bool oldMuted = g_currentVolumeMuted.exchange(muted);
    if (oldPercent != percent || oldMuted != muted) {
        g_needsUiUpdate = true;
        HWND popupWindow =
            g_volumePopupWindow.load(std::memory_order_acquire);
        if (popupWindow && IsWindow(popupWindow)) {
            PostMessageW(popupWindow, kVolumeStateChangedMessage,
                         static_cast<WPARAM>(percent), muted ? 1 : 0);
        }
    }
}

static bool IsMusicBeeProcessId(DWORD processId) {
    if (!processId) return false;
    HANDLE process = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) return false;

    const std::wstring path = QueryProcessImagePath(process);
    CloseHandle(process);
    if (path.empty()) return false;

    return ToLowerCopy(PathFileStem(path)) == L"musicbee";
}

using GetNamedPipeServerProcessIdFn = BOOL(WINAPI*)(HANDLE, PULONG);

static bool QueryNamedPipeServerProcessId(HANDLE pipe, DWORD& processId) {
    // Windhawk's bundled import libraries don't necessarily expose this
    // Kernel32 entry point even though it is available on supported Windows 11
    // systems. Resolve it dynamically to keep the safety check without adding
    // a hard linker dependency.
    static GetNamedPipeServerProcessIdFn getServerProcessId = []() {
        HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
        if (!kernel32) return static_cast<GetNamedPipeServerProcessIdFn>(nullptr);
        return reinterpret_cast<GetNamedPipeServerProcessIdFn>(
            GetProcAddress(kernel32, "GetNamedPipeServerProcessId"));
    }();

    if (!getServerProcessId) {
        Wh_Log(L"MusicBee bridge: GetNamedPipeServerProcessId is unavailable");
        return false;
    }

    ULONG serverProcessId = 0;
    if (!getServerProcessId(pipe, &serverProcessId)) {
        Wh_Log(L"MusicBee bridge: pipe-server query failed (%u)",
               GetLastError());
        return false;
    }

    processId = static_cast<DWORD>(serverProcessId);
    return processId != 0;
}

static bool ExchangeMusicBeeVolumeBridge(const char* request,
                                          int& percent, bool& muted,
                                          DWORD totalTimeoutMs) {
    static constexpr wchar_t kPipeName[] =
        L"\\\\.\\pipe\\TaskbarMediaPresence.MusicBeeVolume.v1";
    totalTimeoutMs = std::max<DWORD>(totalTimeoutMs, 50);
    const ULONGLONG deadline = GetTickCount64() + totalTimeoutMs;
    auto remainingTime = [deadline]() -> DWORD {
        ULONGLONG now = GetTickCount64();
        return now < deadline
            ? static_cast<DWORD>(deadline - now)
            : 0;
    };

    HANDLE pipe = INVALID_HANDLE_VALUE;
    while (remainingTime() > 0) {
        pipe = CreateFileW(kPipeName, GENERIC_READ | GENERIC_WRITE,
                           0, nullptr, OPEN_EXISTING,
                           FILE_FLAG_OVERLAPPED, nullptr);
        if (pipe != INVALID_HANDLE_VALUE) break;

        DWORD error = GetLastError();
        DWORD remaining = remainingTime();
        if (!remaining) break;
        if (error == ERROR_PIPE_BUSY) {
            WaitNamedPipeW(kPipeName, std::min<DWORD>(remaining, 100));
            continue;
        }
        // The companion creates its pipe shortly after MusicBee finishes
        // plugin startup. Interactive operations run on background workers, so
        // they can tolerate a short retry window; the periodic poll uses a
        // 250-ms total budget and stays responsive.
        if (error == ERROR_FILE_NOT_FOUND && totalTimeoutMs > 250) {
            Sleep(std::min<DWORD>(remaining, 40));
            continue;
        }
        break;
    }
    if (pipe == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD serverProcessId = 0;
    if (!QueryNamedPipeServerProcessId(pipe, serverProcessId) ||
        !IsMusicBeeProcessId(serverProcessId)) {
        Wh_Log(L"MusicBee bridge: rejected pipe server process %u",
               serverProcessId);
        CloseHandle(pipe);
        return false;
    }

    auto transferWithTimeout = [pipe, &remainingTime](
                                   bool write, void* buffer, DWORD size,
                                   DWORD& transferred) {
        OVERLAPPED overlapped{};
        overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!overlapped.hEvent) return false;

        BOOL started = write
            ? WriteFile(pipe, buffer, size, &transferred, &overlapped)
            : ReadFile(pipe, buffer, size, &transferred, &overlapped);
        bool success = started != FALSE;
        if (!success && GetLastError() == ERROR_IO_PENDING) {
            DWORD remaining = remainingTime();
            DWORD waitResult = remaining
                ? WaitForSingleObject(overlapped.hEvent, remaining)
                : WAIT_TIMEOUT;
            if (waitResult == WAIT_OBJECT_0) {
                success = GetOverlappedResult(
                    pipe, &overlapped, &transferred, FALSE) != FALSE;
            } else {
                CancelIoEx(pipe, &overlapped);
                GetOverlappedResult(pipe, &overlapped, &transferred, TRUE);
                success = false;
            }
        }
        CloseHandle(overlapped.hEvent);
        return success;
    };

    DWORD written = 0;
    DWORD requestLength = static_cast<DWORD>(strlen(request));
    bool success = transferWithTimeout(
                       true, const_cast<char*>(request), requestLength,
                       written) &&
                   written == requestLength;
    char response[96]{};
    DWORD read = 0;
    if (success) {
        success = transferWithTimeout(
            false, response, sizeof(response) - 1, read);
    }
    CloseHandle(pipe);
    if (!success) {
        return false;
    }

    response[std::min<DWORD>(read, sizeof(response) - 1)] = '\0';
    int muteValue = 0;
    if (std::sscanf(response, "OK %d %d", &percent, &muteValue) != 2) {
        return false;
    }
    percent = std::clamp(percent, 0, 100);
    muted = muteValue != 0;
    return true;
}

static bool IsCurrentMediaApp(const std::wstring& appUserModelId) {
    return !appUserModelId.empty() &&
           _wcsicmp(CurrentMediaAppUserModelId().c_str(),
                    appUserModelId.c_str()) == 0;
}

static bool GetControllableVolumeForApp(
    const std::wstring& appUserModelId, int& percent, bool& muted,
    DWORD musicBeeTimeoutMs = 2250) {
    if (appUserModelId.empty()) return false;

    if (IsMusicBeeAppId(appUserModelId)) {
        bool success =
            ExchangeMusicBeeVolumeBridge(
                "GET\n", percent, muted, musicBeeTimeoutMs);
        if (success && IsCurrentMediaApp(appUserModelId)) {
            PublishVolumeState(percent, muted);
        }
        return success;
    }

    float volume = 0.0f;
    bool appMuted = false;
    if (!GetAppVolumeStateByAUMID(
            appUserModelId, volume, appMuted)) {
        return false;
    }
    percent = std::clamp((int)std::lround(volume * 100.0f), 0, 100);
    muted = appMuted;
    if (IsCurrentMediaApp(appUserModelId)) {
        PublishVolumeState(percent, muted);
    }
    return true;
}

static bool GetCurrentControllableVolume(int& percent, bool& muted) {
    return GetControllableVolumeForApp(
        CurrentMediaAppUserModelId(), percent, muted, 250);
}

static bool SetControllableVolumeForApp(
    const std::wstring& appUserModelId, int requestedPercent,
    DWORD musicBeeTimeoutMs = 2250) {
    requestedPercent = std::clamp(requestedPercent, 0, 100);
    if (appUserModelId.empty()) return false;

    if (IsMusicBeeAppId(appUserModelId)) {
        char request[32]{};
        std::snprintf(request, sizeof(request), "SET %d\n", requestedPercent);
        int actualPercent = requestedPercent;
        bool muted = false;
        bool success =
            ExchangeMusicBeeVolumeBridge(
                request, actualPercent, muted, musicBeeTimeoutMs);
        if (success && IsCurrentMediaApp(appUserModelId)) {
            PublishVolumeState(actualPercent, muted);
        }
        return success;
    }

    bool changed = SetAppVolumeStateByAUMID(
        appUserModelId, requestedPercent / 100.0f,
        requestedPercent > 0);
    if (changed && IsCurrentMediaApp(appUserModelId)) {
        PublishVolumeState(requestedPercent, false);
    }
    return changed;
}

static std::mutex g_pendingVolumeSetMtx;
static std::wstring g_pendingVolumeSetApp;
static int g_pendingVolumeSetPercent = 0;
static uint64_t g_pendingVolumeSetGeneration = 0;
static bool g_volumeSetWorkerRunning = false;

static bool QueueControllableVolumeSet(
    const std::wstring& appUserModelId, int requestedPercent) {
    if (appUserModelId.empty()) return false;
    {
        std::lock_guard<std::mutex> lock(g_pendingVolumeSetMtx);
        g_pendingVolumeSetApp = appUserModelId;
        g_pendingVolumeSetPercent =
            std::clamp(requestedPercent, 0, 100);
        ++g_pendingVolumeSetGeneration;
        if (g_volumeSetWorkerRunning) return true;
        g_volumeSetWorkerRunning = true;
    }

    bool queued = QueueAsyncTask([]() {
        HRESULT apartmentResult =
            CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        while (!g_unloading) {
            // Coalesce continuous slider movement and mouse-wheel changes to at
            // most about 25 audio writes per second.
            Sleep(40);
            if (g_unloading) break;

            std::wstring appUserModelId;
            int requestedPercent = 0;
            uint64_t generation = 0;
            {
                std::lock_guard<std::mutex> lock(g_pendingVolumeSetMtx);
                appUserModelId = g_pendingVolumeSetApp;
                requestedPercent = g_pendingVolumeSetPercent;
                generation = g_pendingVolumeSetGeneration;
            }

            SetControllableVolumeForApp(
                appUserModelId, requestedPercent);

            std::lock_guard<std::mutex> lock(g_pendingVolumeSetMtx);
            if (generation == g_pendingVolumeSetGeneration) {
                g_volumeSetWorkerRunning = false;
                break;
            }
        }
        if (g_unloading) {
            std::lock_guard<std::mutex> lock(g_pendingVolumeSetMtx);
            g_volumeSetWorkerRunning = false;
        }
        if (SUCCEEDED(apartmentResult)) CoUninitialize();
    });
    if (!queued) {
        std::lock_guard<std::mutex> lock(g_pendingVolumeSetMtx);
        g_volumeSetWorkerRunning = false;
    }
    return queued;
}

static std::mutex g_pendingVolumeAdjustMtx;
static std::wstring g_pendingVolumeAdjustApp;
static int g_pendingVolumeAdjustSteps = 0;
static bool g_volumeAdjustWorkerRunning = false;

static void AdjustCurrentMediaAppVolumeAsync(int wheelDelta) {
    std::wstring appUserModelId = CurrentMediaAppUserModelId();
    if (appUserModelId.empty() || wheelDelta == 0) return;

    int steps = wheelDelta / WHEEL_DELTA;
    if (!steps) steps = wheelDelta > 0 ? 1 : -1;
    steps = std::clamp(steps, -50, 50);

    {
        std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
        if (!g_pendingVolumeAdjustApp.empty() &&
            _wcsicmp(g_pendingVolumeAdjustApp.c_str(),
                     appUserModelId.c_str()) != 0) {
            // A source change makes old, not-yet-processed wheel input stale.
            g_pendingVolumeAdjustSteps = 0;
        }
        g_pendingVolumeAdjustApp = appUserModelId;
        g_pendingVolumeAdjustSteps = std::clamp(
            g_pendingVolumeAdjustSteps + steps, -50, 50);
        if (g_volumeAdjustWorkerRunning) return;
        g_volumeAdjustWorkerRunning = true;
    }

    bool queued = QueueAsyncTask([]() {
        HRESULT apartmentResult =
            CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        while (!g_unloading) {
            std::wstring appUserModelId;
            int steps = 0;
            {
                std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
                appUserModelId = g_pendingVolumeAdjustApp;
                steps = g_pendingVolumeAdjustSteps;
                g_pendingVolumeAdjustSteps = 0;
            }

            if (appUserModelId.empty() || !steps) {
                std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
                if (!g_pendingVolumeAdjustSteps) {
                    g_volumeAdjustWorkerRunning = false;
                    break;
                }
                continue;
            }

            int percent = 0;
            bool muted = false;
            DWORD bridgeTimeout = IsMusicBeeAppId(appUserModelId) ? 750 : 2250;
            bool changed = false;
            if (GetControllableVolumeForApp(
                    appUserModelId, percent, muted, bridgeTimeout)) {
                int requestedPercent = std::clamp(
                    percent + steps * 2, 0, 100);
                changed = SetControllableVolumeForApp(
                    appUserModelId, requestedPercent, bridgeTimeout);
            }

            if (!changed) {
                Wh_Log(L"Volume control: no supported volume endpoint for %ls",
                       appUserModelId.c_str());
            }

            std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
            if (!g_pendingVolumeAdjustSteps) {
                g_volumeAdjustWorkerRunning = false;
                break;
            }
        }
        if (g_unloading) {
            std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
            g_pendingVolumeAdjustSteps = 0;
            g_volumeAdjustWorkerRunning = false;
        }
        if (SUCCEEDED(apartmentResult)) CoUninitialize();
    });

    if (!queued) {
        std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
        g_volumeAdjustWorkerRunning = false;
        Wh_Log(L"Volume adjustment worker couldn't be queued for %ls",
               appUserModelId.c_str());
    }
}

static void UpdateVolumePopupPercent(int volumePercent) {
    volumePercent = std::clamp(volumePercent, 0, 100);
    g_volumePopupValue = volumePercent;
    if (g_volumePopupPercent) {
        std::wstring text = std::to_wstring(volumePercent) + L"%";
        SetWindowTextW(g_volumePopupPercent, text.c_str());
    }
    HWND popupWindow =
        g_volumePopupWindow.load(std::memory_order_acquire);
    if (popupWindow) {
        InvalidateRect(popupWindow, nullptr, FALSE);
    }
}

static int VolumePercentFromPopupY(int y) {
    int trackTop = VolumeTrackTop();
    int trackBottom = VolumeTrackBottom();
    y = std::clamp(y, trackTop, trackBottom);
    double ratio = static_cast<double>(trackBottom - y) /
                   static_cast<double>(trackBottom - trackTop);
    return std::clamp(static_cast<int>(std::lround(ratio * 100.0)), 0, 100);
}

static int PopupYFromVolumePercent(int percent) {
    percent = std::clamp(percent, 0, 100);
    double ratio = percent / 100.0;
    int trackTop = VolumeTrackTop();
    int trackBottom = VolumeTrackBottom();
    return static_cast<int>(std::lround(
        trackBottom - ratio * (trackBottom - trackTop)));
}

static bool IsPointInVolumeTrack(int x, int y) {
    return x >= ScaleVolumePopupMetric(20) &&
           x <= ScaleVolumePopupMetric(64) &&
           y >= VolumeTrackTop() - ScaleVolumePopupMetric(10) &&
           y <= VolumeTrackBottom() + ScaleVolumePopupMetric(10);
}

static void SetPopupVolumeFromY(HWND window, int y) {
    if (!g_volumePopupEnabled) return;
    int requested = VolumePercentFromPopupY(y);
    if (requested == g_volumePopupValue) return;

    int previous = g_volumePopupValue;
    UpdateVolumePopupPercent(requested);
    if (!QueueControllableVolumeSet(
            g_volumePopupAppUserModelId, requested)) {
        UpdateVolumePopupPercent(previous);
    }
    InvalidateRect(window, nullptr, FALSE);
}

static bool IsCursorInsideVolumePopup(HWND window) {
    POINT cursor{};
    RECT popupRect{};
    return GetCursorPos(&cursor) && GetWindowRect(window, &popupRect) &&
           PtInRect(&popupRect, cursor);
}

static bool CloseVolumePopupWindow(HWND window) {
    if (!window || !IsWindow(window)) return true;
    if (g_volumePopupClosing) return false;

    g_volumePopupClosing = true;
    if (GetCapture() == window) {
        ReleaseCapture();
    }
    if (!DestroyWindow(window)) {
        g_volumePopupClosing = false;
        return false;
    }
    return true;
}

static LRESULT CALLBACK VolumePopupWindowProc(HWND window, UINT message,
                                               WPARAM wParam,
                                               LPARAM lParam) noexcept {
    try {
        switch (message) {
        case WM_CREATE: {
            g_volumePopupDpi = GetDpiForWindowWithFallback(window);
            RecreateVolumePopupFont();
            HFONT font = g_volumePopupFont
                ? g_volumePopupFont
                : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
            std::wstring title = FriendlyMediaAppName(g_volumePopupAppUserModelId);

            g_volumePopupTitle = CreateWindowExW(
                0, L"STATIC", title.c_str(),
                WS_CHILD | WS_VISIBLE | SS_CENTER | SS_ENDELLIPSIS,
                ScaleVolumePopupMetric(6), ScaleVolumePopupMetric(8),
                ScaleVolumePopupMetric(72), ScaleVolumePopupMetric(20),
                window, nullptr, GetModuleHandleW(nullptr), nullptr);
            SendMessageW(g_volumePopupTitle, WM_SETFONT,
                         reinterpret_cast<WPARAM>(font), TRUE);

            g_volumePopupPercent = CreateWindowExW(
                0, L"STATIC", L"0%", WS_CHILD | WS_VISIBLE | SS_CENTER,
                ScaleVolumePopupMetric(10), ScaleVolumePopupMetric(154),
                ScaleVolumePopupMetric(64), ScaleVolumePopupMetric(22),
                window, nullptr, GetModuleHandleW(nullptr), nullptr);
            SendMessageW(g_volumePopupPercent, WM_SETFONT,
                         reinterpret_cast<WPARAM>(font), TRUE);

            int volumePercent = g_currentVolumePercent.load();
            if (volumePercent < 0) volumePercent = 100;
            g_volumePopupEnabled =
                !g_volumePopupAppUserModelId.empty();
            UpdateVolumePopupPercent(volumePercent);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (!IsCursorInsideVolumePopup(window)) {
                CloseVolumePopupWindow(window);
                return 0;
            }
            int x = static_cast<short>(LOWORD(lParam));
            int y = static_cast<short>(HIWORD(lParam));
            if (g_volumePopupEnabled && IsPointInVolumeTrack(x, y)) {
                g_volumePopupDragging = true;
                SetCapture(window);
                SetPopupVolumeFromY(window, y);
            }
            return 0;
        }
        case WM_MOUSEMOVE:
            if (g_volumePopupDragging && GetCapture() == window) {
                SetPopupVolumeFromY(
                    window, static_cast<short>(HIWORD(lParam)));
            }
            return 0;
        case WM_LBUTTONUP:
            if (g_volumePopupDragging) {
                SetPopupVolumeFromY(
                    window, static_cast<short>(HIWORD(lParam)));
                g_volumePopupDragging = false;
            }
            return 0;
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            if (!IsCursorInsideVolumePopup(window)) {
                CloseVolumePopupWindow(window);
            }
            return 0;
        case WM_CAPTURECHANGED:
            g_volumePopupDragging = false;
            if (!g_volumePopupClosing &&
                reinterpret_cast<HWND>(lParam) != window) {
                CloseVolumePopupWindow(window);
            }
            return 0;
        case WM_MOUSEWHEEL:
            if (!IsCursorInsideVolumePopup(window)) {
                CloseVolumePopupWindow(window);
                return 0;
            }
            if (g_volumePopupEnabled) {
                int next = std::clamp(
                    g_volumePopupValue +
                    (GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 2 : -2),
                    0, 100);
                UpdateVolumePopupPercent(next);
                QueueControllableVolumeSet(
                    g_volumePopupAppUserModelId, next);
            }
            return 0;
        case kVolumeStateChangedMessage:
            UpdateVolumePopupPercent(
                std::clamp(static_cast<int>(wParam), 0, 100));
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_DPICHANGED: {
            UINT dpi = LOWORD(wParam);
            RECT suggested = *reinterpret_cast<RECT*>(lParam);
            g_volumePopupDpi = dpi ? dpi : 96;
            SetWindowPos(
                window, nullptr, suggested.left, suggested.top,
                VolumePopupWidth(), VolumePopupHeight(),
                SWP_NOZORDER | SWP_NOACTIVATE);
            ApplyVolumePopupDpiLayout(window, g_volumePopupDpi);
            return 0;
        }
        case WM_CTLCOLORSTATIC: {
            HDC dc = (HDC)wParam;
            SetTextColor(dc, g_volumePopupTextColor);
            SetBkMode(dc, TRANSPARENT);
            return (LRESULT)g_volumePopupBrush;
        }
        case WM_CTLCOLORBTN: {
            HDC dc = (HDC)wParam;
            SetTextColor(dc, g_volumePopupTextColor);
            SetBkMode(dc, TRANSPARENT);
            return (LRESULT)g_volumePopupBrush;
        }
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(window, &paint);
            RECT client{};
            GetClientRect(window, &client);
            FillRect(dc, &client, g_volumePopupBrush);

            HPEN oldPen = static_cast<HPEN>(
                SelectObject(dc, GetStockObject(NULL_PEN)));
            HBRUSH trackBrush = CreateSolidBrush(g_volumePopupTrackColor);
            HBRUSH fillBrush = CreateSolidBrush(
                g_volumePopupEnabled ? g_volumePopupFillColor
                                     : g_volumePopupDisabledFillColor);

            int centerX = VolumeTrackCenterX();
            int trackTop = VolumeTrackTop();
            int trackBottom = VolumeTrackBottom();
            int halfTrack = std::max(1, ScaleVolumePopupMetric(2));
            int roundness = std::max(2, ScaleVolumePopupMetric(4));
            int thumbRadius = std::max(4, ScaleVolumePopupMetric(7));

            HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(dc, trackBrush));
            RoundRect(dc, centerX - halfTrack, trackTop,
                      centerX + halfTrack, trackBottom,
                      roundness, roundness);

            int thumbY = PopupYFromVolumePercent(g_volumePopupValue);
            if (thumbY < trackBottom) {
                SelectObject(dc, fillBrush);
                RoundRect(dc, centerX - halfTrack, thumbY,
                          centerX + halfTrack, trackBottom,
                          roundness, roundness);
            }
            SelectObject(dc, fillBrush);
            Ellipse(dc, centerX - thumbRadius, thumbY - thumbRadius,
                    centerX + thumbRadius, thumbY + thumbRadius);

            SelectObject(dc, oldBrush);
            SelectObject(dc, oldPen);
            DeleteObject(trackBrush);
            DeleteObject(fillBrush);
            EndPaint(window, &paint);
            return 0;
        }
        case WM_ERASEBKGND: {
            RECT rect{};
            GetClientRect(window, &rect);
            FillRect((HDC)wParam, &rect, g_volumePopupBrush);
            return 1;
        }
            case WM_NCDESTROY: {
                g_volumePopupClosing = true;
                if (GetCapture() == window) ReleaseCapture();
                HWND expected = window;
                g_volumePopupWindow.compare_exchange_strong(
                    expected, nullptr, std::memory_order_acq_rel,
                    std::memory_order_acquire);
                g_volumePopupTitle = nullptr;
                g_volumePopupPercent = nullptr;
                if (g_volumePopupFont) {
                    DeleteObject(g_volumePopupFont);
                    g_volumePopupFont = nullptr;
                }
                g_volumePopupDragging = false;
                g_volumePopupEnabled = false;
                g_volumePopupClosing = false;
                return 0;
            }
        }
        return DefWindowProcW(window, message, wParam, lParam);
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Volume popup callback failed: 0x%08X",
               static_cast<uint32_t>(error.code()));
    } catch (...) {
        Wh_Log(L"Volume popup callback failed with an unexpected exception");
    }

    // C++ exceptions must never escape an Explorer window procedure. If the
    // failure happened while the HWND was being torn down, clear the published
    // handle as well so worker threads cannot post to a recycled window value.
    if (message == WM_NCDESTROY) {
        g_volumePopupClosing = true;
        if (GetCapture() == window) ReleaseCapture();
        HWND expected = window;
        g_volumePopupWindow.compare_exchange_strong(
            expected, nullptr, std::memory_order_acq_rel,
            std::memory_order_acquire);
        g_volumePopupTitle = nullptr;
        g_volumePopupPercent = nullptr;
        if (g_volumePopupFont) {
            DeleteObject(g_volumePopupFont);
            g_volumePopupFont = nullptr;
        }
        g_volumePopupDragging = false;
        g_volumePopupEnabled = false;
        g_volumePopupClosing = false;
    }
    if (message == WM_CREATE) return -1;
    return DefWindowProcW(window, message, wParam, lParam);
}

static bool DestroyVolumePopup(bool shutdownCleanup = false) {
    HWND window = g_volumePopupWindow.load(std::memory_order_acquire);
    if (!window || !IsWindow(window)) {
        HWND expected = window;
        if (g_volumePopupWindow.compare_exchange_strong(
                expected, nullptr, std::memory_order_acq_rel,
                std::memory_order_acquire)) {
            g_volumePopupTitle = nullptr;
            g_volumePopupPercent = nullptr;
            if (g_volumePopupFont) {
                DeleteObject(g_volumePopupFont);
                g_volumePopupFont = nullptr;
            }
            g_volumePopupDragging = false;
            g_volumePopupEnabled = false;
            g_volumePopupClosing = false;
        }
        return true;
    }

    struct PopupDestroyWork {
        HWND window;
        bool destroyed = false;
    } work{window};

    auto destroy = [](void* parameter) {
        auto* workItem = static_cast<PopupDestroyWork*>(parameter);
        if (!IsWindow(workItem->window)) {
            workItem->destroyed = true;
            return;
        }
        workItem->destroyed = CloseVolumePopupWindow(workItem->window);
    };

    bool dispatched = false;
    DWORD windowThread = GetWindowThreadProcessId(window, nullptr);
    if (windowThread == GetCurrentThreadId()) {
        dispatched = InvokeWindowThreadProcSafely(destroy, &work);
    } else if (shutdownCleanup) {
        dispatched = RunFromWindowThreadForCleanup(window, destroy, &work);
    } else {
        dispatched = RunFromWindowThread(window, destroy, &work);
    }

    bool gone = !IsWindow(window) ||
                g_volumePopupWindow.load(std::memory_order_acquire) != window;
    if (!dispatched || !work.destroyed || !gone) {
        Wh_Log(L"DestroyVolumePopup: owner-thread destruction was not confirmed");
        return false;
    }
    return true;
}

static bool UnregisterVolumePopupClassConfirmed() {
    HWND popupWindow =
        g_volumePopupWindow.load(std::memory_order_acquire);
    if (popupWindow && IsWindow(popupWindow)) return false;
    if (!g_volumePopupClassRegistered) return true;
    if (!g_modModuleHandle) return false;

    SetLastError(ERROR_SUCCESS);
    if (UnregisterClassW(kVolumePopupClass, g_modModuleHandle)) {
        g_volumePopupClassRegistered = false;
        return true;
    }

    DWORD error = GetLastError();
    if (error == ERROR_CLASS_DOES_NOT_EXIST) {
        g_volumePopupClassRegistered = false;
        return true;
    }

    Wh_Log(L"Volume popup: class unregister wasn't confirmed (%u)", error);
    return false;
}

static void ShowAppVolumeFlyout(FrameworkElement const& target) {
    if (!target || g_unloading) return;
    HWND existingPopup =
        g_volumePopupWindow.load(std::memory_order_acquire);
    if (existingPopup && IsWindow(existingPopup)) {
        DestroyVolumePopup();
        return;
    }
    g_volumePopupAppUserModelId = CurrentMediaAppUserModelId();
    if (g_volumePopupAppUserModelId.empty()) return;

    if (!g_modModuleHandle) {
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&VolumePopupWindowProc),
            &g_modModuleHandle);
    }
    if (!g_modModuleHandle) return;

    if (!g_volumePopupClassRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = VolumePopupWindowProc;
        windowClass.hInstance = g_modModuleHandle;
        windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        windowClass.lpszClassName = kVolumePopupClass;
        ATOM popupClass = RegisterClassExW(&windowClass);
        if (!popupClass) {
            Wh_Log(L"Volume popup: class registration failed (%u)",
                   GetLastError());
            return;
        }
        g_volumePopupClassRegistered = true;
    }

    const bool popupUsesLightTheme = IsSystemLightTheme();
    const COLORREF popupBackgroundColor =
        popupUsesLightTheme ? RGB(248, 248, 248) : RGB(36, 36, 40);
    g_volumePopupTextColor =
        popupUsesLightTheme ? RGB(28, 28, 28) : RGB(245, 245, 245);
    g_volumePopupTrackColor =
        popupUsesLightTheme ? RGB(190, 190, 196) : RGB(96, 96, 102);
    g_volumePopupDisabledFillColor =
        popupUsesLightTheme ? RGB(150, 150, 156) : RGB(100, 100, 104);

    const DWORD accentColor = GetWindowsAccentColor();
    g_volumePopupFillColor = RGB(
        (accentColor >> 16) & 0xFF,
        (accentColor >> 8) & 0xFF,
        accentColor & 0xFF);

    if (g_volumePopupBrush) {
        DeleteObject(g_volumePopupBrush);
        g_volumePopupBrush = nullptr;
    }
    g_volumePopupBrush = CreateSolidBrush(popupBackgroundColor);
    if (!g_volumePopupBrush) return;

    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR popupMonitor =
        MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    g_volumePopupDpi = GetDpiForMonitorWithFallback(popupMonitor);
    int popupWidth = VolumePopupWidth();
    int popupHeight = VolumePopupHeight();
    int edgeOffset = ScaleVolumePopupMetric(8);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(popupMonitor, &monitorInfo);
    int x = std::clamp(cursor.x - popupWidth / 2,
                       monitorInfo.rcWork.left,
                       monitorInfo.rcWork.right - popupWidth);
    int y = monitorInfo.rcWork.bottom - popupHeight - edgeOffset;
    if (monitorInfo.rcWork.top > monitorInfo.rcMonitor.top) {
        y = monitorInfo.rcWork.top + edgeOffset;
    }

    HWND popupWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        kVolumePopupClass, L"Media app volume", WS_POPUP,
        x, y, popupWidth, popupHeight, g_taskbarWnd, nullptr,
        g_modModuleHandle, nullptr);
    if (!popupWindow) return;

    HWND expectedPopup = nullptr;
    if (!g_volumePopupWindow.compare_exchange_strong(
            expectedPopup, popupWindow, std::memory_order_release,
            std::memory_order_acquire)) {
        DestroyWindow(popupWindow);
        return;
    }

    const DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_ROUND;
    DwmSetWindowAttribute(
        popupWindow, DWMWA_WINDOW_CORNER_PREFERENCE,
        &cornerPreference, sizeof(cornerPreference));
    const BOOL useDarkMode = popupUsesLightTheme ? FALSE : TRUE;
    DwmSetWindowAttribute(
        popupWindow, DWMWA_USE_IMMERSIVE_DARK_MODE,
        &useDarkMode, sizeof(useDarkMode));
    ShowWindow(popupWindow, SW_SHOWNOACTIVATE);
    SetWindowPos(popupWindow, HWND_TOPMOST, x, y, popupWidth, popupHeight,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    SetCapture(popupWindow);
    if (GetCapture() != popupWindow) {
        Wh_Log(L"Volume popup: mouse capture couldn't be acquired");
    }
}

static std::wstring TrimCopy(std::wstring value) {
    const wchar_t* ws = L" \t\r\n";
    size_t first = value.find_first_not_of(ws);
    if (first == std::wstring::npos) return L"";
    size_t last = value.find_last_not_of(ws);
    return value.substr(first, last - first + 1);
}

static bool IsIgnoredMediaApp(const std::wstring& appUserModelId) {
    auto cfg = Cfg();
    const std::wstring ignoredProcesses = cfg->ignoredProcesses;
    if (ignoredProcesses.empty() || appUserModelId.empty()) return false;

    const std::wstring appLower = ToLowerCopy(appUserModelId);
    const std::wstring appStemLower =
        ToLowerCopy(PathFileStem(appUserModelId));
    size_t start = 0;
    while (start <= ignoredProcesses.size()) {
        size_t end = ignoredProcesses.find(L';', start);
        std::wstring item = TrimCopy(ignoredProcesses.substr(
            start, end == std::wstring::npos
                       ? std::wstring::npos
                       : end - start));
        if (!item.empty()) {
            const std::wstring itemLower = ToLowerCopy(item);
            const std::wstring itemStemLower =
                ToLowerCopy(PathFileStem(item));
            if (appLower == itemLower ||
                (!itemStemLower.empty() &&
                 appStemLower == itemStemLower) ||
                (itemLower.size() >= 8 &&
                 ContainsIdentifierToken(appLower, itemLower)) ||
                (!itemStemLower.empty() &&
                 ContainsIdentifierToken(appLower, itemStemLower))) {
                return true;
            }
        }
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return false;
}

static std::wstring GetWindowAppUserModelId(HWND hWnd) {
    static const PROPERTYKEY kAppUserModelIdKey = {
        {0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
        5
    };
    std::wstring result;
    IPropertyStore* store = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&store))) && store) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        if (SUCCEEDED(store->GetValue(kAppUserModelIdKey, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) {
            result = pv.pwszVal;
        }
        PropVariantClear(&pv);
        store->Release();
    }
    return result;
}

static bool AppIdMatchesProcess(
    const std::wstring& appUserModelId, HWND hWnd,
    DWORD* outPid = nullptr, std::wstring* outProcPath = nullptr) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (outPid) *outPid = pid;
    if (!pid || appUserModelId.empty()) return false;

    const std::wstring windowAumid = GetWindowAppUserModelId(hWnd);
    if (SamePackagedAppIdentity(appUserModelId, windowAumid)) {
        return true;
    }

    HANDLE process = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    std::wstring processPath;
    std::wstring processAumid;
    if (process) {
        processPath = QueryProcessImagePath(process);
        processAumid = ProcessApplicationUserModelId(process);
        CloseHandle(process);
    }
    if (outProcPath) *outProcPath = processPath;

    if (SamePackagedAppIdentity(appUserModelId, processAumid)) {
        return true;
    }

    const std::wstring appLower = ToLowerCopy(appUserModelId);
    std::wstring appStemLower =
        ToLowerCopy(PathFileStem(appUserModelId));
    if (appStemLower == L"applicationframehost" ||
        appStemLower == L"explorer") {
        appStemLower.clear();
    }

    const std::wstring processLower = ToLowerCopy(processPath);
    const std::wstring processStemLower =
        ToLowerCopy(PathFileStem(processPath));
    if (processStemLower.empty()) return false;

    return processLower == appLower ||
           (!appStemLower.empty() &&
            processStemLower == appStemLower) ||
           KnownMediaPlayerProcessAlias(
               appLower, processStemLower) ||
           ContainsIdentifierToken(
               appLower, processStemLower) ||
           (!windowAumid.empty() &&
            ContainsIdentifierToken(
                ToLowerCopy(windowAumid), processStemLower));
}

static std::vector<BYTE> RenderIconToBytes(HICON hIcon, int iconSize) {
    if (!hIcon || iconSize <= 0) return {};

    ICONINFO ii{};
    if (!GetIconInfo(hIcon, &ii)) return {};

    BITMAP bm{};
    GetObjectW(ii.hbmColor ? ii.hbmColor : ii.hbmMask, sizeof(bm), &bm);
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);

    int srcW = bm.bmWidth  > 0 ? bm.bmWidth  : iconSize;
    int srcH = bm.bmHeight > 0 ? bm.bmHeight : iconSize;

    HDC screenDC = GetDC(nullptr);
    HDC hdc      = CreateCompatibleDC(screenDC);
    HBITMAP hBmp = CreateCompatibleBitmap(screenDC, srcW, srcH);
    ReleaseDC(nullptr, screenDC);
    HBITMAP hOld = (HBITMAP)SelectObject(hdc, hBmp);

    RECT rc{ 0, 0, srcW, srcH };
    HBRUSH hBr = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hdc, &rc, hBr);
    DrawIconEx(hdc, 0, 0, hIcon, srcW, srcH, 0, nullptr, DI_NORMAL);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = srcW;
    bi.bmiHeader.biHeight      = -srcH;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> src(srcW * srcH * 4, 0);
    GetDIBits(hdc, hBmp, 0, srcH, src.data(), &bi, DIB_RGB_COLORS);
    SelectObject(hdc, hOld);
    DeleteObject(hBmp);
    DeleteDC(hdc);

    for (int i = 0; i + 3 < (int)src.size(); i += 4)
        std::swap(src[i], src[i + 2]);

    if (srcW == iconSize && srcH == iconSize)
        return src;

    std::vector<BYTE> dst(iconSize * iconSize * 4, 0);
    float scaleX = (float)srcW / iconSize;
    float scaleY = (float)srcH / iconSize;
    for (int dy = 0; dy < iconSize; ++dy) {
        for (int dx = 0; dx < iconSize; ++dx) {
            float fx = (dx + 0.5f) * scaleX - 0.5f;
            float fy = (dy + 0.5f) * scaleY - 0.5f;
            int x0 = (int)fx; int y0 = (int)fy;
            int x1 = x0 + 1;  int y1 = y0 + 1;
            x0 = x0 < 0 ? 0 : (x0 > srcW - 1 ? srcW - 1 : x0);
            y0 = y0 < 0 ? 0 : (y0 > srcH - 1 ? srcH - 1 : y0);
            x1 = x1 < 0 ? 0 : (x1 > srcW - 1 ? srcW - 1 : x1);
            y1 = y1 < 0 ? 0 : (y1 > srcH - 1 ? srcH - 1 : y1);
            float wx = fx - (int)fx; float wy = fy - (int)fy;
            int di = (dy * iconSize + dx) * 4;
            for (int c = 0; c < 4; ++c) {
                float v = src[(y0 * srcW + x0) * 4 + c] * (1-wx)*(1-wy)
                        + src[(y0 * srcW + x1) * 4 + c] * wx    *(1-wy)
                        + src[(y1 * srcW + x0) * 4 + c] * (1-wx)* wy
                        + src[(y1 * srcW + x1) * 4 + c] * wx    * wy;
                dst[di + c] = (BYTE)(v + 0.5f);
            }
        }
    }
    return dst;
}

static HICON QueryWindowIconWithTimeout(HWND window, WPARAM iconType) {
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(
            window, WM_GETICON, iconType, 0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result)) {
        return nullptr;
    }
    return reinterpret_cast<HICON>(result);
}

static std::vector<BYTE> FetchAppIconBytes(const std::wstring& appUserModelId, int iconSize) {
    std::vector<BYTE> result;
    if (appUserModelId.empty()) return result;

    auto Render = [&](HICON h, bool own) -> bool {
        if (!h) return false;
        result = RenderIconToBytes(h, iconSize);
        if (own) DestroyIcon(h);
        return !result.empty();
    };

    {
        std::wstring shellPath = L"shell:AppsFolder\\" + appUserModelId;
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(shellPath.c_str(), nullptr, &pidl, 0, nullptr)) && pidl) {
            SHFILEINFOW sfi{};
            DWORD flags = SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON;
            if (SHGetFileInfoW(reinterpret_cast<LPCWSTR>(pidl), 0, &sfi, sizeof(sfi), flags) && sfi.hIcon) {
                CoTaskMemFree(pidl);
                if (Render(sfi.hIcon, true)) return result;
            } else {
                CoTaskMemFree(pidl);
            }
        }
    }
    {
        struct EnumCtx {
            const std::wstring* aumid;
            HICON  exactIcon = nullptr;
            HICON  fuzzyIcon = nullptr;
            DWORD  exactPid  = 0;
            DWORD  fuzzyPid  = 0;
            std::wstring exactPath;
            std::wstring fuzzyPath;
        };
        EnumCtx ctx{};
        ctx.aumid = &appUserModelId;

        EnumWindows([](HWND hWnd, LPARAM lParam) CALLBACK -> BOOL {
            if (!IsWindowVisible(hWnd)) return TRUE;
            auto* c = reinterpret_cast<EnumCtx*>(lParam);

            if (!c->exactIcon) {
                IPropertyStore* pps = nullptr;
                if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&pps)))) {
                    PROPVARIANT var;
                    PropVariantInit(&var);
                    if (SUCCEEDED(pps->GetValue(PKEY_AppUserModel_ID, &var)) && var.vt == VT_LPWSTR) {
                        std::wstring winAumid  = ToLowerCopy(std::wstring(var.pwszVal));
                        std::wstring wantAumid = ToLowerCopy(*c->aumid);
                        if (winAumid == wantAumid) {
                            HICON icon = QueryWindowIconWithTimeout(hWnd, ICON_BIG);
                            if (!icon) icon = QueryWindowIconWithTimeout(hWnd, ICON_SMALL);
                            if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
                            if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
                            if (icon) {
                                c->exactIcon = icon;
                                GetWindowThreadProcessId(hWnd, &c->exactPid);
                                wchar_t path[MAX_PATH]{};
                                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, c->exactPid);
                                if (hProc) { DWORD sz = MAX_PATH; QueryFullProcessImageNameW(hProc, 0, path, &sz); CloseHandle(hProc); }
                                c->exactPath = path;
                            }
                        }
                    }
                    PropVariantClear(&var);
                    pps->Release();
                    if (c->exactIcon) return FALSE;
                }
            }

            if (!c->exactIcon && !c->fuzzyIcon) {
                std::wstring windowAumid = GetWindowAppUserModelId(hWnd);
                if (windowAumid.empty()) {
                    DWORD pid = 0;
                    std::wstring procPath;
                    if (AppIdMatchesProcess(*c->aumid, hWnd, &pid, &procPath)) {
                        HICON icon = QueryWindowIconWithTimeout(hWnd, ICON_BIG);
                        if (!icon) icon = QueryWindowIconWithTimeout(hWnd, ICON_SMALL);
                        if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
                        if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
                        if (icon) {
                            c->fuzzyIcon = icon;
                            c->fuzzyPid  = pid;
                            c->fuzzyPath = procPath;
                        } else if (!c->fuzzyPid) {
                            c->fuzzyPid  = pid;
                            c->fuzzyPath = procPath;
                        }
                    }
                }
            }

            return TRUE;
        }, reinterpret_cast<LPARAM>(&ctx));

        if (ctx.exactIcon && Render(ctx.exactIcon, false)) return result;

        if (ctx.fuzzyIcon) {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ctx.fuzzyPid);
            if (hProc) {
                DWORD exitCode = 0;
                if (GetExitCodeProcess(hProc, &exitCode) && exitCode == STILL_ACTIVE) {
                    CloseHandle(hProc);
                    if (Render(ctx.fuzzyIcon, false)) return result;
                } else {
                    CloseHandle(hProc);
                }
            }
        }

        auto resolvePid = [](DWORD pid) -> std::wstring {
            if (!pid) return {};
            wchar_t path[MAX_PATH]{};
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hProc) { DWORD sz = MAX_PATH; QueryFullProcessImageNameW(hProc, 0, path, &sz); CloseHandle(hProc); }
            return path;
        };

        auto tryExePath = [&](const std::wstring& path) -> bool {
            if (path.empty()) return false;
            SHFILEINFOW sfi{};
            if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) && sfi.hIcon)
                return Render(sfi.hIcon, true);
            if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON) && sfi.hIcon)
                return Render(sfi.hIcon, true);
            return false;
        };

        auto tryExtractIconEx = [&](const std::wstring& path) -> bool {
            if (path.empty()) return false;
            HICON hL = nullptr, hS = nullptr;
            if (ExtractIconExW(path.c_str(), 0, &hL, &hS, 1)) {
                HICON chosen = hL ? hL : hS;
                if (chosen) {
                    bool ok = Render(chosen, false);
                    if (hL) DestroyIcon(hL);
                    if (hS) DestroyIcon(hS);
                    if (ok) return true;
                }
            }
            return false;
        };

        std::wstring ep = ctx.exactPath.empty() ? resolvePid(ctx.exactPid) : ctx.exactPath;
        std::wstring fp = ctx.fuzzyPath.empty() ? resolvePid(ctx.fuzzyPid) : ctx.fuzzyPath;

        if (tryExePath(ep))        return result;
        if (tryExePath(fp))        return result;
        if (tryExtractIconEx(ep))  return result;
        if (tryExtractIconEx(fp))  return result;
    }

    if (appUserModelId.find(L".exe") != std::wstring::npos) {
        std::wstring exePath = appUserModelId;
        if (exePath.size() >= 2 && exePath.front() == L'"' && exePath.back() == L'"')
            exePath = exePath.substr(1, exePath.size() - 2);

        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(exePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) && sfi.hIcon)
            if (Render(sfi.hIcon, true)) return result;

        HICON hL = nullptr, hS = nullptr;
        if (ExtractIconExW(exePath.c_str(), 0, &hL, &hS, 1)) {
            HICON chosen = hL ? hL : hS;
            if (chosen) {
                Render(chosen, false);
                if (hL) DestroyIcon(hL);
                if (hS) DestroyIcon(hS);
                if (!result.empty()) return result;
            }
        }
    }

    return result;
}
static bool IsCurrentMediaSession(
    const GlobalSystemMediaTransportControlsSession& session,
    uint64_t generation) {
    if (!session || g_sessionGeneration.load() != generation) return false;
    std::lock_guard<std::mutex> lock(g_sessionMtx);
    return g_sessionGeneration.load() == generation &&
           g_currentSession == session;
}

static void UpdateMediaTimeline(
    const GlobalSystemMediaTransportControlsSession& session,
    uint64_t generation) {
    if (!IsCurrentMediaSession(session, generation)) return;
    try {
        auto timeline = session.GetTimelineProperties();
        constexpr int64_t kTicksPerSecond = 10'000'000;
        int64_t startTicks = timeline.StartTime().count();
        int64_t endTicks = timeline.EndTime().count();
        int64_t positionTicks = timeline.Position().count();
        int64_t durationSeconds = std::max<int64_t>(
            0, (endTicks - startTicks) / kTicksPerSecond);
        int64_t positionSeconds = std::clamp<int64_t>(
            (positionTicks - startTicks) / kTicksPerSecond,
            0, durationSeconds);

        if (ShouldIgnoreTimelineSampleAfterSeek(
                generation, positionSeconds, durationSeconds)) {
            return;
        }
        if (!IsCurrentMediaSession(session, generation)) return;
        std::lock_guard<std::mutex> lock(g_mediaMtx);
        if (g_sessionGeneration.load() != generation) return;
        g_media.positionSeconds = positionSeconds;
        g_media.durationSeconds = durationSeconds;
        g_media.timelineSampleTick = GetTickCount64();
    } catch (...) {
        Wh_Log(L"Media timeline: unavailable");
    }
}

static void FetchMediaPropertiesAsync() {
    bool expected = false;
    if (!g_mediaPropertiesFetchActive.compare_exchange_strong(expected, true)) {
        g_mediaPropertiesFetchPending = true;
        return;
    }

    bool queued = QueueAsyncTask([]() {
        struct ActiveGuard {
            ~ActiveGuard() {
                g_mediaPropertiesFetchActive = false;
                if (!g_unloading &&
                    g_mediaPropertiesFetchPending.exchange(false)) {
                    FetchMediaPropertiesAsync();
                }
            }
        } activeGuard;
        if (g_unloading) return;

        struct ApartmentGuard {
            bool initialized = false;
            ~ApartmentGuard() {
                if (initialized) winrt::uninit_apartment();
            }
        } apartment;
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            apartment.initialized = true;

            GlobalSystemMediaTransportControlsSession session{nullptr};
            std::wstring aumid;
            uint64_t generation = 0;
            {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                session = g_currentSession;
                generation = g_sessionGeneration.load();
                if (session) {
                    aumid = std::wstring(session.SourceAppUserModelId());
                }
            }
            if (!session || g_unloading) {
                return;
            }

            auto propsResult = WaitForWinrtOperation(
                session.TryGetMediaPropertiesAsync(), 5000,
                L"Media-properties request");
            if (!propsResult) {
                return;
            }
            auto props = *propsResult;
            if (!props || !IsCurrentMediaSession(session, generation)) {
                return;
            }

            std::wstring title(props.Title());
            std::wstring artist(props.Artist());
            std::wstring albumTitle(props.AlbumTitle());
            std::vector<BYTE> thumbBytes;
            uint64_t thumbHash = 0;
            bool thumbnailAdvertised = false;
            bool thumbnailReadFailed = false;

            if (auto thumbRef = props.Thumbnail()) {
                thumbnailAdvertised = true;
                try {
                    auto streamResult = WaitForWinrtOperation(
                        thumbRef.OpenReadAsync(), 3000,
                        L"Album-art stream");
                    if (!streamResult) {
                        throw winrt::hresult_error(
                            HRESULT_FROM_WIN32(ERROR_TIMEOUT));
                    }
                    auto stream = *streamResult;
                    if (!stream) {
                        throw winrt::hresult_error(E_FAIL);
                    }
                    UINT64 size = stream.Size();
                    if (size == 0 || size >= 4 * 1024 * 1024) {
                        throw winrt::hresult_error(E_FAIL);
                    }

                    DataReader reader(stream);
                    auto loaded = WaitForWinrtOperation(
                        reader.LoadAsync(static_cast<UINT32>(size)),
                        3000, L"Album-art read");
                    if (!loaded ||
                        *loaded < static_cast<UINT32>(size)) {
                        reader.DetachStream();
                        throw winrt::hresult_error(
                            HRESULT_FROM_WIN32(ERROR_TIMEOUT));
                    }
                    thumbBytes.resize(static_cast<size_t>(size));
                    reader.ReadBytes(winrt::array_view<BYTE>(thumbBytes));
                    reader.DetachStream();
                    for (size_t i = 0; i < thumbBytes.size(); i += 1024) {
                        thumbHash = thumbHash * 31 + thumbBytes[i];
                    }
                } catch (...) {
                    thumbnailReadFailed = true;
                    thumbBytes.clear();
                    thumbHash = 0;
                }
            }

            Wh_Log(
                L"Album art metadata: advertised=%d readFailed=%d bytes=%zu",
                thumbnailAdvertised ? 1 : 0,
                thumbnailReadFailed ? 1 : 0,
                thumbBytes.size());

            if (!IsCurrentMediaSession(session, generation) || g_unloading) {
                return;
            }

            std::vector<BYTE> appIconBytes;
            std::wstring appIconKey;
            bool forceIconRefresh = false;
            bool oldHasMedia = false;
            std::wstring oldTitle;
            std::wstring oldArtist;
            std::wstring oldAumid;
            std::vector<BYTE> oldThumbnailBytes;
            std::vector<BYTE> oldAppIconBytes;
            std::wstring oldAppIconKey;
            {
                std::lock_guard<std::mutex> lock(g_mediaMtx);
                oldHasMedia = g_media.hasMedia;
                oldTitle = g_media.title;
                oldArtist = g_media.artist;
                oldAumid = g_media.appUserModelId;
                oldThumbnailBytes = g_media.thumbnailBytes;
                oldAppIconBytes = g_media.appIconBytes;
                oldAppIconKey = g_media.appIconKey;
                appIconKey = oldAppIconKey;
                appIconBytes = oldAppIconBytes;
            }
            {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                forceIconRefresh = g_userSwitchedSession;
            }

            bool transitionActive = MediaVisualTransitionActive();
            if (Cfg()->showAppIcon &&
                (aumid != appIconKey || appIconBytes.empty() ||
                 forceIconRefresh ||
                 g_cachedAppIconSize != Cfg()->appIconSize)) {
                auto fetchedAppIcon =
                    FetchAppIconBytes(aumid, Cfg()->appIconSize);
                if (!fetchedAppIcon.empty() || !transitionActive ||
                    oldAppIconBytes.empty()) {
                    appIconBytes = std::move(fetchedAppIcon);
                    appIconKey = aumid;
                }
                g_cachedAppIconSize = Cfg()->appIconSize;
            }

            if (!IsCurrentMediaSession(session, generation) || g_unloading) {
                return;
            }

            bool hasMedia = !title.empty() || !artist.empty();
            bool sameTrackAsCached =
                oldAumid == aumid && oldTitle == title &&
                oldArtist == artist;
            bool trackChanged = hasMedia && oldHasMedia &&
                !sameTrackAsCached;
            if (trackChanged) {
                ClearPendingSeek();
            }

            bool preserveEmptyMetadata =
                !hasMedia && oldHasMedia && transitionActive;
            bool preserveOldArtwork =
                !oldThumbnailBytes.empty() && thumbBytes.empty() &&
                transitionActive &&
                (!sameTrackAsCached || thumbnailReadFailed ||
                 !thumbnailAdvertised);

            {
                std::lock_guard<std::mutex> lock(g_mediaMtx);
                if (g_sessionGeneration.load() != generation) {
                    return;
                }

                if (!preserveEmptyMetadata) {
                    g_media.title = title;
                    g_media.artist = artist;
                    g_media.albumTitle = albumTitle;
                    g_media.hasMedia = hasMedia;
                    g_media.appUserModelId = aumid;
                }

                if (!thumbBytes.empty()) {
                    g_media.thumbnailBytes = std::move(thumbBytes);
                    g_media.thumbnailHash = thumbHash;
                } else if (thumbnailReadFailed && sameTrackAsCached) {
                    // Preserve an already-decoded cover for a transient stream
                    // failure on the same track.
                } else if (preserveOldArtwork || preserveEmptyMetadata) {
                    // Keep the previous cover until the replacement is ready.
                } else if (!preserveEmptyMetadata) {
                    g_media.thumbnailBytes.clear();
                    g_media.thumbnailHash = 0;
                }

                if (Cfg()->showAppIcon) {
                    g_media.appIconBytes = std::move(appIconBytes);
                    g_media.appIconKey = appIconKey;
                }
                Wh_Log(L"Media metadata: hasMedia=%d title='%s' artist='%s' preserved=%d",
                       g_media.hasMedia, g_media.title.c_str(),
                       g_media.artist.c_str(),
                       (preserveEmptyMetadata || preserveOldArtwork) ? 1 : 0);
            }

            if (preserveEmptyMetadata || preserveOldArtwork) {
                // Ensure there is a post-grace refresh. If the player truly has
                // no cover/metadata, the old visual is cleared after the grace
                // rather than being retained indefinitely.
                ArmMetadataRetry(static_cast<DWORD>(
                    std::max(3500, Cfg()->mediaTransitionGraceMs + 1000)));
            } else if (hasMedia) {
                if (thumbnailAdvertised && thumbnailReadFailed) {
                    ArmMetadataRetry(8000);
                } else {
                    ULONGLONG confirmationDeadline =
                        GetTickCount64() + 1500;
                    ULONGLONG currentDeadline =
                        g_metadataRetryUntilTick.load();
                    if (currentDeadline == 0 ||
                        currentDeadline > confirmationDeadline) {
                        g_metadataRetryUntilTick = confirmationDeadline;
                    }
                }
            }
            if (forceIconRefresh) {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                if (g_sessionGeneration.load() == generation &&
                    g_currentSession == session) {
                    g_userSwitchedSession = false;
                }
            }
            UpdateMediaTimeline(session, generation);
            DispatchMediaUpdate();
        } catch (...) {
            Wh_Log(L"Media metadata refresh failed");
        }
    });

    if (!queued) g_mediaPropertiesFetchActive = false;
}

static void FetchPlaybackInfoAsync() {
    bool expected = false;
    if (!g_playbackInfoFetchActive.compare_exchange_strong(expected, true)) {
        g_playbackInfoFetchPending = true;
        return;
    }

    bool queued = QueueAsyncTask([]() {
        struct ActiveGuard {
            ~ActiveGuard() {
                g_playbackInfoFetchActive = false;
                if (!g_unloading &&
                    g_playbackInfoFetchPending.exchange(false)) {
                    FetchPlaybackInfoAsync();
                }
            }
        } activeGuard;
        if (g_unloading) return;

        struct ApartmentGuard {
            bool initialized = false;
            ~ApartmentGuard() {
                if (initialized) winrt::uninit_apartment();
            }
        } apartment;
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            apartment.initialized = true;

            GlobalSystemMediaTransportControlsSession session{nullptr};
            uint64_t generation = 0;
            {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                session = g_currentSession;
                generation = g_sessionGeneration.load();
            }
            if (!session || g_unloading) {
                return;
            }

            auto info = session.GetPlaybackInfo();
            if (!info || !IsCurrentMediaSession(session, generation)) {
                return;
            }

            bool playing = info.PlaybackStatus() ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            bool wasPlaying = false;
            bool controlsAvailable = false;
            bool canPrevious = true;
            bool canNext = true;
            bool canShuffle = true;
            bool canRepeat = true;
            bool canSeek = true;

            try {
                if (auto controls = info.Controls()) {
                    controlsAvailable = true;
                    canPrevious = controls.IsPreviousEnabled();
                    canNext = controls.IsNextEnabled();
                    canShuffle = controls.IsShuffleEnabled();
                    canRepeat = controls.IsRepeatEnabled();
                    canSeek = controls.IsPlaybackPositionEnabled();
                }
            } catch (...) {}

            if (!IsCurrentMediaSession(session, generation)) {
                return;
            }
            {
                std::lock_guard<std::mutex> lock(g_mediaMtx);
                if (g_sessionGeneration.load() != generation) {
                    return;
                }
                wasPlaying = g_media.isPlaying;
                g_media.isPlaying = playing;
                if (playing) {
                    g_currentSessionLastPlayingTick.store(
                        GetTickCount64(), std::memory_order_release);
                }
                g_media.playbackStateKnown = true;
                if (controlsAvailable) {
                    g_media.canSkipPrevious = canPrevious;
                    g_media.canSkipNext = canNext;
                    g_media.canShuffle = canShuffle;
                    g_media.canRepeat = canRepeat;
                    g_media.canSeek = canSeek;
                }
            }

            try {
                auto shuffle = info.IsShuffleActive();
                if (shuffle) g_shuffleEnabled = shuffle.Value();
            } catch (...) {}
            try {
                auto repeat = info.AutoRepeatMode();
                if (repeat) {
                    using Mode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
                    auto value = repeat.Value();
                    g_repeatMode = value == Mode::Track
                        ? RepeatMode::One
                        : (value == Mode::List ? RepeatMode::All
                                              : RepeatMode::Off);
                }
            } catch (...) {}

            UpdateMediaTimeline(session, generation);
            if (playing != wasPlaying) {
                Wh_Log(L"Playback state changed: %s -> %s",
                       wasPlaying ? L"Playing" : L"Not playing",
                       playing ? L"Playing" : L"Not playing");
            }
            DispatchMediaUpdate();
        } catch (...) {
            Wh_Log(L"Playback-state refresh failed");
        }
    });

    if (!queued) g_playbackInfoFetchActive = false;
}

static bool RemoveSessionEventSubscriptions(
    GlobalSystemMediaTransportControlsSession const& session,
    winrt::event_token mediaPropertiesToken,
    winrt::event_token playbackToken,
    winrt::event_token timelineToken) noexcept {
    bool subscriptionsRemoved =
        session || (!mediaPropertiesToken.value &&
                    !playbackToken.value &&
                    !timelineToken.value);
    if (session && mediaPropertiesToken.value) {
        try {
            session.MediaPropertiesChanged(mediaPropertiesToken);
        } catch (...) {
            subscriptionsRemoved = false;
        }
    }
    if (session && playbackToken.value) {
        try {
            session.PlaybackInfoChanged(playbackToken);
        } catch (...) {
            subscriptionsRemoved = false;
        }
    }
    if (session && timelineToken.value) {
        try {
            session.TimelinePropertiesChanged(timelineToken);
        } catch (...) {
            subscriptionsRemoved = false;
        }
    }
    if (!subscriptionsRemoved) {
        RecordMediaEventUnsubscribeFailure(
            L"failed GSMTC session event unsubscription");
    }
    return subscriptionsRemoved;
}

static void DetachCurrentSession() {
    ClearPendingSeek();
    g_currentSessionLastPlayingTick = 0;
    // These modes are optional session-local state. Never display the last
    // player's values while no session is attached or a replacement session
    // is still being queried.
    g_shuffleEnabled = false;
    g_repeatMode = RepeatMode::Off;

    GlobalSystemMediaTransportControlsSession session{nullptr};
    winrt::event_token mediaPropertiesToken{};
    winrt::event_token playbackToken{};
    winrt::event_token timelineToken{};
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        if (!g_currentSession) return;
        session = g_currentSession;
        mediaPropertiesToken = g_evMediaProps;
        playbackToken = g_evPlayback;
        timelineToken = g_evTimeline;
        g_evMediaProps = {};
        g_evPlayback = {};
        g_evTimeline = {};
        g_currentSession = nullptr;
        g_sessionGeneration.fetch_add(1);
    }

    RemoveSessionEventSubscriptions(
        session, mediaPropertiesToken, playbackToken, timelineToken);

    {
        std::lock_guard<std::mutex> lock(g_mediaMtx);
        g_media.canSkipPrevious = true;
        g_media.canSkipNext     = true;
        g_media.canShuffle      = true;
        g_media.canRepeat       = true;
        g_media.canSeek         = true;
        g_media.isPlaying       = false;
        g_media.playbackStateKnown = false;
        g_media.positionSeconds = 0;
        g_media.durationSeconds = 0;
        g_media.timelineSampleTick = 0;
    }
}

static GlobalSystemMediaTransportControlsSession PickBestSession() {
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    GlobalSystemMediaTransportControlsSession currentSession{nullptr};
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        manager = g_sessionMgr;
        currentSession = g_currentSession;
    }
    if (!manager) return nullptr;

    try {
        auto sessions = manager.GetSessions();
        int sessionCount = sessions.Size();
        Wh_Log(L"PickBestSession: Found %d media sessions", sessionCount);

        std::wstring preferredApp = ToLowerCopy(GetPreferredMediaApp());
        GlobalSystemMediaTransportControlsSession firstAvailable{nullptr};
        GlobalSystemMediaTransportControlsSession firstPlaying{nullptr};
        GlobalSystemMediaTransportControlsSession currentAvailable{nullptr};
        bool currentPlaying = false;
        int index = 0;

        for (auto const& session : sessions) {
            try {
                std::wstring appId = session.SourceAppUserModelId().c_str();
                if (IsIgnoredMediaApp(appId)) {
                    Wh_Log(L"PickBestSession: Session %d (%s) is ignored",
                           index, appId.c_str());
                    ++index;
                    continue;
                }

                if (!preferredApp.empty() &&
                    ToLowerCopy(appId) != preferredApp) {
                    ++index;
                    continue;
                }

                auto playback = session.GetPlaybackInfo();
                if (!playback) {
                    Wh_Log(
                        L"PickBestSession: Session %d (%s) has no playback info",
                        index, appId.c_str());
                    ++index;
                    continue;
                }

                auto status = playback.PlaybackStatus();
                bool playing = status ==
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                const wchar_t* statusText = playing ? L"Playing" :
                    (status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused
                         ? L"Paused"
                         : (status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Stopped
                                ? L"Stopped"
                                : L"Unknown"));
                Wh_Log(L"PickBestSession: Session %d (%s) status: %s",
                       index, appId.c_str(), statusText);

                if (!firstAvailable) firstAvailable = session;
                if (playing && !firstPlaying) firstPlaying = session;
                if (currentSession && session == currentSession) {
                    currentAvailable = session;
                    currentPlaying = playing;
                }
            } catch (...) {
                Wh_Log(L"PickBestSession: Exception processing session %d",
                       index);
            }
            ++index;
        }

        // A preferred application is explicit, so select its active session
        // without applying cross-application stickiness.
        if (!preferredApp.empty()) {
            auto selected = firstPlaying ? firstPlaying : firstAvailable;
            if (!selected) {
                Wh_Log(L"PickBestSession: Preferred media app is not available: %s",
                       preferredApp.c_str());
            }
            return selected;
        }

        // Keep an already-playing player selected. Opening a YouTube tab while
        // MusicBee is playing must not make the widget flicker or jump sources.
        if (currentAvailable && currentPlaying) {
            Wh_Log(L"PickBestSession: Keeping the current playing session");
            return currentAvailable;
        }

        // Players can briefly report Paused/Stopped between songs. Hold the
        // current source for a configurable grace period before another active
        // session is allowed to take over.
        if (currentAvailable) {
            ULONGLONG lastPlayingTick =
                g_currentSessionLastPlayingTick.load(std::memory_order_acquire);
            ULONGLONG nowTick = GetTickCount64();
            DWORD graceMs = static_cast<DWORD>(
                std::max(0, Cfg()->sessionSwitchGraceMs));
            if (lastPlayingTick && nowTick >= lastPlayingTick &&
                nowTick - lastPlayingTick < graceMs) {
                Wh_Log(L"PickBestSession: Keeping current session during switch grace");
                return currentAvailable;
            }
        }

        if (firstPlaying) {
            Wh_Log(L"PickBestSession: Selected another actively playing session");
            return firstPlaying;
        }
        if (currentAvailable) {
            Wh_Log(L"PickBestSession: No player is active; keeping current session");
            return currentAvailable;
        }
        if (firstAvailable) {
            Wh_Log(L"PickBestSession: No playing session; selected first available");
            return firstAvailable;
        }

        Wh_Log(L"PickBestSession: No suitable session found");
        return nullptr;
    } catch (...) {
        Wh_Log(L"PickBestSession: Exception occurred");
        return nullptr;
    }
}

static void AttachToSession(GlobalSystemMediaTransportControlsSession session) {
    if (!session) {
        Wh_Log(L"AttachToSession: No session provided, detaching current session");
        ArmMediaVisualTransition();
        DetachCurrentSession();
        // Keep the last stable metadata visible during the short broker/session
        // transition. ShouldHidePlayer hides it after the grace expires if no
        // replacement session appears.
        DispatchMediaUpdate();
        return;
    }

    bool sameSessionReady = false;
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        if (g_currentSession == session) {
            sameSessionReady = g_evMediaProps.value && g_evPlayback.value &&
                               g_evTimeline.value;
        }
    }
    if (sameSessionReady) {
        FetchMediaPropertiesAsync();
        FetchPlaybackInfoAsync();
        return;
    }

    try {
        std::wstring appId = session.SourceAppUserModelId().c_str();
        Wh_Log(L"AttachToSession: Attaching to new session from app: %s", appId.c_str());
    } catch (...) {
        Wh_Log(L"AttachToSession: Attaching to new session (app ID unavailable)");
    }

    ArmMediaVisualTransition();
    DetachCurrentSession();
    g_shuffleEnabled = false;
    g_repeatMode = RepeatMode::Off;

    // Do not carry the previous player's volume glyph/value into the new
    // session while its own audio session is being discovered.
    g_currentVolumePercent = -1;
    g_currentVolumeMuted = false;

    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        // Preserve the last stable title, artwork and app icon until the new
        // session publishes replacements. DetachCurrentSession already reset
        // playback/timeline state, so stale controls cannot act on old media.
        g_media.isPlaying = false;
        g_media.playbackStateKnown = false;
    }
    g_cachedAppIconSize = -1;
    g_scrollResetRequested = true;

    uint64_t generation = 0;
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        g_currentSession = session;
        g_sessionGeneration.fetch_add(1);
        generation = g_sessionGeneration.load();
        ArmMetadataRetry();
    }

    // Prime playback state synchronously on the serialized media thread. The
    // metadata and playback workers can otherwise finish out of order and
    // briefly present an already-playing song as paused after sign-in/restart.
    try {
        auto initialPlayback = session.GetPlaybackInfo();
        if (initialPlayback &&
            IsCurrentMediaSession(session, generation)) {
            bool initialPlaying =
                initialPlayback.PlaybackStatus() ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            std::lock_guard<std::mutex> lock(g_mediaMtx);
            if (g_sessionGeneration.load() == generation) {
                g_media.isPlaying = initialPlaying;
                g_media.playbackStateKnown = true;
                if (initialPlaying) {
                    g_currentSessionLastPlayingTick.store(
                        GetTickCount64(), std::memory_order_release);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"AttachToSession: initial playback snapshot unavailable");
    }

    winrt::event_token mediaPropertiesToken{};
    winrt::event_token playbackToken{};
    winrt::event_token timelineToken{};
    try {
        mediaPropertiesToken = session.MediaPropertiesChanged([](auto const&, auto const&) {
            MediaEventCallbackGuard callbackGuard;
            Wh_Log(L"MediaPropertiesChanged event fired");
            if (!g_unloading) {
                ArmMediaVisualTransition();
                ArmMetadataRetry(8000);
                FetchMediaPropertiesAsync();
            }
        });
        playbackToken = session.PlaybackInfoChanged([](auto const&, auto const&) {
            MediaEventCallbackGuard callbackGuard;
            Wh_Log(L"PlaybackInfoChanged event fired");
            if (!g_unloading) FetchPlaybackInfoAsync();
        });
        timelineToken = session.TimelinePropertiesChanged([](auto const&, auto const&) {
            MediaEventCallbackGuard callbackGuard;
            if (!g_unloading) FetchPlaybackInfoAsync();
        });
    } catch (...) {
        RemoveSessionEventSubscriptions(
            session, mediaPropertiesToken, playbackToken, timelineToken);
        {
            std::lock_guard<std::mutex> lock(g_sessionMtx);
            if (g_currentSession == session &&
                g_sessionGeneration.load() == generation) {
                g_currentSession = nullptr;
                g_sessionGeneration.fetch_add(1);
            }
        }
        Wh_Log(L"AttachToSession: Failed to attach event handlers");
        DispatchMediaUpdate();
        return;
    }

    bool committed = false;
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        if (g_currentSession == session &&
            g_sessionGeneration.load() == generation) {
            g_evMediaProps = mediaPropertiesToken;
            g_evPlayback = playbackToken;
            g_evTimeline = timelineToken;
            committed = true;
        }
    }
    if (!committed) {
        RemoveSessionEventSubscriptions(
            session, mediaPropertiesToken, playbackToken, timelineToken);
        return;
    }

    Wh_Log(L"AttachToSession: Event handlers attached successfully");
    FetchMediaPropertiesAsync();
    FetchPlaybackInfoAsync();
}

static void OnSessionsChanged() {
    if (g_unloading) return;
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        manager = g_sessionMgr;
    }
    if (!manager) return;

    bool userSwitched = false;
    bool forceRefresh = g_forceSessionRefresh.exchange(false);
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        userSwitched = g_userSwitchedSession;
    }

    Wh_Log(L"OnSessionsChanged: userSwitched=%d, forceRefresh=%d", userSwitched, forceRefresh);

    if (forceRefresh || !userSwitched) {
        try {
            auto newSession = PickBestSession();
            if (newSession) {
                try {
                    auto pb = newSession.GetPlaybackInfo();
                    if (pb && pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                        Wh_Log(L"OnSessionsChanged: Found actively playing session, resetting user switch flag");
                        std::lock_guard<std::mutex> lk(g_sessionMtx);
                        g_userSwitchedSession = false;
                    }
                } catch (...) {}
            } else {
                Wh_Log(L"OnSessionsChanged: No session found, resetting user switch flag");
                std::lock_guard<std::mutex> lk(g_sessionMtx);
                g_userSwitchedSession = false;
            }
            AttachToSession(newSession);
        } catch (...) {
            Wh_Log(L"OnSessionsChanged: Exception occurred");
        }
    } else {
        Wh_Log(L"OnSessionsChanged: Skipping due to user switch - checking if current session still exists");
        try {
            bool currentSessionExists = false;
            GlobalSystemMediaTransportControlsSession currentSession{nullptr};
            {
                std::lock_guard<std::mutex> lk(g_sessionMtx);
                currentSession = g_currentSession;
            }

            if (currentSession) {
                auto sessions = manager.GetSessions();
                for (auto const& s : sessions) {
                    if (s == currentSession) {
                        currentSessionExists = true;
                        break;
                    }
                }
            }

            if (!currentSessionExists) {
                Wh_Log(L"OnSessionsChanged: Current session no longer exists, resetting user switch flag and picking new session");
                {
                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                    g_userSwitchedSession = false;
                }
                auto newSession = PickBestSession();
                AttachToSession(newSession);
            } else {
                Wh_Log(L"OnSessionsChanged: Current session still exists, keeping it");
            }
        } catch (...) {
            Wh_Log(L"OnSessionsChanged: Exception checking current session validity");
        }
    }
}

static DWORD MediaThreadMain() {
    Wh_Log(L"MediaThreadProc: Starting media thread");
    HANDLE stopEvent = SnapshotWorkerEventHandle(g_mediaStopEvent);
    HANDLE refreshEvent = SnapshotWorkerEventHandle(g_mediaRefreshEvent);
    if (!stopEvent || !refreshEvent) {
        Wh_Log(L"MediaThreadProc: worker events are unavailable");
        return 0;
    }
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    } catch (...) {
        Wh_Log(L"MediaThreadProc: failed to initialize COM apartment");
        return 0;
    }
    bool callCancellationEnabled =
        SUCCEEDED(CoEnableCallCancellation(nullptr));

    struct MediaApartmentCleanup {
        bool active;
        bool callCancellationEnabled;
        ~MediaApartmentCleanup() noexcept {
            if (callCancellationEnabled) {
                CoDisableCallCancellation(nullptr);
            }
            if (!active) return;
            try { winrt::uninit_apartment(); } catch (...) {}
        }
    } apartmentCleanup{apartmentInitialized, callCancellationEnabled};

    struct MediaThreadStateCleanup {
        ~MediaThreadStateCleanup() noexcept {
            GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
            try {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                manager = g_sessionMgr;
            } catch (...) {}
            bool managerSubscriptionsRemoved = true;
            if (manager) {
                try {
                    if (g_evSessionsChanged.value) {
                        manager.SessionsChanged(g_evSessionsChanged);
                    }
                } catch (...) {
                    managerSubscriptionsRemoved = false;
                }
                try {
                    if (g_evCurrentChanged.value) {
                        manager.CurrentSessionChanged(g_evCurrentChanged);
                    }
                } catch (...) {
                    managerSubscriptionsRemoved = false;
                }
            } else if (g_evSessionsChanged.value ||
                       g_evCurrentChanged.value) {
                managerSubscriptionsRemoved = false;
            }
            if (!managerSubscriptionsRemoved) {
                RecordMediaEventUnsubscribeFailure(
                    L"failed GSMTC manager event unsubscription");
            }
            g_evSessionsChanged = {};
            g_evCurrentChanged = {};
            try { DetachCurrentSession(); } catch (...) {}
            try {
                std::lock_guard<std::mutex> lock(g_mediaMtx);
                g_media = MediaState{};
            } catch (...) {}
            g_scrollResetRequested.store(true, std::memory_order_release);
            try { DispatchMediaUpdate(); } catch (...) {}
            try {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                g_sessionMgr = nullptr;
            } catch (...) {}
        }
    } stateCleanup;

    DWORD retryDelayMs = 250;
    while (!g_unloading &&
           WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {
        GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
        g_evSessionsChanged = {};
        g_evCurrentChanged = {};
        bool managerReady = false;

        try {
            Wh_Log(L"MediaThreadProc: Requesting session manager");
            auto operation =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
            if (WaitForSingleObject(stopEvent, 0) ==
                WAIT_OBJECT_0) {
                try { operation.Cancel(); } catch (...) {}
                break;
            }
            auto managerResult = WaitForWinrtOperation(
                operation, 5000, L"Windows media-session manager");
            if (!managerResult) {
                throw winrt::hresult_error(
                    HRESULT_FROM_WIN32(ERROR_TIMEOUT));
            }
            manager = *managerResult;
            if (!manager) throw winrt::hresult_error(E_FAIL);
            {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                g_sessionMgr = manager;
            }
            managerReady = true;
            Wh_Log(L"MediaThreadProc: Session manager obtained");

            g_evSessionsChanged =
                manager.SessionsChanged([](auto const&, auto const&) {
                    MediaEventCallbackGuard callbackGuard;
                    RequestMediaSessionRefresh();
                });
            g_evCurrentChanged =
                manager.CurrentSessionChanged([](auto const&, auto const&) {
                    MediaEventCallbackGuard callbackGuard;
                    RequestMediaSessionRefresh();
                });

            // The first refresh runs immediately on this serialized media
            // thread, avoiding an extra startup round trip.
            OnSessionsChanged();
            retryDelayMs = 250;

            while (!g_unloading) {
                HANDLE handles[] = {
                    stopEvent, refreshEvent};
                DWORD waitResult = WaitForMultipleObjects(
                    ARRAYSIZE(handles), handles, FALSE, 30000);
                if (waitResult == WAIT_OBJECT_0) break;
                if (waitResult == WAIT_OBJECT_0 + 1) {
                    OnSessionsChanged();
                } else if (waitResult == WAIT_TIMEOUT) {
                    // Events drive all normal session and metadata updates.
                    // A low-frequency, metadata-free broker call only verifies
                    // that the WinRT broker is still alive; failure falls into
                    // the reconnect path below.
                    auto sessions = manager.GetSessions();
                    (void)sessions.Size();
                }
                if (waitResult == WAIT_FAILED) {
                    throw winrt::hresult_error(
                        HRESULT_FROM_WIN32(GetLastError()));
                }
            }
        } catch (const winrt::hresult_error& error) {
            Wh_Log(L"MediaThreadProc: broker/session failure 0x%08X",
                   static_cast<uint32_t>(error.code()));
        } catch (...) {
            Wh_Log(L"MediaThreadProc: transient session-manager failure");
        }

        bool managerSubscriptionsRemoved = true;
        if (managerReady && manager) {
            try {
                if (g_evSessionsChanged.value) {
                    manager.SessionsChanged(g_evSessionsChanged);
                }
            } catch (...) {
                managerSubscriptionsRemoved = false;
            }
            try {
                if (g_evCurrentChanged.value) {
                    manager.CurrentSessionChanged(g_evCurrentChanged);
                }
            } catch (...) {
                managerSubscriptionsRemoved = false;
            }
        } else if (g_evSessionsChanged.value ||
                   g_evCurrentChanged.value) {
            managerSubscriptionsRemoved = false;
        }
        if (!managerSubscriptionsRemoved) {
            RecordMediaEventUnsubscribeFailure(
                L"failed GSMTC manager event unsubscription");
        }
        g_evSessionsChanged = {};
        g_evCurrentChanged = {};
        ArmMediaVisualTransition();
        DetachCurrentSession();
        g_scrollResetRequested = true;
        DispatchMediaUpdate();
        {
            std::lock_guard<std::mutex> lock(g_sessionMtx);
            g_sessionMgr = nullptr;
        }

        if (g_unloading ||
            WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
            break;
        }
        Wh_Log(L"MediaThreadProc: retrying broker in %u ms",
               retryDelayMs);
        if (WaitForSingleObject(stopEvent, retryDelayMs) ==
            WAIT_OBJECT_0) {
            break;
        }
        retryDelayMs = std::min<DWORD>(retryDelayMs * 2, 5000);
    }

    Wh_Log(L"MediaThreadProc: Media thread ended");
    return 0;
}

static DWORD WINAPI MediaThreadProc(void*) noexcept {
    g_mediaThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    DWORD result = 0;
    try {
        result = MediaThreadMain();
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"MediaThreadProc: unhandled WinRT failure 0x%08X",
               static_cast<uint32_t>(error.code()));
    } catch (...) {
        Wh_Log(L"MediaThreadProc: unhandled worker exception");
    }
    g_mediaThreadId.store(0, std::memory_order_release);
    return result;
}

static void StartMediaThread() {
    if (g_mediaThread) return;
    bool stopEventCreated =
        CreateWorkerEventHandle(g_mediaStopEvent, true, false);
    bool refreshEventCreated =
        CreateWorkerEventHandle(g_mediaRefreshEvent, false, false);
    if (!stopEventCreated || !refreshEventCreated) {
        CloseWorkerEventHandle(g_mediaStopEvent);
        CloseWorkerEventHandle(g_mediaRefreshEvent);
        return;
    }
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    if (!g_mediaThread) {
        CloseWorkerEventHandle(g_mediaStopEvent);
        CloseWorkerEventHandle(g_mediaRefreshEvent);
    }
}
static bool StopMediaThread(bool shutdownCleanup = false) {
    SignalWorkerEventHandle(g_mediaStopEvent);
    DWORD mediaThreadId =
        g_mediaThreadId.load(std::memory_order_acquire);
    if (mediaThreadId) CoCancelCall(mediaThreadId, 0);

    bool threadStopped = true;
    constexpr DWORD timeoutMs = 5000;
    if (g_mediaThread) {
        threadStopped = WaitForThreadExit(g_mediaThread, timeoutMs);
        if (!threadStopped) {
            if (shutdownCleanup) {
                Wh_Log(
                    L"Media thread exceeded the unload deadline; retaining "
                    L"the module if it remains active");
            } else {
                Wh_Log(
                    L"Media thread didn't stop within the "
                    L"settings-change deadline");
            }
        } else {
            CloseHandle(g_mediaThread);
            g_mediaThread = nullptr;
        }
    }

    bool callbacksIdle = threadStopped &&
        WaitForMediaEventCallbacksIdle(timeoutMs);
    if (!callbacksIdle) {
        if (shutdownCleanup) {
            Wh_Log(
                L"Media callbacks didn't drain before the unload deadline");
        } else {
            Wh_Log(
                L"Media callbacks didn't drain within the "
                L"settings-change deadline");
        }
    }

    if (threadStopped && callbacksIdle) {
        CloseWorkerEventHandle(g_mediaStopEvent);
        CloseWorkerEventHandle(g_mediaRefreshEvent);
    }
    return threadStopped && callbacksIdle;
}

static HANDLE g_timerThread    = nullptr;
static std::atomic<DWORD> g_timerThreadId{0};
static HANDLE g_timerStopEvent = nullptr;
static HANDLE g_timerUpdateEvent = nullptr;

[[clang::no_destroy]] static winrt::Windows::UI::Xaml::DispatcherTimer g_scrollDispatcherTimer{nullptr};
static winrt::event_token                        g_scrollDispatcherTimerToken{};
static bool                                       g_scrollDispatcherTimerHasToken = false;
static int                                        g_scrollDispatcherTimerIntervalMs = 0;
static ULONGLONG                                  g_nextProgressAnimationTick = 0;
static std::atomic<bool>                          g_scrollDispatcherTimerRegistered{false};
static std::atomic<HWND>                          g_scrollDispatcherTimerOwnerWindow{nullptr};
static std::atomic<DWORD>                         g_scrollDispatcherTimerOwnerThreadId{0};
static std::atomic<HANDLE>                        g_scrollDispatcherTimerOwnerThreadHandle{nullptr};

static void TickScrollState(TextScrollState& s, int stepPx, int pauseMs, const std::wstring& mode) {
    if (!s.active) return;

    if (s.pausing) {
        s.pauseTick -= s.tickMs;
        if (s.pauseTick <= 0) {
            s.pausing = false;
            s.pauseTick = 0;
        }
        return;
    }

    double maxOff = s.textWidth - s.viewWidth + 10.0;
    if (maxOff < 0.0) maxOff = 0.0;

    if (mode == L"marquee") {
        // A true single-copy marquee: move only right-to-left until every
        // character has left the viewport, then restart beyond the right edge.
        // There is never a reflected/bouncing pass or a duplicated title.
        s.offset -= stepPx;
        if (s.offset <= -s.textWidth) {
            s.offset = s.viewWidth + Cfg()->loopGap;
            s.pausing = true;
            s.pauseTick = pauseMs;
        }
    } else {

        if (s.forward) {
            s.offset += stepPx;
            if (s.offset >= maxOff) {
                s.offset = maxOff;
                s.forward = false;
                s.pausing = true;
                s.pauseTick = pauseMs;
            }
        } else {
            s.offset -= stepPx;
            if (s.offset <= 0.0) {
                s.offset = 0.0;
                s.forward = true;
                s.pausing = true;
                s.pauseTick = pauseMs;
            }
        }
    }
}

static void UpdateScrollTransforms();
static void RefreshProgressBars();

static bool HasActiveScrollAnimation() {
    if (g_primaryVisualState->titleScroll.active ||
        g_primaryVisualState->artistScroll.active) {
        return true;
    }
    for (const auto& instance : SnapshotMirrorPlayers()) {
        if (!instance || !instance->visualState ||
            instance->ownerThreadId != GetCurrentThreadId()) {
            continue;
        }
        if (instance->visualState->titleScroll.active ||
            instance->visualState->artistScroll.active) {
            return true;
        }
    }
    return false;
}

static bool HasActiveProgressAnimation() {
    if (!Cfg()->showProgressBar) return false;
    std::lock_guard<std::mutex> lock(g_mediaMtx);
    return g_media.hasMedia && g_media.isPlaying &&
           g_media.durationSeconds > 0;
}

static void SetScrollDispatcherTimerInterval(int intervalMs) {
    if (!g_scrollDispatcherTimer ||
        g_scrollDispatcherTimerIntervalMs == intervalMs) {
        return;
    }
    g_scrollDispatcherTimer.Interval(
        winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(intervalMs)});
    g_scrollDispatcherTimerIntervalMs = intervalMs;
}

static void RefreshScrollDispatcherTimerCadence() {
    if (!g_scrollDispatcherTimerRegistered.load(
            std::memory_order_acquire) ||
        !g_scrollDispatcherTimer ||
        g_scrollDispatcherTimerOwnerThreadId.load(
            std::memory_order_acquire) != GetCurrentThreadId()) {
        return;
    }

    if (HasActiveScrollAnimation()) {
        SetScrollDispatcherTimerInterval(16);
    } else if (HasActiveProgressAnimation()) {
        SetScrollDispatcherTimerInterval(250);
    } else {
        // Keep one inexpensive idle heartbeat so a newly published media state
        // can resume progress animation without a cross-thread hook dispatch.
        SetScrollDispatcherTimerInterval(1000);
    }
}

static void ScrollTimerTick(winrt::Windows::Foundation::IInspectable const&,
                             winrt::Windows::Foundation::IInspectable const&) {
    if (TaskbarXamlCallbacksSuppressed()) return;
    try {
        bool needsScroll = HasActiveScrollAnimation();
        bool needsProgress = HasActiveProgressAnimation();

        if (needsScroll) {
            int stepPx = std::max(1, Cfg()->scrollSpeed);
            int pauseMs = Cfg()->scrollPauseDuration;
            TickScrollState(
                g_primaryVisualState->titleScroll, stepPx, pauseMs, Cfg()->scrollMode);
            TickScrollState(
                g_primaryVisualState->artistScroll, stepPx, pauseMs, Cfg()->scrollMode);
            UpdateScrollTransforms();
        }

        ULONGLONG now = GetTickCount64();
        if (needsProgress) {
            if (!g_nextProgressAnimationTick ||
                now >= g_nextProgressAnimationTick) {
                RefreshProgressBars();
                g_nextProgressAnimationTick = now + 250;
            }
        } else {
            g_nextProgressAnimationTick = 0;
        }

        RefreshScrollDispatcherTimerCadence();
    } catch (...) {
        Wh_Log(L"Taskbar UI animation timer callback failed");
    }
}

static bool StartScrollTimer() {
    if (g_scrollDispatcherTimerRegistered.load(std::memory_order_acquire)) {
        return true;
    }
    HWND hWnd = g_playerOwnerWindow.load();
    DWORD ownerThreadId = g_playerOwnerThreadId.load();
    HANDLE ownerThreadHandle = g_playerOwnerThreadHandle.load();
    DWORD windowThreadId = 0;
    if (!IsOriginalTaskbarThreadAlive(
            ownerThreadHandle, ownerThreadId) ||
        !IsCurrentProcessTaskbarWindow(
            hWnd, &windowThreadId, nullptr) ||
        windowThreadId != ownerThreadId) {
        return false;
    }

    struct StartScrollTimerWork {
        HWND ownerWindow;
        DWORD ownerThreadId;
        bool started = false;
    } work{hWnd, ownerThreadId};

    bool dispatched = RunFromWindowThread(hWnd, [](void* parameter) {
        auto* workItem = static_cast<StartScrollTimerWork*>(parameter);
        if (g_scrollDispatcherTimerRegistered.load(
                std::memory_order_acquire)) {
            workItem->started = true;
            return;
        }
        if (GetCurrentThreadId() != workItem->ownerThreadId) {
            return;
        }
        HANDLE ownerThreadHandle = OpenTaskbarOwnerThreadHandle(
            GetCurrentThreadId());
        if (!ownerThreadHandle) return;
        g_scrollDispatcherTimerOwnerWindow = workItem->ownerWindow;
        g_scrollDispatcherTimerOwnerThreadId = GetCurrentThreadId();
        g_scrollDispatcherTimerOwnerThreadHandle = ownerThreadHandle;
        g_scrollDispatcherTimerRegistered = true;
        try {
            if (!g_scrollDispatcherTimer) {
                g_scrollDispatcherTimer = winrt::Windows::UI::Xaml::DispatcherTimer();
                g_scrollDispatcherTimer.Interval(
                    winrt::Windows::Foundation::TimeSpan{
                        std::chrono::milliseconds(250)});
                g_scrollDispatcherTimerIntervalMs = 250;
                g_scrollDispatcherTimerToken = g_scrollDispatcherTimer.Tick(&ScrollTimerTick);
                g_scrollDispatcherTimerHasToken = true;
            }
            g_scrollDispatcherTimer.Start();
            workItem->started = true;
        } catch (...) {
            bool rolledBack = true;
            try {
                if (g_scrollDispatcherTimer) {
                    try { g_scrollDispatcherTimer.Stop(); } catch (...) {}
                    if (g_scrollDispatcherTimerHasToken) {
                        g_scrollDispatcherTimer.Tick(
                            g_scrollDispatcherTimerToken);
                        g_scrollDispatcherTimerHasToken = false;
                    }
                    g_scrollDispatcherTimer = nullptr;
                    g_scrollDispatcherTimerIntervalMs = 0;
                    g_nextProgressAnimationTick = 0;
                }
            } catch (...) {
                rolledBack = false;
            }
            if (rolledBack) {
                g_scrollDispatcherTimerOwnerWindow = nullptr;
                g_scrollDispatcherTimerOwnerThreadId = 0;
                CloseOwnedThreadHandle(
                    g_scrollDispatcherTimerOwnerThreadHandle);
                g_scrollDispatcherTimerRegistered = false;
            }
        }
    }, &work);
    return dispatched && work.started;
}

static bool StopScrollTimer(bool shutdownCleanup = false) {
    if (!g_scrollDispatcherTimerRegistered.load(std::memory_order_acquire)) {
        return true;
    }

    DWORD ownerThreadId = g_scrollDispatcherTimerOwnerThreadId.load();
    HANDLE ownerThreadHandle =
        g_scrollDispatcherTimerOwnerThreadHandle.load();
    HWND hWnd = g_scrollDispatcherTimerOwnerWindow.load();
    DWORD currentOwnerThreadId = 0;
    if (!IsOriginalTaskbarThreadAlive(ownerThreadHandle, ownerThreadId)) {
        hWnd = nullptr;
    } else if (!IsCurrentProcessTaskbarWindow(
            hWnd, &currentOwnerThreadId, nullptr) ||
        currentOwnerThreadId != ownerThreadId) {
        hWnd = FindCurrentProcessTaskbarWndForThread(ownerThreadId);
    }

    struct StopScrollTimerWork {
        bool stopped = false;
    } work;

    auto stop = [](void* parameter) {
        auto* workItem = static_cast<StopScrollTimerWork*>(parameter);
        try {
            if (g_scrollDispatcherTimer) {
                g_scrollDispatcherTimer.Stop();
                if (g_scrollDispatcherTimerHasToken) {
                    g_scrollDispatcherTimer.Tick(
                        g_scrollDispatcherTimerToken);
                    g_scrollDispatcherTimerHasToken = false;
                }
                g_scrollDispatcherTimer = nullptr;
                g_scrollDispatcherTimerIntervalMs = 0;
                g_nextProgressAnimationTick = 0;
            }
            g_scrollDispatcherTimerOwnerWindow = nullptr;
            g_scrollDispatcherTimerOwnerThreadId = 0;
            CloseOwnedThreadHandle(
                g_scrollDispatcherTimerOwnerThreadHandle);
            g_scrollDispatcherTimerRegistered = false;
            workItem->stopped = true;
        } catch (...) {}
    };

    bool dispatched = false;
    if (ownerThreadId && ownerThreadId == GetCurrentThreadId()) {
        WindowDispatchShutdownScope shutdownScope(shutdownCleanup);
        dispatched = InvokeWindowThreadProcSafely(stop, &work);
    } else if (hWnd && IsWindow(hWnd)) {
        dispatched = shutdownCleanup
            ? RunFromWindowThreadForCleanup(hWnd, stop, &work)
            : RunFromWindowThread(hWnd, stop, &work);
    } else {
        Wh_Log(L"Scroll timer: owner UI thread is unavailable; cleanup deferred");
    }
    return dispatched && work.stopped;
}

static void ResetScrollState(TextScrollState& s) {
    s.offset    = 0.0;
    s.textWidth = 0.0;
    s.viewWidth = 0.0;
    s.forward   = true;
    s.active    = false;
    s.pausing  = true;
    s.pauseTick = Cfg()->scrollPauseDuration;
}

static constexpr wchar_t kTitleScrollViewName[]  = L"TaskbarMediaPresence_TitleScrollView";
static constexpr wchar_t kArtistScrollViewName[] = L"TaskbarMediaPresence_ArtistScrollView";
static constexpr wchar_t kTitleCloneName[]       = L"TaskbarMediaPresence_TitleClone";
static constexpr wchar_t kArtistCloneName[]      = L"TaskbarMediaPresence_ArtistClone";
static constexpr wchar_t kPanelGridName[]        = L"TaskbarMediaPresence_PanelGrid";

static double GetAvailableScrollTextAreaWidth(Grid const& playerGrid) {
    try {
        if (Cfg()->playerMaxWidth <= 0) return 0.0;

        if (!playerGrid) return 0.0;
        auto panelFe = FindChildByName(playerGrid, kPanelGridName);
        if (!panelFe) return 0.0;
        auto panelGrid = panelFe.try_as<Grid>();
        if (!panelGrid) return 0.0;

        double total = panelGrid.ActualWidth();
        if (total <= 0.0) return 0.0;

        auto cols = panelGrid.ColumnDefinitions();
        double used = 0.0;
        for (uint32_t i = 0; i < cols.Size(); i++) {
            if (i == 1) continue;
            used += cols.GetAt(i).ActualWidth();
        }

        double leftMargin  = (double)Cfg()->textAreaLeftMargin;
        double rightMargin = (double)Cfg()->textAreaRightMargin;

        double available = total - used - leftMargin - rightMargin;
        if (available < 0.0) available = 0.0;
        return available;
    } catch (...) {
        return 0.0;
    }
}


static void UpdateScrollTransformsForGrid(
    Grid const& playerGrid,
    std::shared_ptr<PlayerVisualState> const& visualState) {
    if (!playerGrid || !visualState ||
        (!Cfg()->enableTitleScrolling &&
         !Cfg()->enableArtistScrolling)) {
        return;
    }
    bool isMarquee = (Cfg()->scrollMode == L"marquee");

    if (Cfg()->enableTitleScrolling) {
        try {
            if (auto fe = FindChildByName(playerGrid, kTitleScrollViewName)) {
                if (auto cv = fe.try_as<Canvas>()) {
                    int n = VisualTreeHelper::GetChildrenCount(cv);
                    for (int i = 0; i < n; i++) {
                        auto child = VisualTreeHelper::GetChild(cv, i);
                        if (auto tb = child.try_as<TextBlock>()) {
                            auto name = tb.Name();
                            if (name == kTitleBlockName) {
                                Canvas::SetLeft(tb, isMarquee
                                    ? visualState->titleScroll.offset
                                    : -visualState->titleScroll.offset);
                            } else if (name == kTitleCloneName) {
                                tb.Visibility(Visibility::Collapsed);
                            }
                        }
                    }
                }
            }
        } catch (...) {}
    }

    if (Cfg()->enableArtistScrolling) {
        try {
            if (auto fe = FindChildByName(playerGrid, kArtistScrollViewName)) {
                if (auto cv = fe.try_as<Canvas>()) {
                    int n = VisualTreeHelper::GetChildrenCount(cv);
                    for (int i = 0; i < n; i++) {
                        auto child = VisualTreeHelper::GetChild(cv, i);
                        if (auto ab = child.try_as<TextBlock>()) {
                            auto name = ab.Name();
                            if (name == kArtistBlockName) {
                                Canvas::SetLeft(ab, isMarquee
                                    ? visualState->artistScroll.offset
                                    : -visualState->artistScroll.offset);
                            } else if (name == kArtistCloneName) {
                                ab.Visibility(Visibility::Collapsed);
                            }
                        }
                    }
                }
            }
        } catch (...) {}
    }
}

static void UpdateScrollTransforms() {
    if (TaskbarXamlCallbacksSuppressed()) return;
    Grid primaryGrid = g_playerGrid;
    if (primaryGrid) {
        UpdateScrollTransformsForGrid(primaryGrid, g_primaryVisualState);
    }

    auto mirrors = SnapshotMirrorPlayers();
    for (const auto& instance : mirrors) {
        if (!instance || !instance->playerGrid || !instance->visualState) {
            continue;
        }
        // Never synchronously cross-dispatch from a 60 Hz UI timer. Mirrors
        // that share the primary taskbar UI thread animate here; mirrors on a
        // separate dispatcher keep their safely clipped static text.
        if (instance->ownerThreadId != GetCurrentThreadId()) continue;
        try {
            int stepPx = std::max(1, Cfg()->scrollSpeed);
            int pauseMs = Cfg()->scrollPauseDuration;
            TickScrollState(instance->visualState->titleScroll, stepPx,
                            pauseMs, Cfg()->scrollMode);
            TickScrollState(instance->visualState->artistScroll, stepPx,
                            pauseMs, Cfg()->scrollMode);
            UpdateScrollTransformsForGrid(
                instance->playerGrid, instance->visualState);
        } catch (...) {}
    }
}

static void DispatchMediaUpdate() {
    if (g_unloading || g_applyingSettings) return;

    g_needsUiUpdate = true;
    SignalDiscordPresenceUpdate();
    SignalWorkerEventHandle(g_timerUpdateEvent);
}

static void RefreshPlayerContents();
static void UpdateVisibility();
static void RefreshThemeColors();
static winrt::Windows::UI::Color ProgressFillColor();
static winrt::Windows::UI::Color ProgressTrackColor();
static bool RemovePlayerGrid(bool shutdownCleanup = false);
static bool InjectPlayerGrid();
static bool DeactivatePlayerXamlCallbacks(
    Grid const& playerGrid,
    std::shared_ptr<PlayerVisualState> const& visualState);
static uint64_t GetTaskbarTopologyFingerprint();
static void QueueQuickMonitorRebuild();


struct TaskbarTopologyNotificationState {
    HANDLE updateEvent = nullptr;
    bool changePending = false;
};

static constexpr wchar_t kTaskbarTopologyNotificationClass[] =
    L"TaskbarMediaPresenceTopologyNotification";

static LRESULT CALLBACK TaskbarTopologyNotificationWindowProc(
    HWND window, UINT message, WPARAM wParam, LPARAM lParam) noexcept {
    auto* state = reinterpret_cast<TaskbarTopologyNotificationState*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        state = create
            ? static_cast<TaskbarTopologyNotificationState*>(
                  create->lpCreateParams)
            : nullptr;
        if (!state) return FALSE;

        SetLastError(ERROR_SUCCESS);
        LONG_PTR previous = SetWindowLongPtrW(
            window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
        if (!previous && GetLastError() != ERROR_SUCCESS) {
            return FALSE;
        }
    }

    if (state) {
        switch (message) {
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            g_taskbarCenteredState.store(-1, std::memory_order_release);
            [[fallthrough]];
        case WM_DEVICECHANGE:
        case WM_POWERBROADCAST:
            state->changePending = true;
            if (state->updateEvent) {
                SetEvent(state->updateEvent);
            }
            break;
        }
    }

    if (message == WM_NCDESTROY) {
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

struct TaskbarTopologyNotificationWindow {
    HWND window = nullptr;
    HMODULE moduleHandle = nullptr;
    bool classRegistered = false;
    TaskbarTopologyNotificationState state;

    void ReleaseClassRegistration() noexcept {
        if (!classRegistered || !moduleHandle) return;

        SetLastError(ERROR_SUCCESS);
        if (UnregisterClassW(
                kTaskbarTopologyNotificationClass, moduleHandle)) {
            classRegistered = false;
            return;
        }

        DWORD error = GetLastError();
        if (error == ERROR_CLASS_DOES_NOT_EXIST) {
            classRegistered = false;
            return;
        }

        Wh_Log(
            L"Taskbar topology notifications: class unregister failed (%u)",
            error);
        ReportOutstandingCallbackRisk(
            L"the taskbar topology notification window class registered");
    }

    bool Create(HANDLE updateEvent) {
        state.updateEvent = updateEvent;

        if (!GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(
                    &TaskbarTopologyNotificationWindowProc),
                &moduleHandle) ||
            !moduleHandle) {
            Wh_Log(
                L"Taskbar topology notifications: module handle lookup failed");
            return false;
        }

        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc =
            TaskbarTopologyNotificationWindowProc;
        windowClass.hInstance = moduleHandle;
        windowClass.lpszClassName =
            kTaskbarTopologyNotificationClass;

        if (!RegisterClassExW(&windowClass)) {
            Wh_Log(
                L"Taskbar topology notifications: class registration failed "
                L"(%u)",
                GetLastError());
            moduleHandle = nullptr;
            return false;
        }
        classRegistered = true;

        window = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kTaskbarTopologyNotificationClass, L"", WS_POPUP,
            0, 0, 0, 0, nullptr, nullptr, moduleHandle, &state);
        if (!window) {
            Wh_Log(
                L"Taskbar topology notifications: hidden window creation "
                L"failed (%u)",
                GetLastError());
            ReleaseClassRegistration();
            moduleHandle = nullptr;
            return false;
        }

        return true;
    }

    void PumpMessages() {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    ~TaskbarTopologyNotificationWindow() noexcept {
        if (window && IsWindow(window)) {
            if (!DestroyWindow(window)) {
                Wh_Log(
                    L"Taskbar topology notifications: hidden window "
                    L"destruction failed (%u)",
                    GetLastError());
            }
        }
        window = nullptr;

        ReleaseClassRegistration();
        moduleHandle = nullptr;
    }
};


static DWORD TimerThreadMain() {
    bool lastThemeWasLight = IsSystemLightTheme();
    ULONGLONG nextVolumePollTick = 0;
    ULONGLONG nextMetadataRetryTick = 0;
    ULONGLONG nextInjectionRetryTick = 0;
    ULONGLONG taskbarTopologyRebuildDueTick = 0;
    ULONGLONG themeRefreshDueTick = 0;
    ULONGLONG idleStoppedSinceTick = 0;
    uint64_t lastTaskbarTopology = GetTaskbarTopologyFingerprint();
    bool topologyOwnerWasLive = true;
    bool externalTransparencyProviderPresent =
        HasExternalTaskbarTransparencyProvider();
    HANDLE stopEvent = SnapshotWorkerEventHandle(g_timerStopEvent);
    HANDLE updateEvent = SnapshotWorkerEventHandle(g_timerUpdateEvent);
    if (!stopEvent || !updateEvent) return 0;
    HRESULT timerApartment = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool callCancellationEnabled =
        SUCCEEDED(CoEnableCallCancellation(nullptr));

    HKEY hKey = nullptr;
    HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    struct TimerResourceCleanup {
        HKEY& key;
        HANDLE& event;
        HRESULT apartmentResult;
        bool callCancellationEnabled;
        ~TimerResourceCleanup() noexcept {
            if (key) {
                RegCloseKey(key);
                key = nullptr;
            }
            if (event) {
                CloseHandle(event);
                event = nullptr;
            }
            if (callCancellationEnabled) {
                CoDisableCallCancellation(nullptr);
            }
            if (SUCCEEDED(apartmentResult)) CoUninitialize();
        }
    } timerResourceCleanup{
        hKey, hEvent, timerApartment, callCancellationEnabled};
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_NOTIFY, &hKey) == ERROR_SUCCESS) {
        RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE);
    }

    TaskbarTopologyNotificationWindow topologyNotifications;
    topologyNotifications.Create(updateEvent);

    while (!g_unloading) {
        HANDLE handles[3]{};
        DWORD handleCount = 0;
        const DWORD stopIndex = handleCount;
        handles[handleCount++] = stopEvent;
        const DWORD themeIndex = hEvent ? handleCount : MAXDWORD;
        if (hEvent) handles[handleCount++] = hEvent;
        const DWORD updateIndex = handleCount;
        handles[handleCount++] = updateEvent;

        DWORD waitTimeout = 1000;
        HWND waitPopupWindow =
            g_volumePopupWindow.load(std::memory_order_acquire);
        if (waitPopupWindow && IsWindowVisible(waitPopupWindow)) {
            waitTimeout = 250;
        }

        auto shortenWaitForDeadline =
            [&waitTimeout](ULONGLONG deadline) {
                if (!deadline) return;
                ULONGLONG now = GetTickCount64();
                if (deadline <= now) {
                    waitTimeout = 0;
                    return;
                }
                waitTimeout = std::min<DWORD>(
                    waitTimeout,
                    static_cast<DWORD>(std::min<ULONGLONG>(
                        deadline - now, MAXDWORD)));
            };
        shortenWaitForDeadline(themeRefreshDueTick);
        shortenWaitForDeadline(taskbarTopologyRebuildDueTick);

        DWORD wait = MsgWaitForMultipleObjectsEx(
            handleCount, handles, waitTimeout, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 + handleCount) {
            topologyNotifications.PumpMessages();
            wait = WAIT_TIMEOUT;
        }

        if (wait == WAIT_OBJECT_0 + stopIndex || g_unloading) break;
        if (g_applyingSettings) continue;

        ULONGLONG nowTick = GetTickCount64();

        HWND selectedTaskbar = g_taskbarWnd.load();
        if (!selectedTaskbar || !IsWindow(selectedTaskbar)) {
            selectedTaskbar = FindCurrentProcessTaskbarWnd();
            if (selectedTaskbar) g_taskbarWnd = selectedTaskbar;
        }

        DWORD committedOwnerThreadId = g_playerOwnerThreadId.load();
        HANDLE committedOwnerThreadHandle =
            g_playerOwnerThreadHandle.load();
        HWND committedOwnerWindow = g_playerOwnerWindow.load();
        DWORD currentOwnerThreadId = 0;
        bool committedOwnerLive =
            IsOriginalTaskbarThreadAlive(
                committedOwnerThreadHandle, committedOwnerThreadId) &&
            IsCurrentProcessTaskbarWindow(
                committedOwnerWindow, &currentOwnerThreadId, nullptr) &&
            currentOwnerThreadId == committedOwnerThreadId;
        bool hasCommittedOwner = committedOwnerThreadId != 0;
        bool taskbarOwnerWasLive = hasCommittedOwner
            ? committedOwnerLive
            : selectedTaskbar && IsWindow(selectedTaskbar);
        HWND uiUpdateWindow = hasCommittedOwner
            ? (committedOwnerLive ? committedOwnerWindow : nullptr)
            : selectedTaskbar;
        if (!selectedTaskbar && !uiUpdateWindow) continue;

        bool externalTransparencyProviderNow =
            HasExternalTaskbarTransparencyProvider();
        if (externalTransparencyProviderNow !=
                externalTransparencyProviderPresent) {
            externalTransparencyProviderPresent =
                externalTransparencyProviderNow;
            if (Cfg()->transparentTaskbar) {
                // Restore our captured values when another provider appears,
                // or reapply the user's setting after that provider unloads.
                ApplyTaskbarTransparencyToAll();
            }
        }

        if (themeIndex != MAXDWORD &&
            wait == WAIT_OBJECT_0 + themeIndex) {
            if (hKey) {
                RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE);
            }
            bool currentThemeIsLight = IsSystemLightTheme();
            if (currentThemeIsLight != lastThemeWasLight) {
                lastThemeWasLight = currentThemeIsLight;
                themeRefreshDueTick = nowTick + 150;
                SetEvent(updateEvent);
            }
        }

        if (themeRefreshDueTick && nowTick >= themeRefreshDueTick) {
            themeRefreshDueTick = 0;
            g_needsUiUpdate = true;
            if (!TaskbarXamlCallbacksSuppressed() && uiUpdateWindow) {
                RunFromWindowThread(uiUpdateWindow, [](void*) {
                    if (!TaskbarXamlCallbacksSuppressed() && g_playerGrid) {
                        RefreshThemeColors();
                    }
                }, nullptr);
            }
        }

        // Audio-session enumeration is comparatively expensive. Poll quickly
        // while the volume flyout is visible, otherwise refresh only every
        // 1.5 seconds or immediately after a media-state update.

        ULONGLONG restartNotBefore =
            g_taskbarRestartNotBeforeTick.load(std::memory_order_acquire);
        if (restartNotBefore && nowTick >= restartNotBefore &&
            g_taskbarRestartNotBeforeTick.compare_exchange_strong(
                restartNotBefore, 0, std::memory_order_acq_rel)) {
            Wh_Log(L"Taskbar startup settled; queuing owner-thread rebuild");
            QueueQuickMonitorRebuild();
            nextInjectionRetryTick = nowTick + 1000;
            continue;
        }

        // WM_DISPLAYCHANGE/WM_SETTINGCHANGE notifications wake this worker
        // when monitor or taskbar configuration changes. Debounce a topology
        // snapshot and reuse the existing serialized monitor rebuild. If the
        // widget's owner itself disappeared, don't touch stale XAML from a
        // different thread; TrayUI startup recovery remains the safe
        // owner-replacement path.
        if (topologyNotifications.state.changePending) {
            topologyNotifications.state.changePending = false;
            uint64_t topology = GetTaskbarTopologyFingerprint();
            if (topology != lastTaskbarTopology) {
                lastTaskbarTopology = topology;
                taskbarTopologyRebuildDueTick =
                    topology ? nowTick + 750 : 0;
                topologyOwnerWasLive = taskbarOwnerWasLive;
            }
        }
        if (taskbarTopologyRebuildDueTick &&
            nowTick >= taskbarTopologyRebuildDueTick) {
            if (TaskbarXamlCallbacksSuppressed()) {
                taskbarTopologyRebuildDueTick = nowTick + 500;
            } else {
                taskbarTopologyRebuildDueTick = 0;
                ApplyTaskbarTransparencyToAll();
                if (topologyOwnerWasLive &&
                    (committedOwnerLive ||
                     (selectedTaskbar && IsWindow(selectedTaskbar)))) {
                    Wh_Log(
                        L"Taskbar topology changed; rebuilding monitor widgets");
                    QueueQuickMonitorRebuild();
                } else {
                    Wh_Log(
                        L"Taskbar topology changed with a replaced owner; "
                        L"deferring widget injection to taskbar startup recovery");
                }
            }
        }

        // Windhawk can load after TrayUI::StartTaskbar has already fired, and
        // early boot can expose the taskbar HWND before its XAML tree is ready.
        // Retry one serialized, fail-closed injection per second instead of
        // requiring the user to disable and re-enable the mod.
        if (!restartNotBefore && nowTick >= nextInjectionRetryTick &&
            selectedTaskbar && !hasCommittedOwner &&
            !g_hookInjectionInProgress.exchange(true)) {
            bool dispatched = RunFromWindowThread(selectedTaskbar, [](void*) {
                if (!TaskbarXamlCallbacksSuppressed() && !g_playerGrid) {
                    try {
                        if (InjectPlayerGrid() &&
                            (Cfg()->enableTitleScrolling ||
                             Cfg()->enableArtistScrolling ||
                             Cfg()->showProgressBar)) {
                            StartScrollTimer();
                        }
                    } catch (...) {
                        Wh_Log(L"Startup recovery: taskbar injection failed");
                    }
                }
                g_hookInjectionInProgress = false;
            }, nullptr);
            if (!dispatched) g_hookInjectionInProgress = false;
            nextInjectionRetryTick = nowTick + 1000;
        }

        ULONGLONG retryUntil = g_metadataRetryUntilTick.load();
        if (retryUntil != 0 && nowTick < retryUntil &&
            nowTick >= nextMetadataRetryTick) {
            bool hasSession = false;
            {
                std::lock_guard<std::mutex> lock(g_sessionMtx);
                hasSession = g_currentSession != nullptr;
            }
            if (hasSession) {
                FetchMediaPropertiesAsync();
                FetchPlaybackInfoAsync();
            }
            nextMetadataRetryTick = nowTick + 750;
        }

        HWND popupWindow =
            g_volumePopupWindow.load(std::memory_order_acquire);
        bool volumePopupVisible =
            popupWindow && IsWindowVisible(popupWindow);
        bool mediaUpdateSignaled =
            wait == WAIT_OBJECT_0 + updateIndex;
        bool volumeButtonConfigured =
            Cfg()->showMediaButtons &&
            HasConfiguredMediaButton(MediaButtonType::Volume);
        bool shouldPollVolume = volumePopupVisible || volumeButtonConfigured;
        if (shouldPollVolume &&
            (mediaUpdateSignaled || nowTick >= nextVolumePollTick)) {
            int currentVolume = 0;
            bool currentMuted = false;
            GetCurrentControllableVolume(currentVolume, currentMuted);
            nextVolumePollTick = nowTick + (volumePopupVisible ? 250 : 1500);
        } else if (!shouldPollVolume) {
            nextVolumePollTick = 0;
        }

        bool needsUpdate = g_needsUiUpdate.exchange(false);

        if (Cfg()->idleHideSeconds > 0) {
            bool playing = false;
            {
                std::lock_guard<std::mutex> lk(g_mediaMtx);
                playing = g_media.isPlaying;
            }
            if (playing) {
                idleStoppedSinceTick = 0;
                g_idleSeconds.store(0, std::memory_order_relaxed);
                if (g_hiddenByIdle.load(std::memory_order_relaxed)) {
                    g_hiddenByIdle.store(false, std::memory_order_relaxed);
                    needsUpdate = true;
                }
            } else {
                if (!idleStoppedSinceTick) idleStoppedSinceTick = nowTick;
                const int idleSeconds = static_cast<int>(
                    (nowTick - idleStoppedSinceTick) / 1000);
                g_idleSeconds.store(idleSeconds, std::memory_order_relaxed);
                if (!g_hiddenByIdle.load(std::memory_order_relaxed) &&
                    idleSeconds >= Cfg()->idleHideSeconds) {
                    g_hiddenByIdle.store(true, std::memory_order_relaxed);
                    needsUpdate = true;
                }
            }
        } else {
            idleStoppedSinceTick = 0;
            if (g_hiddenByIdle.load(std::memory_order_relaxed)) {
                g_hiddenByIdle.store(false, std::memory_order_relaxed);
                g_idleSeconds.store(0, std::memory_order_relaxed);
                needsUpdate = true;
            }
        }

        if (needsUpdate && uiUpdateWindow) {
            RunFromWindowThread(uiUpdateWindow, [](void*) {
                if (TaskbarXamlCallbacksSuppressed()) return;
                if (g_playerGrid) {
                    RefreshPlayerContents();
                    UpdateVisibility();
                    RefreshScrollDispatcherTimerCadence();
                }
            }, nullptr);
        }
    }

    return 0;
}

static DWORD WINAPI TimerThreadProc(void*) noexcept {
    g_timerThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    DWORD result = 0;
    try {
        result = TimerThreadMain();
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Timer worker failed: 0x%08X",
               static_cast<uint32_t>(error.code()));
    } catch (...) {
        Wh_Log(L"Timer worker failed with an unexpected exception");
    }
    g_timerThreadId.store(0, std::memory_order_release);
    return result;
}

static void StartTimerThread() {
    if (g_timerThread) return;
    bool stopEventCreated =
        CreateWorkerEventHandle(g_timerStopEvent, true, false);
    bool updateEventCreated =
        CreateWorkerEventHandle(g_timerUpdateEvent, false, false);
    if (!stopEventCreated || !updateEventCreated) {
        CloseWorkerEventHandle(g_timerStopEvent);
        CloseWorkerEventHandle(g_timerUpdateEvent);
        return;
    }

    g_timerThread    = CreateThread(nullptr, 0, TimerThreadProc, nullptr, 0, nullptr);
    if (!g_timerThread) {
        CloseWorkerEventHandle(g_timerStopEvent);
        CloseWorkerEventHandle(g_timerUpdateEvent);
        return;
    }

    StartDiscordPresenceThread();
}
static bool StopTimerThread(bool shutdownCleanup = false,
                            bool cleanupScrollTimer = true) {
    bool discordStopped = StopDiscordPresenceThread(shutdownCleanup);
    SignalWorkerEventHandle(g_timerStopEvent);
    DWORD timerThreadId =
        g_timerThreadId.load(std::memory_order_acquire);
    if (timerThreadId) CoCancelCall(timerThreadId, 0);

    bool timerStopped = true;
    constexpr DWORD timeoutMs = 5000;
    if (g_timerThread) {
        timerStopped = WaitForThreadExit(g_timerThread, timeoutMs);
        if (!timerStopped) {
            if (shutdownCleanup) {
                Wh_Log(
                    L"Timer thread exceeded the unload deadline; retaining "
                    L"the module if it remains active");
            } else {
                Wh_Log(
                    L"Timer thread didn't stop within the "
                    L"settings-change deadline");
            }
        } else {
            CloseHandle(g_timerThread);
            g_timerThread = nullptr;
        }
    }
    if (timerStopped) {
        CloseWorkerEventHandle(g_timerStopEvent);
        CloseWorkerEventHandle(g_timerUpdateEvent);
    }
    bool scrollTimerStopped = cleanupScrollTimer
        ? StopScrollTimer(shutdownCleanup)
        : false;
    return discordStopped && timerStopped && scrollTimerStopped;
}
static void RefreshThemeColorsForGrid(
    Grid const& playerGrid,
    std::shared_ptr<PlayerVisualState> const& visualState) {
    if (!playerGrid || !visualState || TaskbarXamlCallbacksSuppressed()) {
        return;
    }
    try {
        auto textClr = TextColor();
        auto artistClr = ArtistColor();
        auto buttonClr = ButtonColor();

        if (auto progressFe = FindChildByName(
                playerGrid, kProgressTrackName)) {
            if (auto progressTrack = progressFe.try_as<Border>()) {
                progressTrack.Background(MakeBrush(ProgressTrackColor()));
            }
        }
        if (auto progressFe = FindChildByName(
                playerGrid, kProgressFillName)) {
            if (auto progressFill = progressFe.try_as<Border>()) {
                progressFill.Background(MakeBrush(ProgressFillColor()));
            }
        }

        if (auto bgFe = FindChildByName(playerGrid, L"TaskbarMediaPresence_Background")) {
            if (auto bgBorder = bgFe.try_as<Border>()) {
                auto bgType = Cfg()->backgroundType;

                if (bgType == L"album_art_blur") {
                    if (!visualState->cachedThumbnailBytes.empty()) {
                        int w = (int)bgBorder.ActualWidth();
                        int h = (int)bgBorder.ActualHeight();
                        if (w > 0 && h > 0) {
                            bgBorder.Background(MakeAlbumBlurBrush(
                                visualState->blurBgCache,
                                visualState->cachedThumbnailBytes, w, h));
                        }
                    } else {
                        auto fallbackCol = IsSystemLightTheme()
                            ? winrt::Windows::UI::Color{0xCC, 0xF3, 0xF3, 0xF3}
                            : winrt::Windows::UI::Color{0xCC, 0x20, 0x20, 0x20};
                        bgBorder.Background(MakeBrush(fallbackCol));
                    }
                    bgBorder.Visibility(Visibility::Visible);
                    bgBorder.Opacity(Cfg()->blurOpacity / 100.0);
                } else if (bgType == L"solid" || bgType == L"gradient" || bgType == L"acrylic" || bgType == L"mica" || bgType == L"mica_alt") {
                    bgBorder.Background(MakeBackgroundBrush());
                    bgBorder.Visibility(Visibility::Visible);
                    bgBorder.Opacity(1.0);
                } else {
                    bgBorder.Background(nullptr);
                    bgBorder.Visibility(Visibility::Collapsed);
                }
            }
        }

        if (auto fe = FindChildByName(playerGrid, L"TaskbarMediaPresence_OuterBorder")) {
            if (auto btn = fe.try_as<Button>()) {
                try {
                    auto normalBg = MakeBackgroundBrush();
                    ApplyPlayerButtonState(btn, normalBg, false, false);
                } catch (...) {}
            }
        }

        if (auto fe = FindChildByName(playerGrid, kTitleBlockName))
            if (auto tb = fe.try_as<TextBlock>()) tb.Foreground(MakeBrush(textClr));

        if (auto fe = FindChildByName(playerGrid, kArtistBlockName))
            if (auto ab = fe.try_as<TextBlock>()) ab.Foreground(MakeBrush(artistClr));

        for (const wchar_t* name : {kPrevBtnName, kPlayBtnName,
                                    kNextBtnName, kVolumeBtnName,
                                    kRewindBtnName, kForwardBtnName,
                                    kShuffleBtnName, kRepeatBtnName}) {
            if (auto fe = FindChildByName(playerGrid, name)) {
                if (auto btn = fe.try_as<Button>()) {
                    if (auto ct = btn.Content().try_as<TextBlock>()) ct.Foreground(MakeBrush(buttonClr));
                    ApplyMediaButtonState(btn, false, false);
                }
            }
        }
    } catch (...) {}
}

static void RefreshThemeColors() {
    Grid primaryGrid = g_playerGrid;
    if (primaryGrid) {
        RefreshThemeColorsForGrid(primaryGrid, g_primaryVisualState);
    }

    auto mirrors = SnapshotMirrorPlayers();
    for (const auto& instance : mirrors) {
        if (!instance || !instance->playerGrid || !instance->visualState) {
            continue;
        }
        if (instance->ownerThreadId != GetCurrentThreadId()) continue;
        RefreshThemeColorsForGrid(
            instance->playerGrid, instance->visualState);
    }
}

struct TaskbarWindowInfo {
    HWND hWnd;
    bool primary;
    RECT monitorRect;
};

static bool IsCurrentProcessTaskbarWindow(HWND window,
                                          DWORD* threadId,
                                          bool* secondary) {
    if (!window || !IsWindow(window)) return false;

    DWORD processId = 0;
    DWORD ownerThreadId = GetWindowThreadProcessId(window, &processId);
    if (!ownerThreadId || processId != GetCurrentProcessId()) return false;

    wchar_t className[64]{};
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) return false;
    bool isPrimary = _wcsicmp(className, L"Shell_TrayWnd") == 0;
    bool isSecondary =
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
    if (!isPrimary && !isSecondary) return false;

    if (threadId) *threadId = ownerThreadId;
    if (secondary) *secondary = isSecondary;
    return true;
}

static std::vector<TaskbarWindowInfo> EnumerateTaskbarWindows() {
    std::vector<TaskbarWindowInfo> windows;
    EnumWindows([](HWND hWnd, LPARAM lp) CALLBACK -> BOOL {
        DWORD pid = 0;
        wchar_t cls[64] = {};
        if (!GetWindowThreadProcessId(hWnd, &pid) ||
            pid != GetCurrentProcessId() ||
            !GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
            return TRUE;
        }

        bool primary = _wcsicmp(cls, L"Shell_TrayWnd") == 0;
        bool secondary = _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
        if (!primary && !secondary) return TRUE;

        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        RECT monitorRect{};
        if (HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST)) {
            if (GetMonitorInfoW(monitor, &monitorInfo)) {
                monitorRect = monitorInfo.rcMonitor;
            }
        }

        reinterpret_cast<std::vector<TaskbarWindowInfo>*>(lp)->push_back(
            {hWnd, primary, monitorRect});
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));

    std::sort(windows.begin(), windows.end(), [](const auto& a, const auto& b) {
        if (a.primary != b.primary) return a.primary;
        if (a.monitorRect.left != b.monitorRect.left) {
            return a.monitorRect.left < b.monitorRect.left;
        }
        if (a.monitorRect.top != b.monitorRect.top) {
            return a.monitorRect.top < b.monitorRect.top;
        }
        return reinterpret_cast<UINT_PTR>(a.hWnd) <
               reinterpret_cast<UINT_PTR>(b.hWnd);
    });

    return windows;
}

static uint64_t GetTaskbarTopologyFingerprint() {
    auto windows = EnumerateTaskbarWindows();
    if (windows.empty()) return 0;

    uint64_t hash = 1469598103934665603ULL;
    auto mix = [&hash](uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ULL;
    };
    mix(windows.size());
    for (const auto& window : windows) {
        mix(reinterpret_cast<UINT_PTR>(window.hWnd));
        mix(window.primary ? 1 : 0);
        mix(static_cast<uint32_t>(window.monitorRect.left));
        mix(static_cast<uint32_t>(window.monitorRect.top));
        mix(static_cast<uint32_t>(window.monitorRect.right));
        mix(static_cast<uint32_t>(window.monitorRect.bottom));
    }
    return hash;
}

static HWND FindCurrentProcessTaskbarWnd() {
    auto windows = EnumerateTaskbarWindows();
    if (windows.empty()) return nullptr;

    int requestedTaskbar = std::max(Cfg()->taskbarNumber, 1);
    if (requestedTaskbar == 1) {
        auto primary = std::find_if(windows.begin(), windows.end(),
                                    [](const auto& item) { return item.primary; });
        return primary != windows.end() ? primary->hWnd : windows.front().hWnd;
    }

    int secondaryIndex = requestedTaskbar - 2;
    for (const auto& item : windows) {
        if (!item.primary && secondaryIndex-- == 0) {
            return item.hWnd;
        }
    }

    Wh_Log(L"Requested taskbar %d was not found; falling back to the primary taskbar",
           requestedTaskbar);
    auto primary = std::find_if(windows.begin(), windows.end(),
                                [](const auto& item) { return item.primary; });
    return primary != windows.end() ? primary->hWnd : windows.front().hWnd;
}

static HWND FindCurrentProcessTaskbarWndForThread(DWORD threadId) {
    if (!threadId) return nullptr;
    for (const auto& taskbar : EnumerateTaskbarWindows()) {
        DWORD ownerThreadId = 0;
        if (IsCurrentProcessTaskbarWindow(
                taskbar.hWnd, &ownerThreadId, nullptr) &&
            ownerThreadId == threadId) {
            return taskbar.hWnd;
        }
    }
    return nullptr;
}

enum class ShellExplorerProcessIdentity {
    NonShell,
    ConfirmedShell,
    StartupCandidate,
};

static bool IsSystemExplorerWithoutArguments() {
    wchar_t processPath[MAX_PATH]{};
    DWORD processPathLength =
        GetModuleFileNameW(nullptr, processPath, ARRAYSIZE(processPath));
    if (!processPathLength || processPathLength >= ARRAYSIZE(processPath)) {
        return false;
    }

    wchar_t windowsDirectory[MAX_PATH]{};
    UINT windowsDirectoryLength =
        GetWindowsDirectoryW(windowsDirectory, ARRAYSIZE(windowsDirectory));
    if (!windowsDirectoryLength ||
        windowsDirectoryLength >= ARRAYSIZE(windowsDirectory)) {
        return false;
    }

    std::wstring expectedPath(windowsDirectory, windowsDirectoryLength);
    if (!expectedPath.empty() &&
        expectedPath.back() != L'\\' &&
        expectedPath.back() != L'/') {
        expectedPath.push_back(L'\\');
    }
    expectedPath += L"explorer.exe";
    if (_wcsicmp(processPath, expectedPath.c_str()) != 0) {
        return false;
    }

    int argumentCount = 0;
    LPWSTR* arguments =
        CommandLineToArgvW(GetCommandLineW(), &argumentCount);
    if (!arguments) {
        return false;
    }

    // The desktop shell is launched as explorer.exe without a target or
    // switches. Separate folder/Control Panel hosts use arguments such as
    // /factory and -Embedding. Treat the no-argument form only as a
    // provisional candidate until this process creates the taskbar.
    bool result = argumentCount == 1;
    LocalFree(arguments);
    return result;
}

static ShellExplorerProcessIdentity GetCurrentProcessShellExplorerIdentity() {
    DWORD currentProcessId = GetCurrentProcessId();
    DWORD shellProcessId = 0;
    DWORD taskbarProcessId = 0;

    if (HWND shellWindow = GetShellWindow()) {
        GetWindowThreadProcessId(shellWindow, &shellProcessId);
    }
    if (HWND taskbarWindow = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(taskbarWindow, &taskbarProcessId);
    }

    // During an Explorer restart the desktop and taskbar registrations can be
    // updated a few milliseconds apart. Either authoritative window belonging
    // to this PID is enough to confirm ownership.
    if (shellProcessId == currentProcessId ||
        taskbarProcessId == currentProcessId) {
        return ShellExplorerProcessIdentity::ConfirmedShell;
    }

    // A registered shell/taskbar owned by another Explorer is authoritative.
    // This keeps the mod out of separate folder and Control Panel processes.
    if (shellProcessId || taskbarProcessId) {
        return ShellExplorerProcessIdentity::NonShell;
    }

    // At cold sign-in Windhawk can initialize before Explorer calls
    // SetShellWindowEx or creates Shell_TrayWnd. Do not permanently reject
    // that process: accept only the system, no-argument Explorer as a startup
    // candidate. FindCurrentProcessTaskbarWnd still gates all XAML injection
    // on a taskbar window owned by this exact PID.
    return IsSystemExplorerWithoutArguments()
        ? ShellExplorerProcessIdentity::StartupCandidate
        : ShellExplorerProcessIdentity::NonShell;
}

static bool IsReadableMemoryRange(const void* address, size_t size) {
    if (!address || size == 0) return false;
    MEMORY_BASIC_INFORMATION memory{};
    if (!VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    const auto start = reinterpret_cast<uintptr_t>(address);
    const auto regionStart = reinterpret_cast<uintptr_t>(memory.BaseAddress);
    const auto regionEnd = regionStart + memory.RegionSize;
    return start >= regionStart && start <= regionEnd &&
           size <= regionEnd - start;
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    DWORD ownerThreadId = 0;
    bool isSecondary = false;
    if (!IsCurrentProcessTaskbarWindow(
            hTaskbarWnd, &ownerThreadId, &isSecondary) ||
        ownerThreadId != GetCurrentThreadId()) {
        Wh_Log(L"GetTaskbarXamlRoot: rejected invalid or wrong-thread taskbar %p",
               hTaskbarWnd);
        return nullptr;
    }

    HWND hTaskSwWnd = isSecondary
        ? FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
        : (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;

    void* expectedVftable = isSecondary
        ? CSecondaryTaskBand_ITaskListWndSite_vftable
        : CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = isSecondary
        ? CSecondaryTaskBand_GetTaskbarHost_Original
        : CTaskBand_GetTaskbarHost_Original;
    if (!expectedVftable || !getTaskbarHost) return nullptr;

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; i <= 20; ++i) {
        if (!IsReadableMemoryRange(taskBandForTaskListWndSite,
                                   sizeof(void*))) {
            return nullptr;
        }
        if (*(void**)taskBandForTaskListWndSite == expectedVftable) break;
        if (i == 20) return nullptr;
        taskBandForTaskListWndSite =
            (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0;
    bool frameHeightPatternRecognized = false;
#if defined(_M_X64) || defined(__x86_64__)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (IsReadableMemoryRange(b, 8) &&
            b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
            frameHeightPatternRecognized = true;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern (x64)");
        }
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp fp, lr, [sp, #-0x10]!
        // fd030091 mov fp, sp
        // 080c41f8 ldr x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (IsReadableMemoryRange(p, sizeof(DWORD) * 4) &&
            p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
            frameHeightPatternRecognized = true;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern (arm64)");
        }
    }
#else
    Wh_Log(L"GetTaskbarXamlRoot: unsupported architecture");
#endif

    if (!frameHeightPatternRecognized ||
        !IsReadableMemoryRange(
            static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
                taskbarElementIUnknownOffset,
            sizeof(IUnknown*))) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    if (!taskbarElementIUnknown) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement{nullptr};
    HRESULT queryResult = taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    return SUCCEEDED(queryResult) ? result : nullptr;
}

static FrameworkElement FindDescendantByRuntimeClass(
    FrameworkElement const& root, std::wstring_view className,
    int depth = 48) {
    if (!root || depth < 0) return nullptr;
    try {
        if (winrt::get_class_name(root) == className) return root;
        int childCount = VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < childCount; ++i) {
            auto child = VisualTreeHelper::GetChild(root, i)
                             .try_as<FrameworkElement>();
            if (!child) continue;
            auto match = FindDescendantByRuntimeClass(
                child, className, depth - 1);
            if (match) return match;
        }
    } catch (...) {}
    return nullptr;
}

struct TaskbarTransparencyState {
    DWORD ownerThreadId = 0;
    bool taskbarBackgroundCaptured = false;
    double taskbarBackgroundOpacity = 1.0;
    bool backgroundFillCaptured = false;
    double backgroundFillOpacity = 1.0;
    bool backgroundStrokeCaptured = false;
    double backgroundStrokeOpacity = 1.0;
};

static std::mutex g_taskbarTransparencyMtx;
static std::unordered_map<UINT_PTR, TaskbarTransparencyState>
    g_taskbarTransparencyStates;

static bool HasTaskbarTransparencyState(HWND taskbarWindow) {
    std::lock_guard<std::mutex> lock(g_taskbarTransparencyMtx);
    return g_taskbarTransparencyStates.find(
               reinterpret_cast<UINT_PTR>(taskbarWindow)) !=
           g_taskbarTransparencyStates.end();
}

static void DiscardTaskbarTransparencyStatesForThread(DWORD ownerThreadId) {
    if (!ownerThreadId) return;
    std::lock_guard<std::mutex> lock(g_taskbarTransparencyMtx);
    for (auto iterator = g_taskbarTransparencyStates.begin();
         iterator != g_taskbarTransparencyStates.end();) {
        if (iterator->second.ownerThreadId == ownerThreadId) {
            iterator = g_taskbarTransparencyStates.erase(iterator);
        } else {
            ++iterator;
        }
    }
}

static bool HasExternalTaskbarTransparencyProvider() {
    return GetModuleHandleW(L"ExplorerTAP.dll") != nullptr ||
           GetModuleHandleW(L"ExplorerHooks.dll") != nullptr;
}

static bool ApplyTaskbarTransparencyToWindow(HWND taskbarWindow,
                                              bool transparent) {
    const UINT_PTR stateKey = reinterpret_cast<UINT_PTR>(taskbarWindow);
    if (!transparent && !HasTaskbarTransparencyState(taskbarWindow)) {
        // Avoid touching the taskbar XAML tree when this mod has nothing to
        // restore. This is the normal path when transparency is disabled.
        return true;
    }

    try {
        auto xamlRoot = GetTaskbarXamlRoot(taskbarWindow);
        auto root = xamlRoot
            ? xamlRoot.Content().try_as<FrameworkElement>()
            : nullptr;
        if (!root) return false;

        auto taskbarFrame = FindDescendantByRuntimeClass(
            root, L"Taskbar.TaskbarFrame");
        if (!taskbarFrame) taskbarFrame = root;

        auto taskbarBackground = FindDescendantByRuntimeClass(
            taskbarFrame, L"Taskbar.TaskbarBackground");
        if (taskbarBackground &&
            taskbarBackground.Name() == L"HoverFlyoutBackgroundControl") {
            taskbarBackground = nullptr;
        }

        FrameworkElement backgroundFill =
            FindChildByName(taskbarFrame, L"BackgroundFill");
        FrameworkElement backgroundStroke =
            FindChildByName(taskbarFrame, L"BackgroundStroke");
        if (transparent) {
            bool changed = false;
            if (taskbarBackground) {
                double originalOpacity = taskbarBackground.Opacity();
                {
                    std::lock_guard<std::mutex> lock(
                        g_taskbarTransparencyMtx);
                    auto& state = g_taskbarTransparencyStates[stateKey];
                    if (state.ownerThreadId != GetCurrentThreadId()) {
                        state = {};
                    }
                    state.ownerThreadId = GetCurrentThreadId();
                    if (!state.taskbarBackgroundCaptured) {
                        // A taskbar XAML rebuild can keep the HWND while
                        // switching between the runtime-class and named
                        // background variants. Captures from the old tree no
                        // longer need restoration.
                        state.backgroundFillCaptured = false;
                        state.backgroundStrokeCaptured = false;
                        state.taskbarBackgroundCaptured = true;
                        state.taskbarBackgroundOpacity = originalOpacity;
                    }
                }
                taskbarBackground.Opacity(0.0);
                changed = true;
            } else {
                auto makeTransparent = [&](FrameworkElement const& element,
                                           bool TaskbarTransparencyState::*captured,
                                           double TaskbarTransparencyState::*opacity) {
                    if (!element) return;
                    double originalOpacity = element.Opacity();
                    {
                        std::lock_guard<std::mutex> lock(
                            g_taskbarTransparencyMtx);
                        auto& state = g_taskbarTransparencyStates[stateKey];
                        if (state.ownerThreadId != GetCurrentThreadId()) {
                            state = {};
                        }
                        state.ownerThreadId = GetCurrentThreadId();
                        if (!(state.*captured)) {
                            state.*captured = true;
                            state.*opacity = originalOpacity;
                        }
                    }
                    element.Opacity(0.0);
                    changed = true;
                };
                {
                    std::lock_guard<std::mutex> lock(
                        g_taskbarTransparencyMtx);
                    auto& state = g_taskbarTransparencyStates[stateKey];
                    if (state.ownerThreadId != GetCurrentThreadId()) {
                        state = {};
                    }
                    state.ownerThreadId = GetCurrentThreadId();
                    if (!state.backgroundFillCaptured &&
                        !state.backgroundStrokeCaptured) {
                        state.taskbarBackgroundCaptured = false;
                    }
                }
                makeTransparent(
                    backgroundFill,
                    &TaskbarTransparencyState::backgroundFillCaptured,
                    &TaskbarTransparencyState::backgroundFillOpacity);
                makeTransparent(
                    backgroundStroke,
                    &TaskbarTransparencyState::backgroundStrokeCaptured,
                    &TaskbarTransparencyState::backgroundStrokeOpacity);
            }
            if (changed) {
                Wh_Log(L"Taskbar transparency: enabled on %p",
                       taskbarWindow);
            }
            return changed;
        }

        TaskbarTransparencyState state;
        {
            std::lock_guard<std::mutex> lock(g_taskbarTransparencyMtx);
            auto entry = g_taskbarTransparencyStates.find(stateKey);
            if (entry == g_taskbarTransparencyStates.end()) {
                // The mod never changed this taskbar, so leave any theme or
                // third-party opacity exactly as it is.
                return true;
            }
            state = entry->second;
            if (state.ownerThreadId != GetCurrentThreadId()) {
                // The HWND was reused on another UI thread. Its old XAML tree
                // is gone, so never apply the old tree's opacity to the new one.
                g_taskbarTransparencyStates.erase(entry);
                return true;
            }
        }

        bool restoredAny = false;
        bool restoredAll = true;
        auto restoreOwnedOpacity = [&](FrameworkElement const& element,
                                       bool captured,
                                       double originalOpacity) {
            if (!captured) return;
            if (!element) {
                restoredAll = false;
                return;
            }

            // Zero is the value owned by this mod. If another taskbar-style mod
            // changed the element after our write, leave its value untouched and
            // discard our stale snapshot instead of clobbering it on disable.
            if (std::abs(element.Opacity()) <= 0.0001) {
                element.Opacity(originalOpacity);
                restoredAny = true;
            } else {
                Wh_Log(
                    L"Taskbar transparency: opacity on %p was changed by "
                    L"another provider; leaving it untouched",
                    taskbarWindow);
            }
        };
        restoreOwnedOpacity(
            taskbarBackground, state.taskbarBackgroundCaptured,
            state.taskbarBackgroundOpacity);
        restoreOwnedOpacity(
            backgroundFill, state.backgroundFillCaptured,
            state.backgroundFillOpacity);
        restoreOwnedOpacity(
            backgroundStroke, state.backgroundStrokeCaptured,
            state.backgroundStrokeOpacity);

        if (restoredAll) {
            std::lock_guard<std::mutex> lock(g_taskbarTransparencyMtx);
            g_taskbarTransparencyStates.erase(stateKey);
        }
        if (restoredAny) {
            Wh_Log(L"Taskbar transparency: restored prior opacity on %p",
                   taskbarWindow);
        }
        return restoredAll;
    } catch (...) {
        Wh_Log(L"Taskbar transparency: failed on %p", taskbarWindow);
        return false;
    }
}

static bool ApplyTaskbarTransparencyToAll(
    bool shutdownCleanup,
    std::optional<bool> transparentOverride) {
    uint64_t restartGeneration =
        g_taskbarRestartGeneration.load(std::memory_order_acquire);
    if (!shutdownCleanup && TaskbarRestartSettleWindowActive()) {
        return false;
    }
    bool transparent = transparentOverride.value_or(
        Cfg()->transparentTaskbar);
    if (transparent && HasExternalTaskbarTransparencyProvider()) {
        static std::atomic<bool> conflictLogged{false};
        if (!conflictLogged.exchange(true)) {
            Wh_Log(
                L"Taskbar transparency: another transparency provider is "
                L"loaded; skipping this mod's taskbar-background changes");
        }

        // Treat the request as disabled while the external provider is active.
        // If this mod applied transparency earlier, the normal false path below
        // restores only the values that this mod captured.
        transparent = false;
    }

    bool allSucceeded = true;
    auto taskbars = EnumerateTaskbarWindows();
    for (const auto& taskbar : taskbars) {
        if (!transparent && !HasTaskbarTransparencyState(taskbar.hWnd)) {
            continue;
        }
        struct TransparencyWork {
            HWND window;
            bool transparent;
            bool enforceRestartSafety;
            uint64_t restartGeneration;
            bool applied = false;
        } work{taskbar.hWnd, transparent, !shutdownCleanup,
               restartGeneration};

        auto apply = [](void* param) {
            auto* workItem = static_cast<TransparencyWork*>(param);
            if (workItem->enforceRestartSafety &&
                (TaskbarRestartSettleWindowActive() ||
                 workItem->restartGeneration !=
                     g_taskbarRestartGeneration.load(
                         std::memory_order_acquire))) {
                return;
            }
            workItem->applied = ApplyTaskbarTransparencyToWindow(
                workItem->window, workItem->transparent);
        };
        bool dispatched = false;
        if (shutdownCleanup) {
            dispatched = RunFromWindowThreadForCleanup(
                taskbar.hWnd, apply, &work);
        } else {
            dispatched = RunFromWindowThread(taskbar.hWnd, apply, &work);
        }
        allSucceeded = allSucceeded && dispatched && work.applied;
    }

    // A disconnected monitor destroys its taskbar, so there is no live XAML
    // element left to restore. Drop only those dead-window snapshots; live
    // restore failures remain available for a later taskbar/settings pass.
    std::lock_guard<std::mutex> lock(g_taskbarTransparencyMtx);
    for (auto entry = g_taskbarTransparencyStates.begin();
         entry != g_taskbarTransparencyStates.end();) {
        HWND window = reinterpret_cast<HWND>(entry->first);
        if (!IsWindow(window)) {
            entry = g_taskbarTransparencyStates.erase(entry);
        } else {
            ++entry;
        }
    }
    if (shutdownCleanup && !g_taskbarTransparencyStates.empty()) {
        allSucceeded = false;
    }
    return allSucceeded;
}

static bool RestoreTaskbarTransparencyForCurrentThread() {
    DWORD ownerThreadId = GetCurrentThreadId();
    bool allSucceeded = true;
    for (const auto& taskbar : EnumerateTaskbarWindows()) {
        if (GetWindowThreadProcessId(taskbar.hWnd, nullptr) !=
                ownerThreadId ||
            !HasTaskbarTransparencyState(taskbar.hWnd)) {
            continue;
        }
        allSucceeded =
            ApplyTaskbarTransparencyToWindow(taskbar.hWnd, false) &&
            allSucceeded;
    }
    {
        std::lock_guard<std::mutex> lock(g_taskbarTransparencyMtx);
        for (const auto& [window, state] : g_taskbarTransparencyStates) {
            (void)window;
            if (state.ownerThreadId == ownerThreadId) {
                allSucceeded = false;
                break;
            }
        }
    }
    return allSucceeded;
}

static const wchar_t* GetGlyph(int cmd, bool isPlaying = false) {
    bool isFluent = (Cfg()->iconStyle == L"fluent_outline" || Cfg()->iconStyle == L"fluent_filled");
    bool isFilled = (Cfg()->iconStyle == L"fluent_filled" || Cfg()->iconStyle == L"mdl2_filled");

    switch (cmd) {
        case 1:
            if (isFilled) return L"\uF8AC";
            return L"\uE892";
        case 2:
            if (isPlaying) {
                if (isFluent && isFilled) return L"\uE62E";
                if (!isFluent && isFilled) return L"\uF8AE";
                return L"\uE769";
            } else {
                if (isFilled) return L"\uF5B0";
                return L"\uE768";
            }
        case 3:
            if (isFilled) return L"\uF8AD";
            return L"\uE893";
        case 5:
            if (isFilled) return L"\uE627";
            return L"\uEB9E";
        case 6:
            if (isFilled) return L"\uE628";
            return L"\uEB9D";
        case 7:
            return L"\uE8B1";
        case 8: {
            RepeatMode mode = g_repeatMode.load();
            switch (mode) {
                case RepeatMode::Off: return L"\uF5E7";
                case RepeatMode::All: return L"\uE8EE";
                case RepeatMode::One: return L"\uE8ED";
            }
            break;
        }
        case 13: return L"\uE767";
    }
    return L"";
}

static TextBlock MakeIconText(const wchar_t* glyph, double sz, winrt::Windows::UI::Color c) {
    TextBlock t;
    t.Text(glyph);
    t.FontSize(sz);
    t.Foreground(MakeBrush(c));
    t.VerticalAlignment(VerticalAlignment::Center);
    t.HorizontalAlignment(HorizontalAlignment::Center);

    bool useFluent = (Cfg()->iconStyle == L"fluent_outline" || Cfg()->iconStyle == L"fluent_filled");

    try {
        if (useFluent) {
            t.FontFamily(FontFamily(L"Segoe Fluent Icons"));
        } else {
            t.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
        }
    } catch (...) {
        try {
            t.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
        } catch (...) {
            try {
                t.FontFamily(FontFamily(L"Segoe UI Symbol"));
            } catch (...) {}
        }
    }
    return t;
}

static Button MakeControlButton(
    int cmd,
    bool isPlaying,
    winrt::Windows::UI::Color iconColor,
    std::shared_ptr<PlayerVisualState> const& visualState) {
    Button btn;
    try {

        if (!((cmd >= 1 && cmd <= 9) || cmd == 13)) {
            Wh_Log(L"MakeControlButton: Invalid command %d, defaulting to 2 (play/pause)", cmd);
            cmd = 2;
        }

        btn.Width((double)Cfg()->buttonSize);
        btn.Height((double)Cfg()->buttonSize);
        btn.Padding({1,1,1,1});
        btn.CornerRadius({
            Cfg()->buttonCornerRadiusTL,
            Cfg()->buttonCornerRadiusTR,
            Cfg()->buttonCornerRadiusBR,
            Cfg()->buttonCornerRadiusBL
        });
        btn.BorderThickness({0,0,0,0});
        btn.VerticalAlignment(VerticalAlignment::Center);
        btn.HorizontalAlignment(HorizontalAlignment::Center);

        const wchar_t* glyph = GetGlyph(cmd, isPlaying);

        double opacity = 1.0;
        if (cmd == 7 && !g_shuffleEnabled.load()) {
            opacity = 0.4;
        }

        auto iconText = MakeIconText(glyph, (double)Cfg()->buttonIconSize, iconColor);
        iconText.Opacity(opacity);
        btn.Content(winrt::box_value(iconText));

        std::weak_ptr<PlayerVisualState> weakVisualState = visualState;
        auto clickToken = btn.Click([cmd, weakVisualState](
                      winrt::Windows::Foundation::IInspectable const& sender,
                      RoutedEventArgs const&) {
            if (PlayerXamlCallbacksAllowed(weakVisualState)) {
                try {
                    if (cmd == 13) {
                        ShowAppVolumeFlyout(sender.try_as<FrameworkElement>());
                    } else {
                        SendMediaCommandAsync(cmd);
                        DispatchMediaUpdate();
                    }
                } catch (...) {
                    Wh_Log(L"MakeControlButton: Exception in Click handler for cmd %d", cmd);
                }
            }
        });
        TrackPlayerXamlSubscription(
            visualState, btn, clickToken,
            [](Button const& source, winrt::event_token token) {
                source.Click(token);
            });

        if (cmd == 13) {
            auto wheelToken = btn.PointerWheelChanged(
                [weakVisualState](auto const&, PointerRoutedEventArgs const& e) {
                if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                try {
                    int delta = e.GetCurrentPoint(nullptr)
                                    .Properties().MouseWheelDelta();
                    AdjustCurrentMediaAppVolumeAsync(delta);
                    e.Handled(true);
                } catch (...) {
                    Wh_Log(L"Volume wheel callback failed");
                }
            });
            TrackPlayerXamlSubscription(
                visualState, btn, wheelToken,
                [](Button const& source, winrt::event_token token) {
                    source.PointerWheelChanged(token);
                });
        }

        ApplyFluentMediaButtonStyle(btn);

        auto isPressed = std::make_shared<bool>(false);
        auto isHovered = std::make_shared<bool>(false);
        auto weakButton = winrt::make_weak(btn);

        auto updateBtnVisualState = [weakButton, weakVisualState,
                                     isPressed, isHovered]() {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                if (auto button = weakButton.get()) {
                    ApplyMediaButtonState(
                        button, *isHovered, *isPressed);
                }
            } catch (...) {

            }
        };

        updateBtnVisualState();

        auto enteredToken = btn.PointerEntered(
            [weakVisualState, isHovered, updateBtnVisualState](auto const&, auto const&) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            *isHovered = true;
            updateBtnVisualState();
        });
        TrackPlayerXamlSubscription(
            visualState, btn, enteredToken,
            [](Button const& source, winrt::event_token token) {
                source.PointerEntered(token);
            });

        auto exitedToken = btn.PointerExited(
            [weakVisualState, isHovered, updateBtnVisualState](auto const&, auto const&) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            *isHovered = false;
            updateBtnVisualState();
        });
        TrackPlayerXamlSubscription(
            visualState, btn, exitedToken,
            [](Button const& source, winrt::event_token token) {
                source.PointerExited(token);
            });

        auto pressedToken = btn.PointerPressed(
            [weakVisualState, isPressed, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                if (auto elem = sender.template try_as<UIElement>()) {
                    elem.CapturePointer(e.Pointer());
                }
                *isPressed = true;
                updateBtnVisualState();
            } catch (...) {
                Wh_Log(L"Media button PointerPressed callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, btn, pressedToken,
            [](Button const& source, winrt::event_token token) {
                source.PointerPressed(token);
            });

        auto releasedToken = btn.PointerReleased(
            [weakVisualState, isPressed, isHovered, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                bool actuallyHovered = false;
                if (auto elem = sender.template try_as<UIElement>()) {
                    elem.ReleasePointerCapture(e.Pointer());
                    auto bounds = elem.RenderSize();
                    auto pos = e.GetCurrentPoint(elem).Position();
                    actuallyHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                }
                *isPressed = false;
                *isHovered = actuallyHovered;
                updateBtnVisualState();
                e.Handled(true);
            } catch (...) {
                Wh_Log(L"Media button PointerReleased callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, btn, releasedToken,
            [](Button const& source, winrt::event_token token) {
                source.PointerReleased(token);
            });

        auto canceledToken = btn.PointerCanceled(
            [weakVisualState, isPressed, isHovered, updateBtnVisualState](auto const&, auto const&) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            *isPressed = false;
            *isHovered = false;
            updateBtnVisualState();
        });
        TrackPlayerXamlSubscription(
            visualState, btn, canceledToken,
            [](Button const& source, winrt::event_token token) {
                source.PointerCanceled(token);
            });

        auto captureLostToken = btn.PointerCaptureLost(
            [weakVisualState, isPressed, isHovered, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                *isPressed = false;
                if (auto elem = sender.template try_as<UIElement>()) {
                    auto bounds = elem.RenderSize();
                    auto pos = e.GetCurrentPoint(elem).Position();
                    *isHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                }
                updateBtnVisualState();
            } catch (...) {
                *isHovered = false;
                Wh_Log(L"Media button PointerCaptureLost callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, btn, captureLostToken,
            [](Button const& source, winrt::event_token token) {
                source.PointerCaptureLost(token);
            });

    } catch (...) {}
    return btn;
}

static void AddLayoutAnchorOverlay(Grid const& target, const wchar_t* name, winrt::Windows::UI::Color color) {
    if (!target || !Cfg()->showLayoutAnchors) return;
    try {
        Grid overlay;
        overlay.Name(name);
        overlay.IsHitTestVisible(false);
        overlay.HorizontalAlignment(HorizontalAlignment::Stretch);
        overlay.VerticalAlignment(VerticalAlignment::Stretch);

        winrt::Windows::UI::Xaml::Shapes::Rectangle vLine;
        vLine.Width(1);
        vLine.Fill(MakeBrush(color));
        vLine.HorizontalAlignment(HorizontalAlignment::Center);
        vLine.VerticalAlignment(VerticalAlignment::Stretch);

        winrt::Windows::UI::Xaml::Shapes::Rectangle hLine;
        hLine.Height(1);
        hLine.Fill(MakeBrush(color));
        hLine.HorizontalAlignment(HorizontalAlignment::Stretch);
        hLine.VerticalAlignment(VerticalAlignment::Center);

        Border outline;
        outline.BorderBrush(MakeBrush(color));
        outline.BorderThickness({1,1,1,1});
        outline.HorizontalAlignment(HorizontalAlignment::Stretch);
        outline.VerticalAlignment(VerticalAlignment::Stretch);

        overlay.Children().Append(outline);
        overlay.Children().Append(vLine);
        overlay.Children().Append(hLine);
        Canvas::SetZIndex(overlay, 5000);
        target.Children().Append(overlay);
    } catch (...) {}
}

struct ContextMenuClickSubscription {
    MenuFlyoutItem item{nullptr};
    winrt::event_token token{};
};

[[clang::no_destroy]] static MenuFlyout g_activeContextMenu{nullptr};
[[clang::no_destroy]] static std::vector<ContextMenuClickSubscription>
    g_activeContextMenuClickSubscriptions;
static std::atomic<HWND> g_activeContextMenuOwnerWindow{nullptr};
static std::atomic<DWORD> g_activeContextMenuOwnerThreadId{0};
static std::atomic<HANDLE> g_activeContextMenuOwnerThreadHandle{nullptr};
static std::atomic_flag g_activeContextMenuOperation = ATOMIC_FLAG_INIT;

struct ActiveContextMenuOperationGuard {
    bool acquired = false;
    ActiveContextMenuOperationGuard() {
        acquired = !g_activeContextMenuOperation.test_and_set(
            std::memory_order_acquire);
    }
    ~ActiveContextMenuOperationGuard() {
        if (acquired) {
            g_activeContextMenuOperation.clear(std::memory_order_release);
        }
    }
    explicit operator bool() const { return acquired; }
};

static bool CloseActiveContextMenuOnCurrentThread() {
    DWORD ownerThreadId = g_activeContextMenuOwnerThreadId.load();
    if (ownerThreadId && ownerThreadId != GetCurrentThreadId()) return false;

    if (g_activeContextMenu) {
        try { g_activeContextMenu.Hide(); } catch (...) {}
    }

    bool revoked = true;
    for (auto& subscription : g_activeContextMenuClickSubscriptions) {
        if (!subscription.item || !subscription.token.value) continue;
        try {
            subscription.item.Click(subscription.token);
            subscription.token = {};
        } catch (...) {
            revoked = false;
        }
    }
    if (!revoked) return false;

    g_activeContextMenuClickSubscriptions.clear();
    g_activeContextMenu = nullptr;
    g_activeContextMenuOwnerWindow = nullptr;
    g_activeContextMenuOwnerThreadId = 0;
    CloseOwnedThreadHandle(g_activeContextMenuOwnerThreadHandle);
    return true;
}

static bool CloseActiveContextMenu(bool shutdownCleanup = false) {
    ActiveContextMenuOperationGuard operationGuard;
    if (!operationGuard) return false;

    DWORD ownerThreadId = g_activeContextMenuOwnerThreadId.load();
    HANDLE ownerThreadHandle =
        g_activeContextMenuOwnerThreadHandle.load();
    if (!ownerThreadId) return true;
    if (ownerThreadId == GetCurrentThreadId()) {
        WindowDispatchShutdownScope shutdownScope(shutdownCleanup);
        return CloseActiveContextMenuOnCurrentThread();
    }

    HWND ownerWindow = g_activeContextMenuOwnerWindow.load();
    DWORD currentWindowThreadId = 0;
    if (!IsOriginalTaskbarThreadAlive(ownerThreadHandle, ownerThreadId)) {
        ownerWindow = nullptr;
    } else if (!IsCurrentProcessTaskbarWindow(
            ownerWindow, &currentWindowThreadId, nullptr) ||
        currentWindowThreadId != ownerThreadId) {
        ownerWindow = FindCurrentProcessTaskbarWndForThread(ownerThreadId);
    }
    if (!ownerWindow) return false;

    struct ContextMenuCleanupWork {
        bool closed = false;
    } work;
    auto close = [](void* parameter) {
        auto* workItem = static_cast<ContextMenuCleanupWork*>(parameter);
        workItem->closed = CloseActiveContextMenuOnCurrentThread();
    };
    bool dispatched = shutdownCleanup
        ? RunFromWindowThreadForCleanup(ownerWindow, close, &work)
        : RunFromWindowThread(ownerWindow, close, &work);
    return dispatched && work.closed;
}

static MenuFlyoutItem MakeActionContextMenuItem(const wchar_t* glyph, const wchar_t* label,
                                                 std::function<void()> onClick) {
    MenuFlyoutItem item;
    item.Text(label);
    item.IsEnabled(true);

    try {
        FontIcon icon;
        icon.Glyph(glyph);
        icon.FontSize((double)Cfg()->buttonIconSize);
        bool useFluent = (ContextMenuIconStyle() == L"fluent_outline" || ContextMenuIconStyle() == L"fluent_filled");
        try {
            icon.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
        } catch (...) {
            try {
                icon.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
            } catch (...) {}
        }
        icon.Foreground(MakeBrush(ContextMenuIconColor()));
        icon.Opacity(1.0);
        item.Icon(icon);
    } catch (...) {}

    auto clickToken = item.Click([onClick](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
        if (TaskbarXamlCallbacksSuppressed()) return;
        try {
            onClick();
        } catch (...) {}
    });
    g_activeContextMenuClickSubscriptions.push_back(
        {item, clickToken});

    return item;
}

static std::mutex g_quickRebuildMtx;
static uint64_t g_quickRebuildGeneration = 0;
static bool g_quickRebuildWorkerRunning = false;
static bool g_quickRebuildNeedsMonitorMove = false;

static bool AcquireSettingsApplyGateForFullApply() {
    while (!g_unloading) {
        bool expected = false;
        if (g_applyingSettings.compare_exchange_weak(
                expected, true, std::memory_order_acq_rel)) {
            return true;
        }

        // A quick rebuild can be synchronously dispatching to this very UI
        // thread. Pump sent messages while waiting so the owner can finish and
        // release the gate instead of timing out or deadlocking.
        MsgWaitForMultipleObjectsEx(
            0, nullptr, 25, QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
        MSG message{};
        while (PeekMessageW(
            &message, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    return false;
}

static void FinishQuickRebuildWorker() {
    std::lock_guard<std::mutex> lock(g_quickRebuildMtx);
    g_quickRebuildWorkerRunning = false;
}

static bool ApplyQuickMonitorRebuild() {
    uint64_t restartGeneration =
        g_taskbarRestartGeneration.load(std::memory_order_acquire);
    if (TaskbarRestartSettleWindowActive()) {
        Wh_Log(L"Quick monitor rebuild: taskbar is still settling");
        return false;
    }
    if (!CloseActiveContextMenu()) {
        Wh_Log(L"Quick monitor rebuild: context-menu cleanup failed");
        return false;
    }
    if (!StopScrollTimer()) {
        Wh_Log(L"Quick monitor rebuild: scroll timer cleanup failed");
        return false;
    }

    DWORD previousOwnerThreadId = g_playerOwnerThreadId.load();
    if (previousOwnerThreadId) {
        HANDLE previousOwnerThreadHandle =
            g_playerOwnerThreadHandle.load();
        if (!IsOriginalTaskbarThreadAlive(
                previousOwnerThreadHandle, previousOwnerThreadId)) {
            Wh_Log(L"Quick monitor rebuild: old owner thread exited");
            return false;
        }
        HWND cleanupWindow = g_playerOwnerWindow.load();
        DWORD cleanupWindowThreadId = 0;
        if (!IsCurrentProcessTaskbarWindow(
                cleanupWindow, &cleanupWindowThreadId, nullptr) ||
            cleanupWindowThreadId != previousOwnerThreadId) {
            cleanupWindow = FindCurrentProcessTaskbarWndForThread(
                previousOwnerThreadId);
        }

        if (!cleanupWindow) {
            Wh_Log(
                L"Quick monitor rebuild: old XAML owner thread is unavailable");
            return false;
        }

        struct PrimaryCleanupWork {
            uint64_t restartGeneration = 0;
            bool cleaned = false;
        } cleanupWork{restartGeneration};
        bool cleanupDispatched = RunFromWindowThread(
            cleanupWindow, [](void* parameter) {
                auto* workItem =
                    static_cast<PrimaryCleanupWork*>(parameter);
                if (!g_unloading &&
                    !TaskbarRestartSettleWindowActive() &&
                    workItem->restartGeneration ==
                        g_taskbarRestartGeneration.load(
                            std::memory_order_acquire)) {
                    workItem->cleaned = RemovePlayerGrid();
                }
            }, &cleanupWork);
        if (!cleanupDispatched || !cleanupWork.cleaned) {
            Wh_Log(
                L"Quick monitor rebuild: old taskbar cleanup failed; "
                L"leaving the current widget intact");
            return false;
        }
    } else if (!MirrorPlayersEmpty() ||
               !g_primaryVisualState->xamlSubscriptionRevokers.empty() ||
               g_primaryVisualState->xamlCallbacksActive.load(
                   std::memory_order_acquire)) {
        if (restartGeneration !=
                g_taskbarRestartGeneration.load(
                    std::memory_order_acquire) ||
            TaskbarRestartSettleWindowActive() ||
            !RemovePlayerGrid()) {
            Wh_Log(L"Quick monitor rebuild: orphan cleanup failed");
            return false;
        }
    }

    if (restartGeneration !=
            g_taskbarRestartGeneration.load(std::memory_order_acquire) ||
        TaskbarRestartSettleWindowActive()) {
        Wh_Log(L"Quick monitor rebuild: taskbar restarted during cleanup");
        return false;
    }

    if (g_unloading) return false;
    HWND requestedTaskbar = FindCurrentProcessTaskbarWnd();
    if (!requestedTaskbar) return false;
    g_taskbarWnd = requestedTaskbar;

    struct PrimaryInjectionWork {
        uint64_t restartGeneration = 0;
        bool injected = false;
    } injectionWork{restartGeneration};
    bool injectionDispatched = RunFromWindowThread(
        requestedTaskbar, [](void* parameter) {
        auto* workItem = static_cast<PrimaryInjectionWork*>(parameter);
        if (g_unloading || TaskbarRestartSettleWindowActive() ||
            workItem->restartGeneration !=
                g_taskbarRestartGeneration.load(std::memory_order_acquire)) {
            return;
        }
        workItem->injected = InjectPlayerGrid();
        if (workItem->injected &&
            (Cfg()->enableTitleScrolling ||
             Cfg()->enableArtistScrolling ||
             Cfg()->showProgressBar)) {
            workItem->injected = StartScrollTimer();
        }
        g_needsUiUpdate = true;
    }, &injectionWork);
    if (!injectionDispatched || !injectionWork.injected) return false;
    bool transparencyApplied = ApplyTaskbarTransparencyToAll();
    if (!transparencyApplied || restartGeneration !=
            g_taskbarRestartGeneration.load(std::memory_order_acquire) ||
        TaskbarRestartSettleWindowActive()) {
        return false;
    }
    g_taskbarXamlCallbacksSuppressed.store(
        false, std::memory_order_release);
    return true;
}

static void QueueQuickRebuild(bool monitorMove) {
    if (g_unloading) return;

    bool startWorker = false;
    {
        std::lock_guard<std::mutex> lock(g_quickRebuildMtx);
        ++g_quickRebuildGeneration;
        g_quickRebuildNeedsMonitorMove =
            g_quickRebuildNeedsMonitorMove || monitorMove;
        if (!g_quickRebuildWorkerRunning) {
            g_quickRebuildWorkerRunning = true;
            startWorker = true;
        }
    }
    if (!startWorker) return;

    bool queued = false;
    try {
        queued = QueueAsyncTask([]() {
        try {
            while (!g_unloading) {
                // Coalesce clicks made while the flyout is closing, including
                // a position click immediately followed by a monitor click.
                Sleep(100);

                uint64_t generation = 0;
                bool monitorMove = false;
                {
                    std::lock_guard<std::mutex> lock(g_quickRebuildMtx);
                    generation = g_quickRebuildGeneration;
                    monitorMove = g_quickRebuildNeedsMonitorMove;
                    g_quickRebuildNeedsMonitorMove = false;
                }

                bool expected = false;
                if (!g_applyingSettings.compare_exchange_strong(
                        expected, true, std::memory_order_acq_rel)) {
                    break;
                }
                struct QuickApplyGateRelease {
                    ~QuickApplyGateRelease() {
                        g_applyingSettings.store(
                            false, std::memory_order_release);
                    }
                } applyGateRelease;

                bool rebuilt = false;
                if (!g_unloading &&
                    CloseActiveContextMenu() &&
                    DestroyVolumePopup()) {
                    LoadSettings();
                    if (monitorMove) {
                        rebuilt = ApplyQuickMonitorRebuild();
                    } else if (!TaskbarRestartSettleWindowActive()) {
                        HWND taskbarWindow = g_playerOwnerWindow.load();
                        if (!taskbarWindow) {
                            taskbarWindow = g_taskbarWnd.load();
                        }
                        if (taskbarWindow && IsWindow(taskbarWindow)) {
                            struct QuickSettingsWork {
                                bool applied = false;
                            } work;
                            rebuilt = RunFromWindowThread(
                                taskbarWindow, [](void* parameter) {
                                    if (g_unloading) return;
                                    auto* workItem = static_cast<
                                        QuickSettingsWork*>(parameter);
                                    workItem->applied = ApplySettings();
                                    g_needsUiUpdate = true;
                                }, &work) && work.applied;
                        }
                    }
                }

                if (rebuilt) {
                    SignalWorkerEventHandle(g_timerUpdateEvent);
                } else if (!g_unloading &&
                           g_taskbarXamlCallbacksSuppressed.load(
                               std::memory_order_acquire)) {
                    ScheduleTaskbarRebuildRetry(1000);
                    SignalWorkerEventHandle(g_timerUpdateEvent);
                }

                bool finished = false;
                {
                    std::lock_guard<std::mutex> lock(g_quickRebuildMtx);
                    if (generation == g_quickRebuildGeneration) {
                        g_quickRebuildWorkerRunning = false;
                        finished = true;
                    }
                }
                if (finished) return;
            }
        } catch (...) {
            g_applyingSettings.store(false, std::memory_order_release);
            Wh_Log(L"Quick rebuild worker: unhandled exception");
            if (!g_unloading &&
                g_taskbarXamlCallbacksSuppressed.load(
                    std::memory_order_acquire)) {
                ScheduleTaskbarRebuildRetry(1000);
                SignalWorkerEventHandle(g_timerUpdateEvent);
            }
        }

        FinishQuickRebuildWorker();
        });
    } catch (...) {
        queued = false;
    }

    if (!queued) {
        FinishQuickRebuildWorker();
        if (!g_unloading &&
            g_taskbarXamlCallbacksSuppressed.load(
                std::memory_order_acquire)) {
            ScheduleTaskbarRebuildRetry(1000);
            SignalWorkerEventHandle(g_timerUpdateEvent);
        }
    }
}

static void QueueQuickSettingsRebuild() {
    QueueQuickRebuild(false);
}

static void QueueQuickMonitorRebuild() {
    QueueQuickRebuild(true);
}


struct MediaSourceMenuEntry {
    std::wstring appId;
    std::wstring label;
    bool playing = false;
};

static std::vector<MediaSourceMenuEntry> EnumerateMediaSourcesForMenu() {
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        manager = g_sessionMgr;
    }
    if (!manager) return {};

    std::vector<MediaSourceMenuEntry> result;
    std::set<std::wstring> seen;
    try {
        for (auto const& session : manager.GetSessions()) {
            try {
                std::wstring appId(session.SourceAppUserModelId());
                if (appId.empty() || IsIgnoredMediaApp(appId)) continue;
                std::wstring key = ToLowerCopy(appId);
                if (!seen.insert(key).second) continue;

                bool playing = false;
                if (auto info = session.GetPlaybackInfo()) {
                    playing = info.PlaybackStatus() ==
                        GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                }
                result.push_back({appId, FriendlyMediaAppName(appId), playing});
            } catch (...) {}
        }
    } catch (...) {}

    std::sort(result.begin(), result.end(), [](const auto& first,
                                                const auto& second) {
        if (first.playing != second.playing) return first.playing;
        return _wcsicmp(first.label.c_str(), second.label.c_str()) < 0;
    });
    return result;
}

static void ShowMediaContextMenu(FrameworkElement const& target) {
    if (!target || g_unloading) return;

    if (!CloseActiveContextMenu()) {
        Wh_Log(L"ShowMediaContextMenu: prior menu cleanup failed");
        return;
    }
    ActiveContextMenuOperationGuard operationGuard;
    if (!operationGuard || g_unloading ||
        g_activeContextMenuOwnerThreadId.load(
            std::memory_order_acquire)) {
        return;
    }

    try {
        MenuFlyout menu;
        HANDLE ownerThreadHandle = OpenTaskbarOwnerThreadHandle(
            GetCurrentThreadId());
        if (!ownerThreadHandle) return;
        g_activeContextMenu = menu;
        g_activeContextMenuOwnerThreadId = GetCurrentThreadId();
        g_activeContextMenuOwnerThreadHandle = ownerThreadHandle;
        g_activeContextMenuOwnerWindow =
            FindCurrentProcessTaskbarWndForThread(GetCurrentThreadId());
        try {
            menu.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
        } catch (...) {}

        const bool transparent = Cfg()->backgroundType == L"none";
        std::wstring transparentLabel = transparent
            ? L"\u2713 Transparent media widget"
            : L"Transparent media widget";
        menu.Items().Append(MakeActionContextMenuItem(
            L"\uE790", transparentLabel.c_str(), [transparent]() {
                Wh_SetIntValue(L"quickBackground", transparent ? 1 : 0);
                QueueQuickSettingsRebuild();
            }));

        const bool taskbarTransparent = Cfg()->transparentTaskbar;
        std::wstring taskbarTransparentLabel = taskbarTransparent
            ? L"\u2713 Transparent taskbar"
            : L"Transparent taskbar";
        menu.Items().Append(MakeActionContextMenuItem(
            L"\uE7F4", taskbarTransparentLabel.c_str(),
            [taskbarTransparent]() {
                const bool requestedTransparency = !taskbarTransparent;
                Wh_SetIntValue(
                    L"quickTransparentTaskbar",
                    requestedTransparency ? 1 : 0);
                if (!ApplyTaskbarTransparencyToAll(
                        false, requestedTransparency)) {
                    Wh_Log(
                        L"Taskbar transparency: immediate context-menu apply "
                        L"was incomplete; queuing a settings retry");
                }
                QueueQuickSettingsRebuild();
            }));

        MenuFlyoutSubItem sourceMenu;
        sourceMenu.Text(L"Media source");
        try {
            FontIcon icon;
            icon.Glyph(L"\uE8D6");
            icon.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
            sourceMenu.Icon(icon);
        } catch (...) {}

        std::wstring preferredApp = GetPreferredMediaApp();
        std::wstring automaticLabel = preferredApp.empty()
            ? L"\u2713 Automatic (currently playing)"
            : L"Automatic (currently playing)";
        sourceMenu.Items().Append(MakeActionContextMenuItem(
            L"\uE8D6", automaticLabel.c_str(), []() {
                SelectMediaSource(L"", true);
            }));

        auto mediaSources = EnumerateMediaSourcesForMenu();
        if (!mediaSources.empty() || !preferredApp.empty()) {
            try {
                Controls::MenuFlyoutSeparator separator;
                sourceMenu.Items().Append(separator);
            } catch (...) {}
        }

        bool preferredIsActive = false;
        for (const auto& source : mediaSources) {
            bool selected = !preferredApp.empty() &&
                _wcsicmp(preferredApp.c_str(), source.appId.c_str()) == 0;
            preferredIsActive = preferredIsActive || selected;
            std::wstring label = selected
                ? std::wstring(L"\u2713 ") + source.label
                : source.label;
            if (source.playing) label += L" (playing)";
            sourceMenu.Items().Append(MakeActionContextMenuItem(
                L"\uE768", label.c_str(), [appId = source.appId]() {
                    SelectMediaSource(appId, true);
                }));
        }

        if (!preferredApp.empty() && !preferredIsActive) {
            std::wstring label = L"\u2713 " +
                FriendlyMediaAppName(preferredApp) + L" (not active)";
            sourceMenu.Items().Append(MakeActionContextMenuItem(
                L"\uE768", label.c_str(), [appId = preferredApp]() {
                    SelectMediaSource(appId, true);
                }));
        }
        menu.Items().Append(sourceMenu);

        MenuFlyoutSubItem positionMenu;
        positionMenu.Text(L"Position");
        try {
            FontIcon icon;
            icon.Glyph(L"\uE8AB");
            icon.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
            positionMenu.Icon(icon);
        } catch (...) {}

        auto appendPosition = [&](const wchar_t* label, const wchar_t* preset,
                                  int persistedValue) {
            std::wstring itemLabel = Cfg()->positionPreset == preset
                ? std::wstring(L"\u2713 ") + label
                : std::wstring(label);
            positionMenu.Items().Append(MakeActionContextMenuItem(
                L"\uE7C9", itemLabel.c_str(),
                [persistedValue]() {
                    Wh_SetIntValue(L"quickPositionPreset", persistedValue);
                    QueueQuickSettingsRebuild();
                }));
        };

        appendPosition(L"After applications", L"apps", 0);
        appendPosition(L"Left of Start", L"left", 1);
        appendPosition(IsWindowsTaskbarCentered()
                           ? L"Far left (centered Windows taskbar)"
                           : L"Middle",
                       L"middle", 2);
        appendPosition(L"Before system tray", L"right", 3);

        std::wstring customLabel = Cfg()->positionPreset == L"custom"
            ? L"\u2713 Custom (configure in Windhawk settings)"
            : L"Custom (configure in Windhawk settings)";
        positionMenu.Items().Append(MakeActionContextMenuItem(
            L"\uE713", customLabel.c_str(), []() {
                Wh_SetIntValue(L"quickPositionPreset", 4);
                QueueQuickSettingsRebuild();
            }));
        menu.Items().Append(positionMenu);

        MenuFlyoutSubItem monitorMenu;
        monitorMenu.Text(L"Monitor");
        try {
            FontIcon icon;
            icon.Glyph(L"\uE7F4");
            icon.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
            monitorMenu.Icon(icon);
        } catch (...) {}

        const bool allMonitors = Cfg()->taskbarMode == L"all";
        std::wstring allLabel = allMonitors
            ? L"\u2713 All monitors"
            : L"All monitors";
        monitorMenu.Items().Append(MakeActionContextMenuItem(
            L"\uE8B9", allLabel.c_str(), []() {
                Wh_SetIntValue(L"quickTaskbarMode", 1);
                QueueQuickMonitorRebuild();
            }));

        int secondaryNumber = 2;
        for (const auto& taskbar : EnumerateTaskbarWindows()) {
            int monitorNumber = taskbar.primary ? 1 : secondaryNumber++;
            std::wstring label = L"Monitor " + std::to_wstring(monitorNumber);
            if (taskbar.primary) label += L" (Primary)";
            if (!allMonitors && Cfg()->taskbarNumber == monitorNumber) {
                label = std::wstring(L"\u2713 ") + label;
            }

            monitorMenu.Items().Append(MakeActionContextMenuItem(
                L"\uE7F4", label.c_str(), [monitorNumber]() {
                    Wh_SetIntValue(L"quickTaskbarMode", 0);
                    Wh_SetIntValue(L"quickTaskbarNumber", monitorNumber);
                    QueueQuickMonitorRebuild();
                }));
        }
        menu.Items().Append(monitorMenu);

        if (g_unloading || TaskbarXamlCallbacksSuppressed()) {
            CloseActiveContextMenuOnCurrentThread();
            return;
        }
        menu.ShowAt(target);
    } catch (...) {
        Wh_Log(L"ShowMediaContextMenu: exception");
        CloseActiveContextMenuOnCurrentThread();
    }
}


static void RestoreConfiguredAlbumArtSize(
    FrameworkElement const& artContainer) {
    if (!artContainer) return;

    artContainer.ClearValue(FrameworkElement::WidthProperty());
    artContainer.ClearValue(FrameworkElement::HeightProperty());

    if (Cfg()->albumArtMinWidth > 0) {
        artContainer.MinWidth(
            static_cast<double>(Cfg()->albumArtMinWidth));
    } else {
        artContainer.MinWidth(0.0);
    }

    if (Cfg()->albumArtMaxWidth > 0) {
        artContainer.MaxWidth(
            static_cast<double>(Cfg()->albumArtMaxWidth));
    } else {
        artContainer.ClearValue(FrameworkElement::MaxWidthProperty());
    }

    if (Cfg()->albumArtMinHeight > 0) {
        artContainer.MinHeight(
            static_cast<double>(Cfg()->albumArtMinHeight));
    } else {
        artContainer.MinHeight(0.0);
    }

    if (Cfg()->albumArtMaxHeight > 0) {
        artContainer.MaxHeight(
            static_cast<double>(Cfg()->albumArtMaxHeight));
    } else {
        artContainer.ClearValue(FrameworkElement::MaxHeightProperty());
    }
}

static void ApplyAlbumArtAspectLayout(
    Grid const& playerGrid, Controls::Image const& image,
    int sourceWidth, int sourceHeight) {
    if (!playerGrid || !image) return;

    auto artContainerFe =
        FindChildByName(playerGrid, kArtContainerName);
    auto artContainer = artContainerFe.try_as<FrameworkElement>();
    if (!artContainer) return;

    try {
        RestoreConfiguredAlbumArtSize(artContainer);

        if (Cfg()->albumArtFitMode == L"crop") {
            image.Stretch(Stretch::UniformToFill);
            return;
        }

        if (Cfg()->albumArtFitMode == L"fit" ||
            sourceWidth <= 0 || sourceHeight <= 0) {
            image.Stretch(Stretch::Uniform);
            return;
        }

        // Adaptive mode keeps the configured square/portrait footprint, but
        // widens the Auto-sized album-art column for landscape thumbnails.
        // This moves the text start position to the right while preserving the
        // complete image instead of cropping it to 1:1.
        double baseHeight =
            Cfg()->albumArtMaxHeight > 0
                ? static_cast<double>(Cfg()->albumArtMaxHeight)
                : (Cfg()->albumArtMinHeight > 0
                       ? static_cast<double>(Cfg()->albumArtMinHeight)
                       : 36.0);
        double baseWidth =
            Cfg()->albumArtMaxWidth > 0
                ? static_cast<double>(Cfg()->albumArtMaxWidth)
                : (Cfg()->albumArtMinWidth > 0
                       ? static_cast<double>(Cfg()->albumArtMinWidth)
                       : baseHeight);

        const double aspect =
            static_cast<double>(sourceWidth) / sourceHeight;
        double targetWidth = baseWidth;

        // Treat nearly-square art as square to avoid tiny layout jumps from
        // covers whose encoded dimensions differ by only a few pixels.
        if (aspect > 1.10) {
            targetWidth = baseHeight * aspect;
            double adaptiveLimit = std::max(
                baseWidth,
                static_cast<double>(
                    Cfg()->albumArtAdaptiveMaxWidth));
            targetWidth = std::clamp(
                targetWidth, baseWidth, adaptiveLimit);
        }

        targetWidth = std::max(1.0, std::round(targetWidth));
        baseHeight = std::max(1.0, std::round(baseHeight));

        artContainer.MinWidth(targetWidth);
        artContainer.MaxWidth(targetWidth);
        artContainer.Width(targetWidth);
        artContainer.MinHeight(baseHeight);
        artContainer.MaxHeight(baseHeight);
        artContainer.Height(baseHeight);
        image.Stretch(Stretch::Uniform);
    } catch (...) {
        // Fall back to a non-cropping fixed box if a XAML property update
        // fails during a taskbar rebuild.
        try {
            RestoreConfiguredAlbumArtSize(artContainer);
            image.Stretch(Stretch::Uniform);
        } catch (...) {}
    }
}

static std::wstring ResolveProgressFillColorSetting() {
    auto preset = Cfg()->progressBarColorPreset;
    if (preset == L"red") {
        return L"255 0 51";
    }
    if (preset == L"purple") {
        return L"167 139 250";
    }
    if (preset == L"orange") {
        return L"255 149 0";
    }
    if (preset == L"accent") {
        return L"-1 -1 -1";
    }
    if (preset == L"album_art") {
        return L"-2 -2 -2";
    }
    if (preset == L"custom") {
        return Cfg()->progressBarColor;
    }
    // Bright cyan is the default. It is visible on both light and dark taskbars
    // without resembling the green commonly used for downloads or updates.
    return L"0 188 255";
}

static winrt::Windows::UI::Color ProgressFillColor() {
    BYTE alpha = static_cast<BYTE>(std::clamp(
        Cfg()->progressBarOpacity, 0, 100) * 255 / 100);
    std::wstring colorSetting = ResolveProgressFillColorSetting();
    return ParseColorWithThemeSupport(colorSetting, alpha);
}

static winrt::Windows::UI::Color ProgressTrackColor() {
    BYTE alpha = static_cast<BYTE>(std::clamp(
        Cfg()->progressTrackOpacity, 0, 100) * 255 / 100);
    return ParseColorWithThemeSupport(Cfg()->progressTrackColor, alpha);
}

static std::wstring FormatPlaybackTime(int64_t seconds) {
    seconds = std::max<int64_t>(0, seconds);
    int64_t hours = seconds / 3600;
    int64_t minutes = (seconds % 3600) / 60;
    int64_t remainingSeconds = seconds % 60;
    wchar_t buffer[64]{};
    if (hours > 0) {
        swprintf_s(
            buffer, L"%lld:%02lld:%02lld",
            static_cast<long long>(hours),
            static_cast<long long>(minutes),
            static_cast<long long>(remainingSeconds));
    } else {
        swprintf_s(
            buffer, L"%lld:%02lld",
            static_cast<long long>(minutes),
            static_cast<long long>(remainingSeconds));
    }
    return buffer;
}

static std::wstring FormatProgressTimeText(
    int64_t positionSeconds, int64_t durationSeconds) {
    positionSeconds = std::clamp<int64_t>(
        positionSeconds, 0, std::max<int64_t>(0, durationSeconds));
    if (Cfg()->progressTimeFormat == L"remaining") {
        return L"-" + FormatPlaybackTime(
            std::max<int64_t>(0, durationSeconds - positionSeconds));
    }
    if (Cfg()->progressTimeFormat == L"elapsed") {
        return FormatPlaybackTime(positionSeconds);
    }
    return FormatPlaybackTime(positionSeconds) + L" / " +
           FormatPlaybackTime(durationSeconds);
}

static double EstimatePlaybackPositionSeconds(
    bool isPlaying, int64_t positionSeconds,
    int64_t durationSeconds, ULONGLONG timelineSampleTick) {
    double estimated = static_cast<double>(std::clamp<int64_t>(
        positionSeconds, 0, std::max<int64_t>(0, durationSeconds)));
    ULONGLONG nowTick = GetTickCount64();
    if (isPlaying && timelineSampleTick && nowTick > timelineSampleTick) {
        estimated += static_cast<double>(nowTick - timelineSampleTick) /
                     1000.0;
    }
    return std::clamp(
        estimated, 0.0,
        static_cast<double>(std::max<int64_t>(0, durationSeconds)));
}

static double ProgressFractionFromPointer(
    FrameworkElement const& element,
    PointerRoutedEventArgs const& eventArgs) {
    if (!element) return 0.0;
    double width = element.ActualWidth();
    if (width <= 0.0) width = element.RenderSize().Width;
    if (width <= 0.0) return 0.0;
    double x = eventArgs.GetCurrentPoint(element).Position().X;
    return std::clamp(x / width, 0.0, 1.0);
}

static bool PointInsideElement(
    FrameworkElement const& element,
    PointerRoutedEventArgs const& eventArgs) {
    if (!element) return false;
    auto point = eventArgs.GetCurrentPoint(element).Position();
    auto size = element.RenderSize();
    return point.X >= 0.0 && point.X <= size.Width &&
           point.Y >= 0.0 && point.Y <= size.Height;
}

static void SetProgressTooltip(
    FrameworkElement const& target,
    std::shared_ptr<PlayerVisualState> const& visualState,
    int64_t positionSeconds, int64_t durationSeconds) {
    if (!target || !visualState) return;
    try {
        if (!Cfg()->progressShowTimeTooltip || durationSeconds <= 0) {
            ToolTipService::SetToolTip(target, nullptr);
            return;
        }
        if (!visualState->progressToolTip) {
            visualState->progressToolTip = Controls::ToolTip();
            ToolTipService::SetToolTip(
                target, visualState->progressToolTip);
            ToolTipService::SetPlacement(
                target, Controls::Primitives::PlacementMode::Top);
        }
        visualState->progressToolTip.Content(
            winrt::box_value(winrt::hstring(
                FormatProgressTimeText(
                    positionSeconds, durationSeconds))));
    } catch (...) {}
}

static void UpdateProgressBarThicknessForGrid(
    Grid const& playerGrid,
    std::shared_ptr<PlayerVisualState> const& visualState) {
    if (!playerGrid || !visualState) return;
    double height = static_cast<double>(
        visualState->progressHovered || visualState->progressDragging
            ? Cfg()->progressBarHoverHeight
            : Cfg()->progressBarHeight);
    try {
        if (auto element = FindChildByName(playerGrid, kProgressTrackName)) {
            element.Height(height);
            if (auto border = element.try_as<Border>()) {
                double radius = height / 2.0;
                border.CornerRadius({radius, radius, radius, radius});
            }
        }
        if (auto element = FindChildByName(playerGrid, kProgressFillName)) {
            element.Height(height);
            if (auto border = element.try_as<Border>()) {
                double radius = height / 2.0;
                border.CornerRadius({radius, radius, radius, radius});
            }
        }
    } catch (...) {}
}

static void UpdateProgressBarForGrid(
    Grid const& playerGrid,
    std::shared_ptr<PlayerVisualState> const& visualState) {
    if (!playerGrid || !visualState || !Cfg()->showProgressBar) return;

    auto hitElement = FindChildByName(playerGrid, kProgressHitName);
    auto trackElement = FindChildByName(playerGrid, kProgressTrackName);
    auto fillElement = FindChildByName(playerGrid, kProgressFillName);
    if (!hitElement || !trackElement || !fillElement) return;

    bool hasMedia = false;
    bool isPlaying = false;
    bool canSeek = false;
    int64_t positionSeconds = 0;
    int64_t durationSeconds = 0;
    ULONGLONG timelineSampleTick = 0;
    {
        std::lock_guard<std::mutex> lock(g_mediaMtx);
        hasMedia = g_media.hasMedia;
        isPlaying = g_media.isPlaying;
        canSeek = g_media.canSeek;
        positionSeconds = g_media.positionSeconds;
        durationSeconds = g_media.durationSeconds;
        timelineSampleTick = g_media.timelineSampleTick;
    }

    bool visible = hasMedia && durationSeconds > 0;
    try {
        hitElement.Visibility(
            visible ? Visibility::Visible : Visibility::Collapsed);
    } catch (...) {}
    if (!visible) {
        visualState->progressDragging = false;
        visualState->progressHovered = false;
        return;
    }

    bool dragIdentityStillValid =
        visualState->progressDragDurationSeconds == durationSeconds &&
        visualState->progressDragGeneration ==
            g_sessionGeneration.load(std::memory_order_acquire);
    if (visualState->progressDragging && !dragIdentityStillValid) {
        // A session or track changed while the pointer was held. Cancel the
        // preview so releasing the old drag can never seek the new media.
        visualState->progressDragging = false;
    }

    double fraction = 0.0;
    if (visualState->progressDragging && dragIdentityStillValid) {
        fraction = std::clamp(
            visualState->progressDragFraction, 0.0, 1.0);
        positionSeconds = static_cast<int64_t>(std::llround(
            fraction * static_cast<double>(durationSeconds)));
    } else {
        double estimatedPosition = EstimatePlaybackPositionSeconds(
            isPlaying, positionSeconds, durationSeconds,
            timelineSampleTick);
        fraction = durationSeconds > 0
            ? estimatedPosition / static_cast<double>(durationSeconds)
            : 0.0;
        positionSeconds = static_cast<int64_t>(
            std::floor(estimatedPosition));
    }

    try {
        if (auto track = trackElement.try_as<Border>()) {
            track.Background(MakeBrush(ProgressTrackColor()));
        }
        if (auto fill = fillElement.try_as<Border>()) {
            fill.Background(MakeBrush(ProgressFillColor()));
        }
        hitElement.Opacity(
            Cfg()->progressBarSeekEnabled && canSeek ? 1.0 : 0.68);
        double width = trackElement.ActualWidth();
        if (width <= 0.0) width = hitElement.ActualWidth();
        if (width <= 0.0) {
            // Layout is normally already valid on the taskbar UI thread. Only
            // force one pass for the first frame after injection.
            playerGrid.UpdateLayout();
            width = trackElement.ActualWidth();
            if (width <= 0.0) width = hitElement.ActualWidth();
        }
        fillElement.Width(std::max(0.0, width * fraction));
        UpdateProgressBarThicknessForGrid(playerGrid, visualState);
        if (visualState->progressHovered || visualState->progressDragging) {
            SetProgressTooltip(
                hitElement, visualState,
                positionSeconds, durationSeconds);
        }
    } catch (...) {}
}

static void RefreshProgressBars() {
    if (TaskbarXamlCallbacksSuppressed()) return;
    Grid primaryGrid = g_playerGrid;
    if (primaryGrid) {
        UpdateProgressBarForGrid(primaryGrid, g_primaryVisualState);
    }

    auto mirrors = SnapshotMirrorPlayers();
    for (const auto& instance : mirrors) {
        if (!instance || !instance->playerGrid || !instance->visualState ||
            instance->ownerThreadId != GetCurrentThreadId()) {
            continue;
        }
        UpdateProgressBarForGrid(
            instance->playerGrid, instance->visualState);
    }
}

static Grid BuildPlayerGrid(
    std::shared_ptr<PlayerVisualState> const& visualState) {
    if (!visualState || !visualState->xamlSubscriptionRevokers.empty()) {
        Wh_Log(L"BuildPlayerGrid: unresolved event subscriptions");
        return nullptr;
    }
    visualState->xamlCallbacksActive.store(true, std::memory_order_release);
    try {
        auto textClr = TextColor();
        auto artistClr = ArtistColor();
        auto buttonClr = ButtonColor();
        auto bgBrush = MakeBackgroundBrush();
        double phMin = (double)Cfg()->playerMinHeight;
        double phMax = (double)Cfg()->playerMaxHeight;

        bool hasTextOrButtons = Cfg()->showTrackTitle || Cfg()->showTrackArtist ||
                                (Cfg()->showMediaButtons && HasConfiguredMediaButtons());

        Border backgroundBorder;
        backgroundBorder.Name(L"TaskbarMediaPresence_Background");
        backgroundBorder.CornerRadius({
            Cfg()->cornerRadiusTL,
            Cfg()->cornerRadiusTR,
            Cfg()->cornerRadiusBR,
            Cfg()->cornerRadiusBL
        });
        backgroundBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
        backgroundBorder.VerticalAlignment(VerticalAlignment::Stretch);
        backgroundBorder.IsHitTestVisible(false);
        backgroundBorder.Visibility(Visibility::Collapsed);

        Button playerButton;
        playerButton.Name(L"TaskbarMediaPresence_OuterBorder");
        playerButton.CornerRadius({
            Cfg()->cornerRadiusTL,
            Cfg()->cornerRadiusTR,
            Cfg()->cornerRadiusBR,
            Cfg()->cornerRadiusBL
        });
        playerButton.BorderThickness({0, 0, 0, 0});
        playerButton.UseSystemFocusVisuals(false);
        playerButton.IsHitTestVisible(false);
        playerButton.HorizontalAlignment(HorizontalAlignment::Stretch);
        playerButton.VerticalAlignment(VerticalAlignment::Stretch);
        if (phMin > 0) {
            playerButton.MinHeight(phMin);
        }
        if (phMax > 0) {
            playerButton.MaxHeight(phMax);
        }

        Grid chromeFill;
        if (phMin > 0) {
            chromeFill.MinHeight(phMin);
        }
        if (phMax > 0) {
            chromeFill.MaxHeight(phMax);
        }
        chromeFill.IsHitTestVisible(false);
        playerButton.Content(chromeFill);

        if (Cfg()->showDebugBorders) {
            playerButton.BorderBrush(MakeBrush({0xFF, 0xFF, 0x00, 0x00}));
            playerButton.BorderThickness({2, 2, 2, 2});
        }

        Grid panel;
        panel.Name(kPanelGridName);
        panel.VerticalAlignment(VerticalAlignment::Center);
        panel.HorizontalAlignment(HorizontalAlignment::Stretch);
        if (hasTextOrButtons) {
            panel.Margin({4, 2, 4, 2});
        }
        AddLayoutAnchorOverlay(panel, L"TaskbarMediaPresence_DebugPanelAnchors", {0xD0, 0x00, 0xFF, 0x00});

        if (Cfg()->showDebugBorders) {
            Border panelDebugBorder;
            panelDebugBorder.BorderBrush(MakeBrush({0xFF, 0x00, 0xFF, 0x00}));
            panelDebugBorder.BorderThickness({1,1,1,1});
            panel.Children().Append(panelDebugBorder);
        }

        bool buttonsLeft = Cfg()->mirrorLayout;
        bool albumArtLeft = !Cfg()->mirrorLayout;
        bool hasText = Cfg()->showTrackTitle || Cfg()->showTrackArtist;

        ColumnDefinition colFirst, colText, colSpacer, colLast;
        colFirst.Width({1.0, GridUnitType::Auto});

        if (hasText) {
            colText.Width({1.0, GridUnitType::Star});
        } else {
            colText.Width({0.0, GridUnitType::Pixel});
        }

        colSpacer.Width({0.0, GridUnitType::Pixel});

        colLast.Width({1.0, GridUnitType::Auto});
        panel.ColumnDefinitions().Append(colFirst);
        panel.ColumnDefinitions().Append(colText);
        panel.ColumnDefinitions().Append(colSpacer);
        panel.ColumnDefinitions().Append(colLast);

        Grid artContainer{nullptr};

        if (Cfg()->showAlbumArt) {
            int iconSz = Cfg()->appIconSize;

            artContainer = Grid();
            artContainer.Name(kArtContainerName);
            artContainer.VerticalAlignment(VerticalAlignment::Center);
            artContainer.HorizontalAlignment(HorizontalAlignment::Center);

            if (Cfg()->albumArtMinWidth > 0) {
                artContainer.MinWidth((double)Cfg()->albumArtMinWidth);
            }
            if (Cfg()->albumArtMaxWidth > 0) {
                artContainer.MaxWidth((double)Cfg()->albumArtMaxWidth);
            }
            if (Cfg()->albumArtMinHeight > 0) {
                artContainer.MinHeight((double)Cfg()->albumArtMinHeight);
            }
            if (Cfg()->albumArtMaxHeight > 0) {
                artContainer.MaxHeight((double)Cfg()->albumArtMaxHeight);
            }

            double artLeftMargin = (double)Cfg()->albumArtLeftMargin;
            double artRightMargin = (double)Cfg()->albumArtRightMargin;
            artContainer.Margin({artLeftMargin, 0, artRightMargin, 0});

            artContainer.Opacity(Cfg()->albumArtOpacity / 100.0);
            artContainer.Background(MakeBrush({0x00,0x00,0x00,0x00}));
            AddLayoutAnchorOverlay(artContainer, L"TaskbarMediaPresence_DebugArtAnchors", {0xD0, 0xFF, 0xFF, 0x00});

            if (Cfg()->showDebugBorders) {
                Border artDebugBorder;
                artDebugBorder.BorderBrush(MakeBrush({0xFF, 0xFF, 0xFF, 0x00}));
                artDebugBorder.BorderThickness({2,2,2,2});
                artContainer.Children().Append(artDebugBorder);
            }

            winrt::Windows::UI::Xaml::Shapes::Rectangle placeholder;
            placeholder.Name(L"TaskbarMediaPresence_ArtPlaceholder");
            placeholder.Fill(MakeBrush({0x40,0x80,0x80,0x80}));
            Canvas::SetZIndex(placeholder, 0);

            double maxRadius = std::max({Cfg()->albumArtCornerRadiusTL, Cfg()->albumArtCornerRadiusTR,
                                         Cfg()->albumArtCornerRadiusBR, Cfg()->albumArtCornerRadiusBL});
            placeholder.RadiusX(maxRadius);
            placeholder.RadiusY(maxRadius);
            placeholder.HorizontalAlignment(HorizontalAlignment::Stretch);
            placeholder.VerticalAlignment(VerticalAlignment::Stretch);
            artContainer.Children().Append(placeholder);

            Border artBorder;
            Canvas::SetZIndex(artBorder, 1);
            artBorder.CornerRadius({
                Cfg()->albumArtCornerRadiusTL,
                Cfg()->albumArtCornerRadiusTR,
                Cfg()->albumArtCornerRadiusBR,
                Cfg()->albumArtCornerRadiusBL
            });
            artBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
            artBorder.VerticalAlignment(VerticalAlignment::Stretch);

            Controls::Image artImage;
            artImage.Name(kArtImageName);

            artImage.Stretch(
                Cfg()->albumArtFitMode == L"crop"
                    ? Stretch::UniformToFill
                    : Stretch::Uniform);

            artImage.HorizontalAlignment(HorizontalAlignment::Center);
            artImage.VerticalAlignment(VerticalAlignment::Center);

            Grid artInnerGrid;
            artInnerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
            artInnerGrid.VerticalAlignment(VerticalAlignment::Stretch);
            artInnerGrid.Children().Append(artImage);

            {

                auto clipGeo =
                    winrt::Windows::UI::Xaml::Media::RectangleGeometry();

                // SizeChanged commonly fires while settings/injection suppresses
                // normal callbacks. The guarded handler used to return at that
                // moment and leave the clip at its default 0x0 rectangle,
                // hiding every album image behind the gray placeholder.
                double initialClipWidth =
                    Cfg()->albumArtMaxWidth > 0
                        ? (double)Cfg()->albumArtMaxWidth
                        : (Cfg()->albumArtMinWidth > 0
                               ? (double)Cfg()->albumArtMinWidth
                               : 64.0);
                double initialClipHeight =
                    Cfg()->albumArtMaxHeight > 0
                        ? (double)Cfg()->albumArtMaxHeight
                        : (Cfg()->albumArtMinHeight > 0
                               ? (double)Cfg()->albumArtMinHeight
                               : 64.0);
                clipGeo.Rect(
                    {0, 0, (float)std::max(1.0, initialClipWidth),
                     (float)std::max(1.0, initialClipHeight)});
                artInnerGrid.Clip(clipGeo);

                std::weak_ptr<PlayerVisualState> weakVisualState = visualState;
                auto artSizeToken = artInnerGrid.SizeChanged(
                    [clipGeo, weakVisualState](
                        winrt::Windows::Foundation::IInspectable const& sender,
                        winrt::Windows::UI::Xaml::SizeChangedEventArgs const&) mutable {
                        auto state = weakVisualState.lock();
                        if (!state ||
                            !state->xamlCallbacksActive.load(
                                std::memory_order_acquire)) {
                            return;
                        }
                        try {
                            if (auto fe = sender.try_as<FrameworkElement>()) {
                                double width = fe.ActualWidth();
                                double height = fe.ActualHeight();
                                if (width > 0.0 && height > 0.0) {
                                    clipGeo.Rect(
                                        {0, 0, (float)width, (float)height});
                                }
                            }
                        } catch (...) {}
                    });
                TrackPlayerXamlSubscription(
                    visualState, artInnerGrid, artSizeToken,
                    [](Grid const& source, winrt::event_token token) {
                        source.SizeChanged(token);
                    });
            }

            artBorder.Child(artInnerGrid);
            artContainer.Children().Append(artBorder);

            Border artRing;
            artRing.CornerRadius({
                Cfg()->albumArtCornerRadiusTL,
                Cfg()->albumArtCornerRadiusTR,
                Cfg()->albumArtCornerRadiusBR,
                Cfg()->albumArtCornerRadiusBL
            });
            artRing.BorderThickness({1,1,1,1});
            artRing.BorderBrush(MakeBrush({0x25,0x80,0x80,0x80}));
            artContainer.Children().Append(artRing);

            if (Cfg()->showAppIcon) {
                Grid iconOverlay;
                iconOverlay.VerticalAlignment(VerticalAlignment::Stretch);
                iconOverlay.HorizontalAlignment(HorizontalAlignment::Stretch);

                Controls::Image appIconImage;
                appIconImage.Name(kAppIconImageName);
                appIconImage.Width(iconSz);
                appIconImage.Height(iconSz);
                appIconImage.Stretch(Stretch::UniformToFill);
                appIconImage.Visibility(Visibility::Collapsed);

                auto corner = Cfg()->appIconCorner;
                double margin_right  = 0, margin_bottom = 0;
                double margin_left   = 0, margin_top    = 0;
                HorizontalAlignment ha = HorizontalAlignment::Right;
                VerticalAlignment   va = VerticalAlignment::Bottom;

                if (corner == L"top_left") {
                    ha = HorizontalAlignment::Left;
                    va = VerticalAlignment::Top;
                } else if (corner == L"top_right") {
                    ha = HorizontalAlignment::Right;
                    va = VerticalAlignment::Top;
                } else if (corner == L"bottom_left") {
                    ha = HorizontalAlignment::Left;
                    va = VerticalAlignment::Bottom;
                } else {
                    ha = HorizontalAlignment::Right;
                    va = VerticalAlignment::Bottom;
                }

                appIconImage.HorizontalAlignment(ha);
                appIconImage.VerticalAlignment(va);
                appIconImage.Margin({margin_left, margin_top, margin_right, margin_bottom});

                iconOverlay.Children().Append(appIconImage);
                Canvas::SetZIndex(iconOverlay, 15);
                artContainer.Children().Append(iconOverlay);
            }

            if (Cfg()->showPauseOverlay) {
                Border pauseBorder;
                pauseBorder.Name(L"PauseIconOverlay");
                pauseBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
                pauseBorder.VerticalAlignment(VerticalAlignment::Stretch);
                BYTE opacity = (BYTE)((Cfg()->pauseOverlayOpacity * 255) / 100);
                pauseBorder.Background(MakeBrush({opacity, 0x00, 0x00, 0x00}));
                pauseBorder.Visibility(Visibility::Collapsed);
                Canvas::SetZIndex(pauseBorder, 8);

                TextBlock pauseIcon;
                pauseIcon.Text(L"\uE769");
                pauseIcon.FontFamily(Media::FontFamily(L"Segoe MDL2 Assets"));
                pauseIcon.FontSize((double)Cfg()->pauseOverlayIconSize);
                pauseIcon.Foreground(MakeBrush({0xFF, 0xFF, 0xFF, 0xFF}));
                pauseIcon.HorizontalAlignment(HorizontalAlignment::Center);
                pauseIcon.VerticalAlignment(VerticalAlignment::Center);

                pauseBorder.Child(pauseIcon);
                artInnerGrid.Children().Append(pauseBorder);
            }

            if (Cfg()->disableAlbumArtClick) {
                artContainer.IsHitTestVisible(false);
            } else {
                std::weak_ptr<PlayerVisualState> weakVisualState = visualState;
                auto artPressedToken = artContainer.PointerPressed(
                    [weakVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    try {
                        if (auto elem = sender.template try_as<UIElement>()) {
                            elem.CapturePointer(e.Pointer());
                        }
                        e.Handled(true);
                    } catch (...) {
                        Wh_Log(L"Album-art PointerPressed callback failed");
                    }
                });
                TrackPlayerXamlSubscription(
                    visualState, artContainer, artPressedToken,
                    [](Grid const& source, winrt::event_token token) {
                        source.PointerPressed(token);
                    });

                auto artReleasedToken = artContainer.PointerReleased(
                    [weakVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    try {
                        bool actuallyHovered = false;
                        if (auto elem = sender.template try_as<UIElement>()) {
                            elem.ReleasePointerCapture(e.Pointer());
                            auto pointerPoint = e.GetCurrentPoint(elem);
                            auto bounds = elem.RenderSize();
                            auto pos = pointerPoint.Position();
                            actuallyHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                        }
                        if (actuallyHovered) {
                            auto kind = e.GetCurrentPoint(nullptr)
                                            .Properties().PointerUpdateKind();
                            auto fe = sender.template try_as<FrameworkElement>();
                            if (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased) {
                                ExecuteMediaAction(Cfg()->albumArtLeftClick, fe);
                            } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased) {
                                ExecuteMediaAction(Cfg()->albumArtRightClick, fe);
                            } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::MiddleButtonReleased) {
                                ExecuteMediaAction(Cfg()->albumArtMiddleClick, fe);
                            }
                        }
                        e.Handled(true);
                    } catch (...) {
                        Wh_Log(L"Album-art PointerReleased callback failed");
                    }
                });
                TrackPlayerXamlSubscription(
                    visualState, artContainer, artReleasedToken,
                    [](Grid const& source, winrt::event_token token) {
                        source.PointerReleased(token);
                    });

                auto artDoubleTappedToken = artContainer.DoubleTapped(
                    [weakVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e) mutable {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    try {
                        ExecuteMediaAction(
                            Cfg()->albumArtLeftDoubleClick,
                            sender.template try_as<FrameworkElement>());
                        e.Handled(true);
                    } catch (...) {
                        Wh_Log(L"Album-art DoubleTapped callback failed");
                    }
                });
                TrackPlayerXamlSubscription(
                    visualState, artContainer, artDoubleTappedToken,
                    [](Grid const& source, winrt::event_token token) {
                        source.DoubleTapped(token);
                    });

                auto artWheelToken = artContainer.PointerWheelChanged(
                    [weakVisualState](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    try {
                        int delta = e.GetCurrentPoint(nullptr)
                                        .Properties().MouseWheelDelta();
                        if (!delta) return;
                        auto action = delta > 0
                            ? Cfg()->albumArtWheelUpAction
                            : Cfg()->albumArtWheelDownAction;
                        if (action == L"none") {
                            action = Cfg()->albumArtWheelAction;
                        }
                        if (action == L"none") return;
                        if (action == L"switch_tracks") {
                            if (delta > 0) SendMediaCommandAsync(1);
                            else if (delta < 0) SendMediaCommandAsync(3);
                            DispatchMediaUpdate();
                        } else if (action == L"switch_tracks_inverted") {
                            if (delta > 0) SendMediaCommandAsync(3);
                            else if (delta < 0) SendMediaCommandAsync(1);
                            DispatchMediaUpdate();
                        } else if (action == L"app_sound") {
                            AdjustCurrentMediaAppVolumeAsync(delta);
                        }
                        e.Handled(true);
                    } catch (...) {
                        Wh_Log(L"Album-art wheel callback failed");
                    }
                });
                TrackPlayerXamlSubscription(
                    visualState, artContainer, artWheelToken,
                    [](Grid const& source, winrt::event_token token) {
                        source.PointerWheelChanged(token);
                    });
            }

            artContainer.Tag(winrt::box_value(winrt::hstring(L"FluentMediaArtContainer")));

            if (albumArtLeft) {
                Grid::SetColumn(artContainer, 0);
            } else {
                Grid::SetColumn(artContainer, 3);
            }
            panel.Children().Append(artContainer);
        }

        if (Cfg()->showTrackTitle || Cfg()->showTrackArtist) {
            Border textContainer;
            textContainer.VerticalAlignment(VerticalAlignment::Center);

            if (albumArtLeft) {
                textContainer.HorizontalAlignment(HorizontalAlignment::Left);
            } else {
                textContainer.HorizontalAlignment(HorizontalAlignment::Right);
            }

            if (Cfg()->textAreaMinWidth > 0) {
                textContainer.MinWidth((double)Cfg()->textAreaMinWidth);
            }

            if (Cfg()->textAreaMaxWidth > 0) {
                textContainer.MaxWidth((double)Cfg()->textAreaMaxWidth);
            }
            if (Cfg()->textAreaMinHeight > 0) {
                textContainer.MinHeight((double)Cfg()->textAreaMinHeight);
            }
            if (Cfg()->textAreaMaxHeight > 0) {
                textContainer.MaxHeight((double)Cfg()->textAreaMaxHeight);
            }

            double leftMargin = (double)Cfg()->textAreaLeftMargin;
            double rightMargin = (double)Cfg()->textAreaRightMargin;
            textContainer.Margin({leftMargin, 0, rightMargin, 0});

            if (Cfg()->showDebugBorders) {
                textContainer.BorderBrush(MakeBrush({0xFF, 0x00, 0xFF, 0xFF}));
                textContainer.BorderThickness({1,1,1,1});
            }

            StackPanel textStack;
            textStack.Name(kTextStackName);
            textStack.Orientation(Orientation::Vertical);
            textStack.VerticalAlignment(VerticalAlignment::Center);

            if (Cfg()->enableTitleScrolling || Cfg()->enableArtistScrolling) {
                textStack.HorizontalAlignment(HorizontalAlignment::Stretch);
            } else {
                textStack.HorizontalAlignment(Cfg()->mirrorLayout ? HorizontalAlignment::Right : HorizontalAlignment::Left);
            }
            textStack.Spacing((double)Cfg()->textSpacing);

            if (Cfg()->showTrackTitle || Cfg()->showTrackArtist) {
                TextBlock titleBlock{nullptr};
                TextBlock artistBlock{nullptr};

                if (Cfg()->showTrackTitle) {
                    titleBlock = TextBlock();
                    titleBlock.Name(kTitleBlockName);
                    titleBlock.FontSize((double)Cfg()->titleFontSize);

                    std::wstring titleFontName = Cfg()->titleFont.empty() ? Cfg()->titleFontFamily : Cfg()->titleFont;
                    if (!titleFontName.empty()) {
                        try {
                            titleBlock.FontFamily(Media::FontFamily(titleFontName));
                        } catch (...) {}
                    }

                    if (!Cfg()->titleFontWeight.empty()) {
                        try {
                            auto fontWeight = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontWeight"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(Cfg()->titleFontWeight))
                                .as<winrt::Windows::UI::Text::FontWeight>();
                            titleBlock.FontWeight(fontWeight);
                        } catch (...) {}
                    }

                    if (!Cfg()->titleFontStyle.empty()) {
                        try {
                            auto fontStyle = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontStyle"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(Cfg()->titleFontStyle))
                                .as<winrt::Windows::UI::Text::FontStyle>();
                            titleBlock.FontStyle(fontStyle);
                        } catch (...) {}
                    }

                    if (Cfg()->titleCharacterSpacing != 0) {
                        titleBlock.CharacterSpacing(Cfg()->titleCharacterSpacing);
                    }

                    titleBlock.Foreground(MakeBrush(textClr));
                    titleBlock.TextWrapping(TextWrapping::NoWrap);
                    titleBlock.TextTrimming(TextTrimming::CharacterEllipsis);
                    titleBlock.TextAlignment(Cfg()->mirrorLayout ? TextAlignment::Right : TextAlignment::Left);
                }

                if (Cfg()->showTrackArtist) {
                    artistBlock = TextBlock();
                    artistBlock.Name(kArtistBlockName);
                    artistBlock.FontSize((double)Cfg()->artistFontSize);

                    std::wstring artistFontName = Cfg()->artistFont.empty() ? Cfg()->artistFontFamily : Cfg()->artistFont;
                    if (!artistFontName.empty()) {
                        try {
                            artistBlock.FontFamily(Media::FontFamily(artistFontName));
                        } catch (...) {}
                    }

                    if (!Cfg()->artistFontWeight.empty()) {
                        try {
                            auto fontWeight = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontWeight"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(Cfg()->artistFontWeight))
                                .as<winrt::Windows::UI::Text::FontWeight>();
                            artistBlock.FontWeight(fontWeight);
                        } catch (...) {}
                    }

                    if (!Cfg()->artistFontStyle.empty()) {
                        try {
                            auto fontStyle = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontStyle"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(Cfg()->artistFontStyle))
                                .as<winrt::Windows::UI::Text::FontStyle>();
                            artistBlock.FontStyle(fontStyle);
                        } catch (...) {}
                    }

                    if (Cfg()->artistCharacterSpacing != 0) {
                        artistBlock.CharacterSpacing(Cfg()->artistCharacterSpacing);
                    }

                    artistBlock.Foreground(MakeBrush(artistClr));
                    artistBlock.TextWrapping(TextWrapping::NoWrap);
                    artistBlock.TextTrimming(TextTrimming::CharacterEllipsis);
                    artistBlock.TextAlignment(Cfg()->mirrorLayout ? TextAlignment::Right : TextAlignment::Left);
                }

                auto MakeScrollView = [&](Canvas& scrollView, TextBlock& block, const wchar_t* viewName, const wchar_t* blockName, const wchar_t* cloneName) {
                    scrollView = Canvas();
                    scrollView.Name(viewName);
                    scrollView.VerticalAlignment(VerticalAlignment::Center);
                    scrollView.HorizontalAlignment(Cfg()->mirrorLayout ? HorizontalAlignment::Right : HorizontalAlignment::Left);

                    scrollView.Width(100.0);
                    block.Name(blockName);
                    block.TextTrimming(TextTrimming::None);
                    block.TextWrapping(TextWrapping::NoWrap);
                    Canvas::SetLeft(block, 0.0);
                    Canvas::SetTop(block, 0.0);
                    scrollView.Children().Append(block);

                    auto geo = winrt::Windows::UI::Xaml::Media::RectangleGeometry();
                    scrollView.Clip(geo);
                    auto weakScrollView = winrt::make_weak(scrollView);
                    std::weak_ptr<PlayerVisualState> weakVisualState = visualState;
                    auto textSizeToken = block.SizeChanged(
                        [weakScrollView, geo, weakVisualState](winrt::Windows::Foundation::IInspectable const&, SizeChangedEventArgs const& e) mutable {
                        if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                        try {
                            auto scrollView = weakScrollView.get();
                            if (!scrollView) return;
                            double h = e.NewSize().Height;
                            if (h < 1.0) h = 16.0;
                            double w = scrollView.Width();
                            scrollView.Height(h);
                            geo.Rect({0, 0, (float)w, (float)h});
                        } catch (...) {}
                    });
                    TrackPlayerXamlSubscription(
                        visualState, block, textSizeToken,
                        [](TextBlock const& source, winrt::event_token token) {
                            source.SizeChanged(token);
                        });
                };


                if (Cfg()->swapTitleArtist) {
                    if (artistBlock) {
                        if (Cfg()->enableArtistScrolling) {
                            Canvas artistScrollView;
                            MakeScrollView(artistScrollView, artistBlock, kArtistScrollViewName, kArtistBlockName, kArtistCloneName);
                            textStack.Children().Append(artistScrollView);
                        } else {
                            if (Cfg()->textAreaMaxWidth > 0) {
                                artistBlock.MaxWidth((double)Cfg()->textAreaMaxWidth);
                            }
                            textStack.Children().Append(artistBlock);
                        }
                    }
                    if (titleBlock) {
                        if (Cfg()->enableTitleScrolling) {
                            Canvas titleScrollView;
                            MakeScrollView(titleScrollView, titleBlock, kTitleScrollViewName, kTitleBlockName, kTitleCloneName);
                            textStack.Children().Append(titleScrollView);
                        } else {
                            if (Cfg()->textAreaMaxWidth > 0) {
                                titleBlock.MaxWidth((double)Cfg()->textAreaMaxWidth);
                            }
                            textStack.Children().Append(titleBlock);
                        }
                    }
                } else {
                    if (titleBlock) {
                        if (Cfg()->enableTitleScrolling) {
                            Canvas titleScrollView;
                            MakeScrollView(titleScrollView, titleBlock, kTitleScrollViewName, kTitleBlockName, kTitleCloneName);
                            textStack.Children().Append(titleScrollView);
                        } else {
                            if (Cfg()->textAreaMaxWidth > 0) {
                                titleBlock.MaxWidth((double)Cfg()->textAreaMaxWidth);
                            }
                            textStack.Children().Append(titleBlock);
                        }
                    }
                    if (artistBlock) {
                        if (Cfg()->enableArtistScrolling) {
                            Canvas artistScrollView;
                            MakeScrollView(artistScrollView, artistBlock, kArtistScrollViewName, kArtistBlockName, kArtistCloneName);
                            textStack.Children().Append(artistScrollView);
                        } else {
                            if (Cfg()->textAreaMaxWidth > 0) {
                                artistBlock.MaxWidth((double)Cfg()->textAreaMaxWidth);
                            }
                            textStack.Children().Append(artistBlock);
                        }
                    }
                }
            }

            textContainer.Child(textStack);

            Grid::SetColumn(textContainer, 1);
            panel.Children().Append(textContainer);
        }

        if (Cfg()->showMediaButtons) {
            StackPanel ctrlPanel;
            ctrlPanel.Orientation(Orientation::Horizontal);
            ctrlPanel.Spacing((double)Cfg()->buttonSpacing);
            ctrlPanel.VerticalAlignment(VerticalAlignment::Center);
            ctrlPanel.HorizontalAlignment(buttonsLeft ? HorizontalAlignment::Left : HorizontalAlignment::Right);

            std::vector<MediaButtonConfig> currentButtons;
            try {
                std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
                if (!g_mediaButtons.empty()) {
                    currentButtons = g_mediaButtons;
                } else {
                    Wh_Log(L"CreatePlayerGrid: Media buttons vector is empty, using defaults");
                    currentButtons = {
                        {MediaButtonType::Shuffle, 7},
                        {MediaButtonType::Repeat, 8},
                        {MediaButtonType::Previous, 1},
                        {MediaButtonType::PlayPause, 2},
                        {MediaButtonType::Next, 3},
                        {MediaButtonType::Volume, 13}
                    };
                }
            } catch (const std::exception& e) {
                Wh_Log(L"CreatePlayerGrid: Exception accessing media buttons (std::exception), using defaults");
                currentButtons = {
                    {MediaButtonType::Shuffle, 7},
                    {MediaButtonType::Repeat, 8},
                    {MediaButtonType::Previous, 1},
                    {MediaButtonType::PlayPause, 2},
                    {MediaButtonType::Next, 3},
                    {MediaButtonType::Volume, 13}
                };
            } catch (...) {
                Wh_Log(L"CreatePlayerGrid: Unknown exception accessing media buttons, using defaults");
                currentButtons = {
                    {MediaButtonType::Shuffle, 7},
                    {MediaButtonType::Repeat, 8},
                    {MediaButtonType::Previous, 1},
                    {MediaButtonType::PlayPause, 2},
                    {MediaButtonType::Next, 3},
                    {MediaButtonType::Volume, 13}
                };
            }

            bool hasButtons = !currentButtons.empty();
            if (hasButtons) {
                try {
                    ctrlPanel.Margin({(double)Cfg()->mediaButtonsLeftMargin, 0, (double)Cfg()->mediaButtonsRightMargin, 0});
                } catch (...) {
                    Wh_Log(L"CreatePlayerGrid: Exception setting control panel margin");
                }
            }

            if (Cfg()->showDebugBorders) {
                try {
                    Border ctrlDebugBorder;
                    ctrlDebugBorder.BorderBrush(MakeBrush({0xFF, 0xFF, 0x00, 0xFF}));
                    ctrlDebugBorder.BorderThickness({1,1,1,1});
                    Grid::SetColumn(ctrlDebugBorder, buttonsLeft ? 0 : 3);
                    panel.Children().Append(ctrlDebugBorder);
                } catch (...) {
                    Wh_Log(L"CreatePlayerGrid: Exception creating debug border");
                }
            }

            for (size_t i = 0; i < currentButtons.size(); i++) {
                try {
                    const auto& btnCfg = currentButtons[i];
                    auto btn = MakeControlButton(
                        btnCfg.cmd, false, buttonClr, visualState);
                    if (!btn) {
                        Wh_Log(L"CreatePlayerGrid: MakeControlButton returned null for button %zu", i);
                        continue;
                    }

                    switch (btnCfg.type) {
                        case MediaButtonType::Previous:
                            btn.Name(kPrevBtnName);
                            break;
                        case MediaButtonType::PlayPause:
                            btn.Name(kPlayBtnName);
                            break;
                        case MediaButtonType::Next:
                            btn.Name(kNextBtnName);
                            break;
                        case MediaButtonType::Rewind5s:
                            btn.Name(kRewindBtnName);
                            break;
                        case MediaButtonType::Forward5s:
                            btn.Name(kForwardBtnName);
                            break;
                        case MediaButtonType::Shuffle:
                            btn.Name(kShuffleBtnName);
                            break;
                        case MediaButtonType::Repeat:
                            btn.Name(kRepeatBtnName);
                            break;
                        case MediaButtonType::Volume:
                            btn.Name(kVolumeBtnName);
                            break;
                        default:
                            Wh_Log(L"CreatePlayerGrid: Unknown button type %d", static_cast<int>(btnCfg.type));
                            continue;
                    }

                    ctrlPanel.Children().Append(btn);
                } catch (const winrt::hresult_error& e) {
                    Wh_Log(L"CreatePlayerGrid: WinRT exception creating button %zu: 0x%08X", i, static_cast<uint32_t>(e.code()));
                } catch (const std::exception& e) {
                    Wh_Log(L"CreatePlayerGrid: std::exception creating button %zu", i);
                } catch (...) {
                    Wh_Log(L"CreatePlayerGrid: Unknown exception creating button %zu, skipping", i);
                }
            }

            if (buttonsLeft) {
                Grid::SetColumn(ctrlPanel, 0);
            } else {
                Grid::SetColumn(ctrlPanel, 3);
            }

            if (hasButtons) {
                panel.Children().Append(ctrlPanel);
            }
        }

        Grid wrapper;
        wrapper.Name(kGridName);
        wrapper.VerticalAlignment(VerticalAlignment::Stretch);
        wrapper.HorizontalAlignment(HorizontalAlignment::Left);
        AddLayoutAnchorOverlay(wrapper, L"TaskbarMediaPresence_DebugPlayerAnchors", {0xD0, 0xFF, 0x50, 0x50});

        // RepositionThemeTransition is intentionally not attached here. The
        // tracking layouts update wrapper.Margin from LayoutUpdated; a theme
        // transition queues animations for those corrections and can leave the
        // widget visually displaced or hidden after Start/taskbar relayouts.

        if ((hasTextOrButtons || Cfg()->showProgressBar) &&
            Cfg()->playerMinWidth > 0) {
            wrapper.MinWidth((double)Cfg()->playerMinWidth);
        }

        if (Cfg()->playerMaxWidth > 0) {
            wrapper.MaxWidth((double)Cfg()->playerMaxWidth);
        }

        if (Cfg()->playerMinHeight > 0) {
            wrapper.MinHeight((double)Cfg()->playerMinHeight);
        }

        if (Cfg()->playerMaxHeight > 0) {
            wrapper.MaxHeight((double)Cfg()->playerMaxHeight);
        }

        wrapper.Background(MakeBrush({0x00, 0, 0, 0}));

        Canvas::SetZIndex(backgroundBorder, 0);
        Canvas::SetZIndex(playerButton, 1);
        Canvas::SetZIndex(panel, 2);

        wrapper.Children().Append(backgroundBorder);
        wrapper.Children().Append(playerButton);
        wrapper.Children().Append(panel);

        Grid progressHitArea{nullptr};
        if (Cfg()->showProgressBar) {
            progressHitArea = Grid();
            progressHitArea.Name(kProgressHitName);
            progressHitArea.HorizontalAlignment(HorizontalAlignment::Stretch);
            progressHitArea.VerticalAlignment(VerticalAlignment::Bottom);
            progressHitArea.Height(static_cast<double>(std::max(
                6, Cfg()->progressBarHoverHeight + 1)));
            progressHitArea.Background(MakeBrush({0x00, 0, 0, 0}));
            if (Cfg()->progressShowTimeTooltip) {
                visualState->progressToolTip = Controls::ToolTip();
                visualState->progressToolTip.Content(
                    winrt::box_value(winrt::hstring(L"0:00 / 0:00")));
                ToolTipService::SetToolTip(
                    progressHitArea, visualState->progressToolTip);
                ToolTipService::SetPlacement(
                    progressHitArea,
                    Controls::Primitives::PlacementMode::Top);
            }

            Border progressTrack;
            progressTrack.Name(kProgressTrackName);
            progressTrack.HorizontalAlignment(HorizontalAlignment::Stretch);
            progressTrack.VerticalAlignment(VerticalAlignment::Bottom);
            progressTrack.Height(
                static_cast<double>(Cfg()->progressBarHeight));
            progressTrack.Background(MakeBrush(ProgressTrackColor()));
            double trackRadius = Cfg()->progressBarHeight / 2.0;
            progressTrack.CornerRadius(
                {trackRadius, trackRadius, trackRadius, trackRadius});

            Border progressFill;
            progressFill.Name(kProgressFillName);
            progressFill.HorizontalAlignment(HorizontalAlignment::Left);
            progressFill.VerticalAlignment(VerticalAlignment::Bottom);
            progressFill.Height(
                static_cast<double>(Cfg()->progressBarHeight));
            progressFill.Width(0.0);
            progressFill.Background(MakeBrush(ProgressFillColor()));
            progressFill.CornerRadius(
                {trackRadius, trackRadius, trackRadius, trackRadius});

            Canvas::SetZIndex(progressTrack, 0);
            Canvas::SetZIndex(progressFill, 1);
            progressHitArea.Children().Append(progressTrack);
            progressHitArea.Children().Append(progressFill);
            Canvas::SetZIndex(progressHitArea, 5);
            wrapper.Children().Append(progressHitArea);
        }

        ApplyFluentMediaButtonStyle(playerButton);
        if (!Cfg()->showDebugBorders) {
            playerButton.BorderThickness({1, 1, 1, 1});
        }

        auto isPressed = std::make_shared<bool>(false);
        auto isHovered = std::make_shared<bool>(false);
        std::weak_ptr<PlayerVisualState> weakVisualState = visualState;

        Brush playerNormalBg = MakeBackgroundBrush();
        auto weakPlayerButton = winrt::make_weak(playerButton);
        auto weakWrapper = winrt::make_weak(wrapper);

        auto updatePlayerVisualState = [weakPlayerButton, weakVisualState,
                                        isPressed, isHovered]() {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                if (auto button = weakPlayerButton.get()) {
                    ApplyPlayerButtonState(
                        button, MakeBackgroundBrush(),
                        *isHovered, *isPressed);
                }
            } catch (...) {}
        };

        ApplyPlayerButtonState(playerButton, playerNormalBg, false, false);

        if (progressHitArea) {
            auto weakProgressHit = winrt::make_weak(progressHitArea);

            auto updateProgressFromPointer =
                [weakProgressHit, weakVisualState](
                    PointerRoutedEventArgs const& eventArgs,
                    bool updatePreview) {
                auto state = weakVisualState.lock();
                auto hit = weakProgressHit.get();
                if (!state || !hit ||
                    !PlayerXamlCallbacksAllowed(weakVisualState)) {
                    return;
                }

                int64_t durationSeconds = 0;
                bool canSeek = false;
                {
                    std::lock_guard<std::mutex> lock(g_mediaMtx);
                    durationSeconds = g_media.durationSeconds;
                    canSeek = g_media.canSeek;
                }
                if (durationSeconds <= 0) return;

                double fraction = ProgressFractionFromPointer(
                    hit, eventArgs);
                int64_t pointerSeconds = static_cast<int64_t>(std::llround(
                    fraction * static_cast<double>(durationSeconds)));
                SetProgressTooltip(
                    hit, state, pointerSeconds, durationSeconds);

                if (updatePreview && state->progressDragging &&
                    Cfg()->progressBarSeekEnabled && canSeek) {
                    state->progressDragFraction = fraction;
                    if (auto owner = hit.Parent().try_as<Grid>()) {
                        UpdateProgressBarForGrid(owner, state);
                    }
                }
            };

            auto progressEnteredToken = progressHitArea.PointerEntered(
                [weakVisualState, weakProgressHit](auto const&, auto const&) {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    auto state = weakVisualState.lock();
                    auto hit = weakProgressHit.get();
                    if (!state || !hit) return;
                    try {
                        state->progressHovered = true;
                        if (auto owner = hit.Parent().try_as<Grid>()) {
                            UpdateProgressBarForGrid(owner, state);
                        }
                    } catch (...) {
                        Wh_Log(L"Progress bar: PointerEntered callback failed");
                    }
                });
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressEnteredToken,
                [](Grid const& source, winrt::event_token token) {
                    source.PointerEntered(token);
                });

            auto progressExitedToken = progressHitArea.PointerExited(
                [weakVisualState, weakProgressHit](auto const&, auto const&) {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    auto state = weakVisualState.lock();
                    auto hit = weakProgressHit.get();
                    if (!state || !hit || state->progressDragging) return;
                    try {
                        state->progressHovered = false;
                        if (auto owner = hit.Parent().try_as<Grid>()) {
                            UpdateProgressBarThicknessForGrid(owner, state);
                        }
                    } catch (...) {
                        Wh_Log(L"Progress bar: PointerExited callback failed");
                    }
                });
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressExitedToken,
                [](Grid const& source, winrt::event_token token) {
                    source.PointerExited(token);
                });

            auto progressPressedToken = progressHitArea.PointerPressed(
                [weakVisualState, weakProgressHit, updateProgressFromPointer](
                    auto const&,
                    PointerRoutedEventArgs const& eventArgs) mutable {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    auto state = weakVisualState.lock();
                    auto hit = weakProgressHit.get();
                    if (!state || !hit) return;
                    try {
                        auto kind = eventArgs.GetCurrentPoint(nullptr)
                                        .Properties().PointerUpdateKind();
                        if (kind != winrt::Windows::UI::Input::
                                PointerUpdateKind::LeftButtonPressed) {
                            return;
                        }

                        bool canSeek = false;
                        bool hasMedia = false;
                        int64_t durationSeconds = 0;
                        std::wstring title;
                        std::wstring artist;
                        uint64_t thumbnailHash = 0;
                        {
                            std::lock_guard<std::mutex> lock(g_mediaMtx);
                            canSeek = g_media.canSeek;
                            hasMedia = g_media.hasMedia;
                            durationSeconds = g_media.durationSeconds;
                            title = g_media.title;
                            artist = g_media.artist;
                            thumbnailHash = g_media.thumbnailHash;
                        }

                        // The progress area owns left clicks even when the
                        // current player can't seek, so a disabled bar never
                        // triggers the widget's configurable click action.
                        eventArgs.Handled(true);
                        if (!Cfg()->progressBarSeekEnabled || !canSeek ||
                            !hasMedia || durationSeconds <= 0) {
                            return;
                        }

                        hit.CapturePointer(eventArgs.Pointer());
                        state->progressDragging = true;
                        state->progressHovered = true;
                        state->progressDragGeneration =
                            g_sessionGeneration.load(std::memory_order_acquire);
                        state->progressDragDurationSeconds = durationSeconds;
                        state->progressDragTitle = std::move(title);
                        state->progressDragArtist = std::move(artist);
                        state->progressDragThumbnailHash = thumbnailHash;
                        updateProgressFromPointer(eventArgs, true);
                    } catch (...) {
                        Wh_Log(L"Progress bar: PointerPressed callback failed");
                    }
                });
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressPressedToken,
                [](Grid const& source, winrt::event_token token) {
                    source.PointerPressed(token);
                });

            auto progressMovedToken = progressHitArea.PointerMoved(
                [weakVisualState, updateProgressFromPointer](
                    auto const&,
                    PointerRoutedEventArgs const& eventArgs) mutable {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    auto state = weakVisualState.lock();
                    if (!state) return;
                    try {
                        updateProgressFromPointer(
                            eventArgs, state->progressDragging);
                        if (state->progressDragging) eventArgs.Handled(true);
                    } catch (...) {
                        Wh_Log(L"Progress bar: PointerMoved callback failed");
                    }
                });
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressMovedToken,
                [](Grid const& source, winrt::event_token token) {
                    source.PointerMoved(token);
                });

            auto progressReleasedToken = progressHitArea.PointerReleased(
                [weakVisualState, weakProgressHit, updateProgressFromPointer](
                    auto const&,
                    PointerRoutedEventArgs const& eventArgs) mutable {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    auto state = weakVisualState.lock();
                    auto hit = weakProgressHit.get();
                    if (!state || !hit) return;
                    try {
                        auto kind = eventArgs.GetCurrentPoint(nullptr)
                                        .Properties().PointerUpdateKind();
                        if (kind != winrt::Windows::UI::Input::
                                PointerUpdateKind::LeftButtonReleased) {
                            return;
                        }

                        eventArgs.Handled(true);
                        updateProgressFromPointer(eventArgs, true);
                        bool wasDragging = state->progressDragging;
                        double fraction = state->progressDragFraction;
                        uint64_t generation =
                            state->progressDragGeneration;
                        int64_t durationSeconds =
                            state->progressDragDurationSeconds;
                        std::wstring title = state->progressDragTitle;
                        std::wstring artist = state->progressDragArtist;
                        uint64_t thumbnailHash =
                            state->progressDragThumbnailHash;

                        try { hit.ReleasePointerCapture(eventArgs.Pointer()); }
                        catch (...) {}
                        state->progressDragging = false;
                        state->progressHovered =
                            PointInsideElement(hit, eventArgs);

                        if (auto owner = hit.Parent().try_as<Grid>()) {
                            UpdateProgressBarThicknessForGrid(owner, state);
                        }

                        if (wasDragging && durationSeconds > 0) {
                            int64_t targetSeconds =
                                static_cast<int64_t>(std::llround(
                                    std::clamp(fraction, 0.0, 1.0) *
                                    static_cast<double>(durationSeconds)));
                            SendMediaSeekAsync(
                                targetSeconds, generation,
                                std::move(title), std::move(artist),
                                thumbnailHash, durationSeconds);
                        }
                    } catch (...) {
                        state->progressDragging = false;
                        Wh_Log(L"Progress bar: PointerReleased callback failed");
                    }
                });
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressReleasedToken,
                [](Grid const& source, winrt::event_token token) {
                    source.PointerReleased(token);
                });

            auto cancelProgressDrag =
                [weakVisualState, weakProgressHit](auto const&, auto const&) {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    auto state = weakVisualState.lock();
                    auto hit = weakProgressHit.get();
                    if (!state || !hit) return;
                    try {
                        state->progressDragging = false;
                        state->progressHovered = false;
                        if (auto owner = hit.Parent().try_as<Grid>()) {
                            UpdateProgressBarForGrid(owner, state);
                        }
                    } catch (...) {
                        Wh_Log(L"Progress bar: drag cancellation callback failed");
                    }
                };

            auto progressCanceledToken = progressHitArea.PointerCanceled(
                cancelProgressDrag);
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressCanceledToken,
                [](Grid const& source, winrt::event_token token) {
                    source.PointerCanceled(token);
                });

            auto progressCaptureLostToken =
                progressHitArea.PointerCaptureLost(cancelProgressDrag);
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressCaptureLostToken,
                [](Grid const& source, winrt::event_token token) {
                    source.PointerCaptureLost(token);
                });

            auto progressDoubleTappedToken = progressHitArea.DoubleTapped(
                [weakVisualState](auto const&, DoubleTappedRoutedEventArgs const& eventArgs) {
                    if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
                    try {
                        eventArgs.Handled(true);
                    } catch (...) {
                        Wh_Log(L"Progress bar: DoubleTapped callback failed");
                    }
                });
            TrackPlayerXamlSubscription(
                visualState, progressHitArea, progressDoubleTappedToken,
                [](Grid const& source, winrt::event_token token) {
                    source.DoubleTapped(token);
                });
        }

        auto panelEnteredToken = panel.PointerEntered(
            [weakVisualState, isHovered, updatePlayerVisualState](auto const&, auto const&) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            *isHovered = true;
            updatePlayerVisualState();
        });
        TrackPlayerXamlSubscription(
            visualState, panel, panelEnteredToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerEntered(token);
            });

        auto panelExitedToken = panel.PointerExited(
            [weakVisualState, weakWrapper, isHovered, updatePlayerVisualState](auto const&, PointerRoutedEventArgs const& e) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            bool stillInside = false;
            try {
                auto wrapperElement = weakWrapper.get();
                if (!wrapperElement) return;
                auto pos = e.GetCurrentPoint(wrapperElement).Position();
                auto bounds = wrapperElement.RenderSize();
                stillInside = pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height;
            } catch (...) {}
            if (!stillInside) {
                *isHovered = false;
                updatePlayerVisualState();
            }
        });
        TrackPlayerXamlSubscription(
            visualState, panel, panelExitedToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerExited(token);
            });

        auto wrapperEnteredToken = wrapper.PointerEntered(
            [weakVisualState, isHovered, updatePlayerVisualState](auto const&, auto const&) mutable {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            *isHovered = true;
            updatePlayerVisualState();
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperEnteredToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerEntered(token);
            });

        auto wrapperExitedToken = wrapper.PointerExited(
            [weakVisualState, isHovered, updatePlayerVisualState](auto const&, auto const&) mutable {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            *isHovered = false;
            updatePlayerVisualState();
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperExitedToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerExited(token);
            });

        auto wrapperPressedToken = wrapper.PointerPressed(
            [weakVisualState, isPressed, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                if (auto elem = sender.template try_as<UIElement>()) {
                    elem.CapturePointer(e.Pointer());
                }
                *isPressed = true;
                updatePlayerVisualState();
            } catch (...) {
                Wh_Log(L"Player PointerPressed callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperPressedToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerPressed(token);
            });

        auto wrapperReleasedToken = wrapper.PointerReleased(
            [weakVisualState, isPressed, isHovered, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                *isPressed = false;
                bool actuallyHovered = false;
                if (auto elem = sender.template try_as<UIElement>()) {
                    elem.ReleasePointerCapture(e.Pointer());
                    auto pointerPoint = e.GetCurrentPoint(elem);
                    auto bounds = elem.RenderSize();
                    auto pos = pointerPoint.Position();
                    actuallyHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                }
                *isHovered = actuallyHovered;
                updatePlayerVisualState();
                if (e.Handled()) return;
                if (actuallyHovered) {
                    auto kind = e.GetCurrentPoint(nullptr)
                                    .Properties().PointerUpdateKind();
                    auto fe = sender.template try_as<FrameworkElement>();
                    if (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased) {
                        ExecuteMediaAction(Cfg()->playerLeftClick, fe);
                    } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased) {
                        ExecuteMediaAction(Cfg()->playerRightClick, fe);
                    } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::MiddleButtonReleased) {
                        ExecuteMediaAction(Cfg()->playerMiddleClick, fe);
                    }
                }
            } catch (...) {
                Wh_Log(L"Player PointerReleased callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperReleasedToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerReleased(token);
            });

        auto wrapperCanceledToken = wrapper.PointerCanceled(
            [weakVisualState, isPressed, isHovered, updatePlayerVisualState](auto const&, auto const&) mutable {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            *isPressed = false;
            *isHovered = false;
            updatePlayerVisualState();
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperCanceledToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerCanceled(token);
            });

        auto wrapperCaptureLostToken = wrapper.PointerCaptureLost(
            [weakVisualState, isPressed, isHovered, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                *isPressed = false;
                if (auto elem = sender.template try_as<UIElement>()) {
                    auto pointerPoint = e.GetCurrentPoint(elem);
                    auto bounds = elem.RenderSize();
                    auto pos = pointerPoint.Position();
                    *isHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                }
                updatePlayerVisualState();
            } catch (...) {
                *isHovered = false;
                Wh_Log(L"Player PointerCaptureLost callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperCaptureLostToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerCaptureLost(token);
            });

        auto wrapperDoubleTappedToken = wrapper.DoubleTapped(
            [weakVisualState, isPressed, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e) mutable {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                *isPressed = false;
                updatePlayerVisualState();
                ExecuteMediaAction(
                    Cfg()->playerLeftDoubleClick,
                    sender.template try_as<FrameworkElement>());
                e.Handled(true);
            } catch (...) {
                Wh_Log(L"Player DoubleTapped callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperDoubleTappedToken,
            [](Grid const& source, winrt::event_token token) {
                source.DoubleTapped(token);
            });

        wrapper.Tag(winrt::box_value(winrt::hstring(L"TaskbarMediaPresenceBarWrapper")));

        auto wrapperWheelToken = wrapper.PointerWheelChanged(
            [weakVisualState](auto const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (!PlayerXamlCallbacksAllowed(weakVisualState)) return;
            try {
                int delta = e.GetCurrentPoint(nullptr)
                                .Properties().MouseWheelDelta();
                if (!delta) return;
                auto action = delta > 0
                    ? Cfg()->playerWheelUpAction
                    : Cfg()->playerWheelDownAction;
                if (action == L"none") {
                    action = Cfg()->playerWheelAction;
                }
                if (action == L"none") return;
                if (action == L"switch_tracks") {
                    if (delta > 0) SendMediaCommandAsync(1);
                    else if (delta < 0) SendMediaCommandAsync(3);
                    DispatchMediaUpdate();
                } else if (action == L"switch_tracks_inverted") {
                    if (delta > 0) SendMediaCommandAsync(3);
                    else if (delta < 0) SendMediaCommandAsync(1);
                    DispatchMediaUpdate();
                } else if (action == L"app_sound") {
                    AdjustCurrentMediaAppVolumeAsync(delta);
                }
                e.Handled(true);
            } catch (...) {
                Wh_Log(L"Player wheel callback failed");
            }
        });
        TrackPlayerXamlSubscription(
            visualState, wrapper, wrapperWheelToken,
            [](Grid const& source, winrt::event_token token) {
                source.PointerWheelChanged(token);
            });

        try {
            winrt::Windows::UI::Xaml::Interop::TypeName gridType;
            gridType.Name = L"Windows.UI.Xaml.Controls.Grid";
            gridType.Kind = winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata;

            winrt::Windows::UI::Xaml::Style wrapperStyle(gridType);
            wrapper.Style(wrapperStyle);
        } catch (...) {}

        return wrapper;
    } catch (...) {
        Wh_Log(L"BuildPlayerGrid: Exception occurred");
        RevokePlayerXamlSubscriptions(visualState);
        return nullptr;
    }
}

struct InjectionTarget {
    Grid grid;
    int  insertCol = 0;
};

static int RemovePlayerGridChildren(Grid const& targetGrid) {
    if (!targetGrid) return -1;

    int firstCol = -1;
    for (int i = (int)targetGrid.Children().Size() - 1; i >= 0; --i) {
        auto fe = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == kGridName) {
            if (firstCol < 0) firstCol = Grid::GetColumn(fe);
            try { targetGrid.Children().RemoveAt(i); } catch (...) {}
        }
    }
    return firstCol;
}

static void InsertTrackedPlayerColumn(
    Grid const& targetGrid,
    int requestedColumn,
    int& playerColumn,
    bool& columnInserted,
    bool& shiftCommitted,
    std::vector<ShiftedColumnChild>& shiftedChildren) {
    auto columns = targetGrid.ColumnDefinitions();
    auto children = targetGrid.Children();
    int actualColumn = std::clamp(
        requestedColumn, 0, static_cast<int>(columns.Size()));
    shiftedChildren.clear();
    shiftedChildren.reserve(children.Size());
    shiftCommitted = false;

    ColumnDefinition newColumn;
    newColumn.Width({1.0, GridUnitType::Auto});
    if (actualColumn == static_cast<int>(columns.Size())) {
        columns.Append(newColumn);
    } else {
        columns.InsertAt(actualColumn, newColumn);
    }
    playerColumn = actualColumn;
    columnInserted = true;

    if (actualColumn < static_cast<int>(columns.Size()) - 1) {
        for (uint32_t index = 0; index < children.Size(); ++index) {
            auto child = children.GetAt(index).try_as<FrameworkElement>();
            if (!child) continue;
            int originalColumn = Grid::GetColumn(child);
            if (originalColumn < actualColumn) continue;
            shiftedChildren.push_back({child, originalColumn});
            Grid::SetColumn(child, originalColumn + 1);
        }
    }
    shiftCommitted = true;
    shiftedChildren.clear();
}

static bool RemoveTrackedPlayerColumn(
    Grid const& targetGrid,
    int& playerColumn,
    bool& columnInserted,
    bool& shiftCommitted,
    std::vector<ShiftedColumnChild>& shiftedChildren) {
    if (!columnInserted) {
        playerColumn = -1;
        shiftCommitted = false;
        shiftedChildren.clear();
        return true;
    }
    if (!targetGrid || playerColumn < 0 ||
        playerColumn >=
            static_cast<int>(targetGrid.ColumnDefinitions().Size())) {
        return false;
    }

    if (!shiftCommitted) {
        bool restored = true;
        for (auto iterator = shiftedChildren.rbegin();
             iterator != shiftedChildren.rend(); ++iterator) {
            try {
                if (iterator->element) {
                    Grid::SetColumn(
                        iterator->element, iterator->originalColumn);
                }
            } catch (...) {
                restored = false;
            }
        }
        if (!restored) return false;
    }

    std::vector<ShiftedColumnChild> removalChanges;
    if (shiftCommitted) {
        try {
            auto children = targetGrid.Children();
            removalChanges.reserve(children.Size());
            for (uint32_t index = 0; index < children.Size(); ++index) {
                auto child =
                    children.GetAt(index).try_as<FrameworkElement>();
                if (!child) continue;
                int originalColumn = Grid::GetColumn(child);
                if (originalColumn <= playerColumn) continue;
                removalChanges.push_back({child, originalColumn});
                Grid::SetColumn(child, originalColumn - 1);
            }
        } catch (...) {
            for (auto iterator = removalChanges.rbegin();
                 iterator != removalChanges.rend(); ++iterator) {
                try {
                    if (iterator->element) {
                        Grid::SetColumn(
                            iterator->element, iterator->originalColumn);
                    }
                } catch (...) {}
            }
            return false;
        }
    }

    try {
        targetGrid.ColumnDefinitions().RemoveAt(playerColumn);
    } catch (...) {
        for (auto iterator = removalChanges.rbegin();
             iterator != removalChanges.rend(); ++iterator) {
            try {
                if (iterator->element) {
                    Grid::SetColumn(
                        iterator->element, iterator->originalColumn);
                }
            } catch (...) {}
        }
        return false;
    }

    playerColumn = -1;
    columnInserted = false;
    shiftCommitted = false;
    shiftedChildren.clear();
    return true;
}

static void RemoveAnchorDebugOverlays(Grid const& targetGrid) {
    if (!targetGrid) return;
    for (int i = (int)targetGrid.Children().Size() - 1; i >= 0; --i) {
        auto fe = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == kAnchorOverlayName) {
            try { targetGrid.Children().RemoveAt(i); } catch (...) {}
        }
    }
}

static void UpdateAnchorDebugOverlay(Grid const& targetGrid, FrameworkElement const& targetElem) {
    if (!targetGrid) return;

    if (!Cfg()->showLayoutAnchors || !targetElem) {
        RemoveAnchorDebugOverlays(targetGrid);
        return;
    }

    try {
        Border overlay{nullptr};
        for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
            auto fe = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
            if (fe && fe.Name() == kAnchorOverlayName) {
                overlay = fe.try_as<Border>();
                break;
            }
        }

        if (!overlay) {
            overlay = Border();
            overlay.Name(kAnchorOverlayName);
            overlay.IsHitTestVisible(false);
            overlay.BorderBrush(MakeBrush({0xE0, 0x00, 0xA2, 0xFF}));
            overlay.BorderThickness({2,2,2,2});
            overlay.Background(MakeBrush({0x20, 0x00, 0xA2, 0xFF}));
            overlay.HorizontalAlignment(HorizontalAlignment::Left);
            overlay.VerticalAlignment(VerticalAlignment::Top);
            Canvas::SetZIndex(overlay, 5001);
            targetGrid.Children().Append(overlay);
        }

        auto transform = targetElem.TransformToVisual(targetGrid);
        auto point = transform.TransformPoint({0, 0});
        overlay.Width(std::max(1.0, targetElem.ActualWidth()));
        overlay.Height(std::max(1.0, targetElem.ActualHeight()));
        overlay.Margin({point.X, point.Y, 0, 0});
        overlay.Visibility(Visibility::Visible);
    } catch (...) {
        RemoveAnchorDebugOverlays(targetGrid);
    }
}

static const wchar_t* const kStartButtonNames[] = {
    L"StartButton",
    L"StartMenuButton",
    L"StartMenuLaunchButton",
    L"LaunchListButton",
};

static Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    FrameworkElement taskbarFrame = nullptr;
    int count = VisualTreeHelper::GetChildrenCount(root);

    for (int i = 0; i < count; i++) {
        auto c = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (c) {
            auto className = winrt::get_class_name(c);
            if (className == L"Taskbar.TaskbarFrame") {
                taskbarFrame = c;
                break;
            }
        }
    }

    if (!taskbarFrame) {
        return nullptr;
    }

    auto rootGrid = FindChildByName(taskbarFrame, L"RootGrid");

    return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
}

static FrameworkElement FindElementInRepeater(FrameworkElement const& repeater, const wchar_t* const* names, int nameCount) {
    if (!repeater) return nullptr;

    int childCount = VisualTreeHelper::GetChildrenCount(repeater);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;

        for (int j = 0; j < nameCount; j++) {
            if (child.Name() == names[j]) return child;
        }
    }

    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;

        int subChildCount = VisualTreeHelper::GetChildrenCount(child);
        for (int k = 0; k < subChildCount; k++) {
            auto subChild = VisualTreeHelper::GetChild(child, k).try_as<FrameworkElement>();
            if (!subChild) continue;

            for (int j = 0; j < nameCount; j++) {
                if (subChild.Name() == names[j]) return subChild;
            }
        }
    }

    return nullptr;
}

static FrameworkElement FindElementByClassName(FrameworkElement const& parent, const wchar_t* className) {
    if (!parent) return nullptr;

    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;

        auto childClassName = winrt::get_class_name(child);
        if (childClassName == className) return child;
    }

    return nullptr;
}

static FrameworkElement FindNthElementByClassName(FrameworkElement const& parent, const wchar_t* className, int index) {
    if (!parent) return nullptr;

    int foundCount = 0;
    int childCount = VisualTreeHelper::GetChildrenCount(parent);

    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;

        auto childClassName = winrt::get_class_name(child);
        if (childClassName == className) {
            if (foundCount == index) return child;
            foundCount++;
        }
    }

    return nullptr;
}

static FrameworkElement FindChildByClassName(FrameworkElement const& parent, const wchar_t* className, int depth = 32) {
    if (!parent || depth <= 0) return nullptr;

    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;

        if (winrt::get_class_name(child) == className) return child;
        if (auto found = FindChildByClassName(child, className, depth - 1)) return found;
    }

    return nullptr;
}

static double FindLeftmostVisibleChildX(FrameworkElement const& parent, UIElement const& relativeTo, int depth = 3) {
    if (!parent || !relativeTo || depth < 0) return -1.0;

    double leftmost = -1.0;
    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;

        try {
            if (child.Visibility() == Visibility::Visible && child.ActualWidth() > 1.0) {
                auto transform = child.TransformToVisual(relativeTo);
                auto point = transform.TransformPoint({0, 0});
                if (point.X >= 0.0 && (leftmost < 0.0 || point.X < leftmost))
                    leftmost = point.X;
            }
        } catch (...) {}

        double nested = FindLeftmostVisibleChildX(child, relativeTo, depth - 1);
        if (nested >= 0.0 && (leftmost < 0.0 || nested < leftmost))
            leftmost = nested;
    }

    return leftmost;
}

// TaskbarFrameRepeater can stretch across most of the taskbar even when its
// actual buttons occupy only a small area. Use the right edge of its compact
// visible descendants so "After applications" follows the final app button,
// not the repeater's tray-side edge.
static double FindRightmostTaskbarItemEdge(
    FrameworkElement const& parent, FrameworkElement const& relativeTo,
    int depth = 8) {
    if (!parent || !relativeTo || depth < 0) return -1.0;

    double rightmost = -1.0;
    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; ++i) {
        auto child = VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;

        try {
            if (child.Visibility() == Visibility::Visible) {
                double width = child.ActualWidth();
                double height = child.ActualHeight();
                std::wstring typeName(
                    winrt::get_class_name(child));
                std::wstring elementName(child.Name());
                bool isTaskbarItem =
                    typeName.find(L"Button") != std::wstring::npos ||
                    typeName.find(L"TaskList") != std::wstring::npos ||
                    typeName.find(L"TaskbarExtensionElement") !=
                        std::wstring::npos ||
                    elementName == L"StartButton" ||
                    elementName == L"SearchBoxButton" ||
                    elementName == L"AugmentedEntryPointButton";
                // Individual Start/Search/app buttons are compact. Excluding
                // stretched layout panels prevents selecting the tray edge.
                if (isTaskbarItem && width > 1.0 && width <= 400.0 &&
                    height > 1.0) {
                    auto transform = child.TransformToVisual(relativeTo);
                    auto point = transform.TransformPoint({0, 0});
                    double edge = point.X + width;
                    if (point.X >= 0.0 &&
                        edge <= relativeTo.ActualWidth() + 4.0) {
                        rightmost = std::max(rightmost, edge);
                    }
                }
            }
        } catch (...) {}

        double nested = FindRightmostTaskbarItemEdge(
            child, relativeTo, depth - 1);
        rightmost = std::max(rightmost, nested);
    }

    return rightmost;
}

static double FindSystemTrayLeftEdge(
    FrameworkElement const& root, FrameworkElement const& relativeTo) {
    if (!root || !relativeTo) return -1.0;
    try {
        auto tray = FindChildByName(root, L"SystemTrayFrameGrid");
        if (!tray) return -1.0;
        auto transform = tray.TransformToVisual(relativeTo);
        return transform.TransformPoint({0, 0}).X;
    } catch (...) {
        return -1.0;
    }
}

static FrameworkElement FindTrayElement(FrameworkElement const& trayGrid, FrameworkElement const& root, const wchar_t* name) {
    auto elem = FindChildByName(trayGrid, name);
    if (!elem) elem = FindChildByName(root, name);
    return elem;
}

static bool IsStartButtonModActive(FrameworkElement const& root) {
    try {
        auto rootGrid = FindTaskbarRootGrid(root);
        if (!rootGrid) return false;

        auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
        if (!repeater) return false;

        static const wchar_t* kStartNames[] = {L"StartButton"};
        auto startButton = FindElementInRepeater(repeater, kStartNames, 1);
        if (!startButton) return false;

        auto margin = startButton.Margin();
        return margin.Right < -10.0;
    } catch (...) {
        return false;
    }
}

static double GetStartButtonAdjustment(FrameworkElement const& root) {
    try {
        auto rootGrid = FindTaskbarRootGrid(root);
        if (!rootGrid) return 0.0;

        auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
        if (!repeater) return 0.0;

        static const wchar_t* kStartNames[] = {L"StartButton"};
        auto startButton = FindElementInRepeater(repeater, kStartNames, 1);
        if (!startButton) return 0.0;

        return startButton.ActualWidth();
    } catch (...) {
        return 0.0;
    }
}

static InjectionTarget ResolveInjectionTarget(
    FrameworkElement const& root,
    std::wstring_view position)
{
    auto trayFrame = FindChildByName(root, L"SystemTrayFrameGrid");
    if (auto trayGrid = trayFrame ? trayFrame.try_as<Grid>() : nullptr) {

        int col = -1;
        if      (position == L"tray_right")
            col = (int)trayGrid.ColumnDefinitions().Size();
        else if (position == L"tray_left")
            col = 0;
        else if (position == L"tray_before_clock") {
            auto clockBtn = FindChildByName(trayGrid, L"NotificationCenterButton");
            if (!clockBtn) clockBtn = FindChildByName(root, L"NotificationCenterButton");
            col = clockBtn ? Grid::GetColumn(clockBtn) : -1;
        }
        else if (position == L"tray_after_clock") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            col = showDesktop ? Grid::GetColumn(showDesktop) : -1;
        }
        else if (position == L"tray_before_omni_left") {
            auto omniBtn = FindChildByName(trayGrid, L"ControlCenterButton");
            if (!omniBtn) omniBtn = FindChildByName(root, L"ControlCenterButton");
            col = omniBtn ? Grid::GetColumn(omniBtn) : -1;
        }
        else if (position == L"tray_before_omni_right") {
            auto omniBtn = FindChildByName(trayGrid, L"ControlCenterButton");
            if (!omniBtn) omniBtn = FindChildByName(root, L"ControlCenterButton");
            if (omniBtn) col = Grid::GetColumn(omniBtn) + 1;
            else col = -1;
        }
        else if (position == L"tray_language_left") {
            auto languageBtn = FindTrayElement(trayGrid, root, L"NonActivatableStack");
            col = languageBtn ? Grid::GetColumn(languageBtn) : -1;
        }
        else if (position == L"tray_language_right") {
            auto languageBtn = FindTrayElement(trayGrid, root, L"NonActivatableStack");
            col = languageBtn ? Grid::GetColumn(languageBtn) + 1 : -1;
        }
        else if (position == L"tray_hidden_icons_left") {
            auto hiddenIconsBtn = FindTrayElement(trayGrid, root, L"NotifyIconStack");
            col = hiddenIconsBtn ? Grid::GetColumn(hiddenIconsBtn) : -1;
        }
        else if (position == L"tray_hidden_icons_right") {
            auto hiddenIconsBtn = FindTrayElement(trayGrid, root, L"NotifyIconStack");
            col = hiddenIconsBtn ? Grid::GetColumn(hiddenIconsBtn) + 1 : -1;
        }
        else if (position == L"tray_icons_left") {
            auto trayIcons = FindTrayElement(trayGrid, root, L"NotificationAreaIcons");
            col = trayIcons ? Grid::GetColumn(trayIcons) : -1;
        }
        else if (position == L"tray_icons_right") {
            auto trayIcons = FindTrayElement(trayGrid, root, L"NotificationAreaIcons");
            col = trayIcons ? Grid::GetColumn(trayIcons) + 1 : -1;
        }
        else if (position == L"tray_after_showdesktop_left") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            col = showDesktop ? Grid::GetColumn(showDesktop) : -1;
        }
        else if (position == L"tray_after_showdesktop_right") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            if (showDesktop) col = Grid::GetColumn(showDesktop) + 1;
            else col = (int)trayGrid.ColumnDefinitions().Size();
        }

        if (col >= 0) {
            return {trayGrid, col};
        }
    }

    if (position == L"taskbar_left_start"  ||
        position == L"taskbar_right_start" ||
        position == L"taskbar_after_apps" ||
        position == L"taskbar_after_search_left"||
        position == L"taskbar_after_search_right"||
        position == L"taskbar_after_taskview_left"||
        position == L"taskbar_after_taskview_right"||
        position == L"taskbar_after_widgets_left"||
        position == L"taskbar_after_widgets_right"||
        position == L"taskbar_far_left_reserved" ||
        position == L"taskbar_left_edge"   ||
        position == L"taskbar_center_edge" ||
        position == L"taskbar_right_edge")
    {
        auto rootGrid = FindTaskbarRootGrid(root);
        if (!rootGrid) {
            auto tf2 = FindChildByName(root, L"SystemTrayFrameGrid");
            if (auto tg2 = tf2 ? tf2.try_as<Grid>() : nullptr)
                return {tg2, (int)tg2.ColumnDefinitions().Size()};
            return {};
        }

        return {rootGrid, -1};
    }

    return {};
}

static void ApplyCustomPlayerOffset(Grid const& playerGrid) {
    if (!playerGrid || Cfg()->positionPreset != L"custom" ||
        (!Cfg()->customOffsetX && !Cfg()->customOffsetY)) {
        return;
    }
    TranslateTransform offset;
    offset.X((double)Cfg()->customOffsetX);
    offset.Y((double)Cfg()->customOffsetY);
    playerGrid.RenderTransform(offset);
}

static FrameworkElement FindTrackingElementForPosition(
    FrameworkElement const& root, Grid const& targetGrid,
    const std::wstring& position, bool& placeBefore) {
    placeBefore = false;
    auto repeater = FindChildByName(targetGrid, L"TaskbarFrameRepeater");
    if (!repeater) return nullptr;

    if (position == L"taskbar_far_left_reserved") {
        placeBefore = true;
        return repeater;
    }
    if (position == L"taskbar_after_apps") return repeater;
    if (position == L"taskbar_left_start" || position == L"taskbar_right_start") {
        placeBefore = position == L"taskbar_left_start";
        return FindElementInRepeater(
            repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
    }
    if (position == L"taskbar_after_search_left" ||
        position == L"taskbar_after_search_right") {
        placeBefore = position == L"taskbar_after_search_left";
        return FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
    }
    if (position == L"taskbar_after_taskview_left" ||
        position == L"taskbar_after_taskview_right") {
        placeBefore = position == L"taskbar_after_taskview_left";
        return FindNthElementByClassName(
            repeater, L"Taskbar.ExperienceToggleButton", 1);
    }
    if (position == L"taskbar_after_widgets_left" ||
        position == L"taskbar_after_widgets_right") {
        placeBefore = position == L"taskbar_after_widgets_left";
        auto element = FindChildByName(repeater, L"AugmentedEntryPointButton");
        if (!element) {
            element = FindChildByClassName(
                repeater, L"Taskbar.AugmentedEntryPointButton");
        }
        return element;
    }
    return nullptr;
}

static bool RemoveMirrorPlayerOnCurrentThread(
    std::shared_ptr<MirrorPlayerInstance> const& mirror) {
    if (!mirror) return true;
    if (mirror->ownerThreadId &&
        mirror->ownerThreadId != GetCurrentThreadId()) {
        return false;
    }

    if (!DeactivatePlayerXamlCallbacks(
            mirror->playerGrid, mirror->visualState)) {
        Wh_Log(L"RemoveMirrorPlayer: player callback revocation failed");
        return false;
    }

    if (mirror->layoutUpdatedToken.value) {
        if (!mirror->targetGrid) return false;
        try {
            mirror->targetGrid.LayoutUpdated(mirror->layoutUpdatedToken);
            mirror->layoutUpdatedToken = {};
        } catch (...) {
            return false;
        }
    }

    try {
        if (mirror->trackedElement) {
            mirror->trackedElement.Margin(mirror->trackedOriginalMargin);
        }
    } catch (...) {}

    bool playerDetached = !mirror->playerGrid;
    try {
        if (mirror->targetGrid) {
            RemovePlayerGridChildren(mirror->targetGrid);
            if (!RemoveTrackedPlayerColumn(
                    mirror->targetGrid, mirror->playerColumn,
                    mirror->playerColumnInserted,
                    mirror->playerColumnShiftCommitted,
                    mirror->playerColumnShiftedChildren)) {
                return false;
            }

            playerDetached = true;
            for (uint32_t index = 0;
                 index < mirror->targetGrid.Children().Size(); ++index) {
                auto child = mirror->targetGrid.Children()
                                 .GetAt(index)
                                 .try_as<FrameworkElement>();
                if (child && child.Name() == kGridName) {
                    playerDetached = false;
                    break;
                }
            }
        }
    } catch (...) {
        playerDetached = false;
    }
    if (!playerDetached) return false;

    mirror->playerGrid = nullptr;
    mirror->targetGrid = nullptr;
    mirror->trackedElement = nullptr;
    mirror->playerColumn = -1;
    if (mirror->ownerThreadHandle) {
        CloseHandle(mirror->ownerThreadHandle);
        mirror->ownerThreadHandle = nullptr;
    }
    return true;
}

static bool InjectMirrorPlayer(HWND taskbarWindow) {
    std::shared_ptr<MirrorPlayerInstance> instance;
    try {
        auto xamlRoot = GetTaskbarXamlRoot(taskbarWindow);
        if (!xamlRoot) return false;
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) return false;

        auto [targetGrid, insertCol] = ResolveInjectionTarget(root, Cfg()->position);
        if (!targetGrid) return false;

        instance = std::make_shared<MirrorPlayerInstance>();
        instance->taskbarWindow = taskbarWindow;
        instance->ownerThreadId = GetCurrentThreadId();
        instance->ownerThreadHandle = OpenTaskbarOwnerThreadHandle(
            instance->ownerThreadId);
        if (!instance->ownerThreadHandle) return false;
        instance->targetGrid = targetGrid;
        // Track cleanup ownership before BuildPlayerGrid installs delegates.
        AddMirrorPlayer(instance);
        instance->playerGrid = BuildPlayerGrid(instance->visualState);
        if (!instance->playerGrid) {
            if (RemoveMirrorPlayerOnCurrentThread(instance)) {
                RemoveMirrorPlayerReference(instance);
            }
            return false;
        }
        ApplyCustomPlayerOffset(instance->playerGrid);

        RemovePlayerGridChildren(targetGrid);
        bool isTrayGrid = targetGrid.Name() == L"SystemTrayFrameGrid";
        if (isTrayGrid) {
            InsertTrackedPlayerColumn(
                targetGrid, insertCol, instance->playerColumn,
                instance->playerColumnInserted,
                instance->playerColumnShiftCommitted,
                instance->playerColumnShiftedChildren);
            instance->playerGrid.Margin({
                (double)Cfg()->playerMarginLeft, 0,
                (double)Cfg()->playerMarginRight, 0});
            Grid::SetColumn(instance->playerGrid, insertCol);
            targetGrid.Children().Append(instance->playerGrid);
        } else {
            bool edgePosition =
                Cfg()->position == L"taskbar_left_edge" ||
                Cfg()->position == L"taskbar_center_edge" ||
                Cfg()->position == L"taskbar_right_edge";

            instance->playerGrid.HorizontalAlignment(HorizontalAlignment::Left);
            if (edgePosition) {
                if (Cfg()->position == L"taskbar_center_edge") {
                    instance->playerGrid.HorizontalAlignment(HorizontalAlignment::Center);
                } else if (Cfg()->position == L"taskbar_right_edge") {
                    instance->playerGrid.HorizontalAlignment(HorizontalAlignment::Right);
                }
                instance->playerGrid.Margin({
                    (double)Cfg()->playerMarginLeft, 0,
                    (double)Cfg()->playerMarginRight, 0});
            } else {
                bool placeBefore = false;
                auto tracked = FindTrackingElementForPosition(
                    root, targetGrid, Cfg()->position, placeBefore);
                if (!tracked) {
                    tracked = FindTrackingElementForPosition(
                        root, targetGrid, L"taskbar_after_apps", placeBefore);
                }
                if (tracked) {
                    instance->trackedElement = tracked;
                    instance->trackedOriginalMargin = tracked.Margin();
                    std::weak_ptr<MirrorPlayerInstance> weakInstance =
                        instance;
                    instance->layoutUpdatedToken = targetGrid.LayoutUpdated(
                        [weakInstance, placeBefore](auto const&, auto const&) {
                            auto instance = weakInstance.lock();
                            if (!instance) return;
                            if (TaskbarXamlCallbacksSuppressed() ||
                                !instance->visualState ||
                                !instance->visualState->xamlCallbacksActive.load(
                                    std::memory_order_acquire) ||
                                !instance->playerGrid ||
                                !instance->trackedElement) return;
                            try {
                                bool visible = instance->playerGrid.Visibility() == Visibility::Visible;
                                double width = visible ? instance->playerGrid.ActualWidth() : 0.0;
                                double gap = visible
                                    ? width + Cfg()->playerMarginLeft + Cfg()->playerMarginRight
                                    : 0.0;
                                auto margin = instance->trackedOriginalMargin;
                                bool centeredAfterApps =
                                    Cfg()->position ==
                                        L"taskbar_after_apps" &&
                                    IsWindowsTaskbarCentered();
                                if (!centeredAfterApps) {
                                    if (placeBefore) margin.Left += gap;
                                    else margin.Right += gap;
                                }
                                instance->trackedElement.Margin(margin);

                                auto transform = instance->trackedElement.TransformToVisual(
                                    instance->targetGrid);
                                auto point = transform.TransformPoint({0, 0});
                                double x = 0.0;
                                if (Cfg()->position == L"taskbar_after_apps") {
                                    double appEdge = FindRightmostTaskbarItemEdge(
                                        instance->trackedElement,
                                        instance->targetGrid);
                                    x = appEdge >= 0.0
                                        ? appEdge + Cfg()->playerMarginLeft
                                        : point.X + instance->trackedElement.ActualWidth() +
                                            Cfg()->playerMarginLeft;
                                    double trayLeft = FindSystemTrayLeftEdge(
                                        instance->targetGrid,
                                        instance->targetGrid);
                                    if (centeredAfterApps &&
                                        trayLeft >= 0.0 &&
                                        x + width +
                                                Cfg()->playerMarginRight >
                                            trayLeft - 4.0) {
                                        x = Cfg()->playerMarginLeft;
                                    }
                                } else {
                                    x = placeBefore
                                        ? point.X - gap + Cfg()->playerMarginLeft
                                        : point.X + instance->trackedElement.ActualWidth() +
                                            Cfg()->playerMarginLeft;
                                }
                                instance->playerGrid.Margin({x, 0, 0, 0});
                            } catch (...) {}
                        });
                } else {
                    instance->playerGrid.Margin({
                        (double)Cfg()->playerMarginLeft, 0,
                        (double)Cfg()->playerMarginRight, 0});
                }
            }
            Grid::SetColumn(instance->playerGrid, 0);
            Canvas::SetZIndex(instance->playerGrid, 1000);
            targetGrid.Children().Append(instance->playerGrid);
        }

        instance->playerGrid.Visibility(Visibility::Visible);
        instance->playerGrid.UpdateLayout();
        return true;
    } catch (...) {
        Wh_Log(L"InjectMirrorPlayer: failed for taskbar %p", taskbarWindow);
        if (instance && RemoveMirrorPlayerOnCurrentThread(instance)) {
            RemoveMirrorPlayerReference(instance);
        }
        return false;
    }
}

static void InjectConfiguredMirrorPlayers() {
    if (Cfg()->taskbarMode != L"all") return;
    for (const auto& taskbar : EnumerateTaskbarWindows()) {
        if (taskbar.hWnd == g_taskbarWnd) continue;
        DWORD threadId = GetWindowThreadProcessId(taskbar.hWnd, nullptr);
        if (threadId == GetCurrentThreadId()) {
            InjectMirrorPlayer(taskbar.hWnd);
        } else {
            Wh_Log(
                L"Mirror taskbar %p uses a separate UI dispatcher; "
                L"skipping it to avoid nested cross-thread XAML calls",
                taskbar.hWnd);
        }
    }
}

static bool RemoveMirrorPlayers(bool shutdownCleanup = false) {
    auto mirrors = SnapshotMirrorPlayers();
    std::vector<std::shared_ptr<MirrorPlayerInstance>> removed;
    removed.reserve(mirrors.size());

    for (auto& instance : mirrors) {
        if (!instance) continue;
        struct MirrorRemovalWork {
            std::shared_ptr<MirrorPlayerInstance> instance;
            bool cleaned = false;
        } work{instance};
        HWND dispatchWindow = instance->taskbarWindow;
        DWORD currentOwnerThreadId = 0;
        bool originalThreadLive = IsOriginalTaskbarThreadAlive(
            instance->ownerThreadHandle, instance->ownerThreadId);
        if (!originalThreadLive) {
            dispatchWindow = nullptr;
        } else if (!IsCurrentProcessTaskbarWindow(
                dispatchWindow, &currentOwnerThreadId, nullptr) ||
            currentOwnerThreadId != instance->ownerThreadId) {
            dispatchWindow = FindCurrentProcessTaskbarWndForThread(
                instance->ownerThreadId);
        }

        auto remove = [](void* parameter) {
            auto* workItem = static_cast<MirrorRemovalWork*>(parameter);
            auto const& mirror = workItem->instance;
            if (!mirror) {
                workItem->cleaned = true;
                return;
            }
            workItem->cleaned =
                RemoveMirrorPlayerOnCurrentThread(mirror);
        };

        bool dispatched = false;
        if (instance->ownerThreadId == GetCurrentThreadId()) {
            WindowDispatchShutdownScope shutdownScope(shutdownCleanup);
            dispatched = InvokeWindowThreadProcSafely(remove, &work);
        } else if (dispatchWindow) {
            dispatched = shutdownCleanup
                ? RunFromWindowThreadForCleanup(
                      dispatchWindow, remove, &work)
                : RunFromWindowThread(dispatchWindow, remove, &work);
        }

        if (!dispatched || !work.cleaned) {
            Wh_Log(L"RemoveMirrorPlayers: preserving unresolved mirror %p",
                   instance->taskbarWindow);
        } else {
            removed.push_back(instance);
        }
    }

    // Remove only instances from this snapshot that were actually cleaned.
    // A mirror injected concurrently after the snapshot must remain tracked.
    RemoveMirrorPlayerReferences(removed);
    return MirrorPlayersEmpty();
}

static bool InjectPlayerGrid() {
    Wh_Log(L"InjectPlayerGrid: Starting");
    HANDLE pendingOwnerThreadHandle = nullptr;
    if (g_playerGrid || g_injectionParent || g_layoutUpdateToken.value ||
        !MirrorPlayersEmpty() ||
        !g_primaryVisualState->xamlSubscriptionRevokers.empty() ||
        g_primaryVisualState->xamlCallbacksActive.load(
            std::memory_order_acquire)) {
        Wh_Log(L"InjectPlayerGrid: unresolved prior XAML state; refusing reinjection");
        return false;
    }

    HWND hWnd = g_taskbarWnd.load();
    if (!hWnd) hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"InjectPlayerGrid: No taskbar window found");
        return false;
    }
    DWORD ownerThreadId = 0;
    if (!IsCurrentProcessTaskbarWindow(hWnd, &ownerThreadId, nullptr) ||
        ownerThreadId != GetCurrentThreadId()) {
        Wh_Log(L"InjectPlayerGrid: taskbar owner thread mismatch");
        return false;
    }
    g_taskbarWnd = hWnd;

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            Wh_Log(L"InjectPlayerGrid: Failed to get XAML root");
            return false;
        }

        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) {
            Wh_Log(L"InjectPlayerGrid: Failed to get root FrameworkElement");
            return false;
        }

        Wh_Log(L"InjectPlayerGrid: Got XAML root and framework element");

        if (Cfg()->enableTreeDump) {
            DumpXamlTree(root, 0, 5);
            auto rootGrid = FindTaskbarRootGrid(root);
            if (rootGrid) {
                auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
                if (repeater) {
                    DumpXamlTree(repeater, 0, 3);
                }
            }
        }

        auto [targetGrid, insertCol] = ResolveInjectionTarget(root, Cfg()->position);

        if (!targetGrid) {
            if (Cfg()->enableTreeDump) {
                DumpXamlTree(root, 0, 8);
            }
            return false;
        }

        pendingOwnerThreadHandle =
            OpenTaskbarOwnerThreadHandle(ownerThreadId);
        if (!pendingOwnerThreadHandle) return false;
        g_playerOwnerWindow = hWnd;
        g_playerOwnerThreadId = ownerThreadId;
        g_playerOwnerThreadHandle = pendingOwnerThreadHandle;
        pendingOwnerThreadHandle = nullptr;
        Grid playerGrid = BuildPlayerGrid(g_primaryVisualState);
        if (!playerGrid) {
            if (g_primaryVisualState->xamlSubscriptionRevokers.empty() &&
                !g_primaryVisualState->xamlCallbacksActive.load(
                    std::memory_order_acquire)) {
                g_playerOwnerWindow = nullptr;
                g_playerOwnerThreadId = 0;
                CloseOwnedThreadHandle(g_playerOwnerThreadHandle);
            }
            return false;
        }

        // Publish cleanup ownership before the first taskbar-tree mutation.
        // Any later exception can now be rolled back by RemovePlayerGrid.
        g_playerGrid = playerGrid;
        g_injectionParent = targetGrid;
        g_playerColumn = -1;
        g_playerColumnInserted = false;
        g_playerColumnShiftCommitted = false;
        g_playerColumnShiftedChildren.clear();
        ApplyCustomPlayerOffset(playerGrid);

        bool startButtonModActive = IsStartButtonModActive(root);
        (void)startButtonModActive;

        bool isTrayGrid = (targetGrid.Name() == L"SystemTrayFrameGrid");
        RemovePlayerGridChildren(targetGrid);
        RemoveAnchorDebugOverlays(targetGrid);

        if (isTrayGrid) {
            InsertTrackedPlayerColumn(
                targetGrid, insertCol, g_playerColumn,
                g_playerColumnInserted,
                g_playerColumnShiftCommitted,
                g_playerColumnShiftedChildren);

            playerGrid.Margin({(double)Cfg()->playerMarginLeft, 0,
                              (double)Cfg()->playerMarginRight, 0});
            Grid::SetColumn(playerGrid, insertCol);
            targetGrid.Children().Append(playerGrid);
        }
        else {
            auto repeater  = FindChildByName(targetGrid, L"TaskbarFrameRepeater");
            auto trayFrame = FindChildByName(targetGrid, L"SystemTrayFrameGrid");

            bool isEdgePosition = (Cfg()->position == L"taskbar_left_edge" ||
                                   Cfg()->position == L"taskbar_center_edge" ||
                                   Cfg()->position == L"taskbar_right_edge");

            bool isTrackingPosition = (Cfg()->position == L"taskbar_left_start" ||
                                       Cfg()->position == L"taskbar_right_start" ||
                                       Cfg()->position == L"taskbar_after_apps" ||
                                       Cfg()->position == L"taskbar_far_left_reserved" ||
                                       Cfg()->position == L"taskbar_after_search_left" ||
                                       Cfg()->position == L"taskbar_after_search_right" ||
                                       Cfg()->position == L"taskbar_after_taskview_left" ||
                                       Cfg()->position == L"taskbar_after_taskview_right" ||
                                       Cfg()->position == L"taskbar_after_widgets_left" ||
                                       Cfg()->position == L"taskbar_after_widgets_right");

            if (isEdgePosition || isTrackingPosition) {
                double leftMargin  = (double)Cfg()->playerMarginLeft;
                double rightMargin = (double)Cfg()->playerMarginRight;

                playerGrid.HorizontalAlignment(HorizontalAlignment::Left);

                if (isEdgePosition) {
                    if (Cfg()->position == L"taskbar_left_edge") {
                        playerGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                    else if (Cfg()->position == L"taskbar_center_edge") {
                        playerGrid.HorizontalAlignment(HorizontalAlignment::Center);
                        playerGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                    else if (Cfg()->position == L"taskbar_right_edge") {
                        playerGrid.HorizontalAlignment(HorizontalAlignment::Right);
                        if (trayFrame) rightMargin += trayFrame.ActualWidth() + 4;
                        playerGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                } else if (isTrackingPosition) {
                    FrameworkElement targetElem = nullptr;
                    std::wstring trackSide = L"right";

                    if (repeater) {
                        if (Cfg()->position == L"taskbar_far_left_reserved") {
                            targetElem = repeater;
                            trackSide = L"far_left";
                        } else if (Cfg()->position == L"taskbar_after_apps") {
                            targetElem = repeater;
                            trackSide = L"after_apps";
                        } else if (Cfg()->position == L"taskbar_left_start") {
                            targetElem = FindElementInRepeater(repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
                            trackSide = L"left";
                        } else if (Cfg()->position == L"taskbar_right_start") {
                            targetElem = FindElementInRepeater(repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
                            trackSide = L"right";
                        } else if (Cfg()->position == L"taskbar_after_search_left") {
                            targetElem = FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
                            trackSide = L"left";
                        } else if (Cfg()->position == L"taskbar_after_search_right") {
                            targetElem = FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
                            trackSide = L"right";
                        } else if (Cfg()->position == L"taskbar_after_taskview_left") {
                            targetElem = FindNthElementByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
                            trackSide = L"left";
                        } else if (Cfg()->position == L"taskbar_after_taskview_right") {
                            targetElem = FindNthElementByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
                            trackSide = L"right";
                        } else if (Cfg()->position == L"taskbar_after_widgets_left") {
                            targetElem = FindChildByName(repeater, L"AugmentedEntryPointButton");
                            if (!targetElem) targetElem = FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton");
                            trackSide = L"left";
                        } else if (Cfg()->position == L"taskbar_after_widgets_right") {
                            targetElem = FindChildByName(repeater, L"AugmentedEntryPointButton");
                            if (!targetElem) targetElem = FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton");
                            trackSide = L"right";
                        }
                    }

                    if (targetElem) {
                        g_trackedElement = targetElem;
                        g_trackedElementOriginalMargin = targetElem.Margin();
                        g_hasTrackedElementOriginalMargin = true;
                        g_trackPosition = trackSide;

                        bool startButtonModActiveMod = IsStartButtonModActive(root);
                        double startButtonOffset = 0.0;

                        if (startButtonModActiveMod &&
                            (Cfg()->position == L"taskbar_left_start" ||
                             Cfg()->position == L"taskbar_right_start" ||
                             Cfg()->position == L"taskbar_after_taskview_left" ||
                             Cfg()->position == L"taskbar_after_taskview_right")) {
                            startButtonOffset = GetStartButtonAdjustment(root);
                        }

                        auto weakTargetGrid = winrt::make_weak(targetGrid);
                        g_layoutUpdateToken = targetGrid.LayoutUpdated(
                            [weakTargetGrid, startButtonModActiveMod, startButtonOffset](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
                                try {
                                    auto targetGrid = weakTargetGrid.get();
                                    if (!targetGrid) return;
                                    if (TaskbarXamlCallbacksSuppressed() ||
                                        !g_primaryVisualState ||
                                        !g_primaryVisualState->xamlCallbacksActive.load(
                                            std::memory_order_acquire) ||
                                        !g_playerGrid || !g_trackedElement) {
                                        return;
                                    }
                                    UpdateAnchorDebugOverlay(targetGrid, g_trackedElement);

                                    bool isVisible = (g_playerGrid.Visibility() == Visibility::Visible);
                                    double w = isVisible ? g_playerGrid.ActualWidth() : 0.0;

                                    double desiredGap = isVisible ? (w + Cfg()->playerMarginLeft + Cfg()->playerMarginRight) : 0.0;
                                    auto m = g_hasTrackedElementOriginalMargin ? g_trackedElementOriginalMargin : g_trackedElement.Margin();
                                    auto currentMargin = g_trackedElement.Margin();
                                    bool changedMargin = false;

                                    if (g_trackPosition == L"far_left") {
                                        try {
                                            double originalLeft = g_hasTrackedElementOriginalMargin ? g_trackedElementOriginalMargin.Left : 0.0;
                                            double currentLeftmost = FindLeftmostVisibleChildX(g_trackedElement, targetGrid, 4);
                                            double naturalLeft = currentLeftmost >= 0.0
                                                ? currentLeftmost - (currentMargin.Left - originalLeft)
                                                : desiredGap;
                                            double requiredExtra = std::max(0.0, desiredGap - naturalLeft);
                                            double targetLeft = originalLeft + requiredExtra;

                                            if (std::abs(currentMargin.Left - targetLeft) > 1.0 ||
                                                std::abs(currentMargin.Right - m.Right) > 1.0) {
                                                m.Left = targetLeft;
                                                changedMargin = true;
                                            }
                                        } catch (...) {
                                            if (g_hasTrackedElementOriginalMargin &&
                                                (std::abs(currentMargin.Left - m.Left) > 1.0 ||
                                                 std::abs(currentMargin.Right - m.Right) > 1.0)) {
                                                changedMargin = true;
                                            }
                                        }
                                    } else if (g_trackPosition == L"left") {
                                        if (std::abs(currentMargin.Left - desiredGap) > 1.0) { m.Left = desiredGap; changedMargin = true; }
                                    } else if (
                                        g_trackPosition == L"after_apps" &&
                                        IsWindowsTaskbarCentered()) {
                                        if (std::abs(currentMargin.Left - m.Left) > 1.0 ||
                                            std::abs(currentMargin.Right - m.Right) > 1.0) {
                                            changedMargin = true;
                                        }
                                    } else {
                                        if (std::abs(currentMargin.Right - desiredGap) > 1.0) { m.Right = desiredGap; changedMargin = true; }
                                    }

                                    if (changedMargin) g_trackedElement.Margin(m);

                                    if (isVisible) {
                                        try {
                                            auto transform = g_trackedElement.TransformToVisual(targetGrid);
                                            auto point = transform.TransformPoint({0, 0});
                                            double leftPos = point.X;

                                            if (g_trackPosition == L"far_left") {
                                                leftPos = Cfg()->playerMarginLeft;
                                            } else if (g_trackPosition == L"left") {
                                                leftPos = point.X - desiredGap + Cfg()->playerMarginLeft;
                                                if (startButtonModActiveMod && startButtonOffset > 0) {
                                                    leftPos += startButtonOffset;
                                                }
                                            } else if (g_trackPosition == L"after_apps") {
                                                double appEdge = FindRightmostTaskbarItemEdge(
                                                    g_trackedElement, targetGrid);
                                                leftPos = appEdge >= 0.0
                                                    ? appEdge + Cfg()->playerMarginLeft
                                                    : point.X + g_trackedElement.ActualWidth() +
                                                        Cfg()->playerMarginLeft;
                                                if (IsWindowsTaskbarCentered()) {
                                                    double trayLeft =
                                                        FindSystemTrayLeftEdge(
                                                            targetGrid,
                                                            targetGrid);
                                                    if (trayLeft >= 0.0 &&
                                                        leftPos + w +
                                                                Cfg()->playerMarginRight >
                                                            trayLeft - 4.0) {
                                                        leftPos =
                                                            Cfg()->playerMarginLeft;
                                                    }
                                                }
                                            } else {
                                                leftPos = point.X + g_trackedElement.ActualWidth() + Cfg()->playerMarginLeft;
                                            }

                                            auto pm = g_playerGrid.Margin();
                                            if (std::abs(pm.Left - leftPos) > 1.0) {
                                                g_playerGrid.Margin({leftPos, 0, 0, 0});
                                            }
                                        } catch (...) {}
                                    }
                                } catch (...) {
                                    Wh_Log(L"Player LayoutUpdated: tracked element became invalid, untracking");
                                    g_trackedElement = nullptr;
                                    g_hasTrackedElementOriginalMargin = false;
                                }
                            }
                        );
                    } else {
                        playerGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                }

                Grid::SetColumn(playerGrid, 0);
                Canvas::SetZIndex(playerGrid, 1000);
                targetGrid.Children().Append(playerGrid);
                g_playerColumn = -1;
                Wh_Log(L"InjectPlayerGrid: Added player to targetGrid.Children (edge/tracking position), column=0, zindex=1000");
            }
            else {
                InsertTrackedPlayerColumn(
                    targetGrid, insertCol, g_playerColumn,
                    g_playerColumnInserted,
                    g_playerColumnShiftCommitted,
                    g_playerColumnShiftedChildren);

                playerGrid.Margin({(double)Cfg()->playerMarginLeft, 0,
                                  (double)Cfg()->playerMarginRight, 0});
                Grid::SetColumn(playerGrid, insertCol);
                targetGrid.Children().Append(playerGrid);
            }
        }

        InjectConfiguredMirrorPlayers();
        RefreshPlayerContents();

        g_playerGrid.Visibility(Visibility::Visible);
        Wh_Log(L"InjectPlayerGrid: Set g_playerGrid.Visibility to Visible");
        g_playerGrid.UpdateLayout();

        Wh_Log(L"InjectPlayerGrid: Player size - ActualWidth=%f, ActualHeight=%f, Width=%f, Height=%f",
               g_playerGrid.ActualWidth(), g_playerGrid.ActualHeight(),
               g_playerGrid.Width(), g_playerGrid.Height());
        auto margin = g_playerGrid.Margin();
        Wh_Log(L"InjectPlayerGrid: Player margin - Left=%f, Top=%f, Right=%f, Bottom=%f",
               margin.Left, margin.Top, margin.Right, margin.Bottom);

        if (g_injectionParent) {
            g_injectionParent.UpdateLayout();
            Wh_Log(L"InjectPlayerGrid: Called UpdateLayout on injectionParent");
        }

        if (g_playerGrid.ActualWidth() == 0.0 && g_playerGrid.ActualHeight() == 0.0) {
            Wh_Log(L"InjectPlayerGrid: XAML not laid out yet, scheduling deferred update");
            g_needsUiUpdate = true;
            SignalWorkerEventHandle(g_timerUpdateEvent);
        }

        try {
            if (!g_unloading && g_playerGrid) RefreshThemeColors();
        } catch (...) {
            Wh_Log(L"InjectPlayerGrid: Failed to refresh theme colors");
        }

        Canvas::SetZIndex(g_playerGrid, 1000);

        RequestMediaSessionRefresh();
        g_needsUiUpdate = true;
        return true;
    } catch (...) {
        Wh_Log(L"InjectPlayerGrid: Exception during injection");
        if (pendingOwnerThreadHandle) {
            CloseHandle(pendingOwnerThreadHandle);
            pendingOwnerThreadHandle = nullptr;
        }
        try {
            if (!RemovePlayerGrid()) {
                Wh_Log(L"InjectPlayerGrid: rollback wasn't confirmed");
            }
        } catch (...) {
            Wh_Log(L"InjectPlayerGrid: rollback threw an exception");
        }
        return false;
    }
}

static bool RemovePlayerGrid(bool shutdownCleanup) {
    bool mirrorsClean = RemoveMirrorPlayers(shutdownCleanup);

    DWORD ownerThreadId = g_playerOwnerThreadId.load();
    if (ownerThreadId && ownerThreadId != GetCurrentThreadId()) {
        Wh_Log(L"RemovePlayerGrid: refused wrong-thread XAML cleanup");
        return false;
    }

    bool hasPrimaryStructure = g_playerGrid || g_injectionParent ||
                               g_layoutUpdateToken.value;
    bool hasPrimaryCallbacks = g_primaryVisualState &&
        (!g_primaryVisualState->xamlSubscriptionRevokers.empty() ||
         g_primaryVisualState->xamlCallbacksActive.load(
             std::memory_order_acquire));
    if (!hasPrimaryStructure && !hasPrimaryCallbacks) {
        g_playerOwnerWindow = nullptr;
        g_playerOwnerThreadId = 0;
        CloseOwnedThreadHandle(g_playerOwnerThreadHandle);
        return mirrorsClean;
    }

    if (!DeactivatePlayerXamlCallbacks(
            g_playerGrid, g_primaryVisualState)) {
        Wh_Log(L"RemovePlayerGrid: player callback revocation failed");
        return false;
    }
    if (!hasPrimaryStructure) {
        g_playerOwnerWindow = nullptr;
        g_playerOwnerThreadId = 0;
        CloseOwnedThreadHandle(g_playerOwnerThreadHandle);
        return mirrorsClean;
    }

    Grid targetGrid{nullptr};
    try {
        targetGrid = g_injectionParent.try_as<Grid>();
    } catch (...) {}
    if (!targetGrid) {
        Wh_Log(L"RemovePlayerGrid: injection parent is unavailable");
        return false;
    }

    if (g_layoutUpdateToken.value) {
        try {
            targetGrid.LayoutUpdated(g_layoutUpdateToken);
            g_layoutUpdateToken = {};
        } catch (...) {
            Wh_Log(L"RemovePlayerGrid: LayoutUpdated callback revocation failed");
            return false;
        }
    }

    try {
        if (g_trackedElement) {
            if (g_hasTrackedElementOriginalMargin) {
                g_trackedElement.Margin(g_trackedElementOriginalMargin);
            } else {
                auto margin = g_trackedElement.Margin();
                if (g_trackPosition == L"left" ||
                    g_trackPosition == L"far_left") {
                    margin.Left = 0;
                }
                if (g_trackPosition == L"right") margin.Right = 0;
                g_trackedElement.Margin(margin);
            }
        }
    } catch (...) {
        // The tracked taskbar element can disappear during a taskbar rebuild;
        // the callback has already been revoked, so releasing it is safe.
    }

    try {
        RemoveAnchorDebugOverlays(targetGrid);
        RemovePlayerGridChildren(targetGrid);

        if (!RemoveTrackedPlayerColumn(
                targetGrid, g_playerColumn,
                g_playerColumnInserted,
                g_playerColumnShiftCommitted,
                g_playerColumnShiftedChildren)) {
            Wh_Log(L"RemovePlayerGrid: column rollback failed");
            return false;
        }

        for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
            auto child = targetGrid.Children()
                             .GetAt(i)
                             .try_as<FrameworkElement>();
            if (child && child.Name() == kGridName) {
                Wh_Log(L"RemovePlayerGrid: player detachment wasn't confirmed");
                return false;
            }
        }
    } catch (...) {
        Wh_Log(L"RemovePlayerGrid: player detachment failed");
        return false;
    }

    g_playerGrid = nullptr;
    g_injectionParent = nullptr;
    g_playerColumn = -1;
    g_playerOwnerWindow = nullptr;
    g_playerOwnerThreadId = 0;
    CloseOwnedThreadHandle(g_playerOwnerThreadHandle);
    g_trackedElement = nullptr;
    g_hasTrackedElementOriginalMargin = false;
    g_trackPosition.clear();

    g_primaryVisualState->cachedAlbumTitle.clear();
    g_primaryVisualState->cachedAlbumArtist.clear();
    g_primaryVisualState->cachedThumbnailBytes.clear();
    g_primaryVisualState->cachedPaletteHash = 0;
    g_primaryVisualState->blurBgCache.Invalidate();
    g_cachedAppIconSize = -1;
    g_primaryVisualState->scrollCachedTitle.clear();
    g_primaryVisualState->scrollCachedArtist.clear();
    ResetScrollState(g_primaryVisualState->titleScroll);
    ResetScrollState(g_primaryVisualState->artistScroll);
    return mirrorsClean;
}

static void ReleasePointerCapturesRecursive(
    DependencyObject const& object) {
    if (!object) return;
    try {
        if (auto element = object.try_as<UIElement>()) {
            element.ReleasePointerCaptures();
        }
    } catch (...) {}

    try {
        int childCount = VisualTreeHelper::GetChildrenCount(object);
        for (int index = 0; index < childCount; ++index) {
            ReleasePointerCapturesRecursive(
                VisualTreeHelper::GetChild(object, index));
        }
    } catch (...) {}
}

static bool DeactivatePlayerXamlCallbacks(
    Grid const& playerGrid,
    std::shared_ptr<PlayerVisualState> const& visualState) {
    if (visualState) {
        visualState->xamlCallbacksActive.store(
            false, std::memory_order_release);
    }
    ReleasePointerCapturesRecursive(playerGrid);
    return RevokePlayerXamlSubscriptions(visualState);
}

static void RefreshPlayerContentsForGrid(
    Grid const& playerGrid,
    std::shared_ptr<PlayerVisualState> const& visualState,
    bool forceVisualRefresh) {
    if (!playerGrid || !visualState || TaskbarXamlCallbacksSuppressed()) {
        return;
    }

    Wh_Log(L"RefreshPlayerContents: Starting");
    std::wstring      title, artist;
    bool              isPlaying = false, playbackStateKnown = false;
    bool              hasMedia = false;
    std::vector<BYTE> thumbBytes;
    std::vector<BYTE> appIconBytes;
    uint64_t          thumbHash = 0;
    bool              canSkipPrevious = true, canSkipNext = true;
    bool              canShuffle = true, canRepeat = true, canSeek = true;
    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        title        = g_media.title;
        artist       = g_media.artist;
        isPlaying    = g_media.isPlaying;
        playbackStateKnown = g_media.playbackStateKnown;
        hasMedia     = g_media.hasMedia;
        thumbBytes   = g_media.thumbnailBytes;
        thumbHash    = g_media.thumbnailHash;
        appIconBytes = g_media.appIconBytes;
        canSkipPrevious = g_media.canSkipPrevious;
        canSkipNext     = g_media.canSkipNext;
        canShuffle      = g_media.canShuffle;
        canRepeat       = g_media.canRepeat;
        canSeek         = g_media.canSeek;
    }
    Wh_Log(L"RefreshPlayerContents: title='%s', artist='%s', isPlaying=%d, hasMedia=%d",
           title.c_str(), artist.c_str(), isPlaying, hasMedia);
    bool hasSession = false;
    { std::lock_guard<std::mutex> lk(g_sessionMtx); hasSession = (g_currentSession != nullptr); }

    playerGrid.UpdateLayout();
    UpdateProgressBarForGrid(playerGrid, visualState);

    if (g_scrollResetRequested.exchange(false)) {
        ResetScrollState(visualState->titleScroll);
        ResetScrollState(visualState->artistScroll);
        visualState->scrollCachedTitle.clear();
        visualState->scrollCachedArtist.clear();
    }

    if (title != visualState->scrollCachedTitle ||
        artist != visualState->scrollCachedArtist) {
        visualState->scrollCachedTitle = title;
        visualState->scrollCachedArtist = artist;
        ResetScrollState(visualState->titleScroll);
        ResetScrollState(visualState->artistScroll);
        try {
            if (auto fe = FindChildByName(playerGrid, kTitleCloneName))
                if (auto cl = fe.try_as<TextBlock>())
                    cl.Visibility(Visibility::Collapsed);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(playerGrid, kArtistCloneName))
                if (auto cl = fe.try_as<TextBlock>())
                    cl.Visibility(Visibility::Collapsed);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(playerGrid, kTitleBlockName))
                if (auto tb = fe.try_as<TextBlock>())
                    Canvas::SetLeft(tb, 0.0);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(playerGrid, kArtistBlockName))
                if (auto ab = fe.try_as<TextBlock>())
                    Canvas::SetLeft(ab, 0.0);
        } catch (...) {}
    }

    bool titleVisible = false;
    bool artistVisible = false;

    if (auto fe = FindChildByName(playerGrid, kTitleBlockName))
        if (auto tb = fe.try_as<TextBlock>())
            try {
                std::wstring displayTitle = title;
                if (!hasSession) {
                    displayTitle = Cfg()->noMediaTitleText;
                } else if (!hasMedia) {
                    displayTitle.clear();
                } else if (title.empty()) {
                    displayTitle = Cfg()->emptyTitleText;
                }
                tb.Text(winrt::hstring(displayTitle));
                tb.Foreground(MakeBrush(TextColor()));
                bool visible = Cfg()->showTrackTitle && !displayTitle.empty();
                titleVisible = visible;
                tb.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);

                if (Cfg()->showFullTitleOnHover && !title.empty() && hasSession) {
                    ToolTipService::SetToolTip(tb, winrt::box_value(winrt::hstring(title)));
                    ToolTipService::SetPlacement(tb, Controls::Primitives::PlacementMode::Top);
                } else {
                    ToolTipService::SetToolTip(tb, nullptr);
                }

                if (Cfg()->enableTitleScrolling && visible) {
                    try {
                        if (auto viewFe = FindChildByName(playerGrid, kTitleScrollViewName))
                            viewFe.Visibility(Visibility::Visible);
                    } catch (...) {}
                    if (auto panelFe = FindChildByName(playerGrid, kPanelGridName)) {
                        panelFe.UpdateLayout();
                    }
                    tb.UpdateLayout();
                    double textW = tb.DesiredSize().Width;
                    if (auto viewFe = FindChildByName(playerGrid, kTitleScrollViewName)) {
                        if (auto viewCanvas = viewFe.try_as<Canvas>()) {
                            double minW = (double)Cfg()->textAreaMinWidth;
                            double maxW = (double)Cfg()->textAreaMaxWidth;
                            double viewW = textW;

                            if (maxW > 0 && viewW > maxW) viewW = maxW;

                            double availW = GetAvailableScrollTextAreaWidth(playerGrid);
                            if (availW > 0.0 && viewW > availW) {
                                viewW = (minW > 0.0) ? std::max(availW, minW) : availW;
                            }

                            if (minW > 0 && viewW < minW) viewW = minW;
                            if (std::abs(viewCanvas.Width() - viewW) > 0.5) {
                                viewCanvas.Width(viewW);
                                try {
                                    if (auto geo = viewCanvas.Clip().try_as<winrt::Windows::UI::Xaml::Media::RectangleGeometry>()) {
                                        auto r = geo.Rect();
                                        geo.Rect({0, 0, (float)viewW, r.Height});
                                    }
                                } catch (...) {}
                            }
                            auto& scroll = visualState->titleScroll;
                            bool wasActive = scroll.active;
                            scroll.textWidth = textW;
                            scroll.viewWidth = viewW;
                            scroll.active = (textW > viewW + 2.0);
                            if (!scroll.active) {
                                scroll.offset = 0.0;
                                scroll.forward = true;
                                Canvas::SetLeft(tb, 0.0);
                            } else if (!wasActive) {
                                scroll.offset = 0.0;
                                scroll.forward = true;
                                scroll.pausing = true;
                                scroll.pauseTick =
                                    Cfg()->scrollPauseDuration;
                            }
                            if (auto cloneFe = FindChildByName(playerGrid, kTitleCloneName)) {
                                if (auto clone = cloneFe.try_as<TextBlock>()) {
                                    clone.Text(tb.Text());
                                    clone.Foreground(tb.Foreground());
                                    clone.Visibility(Visibility::Collapsed);
                                }
                            }
                        }
                    }
                } else {
                    visualState->titleScroll.active = false;
                    visualState->titleScroll.offset = 0.0;
                    if (Cfg()->enableTitleScrolling) {
                        try {
                            if (auto viewFe = FindChildByName(playerGrid, kTitleScrollViewName))
                                viewFe.Visibility(Visibility::Collapsed);
                        } catch (...) {}
                    }
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kArtistBlockName))
        if (auto ab = fe.try_as<TextBlock>())
            try {
                std::wstring displayArtist = artist;
                if (!hasSession) {
                    displayArtist = Cfg()->noMediaArtistText;
                } else if (!hasMedia) {
                    displayArtist.clear();
                } else if (artist.empty()) {
                    displayArtist = Cfg()->emptyArtistText;
                }
                ab.Text(winrt::hstring(displayArtist));
                bool visible = Cfg()->showTrackArtist && !displayArtist.empty();
                artistVisible = visible;
                ab.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
                ab.Foreground(MakeBrush(ArtistColor()));

                if (Cfg()->showFullTitleOnHover && !artist.empty() && hasSession) {
                    ToolTipService::SetToolTip(ab, winrt::box_value(winrt::hstring(artist)));
                    ToolTipService::SetPlacement(ab, Controls::Primitives::PlacementMode::Top);
                } else {
                    ToolTipService::SetToolTip(ab, nullptr);
                }

                if (Cfg()->enableArtistScrolling && visible) {
                    try {
                        if (auto viewFe = FindChildByName(playerGrid, kArtistScrollViewName))
                            viewFe.Visibility(Visibility::Visible);
                    } catch (...) {}
                    if (auto panelFe = FindChildByName(playerGrid, kPanelGridName)) {
                        panelFe.UpdateLayout();
                    }
                    ab.UpdateLayout();
                    double textW = ab.DesiredSize().Width;
                    if (auto viewFe = FindChildByName(playerGrid, kArtistScrollViewName)) {
                        if (auto viewCanvas = viewFe.try_as<Canvas>()) {
                            double minW = (double)Cfg()->textAreaMinWidth;
                            double maxW = (double)Cfg()->textAreaMaxWidth;
                            double viewW = textW;

                            if (maxW > 0 && viewW > maxW) viewW = maxW;

                            double availW = GetAvailableScrollTextAreaWidth(playerGrid);
                            if (availW > 0.0 && viewW > availW) {
                                viewW = (minW > 0.0) ? std::max(availW, minW) : availW;
                            }

                            if (minW > 0 && viewW < minW) viewW = minW;
                            if (std::abs(viewCanvas.Width() - viewW) > 0.5) {
                                viewCanvas.Width(viewW);
                                try {
                                    if (auto geo = viewCanvas.Clip().try_as<winrt::Windows::UI::Xaml::Media::RectangleGeometry>()) {
                                        auto r = geo.Rect();
                                        geo.Rect({0, 0, (float)viewW, r.Height});
                                    }
                                } catch (...) {}
                            }
                            auto& scroll = visualState->artistScroll;
                            bool wasActive = scroll.active;
                            scroll.textWidth = textW;
                            scroll.viewWidth = viewW;
                            scroll.active = (textW > viewW + 2.0);
                            if (!scroll.active) {
                                scroll.offset = 0.0;
                                scroll.forward = true;
                                Canvas::SetLeft(ab, 0.0);
                            } else if (!wasActive) {
                                scroll.offset = 0.0;
                                scroll.forward = true;
                                scroll.pausing = true;
                                scroll.pauseTick =
                                    Cfg()->scrollPauseDuration;
                            }
                            if (auto cloneFe = FindChildByName(playerGrid, kArtistCloneName)) {
                                if (auto clone = cloneFe.try_as<TextBlock>()) {
                                    clone.Text(ab.Text());
                                    clone.Foreground(ab.Foreground());
                                    clone.Visibility(Visibility::Collapsed);
                                }
                            }
                        }
                    }
                } else {
                    visualState->artistScroll.active = false;
                    visualState->artistScroll.offset = 0.0;
                    if (Cfg()->enableArtistScrolling) {
                        try {
                            if (auto viewFe = FindChildByName(playerGrid, kArtistScrollViewName))
                                viewFe.Visibility(Visibility::Collapsed);
                        } catch (...) {}
                    }
                }
            } catch (...) {}

    try {
        if (auto stackFe = FindChildByName(playerGrid, kTextStackName)) {
            bool anyTextVisible = titleVisible || artistVisible;
            stackFe.Visibility(anyTextVisible ? Visibility::Visible : Visibility::Collapsed);
        }
    } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kPlayBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                btn.IsEnabled(playbackStateKnown);
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(2, isPlaying);
                    ct.Text(winrt::hstring(glyph));
                    ct.Foreground(MakeBrush(ButtonColor()));
                    ct.Opacity(playbackStateKnown ? 1.0 : 0.35);
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kPrevBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSkipPrevious;
                btn.IsEnabled(supported);
                if (Cfg()->hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(1);
                    ct.Text(winrt::hstring(glyph));
                    if (!supported && !Cfg()->hideUnsupportedButtons) {
                        ct.Opacity(0.35);
                        ct.Foreground(MakeBrush(ButtonColor()));
                    } else {
                        ct.Opacity(1.0);
                        ct.Foreground(MakeBrush(ButtonColor()));
                    }
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kNextBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSkipNext;
                btn.IsEnabled(supported);
                if (Cfg()->hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(3);
                    ct.Text(winrt::hstring(glyph));
                    ct.Opacity(supported ? 1.0 : 0.35);
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kVolumeBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    int percent = g_currentVolumePercent.load();
                    bool muted = g_currentVolumeMuted.load();
                    ct.Text(VolumeGlyphForState(
                        percent < 0 ? 100 : percent, muted));
                    ct.Foreground(MakeBrush(ButtonColor()));
                    ct.Opacity(1.0);
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kRewindBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSeek;
                btn.IsEnabled(supported);
                if (Cfg()->hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(5);
                    ct.Text(winrt::hstring(glyph));
                    ct.Opacity(supported ? 1.0 : 0.35);
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kForwardBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSeek;
                btn.IsEnabled(supported);
                if (Cfg()->hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(6);
                    ct.Text(winrt::hstring(glyph));
                    ct.Opacity(supported ? 1.0 : 0.35);
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kShuffleBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canShuffle;
                btn.IsEnabled(supported);
                if (Cfg()->hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    bool isEnabled = g_shuffleEnabled.load();
                    const wchar_t* glyph = L"\uE8B1";
                    ct.Text(winrt::hstring(glyph));
                    if (!supported && !Cfg()->hideUnsupportedButtons) {
                        ct.Opacity(0.35);
                    } else {
                        ct.Opacity(isEnabled ? 1.0 : 0.4);
                    }
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}

    if (auto fe = FindChildByName(playerGrid, kRepeatBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canRepeat;
                btn.IsEnabled(supported);
                if (Cfg()->hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    RepeatMode mode = g_repeatMode.load();
                    const wchar_t* glyph;
                    switch (mode) {
                        case RepeatMode::Off:
                            glyph = L"\uF5E7";
                            break;
                        case RepeatMode::All:
                            glyph = L"\uE8EE";
                            break;
                        case RepeatMode::One:
                            glyph = L"\uE8ED";
                            break;
                    }
                    ct.Text(winrt::hstring(glyph));
                    ct.Foreground(MakeBrush(ButtonColor()));
                    if (!supported && !Cfg()->hideUnsupportedButtons) {
                        ct.Opacity(0.35);
                    } else {
                        ct.Opacity(1.0);
                    }
                }
            } catch (...) {}

    if (Cfg()->showPauseOverlay && Cfg()->showAlbumArt) {
        if (auto fe = FindChildByName(playerGrid, L"PauseIconOverlay"))
            if (auto overlay = fe.try_as<Border>()) {
                try {
                    bool showPause =
                        hasMedia && playbackStateKnown && !isPlaying;
                    overlay.Visibility(showPause ? Visibility::Visible : Visibility::Collapsed);

                    if (auto pauseIcon = overlay.Child().try_as<TextBlock>()) {
                        pauseIcon.Text(GetGlyph(2, true));
                        bool useFluent = (Cfg()->iconStyle == L"fluent_outline" || Cfg()->iconStyle == L"fluent_filled");
                        pauseIcon.FontFamily(Media::FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                        pauseIcon.FontSize((double)Cfg()->pauseOverlayIconSize);
                    }

                    if (showPause) {
                        if (auto artImg = FindChildByName(playerGrid, kArtImageName)) {
                            if (auto parent = VisualTreeHelper::GetParent(artImg)) {
                                if (auto artInnerGrid = parent.try_as<Grid>()) {
                                    for (uint32_t i = 0; i < artInnerGrid.Children().Size(); ++i) {
                                        auto child = artInnerGrid.Children().GetAt(i);
                                        if (auto border = child.try_as<Border>()) {
                                            if (border.Name() == L"EmptyIconBorder") {
                                                border.Visibility(Visibility::Collapsed);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } catch (...) {}
            }
    }

    bool paletteChanged = false;

    if (auto fe = FindChildByName(playerGrid, kArtImageName))
        if (auto img = fe.try_as<Controls::Image>()) {
            if (!thumbBytes.empty() && Cfg()->showAlbumArt) {
                bool isSameAlbum = (!forceVisualRefresh &&
                                   img.Source() &&
                                   !visualState->cachedThumbnailBytes.empty() &&
                                   thumbBytes ==
                                       visualState->cachedThumbnailBytes);
                if (isSameAlbum) {
                    // The title can change before the new cover is published.
                    // Keep the decoded old bitmap alive instead of reloading the
                    // same bytes and briefly exposing the gray placeholder.
                    visualState->cachedAlbumTitle = title;
                    visualState->cachedAlbumArtist = artist;
                }

                size_t newHash = (size_t)thumbHash;
                if (newHash != g_primaryVisualState->cachedPaletteHash &&
                    newHash != 0) {
                    g_primaryVisualState->cachedAlbumPalette = ExtractAlbumPalette(thumbBytes);
                    g_primaryVisualState->cachedPaletteHash = newHash;
                    paletteChanged = true;
                }

                int sourcePixelWidth =
                    visualState->cachedAlbumPixelWidth;
                int sourcePixelHeight =
                    visualState->cachedAlbumPixelHeight;
                if (!isSameAlbum ||
                    sourcePixelWidth <= 0 || sourcePixelHeight <= 0) {
                    if (!GetEncodedImageDimensions(
                            thumbBytes, sourcePixelWidth,
                            sourcePixelHeight)) {
                        sourcePixelWidth = 0;
                        sourcePixelHeight = 0;
                    }
                }
                ApplyAlbumArtAspectLayout(
                    playerGrid, img,
                    sourcePixelWidth, sourcePixelHeight);

                if (!isSameAlbum) {
                    try {
                        // Restore the BitmapImage stream loader used by the
                        // last working release. WriteableBitmap/WIC decoding
                        // can produce an empty XAML surface inside Explorer
                        // even when WIC successfully decoded the bytes.
                        if (visualState->albumArtDecodeAction) {
                            try {
                                visualState->albumArtDecodeAction.Cancel();
                            } catch (...) {}
                            visualState->albumArtDecodeAction = nullptr;
                        }

                        IStream* rawStream = SHCreateMemStream(
                            thumbBytes.data(),
                            static_cast<UINT>(thumbBytes.size()));
                        if (!rawStream) {
                            throw winrt::hresult_error(E_OUTOFMEMORY);
                        }

                        winrt::com_ptr<IStream> memoryStream;
                        memoryStream.attach(rawStream);

                        winrt::Windows::Storage::Streams::IRandomAccessStream
                            randomAccessStream{nullptr};
                        winrt::check_hresult(
                            ::CreateRandomAccessStreamOverStream(
                                memoryStream.get(),
                                BSOS_DEFAULT,
                                winrt::guid_of<
                                    winrt::Windows::Storage::Streams::
                                        IRandomAccessStream>(),
                                winrt::put_abi(randomAccessStream)));

                        BitmapImage bitmap;
                        if (Cfg()->albumArtQuality == L"low") {
                            int baseHeight =
                                Cfg()->albumArtMaxHeight > 0
                                    ? Cfg()->albumArtMaxHeight
                                    : 64;
                            bitmap.DecodePixelHeight(
                                std::max(16, baseHeight / 2));
                        } else if (
                            Cfg()->albumArtQuality == L"medium" &&
                            Cfg()->albumArtMaxHeight > 0) {
                            bitmap.DecodePixelHeight(
                                Cfg()->albumArtMaxHeight);
                        } else if (
                            Cfg()->albumArtQuality == L"high") {
                            int baseHeight =
                                Cfg()->albumArtMaxHeight > 0
                                    ? Cfg()->albumArtMaxHeight
                                    : 64;
                            bitmap.DecodePixelHeight(
                                std::clamp(baseHeight * 2, 32, 512));
                        }

                        // Assign the source first so XAML repaints as soon as
                        // SetSourceAsync finishes. Keep the action in the
                        // per-widget state and cancel it during teardown.
                        img.Source(bitmap);
                        visualState->albumArtDecodeAction =
                            bitmap.SetSourceAsync(randomAccessStream);
                        img.Visibility(Visibility::Visible);
                        if (auto placeholderFe = FindChildByName(
                                playerGrid,
                                L"TaskbarMediaPresence_ArtPlaceholder")) {
                            placeholderFe.Visibility(Visibility::Collapsed);
                        }

                        visualState->cachedAlbumTitle = title;
                        visualState->cachedAlbumArtist = artist;
                        visualState->cachedThumbnailBytes = thumbBytes;
                        visualState->cachedAlbumPixelWidth =
                            sourcePixelWidth;
                        visualState->cachedAlbumPixelHeight =
                            sourcePixelHeight;

                        if (auto parent = VisualTreeHelper::GetParent(img)) {
                            if (auto artInnerGrid = parent.try_as<Grid>()) {
                                if (auto grandParent =
                                        VisualTreeHelper::GetParent(artInnerGrid)) {
                                    if (auto container =
                                            grandParent.try_as<FrameworkElement>()) {
                                        if (auto greatGrandParent =
                                                VisualTreeHelper::GetParent(container)) {
                                            if (auto artContainer =
                                                    greatGrandParent.try_as<Grid>()) {
                                                artContainer.Visibility(
                                                    Visibility::Visible);
                                            }
                                        }
                                    }
                                }
                                for (uint32_t i = 0;
                                     i < artInnerGrid.Children().Size(); ++i) {
                                    auto child = artInnerGrid.Children().GetAt(i);
                                    if (auto border = child.try_as<Border>()) {
                                        if (border.Name() == L"EmptyIconBorder") {
                                            border.Visibility(
                                                Visibility::Collapsed);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    } catch (winrt::hresult_error const& error) {
                        Wh_Log(
                            L"Album art: BitmapImage load setup failed "
                            L"(0x%08X, %ls, bytes=%zu)",
                            static_cast<unsigned int>(error.code().value),
                            error.message().c_str(), thumbBytes.size());
                        try {
                            img.Source(nullptr);
                            img.Visibility(Visibility::Collapsed);
                            if (auto placeholderFe = FindChildByName(
                                    playerGrid,
                                    L"TaskbarMediaPresence_ArtPlaceholder")) {
                                placeholderFe.Visibility(Visibility::Visible);
                            }
                        } catch (...) {}
                        visualState->cachedAlbumTitle.clear();
                        visualState->cachedAlbumArtist.clear();
                        visualState->cachedThumbnailBytes.clear();
                        visualState->cachedAlbumPixelWidth = 0;
                        visualState->cachedAlbumPixelHeight = 0;
                    } catch (...) {
                        Wh_Log(
                            L"Album art: BitmapImage load setup failed "
                            L"(unknown error, bytes=%zu)",
                            thumbBytes.size());
                        try {
                            img.Source(nullptr);
                            img.Visibility(Visibility::Collapsed);
                            if (auto placeholderFe = FindChildByName(
                                    playerGrid,
                                    L"TaskbarMediaPresence_ArtPlaceholder")) {
                                placeholderFe.Visibility(Visibility::Visible);
                            }
                        } catch (...) {}
                        visualState->cachedAlbumTitle.clear();
                        visualState->cachedAlbumArtist.clear();
                        visualState->cachedThumbnailBytes.clear();
                        visualState->cachedAlbumPixelWidth = 0;
                        visualState->cachedAlbumPixelHeight = 0;
                    }
                } else {
                    img.Visibility(Visibility::Visible);
                    if (auto parent = VisualTreeHelper::GetParent(img)) {
                        if (auto container = parent.try_as<FrameworkElement>()) {
                            if (auto grandParent = VisualTreeHelper::GetParent(container)) {
                                if (auto greatGrandParent = VisualTreeHelper::GetParent(grandParent)) {
                                    if (auto artContainer = greatGrandParent.try_as<Grid>()) {
                                        artContainer.Visibility(Visibility::Visible);
                                    }
                                }
                            }
                        }
                    }
                }

                if (auto bgFe = FindChildByName(playerGrid, L"TaskbarMediaPresence_Background")) {
                    if (auto bgBorder = bgFe.try_as<Border>()) {
                        auto bgType = Cfg()->backgroundType;

                        if (bgType == L"album_art_blur") {
                            try {
                                visualState->blurBgCache.Invalidate();
                                bgBorder.Visibility(Visibility::Visible);
                                bgBorder.Opacity(Cfg()->blurOpacity / 100.0);
                                auto weakBackground =
                                    winrt::make_weak(bgBorder);
                                std::weak_ptr<PlayerVisualState>
                                    weakVisualState = visualState;
                                auto applyBlur = [
                                    weakBackground,
                                    weakVisualState,
                                    thumbBytesSnap = thumbBytes]() {
                                    try {
                                        auto bgBorder =
                                            weakBackground.get();
                                        auto visualState =
                                            weakVisualState.lock();
                                        if (!bgBorder || !visualState) {
                                            return;
                                        }
                                        int w = (int)bgBorder.ActualWidth();
                                        int h = (int)bgBorder.ActualHeight();
                                        if (w <= 0 || h <= 0) return;
                                        visualState->blurBgCache.Invalidate();
                                        bgBorder.Background(
                                            MakeAlbumBlurBrush(
                                                visualState->blurBgCache,
                                                thumbBytesSnap, w, h));
                                        bgBorder.Opacity(Cfg()->blurOpacity / 100.0);
                                        bgBorder.Visibility(Visibility::Visible);
                                    } catch (...) {}
                                };
                                // A later media/timer refresh retries once the
                                // taskbar has completed layout. Avoid a delayed
                                // one-shot SizeChanged delegate that could
                                // survive detachment or DLL unload.
                                if (bgBorder.ActualWidth() > 0 &&
                                    bgBorder.ActualHeight() > 0) {
                                    applyBlur();
                                }
                            } catch (...) {}
                        } else if (bgType == L"solid" || bgType == L"gradient" || bgType == L"acrylic" || bgType == L"mica" || bgType == L"mica_alt") {
                            try {
                                bgBorder.Background(MakeBackgroundBrush());
                                bgBorder.Visibility(Visibility::Visible);
                                bgBorder.Opacity(1.0);
                            } catch (...) {}
                        }
                    }
                }
            } else {
                visualState->cachedAlbumTitle.clear();
                visualState->cachedAlbumArtist.clear();
                visualState->cachedThumbnailBytes.clear();
                visualState->cachedAlbumPixelWidth = 0;
                visualState->cachedAlbumPixelHeight = 0;
                ApplyAlbumArtAspectLayout(playerGrid, img, 0, 0);
                g_primaryVisualState->cachedPaletteHash = 0;
                visualState->blurBgCache.Invalidate();

                if (auto bgFe = FindChildByName(playerGrid, L"TaskbarMediaPresence_Background")) {
                    if (auto bgBorder = bgFe.try_as<Border>()) {
                        try {
                            auto bgType = Cfg()->backgroundType;
                            if (bgType == L"solid" || bgType == L"gradient" || bgType == L"acrylic" ||
                                bgType == L"mica" || bgType == L"mica_alt") {
                                bgBorder.Background(MakeBackgroundBrush());
                                bgBorder.Visibility(Visibility::Visible);
                                bgBorder.Opacity(1.0);
                            } else {
                                bgBorder.Background(nullptr);
                                bgBorder.Visibility(Visibility::Collapsed);
                            }
                        } catch (...) {}
                    }
                }

                try {
                    img.Source(nullptr);
                    img.Visibility(Visibility::Collapsed);
                    if (auto placeholderFe = FindChildByName(
                            playerGrid,
                            L"TaskbarMediaPresence_ArtPlaceholder")) {
                        placeholderFe.Visibility(Visibility::Visible);
                    }

                    if (Cfg()->albumArtEmptyBehavior == L"hide" && thumbBytes.empty()) {
                        if (auto parent = VisualTreeHelper::GetParent(img)) {
                            if (auto container = parent.try_as<FrameworkElement>()) {
                                if (auto grandParent = VisualTreeHelper::GetParent(container)) {
                                    if (auto greatGrandParent = VisualTreeHelper::GetParent(grandParent)) {
                                        if (auto artContainer = greatGrandParent.try_as<FrameworkElement>()) {
                                            artContainer.Visibility(Visibility::Collapsed);
                                        }
                                    }
                                }
                            }
                        }
                    } else if (Cfg()->albumArtEmptyBehavior == L"show_icon" && thumbBytes.empty()) {
                        if (auto parent = VisualTreeHelper::GetParent(img)) {
                            if (auto artInnerGrid = parent.try_as<Grid>()) {
                                if (auto grandParent = VisualTreeHelper::GetParent(artInnerGrid)) {
                                    if (auto container = grandParent.try_as<FrameworkElement>()) {
                                        if (auto greatGrandParent = VisualTreeHelper::GetParent(container)) {
                                            if (auto artContainer = greatGrandParent.try_as<Grid>()) {
                                                artContainer.Visibility(Visibility::Visible);
                                            }
                                        }
                                    }
                                }

                                Border iconBorder = nullptr;
                                for (uint32_t i = 0; i < artInnerGrid.Children().Size(); ++i) {
                                    auto child = artInnerGrid.Children().GetAt(i);
                                    if (auto border = child.try_as<Border>()) {
                                        if (border.Name() == L"EmptyIconBorder") {
                                            iconBorder = border;
                                            break;
                                        }
                                    }
                                }

                                if (!iconBorder) {
                                    iconBorder = Border();
                                    iconBorder.Name(L"EmptyIconBorder");
                                    iconBorder.Background(MakeBrush({0x00, 0x00, 0x00, 0x00}));
                                    iconBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
                                    iconBorder.VerticalAlignment(VerticalAlignment::Stretch);
                                    Canvas::SetZIndex(iconBorder, 5);

                                    TextBlock iconText = TextBlock();
                                    iconText.Name(L"EmptyIconText");
                                    iconText.HorizontalAlignment(HorizontalAlignment::Center);
                                    iconText.VerticalAlignment(VerticalAlignment::Center);

                                    iconBorder.Child(iconText);
                                    artInnerGrid.Children().InsertAt(0, iconBorder);
                                }

                                if (auto textBlock = iconBorder.Child().try_as<TextBlock>()) {
                                    std::wstring glyphStr;
                                    try {
                                        unsigned long cp = std::stoul(Cfg()->emptyIconGlyph, nullptr, 16);
                                        if (cp <= 0xFFFF) {
                                            glyphStr = std::wstring(1, (wchar_t)cp);
                                        } else {
                                            cp -= 0x10000;
                                            glyphStr += (wchar_t)(0xD800 + (cp >> 10));
                                            glyphStr += (wchar_t)(0xDC00 + (cp & 0x3FF));
                                        }
                                    } catch (...) {
                                        glyphStr = L"\uE189";
                                    }
                                    textBlock.Text(glyphStr);

                                    bool useFluent = (Cfg()->emptyIconFont == L"segoe_fluent");
                                    textBlock.FontFamily(Media::FontFamily(
                                        useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                                    textBlock.FontSize((double)Cfg()->emptyIconSize);
                                    BYTE alpha = (BYTE)std::clamp((int)std::round(Cfg()->emptyIconOpacity * 255.0 / 100.0), 0, 255);
                                    auto iconClr = ParseColorWithThemeSupport(Cfg()->emptyIconColor, alpha);
                                    textBlock.Foreground(MakeBrush(iconClr));
                                }

                                iconBorder.Visibility(Visibility::Visible);
                            }
                        }
                    }
                } catch (...) {}
            }
        }

    if (paletteChanged) {
        try {
            if (Cfg()->backgroundType == L"gradient" ||
                Cfg()->backgroundType == L"solid" ||
                Cfg()->backgroundType == L"acrylic" ||
                Cfg()->backgroundType == L"mica" ||
                Cfg()->backgroundType == L"mica_alt") {
                if (auto bgFe = FindChildByName(playerGrid, L"TaskbarMediaPresence_Background")) {
                    if (auto bgBorder = bgFe.try_as<Border>()) {
                        bgBorder.Background(MakeBackgroundBrush());
                    }
                }
            }

            auto textClr = TextColor();
            auto artistClr = ArtistColor();

            if (auto titleFe = FindChildByName(playerGrid, kTitleBlockName)) {
                if (auto titleBlock = titleFe.try_as<TextBlock>()) {
                    titleBlock.Foreground(SolidColorBrush(textClr));
                }
            }

            if (auto artistFe = FindChildByName(playerGrid, kArtistBlockName)) {
                if (auto artistBlock = artistFe.try_as<TextBlock>()) {
                    artistBlock.Foreground(SolidColorBrush(artistClr));
                }
            }

            auto buttonClr = ButtonColor();
            for (const auto& btnName : {kPlayBtnName, kPrevBtnName,
                                        kNextBtnName, kVolumeBtnName,
                                        kRewindBtnName, kForwardBtnName, kShuffleBtnName, kRepeatBtnName}) {
                if (auto btnFe = FindChildByName(playerGrid, btnName)) {
                    if (auto btn = btnFe.try_as<Button>()) {
                        if (auto content = btn.Content()) {
                            if (auto icon = content.try_as<TextBlock>()) {
                                icon.Foreground(SolidColorBrush(buttonClr));
                            }
                        }
                    }
                }
            }
        } catch (...) {}
    }

    if (Cfg()->showAppIcon) {
        if (auto fe = FindChildByName(playerGrid, kAppIconImageName))
            if (auto img = fe.try_as<Controls::Image>()) {
                bool sizeChanged =
                    g_cachedAppIconSize != Cfg()->appIconSize;
                if (sizeChanged) {
                    // FetchMediaPropertiesAsync performs the potentially slow
                    // window/COM icon search on a worker and publishes the
                    // result back through g_media.
                    FetchMediaPropertiesAsync();
                }

                if (!appIconBytes.empty()) {
                    try {
                        int iconSz = Cfg()->appIconSize;
                        size_t expectedBytes = (size_t)iconSz * iconSz * 4;
                        if (appIconBytes.size() != expectedBytes) {
                            int computed = (int)std::sqrt((double)appIconBytes.size() / 4.0);
                            if (computed > 0 && (size_t)computed * computed * 4 == appIconBytes.size())
                                iconSz = computed;
                        }

                        img.Width(iconSz);
                        img.Height(iconSz);

                        size_t bytesNeeded = (size_t)iconSz * iconSz * 4;
                        winrt::Windows::UI::Xaml::Media::Imaging::WriteableBitmap wb(iconSz, iconSz);
                        auto buf = wb.PixelBuffer();

                        auto bufferByteAccess = buf.as<Windows::Storage::Streams::IBufferByteAccess>();
                        BYTE* pixels = nullptr;
                        bufferByteAccess->Buffer(&pixels);

                        if (appIconBytes.size() >= bytesNeeded && pixels) {
                            for (size_t i = 0; i + 3 < bytesNeeded; i += 4) {
                                pixels[i+0] = appIconBytes[i+2];
                                pixels[i+1] = appIconBytes[i+1];
                                pixels[i+2] = appIconBytes[i+0];
                                pixels[i+3] = appIconBytes[i+3];
                            }
                        }
                        buf.Length(static_cast<uint32_t>(bytesNeeded));
                        wb.Invalidate();
                        img.Source(wb);
                        img.Visibility(Visibility::Visible);
                    } catch (...) {
                        try { img.Source(nullptr); img.Visibility(Visibility::Collapsed); } catch (...) {}
                    }
                } else {
                    try { img.Source(nullptr); img.Visibility(Visibility::Collapsed); } catch (...) {}
                }
            }
    }

    RefreshScrollDispatcherTimerCadence();
}

static void RefreshPlayerContents() {
    if (TaskbarXamlCallbacksSuppressed()) return;
    Grid primaryGrid = g_playerGrid;
    if (primaryGrid) {
        RefreshPlayerContentsForGrid(
            primaryGrid, g_primaryVisualState, false);
    }

    auto mirrors = SnapshotMirrorPlayers();
    for (const auto& instance : mirrors) {
        if (!instance || !instance->playerGrid || !instance->visualState) {
            continue;
        }
        struct MirrorRefreshWork {
            std::shared_ptr<MirrorPlayerInstance> instance;
        } work{instance};
        RunFromWindowThread(instance->taskbarWindow, [](void* parameter) {
            if (TaskbarXamlCallbacksSuppressed()) return;
            auto* workItem = static_cast<MirrorRefreshWork*>(parameter);
            auto const& mirror = workItem->instance;
            if (!mirror || !mirror->playerGrid || !mirror->visualState) return;
            RefreshPlayerContentsForGrid(
                mirror->playerGrid, mirror->visualState, false);
        }, &work);
    }
}

static bool IsFullscreenActive() {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_NOT_PRESENT;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
        (state == QUNS_RUNNING_D3D_FULL_SCREEN ||
         state == QUNS_PRESENTATION_MODE)) {
        return true;
    }

    // QUNS_BUSY is intentionally ignored. It is also reported for several
    // non-fullscreen shell/user states; opening Start can temporarily clear it,
    // making the widget appear only while the Windows key is held/open.
    HWND foreground = GetForegroundWindow();
    if (!foreground || IsIconic(foreground)) return false;

    wchar_t className[128]{};
    GetClassNameW(foreground, className, ARRAYSIZE(className));
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(className, L"Progman") == 0 ||
        _wcsicmp(className, L"WorkerW") == 0) {
        return false;
    }

    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(
            foreground, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    RECT windowRect{};
    if (FAILED(DwmGetWindowAttribute(
            foreground, DWMWA_EXTENDED_FRAME_BOUNDS,
            &windowRect, sizeof(windowRect))) &&
        !GetWindowRect(foreground, &windowRect)) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(
        foreground, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) return false;

    constexpr LONG tolerance = 2;
    const RECT& monitorRect = monitorInfo.rcMonitor;
    return windowRect.left <= monitorRect.left + tolerance &&
           windowRect.top <= monitorRect.top + tolerance &&
           windowRect.right >= monitorRect.right - tolerance &&
           windowRect.bottom >= monitorRect.bottom - tolerance;
}

static bool ShouldHidePlayer() {
    bool hide = false;
    if (Cfg()->hideFullscreen && IsFullscreenActive()) hide = true;
    if (!hide && g_hiddenByIdle.load(std::memory_order_relaxed)) hide = true;

    if (!hide) {
        bool hasMedia = false, hasSession = false;
        {
            std::lock_guard<std::mutex> lock(g_mediaMtx);
            hasMedia = g_media.hasMedia;
        }
        {
            std::lock_guard<std::mutex> lock(g_sessionMtx);
            hasSession = g_currentSession != nullptr;
        }
        if (hasMedia) {
            g_lastMediaTime = std::chrono::steady_clock::now();
        }

        if (Cfg()->hideWhenNoMedia) {
            if (!hasSession) {
                if (hasMedia && MediaVisualTransitionActive()) {
                    // A session can be replaced between tracks. Keep the last
                    // stable visual during the grace instead of collapsing the
                    // taskbar column and making the widget flash.
                    g_needsUiUpdate = true;
                } else {
                    hide = true;
                }
            } else if (!hasMedia) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - g_lastMediaTime)
                        .count();
                if (elapsed > 2500) {
                    hide = true;
                } else {
                    g_needsUiUpdate = true;
                }
            }
        }
    }
    return hide;
}

static void UpdateVisibilityForGrid(
    Grid const& playerGrid,
    FrameworkElement const& injectionParent,
    int playerColumn,
    bool hide) {
    if (!playerGrid) return;
    try {
        playerGrid.Visibility(
            hide ? Visibility::Collapsed : Visibility::Visible);
        playerGrid.Opacity(hide ? 0.0 : 1.0);

        if (playerColumn < 0) {
            playerGrid.UpdateLayout();
            return;
        }

        if (auto targetGrid = injectionParent.try_as<Grid>()) {
            if (playerColumn <
                static_cast<int>(targetGrid.ColumnDefinitions().Size())) {
                auto column =
                    targetGrid.ColumnDefinitions().GetAt(playerColumn);
                column.Width(hide
                                 ? GridLength{0.0, GridUnitType::Pixel}
                                 : GridLength{1.0, GridUnitType::Auto});
            }
        }

        if (hide) {
            playerGrid.MinWidth(0);
            playerGrid.MaxWidth(0);
            playerGrid.Width(0);
        } else {
            bool hasTextOrButtons =
                Cfg()->showTrackTitle ||
                Cfg()->showTrackArtist ||
                (Cfg()->showMediaButtons &&
                 HasConfiguredMediaButtons());
            if ((hasTextOrButtons || Cfg()->showProgressBar) &&
                Cfg()->playerMinWidth > 0) {
                playerGrid.MinWidth(
                    static_cast<double>(Cfg()->playerMinWidth));
            } else {
                playerGrid.MinWidth(0);
            }
            if (Cfg()->playerMaxWidth > 0) {
                playerGrid.MaxWidth(
                    static_cast<double>(Cfg()->playerMaxWidth));
            } else {
                playerGrid.ClearValue(
                    FrameworkElement::MaxWidthProperty());
            }
            playerGrid.ClearValue(FrameworkElement::WidthProperty());
        }

        playerGrid.UpdateLayout();
    } catch (...) {}
}

static void UpdateVisibility() {
    if (TaskbarXamlCallbacksSuppressed()) return;
    bool hide = ShouldHidePlayer();

    Grid primaryGrid = g_playerGrid;
    FrameworkElement primaryParent = g_injectionParent;
    if (primaryGrid) {
        UpdateVisibilityForGrid(
            primaryGrid, primaryParent, g_playerColumn, hide);
    }

    auto mirrors = SnapshotMirrorPlayers();
    for (const auto& instance : mirrors) {
        if (!instance || !instance->playerGrid) continue;
        struct MirrorVisibilityWork {
            std::shared_ptr<MirrorPlayerInstance> instance;
            bool hide;
        } work{instance, hide};
        RunFromWindowThread(instance->taskbarWindow, [](void* parameter) {
            if (TaskbarXamlCallbacksSuppressed()) return;
            auto* workItem = static_cast<MirrorVisibilityWork*>(parameter);
            auto const& mirror = workItem->instance;
            if (!mirror || !mirror->playerGrid) return;
            UpdateVisibilityForGrid(
                mirror->playerGrid, mirror->targetGrid,
                mirror->playerColumn, workItem->hide);
        }, &work);
    }
}

static bool ApplySettings() {
    Wh_Log(L"ApplySettings: Called");
    uint64_t restartGeneration =
        g_taskbarRestartGeneration.load(std::memory_order_acquire);
    if (TaskbarRestartSettleWindowActive()) return false;
    g_idleSeconds.store(0, std::memory_order_relaxed);
    g_hiddenByIdle.store(false, std::memory_order_relaxed);
    if (restartGeneration !=
            g_taskbarRestartGeneration.load(std::memory_order_acquire) ||
        TaskbarRestartSettleWindowActive()) {
        return false;
    }
    bool transparencyApplied = ApplyTaskbarTransparencyToAll();
    if (restartGeneration !=
            g_taskbarRestartGeneration.load(std::memory_order_acquire) ||
        TaskbarRestartSettleWindowActive()) {
        return false;
    }
    bool removed = false;
    try {
        removed = RemovePlayerGrid();
    } catch (...) {
        Wh_Log(L"ApplySettings: Exception in RemovePlayerGrid");
    }
    if (!removed) {
        Wh_Log(L"ApplySettings: prior XAML cleanup wasn't confirmed");
        return false;
    }

    if (restartGeneration !=
            g_taskbarRestartGeneration.load(std::memory_order_acquire) ||
        TaskbarRestartSettleWindowActive()) {
        return false;
    }

    bool injected = g_unloading;
    if (!g_unloading) {
        Wh_Log(L"ApplySettings: Calling InjectPlayerGrid");
        try {
            injected = InjectPlayerGrid();
            if (injected &&
                (Cfg()->enableTitleScrolling ||
                 Cfg()->enableArtistScrolling ||
                 Cfg()->showProgressBar)) {
                injected = StartScrollTimer();
            }
        } catch (...) {
            Wh_Log(L"ApplySettings: Exception in InjectPlayerGrid");
            injected = false;
        }
    }
    Wh_Log(L"ApplySettings: Finished, g_playerGrid exists = %d", g_playerGrid ? 1 : 0);
    bool completed = transparencyApplied && injected &&
        restartGeneration ==
            g_taskbarRestartGeneration.load(std::memory_order_acquire) &&
        !TaskbarRestartSettleWindowActive();
    if (completed) {
        g_taskbarXamlCallbacksSuppressed.store(
            false, std::memory_order_release);
    }
    return completed;
}


using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

static void TrayUI_StartTaskbar_HookImpl(void* pThis,
                                         bool& originalCalled) {
    g_taskbarStartInProgress.fetch_add(1, std::memory_order_acq_rel);
    g_taskbarOriginalsInProgress.fetch_add(1, std::memory_order_acq_rel);
    struct StartHookCountGuard {
        bool originalActive = true;
        ~StartHookCountGuard() {
            if (originalActive) {
                g_taskbarOriginalsInProgress.fetch_sub(
                    1, std::memory_order_acq_rel);
            }
            g_taskbarStartInProgress.fetch_sub(
                1, std::memory_order_acq_rel);
        }
        unsigned CompleteOriginal() {
            originalActive = false;
            return g_taskbarOriginalsInProgress.fetch_sub(
                       1, std::memory_order_acq_rel) - 1;
        }
    } startHookCountGuard;

    Wh_Log(L"TrayUI_StartTaskbar_Hook: Called");
    if (g_unloading) {
        originalCalled = true;
        TrayUI_StartTaskbar_Original(pThis);
        startHookCountGuard.CompleteOriginal();
        return;
    }
    g_taskbarXamlCallbacksSuppressed.store(true, std::memory_order_release);
    g_taskbarRestartGeneration.fetch_add(1, std::memory_order_acq_rel);
    // A non-expiring barrier while the original StartTaskbar call is on-stack.
    // It is replaced with the short settle deadline after the call returns.
    g_taskbarRestartNotBeforeTick.store(
        std::numeric_limits<ULONGLONG>::max(), std::memory_order_release);
    bool transparencyRestored = false;
    try {
        if (g_activeContextMenuOwnerThreadId.load(
                std::memory_order_acquire) == GetCurrentThreadId()) {
            if (!CloseActiveContextMenu()) {
                Wh_Log(
                    L"TrayUI_StartTaskbar_Hook: context-menu cleanup deferred");
            }
        }
        // Restore snapshots owned by this UI thread only. Never cross-dispatch
        // from a StartTaskbar call stack.
        transparencyRestored =
            RestoreTaskbarTransparencyForCurrentThread();
        if (!transparencyRestored) {
            Wh_Log(
                L"TrayUI_StartTaskbar_Hook: transparency restore deferred");
        }
    } catch (...) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: optional preparation failed");
    }
    originalCalled = true;
    TrayUI_StartTaskbar_Original(pThis);

    unsigned startsRemaining = startHookCountGuard.CompleteOriginal();

    if (g_unloading) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: Unloading, skipping");
        return;
    }

    try {
        if (transparencyRestored) {
            DiscardTaskbarTransparencyStatesForThread(GetCurrentThreadId());
        }
        if (startsRemaining) return;
        g_taskbarRestartNotBeforeTick.store(
            GetTickCount64() + 350, std::memory_order_release);
        if (g_taskbarOriginalsInProgress.load(std::memory_order_acquire)) {
            g_taskbarRestartNotBeforeTick.store(
                std::numeric_limits<ULONGLONG>::max(),
                std::memory_order_release);
        }
        SignalWorkerEventHandle(g_timerUpdateEvent);
    } catch (...) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: optional completion failed");
        g_taskbarRestartNotBeforeTick.store(
            std::numeric_limits<ULONGLONG>::max(),
            std::memory_order_release);
    }
}

static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) noexcept {
    bool originalCalled = false;
    try {
        TrayUI_StartTaskbar_HookImpl(pThis, originalCalled);
        return;
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: unhandled WinRT failure 0x%08X",
               static_cast<uint32_t>(error.code()));
    } catch (...) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: unhandled callback exception");
    }

    // Fail closed after an unexpected native-hook exception. Keep the module
    // mapped and suppress all custom XAML work. If the exception happened
    // before Windows' implementation ran, call it exactly once under counters
    // that make concurrent unload wait for this fallback invocation.
    g_taskbarXamlCallbacksSuppressed.store(true, std::memory_order_release);
    g_taskbarRestartNotBeforeTick.store(
        std::numeric_limits<ULONGLONG>::max(), std::memory_order_release);
    if (originalCalled) return;

    g_taskbarStartInProgress.fetch_add(1, std::memory_order_acq_rel);
    g_taskbarOriginalsInProgress.fetch_add(1, std::memory_order_acq_rel);
    struct FallbackStartCountGuard {
        ~FallbackStartCountGuard() {
            g_taskbarOriginalsInProgress.fetch_sub(
                1, std::memory_order_acq_rel);
            g_taskbarStartInProgress.fetch_sub(
                1, std::memory_order_acq_rel);
        }
    } fallbackStartCountGuard;

    originalCalled = true;
    try {
        TrayUI_StartTaskbar_Original(pThis);
    } catch (...) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: original failed in fallback path");
    }
}

static bool HookTaskbarDllSymbols() {
    HMODULE h = LoadLibraryExW(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h) return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original,
         TrayUI_StartTaskbar_Hook},
    };
    return WindhawkUtils::HookSymbols(
        h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static bool HasOutstandingExecutableCallbacks() {
    if (g_taskbarStartInProgress.load(std::memory_order_acquire) ||
        g_taskbarOriginalsInProgress.load(std::memory_order_acquire) ||
        g_mediaEventUnsubscribeFailed.load(std::memory_order_acquire) ||
        g_scrollDispatcherTimerRegistered.load(std::memory_order_acquire) ||
        g_scrollDispatcherTimerHasToken ||
        g_volumePopupWindow.load(std::memory_order_acquire) ||
        g_volumePopupClassRegistered ||
        !g_activeContextMenuClickSubscriptions.empty() ||
        g_layoutUpdateToken.value) {
        return true;
    }

    if (g_primaryVisualState &&
        (g_primaryVisualState->xamlCallbacksActive.load(
             std::memory_order_acquire) ||
         !g_primaryVisualState->xamlSubscriptionRevokers.empty())) {
        return true;
    }
    for (auto const& mirror : SnapshotMirrorPlayers()) {
        if (!mirror) continue;
        if (mirror->layoutUpdatedToken.value ||
            (mirror->visualState &&
             (mirror->visualState->xamlCallbacksActive.load(
                  std::memory_order_acquire) ||
              !mirror->visualState->xamlSubscriptionRevokers.empty()))) {
            return true;
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_windowDispatchRequestsMtx);
        if (!g_windowDispatchRequests.empty()) return true;
    }
    {
        std::lock_guard<std::mutex> lock(g_failedWindowDispatchHooksMtx);
        if (!g_failedWindowDispatchHooks.empty()) return true;
    }
    AcquireSRWLockExclusive(&g_windowDispatchActivityLock);
    bool dispatchActive = g_activeWindowDispatchCalls != 0 ||
                          g_activeWindowDispatchHookCallbacks != 0;
    ReleaseSRWLockExclusive(&g_windowDispatchActivityLock);
    if (dispatchActive) return true;

    if ((g_discordPresenceThread &&
         WaitForSingleObject(g_discordPresenceThread, 0) == WAIT_TIMEOUT) ||
        (g_mediaThread &&
         WaitForSingleObject(g_mediaThread, 0) == WAIT_TIMEOUT) ||
        (g_timerThread &&
         WaitForSingleObject(g_timerThread, 0) == WAIT_TIMEOUT)) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
        if (g_asyncTaskThread &&
            WaitForSingleObject(g_asyncTaskThread, 0) == WAIT_TIMEOUT) {
            return true;
        }
    }
    return false;
}

static BOOL ModInitImpl() {
    ShellExplorerProcessIdentity shellIdentity =
        GetCurrentProcessShellExplorerIdentity();
    g_isShellExplorerProcess =
        shellIdentity != ShellExplorerProcessIdentity::NonShell;
    if (!g_isShellExplorerProcess) {
        Wh_Log(L"Wh_ModInit: skipping non-shell Explorer process %u",
               GetCurrentProcessId());
        return TRUE;
    }
    if (shellIdentity == ShellExplorerProcessIdentity::StartupCandidate) {
        Wh_Log(
            L"Wh_ModInit: shell windows are not registered yet; accepting "
            L"system Explorer process %u as a deferred startup candidate",
            GetCurrentProcessId());
    }

    if (!g_primaryVisualState) {
        g_primaryVisualState = std::make_shared<PlayerVisualState>();
    }

    if (g_moduleSafetyPinned.load(std::memory_order_acquire) ||
        g_mediaEventUnsubscribeFailed.load(std::memory_order_acquire) ||
        g_scrollDispatcherTimerRegistered.load() ||
        g_scrollDispatcherTimerOwnerWindow.load() ||
        g_scrollDispatcherTimerOwnerThreadId.load() ||
        g_scrollDispatcherTimerOwnerThreadHandle.load() ||
        g_playerOwnerWindow.load() ||
        g_playerOwnerThreadId.load() ||
        g_playerOwnerThreadHandle.load() ||
        g_playerGrid || g_injectionParent || g_layoutUpdateToken.value ||
        g_playerColumn != -1 || g_playerColumnInserted ||
        g_playerColumnShiftCommitted ||
        !g_playerColumnShiftedChildren.empty() ||
        g_trackedElement || g_hasTrackedElementOriginalMargin ||
        (g_primaryVisualState &&
         (g_primaryVisualState->xamlCallbacksActive.load(
              std::memory_order_acquire) ||
          !g_primaryVisualState->xamlSubscriptionRevokers.empty())) ||
        !MirrorPlayersEmpty() ||
        g_volumePopupWindow.load(std::memory_order_acquire) ||
        g_volumePopupClassRegistered ||
        g_activeContextMenuOwnerWindow.load() ||
        g_activeContextMenuOwnerThreadId.load() ||
        g_activeContextMenuOwnerThreadHandle.load()) {
        Wh_Log(
            L"Wh_ModInit: refusing same-process reuse after incomplete "
            L"callback cleanup; restart Explorer before enabling the mod");
        return FALSE;
    }

    g_unloading = false;
    ResetWindowDispatchShutdown();
    g_applyingSettings = false;
    g_hookInjectionInProgress = false;
    g_taskbarWnd = nullptr;
    g_playerOwnerWindow = nullptr;
    g_playerOwnerThreadId = 0;
    g_playerColumn = -1;
    g_playerColumnInserted = false;
    g_playerColumnShiftCommitted = false;
    g_playerColumnShiftedChildren.clear();
    g_trackedElement = nullptr;
    g_hasTrackedElementOriginalMargin = false;
    g_trackPosition.clear();
    g_layoutUpdateToken = {};
    g_taskbarRestartNotBeforeTick = 0;
    g_taskbarRestartGeneration = 0;
    g_taskbarStartInProgress = 0;
    g_taskbarOriginalsInProgress = 0;
    g_taskbarXamlCallbacksSuppressed = false;
    g_needsUiUpdate = false;
    g_mediaEventUnsubscribeFailed = false;
    ClearMirrorPlayers();
    {
        std::lock_guard<std::mutex> lock(g_pendingMediaCommandsMtx);
        g_pendingMediaCommands.clear();
        g_mediaCommandWorkerRunning = false;
    }
    {
        std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
        g_pendingVolumeAdjustApp.clear();
        g_pendingVolumeAdjustSteps = 0;
        g_volumeAdjustWorkerRunning = false;
    }
    g_activeContextMenuOperation.clear(std::memory_order_release);
    {
        std::lock_guard<std::mutex> lock(g_asyncTasksMtx);
        g_acceptAsyncTasks = true;
        g_asyncTaskQueue.clear();
        g_asyncTaskRunning = false;
    }
    {
        std::lock_guard<std::mutex> lock(g_quickRebuildMtx);
        g_quickRebuildGeneration = 0;
        g_quickRebuildWorkerRunning = false;
        g_quickRebuildNeedsMonitorMove = false;
    }

    // Compact context-menu choices are stored with Wh_Set*Value and are
    // intentionally preserved across Explorer and Windows restarts. Load them
    // before creating the first settings snapshot. Applying full Windhawk
    // settings still clears the overrides in Wh_ModSettingsChanged so an
    // explicitly saved settings-page value remains authoritative.
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Wh_ModInit: HookTaskbarDllSymbols failed");
        return FALSE;
    }
    return TRUE;
}

BOOL Wh_ModInit() noexcept {
    try {
        return ModInitImpl();
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Wh_ModInit: unhandled WinRT failure 0x%08X",
               static_cast<uint32_t>(error.code()));
    } catch (...) {
        Wh_Log(L"Wh_ModInit: unhandled initialization exception");
    }
    g_unloading.store(true, std::memory_order_release);
    return FALSE;
}

static void ModAfterInitImpl() {
    if (!g_isShellExplorerProcess) return;

    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    StartMediaThread();

    // Cold sign-in can expose the HWND before the taskbar XAML tree is stable.
    // Use the same delayed, coalesced path as TrayUI::StartTaskbar.
    g_taskbarXamlCallbacksSuppressed.store(true, std::memory_order_release);
    g_taskbarRestartGeneration.fetch_add(1, std::memory_order_acq_rel);
    // This max-preserving update cannot shorten an active StartTaskbar
    // barrier. If the hook has already completed, it merely extends the
    // finite settle deadline; if it is still running, that hook will replace
    // the UINT64_MAX barrier when the original call returns.
    ScheduleTaskbarRebuildRetry(350);
    StartTimerThread();
    SignalWorkerEventHandle(g_timerUpdateEvent);
}

void Wh_ModAfterInit() noexcept {
    try {
        ModAfterInitImpl();
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Wh_ModAfterInit: unhandled WinRT failure 0x%08X",
               static_cast<uint32_t>(error.code()));
        g_taskbarXamlCallbacksSuppressed.store(
            true, std::memory_order_release);
        g_taskbarRestartNotBeforeTick.store(
            std::numeric_limits<ULONGLONG>::max(),
            std::memory_order_release);
    } catch (...) {
        Wh_Log(L"Wh_ModAfterInit: unhandled post-initialization exception");
        g_taskbarXamlCallbacksSuppressed.store(
            true, std::memory_order_release);
        g_taskbarRestartNotBeforeTick.store(
            std::numeric_limits<ULONGLONG>::max(),
            std::memory_order_release);
    }
}

static void ModUninitImpl() {
    g_unloading = true;
    if (!g_isShellExplorerProcess) return;

    BeginWindowDispatchShutdown();
    ULONGLONG hookDrainDeadline = GetTickCount64() + 5000;
    while (g_taskbarStartInProgress.load(std::memory_order_acquire) &&
           GetTickCount64() < hookDrainDeadline) {
        MsgWaitForMultipleObjectsEx(
            0, nullptr, 10, QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
        MSG message{};
        while (PeekMessageW(
            &message, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    bool taskbarStartHooksDrained =
        !g_taskbarStartInProgress.load(std::memory_order_acquire);
    if (!taskbarStartHooksDrained) {
        Wh_Log(
            L"Wh_ModUninit: TrayUI::StartTaskbar exceeded the unload "
            L"deadline; retaining the module if the hook remains active");
    }
    PauseAsyncTasks(true);
    CancelAsyncTaskWorkerCall();
    {
        std::lock_guard<std::mutex> lock(g_pendingMediaCommandsMtx);
        g_pendingMediaCommands.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_pendingVolumeAdjustMtx);
        g_pendingVolumeAdjustSteps = 0;
    }
    g_hookInjectionInProgress = false;
    g_taskbarRestartNotBeforeTick = 0;

    bool scrollTimerStopped = StopTimerThread(
        true, taskbarStartHooksDrained);
    bool mediaThreadStopped = StopMediaThread(true);
    bool asyncTasksDrained = WaitForAsyncTasks(5000);
    bool asyncWorkerStopped = StopAsyncTaskWorker(3000);
    bool asyncTasksStopped = asyncTasksDrained && asyncWorkerStopped;
    if (!asyncTasksStopped) {
        Wh_Log(
            L"Wh_ModUninit: serialized background worker exceeded the "
            L"unload deadline; retaining the module if it remains active");
    }
    // Existing quick workers were allowed to finish before the final XAML
    // callback drain. This catches a timer registered just before unloading.
    if (taskbarStartHooksDrained) {
        scrollTimerStopped =
            StopScrollTimer(true) && scrollTimerStopped;
    }

    bool contextMenuClosed = taskbarStartHooksDrained &&
        CloseActiveContextMenu(true);
    bool transparencyRestored =
        ApplyTaskbarTransparencyToAll(true, false);

    bool popupDestroyed = DestroyVolumePopup(true);
    bool popupClassUnregistered =
        popupDestroyed && UnregisterVolumePopupClassConfirmed();

    struct PlayerCleanupResult {
        bool confirmed = false;
    } playerCleanup;

    auto cleanupPlayer = [](void* parameter) {
        auto* result = static_cast<PlayerCleanupResult*>(parameter);
        try {
            result->confirmed = RemovePlayerGrid(true);
        } catch (...) {
            result->confirmed = false;
        }
    };

    bool playerCleanupDispatched = false;
    DWORD playerOwnerThreadId = g_playerOwnerThreadId.load();
    HANDLE playerOwnerThreadHandle = g_playerOwnerThreadHandle.load();
    HWND playerCleanupWindow = g_playerOwnerWindow.load();
    DWORD cleanupWindowThreadId = 0;
    if (!taskbarStartHooksDrained) {
        Wh_Log(L"Wh_ModUninit: skipping XAML cleanup while hook is active");
    } else if (playerOwnerThreadId &&
        IsOriginalTaskbarThreadAlive(
            playerOwnerThreadHandle, playerOwnerThreadId) &&
        (!IsCurrentProcessTaskbarWindow(
             playerCleanupWindow, &cleanupWindowThreadId, nullptr) ||
         cleanupWindowThreadId != playerOwnerThreadId)) {
        playerCleanupWindow = FindCurrentProcessTaskbarWndForThread(
            playerOwnerThreadId);
    }

    if (taskbarStartHooksDrained) {
        if (playerOwnerThreadId &&
            IsOriginalTaskbarThreadAlive(
                playerOwnerThreadHandle, playerOwnerThreadId) &&
            playerCleanupWindow) {
            playerCleanupDispatched = RunFromWindowThreadForCleanup(
                playerCleanupWindow, cleanupPlayer, &playerCleanup);
        } else if (playerOwnerThreadId) {
            Wh_Log(L"Wh_ModUninit: player owner UI thread is unavailable");
        } else {
            WindowDispatchShutdownScope shutdownScope(true);
            playerCleanupDispatched =
                InvokeWindowThreadProcSafely(cleanupPlayer, &playerCleanup);
        }
    }

    bool hooksUnregistered =
        WaitForFailedWindowDispatchHooksRemoved(3000);
    if (!hooksUnregistered) {
        Wh_Log(
            L"Wh_ModUninit: window-dispatch hooks remain after the "
            L"deadline; retaining the module");
    }

    // A callback that entered just before the final unhook can still be
    // on-stack. A bounded drain avoids hanging Explorer; the emergency module
    // retention below prevents live code from being unmapped on timeout.
    bool dispatchActivityIdle =
        WaitForWindowDispatchActivityIdle(3000);
    if (!dispatchActivityIdle) {
        Wh_Log(
            L"Wh_ModUninit: window-dispatch callbacks remain after the "
            L"deadline; retaining the module");
    }

    bool requestsDrained = false;
    {
        std::lock_guard<std::mutex> lock(g_windowDispatchRequestsMtx);
        requestsDrained = g_windowDispatchRequests.empty();
    }

    bool executableCallbacksOutstanding =
        !taskbarStartHooksDrained ||
        !scrollTimerStopped ||
        !mediaThreadStopped ||
        !asyncTasksStopped ||
        !dispatchActivityIdle ||
        !contextMenuClosed ||
        !popupDestroyed ||
        !popupClassUnregistered ||
        !playerCleanupDispatched ||
        !playerCleanup.confirmed ||
        !hooksUnregistered ||
        !requestsDrained ||
        HasOutstandingExecutableCallbacks();
    if (executableCallbacksOutstanding) {
        ReportOutstandingCallbackRisk(
            L"an executable callback, worker, timer, or window procedure remains");
    }

    bool ordinaryCleanupIncomplete = !transparencyRestored;
    if (ordinaryCleanupIncomplete && !executableCallbacksOutstanding) {
        Wh_Log(
            L"Wh_ModUninit: non-callback UI cleanup was incomplete; "
            L"continuing with normal Windhawk module lifetime");
    }

    if (playerCleanup.confirmed) {
        std::vector<ShiftedColumnChild>().swap(
            g_playerColumnShiftedChildren);
        ReleaseMirrorPlayersStorage();
        if (!executableCallbacksOutstanding) {
            g_primaryVisualState = nullptr;
        }
    }
    if (contextMenuClosed) {
        std::vector<ContextMenuClickSubscription>().swap(
            g_activeContextMenuClickSubscriptions);
    }

    if (popupDestroyed && popupClassUnregistered && g_volumePopupBrush) {
        DeleteObject(g_volumePopupBrush);
        g_volumePopupBrush = nullptr;
    }
}

void Wh_ModUninit() noexcept {
    try {
        ModUninitImpl();
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Wh_ModUninit: unhandled WinRT failure 0x%08X",
               static_cast<uint32_t>(error.code()));
        if (HasOutstandingExecutableCallbacks()) {
            ReportOutstandingCallbackRisk(
                L"cleanup exception left an executable callback active");
        }
    } catch (...) {
        Wh_Log(L"Wh_ModUninit: unhandled cleanup exception");
        if (HasOutstandingExecutableCallbacks()) {
            ReportOutstandingCallbackRisk(
                L"cleanup exception left an executable callback active");
        }
    }
}

static void ModSettingsChangedImpl() {
    if (!g_isShellExplorerProcess) return;

    if (!AcquireSettingsApplyGateForFullApply()) return;
    struct SettingsApplyGateRelease {
        bool active = true;
        ~SettingsApplyGateRelease() {
            if (active) {
                g_applyingSettings.store(
                    false, std::memory_order_release);
            }
        }
    } gateRelease;

    PauseAsyncTasks();
    CancelAsyncTaskWorkerCall();
    if (!DestroyVolumePopup()) {
        Wh_Log(
            L"Wh_ModSettingsChanged: volume popup cleanup was deferred");
    }
    bool scrollTimerStopped = StopTimerThread();
    bool mediaThreadStopped = StopMediaThread();
    bool asyncTasksStopped = WaitForAsyncTasks(5000);
    if (!mediaThreadStopped) {
        Wh_Log(
            L"Wh_ModSettingsChanged: media worker cleanup exceeded the "
            L"settings-change deadline; continuing without hanging Explorer");
    }
    if (!asyncTasksStopped) {
        Wh_Log(
            L"Wh_ModSettingsChanged: background tasks exceeded the "
            L"settings-change deadline; continuing without hanging Explorer");
    }

    // Values selected from the compact context menu are persistent overrides
    // across Explorer/Windows restarts. Applying the full Windhawk settings
    // intentionally clears them so the newly saved settings-page values become
    // authoritative.
    Wh_SetIntValue(L"quickPositionPreset", -1);
    Wh_SetIntValue(L"quickTaskbarMode", -1);
    Wh_SetIntValue(L"quickTaskbarNumber", -1);
    Wh_SetIntValue(L"quickBackground", -1);
    Wh_SetIntValue(L"quickTransparentTaskbar", -1);
    Wh_SetIntValue(L"quickMediaSourceOverride", -1);
    Wh_SetStringValue(L"quickPreferredMediaApp", L"");

    LoadSettings();
    bool rebuilt = scrollTimerStopped && ApplyQuickMonitorRebuild();
    if (!rebuilt) {
        Wh_Log(
            L"Wh_ModSettingsChanged: lifecycle-safe taskbar rebuild was deferred");
    }

    ResumeAsyncTasks();
    g_applyingSettings.store(false, std::memory_order_release);
    gateRelease.active = false;
    StartMediaThread();
    StartTimerThread();
    RequestMediaSessionRefresh();
}

void Wh_ModSettingsChanged() noexcept {
    try {
        ModSettingsChangedImpl();
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Wh_ModSettingsChanged: unhandled WinRT failure 0x%08X",
               static_cast<uint32_t>(error.code()));
        g_applyingSettings.store(false, std::memory_order_release);
        g_taskbarXamlCallbacksSuppressed.store(
            true, std::memory_order_release);
        g_taskbarRestartNotBeforeTick.store(
            std::numeric_limits<ULONGLONG>::max(),
            std::memory_order_release);
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged: unhandled settings-change exception");
        g_applyingSettings.store(false, std::memory_order_release);
        g_taskbarXamlCallbacksSuppressed.store(
            true, std::memory_order_release);
        g_taskbarRestartNotBeforeTick.store(
            std::numeric_limits<ULONGLONG>::max(),
            std::memory_order_release);
    }
}
