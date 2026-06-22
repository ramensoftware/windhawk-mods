// ==WindhawkMod==
// @id              taskbar-fluent-media-player
// @name            Taskbar Fluent Media Player
// @description     Taskbar Fluent Media Player — is a Windhawk mod that integrates a modern media player with Fluent Design directly into the Windows 11 taskbar. It allows you to control music and view track information seamlessly without interrupting your workflow.
// @description:ru-RU Taskbar Fluent Media Player — это мод Windhawk, который интегрирует современный медиаплеер в стиле Fluent Design прямо в панель задач Windows 11. Он позволяет управлять музыкой и просматривать информацию о треке без прерывания работы.
// @version         1.4.0
// @author          Salyts
// @github          https://github.com/Salyts
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -luuid -luser32 -lwindowsapp -lshell32 -lgdi32 -lshlwapi -lwindowscodecs -ldwmapi -lshcore -lksuser
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Fluent Media Player 1.4.0

**Taskbar Fluent Media Player —** is a Windhawk mod that integrates a modern media player with Fluent Design directly into the Windows 11 taskbar. It allows you to control music and view track information seamlessly without interrupting your workflow.

### [> Russian documentation <](https://github.com/Salyts/Taskbar-Fluent-Media-Player/blob/main/README_RU.md)

![video](https://i.imgur.com/pGFnmiG.gif)
### Key Features:
* **Deep Integration —** Place the player in the tray area (left or right of the clock), next to system icons, or in the center of the taskbar.
* **Fluent UI Effects —** Full support for native Windows 11 materials including **Acrylic**, **Mica**, and **Mica Alt**.
* **Adaptive Design —** The player can automatically adapt its colors based on the album art or follow the system accent color.
* **Full Media Control —** Buttons for Play/Pause, Skip, 5-second Seek, Shuffle, and Repeat modes.
* **Smart Behavior —** Automatically hides the player when no media is playing, when entering full-screen mode, or after a period of inactivity (idle).
* **Audio Visualizer —** A real-time spectrum visualizer (WASAPI loopback + FFT) with multiple bar shapes (Stereo, Mountain, Mirror, Wave, Breathe), color modes (Solid, Dynamic Album, Gradient, Acrylic, Accent), EQ presets, vertical anchoring, and per-side placement.

### Advanced Customization:
* **Visuals —** Customize fonts (Segoe UI, Aptos, etc.), text sizes, padding, and element transparency.
* **Album Art —** Adjustable corner radius, source app icon overlay, and pause overlay effects.
* **Mouse Interactions —** Assign custom actions (Volume control, Track seeking, Session switching) to the mouse wheel, single clicks, or double clicks on different parts of the player.

---

### Credits
* **[Salyts](https://github.com/Salyts) —** Author of Taskbar Fluent Media Player.
* **[GR0UD](https://github.com/GR0UD) —** Audio visualizer (capture + FFT engine) ported from his Taskbar Media Player.

### Report a Bug
If you encounter any issues, bugs, or have suggestions for new features, please report them on the project's GitHub page:
👉 **[Report an Issue on GitHub](https://github.com/Salyts/Taskbar-Fluent-Media-Player/issues)**
*/
// ==/WindhawkModReadme==

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
    - mirrorLayout: false
      $name: Mirror layout
      $name:ru-RU: Зеркальное расположение
      $description: Album art, text, and buttons will be displayed on the opposite side
      $description:ru-RU: Обложка альбома, текст и кнопки будут отображаться с противоположной стороны
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
      - prev: Previous Track
      - play: Play/Pause
      - next: Next Track
      - rewind: Rewind 5s
      - forward: Forward 5s
      - shuffle: Toggle Shuffle
      - repeat: Toggle Repeat
      - switch_sessions: Switch Sessions
      $options:ru-RU:
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
    - vizBarSize: "5 15"
      $name: Bar size (width height)
      $name:ru-RU: Размер столбика (ширина высота)
      $description: Two values "width height" — bar width in px and idle (resting) height in %.
      $description:ru-RU: Два значения "ширина высота" — ширина столбика в пикселях и высота покоя в %.
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
        - action: play_pause
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
          - mouse_wheel_up:   Mouse wheel up
          - mouse_wheel_down: Mouse wheel down
          $options:ru-RU:
          - none:             Ничего
          - mouse_wheel:      Колесо мыши
          - mouse_wheel_up:   Колесо мыши вверх
          - mouse_wheel_down: Колесо мыши вниз
        - action: switch_tracks
          $name: Action
          $name:ru-RU: Действие
          $options:
          - none:              Nothing
          - "switch_tracks":   "Switch tracks"
          - "switch_sessions": "Switch sessions"
          - "system_sound":    "Change system sound volume"
          - "app_sound":       "Change app sound volume"
          $options:ru-RU:
          - none:              Ничего
          - "switch_tracks":   "Переключить треки"
          - "switch_sessions": "Переключить сессии"
          - "system_sound":    "Изменить громкость системы"
          - "app_sound":       "Изменить громкость приложения"
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
  $name: Animation Settings
  $name:ru-RU: Настройки анимации

- NotificationSettings:
  - showSuccessNotification: false
    $name: Show notification on successful mod load
    $name:ru-RU: Показывать уведомление при успешной загрузке мода
    $description: Display a Windows notification when the mod is successfully loaded or reloaded
    $description:ru-RU: Отображать уведомление Windows при успешной загрузке или перезагрузке мода
  $name: Notification Settings
  $name:ru-RU: Настройки уведомлений

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
#include <set>
#include <algorithm>
#include <thread>
#include <cmath>
#include <chrono>

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
    std::wstring position             = L"tray_left";
    std::wstring albumArtLeftClick    = L"none";
    std::wstring albumArtRightClick   = L"none";
    std::wstring albumArtMiddleClick  = L"none";
    std::wstring albumArtLeftDoubleClick  = L"none";
    std::wstring albumArtRightDoubleClick = L"none";
    std::wstring albumArtWheelAction  = L"none";
    std::wstring playerLeftClick      = L"none";
    std::wstring playerRightClick     = L"none";
    std::wstring playerMiddleClick    = L"none";
    std::wstring playerLeftDoubleClick  = L"none";
    std::wstring playerRightDoubleClick = L"none";
    std::wstring playerWheelAction    = L"none";
    bool         mirrorLayout         = false;
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
    bool         showLayoutAnchors    = false;
    bool         showRestartButton    = false;
    bool         showSuccessNotification = false;
    bool         hideUnsupportedButtons  = false;
    bool         disableAlbumArtClick    = false;
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
    int          vizIdleBarSize  = 15;
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
    auto Str = [](const wchar_t* key, const wchar_t* def) -> std::wstring {
        PCWSTR p = Wh_GetStringSetting(key);
        std::wstring r = (*p != L'\0') ? p : def;
        Wh_FreeStringSetting(p);
        return r;
    };
    auto StrAllowEmpty = [](const wchar_t* key, const wchar_t* def) -> std::wstring {
        PCWSTR p = Wh_GetStringSetting(key);
        std::wstring r = p ? p : def;
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
    g_settings.showMediaButtons     = Wh_GetIntSetting(L"MainSettings.MediaButtonsSettings.showMediaButtons") != 0;
    ParseMargin(L"MainSettings.MediaButtonsSettings.mediaButtonsMargin", L"2 2", g_settings.mediaButtonsLeftMargin, g_settings.mediaButtonsRightMargin);
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
        int w = 5, h = 15;
        ParseTwoInts(Str(L"MainSettings.VisualizerFunctionsSettings.vizBarSize", L"5 15"), w, h);
        g_settings.vizBarWidth    = std::clamp(w, 1, 40);
        g_settings.vizIdleBarSize = std::clamp(h, 0, 100);
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

    g_settings.albumArtLeftClick        = L"none";
    g_settings.albumArtRightClick       = L"none";
    g_settings.albumArtMiddleClick      = L"none";
    g_settings.albumArtLeftDoubleClick  = L"none";
    g_settings.albumArtRightDoubleClick = L"none";
    g_settings.playerLeftClick          = L"none";
    g_settings.playerRightClick         = L"none";
    g_settings.playerMiddleClick        = L"none";
    g_settings.playerLeftDoubleClick    = L"none";
    g_settings.playerRightDoubleClick   = L"none";

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
        } else if (object == L"player") {
            if (click == L"left_click") g_settings.playerLeftClick = action;
            else if (click == L"right_click") g_settings.playerRightClick = action;
            else if (click == L"middle_click") g_settings.playerMiddleClick = action;
            else if (click == L"left_double_click") g_settings.playerLeftDoubleClick = action;
            else if (click == L"right_double_click") g_settings.playerRightDoubleClick = action;
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

    g_settings.enableSmoothPositionAnimation = Wh_GetIntSetting(L"AnimationSettings.enableSmoothPositionAnimation") != 0;

    g_settings.showSuccessNotification = Wh_GetIntSetting(L"NotificationSettings.showSuccessNotification") != 0;

    g_settings.hideUnsupportedButtons  = Wh_GetIntSetting(L"MainSettings.MediaButtonsSettings.hideUnsupportedButtons") != 0;

    g_settings.disableAlbumArtClick    = Wh_GetIntSetting(L"BehaviorSettings.disableAlbumArtClick") != 0;

    g_settings.ignoredProcesses     = Str(L"DebugSettings.ignoredProcesses", L"");
    g_settings.enableTreeDump       = Wh_GetIntSetting(L"DebugSettings.enableTreeDump")    != 0;
    g_settings.showDebugBorders     = Wh_GetIntSetting(L"DebugSettings.showDebugBorders")  != 0;
    g_settings.showLayoutAnchors    = Wh_GetIntSetting(L"DebugSettings.showLayoutAnchors") != 0;
    g_settings.showRestartButton    = Wh_GetIntSetting(L"DebugSettings.showRestartButton") != 0;

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

        if (g_mediaButtons.empty()) {
            Wh_Log(L"LoadSettings: No media buttons configured, showing none");
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
static void ApplySettings();

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_applyingSettings{false};
static std::atomic<bool> g_hookInjectionInProgress{false};
static std::atomic<int>  g_hookFailCount{0};

static IMMDeviceEnumerator* g_pDeviceEnumerator = nullptr;

static const CLSID XIID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID XIID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID XIID_IAudioSessionManager2 = __uuidof(IAudioSessionManager2);

static HWND             g_taskbarWnd      = nullptr;
static Grid             g_playerGrid      = nullptr;
static FrameworkElement g_injectionParent = nullptr;
static int              g_playerColumn    = -1;
static std::atomic<bool> g_needsUiUpdate{false};

static void ShowSuccessNotification() {
    if (!g_settings.showSuccessNotification) {
        return;
    }

    MessageBoxW(
        nullptr,
        L"Media player successfully loaded and ready to use",
        L"Taskbar Fluent Media Player",
        MB_ICONINFORMATION | MB_OK | MB_TOPMOST | MB_SETFOREGROUND
    );
}


static FrameworkElement g_trackedElement = nullptr;
static Thickness g_trackedElementOriginalMargin{};
static bool g_hasTrackedElementOriginalMargin = false;
static std::wstring g_trackPosition = L"";
static winrt::event_token g_layoutUpdateToken{};

using CTaskBand_GetTaskbarHost_t  = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t   = int  (WINAPI*)(void*);
using Std_Ref_Decref_t            = void (WINAPI*)(void*);

static CTaskBand_GetTaskbarHost_t  CTaskBand_GetTaskbarHost_Original  = nullptr;
static TaskbarHost_FrameHeight_t   TaskbarHost_FrameHeight_Original   = nullptr;
static Std_Ref_Decref_t            Std_Ref_Decref_Original            = nullptr;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

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

static GlobalSystemMediaTransportControlsSessionManager g_sessionMgr     = nullptr;
static GlobalSystemMediaTransportControlsSession        g_currentSession = nullptr;
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

static SolidColorBrush g_mediaHoverBrush   = nullptr;
static SolidColorBrush g_mediaPressedBrush = nullptr;
static SolidColorBrush g_playerHoverBrush   = nullptr;
static SolidColorBrush g_playerPressedBrush = nullptr;
static SolidColorBrush g_playerBorderBrush  = nullptr;
static SolidColorBrush g_playerBorderPressedBrush = nullptr;

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

static ObjectAnimationUsingKeyFrames MakeDiscreteObjectAnimation(
    DependencyObject const& target,
    const wchar_t* propertyPath,
    winrt::Windows::Foundation::IInspectable const& value)
{
    ObjectAnimationUsingKeyFrames anim;
    anim.EnableDependentAnimation(true);
    DiscreteObjectKeyFrame kf;
    kf.Value(value);
    anim.KeyFrames().Append(kf);
    Storyboard::SetTarget(anim, target);
    Storyboard::SetTargetProperty(anim, winrt::hstring(propertyPath));
    return anim;
}

static Storyboard MakeRootBorderStoryboard(
    Border const& root,
    Brush const& background,
    Brush const& borderBrush)
{
    Storyboard sb;
    sb.Children().Append(MakeDiscreteObjectAnimation(
        root, L"Background", winrt::box_value(background)));
    sb.Children().Append(MakeDiscreteObjectAnimation(
        root, L"BorderBrush", winrt::box_value(borderBrush)));
    return sb;
}

static VisualState FindVisualState(VisualStateGroup const& group, std::wstring_view name) {
    for (auto const& state : group.States()) {
        if (state.Name() == name) return state;
    }
    return nullptr;
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

static VisualStateGroup FindCommonStatesGroup(Button const& btn) {
    if (auto root = GetButtonTemplateRoot(btn)) {
        for (auto const& group : VisualStateManager::GetVisualStateGroups(root)) {
            if (group.Name() == L"CommonStates") return group;
        }
    }
    for (auto const& group : VisualStateManager::GetVisualStateGroups(btn)) {
        if (group.Name() == L"CommonStates") return group;
    }
    return nullptr;
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
          <VisualStateManager.VisualStateGroups>
            <VisualStateGroup x:Name="CommonStates">
              <VisualState x:Name="Normal"/>
              <VisualState x:Name="PointerOver"/>
              <VisualState x:Name="Pressed"/>
              <VisualState x:Name="Disabled"/>
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

static void ApplyFreshFluentMediaButtonStyle(Button const& btn) {
    if (auto style = CreateFluentMediaButtonStyle()) {
        btn.Style(style);
    }
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

    auto commonStates = FindCommonStatesGroup(btn);
    if (!commonStates) return;

    auto assignStoryboard = [&](std::wstring_view name, Brush const& bg, Brush const& border) {
        if (auto state = FindVisualState(commonStates, name)) {
            state.Storyboard(MakeRootBorderStoryboard(root, bg, border));
        }
    };

    assignStoryboard(L"Normal", normalBackground, normalBorderBrush);
    assignStoryboard(L"PointerOver", pointerOverBackground, pointerOverBorderBrush);
    assignStoryboard(L"Pressed", pressedBackground, pressedBorderBrush);
    assignStoryboard(L"Disabled", disabledBackground, disabledBorderBrush);
}

static void GoToCommonState(Button const& btn, bool effectEnabled, bool pressed, bool hovered) {
    if (!btn) return;
    winrt::hstring stateName = L"Normal";
    if (effectEnabled) {
        if (pressed) {
            stateName = L"Pressed";
        } else if (hovered) {
            stateName = L"PointerOver";
        }
    }
    try {
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
    auto transparent = MakeBrush({0x00, 0, 0, 0});
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
    auto transparent = MakeBrush({0x00, 0, 0, 0});
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
        auto stream = winrt::Windows::Storage::Streams::InMemoryRandomAccessStream();
        auto writer = winrt::Windows::Storage::Streams::DataWriter(stream);
        writer.WriteBytes(thumbBytes);
        writer.StoreAsync().get();
        writer.DetachStream();
        stream.Seek(0);

        auto decoder = winrt::Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream).get();

        auto transform = winrt::Windows::Graphics::Imaging::BitmapTransform();
        auto pixelData = decoder.GetPixelDataAsync(
            winrt::Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8,
            winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Premultiplied,
            transform,
            winrt::Windows::Graphics::Imaging::ExifOrientationMode::RespectExifOrientation,
            winrt::Windows::Graphics::Imaging::ColorManagementMode::DoNotColorManage
        ).get();

        auto pixels = pixelData.DetachPixelData();
        uint32_t w = decoder.PixelWidth();
        uint32_t h = decoder.PixelHeight();

        if (w == 0 || h == 0 || pixels.size() < w * h * 4)
            return {fallbackPrimary, fallbackSecondary};

        struct Bucket { uint32_t r=0, g=0, b=0, n=0; };
        Bucket buckets[16][16][16]{};

        for (uint32_t y = 0; y < h; y += 4) {
            for (uint32_t x = 0; x < w; x += 4) {
                uint32_t idx = (y * w + x) * 4;
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
static bool g_hiddenByIdle = false;
static std::chrono::steady_clock::time_point g_lastMediaTime = std::chrono::steady_clock::now();

static void SwitchMediaSession();

static void SendMediaCommandAsync(int cmd) {
    std::thread([cmd]() {
        if (g_unloading) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            { std::lock_guard<std::mutex> lk(g_sessionMtx); session = g_currentSession; }
            if (session) {
                switch (cmd) {
                    case 1: session.TrySkipPreviousAsync().get();    break;
                    case 2: session.TryTogglePlayPauseAsync().get(); break;
                    case 3: session.TrySkipNextAsync().get();        break;
                    case 4:
                        try {
                            Wh_Log(L"SendMediaCommandAsync: Attempting to stop playback");

                            auto result = session.TryStopAsync().get();
                            Wh_Log(L"SendMediaCommandAsync: TryStopAsync returned %d", result);

                            if (!result) {
                                Wh_Log(L"SendMediaCommandAsync: Stop not supported, using pause + seek fallback");
                                session.TryPauseAsync().get();
                                session.TryChangePlaybackPositionAsync(0).get();
                            }
                        } catch (...) {
                            Wh_Log(L"SendMediaCommandAsync: Stop failed, trying pause + seek fallback");

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
                }
            }
        } catch (...) {}
        winrt::uninit_apartment();
    }).detach();
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
static void AttachToSession(GlobalSystemMediaTransportControlsSession session);
static void SwitchMediaSession();

static void SwitchMediaSession() {
    GlobalSystemMediaTransportControlsSession nextSession{nullptr};
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        if (!g_sessionMgr) return;
        g_userSwitchedSession = true;
        try {
            auto sessions = g_sessionMgr.GetSessions();
            int count = (int)sessions.Size();
            if (count <= 1) return;

            int currentIndex = -1;
            if (g_currentSession) {
                auto curId = g_currentSession.SourceAppUserModelId();
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
    }
    if (nextSession) {
        AttachToSession(nextSession);
    }
}

static void InitAudioDeviceEnumerator() {
    if (!g_pDeviceEnumerator) {
        CoCreateInstance(
            XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
            XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    }
}

static void CleanupAudioDeviceEnumerator() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = nullptr;
    }
}

static bool AdjustAppVolumeByAUMID(const std::wstring& aumid, float volumeDelta) {
    InitAudioDeviceEnumerator();
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

[[maybe_unused]]
static void ExecuteMediaAction(const std::wstring& action, FrameworkElement const& sourceElement = nullptr) {
    if (action == L"none") {
        return;
    } else if (action == L"open_context_menu") {
        if (sourceElement) {
            ShowMediaContextMenu(sourceElement);
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
        std::thread([]() {
            for (DWORD delay : {300, 1200, 2500}) {
                Sleep(delay);
                if (g_unloading) return;
                g_forceSessionRefresh = true;
                OnSessionsChanged();
                FetchPlaybackInfoAsync();
                FetchMediaPropertiesAsync();
            }
        }).detach();
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
        std::thread([]() {
            std::wstring title, appAumid;
            {
                std::lock_guard<std::mutex> lk(g_mediaMtx);
                title = g_media.title;
            }
            {
                std::lock_guard<std::mutex> lk(g_sessionMtx);
                if (g_currentSession) {
                    try {
                        appAumid = std::wstring(g_currentSession.SourceAppUserModelId());
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

        }).detach();
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
                            HICON icon = (HICON)SendMessageW(hWnd, WM_GETICON, ICON_BIG,   0);
                            if (!icon) icon = (HICON)SendMessageW(hWnd, WM_GETICON, ICON_SMALL, 0);
                            if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
                            if (!icon) icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
                            if (icon) {
                                c->exactIcon = icon;
                                GetWindowThreadProcessId(hWnd, &c->exactPid);
                                wchar_t path[MAX_PATH]{};
                                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, c->exactPid);
                                if (hProc) { DWORD sz = MAX_PATH; QueryFullProcessImageNameW(hProc, 0, path, &sz); CloseHandle(hProc); }
                                c->exactPath = path;
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
                        HICON icon = (HICON)SendMessageW(hWnd, WM_GETICON, ICON_BIG,   0);
                        if (!icon) icon = (HICON)SendMessageW(hWnd, WM_GETICON, ICON_SMALL, 0);
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
    Wh_Log(L"FetchMediaPropertiesAsync: Called");
    std::thread([]() {
        if (g_unloading) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            std::wstring aumid;
            {
                std::lock_guard<std::mutex> lk(g_sessionMtx);
                session = g_currentSession;
                if (session) {
                    try {
                        aumid = std::wstring(session.SourceAppUserModelId());
                    } catch (...) {
                        winrt::uninit_apartment();
                        return;
                    }
                }
            }
            if (!session) {
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

                if (auto thumbRef = props.Thumbnail()) {
                    try {
                        auto stream = thumbRef.OpenReadAsync().get();
                        if (stream) {
                            UINT64 sz = stream.Size();
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
                    } catch (...) { thumbBytes.clear(); }
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
                        g_media.appUserModelId = aumid;
                        if (g_settings.showAppIcon) {
                            g_media.appIconBytes   = std::move(appIconBytes);
                            g_media.appIconKey     = appIconKey;
                        }
                        Wh_Log(L"FetchMediaPropertiesAsync: Set hasMedia=%d (title='%s', artist='%s')",
                               g_media.hasMedia, g_media.title.c_str(), g_media.artist.c_str());
                    } catch (...) {

                    }
                }

                if (forceIconRefresh && !appIconBytes.empty()) {
                    std::lock_guard<std::mutex> lk(g_sessionMtx);
                    g_userSwitchedSession = false;
                    Wh_Log(L"FetchMediaPropertiesAsync: Icon refreshed after session switch");
                }
            } catch (...) {

            }
        } catch (...) {}
        Wh_Log(L"FetchMediaPropertiesAsync: About to call DispatchMediaUpdate");
        DispatchMediaUpdate();
        winrt::uninit_apartment();
    }).detach();
}

static void FetchPlaybackInfoAsync() {
    Wh_Log(L"FetchPlaybackInfoAsync: Called");
    std::thread([]() {
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
                        Wh_Log(L"Playback state changed: %s -> %s",
                            wasPlaying ? L"Playing" : L"Not Playing",
                            playing ? L"Playing" : L"Not Playing");
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
                        }
                    } catch (...) {}

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
        Wh_Log(L"FetchPlaybackInfoAsync: About to call DispatchMediaUpdate");
        DispatchMediaUpdate();
        winrt::uninit_apartment();
    }).detach();
}

static void DetachCurrentSession() {
    std::lock_guard<std::mutex> lk(g_sessionMtx);
    if (!g_currentSession) return;
    try {
        if (g_evMediaProps.value) { g_currentSession.MediaPropertiesChanged(g_evMediaProps); g_evMediaProps = {}; }
        if (g_evPlayback.value)   { g_currentSession.PlaybackInfoChanged(g_evPlayback); g_evPlayback = {}; }
    } catch (...) {}
    g_currentSession = nullptr;
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
    if (!g_sessionMgr) return nullptr;
    try {
        auto sessions = g_sessionMgr.GetSessions();
        int sessionCount = sessions.Size();
        Wh_Log(L"PickBestSession: Found %d media sessions", sessionCount);

        GlobalSystemMediaTransportControlsSession best{nullptr};
        int index = 0;
        for (auto const& s : sessions) {
            try {
                std::wstring appId = s.SourceAppUserModelId().c_str();
                if (IsIgnoredMediaApp(appId)) {
                    Wh_Log(L"PickBestSession: Session %d (%s) is ignored", index, appId.c_str());
                    index++;
                    continue;
                }

                auto pb = s.GetPlaybackInfo();
                if (!pb) {
                    Wh_Log(L"PickBestSession: Session %d (%s) has no playback info", index, appId.c_str());
                    index++;
                    continue;
                }

                auto status = pb.PlaybackStatus();
                const wchar_t* statusStr = L"Unknown";
                if (status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) statusStr = L"Playing";
                else if (status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused) statusStr = L"Paused";
                else if (status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Stopped) statusStr = L"Stopped";

                Wh_Log(L"PickBestSession: Session %d (%s) status: %s", index, appId.c_str(), statusStr);

                if (status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                    Wh_Log(L"PickBestSession: Selected playing session %d (%s)", index, appId.c_str());
                    return s;
                }
                if (!best) best = s;
            } catch (...) {
                Wh_Log(L"PickBestSession: Exception processing session %d", index);
            }
            index++;
        }

        if (best) {
            try {
                std::wstring appId = best.SourceAppUserModelId().c_str();
                Wh_Log(L"PickBestSession: No playing session, selected first available (%s)", appId.c_str());
            } catch (...) {
                Wh_Log(L"PickBestSession: No playing session, selected first available (app ID unavailable)");
            }
        } else {
            Wh_Log(L"PickBestSession: No suitable session found");
        }

        return best;
    } catch (...) {
        Wh_Log(L"PickBestSession: Exception occurred");
        return nullptr;
    }
}

static void AttachToSession(GlobalSystemMediaTransportControlsSession session) {
    if (!session) {
        Wh_Log(L"AttachToSession: No session provided, detaching current session");
        DetachCurrentSession();
        {
            std::lock_guard<std::mutex> lk(g_mediaMtx);
            g_media = MediaState{};
            Wh_Log(L"AttachToSession: Cleared media state (hasMedia=false)");
        }
        DispatchMediaUpdate();
        return;
    }

    bool needsReattach = false;
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        if (g_currentSession == session) {
            if (!g_evMediaProps.value || !g_evPlayback.value) {
                Wh_Log(L"AttachToSession: Same session but events not attached, reattaching");
                needsReattach = true;
            } else {
                Wh_Log(L"AttachToSession: Same session already attached, fetching data");
                goto fetch;
            }
        }
    }
    (void)needsReattach;

    try {
        std::wstring appId = session.SourceAppUserModelId().c_str();
        Wh_Log(L"AttachToSession: Attaching to new session from app: %s", appId.c_str());
    } catch (...) {
        Wh_Log(L"AttachToSession: Attaching to new session (app ID unavailable)");
    }

    DetachCurrentSession();

    {
        std::lock_guard<std::mutex> lk(g_mediaMtx);
        g_media.appIconKey   = L"";
        g_media.appIconBytes.clear();
    }
    g_cachedAppIconSize = -1;

    ResetScrollState(g_titleScroll);
    ResetScrollState(g_artistScroll);

    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        g_currentSession = session;
        try {
            g_evMediaProps = g_currentSession.MediaPropertiesChanged([](auto const&, auto const&) {
                Wh_Log(L"MediaPropertiesChanged event fired");
                if (!g_unloading) FetchMediaPropertiesAsync();
            });
            g_evPlayback = g_currentSession.PlaybackInfoChanged([](auto const&, auto const&) {
                Wh_Log(L"PlaybackInfoChanged event fired");
                if (!g_unloading) FetchPlaybackInfoAsync();
            });
            Wh_Log(L"AttachToSession: Event handlers attached successfully");
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
    bool userSwitched = false;
    bool forceRefresh = g_forceSessionRefresh.exchange(false);
    {
        std::lock_guard<std::mutex> lk(g_sessionMtx);
        userSwitched = g_userSwitchedSession;
    }

    try {
        if (g_sessionMgr) {
            auto sessions = g_sessionMgr.GetSessions();
            g_sessionCount.store((int)sessions.Size());
        }
    } catch (...) {}

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
                auto sessions = g_sessionMgr.GetSessions();
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

static DWORD WINAPI MediaThreadProc(void*) {
    Wh_Log(L"MediaThreadProc: Starting media thread");
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        Wh_Log(L"MediaThreadProc: Requesting session manager");
        auto op = GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        while (op.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
            if (WaitForSingleObject(g_mediaStopEvent, 50) == WAIT_OBJECT_0) goto done;
        }
        g_sessionMgr = op.GetResults();
        Wh_Log(L"MediaThreadProc: Session manager obtained");

        g_evSessionsChanged = g_sessionMgr.SessionsChanged([](auto const&, auto const&) {
            Wh_Log(L"MediaThreadProc: SessionsChanged event fired");
            OnSessionsChanged();
        });
        g_evCurrentChanged  = g_sessionMgr.CurrentSessionChanged([](auto const&, auto const&) {
            Wh_Log(L"MediaThreadProc: CurrentSessionChanged event fired");
            OnSessionsChanged();
        });

        Wh_Log(L"MediaThreadProc: Initial session check");
        OnSessionsChanged();

        Wh_Log(L"MediaThreadProc: Waiting 2 seconds before retry check");
        if (WaitForSingleObject(g_mediaStopEvent, 2000) == WAIT_TIMEOUT) {
            if (!g_unloading) {
                Wh_Log(L"MediaThreadProc: Performing retry session check");
                g_forceSessionRefresh = true;
                OnSessionsChanged();
            }
        }

        Wh_Log(L"MediaThreadProc: Entering wait loop");
        WaitForSingleObject(g_mediaStopEvent, INFINITE);

        Wh_Log(L"MediaThreadProc: Shutting down");
        try { if (g_evSessionsChanged.value) g_sessionMgr.SessionsChanged(g_evSessionsChanged); } catch (...) { Wh_Log(L"MediaThreadProc: Failed to unregister SessionsChanged event"); }
        try { if (g_evCurrentChanged.value)  g_sessionMgr.CurrentSessionChanged(g_evCurrentChanged); } catch (...) { Wh_Log(L"MediaThreadProc: Failed to unregister CurrentSessionChanged event"); }
        DetachCurrentSession();
        g_sessionMgr = nullptr;
    done:
        winrt::uninit_apartment();
        Wh_Log(L"MediaThreadProc: Media thread ended");
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
    if (g_mediaThread) { WaitForSingleObject(g_mediaThread, 5000); CloseHandle(g_mediaThread); g_mediaThread = nullptr; }
    if (g_mediaStopEvent) { CloseHandle(g_mediaStopEvent); g_mediaStopEvent = nullptr; }
}

static HANDLE g_timerThread    = nullptr;
static HANDLE g_timerStopEvent = nullptr;
static HANDLE g_timerUpdateEvent = nullptr;

static winrt::Windows::UI::Xaml::DispatcherTimer g_scrollDispatcherTimer{nullptr};
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
    if (!needsScroll) return;

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
        Wh_Log(L"DispatchMediaUpdate: Skipped (unloading=%d, applyingSettings=%d)",
               unloading, applyingSettings);
        return;
    }

    Wh_Log(L"DispatchMediaUpdate: Called");

    g_needsUiUpdate = true;
    if (g_timerUpdateEvent) {
        SetEvent(g_timerUpdateEvent);
        Wh_Log(L"DispatchMediaUpdate: Signaled timer update event");
    }
}

static void RefreshPlayerContents();
static void UpdateVisibility();
static void RefreshThemeColors();
static void RemovePlayerGrid();
static bool InjectPlayerGrid();


static std::atomic<bool> g_themeChangePending{false};

static DWORD WINAPI TimerThreadProc(void*) {
    static bool lastThemeWasLight = IsSystemLightTheme();

    HKEY hKey = nullptr;
    HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_NOTIFY, &hKey) == ERROR_SUCCESS) {
        RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hEvent, TRUE);
    }

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
            if (g_settings.playerHoverEffectMode == L"auto" ||
                g_settings.mediaButtonsHoverEffectMode == L"auto") {
                bool currentThemeIsLight = IsSystemLightTheme();
                if (currentThemeIsLight != lastThemeWasLight) {
                    lastThemeWasLight = currentThemeIsLight;
                    g_themeChangePending = true;
                    g_needsUiUpdate = true;
                    if (g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
                }
            } else {
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
        if (needsUpdate) {
            RunFromWindowThread(hWnd, [](void*) {
                if (g_unloading || g_applyingSettings) return;
                if (g_playerGrid) {
                    g_hookFailCount = 0;
                    RefreshPlayerContents();
                    UpdateVisibility();
                }
            }, nullptr);
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
static bool g_vizCurrentlyVisible = false;
static std::thread* g_CaptureThread = nullptr;
static HANDLE g_hCaptureEvent = nullptr;

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

    winrt::com_ptr<IMMDevice> pDev;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, pDev.put()))) {
        g_CaptureRunning.store(false);
        CoUninitialize();
        return;
    }

    winrt::com_ptr<IAudioClient> pClient;
    winrt::com_ptr<IAudioCaptureClient> pCapture;
    UINT32 sampleRate = 48000, channels = 2;
    bool isFloat = true;

    {
        winrt::com_ptr<IAudioClient> pC;
        if (SUCCEEDED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                                     pC.put_void()))) {
            WAVEFORMATEX* pwfx = nullptr;
            pC->GetMixFormat(&pwfx);
            if (pwfx) {
                sampleRate = pwfx->nSamplesPerSec;
                channels = pwfx->nChannels;
                isFloat =
                    (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
                    (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
                     reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->SubFormat ==
                         KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
                if (SUCCEEDED(pC->Initialize(
                        AUDCLNT_SHAREMODE_SHARED,
                        AUDCLNT_STREAMFLAGS_LOOPBACK |
                            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                        200000, 0, pwfx, nullptr))) {
                    if (g_hCaptureEvent)
                        pC->SetEventHandle(g_hCaptureEvent);
                    winrt::com_ptr<IAudioCaptureClient> pCap;
                    if (SUCCEEDED(pC->GetService(__uuidof(IAudioCaptureClient),
                                                 pCap.put_void()))) {
                        pClient = pC;
                        pCapture = pCap;
                    }
                }
                CoTaskMemFree(pwfx);
            }
        }
    }

    BuildLogBins(sampleRate);

    static constexpr int RING_CAP = VIZ_FFT_SIZE * 4;
    std::vector<float> ringBuf(RING_CAP, 0.f);
    int ringHead = 0, ringCount = 0;
    std::vector<float> re(VIZ_FFT_SIZE), im(VIZ_FFT_SIZE);

    float bandEnv[VIZ_NUM_BANDS] = {};
    static constexpr float GRAVITY[VIZ_NUM_BANDS] = {0.018f, 0.020f, 0.022f, 0.025f,
                                                     0.030f, 0.036f, 0.042f};

    if (pClient)
        pClient->Start();

    while (g_CaptureRunning.load(std::memory_order_relaxed)) {
        if (g_hCaptureEvent)
            WaitForSingleObject(g_hCaptureEvent, 20);
        else
            Sleep(8);

        if (!pCapture)
            continue;

        UINT32 packetSize = 0;
        if (FAILED(pCapture->GetNextPacketSize(&packetSize)) || packetSize == 0) {
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
            if (FAILED(pCapture->GetBuffer(&pData, &numFrames, &flags, nullptr,
                                           nullptr)))
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
            if (FAILED(pCapture->GetNextPacketSize(&packetSize)))
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
    CoUninitialize();
}

static void StartVizCaptureThread() {
    if (g_CaptureRunning.load())
        return;
    if (g_CaptureThread) {
        if (g_CaptureThread->joinable())
            g_CaptureThread->join();
        delete g_CaptureThread;
        g_CaptureThread = nullptr;
    }
    if (!g_hCaptureEvent)
        g_hCaptureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_CaptureRunning.store(true);
    g_CaptureThread = new std::thread(VizCaptureThreadProc);
}

static void StopVizCaptureThread() {
    g_CaptureRunning.store(false);
    if (g_hCaptureEvent)
        SetEvent(g_hCaptureEvent);
    if (g_CaptureThread) {
        if (g_CaptureThread->joinable())
            g_CaptureThread->join();
        delete g_CaptureThread;
        g_CaptureThread = nullptr;
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
    float idleFloor = g_settings.vizIdleBarSize / 100.0f;

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

        g_VizTarget[i] = std::max(idleFloor, std::min(1.f, target));
    }
}

using VizRect = winrt::Windows::UI::Xaml::Shapes::Rectangle;
static std::vector<VizRect> g_vizBars;
static std::vector<SolidColorBrush> g_vizBrushes;

static winrt::Windows::UI::Color VizLerpColor(winrt::Windows::UI::Color a,
                                              winrt::Windows::UI::Color b, float t) {
    auto L = [](BYTE x, BYTE y, float tt) -> BYTE {
        return (BYTE)((int)x + (int)((float)((int)y - (int)x) * tt));
    };
    return winrt::Windows::UI::Color{255, L(a.R, b.R, t), L(a.G, b.G, t), L(a.B, b.B, t)};
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
    if (!g_settings.vizEnabled || g_vizBars.empty())
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

    int barCount = std::min((int)g_vizBars.size(), std::max(1, g_settings.vizBars));
    double zoneH = VizZoneHeight();
    double maxBH = std::max(4.0, zoneH - 6.0);
    double minBH = std::max((double)g_settings.vizBarWidth, 3.0);
    float  idleH = g_settings.vizIdleBarSize / 100.f;

    winrt::Windows::UI::Color baseCol{255, 255, 255, 255};
    if (g_settings.vizColorMode == VizColorMode::DynamicAlbum) {
        baseCol = g_cachedAlbumPalette.primary;
        baseCol.A = 255;
    } else {
        if (g_vizBaseColorDirty) {
            g_cachedVizBaseColor = ParseColorWithThemeSupport(g_settings.vizColor, 255);
            g_vizBaseColorDirty = false;
        }
        baseCol = g_cachedVizBaseColor;
    }

    auto pal0   = g_cachedAlbumPalette.primary;
    auto pal1   = g_cachedAlbumPalette.secondary;
    auto cg0    = ParseColorWithSpecialValues(g_settings.vizColor1, 255);
    auto cg1    = ParseColorWithSpecialValues(g_settings.vizColor2, 255);
    auto acrCol = ParseColorWithThemeSupport(g_settings.vizColor, 255);

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

        float  fac = std::max(idleH, g_VizPeak[i]);
        double bh = minBH + fac * (maxBH - minBH);

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
            g_vizBars[i].Height(bh);
            if (i < (int)g_vizBrushes.size() && g_vizBrushes[i])
                g_vizBrushes[i].Color(c);
        } catch (...) {}
    }
}

static winrt::Windows::UI::Xaml::DispatcherTimer g_vizDispatcherTimer{nullptr};
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
    g_vizBars.clear();
    g_vizBrushes.clear();

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

    double minBH   = std::max((double)g_settings.vizBarWidth, 3.0);
    double corner  = std::max(1.0, g_settings.vizBarWidth * 0.5);

    for (int i = 0; i < barCount; i++) {
        VizRect r;
        r.Width((double)g_settings.vizBarWidth);
        r.Height(minBH);
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
        g_vizBars.push_back(r);
        g_vizBrushes.push_back(br);
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
                    ApplyFreshFluentMediaButtonStyle(btn);
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
                    ApplyFreshFluentMediaButtonStyle(btn);
                    btn.ApplyTemplate();
                    SetupMediaButtonCommonStates(btn);
                    try { VisualStateManager::GoToState(btn, L"Normal", false); } catch (...) {}
                }
            }
        }
    } catch (...) {}
}

static HWND FindCurrentProcessTaskbarWnd() {
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

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) return nullptr;
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) return nullptr;

    size_t taskbarElementIUnknownOffset = 0x10;
#if defined(_M_X64) || defined(__x86_64__)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
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
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern (arm64)");
        }
    }
#else
    Wh_Log(L"GetTaskbarXamlRoot: Unknown architecture, using default offset 0x10");
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement{nullptr};
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

static const wchar_t* GetGlyph(int cmd, bool isPlaying = false) {
    bool isFluent = (g_settings.iconStyle == L"fluent_outline" || g_settings.iconStyle == L"fluent_filled");
    bool isFilled = (g_settings.iconStyle == L"fluent_filled" || g_settings.iconStyle == L"mdl2_filled");

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

        btn.PointerPressed([isPressed, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            if (auto elem = sender.template try_as<UIElement>()) {
                elem.CapturePointer(e.Pointer());
            }
            *isPressed = true;
            updateBtnVisualState();
        });

        btn.PointerReleased([isPressed, isHovered, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
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
        });

        btn.PointerCanceled([isPressed, isHovered, updateBtnVisualState](auto const&, auto const&) {
            *isPressed = false;
            *isHovered = false;
            updateBtnVisualState();
        });

        btn.PointerCaptureLost([isPressed, isHovered, updateBtnVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
            *isPressed = false;
            if (auto elem = sender.template try_as<UIElement>()) {
                try {
                    auto bounds = elem.RenderSize();
                    auto pos = e.GetCurrentPoint(elem).Position();
                    *isHovered = (pos.X >= 0 && pos.X <= bounds.Width && pos.Y >= 0 && pos.Y <= bounds.Height);
                } catch (...) { *isHovered = false; }
            }
            updateBtnVisualState();
        });

    } catch (...) {}
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
        bool useFluent = (g_settings.iconStyle == L"fluent_outline" || g_settings.iconStyle == L"fluent_filled");
        try {
            icon.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
        } catch (...) {
            try {
                icon.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
            } catch (...) {}
        }
        icon.Foreground(MakeBrush(ButtonColor()));
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
    std::thread([]() {
        wchar_t expanded[MAX_PATH]{};
        ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windhawk\\windhawk.exe", expanded, MAX_PATH);

        DWORD attrs = GetFileAttributesW(expanded);
        if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            ShellExecuteW(nullptr, L"open", expanded, nullptr, nullptr, SW_SHOWNORMAL);
        } else {
            ShellExecuteW(nullptr, L"open", L"windhawk.exe", nullptr, nullptr, SW_SHOWNORMAL);
        }
    }).detach();
}

static MenuFlyoutItem MakeMediaContextMenuItem(int cmd, const wchar_t* label, bool enabled, double iconOpacity) {
    MenuFlyoutItem item;
    item.Text(label);
    item.IsEnabled(enabled);

    try {
        FontIcon icon;
        icon.Glyph(GetGlyph(cmd));
        icon.FontSize((double)g_settings.buttonIconSize);
        bool useFluent = (g_settings.iconStyle == L"fluent_outline" || g_settings.iconStyle == L"fluent_filled");
        try {
            icon.FontFamily(FontFamily(useFluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets"));
        } catch (...) {
            try {
                icon.FontFamily(FontFamily(L"Segoe MDL2 Assets"));
            } catch (...) {}
        }
        icon.Foreground(MakeBrush(ButtonColor()));
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

        menu.Items().Append(MakeMediaContextMenuItem(8, L"Toggle Repeat", canRepeat, canRepeat ? 1.0 : 0.4));

        double shuffleOpacity = canShuffle ? (shuffleEnabled ? 1.0 : 0.4) : 0.4;
        menu.Items().Append(MakeMediaContextMenuItem(7, L"Toggle Shuffle", canShuffle, shuffleOpacity));

        menu.Items().Append(MakeMediaContextMenuItem(6, L"Forward 5s", canSeek, canSeek ? 1.0 : 0.4));
        menu.Items().Append(MakeMediaContextMenuItem(5, L"Rewind 5s", canSeek, canSeek ? 1.0 : 0.4));
        menu.Items().Append(MakeMediaContextMenuItem(3, L"Next Track", canSkipNext, canSkipNext ? 1.0 : 0.4));
        menu.Items().Append(MakeMediaContextMenuItem(1, L"Previous Track", canSkipPrevious, canSkipPrevious ? 1.0 : 0.4));

        menu.Items().Append(MakeMediaContextMenuItem(9, L"Switch Sessions", g_sessionCount.load() > 1, g_sessionCount.load() > 1 ? 1.0 : 0.4));
        menu.Items().Append(MakeActionContextMenuItem(L"\uE8A7", L"Open media app", []() {
            ExecuteMediaAction(L"open_app");
        }));

        try {
            Controls::MenuFlyoutSeparator separator;
            menu.Items().Append(separator);
        } catch (...) {}

        if (g_settings.showRestartButton) {
            menu.Items().Append(MakeActionContextMenuItem(L"\uE72C", L"Restart Player", []() {
                try {
                    Wh_Log(L"Restart Player: Starting");
                    StopVizTimer();
                    g_vizCurrentlyVisible = false;
                    RemovePlayerGrid();
                    auto dispatcher = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher();
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [=]() {
                        try {
                            InjectPlayerGrid();
                            Wh_Log(L"Restart Player: Player reinjected successfully");
                        } catch (...) {
                            Wh_Log(L"Restart Player: Exception in InjectPlayerGrid");
                        }
                    });
                } catch (...) {
                    Wh_Log(L"Restart Player: exception");
                }
            }));
        }

        menu.Items().Append(MakeActionContextMenuItem(L"\uE713", L"Open Windhawk", []() {
            OpenWindhawk();
        }));

        menu.ShowAt(target);
    } catch (...) {
        Wh_Log(L"ShowMediaContextMenu: exception");
    }
}

static Grid BuildPlayerGrid() {
    try {
        g_vizBars.clear();
        g_vizBrushes.clear();

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
            artImage.Stretch(isSquare ? Stretch::UniformToFill : Stretch::Uniform);

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
                pauseIcon.Text(L"");
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

                artContainer.PointerReleased([](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
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
                        if (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased) {
                            ExecuteMediaAction(g_settings.albumArtLeftClick, fe);
                        } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased) {
                            ExecuteMediaAction(g_settings.albumArtRightClick, fe);
                        } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::MiddleButtonReleased) {
                            ExecuteMediaAction(g_settings.albumArtMiddleClick, fe);
                        }
                    }
                    e.Handled(true);
                });

                artContainer.DoubleTapped([](auto const& sender, winrt::Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e) mutable {
                    if (g_unloading) return;
                    ExecuteMediaAction(g_settings.albumArtLeftDoubleClick, sender.template try_as<FrameworkElement>());
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
                    } else if (action == L"switch_sessions") {
                        if (delta != 0) SwitchMediaSession();
                    } else if (action == L"system_sound") {
                        ChangeSystemVolume(delta > 0);
                    } else if (action == L"app_sound") {
                        std::thread([delta]() {
                            std::wstring aumid;
                            {
                                std::lock_guard<std::mutex> lk(g_sessionMtx);
                                if (g_currentSession) {
                                    try {
                                        aumid = std::wstring(g_currentSession.SourceAppUserModelId());
                                    } catch (...) {}
                                }
                            }
                            if (!aumid.empty()) {
                                float volumeDelta = (delta > 0) ? 0.02f : -0.02f;
                                AdjustAppVolumeByAUMID(aumid, volumeDelta);
                            }
                        }).detach();
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
                    Wh_Log(L"CreatePlayerGrid: Media buttons vector is empty, using defaults");
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

        Grid wrapper;
        wrapper.Name(kGridName);
        wrapper.VerticalAlignment(VerticalAlignment::Stretch);
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

        if (g_settings.playerMinHeight > 0) {
            wrapper.MinHeight((double)g_settings.playerMinHeight);
        }

        if (g_settings.playerMaxHeight > 0) {
            wrapper.MaxHeight((double)g_settings.playerMaxHeight);
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
            ApplyPlayerButtonState(playerButton, playerNormalBg, *isHovered, *isPressed);
        };

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

        wrapper.PointerReleased([isPressed, isHovered, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) mutable {
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
                if (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased) {
                    ExecuteMediaAction(g_settings.playerLeftClick, fe);
                } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased) {
                    ExecuteMediaAction(g_settings.playerRightClick, fe);
                } else if (kind == winrt::Windows::UI::Input::PointerUpdateKind::MiddleButtonReleased) {
                    ExecuteMediaAction(g_settings.playerMiddleClick, fe);
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

        wrapper.DoubleTapped([isPressed, updatePlayerVisualState](auto const& sender, winrt::Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e) mutable {
            *isPressed = false;
            updatePlayerVisualState();
            if (g_unloading) return;
            ExecuteMediaAction(g_settings.playerLeftDoubleClick, sender.template try_as<FrameworkElement>());
            e.Handled(true);
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
            } else if (action == L"switch_sessions") {
                if (delta != 0) SwitchMediaSession();
            } else if (action == L"system_sound") {
                ChangeSystemVolume(delta > 0);
            } else if (action == L"app_sound") {
                std::thread([delta]() {
                    std::wstring aumid;
                    {
                        std::lock_guard<std::mutex> lk(g_sessionMtx);
                        if (g_currentSession) {
                            try {
                                aumid = std::wstring(g_currentSession.SourceAppUserModelId());
                            } catch (...) {}
                        }
                    }
                    if (!aumid.empty()) {
                        float volumeDelta = (delta > 0) ? 0.02f : -0.02f;
                        AdjustAppVolumeByAUMID(aumid, volumeDelta);
                    }
                }).detach();
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
    Wh_Log(L"InjectPlayerGrid: Starting");
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

        Wh_Log(L"InjectPlayerGrid: Got XAML root and framework element");

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
            if (g_settings.enableTreeDump) {
                DumpXamlTree(root, 0, 8);
            }
            return false;
        }

        Grid playerGrid = BuildPlayerGrid();
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
    } catch (...) {
        g_playerGrid      = nullptr;
        g_injectionParent = nullptr;
        g_playerColumn    = -1;

        g_cachedAlbumTitle.clear();
        g_cachedAlbumArtist.clear();
        g_cachedThumbnailBytes.clear();
        g_cachedPaletteHash = 0;
        g_blurBgCache.Invalidate();
        g_cachedAppIconSize = -1;

        ResetScrollState(g_titleScroll);
        ResetScrollState(g_artistScroll);
    }
}

static void RefreshPlayerContents() {
    if (!g_playerGrid || g_unloading || g_applyingSettings) return;

    Wh_Log(L"RefreshPlayerContents: Starting");
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
    Wh_Log(L"RefreshPlayerContents: title='%s', artist='%s', isPlaying=%d, hasMedia=%d",
           title.c_str(), artist.c_str(), isPlaying, hasMedia);
    (void)hasMedia;
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

                if (g_settings.showFullTitleOnHover && !title.empty() && hasSession) {
                    ToolTipService::SetToolTip(tb, winrt::box_value(winrt::hstring(title)));
                    ToolTipService::SetPlacement(tb, Controls::Primitives::PlacementMode::Top);
                } else {
                    ToolTipService::SetToolTip(tb, nullptr);
                }

                if (g_settings.enableTitleScrolling && visible) {
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
                }
            } catch (...) {}

    if (auto fe = FindChildByName(g_playerGrid, kArtistBlockName))
        if (auto ab = fe.try_as<TextBlock>())
            try {
                std::wstring displayArtist = artist;
                if (!hasSession) {
                    displayArtist = g_settings.noMediaArtistText;
                } else if (artist.empty()) {
                    displayArtist = g_settings.emptyArtistText;
                }
                ab.Text(winrt::hstring(displayArtist));
                bool visible = g_settings.showTrackArtist && !displayArtist.empty();
                artistVisible = visible;
                ab.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
                ab.Foreground(MakeBrush(ArtistColor()));

                if (g_settings.showFullTitleOnHover && !artist.empty() && hasSession) {
                    ToolTipService::SetToolTip(ab, winrt::box_value(winrt::hstring(artist)));
                    ToolTipService::SetPlacement(ab, Controls::Primitives::PlacementMode::Top);
                } else {
                    ToolTipService::SetToolTip(ab, nullptr);
                }

                if (g_settings.enableArtistScrolling && visible) {
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
                }
            } catch (...) {}

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
                    std::wstring aumid;
                    {
                        std::lock_guard<std::mutex> lk(g_mediaMtx);
                        aumid = g_media.appUserModelId;
                    }
                    if (!aumid.empty()) {
                        appIconBytes = FetchAppIconBytes(aumid, g_settings.appIconSize);
                        {
                            std::lock_guard<std::mutex> lk(g_mediaMtx);
                            g_media.appIconBytes = appIconBytes;
                        }
                    }
                    g_cachedAppIconSize = g_settings.appIconSize;
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

    if (!hide && g_settings.idleHideSeconds > 0) {
        bool playing = false;
        { std::lock_guard<std::mutex> lk(g_mediaMtx); playing = g_media.isPlaying; }
        if (playing) { g_idleSeconds = 0; g_hiddenByIdle = false; }
        else if (++g_idleSeconds >= g_settings.idleHideSeconds) g_hiddenByIdle = true;
        if (g_hiddenByIdle) hide = true;
    }

    if (!hide) {
        bool hasMedia = false, hasSession = false;
        { std::lock_guard<std::mutex> lk(g_mediaMtx); hasMedia = g_media.hasMedia; }
        { std::lock_guard<std::mutex> lk(g_sessionMtx); hasSession = (g_currentSession != nullptr); }
        Wh_Log(L"UpdateVisibility: hasMedia=%d, hasSession=%d, hideWhenNoMedia=%d", hasMedia, hasSession, g_settings.hideWhenNoMedia);

        if (hasMedia || hasSession) {
            g_lastMediaTime = std::chrono::steady_clock::now();
        }

        if (g_settings.hideWhenNoMedia) {

            if (!hasSession) {
                hide = true;
            }

            else if (!hasMedia) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastMediaTime).count();

                if (elapsed > 2500) {
                    hide = true;
                } else {
                    std::thread([]() {
                        if (!g_unloading && g_timerUpdateEvent) SetEvent(g_timerUpdateEvent);
                    }).detach();
                }
            }
        }
    }

    Wh_Log(L"UpdateVisibility: Final decision - hide=%d", hide);

    try {
        Wh_Log(L"UpdateVisibility: g_playerColumn=%d", g_playerColumn);
        if (g_playerColumn == -1) {
            if (hide) {
                g_playerGrid.Visibility(Visibility::Collapsed);
                Wh_Log(L"UpdateVisibility: Set Visibility to Collapsed (playerColumn=-1)");
            } else {
                g_playerGrid.Visibility(Visibility::Visible);
                Wh_Log(L"UpdateVisibility: Set Visibility to Visible (playerColumn=-1)");
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

                std::thread([]() {
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
                }).detach();
            } else {
                g_playerGrid.Visibility(hide ? Visibility::Collapsed : Visibility::Visible);
                g_playerGrid.Opacity(hide ? 0.0 : 1.0);
                Wh_Log(L"UpdateVisibility: Set Visibility=%d, Opacity=%f", hide ? 0 : 1, hide ? 0.0 : 1.0);

                if (g_injectionParent) {
                    if (auto targetGrid = g_injectionParent.try_as<Grid>()) {
                        if (g_playerColumn < (int)targetGrid.ColumnDefinitions().Size()) {
                            auto colDef = targetGrid.ColumnDefinitions().GetAt(g_playerColumn);
                            if (hide) {
                                colDef.Width({0.0, GridUnitType::Pixel});
                                Wh_Log(L"UpdateVisibility: Set column width to 0");
                            } else {
                                colDef.Width({1.0, GridUnitType::Auto});
                                Wh_Log(L"UpdateVisibility: Set column width to Auto");
                            }
                        }
                    }
                } else {
                    Wh_Log(L"UpdateVisibility: g_injectionParent is NULL");
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
                StopVizCaptureThread();
            }
        }
    } catch (...) {}
}

static void ApplySettings() {
    Wh_Log(L"ApplySettings: Called");
    try { RemovePlayerGrid(); } catch (...) { Wh_Log(L"ApplySettings: Exception in RemovePlayerGrid"); }
    if (!g_unloading) {
        Wh_Log(L"ApplySettings: Calling InjectPlayerGrid");
        try { InjectPlayerGrid(); } catch (...) { Wh_Log(L"ApplySettings: Exception in InjectPlayerGrid"); }
    }
    Wh_Log(L"ApplySettings: Finished, g_playerGrid exists = %d", g_playerGrid ? 1 : 0);
}


static void ApplySettingsWithRetry(FrameworkElement xamlRootContent) {
    auto retry = [&]() {
        auto timer = DispatcherTimer();
        timer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(100)});
        auto tickToken = std::make_shared<winrt::event_token>();
        *tickToken = timer.Tick(
            [timer, tickToken, xamlRootContent](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) {
                timer.Stop();
                timer.Tick(*tickToken);
                ApplySettingsWithRetry(xamlRootContent);
            });
        timer.Start();
    };

    if (g_unloading) {
        return;
    }

    auto systemTrayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        Wh_Log(L"SystemTrayFrame not found, retrying...");
        retry();
        return;
    }

    auto systemTrayFrameGrid = FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    if (!systemTrayFrameGrid) {
        Wh_Log(L"SystemTrayFrameGrid not found, retrying...");
        retry();
        return;
    }

    ApplySettings();
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L"TrayUI_StartTaskbar_Hook: Called");
    TrayUI_StartTaskbar_Original(pThis);

    if (g_unloading) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: Unloading, skipping");
        return;
    }

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: Taskbar window not found");
        return;
    }

    Wh_Log(L"TrayUI_StartTaskbar_Hook: Reinitializing (hWnd=%p, prev=%p)", hWnd, g_taskbarWnd);
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

    g_hookFailCount = 0;

    StopVizTimer();
    g_vizDispatcherTimer = nullptr;
    g_vizDispatcherTimerHasToken = false;

    StopScrollTimer();
    g_scrollDispatcherTimer = nullptr;
    g_scrollDispatcherTimerHasToken = false;

    if (g_settings.vizEnabled) {
        StopVizCaptureThread();
        StartVizCaptureThread();
        StartVizTimer();
    }
    if (g_settings.enableTitleScrolling || g_settings.enableArtistScrolling) {
        StartScrollTimer();
    }

    auto xamlRoot = GetTaskbarXamlRoot(hWnd);
    if (!xamlRoot) {
        Wh_Log(L"InjectPlayerGrid: Failed to get XAML root");
        return;
    }

    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
    if (!xamlRootContent) {
        Wh_Log(L"InjectPlayerGrid: Failed to get XAML root content");
        return;
    }

    ApplySettingsWithRetry(xamlRootContent);
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
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
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
    Wh_Log(L"Wh_ModInit: Starting");
    g_unloading = false;
    g_applyingSettings = false;
    g_hookInjectionInProgress = false;
    g_hookFailCount = 0;
    g_taskbarWnd = nullptr;
    g_needsUiUpdate = false;

    LoadSettings();

    Wh_Log(L"Wh_ModInit: Calling HookTaskbarDllSymbols");
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Wh_ModInit: HookTaskbarDllSymbols failed");
        return FALSE;
    }

    Wh_Log(L"Wh_ModInit: Completed successfully");
    return TRUE;
}

void Wh_ModAfterInit() {

    Wh_Log(L"Wh_ModAfterInit: Starting initialization");

    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    Wh_Log(L"Wh_ModAfterInit: Taskbar window = %p", g_taskbarWnd);

    StartMediaThread();
    Wh_Log(L"Wh_ModAfterInit: Media thread started");

    StartTimerThread();
    Wh_Log(L"Wh_ModAfterInit: Timer thread started");

    if (g_taskbarWnd) {
        Wh_Log(L"Wh_ModAfterInit: Found taskbar window, running ApplySettings from window thread");
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            Wh_Log(L"Wh_ModAfterInit: Inside window thread callback");
            g_cachedAlbumTitle.clear();
            g_cachedAlbumArtist.clear();
            g_cachedThumbnailBytes.clear();
            g_cachedPaletteHash = 0;
            g_cachedAppIconSize = -1;
            g_blurBgCache.Invalidate();
            ApplySettings();
            if (g_playerGrid) {
                Wh_Log(L"Wh_ModAfterInit: Player grid exists, refreshing UI");
                ShowSuccessNotification();
                RefreshPlayerContents();
                UpdateVisibility();
                g_needsUiUpdate = true;
                if (g_timerUpdateEvent) {
                    SetEvent(g_timerUpdateEvent);
                    Wh_Log(L"Wh_ModAfterInit: Signaled timer update event");
                }
            } else {
                Wh_Log(L"Wh_ModAfterInit: Player grid is NULL after ApplySettings - will init via hook");
            }
        }, nullptr);
    } else {
        Wh_Log(L"Wh_ModAfterInit: No taskbar window found - will init via hook when DLL loads");
    }
}

void Wh_ModUninit() {
    g_unloading = true;
    g_hookInjectionInProgress = false;
    g_hookFailCount = 0;

    StopTimerThread();
    StopMediaThread();

    if (g_taskbarWnd)
        RunFromWindowThread(g_taskbarWnd, [](void*) {
            RemovePlayerGrid();
            g_mediaHoverBrush   = nullptr;
            g_mediaPressedBrush = nullptr;
            g_playerHoverBrush  = nullptr;
            g_playerPressedBrush = nullptr;
            g_playerBorderBrush  = nullptr;
            g_playerBorderPressedBrush = nullptr;
        }, nullptr);
    else {
        g_mediaHoverBrush   = nullptr;
        g_mediaPressedBrush = nullptr;
        g_playerHoverBrush  = nullptr;
        g_playerPressedBrush = nullptr;
        g_playerBorderBrush  = nullptr;
        g_playerBorderPressedBrush = nullptr;
    }

    CleanupAudioDeviceEnumerator();
}

void Wh_ModSettingsChanged() {
    g_applyingSettings = true;
    g_vizBaseColorDirty = true;
    g_vizCurrentlyVisible = false;

    StopTimerThread();

    LoadSettings();

    HWND hWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        g_applyingSettings = false;
        StartTimerThread();
        return;
    }
    g_taskbarWnd = hWnd;

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId && threadId != GetCurrentThreadId()) {
        struct WorkData { HWND hWnd; };
        WorkData* data = new WorkData{hWnd};

        HANDLE hThread = CreateThread(nullptr, 0, [](LPVOID param) WINAPI -> DWORD {
            WorkData* data2 = (WorkData*)param;
            RunFromWindowThread(data2->hWnd, [](void*) {
                try {
                    RemovePlayerGrid();
                    g_cachedAlbumTitle.clear();
                    g_cachedAlbumArtist.clear();
                    g_cachedThumbnailBytes.clear();
                    g_cachedPaletteHash = 0;
                    g_blurBgCache.Invalidate();
                    g_applyingSettings = false;
                    if (!g_unloading) {
                        InjectPlayerGrid();
                        g_needsUiUpdate = true;
                    }
                } catch (...) {
                    Wh_Log(L"Wh_ModSettingsChanged: Exception during RemovePlayerGrid/InjectPlayerGrid");
                    g_applyingSettings = false;
                    g_playerGrid = nullptr;
                    g_injectionParent = nullptr;
                }
            }, nullptr);
            return 0;
        }, data, 0, nullptr);

        if (hThread) {
            WaitForSingleObject(hThread, 2000);
            CloseHandle(hThread);
        }
        delete data;
    } else {
        try {
            RemovePlayerGrid();
            g_cachedAlbumTitle.clear();
            g_cachedAlbumArtist.clear();
            g_cachedThumbnailBytes.clear();
            g_cachedPaletteHash = 0;
            g_blurBgCache.Invalidate();
            g_applyingSettings = false;
            if (!g_unloading) {
                InjectPlayerGrid();
                g_needsUiUpdate = true;
            }
        } catch (...) {
            Wh_Log(L"Wh_ModSettingsChanged: Exception during RemovePlayerGrid/InjectPlayerGrid (same thread)");
            g_applyingSettings = false;
            g_playerGrid = nullptr;
            g_injectionParent = nullptr;
        }
    }

    StartTimerThread();
}
