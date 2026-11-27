// ==WindhawkMod==
// @id              taskbar-fluent-media-player-fork
// @name            Taskbar Fluent Media + Lyrics
// @description     Windows 11 taskbar media controls, album art, visualizer and synchronized lyrics with customizable placement.
// @version         3.1.5.4
// @author          ironsensey
// @github          https://github.com/ironsensey
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -luser32 -lwindowsapp -lshell32 -lgdi32 -lshlwapi -lwindowscodecs -ldwmapi -lshcore -lksuser -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Fluent Media + Lyrics 3.1.5.4

Album art, track details, playback controls and synced lyrics share one player.
Default order: **cover | title / artist | previous, play / pause, next | lyrics**.
Mirror layout reverses the cover and controls. Their sides can also be chosen independently.

The default is **Taskbar - Left edge (Overlay)**. It uses the leftmost available
space between Windows controls; if the full player cannot fit it
hides instead of covering your apps. Hidden Search / Task View / Widgets anchors fall
back to the left of the tray, with a Windhawk log message. Enable **Layout preview**
and **Show debug borders** to find the cover and each component even with no music.
Turn preview off when finished. Offsets and sizes use device-independent pixels (DIPs).

Settings with bounded numbers use Windhawk 2.x's native `$format: slider` plus a number
input on builds supporting that format. Windhawk 1.7.3 displays number inputs; a mod
cannot change the settings editor shipped with that version.
Existing settings survive source replacement: enable cover, title, artist and media
buttons in Main Settings if an older version saved those switches as off.

Lyrics use HTTPS requests to LRCLIB (title and artist are sent). Spotify multi-artist/featured credits get safe fallback matching. English is unchanged;
Hindi and Gurmukhi can use Roman transliteration (legacy default) or their original scripts.
Optional plain-lyrics fallback is explicitly labeled UNSYNCED and does not claim accurate karaoke timing.
Song-change transitions animate art and text, and album-derived visualizer colors smoothly.
This changes the script only, without translation. Missing lyrics
stay blank; playback controls and cover continue working. Playback capabilities,
artwork and timeline accuracy depend on the selected Windows media session.

Use **Lyrics only** for the compact original presentation, or disable **Show lyrics**
to use just the media player. Visualizer and text scrolling are optional power costs
and are off by default. In the no-lyrics layout, the visualizer docks directly
right of the album cover with collision-safe spacing independent of its neighbor
gap setting. Audio capture uses more precise bass bands, hop-based FFT updates,
whole-range grouping even at one visible bar, real audio-driven shapes, and
configurable time-based smoothing. Rounded capsule bars, configurable resting
height, and 30/60 FPS limits are available in Visualizer settings. Version 3.1.2 docks the
visualizer during instrumental gaps and animates the *full distance* toward its far
position ahead of the next cue. Eleven easing presets control acceleration and
braking. Separate near-to-far and far-to-near presets are available, with a
simulated no-music preview that cycles both directions. Slide transitions do not
fade the bars; the optional Fade style remains available. Mirrored layouts where
artwork blocks the straight path use a short artwork-safe slide instead.
For instrumental sections, cue gaps longer than the hold limit are treated as
silent lyric gaps; unsynchronized/incorrect cue timestamps cannot predict vocals.
The mod requires Windows 11's XAML taskbar and Windhawk.
Based on the original mod by Salyts: https://github.com/Salyts/Taskbar-Fluent-Media-Player

### Credits and publication
This is a derivative of [Taskbar Fluent Media Player by Salyts](https://github.com/Salyts/Taskbar-Fluent-Media-Player).
The original mod also credits GR0UD for the audio visualizer engine.
The original Windhawk catalog mod is offered under the repository's default MIT license; attribution to the upstream contributors is retained here.
The taskbar lyrics use LRCLIB, not Spotify's built-in lyric feed; availability, language, and timing depend on LRCLIB records.
This version requires real Windows 11 / Windhawk tests before being treated as a stable release.
*/
// v3.1.5.4: Strip enhanced-LRC <mm:ss.mmm> word timestamps from displayed
// lyrics, including plain fallback. Lines that start with an enhanced timestamp
// can be read without an outer LRC line tag. Line sync remains line-based.
// LRCLIB does not provide lyrics for every recording; turn off the native-script
// only filter for Romanized Hindi/Punjabi, and enable unsynced fallback as needed.
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- MainSettings:
  - PlayerSetting:
    - displayMode: "full"
      $name: Display mode
      $options:
      - full: Media player and lyrics
      - lyrics_only: Lyrics only
    - compactLayout: true
      $name: Compact horizontal layout
      $description: Fit content with a cover up to 28 DIPs, track text up to 80, lyrics up to 120 and buttons up to 24. Reduces horizontal gaps and overrides whole-player width limits. Turn off to use larger custom sizes.
    - doubleClickTarget: "none"
      $name: Double-click target
      $description: Choose where the double-click action works. Off preserves existing click behavior. With the cover playback button enabled, a single cover click waits for the Windows double-click interval to prevent conflicting actions.
      $options:
      - none: Off
      - lyrics: Lyrics
      - album_art: Album artwork
      - both: Lyrics and album artwork
    - doubleClickAction: "open_app"
      $name: Double-click action
      $options:
      - open_app: Open music app
      - open_mini_player: Open expanded player
      - next_monitor: Move to next monitor
      - play_pause: Play / pause
    - previewLayout: false
      $name: Layout preview (turn off after arranging)
      $description: Show sample text, a cover placeholder and lyrics even when no media app is open. Playback buttons still control only a real media session.
    - horizontalOffset: 0
      #! $min: -1000
      #! $max: 1000
      #! $format: slider
      $name: Whole player horizontal offset (DIPs)
    - verticalOffset: 0
      #! $min: -80
      #! $max: 80
      #! $format: slider
      $name: Whole player vertical offset (DIPs)
    - fixedWidth: 0
      #! $min: 0
      #! $max: 1600
      #! $format: slider
      $name: Player width (DIPs; 0 = fit content / legacy min-max)
    - fixedHeight: 40
      #! $min: 0
      #! $max: 120
      #! $format: slider
      $name: Player height (DIPs; 0 = legacy min-max)
    - position: "taskbar_left_edge"
      $name: Media player position
      $name:ru-RU: Расположение медиаплеера
      $options:
      - "taskbar_left_edge": "Taskbar - Left edge (Overlay)"
      - "taskbar_center_edge": "Taskbar - Center (Overlay)"
      - "taskbar_right_edge": "Taskbar - Right edge (Overlay)"
      - "taskbar_left_start": "Taskbar - Left of Start button"
      - "taskbar_right_start": "Taskbar - Right of Start button"
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
      $options:ru-RU:
      - "taskbar_left_edge": "Панель задач - Левый край (оверлей)"
      - "taskbar_center_edge": "Панель задач - Центр (оверлей)"
      - "taskbar_right_edge": "Панель задач - Правый край (оверлей)"
      - "taskbar_left_start": "Панель задач - Слева от кнопки Пуск"
      - "taskbar_right_start": "Панель задач - Справа от кнопки Пуск"
      - "taskbar_after_search_left": "Панель задач - Слева от кнопки Поиск"
      - "taskbar_after_search_right": "Панель задач - Справа от кнопки Поиск"
      - "taskbar_after_taskview_left": "Панель задач - Слева от кнопки Представление задач"
      - "taskbar_after_taskview_right": "Панель задач - Справа от кнопки Представление задач"
      - "taskbar_after_widgets_left": "Панель задач - Слева от кнопки Мини-приложений"
      - "taskbar_after_widgets_right": "Панель задач - Справа от кнопки Мини-приложений"
      - "tray_left": "Трей - Край слева"
      - "tray_right": "Трей - Край справа"
      - "tray_before_clock": "Трей - Слева от часов"
      - "tray_after_clock": "Трей - Справа от часов"
      - "tray_before_omni_left": "Трей - Слева от кнопки Сеть/Громкость"
      - "tray_before_omni_right": "Трей - Справа от кнопки Сеть/Громкость"
      - "tray_language_left": "Трей - Слева от кнопки языка"
      -  "tray_language_right": "Трей - Справа от кнопки языка"
      - "tray_icons_left": "Трей - Слева от значков области уведомлений"
      - "tray_icons_right": "Трей - Справа от значков области уведомлений"
      - "tray_hidden_icons_left": "Трей - Слева от скрытых значков"
      - "tray_hidden_icons_right": "Трей - Справа от скрытых значков"
      - "tray_after_showdesktop_left": "Трей - Слева от кнопки Показать рабочий стол"
      - "tray_after_showdesktop_right": "Трей - Справа от кнопки Показать рабочий стол"
    - monitor: 1
      #! $min: 1
      #! $max: 16
      #! $format: slider
      $name: Monitor
      $name:ru-RU: Монитор
      $description: >-
        The monitor number the player will appear on (1, 2, 3...).
        This number may differ from the monitor number shown in
        Windows Display Settings.
      $description:ru-RU: >-
        Номер монитора, на котором будет отображаться плеер (1, 2, 3...).
        Это число может отличаться от номера монитора в параметрах
        экрана Windows.
    - playerWidth: "0 0"
      $name: Media player width (min max)
      $name:ru-RU: Ширина медиаплеера (min max)
      $description: The first number is the minimum size, and the second is the maximum. You can also set it to 0, which means no limit.
      $description:ru-RU: Первая цифра — это минимальный размер, а вторая — максимальный. Также можно указать 0 — это без лимита.
    - playerHeight: "40 40"
      $name: Media player height (min max)
      $name:ru-RU: Высота медиаплеера (min max)
    - playerMargin: "4 4"
      $name: Media player margin (left right)
      $name:ru-RU: Отступ медиаплеера (left right)
      $description: The first number is the distance to the left, and the second is to the right.
      $description:ru-RU: Первая цифра — это расстояние влево, а вторая — вправо.
    - autoSwitchSession: true
      $name: Auto-switch to active media session
      $name:ru-RU: Автопереключение на активную медиасессию
      $description: When enabled, the player automatically switches to whichever media session is currently playing. If the active session pauses and another session is already playing, the player will switch to it.
      $description:ru-RU: Если включено, плеер автоматически переключается на воспроизводящую медиасессию. Если текущая сессия поставлена на паузу, а другое приложение уже играет — плеер переключится на него.
    - mirrorLayout: false
      $name: Mirror layout
      $name:ru-RU: Зеркальное расположение
      $description: Album art, text, and buttons will be displayed on the opposite side
      $description:ru-RU: Обложка альбома, текст и кнопки будут отображаться с противоположной стороны
    - fullHeightHitArea: true
      $name: Full-height invisible hit area
      $name:ru-RU: Невидимая область клика на всю высоту
      $description: Extends the clickable/hoverable area of the player to the full height of the taskbar, even if the player itself is shorter. Disable this if you don't want the player to react to clicks/hovers above or below its visible bounds.
      $description:ru-RU: Расширяет кликабельную/наводимую область плеера на всю высоту панели задач, даже если сам плеер меньше по высоте. Отключите, если не хотите, чтобы плеер реагировал на клики/наведение выше или ниже его видимых границ.
    $name: Media player
    $name:ru-RU: Медиаплеер

  - AlbumArtSetting:
    - showAlbumArt: true
      $name: Show album art
    - albumArtPlaybackButton: true
      $name: Play/pause button on cover
      $description: The playback button appears ONLY while hovering over the cover. Clicking the cover still toggles playback. Turn off to use custom cover click actions and the legacy paused indicator.
    - albumArtSide: "auto"
      $name: Cover side
      $options:
      - auto: Follow mirror layout
      - left: Left
      - right: Right
    - albumArtSize: 32
      #! $min: 0
      #! $max: 96
      #! $format: slider
      $name: Square cover size (DIPs; 0 = legacy min-max)
    - albumArtWidth: "32 64"
      $name: Album art width (min max)
      $name:ru-RU: Ширина обложки альбома (min max)
    - albumArtHeight: "32 32"
      $name: Album art height (min max)
      $name:ru-RU: Высота обложки альбома (min max)
    - albumArtMargin: "0 0"
      $name: Album art margin (left right)
      $name:ru-RU: Отступ обложки альбома (left right)
    $name: Album Art
    $name:ru-RU: Обложка альбома

  - TextAreaSetting:
    - showTrackTitle: true
      $name: Show track title
      $name:ru-RU: Отображать название трека
    - showTrackArtist: true
      $name: Show artist name
      $name:ru-RU: Отображать имя исполнителя
    - textAreaWidth: "0 120"
      $name: Text area width (min max)
      $name:ru-RU: Ширина текстовой области (min max)
    - textAreaHeight: "0 0"
      $name: Text area height (min max)
      $name:ru-RU: Высота текстовой области (min max)
    - textAreaMargin: "5 5"
      $name: Text area margin (left right)
      $name:ru-RU: Отступ текстовой области (left right)
    - textSpacing: -1
      #! $min: -10
      #! $max: 30
      #! $format: slider
      $name: Spacing between title and artist
      $name:ru-RU: Отступ между названием и исполнителем
    - enableTitleScrolling: false
      $name: Enable track title scrolling
      $name:ru-RU: Включить прокрутку названия трека
      $description: When enabled, the track title will scroll horizontally if it overflows the text area.
      $description:ru-RU: Если включено, название трека будет прокручиваться горизонтально при переполнении текстовой области.
    - enableArtistScrolling: false
      $name: Enable artist name scrolling
      $name:ru-RU: Включить прокрутку имени исполнителя
      $description: When enabled, the artist name will scroll horizontally if it overflows the text area.
      $description:ru-RU: Если включено, имя исполнителя будет прокручиваться горизонтально при переполнении текстовой области.
    - scrollSpeed: 1
      #! $min: 1
      #! $max: 10
      #! $format: slider
      $name: Scroll speed (1-10)
      $name:ru-RU: Скорость прокрутки (1-10)
      $description: Controls how fast the text scrolls. 1 = slowest, 10 = fastest.
      $description:ru-RU: Управляет скоростью прокрутки текста. 1 = медленнее всего, 10 = быстрее всего.
    - scrollPauseDuration: 1000
      #! $min: 0
      #! $max: 10000
      #! $format: slider
      $name: Pause duration at edges (ms)
      $name:ru-RU: Пауза на краях (мс)
      $description: How long (in milliseconds) scrolling pauses at the start and end before reversing direction.
      $description:ru-RU: Сколько миллисекунд прокрутка делает паузу на начале и конце перед сменой направления.
    - scrollMode: "bounce"
      $name: Scroll mode
      $name:ru-RU: Режим прокрутки
      $options:
      - "bounce": "Bounce (back and forth)"
      - "loop":   "Loop (continuous)"
      $options:ru-RU:
      - "bounce": "Отскок (туда-обратно)"
      - "loop":   "Петля (непрерывная)"
    - loopGap: 40
      #! $min: 0
      #! $max: 500
      #! $format: slider
      $name: Loop gap (px)
      $name:ru-RU: Отступ между повторами (пикс.)
      $description: Distance in pixels between the end of the text and the beginning of its copy in Loop mode.
      $description:ru-RU: Расстояние в пикселях между концом текста и началом его копии в режиме «Петля».
    - swapTitleArtist: false
      $name: Swap artist name and track title
      $name:ru-RU: Поменять местами название трека и имя исполнителя
    - emptyTitleText: "Untitled"
      $name: Title text when track has no name
      $name:ru-RU: Текст названия, если у трека нет имени
      $description: Shown in the title field when a track is playing but has no title. Leave empty to hide the title text in this case.
      $description:ru-RU: Отображается в поле названия, когда трек играет, но у него нет имени. Оставьте пустым, чтобы скрыть текст названия в этом случае.
    - noMediaTitleText: "Not Playing"
      $name: Title text when nothing is playing
      $name:ru-RU: Текст названия, когда ничего не воспроизводится
      $description: Shown in the title field when there is no media session at all. Leave empty to hide the title text in this case.
      $description:ru-RU: Отображается в поле названия, когда нет ни одной медиа-сессии. Оставьте пустым, чтобы скрыть текст названия в этом случае.
    - emptyArtistText: ""
      $name: Artist text when track has no artist
      $name:ru-RU: Текст исполнителя, если у трека не указан автор
      $description: Shown in the artist field when a track is playing but has no artist specified. Leave empty to hide the artist text in this case.
      $description:ru-RU: Отображается в поле исполнителя, когда трек играет, но автор не указан. Оставьте пустым, чтобы скрыть текст исполнителя в этом случае.
    - noMediaArtistText: ""
      $name: Artist text when nothing is playing
      $name:ru-RU: Текст исполнителя, когда ничего не воспроизводится
      $description: Shown in the artist field when there is no media session at all. Leave empty to hide the artist text in this case.
      $description:ru-RU: Отображается в поле исполнителя, когда нет ни одной медиа-сессии. Оставьте пустым, чтобы скрыть текст исполнителя в этом случае.
    $name: Text area
    $name:ru-RU: Текстовая область

  - MediaButtonsSettings:
    - controlsSide: "auto"
      $name: Controls side
      $options:
      - auto: Follow mirror layout
      - left: Left
      - right: Right
    - showMediaButtons: true
      $name: Show media buttons
      $name:ru-RU: Отображать кнопки управления
    - mediaButtons: [prev, play, next]
      $name: Media buttons order
      $name:ru-RU: Расположение кнопок управления
      $description: Select which media control buttons to display and their order. Duplicates are ignored.
      $description:ru-RU: Выберите, какие кнопки управления воспроизведением отображать, а также их порядок. Дубликаты игнорируются.
      $options:
      - none: Nothing
      - prev: Previous Track
      - play: Play/Pause
      - next: Next Track
      - rewind: Rewind 5s
      - forward: Forward 5s
      - shuffle: Toggle Shuffle
      - repeat: Toggle Repeat
      - switch_sessions: Switch Sessions
      $options:ru-RU:
      - none: Ничего
      - prev: Предыдущий трек
      - play: Воспроизведение/Пауза
      - next: Следующий трек
      - rewind: Перемотка назад 5 сек
      - forward: Перемотка вперёд 5 сек
      - shuffle: Случайный порядок
      - repeat: Повтор
      - switch_sessions: Переключить сессию
    - mediaButtonsMargin: "2 2"
      $name: Media buttons margin (left right)
      $name:ru-RU: Отступ кнопок управления (left right)
    - buttonSize: 28
      #! $min: 16
      #! $max: 48
      #! $format: slider
      $name: Button size
      $name:ru-RU: Размер кнопок управления
    - hideUnsupportedButtons: false
      $name: Hide unsupported buttons
      $name:ru-RU: Скрывать неподдерживаемые кнопки
      $description: When enabled, buttons for actions not supported by the current media session are completely hidden instead of shown as disabled.
      $description:ru-RU: Когда включено, кнопки действий, не поддерживаемых текущей медиа-сессией, полностью скрываются вместо отображения в неактивном состоянии.
    $name: Media Buttons
    $name:ru-RU: Кнопки управления

  - VisualizerFunctionsSettings:
    - vizEnabled: false
      $name: Enable audio visualizer
      $name:ru-RU: Включить аудиовизуализатор
      $description: Real-time spectrum bars driven by the system audio output (WASAPI loopback + FFT).
      $description:ru-RU: Спектральные столбики в реальном времени, управляемые системным аудиовыходом (WASAPI loopback + FFT).
    - vizPosition: "right"
      $name: Position
      $name:ru-RU: Положение
      $description: Which side of the player the bars sit on.
      $description:ru-RU: С какой стороны плеера расположены столбики.
      $options:
      - "left": Left
      - "right": Right
      $options:ru-RU:
      - "left": Слева
      - "right": Справа
    - vizCompactEnabled: true
      $name: Dock visualizer beside cover when no lyric line is showing
      $description: Includes instrumental intros, long gaps between lines, and songs with no synchronized lyrics. Returns to the original far position when lyrics resume.
    - vizCompactGap: 0
      #! $min: 0
      #! $max: 80
      #! $format: slider
      $name: Near position - extra gap after album art (DIPs)
      $description: Extra gap in the compact dock. An independent 3-DIP safety clearance always prevents overlap even at 0. The normal neighbor-gap setting is unchanged.
    - vizCompactHorizontalOffset: 0
      #! $min: -100
      #! $max: 100
      #! $format: slider
      $name: Near position - horizontal adjustment (DIPs)
      $description: Positive adds space before bars, negative adds space after bars. Does not draw over album art.
    - vizCompactVerticalOffset: 0
      #! $min: -20
      #! $max: 20
      #! $format: slider
      $name: Near position - vertical adjustment (DIPs)
    - vizTransitionStyle: "slide"
      $name: Visualizer dock transition style
      $description: Slide visibly travels from the near dock to its original far position (and back), with selectable acceleration presets. Fade is the older appearance-only effect; Instant disables motion. A mirrored arrangement that would cross the cover uses a short slide to keep artwork visible.
      $options:
      - "slide": Smooth slide (new default)
      - "fade": Fade in at destination (previous behavior)
      - "instant": No animation
    - vizMoveAnimationMs: 350
      #! $min: 0
      #! $max: 1600
      #! $format: slider
      $name: Visualizer position transition duration (ms)
      $description: Total travel time for the near/far slide. 350 ms is quick and smooth; 500-650 ms feels more cinematic. 0 disables animation. When using lyrics, set the return-before-lyrics lead to at least the animation duration for the full effect.
    - vizNearToFarEase: "cinematic"
      $name: Near to far - movement preset
      $description: Select how the bars accelerate and slow down while traveling from the album cover to their normal far position. These presets move the visualizer across the full available distance; they do not fade it.
      $options:
      - cinematic: Cinematic (slow start, fast middle, slow stop)
      - silky: Silky (very soft acceleration and braking)
      - gentle: Gentle (light acceleration and braking)
      - dramatic: Dramatic (long slow start, quick middle, soft stop)
      - powerful: Powerful (strong acceleration and deceleration)
      - arc: Arc glide (smooth rounded movement)
      - quick: Quick launch (fast start, gradual stop)
      - feather: Feather stop (fast start, very gentle finish)
      - glide: Smooth coast (moderate start, soft finish)
      - buildup: Slow buildup (gentle start, faster finish)
      - linear: Linear (constant speed)
    - vizFarToNearEase: "match"
      $name: Far to near - movement preset
      $description: Control the return trip independently, or choose Match near to far to reuse its animation curve.
      $options:
      - match: Match near-to-far preset
      - cinematic: Cinematic (slow start, fast middle, slow stop)
      - silky: Silky (very soft acceleration and braking)
      - gentle: Gentle (light acceleration and braking)
      - dramatic: Dramatic (long slow start, quick middle, soft stop)
      - powerful: Powerful (strong acceleration and deceleration)
      - arc: Arc glide (smooth rounded movement)
      - quick: Quick launch (fast start, gradual stop)
      - feather: Feather stop (fast start, very gentle finish)
      - glide: Smooth coast (moderate start, soft finish)
      - buildup: Slow buildup (gentle start, faster finish)
      - linear: Linear (constant speed)
    - vizBeforeLyricsMs: 500
      #! $min: 0
      #! $max: 1500
      #! $format: slider
      $name: Return to far position BEFORE next lyric (ms)
      $description: 500 ms returns the visualizer half a second before the next synchronized lyric cue appears. Depends on accurate timestamps from the music player and lyric provider.
    - vizInstrumentalAfterMs: 5200
      #! $min: 500
      #! $max: 30000
      #! $format: slider
      $name: Clear held lyric after long instrumental gap (ms)
      $description: During cue gaps longer than this value, remove the old lyric and dock the visualizer when the last line has been held for this long. 5200 works well for most songs.
    - vizPreviewMode: "off"
      $name: Visualizer test preview (works without music)
      $description: Temporarily show simulated responsive bars and the player even without playback. Choose a dock to test offsets, or cycle between near/far. Set Off after testing to save power. Does not override actual audio when music is playing.
      $options:
      - "off": Off (real music only)
      - "near": Preview beside album cover
      - "far": Preview original far position
      - "cycle": Automatically alternate near / far every 4 seconds
    - vizPreviewStrength: 70
      #! $min: 10
      #! $max: 100
      #! $format: slider
      $name: Preview simulated beat strength (%)
      $description: Controls only the test pattern; real music sensitivity is unchanged.
    - vizShape: "stereo"
      $name: Bar shape
      $name:ru-RU: Форма столбиков
      $options:
      - "stereo": Stereo
      - "mountain": Mountain
      - "mirror": Mirror
      - "wave": Wave
      - "breathe": Breathe
    - vizEQ: "default"
      $name: EQ preset
      $name:ru-RU: Пресет эквалайзера
      $options:
      - "default": Balanced
      - "bass": Bass
      - "rock": Rock
      - "pop": Pop
      - "jazz": Jazz
      - "electronic": Electronic
      $options:ru-RU:
      - "default": Сбалансированный
      - "bass": Бас
      - "rock": Рок
      - "pop": Поп
      - "jazz": Джаз
      - "electronic": Электроника
    - vizHorizontalOffset: 0
      #! $min: -100
      #! $max: 100
      #! $format: slider
      $name: Far position - horizontal adjustment (DIPs)
      $description: Move the bars within reserved space; the player allocates extra room rather than overlapping its neighbors.
    - vizVerticalOffset: 0
      #! $min: -20
      #! $max: 20
      #! $format: slider
      $name: Far position - vertical adjustment (DIPs)
    - vizSectionGap: 6
      #! $min: 0
      #! $max: 40
      #! $format: slider
      $name: Far position - gap between visualizer and neighbors (DIPs)
      $description: Optional spacing in the normal layout. When lyrics are missing, a separate fixed 3-DIP safety clearance keeps the visualizer next to, never on top of, the album art even at 0.
    - vizAnchor: "middle"
      $name: Vertical anchor
      $name:ru-RU: Вертикальная привязка
      $options:
      - "top": Top
      - "middle": Middle
      - "bottom": Bottom
      $options:ru-RU:
      - "top": Сверху
      - "middle": По центру
      - "bottom": Снизу
    - vizBarCountGap: "7 5"
      $name: Bars (count gap)
      $name:ru-RU: Столбики (количество промежуток)
      $description: Two values "count gap" — number of bars (1-20) and spacing between them in px.
      $description:ru-RU: Два значения "количество промежуток" — количество столбиков (1-20) и расстояние между ними в пикселях.
    - vizBarSize: "5 3"
      $name: Bar size (width legacy resting height)
      $name:ru-RU: Размер столбика (ширина высота)
      $description: Preserved legacy setting. First value = width (0-40); second = original idle height (0-15). The separate Resting bar height slider below overrides only the idle height when set to 0 or greater. Leave that slider on -1 for identical legacy behavior.
      $description:ru-RU: Старая настройка ширины и высоты покоя; отдельный ползунок высоты покоя переопределяет её, когда значение не -1.
    - vizRestingHeight: -1
      #! $min: -1
      #! $max: 60
      #! $format: slider
      $name: Resting bar height (DIPs)
      $description: -1 = preserve your old Bar size resting-height behavior; 0 = disappear completely in silence; 1-60 = exact resting height of each stick. Capped automatically to available visualizer height. Does not affect the maximum loud height.
    - vizPadding: "0 0"
      $name: Padding (left right)
      $name:ru-RU: Отступы (слева справа)
      $description: Two values "left right" in px.
      $description:ru-RU: Два значения "слева справа" в пикселях.
    - vizRoundness: 100
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Visualizer bar-end roundness (%)
      $description: 100 = full pill-shaped bars with rounded top and bottom, including tiny idle bars; 0 = square corners. Existing bar width, gap and color stay unchanged.
    - vizResponseMode: "precise"
      $name: Music sync response
      $description: Precise = tighter peaks and faster beat response (recommended); Balanced = softer movement; Smooth = gentle movement. No artificial beat motion is added to live audio.
      $options:
      - "precise": Precise - quick attacks, natural short decay
      - "balanced": Balanced - steady, moderately smooth
      - "smooth": Smooth - relaxed, longer tails
    - vizFrameRate: "60"
      $name: Visualizer refresh limit
      $description: 60 FPS is smoothest (recommended); 30 FPS reduces UI updates and is useful on battery or slower computers. Audio capture still analyzes the music separately.
      $options:
      - "60": Smooth 60 FPS
      - "30": Economy 30 FPS
    - vizSensitivity: 150
      #! $min: 0
      #! $max: 300
      #! $format: slider
      $name: Sensitivity (0-300)
      $name:ru-RU: Чувствительность (0-300)
    $name: Visualizer
    $name:ru-RU: Визуализатор
  $name: Main Settings
  $name:ru-RU: Основные настройки

- LyricsSettings:
  - showLyrics: true
    $name: Show synchronized lyrics
  - lyricsSide: "right"
    $name: Lyrics side in full player
    $options:
    - left: Left
    - right: Right
  - onlineLookup: true
    $name: Fetch synced lyrics online (LRCLIB)
    $description: Sends track title and artist to LRCLIB via HTTPS. Disable to prevent network lyrics lookups.
  - hindiLyricsScript: "roman"
    $name: Hindi / Punjabi lyrics text
    $description: Roman keeps the previous Hinglish / Roman Punjabi behavior; Native displays the original script exactly as provided. This does not translate English lyrics to Hindi.
    $options:
    - roman: Romanized (previous behavior)
    - native: Original Devanagari / Gurmukhi script
  - lyricsRequireNativeScript: false
    $name: Strict Hindi/Punjabi source - only original-script lyrics
    $description: Optional filter for Hindi/Punjabi tracks. Accept only lyric records containing Devanagari or Gurmukhi. This prevents unrelated English text but also hides valid Romanized Hindi/Punjabi lyrics. Turn OFF for English songs and Romanized-only records. Independent of display script.
  - showPlainLyrics: true
    $name: Show unsynchronized lyrics if timed lyrics are missing
    $description: Show LRCLIB plain text as a labeled UNSYNCED preview. Lines advance at a fixed interval; this is NOT synchronized to vocals and cannot replace timed lyrics.
  - lyricsNoTimelineFallback: true
    $name: Display approximate lyrics when Spotify provides no playback timeline
    $description: If Windows gives a song title but no usable playback position, advance an estimated clock while playing rather than leaving all lyrics invisible. Sync will be approximate and seeking may be inaccurate. Disable for strict timestamp-only behavior.
  - plainLyricsLineMs: 5000
    #! $min: 2000
    #! $max: 12000
    #! $format: slider
    $name: Unsynced line display interval (ms)
    $description: Only affects clearly marked unsynced fallback lyrics; real synchronized LRC timing is unchanged.
  - lyricsWidth: 230
    #! $min: 120
    #! $max: 700
    #! $format: slider
    $name: Lyric area width (px)
    $description: Maximum width in DIPs; lyrics wrap onto a second line only when needed. Actual width narrows when the available taskbar space decreases.
  - lyricsFontSize: 12
    #! $min: 8
    #! $max: 28
    #! $format: slider
    $name: Lyric font size
    $description: For two-line lyrics in a 40 px taskbar, 12-13 usually fits better than 14.
  - lyricsTwoLineWrap: true
    $name: Wrap longer lyric lines onto a second line
    $description: Short lyrics remain one line. The second line is only used for words that do not fit. Very long lyrics can still be ellipsized after two lines.
  - lyricsAnimationMs: 400
    #! $min: 100
    #! $max: 1200
    #! $format: slider
    $name: Transition duration (ms)
    $description: How long one line takes to enter from below as the older line rises and fades.
  - lyricsSlidePixels: 18
    #! $min: 4
    #! $max: 60
    #! $format: slider
    $name: Vertical slide distance (px)
  - lyricsOffsetMs: 0
    #! $min: -10000
    #! $max: 10000
    #! $format: slider
    $name: Lyric timing adjustment (ms; +500 = 0.5 seconds early)
    $description: Set +500 to display lyrics 0.5 seconds EARLIER; -500 to display them 0.5 seconds LATER. Start at 0 and adjust for your music app.
  - lyricsHorizontalOffset: 0
    #! $min: -500
    #! $max: 500
    #! $format: slider
    $name: Lyrics position - horizontal offset (px)
    $description: Move the lyrics left (negative) or right (positive) after normal taskbar placement. Use small adjustments to avoid overlapping icons.
  - lyricsVerticalOffset: 0
    #! $min: -80
    #! $max: 80
    #! $format: slider
    $name: Lyrics position - vertical offset (px)
    $description: Move text upward (negative) or downward (positive) within the taskbar.
  - lyricsAdaptiveWidth: true
    $name: Automatically shrink lyrics as taskbar fills
    $description: In taskbar edge/Start placements, show lyrics only in unoccupied space; smoothly narrow and fade out without modifying Windows taskbar icons.
  - lyricsAdaptiveMinWidth: 72
    #! $min: 40
    #! $max: 240
    #! $format: slider
    $name: Minimum readable width (px)
    $description: Fade out lyrics if free space is smaller than this value. Taskbar icons are never resized by the lyrics view.
  - lyricsAdaptiveGap: 12
    #! $min: 0
    #! $max: 50
    #! $format: slider
    $name: Gap from taskbar icons (px)
    $description: Safety gap between the lyrics display and the first neighboring taskbar icon.
  - lyricsResizeAnimationMs: 220
    #! $min: 80
    #! $max: 800
    #! $format: slider
    $name: Adaptive resize animation duration (ms)
    $description: Smoothly animate width changes as more apps open or close.
  $name: Lyrics Settings

- SongChangeSettings:
   - enabled: true
     $name: Animate song changes
     $description: Animate only after actual track metadata changes. Keeps your existing cover size, player position and visualizer layout.
   - preset: "cinematic"
     $name: Song-change animation preset
     $options:
     - cinematic: Cinematic - fade with gentle lift and zoom
     - slide_left: Slide in from left
     - slide_right: Slide in from right
     - slide_up: Slide in from below
     - slide_down: Slide in from above
     - zoom: Soft zoom in
     - fade: Crossfade-style reveal
     - instant: No animation
   - easing: "silky"
     $name: Song-change easing preset
     $options:
     - silky: Silky - slow start, fast middle, soft stop
     - cinematic: Cinematic - strong smooth ease in/out
     - quick: Quick start, soft finish
     - linear: Linear
   - durationMs: 420
     #! $min: 100
     #! $max: 1400
     #! $format: slider
     $name: Song-change transition duration (ms)
   - travelDips: 12
     #! $min: 0
     #! $max: 45
     #! $format: slider
     $name: Song-change travel distance (DIPs)
   - fadeFrom: 15
     #! $min: 0
     #! $max: 100
     #! $format: slider
     $name: Starting opacity (%)
   - animateCover: true
     $name: Animate album artwork
   - animateText: true
     $name: Animate song title and artist
   - animateVisualizerColor: true
     $name: Blend visualizer colors when album changes
     $description: Applies to album-color modes. Solid/custom user colors are never overwritten.
- AppearanceSettings:
  - BackgroundStyleSettings:
    - backgroundType: "none"
      $name: Background type
      $name:ru-RU: Тип фона
      $options:
      - "none":           "None (transparent)"
      - "solid":          "Solid color"
      - "gradient":       "Gradient"
      - "acrylic":        "Acrylic"
      - "mica":           "Mica"
      - "mica_alt":       "Mica Alt"
      - "album_art_blur": "Blurred album cover"
      $options:ru-RU:
      - "none":           "Нет (прозрачный)"
      - "solid":          "Сплошной цвет"
      - "gradient":       "Градиент"
      - "acrylic":        "Акрил"
      - "mica":           "Mica"
      - "mica_alt":       "Mica Alt"
      - "album_art_blur": "Размытая обложка альбома"
    - solidColor: "35 35 35"
      $name: Background color (RGB)
      $name:ru-RU: Цвет фона (RGB)
      $description: "Use '-1 -1 -1' for the system contrast color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
      $description:ru-RU: "Используйте '-1 -1 -1' для системного контрастного цвета, '-2 -2 -2' — для цвета обложки альбома. Также можно указать два цвета через символ $ (например, '0 0 0$255 255 255'), где первый цвет для светлой темы, второй для тёмной."
    - solidColor2: "35 35 35"
      $name: Gradient color 1 (RGB)
      $name:ru-RU: Цвет градиента 1 (RGB)
    - gradientColor2: "128 128 128"
      $name: Gradient color 2 (RGB)
      $name:ru-RU: Цвет градиента 2 (RGB)
    - solidOpacity: 100
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Solid color opacity (0-100)
      $name:ru-RU: Прозрачность сплошного цвета (0-100)
    - gradientAngle: 50
      #! $min: 0
      #! $max: 360
      #! $format: slider
      $name: Gradient rotation angle (0-360)
      $name:ru-RU: Угол поворота градиента (0-360)
    - gradientBalance: 50
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Gradient color balance (0-100)
      $name:ru-RU: Градиентный цветовой баланс (0-100)
    - acrylicTintOpacity: 50
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Acrylic tint opacity (0-100)
      $name:ru-RU: Прозрачность акрилового оттенка (0-100)
    - micaOpacity: 50
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Mica/Mica Alt opacity (0-100)
      $name:ru-RU: "'Mica/Mica Alt' прозрачность (0-100)"
    - blurOpacity: 65
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Album art blur opacity (0-100)
      $name:ru-RU: "'Размытая обложка альбома' прозрачность (0-100)"
    - blurRadius: 11
      #! $min: 1
      #! $max: 50
      #! $format: slider
      $name: Album art blur strength (1-50)
      $name:ru-RU: "'Размытая обложка альбома' Сила размытия (1-50)"
    - cornerRadius: "4"
      $name: Media player corner radius
      $name:ru-RU: Радиус скругления медиаплеера
      $description: "Use single value (e.g., '4') for uniform corners, or four space-separated values (e.g., '4 2 4 2') for individual corners."
      $description:ru-RU: "Используйте одно значение (например, '4') для одинаковых углов, или четыре значения через пробел (например, '4 2 4 2') для каждого угла отдельно."
    - enablePlayerHoverEffect: "auto"
      $name: Player hover effect
      $name:ru-RU: Эффект при наведении на плеер
      $options:
      - "auto":  "Auto (theme changes automatically)"
      - "black": "Black"
      - "white": "White"
      - "off":   "Disable hover effect"
      $options:ru-RU:
      - "auto":  "Авто (тема изменяется автоматически)"
      - "black": "Чёрный"
      - "white": "Белый"
      - "off":   "Выключить эффект наведения"
    - enableMediaButtonsHoverEffect: "auto"
      $name: Media buttons hover effect
      $name:ru-RU: Эффект при наведении на кнопки управления
      $options:
      - "auto":  "Auto (theme changes automatically)"
      - "black": "Black"
      - "white": "White"
      - "off":   "Disable hover effect"
      $options:ru-RU:
      - "auto":  "Авто (тема изменяется автоматически)"
      - "black": "Чёрный"
      - "white": "Белый"
      - "off":   "Выключить эффект наведения"
    - enableHoverAnimation: true
      $name: Smooth hover animation
      $name:ru-RU: Плавная анимация наведения
      $description: >-
        Fades the hover/press background and border in and out. Disable this if you
        want an instant, non-animated hover effect, or if you're styling the player
        background with the Windows 11 Taskbar Styler mod and want it to react
        immediately.
      $description:ru-RU: >-
        Плавно проявляет/скрывает фон и рамку при наведении/нажатии. Выключите, если
        нужен мгновенный эффект наведения без анимации, или если вы стилизуете фон
        плеера модом Windows 11 Taskbar Styler и хотите, чтобы он реагировал мгновенно.
    $name: Background Style
    $name:ru-RU: Стиль фона

  - MediaButtonsStyleSettings:
    - iconStyle: "fluent_outline"
      $name: Icon style
      $name:ru-RU: Вид иконок
      $options:
      - "fluent_outline": "Segoe Fluent Icons (Outline)"
      - "fluent_filled":  "Segoe Fluent Icons (Filled)"
      - "mdl2_outline":   "Segoe MDL2 Assets (Outline)"
      - "mdl2_filled":    "Segoe MDL2 Assets (Filled)"
      $options:ru-RU:
      - "fluent_outline": "Segoe Fluent Icons (Контур)"
      - "fluent_filled":  "Segoe Fluent Icons (Заполненный)"
      - "mdl2_outline":   "Segoe MDL2 Assets (Контур)"
      - "mdl2_filled":    "Segoe MDL2 Assets (Заполненный)"
    - buttonSpacing: 0
      #! $min: 0
      #! $max: 30
      #! $format: slider
      $name: Spacing between media buttons
      $name:ru-RU: Расстояние между кнопками управления
    - buttonIconSize: 12
      #! $min: 8
      #! $max: 32
      #! $format: slider
      $name: Button icon size
      $name:ru-RU: Размер иконок кнопок
    - buttonCornerRadius: "4"
      $name: Media buttons corner radius
      $name:ru-RU: Скругление кнопок управления
      $description: "Use single value (e.g., '4') for uniform corners, or four space-separated values (e.g., '4 2 4 2') for individual corners."
      $description:ru-RU: "Используйте одно значение (например, '4') для одинаковых углов, или четыре значения через пробел (например, '4 2 4 2') для каждого угла отдельно."
    - buttonColor: "0 0 0$255 255 255"
      $name: Media buttons icons color (RGB)
      $name:ru-RU: Цвет иконок для кнопок управления (RGB)
      $description: "Use '-1 -1 -1' for the system contrast color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
      $description:ru-RU: "Используйте '-1 -1 -1' для системного контрастного цвета, '-2 -2 -2' — для цвета обложки альбома. Также можно указать два цвета через символ $ (например, '0 0 0$255 255 255'), где первый цвет для светлой темы, второй для тёмной."
    - buttonColorOpacity: 100
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Media buttons icons opacity (0-100)
      $name:ru-RU: Прозрачность иконок для кнопок управления (0-100)
    $name: Media Buttons Style
    $name:ru-RU: Стиль кнопок управления

  - TitleTextStyleSettings:
    - titleColor: "0 0 0$255 255 255"
      $name: Title color (RGB)
      $name:ru-RU: Цвет заголовка (RGB)
      $description: "Use '-1 -1 -1' for the system contrast color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
      $description:ru-RU: "Используйте '-1 -1 -1' для системного контрастного цвета, '-2 -2 -2' — для цвета обложки альбома. Также можно указать два цвета через символ $ (например, '0 0 0$255 255 255'), где первый цвет для светлой темы, второй для тёмной."
    - titleColorOpacity: 100
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Title opacity (0-100)
      $name:ru-RU: Прозрачность заголовка (0-100)
    - titleFont: segoe_ui_variable
      $name: Title font
      $name:ru-RU: Шрифт заголовка
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
      $options:ru-RU:
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
      - custom:            Другой...
    - titleFontSize: 12
      #! $min: 7
      #! $max: 24
      #! $format: slider
      $name: Title font size
      $name:ru-RU: Размер шрифта заголовка
    - titleFontFamily: ""
      $name: Title font family (for Custom option)
      $name:ru-RU: Семейство шрифтов заголовка (для варианта «Другой»)
      $description: >-
        For a list of fonts that are shipped with Windows 11, refer to the
        following page:
        https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
      $description:ru-RU: >-
        Список шрифтов, поставляемых с Windows 11, можно найти на следующей странице:
        https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
    - titleFontWeight: ""
      $name: Title font weight
      $name:ru-RU: Насыщенность шрифта заголовка
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
      $options:ru-RU:
      - "":          По умолчанию
      - Thin:        Тонкий
      - ExtraLight:  Очень тонкий
      - Light:       Тонкий
      - SemiLight:   Полутонкий
      - Normal:      Обычный
      - Medium:      Средний
      - SemiBold:    Полужирный
      - Bold:        Жирный
      - ExtraBold:   Очень жирный
      - Black:       Чёрный
      - ExtraBlack:  Сверхчёрный
    - titleFontStyle: ""
      $name: Title font style
      $name:ru-RU: Стиль шрифта заголовка
      $options:
      - "":       Default
      - Normal:   Normal
      - Oblique:  Oblique
      - Italic:   Italic
      $options:ru-RU:
      - "":       По умолчанию
      - Normal:   Обычный
      - Oblique:  Наклонный
      - Italic:   Курсив
    - titleCharacterSpacing: 0
      $name: Title character spacing
      $name:ru-RU: Межсимвольный интервал заголовка
      $description: Can be a positive or a negative number.
      $description:ru-RU: Может быть положительным или отрицательным числом.
    $name: Title Text Style
    $name:ru-RU: Стиль текста заголовка

  - ArtistTextStyleSettings:
    - artistColor: "0 0 0$255 255 255"
      $name: Artist color (RGB)
      $name:ru-RU: Цвет имени исполнителя (RGB)
      $description: "Use '-1 -1 -1' for system contrast color, '-2 -2 -2' for album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
      $description:ru-RU: "Используйте '-1 -1 -1' для системного контрастного цвета, '-2 -2 -2' — для цвета обложки альбома. Также можно указать два цвета через символ $ (например, '0 0 0$255 255 255'), где первый цвет для светлой темы, второй для тёмной."
    - artistColorOpacity: 80
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Artist opacity (0-100)
      $name:ru-RU: Прозрачность имени исполнителя (0-100)
    - artistFont: segoe_ui_variable
      $name: Artist font
      $name:ru-RU: Шрифт имени исполнителя
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
      $options:ru-RU:
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
      - custom:            Другой...
    - artistFontSize: 11
      #! $min: 7
      #! $max: 24
      #! $format: slider
      $name: Artist font size
      $name:ru-RU: Размер шрифта исполнителя
    - artistFontFamily: ""
      $name: Artist font family (for Custom option)
      $name:ru-RU: Семейство шрифтов исполнителя (для варианта «Другой»)
      $description: >-
        For a list of fonts that are shipped with Windows 11, refer to the
        following page:
        https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
      $description:ru-RU: >-
        Список шрифтов, поставляемых с Windows 11, можно найти на следующей странице:
        https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
    - artistFontWeight: ""
      $name: Artist font weight
      $name:ru-RU: Насыщенность шрифта исполнителя
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
      $options:ru-RU:
      - "":          По умолчанию
      - Thin:        Тонкий
      - ExtraLight:  Очень тонкий
      - Light:       Тонкий
      - SemiLight:   Полутонкий
      - Normal:      Обычный
      - Medium:      Средний
      - SemiBold:    Полужирный
      - Bold:        Жирный
      - ExtraBold:   Очень жирный
      - Black:       Чёрный
      - ExtraBlack:  Сверхчёрный
    - artistFontStyle: ""
      $name: Artist font style
      $name:ru-RU: Стиль шрифта исполнителя
      $options:
      - "":       Default
      - Normal:   Normal
      - Oblique:  Oblique
      - Italic:   Italic
      $options:ru-RU:
      - "":       По умолчанию
      - Normal:   Обычный
      - Oblique:  Наклонный
      - Italic:   Курсив
    - artistCharacterSpacing: 0
      $name: Artist character spacing
      $name:ru-RU: Межсимвольный интервал исполнителя
      $description: Can be a positive or a negative number.
      $description:ru-RU: Может быть положительным или отрицательным числом.
    $name: Artist Text Style
    $name:ru-RU: Стиль текста исполнителя

  - AlbumArtDisplaySettings:
    - albumArtEmptyBehavior: "show"
      $name: Album art behavior when no cover available
      $name:ru-RU: Поведение обложки при отсутствии изображения
      $options:
      - "show":          "Show area"
      - "hide":          "Hide area"
      - "show_icon":     "Show area with icon"
      $options:ru-RU:
      - "show":          "Показать область"
      - "hide":          "Скрыть область"
      - "show_icon":     "Показать область со значком"
    - emptyIconGlyph: "E189"
      $name: Icon glyph code (hex)
      $name:ru-RU: Код символа значка (hex)
      $description: "Hex code of the glyph to show, e.g. 'E189' for music note. See https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font"
      $description:ru-RU: "Hex-код символа, например 'E189' для ноты. См. https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font"
    - emptyIconSize: 16
      #! $min: 1
      #! $max: 256
      #! $format: slider
      $name: Icon size
      $name:ru-RU: Размер значка
    - emptyIconFont: "segoe_fluent"
      $name: Icon font style
      $name:ru-RU: Стиль шрифта значка
      $options:
      - "segoe_fluent": "Segoe Fluent Icons"
      - "segoe_mdl2":   "Segoe MDL2 Assets"
      $options:ru-RU:
      - "segoe_fluent": "Segoe Fluent Icons"
      - "segoe_mdl2":   "Segoe MDL2 Assets"
    - emptyIconColor: "140 140 140"
      $name: Icon color (RGB)
      $name:ru-RU: Цвет значка (RGB)
      $description: "Use '-1 -1 -1' for the system accent color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
      $description:ru-RU: "Используйте '-1 -1 -1' для цвета акцента системы и '-2 -2 -2' для цвета обложки альбома. Также можно указать два цвета через символ $ (например, '0 0 0$255 255 255'), где первый цвет для светлой темы, второй для тёмной."
    - emptyIconOpacity: 100
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Icon opacity (0-100)
      $name:ru-RU: Прозрачность значка (0-100)
    - albumArtQuality: "medium"
      $name: Album art quality
      $name:ru-RU: Качество обложки альбома
      $options:
      - "low":    "Low (faster, less memory)"
      - "medium": "Medium (default)"
      - "high":   "High (best quality)"
      $options:ru-RU:
      - "low":    "Низкое (быстрее, меньше памяти)"
      - "medium": "Среднее (по умолчанию)"
      - "high":   "Высокое (наилучшее качество)"
    - showPauseOverlay: true
      $name: Show pause icon overlay on album art when paused
      $name:ru-RU: Показывать значок паузы на обложке при паузе
    - pauseOverlayIconSize: 16
      #! $min: 1
      #! $max: 256
      #! $format: slider
      $name: Pause icon size
      $name:ru-RU: Размер значка паузы
    - pauseOverlayOpacity: 60
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Pause overlay background opacity (0-100)
      $name:ru-RU: Прозрачность фона оверлея паузы (0-100)
    - albumArtOpacity: 100
      #! $min: 0
      #! $max: 100
      #! $format: slider
      $name: Album art opacity (0-100)
      $name:ru-RU: Прозрачность обложки альбома (0-100)
    - albumArtCornerRadius: "4"
      $name: Album art corner radius
      $name:ru-RU: Радиус скругления обложки альбома
      $description: "Use single value (e.g., '4') for uniform corners, or four space-separated values (e.g., '4 2 4 2') for individual corners."
      $description:ru-RU: "Используйте одно значение (например, '4') для одинаковых углов, или четыре значения через пробел (например, '4 2 4 2') для каждого угла отдельно."
    - showAppIcon: false
      $name: Show media app icon overlay
      $name:ru-RU: Показывать значок медиаприложения поверх обложки
    - appIconCorner: "bottom_right"
      $name: App icon corner
      $name:ru-RU: Угол размещения значка приложения
      $options:
      - "top_left":     "Top left"
      - "top_right":    "Top right"
      - "bottom_left":  "Bottom left"
      - "bottom_right": "Bottom right"
      $options:ru-RU:
      - "top_left":     "Верхний левый"
      - "top_right":    "Верхний правый"
      - "bottom_left":  "Нижний левый"
      - "bottom_right": "Нижний правый"
    - appIconSize: 12
      #! $min: 8
      #! $max: 32
      #! $format: slider
      $name: App icon size
      $name:ru-RU: Размер значка приложения
    $name: Album Art Display
    $name:ru-RU: Отображение обложки альбома

  - VisualizerStyleSettings:
    - vizColorMode: "solid"
      $name: Color mode
      $name:ru-RU: Цветовой режим
      $options:
      - "solid": Solid
      - "dynamic_album": Dynamic album color
      - "dynamic_gradient": Dynamic gradient
      - "custom_gradient": Custom gradient
      - "acrylic": Acrylic
      $options:ru-RU:
      - "solid": Сплошной
      - "dynamic_album": Динамический цвет альбома
      - "dynamic_gradient": Динамический градиент
      - "custom_gradient": Пользовательский градиент
      - "acrylic": Акрил
    - vizColor: "0 0 0$255 255 255"
      $name: Bar color (RGB)
      $name:ru-RU: Цвет столбиков (RGB)
      $description: "Used by Solid and Acrylic modes. Use '-1 -1 -1' for the system accent color, '-2 -2 -2' for the album color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme."
      $description:ru-RU: "Используется в режимах Сплошной и Акрил. Используйте '-1 -1 -1' для системного цвета акцента, '-2 -2 -2' для цвета альбома. Также можно указать два цвета через символ $ (например, '0 0 0$255 255 255'), где первый цвет для светлой темы, второй для тёмной."
    - vizColor1: "30 215 96"
      $name: Gradient color 1 (RGB)
      $name:ru-RU: Цвет градиента 1 (RGB)
    - vizColor2: "0 180 255"
      $name: Gradient color 2 (RGB)
      $name:ru-RU: Цвет градиента 2 (RGB)
    $name: Visualizer Style
    $name:ru-RU: Стиль визуализатора
  $name: Appearance Settings
  $name:ru-RU: Настройки внешнего вида

- PlayerMenuSettings:
  - placementMode: screen
    $name: Placement mode
    $name:ru-RU: Режим расположения
    $description: Choose whether the player menu (mini player popup) opens directly Near the media player, or at a fixed position on the screen.
    $description:ru-RU: Выберите, где открывается меню плеера (мини-плеер) — Рядом с медиаплеером или в фиксированном месте на экране.
    $options:
    - near: Near the media player
    - screen: Placement on the screen
    $options:ru-RU:
    - near: Рядом с медиаплеером
    - screen: Расположение на экране

  - PlayerMenuSettingsNear:
    - miniPlayerHorizontalOffsetNear: 0
      $name: Menu horizontal offset (right/left)
      $name:ru-RU: Горизонтальное смещение меню (вправо/влево)
    - miniPlayerVerticalPlacementNear: top
      $name: Vertical placement
      $name:ru-RU: Вертикальное расположение
      $description: Whether the menu opens above or below the media player. Also controls the slide-in animation direction.
      $description:ru-RU: Появляется ли меню сверху или снизу от медиаплеера. Также определяет направление анимации появления.
      $options:
      - top: Top
      - bottom: Bottom
      $options:ru-RU:
      - top: Сверху
      - bottom: Снизу
    $name: Near the media player
    $name:ru-RU: Рядом с медиаплеером

  - PlayerMenuSettingsScreen:
    - miniPlayerHorizontalPlacement: right
      $name: Horizontal placement on the screen
      $name:ru-RU: Горизонтальное расположение на экране
      $options:
      - left: Left
      - center: Center
      - right: Right
      $options:ru-RU:
      - left: Слева
      - center: По центру
      - right: Справа

    - horizontalDistanceFromScreenEdge: 0
      $name: Distance from the right/left side of the screen
      $name:ru-RU: Расстояние от правого/левого края экрана

    - miniPlayerVerticalPlacement: bottom
      $name: Vertical placement on the screen
      $name:ru-RU: Вертикальное расположение на экране
      $options:
      - top: Top
      - center: Center
      - bottom: Bottom
      $options:ru-RU:
      - top: Сверху
      - center: По центру
      - bottom: Снизу

    - verticalDistanceFromScreenEdge: 0
      $name: Distance from the bottom/top side of the screen
      $name:ru-RU: Расстояние от нижнего/верхнего края экрана

    - miniPlayerAnimation: auto
      $name: Appearance animation
      $name:ru-RU: Анимация появления
      $options:
      - auto: Automatic
      - top: From top
      - bottom: From bottom
      - left: From left
      - right: From right
      $options:ru-RU:
      - auto: Автоматически
      - top: Сверху
      - bottom: Снизу
      - left: Слева
      - right: Справа
    $name: Placement on the screen
    $name:ru-RU: Расположение на экране

  - keepMiniPlayerOpen: false
    $name: Keep mini player menu open
    $name:ru-RU: Не закрывать мини-плеер при клике вне
  - hideMediaSessionsList: false
    $name: Hide media sessions list
    $name:ru-RU: Выключить список медиа сессий
    $description: Hide the list of other media sessions/apps in the player menu
    $description:ru-RU: Не отображать список других медиа сессий/приложений в меню плеера
  $name: Player Menu Settings
  $name:ru-RU: Настройки меню плеера

- BehaviorSettings:
  - disableAlbumArtClick: false
    $name: Disable album art click (click through to player)
    $name:ru-RU: Отключить клик по обложке (сквозной клик на плеер)
    $description: When enabled, clicks on the album art will pass through to the player area underneath
    $description:ru-RU: Когда включено, клики по обложке альбома будут передаваться в область плеера под ней
  - ClickActionSettings:
      - - object: player
          $name: Object
          $name:ru-RU: Объект
          $options:
          - none:       Nothing
          - player:     Player area
          - album_art:  Album art area
          $options:ru-RU:
          - none:       Ничего
          - player:     Область плеера
          - album_art:  Область обложки альбома
        - click: left_click
          $name: Click type
          $name:ru-RU: Тип клика
          $options:
          - none:                Nothing
          - left_click:          Left click
          - left_double_click:   Left double click
          - right_click:         Right click
          - right_double_click:  Right double click
          - middle_click:        Middle click
          - middle_double_click: Middle double click
          $options:ru-RU:
          - none:                Ничего
          - left_click:          Левый клик
          - left_double_click:   Двойной левый клик
          - right_click:         Правый клик
          - right_double_click:  Двойной правый клик
          - middle_click:        Клик средней кнопкой
          - middle_double_click: Двойной клик средней кнопкой
        - action: open_mini_player
          $name: Action
          $name:ru-RU: Действие
          $options:
          - none:            Nothing
          - switch_session:  Switch active media session
          - play_pause:      Play/Pause
          - next_track:      Next track
          - prev_track:      Previous track
          - stop:            Stop playback
          - rewind_5s:       Rewind 5s
          - forward_5s:      Forward 5s
          - toggle_shuffle:  Toggle Shuffle
          - toggle_repeat:   Toggle Repeat
          - open_app:        Open media app
          - open_context_menu: Open context menu
          - open_mini_player: Open player menu
          $options:ru-RU:
          - none:            Ничего
          - switch_session:  Переключить медиасессию
          - play_pause:      Воспроизведение/Пауза
          - next_track:      Следующий трек
          - prev_track:      Предыдущий трек
          - stop:            Остановить воспроизведение
          - rewind_5s:       Перемотка назад 5 сек
          - forward_5s:      Перемотка вперёд 5 сек
          - toggle_shuffle:  Случайный порядок
          - toggle_repeat:   Повтор
          - open_app:        Открыть медиаприложение
          - open_context_menu: Открыть контекстное меню
          - open_mini_player: Открыть меню плеера
      - - object: player
        - click: right_click
        - action: open_context_menu
      - - object: album_art
        - click: left_double_click
        - action: open_app
    $name: Click Actions
    $name:ru-RU: Действия при клике
  - MouseWheelActionSettings:
      - - object: player
          $name: Object
          $name:ru-RU: Объект
          $options:
          - none:       Nothing
          - player:     Player area
          - album_art:  Album art area
          $options:ru-RU:
          - none:       Ничего
          - player:     Область плеера
          - album_art:  Область обложки альбома
        - click: mouse_wheel
          $name: Mouse type
          $name:ru-RU: Тип мыши
          $options:
          - none:             Nothing
          - mouse_wheel:      Mouse wheel
          $options:ru-RU:
          - none:             Ничего
          - mouse_wheel:      Колесо мыши
        - action: switch_tracks
          $name: Action
          $name:ru-RU: Действие
          $options:
          - none:                      Nothing
          - "switch_tracks":           "Switch tracks"
          - "switch_tracks_inverted":  "Switch tracks (inverted)"
          - "switch_sessions":         "Switch sessions"
          - "system_sound":            "Change system sound volume"
          - "app_sound":               "Change app sound volume"
          $options:ru-RU:
          - none:                      Ничего
          - "switch_tracks":           "Переключить треки"
          - "switch_tracks_inverted":  "Переключить треки (инверсия)"
          - "switch_sessions":         "Переключить сессии"
          - "system_sound":            "Изменить громкость системы"
          - "app_sound":               "Изменить громкость приложения"
      - - object: album_art
        - click: mouse_wheel
        - action: switch_sessions
    $name: Mouse wheel Actions
    $name:ru-RU: Действия колеса мыши
  - hideWhenNoMedia: true
    $name: Hide when no media is playing
    $name:ru-RU: Скрывать, когда ничего не воспроизводится
  - hideFullscreen: true
    $name: Hide when a fullscreen app is running
    $name:ru-RU: Скрывать при запущенном полноэкранном приложении
    $description: Existing behavior; full-screen is detected on the player's own monitor, never from a split/maximized window.
  - allowInFullscreen: false
    $name: Allow player in F11 / borderless fullscreen
    $description: Overrides "Hide when a fullscreen app is running" when enabled. Windows may still hide its entire taskbar during fullscreen use; this mod does not force the taskbar above apps.
  - idleHideSeconds: 0
    $name: Idle auto-hide timeout (seconds, 0 = disabled)
    $name:ru-RU: Таймаут автоскрытия при бездействии (секунды, 0 = отключено)
  - showFullTitleOnHover: true
    $name: Show full track title on hover (tooltip)
    $name:ru-RU: Показывать полное название трека при наведении (подсказка)
  $name: Behavior Settings
  $name:ru-RU: Настройки поведения

- AnimationSettings:
  - enableSmoothPositionAnimation: true
    $name: Enable smooth position animation
    $name:ru-RU: Включить плавную анимацию позиции
  $name: Animation Settings media player
  $name:ru-RU: Настройки анимации медиаплеера

- NotificationSettings:
  - showSuccessNotification: false
    $name: Log successful startup (no popup)
    $description: Write successful initialization to the Windhawk log. No modal Windows notification is opened.
  $name: Notification Settings
  $name:ru-RU: Настройки уведомлений

- ContextMenuSettings:
  - contextMenuItems: [switch_sessions, open_app]
    $name: Context menu items
    $name:ru-RU: Пункты контекстного меню
    $description: Select which items to show in the context menu and their order. Duplicates are ignored.
    $description:ru-RU: Выберите, какие пункты отображать в контекстном меню, и их порядок. Дубликаты игнорируются.
    $options:
    - none:             Nothing
    - repeat:           Repeat
    - shuffle:          Shuffle
    - forward:          Forward 5s
    - rewind:           Rewind 5s
    - next:             Next Track
    - prev:             Previous Track
    - switch_sessions:  Switch Sessions
    - open_app:         Open media app
    $options:ru-RU:
    - none:             Ничего
    - repeat:           Повтор
    - shuffle:          Случайный порядок
    - forward:          Вперёд 5 сек
    - rewind:           Назад 5 сек
    - next:             Следующий трек
    - prev:             Предыдущий трек
    - switch_sessions:  Переключить сессию
    - open_app:         Открыть медиаприложение
  - repeatStyle: "submenu"
    $name: Repeat style
    $name:ru-RU: Стиль повтора
    $options:
    - "submenu": "Submenu (Repeat off / Repeat all / Repeat one)"
    - "toggle":  "Toggle (cycle through modes)"
    $options:ru-RU:
    - "submenu": "Подменю (Выкл / Все / Один)"
    - "toggle":  "Переключатель (цикл по режимам)"
  - shuffleStyle: "toggle"
    $name: Shuffle style
    $name:ru-RU: Стиль случайного воспроизведения
    $options:
    - "submenu": "Submenu (Shuffle off / Shuffle on)"
    - "toggle":  "Toggle (single button)"
    $options:ru-RU:
    - "submenu": "Подменю (Выкл / Вкл)"
    - "toggle":  "Переключатель (одна кнопка)"
  - showOpenWindhawk: true
    $name: Show "Open Windhawk" button
    $name:ru-RU: Показывать кнопку «Открыть Windhawk»
  - contextMenuIconStyle: "as_media_buttons"
    $name: Context menu icon style
    $name:ru-RU: Стиль иконок контекстного меню
    $options:
    - "as_media_buttons": "Same as Media Buttons"
    - "fluent_outline":   "Segoe Fluent Icons (Outline)"
    - "fluent_filled":    "Segoe Fluent Icons (Filled)"
    - "mdl2_outline":     "Segoe MDL2 Assets (Outline)"
    - "mdl2_filled":      "Segoe MDL2 Assets (Filled)"
    $options:ru-RU:
    - "as_media_buttons": "Как у кнопок управления"
    - "fluent_outline":   "Segoe Fluent Icons (Контур)"
    - "fluent_filled":    "Segoe Fluent Icons (Заполненный)"
    - "mdl2_outline":     "Segoe MDL2 Assets (Контур)"
    - "mdl2_filled":      "Segoe MDL2 Assets (Заполненный)"
  - contextMenuIconColor: "0 0 0$255 255 255"
    $name: Context menu icons color (RGB)
    $name:ru-RU: Цвет иконок контекстного меню (RGB)
    $description: "Use '-1 -1 -1' for the system contrast color and '-2 -2 -2' for the album art color. You can also specify two colors separated by a $ symbol (e.g., '0 0 0$255 255 255') where the first color is for light theme and the second for dark theme. Leave empty to use the Media Buttons color."
    $description:ru-RU: "Используйте '-1 -1 -1' для системного контрастного цвета, '-2 -2 -2' — для цвета обложки альбома. Также можно указать два цвета через $ (например, '0 0 0$255 255 255'). Оставьте пустым, чтобы использовать цвет кнопок управления."
  - contextMenuIconOpacity: 100
    #! $min: 0
    #! $max: 100
    #! $format: slider
    $name: Context menu icons opacity (0-100)
    $name:ru-RU: Прозрачность иконок контекстного меню (0-100)
  $name: Context Menu Settings
  $name:ru-RU: Настройки контекстного меню

- PowerSettings:
  - lyricRefreshMs: 100
    #! $min: 50
    #! $max: 1000
    #! $format: slider
    $name: Lyric refresh interval (ms)
    $description: Lower values improve timing; battery mode uses at least 200 ms. Paused or absent media uses a 1-second wait.
  - batterySaver: true
    $name: Reduce animation and polling on battery
    $description: Stop the visualizer, pause marquee scrolling and disable lyric transitions while running on battery or Windows energy saver.
  $name: Power and battery

- DebugSettings:
  - ignoredProcesses: ""
    $name: Ignore media from processes (separate with ; )
    $name:ru-RU: Игнорировать медиа от процессов (разделять через ; )
  - enableTreeDump: false
    $name: Dump XAML element names to log on inject
    $name:ru-RU: Записывать имена XAML-элементов в лог при инъекции
  - showDebugBorders: false
    $name: Show debug borders
    $name:ru-RU: Показывать отладочные границы
  - showMiniPlayerBorder: false
    $name: Show debug mini player menu border
    $name:ru-RU: Показывать отладочные границу мини-плеера
  - showLayoutAnchors: false
    $name: Show layout anchors and centers
    $name:ru-RU: Показывать якоря и центры разметки
  - showRestartButton: false
    $name: Show Restart Player in context menu
    $name:ru-RU: Показывать кнопку перезагрузки плеера в контекстном меню
  $name: Debug Settings
  $name:ru-RU: Настройки отладки
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime
#include <exception>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Data.Json.h>
#include <winhttp.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <robuffer.h>
#include <shcore.h>
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <commoncontrols.h>
#include <wincodec.h>
#include <propsys.h>
#include <dwmapi.h>
#include <windhawk_utils.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <propkey.h>
#include <atomic>
#include <functional>
#include <memory>
#include <utility>
#include <type_traits>
#include <mutex>
#include <string>
#include <vector>
#include <optional>
#include <set>
#include <algorithm>
#include <thread>
#include <cmath>
#include <chrono>
#include <unordered_map>
#include <cctype>
#include <cwchar>
#include <cwctype>
#include <cstdint>
#include <limits>
// BEGIN PORTABLE SETTINGS LIFECYCLE
namespace LiveSettings {
template <class Actions>
static bool Apply(Actions& actions) noexcept {
    try { actions.Pause(); }
    catch (...) { actions.ReportFailure(); return false; }
    bool applied = false;
    try {
        actions.StopProducers();
        actions.DrainWorkers();
        applied = actions.ReconfigureOnUi();
    } catch (...) { actions.ReportFailure(); }
    // Resume even if a settings read/UI dispatch fails; never request DLL unload.
    try { actions.Resume(); }
    catch (...) { actions.ReportFailure(); return false; }
    return applied;
}
}
// END PORTABLE SETTINGS LIFECYCLE

// BEGIN PORTABLE PLAYER GEOMETRY
namespace PlayerGeometry {
struct Interval { double left, right; };
struct Placement { double x = 0, width = 0; bool visible = false; };
enum class Anchor { LeftEdge, Center, RightEdge, Before, After };
static Placement Place(double extent, double requested, double minimum,
                       double marginLeft, double marginRight, double gap,
                       Anchor mode, double anchorX, double anchorWidth,
                       double offset, std::vector<Interval> occupied) {
    if (!std::isfinite(extent) || !std::isfinite(requested) || extent <= 0 || requested <= 0) return {};
    const double left = std::clamp(marginLeft, 0.0, extent);
    const double right = std::clamp(extent - marginRight, left, extent);
    gap = std::max(0.0, gap);
    for (auto& r : occupied) {
        r.left = std::clamp(r.left - gap, left, right);
        r.right = std::clamp(r.right + gap, left, right);
    }
    std::sort(occupied.begin(), occupied.end(), [](auto a, auto b) { return a.left < b.left; });
    std::vector<Interval> free;
    double cursor = left;
    for (const auto& r : occupied) {
        if (r.right <= r.left) continue;
        if (r.left > cursor) free.push_back({cursor, r.left});
        cursor = std::max(cursor, r.right);
    }
    if (cursor < right) free.push_back({cursor, right});
    Placement best;
    double bestScore = std::numeric_limits<double>::infinity();
    for (auto r : free) {
        if (mode == Anchor::Before) r.right = std::min(r.right, anchorX - gap);
        if (mode == Anchor::After) r.left = std::max(r.left, anchorX + anchorWidth + gap);
        const double width = std::min(requested, r.right - r.left);
        if (width < minimum || width <= 0) continue;
        double desired = r.left;
        switch (mode) {
        case Anchor::LeftEdge: desired = left; break;
        case Anchor::RightEdge: desired = right - width; break;
        case Anchor::Center: desired = (extent - width) / 2; break;
        case Anchor::Before: desired = anchorX - gap - width; break;
        case Anchor::After: desired = anchorX + anchorWidth + gap; break;
        }
        const double x = std::clamp(desired + offset, r.left, r.right - width);
        // Prefer a full-size slot; only compact lyrics may shrink as space fills.
        const double score = (requested - width) * (extent + 1) + std::abs(x - desired - offset);
        if (score < bestScore) { best = {x, width, true}; bestScore = score; }
    }
    return best;
}
}
// END PORTABLE PLAYER GEOMETRY

// BEGIN PORTABLE COMPACT LAYOUT
// Apply after normalizing persisted sizes so upgrades also get the compact preset.
template <class Settings>
static void ApplyCompactLayout(Settings& settings) {
    if (!settings.compactLayout || settings.lyricsOnly) return;
    auto capSize = [](int& minimum, int& maximum, int cap) {
        maximum = maximum > 0 ? std::min(maximum, cap) : cap;
        minimum = std::min(minimum, maximum);
    };
    settings.playerMinWidth = settings.playerMaxWidth = 0; // Fit actual content.
    capSize(settings.albumArtMinWidth, settings.albumArtMaxWidth, 28);
    capSize(settings.albumArtMinHeight, settings.albumArtMaxHeight, 28);
    capSize(settings.textAreaMinWidth, settings.textAreaMaxWidth, 80);
    settings.lyricsWidth = std::min(settings.lyricsWidth, 120);
    settings.buttonSize = std::min(settings.buttonSize, 24);
    settings.buttonIconSize = std::min(settings.buttonIconSize, settings.buttonSize - 4);
    settings.buttonSpacing = std::clamp(settings.buttonSpacing, 0, 2);
    for (int* margin : {&settings.playerMarginLeft, &settings.playerMarginRight,
                       &settings.albumArtLeftMargin, &settings.albumArtRightMargin,
                       &settings.textAreaLeftMargin, &settings.textAreaRightMargin,
                       &settings.mediaButtonsLeftMargin, &settings.mediaButtonsRightMargin})
        *margin = std::clamp(*margin, 0, 2);
}
// END PORTABLE COMPACT LAYOUT

// BEGIN PORTABLE DOUBLE CLICK
namespace DoubleClickGesture {
struct State {
    bool pending = false;
    uint64_t time = 0;
    double x = 0, y = 0;
    enum class Result { Pending, FlushSingle, Double };
    Result Click(uint64_t now, double px, double py, uint64_t interval, double dx, double dy) {
        if (pending && now >= time && now - time <= interval &&
            std::abs(px - x) <= dx && std::abs(py - y) <= dy) {
            pending = false;
            return Result::Double;
        }
        const bool flush = pending;
        pending = true; time = now; x = px; y = py;
        return flush ? Result::FlushSingle : Result::Pending;
    }
    bool Flush() { const bool result = pending; pending = false; return result; }
};
}
// END PORTABLE DOUBLE CLICK

// BEGIN PORTABLE MONITOR CYCLE
namespace MonitorCycle {
// Skip displays without an Explorer taskbar and wrap after the last display.
static int Next(const std::vector<bool>& available, int current) {
    const int count = (int)available.size();
    if (!count) return -1;
    if (current < 0 || current >= count) {
        for (int i = 0; i < count; ++i) if (available[i]) return i;
        return -1;
    }
    for (int step = 1; step < count; ++step) {
        const int index = (current + step) % count;
        if (available[index]) return index;
    }
    return available[current] ? current : -1;
}
}
// END PORTABLE MONITOR CYCLE




using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::Windows::UI::Xaml::Media::Animation;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Storage::Streams;
enum class VizShape { Stereo, Mountain, Mirror, Wave, Breathe };
enum class VizColorMode { Solid, DynamicAlbum, DynamicGradient, CustomGradient, Acrylic };
enum class VizEQ { Default, Bass, Rock, Pop, Jazz, Electronic };
enum class VizAnchor { Top, Middle, Bottom };
struct ModSettings {
    bool         lyricsOnly           = false;
    bool         showLyrics           = true;
    bool         compactLayout        = true;
    std::wstring doubleClickTarget    = L"none";
    std::wstring doubleClickAction    = L"open_app";
    bool         previewLayout        = false;
    int          horizontalOffset     = 0;
    int          verticalOffset       = 0;
    int          fixedWidth           = 0;
    int          fixedHeight          = 40;
    int          albumArtSize         = 32;
    std::wstring albumArtSide         = L"auto";
    std::wstring controlsSide         = L"auto";
    std::wstring lyricsSide           = L"right";
    int          lyricRefreshMs       = 100;
    bool         batterySaver         = true;
    bool         lyricsOnlineLookup   = true;
    std::wstring hindiLyricsScript = L"roman";
    bool         lyricsRequireNativeScript = false;
    bool         showPlainLyrics = true;
    bool         lyricsNoTimelineFallback = true;
    int          plainLyricsLineMs = 5000;
    bool         songChangeEnabled = true;
    std::wstring songChangePreset = L"cinematic";
    std::wstring songChangeEase = L"silky";
    int          songChangeDurationMs = 420;
    int          songChangeTravel = 12;
    int          songChangeStartOpacity = 15;
    bool         songChangeCover = true;
    bool         songChangeText = true;
    bool         songChangeVizColor = true;
    int          lyricsWidth          = 230;
    int          lyricsFontSize       = 13;
    bool         lyricsTwoLineWrap    = true;
    int          lyricsAnimationMs    = 400;
    int          lyricsSlidePixels    = 18;
    int          lyricsOffsetMs       = 0;
    int          lyricsHorizontalOffset = 0;
    int          lyricsVerticalOffset = 0;
    bool         lyricsAdaptiveWidth = true;
    int          lyricsAdaptiveMinWidth = 72;
    int          lyricsAdaptiveGap = 12;
    int          lyricsResizeAnimationMs = 220;
    int          monitor              = 1;
    std::wstring position             = L"taskbar_left_edge";
    std::wstring albumArtLeftClick    = L"none";
    std::wstring albumArtRightClick   = L"none";
    std::wstring albumArtMiddleClick  = L"none";
    std::wstring albumArtLeftDoubleClick  = L"none";
    std::wstring albumArtRightDoubleClick = L"none";
    std::wstring albumArtMiddleDoubleClick = L"none";
    std::wstring albumArtWheelAction  = L"none";
    std::wstring playerLeftClick      = L"none";
    std::wstring playerRightClick     = L"none";
    std::wstring playerMiddleClick    = L"none";
    std::wstring playerLeftDoubleClick  = L"none";
    std::wstring playerRightDoubleClick = L"none";
    std::wstring playerMiddleDoubleClick = L"none";
    std::wstring playerWheelAction    = L"none";
    bool         mirrorLayout         = false;
    bool         fullHeightHitArea    = true;
    bool         autoSwitchSession    = true;
    bool         showMediaButtons     = true;
    int          playerMinWidth       = 0;
    int          playerMaxWidth       = 0;
    int          playerMinHeight      = 40;
    int          playerMaxHeight      = 40;
    bool         showAlbumArt         = true;
    std::wstring albumArtEmptyBehavior = L"show";
    std::wstring emptyIconGlyph       = L"E189";
    int          emptyIconSize        = 16;
    std::wstring emptyIconFont        = L"segoe_fluent";
    std::wstring emptyIconColor       = L"255 255 255";
    int          emptyIconOpacity     = 100;
    std::wstring albumArtQuality      = L"medium";
    bool         albumArtPlaybackButton = true;
    bool         showPauseOverlay     = true;
    int          pauseOverlayIconSize = 16;
    int          pauseOverlayOpacity  = 60;
    int          albumArtMinWidth     = 32;
    int          albumArtMaxWidth     = 64;
    int          albumArtMinHeight    = 32;
    int          albumArtMaxHeight    = 32;
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
    bool         enableHoverAnimation = true;
    bool         enableSmoothPositionAnimation = true;
    int          playerMarginLeft     = 4;
    int          playerMarginRight    = 4;
    int          mediaButtonsLeftMargin  = 2;
    int          mediaButtonsRightMargin = 2;
    int          textAreaMinWidth     = 0;
    int          textAreaMaxWidth     = 120;
    int          textAreaMinHeight    = 0;
    int          textAreaMaxHeight    = 0;
    int          textAreaLeftMargin   = 5;
    int          textAreaRightMargin  = 5;
    bool         hideFullscreen       = true;
    bool         allowInFullscreen    = false;
    int          idleHideSeconds      = 0;
    std::wstring backgroundType       = L"none";
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
    std::wstring scrollMode           = L"bounce";
    int          loopGap              = 40;
    std::wstring solidColor           = L"35 35 35";
    std::wstring solidColor2          = L"35 35 35";
    std::wstring gradientColor2       = L"128 128 128";
    int          solidOpacity         = 100;
    int          gradientAngle        = 50;
    int          gradientBalance      = 50;
    int          acrylicTintOpacity   = 50;
    int          micaOpacity          = 50;
    std::wstring buttonColor          = L"255 255 255";
    int          buttonColorOpacity   = 100;
    std::wstring titleColor           = L"255 255 255";
    int          titleColorOpacity    = 100;
    std::wstring artistColor          = L"255 255 255";
    int          artistColorOpacity   = 80;
    std::wstring ignoredProcesses     = L"";
    bool         enableTreeDump       = false;
    bool         showDebugBorders     = false;
    bool         showMiniPlayerBorder = false;
    bool         keepMiniPlayerOpen   = false;
    bool         hideMediaSessionsList = false;
    std::wstring miniPlayerPlacementMode = L"screen";
    int          miniPlayerHorizontalOffsetAbove = 0;
    std::wstring miniPlayerVerticalPlacementNear = L"top";
    std::wstring miniPlayerHorizontalPlacement = L"right";
    int          miniPlayerHorizontalDistanceFromScreenEdge = 0;
    std::wstring miniPlayerVerticalPlacement = L"bottom";
    int          miniPlayerVerticalDistanceFromScreenEdge = 0;
    std::wstring miniPlayerAnimation = L"auto";
    bool         showLayoutAnchors    = false;
    bool         showRestartButton    = false;
    bool         showSuccessNotification = false;
    bool         hideUnsupportedButtons  = false;
    bool         disableAlbumArtClick    = false;
    std::vector<std::wstring> contextMenuItems;
    std::wstring contextMenuRepeatStyle   = L"submenu";
    std::wstring contextMenuShuffleStyle  = L"submenu";
    bool         showOpenWindhawk         = true;
    std::wstring contextMenuIconStyle     = L"as_media_buttons";
    std::wstring contextMenuIconColor     = L"255 255 255";
    int          contextMenuIconOpacity   = 100;
    bool         vizEnabled      = false;
    std::wstring vizPosition     = L"right";
    int          vizHorizontalOffset = 0;
    int          vizVerticalOffset   = 0;
    int          vizSectionGap       = 6;
    bool         vizCompactEnabled   = true;
    int          vizCompactGap       = 0;
    int          vizCompactHorizontalOffset = 0;
    int          vizCompactVerticalOffset   = 0;
    int          vizMoveAnimationMs  = 350;
    std::wstring vizTransitionStyle = L"slide";
    std::wstring vizNearToFarEase = L"cinematic";
    std::wstring vizFarToNearEase = L"match";
    int          vizBeforeLyricsMs   = 500;
    int          vizInstrumentalAfterMs = 5200;
    std::wstring vizPreviewMode      = L"off";
    int          vizPreviewStrength  = 70;
    VizShape     vizShape        = VizShape::Stereo;
    VizColorMode vizColorMode    = VizColorMode::Solid;
    VizEQ        vizEq           = VizEQ::Default;
    VizAnchor    vizAnchor       = VizAnchor::Middle;
    std::wstring vizColor        = L"255 255 255";
    std::wstring vizColor1       = L"30 215 96";
    std::wstring vizColor2       = L"0 180 255";
    int          vizBars         = 7;
    int          vizBarWidth     = 5;
    int          vizBarGap       = 5;
    int          vizIdleBarSize  = 3;
    int          vizRestingHeight = -1;  // -1 = preserve the old Width/Idle setting
    int          vizSensitivity  = 150;
    int          vizRoundness    = 100;
    std::wstring vizResponseMode = L"precise";
    std::wstring vizFrameRate    = L"60";
    int          vizPadLeft      = 0;
    int          vizPadRight     = 0;
};
static ModSettings g_settings;
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
    Rewind5s = 4,
    Forward5s = 5,
    Shuffle = 6,
    Repeat = 7,
    SwitchSessions = 8,
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
    {L"switch_sessions", MediaButtonType::SwitchSessions, 9},
};
struct MediaButtonConfig {
    MediaButtonType type;
    int cmd;
};
static std::vector<MediaButtonConfig> g_mediaButtons;
static std::mutex g_mediaButtonsMutex;
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
static void LoadSettings() {
    auto Str = [](const wchar_t* key, const wchar_t* def) -> std::wstring {
        PCWSTR p = Wh_GetStringSetting(key);
        std::wstring r = (p && *p != L'\0') ? p : def;
        Wh_FreeStringSetting(p);
        return r;
    };
    auto StrAllowEmpty = [](const wchar_t* key) -> std::wstring {
        PCWSTR p = Wh_GetStringSetting(key);
        std::wstring r = p ? p : L"";
        Wh_FreeStringSetting(p);
        return r;
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
    g_settings.lyricsOnly = Str(L"MainSettings.PlayerSetting.displayMode", L"full") == L"lyrics_only";
    g_settings.showLyrics = g_settings.lyricsOnly || Wh_GetIntSetting(L"LyricsSettings.showLyrics") != 0;
    g_settings.compactLayout = Wh_GetIntSetting(L"MainSettings.PlayerSetting.compactLayout") != 0;
    g_settings.doubleClickTarget = Str(L"MainSettings.PlayerSetting.doubleClickTarget", L"none");
    g_settings.doubleClickAction = Str(L"MainSettings.PlayerSetting.doubleClickAction", L"open_app");
    if (g_settings.doubleClickTarget != L"lyrics" && g_settings.doubleClickTarget != L"album_art" &&
        g_settings.doubleClickTarget != L"both") g_settings.doubleClickTarget = L"none";
    if (g_settings.doubleClickAction != L"open_app" && g_settings.doubleClickAction != L"open_mini_player" &&
        g_settings.doubleClickAction != L"play_pause" && g_settings.doubleClickAction != L"next_monitor") g_settings.doubleClickAction = L"open_app";
    g_settings.previewLayout = Wh_GetIntSetting(L"MainSettings.PlayerSetting.previewLayout") != 0;
    g_settings.horizontalOffset = Int(L"MainSettings.PlayerSetting.horizontalOffset", -1000, 1000, 0);
    g_settings.verticalOffset = Int(L"MainSettings.PlayerSetting.verticalOffset", -80, 80, 0);
    g_settings.fixedWidth = Int(L"MainSettings.PlayerSetting.fixedWidth", 0, 1600, 0);
    g_settings.fixedHeight = Int(L"MainSettings.PlayerSetting.fixedHeight", 0, 120, 40);
    g_settings.albumArtSize = Int(L"MainSettings.AlbumArtSetting.albumArtSize", 0, 96, 32);
    g_settings.albumArtSide = Str(L"MainSettings.AlbumArtSetting.albumArtSide", L"auto");
    g_settings.controlsSide = Str(L"MainSettings.MediaButtonsSettings.controlsSide", L"auto");
    g_settings.lyricsSide = Str(L"LyricsSettings.lyricsSide", L"right");
    g_settings.lyricRefreshMs = Int(L"PowerSettings.lyricRefreshMs", 50, 1000, 100);
    g_settings.batterySaver = Wh_GetIntSetting(L"PowerSettings.batterySaver") != 0;
    g_settings.lyricsOnlineLookup     = Wh_GetIntSetting(L"LyricsSettings.onlineLookup") != 0;
    g_settings.hindiLyricsScript = Str(L"LyricsSettings.hindiLyricsScript", L"roman");
    if (g_settings.hindiLyricsScript != L"native") g_settings.hindiLyricsScript = L"roman";
    g_settings.lyricsRequireNativeScript = Wh_GetIntSetting(L"LyricsSettings.lyricsRequireNativeScript") != 0;
    g_settings.showPlainLyrics = Wh_GetIntSetting(L"LyricsSettings.showPlainLyrics") != 0;
    g_settings.lyricsNoTimelineFallback = Wh_GetIntSetting(L"LyricsSettings.lyricsNoTimelineFallback") != 0;
    g_settings.plainLyricsLineMs = Int(L"LyricsSettings.plainLyricsLineMs", 2000, 12000, 5000);
    g_settings.songChangeEnabled = Wh_GetIntSetting(L"SongChangeSettings.enabled") != 0;
    g_settings.songChangePreset = Str(L"SongChangeSettings.preset", L"cinematic");
    if (g_settings.songChangePreset != L"cinematic" && g_settings.songChangePreset != L"slide_left" &&
        g_settings.songChangePreset != L"slide_right" && g_settings.songChangePreset != L"slide_up" &&
        g_settings.songChangePreset != L"slide_down" && g_settings.songChangePreset != L"zoom" &&
        g_settings.songChangePreset != L"fade" && g_settings.songChangePreset != L"instant")
        g_settings.songChangePreset = L"cinematic";
    g_settings.songChangeEase = Str(L"SongChangeSettings.easing", L"silky");
    if (g_settings.songChangeEase != L"silky" && g_settings.songChangeEase != L"cinematic" &&
        g_settings.songChangeEase != L"quick" && g_settings.songChangeEase != L"linear")
        g_settings.songChangeEase = L"silky";
    g_settings.songChangeDurationMs = Int(L"SongChangeSettings.durationMs", 100, 1400, 420);
    g_settings.songChangeTravel = Int(L"SongChangeSettings.travelDips", 0, 45, 12);
    g_settings.songChangeStartOpacity = Int(L"SongChangeSettings.fadeFrom", 0, 100, 15);
    g_settings.songChangeCover = Wh_GetIntSetting(L"SongChangeSettings.animateCover") != 0;
    g_settings.songChangeText = Wh_GetIntSetting(L"SongChangeSettings.animateText") != 0;
    g_settings.songChangeVizColor = Wh_GetIntSetting(L"SongChangeSettings.animateVisualizerColor") != 0;
    g_settings.lyricsWidth            = Int(L"LyricsSettings.lyricsWidth", 120, 700, 230);
    g_settings.lyricsFontSize         = Int(L"LyricsSettings.lyricsFontSize", 8, 28, 12);
    g_settings.lyricsTwoLineWrap      = Wh_GetIntSetting(L"LyricsSettings.lyricsTwoLineWrap") != 0;
    g_settings.lyricsAnimationMs      = Int(L"LyricsSettings.lyricsAnimationMs", 100, 1200, 400);
    g_settings.lyricsSlidePixels      = Int(L"LyricsSettings.lyricsSlidePixels", 4, 60, 18);
    g_settings.lyricsOffsetMs         = Int(L"LyricsSettings.lyricsOffsetMs", -10000, 10000, 0);
    g_settings.lyricsHorizontalOffset = Int(L"LyricsSettings.lyricsHorizontalOffset", -500, 500, 0);
    g_settings.lyricsVerticalOffset   = Int(L"LyricsSettings.lyricsVerticalOffset", -80, 80, 0);
    g_settings.lyricsAdaptiveWidth     = Wh_GetIntSetting(L"LyricsSettings.lyricsAdaptiveWidth") != 0;
    g_settings.lyricsAdaptiveMinWidth  = Int(L"LyricsSettings.lyricsAdaptiveMinWidth", 40, 240, 72);
    g_settings.lyricsAdaptiveGap       = Int(L"LyricsSettings.lyricsAdaptiveGap", 0, 50, 12);
    g_settings.lyricsResizeAnimationMs = Int(L"LyricsSettings.lyricsResizeAnimationMs", 80, 800, 220);
    g_settings.monitor               = std::max(1, Wh_GetIntSetting(L"MainSettings.PlayerSetting.monitor"));
    g_settings.position             = Str(L"MainSettings.PlayerSetting.position",    L"taskbar_left_edge");
    ParseMargin(L"MainSettings.PlayerSetting.playerMargin", L"4 4", g_settings.playerMarginLeft, g_settings.playerMarginRight);
    ParseMargin(L"MainSettings.PlayerSetting.playerWidth", L"0 0", g_settings.playerMinWidth, g_settings.playerMaxWidth);
    ParseMargin(L"MainSettings.PlayerSetting.playerHeight", L"40 40", g_settings.playerMinHeight, g_settings.playerMaxHeight);
    ParseMargin(L"MainSettings.AlbumArtSetting.albumArtWidth", L"32 64", g_settings.albumArtMinWidth, g_settings.albumArtMaxWidth);
    ParseMargin(L"MainSettings.AlbumArtSetting.albumArtHeight", L"32 32", g_settings.albumArtMinHeight, g_settings.albumArtMaxHeight);
    ParseMargin(L"MainSettings.AlbumArtSetting.albumArtMargin", L"0 0", g_settings.albumArtLeftMargin, g_settings.albumArtRightMargin);
    ParseMargin(L"MainSettings.TextAreaSetting.textAreaWidth", L"0 120", g_settings.textAreaMinWidth, g_settings.textAreaMaxWidth);
    ParseMargin(L"MainSettings.TextAreaSetting.textAreaHeight", L"0 0", g_settings.textAreaMinHeight, g_settings.textAreaMaxHeight);
    ParseMargin(L"MainSettings.TextAreaSetting.textAreaMargin", L"5 5", g_settings.textAreaLeftMargin, g_settings.textAreaRightMargin);
    g_settings.mirrorLayout         = Wh_GetIntSetting(L"MainSettings.PlayerSetting.mirrorLayout") != 0;
    g_settings.fullHeightHitArea    = Wh_GetIntSetting(L"MainSettings.PlayerSetting.fullHeightHitArea") != 0;
    g_settings.autoSwitchSession    = Wh_GetIntSetting(L"MainSettings.PlayerSetting.autoSwitchSession") != 0;
    g_settings.showMediaButtons     = Wh_GetIntSetting(L"MainSettings.MediaButtonsSettings.showMediaButtons") != 0;
    ParseMargin(L"MainSettings.MediaButtonsSettings.mediaButtonsMargin", L"2 2", g_settings.mediaButtonsLeftMargin, g_settings.mediaButtonsRightMargin);
    g_settings.showTrackTitle       = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.showTrackTitle")    != 0;
    g_settings.showFullTitleOnHover = Wh_GetIntSetting(L"BehaviorSettings.showFullTitleOnHover") != 0;
    g_settings.showTrackArtist      = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.showTrackArtist")   != 0;
    g_settings.swapTitleArtist      = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.swapTitleArtist")   != 0;
    g_settings.emptyTitleText       = StrAllowEmpty(L"MainSettings.TextAreaSetting.emptyTitleText");
    g_settings.noMediaTitleText     = StrAllowEmpty(L"MainSettings.TextAreaSetting.noMediaTitleText");
    g_settings.emptyArtistText      = StrAllowEmpty(L"MainSettings.TextAreaSetting.emptyArtistText");
    g_settings.noMediaArtistText    = StrAllowEmpty(L"MainSettings.TextAreaSetting.noMediaArtistText");
    g_settings.showAlbumArt         = Wh_GetIntSetting(L"MainSettings.AlbumArtSetting.showAlbumArt")      != 0;
    g_settings.albumArtEmptyBehavior = Str(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtEmptyBehavior", L"show");
    g_settings.emptyIconGlyph       = Str(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconGlyph",       L"E189");
    g_settings.emptyIconSize        = Int(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconSize",          1, 256, 16);
    g_settings.emptyIconFont        = Str(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconFont",        L"segoe_fluent");
    g_settings.emptyIconColor       = Str(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconColor",       L"255 255 255");
    g_settings.emptyIconOpacity     = Int(L"AppearanceSettings.AlbumArtDisplaySettings.emptyIconOpacity",       0, 100, 100);
    g_settings.albumArtQuality      = Str(L"AppearanceSettings.AlbumArtDisplaySettings.albumArtQuality", L"medium");
    g_settings.albumArtPlaybackButton = Wh_GetIntSetting(L"MainSettings.AlbumArtSetting.albumArtPlaybackButton") != 0;
    g_settings.showPauseOverlay     = Wh_GetIntSetting(L"AppearanceSettings.AlbumArtDisplaySettings.showPauseOverlay")  != 0;
    g_settings.pauseOverlayIconSize = Int(L"AppearanceSettings.AlbumArtDisplaySettings.pauseOverlayIconSize",     1, 256, 16);
    g_settings.pauseOverlayOpacity  = Int(L"AppearanceSettings.AlbumArtDisplaySettings.pauseOverlayOpacity",     0, 100,  60);
    g_settings.iconStyle            = Str(L"AppearanceSettings.MediaButtonsStyleSettings.iconStyle", L"fluent_outline");
    g_settings.showAppIcon          = Wh_GetIntSetting(L"AppearanceSettings.AlbumArtDisplaySettings.showAppIcon")       != 0;
    g_settings.appIconCorner        = Str(L"AppearanceSettings.AlbumArtDisplaySettings.appIconCorner",  L"bottom_right");
    g_settings.appIconSize          = Int(L"AppearanceSettings.AlbumArtDisplaySettings.appIconSize",         8,  32,  12);
    g_settings.backgroundType       = Str(L"AppearanceSettings.BackgroundStyleSettings.backgroundType", L"none");
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
    g_settings.scrollMode           = Str(L"MainSettings.TextAreaSetting.scrollMode", L"bounce");
    g_settings.loopGap              = Int(L"MainSettings.TextAreaSetting.loopGap", 0, 500, 40);
    g_settings.solidColor           = Str(L"AppearanceSettings.BackgroundStyleSettings.solidColor", L"35 35 35");
    g_settings.solidColor2          = Str(L"AppearanceSettings.BackgroundStyleSettings.solidColor2", L"35 35 35");
    g_settings.gradientColor2       = Str(L"AppearanceSettings.BackgroundStyleSettings.gradientColor2", L"128 128 128");
    g_settings.solidOpacity         = Int(L"AppearanceSettings.BackgroundStyleSettings.solidOpacity", 0, 100, 100);
    g_settings.gradientAngle        = Int(L"AppearanceSettings.BackgroundStyleSettings.gradientAngle", 0, 360, 50);
    g_settings.gradientBalance      = Int(L"AppearanceSettings.BackgroundStyleSettings.gradientBalance", 0, 100, 50);
    g_settings.acrylicTintOpacity   = Int(L"AppearanceSettings.BackgroundStyleSettings.acrylicTintOpacity", 0, 100, 50);
    g_settings.micaOpacity          = Int(L"AppearanceSettings.BackgroundStyleSettings.micaOpacity", 0, 100, 50);
    g_settings.buttonColor          = Str(L"AppearanceSettings.MediaButtonsStyleSettings.buttonColor", L"255 255 255");
    g_settings.buttonColorOpacity   = Int(L"AppearanceSettings.MediaButtonsStyleSettings.buttonColorOpacity", 0, 100, 100);
    g_settings.titleColor           = Str(L"AppearanceSettings.TitleTextStyleSettings.titleColor", L"255 255 255");
    g_settings.titleColorOpacity    = Int(L"AppearanceSettings.TitleTextStyleSettings.titleColorOpacity", 0, 100, 100);
    g_settings.artistColor          = Str(L"AppearanceSettings.ArtistTextStyleSettings.artistColor", L"255 255 255");
    g_settings.artistColorOpacity   = Int(L"AppearanceSettings.ArtistTextStyleSettings.artistColorOpacity", 0, 100, 80);
    g_settings.vizEnabled      = Wh_GetIntSetting(L"MainSettings.VisualizerFunctionsSettings.vizEnabled") != 0;
    g_settings.vizPosition     = Str(L"MainSettings.VisualizerFunctionsSettings.vizPosition", L"right");
    g_settings.vizHorizontalOffset = Int(L"MainSettings.VisualizerFunctionsSettings.vizHorizontalOffset", -100, 100, 0);
    g_settings.vizVerticalOffset = Int(L"MainSettings.VisualizerFunctionsSettings.vizVerticalOffset", -20, 20, 0);
    g_settings.vizSectionGap = Int(L"MainSettings.VisualizerFunctionsSettings.vizSectionGap", 0, 40, 6);
    g_settings.vizCompactEnabled = Wh_GetIntSetting(L"MainSettings.VisualizerFunctionsSettings.vizCompactEnabled") != 0;
    g_settings.vizCompactGap = Int(L"MainSettings.VisualizerFunctionsSettings.vizCompactGap", 0, 80, 0);
    g_settings.vizCompactHorizontalOffset = Int(L"MainSettings.VisualizerFunctionsSettings.vizCompactHorizontalOffset", -100, 100, 0);
    g_settings.vizCompactVerticalOffset = Int(L"MainSettings.VisualizerFunctionsSettings.vizCompactVerticalOffset", -20, 20, 0);
    g_settings.vizMoveAnimationMs = Int(L"MainSettings.VisualizerFunctionsSettings.vizMoveAnimationMs", 0, 1600, 350);
    g_settings.vizTransitionStyle = Str(L"MainSettings.VisualizerFunctionsSettings.vizTransitionStyle", L"slide");
    if (g_settings.vizTransitionStyle != L"slide" && g_settings.vizTransitionStyle != L"fade" &&
        g_settings.vizTransitionStyle != L"instant") g_settings.vizTransitionStyle = L"slide";
    auto validVizEase = [](std::wstring const& v) {
        return v == L"cinematic" || v == L"silky" || v == L"gentle" ||
               v == L"dramatic" || v == L"powerful" || v == L"arc" ||
               v == L"quick" || v == L"feather" || v == L"glide" ||
               v == L"buildup" || v == L"linear";
    };
    g_settings.vizNearToFarEase = Str(L"MainSettings.VisualizerFunctionsSettings.vizNearToFarEase", L"cinematic");
    if (!validVizEase(g_settings.vizNearToFarEase)) g_settings.vizNearToFarEase = L"cinematic";
    g_settings.vizFarToNearEase = Str(L"MainSettings.VisualizerFunctionsSettings.vizFarToNearEase", L"match");
    if (g_settings.vizFarToNearEase != L"match" && !validVizEase(g_settings.vizFarToNearEase))
        g_settings.vizFarToNearEase = L"match";
    g_settings.vizBeforeLyricsMs = Int(L"MainSettings.VisualizerFunctionsSettings.vizBeforeLyricsMs", 0, 1500, 500);
    g_settings.vizInstrumentalAfterMs = Int(L"MainSettings.VisualizerFunctionsSettings.vizInstrumentalAfterMs", 500, 30000, 5200);
    g_settings.vizPreviewMode = Str(L"MainSettings.VisualizerFunctionsSettings.vizPreviewMode", L"off");
    if (g_settings.vizPreviewMode != L"near" && g_settings.vizPreviewMode != L"far" &&
        g_settings.vizPreviewMode != L"cycle") g_settings.vizPreviewMode = L"off";
    g_settings.vizPreviewStrength = Int(L"MainSettings.VisualizerFunctionsSettings.vizPreviewStrength", 10, 100, 70);
    g_settings.vizColor        = Str(L"AppearanceSettings.VisualizerStyleSettings.vizColor",  L"255 255 255");
    g_settings.vizColor1       = Str(L"AppearanceSettings.VisualizerStyleSettings.vizColor1", L"30 215 96");
    g_settings.vizColor2       = Str(L"AppearanceSettings.VisualizerStyleSettings.vizColor2", L"0 180 255");
    g_settings.vizSensitivity  = Int(L"MainSettings.VisualizerFunctionsSettings.vizSensitivity", 0, 300, 150);
    g_settings.vizRoundness = Int(L"MainSettings.VisualizerFunctionsSettings.vizRoundness", 0, 100, 100);
    g_settings.vizResponseMode = Str(L"MainSettings.VisualizerFunctionsSettings.vizResponseMode", L"precise");
    if (g_settings.vizResponseMode != L"precise" &&
        g_settings.vizResponseMode != L"balanced" &&
        g_settings.vizResponseMode != L"smooth") g_settings.vizResponseMode = L"precise";
    g_settings.vizFrameRate = Str(L"MainSettings.VisualizerFunctionsSettings.vizFrameRate", L"60");
    if (g_settings.vizFrameRate != L"30" && g_settings.vizFrameRate != L"60")
        g_settings.vizFrameRate = L"60";
    {
        int n = 7, gap = 5;
        ParseTwoInts(Str(L"MainSettings.VisualizerFunctionsSettings.vizBarCountGap", L"7 5"), n, gap);
        g_settings.vizBars   = std::clamp(n, 1, 20);
        g_settings.vizBarGap = std::clamp(gap, 0, 40);
        int w = 5, h = 3;
        ParseTwoInts(Str(L"MainSettings.VisualizerFunctionsSettings.vizBarSize", L"5 3"), w, h);
        g_settings.vizBarWidth    = std::clamp(w, 0, 40);
        g_settings.vizIdleBarSize = std::clamp(h, 0, 15);
        g_settings.vizRestingHeight = Int(
            L"MainSettings.VisualizerFunctionsSettings.vizRestingHeight", -1, 60, -1);
        int l = 0, r = 0;
        ParseTwoInts(Str(L"MainSettings.VisualizerFunctionsSettings.vizPadding", L"0 0"), l, r);
        g_settings.vizPadLeft  = std::clamp(l, 0, 200);
        g_settings.vizPadRight = std::clamp(r, 0, 200);
    }
    {
        std::wstring shape = Str(L"MainSettings.VisualizerFunctionsSettings.vizShape", L"stereo");
        g_settings.vizShape = (shape == L"mountain") ? VizShape::Mountain
                    : (shape == L"mirror")   ? VizShape::Mirror
                    : (shape == L"wave")     ? VizShape::Wave
                    : (shape == L"breathe")  ? VizShape::Breathe
                                            : VizShape::Stereo;
        std::wstring mode = Str(L"AppearanceSettings.VisualizerStyleSettings.vizColorMode", L"solid");
        g_settings.vizColorMode = (mode == L"dynamic_album")    ? VizColorMode::DynamicAlbum
                        : (mode == L"dynamic_gradient") ? VizColorMode::DynamicGradient
                        : (mode == L"custom_gradient")  ? VizColorMode::CustomGradient
                        : (mode == L"acrylic")          ? VizColorMode::Acrylic
                                                        : VizColorMode::Solid;
        std::wstring eq = Str(L"MainSettings.VisualizerFunctionsSettings.vizEQ", L"default");
        g_settings.vizEq = (eq == L"bass")       ? VizEQ::Bass
                : (eq == L"rock")       ? VizEQ::Rock
                : (eq == L"pop")        ? VizEQ::Pop
                : (eq == L"jazz")       ? VizEQ::Jazz
                : (eq == L"electronic") ? VizEQ::Electronic
                                        : VizEQ::Default;
        std::wstring anchor = Str(L"MainSettings.VisualizerFunctionsSettings.vizAnchor", L"middle");
        g_settings.vizAnchor = (anchor == L"top")    ? VizAnchor::Top
                    : (anchor == L"bottom") ? VizAnchor::Bottom
                                            : VizAnchor::Middle;
    }
    g_settings.albumArtLeftClick         = L"none";
    g_settings.albumArtRightClick        = L"none";
    g_settings.albumArtMiddleClick       = L"none";
    g_settings.albumArtLeftDoubleClick   = L"none";
    g_settings.albumArtRightDoubleClick  = L"none";
    g_settings.albumArtMiddleDoubleClick = L"none";
    g_settings.playerLeftClick           = L"none";
    g_settings.playerRightClick          = L"none";
    g_settings.playerMiddleClick         = L"none";
    g_settings.playerLeftDoubleClick     = L"none";
    g_settings.playerRightDoubleClick    = L"none";
    g_settings.playerMiddleDoubleClick   = L"none";
    for (int i = 0; i < 20; i++) {
        PCWSTR objectStr = Wh_GetStringSetting(L"BehaviorSettings.ClickActionSettings[%d].object", i);
        PCWSTR clickStr = Wh_GetStringSetting(L"BehaviorSettings.ClickActionSettings[%d].click", i);
        PCWSTR actionStr = Wh_GetStringSetting(L"BehaviorSettings.ClickActionSettings[%d].action", i);
        if (*objectStr == L'\0' || *clickStr == L'\0' || *actionStr == L'\0') {
            Wh_FreeStringSetting(objectStr);
            Wh_FreeStringSetting(clickStr);
            Wh_FreeStringSetting(actionStr);
            break;
        }
        std::wstring object(objectStr);
        std::wstring click(clickStr);
        std::wstring action(actionStr);
        Wh_FreeStringSetting(objectStr);
        Wh_FreeStringSetting(clickStr);
        Wh_FreeStringSetting(actionStr);
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
            else if (click == L"right_double_click") g_settings.albumArtRightDoubleClick = action;
            else if (click == L"middle_double_click") g_settings.albumArtMiddleDoubleClick = action;
        } else if (object == L"player") {
            if (click == L"left_click") g_settings.playerLeftClick = action;
            else if (click == L"right_click") g_settings.playerRightClick = action;
            else if (click == L"middle_click") g_settings.playerMiddleClick = action;
            else if (click == L"left_double_click") g_settings.playerLeftDoubleClick = action;
            else if (click == L"right_double_click") g_settings.playerRightDoubleClick = action;
            else if (click == L"middle_double_click") g_settings.playerMiddleDoubleClick = action;
        }
    }
    g_settings.albumArtWheelAction = L"none";
    g_settings.playerWheelAction   = L"none";
    for (int i = 0; i < 20; i++) {
        PCWSTR objectStr = Wh_GetStringSetting(L"BehaviorSettings.MouseWheelActionSettings[%d].object", i);
        PCWSTR clickStr = Wh_GetStringSetting(L"BehaviorSettings.MouseWheelActionSettings[%d].click", i);
        PCWSTR actionStr = Wh_GetStringSetting(L"BehaviorSettings.MouseWheelActionSettings[%d].action", i);
        if (*objectStr == L'\0' || *clickStr == L'\0' || *actionStr == L'\0') {
            Wh_FreeStringSetting(objectStr);
            Wh_FreeStringSetting(clickStr);
            Wh_FreeStringSetting(actionStr);
            break;
        }
        std::wstring object(objectStr);
        std::wstring click(clickStr);
        std::wstring action(actionStr);
        Wh_FreeStringSetting(objectStr);
        Wh_FreeStringSetting(clickStr);
        Wh_FreeStringSetting(actionStr);
        if (object.empty()) object = L"none";
        if (click.empty()) click = L"none";
        if (action.empty()) action = L"none";
        if (object == L"none" || click == L"none") {
            continue;
        }
        if (object == L"album_art" && click == L"mouse_wheel") {
            g_settings.albumArtWheelAction = action;
        } else if (object == L"player" && click == L"mouse_wheel") {
            g_settings.playerWheelAction = action;
        }
    }
    g_settings.hideWhenNoMedia      = Wh_GetIntSetting(L"BehaviorSettings.hideWhenNoMedia")   != 0;
    g_settings.hideFullscreen       = Wh_GetIntSetting(L"BehaviorSettings.hideFullscreen")    != 0;
    g_settings.allowInFullscreen    = Wh_GetIntSetting(L"BehaviorSettings.allowInFullscreen") != 0;
    g_settings.idleHideSeconds      = std::max(Wh_GetIntSetting(L"BehaviorSettings.idleHideSeconds"), 0);
    g_settings.playerHoverEffectMode = HoverMode(L"AppearanceSettings.BackgroundStyleSettings.enablePlayerHoverEffect");
    g_settings.mediaButtonsHoverEffectMode = HoverMode(L"AppearanceSettings.BackgroundStyleSettings.enableMediaButtonsHoverEffect");
    g_settings.enableHoverAnimation = Wh_GetIntSetting(L"AppearanceSettings.BackgroundStyleSettings.enableHoverAnimation") != 0;
    g_settings.enableSmoothPositionAnimation = Wh_GetIntSetting(L"AnimationSettings.enableSmoothPositionAnimation") != 0;
    g_settings.showSuccessNotification = Wh_GetIntSetting(L"NotificationSettings.showSuccessNotification") != 0;
    g_settings.hideUnsupportedButtons  = Wh_GetIntSetting(L"MainSettings.MediaButtonsSettings.hideUnsupportedButtons") != 0;
    g_settings.disableAlbumArtClick    = Wh_GetIntSetting(L"BehaviorSettings.disableAlbumArtClick") != 0;
    g_settings.ignoredProcesses     = Str(L"DebugSettings.ignoredProcesses", L"");
    g_settings.enableTreeDump       = Wh_GetIntSetting(L"DebugSettings.enableTreeDump")    != 0;
    g_settings.showDebugBorders     = Wh_GetIntSetting(L"DebugSettings.showDebugBorders")  != 0;
    g_settings.showMiniPlayerBorder = Wh_GetIntSetting(L"DebugSettings.showMiniPlayerBorder") != 0;
    g_settings.showLayoutAnchors    = Wh_GetIntSetting(L"DebugSettings.showLayoutAnchors") != 0;
    g_settings.keepMiniPlayerOpen   = Wh_GetIntSetting(L"PlayerMenuSettings.keepMiniPlayerOpen") != 0;
    g_settings.hideMediaSessionsList = Wh_GetIntSetting(L"PlayerMenuSettings.hideMediaSessionsList") != 0;
    g_settings.miniPlayerPlacementMode = Str(L"PlayerMenuSettings.placementMode", L"screen");
    g_settings.miniPlayerHorizontalOffsetAbove =
        Wh_GetIntSetting(L"PlayerMenuSettings.PlayerMenuSettingsNear.miniPlayerHorizontalOffsetNear");
    g_settings.miniPlayerVerticalPlacementNear =
        Str(L"PlayerMenuSettings.PlayerMenuSettingsNear.miniPlayerVerticalPlacementNear", L"top");
    g_settings.miniPlayerHorizontalPlacement =
        Str(L"PlayerMenuSettings.PlayerMenuSettingsScreen.miniPlayerHorizontalPlacement", L"right");
    g_settings.miniPlayerHorizontalDistanceFromScreenEdge =
        Wh_GetIntSetting(L"PlayerMenuSettings.PlayerMenuSettingsScreen.horizontalDistanceFromScreenEdge");
    g_settings.miniPlayerVerticalPlacement =
        Str(L"PlayerMenuSettings.PlayerMenuSettingsScreen.miniPlayerVerticalPlacement", L"bottom");
    g_settings.miniPlayerVerticalDistanceFromScreenEdge =
        Wh_GetIntSetting(L"PlayerMenuSettings.PlayerMenuSettingsScreen.verticalDistanceFromScreenEdge");
    g_settings.miniPlayerAnimation =
        Str(L"PlayerMenuSettings.PlayerMenuSettingsScreen.miniPlayerAnimation", L"auto");
    g_settings.showRestartButton    = Wh_GetIntSetting(L"DebugSettings.showRestartButton") != 0;
    g_settings.contextMenuRepeatStyle  = Str(L"ContextMenuSettings.repeatStyle",  L"submenu");
    g_settings.contextMenuShuffleStyle = Str(L"ContextMenuSettings.shuffleStyle", L"toggle");
    g_settings.showOpenWindhawk        = Wh_GetIntSetting(L"ContextMenuSettings.showOpenWindhawk") != 0;
    g_settings.contextMenuIconStyle    = Str(L"ContextMenuSettings.contextMenuIconStyle", L"as_media_buttons");
    g_settings.contextMenuIconColor    = StrAllowEmpty(L"ContextMenuSettings.contextMenuIconColor");
    g_settings.contextMenuIconOpacity  = Int(L"ContextMenuSettings.contextMenuIconOpacity", 0, 100, 100);
    {
        g_settings.contextMenuItems.clear();
        const wchar_t* defaultItems[] = {
            L"repeat", L"shuffle", L"forward", L"rewind",
            L"next", L"prev", L"switch_sessions", L"open_app"
        };
        bool hasAny = false;
        std::set<std::wstring> seen;
        for (int i = 0; i < 20; i++) {
            PCWSTR p = Wh_GetStringSetting(L"ContextMenuSettings.contextMenuItems[%d]", i);
            if (!p || !*p) { Wh_FreeStringSetting(p); break; }
            std::wstring s(p);
            Wh_FreeStringSetting(p);
            if (seen.insert(s).second) {
                g_settings.contextMenuItems.push_back(s);
                hasAny = true;
            }
        }
        if (!hasAny) {
            for (auto& d : defaultItems)
                g_settings.contextMenuItems.push_back(d);
        }
    }
    try {
        std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
        g_mediaButtons.clear();
        std::set<MediaButtonType> seen;
        for (int i = 0; i < 10; i++) {
            try {
                PCWSTR itemStr = Wh_GetStringSetting(L"MainSettings.MediaButtonsSettings.mediaButtons[%d]", i);
                if (!itemStr || !*itemStr) {
                    Wh_FreeStringSetting(itemStr);
                    break;
                }
                std::wstring keyword(itemStr);
                Wh_FreeStringSetting(itemStr);
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
    } catch (...) {
        Wh_Log(L"LoadSettings: Critical exception in media buttons parsing, using defaults");
        try {
            std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
            g_mediaButtons = {
                {MediaButtonType::Previous, 1},
                {MediaButtonType::PlayPause, 2},
                {MediaButtonType::Next, 3}
            };
        } catch (...) {}
    }
    // Normalize persisted/hand-edited sizes before assigning XAML constraints.
    auto normalizeSize = [](int& minimum, int& maximum, int limit) {
        minimum = std::clamp(minimum, 0, limit);
        maximum = std::clamp(maximum, 0, limit);
        if (maximum > 0) minimum = std::min(minimum, maximum);
    };
    if (g_settings.fixedWidth > 0) g_settings.playerMinWidth = g_settings.playerMaxWidth = g_settings.fixedWidth;
    if (g_settings.fixedHeight > 0) g_settings.playerMinHeight = g_settings.playerMaxHeight = g_settings.fixedHeight;
    if (g_settings.albumArtSize > 0) {
        g_settings.albumArtMinWidth = g_settings.albumArtMaxWidth = g_settings.albumArtSize;
        g_settings.albumArtMinHeight = g_settings.albumArtMaxHeight = g_settings.albumArtSize;
    }
    normalizeSize(g_settings.playerMinWidth, g_settings.playerMaxWidth, 1600);
    normalizeSize(g_settings.playerMinHeight, g_settings.playerMaxHeight, 120);
    normalizeSize(g_settings.albumArtMinWidth, g_settings.albumArtMaxWidth, 256);
    normalizeSize(g_settings.albumArtMinHeight, g_settings.albumArtMaxHeight, 120);
    normalizeSize(g_settings.textAreaMinWidth, g_settings.textAreaMaxWidth, 1200);
    normalizeSize(g_settings.textAreaMinHeight, g_settings.textAreaMaxHeight, 120);
    ApplyCompactLayout(g_settings);
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
}
static HWND FindCurrentProcessTaskbarWnd();
static void DispatchMediaUpdate();
static void RefreshMiniPlayerFlyoutUI();
static void FetchMiniSessionInfosAsync(HWND taskbarWnd);
struct MiniSessionInfo;
static void AnimateSessionPill(winrt::Windows::UI::Xaml::Shapes::Rectangle const& pill, bool isCurrent, bool animate);
static winrt::Windows::UI::Xaml::Controls::Button BuildSessionRowButton(const MiniSessionInfo& info, bool isCurrent, HWND taskbarWnd, int rowIndex);

static void ApplySettings();
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_applyingSettings{false};
static HWND g_taskbarWnd = nullptr;
static HANDLE g_timerUpdateEvent = nullptr;
static std::atomic<bool> g_onBattery{false};
static bool ReduceMotionForPower() { return g_settings.batterySaver && g_onBattery.load(); }
static std::atomic<bool> g_needsLyricsUiUpdate{false};
static std::atomic<bool> g_timelineDirty{true};
static void DispatchLyricsUpdate() {
    if (!g_unloading && !g_applyingSettings) g_needsLyricsUiUpdate = true;
}


static std::atomic<int> g_activeWorkerThreads{0};
static std::mutex g_workerAdmissionMutex;
static std::mutex g_settingsChangeMutex;
template <typename F>
static void SpawnTrackedWorker(F&& fn) {
    std::lock_guard<std::mutex> admission(g_workerAdmissionMutex);
    if (g_unloading || g_applyingSettings) return;
    g_activeWorkerThreads.fetch_add(1, std::memory_order_relaxed);
    using Work = std::decay_t<F>;
    try { std::thread([work = std::optional<Work>(std::in_place, std::forward<F>(fn))]() mutable {
        struct Decrementer {
            ~Decrementer() { g_activeWorkerThreads.fetch_sub(1, std::memory_order_release); }
        } dec;
        try { (*work)(); } catch (...) { Wh_Log(L"Media worker failed"); }
        // Destroy captured COM/UI resources before reporting that this job drained.
        work.reset();
    }).detach();
    } catch (...) { g_activeWorkerThreads.fetch_sub(1, std::memory_order_relaxed); Wh_Log(L"Cannot create media worker"); }
}

static void WaitForThreadToExit(HANDLE thread) {
    const bool ui = g_taskbarWnd && GetWindowThreadProcessId(g_taskbarWnd, nullptr) == GetCurrentThreadId();
    for (;;) {
        const DWORD result = ui ? MsgWaitForMultipleObjects(1, &thread, FALSE, 50, QS_SENDMESSAGE)
                                : WaitForSingleObject(thread, 50);
        if (result == WAIT_OBJECT_0) return;
        if (result == WAIT_FAILED) { Wh_Log(L"Thread wait failed: %lu", GetLastError()); return; }
        if (ui) {
            MSG msg;
            // PeekMessage dispatches pending sent calls; only drain that queue.
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE)) {
                TranslateMessage(&msg); DispatchMessageW(&msg);
            }
        }
    }
}
static void WaitForTrackedWorkers() {
    HWND hTaskbar = g_taskbarWnd;
    DWORD tid = GetCurrentThreadId();
    bool isUiThread = hTaskbar && (GetWindowThreadProcessId(hTaskbar, nullptr) == tid);
    if (isUiThread) {
        while (g_activeWorkerThreads.load(std::memory_order_acquire) > 0) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            Sleep(10);
        }
    } else {
        while (g_activeWorkerThreads.load(std::memory_order_acquire) > 0) {
            Sleep(10);
        }
    }
}

static std::atomic<bool> g_miniPlayerFlyoutOpen{false};
static int    g_miniPlayerAnimAxis = 0;
static double g_miniPlayerAnimSign = 1.0;
static std::atomic<bool> g_miniPlayerClosingAnimInProgress{false};
static std::atomic<bool> g_miniPlayerClosingAnimStarted{false};
static std::atomic<bool> g_miniPlayerExplicitCloseRequested{false};
static IMMDeviceEnumerator* g_pDeviceEnumerator = nullptr;
static const CLSID XIID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID XIID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID XIID_IAudioSessionManager2 = __uuidof(IAudioSessionManager2);
[[clang::no_destroy]] static Grid             g_playerGrid      = nullptr;
[[clang::no_destroy]] static FrameworkElement g_injectionParent = nullptr;
[[clang::no_destroy]] static XamlRoot g_cachedTaskbarXamlRoot{nullptr};
static HWND g_cachedTaskbarRootWindow = nullptr;
static int              g_playerColumn    = -1;
[[clang::no_destroy]] static std::function<void()> g_playerButtonStateUpdater;
[[clang::no_destroy]] static ToolTip          g_playerToolTip   = nullptr;
static std::atomic<bool> g_needsUiUpdate{false};
static std::atomic<bool> g_resetScrollPending{false};
static void ShowSuccessNotification() {
    if (g_settings.showSuccessNotification) Wh_Log(L"Taskbar media player initialized successfully");
}
// The retry timer must be canceled on the XAML thread before module unload.
[[clang::no_destroy]] static DispatcherTimer g_startupRetryTimer{nullptr};
static winrt::event_token g_startupRetryToken{};
static uint64_t g_startupRetryGeneration = 0;
static void CancelStartupRetry() {
    ++g_startupRetryGeneration;
    if (g_startupRetryTimer) {
        try { g_startupRetryTimer.Stop(); } catch (...) {}
        if (g_startupRetryToken.value) {
            try { g_startupRetryTimer.Tick(g_startupRetryToken); } catch (...) {}
        }
        g_startupRetryTimer = nullptr;
    }
    g_startupRetryToken = {};
}

[[clang::no_destroy]] static FrameworkElement g_trackedElement = nullptr;
static Thickness g_trackedElementOriginalMargin{};
static bool g_hasTrackedElementOriginalMargin = false;
static std::wstring g_trackPosition = L"";
static winrt::event_token g_layoutUpdateToken{};
static constexpr wchar_t kLyricsOverlayHostName[] = L"FluentLyrics_ZeroMeasureOverlay";
[[clang::no_destroy]] static Canvas g_lyricsOverlayHost = nullptr;
[[clang::no_destroy]] static Grid g_lyricsViewport = nullptr;
[[clang::no_destroy]] static RectangleGeometry g_lyricsGuardClip = nullptr;
[[clang::no_destroy]] static Storyboard g_lyricsWidthStoryboard = nullptr;
[[clang::no_destroy]] static Storyboard g_lyricsOpacityStoryboard = nullptr;
static double g_lyricsTargetWidth = -1.0;
static double g_playerOverlayWidth = 0.0;
// Normal visualizer column belongs to the full player grid; no UI refs survive reinjection.
static int g_vizNormalColumn = -1;
static bool g_compactNoLyricsActive = false;
// UI-thread-only movement storyboard is stopped before destroying the player.
[[clang::no_destroy]] static Storyboard g_vizDockStoryboard = nullptr;
// Preview alternates at a steady cadence; never depends on an online lyric lookup.
static const auto g_vizPreviewEpoch = std::chrono::steady_clock::now();
static bool g_albumArtworkHovered = false;
static bool g_albumArtworkPlaying = false;
static bool g_lyricsTargetVisible = true;
static std::chrono::steady_clock::time_point g_lyricsLastLayoutTime{};
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
static int g_windowDispatchIdentity;
static UINT WindowDispatchMessage() {
    static const UINT message = [] {
        std::wstring name = L"Windhawk_MediaUiDispatch_" WH_MOD_ID L"_";
        name += std::to_wstring(reinterpret_cast<uintptr_t>(&g_windowDispatchIdentity));
        return RegisterWindowMessageW(name.c_str());
    }();
    return message;
}
static bool InvokeWindowThreadProc(WindowThreadProc proc, void* param) noexcept {
    try {
        proc(param);
        return true;
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"Taskbar UI callback failed: HRESULT 0x%08X: %s", static_cast<unsigned>(error.code()), error.message().c_str());
    } catch (...) {
        Wh_Log(L"Taskbar UI callback failed; exception contained before returning to Explorer");
    }
    return false;
}
static bool RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param) {
    if (!hWnd || !proc) return false;
    const UINT message = WindowDispatchMessage();
    if (!message) return false;
    struct Payload { WindowThreadProc proc; void* param; bool succeeded; };
    const DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) return InvokeWindowThreadProc(proc, param);
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM w, LPARAM l) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* call = reinterpret_cast<const CWPSTRUCT*>(l);
                if (call->message == WindowDispatchMessage() &&
                    call->wParam == reinterpret_cast<WPARAM>(&g_windowDispatchIdentity)) {
                    auto* payload = reinterpret_cast<Payload*>(call->lParam);
                    payload->succeeded = InvokeWindowThreadProc(payload->proc, payload->param);
                }
            }
            return CallNextHookEx(nullptr, code, w, l);
        }, nullptr, tid);
    if (!hook) { Wh_Log(L"Cannot install taskbar UI dispatch hook: %lu", GetLastError()); return false; }
    Payload payload{proc, param, false};
    SendMessageW(hWnd, message, reinterpret_cast<WPARAM>(&g_windowDispatchIdentity), reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hook);
    return payload.succeeded;
}
struct MediaState {
    std::wstring      title;
    std::wstring      artist;
    std::wstring      appUserModelId;
    bool              isPlaying     = false;
    bool              hasMedia      = false;
    std::vector<BYTE> thumbnailBytes;
    uint64_t          thumbnailHash = 0;
    uint64_t          thumbnailStreamSize = 0;
    std::vector<BYTE> appIconBytes;
    std::wstring      appIconKey;
    bool              canPlayPause     = false;
    bool              canSkipPrevious  = true;
    bool              canSkipNext      = true;
    bool              canShuffle       = true;
    bool              canRepeat        = true;
    bool              canSeek          = true;
};
static MediaState g_media;
static std::mutex g_mediaMtx;

// Shared media session is used by both the original player and the lyrics engine.
// Define these globals before UpdateLyricsPositionFromSession() to prevent
// undeclared-identifier errors in the Windhawk C++ compiler.
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSession g_currentSession = nullptr;
static std::mutex g_sessionMtx;

// ---- Time-synchronized lyrics engine (LRCLIB; playback timeline is from GSMTC) ----
struct LyricCue { int64_t timeMs = 0; std::wstring text; };
struct LyricRecord { std::vector<LyricCue> cues; bool unsynced = false; };
struct LyricLookupResult { std::vector<LyricCue> cues; bool unsynced = false; bool networkError = false; };
static std::mutex g_lyricsMutex;
static std::wstring g_lyricsKey;
static std::wstring g_lyricsRequestedTitle, g_lyricsRequestedArtist, g_lyricsRequestedAlbum;
static int g_lyricsRequestedDuration = 0;
static std::vector<LyricCue> g_lyricsCues;
static bool g_lyricsUnsynced = false;
// Cache only successful lookups; unavailable songs can appear in the catalog later.
static std::vector<std::pair<std::wstring, LyricRecord>> g_lyricsCache;
static uint64_t g_lyricsGeneration = 0;
static int g_lyricsCurrentIndex = -1;
static int g_lyricsDisplayIndex = -1; // -1 during instrumental/silent-lyric intervals
static bool g_lyricsFarDueSoon = false; // true during configured pre-cue lead-in
static bool g_lyricsWorkerActive = false;
static bool g_lyricsPending = false;
static bool g_lyricsNeedRetry = false;
static int g_lyricsFailureCount = 0;
static std::chrono::steady_clock::time_point g_lyricsNextRetry{};
static int64_t g_lyricsLastPositionMs = -1;

static std::string LyricsToUtf8(const std::wstring& text) {
    if (text.empty()) return {};
    int count = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    if (count <= 0) return {};
    std::string out((size_t)count, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), out.data(), count, nullptr, nullptr);
    return out;
}
static std::wstring LyricsFromUtf8(const std::string& input) {
    if (input.empty()) return {};
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(), (int)input.size(), nullptr, 0);
    if (count <= 0) return {};
    std::wstring out((size_t)count, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(), (int)input.size(), out.data(), count);
    return out;
}
static std::wstring LyricsUrlEncode(const std::wstring& text) {
    static constexpr wchar_t digits[] = L"0123456789ABCDEF";
    std::wstring encoded;
    for (unsigned char c : LyricsToUtf8(text)) {
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded.push_back((wchar_t)c);
        } else {
            encoded += L'%';
            encoded += digits[c >> 4];
            encoded += digits[c & 15];
        }
    }
    return encoded;
}
// Enhanced LRC can contain <mm:ss.mmm> markers before individual words.
// Parse only valid timing tags: never delete ordinary angle-bracketed text.
// This line-based renderer uses the markers for cleanup, not word-level UI.
static bool ParseEnhancedLrcTimeTag(const std::wstring& line, size_t at,
                                    size_t& after, int64_t* timeMs = nullptr) {
    if (at >= line.size() || line[at] != L'<') return false;
    size_t p = at + 1;
    int minutes = 0;
    int mDigits = 0;
    while (p < line.size() && line[p] >= L'0' && line[p] <= L'9' && mDigits < 3) {
        minutes = minutes * 10 + (line[p++] - L'0');
        ++mDigits;
    }
    if (mDigits < 1 || p >= line.size() || line[p++] != L':') return false;
    if (p + 1 >= line.size() || line[p] < L'0' || line[p] > L'9' ||
        line[p + 1] < L'0' || line[p + 1] > L'9') return false;
    const int seconds = (line[p] - L'0') * 10 + (line[p + 1] - L'0');
    p += 2;
    if (seconds >= 60) return false;
    int millis = 0;
    int decimals = 0;
    if (p < line.size() && line[p] == L'.') {
        ++p;
        while (p < line.size() && line[p] >= L'0' && line[p] <= L'9' && decimals < 3) {
            millis = millis * 10 + (line[p++] - L'0');
            ++decimals;
        }
        if (decimals == 0) return false;
        while (decimals++ < 3) millis *= 10;
    }
    if (p >= line.size() || line[p] != L'>') return false;
    after = p + 1;
    if (timeMs) *timeMs = minutes * 60000LL + seconds * 1000LL + millis;
    return true;
}
static std::wstring StripEnhancedLrcTimeTags(const std::wstring& text) {
    if (text.find(L'<') == std::wstring::npos) return text;
    std::wstring result;
    result.reserve(text.size());
    for (size_t i = 0; i < text.size();) {
        size_t after = i;
        if (text[i] == L'<' && ParseEnhancedLrcTimeTag(text, i, after)) {
            i = after;
        } else {
            result.push_back(text[i++]);
        }
    }
    return result;
}
static std::vector<LyricCue> ParseLrcLines(const std::wstring& lrc) {
    std::vector<LyricCue> cues;
    size_t start = 0;
    while (start < lrc.size() && cues.size() < 4000) {
        size_t end = lrc.find_first_of(L"\r\n", start);
        if (end == std::wstring::npos) end = lrc.size();
        std::wstring line = lrc.substr(start, end - start);
        start = end + 1;
        // Multiple [mm:ss.xx] prefixes on one line are allowed in LRC.
        size_t pos = 0;
        std::vector<int64_t> timestamps;
        while (pos < line.size() && line[pos] == L'[') {
            size_t close = line.find(L']', pos + 1);
            if (close == std::wstring::npos) break;
            std::wstring stamp = line.substr(pos + 1, close - pos - 1);
            size_t colon = stamp.find(L':');
            if (colon == std::wstring::npos || colon > 3) break;
            try {
                // Reject tags like [ar:...], [ti:...], etc.
                if (stamp.empty() || stamp[0] < L'0' || stamp[0] > L'9') break;
                std::wstring m = stamp.substr(0, colon);
                std::wstring s = stamp.substr(colon + 1);
                if (s.empty()) break;
                size_t mpos = 0, spos = 0;
                int64_t min = std::stoll(m, &mpos);
                double seconds = std::stod(s, &spos);
                if (mpos == m.size() && spos == s.size() && min >= 0 && min < 1000 && seconds >= 0.0 && seconds < 60.0)
                    timestamps.push_back(min * 60000 + (int64_t)std::llround(seconds * 1000.0));
            } catch (...) { break; }
            pos = close + 1;
        }
        if (timestamps.empty()) {
            // Some enhanced-LRC files omit the outer [mm:ss.xx] line marker.
            // The first <mm:ss.mmm> word tag can still anchor the line.
            size_t first = line.find_first_not_of(L" \t");
            size_t after = 0;
            int64_t firstMs = 0;
            if (first == std::wstring::npos ||
                !ParseEnhancedLrcTimeTag(line, first, after, &firstMs)) continue;
            timestamps.push_back(firstMs);
            pos = first;
        }
        std::wstring content = StripEnhancedLrcTimeTags(line.substr(pos));
        size_t first = content.find_first_not_of(L" \t");
        if (first == std::wstring::npos) content.clear();
        else content.erase(0, first);
        size_t last = content.find_last_not_of(L" \t");
        if (last != std::wstring::npos) content.erase(last + 1);
        for (auto ms : timestamps) if (cues.size() < 4000) cues.push_back({ms, content});
    }
    std::stable_sort(cues.begin(), cues.end(), [](const LyricCue& a, const LyricCue& b) {
        return a.timeMs < b.timeMs;
    });
    // Multiple LRC tags may resolve to the same timestamp. Last nonempty line wins.
    std::vector<LyricCue> unique;
    unique.reserve(cues.size());
    for (auto& cue : cues) {
        if (!unique.empty() && unique.back().timeMs == cue.timeMs) {
            if (!cue.text.empty() || unique.back().text.empty()) unique.back().text = std::move(cue.text);
        } else unique.push_back(std::move(cue));
    }
    return unique;
}

// Only change lyric display characters, not their timestamps. This is an
// offline, approximate Hindi romanizer; never translate or touch Latin text.
static bool LyricsHasDevanagari(const std::wstring& input) {
    for (wchar_t c : input)
        if (c >= 0x0900 && c <= 0x097F) return true;
    return false;
}
static const wchar_t* HindiConsonantRoot(wchar_t c) {
    switch (c) {
    case 0x0915: return L"k";  case 0x0916: return L"kh";
    case 0x0917: return L"g";  case 0x0918: return L"gh";
    case 0x0919: return L"ng"; case 0x091A: return L"ch";
    case 0x091B: return L"chh";case 0x091C: return L"j";
    case 0x091D: return L"jh"; case 0x091E: return L"ny";
    case 0x091F: return L"t";  case 0x0920: return L"th";
    case 0x0921: return L"d";  case 0x0922: return L"dh";
    case 0x0923: return L"n";  case 0x0924: return L"t";
    case 0x0925: return L"th"; case 0x0926: return L"d";
    case 0x0927: return L"dh"; case 0x0928: return L"n";
    case 0x092A: return L"p";  case 0x092B: return L"ph";
    case 0x092C: return L"b";  case 0x092D: return L"bh";
    case 0x092E: return L"m";  case 0x092F: return L"y";
    case 0x0930: return L"r";  case 0x0932: return L"l";
    case 0x0933: return L"l";  case 0x0935: return L"v";
    case 0x0936: return L"sh"; case 0x0937: return L"sh";
    case 0x0938: return L"s";  case 0x0939: return L"h";
    case 0x0958: return L"q";  case 0x0959: return L"kh";
    case 0x095A: return L"gh"; case 0x095B: return L"z";
    case 0x095C: return L"r";  case 0x095D: return L"rh";
    case 0x095E: return L"f";  case 0x095F: return L"y";
    default: return nullptr;
    }
}
static const wchar_t* HindiMatra(wchar_t c) {
    switch (c) {
    case 0x093E: return L"aa";
    case 0x093F: return L"i";
    case 0x0940: return L"ee";
    case 0x0941: return L"u";
    case 0x0942: return L"oo";
    case 0x0943: return L"ri";
    case 0x0944: return L"ree";
    case 0x0945: case 0x0946: case 0x0947: return L"e";
    case 0x0948: return L"ai";
    case 0x0949: case 0x094A: case 0x094B: return L"o";
    case 0x094C: return L"au";
    case 0x0962: return L"li";
    case 0x0963: return L"lee";
    default: return nullptr;
    }
}
static const wchar_t* HindiIndependentVowel(wchar_t c) {
    switch (c) {
    case 0x0905: return L"a";  case 0x0906: return L"aa";
    case 0x0907: return L"i";  case 0x0908: return L"ee";
    case 0x0909: return L"u";  case 0x090A: return L"oo";
    case 0x090B: return L"ri"; case 0x090C: return L"li";
    case 0x090D: case 0x090E: case 0x090F: return L"e";
    case 0x0910: return L"ai";
    case 0x0911: case 0x0912: case 0x0913: return L"o";
    case 0x0914: return L"au";
    default: return nullptr;
    }
}
static std::wstring LyricsRomanizeDevanagari(const std::wstring& source) {
    if (!LyricsHasDevanagari(source)) return source;
    static const std::unordered_map<std::wstring, std::wstring> common = {
        {L"\u092E\u0948\u0902", L"main"},
        {L"\u0939\u0942\u0901", L"hoon"},
        {L"\u0939\u0942\u0902", L"hoon"},
        {L"\u0939\u0948", L"hai"},
        {L"\u0939\u0948\u0902", L"hain"},
        {L"\u0924\u0941\u092E", L"tum"},
        {L"\u0924\u0941\u092E\u094D\u0939\u0947\u0902", L"tumhe"},
        {L"\u0924\u0941\u092E\u0938\u0947", L"tumse"},
        {L"\u0924\u0941\u092E\u094D\u0939\u093E\u0930\u093E", L"tumhara"},
        {L"\u0939\u092E", L"hum"},
        {L"\u0939\u092E\u0938\u0947", L"humse"},
        {L"\u0939\u092E\u093E\u0930\u093E", L"hamara"},
        {L"\u092E\u0941\u091D\u0947", L"mujhe"},
        {L"\u092E\u0947\u0930\u0947", L"mere"},
        {L"\u092E\u0947\u0930\u093E", L"mera"},
        {L"\u092E\u0947\u0930\u0940", L"meri"},
        {L"\u0924\u0947\u0930\u0947", L"tere"},
        {L"\u0924\u0947\u0930\u093E", L"tera"},
        {L"\u0924\u0947\u0930\u0940", L"teri"},
        {L"\u092A\u094D\u092F\u093E\u0930", L"pyaar"},
        {L"\u0926\u093F\u0932", L"dil"},
        {L"\u0926\u093F\u0932\u094B\u0902", L"dilon"},
        {L"\u0915\u094D\u092F\u093E", L"kya"},
        {L"\u0915\u094D\u092F\u094B\u0902", L"kyun"},
        {L"\u0915\u0939\u093E\u0901", L"kahan"},
        {L"\u0915\u0939\u093E\u0902", L"kahan"},
        {L"\u0928\u0939\u0940\u0902", L"nahi"},
        {L"\u0915\u092D\u0940", L"kabhi"},
        {L"\u0915\u0941\u091B", L"kuch"},
        {L"\u0938\u093E\u0925", L"saath"},
        {L"\u0915\u0930\u0924\u0947", L"karte"},
        {L"\u0915\u0930\u0924\u093E", L"karta"},
        {L"\u0915\u0930\u0924\u0940", L"karti"},
        {L"\u0915\u0930\u0942\u0901", L"karoon"},
        {L"\u0915\u0930\u0942\u0902", L"karoon"},
        {L"\u0915\u0930\u0947", L"kare"},
        {L"\u091C\u093E\u0928\u0947", L"jaane"},
        {L"\u091C\u093E\u0928\u093E", L"jaana"},
        {L"\u091C\u0940\u0928\u093E", L"jeena"},
        {L"\u091C\u093F\u0902\u0926\u0917\u0940", L"zindagi"},
        {L"\u091C\u093C\u093F\u0902\u0926\u0917\u0940", L"zindagi"},
        {L"\u0907\u0936\u094D\u0915\u093C", L"ishq"},
        {L"\u0907\u0936\u094D\u0915", L"ishq"},
        {L"\u0906\u0902\u0916\u094B\u0902", L"aankhon"},
        {L"\u0906\u0901\u0916\u094B\u0902", L"aankhon"},
        {L"\u0906\u0902\u0916\u0947\u0902", L"aankhen"},
        {L"\u0906\u0901\u0916\u0947\u0902", L"aankhen"},
        {L"\u092F\u093E\u0926", L"yaad"},
        {L"\u090F\u0915", L"ek"},
        {L"\u0914\u0930", L"aur"},
        {L"\u092E\u094B\u0939\u092C\u094D\u092C\u0924", L"mohabbat"},
        {L"\u092E\u0947\u0902", L"mein"},
        {L"\u0938\u0947", L"se"},
        {L"\u092A\u0930", L"par"},
        {L"\u0906\u0924\u0940", L"aati"},
        {L"\u095B\u093F\u0902\u0926\u0917\u0940", L"zindagi"},
        {L"\u0938\u092A\u0928\u093E", L"sapna"},
        {L"\u0938\u092A\u0928\u0947", L"sapne"},
        {L"\u0926\u093F\u0932\u092C\u0930", L"dilbar"},
        {L"\u092C\u093F\u0928\u093E", L"bina"},
        {L"\u091A\u0932", L"chal"},
        {L"\u0938\u0941\u0928", L"sun"},
        {L"\u0930\u093E\u0924", L"raat"},
        {L"\u0926\u093F\u0928", L"din"},
        {L"\u0924\u0942", L"tu"},
        {L"\u0924\u0941\u091D\u0947", L"tujhe"},
        {L"\u0924\u0941\u091D\u0938\u0947", L"tujhse"},
        {L"\u0916\u0941\u0926", L"khud"},
        {L"\u091A\u093E\u0939\u0947", L"chahe"},
        {L"\u091A\u093E\u0939\u0924\u093E", L"chahta"},
        {L"\u091A\u093E\u0939\u0924\u0940", L"chahti"},
        {L"\u092C\u093E\u0924", L"baat"},
        {L"\u0932\u0917\u0947", L"lage"},
        {L"\u0932\u0917\u0924\u093E", L"lagta"},
        {L"\u0932\u0917\u0924\u0940", L"lagti"},
        {L"\u092B\u093F\u0930", L"phir"},
        {L"\u092B\u093C\u093F\u0930", L"phir"},
        {L"\u0907\u0938", L"is"},
        {L"\u0909\u0938", L"us"},
        {L"\u092F\u0947", L"ye"},
        {L"\u092F\u0939", L"yeh"},
        {L"\u0935\u094B", L"wo"},
        {L"\u0935\u0939", L"woh"},
        {L"\u0915\u094C\u0928", L"kaun"},
        {L"\u0926\u093F\u0932\u094D\u0932\u0940", L"dilli"},
    };
    std::wstring out;
    out.reserve(source.size() * 3);
    for (size_t i = 0; i < source.size();) {
        const wchar_t c = source[i];
        if (c < 0x0900 || c > 0x097F) {
            out += c; ++i; continue;
        }
        // Match commonly used Hindi words before applying generic phonetics.
        size_t wordEnd = i;
        while (wordEnd < source.size() &&
               source[wordEnd] >= 0x0900 && source[wordEnd] <= 0x097F)
            ++wordEnd;
        if (wordEnd > i) {
            auto found = common.find(source.substr(i, wordEnd - i));
            if (found != common.end()) {
                out += found->second;
                i = wordEnd;
                continue;
            }
        }
        // Frequently occurring conjuncts in song titles and lyrics.
        if (i + 2 < source.size()) {
            if (c == 0x0915 && source[i+1] == 0x094D && source[i+2] == 0x0937) {
                out += L"ksh"; i += 3; continue;
            }
            if (c == 0x091C && source[i+1] == 0x094D && source[i+2] == 0x091E) {
                out += L"gya"; i += 3; continue;
            }
        }
        if (const wchar_t* root = HindiConsonantRoot(c)) {
            std::wstring stem = root;
            size_t next = i + 1;
            if (next < source.size() && source[next] == 0x093C) {
                // Nuktas can be encoded as a separate diacritic.
                switch (c) {
                case 0x0915: stem = L"q"; break;
                case 0x0916: stem = L"kh"; break;
                case 0x0917: stem = L"gh"; break;
                case 0x091C: stem = L"z"; break;
                case 0x0921: stem = L"r"; break;
                case 0x0922: stem = L"rh"; break;
                case 0x092B: stem = L"f"; break;
                default: break;
                }
                ++next;
            }
            if (next < source.size() && source[next] == 0x094D) {
                out += stem; i = next + 1; continue;
            }
            if (next < source.size()) {
                if (const wchar_t* vowel = HindiMatra(source[next])) {
                    out += stem; out += vowel; i = next + 1; continue;
                }
            }
            // Hindi normally omits word-final implicit "a" (e.g. दिल -> dil).
            const bool atEnd = next >= source.size() ||
                (source[next] < 0x0900 || source[next] > 0x097F);
            out += stem;
            if (!atEnd) out += L'a';
            i = next;
            continue;
        }
        if (const wchar_t* vowel = HindiIndependentVowel(c)) {
            out += vowel; ++i; continue;
        }
        if (c == 0x0901 || c == 0x0902) {
            // Use n before most consonants; before labials use m.
            wchar_t following = i + 1 < source.size() ? source[i + 1] : 0;
            const bool labial = following >= 0x092A && following <= 0x092E;
            out += labial ? L'm' : L'n'; ++i; continue;
        }
        if (c == 0x0903) { out += L'h'; ++i; continue; }
        if (c >= 0x0966 && c <= 0x096F) {
            out += wchar_t(L'0' + c - 0x0966); ++i; continue;
        }
        if (c == 0x0964 || c == 0x0965) { out += L'.'; ++i; continue; }
        if (c == 0x0950) { out += L"om"; ++i; continue; }
        // Unrecognized Indic marks are kept, not silently lost.
        out += c;
        ++i;
    }
    return out;
}

// Convert Gurmukhi Punjabi to readable Roman letters; preserve Latin/English text.
// This is local *transliteration*, not automatic translation; lyrics can have
// regional pronunciations that do not map to a unique Roman spelling.
static bool LyricsHasGurmukhi(const std::wstring& input) {
    for (wchar_t c : input)
        if (c >= 0x0A00 && c <= 0x0A7F) return true;
    return false;
}
static const wchar_t* PunjabiConsonantRoot(wchar_t c) {
    switch (c) {
    case 0x0A15: return L"k";   case 0x0A16: return L"kh";
    case 0x0A17: return L"g";   case 0x0A18: return L"gh";
    case 0x0A19: return L"ng";  case 0x0A1A: return L"ch";
    case 0x0A1B: return L"chh"; case 0x0A1C: return L"j";
    case 0x0A1D: return L"jh";  case 0x0A1E: return L"ny";
    case 0x0A1F: return L"t";   case 0x0A20: return L"th";
    case 0x0A21: return L"d";   case 0x0A22: return L"dh";
    case 0x0A23: return L"n";   case 0x0A24: return L"t";
    case 0x0A25: return L"th";  case 0x0A26: return L"d";
    case 0x0A27: return L"dh";  case 0x0A28: return L"n";
    case 0x0A2A: return L"p";   case 0x0A2B: return L"ph";
    case 0x0A2C: return L"b";   case 0x0A2D: return L"bh";
    case 0x0A2E: return L"m";   case 0x0A2F: return L"y";
    case 0x0A30: return L"r";   case 0x0A32: return L"l";
    case 0x0A33: return L"l";   case 0x0A35: return L"v";
    case 0x0A36: return L"sh";  case 0x0A38: return L"s";
    case 0x0A39: return L"h";   case 0x0A59: return L"kh";
    case 0x0A5A: return L"gh";  case 0x0A5B: return L"z";
    case 0x0A5C: return L"rh";  case 0x0A5E: return L"f";
    default: return nullptr;
    }
}
static const wchar_t* PunjabiMatra(wchar_t c) {
    switch (c) {
    case 0x0A3E: return L"aa"; case 0x0A3F: return L"i";
    case 0x0A40: return L"ee"; case 0x0A41: return L"u";
    case 0x0A42: return L"oo"; case 0x0A47: return L"e";
    case 0x0A48: return L"ai"; case 0x0A4B: return L"o";
    case 0x0A4C: return L"au"; case 0x0A43: return L"ri";
    default: return nullptr;
    }
}
static const wchar_t* PunjabiVowel(wchar_t c) {
    switch (c) {
    case 0x0A05: return L"a";  case 0x0A06: return L"aa";
    case 0x0A07: return L"i";  case 0x0A08: return L"ee";
    case 0x0A09: return L"u";  case 0x0A0A: return L"oo";
    case 0x0A0F: return L"e";  case 0x0A10: return L"ai";
    case 0x0A13: return L"o";  case 0x0A14: return L"au";
    default: return nullptr;
    }
}
static std::wstring LyricsRomanizeGurmukhi(const std::wstring& source) {
    if (!LyricsHasGurmukhi(source)) return source;
    static const std::unordered_map<std::wstring, std::wstring> common = {
        {L"\u0A2E\u0A48\u0A02", L"main"},
        {L"\u0A24\u0A42\u0A70", L"tu"},
        {L"\u0A24\u0A47\u0A30\u0A3E", L"tera"},
        {L"\u0A24\u0A47\u0A30\u0A40", L"teri"},
        {L"\u0A24\u0A48\u0A28\u0A42\u0A70", L"tainu"},
        {L"\u0A2E\u0A47\u0A30\u0A3E", L"mera"},
        {L"\u0A2E\u0A47\u0A30\u0A40", L"meri"},
        {L"\u0A2E\u0A47\u0A30\u0A47", L"mere"},
        {L"\u0A39\u0A48", L"hai"},
        {L"\u0A28\u0A39\u0A40\u0A02", L"nahi"},
        {L"\u0A26\u0A3F\u0A32", L"dil"},
        {L"\u0A2A\u0A3F\u0A06\u0A30", L"pyaar"},
        {L"\u0A2A\u0A4D\u0A2F\u0A3E\u0A30", L"pyaar"},
        {L"\u0A35\u0A47", L"ve"},
        {L"\u0A35\u0A3F\u0A71\u0A1A", L"vich"},
        {L"\u0A28\u0A42\u0A70", L"nu"},
        {L"\u0A30\u0A3E\u0A24", L"raat"},
        {L"\u0A38\u0A3E\u0A39", L"saah"},
        {L"\u0A2F\u0A3E\u0A26", L"yaad"},
        {L"\u0A07\u0A36\u0A15", L"ishq"},
        {L"\u0A28\u0A3E\u0A32", L"naal"},
        {L"\u0A24\u0A47", L"te"},
        {L"\u0A15\u0A40", L"ki"},
        {L"\u0A06", L"aa"},
        {L"\u0A39\u0A4B", L"ho"},
        {L"\u0A1C\u0A3F\u0A70\u0A26\u0A17\u0A40", L"zindagi"},
    };
    std::wstring out;
    out.reserve(source.size() * 3);
    for (size_t i = 0; i < source.size();) {
        wchar_t c = source[i];
        if (c < 0x0A00 || c > 0x0A7F) { out += c; ++i; continue; }
        size_t end = i;
        while (end < source.size() && source[end] >= 0x0A00 && source[end] <= 0x0A7F) ++end;
        auto it = common.find(source.substr(i, end-i));
        if (it != common.end()) { out += it->second; i=end; continue; }
        if (const wchar_t* root = PunjabiConsonantRoot(c)) {
            std::wstring stem(root);
            size_t next = i+1;
            if (next < source.size() && source[next] == 0x0A3C) {
                // Nukta variations often encode z, f, kh and gh separately.
                switch (c) {
                case 0x0A1C: stem=L"z"; break;
                case 0x0A2B: stem=L"f"; break;
                case 0x0A16: stem=L"kh"; break;
                case 0x0A17: stem=L"gh"; break;
                case 0x0A21: stem=L"rh"; break;
                default: break;
                }
                ++next;
            }
            if (next < source.size() && source[next] == 0x0A4D) {
                out += stem; i=next+1; continue;
            }
            if (next < source.size()) {
                if (const wchar_t* matra = PunjabiMatra(source[next])) {
                    out += stem; out += matra;
                    i=next+1; continue;
                }
            }
            const bool atEnd = next >= source.size() || source[next] < 0x0A00 || source[next] > 0x0A7F;
            out += stem;
            if (!atEnd) out += L'a';
            i=next; continue;
        }
        if (const wchar_t* vowel = PunjabiVowel(c)) {
            out += vowel; ++i; continue;
        }
        if (c == 0x0A70 || c == 0x0A02 || c == 0x0A01) { out += L'n'; ++i; continue; }
        if (c == 0x0A03) { out += L'h'; ++i; continue; }
        if (c == 0x0A71) { // Addak doubles the following consonant when possible.
            if (i+1 < source.size()) {
                const wchar_t* next = PunjabiConsonantRoot(source[i+1]);
                if (next) out += next;
            }
            ++i; continue;
        }
        if (c == 0x0A3C || c == 0x0A4D || c == 0x0A51 || c == 0x0A75) { ++i; continue; }
        if (c >= 0x0A66 && c <= 0x0A6F) { out += wchar_t(L'0' + c - 0x0A66); ++i; continue; }
        if (c == 0x0A64) { out += L'.'; ++i; continue; }
        // Some rare characters lack a reliable pronunciation; preserve rather
        // than silently deleting parts of the lyrics.
        out += c;
        ++i;
    }
    return out;
}
static bool LyricsHasIndicScript(const std::wstring& line) {
    return LyricsHasDevanagari(line) || LyricsHasGurmukhi(line);
}
static std::wstring LyricsRomanizeIndic(const std::wstring& line) {
    if (!LyricsHasIndicScript(line)) return line;
    std::wstring out;
    out.reserve(line.size() * 2);
    auto script = [](wchar_t c) {
        if (c >= 0x0900 && c <= 0x097F) return 1;
        if (c >= 0x0A00 && c <= 0x0A7F) return 2;
        return 0;
    };
    for (size_t i = 0; i < line.size();) {
        const int currentScript = script(line[i]);
        if (!currentScript) {
            // Preserve English, punctuation, accents, spacing and emoji verbatim.
            out += line[i++];
            continue;
        }
        std::wstring run;
        while (i < line.size()) {
            if (script(line[i]) == currentScript) run += line[i++];
            else if ((line[i] == 0x200C || line[i] == 0x200D || line[i] == 0xFEFF) &&
                     i + 1 < line.size() && script(line[i + 1]) == currentScript) ++i;
            else break;
        }
        out += currentScript == 1 ? LyricsRomanizeDevanagari(run) : LyricsRomanizeGurmukhi(run);
    }
    return out;
}

// Normalize cosmetic suffixes, but retain version words ("live", "remix") to avoid
// showing lyrics for a completely different recording when the title is ambiguous.
static std::wstring NormalizeLyricsMatch(std::wstring text) {
    std::wstring out;
    bool space = false;
    for (wchar_t c : text) {
        c = (wchar_t)towlower(c);
        if (iswalnum(c)) {
            if (space && !out.empty()) out += L' ';
            out += c;
            space = false;
        } else space = true;
    }
    return out;
}
// Spotify's GSMTC Artist field often combines performers ("A, B", "A & B", etc.),
// while LRCLIB may credit only one primary artist. Keep the full name as the
// strongest match and accept a primary-artist match only as a fallback.
static std::vector<std::wstring> LyricsArtistMatchKeys(const std::wstring& artist) {
    std::vector<std::wstring> keys;
    auto add = [&](const std::wstring& value) {
        auto normalized = NormalizeLyricsMatch(LyricsRomanizeIndic(value));
        if (normalized.size() < 3) return;  // Avoid unsafe one-letter matches.
        if (std::find(keys.begin(), keys.end(), normalized) == keys.end())
            keys.push_back(std::move(normalized));
    };
    add(artist);
    std::wstring lower = artist;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](wchar_t c) { return (wchar_t)towlower(c); });
    size_t split = std::wstring::npos;
    for (auto delim : {L", ", L"; ", L" & ", L" + ", L" feat. ",
                       L" ft. ", L" featuring ", L" with ", L" x "}) {
        size_t at = lower.find(delim);
        if (at != std::wstring::npos && (split == std::wstring::npos || at < split))
            split = at;
    }
    if (split != std::wstring::npos) add(artist.substr(0, split));
    return keys;
}
static std::wstring LyricsCleanTitle(std::wstring title) {
    // Strip just the common VIDEO/AUDIO tags appended by browser media sessions.
    for (const wchar_t* tag : {L"(official video)", L"(official music video)",
                               L"(official audio)", L"(lyrics)", L"[official video]",
                               L"[official audio]", L"[lyrics]", L"(lyric video)",
                               L"(official lyric video)", L"(music video)",
                               L"[official music video]", L"(visualizer)"}) {
        std::wstring lower = title;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](wchar_t c) { return (wchar_t)towlower(c); });
        const size_t loc = lower.find(tag);
        if (loc != std::wstring::npos) title.erase(loc, wcslen(tag));
    }
    const size_t left = title.find_first_not_of(L" \t-|");
    if (left == std::wstring::npos) return L"";
    const size_t right = title.find_last_not_of(L" \t-|");
    return title.substr(left, right - left + 1);
}
// Different media apps sometimes publish "Artist - Song" as the title, or
// append a cosmetic prefix. Restrict splitting to known artist matches to
// avoid mistakenly interpreting genuine dashed song names.
static std::vector<std::wstring> LyricsTitleVariants(const std::wstring& original,
                                                     const std::wstring& artist) {
    std::vector<std::wstring> variants;
    auto add = [&](std::wstring s) {
        s = LyricsCleanTitle(std::move(s));
        if (s.empty() || s.size() > 240) return;
        for (const auto& v : variants)
            if (NormalizeLyricsMatch(v) == NormalizeLyricsMatch(s)) return;
        variants.push_back(std::move(s));
    };
    add(original);
    if (LyricsHasIndicScript(original)) add(LyricsRomanizeIndic(original));
    // Spotify sometimes appends a featured-artist credit to the *title* even
    // though LRCLIB stores the recording under its plain name. Try this only
    // after the full title, and never remove (Live), (Remix), or (Acoustic).
    std::wstring titleLower = original;
    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(),
                   [](wchar_t c) { return (wchar_t)towlower(c); });
    size_t feat = std::wstring::npos;
    for (auto marker : {L" (feat.", L" [feat.", L" (ft.", L" [ft.",
                        L" (featuring ", L" [featuring "}) {
        const auto pos = titleLower.find(marker);
        if (pos != std::wstring::npos && (feat == std::wstring::npos || pos < feat))
            feat = pos;
    }
    if (feat != std::wstring::npos) add(original.substr(0, feat));
    for (const wchar_t* cosmetic : {L" (from ", L" [from ", L" (official song)",
                                   L" (official music)", L" (full song)", L" (audio)",
                                   L" (video)", L" (song)", L" | official"}) {
        const auto pos = titleLower.find(cosmetic);
        if (pos != std::wstring::npos && pos >= 2) add(original.substr(0, pos));
    }
    // A streaming title may append a music-video description after a pipe.
    // The original title still takes priority; artist matching remains mandatory.
    const auto pipe = original.find(L" | ");
    if (pipe != std::wstring::npos && pipe >= 2) add(original.substr(0, pipe));
    const auto sep = original.find(L" - ");
    if (sep != std::wstring::npos && !artist.empty()) {
        auto left = original.substr(0, sep);
        auto right = original.substr(sep + 3);
        const auto artistKey = NormalizeLyricsMatch(artist);
        if (NormalizeLyricsMatch(left) == artistKey) add(right);
        if (NormalizeLyricsMatch(right) == artistKey) add(left);
    }
    return variants;
}
// Fixed-host HTTPS transport. Network failures do not permanently cache misses.
static bool LyricsHttpGet(const std::wstring& path, std::string& json, DWORD& status) {
    status = 0;
    json.clear();
    if ((g_unloading.load() || g_applyingSettings.load())) return false;
    struct InternetHandle { HINTERNET value = nullptr; ~InternetHandle() { if (value) WinHttpCloseHandle(value); } } session, connect, request;
    session.value = WinHttpOpen(L"TaskbarFluentLyrics/3.1.4 (Windhawk taskbar lyrics)", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                 WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session.value) return false;
    WinHttpSetTimeouts(session.value, 2000, 2000, 5000, 5000);
    connect.value = WinHttpConnect(session.value, L"lrclib.net", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!connect.value) return false;
    request.value = WinHttpOpenRequest(connect.value, L"GET", path.c_str(), nullptr,
                                        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!request.value) return false;
    WinHttpAddRequestHeaders(request.value, L"Accept: application/json\r\n", (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD);
    if (!WinHttpSendRequest(request.value, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request.value, nullptr)) return false;
    DWORD statusSize = sizeof(status);
    if (!WinHttpQueryHeaders(request.value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX)) return false;
    if (status != 200) return true; // 404 is a catalog miss, not a transport failure.
    while (!(g_unloading.load() || g_applyingSettings.load()) && json.size() < 512 * 1024) {
        char buf[4096];
        DWORD bytes = 0;
        if (!WinHttpReadData(request.value, buf, sizeof(buf), &bytes)) return false;
        if (!bytes) break;
        if (json.size() + bytes > 512 * 1024) return false;
        json.append(buf, bytes);
    }
    return !(g_unloading.load() || g_applyingSettings.load());
}
static std::vector<LyricCue> LyricsCuesFromJson(winrt::Windows::Data::Json::JsonObject const& obj,
                                                   bool* isPlain = nullptr) {
    if (isPlain) *isPlain = false;
    if (!obj) return {};
    if (obj.HasKey(L"syncedLyrics")) {
        auto value = obj.Lookup(L"syncedLyrics");
        if (value && value.ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
            auto timed = ParseLrcLines(std::wstring(value.GetString()));
            if (!timed.empty()) return timed;
        }
    }
    if (!g_settings.showPlainLyrics || !obj.HasKey(L"plainLyrics")) return {};
    auto plain = obj.Lookup(L"plainLyrics");
    if (!plain || plain.ValueType() != winrt::Windows::Data::Json::JsonValueType::String) return {};
    const std::wstring content = plain.GetString().c_str();
    std::vector<LyricCue> result;
    size_t start = 0;
    while (start < content.size() && result.size() < 70) {
        size_t end = content.find_first_of(L"\r\n", start);
        if (end == std::wstring::npos) end = content.size();
        auto line = StripEnhancedLrcTimeTags(
            content.substr(start, std::min<size_t>(end - start, 200)));
        start = end + 1;
        auto first = line.find_first_not_of(L" \t");
        if (first == std::wstring::npos) continue;
        line.erase(0, first);
        auto last = line.find_last_not_of(L" \t");
        if (last != std::wstring::npos) line.erase(last + 1);
        if (line.empty()) continue;
        // These timestamps are display intervals, NOT a claim of singing sync.
        result.push_back({(int64_t)result.size() * g_settings.plainLyricsLineMs,
                          L"Unsynced \u00b7 " + line});
    }
    if (!result.empty() && isPlain) *isPlain = true;
    return result;
}
// Reject wrong-track LRCLIB results BEFORE reading or caching their lyrics.
// Search returns records with loose matching; /api/get is also a best-match API,
// so neither endpoint is trusted without this independent verification.
static int LyricsMatchRecordScore(const winrt::Windows::Data::Json::JsonObject& obj,
                                  const std::wstring& title, const std::wstring& artist,
                                  const std::wstring& album, int durationSec) {
    if (!obj) return -1;
    const auto trackKey = NormalizeLyricsMatch(LyricsRomanizeIndic(title));
    const auto recordKey = NormalizeLyricsMatch(LyricsRomanizeIndic(
        std::wstring(obj.GetNamedString(L"trackName", L""))));
    if (trackKey.empty() || recordKey != trackKey) return -1;
    int score = 100;
    if (!artist.empty()) {
        const auto wanted = LyricsArtistMatchKeys(artist);
        const auto received = LyricsArtistMatchKeys(std::wstring(obj.GetNamedString(L"artistName", L"")));
        if (wanted.empty() || received.empty()) return -1;
        bool matched = false;
        for (const auto& w : wanted)
            for (const auto& r : received)
                if (w == r) matched = true;
        if (!matched) return -1; // No substring / single-token artist guesses.
        score += wanted.front() == received.front() ? 60 : 25;
    }
    if (durationSec > 0 && obj.HasKey(L"duration")) {
        auto v = obj.Lookup(L"duration");
        if (v && v.ValueType() == winrt::Windows::Data::Json::JsonValueType::Number) {
            const double seconds = v.GetNumber();
            if (std::isfinite(seconds) && seconds > 0) {
                // A different edit/remaster can contain unrelated timed verses.
                if (std::fabs(seconds - durationSec) > 4.0) return -1;
                score += 50;
            }
        }
    }
    if (!album.empty()) {
        const auto wantedAlbum = NormalizeLyricsMatch(LyricsRomanizeIndic(album));
        const auto foundAlbum = NormalizeLyricsMatch(LyricsRomanizeIndic(
            std::wstring(obj.GetNamedString(L"albumName", L""))));
        if (!foundAlbum.empty() && wantedAlbum == foundAlbum) score += 25;
        // Do not reject differing album names: singles and compilations can
        // legitimately contain the same recording.
    }
    return score;
}
static bool LyricsRecordScriptAllowed(const std::vector<LyricCue>& cues) {
    if (!g_settings.lyricsRequireNativeScript) return true;
    for (const auto& cue : cues) if (LyricsHasIndicScript(cue.text)) return true;
    return false;
}
static LyricLookupResult FetchSyncedLyricsFromLrclib(const std::wstring& title,
                                                       const std::wstring& artist,
                                                       const std::wstring& album,
                                                       int durationSec) {
    LyricLookupResult result;
    if (title.empty() || (g_unloading.load() || g_applyingSettings.load())) return result;
    auto titles = LyricsTitleVariants(title, artist);
    if (titles.empty() || artist.size() > 240) return result;

    int bestScore = -1;
    // Compare scores across ALL search passes and title variants. Avoid a later
    // broad search replacing an earlier exact / high-confidence match.
    auto evaluateSearchResults = [&](const std::string& response,
                                     const std::wstring& requestedTitle) {
        try {
            winrt::Windows::Data::Json::JsonArray records{nullptr};
            if (!winrt::Windows::Data::Json::JsonArray::TryParse(
                    winrt::hstring(LyricsFromUtf8(response)), records)) return;
            const uint32_t count = std::min<uint32_t>(records.Size(), 20);
            for (uint32_t i = 0; i < count; ++i) {
                if (g_unloading.load() || g_applyingSettings.load()) return;
                auto value = records.GetAt(i);
                if (value.ValueType() != winrt::Windows::Data::Json::JsonValueType::Object) continue;
                auto obj = value.GetObject();
                int score = LyricsMatchRecordScore(obj, requestedTitle, artist, album, durationSec);
                if (score < 0) continue;
                bool plain = false;
                auto cues = LyricsCuesFromJson(obj, &plain);
                if (cues.empty() || !LyricsRecordScriptAllowed(cues)) continue;
                if (plain) score -= 1000;
                if (score > bestScore) {
                    bestScore = score;
                    result.cues = std::move(cues);
                    result.unsynced = plain;
                }
            }
        } catch (...) {}
    };
    std::string json;
    DWORD status = 0;
    for (size_t variant = 0; variant < titles.size() && variant < 5; ++variant) {
        if ((g_unloading.load() || g_applyingSettings.load())) break;
        const auto& cleanTitle = titles[variant];
        // Query Roman artist for Roman title; retain original when both are Indic.
        const std::wstring lookupArtist = LyricsHasIndicScript(cleanTitle)
            ? artist : LyricsRomanizeIndic(artist);
        if (!artist.empty() && variant == 0) {
            std::wstring exact = L"/api/get?track_name=" + LyricsUrlEncode(cleanTitle) +
                L"&artist_name=" + LyricsUrlEncode(lookupArtist);
            if (!album.empty()) exact += L"&album_name=" + LyricsUrlEncode(album);
            if (durationSec > 0) exact += L"&duration=" + std::to_wstring(durationSec);
            if (!LyricsHttpGet(exact, json, status)) result.networkError = true;
            else if (status == 200) {
                try {
                    winrt::Windows::Data::Json::JsonObject obj{nullptr};
                    if (winrt::Windows::Data::Json::JsonObject::TryParse(
                            winrt::hstring(LyricsFromUtf8(json)), obj)) {
                        const int score = LyricsMatchRecordScore(obj, cleanTitle, artist,
                                                               album, durationSec);
                        if (score >= 0) {
                            bool plain = false;
                            auto cues = LyricsCuesFromJson(obj, &plain);
                            if (!cues.empty() && LyricsRecordScriptAllowed(cues)) {
                                result.cues = std::move(cues);
                                result.unsynced = plain;
                                bestScore = score - (plain ? 1000 : 0);
                                if (!plain) return result;
                            }
                        } else {
                            Wh_Log(L"Lyrics: rejected /api/get result with mismatched track, artist or duration");
                        }
                    }
                } catch (...) {}
            } else if (status == 429) { result.networkError = true; return result; }
            else if (status >= 500) result.networkError = true;
        }
        // Search first with exact title+artist (precision), then with title only
        // for records with alternate artist credits. All matches are validated.
        for (int pass = 0; pass < 3 && !(g_unloading.load() || g_applyingSettings.load()); ++pass) {
            if (pass == 1 && artist.empty()) continue;
            std::wstring query;
            if (pass == 0) {
                query = L"/api/search?track_name=" + LyricsUrlEncode(cleanTitle);
                if (!artist.empty()) query += L"&artist_name=" + LyricsUrlEncode(lookupArtist);
            } else if (pass == 1) {
                query = L"/api/search?track_name=" + LyricsUrlEncode(cleanTitle);
            } else {
                // LRCLIB's keyword search also covers album/alternate metadata.
                query = L"/api/search?q=" + LyricsUrlEncode(
                    artist.empty() ? cleanTitle : cleanTitle + L" " + lookupArtist);
            }
            if (!LyricsHttpGet(query, json, status)) { result.networkError = true; continue; }
            if (status == 429) { result.networkError = true; return result; }
            if (status >= 500) { result.networkError = true; continue; }
            if (status != 200) continue;
            evaluateSearchResults(json, cleanTitle);
            if (!result.cues.empty() && !result.unsynced) return result;
        }
        if (result.unsynced && variant >= 1) break;
    }
    return result;
}
static void LyricsLookupWorker() {
    // WinRT JsonObject activation is performed on this worker thread.
    struct Apartment {
        bool initialized = false;
        Apartment() {
            try { winrt::init_apartment(winrt::apartment_type::multi_threaded); initialized = true; }
            catch (...) {}
        }
        ~Apartment() { if (initialized) winrt::uninit_apartment(); }
    } apartment;
    for (;;) {
        std::wstring key, title, artist, album;
        int durationSec = 0;
        uint64_t serial = 0;
        {
            std::lock_guard<std::mutex> lk(g_lyricsMutex);
            if (!g_lyricsPending || (g_unloading.load() || g_applyingSettings.load())) {
                g_lyricsWorkerActive = false;
                return;
            }
            g_lyricsPending = false;
            key = g_lyricsKey;
            title = g_lyricsRequestedTitle;
            artist = g_lyricsRequestedArtist;
            album = g_lyricsRequestedAlbum;
            durationSec = g_lyricsRequestedDuration;
            serial = g_lyricsGeneration;
        }
        auto found = FetchSyncedLyricsFromLrclib(title, artist, album, durationSec);
        bool resultChanged = false;
        {
            std::lock_guard<std::mutex> lk(g_lyricsMutex);
            if ((g_unloading.load() || g_applyingSettings.load())) { g_lyricsWorkerActive = false; return; }
            if (g_lyricsGeneration == serial && g_lyricsKey == key) {
                if (!found.cues.empty()) {
                    g_lyricsCues = found.cues;
                    g_lyricsUnsynced = found.unsynced;
                    g_lyricsCurrentIndex = -1;
                    g_lyricsDisplayIndex = -1;
                    g_lyricsFarDueSoon = false;
                    g_lyricsLastPositionMs = -1;
                    g_lyricsCache.emplace_back(key, LyricRecord{std::move(found.cues), found.unsynced});
                    if (g_lyricsCache.size() > 32) g_lyricsCache.erase(g_lyricsCache.begin());
                    g_lyricsNeedRetry = false;
                    if (found.unsynced) {
                        Wh_Log(L"Lyrics: loaded plain text (unsynced display)");
                    } else {
                        Wh_Log(L"Lyrics: loaded synchronized lines");
                    }
                    resultChanged = true;
                } else {
                    // Useful Windhawk logging without including private track metadata.
                    if (found.networkError) {
                        Wh_Log(L"Lyrics: LRCLIB request failed / rate-limited; retry scheduled");
                    } else {
                        Wh_Log(L"Lyrics: no verified match (title/artist/duration/script); unrelated translations rejected");
                    }
                    g_lyricsNeedRetry = ++g_lyricsFailureCount < 3;
                    g_lyricsNextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(found.networkError ? 45 : 300);
                }
            }
        }
        if (resultChanged) DispatchMediaUpdate();
    }
}
static void RequestLyricsForTrack(const std::wstring& title, const std::wstring& artist,
                                  GlobalSystemMediaTransportControlsSession expectedSession = nullptr,
                                  const std::wstring& album = L"", int durationSec = 0) {
    if (!g_settings.showLyrics) return;
    std::unique_lock<std::mutex> sessionGuard(g_sessionMtx);
    if (expectedSession && g_currentSession != expectedSession) return;
    // Include album and duration so a different release cannot reuse cached
    // lyrics selected for another recording with the same title and artist.
    std::wstring key = title + L"\x1f" + artist + L"\x1f" + album + L"\x1f" +
                       std::to_wstring(durationSec);
    bool launch = false;
    {
        std::lock_guard<std::mutex> lk(g_lyricsMutex);
        if (g_lyricsKey == key) return;
        g_lyricsKey = key;
        g_lyricsRequestedTitle = title;
        g_lyricsRequestedArtist = artist;
        g_lyricsRequestedAlbum = album;
        g_lyricsRequestedDuration = durationSec;
        ++g_lyricsGeneration;
        g_lyricsCurrentIndex = -1;
        g_lyricsDisplayIndex = -1;
        g_lyricsFarDueSoon = false;
        g_lyricsLastPositionMs = -1;
        g_lyricsCues.clear();
        g_lyricsUnsynced = false;
        g_lyricsPending = false;
        g_lyricsNeedRetry = false;
        g_lyricsFailureCount = 0;
        if (!title.empty() && g_settings.lyricsOnlineLookup) {
            bool cached = false;
            for (const auto& entry : g_lyricsCache) {
                if (entry.first == key) {
                    g_lyricsCues = entry.second.cues;
                    g_lyricsUnsynced = entry.second.unsynced;
                    cached = true;
                    break;
                }
            }
            if (!cached) {
                g_lyricsPending = true;
                if (!g_lyricsWorkerActive) {
                    g_lyricsWorkerActive = true;
                    launch = true;
                }
            }
        }
    }
    sessionGuard.unlock();
    DispatchMediaUpdate();
    if (launch) SpawnTrackedWorker([] { LyricsLookupWorker(); });
}
static void MaybeRetryLyricsLookup() {
    if (!g_settings.showLyrics || !g_settings.lyricsOnlineLookup || g_unloading.load()) return;
    bool launch = false;
    {
        std::lock_guard<std::mutex> lk(g_lyricsMutex);
        if (g_lyricsNeedRetry && !g_lyricsWorkerActive && !g_lyricsPending &&
            !g_lyricsRequestedTitle.empty() &&
            std::chrono::steady_clock::now() >= g_lyricsNextRetry) {
            g_lyricsNeedRetry = false;
            g_lyricsPending = true;
            g_lyricsWorkerActive = true;
            launch = true;
        }
    }
    if (launch) SpawnTrackedWorker([] { LyricsLookupWorker(); });
}
static void UpdateLyricsPositionFromSession() {
    if (!g_settings.showLyrics || g_unloading.load()) return;
    GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        session = g_currentSession;
    }
    int64_t ms = -1;
    bool playing = false;
    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        playing = g_media.isPlaying;
    }
    struct TimelineCache {
        GlobalSystemMediaTransportControlsSession session{nullptr};
        int64_t position = -1, end = 0;
        winrt::Windows::Foundation::DateTime updated{};
        double rate = 1.0;
        std::chrono::steady_clock::time_point fetched{};
        // Spotify/browser media sessions can expose metadata without a timeline.
        // This estimate is intentionally separate from the real GSMTC clock.
        uint64_t estimatedGeneration = UINT64_MAX;
        int64_t estimatedPositionMs = 0;
        std::chrono::steady_clock::time_point estimatedUpdated{};
        bool estimatedWarningLogged = false;
    };
    // Timer-thread local; reset it before the timer thread is destroyed.
    thread_local TimelineCache cache;
    const auto now = std::chrono::steady_clock::now();
    if (session) try {
        if (cache.session != session || g_timelineDirty.exchange(false) ||
            now - cache.fetched >= std::chrono::seconds(1)) {
            auto timeline = session.GetTimelineProperties();
            cache.session = session;
            cache.position = -1;
            cache.fetched = now;
            if (timeline) {
                cache.position = std::chrono::duration_cast<std::chrono::milliseconds>(timeline.Position()).count();
                cache.end = std::chrono::duration_cast<std::chrono::milliseconds>(timeline.EndTime()).count();
                cache.updated = timeline.LastUpdatedTime();
                cache.rate = 1.0;
                if (auto info = session.GetPlaybackInfo()) {
                    if (auto rate = info.PlaybackRate()) {
                        if (std::isfinite(rate.Value())) cache.rate = rate.Value();
                    }
                }
            }
        }
        ms = cache.position;
        if (ms >= 0) {
            if (playing) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(winrt::clock::now() - cache.updated).count();
                if (elapsed > 0 && elapsed < 6LL * 60 * 60 * 1000) ms += (int64_t)(elapsed * cache.rate);
            }
            if (cache.end > 0 && cache.end < 24LL * 60 * 60 * 1000) ms = std::min(ms, cache.end);
            ms = std::max<int64_t>(0, ms + g_settings.lyricsOffsetMs);
        }
    } catch (...) { ms = -1; cache.session = nullptr; }
    else cache = {};
    // Some Spotify desktop/browser combinations publish the track details but
    // no usable GSMTC position. Don't silently suppress an otherwise valid
    // lyric record forever. The fallback is visibly documented as approximate.
    if (ms < 0 && session && g_settings.lyricsNoTimelineFallback) {
        uint64_t generation = 0;
        bool haveLyrics = false;
        {
            std::lock_guard<std::mutex> lk(g_lyricsMutex);
            generation = g_lyricsGeneration;
            haveLyrics = !g_lyricsCues.empty();
        }
        if (cache.estimatedGeneration != generation) {
            cache.estimatedGeneration = generation;
            cache.estimatedPositionMs = 0;
            cache.estimatedUpdated = now;
            cache.estimatedWarningLogged = false;
        }
        if (cache.estimatedUpdated != std::chrono::steady_clock::time_point{}) {
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - cache.estimatedUpdated).count();
            if (playing && elapsed > 0 && elapsed < 10000)
                cache.estimatedPositionMs += elapsed;
        }
        cache.estimatedUpdated = now;
        if (haveLyrics) {
            ms = std::max<int64_t>(0, cache.estimatedPositionMs + g_settings.lyricsOffsetMs);
            if (!cache.estimatedWarningLogged) {
                Wh_Log(L"Lyrics: Windows did not provide track timing; using approximate clock (seek sync unavailable)");
                cache.estimatedWarningLogged = true;
            }
        }
    } else if (ms >= 0) {
        cache.estimatedGeneration = UINT64_MAX;
    }
    bool changed = false;
    {
        std::lock_guard<std::mutex> lk(g_lyricsMutex);
        // Media timelines can jitter slightly backward during polling. Keep the
        // currently displayed line until a genuine seek or previous cue boundary.
        int newIndex = -1;
        if (ms >= 0 && !g_lyricsCues.empty()) {
            auto it = std::upper_bound(g_lyricsCues.begin(), g_lyricsCues.end(), ms,
                [](int64_t x, const LyricCue& cue) { return x < cue.timeMs; });
            if (it != g_lyricsCues.begin()) newIndex = (int)(it - g_lyricsCues.begin()) - 1;
            if (newIndex < g_lyricsCurrentIndex && g_lyricsLastPositionMs >= 0 &&
                g_lyricsLastPositionMs - ms < 750 &&
                g_lyricsCurrentIndex < (int)g_lyricsCues.size() &&
                ms + 180 >= g_lyricsCues[g_lyricsCurrentIndex].timeMs)
                newIndex = g_lyricsCurrentIndex;
        }
        g_lyricsLastPositionMs = ms;
        if (newIndex != g_lyricsCurrentIndex) {
            g_lyricsCurrentIndex = newIndex;
            changed = true;
        }
        // Distinguish a song having LRC cues from a cue actually being displayed.
        // In a long instrumental gap the previous line should not stay on screen
        // forever. During the last configured lead time before a new nonempty
        // cue, return the visualizer to its normal far slot ahead of the words.
        int displayIndex = newIndex;
        bool farDueSoon = false;
        if (ms >= 0 && !g_lyricsCues.empty()) {
            if (displayIndex >= 0 && g_lyricsCues[displayIndex].text.empty())
                displayIndex = -1;
            const int next = newIndex + 1;
            const bool haveNext = next >= 0 && next < (int)g_lyricsCues.size();
            const int64_t heldMs = newIndex >= 0 ? ms - g_lyricsCues[newIndex].timeMs : 0;
            const int64_t nextDistance = haveNext ? g_lyricsCues[next].timeMs - ms : INT64_MAX;
            const bool longGap = !haveNext ||
                (newIndex >= 0 && g_lyricsCues[next].timeMs - g_lyricsCues[newIndex].timeMs >
                    (int64_t)g_settings.vizInstrumentalAfterMs);
            if (!g_lyricsUnsynced && playing && newIndex >= 0 && longGap &&
                heldMs >= g_settings.vizInstrumentalAfterMs)
                displayIndex = -1;
            // A line that will start shortly may be prepared in advance, but
            // do not show its text until its own timestamp has arrived.
            if (!g_lyricsUnsynced && playing && haveNext && !g_lyricsCues[next].text.empty() &&
                nextDistance > 0 && nextDistance <= g_settings.vizBeforeLyricsMs &&
                displayIndex == -1)
                farDueSoon = true;
            if (!g_lyricsUnsynced && playing && newIndex < 0 && !g_lyricsCues.front().text.empty() &&
                g_lyricsCues.front().timeMs - ms > 0 &&
                g_lyricsCues.front().timeMs - ms <= g_settings.vizBeforeLyricsMs)
                farDueSoon = true;
        }
        if (displayIndex != g_lyricsDisplayIndex || farDueSoon != g_lyricsFarDueSoon) {
            g_lyricsDisplayIndex = displayIndex;
            g_lyricsFarDueSoon = farDueSoon;
            changed = true;
        }
    }
    if (changed) DispatchLyricsUpdate();
}

// UI references are used only on the taskbar's XAML thread.
[[clang::no_destroy]] static TextBlock g_lyricsLineA = nullptr;
[[clang::no_destroy]] static TextBlock g_lyricsLineB = nullptr;
[[clang::no_destroy]] static Storyboard g_lyricsStoryboard = nullptr;
static bool g_lyricsFirstLineActive = true;
static uint64_t g_lyricsUiGeneration = UINT64_MAX;
static int g_lyricsUiIndex = -2;
static std::wstring g_lyricsVisibleText;

static void ResetLyricsVisuals() {
    try { if (g_lyricsStoryboard) g_lyricsStoryboard.Stop(); } catch (...) {}
    g_lyricsStoryboard = nullptr;
    g_lyricsLineA = nullptr;
    g_lyricsLineB = nullptr;
    g_lyricsFirstLineActive = true;
    g_lyricsUiGeneration = UINT64_MAX;
    g_lyricsUiIndex = -2;
    g_lyricsVisibleText.clear();
}

static void RefreshLyricsOnlyUi() {
    if (!g_playerGrid || !g_lyricsLineA || !g_lyricsLineB) return;
    std::wstring nextLine;
    uint64_t generation = 0;
    int index = -1;
    {
        std::lock_guard<std::mutex> lk(g_lyricsMutex);
        generation = g_lyricsGeneration;
        index = g_lyricsDisplayIndex;
        if (index >= 0 && index < (int)g_lyricsCues.size()) nextLine = g_lyricsCues[index].text;
    }
    if (g_settings.previewLayout) nextLine = L"Your synchronized lyrics appear here";
    if (generation == g_lyricsUiGeneration && index == g_lyricsUiIndex) return;
    // Remove invisible directional format controls supplied by some LRC files.
    // These can reverse mixed English/Indic punctuation or clip line layout.
    nextLine.erase(std::remove_if(nextLine.begin(), nextLine.end(), [](wchar_t ch) {
        return ch == 0xFEFF || ch == 0x200E || ch == 0x200F ||
               (ch >= 0x202A && ch <= 0x202E) || (ch >= 0x2066 && ch <= 0x2069);
    }), nextLine.end());
    // Always display supported Indian-script lyrics in Roman letters.
    // Other scripts are preserved (not falsely converted with Hindi rules).
    if (g_settings.hindiLyricsScript == L"roman" && LyricsHasIndicScript(nextLine))
        nextLine = LyricsRomanizeIndic(nextLine);
    const bool songChanged = (generation != g_lyricsUiGeneration);
    g_lyricsUiGeneration = generation;
    g_lyricsUiIndex = index;
    // Handle tracks, playback seeking, missing lyrics, or repeated identical lyric lines.
    if (nextLine == g_lyricsVisibleText) return;
    bool hadLine = !g_lyricsVisibleText.empty();
    // Changing song should never animate the previous track's lyric into the next one.
    if (songChanged) hadLine = false;
    g_lyricsVisibleText = nextLine;
    try {
        if (g_lyricsStoryboard) g_lyricsStoryboard.Stop();
        g_lyricsStoryboard = nullptr;
        TextBlock departing = g_lyricsFirstLineActive ? g_lyricsLineA : g_lyricsLineB;
        TextBlock arriving = g_lyricsFirstLineActive ? g_lyricsLineB : g_lyricsLineA;
        g_lyricsFirstLineActive = !g_lyricsFirstLineActive;
        if (songChanged) {
            g_lyricsLineA.Opacity(0.0);
            g_lyricsLineB.Opacity(0.0);
        }
        arriving.Text(winrt::hstring(nextLine));
        arriving.Opacity(nextLine.empty() ? 0.0 : 1.0);
        // Set *settled* base values; storyboards animate From -> To over them.
        // When a storyboard is stopped/replaced, old lines must not flash back.
        departing.Opacity(0.0);
        double distance = (double)g_settings.lyricsSlidePixels;
        auto departingTransform = departing.RenderTransform().try_as<TranslateTransform>();
        auto arrivingTransform = arriving.RenderTransform().try_as<TranslateTransform>();
        if (departingTransform) departingTransform.Y(-distance);
        if (arrivingTransform) arrivingTransform.Y(0.0);
        Storyboard sb;
        auto easing = CubicEase();
        easing.EasingMode(EasingMode::EaseOut);
        auto addAnimation = [&](TextBlock const& block, const wchar_t* prop,
                                double start, double finish, bool ease) {
            DoubleAnimation anim;
            anim.From(start);
            anim.To(finish);
            anim.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(g_settings.lyricsAnimationMs)));
            if (ease) anim.EasingFunction(easing);
            Storyboard::SetTarget(anim, block);
            Storyboard::SetTargetProperty(anim, prop);
            sb.Children().Append(anim);
        };
        if (hadLine) {
            addAnimation(departing, L"(UIElement.RenderTransform).(TranslateTransform.Y)", 0, -distance, true);
            addAnimation(departing, L"Opacity", 1.0, 0.0, false);
        }
        if (!nextLine.empty()) {
            addAnimation(arriving, L"(UIElement.RenderTransform).(TranslateTransform.Y)", distance, 0, true);
            addAnimation(arriving, L"Opacity", 0.0, 1.0, false);
        }
        if (!ReduceMotionForPower() && g_settings.lyricsAnimationMs > 0) {
            g_lyricsStoryboard = sb;
            sb.Begin();
        }
    } catch (...) {
        try {
            g_lyricsLineA.Opacity(g_lyricsFirstLineActive ? 1.0 : 0.0);
            g_lyricsLineB.Opacity(g_lyricsFirstLineActive ? 0.0 : 1.0);
        } catch (...) {}
    }
}

static std::wstring g_lastTitleArtistKey;
static uint64_t     g_suspectArtSize   = 0;
static uint64_t     g_suspectArtHash   = 0;
static bool         g_artDelayPending  = false;
static bool         g_artNewBrowserSession = false;
enum class RepeatMode {
    Off = 0,
    All = 1,
    One = 2,
};
static std::atomic<bool> g_shuffleEnabled{false};
static std::atomic<RepeatMode> g_repeatMode{RepeatMode::Off};
static std::wstring g_cachedAlbumTitle;
static std::wstring g_cachedAlbumArtist;
static std::vector<BYTE> g_cachedThumbnailBytes;
static std::atomic<int> g_cachedAppIconSize{-1};
static std::wstring g_scrollCachedTitle;
static std::wstring g_scrollCachedArtist;
struct BlurBgCache {
    std::vector<BYTE>  blurredPixels;
    int                width  = 0;
    int                height = 0;
    size_t             artHash = 0;
    void Invalidate() {
        blurredPixels.clear();
        width = height = 0;
        artHash = 0;
    }
} g_blurBgCache;
struct AlbumPalette {
    winrt::Windows::UI::Color primary;
    winrt::Windows::UI::Color secondary;
};
static AlbumPalette g_cachedAlbumPalette = {
    winrt::Windows::UI::Color{255, 18, 18, 18},
    winrt::Windows::UI::Color{255, 45, 45, 45}
};
static size_t g_cachedPaletteHash = 0;
static AlbumPalette g_songColorFrom = g_cachedAlbumPalette;
static std::chrono::steady_clock::time_point g_songColorTransitionAt{};
static bool g_songColorTransitionActive = false;
static winrt::Windows::UI::Color g_cachedVizBaseColor{255, 255, 255, 255};
static bool g_vizBaseColorDirty = true;
static winrt::Windows::UI::Color g_cachedVizPal0{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizPal1{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizCg0{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizCg1{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizAcrCol{255, 255, 255, 255};
static bool g_vizPaletteColorsDirty = true;
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSessionManager g_sessionMgr     = nullptr;
static bool g_userSwitchedSession = false;
static std::atomic<bool> g_forceSessionRefresh{false};
static std::atomic<int> g_sessionCount{0};
static winrt::event_token g_evSessionsChanged{};
static winrt::event_token g_evCurrentChanged{};
static winrt::event_token g_evMediaProps{};
static winrt::event_token g_evPlayback{};
static winrt::event_token g_evTimeline{};
static HANDLE g_mediaThread    = nullptr;
static HANDLE g_mediaStopEvent = nullptr;
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
static winrt::Windows::UI::Color GetSystemButtonBorderColor(std::wstring const& mode) {
    if (IsHoverLightTheme(mode)) {
        return winrt::Windows::UI::Color{0x08, 0x00, 0x00, 0x00};
    }
    return winrt::Windows::UI::Color{0x14, 0xFF, 0xFF, 0xFF};
}
static winrt::Windows::UI::Color GetSystemButtonBorderPressedColor(std::wstring const& mode) {
    if (IsHoverLightTheme(mode)) {
        return winrt::Windows::UI::Color{0x05, 0x00, 0x00, 0x00};
    }
    return winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF};
}
[[clang::no_destroy]] static SolidColorBrush g_mediaHoverBrush   = nullptr;
[[clang::no_destroy]] static SolidColorBrush g_mediaPressedBrush = nullptr;
[[clang::no_destroy]] static SolidColorBrush g_playerHoverBrush   = nullptr;
[[clang::no_destroy]] static SolidColorBrush g_playerPressedBrush = nullptr;
[[clang::no_destroy]] static SolidColorBrush g_playerBorderBrush  = nullptr;
[[clang::no_destroy]] static SolidColorBrush g_playerBorderPressedBrush = nullptr;
static void EnsureHoverBrushes() {
    if (!g_mediaHoverBrush) {
        g_mediaHoverBrush   = SolidColorBrush(GetSystemButtonHoverColor(g_settings.mediaButtonsHoverEffectMode));
        g_mediaPressedBrush = SolidColorBrush(GetSystemButtonPressedColor(g_settings.mediaButtonsHoverEffectMode));
    }
    if (!g_playerHoverBrush) {
        g_playerHoverBrush   = SolidColorBrush(GetSystemButtonHoverColor(g_settings.playerHoverEffectMode));
        g_playerPressedBrush = SolidColorBrush(GetSystemButtonPressedColor(g_settings.playerHoverEffectMode));
        g_playerBorderBrush  = SolidColorBrush(GetSystemButtonBorderColor(g_settings.playerHoverEffectMode));
        g_playerBorderPressedBrush = SolidColorBrush(GetSystemButtonBorderPressedColor(g_settings.playerHoverEffectMode));
    }
}
static void UpdateHoverBrushColors() {
    if (g_mediaHoverBrush) {
        try { g_mediaHoverBrush.Color(GetSystemButtonHoverColor(g_settings.mediaButtonsHoverEffectMode)); } catch (...) {}
    } else {
        try { g_mediaHoverBrush = SolidColorBrush(GetSystemButtonHoverColor(g_settings.mediaButtonsHoverEffectMode)); } catch (...) {}
    }
    if (g_mediaPressedBrush) {
        try { g_mediaPressedBrush.Color(GetSystemButtonPressedColor(g_settings.mediaButtonsHoverEffectMode)); } catch (...) {}
    } else {
        try { g_mediaPressedBrush = SolidColorBrush(GetSystemButtonPressedColor(g_settings.mediaButtonsHoverEffectMode)); } catch (...) {}
    }
    if (g_playerHoverBrush) {
        try { g_playerHoverBrush.Color(GetSystemButtonHoverColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    } else {
        try { g_playerHoverBrush = SolidColorBrush(GetSystemButtonHoverColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    }
    if (g_playerPressedBrush) {
        try { g_playerPressedBrush.Color(GetSystemButtonPressedColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    } else {
        try { g_playerPressedBrush = SolidColorBrush(GetSystemButtonPressedColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    }
    if (g_playerBorderBrush) {
        try { g_playerBorderBrush.Color(GetSystemButtonBorderColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    } else {
        try { g_playerBorderBrush = SolidColorBrush(GetSystemButtonBorderColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    }
    if (g_playerBorderPressedBrush) {
        try { g_playerBorderPressedBrush.Color(GetSystemButtonBorderPressedColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    } else {
        try { g_playerBorderPressedBrush = SolidColorBrush(GetSystemButtonBorderPressedColor(g_settings.playerHoverEffectMode)); } catch (...) {}
    }
}
static Border GetButtonTemplateRoot(Button const& btn) {
    if (!btn) return nullptr;
    try {
        btn.ApplyTemplate();
        if (VisualTreeHelper::GetChildrenCount(btn) > 0) {
            if (auto border = VisualTreeHelper::GetChild(btn, 0).try_as<Border>())
                return border;
        }
        if (auto named = btn.FindName(L"Root")) {
            if (auto border = named.try_as<Border>()) return border;
        }
    } catch (...) {}
    return nullptr;
}

[[clang::no_destroy]] static Style g_fluentMediaButtonStyle = nullptr;
static Style GetFluentMediaButtonStyle() {
    if (!g_fluentMediaButtonStyle) {
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
        <Border.BackgroundTransition>
            <BrushTransition Duration="0:0:0.083"/>
        </Border.BackgroundTransition>
        <VisualStateManager.VisualStateGroups>
            <VisualStateGroup x:Name="CommonStates">
            <VisualState x:Name="Normal">
                <VisualState.Setters>
                    <Setter Target="Root.Background" Value="Transparent"/>
                    <Setter Target="Root.BorderBrush" Value="Transparent"/>
                </VisualState.Setters>
            </VisualState>
            <VisualState x:Name="PointerOver">
                <VisualState.Setters>
                    <Setter Target="Root.Background" Value="Transparent"/>
                    <Setter Target="Root.BorderBrush" Value="Transparent"/>
                </VisualState.Setters>
            </VisualState>
            <VisualState x:Name="Pressed">
                <VisualState.Setters>
                    <Setter Target="Root.Background" Value="Transparent"/>
                    <Setter Target="Root.BorderBrush" Value="Transparent"/>
                </VisualState.Setters>
            </VisualState>
            <VisualState x:Name="Disabled">
                <VisualState.Setters>
                    <Setter Target="Root.Background" Value="Transparent"/>
                    <Setter Target="Root.BorderBrush" Value="Transparent"/>
                </VisualState.Setters>
            </VisualState>
            </VisualStateGroup>
        </VisualStateManager.VisualStateGroups>
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
            g_fluentMediaButtonStyle = winrt::Windows::UI::Xaml::Markup::XamlReader::Load(
                winrt::hstring(kStyleXaml)).as<Style>();
        } catch (...) {}
    }
    return g_fluentMediaButtonStyle;
}
static void ApplyFluentMediaButtonStyle(Button const& btn) {
    if (auto style = GetFluentMediaButtonStyle()) {
        btn.Style(style);
    }
}

static VisualState FindCommonState(Border const& root, std::wstring const& stateName) {
    if (!root) return nullptr;
    try {
        auto groups = VisualStateManager::GetVisualStateGroups(root);
        for (auto const& g : groups) {
            if (g.Name() == L"CommonStates") {
                for (auto const& s : g.States()) {
                    if (s.Name() == stateName) return s;
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

static void SetStateBrush(Border const& root, std::wstring const& stateName, uint32_t setterIndex, Brush const& value) {
    if (!root) return;
    try {
        auto state = FindCommonState(root, stateName);
        if (!state) return;
        auto setters = state.Setters();
        if (setterIndex >= setters.Size()) return;
        if (auto setter = setters.GetAt(setterIndex).try_as<Setter>()) {
            setter.Value(value);
        }
    } catch (...) {}
}
static void ApplyHoverTransitionSetting(Border const& root) {
    if (!root) return;
    try {
        if (g_settings.enableHoverAnimation) {
            BrushTransition bt;
            bt.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(83)));
            root.BackgroundTransition(bt);
        } else {
            root.BackgroundTransition(nullptr);
        }
    } catch (...) {}
}
static void SetupCommonStates(
    Button const& btn,
    Brush const& normalBackground,
    Brush const& pointerOverBackground,
    Brush const& pressedBackground,
    Brush const& disabledBackground,
    Brush const& normalBorderBrush,
    Brush const& pointerOverBorderBrush,
    Brush const& pressedBorderBrush,
    Brush const& disabledBorderBrush)
{
    auto root = GetButtonTemplateRoot(btn);
    if (!root) return;
    ApplyHoverTransitionSetting(root);
    SetStateBrush(root, L"Normal",      0, normalBackground);
    SetStateBrush(root, L"Normal",      1, normalBorderBrush);
    SetStateBrush(root, L"PointerOver", 0, pointerOverBackground);
    SetStateBrush(root, L"PointerOver", 1, pointerOverBorderBrush);
    SetStateBrush(root, L"Pressed",     0, pressedBackground);
    SetStateBrush(root, L"Pressed",     1, pressedBorderBrush);
    SetStateBrush(root, L"Disabled",    0, disabledBackground);
    SetStateBrush(root, L"Disabled",    1, disabledBorderBrush);
    try {
        root.Background(normalBackground);
        root.BorderBrush(normalBorderBrush);
    } catch (...) {}
}
static void GoToCommonState(Button const& btn, bool effectEnabled, bool pressed, bool hovered) {
    if (!btn) return;
    try {
        winrt::hstring stateName = L"Normal";
        if (effectEnabled) {
            if (pressed)      stateName = L"Pressed";
            else if (hovered) stateName = L"PointerOver";
        }
        if (pressed && g_settings.enableHoverAnimation) {
            if (auto root = GetButtonTemplateRoot(btn)) {
                auto saved = root.BackgroundTransition();
                root.BackgroundTransition(nullptr);
                VisualStateManager::GoToState(btn, stateName, true);
                root.BackgroundTransition(saved);
                return;
            }
        }
        VisualStateManager::GoToState(btn, stateName, true);
    } catch (...) {}
}
// Removed on the XAML thread before detaching the player tree.
static std::vector<std::function<void()>> g_uiCleanup;
static void RunWhenButtonReady(Button const& btn, std::function<void()> const& action) {
    if (!btn || !action) return;
    auto run = std::make_shared<std::function<void()>>(action);
    auto weak = winrt::make_weak(btn);
    auto invoke = [weak, run]() {
        try {
            if (auto button = weak.get()) {
                button.ApplyTemplate();
                auto once = std::move(*run);
                *run = {};
                if (once) once();
            }
        } catch (...) { *run = {}; }
    };
    try {
        if (btn.IsLoaded()) invoke();
        else {
            auto token = std::make_shared<winrt::event_token>();
            *token = btn.Loaded([weak, token, invoke](auto const&, auto const&) {
                if (auto button = weak.get()) button.Loaded(*token);
                invoke();
            });
            g_uiCleanup.push_back([weak, token, run] {
                if (auto button = weak.get()) { try { button.Loaded(*token); } catch (...) {} }
                *run = {};
            });
        }
    } catch (...) { invoke(); }
}

static void SetupMediaButtonCommonStates(Button const& btn) {
    auto transparent = MakeBrush({0x00, 0xFF, 0xFF, 0xFF});
    if (!IsHoverEffectEnabled(g_settings.mediaButtonsHoverEffectMode)) {
        SetupCommonStates(
            btn,
            transparent, transparent, transparent, transparent,
            transparent, transparent, transparent, transparent);
        return;
    }
    EnsureHoverBrushes();
    SetupCommonStates(
        btn,
        transparent,
        g_mediaHoverBrush,
        g_mediaPressedBrush,
        transparent,
        transparent, transparent, transparent, transparent);
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
static void SetupPlayerCommonStates(Button const& btn, Brush const& normalBg) {
    auto transparent = MakeBrush({0x00, 0xFF, 0xFF, 0xFF});
    if (!IsHoverEffectEnabled(g_settings.playerHoverEffectMode)) {
        SetupCommonStates(
            btn,
            normalBg, normalBg, normalBg, normalBg,
            transparent, transparent, transparent, transparent);
        return;
    }
    EnsureHoverBrushes();
    auto elevBorder        = MakeElevationBorderBrush(g_settings.playerHoverEffectMode);
    auto elevBorderPressed = MakeBrush(GetSystemButtonBorderPressedColor(g_settings.playerHoverEffectMode));
    SetupCommonStates(
        btn,
        normalBg,
        g_playerHoverBrush,
        g_playerPressedBrush,
        normalBg,
        transparent,
        elevBorder,
        elevBorderPressed,
        transparent);
}
static void ApplyPlayerButtonState(Button const& btn, Brush const& /*normalBg*/, bool hovered, bool pressed) {
    if (!btn) return;
    try {
        GoToCommonState(btn, IsHoverEffectEnabled(g_settings.playerHoverEffectMode), pressed, hovered);
    } catch (...) {}
}
static bool DecodeImageToBGRA(const std::vector<BYTE>& imgBytes,
                            std::vector<BYTE>& outPixels,
                            int& outW, int& outH)
{
    if (imgBytes.empty()) return false;
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
            IWICFormatConverter* pConv = nullptr;
            if (SUCCEEDED(pFactory->CreateFormatConverter(&pConv))) {
                UINT originalW = 0, originalH = 0;
                bool allowed = SUCCEEDED(pFrame->GetSize(&originalW, &originalH)) && originalW > 0 && originalH > 0 && originalW <= 32768 && originalH <= 32768;
                IWICBitmapSource* analysisSource = pFrame;
                winrt::com_ptr<IWICBitmapScaler> scaled;
                if (allowed && std::max(originalW, originalH) > 256) {
                    const double ratio = 256.0 / std::max(originalW, originalH);
                    allowed = SUCCEEDED(pFactory->CreateBitmapScaler(scaled.put())) &&
                        SUCCEEDED(scaled->Initialize(pFrame, std::max(1U, (UINT)std::lround(originalW * ratio)),
                            std::max(1U, (UINT)std::lround(originalH * ratio)), WICBitmapInterpolationModeFant));
                    if (allowed) analysisSource = scaled.get();
                }
                if (allowed && SUCCEEDED(pConv->Initialize(
                        analysisSource, GUID_WICPixelFormat32bppBGRA,
                        WICBitmapDitherTypeNone, nullptr, 0.0,
                        WICBitmapPaletteTypeMedianCut))) {
                    UINT w = 0, h = 0;
                    pConv->GetSize(&w, &h);
                    if (w > 0 && h > 0) {
                        outPixels.resize((size_t)w * h * 4);
                        if (SUCCEEDED(pConv->CopyPixels(nullptr, w * 4,
                                (UINT)outPixels.size(), outPixels.data()))) {
                            outW = (int)w; outH = (int)h; ok = true;
                        }
                    }
                }
                pConv->Release();
            }
            pFrame->Release();
        }
        pDecoder->Release();
    }
    pStream->Release();
    pFactory->Release();
    return ok;
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
static bool UpdateAlbumBlurBgCache(const std::vector<BYTE>& thumbBytes,
                                    int targetW, int targetH)
{
    if (thumbBytes.empty() || targetW <= 0 || targetH <= 0) {
        g_blurBgCache.Invalidate(); return false;
    }
    size_t artHash = 0;
    for (size_t i = 0; i < thumbBytes.size(); i += 512)
        artHash = artHash * 31 + thumbBytes[i];
    if (g_blurBgCache.artHash == artHash && g_blurBgCache.width == targetW &&
        g_blurBgCache.height == targetH && !g_blurBgCache.blurredPixels.empty())
        return true;
    std::vector<BYTE> srcPixels; int srcW = 0, srcH = 0;
    if (!DecodeImageToBGRA(thumbBytes, srcPixels, srcW, srcH)) return false;
    int blurDiv = 8;
    int smallW = srcW / blurDiv; if (smallW < 1) smallW = 1;
    int smallH = srcH / blurDiv; if (smallH < 1) smallH = 1;
    std::vector<BYTE> small;
    DownsampleBGRA(srcPixels, srcW, srcH, small, smallW, smallH);
    int blurRadius = std::clamp(g_settings.blurRadius, 1, 50);
    for (int i = 0; i < 3; ++i) {
        ApplyBoxBlurBGRA(small, smallW, smallH, blurRadius);
    }
    std::vector<BYTE> blurred;
    DownsampleBGRA(small, smallW, smallH, blurred, targetW, targetH);
    g_blurBgCache.blurredPixels = std::move(blurred);
    g_blurBgCache.width   = targetW;
    g_blurBgCache.height  = targetH;
    g_blurBgCache.artHash = artHash;
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
                if (g_cachedPaletteHash == 0) {
                    return winrt::Windows::UI::Color{0, 255, 255, 255};
                }
                return winrt::Windows::UI::Color{alpha,
                    g_cachedAlbumPalette.primary.R,
                    g_cachedAlbumPalette.primary.G,
                    g_cachedAlbumPalette.primary.B};
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
    BYTE alpha = (BYTE)((g_settings.titleColorOpacity / 100.0) * 255);
    return ParseColorWithThemeSupport(g_settings.titleColor, alpha);
}
static winrt::Windows::UI::Color ArtistColor() {
    BYTE alpha = (BYTE)((g_settings.artistColorOpacity / 100.0) * 255);
    return ParseColorWithThemeSupport(g_settings.artistColor, alpha);
}
static winrt::Windows::UI::Color ButtonColor() {
    BYTE alpha = (BYTE)((g_settings.buttonColorOpacity / 100.0) * 255);
    return ParseColorWithThemeSupport(g_settings.buttonColor, alpha);
}
static winrt::Windows::UI::Color ContextMenuIconColor() {
    BYTE alpha = (BYTE)((g_settings.contextMenuIconOpacity / 100.0) * 255);
    const std::wstring& clr = g_settings.contextMenuIconColor;
    if (clr.empty()) {
        return ParseColorWithThemeSupport(g_settings.buttonColor, alpha);
    }
    return ParseColorWithThemeSupport(clr, alpha);
}
static const std::wstring& ContextMenuIconStyle() {
    if (g_settings.contextMenuIconStyle == L"as_media_buttons")
        return g_settings.iconStyle;
    return g_settings.contextMenuIconStyle;
}
static Brush MakeAlbumBlurBrush(const std::vector<BYTE>& thumbBytes,
                                int panelW, int panelH)
{
    if (!UpdateAlbumBlurBgCache(thumbBytes, panelW, panelH) ||
        g_blurBgCache.blurredPixels.empty())
        return MakeBrush({0x00, 0x00, 0x00, 0x00});
    try {
        size_t bytesNeeded = (size_t)panelW * panelH * 4;
        WriteableBitmap wb(panelW, panelH);
        auto buf = wb.PixelBuffer();
        auto byteAccess = buf.as<Windows::Storage::Streams::IBufferByteAccess>();
        BYTE* pixels = nullptr;
        byteAccess->Buffer(&pixels);
        if (pixels && g_blurBgCache.blurredPixels.size() >= bytesNeeded)
            memcpy(pixels, g_blurBgCache.blurredPixels.data(), bytesNeeded);
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
    auto& t = g_settings.backgroundType;
    BYTE opacity = (BYTE)((g_settings.solidOpacity / 100.0) * 255);
    auto color1 = ParseColorWithThemeSupport(g_settings.solidColor, opacity);
    auto color2 = ParseColorWithSpecialValues(g_settings.solidColor2, opacity);
    auto gradientColor2 = ParseColorWithSpecialValues(g_settings.gradientColor2, opacity);
    if (t == L"gradient") {
        try {
            winrt::Windows::UI::Xaml::Media::LinearGradientBrush brush;
            double angleRad = (g_settings.gradientAngle % 360) * 3.14159265358979323846 / 180.0;
            double startX = 0.5 - 0.5 * std::cos(angleRad);
            double startY = 0.5 - 0.5 * std::sin(angleRad);
            double endX = 0.5 + 0.5 * std::cos(angleRad);
            double endY = 0.5 + 0.5 * std::sin(angleRad);
            brush.StartPoint(winrt::Windows::Foundation::Point((float)startX, (float)startY));
            brush.EndPoint(winrt::Windows::Foundation::Point((float)endX, (float)endY));
            double balancePoint = std::clamp(g_settings.gradientBalance, 0, 100) / 100.0;
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
            brush.TintOpacity(g_settings.acrylicTintOpacity / 100.0);
            brush.FallbackColor(winrt::Windows::UI::Color{0xCC, color1.R, color1.G, color1.B});
            return brush;
        } catch (...) {}
    }
    if (t == L"mica" || t == L"mica_alt") {
        BYTE micaAlpha = (BYTE)((g_settings.micaOpacity / 100.0) * 255);
        auto col = winrt::Windows::UI::Color{micaAlpha, color1.R, color1.G, color1.B};
        return MakeBrush(col);
    }
    if (t == L"solid") {
        return MakeBrush(color1);
    }
    if (t == L"album_art_blur") {
        return MakeBrush({0x00, 0xFF, 0xFF, 0xFF});
    }
    return MakeBrush({0x00, 0xFF, 0xFF, 0xFF});
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
static constexpr wchar_t kGridName[]        = L"FluentMediaBar";
static constexpr wchar_t kArtImageName[]    = L"FluentMedia_Art";
static constexpr wchar_t kAppIconImageName[]= L"FluentMedia_AppIcon";
static constexpr wchar_t kAnchorOverlayName[]= L"FluentMedia_DebugAnchorTarget";
static constexpr wchar_t kTextStackName[]   = L"FluentMedia_TextStack";
static constexpr wchar_t kTitleBlockName[]  = L"FluentMedia_Title";
static constexpr wchar_t kArtistBlockName[] = L"FluentMedia_Artist";
static constexpr wchar_t kPlayBtnName[]     = L"FluentMedia_Play";
static constexpr wchar_t kPrevBtnName[]     = L"FluentMedia_Prev";
static constexpr wchar_t kNextBtnName[]     = L"FluentMedia_Next";
static constexpr wchar_t kRewindBtnName[]   = L"FluentMedia_Rewind";
static constexpr wchar_t kForwardBtnName[]  = L"FluentMedia_Forward";
static constexpr wchar_t kShuffleBtnName[]  = L"FluentMedia_Shuffle";
static constexpr wchar_t kRepeatBtnName[]   = L"FluentMedia_Repeat";
static constexpr wchar_t kSwitchSessionsBtnName[] = L"FluentMedia_SwitchSessions";
static int  g_idleSeconds  = 0;
static int  g_idleTicks    = 0;
static std::atomic<bool> g_hiddenByIdle{false};
static std::chrono::steady_clock::time_point g_lastMediaTime = std::chrono::steady_clock::now();
static void SwitchMediaSession();
static void FetchPlaybackInfoAsync();
static winrt::Windows::Foundation::TimeSpan CurrentPlaybackPosition(GlobalSystemMediaTransportControlsSession const& session) {
    auto timeline = session.GetTimelineProperties();
    auto position = timeline.Position();
    auto info = session.GetPlaybackInfo();
    if (info && info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
        auto elapsed = winrt::clock::now() - timeline.LastUpdatedTime();
        if (elapsed.count() > 0 && elapsed < std::chrono::hours(6)) {
            double rate = info.PlaybackRate() ? info.PlaybackRate().Value() : 1.0;
            if (std::isfinite(rate)) position += winrt::Windows::Foundation::TimeSpan{(int64_t)(elapsed.count() * rate)};
        }
    }
    auto end = timeline.EndTime();
    position = std::max(position, timeline.StartTime());
    if (end > timeline.StartTime()) position = std::min(position, end);
    return position;
}
static void SendMediaCommandAsync(int cmd) {
    GlobalSystemMediaTransportControlsSession requestedSession{nullptr};
    { std::lock_guard<std::mutex> lock(g_sessionMtx); requestedSession = g_currentSession; }
    SpawnTrackedWorker([cmd, requestedSession]() {
        if (g_unloading) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            auto session = requestedSession;
            if (cmd == 9) { SwitchMediaSession(); winrt::uninit_apartment(); return; }
            if (session) {
                switch (cmd) {
                    case 1: session.TrySkipPreviousAsync().get();    break;
                    case 2: {
                        auto info = session.GetPlaybackInfo();
                        bool isPlaying = info &&
                            info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                        bool accepted = false;
                        if (info && info.Controls().IsPlayPauseToggleEnabled()) accepted = session.TryTogglePlayPauseAsync().get();
                        else if (isPlaying) accepted = session.TryPauseAsync().get();
                        else accepted = session.TryPlayAsync().get();
                        if (!accepted) Wh_Log(L"The media app rejected play / pause");
                        break;
                    }
                    case 3: session.TrySkipNextAsync().get();        break;
                    case 4:
                        try {
                            auto result = session.TryStopAsync().get();
                            if (!result) {
                                session.TryPauseAsync().get();
                                session.TryChangePlaybackPositionAsync(0).get();
                            }
                        } catch (...) {
                            try {
                                session.TryPauseAsync().get();
                                session.TryChangePlaybackPositionAsync(0).get();
                            } catch (...) {
                                Wh_Log(L"SendMediaCommandAsync: Fallback also failed");
                            }
                        }
                        break;
                    case 5:
                        try {
                            auto timeline = session.GetTimelineProperties();
                            auto currentPos = CurrentPlaybackPosition(session);
                            auto newPos = currentPos - std::chrono::seconds(5);
                            if (newPos < timeline.StartTime()) newPos = timeline.StartTime();
                            session.TryChangePlaybackPositionAsync(newPos.count()).get();
                        } catch (...) {}
                        break;
                    case 6:
                        try {
                            auto timeline = session.GetTimelineProperties();
                            auto currentPos = CurrentPlaybackPosition(session);
                            auto endTime = timeline.EndTime();
                            auto newPos = currentPos + std::chrono::seconds(5);
                            if (endTime > timeline.StartTime() && newPos > endTime) newPos = endTime;
                            session.TryChangePlaybackPositionAsync(newPos.count()).get();
                        } catch (...) {}
                        break;
                    case 7:
                        try {
                            bool currentShuffle = g_shuffleEnabled.load();
                            if (session.TryChangeShuffleActiveAsync(!currentShuffle).get()) {
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
                                    mode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List;
                                    next = RepeatMode::All;
                                    break;
                                case RepeatMode::All:
                                    mode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track;
                                    next = RepeatMode::One;
                                    break;
                                case RepeatMode::One:
                                default:
                                    mode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                                    next = RepeatMode::Off;
                                    break;
                            }
                            if (session.TryChangeAutoRepeatModeAsync(mode).get()) {
                                g_repeatMode = next;
                                DispatchMediaUpdate();
                            }
                        } catch (...) {}
                        break;
                    case 9:
                        SwitchMediaSession();
                        break;
                    case 10:
                        try {
                            if (session.TryChangeAutoRepeatModeAsync(winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None).get()) {
                                g_repeatMode = RepeatMode::Off;
                                DispatchMediaUpdate();
                            }
                        } catch (...) {}
                        break;
                    case 11:
                        try {
                            if (session.TryChangeAutoRepeatModeAsync(winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List).get()) {
                                g_repeatMode = RepeatMode::All;
                                DispatchMediaUpdate();
                            }
                        } catch (...) {}
                        break;
                    case 12:
                        try {
                            if (session.TryChangeAutoRepeatModeAsync(winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track).get()) {
                                g_repeatMode = RepeatMode::One;
                                DispatchMediaUpdate();
                            }
                        } catch (...) {}
                        break;
                }
            }
        } catch (...) { Wh_Log(L"Media command %d failed", cmd); }
        g_timelineDirty = true;
        if (!g_unloading) FetchPlaybackInfoAsync();
        winrt::uninit_apartment();
    });
}
struct TextScrollState {
    double offset    = 0.0;
    double textWidth = 0.0;
    double viewWidth = 0.0;
    bool   forward   = true;
    bool   active    = false;
    bool   pausing   = false;
    int    pauseTick = 0;
    int    tickMs    = 33;
};
static TextScrollState g_titleScroll;
static TextScrollState g_artistScroll;
static void ResetScrollState(TextScrollState& s);
static void FetchMediaPropertiesAsync();
static void FetchPlaybackInfoAsync();
static void OnSessionsChanged();
static void AttachToSession(GlobalSystemMediaTransportControlsSession session);
static void SwitchMediaSession();
static void SwitchMediaSession() {
    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    GlobalSystemMediaTransportControlsSession curSession{nullptr};
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        if (!g_sessionMgr) return;
        mgr = g_sessionMgr;
        curSession = g_currentSession;
        g_userSwitchedSession = true;
    }
    if (!mgr) return;
    GlobalSystemMediaTransportControlsSession nextSession{nullptr};
    try {
        auto sessions = mgr.GetSessions();
        int count = (int)sessions.Size();
        if (count <= 1) return;
        int currentIndex = -1;
        if (curSession) {
            auto curId = curSession.SourceAppUserModelId();
            for (int i = 0; i < count; ++i) {
                if (sessions.GetAt(i).SourceAppUserModelId() == curId) {
                    currentIndex = i;
                    break;
                }
            }
        }
        int nextIndex = (currentIndex + 1) % count;
        nextSession = sessions.GetAt(nextIndex);
    } catch (...) { return; }
    if (nextSession) {
        AttachToSession(nextSession);
    }
}
static std::mutex g_deviceEnumeratorMtx;
static void InitAudioDeviceEnumerator() {
    std::lock_guard<std::mutex> lk(g_deviceEnumeratorMtx);
    if (!g_pDeviceEnumerator) {
        CoCreateInstance(
            XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
            XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    }
}
static void CleanupAudioDeviceEnumerator() {
    std::lock_guard<std::mutex> lk(g_deviceEnumeratorMtx);
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = nullptr;
    }
}
static bool AdjustAppVolumeByAUMID(const std::wstring& aumid, float volumeDelta) {
    InitAudioDeviceEnumerator();
    std::lock_guard<std::mutex> lk(g_deviceEnumeratorMtx);
    if (!g_pDeviceEnumerator) return false;
    winrt::com_ptr<IMMDevice> defaultDevice;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, defaultDevice.put());
    if (FAILED(hr)) return false;
    winrt::com_ptr<IAudioSessionManager2> sessionManager;
    hr = defaultDevice->Activate(XIID_IAudioSessionManager2, CLSCTX_ALL, NULL, sessionManager.put_void());
    if (FAILED(hr)) return false;
    winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
    hr = sessionManager->GetSessionEnumerator(sessionEnumerator.put());
    if (FAILED(hr)) return false;
    int sessionCount = 0;
    hr = sessionEnumerator->GetCount(&sessionCount);
    if (FAILED(hr)) return false;
    bool found = false;
    for (int i = 0; i < sessionCount; i++) {
        winrt::com_ptr<IAudioSessionControl> sessionControl;
        hr = sessionEnumerator->GetSession(i, sessionControl.put());
        if (FAILED(hr)) continue;
        winrt::com_ptr<IAudioSessionControl2> sessionControl2;
        hr = sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), sessionControl2.put_void());
        if (FAILED(hr)) continue;
        LPWSTR sessionId = nullptr;
        hr = sessionControl2->GetSessionIdentifier(&sessionId);
        if (SUCCEEDED(hr) && sessionId) {
            std::wstring sessionIdStr(sessionId);
            CoTaskMemFree(sessionId);
            if (sessionIdStr.find(aumid) != std::wstring::npos) {
                winrt::com_ptr<ISimpleAudioVolume> vol;
                if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), vol.put_void()))) {
                    float currentVolume = 0.0f;
                    if (SUCCEEDED(vol->GetMasterVolume(&currentVolume))) {
                        float newVolume = std::clamp(currentVolume + volumeDelta, 0.0f, 1.0f);
                        vol->SetMasterVolume(newVolume, NULL);
                        found = true;
                    }
                }
            }
        }
    }
    return found;
}
static void ChangeSystemVolume(bool increase) {
    InitAudioDeviceEnumerator();
    std::lock_guard<std::mutex> lk(g_deviceEnumeratorMtx);
    if (!g_pDeviceEnumerator) return;
    winrt::com_ptr<IMMDevice> defaultDevice;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, defaultDevice.put());
    if (FAILED(hr)) return;
    winrt::com_ptr<IAudioEndpointVolume> endpointVolume;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, endpointVolume.put_void());
    if (FAILED(hr)) return;
    float currentVolume = 0.0f;
    if (SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(&currentVolume))) {
        float newVolume = currentVolume + (increase ? 0.005f : -0.005f);
        newVolume = std::clamp(newVolume, 0.0f, 1.0f);
        endpointVolume->SetMasterVolumeLevelScalar(newVolume, NULL);
    }
    HWND hShellTrayWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hShellTrayWnd) {
        SHORT appCommand = increase ? APPCOMMAND_VOLUME_UP : APPCOMMAND_VOLUME_DOWN;
        PostMessage(hShellTrayWnd, WM_APPCOMMAND, (WPARAM)hShellTrayWnd,
                    MAKELPARAM(0, appCommand));
    }
}
static std::wstring ToLowerCopy(std::wstring value);
static std::wstring GetWindowAppUserModelId(HWND hWnd);
static void ShowMediaContextMenu(FrameworkElement const& target);
static void ShowMiniPlayerFlyout(FrameworkElement const& target);
[[maybe_unused]]
static void ExecuteMediaAction(const std::wstring& action, FrameworkElement const& sourceElement = nullptr) {
    if (action == L"none") {
        return;
    } else if (action == L"open_context_menu") {
        if (sourceElement) {
            ShowMediaContextMenu(sourceElement);
        }
        return;
    } else if (action == L"open_mini_player") {
        if (sourceElement) {
            ShowMiniPlayerFlyout(sourceElement);
        }
        return;
    } else if (action == L"switch_session") {
        SwitchMediaSession();
    } else if (action == L"play_pause") {
        SendMediaCommandAsync(2);
        DispatchMediaUpdate();
    } else if (action == L"next_track") {
        SendMediaCommandAsync(3);
        DispatchMediaUpdate();
    } else if (action == L"prev_track") {
        SendMediaCommandAsync(1);
        DispatchMediaUpdate();
    } else if (action == L"stop") {
        g_forceSessionRefresh = true;
        SendMediaCommandAsync(4);
        DispatchMediaUpdate();
        SpawnTrackedWorker([]() {
            for (DWORD delay : {300, 1200, 2500}) {
                Sleep(delay);
                if (g_unloading) return;
                g_forceSessionRefresh = true;
                OnSessionsChanged();
                FetchPlaybackInfoAsync();
                FetchMediaPropertiesAsync();
            }
        });
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
        SpawnTrackedWorker([]() {
            std::wstring title, appAumid;
            {
                std::lock_guard<std::mutex> lk(g_mediaMtx);
                title = g_media.title;
            }
            {
                GlobalSystemMediaTransportControlsSession sessionCopy{nullptr};
                {
                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                    sessionCopy = g_currentSession;
                }
                if (sessionCopy) {
                    try {
                        appAumid = std::wstring(sessionCopy.SourceAppUserModelId());
                    } catch (...) {}
                }
            }
            struct WindowSearch {
                std::wstring targetTitle;
                std::wstring targetAumid;
                HWND aumidHwnd = nullptr;
                HWND processHwnd = nullptr;
                HWND titleHwnd = nullptr;
            };
            WindowSearch search;
            search.targetTitle = title;
            std::transform(search.targetTitle.begin(), search.targetTitle.end(), search.targetTitle.begin(), ::towlower);
            search.targetAumid = appAumid;
            std::transform(search.targetAumid.begin(), search.targetAumid.end(), search.targetAumid.begin(), ::towlower);
            EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
                if (!IsWindowVisible(hwnd)) return TRUE;
                WINDOWINFO wi{};
                wi.cbSize = sizeof(wi);
                GetWindowInfo(hwnd, &wi);
                if ((wi.dwStyle & WS_CHILD) != 0) return TRUE;
                auto* s = reinterpret_cast<WindowSearch*>(lParam);
                if (!s->aumidHwnd && !s->targetAumid.empty()) {
                    IPropertyStore* pps = nullptr;
                    if (SUCCEEDED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&pps)))) {
                        PROPVARIANT var;
                        PropVariantInit(&var);
                        if (SUCCEEDED(pps->GetValue(PKEY_AppUserModel_ID, &var)) && var.vt == VT_LPWSTR) {
                            std::wstring winAumid(var.pwszVal);
                            std::transform(winAumid.begin(), winAumid.end(), winAumid.begin(), ::towlower);
                            if (winAumid == s->targetAumid ||
                                winAumid.find(s->targetAumid) != std::wstring::npos ||
                                s->targetAumid.find(winAumid) != std::wstring::npos) {
                                s->aumidHwnd = hwnd;
                            }
                        }
                        PropVariantClear(&var);
                        pps->Release();
                    }
                }
                if (!s->processHwnd && !s->targetAumid.empty()) {
                    DWORD pid = 0;
                    GetWindowThreadProcessId(hwnd, &pid);
                    if (pid) {
                        wchar_t procPath[MAX_PATH]{};
                        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                        if (hProc) {
                            DWORD sz = MAX_PATH;
                            QueryFullProcessImageNameW(hProc, 0, procPath, &sz);
                            CloseHandle(hProc);
                        }
                        if (procPath[0]) {
                            std::wstring stem = procPath;
                            auto slash = stem.find_last_of(L"\\/");
                            if (slash != std::wstring::npos) stem = stem.substr(slash + 1);
                            auto dot = stem.rfind(L'.');
                            if (dot != std::wstring::npos) stem = stem.substr(0, dot);
                            std::transform(stem.begin(), stem.end(), stem.begin(), ::towlower);
                            if (!stem.empty() &&
                                (s->targetAumid.find(stem) != std::wstring::npos ||
                                stem.find(s->targetAumid) != std::wstring::npos)) {
                                s->processHwnd = hwnd;
                            }
                        }
                    }
                }
                if (!s->titleHwnd && !s->targetTitle.empty()) {
                    wchar_t windowTitle[512];
                    if (GetWindowTextW(hwnd, windowTitle, 512) > 0) {
                        std::wstring wTitle(windowTitle);
                        std::transform(wTitle.begin(), wTitle.end(), wTitle.begin(), ::towlower);
                        if (wTitle.find(s->targetTitle) != std::wstring::npos) {
                            s->titleHwnd = hwnd;
                        }
                    }
                }
                return TRUE;
            }, reinterpret_cast<LPARAM>(&search));
            HWND targetWindow = search.aumidHwnd
                ? search.aumidHwnd
                : (search.processHwnd ? search.processHwnd : search.titleHwnd);
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
    }
}
static bool DoubleClickEnabled(const wchar_t* target) {
    return g_settings.doubleClickTarget == target || g_settings.doubleClickTarget == L"both";
}
static bool LyricsInteractive() {
    return DoubleClickEnabled(L"lyrics");
}
static void MovePlayerToNextMonitor();
static void RunDoubleClickAction(FrameworkElement const& source) {
    if (g_unloading || g_applyingSettings) return;
    try {
        if (g_settings.doubleClickAction == L"next_monitor") MovePlayerToNextMonitor();
        else ExecuteMediaAction(g_settings.doubleClickAction, source);
    }
    catch (...) { Wh_Log(L"Double-click action failed"); }
}
static void BindDoubleTap(FrameworkElement const& target) {
    target.IsDoubleTapEnabled(true);
    target.DoubleTapped([](auto const& sender, DoubleTappedRoutedEventArgs const& args) {
        args.Handled(true);
        if (auto source = sender.template try_as<FrameworkElement>()) RunDoubleClickAction(source);
    });
}
static void RunArtworkSingleClick() {
    if (g_unloading || g_applyingSettings) return;
    { std::lock_guard<std::mutex> lock(g_mediaMtx); if (!g_media.canPlayPause) return; }
    SendMediaCommandAsync(2);
}
// The cover uses native Click for keyboard activation and a single one-shot timer.
// Its callback holds only a weak state; cleanup cancels it before removing the UI.
static void BindArtworkPlayback(Button const& button) {
    struct ClickState {
        DoubleClickGesture::State gesture;
        double x = 0, y = 0;
        DispatcherTimer timer{nullptr};
        winrt::event_token token{};
    };
    auto state = std::make_shared<ClickState>();
    if (DoubleClickEnabled(L"album_art")) {
        state->timer = DispatcherTimer();
        state->timer.Interval(std::chrono::milliseconds(GetDoubleClickTime() + 1));
        state->token = state->timer.Tick([weak = std::weak_ptr<ClickState>(state)](auto const&, auto const&) {
            if (auto current = weak.lock()) {
                if (!current->timer) return;
                try {
                    current->timer.Stop();
                    if (current->gesture.Flush()) RunArtworkSingleClick();
                } catch (...) { current->gesture.Flush(); Wh_Log(L"Delayed cover playback failed"); }
            }
        });
        g_uiCleanup.push_back([state] {
            state->gesture.Flush();
            if (state->timer) {
                try { state->timer.Stop(); } catch (...) {}
                try { state->timer.Tick(state->token); } catch (...) {}
                state->timer = nullptr;
            }
        });
        button.AddHandler(UIElement::PointerPressedEvent(), winrt::box_value(PointerEventHandler(
            [state](auto const& sender, PointerRoutedEventArgs const& args) {
                if (auto element = sender.template try_as<UIElement>()) {
                    try {
                        const auto point = args.GetCurrentPoint(element).Position();
                        state->x = point.X; state->y = point.Y;
                    } catch (...) { state->gesture.Flush(); }
                }
            })), true);
    }
    button.Click([state](auto const& sender, RoutedEventArgs const&) {
        if (g_unloading || g_applyingSettings) return;
        try {
            if (!DoubleClickEnabled(L"album_art")) { RunArtworkSingleClick(); return; }
            if (!state->timer) return; // Already removed/reconfigured.
            state->timer.Stop();
            const double scale = std::max(1.0, GetDpiForWindow(g_taskbarWnd) / 96.0);
            const auto result = state->gesture.Click(GetTickCount64(), state->x, state->y,
                GetDoubleClickTime(), GetSystemMetrics(SM_CXDOUBLECLK) / (2.0 * scale),
                GetSystemMetrics(SM_CYDOUBLECLK) / (2.0 * scale));
            if (result == DoubleClickGesture::State::Result::Double) {
                if (auto source = sender.template try_as<FrameworkElement>()) RunDoubleClickAction(source);
            } else {
                if (result == DoubleClickGesture::State::Result::FlushSingle) RunArtworkSingleClick();
                state->timer.Start();
            }
        } catch (...) { state->gesture.Flush(); Wh_Log(L"Artwork click action failed"); }
    });
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
static std::wstring TrimCopy(std::wstring value) {
    const wchar_t* ws = L" \t\r\n";
    size_t first = value.find_first_not_of(ws);
    if (first == std::wstring::npos) return L"";
    size_t last = value.find_last_not_of(ws);
    return value.substr(first, last - first + 1);
}
static bool IsIgnoredMediaApp(const std::wstring& appUserModelId) {
    if (g_settings.ignoredProcesses.empty() || appUserModelId.empty()) return false;
    std::wstring appLower = ToLowerCopy(appUserModelId);
    std::wstring appStemLower = ToLowerCopy(PathFileStem(appUserModelId));
    size_t start = 0;
    while (start <= g_settings.ignoredProcesses.size()) {
        size_t end = g_settings.ignoredProcesses.find(L';', start);
        std::wstring item = TrimCopy(g_settings.ignoredProcesses.substr(
            start, end == std::wstring::npos ? std::wstring::npos : end - start));
        if (!item.empty()) {
            std::wstring itemLower = ToLowerCopy(item);
            std::wstring itemStemLower = ToLowerCopy(PathFileStem(item));
            if (appLower == itemLower ||
                appStemLower == itemStemLower ||
                appLower.find(itemLower) != std::wstring::npos ||
                appLower.find(itemStemLower) != std::wstring::npos) {
                return true;
            }
        }
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return false;
}
static bool IsBrowserAumid(const std::wstring& appUserModelId) {
    if (appUserModelId.empty()) return false;
    static const wchar_t* kBrowserSubstrings[] = {
        L"chrome", L"msedge", L"firefox", L"opera", L"brave", L"vivaldi"
    };
    std::wstring lower = ToLowerCopy(appUserModelId);
    for (auto* needle : kBrowserSubstrings) {
        if (lower.find(needle) != std::wstring::npos) return true;
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
static bool AppIdMatchesProcess(const std::wstring& appUserModelId, HWND hWnd, DWORD* outPid = nullptr, std::wstring* outProcPath = nullptr) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (outPid) *outPid = pid;
    if (!pid) return false;
    std::wstring windowAumid = GetWindowAppUserModelId(hWnd);
    std::wstring appLower = ToLowerCopy(appUserModelId);
    std::wstring windowAumidLower = ToLowerCopy(windowAumid);
    if (!windowAumidLower.empty() &&
        (appLower == windowAumidLower ||
        appLower.find(windowAumidLower) != std::wstring::npos ||
        windowAumidLower.find(appLower) != std::wstring::npos)) {
        return true;
    }
    wchar_t procPath[MAX_PATH] = {};
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProc) {
        DWORD sz = MAX_PATH;
        QueryFullProcessImageNameW(hProc, 0, procPath, &sz);
        CloseHandle(hProc);
    }
    if (outProcPath) *outProcPath = procPath;
    std::wstring procLower = ToLowerCopy(PathFileStem(procPath));
    if (procLower.empty()) return false;
    std::wstring appStemLower = ToLowerCopy(PathFileStem(appUserModelId));
    return appLower.find(procLower) != std::wstring::npos ||
        procLower.find(appLower) != std::wstring::npos ||
        appStemLower.find(procLower) != std::wstring::npos ||
        procLower.find(appStemLower) != std::wstring::npos ||
        (!windowAumidLower.empty() &&
            (windowAumidLower.find(procLower) != std::wstring::npos ||
            procLower.find(windowAumidLower) != std::wstring::npos));
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
        EnumCtx ctx{ &appUserModelId };
        EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
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
                            auto getIconTimeout = [&](HWND w, WPARAM iconType) -> HICON {
                                DWORD_PTR res = 0;
                                if (SendMessageTimeoutW(w, WM_GETICON, iconType, 0,
                                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &res)) {
                                    return (HICON)res;
                                }
                                return nullptr;
                            };
                            HICON icon = getIconTimeout(hWnd, ICON_BIG);
                            if (!icon) icon = getIconTimeout(hWnd, ICON_SMALL);
                            if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
                            if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
                            if (icon) {
                                c->exactIcon = icon;
                                GetWindowThreadProcessId(hWnd, &c->exactPid);
                                wchar_t path[MAX_PATH]{};
                                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, c->exactPid);
                                if (hProc) { DWORD sz = MAX_PATH; QueryFullProcessImageNameW(hProc, 0, path, &sz); CloseHandle(hProc); }
                                c->exactPath = path;
                                PropVariantClear(&var);
                                pps->Release();
                                return FALSE;
                            }
                        }
                    }
                    PropVariantClear(&var);
                    pps->Release();
                }
            }
            if (!c->exactIcon && !c->fuzzyIcon) {
                std::wstring windowAumid = GetWindowAppUserModelId(hWnd);
                if (windowAumid.empty()) {
                    DWORD pid = 0;
                    std::wstring procPath;
                    if (AppIdMatchesProcess(*c->aumid, hWnd, &pid, &procPath)) {
                        auto getIconTimeout = [&](HWND w, WPARAM iconType) -> HICON {
                            DWORD_PTR res = 0;
                            if (SendMessageTimeoutW(w, WM_GETICON, iconType, 0,
                                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &res)) {
                                return (HICON)res;
                            }
                            return nullptr;
                        };
                        HICON icon = getIconTimeout(hWnd, ICON_BIG);
                        if (!icon) icon = getIconTimeout(hWnd, ICON_SMALL);
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
static std::atomic<uint64_t> g_lyricsMediaPropsRequestId{0};
static void FetchMediaPropertiesOnce(uint64_t requestId) {
        if (g_unloading) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            std::wstring aumid;
            {
                std::lock_guard<std::mutex> lk(g_sessionMtx);
                session = g_currentSession;
            }
            if (!session) {
                winrt::uninit_apartment();
                return;
            }
            try {
                aumid = std::wstring(session.SourceAppUserModelId());
            } catch (...) {
                winrt::uninit_apartment();
                return;
            }
            try {
                auto props = session.TryGetMediaPropertiesAsync().get();
                if (!props) {
                    winrt::uninit_apartment();
                    return;
                }
                // A previous async request must not overwrite a newly selected song.
                {
                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                    if (g_currentSession != session ||
                        requestId != g_lyricsMediaPropsRequestId.load()) {
                        winrt::uninit_apartment();
                        return;
                    }
                }
                std::vector<BYTE> thumbBytes;
                uint64_t          thumbHash = 0;
                uint64_t          thumbStreamSize = 0;
                if (!g_settings.lyricsOnly) if (auto thumbRef = props.Thumbnail()) {
                    try {
                        auto stream = thumbRef.OpenReadAsync().get();
                        if (stream) {
                            UINT64 sz = stream.Size();
                            thumbStreamSize = sz;
                            if (sz > 0 && sz < 4 * 1024 * 1024) {
                                DataReader reader(stream);
                                reader.LoadAsync((UINT32)sz).get();
                                thumbBytes.resize((size_t)sz);
                                reader.ReadBytes(winrt::array_view<BYTE>(thumbBytes));
                                reader.DetachStream();
                                thumbHash = 14695981039346656037ULL;
                                for (BYTE byte : thumbBytes) thumbHash = (thumbHash ^ byte) * 1099511628211ULL;
                            }
                        }
                    } catch (...) { thumbBytes.clear(); thumbStreamSize = 0; }
                }

                // Reused artwork is valid across tracks (e.g. an album or playlist).
                // Session identity and request generation below reject stale results.
                std::vector<BYTE> appIconBytes;
                std::wstring      appIconKey;
                bool forceIconRefresh = false;
                {
                    std::lock_guard<std::mutex> lk(g_mediaMtx);
                    appIconKey = g_media.appIconKey;
                    appIconBytes = g_media.appIconBytes;
                }
                {
                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                    forceIconRefresh = g_userSwitchedSession;
                }
                if (!g_settings.lyricsOnly && g_settings.showAppIcon && (aumid != appIconKey || appIconBytes.empty() || forceIconRefresh || g_cachedAppIconSize != g_settings.appIconSize)) {
                    try {
                        appIconBytes = FetchAppIconBytes(aumid, g_settings.appIconSize);
                        appIconKey   = aumid;
                        g_cachedAppIconSize = g_settings.appIconSize;
                    } catch (...) {
                    }
                }
                std::unique_lock<std::mutex> sessionCommit(g_sessionMtx);
                if (g_currentSession != session || requestId != g_lyricsMediaPropsRequestId.load()) {
                    sessionCommit.unlock();
                    winrt::uninit_apartment();
                    return;
                }
                {
                    std::lock_guard<std::mutex> lk(g_mediaMtx);
                    try {
                        g_media.title          = std::wstring(props.Title());
                        g_media.artist         = std::wstring(props.Artist());
                        g_media.hasMedia       = !g_media.title.empty() || !g_media.artist.empty();
                        g_media.thumbnailBytes = std::move(thumbBytes);
                        g_media.thumbnailHash  = thumbHash;
                        g_media.thumbnailStreamSize = thumbStreamSize;
                        g_media.appUserModelId = aumid;
                        if (g_settings.showAppIcon) {
                            g_media.appIconBytes   = std::move(appIconBytes);
                            g_media.appIconKey     = appIconKey;
                        }
                    } catch (...) {
                    }
                }
                if (forceIconRefresh) g_userSwitchedSession = false;
                sessionCommit.unlock();
                if (g_settings.showLyrics) {
                    int durationSec = 0;
                    try {
                        auto timeInfo = session.GetTimelineProperties();
                        if (timeInfo) {
                            auto sec = std::chrono::duration_cast<std::chrono::seconds>(
                                timeInfo.EndTime()).count();
                            if (sec > 0 && sec <= 3600) durationSec = (int)sec;
                        }
                    } catch (...) {}
                    RequestLyricsForTrack(std::wstring(props.Title()), std::wstring(props.Artist()),
                                          session, std::wstring(props.AlbumTitle()), durationSec);
                }
            } catch (...) {
            }
        } catch (...) {}
        DispatchMediaUpdate();
        winrt::uninit_apartment();
}
static std::atomic<bool> g_mediaPropertiesBusy{false};
static void FetchMediaPropertiesAsync() {
    if (g_unloading || g_applyingSettings) return;
    ++g_lyricsMediaPropsRequestId;
    if (g_mediaPropertiesBusy.exchange(true)) return;
    SpawnTrackedWorker([] {
        for (;;) {
            const uint64_t requestId = g_lyricsMediaPropsRequestId.load();
            try { FetchMediaPropertiesOnce(requestId); } catch (...) { Wh_Log(L"Media properties request failed"); }
            g_mediaPropertiesBusy = false;
            if (g_unloading || requestId == g_lyricsMediaPropsRequestId.load() || g_mediaPropertiesBusy.exchange(true)) return;
        }
    });
}
static void FetchPlaybackInfoAsync() {
    SpawnTrackedWorker([] {
        if (g_unloading) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        bool switchedFromPlaying = false;
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            { std::lock_guard<std::mutex> lock(g_sessionMtx); session = g_currentSession; }
            if (session) {
                auto info = session.GetPlaybackInfo();
                if (info) {
                    const bool playing = info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                    auto controls = info.Controls();
                    auto shuffle = info.IsShuffleActive();
                    auto repeat = info.AutoRepeatMode();
                    std::scoped_lock lock(g_sessionMtx, g_mediaMtx);
                    if (g_currentSession == session) {
                        switchedFromPlaying = g_media.isPlaying && !playing;
                        g_media.isPlaying = playing;
                        g_media.canPlayPause = controls && (controls.IsPlayPauseToggleEnabled() || (playing ? controls.IsPauseEnabled() : controls.IsPlayEnabled()));
                        g_media.canSkipPrevious = controls && controls.IsPreviousEnabled();
                        g_media.canSkipNext = controls && controls.IsNextEnabled();
                        g_media.canShuffle = controls && controls.IsShuffleEnabled();
                        g_media.canRepeat = controls && controls.IsRepeatEnabled();
                        g_media.canSeek = controls && controls.IsPlaybackPositionEnabled();
                        g_shuffleEnabled = shuffle && shuffle.Value();
                        using RM = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
                        g_repeatMode = !repeat ? RepeatMode::Off : repeat.Value() == RM::Track ? RepeatMode::One : repeat.Value() == RM::List ? RepeatMode::All : RepeatMode::Off;
                        g_timelineDirty = true;
                    }
                }
            }
        } catch (...) { Wh_Log(L"Playback info request failed"); }
        if (switchedFromPlaying && !g_unloading) OnSessionsChanged();
        DispatchMediaUpdate();
        winrt::uninit_apartment();
    });
}
static void DetachCurrentSession() {
    GlobalSystemMediaTransportControlsSession session{nullptr};
    winrt::event_token properties{}, playback{}, timeline{};
    {
        std::lock_guard<std::mutex> lock(g_sessionMtx);
        session = g_currentSession;
        g_currentSession = nullptr;
        properties = std::exchange(g_evMediaProps, {});
        playback = std::exchange(g_evPlayback, {});
        timeline = std::exchange(g_evTimeline, {});
    }
    if (session) {
        if (properties.value) try { session.MediaPropertiesChanged(properties); } catch (...) { Wh_Log(L"Cannot remove media-properties handler"); }
        if (playback.value) try { session.PlaybackInfoChanged(playback); } catch (...) { Wh_Log(L"Cannot remove playback handler"); }
        if (timeline.value) try { session.TimelinePropertiesChanged(timeline); } catch (...) { Wh_Log(L"Cannot remove timeline handler"); }
    }
    g_timelineDirty = true;
    g_repeatMode = RepeatMode::Off;
    g_shuffleEnabled = false;
    {
        std::lock_guard<std::mutex> lock(g_mediaMtx);
        g_media.isPlaying = false;
        g_media.canPlayPause = false;
        g_media.canSkipPrevious = false;
        g_media.canSkipNext = false;
        g_media.canShuffle = false;
        g_media.canRepeat = false;
        g_media.canSeek = false;
    }
}
static GlobalSystemMediaTransportControlsSession PickBestSession() {
    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    {
        std::lock_guard<std::mutex> lkMgr(g_sessionMtx);
        mgr = g_sessionMgr;
    }
    if (!mgr) return nullptr;
    try {
        auto sessions = mgr.GetSessions();
        int sessionCount = (int)sessions.Size();
        if (sessionCount == 0) {
            return nullptr;
        }
        using S = GlobalSystemMediaTransportControlsSessionPlaybackStatus;
        GlobalSystemMediaTransportControlsSession current{nullptr};
        bool userSwitched = false;
        {
            std::lock_guard<std::mutex> lk(g_sessionMtx);
            current      = g_currentSession;
            userSwitched = g_userSwitchedSession;
        }
        GlobalSystemMediaTransportControlsSession playingFirst{nullptr};
        GlobalSystemMediaTransportControlsSession firstValid{nullptr};
        bool currentExists  = false;
        bool currentPlaying = false;
        for (auto const& s : sessions) {
            try {
                if (!s) continue;
                std::wstring appId = s.SourceAppUserModelId().c_str();
                if (IsIgnoredMediaApp(appId)) continue;
                auto pb = s.GetPlaybackInfo();
                if (!pb) continue;
                auto status = pb.PlaybackStatus();
                bool isPlaying = (status == S::Playing);
                if (!firstValid) firstValid = s;
                if (s == current) {
                    currentExists  = true;
                    currentPlaying = isPlaying;
                }
                if (isPlaying && !playingFirst) {
                    playingFirst = s;
                }
            } catch (...) {}
        }
        if (userSwitched) {
            if (currentExists) {
                return current;
            }
            { std::lock_guard<std::mutex> lk(g_sessionMtx); g_userSwitchedSession = false; }
        }
        if (!g_settings.autoSwitchSession) {
            if (currentExists) {
                return current;
            }
            return firstValid;
        }
        if (currentExists && currentPlaying) {
            return current;
        }
        if (playingFirst && playingFirst != current) {
            return playingFirst;
        }
        if (currentExists) {
            return current;
        }
        return firstValid;
    } catch (...) {
        Wh_Log(L"PickBestSession: Exception occurred");
        return nullptr;
    }
}
static void AttachToSession(GlobalSystemMediaTransportControlsSession session) {
    if (!session) {
        RequestLyricsForTrack(L"", L"");
        DetachCurrentSession();
        {
            std::lock_guard<std::mutex> lk(g_mediaMtx);
            g_media = MediaState{};
            g_lastTitleArtistKey.clear();
            g_suspectArtSize  = 0;
            g_suspectArtHash  = 0;
            g_artDelayPending = false;
            g_artNewBrowserSession = false;
        }
        DispatchMediaUpdate();
        return;
    }
    bool needsReattach = false;
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        if (g_currentSession == session) {
            if (!g_evMediaProps.value || !g_evPlayback.value) {
                needsReattach = true;
            } else {
                goto fetch;
            }
        }
    }
    (void)needsReattach;
    {
        std::wstring appId;
        try {
            appId = std::wstring(session.SourceAppUserModelId());
        } catch (...) {
        }
        ++g_lyricsMediaPropsRequestId;
        DetachCurrentSession();
        RequestLyricsForTrack(L"", L"");
        {
            std::lock_guard<std::mutex> lk(g_mediaMtx);
            g_media.appIconKey   = L"";
            g_media.appIconBytes.clear();
            g_media.thumbnailBytes.clear();
            g_media.thumbnailHash = 0;
            g_media.thumbnailStreamSize = 0;
            g_lastTitleArtistKey.clear();
            g_suspectArtSize  = 0;
            g_suspectArtHash  = 0;
            g_artDelayPending = false;
            g_artNewBrowserSession = IsBrowserAumid(appId);
        }
    }
    g_cachedAppIconSize = -1;
    g_resetScrollPending = true;
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        g_currentSession = session;
        g_timelineDirty = true;
        try {
            g_evMediaProps = g_currentSession.MediaPropertiesChanged([](auto const&, auto const&) {
                SpawnTrackedWorker([] { FetchMediaPropertiesAsync(); });
            });
            g_evPlayback = g_currentSession.PlaybackInfoChanged([](auto const&, auto const&) {
                g_timelineDirty = true;
                if (!g_unloading) FetchPlaybackInfoAsync();
            });
            g_evTimeline = g_currentSession.TimelinePropertiesChanged([](auto const&, auto const&) {
                g_timelineDirty = true;
                if (!g_unloading && !g_applyingSettings && g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
            });
        } catch (...) {
            Wh_Log(L"AttachToSession: Failed to attach event handlers; rolling back partial subscriptions");
            if (g_evMediaProps.value) try { session.MediaPropertiesChanged(g_evMediaProps); } catch (...) {}
            if (g_evPlayback.value) try { session.PlaybackInfoChanged(g_evPlayback); } catch (...) {}
            if (g_evTimeline.value) try { session.TimelinePropertiesChanged(g_evTimeline); } catch (...) {}
            g_evMediaProps = g_evPlayback = g_evTimeline = {};
            g_currentSession = nullptr;
            return;
        }
    }
fetch:
    FetchMediaPropertiesAsync();
    FetchPlaybackInfoAsync();
}
static void OnSessionsChanged() {
    if (g_unloading || g_applyingSettings) return;
    bool forceRefresh = g_forceSessionRefresh.exchange(false);
    try {
        GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
        {
            std::lock_guard<std::mutex> lk(g_sessionMtx);
            mgr = g_sessionMgr;
        }
        if (mgr) {
            auto sessions = mgr.GetSessions();
            g_sessionCount.store((int)sessions.Size());
        }
    } catch (...) {}
    if (forceRefresh) {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        g_userSwitchedSession = false;
    }
    try {
        auto newSession = PickBestSession();
        AttachToSession(newSession);
    } catch (...) {
        Wh_Log(L"OnSessionsChanged: Exception occurred");
    }
}
static DWORD WINAPI MediaThreadProc(void*) {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        auto op = GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        while (op.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
            if (WaitForSingleObject(g_mediaStopEvent, 50) == WAIT_OBJECT_0) goto done;
        }
        {
            std::lock_guard<std::mutex> lk(g_sessionMtx);
            g_sessionMgr = op.GetResults();
        }
        g_evSessionsChanged = g_sessionMgr.SessionsChanged([](auto const&, auto const&) {
            SpawnTrackedWorker([] { OnSessionsChanged(); });
        });
        g_evCurrentChanged  = g_sessionMgr.CurrentSessionChanged([](auto const&, auto const&) {
            SpawnTrackedWorker([] { OnSessionsChanged(); });
        });
        OnSessionsChanged();
        if (WaitForSingleObject(g_mediaStopEvent, 2000) == WAIT_TIMEOUT) {
            if (!g_unloading) {
                g_forceSessionRefresh = true;
                OnSessionsChanged();
            }
        }
        WaitForSingleObject(g_mediaStopEvent, INFINITE);
        try { if (g_evSessionsChanged.value) g_sessionMgr.SessionsChanged(g_evSessionsChanged); } catch (...) { Wh_Log(L"MediaThreadProc: Failed to unregister SessionsChanged event"); }
        try { if (g_evCurrentChanged.value)  g_sessionMgr.CurrentSessionChanged(g_evCurrentChanged); } catch (...) { Wh_Log(L"MediaThreadProc: Failed to unregister CurrentSessionChanged event"); }
        DetachCurrentSession();
        {
            std::lock_guard<std::mutex> lk(g_sessionMtx);
            g_sessionMgr = nullptr;
        }
    done:
        winrt::uninit_apartment();
    } catch (...) {
        Wh_Log(L"MediaThreadProc: Exception in media thread");
    }
    return 0;
}
static void StartMediaThread() {
    if (g_mediaThread) return;
    g_mediaStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_mediaStopEvent) return;
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    if (!g_mediaThread) { CloseHandle(g_mediaStopEvent); g_mediaStopEvent = nullptr; }
}
static void StopMediaThread() {
    if (g_mediaStopEvent) SetEvent(g_mediaStopEvent);
    if (g_mediaThread) { WaitForThreadToExit(g_mediaThread); CloseHandle(g_mediaThread); g_mediaThread = nullptr; }
    if (g_mediaStopEvent) { CloseHandle(g_mediaStopEvent); g_mediaStopEvent = nullptr; }
}
static HANDLE g_timerThread    = nullptr;
static HANDLE g_timerStopEvent = nullptr;
[[clang::no_destroy]] static winrt::Windows::UI::Xaml::DispatcherTimer g_scrollDispatcherTimer{nullptr};
static winrt::event_token                        g_scrollDispatcherTimerToken{};
static bool                                       g_scrollDispatcherTimerHasToken = false;
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
    if (mode == L"loop") {
        s.offset += stepPx;
        if (s.offset >= s.textWidth + g_settings.loopGap) {
            s.offset = 0.0;
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
static void ScrollTimerTick(winrt::Windows::Foundation::IInspectable const&,
                            winrt::Windows::Foundation::IInspectable const&) {
    if (g_unloading || g_applyingSettings || ReduceMotionForPower()) return;
    bool needsScroll = (g_titleScroll.active || g_artistScroll.active);
    if (!needsScroll) {
        if (g_scrollDispatcherTimer) {
            try { g_scrollDispatcherTimer.Stop(); } catch (...) {}
        }
        return;
    }
    int stepPx = std::max(1, g_settings.scrollSpeed);
    int pauseMs = g_settings.scrollPauseDuration;
    TickScrollState(g_titleScroll, stepPx, pauseMs, g_settings.scrollMode);
    TickScrollState(g_artistScroll, stepPx, pauseMs, g_settings.scrollMode);
    UpdateScrollTransforms();
}
static void StartScrollTimer() {
    HWND hWnd = g_taskbarWnd;
    if (!hWnd || !IsWindow(hWnd)) return;
    RunFromWindowThread(hWnd, [](void*) {
        try {
            if (!g_scrollDispatcherTimer) {
                g_scrollDispatcherTimer = winrt::Windows::UI::Xaml::DispatcherTimer();
                g_scrollDispatcherTimer.Interval(
                    winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(33)});
                g_scrollDispatcherTimerToken = g_scrollDispatcherTimer.Tick(&ScrollTimerTick);
                g_scrollDispatcherTimerHasToken = true;
            }
            g_scrollDispatcherTimer.Start();
        } catch (...) {}
    }, nullptr);
}
static void StopScrollTimer() {
    HWND hWnd = g_taskbarWnd;
    auto stop = [](void*) {
        try {
            if (g_scrollDispatcherTimer) {
                g_scrollDispatcherTimer.Stop();
                if (g_unloading) {
                    if (g_scrollDispatcherTimerHasToken) {
                        g_scrollDispatcherTimer.Tick(g_scrollDispatcherTimerToken);
                        g_scrollDispatcherTimerHasToken = false;
                    }
                    g_scrollDispatcherTimer = nullptr;
                }
            }
        } catch (...) {}
    };
    if (hWnd && IsWindow(hWnd)) {
        RunFromWindowThread(hWnd, stop, nullptr);
    } else {
        stop(nullptr);
    }
}
static void ResetScrollState(TextScrollState& s) {
    s.offset    = 0.0;
    s.textWidth = 0.0;
    s.viewWidth = 0.0;
    s.forward   = true;
    s.active    = false;
    if (g_settings.scrollMode == L"loop") {
        s.pausing  = false;
        s.pauseTick = 0;
    } else {
        s.pausing  = true;
        s.pauseTick = g_settings.scrollPauseDuration;
    }
}
static constexpr wchar_t kTitleScrollViewName[]  = L"FluentMedia_TitleScrollView";
static constexpr wchar_t kArtistScrollViewName[] = L"FluentMedia_ArtistScrollView";
static constexpr wchar_t kTitleCloneName[]       = L"FluentMedia_TitleClone";
static constexpr wchar_t kArtistCloneName[]      = L"FluentMedia_ArtistClone";
static constexpr wchar_t kPanelGridName[]        = L"FluentMedia_PanelGrid";
static double GetAvailableScrollTextAreaWidth() {
    try {
        if (g_settings.playerMaxWidth <= 0) return 0.0;
        if (!g_playerGrid) return 0.0;
        auto panelFe = FindChildByName(g_playerGrid, kPanelGridName);
        if (!panelFe) return 0.0;
        auto panelGrid = panelFe.try_as<Grid>();
        if (!panelGrid) return 0.0;
        double total = panelGrid.ActualWidth();
        if (total <= 0.0) return 0.0;
        auto cols = panelGrid.ColumnDefinitions();
        double used = 0.0;
        auto textContainer = FindChildByName(panelGrid, L"FluentMedia_TextContainer");
        const int textColumn = textContainer ? Grid::GetColumn(textContainer) : 2;
        for (uint32_t i = 0; i < cols.Size(); i++) {
            if ((int)i == textColumn) continue;
            used += cols.GetAt(i).ActualWidth();
        }
        double leftMargin  = (double)g_settings.textAreaLeftMargin;
        double rightMargin = (double)g_settings.textAreaRightMargin;
        double available = total - used - leftMargin - rightMargin;
        if (available < 0.0) available = 0.0;
        return available;
    } catch (...) {
        return 0.0;
    }
}
static void UpdateScrollTransforms() {
    if (!g_playerGrid || (!g_settings.enableTitleScrolling && !g_settings.enableArtistScrolling)) return;
    bool isLoop = (g_settings.scrollMode == L"loop");
    if (g_settings.enableTitleScrolling) {
        try {
            if (auto fe = FindChildByName(g_playerGrid, kTitleScrollViewName)) {
                if (auto cv = fe.try_as<Canvas>()) {
                    int n = VisualTreeHelper::GetChildrenCount(cv);
                    for (int i = 0; i < n; i++) {
                        auto child = VisualTreeHelper::GetChild(cv, i);
                        if (auto tb = child.try_as<TextBlock>()) {
                            auto name = tb.Name();
                            if (name == kTitleBlockName) {
                                Canvas::SetLeft(tb, -g_titleScroll.offset);
                            } else if (isLoop && name == kTitleCloneName) {
                                double gap = g_titleScroll.textWidth + g_settings.loopGap;
                                Canvas::SetLeft(tb, gap - g_titleScroll.offset);
                            }
                        }
                    }
                }
            }
        } catch (...) {}
    }
    if (g_settings.enableArtistScrolling) {
        try {
            if (auto fe = FindChildByName(g_playerGrid, kArtistScrollViewName)) {
                if (auto cv = fe.try_as<Canvas>()) {
                    int n = VisualTreeHelper::GetChildrenCount(cv);
                    for (int i = 0; i < n; i++) {
                        auto child = VisualTreeHelper::GetChild(cv, i);
                        if (auto ab = child.try_as<TextBlock>()) {
                            auto name = ab.Name();
                            if (name == kArtistBlockName) {
                                Canvas::SetLeft(ab, -g_artistScroll.offset);
                            } else if (isLoop && name == kArtistCloneName) {
                                double gap = g_artistScroll.textWidth + g_settings.loopGap;
                                Canvas::SetLeft(ab, gap - g_artistScroll.offset);
                            }
                        }
                    }
                }
            }
        } catch (...) {}
    }
}
static void DispatchMediaUpdate() {
    bool unloading = g_unloading;
    bool applyingSettings = g_applyingSettings;
    if (unloading || applyingSettings) {
        return;
    }
    g_needsUiUpdate = true;
    if (g_timerUpdateEvent) {
        SetEvent(g_timerUpdateEvent);
    }

    if (g_miniPlayerFlyoutOpen && g_taskbarWnd) {
        if (!g_settings.hideMediaSessionsList)
            FetchMiniSessionInfosAsync(g_taskbarWnd);
        HWND tw = g_taskbarWnd;
        RunFromWindowThread(tw, [](void*) {
            if (g_miniPlayerFlyoutOpen) RefreshMiniPlayerFlyoutUI();
        }, nullptr);
    }
}
static void RefreshPlayerContents();
static void RefreshCompactMissingLyricsLayout();
static void UpdateVisibility();
static void RefreshThemeColors();
static void RemovePlayerGrid();
static bool InjectPlayerGrid();
static std::atomic<bool> g_themeChangePending{false};
static DWORD WINAPI TimerThreadProc(void*) {
    const HRESULT apartmentResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    struct ApartmentGuard { HRESULT result; ~ApartmentGuard() { if (SUCCEEDED(result)) CoUninitialize(); } } apartment{apartmentResult};
    auto idleSince = std::chrono::steady_clock::now();
    auto nextVisibilityCheck = idleSince;
    auto nextPowerCheck = idleSince;
    static bool lastThemeWasLight = IsSystemLightTheme();
    HKEY hKey = nullptr;
    HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!hEvent) { Wh_Log(L"Cannot create theme event: %lu", GetLastError()); return 1; }
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_NOTIFY, &hKey) == ERROR_SUCCESS) {
        RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE);
    }
    while (!g_unloading) {
        HANDLE handles[] = {g_timerStopEvent, hEvent, g_timerUpdateEvent};
        bool playingNow = false;
        { std::lock_guard<std::mutex> lk(g_mediaMtx); playingNow = g_media.isPlaying; }
        const DWORD refreshMs = g_settings.showLyrics && playingNow
            ? (DWORD)std::max(g_settings.lyricRefreshMs, ReduceMotionForPower() ? 200 : 50) : 1000;
        DWORD wait = WaitForMultipleObjects(3, handles, FALSE, refreshMs);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_FAILED) { Wh_Log(L"Timer event wait failed: %lu", GetLastError()); break; }
        if (g_applyingSettings) continue;
        HWND hWnd = g_taskbarWnd;
        if (!hWnd || !IsWindow(hWnd)) {
            hWnd = FindCurrentProcessTaskbarWnd();
            g_taskbarWnd = hWnd;
            if (!hWnd) continue;
        }
        if (wait == WAIT_OBJECT_0 + 1) {
            if (hKey) {
                RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE);
            }
            {
                bool currentThemeIsLight = IsSystemLightTheme();
                if (currentThemeIsLight != lastThemeWasLight) {
                    lastThemeWasLight = currentThemeIsLight;
                    g_themeChangePending = true;
                    g_needsUiUpdate = true;
                    if (g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
                }
            }
        }
        if (g_themeChangePending.exchange(false)) {
            Sleep(150);
            if (!g_unloading && !g_applyingSettings) {
                RunFromWindowThread(hWnd, [](void*) {
                    if (!g_unloading && !g_applyingSettings && g_playerGrid) {
                        RefreshThemeColors();
                    }
                }, nullptr);
            }
        }
        if (g_settings.showLyrics) {
            UpdateLyricsPositionFromSession();
            MaybeRetryLyricsLookup();
        }
        const auto now = std::chrono::steady_clock::now();
        const bool needsUpdate = g_needsUiUpdate.exchange(false);
        const bool lyricsUpdate = g_needsLyricsUiUpdate.exchange(false);
        bool checkVisibility = now >= nextVisibilityCheck || needsUpdate;
        if (now >= nextPowerCheck) {
            SYSTEM_POWER_STATUS status{};
            if (GetSystemPowerStatus(&status)) {
                bool battery = status.ACLineStatus == 0 || status.SystemStatusFlag != 0;
                if (g_onBattery.exchange(battery) != battery) checkVisibility = true;
            }
            nextPowerCheck = now + std::chrono::seconds(5);
        }
        if (playingNow) idleSince = now;
        const bool idleHidden = g_settings.idleHideSeconds > 0 && !playingNow &&
            now - idleSince >= std::chrono::seconds(g_settings.idleHideSeconds);
        if (g_hiddenByIdle.exchange(idleHidden) != idleHidden) checkVisibility = true;
        if (checkVisibility) nextVisibilityCheck = now + std::chrono::seconds(1);
        if (needsUpdate || lyricsUpdate || checkVisibility) {
            struct Update { bool contents, lyrics, visibility; } update{needsUpdate, lyricsUpdate, checkVisibility};
            RunFromWindowThread(hWnd, [](void* param) {
                const auto& u = *static_cast<Update*>(param);
                if (g_unloading || g_applyingSettings || !g_playerGrid) return;
                if (u.contents) RefreshPlayerContents();
                else if (u.lyrics) {
                    RefreshLyricsOnlyUi();
                    // Lyrics can arrive after a network lookup without a track
                    // metadata change; restore/dock the visualizer immediately.
                    RefreshCompactMissingLyricsLayout();
                }
                if (u.visibility) UpdateVisibility();
            }, &update);
        }
    }
    if (hKey) RegCloseKey(hKey);
    if (hEvent) CloseHandle(hEvent);
    return 0;
}
static constexpr int VIZ_BARS_MAX = 20;
// 2048 samples gives ~23 Hz frequency resolution at 48 kHz while keeping
// the window ~43 ms; a 512-sample hop updates the spectrum ~every 11 ms.
static constexpr int VIZ_FFT_SIZE = 2048;
static constexpr int VIZ_FFT_HOP = VIZ_FFT_SIZE / 4;
// Extra internal frequency bands improve beat and instrument discrimination
// even when the user keeps the original seven on-screen bars.
static constexpr int VIZ_NUM_BANDS = 13;
static constexpr float VIZ_PI = 3.14159265f;
static std::atomic<float> g_VizBands[VIZ_NUM_BANDS] = {};
static std::atomic<bool> g_CaptureRunning{false};
static std::atomic<bool> g_vizCurrentlyVisible{false};
[[clang::no_destroy]] static std::optional<std::thread> g_CaptureThread;
static HANDLE g_hCaptureEvent = nullptr;
static std::atomic<bool> g_VizDeviceChanged{false};
static std::mutex g_captureThreadMtx;
class VizEndpointNotificationClient : public IMMNotificationClient {
public:
    virtual ~VizEndpointNotificationClient() = default;
    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_ref);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0)
            delete this;
        return ref;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole,
                                                    LPCWSTR) override {
        if (flow == eRender)
            g_VizDeviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
        g_VizDeviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        g_VizDeviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
        g_VizDeviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR,
                                                    const PROPERTYKEY) override {
        return S_OK;
    }
private:
    LONG m_ref = 1;
};
static float g_HannWindow[VIZ_FFT_SIZE] = {};
static float g_TwiddleRe[VIZ_FFT_SIZE / 2] = {};
static float g_TwiddleIm[VIZ_FFT_SIZE / 2] = {};
static int g_LogBinStart[VIZ_NUM_BANDS + 1] = {};
static void BuildHannWindow() {
    for (int i = 0; i < VIZ_FFT_SIZE; i++)
        g_HannWindow[i] = 0.5f * (1.f - cosf(2.f * VIZ_PI * i / (VIZ_FFT_SIZE - 1)));
}
static void BuildTwiddleFactors() {
    for (int i = 0; i < VIZ_FFT_SIZE / 2; i++) {
        float ang = -2.0f * VIZ_PI * i / VIZ_FFT_SIZE;
        g_TwiddleRe[i] = cosf(ang);
        g_TwiddleIm[i] = sinf(ang);
    }
}
static void BuildLogBins(UINT32 sampleRate) {
    static constexpr float FREQ_EDGES[VIZ_NUM_BANDS + 1] = {
        20.f, 55.f, 95.f, 150.f, 240.f, 370.f, 560.f,
        850.f, 1350.f, 2200.f, 3500.f, 5600.f, 9000.f, 18000.f};
    for (int b = 0; b <= VIZ_NUM_BANDS; b++) {
        int bin = (int)(FREQ_EDGES[b] * VIZ_FFT_SIZE / (float)sampleRate);
        g_LogBinStart[b] = std::max(1, std::min(VIZ_FFT_SIZE / 2 - 1, bin));
    }
}
static void VizFFT(std::vector<float>& re, std::vector<float>& im) {
    int n = (int)re.size();
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) {
            std::swap(re[i], re[j]);
            std::swap(im[i], im[j]);
        }
    }
    for (int len = 2; len <= n; len <<= 1) {
        int halfLen = len / 2;
        int stride = n / len;
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < halfLen; j++) {
                float wRe = g_TwiddleRe[j * stride];
                float wIm = g_TwiddleIm[j * stride];
                float uRe = re[i + j], uIm = im[i + j];
                float vRe = re[i + j + halfLen] * wRe - im[i + j + halfLen] * wIm;
                float vIm = re[i + j + halfLen] * wIm + im[i + j + halfLen] * wRe;
                re[i + j] = uRe + vRe;
                im[i + j] = uIm + vIm;
                re[i + j + halfLen] = uRe - vRe;
                im[i + j + halfLen] = uIm - vIm;
            }
        }
    }
}
struct VizEQMul { float low, mid, high; };
static VizEQMul GetVizEQMultipliers(VizEQ eq) {
    switch (eq) {
    case VizEQ::Bass: return {2.0f, 0.6f, 0.4f};
    case VizEQ::Rock: return {1.3f, 1.5f, 1.2f};
    case VizEQ::Pop: return {0.8f, 1.2f, 1.8f};
    case VizEQ::Jazz: return {1.1f, 0.8f, 0.6f};
    case VizEQ::Electronic: return {1.7f, 0.6f, 1.7f};
    default: return {1.0f, 1.0f, 1.0f};
    }
}
static bool VizInitAudioClient(IMMDeviceEnumerator* pEnum,
                                winrt::com_ptr<IAudioClient>& pClient,
                                winrt::com_ptr<IAudioCaptureClient>& pCapture,
                                UINT32& sampleRate, UINT32& channels,
                                bool& isFloat, HANDLE hEvent) {
    pClient = nullptr;
    pCapture = nullptr;
    winrt::com_ptr<IMMDevice> pDev;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, pDev.put())))
        return false;
    winrt::com_ptr<IAudioClient> pC;
    if (FAILED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                            pC.put_void())))
        return false;
    WAVEFORMATEX* pwfx = nullptr;
    pC->GetMixFormat(&pwfx);
    if (!pwfx)
        return false;
    sampleRate = pwfx->nSamplesPerSec;
    channels = pwfx->nChannels;
    isFloat = (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
            (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
            reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->SubFormat ==
                KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
    HRESULT hr = pC->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        200000, 0, pwfx, nullptr);
    CoTaskMemFree(pwfx);
    if (FAILED(hr))
        return false;
    if (hEvent)
        pC->SetEventHandle(hEvent);
    winrt::com_ptr<IAudioCaptureClient> pCap;
    if (FAILED(pC->GetService(__uuidof(IAudioCaptureClient), pCap.put_void())))
        return false;
    if (FAILED(pC->Start()))
        return false;
    pClient = pC;
    pCapture = pCap;
    return true;
}
static void VizCaptureThreadProc() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    BuildHannWindow();
    BuildTwiddleFactors();
    winrt::com_ptr<IMMDeviceEnumerator> pEnum;
    if (FAILED(CoCreateInstance(XIID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                XIID_IMMDeviceEnumerator, pEnum.put_void()))) {
        g_CaptureRunning.store(false);
        CoUninitialize();
        return;
    }
    auto* notifyClient = new VizEndpointNotificationClient();
    bool notifyRegistered =
        SUCCEEDED(pEnum->RegisterEndpointNotificationCallback(notifyClient));
    winrt::com_ptr<IAudioClient> pClient;
    winrt::com_ptr<IAudioCaptureClient> pCapture;
    UINT32 sampleRate = 48000, channels = 2;
    bool isFloat = true;
    g_VizDeviceChanged.store(false, std::memory_order_relaxed);
    if (VizInitAudioClient(pEnum.get(), pClient, pCapture, sampleRate, channels,
                        isFloat, g_hCaptureEvent))
        BuildLogBins(sampleRate);
    static constexpr int RING_CAP = VIZ_FFT_SIZE * 4;
    std::vector<float> ringBuf(RING_CAP, 0.f);
    int ringHead = 0, ringCount = 0;
    std::vector<float> re(VIZ_FFT_SIZE), im(VIZ_FFT_SIZE);
    float bandEnv[VIZ_NUM_BANDS] = {};
    ULONGLONG lastReinitAttempt = GetTickCount64() - 1000;
    ULONGLONG lastPacketMs = GetTickCount64();
    ULONGLONG lastIdleUpdateMs = lastPacketMs;
    while (g_CaptureRunning.load(std::memory_order_relaxed)) {
        if (g_hCaptureEvent)
            WaitForSingleObject(g_hCaptureEvent, 20);
        else
            Sleep(8);
        bool needsReinit = g_VizDeviceChanged.exchange(false, std::memory_order_relaxed) ||
                            !pClient;
        if (needsReinit) {
            ULONGLONG now = GetTickCount64();
            if (now - lastReinitAttempt >= 500) {
                lastReinitAttempt = now;
                if (pClient)
                    pClient->Stop();
                ringHead = 0;
                ringCount = 0;
                lastPacketMs = lastIdleUpdateMs = now;
                for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                    bandEnv[b] = 0.f;
                    g_VizBands[b].store(0.f, std::memory_order_relaxed);
                }
                if (VizInitAudioClient(pEnum.get(), pClient, pCapture, sampleRate,
                                    channels, isFloat, g_hCaptureEvent))
                    BuildLogBins(sampleRate);
            }
        }
        if (!pCapture)
            continue;
        UINT32 packetSize = 0;
        HRESULT hr = pCapture->GetNextPacketSize(&packetSize);
        if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
            g_VizDeviceChanged.store(true, std::memory_order_relaxed);
            continue;
        }
        if (FAILED(hr) || packetSize == 0) {
            // No packets should mean falling towards silence, not a frozen
            // visualizer. Decay by elapsed time, not by the number of wakes.
            ULONGLONG nowMs = GetTickCount64();
            float elapsed = std::min(100.f, (float)(nowMs - lastIdleUpdateMs)) / 1000.f;
            lastIdleUpdateMs = nowMs;
            if (nowMs - lastPacketMs >= 35) {
                const float releaseSeconds = (g_settings.vizResponseMode == L"smooth")
                    ? 0.150f : (g_settings.vizResponseMode == L"balanced" ? 0.105f : 0.072f);
                const float release = expf(-elapsed / releaseSeconds);
                for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                    bandEnv[b] *= release;
                    if (bandEnv[b] < 0.001f) bandEnv[b] = 0.f;
                    g_VizBands[b].store(bandEnv[b], std::memory_order_relaxed);
                }
            }
            continue;
        }
        lastPacketMs = lastIdleUpdateMs = GetTickCount64();
        while (packetSize > 0) {
            BYTE* pData = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;
            HRESULT hrBuf =
                pCapture->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
            if (hrBuf == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_VizDeviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hrBuf))
                break;
            // WASAPI silent packets have duration: feed zeros into the FFT
            // ring or the previous music window can persist indefinitely.
            if ((flags & AUDCLNT_BUFFERFLAGS_SILENT) || !pData) {
                for (UINT32 f = 0; f < numFrames; ++f) {
                    ringBuf[ringHead] = 0.f;
                    ringHead = (ringHead + 1) % RING_CAP;
                    if (ringCount < RING_CAP) ++ringCount;
                }
            } else if (numFrames > 0) {
                if (isFloat) {
                    float* src = reinterpret_cast<float*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++)
                            mono += src[f * channels + c];
                        ringBuf[ringHead] = mono / (float)channels;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP)
                            ringCount++;
                    }
                } else {
                    INT16* src = reinterpret_cast<INT16*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++)
                            mono += src[f * channels + c] / 32768.f;
                        ringBuf[ringHead] = mono / (float)channels;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP)
                            ringCount++;
                    }
                }
            }
            pCapture->ReleaseBuffer(numFrames);
            hr = pCapture->GetNextPacketSize(&packetSize);
            if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_VizDeviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hr))
                break;
        }
        while (ringCount >= VIZ_FFT_SIZE) {
            int readStart = (ringHead - ringCount + RING_CAP) % RING_CAP;
            for (int i = 0; i < VIZ_FFT_SIZE; i++) {
                re[i] = ringBuf[(readStart + i) % RING_CAP] * g_HannWindow[i];
                im[i] = 0.f;
            }
            ringCount -= VIZ_FFT_HOP;
            VizFFT(re, im);
            float t_sens = g_settings.vizSensitivity / 100.0f;
            float sliderGain = (t_sens <= 1.0f)
                ? 0.25f + t_sens * t_sens * 2.75f
                : 3.0f + (t_sens - 1.0f) * 4.0f;
            auto eq = GetVizEQMultipliers(g_settings.vizEq);
            static constexpr float BAND_SENSITIVITY[VIZ_NUM_BANDS] = {
                0.30f, 0.27f, 0.23f, 0.19f, 0.15f, 0.11f, 0.085f,
                0.065f, 0.047f, 0.034f, 0.025f, 0.018f, 0.012f};
            const float hopSeconds = (float)VIZ_FFT_HOP / (float)std::max(1u, sampleRate);
            // One audio-side envelope is enough for stable measured energy.
            // The UI uses only a very short interpolation for visible motion.
            const float attackSeconds = (g_settings.vizResponseMode == L"smooth")
                ? 0.022f : (g_settings.vizResponseMode == L"balanced" ? 0.012f : 0.005f);
            const float releaseSeconds = (g_settings.vizResponseMode == L"smooth")
                ? 0.150f : (g_settings.vizResponseMode == L"balanced" ? 0.105f : 0.072f);
            const float attackK = 1.f - expf(-hopSeconds / attackSeconds);
            const float releaseK = 1.f - expf(-hopSeconds / releaseSeconds);
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                int bStart = g_LogBinStart[b];
                int bEnd = g_LogBinStart[b + 1];
                if (bEnd <= bStart)
                    bEnd = bStart + 1;
                float sumSq = 0.f;
                float bandPeak = 0.f;
                int count = 0;
                for (int k = bStart; k < bEnd && k < VIZ_FFT_SIZE / 2; ++k) {
                    float power = re[k] * re[k] + im[k] * im[k];
                    sumSq += power;
                    bandPeak = std::max(bandPeak, power);
                    ++count;
                }
                const float rms = count > 0 ? sqrtf(sumSq / (float)count) : 0.f;
                // A little emphasis on narrow transient peaks, without
                // reacting solely to a single noisy frequency bin.
                const float energy = 0.82f * rms + 0.18f * sqrtf(bandPeak);
                const float eqM = b < 4 ? eq.low : (b < 9 ? eq.mid : eq.high);
                float mag = std::max(
                    0.f, std::min(1.f, (energy / (VIZ_FFT_SIZE * 0.5f)) /
                                        BAND_SENSITIVITY[b] * sliderGain * eqM));
                // Release and attack are expressed in seconds, independent of
                // hardware packet sizes and callback scheduling.
                float k = mag > bandEnv[b] ? attackK : releaseK;
                bandEnv[b] += (mag - bandEnv[b]) * k;
                g_VizBands[b].store(bandEnv[b], std::memory_order_relaxed);
            }
        }
    }
    if (pClient)
        pClient->Stop();
    if (notifyRegistered)
        pEnum->UnregisterEndpointNotificationCallback(notifyClient);
    notifyClient->Release();
    CoUninitialize();
}
static void StartVizCaptureThread() {
    std::lock_guard<std::mutex> lk(g_captureThreadMtx);
    if (g_CaptureRunning.load())
        return;
    if (g_CaptureThread && g_CaptureThread->joinable())
        g_CaptureThread->join();
    g_CaptureThread.reset();
    if (!g_hCaptureEvent)
        g_hCaptureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_CaptureRunning.store(true);
    g_CaptureThread.emplace(VizCaptureThreadProc);
}
static void StopVizCaptureThread() {
    g_CaptureRunning.store(false);
    if (g_hCaptureEvent)
        SetEvent(g_hCaptureEvent);
    std::lock_guard<std::mutex> lk(g_captureThreadMtx);
    if (g_CaptureThread) {
        if (g_CaptureThread->joinable())
            g_CaptureThread->join();
        g_CaptureThread.reset();
    }
    if (g_hCaptureEvent) {
        CloseHandle(g_hCaptureEvent);
        g_hCaptureEvent = nullptr;
    }
    for (int i = 0; i < VIZ_NUM_BANDS; i++)
        g_VizBands[i].store(0.f);
}
static float g_VizPeak[VIZ_BARS_MAX] = {};
static float g_VizTarget[VIZ_BARS_MAX] = {};
static float g_VizBreatheEnv = 0.f;
static bool VizPreviewWithoutPlayback() {
    if (g_settings.vizPreviewMode == L"off" || !g_settings.vizEnabled) return false;
    std::lock_guard<std::mutex> lock(g_mediaMtx);
    return !g_media.isPlaying; // Paused and no-session preview, never override live audio.
}
static constexpr float VIZ_SEEDS[VIZ_BARS_MAX] = {
    0.83f, 0.41f, 1.27f, 0.61f, 1.09f, 0.37f, 0.95f, 0.52f, 1.18f, 0.74f,
    0.29f, 1.03f, 0.66f, 0.88f, 0.45f, 1.21f, 0.57f, 0.93f, 0.31f, 1.15f};
static void UpdateVisualizerPeaks() {
    const int vizBars = std::max(1, std::min(g_settings.vizBars, VIZ_BARS_MAX));
    float bands[VIZ_NUM_BANDS];
    float masterPeak = 0.f;
    const bool simulation = VizPreviewWithoutPlayback();
    // UI-only low-cost test signal; never routed through WASAPI and never used
    // when a real music session is currently playing.
    const float t = std::chrono::duration<float>(
        std::chrono::steady_clock::now() - g_vizPreviewEpoch).count();
    const float beat = simulation ? powf(std::max(0.f, cosf(t * 6.3f)), 5.f) : 0.f;
    for (int i = 0; i < VIZ_NUM_BANDS; i++) {
        if (simulation) {
            float wave = 0.28f + 0.26f * sinf(t * (2.4f + i * 0.18f) + i * 0.62f);
            bands[i] = std::clamp((beat * (0.65f - i * 0.023f) + wave) *
                                   (g_settings.vizPreviewStrength / 100.f), 0.f, 1.f);
        } else bands[i] = g_VizBands[i].load(std::memory_order_relaxed);
        masterPeak = std::max(masterPeak, bands[i]);
    }
    // One bar must represent the WHOLE spectrum, not just its middle!
    // Divide 20 Hz..18 kHz into N logarithmically sized contiguous groups.
    // Integrate overlap with our 13 measured bands, not 1-2 point samples.
    // Recompute mapping only when the user changes the number of bars.
    static int lastMappedCount = 0;
    static float groupWeights[VIZ_BARS_MAX][VIZ_NUM_BANDS] = {};
    if (lastMappedCount != vizBars) {
        static constexpr float edges[VIZ_NUM_BANDS + 1] = {
            20.f, 55.f, 95.f, 150.f, 240.f, 370.f, 560.f,
            850.f, 1350.f, 2200.f, 3500.f, 5600.f, 9000.f, 18000.f};
        constexpr float lo = 20.f, hi = 18000.f;
        const float logRange = logf(hi / lo);
        for (int bar = 0; bar < vizBars; ++bar) {
            const float groupLo = logf(lo) + logRange * (float)bar / (float)vizBars;
            const float groupHi = logf(lo) + logRange * (float)(bar + 1) / (float)vizBars;
            float total = 0.f;
            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                float left = std::max(groupLo, logf(edges[b]));
                float right = std::min(groupHi, logf(edges[b + 1]));
                float w = std::max(0.f, right - left);
                groupWeights[bar][b] = w;
                total += w;
            }
            if (total > 0.f)
                for (int b = 0; b < VIZ_NUM_BANDS; ++b)
                    groupWeights[bar][b] /= total;
        }
        lastMappedCount = vizBars;
    }
    float spectrum[VIZ_BARS_MAX] = {};
    for (int bar = 0; bar < vizBars; ++bar) {
        float power = 0.f, localPeak = 0.f;
        for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
            const float w = groupWeights[bar][b];
            if (w <= 0.f) continue;
            const float v = std::clamp(bands[b], 0.f, 1.f);
            power += w * v * v;
            localPeak = std::max(localPeak, v);
        }
        // RMS + transient peak: continuous accurate band energy, with drums
        // still visible in wide groups such as 1 or 2 bars.
        spectrum[bar] = std::clamp(0.78f * sqrtf(power) + 0.22f * localPeak,
                                   0.f, 1.f);
    }
    auto sampleBands = [&](float t) -> float {
        const float pos = std::clamp(t, 0.f, 1.f) * (float)(vizBars - 1);
        const int low = (int)pos;
        const int high = std::min(low + 1, vizBars - 1);
        return spectrum[low] * (1.f - (pos - low)) +
               spectrum[high] * (pos - low);
    };
    float center = (vizBars - 1) * 0.5f;
    for (int i = 0; i < vizBars; i++) {
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;
        switch (g_settings.vizShape) {
            case VizShape::Stereo:
                target = sampleBands(freqT);
                break;
            case VizShape::Mountain: {
                float dist = fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(dist);
                float taper = 1.6f - dist * 0.9f;
                target = std::max(
                    0.f, std::min(1.f, (energy + masterPeak * (0.2f - dist * 0.12f)) *
                                        taper));
                break;
            }
            case VizShape::Mirror: {
                float mirT = 1.f - fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(mirT);
                target = std::max(
                    0.f, std::min(1.f, (energy + masterPeak * (0.1f + mirT * 0.12f)) *
                                        1.3f));
                break;
            }
            case VizShape::Wave: {
                // Shape follows changing adjacent frequency energy instead
                // of using a free-running sine oscillator unrelated to music.
                float energy = sampleBands(freqT);
                float neighbor = sampleBands(std::min(1.f, freqT + 0.08f));
                target = std::clamp(0.75f * energy + 0.25f * neighbor, 0.f, 1.f);
                break;
            }
            case VizShape::Breathe: {
                if (i == 0) {
                    // Breathing is a *smoothed* music amplitude, not an
                    // autonomous animation that continues during silence.
                    float k = masterPeak > g_VizBreatheEnv ? 0.30f : 0.11f;
                    g_VizBreatheEnv += (masterPeak - g_VizBreatheEnv) * k;
                }
                target = std::clamp(g_VizBreatheEnv *
                                    (0.85f + 0.15f * VIZ_SEEDS[i % VIZ_BARS_MAX]),
                                    0.f, 1.f);
                break;
            }
        }
        g_VizTarget[i] = std::max(0.f, std::min(1.f, target));
    }
}
using VizRect = winrt::Windows::UI::Xaml::Shapes::Rectangle;
[[clang::no_destroy]] static std::optional<std::vector<VizRect>> g_vizBars{std::in_place};
[[clang::no_destroy]] static std::optional<std::vector<SolidColorBrush>> g_vizBrushes{std::in_place};
static winrt::Windows::UI::Color VizLerpColor(winrt::Windows::UI::Color a,
                                            winrt::Windows::UI::Color b, float t) {
    auto L = [](BYTE x, BYTE y, float tt) -> BYTE {
        return (BYTE)((int)x + (int)((float)((int)y - (int)x) * tt));
    };
    return winrt::Windows::UI::Color{255, L(a.R, b.R, t), L(a.G, b.G, t), L(a.B, b.B, t)};
}
static winrt::Windows::UI::Color VizEnsureContrast(winrt::Windows::UI::Color c) {
    const int kLumaThreshold = 90;
    bool light = IsSystemLightTheme();
    int luma = (c.R * 299 + c.G * 587 + c.B * 114) / 1000;
    int edgeDist = light ? (255 - luma) : luma;
    if (edgeDist >= kLumaThreshold) return c;
    winrt::Windows::UI::Color base = light
        ? winrt::Windows::UI::Color{255, 0, 0, 0}
        : winrt::Windows::UI::Color{255, 255, 255, 255};
    if (g_cachedThumbnailBytes.empty()) return base;
    float t = edgeDist / (float)kLumaThreshold;
    float tintAmount = 0.65f + t * 0.35f;
    return VizLerpColor(base, c, tintAmount);
}
// A dedicated setting overrides just the quiet-bar height. -1 keeps the
// original two-number Bar size setting byte-for-byte compatible in behavior.
// In legacy mode, second value 0 has always meant fully hidden at silence.
static double VizQuietBarHeight(double usableHeight) {
    if (g_settings.vizRestingHeight >= 0)
        return std::clamp((double)g_settings.vizRestingHeight, 0.0, usableHeight);
    if (g_settings.vizIdleBarSize == 0)
        return 0.0;
    return std::clamp((double)g_settings.vizBarWidth +
                      (double)g_settings.vizIdleBarSize, 0.0, usableHeight);
}
static double VizZoneHeight() {
    double h = 0.0;
    if (g_settings.showAlbumArt && g_settings.albumArtMaxHeight > 0)
        h = std::max(h, (double)g_settings.albumArtMaxHeight);
    if (g_settings.showMediaButtons)
        h = std::max(h, (double)g_settings.buttonSize);
    if (h <= 0.0)
        h = (g_settings.playerMaxHeight > 0 ? (double)g_settings.playerMaxHeight : 40.0);
    if (g_settings.playerMaxHeight > 0)
        h = std::min(h, (double)g_settings.playerMaxHeight);
    return h;
}
static void VizApplyFrame() {
    if (!g_settings.vizEnabled || g_vizBars->empty())
        return;
    UpdateVisualizerPeaks();
    // Consistent response at 30/60/120 Hz or during temporary Explorer lag.
    static auto lastFrame = std::chrono::steady_clock::now();
    const auto frameNow = std::chrono::steady_clock::now();
    float dt = std::clamp(std::chrono::duration<float>(frameNow - lastFrame).count(),
                          0.001f, 0.100f);
    lastFrame = frameNow;
    float attackSeconds = 0.006f, decaySeconds = 0.032f;
    if (g_settings.vizResponseMode == L"balanced") {
        attackSeconds = 0.012f; decaySeconds = 0.052f;
    } else if (g_settings.vizResponseMode == L"smooth") {
        attackSeconds = 0.024f; decaySeconds = 0.090f;
    }
    // Artistic Breathe retains its deliberately gentler visual response.
    if (g_settings.vizShape == VizShape::Breathe) {
        attackSeconds *= 2.5f;
        decaySeconds *= 1.8f;
    }
    const float attack = 1.f - expf(-dt / attackSeconds);
    const float decay  = 1.f - expf(-dt / decaySeconds);
    int barCount = std::min((int)g_vizBars->size(), std::max(1, g_settings.vizBars));
    double zoneH = VizZoneHeight();
    double maxBH = std::max(4.0, zoneH - 6.0);
    const double restBH = VizQuietBarHeight(maxBH);
    const double range = std::max(0.0, maxBH - restBH);
    winrt::Windows::UI::Color baseCol{255, 255, 255, 255};
    if (g_settings.vizColorMode == VizColorMode::DynamicAlbum) {
        if (g_vizBaseColorDirty || g_vizPaletteColorsDirty) {
            g_cachedVizBaseColor = VizEnsureContrast(g_cachedAlbumPalette.primary);
            g_cachedVizBaseColor.A = 255;
        }
        baseCol = g_cachedVizBaseColor;
    } else {
        if (g_vizBaseColorDirty) {
            g_cachedVizBaseColor = ParseColorWithThemeSupport(g_settings.vizColor, 255);
        }
        baseCol = g_cachedVizBaseColor;
    }
    if (g_vizPaletteColorsDirty || g_vizBaseColorDirty) {
        g_cachedVizPal0   = VizEnsureContrast(g_cachedAlbumPalette.primary);
        g_cachedVizPal1   = VizEnsureContrast(g_cachedAlbumPalette.secondary);
        g_cachedVizCg0    = ParseColorWithSpecialValues(g_settings.vizColor1, 255);
        g_cachedVizCg1    = ParseColorWithSpecialValues(g_settings.vizColor2, 255);
        g_cachedVizAcrCol = ParseColorWithThemeSupport(g_settings.vizColor, 255);
        g_vizPaletteColorsDirty = false;
        g_vizBaseColorDirty = false;
    }
    auto pal0 = g_cachedVizPal0;
    auto pal1 = g_cachedVizPal1;
    if (g_songColorTransitionActive && g_settings.songChangeEnabled &&
        g_settings.songChangeVizColor) {
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            frameNow - g_songColorTransitionAt).count();
        double t = std::clamp((double)elapsed / std::max(1, g_settings.songChangeDurationMs), 0.0, 1.0);
        // Ease in/out for gentle color blending without per-frame palette extraction.
        t = t * t * (3.0 - 2.0 * t);
        auto old0 = VizEnsureContrast(g_songColorFrom.primary);
        auto old1 = VizEnsureContrast(g_songColorFrom.secondary);
        pal0 = VizLerpColor(old0, pal0, (float)t);
        pal1 = VizLerpColor(old1, pal1, (float)t);
        if (g_settings.vizColorMode == VizColorMode::DynamicAlbum)
            baseCol = VizLerpColor(old0, baseCol, (float)t);
        if (elapsed >= g_settings.songChangeDurationMs) g_songColorTransitionActive = false;
    }
    const auto& cg0    = g_cachedVizCg0;
    const auto& cg1    = g_cachedVizCg1;
    const auto& acrCol = g_cachedVizAcrCol;
    for (int i = 0; i < barCount; i++) {
        float tgt = g_VizTarget[i], cur = g_VizPeak[i];
        float a = attack, d = decay;
        float next = cur + (tgt - cur) * ((tgt > cur) ? a : d);
        g_VizPeak[i] = (fabsf(next - cur) > 0.0005f) ? next : tgt;
        float fac = std::max(0.f, g_VizPeak[i]);
        const double bh = restBH + fac * range;
        winrt::Windows::UI::Color c = baseCol;
        if (g_settings.vizColorMode == VizColorMode::DynamicGradient) {
            float t = (float)i / std::max(1, barCount - 1);
            float freqT = std::min(1.f, t * 0.6f + fac * 0.4f);
            c = VizLerpColor(pal0, pal1, freqT);
        } else if (g_settings.vizColorMode == VizColorMode::CustomGradient) {
            float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
            c = VizLerpColor(cg0, cg1, t);
        } else if (g_settings.vizColorMode == VizColorMode::Acrylic) {
            BYTE aa = (BYTE)std::max(30, std::min(180, (int)(150.f * fac + 30.f)));
            c = winrt::Windows::UI::Color{aa, acrCol.R, acrCol.G, acrCol.B};
        }
        try {
            auto const& rect = (*g_vizBars)[i];
            if (fabs(rect.Height() - bh) >= 0.12) rect.Height(bh);
            // Radius cannot be bigger than HALF the shorter side; this makes
            // both tips true semicircular caps even for tiny / idle sticks.
            const double rounding = (double)g_settings.vizRoundness / 100.0;
            const double cap = 0.5 * std::min((double)g_settings.vizBarWidth, bh) * rounding;
            if (fabs(rect.RadiusX() - cap) >= 0.02) {
                rect.RadiusX(cap);
                rect.RadiusY(cap);
            }
            if (i < (int)g_vizBrushes->size() && (*g_vizBrushes)[i]) {
                auto brush = (*g_vizBrushes)[i];
                auto was = brush.Color();
                if (was.A != c.A || was.R != c.R || was.G != c.G || was.B != c.B)
                    brush.Color(c);
            }
        } catch (...) {}
    }
}
[[clang::no_destroy]] static winrt::Windows::UI::Xaml::DispatcherTimer g_vizDispatcherTimer{nullptr};
static winrt::event_token g_vizDispatcherTimerToken{};
static bool g_vizDispatcherTimerHasToken = false;
static void VizTimerTick(winrt::Windows::Foundation::IInspectable const&,
                        winrt::Windows::Foundation::IInspectable const&) {
    if (g_unloading || g_applyingSettings) return;
    if (!g_settings.vizEnabled) return;
    VizApplyFrame();
    if (g_settings.vizPreviewMode == L"cycle" && VizPreviewWithoutPlayback()) {
        static auto nextCheck = std::chrono::steady_clock::now();
        const auto now = std::chrono::steady_clock::now();
        if (now >= nextCheck) {
            nextCheck = now + std::chrono::milliseconds(180);
            RefreshCompactMissingLyricsLayout();
        }
    }
}
static void StartVizTimer() {
    HWND hWnd = g_taskbarWnd;
    if (!hWnd || !IsWindow(hWnd)) return;
    RunFromWindowThread(hWnd, [](void*) {
        try {
            if (!g_vizDispatcherTimer) {
                g_vizDispatcherTimer = winrt::Windows::UI::Xaml::DispatcherTimer();
                g_vizDispatcherTimerToken = g_vizDispatcherTimer.Tick(&VizTimerTick);
                g_vizDispatcherTimerHasToken = true;
            }
            // Even an existing timer must pick up a changed 30/60 FPS setting.
            const int intervalMs = (g_settings.vizFrameRate == L"30") ? 33 : 16;
            g_vizDispatcherTimer.Interval(
                winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(intervalMs)});
            g_vizDispatcherTimer.Start();
        } catch (...) {}
    }, nullptr);
}
static void StopVizTimer() {
    HWND hWnd = g_taskbarWnd;
    auto stop = [](void*) {
        try {
            if (g_vizDispatcherTimer) {
                g_vizDispatcherTimer.Stop();
                if (g_unloading) {
                    if (g_vizDispatcherTimerHasToken) {
                        g_vizDispatcherTimer.Tick(g_vizDispatcherTimerToken);
                        g_vizDispatcherTimerHasToken = false;
                    }
                    g_vizDispatcherTimer = nullptr;
                }
            }
        } catch (...) {}
    };
    if (hWnd && IsWindow(hWnd)) {
        RunFromWindowThread(hWnd, stop, nullptr);
    } else {
        stop(nullptr);
    }
}
static FrameworkElement BuildVisualizerElement() {
    g_vizBars->clear();
    g_vizBrushes->clear();
    int barCount = std::clamp(g_settings.vizBars, 1, VIZ_BARS_MAX);
    double zoneH = VizZoneHeight();
    StackPanel bars;
    bars.Name(L"FluentMedia_VisualizerBars");
    bars.Orientation(Orientation::Horizontal);
    bars.Height(zoneH);
    bars.VerticalAlignment(VerticalAlignment::Center);
    bars.HorizontalAlignment(HorizontalAlignment::Center);
    bars.IsHitTestVisible(false);
    VerticalAlignment va = (g_settings.vizAnchor == VizAnchor::Top)    ? VerticalAlignment::Top
                        : (g_settings.vizAnchor == VizAnchor::Bottom) ? VerticalAlignment::Bottom
                                                            : VerticalAlignment::Center;
    const double maxBH = std::max(4.0, zoneH - 6.0);
    const double restBH = VizQuietBarHeight(maxBH);
    const double corner = g_settings.vizBarWidth * (g_settings.vizRoundness / 200.0);
    for (int i = 0; i < barCount; i++) {
        VizRect r;
        r.Width((double)g_settings.vizBarWidth);
        r.Height(restBH);
        const double initialCap = std::min(corner, 0.5 * restBH);
        r.RadiusX(initialCap);
        r.RadiusY(initialCap);
        r.VerticalAlignment(va);
        if (i > 0)
            r.Margin({(double)g_settings.vizBarGap, 0, 0, 0});
        SolidColorBrush br = MakeBrush({255, 255, 255, 255});
        r.Fill(br);
        if (g_settings.vizColorMode == VizColorMode::Acrylic) {
            r.Stroke(MakeBrush({0x40, 0xFF, 0xFF, 0xFF}));
            r.StrokeThickness(0.8);
        }
        bars.Children().Append(r);
        g_vizBars->push_back(r);
        g_vizBrushes->push_back(br);
    }
    bool vizLeft = (g_settings.vizPosition == L"left");
    if (g_settings.mirrorLayout) vizLeft = !vizLeft;
    const double sectionGap = (double)g_settings.vizSectionGap;
    // Positive horizontal adjustment adds space to the left; negative adds it
    // to the right. This reserves width instead of drawing across other icons.
    double marginL = (double)g_settings.vizPadLeft + (vizLeft ? 0.0 : sectionGap) +
                     (double)std::max(0, g_settings.vizHorizontalOffset);
    double marginR = (double)g_settings.vizPadRight + (vizLeft ? sectionGap : 0.0) +
                     (double)std::max(0, -g_settings.vizHorizontalOffset);
    Grid vizContainer;
    vizContainer.Name(L"FluentMedia_Visualizer");
    vizContainer.Height(zoneH);
    vizContainer.VerticalAlignment(VerticalAlignment::Center);
    vizContainer.HorizontalAlignment(HorizontalAlignment::Center);
    vizContainer.IsHitTestVisible(false);
    TranslateTransform vizVerticalShift;
    // This does not change the height or width of any of the other controls.
    vizVerticalShift.Y((double)g_settings.vizVerticalOffset);
    vizContainer.RenderTransform(vizVerticalShift);
    vizContainer.Background(MakeBrush({0x00, 0x00, 0x00, 0x00}));
    vizContainer.Margin({marginL, 0, marginR, 0});
    if (g_settings.showDebugBorders) {
        Border vizDebugBorder;
        vizDebugBorder.BorderBrush(MakeBrush({0xFF, 0xFF, 0xFF, 0x00}));
        vizDebugBorder.BorderThickness({1, 1, 1, 1});
        vizDebugBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
        vizDebugBorder.VerticalAlignment(VerticalAlignment::Stretch);
        vizContainer.Children().Append(vizDebugBorder);
    }
    vizContainer.Children().Append(bars);
    return vizContainer;
}
static void StartTimerThread() {
    if (g_timerThread) return;
    g_timerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_timerUpdateEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_timerStopEvent || !g_timerUpdateEvent) {
        if (g_timerStopEvent) CloseHandle(g_timerStopEvent);
        if (g_timerUpdateEvent) CloseHandle(g_timerUpdateEvent);
        g_timerStopEvent = g_timerUpdateEvent = nullptr;
        Wh_Log(L"Cannot create timer events");
        return;
    }
    g_timerThread    = CreateThread(nullptr, 0, TimerThreadProc, nullptr, 0, nullptr);
    if (!g_timerThread) {
        CloseHandle(g_timerStopEvent);
        g_timerStopEvent = nullptr;
        CloseHandle(g_timerUpdateEvent);
        g_timerUpdateEvent = nullptr;
    }
    if (!g_settings.lyricsOnly && (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling)) {
        StartScrollTimer();
    }
}
static void StopTimerThread() {
    StopScrollTimer();
    StopVizTimer();
    StopVizCaptureThread();
    if (g_timerStopEvent) SetEvent(g_timerStopEvent);
    if (g_timerThread) {
        WaitForThreadToExit(g_timerThread);
        CloseHandle(g_timerThread);
        g_timerThread = nullptr;
    }
    if (g_timerStopEvent) { CloseHandle(g_timerStopEvent); g_timerStopEvent = nullptr; }
    if (g_timerUpdateEvent) { CloseHandle(g_timerUpdateEvent); g_timerUpdateEvent = nullptr; }
}
static void RefreshThemeColors() {
    try {
        if (g_lyricsLineA) g_lyricsLineA.Foreground(MakeBrush(TextColor()));
        if (g_lyricsLineB) g_lyricsLineB.Foreground(MakeBrush(TextColor()));
    } catch (...) {}
    if (g_settings.lyricsOnly) {
        try {
            if (g_lyricsLineA) g_lyricsLineA.Foreground(MakeBrush(TextColor()));
            if (g_lyricsLineB) g_lyricsLineB.Foreground(MakeBrush(TextColor()));
            if (g_playerGrid) {
                auto bg = FindChildByName(g_playerGrid, L"FluentMedia_Background");
                if (auto border = bg.try_as<Border>())
                    border.Background(g_settings.backgroundType == L"none" ? nullptr : MakeBackgroundBrush());
            }
        } catch (...) {}
        return;
    }
    if (!g_playerGrid || g_unloading || g_applyingSettings) return;
    g_vizBaseColorDirty = true;
    g_vizPaletteColorsDirty = true;
    try {
        UpdateHoverBrushColors();
        auto textClr = TextColor();
        auto artistClr = ArtistColor();
        auto buttonClr = ButtonColor();
        if (auto bgFe = FindChildByName(g_playerGrid, L"FluentMedia_Background")) {
            if (auto bgBorder = bgFe.try_as<Border>()) {
                auto& bgType = g_settings.backgroundType;
                if (bgType == L"album_art_blur") {
                    if (!g_cachedThumbnailBytes.empty()) {
                        int w = (int)bgBorder.ActualWidth();
                        int h = (int)bgBorder.ActualHeight();
                        if (w > 0 && h > 0) {
                            bgBorder.Background(MakeAlbumBlurBrush(g_cachedThumbnailBytes, w, h));
                        }
                    } else {
                        auto fallbackCol = IsSystemLightTheme()
                            ? winrt::Windows::UI::Color{0xCC, 0xF3, 0xF3, 0xF3}
                            : winrt::Windows::UI::Color{0xCC, 0x20, 0x20, 0x20};
                        bgBorder.Background(MakeBrush(fallbackCol));
                    }
                    bgBorder.Visibility(Visibility::Visible);
                    bgBorder.Opacity(g_settings.blurOpacity / 100.0);
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
        if (auto fe = FindChildByName(g_playerGrid, L"FluentMedia_OuterBorder")) {
            if (auto btn = fe.try_as<Button>()) {
                try {
                    auto normalBg = MakeBackgroundBrush();
                    ApplyFluentMediaButtonStyle(btn);
                    btn.ApplyTemplate();
                    SetupPlayerCommonStates(btn, normalBg);
                    GoToCommonState(btn, IsHoverEffectEnabled(g_settings.playerHoverEffectMode), false, false);
                } catch (...) {}
            }
        }
        if (auto fe = FindChildByName(g_playerGrid, kTitleBlockName))
            if (auto tb = fe.try_as<TextBlock>()) tb.Foreground(MakeBrush(textClr));
        if (auto fe = FindChildByName(g_playerGrid, kArtistBlockName))
            if (auto ab = fe.try_as<TextBlock>()) ab.Foreground(MakeBrush(artistClr));
        for (const wchar_t* name : {kPrevBtnName, kPlayBtnName, kNextBtnName, kRewindBtnName, kForwardBtnName, kShuffleBtnName, kRepeatBtnName, kSwitchSessionsBtnName}) {
            if (auto fe = FindChildByName(g_playerGrid, name)) {
                if (auto btn = fe.try_as<Button>()) {
                    if (auto ct = btn.Content().try_as<TextBlock>()) ct.Foreground(MakeBrush(buttonClr));
                    ApplyFluentMediaButtonStyle(btn);
                    btn.ApplyTemplate();
                    SetupMediaButtonCommonStates(btn);
                    try { VisualStateManager::GoToState(btn, L"Normal", false); } catch (...) {}
                }
            }
        }
    } catch (...) {}
}
static HMONITOR GetMonitorByNumber(int monitorNumber) {
    HMONITOR result = nullptr;
    int current = 0;
    struct Ctx { int target; HMONITOR* result; int* current; };
    Ctx ctx{monitorNumber - 1, &result, &current};
    EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR hMon, HDC, LPRECT, LPARAM lp) -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lp);
            if (*c->current == c->target) { *c->result = hMon; return FALSE; }
            (*c->current)++;
            return TRUE;
        }, reinterpret_cast<LPARAM>(&ctx));
    return result;
}
static HWND FindTaskbarWndForMonitor(HMONITOR targetMonitor) {
    struct Ctx { HMONITOR target; HWND result; };
    Ctx ctx{targetMonitor, nullptr};
    EnumWindows([](HWND hWnd, LPARAM lp) -> BOOL {
        auto* c = reinterpret_cast<Ctx*>(lp);
        DWORD pid = 0; wchar_t cls[64] = {};
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
            (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
             _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0)) {
            HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            if (hMon == c->target) {
                c->result = hWnd;
                return FALSE;
            }
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));
    return ctx.result;
}
static HWND FindCurrentProcessTaskbarWnd() {
    if (HMONITOR mon = GetMonitorByNumber(g_settings.monitor)) {
        if (HWND hWnd = FindTaskbarWndForMonitor(mon)) {
            return hWnd;
        }
    } else {
    }
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lp) -> BOOL {
        DWORD pid = 0; wchar_t cls[32] = {};
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"Shell_TrayWnd") == 0)
        {
            *reinterpret_cast<HWND*>(lp) = hWnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
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
    // Settings edits reuse the live XAML root rather than re-entering Explorer's
    // private TaskbarHost/shared_ptr ABI on every UI rebuild.
    if (hTaskbarWnd == g_cachedTaskbarRootWindow && g_cachedTaskbarXamlRoot) return g_cachedTaskbarXamlRoot;
    wchar_t clsBuf[64] = {};
    GetClassNameW(hTaskbarWnd, clsBuf, ARRAYSIZE(clsBuf));
    bool isSecondary = _wcsicmp(clsBuf, L"Shell_SecondaryTrayWnd") == 0;
    HWND hTaskSwWnd = isSecondary
        ? FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
        : (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        Wh_Log(L"GetTaskbarXamlRoot: could not find taskband host window, aborting");
        return nullptr;
    }
    wchar_t swCls[64] = {};
    GetClassNameW(hTaskSwWnd, swCls, ARRAYSIZE(swCls));
    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand) {
        Wh_Log(L"GetTaskbarXamlRoot: taskBand pointer is null, aborting");
        return nullptr;
    }
    void* expectedVftable = isSecondary
        ? CSecondaryTaskBand_ITaskListWndSite_vftable
        : CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = isSecondary
        ? CSecondaryTaskBand_GetTaskbarHost_Original
        : CTaskBand_GetTaskbarHost_Original;
    if (!expectedVftable || !getTaskbarHost) {
        Wh_Log(L"GetTaskbarXamlRoot: %s symbols not resolved, aborting",
            isSecondary ? L"CSecondaryTaskBand" : L"CTaskBand");
        return nullptr;
    }
    void* taskBandForTaskListWndSite = taskBand;
    int i = 0;
    constexpr int kMaxSlotsToScan = 20;
    for (;; i++) {
        if (!IsReadableMemoryRange(taskBandForTaskListWndSite, sizeof(void*))) {
            Wh_Log(L"GetTaskbarXamlRoot: hit unreadable memory at slot %d, aborting", i);
            return nullptr;
        }
        if (*(void**)taskBandForTaskListWndSite == expectedVftable) break;
        if (i == kMaxSlotsToScan) {
            Wh_Log(L"GetTaskbarXamlRoot: ITaskListWndSite vftable not found within %d slots, aborting",
                kMaxSlotsToScan);
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        Wh_Log(L"GetTaskbarXamlRoot: TaskbarHost shared_ptr is empty, aborting");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    size_t taskbarElementIUnknownOffset = 0;
    bool frameHeightPatternRecognized = false;
#if defined(_M_X64) || defined(__x86_64__)
    {
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
    Wh_Log(L"GetTaskbarXamlRoot: Unknown architecture, using default offset 0x10");
    taskbarElementIUnknownOffset = 0x10;
    frameHeightPatternRecognized = true;
#endif
    if (!frameHeightPatternRecognized ||
        !IsReadableMemoryRange(
            static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
                taskbarElementIUnknownOffset,
            sizeof(IUnknown*))) {
        Wh_Log(L"GetTaskbarXamlRoot: FrameHeight pattern not recognized or offset unreadable, aborting");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                    taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        Wh_Log(L"GetTaskbarXamlRoot: taskbarElementIUnknown is null, aborting");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    void* vtable = nullptr;
    SIZE_T copied = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), taskbarElementIUnknown, &vtable, sizeof(vtable), &copied) ||
        copied != sizeof(vtable) || !IsReadableMemoryRange(vtable, 3 * sizeof(void*))) {
        Wh_Log(L"GetTaskbarXamlRoot: invalid taskbar COM object; refusing to query it");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    void* methods[3]{};
    bool validMethods = ReadProcessMemory(GetCurrentProcess(), vtable, methods, sizeof(methods), &copied) && copied == sizeof(methods);
    for (void* method : methods) {
        MEMORY_BASIC_INFORMATION memory{};
        validMethods = validMethods && method && VirtualQuery(method, &memory, sizeof(memory)) &&
            memory.State == MEM_COMMIT && !(memory.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
            (memory.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
    }
    if (!validMethods) {
        Wh_Log(L"GetTaskbarXamlRoot: invalid IUnknown methods; taskbar layout may be unsupported");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
    FrameworkElement taskbarElement{nullptr};
    HRESULT hr = taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    if (SUCCEEDED(hr) && result) {
        g_cachedTaskbarXamlRoot = result;
        g_cachedTaskbarRootWindow = hTaskbarWnd;
        return result;
    }
    return nullptr;
}
static const wchar_t* GetGlyphWithStyle(int cmd, const std::wstring& style, bool isPlaying = false) {
    bool isFluent = (style == L"fluent_outline" || style == L"fluent_filled");
    bool isFilled = (style == L"fluent_filled" || style == L"mdl2_filled");
    switch (cmd) {
        case 1:
            if (isFilled) return L"";
            return L"";
        case 2:
            if (isPlaying) {
                if (isFluent && isFilled) return L"";
                if (!isFluent && isFilled) return L"";
                return L"";
            } else {
                if (isFilled) return L"";
                return L"";
            }
        case 3:
            if (isFilled) return L"";
            return L"";
        case 5:
            if (isFilled) return L"";
            return L"";
        case 6:
            if (isFilled) return L"";
            return L"";
        case 7:
            return L"";
        case 8: {
            RepeatMode mode = g_repeatMode.load();
            switch (mode) {
                case RepeatMode::Off: return L"";
                case RepeatMode::All: return L"";
                case RepeatMode::One: return L"";
            }
        }
        case 9:
            return L"";
        case 10: return L"";
        case 11: return L"";
        case 12: return L"";
    }
    return L"";
}
static const wchar_t* GetGlyph(int cmd, bool isPlaying = false) {
    return GetGlyphWithStyle(cmd, g_settings.iconStyle, isPlaying);
}
static TextBlock MakeIconText(const wchar_t* glyph, double sz, winrt::Windows::UI::Color c) {
    TextBlock t;
    t.Text(glyph);
    t.FontSize(sz);
    t.Foreground(MakeBrush(c));
    t.VerticalAlignment(VerticalAlignment::Center);
    t.HorizontalAlignment(HorizontalAlignment::Center);
    bool useFluent = (g_settings.iconStyle == L"fluent_outline" || g_settings.iconStyle == L"fluent_filled");
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
static Button MakeControlButton(int cmd, bool isPlaying, winrt::Windows::UI::Color iconColor) {
    Button btn;
    try {
        if (cmd < 1 || cmd > 9) {
            Wh_Log(L"MakeControlButton: Invalid command %d, defaulting to 2 (play/pause)", cmd);
            cmd = 2;
        }
        btn.Width((double)g_settings.buttonSize);
        btn.Height((double)g_settings.buttonSize);
        btn.Padding({1,1,1,1});
        btn.CornerRadius({
            g_settings.buttonCornerRadiusTL,
            g_settings.buttonCornerRadiusTR,
            g_settings.buttonCornerRadiusBR,
            g_settings.buttonCornerRadiusBL
        });
        btn.BorderThickness({0,0,0,0});
        btn.VerticalAlignment(VerticalAlignment::Center);
        btn.HorizontalAlignment(HorizontalAlignment::Center);
        const wchar_t* labels[] = {L"", L"Previous track", L"Play / pause", L"Next track", L"Stop", L"Back 5 seconds", L"Forward 5 seconds", L"Shuffle", L"Repeat", L"Switch media session"};
        Automation::AutomationProperties::SetName(btn, labels[cmd]);
        ToolTipService::SetToolTip(btn, winrt::box_value(winrt::hstring(labels[cmd])));
        const wchar_t* glyph = GetGlyph(cmd, isPlaying);
        double opacity = 1.0;
        if (cmd == 7 && !g_shuffleEnabled.load()) {
            opacity = 0.4;
        }
        auto iconText = MakeIconText(glyph, (double)g_settings.buttonIconSize, iconColor);
        iconText.Opacity(opacity);
        btn.Content(winrt::box_value(iconText));
        btn.Click([cmd](auto const&, auto const&) {
            if (!g_unloading) {
                try {
                    SendMediaCommandAsync(cmd);
                    DispatchMediaUpdate();
                } catch (...) {
                    Wh_Log(L"MakeControlButton: Exception in Click handler for cmd %d", cmd);
                }
            }
        });
        ApplyFluentMediaButtonStyle(btn);

        auto isPressed = std::make_shared<bool>(false);
        auto isHovered = std::make_shared<bool>(false);

        auto updateBtnVisualState = [weak = winrt::make_weak(btn), isPressed, isHovered]() {
            auto btn = weak.get();
            if (!btn) return;
            try {
                GoToCommonState(btn, IsHoverEffectEnabled(g_settings.mediaButtonsHoverEffectMode), *isPressed, *isHovered);
            } catch (...) {

            }
        };

        RunWhenButtonReady(btn, [btn, updateBtnVisualState]() {
            try {
                SetupMediaButtonCommonStates(btn);
                updateBtnVisualState();
            } catch (...) {
                Wh_Log(L"MakeControlButton: Exception in RunWhenButtonReady");
            }
        });

        btn.PointerEntered([isHovered, updateBtnVisualState](auto const&, auto const&) {
            *isHovered = true;
            updateBtnVisualState();
        });

        btn.PointerExited([isHovered, updateBtnVisualState](auto const&, auto const&) {
            *isHovered = false;
            updateBtnVisualState();
        });

        btn.AddHandler(UIElement::PointerPressedEvent(), winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
            [isPressed, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (auto elem = sender.template try_as<UIElement>()) {
                elem.CapturePointer(e.Pointer());
            }
            *isPressed = true;
            updateBtnVisualState();
        })), true);

        btn.AddHandler(UIElement::PointerReleasedEvent(), winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
            [isPressed, isHovered, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            bool actuallyHovered = false;

            if (auto elem = sender.template try_as<UIElement>()) {
                elem.ReleasePointerCapture(e.Pointer());
                try {
                    auto bounds = elem.RenderSize();
                    auto pos = e.GetCurrentPoint(elem).Position();
                    actuallyHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                } catch (...) {}
            }
            *isPressed = false;
            *isHovered = actuallyHovered;
            updateBtnVisualState();
            e.Handled(true);
        })), true);

        btn.PointerCanceled([isPressed, isHovered, updateBtnVisualState](auto const&, auto const&) {
            *isPressed = false;
            *isHovered = false;
            updateBtnVisualState();
        });

        btn.PointerCaptureLost([isPressed, isHovered, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (auto elem = sender.template try_as<UIElement>()) {
                try {
                    auto bounds = elem.RenderSize();
                    auto pos = e.GetCurrentPoint(elem).Position();
                    *isHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                } catch (...) { *isHovered = false; }
            }
            updateBtnVisualState();
        });
    } catch (...) {
        Wh_Log(L"MakeControlButton: Exception while creating button");
    }
    return btn;
}
static void AddLayoutAnchorOverlay(Grid const& target, const wchar_t* name, winrt::Windows::UI::Color color) {
    if (!target || !g_settings.showLayoutAnchors) return;
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
static MenuFlyoutItem MakeActionContextMenuItem(const wchar_t* glyph, const wchar_t* label,
                                                std::function<void()> onClick) {
    MenuFlyoutItem item;
    item.Text(label);
    item.IsEnabled(true);
    try {
        FontIcon icon;
        icon.Glyph(glyph);
        icon.FontSize((double)g_settings.buttonIconSize);
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
    item.Click([onClick](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
        if (g_unloading) return;
        try {
            onClick();
        } catch (...) {}
    });
    return item;
}
static void OpenWindhawk() {
    SpawnTrackedWorker([]() {
        wchar_t expanded[MAX_PATH]{};
        ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windhawk\\windhawk.exe", expanded, MAX_PATH);
        DWORD attrs = GetFileAttributesW(expanded);
        if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            ShellExecuteW(nullptr, L"open", expanded, nullptr, nullptr, SW_SHOWNORMAL);
        } else {
            ShellExecuteW(nullptr, L"open", L"windhawk.exe", nullptr, nullptr, SW_SHOWNORMAL);
        }
    });
}
static MenuFlyoutItem MakeMediaContextMenuItem(int cmd, const wchar_t* label, bool enabled, double iconOpacity) {
    MenuFlyoutItem item;
    item.Text(label);
    item.IsEnabled(enabled);
    try {
        FontIcon icon;
        icon.Glyph(GetGlyphWithStyle(cmd, ContextMenuIconStyle()));
        icon.FontSize((double)g_settings.buttonIconSize);
        bool useFluent = (ContextMenuIconStyle() == L"fluent_outline" || ContextMenuIconStyle() == L"fluent_filled");
        try {
            icon.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
        } catch (...) {
            try {
                icon.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
            } catch (...) {}
        }
        icon.Foreground(MakeBrush(ContextMenuIconColor()));
        icon.Opacity(iconOpacity);
        item.Icon(icon);
    } catch (...) {}
    item.Click([cmd](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
        if (g_unloading) return;
        try {
            SendMediaCommandAsync(cmd);
            DispatchMediaUpdate();
        } catch (...) {}
    });
    return item;
}
static void ShowMediaContextMenu(FrameworkElement const& target) {
    if (!target || g_unloading) return;
    try {
        bool canSkipPrevious = true, canSkipNext = true;
        bool canShuffle = true, canRepeat = true, canSeek = true;
        bool shuffleEnabled = false;
        {
            std::lock_guard<std::mutex> lk(g_mediaMtx);
            canSkipPrevious = g_media.canSkipPrevious;
            canSkipNext     = g_media.canSkipNext;
            canShuffle      = g_media.canShuffle;
            canRepeat       = g_media.canRepeat;
            canSeek         = g_media.canSeek;
            shuffleEnabled  = g_shuffleEnabled.load();
        }
        MenuFlyout menu;
        try {
            menu.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
        } catch (...) {}
        RepeatMode curRepeat = g_repeatMode.load();
        auto makeRepeatSubItem = [&](int cmd, const wchar_t* label, const wchar_t* glyph, bool isCurrent) {
            MenuFlyoutItem ri;
            ri.Text(label);
            ri.IsEnabled(canRepeat);
            try {
                FontIcon ic;
                ic.Glyph(glyph);
                ic.FontSize((double)g_settings.buttonIconSize);
                bool useFluent = (ContextMenuIconStyle() == L"fluent_outline" || ContextMenuIconStyle() == L"fluent_filled");
                try {
                    ic.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                } catch (...) {}
                ic.Foreground(MakeBrush(ContextMenuIconColor()));
                ic.Opacity(isCurrent ? 1.0 : 0.5);
                ri.Icon(ic);
            } catch (...) {}
            ri.Click([cmd](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
                if (g_unloading) return;
                try {
                    SendMediaCommandAsync(cmd);
                    DispatchMediaUpdate();
                } catch (...) {}
            });
            return ri;
        };
        bool addedSeparator = false;
        auto ensureSeparator = [&]() {
            if (!addedSeparator) {
                try {
                    Controls::MenuFlyoutSeparator sep;
                    menu.Items().Append(sep);
                } catch (...) {}
                addedSeparator = true;
            }
        };
        for (const auto& item : g_settings.contextMenuItems) {
            if (item == L"repeat") {
                if (g_settings.contextMenuRepeatStyle == L"toggle") {
                    const wchar_t* repeatLabel =
                        (curRepeat == RepeatMode::All) ? L"Repeat all" :
                        (curRepeat == RepeatMode::One) ? L"Repeat one" : L"Repeat off";
                    double repeatOpacity = canRepeat ? (curRepeat != RepeatMode::Off ? 1.0 : 0.4) : 0.4;
                    menu.Items().Append(MakeMediaContextMenuItem(8, repeatLabel, canRepeat, repeatOpacity));
                } else {
                    MenuFlyoutSubItem repeatSubMenu;
                    repeatSubMenu.Text(L"Repeat");
                    try {
                        FontIcon repeatIcon;
                        repeatIcon.Glyph(GetGlyphWithStyle(8, ContextMenuIconStyle()));
                        repeatIcon.FontSize((double)g_settings.buttonIconSize);
                        bool useFluent = (ContextMenuIconStyle() == L"fluent_outline" || ContextMenuIconStyle() == L"fluent_filled");
                        try {
                            repeatIcon.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                        } catch (...) {}
                        repeatIcon.Foreground(MakeBrush(ContextMenuIconColor()));
                        repeatIcon.Opacity(canRepeat ? 1.0 : 0.4);
                        repeatSubMenu.Icon(repeatIcon);
                    } catch (...) {}
                    repeatSubMenu.IsEnabled(canRepeat);
                    repeatSubMenu.Items().Append(makeRepeatSubItem(10, L"Repeat off", GetGlyphWithStyle(10, ContextMenuIconStyle()), curRepeat == RepeatMode::Off));
                    repeatSubMenu.Items().Append(makeRepeatSubItem(11, L"Repeat all", GetGlyphWithStyle(11, ContextMenuIconStyle()), curRepeat == RepeatMode::All));
                    repeatSubMenu.Items().Append(makeRepeatSubItem(12, L"Repeat one", GetGlyphWithStyle(12, ContextMenuIconStyle()), curRepeat == RepeatMode::One));
                    menu.Items().Append(repeatSubMenu);
                }
            } else if (item == L"shuffle") {
                if (g_settings.contextMenuShuffleStyle == L"toggle") {
                    const wchar_t* shuffleLabel = shuffleEnabled ? L"Shuffle on" : L"Shuffle off";
                    double shuffleOpacity = canShuffle ? (shuffleEnabled ? 1.0 : 0.4) : 0.4;
                    menu.Items().Append(MakeMediaContextMenuItem(7, shuffleLabel, canShuffle, shuffleOpacity));
                } else {
                    MenuFlyoutSubItem shuffleSubMenu;
                    shuffleSubMenu.Text(L"Shuffle");
                    try {
                        FontIcon shuffleIcon;
                        shuffleIcon.Glyph(GetGlyphWithStyle(7, ContextMenuIconStyle()));
                        shuffleIcon.FontSize((double)g_settings.buttonIconSize);
                        bool useFluent = (ContextMenuIconStyle() == L"fluent_outline" || ContextMenuIconStyle() == L"fluent_filled");
                        try {
                            shuffleIcon.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                        } catch (...) {}
                        shuffleIcon.Foreground(MakeBrush(ContextMenuIconColor()));
                        shuffleIcon.Opacity(canShuffle ? 1.0 : 0.4);
                        shuffleSubMenu.Icon(shuffleIcon);
                    } catch (...) {}
                    shuffleSubMenu.IsEnabled(canShuffle);
                    MenuFlyoutItem shuffleOffItem;
                    shuffleOffItem.Text(L"Shuffle off");
                    shuffleOffItem.IsEnabled(canShuffle);
                    try {
                        FontIcon ic;
                        ic.Glyph(GetGlyphWithStyle(7, ContextMenuIconStyle()));
                        ic.FontSize((double)g_settings.buttonIconSize);
                        bool useFluent = (ContextMenuIconStyle() == L"fluent_outline" || ContextMenuIconStyle() == L"fluent_filled");
                        try { ic.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets")); } catch (...) {}
                        ic.Foreground(MakeBrush(ContextMenuIconColor()));
                        ic.Opacity(!shuffleEnabled ? 1.0 : 0.5);
                        shuffleOffItem.Icon(ic);
                    } catch (...) {}
                    shuffleOffItem.Click([](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
                        if (g_unloading) return;
                        try {
                            if (g_shuffleEnabled.load()) {
                                SendMediaCommandAsync(7);
                                DispatchMediaUpdate();
                            }
                        } catch (...) {}
                    });
                    MenuFlyoutItem shuffleOnItem;
                    shuffleOnItem.Text(L"Shuffle on");
                    shuffleOnItem.IsEnabled(canShuffle);
                    try {
                        FontIcon ic;
                        ic.Glyph(GetGlyphWithStyle(7, ContextMenuIconStyle()));
                        ic.FontSize((double)g_settings.buttonIconSize);
                        bool useFluent = (ContextMenuIconStyle() == L"fluent_outline" || ContextMenuIconStyle() == L"fluent_filled");
                        try { ic.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets")); } catch (...) {}
                        ic.Foreground(MakeBrush(ContextMenuIconColor()));
                        ic.Opacity(shuffleEnabled ? 1.0 : 0.5);
                        shuffleOnItem.Icon(ic);
                    } catch (...) {}
                    shuffleOnItem.Click([](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
                        if (g_unloading) return;
                        try {
                            if (!g_shuffleEnabled.load()) {
                                SendMediaCommandAsync(7);
                                DispatchMediaUpdate();
                            }
                        } catch (...) {}
                    });
                    shuffleSubMenu.Items().Append(shuffleOffItem);
                    shuffleSubMenu.Items().Append(shuffleOnItem);
                    menu.Items().Append(shuffleSubMenu);
                }
            } else if (item == L"forward") {
                menu.Items().Append(MakeMediaContextMenuItem(6, L"Forward 5s", canSeek, canSeek ? 1.0 : 0.4));
            } else if (item == L"rewind") {
                menu.Items().Append(MakeMediaContextMenuItem(5, L"Rewind 5s", canSeek, canSeek ? 1.0 : 0.4));
            } else if (item == L"next") {
                menu.Items().Append(MakeMediaContextMenuItem(3, L"Next Track", canSkipNext, canSkipNext ? 1.0 : 0.4));
            } else if (item == L"prev") {
                menu.Items().Append(MakeMediaContextMenuItem(1, L"Previous Track", canSkipPrevious, canSkipPrevious ? 1.0 : 0.4));
            } else if (item == L"switch_sessions") {
                menu.Items().Append(MakeMediaContextMenuItem(9, L"Switch Sessions", g_sessionCount.load() > 1, g_sessionCount.load() > 1 ? 1.0 : 0.4));
            } else if (item == L"open_app") {
                menu.Items().Append(MakeActionContextMenuItem(L"\uE8A7", L"Open media app", []() {
                    ExecuteMediaAction(L"open_app");
                }));
            }
        }
        if (g_settings.showRestartButton || g_settings.showOpenWindhawk) {
            ensureSeparator();
        }
        if (g_settings.showRestartButton) {
            menu.Items().Append(MakeActionContextMenuItem(L"\uE72C", L"Restart Player", []() {
                try {
                    StopVizTimer();
                    g_vizCurrentlyVisible = false;
                    RemovePlayerGrid();
                    auto dispatcher = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher();
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [=]() {
                        try {
                            InjectPlayerGrid();
                        } catch (...) {
                            Wh_Log(L"Restart Player: Exception in InjectPlayerGrid");
                        }
                    });
                } catch (...) {
                    Wh_Log(L"Restart Player: exception");
                }
            }));
        }
        if (g_settings.showOpenWindhawk) {
            menu.Items().Append(MakeActionContextMenuItem(L"\uE713", L"Open Windhawk", []() {
                OpenWindhawk();
            }));
        }
        Controls::Primitives::FlyoutShowOptions flyoutOptions;
        flyoutOptions.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
        flyoutOptions.ShowMode(Controls::Primitives::FlyoutShowMode::Standard);
        flyoutOptions.ExclusionRect(winrt::Windows::Foundation::Rect{
            0.f, 0.f,
            (float)target.ActualWidth(),
            (float)target.ActualHeight() + 8.f
        });

        menu.Closed([](auto const&, auto const&) {
            try {
                if (g_playerButtonStateUpdater) {
                    g_playerButtonStateUpdater();
                }
                if (g_playerGrid && !g_unloading && !g_applyingSettings) {
                    if (auto bgFe = FindChildByName(g_playerGrid, L"FluentMedia_Background")) {
                        if (auto bgBorder = bgFe.try_as<Border>()) {
                            auto& bgType = g_settings.backgroundType;
                            if (bgType == L"acrylic" || bgType == L"solid" || bgType == L"gradient" ||
                                bgType == L"mica" || bgType == L"mica_alt") {
                                bgBorder.Background(MakeBackgroundBrush());
                                bgBorder.Visibility(Visibility::Visible);
                                bgBorder.Opacity(1.0);
                            }
                        }
                    }
                }
            } catch (...) {
                Wh_Log(L"ShowMediaContextMenu: Closed handler exception");
            }
        });

        menu.ShowAt(target, flyoutOptions);
    } catch (...) {
        Wh_Log(L"ShowMediaContextMenu: exception");
    }
}

[[clang::no_destroy]] static winrt::Windows::UI::Xaml::Controls::Flyout g_miniPlayerFlyout{nullptr};
[[clang::no_destroy]] static Controls::Image g_miniPlayerArtRef{nullptr};
[[clang::no_destroy]] static TextBlock       g_miniPlayerTitleRef{nullptr};
[[clang::no_destroy]] static TextBlock       g_miniPlayerArtistRef{nullptr};
[[clang::no_destroy]] static Button          g_miniPlayerPlayBtnRef{nullptr};
[[clang::no_destroy]] static Button          g_miniPlayerPrevBtnRef{nullptr};
[[clang::no_destroy]] static Button          g_miniPlayerNextBtnRef{nullptr};
[[clang::no_destroy]] static Button          g_miniPlayerShuffleBtnRef{nullptr};
[[clang::no_destroy]] static Button          g_miniPlayerRepeatBtnRef{nullptr};
[[clang::no_destroy]] static StackPanel      g_miniPlayerSessionListRef{nullptr};
[[clang::no_destroy]] static Border          g_miniPlayerSessionListSepRef{nullptr};

struct MiniSessionInfo {
    std::wstring id;
    std::wstring title;
    std::wstring artist;
    std::vector<BYTE> thumbBytes;
};
static std::vector<MiniSessionInfo> g_miniSessionInfos;
static std::mutex                   g_miniSessionMtx;
static std::vector<BYTE> g_miniPlayerCachedThumb;
static std::vector<size_t> g_miniSessionCachedThumbSizes;
static std::atomic<int> g_miniSessionHoveredIndex{-1};

static void LoadMiniPlayerArtBitmap(Controls::Image const& img, std::vector<BYTE> const& thumbBytes) {
    if (!img || thumbBytes.empty()) return;
    try {
        IStream* pRawStream = SHCreateMemStream(
            thumbBytes.data(), static_cast<UINT>(thumbBytes.size()));
        if (!pRawStream) return;
        winrt::com_ptr<IStream> comStream;
        comStream.attach(pRawStream);
        winrt::Windows::Storage::Streams::IRandomAccessStream rasStream{nullptr};
        ::CreateRandomAccessStreamOverStream(
            comStream.get(), BSOS_DEFAULT,
            winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(),
            winrt::put_abi(rasStream));
        if (!rasStream) return;
        BitmapImage bmp;
        img.Source(bmp);
        bmp.SetSourceAsync(rasStream);
        img.Visibility(Visibility::Visible);
    } catch (...) {}
}

static std::vector<std::shared_ptr<std::atomic<bool>>> g_miniSessionCurrentFlags;

static void RefreshMiniPlayerFlyoutUI() {
    if (!g_miniPlayerFlyoutOpen) return;
    std::wstring title, artist;
    bool isPlaying = false;
    bool hasMedia = false;
    bool canSkipPrevious = true, canSkipNext = true;
    bool canShuffle = true, canRepeat = true;
    std::vector<BYTE> thumbBytes;
    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        title           = g_media.title;
        artist          = g_media.artist;
        isPlaying       = g_media.isPlaying;
        hasMedia        = g_media.hasMedia;
        canSkipPrevious = g_media.canSkipPrevious;
        canSkipNext     = g_media.canSkipNext;
        canShuffle      = g_media.canShuffle;
        canRepeat       = g_media.canRepeat;
        thumbBytes      = g_media.thumbnailBytes;
    }
    std::wstring currentId;
    {
        GlobalSystemMediaTransportControlsSession sessionCopy{nullptr};
        {
            std::lock_guard<std::mutex> lk(g_sessionMtx);
            sessionCopy = g_currentSession;
        }
        if (sessionCopy) {
            try { currentId = std::wstring(sessionCopy.SourceAppUserModelId()); } catch (...) {}
        }
    }

    try {
        if (g_miniPlayerTitleRef) {
            std::wstring displayTitle;
            if (!hasMedia) {
                displayTitle = g_settings.noMediaTitleText;
            } else if (title.empty()) {
                displayTitle = g_settings.emptyTitleText;
            } else {
                displayTitle = title;
            }
            g_miniPlayerTitleRef.Text(winrt::hstring(displayTitle));
        }
        if (g_miniPlayerArtistRef) {
            std::wstring trimmedArtist = artist;
            trimmedArtist.erase(0, trimmedArtist.find_first_not_of(L" \t\n\r"));
            trimmedArtist.erase(trimmedArtist.find_last_not_of(L" \t\n\r") + 1);
            bool isEmpty = trimmedArtist.empty();

            winrt::hstring newArtist = isEmpty ? winrt::hstring(L"") : winrt::hstring(artist);
            g_miniPlayerArtistRef.Text(newArtist);
            g_miniPlayerArtistRef.Visibility(isEmpty ? Visibility::Collapsed : Visibility::Visible);
        }

        if (g_miniPlayerPlayBtnRef) {
            g_miniPlayerPlayBtnRef.IsEnabled(hasMedia);
            g_miniPlayerPlayBtnRef.Opacity(hasMedia ? 1.0 : 0.35);
            if (auto icon = g_miniPlayerPlayBtnRef.Content().try_as<TextBlock>()) {
                const wchar_t* glyph = isPlaying ? L"\uE62E" : L"\uF5B0";
                icon.Text(winrt::hstring(glyph));
            }
        }

        if (g_miniPlayerPrevBtnRef) {
            bool enabled = hasMedia && canSkipPrevious;
            g_miniPlayerPrevBtnRef.IsEnabled(enabled);
            g_miniPlayerPrevBtnRef.Opacity(enabled ? 1.0 : 0.35);
        }

        if (g_miniPlayerNextBtnRef) {
            bool enabled = hasMedia && canSkipNext;
            g_miniPlayerNextBtnRef.IsEnabled(enabled);
            g_miniPlayerNextBtnRef.Opacity(enabled ? 1.0 : 0.35);
        }

        if (g_miniPlayerShuffleBtnRef) {
            bool enabled = hasMedia && canShuffle;
            g_miniPlayerShuffleBtnRef.IsEnabled(enabled);
            if (auto icon = g_miniPlayerShuffleBtnRef.Content().try_as<TextBlock>()) {
                icon.Opacity(enabled && g_shuffleEnabled.load() ? 1.0 : 0.40);
            }
        }

        if (g_miniPlayerRepeatBtnRef) {
            bool enabled = hasMedia && canRepeat;
            g_miniPlayerRepeatBtnRef.IsEnabled(enabled);
            if (auto icon = g_miniPlayerRepeatBtnRef.Content().try_as<TextBlock>()) {
                RepeatMode mode = g_repeatMode.load();
                icon.Text(mode == RepeatMode::One ? L"\uE8ED" :
                          mode == RepeatMode::All ? L"\uE8EE" : L"\uF5E7");
                icon.Opacity(enabled && mode != RepeatMode::Off ? 1.0 : 0.40);
            }
        }

        if (g_miniPlayerArtRef) {
            bool sameThumb = (!thumbBytes.empty() && thumbBytes == g_miniPlayerCachedThumb);
            if (!thumbBytes.empty() && !sameThumb) {
                LoadMiniPlayerArtBitmap(g_miniPlayerArtRef, thumbBytes);
                g_miniPlayerCachedThumb = thumbBytes;
            } else if (thumbBytes.empty()) {
                try { g_miniPlayerArtRef.Source(nullptr); } catch (...) {}
                g_miniPlayerArtRef.Visibility(Visibility::Collapsed);
                g_miniPlayerCachedThumb.clear();
            }
        }
        {
            std::vector<MiniSessionInfo> infos;
            {
                std::lock_guard<std::mutex> lk(g_miniSessionMtx);
                infos = g_miniSessionInfos;
            }

            bool noMusic = (!hasMedia || title.empty()) && infos.empty();
            bool hasOtherSessions = !infos.empty();
            if (g_miniPlayerSessionListSepRef) {
                g_miniPlayerSessionListSepRef.Visibility(
                    (noMusic || !hasOtherSessions || g_settings.hideMediaSessionsList) ? Visibility::Collapsed : Visibility::Visible);
            }

            if (g_miniPlayerSessionListRef) {
                auto sessionList = g_miniPlayerSessionListRef;
                Visibility listVisibility = (noMusic || !hasOtherSessions) ? Visibility::Collapsed : Visibility::Visible;
                sessionList.Visibility(listVisibility);

                uint32_t childCount = sessionList.Children().Size();
                if (infos.empty() || childCount != infos.size()) {
                    sessionList.Children().Clear();
                    {
                        std::lock_guard<std::mutex> lk(g_miniSessionMtx);
                        g_miniSessionCurrentFlags.clear();
                    }
                    g_miniSessionCachedThumbSizes.clear();
                    g_miniSessionCachedThumbSizes.resize(infos.size(), SIZE_MAX);
                HWND tw = g_taskbarWnd;
                for (size_t i = 0; i < infos.size(); ++i) {
                    bool isCurrent = (infos[i].id == currentId);
                    auto rowBtn = BuildSessionRowButton(infos[i], isCurrent, tw, static_cast<int>(i));
                    sessionList.Children().Append(rowBtn);
                    g_miniSessionCachedThumbSizes[i] = infos[i].thumbBytes.size();
                }
            } else {
                bool isLight = IsSystemLightTheme();
                int hoveredIdx = g_miniSessionHoveredIndex.load();

                for (uint32_t i = 0; i < childCount && i < infos.size(); ++i) {
                    bool isCurrent = (infos[i].id == currentId);
                    bool wasCurrent = isCurrent;
                    if (i < g_miniSessionCurrentFlags.size() && g_miniSessionCurrentFlags[i]) {
                        wasCurrent = g_miniSessionCurrentFlags[i]->load();
                        g_miniSessionCurrentFlags[i]->store(isCurrent);
                    }
                    if (auto btn = sessionList.Children().GetAt(i).try_as<Button>()) {
                        if (auto hb = btn.Content().try_as<Border>()) {
                            if (hoveredIdx != static_cast<int>(i)) {
                                hb.Background(MakeBrush(isCurrent
                                    ? (isLight ? winrt::Windows::UI::Color{0x20, 0x00, 0x00, 0x00}
                                               : winrt::Windows::UI::Color{0x2C, 0xFF, 0xFF, 0xFF})
                                    : winrt::Windows::UI::Color{0x00, 0x00, 0x00, 0x00}));
                            }
                            if (auto grid = hb.Child().try_as<Grid>()) {
                                for (uint32_t j = 0; j < grid.Children().Size(); ++j) {
                                    auto child = grid.Children().GetAt(j);
                                    if (auto pillRect = child.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>()) {
                                        if (wasCurrent != isCurrent) {
                                            AnimateSessionPill(pillRect, isCurrent, /*animate*/ true);
                                        }
                                        continue;
                                    }
                                    if (auto artBdr = child.try_as<Border>()) {
                                        size_t newSize = infos[i].thumbBytes.size();
                                        size_t cachedSize = (i < g_miniSessionCachedThumbSizes.size())
                                            ? g_miniSessionCachedThumbSizes[i] : SIZE_MAX;
                                        if (newSize != cachedSize) {
                                            if (i < g_miniSessionCachedThumbSizes.size())
                                                g_miniSessionCachedThumbSizes[i] = newSize;
                                            if (!infos[i].thumbBytes.empty()) {
                                                try {
                                                    IStream* ps = SHCreateMemStream(
                                                        infos[i].thumbBytes.data(),
                                                        (UINT)infos[i].thumbBytes.size());
                                                    if (ps) {
                                                        winrt::com_ptr<IStream> cs; cs.attach(ps);
                                                        winrt::Windows::Storage::Streams::IRandomAccessStream rs{nullptr};
                                                        ::CreateRandomAccessStreamOverStream(
                                                            cs.get(), BSOS_DEFAULT,
                                                            winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(),
                                                            winrt::put_abi(rs));
                                                        if (rs) {
                                                            Controls::Image rowImg;
                                                            rowImg.Width(32);
                                                            rowImg.Height(32);
                                                            rowImg.Stretch(Stretch::Uniform);
                                                            BitmapImage bmp;
                                                            bmp.DecodePixelHeight(32);
                                                            rowImg.Source(bmp);
                                                            bmp.SetSourceAsync(rs);
                                                            artBdr.Child(rowImg);
                                                        }
                                                    }
                                                } catch (...) {}
                                            } else {
                                                try { artBdr.Child(nullptr); } catch (...) {}
                                            }
                                        }
                                        continue;
                                    }
                                    if (auto sp = child.try_as<StackPanel>()) {
                                        std::wstring disp = infos[i].title.empty() ? infos[i].id : infos[i].title;
                                        if (sp.Children().Size() > 0) {
                                            if (auto tb = sp.Children().GetAt(0).try_as<TextBlock>()) {
                                                tb.Text(winrt::hstring(disp));
                                            }
                                        }
                                        if (sp.Children().Size() > 1) {
                                            if (auto tb = sp.Children().GetAt(1).try_as<TextBlock>()) {
                                                tb.Text(winrt::hstring(infos[i].artist));
                                                tb.Visibility(infos[i].artist.empty()
                                                    ? Visibility::Collapsed
                                                    : Visibility::Visible);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            }
        }
    } catch (...) {}
}

static void AnimateSessionPill(winrt::Windows::UI::Xaml::Shapes::Rectangle const& pill, bool isCurrent, bool animate) {
    using namespace winrt::Windows::UI::Xaml::Media;
    using namespace winrt::Windows::UI::Xaml::Media::Animation;

    if (isCurrent) {
        try {
            auto uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
            auto accentColor = uiSettings.GetColorValue(
                winrt::Windows::UI::ViewManagement::UIColorType::Accent);
            BYTE r = std::min(255, (int)accentColor.R + 60);
            BYTE g = std::min(255, (int)accentColor.G + 60);
            BYTE b = std::min(255, (int)accentColor.B + 60);

            pill.Fill(MakeBrush({0xFF, r, g, b}));
        } catch (...) {
            pill.Fill(MakeBrush({0xFF, 0x40, 0xA0, 0xFF}));
        }
    }

    auto st = pill.RenderTransform().try_as<ScaleTransform>();
    if (!st) {
        st = ScaleTransform();
        pill.RenderTransformOrigin({0.5, 0.5});
        pill.RenderTransform(st);
    }

    if (!animate) {
        pill.Opacity(isCurrent ? 1.0 : 0.0);
        st.ScaleX(1.0);
        st.ScaleY(isCurrent ? 1.0 : 0.0);
        return;
    }

    if (isCurrent) {
        pill.Opacity(0.0);
        st.ScaleX(1.0);
        st.ScaleY(0.0);

        try {
            Storyboard sb;

            DoubleAnimation fadeAnim;
            fadeAnim.From(0.0);
            fadeAnim.To(1.0);
            fadeAnim.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(100)));
            Storyboard::SetTarget(fadeAnim, pill);
            Storyboard::SetTargetProperty(fadeAnim, L"Opacity");
            sb.Children().Append(fadeAnim);
            DoubleAnimation scaleAnim;
            scaleAnim.From(0.0);
            scaleAnim.To(1.0);
            scaleAnim.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(350)));
            auto ease = BackEase();
            ease.EasingMode(EasingMode::EaseOut);
            ease.Amplitude(0.6);
            scaleAnim.EasingFunction(ease);
            Storyboard::SetTarget(scaleAnim, pill);
            Storyboard::SetTargetProperty(scaleAnim,
                L"(UIElement.RenderTransform).(ScaleTransform.ScaleY)");
            sb.Children().Append(scaleAnim);

            sb.Begin();
        } catch (...) {
            pill.Opacity(1.0);
            st.ScaleX(1.0);
            st.ScaleY(1.0);
        }
    } else {
        try {
            Storyboard sb;

            DoubleAnimation scaleAnim;
            scaleAnim.To(0.0);
            scaleAnim.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(150)));
            auto ease = CircleEase();
            ease.EasingMode(EasingMode::EaseIn);
            scaleAnim.EasingFunction(ease);
            Storyboard::SetTarget(scaleAnim, pill);
            Storyboard::SetTargetProperty(scaleAnim,
                L"(UIElement.RenderTransform).(ScaleTransform.ScaleY)");
            sb.Children().Append(scaleAnim);

            DoubleAnimation fadeAnim;
            fadeAnim.To(0.0);
            fadeAnim.BeginTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(100)));
            fadeAnim.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(80)));
            Storyboard::SetTarget(fadeAnim, pill);
            Storyboard::SetTargetProperty(fadeAnim, L"Opacity");
            sb.Children().Append(fadeAnim);

            sb.Begin();
        } catch (...) {
            pill.Opacity(0.0);
            st.ScaleX(1.0);
            st.ScaleY(0.0);
        }
    }
}

static Button BuildSessionRowButton(const MiniSessionInfo& info, bool isCurrent, HWND taskbarWnd, int rowIndex) {
    bool isLight = IsSystemLightTheme();

    winrt::Windows::UI::Color bgSelected    = isLight
        ? winrt::Windows::UI::Color{0x20, 0x00, 0x00, 0x00}
        : winrt::Windows::UI::Color{0x2C, 0xFF, 0xFF, 0xFF};
    winrt::Windows::UI::Color bgUnselected  = winrt::Windows::UI::Color{0x00, 0x00, 0x00, 0x00};
    winrt::Windows::UI::Color bgNormal = isCurrent ? bgSelected : bgUnselected;

    winrt::Windows::UI::Color bgHover = isLight
        ? winrt::Windows::UI::Color{0x0E, 0x00, 0x00, 0x00}
        : winrt::Windows::UI::Color{0x14, 0xFF, 0xFF, 0xFF};
    winrt::Windows::UI::Color bgPressed = isLight
        ? winrt::Windows::UI::Color{0x18, 0x00, 0x00, 0x00}
        : winrt::Windows::UI::Color{0x20, 0xFF, 0xFF, 0xFF};

    auto isCurrentFlag = std::make_shared<std::atomic<bool>>(isCurrent);
    {
        std::lock_guard<std::mutex> lk(g_miniSessionMtx);
        if (static_cast<size_t>(rowIndex) >= g_miniSessionCurrentFlags.size()) {
            g_miniSessionCurrentFlags.resize(rowIndex + 1);
        }
        g_miniSessionCurrentFlags[rowIndex] = isCurrentFlag;
    }

    Border hoverBorder;
    hoverBorder.CornerRadius({4, 4, 4, 4});
    hoverBorder.Background(MakeBrush(bgNormal));
    hoverBorder.Height(48);
    hoverBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
    hoverBorder.Padding({0, 0, 12, 0});
    try {
        BrushTransition bt;
        bt.Duration(std::chrono::milliseconds(83));
        hoverBorder.BackgroundTransition(bt);
    } catch (...) {}

    Grid rowGrid;
    rowGrid.Height(48);
    rowGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
    ColumnDefinition cPill, c0, c1;
    cPill.Width({8, GridUnitType::Pixel});
    c0.Width({34, GridUnitType::Pixel});
    c1.Width({1,  GridUnitType::Star});
    rowGrid.ColumnDefinitions().Append(cPill);
    rowGrid.ColumnDefinitions().Append(c0);
    rowGrid.ColumnDefinitions().Append(c1);

    {
        winrt::Windows::UI::Xaml::Shapes::Rectangle pill;
        pill.Width(3);
        pill.Height(20);
        pill.RadiusX(1.5);
        pill.RadiusY(1.5);
        pill.Margin({0, 0, 0, 0});
        pill.VerticalAlignment(VerticalAlignment::Center);
        pill.HorizontalAlignment(HorizontalAlignment::Left);

        AnimateSessionPill(pill, isCurrent, /*animate*/ false);
        if (isCurrent) {
            pill.Loaded([pill](auto const&, auto const&) {
                AnimateSessionPill(pill, true, /*animate*/ true);
            });
        }

        Grid::SetColumn(pill, 0);
        rowGrid.Children().Append(pill);
    }

    Border rowArt;
    rowArt.Width(32); rowArt.Height(32);
    rowArt.CornerRadius({4, 4, 4, 4});
    rowArt.Margin({2, 0, 0, 0});
    rowArt.Background(MakeBrush({0x40, 0x80, 0x80, 0x80}));
    rowArt.VerticalAlignment(VerticalAlignment::Center);
    if (!info.thumbBytes.empty()) {
        try {
            IStream* ps = SHCreateMemStream(info.thumbBytes.data(), (UINT)info.thumbBytes.size());
            if (ps) {
                winrt::com_ptr<IStream> cs; cs.attach(ps);
                winrt::Windows::Storage::Streams::IRandomAccessStream rs{nullptr};
                ::CreateRandomAccessStreamOverStream(cs.get(), BSOS_DEFAULT,
                    winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(),
                    winrt::put_abi(rs));
                if (rs) {
                    Controls::Image rowImg;
                    rowImg.Stretch(Stretch::None);
                    rowImg.HorizontalAlignment(HorizontalAlignment::Center);
                    rowImg.VerticalAlignment(VerticalAlignment::Center);
                    BitmapImage bmp;
                    bmp.DecodePixelHeight(32);
                    rowImg.Source(bmp);
                    bmp.SetSourceAsync(rs);
                    rowArt.Child(rowImg);
                }
            }
        } catch (...) {}
    }
    Grid::SetColumn(rowArt, 1);
    rowGrid.Children().Append(rowArt);

    StackPanel rowText;
    rowText.Orientation(Orientation::Vertical);
    rowText.VerticalAlignment(VerticalAlignment::Center);
    rowText.HorizontalAlignment(HorizontalAlignment::Left);
    rowText.Margin({8, 0, 0, 0});
    rowText.MaxWidth(240);

    std::wstring displayTitle = info.title.empty() ? info.id : info.title;
    TextBlock rowTitle;
    rowTitle.Text(winrt::hstring(displayTitle));
    rowTitle.FontSize(12);
    rowTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    rowTitle.TextTrimming(TextTrimming::CharacterEllipsis);
    rowTitle.TextWrapping(TextWrapping::NoWrap);
    rowTitle.HorizontalAlignment(HorizontalAlignment::Left);
    rowTitle.TextAlignment(TextAlignment::Left);
    rowTitle.Foreground(MakeBrush(isLight
        ? winrt::Windows::UI::Color{0xFF, 0x10, 0x10, 0x10}
        : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF}));
    rowText.Children().Append(rowTitle);

    if (!info.artist.empty()) {
        TextBlock rowArtist;
        rowArtist.Text(winrt::hstring(info.artist));
        rowArtist.FontSize(10.5);
        rowArtist.Opacity(0.65);
        rowArtist.TextTrimming(TextTrimming::CharacterEllipsis);
        rowArtist.TextWrapping(TextWrapping::NoWrap);
        rowArtist.HorizontalAlignment(HorizontalAlignment::Left);
        rowArtist.TextAlignment(TextAlignment::Left);
        rowArtist.Margin({0, 2, 0, 0});
        rowArtist.Foreground(MakeBrush(isLight
            ? winrt::Windows::UI::Color{0xFF, 0x50, 0x50, 0x50}
            : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF}));
        rowText.Children().Append(rowArtist);
    }

    if (g_settings.showMiniPlayerBorder) {
        rowArt.BorderBrush(MakeBrush({0xFF, 0xFF, 0x40, 0x00}));
        rowArt.BorderThickness({1, 1, 1, 1});
        rowText.BorderBrush(MakeBrush({0xFF, 0x00, 0xFF, 0xFF}));
        rowText.BorderThickness({1, 1, 1, 1});
        hoverBorder.BorderBrush(MakeBrush({0xFF, 0xFF, 0x00, 0xFF}));
        hoverBorder.BorderThickness({1, 1, 1, 1});
    }

    Grid::SetColumn(rowText, 2);
    rowGrid.Children().Append(rowText);
    hoverBorder.Child(rowGrid);

    Button rowBtn;
    rowBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
    rowBtn.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    rowBtn.Padding({0, 0, 0, 0});
    rowBtn.Margin({4, 2, 4, 2});
    rowBtn.CornerRadius({4, 4, 4, 4});
    rowBtn.BorderThickness({0, 0, 0, 0});
    rowBtn.Background(MakeBrush({0x00, 0x00, 0x00, 0x00}));
    rowBtn.Content(hoverBorder);

    std::wstring capturedId = info.id;
    rowBtn.Click([capturedId](auto const&, auto const&) {
        if (g_unloading) return;
        SpawnTrackedWorker([capturedId]() {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            GlobalSystemMediaTransportControlsSession targetSession{nullptr};
            {
                GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
                {
                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                    mgr = g_sessionMgr;
                }
                if (mgr) {
                    try {
                        auto sessions = mgr.GetSessions();
                        for (auto const& s : sessions) {
                            try {
                                if (std::wstring(s.SourceAppUserModelId()) == capturedId) {
                                    targetSession = s;
                                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                                    g_userSwitchedSession = true;
                                    break;
                                }
                            } catch (...) {}
                        }
                    } catch (...) {}
                }
            }
            if (targetSession) {
                AttachToSession(targetSession);
            }
            winrt::uninit_apartment();
        });
    });

    ApplyFluentMediaButtonStyle(rowBtn);

    auto hBorderWeak  = winrt::make_weak(hoverBorder);
    auto brushSelected   = MakeBrush(bgSelected);
    auto brushUnselected = MakeBrush(bgUnselected);
    auto brushHover       = MakeBrush(bgHover);
    auto brushPressed     = MakeBrush(bgPressed);
    auto restingBrush = [isCurrentFlag, brushSelected, brushUnselected]() {
        return isCurrentFlag->load() ? brushSelected : brushUnselected;
    };

    rowBtn.PointerEntered([hBorderWeak, brushHover, rowIndex](auto const&, auto const&) {
        g_miniSessionHoveredIndex.store(rowIndex);
        if (auto b = hBorderWeak.get()) b.Background(brushHover);
    });
    rowBtn.PointerExited([hBorderWeak, restingBrush, rowIndex](auto const&, auto const&) {
        int expected = rowIndex;
        g_miniSessionHoveredIndex.compare_exchange_strong(expected, -1);
        if (auto b = hBorderWeak.get()) b.Background(restingBrush());
    });
    rowBtn.AddHandler(UIElement::PointerPressedEvent(), winrt::box_value(
        winrt::Windows::UI::Xaml::Input::PointerEventHandler(
        [hBorderWeak, brushPressed](auto const& sender,
        winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
        if (auto elem = sender.template try_as<UIElement>()) elem.CapturePointer(e.Pointer());
        if (auto b = hBorderWeak.get()) b.Background(brushPressed);
    })), true);
    rowBtn.AddHandler(UIElement::PointerReleasedEvent(), winrt::box_value(
        winrt::Windows::UI::Xaml::Input::PointerEventHandler(
        [hBorderWeak, brushHover](auto const& sender,
        winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
        if (auto elem = sender.template try_as<UIElement>()) elem.ReleasePointerCapture(e.Pointer());
        if (auto b = hBorderWeak.get()) b.Background(brushHover);
    })), true);
    rowBtn.PointerCanceled([hBorderWeak, restingBrush, rowIndex](auto const&, auto const&) {
        int expected = rowIndex;
        g_miniSessionHoveredIndex.compare_exchange_strong(expected, -1);
        if (auto b = hBorderWeak.get()) b.Background(restingBrush());
    });
    rowBtn.PointerCaptureLost([hBorderWeak, restingBrush, rowIndex](auto const&, auto const&) {
        int expected = rowIndex;
        g_miniSessionHoveredIndex.compare_exchange_strong(expected, -1);
        if (auto b = hBorderWeak.get()) b.Background(restingBrush());
    });

    return rowBtn;
}

static void FetchMiniSessionInfosAsync(HWND taskbarWnd) {
    SpawnTrackedWorker([taskbarWnd]() {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        std::vector<MiniSessionInfo> infos;
        GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
        {
            std::lock_guard<std::mutex> lk(g_sessionMtx);
            mgr = g_sessionMgr;
        }
        if (!mgr) { winrt::uninit_apartment(); return; }
        {
            try {
                auto sessions = mgr.GetSessions();
                for (auto const& s : sessions) {
                    try {
                        std::wstring sid = std::wstring(s.SourceAppUserModelId());
                        if (IsIgnoredMediaApp(sid)) continue;
                        MiniSessionInfo info;
                        info.id = sid;
                        try {
                            auto props = s.TryGetMediaPropertiesAsync().get();
                            if (props) {
                                info.title  = std::wstring(props.Title());
                                info.artist = std::wstring(props.Artist());
                                if (!g_settings.lyricsOnly) if (auto thumbRef = props.Thumbnail()) {
                                    try {
                                        auto stream = thumbRef.OpenReadAsync().get();
                                        if (stream) {
                                            UINT64 sz = stream.Size();
                                            if (sz > 0 && sz < 2 * 1024 * 1024) {
                                                DataReader reader(stream);
                                                reader.LoadAsync((UINT32)sz).get();
                                                info.thumbBytes.resize((size_t)sz);
                                                reader.ReadBytes(winrt::array_view<BYTE>(info.thumbBytes));
                                                reader.DetachStream();
                                            }
                                        }
                                    } catch (...) { info.thumbBytes.clear(); }
                                }
                            }
                        } catch (...) {}
                        if (info.title.empty() && info.artist.empty()) continue;
                        infos.push_back(std::move(info));
                    } catch (...) {}
                }
            } catch (...) {}
        }
        {
            std::lock_guard<std::mutex> lk(g_miniSessionMtx);
            g_miniSessionInfos = std::move(infos);
        }
        winrt::uninit_apartment();

        if (taskbarWnd) {
            RunFromWindowThread(taskbarWnd, [](void*) {
                if (!g_miniPlayerFlyoutOpen) return;
                RefreshMiniPlayerFlyoutUI();
            }, nullptr);
        }
    });
}

static Grid BuildMiniPlayerFlyoutContent() {
    Grid outer;
    outer.MinWidth(360);
    outer.MaxWidth(360);
    outer.MinHeight(400);

    bool isPlaying = false;
    bool hasMedia = false;
    bool canSkipPrevious = true, canSkipNext = true;
    bool canShuffle = true, canRepeat = true;
    std::wstring title, artist;
    std::vector<BYTE> thumbBytes;
    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        isPlaying       = g_media.isPlaying;
        hasMedia        = g_media.hasMedia;
        canSkipPrevious = g_media.canSkipPrevious;
        canSkipNext     = g_media.canSkipNext;
        canShuffle      = g_media.canShuffle;
        canRepeat       = g_media.canRepeat;
        title           = g_media.title;
        artist          = g_media.artist;
        thumbBytes      = g_media.thumbnailBytes;
    }

    Border bg;
    bg.CornerRadius({6, 6, 6, 6});
    bg.Padding({16, 16, 16, 0});
    bg.Margin({13, 13, 13, 13});

    bool isLight = IsSystemLightTheme();

    if (g_settings.showMiniPlayerBorder) {
        bg.BorderThickness({2, 2, 2, 2});
        bg.BorderBrush(MakeBrush(isLight
            ? winrt::Windows::UI::Color{0xFF, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF}));
    } else {
        bg.BorderThickness({1, 1, 1, 1});
        bg.BorderBrush(MakeBrush(isLight
            ? winrt::Windows::UI::Color{0x30, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{0x30, 0xFF, 0xFF, 0xFF}));
    }

    try {
        using namespace winrt::Windows::UI::Xaml::Media;
        ThemeShadow shadow;
        bg.Shadow(shadow);
        bg.Translation({0.f, 0.f, 32.f});
    } catch (...) {

    }

    Brush flyoutContentBackground{nullptr};
    try {
        winrt::Windows::UI::Xaml::Media::AcrylicBrush acrylic;
        acrylic.BackgroundSource(winrt::Windows::UI::Xaml::Media::AcrylicBackgroundSource::Backdrop);

        winrt::Windows::UI::Color tint = isLight
            ? winrt::Windows::UI::Color{0xff, 0xf2, 0xf2, 0xf2}
            : winrt::Windows::UI::Color{0xFF, 0x24, 0x24, 0x24};

        acrylic.TintColor(tint);
        acrylic.TintOpacity(isLight ? 0.0 : 0.5);
        acrylic.TintLuminosityOpacity(isLight ? 0.9 : 0.96);
        winrt::Windows::UI::Color fallback = isLight
            ? winrt::Windows::UI::Color{0xff, 0xf2, 0xf2, 0xf2}
            : winrt::Windows::UI::Color{0xFF, 0x24, 0x24, 0x24};
        acrylic.FallbackColor(fallback);

        flyoutContentBackground = acrylic;
    } catch (...) {
        flyoutContentBackground = MakeBrush(isLight
            ? winrt::Windows::UI::Color{0xf5, 0xf2, 0xf2, 0xf2}
            : winrt::Windows::UI::Color{0xF5, 0x24, 0x24, 0x24});
    }
    bg.Background(nullptr);

    StackPanel content;
    content.Orientation(Orientation::Vertical);
    content.HorizontalAlignment(HorizontalAlignment::Stretch);

    auto wrapDebugBorder = [](FrameworkElement const& elem, winrt::Windows::UI::Color color) -> FrameworkElement {
        if (!g_settings.showMiniPlayerBorder) return elem;
        Grid wrapper;
        wrapper.Children().Append(elem);
        Border dbg;
        dbg.BorderBrush(MakeBrush(color));
        dbg.BorderThickness({1, 1, 1, 1});
        dbg.IsHitTestVisible(false);
        wrapper.Children().Append(dbg);
        return wrapper;
    };

    Border artBorder;
    artBorder.MaxWidth(300);
    artBorder.MinWidth(170);
    artBorder.Height(169);
    artBorder.HorizontalAlignment(HorizontalAlignment::Center);
    artBorder.CornerRadius({6, 6, 6, 6});
    artBorder.Background(MakeBrush({0x40, 0x80, 0x80, 0x80}));
    artBorder.Margin({0, 0, 0, 10});
    {
        bool isLight = IsSystemLightTheme();
        artBorder.BorderThickness(g_settings.showMiniPlayerBorder ? Thickness{2, 2, 2, 2} : Thickness{1, 1, 1, 1});
        if (g_settings.showMiniPlayerBorder) {
            artBorder.BorderBrush(MakeBrush({0xFF, 0xFF, 0x00, 0x00}));
        } else {
            artBorder.BorderBrush(MakeBrush(isLight
                ? winrt::Windows::UI::Color{0x30, 0x00, 0x00, 0x00}
                : winrt::Windows::UI::Color{0x30, 0xFF, 0xFF, 0xFF}));
        }
    }

    Grid artInnerGrid;
    artInnerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
    artInnerGrid.VerticalAlignment(VerticalAlignment::Stretch);
    {
        auto clipGeo = winrt::Windows::UI::Xaml::Media::RectangleGeometry();
        artInnerGrid.SizeChanged([clipGeo](
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::UI::Xaml::SizeChangedEventArgs const&) mutable {
                try {
                    if (auto fe = sender.try_as<FrameworkElement>()) {
                        clipGeo.Rect({0.f, 0.f,
                            (float)fe.ActualWidth(),
                            (float)fe.ActualHeight()});
                    }
                } catch (...) {}
            });
        artInnerGrid.Clip(clipGeo);
    }

    Controls::Image artImage;
    artImage.Stretch(Stretch::Uniform );
    artImage.HorizontalAlignment(HorizontalAlignment::Center);
    artImage.VerticalAlignment(VerticalAlignment::Center);
    artImage.Visibility(Visibility::Collapsed);
    if (!thumbBytes.empty()) {
        LoadMiniPlayerArtBitmap(artImage, thumbBytes);
        g_miniPlayerCachedThumb = thumbBytes;
    }
    artInnerGrid.Children().Append(artImage);
    artBorder.Child(artInnerGrid);
    content.Children().Append(artBorder);

    TextBlock titleBlock;
    titleBlock.FontSize(16);
    titleBlock.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    titleBlock.TextTrimming(TextTrimming::CharacterEllipsis);
    titleBlock.TextWrapping(TextWrapping::NoWrap);
    titleBlock.HorizontalAlignment(HorizontalAlignment::Center);
    titleBlock.TextAlignment(TextAlignment::Center);
    titleBlock.Foreground(MakeBrush(TextColor()));
    titleBlock.Margin({0, 0, 0, 4});
    {
        std::wstring displayTitle;
        if (!hasMedia) {
            displayTitle = g_settings.noMediaTitleText;
        } else if (title.empty()) {
            displayTitle = g_settings.emptyTitleText;
        } else {
            displayTitle = title;
        }
        titleBlock.Text(winrt::hstring(displayTitle));
    }
    content.Children().Append(wrapDebugBorder(titleBlock, {0xFF, 0xFF, 0xFF, 0x00}));

    TextBlock artistBlock;
    artistBlock.FontSize(13);
    artistBlock.TextTrimming(TextTrimming::CharacterEllipsis);
    artistBlock.TextWrapping(TextWrapping::NoWrap);
    artistBlock.HorizontalAlignment(HorizontalAlignment::Center);
    artistBlock.TextAlignment(TextAlignment::Center);
    artistBlock.Opacity(0.65);
    artistBlock.Foreground(MakeBrush(ArtistColor()));
    artistBlock.Margin({0, 0, 0, 5});
    artistBlock.Text(artist.empty() ? winrt::hstring(L" ") : winrt::hstring(artist));
    content.Children().Append(wrapDebugBorder(artistBlock, {0xFF, 0x00, 0xFF, 0x00}));

    StackPanel controlsRow;
    controlsRow.Orientation(Orientation::Horizontal);
    controlsRow.HorizontalAlignment(HorizontalAlignment::Center);
    controlsRow.Spacing(4);
    controlsRow.Margin({0, 10, 0, 10});

    auto iconClr = ButtonColor();

    auto MakeBigBtn = [&](int cmd, bool playing, bool executeCommand = true) -> Button {
        Button btn;
        btn.Width(48);
        btn.Height(48);
        btn.Padding({1,1,1,1});
        btn.CornerRadius({4,4,4,4});
        btn.BorderThickness({0,0,0,0});
        btn.VerticalAlignment(VerticalAlignment::Center);
        btn.HorizontalAlignment(HorizontalAlignment::Center);

        const wchar_t* glyph = L"";
        if (cmd == 1) glyph = L"\uE622";
        else if (cmd == 2) glyph = playing ? L"\uE62E" : L"\uF5B0";
        else if (cmd == 3) glyph = L"\uE623";
        else if (cmd == 7) glyph = L"\uE8B1";
        else if (cmd == 8) {
            RepeatMode mode = g_repeatMode.load();
            glyph = mode == RepeatMode::One ? L"\uE8ED" :
                    mode == RepeatMode::All ? L"\uE8EE" : L"\uF5E7";
        }

        TextBlock iconText;
        iconText.Text(winrt::hstring(glyph));
        iconText.FontSize(16);
        iconText.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
        iconText.Foreground(MakeBrush(iconClr));
        iconText.HorizontalAlignment(HorizontalAlignment::Center);
        iconText.VerticalAlignment(VerticalAlignment::Center);
        if (cmd == 7) iconText.Opacity(g_shuffleEnabled.load() ? 1.0 : 0.40);
        if (cmd == 8) iconText.Opacity(g_repeatMode.load() != RepeatMode::Off ? 1.0 : 0.40);
        btn.Content(winrt::box_value(iconText));

        if (executeCommand) {
            btn.Click([cmd](auto const&, auto const&) {
                if (!g_unloading) {
                    try {
                        SendMediaCommandAsync(cmd);
                        DispatchMediaUpdate();
                    } catch (...) {}
                }
            });
        }

        ApplyFluentMediaButtonStyle(btn);

        auto isHovered = std::make_shared<bool>(false);
        auto pressLocked = std::make_shared<bool>(false);
        auto applyState = [weak = winrt::make_weak(btn), isHovered, pressLocked](bool pressed) {
            auto btn = weak.get();
            if (!btn) return;
            try {
                GoToCommonState(btn, IsHoverEffectEnabled(g_settings.mediaButtonsHoverEffectMode), pressed, *isHovered);
            } catch (...) {}
        };
        auto updateBtnVisualState = [applyState, pressLocked]() {
            if (*pressLocked) return;
            applyState(false);
        };

        RunWhenButtonReady(btn, [btn, applyState]() {
            try {
                SetupMediaButtonCommonStates(btn);
                applyState(false);
            } catch (...) {}
        });

        btn.PointerEntered([isHovered, updateBtnVisualState](auto const&, auto const&) {
            *isHovered = true;
            updateBtnVisualState();
        });
        btn.PointerExited([isHovered, updateBtnVisualState](auto const&, auto const&) {
            *isHovered = false;
            updateBtnVisualState();
        });
        btn.AddHandler(UIElement::PointerPressedEvent(), winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
            [applyState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            try {
                if (auto elem = sender.template try_as<UIElement>()) elem.CapturePointer(e.Pointer());
                applyState(true);
            } catch (...) {}
        })), true);
        btn.AddHandler(UIElement::PointerReleasedEvent(), winrt::box_value(
            winrt::Windows::UI::Xaml::Input::PointerEventHandler(
            [applyState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            try {
                if (auto elem = sender.template try_as<UIElement>()) elem.ReleasePointerCapture(e.Pointer());
                applyState(false);
            } catch (...) {}
        })), true);

        return btn;
    };

    Button shuffleBtn = MakeBigBtn(7, false);
    Button prevBtn    = MakeBigBtn(1, false);
    Button playBtn    = MakeBigBtn(2, isPlaying);
    Button nextBtn    = MakeBigBtn(3, false);
    Button repeatBtn  = MakeBigBtn(8, false, false);

    shuffleBtn.IsEnabled(hasMedia && canShuffle);
    shuffleBtn.Opacity(hasMedia && canShuffle ? 1.0 : 0.35);
    prevBtn.IsEnabled(hasMedia && canSkipPrevious);
    prevBtn.Opacity(hasMedia && canSkipPrevious ? 1.0 : 0.35);
    playBtn.IsEnabled(hasMedia);
    playBtn.Opacity(hasMedia ? 1.0 : 0.35);
    nextBtn.IsEnabled(hasMedia && canSkipNext);
    nextBtn.Opacity(hasMedia && canSkipNext ? 1.0 : 0.35);
    repeatBtn.IsEnabled(hasMedia && canRepeat);
    repeatBtn.Opacity(hasMedia && canRepeat ? 1.0 : 0.35);

    ToolTip shuffleToolTip;
    shuffleToolTip.Content(winrt::box_value(winrt::hstring(L"Toggle Shuffle")));
    ToolTipService::SetToolTip(shuffleBtn, shuffleToolTip);
    ToolTip repeatToolTip;
    repeatToolTip.Content(winrt::box_value(winrt::hstring(L"Toggle Repeat")));
    ToolTipService::SetToolTip(repeatBtn, repeatToolTip);

    repeatBtn.Click([](auto const&, auto const&) {
        if (g_unloading) return;
        try {
            RepeatMode cur = g_repeatMode.load();
            RepeatMode next;
            int cmd;
            if (cur == RepeatMode::Off) {
                next = RepeatMode::All; cmd = 11;
            } else if (cur == RepeatMode::All) {
                next = RepeatMode::One; cmd = 12;
            } else {
                next = RepeatMode::Off; cmd = 10;
            }
            g_repeatMode.store(next);
            if (g_miniPlayerRepeatBtnRef) {
                if (auto icon = g_miniPlayerRepeatBtnRef.Content().try_as<TextBlock>()) {
                    icon.Text(next == RepeatMode::One ? L"\uE8ED" :
                              next == RepeatMode::All ? L"\uE8EE" : L"\uF5E7");
                    icon.Opacity(next != RepeatMode::Off ? 1.0 : 0.40);
                }
            }
            SendMediaCommandAsync(cmd);
            DispatchMediaUpdate();
        } catch (...) {}
    });

    controlsRow.Children().Append(shuffleBtn);
    controlsRow.Children().Append(prevBtn);
    controlsRow.Children().Append(playBtn);
    controlsRow.Children().Append(nextBtn);
    controlsRow.Children().Append(repeatBtn);

    if (g_settings.showMiniPlayerBorder) {
        prevBtn.BorderBrush(MakeBrush({0xFF, 0xFF, 0x40, 0x40}));
        prevBtn.BorderThickness({1, 1, 1, 1});
        playBtn.BorderBrush(MakeBrush({0xFF, 0x40, 0xFF, 0x40}));
        playBtn.BorderThickness({1, 1, 1, 1});
        nextBtn.BorderBrush(MakeBrush({0xFF, 0x40, 0x40, 0xFF}));
        nextBtn.BorderThickness({1, 1, 1, 1});
    }

    content.Children().Append(wrapDebugBorder(controlsRow, {0xFF, 0x00, 0x00, 0xFF}));

    {
        Border sep;
        sep.Height(1);
        sep.Margin({-16, 0, -16, 0});
        bool isLight = IsSystemLightTheme();
        sep.Background(MakeBrush(isLight
            ? winrt::Windows::UI::Color{0x20, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{0x20, 0xFF, 0xFF, 0xFF}));
        sep.Visibility(Visibility::Collapsed);
        g_miniPlayerSessionListSepRef = sep;
        content.Children().Append(sep);
    }

    if (!g_settings.hideMediaSessionsList) {
        StackPanel sessionList;
        sessionList.Orientation(Orientation::Vertical);
        sessionList.Spacing(0);
        sessionList.Margin({-16, 3, -16, 3});
        sessionList.Visibility(Visibility::Collapsed);
        g_miniPlayerSessionListRef = sessionList;
        content.Children().Append(wrapDebugBorder(sessionList, {0xFF, 0xFF, 0x00, 0xFF}));
    } else {
        g_miniPlayerSessionListRef = nullptr;
    }

    Grid bottomBar;
    Border bottomSep;
    {
        bool isLight = IsSystemLightTheme();

        bottomSep.Height(1);
        bottomSep.Background(MakeBrush(isLight
            ? winrt::Windows::UI::Color{0x20, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{0x20, 0xFF, 0xFF, 0xFF}));

        bottomBar.Margin({-16, 0, -16, -16});
        bottomBar.Padding({12, 6, 12, 6});
        bottomBar.Height(48);
        bottomBar.VerticalAlignment(VerticalAlignment::Bottom);
        bottomBar.CornerRadius({0, 0, 6, 6});

        try {
            AcrylicBrush bottomAcrylic;
            bottomAcrylic.BackgroundSource(AcrylicBackgroundSource::Backdrop);
            bottomAcrylic.TintColor(isLight
                ? winrt::Windows::UI::Color{0xFF, 0xEE, 0xEE, 0xEE}
                : winrt::Windows::UI::Color{0xFF, 0x1C, 0x1C, 0x1C});
            bottomAcrylic.TintOpacity(isLight ? 0.00 : 0.50);
            bottomAcrylic.TintLuminosityOpacity(isLight ? 0.90 : 0.96);
            bottomAcrylic.FallbackColor(isLight
                ? winrt::Windows::UI::Color{0xFF, 0xEE, 0xEE, 0xEE}
                : winrt::Windows::UI::Color{0xFF, 0x1C, 0x1C, 0x1C});
            bottomBar.Background(bottomAcrylic);
        } catch (...) {
            bottomBar.Background(MakeBrush(isLight
                ? winrt::Windows::UI::Color{0xFF, 0xEE, 0xEE, 0xEE}
                : winrt::Windows::UI::Color{0xFF, 0x1C, 0x1C, 0x1C}));
        }

        StackPanel bottomRow;
        bottomRow.Orientation(Orientation::Horizontal);
        bottomRow.HorizontalAlignment(HorizontalAlignment::Right);
        bottomRow.VerticalAlignment(VerticalAlignment::Center);
        bottomRow.Spacing(2);

        auto MakeBottomBtn = [&](const wchar_t* glyph, std::wstring tooltip) -> Button {
            Button btn;
            btn.Width(35);
            btn.Height(35);
            btn.Padding({0,0,0,0});
            btn.CornerRadius({4,4,4,4});
            btn.BorderThickness({0,0,0,0});
            btn.VerticalAlignment(VerticalAlignment::Center);

            TextBlock icon;
            icon.Text(winrt::hstring(glyph));
            icon.FontSize(14);
            icon.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
            icon.Foreground(MakeBrush(isLight
                ? winrt::Windows::UI::Color{0xCC, 0x10, 0x10, 0x10}
                : winrt::Windows::UI::Color{0xCC, 0xFF, 0xFF, 0xFF}));
            icon.HorizontalAlignment(HorizontalAlignment::Center);
            icon.VerticalAlignment(VerticalAlignment::Center);
            btn.Content(winrt::box_value(icon));

            if (!tooltip.empty()) {
                ToolTip tt;
                tt.Content(winrt::box_value(winrt::hstring(tooltip)));
                ToolTipService::SetToolTip(btn, tt);
            }

            ApplyFluentMediaButtonStyle(btn);
            auto isHovered = std::make_shared<bool>(false);
            auto applyState = [weak = winrt::make_weak(btn), isHovered](bool pressed) {
                auto btn = weak.get();
                if (!btn) return;
                try {
                    GoToCommonState(btn,
                        IsHoverEffectEnabled(g_settings.mediaButtonsHoverEffectMode),
                        pressed, *isHovered);
                } catch (...) {}
            };
            RunWhenButtonReady(btn, [btn, applyState]() {
                try {
                    SetupMediaButtonCommonStates(btn);
                    applyState(false);
                } catch (...) {}
            });
            btn.PointerEntered([isHovered, applyState](auto const&, auto const&) {
                *isHovered = true; applyState(false);
            });
            btn.PointerExited([isHovered, applyState](auto const&, auto const&) {
                *isHovered = false; applyState(false);
            });
            btn.AddHandler(UIElement::PointerPressedEvent(), winrt::box_value(
                winrt::Windows::UI::Xaml::Input::PointerEventHandler(
                [applyState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
                try {
                    if (auto el = sender.template try_as<UIElement>())
                        el.CapturePointer(e.Pointer());
                    applyState(true);
                } catch (...) {}
            })), true);
            btn.AddHandler(UIElement::PointerReleasedEvent(), winrt::box_value(
                winrt::Windows::UI::Xaml::Input::PointerEventHandler(
                [applyState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
                try {
                    if (auto el = sender.template try_as<UIElement>())
                        el.ReleasePointerCapture(e.Pointer());
                    applyState(false);
                } catch (...) {}
            })), true);

            return btn;
        };

        Button settingsBtn = MakeBottomBtn(L"\uE713", L"Open Windhawk");
        settingsBtn.Click([](auto const&, auto const&) {
            OpenWindhawk();
        });

        bottomRow.Children().Append(settingsBtn);
        bottomBar.Children().Append(bottomRow);
    }

    Grid innerGrid;
    innerGrid.VerticalAlignment(VerticalAlignment::Stretch);
    RowDefinition rowContent;
    rowContent.Height(GridLengthHelper::FromValueAndType(1.0, GridUnitType::Star));
    RowDefinition rowBottom;
    rowBottom.Height(GridLengthHelper::FromValueAndType(48.0, GridUnitType::Pixel));
    innerGrid.RowDefinitions().Append(rowContent);
    innerGrid.RowDefinitions().Append(rowBottom);

    content.VerticalAlignment(VerticalAlignment::Top);
    Grid::SetRow(content, 0);

    Border contentBackground;
    contentBackground.Background(flyoutContentBackground);
    contentBackground.CornerRadius({6, 6, 0, 0});
    contentBackground.Margin({-16, -16, -16, 0});
    Grid::SetRow(contentBackground, 0);
    innerGrid.Children().Append(contentBackground);
    innerGrid.Children().Append(content);

    bottomBar.Margin({-16, 0, -16, 0});
    bottomBar.VerticalAlignment(VerticalAlignment::Stretch);
    Grid::SetRow(bottomBar, 1);
    innerGrid.Children().Append(bottomBar);

    bottomSep.Margin({-16, 0, -16, 0});
    bottomSep.VerticalAlignment(VerticalAlignment::Top);
    Grid::SetRow(bottomSep, 1);
    innerGrid.Children().Append(bottomSep);

    if (g_settings.showMiniPlayerBorder) {
        innerGrid.BorderBrush(MakeBrush({0xFF, 0x80, 0x80, 0xFF}));
        innerGrid.BorderThickness({1, 1, 1, 1});
        bottomBar.BorderBrush(MakeBrush({0xFF, 0xFF, 0x80, 0x00}));
        bottomBar.BorderThickness({1, 1, 1, 1});
    }

    bg.Child(innerGrid);
    outer.Children().Append(bg);

    outer.Opacity(0.0);
    try {
        using namespace winrt::Windows::UI::Xaml::Media;
        CompositeTransform transform;
        transform.TranslateY(0.0);
        outer.RenderTransform(transform);
    } catch (...) {}

    g_miniPlayerArtRef       = artImage;
    g_miniPlayerTitleRef     = titleBlock;
    g_miniPlayerArtistRef    = artistBlock;
    g_miniPlayerPlayBtnRef   = playBtn;
    g_miniPlayerPrevBtnRef   = prevBtn;
    g_miniPlayerNextBtnRef   = nextBtn;
    g_miniPlayerShuffleBtnRef = shuffleBtn;
    g_miniPlayerRepeatBtnRef  = repeatBtn;

    return outer;
}

struct MiniPlayerAnchor {
    winrt::Windows::Foundation::Point placementPoint{0.f, 0.f};
    Controls::Primitives::FlyoutPlacementMode placement =
        Controls::Primitives::FlyoutPlacementMode::Top;
    int    animAxis = 0;
    double animSign = 1.0;
};

static bool GetTaskbarMonitorWorkAreaInfo(FrameworkElement const& rootContent,
                                          RECT& outWorkAreaPx,
                                          POINT& outOriginPx,
                                          double& outScale) {
    if (!g_taskbarWnd || !rootContent) return false;

    HMONITOR mon = MonitorFromWindow(g_taskbarWnd, MONITOR_DEFAULTTONEAREST);
    if (!mon) return false;

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(mon, &mi)) return false;
    outWorkAreaPx = mi.rcWork;

    POINT originPx{0, 0};
    if (!ClientToScreen(g_taskbarWnd, &originPx)) return false;
    outOriginPx = originPx;

    double scale = 1.0;
    try {
        auto xamlRoot = rootContent.XamlRoot();
        if (xamlRoot) scale = xamlRoot.RasterizationScale();
    } catch (...) {}
    if (scale <= 0.0) scale = 1.0;
    outScale = scale;

    return true;
}

static bool ComputeScreenPlacementAnchor(FrameworkElement const& rootContent,
                                         MiniPlayerAnchor& outAnchor) {
    RECT workPx;
    POINT originPx;
    double scale;
    if (!GetTaskbarMonitorWorkAreaInfo(rootContent, workPx, originPx, scale)) {
        return false;
    }

    double left   = (workPx.left   - originPx.x) / scale;
    double top    = (workPx.top    - originPx.y) / scale;
    double right  = (workPx.right  - originPx.x) / scale;
    double bottom = (workPx.bottom - originPx.y) / scale;

    const std::wstring& hPlace = g_settings.miniPlayerHorizontalPlacement;
    const std::wstring& vPlace = g_settings.miniPlayerVerticalPlacement;
    int hDist = g_settings.miniPlayerHorizontalDistanceFromScreenEdge;
    int vDist = g_settings.miniPlayerVerticalDistanceFromScreenEdge;

    bool hLeft  = (hPlace == L"left");
    bool hRight = (hPlace == L"right");
    bool vTop   = (vPlace == L"top");
    bool vBottom = (vPlace == L"bottom");

    double anchorX;
    if (hLeft)       anchorX = left + hDist;
    else if (hRight) anchorX = right - hDist;
    else              anchorX = (left + right) / 2.0 + hDist;

    double anchorY;
    if (vTop)         anchorY = top + vDist;
    else if (vBottom) anchorY = bottom - vDist;
    else               anchorY = (top + bottom) / 2.0 + vDist;

    using FPM = Controls::Primitives::FlyoutPlacementMode;
    FPM placement = FPM::Top;
    int animAxis = 0;
    double animSign = 1.0;

    if (vBottom) {
        placement = hLeft ? FPM::TopEdgeAlignedLeft
                  : hRight ? FPM::TopEdgeAlignedRight
                  : FPM::Top;
        animAxis = 0;
        animSign = 1.0;
    } else if (vTop) {
        placement = hLeft ? FPM::BottomEdgeAlignedLeft
                  : hRight ? FPM::BottomEdgeAlignedRight
                  : FPM::Bottom;
        animAxis = 0;
        animSign = -1.0;
    } else {
        if (hLeft) {
            placement = FPM::Right;
            animAxis = 1;
            animSign = -1.0;
        } else if (hRight) {
            placement = FPM::Left;
            animAxis = 1;
            animSign = 1.0;
        } else {
            placement = FPM::Top;
            animAxis = 0;
            animSign = 1.0;
        }
    }

    if (g_settings.miniPlayerAnimation == L"top") {
        animAxis = 0; animSign = -1.0;
    } else if (g_settings.miniPlayerAnimation == L"bottom") {
        animAxis = 0; animSign = 1.0;
    } else if (g_settings.miniPlayerAnimation == L"left") {
        animAxis = 1; animSign = -1.0;
    } else if (g_settings.miniPlayerAnimation == L"right") {
        animAxis = 1; animSign = 1.0;
    }

    outAnchor.placementPoint = {(float)anchorX, (float)anchorY};
    outAnchor.placement = placement;
    outAnchor.animAxis = animAxis;
    outAnchor.animSign = animSign;
    return true;
}

static void ShowMiniPlayerFlyout(FrameworkElement const& target) {
    if (!target || g_unloading) return;
    try {

        if (g_miniPlayerFlyoutOpen && g_miniPlayerFlyout) {
            g_miniPlayerExplicitCloseRequested.store(true);
            g_miniPlayerFlyout.Hide();
            return;
        }

        Flyout flyout;
        Grid content = BuildMiniPlayerFlyoutContent();

        Grid clipHost;
        clipHost.HorizontalAlignment(HorizontalAlignment::Stretch);
        clipHost.VerticalAlignment(VerticalAlignment::Stretch);
        clipHost.Children().Append(content);
        flyout.Content(clipHost);

        Style flyoutStyle(winrt::xaml_typename<Controls::FlyoutPresenter>());
        flyoutStyle.Setters().Append(Setter(
            Controls::Control::BackgroundProperty(),
            winrt::box_value(winrt::Windows::UI::Xaml::Media::SolidColorBrush{
                winrt::Windows::UI::Colors::Transparent()})));
        flyoutStyle.Setters().Append(Setter(
            FrameworkElement::MarginProperty(),
            winrt::box_value(Thickness{0, 0, 0, 0})));
        flyoutStyle.Setters().Append(Setter(
            Controls::Control::PaddingProperty(),
            winrt::box_value(Thickness{0, 0, 0, 0})));
        flyoutStyle.Setters().Append(Setter(
            Controls::Control::BorderThicknessProperty(),
            winrt::box_value(Thickness{0, 0, 0, 0})));
        flyoutStyle.Setters().Append(Setter(
            Controls::Control::BorderBrushProperty(),
            winrt::box_value(winrt::Windows::UI::Xaml::Media::SolidColorBrush{
                winrt::Windows::UI::Colors::Transparent()})));
        flyoutStyle.Setters().Append(Setter(
            FrameworkElement::MaxWidthProperty(),
            winrt::box_value(10000.0)));
        flyoutStyle.Setters().Append(Setter(
            FrameworkElement::MaxHeightProperty(),
            winrt::box_value(10000.0)));
        flyout.FlyoutPresenterStyle(flyoutStyle);

        flyout.Opened([content](auto const&, auto const&) {
            g_miniPlayerFlyoutOpen = true;
            g_miniPlayerClosingAnimStarted.store(false);
            if (g_playerButtonStateUpdater) g_playerButtonStateUpdater();
            g_miniSessionHoveredIndex.store(-1);
            {
                std::lock_guard<std::mutex> lk(g_miniSessionMtx);
                g_miniSessionInfos.clear();
            }
            RefreshMiniPlayerFlyoutUI();
            if (!g_settings.hideMediaSessionsList) {
                FetchMiniSessionInfosAsync(g_taskbarWnd);
            }

            try {
                Controls::FlyoutPresenter presenter{nullptr};
                {
                    auto node = content.as<winrt::Windows::UI::Xaml::DependencyObject>();
                    for (int i = 0; i < 12 && node; ++i) {
                        if (auto p = node.try_as<Controls::FlyoutPresenter>()) {
                            presenter = p;
                            break;
                        }
                        node = VisualTreeHelper::GetParent(node);
                    }
                }
                if (presenter) {
                    int childCount = VisualTreeHelper::GetChildrenCount(presenter);
                    for (int i = 0; i < childCount; ++i) {
                        auto child = VisualTreeHelper::GetChild(presenter, i);
                        if (auto border = child.try_as<Controls::Border>()) {
                            border.Shadow(nullptr);
                            break;
                        }
                    }
                }
            } catch (...) {
                Wh_Log(L"Flyout.Opened: Exception removing Shadow");
            }

            content.Opacity(0.0);
            auto transform = content.RenderTransform()
                .try_as<winrt::Windows::UI::Xaml::Media::CompositeTransform>();
            if (!transform) {
                transform = winrt::Windows::UI::Xaml::Media::CompositeTransform();
                content.RenderTransform(transform);
            }
            transform.TranslateX(0.0);
            transform.TranslateY(0.0);

            auto fired = std::make_shared<bool>(false);
            auto doReveal = [content, transform, fired]() mutable {
                if (*fired) return;
                *fired = true;
                try {
                    if (!transform) return;
                    bool horizontal = (g_miniPlayerAnimAxis == 1);
                    double size = horizontal ? content.ActualWidth() : content.ActualHeight();
                    if (size <= 0) size = 450.0;
                    double startOffset = size * g_miniPlayerAnimSign;

                    PCWSTR propertyPath = horizontal
                        ? L"(UIElement.RenderTransform).(CompositeTransform.TranslateX)"
                        : L"(UIElement.RenderTransform).(CompositeTransform.TranslateY)";

                    if (horizontal) transform.TranslateX(startOffset);
                    else            transform.TranslateY(startOffset);
                    content.Opacity(1.0);

                    using namespace winrt::Windows::UI::Xaml::Media::Animation;
                    Storyboard sb;
                    DoubleAnimation anim;
                    anim.From(startOffset);
                    anim.To(0.0);
                    anim.Duration(winrt::Windows::UI::Xaml::DurationHelper::FromTimeSpan(
                        std::chrono::milliseconds(250)));
                    auto ease = CircleEase();
                    ease.EasingMode(EasingMode::EaseOut);
                    anim.EasingFunction(ease);
                    Storyboard::SetTarget(anim, content);
                    Storyboard::SetTargetProperty(anim, propertyPath);
                    sb.Children().Append(anim);
                    sb.Begin();
                } catch (...) {}
            };

            auto token = std::make_shared<winrt::event_token>();
            *token = content.SizeChanged([content, token, doReveal](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::UI::Xaml::SizeChangedEventArgs const&) mutable
            {
                try { content.SizeChanged(*token); } catch (...) {}
                doReveal();
            });

            auto fallbackTimer = DispatcherTimer();
            fallbackTimer.Interval(
                winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(80)});
            auto fallbackToken = std::make_shared<winrt::event_token>();
            *fallbackToken = fallbackTimer.Tick([fallbackTimer, fallbackToken, doReveal](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) mutable
            {
                try { fallbackTimer.Stop(); } catch (...) {}
                doReveal();
            });
            fallbackTimer.Start();
        });

        flyout.Closing([content](
            winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase const& sender,
            winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBaseClosingEventArgs const& e)
        {
            if (g_unloading || g_applyingSettings) return;

            if (g_miniPlayerClosingAnimInProgress.exchange(false)) return;

            bool explicitClose = g_miniPlayerExplicitCloseRequested.exchange(false);

            if (g_settings.keepMiniPlayerOpen && !explicitClose) {
                e.Cancel(true);
                return;
            }

            if (g_miniPlayerClosingAnimStarted.load()) {
                e.Cancel(true);
                return;
            }

            try {
                auto transform = content.RenderTransform()
                    .try_as<winrt::Windows::UI::Xaml::Media::CompositeTransform>();
                if (!transform) return;
                e.Cancel(true);

                bool horizontal = (g_miniPlayerAnimAxis == 1);
                double size = horizontal ? content.ActualWidth() : content.ActualHeight();
                if (size <= 0) size = 450.0;
                double endOffset = (size + 16.0) * g_miniPlayerAnimSign;

                PCWSTR propertyPath = horizontal
                    ? L"(UIElement.RenderTransform).(CompositeTransform.TranslateX)"
                    : L"(UIElement.RenderTransform).(CompositeTransform.TranslateY)";

                using namespace winrt::Windows::UI::Xaml::Media::Animation;
                Storyboard sb;
                DoubleAnimation anim;
                anim.To(endOffset);
                anim.Duration(winrt::Windows::UI::Xaml::DurationHelper::FromTimeSpan(
                    std::chrono::milliseconds(250)));
                auto ease = CircleEase();
                ease.EasingMode(EasingMode::EaseIn);
                anim.EasingFunction(ease);
                Storyboard::SetTarget(anim, content);
                Storyboard::SetTargetProperty(anim, propertyPath);
                sb.Children().Append(anim);
                g_miniPlayerClosingAnimStarted.store(true);
                auto doHide = [sender]() {
                    g_miniPlayerClosingAnimStarted.store(false);
                    try {
                        g_miniPlayerClosingAnimInProgress.store(true);
                        sender.Hide();
                    } catch (...) {
                        g_miniPlayerClosingAnimInProgress.store(false);
                    }
                };
                sb.Completed([doHide](auto const&, auto const&) { doHide(); });
                auto closingFallback = DispatcherTimer();
                closingFallback.Interval(winrt::Windows::Foundation::TimeSpan{
                    std::chrono::milliseconds(350)});
                auto closingFallbackToken = std::make_shared<winrt::event_token>();
                *closingFallbackToken = closingFallback.Tick(
                    [closingFallback, closingFallbackToken, doHide](
                        winrt::Windows::Foundation::IInspectable const&,
                        winrt::Windows::Foundation::IInspectable const&) mutable {
                        try { closingFallback.Stop(); } catch (...) {}
                        closingFallback.Tick(*closingFallbackToken);
                        if (g_miniPlayerClosingAnimStarted.load())
                            doHide();
                    });
                closingFallback.Start();
                sb.Begin();
            } catch (...) {
                g_miniPlayerClosingAnimStarted.store(false);
            }
        });

        flyout.Closed([](auto const&, auto const&) {
            g_miniPlayerFlyoutOpen = false;
            g_miniPlayerClosingAnimStarted.store(false);
            g_miniSessionHoveredIndex.store(-1);
            g_miniPlayerArtRef = nullptr;
            g_miniPlayerTitleRef = nullptr;
            g_miniPlayerArtistRef = nullptr;
            g_miniPlayerPlayBtnRef = nullptr;
            g_miniPlayerPrevBtnRef = nullptr;
            g_miniPlayerNextBtnRef = nullptr;
            g_miniPlayerShuffleBtnRef = nullptr;
            g_miniPlayerRepeatBtnRef = nullptr;
            g_miniPlayerSessionListRef = nullptr;
            g_miniPlayerSessionListSepRef = nullptr;
            g_miniSessionCachedThumbSizes.clear();
            if (g_playerButtonStateUpdater) g_playerButtonStateUpdater();
        });


        bool useScreenPlacement = (g_settings.miniPlayerPlacementMode == L"screen");
        Controls::Primitives::FlyoutPlacementMode placementMode =
            Controls::Primitives::FlyoutPlacementMode::Top;
        winrt::Windows::Foundation::Point anchorPoint{0.f, 0.f};
        FrameworkElement showAtElem = target;
        bool anchorComputed = false;

        try {
            auto xamlRoot = target.XamlRoot();
            if (xamlRoot) {
                auto rootContent = xamlRoot.Content().try_as<FrameworkElement>();
                if (rootContent) {
                    showAtElem = rootContent;

                    try {
                        flyout.OverlayInputPassThroughElement(rootContent);
                    } catch (...) {}

                    if (useScreenPlacement) {
                        MiniPlayerAnchor screenAnchor;
                        if (ComputeScreenPlacementAnchor(rootContent, screenAnchor)) {
                            anchorPoint = screenAnchor.placementPoint;
                            placementMode = screenAnchor.placement;
                            g_miniPlayerAnimAxis = screenAnchor.animAxis;
                            g_miniPlayerAnimSign = screenAnchor.animSign;
                            anchorComputed = true;
                        } else {
                            Wh_Log(L"ShowMiniPlayerFlyout: Failed to compute screen anchor, "
                                   L"falling back to 'over the player' placement");
                        }
                    }

                    if (!anchorComputed) {
                        auto xform = target.TransformToVisual(rootContent);
                        auto pt = xform.TransformPoint({0.f, 0.f});
                        float cx = pt.X + (float)target.ActualWidth() * 0.5f +
                                   (float)g_settings.miniPlayerHorizontalOffsetAbove;

                        bool placeBelow = (g_settings.miniPlayerVerticalPlacementNear == L"bottom");
                        if (placeBelow) {
                            float ty = pt.Y + (float)target.ActualHeight();
                            anchorPoint = {cx, ty};
                            placementMode = Controls::Primitives::FlyoutPlacementMode::Bottom;
                            g_miniPlayerAnimAxis = 0;
                            g_miniPlayerAnimSign = -1.0;
                        } else {
                            float ty = pt.Y;
                            anchorPoint = {cx, ty};
                            placementMode = Controls::Primitives::FlyoutPlacementMode::Top;
                            g_miniPlayerAnimAxis = 0;
                            g_miniPlayerAnimSign = 1.0;
                        }
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"ShowMiniPlayerFlyout: Exception setting position");
        }

        try {
            flyout.ShouldConstrainToRootBounds(false);
            flyout.Placement(placementMode);
        } catch (...) {}

        g_miniPlayerFlyout = flyout;


        Controls::Primitives::FlyoutShowOptions opts;
        opts.Placement(placementMode);
        opts.Position(anchorPoint);

        flyout.ShowAt(showAtElem, opts);

    } catch (...) {
        Wh_Log(L"ShowMiniPlayerFlyout: Exception in main try block");
    }
}


static Grid BuildLyricsOnlyPlayerGrid() {
    ResetLyricsVisuals();
    const double width = (double)g_settings.lyricsWidth;
    const double height = (double)std::max(24, g_settings.playerMaxHeight > 0
                                               ? g_settings.playerMaxHeight : g_settings.playerMinHeight);
    Grid wrapper;
    wrapper.Name(kGridName);
    wrapper.Tag(winrt::box_value(winrt::hstring(L"FluentMediaBarWrapper")));
    wrapper.Width(width);
    wrapper.MinWidth(0);
    wrapper.MaxWidth(width);
    wrapper.Height(height);
    wrapper.MinHeight(height);
    wrapper.MaxHeight(height);
    wrapper.VerticalAlignment(VerticalAlignment::Center);
    wrapper.HorizontalAlignment(HorizontalAlignment::Left);
    // RenderTransform leaves the layout slot unchanged; accurate even in tray columns.
    TranslateTransform positionOffset;
    positionOffset.X((double)(g_settings.lyricsHorizontalOffset + g_settings.horizontalOffset));
    positionOffset.Y((double)(g_settings.lyricsVerticalOffset + g_settings.verticalOffset));
    wrapper.RenderTransform(positionOffset);
    wrapper.IsHitTestVisible(LyricsInteractive());
    if (LyricsInteractive()) {
        wrapper.Background(MakeBrush({0, 0, 0, 0}));
        BindDoubleTap(wrapper);
        // Keep a lyric click from triggering the outer player's configured actions.
        wrapper.PointerPressed([](auto const&, PointerRoutedEventArgs const& args) { args.Handled(true); });
        wrapper.PointerReleased([](auto const&, PointerRoutedEventArgs const& args) { args.Handled(true); });
    }
    Border background;
    background.Name(L"FluentMedia_Background");
    background.CornerRadius({g_settings.cornerRadiusTL, g_settings.cornerRadiusTR,
                             g_settings.cornerRadiusBR, g_settings.cornerRadiusBL});
    if (g_settings.backgroundType != L"none") background.Background(MakeBackgroundBrush());
    background.IsHitTestVisible(false);
    wrapper.Children().Append(background);
    Grid viewport;
    viewport.Name(L"FluentLyrics_Viewport");
    viewport.Width(width);
    viewport.Height(height);
    viewport.HorizontalAlignment(HorizontalAlignment::Center);
    viewport.VerticalAlignment(VerticalAlignment::Center);
    viewport.IsHitTestVisible(false);
    auto clip = winrt::Windows::UI::Xaml::Media::RectangleGeometry();
    clip.Rect({0, 0, (float)width, (float)height});
    g_lyricsGuardClip = clip; // Keep clip synchronized during animated width changes.
    viewport.Clip(clip);
    viewport.SizeChanged([clip](winrt::Windows::Foundation::IInspectable const&,
                                SizeChangedEventArgs const& args) {
        try {
            const double w = std::max(0.0, static_cast<double>(args.NewSize().Width));
            clip.Rect({0, 0, (float)w, (float)args.NewSize().Height});
            if (g_lyricsLineA) g_lyricsLineA.MaxWidth(std::max(1.0, w - 12));
            if (g_lyricsLineB) g_lyricsLineB.MaxWidth(std::max(1.0, w - 12));
        } catch (...) {}
    });
    g_lyricsViewport = viewport;
    auto createLyricBlock = [&](const wchar_t* name) {
        TextBlock block;
        block.Name(name);
        block.FontSize((double)g_settings.lyricsFontSize);
        // Fixed, readable baseline spacing for the optional two-line lyric.
        block.LineHeight((double)g_settings.lyricsFontSize + 3.0);
        block.LineStackingStrategy(LineStackingStrategy::BlockLineHeight);
        std::wstring font = g_settings.titleFont.empty() ? g_settings.titleFontFamily : g_settings.titleFont;
        if (!font.empty()) block.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(winrt::hstring(font)));
        block.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        block.Foreground(MakeBrush(TextColor()));
        block.TextAlignment(TextAlignment::Center);
        block.HorizontalAlignment(HorizontalAlignment::Center);
        block.VerticalAlignment(VerticalAlignment::Center);
        block.MaxWidth(std::max(1.0, width - 12));
        block.MaxHeight(std::max(18.0, height - 4.0));
        block.TextTrimming(TextTrimming::CharacterEllipsis);
        block.TextWrapping(g_settings.lyricsTwoLineWrap ? TextWrapping::WrapWholeWords : TextWrapping::NoWrap);
        block.MaxLines(g_settings.lyricsTwoLineWrap ? 2 : 1);
        block.IsHitTestVisible(false);
        block.Opacity(0.0);
        block.RenderTransform(TranslateTransform());
        return block;
    };
    g_lyricsLineA = createLyricBlock(L"FluentLyrics_LineA");
    g_lyricsLineB = createLyricBlock(L"FluentLyrics_LineB");
    viewport.Children().Append(g_lyricsLineA);
    viewport.Children().Append(g_lyricsLineB);
    wrapper.Children().Append(viewport);
    return wrapper;
}
static Grid BuildPlayerGrid() {
    if (g_settings.lyricsOnly) {
        try { return BuildLyricsOnlyPlayerGrid(); }
        catch (...) { Wh_Log(L"BuildLyricsOnlyPlayerGrid: construction failed"); return nullptr; }
    }
    ResetLyricsVisuals();
    g_vizNormalColumn = -1;
    g_compactNoLyricsActive = false;
    try { if (g_vizDockStoryboard) g_vizDockStoryboard.Stop(); } catch (...) {}
    g_vizDockStoryboard = nullptr;
    try {
        g_vizBars->clear();
        g_vizBrushes->clear();
        auto textClr = TextColor();
        auto artistClr = ArtistColor();
        auto buttonClr = ButtonColor();
        auto bgBrush = MakeBackgroundBrush();
        double phMin = (double)g_settings.playerMinHeight;
        double phMax = (double)g_settings.playerMaxHeight;
        bool hasTextOrButtons = g_settings.showTrackTitle || g_settings.showTrackArtist || (g_settings.showMediaButtons && !g_mediaButtons.empty());
        Border backgroundBorder;
        backgroundBorder.Name(L"FluentMedia_Background");
        backgroundBorder.CornerRadius({
            g_settings.cornerRadiusTL,
            g_settings.cornerRadiusTR,
            g_settings.cornerRadiusBR,
            g_settings.cornerRadiusBL
        });
        backgroundBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
        backgroundBorder.VerticalAlignment(VerticalAlignment::Stretch);
        backgroundBorder.IsHitTestVisible(false);
        backgroundBorder.Visibility(Visibility::Collapsed);
        if (phMin > 0) {
            backgroundBorder.MinHeight(phMin);
        }
        if (phMax > 0) {
            backgroundBorder.MaxHeight(phMax);
        }
        Button playerButton;
        playerButton.Name(L"FluentMedia_OuterBorder");
        playerButton.CornerRadius({
            g_settings.cornerRadiusTL,
            g_settings.cornerRadiusTR,
            g_settings.cornerRadiusBR,
            g_settings.cornerRadiusBL
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
        if (g_settings.showDebugBorders) {
            playerButton.BorderBrush(MakeBrush({0xFF, 0xFF, 0x00, 0x00}));
            playerButton.BorderThickness({2, 2, 2, 2});
        }
        Grid panel;
        panel.Name(kPanelGridName);
        panel.VerticalAlignment(VerticalAlignment::Center);
        panel.HorizontalAlignment(HorizontalAlignment::Stretch);
        if (hasTextOrButtons) {
            const double inset = g_settings.compactLayout ? 2.0 : 4.0;
            panel.Margin({inset, 2, inset, 2});
        }
        AddLayoutAnchorOverlay(panel, L"FluentMedia_DebugPanelAnchors", {0xD0, 0x00, 0xFF, 0x00});
        if (g_settings.showDebugBorders) {
            Border panelDebugBorder;
            panelDebugBorder.BorderBrush(MakeBrush({0xFF, 0x00, 0xFF, 0x00}));
            panelDebugBorder.BorderThickness({1,1,1,1});
            panel.Children().Append(panelDebugBorder);
        }
        bool buttonsLeft = g_settings.controlsSide == L"left" ||
            (g_settings.controlsSide == L"auto" && g_settings.mirrorLayout);
        bool albumArtLeft = g_settings.albumArtSide == L"left" ||
            (g_settings.albumArtSide == L"auto" && !g_settings.mirrorLayout);
        bool hasText = g_settings.showTrackTitle || g_settings.showTrackArtist;
        for (int i = 0; i < 5; ++i) {
            ColumnDefinition col;
            col.Width({1.0, i == 2 && hasText ? GridUnitType::Star : GridUnitType::Auto});
            panel.ColumnDefinitions().Append(col);
        }
        Grid artContainer{nullptr};
        StackPanel artVizDock{nullptr};
        if (g_settings.showAlbumArt) {
            int iconSz = g_settings.appIconSize;
            artContainer = Grid();
            artContainer.VerticalAlignment(VerticalAlignment::Center);
            artContainer.HorizontalAlignment(HorizontalAlignment::Center);
            if (g_settings.albumArtMinWidth > 0) {
                artContainer.MinWidth((double)g_settings.albumArtMinWidth);
            }
            if (g_settings.albumArtMaxWidth > 0) {
                artContainer.MaxWidth((double)g_settings.albumArtMaxWidth);
            }
            if (g_settings.albumArtMinHeight > 0) {
                artContainer.MinHeight((double)g_settings.albumArtMinHeight);
            }
            if (g_settings.albumArtMaxHeight > 0) {
                artContainer.MaxHeight((double)g_settings.albumArtMaxHeight);
            }
            double artLeftMargin = (double)g_settings.albumArtLeftMargin;
            double artRightMargin = (double)g_settings.albumArtRightMargin;
            artContainer.Margin({artLeftMargin, 0, artRightMargin, 0});
            artContainer.Opacity(g_settings.albumArtOpacity / 100.0);
            artContainer.Background(MakeBrush({0x00,0x00,0x00,0x00}));
            AddLayoutAnchorOverlay(artContainer, L"FluentMedia_DebugArtAnchors", {0xD0, 0xFF, 0xFF, 0x00});
            if (g_settings.showDebugBorders) {
                Border artDebugBorder;
                artDebugBorder.BorderBrush(MakeBrush({0xFF, 0xFF, 0xFF, 0x00}));
                artDebugBorder.BorderThickness({2,2,2,2});
                artContainer.Children().Append(artDebugBorder);
            }
            winrt::Windows::UI::Xaml::Shapes::Rectangle placeholder;
            placeholder.Fill(MakeBrush({0x40,0x80,0x80,0x80}));
            double maxRadius = std::max({g_settings.albumArtCornerRadiusTL, g_settings.albumArtCornerRadiusTR,
                                        g_settings.albumArtCornerRadiusBR, g_settings.albumArtCornerRadiusBL});
            placeholder.RadiusX(maxRadius);
            placeholder.RadiusY(maxRadius);
            placeholder.HorizontalAlignment(HorizontalAlignment::Stretch);
            placeholder.VerticalAlignment(VerticalAlignment::Stretch);
            artContainer.Children().Append(placeholder);
            Border artBorder;
            artBorder.CornerRadius({
                g_settings.albumArtCornerRadiusTL,
                g_settings.albumArtCornerRadiusTR,
                g_settings.albumArtCornerRadiusBR,
                g_settings.albumArtCornerRadiusBL
            });
            artBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
            artBorder.VerticalAlignment(VerticalAlignment::Stretch);
            Controls::Image artImage;
            artImage.Name(kArtImageName);
            bool isSquare = (g_settings.albumArtMinWidth == g_settings.albumArtMinHeight) &&
                        (g_settings.albumArtMaxWidth == g_settings.albumArtMaxHeight);
            artImage.Stretch(isSquare ? Stretch::Uniform : Stretch::Uniform);
            artImage.HorizontalAlignment(HorizontalAlignment::Center);
            artImage.VerticalAlignment(VerticalAlignment::Center);
            Grid artInnerGrid;
            artInnerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
            artInnerGrid.VerticalAlignment(VerticalAlignment::Stretch);
            artInnerGrid.Children().Append(artImage);
            {
                auto clipGeo = winrt::Windows::UI::Xaml::Media::RectangleGeometry();
                artInnerGrid.Clip(clipGeo);
                artInnerGrid.SizeChanged([clipGeo](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    winrt::Windows::UI::Xaml::SizeChangedEventArgs const&) mutable {
                        try {
                            if (auto fe = sender.try_as<FrameworkElement>()) {
                                clipGeo.Rect({0, 0,
                                    (float)fe.ActualWidth(),
                                    (float)fe.ActualHeight()});
                            }
                        } catch (...) {}
                    });
            }
            artBorder.Child(artInnerGrid);
            artContainer.Children().Append(artBorder);
            Border artRing;
            artRing.CornerRadius({
                g_settings.albumArtCornerRadiusTL,
                g_settings.albumArtCornerRadiusTR,
                g_settings.albumArtCornerRadiusBR,
                g_settings.albumArtCornerRadiusBL
            });
            artRing.BorderThickness({1,1,1,1});
            artRing.BorderBrush(MakeBrush({0x25,0x80,0x80,0x80}));
            artContainer.Children().Append(artRing);
            if (g_settings.showAppIcon) {
                Grid iconOverlay;
                iconOverlay.VerticalAlignment(VerticalAlignment::Stretch);
                iconOverlay.HorizontalAlignment(HorizontalAlignment::Stretch);
                Controls::Image appIconImage;
                appIconImage.Name(kAppIconImageName);
                appIconImage.Width(iconSz);
                appIconImage.Height(iconSz);
                appIconImage.Stretch(Stretch::UniformToFill);
                appIconImage.Visibility(Visibility::Collapsed);
                const auto& corner = g_settings.appIconCorner;
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
            if (g_settings.albumArtPlaybackButton) {
                // A native Button provides mouse, touch and keyboard activation.
                // Overlay the cover rather than allocating another horizontal slot.
                Button playback;
                playback.Name(L"FluentMedia_ArtPlaybackButton");
                playback.MinWidth(0);
                playback.MinHeight(0);
                playback.Padding({0, 0, 0, 0});
                playback.BorderThickness({0, 0, 0, 0});
                playback.CornerRadius(artBorder.CornerRadius());
                playback.HorizontalAlignment(HorizontalAlignment::Stretch);
                playback.VerticalAlignment(VerticalAlignment::Stretch);
                playback.HorizontalContentAlignment(HorizontalAlignment::Center);
                playback.VerticalContentAlignment(VerticalAlignment::Center);
                playback.Background(MakeBrush({(BYTE)((g_settings.pauseOverlayOpacity * 255) / 100), 0, 0, 0}));
                const double iconSize = std::min((double)g_settings.pauseOverlayIconSize,
                    g_settings.albumArtMaxHeight > 0 ? std::max(8.0, (double)g_settings.albumArtMaxHeight - 4.0)
                        : (double)g_settings.pauseOverlayIconSize);
                playback.Content(MakeIconText(GetGlyph(2, false), iconSize, {0xFF, 0xFF, 0xFF, 0xFF}));
                playback.IsEnabled(false); // Enabled by the current media session's capabilities.
                // Keep the cover visible while idle. A fully transparent Button stays
                // hit-testable so playback continues to work on click/touch.
                playback.Opacity(0.0);
                Automation::AutomationProperties::SetName(playback, L"Play");
                ToolTipService::SetToolTip(playback, winrt::box_value(L"Play"));
                BindArtworkPlayback(playback);
                Canvas::SetZIndex(playback, 20);
                artContainer.Children().Append(playback);
            }
            if (!g_settings.albumArtPlaybackButton && g_settings.showPauseOverlay) {
                Border pauseBorder;
                pauseBorder.Name(L"PauseIconOverlay");
                pauseBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
                pauseBorder.VerticalAlignment(VerticalAlignment::Stretch);
                BYTE opacity = (BYTE)((g_settings.pauseOverlayOpacity * 255) / 100);
                pauseBorder.Background(MakeBrush({opacity, 0x00, 0x00, 0x00}));
                pauseBorder.Visibility(Visibility::Collapsed);
                Canvas::SetZIndex(pauseBorder, 8);
                TextBlock pauseIcon;
                pauseIcon.Text(L"");
                pauseIcon.FontFamily(Media::FontFamily(L"Segoe MDL2 Assets"));
                pauseIcon.FontSize((double)g_settings.pauseOverlayIconSize);
                pauseIcon.Foreground(MakeBrush({0xFF, 0xFF, 0xFF, 0xFF}));
                pauseIcon.HorizontalAlignment(HorizontalAlignment::Center);
                pauseIcon.VerticalAlignment(VerticalAlignment::Center);
                pauseBorder.Child(pauseIcon);
                artInnerGrid.Children().Append(pauseBorder);
            }
            // Artwork overlays are mouse-hover-only; this also applies to the
            // legacy pause indicator when the cover playback button is disabled.
            g_albumArtworkHovered = false;
            artContainer.PointerEntered([weakArt = winrt::make_weak(artContainer)](auto const&, auto const&) {
                g_albumArtworkHovered = true;
                if (auto art = weakArt.get()) {
                    if (auto playback = FindChildByName(art, L"FluentMedia_ArtPlaybackButton"))
                        playback.Opacity(1.0);
                    if (auto paused = FindChildByName(art, L"PauseIconOverlay"))
                        paused.Visibility(!g_albumArtworkPlaying ? Visibility::Visible : Visibility::Collapsed);
                }
            });
            artContainer.PointerExited([weakArt = winrt::make_weak(artContainer)](auto const&, auto const&) {
                g_albumArtworkHovered = false;
                if (auto art = weakArt.get()) {
                    if (auto playback = FindChildByName(art, L"FluentMedia_ArtPlaybackButton"))
                        playback.Opacity(0.0);
                    if (auto paused = FindChildByName(art, L"PauseIconOverlay"))
                        paused.Visibility(Visibility::Collapsed);
                }
            });
            if (DoubleClickEnabled(L"album_art") && !g_settings.albumArtPlaybackButton) BindDoubleTap(artContainer);
            if (g_settings.disableAlbumArtClick && !g_settings.albumArtPlaybackButton && !DoubleClickEnabled(L"album_art")) {
                artContainer.IsHitTestVisible(false);
            } else {
                artContainer.PointerPressed([](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
                    if (g_settings.albumArtPlaybackButton || DoubleClickEnabled(L"album_art")) { e.Handled(true); return; }
                    if (g_settings.disableAlbumArtClick) return;
                    if (auto elem = sender.template try_as<UIElement>()) {
                        elem.CapturePointer(e.Pointer());
                    }
                    e.Handled(true);
                });
                auto artDblClickLastTime = std::make_shared<ULONGLONG>(0);
                auto artDblClickLastKind = std::make_shared<winrt::Windows::UI::Input::PointerUpdateKind>(
                    winrt::Windows::UI::Input::PointerUpdateKind::Other);
                artContainer.PointerReleased([artDblClickLastTime, artDblClickLastKind](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
                    if (g_settings.albumArtPlaybackButton || DoubleClickEnabled(L"album_art")) { e.Handled(true); return; }
                    if (g_settings.disableAlbumArtClick) return;
                    bool actuallyHovered = false;
                    if (auto elem = sender.template try_as<UIElement>()) {
                        elem.ReleasePointerCapture(e.Pointer());
                        try {
                            auto pointerPoint = e.GetCurrentPoint(elem);
                            auto bounds = elem.RenderSize();
                            auto pos = pointerPoint.Position();
                            actuallyHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                        } catch (...) { actuallyHovered = false; }
                    }
                    if (g_unloading) return;
                    if (actuallyHovered) {
                        auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
                        auto fe = sender.template try_as<FrameworkElement>();
                        ULONGLONG now = GetTickCount64();
                        UINT dblClickMs = GetDoubleClickTime();
                        bool isDouble = (kind == *artDblClickLastKind) &&
                                        (now - *artDblClickLastTime) <= dblClickMs;
                        *artDblClickLastTime = isDouble ? 0 : now;
                        *artDblClickLastKind = kind;
                        using Kind = winrt::Windows::UI::Input::PointerUpdateKind;
                        if (kind == Kind::LeftButtonReleased) {
                            if (isDouble) ExecuteMediaAction(g_settings.albumArtLeftDoubleClick, fe);
                            else          ExecuteMediaAction(g_settings.albumArtLeftClick, fe);
                        } else if (kind == Kind::RightButtonReleased) {
                            if (isDouble) ExecuteMediaAction(g_settings.albumArtRightDoubleClick, fe);
                            else          ExecuteMediaAction(g_settings.albumArtRightClick, fe);
                        } else if (kind == Kind::MiddleButtonReleased) {
                            if (isDouble) ExecuteMediaAction(g_settings.albumArtMiddleDoubleClick, fe);
                            else          ExecuteMediaAction(g_settings.albumArtMiddleClick, fe);
                        }
                    }
                    e.Handled(true);
                });
                artContainer.PointerWheelChanged([](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
                    if (g_unloading) return;
                    auto action = g_settings.albumArtWheelAction;
                    if (action == L"none") return;
                    auto props = e.GetCurrentPoint(nullptr).Properties();
                    int delta = props.MouseWheelDelta();
                    if (action == L"switch_tracks") {
                        if (delta > 0) SendMediaCommandAsync(1);
                        else if (delta < 0) SendMediaCommandAsync(3);
                        DispatchMediaUpdate();
                    } else if (action == L"switch_tracks_inverted") {
                        if (delta > 0) SendMediaCommandAsync(3);
                        else if (delta < 0) SendMediaCommandAsync(1);
                        DispatchMediaUpdate();
                    } else if (action == L"switch_sessions") {
                        if (delta != 0) SwitchMediaSession();
                    } else if (action == L"system_sound") {
                        SpawnTrackedWorker([delta]() { ChangeSystemVolume(delta > 0); });
                    } else if (action == L"app_sound") {
                        SpawnTrackedWorker([delta]() {
                            winrt::init_apartment(winrt::apartment_type::multi_threaded);
                            std::wstring aumid;
                            {
                                GlobalSystemMediaTransportControlsSession sessionCopy{nullptr};
                                {
                                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                                    sessionCopy = g_currentSession;
                                }
                                if (sessionCopy) {
                                    try {
                                        aumid = std::wstring(sessionCopy.SourceAppUserModelId());
                                    } catch (...) {}
                                }
                            }
                            if (!aumid.empty()) {
                                float volumeDelta = (delta > 0) ? 0.02f : -0.02f;
                                AdjustAppVolumeByAUMID(aumid, volumeDelta);
                            }
                            winrt::uninit_apartment();
                        });
                    }
                    e.Handled(true);
                });
            }
            artContainer.Tag(winrt::box_value(winrt::hstring(L"FluentMediaArtContainer")));
            artVizDock = StackPanel();
            artVizDock.Name(L"FluentMedia_AlbumVizDock");
            artVizDock.Orientation(Orientation::Horizontal);
            artVizDock.VerticalAlignment(VerticalAlignment::Center);
            artVizDock.HorizontalAlignment(albumArtLeft ? HorizontalAlignment::Left
                                                          : HorizontalAlignment::Right);
            // Always keep artwork and visualizer distinct, even when the user
            // sets the separate neighbor-gap setting to zero.
            artVizDock.Spacing(3.0);
            artVizDock.Children().Append(artContainer);
            Grid::SetColumn(artVizDock, albumArtLeft ? 0 : 4);
            panel.Children().Append(artVizDock);
        }
        if (g_settings.showTrackTitle || g_settings.showTrackArtist) {
            Border textContainer;
            textContainer.Name(L"FluentMedia_TextContainer");
            textContainer.VerticalAlignment(VerticalAlignment::Center);
            if (albumArtLeft) {
                textContainer.HorizontalAlignment(HorizontalAlignment::Left);
            } else {
                textContainer.HorizontalAlignment(HorizontalAlignment::Right);
            }
            if (g_settings.textAreaMinWidth > 0) {
                textContainer.MinWidth((double)g_settings.textAreaMinWidth);
            }
            if (g_settings.textAreaMaxWidth > 0) {
                textContainer.MaxWidth((double)g_settings.textAreaMaxWidth);
            }
            if (g_settings.textAreaMinHeight > 0) {
                textContainer.MinHeight((double)g_settings.textAreaMinHeight);
            }
            if (g_settings.textAreaMaxHeight > 0) {
                textContainer.MaxHeight((double)g_settings.textAreaMaxHeight);
            }
            double leftMargin = (double)g_settings.textAreaLeftMargin;
            double rightMargin = (double)g_settings.textAreaRightMargin;
            textContainer.Margin({leftMargin, 0, rightMargin, 0});
            if (g_settings.showDebugBorders) {
                textContainer.BorderBrush(MakeBrush({0xFF, 0x00, 0xFF, 0xFF}));
                textContainer.BorderThickness({1,1,1,1});
            }
            StackPanel textStack;
            textStack.Name(kTextStackName);
            textStack.Orientation(Orientation::Vertical);
            textStack.VerticalAlignment(VerticalAlignment::Center);
            if (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) {
                textStack.HorizontalAlignment(HorizontalAlignment::Stretch);
            } else {
                textStack.HorizontalAlignment(g_settings.mirrorLayout ? HorizontalAlignment::Right : HorizontalAlignment::Left);
            }
            textStack.Spacing((double)g_settings.textSpacing);
            if (g_settings.showTrackTitle || g_settings.showTrackArtist) {
                TextBlock titleBlock{nullptr};
                TextBlock artistBlock{nullptr};
                if (g_settings.showTrackTitle) {
                    titleBlock = TextBlock();
                    titleBlock.Name(kTitleBlockName);
                    titleBlock.FontSize((double)g_settings.titleFontSize);
                    std::wstring titleFontName = g_settings.titleFont.empty() ? g_settings.titleFontFamily : g_settings.titleFont;
                    if (!titleFontName.empty()) {
                        try {
                            titleBlock.FontFamily(Media::FontFamily(titleFontName));
                        } catch (...) {}
                    }
                    if (!g_settings.titleFontWeight.empty()) {
                        try {
                            auto fontWeight = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontWeight"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(g_settings.titleFontWeight))
                                .as<winrt::Windows::UI::Text::FontWeight>();
                            titleBlock.FontWeight(fontWeight);
                        } catch (...) {}
                    }
                    if (!g_settings.titleFontStyle.empty()) {
                        try {
                            auto fontStyle = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontStyle"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(g_settings.titleFontStyle))
                                .as<winrt::Windows::UI::Text::FontStyle>();
                            titleBlock.FontStyle(fontStyle);
                        } catch (...) {}
                    }
                    if (g_settings.titleCharacterSpacing != 0) {
                        titleBlock.CharacterSpacing(g_settings.titleCharacterSpacing);
                    }
                    titleBlock.Foreground(MakeBrush(textClr));
                    titleBlock.TextWrapping(TextWrapping::NoWrap);
                    titleBlock.TextTrimming(TextTrimming::CharacterEllipsis);
                    titleBlock.TextAlignment(g_settings.mirrorLayout ? TextAlignment::Right : TextAlignment::Left);
                }
                if (g_settings.showTrackArtist) {
                    artistBlock = TextBlock();
                    artistBlock.Name(kArtistBlockName);
                    artistBlock.FontSize((double)g_settings.artistFontSize);
                    std::wstring artistFontName = g_settings.artistFont.empty() ? g_settings.artistFontFamily : g_settings.artistFont;
                    if (!artistFontName.empty()) {
                        try {
                            artistBlock.FontFamily(Media::FontFamily(artistFontName));
                        } catch (...) {}
                    }
                    if (!g_settings.artistFontWeight.empty()) {
                        try {
                            auto fontWeight = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontWeight"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(g_settings.artistFontWeight))
                                .as<winrt::Windows::UI::Text::FontWeight>();
                            artistBlock.FontWeight(fontWeight);
                        } catch (...) {}
                    }
                    if (!g_settings.artistFontStyle.empty()) {
                        try {
                            auto fontStyle = Markup::XamlBindingHelper::ConvertValue(
                                winrt::Windows::UI::Xaml::Interop::TypeName{
                                    winrt::hstring{L"Windows.UI.Text.FontStyle"},
                                    winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
                                },
                                winrt::box_value(g_settings.artistFontStyle))
                                .as<winrt::Windows::UI::Text::FontStyle>();
                            artistBlock.FontStyle(fontStyle);
                        } catch (...) {}
                    }
                    if (g_settings.artistCharacterSpacing != 0) {
                        artistBlock.CharacterSpacing(g_settings.artistCharacterSpacing);
                    }
                    artistBlock.Foreground(MakeBrush(artistClr));
                    artistBlock.TextWrapping(TextWrapping::NoWrap);
                    artistBlock.TextTrimming(TextTrimming::CharacterEllipsis);
                    artistBlock.TextAlignment(g_settings.mirrorLayout ? TextAlignment::Right : TextAlignment::Left);
                }
                auto MakeScrollView = [&](Canvas& scrollView, TextBlock& block, const wchar_t* viewName, const wchar_t* blockName, const wchar_t* cloneName) {
                    scrollView = Canvas();
                    scrollView.Name(viewName);
                    scrollView.VerticalAlignment(VerticalAlignment::Center);
                    scrollView.HorizontalAlignment(g_settings.mirrorLayout ? HorizontalAlignment::Right : HorizontalAlignment::Left);
                    scrollView.Width(100.0);
                    block.Name(blockName);
                    block.TextTrimming(TextTrimming::None);
                    block.TextWrapping(TextWrapping::NoWrap);
                    Canvas::SetLeft(block, 0.0);
                    Canvas::SetTop(block, 0.0);
                    scrollView.Children().Append(block);
                    if (g_settings.scrollMode == L"loop") {
                        TextBlock clone;
                        clone.Name(cloneName);
                        clone.Text(block.Text());
                        clone.FontSize(block.FontSize());
                        clone.FontFamily(block.FontFamily());
                        clone.FontWeight(block.FontWeight());
                        clone.FontStyle(block.FontStyle());
                        clone.CharacterSpacing(block.CharacterSpacing());
                        clone.Foreground(block.Foreground());
                        clone.TextWrapping(TextWrapping::NoWrap);
                        clone.TextTrimming(TextTrimming::None);
                        clone.TextAlignment(block.TextAlignment());
                        Canvas::SetLeft(clone, 9999.0);
                        Canvas::SetTop(clone, 0.0);
                        scrollView.Children().Append(clone);
                    }
                    auto geo = winrt::Windows::UI::Xaml::Media::RectangleGeometry();
                    scrollView.Clip(geo);
                    block.SizeChanged([scrollView, geo](winrt::Windows::Foundation::IInspectable const&, SizeChangedEventArgs const& e) mutable {
                        try {
                            double h = e.NewSize().Height;
                            if (h < 1.0) h = 16.0;
                            double w = scrollView.Width();
                            scrollView.Height(h);
                            geo.Rect({0, 0, (float)w, (float)h});
                        } catch (...) {}
                    });
                };
                if (g_settings.swapTitleArtist) {
                    if (artistBlock) {
                        if (g_settings.enableArtistScrolling) {
                            Canvas artistScrollView;
                            MakeScrollView(artistScrollView, artistBlock, kArtistScrollViewName, kArtistBlockName, kArtistCloneName);
                            textStack.Children().Append(artistScrollView);
                        } else {
                            if (g_settings.enableTitleScrolling && g_settings.textAreaMaxWidth > 0) {
                                artistBlock.MaxWidth((double)g_settings.textAreaMaxWidth);
                            }
                            textStack.Children().Append(artistBlock);
                        }
                    }
                    if (titleBlock) {
                        if (g_settings.enableTitleScrolling) {
                            Canvas titleScrollView;
                            MakeScrollView(titleScrollView, titleBlock, kTitleScrollViewName, kTitleBlockName, kTitleCloneName);
                            textStack.Children().Append(titleScrollView);
                        } else {
                            if (g_settings.enableArtistScrolling && g_settings.textAreaMaxWidth > 0) {
                                titleBlock.MaxWidth((double)g_settings.textAreaMaxWidth);
                            }
                            textStack.Children().Append(titleBlock);
                        }
                    }
                } else {
                    if (titleBlock) {
                        if (g_settings.enableTitleScrolling) {
                            Canvas titleScrollView;
                            MakeScrollView(titleScrollView, titleBlock, kTitleScrollViewName, kTitleBlockName, kTitleCloneName);
                            textStack.Children().Append(titleScrollView);
                        } else {
                            if (g_settings.enableArtistScrolling && g_settings.textAreaMaxWidth > 0) {
                                titleBlock.MaxWidth((double)g_settings.textAreaMaxWidth);
                            }
                            textStack.Children().Append(titleBlock);
                        }
                    }
                    if (artistBlock) {
                        if (g_settings.enableArtistScrolling) {
                            Canvas artistScrollView;
                            MakeScrollView(artistScrollView, artistBlock, kArtistScrollViewName, kArtistBlockName, kArtistCloneName);
                            textStack.Children().Append(artistScrollView);
                        } else {
                            if (g_settings.enableTitleScrolling && g_settings.textAreaMaxWidth > 0) {
                                artistBlock.MaxWidth((double)g_settings.textAreaMaxWidth);
                            }
                            textStack.Children().Append(artistBlock);
                        }
                    }
                }
            }
            textContainer.Child(textStack);
            Grid::SetColumn(textContainer, 2);
            panel.Children().Append(textContainer);
        }
        if (g_settings.showMediaButtons) {
            StackPanel ctrlPanel;
            ctrlPanel.Orientation(Orientation::Horizontal);
            ctrlPanel.Spacing((double)g_settings.buttonSpacing);
            ctrlPanel.VerticalAlignment(VerticalAlignment::Center);
            ctrlPanel.HorizontalAlignment(buttonsLeft ? HorizontalAlignment::Left : HorizontalAlignment::Right);
            std::vector<MediaButtonConfig> currentButtons;
            try {
                std::lock_guard<std::mutex> lock(g_mediaButtonsMutex);
                currentButtons = g_mediaButtons;
            } catch (const std::exception& e) {
                Wh_Log(L"CreatePlayerGrid: Exception accessing media buttons (std::exception), using defaults");
                currentButtons = {
                    {MediaButtonType::Previous, 1},
                    {MediaButtonType::PlayPause, 2},
                    {MediaButtonType::Next, 3}
                };
            } catch (...) {
                Wh_Log(L"CreatePlayerGrid: Unknown exception accessing media buttons, using defaults");
                currentButtons = {
                    {MediaButtonType::Previous, 1},
                    {MediaButtonType::PlayPause, 2},
                    {MediaButtonType::Next, 3}
                };
            }
            bool hasButtons = !currentButtons.empty();
            if (hasButtons) {
                try {
                    ctrlPanel.Margin({(double)g_settings.mediaButtonsLeftMargin, 0, (double)g_settings.mediaButtonsRightMargin, 0});
                } catch (...) {
                    Wh_Log(L"CreatePlayerGrid: Exception setting control panel margin");
                }
            }
            if (g_settings.showDebugBorders) {
                try {
                    Border ctrlDebugBorder;
                    ctrlDebugBorder.BorderBrush(MakeBrush({0xFF, 0xFF, 0x00, 0xFF}));
                    ctrlDebugBorder.BorderThickness({1,1,1,1});
                    Grid::SetColumn(ctrlDebugBorder, buttonsLeft ? 1 : 3);
                    panel.Children().Append(ctrlDebugBorder);
                } catch (...) {
                    Wh_Log(L"CreatePlayerGrid: Exception creating debug border");
                }
            }
            for (size_t i = 0; i < currentButtons.size(); i++) {
                try {
                    const auto& btnCfg = currentButtons[i];
                    auto btn = MakeControlButton(btnCfg.cmd, false, buttonClr);
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
                        case MediaButtonType::SwitchSessions:
                            btn.Name(kSwitchSessionsBtnName);
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
                Grid::SetColumn(ctrlPanel, 1);
            } else {
                Grid::SetColumn(ctrlPanel, 3);
            }
            if (hasButtons) {
                panel.Children().Append(ctrlPanel);
            }
        }
        if (g_settings.showLyrics) {
            auto lyrics = BuildLyricsOnlyPlayerGrid();
            lyrics.Name(L"FluentLyrics_Embedded");
            TranslateTransform lyricOffset;
            lyricOffset.X(g_settings.lyricsHorizontalOffset);
            lyricOffset.Y(g_settings.lyricsVerticalOffset);
            lyrics.RenderTransform(lyricOffset);
            lyrics.Children().RemoveAt(0); // The outer player already owns the background.
            const double gap = g_settings.compactLayout ? 2.0 : 6.0;
            lyrics.Margin({gap, 0, gap, 0});
            ColumnDefinition lyricColumn;
            lyricColumn.Width({1.0, GridUnitType::Auto});
            if (g_settings.lyricsSide == L"left") {
                panel.ColumnDefinitions().InsertAt(0, lyricColumn);
                for (auto child : panel.Children()) {
                    if (auto fe = child.try_as<FrameworkElement>()) Grid::SetColumn(fe, Grid::GetColumn(fe) + 1);
                }
                Grid::SetColumn(lyrics, 0);
            } else {
                panel.ColumnDefinitions().Append(lyricColumn);
                Grid::SetColumn(lyrics, (int)panel.ColumnDefinitions().Size() - 1);
            }
            panel.Children().Append(lyrics);
        }
        if (g_settings.vizEnabled) {
            try {
                auto vizEl = BuildVisualizerElement();
                if (vizEl) {
                    // The visualizer has an explicit, interruptible XAML storyboard
                    // for dock changes. Don't add RepositionThemeTransition too.
                    bool left = (g_settings.vizPosition == L"left");
                    if (g_settings.mirrorLayout) left = !left;
                    ColumnDefinition vcol;
                    vcol.Width({1.0, GridUnitType::Auto});
                    if (left) {
                        panel.ColumnDefinitions().InsertAt(0, vcol);
                        auto kids = panel.Children();
                        for (uint32_t k = 0; k < kids.Size(); k++) {
                            if (auto fe = kids.GetAt(k).try_as<FrameworkElement>())
                                Grid::SetColumn(fe, Grid::GetColumn(fe) + 1);
                        }
                        Grid::SetColumn(vizEl, 0);
                    } else {
                        panel.ColumnDefinitions().Append(vcol);
                        Grid::SetColumn(vizEl, (int)panel.ColumnDefinitions().Size() - 1);
                    }
                    panel.Children().Append(vizEl);
                    // Normal lyrics-present placement stays in its own column.
                    // Missing-lyrics placement re-parents into the horizontal
                    // album-art dock below, preserving measured widths.
                    if (artVizDock)
                        g_vizNormalColumn = Grid::GetColumn(vizEl);
                }
            } catch (...) {
                Wh_Log(L"BuildPlayerGrid: Exception adding inline visualizer");
            }
        }
        Grid wrapper;
        wrapper.Name(kGridName);
        TranslateTransform positionOffset;
        positionOffset.X(g_settings.horizontalOffset);
        positionOffset.Y(g_settings.verticalOffset);
        wrapper.RenderTransform(positionOffset);
        wrapper.VerticalAlignment(g_settings.fullHeightHitArea ? VerticalAlignment::Stretch : VerticalAlignment::Center);
        wrapper.HorizontalAlignment(HorizontalAlignment::Left);
        AddLayoutAnchorOverlay(wrapper, L"FluentMedia_DebugPlayerAnchors", {0xD0, 0xFF, 0x50, 0x50});
        try {
            if (g_settings.enableSmoothPositionAnimation) {
                TransitionCollection transitions;
                RepositionThemeTransition marginTransition;
                transitions.Append(marginTransition);
                wrapper.Transitions(transitions);
            }
        } catch (...) {}
        if (hasTextOrButtons && g_settings.playerMinWidth > 0) {
            wrapper.MinWidth((double)g_settings.playerMinWidth);
        }
        if (g_settings.playerMaxWidth > 0) {
            wrapper.MaxWidth((double)g_settings.playerMaxWidth);
        }

        wrapper.Background(MakeBrush({0x00, 0, 0, 0}));

        Canvas::SetZIndex(backgroundBorder, 0);
        Canvas::SetZIndex(playerButton, 1);
        Canvas::SetZIndex(panel, 2);
        wrapper.Children().Append(backgroundBorder);
        wrapper.Children().Append(playerButton);
        wrapper.Children().Append(panel);
        ApplyFluentMediaButtonStyle(playerButton);
        if (!g_settings.showDebugBorders) {
            playerButton.BorderThickness({1, 1, 1, 1});
        }
        auto isPressed = std::make_shared<bool>(false);
        auto isHovered = std::make_shared<bool>(false);
        auto playerNormalBg = MakeBackgroundBrush();
        auto updatePlayerVisualState = [playerButton, playerNormalBg, isPressed, isHovered]() {
            bool hovered = *isHovered || g_miniPlayerFlyoutOpen;
            ApplyPlayerButtonState(playerButton, playerNormalBg, hovered, *isPressed);
        };
        g_playerButtonStateUpdater = updatePlayerVisualState;
        RunWhenButtonReady(playerButton, [playerButton, playerNormalBg]() {
            try {
                SetupPlayerCommonStates(playerButton, playerNormalBg);
                GoToCommonState(playerButton, IsHoverEffectEnabled(g_settings.playerHoverEffectMode), false, false);
            } catch (...) {}
        });
        wrapper.Loaded([playerButton, playerNormalBg](auto const&, auto const&) {
            try {
                SetupPlayerCommonStates(playerButton, playerNormalBg);
                GoToCommonState(playerButton, IsHoverEffectEnabled(g_settings.playerHoverEffectMode), false, false);
            } catch (...) {}
        });
        panel.PointerEntered([isHovered, updatePlayerVisualState](auto const&, auto const&) {
            *isHovered = true;
            updatePlayerVisualState();
        });
        panel.PointerExited([weak = winrt::make_weak(wrapper), isHovered, updatePlayerVisualState](auto const&, PointerRoutedEventArgs const& e) {
            auto wrapper = weak.get();
            if (!wrapper) return;
            bool stillInside = false;
            try {
                auto pos = e.GetCurrentPoint(wrapper).Position();
                auto bounds = wrapper.RenderSize();
                stillInside = pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height;
            } catch (...) {}
            if (!stillInside) {
                *isHovered = false;
                updatePlayerVisualState();
            }
        });
    wrapper.PointerEntered([isHovered, updatePlayerVisualState](auto const&, auto const&) mutable {
        *isHovered = true;
        updatePlayerVisualState();
    });
    wrapper.PointerExited([isHovered, updatePlayerVisualState](auto const&, auto const&) mutable {
        *isHovered = false;
        updatePlayerVisualState();
    });
    wrapper.PointerPressed([isPressed, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
        if (auto elem = sender.template try_as<UIElement>()) {
            elem.CapturePointer(e.Pointer());
        }
        *isPressed = true;
        updatePlayerVisualState();
    });
    auto wrapperDblClickLastTime = std::make_shared<ULONGLONG>(0);
    auto wrapperDblClickLastKind = std::make_shared<winrt::Windows::UI::Input::PointerUpdateKind>(
        winrt::Windows::UI::Input::PointerUpdateKind::Other);
    wrapper.PointerReleased([isPressed, isHovered, updatePlayerVisualState, wrapperDblClickLastTime, wrapperDblClickLastKind](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
        *isPressed = false;
        bool actuallyHovered = false;
        if (auto elem = sender.template try_as<UIElement>()) {
            elem.ReleasePointerCapture(e.Pointer());
            try {
                auto pointerPoint = e.GetCurrentPoint(elem);
                auto bounds = elem.RenderSize();
                auto pos = pointerPoint.Position();
                actuallyHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
            } catch (...) { actuallyHovered = false; }
        }
        *isHovered = actuallyHovered;
        updatePlayerVisualState();
        if (g_unloading || e.Handled()) return;
        if (actuallyHovered) {
            auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
            auto fe = sender.template try_as<FrameworkElement>();
            ULONGLONG now = GetTickCount64();
            UINT dblClickMs = GetDoubleClickTime();
            bool isDouble = (kind == *wrapperDblClickLastKind) &&
                            (now - *wrapperDblClickLastTime) <= dblClickMs;
            *wrapperDblClickLastTime = isDouble ? 0 : now;
            *wrapperDblClickLastKind = kind;
            using Kind = winrt::Windows::UI::Input::PointerUpdateKind;
            if (kind == Kind::LeftButtonReleased) {
                if (isDouble) ExecuteMediaAction(g_settings.playerLeftDoubleClick, fe);
                else          ExecuteMediaAction(g_settings.playerLeftClick, fe);
            } else if (kind == Kind::RightButtonReleased) {
                if (isDouble) ExecuteMediaAction(g_settings.playerRightDoubleClick, fe);
                else          ExecuteMediaAction(g_settings.playerRightClick, fe);
            } else if (kind == Kind::MiddleButtonReleased) {
                if (isDouble) ExecuteMediaAction(g_settings.playerMiddleDoubleClick, fe);
                else          ExecuteMediaAction(g_settings.playerMiddleClick, fe);
            }
        }
    });
    wrapper.PointerCanceled([isPressed, isHovered, updatePlayerVisualState](auto const&, auto const&) mutable {
        *isPressed = false;
        *isHovered = false;
        updatePlayerVisualState();
    });
    wrapper.PointerCaptureLost([isPressed, isHovered, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
        *isPressed = false;
        if (auto elem = sender.template try_as<UIElement>()) {
            try {
                auto pointerPoint = e.GetCurrentPoint(elem);
                auto bounds = elem.RenderSize();
                auto pos = pointerPoint.Position();
                *isHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
            } catch (...) { *isHovered = false; }
        }
        updatePlayerVisualState();
    });
        wrapper.Tag(winrt::box_value(winrt::hstring(L"FluentMediaBarWrapper")));
        wrapper.PointerWheelChanged([](auto const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (g_unloading) return;
            auto action = g_settings.playerWheelAction;
            if (action == L"none") return;
            auto props = e.GetCurrentPoint(nullptr).Properties();
            int delta = props.MouseWheelDelta();
            if (action == L"switch_tracks") {
                if (delta > 0) SendMediaCommandAsync(1);
                else if (delta < 0) SendMediaCommandAsync(3);
                DispatchMediaUpdate();
            } else if (action == L"switch_tracks_inverted") {
                if (delta > 0) SendMediaCommandAsync(3);
                else if (delta < 0) SendMediaCommandAsync(1);
                DispatchMediaUpdate();
            } else if (action == L"switch_sessions") {
                if (delta != 0) SwitchMediaSession();
            } else if (action == L"system_sound") {
                SpawnTrackedWorker([delta]() { ChangeSystemVolume(delta > 0); });
            } else if (action == L"app_sound") {
                SpawnTrackedWorker([delta]() {
                    winrt::init_apartment(winrt::apartment_type::multi_threaded);
                    std::wstring aumid;
                    {
                        GlobalSystemMediaTransportControlsSession sessionCopy{nullptr};
                        {
                            std::lock_guard<std::mutex> lk(g_sessionMtx);
                            sessionCopy = g_currentSession;
                        }
                        if (sessionCopy) {
                            try {
                                aumid = std::wstring(sessionCopy.SourceAppUserModelId());
                            } catch (...) {}
                        }
                    }
                    if (!aumid.empty()) {
                        float volumeDelta = (delta > 0) ? 0.02f : -0.02f;
                        AdjustAppVolumeByAUMID(aumid, volumeDelta);
                    }
                    winrt::uninit_apartment();
                });
            }
            e.Handled(true);
        });
        try {
            winrt::Windows::UI::Xaml::Interop::TypeName gridType;
            gridType.Name = L"Windows.UI.Xaml.Controls.Grid";
            gridType.Kind = winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata;
            winrt::Windows::UI::Xaml::Style wrapperStyle(gridType);
            wrapper.Style(wrapperStyle);
        } catch (...) {}
        try {
            ToolTip tt;
            TextBlock ttText;
            ttText.TextWrapping(TextWrapping::Wrap);
            ttText.MaxWidth(300);
            tt.Content(ttText);
            ToolTipService::SetToolTip(wrapper, tt);
            g_playerToolTip = tt;
        } catch (...) {}
        return wrapper;
    } catch (...) {
        Wh_Log(L"BuildPlayerGrid: Exception occurred");
        return nullptr;
    }
}
struct InjectionTarget {
    Panel grid{nullptr};
    int  insertCol = 0;
};
static int RemovePlayerGridChildren(Panel const& targetGrid) {
    if (!targetGrid) return -1;
    int firstCol = -1;
    for (int i = (int)targetGrid.Children().Size() - 1; i >= 0; --i) {
        auto fe = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == kLyricsOverlayHostName) {
            try { targetGrid.Children().RemoveAt(i); } catch (...) {}
        } else if (fe && fe.Name() == kGridName) {
            if (firstCol < 0) firstCol = Grid::GetColumn(fe);
            try { targetGrid.Children().RemoveAt(i); } catch (...) {}
        }
    }
    return firstCol;
}
static void RemoveAnchorDebugOverlays(Panel const& targetGrid) {
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
    if (!g_settings.showLayoutAnchors || !targetElem) {
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
static FrameworkElement FindChildByClassName(FrameworkElement const& parent, const wchar_t* className, int depth);
static Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    auto taskbarFrame = winrt::get_class_name(root) == L"Taskbar.TaskbarFrame"
        ? root : FindChildByClassName(root, L"Taskbar.TaskbarFrame", 12);
    if (!taskbarFrame) return nullptr;
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
// Grid::GetColumn on an INNER XAML element often returns zero. Find the
// nearest DIRECT child of the tray grid before reading the attached column.
static int TrayColumn(Panel const& trayGrid, FrameworkElement const& element) {
    if (!trayGrid || !element) return -1;
    try {
        auto current = element;
        for (int i = 0; i < 24 && current; ++i) {
            auto parent = VisualTreeHelper::GetParent(current).try_as<FrameworkElement>();
            if (!parent) break;
            if (parent == trayGrid.try_as<FrameworkElement>()) {
                if (trayGrid.try_as<Grid>()) return Grid::GetColumn(current);
                uint32_t index = 0;
                if (trayGrid.Children().IndexOf(current, index)) return (int)index;
                return -1;
            }
            current = parent;
        }
    } catch (...) {}
    return -1;
}
static int TraySlotCount(Panel const& panel) {
    if (auto grid = panel.try_as<Grid>()) return (int)grid.ColumnDefinitions().Size();
    return (int)panel.Children().Size();
}
static FrameworkElement LyricsFindTarget(FrameworkElement const& repeater, std::wstring_view position);
static InjectionTarget ResolveInjectionTarget(
    FrameworkElement const& root,
    std::wstring_view position)
{
    auto trayFrame = FindChildByName(root, L"SystemTrayFrameGrid");
    if (auto trayGrid = trayFrame ? trayFrame.try_as<Panel>() : nullptr) {
        int col = -1;
        if      (position == L"tray_right")
            col = TraySlotCount(trayGrid);
        else if (position == L"tray_left")
            col = 0;
        else if (position == L"tray_before_clock") {
            auto clockBtn = FindChildByName(trayGrid, L"NotificationCenterButton");
            if (!clockBtn) clockBtn = FindChildByName(root, L"NotificationCenterButton");
            col = clockBtn ? TrayColumn(trayGrid, clockBtn) : -1;
        }
        else if (position == L"tray_after_clock") {
            auto clock = FindTrayElement(trayGrid, root, L"NotificationCenterButton");
            const int clockCol = TrayColumn(trayGrid, clock);
            if (clockCol >= 0) col = clockCol + 1;
        }
        else if (position == L"tray_before_omni_left") {
            auto omniBtn = FindChildByName(trayGrid, L"ControlCenterButton");
            if (!omniBtn) omniBtn = FindChildByName(root, L"ControlCenterButton");
            col = omniBtn ? TrayColumn(trayGrid, omniBtn) : -1;
        }
        else if (position == L"tray_before_omni_right") {
            auto omniBtn = FindChildByName(trayGrid, L"ControlCenterButton");
            if (!omniBtn) omniBtn = FindChildByName(root, L"ControlCenterButton");
            col = TrayColumn(trayGrid, omniBtn);
            if (col >= 0) ++col;
        }
        else if (position == L"tray_language_left") {
            auto languageBtn = FindTrayElement(trayGrid, root, L"NonActivatableStack");
            col = languageBtn ? TrayColumn(trayGrid, languageBtn) : -1;
        }
        else if (position == L"tray_language_right") {
            auto languageBtn = FindTrayElement(trayGrid, root, L"NonActivatableStack");
            col = TrayColumn(trayGrid, languageBtn); if (col >= 0) ++col;
        }
        else if (position == L"tray_hidden_icons_left") {
            auto hiddenIconsBtn = FindTrayElement(trayGrid, root, L"NotifyIconStack");
            col = hiddenIconsBtn ? TrayColumn(trayGrid, hiddenIconsBtn) : -1;
        }
        else if (position == L"tray_hidden_icons_right") {
            auto hiddenIconsBtn = FindTrayElement(trayGrid, root, L"NotifyIconStack");
            col = TrayColumn(trayGrid, hiddenIconsBtn); if (col >= 0) ++col;
        }
        else if (position == L"tray_icons_left") {
            auto trayIcons = FindTrayElement(trayGrid, root, L"NotificationAreaIcons");
            col = trayIcons ? TrayColumn(trayGrid, trayIcons) : -1;
        }
        else if (position == L"tray_icons_right") {
            auto trayIcons = FindTrayElement(trayGrid, root, L"NotificationAreaIcons");
            col = TrayColumn(trayGrid, trayIcons); if (col >= 0) ++col;
        }
        else if (position == L"tray_after_showdesktop_left") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            col = showDesktop ? TrayColumn(trayGrid, showDesktop) : -1;
        }
        else if (position == L"tray_after_showdesktop_right") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            col = TrayColumn(trayGrid, showDesktop);
            if (col >= 0) ++col;
            else col = TraySlotCount(trayGrid);
        }
        if (col >= 0) {
            return {trayGrid, col};
        }
    }
    if (position.rfind(L"tray_", 0) == 0) {
        if (auto tray = trayFrame ? trayFrame.try_as<Panel>() : nullptr) {
            Wh_Log(L"Requested tray anchor is unavailable; using Tray - Far left");
            return {tray, 0};
        }
    }
    if (position == L"taskbar_left_start"  ||
        position == L"taskbar_right_start" ||
        position == L"taskbar_after_search_left"||
        position == L"taskbar_after_search_right"||
        position == L"taskbar_after_taskview_left"||
        position == L"taskbar_after_taskview_right"||
        position == L"taskbar_after_widgets_left"||
        position == L"taskbar_after_widgets_right"||
        position == L"taskbar_left_edge"   ||
        position == L"taskbar_center_edge" ||
        position == L"taskbar_right_edge")
    {
        auto rootGrid = FindTaskbarRootGrid(root);
        auto fallback = [&]() -> InjectionTarget {
            auto tray = trayFrame ? trayFrame.try_as<Panel>() : nullptr;
            Wh_Log(L"Requested taskbar anchor is unavailable; using Tray - Far left");
            return tray ? InjectionTarget{tray, 0} : InjectionTarget{};
        };
        if (!rootGrid) return fallback();
        if (position != L"taskbar_left_edge" && position != L"taskbar_center_edge" && position != L"taskbar_right_edge") {
            auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
            auto target = LyricsFindTarget(repeater, position);
            if (!target || target.Visibility() != Visibility::Visible || target.ActualWidth() < 1) return fallback();
        }
        return {rootGrid.as<Panel>(), -1};
    }
    return {};
}
// The Canvas is overlaid across the parent grid: it must not
// pushes/shrinks Start, task buttons, or tray controls. All geometry is in DIPs.
static double LyricsElementX(FrameworkElement const& e, UIElement const& relativeTo) {
    if (!e || e.Visibility() != Visibility::Visible || e.ActualWidth() < 1.0) return -1.0;
    try {
        return e.TransformToVisual(relativeTo).TransformPoint({0, 0}).X;
    } catch (...) { return -1.0; }
}
static FrameworkElement LyricsFindTarget(FrameworkElement const& repeater,
                                        std::wstring_view position) {
    if (!repeater) return nullptr;
    if (position == L"taskbar_left_start" || position == L"taskbar_right_start") {
        for (auto name : kStartButtonNames) if (auto target = FindChildByName(repeater, name)) return target;
    }
    if (position == L"taskbar_after_search_left" || position == L"taskbar_after_search_right") {
        if (auto target = FindChildByName(repeater, L"SearchButton")) return target;
        return FindChildByClassName(repeater, L"Taskbar.TaskbarExtensionElement", 8);
    }
    if (position == L"taskbar_after_taskview_left" || position == L"taskbar_after_taskview_right") {
        if (auto target = FindChildByName(repeater, L"TaskViewButton")) return target;
        return FindChildByClassName(repeater, L"Taskbar.ExperienceToggleButton", 8);
    }
    if (position == L"taskbar_after_widgets_left" || position == L"taskbar_after_widgets_right") {
        if (auto target = FindChildByName(repeater, L"AugmentedEntryPointButton")) return target;
        return FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton", 8);
    }
    return nullptr;
}
static void AnimateLyricsWidth(double targetWidth) {
    if (!g_playerGrid || !g_lyricsViewport) return;
    targetWidth = std::clamp(targetWidth, 1.0, (double)g_settings.lyricsWidth);
    if (std::abs(targetWidth - g_lyricsTargetWidth) < 2.0) return;
    const double from = std::max(1.0, g_playerGrid.ActualWidth());
    try { if (g_lyricsWidthStoryboard) g_lyricsWidthStoryboard.Stop(); } catch (...) {}
    g_lyricsWidthStoryboard = nullptr;
    g_lyricsTargetWidth = targetWidth;
    try {
        // Let the viewport SizeChanged handler update the clip throughout the
        // width storyboard; preclipping to the destination causes a visible snap.
        // Base values remain the destination when a storyboard is interrupted.
        g_playerGrid.Width(targetWidth);
        g_lyricsViewport.Width(targetWidth);
        Storyboard sb;
        auto easing = CubicEase();
        easing.EasingMode(EasingMode::EaseOut);
        auto animate = [&](FrameworkElement const& el) {
            DoubleAnimation a;
            a.From(from);
            a.To(targetWidth);
            a.Duration(DurationHelper::FromTimeSpan(
                std::chrono::milliseconds(g_settings.lyricsResizeAnimationMs)));
            a.EasingFunction(easing);
            Storyboard::SetTarget(a, el);
            Storyboard::SetTargetProperty(a, L"Width");
            sb.Children().Append(a);
        };
        animate(g_playerGrid);
        animate(g_lyricsViewport);
        if (!ReduceMotionForPower()) {
            g_lyricsWidthStoryboard = sb;
            sb.Begin();
        }
    } catch (...) {
        try { g_playerGrid.Width(targetWidth); g_lyricsViewport.Width(targetWidth); } catch (...) {}
    }
}
static void AnimateLyricsAdaptiveOpacity(bool visible) {
    if (!g_playerGrid || visible == g_lyricsTargetVisible) return;
    g_lyricsTargetVisible = visible;
    try {
        const double from = std::clamp(g_playerGrid.Opacity(), 0.0, 1.0);
        if (g_lyricsOpacityStoryboard) g_lyricsOpacityStoryboard.Stop();
        g_lyricsOpacityStoryboard = nullptr;
        const double to = visible ? 1.0 : 0.0;
        g_playerGrid.Opacity(to);
        Storyboard sb;
        DoubleAnimation fade;
        fade.From(from);
        fade.To(to);
        fade.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(170)));
        Storyboard::SetTarget(fade, g_playerGrid);
        Storyboard::SetTargetProperty(fade, L"Opacity");
        sb.Children().Append(fade);
        if (!ReduceMotionForPower()) {
            g_lyricsOpacityStoryboard = sb;
            sb.Begin();
        }
    } catch (...) {
        try { g_playerGrid.Opacity(visible ? 1.0 : 0.0); } catch (...) {}
    }
}
// Collect interactive taskbar items, not their full-width repeater/container.
static void CollectTaskbarObstacles(FrameworkElement const& node, UIElement const& relativeTo,
                                    std::vector<PlayerGeometry::Interval>& occupied, int depth = 12) {
    if (!node || depth <= 0 || node.Visibility() != Visibility::Visible) return;
    const std::wstring cls(winrt::get_class_name(node));
    const auto name = node.Name();
    if (name == kLyricsOverlayHostName || name == kGridName) return;
    const bool item = cls.find(L"Button") != std::wstring::npos ||
        cls == L"Taskbar.TaskbarExtensionElement" || name == L"AugmentedEntryPointButton";
    if (item && node.ActualWidth() > 0) {
        const double x = LyricsElementX(node, relativeTo);
        if (x >= 0) occupied.push_back({x, x + node.ActualWidth()});
        return;
    }
    const int count = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        if (auto child = VisualTreeHelper::GetChild(node, i).try_as<FrameworkElement>())
            CollectTaskbarObstacles(child, relativeTo, occupied, depth - 1);
    }
}
static void UpdateAdaptiveLyricsOverlay(Grid const& rootGrid,
                                        FrameworkElement const& repeater,
                                        FrameworkElement const& trayFrame) {
    if (!g_lyricsOverlayHost || !g_playerGrid || !rootGrid || g_unloading.load()) return;
    if (g_playerGrid.Visibility() != Visibility::Visible) return;
    auto now = std::chrono::steady_clock::now();
    if (now - g_lyricsLastLayoutTime < std::chrono::milliseconds(50)) return;
    g_lyricsLastLayoutTime = now;
    try {
        const double canvasW = rootGrid.ActualWidth();
        const double canvasH = rootGrid.ActualHeight();
        if (canvasW < 1 || canvasH < 1) return;
        // The overlay measures to zero; explicitly constrain its coordinate space.
        if (std::abs(g_lyricsOverlayHost.Width() - canvasW) > .5 || !std::isfinite(g_lyricsOverlayHost.Width()))
            g_lyricsOverlayHost.Width(canvasW);
        if (std::abs(g_lyricsOverlayHost.Height() - canvasH) > .5 || !std::isfinite(g_lyricsOverlayHost.Height()))
            g_lyricsOverlayHost.Height(canvasH);
        const double requested = g_settings.lyricsOnly ? (double)g_settings.lyricsWidth : g_playerOverlayWidth;
        if (requested <= 0) return;
        std::vector<PlayerGeometry::Interval> occupied;
        CollectTaskbarObstacles(rootGrid, g_lyricsOverlayHost, occupied);
        const double trayX = LyricsElementX(trayFrame, g_lyricsOverlayHost);
        if (trayX >= 0) occupied.push_back({trayX, canvasW});
        // A repeater with no recognizable items is unsafe for interactive overlays.
        if (occupied.empty() || (!repeater && !trayFrame)) { AnimateLyricsAdaptiveOpacity(false); return; }
        auto mode = PlayerGeometry::Anchor::LeftEdge;
        const auto& pos = g_settings.position;
        double anchorX = 0, anchorWidth = 0;
        if (pos == L"taskbar_center_edge") mode = PlayerGeometry::Anchor::Center;
        else if (pos == L"taskbar_right_edge") mode = PlayerGeometry::Anchor::RightEdge;
        else if (pos != L"taskbar_left_edge") {
            auto target = LyricsFindTarget(repeater, pos);
            anchorX = LyricsElementX(target, g_lyricsOverlayHost);
            if (anchorX < 0) { AnimateLyricsAdaptiveOpacity(false); return; }
            anchorWidth = target.ActualWidth();
            mode = pos.find(L"_left") != std::wstring::npos ? PlayerGeometry::Anchor::Before : PlayerGeometry::Anchor::After;
        }
        const int offset = g_settings.horizontalOffset + (g_settings.lyricsOnly ? g_settings.lyricsHorizontalOffset : 0);
        const double minimum = g_settings.lyricsOnly && g_settings.lyricsAdaptiveWidth
            ? (double)std::min(g_settings.lyricsAdaptiveMinWidth, g_settings.lyricsWidth) : requested;
        const auto place = PlayerGeometry::Place(canvasW, requested, minimum,
            g_settings.playerMarginLeft, g_settings.playerMarginRight, g_settings.lyricsAdaptiveGap,
            mode, anchorX, anchorWidth, offset, std::move(occupied));
        if (place.visible) {
            // X is clamped in geometry; neutralize the transform's X only for overlays.
            if (auto transform = g_playerGrid.RenderTransform().try_as<TranslateTransform>()) transform.X(0);
            if (std::abs(Canvas::GetLeft(g_playerGrid) - place.x) > .5) Canvas::SetLeft(g_playerGrid, place.x);
            const double requestedY = (canvasH - g_playerGrid.ActualHeight()) * .5;
            const double dy = g_settings.verticalOffset + (g_settings.lyricsOnly ? g_settings.lyricsVerticalOffset : 0);
            const double y = std::clamp(requestedY + dy, 0.0, std::max(0.0, canvasH - g_playerGrid.ActualHeight()));
            if (auto transform = g_playerGrid.RenderTransform().try_as<TranslateTransform>()) transform.Y(0);
            if (std::abs(Canvas::GetTop(g_playerGrid) - y) > .5) Canvas::SetTop(g_playerGrid, y);
            if (g_settings.lyricsOnly) AnimateLyricsWidth(place.width);
            else if (!std::isfinite(g_playerGrid.Width()) || std::abs(g_playerGrid.Width() - place.width) > .5)
                g_playerGrid.Width(place.width);
        }
        AnimateLyricsAdaptiveOpacity(place.visible);
        g_playerGrid.IsHitTestVisible(place.visible && (!g_settings.lyricsOnly || LyricsInteractive()));
    } catch (...) { Wh_Log(L"Player overlay: layout update failed"); }
}

static bool InjectPlayerGrid() {
    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"InjectPlayerGrid: No taskbar window found");
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
        if (g_settings.enableTreeDump) {
            DumpXamlTree(root, 0, 5);
            auto rootGrid = FindTaskbarRootGrid(root);
            if (rootGrid) {
                auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
                if (repeater) {
                    DumpXamlTree(repeater, 0, 3);
                }
            }
        }
        auto [targetPanel, insertCol] = ResolveInjectionTarget(root, g_settings.position);
        auto targetGrid = targetPanel ? targetPanel.try_as<Grid>() : nullptr;
        if (!targetPanel) {
            if (g_settings.enableTreeDump) {
                DumpXamlTree(root, 0, 8);
            }
            return false;
        }
        Grid playerGrid = BuildPlayerGrid();
        if (!playerGrid) return false;
        bool startButtonModActive = IsStartButtonModActive(root);
        (void)startButtonModActive;
        bool isTrayGrid = (targetPanel.Name() == L"SystemTrayFrameGrid");
        RemovePlayerGridChildren(targetPanel);
        RemoveAnchorDebugOverlays(targetPanel);
        bool isTaskbarPosition =
            g_settings.position.rfind(L"taskbar_", 0) == 0;
        if (auto stack = targetPanel.try_as<StackPanel>()) {
            playerGrid.Margin({(double)g_settings.playerMarginLeft, 0, (double)g_settings.playerMarginRight, 0});
            stack.Children().InsertAt(std::clamp(insertCol, 0, (int)stack.Children().Size()), playerGrid);
            g_playerColumn = -1;
        } else if (isTaskbarPosition && !isTrayGrid) {
            // Canvas adds ZERO desired size to the root Grid. This avoids
            // compressing taskbar buttons regardless of lyrics width.
            Canvas overlay;
            overlay.Name(kLyricsOverlayHostName);
            overlay.HorizontalAlignment(HorizontalAlignment::Stretch);
            overlay.VerticalAlignment(VerticalAlignment::Stretch);
            overlay.IsHitTestVisible(!g_settings.lyricsOnly || LyricsInteractive());
            Grid::SetColumn(overlay, 0);
            Grid::SetRow(overlay, 0);
            Grid::SetColumnSpan(overlay, std::max(1, (int)targetGrid.ColumnDefinitions().Size()));
            Grid::SetRowSpan(overlay, std::max(1, (int)targetGrid.RowDefinitions().Size()));
            playerGrid.Margin({0, 0, 0, 0});
            Canvas::SetLeft(playerGrid, (double)g_settings.playerMarginLeft);
            Canvas::SetTop(playerGrid, 0.0);
            playerGrid.Measure({std::numeric_limits<float>::infinity(), (float)std::max(24.0, targetGrid.ActualHeight())});
            g_playerOverlayWidth = g_settings.lyricsOnly ? g_settings.lyricsWidth : std::max(1.0, (double)playerGrid.DesiredSize().Width);
            overlay.Children().Append(playerGrid);
            targetGrid.Children().Append(overlay);
            Canvas::SetZIndex(overlay, 1000);
            g_lyricsOverlayHost = overlay;
            g_playerColumn = -1;
            g_lyricsTargetWidth = (double)g_settings.lyricsWidth;
            g_lyricsTargetVisible = true;
            g_lyricsLastLayoutTime = {};
            // Content-independent update: responds when the Windows taskbar
            // repeater shifts Start/apps as new windows open and close.
            auto repeaterForOverlay = FindChildByName(targetGrid, L"TaskbarFrameRepeater");
            auto trayForOverlay = FindChildByName(targetGrid, L"SystemTrayFrameGrid");
            g_layoutUpdateToken = targetGrid.LayoutUpdated(
                [targetGrid, repeaterForOverlay, trayForOverlay](
                    winrt::Windows::Foundation::IInspectable const&,
                    winrt::Windows::Foundation::IInspectable const&) {
                    UpdateAdaptiveLyricsOverlay(targetGrid, repeaterForOverlay, trayForOverlay);
                });
        } else if (isTrayGrid) {
            ColumnDefinition newCol;
            newCol.Width({1.0, GridUnitType::Auto});
            if (insertCol >= (int)targetGrid.ColumnDefinitions().Size()) {
                targetGrid.ColumnDefinitions().Append(newCol);
            } else {
                targetGrid.ColumnDefinitions().InsertAt(insertCol, newCol);
                for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
                    auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                    if (child) {
                        int childCol = Grid::GetColumn(child);
                        if (childCol >= insertCol) Grid::SetColumn(child, childCol + 1);
                        else if (childCol + Grid::GetColumnSpan(child) > insertCol)
                            Grid::SetColumnSpan(child, Grid::GetColumnSpan(child) + 1);
                    }
                }
            }
            playerGrid.Margin({(double)g_settings.playerMarginLeft, 0,
                            (double)g_settings.playerMarginRight, 0});
            Grid::SetColumn(playerGrid, insertCol);
            targetGrid.Children().Append(playerGrid);
            g_playerColumn = insertCol;
        }
        else {
            Wh_Log(L"Unsupported player placement");
            return false;
        }
        g_playerGrid      = playerGrid;
        g_injectionParent = targetPanel;
        g_playerGrid.Visibility(Visibility::Visible);
        RefreshPlayerContents();
        UpdateVisibility();
        g_playerGrid.UpdateLayout();
        if (g_injectionParent) {
            g_injectionParent.UpdateLayout();
            if (g_lyricsOverlayHost) {
                auto parentGrid = g_injectionParent.try_as<Grid>();
                if (parentGrid) {
                    auto buttons = FindChildByName(parentGrid, L"TaskbarFrameRepeater");
                    auto tray = FindChildByName(parentGrid, L"SystemTrayFrameGrid");
                    g_lyricsLastLayoutTime = {};
                    UpdateAdaptiveLyricsOverlay(parentGrid, buttons, tray);
                }
            }
        }
        if (g_playerGrid.ActualWidth() == 0.0 && g_playerGrid.ActualHeight() == 0.0) {
            g_needsUiUpdate = true;
            if (g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
        }
        auto dispatcher = g_playerGrid.Dispatcher();
        if (dispatcher) {
            try {
                dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [=]() {
                    if (!g_unloading && g_playerGrid) RefreshThemeColors();
                });
            } catch (...) {
                Wh_Log(L"InjectPlayerGrid: Failed to dispatch RefreshThemeColors");
            }
        }
        Canvas::SetZIndex(g_playerGrid, 1000);
        OnSessionsChanged();
        g_needsUiUpdate = true;
        return true;
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"InjectPlayerGrid: HRESULT 0x%08X: %s", static_cast<unsigned>(error.code()), error.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"InjectPlayerGrid: Exception during injection");
        return false;
    }
}
// Song-change animation cleanup is defined with the animation helpers below.
// These declarations must precede RemovePlayerGrid, which uses both symbols.
static std::wstring g_lastAnimatedTrack;
static void StopSongChangeAnimations();

static void RemovePlayerGrid() {
    StopSongChangeAnimations();
    g_lastAnimatedTrack.clear();
    try { if (g_vizDockStoryboard) g_vizDockStoryboard.Stop(); } catch (...) {}
    g_vizDockStoryboard = nullptr;
    CancelStartupRetry();
    for (auto& cleanup : g_uiCleanup) { try { cleanup(); } catch (...) {} }
    g_uiCleanup.clear();
    try { if (g_lyricsWidthStoryboard) g_lyricsWidthStoryboard.Stop(); } catch (...) {}
    try { if (g_lyricsOpacityStoryboard) g_lyricsOpacityStoryboard.Stop(); } catch (...) {}
    g_lyricsWidthStoryboard = nullptr;
    g_lyricsOpacityStoryboard = nullptr;
    ResetLyricsVisuals();
    if (!g_injectionParent) return;
    try {
        if (g_miniPlayerFlyoutOpen && g_miniPlayerFlyout) {
            g_miniPlayerExplicitCloseRequested.store(true);
            try { g_miniPlayerFlyout.Hide(); } catch (...) {}
        }
        g_miniPlayerFlyout = nullptr;
        g_miniPlayerFlyoutOpen = false;
    } catch (...) {}
    try {
        if (g_layoutUpdateToken.value) {
            auto targetGrid = g_injectionParent.try_as<Grid>();
            if (targetGrid) {
                try { targetGrid.LayoutUpdated(g_layoutUpdateToken); } catch (...) {}
            }
            g_layoutUpdateToken = {};
        }

        if (g_trackedElement) {
            try {
                if (g_hasTrackedElementOriginalMargin) {
                    g_trackedElement.Margin(g_trackedElementOriginalMargin);
                } else {
                    auto m = g_trackedElement.Margin();
                    if (g_trackPosition == L"left" || g_trackPosition == L"far_left") m.Left = 0;
                    if (g_trackPosition == L"right") m.Right = 0;
                    g_trackedElement.Margin(m);
                }
            } catch (...) {}
            g_trackedElement = nullptr;
        }
        g_hasTrackedElementOriginalMargin = false;
        g_trackPosition = L"";
        auto targetGrid = g_injectionParent.try_as<Grid>();
        const int playerCol = g_playerColumn;
        auto targetPanel = g_injectionParent.try_as<Panel>();
        RemoveAnchorDebugOverlays(targetPanel);
        RemovePlayerGridChildren(targetPanel);
        if (targetGrid && playerCol >= 0 && playerCol < (int)targetGrid.ColumnDefinitions().Size()) {
            for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
                auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (child) {
                    int childCol = Grid::GetColumn(child);
                    if (childCol > playerCol) Grid::SetColumn(child, childCol - 1);
                    else if (childCol < playerCol && childCol + Grid::GetColumnSpan(child) > playerCol)
                        Grid::SetColumnSpan(child, Grid::GetColumnSpan(child) - 1);
                }
            }
            targetGrid.ColumnDefinitions().RemoveAt(playerCol);
        }
        g_playerGrid      = nullptr;
        g_lyricsOverlayHost = nullptr;
        g_lyricsViewport = nullptr;
        g_lyricsGuardClip = nullptr;
        g_lyricsTargetWidth = -1.0;
        g_lyricsTargetVisible = true;
        g_injectionParent = nullptr;
        g_playerColumn    = -1;
        g_playerToolTip   = nullptr;
        g_playerButtonStateUpdater = nullptr;
        g_cachedAlbumTitle.clear();
        g_cachedAlbumArtist.clear();
        g_cachedThumbnailBytes.clear();
        g_cachedPaletteHash = 0;
        g_blurBgCache.Invalidate();
        g_cachedAppIconSize = -1;
        g_scrollCachedTitle.clear();
        g_scrollCachedArtist.clear();
        ResetScrollState(g_titleScroll);
        ResetScrollState(g_artistScroll);
        g_miniPlayerArtRef            = nullptr;
        g_miniPlayerTitleRef          = nullptr;
        g_miniPlayerArtistRef         = nullptr;
        g_miniPlayerPlayBtnRef        = nullptr;
        g_miniPlayerPrevBtnRef        = nullptr;
        g_miniPlayerNextBtnRef        = nullptr;
        g_miniPlayerShuffleBtnRef     = nullptr;
        g_miniPlayerRepeatBtnRef      = nullptr;
        g_miniPlayerSessionListRef    = nullptr;
        g_miniPlayerSessionListSepRef = nullptr;
    } catch (...) {
        g_playerGrid      = nullptr;
        g_lyricsOverlayHost = nullptr;
        g_lyricsViewport = nullptr;
        g_lyricsGuardClip = nullptr;
        g_lyricsTargetWidth = -1.0;
        g_lyricsTargetVisible = true;
        g_injectionParent = nullptr;
        g_playerColumn    = -1;
        g_playerToolTip   = nullptr;
        g_playerButtonStateUpdater = nullptr;
        g_cachedAlbumTitle.clear();
        g_cachedAlbumArtist.clear();
        g_cachedThumbnailBytes.clear();
        g_cachedPaletteHash = 0;
        g_blurBgCache.Invalidate();
        g_cachedAppIconSize = -1;
        ResetScrollState(g_titleScroll);
        ResetScrollState(g_artistScroll);
        g_miniPlayerArtRef            = nullptr;
        g_miniPlayerTitleRef          = nullptr;
        g_miniPlayerArtistRef         = nullptr;
        g_miniPlayerPlayBtnRef        = nullptr;
        g_miniPlayerPrevBtnRef        = nullptr;
        g_miniPlayerNextBtnRef        = nullptr;
        g_miniPlayerShuffleBtnRef     = nullptr;
        g_miniPlayerRepeatBtnRef      = nullptr;
        g_miniPlayerSessionListRef    = nullptr;
        g_miniPlayerSessionListSepRef = nullptr;
    }
}
// Smooth, monotone interpolation; none of these presets overshoots the
// destination or briefly crosses back into the album artwork.
static EasingFunctionBase MakeVizDockEase(const std::wstring& preset) {
    if (preset == L"linear") return nullptr;
    if (preset == L"silky" || preset == L"feather") {
        SineEase ease;
        ease.EasingMode(preset == L"feather" ? EasingMode::EaseOut : EasingMode::EaseInOut);
        return ease;
    }
    if (preset == L"gentle" || preset == L"buildup") {
        QuadraticEase ease;
        ease.EasingMode(preset == L"buildup" ? EasingMode::EaseIn : EasingMode::EaseInOut);
        return ease;
    }
    if (preset == L"dramatic") {
        QuinticEase ease;
        ease.EasingMode(EasingMode::EaseInOut);
        return ease;
    }
    if (preset == L"powerful") {
        QuarticEase ease;
        ease.EasingMode(EasingMode::EaseInOut);
        return ease;
    }
    if (preset == L"arc") {
        CircleEase ease;
        ease.EasingMode(EasingMode::EaseInOut);
        return ease;
    }
    if (preset == L"glide") {
        QuadraticEase ease;
        ease.EasingMode(EasingMode::EaseOut);
        return ease;
    }
    CubicEase ease;
    ease.EasingMode(preset == L"quick" ? EasingMode::EaseOut : EasingMode::EaseInOut);
    return ease;
}

// Lightweight song-change animations. These affect presentation only, never
// media playback, layout measurements, or the existing visualizer dock movement.
[[clang::no_destroy]] static std::vector<Storyboard> g_songChangeStoryboards;
static void StopSongChangeAnimations() {
    for (auto& sb : g_songChangeStoryboards) {
        try { if (sb) sb.Stop(); } catch (...) {}
    }
    g_songChangeStoryboards.clear();
}
static void AnimateSongChangeElement(FrameworkElement element) {
    if (!element || !g_settings.songChangeEnabled ||
        g_settings.songChangePreset == L"instant" || ReduceMotionForPower()) return;
    try {
        const auto& style = g_settings.songChangePreset;
        const double distance = (double)g_settings.songChangeTravel;
        const double startOpacity = g_settings.songChangeStartOpacity / 100.0;
        const auto duration = DurationHelper::FromTimeSpan(
            std::chrono::milliseconds(g_settings.songChangeDurationMs));
        auto ease = MakeVizDockEase(g_settings.songChangeEase);
        CompositeTransform transform;
        element.RenderTransform(transform);
        element.RenderTransformOrigin({0.5, 0.5});
        Storyboard sb;
        auto animate = [&](DependencyObject target, const wchar_t* property,
                           double from, double to) {
            if (fabs(from - to) < 0.0001) return;
            DoubleAnimation a;
            a.From(from);
            a.To(to);
            a.Duration(duration);
            if (ease) a.EasingFunction(ease);
            Storyboard::SetTarget(a, target);
            Storyboard::SetTargetProperty(a, property);
            sb.Children().Append(a);
        };
        animate(element, L"Opacity", startOpacity, 1.0);
        double x = 0, y = 0, scale = 1.0;
        if (style == L"cinematic") { y = distance * 0.50; scale = 0.93; }
        else if (style == L"slide_left") x = -distance;
        else if (style == L"slide_right") x = distance;
        else if (style == L"slide_up") y = distance;
        else if (style == L"slide_down") y = -distance;
        else if (style == L"zoom") scale = 0.86;
        if (style != L"fade") {
            animate(transform, L"TranslateX", x, 0.0);
            animate(transform, L"TranslateY", y, 0.0);
            animate(transform, L"ScaleX", scale, 1.0);
            animate(transform, L"ScaleY", scale, 1.0);
        }
        if (sb.Children().Size()) {
            sb.Begin();
            g_songChangeStoryboards.push_back(sb);
        }
    } catch (...) {
        // Animation failure must never hide content or affect playback.
        try { element.Opacity(1.0); element.RenderTransform(nullptr); } catch (...) {}
        Wh_Log(L"Song-change animation failed, restored normal appearance");
    }
}
// UI-thread-only adaptive layout. Reparent the visualizer only on a lyrics
// availability transition, not every frame or lyric syllable.
static void RefreshCompactMissingLyricsLayout() {
    if (!g_playerGrid || g_settings.lyricsOnly ||
        g_unloading || g_applyingSettings) return;
    bool lyricVisible = false, farDueSoon = false;
    {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        lyricVisible = g_lyricsDisplayIndex >= 0 &&
            g_lyricsDisplayIndex < (int)g_lyricsCues.size() &&
            !g_lyricsCues[g_lyricsDisplayIndex].text.empty();
        farDueSoon = g_lyricsFarDueSoon;
    }
    bool compact = g_settings.vizCompactEnabled &&
        (!g_settings.showLyrics || (!lyricVisible && !farDueSoon));
    if (VizPreviewWithoutPlayback()) {
        if (g_settings.vizPreviewMode == L"near") compact = true;
        else if (g_settings.vizPreviewMode == L"far") compact = false;
        else if (g_settings.vizPreviewMode == L"cycle") {
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - g_vizPreviewEpoch).count();
            compact = ((ms / 4000) % 2) == 0;
        }
    } else if (g_settings.previewLayout) {
        // Preserve the original global layout-preview behavior.
        compact = false;
    }
    if (compact == g_compactNoLyricsActive) return;
    try {
        auto panel = FindChildByName(g_playerGrid, kPanelGridName).try_as<Grid>();
        if (!panel) return;
        auto lyrics = FindChildByName(panel, L"FluentLyrics_Embedded");
        auto viz = FindChildByName(panel, L"FluentMedia_Visualizer").try_as<FrameworkElement>();
        auto dock = FindChildByName(panel, L"FluentMedia_AlbumVizDock").try_as<StackPanel>();
        // The near layout requires a genuine album-art container. Otherwise
        // keep the visualizer at its safe original slot, with no orphan views.
        if (!viz || !dock || g_vizNormalColumn < 0) compact = false;
        double oldVizX = 0.0;
        bool haveOldVizX = false;
        if (compact != g_compactNoLyricsActive && viz && dock && g_vizNormalColumn >= 0) {
            // Record on-screen X before reparenting. The source and destination
            // are measured relative to the SAME panel, regardless of column width.
            try {
                const auto point = viz.TransformToVisual(panel).TransformPoint({0.0f, 0.0f});
                oldVizX = point.X;
                haveOldVizX = std::isfinite(oldVizX);
            } catch (...) {}
            if (g_vizDockStoryboard) {
                // Capture the current visual position above, *before* canceling
                // an unfinished animation. A rapid lyric change can now reverse
                // direction without jumping back to the last fixed position.
                try { g_vizDockStoryboard.Stop(); } catch (...) {}
                g_vizDockStoryboard = nullptr;
            }
            // Reparent in one UI-thread operation; the new storyboard starts
            // at the captured screen coordinate after UpdateLayout.
            if (auto previousShift = viz.RenderTransform().try_as<TranslateTransform>())
                previousShift.X(0.0);
            auto vizElement = viz.as<UIElement>();
            if (compact) {
                auto children = panel.Children();
                bool removed = false;
                for (uint32_t i = 0; i < children.Size(); ++i) {
                    if (children.GetAt(i) == vizElement) {
                        children.RemoveAt(i);
                        removed = true;
                        break;
                    }
                }
                if (!removed) return;
                try { dock.Children().Append(vizElement); }
                catch (...) {
                    Grid::SetColumn(viz, g_vizNormalColumn);
                    panel.Children().Append(vizElement);
                    throw;
                }
            } else {
                auto children = dock.Children();
                bool removed = false;
                for (uint32_t i = 0; i < children.Size(); ++i) {
                    if (children.GetAt(i) == vizElement) {
                        children.RemoveAt(i);
                        removed = true;
                        break;
                    }
                }
                if (!removed) return;
                Grid::SetColumn(viz, g_vizNormalColumn);
                try { panel.Children().Append(vizElement); }
                catch (...) { dock.Children().Append(vizElement); throw; }
            }
            // Reparenting changes real layout, never paints into the artwork.
            // Keep far margins *exactly* as the previous version used them.
            const bool vizLeft = (g_settings.vizPosition == L"left") != g_settings.mirrorLayout;
            const double gap = (double)g_settings.vizSectionGap;
            const double ml = compact ?
                (double)(g_settings.vizCompactGap +
                         std::max(0, g_settings.vizCompactHorizontalOffset)) :
                (double)(g_settings.vizPadLeft + (vizLeft ? 0.0 : gap) +
                         std::max(0, g_settings.vizHorizontalOffset));
            const double mr = compact ?
                (double)std::max(0, -g_settings.vizCompactHorizontalOffset) :
                (double)(g_settings.vizPadRight + (vizLeft ? gap : 0.0) +
                         std::max(0, -g_settings.vizHorizontalOffset));
            viz.Margin({ml, 0, mr, 0});
            if (auto tr = viz.RenderTransform().try_as<TranslateTransform>())
                tr.Y((double)(compact ? g_settings.vizCompactVerticalOffset :
                                           g_settings.vizVerticalOffset));
            // Run the transition after lyrics visibility and layout settle.
            // Otherwise the destination coordinate is still the old column.
        }
        // Lyrics are shown only when a cue is active, except the pre-cue
        // reservation (the empty area is prepared 500ms ahead of the words).
        const bool previewFar = VizPreviewWithoutPlayback() &&
            (g_settings.vizPreviewMode == L"far" ||
             (g_settings.vizPreviewMode == L"cycle" && !compact));
        if (lyrics)
            lyrics.Visibility((compact && !previewFar) ? Visibility::Collapsed : Visibility::Visible);
        g_compactNoLyricsActive = compact;
        if (g_lyricsOverlayHost) {
            g_playerGrid.ClearValue(FrameworkElement::WidthProperty());
            g_playerGrid.Measure({std::numeric_limits<float>::infinity(),
                                  (float)std::max(24, g_settings.playerMaxHeight)});
            g_playerOverlayWidth = std::max(1.0, (double)g_playerGrid.DesiredSize().Width);
            g_lyricsLastLayoutTime = {};
        }
        if (viz && dock && g_vizNormalColumn >= 0) {
            // Layout is synchronous on Explorer's UI thread, with no polling
            // and no high-frequency timer. This is only called on dock changes.
            viz.Opacity(1.0);
            if (auto shift = viz.RenderTransform().try_as<TranslateTransform>()) {
                shift.X(0.0);
                if (g_settings.vizMoveAnimationMs > 0 && !ReduceMotionForPower() &&
                    g_settings.vizTransitionStyle != L"instant") {
                    try {
                        panel.UpdateLayout();
                        const auto target = viz.TransformToVisual(panel).TransformPoint({0.0f, 0.0f});
                        double dx = (haveOldVizX && std::isfinite((double)target.X)) ?
                                    oldVizX - (double)target.X : 0.0;
                        // If artwork lies between the two positions (e.g. mirrored
                        // and far-left layouts), slide locally at the destination
                        // rather than sweeping directly across album artwork.
                        if (g_settings.vizTransitionStyle == L"slide" && std::abs(dx) > 0.5) {
                            try {
                                if (dock.Children().Size() > 0) {
                                    auto art = dock.Children().GetAt(0).try_as<FrameworkElement>();
                                    if (art) {
                                        auto artP = art.TransformToVisual(panel).TransformPoint({0.0f,0.0f});
                                        const double artL = artP.X;
                                        const double artR = artL + art.ActualWidth();
                                        const double destX = target.X;
                                        const double startX = oldVizX;
                                        if (std::min(startX, destX) < artR &&
                                            std::max(startX, destX) + viz.ActualWidth() > artL) {
                                            // Approach the destination from its own
                                            // artwork-safe side, never across the cover.
                                            if (destX >= artR + 1.0) dx = 18.0;
                                            else if (destX + viz.ActualWidth() <= artL - 1.0) dx = -18.0;
                                            else dx = 0.0; // Fade if destination geometry is ambiguous.
                                        }
                                    }
                                }
                            } catch (...) { dx = std::clamp(dx, -18.0, 18.0); }
                        }
                        const auto duration = DurationHelper::FromTimeSpan(
                            std::chrono::milliseconds(g_settings.vizMoveAnimationMs));
                        Storyboard sb;
                        const std::wstring& selectedEase = compact ?
                            (g_settings.vizFarToNearEase == L"match" ? g_settings.vizNearToFarEase
                                                                      : g_settings.vizFarToNearEase) :
                            g_settings.vizNearToFarEase;
                        auto ease = MakeVizDockEase(selectedEase);
                        bool hasMovement = false;
                        if (g_settings.vizTransitionStyle == L"slide" &&
                            std::isfinite(dx) && std::abs(dx) >= 0.5 && std::abs(dx) <= 2000.0) {
                            // Render from the *previous screen position* all
                            // the way to the newly allocated destination. Unlike
                            // the earlier fade/short-shift mode, bars stay fully
                            // opaque and visually travel the whole distance.
                            shift.X(dx);
                            viz.Opacity(1.0);
                            DoubleAnimation move;
                            move.From(dx);
                            move.To(0.0);
                            move.Duration(duration);
                            if (ease) move.EasingFunction(ease);
                            // RenderTransform.X is animated on the UI thread;
                            // no 16-ms custom movement timer is needed.
                            Storyboard::SetTarget(move, shift);
                            Storyboard::SetTargetProperty(move, L"X");
                            sb.Children().Append(move);
                            hasMovement = true;
                        }
                        if (!hasMovement) {
                            // Respect the existing Fade style, as well as a
                            // safe fallback when source/target cannot be measured.
                            DoubleAnimation fade;
                            fade.From(g_settings.vizTransitionStyle == L"fade" ? 0.0 : 1.0);
                            fade.To(1.0);
                            fade.Duration(duration);
                            if (ease) fade.EasingFunction(ease);
                            Storyboard::SetTarget(fade, viz);
                            Storyboard::SetTargetProperty(fade, L"Opacity");
                            sb.Children().Append(fade);
                        }
                        g_vizDockStoryboard = sb;
                        sb.Begin();
                    } catch (...) {
                        shift.X(0.0);
                        viz.Opacity(1.0);
                        Wh_Log(L"Visualizer dock slide failed; restored final location");
                    }
                }
            }
        }
    } catch (...) { Wh_Log(L"Compact lyrics layout transition failed"); }
}
static void RefreshPlayerContents() {
    if (!g_playerGrid || g_unloading || g_applyingSettings) return;
    if (g_resetScrollPending.exchange(false)) {
        ResetScrollState(g_titleScroll);
        ResetScrollState(g_artistScroll);
        g_scrollCachedTitle.clear();
        g_scrollCachedArtist.clear();
    }
    RefreshLyricsOnlyUi();
    if (g_settings.lyricsOnly) return;
    RefreshCompactMissingLyricsLayout();
    std::wstring      title, artist;
    bool              isPlaying = false, hasMedia = false;
    std::vector<BYTE> thumbBytes;
    std::vector<BYTE> appIconBytes;
    uint64_t          thumbHash = 0;
    bool              canPlayPause = false;
    bool              canSkipPrevious = true, canSkipNext = true;
    bool              canShuffle = true, canRepeat = true, canSeek = true;
    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        title        = g_media.title;
        artist       = g_media.artist;
        isPlaying    = g_media.isPlaying;
        hasMedia     = g_media.hasMedia;
        thumbBytes   = g_media.thumbnailBytes;
        thumbHash    = g_media.thumbnailHash;
        appIconBytes = g_media.appIconBytes;
        canPlayPause    = g_media.canPlayPause;
        canSkipPrevious = g_media.canSkipPrevious;
        canSkipNext     = g_media.canSkipNext;
        canShuffle      = g_media.canShuffle;
        canRepeat       = g_media.canRepeat;
        canSeek         = g_media.canSeek;
    }
    if (g_settings.previewLayout && title.empty() && artist.empty()) {
        title = L"Layout preview";
        artist = L"Album art · Controls · Lyrics";
    }
    (void)hasMedia;
    g_albumArtworkPlaying = isPlaying;
    bool hasSession = false;
    { std::lock_guard<std::mutex> lk(g_sessionMtx); hasSession = (g_currentSession != nullptr); }
    g_playerGrid.UpdateLayout();
    if (title != g_scrollCachedTitle || artist != g_scrollCachedArtist) {
        g_scrollCachedTitle  = title;
        g_scrollCachedArtist = artist;
        ResetScrollState(g_titleScroll);
        ResetScrollState(g_artistScroll);
        try {
            if (auto fe = FindChildByName(g_playerGrid, kTitleCloneName))
                if (auto cl = fe.try_as<TextBlock>())
                    cl.Visibility(Visibility::Collapsed);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(g_playerGrid, kArtistCloneName))
                if (auto cl = fe.try_as<TextBlock>())
                    cl.Visibility(Visibility::Collapsed);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(g_playerGrid, kTitleBlockName))
                if (auto tb = fe.try_as<TextBlock>())
                    Canvas::SetLeft(tb, 0.0);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(g_playerGrid, kArtistBlockName))
                if (auto ab = fe.try_as<TextBlock>())
                    Canvas::SetLeft(ab, 0.0);
        } catch (...) {}
    }
    bool titleVisible = false;
    bool artistVisible = false;
    if (auto fe = FindChildByName(g_playerGrid, kTitleBlockName))
        if (auto tb = fe.try_as<TextBlock>())
            try {
                std::wstring displayTitle = title;
                if (!hasSession && !g_settings.previewLayout) {
                    displayTitle = g_settings.noMediaTitleText;
                } else if (title.empty()) {
                    displayTitle = g_settings.emptyTitleText;
                }
                tb.Text(winrt::hstring(displayTitle));
                tb.Foreground(MakeBrush(TextColor()));
                bool visible = g_settings.showTrackTitle && !displayTitle.empty();
                titleVisible = visible;
                tb.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
                ToolTipService::SetToolTip(tb, nullptr);
                if (g_settings.enableTitleScrolling && visible) {
                    try {
                        if (auto viewFe = FindChildByName(g_playerGrid, kTitleScrollViewName))
                            viewFe.Visibility(Visibility::Visible);
                    } catch (...) {}
                    if (auto panelFe = FindChildByName(g_playerGrid, kPanelGridName)) {
                        panelFe.UpdateLayout();
                    }
                    tb.UpdateLayout();
                    double textW = tb.DesiredSize().Width;
                    if (auto viewFe = FindChildByName(g_playerGrid, kTitleScrollViewName)) {
                        if (auto viewCanvas = viewFe.try_as<Canvas>()) {
                            double minW = (double)g_settings.textAreaMinWidth;
                            double maxW = (double)g_settings.textAreaMaxWidth;
                            double viewW = textW;
                            if (maxW > 0 && viewW > maxW) viewW = maxW;
                            double availW = GetAvailableScrollTextAreaWidth();
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
                            bool wasActive = g_titleScroll.active;
                            g_titleScroll.textWidth = textW;
                            g_titleScroll.viewWidth = viewW;
                            g_titleScroll.active = (textW > viewW + 2.0);
                            if (!g_titleScroll.active) {
                                g_titleScroll.offset = 0.0;
                                g_titleScroll.forward = true;
                                Canvas::SetLeft(tb, 0.0);
                            } else if (!wasActive) {
                                g_titleScroll.offset = 0.0;
                                g_titleScroll.forward = true;
                                if (g_settings.scrollMode == L"loop") {
                                    g_titleScroll.pausing  = false;
                                    g_titleScroll.pauseTick = 0;
                                } else {
                                    g_titleScroll.pausing  = true;
                                    g_titleScroll.pauseTick = g_settings.scrollPauseDuration;
                                }
                            }
                            if (auto cloneFe = FindChildByName(g_playerGrid, kTitleCloneName)) {
                                if (auto clone = cloneFe.try_as<TextBlock>()) {
                                    clone.Text(tb.Text());
                                    clone.Foreground(tb.Foreground());
                                    clone.Visibility(g_settings.scrollMode == L"loop" && g_titleScroll.active
                                        ? Visibility::Visible : Visibility::Collapsed);
                                }
                            }
                        }
                    }
                } else {
                    g_titleScroll.active = false;
                    g_titleScroll.offset = 0.0;
                    if (g_settings.enableTitleScrolling) {
                        try {
                            if (auto viewFe = FindChildByName(g_playerGrid, kTitleScrollViewName))
                                viewFe.Visibility(Visibility::Collapsed);
                        } catch (...) {}
                    }
                }
            } catch (...) {}
    if (auto fe = FindChildByName(g_playerGrid, kArtistBlockName))
        if (auto ab = fe.try_as<TextBlock>())
            try {
                std::wstring displayArtist = artist;
                if (!hasSession && !g_settings.previewLayout) {
                    displayArtist = g_settings.noMediaArtistText;
                } else if (artist.empty()) {
                    displayArtist = g_settings.emptyArtistText;
                }
                ab.Text(winrt::hstring(displayArtist));
                bool visible = g_settings.showTrackArtist && !displayArtist.empty();
                artistVisible = visible;
                ab.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
                ab.Foreground(MakeBrush(ArtistColor()));
                ToolTipService::SetToolTip(ab, nullptr);
                if (g_settings.enableArtistScrolling && visible) {
                    try {
                        if (auto viewFe = FindChildByName(g_playerGrid, kArtistScrollViewName))
                            viewFe.Visibility(Visibility::Visible);
                    } catch (...) {}
                    if (auto panelFe = FindChildByName(g_playerGrid, kPanelGridName)) {
                        panelFe.UpdateLayout();
                    }
                    ab.UpdateLayout();
                    double textW = ab.DesiredSize().Width;
                    if (auto viewFe = FindChildByName(g_playerGrid, kArtistScrollViewName)) {
                        if (auto viewCanvas = viewFe.try_as<Canvas>()) {
                            double minW = (double)g_settings.textAreaMinWidth;
                            double maxW = (double)g_settings.textAreaMaxWidth;
                            double viewW = textW;
                            if (maxW > 0 && viewW > maxW) viewW = maxW;
                            double availW = GetAvailableScrollTextAreaWidth();
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
                            bool wasActive = g_artistScroll.active;
                            g_artistScroll.textWidth = textW;
                            g_artistScroll.viewWidth = viewW;
                            g_artistScroll.active = (textW > viewW + 2.0);
                            if (!g_artistScroll.active) {
                                g_artistScroll.offset = 0.0;
                                g_artistScroll.forward = true;
                                Canvas::SetLeft(ab, 0.0);
                            } else if (!wasActive) {
                                g_artistScroll.offset = 0.0;
                                g_artistScroll.forward = true;
                                if (g_settings.scrollMode == L"loop") {
                                    g_artistScroll.pausing  = false;
                                    g_artistScroll.pauseTick = 0;
                                } else {
                                    g_artistScroll.pausing  = true;
                                    g_artistScroll.pauseTick = g_settings.scrollPauseDuration;
                                }
                            }
                            if (auto cloneFe = FindChildByName(g_playerGrid, kArtistCloneName)) {
                                if (auto clone = cloneFe.try_as<TextBlock>()) {
                                    clone.Text(ab.Text());
                                    clone.Foreground(ab.Foreground());
                                    clone.Visibility(g_settings.scrollMode == L"loop" && g_artistScroll.active
                                        ? Visibility::Visible : Visibility::Collapsed);
                                }
                            }
                        }
                    }
                } else {
                    g_artistScroll.active = false;
                    g_artistScroll.offset = 0.0;
                    if (g_settings.enableArtistScrolling) {
                        try {
                            if (auto viewFe = FindChildByName(g_playerGrid, kArtistScrollViewName))
                                viewFe.Visibility(Visibility::Collapsed);
                        } catch (...) {}
                    }
                }
            } catch (...) {}
    if (!ReduceMotionForPower() && (g_titleScroll.active || g_artistScroll.active) &&
        (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling)) {
        if (g_scrollDispatcherTimer && !g_scrollDispatcherTimer.IsEnabled()) {
            try { g_scrollDispatcherTimer.Start(); } catch (...) {}
        }
    }
    try {
        if (auto stackFe = FindChildByName(g_playerGrid, kTextStackName)) {
            bool anyTextVisible = titleVisible || artistVisible;
            stackFe.Visibility(anyTextVisible ? Visibility::Visible : Visibility::Collapsed);
        }
    } catch (...) {}
    if (auto fe = FindChildByName(g_playerGrid, kPlayBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                btn.IsEnabled(hasSession && canPlayPause);
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(2, isPlaying);
                    ct.Text(winrt::hstring(glyph));
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}
    if (auto fe = FindChildByName(g_playerGrid, kPrevBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = hasSession && canSkipPrevious;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(1);
                    ct.Text(winrt::hstring(glyph));
                    if (!supported && !g_settings.hideUnsupportedButtons) {
                        ct.Opacity(0.35);
                        ct.Foreground(MakeBrush(ButtonColor()));
                    } else {
                        ct.Opacity(1.0);
                        ct.Foreground(MakeBrush(ButtonColor()));
                    }
                }
            } catch (...) {}
    if (auto fe = FindChildByName(g_playerGrid, kNextBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = hasSession && canSkipNext;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
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
    if (auto fe = FindChildByName(g_playerGrid, kRewindBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = hasSession && canSeek;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
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
    if (auto fe = FindChildByName(g_playerGrid, kForwardBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = hasSession && canSeek;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
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
    if (auto fe = FindChildByName(g_playerGrid, kShuffleBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = hasSession && canShuffle;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    bool isEnabled = g_shuffleEnabled.load();
                    const wchar_t* glyph = L"";
                    ct.Text(winrt::hstring(glyph));
                    if (!supported && !g_settings.hideUnsupportedButtons) {
                        ct.Opacity(0.35);
                    } else {
                        ct.Opacity(isEnabled ? 1.0 : 0.4);
                    }
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}
    if (auto fe = FindChildByName(g_playerGrid, kRepeatBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = hasSession && canRepeat;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    RepeatMode mode = g_repeatMode.load();
                    const wchar_t* glyph;
                    switch (mode) {
                        case RepeatMode::Off:
                            glyph = L"";
                            break;
                        case RepeatMode::All:
                            glyph = L"";
                            break;
                        case RepeatMode::One:
                            glyph = L"";
                            break;
                    }
                    ct.Text(winrt::hstring(glyph));
                    ct.Foreground(MakeBrush(ButtonColor()));
                    if (!supported && !g_settings.hideUnsupportedButtons) {
                        ct.Opacity(0.35);
                    } else {
                        ct.Opacity(1.0);
                    }
                }
            } catch (...) {}
    if (auto fe = FindChildByName(g_playerGrid, kSwitchSessionsBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = g_sessionCount.load() > 1;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    ct.Opacity(supported ? 1.0 : 0.35);
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}
    if (g_settings.albumArtPlaybackButton && g_settings.showAlbumArt) {
        if (auto fe = FindChildByName(g_playerGrid, L"FluentMedia_ArtPlaybackButton")) {
            if (auto button = fe.try_as<Button>()) {
                try {
                    const bool doubleEnabled = DoubleClickEnabled(L"album_art");
                    const bool otherAction = doubleEnabled && g_settings.doubleClickAction != L"play_pause";
                    button.IsEnabled((hasSession && canPlayPause) ||
                        (otherAction && (hasSession || (g_settings.doubleClickAction == L"open_mini_player" || g_settings.doubleClickAction == L"next_monitor"))));
                    std::wstring label = isPlaying ? L"Pause" : L"Play";
                    if (doubleEnabled) {
                        label += L"; double-click: ";
                        label += g_settings.doubleClickAction == L"open_app" ? L"open music app" :
                            g_settings.doubleClickAction == L"open_mini_player" ? L"open expanded player" :
                            g_settings.doubleClickAction == L"next_monitor" ? L"move to next monitor" : L"play / pause";
                    }
                    Automation::AutomationProperties::SetName(button, label);
                    ToolTipService::SetToolTip(button, winrt::box_value(label));
                    if (auto icon = button.Content().try_as<TextBlock>())
                        icon.Text(GetGlyph(2, isPlaying));
                } catch (...) { Wh_Log(L"Artwork playback button update failed"); }
            }
        }
    }
    if (!g_settings.albumArtPlaybackButton && g_settings.showPauseOverlay && g_settings.showAlbumArt) {
        if (auto fe = FindChildByName(g_playerGrid, L"PauseIconOverlay"))
            if (auto overlay = fe.try_as<Border>()) {
                try {
                    bool showPause = !isPlaying && g_albumArtworkHovered;
                    overlay.Visibility(showPause ? Visibility::Visible : Visibility::Collapsed);
                    if (auto pauseIcon = overlay.Child().try_as<TextBlock>()) {
                        pauseIcon.Text(GetGlyph(2, true));
                        bool useFluent = (g_settings.iconStyle == L"fluent_outline" || g_settings.iconStyle == L"fluent_filled");
                        pauseIcon.FontFamily(Media::FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                        pauseIcon.FontSize((double)g_settings.pauseOverlayIconSize);
                    }
                    if (showPause) {
                        if (auto artImg = FindChildByName(g_playerGrid, kArtImageName)) {
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
    bool artChangedThisUpdate = false;
    if (auto fe = FindChildByName(g_playerGrid, kArtImageName))
        if (auto img = fe.try_as<Controls::Image>()) {
            if (!thumbBytes.empty() && g_settings.showAlbumArt) {
                bool isSameAlbum = (!g_cachedThumbnailBytes.empty() &&
                                title == g_cachedAlbumTitle &&
                                artist == g_cachedAlbumArtist &&
                                thumbBytes == g_cachedThumbnailBytes);
                size_t newHash = (size_t)thumbHash;
                if (newHash != g_cachedPaletteHash && newHash != 0) {
                    g_songColorFrom = g_cachedAlbumPalette;
                    g_cachedAlbumPalette = ExtractAlbumPalette(thumbBytes);
                    g_songColorTransitionAt = std::chrono::steady_clock::now();
                    g_songColorTransitionActive = g_settings.songChangeEnabled &&
                        g_settings.songChangeVizColor;
                    g_cachedPaletteHash = newHash;
                    g_vizPaletteColorsDirty = true;
                    paletteChanged = true;
                }
                if (!isSameAlbum) {
                    try {
                        IStream* pRawStream = SHCreateMemStream(
                            thumbBytes.data(), static_cast<UINT>(thumbBytes.size()));
                        if (pRawStream) {
                            winrt::com_ptr<IStream> comStream;
                            comStream.attach(pRawStream);
                            winrt::Windows::Storage::Streams::IRandomAccessStream rasStream{ nullptr };
                            ::CreateRandomAccessStreamOverStream(
                                comStream.get(),
                                BSOS_DEFAULT,
                                winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(),
                                winrt::put_abi(rasStream));
                            if (rasStream) {
                                BitmapImage bmp;
                                if (g_settings.albumArtQuality == L"low") {
                                    int baseHeight = g_settings.albumArtMaxHeight > 0 ? g_settings.albumArtMaxHeight : 64;
                                    int decodeHeight = baseHeight / 2;
                                    if (decodeHeight < 16) decodeHeight = 16;
                                    bmp.DecodePixelHeight(decodeHeight);
                                } else {
                                    const int height = g_settings.albumArtMaxHeight > 0 ? g_settings.albumArtMaxHeight : 64;
                                    // Medium covers 200% DPI; high covers 300%, with a bounded decode.
                                    const int scale = g_settings.albumArtQuality == L"medium" ? 2 : 3;
                                    bmp.DecodePixelHeight(std::clamp(height * scale, 32, 512));
                                }
                                bmp.ImageOpened([](auto const&, auto const&) {
                                    if (g_unloading || g_applyingSettings || !g_playerGrid) return;
                                    try {
                                        if (auto panelFe = FindChildByName(g_playerGrid, kPanelGridName)) {
                                            panelFe.UpdateLayout();
                                        }
                                        g_needsUiUpdate = true;
                                        if (g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
                                    } catch (...) {}
                                });
                                img.Source(bmp);
                                artChangedThisUpdate = true;
                                bmp.SetSourceAsync(rasStream);
                                img.Visibility(Visibility::Visible);
                                g_cachedAlbumTitle = title;
                                g_cachedAlbumArtist = artist;
                                g_cachedThumbnailBytes = thumbBytes;
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
                    } catch (...) { try { img.Source(nullptr); } catch (...) {} }
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
                if (auto bgFe = FindChildByName(g_playerGrid, L"FluentMedia_Background")) {
                    if (auto bgBorder = bgFe.try_as<Border>()) {
                        auto& bgType = g_settings.backgroundType;
                        if (bgType == L"album_art_blur") {
                            try {
                                g_blurBgCache.Invalidate();
                                bgBorder.Visibility(Visibility::Visible);
                                bgBorder.Opacity(g_settings.blurOpacity / 100.0);
                                auto applyBlur = [bgBorder, thumbBytesSnap = thumbBytes]() {
                                    try {
                                        int w = (int)bgBorder.ActualWidth();
                                        int h = (int)bgBorder.ActualHeight();
                                        if (w <= 0 || h <= 0) return;
                                        g_blurBgCache.Invalidate();
                                        bgBorder.Background(MakeAlbumBlurBrush(thumbBytesSnap, w, h));
                                        bgBorder.Opacity(g_settings.blurOpacity / 100.0);
                                        bgBorder.Visibility(Visibility::Visible);
                                    } catch (...) {}
                                };
                                if (bgBorder.ActualWidth() > 0 && bgBorder.ActualHeight() > 0) {
                                    applyBlur();
                                } else {
                                    auto tokenHolder = std::make_shared<winrt::event_token>();
                                    *tokenHolder = bgBorder.SizeChanged(
                                        [applyBlur, bgBorder, tokenHolder](auto const&, auto const&) mutable {
                                            applyBlur();
                                            try { bgBorder.SizeChanged(*tokenHolder); } catch (...) {}
                                        });
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
                g_cachedAlbumTitle.clear();
                g_cachedAlbumArtist.clear();
                g_cachedThumbnailBytes.clear();
                g_cachedPaletteHash = 0;
                g_blurBgCache.Invalidate();
                if (auto bgFe = FindChildByName(g_playerGrid, L"FluentMedia_Background")) {
                    if (auto bgBorder = bgFe.try_as<Border>()) {
                        try {
                            auto& bgType = g_settings.backgroundType;
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
                    if (!g_settings.previewLayout && !VizPreviewWithoutPlayback() &&
                        g_settings.albumArtEmptyBehavior == L"hide" && thumbBytes.empty()) {
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
                    } else if ((g_settings.previewLayout || VizPreviewWithoutPlayback() ||
                                g_settings.albumArtEmptyBehavior == L"show_icon") && thumbBytes.empty()) {
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
                                        unsigned long cp = std::stoul(g_settings.emptyIconGlyph, nullptr, 16);
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
                                    bool useFluent = (g_settings.emptyIconFont == L"segoe_fluent");
                                    textBlock.FontFamily(Media::FontFamily(
                                        useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                                    textBlock.FontSize((double)g_settings.emptyIconSize);
                                    BYTE alpha = (BYTE)std::clamp((int)std::round(g_settings.emptyIconOpacity * 255.0 / 100.0), 0, 255);
                                    auto iconClr = ParseColorWithThemeSupport(g_settings.emptyIconColor, alpha);
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
            if (g_settings.backgroundType == L"gradient" ||
                g_settings.backgroundType == L"solid" ||
                g_settings.backgroundType == L"acrylic" ||
                g_settings.backgroundType == L"mica" ||
                g_settings.backgroundType == L"mica_alt") {
                if (auto bgFe = FindChildByName(g_playerGrid, L"FluentMedia_Background")) {
                    if (auto bgBorder = bgFe.try_as<Border>()) {
                        bgBorder.Background(MakeBackgroundBrush());
                    }
                }
            }
            auto textClr = TextColor();
            auto artistClr = ArtistColor();
            if (auto titleFe = FindChildByName(g_playerGrid, kTitleBlockName)) {
                if (auto titleBlock = titleFe.try_as<TextBlock>()) {
                    titleBlock.Foreground(SolidColorBrush(textClr));
                }
            }
            if (auto artistFe = FindChildByName(g_playerGrid, kArtistBlockName)) {
                if (auto artistBlock = artistFe.try_as<TextBlock>()) {
                    artistBlock.Foreground(SolidColorBrush(artistClr));
                }
            }
            auto buttonClr = ButtonColor();
            for (const auto& btnName : {kPlayBtnName, kPrevBtnName, kNextBtnName,
                                        kRewindBtnName, kForwardBtnName, kShuffleBtnName, kRepeatBtnName}) {
                if (auto btnFe = FindChildByName(g_playerGrid, btnName)) {
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
    if (g_settings.showAppIcon) {
        if (auto fe = FindChildByName(g_playerGrid, kAppIconImageName))
            if (auto img = fe.try_as<Controls::Image>()) {
                bool sizeChanged = (g_cachedAppIconSize != g_settings.appIconSize);
                if (sizeChanged && !appIconBytes.empty()) {
                    g_cachedAppIconSize = g_settings.appIconSize;
                    FetchMediaPropertiesAsync();
                }
                if (!appIconBytes.empty()) {
                    try {
                        int iconSz = g_settings.appIconSize;
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
    try {
        FrameworkElement tooltipHost = g_playerGrid;
        if (g_settings.showFullTitleOnHover && hasSession && (!title.empty() || !artist.empty())) {
            std::wstring tooltipText;
            if (!title.empty())  tooltipText += title;
            if (!title.empty() && !artist.empty()) tooltipText += L"\n\n";
            if (!artist.empty()) tooltipText += artist;
            if (g_playerToolTip) {
                try {
                    auto content = g_playerToolTip.Content();
                    if (auto ttText = content.try_as<TextBlock>()) {
                        ttText.Text(winrt::hstring(tooltipText));
                    }
                    g_playerToolTip.IsEnabled(true);
                } catch (...) {}
                if (tooltipHost) {
                    try { ToolTipService::SetToolTip(tooltipHost, g_playerToolTip); } catch (...) {}
                }
            }
        } else {
            if (g_playerToolTip) {
                try {
                    auto content = g_playerToolTip.Content();
                    if (auto ttText = content.try_as<TextBlock>()) {
                        ttText.Text(L"");
                    }
                    g_playerToolTip.IsEnabled(false);
                } catch (...) {}
            }

            if (tooltipHost) {
                try { ToolTipService::SetToolTip(tooltipHost, nullptr); } catch (...) {}
            }
        }
    } catch (...) {}
    if (g_lyricsOverlayHost && !g_settings.lyricsOnly) {
        try {
            g_playerGrid.ClearValue(FrameworkElement::WidthProperty());
            g_playerGrid.Measure({std::numeric_limits<float>::infinity(), (float)std::max(24, g_settings.playerMaxHeight)});
            g_playerOverlayWidth = std::max(1.0, (double)g_playerGrid.DesiredSize().Width);
        } catch (...) {}
    }
    // Trigger only on song metadata changes, never on play/pause or bar frames.
    const std::wstring songIdentity = hasSession ? title + L"\x1f" + artist : L"";
    if (songIdentity != g_lastAnimatedTrack) {
        const bool hadPreviousTrack = !g_lastAnimatedTrack.empty();
        StopSongChangeAnimations();
        g_lastAnimatedTrack = songIdentity;
        if (hadPreviousTrack && !songIdentity.empty() && g_settings.songChangeText) {
            if (auto titleFe = FindChildByName(g_playerGrid, kTitleBlockName))
                AnimateSongChangeElement(titleFe);
            if (auto artistFe = FindChildByName(g_playerGrid, kArtistBlockName))
                AnimateSongChangeElement(artistFe);
        }
    }
    if (artChangedThisUpdate && g_settings.songChangeCover && !songIdentity.empty()) {
        if (auto artFe = FindChildByName(g_playerGrid, kArtImageName))
            AnimateSongChangeElement(artFe);
    }
    RefreshMiniPlayerFlyoutUI();
}
static bool IsFullscreenActive() {
    // Older SHQueryUserNotificationState-like checks can report a busy or
    // presentation shell state even when two ordinary windows are snapped.
    // Instead inspect the foreground application's visible frame on this
    // taskbar's monitor. Split windows can never pass all four edge checks.
    HWND foreground = GetForegroundWindow();
    if (!foreground || !IsWindow(foreground) || IsIconic(foreground)) return false;
    foreground = GetAncestor(foreground, GA_ROOT);
    if (!foreground || foreground == g_taskbarWnd) return false;
    wchar_t cls[96]{};
    GetClassNameW(foreground, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
        wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0)
        return false;
    HMONITOR playerMonitor = g_taskbarWnd
        ? MonitorFromWindow(g_taskbarWnd, MONITOR_DEFAULTTONEAREST) : nullptr;
    HMONITOR foregroundMonitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
    if (!playerMonitor || playerMonitor != foregroundMonitor) return false;
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(playerMonitor, &info)) return false;
    RECT bounds{};
    HRESULT dwm = DwmGetWindowAttribute(foreground, DWMWA_EXTENDED_FRAME_BOUNDS,
                                         &bounds, sizeof(bounds));
    if (FAILED(dwm) && !GetWindowRect(foreground, &bounds)) return false;
    constexpr LONG tolerance = 3;
    const bool coversMonitor = bounds.left <= info.rcMonitor.left + tolerance &&
        bounds.top <= info.rcMonitor.top + tolerance &&
        bounds.right >= info.rcMonitor.right - tolerance &&
        bounds.bottom >= info.rcMonitor.bottom - tolerance;
    if (!coversMonitor) return false;
    // Maximized framed windows with auto-hide taskbars sometimes occupy the
    // complete screen without being F11/borderless fullscreen.
    LONG_PTR style = GetWindowLongPtrW(foreground, GWL_STYLE);
    if (IsZoomed(foreground) && (style & WS_CAPTION) && (style & WS_THICKFRAME))
        return false;
    return true;
}
static void UpdateVisibility() {
    if (!g_playerGrid || g_unloading || g_applyingSettings) return;
    bool hasSession = false, hasMedia = false;
    { std::lock_guard<std::mutex> lk(g_sessionMtx); hasSession = (g_currentSession != nullptr); }
    { std::lock_guard<std::mutex> lk(g_mediaMtx); hasMedia = g_media.hasMedia; }
    const auto now = std::chrono::steady_clock::now();
    if (hasMedia) g_lastMediaTime = now;
    bool hide = (g_settings.hideFullscreen && !g_settings.allowInFullscreen && IsFullscreenActive()) || g_hiddenByIdle;
    if (g_settings.previewLayout || (g_settings.vizEnabled &&
        g_settings.vizPreviewMode != L"off")) hide = false;
    else if (g_settings.hideWhenNoMedia && !g_miniPlayerFlyoutOpen) {
        hide = hide || !hasSession || (!hasMedia && now - g_lastMediaTime > std::chrono::milliseconds(2500));
    }
    try {
        const auto visibility = hide ? Visibility::Collapsed : Visibility::Visible;
        if (g_playerGrid.Visibility() != visibility) g_playerGrid.Visibility(visibility);
        // A collapsed auto column releases its space without destroying min/max constraints.
        if (g_playerColumn >= 0 && g_injectionParent) {
            auto grid = g_injectionParent.try_as<Grid>();
            if (grid && g_playerColumn < (int)grid.ColumnDefinitions().Size()) {
                auto col = grid.ColumnDefinitions().GetAt(g_playerColumn);
                const GridLength desired{hide ? 0.0 : 1.0, hide ? GridUnitType::Pixel : GridUnitType::Auto};
                if (col.Width().GridUnitType != desired.GridUnitType || col.Width().Value != desired.Value) col.Width(desired);
            }
        }
        // An explicit no-music test preview remains available even if normal
        // battery saving suppresses the real-time audio visualizer.
        const bool previewDemo = VizPreviewWithoutPlayback();
        const bool runViz = !hide && !g_settings.lyricsOnly && g_settings.vizEnabled &&
                            (!ReduceMotionForPower() || previewDemo);
        if (runViz != g_vizCurrentlyVisible) {
            g_vizCurrentlyVisible = runViz;
            if (runViz) StartVizTimer();
            else StopVizTimer();
        }
        // Do not open a real WASAPI loopback stream just for test animation.
        const bool captureNeeded = runViz && !VizPreviewWithoutPlayback();
        // Use the real worker state, not a function-static remembered flag:
        // an Explorer/taskbar reinjection may have already stopped that worker.
        if (captureNeeded != g_CaptureRunning.load()) {
            if (captureNeeded) StartVizCaptureThread();
            else StopVizCaptureThread();
        }
        if (!hide && !ReduceMotionForPower() && !g_settings.lyricsOnly &&
            (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) &&
            (g_titleScroll.active || g_artistScroll.active)) StartScrollTimer();
        else StopScrollTimer();
    } catch (...) { Wh_Log(L"UpdateVisibility: failed"); }
}
static void ApplySettings() {
    g_idleSeconds  = 0;
    g_idleTicks    = 0;
    g_hiddenByIdle = false;
    try { RemovePlayerGrid(); } catch (...) { Wh_Log(L"ApplySettings: Exception in RemovePlayerGrid"); }
    if (!g_unloading) {
        try { InjectPlayerGrid(); } catch (...) { Wh_Log(L"ApplySettings: Exception in InjectPlayerGrid"); }
    }
}
static void ApplySettingsWithRetry(FrameworkElement xamlRootContent, int retryCount = 0) {
    if (g_unloading) return;
    bool ready = false;
    try {
        if (!xamlRootContent && g_taskbarWnd) {
            if (auto root = GetTaskbarXamlRoot(g_taskbarWnd)) xamlRootContent = root.Content().try_as<FrameworkElement>();
        }
        if (xamlRootContent && FindTaskbarRootGrid(xamlRootContent)) {
            ApplySettings();
            ready = g_playerGrid != nullptr;
        }
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"Taskbar initialization failed: HRESULT 0x%08X", static_cast<unsigned>(error.code()));
    } catch (...) {
        Wh_Log(L"Taskbar initialization failed; waiting for the taskbar tree");
    }
    if (ready || g_unloading) return;
    constexpr int kMaxRetries = 50;
    if (retryCount >= kMaxRetries) {
        Wh_Log(L"Taskbar player could not initialize after %d attempts; check Windows build and taskbar symbols", kMaxRetries);
        return;
    }
    CancelStartupRetry();
    const uint64_t generation = g_startupRetryGeneration;
    try {
        g_startupRetryTimer = DispatcherTimer();
        g_startupRetryTimer.Interval(std::chrono::milliseconds(100));
        g_startupRetryToken = g_startupRetryTimer.Tick([generation, retryCount](auto const&, auto const&) {
            if (generation != g_startupRetryGeneration || g_unloading) return;
            CancelStartupRetry();
            ApplySettingsWithRetry(nullptr, retryCount + 1);
        });
        g_startupRetryTimer.Start();
    } catch (...) {
        CancelStartupRetry();
        Wh_Log(L"Cannot schedule taskbar initialization retry");
    }
}
using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading || g_applyingSettings) {
        return;
    }
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: Taskbar window not found");
        return;
    }
    // Discard pending cover clicks and old UI delegates on an actual taskbar restart.
    StopSongChangeAnimations();
    g_lastAnimatedTrack.clear();
    for (auto& cleanup : g_uiCleanup) { try { cleanup(); } catch (...) {} }
    g_uiCleanup.clear();
    g_cachedTaskbarXamlRoot = nullptr;
    g_cachedTaskbarRootWindow = nullptr;
    g_playerGrid      = nullptr;
    g_vizNormalColumn = -1;
    g_compactNoLyricsActive = false;
    g_albumArtworkHovered = false;
    g_lyricsOverlayHost = nullptr;
    g_lyricsViewport = nullptr;
    g_lyricsGuardClip = nullptr;
    g_lyricsWidthStoryboard = nullptr;
    g_lyricsOpacityStoryboard = nullptr;
    g_lyricsTargetWidth = -1.0;
    g_injectionParent = nullptr;
    g_playerColumn    = -1;
    g_trackedElement  = nullptr;
    g_hasTrackedElementOriginalMargin = false;
    g_trackPosition   = L"";
    g_layoutUpdateToken = {};
    g_taskbarWnd = hWnd;
    g_cachedAlbumTitle.clear();
    g_cachedAlbumArtist.clear();
    g_cachedThumbnailBytes.clear();
    g_cachedPaletteHash = 0;
    g_cachedAppIconSize = -1;
    g_blurBgCache.Invalidate();
    StopVizTimer();
    g_vizDispatcherTimer = nullptr;
    g_vizDispatcherTimerHasToken = false;
    StopScrollTimer();
    g_scrollDispatcherTimer = nullptr;
    g_scrollDispatcherTimerHasToken = false;
    if (!g_settings.lyricsOnly && g_settings.vizEnabled && !ReduceMotionForPower()) {
        SpawnTrackedWorker([]() {
            StopVizCaptureThread();
            StartVizCaptureThread();
        });
        StartVizTimer();
    }
    if (!g_settings.lyricsOnly && (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling)) {
        StartScrollTimer();
    }
    ApplySettingsWithRetry(nullptr);
}
static bool HookTaskbarDllSymbols() {
    static const wchar_t* const kCandidates[] = {
        L"taskbar.dll",
    };
    HMODULE h = nullptr;
    for (auto* name : kCandidates) {
        h = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (h) break;
    }
    if (!h) { return FALSE; }
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
    if (!WindhawkUtils::HookSymbols(h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        return FALSE;
    }
    return TRUE;
}
BOOL Wh_ModInit() {
    g_unloading = false;
    g_applyingSettings = false;
    g_taskbarWnd = nullptr;
    g_needsUiUpdate = false;
    if (!g_vizBars) g_vizBars.emplace();
    if (!g_vizBrushes) g_vizBrushes.emplace();
    LoadSettings();
    SYSTEM_POWER_STATUS power{};
    if (GetSystemPowerStatus(&power)) g_onBattery = power.ACLineStatus == 0 || power.SystemStatusFlag != 0;
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Wh_ModInit: HookTaskbarDllSymbols failed");
        return FALSE;
    }
    return TRUE;
}
void Wh_ModAfterInit() {
    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    StartMediaThread();
    StartTimerThread();
    if (g_taskbarWnd) {
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            g_cachedAlbumTitle.clear();
            g_cachedAlbumArtist.clear();
            g_cachedThumbnailBytes.clear();
            g_cachedPaletteHash = 0;
            g_cachedAppIconSize = -1;
            g_blurBgCache.Invalidate();
            ApplySettingsWithRetry(nullptr);
            if (g_playerGrid) {
                ShowSuccessNotification();
                RefreshPlayerContents();
                UpdateVisibility();
                g_needsUiUpdate = true;
                if (g_timerUpdateEvent) {
                    SetEvent(g_timerUpdateEvent);
                }
            }
        }, nullptr);
    }
}
void Wh_ModUninit() {
    // A monitor gesture can also be rebuilding the UI; finish it before teardown.
    std::lock_guard<std::mutex> change(g_settingsChangeMutex);
    {
        std::lock_guard<std::mutex> admission(g_workerAdmissionMutex);
        g_unloading = true;
    }
    StopTimerThread();
    StopMediaThread();
    WaitForTrackedWorkers();
    if (g_taskbarWnd)
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            RemovePlayerGrid();
            g_cachedTaskbarXamlRoot = nullptr;
            g_cachedTaskbarRootWindow = nullptr;
            g_mediaHoverBrush   = nullptr;
            g_mediaPressedBrush = nullptr;
            g_playerHoverBrush  = nullptr;
            g_playerPressedBrush = nullptr;
            g_playerBorderBrush  = nullptr;
            g_playerBorderPressedBrush = nullptr;
            g_vizBars.reset();
            g_vizBrushes.reset();
            g_miniPlayerArtRef            = nullptr;
            g_miniPlayerTitleRef          = nullptr;
            g_miniPlayerArtistRef         = nullptr;
            g_miniPlayerPlayBtnRef        = nullptr;
            g_miniPlayerPrevBtnRef        = nullptr;
            g_miniPlayerNextBtnRef        = nullptr;
            g_miniPlayerShuffleBtnRef     = nullptr;
            g_miniPlayerRepeatBtnRef      = nullptr;
            g_miniPlayerSessionListRef    = nullptr;
            g_miniPlayerSessionListSepRef = nullptr;
            g_fluentMediaButtonStyle      = nullptr;
        }, nullptr);
    else {
        g_mediaHoverBrush   = nullptr;
        g_mediaPressedBrush = nullptr;
        g_playerHoverBrush  = nullptr;
        g_playerPressedBrush = nullptr;
        g_playerBorderBrush  = nullptr;
        g_playerBorderPressedBrush = nullptr;
        g_vizBars.reset();
        g_vizBrushes.reset();
    }
    CleanupAudioDeviceEnumerator();
}
static bool ReconfigureSettingsOnUi(int monitorOverride) {
    HWND oldWindow = g_taskbarWnd;
    if (!oldWindow || !IsWindow(oldWindow)) oldWindow = FindCurrentProcessTaskbarWnd();
    if (!oldWindow) {
        Wh_Log(L"Settings update deferred: no live taskbar UI window");
        return false;
    }
    // A display may have disconnected while producers were draining.
    if (monitorOverride > 0 && !FindTaskbarWndForMonitor(GetMonitorByNumber(monitorOverride))) return false;
    struct Context { bool loaded = false; int monitorOverride = 0; } context{false, monitorOverride};
    const bool removed = RunFromWindowThread(oldWindow, [](void* param) {
        auto& state = *static_cast<Context*>(param);
        RemovePlayerGrid();
        // Release old-thread-affine timers/styles before moving to another monitor.
        if (g_scrollDispatcherTimer) {
            g_scrollDispatcherTimer.Stop();
            if (g_scrollDispatcherTimerHasToken) g_scrollDispatcherTimer.Tick(g_scrollDispatcherTimerToken);
            g_scrollDispatcherTimer = nullptr;
        }
        g_scrollDispatcherTimerHasToken = false;
        g_scrollDispatcherTimerToken = {};
        if (g_vizDispatcherTimer) {
            g_vizDispatcherTimer.Stop();
            if (g_vizDispatcherTimerHasToken) g_vizDispatcherTimer.Tick(g_vizDispatcherTimerToken);
            g_vizDispatcherTimer = nullptr;
        }
        g_vizDispatcherTimerHasToken = false;
        g_vizDispatcherTimerToken = {};
        g_vizCurrentlyVisible = false;
        if (g_vizBars) g_vizBars->clear();
        if (g_vizBrushes) g_vizBrushes->clear();
        g_mediaHoverBrush = nullptr;
        g_mediaPressedBrush = nullptr;
        g_playerHoverBrush = nullptr;
        g_playerPressedBrush = nullptr;
        g_playerBorderBrush = nullptr;
        g_playerBorderPressedBrush = nullptr;
        g_fluentMediaButtonStyle = nullptr;
        ModSettings previous = g_settings;
        std::vector<MediaButtonConfig> previousButtons;
        { std::lock_guard<std::mutex> lock(g_mediaButtonsMutex); previousButtons = g_mediaButtons; }
        try {
            if (state.monitorOverride > 0) g_settings.monitor = state.monitorOverride;
            else LoadSettings();
            state.loaded = true;
        } catch (...) {
            g_settings = std::move(previous);
            { std::lock_guard<std::mutex> lock(g_mediaButtonsMutex); g_mediaButtons = std::move(previousButtons); }
            Wh_Log(L"Settings read failed; restoring the previous configuration");
        }
        if (g_settings.monitor != previous.monitor) {
            g_cachedTaskbarXamlRoot = nullptr;
            g_cachedTaskbarRootWindow = nullptr;
        }
        // No producers or tracked jobs are running at this point.
        ++g_lyricsMediaPropsRequestId;
        g_mediaPropertiesBusy = false;
        {
            std::lock_guard<std::mutex> lock(g_lyricsMutex);
            g_lyricsKey.clear();
            // A change to source-script or matching settings invalidates cached lyrics.
            g_lyricsCache.clear();
            ++g_lyricsGeneration;
            g_lyricsCues.clear();
            g_lyricsCurrentIndex = -1;
            g_lyricsDisplayIndex = -1;
            g_lyricsFarDueSoon = false;
            g_lyricsLastPositionMs = -1;
            g_lyricsWorkerActive = false;
            g_lyricsPending = false;
            g_lyricsNeedRetry = false;
            g_lyricsFailureCount = 0;
        }
        g_timelineDirty = true;
        g_vizBaseColorDirty = true;
        g_vizPaletteColorsDirty = true;
        g_hiddenByIdle = false;
        g_needsLyricsUiUpdate = false;
    }, &context);
    if (!removed) return false;
    HWND newWindow = FindCurrentProcessTaskbarWnd();
    if (!newWindow) newWindow = IsWindow(oldWindow) ? oldWindow : nullptr;
    if (!newWindow) return false;
    g_taskbarWnd = newWindow;
    const bool rebuilt = RunFromWindowThread(newWindow, [](void*) {
        if (!g_vizBars) g_vizBars.emplace();
        if (!g_vizBrushes) g_vizBrushes.emplace();
        ApplySettingsWithRetry(nullptr);
    }, nullptr);
    return context.loaded && rebuilt;
}
static bool ApplySettingsLive(int monitorOverride = 0) {
    // A gesture runs on Explorer's UI thread: never block it behind a settings
    // callback which might be waiting to dispatch onto that same thread.
    std::unique_lock<std::mutex> change(g_settingsChangeMutex, std::defer_lock);
    if (monitorOverride > 0) { if (!change.try_lock()) return false; }
    else change.lock();
    if (g_unloading) return false;
    struct Actions {
        int monitorOverride;
        void Pause() {
            std::lock_guard<std::mutex> admission(g_workerAdmissionMutex);
            g_applyingSettings = true;
        }
        void StopProducers() { StopTimerThread(); StopMediaThread(); }
        void DrainWorkers() { WaitForTrackedWorkers(); }
        bool ReconfigureOnUi() { return ReconfigureSettingsOnUi(monitorOverride); }
        void Resume() {
            {
                std::lock_guard<std::mutex> admission(g_workerAdmissionMutex);
                g_applyingSettings = false;
            }
            if (!g_unloading) {
                StartMediaThread();
                StartTimerThread();
                DispatchMediaUpdate();
            }
        }
        void ReportFailure() noexcept { Wh_Log(L"In-place settings update failed; the module remains loaded"); }
    } actions{monitorOverride};
    return LiveSettings::Apply(actions);
}
static void MovePlayerToNextMonitor() {
    if (g_unloading || g_applyingSettings || !g_taskbarWnd) return;
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR monitor, HDC, LPRECT, LPARAM data) -> BOOL {
        try { reinterpret_cast<std::vector<HMONITOR>*>(data)->push_back(monitor); }
        catch (...) { return FALSE; }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&monitors));
    const HMONITOR currentMonitor = MonitorFromWindow(g_taskbarWnd, MONITOR_DEFAULTTONEAREST);
    int current = -1;
    std::vector<bool> available;
    for (size_t i = 0; i < monitors.size(); ++i) {
        if (monitors[i] == currentMonitor) current = (int)i;
        available.push_back(FindTaskbarWndForMonitor(monitors[i]) != nullptr);
    }
    const int next = MonitorCycle::Next(available, current);
    if (next < 0 || next == current) {
        Wh_Log(L"Monitor switch: no other display with an Explorer taskbar");
        return;
    }
    if (!ApplySettingsLive(next + 1)) Wh_Log(L"Monitor switch was not applied");
}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    // XAML objects/delegates may survive a tree removal. Keep their code mapped.
    if (reload) *reload = FALSE;
    return ApplySettingsLive() ? TRUE : FALSE;
}