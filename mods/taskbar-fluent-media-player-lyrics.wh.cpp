// ==WindhawkMod==
// @id              taskbar-fluent-media-player-lyrics
// @name            Taskbar Fluent Media Player Lyrics
// @description     A fork of Taskbar Fluent Media Player with synchronized lyrics and a full player on every monitor's taskbar.
// @description:zh-CN Taskbar Fluent Media Player 的分支，支持同步歌词和多显示器任务栏播放器。
// @description:ru-RU Форк Taskbar Fluent Media Player с синхронизированными текстами песен и полным плеером на каждом мониторе.
// @version         2.0
// @author          momoca
// @github          https://github.com/momoca
// @homepage        https://github.com/momoca/Taskbar-Fluent-Media-Player-Lyrics
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -luser32 -lwindowsapp -lshell32 -lgdi32 -lshlwapi -lwindowscodecs -ldwmapi -lshcore -lksuser -lwinhttp -lbcrypt
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Taskbar Fluent Media Player Lyrics

A fork of **[Taskbar Fluent Media Player](https://github.com/Salyts/Taskbar-Fluent-Media-Player)** by Salyts, maintained by [momoca](https://github.com/momoca).
Version 2.0 adds a dedicated two-line lyrics area and full-player mirroring across monitors.

[English](https://github.com/momoca/Taskbar-Fluent-Media-Player-Lyrics/blob/main/README.md) | [简体中文](https://github.com/momoca/Taskbar-Fluent-Media-Player-Lyrics/blob/main/README_CN.md) | [Русский](https://github.com/momoca/Taskbar-Fluent-Media-Player-Lyrics/blob/main/README_RU.md)

## Full multi-monitor player

Show a complete player on every available Windows taskbar: album art, title,
artist, controls, visualization, and lyrics when available. Each display has
its own UI; playback and settings are shared. Main Settings provides the
all-monitor switch, primary Monitor selection, missing-anchor fallback and
secondary horizontal offset. Mirror layout only reverses element order.

## Synchronized lyrics

The separate two-line lyrics area is visible only when enabled and lyrics
have been retrieved for the current track. Otherwise the whole area is hidden
and its space released. Width defaults to 260 DIP (80-1200); height and font
size follow each taskbar. Lines are vertically centered with compact spacing.

Local UTF-8 LRC, plain QRC and YRC are checked first. Online providers are QQ
Music, NetEase and LRCLIB. QQ QRC, NetEase YRC and enhanced LRC use real word
timing; ordinary LRC stays line-synchronized without estimated word timing.
Long lines scroll within the area. Global and per-source offsets are supported.

## Media sources

QQ, NetEase, KuGou/MoeKoeMusic, Spotify, Apple Music/iTunes, Soda Music,
foobar2000, browsers and other SMTC players have separate switches, priorities
and offsets in Media Sources. Accurate metadata and a usable timeline are
required. NetEase may need inflink-rs; foobar2000 may need foo_mediacontrol.
These companions are not installed automatically. See the README compatibility
table for tested and conditional integrations.

## Installation and network use

Use this repository's source to install or update a local Windhawk mod; the
upstream gallery entry does not install this fork. Enable only one copy of
this taskbar player. Upgrading from an earlier local build with the original ID requires copying or reapplying its settings to the new mod ID.

Online lookup sends song metadata or service track IDs to lyrics providers.
No player account or user login cookie is read. Disable online retrieval for
local-only operation. Successful lyrics are cached only in memory.

## Credits

Original player: Salyts; original visualization: GR0UD. Fork additions: momoca.
Lyrics architecture and QQ reference: ANYNC/TaskbarLyrics. NetEase protocol
reference: Lyricify Lyrics Helper. Embedded DEFLATE: Mark Adler's puff.
The upstream MIT license and embedded third-party notices are included in the source.

[Report fork issues](https://github.com/momoca/Taskbar-Fluent-Media-Player-Lyrics/issues)
*/
// ==/WindhawkModReadme==

/*
MIT License

Copyright (c) 2026 Salyts

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


// ==WindhawkModSettings==
/*
- MainSettings:
  - PlayerSetting:
    - position: "tray_left"
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
    - mirrorAllMonitors: true
      $name: Show identical full player on all monitors
      $name:zh-CN: 所有显示器显示完整相同样式播放器
      $description: Creates an independent fully styled copy of the player on every taskbar. Monitor selects the primary instance.
    - mirrorMissingAnchor: "start"
      $name: Secondary taskbar missing anchor fallback
      $name:zh-CN: 副屏缺少小组件等定位锚点时
      $description: When a target button is absent on a secondary taskbar, choose a nearby Start-button anchor or a left-edge overlay. This setting does not change explicit edge positions.
      $options:
      - "start": "Next to Start button (reserve space)"
      - "edge": "Taskbar left edge (overlay, no reserved space)"
      $options:zh-CN:
      - "start": "定位到开始按钮旁（为播放器预留空间）"
      - "edge": "定位到副屏任务栏左边缘（覆盖式，不预留空间）"
    - mirrorSecondaryOffsetX: 0
      $name: Secondary player horizontal offset (px)
      $name:zh-CN: 副屏播放器水平偏移（像素）
      $description: Positive = right, negative = left; only affects secondary displays. Recommended range -300 to 300.
    - monitor: 1
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
      $name:ru-RU: Отображать обложку альбома
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


  - MediaSourceSetting:
    - priority: "qq,netease,kugou,spotify,apple,soda,foobar,other,browser"
      $name: Player priority (comma-separated keys)
      $name:zh-CN: 播放器优先级（英文代号，逗号分隔）
      $description: qq,netease,kugou,spotify,apple,soda,foobar,other,browser. Playing sessions take precedence; manual session selection is retained.
    - qqEnabled: true
      $name: Enable QQ 音乐
      $name:zh-CN: 启用 QQ 音乐
    - qqOffsetMs: 0
      $name: QQ 音乐 lyric offset (ms)
      $name:zh-CN: QQ 音乐 歌词偏移（毫秒，正值提前）
    - neteaseEnabled: true
      $name: Enable 网易云音乐
      $name:zh-CN: 启用 网易云音乐
    - neteaseOffsetMs: 0
      $name: 网易云音乐 lyric offset (ms)
      $name:zh-CN: 网易云音乐 歌词偏移（毫秒，正值提前）
    - kugouEnabled: true
      $name: Enable 酷狗 / MoeKoeMusic
      $name:zh-CN: 启用 酷狗 / MoeKoeMusic
    - kugouOffsetMs: 0
      $name: 酷狗 / MoeKoeMusic lyric offset (ms)
      $name:zh-CN: 酷狗 / MoeKoeMusic 歌词偏移（毫秒，正值提前）
    - spotifyEnabled: true
      $name: Enable Spotify
      $name:zh-CN: 启用 Spotify
    - spotifyOffsetMs: 0
      $name: Spotify lyric offset (ms)
      $name:zh-CN: Spotify 歌词偏移（毫秒，正值提前）
    - appleEnabled: true
      $name: Enable Apple Music / iTunes
      $name:zh-CN: 启用 Apple Music / iTunes
    - appleOffsetMs: 0
      $name: Apple Music / iTunes lyric offset (ms)
      $name:zh-CN: Apple Music / iTunes 歌词偏移（毫秒，正值提前）
    - sodaEnabled: true
      $name: Enable 汽水音乐
      $name:zh-CN: 启用 汽水音乐
    - sodaOffsetMs: 0
      $name: 汽水音乐 lyric offset (ms)
      $name:zh-CN: 汽水音乐 歌词偏移（毫秒，正值提前）
    - foobarEnabled: true
      $name: Enable foobar2000
      $name:zh-CN: 启用 foobar2000
    - foobarOffsetMs: 0
      $name: foobar2000 lyric offset (ms)
      $name:zh-CN: foobar2000 歌词偏移（毫秒，正值提前）
    - otherEnabled: true
      $name: Enable 其他 SMTC 播放器
      $name:zh-CN: 启用 其他 SMTC 播放器
    - otherOffsetMs: 0
      $name: 其他 SMTC 播放器 lyric offset (ms)
      $name:zh-CN: 其他 SMTC 播放器 歌词偏移（毫秒，正值提前）
    - browserEnabled: true
      $name: Enable 浏览器媒体
      $name:zh-CN: 启用 浏览器媒体
    - browserOffsetMs: 0
      $name: 浏览器媒体 lyric offset (ms)
      $name:zh-CN: 浏览器媒体 歌词偏移（毫秒，正值提前）
    $name: Media Sources
    $name:zh-CN: 播放源
    $description: Netease may need inflink-rs; foobar2000 needs foo_mediacontrol. Incomplete player timelines cannot be reconstructed reliably.
  - TextAreaSetting:
    - showLyrics: true
      $name: Show separate lyrics area
      $name:zh-CN: 显示独立歌词区域
      $description: Fixed-width two-line lyrics beside the player. Height follows the taskbar; the artist row is preserved.
    - lyricsWidth: 260
      $name: Lyrics area width (DIP)
      $name:zh-CN: 歌词区域长度（DIP）
      $description: Fixed width from 80 to 1200; independent of each lyric line's length. Height automatically fits the taskbar.
    - lyricsWordHighlight: true
      $name: Highlight real word timing
      $name:zh-CN: 逐字高亮（仅真实逐字时间）
      $description: Uses QQ QRC, Netease YRC or local enhanced LRC word timing. Ordinary LRC remains line-synchronized; word timing is never estimated.
    - onlineLyrics: true
      $name: Fetch lyrics from QQ Music, Netease and LRCLIB
      $name:zh-CN: 从 QQ 音乐、网易云及 LRCLIB 在线获取歌词
      $description: Sends song metadata or supplied service song IDs to QQ Music, Netease and LRCLIB. Local lyrics are checked first.
    - lyricsFolder: ""
      $name: Local UTF-8 LRC folder
      $name:zh-CN: 本地 UTF-8 LRC 歌词目录
      $description: Looks for Artist - Title.lrc, Title - Artist.lrc, or Title.lrc directly in this folder.
    - lyricsOffsetMs: 0
      $name: Lyrics offset (milliseconds)
      $name:zh-CN: 歌词时间偏移（毫秒，正值提前）
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
      $name: Spacing between title and artist
      $name:ru-RU: Отступ между названием и исполнителем
    - enableTitleScrolling: true
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
      $name: Scroll speed (1-10)
      $name:ru-RU: Скорость прокрутки (1-10)
      $description: Controls how fast the text scrolls. 1 = slowest, 10 = fastest.
      $description:ru-RU: Управляет скоростью прокрутки текста. 1 = медленнее всего, 10 = быстрее всего.
    - scrollPauseDuration: 1000
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
      $name: Bar size (width height)
      $name:ru-RU: Размер столбика (ширина высота)
      $description: Two values "width height" — bar width in px (0-40) and idle (resting) height in px (0-15). Width 0 hides the bars.
      $description:ru-RU: Два значения "ширина высота" — ширина столбика в пикселях (0-40) и высота покоя в пикселях (0-15). Ширина 0 скрывает столбики.
    - vizPadding: "0 0"
      $name: Padding (left right)
      $name:ru-RU: Отступы (слева справа)
      $description: Two values "left right" in px.
      $description:ru-RU: Два значения "слева справа" в пикселях.
    - vizSensitivity: 150
      $name: Sensitivity (0-300)
      $name:ru-RU: Чувствительность (0-300)
    $name: Visualizer
    $name:ru-RU: Визуализатор
  $name: Main Settings
  $name:ru-RU: Основные настройки


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
      $name: Solid color opacity (0-100)
      $name:ru-RU: Прозрачность сплошного цвета (0-100)
    - gradientAngle: 50
      $name: Gradient rotation angle (0-360)
      $name:ru-RU: Угол поворота градиента (0-360)
    - gradientBalance: 50
      $name: Gradient color balance (0-100)
      $name:ru-RU: Градиентный цветовой баланс (0-100)
    - acrylicTintOpacity: 50
      $name: Acrylic tint opacity (0-100)
      $name:ru-RU: Прозрачность акрилового оттенка (0-100)
    - micaOpacity: 50
      $name: Mica/Mica Alt opacity (0-100)
      $name:ru-RU: "'Mica/Mica Alt' прозрачность (0-100)"
    - blurOpacity: 65
      $name: Album art blur opacity (0-100)
      $name:ru-RU: "'Размытая обложка альбома' прозрачность (0-100)"
    - blurRadius: 11
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
      $name: Spacing between media buttons
      $name:ru-RU: Расстояние между кнопками управления
    - buttonIconSize: 12
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
      $name: Pause icon size
      $name:ru-RU: Размер значка паузы
    - pauseOverlayOpacity: 60
      $name: Pause overlay background opacity (0-100)
      $name:ru-RU: Прозрачность фона оверлея паузы (0-100)
    - albumArtOpacity: 100
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
    $name: Show notification on successful mod load
    $name:ru-RU: Показывать уведомление при успешной загрузке мода
    $description: Display a Windows notification when the mod is successfully loaded or reloaded
    $description:ru-RU: Отображать уведомление Windows при успешной загрузке или перезагрузке мода
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
    $name: Context menu icons opacity (0-100)
    $name:ru-RU: Прозрачность иконок контекстного меню (0-100)
  $name: Context Menu Settings
  $name:ru-RU: Настройки контекстного меню


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
#include <mutex>
#include <string>
#include <vector>
#include <list>
#include <optional>
#include <set>
#include <algorithm>
#include <thread>
#include <cmath>
#include <chrono>
#include <winhttp.h>
#include <winrt/Windows.Data.Json.h>
#include <sstream>
#include <cwctype>
#include <array>
#include <bcrypt.h>

enum class PlayerSource { QQ, Netease, Kugou, Spotify, Apple, Soda, Foobar, Other, Browser, Count };
static constexpr size_t kPlayerSourceCount = (size_t)PlayerSource::Count;
static constexpr const wchar_t* kPlayerSourceKeys[] = {L"qq",L"netease",L"kugou",L"spotify",L"apple",L"soda",L"foobar",L"other",L"browser"};
static PlayerSource ClassifyPlayerSource(std::wstring id) {
    for (auto& c : id) if(c>=L'A' && c<=L'Z') c += L'a'-L'A';
    auto has=[&](const wchar_t* token){return id.find(token)!=std::wstring::npos;};
    if(has(L"qqmusic")) return PlayerSource::QQ;
    if(has(L"cloudmusic")||has(L"netease")||has(L"163music")||has(L"music.163")||has(L"inflink")||has(L"wyy")) return PlayerSource::Netease;
    if(has(L"kugou")||has(L"moekoe")) return PlayerSource::Kugou;
    if(has(L"spotify")) return PlayerSource::Spotify;
    if(has(L"applemusic")||has(L"itunes")) return PlayerSource::Apple;
    if(has(L"com.soda.music")||has(L"sodamusic")||has(L"汽水")) return PlayerSource::Soda;
    if(has(L"foobar2000")||has(L"foobar")) return PlayerSource::Foobar;
    if(has(L"chrome")||has(L"msedge")||has(L"microsoftedge")||has(L"firefox")||has(L"opera")||has(L"brave")||has(L"vivaldi")||has(L"thebrowsercompany.arc")||id==L"arc.exe") return PlayerSource::Browser;
    return PlayerSource::Other;
}
struct PlayerSourceConfig { bool enabled = true; int offsetMs = 0; int priority = 0; };
static std::mutex g_sourceConfigMtx;
static std::array<PlayerSourceConfig,kPlayerSourceCount> g_sourceConfigs;
static PlayerSourceConfig GetSourceConfig(PlayerSource source) {
    std::lock_guard<std::mutex> lock(g_sourceConfigMtx);
    return g_sourceConfigs[(size_t)source];
}
static std::array<int,kPlayerSourceCount> ParsePlayerPriority(std::wstring order) {
    std::array<int,kPlayerSourceCount> priorities;
    priorities.fill(-1);
    int rank=0;
    size_t pos=0;
    while(pos<order.size()) {
        size_t end=order.find(L',',pos);
        std::wstring key=order.substr(pos,end==std::wstring::npos?end:end-pos);
        key.erase(std::remove_if(key.begin(),key.end(),[](wchar_t c){return iswspace(c);}),key.end());
        for(auto& c:key)if(c>=L'A'&&c<=L'Z')c+=L'a'-L'A';
        for(size_t i=0;i<kPlayerSourceCount;++i) if(key==kPlayerSourceKeys[i]&&priorities[i]<0) priorities[i]=rank++;
        if(end==std::wstring::npos) break;
        pos=end+1;
    }
    for(auto& priority:priorities)if(priority<0)priority=rank++;
    return priorities;
}
struct PlayerTimelineSmoother {
    std::wstring identity;
    bool initialized=false, wasPlaying=false;
    int64_t lastRaw=0, lastUpdated=0;
    uint64_t sampledAt=0;
    double position=0, previousRate=1;
    double Select(PlayerSource source,const std::wstring& track,int64_t raw,int64_t updated,
                  int64_t wallNow,uint64_t now,bool playing,double rate) {
        rate=std::isfinite(rate)?std::clamp(rate,0.0,4.0):1.0;
        int64_t age=wallNow-updated;
        bool usable=updated>0 && age>=0 && age<86400000;
        if(source!=PlayerSource::Apple) { initialized=false; return std::max(0.0,raw+(playing&&usable?age*rate:0)); }
        bool reset=!initialized||identity!=track;
        double projected=position+(wasPlaying?(now-sampledAt)*previousRate:0);
        if(reset) projected=raw+(playing&&usable&&age<=1000?age*rate:0);
        else if(updated>lastUpdated) {
            double error=raw-projected;
            // Ignore timestamp-only updates of Apple's whole-second position.
            if((raw!=lastRaw && (raw<lastRaw||std::abs(error)>2000)) ||
               (playing!=wasPlaying && std::abs(error)>1000)) projected=raw;
        }
        position=std::max(0.0,projected); identity=track; initialized=true;
        lastRaw=raw; lastUpdated=updated; sampledAt=now; wasPlaying=playing; previousRate=rate;
        return position;
    }
};
// Separate snapshots keep the lyrics worker independent of UI settings reloads.
#include <tlhelp32.h>
// Native NetEase SMTC currently omits its timeline. Read only the playback
// clock referenced by this distinctive pair of x64 stores in cloudmusic.dll.
// Discovery approach: https://github.com/nczxe/SMTCPlayer (MIT),
// server/netease_watcher_server.py. No injection or process writes are used.
struct NeteaseNativeClock {
    struct Candidate {
        HANDLE process = nullptr;
        uintptr_t address = 0;
        double previous = -1, movement = 0;
        uint64_t sampled = 0, advancing = 0;
        unsigned samples = 0;
        bool validated = false;
    };
    std::vector<Candidate> candidates;
    uint64_t nextScan = 0;
    ~NeteaseNativeClock() { Clear(); }
    void Clear() {
        for (auto& c : candidates) if (c.process) CloseHandle(c.process);
        candidates.clear();
    }
    static bool Read(const Candidate& c, double& value) {
        SIZE_T read = 0;
        return ReadProcessMemory(c.process, reinterpret_cast<void*>(c.address), &value,
                   sizeof(value), &read) && read == sizeof(value) &&
               std::isfinite(value) && value >= 0 && value <= 86400;
    }
    void Discover() {
        Clear();
#ifdef _WIN64
        auto kernel = GetModuleHandleW(L"kernel32.dll");
        auto moduleFirst = reinterpret_cast<decltype(&Module32FirstW)>(GetProcAddress(kernel, "Module32FirstW"));
        auto moduleNext = reinterpret_cast<decltype(&Module32NextW)>(GetProcAddress(kernel, "Module32NextW"));
        if (!moduleFirst || !moduleNext) return;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return;
        PROCESSENTRY32W entry{sizeof(entry)};
        if (Process32FirstW(snapshot, &entry)) do {
            if (_wcsicmp(entry.szExeFile, L"cloudmusic.exe")) continue;
            HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
                                         FALSE, entry.th32ProcessID);
            if (!process) continue;
            HANDLE modules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
                                                      entry.th32ProcessID);
            uintptr_t found = 0;
            MODULEENTRY32W module{sizeof(module)};
            if (modules != INVALID_HANDLE_VALUE && moduleFirst(modules, &module)) do {
                if (_wcsicmp(module.szModule, L"cloudmusic.dll")) continue;
                // Restrict discovery to a bounded image, never scan private heaps.
                if (module.modBaseSize > 128 * 1024 * 1024) break;
                std::vector<BYTE> image(module.modBaseSize);
                SIZE_T size = 0;
                if (!ReadProcessMemory(process, module.modBaseAddr, image.data(), image.size(), &size)
                    || size != image.size() || size < sizeof(IMAGE_DOS_HEADER)) break;
                auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image.data());
                if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 ||
                    size_t(dos->e_lfanew) + sizeof(IMAGE_NT_HEADERS64) > size) break;
                auto nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image.data() + dos->e_lfanew);
                if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) break;
                size_t sectionOffset = size_t(dos->e_lfanew) + offsetof(IMAGE_NT_HEADERS64, OptionalHeader)
                    + nt->FileHeader.SizeOfOptionalHeader;
                if (sectionOffset + size_t(nt->FileHeader.NumberOfSections) * sizeof(IMAGE_SECTION_HEADER) > size) break;
                auto sections = reinterpret_cast<const IMAGE_SECTION_HEADER*>(image.data() + sectionOffset);
                unsigned matches = 0;
                for (unsigned s = 0; s < nt->FileHeader.NumberOfSections; ++s) {
                    const auto& section = sections[s];
                    if (!(section.Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;
                    size_t begin = section.VirtualAddress;
                    size_t end = std::min<size_t>(size, begin + section.Misc.VirtualSize);
                    for (size_t i = begin; i + 16 <= end; ++i) {
                        const BYTE first[] = {0xf2,0x0f,0x11,0x3d}, second[] = {0xf2,0x0f,0x11,0x35};
                        if (memcmp(image.data()+i, first, 4) || memcmp(image.data()+i+8, second, 4)) continue;
                        int32_t displacement = 0;
                        memcpy(&displacement, image.data()+i+4, 4);
                        int64_t target = int64_t(i) + 8 + displacement;
                        if (target < 0 || uint64_t(target) + sizeof(double) > size || target % 8) continue;
                        bool writable = false;
                        for (unsigned j = 0; j < nt->FileHeader.NumberOfSections; ++j) {
                            const auto& data = sections[j];
                            if ((data.Characteristics & IMAGE_SCN_MEM_WRITE) &&
                                uint64_t(target) >= data.VirtualAddress &&
                                uint64_t(target)+8 <= uint64_t(data.VirtualAddress)+data.Misc.VirtualSize)
                                writable = true;
                        }
                        if (writable) { found = uintptr_t(module.modBaseAddr) + uintptr_t(target); ++matches; }
                    }
                }
                if (matches != 1) found = 0; // Unknown versions must fail closed.
                break;
            } while (moduleNext(modules, &module));
            if (modules != INVALID_HANDLE_VALUE) CloseHandle(modules);
            if (found) candidates.push_back({process, found});
            else CloseHandle(process);
        } while (Process32NextW(snapshot, &entry));
        CloseHandle(snapshot);
#endif
    }
    bool Sample(bool playing, double rate, int64_t& positionMs) {
        uint64_t now = GetTickCount64();
        bool anyValidated = false;
        for (auto& c : candidates) anyValidated |= c.validated;
        if (!anyValidated && now >= nextScan) {
            Discover();
            now = GetTickCount64();
            nextScan = now + 10000;
        }
        bool available = false;
        double selected = 0;
        for (auto& c : candidates) {
            double value = 0;
            if (!Read(c, value)) { c.validated = false; c.previous = -1; c.samples = 0; continue; }
            if (playing && c.previous >= 0 && now > c.sampled) {
                double elapsed = (now - c.sampled) / 1000.0;
                double delta = value - c.previous;
                if (delta > 0 && std::abs(delta - elapsed * rate) <= std::max(0.12, elapsed * 0.35)) {
                    c.movement += delta; ++c.samples; c.advancing = now;
                    if (c.samples >= 3 && c.movement >= 0.6) c.validated = true;
                } else if (delta != 0) {
                    // A seek/track change keeps a previously validated clock, but
                    // an unvalidated candidate must demonstrate continuous motion.
                    c.samples = 0; c.movement = 0;
                    if (c.validated) c.advancing = now;
                }
                if (c.validated && now - c.advancing > 2500) c.validated = false;
            }
            c.previous = value; c.sampled = now;
            if (c.validated) {
                if (available && std::abs(selected - value) > 0.5) return false;
                selected = value; available = true;
            }
        }
        if (available) positionMs = int64_t(selected * 1000);
        return available;
    }
};
static std::atomic<bool> g_lyricsEnabled{true}, g_lyricsOnline{true};
static std::atomic<int> g_lyricsOffsetMs{0};
static int g_lyricsWidth = 260;
static bool g_lyricsWordHighlight = true;
static std::mutex g_lyricsMtx;
static std::wstring g_lyricsFolder, g_lyricTitle, g_lyricArtist, g_lyricApp, g_lyricText;
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
    int          monitor              = 1;
    bool         mirrorAllMonitors    = true;
    std::wstring mirrorMissingAnchor = L"start";
    int          mirrorSecondaryOffsetX = 0;
    std::wstring position             = L"tray_left";
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
    int          vizSensitivity  = 150;
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
    {
        PCWSTR order = Wh_GetStringSetting(L"MainSettings.MediaSourceSetting.priority");
        auto priorities = ParsePlayerPriority(order ? order : L"");
        Wh_FreeStringSetting(order);
        std::lock_guard<std::mutex> lock(g_sourceConfigMtx);
        for (size_t i=0;i<kPlayerSourceCount;++i) {
            std::wstring prefix=L"MainSettings.MediaSourceSetting." + std::wstring(kPlayerSourceKeys[i]);
            g_sourceConfigs[i] = {
                Wh_GetIntSetting((prefix+L"Enabled").c_str()) != 0,
                std::clamp(Wh_GetIntSetting((prefix+L"OffsetMs").c_str()),-10000,10000),
                priorities[i]};
        }
    }
    g_lyricsEnabled = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.showLyrics") != 0;
    g_lyricsOnline = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.onlineLyrics") != 0;
    g_lyricsOffsetMs = std::clamp(Wh_GetIntSetting(L"MainSettings.TextAreaSetting.lyricsOffsetMs"), -10000, 10000);
    int lyricsWidth = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.lyricsWidth");
    g_lyricsWidth = lyricsWidth == 0 ? 260 : std::clamp(lyricsWidth, 80, 1200);
    g_lyricsWordHighlight = Wh_GetIntSetting(L"MainSettings.TextAreaSetting.lyricsWordHighlight") != 0;
    {
        PCWSTR folder = Wh_GetStringSetting(L"MainSettings.TextAreaSetting.lyricsFolder");
        std::lock_guard<std::mutex> lock(g_lyricsMtx);
        g_lyricsFolder = folder ? folder : L"";
        Wh_FreeStringSetting(folder);
        g_lyricText.clear();
    }
    auto Str = [](const wchar_t* key, const wchar_t* def) -> std::wstring {
        PCWSTR p = Wh_GetStringSetting(key);
        std::wstring r = (*p != L'\0') ? p : def;
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
    g_settings.monitor               = std::max(1, Wh_GetIntSetting(L"MainSettings.PlayerSetting.monitor"));
    g_settings.mirrorAllMonitors      = Wh_GetIntSetting(L"MainSettings.PlayerSetting.mirrorAllMonitors") != 0;
    g_settings.mirrorMissingAnchor = Str(L"MainSettings.PlayerSetting.mirrorMissingAnchor", L"start");
    if (g_settings.mirrorMissingAnchor != L"start" && g_settings.mirrorMissingAnchor != L"edge")
        g_settings.mirrorMissingAnchor = L"start";
    g_settings.mirrorSecondaryOffsetX = std::clamp(
        Wh_GetIntSetting(L"MainSettings.PlayerSetting.mirrorSecondaryOffsetX"), -500, 500);
    g_settings.position             = Str(L"MainSettings.PlayerSetting.position",    L"tray_left");
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
    g_settings.vizColor        = Str(L"AppearanceSettings.VisualizerStyleSettings.vizColor",  L"255 255 255");
    g_settings.vizColor1       = Str(L"AppearanceSettings.VisualizerStyleSettings.vizColor1", L"30 215 96");
    g_settings.vizColor2       = Str(L"AppearanceSettings.VisualizerStyleSettings.vizColor2", L"0 180 255");
    g_settings.vizSensitivity  = Int(L"MainSettings.VisualizerFunctionsSettings.vizSensitivity", 0, 300, 150);
    {
        int n = 7, gap = 5;
        ParseTwoInts(Str(L"MainSettings.VisualizerFunctionsSettings.vizBarCountGap", L"7 5"), n, gap);
        g_settings.vizBars   = std::clamp(n, 1, 20);
        g_settings.vizBarGap = std::clamp(gap, 0, 40);
        int w = 5, h = 3;
        ParseTwoInts(Str(L"MainSettings.VisualizerFunctionsSettings.vizBarSize", L"5 3"), w, h);
        g_settings.vizBarWidth    = std::clamp(w, 0, 40);
        g_settings.vizIdleBarSize = std::clamp(h, 0, 15);
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


static void TryMountPlayerOnTaskbar();
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_applyingSettings{false};
static HWND g_taskbarWnd = nullptr;


static std::atomic<int> g_activeWorkerThreads{0};
template <typename F>
static void SpawnTrackedWorker(F&& fn) {
    g_activeWorkerThreads.fetch_add(1, std::memory_order_relaxed);
    std::thread([f = std::forward<F>(fn)]() mutable {
        struct Decrementer {
            ~Decrementer() { g_activeWorkerThreads.fetch_sub(1, std::memory_order_relaxed); }
        } dec;
        f();
    }).detach();
}


static void WaitForTrackedWorkers() {
    static constexpr DWORD kTimeoutMs = 2000;
    DWORD deadline = GetTickCount() + kTimeoutMs;
    HWND hTaskbar = g_taskbarWnd;
    DWORD tid = GetCurrentThreadId();
    bool isUiThread = hTaskbar && (GetWindowThreadProcessId(hTaskbar, nullptr) == tid);
    if (isUiThread) {
        while (g_activeWorkerThreads.load(std::memory_order_relaxed) > 0 &&
               GetTickCount() < deadline) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            Sleep(10);
        }
    } else {
        while (g_activeWorkerThreads.load(std::memory_order_relaxed) > 0 &&
               GetTickCount() < deadline) {
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
static int              g_playerColumn    = -1;
[[clang::no_destroy]] static std::function<void()> g_playerButtonStateUpdater;
[[clang::no_destroy]] static ToolTip          g_playerToolTip   = nullptr;
static std::atomic<bool> g_needsUiUpdate{false};
static void ShowSuccessNotification() {
    if (!g_settings.showSuccessNotification) {
        return;
    }
    std::thread([]() {
        MessageBoxW(
            nullptr,
            L"Media player successfully loaded and ready to use",
            L"Taskbar Fluent Media Player",
            MB_ICONINFORMATION | MB_OK | MB_TOPMOST | MB_SETFOREGROUND
        );
    }).detach();
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
static bool RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param) {
    static const UINT kMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Payload { WindowThreadProc proc; void* param; };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        proc(param);
        return true;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM w, LPARAM l) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(l);
                static const UINT kM = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == kM) {
                    auto* p = reinterpret_cast<Payload*>(cwp->lParam);
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, code, w, l);
        }, nullptr, tid);
    if (!hook) return false;
    Payload pay{proc, param};
    SendMessageW(hWnd, kMsg, 0, reinterpret_cast<LPARAM>(&pay));
    UnhookWindowsHookEx(hook);
    return true;
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
    bool              canSkipPrevious  = true;
    bool              canSkipNext      = true;
    bool              canShuffle       = true;
    bool              canRepeat        = true;
    bool              canSeek          = true;
};
static MediaState g_media;
static std::mutex g_mediaMtx;
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
static int g_cachedAppIconSize = -1;
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
static winrt::Windows::UI::Color g_cachedVizBaseColor{255, 255, 255, 255};
static bool g_vizBaseColorDirty = true;
static winrt::Windows::UI::Color g_cachedVizPal0{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizPal1{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizCg0{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizCg1{255, 255, 255, 255};
static winrt::Windows::UI::Color g_cachedVizAcrCol{255, 255, 255, 255};
static bool g_vizPaletteColorsDirty = true;
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSessionManager g_sessionMgr     = nullptr;
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSession        g_currentSession = nullptr;
static std::mutex  g_sessionMtx;
static bool g_userSwitchedSession = false;
static std::atomic<bool> g_forceSessionRefresh{false};
static std::atomic<int> g_sessionCount{0};
static winrt::event_token g_evSessionsChanged{};
static winrt::event_token g_evCurrentChanged{};
static winrt::event_token g_evMediaProps{};
static winrt::event_token g_evPlayback{};
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
static void RunWhenButtonReady(Button const& btn, std::function<void()> const& action) {
    if (!btn || !action) return;
    auto run = std::make_shared<std::function<void()>>(action);
    auto invoke = [btn, run]() {
        try {
            btn.ApplyTemplate();
            (*run)();
        } catch (...) {}
    };
    try {
        if (btn.IsLoaded()) {
            invoke();
        } else {
            btn.Loaded([invoke](auto const&, auto const&) { invoke(); });
        }
    } catch (...) {
        invoke();
    }
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
                if (SUCCEEDED(pConv->Initialize(
                        pFrame, GUID_WICPixelFormat32bppBGRA,
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
static void SendMediaCommandAsync(int cmd) {
    SpawnTrackedWorker([cmd]() {
        if (g_unloading) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            { std::lock_guard<std::mutex> lk(g_sessionMtx); session = g_currentSession; }
            if (session) {
                switch (cmd) {
                    case 1: session.TrySkipPreviousAsync().get();    break;
                    case 2: {
                        auto info = session.GetPlaybackInfo();
                        bool isPlaying = info &&
                            info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                        if (isPlaying)
                            session.TryPauseAsync().get();
                        else
                            session.TryPlayAsync().get();
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
                            auto currentPos = timeline.Position();
                            auto newPos = currentPos - std::chrono::seconds(5);
                            if (newPos.count() < 0) newPos = std::chrono::seconds(0);
                            session.TryChangePlaybackPositionAsync(newPos.count()).get();
                        } catch (...) {}
                        break;
                    case 6:
                        try {
                            auto timeline = session.GetTimelineProperties();
                            auto currentPos = timeline.Position();
                            auto endTime = timeline.EndTime();
                            auto newPos = currentPos + std::chrono::seconds(5);
                            if (newPos > endTime) newPos = endTime;
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
        } catch (...) {}
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
    int    tickMs    = 16;
};
static TextScrollState g_titleScroll;
static TextScrollState g_artistScroll;
static void ResetScrollState(TextScrollState& s);
static void FetchMediaPropertiesAsync();
static void FetchPlaybackInfoAsync();
static void OnSessionsChanged();
static bool IsIgnoredMediaApp(const std::wstring& appUserModelId);
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
        for (int step=1; step<=count; ++step) {
            auto candidate = sessions.GetAt((currentIndex + step) % count);
            if (candidate && !IsIgnoredMediaApp(std::wstring(candidate.SourceAppUserModelId()))) {
                nextSession = candidate; break;
            }
        }
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
            EnumWindows([](HWND hwnd, LPARAM lParam) CALLBACK -> BOOL {
                const bool visible = IsWindowVisible(hwnd);
                // Tray-minimized players can hide their main window while
                // retaining the minimized state. Restore it if its app matches.
                if (!visible && !IsIconic(hwnd)) return TRUE;
                WINDOWINFO wi{};
                wi.cbSize = sizeof(wi);
                if (!GetWindowInfo(hwnd, &wi) ||
                    (wi.dwStyle & WS_CHILD) ||
                    (wi.dwExStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) ||
                    (GetWindow(hwnd, GW_OWNER) && !(wi.dwExStyle & WS_EX_APPWINDOW))) {
                    return TRUE;
                }
                DWORD cloaked = 0;
                if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
                    cloaked) {
                    return TRUE;
                }
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
                if (visible && !s->titleHwnd && !s->targetTitle.empty()) {
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
            if (g_unloading) return;
            if (targetWindow && IsWindow(targetWindow)) {
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
    if (!GetSourceConfig(ClassifyPlayerSource(appUserModelId)).enabled) return true;
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
static void FetchMediaPropertiesAsync() {
    SpawnTrackedWorker([]() {
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
                std::vector<BYTE> thumbBytes;
                uint64_t          thumbHash = 0;
                uint64_t          thumbStreamSize = 0;
                if (auto thumbRef = props.Thumbnail()) {
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
                                for (size_t i = 0; i < thumbBytes.size(); i += 1024)
                                    thumbHash = thumbHash * 31 + thumbBytes[i];
                            }
                        }
                    } catch (...) { thumbBytes.clear(); thumbStreamSize = 0; }
                }


                {
                    bool isBrowserSession = IsBrowserAumid(aumid);
                    std::wstring titleArtistKey =
                        std::wstring(props.Title()) + L"\x1f" + std::wstring(props.Artist());
                    bool suspectMatch = false;
                    {
                        std::lock_guard<std::mutex> lk(g_mediaMtx);
                        bool     trackChanged = (titleArtistKey != g_lastTitleArtistKey);
                        uint64_t prevArtSize  = g_media.thumbnailStreamSize;
                        uint64_t prevArtHash  = g_media.thumbnailHash;
                        g_lastTitleArtistKey = titleArtistKey;


                        if (g_artNewBrowserSession) {
                            if (thumbStreamSize > 0) {
                                g_suspectArtSize  = thumbStreamSize;
                                g_suspectArtHash  = thumbHash;
                                g_artDelayPending = true;
                                suspectMatch      = true;
                            }
                            g_artNewBrowserSession = false;
                        } else if (trackChanged) {
                            g_suspectArtSize  = isBrowserSession ? prevArtSize : 0;
                            g_suspectArtHash  = isBrowserSession ? prevArtHash : 0;
                            g_artDelayPending = isBrowserSession && prevArtSize > 0;
                        }


                        if (!suspectMatch && g_artDelayPending && isBrowserSession) {
                            bool matchesSuspect = thumbStreamSize > 0 &&
                                                  thumbStreamSize == g_suspectArtSize &&
                                                  (g_suspectArtHash == 0 || thumbHash == g_suspectArtHash);
                            if (matchesSuspect) {
                                suspectMatch = true;
                            } else {
                                g_artDelayPending = false;
                            }
                        }


                        if (!suspectMatch && isBrowserSession &&
                            thumbStreamSize == 0 && prevArtSize > 0) {
                            suspectMatch = true;
                        }
                    }
                    if (suspectMatch) {
                        thumbBytes.clear();
                        thumbHash = 0;
                        thumbStreamSize = 0;
                    }
                }
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
                if (g_settings.showAppIcon && (aumid != appIconKey || appIconBytes.empty() || forceIconRefresh || g_cachedAppIconSize != g_settings.appIconSize)) {
                    try {
                        appIconBytes = FetchAppIconBytes(aumid, g_settings.appIconSize);
                        appIconKey   = aumid;
                        g_cachedAppIconSize = g_settings.appIconSize;
                    } catch (...) {
                    }
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
                if (forceIconRefresh) {
                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                    g_userSwitchedSession = false;
                }
            } catch (...) {
            }
        } catch (...) {}
        DispatchMediaUpdate();
        winrt::uninit_apartment();
    });
}
static void FetchPlaybackInfoAsync() {
    SpawnTrackedWorker([]() {
        if (g_unloading) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            { std::lock_guard<std::mutex> lk(g_sessionMtx); session = g_currentSession; }
            if (session) {
                try {
                    auto info = session.GetPlaybackInfo();
                    if (!info) {
                        winrt::uninit_apartment();
                        return;
                    }
                    auto status = info.PlaybackStatus();
                    bool playing = (status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                    bool wasPlaying = false;
                    {
                        std::lock_guard<std::mutex> lk(g_mediaMtx);
                        wasPlaying = g_media.isPlaying;
                        g_media.isPlaying = playing;
                    }
                    if (playing != wasPlaying) {
                        if (!playing && wasPlaying) {
                            if (!g_unloading) OnSessionsChanged();
                        }
                    }
                    try {
                        auto shuffleRef = info.IsShuffleActive();
                        if (shuffleRef) {
                            g_shuffleEnabled = shuffleRef.Value();
                        }
                    } catch (...) {}
                    try {
                        auto repeatRef = info.AutoRepeatMode();
                        if (repeatRef) {
                            using RM = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
                            auto v = repeatRef.Value();
                            if (v == RM::Track) g_repeatMode = RepeatMode::One;
                            else if (v == RM::List) g_repeatMode = RepeatMode::All;
                            else g_repeatMode = RepeatMode::Off;
                        } else {
                            g_repeatMode = RepeatMode::Off;
                        }
                    } catch (...) {
                        g_repeatMode = RepeatMode::Off;
                    }
                    try {
                        auto controls = info.Controls();
                        if (controls) {
                            bool canPrev   = controls.IsPreviousEnabled();
                            bool canNext   = controls.IsNextEnabled();
                            bool canShuf   = controls.IsShuffleEnabled();
                            bool canRep    = controls.IsRepeatEnabled();
                            bool canSeek   = controls.IsPlaybackPositionEnabled();
                            {
                                std::lock_guard<std::mutex> lk(g_mediaMtx);
                                g_media.canSkipPrevious = canPrev;
                                g_media.canSkipNext     = canNext;
                                g_media.canShuffle      = canShuf;
                                g_media.canRepeat       = canRep;
                                g_media.canSeek         = canSeek;
                            }
                        }
                    } catch (...) {}
                } catch (...) {
                }
            }
        } catch (...) {}
        DispatchMediaUpdate();
        winrt::uninit_apartment();
    });
}
static void DetachCurrentSession() {
    std::lock_guard<std::mutex> lk(g_sessionMtx);
    if (!g_currentSession) return;
    try {
        if (g_evMediaProps.value) { g_currentSession.MediaPropertiesChanged(g_evMediaProps); g_evMediaProps = {}; }
        if (g_evPlayback.value)   { g_currentSession.PlaybackInfoChanged(g_evPlayback); g_evPlayback = {}; }
    } catch (...) {}
    g_currentSession = nullptr;
    g_repeatMode = RepeatMode::Off;
    g_shuffleEnabled = false;
    {
        std::lock_guard<std::mutex> lkm(g_mediaMtx);
        g_media.canSkipPrevious = true;
        g_media.canSkipNext     = true;
        g_media.canShuffle      = true;
        g_media.canRepeat       = true;
        g_media.canSeek         = true;
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
        int playingPriority = 1000, firstPriority = 1000, currentPriority = 1000;
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
                int priority = GetSourceConfig(ClassifyPlayerSource(appId)).priority;
                if (priority < firstPriority) { firstValid = s; firstPriority = priority; }
                if (s == current) {
                    currentExists  = true;
                    currentPlaying = isPlaying;
                    currentPriority = priority;
                }
                if (isPlaying && priority < playingPriority) {
                    playingFirst = s;
                    playingPriority = priority;
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
        if (currentExists && currentPlaying && currentPriority <= playingPriority) {
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
        DetachCurrentSession();
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
    ResetScrollState(g_titleScroll);
    ResetScrollState(g_artistScroll);
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        g_currentSession = session;
        try {
            g_evMediaProps = g_currentSession.MediaPropertiesChanged([](auto const&, auto const&) {
                if (!g_unloading) FetchMediaPropertiesAsync();
            });
            g_evPlayback = g_currentSession.PlaybackInfoChanged([](auto const&, auto const&) {
                if (!g_unloading) FetchPlaybackInfoAsync();
            });
        } catch (...) {
            Wh_Log(L"AttachToSession: Failed to attach event handlers");
            g_currentSession = nullptr;
            return;
        }
    }
fetch:
    FetchMediaPropertiesAsync();
    FetchPlaybackInfoAsync();
}
static void OnSessionsChanged() {
    if (g_unloading) return;
    bool forceRefresh = g_forceSessionRefresh.exchange(false);
    try {
        GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
        {
            std::lock_guard<std::mutex> lk(g_sessionMtx);
            mgr = g_sessionMgr;
        }
        if (mgr) {
            auto sessions = mgr.GetSessions();
            int count = 0;
            for (auto const& candidate : sessions) {
                try { if (candidate && !IsIgnoredMediaApp(std::wstring(candidate.SourceAppUserModelId()))) ++count; } catch (...) {}
            }
            g_sessionCount.store(count);
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
            OnSessionsChanged();
        });
        g_evCurrentChanged  = g_sessionMgr.CurrentSessionChanged([](auto const&, auto const&) {
            OnSessionsChanged();
        });
        OnSessionsChanged();
        if (WaitForSingleObject(g_mediaStopEvent, 2000) == WAIT_TIMEOUT) {
            if (!g_unloading) {
                g_forceSessionRefresh = true;
                OnSessionsChanged();
            }
        }
        // Detect playback/priority changes even when a player does not notify the manager.
        while (WaitForSingleObject(g_mediaStopEvent, 2000) == WAIT_TIMEOUT) {
            if (g_unloading) break;
            if (g_applyingSettings) continue;
            auto best = PickBestSession();
            bool changed;
            { std::lock_guard<std::mutex> lock(g_sessionMtx); changed = best != g_currentSession; }
            if (changed) AttachToSession(best);
        }
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
// QQ QRC cipher port adapted from ANYNC/TaskbarLyrics (MIT), Copyright (c) 2026 anync.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <setjmp.h>
namespace QqQrc {
/* puff.h
  Copyright (C) 2002-2013 Mark Adler, all rights reserved
  version 2.3, 21 Jan 2013

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Mark Adler    madler@alumni.caltech.edu
 */


/*
 * See puff.c for purpose and usage.
 */
#ifndef NIL
#  define NIL ((unsigned char *)0)      /* for no output option */
#endif

int puff(unsigned char *dest,           /* pointer to destination pointer */
         unsigned long *destlen,        /* amount of output space */
         const unsigned char *source,   /* pointer to source data pointer */
         unsigned long *sourcelen);     /* amount of input available */

/*
 * puff.c
 * Copyright (C) 2002-2013 Mark Adler
 * For conditions of distribution and use, see copyright notice in puff.h
 * version 2.3, 21 Jan 2013
 *
 * puff.c is a simple inflate written to be an unambiguous way to specify the
 * deflate format.  It is not written for speed but rather simplicity.  As a
 * side benefit, this code might actually be useful when small code is more
 * important than speed, such as bootstrap applications.  For typical deflate
 * data, zlib's inflate() is about four times as fast as puff().  zlib's
 * inflate compiles to around 20K on my machine, whereas puff.c compiles to
 * around 4K on my machine (a PowerPC using GNU cc).  If the faster decode()
 * function here is used, then puff() is only twice as slow as zlib's
 * inflate().
 *
 * All dynamically allocated memory comes from the stack.  The stack required
 * is less than 2K bytes.  This code is compatible with 16-bit int's and
 * assumes that long's are at least 32 bits.  puff.c uses the short data type,
 * assumed to be 16 bits, for arrays in order to conserve memory.  The code
 * works whether integers are stored big endian or little endian.
 *
 * In the comments below are "Format notes" that describe the inflate process
 * and document some of the less obvious aspects of the format.  This source
 * code is meant to supplement RFC 1951, which formally describes the deflate
 * format:
 *
 *    https://datatracker.ietf.org/doc/html/rfc1951
 */

/*
 * Change history:
 *
 * 1.0  10 Feb 2002     - First version
 * 1.1  17 Feb 2002     - Clarifications of some comments and notes
 *                      - Update puff() dest and source pointers on negative
 *                        errors to facilitate debugging deflators
 *                      - Remove longest from struct huffman -- not needed
 *                      - Simplify offs[] index in construct()
 *                      - Add input size and checking, using longjmp() to
 *                        maintain easy readability
 *                      - Use short data type for large arrays
 *                      - Use pointers instead of long to specify source and
 *                        destination sizes to avoid arbitrary 4 GB limits
 * 1.2  17 Mar 2002     - Add faster version of decode(), doubles speed (!),
 *                        but leave simple version for readability
 *                      - Make sure invalid distances detected if pointers
 *                        are 16 bits
 *                      - Fix fixed codes table error
 *                      - Provide a scanning mode for determining size of
 *                        uncompressed data
 * 1.3  20 Mar 2002     - Go back to lengths for puff() parameters [Gailly]
 *                      - Add a puff.h file for the interface
 *                      - Add braces in puff() for else do [Gailly]
 *                      - Use indexes instead of pointers for readability
 * 1.4  31 Mar 2002     - Simplify construct() code set check
 *                      - Fix some comments
 *                      - Add FIXLCODES #define
 * 1.5   6 Apr 2002     - Minor comment fixes
 * 1.6   7 Aug 2002     - Minor format changes
 * 1.7   3 Mar 2003     - Added test code for distribution
 *                      - Added zlib-like license
 * 1.8   9 Jan 2004     - Added some comments on no distance codes case
 * 1.9  21 Feb 2008     - Fix bug on 16-bit integer architectures [Pohland]
 *                      - Catch missing end-of-block symbol error
 * 2.0  25 Jul 2008     - Add #define to permit distance too far back
 *                      - Add option in TEST code for puff to write the data
 *                      - Add option in TEST code to skip input bytes
 *                      - Allow TEST code to read from piped stdin
 * 2.1   4 Apr 2010     - Avoid variable initialization for happier compilers
 *                      - Avoid unsigned comparisons for even happier compilers
 * 2.2  25 Apr 2010     - Fix bug in variable initializations [Oberhumer]
 *                      - Add const where appropriate [Oberhumer]
 *                      - Split if's and ?'s for coverage testing
 *                      - Break out test code to separate file
 *                      - Move NIL to puff.h
 *                      - Allow incomplete code only if single code length is 1
 *                      - Add full code coverage test to Makefile
 * 2.3  21 Jan 2013     - Check for invalid code length codes in dynamic blocks
 */

// setjmp.h is included outside the namespace.             /* for setjmp(), longjmp(), and jmp_buf */
// puff.h is embedded above.               /* prototype for puff() */

#define local static            /* for local function definitions */

/*
 * Maximums for allocations and loops.  It is not useful to change these --
 * they are fixed by the deflate format.
 */
#define MAXBITS 15              /* maximum bits in a code */
#define MAXLCODES 286           /* maximum number of literal/length codes */
#define MAXDCODES 30            /* maximum number of distance codes */
#define MAXCODES (MAXLCODES+MAXDCODES)  /* maximum codes lengths to read */
#define FIXLCODES 288           /* number of fixed literal/length codes */

/* input and output state */
struct state {
    /* output state */
    unsigned char *out;         /* output buffer */
    unsigned long outlen;       /* available space at out */
    unsigned long outcnt;       /* bytes written to out so far */

    /* input state */
    const unsigned char *in;    /* input buffer */
    unsigned long inlen;        /* available input at in */
    unsigned long incnt;        /* bytes read so far */
    int bitbuf;                 /* bit buffer */
    int bitcnt;                 /* number of bits in bit buffer */

    /* input limit error return state for bits() and decode() */
    jmp_buf env;
};

/*
 * Return need bits from the input stream.  This always leaves less than
 * eight bits in the buffer.  bits() works properly for need == 0.
 *
 * Format notes:
 *
 * - Bits are stored in bytes from the least significant bit to the most
 *   significant bit.  Therefore bits are dropped from the bottom of the bit
 *   buffer, using shift right, and new bytes are appended to the top of the
 *   bit buffer, using shift left.
 */
local int bits(struct state *s, int need)
{
    long val;           /* bit accumulator (can use up to 20 bits) */

    /* load at least need bits into val */
    val = s->bitbuf;
    while (s->bitcnt < need) {
        if (s->incnt == s->inlen)
            longjmp(s->env, 1);         /* out of input */
        val |= (long)(s->in[s->incnt++]) << s->bitcnt;  /* load eight bits */
        s->bitcnt += 8;
    }

    /* drop need bits and update buffer, always zero to seven bits left */
    s->bitbuf = (int)(val >> need);
    s->bitcnt -= need;

    /* return need bits, zeroing the bits above that */
    return (int)(val & ((1L << need) - 1));
}

/*
 * Process a stored block.
 *
 * Format notes:
 *
 * - After the two-bit stored block type (00), the stored block length and
 *   stored bytes are byte-aligned for fast copying.  Therefore any leftover
 *   bits in the byte that has the last bit of the type, as many as seven, are
 *   discarded.  The value of the discarded bits are not defined and should not
 *   be checked against any expectation.
 *
 * - The second inverted copy of the stored block length does not have to be
 *   checked, but it's probably a good idea to do so anyway.
 *
 * - A stored block can have zero length.  This is sometimes used to byte-align
 *   subsets of the compressed data for random access or partial recovery.
 */
local int stored(struct state *s)
{
    unsigned len;       /* length of stored block */

    /* discard leftover bits from current byte (assumes s->bitcnt < 8) */
    s->bitbuf = 0;
    s->bitcnt = 0;

    /* get length and check against its one's complement */
    if (s->incnt + 4 > s->inlen)
        return 2;                               /* not enough input */
    len = s->in[s->incnt++];
    len |= s->in[s->incnt++] << 8;
    if (s->in[s->incnt++] != (~len & 0xff) ||
        s->in[s->incnt++] != ((~len >> 8) & 0xff))
        return -2;                              /* didn't match complement! */

    /* copy len bytes from in to out */
    if (s->incnt + len > s->inlen)
        return 2;                               /* not enough input */
    if (s->out != NIL) {
        if (s->outcnt + len > s->outlen)
            return 1;                           /* not enough output space */
        while (len--)
            s->out[s->outcnt++] = s->in[s->incnt++];
    }
    else {                                      /* just scanning */
        s->outcnt += len;
        s->incnt += len;
    }

    /* done with a valid stored block */
    return 0;
}

/*
 * Huffman code decoding tables.  count[1..MAXBITS] is the number of symbols of
 * each length, which for a canonical code are stepped through in order.
 * symbol[] are the symbol values in canonical order, where the number of
 * entries is the sum of the counts in count[].  The decoding process can be
 * seen in the function decode() below.
 */
struct huffman {
    short *count;       /* number of symbols of each length */
    short *symbol;      /* canonically ordered symbols */
};

/*
 * Decode a code from the stream s using huffman table h.  Return the symbol or
 * a negative value if there is an error.  If all of the lengths are zero, i.e.
 * an empty code, or if the code is incomplete and an invalid code is received,
 * then -10 is returned after reading MAXBITS bits.
 *
 * Format notes:
 *
 * - The codes as stored in the compressed data are bit-reversed relative to
 *   a simple integer ordering of codes of the same lengths.  Hence below the
 *   bits are pulled from the compressed data one at a time and used to
 *   build the code value reversed from what is in the stream in order to
 *   permit simple integer comparisons for decoding.  A table-based decoding
 *   scheme (as used in zlib) does not need to do this reversal.
 *
 * - The first code for the shortest length is all zeros.  Subsequent codes of
 *   the same length are simply integer increments of the previous code.  When
 *   moving up a length, a zero bit is appended to the code.  For a complete
 *   code, the last code of the longest length will be all ones.
 *
 * - Incomplete codes are handled by this decoder, since they are permitted
 *   in the deflate format.  See the format notes for fixed() and dynamic().
 */
#ifdef SLOW
local int decode(struct state *s, const struct huffman *h)
{
    int len;            /* current number of bits in code */
    int code;           /* len bits being decoded */
    int first;          /* first code of length len */
    int count;          /* number of codes of length len */
    int index;          /* index of first code of length len in symbol table */

    code = first = index = 0;
    for (len = 1; len <= MAXBITS; len++) {
        code |= bits(s, 1);             /* get next bit */
        count = h->count[len];
        if (code - count < first)       /* if length len, return symbol */
            return h->symbol[index + (code - first)];
        index += count;                 /* else update for next length */
        first += count;
        first <<= 1;
        code <<= 1;
    }
    return -10;                         /* ran out of codes */
}

/*
 * A faster version of decode() for real applications of this code.   It's not
 * as readable, but it makes puff() twice as fast.  And it only makes the code
 * a few percent larger.
 */
#else /* !SLOW */
local int decode(struct state *s, const struct huffman *h)
{
    int len;            /* current number of bits in code */
    int code;           /* len bits being decoded */
    int first;          /* first code of length len */
    int count;          /* number of codes of length len */
    int index;          /* index of first code of length len in symbol table */
    int bitbuf;         /* bits from stream */
    int left;           /* bits left in next or left to process */
    short *next;        /* next number of codes */

    bitbuf = s->bitbuf;
    left = s->bitcnt;
    code = first = index = 0;
    len = 1;
    next = h->count + 1;
    while (1) {
        while (left--) {
            code |= bitbuf & 1;
            bitbuf >>= 1;
            count = *next++;
            if (code - count < first) { /* if length len, return symbol */
                s->bitbuf = bitbuf;
                s->bitcnt = (s->bitcnt - len) & 7;
                return h->symbol[index + (code - first)];
            }
            index += count;             /* else update for next length */
            first += count;
            first <<= 1;
            code <<= 1;
            len++;
        }
        left = (MAXBITS+1) - len;
        if (left == 0)
            break;
        if (s->incnt == s->inlen)
            longjmp(s->env, 1);         /* out of input */
        bitbuf = s->in[s->incnt++];
        if (left > 8)
            left = 8;
    }
    return -10;                         /* ran out of codes */
}
#endif /* SLOW */

/*
 * Given the list of code lengths length[0..n-1] representing a canonical
 * Huffman code for n symbols, construct the tables required to decode those
 * codes.  Those tables are the number of codes of each length, and the symbols
 * sorted by length, retaining their original order within each length.  The
 * return value is zero for a complete code set, negative for an over-
 * subscribed code set, and positive for an incomplete code set.  The tables
 * can be used if the return value is zero or positive, but they cannot be used
 * if the return value is negative.  If the return value is zero, it is not
 * possible for decode() using that table to return an error--any stream of
 * enough bits will resolve to a symbol.  If the return value is positive, then
 * it is possible for decode() using that table to return an error for received
 * codes past the end of the incomplete lengths.
 *
 * Not used by decode(), but used for error checking, h->count[0] is the number
 * of the n symbols not in the code.  So n - h->count[0] is the number of
 * codes.  This is useful for checking for incomplete codes that have more than
 * one symbol, which is an error in a dynamic block.
 *
 * Assumption: for all i in 0..n-1, 0 <= length[i] <= MAXBITS
 * This is assured by the construction of the length arrays in dynamic() and
 * fixed() and is not verified by construct().
 *
 * Format notes:
 *
 * - Permitted and expected examples of incomplete codes are one of the fixed
 *   codes and any code with a single symbol which in deflate is coded as one
 *   bit instead of zero bits.  See the format notes for fixed() and dynamic().
 *
 * - Within a given code length, the symbols are kept in ascending order for
 *   the code bits definition.
 */
local int construct(struct huffman *h, const short *length, int n)
{
    int symbol;         /* current symbol when stepping through length[] */
    int len;            /* current length when stepping through h->count[] */
    int left;           /* number of possible codes left of current length */
    short offs[MAXBITS+1];      /* offsets in symbol table for each length */

    /* count number of codes of each length */
    for (len = 0; len <= MAXBITS; len++)
        h->count[len] = 0;
    for (symbol = 0; symbol < n; symbol++)
        (h->count[length[symbol]])++;   /* assumes lengths are within bounds */
    if (h->count[0] == n)               /* no codes! */
        return 0;                       /* complete, but decode() will fail */

    /* check for an over-subscribed or incomplete set of lengths */
    left = 1;                           /* one possible code of zero length */
    for (len = 1; len <= MAXBITS; len++) {
        left <<= 1;                     /* one more bit, double codes left */
        left -= h->count[len];          /* deduct count from possible codes */
        if (left < 0)
            return left;                /* over-subscribed--return negative */
    }                                   /* left > 0 means incomplete */

    /* generate offsets into symbol table for each length for sorting */
    offs[1] = 0;
    for (len = 1; len < MAXBITS; len++)
        offs[len + 1] = offs[len] + h->count[len];

    /*
     * put symbols in table sorted by length, by symbol order within each
     * length
     */
    for (symbol = 0; symbol < n; symbol++)
        if (length[symbol] != 0)
            h->symbol[offs[length[symbol]]++] = symbol;

    /* return zero for complete set, positive for incomplete set */
    return left;
}

/*
 * Decode literal/length and distance codes until an end-of-block code.
 *
 * Format notes:
 *
 * - Compressed data that is after the block type if fixed or after the code
 *   description if dynamic is a combination of literals and length/distance
 *   pairs terminated by and end-of-block code.  Literals are simply Huffman
 *   coded bytes.  A length/distance pair is a coded length followed by a
 *   coded distance to represent a string that occurs earlier in the
 *   uncompressed data that occurs again at the current location.
 *
 * - Literals, lengths, and the end-of-block code are combined into a single
 *   code of up to 286 symbols.  They are 256 literals (0..255), 29 length
 *   symbols (257..285), and the end-of-block symbol (256).
 *
 * - There are 256 possible lengths (3..258), and so 29 symbols are not enough
 *   to represent all of those.  Lengths 3..10 and 258 are in fact represented
 *   by just a length symbol.  Lengths 11..257 are represented as a symbol and
 *   some number of extra bits that are added as an integer to the base length
 *   of the length symbol.  The number of extra bits is determined by the base
 *   length symbol.  These are in the static arrays below, lens[] for the base
 *   lengths and lext[] for the corresponding number of extra bits.
 *
 * - The reason that 258 gets its own symbol is that the longest length is used
 *   often in highly redundant files.  Note that 258 can also be coded as the
 *   base value 227 plus the maximum extra value of 31.  While a good deflate
 *   should never do this, it is not an error, and should be decoded properly.
 *
 * - If a length is decoded, including its extra bits if any, then it is
 *   followed a distance code.  There are up to 30 distance symbols.  Again
 *   there are many more possible distances (1..32768), so extra bits are added
 *   to a base value represented by the symbol.  The distances 1..4 get their
 *   own symbol, but the rest require extra bits.  The base distances and
 *   corresponding number of extra bits are below in the static arrays dist[]
 *   and dext[].
 *
 * - Literal bytes are simply written to the output.  A length/distance pair is
 *   an instruction to copy previously uncompressed bytes to the output.  The
 *   copy is from distance bytes back in the output stream, copying for length
 *   bytes.
 *
 * - Distances pointing before the beginning of the output data are not
 *   permitted.
 *
 * - Overlapped copies, where the length is greater than the distance, are
 *   allowed and common.  For example, a distance of one and a length of 258
 *   simply copies the last byte 258 times.  A distance of four and a length of
 *   twelve copies the last four bytes three times.  A simple forward copy
 *   ignoring whether the length is greater than the distance or not implements
 *   this correctly.  You should not use memcpy() since its behavior is not
 *   defined for overlapped arrays.  You should not use memmove() or bcopy()
 *   since though their behavior -is- defined for overlapping arrays, it is
 *   defined to do the wrong thing in this case.
 */
local int codes(struct state *s,
                const struct huffman *lencode,
                const struct huffman *distcode)
{
    int symbol;         /* decoded symbol */
    int len;            /* length for copy */
    unsigned dist;      /* distance for copy */
    static const short lens[29] = { /* Size base for length codes 257..285 */
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
    static const short lext[29] = { /* Extra bits for length codes 257..285 */
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    static const short dists[30] = { /* Offset base for distance codes 0..29 */
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
    static const short dext[30] = { /* Extra bits for distance codes 0..29 */
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};

    /* decode literals and length/distance pairs */
    do {
        symbol = decode(s, lencode);
        if (symbol < 0)
            return symbol;              /* invalid symbol */
        if (symbol < 256) {             /* literal: symbol is the byte */
            /* write out the literal */
            if (s->out != NIL) {
                if (s->outcnt == s->outlen)
                    return 1;
                s->out[s->outcnt] = symbol;
            }
            s->outcnt++;
        }
        else if (symbol > 256) {        /* length */
            /* get and compute length */
            symbol -= 257;
            if (symbol >= 29)
                return -10;             /* invalid fixed code */
            len = lens[symbol] + bits(s, lext[symbol]);

            /* get and check distance */
            symbol = decode(s, distcode);
            if (symbol < 0)
                return symbol;          /* invalid symbol */
            dist = dists[symbol] + bits(s, dext[symbol]);
#ifndef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR
            if (dist > s->outcnt)
                return -11;     /* distance too far back */
#endif

            /* copy length bytes from distance bytes back */
            if (s->out != NIL) {
                if (s->outcnt + len > s->outlen)
                    return 1;
                while (len--) {
                    s->out[s->outcnt] =
#ifdef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR
                        dist > s->outcnt ?
                            0 :
#endif
                            s->out[s->outcnt - dist];
                    s->outcnt++;
                }
            }
            else
                s->outcnt += len;
        }
    } while (symbol != 256);            /* end of block symbol */

    /* done with a valid fixed or dynamic block */
    return 0;
}

/*
 * Process a fixed codes block.
 *
 * Format notes:
 *
 * - This block type can be useful for compressing small amounts of data for
 *   which the size of the code descriptions in a dynamic block exceeds the
 *   benefit of custom codes for that block.  For fixed codes, no bits are
 *   spent on code descriptions.  Instead the code lengths for literal/length
 *   codes and distance codes are fixed.  The specific lengths for each symbol
 *   can be seen in the "for" loops below.
 *
 * - The literal/length code is complete, but has two symbols that are invalid
 *   and should result in an error if received.  This cannot be implemented
 *   simply as an incomplete code since those two symbols are in the "middle"
 *   of the code.  They are eight bits long and the longest literal/length\
 *   code is nine bits.  Therefore the code must be constructed with those
 *   symbols, and the invalid symbols must be detected after decoding.
 *
 * - The fixed distance codes also have two invalid symbols that should result
 *   in an error if received.  Since all of the distance codes are the same
 *   length, this can be implemented as an incomplete code.  Then the invalid
 *   codes are detected while decoding.
 */
local int fixed(struct state *s)
{
    static int virgin = 1;
    static short lencnt[MAXBITS+1], lensym[FIXLCODES];
    static short distcnt[MAXBITS+1], distsym[MAXDCODES];
    static struct huffman lencode, distcode;

    /* build fixed huffman tables if first call (may not be thread safe) */
    if (virgin) {
        int symbol;
        short lengths[FIXLCODES];

        /* construct lencode and distcode */
        lencode.count = lencnt;
        lencode.symbol = lensym;
        distcode.count = distcnt;
        distcode.symbol = distsym;

        /* literal/length table */
        for (symbol = 0; symbol < 144; symbol++)
            lengths[symbol] = 8;
        for (; symbol < 256; symbol++)
            lengths[symbol] = 9;
        for (; symbol < 280; symbol++)
            lengths[symbol] = 7;
        for (; symbol < FIXLCODES; symbol++)
            lengths[symbol] = 8;
        construct(&lencode, lengths, FIXLCODES);

        /* distance table */
        for (symbol = 0; symbol < MAXDCODES; symbol++)
            lengths[symbol] = 5;
        construct(&distcode, lengths, MAXDCODES);

        /* do this just once */
        virgin = 0;
    }

    /* decode data until end-of-block code */
    return codes(s, &lencode, &distcode);
}

/*
 * Process a dynamic codes block.
 *
 * Format notes:
 *
 * - A dynamic block starts with a description of the literal/length and
 *   distance codes for that block.  New dynamic blocks allow the compressor to
 *   rapidly adapt to changing data with new codes optimized for that data.
 *
 * - The codes used by the deflate format are "canonical", which means that
 *   the actual bits of the codes are generated in an unambiguous way simply
 *   from the number of bits in each code.  Therefore the code descriptions
 *   are simply a list of code lengths for each symbol.
 *
 * - The code lengths are stored in order for the symbols, so lengths are
 *   provided for each of the literal/length symbols, and for each of the
 *   distance symbols.
 *
 * - If a symbol is not used in the block, this is represented by a zero as the
 *   code length.  This does not mean a zero-length code, but rather that no
 *   code should be created for this symbol.  There is no way in the deflate
 *   format to represent a zero-length code.
 *
 * - The maximum number of bits in a code is 15, so the possible lengths for
 *   any code are 1..15.
 *
 * - The fact that a length of zero is not permitted for a code has an
 *   interesting consequence.  Normally if only one symbol is used for a given
 *   code, then in fact that code could be represented with zero bits.  However
 *   in deflate, that code has to be at least one bit.  So for example, if
 *   only a single distance base symbol appears in a block, then it will be
 *   represented by a single code of length one, in particular one 0 bit.  This
 *   is an incomplete code, since if a 1 bit is received, it has no meaning,
 *   and should result in an error.  So incomplete distance codes of one symbol
 *   should be permitted, and the receipt of invalid codes should be handled.
 *
 * - It is also possible to have a single literal/length code, but that code
 *   must be the end-of-block code, since every dynamic block has one.  This
 *   is not the most efficient way to create an empty block (an empty fixed
 *   block is fewer bits), but it is allowed by the format.  So incomplete
 *   literal/length codes of one symbol should also be permitted.
 *
 * - If there are only literal codes and no lengths, then there are no distance
 *   codes.  This is represented by one distance code with zero bits.
 *
 * - The list of up to 286 length/literal lengths and up to 30 distance lengths
 *   are themselves compressed using Huffman codes and run-length encoding.  In
 *   the list of code lengths, a 0 symbol means no code, a 1..15 symbol means
 *   that length, and the symbols 16, 17, and 18 are run-length instructions.
 *   Each of 16, 17, and 18 are followed by extra bits to define the length of
 *   the run.  16 copies the last length 3 to 6 times.  17 represents 3 to 10
 *   zero lengths, and 18 represents 11 to 138 zero lengths.  Unused symbols
 *   are common, hence the special coding for zero lengths.
 *
 * - The symbols for 0..18 are Huffman coded, and so that code must be
 *   described first.  This is simply a sequence of up to 19 three-bit values
 *   representing no code (0) or the code length for that symbol (1..7).
 *
 * - A dynamic block starts with three fixed-size counts from which is computed
 *   the number of literal/length code lengths, the number of distance code
 *   lengths, and the number of code length code lengths (ok, you come up with
 *   a better name!) in the code descriptions.  For the literal/length and
 *   distance codes, lengths after those provided are considered zero, i.e. no
 *   code.  The code length code lengths are received in a permuted order (see
 *   the order[] array below) to make a short code length code length list more
 *   likely.  As it turns out, very short and very long codes are less likely
 *   to be seen in a dynamic code description, hence what may appear initially
 *   to be a peculiar ordering.
 *
 * - Given the number of literal/length code lengths (nlen) and distance code
 *   lengths (ndist), then they are treated as one long list of nlen + ndist
 *   code lengths.  Therefore run-length coding can and often does cross the
 *   boundary between the two sets of lengths.
 *
 * - So to summarize, the code description at the start of a dynamic block is
 *   three counts for the number of code lengths for the literal/length codes,
 *   the distance codes, and the code length codes.  This is followed by the
 *   code length code lengths, three bits each.  This is used to construct the
 *   code length code which is used to read the remainder of the lengths.  Then
 *   the literal/length code lengths and distance lengths are read as a single
 *   set of lengths using the code length codes.  Codes are constructed from
 *   the resulting two sets of lengths, and then finally you can start
 *   decoding actual compressed data in the block.
 *
 * - For reference, a "typical" size for the code description in a dynamic
 *   block is around 80 bytes.
 */
local int dynamic(struct state *s)
{
    int nlen, ndist, ncode;             /* number of lengths in descriptor */
    int index;                          /* index of lengths[] */
    int err;                            /* construct() return value */
    short lengths[MAXCODES];            /* descriptor code lengths */
    short lencnt[MAXBITS+1], lensym[MAXLCODES];         /* lencode memory */
    short distcnt[MAXBITS+1], distsym[MAXDCODES];       /* distcode memory */
    struct huffman lencode, distcode;   /* length and distance codes */
    static const short order[19] =      /* permutation of code length codes */
        {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    /* construct lencode and distcode */
    lencode.count = lencnt;
    lencode.symbol = lensym;
    distcode.count = distcnt;
    distcode.symbol = distsym;

    /* get number of lengths in each table, check lengths */
    nlen = bits(s, 5) + 257;
    ndist = bits(s, 5) + 1;
    ncode = bits(s, 4) + 4;
    if (nlen > MAXLCODES || ndist > MAXDCODES)
        return -3;                      /* bad counts */

    /* read code length code lengths (really), missing lengths are zero */
    for (index = 0; index < ncode; index++)
        lengths[order[index]] = bits(s, 3);
    for (; index < 19; index++)
        lengths[order[index]] = 0;

    /* build huffman table for code lengths codes (use lencode temporarily) */
    err = construct(&lencode, lengths, 19);
    if (err != 0)               /* require complete code set here */
        return -4;

    /* read length/literal and distance code length tables */
    index = 0;
    while (index < nlen + ndist) {
        int symbol;             /* decoded value */
        int len;                /* last length to repeat */

        symbol = decode(s, &lencode);
        if (symbol < 0)
            return symbol;          /* invalid symbol */
        if (symbol < 16)                /* length in 0..15 */
            lengths[index++] = symbol;
        else {                          /* repeat instruction */
            len = 0;                    /* assume repeating zeros */
            if (symbol == 16) {         /* repeat last length 3..6 times */
                if (index == 0)
                    return -5;          /* no last length! */
                len = lengths[index - 1];       /* last length */
                symbol = 3 + bits(s, 2);
            }
            else if (symbol == 17)      /* repeat zero 3..10 times */
                symbol = 3 + bits(s, 3);
            else                        /* == 18, repeat zero 11..138 times */
                symbol = 11 + bits(s, 7);
            if (index + symbol > nlen + ndist)
                return -6;              /* too many lengths! */
            while (symbol--)            /* repeat last or zero symbol times */
                lengths[index++] = len;
        }
    }

    /* check for end-of-block code -- there better be one! */
    if (lengths[256] == 0)
        return -9;

    /* build huffman table for literal/length codes */
    err = construct(&lencode, lengths, nlen);
    if (err && (err < 0 || nlen != lencode.count[0] + lencode.count[1]))
        return -7;      /* incomplete code ok only for single length 1 code */

    /* build huffman table for distance codes */
    err = construct(&distcode, lengths + nlen, ndist);
    if (err && (err < 0 || ndist != distcode.count[0] + distcode.count[1]))
        return -8;      /* incomplete code ok only for single length 1 code */

    /* decode data until end-of-block code */
    return codes(s, &lencode, &distcode);
}

/*
 * Inflate source to dest.  On return, destlen and sourcelen are updated to the
 * size of the uncompressed data and the size of the deflate data respectively.
 * On success, the return value of puff() is zero.  If there is an error in the
 * source data, i.e. it is not in the deflate format, then a negative value is
 * returned.  If there is not enough input available or there is not enough
 * output space, then a positive error is returned.  In that case, destlen and
 * sourcelen are not updated to facilitate retrying from the beginning with the
 * provision of more input data or more output space.  In the case of invalid
 * inflate data (a negative error), the dest and source pointers are updated to
 * facilitate the debugging of deflators.
 *
 * puff() also has a mode to determine the size of the uncompressed output with
 * no output written.  For this dest must be (unsigned char *)0.  In this case,
 * the input value of *destlen is ignored, and on return *destlen is set to the
 * size of the uncompressed output.
 *
 * The return codes are:
 *
 *   2:  available inflate data did not terminate
 *   1:  output space exhausted before completing inflate
 *   0:  successful inflate
 *  -1:  invalid block type (type == 3)
 *  -2:  stored block length did not match one's complement
 *  -3:  dynamic block code description: too many length or distance codes
 *  -4:  dynamic block code description: code lengths codes incomplete
 *  -5:  dynamic block code description: repeat lengths with no first length
 *  -6:  dynamic block code description: repeat more than specified lengths
 *  -7:  dynamic block code description: invalid literal/length code lengths
 *  -8:  dynamic block code description: invalid distance code lengths
 *  -9:  dynamic block code description: missing end-of-block code
 * -10:  invalid literal/length or distance code in fixed or dynamic block
 * -11:  distance is too far back in fixed or dynamic block
 *
 * Format notes:
 *
 * - Three bits are read for each block to determine the kind of block and
 *   whether or not it is the last block.  Then the block is decoded and the
 *   process repeated if it was not the last block.
 *
 * - The leftover bits in the last byte of the deflate data after the last
 *   block (if it was a fixed or dynamic block) are undefined and have no
 *   expected values to check.
 */
int puff(unsigned char *dest,           /* pointer to destination pointer */
         unsigned long *destlen,        /* amount of output space */
         const unsigned char *source,   /* pointer to source data pointer */
         unsigned long *sourcelen)      /* amount of input available */
{
    struct state s;             /* input/output state */
    int last, type;             /* block information */
    int err;                    /* return value */

    /* initialize output state */
    s.out = dest;
    s.outlen = *destlen;                /* ignored if dest is NIL */
    s.outcnt = 0;

    /* initialize input state */
    s.in = source;
    s.inlen = *sourcelen;
    s.incnt = 0;
    s.bitbuf = 0;
    s.bitcnt = 0;

    /* return if bits() or decode() tries to read past available input */
    if (setjmp(s.env) != 0)             /* if came back here via longjmp() */
        err = 2;                        /* then skip do-loop, return error */
    else {
        /* process blocks until last block or error */
        do {
            last = bits(&s, 1);         /* one if last block */
            type = bits(&s, 2);         /* block type 0..3 */
            err = type == 0 ?
                    stored(&s) :
                    (type == 1 ?
                        fixed(&s) :
                        (type == 2 ?
                            dynamic(&s) :
                            -1));       /* type == 3, invalid */
            if (err != 0)
                break;                  /* return with error */
        } while (!last);
    }

    /* update the lengths and return */
    if (err <= 0) {
        *destlen = s.outcnt;
        *sourcelen = s.incnt;
    }
    return err;
}

#undef local
#undef MAXBITS
#undef MAXLCODES
#undef MAXDCODES
#undef MAXCODES
#undef FIXLCODES
#undef NIL
static constexpr uint32_t box1[] = {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13};
static constexpr uint32_t box2[] = {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,3,13,4,7,15,2,8,15,12,0,1,10,6,9,11,5,0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9};
static constexpr uint32_t box3[] = {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12};
static constexpr uint32_t box4[] = {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,3,15,0,6,10,10,13,8,9,4,5,11,12,7,2,14};
static constexpr uint32_t box5[] = {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3};
static constexpr uint32_t box6[] = {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13};
static constexpr uint32_t box7[] = {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12};
static constexpr uint32_t box8[] = {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11};
static constexpr uint32_t KEY_RND_SHIFT[] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
static constexpr uint32_t KEY_PERM_C[] = {56,48,40,32,24,16,8,0,57,49,41,33,25,17,9,1,58,50,42,34,26,18,10,2,59,51,43,35};
static constexpr uint32_t KEY_PERM_D[] = {62,54,46,38,30,22,14,6,61,53,45,37,29,21,13,5,60,52,44,36,28,20,12,4,27,19,11,3};
static constexpr uint32_t KEY_COMPRESSION[] = {13,16,10,23,0,4,2,27,14,5,20,9,22,18,11,3,25,7,15,6,26,19,12,1,40,51,30,36,46,54,29,39,50,44,32,47,43,48,38,55,33,52,45,41,49,35,28,31};
static uint32_t gba(const uint8_t* a, int b, int c) {
    return uint32_t((a[(b / 32) * 4 + 3 - (b % 32) / 8] >> (7 - b % 8)) & 1) << c;
}
static uint32_t gbR(uint32_t a, int b, int c) { return ((a >> (31 - b)) & 1u) << c; }
static uint32_t gbL(uint32_t a, int b, int c) { return ((a << b) & 0x80000000u) >> c; }
static uint32_t fsb(uint32_t a) { return (a & 0x20) | ((a & 0x1f) >> 1) | ((a & 1) << 4); }
static void keySchedule(const uint8_t* key, int offset, uint8_t schedule[16][6], bool encrypt) {
    uint32_t c = 0, d = 0;
    for (int i = 0; i < 28; ++i) { c |= gba(key, KEY_PERM_C[i] + offset * 8, 31-i); d |= gba(key, KEY_PERM_D[i] + offset * 8, 31-i); }
    for (int i = 0; i < 16; ++i) {
        c = ((c << KEY_RND_SHIFT[i]) | (c >> (28-KEY_RND_SHIFT[i]))) & 0xfffffff0u;
        d = ((d << KEY_RND_SHIFT[i]) | (d >> (28-KEY_RND_SHIFT[i]))) & 0xfffffff0u;
        int target = encrypt ? i : 15-i;
        for (int k = 0; k < 6; ++k) schedule[target][k] = 0;
        for (int k = 0; k < 24; ++k) schedule[target][k/8] |= gbR(c,KEY_COMPRESSION[k],7-k%8);
        for (int k = 24; k < 48; ++k) schedule[target][k/8] |= gbR(d,KEY_COMPRESSION[k]-27,7-k%8);
    }
}
static void initialPermutation(uint32_t* state, const uint8_t* input){
  state[0]=gba(input,57,31)|gba(input,49,30)|gba(input,41,29)|gba(input,33,28)|gba(input,25,27)|gba(input,17,26)|gba(input,9,25)|gba(input,1,24)|gba(input,59,23)|gba(input,51,22)|gba(input,43,21)|gba(input,35,20)|gba(input,27,19)|gba(input,19,18)|gba(input,11,17)|gba(input,3,16)|gba(input,61,15)|gba(input,53,14)|gba(input,45,13)|gba(input,37,12)|gba(input,29,11)|gba(input,21,10)|gba(input,13,9)|gba(input,5,8)|gba(input,63,7)|gba(input,55,6)|gba(input,47,5)|gba(input,39,4)|gba(input,31,3)|gba(input,23,2)|gba(input,15,1)|gba(input,7,0);
  state[1]=gba(input,56,31)|gba(input,48,30)|gba(input,40,29)|gba(input,32,28)|gba(input,24,27)|gba(input,16,26)|gba(input,8,25)|gba(input,0,24)|gba(input,58,23)|gba(input,50,22)|gba(input,42,21)|gba(input,34,20)|gba(input,26,19)|gba(input,18,18)|gba(input,10,17)|gba(input,2,16)|gba(input,60,15)|gba(input,52,14)|gba(input,44,13)|gba(input,36,12)|gba(input,28,11)|gba(input,20,10)|gba(input,12,9)|gba(input,4,8)|gba(input,62,7)|gba(input,54,6)|gba(input,46,5)|gba(input,38,4)|gba(input,30,3)|gba(input,22,2)|gba(input,14,1)|gba(input,6,0);
}

static void inverseInitialPermutation(const uint32_t* state, uint8_t* output) {
    constexpr int oi[] = {3,2,1,0,7,6,5,4}, bo[] = {7,6,5,4,3,2,1,0};
    for (int i=0;i<8;++i) output[oi[i]] = gbR(state[1],bo[i],7)|gbR(state[0],bo[i],6)|gbR(state[1],bo[i]+8,5)|gbR(state[0],bo[i]+8,4)|gbR(state[1],bo[i]+16,3)|gbR(state[0],bo[i]+16,2)|gbR(state[1],bo[i]+24,1)|gbR(state[0],bo[i]+24,0);
}
static uint32_t f(uint32_t stateIn, const uint8_t* key){
  uint32_t t1=gbL(stateIn,31,0)|((stateIn&-0x10000000)>>1)|gbL(stateIn,4,5)|gbL(stateIn,3,6)|((stateIn&0x0f000000)>>3)|gbL(stateIn,8,11)|gbL(stateIn,7,12)|((stateIn&0x00f00000)>>5)|gbL(stateIn,12,17)|gbL(stateIn,11,18)|((stateIn&0x000f0000)>>7)|gbL(stateIn,16,23);
  uint32_t t2=gbL(stateIn,15,0)|((stateIn&0x0000f000)<<15)|gbL(stateIn,20,5)|gbL(stateIn,19,6)|((stateIn&0x00000f00)<<13)|gbL(stateIn,24,11)|gbL(stateIn,23,12)|((stateIn&0x000000f0)<<11)|gbL(stateIn,28,17)|gbL(stateIn,27,18)|((stateIn&0x0000000f)<<9)|gbL(stateIn,0,23);
  uint32_t x0=((t1>>24)&0xFF)^(key[0]&0xFF),x1=((t1>>16)&0xFF)^(key[1]&0xFF),x2=((t1>>8)&0xFF)^(key[2]&0xFF);
  uint32_t x3=((t2>>24)&0xFF)^(key[3]&0xFF),x4=((t2>>16)&0xFF)^(key[4]&0xFF),x5=((t2>>8)&0xFF)^(key[5]&0xFF);
  uint32_t s=(box1[fsb(x0>>2)]<<28)|(box2[fsb(((x0&3)<<4)|(x1>>4))]<<24)|(box3[fsb(((x1&0x0f)<<2)|(x2>>6))]<<20)|(box4[fsb(x2&0x3f)]<<16)|(box5[fsb(x3>>2)]<<12)|(box6[fsb(((x3&3)<<4)|(x4>>4))]<<8)|(box7[fsb(((x4&0x0f)<<2)|(x5>>6))]<<4)|box8[fsb(x5&0x3f)];
  return gbL(s,15,0)|gbL(s,6,1)|gbL(s,19,2)|gbL(s,20,3)|gbL(s,28,4)|gbL(s,11,5)|gbL(s,27,6)|gbL(s,16,7)|gbL(s,0,8)|gbL(s,14,9)|gbL(s,22,10)|gbL(s,25,11)|gbL(s,4,12)|gbL(s,17,13)|gbL(s,30,14)|gbL(s,9,15)|gbL(s,1,16)|gbL(s,7,17)|gbL(s,23,18)|gbL(s,13,19)|gbL(s,31,20)|gbL(s,26,21)|gbL(s,2,22)|gbL(s,8,23)|gbL(s,18,24)|gbL(s,12,25)|gbL(s,29,26)|gbL(s,5,27)|gbL(s,21,28)|gbL(s,10,29)|gbL(s,3,30)|gbL(s,24,31);
}

static void crypt(const uint8_t* input, uint8_t* output, const uint8_t key[16][6]) {
    uint32_t state[2]{}; initialPermutation(state,input);
    for (int i=0;i<15;++i) { uint32_t t=state[1]; state[1]=f(state[1],key[i])^state[0]; state[0]=t; }
    state[0]=f(state[1],key[15])^state[0]; inverseInitialPermutation(state,output);
}
static std::string Decode(const std::wstring& hex) {
    if (hex.empty() || hex.size() % 16 || hex.size() > 4*1024*1024) return {};
    std::vector<uint8_t> bytes(hex.size()/2);
    auto digit=[](wchar_t c)->int { if(c>=L'0'&&c<=L'9')return c-L'0'; if(c>=L'a'&&c<=L'f')return c-L'a'+10; if(c>=L'A'&&c<=L'F')return c-L'A'+10; return -1; };
    for(size_t i=0;i<bytes.size();++i) { int a=digit(hex[i*2]), b=digit(hex[i*2+1]); if(a<0||b<0)return {}; bytes[i]=(a<<4)|b; }
    uint8_t schedule[3][16][6]{};
    const uint8_t key[] = "!@#)(*$%123ZXC!@!@#)(NHL";
    keySchedule(key,16,schedule[0],false); keySchedule(key,8,schedule[1],true); keySchedule(key,0,schedule[2],false);
    for (size_t i = 0; i < bytes.size(); i += 8) {
        crypt(bytes.data() + i, bytes.data() + i, schedule[0]);
        crypt(bytes.data() + i, bytes.data() + i, schedule[1]);
        crypt(bytes.data() + i, bytes.data() + i, schedule[2]);
    }
    if(bytes.size()<6 || (bytes[0]&15)!=8 || ((bytes[0]<<8)+bytes[1])%31 || (bytes[1]&32)) return {};
    std::string text(2*1024*1024,'\0');
    unsigned long outputSize=(unsigned long)text.size(), inputSize=(unsigned long)bytes.size()-2;
    if(puff((unsigned char*)text.data(),&outputSize,bytes.data()+2,&inputSize)!=0) return {};
    // Verify zlib Adler-32 at the actual end of DEFLATE, ignoring cipher padding.
    if(inputSize+6>bytes.size())return {};
    uint32_t a=1,b=0; for(unsigned long i=0;i<outputSize;++i){a=(a+(uint8_t)text[i])%65521;b=(b+a)%65521;}
    size_t p=inputSize+2;
    uint32_t expected=(uint32_t(bytes[p])<<24)|(uint32_t(bytes[p+1])<<16)|(uint32_t(bytes[p+2])<<8)|bytes[p+3];
    if(((b<<16)|a)!=expected)return {};
    text.resize(outputSize); return text;
}
} // namespace QqQrc
struct LyricWord { int64_t ms, duration; size_t start, length; };
struct LyricLine { int64_t ms; std::wstring text; std::vector<LyricWord> words; int64_t duration = 0; };
struct LyricFrame {
    std::wstring title, artist, app, text, next;
    std::vector<LyricWord> words;
    int64_t position = 0;
    double rate = 0;
    uint64_t sampledAt = 0;
    bool hasLyrics = false;
};
static LyricFrame g_lyricFrame; // Protected by g_lyricsMtx.
static double LyricHighlightWidth(const std::vector<LyricWord>& words, double position, const std::vector<double>& widths) {
    double result = 0;
    for (const auto& word : words) {
        if (word.start >= widths.size() || word.length >= widths.size() - word.start || position < word.ms) continue;
        double fraction = word.duration > 0 ? std::clamp((position - word.ms) / word.duration, 0.0, 1.0) : 1.0;
        double start = widths[word.start], end = widths[word.start + word.length];
        result = std::max(result, start + (end - start) * fraction);
    }
    return result;
}
static std::vector<LyricLine> ParseLyrics(const std::wstring& lrc) {
    std::vector<LyricLine> result;
    std::wistringstream input(lrc);
    std::wstring line;
    int64_t offset = 0;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (!line.empty() && line.front() == 0xFEFF) line.erase(0, 1);
        if (line.compare(0, 8, L"[offset:") == 0) {
            try { offset = std::clamp(std::stoll(line.substr(8)), -600000LL, 600000LL); } catch (...) {}
            continue;
        }
        size_t pos = 0;
        std::vector<int64_t> times;
        while (pos < line.size() && line[pos] == L'[') {
            size_t end = line.find(L']', pos);
            if (end == std::wstring::npos) break;
            std::wstring tag = line.substr(pos + 1, end - pos - 1);
            size_t colon = tag.find(L':');
            if (colon == std::wstring::npos || colon == 0) break;
            bool valid = true;
            for (size_t i = 0; i < tag.size(); ++i)
                if (!(tag[i] >= L'0' && tag[i] <= L'9') && i != colon && tag[i] != L'.') valid = false;
            if (!valid) break;
            try {
                size_t consumed = 0;
                double seconds = std::stod(tag.substr(colon + 1), &consumed);
                int minutes = std::stoi(tag.substr(0, colon));
                if (consumed != tag.size() - colon - 1 || seconds >= 60 || minutes > 1440) break;
                times.push_back(minutes * 60000LL + static_cast<int64_t>(std::llround(seconds * 1000)));
            } catch (...) { break; }
            pos = end + 1;
        }
        std::wstring text = line.substr(pos);
        // Empty timed lines deliberately clear the lyric during instrumental gaps.
        for (auto ms : times) result.push_back({ms, text});
    }
    for (auto& item : result) item.ms -= offset;
    std::stable_sort(result.begin(), result.end(), [](auto& a, auto& b) { return a.ms < b.ms; });
    std::vector<LyricLine> merged;
    for (auto& item : result) {
        if (!merged.empty() && merged.back().ms == item.ms) {
            if (!item.text.empty() && item.text != merged.back().text) {
                if (!merged.back().text.empty()) merged.back().text += L" / ";
                merged.back().text += item.text;
            }
        } else merged.push_back(std::move(item));
    }
    return merged;
}
static std::wstring LyricUtf8(const std::string& bytes) {
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), (int)bytes.size(), nullptr, 0);
    if (n <= 0) return {};
    std::wstring text(n, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), (int)bytes.size(), text.data(), n);
    return text;
}
static std::wstring LyricUrlEncode(const std::wstring& value) {
    int n = WideCharToMultiByte(CP_UTF8, 0, value.data(), (int)value.size(), nullptr, 0, nullptr, nullptr);
    std::string bytes(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.data(), (int)value.size(), bytes.data(), n, nullptr, nullptr);
    std::wstring encoded;
    constexpr wchar_t hex[] = L"0123456789ABCDEF";
    for (unsigned char c : bytes) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') encoded += (wchar_t)c;
        else { encoded += L'%'; encoded += hex[c >> 4]; encoded += hex[c & 15]; }
    }
    return encoded;
}
struct LyricHttpHandle {
    HINTERNET value;
    ~LyricHttpHandle() { if (value) WinHttpCloseHandle(value); }
};
static std::wstring DownloadLyrics(const std::wstring& path, const wchar_t* host = L"lrclib.net",
    const std::string& form = {}) {
    if (g_unloading) return {};
    LyricHttpHandle http{WinHttpOpen(L"Mozilla/5.0 TaskbarFluentMediaPlayer/1.6",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)};
    if (!http.value) return {};
    WinHttpSetTimeouts(http.value, 1500, 1500, 1500, 1500);
    LyricHttpHandle connection{WinHttpConnect(http.value, host, INTERNET_DEFAULT_HTTPS_PORT, 0)};
    if (!connection.value) return {};
    LyricHttpHandle request{WinHttpOpenRequest(connection.value, form.empty() ? L"GET" : L"POST", path.c_str(), nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE)};
    if (!request.value) return {};
    DWORD redirect = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    WinHttpSetOption(request.value, WINHTTP_OPTION_REDIRECT_POLICY, &redirect, sizeof(redirect));
    const wchar_t* headers = wcsstr(host,L"163.com")
        ? L"Accept: application/json\r\nReferer: https://music.163.com/\r\nCookie: os=pc; appver=3.0.18\r\nContent-Type: application/x-www-form-urlencoded\r\n"
        : L"Accept: application/json\r\nReferer: https://y.qq.com/\r\nContent-Type: application/x-www-form-urlencoded\r\n";
    if (!WinHttpSendRequest(request.value, headers, (DWORD)-1L,
        form.empty() ? WINHTTP_NO_REQUEST_DATA : (void*)form.data(), (DWORD)form.size(), (DWORD)form.size(), 0)
        || !WinHttpReceiveResponse(request.value, nullptr)) return {};
    DWORD status = 0, size = sizeof(status);
    if (!WinHttpQueryHeaders(request.value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX) || status != 200) return {};
    std::string bytes;
    ULONGLONG deadline = GetTickCount64() + 5000;
    for (;;) {
        if (g_unloading || GetTickCount64() > deadline) return {};
        char buffer[8192]; DWORD read = 0;
        if (!WinHttpReadData(request.value, buffer, sizeof(buffer), &read)) return {};
        if (!read) break;
        if (bytes.size() + read > 2 * 1024 * 1024) return {};
        bytes.append(buffer, read);
    }
    return LyricUtf8(bytes);
}
static std::wstring LyricSafeFilename(std::wstring text) {
    for (auto& c : text) if (c < 32 || wcschr(L"<>:\"/\\|?*", c)) c = L'_';
    return text;
}
static std::wstring LyricIdentity(std::wstring text) {
    std::wstring result;
    for (wchar_t c : text) {
        if (c >= L'A' && c <= L'Z') c += L'a' - L'A';
        if ((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') ||
            (c >= 0x3400 && c <= 0x9fff) || (c >= 0x3040 && c <= 0x30ff) || (c >= 0xac00 && c <= 0xd7af)) result += c;
    }
    return result;
}
static std::vector<std::wstring> LyricArtistSet(const std::wstring& names) {
    std::vector<std::wstring> result;
    size_t pos=0;
    while(pos<names.size()) {
        size_t end=names.find_first_of(L"/&、,，;；",pos);
        auto name=LyricIdentity(names.substr(pos,end==std::wstring::npos?end:end-pos));
        if(!name.empty())result.push_back(std::move(name));
        if(end==std::wstring::npos)break;
        pos=end+1;
    }
    std::sort(result.begin(),result.end());
    result.erase(std::unique(result.begin(),result.end()),result.end());
    return result;
}
static bool LyricArtistsEquivalent(const std::wstring& expected,const std::wstring& actual) {
    auto expectedNames=LyricArtistSet(expected);
    return !expectedNames.empty() && expectedNames==LyricArtistSet(actual);
}
static std::wstring LyricBase64(const std::wstring& input) {
    constexpr wchar_t alphabet[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string bytes;
    unsigned int bits = 0; int count = 0;
    for (wchar_t c : input) {
        if (c == L'=') break;
        if (iswspace(c)) continue;
        const wchar_t* digit = c ? wcschr(alphabet, c) : nullptr;
        if (!digit) return {};
        bits = (bits << 6) | (unsigned int)(digit - alphabet); count += 6;
        if (count >= 8) { count -= 8; bytes += (char)((bits >> count) & 255); }
    }
    return LyricUtf8(bytes);
}
static std::wstring LyricUnescape(std::wstring text) {
    for (const auto& item : {std::pair{L"&apos;", L"'"}, {L"&#39;", L"'"}, {L"&quot;", L"\""},
        {L"&#10;", L"\n"}, {L"&#13;", L"\r"}, {L"&lt;", L"<"}, {L"&gt;", L">"}, {L"&amp;", L"&"}}) {
        size_t pos = 0;
        while ((pos = text.find(item.first, pos)) != std::wstring::npos) {
            text.replace(pos, wcslen(item.first), item.second); pos += wcslen(item.second);
        }
    }
    return text;
}
static std::vector<LyricLine> ParseWordLyrics(std::wstring text) {
    // QRC can be a plain body or an XML document with LyricContent in an attribute.
    size_t attr = text.find(L"LyricContent=\"");
    if (attr != std::wstring::npos) {
        attr += 14;
        size_t end = text.find(L'\"', attr);
        if (end == std::wstring::npos) return {};
        text = text.substr(attr, end - attr);
    }
    text = LyricUnescape(text);
    int64_t wordOffset = 0;
    size_t offsetTag = text.find(L"[offset:");
    if (offsetTag != std::wstring::npos) {
        try { wordOffset = std::clamp(std::stoll(text.substr(offsetTag + 8)), -600000LL, 600000LL); } catch (...) {}
    }
    // QQ sometimes represents line breaks as literal backslash-n inside XML.
    size_t escaped = 0;
    while ((escaped = text.find(L"\\n", escaped)) != std::wstring::npos) text.replace(escaped, 2, L"\n");
    std::vector<LyricLine> result;
    std::wistringstream input(text);
    std::wstring line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (line.empty() || line.front() != L'[') continue;
        size_t comma = line.find(L','), close = line.find(L']');
        if (comma == std::wstring::npos || close == std::wstring::npos || comma > close) continue;
        try {
            LyricLine parsed{};
            size_t used;
            parsed.ms = std::stoll(line.substr(1, comma - 1), &used);
            if (used != comma - 1 || parsed.ms < 0 || parsed.ms > 86400000) continue;
            parsed.duration = std::stoll(line.substr(comma + 1, close - comma - 1), &used);
            if (used != close - comma - 1 || parsed.duration < 0 || parsed.duration > 600000) continue;
            size_t pos = close + 1;
            while (pos < line.size()) {
                size_t open = line.find(L'(', pos), separator = line.find(L',', open), end = line.find(L')', open);
                if (open == std::wstring::npos || separator == std::wstring::npos || end == std::wstring::npos || separator > end) {
                    parsed.text += line.substr(pos); break;
                }
                int64_t start = std::stoll(line.substr(open + 1, separator - open - 1), &used);
                if (used != separator - open - 1) throw std::invalid_argument("qrc start");
                int64_t duration = std::stoll(line.substr(separator + 1, end - separator - 1), &used);
                if (used != end - separator - 1 || start < 0 || duration < 0 || duration > 600000 || start > 86400000)
                    throw std::invalid_argument("qrc duration");
                size_t length = open - pos;
                if (length) parsed.words.push_back({start, duration, parsed.text.size(), length});
                parsed.text += line.substr(pos, length);
                pos = end + 1;
            }
            result.push_back(std::move(parsed));
        } catch (...) { continue; }
    }
    if (!result.empty()) {
        for (auto& item : result) { item.ms -= wordOffset; for (auto& word : item.words) word.ms -= wordOffset; }
        std::stable_sort(result.begin(), result.end(), [](auto& a, auto& b) { return a.ms < b.ms; });
        return result;
    }
    // Enhanced LRC: [00:01.00]<00:01.00>word<00:01.40>word<00:02.00>
    result = ParseLyrics(text);
    for (size_t index = 0; index < result.size(); ++index) {
        auto& item = result[index];
        std::wstring source = item.text, plain;
        std::vector<std::pair<int64_t, size_t>> markers;
        size_t pos = 0;
        while (pos < source.size()) {
            if (source[pos] == L'<') {
                size_t end = source.find(L'>', pos);
                if (end != std::wstring::npos) {
                    auto stamp = ParseLyrics(L"[" + source.substr(pos + 1, end - pos - 1) + L"]");
                    if (!stamp.empty()) { markers.emplace_back(stamp[0].ms - wordOffset, plain.size()); pos = end + 1; continue; }
                }
            }
            plain += source[pos++];
        }
        if (markers.size() < 2) continue;
        bool valid = true;
        for (size_t i = 0; i + 1 < markers.size(); ++i) {
            auto [time, offset] = markers[i];
            auto [nextTime, nextOffset] = markers[i+1];
            if (nextTime < time) { valid = false; break; }
            if (nextOffset > offset) item.words.push_back({time, nextTime - time, offset, nextOffset - offset});
        }
        // Without an explicit end marker, the last token stays unhighlighted.
        if (valid) item.text = std::move(plain); else item.words.clear();
    }
    return result;
}
static std::wstring QrcContent(const std::wstring& xml) {
    size_t tag = xml.find(L"<content");
    if (tag == std::wstring::npos) return {};
    size_t start = xml.find(L"<![CDATA[", tag), end = xml.find(L"]]>", start);
    if (start == std::wstring::npos || end == std::wstring::npos) return {};
    std::wstring body = xml.substr(start + 9, end - start - 9);
    if (body.find(L'[') != std::wstring::npos || body.find(L'<') != std::wstring::npos) return body;
    body.erase(std::remove_if(body.begin(), body.end(), [](wchar_t c) { return iswspace(c); }), body.end());
    return LyricUtf8(QqQrc::Decode(body));
}
static std::string LyricUtf8Bytes(const std::wstring& value) {
    int n = WideCharToMultiByte(CP_UTF8,0,value.data(),(int)value.size(),nullptr,0,nullptr,nullptr);
    std::string bytes(n,'\0');
    WideCharToMultiByte(CP_UTF8,0,value.data(),(int)value.size(),bytes.data(),n,nullptr,nullptr);
    return bytes;
}
static std::string LyricHex(const unsigned char* bytes, size_t size, bool upper) {
    const char* hex = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    std::string result; result.reserve(size*2);
    for(size_t i=0;i<size;++i){result+=hex[bytes[i]>>4];result+=hex[bytes[i]&15];}
    return result;
}
struct LyricCryptoAlgorithm {
    BCRYPT_ALG_HANDLE value=nullptr;
    ~LyricCryptoAlgorithm(){if(value)BCryptCloseAlgorithmProvider(value,0);}
};
struct LyricCryptoKey {
    BCRYPT_KEY_HANDLE value=nullptr;
    ~LyricCryptoKey(){if(value)BCryptDestroyKey(value);}
};
static std::string NeteaseEapiForm(const std::string& path, const std::string& json) {
    // EAPI request envelope: MD5 of the canonical request, then AES-128-ECB/PKCS7.
    LyricCryptoAlgorithm md5;
    if(BCryptOpenAlgorithmProvider(&md5.value,BCRYPT_MD5_ALGORITHM,nullptr,0)<0)return {};
    std::string message="nobody"+path+"use"+json+"md5forencrypt";
    unsigned char digest[16]{};
    if(BCryptHash(md5.value,nullptr,0,(PUCHAR)message.data(),(ULONG)message.size(),digest,sizeof(digest))<0)return {};
    std::string plaintext=path+"-36cd479b6b5-"+json+"-36cd479b6b5-"+LyricHex(digest,sizeof(digest),false);
    LyricCryptoAlgorithm aes;
    LyricCryptoKey key;
    if(BCryptOpenAlgorithmProvider(&aes.value,BCRYPT_AES_ALGORITHM,nullptr,0)<0)return {};
    if(BCryptSetProperty(aes.value,BCRYPT_CHAINING_MODE,(PUCHAR)BCRYPT_CHAIN_MODE_ECB,sizeof(BCRYPT_CHAIN_MODE_ECB),0)<0)return {};
    const char secret[]="e82ckenh8dichen8";
    if(BCryptGenerateSymmetricKey(aes.value,&key.value,nullptr,0,(PUCHAR)secret,16,0)<0)return {};
    ULONG size=0;
    if(BCryptEncrypt(key.value,(PUCHAR)plaintext.data(),(ULONG)plaintext.size(),nullptr,nullptr,0,nullptr,0,&size,BCRYPT_BLOCK_PADDING)<0)return {};
    std::vector<unsigned char> encrypted(size);
    if(BCryptEncrypt(key.value,(PUCHAR)plaintext.data(),(ULONG)plaintext.size(),nullptr,nullptr,0,encrypted.data(),size,&size,BCRYPT_BLOCK_PADDING)<0)return {};
    return "params="+LyricHex(encrypted.data(),size,true);
}
static std::wstring DownloadNetease(const std::wstring& apiPath, winrt::Windows::Data::Json::JsonObject data) {
    using namespace winrt::Windows::Data::Json;
    JsonObject header;
    header.SetNamedValue(L"os",JsonValue::CreateStringValue(L"pc"));
    header.SetNamedValue(L"appver",JsonValue::CreateStringValue(L"3.0.18"));
    header.SetNamedValue(L"deviceId",JsonValue::CreateStringValue(L""));
    data.SetNamedValue(L"header",JsonValue::CreateStringValue(header.Stringify()));
    auto form=NeteaseEapiForm(LyricUtf8Bytes(apiPath),LyricUtf8Bytes(std::wstring(data.Stringify())));
    if(form.empty())return {};
    return DownloadLyrics(L"/eapi"+apiPath.substr(4),L"interface3.music.163.com",form);
}
static std::vector<LyricLine> ParseYrcLyrics(const std::wstring& yrc) {
    std::wistringstream input(yrc);
    std::wstring line, qrc;
    while(std::getline(input,line)) {
        if(line.empty()||line.front()!=L'[')continue;
        size_t close=line.find(L']');
        if(close==std::wstring::npos||line.find(L',')>close)continue;
        std::wstring converted=line.substr(0,close+1);
        size_t pos=close+1;
        bool valid=true;
        while(pos<line.size()&&line[pos]!=L'\r') {
            if(line[pos]!=L'('){valid=false;break;}
            size_t end=line.find(L')',pos), first=line.find(L',',pos), second=line.find(L',',first==std::wstring::npos?pos:first+1);
            if(end==std::wstring::npos||first==std::wstring::npos||second==std::wstring::npos||second>end){valid=false;break;}
            size_t next=line.find(L'(',end+1);
            std::wstring token=line.substr(end+1,next==std::wstring::npos?next:next-end-1);
            if(!token.empty()&&token.back()==L'\r')token.pop_back();
            converted+=token+L"("+line.substr(pos+1,second-pos-1)+L")";
            if(next==std::wstring::npos)break;
            pos=next;
        }
        if(valid)qrc+=converted+L"\n";
    }
    return ParseWordLyrics(qrc);
}
static bool LyricNumericId(const std::wstring& id) {
    return !id.empty() && id.size()<=20 &&
        std::all_of(id.begin(),id.end(),[](wchar_t c){return c>=L'0'&&c<=L'9';});
}
static std::wstring ExtractPlayerSongId(PlayerSource source, const std::vector<std::wstring>& genres) {
    const wchar_t* prefix=source==PlayerSource::Netease?L"NCM-":source==PlayerSource::QQ?L"QQ-":nullptr;
    if(!prefix)return {};
    size_t size=wcslen(prefix);
    for(const auto& genre:genres) {
        if(genre.size()<=size)continue;
        bool matches=true;
        for(size_t i=0;i<size;++i) if(towupper(genre[i])!=prefix[i])matches=false;
        if(matches) {
            auto id=genre.substr(size);
            if(source==PlayerSource::Netease && !LyricNumericId(id))continue;
            if(id.size()<=64 && std::all_of(id.begin(),id.end(),[](wchar_t c){return (c>=L'0'&&c<=L'9')||(c>=L'a'&&c<=L'z')||(c>=L'A'&&c<=L'Z');}))return id;
        }
    }
    return {};
}
static std::vector<LyricLine> FetchNeteaseLyrics(const std::wstring& id) {
    using namespace winrt::Windows::Data::Json;
    if(!LyricNumericId(id)||g_unloading)return {};
    try {
        JsonObject request;
        request.SetNamedValue(L"id",JsonValue::CreateStringValue(id));
        request.SetNamedValue(L"cp",JsonValue::CreateStringValue(L"false"));
        for(const auto* key:{L"lv",L"kv",L"tv",L"rv",L"yv",L"ytv",L"yrv"})
            request.SetNamedValue(key,JsonValue::CreateStringValue(L"0"));
        auto response=DownloadNetease(L"/api/song/lyric/v1",request);
        if(response.empty())return {};
        auto json=JsonObject::Parse(response);
        if(json.HasKey(L"yrc")) {
            auto yrc=ParseYrcLyrics(std::wstring(json.GetNamedObject(L"yrc").GetNamedString(L"lyric",L"")));
            if(!yrc.empty())return yrc;
        }
        if(json.HasKey(L"lrc"))return ParseWordLyrics(std::wstring(json.GetNamedObject(L"lrc").GetNamedString(L"lyric",L"")));
    }catch(...){Wh_Log(L"Lyrics: Netease lyric request failed");}
    return {};
}
static std::vector<LyricLine> ResolveNeteaseLyrics(const std::wstring& title,const std::wstring& artist,double duration,const std::wstring& id=L"") {
    using namespace winrt::Windows::Data::Json;
    auto direct=FetchNeteaseLyrics(id);
    if(!direct.empty())return direct;
    if(title.empty()||artist.empty()||g_unloading)return {};
    try {
        JsonObject request;
        request.SetNamedValue(L"s",JsonValue::CreateStringValue(title+L" "+artist));
        for(const auto& item:{std::pair{L"type",L"1"},{L"limit",L"10"},{L"offset",L"0"},{L"total",L"true"}})
            request.SetNamedValue(item.first,JsonValue::CreateStringValue(item.second));
        auto response=DownloadNetease(L"/api/cloudsearch/pc",request);
        if(response.empty())return {};
        auto json=JsonObject::Parse(response);
        auto songs=json.GetNamedObject(L"result").GetNamedArray(L"songs");
        for(auto value:songs) {
            auto song=value.GetObject();
            if(LyricIdentity(std::wstring(song.GetNamedString(L"name",L"")))!=LyricIdentity(title))continue;
            auto singers=song.GetNamedArray(song.HasKey(L"ar")?L"ar":L"artists");
            std::wstring candidateArtists;
            for(auto singer:singers) {
                if(!candidateArtists.empty())candidateArtists+=L"/";
                candidateArtists+=singer.GetObject().GetNamedString(L"name",L"");
            }
            if(!LyricArtistsEquivalent(artist,candidateArtists))continue;
            double seconds=song.GetNamedNumber(song.HasKey(L"dt")?L"dt":L"duration",0)/1000;
            if(duration>0&&seconds>0&&std::abs(seconds-duration)>8)continue;
            auto lyrics=FetchNeteaseLyrics(std::to_wstring((uint64_t)song.GetNamedNumber(L"id",0)));
            if(!lyrics.empty())return lyrics;
            break; // Only download one verified recording per search.
        }
    }catch(...){Wh_Log(L"Lyrics: Netease search failed");}
    return {};
}
static std::vector<LyricLine> ResolveQqLyrics(const std::wstring& title, const std::wstring& artist, double duration) {
    using namespace winrt::Windows::Data::Json;
    try {
        auto response = DownloadLyrics(L"/soso/fcgi-bin/search_for_qq_cp?format=json&n=10&p=1&t=0&remoteplace=txt.yqq.song&platform=yqq.json&w="
            + LyricUrlEncode(title + L" " + artist), L"c.y.qq.com");
        if (response.empty() || g_unloading) return {};
        auto json = JsonObject::Parse(response);
        auto candidates = json.GetNamedObject(L"data").GetNamedObject(L"song").GetNamedArray(L"list");
        std::wstring wantedTitle = LyricIdentity(title);
        for (auto value : candidates) {
            auto song = value.GetObject();
            if (LyricIdentity(std::wstring(song.GetNamedString(L"songname", L""))) != wantedTitle) continue;
            std::wstring candidateArtists;
            for (auto singer : song.GetNamedArray(L"singer")) {
                if (!candidateArtists.empty()) candidateArtists += L"/";
                candidateArtists += singer.GetObject().GetNamedString(L"name", L"");
            }
            if (!LyricArtistsEquivalent(artist,candidateArtists)) continue;
            double candidateDuration = song.GetNamedNumber(L"interval", 0);
            if (duration > 61 && candidateDuration > 0 && std::abs(candidateDuration - duration) > 8) continue;
            double songId = song.GetNamedNumber(L"songid", 0);
            if (songId > 0) {
                std::string qrcForm = "version=15&miniversion=100&lrctype=4&musicid=" + std::to_string((uint64_t)songId);
                auto xml = DownloadLyrics(L"/qqmusic/fcgi-bin/lyric_download.fcg", L"c.y.qq.com", qrcForm);
                auto qrc = ParseWordLyrics(QrcContent(xml));
                if (!qrc.empty()) return qrc;
                if (g_unloading) return {};
            }
            auto mid = song.GetNamedString(L"songmid", L"");
            if (mid.empty()) continue;
            std::wstring encoded = LyricUrlEncode(std::wstring(mid));
            std::string form = "format=json&inCharset=utf8&outCharset=utf8&platform=yqq&g_tk=5381&songmid=";
            form.append(encoded.begin(), encoded.end());
            auto lyricResponse = DownloadLyrics(L"/lyric/fcgi-bin/fcg_query_lyric_new.fcg", L"c.y.qq.com", form);
            if (lyricResponse.empty() || g_unloading) return {};
            auto lyric = JsonObject::Parse(lyricResponse);
            auto lines = ParseWordLyrics(LyricUnescape(LyricBase64(std::wstring(lyric.GetNamedString(L"lyric", L"")))));
            if (!lines.empty()) return lines;
        }
    } catch (...) { Wh_Log(L"Lyrics: QQ Music lookup failed"); }
    return {};
}
static std::vector<LyricLine> ResolveLyrics(const std::wstring& folder, bool online,
    const std::wstring& title, const std::wstring& artist, const std::wstring& album, double duration,
    PlayerSource source = PlayerSource::Other, const std::wstring& songId = L"") {
    if (!folder.empty()) {
        for (const auto& name : {artist + L" - " + title, title + L" - " + artist, title}) {
          for (const auto& extension : {L".qrc", L".lrc", L".yrc"}) {
            std::wstring path = folder + L"\\" + LyricSafeFilename(name) + extension;
            HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE) continue;
            LARGE_INTEGER size{};
            std::string bytes;
            if (GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart <= 2 * 1024 * 1024) {
                bytes.resize((size_t)size.QuadPart); DWORD read = 0;
                if (!ReadFile(file, bytes.data(), (DWORD)bytes.size(), &read, nullptr)) bytes.clear();
                else bytes.resize(read);
            }
            CloseHandle(file);
            auto text = LyricUtf8(bytes);
            auto lines = wcscmp(extension,L".yrc") == 0 ? ParseYrcLyrics(text) : ParseWordLyrics(text);
            if (!lines.empty()) return lines;
          }
        }
    }
    if (!online || g_unloading) return {};
    if (source == PlayerSource::Netease) {
        auto netease = ResolveNeteaseLyrics(title, artist, duration, songId);
        if (!netease.empty()) return netease;
    }
    if (source == PlayerSource::QQ && LyricNumericId(songId)) {
        auto xml = DownloadLyrics(L"/qqmusic/fcgi-bin/lyric_download.fcg", L"c.y.qq.com",
            "version=15&miniversion=100&lrctype=4&musicid=" + LyricUtf8Bytes(songId));
        auto direct = ParseWordLyrics(QrcContent(xml));
        if (!direct.empty()) return direct;
    }
    if (artist.empty()) return {};
    auto qqLines = ResolveQqLyrics(title, artist, duration);
    if (!qqLines.empty()) return qqLines;
    if (source != PlayerSource::Netease) {
        auto netease = ResolveNeteaseLyrics(title, artist, duration);
        if (!netease.empty()) return netease;
    }
    if (duration <= 0) return {};
    // Exact metadata endpoint avoids displaying a different recording from a fuzzy search.
    std::wstring path = L"/api/get?track_name=" + LyricUrlEncode(title) + L"&artist_name=" + LyricUrlEncode(artist)
        + L"&album_name=" + LyricUrlEncode(album) + L"&duration=" + std::to_wstring((int)std::llround(duration));
    try {
        auto response = DownloadLyrics(path);
        if (response.empty()) return {};
        auto json = winrt::Windows::Data::Json::JsonObject::Parse(response);
        auto text = json.Lookup(L"syncedLyrics");
        if (text.ValueType() == winrt::Windows::Data::Json::JsonValueType::String)
            return ParseWordLyrics(std::wstring(text.GetString()));
    } catch (...) { Wh_Log(L"Lyrics: unavailable or invalid LRCLIB response"); }
    return {};
}
static std::wstring CurrentLyric(const std::wstring& title, const std::wstring& artist) {
    if (!g_lyricsEnabled) return {};
    std::lock_guard<std::mutex> lock(g_lyricsMtx);
    return title == g_lyricTitle && artist == g_lyricArtist ? g_lyricText : L"";
}
static HANDLE g_lyricsThread = nullptr;
static HANDLE g_lyricsStop = nullptr;
static DWORD WINAPI LyricsThreadProc(void*) {
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); } catch (...) { return 0; }
    std::wstring identity;
    ULONGLONG retryAt = 0;
    std::vector<LyricLine> lines;
    // Bounded successful-result cache; misses retry after 30 seconds.
    PlayerTimelineSmoother smoother;
    NeteaseNativeClock neteaseClock;
    std::list<std::pair<std::wstring, std::vector<LyricLine>>> cache;
    while (!g_unloading && WaitForSingleObject(g_lyricsStop, 200) == WAIT_TIMEOUT) {
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            { std::lock_guard<std::mutex> lock(g_sessionMtx); session = g_currentSession; }
            std::wstring title, artist, app, folder, key, display;
            LyricFrame frame;
            bool needsNativeClock = false, hasNativeClock = false;
            if (session && g_lyricsEnabled) {
                auto readProps = [&]() {
                    auto op = session.TryGetMediaPropertiesAsync();
                    const ULONGLONG deadline = GetTickCount64() + 2000;
                    while (op.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
                        if (WaitForSingleObject(g_lyricsStop, 50) == WAIT_OBJECT_0 || GetTickCount64() >= deadline) {
                            op.Cancel();
                            winrt::throw_hresult(E_ABORT);
                        }
                    }
                    return op.GetResults();
                };
                auto props = readProps();
                title = props.Title(); artist = props.Artist(); app = session.SourceAppUserModelId();
                PlayerSource source = ClassifyPlayerSource(app);
                std::wstring lookupArtist = artist.empty() ? std::wstring(props.AlbumArtist()) : artist;
                std::vector<std::wstring> genres;
                for (auto const& genre : props.Genres()) genres.emplace_back(genre);
                std::wstring songId = ExtractPlayerSongId(source, genres);
                auto timeline = session.GetTimelineProperties();
                double duration = (timeline.EndTime() - timeline.StartTime()).count() / 10000000.0;
                bool online = g_lyricsOnline;
                { std::lock_guard<std::mutex> lock(g_lyricsMtx); folder = g_lyricsFolder; }
                key = app + L"\n" + title + L"\n" + artist + L"\n" + std::wstring(props.AlbumTitle())
                    + L"\n" + folder + (online ? L"\n1" : L"\n0") + L"\n" + std::to_wstring((int)duration)
                    + L"\n" + lookupArtist + L"\n" + songId;
                if (key != identity || (lines.empty() && GetTickCount64() >= retryAt)) {
                    identity = key; lines.clear();
                    // Clear the previous track before doing any network I/O.
                    {
                        std::lock_guard<std::mutex> lock(g_lyricsMtx);
                        g_lyricText.clear();
                        g_lyricFrame = {title, artist, app, L"正在获取歌词…"};
                    }
                    g_needsUiUpdate = true;
                    auto hit = std::find_if(cache.begin(), cache.end(), [&](auto& entry) { return entry.first == key; });
                    if (hit != cache.end()) lines = hit->second;
                    else if (!title.empty()) {
                        lines = ResolveLyrics(folder, online, title, lookupArtist, std::wstring(props.AlbumTitle()), duration, source, songId);
                        if (!lines.empty()) cache.emplace_front(key, lines);
                        if (cache.size() > 64) cache.pop_back();
                    }
                    retryAt = GetTickCount64() + 30000;
                    auto latest = readProps();
                    if (latest.Title() != props.Title() || latest.Artist() != props.Artist()) {
                        identity.clear();
                        continue;
                    }
                    // Re-read the timeline after the fetch; downloading may take seconds.
                    timeline = session.GetTimelineProperties();
                }
                needsNativeClock = source == PlayerSource::Netease &&
                    timeline.LastUpdatedTime().time_since_epoch().count() == 0 && duration <= 0;
                if (!lines.empty()) {
                    auto rawPosition = (timeline.Position() - timeline.StartTime()).count() / 10000;
                    auto info = session.GetPlaybackInfo();
                    bool playing = info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                    auto rate = info.PlaybackRate();
                    double playbackRate = rate && std::isfinite(rate.Value()) ? std::clamp(rate.Value(),0.0,4.0) : 1.0;
                    uint64_t sampledAt = GetTickCount64();
                    int64_t position = (int64_t)smoother.Select(source, app+L"\n"+title+L"\n"+artist,
                        rawPosition, timeline.LastUpdatedTime().time_since_epoch().count()/10000,
                        winrt::clock::now().time_since_epoch().count()/10000, sampledAt, playing, playbackRate);
                    if (needsNativeClock) {
                        hasNativeClock = neteaseClock.Sample(playing, playbackRate, position);
                        sampledAt = GetTickCount64();
                    }
                    position = std::max<int64_t>(position,0);
                    if (duration > 0) position = std::min<int64_t>(position,(int64_t)(duration*1000));
                    position += g_lyricsOffsetMs.load() + GetSourceConfig(source).offsetMs;
                    auto next = std::upper_bound(lines.begin(), lines.end(), position,
                        [](int64_t ms, const LyricLine& line) { return ms < line.ms; });
                    if (next != lines.begin()) {
                        auto& line = *std::prev(next);
                        display = line.text;
                        frame.words = line.words;
                    }
                    if (next != lines.end()) frame.next = next->text;
                    frame.position = position;
                    frame.rate = playing ? playbackRate : 0;
                    frame.sampledAt = sampledAt;
                    if (needsNativeClock && !hasNativeClock) { display.clear(); frame.next.clear(); frame.words.clear(); }
                }
                // Drop results if the selected media session changed while we were waiting.
                { std::lock_guard<std::mutex> lock(g_sessionMtx); if (session != g_currentSession) continue; }
            } else { identity.clear(); lines.clear(); smoother = {}; }
            bool changed;
            {
                std::lock_guard<std::mutex> lock(g_lyricsMtx);
                changed = g_lyricText != display || g_lyricTitle != title || g_lyricArtist != artist || g_lyricApp != app;
                g_lyricTitle = title; g_lyricArtist = artist; g_lyricApp = app; g_lyricText = display;
                frame.title = title; frame.artist = artist; frame.app = app;
                frame.text = display.empty() ? (title.empty() ? L"暂无播放" : (lines.empty() ? L"暂未找到歌词" : L"♪")) : display;
                frame.hasLyrics = !lines.empty() && (!needsNativeClock || hasNativeClock);
                g_lyricFrame = std::move(frame);
            }
            if (changed) g_needsUiUpdate = true;
        } catch (...) {
            { std::lock_guard<std::mutex> lock(g_lyricsMtx); g_lyricText.clear(); g_lyricFrame = {}; }
            g_needsUiUpdate = true;
        }
    }
    winrt::uninit_apartment();
    return 0;
}
static void StartMediaThread() {
    if (g_mediaThread) return;
    g_mediaStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_mediaStopEvent) return;
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    if (!g_mediaThread) { CloseHandle(g_mediaStopEvent); g_mediaStopEvent = nullptr; return; }
    g_lyricsStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_lyricsStop) {
        g_lyricsThread = CreateThread(nullptr, 0, LyricsThreadProc, nullptr, 0, nullptr);
        if (!g_lyricsThread) { CloseHandle(g_lyricsStop); g_lyricsStop = nullptr; }
    }
}
static void StopMediaThread() {
    if (g_lyricsStop) SetEvent(g_lyricsStop);
    if (g_lyricsThread) { WaitForSingleObject(g_lyricsThread, INFINITE); CloseHandle(g_lyricsThread); g_lyricsThread = nullptr; }
    if (g_lyricsStop) { CloseHandle(g_lyricsStop); g_lyricsStop = nullptr; }
    if (g_mediaStopEvent) SetEvent(g_mediaStopEvent);
    if (g_mediaThread) { WaitForSingleObject(g_mediaThread, 5000); CloseHandle(g_mediaThread); g_mediaThread = nullptr; }
    if (g_mediaStopEvent) { CloseHandle(g_mediaStopEvent); g_mediaStopEvent = nullptr; }
}
static HANDLE g_timerThread    = nullptr;
static HANDLE g_timerStopEvent = nullptr;
static HANDLE g_timerUpdateEvent = nullptr;
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
    if (g_unloading || g_applyingSettings) return;
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
                    winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(16)});
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
        for (uint32_t i = 0; i < cols.Size(); i++) {
            if (i == 1) continue;
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
static void UpdateVisibility();
static void RefreshThemeColors();
static void RemovePlayerGrid();
static bool InjectPlayerGrid();
static std::atomic<bool> g_themeChangePending{false};
static void UpdateSecondaryMediaViews(bool missingOnly);
static void RemoveAllSecondaryMediaViews();
static DWORD WINAPI TimerThreadProc(void*) {
    static bool lastThemeWasLight = IsSystemLightTheme();
    HKEY hKey = nullptr;
    HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_NOTIFY, &hKey) == ERROR_SUCCESS) {
        RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE);
    }
    constexpr ULONGLONG kMountCheckIntervalMs = 4000;
    ULONGLONG nextMountCheck = GetTickCount64() + kMountCheckIntervalMs;
    while (!g_unloading) {
        HANDLE handles[] = {g_timerStopEvent, hEvent, g_timerUpdateEvent};
        DWORD wait = WaitForMultipleObjects(3, handles, FALSE, 500);
        if (wait == WAIT_OBJECT_0) break;
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
        bool needsUpdate = g_needsUiUpdate.exchange(false);
        if (g_settings.idleHideSeconds > 0) {
            bool playing = false;
            { std::lock_guard<std::mutex> lk(g_mediaMtx); playing = g_media.isPlaying; }
            if (playing) {
                g_idleSeconds = 0;
                g_idleTicks   = 0;
                if (g_hiddenByIdle) {
                    g_hiddenByIdle = false;
                    needsUpdate = true;
                }
            } else {
                ++g_idleTicks;
                if (g_idleTicks >= 2) {
                    g_idleTicks = 0;
                    ++g_idleSeconds;
                }
                if (!g_hiddenByIdle && g_idleSeconds >= g_settings.idleHideSeconds) {
                    g_hiddenByIdle = true;
                    needsUpdate = true;
                }
            }
        } else {
            if (g_hiddenByIdle) {
                g_hiddenByIdle = false;
                g_idleSeconds  = 0;
                g_idleTicks    = 0;
                needsUpdate    = true;
            }
        }
        const ULONGLONG now = GetTickCount64();
        const bool checkMount = now >= nextMountCheck;
        if (checkMount) {
            nextMountCheck = now + kMountCheckIntervalMs;
        }
        // Secondary UI is refreshed by its primary-dispatcher timer.
        if (needsUpdate || checkMount) {
            struct TaskbarUpdate {
                HWND hwnd;
                bool refresh;
                bool checkMount;
            } update{hWnd, needsUpdate, checkMount};
            RunFromWindowThread(hWnd, [](void* param) {
                const auto& update = *static_cast<TaskbarUpdate*>(param);
                if (g_unloading || g_applyingSettings || g_taskbarWnd != update.hwnd) return;
                if (!g_playerGrid && update.checkMount) {
                    TryMountPlayerOnTaskbar();
                } else if (g_playerGrid && update.refresh) {
                    RefreshPlayerContents();
                    UpdateVisibility();
                }
            }, &update);
        }
    }
    if (hKey) RegCloseKey(hKey);
    if (hEvent) CloseHandle(hEvent);
    return 0;
}
static constexpr int VIZ_BARS_MAX = 20;
static constexpr int VIZ_FFT_SIZE = 1024;
static constexpr int VIZ_NUM_BANDS = 7;
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
        20.f, 120.f, 300.f, 800.f, 2500.f, 6000.f, 14000.f, 20000.f};
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
    static constexpr float GRAVITY[VIZ_NUM_BANDS] = {0.018f, 0.020f, 0.022f, 0.025f,
                                                    0.030f, 0.036f, 0.042f};
    ULONGLONG lastReinitAttempt = GetTickCount64() - 1000;
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
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                bandEnv[b] = std::max(0.f, bandEnv[b] - GRAVITY[b]);
                g_VizBands[b].store(bandEnv[b], std::memory_order_relaxed);
            }
            continue;
        }
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
            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData && numFrames > 0) {
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
            ringCount -= VIZ_FFT_SIZE / 2;
            VizFFT(re, im);
            float t_sens = g_settings.vizSensitivity / 100.0f;
            float sliderGain = (t_sens <= 1.0f)
                ? 0.25f + t_sens * t_sens * 2.75f
                : 3.0f + (t_sens - 1.0f) * 4.0f;
            auto eq = GetVizEQMultipliers(g_settings.vizEq);
            static constexpr float BAND_SENSITIVITY[VIZ_NUM_BANDS] = {
                0.30f, 0.22f, 0.12f, 0.06f, 0.030f, 0.018f, 0.010f};
            static constexpr int BAND_EQ_ZONE[VIZ_NUM_BANDS] = {0, 0, 1, 1, 2, 2, 2};
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                int bStart = g_LogBinStart[b];
                int bEnd = g_LogBinStart[b + 1];
                if (bEnd <= bStart)
                    bEnd = bStart + 1;
                float sumSq = 0.f;
                int count = 0;
                for (int k = bStart; k < bEnd; k++) {
                    sumSq += re[k] * re[k] + im[k] * im[k];
                    count++;
                }
                float rms = (count > 0) ? sqrtf(sumSq / (float)count) : 0.f;
                float eqM = (BAND_EQ_ZONE[b] == 0)   ? eq.low
                            : (BAND_EQ_ZONE[b] == 1) ? eq.mid
                                                    : eq.high;
                float mag = std::max(
                    0.f, std::min(1.f, (rms / (VIZ_FFT_SIZE * 0.5f)) /
                                        BAND_SENSITIVITY[b] * sliderGain * eqM));
                bandEnv[b] = (mag >= bandEnv[b])
                                ? mag
                                : std::max(0.f, bandEnv[b] - GRAVITY[b]);
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
static constexpr float VIZ_SEEDS[VIZ_BARS_MAX] = {
    0.83f, 0.41f, 1.27f, 0.61f, 1.09f, 0.37f, 0.95f, 0.52f, 1.18f, 0.74f,
    0.29f, 1.03f, 0.66f, 0.88f, 0.45f, 1.21f, 0.57f, 0.93f, 0.31f, 1.15f};
static void UpdateVisualizerPeaks() {
    const int vizBars = std::max(1, std::min(g_settings.vizBars, VIZ_BARS_MAX));
    float bands[VIZ_NUM_BANDS];
    float masterPeak = 0.f;
    for (int i = 0; i < VIZ_NUM_BANDS; i++) {
        bands[i] = g_VizBands[i].load(std::memory_order_relaxed);
        masterPeak = std::max(masterPeak, bands[i]);
    }
    auto eq = GetVizEQMultipliers(g_settings.vizEq);
    auto sampleBands = [&](float t) -> float {
        float pos = t * (VIZ_NUM_BANDS - 1);
        int lo = (int)pos;
        int hi = std::min(lo + 1, VIZ_NUM_BANDS - 1);
        return bands[lo] * (1.f - (pos - (float)lo)) +
            bands[hi] * (pos - (float)lo);
    };
    auto eqForT = [&](float t) -> float {
        return (t < 0.33f) ? eq.low : (t < 0.66f) ? eq.mid : eq.high;
    };
    float t = (float)GetTickCount64() * 0.001f;
    float center = (vizBars - 1) * 0.5f;
    for (int i = 0; i < vizBars; i++) {
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;
        switch (g_settings.vizShape) {
            case VizShape::Stereo:
                target = sampleBands(freqT) * eqForT(freqT);
                break;
            case VizShape::Mountain: {
                float dist = fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(dist) * eqForT(dist);
                float taper = 1.6f - dist * 0.9f;
                target = std::max(
                    0.f, std::min(1.f, (energy + masterPeak * (0.2f - dist * 0.12f)) *
                                        taper));
                break;
            }
            case VizShape::Mirror: {
                float mirT = 1.f - fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(mirT) * eqForT(mirT);
                target = std::max(
                    0.f, std::min(1.f, (energy + masterPeak * (0.1f + mirT * 0.12f)) *
                                        1.3f));
                break;
            }
            case VizShape::Wave: {
                float phase = (float)i * (2.f * VIZ_PI / (float)vizBars);
                float wave = 0.55f + 0.45f * sinf(t * 3.5f - phase);
                float energy = sampleBands(freqT) * eqForT(freqT);
                target = std::max(0.f, std::min(1.f, energy * wave + masterPeak * 0.15f));
                break;
            }
            case VizShape::Breathe: {
                if (i == 0) {
                    float k = (masterPeak > g_VizBreatheEnv) ? 0.04f : 0.015f;
                    g_VizBreatheEnv += (masterPeak - g_VizBreatheEnv) * k;
                }
                float rate = 0.55f + VIZ_SEEDS[i % VIZ_BARS_MAX] * 0.18f;
                float inhale =
                    0.5f + 0.5f * sinf(t * rate + VIZ_SEEDS[i % VIZ_BARS_MAX] * 1.2f);
                target = std::max(0.f, std::min(1.f,
                                                inhale * (0.12f + g_VizBreatheEnv * 0.88f)));
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
    float attack = 0.55f, decay = 0.18f;
    float sensBoost = std::max(0.f, (g_settings.vizSensitivity - 100) / 200.f) * 0.12f;
    switch (g_settings.vizShape) {
        case VizShape::Stereo:  attack = 0.72f; decay = 0.22f + sensBoost; break;
        case VizShape::Mirror:  attack = 0.52f; decay = 0.20f + sensBoost; break;
        case VizShape::Wave:    attack = 0.34f; decay = 0.17f + sensBoost; break;
        case VizShape::Breathe: attack = 0.20f; decay = 0.11f + sensBoost; break;
        default:                decay += sensBoost; break;
    }
    int barCount = std::min((int)g_vizBars->size(), std::max(1, g_settings.vizBars));
    double zoneH = VizZoneHeight();
    double maxBH = std::max(4.0, zoneH - 6.0);
    double minBH = (double)g_settings.vizBarWidth;
    double idlePx = std::min((double)g_settings.vizIdleBarSize, std::max(0.0, maxBH - minBH));
    double range = std::max(0.0, maxBH - minBH - idlePx);
    bool idleHidden = (g_settings.vizIdleBarSize == 0);
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
    const auto& pal0   = g_cachedVizPal0;
    const auto& pal1   = g_cachedVizPal1;
    const auto& cg0    = g_cachedVizCg0;
    const auto& cg1    = g_cachedVizCg1;
    const auto& acrCol = g_cachedVizAcrCol;
    for (int i = 0; i < barCount; i++) {
        float tgt = g_VizTarget[i], cur = g_VizPeak[i];
        float a = attack, d = decay;
        if (g_settings.vizShape == VizShape::Mountain) {
            float dist = fabsf((float)i - ((barCount - 1) * 0.5f));
            if (dist < 0.5f)      { a = 0.85f; d = 0.26f + sensBoost; }
            else if (dist < 1.5f) { a = 0.62f; d = 0.20f + sensBoost; }
            else                  { a = 0.92f; d = 0.34f + sensBoost; }
        }
        float next = cur + (tgt - cur) * ((tgt > cur) ? a : d);
        g_VizPeak[i] = (fabsf(next - cur) > 0.0005f) ? next : tgt;
        float fac = std::max(0.f, g_VizPeak[i]);
        double bh = idleHidden
            ? (fac * maxBH)
            : (minBH + idlePx + fac * range);
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
            (*g_vizBars)[i].Height(bh);
            if (i < (int)g_vizBrushes->size() && (*g_vizBrushes)[i])
                (*g_vizBrushes)[i].Color(c);
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
}
static void StartVizTimer() {
    HWND hWnd = g_taskbarWnd;
    if (!hWnd || !IsWindow(hWnd)) return;
    RunFromWindowThread(hWnd, [](void*) {
        try {
            if (!g_vizDispatcherTimer) {
                g_vizDispatcherTimer = winrt::Windows::UI::Xaml::DispatcherTimer();
                g_vizDispatcherTimer.Interval(
                    winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(16)});
                g_vizDispatcherTimerToken = g_vizDispatcherTimer.Tick(&VizTimerTick);
                g_vizDispatcherTimerHasToken = true;
            }
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
    double minBH = (double)g_settings.vizBarWidth;
    double maxBH = std::max(4.0, zoneH - 6.0);
    double idlePx = std::min((double)g_settings.vizIdleBarSize, std::max(0.0, maxBH - minBH));
    double corner = g_settings.vizBarWidth * 0.5;
    bool idleHidden = (g_settings.vizIdleBarSize == 0);
    for (int i = 0; i < barCount; i++) {
        VizRect r;
        r.Width((double)g_settings.vizBarWidth);
        r.Height(idleHidden ? 0.0 : (minBH + idlePx));
        r.RadiusX(corner);
        r.RadiusY(corner);
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
    const double kVizSectionGap = 6.0;
    double marginL = (double)g_settings.vizPadLeft  + (vizLeft ? 0.0 : kVizSectionGap);
    double marginR = (double)g_settings.vizPadRight + (vizLeft ? kVizSectionGap : 0.0);
    Grid vizContainer;
    vizContainer.Name(L"FluentMedia_Visualizer");
    vizContainer.Height(zoneH);
    vizContainer.VerticalAlignment(VerticalAlignment::Center);
    vizContainer.HorizontalAlignment(HorizontalAlignment::Center);
    vizContainer.IsHitTestVisible(false);
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
    g_timerThread    = CreateThread(nullptr, 0, TimerThreadProc, nullptr, 0, nullptr);
    if (!g_timerThread) {
        CloseHandle(g_timerStopEvent);
        g_timerStopEvent = nullptr;
        CloseHandle(g_timerUpdateEvent);
        g_timerUpdateEvent = nullptr;
    }
    if (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) {
        StartScrollTimer();
    }
}
static void StopTimerThread() {
    StopScrollTimer();
    StopVizTimer();
    StopVizCaptureThread();
    if (g_timerStopEvent) SetEvent(g_timerStopEvent);
    if (g_timerThread) {
        DWORD tid = GetCurrentThreadId();
        HWND hTaskbar = g_taskbarWnd;
        bool isUiThread = hTaskbar && (GetWindowThreadProcessId(hTaskbar, nullptr) == tid);
        if (isUiThread) {
            DWORD result = WAIT_TIMEOUT;
            DWORD deadline = GetTickCount() + 2000;
            while (result == WAIT_TIMEOUT && GetTickCount() < deadline) {
                result = MsgWaitForMultipleObjects(1, &g_timerThread, FALSE, 50, QS_SENDMESSAGE);
                if (result == WAIT_OBJECT_0 + 1) {
                    MSG msg;
                    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                    result = WAIT_TIMEOUT;
                }
            }
        } else {
            WaitForSingleObject(g_timerThread, 2000);
        }
        CloseHandle(g_timerThread);
        g_timerThread = nullptr;
    }
    if (g_timerStopEvent) { CloseHandle(g_timerStopEvent); g_timerStopEvent = nullptr; }
    if (g_timerUpdateEvent) { CloseHandle(g_timerUpdateEvent); g_timerUpdateEvent = nullptr; }
}
static void RefreshThemeColors() {
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
        [](HMONITOR hMon, HDC, LPRECT, LPARAM lp) CALLBACK -> BOOL {
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
    EnumWindows([](HWND hWnd, LPARAM lp) CALLBACK -> BOOL {
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
    EnumWindows([](HWND hWnd, LPARAM lp) CALLBACK -> BOOL {
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
    FrameworkElement taskbarElement{nullptr};
    HRESULT hr = taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    return SUCCEEDED(hr) ? result : nullptr;
}
static const wchar_t* GetGlyphWithStyle(int cmd, const std::wstring& style, bool isPlaying = false) {
    bool isFluent = (style == L"fluent_outline" || style == L"fluent_filled");
    bool isFilled = (style == L"fluent_filled" || style == L"mdl2_filled");
    switch (cmd) {
        case 1:
            if (isFilled) return L"";
            return L"";
        case 2:
            if (isPlaying) {
                if (isFluent && isFilled) return L"";
                if (!isFluent && isFilled) return L"";
                return L"";
            } else {
                if (isFilled) return L"";
                return L"";
            }
        case 3:
            if (isFilled) return L"";
            return L"";
        case 5:
            if (isFilled) return L"";
            return L"";
        case 6:
            if (isFilled) return L"";
            return L"";
        case 7:
            return L"";
        case 8: {
            RepeatMode mode = g_repeatMode.load();
            switch (mode) {
                case RepeatMode::Off: return L"";
                case RepeatMode::All: return L"";
                case RepeatMode::One: return L"";
            }
        }
        case 9:
            return L"";
        case 10: return L"";
        case 11: return L"";
        case 12: return L"";
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


        auto updateBtnVisualState = [btn, isPressed, isHovered]() {
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
                                if (auto thumbRef = props.Thumbnail()) {
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
        auto applyState = [btn, isHovered, pressLocked](bool pressed) {
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
            auto applyState = [btn, isHovered](bool pressed) {
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


struct LyricsRegionState {
    winrt::weak_ref<Canvas> canvas;
    ColumnDefinition column{nullptr};
    bool visible = false;
    TextBlock current{nullptr}, highlight{nullptr}, next{nullptr};
    RectangleGeometry highlightClip{nullptr}, viewportClip{nullptr};
    DispatcherTimer timer{nullptr};
    winrt::event_token token{};
    HWND taskbar = nullptr;
    std::wstring cachedText;
    std::vector<double> widths;
    double cachedFont = 0;
    uint64_t lineAppearedAt = 0;
    uint32_t cachedColor = 0;
    void Render() {
        if (g_unloading || g_applyingSettings) return;
        auto area = canvas.get();
        if (!area) return;
        LyricFrame frame;
        { std::lock_guard<std::mutex> lock(g_lyricsMtx); frame = g_lyricFrame; }
        {
            std::lock_guard<std::mutex> lock(g_mediaMtx);
            if (frame.title != g_media.title || frame.artist != g_media.artist || frame.app != g_media.appUserModelId)
                frame = {g_media.title, g_media.artist, g_media.appUserModelId, g_media.hasMedia ? L"正在获取歌词…" : L"暂无播放"};
        }
        const bool shouldShow = g_lyricsEnabled && frame.hasLyrics;
        if (visible != shouldShow) {
            visible = shouldShow;
            column.Width({visible ? (double)g_lyricsWidth + 10 : 0.0, GridUnitType::Pixel});
            area.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
            if (g_settings.playerMaxWidth > 0) {
                auto panel = VisualTreeHelper::GetParent(area);
                auto wrapper = panel ? VisualTreeHelper::GetParent(panel).try_as<FrameworkElement>() : nullptr;
                if (wrapper) wrapper.MaxWidth((double)g_settings.playerMaxWidth + (visible ? g_lyricsWidth + 10 : 0));
            }
        }
        if (!shouldShow) return;
        RECT rect{};
        double height = 40;
        if (taskbar && GetClientRect(taskbar, &rect)) {
            UINT dpi = GetDpiForWindow(taskbar);
            height = (rect.bottom - rect.top) * 96.0 / (dpi ? dpi : 96) - 4;
        }
        height = std::clamp(height, 20.0, 120.0);
        const double width = g_lyricsWidth, rowHeight = height / 2;
        const double font = std::clamp(rowHeight * .68, 9.0, 24.0);
        if (area.Height() != height) area.Height(height);
        viewportClip.Rect({0, 0, (float)width, (float)height});
        bool measure = cachedFont != font || cachedText != frame.text;
        if (cachedFont != font) {
            current.FontSize(font); highlight.FontSize(font); next.FontSize(font * .92);
            current.LineHeight(font * 1.18); highlight.LineHeight(font * 1.18); next.LineHeight(font * .92 * 1.18);
            cachedFont = font;
        }
        auto color = TextColor();
        uint32_t argb = uint32_t(color.A)<<24 | uint32_t(color.R)<<16 | uint32_t(color.G)<<8 | color.B;
        if (argb != cachedColor) {
            cachedColor = argb;
            current.Foreground(MakeBrush(color)); highlight.Foreground(MakeBrush(color)); next.Foreground(MakeBrush(color));
        }
        bool timed = g_lyricsWordHighlight && !frame.words.empty();
        current.Opacity(timed ? .45 : 1.0);
        highlight.Visibility(timed ? Visibility::Visible : Visibility::Collapsed);
        if (next.Text() != frame.next) next.Text(frame.next);
        if (measure) {
            if (cachedText != frame.text) lineAppearedAt = GetTickCount64();
            cachedText = frame.text;
            current.Text(frame.text); highlight.Text(frame.text);
            TextBlock ruler;
            ruler.FontFamily(current.FontFamily()); ruler.FontSize(font); ruler.FontWeight(current.FontWeight());
            widths.assign(frame.text.size() + 1, 0);
            for (size_t i = 1; i <= frame.text.size(); ++i) {
                if (i < frame.text.size() && frame.text[i-1] >= 0xd800 && frame.text[i-1] <= 0xdbff) continue;
                ruler.Text(frame.text.substr(0, i));
                ruler.Measure({100000, (float)height});
                widths[i] = ruler.DesiredSize().Width;
            }
        }
        // Center the measured two-line block, rather than placing glyphs at the
        // top of two half-height slots. Keep the baselines compact and stable.
        current.Measure({100000, (float)height});
        next.Measure({(float)width, (float)height});
        double currentHeight = std::max(font * 1.18, (double)current.DesiredSize().Height);
        double nextHeight = std::max(font * .92 * 1.18, (double)next.DesiredSize().Height);
        double blockHeight = currentHeight + nextHeight;
        double top = std::max(0.0, (height - blockHeight) / 2);
        Canvas::SetTop(current, top); Canvas::SetTop(highlight, top);
        Canvas::SetTop(next, top + currentHeight);
        double sungWidth = 0;
        if (timed && !widths.empty()) {
            double position = frame.position + (GetTickCount64() - frame.sampledAt) * frame.rate;
            sungWidth = LyricHighlightWidth(frame.words, position, widths);
        }
        highlightClip.Rect({0, 0, (float)sungWidth, (float)(currentHeight + 2)});
        double overflow = widths.empty() ? 0 : std::max(0.0, widths.back() - width), offset = 0;
        if (overflow > 0) {
            if (timed) offset = std::clamp(sungWidth - width * .65, 0.0, overflow);
            else {
                double elapsed = (GetTickCount64() - lineAppearedAt) / 1000.0, duration = overflow / 24.0;
                double phase = std::fmod(elapsed, 2 * duration + 4);
                if (phase > 2 && phase <= duration + 2) offset = (phase - 2) * 24;
                else if (phase > duration + 2 && phase <= duration + 4) offset = overflow;
                else if (phase > duration + 4) offset = overflow - (phase - duration - 4) * 24;
            }
        }
        Canvas::SetLeft(current, -offset); Canvas::SetLeft(highlight, -offset);
    }
};
static Canvas BuildLyricsRegion(HWND taskbar, ColumnDefinition const& column) {
    Canvas region;
    region.Name(L"FluentMedia_LyricsRegion");
    region.Width(g_lyricsWidth); region.MinWidth(g_lyricsWidth); region.MaxWidth(g_lyricsWidth);
    region.Height(40); region.Margin({10, 0, 0, 0});
    region.Visibility(Visibility::Collapsed);
    region.VerticalAlignment(VerticalAlignment::Center);
    auto state = std::make_shared<LyricsRegionState>();
    state->canvas = winrt::make_weak(region); state->taskbar = taskbar; state->column = column;
    state->viewportClip = RectangleGeometry(); region.Clip(state->viewportClip);
    state->current = TextBlock(); state->highlight = TextBlock(); state->next = TextBlock();
    state->highlightClip = RectangleGeometry(); state->highlight.Clip(state->highlightClip);
    for (auto block : {state->current, state->highlight, state->next}) {
        block.TextWrapping(TextWrapping::NoWrap); block.TextTrimming(TextTrimming::None);
        block.LineStackingStrategy(LineStackingStrategy::BlockLineHeight);
        block.FontFamily(Media::FontFamily(g_settings.artistFont.empty() ? L"Segoe UI" : g_settings.artistFont));
        block.IsHitTestVisible(false);
        region.Children().Append(block);
    }
    state->next.Width(g_lyricsWidth); state->next.TextTrimming(TextTrimming::CharacterEllipsis);
    state->next.Opacity(.65);
    region.Loaded([state](auto const&, auto const&) {
        try {
            state->Render();
            if (state->timer) return;
            state->timer = DispatcherTimer();
            state->timer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(32)});
            std::weak_ptr<LyricsRegionState> weak = state;
            state->token = state->timer.Tick([weak](auto const&, auto const&) {
                if (auto live = weak.lock()) { try { live->Render(); } catch (...) {} }
            });
            state->timer.Start();
        } catch (...) { Wh_Log(L"Lyrics: failed to start region rendering"); }
    });
    region.Unloaded([state](auto const&, auto const&) {
        try { if (state->timer) { state->timer.Stop(); state->timer.Tick(state->token); state->timer = nullptr; } } catch (...) {}
    });
    return region;
}
static Grid BuildPlayerGrid(HWND taskbar) {
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
            panel.Margin({4, 2, 4, 2});
        }
        AddLayoutAnchorOverlay(panel, L"FluentMedia_DebugPanelAnchors", {0xD0, 0x00, 0xFF, 0x00});
        if (g_settings.showDebugBorders) {
            Border panelDebugBorder;
            panelDebugBorder.BorderBrush(MakeBrush({0xFF, 0x00, 0xFF, 0x00}));
            panelDebugBorder.BorderThickness({1,1,1,1});
            panel.Children().Append(panelDebugBorder);
        }
        bool buttonsLeft = g_settings.mirrorLayout;
        bool albumArtLeft = !g_settings.mirrorLayout;
        bool hasText = g_settings.showTrackTitle || g_settings.showTrackArtist;
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
            if (g_settings.showPauseOverlay) {
                Border pauseBorder;
                pauseBorder.Name(L"PauseIconOverlay");
                pauseBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
                pauseBorder.VerticalAlignment(VerticalAlignment::Stretch);
                BYTE opacity = (BYTE)((g_settings.pauseOverlayOpacity * 255) / 100);
                pauseBorder.Background(MakeBrush({opacity, 0x00, 0x00, 0x00}));
                pauseBorder.Visibility(Visibility::Collapsed);
                Canvas::SetZIndex(pauseBorder, 8);
                TextBlock pauseIcon;
                pauseIcon.Text(L"");
                pauseIcon.FontFamily(Media::FontFamily(L"Segoe MDL2 Assets"));
                pauseIcon.FontSize((double)g_settings.pauseOverlayIconSize);
                pauseIcon.Foreground(MakeBrush({0xFF, 0xFF, 0xFF, 0xFF}));
                pauseIcon.HorizontalAlignment(HorizontalAlignment::Center);
                pauseIcon.VerticalAlignment(VerticalAlignment::Center);
                pauseBorder.Child(pauseIcon);
                artInnerGrid.Children().Append(pauseBorder);
            }
            if (g_settings.disableAlbumArtClick) {
                artContainer.IsHitTestVisible(false);
            } else {
                artContainer.PointerPressed([](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
                    if (auto elem = sender.template try_as<UIElement>()) {
                        elem.CapturePointer(e.Pointer());
                    }
                    e.Handled(true);
                });
                auto artDblClickLastTime = std::make_shared<ULONGLONG>(0);
                auto artDblClickLastKind = std::make_shared<winrt::Windows::UI::Input::PointerUpdateKind>(
                    winrt::Windows::UI::Input::PointerUpdateKind::Other);
                artContainer.PointerReleased([artDblClickLastTime, artDblClickLastKind](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
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
            if (albumArtLeft) {
                Grid::SetColumn(artContainer, 0);
            } else {
                Grid::SetColumn(artContainer, 3);
            }
            panel.Children().Append(artContainer);
        }
        if (g_settings.showTrackTitle || g_settings.showTrackArtist) {
            Border textContainer;
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
            Grid::SetColumn(textContainer, 1);
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
                if (!g_mediaButtons.empty()) {
                    currentButtons = g_mediaButtons;
                } else {
                    currentButtons = {
                        {MediaButtonType::Previous, 1},
                        {MediaButtonType::PlayPause, 2},
                        {MediaButtonType::Next, 3}
                    };
                }
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
                    Grid::SetColumn(ctrlDebugBorder, buttonsLeft ? 0 : 3);
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
                Grid::SetColumn(ctrlPanel, 0);
            } else {
                Grid::SetColumn(ctrlPanel, 3);
            }
            if (hasButtons) {
                panel.Children().Append(ctrlPanel);
            }
        }
        if (g_settings.vizEnabled) {
            try {
                auto vizEl = BuildVisualizerElement();
                if (vizEl) {
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
                }
            } catch (...) {
                Wh_Log(L"BuildPlayerGrid: Exception adding inline visualizer");
            }
        }
        if (g_lyricsEnabled) {
            ColumnDefinition lyricsColumn;
            lyricsColumn.Width({0.0, GridUnitType::Pixel});
            int column = (int)panel.ColumnDefinitions().Size();
            panel.ColumnDefinitions().Append(lyricsColumn);
            auto region = BuildLyricsRegion(taskbar, lyricsColumn);
            Grid::SetColumn(region, column);
            panel.Children().Append(region);
        }
        Grid wrapper;
        wrapper.Name(kGridName);
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
        panel.PointerExited([wrapper, isHovered, updatePlayerVisualState](auto const&, PointerRoutedEventArgs const& e) {
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
        auto [targetGrid, insertCol] = ResolveInjectionTarget(root, g_settings.position);
        if (!targetGrid) {
            Wh_Log(L"InjectPlayerGrid: No target grid for position %s", g_settings.position.c_str());
            if (g_settings.enableTreeDump) {
                DumpXamlTree(root, 0, 8);
            }
            return false;
        }
        Grid playerGrid = BuildPlayerGrid(g_taskbarWnd);
        if (!playerGrid) return false;
        bool startButtonModActive = IsStartButtonModActive(root);
        (void)startButtonModActive;
        bool isTrayGrid = (targetGrid.Name() == L"SystemTrayFrameGrid");
        RemovePlayerGridChildren(targetGrid);
        RemoveAnchorDebugOverlays(targetGrid);
        if (isTrayGrid) {
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
                        if (childCol >= insertCol)
                            Grid::SetColumn(child, childCol + 1);
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
            auto repeater  = FindChildByName(targetGrid, L"TaskbarFrameRepeater");
            auto trayFrame = FindChildByName(targetGrid, L"SystemTrayFrameGrid");
            bool isEdgePosition = (g_settings.position == L"taskbar_left_edge" ||
                                g_settings.position == L"taskbar_center_edge" ||
                                g_settings.position == L"taskbar_right_edge");
            bool isTrackingPosition = (g_settings.position == L"taskbar_left_start" ||
                                    g_settings.position == L"taskbar_right_start" ||
                                    g_settings.position == L"taskbar_after_search_left" ||
                                    g_settings.position == L"taskbar_after_search_right" ||
                                    g_settings.position == L"taskbar_after_taskview_left" ||
                                    g_settings.position == L"taskbar_after_taskview_right" ||
                                    g_settings.position == L"taskbar_after_widgets_left" ||
                                    g_settings.position == L"taskbar_after_widgets_right");
            if (isEdgePosition || isTrackingPosition) {
                double leftMargin  = (double)g_settings.playerMarginLeft;
                double rightMargin = (double)g_settings.playerMarginRight;
                playerGrid.HorizontalAlignment(HorizontalAlignment::Left);
                if (isEdgePosition) {
                    if (g_settings.position == L"taskbar_left_edge") {
                        playerGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                    else if (g_settings.position == L"taskbar_center_edge") {
                        playerGrid.HorizontalAlignment(HorizontalAlignment::Center);
                        playerGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                    else if (g_settings.position == L"taskbar_right_edge") {
                        playerGrid.HorizontalAlignment(HorizontalAlignment::Right);
                        if (trayFrame) rightMargin += trayFrame.ActualWidth() + 4;
                        playerGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                } else if (isTrackingPosition) {
                    FrameworkElement targetElem = nullptr;
                    std::wstring trackSide = L"right";
                    if (repeater) {
                        if (g_settings.position == L"taskbar_left_start") {
                            targetElem = FindElementInRepeater(repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_right_start") {
                            targetElem = FindElementInRepeater(repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
                            trackSide = L"right";
                        } else if (g_settings.position == L"taskbar_after_search_left") {
                            targetElem = FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_after_search_right") {
                            targetElem = FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
                            trackSide = L"right";
                        } else if (g_settings.position == L"taskbar_after_taskview_left") {
                            targetElem = FindNthElementByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_after_taskview_right") {
                            targetElem = FindNthElementByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
                            trackSide = L"right";
                        } else if (g_settings.position == L"taskbar_after_widgets_left") {
                            targetElem = FindChildByName(repeater, L"AugmentedEntryPointButton");
                            if (!targetElem) targetElem = FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton");
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_after_widgets_right") {
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
                            (g_settings.position == L"taskbar_left_start" ||
                            g_settings.position == L"taskbar_right_start" ||
                            g_settings.position == L"taskbar_after_taskview_left" ||
                            g_settings.position == L"taskbar_after_taskview_right")) {
                            startButtonOffset = GetStartButtonAdjustment(root);
                        }
                        g_layoutUpdateToken = targetGrid.LayoutUpdated(
                            [targetGrid, startButtonModActiveMod, startButtonOffset](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
                                try {
                                    if (!g_playerGrid || !g_trackedElement || g_unloading) return;
                                    UpdateAnchorDebugOverlay(targetGrid, g_trackedElement);
                                    bool isVisible = (g_playerGrid.Visibility() == Visibility::Visible);
                                    double w = isVisible ? g_playerGrid.ActualWidth() : 0.0;
                                    double desiredGap = isVisible ? (w + g_settings.playerMarginLeft + g_settings.playerMarginRight) : 0.0;
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
                                                leftPos = g_settings.playerMarginLeft;
                                            } else if (g_trackPosition == L"left") {
                                                leftPos = point.X - desiredGap + g_settings.playerMarginLeft;
                                                if (startButtonModActiveMod && startButtonOffset > 0) {
                                                    leftPos += startButtonOffset;
                                                }
                                            } else {
                                                leftPos = point.X + g_trackedElement.ActualWidth() + g_settings.playerMarginLeft;
                                            }
                                            auto pm = g_playerGrid.Margin();
                                            if (std::abs(pm.Left - leftPos) > 1.0) {
                                                g_playerGrid.Margin({leftPos, 0, 0, 0});
                                            }
                                        } catch (...) {}
                                    }
                                } catch (...) {
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
            }
            else {
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
                            if (childCol >= insertCol)
                                Grid::SetColumn(child, childCol + 1);
                        }
                    }
                }
                playerGrid.Margin({(double)g_settings.playerMarginLeft, 0,
                                (double)g_settings.playerMarginRight, 0});
                Grid::SetColumn(playerGrid, insertCol);
                targetGrid.Children().Append(playerGrid);
                g_playerColumn = insertCol;
            }
        }
        g_playerGrid      = playerGrid;
        g_injectionParent = targetGrid;
        RefreshPlayerContents();
        g_playerGrid.Visibility(Visibility::Visible);
        g_playerGrid.UpdateLayout();
        if (g_injectionParent) {
            g_injectionParent.UpdateLayout();
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
    } catch (...) {
        Wh_Log(L"InjectPlayerGrid: Exception during injection");
        return false;
    }
}
static void RemovePlayerGrid() {
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
        int playerCol = -1;
        RemoveAnchorDebugOverlays(targetGrid);
        playerCol = RemovePlayerGridChildren(targetGrid);
        bool isTrackingPosition = (g_settings.position == L"taskbar_left_edge" ||
                                g_settings.position == L"taskbar_center_edge" ||
                                g_settings.position == L"taskbar_right_edge" ||
                                g_settings.position == L"taskbar_left_start" ||
                                g_settings.position == L"taskbar_right_start" ||
                                g_settings.position == L"taskbar_after_search_left" ||
                                g_settings.position == L"taskbar_after_search_right" ||
                                g_settings.position == L"taskbar_after_taskview_left" ||
                                g_settings.position == L"taskbar_after_taskview_right" ||
                                g_settings.position == L"taskbar_after_widgets_left" ||
                                g_settings.position == L"taskbar_after_widgets_right");
        if (!isTrackingPosition && playerCol >= 0 && playerCol < (int)targetGrid.ColumnDefinitions().Size()) {
            for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
                auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (child) {
                    int childCol = Grid::GetColumn(child);
                    if (childCol > playerCol)
                        Grid::SetColumn(child, childCol - 1);
                }
            }
            targetGrid.ColumnDefinitions().RemoveAt(playerCol);
        }
        g_playerGrid      = nullptr;
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
static void RefreshPlayerContents() {
    if (!g_playerGrid || g_unloading || g_applyingSettings) return;
    std::wstring      title, artist;
    bool              isPlaying = false, hasMedia = false;
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
    (void)hasMedia;
    bool hasSession = false;
    { std::lock_guard<std::mutex> lk(g_sessionMtx); hasSession = (g_currentSession != nullptr); }
    g_playerGrid.UpdateLayout();
    const std::wstring scrollArtist = artist;
    if (title != g_scrollCachedTitle || scrollArtist != g_scrollCachedArtist) {
        g_scrollCachedTitle  = title;
        g_scrollCachedArtist = scrollArtist;
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
                if (!hasSession) {
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
                std::wstring displayArtist = scrollArtist;
                if (!hasSession) {
                    displayArtist = g_settings.noMediaArtistText;
                } else if (displayArtist.empty()) {
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
    if ((g_titleScroll.active || g_artistScroll.active) &&
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
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(2, isPlaying);
                    ct.Text(winrt::hstring(glyph));
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}
    if (auto fe = FindChildByName(g_playerGrid, kPrevBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSkipPrevious;
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
                bool supported = canSkipNext;
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
                bool supported = canSeek;
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
                bool supported = canSeek;
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
                bool supported = canShuffle;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    bool isEnabled = g_shuffleEnabled.load();
                    const wchar_t* glyph = L"";
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
                bool supported = canRepeat;
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
                            glyph = L"";
                            break;
                        case RepeatMode::All:
                            glyph = L"";
                            break;
                        case RepeatMode::One:
                            glyph = L"";
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
    if (g_settings.showPauseOverlay && g_settings.showAlbumArt) {
        if (auto fe = FindChildByName(g_playerGrid, L"PauseIconOverlay"))
            if (auto overlay = fe.try_as<Border>()) {
                try {
                    bool showPause = !isPlaying;
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
    if (auto fe = FindChildByName(g_playerGrid, kArtImageName))
        if (auto img = fe.try_as<Controls::Image>()) {
            if (!thumbBytes.empty() && g_settings.showAlbumArt) {
                bool isSameAlbum = (!g_cachedThumbnailBytes.empty() &&
                                title == g_cachedAlbumTitle &&
                                artist == g_cachedAlbumArtist &&
                                thumbBytes == g_cachedThumbnailBytes);
                size_t newHash = (size_t)thumbHash;
                if (newHash != g_cachedPaletteHash && newHash != 0) {
                    g_cachedAlbumPalette = ExtractAlbumPalette(thumbBytes);
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
                                } else if (g_settings.albumArtQuality == L"medium") {
                                    if (g_settings.albumArtMaxHeight > 0) {
                                        bmp.DecodePixelHeight(g_settings.albumArtMaxHeight);
                                    }
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
                    if (g_settings.albumArtEmptyBehavior == L"hide" && thumbBytes.empty()) {
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
                    } else if (g_settings.albumArtEmptyBehavior == L"show_icon" && thumbBytes.empty()) {
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
    RefreshMiniPlayerFlyoutUI();
}
static bool IsFullscreenActive() {
    using Fn = HRESULT(WINAPI*)(int*);
    static Fn pfn = nullptr; static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE h = GetModuleHandleW(L"shell32.dll");
        if (!h) h = LoadLibraryW(L"shell32.dll");
        if (h) pfn = (Fn)GetProcAddress(h, (LPCSTR)2573);
    }
    if (!pfn) return false;
    int s = 0;
    return SUCCEEDED(pfn(&s)) && (s == 2 || s == 3 || s == 4);
}
static void UpdateVisibility() {
    if (!g_playerGrid || g_unloading || g_applyingSettings) return;
    bool hide = false;
    if (g_settings.hideFullscreen && IsFullscreenActive()) hide = true;
    if (!hide && g_hiddenByIdle) hide = true;
    if (!hide) {
        bool hasMedia = false, hasSession = false;
        { std::lock_guard<std::mutex> lk(g_mediaMtx); hasMedia = g_media.hasMedia; }
        { std::lock_guard<std::mutex> lk(g_sessionMtx); hasSession = (g_currentSession != nullptr); }
        if (hasMedia || hasSession) {
            g_lastMediaTime = std::chrono::steady_clock::now();
        }


        if (g_settings.hideWhenNoMedia && !g_miniPlayerFlyoutOpen) {
            if (!hasSession) {
                hide = true;
            }
            else if (!hasMedia) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastMediaTime).count();
                if (elapsed > 2500) {
                    hide = true;
                } else {
                    if (!g_unloading && g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
                }
            }
        }
    }
    try {
        if (g_playerColumn == -1) {
            if (hide) {
                g_playerGrid.Visibility(Visibility::Collapsed);
            } else {
                g_playerGrid.Visibility(Visibility::Visible);
            }
        } else {
            bool isTrackingPosition = (g_settings.position == L"taskbar_left_start" ||
                                    g_settings.position == L"taskbar_right_start" ||
                                    g_settings.position == L"taskbar_after_search_left" ||
                                    g_settings.position == L"taskbar_after_search_right" ||
                                    g_settings.position == L"taskbar_after_taskview_left" ||
                                    g_settings.position == L"taskbar_after_taskview_right" ||
                                    g_settings.position == L"taskbar_after_widgets_left" ||
                                    g_settings.position == L"taskbar_after_widgets_right" ||
                                    g_settings.position == L"taskbar_far_edge_left");
            if (hide && isTrackingPosition && g_settings.enableSmoothPositionAnimation) {
                g_playerGrid.Opacity(0.0);
                SpawnTrackedWorker([]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    try {
                        RunFromWindowThread(g_taskbarWnd, [](void* param) {
                            try {
                                if (g_playerGrid) {
                                    g_playerGrid.Visibility(Visibility::Collapsed);
                                    if (g_injectionParent) {
                                        if (auto targetGrid = g_injectionParent.try_as<Grid>()) {
                                            if (g_playerColumn < (int)targetGrid.ColumnDefinitions().Size()) {
                                                auto colDef = targetGrid.ColumnDefinitions().GetAt(g_playerColumn);
                                                colDef.Width({0.0, GridUnitType::Pixel});
                                            }
                                        }
                                    }
                                    g_playerGrid.MinWidth(0);
                                    g_playerGrid.MaxWidth(0);
                                    g_playerGrid.Width(0);
                                }
                            } catch (...) {}
                        }, nullptr);
                    } catch (...) {}
                });
            } else {
                g_playerGrid.Visibility(hide ? Visibility::Collapsed : Visibility::Visible);
                g_playerGrid.Opacity(hide ? 0.0 : 1.0);
                if (g_injectionParent) {
                    if (auto targetGrid = g_injectionParent.try_as<Grid>()) {
                        if (g_playerColumn < (int)targetGrid.ColumnDefinitions().Size()) {
                            auto colDef = targetGrid.ColumnDefinitions().GetAt(g_playerColumn);
                            if (hide) {
                                colDef.Width({0.0, GridUnitType::Pixel});
                            } else {
                                colDef.Width({1.0, GridUnitType::Auto});
                            }
                        }
                    }
                } else {
                }
                if (hide) {
                    g_playerGrid.MinWidth(0);
                    g_playerGrid.MaxWidth(0);
                    g_playerGrid.Width(0);
                } else {
                    bool hasTextOrButtons = g_settings.showTrackTitle || g_settings.showTrackArtist || (g_settings.showMediaButtons && !g_mediaButtons.empty());
                    if (hasTextOrButtons && g_settings.playerMinWidth > 0) {
                        g_playerGrid.MinWidth((double)g_settings.playerMinWidth);
                    } else {
                        g_playerGrid.MinWidth(0);
                    }
                    if (g_settings.playerMaxWidth > 0) {
                        g_playerGrid.MaxWidth((double)g_settings.playerMaxWidth);
                    } else {
                        g_playerGrid.ClearValue(FrameworkElement::MaxWidthProperty());
                    }
                    g_playerGrid.ClearValue(FrameworkElement::WidthProperty());
                }
            }
        }
        g_playerGrid.UpdateLayout();
        if (g_settings.vizEnabled) {
            bool nowVisible = !hide;
            if (nowVisible && !g_vizCurrentlyVisible) {
                g_vizCurrentlyVisible = true;
                StartVizCaptureThread();
                StartVizTimer();
            } else if (!nowVisible && g_vizCurrentlyVisible) {
                g_vizCurrentlyVisible = false;
                StopVizTimer();
                SpawnTrackedWorker([]() { StopVizCaptureThread(); });
            }
        }
        
        if (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) {
            if (hide) StopScrollTimer();
            else      StartScrollTimer();
        }
    } catch (...) {}
}

// === Full-style per-monitor media player mirrors (experimental) ===
// Each secondary taskbar receives BuildPlayerGrid(), the SAME XAML tree as
// the primary. GSMTC/media transport and settings are shared; mutable UI
// data (cover cache, scrolling, tooltips and visualizer shapes) is per screen.
struct SecondaryMediaView {
    HWND hwnd = nullptr;
    Grid root{nullptr};
    Grid target{nullptr};
    int insertedColumn = -1;
    bool addedColumn = false;
    FrameworkElement anchorElement{nullptr};
    Thickness anchorOriginalMargin{};
    bool anchorOnLeft = true;
    bool usingFallbackAnchor = false;
    bool rightEdgeNeedsTrayClearance = false;
    winrt::event_token anchorLayoutToken{};
    ToolTip tooltip{nullptr};
    std::function<void()> buttonUpdater;
    Style buttonStyle{nullptr};
    SolidColorBrush mediaHoverBrush{nullptr};
    SolidColorBrush mediaPressedBrush{nullptr};
    SolidColorBrush playerHoverBrush{nullptr};
    SolidColorBrush playerPressedBrush{nullptr};
    SolidColorBrush playerBorderBrush{nullptr};
    SolidColorBrush playerBorderPressedBrush{nullptr};
    TextScrollState titleScroll{};
    TextScrollState artistScroll{};
    std::wstring scrollCachedTitle;
    std::wstring scrollCachedArtist;
    std::wstring cachedAlbumTitle;
    std::wstring cachedAlbumArtist;
    std::vector<BYTE> cachedThumbnailBytes;
    int cachedAppIconSize = -1;
    std::vector<VizRect> vizBars;
    std::vector<SolidColorBrush> vizBrushes;
    winrt::Windows::UI::Xaml::DispatcherTimer frameTimer{nullptr};
    winrt::event_token frameToken{};
    bool lastLightTheme = false;
};
[[clang::no_destroy]] static std::list<SecondaryMediaView> g_secondaryMediaViews;

static std::vector<HWND> FindAllCurrentProcessTaskbarWnds() {
    std::vector<HWND> handles;
    EnumWindows([](HWND hwnd, LPARAM p) CALLBACK -> BOOL {
        DWORD pid = 0;
        wchar_t className[64]{};
        if (GetWindowThreadProcessId(hwnd, &pid) &&
            pid == GetCurrentProcessId() &&
            GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
             _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
            reinterpret_cast<std::vector<HWND>*>(p)->push_back(hwnd);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&handles));
    return handles;
}
static SecondaryMediaView* FindSecondaryMediaView(HWND hwnd) {
    for (auto& view : g_secondaryMediaViews)
        if (view.hwnd == hwnd) return &view;
    return nullptr;
}
static double GetAvailableMirrorScrollTextAreaWidth(Grid const& root) {
    try {
        if (g_settings.playerMaxWidth <= 0) return 0.0;
        if (!root) return 0.0;
        auto panelFe = FindChildByName(root, kPanelGridName);
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
        double leftMargin  = (double)g_settings.textAreaLeftMargin;
        double rightMargin = (double)g_settings.textAreaRightMargin;
        double available = total - used - leftMargin - rightMargin;
        if (available < 0.0) available = 0.0;
        return available;
    } catch (...) {
        return 0.0;
    }
}
static void UpdateSecondaryScrollTransforms(SecondaryMediaView& view) {
    if (!view.root || (!g_settings.enableTitleScrolling && !g_settings.enableArtistScrolling)) return;
    bool isLoop = (g_settings.scrollMode == L"loop");
    if (g_settings.enableTitleScrolling) {
        try {
            if (auto fe = FindChildByName(view.root, kTitleScrollViewName)) {
                if (auto cv = fe.try_as<Canvas>()) {
                    int n = VisualTreeHelper::GetChildrenCount(cv);
                    for (int i = 0; i < n; i++) {
                        auto child = VisualTreeHelper::GetChild(cv, i);
                        if (auto tb = child.try_as<TextBlock>()) {
                            auto name = tb.Name();
                            if (name == kTitleBlockName) {
                                Canvas::SetLeft(tb, -view.titleScroll.offset);
                            } else if (isLoop && name == kTitleCloneName) {
                                double gap = view.titleScroll.textWidth + g_settings.loopGap;
                                Canvas::SetLeft(tb, gap - view.titleScroll.offset);
                            }
                        }
                    }
                }
            }
        } catch (...) {}
    }
    if (g_settings.enableArtistScrolling) {
        try {
            if (auto fe = FindChildByName(view.root, kArtistScrollViewName)) {
                if (auto cv = fe.try_as<Canvas>()) {
                    int n = VisualTreeHelper::GetChildrenCount(cv);
                    for (int i = 0; i < n; i++) {
                        auto child = VisualTreeHelper::GetChild(cv, i);
                        if (auto ab = child.try_as<TextBlock>()) {
                            auto name = ab.Name();
                            if (name == kArtistBlockName) {
                                Canvas::SetLeft(ab, -view.artistScroll.offset);
                            } else if (isLoop && name == kArtistCloneName) {
                                double gap = view.artistScroll.textWidth + g_settings.loopGap;
                                Canvas::SetLeft(ab, gap - view.artistScroll.offset);
                            }
                        }
                    }
                }
            }
        } catch (...) {}
    }
}
static void RefreshSecondaryThemeColors(SecondaryMediaView& view) {
    if (!view.root || g_unloading || g_applyingSettings) return;
    g_vizBaseColorDirty = true;
    g_vizPaletteColorsDirty = true;
    try {
        UpdateHoverBrushColors();
        auto textClr = TextColor();
        auto artistClr = ArtistColor();
        auto buttonClr = ButtonColor();
        if (auto bgFe = FindChildByName(view.root, L"FluentMedia_Background")) {
            if (auto bgBorder = bgFe.try_as<Border>()) {
                auto& bgType = g_settings.backgroundType;
                if (bgType == L"album_art_blur") {
                    if (!view.cachedThumbnailBytes.empty()) {
                        int w = (int)bgBorder.ActualWidth();
                        int h = (int)bgBorder.ActualHeight();
                        if (w > 0 && h > 0) {
                            bgBorder.Background(MakeAlbumBlurBrush(view.cachedThumbnailBytes, w, h));
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
        if (auto fe = FindChildByName(view.root, L"FluentMedia_OuterBorder")) {
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
        if (auto fe = FindChildByName(view.root, kTitleBlockName))
            if (auto tb = fe.try_as<TextBlock>()) tb.Foreground(MakeBrush(textClr));
        if (auto fe = FindChildByName(view.root, kArtistBlockName))
            if (auto ab = fe.try_as<TextBlock>()) ab.Foreground(MakeBrush(artistClr));
        for (const wchar_t* name : {kPrevBtnName, kPlayBtnName, kNextBtnName, kRewindBtnName, kForwardBtnName, kShuffleBtnName, kRepeatBtnName, kSwitchSessionsBtnName}) {
            if (auto fe = FindChildByName(view.root, name)) {
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
static void VizApplySecondaryFrame(SecondaryMediaView& view) {
    if (!g_settings.vizEnabled || view.vizBars.empty())
        return;
    UpdateVisualizerPeaks();
    float attack = 0.55f, decay = 0.18f;
    float sensBoost = std::max(0.f, (g_settings.vizSensitivity - 100) / 200.f) * 0.12f;
    switch (g_settings.vizShape) {
        case VizShape::Stereo:  attack = 0.72f; decay = 0.22f + sensBoost; break;
        case VizShape::Mirror:  attack = 0.52f; decay = 0.20f + sensBoost; break;
        case VizShape::Wave:    attack = 0.34f; decay = 0.17f + sensBoost; break;
        case VizShape::Breathe: attack = 0.20f; decay = 0.11f + sensBoost; break;
        default:                decay += sensBoost; break;
    }
    int barCount = std::min((int)view.vizBars.size(), std::max(1, g_settings.vizBars));
    double zoneH = VizZoneHeight();
    double maxBH = std::max(4.0, zoneH - 6.0);
    double minBH = (double)g_settings.vizBarWidth;
    double idlePx = std::min((double)g_settings.vizIdleBarSize, std::max(0.0, maxBH - minBH));
    double range = std::max(0.0, maxBH - minBH - idlePx);
    bool idleHidden = (g_settings.vizIdleBarSize == 0);
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
    const auto& pal0   = g_cachedVizPal0;
    const auto& pal1   = g_cachedVizPal1;
    const auto& cg0    = g_cachedVizCg0;
    const auto& cg1    = g_cachedVizCg1;
    const auto& acrCol = g_cachedVizAcrCol;
    for (int i = 0; i < barCount; i++) {
        float tgt = g_VizTarget[i], cur = g_VizPeak[i];
        float a = attack, d = decay;
        if (g_settings.vizShape == VizShape::Mountain) {
            float dist = fabsf((float)i - ((barCount - 1) * 0.5f));
            if (dist < 0.5f)      { a = 0.85f; d = 0.26f + sensBoost; }
            else if (dist < 1.5f) { a = 0.62f; d = 0.20f + sensBoost; }
            else                  { a = 0.92f; d = 0.34f + sensBoost; }
        }
        float next = cur + (tgt - cur) * ((tgt > cur) ? a : d);
        g_VizPeak[i] = (fabsf(next - cur) > 0.0005f) ? next : tgt;
        float fac = std::max(0.f, g_VizPeak[i]);
        double bh = idleHidden
            ? (fac * maxBH)
            : (minBH + idlePx + fac * range);
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
            view.vizBars[i].Height(bh);
            if (i < (int)view.vizBrushes.size() && view.vizBrushes[i])
                view.vizBrushes[i].Color(c);
        } catch (...) {}
    }
}
static void RefreshSecondaryContents(SecondaryMediaView& view) {
    if (!view.root || g_unloading || g_applyingSettings) return;
    std::wstring      title, artist;
    bool              isPlaying = false, hasMedia = false;
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
    (void)hasMedia;
    bool hasSession = false;
    { std::lock_guard<std::mutex> lk(g_sessionMtx); hasSession = (g_currentSession != nullptr); }
    view.root.UpdateLayout();
    const std::wstring scrollArtist = artist;
    if (title != view.scrollCachedTitle || scrollArtist != view.scrollCachedArtist) {
        view.scrollCachedTitle  = title;
        view.scrollCachedArtist = scrollArtist;
        ResetScrollState(view.titleScroll);
        ResetScrollState(view.artistScroll);
        try {
            if (auto fe = FindChildByName(view.root, kTitleCloneName))
                if (auto cl = fe.try_as<TextBlock>())
                    cl.Visibility(Visibility::Collapsed);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(view.root, kArtistCloneName))
                if (auto cl = fe.try_as<TextBlock>())
                    cl.Visibility(Visibility::Collapsed);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(view.root, kTitleBlockName))
                if (auto tb = fe.try_as<TextBlock>())
                    Canvas::SetLeft(tb, 0.0);
        } catch (...) {}
        try {
            if (auto fe = FindChildByName(view.root, kArtistBlockName))
                if (auto ab = fe.try_as<TextBlock>())
                    Canvas::SetLeft(ab, 0.0);
        } catch (...) {}
    }
    bool titleVisible = false;
    bool artistVisible = false;
    if (auto fe = FindChildByName(view.root, kTitleBlockName))
        if (auto tb = fe.try_as<TextBlock>())
            try {
                std::wstring displayTitle = title;
                if (!hasSession) {
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
                        if (auto viewFe = FindChildByName(view.root, kTitleScrollViewName))
                            viewFe.Visibility(Visibility::Visible);
                    } catch (...) {}
                    if (auto panelFe = FindChildByName(view.root, kPanelGridName)) {
                        panelFe.UpdateLayout();
                    }
                    tb.UpdateLayout();
                    double textW = tb.DesiredSize().Width;
                    if (auto viewFe = FindChildByName(view.root, kTitleScrollViewName)) {
                        if (auto viewCanvas = viewFe.try_as<Canvas>()) {
                            double minW = (double)g_settings.textAreaMinWidth;
                            double maxW = (double)g_settings.textAreaMaxWidth;
                            double viewW = textW;
                            if (maxW > 0 && viewW > maxW) viewW = maxW;
                            double availW = GetAvailableMirrorScrollTextAreaWidth(view.root);
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
                            bool wasActive = view.titleScroll.active;
                            view.titleScroll.textWidth = textW;
                            view.titleScroll.viewWidth = viewW;
                            view.titleScroll.active = (textW > viewW + 2.0);
                            if (!view.titleScroll.active) {
                                view.titleScroll.offset = 0.0;
                                view.titleScroll.forward = true;
                                Canvas::SetLeft(tb, 0.0);
                            } else if (!wasActive) {
                                view.titleScroll.offset = 0.0;
                                view.titleScroll.forward = true;
                                if (g_settings.scrollMode == L"loop") {
                                    view.titleScroll.pausing  = false;
                                    view.titleScroll.pauseTick = 0;
                                } else {
                                    view.titleScroll.pausing  = true;
                                    view.titleScroll.pauseTick = g_settings.scrollPauseDuration;
                                }
                            }
                            if (auto cloneFe = FindChildByName(view.root, kTitleCloneName)) {
                                if (auto clone = cloneFe.try_as<TextBlock>()) {
                                    clone.Text(tb.Text());
                                    clone.Foreground(tb.Foreground());
                                    clone.Visibility(g_settings.scrollMode == L"loop" && view.titleScroll.active
                                        ? Visibility::Visible : Visibility::Collapsed);
                                }
                            }
                        }
                    }
                } else {
                    view.titleScroll.active = false;
                    view.titleScroll.offset = 0.0;
                    if (g_settings.enableTitleScrolling) {
                        try {
                            if (auto viewFe = FindChildByName(view.root, kTitleScrollViewName))
                                viewFe.Visibility(Visibility::Collapsed);
                        } catch (...) {}
                    }
                }
            } catch (...) {}
    if (auto fe = FindChildByName(view.root, kArtistBlockName))
        if (auto ab = fe.try_as<TextBlock>())
            try {
                std::wstring displayArtist = scrollArtist;
                if (!hasSession) {
                    displayArtist = g_settings.noMediaArtistText;
                } else if (displayArtist.empty()) {
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
                        if (auto viewFe = FindChildByName(view.root, kArtistScrollViewName))
                            viewFe.Visibility(Visibility::Visible);
                    } catch (...) {}
                    if (auto panelFe = FindChildByName(view.root, kPanelGridName)) {
                        panelFe.UpdateLayout();
                    }
                    ab.UpdateLayout();
                    double textW = ab.DesiredSize().Width;
                    if (auto viewFe = FindChildByName(view.root, kArtistScrollViewName)) {
                        if (auto viewCanvas = viewFe.try_as<Canvas>()) {
                            double minW = (double)g_settings.textAreaMinWidth;
                            double maxW = (double)g_settings.textAreaMaxWidth;
                            double viewW = textW;
                            if (maxW > 0 && viewW > maxW) viewW = maxW;
                            double availW = GetAvailableMirrorScrollTextAreaWidth(view.root);
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
                            bool wasActive = view.artistScroll.active;
                            view.artistScroll.textWidth = textW;
                            view.artistScroll.viewWidth = viewW;
                            view.artistScroll.active = (textW > viewW + 2.0);
                            if (!view.artistScroll.active) {
                                view.artistScroll.offset = 0.0;
                                view.artistScroll.forward = true;
                                Canvas::SetLeft(ab, 0.0);
                            } else if (!wasActive) {
                                view.artistScroll.offset = 0.0;
                                view.artistScroll.forward = true;
                                if (g_settings.scrollMode == L"loop") {
                                    view.artistScroll.pausing  = false;
                                    view.artistScroll.pauseTick = 0;
                                } else {
                                    view.artistScroll.pausing  = true;
                                    view.artistScroll.pauseTick = g_settings.scrollPauseDuration;
                                }
                            }
                            if (auto cloneFe = FindChildByName(view.root, kArtistCloneName)) {
                                if (auto clone = cloneFe.try_as<TextBlock>()) {
                                    clone.Text(ab.Text());
                                    clone.Foreground(ab.Foreground());
                                    clone.Visibility(g_settings.scrollMode == L"loop" && view.artistScroll.active
                                        ? Visibility::Visible : Visibility::Collapsed);
                                }
                            }
                        }
                    }
                } else {
                    view.artistScroll.active = false;
                    view.artistScroll.offset = 0.0;
                    if (g_settings.enableArtistScrolling) {
                        try {
                            if (auto viewFe = FindChildByName(view.root, kArtistScrollViewName))
                                viewFe.Visibility(Visibility::Collapsed);
                        } catch (...) {}
                    }
                }
            } catch (...) {}
    if ((view.titleScroll.active || view.artistScroll.active) &&
        (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling)) {
        if (view.frameTimer && !view.frameTimer.IsEnabled()) {
            try { view.frameTimer.Start(); } catch (...) {}
        }
    }
    try {
        if (auto stackFe = FindChildByName(view.root, kTextStackName)) {
            bool anyTextVisible = titleVisible || artistVisible;
            stackFe.Visibility(anyTextVisible ? Visibility::Visible : Visibility::Collapsed);
        }
    } catch (...) {}
    if (auto fe = FindChildByName(view.root, kPlayBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    const wchar_t* glyph = GetGlyph(2, isPlaying);
                    ct.Text(winrt::hstring(glyph));
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}
    if (auto fe = FindChildByName(view.root, kPrevBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSkipPrevious;
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
    if (auto fe = FindChildByName(view.root, kNextBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSkipNext;
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
    if (auto fe = FindChildByName(view.root, kRewindBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSeek;
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
    if (auto fe = FindChildByName(view.root, kForwardBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canSeek;
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
    if (auto fe = FindChildByName(view.root, kShuffleBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canShuffle;
                btn.IsEnabled(supported);
                if (g_settings.hideUnsupportedButtons) {
                    btn.Visibility(supported ? Visibility::Visible : Visibility::Collapsed);
                } else {
                    btn.Visibility(Visibility::Visible);
                }
                if (auto ct = btn.Content().try_as<TextBlock>()) {
                    bool isEnabled = g_shuffleEnabled.load();
                    const wchar_t* glyph = L"";
                    ct.Text(winrt::hstring(glyph));
                    if (!supported && !g_settings.hideUnsupportedButtons) {
                        ct.Opacity(0.35);
                    } else {
                        ct.Opacity(isEnabled ? 1.0 : 0.4);
                    }
                    ct.Foreground(MakeBrush(ButtonColor()));
                }
            } catch (...) {}
    if (auto fe = FindChildByName(view.root, kRepeatBtnName))
        if (auto btn = fe.try_as<Button>())
            try {
                bool supported = canRepeat;
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
                            glyph = L"";
                            break;
                        case RepeatMode::All:
                            glyph = L"";
                            break;
                        case RepeatMode::One:
                            glyph = L"";
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
    if (auto fe = FindChildByName(view.root, kSwitchSessionsBtnName))
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
    if (g_settings.showPauseOverlay && g_settings.showAlbumArt) {
        if (auto fe = FindChildByName(view.root, L"PauseIconOverlay"))
            if (auto overlay = fe.try_as<Border>()) {
                try {
                    bool showPause = !isPlaying;
                    overlay.Visibility(showPause ? Visibility::Visible : Visibility::Collapsed);
                    if (auto pauseIcon = overlay.Child().try_as<TextBlock>()) {
                        pauseIcon.Text(GetGlyph(2, true));
                        bool useFluent = (g_settings.iconStyle == L"fluent_outline" || g_settings.iconStyle == L"fluent_filled");
                        pauseIcon.FontFamily(Media::FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
                        pauseIcon.FontSize((double)g_settings.pauseOverlayIconSize);
                    }
                    if (showPause) {
                        if (auto artImg = FindChildByName(view.root, kArtImageName)) {
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
    if (auto fe = FindChildByName(view.root, kArtImageName))
        if (auto img = fe.try_as<Controls::Image>()) {
            if (!thumbBytes.empty() && g_settings.showAlbumArt) {
                bool isSameAlbum = (!view.cachedThumbnailBytes.empty() &&
                                title == view.cachedAlbumTitle &&
                                artist == view.cachedAlbumArtist &&
                                thumbBytes == view.cachedThumbnailBytes);
                size_t newHash = (size_t)thumbHash;
                if (newHash != g_cachedPaletteHash && newHash != 0) {
                    g_cachedAlbumPalette = ExtractAlbumPalette(thumbBytes);
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
                                } else if (g_settings.albumArtQuality == L"medium") {
                                    if (g_settings.albumArtMaxHeight > 0) {
                                        bmp.DecodePixelHeight(g_settings.albumArtMaxHeight);
                                    }
                                }
                                bmp.ImageOpened([root = view.root](auto const&, auto const&) {
                                    if (g_unloading || g_applyingSettings || !root) return;
                                    try {
                                        if (auto panelFe = FindChildByName(root, kPanelGridName)) {
                                            panelFe.UpdateLayout();
                                        }
                                        g_needsUiUpdate = true;
                                        if (g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
                                    } catch (...) {}
                                });
                                img.Source(bmp);
                                bmp.SetSourceAsync(rasStream);
                                img.Visibility(Visibility::Visible);
                                view.cachedAlbumTitle = title;
                                view.cachedAlbumArtist = artist;
                                view.cachedThumbnailBytes = thumbBytes;
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
                if (auto bgFe = FindChildByName(view.root, L"FluentMedia_Background")) {
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
                view.cachedAlbumTitle.clear();
                view.cachedAlbumArtist.clear();
                view.cachedThumbnailBytes.clear();
                g_cachedPaletteHash = 0;
                g_blurBgCache.Invalidate();
                if (auto bgFe = FindChildByName(view.root, L"FluentMedia_Background")) {
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
                    if (g_settings.albumArtEmptyBehavior == L"hide" && thumbBytes.empty()) {
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
                    } else if (g_settings.albumArtEmptyBehavior == L"show_icon" && thumbBytes.empty()) {
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
                if (auto bgFe = FindChildByName(view.root, L"FluentMedia_Background")) {
                    if (auto bgBorder = bgFe.try_as<Border>()) {
                        bgBorder.Background(MakeBackgroundBrush());
                    }
                }
            }
            auto textClr = TextColor();
            auto artistClr = ArtistColor();
            if (auto titleFe = FindChildByName(view.root, kTitleBlockName)) {
                if (auto titleBlock = titleFe.try_as<TextBlock>()) {
                    titleBlock.Foreground(SolidColorBrush(textClr));
                }
            }
            if (auto artistFe = FindChildByName(view.root, kArtistBlockName)) {
                if (auto artistBlock = artistFe.try_as<TextBlock>()) {
                    artistBlock.Foreground(SolidColorBrush(artistClr));
                }
            }
            auto buttonClr = ButtonColor();
            for (const auto& btnName : {kPlayBtnName, kPrevBtnName, kNextBtnName,
                                        kRewindBtnName, kForwardBtnName, kShuffleBtnName, kRepeatBtnName}) {
                if (auto btnFe = FindChildByName(view.root, btnName)) {
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
        if (auto fe = FindChildByName(view.root, kAppIconImageName))
            if (auto img = fe.try_as<Controls::Image>()) {
                bool sizeChanged = (view.cachedAppIconSize != g_settings.appIconSize);
                if (sizeChanged && !appIconBytes.empty()) {
                    view.cachedAppIconSize = g_settings.appIconSize;
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
        FrameworkElement tooltipHost = view.root;
        if (g_settings.showFullTitleOnHover && hasSession && (!title.empty() || !artist.empty())) {
            std::wstring tooltipText;
            if (!title.empty())  tooltipText += title;
            if (!title.empty() && !artist.empty()) tooltipText += L"\n\n";
            if (!artist.empty()) tooltipText += artist;
            if (view.tooltip) {
                try {
                    auto content = view.tooltip.Content();
                    if (auto ttText = content.try_as<TextBlock>()) {
                        ttText.Text(winrt::hstring(tooltipText));
                    }
                    view.tooltip.IsEnabled(true);
                } catch (...) {}
                if (tooltipHost) {
                    try { ToolTipService::SetToolTip(tooltipHost, view.tooltip); } catch (...) {}
                }
            }
        } else {
            if (view.tooltip) {
                try {
                    auto content = view.tooltip.Content();
                    if (auto ttText = content.try_as<TextBlock>()) {
                        ttText.Text(L"");
                    }
                    view.tooltip.IsEnabled(false);
                } catch (...) {}
            }


            if (tooltipHost) {
                try { ToolTipService::SetToolTip(tooltipHost, nullptr); } catch (...) {}
            }
        }
    } catch (...) {}
    // Flyout UI belongs to the primary player.
}

static void UpdateSecondaryVisibility(SecondaryMediaView& view) {
    if (!view.root) return;
    bool hide = g_hiddenByIdle;
    if (g_settings.hideFullscreen && IsFullscreenActive()) hide = true;
    bool hasMedia = false, hasSession = false;
    { std::lock_guard<std::mutex> lock(g_mediaMtx); hasMedia = g_media.hasMedia; }
    { std::lock_guard<std::mutex> lock(g_sessionMtx); hasSession = g_currentSession != nullptr; }
    if (g_settings.hideWhenNoMedia && !hasSession && !hasMedia) hide = true;
    try {
        view.root.Visibility(hide ? Visibility::Collapsed : Visibility::Visible);
        view.root.Opacity(hide ? 0 : 1);
    } catch (...) {}
}
static void SecondaryFrameTick(HWND hwnd) {
    if (g_unloading || g_applyingSettings) return;
    auto* view = FindSecondaryMediaView(hwnd);
    if (!view || !view->root) return;
    if (g_settings.vizEnabled && view->root.Visibility() == Visibility::Visible) {
        try { VizApplySecondaryFrame(*view); } catch (...) {}
    }
    if (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) {
        if (view->titleScroll.active || view->artistScroll.active) {
            int step = std::max(1, g_settings.scrollSpeed);
            TickScrollState(view->titleScroll, step, g_settings.scrollPauseDuration, g_settings.scrollMode);
            TickScrollState(view->artistScroll, step, g_settings.scrollPauseDuration, g_settings.scrollMode);
            try { UpdateSecondaryScrollTransforms(*view); } catch (...) {}
        }
    }
}
static void RemoveSecondaryMediaViewOnThread(HWND hwnd) {
    for (auto it = g_secondaryMediaViews.begin(); it != g_secondaryMediaViews.end();) {
        if (it->hwnd != hwnd) { ++it; continue; }
        try {
            if (it->frameTimer) {
                it->frameTimer.Stop();
                it->frameTimer.Tick(it->frameToken);
                it->frameTimer = nullptr;
            }
            if (it->target && it->anchorLayoutToken.value) {
                it->target.LayoutUpdated(it->anchorLayoutToken);
                it->anchorLayoutToken = {};
            }
            if (it->anchorElement) {
                // Restore the original taskbar margin for every tracked button.
                try { it->anchorElement.Margin(it->anchorOriginalMargin); } catch (...) {}
                it->anchorElement = nullptr;
            }
            if (it->root && it->target) {
                auto children = it->target.Children();
                for (uint32_t i = 0; i < children.Size(); ++i) {
                    if (children.GetAt(i) == it->root) { children.RemoveAt(i); break; }
                }
                if (it->addedColumn && it->insertedColumn >= 0 &&
                    it->insertedColumn < (int)it->target.ColumnDefinitions().Size()) {
                    int col = it->insertedColumn;
                    for (uint32_t i = 0; i < children.Size(); ++i) {
                        if (auto child = children.GetAt(i).try_as<FrameworkElement>()) {
                            int childCol = Grid::GetColumn(child);
                            if (childCol > col) Grid::SetColumn(child, childCol - 1);
                        }
                    }
                    it->target.ColumnDefinitions().RemoveAt(col);
                }
            }
        } catch (...) { Wh_Log(L"Mirror: secondary removal failed"); }
        it = g_secondaryMediaViews.erase(it);
    }
}
static void RefreshSecondaryMediaViewOnThread(HWND hwnd) {
    if (g_unloading || g_applyingSettings) return;
    auto* view = FindSecondaryMediaView(hwnd);
    if (!view) return;
    if (view->lastLightTheme != IsSystemLightTheme()) {
        view->lastLightTheme = IsSystemLightTheme();
        // The XAML style and hover brushes are owned by the UI thread that
        // created this view. Restore the primary globals immediately afterward.
        std::swap(g_fluentMediaButtonStyle, view->buttonStyle);
        std::swap(g_mediaHoverBrush, view->mediaHoverBrush);
        std::swap(g_mediaPressedBrush, view->mediaPressedBrush);
        std::swap(g_playerHoverBrush, view->playerHoverBrush);
        std::swap(g_playerPressedBrush, view->playerPressedBrush);
        std::swap(g_playerBorderBrush, view->playerBorderBrush);
        std::swap(g_playerBorderPressedBrush, view->playerBorderPressedBrush);
        try { RefreshSecondaryThemeColors(*view); } catch (...) {}
        std::swap(g_playerBorderPressedBrush, view->playerBorderPressedBrush);
        std::swap(g_playerBorderBrush, view->playerBorderBrush);
        std::swap(g_playerPressedBrush, view->playerPressedBrush);
        std::swap(g_playerHoverBrush, view->playerHoverBrush);
        std::swap(g_mediaPressedBrush, view->mediaPressedBrush);
        std::swap(g_mediaHoverBrush, view->mediaHoverBrush);
        std::swap(g_fluentMediaButtonStyle, view->buttonStyle);
    }
    try {
        RefreshSecondaryContents(*view);
        UpdateSecondaryVisibility(*view);
    } catch (...) { Wh_Log(L"Mirror: secondary refresh failed"); }
}
// On secondary taskbars the Widgets (AugmentedEntryPointButton) is normally
// absent.  Unlike the previous mirror implementation, follow the real XAML
// anchor and reserve layout space, just as InjectPlayerGrid() does on primary.
// If there is no matching anchor, optionally use Start as a fallback.
static FrameworkElement FindSecondaryStartAnchor(Grid const& target) {
    auto repeater = FindChildByName(target, L"TaskbarFrameRepeater");
    if (repeater) {
        auto found = FindElementInRepeater(repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
        if (found) return found;
    }
    // Secondary taskbar trees differ between Windows builds; accept nested
    // StartButton instances rather than assuming the primary tree shape.
    for (auto name : kStartButtonNames) {
        if (auto found = FindChildByName(target, name)) return found;
    }
    return nullptr;
}
static FrameworkElement FindSecondarySearchAnchor(Grid const& target) {
    auto repeater = FindChildByName(target, L"TaskbarFrameRepeater");
    if (!repeater) return nullptr;
    auto found = FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
    if (!found) found = FindChildByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
    if (!found) {
        for (auto name : { L"SearchButton", L"SearchBox", L"SearchBoxButton" }) {
            found = FindChildByName(repeater, name);
            if (found) break;
        }
    }
    return found;
}
static FrameworkElement FindSecondaryTaskViewAnchor(Grid const& target) {
    auto repeater = FindChildByName(target, L"TaskbarFrameRepeater");
    if (!repeater) return nullptr;
    // Prefer a semantic name. The second ExperienceToggleButton is not always
    // Task View on secondary taskbars or on different Windows 11 builds.
    for (auto name : { L"TaskViewButton", L"TaskViewToggleButton", L"TaskViewButtonContainer" }) {
        if (auto found = FindChildByName(repeater, name)) return found;
    }
    auto found = FindNthElementByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
    if (!found) found = FindChildByClassName(repeater, L"Taskbar.ExperienceToggleButton");
    return found;
}
static FrameworkElement FindSecondaryWidgetsAnchor(Grid const& target) {
    auto repeater = FindChildByName(target, L"TaskbarFrameRepeater");
    if (!repeater) return nullptr;
    auto found = FindChildByName(repeater, L"AugmentedEntryPointButton");
    if (!found) found = FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton");
    return found;
}
static FrameworkElement FindSecondaryPositionAnchor(
    Grid const& target, bool& left, bool& fallback) {
    const auto& position = g_settings.position;
    left = position == L"taskbar_left_start" ||
           position == L"taskbar_after_search_left" ||
           position == L"taskbar_after_taskview_left" ||
           position == L"taskbar_after_widgets_left";
    fallback = false;
    FrameworkElement anchor = nullptr;
    if (position == L"taskbar_left_start" || position == L"taskbar_right_start") {
        anchor = FindSecondaryStartAnchor(target);
    } else if (position == L"taskbar_after_search_left" ||
               position == L"taskbar_after_search_right") {
        anchor = FindSecondarySearchAnchor(target);
    } else if (position == L"taskbar_after_taskview_left" ||
               position == L"taskbar_after_taskview_right") {
        anchor = FindSecondaryTaskViewAnchor(target);
        if (!anchor && g_settings.mirrorMissingAnchor == L"start") {
            anchor = FindSecondarySearchAnchor(target);
            fallback = anchor != nullptr;
        }
    } else if (position == L"taskbar_after_widgets_left" ||
               position == L"taskbar_after_widgets_right") {
        anchor = FindSecondaryWidgetsAnchor(target);
        // Keep Widgets -> Start fallback, as this is already verified by the user.
    } else {
        return nullptr; // taskbar edges and tray use independent layout handling
    }
    if (!anchor && g_settings.mirrorMissingAnchor == L"start") {
        anchor = FindSecondaryStartAnchor(target);
        fallback = anchor != nullptr;
    }
    return anchor;
}
static void AlignSecondaryMediaView(SecondaryMediaView& view) {
    if (!view.root || !view.target || g_unloading) return;
    try {
        // The right-edge overlay must avoid the tray, just like the original
        // single-screen implementation. Do this on LayoutUpdated because
        // the tray width can be zero until the secondary XAML tree is ready.
        if (view.rightEdgeNeedsTrayClearance) {
            double right = (double)g_settings.playerMarginRight -
                           (double)g_settings.mirrorSecondaryOffsetX;
            if (auto tray = FindChildByName(view.target, L"SystemTrayFrameGrid")) {
                if (tray.Visibility() == Visibility::Visible)
                    right += tray.ActualWidth() + 4.0;
            }
            auto margin = view.root.Margin();
            if (std::abs(margin.Right - right) > 0.75)
                view.root.Margin({margin.Left, 0, right, 0});
            return;
        }
        if (!view.anchorElement) return;
        bool visible = view.root.Visibility() == Visibility::Visible;
        double width = visible ? view.root.ActualWidth() : 0.0;
        // Use the *actual* player width, including custom text sizes and visualizer.
        double gap = visible ? width + g_settings.playerMarginLeft + g_settings.playerMarginRight : 0.0;
        auto actualMargin = view.anchorElement.Margin();
        auto wantedMargin = view.anchorOriginalMargin;
        if (view.anchorOnLeft) wantedMargin.Left += gap;
        else wantedMargin.Right += gap;
        double current = view.anchorOnLeft ? actualMargin.Left : actualMargin.Right;
        double desired = view.anchorOnLeft ? wantedMargin.Left : wantedMargin.Right;
        if (std::abs(current - desired) > 0.75) {
            view.anchorElement.Margin(wantedMargin);
        }
        if (!visible) return;
        auto transform = view.anchorElement.TransformToVisual(view.target);
        auto point = transform.TransformPoint({0, 0});
        double x = view.anchorOnLeft
            ? point.X - gap + g_settings.playerMarginLeft
            : point.X + view.anchorElement.ActualWidth() + g_settings.playerMarginLeft;
        x += g_settings.mirrorSecondaryOffsetX;
        // Negative margins cause the player to be clipped at the taskbar edge.
        x = std::max(0.0, x);
        auto currentPlayerMargin = view.root.Margin();
        if (std::abs(currentPlayerMargin.Left - x) > 0.75 ||
            std::abs(currentPlayerMargin.Right) > 0.75) {
            view.root.Margin({x, 0, 0, 0});
        }
    } catch (...) {
        Wh_Log(L"Mirror: secondary anchor alignment failed for %p", view.hwnd);
    }
}

static void CreateSecondaryMediaViewOnThread(HWND hwnd) {
    if (g_unloading || !g_settings.mirrorAllMonitors ||
        !IsWindow(hwnd) || hwnd == g_taskbarWnd || FindSecondaryMediaView(hwnd)) return;
    try {
        auto xamlRoot = GetTaskbarXamlRoot(hwnd);
        if (!xamlRoot) return; // retry when secondary XAML root becomes ready
        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content) return; // retry while Explorer is creating taskbar UI
        auto [target, insertionColumn] = ResolveInjectionTarget(content, g_settings.position);
        bool intoTray = target && target.Name() == L"SystemTrayFrameGrid" && insertionColumn >= 0;
        if (!target) target = FindTaskbarRootGrid(content);
        if (!target || target.ActualWidth() < 1.0 || target.ActualHeight() < 1.0)
            return; // Explorer is still building this secondary taskbar

        g_secondaryMediaViews.emplace_back();
        auto& view = g_secondaryMediaViews.back();
        view.hwnd = hwnd;
        view.target = target;
        view.lastLightTheme = IsSystemLightTheme();

        // BuildPlayerGrid uses global single-instance references for its own
        // construction; temporarily isolate and restore those values.
        std::swap(*g_vizBars, view.vizBars);
        std::swap(*g_vizBrushes, view.vizBrushes);
        auto primaryUpdater = std::move(g_playerButtonStateUpdater);
        auto primaryTooltip = g_playerToolTip;
        auto primaryStyle = g_fluentMediaButtonStyle;
        auto primaryMediaHover = g_mediaHoverBrush;
        auto primaryMediaPressed = g_mediaPressedBrush;
        auto primaryPlayerHover = g_playerHoverBrush;
        auto primaryPlayerPressed = g_playerPressedBrush;
        auto primaryPlayerBorder = g_playerBorderBrush;
        auto primaryPlayerBorderPressed = g_playerBorderPressedBrush;
        g_playerButtonStateUpdater = {};
        g_playerToolTip = nullptr;
        g_fluentMediaButtonStyle = nullptr;
        g_mediaHoverBrush = nullptr;
        g_mediaPressedBrush = nullptr;
        g_playerHoverBrush = nullptr;
        g_playerPressedBrush = nullptr;
        g_playerBorderBrush = nullptr;
        g_playerBorderPressedBrush = nullptr;
        Grid mirrorRoot = BuildPlayerGrid(hwnd);
        view.buttonUpdater = std::move(g_playerButtonStateUpdater);
        view.tooltip = g_playerToolTip;
        view.buttonStyle = g_fluentMediaButtonStyle;
        view.mediaHoverBrush = g_mediaHoverBrush;
        view.mediaPressedBrush = g_mediaPressedBrush;
        view.playerHoverBrush = g_playerHoverBrush;
        view.playerPressedBrush = g_playerPressedBrush;
        view.playerBorderBrush = g_playerBorderBrush;
        view.playerBorderPressedBrush = g_playerBorderPressedBrush;
        g_playerButtonStateUpdater = std::move(primaryUpdater);
        g_playerToolTip = primaryTooltip;
        g_fluentMediaButtonStyle = primaryStyle;
        g_mediaHoverBrush = primaryMediaHover;
        g_mediaPressedBrush = primaryMediaPressed;
        g_playerHoverBrush = primaryPlayerHover;
        g_playerPressedBrush = primaryPlayerPressed;
        g_playerBorderBrush = primaryPlayerBorder;
        g_playerBorderPressedBrush = primaryPlayerBorderPressed;
        std::swap(*g_vizBars, view.vizBars);
        std::swap(*g_vizBrushes, view.vizBrushes);
        if (!mirrorRoot) {
            g_secondaryMediaViews.pop_back();
            return;
        }
        view.root = mirrorRoot;
        if (intoTray) {
            ColumnDefinition col;
            col.Width({1.0, GridUnitType::Auto});
            int sz = (int)target.ColumnDefinitions().Size();
            int insertAt = std::max(0, std::min(insertionColumn, sz));
            if (insertAt == sz) target.ColumnDefinitions().Append(col);
            else {
                target.ColumnDefinitions().InsertAt(insertAt, col);
                auto children = target.Children();
                for (uint32_t i = 0; i < children.Size(); ++i) {
                    if (auto child = children.GetAt(i).try_as<FrameworkElement>()) {
                        int oldCol = Grid::GetColumn(child);
                        if (oldCol >= insertAt) Grid::SetColumn(child, oldCol + 1);
                    }
                }
            }
            Grid::SetColumn(mirrorRoot, insertAt);
            view.insertedColumn = insertAt;
            view.addedColumn = true;
            mirrorRoot.HorizontalAlignment(HorizontalAlignment::Left);
            mirrorRoot.Margin({(double)g_settings.playerMarginLeft, 0,
                               (double)g_settings.playerMarginRight, 0});
        } else {
            // Follow the actual XAML anchor on this taskbar instead of blindly
            // positioning the player at its left/right edge.
            bool anchorLeft = true, usedFallback = false;
            auto anchor = FindSecondaryPositionAnchor(target, anchorLeft, usedFallback);
            view.anchorElement = anchor;
            view.anchorOnLeft = anchorLeft;
            view.usingFallbackAnchor = usedFallback;
            if (anchor) view.anchorOriginalMargin = anchor.Margin();
            // Every taskbar_* location other than explicit center/right edge
            // belongs in the task-button region. An absent search/task-view/
            // Widgets anchor must never send the player over the system tray.
            bool edgeLeft = g_settings.position != L"taskbar_right_edge" &&
                            g_settings.position != L"taskbar_center_edge";
            bool center = g_settings.position == L"taskbar_center_edge";
            view.rightEdgeNeedsTrayClearance =
                g_settings.position == L"taskbar_right_edge";
            mirrorRoot.HorizontalAlignment(anchor ? HorizontalAlignment::Left :
                (center ? HorizontalAlignment::Center :
                    (edgeLeft ? HorizontalAlignment::Left : HorizontalAlignment::Right)));
            double offset = (double)g_settings.mirrorSecondaryOffsetX;
            double initialLeft = anchor ? 0.0 : (double)g_settings.playerMarginLeft + offset;
            double initialRight = anchor ? 0.0 : (double)g_settings.playerMarginRight - offset;
            mirrorRoot.Margin({initialLeft, 0, initialRight, 0});
            Grid::SetColumn(mirrorRoot, 0);
            Grid::SetColumnSpan(mirrorRoot, std::max(1, (int)target.ColumnDefinitions().Size()));
        }
        Canvas::SetZIndex(mirrorRoot, 1000);
        target.Children().Append(mirrorRoot);
        if (view.anchorElement || view.rightEdgeNeedsTrayClearance) {
            // No raw view pointer is captured: the HWND is resolved against the
            // live registry on every XAML layout pass.
            view.anchorLayoutToken = target.LayoutUpdated(
                [hwnd](auto const&, auto const&) {
                    if (auto* currentView = FindSecondaryMediaView(hwnd))
                        AlignSecondaryMediaView(*currentView);
                });
            AlignSecondaryMediaView(view);
            Wh_Log(L"Mirror: positioned on %p, anchor=%d, fallback=%d, tray-clearance=%d", hwnd,
                   view.anchorElement ? 1 : 0, view.usingFallbackAnchor ? 1 : 0,
                   view.rightEdgeNeedsTrayClearance ? 1 : 0);
        }

        // Shared capture thread, separate animation timer/UI shapes per taskbar.
        if (g_settings.vizEnabled || g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) {
            view.frameTimer = winrt::Windows::UI::Xaml::DispatcherTimer();
            view.frameTimer.Interval(winrt::Windows::Foundation::TimeSpan{
                std::chrono::milliseconds(16)});
            view.frameToken = view.frameTimer.Tick([hwnd](auto const&, auto const&) {
                SecondaryFrameTick(hwnd);
            });
            view.frameTimer.Start();
        }
        RefreshSecondaryMediaViewOnThread(hwnd);
        Wh_Log(L"Mirror: full player installed on taskbar %p", hwnd);
    } catch (...) {
        Wh_Log(L"Mirror: full secondary XAML creation failed");
        // A failed mirror must never discard or replace the primary.
        RemoveSecondaryMediaViewOnThread(hwnd);
    }
}
static void UpdateSecondaryMediaViews(bool missingOnly = false) {
    if (g_unloading || g_applyingSettings || !g_settings.mirrorAllMonitors) return;
    auto handles = FindAllCurrentProcessTaskbarWnds();
    for (HWND hwnd : handles) {
        if (hwnd == g_taskbarWnd) continue;
        if (missingOnly && FindSecondaryMediaView(hwnd)) continue;
        bool dispatched = RunFromWindowThread(hwnd, [](void* p) {
            HWND h = static_cast<HWND>(p);
            CreateSecondaryMediaViewOnThread(h);
            RefreshSecondaryMediaViewOnThread(h);
        }, hwnd);
        if (!dispatched) {
            // If Explorer's secondary taskbar thread has not begun processing
            // messages yet, the discovery timer will try again later.
            Wh_Log(L"Mirror: secondary taskbar %p not dispatchable yet", hwnd);
        }
    }
}
[[clang::no_destroy]] static winrt::Windows::UI::Xaml::DispatcherTimer g_mirrorDiscoveryTimer{nullptr};
static winrt::event_token g_mirrorDiscoveryToken{};
static void StartMirrorDiscoveryTimerOnPrimaryThread() {
    if (g_unloading || !g_settings.mirrorAllMonitors) return;
    try {
        if (!g_mirrorDiscoveryTimer) {
            g_mirrorDiscoveryTimer = winrt::Windows::UI::Xaml::DispatcherTimer();
            g_mirrorDiscoveryTimer.Interval(winrt::Windows::Foundation::TimeSpan{
                std::chrono::milliseconds(500)});
            g_mirrorDiscoveryToken = g_mirrorDiscoveryTimer.Tick(
                [](auto const&, auto const&) {
                    if (g_unloading || g_applyingSettings) return;
                    UpdateSecondaryMediaViews();
                });
        }
        if (!g_mirrorDiscoveryTimer.IsEnabled()) g_mirrorDiscoveryTimer.Start();
    } catch (...) { Wh_Log(L"Mirror: failed to start discovery timer"); }
}
static void StopMirrorDiscoveryTimerOnPrimaryThread() {
    if (!g_mirrorDiscoveryTimer) return;
    try {
        g_mirrorDiscoveryTimer.Stop();
        g_mirrorDiscoveryTimer.Tick(g_mirrorDiscoveryToken);
        g_mirrorDiscoveryTimer = nullptr;
        g_mirrorDiscoveryToken = {};
    } catch (...) { Wh_Log(L"Mirror: failed to stop discovery timer"); }
}
static void RemoveAllSecondaryMediaViews() {
    // Work from tracked entries, not just present EnumWindows results.
    std::vector<HWND> handles;
    for (auto const& view : g_secondaryMediaViews) handles.push_back(view.hwnd);
    for (HWND hwnd : handles) {
        if (IsWindow(hwnd)) {
            RunFromWindowThread(hwnd, [](void* p) {
                RemoveSecondaryMediaViewOnThread(static_cast<HWND>(p));
            }, hwnd);
        }
    }
    // Unreachable/destroyed HWNDs may have retained stale objects; ensure
    // the next installation does not falsely consider them alive.
    if (g_unloading) g_secondaryMediaViews.clear();
}
// === End full-style per-monitor media player mirrors ===

static void TryMountPlayerOnTaskbar() {
    if (g_unloading || g_applyingSettings || g_playerGrid) return;

    // Let the requested position choose its own XAML host. Taskbar positions
    // can be ready before the system tray, and a failed mount is retried by
    // TimerThreadProc once the taskbar window and its XAML tree are available.
    try {
        if (InjectPlayerGrid()) {
            ShowSuccessNotification();
            UpdateVisibility();
            g_needsUiUpdate = true;
            if (g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
        }
        StartMirrorDiscoveryTimerOnPrimaryThread();
        UpdateSecondaryMediaViews();
    } catch (...) {
        Wh_Log(L"TryMountPlayerOnTaskbar: Exception while initializing the taskbar UI");
    }
}
using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading) {
        return;
    }
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: Taskbar window not found");
        return;
    }
    g_playerGrid      = nullptr;
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
    if (g_settings.vizEnabled) {
        SpawnTrackedWorker([]() {
            StopVizCaptureThread();
            StartVizCaptureThread();
        });
        StartVizTimer();
    }
    if (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) {
        StartScrollTimer();
    }
    RunFromWindowThread(hWnd, [](void*) {
        TryMountPlayerOnTaskbar();
    }, nullptr);
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
    // Windhawk skips the default online cache for local mods. Use the upstream
    // player's cache, which covers these same symbols and can serve builds
    // whose Taskbar.pdb has not been published by Microsoft.
    const WH_HOOK_SYMBOLS_OPTIONS options{
        .optionsSize = sizeof(options),
        .onlineCacheUrl = L"https://ramensoftware.github.io/windhawk-mod-symbol-cache/taskbar-fluent-media-player/",
    };
    if (!WindhawkUtils::HookSymbols(h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks), &options)) {
        return FALSE;
    }
    return TRUE;
}
BOOL Wh_ModInit() {
    g_unloading = false;
    g_applyingSettings = false;
    g_taskbarWnd = nullptr;
    g_needsUiUpdate = false;
    LoadSettings();
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
    if (HWND hWnd = g_taskbarWnd) {
        RunFromWindowThread(hWnd, [](void*) {
            TryMountPlayerOnTaskbar();
        }, nullptr);
    }
}
void Wh_ModUninit() {
    g_unloading = true;
    if (g_taskbarWnd && IsWindow(g_taskbarWnd)) {
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            StopMirrorDiscoveryTimerOnPrimaryThread();
        }, nullptr);
    }
    StopTimerThread();
    RemoveAllSecondaryMediaViews();
    StopMediaThread();
    WaitForTrackedWorkers();
    if (g_taskbarWnd)
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            RemovePlayerGrid();
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
void Wh_ModSettingsChanged() {
    g_applyingSettings = true;
    if (g_taskbarWnd && IsWindow(g_taskbarWnd)) {
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            StopMirrorDiscoveryTimerOnPrimaryThread();
        }, nullptr);
    }
    g_vizBaseColorDirty = true;
    g_vizPaletteColorsDirty = true;
    g_vizCurrentlyVisible = false;
    StopTimerThread();
    RemoveAllSecondaryMediaViews();
    LoadSettings();
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) hWnd = g_taskbarWnd;
    if (hWnd) {
        g_taskbarWnd = hWnd;
        bool ok = RunFromWindowThread(hWnd, [](void*) {
            try {
                RemovePlayerGrid();
                g_cachedAlbumTitle.clear();
                g_cachedAlbumArtist.clear();
                g_cachedThumbnailBytes.clear();
                g_cachedPaletteHash = 0;
                g_blurBgCache.Invalidate();
                if (!g_unloading) {
                    InjectPlayerGrid();
                    g_needsUiUpdate = true;
                }
            } catch (...) {
                Wh_Log(L"Wh_ModSettingsChanged: Exception during RemovePlayerGrid/InjectPlayerGrid");
                g_playerGrid = nullptr;
                g_injectionParent = nullptr;
            }
        }, nullptr);
        if (!ok) {
            Wh_Log(L"Wh_ModSettingsChanged: RunFromWindowThread failed");
        }
    }
    g_applyingSettings = false;
    StartTimerThread();
    if (g_taskbarWnd && IsWindow(g_taskbarWnd)) {
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            StartMirrorDiscoveryTimerOnPrimaryThread();
            UpdateSecondaryMediaViews();
        }, nullptr);
    }
}
