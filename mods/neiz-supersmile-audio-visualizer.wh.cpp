// ==WindhawkMod==
// @id              neiz-supersmile-audio-visualizer
// @name            Desktop Audio Visualizer Plus
// @description     A highly customizable audio visualizer with synced lyrics, media controls and EQ, featuring optional network access to fetch lyrics from lrclib.net
// @description:ru  Настраиваемый аудиовизуализатор с синхронизированным текстом песен, управлением медиа и эквалайзером, с опциональным доступом к сети для загрузки текстов с lrclib.net
// @version         1.1.0
// @license         MIT
// @author          NeiZ
// @github          https://github.com/NeiZqwe
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lmmdevapi -lksuser -lgdi32 -luser32 -lgdiplus -ldwmapi -lruntimeobject -lwindowsapp -lwinhttp -lshell32 -ld3d11 -ldxgi -ldcomp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Audio Visualizer Plus

Desktop Audio Visualizer Plus is a highly customizable Windhawk desktop overlay that combines a real-time audio visualizer, synchronized lyrics, album artwork, media controls and a full 10-band EQ

## Features

### Audio and performance

- WASAPI audio capture for the **whole system** or **selected applications**
- 1024-point FFT with 32 logarithmic frequency bands
- Up to 256 rendered visual bars
- 10-band custom EQ curve applied across the FFT bands
- Auto-Gain normalization
- Frame-rate-independent attack / decay dynamics
- Optional CAVA-style smoothing
- Configurable render FPS; `0` follows the display refresh rate without a software FPS cap
- Adaptive idle rendering: when the visualizer reaches its resting state, rendering is automatically reduced to save resources
- Fullscreen/borderless-fullscreen throttle: while another application occupies an entire monitor, the overlay on that monitor is temporarily reduced to **1 FPS** without changing the user's saved FPS setting; other monitors keep their normal render rate
- Monitor selection: `0` renders on **all monitors**, while `1`, `2`, ... target one specific monitor
- Configurable placement relative to desktop icons: **Behind icons, transparent** (default), **Behind icons, opaque layer**, or **Above icons** ( **Advanced → Desktop placement** )

![1](https://i.imgur.com/mk5fide.png)
![2](https://i.imgur.com/NtlXLCd.png)

---

### Visualization

- Stereo, Mountain, Mirror, Wave, Circular, Dots and Area visualization types
- Linear, Step, Cosine and Catmull-Rom interpolation
- Vertical and horizontal orientations
- Mirrored visualization pass
- Configurable bar count, width, spacing, height and corner radius
- Dynamic bar width based on cursor distance
- Optional foreground PNG image plane

![3](https://i.imgur.com/8NllN1h.gif)

![4](https://i.imgur.com/VNz6GOi.gif)

---

### Appearance and colors

- Square, Rounded, Segmented, Pointed, Continuous Curve and Battery bar styles
- Album-derived solid and gradient colors
- Configurable opacity and dynamic opacity curves
- Dynamic color curves based on bar height
- Customizable bar borders
- Optional customizable visualizer background

![5](https://i.imgur.com/aqXU5Fa.gif)

---

### Lyrics

Lyrics are optional. The widget can display artist/title metadata, synchronized lyrics, previous/upcoming lines and fallback text, with extensive layout and appearance controls.

- Left, center or right text alignment
- Configurable focus-line position
- Configurable number of lyric lines above and below the current line
- Optional wrapping of long highlighted lyric lines
- Fallback, collapse or hide behaviour when lyrics are unavailable
- Album-derived and gradient backgrounds
- Configurable background opacity, rounding and borders
- Click-to-seek lyric rows inside the Media & EQ popup
- Long artist/title metadata is automatically clipped and smoothly marquee-scrolls when it is wider than the available widget area

![6](https://i.imgur.com/TU0iXkc.gif)

### Lyrics data source and local LRC files

Lyrics can be loaded from local `.lrc` files instead of the online LRCLIB lookup.

When **Use local LRC files** is enabled:

- The configured folder is searched for local `.lrc` files
- Network/LRCLIB lyric lookups are disabled
- Local playback can work without sending lyric searches over the network
- Custom or manually edited timing files can be used

Recommended local file names:

1. `Artist - Title.lrc`
2. `Title.lrc`

![7](https://i.imgur.com/ZEOrsmg.png)

![8](https://i.imgur.com/bGYvyU7.png)

When local LRC mode is disabled, the normal online LRCLIB lookup is used. Network access is limited to lyric retrieval and is not used for telemetry, analytics or remote configuration.

---

### Album Widget

- Optional album-art desktop widget
- Mouse-drag positioning
- Separate placement or attachment to the Lyrics widget / visualizer
- Configurable width, height, corner radius and opacity
- Configurable attachment gap and maximum attachment distance
- Configurable album-image resampling quality
- Cached artwork rendering so repeated frames do not continuously rescale the source image

---

### Media & EQ

The Media & EQ button is integrated into the tray on both Windows 11 and Windows 10.

- **Windows 11:** native XAML tray integration
- **Windows 10:** native notification-area (`Shell_NotifyIcon`) integration
- Tray button visibility can be toggled from the settings on both Windows versions
- Clicking the tray button opens the redesigned Media & EQ popup
- Current track, artist and source information
- Timeline seeking
- Optional Click-to-Seek Lyrics section with scrollable LRC rows
- Play/pause, previous/next and ±5 second seeking controls
- Graphical 10-band EQ with `0.0x`–`2.0x` gain range and `1.0x` as neutral
- Built-in presets: Flat, Bass Boost, Bass Cut, Treble Boost, Vocal, Rock, Pop, Classical and Loudness
- Up to 12 custom EQ presets stored in Windhawk settings
- Configurable Media & EQ Layout Builder
- Configurable popup placement and layout

![9](https://i.imgur.com/GA68U6D.png)

![10](https://i.imgur.com/8TvmQ8V.png)

![11](https://i.imgur.com/gRLC7uy.png)

![a](https://i.imgur.com/csqf7kJ.png)

![12](https://i.imgur.com/TE3lOeB.png)

![13](https://i.imgur.com/ZxlcDIV.png)

![14](https://i.imgur.com/kphLg3D.png)

![15](https://i.imgur.com/Kewsx9C.png)


---
## Performance

The visualizer uses a WASAPI/FFT pipeline with adaptive rendering. While audio is active, the render timer follows the configured FPS or display refresh rate. When the bars settle, the timer is automatically reduced. Audio updates can wake the overlay again so active playback returns to the normal render cadence.

When a fullscreen or borderless-fullscreen application occupies a monitor, only the overlay rendered on that monitor temporarily uses a 1 FPS safety throttle. Other selected monitors continue to use the configured FPS. The configured FPS value is **not** overwritten, and normal rendering is restored when fullscreen mode ends.

For multi-monitor setups, `0` renders separate overlay windows across all available monitors. A positive value selects one monitor. If a selected monitor is unavailable, the overlay falls back to the primary monitor.

The Media & EQ SMTC worker runs only while the Media & EQ tray button is enabled, avoiding repeated cross-process media polling when that feature is disabled.

Actual CPU/GPU usage depends on display resolution, effects, visualization style, lyrics, frame rate, number of monitors and other enabled features.

## Windows compatibility

The Media & EQ tray integration uses different shell mechanisms depending on the Windows version:

- **Windows 11:** XAML tray integration inside the Windows taskbar/tray UI
- **Windows 10:** native notification-area icon integration with retry/recovery handling when Explorer recreates the tray

This keeps the user-facing Media & EQ feature consistent while allowing each Windows version to use the shell integration path available to it.

The mod has been tested across multiple builds of Windows 10 and Windows 11.


---

## Credits

- [**NeiZ**](https://github.com/NeiZqwe) — Author and maintainer. 
- [**SuperSmile123**](https://github.com/SuperSmile123) — Contributor.  
- [**SuperAnt220**](https://github.com/SuperAnt220) — Contributor.  
- [**Salyts**](https://github.com/Salyts) — Reference / upstream desktop audio visualizer implementation.  
- [**GR0UD**](https://github.com/GR0UD) — Upstream audio capture / FFT implementation reference.  
  

---

## Report a Bug

Please report bugs and feature requests through the repository issue tracker: [Report an Issue on GitHub](https://github.com/NeiZqwe/windhawk-mods/issues)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Visualizer:
    - barCount: 32
      $name: Bar count
      $name:ru-RU: Количество полос
      $description: Number of bars displayed by the visualizer (0-256)
      $description:ru-RU: Количество полос, отображаемых визуализатором (0-256)

    - barWidth: 7
      $name: Bar width
      $name:ru-RU: Ширина полос
      $description: Controls the width of each bar
      $description:ru-RU: Определяет ширину каждой полосы

    - barSpacing: 3
      $name: Bar spacing
      $name:ru-RU: Отступ между полосами
      $description: Controls the spacing between bars
      $description:ru-RU: Определяет расстояние между полосами

    - orientation: bottom_up
      $name: Orientation
      $name:ru-RU: Направление
      $description: Sets the direction in which the bars grow
      $description:ru-RU: Определяет направление роста полос
      $options:
        - "bottom_up": Bottom up
        - "center_vertical": Center vertical
        - "top_down": Top down
        - "left_right": Left to right
        - "center_horizontal": Center horizontal
        - "right_left": Right to left
      $options:ru-RU:
        - "bottom_up": Снизу вверх
        - "center_vertical": Из центра вверх и вниз
        - "top_down": Сверху вниз
        - "left_right": Слева направо
        - "center_horizontal": Из центра в стороны
        - "right_left": Справа налево

    - interpolationMode: smooth
      $name: Interpolation
      $name:ru-RU: Интерполяция
      $description: Smooths the transition between neighboring bars
      $description:ru-RU: Сглаживает переходы между соседними полосами
      $options:
        - smooth: Linear
        - step: Step
        - cosine: Cosine
        - catmull_rom: Catmull-Rom
      $options:ru-RU:
        - smooth: Линейная
        - step: Ступенчатая
        - cosine: Косинусная
        - catmull_rom: Catmull-Rom

    - barShape: stereo
      $name: Visualization type
      $name:ru-RU: Тип визуализации
      $description: Selects how the audio spectrum is arranged and displayed
      $description:ru-RU: Определяет, как аудиоспектр располагается и отображается
      $options:
        - stereo: Stereo
        - mountain: Mountain
        - mirror: Mirror
        - wave: Wave
        - circular: Circular
        - dots: Dots
        - area: Area
      $options:ru-RU:
        - stereo: Стерео
        - mountain: Гора
        - mirror: Зеркало
        - wave: Волна
        - circular: Круговая
        - dots: Точки
        - area: Область


    - mirroredVisualizer: false
      $name: Mirrored visualizer
      $name:ru-RU: Зеркальный визуализатор
      $description: Shows a mirrored copy of the visualizer on the opposite side of the screen
      $description:ru-RU: Показывает зеркальную копию визуализатора на противоположной стороне экрана

    - Circular:
        - circleRadius: 250
          $name: Radius (px)
          $name:ru-RU: Радиус (px)

        - circleStartAngle: -90
          $name: Start angle (°)
          $name:ru-RU: Начальный угол (°)
          $description: Sets the starting angle of the circular visualization. -90° = top, 0° = right, 90° = bottom
          $description:ru-RU: Задаёт начальный угол кругового визуализатора. -90° = сверху, 0° = справа, 90° = снизу
      $name: Circular type settings
      $name:ru-RU: Настройки кругового визуализатора
    - positionX: 800
      $name: X position (px)
      $description: Adjusts the horizontal position of the visualizer
      $name:ru-RU: X позиция
      $description:ru-RU: Изменяет горизонтальное положение визуализатора
    - positionY: 1000
      $name: Y position (px)
      $description: Adjusts the vertical position of the visualizer
      $name:ru-RU: Y позиция
      $description:ru-RU: Изменяет вертикальное положение визуализатора
    - maxBarHeight: 150
      $name: Max bar height
      $description: Sets the maximum height the bars can reach
      $name:ru-RU: Максимальная высота полосы
      $description:ru-RU: Задаёт максимальную высоту, которой могут достигать полосы
    - minBarHeight: 6
      $name: Min bar height
      $description: Sets the resting height of the bars when there is little or no audio
      $name:ru-RU: Минимальная высота полосы
      $description:ru-RU: Задаёт высоту полос в состоянии покоя при отсутствии или низком уровне звука

  $name: Visualizer
  $name:ru-RU: Визуализатор
  $description: Customize the layout, position, and geometry of the audio visualizer
  $description:ru-RU: Настройка расположения, положения и геометрии аудиовизуализатора

- Performance:
    - targetMonitor: 0
      $name: Target monitor
      $name:ru-RU: Монитор визуализации
      $description: >-
        Selects which monitor receives the visualizer and all desktop widgets.
        0 = all monitors (higher CPU/GPU usage because the same visualizer is
        rendered once per monitor; use only if needed). 1, 2, ... = a specific
        monitor number.
      $description:ru-RU: >-
        Выбирает монитор, на котором выводятся визуализатор и все рабочие
        виджеты. 0 = все мониторы (повышенная нагрузка на CPU/GPU,
        поскольку визуализатор рендерится отдельно на каждом мониторе;
        используйте только при необходимости). 1, 2, ... = конкретный монитор.

    - targetFps: 60
      $name: Visualizer frame rate
      $name:ru-RU: Частота кадров визуализатора
      $description: >-
        Limits the visualizer refresh rate. 0 = no software FPS cap; the
        effective limit is the current display refresh rate. Higher refresh
        rates can make the animation smoother but may increase CPU/GPU usage
        and power consumption.
      $description:ru-RU: >-
        Ограничивает частоту обновления визуализатора. 0 = без программного
        ограничения FPS; фактический предел определяется текущей частотой
        обновления дисплея. Высокая герцовка может сделать анимацию плавнее,
        но способна увеличить нагрузку на CPU/GPU и энергопотребление.

- Audio:
    - Source:
        - audioSource: system
          $name: Audio source
          $name:ru-RU: Источник аудио
          $description: Select whether the visualizer listens to the whole system or selected applications
          $description:ru-RU: Выберите, слушать ли всю систему или выбранные приложения
          $options:
            - system: Whole system
            - application: Selected applications
          $options:ru-RU:
            - system: Вся система
            - application: Выбранные приложения

        - audioApplicationName: ""
          $name: Selected application executable(s)
          $name:ru-RU: Исполняемый файл(ы) выбранных приложений
          $description: Used when Audio source is set to Selected applications. One or more executable names separated by comma, semicolon, slash, pipe, or whitespace, for example Spotify.exe, Discord.exe
          $description:ru-RU: Используется в режиме «Выбранные приложения». Одно или несколько имён .exe, разделённых запятой, точкой с запятой, слэшем, вертикальной чертой или пробелом, например Spotify.exe, Discord.exe
      $name: Audio source
      $name:ru-RU: Источник аудио

    - sensitivity: 65
      $name: Sensitivity
      $name:ru-RU: Чувствительность
      $description: Controls how strongly the visualizer reacts to audio (0-300)
      $description:ru-RU: Определяет, насколько сильно визуализатор реагирует на звук (0-300)

    - AutoGain:
        - autoGainEnabled: false
          $name: Auto-Gain
          $name:ru-RU: Автонормализация
          $description: Automatically adjusts the audio level for a more consistent response
          $description:ru-RU: Автоматически регулирует уровень сигнала для более стабильной реакции

        - autoGainStrength: 90
          $name: Auto-Gain strength
          $name:ru-RU: Сила автонормализации
          $description: Controls how strongly automatic gain adjusts the audio level (0-100)
          $description:ru-RU: Определяет, насколько сильно автогейн регулирует уровень сигнала (0-100)

      $name: Auto-Gain
      $name:ru-RU: Автонормализация
      $description: Controls automatic audio level normalization
      $description:ru-RU: Настройки автоматической нормализации уровня сигнала

    - Dynamics:
        - attackSpeed: 40
          $name: Attack speed
          $name:ru-RU: Скорость атаки
          $description: Controls the attack response. The value is frame-rate independent and preserves the same time response across different refresh rates (0-100)
          $description:ru-RU: Определяет, насколько быстро полосы поднимаются в ответ на усиление звука (0-100)

        - decaySpeed: 16
          $name: Decay speed
          $name:ru-RU: Скорость затухания
          $description: Controls the decay response. The value is frame-rate independent and preserves the same time response across different refresh rates (0-100)
          $description:ru-RU: Определяет, насколько быстро полосы возвращаются к высоте покоя (0-100)

        - cavaSmoothingEnabled: false
          $name: Smoothing
          $name:ru-RU: Сглаживание
          $description: Smooths the rise and fall of the bars
          $description:ru-RU: Сглаживает подъём и спад полос

        - cavaNoiseReduction: 5
          $name: Smoothing strength
          $name:ru-RU: Сила сглаживания
          $description: Controls the amount of smoothing applied to bar movement (0-100)
          $description:ru-RU: Определяет силу сглаживания движения полос (0-100)

      $name: Dynamics
      $name:ru-RU: Динамика
      $description: Controls the attack, decay, and smoothing of the visualizer
      $description:ru-RU: Настройки атаки, затухания и сглаживания визуализатора

  $name: Audio and animation
  $name:ru-RU: Аудио и анимация
  $description: Controls how the visualizer responds to the audio signal and how smoothly it reacts to changes in sound
  $description:ru-RU: Настройки, определяющие реакцию визуализатора на аудиосигнал и плавность его изменения

- Appearance:
    - Style:
        - barStyle: rounded
          $name: Bar style
          $description: Selects the visual style of the bars
          $name:ru-RU: Стиль полос
          $description:ru-RU: Выбирает визуальный стиль полос
          $options:
            - square: Square
            - rounded: Rounded
            - segmented_square: Segmented squares
            - pointed: Pointed
            - curve: Continuous curve
            - battery: Battery
          $options:ru-RU:
            - square: Квадратные
            - rounded: Скруглённые
            - segmented_square: Сегментированные
            - pointed: Заострённые
            - curve: Сплошная кривая
            - battery: Батарейка

        - cornerRadius: 6
          $name: Corner radius
          $name:ru-RU: Скругление
          $description: Controls the corner rounding of bars and segments
          $description:ru-RU: Определяет степень скругления углов полос и сегментов

        - pointedSharpness: 50
          $name: (Pointed) Sharpness
          $name:ru-RU: Острота конца
          $description: Controls how pointed the end of sharp-ended bars is (0-100)
          $description:ru-RU: Определяет, насколько острым будет конец полос (0-100)

        - curveWidth: 800
          $name: Curve width
          $name:ru-RU: Длина кривой
          $description: Controls the length of the continuous curve (px)
          $description:ru-RU: Определяет длину сплошной кривой (px)

        - segmentSpacing: 3
          $name: Segment spacing
          $name:ru-RU: Промежутки между сегментами
          $description: Controls the gap between individual square segments (px)
          $description:ru-RU: Определяет расстояние между отдельными квадратными сегментами (px)

        - segmentHeight: 5
          $name: Segment height
          $name:ru-RU: Высота сегментов
          $description: Controls the height of each square segment (px). 0 = bar width
          $description:ru-RU: Определяет высоту каждого квадратного сегмента (px). 0 = ширина полос

        - borderEnabled: false
          $name: Bar border
          $name:ru-RU: Рамка полос
          $description: Enables an outline around the visualizer bars in any bar style
          $description:ru-RU: Включает рамку вокруг полос визуализатора для любого стиля полос

        - borderThickness: 1
          $name: Border thickness
          $name:ru-RU: Толщина рамки
          $description: Controls the border width in pixels (1-10)
          $description:ru-RU: Определяет толщину рамки в пикселях (1-10)

      $name: Style
      $name:ru-RU: Стиль

    - Colors:
        - colorMode: solid
          $name: Color mode
          $name:ru-RU: Цветовой режим
          $description: Selects how the visualizer colors are generated
          $description:ru-RU: Выбирает способ формирования цветов визуализатора
          $options:
            - solid: Solid
            - gradient: Gradient (horizontal)
            - gradient_vertical: Gradient (vertical)
            - solid_album: Solid (album)
            - gradient_album: Gradient (album)
            - dynamic_acrylic: Acrylic (dynamic)
            - liquid_glass: Glass
            - aero_glass: Glass (peak caps)
          $options:ru-RU:
            - solid: Сплошной
            - gradient: Градиент (по горизонтали)
            - gradient_vertical: Градиент (по вертикали)
            - solid_album: Сплошной (обложка)
            - gradient_album: Градиент (обложка)
            - dynamic_acrylic: Acrylic (динамический)
            - liquid_glass: Стекло
            - aero_glass: Стекло (пики)

        - colorHex: "#FFFFFF"
          $name: Color 1 / Bottom (Hex)
          $name:ru-RU: Цвет 1 / Нижний (Hex)
          $description: Sets the lower or starting color of the visualizer
          $description:ru-RU: Задаёт нижний или начальный цвет визуализатора

        - gradientColorHex: "#00B4FF"
          $name: Color 2 / Upper (Hex)
          $name:ru-RU: Цвет 2 / Верхний (Hex)
          $description: Upper color used for gradients and high peaks. Leave empty to disable color change
          $description:ru-RU: Верхний цвет для градиента и высоких пиков. Оставьте пустым, чтобы цвет не менялся

        - borderMode: solid
          $name: Border color mode
          $name:ru-RU: Цветовой режим рамки
          $description: Selects how the bar border color is generated
          $description:ru-RU: Выбирает способ формирования цвета рамки полос
          $options:
            - solid: Visualizer color
            - gradient: Visualizer gradient
            - solid_album: Solid album
            - gradient_album: Gradient album
            - hex: Hex
            - hex_gradient: Hex (gradient)
          $options:ru-RU:
            - solid: Цвет визуализатора
            - gradient: Градиент визуализатора
            - solid_album: Сплошной от обложки
            - gradient_album: Градиент от обложки
            - hex: Hex
            - hex_gradient: Hex (градиент)

        - borderColorHex: "#FFFFFF"
          $name: Border color (Hex)
          $name:ru-RU: Цвет рамки (Hex)
          $description: Main border color used by the Hex modes
          $description:ru-RU: Основной цвет рамки для режимов Hex

        - borderGradientColorHex: "#00B4FF"
          $name: Border gradient color (Hex)
          $name:ru-RU: Цвет градиента рамки (Hex)
          $description: Second border color used by Gradient and Hex (gradient) modes
          $description:ru-RU: Второй цвет рамки для режимов «Градиент» и «Hex (градиент)»

        - gradientCurveEnabled: false
          $name: Nonlinear color change
          $name:ru-RU: Нелинейное изменение цвета
          $description: Enables a nonlinear color response based on bar height
          $description:ru-RU: Включает нелинейную зависимость цвета от высоты полосы

        - gradientCurve: 0
          $name: Dynamic color curve
          $name:ru-RU: Изгиб динамического цвета
          $description: Adjusts the color response curve. Negative values shift more of the color change toward the lower part of the bars, while positive values shift it toward the upper part (-100..100, 0 = linear)
          $description:ru-RU: Изменяет кривую изменения цвета. Отрицательные значения смещают изменение цвета ближе к нижней части полос, положительные — к верхней (-100..100, 0 = линейно)

        - glassHighlight: 65
          $name: Glass highlight
          $name:ru-RU: Блик стекла
          $description: Controls the intensity of the glass highlight (0-100)
          $description:ru-RU: Определяет интенсивность блика стекла (0-100)
      $name: Colors
      $name:ru-RU: Цвета

    - Opacity:
        - acrylicOpacity: 70
          $name: Opacity
          $name:ru-RU: Прозрачность
          $description: Controls the overall opacity (0-100)
          $description:ru-RU: Определяет общую прозрачность (0-100)

        - dynamicAcrylicMinOpacity: 20
          $name: Minimum opacity
          $name:ru-RU: Мин. прозрачность
          $description: Sets the minimum opacity of bars in Dynamic Acrylic mode (0-100)
          $description:ru-RU: Определяет минимальную прозрачность полос в режиме «Динамический Acrylic» (0-100)

        - opacityCurveEnabled: false
          $name: Nonlinear opacity change
          $name:ru-RU: Нелинейное изменение прозрачности
          $description: Enables a nonlinear opacity response based on bar height
          $description:ru-RU: Включает нелинейную зависимость прозрачности от высоты полосы

        - opacityCurve: 0
          $name: Dynamic opacity curve
          $name:ru-RU: Изгиб динамической прозрачности
          $description: Adjusts the opacity response curve. Negative values shift more of the opacity change toward the lower part of the bars, while positive values shift it toward the upper part (-100..100, 0 = linear)
          $description:ru-RU: Изменяет кривую изменения прозрачности. Отрицательные значения смещают изменение прозрачности ближе к нижней части полос, положительные — к верхней (-100..100, 0 = линейно)
      $name: Opacity
      $name:ru-RU: Прозрачность

    - Background:
        - backgroundEnabled: true
          $name: Visualizer background
          $name:ru-RU: Фон визуализатора
          $description: Enables or disables the visualizer background
          $description:ru-RU: Включает или выключает фон визуализатора

        - backgroundMode: blur
          $name: Background mode
          $name:ru-RU: Режим фона
          $description: Selects the visualizer background mode
          $description:ru-RU: Выбирает режим фона визуализатора
          $options:
            - solid: Solid
            - gradient: Gradient
            - album: Album color
            - album_gradient: Album gradient
            - blur: Blur
          $options:ru-RU:
            - solid: Сплошной
            - gradient: Градиент
            - album: Цвет обложки
            - album_gradient: Градиент от обложки
            - blur: Размытие обоев


        - backgroundColorHex: "#7C68E8"
          $name: Background color (Hex)
          $name:ru-RU: Цвет фона (Hex)
          $description: Sets the main background color
          $description:ru-RU: Задаёт основной цвет фона

        - backgroundGradientColorHex: "#A98CFF"
          $name: Color 2 / Gradient (Hex)
          $name:ru-RU: Цвет 2 / градиент (Hex)
          $description: Sets the second color used for the Gradient mode
          $description:ru-RU: Задаёт второй цвет для режима «Градиент»

        - backgroundOpacity: 65
          $name: Background opacity
          $name:ru-RU: Прозрачность фона
          $description: Controls the opacity of the visualizer background (0-100)
          $description:ru-RU: Определяет прозрачность фона визуализатора (0-100)

        - backgroundCornerRadius: 12
          $name: Background corner radius
          $name:ru-RU: Скругление фона
          $description: Controls the corner radius of the background (px)
          $description:ru-RU: Определяет радиус скругления углов фона (px)

        - backgroundPadding: 8
          $name: Background padding
          $name:ru-RU: Отступ фона
          $description: Adds padding around the automatically calculated background (px)
          $description:ru-RU: Добавляет отступ вокруг автоматически рассчитанного фона (px)

        - backgroundHeightAdjustment: 0
          $name: Background height
          $name:ru-RU: Высота фона
          $description: Adjusts the background height relative to the maximum bar height (px). 0 = no change
          $description:ru-RU: Дополнительно изменяет высоту фона относительно максимальной высоты полос (px). 0 = без изменений

        - backgroundBlurRadius: 12
          $name: Blur strength
          $name:ru-RU: Сила размытия
          $description: Controls wallpaper blur radius when Background mode is Blur (1-24 px)
          $description:ru-RU: Задаёт радиус размытия обоев в режиме «Размытие» (1-24 px)

        - backgroundBorderEnabled: false
          $name: Background border
          $name:ru-RU: Рамка фона
          $description: Enables a border around the visualizer background
          $description:ru-RU: Включает рамку вокруг фона визуализатора

        - backgroundBorderMode: solid_album
          $name: Border preset
          $name:ru-RU: Пресет рамки
          $description: Selects how the background border color is generated
          $description:ru-RU: Выбирает способ формирования цвета рамки фона
          $options:
            - solid_album: Solid album
            - gradient_album: Gradient album
            - hex: Hex
            - hex_gradient: Hex gradient
          $options:ru-RU:
            - solid_album: Сплошной от обложки
            - gradient_album: Градиент от обложки
            - hex: Hex
            - hex_gradient: Градиент Hex

        - backgroundBorderThickness: 1
          $name: Border thickness
          $name:ru-RU: Толщина рамки
          $description: Width of the background border in pixels
          $description:ru-RU: Толщина рамки вокруг фона в пикселях

        - backgroundBorderColorHex: "#808080"
          $name: Border color (Hex)
          $name:ru-RU: Цвет рамки (Hex)
          $description: Main border color used by Hex presets
          $description:ru-RU: Основной цвет рамки для режимов Hex

        - backgroundBorderGradientColorHex: "#C0C0C0"
          $name: Border gradient color (Hex)
          $name:ru-RU: Цвет градиента рамки (Hex)
          $description: Second border color used by the Hex gradient preset
          $description:ru-RU: Второй цвет рамки для режима «Градиент Hex»

        - backgroundBorderOpacity: 100
          $name: Border opacity
          $name:ru-RU: Прозрачность рамки
          $description: Opacity of the background border (0-100)
          $description:ru-RU: Прозрачность рамки вокруг фона (0-100)

      $name: Background settings
      $name:ru-RU: Настройки фона

  $name: Appearance
  $name:ru-RU: Внешний вид


- Lyrics:
    - enabled: false
      $name: Enable lyrics widget
      $name:ru-RU: Включить виджет текста

    - limitBars: false
      $name: Limit visualizer bars with lyrics widget
      $name:ru-RU: Ограничивать полосы виджетом текста
      $description: Keeps a gap between the visualizer bars and the lyrics widget in all non-circular directions
      $description:ru-RU: Оставляет отступ между полосами визуализатора и виджетом текста во всех некруговых направлениях

    - unavailableBehavior: fallback
      $name: When lyrics are unavailable
      $name:ru-RU: Если текст песни недоступен
      $options:
        - fallback: Show fallback
        - collapse: Collapse to title
        - hide: Hide widget
      $options:ru-RU:
        - fallback: Показывать текст-заглушку
        - collapse: Свернуть до названия
        - hide: Скрывать виджет

    - unavailableText: ""
      $name: Fallback
      $name:ru-RU: Текст-заглушка
      $description: "Text shown when the current track has no lyrics. Leave empty to show a random emoticon."
      $description:ru-RU: "Текст, отображаемый если для текущего трека нет текста песни. Оставьте пустым, чтобы показывать случайный эмотикон."

    - showArtist: true
      $name: Show artist
      $name:ru-RU: Показывать автора
      $description: Shows the artist name in the lyrics widget
      $description:ru-RU: Показывает исполнителя в виджете текста

    - showTitle: true
      $name: Show title
      $name:ru-RU: Показывать название
      $description: Shows the song title in the lyrics widget
      $description:ru-RU: Показывает название песни в виджете текста

    - showLyrics: true
      $name: Show lyrics
      $name:ru-RU: Показывать текст песни
      $description: Shows the lyrics/fallback text in the widget
      $description:ru-RU: Показывает текст песни или текст-заглушку в виджете

    - textAlignment: center
      $name: Text alignment
      $name:ru-RU: Выравнивание текста
      $description: Controls horizontal text alignment in the widget
      $description:ru-RU: Определяет горизонтальное выравнивание текста в виджете
      $options:
        - left: Left
        - center: Center
        - right: Right
      $options:ru-RU:
        - left: Слева
        - center: По центру
        - right: Справа

    - focusY: 58
      $name: Focus line Y position
      $name:ru-RU: Положение фокусной строки по Y
      $description: Vertical position of the highlighted current lyric line inside the widget (0-100%)
      $description:ru-RU: Вертикальное положение выделенной текущей строки внутри виджета (0-100%)

    - positionX: 50
      $name: X position
      $name:ru-RU: X позиция
      $description: Horizontal position of the lyrics widget (px)
      $description:ru-RU: Горизонтальное положение виджета текста (px)

    - positionY: 620
      $name: Y position
      $name:ru-RU: Y позиция
      $description: Vertical position of the lyrics widget (px)
      $description:ru-RU: Вертикальное положение виджета текста (px)

    - width: 520
      $name: Width
      $name:ru-RU: Ширина
      $description: Width of the lyrics widget (px)
      $description:ru-RU: Ширина виджета текста (px)

    - height: 240
      $name: Height
      $name:ru-RU: Высота
      $description: Height of the lyrics widget (px)
      $description:ru-RU: Высота виджета текста (px)

    - fontSize: 22
      $name: Font size
      $name:ru-RU: Размер текста
      $description: Main lyrics font size (px)
      $description:ru-RU: Размер основного текста песни (px)

    - artistFont: segoe_ui
      $name: Artist font
      $name:ru-RU: Шрифт исполнителя
      $description: Selects the font used for the artist name
      $description:ru-RU: Выбирает шрифт для имени исполнителя
      $options:
        - segoe_ui: Segoe UI
        - arial: Arial
        - calibri: Calibri
        - tahoma: Tahoma
        - verdana: Verdana
        - trebuchet_ms: Trebuchet MS
        - georgia: Georgia
        - consolas: Consolas
        - times_new_roman: Times New Roman
        - meiryo: Meiryo
      $options:ru-RU:
        - segoe_ui: Segoe UI
        - arial: Arial
        - calibri: Calibri
        - tahoma: Tahoma
        - verdana: Verdana
        - trebuchet_ms: Trebuchet MS
        - georgia: Georgia
        - consolas: Consolas
        - times_new_roman: Times New Roman
        - meiryo: Meiryo


    - lyricsFont: segoe_ui
      $name: Lyrics font
      $name:ru-RU: Шрифт текста песни
      $description: Selects the font used for the lyrics and fallback text
      $description:ru-RU: Выбирает шрифт для текста песни и текста-заглушки
      $options:
        - segoe_ui: Segoe UI
        - arial: Arial
        - calibri: Calibri
        - tahoma: Tahoma
        - verdana: Verdana
        - trebuchet_ms: Trebuchet MS
        - georgia: Georgia
        - consolas: Consolas
        - times_new_roman: Times New Roman
        - meiryo: Meiryo
      $options:ru-RU:
        - segoe_ui: Segoe UI
        - arial: Arial
        - calibri: Calibri
        - tahoma: Tahoma
        - verdana: Verdana
        - trebuchet_ms: Trebuchet MS
        - georgia: Georgia
        - consolas: Consolas
        - times_new_roman: Times New Roman
        - meiryo: Meiryo

    - linesAbove: 1
      $name: Lines above
      $name:ru-RU: Строк выше
      $description: Number of previous lyrics lines shown above the current line
      $description:ru-RU: Количество предыдущих строк, отображаемых над текущей

    - linesBelow: 2
      $name: Lines below
      $name:ru-RU: Строк ниже
      $description: Number of upcoming lyrics lines shown below the current line
      $description:ru-RU: Количество следующих строк, отображаемых под текущей

    - longLineWrapEnabled: true
      $name: Wrap long current lines
      $name:ru-RU: Перенос длинной текущей строки
      $description: Wraps the highlighted lyric line onto multiple vertical lines when it does not fit in the available width
      $description:ru-RU: Переносит выделенную текущую строку на несколько строк по вертикали, если она не помещается по ширине

    - useLocalLrcFiles: true
      $name: Use local LRC files
      $name:ru-RU: Использовать локальные LRC-файлы
      $description: >-
        When enabled, lyrics are loaded strictly from local .lrc files in the folder below. Network and LRCLIB lookups are completely disabled.
        Why use this: offline playback, rare/custom tracks missing from the online database, or manual timing adjustments.
        File placement and naming priority:
        1. "Artist - Title.lrc" (recommended)
        2. "Title.lrc" (fallback)
        How to easily create a .lrc file using LRCGET: ( https://github.com/tranxuanthang/lrcget/releases )
        1. Open LRCGET and switch to the "LRCLIB" tab.
        2. Search for your track, click preview icon, and press "copy".
        3. Paste into notepad and save as "Artist - Title.lrc" inside your chosen folder (select "All Files" type in Notepad to prevent saving as .lrc.txt).
        If disabled, local files are ignored and standard online LRCLIB search is used.
      $description:ru-RU: >-
        При включении текст загружается только из локальных .lrc-файлов в указанной ниже папке. Автоматический онлайн-поиск через LRCLIB полностью отключается.
        Зачем это нужно: Работает без интернета, редкие треки, отсутствующие в онлайн-базе, или ручная настройка таймингов.
        Размещение и приоритет именования файлов:
        1. "Исполнитель - Название.lrc" (рекомендуется)
        2. "Название.lrc" (резервный вариант)
        Как быстро получить .lrc-файл через LRCGET: ( https://github.com/tranxuanthang/lrcget/releases )
        1. Откройте LRCGET и перейдите во вкладку "LRCLIB".
        2. Найдите нужный трек, нажмите просмотра и нажмите "copy".
        3. Вставьте текст в Блокнот и сохраните как "Исполнитель - Название.lrc" в вашу папку (обязательно выберите тип "Все файлы (*.*)", чтобы файл не сохранился как .lrc.txt).
        Если настройка выключена, локальные файлы игнорируются и используется обычный поиск через LRCLIB.

    - localLrcFolder: ""
      $name: Local LRC folder
      $name:ru-RU: Папка локальных LRC-файлов
      $description: >-
        Full path to the folder containing your .lrc files, for example C:\Music\Lyrics.
        With "Use local LRC files" enabled, the mod searches this folder only.
      $description:ru-RU: >-
        Полный путь к папке с вашими .lrc-файлами, например C:\Music\Lyrics.
        При включённой настройке «Использовать локальные LRC-файлы» мод ищет тексты только в этой папке.

    - opacity: 90
      $name: Opacity
      $name:ru-RU: Прозрачность
      $description: Overall opacity of the lyrics widget (0-100)
      $description:ru-RU: Общая прозрачность виджета текста (0-100)

    - backgroundEnabled: true
      $name: Background
      $name:ru-RU: Фон
      $description: Enables the rounded background behind the lyrics
      $description:ru-RU: Включает скруглённый фон под текстом

    - backgroundMode: solid
      $name: Background style
      $name:ru-RU: Стиль фона
      $description: Selects the background style of the lyrics widget
      $description:ru-RU: Выбирает стиль фона виджета текста
      $options:
        - solid: Solid
        - gradient: Gradient
        - album: Album color
        - album_gradient: Album gradient
      $options:ru-RU:
        - solid: Сплошной
        - gradient: Градиент
        - album: Цвет альбома
        - album_gradient: Градиент альбома

    - backgroundColorHex: "#101012"
      $name: Background color (Hex)
      $name:ru-RU: Цвет фона (Hex)
      $description: Color used by the Solid background style
      $description:ru-RU: Цвет, используемый режимом «Сплошной»

    - backgroundGradientColorHex: "#2D2D2D"
      $name: Gradient color (Hex)
      $name:ru-RU: Цвет градиента (Hex)
      $description: Second color used by the Gradient background style
      $description:ru-RU: Второй цвет, используемый режимом «Градиент»

    - backgroundOpacity: 65
      $name: Background opacity
      $name:ru-RU: Прозрачность фона
      $description: Opacity of the lyrics background (0-100)
      $description:ru-RU: Прозрачность фона текста (0-100)

    - rounding: 14
      $name: Corner radius
      $name:ru-RU: Скругление
      $description: Corner radius of the lyrics widget (px)
      $description:ru-RU: Радиус скругления виджета текста (px)

    - borderEnabled: false
      $name: Background border
      $name:ru-RU: Рамка фона
      $description: Enables a border around the lyrics background
      $description:ru-RU: Включает рамку вокруг фона виджета текста

    - borderMode: solid_album
      $name: Border preset
      $name:ru-RU: Пресет рамки
      $description: Selects how the background border color is generated
      $description:ru-RU: Выбирает способ формирования цвета рамки
      $options:
        - solid_album: Solid album
        - gradient_album: Gradient album
        - hex: Hex
        - hex_gradient: Hex gradient
      $options:ru-RU:
        - solid_album: Сплошной от обложки
        - gradient_album: Градиент от обложки
        - hex: Hex
        - hex_gradient: Градиент Hex

    - borderThickness: 1
      $name: Border thickness
      $name:ru-RU: Толщина рамки
      $description: Width of the background border in pixels
      $description:ru-RU: Толщина рамки вокруг фона в пикселях

    - borderColorHex: "#808080"
      $name: Border color (Hex)
      $name:ru-RU: Цвет рамки (Hex)
      $description: Main border color used by Hex presets
      $description:ru-RU: Основной цвет рамки для режимов Hex

    - borderGradientColorHex: "#C0C0C0"
      $name: Border gradient color (Hex)
      $name:ru-RU: Цвет градиента рамки (Hex)
      $description: Second border color used by the Hex gradient preset
      $description:ru-RU: Второй цвет рамки для режима «Градиент Hex»

    - borderOpacity: 100
      $name: Border opacity
      $name:ru-RU: Прозрачность рамки
      $description: Opacity of the background border (0-100)
      $description:ru-RU: Прозрачность рамки вокруг фона (0-100)

  $name: Lyrics
  $name:ru-RU: Текст песни
  $description: Displays lyrics for the currently playing track
  $description:ru-RU: Показывает текст текущего трека

- AlbumWidget:
    - enabled: false
      $name: Album widget enabled
      $name:ru-RU: Включить Album Widget
      $description: Shows album artwork in the third desktop widget. ( High CPU usage )
      $description:ru-RU: Показывает обложку альбома в третьем виджете рабочего стола. ( Высокая нагрузка на ЦП )

    - attachment: separate
      $name: Album widget attachment
      $name:ru-RU: Привязка Album Widget
      $description: Controls whether the album widget is independent or attached to the lyrics/visualizer.
      $description:ru-RU: Определяет, является ли Album Widget отдельным или прикреплённым к lyrics/visualizer.
      $options:
        - separate: Separate
        - lyrics: Attached to lyrics
        - visualizer: Attached to visualizer
      $options:ru-RU:
        - separate: Отдельный
        - lyrics: Прикреплён к visualizer
        - visualizer: Прикреплён к visualizer

    - width: 180
      $name: Album widget width
      $name:ru-RU: Ширина Album Widget

    - height: 180
      $name: Album widget height
      $name:ru-RU: Высота Album Widget

    - quality: standard
      $name: Album image quality
      $name:ru-RU: Качество изображения Album Widget
      $description: Controls the resampling quality used when the album artwork cache is rebuilt. Lower values use less CPU when the artwork, size, or settings change.
      $description:ru-RU: Управляет качеством ресэмплинга при пересоздании кэша обложки. Низкие значения используют меньше ЦП при смене обложки, размера или настроек.
      $options:
        - fastest: Fastest
        - standard: Standard
        - high: High
        - highest: Highest
      $options:ru-RU:
        - fastest: Максимально быстро
        - standard: Стандарт
        - high: Высокое
        - highest: Максимальное

    - cornerRadius: 14
      $name: Album widget corner radius
      $name:ru-RU: Скругление Album Widget

    - opacity: 88
      $name: Album widget opacity
      $name:ru-RU: Прозрачность Album Widget

    - gap: 12
      $name: Album widget attachment gap
      $name:ru-RU: Отступ привязки Album Widget
      $description: Minimum clear space between the album widget and its anchor.
      $description:ru-RU: Минимальное свободное расстояние между Album Widget и привязанным виджетом.

    - maxDistance: 180
      $name: Album widget max attachment distance
      $name:ru-RU: Максимальная дистанция привязки Album Widget
      $description: Maximum allowed distance from the attached widget.
      $description:ru-RU: Максимальное разрешённое расстояние от привязанного виджета.

- Advanced:
    - showMediaEqTrayButton: true
      $name: Show Media & EQ tray button
      $name:ru-RU: Показывать кнопку Media & EQ в трее

    - desktopPlacement: behind_icons_transparent
      $name: Desktop placement
      $name:ru-RU: Размещение на рабочем столе
      $description: >-
        Where the visualizer sits relative to the desktop icons.
        "Behind icons, opaque layer" uses an opaque layer on Windows 11 24H2 and newer.
        "Behind icons, transparent" is an experimental alternative for Windows 11 24H2 and newer.
        "Above icons" is the original placement and works everywhere.
      $description:ru-RU: >-
        Где находится визуализатор относительно значков рабочего стола.
        "За значками, непрозрачный слой" использует непрозрачный слой в Windows 11 24H2 и новее.
        "За значками, прозрачно" — экспериментальный вариант для Windows 11 24H2 и новее.
        "Поверх значков" — исходное размещение, работает везде.
      $options:
        - "behind_icons_opaque": Behind icons, opaque layer (Windows 11 24H2+)
        - "behind_icons_transparent": Behind icons, transparent (experimental, Windows 11 24H2+)
        - "above_icons": Above icons (original)
      $options:ru-RU:
        - "behind_icons_opaque": За значками, непрозрачный слой (Windows 11 24H2+)
        - "behind_icons_transparent": За значками, прозрачно (эксперимент, Windows 11 24H2+)
        - "above_icons": Поверх значков (исходный вариант)

    - ForegroundImage:
        - imagePath: ""
          $name: Foreground plane image path (PNG)
          $description: Full path to a PNG image with transparency, e.g. C:\Wallpapers\foreground.png
          $name:ru-RU: Путь к картинке переднего плана (PNG)
          $description:ru-RU: Полный путь к PNG-файлу с прозрачностью, например C:\Wallpapers\foreground.png

        - imageX: 0
          $name: Image X
          $description: Horizontal position of the image (px)
          $name:ru-RU: Картинка X
          $description:ru-RU: Горизонтальное положение картинки (px)

        - imageY: 0
          $name: Image Y
          $description: Vertical position of the image (px)
          $name:ru-RU: Картинка Y
          $description:ru-RU: Вертикальное положение картинки (px)

        - imageWidth: 0
          $name: Image width
          $description: Width of the image in pixels. 0 = original image width (px)
          $name:ru-RU: Ширина картинки
          $description:ru-RU: Ширина картинки в пикселях. 0 = исходная ширина картинки (px)

        - imageHeight: 0
          $name: Image height
          $description: Height of the image in pixels. 0 = original image height (px)
          $name:ru-RU: Высота картинки
          $description:ru-RU: Высота картинки в пикселях. 0 = исходная высота картинки (px)

      $name: Foreground
      $name:ru-RU: Передний план

    - CursorInteraction:
        - dynamicWidthEnabled: false
          $name: Dynamic width from cursor
          $description: Increases the bar width when the mouse cursor gets closer
          $name:ru-RU: Включить динамическую ширину от курсора
          $description:ru-RU: Увеличивает ширину полосы при приближении курсора мыши

        - dynamicWidthRadiusX: 600
          $name: X-axis influence radius
          $description: Horizontal range in which the cursor affects the visualizer (px)
          $name:ru-RU: Радиус влияния по оси X
          $description:ru-RU: Горизонтальная зона, в пределах которой курсор влияет на визуализатор (px)

        - dynamicWidthRadiusY: 600
          $name: Y-axis influence radius
          $description: Vertical range in which the cursor affects the visualizer (px)
          $name:ru-RU: Радиус влияния по оси Y
          $description:ru-RU: Вертикальная зона, в пределах которой курсор влияет на визуализатор (px)

        - dynamicWidthMaxBonus: 5
          $name: Maximum width bonus
          $description: Maximum additional width applied to a bar directly under the cursor (px)
          $name:ru-RU: Макс. дополнительная ширина
          $description:ru-RU: Максимальная дополнительная ширина полосы прямо под курсором (px)

      $name: Cursor interaction
      $name:ru-RU: Взаимодействие с курсором

  $name: Advanced
  $name:ru-RU: Дополнительно


*/

// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <audioclient.h>
#include <tlhelp32.h>
#include <ksmedia.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <shobjidl.h>
#include <windhawk_utils.h>
#include <windhawk_api.h>
#include <wrl.h>
#include <winhttp.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>


enum WH_AUDIOCLIENT_ACTIVATION_TYPE : DWORD {
    WH_AUDIOCLIENT_ACTIVATION_TYPE_DEFAULT = 0,
    WH_AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK = 1,
};

enum WH_PROCESS_LOOPBACK_MODE : DWORD {
    WH_PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE = 0,
};

struct WH_AUDIOCLIENT_PROCESS_LOOPBACK_PARAMS {
    DWORD TargetProcessId;
    WH_PROCESS_LOOPBACK_MODE ProcessLoopbackMode;
};

struct WH_AUDIOCLIENT_ACTIVATION_PARAMS {
    WH_AUDIOCLIENT_ACTIVATION_TYPE ActivationType;
    union {
        WH_AUDIOCLIENT_PROCESS_LOOPBACK_PARAMS ProcessLoopbackParams;
    };
};


#ifndef VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK
#define VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK L"VAD\\Process_Loopback"
#endif

#include <algorithm>
#include <atomic>
#include <cmath>
#include <chrono>
#include <cstring>
#include <vector>
#include <deque>
#include <array>
#include <memory>
#include <utility>
// #include <new>
#include <mutex>
// #include <thread>
#include <shared_mutex>
#include <objidl.h>
#include <cwctype>
#include <climits>
#include <random>

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::XamlRoot;
// using winrt::Windows::UI::Xaml::Visibility;
using winrt::Windows::UI::Xaml::HorizontalAlignment;
using winrt::Windows::UI::Xaml::VerticalAlignment;
using winrt::Windows::UI::Xaml::Controls::Button;
using winrt::Windows::UI::Xaml::Controls::Grid;
using winrt::Windows::UI::Xaml::Controls::Panel;
using winrt::Windows::UI::Xaml::Controls::StackPanel;
// using winrt::Windows::UI::Xaml::Controls::TextBlock;
using winrt::Windows::UI::Xaml::Controls::ToolTip;
// using winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase;

// Advanced.desktopPlacement values.
static constexpr int DESKTOP_PLACEMENT_OPAQUE = 1;       // raised desktop: opaque holder
static constexpr int DESKTOP_PLACEMENT_TRANSPARENT = 2;  // raised desktop: DirectComposition
static constexpr int DESKTOP_PLACEMENT_ABOVE_ICONS = 3;  // original DefView child

struct VisualizerSettings {
    int barCount;
    int barWidth;
    int barSpacing;
    int orientation;
    bool mirroredVisualizer;
    int barShape;
    int interpolationMode;
    int circleRadius;
    int circleStartAngle;
    int positionX;
    int positionY;
    int maxBarHeight;
    int minBarHeight;
    int targetMonitor; // 0 = all monitors, 1+ = monitor index
    int targetFps;
    bool showMediaEqTrayButton;
    int desktopPlacement; // DESKTOP_PLACEMENT_* (Advanced.desktopPlacement)
    int sensitivity;
    int audioSource; // 0 = whole system, 1 = selected applications
    std::wstring audioApplicationName;
    int autoGainEnabled;
    int autoGainStrength;
    int attackSpeed;
    int decaySpeed;
    bool cavaSmoothingEnabled;
    float cavaNoiseReduction;
    int barStyle;
    int pointedSharpness;
    int curveWidth;
    int segmentSpacing;
    int segmentHeight;
    int cornerRadius;
    bool borderEnabled;
    int borderMode;
    int borderThickness;
    DWORD borderColor1;
    DWORD borderColor2;
    int colorMode;
    int acrylicOpacity;
    int dynamicAcrylicMinOpacity;
    bool opacityCurveEnabled;
    float opacityCurve;
    int glassHighlight;
    bool backgroundEnabled;
    int backgroundMode;
    int backgroundOpacity;
    int backgroundCornerRadius;
    int backgroundPadding;
    int backgroundHeightAdjustment;
    int backgroundBlurRadius;
    bool backgroundBorderEnabled;
    int backgroundBorderMode; // 0 = solid album, 1 = gradient album, 2 = hex, 3 = hex gradient
    int backgroundBorderThickness;
    int backgroundBorderOpacity;
    DWORD backgroundBorderColor1;
    DWORD backgroundBorderColor2;
    DWORD backgroundColor1;
    DWORD backgroundColor2;
    DWORD color1;
    DWORD color2;
    bool gradientCurveEnabled;
    float gradientCurve;
    std::wstring imagePath;
    int imageX;
    int imageY;
    int imageWidth;
    int imageHeight;
    bool dynamicWidthEnabled;
    int dynamicWidthRadiusX;
    int dynamicWidthRadiusY;
    int dynamicWidthMaxBonus;
    bool lyricsEnabled;
    bool lyricsLimitBars;
    std::wstring lyricsUnavailableText;
    int lyricsUnavailableBehavior; // 0 = fallback, 1 = hide widget, 2 = collapse to title
    bool lyricsShowArtist;
    bool lyricsShowTitle;
    bool lyricsShowLyrics;
    int lyricsTextAlignment; // 0 = left, 1 = center, 2 = right
    int lyricsFocusY; // 0..100% vertical position of current lyric
    int lyricsX;
    int lyricsY;
    int lyricsWidth;
    int lyricsHeight;
    int lyricsFontSize;
    std::wstring lyricsArtistFont;
    std::wstring lyricsLyricsFont;
    int lyricsLinesAbove;
    int lyricsLinesBelow;
    bool lyricsLongLineWrapEnabled;
    bool lyricsUseLocalLrcFiles;
    std::wstring lyricsLocalLrcFolder;
    int lyricsOpacity;
    bool lyricsBackgroundEnabled;
    int lyricsBackgroundMode; // 0 = solid, 1 = gradient, 2 = album, 3 = album gradient
    int lyricsBackgroundOpacity;
    int lyricsRounding;
    DWORD lyricsBackgroundColor1;
    DWORD lyricsBackgroundColor2;
    bool lyricsBorderEnabled;
    int lyricsBorderMode; // 0 = solid album, 1 = gradient album, 2 = hex, 3 = hex gradient
    int lyricsBorderThickness;
    int lyricsBorderOpacity;
    DWORD lyricsBorderColor1;
    DWORD lyricsBorderColor2;

    // Third desktop widget: album artwork.
    bool albumWidgetEnabled;
    int albumWidgetAttachment; // 0 = separate, 1 = lyrics, 2 = visualizer
    int albumWidgetWidth;
    int albumWidgetHeight;
    int albumWidgetQuality;
    int albumWidgetCornerRadius;
    int albumWidgetOpacity;
    int albumWidgetGap;
    int albumWidgetMaxDistance;
} g_settings{};

struct AlbumPaletteGdi {
    DWORD primary = RGB(18, 18, 18);
    DWORD secondary = RGB(45, 45, 45);
};

static AlbumPaletteGdi g_albumPalette{};
static std::mutex g_albumPaletteMutex;
static size_t g_albumPaletteHash = 0;

// Album-art bytes are produced by the same media-session worker that already
// supplies the album-derived colors. The actual GDI+ image is decoded lazily
// on the overlay thread, so no GDI+ image object crosses thread boundaries.
static std::mutex g_albumArtworkMutex;
static std::vector<BYTE> g_albumArtworkBytes;
static size_t g_albumArtworkHash = 0;
static size_t g_albumArtworkLoadedHash = 0;
static Gdiplus::Bitmap* g_albumArtworkBitmap = nullptr;
static IStream* g_albumArtworkStream = nullptr;

// The artwork and album palette only change when media metadata changes.
// Revisions let the render loop detect changes without taking the artwork/palette
// mutexes on every frame.
static std::atomic<size_t> g_albumArtworkRevision{0};
static std::atomic<size_t> g_albumPaletteRevision{0};
static std::atomic<DWORD> g_albumPalettePrimaryFast{RGB(18, 18, 18)};
static std::atomic<DWORD> g_albumPaletteSecondaryFast{RGB(45, 45, 45)};

// Final-size cache: expensive scaling, clipping, opacity, and borders are
// performed only when the artwork or relevant widget settings change.
static Gdiplus::Bitmap* g_albumWidgetCacheBitmap = nullptr;
static size_t g_albumWidgetCacheHash = 0;

// Positions are persisted in Windhawk storage, but storage reads are not needed
// in the hot render loop. Keep runtime copies and update them only on load/drag.
static std::atomic<int> g_albumWidgetSeparateX{620};
static std::atomic<int> g_albumWidgetSeparateY{620};
static std::atomic<int> g_albumWidgetLyricsOffsetX{0};
static std::atomic<int> g_albumWidgetLyricsOffsetY{0};
static std::atomic<int> g_albumWidgetVisualizerOffsetX{0};
static std::atomic<int> g_albumWidgetVisualizerOffsetY{0};

static HANDLE g_hAlbumColorThread = nullptr;
static HANDLE g_hAlbumColorStopEvent = nullptr;
static std::atomic<bool> g_albumColorRunning{false};

struct LyricsLine {
    double timeSeconds = 0.0;
    std::wstring text;
};

static std::mutex g_lyricsMutex;
static std::shared_ptr<const std::vector<LyricsLine>> g_lyricsLines;
static std::wstring g_lyricsTrackTitle;
static std::wstring g_lyricsTrackArtist;
static std::wstring g_lyricsTrackKey;
static double g_lyricsPositionSeconds = 0.0;
static double g_lyricsPlaybackRate = 1.0;
static double g_lyricsDurationSeconds = 0.0;
static ULONGLONG g_lyricsPositionAnchorTickMs = 0;
static bool g_lyricsPlaying = false;
static bool g_lyricsHasSynced = false;
static bool g_lyricsAvailable = false;
static std::atomic<bool> g_lyricsAvailableFast{false};
static HANDLE g_hLyricsThread = nullptr;
static HANDLE g_hLyricsStopEvent = nullptr;
static std::atomic<bool> g_lyricsRunning{false};


// Notification-area media player state. This is intentionally independent from
// the optional Lyrics worker so the player remains available when Lyrics is
// disabled (the default configuration).
struct EqMediaState {
    std::wstring title;
    std::wstring artist;
    std::wstring source;
    std::wstring sourceAppUserModelId;
    std::wstring trackKey;
    double positionSeconds = 0.0;
    double durationSeconds = 0.0;
    double playbackRate = 1.0;
    bool playing = false;
    bool hasSession = false;
};

static std::mutex g_eqMediaMutex;
static EqMediaState g_eqMediaState;
static HANDLE g_hEqMediaThread = nullptr;
static HANDLE g_hEqMediaStopEvent = nullptr;
static HANDLE g_hEqMediaWakeEvent = nullptr;
static std::atomic<bool> g_eqMediaRunning{false};

// Layout Builder state. The runtime representation mirrors the requested
// JSON structure:
// { "canvas": { "w": 700, "h": 530 }, "widgets": [ ... ] }
// Windhawk local storage exposes scalar values, so the same state is persisted
// as individual integer fields under MediaEQ.layout.*.
enum EqLayoutWidgetId : int {
    EQ_LAYOUT_EQ = 0,
    EQ_LAYOUT_PRESETS = 1,
    EQ_LAYOUT_MEDIA = 2,
    EQ_LAYOUT_LYRICS = 3,
};

struct EqLayoutWidgetState {
    int id = 0;
    int x = 0;
    int y = 0;
    int w = 100;
    int h = 100;
    int rotation = 0;
    bool present = false;
};

static constexpr int EQ_LAYOUT_CANVAS_W = 700;
static constexpr int EQ_LAYOUT_CANVAS_H = 530;
static constexpr int EQ_LAYOUT_GRID = 10;
static constexpr int EQ_LAYOUT_STORAGE_VERSION = 2;
static constexpr int EQ_LAYOUT_WIDGET_COUNT = 4;

static std::array<EqLayoutWidgetState, EQ_LAYOUT_WIDGET_COUNT> g_eqLayoutWidgets{};
static bool g_eqLayoutLoaded = false;

// Media & EQ popup placement editor. Values are stored per aspect ratio as
// normalized placement state (0..1000). The popup center and the guide cross
// are independent, so the popup can be moved freely in both axes.
enum EqPopupAspectIndex : int {
    EQ_POPUP_ASPECT_16_9 = 0,
    EQ_POPUP_ASPECT_4_3 = 1,
    EQ_POPUP_ASPECT_21_9 = 2,
};
static constexpr int EQ_POPUP_ASPECT_COUNT = 3;

struct EqPopupPlacementState {
    // Popup position is stored as an independent normalized center (0..1000).
    // The cross is a separate placement guide and does not constrain the popup.
    int popupCenterX = 750;
    int popupCenterY = 750;
    int crossX = 500;
    int crossY = 500;
};
static std::array<EqPopupPlacementState, EQ_POPUP_ASPECT_COUNT>
    g_eqPopupPlacement{};
static int g_eqPopupPlacementAspect = EQ_POPUP_ASPECT_16_9;
static bool g_eqPopupPlacementLoaded = false;

enum EqPopupPlacementDragTarget : int {
    EQ_POPUP_PLACEMENT_DRAG_NONE = 0,
    EQ_POPUP_PLACEMENT_DRAG_POPUP = 1,
    EQ_POPUP_PLACEMENT_DRAG_CROSS = 2,
};
static int g_eqPopupPlacementDragTarget = EQ_POPUP_PLACEMENT_DRAG_NONE;
static int g_eqPopupPlacementDragOffsetX = 0;
static int g_eqPopupPlacementDragOffsetY = 0;
// Placement dragging uses the actual desktop cursor delta from the moment the
// drag starts. This avoids mixing client coordinates, settings scroll offset,
// and DPI scaling while the popup is being moved.
static POINT g_eqPopupPlacementDragStartCursor{};
static POINT g_eqPopupPlacementDragStartCenter{};
static POINT g_eqPopupPlacementDragStartCross{};
static int g_eqSettingsScrollOffset = 0;
static bool g_eqSettingsScrollbarDragging = false;
static int g_eqSettingsScrollbarDragStartY = 0;
static int g_eqSettingsScrollbarStartOffset = 0;
static constexpr int EQ_SETTINGS_POSITION_TOP = 560;
static constexpr int EQ_SETTINGS_POSITION_CONTENT_HEIGHT = 1040;
static constexpr int EQ_SETTINGS_SCROLLBAR_LEFT = 692;
static constexpr int EQ_SETTINGS_SCROLLBAR_RIGHT = 698;

static void LoadEqLayoutSettings();
static void SaveEqLayoutSettings();
static void LoadEqPopupPlacementSettings();
static void SaveEqPopupPlacementSettings();

enum class EqMediaCommandType : int {
    PlayPause = 0,
    Previous,
    Next,
    Rewind5,
    Forward5,
    Seek,
};

struct EqMediaCommand {
    EqMediaCommandType type = EqMediaCommandType::PlayPause;
    double seekPositionSeconds = 0.0;
};

static std::mutex g_eqMediaCommandMutex;
static std::deque<EqMediaCommand> g_eqMediaCommands;

static constexpr UINT kEqMediaRefreshTimerId = 0x4E54;
static constexpr UINT kEqMediaRefreshIntervalMs = 33;

static constexpr int VIZ_FFT_SIZE = 1024;
static constexpr int VIZ_NUM_BANDS = 32;
static constexpr int VIZ_BANDS_MAX = 256;
static constexpr float VIZ_PI = 3.14159265358979323846f;

// Ten logarithmic EQ controls cover the audible range without requiring a
// second FFT or changing the existing 32 visualizer bands.
static constexpr int VIZ_EQ_BANDS = 10;
static constexpr std::array<float, VIZ_EQ_BANDS> VIZ_EQ_LOW_HZ = {
    20.0f, 60.0f, 120.0f, 250.0f, 500.0f,
    1000.0f, 2000.0f, 4000.0f, 8000.0f, 14000.0f
};
static constexpr std::array<float, VIZ_EQ_BANDS> VIZ_EQ_HIGH_HZ = {
    60.0f, 120.0f, 250.0f, 500.0f, 1000.0f,
    2000.0f, 4000.0f, 8000.0f, 14000.0f, 20000.0f
};
// Legacy single-curve storage kept for backwards compatibility.
static constexpr std::array<const wchar_t*, VIZ_EQ_BANDS> VIZ_EQ_STORAGE_KEYS = {
    L"customEqBand0", L"customEqBand1", L"customEqBand2", L"customEqBand3",
    L"customEqBand4", L"customEqBand5", L"customEqBand6", L"customEqBand7",
    L"customEqBand8", L"customEqBand9"
};

// Persistent storage for the actual curve currently shown by the EQ. This is
// deliberately separate from custom presets and from the old legacy curve so
// selecting a built-in preset can never overwrite the user's current values.
static constexpr std::array<const wchar_t*, VIZ_EQ_BANDS> VIZ_EQ_ACTIVE_STORAGE_KEYS = {
    L"eqActiveBand0", L"eqActiveBand1", L"eqActiveBand2", L"eqActiveBand3",
    L"eqActiveBand4", L"eqActiveBand5", L"eqActiveBand6", L"eqActiveBand7",
    L"eqActiveBand8", L"eqActiveBand9"
};
static constexpr wchar_t VIZ_EQ_SELECTED_PRESET_STORAGE_KEY[] =
    L"eqSelectedPreset";
static constexpr int VIZ_EQ_BUILTIN_PRESET_COUNT = 8;
static constexpr int VIZ_EQ_MAX_CUSTOM_PRESETS = 12;
static constexpr int EQ_CUSTOM_PRESET_INDEX_BASE = 100;
static std::array<std::array<std::atomic<float>, VIZ_EQ_BANDS>, VIZ_EQ_MAX_CUSTOM_PRESETS> g_eqCustomPresetGains{};
static int g_eqCustomPresetCount = 0;
static int g_eqSelectedPreset = -1;
static std::array<std::atomic<float>, VIZ_EQ_BANDS> g_customEqGains{};
static std::array<std::atomic<float>, VIZ_EQ_BANDS> g_eqActiveGains{};
static bool g_eqStorageInitialized = false;

static std::atomic<float> g_audioBands[VIZ_NUM_BANDS] = {};
static std::atomic<ULONGLONG> g_lastAudioUpdateMs{0};
static float g_currentHeights[VIZ_BANDS_MAX] = {};

// Where the desktop overlay windows live in Explorer's desktop hierarchy.
// Filled in by ResolveOverlayDesktopParent (see the desktop layering notes
// next to it) and consumed by the overlay creation and repair code.
// How overlay frames reach the screen.
static constexpr int OVERLAY_RENDER_ULW = 0;           // layered child, UpdateLayeredWindow (classic desktop, above icons)
static constexpr int OVERLAY_RENDER_OPAQUE_HOLDER = 1; // raised desktop: opaque LWA holder + D3D child, wallpaper redrawn
static constexpr int OVERLAY_RENDER_DCOMP = 2;         // raised desktop: DirectComposition premultiplied alpha

struct OverlayDesktopPlacement {
    HWND parent = nullptr;           // overlay windows (or their holders) are children of this
    HWND insertAfter = nullptr;      // raised desktop: sibling to stack directly below
    HWND wallpaperWorkerW = nullptr; // raised desktop: pushed to the bottom
    bool raised = false;             // Windows 11 24H2+ "raised desktop" layout
    bool behindIcons = false;        // overlay renders behind the desktop icons
    int renderMode = OVERLAY_RENDER_ULW;
};
static OverlayDesktopPlacement g_overlayPlacement;
static void ReassertOverlayZOrder();
static HWND ResolveOverlayDesktopParent(int placementSetting);

// Per-overlay state, parallel to g_overlayWindows (declared below).
// g_overlayHosts[i] is the opaque holder window for OVERLAY_RENDER_OPAQUE_HOLDER
// (nullptr otherwise); g_overlayWindows[i] is always the window rendered into.
static std::vector<HWND> g_overlayHosts;
static int g_overlayBuiltRenderMode = -1;
static bool g_overlayForceRebuild = false;
static bool OverlayPresentFrame(HWND hwnd, int w, int h);
static bool OverlayFillWallpaperBase(HWND hwnd, int w, int h);

static float g_hannWindow[VIZ_FFT_SIZE] = {};
static float g_twiddleRe[VIZ_FFT_SIZE / 2] = {};
static float g_twiddleIm[VIZ_FFT_SIZE / 2] = {};
static int g_logBinStart[VIZ_NUM_BANDS + 1] = {};

static HWND g_hwndOverlay = nullptr;
static std::vector<HWND> g_overlayWindows;
// Per-overlay timestamp used by the fullscreen throttle. When only one monitor
// is covered by a fullscreen app, that monitor can stay at 1 FPS while other
// overlay windows continue rendering at the configured rate.
static std::vector<ULONGLONG> g_overlayLastRenderMs;
static RECT g_overlayScreenRect{}; // Screen-space rectangle currently being rendered.
static HANDLE g_hOverlayThread = nullptr;
static HANDLE g_hOverlayStopEvent = nullptr;
static DWORD g_overlayThreadId = 0;
static std::atomic<bool> g_overlayIdle{false};
static std::atomic<HWND> g_overlayWakeHwnd{nullptr};
static std::atomic<bool> g_shellServicesStarted{false};
static constexpr UINT WM_VIZ_AUDIO_WAKE = WM_APP + 0x2A1;
static constexpr UINT WM_VIZ_REBUILD_OVERLAYS = WM_APP + 0x2A2;
static constexpr UINT WM_VIZ_OPEN_LAYOUT_EDITOR = WM_APP + 0x2A4;
static ULONG_PTR g_gdiplusToken = 0;

static HDC g_renderMemDC = nullptr;
static HBITMAP g_renderBitmap = nullptr;
static HGDIOBJ g_renderOldBitmap = nullptr;
static void* g_renderBits = nullptr;
static int g_renderWidth = 0;
static int g_renderHeight = 0;
static std::shared_mutex g_settingsMutex;

static VisualizerSettings GetSettingsSnapshot() {
    std::shared_lock<std::shared_mutex> lock(g_settingsMutex);
    return g_settings;
}

static HANDLE g_hAudioThread = nullptr;
static HANDLE g_hAudioEvent = nullptr;
static std::atomic<bool> g_running{false};
static std::atomic<bool> g_audioRunning{false};

static Gdiplus::Image* g_pForegroundImage = nullptr;
static std::atomic<bool> g_imageNeedsReload{true};

struct BackgroundBlurCacheEntry {
    RECT rect{};
    int width = 0;
    int height = 0;
    int radius = 0;
    Gdiplus::Bitmap* bitmap = nullptr;
};

static Gdiplus::Bitmap* g_pBackgroundBlurBitmap = nullptr;
static RECT g_backgroundBlurRect{};
static int g_backgroundBlurWidth = 0;
static int g_backgroundBlurHeight = 0;
static int g_backgroundBlurRadius = 0;
static std::vector<BackgroundBlurCacheEntry> g_backgroundBlurCache;
static std::atomic<bool> g_backgroundBlurNeedsReload{true};

static void LoadForegroundImage() {
    g_imageNeedsReload.store(true, std::memory_order_release);
}

static void EnsureForegroundImageLoaded() {
    if (!g_imageNeedsReload.exchange(false, std::memory_order_acq_rel)) {
        return;
    }

    if (g_pForegroundImage) {
        delete g_pForegroundImage;
        g_pForegroundImage = nullptr;
    }

    if (!g_settings.imagePath.empty()) {
        g_pForegroundImage = Gdiplus::Image::FromFile(g_settings.imagePath.c_str());
        if (g_pForegroundImage && g_pForegroundImage->GetLastStatus() != Gdiplus::Ok) {
            delete g_pForegroundImage;
            g_pForegroundImage = nullptr;
        }
    }
}

static void DestroyBackgroundBlurBitmap() {
    for (auto& entry : g_backgroundBlurCache) {
        delete entry.bitmap;
        entry.bitmap = nullptr;
    }
    g_backgroundBlurCache.clear();

    g_pBackgroundBlurBitmap = nullptr;
    g_backgroundBlurRect = {};
    g_backgroundBlurWidth = 0;
    g_backgroundBlurHeight = 0;
    g_backgroundBlurRadius = 0;
}

static void ApplyBoxBlurGdiPlus(Gdiplus::Bitmap& bitmap, int radius) {
    if (radius <= 0)
        return;

    const UINT width = bitmap.GetWidth();
    const UINT height = bitmap.GetHeight();
    if (width == 0 || height == 0)
        return;

    Gdiplus::Rect rect(0, 0, static_cast<INT>(width), static_cast<INT>(height));
    Gdiplus::BitmapData data{};
    if (bitmap.LockBits(
            &rect,
            Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,
            PixelFormat32bppARGB,
            &data) != Gdiplus::Ok)
        return;

    std::vector<BYTE> src(static_cast<size_t>(width) * height * 4);
    std::vector<BYTE> tmp(src.size());

    for (UINT y = 0; y < height; ++y) {
        std::memcpy(
            src.data() + static_cast<size_t>(y) * width * 4,
            static_cast<BYTE*>(data.Scan0) + static_cast<size_t>(y) * data.Stride,
            static_cast<size_t>(width) * 4);
    }

    const int r = std::clamp(radius, 1, 24);

    // Horizontal pass. Keep the exact same box-average definition as the
    // original implementation, but maintain a sliding sum instead of
    // rescanning up to (2*r + 1) source pixels for every output pixel.
    // This changes the work from O(width * height * radius) to O(width * height)
    // while preserving the integer sums and divisions pixel-for-pixel.
    for (UINT y = 0; y < height; ++y) {
        uint32_t sum[4] = {};
        for (int sx = 0; sx <= r && sx < static_cast<int>(width); ++sx) {
            const BYTE* p = src.data() + (static_cast<size_t>(y) * width + sx) * 4;
            for (int c = 0; c < 4; ++c)
                sum[c] += p[c];
        }

        for (UINT x = 0; x < width; ++x) {
            if (x > 0) {
                const int removeX = static_cast<int>(x) - r - 1;
                const int addX = static_cast<int>(x) + r;

                if (removeX >= 0) {
                    const BYTE* p = src.data() +
                        (static_cast<size_t>(y) * width + removeX) * 4;
                    for (int c = 0; c < 4; ++c)
                        sum[c] -= p[c];
                }

                if (addX < static_cast<int>(width)) {
                    const BYTE* p = src.data() +
                        (static_cast<size_t>(y) * width + addX) * 4;
                    for (int c = 0; c < 4; ++c)
                        sum[c] += p[c];
                }
            }

            const int x0 = std::max<int>(0, static_cast<int>(x) - r);
            const int x1 = std::min<int>(static_cast<int>(width) - 1, static_cast<int>(x) + r);
            const int count = x1 - x0 + 1;

            BYTE* out = tmp.data() + (static_cast<size_t>(y) * width + x) * 4;
            for (int c = 0; c < 4; ++c)
                out[c] = static_cast<BYTE>(sum[c] / count);
        }
    }

    // Vertical pass. Same exact box-average semantics, using sliding sums
    // down each column instead of an inner loop over every source row.
    for (UINT x = 0; x < width; ++x) {
        uint32_t sum[4] = {};
        for (int sy = 0; sy <= r && sy < static_cast<int>(height); ++sy) {
            const BYTE* p = tmp.data() + (static_cast<size_t>(sy) * width + x) * 4;
            for (int c = 0; c < 4; ++c)
                sum[c] += p[c];
        }

        for (UINT y = 0; y < height; ++y) {
            if (y > 0) {
                const int removeY = static_cast<int>(y) - r - 1;
                const int addY = static_cast<int>(y) + r;

                if (removeY >= 0) {
                    const BYTE* p = tmp.data() +
                        (static_cast<size_t>(removeY) * width + x) * 4;
                    for (int c = 0; c < 4; ++c)
                        sum[c] -= p[c];
                }

                if (addY < static_cast<int>(height)) {
                    const BYTE* p = tmp.data() +
                        (static_cast<size_t>(addY) * width + x) * 4;
                    for (int c = 0; c < 4; ++c)
                        sum[c] += p[c];
                }
            }

            const int y0 = std::max<int>(0, static_cast<int>(y) - r);
            const int y1 = std::min<int>(static_cast<int>(height) - 1, static_cast<int>(y) + r);
            const int count = y1 - y0 + 1;

            BYTE* out = static_cast<BYTE*>(data.Scan0) +
                        static_cast<size_t>(y) * data.Stride + x * 4;
            for (int c = 0; c < 4; ++c)
                out[c] = static_cast<BYTE>(sum[c] / count);
        }
    }

    bitmap.UnlockBits(&data);
}

static Gdiplus::Bitmap* CreateBackgroundBlurBitmap(const RECT& rect, int blurRadius) {
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0)
        return nullptr;

    WCHAR wallpaperPath[MAX_PATH] = {};
    if (!SystemParametersInfoW(
            SPI_GETDESKWALLPAPER,
            MAX_PATH,
            wallpaperPath,
            0) ||
        !wallpaperPath[0]) {
        return nullptr;
    }

    std::unique_ptr<Gdiplus::Image> wallpaper(
        Gdiplus::Image::FromFile(wallpaperPath, FALSE));
    if (!wallpaper || wallpaper->GetLastStatus() != Gdiplus::Ok)
        return nullptr;

    const UINT sourceWidth = wallpaper->GetWidth();
    const UINT sourceHeight = wallpaper->GetHeight();
    if (sourceWidth == 0 || sourceHeight == 0)
        return nullptr;

    auto* result = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
    if (!result || result->GetLastStatus() != Gdiplus::Ok) {
        delete result;
        return nullptr;
    }

    Gdiplus::Graphics g(result);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

    const int virtualX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int virtualY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int virtualW = std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN));
    const int virtualH = std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN));

    const float scale = std::max(
        static_cast<float>(virtualW) / static_cast<float>(sourceWidth),
        static_cast<float>(virtualH) / static_cast<float>(sourceHeight));

    const float drawW = static_cast<float>(sourceWidth) * scale;
    const float drawH = static_cast<float>(sourceHeight) * scale;
    const float drawX = (static_cast<float>(virtualW) - drawW) * 0.5f -
                        static_cast<float>(virtualX) * 0.0f;
    const float drawY = (static_cast<float>(virtualH) - drawH) * 0.5f;

    // rect is local to the selected overlay monitor. Convert it to virtual
    // desktop coordinates before sampling the wallpaper.
    const int overlayX = g_overlayScreenRect.left;
    const int overlayY = g_overlayScreenRect.top;
    const float localX =
        static_cast<float>(rect.left + overlayX - virtualX);
    const float localY =
        static_cast<float>(rect.top + overlayY - virtualY);

    g.DrawImage(
        wallpaper.get(),
        Gdiplus::RectF(
            -localX + drawX,
            -localY + drawY,
            drawW,
            drawH));

    ApplyBoxBlurGdiPlus(*result, blurRadius);
    return result;
}

static void EnsureBackgroundBlurBitmap(const RECT& rect) {
    if (g_settings.backgroundMode != 5 ||
        g_settings.backgroundOpacity <= 0) {
        DestroyBackgroundBlurBitmap();
        return;
    }

    if (g_backgroundBlurNeedsReload.exchange(false, std::memory_order_acq_rel))
        DestroyBackgroundBlurBitmap();

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        g_pBackgroundBlurBitmap = nullptr;
        return;
    }

    for (auto& entry : g_backgroundBlurCache) {
        if (entry.bitmap &&
            entry.rect.left == rect.left &&
            entry.rect.top == rect.top &&
            entry.rect.right == rect.right &&
            entry.rect.bottom == rect.bottom &&
            entry.width == width &&
            entry.height == height &&
            entry.radius == g_settings.backgroundBlurRadius) {
            g_pBackgroundBlurBitmap = entry.bitmap;
            g_backgroundBlurRect = entry.rect;
            g_backgroundBlurWidth = entry.width;
            g_backgroundBlurHeight = entry.height;
            g_backgroundBlurRadius = entry.radius;
            return;
        }
    }

    BackgroundBlurCacheEntry entry;
    entry.rect = rect;
    entry.width = width;
    entry.height = height;
    entry.radius = g_settings.backgroundBlurRadius;
    entry.bitmap = CreateBackgroundBlurBitmap(rect, g_settings.backgroundBlurRadius);
    if (!entry.bitmap) {
        g_pBackgroundBlurBitmap = nullptr;
        return;
    }

    g_pBackgroundBlurBitmap = entry.bitmap;
    g_backgroundBlurRect = entry.rect;
    g_backgroundBlurWidth = entry.width;
    g_backgroundBlurHeight = entry.height;
    g_backgroundBlurRadius = entry.radius;
    g_backgroundBlurCache.push_back(entry);
}

static DWORD ParseHexColorGDI(LPCWSTR hexStr, DWORD fallback) {
    if (!hexStr || !*hexStr)
        return fallback;

    if (hexStr[0] == L'#')
        ++hexStr;

    if (wcslen(hexStr) != 6)
        return fallback;

    for (const wchar_t* p = hexStr; *p; ++p) {
        if (!iswxdigit(*p))
            return fallback;
    }

    wchar_t* end = nullptr;
    unsigned long rgb = wcstoul(hexStr, &end, 16);
    if (end == hexStr || *end != L'\0')
        return fallback;

    return RGB(
        (rgb >> 16) & 0xFF,
        (rgb >> 8) & 0xFF,
        rgb & 0xFF);
}

static void LoadCustomEQSettings();
static void SaveCustomEQSettings();

static RECT GetThirdAlbumWidgetAnchorRect(const VisualizerSettings& settings);
static void DrawAlbumWidgetBorder(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    const VisualizerSettings& settings);

// ---------------------------------------------------------------------------
// Desktop layout overrides
// ---------------------------------------------------------------------------
// Positions applied from the desktop layout editor are kept in mod storage,
// because a Windhawk mod cannot write its own settings. Precedence rule: a
// stored position is honoured only while the Windhawk setting still equals
// the value it had when the position was applied. Changing the position in
// the Windhawk settings UI therefore always wins; the stale override is simply
// ignored (never rewritten here, since storage writes can re-enter
// Wh_ModSettingsChanged on some builds).
struct LayoutOverrideKeys {
    const wchar_t* flag;
    const wchar_t* x;
    const wchar_t* y;
    const wchar_t* baseX;
    const wchar_t* baseY;
};

static constexpr LayoutOverrideKeys kVisualizerLayoutKeys{
    L"layout.visualizer.override", L"layout.visualizer.x", L"layout.visualizer.y",
    L"layout.visualizer.base.x", L"layout.visualizer.base.y"};
static constexpr LayoutOverrideKeys kLyricsLayoutKeys{
    L"layout.lyrics.override", L"layout.lyrics.x", L"layout.lyrics.y",
    L"layout.lyrics.base.x", L"layout.lyrics.base.y"};

// x/y hold the values from the Windhawk settings on entry. They are replaced
// by the stored override when it is still valid for those setting values.
static bool ApplyLayoutOverride(const LayoutOverrideKeys& keys, int& x, int& y) {
    if (Wh_GetIntValue(keys.flag, 0) == 0)
        return false;
    if (Wh_GetIntValue(keys.baseX, 0) != x || Wh_GetIntValue(keys.baseY, 0) != y)
        return false;
    x = Wh_GetIntValue(keys.x, x);
    y = Wh_GetIntValue(keys.y, y);
    return true;
}

static void StoreLayoutOverride(
    const LayoutOverrideKeys& keys, int x, int y, int baseX, int baseY) {
    Wh_SetIntValue(keys.x, x);
    Wh_SetIntValue(keys.y, y);
    Wh_SetIntValue(keys.baseX, baseX);
    Wh_SetIntValue(keys.baseY, baseY);
    Wh_SetIntValue(keys.flag, 1);
}

static void LoadSettings() {
    LoadEqLayoutSettings();
    LoadEqPopupPlacementSettings();

    const int rawTargetMonitor = Wh_GetIntSetting(L"Performance.targetMonitor");
    g_settings.barCount = std::clamp(Wh_GetIntSetting(L"Visualizer.barCount"), 1, VIZ_BANDS_MAX);
    g_settings.barWidth = std::clamp(Wh_GetIntSetting(L"Visualizer.barWidth"), 1, 50);
    g_settings.barSpacing = std::clamp(Wh_GetIntSetting(L"Visualizer.barSpacing"), 0, 50);
    g_settings.positionX = Wh_GetIntSetting(L"Visualizer.positionX");
    g_settings.positionY = Wh_GetIntSetting(L"Visualizer.positionY");
    ApplyLayoutOverride(kVisualizerLayoutKeys, g_settings.positionX, g_settings.positionY);
    g_settings.maxBarHeight = std::clamp(Wh_GetIntSetting(L"Visualizer.maxBarHeight"), 1, 2000);
    g_settings.minBarHeight = std::clamp(Wh_GetIntSetting(L"Visualizer.minBarHeight"), 0, 1000);
    g_settings.targetMonitor = std::max(0, rawTargetMonitor);
    g_settings.targetFps = std::max(0, Wh_GetIntSetting(L"Performance.targetFps"));
    g_settings.showMediaEqTrayButton =
        Wh_GetIntSetting(L"Advanced.showMediaEqTrayButton") != 0;
    g_settings.desktopPlacement = DESKTOP_PLACEMENT_TRANSPARENT;
    if (PCWSTR placement = Wh_GetStringSetting(L"Advanced.desktopPlacement")) {
        if (wcscmp(placement, L"behind_icons_opaque") == 0)
            g_settings.desktopPlacement = DESKTOP_PLACEMENT_OPAQUE;
        else if (wcscmp(placement, L"behind_icons_transparent") == 0)
            g_settings.desktopPlacement = DESKTOP_PLACEMENT_TRANSPARENT;
        else if (wcscmp(placement, L"above_icons") == 0)
            g_settings.desktopPlacement = DESKTOP_PLACEMENT_ABOVE_ICONS;
        Wh_FreeStringSetting(placement);
    }
    g_settings.sensitivity = std::clamp(Wh_GetIntSetting(L"Audio.sensitivity"), 0, 300);

    PCWSTR audioSourceStr = Wh_GetStringSetting(L"Audio.Source.audioSource");
    g_settings.audioSource = 0;
    if (audioSourceStr) {
        if (wcscmp(audioSourceStr, L"application") == 0)
            g_settings.audioSource = 1;
        Wh_FreeStringSetting(audioSourceStr);
    }

    PCWSTR audioApplicationNameStr =
        Wh_GetStringSetting(L"Audio.Source.audioApplicationName");
    if (audioApplicationNameStr) {
        g_settings.audioApplicationName = audioApplicationNameStr;
        Wh_FreeStringSetting(audioApplicationNameStr);
    } else {
        g_settings.audioApplicationName.clear();
    }

    g_settings.autoGainEnabled = Wh_GetIntSetting(L"Audio.AutoGain.autoGainEnabled") != 0;
    g_settings.autoGainStrength = std::clamp(
        Wh_GetIntSetting(L"Audio.AutoGain.autoGainStrength"), 0, 100);
    g_settings.attackSpeed = std::clamp(Wh_GetIntSetting(L"Audio.Dynamics.attackSpeed"), 1, 100);
    g_settings.decaySpeed = std::clamp(Wh_GetIntSetting(L"Audio.Dynamics.decaySpeed"), 1, 100);
    g_settings.cavaSmoothingEnabled =
        Wh_GetIntSetting(L"Audio.Dynamics.cavaSmoothingEnabled") != 0;
    g_settings.cavaNoiseReduction = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"Audio.Dynamics.cavaNoiseReduction"), 0, 100));
    g_settings.pointedSharpness = std::clamp(
        Wh_GetIntSetting(L"Appearance.Style.pointedSharpness"), 0, 100);
    g_settings.curveWidth = std::clamp(
        Wh_GetIntSetting(L"Appearance.Style.curveWidth"), 50, 5000);
    g_settings.segmentSpacing = std::clamp(
        Wh_GetIntSetting(L"Appearance.Style.segmentSpacing"), 0, 50);
    g_settings.segmentHeight = std::clamp(
        Wh_GetIntSetting(L"Appearance.Style.segmentHeight"), 0, 200);
    g_settings.cornerRadius = std::clamp(Wh_GetIntSetting(L"Appearance.Style.cornerRadius"), 0, 25);
    g_settings.acrylicOpacity = std::clamp(Wh_GetIntSetting(L"Appearance.Opacity.acrylicOpacity"), 0, 100);
    g_settings.dynamicAcrylicMinOpacity = std::clamp(Wh_GetIntSetting(L"Appearance.Opacity.dynamicAcrylicMinOpacity"), 0, 100);
    g_settings.gradientCurveEnabled = Wh_GetIntSetting(L"Appearance.Colors.gradientCurveEnabled") != 0;
    g_settings.opacityCurveEnabled = Wh_GetIntSetting(L"Appearance.Opacity.opacityCurveEnabled") != 0;
    g_settings.glassHighlight = std::clamp(Wh_GetIntSetting(L"Appearance.Colors.glassHighlight"), 0, 100);

    g_settings.backgroundEnabled =
        Wh_GetIntSetting(L"Appearance.Background.backgroundEnabled") != 0;
    g_settings.backgroundOpacity = std::clamp(
        Wh_GetIntSetting(L"Appearance.Background.backgroundOpacity"), 0, 100);
    g_settings.backgroundCornerRadius = std::clamp(
        Wh_GetIntSetting(L"Appearance.Background.backgroundCornerRadius"), 0, 100);
    g_settings.backgroundPadding = std::clamp(
        Wh_GetIntSetting(L"Appearance.Background.backgroundPadding"), 0, 100);
    g_settings.backgroundHeightAdjustment = std::clamp(
        Wh_GetIntSetting(L"Appearance.Background.backgroundHeightAdjustment"), -1000, 1000);
    g_settings.backgroundBlurRadius = std::clamp(
        Wh_GetIntSetting(L"Appearance.Background.backgroundBlurRadius"), 1, 24);

    PCWSTR backgroundModeStr = Wh_GetStringSetting(L"Appearance.Background.backgroundMode");
    if (backgroundModeStr) {
        if (wcscmp(backgroundModeStr, L"gradient") == 0)
            g_settings.backgroundMode = 1;
        else if (wcscmp(backgroundModeStr, L"album") == 0)
            g_settings.backgroundMode = 3;
        else if (wcscmp(backgroundModeStr, L"album_gradient") == 0)
            g_settings.backgroundMode = 4;
        else if (wcscmp(backgroundModeStr, L"blur") == 0)
            g_settings.backgroundMode = 5;
        else
            g_settings.backgroundMode = 0;
        Wh_FreeStringSetting(backgroundModeStr);
    } else {
        g_settings.backgroundMode = 0;
    }

    PCWSTR backgroundColorStr =
        Wh_GetStringSetting(L"Appearance.Background.backgroundColorHex");
    g_settings.backgroundColor1 = ParseHexColorGDI(
        backgroundColorStr, RGB(124, 104, 232));
    if (backgroundColorStr)
        Wh_FreeStringSetting(backgroundColorStr);

    PCWSTR backgroundColor2Str =
        Wh_GetStringSetting(L"Appearance.Background.backgroundGradientColorHex");
    g_settings.backgroundColor2 = ParseHexColorGDI(
        backgroundColor2Str, RGB(169, 140, 255));
    if (backgroundColor2Str)
        Wh_FreeStringSetting(backgroundColor2Str);

    g_settings.backgroundBorderEnabled =
        Wh_GetIntSetting(L"Appearance.Background.backgroundBorderEnabled") != 0;
    g_settings.backgroundBorderThickness = std::clamp(
        Wh_GetIntSetting(L"Appearance.Background.backgroundBorderThickness"), 1, 10);
    g_settings.backgroundBorderOpacity = std::clamp(
        Wh_GetIntSetting(L"Appearance.Background.backgroundBorderOpacity"), 0, 100);

    PCWSTR backgroundBorderModeStr =
        Wh_GetStringSetting(L"Appearance.Background.backgroundBorderMode");
    g_settings.backgroundBorderMode = 0;
    if (backgroundBorderModeStr) {
        if (wcscmp(backgroundBorderModeStr, L"gradient_album") == 0)
            g_settings.backgroundBorderMode = 1;
        else if (wcscmp(backgroundBorderModeStr, L"hex") == 0)
            g_settings.backgroundBorderMode = 2;
        else if (wcscmp(backgroundBorderModeStr, L"hex_gradient") == 0)
            g_settings.backgroundBorderMode = 3;
        Wh_FreeStringSetting(backgroundBorderModeStr);
    }

    PCWSTR backgroundBorderColor1Str =
        Wh_GetStringSetting(L"Appearance.Background.backgroundBorderColorHex");
    g_settings.backgroundBorderColor1 = ParseHexColorGDI(
        backgroundBorderColor1Str, RGB(128, 128, 128));
    if (backgroundBorderColor1Str)
        Wh_FreeStringSetting(backgroundBorderColor1Str);

    PCWSTR backgroundBorderColor2Str =
        Wh_GetStringSetting(L"Appearance.Background.backgroundBorderGradientColorHex");
    g_settings.backgroundBorderColor2 = ParseHexColorGDI(
        backgroundBorderColor2Str, RGB(192, 192, 192));
    if (backgroundBorderColor2Str)
        Wh_FreeStringSetting(backgroundBorderColor2Str);

    g_backgroundBlurNeedsReload.store(true, std::memory_order_release);


    LoadCustomEQSettings();

    PCWSTR barShapeStr = Wh_GetStringSetting(L"Visualizer.barShape");
    if (barShapeStr) {
        if (wcscmp(barShapeStr, L"mountain") == 0) g_settings.barShape = 1;
        else if (wcscmp(barShapeStr, L"mirror") == 0) g_settings.barShape = 2;
        else if (wcscmp(barShapeStr, L"wave") == 0) g_settings.barShape = 3;
        else if (wcscmp(barShapeStr, L"circular") == 0) g_settings.barShape = 4;
        else if (wcscmp(barShapeStr, L"dots") == 0) g_settings.barShape = 5;
        else if (wcscmp(barShapeStr, L"area") == 0) g_settings.barShape = 6;
        else g_settings.barShape = 0; // stereo
        Wh_FreeStringSetting(barShapeStr);
    } else {
        g_settings.barShape = 0;
    }

    PCWSTR interpolationStr = Wh_GetStringSetting(L"Visualizer.interpolationMode");
    if (interpolationStr) {
        if (wcscmp(interpolationStr, L"step") == 0)
            g_settings.interpolationMode = 1;
        else if (wcscmp(interpolationStr, L"cosine") == 0)
            g_settings.interpolationMode = 2;
        else if (wcscmp(interpolationStr, L"catmull_rom") == 0)
            g_settings.interpolationMode = 3;
        else
            g_settings.interpolationMode = 0;
        Wh_FreeStringSetting(interpolationStr);
    } else {
        g_settings.interpolationMode = 0;
    }

    g_settings.circleRadius = std::clamp(
        Wh_GetIntSetting(L"Visualizer.Circular.circleRadius"), 10, 2000);
    g_settings.circleStartAngle = std::clamp(
        Wh_GetIntSetting(L"Visualizer.Circular.circleStartAngle"), -360, 360);

    PCWSTR orientationStr = Wh_GetStringSetting(L"Visualizer.orientation");
    if (orientationStr) {
        if (wcscmp(orientationStr, L"center_vertical") == 0) g_settings.orientation = 1;
        else if (wcscmp(orientationStr, L"top_down") == 0) g_settings.orientation = 2;
        else if (wcscmp(orientationStr, L"left_right") == 0) g_settings.orientation = 3;
        else if (wcscmp(orientationStr, L"center_horizontal") == 0) g_settings.orientation = 4;
        else if (wcscmp(orientationStr, L"right_left") == 0) g_settings.orientation = 5;
        else g_settings.orientation = 0;
        Wh_FreeStringSetting(orientationStr);
    } else {
        g_settings.orientation = 0;
    }

    g_settings.mirroredVisualizer =
        Wh_GetIntSetting(L"Visualizer.mirroredVisualizer") != 0;

    PCWSTR barStyleStr = Wh_GetStringSetting(L"Appearance.Style.barStyle");
    if (barStyleStr) {
        if (wcscmp(barStyleStr, L"square") == 0) g_settings.barStyle = 0;
        else if (wcscmp(barStyleStr, L"segmented_square") == 0) g_settings.barStyle = 2;
        else if (wcscmp(barStyleStr, L"pointed") == 0) g_settings.barStyle = 4;
        else if (wcscmp(barStyleStr, L"curve") == 0) g_settings.barStyle = 3;
        else if (wcscmp(barStyleStr, L"battery") == 0) g_settings.barStyle = 5;
        else g_settings.barStyle = 1;
        Wh_FreeStringSetting(barStyleStr);
    } else {
        g_settings.barStyle = 1;
    }

    g_settings.borderEnabled =
        Wh_GetIntSetting(L"Appearance.Style.borderEnabled") != 0;
    g_settings.borderThickness = std::clamp(
        Wh_GetIntSetting(L"Appearance.Style.borderThickness"), 1, 10);

    PCWSTR borderModeStr = Wh_GetStringSetting(L"Appearance.Colors.borderMode");
    g_settings.borderMode = 0;
    if (borderModeStr) {
        if (wcscmp(borderModeStr, L"gradient") == 0)
            g_settings.borderMode = 1;
        else if (wcscmp(borderModeStr, L"solid_album") == 0)
            g_settings.borderMode = 2;
        else if (wcscmp(borderModeStr, L"gradient_album") == 0)
            g_settings.borderMode = 3;
        else if (wcscmp(borderModeStr, L"hex") == 0)
            g_settings.borderMode = 4;
        else if (wcscmp(borderModeStr, L"hex_gradient") == 0)
            g_settings.borderMode = 5;
        Wh_FreeStringSetting(borderModeStr);
    }

    PCWSTR borderColor1Str = Wh_GetStringSetting(L"Appearance.Colors.borderColorHex");
    g_settings.borderColor1 = ParseHexColorGDI(
        borderColor1Str, RGB(255, 255, 255));
    if (borderColor1Str)
        Wh_FreeStringSetting(borderColor1Str);

    PCWSTR borderColor2Str = Wh_GetStringSetting(L"Appearance.Colors.borderGradientColorHex");
    g_settings.borderColor2 = ParseHexColorGDI(
        borderColor2Str, RGB(0, 180, 255));
    if (borderColor2Str)
        Wh_FreeStringSetting(borderColor2Str);

    PCWSTR colorModeStr = Wh_GetStringSetting(L"Appearance.Colors.colorMode");
    if (colorModeStr) {
        if (wcscmp(colorModeStr, L"solid") == 0) g_settings.colorMode = 0;
        else if (wcscmp(colorModeStr, L"gradient") == 0) g_settings.colorMode = 1;
        else if (wcscmp(colorModeStr, L"gradient_vertical") == 0) g_settings.colorMode = 6;
        else if (wcscmp(colorModeStr, L"solid_album") == 0) g_settings.colorMode = 7;
        else if (wcscmp(colorModeStr, L"gradient_album") == 0) g_settings.colorMode = 8;
        else if (wcscmp(colorModeStr, L"dynamic_acrylic") == 0) g_settings.colorMode = 3;
        else if (wcscmp(colorModeStr, L"liquid_glass") == 0) g_settings.colorMode = 4;
        else if (wcscmp(colorModeStr, L"aero_glass") == 0) g_settings.colorMode = 5;
        else g_settings.colorMode = 3;
        Wh_FreeStringSetting(colorModeStr);
    } else {
        g_settings.colorMode = 3;
    }

    PCWSTR colorStr = Wh_GetStringSetting(L"Appearance.Colors.colorHex");
    g_settings.color1 = ParseHexColorGDI(colorStr, RGB(255, 255, 255));
    Wh_FreeStringSetting(colorStr);

    PCWSTR color2Str = Wh_GetStringSetting(L"Appearance.Colors.gradientColorHex");
    if (!color2Str || !*color2Str) {
        g_settings.color2 = g_settings.color1;
    } else {
        g_settings.color2 = ParseHexColorGDI(color2Str, g_settings.color1);
    }
    if (color2Str) {
        Wh_FreeStringSetting(color2Str);
    }

    if (g_settings.barStyle == 0)
        g_settings.cornerRadius = 0;
    
    g_settings.imageX = Wh_GetIntSetting(L"Advanced.ForegroundImage.imageX");
    g_settings.imageY = Wh_GetIntSetting(L"Advanced.ForegroundImage.imageY");
    g_settings.imageWidth = Wh_GetIntSetting(L"Advanced.ForegroundImage.imageWidth");
    g_settings.imageHeight = Wh_GetIntSetting(L"Advanced.ForegroundImage.imageHeight");

    PCWSTR imgPathStr = Wh_GetStringSetting(L"Advanced.ForegroundImage.imagePath");
    if (imgPathStr) {
        g_settings.imagePath = imgPathStr;
        Wh_FreeStringSetting(imgPathStr);
    } else {
        g_settings.imagePath.clear();
    }
    g_settings.dynamicWidthEnabled = Wh_GetIntSetting(L"Advanced.CursorInteraction.dynamicWidthEnabled") != 0;
    g_settings.dynamicWidthRadiusX = std::clamp(Wh_GetIntSetting(L"Advanced.CursorInteraction.dynamicWidthRadiusX"), 10, 3000);
    g_settings.dynamicWidthRadiusY = std::clamp(Wh_GetIntSetting(L"Advanced.CursorInteraction.dynamicWidthRadiusY"), 10, 3000);
    g_settings.dynamicWidthMaxBonus = std::clamp(Wh_GetIntSetting(L"Advanced.CursorInteraction.dynamicWidthMaxBonus"), 0, 100);
    g_settings.lyricsEnabled = Wh_GetIntSetting(L"Lyrics.enabled") != 0;
    g_settings.lyricsLimitBars =
        Wh_GetIntSetting(L"Lyrics.limitBars") != 0;

    PCWSTR lyricsUnavailableTextStr =
        Wh_GetStringSetting(L"Lyrics.unavailableText");
    if (lyricsUnavailableTextStr) {
        g_settings.lyricsUnavailableText = lyricsUnavailableTextStr;
        Wh_FreeStringSetting(lyricsUnavailableTextStr);
    } else {
        g_settings.lyricsUnavailableText.clear();
    }

    PCWSTR lyricsUnavailableBehaviorStr =
        Wh_GetStringSetting(L"Lyrics.unavailableBehavior");
    if (lyricsUnavailableBehaviorStr) {
        if (wcscmp(lyricsUnavailableBehaviorStr, L"hide") == 0)
            g_settings.lyricsUnavailableBehavior = 1;
        else if (wcscmp(lyricsUnavailableBehaviorStr, L"collapse") == 0)
            g_settings.lyricsUnavailableBehavior = 2;
        else
            g_settings.lyricsUnavailableBehavior = 0;
        Wh_FreeStringSetting(lyricsUnavailableBehaviorStr);
    } else {
        g_settings.lyricsUnavailableBehavior = 0;
    }

    g_settings.lyricsShowArtist =
        Wh_GetIntSetting(L"Lyrics.showArtist") != 0;
    g_settings.lyricsShowTitle =
        Wh_GetIntSetting(L"Lyrics.showTitle") != 0;
    g_settings.lyricsShowLyrics =
        Wh_GetIntSetting(L"Lyrics.showLyrics") != 0;

    PCWSTR lyricsAlignmentStr =
        Wh_GetStringSetting(L"Lyrics.textAlignment");
    if (lyricsAlignmentStr) {
        if (wcscmp(lyricsAlignmentStr, L"left") == 0)
            g_settings.lyricsTextAlignment = 0;
        else if (wcscmp(lyricsAlignmentStr, L"right") == 0)
            g_settings.lyricsTextAlignment = 2;
        else
            g_settings.lyricsTextAlignment = 1;
        Wh_FreeStringSetting(lyricsAlignmentStr);
    } else {
        g_settings.lyricsTextAlignment = 1;
    }

    g_settings.lyricsFocusY = std::clamp(
        Wh_GetIntSetting(L"Lyrics.focusY"), 0, 100);

    g_settings.lyricsX = Wh_GetIntSetting(L"Lyrics.positionX");
    g_settings.lyricsY = Wh_GetIntSetting(L"Lyrics.positionY");
    ApplyLayoutOverride(kLyricsLayoutKeys, g_settings.lyricsX, g_settings.lyricsY);
    g_settings.lyricsWidth = std::clamp(Wh_GetIntSetting(L"Lyrics.width"), 160, 1600);
    g_settings.lyricsHeight = std::clamp(Wh_GetIntSetting(L"Lyrics.height"), 100, 900);
    g_settings.lyricsFontSize = std::clamp(Wh_GetIntSetting(L"Lyrics.fontSize"), 10, 72);

    auto LoadLyricsFontSetting = [](LPCWSTR path, const wchar_t* fallback) {
        PCWSTR value = Wh_GetStringSetting(path);
        std::wstring result = value ? value : fallback;
        if (value)
            Wh_FreeStringSetting(value);
        return result;
    };

    g_settings.lyricsArtistFont =
        LoadLyricsFontSetting(L"Lyrics.artistFont", L"segoe_ui");
    g_settings.lyricsLyricsFont =
        LoadLyricsFontSetting(L"Lyrics.lyricsFont", L"segoe_ui");

    g_settings.lyricsLinesAbove = std::clamp(Wh_GetIntSetting(L"Lyrics.linesAbove"), 0, 4);
    g_settings.lyricsLinesBelow = std::clamp(Wh_GetIntSetting(L"Lyrics.linesBelow"), 0, 6);
    g_settings.lyricsLongLineWrapEnabled =
        Wh_GetIntSetting(L"Lyrics.longLineWrapEnabled") != 0;
    g_settings.lyricsUseLocalLrcFiles =
        Wh_GetIntSetting(L"Lyrics.useLocalLrcFiles") != 0;

    PCWSTR lyricsLocalLrcFolderStr =
        Wh_GetStringSetting(L"Lyrics.localLrcFolder");
    if (lyricsLocalLrcFolderStr) {
        g_settings.lyricsLocalLrcFolder = lyricsLocalLrcFolderStr;
        while (!g_settings.lyricsLocalLrcFolder.empty() &&
               (g_settings.lyricsLocalLrcFolder.back() == L' ' ||
                g_settings.lyricsLocalLrcFolder.back() == L'\\')) {
            if (g_settings.lyricsLocalLrcFolder.size() == 3 &&
                g_settings.lyricsLocalLrcFolder[1] == L':')
                break;
            g_settings.lyricsLocalLrcFolder.pop_back();
        }
        Wh_FreeStringSetting(lyricsLocalLrcFolderStr);
    } else {
        g_settings.lyricsLocalLrcFolder.clear();
    }
    g_settings.lyricsOpacity = std::clamp(Wh_GetIntSetting(L"Lyrics.opacity"), 0, 100);
    g_settings.lyricsBackgroundEnabled = Wh_GetIntSetting(L"Lyrics.backgroundEnabled") != 0;

    PCWSTR lyricsBackgroundModeStr =
        Wh_GetStringSetting(L"Lyrics.backgroundMode");
    g_settings.lyricsBackgroundMode = 0; // solid
    if (lyricsBackgroundModeStr) {
        if (wcscmp(lyricsBackgroundModeStr, L"gradient") == 0)
            g_settings.lyricsBackgroundMode = 1;
        else if (wcscmp(lyricsBackgroundModeStr, L"album") == 0)
            g_settings.lyricsBackgroundMode = 2;
        else if (wcscmp(lyricsBackgroundModeStr, L"album_gradient") == 0)
            g_settings.lyricsBackgroundMode = 3;
        Wh_FreeStringSetting(lyricsBackgroundModeStr);
    }

    PCWSTR lyricsBackgroundColor1Str =
        Wh_GetStringSetting(L"Lyrics.backgroundColorHex");
    g_settings.lyricsBackgroundColor1 = ParseHexColorGDI(
        lyricsBackgroundColor1Str, RGB(16, 16, 18));
    if (lyricsBackgroundColor1Str)
        Wh_FreeStringSetting(lyricsBackgroundColor1Str);

    PCWSTR lyricsBackgroundColor2Str =
        Wh_GetStringSetting(L"Lyrics.backgroundGradientColorHex");
    g_settings.lyricsBackgroundColor2 = ParseHexColorGDI(
        lyricsBackgroundColor2Str, RGB(45, 45, 45));
    if (lyricsBackgroundColor2Str)
        Wh_FreeStringSetting(lyricsBackgroundColor2Str);

    g_settings.lyricsBackgroundOpacity = std::clamp(
        Wh_GetIntSetting(L"Lyrics.backgroundOpacity"), 0, 100);
    g_settings.lyricsRounding = std::clamp(
        Wh_GetIntSetting(L"Lyrics.rounding"), 0, 50);

    PCWSTR lyricsBorderModeStr =
        Wh_GetStringSetting(L"Lyrics.borderMode");
    g_settings.lyricsBorderMode = 0;
    if (lyricsBorderModeStr) {
        if (wcscmp(lyricsBorderModeStr, L"gradient_album") == 0)
            g_settings.lyricsBorderMode = 1;
        else if (wcscmp(lyricsBorderModeStr, L"hex") == 0)
            g_settings.lyricsBorderMode = 2;
        else if (wcscmp(lyricsBorderModeStr, L"hex_gradient") == 0)
            g_settings.lyricsBorderMode = 3;
        Wh_FreeStringSetting(lyricsBorderModeStr);
    }

    g_settings.lyricsBorderEnabled =
        Wh_GetIntSetting(L"Lyrics.borderEnabled") != 0;
    g_settings.lyricsBorderThickness = std::clamp(
        Wh_GetIntSetting(L"Lyrics.borderThickness"), 1, 20);
    g_settings.lyricsBorderOpacity = std::clamp(
        Wh_GetIntSetting(L"Lyrics.borderOpacity"), 0, 100);

    PCWSTR lyricsBorderColor1Str =
        Wh_GetStringSetting(L"Lyrics.borderColorHex");
    g_settings.lyricsBorderColor1 = ParseHexColorGDI(
        lyricsBorderColor1Str, RGB(128, 128, 128));
    if (lyricsBorderColor1Str)
        Wh_FreeStringSetting(lyricsBorderColor1Str);

    PCWSTR lyricsBorderColor2Str =
        Wh_GetStringSetting(L"Lyrics.borderGradientColorHex");
    g_settings.lyricsBorderColor2 = ParseHexColorGDI(
        lyricsBorderColor2Str, RGB(192, 192, 192));
    if (lyricsBorderColor2Str)
        Wh_FreeStringSetting(lyricsBorderColor2Str);

    g_settings.gradientCurve = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"Appearance.Colors.gradientCurve"), -100, 100));

    g_settings.opacityCurve = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"Appearance.Opacity.opacityCurve"), -100, 100));

    g_settings.albumWidgetEnabled =
        Wh_GetIntSetting(L"AlbumWidget.enabled") != 0;
    g_settings.albumWidgetWidth = std::clamp(
        Wh_GetIntSetting(L"AlbumWidget.width"), 20, 2000);
    g_settings.albumWidgetHeight = std::clamp(
        Wh_GetIntSetting(L"AlbumWidget.height"), 20, 2000);
    PCWSTR albumWidgetQualityStr =
        Wh_GetStringSetting(L"AlbumWidget.quality");
    g_settings.albumWidgetQuality = 1; // Standard
    if (albumWidgetQualityStr) {
        if (wcscmp(albumWidgetQualityStr, L"fastest") == 0)
            g_settings.albumWidgetQuality = 0;
        else if (wcscmp(albumWidgetQualityStr, L"high") == 0)
            g_settings.albumWidgetQuality = 2;
        else if (wcscmp(albumWidgetQualityStr, L"highest") == 0)
            g_settings.albumWidgetQuality = 3;
        Wh_FreeStringSetting(albumWidgetQualityStr);
    }
    g_settings.albumWidgetCornerRadius = std::clamp(
        Wh_GetIntSetting(L"AlbumWidget.cornerRadius"), 0, 1000);
    g_settings.albumWidgetOpacity = std::clamp(
        Wh_GetIntSetting(L"AlbumWidget.opacity"), 0, 100);
    g_settings.albumWidgetGap = std::clamp(
        Wh_GetIntSetting(L"AlbumWidget.gap"), 0, 500);
    g_settings.albumWidgetMaxDistance = std::clamp(
        Wh_GetIntSetting(L"AlbumWidget.maxDistance"), 0, 2000);

    PCWSTR albumWidgetAttachmentStr =
        Wh_GetStringSetting(L"AlbumWidget.attachment");
    g_settings.albumWidgetAttachment = 0;
    if (albumWidgetAttachmentStr) {
        if (wcscmp(albumWidgetAttachmentStr, L"lyrics") == 0)
            g_settings.albumWidgetAttachment = 1;
        else if (wcscmp(albumWidgetAttachmentStr, L"visualizer") == 0)
            g_settings.albumWidgetAttachment = 2;
        Wh_FreeStringSetting(albumWidgetAttachmentStr);
    }

    g_albumWidgetSeparateX.store(
        Wh_GetIntValue(L"albumWidget.separate.x", 620),
        std::memory_order_release);
    g_albumWidgetSeparateY.store(
        Wh_GetIntValue(L"albumWidget.separate.y", 620),
        std::memory_order_release);

    if (g_settings.albumWidgetAttachment == 1) {
        const RECT anchor = GetThirdAlbumWidgetAnchorRect(g_settings);
        g_albumWidgetLyricsOffsetX.store(
            Wh_GetIntValue(L"albumWidget.lyrics.offsetX",
                           anchor.right - anchor.left + g_settings.albumWidgetGap),
            std::memory_order_release);
        g_albumWidgetLyricsOffsetY.store(
            Wh_GetIntValue(L"albumWidget.lyrics.offsetY", 0),
            std::memory_order_release);
    } else if (g_settings.albumWidgetAttachment == 2) {
        const RECT anchor = GetThirdAlbumWidgetAnchorRect(g_settings);
        g_albumWidgetVisualizerOffsetX.store(
            Wh_GetIntValue(L"albumWidget.visualizer.offsetX",
                           anchor.right - anchor.left + g_settings.albumWidgetGap),
            std::memory_order_release);
        g_albumWidgetVisualizerOffsetY.store(
            Wh_GetIntValue(L"albumWidget.visualizer.offsetY", 0),
            std::memory_order_release);
    }

    LoadForegroundImage();
}

static BYTE ChannelLerp(BYTE a, BYTE b, float t) {
    return static_cast<BYTE>(static_cast<int>(a) +
        static_cast<int>((static_cast<int>(b) - static_cast<int>(a)) * t));
}

static DWORD LerpColor(DWORD a, DWORD b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return RGB(
        ChannelLerp(GetRValue(a), GetRValue(b), t),
        ChannelLerp(GetGValue(a), GetGValue(b), t),
        ChannelLerp(GetBValue(a), GetBValue(b), t));
}

// Optional symmetric response curve.
// curve = 0   -> linear
// curve < 0   -> faster change near the start
// curve > 0   -> slower change near the start, faster near the end
static float ApplyHeightCurve(float t, bool enabled, float curve) {
    t = std::clamp(t, 0.0f, 1.0f);

    if (!enabled || fabsf(curve) < 0.0001f)
        return t;

    curve = std::clamp(curve, -100.0f, 100.0f);

    // Symmetric mapping:
    // -100 -> exponent 0.1
    //    0 -> exponent 1.0 (linear)
    // +100 -> exponent 10.0
    // Every step away from 0 changes the curve multiplicatively.
    const float exponent = powf(10.0f, curve / 100.0f);

    return std::clamp(powf(t, exponent), 0.0f, 1.0f);
}

static void BuildHannWindow() {
    for (int i = 0; i < VIZ_FFT_SIZE; ++i) {
        g_hannWindow[i] = 0.5f *
            (1.0f - cosf(2.0f * VIZ_PI * i / (VIZ_FFT_SIZE - 1)));
    }
}

static void BuildTwiddleFactors() {
    for (int i = 0; i < VIZ_FFT_SIZE / 2; ++i) {
        float angle = -2.0f * VIZ_PI * i / VIZ_FFT_SIZE;
        g_twiddleRe[i] = cosf(angle);
        g_twiddleIm[i] = sinf(angle);
    }
}

static void BuildLogBins(UINT32 sampleRate) {
    if (sampleRate == 0)
        sampleRate = 48000;

    constexpr float minFreq = 20.0f;
    constexpr float maxFreq = 20000.0f;
    const float logRatio = logf(maxFreq / minFreq);

    for (int b = 0; b <= VIZ_NUM_BANDS; ++b) {
        const float t = b / static_cast<float>(VIZ_NUM_BANDS);
        const float frequency = minFreq * expf(logRatio * t);
        const int bin = static_cast<int>(frequency * VIZ_FFT_SIZE /
                                         static_cast<float>(sampleRate));
        g_logBinStart[b] = std::clamp(bin, 1, VIZ_FFT_SIZE / 2 - 1);
    }

    for (int b = 1; b <= VIZ_NUM_BANDS; ++b)
        g_logBinStart[b] = std::max(g_logBinStart[b], g_logBinStart[b - 1] + 1);

}

static void VizFFT(std::vector<float>& re, std::vector<float>& im) {
    const int n = static_cast<int>(re.size());

    for (int i = 1, j = 0; i < n; ++i) {
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
        const int halfLen = len / 2;
        const int stride = n / len;

        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < halfLen; ++j) {
                const float wRe = g_twiddleRe[j * stride];
                const float wIm = g_twiddleIm[j * stride];

                const float uRe = re[i + j];
                const float uIm = im[i + j];
                const float vRe = re[i + j + halfLen] * wRe -
                                  im[i + j + halfLen] * wIm;
                const float vIm = re[i + j + halfLen] * wIm +
                                  im[i + j + halfLen] * wRe;

                re[i + j] = uRe + vRe;
                im[i + j] = uIm + vIm;
                re[i + j + halfLen] = uRe - vRe;
                im[i + j + halfLen] = uIm - vIm;
            }
        }
    }
}

static void EqGetCustomPresetStorageKey(int presetIndex, int band, wchar_t* buffer, size_t bufferCount) {
    swprintf_s(buffer, bufferCount, L"customEqPreset%dBand%d", presetIndex + 1, band);
}

static void EqSaveCustomPresetCount() {
    Wh_SetIntValue(L"customEqPresetCount", g_eqCustomPresetCount);
}

static void EqSaveCustomPreset(int presetIndex) {
    if (presetIndex < 0 || presetIndex >= g_eqCustomPresetCount)
        return;
    for (int band = 0; band < VIZ_EQ_BANDS; ++band) {
        wchar_t key[64]{};
        EqGetCustomPresetStorageKey(presetIndex, band, key, 64);
        const float gain = std::clamp(
            g_eqCustomPresetGains[static_cast<size_t>(presetIndex)][static_cast<size_t>(band)].load(
                std::memory_order_relaxed), 0.0f, 2.0f);
        Wh_SetIntValue(key, static_cast<int>(std::lround(gain * 100.0f)));
    }
}

static void EqSaveAllCustomPresets() {
    EqSaveCustomPresetCount();
    for (int i = 0; i < g_eqCustomPresetCount; ++i)
        EqSaveCustomPreset(i);
}

static void LoadCustomEQSettings() {
    // Windhawk calls LoadSettings() again when the user changes any mod
    // setting. Do not reload the live EQ curve from storage on those calls,
    // otherwise a normal settings change would silently revert the current
    // EQ values. The storage is loaded once per module lifetime instead.
    if (g_eqStorageInitialized)
        return;

    // Load the current EQ curve independently from preset storage. Newer
    // versions use eqActiveBand0..9; older installations only have the
    // legacy customEqBand0..9 values, so fall back to those when needed.
    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const int activeStored = Wh_GetIntValue(
            VIZ_EQ_ACTIVE_STORAGE_KEYS[static_cast<size_t>(i)], -1);
        const int stored = std::clamp(
            activeStored >= 0
                ? activeStored
                : Wh_GetIntValue(
                    VIZ_EQ_STORAGE_KEYS[static_cast<size_t>(i)], 100),
            0, 200);
        const float gain = stored / 100.0f;
        g_customEqGains[static_cast<size_t>(i)].store(gain, std::memory_order_relaxed);
        g_eqActiveGains[static_cast<size_t>(i)].store(gain, std::memory_order_relaxed);
    }

    g_eqCustomPresetCount = std::clamp(
        Wh_GetIntValue(L"customEqPresetCount", 0), 0, VIZ_EQ_MAX_CUSTOM_PRESETS);
    for (int preset = 0; preset < VIZ_EQ_MAX_CUSTOM_PRESETS; ++preset) {
        for (int band = 0; band < VIZ_EQ_BANDS; ++band) {
            wchar_t key[64]{};
            EqGetCustomPresetStorageKey(preset, band, key, 64);
            const int stored = std::clamp(Wh_GetIntValue(key, 100), 0, 200);
            g_eqCustomPresetGains[static_cast<size_t>(preset)][static_cast<size_t>(band)].store(
                stored / 100.0f, std::memory_order_relaxed);
        }
    }

    const int savedPreset = Wh_GetIntValue(
        VIZ_EQ_SELECTED_PRESET_STORAGE_KEY, -1);
    if (savedPreset >= 0 && savedPreset < VIZ_EQ_BUILTIN_PRESET_COUNT) {
        g_eqSelectedPreset = savedPreset;
    } else if (savedPreset >= EQ_CUSTOM_PRESET_INDEX_BASE &&
               savedPreset < EQ_CUSTOM_PRESET_INDEX_BASE + g_eqCustomPresetCount) {
        g_eqSelectedPreset = savedPreset;
    } else {
        g_eqSelectedPreset = -1;
    }

    g_eqStorageInitialized = true;
}

static void SaveCustomEQSettings() {
    // Always persist the currently visible EQ curve, regardless of whether a
    // built-in preset, a custom preset, or no preset is selected. This keeps
    // the actual slider values stable across settings changes, recompiles,
    // Explorer restarts, and full PC reboots.
    for (int band = 0; band < VIZ_EQ_BANDS; ++band) {
        const float gain = std::clamp(
            g_eqActiveGains[static_cast<size_t>(band)].load(std::memory_order_relaxed),
            0.0f, 2.0f);
        g_customEqGains[static_cast<size_t>(band)].store(gain, std::memory_order_relaxed);
        const int stored = static_cast<int>(std::lround(gain * 100.0f));

        // New persistent active-curve storage.
        Wh_SetIntValue(
            VIZ_EQ_ACTIVE_STORAGE_KEYS[static_cast<size_t>(band)], stored);

        // Keep the legacy keys mirrored so older versions can still restore
        // the last active curve if the user downgrades the mod.
        Wh_SetIntValue(
            VIZ_EQ_STORAGE_KEYS[static_cast<size_t>(band)], stored);
    }

    // A custom preset represents the currently editable curve. Keep the
    // selected preset synchronized with the actual slider values as before.
    if (g_eqSelectedPreset >= EQ_CUSTOM_PRESET_INDEX_BASE) {
        const int presetIndex = g_eqSelectedPreset - EQ_CUSTOM_PRESET_INDEX_BASE;
        if (presetIndex >= 0 && presetIndex < g_eqCustomPresetCount) {
            for (int band = 0; band < VIZ_EQ_BANDS; ++band) {
                g_eqCustomPresetGains[static_cast<size_t>(presetIndex)][static_cast<size_t>(band)].store(
                    std::clamp(
                        g_eqActiveGains[static_cast<size_t>(band)].load(std::memory_order_relaxed),
                        0.0f, 2.0f),
                    std::memory_order_relaxed);
            }
            EqSaveCustomPreset(presetIndex);
        }
    }

    Wh_SetIntValue(VIZ_EQ_SELECTED_PRESET_STORAGE_KEY, g_eqSelectedPreset);
}

struct VizEqBandMap {
    int a = 0;
    int b = 0;
    float t = 0.0f;
};

static void BuildVizEqBandMaps(std::array<VizEqBandMap, VIZ_NUM_BANDS>& maps) {
    constexpr float kMinHz = 20.0f;
    constexpr float kMaxHz = 20000.0f;

    for (int band = 0; band < VIZ_NUM_BANDS; ++band) {
        const float bandT = (static_cast<float>(band) + 0.5f) /
            static_cast<float>(VIZ_NUM_BANDS);
        const float frequency = kMinHz *
            powf(kMaxHz / kMinHz, bandT);

        float previousLog = logf(std::max(kMinHz, VIZ_EQ_LOW_HZ[0]));
        bool mapped = false;

        for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
            const float lowHz = VIZ_EQ_LOW_HZ[static_cast<size_t>(i)];
            const float highHz = VIZ_EQ_HIGH_HZ[static_cast<size_t>(i)];
            const float lowLog = logf(lowHz);
            const float highLog = logf(highHz);

            if (frequency <= lowHz) {
                if (i == 0) {
                    maps[static_cast<size_t>(band)] = {0, 0, 0.0f};
                } else {
                    const float t = std::clamp(
                        (logf(frequency) - previousLog) /
                            std::max(0.0001f, lowLog - previousLog),
                        0.0f, 1.0f);
                    maps[static_cast<size_t>(band)] = {i - 1, i, t};
                }
                mapped = true;
                break;
            }

            if (frequency <= highHz) {
                const float lowT = std::clamp(
                    (logf(frequency) - lowLog) /
                        std::max(0.0001f, highLog - lowLog),
                    0.0f, 1.0f);
                const float blend =
                    lowT < 0.5f ? 0.0f : (lowT - 0.5f) * 2.0f;
                const int next = (i + 1 < VIZ_EQ_BANDS) ? i + 1 : i;
                maps[static_cast<size_t>(band)] = {i, next, blend};
                mapped = true;
                break;
            }

            previousLog = highLog;
        }

        if (!mapped) {
            maps[static_cast<size_t>(band)] =
                {VIZ_EQ_BANDS - 1, VIZ_EQ_BANDS - 1, 0.0f};
        }
    }
}

static void GetVizEQMultipliers(float out[VIZ_NUM_BANDS]) {
    static const std::array<VizEqBandMap, VIZ_NUM_BANDS> maps = [] {
        std::array<VizEqBandMap, VIZ_NUM_BANDS> result{};
        BuildVizEqBandMaps(result);
        return result;
    }();

    // Snapshot the ten controls once per FFT instead of repeatedly touching
    // atomics while rediscovering the same frequency mapping for every band.
    std::array<float, VIZ_EQ_BANDS> gains{};
    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        gains[static_cast<size_t>(i)] = std::clamp(
            g_eqActiveGains[static_cast<size_t>(i)].load(
                std::memory_order_relaxed),
            0.0f, 2.0f);
    }

    for (int band = 0; band < VIZ_NUM_BANDS; ++band) {
        const VizEqBandMap& map = maps[static_cast<size_t>(band)];
        const float a = gains[static_cast<size_t>(map.a)];
        const float b = gains[static_cast<size_t>(map.b)];
        out[band] = std::clamp(a + (b - a) * map.t, 0.0f, 2.0f);
    }
}

static float GetBandGravity(int band) {
    static const std::array<float, VIZ_NUM_BANDS> values = [] {
        std::array<float, VIZ_NUM_BANDS> result{};
        for (int i = 0; i < VIZ_NUM_BANDS; ++i) {
            const float t = (i + 0.5f) / static_cast<float>(VIZ_NUM_BANDS);
            result[static_cast<size_t>(i)] = 0.012f + t * 0.030f;
        }
        return result;
    }();
    return values[static_cast<size_t>(band)];
}

static float GetBandSensitivity(int band) {
    static const std::array<float, VIZ_NUM_BANDS> values = [] {
        std::array<float, VIZ_NUM_BANDS> result{};
        for (int i = 0; i < VIZ_NUM_BANDS; ++i) {
            const float t = (i + 0.5f) / static_cast<float>(VIZ_NUM_BANDS);
            result[static_cast<size_t>(i)] = 0.28f *
                powf(0.045f / 0.28f, t);
        }
        return result;
    }();
    return values[static_cast<size_t>(band)];
}

static void ClearAudioBands() {
    for (auto& band : g_audioBands)
        band.store(0.0f, std::memory_order_relaxed);
}

class AudioInterfaceActivationHandler final
    : public IActivateAudioInterfaceCompletionHandler,
      public IAgileObject {
private:
    std::atomic<ULONG> m_refCount{1};
    std::atomic<HANDLE> m_event{nullptr};
    IAudioClient* m_audioClient = nullptr;
    HRESULT m_result = E_FAIL;

public:
    explicit AudioInterfaceActivationHandler(HANDLE event)
        : m_event(event) {}

    ~AudioInterfaceActivationHandler() {
        if (m_audioClient)
            m_audioClient->Release();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid, void** ppvObject) override {
        if (!ppvObject)
            return E_POINTER;

        *ppvObject = nullptr;

        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IActivateAudioInterfaceCompletionHandler)) {
            *ppvObject = static_cast<IActivateAudioInterfaceCompletionHandler*>(this);
        } else if (riid == __uuidof(IAgileObject)) {
            *ppvObject = static_cast<IAgileObject*>(this);
        } else {
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return m_refCount.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG count = m_refCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (count == 0)
            delete this;
        return count;
    }

    HRESULT STDMETHODCALLTYPE ActivateCompleted(
        IActivateAudioInterfaceAsyncOperation* operation) override {
        m_result = E_UNEXPECTED;

        if (operation) {
            HRESULT activateResult = E_UNEXPECTED;
            IUnknown* unknown = nullptr;
            HRESULT hr = operation->GetActivateResult(&activateResult, &unknown);

            if (SUCCEEDED(hr) && SUCCEEDED(activateResult) && unknown) {
                hr = unknown->QueryInterface(
                    __uuidof(IAudioClient),
                    reinterpret_cast<void**>(&m_audioClient));
                unknown->Release();
                m_result = SUCCEEDED(hr) ? S_OK : hr;
            } else {
                if (unknown)
                    unknown->Release();
                m_result = FAILED(hr) ? hr : activateResult;
            }
        }

        HANDLE event = m_event.load(std::memory_order_acquire);
        if (event)
            SetEvent(event);

        return S_OK;
    }

    void DisableEvent() {
        m_event.store(nullptr, std::memory_order_release);
    }

    HRESULT Result() const {
        return m_result;
    }

    IAudioClient* TakeAudioClient() {
        IAudioClient* client = m_audioClient;
        m_audioClient = nullptr;
        return client;
    }
};

static bool EqualsExecutableName(const wchar_t* currentName,
                                  const std::wstring& wanted) {
    if (!currentName)
        return false;

    std::wstring current = currentName;
    for (wchar_t& ch : current)
        ch = static_cast<wchar_t>(towlower(ch));

    return current == wanted;
}

static std::vector<std::wstring> SplitExecutableList(const std::wstring& input) {
    std::vector<std::wstring> result;
    std::wstring token;

    auto flushToken = [&]() {
        size_t begin = 0;
        while (begin < token.size() && iswspace(token[begin]))
            ++begin;

        size_t end = token.size();
        while (end > begin && iswspace(token[end - 1]))
            --end;

        if (end > begin) {
            std::wstring normalized = token.substr(begin, end - begin);
            for (wchar_t& ch : normalized)
                ch = static_cast<wchar_t>(towlower(ch));
            result.push_back(std::move(normalized));
        }
        token.clear();
    };

    for (wchar_t ch : input) {
        if (ch == L',' || ch == L';' || ch == L'/' || ch == L'\\' ||
            ch == L'|' || ch == L'+' || iswspace(ch)) {
            flushToken();
        } else {
            token.push_back(ch);
        }
    }

    flushToken();
    return result;
}

static bool ExecutableListContains(const std::vector<std::wstring>& executableNames,
                                   const std::wstring& executableName) {
    std::wstring wanted = executableName;
    for (wchar_t& ch : wanted)
        ch = static_cast<wchar_t>(towlower(ch));

    for (const std::wstring& name : executableNames) {
        if (name == wanted)
            return true;
    }
    return false;
}


static bool ProcessExecutableNameMatches(
    DWORD processId, const std::vector<std::wstring>& executableNames) {
    if (!processId || executableNames.empty())
        return false;

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process)
        return false;

    wchar_t exePath[MAX_PATH] = {};
    DWORD pathLen = ARRAYSIZE(exePath);
    bool matches = false;

    if (QueryFullProcessImageNameW(process, 0, exePath, &pathLen)) {
        const wchar_t* base = wcsrchr(exePath, L'\\');
        base = base ? base + 1 : exePath;
        matches = ExecutableListContains(executableNames, base);
    }

    CloseHandle(process);
    return matches;
}

enum class AudioSampleFormat {
    Float32,
    Int16,
    Int32,
};

static bool GetAudioSampleFormat(
    const WAVEFORMATEX* format, AudioSampleFormat* outFormat) {
    if (!format || !outFormat)
        return false;

    if (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT &&
        format->wBitsPerSample == 32) {
        *outFormat = AudioSampleFormat::Float32;
        return true;
    }

    if (format->wFormatTag == WAVE_FORMAT_PCM) {
        if (format->wBitsPerSample == 16) {
            *outFormat = AudioSampleFormat::Int16;
            return true;
        }
        if (format->wBitsPerSample == 32) {
            *outFormat = AudioSampleFormat::Int32;
            return true;
        }
    }

    if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
        format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)) {
        const auto* extensible =
            reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(format);

        if (extensible->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT &&
            format->wBitsPerSample == 32) {
            *outFormat = AudioSampleFormat::Float32;
            return true;
        }

        if (extensible->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
            if (format->wBitsPerSample == 16) {
                *outFormat = AudioSampleFormat::Int16;
                return true;
            }
            if (format->wBitsPerSample == 32) {
                *outFormat = AudioSampleFormat::Int32;
                return true;
            }
        }
    }

    return false;
}

static float ReadAudioMonoSample(
    const BYTE* data,
    UINT32 frameIndex,
    UINT32 channels,
    AudioSampleFormat format) {

    if (!data || channels == 0)
        return 0.0f;

    float mono = 0.0f;

    switch (format) {
    case AudioSampleFormat::Float32: {
        const float* src = reinterpret_cast<const float*>(data);
        for (UINT32 c = 0; c < channels; ++c)
            mono += src[frameIndex * channels + c];
        break;
    }

    case AudioSampleFormat::Int16: {
        const INT16* src = reinterpret_cast<const INT16*>(data);
        for (UINT32 c = 0; c < channels; ++c)
            mono += src[frameIndex * channels + c] / 32768.0f;
        break;
    }

    case AudioSampleFormat::Int32: {
        const INT32* src = reinterpret_cast<const INT32*>(data);
        for (UINT32 c = 0; c < channels; ++c)
            mono += src[frameIndex * channels + c] / 2147483648.0f;
        break;
    }
    }

    return mono / static_cast<float>(channels);
}

static bool InitSystemAudioClient(
    IMMDeviceEnumerator* enumerator,
    IAudioClient** outClient,
    IAudioCaptureClient** outCapture,
    UINT32* outSampleRate,
    UINT32* outChannels,
    AudioSampleFormat* outSampleFormat) {

    if (!enumerator || !outClient || !outCapture ||
        !outSampleRate || !outChannels || !outSampleFormat) {
        return false;
    }

    *outClient = nullptr;
    *outCapture = nullptr;

    IMMDevice* device = nullptr;
    HRESULT hr = enumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &device);
    if (FAILED(hr))
        return false;

    IAudioClient* client = nullptr;
    hr = device->Activate(
        __uuidof(IAudioClient),
        CLSCTX_ALL,
        nullptr,
        reinterpret_cast<void**>(&client));
    device->Release();

    if (FAILED(hr) || !client)
        return false;

    WAVEFORMATEX* format = nullptr;
    hr = client->GetMixFormat(&format);
    if (FAILED(hr) || !format) {
        client->Release();
        return false;
    }

    *outSampleRate = format->nSamplesPerSec;
    *outChannels = std::max<UINT32>(1, format->nChannels);
    if (!GetAudioSampleFormat(format, outSampleFormat)) {
        Wh_Log(L"Unsupported system mix format: tag=%u bits=%u channels=%u",
               static_cast<unsigned>(format->wFormatTag),
               static_cast<unsigned>(format->wBitsPerSample),
               static_cast<unsigned>(format->nChannels));
        CoTaskMemFree(format);
        client->Release();
        return false;
    }

    hr = client->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        200000,
        0,
        format,
        nullptr);

    CoTaskMemFree(format);

    if (FAILED(hr)) {
        client->Release();
        return false;
    }

    if (g_hAudioEvent) {
        hr = client->SetEventHandle(g_hAudioEvent);
        if (FAILED(hr)) {
            client->Release();
            return false;
        }
    }

    IAudioCaptureClient* capture = nullptr;
    hr = client->GetService(
        __uuidof(IAudioCaptureClient),
        reinterpret_cast<void**>(&capture));

    if (FAILED(hr) || !capture) {
        client->Release();
        return false;
    }

    hr = client->Start();
    if (FAILED(hr)) {
        capture->Release();
        client->Release();
        return false;
    }

    *outClient = client;
    *outCapture = capture;
    return true;
}

static bool InitProcessAudioClient(
    DWORD processId,
    IAudioClient** outClient,
    IAudioCaptureClient** outCapture,
    UINT32* outSampleRate,
    UINT32* outChannels,
    AudioSampleFormat* outSampleFormat) {

    if (!processId || !outClient || !outCapture ||
        !outSampleRate || !outChannels || !outSampleFormat) {
        return false;
    }

    *outClient = nullptr;
    *outCapture = nullptr;

    HANDLE completedEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!completedEvent)
        return false;

    AudioInterfaceActivationHandler* handler =
        new AudioInterfaceActivationHandler(completedEvent);
    WH_AUDIOCLIENT_ACTIVATION_PARAMS activationParams{};
    activationParams.ActivationType = WH_AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK;
    activationParams.ProcessLoopbackParams.TargetProcessId = processId;
    activationParams.ProcessLoopbackParams.ProcessLoopbackMode =
        WH_PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE;

    PROPVARIANT activateParams{};
    activateParams.vt = VT_BLOB;
    activateParams.blob.cbSize = sizeof(activationParams);
    activateParams.blob.pBlobData = reinterpret_cast<BYTE*>(&activationParams);

    IActivateAudioInterfaceAsyncOperation* asyncOperation = nullptr;
    HRESULT hr = ActivateAudioInterfaceAsync(
        VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK,
        __uuidof(IAudioClient),
        &activateParams,
        handler,
        &asyncOperation);

    if (FAILED(hr))
        Wh_Log(L"Process loopback activation call failed: 0x%08X (pid=%lu)",
               static_cast<unsigned>(hr), static_cast<unsigned long>(processId));

    if (asyncOperation)
        asyncOperation->Release();

    if (FAILED(hr)) {
        handler->DisableEvent();
        handler->Release();
        CloseHandle(completedEvent);
        return false;
    }

    // The async activation holds a COM reference to the handler until the
    // completion callback has finished. Do not abandon the handler when the
    // audio worker is stopping: unloading the mod while the callback is still
    // pending would leave its vtable/code in an unloaded module.
    constexpr DWORD kActivationWaitTimeoutMs = 10000;
    const DWORD waitResult =
        WaitForSingleObject(completedEvent, kActivationWaitTimeoutMs);

    if (waitResult == WAIT_TIMEOUT) {
        // Activation is expected to complete. The timeout is only a diagnostic
        // guard; it is not safe to release the handler while the callback is
        // still pending, so finish waiting before touching its lifetime.
        Wh_Log(L"Process loopback activation is taking longer than expected; waiting for completion (pid=%lu)",
               static_cast<unsigned long>(processId));

        if (WaitForSingleObject(completedEvent, INFINITE) != WAIT_OBJECT_0) {
            Wh_Log(L"Process loopback activation completion wait failed (pid=%lu)",
                   static_cast<unsigned long>(processId));
            handler->DisableEvent();
            handler->Release();
            CloseHandle(completedEvent);
            return false;
        }
    } else if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"Process loopback activation completion wait failed (pid=%lu)",
               static_cast<unsigned long>(processId));
        handler->DisableEvent();
        handler->Release();
        CloseHandle(completedEvent);
        return false;
    }

    const HRESULT activationResult = handler->Result();
    if (FAILED(activationResult)) {
        Wh_Log(L"Process loopback activation result failed: 0x%08X (pid=%lu)",
               static_cast<unsigned>(activationResult),
               static_cast<unsigned long>(processId));
        handler->DisableEvent();
        handler->Release();
        CloseHandle(completedEvent);
        return false;
    }

    IAudioClient* client = handler->TakeAudioClient();
    handler->DisableEvent();
    handler->Release();
    CloseHandle(completedEvent);

    if (!client) {
        Wh_Log(L"Process loopback activation returned no IAudioClient (pid=%lu)",
               static_cast<unsigned long>(processId));
        return false;
    }

    WAVEFORMATEX format{};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = 48000;
    format.wBitsPerSample = 16;
    format.nBlockAlign = static_cast<WORD>(format.nChannels * format.wBitsPerSample / 8);
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize = 0;

    hr = client->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK |
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK |
            AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
        200000,
        0,
        &format,
        nullptr);

    if (FAILED(hr)) {
        Wh_Log(L"Process loopback IAudioClient::Initialize failed: 0x%08X (pid=%lu)",
               static_cast<unsigned>(hr), static_cast<unsigned long>(processId));
        client->Release();
        return false;
    }

    if (g_hAudioEvent) {
        hr = client->SetEventHandle(g_hAudioEvent);
        if (FAILED(hr)) {
            client->Release();
            return false;
        }
    }

    IAudioCaptureClient* capture = nullptr;
    hr = client->GetService(
        __uuidof(IAudioCaptureClient),
        reinterpret_cast<void**>(&capture));

    if (FAILED(hr) || !capture) {
        client->Release();
        return false;
    }

    hr = client->Start();
    if (FAILED(hr)) {
        capture->Release();
        client->Release();
        return false;
    }

    *outSampleRate = format.nSamplesPerSec;
    *outChannels = format.nChannels;
    *outSampleFormat = AudioSampleFormat::Int16;
    *outClient = client;
    *outCapture = capture;
    return true;
}

static bool InitAudioClient(
    IMMDeviceEnumerator* enumerator,
    const VisualizerSettings& settings,
    IAudioClient** outClient,
    IAudioCaptureClient** outCapture,
    UINT32* outSampleRate,
    UINT32* outChannels,
    AudioSampleFormat* outSampleFormat) {

    if (settings.audioSource == 0) {
        return InitSystemAudioClient(
            enumerator,
            outClient,
            outCapture,
            outSampleRate,
            outChannels,
            outSampleFormat);
    }

    return false;
}

static void PublishAudioBands(const float bands[VIZ_NUM_BANDS]) {
    float peak = 0.0f;
    for (int i = 0; i < VIZ_NUM_BANDS; ++i) {
        g_audioBands[i].store(bands[i], std::memory_order_relaxed);
        peak = std::max(peak, bands[i]);
    }

    g_lastAudioUpdateMs.store(GetTickCount64(), std::memory_order_release);

    if (peak > 0.025f &&
        g_overlayIdle.exchange(false, std::memory_order_acq_rel)) {
        HWND overlay = g_overlayWakeHwnd.load(std::memory_order_acquire);
        if (overlay)
            PostMessageW(overlay, WM_VIZ_AUDIO_WAKE, 0, 0);
    }
}

static std::vector<DWORD> FindAudioProcessIdsByExecutable(
    IMMDeviceEnumerator* enumerator,
    const std::wstring& executableList) {
    const std::vector<std::wstring> executableNames = SplitExecutableList(executableList);
    std::vector<DWORD> result;
    if (executableNames.empty())
        return result;

    auto findNameIndex = [&](const wchar_t* currentName) -> int {
        if (!currentName)
            return -1;
        std::wstring current = currentName;
        for (wchar_t& ch : current)
            ch = static_cast<wchar_t>(towlower(ch));
        for (size_t i = 0; i < executableNames.size(); ++i) {
            if (executableNames[i] == current)
                return static_cast<int>(i);
        }
        return -1;
    };

    // One target PID per configured executable is intentional. Process-loopback in
    // INCLUDE_TARGET_PROCESS_TREE mode captures the target and its child processes,
    // so selecting every helper/content PID would duplicate the same application's audio.
    std::vector<DWORD> bestPid(executableNames.size(), 0);
    std::vector<bool> bestPidActive(executableNames.size(), false);

    if (enumerator) {
        IMMDevice* device = nullptr;
        HRESULT hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        if (SUCCEEDED(hr) && device) {
            IAudioSessionManager2* sessionManager = nullptr;
            hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
                                  reinterpret_cast<void**>(&sessionManager));
            if (SUCCEEDED(hr) && sessionManager) {
                IAudioSessionEnumerator* sessionEnumerator = nullptr;
                hr = sessionManager->GetSessionEnumerator(&sessionEnumerator);
                if (SUCCEEDED(hr) && sessionEnumerator) {
                    int sessionCount = 0;
                    if (SUCCEEDED(sessionEnumerator->GetCount(&sessionCount))) {
                        for (int i = 0; i < sessionCount; ++i) {
                            IAudioSessionControl* control = nullptr;
                            if (FAILED(sessionEnumerator->GetSession(i, &control)) || !control)
                                continue;

                            IAudioSessionControl2* control2 = nullptr;
                            if (SUCCEEDED(control->QueryInterface(
                                    __uuidof(IAudioSessionControl2),
                                    reinterpret_cast<void**>(&control2))) && control2) {
                                DWORD pid = 0;
                                if (SUCCEEDED(control2->GetProcessId(&pid)) && pid) {
                                    HANDLE process = OpenProcess(
                                        PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                                    if (process) {
                                        wchar_t exePath[MAX_PATH] = {};
                                        DWORD pathLen = ARRAYSIZE(exePath);
                                        if (QueryFullProcessImageNameW(
                                                process, 0, exePath, &pathLen)) {
                                            const wchar_t* base = wcsrchr(exePath, L'\\');
                                            base = base ? base + 1 : exePath;
                                            const int nameIndex = findNameIndex(base);
                                            if (nameIndex >= 0) {
                                                AudioSessionState state = AudioSessionStateInactive;
                                                const bool active =
                                                    SUCCEEDED(control->GetState(&state)) &&
                                                    state == AudioSessionStateActive;
                                                if (!bestPid[nameIndex] ||
                                                    (active && !bestPidActive[nameIndex])) {
                                                    bestPid[nameIndex] = pid;
                                                    bestPidActive[nameIndex] = active;
                                                }
                                            }
                                        }
                                        CloseHandle(process);
                                    }
                                }
                                control2->Release();
                            }
                            control->Release();
                        }
                    }
                    sessionEnumerator->Release();
                }
                sessionManager->Release();
            }
            device->Release();
        }
    }

    // If an application currently has no audio session, fall back to one live process
    // for that executable. Its process tree will be captured once audio starts.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W entry{};
        entry.dwSize = sizeof(entry);
        if (Process32FirstW(snapshot, &entry)) {
            do {
                const int nameIndex = findNameIndex(entry.szExeFile);
                if (nameIndex >= 0 && !bestPid[nameIndex])
                    bestPid[nameIndex] = entry.th32ProcessID;
            } while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }

    for (DWORD pid : bestPid) {
        if (pid)
            result.push_back(pid);
    }

    return result;
}

struct ProcessAudioSource {
    DWORD pid = 0;
    IAudioClient* client = nullptr;
    IAudioCaptureClient* capture = nullptr;
    UINT32 sampleRate = 48000;
    UINT32 channels = 2;
    AudioSampleFormat sampleFormat = AudioSampleFormat::Int16;
};


static void ReleaseProcessAudioSources(std::vector<ProcessAudioSource>& sources) {
    for (auto& source : sources) {
        if (source.client) {
            source.client->Stop();
            source.client->Release();
            source.client = nullptr;
        }
        if (source.capture) {
            source.capture->Release();
            source.capture = nullptr;
        }
    }
    sources.clear();
}

static bool BuildProcessAudioSources(
    const std::vector<DWORD>& pids,
    std::vector<ProcessAudioSource>& sources) {
    ReleaseProcessAudioSources(sources);

    UINT32 commonSampleRate = 0;
    UINT32 commonChannels = 0;
    AudioSampleFormat commonSampleFormat = AudioSampleFormat::Int16;

    for (DWORD pid : pids) {
        ProcessAudioSource source{};
        source.pid = pid;
        if (!InitProcessAudioClient(
                pid,
                &source.client,
                &source.capture,
                &source.sampleRate,
                &source.channels,
                &source.sampleFormat)) {
            Wh_Log(L"Failed to initialize selected process audio source (pid=%lu)",
                   static_cast<unsigned long>(pid));
            continue;
        }

        if (commonSampleRate == 0) {
            commonSampleRate = source.sampleRate;
            commonChannels = source.channels;
            commonSampleFormat = source.sampleFormat;
        }

        // Selected process-loopback sources are expected to use the same mix format.
        // If a source reports a different format, keep it out rather than mixing
        // incompatible sample streams.
        if (source.sampleRate != commonSampleRate ||
            source.channels != commonChannels ||
            source.sampleFormat != commonSampleFormat) {
            Wh_Log(L"Skipping selected process PID=%lu because its audio format differs from the first source",
                   static_cast<unsigned long>(pid));
            source.client->Stop();
            source.client->Release();
            source.client = nullptr;
            source.capture->Release();
            source.capture = nullptr;
            continue;
        }

        sources.push_back(source);
    }

    return !sources.empty();
}

static bool SamePidList(
    const std::vector<DWORD>& a,
    const std::vector<DWORD>& b) {
    if (a.size() != b.size())
        return false;
    return std::equal(a.begin(), a.end(), b.begin());
}

static DWORD WINAPI AudioCaptureThreadProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    BuildHannWindow();
    BuildTwiddleFactors();

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&enumerator));

    if (FAILED(hr) || !enumerator) {
        g_audioRunning.store(false, std::memory_order_release);
        CoUninitialize();
        return 0;
    }

    IAudioClient* client = nullptr;
    IAudioCaptureClient* capture = nullptr;
    UINT32 sampleRate = 48000;
    UINT32 channels = 2;
    AudioSampleFormat sampleFormat = AudioSampleFormat::Float32;

    std::vector<ProcessAudioSource> processSources;
    std::vector<DWORD> targetProcessIds;
    std::vector<HANDLE> targetProcessWatchHandles;
    std::vector<std::vector<float>> sourceFrames;
    ULONGLONG lastTargetCheckMs = 0;
    DWORD targetCheckIntervalMs = 750;
    std::wstring lastTargetExecutableList;
    bool previousSelectedMode = GetSettingsSnapshot().audioSource == 1;

    static constexpr int RING_CAP = VIZ_FFT_SIZE * 4;
    std::vector<float> ringBuf(RING_CAP, 0.0f);
    int ringHead = 0;
    int ringCount = 0;

    std::vector<float> re(VIZ_FFT_SIZE, 0.0f);
    std::vector<float> im(VIZ_FFT_SIZE, 0.0f);

    // In selected-app mode multiple process-loopback clients can occasionally
    // expose different amounts of buffered audio in one capture pass. Do not
    // let that backlog make the FFT/visualizer run several audio hops at once.
    // The visualizer must advance at the real-time FFT hop cadence instead.
    ULONGLONG nextSelectedFftMs = 0;

    float bandEnv[VIZ_NUM_BANDS] = {};
    float autoGain = 1.0f;
    float smoothMax = 0.01f;

    auto clearAudioState = [&]() {
        std::fill(ringBuf.begin(), ringBuf.end(), 0.0f);
        ringHead = 0;
        ringCount = 0;
        nextSelectedFftMs = 0;
        for (float& value : bandEnv)
            value = 0.0f;
        ClearAudioBands();
    };

    auto releaseSingleSource = [&]() {
        if (client) {
            client->Stop();
            client->Release();
            client = nullptr;
        }
        if (capture) {
            capture->Release();
            capture = nullptr;
        }
    };

    // Keep a lightweight synchronization handle for each selected process.
    // This lets us notice process termination immediately, so the relatively
    // expensive audio-session/process enumeration can back off when the PID
    // list remains stable without making process restarts slow to detect.
    auto rebuildProcessWatchHandles = [&]() {
        for (HANDLE handle : targetProcessWatchHandles) {
            if (handle)
                CloseHandle(handle);
        }
        targetProcessWatchHandles.clear();
        targetProcessWatchHandles.reserve(targetProcessIds.size());

        for (DWORD pid : targetProcessIds) {
            HANDLE handle = OpenProcess(SYNCHRONIZE, FALSE, pid);
            targetProcessWatchHandles.push_back(handle);
        }
    };

    auto anyWatchedProcessExited = [&]() -> bool {
        for (HANDLE handle : targetProcessWatchHandles) {
            if (!handle)
                continue;
            const DWORD waitResult = WaitForSingleObject(handle, 0);
            if (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_FAILED)
                return true;
        }
        return false;
    };

    const VisualizerSettings initialSettings = GetSettingsSnapshot();
    if (initialSettings.audioSource == 1) {
        targetProcessIds = FindAudioProcessIdsByExecutable(
            enumerator, initialSettings.audioApplicationName);
        lastTargetExecutableList = initialSettings.audioApplicationName;
        rebuildProcessWatchHandles();
        if (BuildProcessAudioSources(targetProcessIds, processSources) &&
            !processSources.empty()) {
            sampleRate = processSources.front().sampleRate;
            channels = processSources.front().channels;
            sampleFormat = processSources.front().sampleFormat;
            BuildLogBins(sampleRate);
        }
    } else if (InitSystemAudioClient(
                   enumerator,
                   &client,
                   &capture,
                   &sampleRate,
                   &channels,
                   &sampleFormat)) {
        BuildLogBins(sampleRate);
    }

    while (g_audioRunning.load(std::memory_order_acquire)) {
        const VisualizerSettings settings = GetSettingsSnapshot();
        const bool selectedMode = settings.audioSource == 1;

        // Handle a live switch between Whole system and Selected applications
        // without carrying audio buffers or media from the previous source.
        if (selectedMode != previousSelectedMode) {
            ReleaseProcessAudioSources(processSources);
            targetProcessIds.clear();
            for (HANDLE handle : targetProcessWatchHandles) {
                if (handle)
                    CloseHandle(handle);
            }
            targetProcessWatchHandles.clear();
            releaseSingleSource();
            clearAudioState();
            lastTargetCheckMs = 0;
            targetCheckIntervalMs = 750;
            lastTargetExecutableList.clear();
            previousSelectedMode = selectedMode;
        }

        if (selectedMode) {
            const ULONGLONG now = GetTickCount64();
            const bool executableListChanged =
                settings.audioApplicationName != lastTargetExecutableList;
            const bool watchedProcessExited = anyWatchedProcessExited();
            const bool sourceSetIncomplete =
                processSources.size() != targetProcessIds.size();
            const bool needsTargetCheck =
                executableListChanged ||
                watchedProcessExited ||
                now - lastTargetCheckMs >= targetCheckIntervalMs;

            if (needsTargetCheck) {
                lastTargetCheckMs = now;

                std::vector<DWORD> newTargetProcessIds =
                    FindAudioProcessIdsByExecutable(
                        enumerator, settings.audioApplicationName);

                const bool pidListChanged =
                    !SamePidList(newTargetProcessIds, targetProcessIds);

                if (executableListChanged || pidListChanged || sourceSetIncomplete ||
                    watchedProcessExited) {
                    const bool logTargetChange =
                        executableListChanged || pidListChanged || watchedProcessExited;

                    ReleaseProcessAudioSources(processSources);
                    targetProcessIds = std::move(newTargetProcessIds);
                    rebuildProcessWatchHandles();
                    clearAudioState();

                    if (BuildProcessAudioSources(
                            targetProcessIds, processSources) &&
                        !processSources.empty()) {
                        sampleRate = processSources.front().sampleRate;
                        channels = processSources.front().channels;
                        sampleFormat = processSources.front().sampleFormat;
                        BuildLogBins(sampleRate);
                    }

                    if (logTargetChange) {
                        Wh_Log(L"Audio target changed: '%s' -> %zu target process(es)",
                               settings.audioApplicationName.c_str(),
                               targetProcessIds.size());
                    }

                    targetCheckIntervalMs = targetProcessIds.empty()
                                                ? 1500
                                                : (sourceSetIncomplete ? 1500 : 750);
                } else if (!newTargetProcessIds.empty()) {
                    // Stable target: exponentially back off discovery. Process
                    // exit is still detected immediately through the watcher
                    // handles above, so this only delays discovery of a newly
                    // appearing/reselected PID while the current target lives.
                    targetCheckIntervalMs =
                        std::min<DWORD>(targetCheckIntervalMs * 2, 3000);
                } else {
                    targetCheckIntervalMs = 1500;
                }

                lastTargetExecutableList = settings.audioApplicationName;
            }

            if (g_hAudioEvent)
                WaitForSingleObject(g_hAudioEvent, 20);
            else
                Sleep(8);

            if (processSources.empty()) {
                clearAudioState();
                Sleep(50);
                continue;
            }

            sourceFrames.resize(processSources.size());
            for (auto& frames : sourceFrames)
                frames.clear();
            bool sourceInvalidated = false;

            for (size_t sourceIndex = 0;
                 sourceIndex < processSources.size();
                 ++sourceIndex) {
                auto& source = processSources[sourceIndex];
                auto& frames = sourceFrames[sourceIndex];

                UINT32 packetSize = 0;
                hr = source.capture->GetNextPacketSize(&packetSize);
                if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                    sourceInvalidated = true;
                    break;
                }
                if (FAILED(hr))
                    continue;

                while (packetSize > 0 &&
                       g_audioRunning.load(std::memory_order_acquire)) {
                    BYTE* data = nullptr;
                    UINT32 numFrames = 0;
                    DWORD flags = 0;

                    hr = source.capture->GetBuffer(
                        &data, &numFrames, &flags, nullptr, nullptr);
                    if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                        sourceInvalidated = true;
                        break;
                    }
                    if (FAILED(hr))
                        break;

                    if (numFrames > 0) {
                        frames.reserve(frames.size() + numFrames);

                        if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                            frames.insert(frames.end(), numFrames, 0.0f);
                        } else if (data && source.channels > 0) {
                            for (UINT32 f = 0; f < numFrames; ++f) {
                                frames.push_back(ReadAudioMonoSample(
                                    data, f, source.channels, source.sampleFormat));
                            }
                        }
                    }

                    source.capture->ReleaseBuffer(numFrames);
                    hr = source.capture->GetNextPacketSize(&packetSize);
                    if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                        sourceInvalidated = true;
                        break;
                    }
                    if (FAILED(hr))
                        break;
                }

                if (sourceInvalidated)
                    break;
            }

            if (sourceInvalidated) {
                ReleaseProcessAudioSources(processSources);
                targetProcessIds.clear();
                for (HANDLE handle : targetProcessWatchHandles) {
                    if (handle)
                        CloseHandle(handle);
                }
                targetProcessWatchHandles.clear();
                clearAudioState();
                lastTargetCheckMs = 0;
                targetCheckIntervalMs = 750;
                continue;
            }

            size_t maxFrames = 0;
            for (const auto& frames : sourceFrames)
                maxFrames = std::max(maxFrames, frames.size());

            if (maxFrames > 0) {
                // Mix all selected executables together. Each configured executable
                // contributes independently, so X.exe + Y.exe means X AND Y.
                for (size_t f = 0; f < maxFrames; ++f) {
                    float mixed = 0.0f;
                    size_t contributingSources = 0;

                    for (const auto& frames : sourceFrames) {
                        if (f < frames.size()) {
                            mixed += frames[f];
                            ++contributingSources;
                        }
                    }

                    if (contributingSources > 0)
                        mixed /= static_cast<float>(contributingSources);

                    ringBuf[ringHead] = mixed;
                    ringHead = (ringHead + 1) % RING_CAP;
                    ringCount = std::min(ringCount + 1, RING_CAP);
                }
            }
        } else {
            if (!client || !capture) {
                releaseSingleSource();
                clearAudioState();

                if (InitSystemAudioClient(
                        enumerator,
                        &client,
                        &capture,
                        &sampleRate,
                        &channels,
                        &sampleFormat)) {
                    BuildLogBins(sampleRate);
                } else {
                    Sleep(50);
                    continue;
                }
            }

            if (g_hAudioEvent)
                WaitForSingleObject(g_hAudioEvent, 20);
            else
                Sleep(8);

            UINT32 packetSize = 0;
            hr = capture->GetNextPacketSize(&packetSize);
            if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                releaseSingleSource();
                clearAudioState();
                continue;
            }

            if (FAILED(hr)) {
                Sleep(5);
                continue;
            }

            while (packetSize > 0 &&
                   g_audioRunning.load(std::memory_order_acquire)) {
                BYTE* data = nullptr;
                UINT32 numFrames = 0;
                DWORD flags = 0;

                hr = capture->GetBuffer(
                    &data, &numFrames, &flags, nullptr, nullptr);
                if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                    releaseSingleSource();
                    clearAudioState();
                    break;
                }
                if (FAILED(hr))
                    break;

                if (numFrames > 0) {
                    if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                        for (UINT32 f = 0; f < numFrames; ++f) {
                            ringBuf[ringHead] = 0.0f;
                            ringHead = (ringHead + 1) % RING_CAP;
                            ringCount = std::min(ringCount + 1, RING_CAP);
                        }
                    } else if (data && channels > 0) {
                        for (UINT32 f = 0; f < numFrames; ++f) {
                            ringBuf[ringHead] = ReadAudioMonoSample(
                                data, f, channels, sampleFormat);
                            ringHead = (ringHead + 1) % RING_CAP;
                            ringCount = std::min(ringCount + 1, RING_CAP);
                        }
                    }
                }

                capture->ReleaseBuffer(numFrames);
                hr = capture->GetNextPacketSize(&packetSize);
                if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                    releaseSingleSource();
                    clearAudioState();
                    break;
                }
                if (FAILED(hr))
                    break;
            }
        }

        const float autoGainStrength =
            std::clamp(settings.autoGainStrength, 0, 100) / 100.0f;

        auto ProcessRawMagnitudes = [&](const float rawMags[VIZ_NUM_BANDS]) {
            float nextBands[VIZ_NUM_BANDS] = {};
            float frameMax = 0.0f;
            for (int b = 0; b < VIZ_NUM_BANDS; ++b)
                frameMax = std::max(frameMax, rawMags[b]);

            if (settings.autoGainEnabled && autoGainStrength > 0.0f) {
                if (frameMax > smoothMax)
                    smoothMax += (frameMax - smoothMax) * 0.5f;
                else
                    smoothMax += (frameMax - smoothMax) * 0.02f;

                smoothMax = std::max(0.0001f, smoothMax);

                float targetGain = 0.98f / smoothMax;
                targetGain = std::clamp(targetGain, 1.0f, 150.0f);
                targetGain = 1.0f + (targetGain - 1.0f) * autoGainStrength;

                if (targetGain > autoGain)
                    autoGain += (targetGain - autoGain) * 0.035f;
                else
                    autoGain += (targetGain - autoGain) * 0.15f;
            } else {
                autoGain = 1.0f;
                smoothMax = 0.01f;
            }

            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                float m = rawMags[b];

                if (settings.autoGainEnabled && smoothMax > 0.0001f) {
                    float ratio = std::clamp(m / smoothMax, 0.0f, 1.0f);
                    ratio = ratio * ratio * ratio;

                    const float appliedGain =
                        1.0f + (autoGain - 1.0f) * ratio;
                    m *= appliedGain;
                }

                m = std::clamp(m, 0.0f, 1.0f);
                bandEnv[b] =
                    (m >= bandEnv[b])
                        ? m
                        : std::max(0.0f, bandEnv[b] - GetBandGravity(b));
                nextBands[b] = bandEnv[b];
            }

            PublishAudioBands(nextBands);
        };

        const float tSens = settings.sensitivity / 100.0f;
        const float sliderGain =
            (tSens <= 1.0f)
                ? 0.25f + tSens * tSens * 2.75f
                : 3.0f + (tSens - 1.0f) * 4.0f;

        while (ringCount >= VIZ_FFT_SIZE &&
               g_audioRunning.load(std::memory_order_acquire)) {
            // A process-loopback source may have accumulated extra packets while
            // another selected source was being read. Processing every available
            // FFT immediately would make the visualizer advance faster than the
            // audio clock. In Selected applications mode, consume at most one
            // FFT hop per real-time interval.
            if (selectedMode) {
                const ULONGLONG nowFftMs = GetTickCount64();
                if (nowFftMs < nextSelectedFftMs)
                    break;
            }

            const int readStart =
                (ringHead - ringCount + RING_CAP) % RING_CAP;

            int ringIndex = readStart;
            for (int i = 0; i < VIZ_FFT_SIZE; ++i) {
                re[i] = ringBuf[ringIndex] * g_hannWindow[i];
                im[i] = 0.0f;
                if (++ringIndex == RING_CAP) {
                    ringIndex = 0;
                }
            }

            ringCount -= VIZ_FFT_SIZE / 2;
            VizFFT(re, im);

            float rawMags[VIZ_NUM_BANDS] = {};
            float eqMultipliers[VIZ_NUM_BANDS] = {};
            GetVizEQMultipliers(eqMultipliers);
            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                int bStart = g_logBinStart[b];
                int bEnd = g_logBinStart[b + 1];
                if (bEnd <= bStart)
                    bEnd = bStart + 1;

                float sumSq = 0.0f;
                int count = 0;
                for (int k = bStart;
                     k < bEnd && k < VIZ_FFT_SIZE / 2;
                     ++k) {
                    sumSq += re[k] * re[k] + im[k] * im[k];
                    ++count;
                }

                const float rms = count > 0
                    ? sqrtf(sumSq / static_cast<float>(count))
                    : 0.0f;
                const float eqM = eqMultipliers[b];
                rawMags[b] =
                    (rms / (VIZ_FFT_SIZE * 0.5f)) /
                    GetBandSensitivity(b) * sliderGain * eqM;
            }

            ProcessRawMagnitudes(rawMags);

            if (selectedMode) {
                // One FFT hop represents VIZ_FFT_SIZE/2 samples. Keep the
                // visualizer time scale tied to the actual sample rate rather
                // than to how many packets happened to be available.
                const double hopMs =
                    sampleRate > 0
                        ? (1000.0 * (VIZ_FFT_SIZE / 2.0)) / sampleRate
                        : 21.0;
                const ULONGLONG intervalMs =
                    static_cast<ULONGLONG>(std::max(1.0, std::ceil(hopMs)));
                nextSelectedFftMs = GetTickCount64() + intervalMs;
            }
        }
    }

    releaseSingleSource();
    ReleaseProcessAudioSources(processSources);
    for (HANDLE handle : targetProcessWatchHandles) {
        if (handle)
            CloseHandle(handle);
    }
    targetProcessWatchHandles.clear();
    enumerator->Release();
    ClearAudioBands();
    CoUninitialize();
    return 0;
}

static void StartAudioCapture() {
    if (g_audioRunning.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    if (!g_hAudioEvent)
        g_hAudioEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    if (!g_hAudioEvent) {
        g_audioRunning.store(false, std::memory_order_release);
        return;
    }

    g_hAudioThread = CreateThread(
        nullptr,
        0,
        AudioCaptureThreadProc,
        nullptr,
        0,
        nullptr);

    if (!g_hAudioThread) {
        g_audioRunning.store(false, std::memory_order_release);
        CloseHandle(g_hAudioEvent);
        g_hAudioEvent = nullptr;
    } else {
    }
}

static void StopAudioCapture() {
    if (!g_audioRunning.exchange(false, std::memory_order_acq_rel))
        return;

    if (g_hAudioEvent)
        SetEvent(g_hAudioEvent);

    if (g_hAudioThread) {
        WaitForSingleObject(g_hAudioThread, INFINITE);
        CloseHandle(g_hAudioThread);
        g_hAudioThread = nullptr;
    }

    if (g_hAudioEvent) {
        CloseHandle(g_hAudioEvent);
        g_hAudioEvent = nullptr;
    }

    ClearAudioBands();
    g_lastAudioUpdateMs.store(GetTickCount64(), std::memory_order_release);
}

static float g_cavaFall[VIZ_BANDS_MAX] = {};
static float g_cavaMem[VIZ_BANDS_MAX] = {};
static float g_cavaPeak[VIZ_BANDS_MAX] = {};
static float g_cavaPrevOut[VIZ_BANDS_MAX] = {};
static bool g_cavaWasEnabled = false;


// Converts a response amount defined for one 60 Hz update into an
// equivalent response for an arbitrary elapsed time. This keeps the same
// attack/decay time constant across 60/120/144/165/180 Hz and other timer
// cadences instead of making the animation react faster at higher FPS.
static float FrameRateIndependentResponse(float perFrameResponse, float dtFrames) {
    perFrameResponse = std::clamp(perFrameResponse, 0.0f, 1.0f);
    dtFrames = std::max(0.0f, dtFrames);

    if (dtFrames <= 0.0f || perFrameResponse <= 0.0f)
        return 0.0f;
    if (perFrameResponse >= 1.0f)
        return 1.0f;

    // Repeated application of
    //     x += (target - x) * perFrameResponse
    // over dtFrames is equivalent to this single step:
    //     alpha = 1 - (1 - perFrameResponse)^dtFrames
    // This is mathematically exact for a constant target between updates.
    return 1.0f - powf(1.0f - perFrameResponse, dtFrames);
}

static float ApplyCavaSmoothing(
    float rawValue,
    int index,
    float noiseReduction) {

    rawValue = std::clamp(rawValue, 0.0f, 1.0f);
    noiseReduction = std::clamp(noiseReduction, 0.0f, 100.0f);

    if (noiseReduction <= 0.1f) {
        g_cavaFall[index] = 0.0f;
        g_cavaPeak[index] = rawValue;
        g_cavaPrevOut[index] = rawValue;
        g_cavaMem[index] = rawValue;
        return rawValue;
    }


    constexpr float cavaFramerate = 60.0f;
    const float framerateMod = 66.0f / cavaFramerate;


    const float gravityMod =
        powf(framerateMod, 2.5f) * 2.0f / noiseReduction;

    float value = rawValue;


    if (value < g_cavaPrevOut[index]) {
        value = g_cavaPeak[index] *
            (1.0f -
             g_cavaFall[index] *
             g_cavaFall[index] *
             gravityMod);

        value = std::max(0.0f, value);
        g_cavaFall[index] += 0.028f;
    } else {
        g_cavaPeak[index] = value;
        g_cavaFall[index] = 0.0f;
    }

    g_cavaPrevOut[index] = value;


    const float integralAlpha =
        1.0f / (1.0f + noiseReduction * 0.12f);

    value = g_cavaMem[index] +
        (value - g_cavaMem[index]) * integralAlpha;

    g_cavaMem[index] = std::clamp(value, 0.0f, 1.0f);

    return g_cavaMem[index];
}

static void ResetCavaSmoothing() {
    std::fill(std::begin(g_cavaFall), std::end(g_cavaFall), 0.0f);
    std::fill(std::begin(g_cavaMem), std::end(g_cavaMem), 0.0f);
    std::fill(std::begin(g_cavaPeak), std::end(g_cavaPeak), 0.0f);
    std::fill(std::begin(g_cavaPrevOut), std::end(g_cavaPrevOut), 0.0f);
}

static void UpdateAnimationFromAudio() {
    const int barCount = std::clamp(g_settings.barCount, 1, VIZ_BANDS_MAX);

    float bands[VIZ_NUM_BANDS];
    float masterPeak = 0.0f;

    for (int i = 0; i < VIZ_NUM_BANDS; ++i) {
        bands[i] = g_audioBands[i].load(std::memory_order_relaxed);
        masterPeak = std::max(masterPeak, bands[i]);
    }

    auto sampleBands = [&](float t) -> float {
        t = std::clamp(t, 0.0f, 1.0f);

        if (g_settings.interpolationMode == 1) {
            int idx = static_cast<int>(t * VIZ_NUM_BANDS);
            idx = std::clamp(idx, 0, VIZ_NUM_BANDS - 1);
            return bands[idx];
        }

        const float pos = t * (VIZ_NUM_BANDS - 1);
        const int lo = std::clamp(static_cast<int>(pos), 0, VIZ_NUM_BANDS - 1);
        const int hi = std::min(lo + 1, VIZ_NUM_BANDS - 1);
        const float frac = pos - static_cast<float>(lo);

        if (g_settings.interpolationMode == 2) {
            const float eased = 0.5f - 0.5f * cosf(frac * VIZ_PI);
            return bands[lo] * (1.0f - eased) + bands[hi] * eased;
        }

        if (g_settings.interpolationMode == 3) {
            const int i0 = std::max(0, lo - 1);
            const int i1 = lo;
            const int i2 = hi;
            const int i3 = std::min(VIZ_NUM_BANDS - 1, hi + 1);
            const float p0 = bands[i0];
            const float p1 = bands[i1];
            const float p2 = bands[i2];
            const float p3 = bands[i3];
            const float f2 = frac * frac;
            const float f3 = f2 * frac;
            const float value = 0.5f * ((2.0f * p1) +
                (-p0 + p2) * frac +
                (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * f2 +
                (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * f3);
            return std::clamp(value, 0.0f, 1.0f);
        }

        return bands[lo] * (1.0f - frac) + bands[hi] * frac;
    };

    const float attack = static_cast<float>(g_settings.attackSpeed) / 100.0f;
    const float decay = static_cast<float>(g_settings.decaySpeed) / 100.0f;
    const float minHeight = static_cast<float>(g_settings.minBarHeight);

    static ULONGLONG fallbackLastMs = 0;
    static LONGLONG animationLastQpc = 0;
    static const double qpcFrequency = []() -> double {
        LARGE_INTEGER frequency{};
        if (!QueryPerformanceFrequency(&frequency) || frequency.QuadPart <= 0)
            return 0.0;
        return static_cast<double>(frequency.QuadPart);
    }();

    const ULONGLONG nowMs = GetTickCount64();
    const ULONGLONG lastAudioMs =
        g_lastAudioUpdateMs.load(std::memory_order_acquire);

    if (fallbackLastMs == 0)
        fallbackLastMs = nowMs;

    LARGE_INTEGER nowQpc{};
    QueryPerformanceCounter(&nowQpc);

    if (animationLastQpc == 0 || qpcFrequency <= 0.0)
        animationLastQpc = nowQpc.QuadPart;

    // Use QPC for the animation clock rather than GetTickCount64.
    // Windows' millisecond clock can quantize multiple 120/144/165/180 Hz
    // callbacks to the same timestamp, which introduces visible motion jitter.
    // QPC keeps the elapsed-time math continuous even when the timer fires at
    // high refresh rates.
    const float dtFrames = std::clamp(
        static_cast<float>(
            static_cast<double>(nowQpc.QuadPart - animationLastQpc) /
            qpcFrequency * 60.0),
        0.0f, 8.0f);
    animationLastQpc = nowQpc.QuadPart;

    if (lastAudioMs != 0 && nowMs > lastAudioMs + 120) {
        if (fallbackLastMs < lastAudioMs)
            fallbackLastMs = lastAudioMs;

        const float audioDtFrames = std::clamp(
            static_cast<float>(nowMs - fallbackLastMs) / (1000.0f / 60.0f),
            0.0f, 8.0f);

        if (audioDtFrames > 0.0f) {
            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                float v = g_audioBands[b].load(std::memory_order_relaxed);
                for (;;) {
                    float dec = std::max(0.0f, v - GetBandGravity(b) * audioDtFrames);
                    if (g_audioBands[b].compare_exchange_weak(v, dec, std::memory_order_relaxed))
                        break;
                }
            }
            fallbackLastMs = nowMs;
        }
    } else {
        fallbackLastMs = nowMs;
    }

    static float g_wavePhase = 0.0f;
    // The Wave deformation is also time-based. Without this multiplier, a
    // 180 Hz render rate would make the wave travel roughly 3x faster than
    // at 60 Hz even though the audio itself did not change.
    g_wavePhase += 0.06f * dtFrames;

    for (int i = 0; i < barCount; ++i) {
        float freqT = 0.5f;

        if (g_settings.barShape == 1) { // Mountain
            float center = (barCount > 1) ? (barCount - 1) * 0.5f : 0.0f;
            float dist = center > 0 ? fabsf(static_cast<float>(i) - center) / center : 0.0f;
            freqT = dist;
        } else if (g_settings.barShape == 2) { // Mirror
            int half = barCount / 2;
            if (half < 1) half = 1;
            int idxInHalf = (i < half) ? i : (barCount - 1 - i);
            freqT = (half > 1) ? static_cast<float>(idxInHalf) / static_cast<float>(half - 1) : 0.5f;
        } else { // Stereo or Wave
            freqT = barCount > 1
                ? static_cast<float>(i) / static_cast<float>(barCount - 1)
                : 0.5f;
        }

        float target = sampleBands(freqT) + masterPeak * 0.04f;

        if (g_settings.barShape == 3) { // Wave
            float wave = sinf(static_cast<float>(i) * 0.3f + g_wavePhase);
            target *= (0.75f + 0.25f * wave);
        }

        target = std::clamp(target, 0.0f, 1.0f);


        if (g_settings.cavaSmoothingEnabled) {
            if (!g_cavaWasEnabled) {
                ResetCavaSmoothing();
                g_cavaWasEnabled = true;
            }

            target = ApplyCavaSmoothing(
                target,
                i,
                g_settings.cavaNoiseReduction);
        } else if (g_cavaWasEnabled) {
            ResetCavaSmoothing();
            g_cavaWasEnabled = false;
        }

        float current = g_currentHeights[i] /
                        std::max(1, g_settings.maxBarHeight);
        current = std::clamp(current, 0.0f, 1.0f);


        if (g_settings.cavaSmoothingEnabled) {
            current = target;
        } else {
            const float response = FrameRateIndependentResponse(
                target > current ? attack : decay,
                dtFrames);
            current += (target - current) * response;
        }

        g_currentHeights[i] = std::clamp(
            current * g_settings.maxBarHeight,
            minHeight,
            static_cast<float>(g_settings.maxBarHeight));
    }

    for (int i = barCount; i < VIZ_BANDS_MAX; ++i)
        g_currentHeights[i] = 0.0f;
}


static DWORD MixColor(DWORD a, DWORD b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return RGB(
        static_cast<BYTE>(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t),
        static_cast<BYTE>(GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t),
        static_cast<BYTE>(GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t));
}

enum PointedDirection {
    POINTED_BOTTOM_UP = 0,
    POINTED_TOP_DOWN = 1,
    POINTED_LEFT_RIGHT = 2,
    POINTED_RIGHT_LEFT = 3,
    POINTED_CENTER_VERTICAL = 4,
    POINTED_CENTER_HORIZONTAL = 5
};

static PointedDirection g_pointedDirection = POINTED_BOTTOM_UP;

static RECT ExpandRect(const RECT& rect, int amount) {
    RECT expanded = rect;
    expanded.left -= amount;
    expanded.top -= amount;
    expanded.right += amount;
    expanded.bottom += amount;
    return expanded;
}

static float GetPointedTipLength(const RECT& rect) {
    const int width = std::max(1L, rect.right - rect.left);
    const int height = std::max(1L, rect.bottom - rect.top);
    const bool horizontal =
        g_pointedDirection == POINTED_LEFT_RIGHT ||
        g_pointedDirection == POINTED_RIGHT_LEFT ||
        g_pointedDirection == POINTED_CENTER_HORIZONTAL;
    const int length = horizontal ? width : height;


    const float sharpness =
        std::clamp(static_cast<float>(g_settings.pointedSharpness), 0.0f, 100.0f) / 100.0f;
    const float fraction = 0.04f + 0.46f * sharpness;
    return std::clamp(
        static_cast<float>(length) * fraction,
        1.0f,
        std::max(1.0f, static_cast<float>(length - 1)));
}

static void AddPointedBarPath(
    Gdiplus::GraphicsPath& path,
    const RECT& rect) {
    const float x = static_cast<float>(rect.left);
    const float y = static_cast<float>(rect.top);
    const float w = static_cast<float>(rect.right - rect.left);
    const float h = static_cast<float>(rect.bottom - rect.top);
    const float cx = x + w * 0.5f;
    const float cy = y + h * 0.5f;
    const float tip = GetPointedTipLength(rect);

    switch (g_pointedDirection) {
        case POINTED_TOP_DOWN:
            // Square top / pointed bottom.
            path.AddLine(x, y, x + w, y);
            path.AddLine(x + w, y, x + w, y + h - tip);
            path.AddLine(x + w, y + h - tip, cx, y + h);
            path.AddLine(cx, y + h, x, y + h - tip);
            path.AddLine(x, y + h - tip, x, y);
            break;

        case POINTED_LEFT_RIGHT:
            // Square left / pointed right.
            path.AddLine(x, y, x + w - tip, y);
            path.AddLine(x + w - tip, y, x + w, cy);
            path.AddLine(x + w, cy, x + w - tip, y + h);
            path.AddLine(x + w - tip, y + h, x, y + h);
            path.AddLine(x, y + h, x, y);
            break;

        case POINTED_RIGHT_LEFT:
            // Square right / pointed left.
            path.AddLine(x + w, y, x + tip, y);
            path.AddLine(x + tip, y, x, cy);
            path.AddLine(x, cy, x + tip, y + h);
            path.AddLine(x + tip, y + h, x + w, y + h);
            path.AddLine(x + w, y + h, x + w, y);
            break;

        case POINTED_CENTER_VERTICAL:
            // Both outer ends are pointed; the center remains full-width/square.
            path.AddLine(cx, y, x + w, y + tip);
            path.AddLine(x + w, y + h - tip, cx, y + h);
            path.AddLine(cx, y + h, x, y + h - tip);
            path.AddLine(x, y + tip, cx, y);
            break;

        case POINTED_CENTER_HORIZONTAL:
            // Both outer ends are pointed; the center remains full-height/square.
            path.AddLine(x, cy, x + tip, y);
            path.AddLine(x + w - tip, y, x + w, cy);
            path.AddLine(x + w, cy, x + w - tip, y + h);
            path.AddLine(x + tip, y + h, x, cy);
            break;

        case POINTED_BOTTOM_UP:
        default:
            // Square bottom / pointed top.
            path.AddLine(x, y + h, x + w, y + h);
            path.AddLine(x + w, y + h, x + w, y + tip);
            path.AddLine(x + w, y + tip, cx, y);
            path.AddLine(cx, y, x, y + tip);
            path.AddLine(x, y + tip, x, y + h);
            break;
    }

    path.CloseFigure();
}


static void AddRoundedRectSubpath(
    Gdiplus::GraphicsPath& path,
    float x,
    float y,
    float w,
    float h,
    float radius) {

    if (w <= 0.0f || h <= 0.0f)
        return;

    radius = std::clamp(radius, 0.0f, std::min(w, h) * 0.5f);
    if (radius <= 0.01f) {
        path.AddRectangle(Gdiplus::RectF(x, y, w, h));
        return;
    }

    const float d = radius * 2.0f;
    path.StartFigure();
    path.AddLine(x + radius, y, x + w - radius, y);
    path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path.AddLine(x + w, y + radius, x + w, y + h - radius);
    path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path.AddLine(x + w - radius, y + h, x + radius, y + h);
    path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path.AddLine(x, y + h - radius, x, y + radius);
    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.CloseFigure();
}


static void AddBatteryBarPath(
    Gdiplus::GraphicsPath& path,
    const RECT& rect) {

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    const float x = static_cast<float>(rect.left);
    const float y = static_cast<float>(rect.top);
    const float w = static_cast<float>(rect.right - rect.left);
    const float h = static_cast<float>(rect.bottom - rect.top);

    const bool horizontal =
        g_pointedDirection == POINTED_LEFT_RIGHT ||
        g_pointedDirection == POINTED_RIGHT_LEFT ||
        g_pointedDirection == POINTED_CENTER_HORIZONTAL;

    const float cross = horizontal ? h : w;
    const float length = horizontal ? w : h;

    const float requestedRadius =
        std::max(2.0f, static_cast<float>(g_settings.cornerRadius));
    const float bodyRadius =
        std::min(6.0f, std::min(requestedRadius, cross * 0.5f));

    const float nubDepth = std::clamp(
        cross * 0.18f,
        2.0f,
        std::max(2.0f, length * 0.30f));
    const float nubWidth = std::clamp(
        cross * 0.42f,
        2.0f,
        std::max(2.0f, cross - 2.0f));
    const float nubRadius = std::max(
        0.5f,
        std::min(1.5f, std::min(nubWidth, nubDepth) * 0.35f));

    if (!horizontal) {
        AddRoundedRectSubpath(path, x, y, w, h, bodyRadius);

        const float nubX = x + (w - nubWidth) * 0.5f;
        if (g_pointedDirection == POINTED_TOP_DOWN) {
            AddRoundedRectSubpath(
                path, nubX, y + h, nubWidth, nubDepth, nubRadius);
        } else if (g_pointedDirection == POINTED_CENTER_VERTICAL) {
            AddRoundedRectSubpath(
                path, nubX, y - nubDepth, nubWidth, nubDepth, nubRadius);
            AddRoundedRectSubpath(
                path, nubX, y + h, nubWidth, nubDepth, nubRadius);
        } else {
            // Bottom-up: flat/square base, cap on the active top.
            AddRoundedRectSubpath(
                path, nubX, y - nubDepth, nubWidth, nubDepth, nubRadius);
        }
    } else {
        AddRoundedRectSubpath(path, x, y, w, h, bodyRadius);

        const float nubY = y + (h - nubWidth) * 0.5f;
        if (g_pointedDirection == POINTED_RIGHT_LEFT) {
            AddRoundedRectSubpath(
                path, x - nubDepth, nubY, nubDepth, nubWidth, nubRadius);
        } else if (g_pointedDirection == POINTED_CENTER_HORIZONTAL) {
            AddRoundedRectSubpath(
                path, x - nubDepth, nubY, nubDepth, nubWidth, nubRadius);
            AddRoundedRectSubpath(
                path, x + w, nubY, nubDepth, nubWidth, nubRadius);
        } else {
            AddRoundedRectSubpath(
                path, x + w, nubY, nubDepth, nubWidth, nubRadius);
        }
    }
}


static float GetBatteryLiquidLevel(int index, float heightRatio) {
    heightRatio = std::clamp(heightRatio, 0.0f, 1.0f);
    if (heightRatio <= 0.03f)
        return 0.0f;

    const float t = static_cast<float>(GetTickCount64() % 1000000) * 0.001f;
    const float phase = t * (0.55f + 0.035f * static_cast<float>((index % 11) + 1))
        + static_cast<float>(index) * 1.917f;

    float n = 0.5f
        + 0.30f * sinf(phase)
        + 0.20f * sinf(phase * 1.63f + 0.9f);
    n = std::clamp(n, 0.0f, 1.0f);


    const float maxGap = 0.08f + 0.20f * std::clamp(heightRatio, 0.0f, 1.0f);
    return std::clamp(maxGap * (0.35f + 0.65f * n), 0.0f, 0.32f);
}

static float GetBatteryLiquidWave(int index, float t) {
    const float phase = static_cast<float>(index) * 1.731f;
    return 0.5f * sinf(t * 2.7f + phase)
         + 0.5f * sinf(t * 4.1f + phase * 1.37f + 0.8f);
}

static void DrawBatteryLiquidGap(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int index,
    float heightRatio,
    DWORD color) {

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    const float x = static_cast<float>(rect.left);
    const float y = static_cast<float>(rect.top);
    const float w = static_cast<float>(rect.right - rect.left);
    const float h = static_cast<float>(rect.bottom - rect.top);

    const bool horizontal =
        g_pointedDirection == POINTED_LEFT_RIGHT ||
        g_pointedDirection == POINTED_RIGHT_LEFT ||
        g_pointedDirection == POINTED_CENTER_HORIZONTAL;

    const bool centered =
        g_pointedDirection == POINTED_CENTER_VERTICAL ||
        g_pointedDirection == POINTED_CENTER_HORIZONTAL;

    const float gapRatio = GetBatteryLiquidLevel(index, heightRatio);
    if (gapRatio <= 0.001f)
        return;

    const float now = static_cast<float>(GetTickCount64() % 1000000) * 0.001f;
    const float wave = GetBatteryLiquidWave(index, now);


    const float nibble = horizontal ? w : h;
    const float wobblePx = std::min(2.0f, nibble * 0.05f);


    const BYTE liquidAlpha = static_cast<BYTE>(
        std::clamp(42.0f + 32.0f * (0.5f + 0.5f * wave), 28.0f, 78.0f));

    Gdiplus::GraphicsState state = graphics.Save();
    Gdiplus::GraphicsPath batteryPath;
    AddBatteryBarPath(batteryPath, rect);
    graphics.SetClip(&batteryPath, Gdiplus::CombineModeIntersect);
    graphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

    if (!horizontal) {
        const float gapH = h * gapRatio;
        if (gapH < 1.0f) {
            graphics.Restore(state);
            return;
        }

        const bool pocketAtTop = g_pointedDirection == POINTED_BOTTOM_UP;
        const bool pocketAtBottom = g_pointedDirection == POINTED_TOP_DOWN;

        auto fillVerticalPocket = [&](float topEdge, float bottomEdge, bool fromTop) {
            Gdiplus::GraphicsPath path;
            path.StartFigure();

            if (fromTop) {
                path.AddLine(x, y, x + w, y);
                const float surfaceY = y + gapH;

                const int samples = 16;
                float prevX = x + w;
                float prevY = surfaceY;
                for (int s = samples - 1; s >= 0; --s) {
                    const float u = static_cast<float>(s) / static_cast<float>(samples);
                    const float px = x + u * w;
                    const float local = sinf(u * 6.2831853f + now * 3.2f + index * 0.37f);
                    const float py = surfaceY + local * wobblePx;
                    path.AddLine(prevX, prevY, px, py);
                    prevX = px;
                    prevY = py;
                }
                path.AddLine(prevX, prevY, x, y);
            } else {
                path.AddLine(x, y + h, x + w, y + h);
                const float surfaceY = y + h - gapH;

                const int samples = 16;
                float prevX = x;
                float prevY = surfaceY;
                for (int s = 1; s <= samples; ++s) {
                    const float u = static_cast<float>(s) / static_cast<float>(samples);
                    const float px = x + u * w;
                    const float local = sinf(u * 6.2831853f + now * 3.2f + index * 0.37f);
                    const float py = surfaceY + local * wobblePx;
                    path.AddLine(prevX, prevY, px, py);
                    prevX = px;
                    prevY = py;
                }
                path.AddLine(prevX, prevY, x + w, y + h);
            }

            path.CloseFigure();
            Gdiplus::SolidBrush liquidBrush(
                Gdiplus::Color(
                    liquidAlpha,
                    GetRValue(color),
                    GetGValue(color),
                    GetBValue(color)));
            graphics.FillPath(&liquidBrush, &path);
        };

        if (pocketAtTop) {
            fillVerticalPocket(y, y + gapH, true);
        } else if (pocketAtBottom) {
            fillVerticalPocket(y + h - gapH, y + h, false);
        } else if (g_pointedDirection == POINTED_CENTER_VERTICAL) {

            const float halfGap = gapH * 0.5f;
            RECT topRect{
                rect.left, rect.top, rect.right,
                static_cast<LONG>(rect.top + halfGap)};
            RECT bottomRect{
                rect.left,
                static_cast<LONG>(rect.bottom - halfGap),
                rect.right, rect.bottom};


            Gdiplus::SolidBrush liquidBrush(
                Gdiplus::Color(
                    liquidAlpha,
                    GetRValue(color),
                    GetGValue(color),
                    GetBValue(color)));
            graphics.FillRectangle(&liquidBrush,
                static_cast<float>(topRect.left), static_cast<float>(topRect.top),
                static_cast<float>(topRect.right - topRect.left),
                static_cast<float>(topRect.bottom - topRect.top));
            graphics.FillRectangle(&liquidBrush,
                static_cast<float>(bottomRect.left), static_cast<float>(bottomRect.top),
                static_cast<float>(bottomRect.right - bottomRect.left),
                static_cast<float>(bottomRect.bottom - bottomRect.top));
        }
    } else {
        const float gapW = w * gapRatio;
        if (gapW < 1.0f) {
            graphics.Restore(state);
            return;
        }

        const bool pocketAtEnd = g_pointedDirection == POINTED_LEFT_RIGHT;

        if (centered) {

            const float halfGap = gapW * 0.5f;
            for (int side = 0; side < 2; ++side) {
                const bool leftSide = side == 0;
                const float x0 = leftSide ? x : (x + w - halfGap);
                const float x1 = leftSide ? (x + halfGap) : (x + w);
                Gdiplus::GraphicsPath path;
                path.StartFigure();
                path.AddLine(x0, y, x1, y);
                path.AddLine(x1, y + h, x0, y + h);
                path.CloseFigure();
                Gdiplus::SolidBrush liquidBrush(
                    Gdiplus::Color(
                        liquidAlpha,
                        GetRValue(color),
                        GetGValue(color),
                        GetBValue(color)));
                graphics.FillPath(&liquidBrush, &path);
            }
        } else {

            const float x0 = pocketAtEnd ? (x + w - gapW) : x;
            const float x1 = pocketAtEnd ? (x + w) : (x + gapW);
            Gdiplus::GraphicsPath path;
            path.StartFigure();
            path.AddLine(x0, y, x1, y);
            path.AddLine(x1, y + h, x0, y + h);
            path.CloseFigure();
            Gdiplus::SolidBrush liquidBrush(
                Gdiplus::Color(
                    liquidAlpha,
                    GetRValue(g_settings.color1),
                    GetGValue(g_settings.color1),
                    GetBValue(g_settings.color1)));
            graphics.FillPath(&liquidBrush, &path);
        }
    }

    graphics.Restore(state);
}

static DWORD GetVisualizerColorPrimary();
static DWORD GetVisualizerColorSecondary();
static DWORD GetAlbumPalettePrimary();
static DWORD GetAlbumPaletteSecondary();

static void DrawGlassBorder(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int radius,
    Gdiplus::Pen& pen);

static void DrawSegmentedBarBorder(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int segmentSize,
    int gap,
    int cornerRadius,
    Gdiplus::Pen& pen) {

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    const bool horizontalAxis = g_settings.orientation > 2;
    const bool centerAlign = g_settings.orientation == 1 ||
                             g_settings.orientation == 4;
    const bool alignToEnd = g_settings.orientation == 0 ||
                            g_settings.orientation == 5;

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    const int length = horizontalAxis ? width : height;
    segmentSize = std::max(1, std::min(segmentSize, length));
    gap = std::clamp(gap, 0, 1000);

    const int count = std::max(
        1, (length + gap) / std::max(1, segmentSize + gap));
    const int totalLength =
        count * segmentSize + std::max(0, count - 1) * gap;
    const int freeSpace = std::max(0, length - totalLength);

    int cursor = 0;
    if (centerAlign)
        cursor = freeSpace / 2;
    else if (alignToEnd)
        cursor = freeSpace;

    for (int i = 0; i < count; ++i) {
        RECT segment{};
        if (horizontalAxis) {
            segment.left = rect.left + cursor;
            segment.top = rect.top;
            segment.right = segment.left + segmentSize;
            segment.bottom = rect.bottom;
        } else {
            segment.left = rect.left;
            segment.top = rect.top + cursor;
            segment.right = rect.right;
            segment.bottom = segment.top + segmentSize;
        }
        DrawGlassBorder(graphics, segment, cornerRadius, pen);
        cursor += segmentSize + gap;
    }
}

static void DrawVisualizerPathBorder(
    Gdiplus::Graphics& graphics,
    const Gdiplus::GraphicsPath& path,
    const Gdiplus::RectF& bounds) {

    if (!g_settings.borderEnabled ||
        g_settings.borderThickness <= 0)
        return;

    DWORD c1 = GetVisualizerColorPrimary();
    DWORD c2 = GetVisualizerColorSecondary();

    switch (g_settings.borderMode) {
        case 2:
            c1 = GetAlbumPalettePrimary();
            c2 = c1;
            break;
        case 3:
            c1 = GetAlbumPalettePrimary();
            c2 = GetAlbumPaletteSecondary();
            break;
        case 4:
            c1 = g_settings.borderColor1;
            c2 = c1;
            break;
        case 5:
            c1 = g_settings.borderColor1;
            c2 = g_settings.borderColor2;
            break;
        case 1:
            break;
        case 0:
        default:
            c2 = c1;
            break;
    }

    const BYTE alpha = static_cast<BYTE>(
        std::clamp(255 * g_settings.acrylicOpacity / 100, 0, 255));

    if (g_settings.borderMode == 1 ||
        g_settings.borderMode == 3 ||
        g_settings.borderMode == 5) {
        const bool horizontalAxis = g_settings.orientation > 2;
        Gdiplus::LinearGradientBrush gradientBrush(
            horizontalAxis
                ? Gdiplus::PointF(bounds.X, bounds.Y)
                : Gdiplus::PointF(bounds.X, bounds.Y + bounds.Height),
            horizontalAxis
                ? Gdiplus::PointF(bounds.X + bounds.Width, bounds.Y)
                : Gdiplus::PointF(bounds.X, bounds.Y),
            Gdiplus::Color(alpha, GetRValue(c1), GetGValue(c1), GetBValue(c1)),
            Gdiplus::Color(alpha, GetRValue(c2), GetGValue(c2), GetBValue(c2)));
        Gdiplus::Pen pen(
            &gradientBrush,
            static_cast<Gdiplus::REAL>(g_settings.borderThickness));
        graphics.DrawPath(&pen, &path);
    } else {
        Gdiplus::Pen pen(
            Gdiplus::Color(
                alpha, GetRValue(c1), GetGValue(c1), GetBValue(c1)),
            static_cast<Gdiplus::REAL>(g_settings.borderThickness));
        graphics.DrawPath(&pen, &path);
    }
}

static void DrawVisualizerBarBorder(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int radius,
    int segmentSize = 0) {

    if (!g_settings.borderEnabled ||
        g_settings.borderThickness <= 0 ||
        rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    DWORD c1 = GetVisualizerColorPrimary();
    DWORD c2 = GetVisualizerColorSecondary();

    switch (g_settings.borderMode) {
        case 2:
            c1 = GetAlbumPalettePrimary();
            c2 = c1;
            break;
        case 3:
            c1 = GetAlbumPalettePrimary();
            c2 = GetAlbumPaletteSecondary();
            break;
        case 4:
            c1 = g_settings.borderColor1;
            c2 = c1;
            break;
        case 5:
            c1 = g_settings.borderColor1;
            c2 = g_settings.borderColor2;
            break;
        case 1:
            break;
        case 0:
        default:
            c2 = c1;
            break;
    }

    const BYTE alpha = static_cast<BYTE>(
        std::clamp(255 * g_settings.acrylicOpacity / 100, 0, 255));

    auto drawWithPen = [&](Gdiplus::Pen& pen) {
        if (g_settings.barStyle == 2) {
            DrawSegmentedBarBorder(
                graphics, rect,
                segmentSize > 0 ? segmentSize : std::max(1, g_settings.barWidth),
                g_settings.segmentSpacing, radius, pen);
        } else {
            DrawGlassBorder(graphics, rect, radius, pen);
        }
    };

    if (g_settings.borderMode == 1 ||
        g_settings.borderMode == 3 ||
        g_settings.borderMode == 5) {
        const bool horizontalAxis = g_settings.orientation > 2;
        Gdiplus::LinearGradientBrush gradientBrush(
            horizontalAxis
                ? Gdiplus::Point(rect.left, rect.top)
                : Gdiplus::Point(rect.left, rect.bottom),
            horizontalAxis
                ? Gdiplus::Point(rect.right, rect.top)
                : Gdiplus::Point(rect.left, rect.top),
            Gdiplus::Color(alpha, GetRValue(c1), GetGValue(c1), GetBValue(c1)),
            Gdiplus::Color(alpha, GetRValue(c2), GetGValue(c2), GetBValue(c2)));
        Gdiplus::Pen pen(
            &gradientBrush,
            static_cast<Gdiplus::REAL>(g_settings.borderThickness));
        drawWithPen(pen);
    } else {
        Gdiplus::Pen pen(
            Gdiplus::Color(
                alpha, GetRValue(c1), GetGValue(c1), GetBValue(c1)),
            static_cast<Gdiplus::REAL>(g_settings.borderThickness));
        drawWithPen(pen);
    }
}

static void DrawBarBrush(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int radius,
    const Gdiplus::Brush& brush) {

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    if (g_settings.barStyle == 4) {
        Gdiplus::GraphicsPath path;
        AddPointedBarPath(path, rect);
        graphics.FillPath(&brush, &path);
        return;
    }

    if (g_settings.barStyle == 5) {
        Gdiplus::GraphicsPath path;
        AddBatteryBarPath(path, rect);
        graphics.FillPath(&brush, &path);
        return;
    }

    if (radius > 0) {

        const int width = rect.right - rect.left;
        const int height = rect.bottom - rect.top;
        radius = std::clamp(radius, 1, std::min(width / 2, height / 2));

        Gdiplus::GraphicsPath path;
        const float d = static_cast<float>(radius * 2);
        const float x = static_cast<float>(rect.left);
        const float y = static_cast<float>(rect.top);
        const float w = static_cast<float>(width);
        const float h = static_cast<float>(height);

        path.AddArc(x, y, d, d, 180.0f, 90.0f);
        path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
        path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
        path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
        path.CloseFigure();
        graphics.FillPath(&brush, &path);
    } else {
        graphics.FillRectangle(
            &brush,
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right - rect.left),
            static_cast<float>(rect.bottom - rect.top));
    }
}

static void DrawGlassBorder(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int radius,
    Gdiplus::Pen& pen) {

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    pen.SetAlignment(Gdiplus::PenAlignmentInset);
    Gdiplus::Pen* drawPen = &pen;

    if (g_settings.barStyle == 4) {
        Gdiplus::GraphicsPath path;
        AddPointedBarPath(path, rect);
        graphics.DrawPath(drawPen, &path);
        return;
    }

    if (g_settings.barStyle == 5) {
        Gdiplus::GraphicsPath path;
        AddBatteryBarPath(path, rect);
        graphics.DrawPath(drawPen, &path);
        return;
    }

    if (radius <= 0) {
        graphics.DrawRectangle(
            drawPen,
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right - rect.left - 1),
            static_cast<float>(rect.bottom - rect.top - 1));
        return;
    }

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    const int pathWidth = std::max(1, width - 1);
    const int pathHeight = std::max(1, height - 1);
    const int maxRadius = std::min(pathWidth / 2, pathHeight / 2);
    if (maxRadius <= 0) {
        graphics.DrawRectangle(
            drawPen,
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(pathWidth),
            static_cast<float>(pathHeight));
        return;
    }
    radius = std::clamp(radius, 1, maxRadius);

    Gdiplus::GraphicsPath path;
    const float d = static_cast<float>(radius * 2);
    const float x = static_cast<float>(rect.left);
    const float y = static_cast<float>(rect.top);
    const float w = static_cast<float>(pathWidth);
    const float h = static_cast<float>(pathHeight);

    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
    graphics.DrawPath(drawPen, &path);
}

static void FillRoundedBarGdiPlus(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int radius,
    const Gdiplus::Brush& brush) {
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0)
        return;

    if (radius <= 0) {
        graphics.FillRectangle(
            &brush,
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(width),
            static_cast<float>(height));
        return;
    }

    const int maxRadius = std::min(width / 2, height / 2);
    radius = std::clamp(radius, 1, maxRadius);

    Gdiplus::GraphicsPath path;
    const float d = static_cast<float>(radius * 2);
    const float x = static_cast<float>(rect.left);
    const float y = static_cast<float>(rect.top);
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);

    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
    graphics.FillPath(&brush, &path);
}


static bool ApplySegmentedSquareClip(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    int segmentSize,
    int gap,
    int cornerRadius,
    bool horizontalAxis,
    bool centerAlign,
    bool alignToEnd,
    Gdiplus::GraphicsState* outState) {

    if (!outState || rect.right <= rect.left || rect.bottom <= rect.top)
        return false;

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    const int length = horizontalAxis ? width : height;
    segmentSize = std::max(1, segmentSize);
    gap = std::clamp(gap, 0, 1000);
    cornerRadius = std::max(0, cornerRadius);


    segmentSize = std::min(segmentSize, length);

    int count = (length + gap) / std::max(1, segmentSize + gap);
    count = std::max(1, count);

    const int totalLength =
        count * segmentSize + std::max(0, count - 1) * gap;

    const int freeSpace = std::max(0, length - totalLength);
    int cursor = 0;
    if (centerAlign) {
        cursor = freeSpace / 2;
    } else if (alignToEnd) {

        cursor = freeSpace;
    }

    Gdiplus::Region region;
    region.MakeEmpty();

    for (int i = 0; i < count; ++i) {
        RECT segment{};
        if (horizontalAxis) {
            segment.left = rect.left + cursor;
            segment.top = rect.top;
            segment.right = segment.left + segmentSize;
            segment.bottom = rect.bottom;
        } else {
            segment.left = rect.left;
            segment.top = rect.top + cursor;
            segment.right = rect.right;
            segment.bottom = segment.top + segmentSize;
        }

        const INT segmentX = static_cast<INT>(segment.left);
        const INT segmentY = static_cast<INT>(segment.top);
        const INT segmentW = static_cast<INT>(
            segment.right > segment.left ? segment.right - segment.left : 1);
        const INT segmentH = static_cast<INT>(
            segment.bottom > segment.top ? segment.bottom - segment.top : 1);

        if (cornerRadius <= 0) {
            region.Union(
                Gdiplus::Rect(segmentX, segmentY, segmentW, segmentH));
        } else {
            const int maxRadius = std::min(segmentW, segmentH) / 2;
            const int radius = std::min(cornerRadius, maxRadius);

            if (radius <= 0) {
                region.Union(
                    Gdiplus::Rect(segmentX, segmentY, segmentW, segmentH));
            } else {
                const float x = static_cast<float>(segmentX);
                const float y = static_cast<float>(segmentY);
                const float w = static_cast<float>(segmentW);
                const float h = static_cast<float>(segmentH);
                const float d = static_cast<float>(radius * 2);

                Gdiplus::GraphicsPath path;
                path.AddArc(x, y, d, d, 180.0f, 90.0f);
                path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
                path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
                path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
                path.CloseFigure();
                region.Union(&path);
            }
        }

        cursor += segmentSize + gap;
    }

    *outState = graphics.Save();
    graphics.SetClip(&region, Gdiplus::CombineModeIntersect);
    return true;
}


static DWORD GetAlbumPalettePrimary();
static DWORD GetAlbumPaletteSecondary();

static DWORD GetVisualizerColorPrimary() {
    if (g_settings.colorMode == 7 || g_settings.colorMode == 8)
        return GetAlbumPalettePrimary();
    return g_settings.color1;
}

static DWORD GetVisualizerColorSecondary() {
    if (g_settings.colorMode == 8)
        return GetAlbumPaletteSecondary();
    return g_settings.color2;
}

static bool IsAlbumPaletteMode() {
    // Keep the expensive palette extraction disabled when the album widget is
    // the only consumer. The widget itself needs the artwork, not the palette.
    return g_settings.colorMode == 7 || g_settings.colorMode == 8 ||
           (g_settings.backgroundEnabled &&
            (g_settings.backgroundMode == 3 || g_settings.backgroundMode == 4)) ||
           (g_settings.backgroundBorderEnabled &&
            (g_settings.backgroundBorderMode == 0 ||
             g_settings.backgroundBorderMode == 1)) ||
           (g_settings.lyricsEnabled && g_settings.lyricsBackgroundEnabled &&
            (g_settings.lyricsBackgroundMode == 2 ||
             g_settings.lyricsBackgroundMode == 3)) ||
           (g_settings.lyricsEnabled && g_settings.lyricsBorderEnabled &&
            (g_settings.lyricsBorderMode == 0 ||
             g_settings.lyricsBorderMode == 1));
}

static bool IsAlbumColorMode() {
    // Album worker is also required for the third desktop widget, which needs
    // artwork even when no album-derived colors are enabled.
    return IsAlbumPaletteMode() || g_settings.albumWidgetEnabled;
}

static void RenderCurveVisualizer(
    Gdiplus::Graphics& graphics,
    int barCount) {

    barCount = std::clamp(barCount, 1, VIZ_BANDS_MAX);

    const bool circular = (g_settings.barShape == 4);
    const bool vertical = (g_settings.orientation <= 2);
    const bool centerMode = (g_settings.orientation == 1 ||
                             g_settings.orientation == 4);

    const float maxHeight =
        static_cast<float>(std::max(1, g_settings.maxBarHeight));

    std::vector<Gdiplus::PointF> points;
    points.reserve(static_cast<size_t>(barCount) + 1);

    if (circular) {

        const float innerRadius =
            static_cast<float>(g_settings.circleRadius);
        const int pointCount = std::max(3, barCount);

        std::vector<Gdiplus::PointF> outer;
        std::vector<Gdiplus::PointF> inner;
        outer.reserve(pointCount + 1);
        inner.reserve(pointCount + 1);

        for (int i = 0; i <= pointCount; ++i) {
            const int idx = (i == pointCount) ? 0 : i;
            const int srcIndex = std::min(idx, barCount - 1);
            const float angle =
                (static_cast<float>(g_settings.circleStartAngle) +
                 360.0f * static_cast<float>(idx) /
                     static_cast<float>(pointCount)) *
                (VIZ_PI / 180.0f);

            const float height =
                std::clamp(g_currentHeights[srcIndex], 0.0f, maxHeight);
            const float outerRadius = innerRadius + height;

            outer.emplace_back(
                static_cast<float>(g_settings.positionX) +
                    cosf(angle) * outerRadius,
                static_cast<float>(g_settings.positionY) +
                    sinf(angle) * outerRadius);

            inner.emplace_back(
                static_cast<float>(g_settings.positionX) +
                    cosf(angle) * innerRadius,
                static_cast<float>(g_settings.positionY) +
                    sinf(angle) * innerRadius);
        }

        Gdiplus::GraphicsPath path;
        path.AddCurve(
            outer.data(),
            static_cast<INT>(outer.size()),
            0.35f);


        std::vector<Gdiplus::PointF> innerReversed(
            inner.rbegin(), inner.rend());
        path.AddCurve(
            innerReversed.data(),
            static_cast<INT>(innerReversed.size()),
            0.35f);
        path.CloseFigure();

        const Gdiplus::RectF bounds(
            static_cast<float>(g_settings.positionX - g_settings.circleRadius - g_settings.maxBarHeight),
            static_cast<float>(g_settings.positionY - g_settings.circleRadius - g_settings.maxBarHeight),
            static_cast<float>(2 * (g_settings.circleRadius + g_settings.maxBarHeight)),
            static_cast<float>(2 * (g_settings.circleRadius + g_settings.maxBarHeight)));

        const float peakRatio = std::clamp(
            g_currentHeights[0] / maxHeight, 0.0f, 1.0f);

        float maxRatio = peakRatio;
        for (int i = 1; i < barCount; ++i) {
            maxRatio = std::max(
                maxRatio,
                std::clamp(g_currentHeights[i] / maxHeight, 0.0f, 1.0f));
        }

        const float colorT = ApplyHeightCurve(
            maxRatio,
            g_settings.gradientCurveEnabled,
            g_settings.gradientCurve);

        int alphaValue = std::clamp(
            255 * g_settings.acrylicOpacity / 100, 10, 255);

        if (g_settings.colorMode == 3) {
            const int minAlpha =
                (g_settings.dynamicAcrylicMinOpacity * 255) / 100;
            const float opacityT = ApplyHeightCurve(
                maxRatio,
                g_settings.opacityCurveEnabled,
                g_settings.opacityCurve);

            alphaValue = std::clamp(
                (minAlpha + static_cast<int>(
                    (255 - minAlpha) * opacityT)) *
                    g_settings.acrylicOpacity / 100,
                10, 255);
        }

        DWORD color = GetVisualizerColorPrimary();
        if (g_settings.colorMode == 3)
            color = LerpColor(GetVisualizerColorPrimary(), GetVisualizerColorSecondary(), colorT);

        Gdiplus::Brush* brush = nullptr;
        Gdiplus::SolidBrush solidBrush(
            Gdiplus::Color(
                static_cast<BYTE>(alphaValue),
                GetRValue(color),
                GetGValue(color),
                GetBValue(color)));

        Gdiplus::LinearGradientBrush* gradientBrush = nullptr;
        Gdiplus::Color gradientC1(
            static_cast<BYTE>(alphaValue),
            GetRValue(GetVisualizerColorPrimary()),
            GetGValue(GetVisualizerColorPrimary()),
            GetBValue(GetVisualizerColorPrimary()));
        Gdiplus::Color gradientC2(
            static_cast<BYTE>(alphaValue),
            GetRValue(GetVisualizerColorSecondary()),
            GetGValue(GetVisualizerColorSecondary()),
            GetBValue(GetVisualizerColorSecondary()));

        if (g_settings.colorMode == 1) {
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X + bounds.Width, bounds.Y),
                gradientC1, gradientC2);
            brush = gradientBrush;
        } else if (g_settings.colorMode == 6 || g_settings.colorMode == 8) {
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X, bounds.Y + bounds.Height),
                gradientC1, gradientC2);
            brush = gradientBrush;
        } else if (g_settings.colorMode == 4) {
            const DWORD lighter =
                MixColor(GetVisualizerColorPrimary(), RGB(255, 255, 255), 0.35f);
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X + bounds.Width, bounds.Y + bounds.Height),
                Gdiplus::Color(
                    static_cast<BYTE>(std::clamp(alphaValue * 0.78f, 8.0f, 255.0f)),
                    GetRValue(lighter), GetGValue(lighter), GetBValue(lighter)),
                Gdiplus::Color(
                    static_cast<BYTE>(std::clamp(alphaValue * 0.35f, 5.0f, 255.0f)),
                    GetRValue(GetVisualizerColorPrimary()),
                    GetGValue(GetVisualizerColorPrimary()),
                    GetBValue(GetVisualizerColorPrimary())));
            brush = gradientBrush;
        } else if (g_settings.colorMode == 5) {
            const DWORD lighter =
                MixColor(color, RGB(255, 255, 255), 0.58f);
            const DWORD darker =
                MixColor(color, RGB(0, 0, 0), 0.18f);
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X, bounds.Y + bounds.Height),
                Gdiplus::Color(
                    static_cast<BYTE>(std::clamp(alphaValue * 0.76f, 8.0f, 255.0f)),
                    GetRValue(lighter), GetGValue(lighter), GetBValue(lighter)),
                Gdiplus::Color(
                    static_cast<BYTE>(std::clamp(alphaValue * 0.52f, 8.0f, 255.0f)),
                    GetRValue(darker), GetGValue(darker), GetBValue(darker)));
            brush = gradientBrush;
        } else {
            brush = &solidBrush;
        }

        graphics.FillPath(brush, &path);

        if (g_settings.borderEnabled) {
            DrawVisualizerPathBorder(graphics, path, bounds);
        }

        if (g_settings.colorMode == 4 || g_settings.colorMode == 5) {
            const BYTE highlightAlpha = static_cast<BYTE>(
                std::clamp(
                    g_settings.glassHighlight *
                        ((g_settings.colorMode == 4) ? 1.8f : 1.5f),
                    1.0f, 255.0f));
            Gdiplus::Pen pen(
                Gdiplus::Color(highlightAlpha, 255, 255, 255),
                1.2f);
            graphics.DrawPath(&pen, &path);
        }

        delete gradientBrush;
        return;
    }

    const int curveWidth = std::max(50, g_settings.curveWidth);
    const int samples = std::max(2, barCount);

    for (int i = 0; i < samples; ++i) {
        const int srcIndex = std::min(i, barCount - 1);
        const float t = (samples > 1)
            ? static_cast<float>(i) / static_cast<float>(samples - 1)
            : 0.0f;
        const float offset = t * static_cast<float>(curveWidth);
        const float height = std::clamp(
            g_currentHeights[srcIndex], 0.0f, maxHeight);

        if (vertical) {
            const float x = static_cast<float>(g_settings.positionX) + offset;

            if (g_settings.orientation == 0) {
                points.emplace_back(
                    x,
                    static_cast<float>(g_settings.positionY) - height);
            } else if (g_settings.orientation == 2) {
                points.emplace_back(
                    x,
                    static_cast<float>(g_settings.positionY) + height);
            } else {
                points.emplace_back(
                    x,
                    static_cast<float>(g_settings.positionY) - height * 0.5f);
            }
        } else {
            const float y = static_cast<float>(g_settings.positionY) + offset;

            if (g_settings.orientation == 3) {
                points.emplace_back(
                    static_cast<float>(g_settings.positionX) + height, y);
            } else if (g_settings.orientation == 5) {
                points.emplace_back(
                    static_cast<float>(g_settings.positionX) - height, y);
            } else {
                points.emplace_back(
                    static_cast<float>(g_settings.positionX) + height * 0.5f, y);
            }
        }
    }

    Gdiplus::GraphicsPath path;

    if (centerMode) {
        std::vector<Gdiplus::PointF> lower;
        lower.reserve(points.size());

        for (size_t i = 0; i < points.size(); ++i) {
            Gdiplus::PointF p = points[i];
            if (vertical) {
                const float center = static_cast<float>(g_settings.positionY);
                p.Y = center + (center - p.Y);
            } else {
                const float center = static_cast<float>(g_settings.positionX);
                p.X = center + (center - p.X);
            }
            lower.push_back(p);
        }

        path.AddCurve(
            points.data(), static_cast<INT>(points.size()), 0.35f);

        std::reverse(lower.begin(), lower.end());
        path.AddCurve(
            lower.data(), static_cast<INT>(lower.size()), 0.35f);
        path.CloseFigure();
    } else {
        path.AddCurve(
            points.data(), static_cast<INT>(points.size()), 0.35f);

        Gdiplus::PointF endBaseline = points.back();
        Gdiplus::PointF startBaseline = points.front();

        if (vertical) {
            endBaseline.Y = static_cast<float>(g_settings.positionY);
            startBaseline.Y = static_cast<float>(g_settings.positionY);
        } else {
            endBaseline.X = static_cast<float>(g_settings.positionX);
            startBaseline.X = static_cast<float>(g_settings.positionX);
        }

        path.AddLine(points.back(), endBaseline);
        path.AddLine(endBaseline, startBaseline);
        path.AddLine(startBaseline, points.front());
        path.CloseFigure();
    }

    Gdiplus::RectF bounds;
    path.GetBounds(&bounds);


    if (vertical) {
        bounds.X = std::min(
            bounds.X, static_cast<float>(g_settings.positionX));
        bounds.Width = std::max(
            bounds.Width, static_cast<float>(curveWidth));
    } else {
        bounds.Y = std::min(
            bounds.Y, static_cast<float>(g_settings.positionY));
        bounds.Height = std::max(
            bounds.Height, static_cast<float>(curveWidth));
    }

    float maxRatio = 0.0f;
    for (int i = 0; i < barCount; ++i) {
        maxRatio = std::max(
            maxRatio,
            std::clamp(g_currentHeights[i] / maxHeight, 0.0f, 1.0f));
    }

    const float colorT = powf(
        maxRatio, g_settings.gradientCurve);

    int alphaValue = std::clamp(
        255 * g_settings.acrylicOpacity / 100, 10, 255);

    if (g_settings.colorMode == 3) {
        const int minAlpha =
            (g_settings.dynamicAcrylicMinOpacity * 255) / 100;
        alphaValue = std::clamp(
            (minAlpha + static_cast<int>(
                (255 - minAlpha) * ApplyHeightCurve(
                        maxRatio,
                        g_settings.opacityCurveEnabled,
                        g_settings.opacityCurve))) *
                g_settings.acrylicOpacity / 100,
            10, 255);
    }

    DWORD color = GetVisualizerColorPrimary();
    if (g_settings.colorMode == 3)
        color = LerpColor(GetVisualizerColorPrimary(), GetVisualizerColorSecondary(), colorT);

    Gdiplus::SolidBrush solidBrush(
        Gdiplus::Color(
            static_cast<BYTE>(alphaValue),
            GetRValue(color),
            GetGValue(color),
            GetBValue(color)));

    Gdiplus::LinearGradientBrush* gradientBrush = nullptr;
    Gdiplus::Brush* brush = &solidBrush;

    Gdiplus::Color c1(
        static_cast<BYTE>(alphaValue),
        GetRValue(GetVisualizerColorPrimary()),
        GetGValue(GetVisualizerColorPrimary()),
        GetBValue(GetVisualizerColorPrimary()));
    Gdiplus::Color c2(
        static_cast<BYTE>(alphaValue),
        GetRValue(GetVisualizerColorSecondary()),
        GetGValue(GetVisualizerColorSecondary()),
        GetBValue(GetVisualizerColorSecondary()));

    if (g_settings.colorMode == 1) {
        if (vertical) {
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X + bounds.Width, bounds.Y),
                c1, c2);
        } else {
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X, bounds.Y + bounds.Height),
                c1, c2);
        }
        brush = gradientBrush;
    } else if (g_settings.colorMode == 6 || g_settings.colorMode == 8) {
        if (vertical) {
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X, bounds.Y + bounds.Height),
                c1, c2);
        } else {
            gradientBrush = new Gdiplus::LinearGradientBrush(
                Gdiplus::PointF(bounds.X, bounds.Y),
                Gdiplus::PointF(bounds.X + bounds.Width, bounds.Y),
                c1, c2);
        }
        brush = gradientBrush;
    } else if (g_settings.colorMode == 4) {
        const DWORD lighter =
            MixColor(color, RGB(255, 255, 255), 0.35f);
        gradientBrush = new Gdiplus::LinearGradientBrush(
            Gdiplus::PointF(bounds.X, bounds.Y),
            Gdiplus::PointF(bounds.X + bounds.Width, bounds.Y + bounds.Height),
            Gdiplus::Color(
                static_cast<BYTE>(std::clamp(alphaValue * 0.78f, 8.0f, 255.0f)),
                GetRValue(lighter), GetGValue(lighter), GetBValue(lighter)),
            Gdiplus::Color(
                static_cast<BYTE>(std::clamp(alphaValue * 0.35f, 5.0f, 255.0f)),
                GetRValue(color), GetGValue(color), GetBValue(color)));
        brush = gradientBrush;
    } else if (g_settings.colorMode == 5) {
        const DWORD lighter =
            MixColor(color, RGB(255, 255, 255), 0.58f);
        const DWORD darker =
            MixColor(color, RGB(0, 0, 0), 0.18f);
        gradientBrush = new Gdiplus::LinearGradientBrush(
            Gdiplus::PointF(bounds.X, bounds.Y),
            Gdiplus::PointF(bounds.X, bounds.Y + bounds.Height),
            Gdiplus::Color(
                static_cast<BYTE>(std::clamp(alphaValue * 0.76f, 8.0f, 255.0f)),
                GetRValue(lighter), GetGValue(lighter), GetBValue(lighter)),
            Gdiplus::Color(
                static_cast<BYTE>(std::clamp(alphaValue * 0.52f, 8.0f, 255.0f)),
                GetRValue(darker), GetGValue(darker), GetBValue(darker)));
        brush = gradientBrush;
    }

    graphics.FillPath(brush, &path);

    if (g_settings.borderEnabled) {
        DrawVisualizerPathBorder(graphics, path, bounds);
    }

    if (g_settings.colorMode == 4 || g_settings.colorMode == 5) {
        const BYTE highlightAlpha = static_cast<BYTE>(
            std::clamp(
                g_settings.glassHighlight *
                    ((g_settings.colorMode == 4) ? 1.8f : 1.5f),
                1.0f, 255.0f));
        Gdiplus::Pen pen(
            Gdiplus::Color(highlightAlpha, 255, 255, 255),
            1.2f);
        graphics.DrawPath(&pen, &path);
    }

    delete gradientBrush;
}


static AlbumPaletteGdi ExtractAlbumPaletteGdi(const std::vector<BYTE>& imageBytes) {
    AlbumPaletteGdi fallback{};
    if (imageBytes.empty())
        return fallback;

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, imageBytes.size());
    if (!hGlobal)
        return fallback;

    void* dst = GlobalLock(hGlobal);
    if (!dst) {
        GlobalFree(hGlobal);
        return fallback;
    }
    std::memcpy(dst, imageBytes.data(), imageBytes.size());
    GlobalUnlock(hGlobal);

    IStream* stream = nullptr;
    HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &stream);
    if (FAILED(hr) || !stream) {
        GlobalFree(hGlobal);
        return fallback;
    }

    Gdiplus::Bitmap bitmap(stream, FALSE);
    if (bitmap.GetLastStatus() != Gdiplus::Ok) {
        stream->Release();
        return fallback;
    }

    const UINT w = bitmap.GetWidth();
    const UINT h = bitmap.GetHeight();
    if (w == 0 || h == 0) {
        stream->Release();
        return fallback;
    }

    // Palette extraction runs only when the artwork changes, so the hot path is
    // deliberately capped. At most ~4096 pixels are sampled, regardless of the
    // thumbnail size. This gives us enough information for a strong palette while
    // keeping the album worker cheap even when a large artwork image is supplied.
    struct Sample {
        float l;
        float a;
        float b;
        float weight;
        float nx;
        float ny;
    };

    constexpr size_t kMaxSamples = 4096;
    constexpr int kClusterCount = 8;
    constexpr int kKMeansIterations = 5;

    std::vector<Sample> samples;
    samples.reserve(kMaxSamples);

    // The artwork is 8-bit per channel, so cache the expensive sRGB transfer
    // function once instead of calling pow() three times for every sample.
    static const std::array<float, 256> srgbToLinearLut = [] {
        std::array<float, 256> lut{};
        for (int i = 0; i < 256; ++i) {
            const float c = static_cast<float>(i) / 255.0f;
            lut[i] = c <= 0.04045f
                ? c / 12.92f
                : std::pow((c + 0.055f) / 1.055f, 2.4f);
        }
        return lut;
    }();

    auto rgbToOklab = [&](BYTE r8, BYTE g8, BYTE b8,
                          float& outL, float& outA, float& outB) {
        const float lr = srgbToLinearLut[r8];
        const float lg = srgbToLinearLut[g8];
        const float lb = srgbToLinearLut[b8];

        const float l =
            0.4122214708f * lr +
            0.5363325363f * lg +
            0.0514459929f * lb;
        const float m =
            0.2119034982f * lr +
            0.6806995451f * lg +
            0.1073969566f * lb;
        const float s =
            0.0883024619f * lr +
            0.2817188376f * lg +
            0.6299787005f * lb;

        const float l3 = std::cbrt(std::max(l, 0.0f));
        const float m3 = std::cbrt(std::max(m, 0.0f));
        const float s3 = std::cbrt(std::max(s, 0.0f));

        outL =
            0.2104542553f * l3 +
            0.7936177850f * m3 -
            0.0040720468f * s3;
        outA =
            1.9779984951f * l3 -
            2.4285922050f * m3 +
            0.4505937099f * s3;
        outB =
            0.0259040371f * l3 +
            0.7827717662f * m3 -
            0.8086757660f * s3;
    };

    auto oklabToRgb = [](float l, float a, float b,
                         float& outR, float& outG, float& outB) {
        const float l3 = l + 0.3963377774f * a + 0.2158037573f * b;
        const float m3 = l - 0.1055613458f * a - 0.0638541728f * b;
        const float s3 = l - 0.0894841775f * a - 1.2914855480f * b;

        const float l_ = l3 * l3 * l3;
        const float m_ = m3 * m3 * m3;
        const float s_ = s3 * s3 * s3;

        const float lr =
            4.0767416621f * l_ -
            3.3077115913f * m_ +
            0.2309699292f * s_;
        const float lg =
            -1.2684380046f * l_ +
            2.6097574011f * m_ -
            0.3413193965f * s_;
        const float lb =
            -0.0041960863f * l_ -
            0.7034186147f * m_ +
            1.7076147010f * s_;

        auto linearToSrgb = [](float c) -> float {
            c = std::clamp(c, 0.0f, 1.0f);
            return c <= 0.0031308f
                ? 12.92f * c
                : 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
        };

        outR = linearToSrgb(lr);
        outG = linearToSrgb(lg);
        outB = linearToSrgb(lb);
    };

    auto smoothStep = [](float edge0, float edge1, float x) -> float {
        if (edge1 <= edge0)
            return x >= edge1 ? 1.0f : 0.0f;
        const float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    };

    // Choose independent X/Y strides from the image aspect ratio. Unlike a
    // single stride based only on total pixel count, this stays near the 4096
    // sample cap even for unusually wide or tall artwork.
    const double aspect =
        static_cast<double>(w) / std::max(1.0, static_cast<double>(h));
    const int targetSampleWidth = std::max(
        1,
        std::min(
            static_cast<int>(w),
            static_cast<int>(std::ceil(
                std::sqrt(static_cast<double>(kMaxSamples) * aspect)))));
    // Allocate the remaining sample budget to Y. This guarantees that even
    // extreme aspect ratios stay within the intended ~4096 sample budget.
    const int targetSampleHeight = std::max(
        1,
        std::min(
            static_cast<int>(h),
            static_cast<int>(kMaxSamples) / targetSampleWidth));
    const int sampleStepX = std::max(
        1, static_cast<int>(std::ceil(
            static_cast<double>(w) / targetSampleWidth)));
    const int sampleStepY = std::max(
        1, static_cast<int>(std::ceil(
            static_cast<double>(h) / targetSampleHeight)));

    Gdiplus::BitmapData locked{};
    Gdiplus::Rect rect(0, 0, static_cast<int>(w), static_cast<int>(h));
    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead,
                        PixelFormat32bppARGB, &locked) != Gdiplus::Ok) {
        stream->Release();
        return fallback;
    }

    const BYTE* pixels = static_cast<const BYTE*>(locked.Scan0);
    const int stride = static_cast<int>(locked.Stride);

    double globalLumaSum = 0.0;
    double globalWeightSum = 0.0;

    for (UINT y = 0; y < h; y += static_cast<UINT>(sampleStepY)) {
        const BYTE* row = pixels + static_cast<int>(y) * stride;
        const float ny =
            (static_cast<float>(y) + 0.5f * sampleStepY) /
            std::max(1.0f, static_cast<float>(h)) * 2.0f - 1.0f;

        for (UINT x = 0; x < w; x += static_cast<UINT>(sampleStepX)) {
            const BYTE* px = row + static_cast<int>(x) * 4;
            const int a8 = px[3];
            if (a8 <= 0)
                continue;

            BYTE r8 = px[2];
            BYTE g8 = px[1];
            BYTE b8 = px[0];

            // GDI+ sources may contain premultiplied RGB for partially
            // transparent pixels. Restore straight-alpha channel values before
            // converting to OKLab. The correction is only done for alpha < 255,
            // so the overwhelmingly common opaque case remains branch-light.
            if (a8 < 255) {
                r8 = static_cast<BYTE>(std::min(
                    255, (static_cast<int>(r8) * 255 + a8 / 2) / a8));
                g8 = static_cast<BYTE>(std::min(
                    255, (static_cast<int>(g8) * 255 + a8 / 2) / a8));
                b8 = static_cast<BYTE>(std::min(
                    255, (static_cast<int>(b8) * 255 + a8 / 2) / a8));
            }

            float l = 0.0f;
            float oa = 0.0f;
            float ob = 0.0f;
            rgbToOklab(r8, g8, b8, l, oa, ob);

            const float chroma = std::sqrt(oa * oa + ob * ob);

            // Keep near-black/near-white pixels available for genuinely
            // monochrome artwork, but reduce their influence instead of deleting
            // them outright. This avoids throwing away useful detail from covers
            // with bright or dark themes.
            const float darkVisibility = smoothStep(0.02f, 0.16f, l);
            const float brightVisibility =
                1.0f - smoothStep(0.88f, 0.995f, l);
            const float brightnessFactor =
                0.28f + 0.72f * std::min(darkVisibility, brightVisibility);

            const float nx =
                (static_cast<float>(x) + 0.5f * sampleStepX) /
                std::max(1.0f, static_cast<float>(w)) * 2.0f - 1.0f;
            const float radial = std::clamp(
                std::sqrt(nx * nx + ny * ny) / std::sqrt(2.0f),
                0.0f, 1.0f);
            const float spatialWeight = 1.08f - 0.16f * radial;
            const float alphaWeight = static_cast<float>(a8) / 255.0f;

            // Strong chroma gets a little more influence during clustering so a
            // tiny, vivid accent is not swallowed by a huge neutral background.
            const float chromaScore = smoothStep(0.012f, 0.14f, chroma);
            const float baseWeight =
                brightnessFactor * spatialWeight * alphaWeight;
            const float characteristicWeight =
                (0.58f + 1.20f * chromaScore) * baseWeight;

            if (characteristicWeight <= 0.0f || baseWeight <= 0.0f)
                continue;

            samples.push_back({
                l, oa, ob, characteristicWeight, nx, ny});

            // Keep global lightness independent from the chroma preference used
            // by clustering. This makes the contrast bonus describe the artwork
            // itself instead of the already-biased palette candidate weights.
            globalLumaSum += static_cast<double>(l) * baseWeight;
            globalWeightSum += baseWeight;
        }
    }

    bitmap.UnlockBits(&locked);
    stream->Release();

    if (samples.empty() || globalWeightSum <= 0.0)
        return fallback;

    const float globalLuma = static_cast<float>(
        globalLumaSum / globalWeightSum);
    const int clusterCount = std::min(
        kClusterCount, static_cast<int>(samples.size()));

    struct Center {
        float l = 0.0f;
        float a = 0.0f;
        float b = 0.0f;
    };

    std::array<Center, kClusterCount> centers{};

    // Deterministic farthest-point seeding: the first seed is the strongest
    // individual colour sample; each later seed is chosen for perceptual
    // separation from the already selected seeds. This is stable, cheap and
    // avoids random clustering results between track changes.
    size_t firstIndex = 0;
    float firstScore = -1.0f;
    for (size_t i = 0; i < samples.size(); ++i) {
        const float chroma = std::sqrt(
            samples[i].a * samples[i].a + samples[i].b * samples[i].b);
        const float chromaScore = smoothStep(0.012f, 0.14f, chroma);
        const float score = samples[i].weight *
            (0.65f + 1.35f * chromaScore);
        if (score > firstScore) {
            firstScore = score;
            firstIndex = i;
        }
    }

    centers[0] = {
        samples[firstIndex].l,
        samples[firstIndex].a,
        samples[firstIndex].b};

    for (int c = 1; c < clusterCount; ++c) {
        size_t bestIndex = 0;
        float bestScore = -1.0f;

        for (size_t i = 0; i < samples.size(); ++i) {
            float minDistanceSq = 1.0e9f;
            for (int previous = 0; previous < c; ++previous) {
                const float dl = samples[i].l - centers[previous].l;
                const float da = samples[i].a - centers[previous].a;
                const float db = samples[i].b - centers[previous].b;
                const float distanceSq = dl * dl + da * da + db * db;
                minDistanceSq = std::min(minDistanceSq, distanceSq);
            }

            const float chroma = std::sqrt(
                samples[i].a * samples[i].a + samples[i].b * samples[i].b);
            const float chromaScore = smoothStep(0.012f, 0.14f, chroma);
            const float score = minDistanceSq * samples[i].weight *
                (0.55f + 0.90f * chromaScore);

            if (score > bestScore) {
                bestScore = score;
                bestIndex = i;
            }
        }

        centers[c] = {
            samples[bestIndex].l,
            samples[bestIndex].a,
            samples[bestIndex].b};
    }

    // Small fixed-size weighted k-means in OKLab. The cluster count and number of
    // iterations are intentionally bounded so accuracy improves without making
    // artwork changes expensive. No allocation occurs during the iterations.
    for (int iteration = 0; iteration < kKMeansIterations; ++iteration) {
        std::array<double, kClusterCount> sumWeight{};
        std::array<double, kClusterCount> sumL{};
        std::array<double, kClusterCount> sumA{};
        std::array<double, kClusterCount> sumB{};

        for (const auto& sample : samples) {
            int bestCluster = 0;
            float bestDistanceSq = 1.0e9f;

            for (int c = 0; c < clusterCount; ++c) {
                const float dl = sample.l - centers[c].l;
                const float da = sample.a - centers[c].a;
                const float db = sample.b - centers[c].b;
                const float distanceSq = dl * dl + da * da + db * db;
                if (distanceSq < bestDistanceSq) {
                    bestDistanceSq = distanceSq;
                    bestCluster = c;
                }
            }

            sumWeight[bestCluster] += sample.weight;
            sumL[bestCluster] += static_cast<double>(sample.l) * sample.weight;
            sumA[bestCluster] += static_cast<double>(sample.a) * sample.weight;
            sumB[bestCluster] += static_cast<double>(sample.b) * sample.weight;
        }

        for (int c = 0; c < clusterCount; ++c) {
            if (sumWeight[c] <= 0.0)
                continue;

            centers[c].l = static_cast<float>(sumL[c] / sumWeight[c]);
            centers[c].a = static_cast<float>(sumA[c] / sumWeight[c]);
            centers[c].b = static_cast<float>(sumB[c] / sumWeight[c]);
        }
    }

    struct ClusterStats {
        float score = 0.0f;
        float mass = 0.0f;
        float l = 0.0f;
        float a = 0.0f;
        float b = 0.0f;
        float spread = 0.0f;
        float chroma = 0.0f;
    };

    std::array<ClusterStats, kClusterCount> stats{};
    std::array<double, kClusterCount> mass{};
    std::array<double, kClusterCount> sumX{};
    std::array<double, kClusterCount> sumY{};
    std::array<double, kClusterCount> sumXX{};
    std::array<double, kClusterCount> sumYY{};

    for (const auto& sample : samples) {
        int bestCluster = 0;
        float bestDistanceSq = 1.0e9f;

        for (int c = 0; c < clusterCount; ++c) {
            const float dl = sample.l - centers[c].l;
            const float da = sample.a - centers[c].a;
            const float db = sample.b - centers[c].b;
            const float distanceSq = dl * dl + da * da + db * db;
            if (distanceSq < bestDistanceSq) {
                bestDistanceSq = distanceSq;
                bestCluster = c;
            }
        }

        const double weight = sample.weight;
        mass[bestCluster] += weight;
        sumX[bestCluster] += static_cast<double>(sample.nx) * weight;
        sumY[bestCluster] += static_cast<double>(sample.ny) * weight;
        sumXX[bestCluster] +=
            static_cast<double>(sample.nx) * sample.nx * weight;
        sumYY[bestCluster] +=
            static_cast<double>(sample.ny) * sample.ny * weight;
    }

    int primaryCluster = -1;
    float primaryScore = -1.0f;

    for (int c = 0; c < clusterCount; ++c) {
        if (mass[c] <= 0.0)
            continue;

        const float l = centers[c].l;
        const float a = centers[c].a;
        const float b = centers[c].b;
        const float chroma = std::sqrt(a * a + b * b);
        const float chromaScore = smoothStep(0.012f, 0.14f, chroma);

        const float meanX = static_cast<float>(sumX[c] / mass[c]);
        const float meanY = static_cast<float>(sumY[c] / mass[c]);
        const float varianceX = std::max(
            0.0f,
            static_cast<float>(sumXX[c] / mass[c]) - meanX * meanX);
        const float varianceY = std::max(
            0.0f,
            static_cast<float>(sumYY[c] / mass[c]) - meanY * meanY);
        const float spatialSpread = std::sqrt(varianceX + varianceY);

        // Broadly distributed colours are more representative of the whole
        // cover than a tiny logo/highlight, but the spread bonus is intentionally
        // mild so a genuinely important local accent can still win.
        const float spreadFactor =
            0.82f + 0.18f * std::clamp(spatialSpread / 0.55f, 0.0f, 1.0f);

        // Neutral colours remain valid for monochrome artwork, but saturated
        // colours receive enough preference to beat bland background clusters.
        const float neutralFactor =
            0.55f + 0.45f * chromaScore;

        // Protect dark shadows and pure highlights with a soft penalty instead
        // of hard filtering them out.
        const float darkVisibility = smoothStep(0.02f, 0.16f, l);
        const float brightVisibility =
            1.0f - smoothStep(0.88f, 0.995f, l);
        const float brightnessFactor =
            0.28f + 0.72f * std::min(darkVisibility, brightVisibility);

        const float contrast = std::clamp(
            std::fabs(l - globalLuma) / 0.50f, 0.0f, 1.0f);
        const float contrastFactor = 0.90f + 0.30f * contrast;

        // Coverage is still the strongest signal; chroma, spread and contrast
        // are controlled bonuses rather than replacements for true dominance.
        const float score =
            static_cast<float>(mass[c]) *
            (0.60f + 1.45f * chromaScore) *
            neutralFactor *
            brightnessFactor *
            contrastFactor *
            spreadFactor;

        stats[c] = {
            score,
            static_cast<float>(mass[c]),
            l,
            a,
            b,
            spatialSpread,
            chroma};

        if (score > primaryScore) {
            primaryScore = score;
            primaryCluster = c;
        }
    }

    if (primaryCluster < 0)
        return fallback;

    auto colorFromLab = [&](const ClusterStats& cluster) -> DWORD {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        oklabToRgb(cluster.l, cluster.a, cluster.b, r, g, b);
        return RGB(
            static_cast<BYTE>(std::lround(std::clamp(r, 0.0f, 1.0f) * 255.0f)),
            static_cast<BYTE>(std::lround(std::clamp(g, 0.0f, 1.0f) * 255.0f)),
            static_cast<BYTE>(std::lround(std::clamp(b, 0.0f, 1.0f) * 255.0f)));
    };

    AlbumPaletteGdi result{
        colorFromLab(stats[primaryCluster]),
        colorFromLab(stats[primaryCluster])};

    int secondaryCluster = -1;
    int fallbackSecondaryCluster = -1;
    float bestSecondaryScore = -1.0f;
    float bestFallbackSecondaryScore = -1.0f;

    const ClusterStats& primary = stats[primaryCluster];
    const float primaryHue = std::atan2(primary.b, primary.a);

    constexpr float kMinSecondaryMassRatio = 0.025f;
    constexpr float kDistinctHueThreshold = 0.18f; // ~32 degrees
    constexpr float kStrongHueThreshold = 0.35f;   // ~63 degrees

    for (int c = 0; c < clusterCount; ++c) {
        if (c == primaryCluster || stats[c].mass <= 0.0f)
            continue;

        const ClusterStats& candidate = stats[c];

        // Allow somewhat smaller accent clusters here. A real secondary colour
        // can be much less common than Primary, while tiny logos/highlights are
        // still filtered out.
        if (candidate.mass < primary.mass * kMinSecondaryMassRatio)
            continue;

        const float dl = candidate.l - primary.l;
        const float da = candidate.a - primary.a;
        const float db = candidate.b - primary.b;
        const float distance = std::sqrt(dl * dl + da * da + db * db);

        if (distance < 0.055f)
            continue;

        const float distanceScore =
            std::clamp(distance / 0.32f, 0.0f, 1.0f);

        float hueSeparation = 0.0f;
        const bool bothChromatic =
            primary.chroma > 0.025f && candidate.chroma > 0.025f;

        if (bothChromatic) {
            const float candidateHue = std::atan2(candidate.b, candidate.a);
            float hueDiff = std::fabs(candidateHue - primaryHue);
            constexpr float kPi = 3.14159265358979323846f;
            if (hueDiff > kPi)
                hueDiff = 2.0f * kPi - hueDiff;
            hueSeparation = std::clamp(hueDiff / kPi, 0.0f, 1.0f);
        }

        const float lightnessSeparation = std::clamp(
            std::fabs(candidate.l - primary.l) / 0.50f,
            0.0f, 1.0f);

        // Keep a fallback based on overall usefulness, but make the preferred
        // path explicitly favour a genuinely different hue. This prevents a
        // red+blue cover from ending up red+slightly-different-red merely because
        // several red clusters have more pixels.
        const float fallbackDistinctiveness =
            0.55f +
            0.90f * distanceScore +
            0.65f * hueSeparation +
            0.18f * lightnessSeparation;
        const float fallbackScore = candidate.score * fallbackDistinctiveness;

        if (fallbackScore > bestFallbackSecondaryScore) {
            bestFallbackSecondaryScore = fallbackScore;
            fallbackSecondaryCluster = c;
        }

        // When both colours are chromatic, a small hue separation is treated as
        // the same colour family. We deliberately do not let such a cluster win
        // while a sufficiently substantial, genuinely different hue exists.
        const bool sufficientlyDifferentHue =
            !bothChromatic || hueSeparation >= kDistinctHueThreshold;

        if (!sufficientlyDifferentHue)
            continue;

        // Hue diversity is intentionally stronger than raw cluster mass here.
        // This makes an actual blue/teal accent beat a second red cluster when
        // the primary colour is red, while still respecting meaningful coverage.
        const float hueDiversityBonus =
            0.70f +
            1.90f * hueSeparation +
            0.65f * std::clamp(
                (hueSeparation - kDistinctHueThreshold) /
                (1.0f - kDistinctHueThreshold),
                0.0f, 1.0f);

        // Give very strongly separated hues an additional modest preference, but
        // do not ignore useful analogous gradients entirely. The coverage/score
        // still matters after hue diversity has passed the gate.
        const float strongHueBonus =
            hueSeparation >= kStrongHueThreshold ? 1.20f : 1.0f;

        const float score =
            candidate.score *
            (0.80f + 0.85f * distanceScore +
             0.20f * lightnessSeparation) *
            hueDiversityBonus *
            strongHueBonus;

        if (score > bestSecondaryScore) {
            bestSecondaryScore = score;
            secondaryCluster = c;
        }
    }

    // Prefer a hue-diverse candidate whenever one exists. Only fall back to a
    // second shade of the same family when the artwork truly does not contain a
    // sufficiently distinct alternative colour.
    if (secondaryCluster < 0)
        secondaryCluster = fallbackSecondaryCluster;

    if (secondaryCluster >= 0) {
        result.secondary = colorFromLab(stats[secondaryCluster]);
    }

    return result;
}

static size_t HashAlbumBytes(const std::vector<BYTE>& bytes) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < bytes.size(); i += 1024) {
        hash ^= static_cast<uint64_t>(bytes[i]);
        hash *= 1099511628211ULL;
    }
    return static_cast<size_t>(hash);
}

static void UpdateAlbumPaletteFromBytes(const std::vector<BYTE>& thumbBytes, size_t hash) {
    if (thumbBytes.empty() || hash == 0) {
        std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
        g_albumPalette = AlbumPaletteGdi{};
        g_albumPaletteHash = 0;
        g_albumPalettePrimaryFast.store(g_albumPalette.primary, std::memory_order_release);
        g_albumPaletteSecondaryFast.store(g_albumPalette.secondary, std::memory_order_release);
        g_albumPaletteRevision.fetch_add(1, std::memory_order_release);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
        if (g_albumPaletteHash == hash)
            return;
    }

    AlbumPaletteGdi palette = ExtractAlbumPaletteGdi(thumbBytes);
    {
        std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
        g_albumPalette = palette;
        g_albumPaletteHash = hash;
        g_albumPalettePrimaryFast.store(palette.primary, std::memory_order_release);
        g_albumPaletteSecondaryFast.store(palette.secondary, std::memory_order_release);
        g_albumPaletteRevision.fetch_add(1, std::memory_order_release);
    }
}

static DWORD GetAlbumPalettePrimary() {
    return g_albumPalettePrimaryFast.load(std::memory_order_acquire);
}

static DWORD GetAlbumPaletteSecondary() {
    return g_albumPaletteSecondaryFast.load(std::memory_order_acquire);
}

static void UpdateAlbumArtworkFromBytes(const std::vector<BYTE>& bytes, size_t hash) {
    std::lock_guard<std::mutex> lock(g_albumArtworkMutex);
    if (hash == g_albumArtworkHash)
        return;
    g_albumArtworkHash = hash;
    g_albumArtworkBytes = bytes;
    g_albumArtworkRevision.fetch_add(1, std::memory_order_release);
}

static void ClearAlbumArtworkCache() {
    std::lock_guard<std::mutex> lock(g_albumArtworkMutex);
    g_albumArtworkHash = 0;
    g_albumArtworkBytes.clear();
}

static void DestroyAlbumArtworkImage() {
    if (g_albumArtworkBitmap) {
        delete g_albumArtworkBitmap;
        g_albumArtworkBitmap = nullptr;
    }
    if (g_albumArtworkStream) {
        g_albumArtworkStream->Release();
        g_albumArtworkStream = nullptr;
    }
    g_albumArtworkLoadedHash = 0;
}

static void EnsureAlbumArtworkLoaded() {
    if (!g_settings.albumWidgetEnabled)
        return;

    size_t hash = 0;
    std::vector<BYTE> bytes;
    {
        std::lock_guard<std::mutex> lock(g_albumArtworkMutex);
        hash = g_albumArtworkHash;
        if (hash == 0 || g_albumArtworkBytes.empty()) {
            if (g_albumArtworkLoadedHash != 0)
                DestroyAlbumArtworkImage();
            return;
        }
        if (hash == g_albumArtworkLoadedHash && g_albumArtworkBitmap)
            return;
        bytes = g_albumArtworkBytes;
    }

    DestroyAlbumArtworkImage();

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!hGlobal)
        return;

    void* dst = GlobalLock(hGlobal);
    if (!dst) {
        GlobalFree(hGlobal);
        return;
    }
    std::memcpy(dst, bytes.data(), bytes.size());
    GlobalUnlock(hGlobal);

    IStream* stream = nullptr;
    if (FAILED(CreateStreamOnHGlobal(hGlobal, TRUE, &stream)) || !stream) {
        GlobalFree(hGlobal);
        return;
    }

    Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromStream(stream, FALSE);
    if (!image || image->GetLastStatus() != Gdiplus::Ok ||
        image->GetWidth() == 0 || image->GetHeight() == 0) {
        delete image;
        stream->Release();
        return;
    }

    g_albumArtworkStream = stream;
    g_albumArtworkBitmap = image;
    g_albumArtworkLoadedHash = hash;
}

static void DestroyAlbumWidgetCache() {
    if (g_albumWidgetCacheBitmap) {
        delete g_albumWidgetCacheBitmap;
        g_albumWidgetCacheBitmap = nullptr;
    }
    g_albumWidgetCacheHash = 0;
}

static Gdiplus::InterpolationMode GetAlbumWidgetInterpolationMode(int quality) {
    switch (std::clamp(quality, 0, 3)) {
    case 0:
        return Gdiplus::InterpolationModeNearestNeighbor;
    case 2:
        return Gdiplus::InterpolationModeHighQualityBilinear;
    case 3:
        return Gdiplus::InterpolationModeHighQualityBicubic;
    default:
        // The old path inherited GDI+'s default Bilinear interpolation.
        // Keep that as the default so the normal visual appearance stays close.
        return Gdiplus::InterpolationModeBilinear;
    }
}

static size_t BuildAlbumWidgetCacheHash(const VisualizerSettings& settings) {
    size_t hash = g_albumArtworkRevision.load(std::memory_order_acquire);
    auto combine = [&hash](size_t value) {
        hash ^= value + static_cast<size_t>(0x9e3779b9) +
                (hash << 6) + (hash >> 2);
    };

    // The artwork revision already invalidates this cache whenever the track
    // changes. Do not tie the widget cache to palette revisions: the album
    // artwork can be identical while an unrelated album-color consumer causes
    // the palette cache to refresh. The standalone album widget does not need
    // that rebuild.
    combine(static_cast<size_t>(settings.albumWidgetWidth));
    combine(static_cast<size_t>(settings.albumWidgetHeight));
    combine(static_cast<size_t>(settings.albumWidgetQuality));
    combine(static_cast<size_t>(settings.albumWidgetCornerRadius));
    combine(static_cast<size_t>(settings.albumWidgetOpacity));
    combine(static_cast<size_t>(settings.albumWidgetAttachment));

    if (settings.albumWidgetAttachment == 1) {
        combine(static_cast<size_t>(settings.lyricsBackgroundEnabled));
        combine(static_cast<size_t>(settings.lyricsBorderEnabled));
        combine(static_cast<size_t>(settings.lyricsBorderMode));
        combine(static_cast<size_t>(settings.lyricsBorderThickness));
        combine(static_cast<size_t>(settings.lyricsBorderOpacity));
        combine(static_cast<size_t>(settings.lyricsRounding));
        combine(static_cast<size_t>(settings.lyricsBorderColor1));
        combine(static_cast<size_t>(settings.lyricsBorderColor2));
    } else if (settings.albumWidgetAttachment == 2) {
        combine(static_cast<size_t>(settings.backgroundBorderEnabled));
        combine(static_cast<size_t>(settings.backgroundBorderMode));
        combine(static_cast<size_t>(settings.backgroundBorderThickness));
        combine(static_cast<size_t>(settings.backgroundBorderOpacity));
        combine(static_cast<size_t>(settings.backgroundCornerRadius));
        combine(static_cast<size_t>(settings.backgroundBorderColor1));
        combine(static_cast<size_t>(settings.backgroundBorderColor2));
    }

    return hash;
}

static bool EnsureAlbumWidgetCache(const VisualizerSettings& settings) {
    if (!settings.albumWidgetEnabled || settings.albumWidgetOpacity <= 0) {
        DestroyAlbumWidgetCache();
        return false;
    }

    const size_t cacheHash = BuildAlbumWidgetCacheHash(settings);
    if (g_albumWidgetCacheBitmap && g_albumWidgetCacheHash == cacheHash &&
        static_cast<int>(g_albumWidgetCacheBitmap->GetWidth()) == settings.albumWidgetWidth &&
        static_cast<int>(g_albumWidgetCacheBitmap->GetHeight()) == settings.albumWidgetHeight) {
        return true;
    }

    // Only touch the artwork mutex/decode path when the cache is actually stale.
    EnsureAlbumArtworkLoaded();
    if (!g_albumArtworkBitmap || g_albumArtworkLoadedHash == 0)
        return false;

    const int w = settings.albumWidgetWidth;
    const int h = settings.albumWidgetHeight;
    if (w <= 0 || h <= 0)
        return false;

    auto* cache = new Gdiplus::Bitmap(w, h, PixelFormat32bppARGB);
    if (!cache || cache->GetLastStatus() != Gdiplus::Ok) {
        delete cache;
        return false;
    }

    Gdiplus::Graphics cacheGraphics(cache);
    cacheGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    cacheGraphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    cacheGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
    cacheGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));

    const RECT localRect{0, 0, w, h};
    const BYTE alpha = static_cast<BYTE>(
        std::clamp(settings.albumWidgetOpacity, 0, 100) * 255 / 100);
    const int radius = std::clamp(
        settings.albumWidgetCornerRadius, 0, std::min(w, h) / 2);

    Gdiplus::GraphicsPath path;
    AddRoundedRectSubpath(
        path, 0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h),
        static_cast<float>(radius));

    Gdiplus::SolidBrush fallbackBrush(Gdiplus::Color(alpha, 20, 20, 23));
    cacheGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
    cacheGraphics.FillPath(&fallbackBrush, &path);

    const UINT imageW = g_albumArtworkBitmap->GetWidth();
    const UINT imageH = g_albumArtworkBitmap->GetHeight();
    if (imageW > 0 && imageH > 0) {
        const float scale = std::max(
            static_cast<float>(w) / static_cast<float>(imageW),
            static_cast<float>(h) / static_cast<float>(imageH));
        const float drawW = static_cast<float>(imageW) * scale;
        const float drawH = static_cast<float>(imageH) * scale;
        const float drawX = (static_cast<float>(w) - drawW) * 0.5f;
        const float drawY = (static_cast<float>(h) - drawH) * 0.5f;

        Gdiplus::GraphicsState clipState = cacheGraphics.Save();
        cacheGraphics.SetClip(&path, Gdiplus::CombineModeReplace);
        cacheGraphics.SetInterpolationMode(
            GetAlbumWidgetInterpolationMode(settings.albumWidgetQuality));

        Gdiplus::ImageAttributes attrs;
        Gdiplus::ColorMatrix matrix = {
            1, 0, 0, 0, 0,
            0, 1, 0, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 0, alpha / 255.0f, 0,
            0, 0, 0, 0, 1};
        attrs.SetColorMatrix(
            &matrix,
            Gdiplus::ColorMatrixFlagsDefault,
            Gdiplus::ColorAdjustTypeBitmap);
        cacheGraphics.DrawImage(
            g_albumArtworkBitmap,
            Gdiplus::RectF(drawX, drawY, drawW, drawH),
            0, 0,
            static_cast<Gdiplus::REAL>(imageW),
            static_cast<Gdiplus::REAL>(imageH),
            Gdiplus::UnitPixel, &attrs);
        cacheGraphics.Restore(clipState);
    }

    if (settings.albumWidgetAttachment == 0) {
        Gdiplus::GraphicsPath innerPath;
        const RECT innerRect{8, 8, w - 8, h - 8};
        if (innerRect.right > innerRect.left && innerRect.bottom > innerRect.top) {
            AddRoundedRectSubpath(
                innerPath,
                static_cast<float>(innerRect.left),
                static_cast<float>(innerRect.top),
                static_cast<float>(innerRect.right - innerRect.left),
                static_cast<float>(innerRect.bottom - innerRect.top),
                static_cast<float>(std::max(0, radius - 4)));
            Gdiplus::Pen innerPen(
                Gdiplus::Color(static_cast<BYTE>(alpha * 0.22f), 255, 255, 255), 1.0f);
            cacheGraphics.DrawPath(&innerPen, &innerPath);
        }
    } else {
        DrawAlbumWidgetBorder(cacheGraphics, localRect, settings);
    }

    DestroyAlbumWidgetCache();
    g_albumWidgetCacheBitmap = cache;
    g_albumWidgetCacheHash = cacheHash;
    return true;
}

static winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession
FindLyricsSession(
    const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager& manager,
    const std::wstring& executableName,
    std::wstring& pinnedSourceAppUserModelId);

template <typename TAsync>
static bool WaitWinrtAsync(
    const TAsync& operation,
    HANDLE stopEvent,
    std::chrono::milliseconds timeout) {
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
        operation.Cancel();
        return false;
    }

    const winrt::Windows::Foundation::AsyncStatus status =
        operation.wait_for(timeout);

    if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
        return true;

    operation.Cancel();
    return false;
}

static DWORD WINAPI AlbumColorThreadProc(LPVOID) {
    bool winrtApartmentInitialized = false;
    Gdiplus::GdiplusStartupInput gdiplusInput;
    ULONG_PTR albumGdiplusToken = 0;
    if (Gdiplus::GdiplusStartup(&albumGdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok)
        albumGdiplusToken = 0;

    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        winrtApartmentInitialized = true;
        auto managerOperation =
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        if (!WaitWinrtAsync(managerOperation, g_hAlbumColorStopEvent, std::chrono::milliseconds(1500)))
            throw winrt::hresult_canceled();
        auto manager = managerOperation.GetResults();

        std::wstring pinnedSourceAppUserModelId;
        winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession cachedSession = nullptr;
        ULONGLONG lastSessionLookupMs = 0;
        std::wstring lastSessionLookupKey;

        std::wstring cachedTitle;
        std::wstring cachedArtist;
        std::wstring cachedAlbum;
        std::wstring cachedTrackKey;
        winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionMediaProperties
            cachedProperties = nullptr;
        ULONGLONG lastMetadataMs = 0;
        ULONGLONG lastThumbnailAttemptMs = 0;
        bool thumbnailAvailableForTrack = false;
        bool albumHadSession = false;
        bool previousPaletteNeeded = false;

        // Keep this worker polling instead of wiring long-lived WinRT event
        // tokens. The mod is injected into Explorer and can be reconfigured or
        // unloaded while the shell is rebuilding; the existing cancellable
        // async waits make this lifecycle predictable without callback-token
        // revocation races. Metadata is cheap enough to refresh at most once
        // per second, while the potentially large thumbnail is read only when
        // the track changes or after a rare retry interval. Album-derived color
        // extraction is skipped entirely when the album widget is the only
        // feature consuming artwork.
        while (g_albumColorRunning.load(std::memory_order_acquire)) {
            const VisualizerSettings settings = GetSettingsSnapshot();
            const bool paletteNeeded = IsAlbumPaletteMode();
            try {
                using Session = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession;
                Session session = nullptr;
                const ULONGLONG nowMs = GetTickCount64();
                const std::wstring lookupKey =
                    settings.audioSource == 1 ? settings.audioApplicationName : L"<system>";

                if (lookupKey != lastSessionLookupKey ||
                    nowMs - lastSessionLookupMs >= 750) {
                    lastSessionLookupKey = lookupKey;
                    lastSessionLookupMs = nowMs;

                    if (settings.audioSource == 1) {
                        if (!settings.audioApplicationName.empty()) {
                            cachedSession = FindLyricsSession(
                                manager,
                                settings.audioApplicationName,
                                pinnedSourceAppUserModelId);
                        } else {
                            pinnedSourceAppUserModelId.clear();
                            cachedSession = nullptr;
                        }
                    } else {
                        cachedSession = manager.GetCurrentSession();
                        pinnedSourceAppUserModelId.clear();
                    }
                }
                session = cachedSession;

                if (!session) {
                    cachedTitle.clear();
                    cachedArtist.clear();
                    cachedAlbum.clear();
                    cachedTrackKey.clear();
                    cachedProperties = nullptr;
                    lastMetadataMs = 0;
                    thumbnailAvailableForTrack = false;
                    if (albumHadSession) {
                        albumHadSession = false;
                        UpdateAlbumArtworkFromBytes({}, 0);
                        if (paletteNeeded)
                            UpdateAlbumPaletteFromBytes({}, 0);
                    }
                } else {
                    albumHadSession = true;
                    bool refreshMetadata =
                        lastMetadataMs == 0 || nowMs - lastMetadataMs >= 1000;
                    if (refreshMetadata) {
                        lastMetadataMs = nowMs;
                        try {
                            auto propsOperation = session.TryGetMediaPropertiesAsync();
                            if (!WaitWinrtAsync(
                                    propsOperation, g_hAlbumColorStopEvent,
                                    std::chrono::milliseconds(1500)))
                                throw winrt::hresult_canceled();
                            auto props = propsOperation.GetResults();
                            if (props) {
                                cachedProperties = props;
                                cachedTitle = props.Title().c_str();
                                cachedArtist = props.Artist().c_str();
                                cachedAlbum = props.AlbumTitle().c_str();
                            }
                        } catch (...) {
                        }
                    }

                    const std::wstring trackKey =
                        cachedArtist + L"\n" + cachedTitle + L"\n" + cachedAlbum;
                    const bool trackChanged = !trackKey.empty() && trackKey != cachedTrackKey;
                    if (trackChanged) {
                        cachedTrackKey = trackKey;
                        thumbnailAvailableForTrack = false;
                    }

                    // Do not reopen/read the thumbnail while the track is unchanged.
                    // A failed/missing cover is retried only every 10 seconds.
                    if (trackChanged ||
                        (!thumbnailAvailableForTrack &&
                         nowMs - lastThumbnailAttemptMs >= 10000)) {
                        lastThumbnailAttemptMs = nowMs;

                        std::vector<BYTE> thumbBytes;
                        try {
                            // Reuse the MediaProperties object already fetched by
                            // the metadata cache. The old path performed a second
                            // TryGetMediaPropertiesAsync() here on every artwork
                            // attempt, doubling the expensive WinRT metadata work.
                            auto props = cachedProperties;
                            if (props) {
                                if (auto thumbRef = props.Thumbnail()) {
                                        auto streamOperation = thumbRef.OpenReadAsync();
                                        if (WaitWinrtAsync(
                                                streamOperation, g_hAlbumColorStopEvent,
                                                std::chrono::milliseconds(1500))) {
                                            auto stream = streamOperation.GetResults();
                                            if (stream) {
                                                const UINT64 size = stream.Size();
                                                if (size > 0 && size <= 4ULL * 1024ULL * 1024ULL) {
                                                    winrt::Windows::Storage::Streams::DataReader reader(stream);
                                                    auto loadOperation = reader.LoadAsync(static_cast<UINT32>(size));
                                                    if (WaitWinrtAsync(
                                                            loadOperation, g_hAlbumColorStopEvent,
                                                            std::chrono::milliseconds(1500))) {
                                                        thumbBytes.resize(static_cast<size_t>(size));
                                                        reader.ReadBytes(winrt::array_view<BYTE>(thumbBytes));
                                                        reader.DetachStream();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                        } catch (...) {
                            thumbBytes.clear();
                        }

                        if (!thumbBytes.empty()) {
                            const size_t thumbHash = HashAlbumBytes(thumbBytes);
                            thumbnailAvailableForTrack = true;
                            UpdateAlbumArtworkFromBytes(thumbBytes, thumbHash);
                            if (paletteNeeded)
                                UpdateAlbumPaletteFromBytes(thumbBytes, thumbHash);
                        } else {
                            thumbnailAvailableForTrack = false;
                            UpdateAlbumArtworkFromBytes({}, 0);
                            if (paletteNeeded)
                                UpdateAlbumPaletteFromBytes({}, 0);
                        }
                    }

                    // If the user enables an album-derived color mode while the
                    // worker is already alive only because of Album Widget, build
                    // the palette once from the already cached artwork instead of
                    // waiting until the next track. This is a one-time copy on the
                    // settings transition, not a polling cost.
                    if (paletteNeeded && !previousPaletteNeeded) {
                        std::vector<BYTE> currentArtwork;
                        size_t currentArtworkHash = 0;
                        {
                            std::lock_guard<std::mutex> lock(g_albumArtworkMutex);
                            currentArtworkHash = g_albumArtworkHash;
                            if (currentArtworkHash != 0 && !g_albumArtworkBytes.empty())
                                currentArtwork = g_albumArtworkBytes;
                        }
                        if (!currentArtwork.empty() && currentArtworkHash != 0)
                            UpdateAlbumPaletteFromBytes(currentArtwork, currentArtworkHash);
                    }
                }
            } catch (...) {
                // Keep the last valid artwork on transient WinRT failures.
            }

            previousPaletteNeeded = paletteNeeded;

            if (g_hAlbumColorStopEvent) {
                if (WaitForSingleObject(g_hAlbumColorStopEvent, 700) == WAIT_OBJECT_0)
                    break;
            } else {
                Sleep(700);
            }
        }
    } catch (...) {
        UpdateAlbumArtworkFromBytes({}, 0);
        UpdateAlbumPaletteFromBytes({}, 0);
    }

    if (winrtApartmentInitialized)
        winrt::uninit_apartment();

    if (albumGdiplusToken)
        Gdiplus::GdiplusShutdown(albumGdiplusToken);

    return 0;
}

static void StartAlbumColorCapture() {
    if (g_albumColorRunning.exchange(true, std::memory_order_acq_rel))
        return;

    if (!g_hAlbumColorStopEvent)
        g_hAlbumColorStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_hAlbumColorStopEvent) {
        g_albumColorRunning.store(false, std::memory_order_release);
        return;
    }

    g_hAlbumColorThread = CreateThread(
        nullptr, 0, AlbumColorThreadProc, nullptr, 0, nullptr);
    if (!g_hAlbumColorThread) {
        g_albumColorRunning.store(false, std::memory_order_release);
        CloseHandle(g_hAlbumColorStopEvent);
        g_hAlbumColorStopEvent = nullptr;
    }
}

static void StopAlbumColorCapture() {
    if (!g_albumColorRunning.exchange(false, std::memory_order_acq_rel))
        return;

    if (g_hAlbumColorStopEvent)
        SetEvent(g_hAlbumColorStopEvent);

    if (g_hAlbumColorThread) {
        WaitForSingleObject(g_hAlbumColorThread, INFINITE);
        CloseHandle(g_hAlbumColorThread);
        g_hAlbumColorThread = nullptr;
    }

    if (g_hAlbumColorStopEvent) {
        CloseHandle(g_hAlbumColorStopEvent);
        g_hAlbumColorStopEvent = nullptr;
    }

    UpdateAlbumArtworkFromBytes({}, 0);
    UpdateAlbumPaletteFromBytes({}, 0);
}

static std::string WideToUtf8(const std::wstring& value) {
    if (value.empty())
        return {};
    const int len = WideCharToMultiByte(CP_UTF8, 0, value.data(),
        static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    if (len <= 0)
        return {};
    std::string out(static_cast<size_t>(len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.data(),
        static_cast<int>(value.size()), out.data(), len, nullptr, nullptr);
    return out;
}

static std::wstring Utf8ToWide(const std::string& value) {
    if (value.empty())
        return {};
    const int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        value.data(), static_cast<int>(value.size()), nullptr, 0);
    if (len <= 0)
        return {};
    std::wstring out(static_cast<size_t>(len), L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        value.data(), static_cast<int>(value.size()), out.data(), len);
    return out;
}

static std::string PercentEncodeUtf8(const std::wstring& value) {
    static const char hex[] = "0123456789ABCDEF";
    const std::string utf8 = WideToUtf8(value);
    std::string out;
    out.reserve(utf8.size() * 3);
    for (unsigned char c : utf8) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' ||
            c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        } else {
            out.push_back('%');
            out.push_back(hex[c >> 4]);
            out.push_back(hex[c & 15]);
        }
    }
    return out;
}

static bool HttpGetUtf8(const std::wstring& host, const std::wstring& path,
                        std::string& response, HANDLE stopEvent) {
    response.clear();

    constexpr DWORD kHttpTimeoutMs = 1000;
    HINTERNET session = WinHttpOpen(
        L"Windhawk Desktop Audio Visualizer/1.1 (lyrics widget)",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!session)
        return false;

    WinHttpSetTimeouts(session, kHttpTimeoutMs, kHttpTimeoutMs,
                       kHttpTimeoutMs, kHttpTimeoutMs);

    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
        WinHttpCloseHandle(session);
        return false;
    }

    HINTERNET connect = WinHttpConnect(
        session, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!connect) {
        WinHttpCloseHandle(session);
        return false;
    }

    HINTERNET request = WinHttpOpenRequest(
        connect, L"GET", path.c_str(), nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!request) {
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return false;
    }

    WinHttpSetTimeouts(request, kHttpTimeoutMs, kHttpTimeoutMs,
                       kHttpTimeoutMs, kHttpTimeoutMs);

    // Deliberately keep synchronous WinHTTP ownership local to this worker.
    // Closing a synchronous request from StopLyricsCapture while another thread
    // is inside WinHttpSendRequest/ReceiveResponse is a concurrency hazard;
    // finite per-operation timeouts plus the stop checks between operations are
    // safer here than cross-thread handle invalidation.
    const bool sent =
        WinHttpSendRequest(
            request,
            L"Accept: application/json\r\n",
            -1L,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0) &&
        WinHttpReceiveResponse(request, nullptr);

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (sent && !(stopEvent &&
                  WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)) {
        WinHttpQueryHeaders(
            request,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX,
            &statusCode,
            &statusSize,
            WINHTTP_NO_HEADER_INDEX);
    }

    bool ok = sent && statusCode == 200 &&
        !(stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0);

    if (ok) {
        for (;;) {
            if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
                ok = false;
                break;
            }

            DWORD available = 0;
            if (!WinHttpQueryDataAvailable(request, &available) || available == 0)
                break;

            std::string chunk(static_cast<size_t>(available), '\0');
            DWORD read = 0;
            if (!WinHttpReadData(request, chunk.data(), available, &read) || read == 0) {
                ok = false;
                break;
            }

            chunk.resize(read);
            response += chunk;
            if (response.size() > 2 * 1024 * 1024) {
                ok = false;
                response.clear();
                break;
            }
        }
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return ok && !response.empty();
}

static bool JsonGetString(const std::string& json, const char* key, std::string& out) {
    const std::string needle = std::string("\"") + key + "\"";
    const size_t keyPos = json.find(needle);
    if (keyPos == std::string::npos)
        return false;
    size_t colon = json.find(':', keyPos + needle.size());
    if (colon == std::string::npos)
        return false;
    size_t quote = colon + 1;
    while (quote < json.size() && (json[quote] == ' ' || json[quote] == '\t' || json[quote] == '\r' || json[quote] == '\n'))
        ++quote;
    if (quote >= json.size() || json[quote] != '\"')
        return false;
    ++quote;
    std::string value;
    bool escape = false;
    for (size_t i = quote; i < json.size(); ++i) {
        const char c = json[i];
        if (escape) {
            switch (c) {
                case '"': value.push_back('"'); break;
                case '\\': value.push_back('\\'); break;
                case '/': value.push_back('/'); break;
                case 'b': value.push_back('\b'); break;
                case 'f': value.push_back('\f'); break;
                case 'n': value.push_back('\n'); break;
                case 'r': value.push_back('\r'); break;
                case 't': value.push_back('\t'); break;
                case 'u': {
                    if (i + 4 >= json.size()) return false;
                    unsigned int code = 0;
                    for (int j = 1; j <= 4; ++j) {
                        char h = json[i + j];
                        code <<= 4;
                        if (h >= '0' && h <= '9') code |= h - '0';
                        else if (h >= 'a' && h <= 'f') code |= h - 'a' + 10;
                        else if (h >= 'A' && h <= 'F') code |= h - 'A' + 10;
                        else return false;
                    }
                    char utf8[4]{};
                    int count = 0;
                    if (code <= 0x7F) { utf8[0] = static_cast<char>(code); count = 1; }
                    else if (code <= 0x7FF) { utf8[0] = static_cast<char>(0xC0 | (code >> 6)); utf8[1] = static_cast<char>(0x80 | (code & 0x3F)); count = 2; }
                    else { utf8[0] = static_cast<char>(0xE0 | (code >> 12)); utf8[1] = static_cast<char>(0x80 | ((code >> 6) & 0x3F)); utf8[2] = static_cast<char>(0x80 | (code & 0x3F)); count = 3; }
                    value.append(utf8, utf8 + count);
                    i += 4;
                    break;
                }
                default: value.push_back(c); break;
            }
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else if (c == '"') {
            out = value;
            return true;
        } else {
            value.push_back(c);
        }
    }
    return false;
}

static bool IsFiniteDouble(double value) {
    return std::isfinite(value);
}

static void ParseSyncedLyrics(const std::wstring& text, std::vector<LyricsLine>& lines) {
    lines.clear();

    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'\n', start);
        if (end == std::wstring::npos)
            end = text.size();

        std::wstring line = text.substr(start, end - start);
        if (!line.empty() && line.back() == L'\r')
            line.pop_back();

        size_t pos = 0;
        std::vector<double> stamps;

        // Accept multiple [mm:ss.xx] timestamps on one line.
        while (pos < line.size() && line[pos] == L'[') {
            const size_t close = line.find(L']', pos + 1);
            if (close == std::wstring::npos)
                break;

            const std::wstring stamp = line.substr(pos + 1, close - pos - 1);
            const size_t colon = stamp.find(L':');
            if (colon != std::wstring::npos) {
                try {
                    const double minutes = std::stod(stamp.substr(0, colon));
                    const double seconds = std::stod(stamp.substr(colon + 1));
                    const double t = minutes * 60.0 + seconds;
                    if (IsFiniteDouble(t) && t >= 0.0)
                        stamps.push_back(t);
                } catch (...) {
                }
            }

            pos = close + 1;
        }

        std::wstring content = line.substr(pos);
        while (!content.empty() && iswspace(content.front()))
            content.erase(content.begin());
        while (!content.empty() && iswspace(content.back()))
            content.pop_back();

        if (!stamps.empty() && !content.empty()) {
            for (double t : stamps)
                lines.push_back({t, content});
        }

        if (end == text.size())
            break;
        start = end + 1;
    }

    std::sort(lines.begin(), lines.end(),
        [](const LyricsLine& a, const LyricsLine& b) {
            return a.timeSeconds < b.timeSeconds;
        });
}

static void ClearLyricsState() {
    std::lock_guard<std::mutex> lock(g_lyricsMutex);
    g_lyricsLines.reset();
    g_lyricsTrackTitle.clear();
    g_lyricsTrackArtist.clear();
    g_lyricsTrackKey.clear();
    g_lyricsPositionSeconds = 0.0;
    g_lyricsPlaybackRate = 1.0;
    g_lyricsDurationSeconds = 0.0;
    g_lyricsPositionAnchorTickMs = 0;
    g_lyricsPlaying = false;
    g_lyricsHasSynced = false;
    g_lyricsAvailable = false;
    g_lyricsAvailableFast.store(g_lyricsAvailable, std::memory_order_release);
}

static void SetLyricsTimelineState(
    double position,
    double duration,
    double playbackRate,
    bool playing) {

    if (!IsFiniteDouble(position) || position < 0.0)
        position = 0.0;
    if (!IsFiniteDouble(duration) || duration < 0.0)
        duration = 0.0;
    if (!IsFiniteDouble(playbackRate) || playbackRate <= 0.0)
        playbackRate = 1.0;

    const ULONGLONG nowMs = GetTickCount64();

    std::lock_guard<std::mutex> lock(g_lyricsMutex);

    g_lyricsPositionSeconds = position;
    g_lyricsPlaybackRate = playbackRate;
    g_lyricsDurationSeconds = duration;
    g_lyricsPlaying = playing;
    g_lyricsPositionAnchorTickMs = nowMs;

    if (duration > 0.0)
        g_lyricsPositionSeconds =
            std::clamp(g_lyricsPositionSeconds, 0.0, duration);
    else
        g_lyricsPositionSeconds = std::max(0.0, g_lyricsPositionSeconds);
}

static double GetCurrentLyricsPosition() {
    std::lock_guard<std::mutex> lock(g_lyricsMutex);

    double position = g_lyricsPositionSeconds;

    if (g_lyricsPlaying && g_lyricsPositionAnchorTickMs != 0) {
        const ULONGLONG nowMs = GetTickCount64();
        const ULONGLONG elapsedMs =
            nowMs >= g_lyricsPositionAnchorTickMs
                ? nowMs - g_lyricsPositionAnchorTickMs
                : 0;

        const double rate =
            IsFiniteDouble(g_lyricsPlaybackRate)
                ? std::clamp(g_lyricsPlaybackRate, 0.05, 8.0)
                : 1.0;

        position += static_cast<double>(elapsedMs) / 1000.0 * rate;
    }

    if (g_lyricsDurationSeconds > 0.0)
        position = std::clamp(position, 0.0, g_lyricsDurationSeconds);
    else
        position = std::max(0.0, position);

    return position;
}

static std::vector<LyricsLine> ParsePlainLyricsAsEstimated(
    const std::wstring& text,
    double durationSeconds) {

    std::vector<std::wstring> rawLines;
    size_t start = 0;

    while (start <= text.size()) {
        size_t end = text.find(L'\n', start);
        if (end == std::wstring::npos)
            end = text.size();

        std::wstring line = text.substr(start, end - start);
        if (!line.empty() && line.back() == L'\r')
            line.pop_back();

        while (!line.empty() && iswspace(line.front()))
            line.erase(line.begin());
        while (!line.empty() && iswspace(line.back()))
            line.pop_back();

        if (!line.empty())
            rawLines.push_back(std::move(line));

        if (end == text.size())
            break;
        start = end + 1;
    }

    std::vector<LyricsLine> lines;
    if (rawLines.empty())
        return lines;

    // Plain lyrics contain no timestamps. They are only a fallback and are
    // explicitly treated as estimated timing rather than real synchronization.
    const double safeDuration = std::max(0.1, durationSeconds);
    const double step = rawLines.size() > 1
        ? safeDuration / static_cast<double>(rawLines.size())
        : 0.0;

    lines.reserve(rawLines.size());
    for (size_t i = 0; i < rawLines.size(); ++i)
        lines.push_back({
            step * static_cast<double>(i),
            rawLines[i]
        });

    return lines;
}


static std::wstring TrimLyricsFileNamePart(std::wstring value) {
    while (!value.empty() && iswspace(value.front()))
        value.erase(value.begin());
    while (!value.empty() && iswspace(value.back()))
        value.pop_back();

    // Replace characters that cannot appear in Windows file names.
    for (wchar_t& ch : value) {
        switch (ch) {
            case L'<': case L'>': case L':': case L'"':
            case L'/': case L'\\': case L'|': case L'?': case L'*':
                ch = L'_';
                break;
            default:
                break;
        }
    }

    while (!value.empty() && (value.back() == L'.' || value.back() == L' '))
        value.pop_back();

    return value;
}

static bool ReadLyricsTextFile(
    const std::wstring& path,
    std::wstring& text) {

    text.clear();

    HANDLE file = CreateFileW(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return false;

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) ||
        size.QuadPart <= 0 ||
        size.QuadPart > 4 * 1024 * 1024) {
        CloseHandle(file);
        return false;
    }

    std::string bytes(static_cast<size_t>(size.QuadPart), '\0');
    DWORD bytesRead = 0;
    const bool readOk =
        ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &bytesRead, nullptr) &&
        bytesRead == bytes.size();
    CloseHandle(file);

    if (!readOk)
        return false;
    bytes.resize(bytesRead);

    if (bytes.size() >= 2 &&
        static_cast<unsigned char>(bytes[0]) == 0xFF &&
        static_cast<unsigned char>(bytes[1]) == 0xFE) {
        const size_t wcharCount = (bytes.size() - 2) / sizeof(wchar_t);
        text.resize(wcharCount);
        if (wcharCount)
            memcpy(text.data(), bytes.data() + 2, wcharCount * sizeof(wchar_t));
        return !text.empty();
    }

    if (bytes.size() >= 3 &&
        static_cast<unsigned char>(bytes[0]) == 0xEF &&
        static_cast<unsigned char>(bytes[1]) == 0xBB &&
        static_cast<unsigned char>(bytes[2]) == 0xBF) {
        bytes.erase(0, 3);
    }

    if (bytes.empty())
        return false;

    const int wideLength = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        bytes.data(),
        static_cast<int>(bytes.size()),
        nullptr,
        0);
    if (wideLength > 0) {
        text.resize(wideLength);
        MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            bytes.data(),
            static_cast<int>(bytes.size()),
            text.data(),
            wideLength);
        return !text.empty();
    }

    // Some older LRC collections are saved using the system ANSI code page.
    const int ansiLength = MultiByteToWideChar(
        CP_ACP,
        0,
        bytes.data(),
        static_cast<int>(bytes.size()),
        nullptr,
        0);
    if (ansiLength <= 0)
        return false;

    text.resize(ansiLength);
    MultiByteToWideChar(
        CP_ACP,
        0,
        bytes.data(),
        static_cast<int>(bytes.size()),
        text.data(),
        ansiLength);
    return !text.empty();
}

static bool TryLoadLocalLyricsForTrack(
    const std::wstring& title,
    const std::wstring& artist,
    const std::wstring& trackKey) {

    const VisualizerSettings settings = GetSettingsSnapshot();
    if (!settings.lyricsUseLocalLrcFiles || settings.lyricsLocalLrcFolder.empty())
        return false;

    std::wstring artistPart = TrimLyricsFileNamePart(artist);
    std::wstring titlePart = TrimLyricsFileNamePart(title);
    if (titlePart.empty())
        return false;

    std::vector<std::wstring> candidateNames;
    if (!artistPart.empty())
        candidateNames.push_back(artistPart + L" - " + titlePart + L".lrc");
    candidateNames.push_back(titlePart + L".lrc");

    // Avoid relying on the exact filename casing. Windows is normally
    // case-insensitive, but this also helps when a collection was copied
    // between unusual filesystems or tools.
    for (const std::wstring& candidateName : candidateNames) {
        std::wstring path = settings.lyricsLocalLrcFolder;
        if (!path.empty() && path.back() != L'\\' && path.back() != L'/')
            path.push_back(L'\\');
        path += candidateName;

        const DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES ||
            (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            continue;

        std::wstring fileText;
        if (!ReadLyricsTextFile(path, fileText))
            continue;

        std::vector<LyricsLine> parsed;
        ParseSyncedLyrics(fileText, parsed);
        if (parsed.empty())
            continue;

        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        if (g_lyricsTrackKey != trackKey)
            return false;

        g_lyricsAvailable = true;
        g_lyricsAvailableFast.store(true, std::memory_order_release);
        g_lyricsHasSynced = true;
        g_lyricsLines = std::make_shared<const std::vector<LyricsLine>>(std::move(parsed));
        return true;
    }

    return false;
}

static void FetchLyricsForTrack(
    const std::wstring& title,
    const std::wstring& artist,
    const std::wstring& album,
    double durationSeconds,
    const std::wstring& trackKey) {

    if (title.empty() || artist.empty() || durationSeconds <= 0.0)
        return;

    const VisualizerSettings settings = GetSettingsSnapshot();
    if (settings.lyricsUseLocalLrcFiles) {
        // Local mode is intentionally exclusive: do not contact LRCLIB at all.
        // A missing/invalid local file simply leaves lyrics unavailable.
        TryLoadLocalLyricsForTrack(title, artist, trackKey);
        return;
    }

    const std::wstring host = L"lrclib.net";
    const std::wstring artistEncoded =
        Utf8ToWide(PercentEncodeUtf8(artist));
    const std::wstring titleEncoded =
        Utf8ToWide(PercentEncodeUtf8(title));
    const std::wstring albumEncoded =
        Utf8ToWide(PercentEncodeUtf8(album));

    // LRCLIB's /api/get endpoint requires the full track signature.
    // Try the exact duration first, then the ±1s/±2s variants to tolerate
    std::vector<int> durationCandidates;
    const int roundedDuration =
        static_cast<int>(std::llround(durationSeconds));

    for (int delta : {0, -1, 1, -2, 2}) {
        const int candidate = roundedDuration + delta;
        if (candidate > 0 &&
            std::find(durationCandidates.begin(), durationCandidates.end(), candidate)
                == durationCandidates.end()) {
            durationCandidates.push_back(candidate);
        }
    }

    std::string response;

    for (int candidate : durationCandidates) {
        const std::wstring path =
            L"/api/get?artist_name=" + artistEncoded +
            L"&track_name=" + titleEncoded +
            L"&album_name=" + albumEncoded +
            L"&duration=" + std::to_wstring(candidate);

        if (g_hLyricsStopEvent && WaitForSingleObject(g_hLyricsStopEvent, 0) == WAIT_OBJECT_0)
            return;

        if (HttpGetUtf8(host, path, response, g_hLyricsStopEvent))
            break;

        response.clear();
    }

    if (response.empty()) {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        if (g_lyricsTrackKey == trackKey) {
            g_lyricsAvailable = false;
            g_lyricsAvailableFast.store(g_lyricsAvailable, std::memory_order_release);
            g_lyricsHasSynced = false;
        }
        return;
    }

    std::string syncedUtf8;
    std::string plainUtf8;
    JsonGetString(response, "syncedLyrics", syncedUtf8);
    JsonGetString(response, "plainLyrics", plainUtf8);

    const std::wstring synced = Utf8ToWide(syncedUtf8);
    const std::wstring plain = Utf8ToWide(plainUtf8);

    std::vector<LyricsLine> parsed;
    bool hasSynced = false;

    if (!synced.empty()) {
        ParseSyncedLyrics(synced, parsed);
        hasSynced = !parsed.empty();
    }

    if (parsed.empty() && !plain.empty()) {
        parsed = ParsePlainLyricsAsEstimated(plain, durationSeconds);
        hasSynced = false;
    }

    std::lock_guard<std::mutex> lock(g_lyricsMutex);
    if (g_lyricsTrackKey != trackKey)
        return;

    g_lyricsAvailable = !parsed.empty();

    g_lyricsAvailableFast.store(g_lyricsAvailable, std::memory_order_release);
    g_lyricsHasSynced = hasSynced;
    g_lyricsLines = std::make_shared<const std::vector<LyricsLine>>(std::move(parsed));
}

static std::wstring NormalizeMediaAppIdentifier(std::wstring value) {
    while (!value.empty() && iswspace(value.back()))
        value.pop_back();
    while (!value.empty() && iswspace(value.front()))
        value.erase(value.begin());

    for (wchar_t& ch : value)
        ch = static_cast<wchar_t>(towlower(ch));

    return value;
}

static std::wstring GetExecutableStem(const std::wstring& executableName) {
    std::wstring stem = NormalizeMediaAppIdentifier(executableName);

    const size_t slash = stem.find_last_of(L"\\/");
    if (slash != std::wstring::npos)
        stem = stem.substr(slash + 1);

    if (stem.size() > 4 && stem.compare(stem.size() - 4, 4, L".exe") == 0)
        stem.erase(stem.size() - 4);

    return stem;
}

static bool MediaSessionMatchesExecutable(
    const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession& session,
    const std::wstring& executableName) {
    if (!session || executableName.empty())
        return false;

    const std::wstring wanted = GetExecutableStem(executableName);
    if (wanted.empty())
        return false;

    try {
        const std::wstring sourceAppId =
            NormalizeMediaAppIdentifier(session.SourceAppUserModelId().c_str());
        if (sourceAppId.empty())
            return false;
        return sourceAppId.find(wanted) != std::wstring::npos;
    } catch (...) {
        return false;
    }
}

static bool MediaSessionMatchesExecutableList(
    const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession& session,
    const std::wstring& executableList) {
    const auto names = SplitExecutableList(executableList);
    for (const auto& name : names) {
        if (MediaSessionMatchesExecutable(session, name))
            return true;
    }
    return false;
}

static winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession
FindLyricsSession(
    const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager& manager,
    const std::wstring& executableName,
    std::wstring& pinnedSourceAppUserModelId) {
    if (executableName.empty())
        return nullptr;

    using Session = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession;

    if (!pinnedSourceAppUserModelId.empty()) {
        try {
            const auto sessions = manager.GetSessions();
            for (const auto& session : sessions) {
                if (!session)
                    continue;

                const std::wstring sourceAppId =
                    NormalizeMediaAppIdentifier(session.SourceAppUserModelId().c_str());
                if (sourceAppId == pinnedSourceAppUserModelId &&
                    MediaSessionMatchesExecutableList(session, executableName))
                    return session;
            }
        } catch (...) {
        }

        pinnedSourceAppUserModelId.clear();
    }

    try {
        const auto sessions = manager.GetSessions();
        Session bestSession = nullptr;

        for (const auto& session : sessions) {
            if (!session || !MediaSessionMatchesExecutableList(session, executableName))
                continue;

            bool playing = false;
            try {
                using PlaybackStatus =
                    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                playing = session.GetPlaybackInfo().PlaybackStatus() == PlaybackStatus::Playing;
            } catch (...) {
            }

            if (playing) {
                bestSession = session;
                break;
            }

            if (!bestSession)
                bestSession = session;
        }

        if (bestSession) {
            pinnedSourceAppUserModelId = NormalizeMediaAppIdentifier(
                bestSession.SourceAppUserModelId().c_str());

            if (!pinnedSourceAppUserModelId.empty()) {
                Wh_Log(L"Media session pinned to AUMID '%s' for executable list '%s'",
                       pinnedSourceAppUserModelId.c_str(), executableName.c_str());
            }
        }

        return bestSession;
    } catch (...) {
        return nullptr;
    }
}


static std::wstring EqMediaSourceDisplayName(const std::wstring& sourceAppId) {
    const std::wstring id = NormalizeMediaAppIdentifier(sourceAppId);
    if (id.find(L"spotify") != std::wstring::npos)
        return L"Spotify";
    if (id.find(L"youtubemusic") != std::wstring::npos ||
        id.find(L"youtube music") != std::wstring::npos)
        return L"YouTube Music";
    if (id.find(L"vlc") != std::wstring::npos)
        return L"VLC";
    if (id.find(L"musicbee") != std::wstring::npos)
        return L"MusicBee";
    if (id.find(L"foobar") != std::wstring::npos)
        return L"foobar2000";
    if (id.find(L"aimp") != std::wstring::npos)
        return L"AIMP";
    if (id.find(L"media player") != std::wstring::npos ||
        id.find(L"zune") != std::wstring::npos)
        return L"Media Player";
    if (id.find(L"chrome") != std::wstring::npos)
        return L"Chrome";
    if (id.find(L"msedge") != std::wstring::npos ||
        id.find(L"edge") != std::wstring::npos)
        return L"Edge";
    if (id.find(L"firefox") != std::wstring::npos)
        return L"Firefox";
    if (id.find(L"brave") != std::wstring::npos)
        return L"Brave";
    if (id.empty())
        return {};

    // Keep an unknown AUMID readable without exposing the long package suffix.
    std::wstring display = sourceAppId;
    const size_t bang = display.find(L'!');
    if (bang != std::wstring::npos)
        display.resize(bang);
    const size_t slash = display.find_last_of(L"\\/");
    if (slash != std::wstring::npos)
        display = display.substr(slash + 1);
    if (display.size() > 28)
        display.resize(28);
    return display;
}

static std::atomic<ULONGLONG> g_eqMediaPositionAnchorTickMs{0};
static std::atomic<ULONGLONG> g_eqMediaLocalOverrideUntilMs{0};

static void SetEqMediaStateEmpty() {
    std::lock_guard<std::mutex> lock(g_eqMediaMutex);
    g_eqMediaState = EqMediaState{};
    g_eqMediaPositionAnchorTickMs.store(0, std::memory_order_release);
    g_eqMediaLocalOverrideUntilMs.store(0, std::memory_order_release);
}

static void SetEqMediaState(
    const std::wstring& title,
    const std::wstring& artist,
    const std::wstring& sourceAppId,
    double position,
    double duration,
    double playbackRate,
    bool playing) {

    if (!IsFiniteDouble(position) || position < 0.0)
        position = 0.0;
    if (!IsFiniteDouble(duration) || duration < 0.0)
        duration = 0.0;
    if (!IsFiniteDouble(playbackRate) || playbackRate <= 0.0)
        playbackRate = 1.0;
    if (duration > 0.0)
        position = std::clamp(position, 0.0, duration);
    else
        position = std::max(0.0, position);

    EqMediaState next;
    next.title = title;
    next.artist = artist;
    next.sourceAppUserModelId = sourceAppId;
    next.source = EqMediaSourceDisplayName(sourceAppId);
    next.positionSeconds = position;
    next.durationSeconds = duration;
    next.playbackRate = playbackRate;
    next.playing = playing;
    next.hasSession = true;
    next.trackKey = artist + L"\n" + title + L"\n" + sourceAppId + L"\n" +
                    std::to_wstring(static_cast<long long>(std::llround(duration)));

    const ULONGLONG now = GetTickCount64();
    std::lock_guard<std::mutex> lock(g_eqMediaMutex);

    const EqMediaState previous = g_eqMediaState;
    const bool sameTrack = previous.hasSession && next.trackKey == previous.trackKey;
    const bool playbackStateChanged = previous.playing != next.playing;
    const ULONGLONG overrideUntil =
        g_eqMediaLocalOverrideUntilMs.load(std::memory_order_acquire);
    const bool localOverrideActive =
        overrideUntil != 0 && now < overrideUntil && sameTrack;

    if (sameTrack && localOverrideActive) {
        // Keep the optimistic UI state for a short time after a local media
        // command. WinRT can report the old playback/position state until its
        // async command has completed; without this guard the worker could
        // immediately paint that stale state back over the popup.
        next.positionSeconds = previous.positionSeconds;
        next.playing = previous.playing;
    } else if (sameTrack && previous.playing && next.playing && !playbackStateChanged) {
        // `position` is already compensated to "now" from the media
        // timeline's LastUpdatedTime (see EqMediaThreadProc below).  When
        // there is no active local seek override, re-anchor to that real
        // player time every sample. This keeps the UI clock synchronized to
        // the actual media position instead of maintaining an independent
        // percentage-based clock that can drift and then jump backwards.
        next.positionSeconds = position;
        g_eqMediaPositionAnchorTickMs.store(now, std::memory_order_release);
    } else {
        // New track, play/pause transition, or a stopped session: accept the
        // media session's position as authoritative and start a fresh local
        // interpolation only when playback is actually running.
        next.positionSeconds = position;
        g_eqMediaPositionAnchorTickMs.store(
            playing ? now : 0, std::memory_order_release);
        if (!localOverrideActive)
            g_eqMediaLocalOverrideUntilMs.store(0, std::memory_order_release);
    }

    if (duration > 0.0)
        next.positionSeconds = std::clamp(next.positionSeconds, 0.0, duration);
    else
        next.positionSeconds = std::max(0.0, next.positionSeconds);

    g_eqMediaState = std::move(next);
}

static EqMediaState GetEqMediaStateSnapshot() {
    std::lock_guard<std::mutex> lock(g_eqMediaMutex);
    return g_eqMediaState;
}

static double GetEqMediaPosition() {
    std::lock_guard<std::mutex> lock(g_eqMediaMutex);
    double position = g_eqMediaState.positionSeconds;
    if (g_eqMediaState.playing && g_eqMediaPositionAnchorTickMs.load(std::memory_order_acquire) != 0) {
        const ULONGLONG now = GetTickCount64();
        const ULONGLONG anchor = g_eqMediaPositionAnchorTickMs.load(std::memory_order_acquire);
        const ULONGLONG elapsed = now >= anchor ? now - anchor : 0;
        const double rate = std::clamp(
            IsFiniteDouble(g_eqMediaState.playbackRate)
                ? g_eqMediaState.playbackRate : 1.0,
            0.05, 8.0);
        position += static_cast<double>(elapsed) / 1000.0 * rate;
    }
    if (g_eqMediaState.durationSeconds > 0.0)
        position = std::clamp(position, 0.0, g_eqMediaState.durationSeconds);
    return std::max(0.0, position);
}

static void EnqueueEqMediaCommand(
    EqMediaCommandType type, double seekPositionSeconds = 0.0) {
    {
        std::lock_guard<std::mutex> lock(g_eqMediaCommandMutex);
        g_eqMediaCommands.push_back(EqMediaCommand{type, seekPositionSeconds});
    }

    if (type == EqMediaCommandType::PlayPause) {
        // Optimistically flip the icon immediately. The actual WinRT command
        // is asynchronous, so waiting for TryPlayAsync/TryPauseAsync before
        // changing this state creates the noticeable ~0.5 s visual lag.
        std::lock_guard<std::mutex> lock(g_eqMediaMutex);
        if (g_eqMediaState.hasSession) {
            const ULONGLONG now = GetTickCount64();
            double position = g_eqMediaState.positionSeconds;
            const ULONGLONG anchor =
                g_eqMediaPositionAnchorTickMs.load(std::memory_order_acquire);
            if (g_eqMediaState.playing && anchor != 0 && now >= anchor) {
                const double rate = std::clamp(
                    IsFiniteDouble(g_eqMediaState.playbackRate)
                        ? g_eqMediaState.playbackRate : 1.0,
                    0.05, 8.0);
                position += static_cast<double>(now - anchor) / 1000.0 * rate;
            }
            if (g_eqMediaState.durationSeconds > 0.0)
                position = std::clamp(position, 0.0, g_eqMediaState.durationSeconds);
            else
                position = std::max(0.0, position);

            g_eqMediaState.positionSeconds = position;
            g_eqMediaState.playing = !g_eqMediaState.playing;
            g_eqMediaPositionAnchorTickMs.store(
                g_eqMediaState.playing ? now : 0, std::memory_order_release);
            g_eqMediaLocalOverrideUntilMs.store(
                now + 1500, std::memory_order_release);
        }
    }

    // Reflect seek/±5s immediately instead of waiting for WinRT's next
    // TimelineProperties sample. This also prevents the thumb from visually
    // jumping back to the old position while the async seek is completing,
    // including when the track is paused.
    if (type == EqMediaCommandType::Seek ||
        type == EqMediaCommandType::Rewind5 ||
        type == EqMediaCommandType::Forward5) {
        std::lock_guard<std::mutex> lock(g_eqMediaMutex);
        if (g_eqMediaState.hasSession) {
            const ULONGLONG now = GetTickCount64();
            double target = g_eqMediaState.positionSeconds;
            const ULONGLONG anchor =
                g_eqMediaPositionAnchorTickMs.load(std::memory_order_acquire);
            if (g_eqMediaState.playing && anchor != 0 && now >= anchor) {
                const double rate = std::clamp(
                    IsFiniteDouble(g_eqMediaState.playbackRate)
                        ? g_eqMediaState.playbackRate : 1.0,
                    0.05, 8.0);
                target += static_cast<double>(now - anchor) / 1000.0 * rate;
            }
            if (type == EqMediaCommandType::Seek) {
                target = seekPositionSeconds;
            } else {
                target += type == EqMediaCommandType::Rewind5 ? -5.0 : 5.0;
            }
            if (g_eqMediaState.durationSeconds > 0.0)
                target = std::clamp(target, 0.0, g_eqMediaState.durationSeconds);
            else
                target = std::max(0.0, target);
            g_eqMediaState.positionSeconds = target;
            // A paused track must not get a running interpolation anchor.
            g_eqMediaPositionAnchorTickMs.store(
                g_eqMediaState.playing ? now : 0, std::memory_order_release);
            g_eqMediaLocalOverrideUntilMs.store(
                now + 3000, std::memory_order_release);
        }
    }

    if (g_hEqMediaWakeEvent)
        SetEvent(g_hEqMediaWakeEvent);
}

static std::vector<EqMediaCommand> DrainEqMediaCommands() {
    std::vector<EqMediaCommand> result;
    std::lock_guard<std::mutex> lock(g_eqMediaCommandMutex);
    result.reserve(g_eqMediaCommands.size());
    while (!g_eqMediaCommands.empty()) {
        result.push_back(g_eqMediaCommands.front());
        g_eqMediaCommands.pop_front();
    }
    return result;
}

static void ExecuteEqMediaCommand(
    const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession& session,
    const EqMediaCommand& command) {
    if (!session)
        return;

    try {
        switch (command.type) {
        case EqMediaCommandType::PlayPause: {
            using PlaybackStatus =
                winrt::Windows::Media::Control::
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus;
            const auto status = session.GetPlaybackInfo().PlaybackStatus();
            if (status == PlaybackStatus::Playing) {
                auto op = session.TryPauseAsync();
                if (WaitWinrtAsync(op, g_hEqMediaStopEvent, std::chrono::milliseconds(800)))
                    (void)op.GetResults();
            } else {
                auto op = session.TryPlayAsync();
                if (WaitWinrtAsync(op, g_hEqMediaStopEvent, std::chrono::milliseconds(800)))
                    (void)op.GetResults();
            }
            break;
        }
        case EqMediaCommandType::Previous: {
            auto op = session.TrySkipPreviousAsync();
            if (WaitWinrtAsync(op, g_hEqMediaStopEvent, std::chrono::milliseconds(800)))
                (void)op.GetResults();
            break;
        }
        case EqMediaCommandType::Next: {
            auto op = session.TrySkipNextAsync();
            if (WaitWinrtAsync(op, g_hEqMediaStopEvent, std::chrono::milliseconds(800)))
                (void)op.GetResults();
            break;
        }
        case EqMediaCommandType::Rewind5:
        case EqMediaCommandType::Forward5: {
            const auto timeline = session.GetTimelineProperties();
            const double duration = timeline.EndTime().count() / 10000000.0;
            double position = timeline.Position().count() / 10000000.0;

            // TimelineProperties::Position is valid as of LastUpdatedTime,
            // not necessarily at this exact instant. Compensate that age so
            // ±5s is applied to the real current playback position.
            try {
                const auto playbackInfo = session.GetPlaybackInfo();
                using PlaybackStatus =
                    winrt::Windows::Media::Control::
                        GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                if (playbackInfo.PlaybackStatus() == PlaybackStatus::Playing) {
                    const auto age = winrt::clock::now() - timeline.LastUpdatedTime();
                    double ageSeconds =
                        static_cast<double>(age.count()) / 10000000.0;
                    if (ageSeconds > 0.0 && ageSeconds < 60.0) {
                        auto rateRef = playbackInfo.PlaybackRate();
                        const double rate = rateRef ? rateRef.Value() : 1.0;
                        if (IsFiniteDouble(rate) && rate > 0.0)
                            position += ageSeconds * rate;
                    }
                }
            } catch (...) {
            }

            const double delta =
                command.type == EqMediaCommandType::Rewind5 ? -5.0 : 5.0;
            position += delta;
            if (duration > 0.0)
                position = std::clamp(position, 0.0, duration);
            else
                position = std::max(0.0, position);
            const int64_t ticks =
                static_cast<int64_t>(std::llround(position * 10000000.0));
            auto op = session.TryChangePlaybackPositionAsync(ticks);
            if (WaitWinrtAsync(op, g_hEqMediaStopEvent, std::chrono::milliseconds(800)))
                (void)op.GetResults();
            break;
        }
        case EqMediaCommandType::Seek: {
            double position = command.seekPositionSeconds;
            const auto timeline = session.GetTimelineProperties();
            const double duration = timeline.EndTime().count() / 10000000.0;
            if (duration > 0.0)
                position = std::clamp(position, 0.0, duration);
            else
                position = std::max(0.0, position);
            const int64_t ticks = static_cast<int64_t>(std::llround(position * 10000000.0));
            auto op = session.TryChangePlaybackPositionAsync(ticks);
            if (WaitWinrtAsync(op, g_hEqMediaStopEvent, std::chrono::milliseconds(800)))
                (void)op.GetResults();
            break;
        }
        }
    } catch (...) {
    }
}

static DWORD WINAPI EqMediaThreadProc(LPVOID) {
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;

        auto managerOperation =
            winrt::Windows::Media::Control::
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        if (!WaitWinrtAsync(managerOperation, g_hEqMediaStopEvent,
                            std::chrono::milliseconds(1500)))
            throw winrt::hresult_canceled();
        auto manager = managerOperation.GetResults();
        std::wstring pinnedSourceAppUserModelId;
        winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession cachedSession = nullptr;
        ULONGLONG lastSessionLookupMs = 0;
        std::wstring lastSessionLookupKey;
        ULONGLONG lastMediaPropertiesMs = 0;
        ULONGLONG lastTimelinePollMs = 0;
        ULONGLONG lastPlaybackInfoPollMs = 0;
        std::wstring cachedMediaTitle;
        std::wstring cachedMediaArtist;
        std::wstring cachedSourceAppId;
        double cachedDuration = 0.0;
        double cachedTimelinePosition = 0.0;
        double cachedPlaybackRate = 1.0;
        bool cachedPlaying = false;
        winrt::Windows::Foundation::DateTime cachedTimelineLastUpdated{};
        bool cachedTimelineLastUpdatedValid = false;
        bool forceMediaRefresh = true;

        while (g_eqMediaRunning.load(std::memory_order_acquire)) {
            const VisualizerSettings settings = GetSettingsSnapshot();
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session = nullptr;

            try {
                const ULONGLONG nowMs = GetTickCount64();
                const std::wstring lookupKey =
                    settings.audioSource == 1 ? settings.audioApplicationName : L"<system>";

                if (settings.audioSource == 1 && !settings.audioApplicationName.empty()) {
                    if (lookupKey != lastSessionLookupKey ||
                        nowMs - lastSessionLookupMs >= 750) {
                        lastSessionLookupKey = lookupKey;
                        lastSessionLookupMs = nowMs;
                        auto newSession = FindLyricsSession(
                            manager, settings.audioApplicationName,
                            pinnedSourceAppUserModelId);
                        if (newSession != cachedSession) {
                            cachedSession = newSession;
                            lastMediaPropertiesMs = 0;
                            lastTimelinePollMs = 0;
                            lastPlaybackInfoPollMs = 0;
                            cachedSourceAppId.clear();
                            cachedDuration = 0.0;
                            cachedTimelinePosition = 0.0;
                            cachedPlaybackRate = 1.0;
                            cachedPlaying = false;
                            cachedTimelineLastUpdated = {};
                            cachedTimelineLastUpdatedValid = false;
                            forceMediaRefresh = true;
                        }
                    }
                    session = cachedSession;
                } else if (settings.audioSource == 1) {
                    lastSessionLookupKey.clear();
                    lastSessionLookupMs = 0;
                    pinnedSourceAppUserModelId.clear();
                    cachedSession = nullptr;
                    cachedSourceAppId.clear();
                    cachedDuration = 0.0;
                    cachedTimelinePosition = 0.0;
                    cachedPlaybackRate = 1.0;
                    cachedPlaying = false;
                    cachedTimelineLastUpdated = {};
                    cachedTimelineLastUpdatedValid = false;
                    lastMediaPropertiesMs = 0;
                    lastTimelinePollMs = 0;
                    lastPlaybackInfoPollMs = 0;
                    forceMediaRefresh = true;
                    session = nullptr;
                } else {
                    // CurrentSession is only needed for detecting an actual session
                    // switch. Position is projected locally between timeline samples.
                    if (lastSessionLookupKey != L"<system>" ||
                        nowMs - lastSessionLookupMs >= 750) {
                        lastSessionLookupKey = L"<system>";
                        lastSessionLookupMs = nowMs;
                        auto newSession = manager.GetCurrentSession();
                        if (newSession != cachedSession) {
                            cachedSession = newSession;
                            lastMediaPropertiesMs = 0;
                            lastTimelinePollMs = 0;
                            lastPlaybackInfoPollMs = 0;
                            cachedSourceAppId.clear();
                            cachedDuration = 0.0;
                            cachedTimelinePosition = 0.0;
                            cachedPlaybackRate = 1.0;
                            cachedPlaying = false;
                            cachedTimelineLastUpdated = {};
                            cachedTimelineLastUpdatedValid = false;
                            forceMediaRefresh = true;
                        }
                    }
                    pinnedSourceAppUserModelId.clear();
                    session = cachedSession;
                }
            } catch (...) {
                session = nullptr;
            }

            auto commands = DrainEqMediaCommands();
            if (session) {
                for (const auto& command : commands)
                    ExecuteEqMediaCommand(session, command);
                if (!commands.empty())
                    forceMediaRefresh = true;
            }

            try {
                if (session) {
                    const ULONGLONG sampleNowMs = GetTickCount64();
                    const bool refreshProperties =
                        forceMediaRefresh || lastMediaPropertiesMs == 0 ||
                        sampleNowMs - lastMediaPropertiesMs >= 1000;
                    const bool refreshTimeline =
                        forceMediaRefresh || lastTimelinePollMs == 0 ||
                        sampleNowMs - lastTimelinePollMs >= 500;
                    const bool refreshPlaybackInfo =
                        forceMediaRefresh || lastPlaybackInfoPollMs == 0 ||
                        sampleNowMs - lastPlaybackInfoPollMs >= 500;

                    bool stateRefreshed = forceMediaRefresh;

                    if (refreshProperties) {
                        lastMediaPropertiesMs = sampleNowMs;
                        try {
                            auto propsOperation = session.TryGetMediaPropertiesAsync();
                            if (WaitWinrtAsync(propsOperation, g_hEqMediaStopEvent,
                                               std::chrono::milliseconds(800))) {
                                auto props = propsOperation.GetResults();
                                if (props) {
                                    cachedMediaTitle = props.Title().c_str();
                                    cachedMediaArtist = props.Artist().c_str();
                                }
                            }
                        } catch (...) {
                        }
                        stateRefreshed = true;
                    }

                    if (refreshTimeline) {
                        lastTimelinePollMs = sampleNowMs;
                        try {
                            const auto timeline = session.GetTimelineProperties();
                            cachedDuration = timeline.EndTime().count() / 10000000.0;
                            cachedTimelinePosition = timeline.Position().count() / 10000000.0;
                            cachedTimelineLastUpdated = timeline.LastUpdatedTime();
                            cachedTimelineLastUpdatedValid = true;
                        } catch (...) {
                        }
                        stateRefreshed = true;
                    }

                    if (refreshPlaybackInfo) {
                        lastPlaybackInfoPollMs = sampleNowMs;
                        try {
                            const auto info = session.GetPlaybackInfo();
                            using PlaybackStatus =
                                winrt::Windows::Media::Control::
                                    GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                            cachedPlaying = info.PlaybackStatus() == PlaybackStatus::Playing;
                            auto rateRef = info.PlaybackRate();
                            const double rate = rateRef ? rateRef.Value() : 1.0;
                            if (IsFiniteDouble(rate) && rate > 0.0)
                                cachedPlaybackRate = rate;
                        } catch (...) {
                        }
                        stateRefreshed = true;
                    }

                    if (cachedSourceAppId.empty() || forceMediaRefresh) {
                        try {
                            cachedSourceAppId = session.SourceAppUserModelId().c_str();
                        } catch (...) {
                        }
                        stateRefreshed = true;
                    }

                    if (stateRefreshed) {
                        double position = cachedTimelinePosition;
                        double duration = cachedDuration;

                        // Keep the old smooth position behavior: advance the last
                        // timeline sample locally instead of querying WinRT each wake.
                        if (cachedPlaying && cachedTimelineLastUpdatedValid) {
                            try {
                                const auto age =
                                    winrt::clock::now() - cachedTimelineLastUpdated;
                                double ageSeconds =
                                    static_cast<double>(age.count()) / 10000000.0;
                                if (ageSeconds > 0.0 && ageSeconds < 60.0)
                                    position += ageSeconds * cachedPlaybackRate;
                            } catch (...) {
                            }
                        }

                        if (duration > 0.0)
                            position = std::clamp(position, 0.0, duration);
                        else
                            position = std::max(0.0, position);

                        SetEqMediaState(
                            cachedMediaTitle, cachedMediaArtist, cachedSourceAppId,
                            position, duration, cachedPlaybackRate, cachedPlaying);
                    }

                    forceMediaRefresh = false;
                } else {
                    cachedMediaTitle.clear();
                    cachedMediaArtist.clear();
                    cachedSourceAppId.clear();
                    cachedDuration = 0.0;
                    cachedTimelinePosition = 0.0;
                    cachedPlaybackRate = 1.0;
                    cachedPlaying = false;
                    cachedTimelineLastUpdated = {};
                    cachedTimelineLastUpdatedValid = false;
                    lastMediaPropertiesMs = 0;
                    lastTimelinePollMs = 0;
                    lastPlaybackInfoPollMs = 0;
                    forceMediaRefresh = true;
                    SetEqMediaStateEmpty();
                    g_eqMediaPositionAnchorTickMs.store(0, std::memory_order_release);
                }
            } catch (...) {
                SetEqMediaStateEmpty();
                g_eqMediaPositionAnchorTickMs.store(0, std::memory_order_release);
            }

            if (g_hEqMediaStopEvent) {
                const DWORD waitMs = g_hEqMediaWakeEvent ? 150 : 150;
                if (WaitForSingleObject(g_hEqMediaWakeEvent, waitMs) == WAIT_OBJECT_0)
                    ResetEvent(g_hEqMediaWakeEvent);
                if (WaitForSingleObject(g_hEqMediaStopEvent, 0) == WAIT_OBJECT_0)
                    break;
            } else {
                Sleep(150);
            }
        }
    } catch (...) {
        SetEqMediaStateEmpty();
    }

    g_eqMediaPositionAnchorTickMs.store(0, std::memory_order_release);
    g_eqMediaRunning.store(false, std::memory_order_release);
    if (apartmentInitialized)
        winrt::uninit_apartment();
    return 0;
}

static void StartEqMediaCapture() {
    if (g_eqMediaRunning.exchange(true, std::memory_order_acq_rel))
        return;

    if (!g_hEqMediaStopEvent)
        g_hEqMediaStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hEqMediaWakeEvent)
        g_hEqMediaWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    if (!g_hEqMediaStopEvent || !g_hEqMediaWakeEvent) {
        g_eqMediaRunning.store(false, std::memory_order_release);
        if (g_hEqMediaStopEvent) {
            CloseHandle(g_hEqMediaStopEvent);
            g_hEqMediaStopEvent = nullptr;
        }
        if (g_hEqMediaWakeEvent) {
            CloseHandle(g_hEqMediaWakeEvent);
            g_hEqMediaWakeEvent = nullptr;
        }
        return;
    }

    ResetEvent(g_hEqMediaStopEvent);
    ResetEvent(g_hEqMediaWakeEvent);
    g_eqMediaPositionAnchorTickMs.store(0, std::memory_order_release);
    g_hEqMediaThread = CreateThread(
        nullptr, 0, EqMediaThreadProc, nullptr, 0, nullptr);
    if (!g_hEqMediaThread) {
        g_eqMediaRunning.store(false, std::memory_order_release);
        CloseHandle(g_hEqMediaStopEvent);
        CloseHandle(g_hEqMediaWakeEvent);
        g_hEqMediaStopEvent = nullptr;
        g_hEqMediaWakeEvent = nullptr;
    }
}

static void StopEqMediaCapture() {
    const bool wasRunning =
        g_eqMediaRunning.exchange(false, std::memory_order_acq_rel);
    if (!wasRunning && !g_hEqMediaThread)
        return;

    if (g_hEqMediaStopEvent)
        SetEvent(g_hEqMediaStopEvent);
    if (g_hEqMediaWakeEvent)
        SetEvent(g_hEqMediaWakeEvent);

    if (g_hEqMediaThread) {
        WaitForSingleObject(g_hEqMediaThread, INFINITE);
        CloseHandle(g_hEqMediaThread);
        g_hEqMediaThread = nullptr;
    }

    if (g_hEqMediaStopEvent) {
        CloseHandle(g_hEqMediaStopEvent);
        g_hEqMediaStopEvent = nullptr;
    }
    if (g_hEqMediaWakeEvent) {
        CloseHandle(g_hEqMediaWakeEvent);
        g_hEqMediaWakeEvent = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(g_eqMediaCommandMutex);
        g_eqMediaCommands.clear();
    }
    g_eqMediaPositionAnchorTickMs.store(0, std::memory_order_release);
    SetEqMediaStateEmpty();
}

static DWORD WINAPI LyricsThreadProc(LPVOID) {
    bool winrtApartmentInitialized = false;

    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        winrtApartmentInitialized = true;
        auto managerOperation =
            winrt::Windows::Media::Control::
                GlobalSystemMediaTransportControlsSessionManager::
                    RequestAsync();
        if (!WaitWinrtAsync(managerOperation, g_hLyricsStopEvent,
                            std::chrono::milliseconds(1500)))
            throw winrt::hresult_canceled();
        auto manager = managerOperation.GetResults();

        std::wstring lastTrackKey;
        ULONGLONG lastFetchMs = 0;
        std::wstring pinnedSourceAppUserModelId;
        winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession cachedSession = nullptr;
        ULONGLONG lastSessionLookupMs = 0;
        std::wstring lastSessionLookupKey;
        ULONGLONG lastMediaPropertiesMs = 0;
        std::wstring cachedMediaTitle;
        std::wstring cachedMediaArtist;
        std::wstring cachedMediaAlbum;

        while (g_lyricsRunning.load(std::memory_order_acquire)) {
            const VisualizerSettings settings = GetSettingsSnapshot();
            std::wstring title;
            std::wstring artist;
            std::wstring album;
            double duration = 0.0;
            double position = 0.0;
            double playbackRate = 1.0;
            bool playing = false;
            bool haveSession = false;
            bool haveTimeline = false;
            winrt::Windows::Foundation::DateTime timelineUpdatedTime{};

            try {
                winrt::Windows::Media::Control::
                    GlobalSystemMediaTransportControlsSession session = nullptr;

                const ULONGLONG nowMs = GetTickCount64();
                const std::wstring lookupKey =
                    settings.audioSource == 1 ? settings.audioApplicationName : L"<system>";

                if (settings.audioSource == 1) {
                    if (!settings.audioApplicationName.empty()) {
                        if (lookupKey != lastSessionLookupKey ||
                            nowMs - lastSessionLookupMs >= 750) {
                            lastSessionLookupKey = lookupKey;
                            lastSessionLookupMs = nowMs;
                            auto newSession = FindLyricsSession(
                                manager, settings.audioApplicationName,
                                pinnedSourceAppUserModelId);
                            if (newSession != cachedSession) {
                                cachedSession = newSession;
                                lastMediaPropertiesMs = 0;
                            }
                        }
                        session = cachedSession;
                    } else {
                        lastSessionLookupKey.clear();
                        lastSessionLookupMs = 0;
                        pinnedSourceAppUserModelId.clear();
                        cachedSession = nullptr;
                    }
                } else {
                    session = manager.GetCurrentSession();
                    cachedSession = session;
                    lastSessionLookupKey = L"<system>";
                    lastSessionLookupMs = nowMs;
                    pinnedSourceAppUserModelId.clear();
                }

                if (session) {
                    haveSession = true;

                    try {
                        const bool refreshProperties =
                            lastMediaPropertiesMs == 0 ||
                            nowMs - lastMediaPropertiesMs >= 1000;
                        if (refreshProperties) {
                            lastMediaPropertiesMs = nowMs;
                            auto propsOperation = session.TryGetMediaPropertiesAsync();
                            if (!WaitWinrtAsync(propsOperation, g_hLyricsStopEvent, std::chrono::milliseconds(1500)))
                                throw winrt::hresult_canceled();
                            auto props = propsOperation.GetResults();
                            if (props) {
                                cachedMediaTitle = props.Title().c_str();
                                cachedMediaArtist = props.Artist().c_str();
                                cachedMediaAlbum = props.AlbumTitle().c_str();
                            }
                        }

                        title = cachedMediaTitle;
                        artist = cachedMediaArtist;
                        album = cachedMediaAlbum;
                    } catch (...) {
                        // Keep the last good metadata if retrieval temporarily fails.
                        title = cachedMediaTitle;
                        artist = cachedMediaArtist;
                        album = cachedMediaAlbum;
                    }

                    try {
                        const auto timeline = session.GetTimelineProperties();
                        duration =
                            timeline.EndTime().count() / 10000000.0;
                        position =
                            timeline.Position().count() / 10000000.0;
                        timelineUpdatedTime = timeline.LastUpdatedTime();
                        haveTimeline = true;
                    } catch (...) {
                        duration = 0.0;
                        position = 0.0;
                    }

                    try {
                        const auto playbackInfo = session.GetPlaybackInfo();

                        using PlaybackStatus =
                            winrt::Windows::Media::Control::
                                GlobalSystemMediaTransportControlsSessionPlaybackStatus;

                        playing =
                            playbackInfo.PlaybackStatus() ==
                            PlaybackStatus::Playing;

                        // PlaybackRate is a regular double in the current
                        // WinRT projection. If unavailable, keep 1.0.
                        auto rateRef = playbackInfo.PlaybackRate();
                        const double rate = rateRef ? rateRef.Value() : 1.0;
                        if (IsFiniteDouble(rate) && rate > 0.0)
                            playbackRate = rate;
                    } catch (...) {
                        std::lock_guard<std::mutex> lock(g_lyricsMutex);
                        playing = g_lyricsPlaying;
                        playbackRate = g_lyricsPlaybackRate;
                    }

                    if (haveTimeline && playing) {
                        try {
                            const auto updated =
                                winrt::clock::to_sys(timelineUpdatedTime);
                            const auto now = std::chrono::system_clock::now();
                            double ageSeconds =
                                std::chrono::duration<double>(now - updated).count();

                            if (IsFiniteDouble(ageSeconds)) {
                                ageSeconds = std::clamp(ageSeconds, 0.0, 5.0);
                                position += ageSeconds * playbackRate;
                            }
                        } catch (...) {
                        }
                    }
                }
            } catch (...) {
                haveSession = false;
            }

            if (!haveSession) {
                cachedMediaTitle.clear();
                cachedMediaArtist.clear();
                cachedMediaAlbum.clear();
                lastMediaPropertiesMs = 0;
                {
                    std::lock_guard<std::mutex> lock(g_lyricsMutex);
                    g_lyricsLines.reset();
                    g_lyricsTrackTitle.clear();
                    g_lyricsTrackArtist.clear();
                    g_lyricsTrackKey.clear();
                    g_lyricsHasSynced = false;
                    g_lyricsAvailable = false;
                    g_lyricsAvailableFast.store(g_lyricsAvailable, std::memory_order_release);
                    g_lyricsPositionSeconds = 0.0;
                    g_lyricsDurationSeconds = 0.0;
                    g_lyricsPositionAnchorTickMs = 0;
                    g_lyricsPlaying = false;
                    g_lyricsPlaybackRate = 1.0;
                }
                lastTrackKey.clear();
            } else {
                const std::wstring trackKey =
                    artist + L"\n" + title + L"\n" + album + L"\n" +
                    std::to_wstring(
                        static_cast<long long>(
                            std::llround(duration)));

                {
                    std::lock_guard<std::mutex> lock(g_lyricsMutex);
                    g_lyricsTrackTitle = title;
                    g_lyricsTrackArtist = artist;
                }

                SetLyricsTimelineState(
                    position,
                    duration,
                    playbackRate,
                    playing);

                const ULONGLONG now = GetTickCount64();

                if (!title.empty() && !artist.empty() &&
                    trackKey != lastTrackKey &&
                    now - lastFetchMs >= 300) {

                    lastTrackKey = trackKey;
                    lastFetchMs = now;

                    {
                        std::lock_guard<std::mutex> lock(g_lyricsMutex);
                        g_lyricsTrackKey = trackKey;
                        g_lyricsLines.reset();
                        g_lyricsHasSynced = false;
                        g_lyricsAvailable = false;
                        g_lyricsAvailableFast.store(g_lyricsAvailable, std::memory_order_release);
                    }

                    FetchLyricsForTrack(
                        title,
                        artist,
                        album,
                        duration,
                        trackKey);
                }
            }

            if (g_hLyricsStopEvent) {
                if (WaitForSingleObject(
                        g_hLyricsStopEvent, 500) == WAIT_OBJECT_0) {
                    break;
                }
            } else {
                Sleep(500);
            }
        }
    } catch (...) {
        ClearLyricsState();
    }

    if (winrtApartmentInitialized)
        winrt::uninit_apartment();
    return 0;
}

static void StartLyricsCapture() {
    if (g_lyricsRunning.exchange(true, std::memory_order_acq_rel))
        return;

    if (g_hLyricsStopEvent)
        CloseHandle(g_hLyricsStopEvent);
    g_hLyricsStopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_hLyricsStopEvent) {
        g_lyricsRunning.store(false, std::memory_order_release);
        return;
    }

    g_hLyricsThread =
        CreateThread(nullptr, 0, LyricsThreadProc, nullptr, 0, nullptr);

    if (!g_hLyricsThread) {
        g_lyricsRunning.store(false, std::memory_order_release);
        CloseHandle(g_hLyricsStopEvent);
        g_hLyricsStopEvent = nullptr;
    }
}

static void StopLyricsCapture() {
    if (!g_lyricsRunning.exchange(false, std::memory_order_acq_rel))
        return;

    if (g_hLyricsStopEvent)
        SetEvent(g_hLyricsStopEvent);

    if (g_hLyricsThread) {
        WaitForSingleObject(g_hLyricsThread, INFINITE);
        CloseHandle(g_hLyricsThread);
        g_hLyricsThread = nullptr;
    }

    if (g_hLyricsStopEvent) {
        CloseHandle(g_hLyricsStopEvent);
        g_hLyricsStopEvent = nullptr;
    }

    ClearLyricsState();
}

static int GetCurrentLyricsLineIndex(
    const std::vector<LyricsLine>& lines,
    double position) {

    if (lines.empty())
        return -1;

    size_t lo = 0;
    size_t hi = lines.size();

    while (lo < hi) {
        const size_t mid = lo + (hi - lo) / 2;
        if (lines[mid].timeSeconds <= position)
            lo = mid + 1;
        else
            hi = mid;
    }

    return lo == 0 ? -1 : static_cast<int>(lo - 1);
}

static UINT GetRenderIntervalMs(const VisualizerSettings& settings);

struct LyricsFontCache {
    std::wstring artistFontId;
    std::wstring lyricsFontId;
    int fontSize = 0;
    std::unique_ptr<Gdiplus::FontFamily> fallbackFamily;
    std::unique_ptr<Gdiplus::FontFamily> artistFamily;
    std::unique_ptr<Gdiplus::FontFamily> lyricsFamily;
    std::unique_ptr<Gdiplus::Font> lyricsFont;
    std::unique_ptr<Gdiplus::Font> artistFont;
    std::unique_ptr<Gdiplus::Font> titleFont;
    std::unique_ptr<Gdiplus::Font> aboveFont;
    std::unique_ptr<Gdiplus::Font> belowFont;

    void Clear() {
        belowFont.reset();
        aboveFont.reset();
        titleFont.reset();
        artistFont.reset();
        lyricsFont.reset();
        lyricsFamily.reset();
        artistFamily.reset();
        fallbackFamily.reset();
        artistFontId.clear();
        lyricsFontId.clear();
        fontSize = 0;
    }
};

// GDI+ owns process-global native resources. Do not let a CRT global destructor
// run after Explorer has already torn down its graphics/runtime state; the
// overlay thread explicitly clears this cache before GdiplusShutdown.
[[clang::no_destroy]] static LyricsFontCache g_lyricsFontCache;

// Cache stable GDI+ glyph measurements. Artist/title sizes only change when
// the track text or font settings change, and the current wrapped-line size
// only changes when the current lyric line, width, or font changes.
struct LyricsMeasureCache {
    std::wstring trackKey;
    std::wstring artist;
    std::wstring title;
    std::wstring artistFontId;
    std::wstring lyricsFontId;
    int fontSize = 0;
    Gdiplus::RectF artistBounds{};
    Gdiplus::RectF titleBounds{};
    Gdiplus::RectF separatorBounds{};
    bool valid = false;

    void Clear() {
        trackKey.clear();
        artist.clear();
        title.clear();
        artistFontId.clear();
        lyricsFontId.clear();
        fontSize = 0;
        artistBounds = {};
        titleBounds = {};
        separatorBounds = {};
        valid = false;
    }
};

[[clang::no_destroy]] static LyricsMeasureCache g_lyricsMeasureCache;

struct LyricsWrapMeasureCache {
    std::wstring trackKey;
    std::wstring lineText;
    std::wstring lyricsFontId;
    int lineIndex = -1;
    int contentWidth = 0;
    int fontSize = 0;
    int linesFilled = 1;
    float height = 0.0f;
    bool valid = false;

    void Clear() {
        trackKey.clear();
        lineText.clear();
        lyricsFontId.clear();
        lineIndex = -1;
        contentWidth = 0;
        fontSize = 0;
        linesFilled = 1;
        height = 0.0f;
        valid = false;
    }
};

[[clang::no_destroy]] static LyricsWrapMeasureCache g_lyricsWrapMeasureCache;

static const wchar_t* ResolveLyricsFontName(const std::wstring& id) {
    if (id == L"arial") return L"Arial";
    if (id == L"calibri") return L"Calibri";
    if (id == L"tahoma") return L"Tahoma";
    if (id == L"verdana") return L"Verdana";
    if (id == L"trebuchet_ms") return L"Trebuchet MS";
    if (id == L"georgia") return L"Georgia";
    if (id == L"consolas") return L"Consolas";
    if (id == L"times_new_roman") return L"Times New Roman";
    if (id == L"meiryo") return L"Meiryo";
    return L"Segoe UI";
}

static bool EnsureLyricsFontCache() {
    if (g_lyricsFontCache.lyricsFont &&
        g_lyricsFontCache.artistFont &&
        g_lyricsFontCache.titleFont &&
        g_lyricsFontCache.aboveFont &&
        g_lyricsFontCache.belowFont &&
        g_lyricsFontCache.artistFontId == g_settings.lyricsArtistFont &&
        g_lyricsFontCache.lyricsFontId == g_settings.lyricsLyricsFont &&
        g_lyricsFontCache.fontSize == g_settings.lyricsFontSize) {
        return true;
    }

    LyricsFontCache cache;
    cache.artistFontId = g_settings.lyricsArtistFont;
    cache.lyricsFontId = g_settings.lyricsLyricsFont;
    cache.fontSize = g_settings.lyricsFontSize;

    cache.fallbackFamily = std::make_unique<Gdiplus::FontFamily>(L"Segoe UI");
    cache.artistFamily = std::make_unique<Gdiplus::FontFamily>(
        ResolveLyricsFontName(cache.artistFontId));
    cache.lyricsFamily = std::make_unique<Gdiplus::FontFamily>(
        ResolveLyricsFontName(cache.lyricsFontId));

    const Gdiplus::FontFamily* artistFamily =
        cache.artistFamily->GetLastStatus() == Gdiplus::Ok
            ? cache.artistFamily.get() : cache.fallbackFamily.get();
    const Gdiplus::FontFamily* lyricsFamily =
        cache.lyricsFamily->GetLastStatus() == Gdiplus::Ok
            ? cache.lyricsFamily.get() : cache.fallbackFamily.get();

    cache.lyricsFont = std::make_unique<Gdiplus::Font>(
        lyricsFamily, static_cast<Gdiplus::REAL>(cache.fontSize),
        Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    cache.artistFont = std::make_unique<Gdiplus::Font>(
        artistFamily,
        static_cast<Gdiplus::REAL>(std::max(10, cache.fontSize - 5)),
        Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    cache.titleFont = std::make_unique<Gdiplus::Font>(
        artistFamily,
        static_cast<Gdiplus::REAL>(std::max(10, cache.fontSize - 5)),
        Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    cache.aboveFont = std::make_unique<Gdiplus::Font>(
        lyricsFamily, static_cast<Gdiplus::REAL>(cache.fontSize) * 0.88f,
        Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    cache.belowFont = std::make_unique<Gdiplus::Font>(
        lyricsFamily, static_cast<Gdiplus::REAL>(cache.fontSize) * 0.92f,
        Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    if (!cache.lyricsFont || cache.lyricsFont->GetLastStatus() != Gdiplus::Ok ||
        !cache.artistFont || cache.artistFont->GetLastStatus() != Gdiplus::Ok ||
        !cache.titleFont || cache.titleFont->GetLastStatus() != Gdiplus::Ok ||
        !cache.aboveFont || cache.aboveFont->GetLastStatus() != Gdiplus::Ok ||
        !cache.belowFont || cache.belowFont->GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    g_lyricsFontCache = std::move(cache);
    return true;
}

static void DrawLyricsWidget(Gdiplus::Graphics& graphics) {
    if (!g_settings.lyricsEnabled || g_settings.lyricsOpacity <= 0)
        return;

    std::shared_ptr<const std::vector<LyricsLine>> linesSnapshot;
    std::wstring title, artist, trackKey;
    bool available = false;
    {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        linesSnapshot = g_lyricsLines;
        title = g_lyricsTrackTitle;
        artist = g_lyricsTrackArtist;
        trackKey = g_lyricsTrackKey;
        available = g_lyricsAvailable;
    }

    static const std::vector<LyricsLine> emptyLyrics;
    const std::vector<LyricsLine>& lines = linesSnapshot ? *linesSnapshot : emptyLyrics;

    const int x = g_settings.lyricsX;
    const int y = g_settings.lyricsY;
    const int w = g_settings.lyricsWidth;
    const int configuredH = g_settings.lyricsHeight;
    if (w <= 0 || configuredH <= 0)
        return;

    if (!available && g_settings.lyricsUnavailableBehavior == 1)
        return;

    const bool collapsedUnavailable =
        !available && g_settings.lyricsUnavailableBehavior == 2;
    const int collapsedHeight = std::max(
        40, g_settings.lyricsFontSize + 18);
    const int h = collapsedUnavailable
        ? std::min(configuredH, collapsedHeight)
        : configuredH;

    const BYTE totalAlpha = static_cast<BYTE>(
        std::clamp(g_settings.lyricsOpacity, 0, 100) * 255 / 100);
    Gdiplus::GraphicsState state = graphics.Save();

    if (g_settings.lyricsBackgroundEnabled &&
        g_settings.lyricsBackgroundOpacity > 0) {
        const BYTE alpha = static_cast<BYTE>(
            std::clamp(g_settings.lyricsBackgroundOpacity, 0, 100) * 255 / 100 *
            totalAlpha / 255);
        Gdiplus::GraphicsPath path;
        const float r = static_cast<float>(
            std::min(g_settings.lyricsRounding, std::min(w, h) / 2));
        AddRoundedRectSubpath(
            path, static_cast<float>(x), static_cast<float>(y),
            static_cast<float>(w), static_cast<float>(h), r);
        if (g_settings.lyricsBackgroundMode == 2 ||
            g_settings.lyricsBackgroundMode == 3) {
            const DWORD albumPrimary = GetAlbumPalettePrimary();
            const DWORD albumSecondary = GetAlbumPaletteSecondary();

            if (g_settings.lyricsBackgroundMode == 2) {
                Gdiplus::SolidBrush bg(Gdiplus::Color(
                    alpha,
                    GetRValue(albumPrimary),
                    GetGValue(albumPrimary),
                    GetBValue(albumPrimary)));
                graphics.FillPath(&bg, &path);
            } else {
                Gdiplus::LinearGradientBrush bg(
                    Gdiplus::PointF(static_cast<float>(x), static_cast<float>(y)),
                    Gdiplus::PointF(static_cast<float>(x + w), static_cast<float>(y + h)),
                    Gdiplus::Color(
                        alpha,
                        GetRValue(albumPrimary),
                        GetGValue(albumPrimary),
                        GetBValue(albumPrimary)),
                    Gdiplus::Color(
                        alpha,
                        GetRValue(albumSecondary),
                        GetGValue(albumSecondary),
                        GetBValue(albumSecondary)));
                graphics.FillPath(&bg, &path);
            }
        } else if (g_settings.lyricsBackgroundMode == 1) {
            Gdiplus::LinearGradientBrush bg(
                Gdiplus::PointF(static_cast<float>(x), static_cast<float>(y)),
                Gdiplus::PointF(static_cast<float>(x + w), static_cast<float>(y + h)),
                Gdiplus::Color(
                    alpha,
                    GetRValue(g_settings.lyricsBackgroundColor1),
                    GetGValue(g_settings.lyricsBackgroundColor1),
                    GetBValue(g_settings.lyricsBackgroundColor1)),
                Gdiplus::Color(
                    alpha,
                    GetRValue(g_settings.lyricsBackgroundColor2),
                    GetGValue(g_settings.lyricsBackgroundColor2),
                    GetBValue(g_settings.lyricsBackgroundColor2)));
            graphics.FillPath(&bg, &path);
        } else {
            Gdiplus::SolidBrush bg(Gdiplus::Color(
                alpha,
                GetRValue(g_settings.lyricsBackgroundColor1),
                GetGValue(g_settings.lyricsBackgroundColor1),
                GetBValue(g_settings.lyricsBackgroundColor1)));
            graphics.FillPath(&bg, &path);
        }

        if (g_settings.lyricsBorderEnabled &&
            g_settings.lyricsBorderOpacity > 0 &&
            g_settings.lyricsBackgroundEnabled) {
            const BYTE borderAlpha = static_cast<BYTE>(
                std::clamp(g_settings.lyricsBorderOpacity, 0, 100) * 255 / 100 *
                totalAlpha / 255);
            const DWORD albumPrimary = GetAlbumPalettePrimary();
            const DWORD albumSecondary = GetAlbumPaletteSecondary();

            if (g_settings.lyricsBorderMode == 1 ||
                g_settings.lyricsBorderMode == 3) {
                DWORD c1 = (g_settings.lyricsBorderMode == 1)
                    ? albumPrimary
                    : g_settings.lyricsBorderColor1;
                DWORD c2 = (g_settings.lyricsBorderMode == 1)
                    ? albumSecondary
                    : g_settings.lyricsBorderColor2;

                Gdiplus::LinearGradientBrush borderBrush(
                    Gdiplus::PointF(static_cast<float>(x), static_cast<float>(y)),
                    Gdiplus::PointF(static_cast<float>(x + w), static_cast<float>(y + h)),
                    Gdiplus::Color(
                        borderAlpha,
                        GetRValue(c1), GetGValue(c1), GetBValue(c1)),
                    Gdiplus::Color(
                        borderAlpha,
                        GetRValue(c2), GetGValue(c2), GetBValue(c2)));

                Gdiplus::Pen pen(&borderBrush, static_cast<Gdiplus::REAL>(
                    g_settings.lyricsBorderThickness));
                graphics.DrawPath(&pen, &path);
            } else {
                DWORD color =
                    (g_settings.lyricsBorderMode == 0)
                    ? albumPrimary
                    : g_settings.lyricsBorderColor1;

                Gdiplus::Pen pen(
                    Gdiplus::Color(
                        borderAlpha,
                        GetRValue(color),
                        GetGValue(color),
                        GetBValue(color)),
                    static_cast<Gdiplus::REAL>(g_settings.lyricsBorderThickness));
                graphics.DrawPath(&pen, &path);
            }
        }
    }

    if (!EnsureLyricsFontCache()) {
        graphics.Restore(state);
        return;
    }

    const Gdiplus::Font& font = *g_lyricsFontCache.lyricsFont;
    const Gdiplus::Font& artistFont = *g_lyricsFontCache.artistFont;
    const Gdiplus::Font& titleFont = *g_lyricsFontCache.titleFont;
    const Gdiplus::Font& aboveFont = *g_lyricsFontCache.aboveFont;
    const Gdiplus::Font& belowFont = *g_lyricsFontCache.belowFont;

    Gdiplus::StringFormat textFormat;
    if (g_settings.lyricsTextAlignment == 0)
        textFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    else if (g_settings.lyricsTextAlignment == 2)
        textFormat.SetAlignment(Gdiplus::StringAlignmentFar);
    else
        textFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
    textFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    textFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    textFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

    const float contentLeft = static_cast<float>(x + 18);
    const float contentWidth = static_cast<float>(std::max(1, w - 36));

    const bool drawArtist =
        g_settings.lyricsShowArtist && !artist.empty();
    const bool drawTitle =
        g_settings.lyricsShowTitle && !title.empty();

    if (drawArtist || drawTitle) {
        const std::wstring separator = L"  •  ";

        Gdiplus::RectF artistBounds{}, titleBounds{}, separatorBounds{};
        Gdiplus::StringFormat measureFormat;
        measureFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        measureFormat.SetTrimming(Gdiplus::StringTrimmingNone);

        const bool measureCacheHit =
            g_lyricsMeasureCache.valid &&
            g_lyricsMeasureCache.trackKey == trackKey &&
            g_lyricsMeasureCache.artist == artist &&
            g_lyricsMeasureCache.title == title &&
            g_lyricsMeasureCache.artistFontId == g_settings.lyricsArtistFont &&
            g_lyricsMeasureCache.lyricsFontId == g_settings.lyricsLyricsFont &&
            g_lyricsMeasureCache.fontSize == g_settings.lyricsFontSize;

        if (measureCacheHit) {
            artistBounds = g_lyricsMeasureCache.artistBounds;
            titleBounds = g_lyricsMeasureCache.titleBounds;
            separatorBounds = g_lyricsMeasureCache.separatorBounds;
        } else {
            const Gdiplus::RectF measureRect(0.0f, 0.0f, 10000.0f, 100.0f);
            if (!artist.empty()) {
                graphics.MeasureString(artist.c_str(), -1, &artistFont, measureRect,
                                       &measureFormat, &artistBounds);
            }
            if (!title.empty()) {
                graphics.MeasureString(title.c_str(), -1, &titleFont, measureRect,
                                       &measureFormat, &titleBounds);
            }
            graphics.MeasureString(separator.c_str(), -1, &artistFont, measureRect,
                                   &measureFormat, &separatorBounds);

            g_lyricsMeasureCache.trackKey = trackKey;
            g_lyricsMeasureCache.artist = artist;
            g_lyricsMeasureCache.title = title;
            g_lyricsMeasureCache.artistFontId = g_settings.lyricsArtistFont;
            g_lyricsMeasureCache.lyricsFontId = g_settings.lyricsLyricsFont;
            g_lyricsMeasureCache.fontSize = g_settings.lyricsFontSize;
            g_lyricsMeasureCache.artistBounds = artistBounds;
            g_lyricsMeasureCache.titleBounds = titleBounds;
            g_lyricsMeasureCache.separatorBounds = separatorBounds;
            g_lyricsMeasureCache.valid = true;
        }

        const float metadataWidth =
            (drawArtist ? artistBounds.Width : 0.0f) +
            (drawArtist && drawTitle ? separatorBounds.Width : 0.0f) +
            (drawTitle ? titleBounds.Width : 0.0f);

        const float metadataY = static_cast<float>(y + 10);
        const float metadataHeight =
            static_cast<float>(g_settings.lyricsFontSize + 8);

        // Keep artist/title strictly inside the widget. If the complete line
        // is wider than the available content area, smoothly pan it from
        // left to right and back instead of letting it overflow the widget.
        static std::wstring lastMetadataTrackKey;
        static ULONGLONG metadataScrollElapsedMs = 0;
        static bool metadataScrollInitialized = false;

        const bool metadataFits = metadataWidth <= contentWidth;
        const float metadataOverflow =
            metadataFits ? 0.0f : (metadataWidth - contentWidth);

        if (!metadataScrollInitialized || trackKey != lastMetadataTrackKey) {
            lastMetadataTrackKey = trackKey;
            metadataScrollElapsedMs = 0;
            metadataScrollInitialized = true;
        }

        float metadataLeft = contentLeft;
        if (metadataFits) {
            if (g_settings.lyricsTextAlignment == 2) {
                metadataLeft = contentLeft + contentWidth - metadataWidth;
            } else if (g_settings.lyricsTextAlignment == 1) {
                metadataLeft = contentLeft + (contentWidth - metadataWidth) * 0.5f;
            }
        } else {
            // Advance the animation once per rendered frame and use the same
            // interval as Performance.targetFps. This keeps the marquee's
            // motion cadence tied to the widget's actual render cadence
            // instead of making it depend on irregular WM_TIMER arrival.
            constexpr ULONGLONG kMetadataPauseMs = 900;
            constexpr double kMetadataSpeedPxPerSecond = 55.0;
            constexpr ULONGLONG kMetadataTravelMs = 650;


            UINT renderIntervalMs = GetRenderIntervalMs(g_settings);
            if (renderIntervalMs == 0)
                renderIntervalMs = 1;

            // Advance by exactly one nominal render interval per rendered
            // frame. This makes the marquee use the same FPS cadence as
            // Performance.targetFps (or the monitor refresh rate when 0),
            // avoiding variable-sized position jumps from timer jitter.
            metadataScrollElapsedMs += static_cast<ULONGLONG>(renderIntervalMs);

            const double travelMs =
                (static_cast<double>(metadataOverflow) /
                 kMetadataSpeedPxPerSecond) * 1000.0;
            const ULONGLONG oneWayMs = static_cast<ULONGLONG>(
                std::max<double>(kMetadataTravelMs, travelMs));
            const ULONGLONG cycleMs =
                kMetadataPauseMs + oneWayMs + kMetadataPauseMs + oneWayMs;

            const ULONGLONG elapsedMs = metadataScrollElapsedMs % cycleMs;

            double offset = 0.0;
            if (elapsedMs < kMetadataPauseMs) {
                offset = 0.0;
            } else if (elapsedMs < kMetadataPauseMs + oneWayMs) {
                const double t =
                    static_cast<double>(elapsedMs - kMetadataPauseMs) /
                    static_cast<double>(oneWayMs);
                offset = metadataOverflow * t;
            } else if (elapsedMs <
                       kMetadataPauseMs + oneWayMs + kMetadataPauseMs) {
                offset = metadataOverflow;
            } else {
                const double t =
                    static_cast<double>(
                        elapsedMs - kMetadataPauseMs - oneWayMs - kMetadataPauseMs) /
                    static_cast<double>(oneWayMs);
                offset = metadataOverflow * (1.0 - t);
            }

            metadataLeft = contentLeft - static_cast<float>(offset);
        }

        Gdiplus::SolidBrush metaBrush(
            Gdiplus::Color(static_cast<BYTE>(totalAlpha * 0.65f),
                           235, 235, 235));

        Gdiplus::GraphicsState metadataClipState = graphics.Save();
        graphics.SetClip(
            Gdiplus::RectF(contentLeft, metadataY, contentWidth, metadataHeight),
            Gdiplus::CombineModeIntersect);

        if (drawArtist) {
            Gdiplus::RectF rect(
                metadataLeft, metadataY, artistBounds.Width, metadataHeight);
            graphics.DrawString(artist.c_str(), -1, &artistFont, rect,
                                &measureFormat, &metaBrush);
            metadataLeft += artistBounds.Width;
        }

        if (drawArtist && drawTitle) {
            Gdiplus::RectF rect(
                metadataLeft, metadataY, separatorBounds.Width, metadataHeight);
            graphics.DrawString(separator.c_str(), -1, &artistFont, rect,
                                &measureFormat, &metaBrush);
            metadataLeft += separatorBounds.Width;
        }

        if (drawTitle) {
            Gdiplus::RectF rect(
                metadataLeft, metadataY, titleBounds.Width, metadataHeight);
            graphics.DrawString(title.c_str(), -1, &titleFont, rect,
                                &measureFormat, &metaBrush);
        }

        graphics.Restore(metadataClipState);
    }

    if (!collapsedUnavailable && g_settings.lyricsShowLyrics) {
        const double position = GetCurrentLyricsPosition();

        const bool haveLyrics = available && !lines.empty();
        int current = haveLyrics ? GetCurrentLyricsLineIndex(lines, position) : -1;

        if (haveLyrics) {
            if (current < 0)
                current = 0;
        }

        const float lineHeight =
            static_cast<float>(g_settings.lyricsFontSize + 11);

        Gdiplus::StringFormat wrapFormat;
        wrapFormat.SetAlignment(textFormat.GetAlignment());
        wrapFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        wrapFormat.SetTrimming(Gdiplus::StringTrimmingNone);

        float currentBlockHeight = lineHeight;
        if (haveLyrics && g_settings.lyricsLongLineWrapEnabled && current >= 0 &&
            current < static_cast<int>(lines.size())) {
            const std::wstring& currentLineText = lines[current].text;
            const bool wrapCacheHit =
                g_lyricsWrapMeasureCache.valid &&
                g_lyricsWrapMeasureCache.trackKey == trackKey &&
                g_lyricsWrapMeasureCache.lineIndex == current &&
                g_lyricsWrapMeasureCache.lineText == currentLineText &&
                g_lyricsWrapMeasureCache.contentWidth == static_cast<int>(contentWidth) &&
                g_lyricsWrapMeasureCache.fontSize == g_settings.lyricsFontSize &&
                g_lyricsWrapMeasureCache.lyricsFontId == g_settings.lyricsLyricsFont;

            int linesFilled = 1;
            float measuredHeight = lineHeight;
            if (wrapCacheHit) {
                linesFilled = g_lyricsWrapMeasureCache.linesFilled;
                measuredHeight = g_lyricsWrapMeasureCache.height;
            } else {
                Gdiplus::RectF measured;
                const Gdiplus::RectF measureRect(0.0f, 0.0f, contentWidth, 10000.0f);
                INT codePointsFitted = 0;
                INT measuredLinesFilled = 0;
                if (graphics.MeasureString(
                        currentLineText.c_str(), -1, &font, measureRect,
                        &wrapFormat, &measured, &codePointsFitted, &measuredLinesFilled) == Gdiplus::Ok) {
                    linesFilled = std::max(1, measuredLinesFilled);
                    measuredHeight = measured.Height;
                }

                g_lyricsWrapMeasureCache.trackKey = trackKey;
                g_lyricsWrapMeasureCache.lineText = currentLineText;
                g_lyricsWrapMeasureCache.lineIndex = current;
                g_lyricsWrapMeasureCache.contentWidth = static_cast<int>(contentWidth);
                g_lyricsWrapMeasureCache.fontSize = g_settings.lyricsFontSize;
                g_lyricsWrapMeasureCache.lyricsFontId = g_settings.lyricsLyricsFont;
                g_lyricsWrapMeasureCache.linesFilled = linesFilled;
                g_lyricsWrapMeasureCache.height = measuredHeight;
                g_lyricsWrapMeasureCache.valid = true;
            }

            if (linesFilled > 1) {
                currentBlockHeight = std::max(
                    lineHeight * static_cast<float>(linesFilled), measuredHeight + 2.0f);
            }
        }

        if (!haveLyrics) {
            std::wstring fallback;
            if (g_settings.lyricsUnavailableText.empty()) {
                // Empty Fallback means "pick a random emoticon for this track".
                // Keep the selected emoticon until the track changes, so it does
                // not change every render frame. Never select the same one twice
                // in a row when moving to another track.
                static const wchar_t* kLyricsFallbackEmoticons[] = {
                    // Shrug
                    L"┐(￣∀￣)┌",
                    L"┐(°ヮ°)┌",
                    L"╮(─▽─)╭",
                    L"┐( ˘_˘ )┌",
                    L"╮(╯▽╰)╭",
                    L"¯\\_(ツ)_/¯",
                    L"¯\\_(•-•)_/¯",
                    L"¯\\_(⊙_⊙)_/¯",
                    L"¯\\_(º_o)_/¯",
                    L"┐(′～′)┌",
                    L"¯\\_(⇀_↼)_/¯",
                    L"╮(. . )╭",
                    L"┐(i_i)┌",
                    L"╮(u_u)╭",

                    // Angry
                    L"(ಠ益ಠ)",
                    L"(ಠ_ಠ)",
                    L"(¬_¬)",
                    L"(＃`Д´)",
                    L"(╬ಠ益ಠ)",
                    L"(ノಠ益ಠ)ノ彡┻━┻",
                    L"┻━┻ ︵ヽ(`Д´)ﾉ︵ ┻━┻",
                    L"(ᗒᗣᗕ)՞",
                    L"(｀Д´)",
                    L"(╯°□°)╯︵",
                    L"(‡▼益▼)",
                    L"(＃ﾟДﾟ)",
                    L"(ಠ╭╮ಠ)",


                    // Sad
                    L"(T_T)",
                    L"(;_;)",
                    L"(ಥ_ಥ)",
                    L"(；＿；)",
                    L"(╥﹏╥)",
                    L"(｡•́︿•̀｡)",
                    L"(っ˘̩╭╮˘̩)っ",
                    L"(இ﹏இ`｡)",
                    L"(´;︵;`)",
                    L"(ノ_<。)",
                    L"(ಥ﹏ಥ)",
                    L"(つ﹏⊂)",
                    L"( ᵕ̩̩ㅅᵕ̩̩ )",
                    L"(╯︵╰,)",

                    // Surprised
                    L"(O_O)",
                    L"(o_O)",
                    L"(⊙_⊙)",
                    L"(☉_☉)",
                    L"(°Д°)",
                    L"(゜ロ゜)",
                    L"(ﾟдﾟ)",
                    L"(⊙o⊙)",
                    L"(ㆆ_ㆆ)",
                    L"(°ロ°) !",
                    L"Σ(°△°|||)",
                    L"Σ(ﾟДﾟ)",
                    L"(⊙_◎)",
                    L"(•᷄ࡇ•᷅)",

                    // Thinking
                    L"(・_・?)",
                    L"(・・?)",
                    L"(￣～￣;)",
                    L"(￢_￢)",
                    L"(˘･_･˘)",
                    L"(´･_･`)",
                    L"(─_─)?",
                    L"(•ิ_•ิ)?",
                    L"(⊙﹏⊙)",
                    L"(・・;)",
                    L"(￣ω￣;)",
                    L"(｡･_･｡)",
                    L"(｀・ω・´)?",
                    L"(・・ )?"
                };
                constexpr size_t kLyricsFallbackEmoticonCount =
                    sizeof(kLyricsFallbackEmoticons) /
                    sizeof(kLyricsFallbackEmoticons[0]);

                static std::wstring lastFallbackTrackKey;
                static std::wstring currentFallbackEmoticon;
                static int lastFallbackIndex = -1;
                static std::mt19937 fallbackRng(
                    static_cast<std::mt19937::result_type>(
                        GetTickCount64() ^
                        static_cast<ULONGLONG>(GetCurrentProcessId())));

                // During the tiny window before the lyrics worker publishes the
                // full track key, use artist+title. Once the full key arrives,
                // recognize that it is the same track so the emoticon is not
                // changed twice during one track switch.
                const std::wstring temporaryTrackKey =
                    artist + L"\n" + title;
                const std::wstring& fallbackTrackKey =
                    trackKey.empty() ? temporaryTrackKey : trackKey;

                bool sameTrackAfterMetadata = false;
                if (!trackKey.empty() &&
                    !lastFallbackTrackKey.empty() &&
                    lastFallbackTrackKey + L"\n" == trackKey.substr(
                        0, lastFallbackTrackKey.size() + 1)) {
                    sameTrackAfterMetadata = true;
                }

                if (fallbackTrackKey != lastFallbackTrackKey) {
                    if (!sameTrackAfterMetadata ||
                        currentFallbackEmoticon.empty()) {
                        std::uniform_int_distribution<size_t> distribution(
                            0, kLyricsFallbackEmoticonCount - 1);
                        size_t nextIndex = distribution(fallbackRng);
                        if (kLyricsFallbackEmoticonCount > 1) {
                            while (static_cast<int>(nextIndex) == lastFallbackIndex)
                                nextIndex = distribution(fallbackRng);
                        }

                        lastFallbackIndex = static_cast<int>(nextIndex);
                        currentFallbackEmoticon =
                            kLyricsFallbackEmoticons[nextIndex];
                    }

                    lastFallbackTrackKey = fallbackTrackKey;
                } else if (currentFallbackEmoticon.empty()) {
                    std::uniform_int_distribution<size_t> distribution(
                        0, kLyricsFallbackEmoticonCount - 1);
                    size_t nextIndex = distribution(fallbackRng);
                    if (kLyricsFallbackEmoticonCount > 1) {
                        while (static_cast<int>(nextIndex) == lastFallbackIndex)
                            nextIndex = distribution(fallbackRng);
                    }

                    lastFallbackIndex = static_cast<int>(nextIndex);
                    currentFallbackEmoticon =
                        kLyricsFallbackEmoticons[nextIndex];
                }

                fallback = currentFallbackEmoticon;
            } else {
                fallback = g_settings.lyricsUnavailableText;
            }

            Gdiplus::SolidBrush fallbackBrush(
                Gdiplus::Color(static_cast<BYTE>(totalAlpha * 0.78f),
                               245, 245, 248));
            const float fallbackY = y + h *
                (static_cast<float>(std::clamp(g_settings.lyricsFocusY, 0, 100)) / 100.0f);

            if (g_settings.lyricsUnavailableText.empty()) {
                // The built-in random fallback is intentionally a two-line block:
                // the selected kaomoji on top and a larger, clearly separated "No lyrics" label below.
                const float emoticonHeight = static_cast<float>(g_settings.lyricsFontSize + 8);
                const float labelFontSize = std::max(14.0f,
                    static_cast<float>(g_settings.lyricsFontSize) * 1.16f);
                // Font::GetFamily() writes into a FontFamily object and returns a Status;
                // it does not return a FontFamily pointer suitable for the Font constructor.
                Gdiplus::FontFamily noLyricsFamily;
                font.GetFamily(&noLyricsFamily);
                Gdiplus::Font noLyricsFont(
                    &noLyricsFamily, labelFontSize, Gdiplus::FontStyleRegular,
                    Gdiplus::UnitPixel);

                Gdiplus::RectF emoticonRect(
                    contentLeft,
                    fallbackY - static_cast<float>(g_settings.lyricsFontSize) * 0.78f,
                    contentWidth,
                    emoticonHeight);
                graphics.DrawString(
                    fallback.c_str(), -1, &font, emoticonRect, &textFormat,
                    &fallbackBrush);

                Gdiplus::RectF labelRect(
                    contentLeft,
                    fallbackY + static_cast<float>(g_settings.lyricsFontSize) * 0.62f,
                    contentWidth,
                    labelFontSize + 10.0f);
                graphics.DrawString(
                    L"No lyrics", -1, &noLyricsFont, labelRect, &textFormat,
                    &fallbackBrush);
            } else {
                // User-provided fallback text keeps the original single-line behavior.
                Gdiplus::RectF fallbackRect(
                    contentLeft,
                    fallbackY - static_cast<float>(g_settings.lyricsFontSize) * 0.7f,
                    contentWidth,
                    static_cast<float>(g_settings.lyricsFontSize + 12));
                graphics.DrawString(
                    fallback.c_str(), -1, &font, fallbackRect, &textFormat,
                    &fallbackBrush);
            }
        } else {
            const int topCount = g_settings.lyricsLinesAbove;
            const int bottomCount = g_settings.lyricsLinesBelow;
            const float focusRatio = static_cast<float>(
                std::clamp(g_settings.lyricsFocusY, 0, 100)) / 100.0f;
            const float centerY = y + h * focusRatio;

            static std::wstring lastTrackKey;
            static int lastCurrent = -2;
            static double lastPosition = -1.0;
            static ULONGLONG transitionStartMs = 0;
            static int transitionDirection = 0;
            static bool transitionActive = false;

            const ULONGLONG nowMs = GetTickCount64();
            const bool trackChanged = trackKey != lastTrackKey;

            if (trackChanged || lastCurrent == -2) {
                lastTrackKey = trackKey;
                lastCurrent = current;
                lastPosition = position;
                transitionActive = false;
                transitionDirection = 0;
            } else if (current != lastCurrent) {
                const int indexDelta = current - lastCurrent;
                const double positionDelta = position - lastPosition;

                const bool normalPlaybackStep =
                    std::abs(indexDelta) == 1 &&
                    std::isfinite(positionDelta) &&
                    positionDelta > -0.08 && positionDelta < 0.75;

                if (normalPlaybackStep) {
                    transitionStartMs = nowMs;
                    transitionDirection = indexDelta > 0 ? 1 : -1;
                    transitionActive = true;
                } else {
                    transitionActive = false;
                    transitionDirection = 0;
                }

                lastCurrent = current;
            }

            float transition = 1.0f;
            if (transitionActive) {
                constexpr float kLyricsTransitionMs = 320.0f;
                const float elapsed = static_cast<float>(
                    nowMs >= transitionStartMs ? nowMs - transitionStartMs : 0);
                const float t = std::clamp(elapsed / kLyricsTransitionMs, 0.0f, 1.0f);
                // Ease-out cubic: quick initial movement, smooth settle at focus.
                transition = 1.0f - std::pow(1.0f - t, 3.0f);
                if (t >= 1.0f) {
                    transitionActive = false;
                    transitionDirection = 0;
                }
            }

            const auto GetSlotY = [&](int slot, float blockHeight) {
                if (slot == 0)
                    return centerY;
                if (slot < 0) {
                    return centerY - currentBlockHeight * 0.5f
                        - (std::abs(slot) - 0.5f) * lineHeight;
                }
                return centerY + currentBlockHeight * 0.5f
                    + (slot - 0.5f) * lineHeight;
            };

            lastPosition = position;

            constexpr float kTopViewportInset = 10.0f;
            const float lyricViewportTop =
                centerY - currentBlockHeight * 0.5f
                - (static_cast<float>(topCount) + 0.5f) * lineHeight
                + kTopViewportInset;
            const float lyricViewportBottom =
                centerY + currentBlockHeight * 0.5f
                + (static_cast<float>(bottomCount) + 0.5f) * lineHeight;
            const Gdiplus::RectF lyricClipRect(
                contentLeft,
                lyricViewportTop,
                contentWidth,
                std::max(1.0f, lyricViewportBottom - lyricViewportTop));
            graphics.SetClip(lyricClipRect, Gdiplus::CombineModeIntersect);

            const int firstSlot =
                -topCount - (transitionActive && transitionDirection > 0 ? 1 : 0);
            const int lastSlot =
                bottomCount + (transitionActive && transitionDirection < 0 ? 1 : 0);

            for (int slot = firstSlot; slot <= lastSlot; ++slot) {
                const int idx = current + slot;
                if (idx < 0 || idx >= static_cast<int>(lines.size()))
                    continue;

                const bool isCurrentLine = (slot == 0);

                BYTE alpha = totalAlpha;
                if (slot < 0)
                    alpha = static_cast<BYTE>(totalAlpha * 0.42f);
                else if (slot > 0)
                    alpha = static_cast<BYTE>(totalAlpha * 0.58f);

                const Gdiplus::Font* lineFont = &font;
                if (slot < 0)
                    lineFont = &aboveFont;
                else if (slot > 0)
                    lineFont = &belowFont;

                Gdiplus::SolidBrush brush(
                    Gdiplus::Color(alpha, 245, 245, 248));

                float lineY = GetSlotY(slot, currentBlockHeight);

                if (transitionActive) {

                    const int oldSlot = slot + transitionDirection;
                    const float oldY = GetSlotY(oldSlot, currentBlockHeight);
                    lineY = oldY + (lineY - oldY) * transition;
                }

                const float blockHeight = isCurrentLine
                    ? currentBlockHeight
                    : lineHeight;

                Gdiplus::RectF rect(
                    contentLeft,
                    lineY - blockHeight * 0.5f,
                    contentWidth,
                    blockHeight);

                if (isCurrentLine && g_settings.lyricsLongLineWrapEnabled) {
                    graphics.DrawString(
                        lines[idx].text.c_str(), -1, lineFont, rect,
                        &wrapFormat, &brush);
                } else {
                    graphics.DrawString(
                        lines[idx].text.c_str(), -1, lineFont, rect, &textFormat,
                        &brush);
                }
            }
        }
    }

    graphics.Restore(state);
}

static float GetCurrentVisualizerPeakRatio(int barCount) {
    barCount = std::clamp(barCount, 1, VIZ_BANDS_MAX);
    const float maxHeight = static_cast<float>(
        std::max(1, g_settings.maxBarHeight));

    float peak = 0.0f;
    for (int i = 0; i < barCount; ++i) {
        peak = std::max(
            peak,
            std::clamp(g_currentHeights[i] / maxHeight, 0.0f, 1.0f));
    }

    const float minRatio = std::clamp(
        static_cast<float>(g_settings.minBarHeight) / maxHeight,
        0.0f, 1.0f);
    return std::max(minRatio, peak);
}

static RECT GetVisualizerBackgroundRect(
    int barCount,
    float heightRatio,
    int padding,
    int heightAdjustment) {

    barCount = std::clamp(barCount, 1, VIZ_BANDS_MAX);
    padding = std::max(0, padding);

    const int baseHeight = std::max(
        1,
        static_cast<int>(std::round(
            g_settings.maxBarHeight * std::clamp(heightRatio, 0.0f, 1.0f))));
    const int maxHeight = std::max(1, baseHeight + heightAdjustment);
    int stripSize = std::max(
        g_settings.barWidth,
        barCount * std::max(1, g_settings.barWidth) +
            std::max(0, barCount - 1) * std::max(0, g_settings.barSpacing));

    if (g_settings.barStyle == 3)
        stripSize = std::max(1, g_settings.curveWidth);

    RECT rect{};

    if (g_settings.barShape == 4) {
        const int radius = std::max(0, g_settings.circleRadius);
        const int outerRadius = radius + maxHeight;
        rect = {
            g_settings.positionX - outerRadius,
            g_settings.positionY - outerRadius,
            g_settings.positionX + outerRadius,
            g_settings.positionY + outerRadius
        };
    } else if (g_settings.orientation <= 2) {

        rect.left = g_settings.positionX;
        rect.right = g_settings.positionX + stripSize;

        if (g_settings.orientation == 0) {
            rect.top = g_settings.positionY - maxHeight;
            rect.bottom = g_settings.positionY;
        } else if (g_settings.orientation == 1) {
            rect.top = g_settings.positionY - maxHeight / 2;
            rect.bottom = g_settings.positionY + maxHeight / 2;
        } else {
            rect.top = g_settings.positionY;
            rect.bottom = g_settings.positionY + maxHeight;
        }
    } else {

        rect.top = g_settings.positionY;
        rect.bottom = g_settings.positionY + stripSize;

        if (g_settings.orientation == 3) {
            rect.left = g_settings.positionX;
            rect.right = g_settings.positionX + maxHeight;
        } else if (g_settings.orientation == 4) {
            rect.left = g_settings.positionX - maxHeight / 2;
            rect.right = g_settings.positionX + maxHeight / 2;
        } else {
            rect.left = g_settings.positionX - maxHeight;
            rect.right = g_settings.positionX;
        }
    }

    rect.left -= padding;
    rect.top -= padding;
    rect.right += padding;
    rect.bottom += padding;
    return rect;
}

static void RenderVisualizerBackground(
    Gdiplus::Graphics& graphics,
    int barCount) {

    if (!g_settings.backgroundEnabled || g_settings.backgroundOpacity <= 0)
        return;

    const float heightRatio = 1.0f;

    RECT rect = GetVisualizerBackgroundRect(
        barCount,
        heightRatio,
        g_settings.backgroundPadding,
        g_settings.backgroundHeightAdjustment);

    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    const BYTE alpha = static_cast<BYTE>(std::clamp(
        255 * g_settings.backgroundOpacity / 100, 0, 255));

    DWORD c1 = g_settings.backgroundColor1;
    DWORD c2 = g_settings.backgroundColor2;

    if (g_settings.backgroundMode == 3) {
        c1 = GetAlbumPalettePrimary();
        c2 = c1;
    } else if (g_settings.backgroundMode == 4) {
        c1 = GetAlbumPalettePrimary();
        c2 = GetAlbumPaletteSecondary();
    }

    Gdiplus::GraphicsPath path;
    const int maxRadius = std::min(
        (rect.right - rect.left) / 2,
        (rect.bottom - rect.top) / 2);
    const int safeRadius = std::max(
        0,
        std::min(g_settings.backgroundCornerRadius, maxRadius));
    const float radius = static_cast<float>(safeRadius);

    AddRoundedRectSubpath(
        path,
        static_cast<float>(rect.left),
        static_cast<float>(rect.top),
        static_cast<float>(rect.right - rect.left),
        static_cast<float>(rect.bottom - rect.top),
        radius);

    if (g_settings.backgroundMode == 5) {
        EnsureBackgroundBlurBitmap(rect);

        if (g_pBackgroundBlurBitmap) {
            Gdiplus::GraphicsState clipState = graphics.Save();
            graphics.SetClip(&path, Gdiplus::CombineModeReplace);

            Gdiplus::ImageAttributes imageAttributes;
            Gdiplus::ColorMatrix colorMatrix = {
                1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f
            };
            imageAttributes.SetColorMatrix(
                &colorMatrix,
                Gdiplus::ColorMatrixFlagsDefault,
                Gdiplus::ColorAdjustTypeBitmap);

            graphics.DrawImage(
                g_pBackgroundBlurBitmap,
                Gdiplus::Rect(
                    rect.left,
                    rect.top,
                    rect.right - rect.left,
                    rect.bottom - rect.top),
                0, 0,
                g_pBackgroundBlurBitmap->GetWidth(),
                g_pBackgroundBlurBitmap->GetHeight(),
                Gdiplus::UnitPixel,
                &imageAttributes);

            graphics.Restore(clipState);
        }
    } else {
        Gdiplus::Color color1(
            alpha,
            GetRValue(c1), GetGValue(c1), GetBValue(c1));

        if (g_settings.backgroundMode == 0 || g_settings.backgroundMode == 3) {
            Gdiplus::SolidBrush brush(color1);
            graphics.FillPath(&brush, &path);
        } else {
            Gdiplus::Color color2(
                alpha,
                GetRValue(c2), GetGValue(c2), GetBValue(c2));

            const bool horizontalAxis = g_settings.orientation > 2;
            Gdiplus::LinearGradientBrush gradient(
                horizontalAxis
                    ? Gdiplus::PointF(
                        static_cast<float>(rect.left),
                        static_cast<float>(rect.top))
                    : Gdiplus::PointF(
                        static_cast<float>(rect.left),
                        static_cast<float>(rect.bottom)),
                horizontalAxis
                    ? Gdiplus::PointF(
                        static_cast<float>(rect.right),
                        static_cast<float>(rect.top))
                    : Gdiplus::PointF(
                        static_cast<float>(rect.left),
                        static_cast<float>(rect.top)),
                color1,
                color2);

            graphics.FillPath(&gradient, &path);
        }
    }

    if (g_settings.backgroundBorderEnabled &&
        g_settings.backgroundBorderOpacity > 0) {
        const BYTE borderAlpha = static_cast<BYTE>(
            std::clamp(g_settings.backgroundBorderOpacity, 0, 100) * 255 / 100);

        const DWORD albumPrimary = GetAlbumPalettePrimary();
        const DWORD albumSecondary = GetAlbumPaletteSecondary();

        if (g_settings.backgroundBorderMode == 1 ||
            g_settings.backgroundBorderMode == 3) {
            const DWORD border1 = (g_settings.backgroundBorderMode == 1)
                ? albumPrimary
                : g_settings.backgroundBorderColor1;
            const DWORD border2 = (g_settings.backgroundBorderMode == 1)
                ? albumSecondary
                : g_settings.backgroundBorderColor2;

            Gdiplus::LinearGradientBrush borderBrush(
                Gdiplus::PointF(
                    static_cast<float>(rect.left),
                    static_cast<float>(rect.top)),
                Gdiplus::PointF(
                    static_cast<float>(rect.right),
                    static_cast<float>(rect.bottom)),
                Gdiplus::Color(
                    borderAlpha,
                    GetRValue(border1),
                    GetGValue(border1),
                    GetBValue(border1)),
                Gdiplus::Color(
                    borderAlpha,
                    GetRValue(border2),
                    GetGValue(border2),
                    GetBValue(border2)));

            Gdiplus::Pen pen(
                &borderBrush,
                static_cast<Gdiplus::REAL>(g_settings.backgroundBorderThickness));
            graphics.DrawPath(&pen, &path);
        } else {
            const DWORD borderColor =
                (g_settings.backgroundBorderMode == 0)
                    ? albumPrimary
                    : g_settings.backgroundBorderColor1;

            Gdiplus::Pen pen(
                Gdiplus::Color(
                    borderAlpha,
                    GetRValue(borderColor),
                    GetGValue(borderColor),
                    GetBValue(borderColor)),
                static_cast<Gdiplus::REAL>(
                    g_settings.backgroundBorderThickness));
            graphics.DrawPath(&pen, &path);
        }
    }
}


static void DestroyRenderTarget() {
    DestroyBackgroundBlurBitmap();

    if (g_renderMemDC) {
        if (g_renderOldBitmap) {
            SelectObject(g_renderMemDC, g_renderOldBitmap);
            g_renderOldBitmap = nullptr;
        }
        if (g_renderBitmap) {
            DeleteObject(g_renderBitmap);
            g_renderBitmap = nullptr;
        }
        DeleteDC(g_renderMemDC);
        g_renderMemDC = nullptr;
    }
    g_renderBits = nullptr;
    g_renderWidth = 0;
    g_renderHeight = 0;
}

static bool g_mirrorRenderPass = false;
static int g_mirrorRenderAxis = 0; // 1 = vertical, 2 = horizontal
static bool g_mirrorCircularPass = false;

static bool EnsureRenderTarget(int w, int h) {
    if (w <= 0 || h <= 0) {
        return false;
    }

    if (g_renderMemDC && g_renderBitmap && g_renderBits &&
        g_renderWidth >= w && g_renderHeight >= h) {
        return true;
    }

    DestroyRenderTarget();

    g_renderMemDC = CreateCompatibleDC(nullptr);
    if (!g_renderMemDC) {
        return false;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    g_renderBitmap = CreateDIBSection(
        nullptr, &bi, DIB_RGB_COLORS, &g_renderBits, nullptr, 0);
    if (!g_renderBitmap || !g_renderBits) {
        DestroyRenderTarget();
        return false;
    }

    g_renderOldBitmap = SelectObject(g_renderMemDC, g_renderBitmap);
    if (!g_renderOldBitmap || g_renderOldBitmap == HGDI_ERROR) {
        DestroyRenderTarget();
        return false;
    }

    g_renderWidth = w;
    g_renderHeight = h;
    return true;
}

static int LimitBarLengthByLyricsWidget(
    int barCrossPos,
    int barThickness,
    int barLen,
    bool mirroredPass,
    int renderWidth,
    int renderHeight) {
    constexpr int kLyricsBarGapPx = 5;

    if (!g_settings.lyricsLimitBars || !g_settings.lyricsEnabled || barLen <= 0)
        return barLen;

    const bool lyricsAvailable =
        g_lyricsAvailableFast.load(std::memory_order_acquire);

    if (!lyricsAvailable && g_settings.lyricsUnavailableBehavior == 1)
        return barLen;

    int lyricsX = g_settings.lyricsX;
    int lyricsY = g_settings.lyricsY;
    const int lyricsW = g_settings.lyricsWidth;
    const int configuredLyricsH = g_settings.lyricsHeight;
    const int lyricsH = (!lyricsAvailable &&
                         g_settings.lyricsUnavailableBehavior == 2)
        ? std::min(configuredLyricsH, std::max(40, g_settings.lyricsFontSize + 18))
        : configuredLyricsH;

    if (lyricsW <= 0 || lyricsH <= 0 || barThickness <= 0)
        return barLen;

    // The mirrored visualizer is rendered with a Graphics transform after the
    // bar geometry is calculated. Keep the bar geometry/orientation unchanged
    // and transform only the lyrics rectangle into that pass's local space.
    // This makes Limit visualizer bars with lyrics widget independent for the
    // primary and mirrored passes without applying the mirror transform twice.
    if (mirroredPass) {
        if (g_settings.orientation <= 2) {
            lyricsY = renderHeight - (lyricsY + lyricsH);
        } else {
            lyricsX = renderWidth - (lyricsX + lyricsW);
        }
    }

    // Vertical directions (0 = bottom-up, 1 = center vertical, 2 = top-down).
    if (g_settings.orientation <= 2) {
        // Only bars whose horizontal span intersects the widget are limited.
        const bool overlapsX =
            barCrossPos < lyricsX + lyricsW &&
            barCrossPos + barThickness > lyricsX;
        if (!overlapsX)
            return barLen;

        const int widgetTop = lyricsY;
        const int widgetBottom = lyricsY + lyricsH;
        const int baseY = g_settings.positionY;

        if (g_settings.orientation == 0) {
            // Bottom-up: top of the bar must remain 5 px below widget bottom.
            const int maxLen = baseY - widgetBottom - kLyricsBarGapPx;
            return std::min(barLen, std::max(0, maxLen));
        }

        if (g_settings.orientation == 2) {
            // Top-down: bottom of the bar must remain 5 px above widget top.
            const int maxLen = widgetTop - baseY - kLyricsBarGapPx;
            return std::min(barLen, std::max(0, maxLen));
        }

        // Center vertical: the bar grows equally in both directions.
        // Reduce the total length so the nearest half stays 5 px away.
        int maxHalf = 0;
        if (baseY < widgetTop) {
            maxHalf = widgetTop - baseY - kLyricsBarGapPx;
        } else if (baseY > widgetBottom) {
            maxHalf = baseY - widgetBottom - kLyricsBarGapPx;
        }

        return std::min(barLen, std::max(0, maxHalf) * 2);
    }

    // Horizontal directions (3 = left-right, 4 = center horizontal, 5 = right-left).
    if (g_settings.orientation >= 3 && g_settings.orientation <= 5) {
        // Only bars whose vertical span intersects the widget are limited.
        const bool overlapsY =
            barCrossPos < lyricsY + lyricsH &&
            barCrossPos + barThickness > lyricsY;
        if (!overlapsY)
            return barLen;

        const int widgetLeft = lyricsX;
        const int widgetRight = lyricsX + lyricsW;
        const int baseX = g_settings.positionX;

        if (g_settings.orientation == 3) {
            // Left-right: right edge must remain 5 px before widget left edge.
            const int maxLen = widgetLeft - baseX - kLyricsBarGapPx;
            return std::min(barLen, std::max(0, maxLen));
        }

        if (g_settings.orientation == 5) {
            // Right-left: left edge must remain 5 px after widget right edge.
            const int maxLen = baseX - widgetRight - kLyricsBarGapPx;
            return std::min(barLen, std::max(0, maxLen));
        }

        // Center horizontal: the bar grows equally in both directions.
        int maxHalf = 0;
        if (baseX < widgetLeft) {
            maxHalf = widgetLeft - baseX - kLyricsBarGapPx;
        } else if (baseX > widgetRight) {
            maxHalf = baseX - widgetRight - kLyricsBarGapPx;
        }

        return std::min(barLen, std::max(0, maxHalf) * 2);
    }

    return barLen;
}

static bool RenderSpecialVisualization(
    Gdiplus::Graphics& graphics,
    int w,
    int h,
    int barCount,
    DWORD primaryColor,
    DWORD secondaryColor) {
    if (g_settings.barShape != 5 && g_settings.barShape != 6)
        return false;

    const int radius = std::max(1, g_settings.barWidth / 2);
    Gdiplus::GraphicsState state = graphics.Save();

    if (g_settings.barShape == 5) {
        for (int i = 0; i < barCount; ++i) {
            const int barLen = std::max(1, static_cast<int>(g_currentHeights[i]));
            const float t = barCount > 1
                ? static_cast<float>(i) / static_cast<float>(barCount - 1)
                : 0.0f;
            DWORD color = (g_settings.colorMode == 1 || g_settings.colorMode == 6 ||
                           g_settings.colorMode == 8)
                ? LerpColor(primaryColor, secondaryColor, t)
                : primaryColor;
            const BYTE alpha = static_cast<BYTE>(std::clamp(
                g_settings.acrylicOpacity * 2.55f, 0.0f, 255.0f));
            Gdiplus::SolidBrush brush(Gdiplus::Color(
                alpha, GetRValue(color), GetGValue(color), GetBValue(color)));

            const int offset = i * (g_settings.barWidth + g_settings.barSpacing);
            float cx = 0.0f;
            float cy = 0.0f;
            if (g_settings.orientation <= 2) {
                cx = static_cast<float>(g_settings.positionX + offset + radius);
                if (g_settings.orientation == 0)
                    cy = static_cast<float>(g_settings.positionY - barLen);
                else if (g_settings.orientation == 1)
                    cy = static_cast<float>(g_settings.positionY + (barLen / 2));
                else
                    cy = static_cast<float>(g_settings.positionY + barLen);
            } else {
                cy = static_cast<float>(g_settings.positionY + offset + radius);
                if (g_settings.orientation == 3)
                    cx = static_cast<float>(g_settings.positionX + barLen);
                else if (g_settings.orientation == 4)
                    cx = static_cast<float>(g_settings.positionX + (barLen / 2));
                else
                    cx = static_cast<float>(g_settings.positionX - barLen);
            }

            graphics.FillEllipse(&brush,
                cx - static_cast<float>(radius),
                cy - static_cast<float>(radius),
                static_cast<float>(radius * 2),
                static_cast<float>(radius * 2));
        }
        graphics.Restore(state);
        return true;
    }

    // Area visualization: one continuous filled spectrum surface.
    Gdiplus::GraphicsPath path;
    std::vector<Gdiplus::PointF> top;
    top.reserve(static_cast<size_t>(barCount));

    for (int i = 0; i < barCount; ++i) {
        const float t = barCount > 1
            ? static_cast<float>(i) / static_cast<float>(barCount - 1)
            : 0.0f;
        const float barLen = static_cast<float>(std::max(1, static_cast<int>(g_currentHeights[i])));
        const float cross = static_cast<float>(
            g_settings.barWidth / 2 + i * (g_settings.barWidth + g_settings.barSpacing));

        if (g_settings.orientation <= 2) {
            const float x = static_cast<float>(g_settings.positionX) + cross;
            float y = static_cast<float>(g_settings.positionY);
            if (g_settings.orientation == 0)
                y -= barLen;
            else if (g_settings.orientation == 1)
                y -= barLen * 0.5f;
            top.emplace_back(x, y);
        } else {
            const float y = static_cast<float>(g_settings.positionY) + cross;
            float x = static_cast<float>(g_settings.positionX);
            if (g_settings.orientation == 3)
                x += barLen;
            else if (g_settings.orientation == 4)
                x += barLen * 0.5f;
            else
                x -= barLen;
            top.emplace_back(x, y);
        }
    }

    if (top.size() >= 2) {
        path.StartFigure();
        for (size_t i = 0; i < top.size(); ++i) {
            if (i == 0)
                path.AddLine(top[i], top[i]);
            else
                path.AddLine(top[i - 1], top[i]);
        }

        if (g_settings.orientation <= 2) {
            path.AddLine(top.back().X, static_cast<float>(g_settings.positionY),
                         top.front().X, static_cast<float>(g_settings.positionY));
        } else {
            path.AddLine(static_cast<float>(g_settings.positionX), top.back().Y,
                         static_cast<float>(g_settings.positionX), top.front().Y);
        }
        path.CloseFigure();

        Gdiplus::RectF bounds;
        path.GetBounds(&bounds);
        const float t1 = (g_settings.colorMode == 1 || g_settings.colorMode == 6 ||
                          g_settings.colorMode == 8) ? 1.0f : 0.0f;
        DWORD c1 = primaryColor;
        DWORD c2 = LerpColor(primaryColor, secondaryColor, t1);
        const BYTE alpha = static_cast<BYTE>(std::clamp(
            g_settings.acrylicOpacity * 2.55f, 0.0f, 255.0f));
        Gdiplus::LinearGradientBrush brush(
            Gdiplus::PointF(bounds.X, bounds.Y),
            Gdiplus::PointF(bounds.GetRight(), bounds.GetBottom()),
            Gdiplus::Color(alpha, GetRValue(c1), GetGValue(c1), GetBValue(c1)),
            Gdiplus::Color(alpha, GetRValue(c2), GetGValue(c2), GetBValue(c2)));
        graphics.FillPath(&brush, &path);
    }

    graphics.Restore(state);
    return true;
}

static void RenderVisualizerPass(
    Gdiplus::Graphics& graphics,
    int w,
    int h) {
    // Snapshot frame-static settings once. This both avoids repeated global reads
    // in the 180 Hz hot path and keeps one frame internally self-consistent.
    const int colorMode = g_settings.colorMode;
    const int barStyle = g_settings.barStyle;
    const int orientation = g_settings.orientation;
    const int positionX = g_settings.positionX;
    const int positionY = g_settings.positionY;
    const int maxBarHeight = std::max(1, g_settings.maxBarHeight);
    const bool isCircle = (g_settings.barShape == 4);
    const bool dynamicAcrylic = (colorMode == 3);
    const bool needsHeightRatio = dynamicAcrylic || (barStyle == 5);
    const bool needsGradientT = (colorMode == 1);
    const bool lyricsLimitBars =
        g_settings.lyricsEnabled && g_settings.lyricsLimitBars;

    const DWORD primaryColor = (colorMode == 7 || colorMode == 8)
        ? GetAlbumPalettePrimary()
        : g_settings.color1;
    const DWORD secondaryColor = (colorMode == 8)
        ? GetAlbumPaletteSecondary()
        : g_settings.color2;

    const int barCount = std::clamp(g_settings.barCount, 1, VIZ_BANDS_MAX);
    const int baseAlphaValue = std::clamp(
        255 * g_settings.acrylicOpacity / 100, 10, 255);


    if (g_mirrorRenderPass && g_settings.barShape == 4) {
        Gdiplus::GraphicsState bgState = graphics.Save();
        if (g_mirrorRenderAxis == 1) {
            graphics.TranslateTransform(0.0f, static_cast<float>(h));
            graphics.ScaleTransform(1.0f, -1.0f);
        } else if (g_mirrorRenderAxis == 2) {
            graphics.TranslateTransform(static_cast<float>(w), 0.0f);
            graphics.ScaleTransform(-1.0f, 1.0f);
        }
        RenderVisualizerBackground(graphics, barCount);
        graphics.Restore(bgState);
    } else {
        RenderVisualizerBackground(graphics, barCount);
    }

    if (RenderSpecialVisualization(graphics, w, h, barCount, primaryColor, secondaryColor))
        return;

    if (g_settings.barStyle == 3) {

        RenderCurveVisualizer(graphics, barCount);
    } else {
        const int requestedRadius = std::max(0, g_settings.cornerRadius);


    POINT ptCursor{};
    if (g_settings.dynamicWidthEnabled) {
        GetCursorPos(&ptCursor);
        if (g_hwndOverlay)
            ScreenToClient(g_hwndOverlay, &ptCursor);
    }
    if (g_mirrorRenderPass) {
        if (g_mirrorRenderAxis == 1)
            ptCursor.y = h - ptCursor.y;
        else if (g_mirrorRenderAxis == 2)
            ptCursor.x = w - ptCursor.x;
    }


    std::array<int, VIZ_BANDS_MAX> barWidths{};
    std::fill_n(barWidths.begin(), static_cast<size_t>(barCount), g_settings.barWidth);
    int totalWidthBonus = 0; 

    if (g_settings.dynamicWidthEnabled) {
        const float rx = static_cast<float>(std::max(1, g_settings.dynamicWidthRadiusX));
        const float ry = static_cast<float>(std::max(1, g_settings.dynamicWidthRadiusY));

        for (int i = 0; i < barCount; ++i) {

            int defaultPos = i * (g_settings.barWidth + g_settings.barSpacing);
            int barCenterX = 0, barCenterY = 0;

            if (g_settings.barShape == 4) { 
                const float angle =
                    (static_cast<float>(g_settings.circleStartAngle) +
                     (360.0f * static_cast<float>(i) /
                      static_cast<float>(std::max(1, barCount)))) *
                    (VIZ_PI / 180.0f);
                const int circleCenterX = g_mirrorCircularPass && g_mirrorRenderAxis == 2
                    ? (w - g_settings.positionX)
                    : g_settings.positionX;
                const int circleCenterY = g_mirrorCircularPass && g_mirrorRenderAxis == 1
                    ? (h - g_settings.positionY)
                    : g_settings.positionY;
                barCenterX = circleCenterX +
                    static_cast<int>(cosf(angle) * g_settings.circleRadius);
                barCenterY = circleCenterY +
                    static_cast<int>(sinf(angle) * g_settings.circleRadius);
            } else if (g_settings.orientation <= 2) { 
                barCenterX = g_settings.positionX + defaultPos + g_settings.barWidth / 2;
                barCenterY = g_settings.positionY;
            } else { 
                barCenterX = g_settings.positionX;
                barCenterY = g_settings.positionY + defaultPos + g_settings.barWidth / 2;
            }

        
            float dx = static_cast<float>(ptCursor.x - barCenterX) / rx;
            float dy = static_cast<float>(ptCursor.y - barCenterY) / ry;
            const float distSq = dx * dx + dy * dy;

            if (distSq < 1.0f) {
                const float normDist = sqrtf(distSq);
                float factor = 1.0f - normDist;
            
                factor = 0.5f * (1.0f - cosf(factor * 3.14159265f));

                int bonus = static_cast<int>(factor * g_settings.dynamicWidthMaxBonus);
                barWidths[i] += bonus;
                totalWidthBonus += bonus;
            }
        }
    }


    int startShift = 0;

    if (g_settings.dynamicWidthEnabled && totalWidthBonus > 0) {
    
        int baseTotalSize = barCount * g_settings.barWidth + (barCount - 1) * g_settings.barSpacing;

    
        int mousePosRel = (g_settings.orientation <= 2) 
            ? (ptCursor.x - g_settings.positionX) 
            : (ptCursor.y - g_settings.positionY);


        float mouseRatio = static_cast<float>(mousePosRel) / static_cast<float>(std::max(1, baseTotalSize));
        mouseRatio = std::clamp(mouseRatio, 0.0f, 1.0f);


        startShift = static_cast<int>(totalWidthBonus * mouseRatio);
    }


    int currentOffset = -startShift;

    for (int i = 0; i < barCount; ++i) {
        int barLen = std::max(1, static_cast<int>(g_currentHeights[i]));
        const int currentBarWidth = barWidths[i];

        if (g_settings.barStyle == 2) {
            const int effectiveSegmentHeight =
                (g_settings.segmentHeight > 0)
                    ? g_settings.segmentHeight
                    : currentBarWidth;
            barLen = std::max(barLen, effectiveSegmentHeight);
        }

        if (!isCircle && lyricsLimitBars) {
            const int barCrossPos = (orientation <= 2)
                ? positionX + currentOffset
                : positionY + currentOffset;
            barLen = LimitBarLengthByLyricsWidget(
                barCrossPos, currentBarWidth, barLen,
                g_mirrorRenderPass, w, h);
            if (barLen <= 0)
                continue;
        }

        RECT barRect{};
        float circleSin = 0.0f;
        float circleCos = 0.0f;

        if (g_settings.barShape == 4) {

            const float angle =
                (static_cast<float>(g_settings.circleStartAngle) +
                 (360.0f * static_cast<float>(i) /
                  static_cast<float>(std::max(1, barCount)))) *
                (VIZ_PI / 180.0f);
            circleCos = cosf(angle);
            circleSin = sinf(angle);
            const float cx = (g_mirrorCircularPass && g_mirrorRenderAxis == 2)
                ? static_cast<float>(w - g_settings.positionX)
                : static_cast<float>(g_settings.positionX);
            const float cy = (g_mirrorCircularPass && g_mirrorRenderAxis == 1)
                ? static_cast<float>(h - g_settings.positionY)
                : static_cast<float>(g_settings.positionY);

            const float px = cx + circleCos *
                static_cast<float>(g_settings.circleRadius);
            const float py = cy + circleSin *
                static_cast<float>(g_settings.circleRadius);
            const int halfW = std::max(1, currentBarWidth / 2);
            barRect = {
                static_cast<int>(px) - halfW,
                static_cast<int>(py) - halfW,
                static_cast<int>(px) + halfW,
                static_cast<int>(py) + halfW
            };
        } else if (g_settings.orientation <= 2) {
            const int x = g_settings.positionX + currentOffset;
            int yTop = 0, yBottom = 0;
            if (g_settings.orientation == 0) {
                yTop = g_settings.positionY - barLen;
                yBottom = g_settings.positionY;
            } else if (g_settings.orientation == 1) {
                yTop = g_settings.positionY - barLen / 2;
                yBottom = g_settings.positionY + barLen / 2;
            } else {
                yTop = g_settings.positionY;
                yBottom = g_settings.positionY + barLen;
            }
            barRect = {x, yTop, x + currentBarWidth, yBottom};
        } else {
            const int y = g_settings.positionY + currentOffset;
            int xLeft = 0, xRight = 0;
            if (g_settings.orientation == 3) {
                xLeft = g_settings.positionX;
                xRight = g_settings.positionX + barLen;
            } else if (g_settings.orientation == 4) {
                xLeft = g_settings.positionX - barLen / 2;
                xRight = g_settings.positionX + barLen / 2;
            } else {
                xLeft = g_settings.positionX - barLen;
                xRight = g_settings.positionX;
            }
            barRect = {xLeft, y, xRight, y + currentBarWidth};
        }


        currentOffset += currentBarWidth + g_settings.barSpacing;

        if (barRect.right <= 0 || barRect.left >= w ||
            barRect.bottom <= 0 || barRect.top >= h)
            continue;

        float heightRatio = 0.0f;
        float colorT = 0.0f;
        float opacityT = 0.0f;
        if (needsHeightRatio) {
            heightRatio = std::clamp(
                static_cast<float>(barLen) / static_cast<float>(maxBarHeight),
                0.0f, 1.0f);
            if (dynamicAcrylic) {
                colorT = ApplyHeightCurve(
                    heightRatio,
                    g_settings.gradientCurveEnabled,
                    g_settings.gradientCurve);
                opacityT = ApplyHeightCurve(
                    heightRatio,
                    g_settings.opacityCurveEnabled,
                    g_settings.opacityCurve);
            }
        }

        const float gradientT = (needsGradientT && barCount > 1)
            ? static_cast<float>(i) / static_cast<float>(barCount - 1)
            : 0.0f;

        DWORD color = primaryColor;
        if (isCircle) {

            const float angle =
                (static_cast<float>(g_settings.circleStartAngle) +
                 (360.0f * static_cast<float>(i) /
                  static_cast<float>(std::max(1, barCount)))) *
                (VIZ_PI / 180.0f);
            const float circleX = 0.5f + 0.5f * circleCos;
            const float circleY = 0.5f + 0.5f * circleSin;

            if (colorMode == 1) {
                color = LerpColor(primaryColor, secondaryColor, circleX);
            } else if (colorMode == 6 || colorMode == 8) {
                color = LerpColor(primaryColor, secondaryColor, circleY);
            } else if (dynamicAcrylic) {
                color = LerpColor(primaryColor, secondaryColor, colorT);
            }
        } else if (colorMode == 1) {
            color = LerpColor(primaryColor, secondaryColor, gradientT);
        } else if (dynamicAcrylic) {
            color = LerpColor(primaryColor, secondaryColor, colorT);
        }

        int alphaValue = baseAlphaValue;

        if (dynamicAcrylic) {
            const int minAlpha =
                (g_settings.dynamicAcrylicMinOpacity * 255) / 100;
            const int maxAlpha = 255;
        
            const int baseA =
                minAlpha + static_cast<int>((maxAlpha - minAlpha) * opacityT);
            
            alphaValue = std::clamp(
                baseA * g_settings.acrylicOpacity / 100, 10, 255);
        }


        const BYTE alpha = static_cast<BYTE>(alphaValue);
        const int radius =
            (g_settings.barStyle == 1 ||
             g_settings.barStyle == 2 ||
             g_settings.barStyle == 5)
                ? requestedRadius
                : 0;

        if (g_settings.barShape == 4) {
            g_pointedDirection = POINTED_TOP_DOWN;

            const float angleDeg =
                static_cast<float>(g_settings.circleStartAngle) +
                (360.0f * static_cast<float>(i) /
                 static_cast<float>(std::max(1, barCount)));

            const Gdiplus::GraphicsState state = graphics.Save();
            if (g_mirrorCircularPass) {
                const float centerX = (g_mirrorRenderAxis == 2)
                    ? static_cast<float>(w - g_settings.positionX)
                    : static_cast<float>(g_settings.positionX);
                const float centerY = (g_mirrorRenderAxis == 1)
                    ? static_cast<float>(h - g_settings.positionY)
                    : static_cast<float>(g_settings.positionY);
                graphics.TranslateTransform(centerX, centerY);
                graphics.RotateTransform(angleDeg);
            } else {
                graphics.TranslateTransform(
                    static_cast<float>(g_settings.positionX),
                    static_cast<float>(g_settings.positionY));
                graphics.RotateTransform(angleDeg);
            }

            const int halfWidth = std::max(1, currentBarWidth / 2);
            RECT radialRect{
                -halfWidth,
                g_settings.circleRadius,
                halfWidth,
                g_settings.circleRadius + barLen
            };

            Gdiplus::GraphicsState segmentedState = 0;
            const bool segmentedClipApplied =
                (g_settings.barStyle == 2) &&
                ApplySegmentedSquareClip(
                    graphics,
                    radialRect,
                    (g_settings.segmentHeight > 0)
                        ? g_settings.segmentHeight
                        : halfWidth * 2,
                    g_settings.segmentSpacing,
                    radius,
                    false,
                    false,
                    false,
                    &segmentedState);

            const int radialRadius =
                std::min(radius, std::max(0, halfWidth));


            if (g_settings.colorMode == 1 || g_settings.colorMode == 6 || g_settings.colorMode == 8) {
                Gdiplus::SolidBrush gradientBar(
                    Gdiplus::Color(
                        alpha,
                        GetRValue(color),
                        GetGValue(color),
                        GetBValue(color)));
                DrawBarBrush(graphics, radialRect, radialRadius, gradientBar);
            }

            else if (g_settings.colorMode == 4) {
                const BYTE bodyAlpha = static_cast<BYTE>(
                    std::clamp(alphaValue * 0.72f, 8.0f, 255.0f));

                Gdiplus::Color c1(
                    bodyAlpha,
                    GetRValue(color),
                    GetGValue(color),
                    GetBValue(color));
                Gdiplus::Color c2(
                    static_cast<BYTE>(std::clamp(
                        alphaValue * 0.38f, 5.0f, 255.0f)),
                    static_cast<BYTE>(std::min(255, GetRValue(color) + 45)),
                    static_cast<BYTE>(std::min(255, GetGValue(color) + 45)),
                    static_cast<BYTE>(std::min(255, GetBValue(color) + 45)));

                Gdiplus::LinearGradientBrush glassBrush(
                    Gdiplus::Point(radialRect.left, radialRect.top),
                    Gdiplus::Point(radialRect.left, radialRect.bottom),
                    c1, c2);
                DrawBarBrush(graphics, radialRect, radialRadius, glassBrush);

                const BYTE highlightAlpha = static_cast<BYTE>(
                    std::clamp(g_settings.glassHighlight * 2.0f, 1.0f, 255.0f));
                Gdiplus::Pen highlightPen(
                    Gdiplus::Color(highlightAlpha, 255, 255, 255), 1.0f);
                DrawGlassBorder(
                    graphics, ExpandRect(radialRect, 1),
                    std::min(radialRadius + 1, 25), highlightPen);

                const BYTE innerAlpha = static_cast<BYTE>(
                    std::clamp(g_settings.glassHighlight * 0.9f, 1.0f, 255.0f));
                Gdiplus::Pen innerPen(
                    Gdiplus::Color(innerAlpha, 255, 255, 255), 0.7f);
                DrawGlassBorder(graphics, radialRect, radialRadius, innerPen);
            }
            // Windows 7 / Aero glass.
            else if (g_settings.colorMode == 5) {
                const DWORD lighter = MixColor(color, RGB(255, 255, 255), 0.58f);
                const DWORD darker = MixColor(color, RGB(0, 0, 0), 0.18f);

                Gdiplus::Color topColor(
                    static_cast<BYTE>(std::clamp(alphaValue * 0.76f, 8.0f, 255.0f)),
                    GetRValue(lighter), GetGValue(lighter), GetBValue(lighter));
                Gdiplus::Color bottomColor(
                    static_cast<BYTE>(std::clamp(alphaValue * 0.52f, 8.0f, 255.0f)),
                    GetRValue(darker), GetGValue(darker), GetBValue(darker));

                Gdiplus::LinearGradientBrush aeroBrush(
                    Gdiplus::Point(radialRect.left, radialRect.top),
                    Gdiplus::Point(radialRect.left, radialRect.bottom),
                    topColor, bottomColor);
                DrawBarBrush(graphics, radialRect, radialRadius, aeroBrush);

                const BYTE highlightAlpha = static_cast<BYTE>(
                    std::clamp(g_settings.glassHighlight * 1.7f, 1.0f, 255.0f));
                Gdiplus::Pen aeroPen(
                    Gdiplus::Color(highlightAlpha, 245, 250, 255), 1.0f);
                DrawGlassBorder(graphics, radialRect, radialRadius, aeroPen);

                if (radialRect.bottom - radialRect.top > 4) {
                    RECT shineRect = radialRect;
                    const int shineHeight =
                        ((radialRect.bottom - radialRect.top) / 5 > 0)
                            ? ((radialRect.bottom - radialRect.top) / 5)
                            : 1;
                    shineRect.bottom = shineRect.top + shineHeight;

                    Gdiplus::Color shineColor(
                        static_cast<BYTE>(
                            std::clamp(g_settings.glassHighlight * 1.2f, 1.0f, 190.0f)),
                        255, 255, 255);
                    Gdiplus::SolidBrush shineBrush(shineColor);
                    DrawBarBrush(
                        graphics, shineRect,
                        std::min(radialRadius, 8), shineBrush);
                }
            }

            else {
                Gdiplus::SolidBrush radialBrush(
                    Gdiplus::Color(
                        alpha,
                        GetRValue(color),
                        GetGValue(color),
                        GetBValue(color)));
                DrawBarBrush(graphics, radialRect, radialRadius, radialBrush);
            }

            if (g_settings.borderEnabled) {
                DrawVisualizerBarBorder(
                    graphics, radialRect, radialRadius, currentBarWidth);
            }

            if (segmentedClipApplied)
                graphics.Restore(segmentedState);

            graphics.Restore(state);

            continue;
        }
        else {
            switch (g_settings.orientation) {
                case 2: g_pointedDirection = POINTED_TOP_DOWN; break;
                case 3: g_pointedDirection = POINTED_LEFT_RIGHT; break;
                case 4: g_pointedDirection = POINTED_CENTER_HORIZONTAL; break;
                case 5: g_pointedDirection = POINTED_RIGHT_LEFT; break;
                case 1: g_pointedDirection = POINTED_CENTER_VERTICAL; break;
                case 0:
                default: g_pointedDirection = POINTED_BOTTOM_UP; break;
            }
        }


        Gdiplus::GraphicsState segmentedState = 0;
        const bool segmentedClipApplied =
            (g_settings.barStyle == 2) &&
            ApplySegmentedSquareClip(
                graphics,
                barRect,
                (g_settings.segmentHeight > 0)
                    ? g_settings.segmentHeight
                    : currentBarWidth,
                g_settings.segmentSpacing,
                radius,
                g_settings.orientation > 2,
                g_settings.orientation == 1 || g_settings.orientation == 4,
                g_settings.orientation == 0 || g_settings.orientation == 5,
                &segmentedState);


        if (g_settings.colorMode == 4) {
            const BYTE bodyAlpha = static_cast<BYTE>(
                std::clamp(alphaValue * 0.72f, 8.0f, 255.0f));

            Gdiplus::Color c1(
                bodyAlpha,
                GetRValue(color),
                GetGValue(color),
                GetBValue(color));
            Gdiplus::Color c2(
                static_cast<BYTE>(std::clamp(
                    alphaValue * 0.38f, 5.0f, 255.0f)),
                static_cast<BYTE>(std::min(255, GetRValue(color) + 45)),
                static_cast<BYTE>(std::min(255, GetGValue(color) + 45)),
                static_cast<BYTE>(std::min(255, GetBValue(color) + 45)));

            Gdiplus::Point p1(
                barRect.left, barRect.top);
            Gdiplus::Point p2(
                barRect.right, barRect.bottom);
            Gdiplus::LinearGradientBrush glassBrush(
                p1, p2, c1, c2);
            DrawBarBrush(graphics, barRect, radius, glassBrush);

            const BYTE highlightAlpha = static_cast<BYTE>(
                std::clamp(g_settings.glassHighlight * 2.0f, 1.0f, 255.0f));
            Gdiplus::Pen highlightPen(
                Gdiplus::Color(highlightAlpha, 255, 255, 255),
                1.0f);
            DrawGlassBorder(graphics, ExpandRect(barRect, 1),
                            std::min(radius + 1, 25), highlightPen);

            const BYTE innerAlpha = static_cast<BYTE>(
                std::clamp(g_settings.glassHighlight * 0.9f, 1.0f, 255.0f));
            Gdiplus::Pen innerPen(
                Gdiplus::Color(innerAlpha, 255, 255, 255),
                0.7f);
            DrawGlassBorder(graphics, barRect, radius, innerPen);
        }


        else if (g_settings.colorMode == 5) {
            const DWORD lighter = MixColor(color, RGB(255, 255, 255), 0.58f);
            const DWORD darker = MixColor(color, RGB(0, 0, 0), 0.18f);

            Gdiplus::Color topColor(
                static_cast<BYTE>(std::clamp(alphaValue * 0.76f, 8.0f, 255.0f)),
                GetRValue(lighter), GetGValue(lighter), GetBValue(lighter));
            Gdiplus::Color bottomColor(
                static_cast<BYTE>(std::clamp(alphaValue * 0.52f, 8.0f, 255.0f)),
                GetRValue(darker), GetGValue(darker), GetBValue(darker));

            Gdiplus::LinearGradientBrush aeroBrush(
                Gdiplus::Point(barRect.left, barRect.top),
                Gdiplus::Point(barRect.left, barRect.bottom),
                topColor, bottomColor);
            DrawBarBrush(graphics, barRect, radius, aeroBrush);

            const BYTE highlightAlpha = static_cast<BYTE>(
                std::clamp(g_settings.glassHighlight * 1.7f, 1.0f, 255.0f));
            Gdiplus::Pen aeroPen(
                Gdiplus::Color(highlightAlpha, 245, 250, 255),
                1.0f);
            DrawGlassBorder(graphics, barRect, radius, aeroPen);

            if (barRect.bottom - barRect.top > 4) {
                RECT shineRect = barRect;
                const int shineHeight =
                    ((barRect.bottom - barRect.top) / 5 > 0)
                        ? ((barRect.bottom - barRect.top) / 5)
                        : 1;
                shineRect.bottom = shineRect.top + shineHeight;

                Gdiplus::Color shineColor(
                    static_cast<BYTE>(
                        std::clamp(g_settings.glassHighlight * 1.2f, 1.0f, 190.0f)),
                    255, 255, 255);
                Gdiplus::SolidBrush shineBrush(shineColor);
                DrawBarBrush(graphics, shineRect,
                             std::min(radius, 8), shineBrush);
            }
        }



        else {
            Gdiplus::Color gdiColor(
                alpha,
                GetRValue(color),
                GetGValue(color),
                GetBValue(color));
            Gdiplus::SolidBrush brush(gdiColor);

            if (g_settings.colorMode == 1) {

                Gdiplus::Color c1(
                    alpha,
                    GetRValue(color),
                    GetGValue(color),
                    GetBValue(color));
                Gdiplus::Color c2(
                    alpha,
                    GetRValue(secondaryColor),
                    GetGValue(secondaryColor),
                    GetBValue(secondaryColor));

                Gdiplus::LinearGradientBrush gradientBrush(
                    Gdiplus::Point(barRect.left, barRect.top),
                    Gdiplus::Point(barRect.right, barRect.top),
                    c1, c2);
                DrawBarBrush(graphics, barRect, radius, gradientBrush);
            } else if (g_settings.colorMode == 6 || g_settings.colorMode == 8) {

                Gdiplus::Color c1(
                    alpha,
                    GetRValue(color),
                    GetGValue(color),
                    GetBValue(color));
                Gdiplus::Color c2(
                    alpha,
                    GetRValue(secondaryColor),
                    GetGValue(secondaryColor),
                    GetBValue(secondaryColor));

                Gdiplus::LinearGradientBrush gradientBrush(
                    Gdiplus::Point(barRect.left, barRect.top),
                    Gdiplus::Point(barRect.left, barRect.bottom),
                    c1, c2);
                DrawBarBrush(graphics, barRect, radius, gradientBrush);
            } else {
                DrawBarBrush(graphics, barRect, radius, brush);
            }
        }


        if (g_settings.barStyle == 5) {
            DrawBatteryLiquidGap(graphics, barRect, i, heightRatio, color);
        }

        if (g_settings.borderEnabled) {
            DrawVisualizerBarBorder(
                graphics, barRect, radius,
                (g_settings.barStyle == 2)
                    ? ((g_settings.segmentHeight > 0)
                        ? g_settings.segmentHeight
                        : currentBarWidth)
                    : 0);
        }

        if (segmentedClipApplied)
            graphics.Restore(segmentedState);
    }
    } 

    }

static RECT GetThirdAlbumWidgetAnchorRect(const VisualizerSettings& settings) {
    if (settings.albumWidgetAttachment == 1) {
        int lyricsH = settings.lyricsHeight;
        const bool lyricsAvailable =
            g_lyricsAvailableFast.load(std::memory_order_acquire);
        if (!lyricsAvailable && settings.lyricsUnavailableBehavior == 2)
            lyricsH = std::min(lyricsH, std::max(40, settings.lyricsFontSize + 18));
        return RECT{
            settings.lyricsX,
            settings.lyricsY,
            settings.lyricsX + settings.lyricsWidth,
            settings.lyricsY + lyricsH};
    }

    const int barCount = std::clamp(settings.barCount, 1, VIZ_BANDS_MAX);
    return GetVisualizerBackgroundRect(
        barCount, 1.0f, settings.backgroundPadding,
        settings.backgroundHeightAdjustment);
}

static RECT ClampAlbumWidgetAttachedRect(
    const RECT& anchor, const RECT& desired, int gap, int maxDistance) {
    const int widgetW = std::max(1L, desired.right - desired.left);
    const int widgetH = std::max(1L, desired.bottom - desired.top);
    gap = std::max(0, gap);
    maxDistance = std::max(gap, maxDistance);

    struct Candidate { RECT rect{}; long long score = 0; };
    auto makeCandidate = [&](int minLeft, int maxLeft,
                             int minTop, int maxTop) -> RECT {
        if (minLeft > maxLeft) {
            const int mid = (minLeft + maxLeft) / 2;
            minLeft = maxLeft = mid;
        }
        if (minTop > maxTop) {
            const int mid = (minTop + maxTop) / 2;
            minTop = maxTop = mid;
        }
        RECT r{};
        r.left = std::clamp<LONG>(desired.left, minLeft, maxLeft);
        r.top = std::clamp<LONG>(desired.top, minTop, maxTop);
        r.right = r.left + widgetW;
        r.bottom = r.top + widgetH;
        return r;
    };
    auto score = [&](const RECT& r) -> long long {
        const long long dx = static_cast<long long>(r.left) - desired.left;
        const long long dy = static_cast<long long>(r.top) - desired.top;
        return dx * dx + dy * dy;
    };

    std::array<Candidate, 4> candidates{};
    candidates[0].rect = makeCandidate(
        anchor.right + gap, anchor.right + maxDistance,
        anchor.top - maxDistance, anchor.bottom + maxDistance - widgetH);
    candidates[1].rect = makeCandidate(
        anchor.left - maxDistance - widgetW, anchor.left - gap - widgetW,
        anchor.top - maxDistance, anchor.bottom + maxDistance - widgetH);
    candidates[2].rect = makeCandidate(
        anchor.left - maxDistance, anchor.right + maxDistance - widgetW,
        anchor.bottom + gap, anchor.bottom + maxDistance);
    candidates[3].rect = makeCandidate(
        anchor.left - maxDistance, anchor.right + maxDistance - widgetW,
        anchor.top - maxDistance - widgetH, anchor.top - gap - widgetH);

    for (auto& candidate : candidates)
        candidate.score = score(candidate.rect);

    Candidate best = candidates[0];
    for (size_t i = 1; i < candidates.size(); ++i) {
        if (candidates[i].score < best.score)
            best = candidates[i];
    }
    return best.rect;
}

static RECT GetThirdAlbumWidgetRect(const VisualizerSettings& settings) {
    const int w = settings.albumWidgetWidth;
    const int h = settings.albumWidgetHeight;

    if (settings.albumWidgetAttachment == 0) {
        const int x = g_albumWidgetSeparateX.load(std::memory_order_acquire);
        const int y = g_albumWidgetSeparateY.load(std::memory_order_acquire);
        return RECT{x, y, x + w, y + h};
    }

    const RECT anchor = GetThirdAlbumWidgetAnchorRect(settings);
    int ox = 0;
    int oy = 0;
    if (settings.albumWidgetAttachment == 1) {
        ox = g_albumWidgetLyricsOffsetX.load(std::memory_order_acquire);
        oy = g_albumWidgetLyricsOffsetY.load(std::memory_order_acquire);
    } else {
        ox = g_albumWidgetVisualizerOffsetX.load(std::memory_order_acquire);
        oy = g_albumWidgetVisualizerOffsetY.load(std::memory_order_acquire);
    }
    const RECT desired{
        anchor.left + ox, anchor.top + oy,
        anchor.left + ox + w, anchor.top + oy + h};
    return ClampAlbumWidgetAttachedRect(
        anchor, desired, settings.albumWidgetGap,
        settings.albumWidgetMaxDistance);
}

// Update the live widget position without touching Windhawk settings.
// Persisting settings while handling WM_MOUSEMOVE can synchronously trigger
// Wh_ModSettingsChanged(), which rebuilds the desktop overlay and makes the
// drag path unstable on some Explorer builds.
static void UpdateThirdAlbumWidgetRectInMemory(
    const VisualizerSettings& settings, const RECT& rect) {
    if (settings.albumWidgetAttachment == 0) {
        g_albumWidgetSeparateX.store(rect.left, std::memory_order_release);
        g_albumWidgetSeparateY.store(rect.top, std::memory_order_release);
        return;
    }

    const RECT anchor = GetThirdAlbumWidgetAnchorRect(settings);
    const RECT clamped = ClampAlbumWidgetAttachedRect(
        anchor, rect, settings.albumWidgetGap,
        settings.albumWidgetMaxDistance);
    const int offsetX = clamped.left - anchor.left;
    const int offsetY = clamped.top - anchor.top;
    if (settings.albumWidgetAttachment == 1) {
        g_albumWidgetLyricsOffsetX.store(offsetX, std::memory_order_release);
        g_albumWidgetLyricsOffsetY.store(offsetY, std::memory_order_release);
    } else {
        g_albumWidgetVisualizerOffsetX.store(offsetX, std::memory_order_release);
        g_albumWidgetVisualizerOffsetY.store(offsetY, std::memory_order_release);
    }
}

// Commit the final drag position to Windhawk settings. This is intentionally
// called only when the drag ends (or is cancelled), never for every mouse move.
static void StoreThirdAlbumWidgetRect(
    const VisualizerSettings& settings, const RECT& rect) {
    if (settings.albumWidgetAttachment == 0) {
        g_albumWidgetSeparateX.store(rect.left, std::memory_order_release);
        g_albumWidgetSeparateY.store(rect.top, std::memory_order_release);
        Wh_SetIntValue(L"albumWidget.separate.x", rect.left);
        Wh_SetIntValue(L"albumWidget.separate.y", rect.top);
        return;
    }

    const RECT anchor = GetThirdAlbumWidgetAnchorRect(settings);
    const RECT clamped = ClampAlbumWidgetAttachedRect(
        anchor, rect, settings.albumWidgetGap,
        settings.albumWidgetMaxDistance);
    const int offsetX = clamped.left - anchor.left;
    const int offsetY = clamped.top - anchor.top;
    if (settings.albumWidgetAttachment == 1) {
        g_albumWidgetLyricsOffsetX.store(offsetX, std::memory_order_release);
        g_albumWidgetLyricsOffsetY.store(offsetY, std::memory_order_release);
        Wh_SetIntValue(L"albumWidget.lyrics.offsetX", offsetX);
        Wh_SetIntValue(L"albumWidget.lyrics.offsetY", offsetY);
    } else {
        g_albumWidgetVisualizerOffsetX.store(offsetX, std::memory_order_release);
        g_albumWidgetVisualizerOffsetY.store(offsetY, std::memory_order_release);
        Wh_SetIntValue(L"albumWidget.visualizer.offsetX", offsetX);
        Wh_SetIntValue(L"albumWidget.visualizer.offsetY", offsetY);
    }
}

static bool IsAlbumWidgetOverlayWindow(HWND hwnd) {
    if (!hwnd)
        return false;
    for (HWND overlay : g_overlayWindows) {
        if (overlay == hwnd)
            return true;
    }
    return false;
}

static void DrawAlbumWidgetBorder(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    const VisualizerSettings& settings) {
    const bool lyricsAnchor = settings.albumWidgetAttachment == 1;
    const bool enabled = lyricsAnchor
        ? (settings.lyricsBackgroundEnabled && settings.lyricsBorderEnabled)
        : settings.backgroundBorderEnabled;
    const int opacity = lyricsAnchor
        ? settings.lyricsBorderOpacity
        : settings.backgroundBorderOpacity;
    if (!enabled || opacity <= 0)
        return;

    const int mode = lyricsAnchor
        ? settings.lyricsBorderMode
        : settings.backgroundBorderMode;
    const int thickness = std::max(1, lyricsAnchor
        ? settings.lyricsBorderThickness
        : settings.backgroundBorderThickness);
    const BYTE alpha = static_cast<BYTE>(
        std::clamp(opacity, 0, 100) * 255 / 100);

    DWORD color1 = lyricsAnchor ? settings.lyricsBorderColor1
                                : settings.backgroundBorderColor1;
    DWORD color2 = lyricsAnchor ? settings.lyricsBorderColor2
                                : settings.backgroundBorderColor2;
    if (mode == 0 || mode == 1) {
        color1 = GetAlbumPalettePrimary();
        color2 = GetAlbumPaletteSecondary();
    }

    const int radius = lyricsAnchor
        ? settings.lyricsRounding
        : settings.backgroundCornerRadius;
    Gdiplus::GraphicsPath path;
    AddRoundedRectSubpath(
        path,
        static_cast<float>(rect.left), static_cast<float>(rect.top),
        static_cast<float>(rect.right - rect.left),
        static_cast<float>(rect.bottom - rect.top),
        static_cast<float>(std::clamp<int>(
            radius, 0, std::min(rect.right - rect.left, rect.bottom - rect.top) / 2)));

    if (mode == 1 || mode == 3) {
        Gdiplus::LinearGradientBrush brush(
            Gdiplus::PointF(static_cast<float>(rect.left), static_cast<float>(rect.top)),
            Gdiplus::PointF(static_cast<float>(rect.right), static_cast<float>(rect.bottom)),
            Gdiplus::Color(alpha, GetRValue(color1), GetGValue(color1), GetBValue(color1)),
            Gdiplus::Color(alpha, GetRValue(color2), GetGValue(color2), GetBValue(color2)));
        Gdiplus::Pen pen(&brush, static_cast<Gdiplus::REAL>(thickness));
        graphics.DrawPath(&pen, &path);
    } else {
        Gdiplus::Pen pen(
            Gdiplus::Color(alpha, GetRValue(color1), GetGValue(color1), GetBValue(color1)),
            static_cast<Gdiplus::REAL>(thickness));
        graphics.DrawPath(&pen, &path);
    }
}

static void DrawAlbumWidget(Gdiplus::Graphics& graphics) {
    const VisualizerSettings& settings = g_settings;
    if (!settings.albumWidgetEnabled || settings.albumWidgetOpacity <= 0)
        return;

    const RECT rect = GetThirdAlbumWidgetRect(settings);
    const int w = rect.right - rect.left;
    const int h = rect.bottom - rect.top;
    if (w <= 0 || h <= 0)
        return;

    if (!EnsureAlbumWidgetCache(settings))
        return;

    // The cache is exactly widget-sized, so the hot path does one 1:1 image
    // composite instead of resizing, clipping, color-matrix setup, and border
    // construction on every visualizer frame.
    graphics.DrawImage(
        g_albumWidgetCacheBitmap,
        Gdiplus::RectF(static_cast<float>(rect.left), static_cast<float>(rect.top),
                       static_cast<float>(w), static_cast<float>(h)));
}

static void RenderOverlay(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    EnsureForegroundImageLoaded();

    RECT screenRect{};
    if (!GetWindowRect(hwnd, &screenRect)) {
        return;
    }
    g_overlayScreenRect = screenRect;

    RECT rc{};
    GetClientRect(hwnd, &rc);
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) {
        return;
    }

    if (!EnsureRenderTarget(w, h)) {
        return;
    }

    // The persistent render target must be cleared completely before each
    // frame. Partial dirty-region clearing can leave stale pixels behind when
    // a bar shrinks, especially with mirrored/circular rendering or effects
    // whose geometry changes with the bar height. Those stale pixels appear as
    // "ghost" bars even though g_currentHeights is already falling normally.
    //
    // Keep the persistent DIB for allocation stability, but use a full-frame
    // clear/upload so every frame exactly represents the current visualizer.
    //
    // The opaque raised-desktop layer hides the real wallpaper, so its frames
    // start from a copy of the wallpaper instead of transparent black.
    if (!(g_overlayBuiltRenderMode == OVERLAY_RENDER_OPAQUE_HOLDER &&
          OverlayFillWallpaperBase(hwnd, w, h))) {
        for (int y = 0; y < h; ++y) {
            std::memset(
                static_cast<BYTE*>(g_renderBits) +
                    static_cast<size_t>(y) * static_cast<size_t>(g_renderWidth) * sizeof(DWORD),
                0,
                static_cast<size_t>(w) * sizeof(DWORD));
        }
    }

    {
        Gdiplus::Graphics graphics(g_renderMemDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

        RenderVisualizerPass(graphics, w, h);

        if (g_settings.mirroredVisualizer) {
            const int axis = (g_settings.orientation > 2) ? 2 : 1;
            g_mirrorRenderAxis = axis;
            g_mirrorRenderPass = true;

            if (g_settings.barShape == 4) {
                g_mirrorRenderAxis = 2;
                g_mirrorCircularPass = true;
                RenderVisualizerPass(graphics, w, h);
                g_mirrorCircularPass = false;
            } else {
                Gdiplus::GraphicsState mirrorState = graphics.Save();
                if (axis == 1) {
                    graphics.TranslateTransform(0.0f, static_cast<float>(h));
                    graphics.ScaleTransform(1.0f, -1.0f);
                } else {
                    graphics.TranslateTransform(static_cast<float>(w), 0.0f);
                    graphics.ScaleTransform(-1.0f, 1.0f);
                }
                RenderVisualizerPass(graphics, w, h);
                graphics.Restore(mirrorState);
            }

            g_mirrorRenderPass = false;
            g_mirrorRenderAxis = 0;
        }

        DrawLyricsWidget(graphics);
        DrawAlbumWidget(graphics);

        if (g_pForegroundImage) {
            int drawW = (g_settings.imageWidth > 0)
                ? g_settings.imageWidth
                : g_pForegroundImage->GetWidth();
            int drawH = (g_settings.imageHeight > 0)
                ? g_settings.imageHeight
                : g_pForegroundImage->GetHeight();

            graphics.DrawImage(
                g_pForegroundImage,
                g_settings.imageX,
                g_settings.imageY,
                drawW,
                drawH
            );
        }
    }

    // Raised-desktop modes present through Direct3D / DirectComposition.
    if (g_overlayBuiltRenderMode != OVERLAY_RENDER_ULW) {
        OverlayPresentFrame(hwnd, w, h);
        return;
    }

    // For a top-level window pptDst is the screen position. For a CHILD
    // window it is relative to the parent's client area, so passing {0,0}
    // moves the overlay to the desktop's top-left corner on every frame.
    // That corner is only the primary monitor's origin when no monitor sits
    // left of or above the primary; on other layouts the overlay is shifted
    // off its monitor and the periodic rebuild pass then recreates it every
    // second. Child overlays are positioned by CreateWindowExW/SetWindowPos,
    // so leave their position untouched here (pptDst = nullptr).
    const bool isChildOverlay = GetParent(hwnd) != nullptr;
    POINT dstPos{0, 0};
    if (!isChildOverlay) {
        RECT windowRect{};
        if (GetWindowRect(hwnd, &windowRect)) {
            dstPos.x = windowRect.left;
            dstPos.y = windowRect.top;
        }
    }
    SIZE size{w, h};
    POINT srcPos{0, 0};
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UPDATELAYEREDWINDOWINFO updateInfo{};
    updateInfo.cbSize = sizeof(updateInfo);
    updateInfo.pptDst = isChildOverlay ? nullptr : &dstPos;
    updateInfo.psize = &size;
    updateInfo.hdcSrc = g_renderMemDC;
    updateInfo.pptSrc = &srcPos;
    updateInfo.pblend = &blend;
    updateInfo.dwFlags = ULW_ALPHA;
    // nullptr updates the complete layered window, matching the previous
    // full-window dirty rectangle without keeping throw-away state.
    updateInfo.prcDirty = nullptr;

    UpdateLayeredWindowIndirect(hwnd, &updateInfo);

}

static UINT GetRenderIntervalMs(const VisualizerSettings& settings);
static UINT GetOverlayTimerIntervalMs(const VisualizerSettings& settings);

static UINT g_cachedDisplayRefreshRateHz = 0;
static UINT g_currentOverlayTimerMs = 0;

// Desktop layout editor (defined next to the overlay thread). While it is
// active the overlay is frozen: no audio-driven frames, no repair passes.
static std::atomic<bool> g_layoutEditActive{false};
static void LayoutEditBegin();
static void LayoutEditEnd(bool apply);

// Automatically throttle only the overlay monitor currently occupied by a
// fullscreen/borderless-fullscreen foreground window. The user's configured
// targetFps is never modified.
static std::atomic<bool> g_fullscreenThrottleActive{false};
static std::atomic<HMONITOR> g_fullscreenMonitor{nullptr};
static ULONGLONG g_lastFullscreenCheckMs = 0;


// popup implementation
static constexpr int EQ_POPUP_WIDTH = 700;
static constexpr int EQ_POPUP_HEIGHT = 530;

// The builder always uses the full logical canvas. The main page may shrink to
// the bounding box of placed widgets; these offsets keep the widgets at the same
// screen coordinates after the popup itself is cropped.
static int g_eqPopupLogicalWidth = EQ_POPUP_WIDTH;
static int g_eqPopupLogicalHeight = EQ_POPUP_HEIGHT;
static int g_eqPopupContentOffsetX = 0;
static int g_eqPopupContentOffsetY = 0;

// Slightly smaller page button (gear/back) so it stays visually separate from
// the media source text while keeping the same top-right placement.
static constexpr int EQ_PAGE_BUTTON_TOP = 8;
static constexpr int EQ_PAGE_BUTTON_BOTTOM = 48;
static constexpr UINT kEqPopupPageAnimationTimerId = 0x4E55;
static constexpr UINT kEqPopupPageAnimationIntervalMs = 16;
static constexpr ULONGLONG kEqPopupPageAnimationDurationMs = 220;

static constexpr int EQ_BUTTON_WIDTH = 32;
static constexpr int EQ_BUTTON_HEIGHT = 32;
static constexpr int EQ_POPUP_TASKBAR_GAP = 16;
static constexpr UINT kEqPopupAnimationTimerId = 0x4E53;
static constexpr UINT kEqPopupAnimationIntervalMs = 16;
static HWND g_eqPopupHwnd = nullptr;
static HWND g_eqTaskbarHwnd = nullptr;
static HINSTANCE g_eqModuleHandle = nullptr;
static bool g_eqClassesRegistered = false;
static int g_eqHotBand = -1;
static int g_eqDraggingBand = -1;
static int g_eqHotPreset = -1;
static const wchar_t* kEqPopupClass = L"WindhawkVisualizerEQPopup";

static constexpr int EQ_PLUS_PRESET_HIT = -2;
static constexpr std::array<const wchar_t*, VIZ_EQ_BUILTIN_PRESET_COUNT> VIZ_EQ_PRESET_NAMES = {
    L"Flat", L"Bass Boost", L"Bass Cut", L"Treble Boost", L"Vocal",
    L"Rock", L"Pop", L"Classical"
};

// 1.00x is neutral. Presets are deliberately moderate so they remain useful
// as starting points instead of acting like hard limiting or a volume boost.
static constexpr std::array<std::array<float, VIZ_EQ_BANDS>, VIZ_EQ_BUILTIN_PRESET_COUNT>
    VIZ_EQ_PRESET_GAINS = {{
        {{1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f}},
        {{1.38f, 1.32f, 1.24f, 1.14f, 1.06f, 1.00f, 0.98f, 0.96f, 0.94f, 0.92f}},
        {{0.44f, 0.60f, 0.75f, 0.94f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f}},
        {{0.90f, 0.92f, 0.95f, 0.98f, 1.00f, 1.12f, 1.25f, 1.42f, 1.52f, 1.68f}},
        {{0.94f, 0.93f, 1.05f, 1.16f, 1.32f, 1.28f, 1.12f, 1.06f, 0.96f, 0.93f}},
        {{1.31f, 1.19f, 1.06f, 0.98f, 0.93f, 1.02f, 1.14f, 1.22f, 1.18f, 1.12f}},
        {{1.10f, 1.03f, 0.98f, 0.98f, 1.04f, 1.08f, 1.13f, 1.18f, 1.14f, 1.08f}},
        {{1.00f, 0.96f, 0.93f, 0.96f, 1.04f, 1.08f, 1.08f, 1.04f, 1.00f, 0.96f}},
    }};

static int g_eqHotPresetDelete = -1;
static int g_eqHotMediaButton = -1;
static bool g_eqHotPageButton = false;
static std::array<float, VIZ_EQ_BANDS> g_eqPresetTargets{};
static ULONGLONG g_eqLastAnimationTick = 0;
static bool g_eqPresetAnimationActive = false;
static bool g_eqMediaSeeking = false;
static double g_eqMediaSeekPreviewSeconds = 0.0;
static int g_eqLyricsScrollOffset = 0;
static int g_eqHotLyricsLine = -1;
static std::wstring g_eqLyricsScrollTrackKey;
static bool g_eqLyricsScrollbarDragging = false;
static int g_eqLyricsScrollbarDragStartY = 0;
static int g_eqLyricsScrollbarStartOffset = 0;

// Manual scrolling temporarily suspends automatic focus-line following.
// Each wheel/scrollbar movement refreshes this deadline, so the 3.5 s delay
// is measured from the user's last manual scroll action.
static ULONGLONG g_eqLyricsAutoFocusResumeTick = 0;
static constexpr ULONGLONG kEqLyricsAutoFocusPauseMs = 3500;

// 0 = Media & EQ page, 1 = Settings page. The value is animated so both
// pages slide horizontally instead of abruptly replacing each other.
static float g_eqPageAnimationProgress = 0.0f;
static float g_eqPageAnimationStart = 0.0f;
static float g_eqPageAnimationTarget = 0.0f;
static ULONGLONG g_eqPageAnimationStartTick = 0;
static bool g_eqPageAnimationActive = false;

static constexpr UINT WM_EQ_NATIVE_TRAY = WM_APP + 0x2A3;
static constexpr UINT kEqNativeTrayIconId = 0x4E51;
static constexpr wchar_t kEqNativeTrayWindowClass[] =
    L"WindhawkVisualizerEQTrayMessageWindow";
static const GUID kEqNativeTrayGuid =
    {0x6ad3e8d6, 0x72f1, 0x4c7e,
     {0x9c, 0x83, 0x7f, 0x3f, 0x39, 0x91, 0x0d, 0x7d}};
static HWND g_eqNativeTrayMessageHwnd = nullptr;
static bool g_eqNativeTrayRegistered = false;
static UINT g_eqTaskbarCreatedMessage = 0;
static constexpr UINT kEqNativeTrayRetryTimerId = 0x4E52;
static constexpr UINT kEqNativeTrayRetryDelayMs = 300;
static constexpr int kEqNativeTrayMaxRetryCount = 8;
static int g_eqNativeTrayRetryCount = 0;

static LRESULT CALLBACK EqNativeTrayMessageProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static int EqPopupScalePx(int value, double scale);
static double EqGetPopupDpiScale();
static void RenderEqPopup(HWND hwnd);

static bool IsDarkThemeEnabled() {
    DWORD data = 0;
    DWORD dataSize = sizeof(data);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme",
                     RRF_RT_REG_DWORD,
                     nullptr, &data, &dataSize) == ERROR_SUCCESS) {
        return data == 0;
    }
    return true;
}
static void ApplyEqPopupWindowAttributes(HWND hwnd) {
    if (!hwnd)
        return;

    const BOOL dark = IsDarkThemeEnabled() ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
                          &dark, sizeof(dark));
}

static EqLayoutWidgetState MakeDefaultEqLayoutWidget(int id) {
    EqLayoutWidgetState w{};
    w.id = id;
    w.rotation = 0;
    w.present = true;
    if (id == EQ_LAYOUT_MEDIA) {
        w.x = 0; w.y = 0; w.w = 700; w.h = 100;
    } else if (id == EQ_LAYOUT_EQ) {
        w.x = 0; w.y = 110; w.w = 480; w.h = 410;
    } else if (id == EQ_LAYOUT_PRESETS) {
        w.x = 490; w.y = 110; w.w = 210; w.h = 410;
    } else {
        // New interactive lyrics section is opt-in, so the existing three
        // sections keep their current default arrangement unchanged.
        w.x = 140; w.y = 130; w.w = 420; w.h = 360;
        w.present = false;
    }
    return w;
}

static int EqLayoutMinWidth(int id) {
    switch (id) {
    case EQ_LAYOUT_MEDIA: return 220;
    case EQ_LAYOUT_EQ: return 220;
    case EQ_LAYOUT_PRESETS: return 150;
    case EQ_LAYOUT_LYRICS: return 180;
    default: return 100;
    }
}

static int EqLayoutMinHeight(int id) {
    switch (id) {
    case EQ_LAYOUT_MEDIA: return 100;
    case EQ_LAYOUT_EQ: return 180;
    case EQ_LAYOUT_PRESETS: return 140;
    case EQ_LAYOUT_LYRICS: return 100;
    default: return 70;
    }
}

static int EqSnap10(int value) {
    return static_cast<int>(std::lround(static_cast<double>(value) /
                                         static_cast<double>(EQ_LAYOUT_GRID))) * EQ_LAYOUT_GRID;
}

static void ClampEqLayoutWidget(EqLayoutWidgetState& w) {
    w.x = std::clamp(w.x, 0, EQ_LAYOUT_CANVAS_W - 1);
    w.y = std::clamp(w.y, 0, EQ_LAYOUT_CANVAS_H - 1);
    w.w = std::clamp(w.w, EqLayoutMinWidth(w.id), EQ_LAYOUT_CANVAS_W);
    w.h = std::clamp(w.h, EqLayoutMinHeight(w.id), EQ_LAYOUT_CANVAS_H);
    w.x = std::clamp(w.x, 0, std::max(0, EQ_LAYOUT_CANVAS_W - w.w));
    w.y = std::clamp(w.y, 0, std::max(0, EQ_LAYOUT_CANVAS_H - w.h));
    w.rotation = ((w.rotation % 360) + 360) % 360;
    w.rotation = (w.rotation / 90) * 90;
}

static EqLayoutWidgetState* EqGetLayoutWidget(int id) {
    for (auto& w : g_eqLayoutWidgets) {
        if (w.id == id)
            return &w;
    }
    return nullptr;
}

static const EqLayoutWidgetState* EqGetLayoutWidgetConst(int id) {
    for (const auto& w : g_eqLayoutWidgets) {
        if (w.id == id)
            return &w;
    }
    return nullptr;
}

static bool EqIsLayoutWidgetPresent(int id) {
    const auto* w = EqGetLayoutWidgetConst(id);
    return w && w->present;
}

static void ResetEqLayoutToDefaults() {
    g_eqLayoutWidgets[0] = MakeDefaultEqLayoutWidget(EQ_LAYOUT_MEDIA);
    g_eqLayoutWidgets[1] = MakeDefaultEqLayoutWidget(EQ_LAYOUT_EQ);
    g_eqLayoutWidgets[2] = MakeDefaultEqLayoutWidget(EQ_LAYOUT_PRESETS);
    g_eqLayoutWidgets[3] = MakeDefaultEqLayoutWidget(EQ_LAYOUT_LYRICS);
}

static int EqLayoutGetStoredField(int id, const wchar_t* field, int fallback) {
    wchar_t key[128]{};
    swprintf_s(key, L"MediaEQ.layout.%d.%s", id, field);
    return Wh_GetIntValue(key, fallback);
}

static void EqLayoutSetStoredField(int id, const wchar_t* field, int value) {
    wchar_t key[128]{};
    swprintf_s(key, L"MediaEQ.layout.%d.%s", id, field);
    Wh_SetIntValue(key, value);
}

static void LoadEqLayoutSettings() {
    ResetEqLayoutToDefaults();

    const int version = Wh_GetIntValue(L"MediaEQ.layout.version", 0);
    if (version == EQ_LAYOUT_STORAGE_VERSION) {
        for (auto& w : g_eqLayoutWidgets) {
            const auto defaults = MakeDefaultEqLayoutWidget(w.id);
            w.x = EqSnap10(EqLayoutGetStoredField(w.id, L"x", defaults.x));
            w.y = EqSnap10(EqLayoutGetStoredField(w.id, L"y", defaults.y));
            w.w = EqSnap10(EqLayoutGetStoredField(w.id, L"w", defaults.w));
            w.h = EqSnap10(EqLayoutGetStoredField(w.id, L"h", defaults.h));
            w.rotation = EqLayoutGetStoredField(w.id, L"rotation", defaults.rotation);
            w.present = EqLayoutGetStoredField(w.id, L"present", defaults.present ? 1 : 0) != 0;
            if (w.id != EQ_LAYOUT_EQ)
                w.rotation = 0;
            ClampEqLayoutWidget(w);
        }
        g_eqLayoutLoaded = true;
        return;
    }

    if (version == 1) {
        // Preserve an existing v1 Layout Builder arrangement exactly, then add
        // the new lyrics section as absent. Do not reset the user's old layout.
        for (auto& w : g_eqLayoutWidgets) {
            if (w.id == EQ_LAYOUT_LYRICS)
                continue;
            const auto defaults = MakeDefaultEqLayoutWidget(w.id);
            w.x = EqSnap10(EqLayoutGetStoredField(w.id, L"x", defaults.x));
            w.y = EqSnap10(EqLayoutGetStoredField(w.id, L"y", defaults.y));
            w.w = EqSnap10(EqLayoutGetStoredField(w.id, L"w", defaults.w));
            w.h = EqSnap10(EqLayoutGetStoredField(w.id, L"h", defaults.h));
            w.rotation = EqLayoutGetStoredField(w.id, L"rotation", defaults.rotation);
            w.present = EqLayoutGetStoredField(w.id, L"present", defaults.present ? 1 : 0) != 0;
            if (w.id != EQ_LAYOUT_EQ)
                w.rotation = 0;
            ClampEqLayoutWidget(w);
        }
        g_eqLayoutWidgets[3] = MakeDefaultEqLayoutWidget(EQ_LAYOUT_LYRICS);
        g_eqLayoutLoaded = true;
        SaveEqLayoutSettings();
        return;
    }

    // One-time migration from the old Show media section toggle. A disabled
    // media section becomes an absent media widget; all other defaults are
    // preserved exactly as before.
    const bool legacyShowMedia =
        Wh_GetIntValue(L"MediaEQ.showMediaSection", 1) != 0;
    if (!legacyShowMedia) {
        if (auto* media = EqGetLayoutWidget(EQ_LAYOUT_MEDIA))
            media->present = false;
    }
    g_eqLayoutLoaded = true;
    SaveEqLayoutSettings();
}

static void SaveEqLayoutSettings() {
    Wh_SetIntValue(L"MediaEQ.layout.version", EQ_LAYOUT_STORAGE_VERSION);
    Wh_SetIntValue(L"MediaEQ.layout.canvas.w", EQ_LAYOUT_CANVAS_W);
    Wh_SetIntValue(L"MediaEQ.layout.canvas.h", EQ_LAYOUT_CANVAS_H);
    for (const auto& w : g_eqLayoutWidgets) {
        EqLayoutSetStoredField(w.id, L"x", w.x);
        EqLayoutSetStoredField(w.id, L"y", w.y);
        EqLayoutSetStoredField(w.id, L"w", w.w);
        EqLayoutSetStoredField(w.id, L"h", w.h);
        EqLayoutSetStoredField(w.id, L"rotation", w.rotation);
        EqLayoutSetStoredField(w.id, L"present", w.present ? 1 : 0);
    }
}

static EqPopupPlacementState EqGetDefaultPopupPlacement(int aspect) {
    (void)aspect;
    return EqPopupPlacementState{};
}

static int EqPopupPlacementGetStoredField(int aspect, const wchar_t* field, int fallback) {
    wchar_t key[160]{};
    swprintf_s(key, L"MediaEQ.popupPlacement.%d.%s", aspect, field);
    return Wh_GetIntValue(key, fallback);
}

static void EqPopupPlacementSetStoredField(int aspect, const wchar_t* field, int value) {
    wchar_t key[160]{};
    swprintf_s(key, L"MediaEQ.popupPlacement.%d.%s", aspect, field);
    Wh_SetIntValue(key, value);
}

static void ClampEqPopupPlacementState(EqPopupPlacementState& state) {
    // Popup and cross are intentionally independent. The popup center is only
    // normalized screen position; the canvas-bound clamp is handled separately
    // because it depends on the popup preview size.
    state.popupCenterX = std::clamp(state.popupCenterX, 0, 1000);
    state.popupCenterY = std::clamp(state.popupCenterY, 0, 1000);
    // Keep the cross away from the exact outer edge so all four guide regions remain usable.
    state.crossX = std::clamp(state.crossX, 1, 999);
    state.crossY = std::clamp(state.crossY, 1, 999);
}

static POINT EqGetPopupPlacementCenterNormalized(const EqPopupPlacementState& state) {
    return POINT{
        std::clamp(state.popupCenterX, 0, 1000),
        std::clamp(state.popupCenterY, 0, 1000)
    };
}

static void EqSetPopupPlacementFromCenterNormalized(EqPopupPlacementState& state,
                                                     int centerX, int centerY) {
    state.popupCenterX = std::clamp(centerX, 0, 1000);
    state.popupCenterY = std::clamp(centerY, 0, 1000);
    ClampEqPopupPlacementState(state);
}

static constexpr int EQ_POPUP_PLACEMENT_STORAGE_VERSION = 3;

static void LoadEqPopupPlacementSettings() {
    g_eqPopupPlacementAspect = std::clamp(
        Wh_GetIntValue(L"MediaEQ.popupPlacement.aspect", EQ_POPUP_ASPECT_16_9),
        0, EQ_POPUP_ASPECT_COUNT - 1);
    const int version = Wh_GetIntValue(L"MediaEQ.popupPlacement.version", 0);

    for (int aspect = 0; aspect < EQ_POPUP_ASPECT_COUNT; ++aspect) {
        auto state = EqGetDefaultPopupPlacement(aspect);
        state.crossX = std::clamp(EqPopupPlacementGetStoredField(aspect, L"crossX", state.crossX), 1, 999);
        state.crossY = std::clamp(EqPopupPlacementGetStoredField(aspect, L"crossY", state.crossY), 1, 999);

        if (version >= EQ_POPUP_PLACEMENT_STORAGE_VERSION) {
            // Version 3 stores the popup center directly, so popup movement is
            // completely independent from the cross/guide position.
            state.popupCenterX = EqPopupPlacementGetStoredField(aspect, L"popupCenterX", state.popupCenterX);
            state.popupCenterY = EqPopupPlacementGetStoredField(aspect, L"popupCenterY", state.popupCenterY);
            ClampEqPopupPlacementState(state);
        } else if (version >= 2) {
            // Migrate the version-2 quadrant + offset representation to the new
            // independent center representation without visibly moving the popup.
            const int legacyQuadrant = std::clamp(
                EqPopupPlacementGetStoredField(aspect, L"popupQuadrant", 3), 0, 3);
            const int legacyOffsetX = std::clamp(
                EqPopupPlacementGetStoredField(aspect, L"popupOffsetX", 0), -1000, 1000);
            const int legacyOffsetY = std::clamp(
                EqPopupPlacementGetStoredField(aspect, L"popupOffsetY", 0), -1000, 1000);
            const bool rightSide = (legacyQuadrant & 1) != 0;
            const bool bottomSide = (legacyQuadrant & 2) != 0;
            const int left = rightSide ? state.crossX : 0;
            const int right = rightSide ? 1000 : state.crossX;
            const int top = bottomSide ? state.crossY : 0;
            const int bottom = bottomSide ? 1000 : state.crossY;
            state.popupCenterX = (left + right) / 2 + legacyOffsetX;
            state.popupCenterY = (top + bottom) / 2 + legacyOffsetY;
            ClampEqPopupPlacementState(state);
        } else {
            // Migrate the original absolute 0..1000 center format.
            const int oldPopupX = EqPopupPlacementGetStoredField(aspect, L"popupX", state.popupCenterX);
            const int oldPopupY = EqPopupPlacementGetStoredField(aspect, L"popupY", state.popupCenterY);
            EqSetPopupPlacementFromCenterNormalized(state, oldPopupX, oldPopupY);
        }

        g_eqPopupPlacement[static_cast<size_t>(aspect)] = state;
    }
    g_eqPopupPlacementLoaded = true;
}

static void SaveEqPopupPlacementSettings() {
    Wh_SetIntValue(L"MediaEQ.popupPlacement.version", EQ_POPUP_PLACEMENT_STORAGE_VERSION);
    Wh_SetIntValue(L"MediaEQ.popupPlacement.aspect", g_eqPopupPlacementAspect);
    for (int aspect = 0; aspect < EQ_POPUP_ASPECT_COUNT; ++aspect) {
        const auto& state = g_eqPopupPlacement[static_cast<size_t>(aspect)];
        EqPopupPlacementSetStoredField(aspect, L"popupCenterX", state.popupCenterX);
        EqPopupPlacementSetStoredField(aspect, L"popupCenterY", state.popupCenterY);
        EqPopupPlacementSetStoredField(aspect, L"crossX", state.crossX);
        EqPopupPlacementSetStoredField(aspect, L"crossY", state.crossY);
    }
}

static RECT EqGetLayoutRectRaw(int id) {
    const auto* w = EqGetLayoutWidgetConst(id);
    if (!w || !w->present)
        return RECT{0, 0, 0, 0};
    return RECT{w->x, w->y, w->x + w->w, w->y + w->h};
}

static RECT EqGetLayoutRect(int id) {
    RECT r = EqGetLayoutRectRaw(id);
    if (r.right <= r.left || r.bottom <= r.top)
        return r;
    r.left -= g_eqPopupContentOffsetX;
    r.right -= g_eqPopupContentOffsetX;
    r.top -= g_eqPopupContentOffsetY;
    r.bottom -= g_eqPopupContentOffsetY;
    return r;
}

static bool EqPointInLayoutRect(int id, int x, int y) {
    const RECT r = EqGetLayoutRect(id);
    return r.right > r.left && PtInRect(&r, POINT{x, y}) != FALSE;
}

static bool EqLayoutWidgetIsVertical(const EqLayoutWidgetState& widget) {
    return widget.rotation == 90 || widget.rotation == 270 || widget.h > widget.w;
}

static float EqClampLayoutScale(float value) {
    return std::clamp(value, 0.08f, 4.0f);
}

struct EqMediaMetrics {
    RECT timeline{};
    std::array<RECT, 5> buttons{};
};

static EqMediaMetrics EqBuildMediaMetricsForRect(const RECT& r, bool vertical) {
    EqMediaMetrics result{};
    const int width = std::max(1, static_cast<int>(r.right - r.left));
    const int height = std::max(1, static_cast<int>(r.bottom - r.top));

    if (vertical) {
        // Vertical Media no longer reserves space for album art. The text sits
        // first, followed by the timeline and transport controls.
        const int textBlock = std::max(34, std::min(52, height / 3));
        const int timelineY = static_cast<int>(r.top) + textBlock +
            std::max(8, std::min(18, (height - textBlock) / 5));
        const int left = r.left + 18;
        const int right = std::max(left + 1, static_cast<int>(r.right) - 18);
        const int timelineTop = std::clamp(timelineY - 7,
            static_cast<int>(r.top) + 4,
            static_cast<int>(r.bottom) - 46);
        const int timelineBottom = std::clamp(timelineY + 7,
            timelineTop + 1,
            static_cast<int>(r.bottom) - 34);
        result.timeline = RECT{left, timelineTop, right, timelineBottom};

        const int buttonWidth = std::clamp((width - 28 - 4 * 6) / 5, 24, 42);
        const int total = buttonWidth * 5 + 6 * 4;
        const int startX = static_cast<int>(r.left) + std::max(0, (width - total) / 2);
        const int top = std::clamp(result.timeline.bottom + 10,
            r.top + 4,
            r.bottom - 30);
        for (int i = 0; i < 5; ++i) {
            const int x = startX + i * (buttonWidth + 6);
            result.buttons[static_cast<size_t>(i)] = RECT{
                x, top, x + buttonWidth,
                std::min(static_cast<int>(r.bottom), top + 30)};
        }
    } else {
        // Horizontal media is deliberately compact enough for the default
        // 700x100 layout: title/artist, timeline, then five transport buttons.
        const int timelineY = static_cast<int>(r.top) + std::clamp(height * 52 / 100, 42, std::max(42, height - 40));
        const int timelineMargin = std::min(40, std::max(22, width / 12));
        const int left = static_cast<int>(r.left) + timelineMargin;
        const int right = std::max(left + 1, static_cast<int>(r.right) - timelineMargin);
        result.timeline = RECT{left, timelineY - 8, right, timelineY + 8};
        const int buttonHeight = std::clamp(height / 3, 28, 34);
        constexpr int gap = 6;
        const int buttonWidth = std::clamp((width - 20 - gap * 4) / 5, 24, 42);
        const int total = buttonWidth * 5 + gap * 4;
        const int startX = static_cast<int>(r.left) + std::max(0, (width - total) / 2);
        const int top = std::clamp(timelineY + 10,
                                   static_cast<int>(r.top) + 2,
                                   static_cast<int>(r.bottom) - buttonHeight - 2);
        for (int i = 0; i < 5; ++i) {
            const int x = startX + i * (buttonWidth + gap);
            result.buttons[static_cast<size_t>(i)] = RECT{
                x, top, x + buttonWidth, std::min(static_cast<int>(r.bottom), top + buttonHeight)};
        }
    }
    return result;
}

static EqMediaMetrics EqGetMediaMetrics() {
    const auto* w = EqGetLayoutWidgetConst(EQ_LAYOUT_MEDIA);
    if (!w || !w->present)
        return EqMediaMetrics{};
    const RECT r = EqGetLayoutRect(EQ_LAYOUT_MEDIA);
    return EqBuildMediaMetricsForRect(r, EqLayoutWidgetIsVertical(*w));
}

static RECT GetEqMediaTimelineRect() {
    return EqGetMediaMetrics().timeline;
}

static RECT GetEqMediaButtonRect(int index) {
    if (index < 0 || index >= 5)
        return RECT{0, 0, 0, 0};
    return EqGetMediaMetrics().buttons[static_cast<size_t>(index)];
}

static int HitTestEqMediaButton(int x, int y) {
    const double scale = EqGetPopupDpiScale();
    const POINT point{static_cast<int>(std::lround(static_cast<double>(x) / scale)),
                      static_cast<int>(std::lround(static_cast<double>(y) / scale))};
    for (int i = 0; i < 5; ++i) {
        const RECT r = GetEqMediaButtonRect(i);
        if (PtInRect(&r, point))
            return i;
    }
    return -1;
}

static bool HitTestEqMediaTimeline(int x, int y) {
    const double scale = EqGetPopupDpiScale();
    const POINT point{static_cast<int>(std::lround(static_cast<double>(x) / scale)),
                      static_cast<int>(std::lround(static_cast<double>(y) / scale))};
    const RECT r = GetEqMediaTimelineRect();
    return PtInRect(&r, point) != FALSE;
}

struct EqSliderMetrics {
    RECT area{};
    int top = 0;
    int bottom = 0;
    bool horizontal = false;
};

static EqSliderMetrics EqGetSliderMetrics() {
    EqSliderMetrics m{};
    const auto* widget = EqGetLayoutWidgetConst(EQ_LAYOUT_EQ);
    if (!widget || !widget->present)
        return m;

    const RECT r = EqGetLayoutRect(EQ_LAYOUT_EQ);
    if (r.right <= r.left)
        return m;

    m.horizontal = widget->rotation == 90 || widget->rotation == 270;
    if (m.horizontal) {
        m.area = RECT{
            r.left + 54,
            r.top + 38,
            std::max(static_cast<int>(r.left) + 55,
                     static_cast<int>(r.right) - 18),
            std::max(static_cast<int>(r.top) + 39,
                     static_cast<int>(r.bottom) - 18)};
    } else {
        m.area = RECT{
            r.left + 20,
            r.top + 46,
            std::max(static_cast<int>(r.left) + 21,
                     static_cast<int>(r.right) - 20),
            std::max(static_cast<int>(r.top) + 47,
                     static_cast<int>(r.bottom) - 42)};
    }
    m.top = m.area.top;
    m.bottom = m.area.bottom;
    return m;
}

static POINT GetEqSliderPoint(int index) {
    const EqSliderMetrics m = EqGetSliderMetrics();
    const int count = std::max(1, VIZ_EQ_BANDS - 1);
    const float t = static_cast<float>(index) / static_cast<float>(count);
    if (m.horizontal) {
        const int y = static_cast<int>(m.area.top) + static_cast<int>(std::round(
            t * std::max(0, static_cast<int>(m.area.bottom) - static_cast<int>(m.area.top))));
        const int x = static_cast<int>(m.area.left + m.area.right) / 2;
        return POINT{x, y};
    }
    const int x = static_cast<int>(m.area.left) + static_cast<int>(std::round(
        t * std::max(0, static_cast<int>(m.area.right) - static_cast<int>(m.area.left))));
    return POINT{x, m.top};
}

static int HitTestEqBand(int x, int y) {
    const double scale = EqGetPopupDpiScale();
    const int logicalX = static_cast<int>(std::lround(static_cast<double>(x) / scale));
    const int logicalY = static_cast<int>(std::lround(static_cast<double>(y) / scale));
    const EqSliderMetrics m = EqGetSliderMetrics();
    const RECT hitRect{
        m.area.left - 18, m.area.top - 18,
        m.area.right + 18, m.area.bottom + 18};
    if (!PtInRect(&hitRect, POINT{logicalX, logicalY}))
        return -1;

    int best = -1;
    int bestDistance = 24;
    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const POINT point = GetEqSliderPoint(i);
        const int distance = m.horizontal
            ? std::abs(logicalY - point.y)
            : std::abs(logicalX - point.x);
        if (distance < bestDistance) {
            bestDistance = distance;
            best = i;
        }
    }
    return best;
}

static bool EqHasCustomPresets() {
    return g_eqCustomPresetCount > 0;
}

static int EqGetPresetItemCount() {
    return VIZ_EQ_BUILTIN_PRESET_COUNT + g_eqCustomPresetCount;
}

struct EqPresetGridMetrics {
    int cols = 1;
    int rows = 1;
    int rowHeight = 22;
    int contentTop = 38;
    int gap = 6;
    int cellWidth = 30;
};

static EqPresetGridMetrics EqGetPresetGridMetrics(const EqLayoutWidgetState& widget) {
    EqPresetGridMetrics m{};
    const int count = std::max(1, EqGetPresetItemCount());
    const int panelW = std::max(1, widget.w);
    const int panelH = std::max(1, widget.h);
    const int availableHeight = std::max(1, panelH - 46);
    constexpr int targetRowHeight = 28;
    constexpr int cellGap = 6;
    constexpr int minCellWidth = 30;
    constexpr int maxColumns = 5;

    const int maxRowsOneColumn = std::max(1, availableHeight / targetRowHeight);
    const int widthColumnCapacity = std::max(1,
        (panelW - 20 + cellGap) / (minCellWidth + cellGap));
    const int allowedColumns = std::max(1, std::min(maxColumns, widthColumnCapacity));

    // Keep one column while the section is tall enough. Once it is not, add
    // columns according to the number of rows that the section can actually fit.
    m.cols = count <= maxRowsOneColumn
        ? 1
        : std::clamp((count + maxRowsOneColumn - 1) / maxRowsOneColumn,
                     1, allowedColumns);
    m.rows = (count + m.cols - 1) / m.cols;
    m.rowHeight = std::max(20, availableHeight / std::max(1, m.rows));
    m.contentTop = 38;
    m.gap = cellGap;

    const int usableW = std::max(1, panelW - 20);
    m.cellWidth = std::max(minCellWidth,
        (usableW - m.gap * (m.cols - 1)) / std::max(1, m.cols));
    return m;
}

static int EqPresetColumnCount(const EqLayoutWidgetState& widget) {
    return EqGetPresetGridMetrics(widget).cols;
}

static int EqPresetRowHeight(const EqLayoutWidgetState& widget) {
    return EqGetPresetGridMetrics(widget).rowHeight;
}

static RECT GetEqPresetRect(int itemIndex) {
    const auto* widget = EqGetLayoutWidgetConst(EQ_LAYOUT_PRESETS);
    if (!widget || !widget->present || itemIndex < 0)
        return RECT{0, 0, 0, 0};

    const RECT r = EqGetLayoutRect(EQ_LAYOUT_PRESETS);
    const EqPresetGridMetrics m = EqGetPresetGridMetrics(*widget);
    const int col = itemIndex % m.cols;
    const int row = itemIndex / m.cols;
    const int left = r.left + 10 + col * (m.cellWidth + m.gap);
    const int top = r.top + m.contentTop + row * m.rowHeight;
    const int right = left + m.cellWidth;
    const int bottom = std::min<int>(static_cast<int>(r.bottom) - 8,
                                     top + m.rowHeight - 4);
    if (right <= left || bottom <= top)
        return RECT{0, 0, 0, 0};
    return RECT{left, top, right, bottom};
}

static RECT GetEqPageButtonRect();
static RECT EqGetDesktopLayoutButtonRect();

static RECT GetEqPlusRect() {
    const auto* widget = EqGetLayoutWidgetConst(EQ_LAYOUT_PRESETS);
    if (!widget || !widget->present ||
        g_eqCustomPresetCount >= VIZ_EQ_MAX_CUSTOM_PRESETS)
        return RECT{0, 0, 0, 0};

    const RECT r = EqGetLayoutRect(EQ_LAYOUT_PRESETS);
    constexpr int buttonSize = 24;
    constexpr int rightMargin = 10;
    constexpr int leftMargin = 8;
    constexpr int gearGap = 8;

    int right = r.right - rightMargin;
    const RECT gear = GetEqPageButtonRect();
    const RECT headerRect{r.left, r.top + 4, r.right, r.top + 36};

    // The plus button lives in the Presets header. If the top-right gear
    // overlaps that area, move the plus button immediately to the left of it.
    if (headerRect.right > gear.left && headerRect.left < gear.right &&
        headerRect.bottom > gear.top && headerRect.top < gear.bottom) {
        right = std::min(right, static_cast<int>(gear.left) - gearGap);
    }

    int left = right - buttonSize;
    if (left < r.left + leftMargin) {
        left = r.left + leftMargin;
        right = left + buttonSize;
    }

    const int top = r.top + 8;
    return RECT{left, top, right, top + buttonSize};
}

static RECT GetEqPresetDeleteRect(int customIndex) {
    RECT r = GetEqPresetRect(VIZ_EQ_BUILTIN_PRESET_COUNT + customIndex);
    r.right = std::min(static_cast<int>(r.right), static_cast<int>(r.left) + 20);
    r.bottom = std::min(static_cast<int>(r.bottom), static_cast<int>(r.top) + 20);
    return r;
}

static int HitTestEqPresetDelete(int x, int y) {
    if (!EqHasCustomPresets()) return -1;
    const double scale = EqGetPopupDpiScale();
    const POINT point{static_cast<int>(std::lround(static_cast<double>(x) / scale)),
                      static_cast<int>(std::lround(static_cast<double>(y) / scale))};
    for (int i = 0; i < g_eqCustomPresetCount; ++i) {
        const RECT r = GetEqPresetDeleteRect(i);
        if (PtInRect(&r, point)) return i;
    }
    return -1;
}

static int HitTestEqPreset(int x, int y) {
    const double scale = EqGetPopupDpiScale();
    const POINT point{static_cast<int>(std::lround(static_cast<double>(x) / scale)),
                      static_cast<int>(std::lround(static_cast<double>(y) / scale))};
    const RECT plus = GetEqPlusRect();
    if (PtInRect(&plus, point)) return EQ_PLUS_PRESET_HIT;
    for (int i = 0; i < EqGetPresetItemCount(); ++i) {
        const RECT r = GetEqPresetRect(i);
        if (PtInRect(&r, point)) return i;
    }
    return -1;
}

static RECT GetEqPageButtonRect() {
    const int right = std::max(40, g_eqPopupLogicalWidth - 8);
    const int left = std::max(0, right - 40);
    return RECT{left, EQ_PAGE_BUTTON_TOP, right, EQ_PAGE_BUTTON_BOTTOM};
}

static bool HitTestEqPageButton(int x, int y) {
    const double scale = EqGetPopupDpiScale();
    const int logicalX = static_cast<int>(std::lround(static_cast<double>(x) / scale));
    const int logicalY = static_cast<int>(std::lround(static_cast<double>(y) / scale));
    const RECT r = GetEqPageButtonRect();
    return PtInRect(&r, POINT{logicalX, logicalY}) != FALSE;
}

static double EqMediaPositionFromMouseX(int x) {
    const double scale = EqGetPopupDpiScale();
    const double logicalX = static_cast<double>(x) / scale;
    const RECT timeline = GetEqMediaTimelineRect();
    const double t = std::clamp(
        (logicalX - static_cast<double>(timeline.left)) /
            static_cast<double>(std::max(1, static_cast<int>(timeline.right - timeline.left))),
        0.0, 1.0);
    const EqMediaState state = GetEqMediaStateSnapshot();
    return state.durationSeconds > 0.0 ? state.durationSeconds * t : 0.0;
}

static float EqEaseInOutCubic(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t < 0.5f
        ? 4.0f * t * t * t
        : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

static void EqStopPageAnimation(HWND hwnd) {
    g_eqPageAnimationActive = false;
    g_eqPageAnimationStartTick = 0;
    g_eqPageAnimationStart = g_eqPageAnimationTarget;
    if (hwnd)
        KillTimer(hwnd, kEqPopupPageAnimationTimerId);
}

static RECT EqGetPresentLayoutBounds() {
    bool found = false;
    int minX = EQ_LAYOUT_CANVAS_W;
    int minY = EQ_LAYOUT_CANVAS_H;
    int maxX = 0;
    int maxY = 0;
    for (const auto& w : g_eqLayoutWidgets) {
        if (!w.present)
            continue;
        minX = std::min(minX, std::max(0, w.x));
        minY = std::min(minY, std::max(0, w.y));
        maxX = std::max(maxX, std::min(EQ_LAYOUT_CANVAS_W, w.x + w.w));
        maxY = std::max(maxY, std::min(EQ_LAYOUT_CANVAS_H, w.y + w.h));
        found = true;
    }
    if (!found || maxX <= minX || maxY <= minY)
        return RECT{0, 0, EQ_LAYOUT_CANVAS_W, EQ_LAYOUT_CANVAS_H};
    return RECT{minX, minY, maxX, maxY};
}

static void EqExpandPopupForLayoutBuilder(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return;
    const double scale = EqGetPopupDpiScale();
    RECT wr{};
    if (!GetWindowRect(hwnd, &wr))
        return;
    const int restoreX = wr.left - EqPopupScalePx(g_eqPopupContentOffsetX, scale);
    const int restoreY = wr.top - EqPopupScalePx(g_eqPopupContentOffsetY, scale);
    g_eqPopupContentOffsetX = 0;
    g_eqPopupContentOffsetY = 0;
    g_eqPopupLogicalWidth = EQ_POPUP_WIDTH;
    g_eqPopupLogicalHeight = EQ_POPUP_HEIGHT;
    SetWindowPos(hwnd, nullptr, restoreX, restoreY,
                 EqPopupScalePx(EQ_POPUP_WIDTH, scale),
                 EqPopupScalePx(EQ_POPUP_HEIGHT, scale),
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

static void EqCropPopupToLayout(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || g_eqPageAnimationTarget >= 0.5f)
        return;
    const RECT bounds = EqGetPresentLayoutBounds();
    const int newLogicalWidth = std::max(1L, bounds.right - bounds.left);
    const int newLogicalHeight = std::max(1L, bounds.bottom - bounds.top);
    const double scale = EqGetPopupDpiScale();
    RECT wr{};
    if (!GetWindowRect(hwnd, &wr))
        return;
    const int newX = wr.left + EqPopupScalePx(bounds.left, scale);
    const int newY = wr.top + EqPopupScalePx(bounds.top, scale);
    g_eqPopupContentOffsetX = bounds.left;
    g_eqPopupContentOffsetY = bounds.top;
    g_eqPopupLogicalWidth = newLogicalWidth;
    g_eqPopupLogicalHeight = newLogicalHeight;
    SetWindowPos(hwnd, nullptr, newX, newY,
                 EqPopupScalePx(newLogicalWidth, scale),
                 EqPopupScalePx(newLogicalHeight, scale),
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

static void EqStartPageAnimation(HWND hwnd, bool showSettings) {
    if (!hwnd || !IsWindow(hwnd))
        return;

    if (showSettings)
        EqExpandPopupForLayoutBuilder(hwnd);

    const float target = showSettings ? 1.0f : 0.0f;
    if (std::abs(g_eqPageAnimationProgress - target) < 0.001f) {
        g_eqPageAnimationProgress = target;
        g_eqPageAnimationStart = target;
        g_eqPageAnimationTarget = target;
        EqStopPageAnimation(hwnd);
        RenderEqPopup(hwnd);
        return;
    }

    g_eqPageAnimationStart = g_eqPageAnimationProgress;
    g_eqPageAnimationTarget = target;
    g_eqPageAnimationStartTick = GetTickCount64();
    if (showSettings) {
        g_eqSettingsScrollOffset = 0;
        g_eqSettingsScrollbarDragging = false;
        g_eqPopupPlacementDragTarget = EQ_POPUP_PLACEMENT_DRAG_NONE;
    } else {
        g_eqSettingsScrollOffset = 0;
    }
    g_eqPageAnimationActive = true;
    SetTimer(hwnd, kEqPopupPageAnimationTimerId,
             kEqPopupPageAnimationIntervalMs, nullptr);
    RenderEqPopup(hwnd);
}

static void EqApplyPageAnimationStep(HWND hwnd) {
    if (!g_eqPageAnimationActive) {
        KillTimer(hwnd, kEqPopupPageAnimationTimerId);
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG elapsed = now >= g_eqPageAnimationStartTick
        ? now - g_eqPageAnimationStartTick : 0;
    const float linear = std::clamp(
        static_cast<float>(elapsed) /
            static_cast<float>(kEqPopupPageAnimationDurationMs),
        0.0f, 1.0f);
    const float eased = EqEaseInOutCubic(linear);
    g_eqPageAnimationProgress = g_eqPageAnimationStart +
        (g_eqPageAnimationTarget - g_eqPageAnimationStart) * eased;

    if (linear >= 1.0f) {
        g_eqPageAnimationProgress = g_eqPageAnimationTarget;
        EqStopPageAnimation(hwnd);
        if (g_eqPageAnimationTarget < 0.5f)
            EqCropPopupToLayout(hwnd);
    }

    RenderEqPopup(hwnd);
}

static std::wstring FormatEqMediaTime(double seconds) {
    if (!IsFiniteDouble(seconds) || seconds < 0.0)
        seconds = 0.0;
    const long long total = static_cast<long long>(std::llround(seconds));
    const long long hours = total / 3600;
    const long long minutes = (total % 3600) / 60;
    const long long secs = total % 60;
    wchar_t buffer[32]{};
    if (hours > 0) {
        swprintf_s(buffer, L"%lld:%02lld:%02lld", hours, minutes, secs);
    } else {
        swprintf_s(buffer, L"%lld:%02lld", minutes, secs);
    }
    return buffer;
}

static float GetEqActiveGainAtBand(int index) {
    return std::clamp(
        g_eqActiveGains[static_cast<size_t>(index)].load(std::memory_order_relaxed),
        0.0f, 2.0f);
}

static float GetEqGainAtBand(int index) {
    return GetEqActiveGainAtBand(index);
}

static void EqStopPresetAnimation(HWND hwnd) {
    g_eqPresetAnimationActive = false;
    g_eqLastAnimationTick = 0;
    if (hwnd)
        KillTimer(hwnd, kEqPopupAnimationTimerId);
}

static void EqStartPresetAnimation(HWND hwnd, int presetIndex) {
    if (presetIndex < 0 || presetIndex >= EqGetPresetItemCount()) return;
    if (presetIndex < VIZ_EQ_BUILTIN_PRESET_COUNT) {
        g_eqSelectedPreset = presetIndex;
        for (int i = 0; i < VIZ_EQ_BANDS; ++i)
            g_eqPresetTargets[static_cast<size_t>(i)] = VIZ_EQ_PRESET_GAINS[static_cast<size_t>(presetIndex)][static_cast<size_t>(i)];
    } else {
        const int customIndex = presetIndex - VIZ_EQ_BUILTIN_PRESET_COUNT;
        if (customIndex < 0 || customIndex >= g_eqCustomPresetCount) return;
        g_eqSelectedPreset = EQ_CUSTOM_PRESET_INDEX_BASE + customIndex;
        for (int i = 0; i < VIZ_EQ_BANDS; ++i)
            g_eqPresetTargets[static_cast<size_t>(i)] = std::clamp(g_eqCustomPresetGains[static_cast<size_t>(customIndex)][static_cast<size_t>(i)].load(std::memory_order_relaxed), 0.0f, 2.0f);
    }
    Wh_SetIntValue(VIZ_EQ_SELECTED_PRESET_STORAGE_KEY, g_eqSelectedPreset);
    g_eqPresetAnimationActive = true;
    g_eqLastAnimationTick = GetTickCount64();
    if (hwnd) SetTimer(hwnd, kEqPopupAnimationTimerId, kEqPopupAnimationIntervalMs, nullptr);
}

static void EqCreateCustomPreset(HWND hwnd) {
    if (g_eqCustomPresetCount >= VIZ_EQ_MAX_CUSTOM_PRESETS) return;
    const int newIndex = g_eqCustomPresetCount++;
    for (int band = 0; band < VIZ_EQ_BANDS; ++band)
        g_eqCustomPresetGains[static_cast<size_t>(newIndex)][static_cast<size_t>(band)].store(
            std::clamp(g_eqActiveGains[static_cast<size_t>(band)].load(std::memory_order_relaxed), 0.0f, 2.0f),
            std::memory_order_relaxed);
    g_eqSelectedPreset = EQ_CUSTOM_PRESET_INDEX_BASE + newIndex;
    EqSaveCustomPreset(newIndex);
    EqSaveCustomPresetCount();
    SaveCustomEQSettings();
    g_eqHotPresetDelete = -1;
    if (hwnd) RenderEqPopup(hwnd);
}

static void EqDeleteCustomPreset(HWND hwnd, int customIndex) {
    if (customIndex < 0 || customIndex >= g_eqCustomPresetCount) return;
    EqStopPresetAnimation(hwnd);
    const int selectedCustom = g_eqSelectedPreset >= EQ_CUSTOM_PRESET_INDEX_BASE
        ? g_eqSelectedPreset - EQ_CUSTOM_PRESET_INDEX_BASE : -1;
    for (int i = customIndex; i + 1 < g_eqCustomPresetCount; ++i) {
        for (int band = 0; band < VIZ_EQ_BANDS; ++band)
            g_eqCustomPresetGains[static_cast<size_t>(i)][static_cast<size_t>(band)].store(
                g_eqCustomPresetGains[static_cast<size_t>(i + 1)][static_cast<size_t>(band)].load(std::memory_order_relaxed),
                std::memory_order_relaxed);
    }
    --g_eqCustomPresetCount;
    for (int band = 0; band < VIZ_EQ_BANDS; ++band)
        g_eqCustomPresetGains[static_cast<size_t>(g_eqCustomPresetCount)][static_cast<size_t>(band)].store(1.0f, std::memory_order_relaxed);
    if (selectedCustom == customIndex) g_eqSelectedPreset = -1;
    else if (selectedCustom > customIndex) g_eqSelectedPreset = EQ_CUSTOM_PRESET_INDEX_BASE + selectedCustom - 1;
    EqSaveAllCustomPresets();
    SaveCustomEQSettings();
    g_eqHotPresetDelete = -1;
    if (hwnd) RenderEqPopup(hwnd);
}

static void EqApplyPresetAnimationStep(HWND hwnd) {
    if (!g_eqPresetAnimationActive)
        return;

    const ULONGLONG now = GetTickCount64();
    ULONGLONG deltaMs = g_eqLastAnimationTick ? now - g_eqLastAnimationTick : 16;
    g_eqLastAnimationTick = now;
    deltaMs = std::clamp<ULONGLONG>(deltaMs, 1, 64);

    // A frame-rate-independent exponential approach gives the soft, Linux-like
    // "all knobs move together" transition without a fixed-duration jump.
    const float alpha = 1.0f - expf(-static_cast<float>(deltaMs) / 90.0f);
    bool finished = true;

    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const size_t index = static_cast<size_t>(i);
        const float current = GetEqActiveGainAtBand(i);
        const float target = g_eqPresetTargets[index];
        float next = current + (target - current) * alpha;
        if (fabsf(target - next) < 0.0025f)
            next = target;
        if (fabsf(target - next) >= 0.0025f)
            finished = false;
        g_eqActiveGains[index].store(std::clamp(next, 0.0f, 2.0f),
                                     std::memory_order_relaxed);
    }

    if (finished) {
        for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
            g_eqActiveGains[static_cast<size_t>(i)].store(
                g_eqPresetTargets[static_cast<size_t>(i)],
                std::memory_order_relaxed);
        }
        EqStopPresetAnimation(hwnd);
        SaveCustomEQSettings();
    }

    RenderEqPopup(hwnd);
    if (g_hwndOverlay)
        PostMessageW(g_hwndOverlay, WM_VIZ_AUDIO_WAKE, 0, 0);
}

static void EqBeginManualBandEdit(HWND hwnd) {
    // A manual edit only stops an in-progress preset transition. The selected
    // preset stays selected, and predefined preset values are never copied
    // into Custom. This means editing Vocal/Pop/etc. is temporary: selecting
    // that preset again restores its built-in curve.
    EqStopPresetAnimation(hwnd);
}

static void SetEqGainFromMouse(int index, int x, int y) {
    if (index < 0 || index >= VIZ_EQ_BANDS)
        return;

    const double scale = EqGetPopupDpiScale();
    const EqSliderMetrics metrics = EqGetSliderMetrics();
    const float logicalX = static_cast<float>(static_cast<double>(x) / scale);
    const float logicalY = static_cast<float>(static_cast<double>(y) / scale);
    float t = 0.0f;
    if (metrics.horizontal) {
        t = std::clamp(
            (logicalX - static_cast<float>(metrics.area.left)) /
                static_cast<float>(std::max(1L, metrics.area.right - metrics.area.left)),
            0.0f, 1.0f);
    } else {
        t = std::clamp(
            (logicalY - static_cast<float>(metrics.top)) /
                static_cast<float>(std::max(1, metrics.bottom - metrics.top)),
            0.0f, 1.0f);
    }
    const float gain = metrics.horizontal
        ? std::clamp(2.0f * t, 0.0f, 2.0f)
        : std::clamp(2.0f * (1.0f - t), 0.0f, 2.0f);
    g_eqActiveGains[static_cast<size_t>(index)].store(
        gain, std::memory_order_relaxed);
    if (g_eqSelectedPreset >= EQ_CUSTOM_PRESET_INDEX_BASE) {
        const int customIndex = g_eqSelectedPreset - EQ_CUSTOM_PRESET_INDEX_BASE;
        if (customIndex >= 0 && customIndex < g_eqCustomPresetCount)
            g_eqCustomPresetGains[static_cast<size_t>(customIndex)][static_cast<size_t>(index)].store(gain, std::memory_order_relaxed);
    } else {
        g_customEqGains[static_cast<size_t>(index)].store(gain, std::memory_order_relaxed);
    }
    g_eqPresetTargets[static_cast<size_t>(index)] = gain;
}

static void DrawEqRoundedPanel(
    Gdiplus::Graphics& graphics,
    const RECT& rect,
    bool dark,
    float radius = 12.0f) {
    const Gdiplus::Color panelColor(
        dark ? 235 : 246,
        dark ? 24 : 239,
        dark ? 24 : 239,
        dark ? 26 : 241);
    Gdiplus::SolidBrush panelBrush(panelColor);
    Gdiplus::GraphicsPath path;
    AddRoundedRectSubpath(path,
                          static_cast<float>(rect.left + 1),
                          static_cast<float>(rect.top + 1),
                          static_cast<float>(std::max(1, static_cast<int>(rect.right - rect.left) - 2)),
                          static_cast<float>(std::max(1, static_cast<int>(rect.bottom - rect.top) - 2)),
                          radius);
    graphics.FillPath(&panelBrush, &path);
}

static void DrawEqMediaWidget(
    Gdiplus::Graphics& graphics,
    const RECT& r,
    bool dark,
    const EqMediaState& media,
    int rotation,
    bool preview) {
    if (r.right <= r.left || r.bottom <= r.top)
        return;

    DrawEqRoundedPanel(graphics, r, dark, std::min(14.0f, std::max(4.0f, static_cast<float>(r.bottom-r.top) * 0.10f)));

    const Gdiplus::Color titleColor(255, dark ? 245 : 30, dark ? 245 : 30, dark ? 248 : 30);
    const Gdiplus::Color mutedColor(205, dark ? 205 : 90, dark ? 205 : 90, dark ? 210 : 95);
    const Gdiplus::Color trackColor(105, dark ? 150 : 125, dark ? 150 : 125, dark ? 155 : 130);
    const Gdiplus::Color accentColor(255, 105, 165, 255);

    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold");
    Gdiplus::FontFamily smallFamily(L"Segoe UI");
    Gdiplus::FontFamily symbolFamily(L"Segoe UI Symbol");
    const float titleSize = std::clamp((r.bottom-r.top) * 0.17f, 11.0f, 16.0f);
    const float smallSize = std::clamp((r.bottom-r.top) * 0.13f, 9.0f, 13.0f);
    Gdiplus::Font titleFont(&titleFamily, titleSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font artistFont(&smallFamily, smallSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font timeFont(&smallFamily, 9.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font controlFont(&symbolFamily, 15.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush titleBrush(titleColor);
    Gdiplus::SolidBrush mutedBrush(mutedColor);

    std::wstring mediaTitle = media.hasSession && !media.title.empty() ? media.title : L"No media playing";
    std::wstring mediaArtist = media.hasSession && !media.artist.empty() ? L"By " + media.artist : L"By —";
    const bool vertical = rotation == 90 || rotation == 270 || (r.bottom-r.top) > (r.right-r.left);

    if (vertical) {
        const int width = std::max(1, static_cast<int>(r.right-r.left));
        const int textTop = static_cast<int>(r.top) + 10;
        const int textWidth = std::max(80, width - 24);
        Gdiplus::StringFormat noWrap;
        noWrap.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        noWrap.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        graphics.DrawString(mediaTitle.c_str(), -1, &titleFont,
                            Gdiplus::RectF((float)r.left+12.0f,
                                           (float)textTop,
                                           (float)textWidth,
                                           titleSize+8.0f),
                            &noWrap, &titleBrush);
        graphics.DrawString(mediaArtist.c_str(), -1, &artistFont,
                            Gdiplus::RectF((float)r.left+12.0f,
                                           (float)textTop + titleSize + 4.0f,
                                           (float)textWidth,
                                           smallSize + 6.0f),
                            &noWrap, &mutedBrush);
    } else {
        const int left = r.left + 16;
        const int top = r.top + 9;
        const int sourceLeft = r.right - 150;
        const int titleW = std::max(60, sourceLeft - left - 10);
        Gdiplus::StringFormat noWrap;
        noWrap.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        noWrap.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        graphics.DrawString(mediaTitle.c_str(), -1, &titleFont,
                            Gdiplus::RectF((float)left, (float)top, (float)titleW, titleSize+8.0f),
                            &noWrap, &titleBrush);
        graphics.DrawString(mediaArtist.c_str(), -1, &artistFont,
                            Gdiplus::PointF((float)left, (float)top+titleSize+4.0f), &mutedBrush);
    }

    const EqLayoutWidgetState temporary{EQ_LAYOUT_MEDIA, r.left, r.top, r.right-r.left, r.bottom-r.top, rotation, true};
    const EqMediaMetrics metrics = EqBuildMediaMetricsForRect(
        r, rotation == 90 || rotation == 270 || temporary.h > temporary.w);

    const double mediaPosition = g_eqMediaSeeking && !preview ? g_eqMediaSeekPreviewSeconds : GetEqMediaPosition();
    const double duration = std::max(0.0, media.durationSeconds);
    const float progress = duration > 0.0 ? (float)std::clamp(mediaPosition/duration,0.0,1.0) : 0.0f;
    const float timelineY = (float)(metrics.timeline.top + metrics.timeline.bottom) * 0.5f;
    const float left = (float)metrics.timeline.left;
    const float right = (float)metrics.timeline.right;
    Gdiplus::Pen timelinePen(trackColor, 4.0f);
    timelinePen.SetStartCap(Gdiplus::LineCapRound); timelinePen.SetEndCap(Gdiplus::LineCapRound);
    graphics.DrawLine(&timelinePen,left,timelineY,right,timelineY);
    Gdiplus::Pen timelineFill(accentColor,4.0f);
    timelineFill.SetStartCap(Gdiplus::LineCapRound); timelineFill.SetEndCap(Gdiplus::LineCapRound);
    const float progressX=left+(right-left)*progress;
    graphics.DrawLine(&timelineFill,left,timelineY,progressX,timelineY);
    if (!vertical) {
        Gdiplus::SolidBrush timeBrush(titleColor);
        graphics.DrawString(FormatEqMediaTime(mediaPosition).c_str(), -1, &timeFont,
                            Gdiplus::PointF((float)r.left+12.0f,(float)r.top+64.0f), &timeBrush);
        Gdiplus::StringFormat durationFormat;
        durationFormat.SetAlignment(Gdiplus::StringAlignmentFar);
        durationFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        graphics.DrawString(FormatEqMediaTime(duration).c_str(), -1, &timeFont,
                            Gdiplus::RectF((float)r.right-62.0f,(float)r.top+64.0f,50.0f,16.0f), &durationFormat, &timeBrush);
    }

    const wchar_t* glyphs[5]={L"\x23EE",L"-5",L"\x23B6",L"+5",L"\x23ED"};
    glyphs[2]=media.playing?L"\x23F8":L"\x25B6";
    for(int i=0;i<5;i++) {
        const RECT br=metrics.buttons[(size_t)i];
        Gdiplus::SolidBrush buttonBrush(Gdiplus::Color(dark?235:245,dark?47:236,dark?47:236,dark?50:238));
        Gdiplus::GraphicsPath path;
        AddRoundedRectSubpath(path,(float)br.left,(float)br.top,(float)(br.right-br.left),(float)(br.bottom-br.top),8.0f);
        graphics.FillPath(&buttonBrush,&path);
        Gdiplus::StringFormat centered;
        centered.SetAlignment(Gdiplus::StringAlignmentCenter); centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::SolidBrush glyphBrush(titleColor);
        graphics.DrawString(glyphs[i],-1,&controlFont,Gdiplus::RectF((float)br.left,(float)br.top,(float)(br.right-br.left),(float)(br.bottom-br.top)),&centered,&glyphBrush);
    }
}

static void DrawEqPresetWidget(
    Gdiplus::Graphics& graphics,
    const RECT& r,
    bool dark,
    int rotation,
    bool preview) {
    if (r.right <= r.left || r.bottom <= r.top) return;
    DrawEqRoundedPanel(graphics, r, dark, 12.0f);
    const Gdiplus::Color titleColor(255, dark?245:30, dark?245:30, dark?248:30);
    const Gdiplus::Color selectedColor(105,165,255,255);
    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold");
    Gdiplus::FontFamily smallFamily(L"Segoe UI");
    Gdiplus::Font headerFont(&titleFamily,13.0f,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    const int panelW = static_cast<int>(r.right - r.left);
    const int panelH = static_cast<int>(r.bottom - r.top);
    const EqLayoutWidgetState layoutWidget{
        EQ_LAYOUT_PRESETS, r.left, r.top, panelW, panelH, rotation, true};
    const EqPresetGridMetrics grid = EqGetPresetGridMetrics(layoutWidget);
    const int cols = grid.cols;
    const int rowH = grid.rowHeight;
    Gdiplus::SolidBrush headerBrush(titleColor);
    graphics.DrawString(L"Presets",-1,&headerFont,Gdiplus::PointF((float)r.left+12.0f,(float)r.top+11.0f),&headerBrush);
    for(int item=0;item<EqGetPresetItemCount();++item){
        const int col=item%cols, row=item/cols;
        const int gap = grid.gap;
        const int cellW = grid.cellWidth;
        const RECT cell{r.left + 10 + col * (cellW + gap),
                        r.top + grid.contentTop + row * rowH,
                        r.left + 10 + col * (cellW + gap) + cellW,
                        std::min<int>(static_cast<int>(r.bottom) - 8,
                                      static_cast<int>(r.top + grid.contentTop + row * rowH + rowH - 4))};
        const bool selected = item<VIZ_EQ_BUILTIN_PRESET_COUNT ? g_eqSelectedPreset==item : g_eqSelectedPreset==EQ_CUSTOM_PRESET_INDEX_BASE+item-VIZ_EQ_BUILTIN_PRESET_COUNT;
        const BYTE alpha = selected?70:24;
        Gdiplus::SolidBrush cellBrush(Gdiplus::Color(alpha, selected?105:255, selected?165:255, selected?255:255));
        Gdiplus::GraphicsPath path; AddRoundedRectSubpath(path,(float)cell.left,(float)cell.top,(float)(cell.right-cell.left),(float)(cell.bottom-cell.top),7.0f); graphics.FillPath(&cellBrush,&path);
        wchar_t customName[32]{}; const wchar_t* name=nullptr; int customIndex=-1;
        if(item<VIZ_EQ_BUILTIN_PRESET_COUNT) name=VIZ_EQ_PRESET_NAMES[(size_t)item];
        else {customIndex=item-VIZ_EQ_BUILTIN_PRESET_COUNT; swprintf_s(customName,L"Custom %d",customIndex+1); name=customName;}
        Gdiplus::Font itemFont(&smallFamily,std::clamp((float)(cell.bottom-cell.top)*0.46f,9.0f,11.0f),Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
        Gdiplus::SolidBrush textBrush(selected?selectedColor:titleColor);
        Gdiplus::StringFormat centered; centered.SetAlignment(Gdiplus::StringAlignmentCenter); centered.SetLineAlignment(Gdiplus::StringAlignmentCenter); centered.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        graphics.DrawString(name,-1,&itemFont,Gdiplus::RectF((float)cell.left,(float)cell.top,(float)(cell.right-cell.left),(float)(cell.bottom-cell.top)),&centered,&textBrush);
        if(!preview && customIndex>=0 && (g_eqHotPreset==item || g_eqHotPresetDelete==customIndex)){
            const RECT dr=GetEqPresetDeleteRect(customIndex);
            Gdiplus::SolidBrush db(Gdiplus::Color(235,220,45,45));
            graphics.FillEllipse(&db, (Gdiplus::REAL)dr.left, (Gdiplus::REAL)dr.top, 18.0f, 18.0f);
            Gdiplus::Pen xp(Gdiplus::Color(255,255,255,255),2.0f);
            xp.SetStartCap(Gdiplus::LineCapRound); xp.SetEndCap(Gdiplus::LineCapRound);
            const Gdiplus::REAL cx=(Gdiplus::REAL)dr.left+9.0f;
            const Gdiplus::REAL cy=(Gdiplus::REAL)dr.top+9.0f;
            graphics.DrawLine(&xp,cx-4.0f,cy-4.0f,cx+4.0f,cy+4.0f);
            graphics.DrawLine(&xp,cx+4.0f,cy-4.0f,cx-4.0f,cy+4.0f);
        }
    }
    if(!preview && g_eqCustomPresetCount<VIZ_EQ_MAX_CUSTOM_PRESETS){
        const RECT plus=GetEqPlusRect();
        Gdiplus::SolidBrush pb(Gdiplus::Color(
            g_eqHotPreset==EQ_PLUS_PRESET_HIT ? 50 : 24, 255, 255, 255));
        Gdiplus::GraphicsPath pp;
        AddRoundedRectSubpath(pp,(float)plus.left,(float)plus.top,
                              (float)(plus.right-plus.left),
                              (float)(plus.bottom-plus.top),7.0f);
        graphics.FillPath(&pb,&pp);
        Gdiplus::Pen pen(titleColor,2.0f);
        const Gdiplus::REAL cx=(Gdiplus::REAL)(plus.left+plus.right)*0.5f;
        const Gdiplus::REAL cy=(Gdiplus::REAL)(plus.top+plus.bottom)*0.5f;
        graphics.DrawLine(&pen,cx-6.0f,cy,cx+6.0f,cy);
        graphics.DrawLine(&pen,cx,cy-6.0f,cx,cy+6.0f);
    }
}

struct EqLyricsMetrics {
    RECT content{};
    RECT scrollbarTrack{};
    int lineHeight = 28;
    int contentHeight = 0;
    int maxScroll = 0;
    std::vector<int> lineHeights;
};

static int EstimateEqLyricsWrappedLineCount(const std::wstring& text, int width) {
    if (text.empty())
        return 1;

    // The section uses a fixed 12 px Segoe UI font. This conservative estimate
    // intentionally gives long rows a little more height than the average
    // measured glyph width, preventing wrapped lines from being clipped.
    constexpr double kAverageGlyphWidthPx = 6.8;
    const int safeWidth = std::max(24, width - 8);
    const int charsPerLine = std::max(1, static_cast<int>(std::floor(
        static_cast<double>(safeWidth) / kAverageGlyphWidthPx)));
    return std::max(1, static_cast<int>((text.size() +
                                         static_cast<size_t>(charsPerLine) - 1) /
                                        static_cast<size_t>(charsPerLine)));
}

static EqLyricsMetrics GetEqLyricsMetrics(
    const EqLayoutWidgetState& widget,
    const std::vector<LyricsLine>* lines) {
    EqLyricsMetrics m{};
    const RECT r = EqGetLayoutRect(widget.id);
    constexpr int headerHeight = 12;
    constexpr int bottomPadding = 12;
    constexpr int scrollbarWidth = 8;
    constexpr int scrollbarGap = 8;
    constexpr int baseRowHeight = 28;
    constexpr int wrappedTextLineHeight = 18;
    constexpr int rowVerticalPadding = 10;
    m.content.left = r.left + 16;
    m.content.top = r.top + headerHeight;
    m.content.right = std::max(m.content.left + 1,
                               r.right - 16 - scrollbarWidth - scrollbarGap);
    m.content.bottom = std::max(m.content.top + 1,
                                r.bottom - bottomPadding);

    const size_t lineCount = lines ? lines->size() : 0;
    m.lineHeights.reserve(lineCount);
    long long totalHeight = 0;
    const int textWidth = std::max(24L, m.content.right - m.content.left);
    for (size_t i = 0; i < lineCount; ++i) {
        const int wrappedLines = EstimateEqLyricsWrappedLineCount(
            (*lines)[i].text, textWidth);
        const int height = std::max(
            baseRowHeight,
            wrappedLines * wrappedTextLineHeight + rowVerticalPadding);
        m.lineHeights.push_back(height);
        totalHeight += height;
        if (totalHeight >= INT_MAX) {
            totalHeight = INT_MAX;
            break;
        }
    }
    m.contentHeight = static_cast<int>(std::min<long long>(totalHeight, INT_MAX));
    if (!m.lineHeights.empty())
        m.lineHeight = m.lineHeights.front();

    const int viewportHeight = std::max(1L, m.content.bottom - m.content.top);
    m.maxScroll = std::max(0, m.contentHeight - viewportHeight);
    m.scrollbarTrack = RECT{
        r.right - 16 - scrollbarWidth,
        m.content.top,
        r.right - 16,
        m.content.bottom};
    return m;
}

static int GetEqLyricsLineTop(const EqLyricsMetrics& metrics, size_t index) {
    if (index >= metrics.lineHeights.size())
        return metrics.content.top;
    int y = metrics.content.top;
    for (size_t i = 0; i < index; ++i)
        y += metrics.lineHeights[i];
    return y;
}

static void PauseEqLyricsAutoFocus() {
    const ULONGLONG now = GetTickCount64();
    g_eqLyricsAutoFocusResumeTick = now + kEqLyricsAutoFocusPauseMs;
}

static bool EqLyricsAutoFocusPaused() {
    return GetTickCount64() < g_eqLyricsAutoFocusResumeTick;
}

static void MaybeAutoFocusEqLyrics(const EqLayoutWidgetState& widget,
                                    const std::vector<LyricsLine>& lines,
                                    const EqLyricsMetrics& metrics,
                                    int activeIndex) {
    if (activeIndex < 0 || static_cast<size_t>(activeIndex) >= lines.size() ||
        metrics.lineHeights.size() != lines.size() ||
        EqLyricsAutoFocusPaused()) {
        return;
    }

    const int viewportHeight =
        std::max(1L, metrics.content.bottom - metrics.content.top);
    const int lineTop = GetEqLyricsLineTop(metrics, static_cast<size_t>(activeIndex));
    const int lineHeight = metrics.lineHeights[static_cast<size_t>(activeIndex)];
    const int lineCenter = lineTop + lineHeight / 2;
    const int targetOffset = lineCenter - metrics.content.top - viewportHeight / 2;
    const int clampedTarget = std::clamp(targetOffset, 0, metrics.maxScroll);

    // The argument is intentionally kept in the helper signature because the
    // widget's geometry is the source of truth for focus placement.
    (void)widget;
    g_eqLyricsScrollOffset = clampedTarget;
}

static int GetEqLyricsLineAtPoint(const EqLayoutWidgetState& widget,
                                  int x, int y) {
    std::shared_ptr<const std::vector<LyricsLine>> lines;
    {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        lines = g_lyricsLines;
    }
    if (!lines || lines->empty())
        return -1;

    const EqLyricsMetrics m = GetEqLyricsMetrics(widget, lines.get());
    if (!PtInRect(&m.content, POINT{x, y}))
        return -1;

    const int contentY = y - m.content.top + g_eqLyricsScrollOffset;
    int cursor = 0;
    for (size_t i = 0; i < m.lineHeights.size(); ++i) {
        const int next = cursor + m.lineHeights[i];
        if (contentY >= cursor && contentY < next)
            return static_cast<int>(i);
        cursor = next;
    }
    return -1;
}

static void ScrollEqLyricsByPixels(const EqLayoutWidgetState& widget,
                                   int delta) {
    std::shared_ptr<const std::vector<LyricsLine>> lines;
    {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        lines = g_lyricsLines;
    }
    const EqLyricsMetrics m = GetEqLyricsMetrics(widget, lines.get());
    g_eqLyricsScrollOffset = std::clamp(
        g_eqLyricsScrollOffset + delta, 0, m.maxScroll);
    if (delta != 0)
        PauseEqLyricsAutoFocus();
}

static void SeekToEqLyricsLine(int index) {
    std::shared_ptr<const std::vector<LyricsLine>> lines;
    bool synced = false;
    {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        lines = g_lyricsLines;
        synced = g_lyricsHasSynced;
    }
    if (!synced || !lines || index < 0 ||
        static_cast<size_t>(index) >= lines->size())
        return;

    const double position = (*lines)[static_cast<size_t>(index)].timeSeconds;
    if (!IsFiniteDouble(position) || position < 0.0)
        return;

    // Reuse the existing GSMTC command worker. Its Seek command calls
    // TryChangePlaybackPositionAsync with the position converted to ticks.
    EnqueueEqMediaCommand(EqMediaCommandType::Seek, position);
}

static RECT GetEqLyricsScrollbarThumb(const EqLyricsMetrics& m) {
    const int trackHeight = std::max(1L,
        m.scrollbarTrack.bottom - m.scrollbarTrack.top);
    if (m.maxScroll <= 0)
        return m.scrollbarTrack;
    const int thumbHeight = std::clamp(
        static_cast<int>(std::lround(
            static_cast<double>(trackHeight) *
            std::min(1.0, static_cast<double>(trackHeight) /
                             std::max(1, m.contentHeight)))),
        28, trackHeight);
    const int travel = std::max(0, trackHeight - thumbHeight);
    const int top = m.scrollbarTrack.top +
        (travel > 0 ? static_cast<int>(std::lround(
            static_cast<double>(travel) * g_eqLyricsScrollOffset /
            std::max(1, m.maxScroll))) : 0);
    return RECT{m.scrollbarTrack.left, top,
                m.scrollbarTrack.right, top + thumbHeight};
}

static void DrawEqLyricsWidget(Gdiplus::Graphics& graphics,
                               const EqLayoutWidgetState& widget,
                               bool dark, bool preview) {
    const RECT r = EqGetLayoutRect(widget.id);
    if (r.right <= r.left || r.bottom <= r.top)
        return;

    DrawEqRoundedPanel(graphics, r, dark, 12.0f);

    const Gdiplus::Color titleColor(255, dark ? 245 : 30, dark ? 245 : 30,
                                    dark ? 248 : 30);
    const Gdiplus::Color lineColor(235, dark ? 232 : 30, dark ? 232 : 30,
                                   dark ? 235 : 32);
    const Gdiplus::Color activeColor(255, 105, 165, 255);
    const Gdiplus::Color hoverColor(48, 105, 165, 255);
    const Gdiplus::Color activeFill(54, 105, 165, 255);

    // Header / track metadata intentionally disabled for the Layout Builder's
    // Lyrics section. The section should contain only lyric text.
    /*
    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold");
    Gdiplus::FontFamily bodyFamily(L"Segoe UI");
    Gdiplus::Font headerFont(&titleFamily, 13.0f,
                            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font metaFont(&bodyFamily, 9.0f,
                           Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    */
    Gdiplus::FontFamily bodyFamily(L"Segoe UI");
    Gdiplus::Font lineFont(&bodyFamily, 12.0f,
                           Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    // Title / artist / track metadata are intentionally disabled here.
    /*
    Gdiplus::SolidBrush titleBrush(titleColor);
    Gdiplus::SolidBrush mutedBrush(mutedColor);
    */
    Gdiplus::SolidBrush lineBrush(lineColor);
    Gdiplus::SolidBrush activeBrush(activeColor);

    std::shared_ptr<const std::vector<LyricsLine>> lines;
    std::wstring trackKey;
    bool available = false;
    bool synced = false;
    {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        lines = g_lyricsLines;
        trackKey = g_lyricsTrackKey;
        available = g_lyricsAvailable;
        synced = g_lyricsHasSynced;
    }

    if (trackKey != g_eqLyricsScrollTrackKey) {
        g_eqLyricsScrollTrackKey = trackKey;
        g_eqLyricsScrollOffset = 0;
        g_eqHotLyricsLine = -1;
        g_eqLyricsAutoFocusResumeTick = 0;
    }

    const size_t lineCount = lines ? lines->size() : 0;
    const EqLyricsMetrics metrics = GetEqLyricsMetrics(widget, lines.get());
    g_eqLyricsScrollOffset = std::clamp(g_eqLyricsScrollOffset, 0,
                                        metrics.maxScroll);

    // Track title / artist display intentionally disabled.
    /*
    std::wstring meta = artist;
    if (!artist.empty() && !title.empty())
        meta += L"  •  ";
    meta += title;
    if (meta.empty())
        meta = L"No current track";
    graphics.DrawString(meta.c_str(), -1, &metaFont,
                        Gdiplus::RectF((float)r.left + 12.0f,
                                       (float)r.top + 27.0f,
                                       (float)(r.right - r.left - 28),
                                       16.0f),
                        nullptr, &mutedBrush);
    */

    if (preview) {
        for (int i = 0; i < 5; ++i) {
            const int y = metrics.content.top + i * metrics.lineHeight;
            if (y + 20 > metrics.content.bottom)
                break;
            graphics.DrawString(
                i == 2 ? L"Current lyric line" : L"Lyric preview line", -1,
                &lineFont,
                Gdiplus::RectF((float)metrics.content.left, (float)y,
                               (float)(metrics.content.right - metrics.content.left),
                               22.0f), nullptr,
                i == 2 ? &activeBrush : &lineBrush);
        }
        return;
    }

    if (!available || lineCount == 0) {
        const int cardW = std::max<int>(140,
            metrics.content.right - metrics.content.left - 8);
        const int cardH = 70;
        const int cardX = metrics.content.left + 4;
        const int cardY = metrics.content.top +
            std::max<int>(0, (metrics.content.bottom - metrics.content.top - cardH) / 2);
        Gdiplus::GraphicsPath cardPath;
        AddRoundedRectSubpath(cardPath, (float)cardX, (float)cardY,
                              (float)cardW, (float)cardH, 10.0f);
        Gdiplus::SolidBrush cardBrush(
            Gdiplus::Color(dark ? 26 : 18, dark ? 255 : 80,
                           dark ? 255 : 80, dark ? 255 : 85));
        graphics.FillPath(&cardBrush, &cardPath);
        Gdiplus::Font unavailableFont(
            &bodyFamily, 12.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::StringFormat centered;
        centered.SetAlignment(Gdiplus::StringAlignmentCenter);
        centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::SolidBrush unavailableBrush(titleColor);
        graphics.DrawString(L"Lyrics unavailable", -1, &unavailableFont,
                            Gdiplus::RectF((float)cardX, (float)cardY,
                                           (float)cardW, (float)cardH),
                            &centered, &unavailableBrush);
        return;
    }

    const int activeIndex = synced
        ? GetCurrentLyricsLineIndex(*lines, GetCurrentLyricsPosition())
        : -1;

    // Follow the playback focus line automatically, unless the user has
    // manually scrolled recently. The manual-scroll deadline is refreshed by
    // every wheel/scrollbar movement and expires 3.5 s after the last action.
    MaybeAutoFocusEqLyrics(widget, *lines, metrics, activeIndex);

    const Gdiplus::GraphicsState clipState = graphics.Save();
    graphics.SetClip(
        Gdiplus::Rect(metrics.content.left, metrics.content.top,
                      std::max(1L, metrics.content.right - metrics.content.left),
                      std::max(1L, metrics.content.bottom - metrics.content.top)),
        Gdiplus::CombineModeIntersect);

    for (size_t i = 0; i < lineCount; ++i) {
        const int lineTop = GetEqLyricsLineTop(metrics, i);
        const int lineHeight = metrics.lineHeights[i];
        const int y = lineTop - g_eqLyricsScrollOffset;
        if (y + lineHeight < metrics.content.top)
            continue;
        if (y > metrics.content.bottom)
            break;

        const bool active = static_cast<int>(i) == activeIndex;
        const bool hovered = static_cast<int>(i) == g_eqHotLyricsLine;
        if (active || hovered) {
            Gdiplus::GraphicsPath rowPath;
            AddRoundedRectSubpath(
                rowPath, (float)metrics.content.left, (float)y + 2.0f,
                (float)(metrics.content.right - metrics.content.left),
                (float)lineHeight - 4.0f, 7.0f);
            Gdiplus::SolidBrush rowBrush(active ? activeFill : hoverColor);
            graphics.FillPath(&rowBrush, &rowPath);
        }

        // Timestamps are intentionally hidden. The entire lyric row remains
        // clickable, so click-to-seek continues to use its parsed LRC time.
        Gdiplus::Brush* currentBrush = active
            ? static_cast<Gdiplus::Brush*>(&activeBrush)
            : static_cast<Gdiplus::Brush*>(&lineBrush);

        // Use the complete row height as the DrawString layout box. GDI+ then
        // wraps long lyric text naturally instead of clipping the second line.
        Gdiplus::StringFormat lyricsFormat;
        lyricsFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
        lyricsFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
        lyricsFormat.SetTrimming(Gdiplus::StringTrimmingNone);
        graphics.DrawString(
            (*lines)[i].text.c_str(), -1, &lineFont,
            Gdiplus::RectF((float)metrics.content.left + 4.0f,
                           (float)y + 3.0f,
                           (float)(metrics.content.right - metrics.content.left - 8),
                           (float)std::max(1, lineHeight - 6)),
            &lyricsFormat, currentBrush);
    }

    graphics.Restore(clipState);

    if (metrics.maxScroll > 0) {
        Gdiplus::GraphicsPath trackPath;
        AddRoundedRectSubpath(
            trackPath, (float)metrics.scrollbarTrack.left,
            (float)metrics.scrollbarTrack.top,
            (float)(metrics.scrollbarTrack.right - metrics.scrollbarTrack.left),
            (float)(metrics.scrollbarTrack.bottom - metrics.scrollbarTrack.top),
            4.0f);
        Gdiplus::SolidBrush trackBrush(
            Gdiplus::Color(dark ? 40 : 28, dark ? 160 : 100,
                           dark ? 160 : 100, dark ? 165 : 105));
        graphics.FillPath(&trackBrush, &trackPath);

        const RECT thumb = GetEqLyricsScrollbarThumb(metrics);
        Gdiplus::GraphicsPath thumbPath;
        AddRoundedRectSubpath(
            thumbPath, (float)thumb.left, (float)thumb.top,
            (float)(thumb.right - thumb.left),
            (float)(thumb.bottom - thumb.top), 4.0f);
        Gdiplus::SolidBrush thumbBrush(
            g_eqLyricsScrollbarDragging
                ? Gdiplus::Color(255, 105, 165, 255)
                : Gdiplus::Color(dark ? 150 : 100, dark ? 205 : 120,
                                 dark ? 205 : 120, dark ? 210 : 125));
        graphics.FillPath(&thumbBrush, &thumbPath);
    }

    // Footer instruction intentionally disabled: the section contains only
    // the lyrics themselves (plus the unavailable-state plate).
    /*
    if (synced) {
        Gdiplus::Font footerFont(&bodyFamily, 8.5f,
                                 Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        graphics.DrawString(
            L"Click a line to seek", -1, &footerFont,
            Gdiplus::RectF((float)r.left + 12.0f,
                           (float)r.bottom - 17.0f,
                           (float)(r.right - r.left - 28), 12.0f),
            nullptr, &mutedBrush);
    }
    */
}

static void DrawEqWidget(
    Gdiplus::Graphics& graphics,
    const EqLayoutWidgetState& widget,
    bool dark,
    const EqMediaState& media,
    bool preview = false) {
    const RECT r = EqGetLayoutRect(widget.id);
    if (r.right <= r.left || r.bottom <= r.top) return;
    if (widget.id == EQ_LAYOUT_MEDIA) {
        DrawEqMediaWidget(graphics,r,dark,media,widget.rotation,preview);
        return;
    }
    if (widget.id == EQ_LAYOUT_PRESETS) {
        DrawEqPresetWidget(graphics,r,dark,widget.rotation,preview);
        return;
    }
    if (widget.id == EQ_LAYOUT_LYRICS) {
        DrawEqLyricsWidget(graphics, widget, dark, preview);
        return;
    }

    DrawEqRoundedPanel(graphics,r,dark,12.0f);
    const Gdiplus::Color titleColor(255,dark?245:30,dark?245:30,dark?248:30);
    const Gdiplus::Color trackColor(100,dark?150:125,dark?150:125,dark?155:130);
    const Gdiplus::Color accentColor(255,105,165,255);
    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold");
    Gdiplus::FontFamily smallFamily(L"Segoe UI");
    Gdiplus::Font headerFont(&titleFamily,13.0f,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    Gdiplus::Font valueFont(&smallFamily,8.5f,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    Gdiplus::SolidBrush headerBrush(titleColor);
    graphics.DrawString(L"Equalizer",-1,&headerFont,
                        Gdiplus::PointF((float)r.left+12.0f,(float)r.top+10.0f),
                        &headerBrush);

    const bool rotated = widget.rotation == 90 || widget.rotation == 270;
    if (rotated) {
        // A rotated section is rendered adaptively rather than rotating text.
        // Ten EQ bands become horizontal sliders, keeping all labels readable.
        const int left = static_cast<int>(r.left) + 54;
        const int right = std::max(left + 30, static_cast<int>(r.right) - 52);
        const int valueLeft = std::min(right + 6, static_cast<int>(r.right) - 42);
        const int top = static_cast<int>(r.top) + 38;
        const int bottom = static_cast<int>(r.bottom) - 18;
        const int usableH = std::max(10, bottom - top);
        const float rowStep = static_cast<float>(usableH) /
                              static_cast<float>(std::max(1, VIZ_EQ_BANDS - 1));
        Gdiplus::Pen trackPen(trackColor,4.0f);
        trackPen.SetStartCap(Gdiplus::LineCapRound);
        trackPen.SetEndCap(Gdiplus::LineCapRound);
        Gdiplus::Pen fillPen(accentColor,4.0f);
        fillPen.SetStartCap(Gdiplus::LineCapRound);
        fillPen.SetEndCap(Gdiplus::LineCapRound);
        for (int i=0;i<VIZ_EQ_BANDS;++i) {
            const float y = static_cast<float>(top) + rowStep * static_cast<float>(i);
            const float gain = GetEqGainAtBand(i);
            const float knobX = static_cast<float>(left) +
                (gain / 2.0f) * static_cast<float>(std::max(1, right-left));
            graphics.DrawLine(&trackPen,(float)left,y,(float)right,y);
            graphics.DrawLine(&fillPen,(float)left,y,knobX,y);
            Gdiplus::SolidBrush knobBrush(accentColor);
            graphics.FillEllipse(&knobBrush,
                (Gdiplus::REAL)(knobX-6.0f),(Gdiplus::REAL)(y-6.0f),
                12.0f,12.0f);
            if (!preview && (i==g_eqHotBand || i==g_eqDraggingBand)) {
                wchar_t txt[16]{};
                swprintf_s(txt,L"%.2f",gain);
                Gdiplus::StringFormat centered;
                centered.SetAlignment(Gdiplus::StringAlignmentCenter);
                centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                centered.SetAlignment(Gdiplus::StringAlignmentNear);
                graphics.DrawString(txt,-1,&valueFont,
                    Gdiplus::RectF((float)valueLeft,y-9.0f,38.0f,18.0f),
                    &centered,&headerBrush);
            }
        }
        return;
    }

    const int left = static_cast<int>(r.left) + 20;
    const int right = static_cast<int>(r.right) - 20;
    const int top = static_cast<int>(r.top) + 46;
    const int bottom = std::max<int>(top + 1, static_cast<int>(r.bottom) - 42);
    const float span=(float)std::max(1,right-left);
    Gdiplus::Pen trackPen(trackColor,4);
    trackPen.SetStartCap(Gdiplus::LineCapRound);
    trackPen.SetEndCap(Gdiplus::LineCapRound);
    Gdiplus::Pen fillPen(accentColor,4);
    fillPen.SetStartCap(Gdiplus::LineCapRound);
    fillPen.SetEndCap(Gdiplus::LineCapRound);
    for(int i=0;i<VIZ_EQ_BANDS;++i){
        const float x=left+span*(float)i/(float)std::max(1,VIZ_EQ_BANDS-1);
        graphics.DrawLine(&trackPen,x,(float)top,x,(float)bottom);
        const float gain=GetEqGainAtBand(i);
        const float knobY=bottom-(gain/2.0f)*(bottom-top);
        graphics.DrawLine(&fillPen,x,knobY,x,(float)bottom);
        Gdiplus::SolidBrush knobBrush(accentColor);
        graphics.FillEllipse(&knobBrush,
            (Gdiplus::REAL)(x - 6.0f),(Gdiplus::REAL)(knobY - 6.0f),12.0f,12.0f);
        if(!preview && (i==g_eqHotBand || i==g_eqDraggingBand)){
            wchar_t txt[16]{}; swprintf_s(txt,L"%.2f",gain);
            Gdiplus::SolidBrush b(titleColor);
            Gdiplus::StringFormat c;
            c.SetAlignment(Gdiplus::StringAlignmentCenter);
            c.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            graphics.DrawString(txt,-1,&valueFont,
                Gdiplus::RectF(x-22,knobY-29,44,18),&c,&b);
        }
        wchar_t label[32]{};
        const float high=VIZ_EQ_HIGH_HZ[(size_t)i];
        if(high>=1000) swprintf_s(label,L"%.0fk",high/1000);
        else swprintf_s(label,L"%.0f",high);
        Gdiplus::SolidBrush b(titleColor);
        Gdiplus::StringFormat c;
        c.SetAlignment(Gdiplus::StringAlignmentCenter);
        c.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        graphics.DrawString(label,-1,&valueFont,
            Gdiplus::RectF(x-26,bottom+10,52,20),&c,&b);
    }}


static void RenderEqMainPage(
    Gdiplus::Graphics& graphics,
    bool dark,
    const EqMediaState& media) {
    for (const auto& widget : g_eqLayoutWidgets) {
        if (!widget.present) continue;
        DrawEqWidget(graphics, widget, dark, media, false);
    }

    const Gdiplus::Color titleColor(255,dark?245:30,dark?245:30,dark?248:30);
    const Gdiplus::FontFamily dummyFamily(L"Segoe MDL2 Assets");
    Gdiplus::Font gearFont(&dummyFamily,18.0f,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    const RECT pageButton=GetEqPageButtonRect();
    if(g_eqHotPageButton){ Gdiplus::SolidBrush pb(Gdiplus::Color(dark?255:255,dark?62:224,dark?62:224,dark?66:226)); Gdiplus::GraphicsPath path; AddRoundedRectSubpath(path,(float)pageButton.left,(float)pageButton.top,(float)(pageButton.right-pageButton.left),(float)(pageButton.bottom-pageButton.top),9); graphics.FillPath(&pb,&path); }
    Gdiplus::SolidBrush gearBrush(g_eqHotPageButton?Gdiplus::Color(255,255,255,255):titleColor); Gdiplus::StringFormat c; c.SetAlignment(Gdiplus::StringAlignmentCenter); c.SetLineAlignment(Gdiplus::StringAlignmentCenter); graphics.DrawString(L"\xE713",-1,&gearFont,Gdiplus::RectF((float)pageButton.left,(float)pageButton.top,(float)(pageButton.right-pageButton.left),(float)(pageButton.bottom-pageButton.top)),&c,&gearBrush);
}

struct EqLayoutBuilderView {
    RECT canvas{};
    float scale = 1.0f;
    int sidebarLeft = 540;
    int sidebarRight = 690;
    int sidebarTop = 74;
};

static EqLayoutBuilderView GetEqLayoutBuilderView() {
    EqLayoutBuilderView v{};
    constexpr int left=20, top=72, maxW=510, maxH=435;
    v.scale=EqClampLayoutScale(std::min((float)maxW/(float)EQ_LAYOUT_CANVAS_W,(float)maxH/(float)EQ_LAYOUT_CANVAS_H));
    const int w=std::max(1,(int)std::lround(EQ_LAYOUT_CANVAS_W*v.scale));
    const int h=std::max(1,(int)std::lround(EQ_LAYOUT_CANVAS_H*v.scale));
    v.canvas=RECT{left,top,left+w,top+h};
    return v;
}

static RECT EqBuilderWidgetScreenRect(const EqLayoutBuilderView& view,const EqLayoutWidgetState& widget){
    return RECT{view.canvas.left+(int)std::lround(widget.x*view.scale),view.canvas.top+(int)std::lround(widget.y*view.scale),view.canvas.left+(int)std::lround((widget.x+widget.w)*view.scale),view.canvas.top+(int)std::lround((widget.y+widget.h)*view.scale)};
}

static int EqBuilderHitWidget(int x,int y){
    const EqLayoutBuilderView view=GetEqLayoutBuilderView();
    for(int i=(int)g_eqLayoutWidgets.size()-1;i>=0;--i){ const auto& w=g_eqLayoutWidgets[(size_t)i]; if(!w.present) continue; const RECT sr=EqBuilderWidgetScreenRect(view,w); if(PtInRect(&sr,POINT{x,y})) return w.id; }
    return -1;
}

static constexpr int EQ_BUILDER_DRAG_NONE = -1;
static int g_eqBuilderSelectedId = -1;
static int g_eqBuilderDragId = EQ_BUILDER_DRAG_NONE;
static int g_eqBuilderResizeId = EQ_BUILDER_DRAG_NONE;
static int g_eqBuilderResizeCorner = -1;
static int g_eqBuilderDragStartX = 0;
static int g_eqBuilderDragStartY = 0;
static int g_eqBuilderOriginalX = 0;
static int g_eqBuilderOriginalY = 0;
static int g_eqBuilderOriginalW = 0;
static int g_eqBuilderOriginalH = 0;
static bool g_eqBuilderRemoveOnDrop = false;

static RECT EqBuilderPaletteRect(int id){
    int idx = 0;
    switch (id) {
    case EQ_LAYOUT_EQ: idx = 0; break;
    case EQ_LAYOUT_PRESETS: idx = 1; break;
    case EQ_LAYOUT_MEDIA: idx = 2; break;
    case EQ_LAYOUT_LYRICS: idx = 3; break;
    default: idx = 0; break;
    }
    return RECT{545,100+idx*62,688,150+idx*62};
}

static const wchar_t* EqBuilderWidgetName(int id){
    switch(id){
    case EQ_LAYOUT_EQ:return L"EQ";
    case EQ_LAYOUT_PRESETS:return L"Presets";
    case EQ_LAYOUT_MEDIA:return L"Media";
    case EQ_LAYOUT_LYRICS:return L"Lyrics";
    default:return L"?";
    }
}

static RECT EqBuilderRotateRect(const RECT& r){ return RECT{r.right-28,r.top+4,r.right-4,r.top+28}; }
static RECT EqBuilderDeleteRect(const RECT& r){ return RECT{r.left+4,r.top+4,r.left+28,r.top+28}; }

static int EqBuilderHitHandle(const RECT& r,int x,int y){
    constexpr int hs=12;
    const RECT handles[4]={{r.left-hs/2,r.top-hs/2,r.left+hs/2,r.top+hs/2},{r.right-hs/2,r.top-hs/2,r.right+hs/2,r.top+hs/2},{r.left-hs/2,r.bottom-hs/2,r.left+hs/2,r.bottom+hs/2},{r.right-hs/2,r.bottom-hs/2,r.right+hs/2,r.bottom+hs/2}};
    for(int i=0;i<4;++i) if(PtInRect(&handles[i],POINT{x,y})) return i;
    return -1;
}

static void DrawEqLayoutBuilder(
    Gdiplus::Graphics& graphics,
    bool dark,
    const EqMediaState& media) {
    const Gdiplus::Color titleColor(255,dark?245:30,dark?245:30,dark?248:30);
    const Gdiplus::Color mutedColor(190,dark?205:90,dark?205:90,dark?210:95);
    const Gdiplus::Color accent(255,105,165,255);
    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold"); Gdiplus::FontFamily smallFamily(L"Segoe UI"); Gdiplus::Font header(&titleFamily,16,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel); Gdiplus::Font small(&smallFamily,10.5f,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    Gdiplus::SolidBrush tb(titleColor), mb(mutedColor);
    graphics.DrawString(L"Layout Builder",-1,&header,Gdiplus::PointF(20,18),&tb);
    graphics.DrawString(L"Drag sections onto the canvas. Move and resize them",-1,&small,Gdiplus::PointF(20,42),&mb);

    // Desktop layout editor entry point (visualizer / album widget / lyrics
    // positions on the desktop).
    {
        const RECT button = EqGetDesktopLayoutButtonRect();
        Gdiplus::SolidBrush buttonBrush(accent);
        Gdiplus::GraphicsPath path;
        AddRoundedRectSubpath(path, static_cast<float>(button.left), static_cast<float>(button.top),
                              static_cast<float>(button.right - button.left),
                              static_cast<float>(button.bottom - button.top), 9.0f);
        graphics.FillPath(&buttonBrush, &path);
        Gdiplus::Font buttonFont(&titleFamily, 12.5f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::StringFormat center;
        center.SetAlignment(Gdiplus::StringAlignmentCenter);
        center.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
        graphics.DrawString(L"Open desktop layout editor", -1, &buttonFont,
                            Gdiplus::RectF(static_cast<float>(button.left), static_cast<float>(button.top),
                                           static_cast<float>(button.right - button.left),
                                           static_cast<float>(button.bottom - button.top)),
                            &center, &textBrush);
    }

    const EqLayoutBuilderView view=GetEqLayoutBuilderView();
    Gdiplus::SolidBrush canvasBrush(Gdiplus::Color(dark?215:250,dark?19:247,dark?19:247,dark?21:249));
    graphics.FillRectangle(&canvasBrush, (Gdiplus::REAL)view.canvas.left, (Gdiplus::REAL)view.canvas.top, (Gdiplus::REAL)(view.canvas.right-view.canvas.left), (Gdiplus::REAL)(view.canvas.bottom-view.canvas.top));
    Gdiplus::Pen gridPen(Gdiplus::Color(dark?28:18,dark?90:80,dark?90:80,dark?95:85),1.0f);
    for(int gx=0;gx<=EQ_LAYOUT_CANVAS_W;gx+=EQ_LAYOUT_GRID){ const float x=view.canvas.left+gx*view.scale; graphics.DrawLine(&gridPen,x,(float)view.canvas.top,x,(float)view.canvas.bottom); }
    for(int gy=0;gy<=EQ_LAYOUT_CANVAS_H;gy+=EQ_LAYOUT_GRID){ const float y=view.canvas.top+gy*view.scale; graphics.DrawLine(&gridPen,(float)view.canvas.left,y,(float)view.canvas.right,y); }
    Gdiplus::Pen borderPen(Gdiplus::Color(150,dark?140:100,dark?140:100,dark?145:105),1.0f); graphics.DrawRectangle(&borderPen, (Gdiplus::REAL)view.canvas.left, (Gdiplus::REAL)view.canvas.top, (Gdiplus::REAL)(view.canvas.right-view.canvas.left-1), (Gdiplus::REAL)(view.canvas.bottom-view.canvas.top-1));

    // The builder uses a lightweight preview so the editor stays cheap and does
    // not invoke the full live widget interaction layer while dragging.
    for(const auto& w:g_eqLayoutWidgets){
        if(!w.present) continue;
        const RECT sr=EqBuilderWidgetScreenRect(view,w);
        // Paint a lightweight adaptive representation over the logical preview.
        Gdiplus::GraphicsPath path; AddRoundedRectSubpath(path,(float)sr.left,(float)sr.top,(float)(sr.right-sr.left),(float)(sr.bottom-sr.top),7);
        Gdiplus::Pen p(w.id==g_eqBuilderSelectedId?Gdiplus::Color(255,105,165,255):Gdiplus::Color(110,dark?150:100,dark?150:100,dark?155:105),w.id==g_eqBuilderSelectedId?2.0f:1.0f); graphics.DrawPath(&p,&path);
        Gdiplus::Font mini(&smallFamily,9,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel); Gdiplus::SolidBrush labelBrush(titleColor);
        Gdiplus::StringFormat center; center.SetAlignment(Gdiplus::StringAlignmentCenter); center.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        graphics.DrawString(EqBuilderWidgetName(w.id),-1,&mini,Gdiplus::RectF((float)sr.left,(float)sr.top,(float)(sr.right-sr.left),18),&center,&labelBrush);
        if(w.id==EQ_LAYOUT_EQ){
            const bool rotated = w.rotation == 90 || w.rotation == 270;
            const int count = 10;
            if (rotated) {
                const int srHeight = static_cast<int>(sr.bottom - sr.top);
                for(int i=0;i<count;i++){
                    const float py=(float)sr.top+30.0f+
                        (float)std::max<int>(1, srHeight-42)*i/(float)(count-1);
                    graphics.DrawLine(&p,(float)sr.left+34.0f,py,
                                      (float)sr.right-10.0f,py);
                }
            } else {
                const int srWidth = static_cast<int>(sr.right - sr.left);
                for(int i=0;i<count;i++){
                    const float px=(float)sr.left+14.0f+
                        (float)std::max<int>(1, srWidth-28)*i/(float)(count-1);
                    const float by=(float)sr.bottom-12;
                    graphics.DrawLine(&p,px,by,px,(float)sr.top+28);
                }
            }
        } else if(w.id==EQ_LAYOUT_PRESETS){
            const EqPresetGridMetrics grid = EqGetPresetGridMetrics(w);
            for(int rr=0;rr<grid.rows;rr++){
                for(int cc=0;cc<grid.cols;cc++){
                    if(rr*grid.cols+cc>=EqGetPresetItemCount()) break;
                    const int cl=sr.left+9+cc*
                        std::max(12, (static_cast<int>(sr.right-sr.left)-18-(grid.cols-1)*4)/grid.cols + 4);
                    const int cw=std::max(12,
                        (static_cast<int>(sr.right-sr.left)-18-(grid.cols-1)*4)/grid.cols);
                    const int ct=sr.top+28+rr*
                        std::max(16, grid.rowHeight * static_cast<int>(view.scale));
                    Gdiplus::SolidBrush b(Gdiplus::Color(28,255,255,255));
                    graphics.FillRectangle(&b,(Gdiplus::REAL)cl,(Gdiplus::REAL)ct,
                                           (Gdiplus::REAL)cw,
                                           std::min(14.0f,(Gdiplus::REAL)grid.rowHeight-4.0f));
                }
            }
        }
        else if(w.id==EQ_LAYOUT_LYRICS){
            Gdiplus::Pen lyricsPreviewPen(Gdiplus::Color(75,255,255,255), 1.0f);
            const int srWidth = static_cast<int>(sr.right - sr.left);
            const int lineRight = sr.left + std::max(24, srWidth - 14);
            for(int i=0;i<5;++i){
                const int py=sr.top+26+i*15;
                if(py+5>=sr.bottom) break;
                graphics.DrawLine(&lyricsPreviewPen,(float)sr.left+12.0f,(float)py,
                                  (float)lineRight,(float)py);
            }
        }
        else {
            // Media preview intentionally has no album-cover area. The live
            // Media section also uses that space for metadata/timeline only.
            Gdiplus::Pen mediaPreviewPen(Gdiplus::Color(75,255,255,255), 1.0f);
            const int srWidth = static_cast<int>(sr.right - sr.left);
            const int lineRight = sr.left + std::max(24, srWidth - 14);
            graphics.DrawLine(&mediaPreviewPen,
                (float)sr.left + 12.0f, (float)sr.top + 24.0f,
                (float)lineRight, (float)sr.top + 24.0f);
            graphics.DrawLine(&mediaPreviewPen,
                (float)sr.left + 12.0f, (float)sr.top + 34.0f,
                (float)lineRight, (float)sr.top + 34.0f);
        }
        if(w.id==g_eqBuilderSelectedId){
            const RECT del=EqBuilderDeleteRect(sr);
            Gdiplus::SolidBrush db(Gdiplus::Color(235,220,45,45));
            graphics.FillEllipse(&db, (Gdiplus::REAL)del.left, (Gdiplus::REAL)del.top, 20.0f, 20.0f);
            Gdiplus::SolidBrush rb(Gdiplus::Color(210,105,165,255));
            if (w.id == EQ_LAYOUT_EQ) {
                const RECT rot=EqBuilderRotateRect(sr);
                graphics.FillEllipse(&rb, (Gdiplus::REAL)rot.left, (Gdiplus::REAL)rot.top, 20.0f, 20.0f);
            }
            Gdiplus::Font sf(L"Segoe MDL2 Assets",11,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
            Gdiplus::SolidBrush wb(Gdiplus::Color(255,255,255,255));
            graphics.DrawString(L"\xE894",-1,&sf,Gdiplus::RectF((float)del.left,(float)del.top,20,20),&center,&wb);
            if (w.id == EQ_LAYOUT_EQ) {
                const RECT rot=EqBuilderRotateRect(sr);
                graphics.DrawString(L"\xE895",-1,&sf,Gdiplus::RectF((float)rot.left,(float)rot.top,20,20),&center,&wb);
            }
            const int hs=8; const POINT corners[4]={{sr.left,sr.top},{sr.right,sr.top},{sr.left,sr.bottom},{sr.right,sr.bottom}}; Gdiplus::SolidBrush hb(accent); for(const auto& c:corners) graphics.FillRectangle(&hb, (Gdiplus::REAL)c.x - hs * 0.5f, (Gdiplus::REAL)c.y - hs * 0.5f, (Gdiplus::REAL)hs, (Gdiplus::REAL)hs);
        }
    }

    Gdiplus::Font paletteHeader(&titleFamily,13,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    graphics.DrawString(L"Sections",-1,&paletteHeader,Gdiplus::PointF(545,82),&tb);
    for(int id=0;id<EQ_LAYOUT_WIDGET_COUNT;++id){ const RECT pr=EqBuilderPaletteRect(id); const bool enabled=!EqIsLayoutWidgetPresent(id); const bool hot=(g_eqBuilderDragId==id); Gdiplus::SolidBrush b(Gdiplus::Color(enabled?(hot?55:35):(dark?18:14), enabled?105:150, enabled?165:155, enabled?255:160)); Gdiplus::GraphicsPath path; AddRoundedRectSubpath(path,(float)pr.left,(float)pr.top,(float)(pr.right-pr.left),(float)(pr.bottom-pr.top),9); graphics.FillPath(&b,&path); Gdiplus::Font f(&smallFamily,12,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel); Gdiplus::SolidBrush text(enabled?titleColor:mutedColor); Gdiplus::StringFormat c; c.SetAlignment(Gdiplus::StringAlignmentCenter); c.SetLineAlignment(Gdiplus::StringAlignmentCenter); graphics.DrawString(EqBuilderWidgetName(id),-1,&f,Gdiplus::RectF((float)pr.left,(float)pr.top,(float)(pr.right-pr.left),(float)(pr.bottom-pr.top)),&c,&text); }
    graphics.DrawString(L"Each section can be placed once. Drag Lyrics onto the canvas for scrollable LRC and click-to-seek.",-1,&small,Gdiplus::RectF(545,360,145,92),nullptr,&mb);

    // Palette drag ghost.
    if(g_eqBuilderDragId>=0){ POINT cursor{}; GetCursorPos(&cursor); ScreenToClient(g_eqPopupHwnd,&cursor); const double scale=EqGetPopupDpiScale(); cursor.x=(LONG)std::lround(cursor.x/scale); cursor.y=(LONG)std::lround(cursor.y/scale); const RECT ghost={cursor.x-45,cursor.y-20,cursor.x+45,cursor.y+20}; Gdiplus::SolidBrush b(Gdiplus::Color(95,105,165,255)); Gdiplus::GraphicsPath path; AddRoundedRectSubpath(path,(float)ghost.left,(float)ghost.top,(float)(ghost.right-ghost.left),(float)(ghost.bottom-ghost.top),8); graphics.FillPath(&b,&path); Gdiplus::StringFormat c; c.SetAlignment(Gdiplus::StringAlignmentCenter); c.SetLineAlignment(Gdiplus::StringAlignmentCenter); graphics.DrawString(EqBuilderWidgetName(g_eqBuilderDragId),-1,&small,Gdiplus::RectF((float)ghost.left,(float)ghost.top,90,40),&c,&tb); }
}

static const wchar_t* EqPopupAspectName(int aspect) {
    switch (aspect) {
    case EQ_POPUP_ASPECT_4_3: return L"4:3";
    case EQ_POPUP_ASPECT_21_9: return L"21:9";
    default: return L"16:9";
    }
}

struct EqPopupPlacementView {
    RECT canvas{};
    float scale = 1.0f;
};

static EqPopupPlacementView GetEqPopupPlacementView() {
    EqPopupPlacementView view{};
    constexpr int left = 20;
    constexpr int top = EQ_SETTINGS_POSITION_TOP + 122;
    constexpr int maxWidth = 658;
    constexpr int maxHeight = 320;

    double aspect = 16.0 / 9.0;
    if (g_eqPopupPlacementAspect == EQ_POPUP_ASPECT_4_3)
        aspect = 4.0 / 3.0;
    else if (g_eqPopupPlacementAspect == EQ_POPUP_ASPECT_21_9)
        aspect = 21.0 / 9.0;

    int width = maxWidth;
    int height = static_cast<int>(std::lround(width / aspect));
    if (height > maxHeight) {
        height = maxHeight;
        width = static_cast<int>(std::lround(height * aspect));
    }

    view.canvas = RECT{left, top, left + width, top + height};
    view.scale = std::max(0.05f, static_cast<float>(width) / 1000.0f);
    return view;
}

static RECT EqGetPopupPlacementAspectButtonRect(int aspect) {
    // Compact buttons with equal margins; they no longer run almost edge-to-edge.
    constexpr int groupWidth = 478; // 3 * 150 + 2 * 14
    constexpr int left = (EQ_POPUP_WIDTH - groupWidth) / 2;
    constexpr int gap = 14;
    constexpr int width = 150;
    constexpr int top = EQ_SETTINGS_POSITION_TOP + 50;
    constexpr int height = 44;
    return RECT{left + aspect * (width + gap), top,
                left + aspect * (width + gap) + width, top + height};
}

// Lives in the settings page header, between the Layout Builder title and
// the back button, so it is reachable without scrolling.
static RECT EqGetDesktopLayoutButtonRect() {
    return RECT{372, 14, 572, 50};
}

static POINT EqPopupPlacementNormalizedToScreen(
    const EqPopupPlacementView& view, int x, int y) {
    return POINT{
        view.canvas.left + static_cast<int>(std::lround(
            (view.canvas.right - view.canvas.left) * x / 1000.0)),
        view.canvas.top + static_cast<int>(std::lround(
            (view.canvas.bottom - view.canvas.top) * y / 1000.0))
    };
}

static RECT EqGetPopupPlacementPreviewRect(const EqPopupPlacementView& view) {
    const auto& state = g_eqPopupPlacement[static_cast<size_t>(g_eqPopupPlacementAspect)];
    const int canvasWidth = view.canvas.right - view.canvas.left;
    const int canvasHeight = view.canvas.bottom - view.canvas.top;
    const int popupWidth = std::clamp(static_cast<int>(std::lround(canvasWidth * 0.26)), 110, 190);
    const int popupHeight = std::clamp(static_cast<int>(std::lround(popupWidth * 530.0 / 700.0)), 82, 150);
    const POINT centerNorm = EqGetPopupPlacementCenterNormalized(state);
    const int halfW = popupWidth / 2;
    const int halfH = popupHeight / 2;
    const int minX = halfW;
    const int maxX = std::max(minX, canvasWidth - halfW);
    const int minY = halfH;
    const int maxY = std::max(minY, canvasHeight - halfH);
    const int centerX = view.canvas.left + std::clamp(
        static_cast<int>(std::lround(canvasWidth * centerNorm.x / 1000.0)), minX, maxX);
    const int centerY = view.canvas.top + std::clamp(
        static_cast<int>(std::lround(canvasHeight * centerNorm.y / 1000.0)), minY, maxY);
    return RECT{centerX - halfW, centerY - halfH, centerX + halfW, centerY + halfH};
}

static void EqClampPopupPlacementToCanvas(const EqPopupPlacementView& view,
                                             EqPopupPlacementState& state) {
    const int canvasWidth = view.canvas.right - view.canvas.left;
    const int canvasHeight = view.canvas.bottom - view.canvas.top;
    if (canvasWidth <= 0 || canvasHeight <= 0)
        return;

    const int popupWidth = std::clamp(static_cast<int>(std::lround(canvasWidth * 0.26)), 110, 190);
    const int popupHeight = std::clamp(static_cast<int>(std::lround(popupWidth * 530.0 / 700.0)), 82, 150);
    const int minNX = static_cast<int>(std::lround((popupWidth / 2.0) * 1000.0 / canvasWidth));
    const int maxNX = 1000 - minNX;
    const int minNY = static_cast<int>(std::lround((popupHeight / 2.0) * 1000.0 / canvasHeight));
    const int maxNY = 1000 - minNY;
    const POINT current = EqGetPopupPlacementCenterNormalized(state);
    const int clampedX = std::clamp<int>(current.x, minNX, std::max(minNX, maxNX));
    const int clampedY = std::clamp<int>(current.y, minNY, std::max(minNY, maxNY));
    if (current.x != clampedX || current.y != clampedY)
        EqSetPopupPlacementFromCenterNormalized(state, clampedX, clampedY);
}

static RECT EqGetPopupPlacementScrollbarTrack() {
    return RECT{EQ_SETTINGS_SCROLLBAR_LEFT, 64, EQ_SETTINGS_SCROLLBAR_RIGHT, EQ_POPUP_HEIGHT - 12};
}

static int EqGetPopupSettingsScrollMax();

static RECT EqGetPopupPlacementScrollbarThumb() {
    const RECT track = EqGetPopupPlacementScrollbarTrack();
    const int trackHeight = std::max(1L, track.bottom - track.top);
    if (EQ_SETTINGS_POSITION_CONTENT_HEIGHT <= EQ_POPUP_HEIGHT)
        return track;
    const int visible = EQ_POPUP_HEIGHT;
    const int thumbHeight = std::clamp(static_cast<int>(std::lround(
        trackHeight * (static_cast<double>(visible) / EQ_SETTINGS_POSITION_CONTENT_HEIGHT))), 48, trackHeight);
    const int travel = std::max(0, trackHeight - thumbHeight);
    const int top = track.top + (travel > 0 ? static_cast<int>(std::lround(
        static_cast<double>(travel) * g_eqSettingsScrollOffset /
        std::max(1, EqGetPopupSettingsScrollMax()))) : 0);
    return RECT{track.left, top, track.right, top + thumbHeight};
}

static int EqGetPopupSettingsScrollMax() {
    return std::max(0, EQ_SETTINGS_POSITION_CONTENT_HEIGHT - EQ_POPUP_HEIGHT);
}

static void EqSetPopupSettingsScrollOffset(int value) {
    g_eqSettingsScrollOffset = std::clamp(value, 0, EqGetPopupSettingsScrollMax());
}

static int EqPopupPlacementHitTarget(int x, int y) {
    const EqPopupPlacementView view = GetEqPopupPlacementView();
    const auto& state = g_eqPopupPlacement[static_cast<size_t>(g_eqPopupPlacementAspect)];
    const POINT cross = EqPopupPlacementNormalizedToScreen(view, state.crossX, state.crossY);
    const RECT popup = EqGetPopupPlacementPreviewRect(view);
    if (PtInRect(&popup, POINT{x, y}))
        return EQ_POPUP_PLACEMENT_DRAG_POPUP;
    const RECT crossHit{cross.x - 10, cross.y - 10, cross.x + 11, cross.y + 11};
    if (PtInRect(&crossHit, POINT{x, y}))
        return EQ_POPUP_PLACEMENT_DRAG_CROSS;
    return EQ_POPUP_PLACEMENT_DRAG_NONE;
}

static int EqPopupPlacementScreenToNormalized(const EqPopupPlacementView& view, int x, int y, bool horizontal) {
    const int span = horizontal ? (view.canvas.right - view.canvas.left) : (view.canvas.bottom - view.canvas.top);
    const int start = horizontal ? view.canvas.left : view.canvas.top;
    if (span <= 0) return 0;
    return std::clamp(static_cast<int>(std::lround((x - start) * 1000.0 / span)), 0, 1000);
}

static POINT EqGetPopupPlacementAnchorForWorkArea(const RECT& workArea) {
    const auto& state = g_eqPopupPlacement[static_cast<size_t>(g_eqPopupPlacementAspect)];
    const POINT centerNorm = EqGetPopupPlacementCenterNormalized(state);
    const double nx = centerNorm.x / 1000.0;
    const double ny = centerNorm.y / 1000.0;
    return POINT{
        workArea.left + static_cast<int>(std::lround((workArea.right - workArea.left) * nx)),
        workArea.top + static_cast<int>(std::lround((workArea.bottom - workArea.top) * ny))
    };
}

static void EqApplyConfiguredPopupPlacement(HWND hwnd, const RECT& workArea) {
    if (!hwnd || !IsWindow(hwnd)) return;
    RECT wr{};
    if (!GetWindowRect(hwnd, &wr)) return;
    POINT anchor = EqGetPopupPlacementAnchorForWorkArea(workArea);
    int width = wr.right - wr.left;
    int height = wr.bottom - wr.top;
    int x = anchor.x - width / 2;
    int y = anchor.y - height / 2;
    x = std::clamp<int>(x, workArea.left, std::max(workArea.left, workArea.right - width));
    y = std::clamp<int>(y, workArea.top, std::max(workArea.top, workArea.bottom - height));
    SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static void DrawEqPopupPlacementEditor(Gdiplus::Graphics& graphics, bool dark) {
    const auto& state = g_eqPopupPlacement[static_cast<size_t>(g_eqPopupPlacementAspect)];
    const EqPopupPlacementView view = GetEqPopupPlacementView();
    const Gdiplus::Color titleColor(255, dark ? 245 : 30, dark ? 245 : 30, dark ? 248 : 30);
    const Gdiplus::Color mutedColor(190, dark ? 205 : 90, dark ? 205 : 90, dark ? 210 : 95);
    const Gdiplus::Color activeColor(255, 105, 165, 255);
    const Gdiplus::Color inactiveColor(255, 126, 126, 126);
    const Gdiplus::Color gridColor(36, dark ? 95 : 80, dark ? 95 : 80, dark ? 100 : 85);

    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold");
    Gdiplus::FontFamily bodyFamily(L"Segoe UI");
    Gdiplus::Font titleFont(&titleFamily, 16.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font bodyFont(&bodyFamily, 10.5f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush titleBrush(titleColor), mutedBrush(mutedColor);
    graphics.DrawString(L"Popup position", -1, &titleFont,
                        Gdiplus::PointF(20.0f, static_cast<float>(EQ_SETTINGS_POSITION_TOP + 12)),
                        &titleBrush);
    graphics.DrawString(L"Choose the screen format, then drag the popup or center cross independently.", -1,
                        &bodyFont,
                        Gdiplus::PointF(20.0f, static_cast<float>(EQ_SETTINGS_POSITION_TOP + 35)),
                        &mutedBrush);

    for (int aspect = 0; aspect < EQ_POPUP_ASPECT_COUNT; ++aspect) {
        const RECT button = EqGetPopupPlacementAspectButtonRect(aspect);
        const bool selected = aspect == g_eqPopupPlacementAspect;
        Gdiplus::SolidBrush buttonBrush(selected ? activeColor : inactiveColor);
        Gdiplus::GraphicsPath path;
        AddRoundedRectSubpath(path, static_cast<float>(button.left), static_cast<float>(button.top),
                              static_cast<float>(button.right - button.left),
                              static_cast<float>(button.bottom - button.top), 10.0f);
        graphics.FillPath(&buttonBrush, &path);
        Gdiplus::Font formatFont(&titleFamily, 13.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::StringFormat center;
        center.SetAlignment(Gdiplus::StringAlignmentCenter);
        center.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
        graphics.DrawString(EqPopupAspectName(aspect), -1, &formatFont,
                            Gdiplus::RectF(static_cast<float>(button.left), static_cast<float>(button.top),
                                           static_cast<float>(button.right - button.left),
                                           static_cast<float>(button.bottom - button.top)),
                            &center, &textBrush);
    }

    Gdiplus::SolidBrush canvasBrush(Gdiplus::Color(dark ? 215 : 250, dark ? 19 : 247, dark ? 19 : 247, dark ? 21 : 249));
    graphics.FillRectangle(&canvasBrush, static_cast<Gdiplus::REAL>(view.canvas.left),
                           static_cast<Gdiplus::REAL>(view.canvas.top),
                           static_cast<Gdiplus::REAL>(view.canvas.right - view.canvas.left),
                           static_cast<Gdiplus::REAL>(view.canvas.bottom - view.canvas.top));

    Gdiplus::Pen gridPen(gridColor, 1.0f);
    const int step = 10;
    for (int gx = 0; gx <= 1000; gx += step) {
        const int x = view.canvas.left + static_cast<int>(std::lround(
            (view.canvas.right - view.canvas.left) * gx / 1000.0));
        graphics.DrawLine(&gridPen, static_cast<float>(x), static_cast<float>(view.canvas.top),
                          static_cast<float>(x), static_cast<float>(view.canvas.bottom));
    }
    for (int gy = 0; gy <= 1000; gy += step) {
        const int y = view.canvas.top + static_cast<int>(std::lround(
            (view.canvas.bottom - view.canvas.top) * gy / 1000.0));
        graphics.DrawLine(&gridPen, static_cast<float>(view.canvas.left), static_cast<float>(y),
                          static_cast<float>(view.canvas.right), static_cast<float>(y));
    }

    Gdiplus::Pen borderPen(Gdiplus::Color(150, dark ? 140 : 100, dark ? 140 : 100, dark ? 145 : 105), 1.0f);
    graphics.DrawRectangle(&borderPen, static_cast<Gdiplus::REAL>(view.canvas.left),
                           static_cast<Gdiplus::REAL>(view.canvas.top),
                           static_cast<Gdiplus::REAL>(view.canvas.right - view.canvas.left - 1),
                           static_cast<Gdiplus::REAL>(view.canvas.bottom - view.canvas.top - 1));

    const POINT cross = EqPopupPlacementNormalizedToScreen(view, state.crossX, state.crossY);
    Gdiplus::Pen crossPen(Gdiplus::Color(215, 215, 215, 215), 1.2f);
    graphics.DrawLine(&crossPen, static_cast<float>(view.canvas.left), static_cast<float>(cross.y),
                      static_cast<float>(view.canvas.right), static_cast<float>(cross.y));
    graphics.DrawLine(&crossPen, static_cast<float>(cross.x), static_cast<float>(view.canvas.top),
                      static_cast<float>(cross.x), static_cast<float>(view.canvas.bottom));

    const RECT popup = EqGetPopupPlacementPreviewRect(view);
    Gdiplus::GraphicsPath popupPath;
    AddRoundedRectSubpath(popupPath, static_cast<float>(popup.left), static_cast<float>(popup.top),
                          static_cast<float>(popup.right - popup.left),
                          static_cast<float>(popup.bottom - popup.top), 8.0f);
    Gdiplus::SolidBrush popupFill(Gdiplus::Color(130, 128, 128, 128));
    Gdiplus::Pen popupPen(activeColor, 2.0f);
    graphics.FillPath(&popupFill, &popupPath);
    graphics.DrawPath(&popupPen, &popupPath);

    Gdiplus::Font popupFont(&bodyFamily, 10.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::StringFormat popupCenter;
    popupCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
    popupCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::SolidBrush popupText(Gdiplus::Color(240, 245, 245, 245));
    graphics.DrawString(L"Media & EQ", -1, &popupFont,
                        Gdiplus::RectF(static_cast<float>(popup.left), static_cast<float>(popup.top),
                                       static_cast<float>(popup.right - popup.left),
                                       static_cast<float>(popup.bottom - popup.top)),
                        &popupCenter, &popupText);

    Gdiplus::Pen crossMarkPen(Gdiplus::Color(235, 235, 235, 235), 2.0f);
    graphics.DrawLine(&crossMarkPen, static_cast<float>(cross.x - 8), static_cast<float>(cross.y),
                      static_cast<float>(cross.x + 8), static_cast<float>(cross.y));
    graphics.DrawLine(&crossMarkPen, static_cast<float>(cross.x), static_cast<float>(cross.y - 8),
                      static_cast<float>(cross.x), static_cast<float>(cross.y + 8));
}

static void RenderEqSettingsPage(Gdiplus::Graphics& graphics, bool dark) {
    const EqMediaState media = GetEqMediaStateSnapshot();
    const Gdiplus::GraphicsState contentState = graphics.Save();
    graphics.TranslateTransform(0.0f, -static_cast<Gdiplus::REAL>(g_eqSettingsScrollOffset));
    DrawEqLayoutBuilder(graphics, dark, media);
    DrawEqPopupPlacementEditor(graphics, dark);
    graphics.Restore(contentState);

    if (EqGetPopupSettingsScrollMax() > 0) {
        const RECT track = EqGetPopupPlacementScrollbarTrack();
        const RECT thumb = EqGetPopupPlacementScrollbarThumb();
        Gdiplus::SolidBrush trackBrush(Gdiplus::Color(52, dark ? 95 : 165, dark ? 95 : 165, dark ? 100 : 170));
        Gdiplus::GraphicsPath trackPath;
        AddRoundedRectSubpath(trackPath, static_cast<float>(track.left), static_cast<float>(track.top),
                              static_cast<float>(track.right - track.left),
                              static_cast<float>(track.bottom - track.top), 3.0f);
        graphics.FillPath(&trackBrush, &trackPath);
        Gdiplus::SolidBrush thumbBrush(Gdiplus::Color(175, dark ? 175 : 120, dark ? 175 : 120, dark ? 180 : 125));
        Gdiplus::GraphicsPath thumbPath;
        AddRoundedRectSubpath(thumbPath, static_cast<float>(thumb.left), static_cast<float>(thumb.top),
                              static_cast<float>(thumb.right - thumb.left),
                              static_cast<float>(thumb.bottom - thumb.top), 3.0f);
        graphics.FillPath(&thumbBrush, &thumbPath);
    }

    const Gdiplus::Color titleColor(255,dark?245:30,dark?245:30,dark?248:30);
    Gdiplus::FontFamily symbolFamily(L"Segoe MDL2 Assets");
    Gdiplus::Font arrowFont(&symbolFamily,19.0f,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    const RECT pageButton=GetEqPageButtonRect();
    if(g_eqHotPageButton){ Gdiplus::SolidBrush pb(Gdiplus::Color(dark?255:255,dark?62:224,dark?62:224,dark?66:226)); Gdiplus::GraphicsPath path; AddRoundedRectSubpath(path,(float)pageButton.left,(float)pageButton.top,(float)(pageButton.right-pageButton.left),(float)(pageButton.bottom-pageButton.top),9); graphics.FillPath(&pb,&path); }
    Gdiplus::SolidBrush arrowBrush(g_eqHotPageButton?Gdiplus::Color(255,255,255,255):titleColor); Gdiplus::StringFormat c; c.SetAlignment(Gdiplus::StringAlignmentCenter); c.SetLineAlignment(Gdiplus::StringAlignmentCenter); graphics.DrawString(L"\xE72B",-1,&arrowFont,Gdiplus::RectF((float)pageButton.left,(float)pageButton.top,(float)(pageButton.right-pageButton.left),(float)(pageButton.bottom-pageButton.top)),&c,&arrowBrush);
}

static void RenderEqPopup(HWND hwnd) {
    if (!hwnd)
        return;

    const bool dark = IsDarkThemeEnabled();
    const double scale = EqGetPopupDpiScale();
    const int logicalWidth = std::max(1, g_eqPopupLogicalWidth);
    const int logicalHeight = std::max(1, g_eqPopupLogicalHeight);
    const int width = EqPopupScalePx(logicalWidth, scale);
    const int height = EqPopupScalePx(logicalHeight, scale);

    const EqMediaState media = GetEqMediaStateSnapshot();

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screen, &bi, DIB_RGB_COLORS,
                                      &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        DeleteDC(mem);
        ReleaseDC(nullptr, screen);
        return;
    }

    HGDIOBJ oldBitmap = SelectObject(mem, bitmap);
    std::memset(bits, 0, static_cast<size_t>(width) * height * 4);

    Gdiplus::Graphics graphics(mem);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    graphics.ScaleTransform(static_cast<Gdiplus::REAL>(scale),
                            static_cast<Gdiplus::REAL>(scale));

    const BYTE bgAlpha = dark ? 232 : 238;
    const Gdiplus::Color bgColor(
        bgAlpha, dark ? 28 : 242, dark ? 28 : 242, dark ? 30 : 244);
    Gdiplus::GraphicsPath bgPath;
    AddRoundedRectSubpath(bgPath, 1.0f, 1.0f,
                          static_cast<float>(logicalWidth - 2),
                          static_cast<float>(logicalHeight - 2), 16.0f);
    Gdiplus::SolidBrush bg(bgColor);
    graphics.FillPath(&bg, &bgPath);

    // Clip both pages to the popup body. The window itself stays stationary;
    // only the two surfaces slide horizontally over the common background.
    Gdiplus::GraphicsPath contentClip;
    AddRoundedRectSubpath(
        contentClip, 1.0f, 1.0f,
        static_cast<float>(EQ_POPUP_WIDTH - 2),
        static_cast<float>(logicalHeight - 2), 16.0f);
    const Gdiplus::GraphicsState clipState = graphics.Save();
    graphics.SetClip(&contentClip, Gdiplus::CombineModeIntersect);

    const float pageWidth = static_cast<float>(logicalWidth);
    const float progress = std::clamp(g_eqPageAnimationProgress, 0.0f, 1.0f);

    // Main page slides to the left as Settings arrives from the right.
    {
        const Gdiplus::GraphicsState state = graphics.Save();
        graphics.TranslateTransform(-progress * pageWidth, 0.0f);
        RenderEqMainPage(graphics, dark, media);
        graphics.Restore(state);
    }

    // Settings page is the mirror image of that motion.
    {
        const Gdiplus::GraphicsState state = graphics.Save();
        graphics.TranslateTransform((1.0f - progress) * pageWidth, 0.0f);
        RenderEqSettingsPage(graphics, dark);
        graphics.Restore(state);
    }

    graphics.Restore(clipState);

    POINT dstPos{};
    RECT popupRect{};
    GetWindowRect(hwnd, &popupRect);
    dstPos.x = popupRect.left;
    dstPos.y = popupRect.top;
    SIZE size{width, height};
    POINT srcPos{0, 0};
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, screen, &dstPos, &size,
                        mem, &srcPos, 0, &blend, ULW_ALPHA);

    SelectObject(mem, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

static void DestroyEqPopup();
static bool EqIsCursorOverTrayButton();

static bool EqPointInLyricsWidget(int x, int y,
                                  const EqLayoutWidgetState*& outWidget) {
    outWidget = nullptr;
    const auto* widget = EqGetLayoutWidgetConst(EQ_LAYOUT_LYRICS);
    if (!widget || !widget->present)
        return false;
    const RECT r = EqGetLayoutRect(EQ_LAYOUT_LYRICS);
    if (!PtInRect(&r, POINT{x, y}))
        return false;
    outWidget = widget;
    return true;
}

static LRESULT CALLBACK EqPopupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_ACTIVATE;
    case WM_SETFOCUS:
        RenderEqPopup(hwnd);
        return 0;
    case WM_CREATE:
        SetTimer(hwnd, kEqMediaRefreshTimerId, kEqMediaRefreshIntervalMs, nullptr);
        return 0;
    case WM_TIMER:
        if (wParam == kEqMediaRefreshTimerId) {
            RenderEqPopup(hwnd);
            return 0;
        }
        if (wParam == kEqPopupAnimationTimerId) {
            EqApplyPresetAnimationStep(hwnd);
            return 0;
        }
        if (wParam == kEqPopupPageAnimationTimerId) {
            EqApplyPageAnimationStep(hwnd);
            return 0;
        }
        break;
    case WM_KILLFOCUS:
        if (g_eqDraggingBand >= 0) {
            SaveCustomEQSettings();
            g_eqDraggingBand = -1;
            ReleaseCapture();
        }
        if (g_eqMediaSeeking) {
            g_eqMediaSeeking = false;
            ReleaseCapture();
        }
        if (g_eqLyricsScrollbarDragging) {
            g_eqLyricsScrollbarDragging = false;
            ReleaseCapture();
        }
        if (g_eqSettingsScrollbarDragging) {
            g_eqSettingsScrollbarDragging = false;
            ReleaseCapture();
        }
        if (g_eqPopupPlacementDragTarget != EQ_POPUP_PLACEMENT_DRAG_NONE) {
            g_eqPopupPlacementDragTarget = EQ_POPUP_PLACEMENT_DRAG_NONE;
            g_eqPopupPlacementDragOffsetX = 0;
            g_eqPopupPlacementDragOffsetY = 0;
        }
        // A click on our tray button also causes the popup to lose focus. Keep
        // it alive for that click so the tray callback can perform a real toggle
        // instead of immediately reopening a popup that is closing.
        if (!EqIsCursorOverTrayButton())
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
        return 0;
    case WM_MOUSEMOVE: {
        // Page buttons are intentionally handled before all page-specific hover
        // state. During the page slide the page content itself is noninteractive.
        if (!g_eqPageAnimationActive) {
            const bool settingsPage = g_eqPageAnimationTarget >= 0.5f;
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            // The gear/back page button uses one shared hitbox on both pages.
            // Track it before page-specific controls so hover also works on
            // the settings page.
            const bool hotPageButton = HitTestEqPageButton(x, y);
            if (hotPageButton != g_eqHotPageButton) {
                g_eqHotPageButton = hotPageButton;
                RenderEqPopup(hwnd);
            }

            if (settingsPage) {
                const EqLayoutBuilderView view = GetEqLayoutBuilderView();
                const double builderDpiScale = EqGetPopupDpiScale();
                const int bx = static_cast<int>(std::lround(static_cast<double>(x) / builderDpiScale));
                const int by = static_cast<int>(std::lround(static_cast<double>(y) / builderDpiScale)) + g_eqSettingsScrollOffset;

                if (g_eqSettingsScrollbarDragging) {
                    const RECT track = EqGetPopupPlacementScrollbarTrack();
                    const RECT thumb = EqGetPopupPlacementScrollbarThumb();
                    const int thumbTravel = std::max(1L, (track.bottom - track.top) - (thumb.bottom - thumb.top));
                    const int dy = y - g_eqSettingsScrollbarDragStartY;
                    EqSetPopupSettingsScrollOffset(g_eqSettingsScrollbarStartOffset +
                        static_cast<int>(std::lround(dy * EqGetPopupSettingsScrollMax() /
                            static_cast<double>(thumbTravel))));
                    RenderEqPopup(hwnd);
                    return 0;
                }

                if (g_eqPopupPlacementDragTarget != EQ_POPUP_PLACEMENT_DRAG_NONE) {
                    const EqPopupPlacementView placementView = GetEqPopupPlacementView();
                    const double placementScale = EqGetPopupDpiScale();
                    const int canvasWidth = placementView.canvas.right - placementView.canvas.left;
                    const int canvasHeight = placementView.canvas.bottom - placementView.canvas.top;
                    POINT cursor{};
                    if (!GetCursorPos(&cursor))
                        return 0;

                    // Use the cursor's actual screen-space delta from drag start.
                    // This deliberately avoids deriving the Y position from the
                    // settings page's scrolling/client coordinates.
                    const double logicalDeltaX =
                        static_cast<double>(cursor.x - g_eqPopupPlacementDragStartCursor.x) /
                        std::max(0.05, placementScale);
                    const double logicalDeltaY =
                        static_cast<double>(cursor.y - g_eqPopupPlacementDragStartCursor.y) /
                        std::max(0.05, placementScale);
                    const int normalizedDeltaX = static_cast<int>(std::lround(
                        logicalDeltaX * 1000.0 / std::max(1, canvasWidth)));
                    const int normalizedDeltaY = static_cast<int>(std::lround(
                        logicalDeltaY * 1000.0 / std::max(1, canvasHeight)));

                    auto& placement = g_eqPopupPlacement[static_cast<size_t>(g_eqPopupPlacementAspect)];
                    if (g_eqPopupPlacementDragTarget == EQ_POPUP_PLACEMENT_DRAG_POPUP) {
                        // Move the popup freely in both dimensions, keeping the
                        // complete preview rectangle inside the placement grid.
                        const int desiredX = g_eqPopupPlacementDragStartCenter.x + normalizedDeltaX;
                        const int desiredY = g_eqPopupPlacementDragStartCenter.y + normalizedDeltaY;
                        const int popupWidth = std::clamp(static_cast<int>(std::lround(canvasWidth * 0.26)), 110, 190);
                        const int popupHeight = std::clamp(static_cast<int>(std::lround(popupWidth * 530.0 / 700.0)), 82, 150);
                        const int minNX = static_cast<int>(std::lround((popupWidth / 2.0) * 1000.0 /
                            std::max(1, canvasWidth)));
                        const int maxNX = 1000 - minNX;
                        const int minNY = static_cast<int>(std::lround((popupHeight / 2.0) * 1000.0 /
                            std::max(1, canvasHeight)));
                        const int maxNY = 1000 - minNY;
                        EqSetPopupPlacementFromCenterNormalized(
                            placement,
                            std::clamp(desiredX, minNX, std::max(minNX, maxNX)),
                            std::clamp(desiredY, minNY, std::max(minNY, maxNY)));
                    } else {
                        placement.crossX = g_eqPopupPlacementDragStartCross.x + normalizedDeltaX;
                        placement.crossY = g_eqPopupPlacementDragStartCross.y + normalizedDeltaY;
                        ClampEqPopupPlacementState(placement);
                        EqClampPopupPlacementToCanvas(placementView, placement);
                    }
                    RenderEqPopup(hwnd);
                    return 0;
                }
                if (g_eqBuilderDragId >= 0) {
                    RenderEqPopup(hwnd);
                    TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0}; TrackMouseEvent(&tme);
                    return 0;
                }
                if (g_eqBuilderResizeId >= 0) {
                    if (auto* w=EqGetLayoutWidget(g_eqBuilderResizeId)) {
                        const int dx = bx - g_eqBuilderDragStartX;
                        const int dy = by - g_eqBuilderDragStartY;
                        int nx=w->x,ny=w->y,nw=g_eqBuilderOriginalW,nh=g_eqBuilderOriginalH;
                        if(g_eqBuilderResizeCorner==0){nx=g_eqBuilderOriginalX+dx;ny=g_eqBuilderOriginalY+dy;nw=g_eqBuilderOriginalW-dx;nh=g_eqBuilderOriginalH-dy;}
                        else if(g_eqBuilderResizeCorner==1){ny=g_eqBuilderOriginalY+dy;nw=g_eqBuilderOriginalW+dx;nh=g_eqBuilderOriginalH-dy;}
                        else if(g_eqBuilderResizeCorner==2){nx=g_eqBuilderOriginalX+dx;nw=g_eqBuilderOriginalW-dx;nh=g_eqBuilderOriginalH+dy;}
                        else {nw=g_eqBuilderOriginalW+dx;nh=g_eqBuilderOriginalH+dy;}
                        w->x=EqSnap10(nx);w->y=EqSnap10(ny);w->w=EqSnap10(nw);w->h=EqSnap10(nh);ClampEqLayoutWidget(*w);
                    }
                    RenderEqPopup(hwnd); return 0;
                }
                const int selectedId=g_eqBuilderSelectedId;
                if(selectedId>=0 && GetCapture()==hwnd && g_eqBuilderDragId==EQ_BUILDER_DRAG_NONE){
                    if(auto* w=EqGetLayoutWidget(selectedId)){
                        const int dx = bx - g_eqBuilderDragStartX;
                        const int dy = by - g_eqBuilderDragStartY;
                        w->x = EqSnap10(g_eqBuilderOriginalX + dx);
                        w->y = EqSnap10(g_eqBuilderOriginalY + dy);
                        ClampEqLayoutWidget(*w);
                        bool overPalette = false;
                        for (int paletteId = 0; paletteId < EQ_LAYOUT_WIDGET_COUNT; ++paletteId) {
                            const RECT paletteRect = EqBuilderPaletteRect(paletteId);
                            if (PtInRect(&paletteRect, POINT{bx, by}) &&
                                paletteId == selectedId) {
                                overPalette = true;
                                break;
                            }
                        }
                        g_eqBuilderRemoveOnDrop = overPalette;
                        RenderEqPopup(hwnd);
                        return 0;
                    }
                }
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0}; TrackMouseEvent(&tme);
                return 0;
            }

            const EqLayoutWidgetState* lyricsWidget = nullptr;
            const bool overLyrics = EqPointInLyricsWidget(x, y, lyricsWidget);
            if (g_eqLyricsScrollbarDragging) {
                lyricsWidget = EqGetLayoutWidgetConst(EQ_LAYOUT_LYRICS);
                if (lyricsWidget && lyricsWidget->present) {
                    std::shared_ptr<const std::vector<LyricsLine>> lines;
                    {
                        std::lock_guard<std::mutex> lock(g_lyricsMutex);
                        lines = g_lyricsLines;
                    }
                    const EqLyricsMetrics metrics = GetEqLyricsMetrics(
                        *lyricsWidget, lines.get());
                    const int trackHeight = std::max(1L,
                        metrics.scrollbarTrack.bottom - metrics.scrollbarTrack.top);
                    const RECT thumb = GetEqLyricsScrollbarThumb(metrics);
                    const int travel = std::max(1L,
                        trackHeight - (thumb.bottom - thumb.top));
                    const int dy = y - g_eqLyricsScrollbarDragStartY;
                    const int maxThumbDelta = std::max(0, travel -
                        static_cast<int>(std::lround(
                            static_cast<double>(g_eqLyricsScrollbarStartOffset) * travel /
                            std::max(1, metrics.maxScroll))));
                    const int initialThumb = static_cast<int>(std::lround(
                        static_cast<double>(g_eqLyricsScrollbarStartOffset) * travel /
                        std::max(1, metrics.maxScroll)));
                    const int thumbDelta = std::clamp(
                        dy, -initialThumb, maxThumbDelta);
                    g_eqLyricsScrollOffset = std::clamp(
                        g_eqLyricsScrollbarStartOffset +
                        static_cast<int>(std::lround(
                            static_cast<double>(thumbDelta) * metrics.maxScroll / travel)),
                        0, metrics.maxScroll);
                    // Any scrollbar movement counts as manual scrolling.
                    // Refresh the deadline on every mouse move so autofocus
                    // resumes 3.5 s after the user's last scrollbar movement.
                    PauseEqLyricsAutoFocus();
                    RenderEqPopup(hwnd);
                    return 0;
                } else {
                    g_eqLyricsScrollbarDragging = false;
                }
            }
            if (overLyrics && lyricsWidget) {
                const int hotLyrics = GetEqLyricsLineAtPoint(*lyricsWidget, x, y);
                if (hotLyrics != g_eqHotLyricsLine) {
                    g_eqHotLyricsLine = hotLyrics;
                    RenderEqPopup(hwnd);
                }
                return 0;
            }
            if (g_eqHotLyricsLine != -1) {
                g_eqHotLyricsLine = -1;
                RenderEqPopup(hwnd);
            }

            if (EqIsLayoutWidgetPresent(EQ_LAYOUT_MEDIA)) {
                if (g_eqMediaSeeking) {
                    g_eqMediaSeekPreviewSeconds = EqMediaPositionFromMouseX(x);
                    RenderEqPopup(hwnd);
                    return 0;
                }
            }

            if (g_eqDraggingBand >= 0) {
                SetEqGainFromMouse(g_eqDraggingBand, x, y);
                RenderEqPopup(hwnd);
                return 0;
            }

            const int hotMediaButton = EqIsLayoutWidgetPresent(EQ_LAYOUT_MEDIA) ? HitTestEqMediaButton(x, y) : -1;
            const int hotDelete = HitTestEqPresetDelete(x, y);
            const int hotPreset = HitTestEqPreset(x, y);
            const int hotBand = hotPreset >= 0 || hotPreset == EQ_PLUS_PRESET_HIT || hotDelete >= 0
                ? -1 : HitTestEqBand(x, y);
            if (hotMediaButton != g_eqHotMediaButton ||
                hotBand != g_eqHotBand || hotPreset != g_eqHotPreset ||
                hotDelete != g_eqHotPresetDelete) {
                g_eqHotMediaButton = hotMediaButton;
                g_eqHotBand = hotBand;
                g_eqHotPreset = hotPreset;
                g_eqHotPresetDelete = hotDelete;
                RenderEqPopup(hwnd);
            }
        }

        TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
        TrackMouseEvent(&tme);
        return 0;
    }
    case WM_MOUSELEAVE:
        if (g_eqDraggingBand < 0 &&
            (g_eqHotPageButton || g_eqHotMediaButton != -1 ||
             g_eqHotBand != -1 || g_eqHotPreset != -1 ||
             g_eqHotPresetDelete != -1 || g_eqHotLyricsLine != -1)) {
            g_eqHotPageButton = false;
            g_eqHotMediaButton = -1;
            g_eqHotBand = -1;
            g_eqHotPreset = -1;
            g_eqHotPresetDelete = -1;
            g_eqHotLyricsLine = -1;
            RenderEqPopup(hwnd);
        }
        return 0;
    case WM_MOUSEWHEEL: {
        if (!g_eqPageAnimationActive) {
            const int notches = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            if (notches != 0 && g_eqPageAnimationTarget >= 0.5f) {
                EqSetPopupSettingsScrollOffset(g_eqSettingsScrollOffset - notches * 56);
                RenderEqPopup(hwnd);
                return 0;
            }

            if (g_eqPageAnimationTarget < 0.5f) {
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ScreenToClient(hwnd, &pt);
                const double scale = EqGetPopupDpiScale();
                const int x = static_cast<int>(std::lround(pt.x / scale));
                const int y = static_cast<int>(std::lround(pt.y / scale));
                const EqLayoutWidgetState* lyricsWidget = nullptr;
                if (EqPointInLyricsWidget(x, y, lyricsWidget)) {
                    if (notches != 0) {
                        // Treat every wheel action as manual scrolling, even when
                        // the content is already at the top/bottom edge.
                        PauseEqLyricsAutoFocus();
                        ScrollEqLyricsByPixels(*lyricsWidget, -notches * 3 * 28);
                        RenderEqPopup(hwnd);
                    }
                    return 0;
                }
            }
        }
        break;
    }
    case WM_LBUTTONDOWN: {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);

        if (g_eqPageAnimationActive)
            return 0;

        const bool settingsPage = g_eqPageAnimationTarget >= 0.5f;
        if (HitTestEqPageButton(x, y)) {
            EqStartPageAnimation(hwnd, !settingsPage);
            return 0;
        }

        if (settingsPage) {
            const EqLayoutBuilderView view = GetEqLayoutBuilderView();
            const double builderDpiScale = EqGetPopupDpiScale();
            const int bx = static_cast<int>(std::lround(static_cast<double>(x) / builderDpiScale));
            const int by = static_cast<int>(std::lround(static_cast<double>(y) / builderDpiScale)) + g_eqSettingsScrollOffset;
            const bool inCanvas = PtInRect(&view.canvas, POINT{bx, by}) != FALSE;

            {
                const RECT layoutButton = EqGetDesktopLayoutButtonRect();
                if (PtInRect(&layoutButton, POINT{bx, by})) {
                    // The popup is owned by the taskbar/XAML thread on Windows 11,
                    // but the layout editor must be owned by the overlay thread so
                    // its timer, rendering and cleanup stay on one thread.
                    DestroyEqPopup();
                    HWND overlay = g_overlayWakeHwnd.load(std::memory_order_acquire);
                    if (overlay && IsWindow(overlay)) {
                        if (!PostMessageW(overlay, WM_VIZ_OPEN_LAYOUT_EDITOR, 0, 0))
                            Wh_Log(L"EqPopupProc: failed to post layout editor request");
                    } else {
                        Wh_Log(L"EqPopupProc: overlay window unavailable for layout editor");
                    }
                    return 0;
                }
            }

            if (EqGetPopupSettingsScrollMax() > 0) {
                const double scrollbarDpiScale = EqGetPopupDpiScale();
                const int logicalScrollbarX = static_cast<int>(std::lround(x / scrollbarDpiScale));
                const int logicalScrollbarY = static_cast<int>(std::lround(y / scrollbarDpiScale));
                const RECT track = EqGetPopupPlacementScrollbarTrack();
                const RECT thumb = EqGetPopupPlacementScrollbarThumb();
                if (PtInRect(&track, POINT{logicalScrollbarX, logicalScrollbarY})) {
                    if (PtInRect(&thumb, POINT{logicalScrollbarX, logicalScrollbarY})) {
                        g_eqSettingsScrollbarDragging = true;
                        g_eqSettingsScrollbarDragStartY = y;
                        g_eqSettingsScrollbarStartOffset = g_eqSettingsScrollOffset;
                        SetCapture(hwnd);
                    } else {
                        const int direction = logicalScrollbarY < thumb.top ? -1 : 1;
                        EqSetPopupSettingsScrollOffset(g_eqSettingsScrollOffset + direction * (EQ_POPUP_HEIGHT - 70));
                        RenderEqPopup(hwnd);
                    }
                    return 0;
                }
            }

            // Format buttons live in the scrollable content area. Convert the
            // mouse coordinates to the same logical space used for drawing.
            const double settingsDpiScale = EqGetPopupDpiScale();
            const int logicalMouseX = static_cast<int>(std::lround(x / settingsDpiScale));
            const int logicalMouseY = static_cast<int>(std::lround((y + g_eqSettingsScrollOffset) / settingsDpiScale));
            for (int aspect = 0; aspect < EQ_POPUP_ASPECT_COUNT; ++aspect) {
                const RECT button = EqGetPopupPlacementAspectButtonRect(aspect);
                if (PtInRect(&button, POINT{logicalMouseX, logicalMouseY})) {
                    if (g_eqPopupPlacementAspect != aspect) {
                        g_eqPopupPlacementAspect = aspect;
                        SaveEqPopupPlacementSettings();
                    }
                    RenderEqPopup(hwnd);
                    return 0;
                }
            }

            const EqPopupPlacementView placementView = GetEqPopupPlacementView();
            const double placementScale = EqGetPopupDpiScale();
            const int placementHitX = static_cast<int>(std::lround(x / placementScale));
            const int placementHitY = static_cast<int>(std::lround((y + g_eqSettingsScrollOffset) / placementScale));
            if (PtInRect(&placementView.canvas, POINT{placementHitX, placementHitY})) {
                const int target = EqPopupPlacementHitTarget(placementHitX, placementHitY);
                if (target != EQ_POPUP_PLACEMENT_DRAG_NONE) {
                    auto& placement = g_eqPopupPlacement[static_cast<size_t>(g_eqPopupPlacementAspect)];
                    POINT cursor{};
                    GetCursorPos(&cursor);
                    g_eqPopupPlacementDragStartCursor = cursor;
                    if (target == EQ_POPUP_PLACEMENT_DRAG_POPUP) {
                        const POINT popupCenter = EqGetPopupPlacementCenterNormalized(placement);
                        g_eqPopupPlacementDragStartCenter = popupCenter;
                    } else {
                        g_eqPopupPlacementDragStartCross = POINT{placement.crossX, placement.crossY};
                    }
                    // Keep these legacy offsets zeroed. Dragging now uses the
                    // cursor-delta model above, which treats X and Y identically.
                    g_eqPopupPlacementDragOffsetX = 0;
                    g_eqPopupPlacementDragOffsetY = 0;
                    g_eqPopupPlacementDragTarget = target;
                    SetCapture(hwnd);
                    return 0;
                }
            }

            if (g_eqBuilderResizeId >= 0) {
                if (auto* w = EqGetLayoutWidget(g_eqBuilderResizeId)) {
                    const int dx = bx - g_eqBuilderDragStartX;
                    const int dy = by - g_eqBuilderDragStartY;
                    int nx=w->x, ny=w->y, nw=g_eqBuilderOriginalW, nh=g_eqBuilderOriginalH;
                    if (g_eqBuilderResizeCorner==0) { nx=g_eqBuilderOriginalX+dx; ny=g_eqBuilderOriginalY+dy; nw=g_eqBuilderOriginalW-dx; nh=g_eqBuilderOriginalH-dy; }
                    else if (g_eqBuilderResizeCorner==1) { ny=g_eqBuilderOriginalY+dy; nw=g_eqBuilderOriginalW+dx; nh=g_eqBuilderOriginalH-dy; }
                    else if (g_eqBuilderResizeCorner==2) { nx=g_eqBuilderOriginalX+dx; nw=g_eqBuilderOriginalW-dx; nh=g_eqBuilderOriginalH+dy; }
                    else { nw=g_eqBuilderOriginalW+dx; nh=g_eqBuilderOriginalH+dy; }
                    w->x=EqSnap10(nx); w->y=EqSnap10(ny); w->w=EqSnap10(nw); w->h=EqSnap10(nh); ClampEqLayoutWidget(*w);
                    RenderEqPopup(hwnd);
                }
                return 0;
            }

            // Rotation and delete handles.
            if (g_eqBuilderSelectedId >= 0) {
                if (auto* selected=EqGetLayoutWidget(g_eqBuilderSelectedId)) {
                    const RECT sr=EqBuilderWidgetScreenRect(view,*selected);
                    const RECT deleteRect = EqBuilderDeleteRect(sr);
                    if (selected->id == EQ_LAYOUT_EQ) {
                        const RECT rotateRect = EqBuilderRotateRect(sr);
                        if (PtInRect(&rotateRect,POINT{bx,by})) {
                            selected->rotation=(selected->rotation+90)%360;
                            SaveEqLayoutSettings(); RenderEqPopup(hwnd); return 0;
                        }
                    }
                    if (PtInRect(&deleteRect,POINT{bx,by})) {
                        const int deletedId = selected->id;
                        selected->present=false;
                        g_eqBuilderSelectedId=-1;
                        if (deletedId == EQ_LAYOUT_LYRICS) {
                            g_eqLyricsScrollOffset = 0;
                            g_eqHotLyricsLine = -1;
                            g_eqLyricsScrollTrackKey.clear();
                            g_eqLyricsAutoFocusResumeTick = 0;
                        }
                        SaveEqLayoutSettings();
                        RenderEqPopup(hwnd);
                        return 0;
                    }
                    const int corner=EqBuilderHitHandle(sr,bx,by);
                    if (corner>=0) { g_eqBuilderResizeId=selected->id; g_eqBuilderResizeCorner=corner; g_eqBuilderDragStartX=bx; g_eqBuilderDragStartY=by; g_eqBuilderOriginalX=selected->x; g_eqBuilderOriginalY=selected->y; g_eqBuilderOriginalW=selected->w; g_eqBuilderOriginalH=selected->h; SetCapture(hwnd); return 0; }
                }
            }

            const int paletteId = [&]() -> int {
                for (int id=0;id<EQ_LAYOUT_WIDGET_COUNT;++id) { const RECT pr=EqBuilderPaletteRect(id); if (PtInRect(&pr,POINT{bx,by}) && !EqIsLayoutWidgetPresent(id)) return id; }
                return -1;
            }();
            if (paletteId>=0) { g_eqBuilderDragId=paletteId; SetCapture(hwnd); RenderEqPopup(hwnd); return 0; }

            const int hit = EqBuilderHitWidget(bx,by);
            if (hit>=0) {
                g_eqBuilderSelectedId=hit;
                const auto* w=EqGetLayoutWidgetConst(hit);
                if (w) { g_eqBuilderDragStartX=bx; g_eqBuilderDragStartY=by; g_eqBuilderOriginalX=w->x; g_eqBuilderOriginalY=w->y; g_eqBuilderRemoveOnDrop=false; g_eqBuilderResizeId=EQ_BUILDER_DRAG_NONE; SetCapture(hwnd); }
                RenderEqPopup(hwnd);
                return 0;
            }
            g_eqBuilderSelectedId=-1;
            RenderEqPopup(hwnd);
            return 0;
        }

        const EqLayoutWidgetState* lyricsWidget = nullptr;
        if (EqPointInLyricsWidget(x, y, lyricsWidget)) {
            std::shared_ptr<const std::vector<LyricsLine>> lines;
            {
                std::lock_guard<std::mutex> lock(g_lyricsMutex);
                lines = g_lyricsLines;
            }
            const EqLyricsMetrics metrics = GetEqLyricsMetrics(
                *lyricsWidget, lines.get());
            if (metrics.maxScroll > 0 &&
                PtInRect(&metrics.scrollbarTrack, POINT{x, y})) {
                const RECT thumb = GetEqLyricsScrollbarThumb(metrics);
                if (PtInRect(&thumb, POINT{x, y})) {
                    g_eqLyricsScrollbarDragging = true;
                    g_eqLyricsScrollbarDragStartY = y;
                    g_eqLyricsScrollbarStartOffset = g_eqLyricsScrollOffset;
                    PauseEqLyricsAutoFocus();
                    SetCapture(hwnd);
                } else {
                    const int direction = y < thumb.top ? -1 : 1;
                    ScrollEqLyricsByPixels(*lyricsWidget, direction *
                        std::max(1L, metrics.content.bottom -
                                 metrics.content.top - 28));
                }
            } else {
                const int lineIndex = GetEqLyricsLineAtPoint(*lyricsWidget, x, y);
                if (lineIndex >= 0)
                    SeekToEqLyricsLine(lineIndex);
            }
            RenderEqPopup(hwnd);
            return 0;
        }

        if (EqIsLayoutWidgetPresent(EQ_LAYOUT_MEDIA) && HitTestEqMediaTimeline(x, y)) {
            const EqMediaState state = GetEqMediaStateSnapshot();
            if (state.hasSession && state.durationSeconds > 0.0) {
                g_eqMediaSeeking = true;
                g_eqMediaSeekPreviewSeconds = EqMediaPositionFromMouseX(x);
                SetCapture(hwnd);
                RenderEqPopup(hwnd);
            }
            return 0;
        }

        if (EqIsLayoutWidgetPresent(EQ_LAYOUT_MEDIA)) {
            const int mediaButton = HitTestEqMediaButton(x, y);
            if (mediaButton >= 0) {
                switch (mediaButton) {
                case 0: EnqueueEqMediaCommand(EqMediaCommandType::Previous); break;
                case 1: EnqueueEqMediaCommand(EqMediaCommandType::Rewind5); break;
                case 2: EnqueueEqMediaCommand(EqMediaCommandType::PlayPause); break;
                case 3: EnqueueEqMediaCommand(EqMediaCommandType::Forward5); break;
                case 4: EnqueueEqMediaCommand(EqMediaCommandType::Next); break;
                }
                RenderEqPopup(hwnd);
                return 0;
            }
        }

        const int deletePreset = HitTestEqPresetDelete(x, y);
        if (deletePreset >= 0) {
            EqDeleteCustomPreset(hwnd, deletePreset);
            return 0;
        }
        const int preset = HitTestEqPreset(x, y);
        if (preset == EQ_PLUS_PRESET_HIT) {
            EqCreateCustomPreset(hwnd);
            return 0;
        }
        if (preset >= 0) {
            EqStartPresetAnimation(hwnd, preset);
            g_eqHotPreset = preset;
            g_eqHotBand = -1;
            RenderEqPopup(hwnd);
            return 0;
        }

        const int band = HitTestEqBand(x, y);
        if (band >= 0) {
            EqBeginManualBandEdit(hwnd);
            g_eqDraggingBand = band;
            g_eqHotBand = band;
            g_eqHotPreset = -1;
            SetEqGainFromMouse(band, x, y);
            SetCapture(hwnd);
            RenderEqPopup(hwnd);
        }
        return 0;
    }
    case WM_LBUTTONUP:
        if (g_eqSettingsScrollbarDragging) {
            g_eqSettingsScrollbarDragging = false;
            if (GetCapture() == hwnd)
                ReleaseCapture();
            RenderEqPopup(hwnd);
            return 0;
        }
        if (g_eqPopupPlacementDragTarget != EQ_POPUP_PLACEMENT_DRAG_NONE) {
            g_eqPopupPlacementDragTarget = EQ_POPUP_PLACEMENT_DRAG_NONE;
            g_eqPopupPlacementDragOffsetX = 0;
            g_eqPopupPlacementDragOffsetY = 0;
            SaveEqPopupPlacementSettings();
            if (GetCapture() == hwnd)
                ReleaseCapture();
            RenderEqPopup(hwnd);
            return 0;
        }
        if (g_eqLyricsScrollbarDragging) {
            g_eqLyricsScrollbarDragging = false;
            if (GetCapture() == hwnd)
                ReleaseCapture();
            RenderEqPopup(hwnd);
            return 0;
        }
        if (g_eqBuilderDragId >= 0) {
            const int dragId = g_eqBuilderDragId;
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            const double builderDpiScale = EqGetPopupDpiScale();
            const int bx = static_cast<int>(std::lround(static_cast<double>(x) / builderDpiScale));
            const int by = static_cast<int>(std::lround(static_cast<double>(y) / builderDpiScale));
            const EqLayoutBuilderView view = GetEqLayoutBuilderView();
            const bool inCanvas = PtInRect(&view.canvas, POINT{bx, by}) != FALSE;
            bool changed = false;
            if (inCanvas && !EqIsLayoutWidgetPresent(dragId)) {
                if (auto* w = EqGetLayoutWidget(dragId)) {
                    const int logicalX = (int)std::lround((bx - view.canvas.left) / view.scale);
                    const int logicalY = (int)std::lround((by - view.canvas.top) / view.scale);
                    w->x = EqSnap10(logicalX - w->w / 2);
                    w->y = EqSnap10(logicalY - w->h / 2);
                    w->present = true;
                    ClampEqLayoutWidget(*w);
                    g_eqBuilderSelectedId = w->id;
                    changed = true;
                }
            }
            g_eqBuilderDragId = EQ_BUILDER_DRAG_NONE;
            ReleaseCapture();
            if (changed) {
                SaveEqLayoutSettings();
                if (dragId == EQ_LAYOUT_LYRICS)
                    StartLyricsCapture();
            }
            RenderEqPopup(hwnd);
            return 0;
        }
        if (g_eqBuilderResizeId >= 0) {
            SaveEqLayoutSettings();
            g_eqBuilderResizeId=EQ_BUILDER_DRAG_NONE; g_eqBuilderResizeCorner=-1; ReleaseCapture(); RenderEqPopup(hwnd); return 0;
        }
        if (g_eqBuilderSelectedId >= 0 && GetCapture()==hwnd && g_eqPageAnimationTarget>=0.5f) {
            if (g_eqBuilderRemoveOnDrop) {
                const int removedId = g_eqBuilderSelectedId;
                if (auto* w=EqGetLayoutWidget(g_eqBuilderSelectedId)) w->present=false;
                if (removedId == EQ_LAYOUT_LYRICS) {
                    g_eqLyricsScrollOffset = 0;
                    g_eqHotLyricsLine = -1;
                    g_eqLyricsScrollTrackKey.clear();
                }
                SaveEqLayoutSettings(); g_eqBuilderSelectedId=-1;
            } else {
                SaveEqLayoutSettings();
            }
            g_eqBuilderRemoveOnDrop=false; ReleaseCapture(); RenderEqPopup(hwnd); return 0;
        }
        if (g_eqMediaSeeking) {
            g_eqMediaSeeking = false;
            ReleaseCapture();
            EnqueueEqMediaCommand(EqMediaCommandType::Seek, g_eqMediaSeekPreviewSeconds);
            RenderEqPopup(hwnd);
            return 0;
        }
        if (g_eqDraggingBand >= 0) {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            SetEqGainFromMouse(g_eqDraggingBand, x, y);
            SaveCustomEQSettings();
            g_eqDraggingBand = -1;
            ReleaseCapture();
            RenderEqPopup(hwnd);
            if (g_hwndOverlay)
                PostMessageW(g_hwndOverlay, WM_VIZ_AUDIO_WAKE, 0, 0);
        }
        return 0;
    case WM_CANCELMODE:
    case WM_CAPTURECHANGED:
        g_eqBuilderDragId=EQ_BUILDER_DRAG_NONE;
        g_eqBuilderResizeId=EQ_BUILDER_DRAG_NONE;
        g_eqBuilderResizeCorner=-1;
        g_eqBuilderRemoveOnDrop=false;
        if (g_eqLyricsScrollbarDragging) {
            g_eqLyricsScrollbarDragging=false;
            if (GetCapture() == hwnd)
                ReleaseCapture();
        }
        if (g_eqSettingsScrollbarDragging) {
            g_eqSettingsScrollbarDragging=false;
            if (GetCapture() == hwnd)
                ReleaseCapture();
        }
        if (g_eqPopupPlacementDragTarget != EQ_POPUP_PLACEMENT_DRAG_NONE) {
            g_eqPopupPlacementDragTarget = EQ_POPUP_PLACEMENT_DRAG_NONE;
            g_eqPopupPlacementDragOffsetX = 0;
            g_eqPopupPlacementDragOffsetY = 0;
        }
        if (g_eqMediaSeeking) {
            g_eqMediaSeeking = false;
        }
        if (g_eqDraggingBand >= 0) {
            SaveCustomEQSettings();
            g_eqDraggingBand = -1;
        }
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_DPICHANGED: {
        const RECT* suggested = reinterpret_cast<const RECT*>(lParam);
        if (suggested) {
            SetWindowPos(hwnd, nullptr,
                         suggested->left, suggested->top,
                         suggested->right - suggested->left,
                         suggested->bottom - suggested->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
        RenderEqPopup(hwnd);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(hwnd, kEqMediaRefreshTimerId);
        KillTimer(hwnd, kEqPopupPageAnimationTimerId);
        g_eqPageAnimationActive = false;
        if (g_eqMediaSeeking) {
            g_eqMediaSeeking = false;
            ReleaseCapture();
        }
        return 0;
    case WM_CLOSE:
        DestroyEqPopup();
        return 0;
    case WM_NCHITTEST:
        return HTCLIENT;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool RegisterEqWindowClasses(HINSTANCE instance) {
    if (g_eqClassesRegistered) return true;
    WNDCLASSW popupClass{};
    popupClass.lpfnWndProc = EqPopupProc;
    popupClass.hInstance = instance;
    popupClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    popupClass.lpszClassName = kEqPopupClass;
    if (!RegisterClassW(&popupClass))
        return false;

    WNDCLASSW trayClass{};
    trayClass.lpfnWndProc = EqNativeTrayMessageProc;
    trayClass.hInstance = instance;
    trayClass.lpszClassName = kEqNativeTrayWindowClass;
    if (!RegisterClassW(&trayClass)) {
        UnregisterClassW(kEqPopupClass, instance);
        return false;
    }

    g_eqModuleHandle = instance;
    g_eqClassesRegistered = true;
    return true;
}
static void EqClosePopup();

static void DestroyEqPopup() {
    if (g_eqDraggingBand >= 0) {
        SaveCustomEQSettings();
        g_eqDraggingBand = -1;
        ReleaseCapture();
    }

    // Closing the popup must not leave the audio EQ halfway between two
    // presets. Finish the current transition before stopping its timer.
    if (g_eqPresetAnimationActive) {
        for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
            g_eqActiveGains[static_cast<size_t>(i)].store(
                g_eqPresetTargets[static_cast<size_t>(i)],
                std::memory_order_relaxed);
        }
    }
    SaveCustomEQSettings();

    HWND hwnd = g_eqPopupHwnd;
    g_eqPopupHwnd = nullptr;
    g_eqHotBand = -1;
    g_eqHotPreset = -1;
    g_eqBuilderSelectedId = -1;
    g_eqBuilderDragId = EQ_BUILDER_DRAG_NONE;
    g_eqBuilderResizeId = EQ_BUILDER_DRAG_NONE;
    g_eqBuilderRemoveOnDrop = false;
    g_eqPopupLogicalWidth = EQ_POPUP_WIDTH;
    g_eqPopupLogicalHeight = EQ_POPUP_HEIGHT;
    g_eqPopupContentOffsetX = 0;
    g_eqPopupContentOffsetY = 0;
    EqStopPresetAnimation(hwnd);
    EqStopPageAnimation(hwnd);
    g_eqPageAnimationProgress = 0.0f;
    g_eqPageAnimationStart = 0.0f;
    g_eqPageAnimationTarget = 0.0f;
    g_eqSettingsScrollOffset = 0;
    g_eqSettingsScrollbarDragging = false;
    g_eqPopupPlacementDragTarget = EQ_POPUP_PLACEMENT_DRAG_NONE;
    g_eqPopupPlacementDragOffsetX = 0;
    g_eqPopupPlacementDragOffsetY = 0;

    if (hwnd && IsWindow(hwnd))
        DestroyWindow(hwnd);
}

// -----------------------------------------------------------------------------
// Custom EQ taskbar integration.
//
// Windows 11 uses the existing native XAML tray integration. Windows 10 uses
// Shell_NotifyIcon, which puts the EQ icon into the real notification area
// instead of creating a separate taskbar window. This keeps the icon under
// Explorer's normal tray layout/visibility management.
// -----------------------------------------------------------------------------
static bool g_eqIsWindows11 = false;
static DWORD g_eqWindowsBuildNumber = 0;

struct EqRtlOsVersionInfo {
    ULONG dwOSVersionInfoSize;
    ULONG dwMajorVersion;
    ULONG dwMinorVersion;
    ULONG dwBuildNumber;
    ULONG dwPlatformId;
    WCHAR szCSDVersion[128];
};
using EqRtlGetVersion_t = LONG (WINAPI*)(EqRtlOsVersionInfo*);

static bool EqDetectWindows11Once() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto rtlGetVersion = ntdll
        ? reinterpret_cast<EqRtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion"))
        : nullptr;

    EqRtlOsVersionInfo version{};
    version.dwOSVersionInfoSize = sizeof(version);
    if (!rtlGetVersion || rtlGetVersion(&version) != 0) {
        Wh_Log(L"EqDetectWindows11Once: RtlGetVersion failed; assuming Windows 10 path");
        g_eqWindowsBuildNumber = 0;
        return false;
    }

    g_eqWindowsBuildNumber = version.dwBuildNumber;
    const bool isWindows11 =
        version.dwMajorVersion >= 10 && version.dwBuildNumber >= 22000;
    Wh_Log(L"EqDetectWindows11Once: Windows %u.%u build %u -> %s path",
           version.dwMajorVersion, version.dwMinorVersion,
           version.dwBuildNumber,
           isWindows11 ? L"Windows 11 XAML" : L"Windows 10 native tray");
    return isWindows11;
}

static constexpr wchar_t kEqXamlButtonName[] = L"WindhawkVisualizerEQButton";
static constexpr wchar_t kEqXamlTrayGridName[] = L"SystemTrayFrameGrid";
static constexpr wchar_t kEqButtonGlyph[] = L"\xE9E9"; // Segoe Fluent Icons: Equalizer.

static bool g_eqTaskbarSymbolsHooked = false;
static std::atomic<bool> g_eqXamlInjectionReady{false};
[[clang::no_destroy]] static FrameworkElement g_eqXamlButton = nullptr;
[[clang::no_destroy]] static Grid g_eqXamlTrayGrid = nullptr;
[[clang::no_destroy]] static FrameworkElement g_eqXamlParent = nullptr;
static int g_eqXamlColumn = -1;
static winrt::event_token g_eqXamlClickToken{};
static bool g_eqXamlClickTokenValid = false;

static int EqPopupScalePx(int value, double scale) {
    return std::max(1, static_cast<int>(std::lround(
        static_cast<double>(value) * scale)));
}

static double EqGetPopupDpiScale() {
    UINT dpi = 96;
    if (g_eqPopupHwnd && IsWindow(g_eqPopupHwnd))
        dpi = GetDpiForWindow(g_eqPopupHwnd);
    else if (g_eqTaskbarHwnd && IsWindow(g_eqTaskbarHwnd))
        dpi = GetDpiForWindow(g_eqTaskbarHwnd);

    if (!dpi)
        dpi = 96;
    return std::max(0.25, static_cast<double>(dpi) / 96.0);
}


// Symbols copied from the proven Taskbar Fluent Media Player XAML integration
// path. They are used only to obtain the taskbar's existing XamlRoot.
using EqCTaskBandGetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using EqCSecondaryTaskBandGetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using EqTaskbarHostFrameHeight_t = int(WINAPI*)(void*);
using EqStdRefDecref_t = void(WINAPI*)(void*);
using EqTrayUIStartTaskbar_t = void(WINAPI*)(void*);

static EqCTaskBandGetTaskbarHost_t g_eqCTaskBandGetTaskbarHost = nullptr;
static EqCSecondaryTaskBandGetTaskbarHost_t g_eqCSecondaryTaskBandGetTaskbarHost = nullptr;
static EqTaskbarHostFrameHeight_t g_eqTaskbarHostFrameHeight = nullptr;
static EqStdRefDecref_t g_eqStdRefDecref = nullptr;
static EqTrayUIStartTaskbar_t g_eqTrayUIStartTaskbarOriginal = nullptr;
static void* g_eqCTaskBandTaskListWndSiteVftable = nullptr;
static void* g_eqCSecondaryTaskBandTaskListWndSiteVftable = nullptr;

static bool EqIsReadableMemoryRange(const void* address, size_t size) {
    if (!address || size == 0)
        return false;
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

static BOOL CALLBACK FindCurrentProcessTaskbarWndProc(HWND hwnd, LPARAM lParam) {
    if (!hwnd || !lParam)
        return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId())
        return TRUE;

    wchar_t cls[64]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return TRUE;

    // Always resolve the primary taskbar. Secondary taskbars are separate
    // shell windows and must not become the owner of the Media & EQ button.
    if (wcscmp(cls, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE;
    }

    return TRUE;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbar = nullptr;
    EnumWindows(FindCurrentProcessTaskbarWndProc, reinterpret_cast<LPARAM>(&taskbar));
    return taskbar;
}
static XamlRoot EqGetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!hTaskbarWnd) {
        Wh_Log(L"EqGetTaskbarXamlRoot: taskbar window is null");
        return nullptr;
    }

    wchar_t clsBuf[64] = {};
    GetClassNameW(hTaskbarWnd, clsBuf, ARRAYSIZE(clsBuf));
    const bool isSecondary = _wcsicmp(clsBuf, L"Shell_SecondaryTrayWnd") == 0;
    HWND hTaskSwWnd = isSecondary
        ? FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
        : (HWND)GetPropW(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        Wh_Log(L"EqGetTaskbarXamlRoot: taskbar worker window not found");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));
    if (!taskBand) {
        Wh_Log(L"EqGetTaskbarXamlRoot: taskband pointer unavailable");
        return nullptr;
    }

    void* expectedVftable = isSecondary
        ? g_eqCSecondaryTaskBandTaskListWndSiteVftable
        : g_eqCTaskBandTaskListWndSiteVftable;
    auto getTaskbarHost = isSecondary
        ? g_eqCSecondaryTaskBandGetTaskbarHost
        : g_eqCTaskBandGetTaskbarHost;
    if (!expectedVftable || !getTaskbarHost || !g_eqStdRefDecref) {
        Wh_Log(L"EqGetTaskbarXamlRoot: required taskbar symbols unavailable");
        return nullptr;
    }

    void* taskListWndSite = taskBand;
    constexpr int kMaxSlotsToScan = 20;
    for (int i = 0; i <= kMaxSlotsToScan; ++i) {
        if (!EqIsReadableMemoryRange(taskListWndSite, sizeof(void*))) {
            Wh_Log(L"EqGetTaskbarXamlRoot: taskband memory is not readable");
            return nullptr;
        }
        if (*(void**)taskListWndSite == expectedVftable)
            break;
        if (i == kMaxSlotsToScan) {
            Wh_Log(L"EqGetTaskbarXamlRoot: expected TaskBand vftable not found");
            return nullptr;
        }
        taskListWndSite = reinterpret_cast<void**>(taskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1] && g_eqStdRefDecref)
            g_eqStdRefDecref(taskbarHostSharedPtr[1]);
        Wh_Log(L"EqGetTaskbarXamlRoot: TaskbarHost unavailable");
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0;
    bool frameHeightPatternRecognized = false;

#if defined(_M_X64) || defined(__x86_64__)
    if (g_eqTaskbarHostFrameHeight) {
        const BYTE* b = reinterpret_cast<const BYTE*>(g_eqTaskbarHostFrameHeight);
        if (EqIsReadableMemoryRange(b, 8) &&
            b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC &&
            b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 &&
            b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
            frameHeightPatternRecognized = true;
        }
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    if (g_eqTaskbarHostFrameHeight) {
        const DWORD* p = reinterpret_cast<const DWORD*>(g_eqTaskbarHostFrameHeight);
        if (EqIsReadableMemoryRange(p, sizeof(DWORD) * 4) &&
            p[0] == 0xD503237F &&
            (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
            frameHeightPatternRecognized = true;
        }
    }
#else
    taskbarElementIUnknownOffset = 0x10;
    frameHeightPatternRecognized = true;
#endif

    if (!frameHeightPatternRecognized) {
        Wh_Log(L"EqGetTaskbarXamlRoot: TaskbarHost::FrameHeight pattern not recognized");
        if (taskbarHostSharedPtr[1] && g_eqStdRefDecref)
            g_eqStdRefDecref(taskbarHostSharedPtr[1]);
        Wh_Log(L"EqGetTaskbarXamlRoot: taskbarElement offset pattern unavailable");
        return nullptr;
    }

    if (!EqIsReadableMemoryRange(
            static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
                taskbarElementIUnknownOffset,
            sizeof(IUnknown*))) {
        if (taskbarHostSharedPtr[1] && g_eqStdRefDecref)
            g_eqStdRefDecref(taskbarHostSharedPtr[1]);
        Wh_Log(L"EqGetTaskbarXamlRoot: TaskbarHost element pointer is not readable");
        return nullptr;
    }

    IUnknown* taskbarElementIUnknown = *reinterpret_cast<IUnknown**>(
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) + taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        if (taskbarHostSharedPtr[1] && g_eqStdRefDecref)
            g_eqStdRefDecref(taskbarHostSharedPtr[1]);
        Wh_Log(L"EqGetTaskbarXamlRoot: taskbar XAML element is null");
        return nullptr;
    }

    FrameworkElement taskbarElement{nullptr};
    HRESULT hr = taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarElement));
    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (taskbarHostSharedPtr[1] && g_eqStdRefDecref)
        g_eqStdRefDecref(taskbarHostSharedPtr[1]);
    if (FAILED(hr) || !result)
        Wh_Log(L"EqGetTaskbarXamlRoot: QueryInterface/XamlRoot failed (hr=0x%08X)", static_cast<unsigned>(hr));
    return SUCCEEDED(hr) ? result : nullptr;
}

using EqWindowThreadProc = void(*)(void*);
static bool EqRunFromWindowThread(HWND hWnd, EqWindowThreadProc proc, void* param) {
    if (!hWnd || !proc)
        return false;
    static const UINT kMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_EQVisualizer");
    struct Payload { EqWindowThreadProc proc; void* param; } payload{proc, param};
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid)
        return false;
    if (tid == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                static const UINT kM =
                    RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_EQVisualizer");
                if (cwp->message == kM) {
                    auto* p = reinterpret_cast<Payload*>(cwp->lParam);
                    if (p && p->proc)
                        p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }, nullptr, tid);
    if (!hook)
        return false;

    SendMessageW(hWnd, kMsg, 0, reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hook);
    return true;
}

static FrameworkElement EqFindChildByName(
    FrameworkElement const& root, const wchar_t* name) {
    if (!root || !name)
        return nullptr;
    try {
        auto children = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < children; ++i) {
            auto child = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (!child)
                continue;
            if (child.Name() == name)
                return child;
            if (auto nested = EqFindChildByName(child, name))
                return nested;
        }
    } catch (...) {
    }
    return nullptr;
}

static bool EqGetXamlButtonScreenRect(Button const& button, RECT* outRect) {
    if (!button || !outRect || !g_eqTaskbarHwnd || !IsWindow(g_eqTaskbarHwnd))
        return false;

    try {
        auto root = button.XamlRoot();
        if (!root)
            return false;
        auto content = root.Content().try_as<FrameworkElement>();
        if (!content)
            return false;

        const auto point = button.TransformToVisual(content).TransformPoint(
            winrt::Windows::Foundation::Point{0.0f, 0.0f});
        const double scale = std::max(0.25, static_cast<double>(root.RasterizationScale()));
        const double bw = std::max(1.0, button.ActualWidth()) * scale;
        const double bh = std::max(1.0, button.ActualHeight()) * scale;

        RECT taskbarRect{};
        if (!GetWindowRect(g_eqTaskbarHwnd, &taskbarRect))
            return false;

        outRect->left = taskbarRect.left + static_cast<int>(std::lround(point.X * scale));
        outRect->top = taskbarRect.top + static_cast<int>(std::lround(point.Y * scale));
        outRect->right = outRect->left + static_cast<int>(std::lround(bw));
        outRect->bottom = outRect->top + static_cast<int>(std::lround(bh));
        return true;
    } catch (...) {
        return false;
    }
}

static NOTIFYICONIDENTIFIER EqMakeNativeTrayIdentifier() {
    NOTIFYICONIDENTIFIER identifier{};
    identifier.cbSize = sizeof(identifier);
    identifier.uID = kEqNativeTrayIconId;
    identifier.guidItem = kEqNativeTrayGuid;
    return identifier;
}

static bool EqGetNativeTrayButtonScreenRect(RECT* outRect) {
    if (!outRect || !g_eqNativeTrayRegistered)
        return false;

    const auto identifier = EqMakeNativeTrayIdentifier();
    return SUCCEEDED(Shell_NotifyIconGetRect(&identifier, outRect));
}

static bool EqIsCursorOverNativeTrayButton() {
    RECT buttonRect{};
    if (!EqGetNativeTrayButtonScreenRect(&buttonRect))
        return false;

    POINT cursor{};
    if (!GetCursorPos(&cursor))
        return false;

    return PtInRect(&buttonRect, cursor) != FALSE;
}

static bool EqIsCursorOverXamlButton() {
    if (!g_eqXamlButton)
        return false;

    RECT buttonRect{};
    if (!EqGetXamlButtonScreenRect(g_eqXamlButton.try_as<Button>(), &buttonRect))
        return false;

    POINT cursor{};
    if (!GetCursorPos(&cursor))
        return false;

    return PtInRect(&buttonRect, cursor) != FALSE;
}

static bool EqIsCursorOverTrayButton() {
    return g_eqIsWindows11
        ? EqIsCursorOverXamlButton()
        : EqIsCursorOverNativeTrayButton();
}

static void EqClosePopup() {
    HWND hwnd = g_eqPopupHwnd;
    if (!hwnd || !IsWindow(hwnd))
        return;

    const DWORD ownerThreadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!ownerThreadId)
        return;

    if (ownerThreadId == GetCurrentThreadId())
        DestroyEqPopup();
    else
        SendMessageW(hwnd, WM_CLOSE, 0, 0);
}

static void EqShowPopupForTrayRect(const RECT& buttonRect) {
    try {
        if (g_eqPopupHwnd && IsWindow(g_eqPopupHwnd)) {
            EqClosePopup();
            return;
        }

        if (!g_eqPopupPlacementLoaded)
            LoadEqPopupPlacementSettings();

        const double scale = EqGetPopupDpiScale();
        const int popupWidth = EqPopupScalePx(EQ_POPUP_WIDTH, scale);
        const int popupHeight = EqPopupScalePx(EQ_POPUP_HEIGHT, scale);

        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        HMONITOR monitor = MonitorFromRect(&buttonRect, MONITOR_DEFAULTTONEAREST);
        RECT workArea{};
        if (monitor && GetMonitorInfoW(monitor, &mi)) {
            workArea = mi.rcWork;
        } else {
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
        }
        if (workArea.right <= workArea.left || workArea.bottom <= workArea.top)
            return;

        // Create and place the full logical popup first. The layout-cropping
        // pass then removes unused edges while preserving the corresponding
        // screen coordinates of the original 700x530 layout canvas. Do not
        // re-center the cropped window afterwards: doing so makes the visible
        // content drift whenever the used layout occupies only one side of the
        // canvas (for example, Lyrics placed at the far right).

        g_eqPopupLogicalWidth = EQ_POPUP_WIDTH;
        g_eqPopupLogicalHeight = EQ_POPUP_HEIGHT;
        g_eqPopupContentOffsetX = 0;
        g_eqPopupContentOffsetY = 0;

        g_eqPopupHwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_LAYERED,
            kEqPopupClass,
            L"Media & EQ",
            WS_POPUP,
            workArea.left, workArea.top, popupWidth, popupHeight,
            nullptr, nullptr, g_eqModuleHandle, nullptr);
        if (!g_eqPopupHwnd)
            return;

        ApplyEqPopupWindowAttributes(g_eqPopupHwnd);
        ShowWindow(g_eqPopupHwnd, SW_SHOWNOACTIVATE);
        SetWindowPos(g_eqPopupHwnd, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
        SetForegroundWindow(g_eqPopupHwnd);
        SetFocus(g_eqPopupHwnd);

        // Apply the configured position while the window still represents
        // the full 700x530 logical canvas. Cropping below then shifts the
        // window to the used section bounds without changing that placement.
        EqApplyConfiguredPopupPlacement(g_eqPopupHwnd, workArea);
        EqCropPopupToLayout(g_eqPopupHwnd);
        RenderEqPopup(g_eqPopupHwnd);

    } catch (...) {
        g_eqPopupHwnd = nullptr;
    }
}

static void EqShowPopupForXamlButton(Button const& button) {
    if (!g_eqIsWindows11 || !button)
        return;

    RECT buttonRect{};
    if (!EqGetXamlButtonScreenRect(button, &buttonRect))
        return;

    EqShowPopupForTrayRect(buttonRect);
}

static void EqShowPopupForNativeTrayButton() {
    if (g_eqIsWindows11)
        return;

    RECT buttonRect{};
    if (!EqGetNativeTrayButtonScreenRect(&buttonRect))
        return;

    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (!taskbar || !IsWindow(taskbar))
        return;

    g_eqTaskbarHwnd = taskbar;
    EqShowPopupForTrayRect(buttonRect);
}

static bool EqIsCurrentXamlButtonAlive(FrameworkElement const& root) {
    if (!root || !g_eqXamlButton)
        return false;

    try {
        auto current = EqFindChildByName(root, kEqXamlButtonName);
        return current && current == g_eqXamlButton;
    } catch (...) {
        return false;
    }
}

static FrameworkElement EqFindDirectChildContaining(
    FrameworkElement const& parent, FrameworkElement const& target) {
    if (!parent || !target)
        return nullptr;
    try {
        auto children = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < children; ++i) {
            auto child = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(parent, i)
                .try_as<FrameworkElement>();
            if (!child)
                continue;
            if (child == target)
                return child;

            auto targetParent = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(target)
                .try_as<FrameworkElement>();
            if (targetParent == child)
                return child;

            // Walk upward from target until we reach this parent.
            auto cursor = targetParent;
            for (int depth = 0; cursor && depth < 64; ++depth) {
                if (cursor == child)
                    return child;
                if (cursor == parent)
                    break;
                cursor = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(cursor)
                    .try_as<FrameworkElement>();
            }
        }
    } catch (...) {
    }
    return nullptr;
}

static HICON EqCreateNativeFluentEqualizerIcon() {
    constexpr int kIconSize = 32;
    Gdiplus::Bitmap bitmap(
        kIconSize, kIconSize, PixelFormat32bppARGB);
    if (bitmap.GetLastStatus() != Gdiplus::Ok)
        return nullptr;

    Gdiplus::Graphics graphics(&bitmap);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

    Gdiplus::FontFamily fontFamily(L"Segoe MDL2 Assets");
    if (fontFamily.GetLastStatus() != Gdiplus::Ok)
        return nullptr;

    Gdiplus::Font font(
        &fontFamily, 28.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    // Keep the tray glyph white on both light and dark Windows 10 tray themes.
    // The notification area background is controlled by Explorer; the EQ icon
    // should remain visually consistent with the white Fluent/XAML button used
    // by the Windows 11 implementation.
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
    const wchar_t glyph[] = {L'\xE9E9', L'\0'};
    const Gdiplus::RectF rect(
        0.0f, 0.0f, static_cast<Gdiplus::REAL>(kIconSize),
        static_cast<Gdiplus::REAL>(kIconSize));
    graphics.DrawString(glyph, -1, &font, rect, &format, &brush);

    HICON icon = nullptr;
    if (bitmap.GetHICON(&icon) != Gdiplus::Ok)
        return nullptr;
    return icon;
}

static bool EqModifyNativeTrayIcon() {
    if (!g_eqNativeTrayMessageHwnd || !g_eqNativeTrayRegistered)
        return false;

    HICON icon = EqCreateNativeFluentEqualizerIcon();
    if (!icon) {
        Wh_Log(L"EqModifyNativeTrayIcon: failed to create icon");
        return false;
    }

    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_eqNativeTrayMessageHwnd;
    nid.uID = kEqNativeTrayIconId;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID;
    nid.hIcon = icon;
    nid.guidItem = kEqNativeTrayGuid;
    wcscpy_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Media & EQ");

    const BOOL ok = Shell_NotifyIconW(NIM_MODIFY, &nid);
    DestroyIcon(icon);
    if (!ok) {
        Wh_Log(L"EqModifyNativeTrayIcon: NIM_MODIFY failed, error=%lu",
               GetLastError());
    }
    return ok != FALSE;
}

static bool EqAddNativeTrayIcon() {
    if (!g_eqNativeTrayMessageHwnd)
        return false;
    if (!GetSettingsSnapshot().showMediaEqTrayButton)
        return false;
    if (g_eqNativeTrayRegistered)
        return true;

    HICON icon = EqCreateNativeFluentEqualizerIcon();
    if (!icon) {
        Wh_Log(L"EqAddNativeTrayIcon: failed to create icon");
        return false;
    }

    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_eqNativeTrayMessageHwnd;
    nid.uID = kEqNativeTrayIconId;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID;
    nid.uCallbackMessage = WM_EQ_NATIVE_TRAY;
    nid.hIcon = icon;
    nid.guidItem = kEqNativeTrayGuid;
    wcscpy_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Media & EQ");

    // Remove only our own fixed-GUID icon before re-adding, which prevents a
    // stale icon from a previous Windhawk hot-reload from becoming a ghost.
    Shell_NotifyIconW(NIM_DELETE, &nid);

    const BOOL ok = Shell_NotifyIconW(NIM_ADD, &nid);
    DestroyIcon(icon);

    if (!ok) {
        Wh_Log(L"EqAddNativeTrayIcon: NIM_ADD failed, error=%lu",
               GetLastError());
        g_eqNativeTrayRegistered = false;
        return false;
    }

    g_eqNativeTrayRegistered = true;
    g_eqNativeTrayRetryCount = 0;
    if (g_eqNativeTrayMessageHwnd)
        KillTimer(g_eqNativeTrayMessageHwnd, kEqNativeTrayRetryTimerId);
    Wh_Log(L"EqAddNativeTrayIcon: registered native Win10 notification-area icon");
    return true;
}

static void EqScheduleNativeTrayIconRetry(UINT delayMs = kEqNativeTrayRetryDelayMs) {
    if (g_eqIsWindows11 || !g_eqNativeTrayMessageHwnd ||
        !g_running.load(std::memory_order_acquire) ||
        g_eqNativeTrayRegistered) {
        return;
    }

    if (g_eqNativeTrayRetryCount >= kEqNativeTrayMaxRetryCount) {
        Wh_Log(L"EqScheduleNativeTrayIconRetry: retry limit reached");
        return;
    }

    ++g_eqNativeTrayRetryCount;
    if (!SetTimer(g_eqNativeTrayMessageHwnd, kEqNativeTrayRetryTimerId,
                  std::max(1u, delayMs), nullptr)) {
        Wh_Log(L"EqScheduleNativeTrayIconRetry: SetTimer failed, error=%lu",
               GetLastError());
    } else {
        Wh_Log(L"EqScheduleNativeTrayIconRetry: scheduled retry %d/%d in %u ms",
               g_eqNativeTrayRetryCount, kEqNativeTrayMaxRetryCount, delayMs);
    }
}

static void EqRemoveNativeTrayIcon() {
    if (g_eqNativeTrayMessageHwnd)
        KillTimer(g_eqNativeTrayMessageHwnd, kEqNativeTrayRetryTimerId);
    g_eqNativeTrayRetryCount = 0;
    if (g_eqNativeTrayMessageHwnd) {
        NOTIFYICONDATAW nid{};
        nid.cbSize = sizeof(nid);
        nid.hWnd = g_eqNativeTrayMessageHwnd;
        nid.uID = kEqNativeTrayIconId;
        nid.uFlags = NIF_GUID;
        nid.guidItem = kEqNativeTrayGuid;
        Shell_NotifyIconW(NIM_DELETE, &nid);
    }
    g_eqNativeTrayRegistered = false;
}

static LRESULT CALLBACK EqNativeTrayMessageProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_eqTaskbarCreatedMessage && msg == g_eqTaskbarCreatedMessage) {
        if (g_running.load(std::memory_order_acquire)) {
            // Explorer rebuilt the notification area, so the old icon is gone.
            // TaskbarCreated can arrive before the new tray is ready for NIM_ADD.
            // Retry a few times with a short one-shot timer instead of polling.
            g_eqNativeTrayRegistered = false;
            g_eqNativeTrayRetryCount = 0;
            if (GetSettingsSnapshot().showMediaEqTrayButton) {
                if (!EqAddNativeTrayIcon())
                    EqScheduleNativeTrayIconRetry();
            }
        }
        return 0;
    }

    if (msg == WM_TIMER && wParam == kEqNativeTrayRetryTimerId) {
        if (!g_eqIsWindows11 && g_running.load(std::memory_order_acquire)) {
            KillTimer(hwnd, kEqNativeTrayRetryTimerId);
            if (GetSettingsSnapshot().showMediaEqTrayButton &&
                !g_eqNativeTrayRegistered && !EqAddNativeTrayIcon())
                EqScheduleNativeTrayIconRetry();
        } else {
            KillTimer(hwnd, kEqNativeTrayRetryTimerId);
        }
        return 0;
    }

    if (msg == WM_EQ_NATIVE_TRAY &&
        wParam == kEqNativeTrayIconId &&
        !g_eqIsWindows11 &&
        g_running.load(std::memory_order_acquire)) {
        const UINT trayEvent = static_cast<UINT>(lParam);
        if (trayEvent == WM_LBUTTONDOWN) {
            EqShowPopupForNativeTrayButton();
            return 0;
        }
    }

    if (msg == WM_SETTINGCHANGE && !g_eqIsWindows11 &&
        g_eqNativeTrayRegistered) {
        EqModifyNativeTrayIcon();
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool EqCreateNativeTrayMessageWindow(HINSTANCE instance) {
    if (g_eqNativeTrayMessageHwnd && IsWindow(g_eqNativeTrayMessageHwnd))
        return true;

    if (!g_eqTaskbarCreatedMessage)
        g_eqTaskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    if (!g_eqTaskbarCreatedMessage) {
        Wh_Log(L"EqCreateNativeTrayMessageWindow: failed to register TaskbarCreated message");
        return false;
    }

    g_eqNativeTrayMessageHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW,
        kEqNativeTrayWindowClass,
        L"WindhawkVisualizerEQTray",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr, nullptr, instance, nullptr);
    if (!g_eqNativeTrayMessageHwnd) {
        Wh_Log(L"EqCreateNativeTrayMessageWindow: CreateWindowEx failed, error=%lu",
               GetLastError());
        return false;
    }

    return true;
}

static void EqDestroyNativeTrayMessageWindowOnItsThread() {
    HWND hwnd = g_eqNativeTrayMessageHwnd;
    if (hwnd && IsWindow(hwnd))
        DestroyWindow(hwnd);
    g_eqNativeTrayMessageHwnd = nullptr;
}

static void EqEnsureNativeTrayIcon() {
    if (g_eqIsWindows11 || !g_eqNativeTrayMessageHwnd)
        return;

    if (!GetSettingsSnapshot().showMediaEqTrayButton) {
        if (g_eqNativeTrayRegistered)
            EqRemoveNativeTrayIcon();
        return;
    }

    if (!EqAddNativeTrayIcon())
        EqScheduleNativeTrayIconRetry();
}

static void EqResetXamlStateOnly() {
    g_eqXamlClickTokenValid = false;
    g_eqXamlColumn = -1;
    g_eqXamlButton = nullptr;
    g_eqXamlParent = nullptr;
    g_eqXamlTrayGrid = nullptr;
    g_eqXamlInjectionReady.store(false, std::memory_order_release);
}

static void EqRemoveXamlButtonOnUiThread() {
    try {
        if (g_eqXamlParent && g_eqXamlButton) {
            if (g_eqXamlClickTokenValid) {
                if (auto button = g_eqXamlButton.try_as<Button>())
                    button.Click(g_eqXamlClickToken);
            }

            auto panel = g_eqXamlParent.try_as<Panel>();
            if (panel) {
                for (uint32_t i = 0; i < panel.Children().Size(); ++i) {
                    auto child = panel.Children().GetAt(i).try_as<FrameworkElement>();
                    if (child == g_eqXamlButton) {
                        panel.Children().RemoveAt(i);
                        break;
                    }
                }
            }
        }
    } catch (...) {
    }

    EqResetXamlStateOnly();
    EqClosePopup();
}

static void EqSetupButtonCommon(Button const& button, winrt::event_token* tokenOut) {
    button.Name(kEqXamlButtonName);
    button.Width(EQ_BUTTON_WIDTH);
    button.Height(EQ_BUTTON_HEIGHT);
    button.MinWidth(EQ_BUTTON_WIDTH);
    button.MinHeight(EQ_BUTTON_HEIGHT);
    button.HorizontalAlignment(HorizontalAlignment::Center);
    button.VerticalAlignment(VerticalAlignment::Center);
    button.Padding({0, 0, 0, 0});
    button.BorderThickness({0, 0, 0, 0});
    button.Background(nullptr);
    button.IsHitTestVisible(true);
    button.IsTabStop(true);

    winrt::Windows::UI::Xaml::Controls::FontIcon icon;
    icon.Glyph(kEqButtonGlyph);
    icon.FontSize(18.0);
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    icon.VerticalAlignment(VerticalAlignment::Center);
    try {
        icon.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(L"Segoe Fluent Icons"));
    } catch (...) {
        try {
            icon.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(L"Segoe MDL2 Assets"));
        } catch (...) {
        }
    }
    const BYTE iconChannel = IsDarkThemeEnabled() ? 255 : 32;
    icon.Foreground(winrt::Windows::UI::Xaml::Media::SolidColorBrush(
        winrt::Windows::UI::Color{255, iconChannel, iconChannel, iconChannel}));
    button.Content(icon);

    ToolTip tooltip;
    tooltip.Content(winrt::box_value(winrt::hstring{L"Media & EQ"}));
    try {
        winrt::Windows::UI::Xaml::Controls::ToolTipService::SetToolTip(button, tooltip);
    } catch (...) {
    }

    auto clickToken = button.Click([button](
        winrt::Windows::Foundation::IInspectable const&,
        winrt::Windows::UI::Xaml::RoutedEventArgs const&) {
        EqShowPopupForXamlButton(button);
    });

    if (tokenOut)
        *tokenOut = clickToken;
}

static bool EqInsertIntoGrid(Grid const& trayGrid, Button const& button) {
    if (!trayGrid || !button)
        return false;

    int insertCol = -1;
    if (auto languageStack = EqFindChildByName(trayGrid, L"NonActivatableStack")) {
        insertCol = Grid::GetColumn(languageStack);
    } else if (auto notifyStack = EqFindChildByName(trayGrid, L"NotifyIconStack")) {
        insertCol = Grid::GetColumn(notifyStack) + 1;
    }

    const int columnCount = static_cast<int>(trayGrid.ColumnDefinitions().Size());
    if (insertCol < 0 || insertCol > columnCount)
        insertCol = columnCount;

    for (uint32_t i = 0; i < trayGrid.Children().Size(); ++i) {
        auto child = trayGrid.Children().GetAt(i).try_as<FrameworkElement>();
        if (!child)
            continue;
        const int column = Grid::GetColumn(child);
        if (column >= insertCol)
            Grid::SetColumn(child, column + 1);
    }

    winrt::Windows::UI::Xaml::Controls::ColumnDefinition newCol;
    newCol.Width({1.0, winrt::Windows::UI::Xaml::GridUnitType::Auto});
    trayGrid.ColumnDefinitions().InsertAt(insertCol, newCol);
    Grid::SetColumn(button, insertCol);
    trayGrid.Children().Append(button);
    g_eqXamlColumn = insertCol;
    return true;
}

static bool EqIsDescendantOf(
    FrameworkElement const& element, FrameworkElement const& ancestor) {
    if (!element || !ancestor)
        return false;

    try {
        auto cursor = element;
        for (int depth = 0; cursor && depth < 64; ++depth) {
            if (cursor == ancestor)
                return true;
            cursor = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(cursor)
                .try_as<FrameworkElement>();
        }
    } catch (...) {
    }
    return false;
}

static FrameworkElement EqFindCommonPanelParent(
    FrameworkElement const& root,
    FrameworkElement const& first,
    FrameworkElement const& second) {
    if (!root || !first || !second)
        return nullptr;

    try {
        auto cursor = first;
        for (int depth = 0; cursor && depth < 64; ++depth) {
            if (auto panel = cursor.try_as<Panel>()) {
                if (EqIsDescendantOf(second, panel))
                    return panel;
            }

            if (cursor == root)
                break;

            cursor = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(cursor)
                .try_as<FrameworkElement>();
        }
    } catch (...) {
    }
    return nullptr;
}

static bool EqInsertIntoStackPanel(
    FrameworkElement const& trayFrame,
    StackPanel const& stack,
    Button const& button,
    FrameworkElement const& notifyIconStack,
    FrameworkElement const& languageStack,
    FrameworkElement* insertionParentOut) {
    if (!trayFrame || !stack || !button)
        return false;

    // The desired location is between the notification/hidden-icon group and
    // the language/input group. On the new StackPanel-based tray, those groups
    // are often siblings under an inner container such as SystemTray.Stack.
    // Insert into that shared parent, immediately before the language group.
    auto commonParent = EqFindCommonPanelParent(
        trayFrame, notifyIconStack, languageStack);

    Panel targetPanel = commonParent ? commonParent.try_as<Panel>() : nullptr;
    if (!targetPanel)
        targetPanel = stack;

    uint32_t index = targetPanel.Children().Size();

    FrameworkElement anchor = nullptr;
    if (languageStack) {
        anchor = EqFindDirectChildContaining(targetPanel, languageStack);
    }
    if (!anchor && notifyIconStack) {
        anchor = EqFindDirectChildContaining(targetPanel, notifyIconStack);
    }

    if (anchor) {
        for (uint32_t i = 0; i < targetPanel.Children().Size(); ++i) {
            auto child = targetPanel.Children().GetAt(i).try_as<FrameworkElement>();
            if (child == anchor) {
                index = i;
                break;
            }
        }
    }

    targetPanel.Children().InsertAt(index, button);

    if (insertionParentOut)
        *insertionParentOut = targetPanel;

    return true;
}

static void EqInjectXamlButtonOnUiThread(HWND taskbar, XamlRoot xamlRoot) {
    if (!taskbar || !xamlRoot)
        return;

    try {
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root)
            return;

        // The outer SystemTrayFrameGrid is the stable named anchor across the
        // current rollout. Its runtime type may now be Grid OR StackPanel.
        auto trayFrame = EqFindChildByName(root, kEqXamlTrayGridName);
        if (!trayFrame) {
            Wh_Log(L"EqInjectXamlButton: SystemTrayFrameGrid not found");
            return;
        }

        auto trayGrid = trayFrame.try_as<Grid>();
        auto trayStack = trayFrame.try_as<StackPanel>();
        auto trayPanel = trayFrame.try_as<Panel>();
        if (!trayPanel) {
            Wh_Log(L"EqInjectXamlButton: SystemTrayFrameGrid is not a Panel");
            return;
        }

        // Remove only our own stale child from the currently selected container.
        for (int i = static_cast<int>(trayPanel.Children().Size()) - 1; i >= 0; --i) {
            auto child = trayPanel.Children().GetAt(i).try_as<FrameworkElement>();
            if (child && child.Name() == kEqXamlButtonName)
                trayPanel.Children().RemoveAt(i);
        }

        Button button;
        EqSetupButtonCommon(button, &g_eqXamlClickToken);

        bool inserted = false;
        FrameworkElement insertionParent = nullptr;
        if (trayGrid) {
            inserted = EqInsertIntoGrid(trayGrid, button);
            if (inserted)
                insertionParent = trayFrame;
        } else if (trayStack) {
            auto notifyIconStack = EqFindChildByName(root, L"NotifyIconStack");
            auto languageStack = EqFindChildByName(root, L"NonActivatableStack");
            inserted = EqInsertIntoStackPanel(
                trayFrame, trayStack, button, notifyIconStack, languageStack,
                &insertionParent);
        } else {
            trayPanel.Children().Append(button);
            insertionParent = trayFrame;
            inserted = true;
        }

        if (!insertionParent)
            insertionParent = trayFrame;


        if (!inserted) {
            Wh_Log(L"EqInjectXamlButton: failed to insert button");
            return;
        }

        g_eqTaskbarHwnd = taskbar;
        g_eqXamlParent = insertionParent;
        g_eqXamlTrayGrid = trayGrid;
        g_eqXamlButton = button;
        g_eqXamlClickTokenValid = true;
        g_eqXamlInjectionReady.store(true, std::memory_order_release);

        Wh_Log(L"EqInjectXamlButton: injected into %s#%s",
               trayGrid ? L"Grid" : (trayStack ? L"StackPanel" : L"Panel"),
               kEqXamlTrayGridName);
    } catch (...) {
        Wh_Log(L"EqInjectXamlButton: exception");
        EqRemoveXamlButtonOnUiThread();
    }
}

static void EqHideLegacyNativeButtons(HWND taskbar) {
    if (!taskbar || !IsWindow(taskbar))
        return;
    // Versions before the XAML rewrite created a native child with this class.
    // Do not DestroyWindow it here: its old WndProc may belong to an unloaded
    // module instance. Hiding it is safe and immediately removes any stale
    // visual while the taskbar rebuilds its tree.
    for (;;) {
        HWND old = FindWindowExW(taskbar, nullptr,
                                 L"WindhawkVisualizerEQButton", nullptr);
        if (!old || !IsWindow(old))
            break;
        ShowWindow(old, SW_HIDE);
        LONG_PTR style = GetWindowLongPtrW(old, GWL_STYLE);
        if (style & WS_VISIBLE)
            SetWindowLongPtrW(old, GWL_STYLE, style & ~static_cast<LONG_PTR>(WS_VISIBLE));
        // Avoid spinning forever if a third-party component instantly re-shows
        // the same stale HWND.
        break;
    }
}

static void EqEnsureXamlButton() {
    if (!g_eqIsWindows11 || !g_eqTaskbarSymbolsHooked)
        return;

    const bool showTrayButton =
        GetSettingsSnapshot().showMediaEqTrayButton;

    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (!taskbar || !IsWindow(taskbar))
        return;

    if (!showTrayButton) {
        if (!g_eqXamlInjectionReady.load(std::memory_order_acquire))
            return;

        struct Payload { HWND taskbar; } payload{taskbar};
        if (!EqRunFromWindowThread(taskbar, [](void* raw) {
            auto* p = static_cast<Payload*>(raw);
            if (!p || !p->taskbar)
                return;
            EqRemoveXamlButtonOnUiThread();
        }, &payload)) {
            Wh_Log(L"EqEnsureXamlButton: failed to remove disabled tray button");
        }
        return;
    }

    static ULONGLONG lastDeepCheckMs = 0;
    const ULONGLONG nowMs = GetTickCount64();

    // TrayUI::StartTaskbar is the primary immediate recovery path. The deep
    // reconciliation below is only a periodic safety net for XAML changes that
    // are not accompanied by that notification.
    if (g_eqXamlInjectionReady.load(std::memory_order_acquire) &&
        g_eqTaskbarHwnd == taskbar &&
        nowMs - lastDeepCheckMs < 30000) {
        return;
    }

    lastDeepCheckMs = nowMs;

    EqHideLegacyNativeButtons(taskbar);

    struct Payload { HWND taskbar; } payload{taskbar};
    if (!EqRunFromWindowThread(taskbar, [](void* raw) {
        auto* p = static_cast<Payload*>(raw);
        if (!p || !p->taskbar)
            return;

        try {
            auto root = EqGetTaskbarXamlRoot(p->taskbar);
            if (!root) {
                Wh_Log(L"EqEnsureXamlButton: XamlRoot unavailable");
                g_eqXamlInjectionReady.store(false, std::memory_order_release);
                return;
            }

            auto rootElement = root.Content().try_as<FrameworkElement>();
            if (!rootElement) {
                Wh_Log(L"EqEnsureXamlButton: XamlRoot content is not a FrameworkElement");
                g_eqXamlInjectionReady.store(false, std::memory_order_release);
                return;
            }

            if (g_eqTaskbarHwnd == p->taskbar &&
                EqIsCurrentXamlButtonAlive(rootElement)) {
                g_eqXamlInjectionReady.store(true, std::memory_order_release);
                return;
            }

            if (g_eqXamlButton)
                EqRemoveXamlButtonOnUiThread();
            else
                EqResetXamlStateOnly();

            EqInjectXamlButtonOnUiThread(p->taskbar, root);
        } catch (...) {
            Wh_Log(L"EqEnsureXamlButton: exception");
            g_eqXamlInjectionReady.store(false, std::memory_order_release);
        }
    }, &payload)) {
        Wh_Log(L"EqEnsureXamlButton: failed to marshal to taskbar UI thread");
    }

}

static void WINAPI EqTrayUIStartTaskbarHook(void* pThis) {
    if (g_eqTrayUIStartTaskbarOriginal)
        g_eqTrayUIStartTaskbarOriginal(pThis);

    if (!g_eqIsWindows11 || !g_running.load(std::memory_order_acquire))
        return;

    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (!taskbar)
        return;

    // StartTaskbar is called when Windows rebuilds the taskbar/tray XAML.
    // Immediately invalidate our cached object graph and either remove or
    // reinject our button on the taskbar UI thread instead of waiting for the
    // 1-second polling fallback.
    EqHideLegacyNativeButtons(taskbar);
    g_eqTaskbarHwnd = taskbar;
    const bool showTrayButton =
        GetSettingsSnapshot().showMediaEqTrayButton;
    EqRunFromWindowThread(taskbar, [](void* raw) {
        const bool show = raw != nullptr;
        EqRemoveXamlButtonOnUiThread();
        if (!show)
            return;
        auto root = EqGetTaskbarXamlRoot(g_eqTaskbarHwnd);
        if (root)
            EqInjectXamlButtonOnUiThread(g_eqTaskbarHwnd, root);
    }, showTrayButton ? reinterpret_cast<void*>(1) : nullptr);
}

static bool EqHookTaskbarSymbols() {
    HMODULE h = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h)
        return false;

    // Resolve all taskbar symbols in one call so the Windhawk symbol cache is
    // shared across the complete taskbar integration. Secondary-taskbar,
    // FrameHeight, and StartTaskbar symbols are optional across builds.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &g_eqCTaskBandTaskListWndSiteVftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &g_eqCTaskBandGetTaskbarHost},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &g_eqStdRefDecref},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &g_eqCSecondaryTaskBandTaskListWndSiteVftable, nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &g_eqCSecondaryTaskBandGetTaskbarHost, nullptr, true},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &g_eqTaskbarHostFrameHeight, nullptr, true},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &g_eqTrayUIStartTaskbarOriginal, EqTrayUIStartTaskbarHook, true},
    };

    if (!WindhawkUtils::HookSymbols(
            h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"EqHookTaskbarSymbols: taskbar symbols could not be resolved");
        return false;
    }

    // The primary TaskbarHost symbols are required. FrameHeight is also
    // required by EqGetTaskbarXamlRoot on supported x64/ARM64 builds.
    if (!g_eqCTaskBandTaskListWndSiteVftable ||
        !g_eqCTaskBandGetTaskbarHost ||
        !g_eqStdRefDecref ||
        !g_eqTaskbarHostFrameHeight) {
        Wh_Log(L"EqHookTaskbarSymbols: required taskbar symbols are missing");
        return false;
    }

    return true;
}

static void EqCleanupIntegration() {
    if (!g_eqIsWindows11) {
        HWND trayMessageWindow = g_eqNativeTrayMessageHwnd;
        if (trayMessageWindow && IsWindow(trayMessageWindow)) {
            EqRunFromWindowThread(trayMessageWindow, [](void*) {
                EqRemoveNativeTrayIcon();
                EqDestroyNativeTrayMessageWindowOnItsThread();
            }, nullptr);
        } else {
            g_eqNativeTrayRegistered = false;
            g_eqNativeTrayMessageHwnd = nullptr;
        }

        EqClosePopup();
        return;
    }

    HWND taskbar = g_eqTaskbarHwnd;
    if (taskbar && IsWindow(taskbar)) {
        EqRunFromWindowThread(taskbar, [](void*) {
            EqRemoveXamlButtonOnUiThread();
        }, nullptr);
    } else {
        g_eqXamlInjectionReady.store(false, std::memory_order_release);
        g_eqTaskbarHwnd = nullptr;
        g_eqXamlColumn = -1;

        // The taskbar owner is gone, so do not release thread-affine XAML
        // objects from this thread. The popup, if still valid, is closed by
        // its own window thread through EqClosePopup().
        EqClosePopup();
    }
}

static HMODULE GetCurrentModModuleHandle();
static bool RebuildOverlayWindows(
    const VisualizerSettings& settings,
    HINSTANCE instance,
    LPCWSTR windowClass,
    HWND desktopParent);
static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_VIZ_OPEN_LAYOUT_EDITOR:
        LayoutEditBegin();
        return 0;

    case WM_VIZ_REBUILD_OVERLAYS: {
        // A settings change while editing invalidates the edit session.
        if (g_layoutEditActive.load(std::memory_order_acquire))
            LayoutEditEnd(false);
        const VisualizerSettings settings = GetSettingsSnapshot();
        g_backgroundBlurNeedsReload.store(true, std::memory_order_release);
        g_cachedDisplayRefreshRateHz = 0;
        // Re-resolve rather than use GetParent(hwnd): the placement setting
        // may have changed, and with the opaque holder the parent is the
        // holder, not the desktop window.
        RebuildOverlayWindows(settings, GetCurrentModModuleHandle(),
                              L"WindhawkVisualizerOverlay",
                              ResolveOverlayDesktopParent(settings.desktopPlacement));
        if (g_hwndOverlay) {
            g_currentOverlayTimerMs = GetOverlayTimerIntervalMs(settings);
            SetTimer(g_hwndOverlay, 1, g_currentOverlayTimerMs, nullptr);
        }

        // Layered windows are updated with UpdateLayeredWindowIndirect; they
        // do not need a separate WM_PAINT render. Render one intentional frame
        // after rebuilding instead of invalidating the window and rendering
        // again from WM_PAINT. This removes duplicate full-frame uploads after
        // settings/display changes without changing the displayed result.
        for (HWND overlay : g_overlayWindows)
            RenderOverlay(overlay);
        return 0;
    }
    case WM_TIMER:
        if (wParam == 1 && hwnd == g_hwndOverlay &&
            !g_layoutEditActive.load(std::memory_order_acquire)) {
            std::shared_lock<std::shared_mutex> settingsLock(g_settingsMutex);
            UpdateAnimationFromAudio();

            // The timer runs at the fastest cadence required by the selected
            // monitors. Individual overlays are then throttled independently,
            // so a fullscreen app on monitor 1 does not slow monitor 2.
            const ULONGLONG nowMs = GetTickCount64();
            const HMONITOR fullscreenMonitor =
                g_fullscreenMonitor.load(std::memory_order_acquire);
            if (g_overlayLastRenderMs.size() != g_overlayWindows.size())
                g_overlayLastRenderMs.assign(g_overlayWindows.size(), 0);

            for (size_t i = 0; i < g_overlayWindows.size(); ++i) {
                HWND overlay = g_overlayWindows[i];
                bool shouldRender = true;

                if (fullscreenMonitor) {
                    const HMONITOR overlayMonitor =
                        MonitorFromWindow(overlay, MONITOR_DEFAULTTONEAREST);
                    if (overlayMonitor == fullscreenMonitor) {
                        const ULONGLONG lastRenderMs = g_overlayLastRenderMs[i];
                        shouldRender = lastRenderMs == 0 ||
                            nowMs - lastRenderMs >= 1000;
                    }
                }

                if (shouldRender) {
                    RenderOverlay(overlay);
                    g_overlayLastRenderMs[i] = nowMs;
                }
            }

            const int barCount = std::clamp(g_settings.barCount, 1, VIZ_BANDS_MAX);
            const float peakRatio = GetCurrentVisualizerPeakRatio(barCount);
            const float minRatio = static_cast<float>(g_settings.minBarHeight) /
                static_cast<float>(std::max(1, g_settings.maxBarHeight));
            const bool shouldIdle =
                peakRatio <= std::min(1.0f, minRatio + 0.01f);
            if (shouldIdle != g_overlayIdle.load(std::memory_order_acquire))
                g_overlayIdle.store(shouldIdle, std::memory_order_release);

            const UINT intervalMs = GetOverlayTimerIntervalMs(g_settings);
            if (intervalMs != g_currentOverlayTimerMs) {
                g_currentOverlayTimerMs = intervalMs;
                SetTimer(g_hwndOverlay, 1, intervalMs, nullptr);
            }
        }
        return 0;

    case WM_VIZ_AUDIO_WAKE:
        if (g_layoutEditActive.load(std::memory_order_acquire))
            return 0; // frozen while the layout editor is open
        g_overlayIdle.store(false, std::memory_order_release);
        {
            const VisualizerSettings settings = GetSettingsSnapshot();
            const UINT intervalMs = GetOverlayTimerIntervalMs(settings);
            if (intervalMs != g_currentOverlayTimerMs && g_hwndOverlay) {
                g_currentOverlayTimerMs = intervalMs;
                SetTimer(g_hwndOverlay, 1, intervalMs, nullptr);
            }
        }
        return 0;

    case WM_PAINT:
        // The overlay is a layered surface updated explicitly by
        // UpdateLayeredWindowIndirect. WM_PAINT must only clear the paint
        // message; rendering here can duplicate a timer-triggered upload.
        ValidateRect(hwnd, nullptr);
        return 0;

    case WM_WINDOWPOSCHANGING: {
        WINDOWPOS* pos = reinterpret_cast<WINDOWPOS*>(lParam);
        if (pos && GetParent(hwnd) == nullptr)
            pos->hwndInsertAfter = HWND_BOTTOM;
        break;
    }

    case WM_SETTINGCHANGE:
        // Wallpaper/theme changes invalidate the cached wallpaper blur and the
        // display query cache used to derive the render interval.
        g_backgroundBlurNeedsReload.store(true, std::memory_order_release);
        g_cachedDisplayRefreshRateHz = 0;
        if (g_eqIsWindows11 &&
            g_eqXamlInjectionReady.load(std::memory_order_acquire) &&
            g_eqTaskbarHwnd && IsWindow(g_eqTaskbarHwnd)) {
            EqRunFromWindowThread(g_eqTaskbarHwnd, [](void*) {
                try {
                    if (auto button = g_eqXamlButton.try_as<Button>())
                        button.InvalidateMeasure();
                    if (g_eqTaskbarHwnd)
                        InvalidateRect(g_eqTaskbarHwnd, nullptr, FALSE);
                    if (g_eqPopupHwnd && IsWindow(g_eqPopupHwnd))
                        RenderEqPopup(g_eqPopupHwnd);
                } catch (...) {
                }
            }, nullptr);
        }
        return 0;

    case WM_DISPLAYCHANGE:
        if (g_layoutEditActive.load(std::memory_order_acquire))
            LayoutEditEnd(false);
        g_backgroundBlurNeedsReload.store(true, std::memory_order_release);
        g_cachedDisplayRefreshRateHz = 0;
        g_currentOverlayTimerMs = 0;
        if (hwnd == g_hwndOverlay) {
            const VisualizerSettings settings = GetSettingsSnapshot();
            RebuildOverlayWindows(settings, GetCurrentModModuleHandle(),
                                  L"WindhawkVisualizerOverlay",
                                  ResolveOverlayDesktopParent(settings.desktopPlacement));
            if (g_hwndOverlay) {
                g_currentOverlayTimerMs = GetOverlayTimerIntervalMs(settings);
                SetTimer(g_hwndOverlay, 1, g_currentOverlayTimerMs, nullptr);
            }
            for (HWND overlay : g_overlayWindows)
                RenderOverlay(overlay);
        }
        return 0;

    case WM_NCHITTEST:
        // Desktop overlays are always click-through. Album Widget positioning
        // is handled by the dedicated Desktop Layout Editor.
        return HTTRANSPARENT;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static HMODULE GetCurrentModModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OverlayProc), &module);
    return module;
}

struct OverlayMonitorInfo {
    HMONITOR monitor = nullptr;
    RECT rect{};
    bool primary = false;
};

static BOOL CALLBACK CollectOverlayMonitorProc(
    HMONITOR hMonitor,
    HDC,
    LPRECT,
    LPARAM lParam) {
    auto* monitors = reinterpret_cast<std::vector<OverlayMonitorInfo>*>(lParam);
    if (!monitors) {
        return FALSE;
    }

    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMonitor, reinterpret_cast<MONITORINFO*>(&mi))) {
        return TRUE;
    }

    OverlayMonitorInfo info;
    info.monitor = hMonitor;
    info.rect = mi.rcMonitor;
    info.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
    monitors->push_back(info);

    return TRUE;
}

static std::vector<OverlayMonitorInfo> GetOverlayMonitors() {
    std::vector<OverlayMonitorInfo> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr, CollectOverlayMonitorProc,
        reinterpret_cast<LPARAM>(&monitors));

    std::stable_sort(monitors.begin(), monitors.end(),
        [](const OverlayMonitorInfo& a, const OverlayMonitorInfo& b) {
            if (a.primary != b.primary)
                return a.primary > b.primary;
            if (a.rect.top != b.rect.top)
                return a.rect.top < b.rect.top;
            if (a.rect.left != b.rect.left)
                return a.rect.left < b.rect.left;
            return a.rect.right < b.rect.right;
        });

    return monitors;
}

static std::vector<RECT> GetOverlayTargetRects(const VisualizerSettings& settings) {
    const auto monitors = GetOverlayMonitors();
    std::vector<RECT> result;

    if (settings.targetMonitor <= 0) {
        result.reserve(monitors.size());
        for (size_t i = 0; i < monitors.size(); ++i) {
            result.push_back(monitors[i].rect);
        }
        return result;
    }

    const size_t index = static_cast<size_t>(settings.targetMonitor - 1);

    if (index < monitors.size()) {
        result.push_back(monitors[index].rect);
        return result;
    }


    for (size_t i = 0; i < monitors.size(); ++i) {
        if (monitors[i].primary) {
            result.push_back(monitors[i].rect);
            return result;
        }
    }

    if (!monitors.empty()) {
        result.push_back(monitors.front().rect);
    } else {
    }
    return result;
}

static RECT GetOverlayTargetScreenRect(const VisualizerSettings& settings) {
    const auto rects = GetOverlayTargetRects(settings);
    if (!rects.empty()) {
        return rects.front();
    }

    RECT fallback{
        GetSystemMetrics(SM_XVIRTUALSCREEN),
        GetSystemMetrics(SM_YVIRTUALSCREEN),
        GetSystemMetrics(SM_XVIRTUALSCREEN) +
            std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN)),
        GetSystemMetrics(SM_YVIRTUALSCREEN) +
            std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN))};
    return fallback;
}

// ===========================================================================
// Raised-desktop presentation (Windows 11 24H2+)
// ===========================================================================
// See docs/research/2026-09-26-win11-raised-desktop.md. On the raised desktop
// an UpdateLayeredWindow child of Progman is never composited. Two ways in:
//
//  - OVERLAY_RENDER_OPAQUE_HOLDER (default there): the recipe known to work
//    for live-wallpaper tools. An opaque layered holder (LWA_ALPHA 255) is a
//    child of Progman directly below the icon layer, and the overlay window
//    inside it presents through a Direct3D swap chain. Being opaque it hides
//    the real wallpaper, so every frame starts from a captured copy of it.
//  - OVERLAY_RENDER_DCOMP (experimental): the overlay window itself is a child
//    of Progman below the icons and presents a premultiplied-alpha
//    DirectComposition swap chain, keeping true transparency.
//
// Everything here runs on the overlay thread.

static const wchar_t* kOverlayHostClass = L"WindhawkVisualizerOverlayHost";
static bool g_overlayHostClassRegistered = false;

struct OverlayPresenter {
    HWND hwnd = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    Microsoft::WRL::ComPtr<IDCompositionTarget> target;
    Microsoft::WRL::ComPtr<IDCompositionVisual> visual;
    int width = 0;
    int height = 0;
    bool loggedFirstPresent = false;
};
static std::vector<OverlayPresenter> g_overlayPresenters;
static Microsoft::WRL::ComPtr<ID3D11Device> g_overlayD3D;
static Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_overlayD3DContext;
static Microsoft::WRL::ComPtr<IDXGIFactory2> g_overlayDxgiFactory;
static Microsoft::WRL::ComPtr<IDCompositionDevice> g_overlayDComp;

struct OverlayWallpaperCache {
    RECT screenRect{};
    int width = 0;
    int height = 0;
    std::vector<DWORD> pixels; // premultiplied BGRA, alpha forced to 255
};
static std::vector<OverlayWallpaperCache> g_overlayWallpapers; // parallel to g_overlayWindows
static ULONGLONG g_overlayWallpaperCapturedMs = 0;
static std::wstring g_overlayWallpaperSignature;

static LRESULT CALLBACK OverlayHostProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    case WM_PAINT: {
        // The overlay window inside covers the whole holder.
        PAINTSTRUCT ps{};
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool EnsureOverlayHostClass(HINSTANCE instance) {
    if (g_overlayHostClassRegistered)
        return true;
    WNDCLASSW wc{};
    wc.lpfnWndProc = OverlayHostProc;
    wc.hInstance = instance;
    wc.lpszClassName = kOverlayHostClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"EnsureOverlayHostClass: RegisterClass failed, error=%lu", GetLastError());
        return false;
    }
    g_overlayHostClassRegistered = true;
    return true;
}

static void UnregisterOverlayHostClass(HINSTANCE instance) {
    if (!g_overlayHostClassRegistered)
        return;
    UnregisterClassW(kOverlayHostClass, instance);
    g_overlayHostClassRegistered = false;
}

static int OverlayIndexOf(HWND hwnd) {
    for (size_t i = 0; i < g_overlayWindows.size(); ++i) {
        if (g_overlayWindows[i] == hwnd)
            return static_cast<int>(i);
    }
    return -1;
}

static bool EnsureOverlayD3D() {
    if (g_overlayD3D && g_overlayDxgiFactory)
        return true;

    const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3};
    const UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, levels,
        ARRAYSIZE(levels), D3D11_SDK_VERSION, &g_overlayD3D, nullptr,
        &g_overlayD3DContext);
    if (FAILED(hr)) {
        Wh_Log(L"Overlay D3D: hardware device failed (hr=0x%08X), trying WARP",
               static_cast<unsigned>(hr));
        hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags, levels,
            ARRAYSIZE(levels), D3D11_SDK_VERSION, &g_overlayD3D, nullptr,
            &g_overlayD3DContext);
    }
    if (FAILED(hr)) {
        Wh_Log(L"Overlay D3D: D3D11CreateDevice failed (hr=0x%08X)", static_cast<unsigned>(hr));
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    if (FAILED(hr = g_overlayD3D.As(&dxgiDevice)) ||
        FAILED(hr = dxgiDevice->GetAdapter(&adapter)) ||
        FAILED(hr = adapter->GetParent(IID_PPV_ARGS(&g_overlayDxgiFactory)))) {
        Wh_Log(L"Overlay D3D: DXGI factory lookup failed (hr=0x%08X)", static_cast<unsigned>(hr));
        g_overlayD3DContext.Reset();
        g_overlayD3D.Reset();
        return false;
    }
    return true;
}

static bool EnsureOverlayDComp() {
    if (g_overlayDComp)
        return true;
    if (!EnsureOverlayD3D())
        return false;
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    HRESULT hr = g_overlayD3D.As(&dxgiDevice);
    if (SUCCEEDED(hr)) {
        hr = DCompositionCreateDevice(
            dxgiDevice.Get(), __uuidof(IDCompositionDevice),
            reinterpret_cast<void**>(g_overlayDComp.GetAddressOf()));
    }
    if (FAILED(hr)) {
        Wh_Log(L"Overlay DComp: DCompositionCreateDevice failed (hr=0x%08X)",
               static_cast<unsigned>(hr));
        return false;
    }
    return true;
}

static void DestroyOverlayPresenters() {
    for (auto& p : g_overlayPresenters) {
        if (p.visual)
            p.visual->SetContent(nullptr);
        if (p.target)
            p.target->SetRoot(nullptr);
        p.visual.Reset();
        p.target.Reset();
        p.swapChain.Reset();
    }
    g_overlayPresenters.clear();
    if (g_overlayDComp)
        g_overlayDComp->Commit();
    if (g_overlayD3DContext) {
        g_overlayD3DContext->ClearState();
        g_overlayD3DContext->Flush();
    }
}

static void ReleaseOverlayGraphics() {
    DestroyOverlayPresenters();
    g_overlayDComp.Reset();
    g_overlayDxgiFactory.Reset();
    g_overlayD3DContext.Reset();
    g_overlayD3D.Reset();
}

static void CreateOverlayPresenter(HWND hwnd, int w, int h, bool composition) {
    OverlayPresenter p{};
    p.hwnd = hwnd;
    p.width = w;
    p.height = h;

    if (!EnsureOverlayD3D() || (composition && !EnsureOverlayDComp())) {
        g_overlayPresenters.push_back(p);
        return;
    }

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = static_cast<UINT>(w);
    desc.Height = static_cast<UINT>(h);
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    HRESULT hr = S_OK;
    if (composition) {
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        hr = g_overlayDxgiFactory->CreateSwapChainForComposition(
            g_overlayD3D.Get(), &desc, nullptr, &p.swapChain);
        if (SUCCEEDED(hr))
            hr = g_overlayDComp->CreateTargetForHwnd(hwnd, TRUE, &p.target);
        if (SUCCEEDED(hr))
            hr = g_overlayDComp->CreateVisual(&p.visual);
        if (SUCCEEDED(hr))
            hr = p.visual->SetContent(p.swapChain.Get());
        if (SUCCEEDED(hr))
            hr = p.target->SetRoot(p.visual.Get());
        if (SUCCEEDED(hr))
            hr = g_overlayDComp->Commit();
        Wh_Log(L"Overlay presenter (DirectComposition, premultiplied) hwnd=%p %dx%d: hr=0x%08X",
               hwnd, w, h, static_cast<unsigned>(hr));
    } else {
        desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        hr = g_overlayDxgiFactory->CreateSwapChainForHwnd(
            g_overlayD3D.Get(), hwnd, &desc, nullptr, nullptr, &p.swapChain);
        if (FAILED(hr)) {
            Wh_Log(L"Overlay presenter: flip swap chain failed (hr=0x%08X), trying blt model",
                   static_cast<unsigned>(hr));
            desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            desc.BufferCount = 1;
            desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            hr = g_overlayDxgiFactory->CreateSwapChainForHwnd(
                g_overlayD3D.Get(), hwnd, &desc, nullptr, nullptr, &p.swapChain);
        }
        Wh_Log(L"Overlay presenter (D3D HWND swap chain) hwnd=%p %dx%d: hr=0x%08X",
               hwnd, w, h, static_cast<unsigned>(hr));
    }

    if (FAILED(hr)) {
        p.visual.Reset();
        p.target.Reset();
        p.swapChain.Reset();
    }
    g_overlayPresenters.push_back(p);
}

static bool OverlayPresentFrame(HWND hwnd, int w, int h) {
    OverlayPresenter* presenter = nullptr;
    for (auto& p : g_overlayPresenters) {
        if (p.hwnd == hwnd) {
            presenter = &p;
            break;
        }
    }

    if (!presenter || !presenter->swapChain) {
        // No swap chain: in the opaque holder the overlay window is a plain
        // child of a layered window, so GDI still reaches the holder's surface.
        if (g_overlayBuiltRenderMode == OVERLAY_RENDER_OPAQUE_HOLDER) {
            HDC dc = GetDC(hwnd);
            if (dc) {
                BitBlt(dc, 0, 0, w, h, g_renderMemDC, 0, 0, SRCCOPY);
                ReleaseDC(hwnd, dc);
                return true;
            }
        }
        return false;
    }

    HRESULT hr = S_OK;
    if (presenter->width != w || presenter->height != h) {
        hr = presenter->swapChain->ResizeBuffers(
            0, static_cast<UINT>(w), static_cast<UINT>(h), DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr)) {
            Wh_Log(L"Overlay presenter: ResizeBuffers failed (hr=0x%08X)", static_cast<unsigned>(hr));
            g_overlayForceRebuild = true;
            return false;
        }
        presenter->width = w;
        presenter->height = h;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = presenter->swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (SUCCEEDED(hr)) {
        const D3D11_BOX box{0, 0, 0, static_cast<UINT>(w), static_cast<UINT>(h), 1};
        g_overlayD3DContext->UpdateSubresource(
            backBuffer.Get(), 0, &box, g_renderBits,
            static_cast<UINT>(g_renderWidth) * sizeof(DWORD), 0);
        backBuffer.Reset();
        hr = presenter->swapChain->Present(0, 0);
    }

    if (!presenter->loggedFirstPresent) {
        presenter->loggedFirstPresent = true;
        Wh_Log(L"Overlay presenter hwnd=%p: first Present hr=0x%08X",
               hwnd, static_cast<unsigned>(hr));
    }
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        Wh_Log(L"Overlay presenter: device lost (hr=0x%08X), rebuilding",
               static_cast<unsigned>(hr));
        g_overlayForceRebuild = true;
    }
    return SUCCEEDED(hr);
}

// ---------------------------------------------------------------------------
// Wallpaper copy for the opaque holder
// ---------------------------------------------------------------------------

static DWORD OverlayDesktopColorPixel() {
    const COLORREF c = GetSysColor(COLOR_DESKTOP);
    return 0xFF000000u | (static_cast<DWORD>(GetRValue(c)) << 16) |
           (static_cast<DWORD>(GetGValue(c)) << 8) | GetBValue(c);
}

static std::wstring OverlayTranscodedWallpaperPath() {
    wchar_t path[MAX_PATH]{};
    ExpandEnvironmentStringsW(
        L"%APPDATA%\\Microsoft\\Windows\\Themes\\TranscodedWallpaper", path, MAX_PATH);
    return path;
}

// Cheap per-second check for wallpaper changes (manual change, slideshow,
// Spotlight all touch at least one of these).
static std::wstring CurrentWallpaperSignature() {
    wchar_t wallpaper[MAX_PATH]{};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, wallpaper, 0);
    std::wstring sig = wallpaper;
    WIN32_FILE_ATTRIBUTE_DATA fad{};
    if (GetFileAttributesExW(OverlayTranscodedWallpaperPath().c_str(),
                             GetFileExInfoStandard, &fad)) {
        wchar_t buf[96]{};
        swprintf_s(buf, L"|%08lX%08lX|%lu", fad.ftLastWriteTime.dwHighDateTime,
                   fad.ftLastWriteTime.dwLowDateTime, fad.nFileSizeLow);
        sig += buf;
    }
    wchar_t color[16]{};
    swprintf_s(color, L"|%06lX", static_cast<unsigned long>(GetSysColor(COLOR_DESKTOP)));
    sig += color;
    return sig;
}

// Pixel-exact copy of what Explorer draws in the wallpaper WorkerW.
static bool CaptureWallpaperFromWorkerW(HWND workerW, std::vector<DWORD>& out, RECT& outRect) {
    if (!workerW || !IsWindow(workerW) || !GetWindowRect(workerW, &outRect))
        return false;
    const int w = outRect.right - outRect.left;
    const int h = outRect.bottom - outRect.top;
    if (w <= 0 || h <= 0)
        return false;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!mem || !bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        if (mem) DeleteDC(mem);
        return false;
    }
    HGDIOBJ old = SelectObject(mem, bmp);
    std::memset(bits, 0, static_cast<size_t>(w) * h * sizeof(DWORD));
    const BOOL printed = PrintWindow(workerW, mem, PW_RENDERFULLCONTENT);
    GdiFlush();

    const DWORD* src = static_cast<const DWORD*>(bits);
    size_t nonBlack = 0;
    size_t samples = 0;
    const size_t total = static_cast<size_t>(w) * h;
    for (size_t i = 0; i < total; i += std::max<size_t>(1, total / 4096)) {
        ++samples;
        if (src[i] & 0x00FFFFFFu)
            ++nonBlack;
    }
    const bool usable = printed && nonBlack * 100 >= samples;
    if (usable) {
        out.resize(total);
        for (size_t i = 0; i < total; ++i)
            out[i] = src[i] | 0xFF000000u;
    }
    Wh_Log(L"Wallpaper capture: PrintWindow(WorkerW %p, %dx%d) printed=%d nonBlack=%zu/%zu -> %s",
           workerW, w, h, printed ? 1 : 0, nonBlack, samples, usable ? L"used" : L"rejected");

    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    return usable;
}

static void DrawWallpaperImage(Gdiplus::Graphics& g, Gdiplus::Bitmap& img,
                               DESKTOP_WALLPAPER_POSITION pos,
                               const RECT& area, const RECT& clip, const RECT& target) {
    const float iw = static_cast<float>(img.GetWidth());
    const float ih = static_cast<float>(img.GetHeight());
    if (iw <= 0 || ih <= 0)
        return;
    const float ax = static_cast<float>(area.left - target.left);
    const float ay = static_cast<float>(area.top - target.top);
    const float aw = static_cast<float>(area.right - area.left);
    const float ah = static_cast<float>(area.bottom - area.top);

    g.SetClip(Gdiplus::Rect(clip.left - target.left, clip.top - target.top,
                            clip.right - clip.left, clip.bottom - clip.top));
    if (pos == DWPOS_TILE) {
        Gdiplus::TextureBrush brush(&img);
        brush.TranslateTransform(ax, ay);
        g.FillRectangle(&brush, ax, ay, aw, ah);
    } else {
        float dw = aw, dh = ah;
        if (pos == DWPOS_CENTER) {
            dw = iw;
            dh = ih;
        } else if (pos == DWPOS_FIT) {
            const float s = std::min(aw / iw, ah / ih);
            dw = iw * s;
            dh = ih * s;
        } else if (pos == DWPOS_FILL || pos == DWPOS_SPAN) {
            const float s = std::max(aw / iw, ah / ih);
            dw = iw * s;
            dh = ih * s;
        }
        g.DrawImage(&img, Gdiplus::RectF(ax + (aw - dw) / 2.0f, ay + (ah - dh) / 2.0f, dw, dh));
    }
    g.ResetClip();
}

// Fallback: redraw the wallpaper from the shell's settings.
static bool RenderWallpaperViaShell(const RECT& target, std::vector<DWORD>& out) {
    const int w = target.right - target.left;
    const int h = target.bottom - target.top;
    if (w <= 0 || h <= 0)
        return false;

    const HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool drewImage = false;
    {
        Microsoft::WRL::ComPtr<IDesktopWallpaper> dw;
        HRESULT hr = CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                      IID_PPV_ARGS(&dw));
        if (SUCCEEDED(hr)) {
            COLORREF bg = GetSysColor(COLOR_DESKTOP);
            dw->GetBackgroundColor(&bg);
            DESKTOP_WALLPAPER_POSITION pos = DWPOS_FILL;
            dw->GetPosition(&pos);

            Gdiplus::Bitmap canvas(w, h, PixelFormat32bppARGB);
            Gdiplus::Graphics g(&canvas);
            g.Clear(Gdiplus::Color(255, GetRValue(bg), GetGValue(bg), GetBValue(bg)));
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

            UINT count = 0;
            dw->GetMonitorDevicePathCount(&count);
            std::vector<RECT> monitorRects;
            std::vector<std::wstring> paths;
            RECT span{};
            for (UINT i = 0; i < count; ++i) {
                LPWSTR id = nullptr;
                if (FAILED(dw->GetMonitorDevicePathAt(i, &id)) || !id)
                    continue;
                RECT mr{};
                LPWSTR path = nullptr;
                if (SUCCEEDED(dw->GetMonitorRECT(id, &mr))) {
                    dw->GetWallpaper(id, &path);
                    monitorRects.push_back(mr);
                    paths.push_back(path ? path : L"");
                    UnionRect(&span, &span, &mr);
                }
                if (path) CoTaskMemFree(path);
                CoTaskMemFree(id);
            }

            for (size_t i = 0; i < monitorRects.size(); ++i) {
                RECT visible{};
                if (!IntersectRect(&visible, &monitorRects[i], &target))
                    continue;
                std::wstring path = paths[i];
                if (path.empty() || GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
                    path = OverlayTranscodedWallpaperPath();
                if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
                    continue;
                Gdiplus::Bitmap img(path.c_str());
                if (img.GetLastStatus() != Gdiplus::Ok)
                    continue;
                const RECT& area = pos == DWPOS_SPAN ? span : monitorRects[i];
                DrawWallpaperImage(g, img, pos, area, visible, target);
                drewImage = true;
            }

            Gdiplus::Rect lockRect(0, 0, w, h);
            Gdiplus::BitmapData data{};
            if (canvas.LockBits(&lockRect, Gdiplus::ImageLockModeRead,
                                PixelFormat32bppARGB, &data) == Gdiplus::Ok) {
                out.resize(static_cast<size_t>(w) * h);
                for (int y = 0; y < h; ++y) {
                    const DWORD* row = reinterpret_cast<const DWORD*>(
                        static_cast<const BYTE*>(data.Scan0) + static_cast<size_t>(y) * data.Stride);
                    for (int x = 0; x < w; ++x)
                        out[static_cast<size_t>(y) * w + x] = row[x] | 0xFF000000u;
                }
                canvas.UnlockBits(&data);
            } else {
                drewImage = false;
            }
            Wh_Log(L"Wallpaper capture: IDesktopWallpaper monitors=%u position=%d drewImage=%d",
                   count, static_cast<int>(pos), drewImage ? 1 : 0);
        } else {
            Wh_Log(L"Wallpaper capture: IDesktopWallpaper unavailable (hr=0x%08X)",
                   static_cast<unsigned>(hr));
        }
    }
    if (SUCCEEDED(hrInit))
        CoUninitialize();
    return drewImage;
}

static void CaptureOverlayWallpapers(const std::vector<RECT>& rects) {
    g_overlayWallpapers.clear();

    std::vector<DWORD> full;
    RECT fullRect{};
    const bool haveFull = CaptureWallpaperFromWorkerW(
        g_overlayPlacement.wallpaperWorkerW, full, fullRect);
    const int fullW = fullRect.right - fullRect.left;

    for (const RECT& rect : rects) {
        OverlayWallpaperCache cache{};
        cache.screenRect = rect;
        cache.width = std::max(1L, rect.right - rect.left);
        cache.height = std::max(1L, rect.bottom - rect.top);
        cache.pixels.assign(static_cast<size_t>(cache.width) * cache.height,
                            OverlayDesktopColorPixel());

        RECT overlap{};
        if (haveFull && IntersectRect(&overlap, &rect, &fullRect)) {
            const int copyW = overlap.right - overlap.left;
            for (int y = overlap.top; y < overlap.bottom; ++y) {
                const DWORD* src = full.data() +
                    static_cast<size_t>(y - fullRect.top) * fullW + (overlap.left - fullRect.left);
                DWORD* dst = cache.pixels.data() +
                    static_cast<size_t>(y - rect.top) * cache.width + (overlap.left - rect.left);
                std::memcpy(dst, src, static_cast<size_t>(copyW) * sizeof(DWORD));
            }
        } else {
            std::vector<DWORD> drawn;
            if (RenderWallpaperViaShell(rect, drawn) &&
                drawn.size() == cache.pixels.size()) {
                cache.pixels.swap(drawn);
            } else {
                Wh_Log(L"Wallpaper capture: falling back to the desktop background color");
            }
        }
        g_overlayWallpapers.push_back(std::move(cache));
    }

    g_overlayWallpaperCapturedMs = GetTickCount64();
    g_overlayWallpaperSignature = CurrentWallpaperSignature();
}

static void RecaptureOverlayWallpapers() {
    std::vector<RECT> rects;
    for (HWND hwnd : g_overlayWindows) {
        RECT r{};
        if (hwnd && GetWindowRect(hwnd, &r))
            rects.push_back(r);
    }
    if (rects.size() == g_overlayWindows.size())
        CaptureOverlayWallpapers(rects);
}

static bool OverlayFillWallpaperBase(HWND hwnd, int w, int h) {
    const int index = OverlayIndexOf(hwnd);
    if (index < 0 || static_cast<size_t>(index) >= g_overlayWallpapers.size())
        return false;
    const OverlayWallpaperCache& cache = g_overlayWallpapers[static_cast<size_t>(index)];
    BYTE* base = static_cast<BYTE*>(g_renderBits);
    const size_t stride = static_cast<size_t>(g_renderWidth) * sizeof(DWORD);
    if (cache.width != w || cache.height != h) {
        // Size changed since the capture: solid color until the next capture.
        const DWORD color = OverlayDesktopColorPixel();
        for (int y = 0; y < h; ++y)
            std::fill_n(reinterpret_cast<DWORD*>(base + y * stride), w, color);
        return true;
    }
    for (int y = 0; y < h; ++y) {
        std::memcpy(base + y * stride,
                    cache.pixels.data() + static_cast<size_t>(y) * w,
                    static_cast<size_t>(w) * sizeof(DWORD));
    }
    return true;
}

// One-shot dump of Progman's children in z-order, for remote debugging on
// machines we cannot test on (Windows 11 raised desktop).
static void LogDesktopTree(const wchar_t* reason) {
    HWND progman = GetShellWindow();
    if (!progman)
        progman = FindWindowW(L"Progman", nullptr);
    if (!progman)
        return;
    Wh_Log(L"Desktop tree (%s): Progman=%p exStyle=0x%08lX mode=%d",
           reason, progman,
           static_cast<unsigned long>(GetWindowLongPtrW(progman, GWL_EXSTYLE)),
           g_overlayPlacement.renderMode);
    int n = 0;
    for (HWND c = GetWindow(progman, GW_CHILD); c && n < 24;
         c = GetWindow(c, GW_HWNDNEXT), ++n) {
        wchar_t cls[64]{};
        GetClassNameW(c, cls, ARRAYSIZE(cls));
        RECT r{};
        GetWindowRect(c, &r);
        BYTE alpha = 0;
        DWORD lwaFlags = 0;
        COLORREF key = 0;
        const LONG_PTR ex = GetWindowLongPtrW(c, GWL_EXSTYLE);
        const bool haveLwa = (ex & WS_EX_LAYERED) &&
                             GetLayeredWindowAttributes(c, &key, &alpha, &lwaFlags);
        Wh_Log(L"  #%d %p %s (%ld,%ld)-(%ld,%ld) vis=%d style=0x%08lX ex=0x%08lX lwa=%s%lu/%u",
               n, c, cls, r.left, r.top, r.right, r.bottom,
               IsWindowVisible(c) ? 1 : 0,
               static_cast<unsigned long>(GetWindowLongPtrW(c, GWL_STYLE)),
               static_cast<unsigned long>(ex),
               haveLwa ? L"" : L"-", static_cast<unsigned long>(lwaFlags),
               static_cast<unsigned>(alpha));
    }
}

static void DestroyAllOverlayWindows() {
    // Swap chains and composition targets go before their windows.
    DestroyOverlayPresenters();
    if (g_overlayForceRebuild) {
        // Device lost or presenter broken: start the next build from scratch.
        ReleaseOverlayGraphics();
        g_overlayForceRebuild = false;
    }
    for (HWND hwnd : g_overlayWindows) {
        if (hwnd && IsWindow(hwnd)) {
            KillTimer(hwnd, 1);
            DestroyWindow(hwnd);
        }
    }
    for (HWND host : g_overlayHosts) {
        if (host && IsWindow(host))
            DestroyWindow(host);
    }
    g_overlayWindows.clear();
    g_overlayHosts.clear();
    g_overlayLastRenderMs.clear();
    g_overlayWallpapers.clear();
    g_overlayBuiltRenderMode = OVERLAY_RENDER_ULW;
    g_hwndOverlay = nullptr;
    g_overlayWakeHwnd.store(nullptr, std::memory_order_release);
}

// Opaque holder for the raised desktop, following the recipe known to work
// there: created top-level with WS_EX_LAYERED, given LWA_ALPHA 255, then
// reparented onto Progman and turned into a WS_CHILD.
static HWND CreateOpaqueOverlayHost(HINSTANCE instance, HWND desktopParent,
                                    const RECT& screenRect, POINT clientTopLeft) {
    if (!EnsureOverlayHostClass(instance))
        return nullptr;
    const int width = std::max(1L, screenRect.right - screenRect.left);
    const int height = std::max(1L, screenRect.bottom - screenRect.top);
    HWND host = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kOverlayHostClass, L"Desktop Audio Visualizer Host", WS_POPUP,
        screenRect.left, screenRect.top, width, height,
        nullptr, nullptr, instance, nullptr);
    if (!host) {
        Wh_Log(L"CreateOpaqueOverlayHost: CreateWindowEx failed, error=%lu", GetLastError());
        return nullptr;
    }
    if (!SetLayeredWindowAttributes(host, 0, 255, LWA_ALPHA))
        Wh_Log(L"CreateOpaqueOverlayHost: SetLayeredWindowAttributes failed, error=%lu", GetLastError());
    if (!SetParent(host, desktopParent))
        Wh_Log(L"CreateOpaqueOverlayHost: SetParent failed, error=%lu", GetLastError());
    LONG_PTR style = GetWindowLongPtrW(host, GWL_STYLE);
    style = (style | WS_CHILD) & ~static_cast<LONG_PTR>(WS_POPUP);
    SetWindowLongPtrW(host, GWL_STYLE, style);
    SetWindowPos(host, nullptr, clientTopLeft.x, clientTopLeft.y, width, height,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
    return host;
}

static bool CreateOverlayWindowsForSettings(
    const VisualizerSettings& settings,
    HINSTANCE instance,
    LPCWSTR windowClass,
    HWND desktopParent) {
    const auto rects = GetOverlayTargetRects(settings);

    if (rects.empty() || !desktopParent || !IsWindow(desktopParent)) {
        return false;
    }

    DestroyAllOverlayWindows();

    int maxW = 1;
    int maxH = 1;
    for (const RECT& rect : rects) {
        maxW = std::max<int>(maxW, rect.right - rect.left);
        maxH = std::max<int>(maxH, rect.bottom - rect.top);
    }
    if (!EnsureRenderTarget(maxW, maxH)) {
        return false;
    }

    const int renderMode = g_overlayPlacement.renderMode;
    if (renderMode == OVERLAY_RENDER_OPAQUE_HOLDER) {
        // Capture before our opaque layer exists (PrintWindow renders only
        // the WorkerW itself, but keep the order obvious).
        CaptureOverlayWallpapers(rects);
    }

    for (size_t rectIndex = 0; rectIndex < rects.size(); ++rectIndex) {
        const RECT& screenRect = rects[rectIndex];
        const int width = std::max(1L, screenRect.right - screenRect.left);
        const int height = std::max(1L, screenRect.bottom - screenRect.top);

        // Keep the overlay as a CHILD of the desktop hierarchy (the wallpaper
        // WorkerW on the classic desktop, Progman on the raised desktop, see
        // ResolveOverlayDesktopParent). Win+D / Show Desktop manipulates
        // top-level windows, while the desktop child hierarchy remains part
        // of the desktop surface. Creating these as WS_POPUP windows makes
        // them disappear on Win+D.
        POINT topLeft{screenRect.left, screenRect.top};
        POINT bottomRight{screenRect.right, screenRect.bottom};
        if (!ScreenToClient(desktopParent, &topLeft) ||
            !ScreenToClient(desktopParent, &bottomRight)) {
            DestroyAllOverlayWindows();
            return false;
        }

        HWND host = nullptr;
        HWND hwnd = nullptr;
        if (renderMode == OVERLAY_RENDER_OPAQUE_HOLDER) {
            host = CreateOpaqueOverlayHost(instance, desktopParent, screenRect, topLeft);
            if (host) {
                hwnd = CreateWindowExW(
                    WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
                    windowClass, L"Desktop Audio Visualizer",
                    WS_CHILD | WS_VISIBLE, 0, 0, width, height,
                    host, nullptr, instance, nullptr);
            }
        } else if (renderMode == OVERLAY_RENDER_DCOMP) {
            hwnd = CreateWindowExW(
                WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                    WS_EX_NOACTIVATE,
                windowClass, L"Desktop Audio Visualizer",
                WS_CHILD | WS_VISIBLE, topLeft.x, topLeft.y, width, height,
                desktopParent, nullptr, instance, nullptr);
        } else {
            hwnd = CreateWindowExW(
                WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                windowClass, L"Desktop Audio Visualizer",
                WS_CHILD | WS_VISIBLE, topLeft.x, topLeft.y, width, height,
                desktopParent, nullptr, instance, nullptr);
        }
        if (!hwnd) {
            Wh_Log(L"CreateOverlayWindowsForSettings: CreateWindowEx failed (mode=%d), error=%lu",
                   renderMode, GetLastError());
            if (host && IsWindow(host))
                DestroyWindow(host);
            DestroyAllOverlayWindows();
            return false;
        }
        if (!host) {
            SetWindowPos(hwnd, nullptr, topLeft.x, topLeft.y, width, height,
                         SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
        BOOL excludeFromPeek = TRUE;
        DwmSetWindowAttribute(host ? host : hwnd, DWMWA_EXCLUDED_FROM_PEEK,
                              &excludeFromPeek, sizeof(excludeFromPeek));
        g_overlayWindows.push_back(hwnd);
        g_overlayHosts.push_back(host);

        if (renderMode != OVERLAY_RENDER_ULW)
            CreateOverlayPresenter(hwnd, width, height, renderMode == OVERLAY_RENDER_DCOMP);
        if (host)
            ShowWindow(host, SW_SHOWNOACTIVATE);
    }
    g_overlayBuiltRenderMode = renderMode;
    g_overlayLastRenderMs.assign(g_overlayWindows.size(), 0);

    // Raised desktop: a new child lands on top of Progman's children, i.e.
    // above the icon layer. Stack it below the icons right away.
    ReassertOverlayZOrder();

    g_hwndOverlay = g_overlayWindows.front();
    g_overlayScreenRect = rects.front();
    g_overlayWakeHwnd.store(g_hwndOverlay, std::memory_order_release);
    g_overlayIdle.store(false, std::memory_order_release);

    Wh_Log(L"Overlay windows created: count=%zu mode=%d first=%p host=%p",
           g_overlayWindows.size(), renderMode, g_overlayWindows.front(),
           g_overlayHosts.front());
    if (g_overlayPlacement.raised)
        LogDesktopTree(L"after overlay creation");
    return true;
}

// Returns true when the overlay windows were recreated. A recreated window
// has no render timer, so the caller must re-arm it.
static bool RebuildOverlayWindows(
    const VisualizerSettings& settings,
    HINSTANCE instance,
    LPCWSTR windowClass,
    HWND desktopParent) {
    const auto desired = GetOverlayTargetRects(settings);
    bool matches = !g_overlayForceRebuild &&
                   desktopParent && IsWindow(desktopParent) &&
                   g_overlayBuiltRenderMode == g_overlayPlacement.renderMode &&
                   desired.size() == g_overlayWindows.size() &&
                   g_overlayHosts.size() == g_overlayWindows.size();
    if (matches) {
        for (size_t i = 0; i < desired.size(); ++i) {
            // The window stacked among the desktop's children: the holder
            // when there is one, otherwise the overlay itself.
            HWND stacked = g_overlayHosts[i] ? g_overlayHosts[i] : g_overlayWindows[i];
            RECT actual{};
            if (!g_overlayWindows[i] || !IsWindow(g_overlayWindows[i]) ||
                !stacked || !IsWindow(stacked) ||
                GetParent(stacked) != desktopParent ||
                !GetWindowRect(g_overlayWindows[i], &actual) ||
                std::memcmp(&actual, &desired[i], sizeof(RECT)) != 0) {
                matches = false;
                break;
            }
        }
    }
    if (!matches && desktopParent && IsWindow(desktopParent)) {
        const bool ok = CreateOverlayWindowsForSettings(
            settings, instance, windowClass, desktopParent);
        Wh_Log(L"RebuildOverlayWindows: overlay windows recreated (ok=%d)",
               ok ? 1 : 0);
        return ok;
    }
    return false;
}

static UINT GetDisplayRefreshRateHz() {
    const VisualizerSettings settings = GetSettingsSnapshot();
    const auto monitors = GetOverlayMonitors();

    if (settings.targetMonitor <= 0 && !monitors.empty()) {
        UINT maxRefreshRate = 1;
        for (const auto& monitor : monitors) {
            MONITORINFOEXW monitorInfo{};
            monitorInfo.cbSize = sizeof(monitorInfo);
            DEVMODEW devMode{};
            devMode.dmSize = sizeof(devMode);

            if (GetMonitorInfoW(
                    monitor.monitor,
                    reinterpret_cast<MONITORINFO*>(&monitorInfo)) &&
                EnumDisplaySettingsW(
                    monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode) &&
                devMode.dmDisplayFrequency > 1) {
                maxRefreshRate = std::max<UINT>(
                    maxRefreshRate,
                    static_cast<UINT>(devMode.dmDisplayFrequency));
            }
        }
        if (maxRefreshRate > 1)
            return maxRefreshRate;
    }

    HMONITOR monitor = nullptr;
    if (g_hwndOverlay)
        monitor = MonitorFromWindow(g_hwndOverlay, MONITOR_DEFAULTTOPRIMARY);
    else
        monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFOEXW monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    DEVMODEW devMode{};
    devMode.dmSize = sizeof(devMode);

    if (monitor &&
        GetMonitorInfoW(
            monitor, reinterpret_cast<MONITORINFO*>(&monitorInfo)) &&
        EnumDisplaySettingsW(
            monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode) &&
        devMode.dmDisplayFrequency > 1) {
        return static_cast<UINT>(devMode.dmDisplayFrequency);
    }

    return 60;
}

static HMONITOR GetFullscreenForegroundMonitor() {
    HWND foreground = GetForegroundWindow();
    if (!foreground || !IsWindow(foreground) || !IsWindowVisible(foreground) ||
        IsIconic(foreground)) {
        return nullptr;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(foreground, &processId);
    if (processId == 0 || processId == GetCurrentProcessId())
        return nullptr;

    LONG style = GetWindowLongW(foreground, GWL_STYLE);
    if (style & WS_CHILD)
        return nullptr;

    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(
            foreground, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) {
        return nullptr;
    }

    RECT windowRect{};
    if (!GetWindowRect(foreground, &windowRect))
        return nullptr;

    HMONITOR monitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONULL);
    if (!monitor)
        return nullptr;

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo))
        return nullptr;

    // Treat both exclusive fullscreen and borderless fullscreen as fullscreen.
    // A two-pixel tolerance covers occasional rounding/DPI edge differences.
    constexpr int kFullscreenTolerancePx = 2;
    const bool fullscreen =
        std::abs(windowRect.left - monitorInfo.rcMonitor.left) <= kFullscreenTolerancePx &&
        std::abs(windowRect.top - monitorInfo.rcMonitor.top) <= kFullscreenTolerancePx &&
        std::abs(windowRect.right - monitorInfo.rcMonitor.right) <= kFullscreenTolerancePx &&
        std::abs(windowRect.bottom - monitorInfo.rcMonitor.bottom) <= kFullscreenTolerancePx;

    return fullscreen ? monitor : nullptr;
}

static bool IsOverlayOnMonitor(HWND overlay, HMONITOR monitor) {
    if (!overlay || !monitor)
        return false;
    return MonitorFromWindow(overlay, MONITOR_DEFAULTTONEAREST) == monitor;
}

static bool AnyOverlayNotFullscreenThrottled(HMONITOR fullscreenMonitor) {
    if (!fullscreenMonitor)
        return true;

    for (HWND overlay : g_overlayWindows) {
        if (!IsOverlayOnMonitor(overlay, fullscreenMonitor))
            return true;
    }

    return false;
}

static UINT GetRenderIntervalMs(const VisualizerSettings& settings) {
    if (g_cachedDisplayRefreshRateHz == 0)
        g_cachedDisplayRefreshRateHz = GetDisplayRefreshRateHz();
    const UINT refreshRateHz = std::max<UINT>(1, g_cachedDisplayRefreshRateHz);

    int effectiveFps = settings.targetFps;
    if (effectiveFps <= 0)
        effectiveFps = static_cast<int>(refreshRateHz);
    else
        effectiveFps = std::min(effectiveFps, static_cast<int>(refreshRateHz));

    effectiveFps = std::max(1, effectiveFps);

    // Use a ceiling so the software timer never intentionally exceeds the
    // requested/display refresh rate.
    return std::max<UINT>(
        1, static_cast<UINT>(std::ceil(1000.0 / effectiveFps)));
}

static UINT GetOverlayTimerIntervalMs(const VisualizerSettings& settings) {
    const HMONITOR fullscreenMonitor =
        g_fullscreenMonitor.load(std::memory_order_acquire);

    // If every rendered overlay is on the fullscreen monitor, the whole timer
    // can sleep at 1 FPS. If at least one monitor still needs normal rendering,
    // keep the shared timer at the configured cadence and throttle only the
    // fullscreen overlay(s) inside WM_TIMER.
    if (fullscreenMonitor && !AnyOverlayNotFullscreenThrottled(fullscreenMonitor))
        return 1000u;

    return g_overlayIdle.load(std::memory_order_acquire)
        ? 200u
        : GetRenderIntervalMs(settings);
}

static bool UpdateFullscreenThrottle(bool forceCheck = false) {
    const ULONGLONG nowMs = GetTickCount64();
    if (!forceCheck && nowMs - g_lastFullscreenCheckMs < 250)
        return g_fullscreenThrottleActive.load(std::memory_order_acquire);

    g_lastFullscreenCheckMs = nowMs;
    const HMONITOR fullscreenMonitor = GetFullscreenForegroundMonitor();
    const HMONITOR previousMonitor = g_fullscreenMonitor.exchange(
        fullscreenMonitor, std::memory_order_acq_rel);
    const bool fullscreen = fullscreenMonitor != nullptr;
    const bool previous = previousMonitor != nullptr;
    g_fullscreenThrottleActive.store(fullscreen, std::memory_order_release);

    if (fullscreenMonitor != previousMonitor) {
        if (g_hwndOverlay && IsWindow(g_hwndOverlay) &&
            !g_layoutEditActive.load(std::memory_order_acquire)) {
            const VisualizerSettings settings = GetSettingsSnapshot();
            const UINT intervalMs = GetOverlayTimerIntervalMs(settings);
            g_currentOverlayTimerMs = intervalMs;
            SetTimer(g_hwndOverlay, 1, intervalMs, nullptr);
        }
    } else if (fullscreen != previous) {
        // Kept for clarity if the state representation changes in the future.
        g_fullscreenThrottleActive.store(fullscreen, std::memory_order_release);
    }

    return fullscreen;
}

// Desktop window layering, top to bottom.
//
// Classic desktop (Windows 10, Windows 11 before 24H2):
//
//   WorkerW (or Progman)          host of the icon layer
//     SHELLDLL_DefView
//       SysListView32 "FolderView" the desktop icons
//   WorkerW                       wallpaper layer, spawned by Progman on 0x052C
//   Progman
//
// "Raised desktop" (Windows 11 24H2+, build 26100+): Progman carries
// WS_EX_NOREDIRECTIONBITMAP and the desktop layers are CHILDREN of Progman:
//
//   Progman
//     SHELLDLL_DefView (or a WorkerW nesting it)   the icon layer
//     WorkerW                                       wallpaper layer
//
// A window parented to SHELLDLL_DefView is a sibling of the icon list and is
// painted ABOVE the icons. To draw BEHIND the icons the overlay lives in the
// wallpaper WorkerW on the classic desktop, and on the raised desktop it is a
// child of Progman stacked between the icon layer and the wallpaper WorkerW.
// A layered child placed INSIDE the raised-desktop WorkerW is not displayed
// at all, which is why the raised layout needs its own path (the recipe used
// by live-wallpaper tools that support 24H2, e.g. RexPaper). Win+D / Show
// Desktop leaves both layers alone.

static HWND FindDesktopProgman() {
    HWND hProgman = GetShellWindow();
    if (!hProgman)
        hProgman = FindWindowW(L"Progman", nullptr);
    if (!hProgman)
        return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(hProgman, &pid);
    return pid == GetCurrentProcessId() ? hProgman : nullptr;
}

static bool IsRaisedDesktop(HWND hProgman) {
    return hProgman &&
           (GetWindowLongPtrW(hProgman, GWL_EXSTYLE) & WS_EX_NOREDIRECTIONBITMAP) != 0;
}

static BOOL CALLBACK FindDefViewChildProc(HWND hwnd, LPARAM lParam) {
    wchar_t cls[32]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        wcscmp(cls, L"SHELLDLL_DefView") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE;
    }
    return TRUE;
}

// Classic: DefView directly under Progman or under a top-level WorkerW.
// Raised: DefView directly under Progman or nested at any depth below it.
static HWND FindDesktopDefView(HWND hProgman) {
    HWND defView = hProgman
        ? FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr)
        : nullptr;
    if (defView)
        return defView;

    HWND worker = nullptr;
    while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr)) != nullptr) {
        defView = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
        if (defView)
            return defView;
    }

    if (hProgman) {
        HWND nested = nullptr;
        EnumChildWindows(hProgman, FindDefViewChildProc,
                         reinterpret_cast<LPARAM>(&nested));
        if (nested)
            return nested;
    }
    return nullptr;
}

// Explorer owns several small, invisible WorkerW windows that have nothing to
// do with the desktop. The wallpaper layer is the visible one that covers the
// same area as Progman and does not itself host the icon layer.
static bool IsWallpaperWorkerWCandidate(HWND hwnd, HWND hProgman) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd))
        return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId())
        return false;

    if (FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr))
        return false;

    RECT r{};
    RECT p{};
    if (!GetWindowRect(hwnd, &r) || !GetWindowRect(hProgman, &p))
        return false;
    return r.left == p.left && r.top == p.top &&
           r.right == p.right && r.bottom == p.bottom;
}

// Classic desktop only.
static HWND FindWallpaperWorkerW(HWND hProgman, HWND defView) {
    if (!hProgman || !defView)
        return nullptr;

    // The wallpaper WorkerW sits right below the top-level WorkerW that
    // hosts SHELLDLL_DefView in the z-order.
    HWND defViewHost = GetAncestor(defView, GA_PARENT);
    if (defViewHost && defViewHost != hProgman &&
        GetAncestor(defViewHost, GA_PARENT) == GetDesktopWindow()) {
        HWND next = defViewHost;
        while ((next = FindWindowExW(nullptr, next, L"WorkerW", nullptr)) != nullptr) {
            if (IsWallpaperWorkerWCandidate(next, hProgman))
                return next;
        }
    }

    // Some builds make it a top-level popup owned by Progman instead.
    HWND worker = nullptr;
    while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr)) != nullptr) {
        if (GetWindow(worker, GW_OWNER) == hProgman &&
            IsWallpaperWorkerWCandidate(worker, hProgman))
            return worker;
    }

    return nullptr;
}

// Raised desktop: the WorkerW child of Progman that does not host the icons.
static HWND FindRaisedWallpaperWorkerW(HWND hProgman, HWND iconHost) {
    HWND worker = nullptr;
    while ((worker = FindWindowExW(hProgman, worker, L"WorkerW", nullptr)) != nullptr) {
        if (worker == iconHost)
            continue;
        if (FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr))
            continue;
        return worker;
    }
    return nullptr;
}

// Raised desktop stacking: icon layer on top, overlay right below it,
// wallpaper WorkerW at the bottom. Explorer may restack its children on
// wallpaper or theme changes, so the repair pass calls this too. No-op for
// the other placements.
static void ReassertOverlayZOrder() {
    // Only the raised desktop behind-icons placements set insertAfter.
    const HWND insertAfter = g_overlayPlacement.insertAfter;
    if (!g_overlayPlacement.raised || !insertAfter)
        return;
    if (IsWindow(insertAfter)) {
        for (size_t i = 0; i < g_overlayWindows.size(); ++i) {
            // Stack the holder when there is one; the overlay inside follows.
            HWND stacked = (i < g_overlayHosts.size() && g_overlayHosts[i])
                ? g_overlayHosts[i] : g_overlayWindows[i];
            if (stacked && IsWindow(stacked)) {
                SetWindowPos(stacked, insertAfter, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        }
    }
    const HWND wallpaper = g_overlayPlacement.wallpaperWorkerW;
    if (wallpaper && IsWindow(wallpaper)) {
        SetWindowPos(wallpaper, HWND_BOTTOM, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

// Returns the window the overlay should be a child of and records the full
// placement in g_overlayPlacement. Returns nullptr while the desktop is not
// ready yet. The 0x052C spawn request is sent once per Progman instance.
// placementSetting is VisualizerSettings::desktopPlacement.
static HWND ResolveOverlayDesktopParent(int placementSetting) {
    static HWND s_spawnRequestedFor = nullptr;
    static HWND s_lastLoggedParent = nullptr;
    static int s_lastLoggedMode = -1;

    HWND hProgman = FindDesktopProgman();
    if (!hProgman)
        return nullptr;

    const bool raised = IsRaisedDesktop(hProgman);
    if (hProgman != s_spawnRequestedFor) {
        s_spawnRequestedFor = hProgman;
        // Undocumented Progman message that makes it create/reposition the
        // wallpaper WorkerW behind the icons. (0xD, 1) is the canonical spawn
        // request. On the raised desktop a trailing (0xD, 0) removes the
        // freshly created WorkerW again, so that variant is only sent on the
        // classic desktop, where both are harmless.
        if (!raised)
            SendMessageTimeoutW(hProgman, 0x052C, 0xD, 0, SMTO_ABORTIFHUNG, 1000, nullptr);
        SendMessageTimeoutW(hProgman, 0x052C, 0xD, 1, SMTO_ABORTIFHUNG, 1000, nullptr);
    }

    HWND defView = FindDesktopDefView(hProgman);
    if (!defView)
        return nullptr;

    OverlayDesktopPlacement placement{};
    placement.raised = raised;
    placement.renderMode = OVERLAY_RENDER_ULW;
    const wchar_t* description = L"";

    if (placementSetting == DESKTOP_PLACEMENT_ABOVE_ICONS) {
        // The original placement: a sibling of the icon list, above it.
        // Renders everywhere, including the raised desktop.
        placement.parent = defView;
        description = L"above icons (setting): child of SHELLDLL_DefView";
    } else if (raised) {
        // The direct child of Progman that hosts the icon view.
        HWND iconHost = defView;
        for (;;) {
            HWND parent = GetAncestor(iconHost, GA_PARENT);
            if (!parent || parent == hProgman)
                break;
            iconHost = parent;
        }
        placement.parent = hProgman;
        placement.insertAfter = iconHost;
        placement.wallpaperWorkerW = FindRaisedWallpaperWorkerW(hProgman, iconHost);
        placement.behindIcons = true;
        // UpdateLayeredWindow content is never composited here, see
        // docs/research/2026-09-26-win11-raised-desktop.md.
        if (placementSetting == DESKTOP_PLACEMENT_TRANSPARENT) {
            placement.renderMode = OVERLAY_RENDER_DCOMP;
            description = L"raised desktop: DirectComposition child of Progman below the icon layer";
        } else {
            placement.renderMode = OVERLAY_RENDER_OPAQUE_HOLDER;
            description = L"raised desktop: opaque holder child of Progman below the icon layer";
        }
    } else {
        HWND wallpaper = FindWallpaperWorkerW(hProgman, defView);
        if (wallpaper) {
            placement.parent = wallpaper;
            placement.behindIcons = true;
            description = L"classic desktop: wallpaper WorkerW, behind icons";
        } else {
            placement.parent = defView;
            description = L"SHELLDLL_DefView fallback, above icons";
        }
    }
    g_overlayPlacement = placement;

    if (placement.parent != s_lastLoggedParent || placement.renderMode != s_lastLoggedMode) {
        s_lastLoggedParent = placement.parent;
        s_lastLoggedMode = placement.renderMode;
        Wh_Log(L"Overlay desktop parent: %s (parent=%p insertAfter=%p wallpaperWorkerW=%p mode=%d)",
               description, placement.parent, placement.insertAfter,
               placement.wallpaperWorkerW, placement.renderMode);
    }
    return placement.parent;
}

// ===========================================================================
// Desktop layout editor
// ===========================================================================
// A dedicated edit mode: the desktop overlay is frozen (render timer stopped,
// repair passes paused), a dimmed top-level surface covers the overlay's
// monitor, and the visualizer / album widget / lyrics can be dragged or
// nudged. Nothing is persisted until Apply. Cancel restores the entry state.
// The editor runs on the overlay thread and shares its state with the
// overlay renderer, so dragging re-renders the frozen overlay at the new
// position at a throttled rate instead of every mouse move.

static constexpr int LAYOUT_TARGET_NONE = -1;
static constexpr int LAYOUT_TARGET_VISUALIZER = 0;
static constexpr int LAYOUT_TARGET_ALBUM = 1;
static constexpr int LAYOUT_TARGET_LYRICS = 2;

static constexpr int LAYOUT_BUTTON_NONE = -1;
static constexpr int LAYOUT_BUTTON_APPLY = 0;
static constexpr int LAYOUT_BUTTON_RESET = 1;
static constexpr int LAYOUT_BUTTON_CANCEL = 2;

static constexpr UINT kLayoutEditTimerId = 1;
static constexpr UINT kLayoutEditTimerMs = 30;
static const wchar_t* kLayoutEditClass = L"WindhawkVisualizerLayoutEditor";
static bool g_layoutEditClassRegistered = false;

struct LayoutEditState {
    HWND hwnd = nullptr;
    RECT screenRect{};
    int width = 0;
    int height = 0;
    double scale = 1.0;

    int dragTarget = LAYOUT_TARGET_NONE;
    int hoverTarget = LAYOUT_TARGET_NONE;
    int selectedTarget = LAYOUT_TARGET_VISUALIZER;
    int hotButton = LAYOUT_BUTTON_NONE;
    POINT dragStartCursor{};
    POINT dragStartOrigin{};
    bool dirty = false;

    // State at entry, restored on cancel.
    int origVisualizerX = 0;
    int origVisualizerY = 0;
    int origLyricsX = 0;
    int origLyricsY = 0;
    int origAlbumSeparateX = 0;
    int origAlbumSeparateY = 0;
    int origAlbumLyricsOffsetX = 0;
    int origAlbumLyricsOffsetY = 0;
    int origAlbumVisualizerOffsetX = 0;
    int origAlbumVisualizerOffsetY = 0;

    HDC memDC = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    void* bits = nullptr;
};
static LayoutEditState g_layoutEdit;

static LRESULT CALLBACK LayoutEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct LayoutEditToolbar {
    RECT panel{};
    RECT apply{};
    RECT reset{};
    RECT cancel{};
};

static int LayoutEditPx(double v) {
    return static_cast<int>(std::lround(v * g_layoutEdit.scale));
}

static LayoutEditToolbar LayoutEditGetToolbar() {
    LayoutEditToolbar t{};
    const int panelW = std::min(g_layoutEdit.width - LayoutEditPx(32), LayoutEditPx(920));
    const int panelH = LayoutEditPx(132);
    const int left = (g_layoutEdit.width - panelW) / 2;
    const int top = LayoutEditPx(24);
    t.panel = RECT{left, top, left + panelW, top + panelH};

    const int buttonH = LayoutEditPx(34);
    const int buttonTop = top + panelH - buttonH - LayoutEditPx(14);
    const int gap = LayoutEditPx(10);
    int x = left + LayoutEditPx(18);
    t.apply = RECT{x, buttonTop, x + LayoutEditPx(110), buttonTop + buttonH};
    x = t.apply.right + gap;
    t.reset = RECT{x, buttonTop, x + LayoutEditPx(170), buttonTop + buttonH};
    x = t.reset.right + gap;
    t.cancel = RECT{x, buttonTop, x + LayoutEditPx(110), buttonTop + buttonH};
    return t;
}

// The helpers below read g_settings; callers hold g_settingsMutex.
static bool LayoutEditTargetAvailable(int target) {
    switch (target) {
    case LAYOUT_TARGET_VISUALIZER:
        return true;
    case LAYOUT_TARGET_ALBUM:
        return g_settings.albumWidgetEnabled;
    case LAYOUT_TARGET_LYRICS:
        return g_settings.lyricsEnabled;
    }
    return false;
}

static const wchar_t* LayoutEditTargetName(int target) {
    switch (target) {
    case LAYOUT_TARGET_VISUALIZER: return L"Visualizer";
    case LAYOUT_TARGET_ALBUM: return L"Album widget";
    case LAYOUT_TARGET_LYRICS: return L"Lyrics";
    }
    return L"";
}

static RECT LayoutEditTargetRect(int target) {
    switch (target) {
    case LAYOUT_TARGET_VISUALIZER: {
        const int barCount = std::clamp(g_settings.barCount, 1, VIZ_BANDS_MAX);
        return GetVisualizerBackgroundRect(
            barCount, 1.0f, std::max(6, g_settings.backgroundPadding),
            g_settings.backgroundHeightAdjustment);
    }
    case LAYOUT_TARGET_ALBUM:
        return GetThirdAlbumWidgetRect(g_settings);
    case LAYOUT_TARGET_LYRICS:
        return RECT{g_settings.lyricsX, g_settings.lyricsY,
                    g_settings.lyricsX + g_settings.lyricsWidth,
                    g_settings.lyricsY + g_settings.lyricsHeight};
    }
    return RECT{};
}

// The point the stored coordinates refer to (anchor for the visualizer,
// top-left for the widgets).
static POINT LayoutEditTargetOrigin(int target) {
    switch (target) {
    case LAYOUT_TARGET_VISUALIZER:
        return POINT{g_settings.positionX, g_settings.positionY};
    case LAYOUT_TARGET_LYRICS:
        return POINT{g_settings.lyricsX, g_settings.lyricsY};
    case LAYOUT_TARGET_ALBUM: {
        const RECT r = GetThirdAlbumWidgetRect(g_settings);
        return POINT{r.left, r.top};
    }
    }
    return POINT{};
}

// Caller holds a unique lock on g_settingsMutex.
static void LayoutEditSetTargetOrigin(int target, int x, int y) {
    switch (target) {
    case LAYOUT_TARGET_VISUALIZER:
        g_settings.positionX = x;
        g_settings.positionY = y;
        break;
    case LAYOUT_TARGET_LYRICS:
        g_settings.lyricsX = x;
        g_settings.lyricsY = y;
        break;
    case LAYOUT_TARGET_ALBUM: {
        const RECT desired{x, y, x + g_settings.albumWidgetWidth,
                           y + g_settings.albumWidgetHeight};
        // Attached widgets are clamped to their anchor exactly like the
        // legacy on-desktop drag did.
        UpdateThirdAlbumWidgetRectInMemory(g_settings, desired);
        break;
    }
    }
}

static int LayoutEditHitTarget(int x, int y) {
    std::shared_lock<std::shared_mutex> lock(g_settingsMutex);
    // Smaller items first so they stay grabbable when overlapping the bars.
    const int order[] = {LAYOUT_TARGET_ALBUM, LAYOUT_TARGET_LYRICS, LAYOUT_TARGET_VISUALIZER};
    for (int target : order) {
        if (!LayoutEditTargetAvailable(target))
            continue;
        const RECT r = LayoutEditTargetRect(target);
        if (PtInRect(&r, POINT{x, y}))
            return target;
    }
    return LAYOUT_TARGET_NONE;
}

static int LayoutEditHitButton(int x, int y) {
    const LayoutEditToolbar t = LayoutEditGetToolbar();
    const POINT p{x, y};
    if (PtInRect(&t.apply, p)) return LAYOUT_BUTTON_APPLY;
    if (PtInRect(&t.reset, p)) return LAYOUT_BUTTON_RESET;
    if (PtInRect(&t.cancel, p)) return LAYOUT_BUTTON_CANCEL;
    return LAYOUT_BUTTON_NONE;
}

static void LayoutEditReleaseSurface() {
    if (g_layoutEdit.memDC && g_layoutEdit.oldBitmap)
        SelectObject(g_layoutEdit.memDC, g_layoutEdit.oldBitmap);
    if (g_layoutEdit.bitmap)
        DeleteObject(g_layoutEdit.bitmap);
    if (g_layoutEdit.memDC)
        DeleteDC(g_layoutEdit.memDC);
    g_layoutEdit.memDC = nullptr;
    g_layoutEdit.bitmap = nullptr;
    g_layoutEdit.oldBitmap = nullptr;
    g_layoutEdit.bits = nullptr;
}

static bool LayoutEditEnsureSurface() {
    if (g_layoutEdit.memDC && g_layoutEdit.bitmap && g_layoutEdit.bits)
        return true;
    LayoutEditReleaseSurface();

    HDC screen = GetDC(nullptr);
    g_layoutEdit.memDC = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g_layoutEdit.width;
    bi.bmiHeader.biHeight = -g_layoutEdit.height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_layoutEdit.bitmap = CreateDIBSection(
        screen, &bi, DIB_RGB_COLORS, &g_layoutEdit.bits, nullptr, 0);
    ReleaseDC(nullptr, screen);

    if (!g_layoutEdit.memDC || !g_layoutEdit.bitmap || !g_layoutEdit.bits) {
        LayoutEditReleaseSurface();
        return false;
    }
    g_layoutEdit.oldBitmap = SelectObject(g_layoutEdit.memDC, g_layoutEdit.bitmap);
    return true;
}

static void LayoutEditDrawButton(
    Gdiplus::Graphics& graphics, const RECT& rect, const wchar_t* text,
    bool accent, bool hot, Gdiplus::Font& font) {
    Gdiplus::Color fill = accent
        ? (hot ? Gdiplus::Color(255, 135, 185, 255) : Gdiplus::Color(255, 105, 165, 255))
        : (hot ? Gdiplus::Color(255, 96, 96, 100) : Gdiplus::Color(255, 70, 70, 74));
    Gdiplus::SolidBrush brush(fill);
    Gdiplus::GraphicsPath path;
    AddRoundedRectSubpath(path, static_cast<float>(rect.left), static_cast<float>(rect.top),
                          static_cast<float>(rect.right - rect.left),
                          static_cast<float>(rect.bottom - rect.top),
                          static_cast<float>(LayoutEditPx(9)));
    graphics.FillPath(&brush, &path);
    Gdiplus::StringFormat center;
    center.SetAlignment(Gdiplus::StringAlignmentCenter);
    center.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
    graphics.DrawString(text, -1, &font,
                        Gdiplus::RectF(static_cast<float>(rect.left), static_cast<float>(rect.top),
                                       static_cast<float>(rect.right - rect.left),
                                       static_cast<float>(rect.bottom - rect.top)),
                        &center, &textBrush);
}

static void LayoutEditRender() {
    if (!g_layoutEdit.hwnd || !IsWindow(g_layoutEdit.hwnd))
        return;
    if (!LayoutEditEnsureSurface())
        return;

    const int w = g_layoutEdit.width;
    const int h = g_layoutEdit.height;

    // Dim the desktop so edit mode is unmistakable. Premultiplied BGRA:
    // black at alpha 64.
    std::fill_n(static_cast<DWORD*>(g_layoutEdit.bits),
                static_cast<size_t>(w) * static_cast<size_t>(h), 0x40000000u);

    Gdiplus::Graphics graphics(g_layoutEdit.memDC);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

    const Gdiplus::Color accent(255, 105, 165, 255);
    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold");
    Gdiplus::FontFamily bodyFamily(L"Segoe UI");
    Gdiplus::Font titleFont(&titleFamily, static_cast<float>(LayoutEditPx(17)),
                            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font bodyFont(&bodyFamily, static_cast<float>(LayoutEditPx(12.5)),
                           Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font labelFont(&titleFamily, static_cast<float>(LayoutEditPx(12.5)),
                            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font buttonFont(&titleFamily, static_cast<float>(LayoutEditPx(13)),
                             Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 245, 245, 248));
    Gdiplus::SolidBrush mutedBrush(Gdiplus::Color(215, 200, 200, 205));
    Gdiplus::SolidBrush accentBrush(accent);

    std::wstring coords;
    {
        std::shared_lock<std::shared_mutex> lock(g_settingsMutex);
        const int order[] = {LAYOUT_TARGET_VISUALIZER, LAYOUT_TARGET_LYRICS, LAYOUT_TARGET_ALBUM};
        for (int target : order) {
            if (!LayoutEditTargetAvailable(target))
                continue;
            const RECT r = LayoutEditTargetRect(target);
            const POINT origin = LayoutEditTargetOrigin(target);
            const bool selected = target == g_layoutEdit.selectedTarget;
            const bool hot = target == g_layoutEdit.hoverTarget ||
                             target == g_layoutEdit.dragTarget;

            Gdiplus::GraphicsPath path;
            AddRoundedRectSubpath(path, static_cast<float>(r.left), static_cast<float>(r.top),
                                  static_cast<float>(std::max(1L, r.right - r.left)),
                                  static_cast<float>(std::max(1L, r.bottom - r.top)),
                                  static_cast<float>(LayoutEditPx(8)));
            Gdiplus::SolidBrush fill(selected
                ? Gdiplus::Color(hot ? 70 : 48, 105, 165, 255)
                : Gdiplus::Color(hot ? 60 : 34, 255, 255, 255));
            Gdiplus::Pen pen(selected ? accent : Gdiplus::Color(230, 235, 235, 235),
                             static_cast<float>(LayoutEditPx(2)));
            graphics.FillPath(&fill, &path);
            graphics.DrawPath(&pen, &path);

            if (target == LAYOUT_TARGET_VISUALIZER) {
                // Mark the anchor the X/Y settings refer to.
                Gdiplus::Pen crossPen(Gdiplus::Color(255, 255, 255, 255),
                                      static_cast<float>(LayoutEditPx(1.5)));
                const float cx = static_cast<float>(origin.x);
                const float cy = static_cast<float>(origin.y);
                const float arm = static_cast<float>(LayoutEditPx(9));
                graphics.DrawLine(&crossPen, cx - arm, cy, cx + arm, cy);
                graphics.DrawLine(&crossPen, cx, cy - arm, cx, cy + arm);
            }

            wchar_t label[96]{};
            swprintf_s(label, L"%s   X %d   Y %d", LayoutEditTargetName(target),
                       origin.x, origin.y);
            Gdiplus::RectF measured;
            graphics.MeasureString(label, -1, &labelFont, Gdiplus::PointF(0.0f, 0.0f), &measured);
            const int pillW = static_cast<int>(measured.Width) + LayoutEditPx(20);
            const int pillH = LayoutEditPx(26);
            int pillX = std::clamp<int>(r.left, LayoutEditPx(8), std::max(LayoutEditPx(8), w - pillW - LayoutEditPx(8)));
            int pillY = r.top - pillH - LayoutEditPx(6);
            if (pillY < LayoutEditPx(8))
                pillY = r.top + LayoutEditPx(6);
            Gdiplus::GraphicsPath pill;
            AddRoundedRectSubpath(pill, static_cast<float>(pillX), static_cast<float>(pillY),
                                  static_cast<float>(pillW), static_cast<float>(pillH),
                                  static_cast<float>(pillH / 2));
            Gdiplus::SolidBrush pillBrush(selected ? accent : Gdiplus::Color(225, 32, 32, 36));
            graphics.FillPath(&pillBrush, &pill);
            Gdiplus::StringFormat center;
            center.SetAlignment(Gdiplus::StringAlignmentCenter);
            center.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            graphics.DrawString(label, -1, &labelFont,
                                Gdiplus::RectF(static_cast<float>(pillX), static_cast<float>(pillY),
                                               static_cast<float>(pillW), static_cast<float>(pillH)),
                                &center, &whiteBrush);

            wchar_t part[64]{};
            swprintf_s(part, L"%s%s X %d  Y %d", coords.empty() ? L"" : L"     ",
                       LayoutEditTargetName(target), origin.x, origin.y);
            coords += part;
        }
    }

    // Toolbar
    const LayoutEditToolbar t = LayoutEditGetToolbar();
    {
        Gdiplus::GraphicsPath panel;
        AddRoundedRectSubpath(panel, static_cast<float>(t.panel.left), static_cast<float>(t.panel.top),
                              static_cast<float>(t.panel.right - t.panel.left),
                              static_cast<float>(t.panel.bottom - t.panel.top),
                              static_cast<float>(LayoutEditPx(14)));
        Gdiplus::SolidBrush panelBrush(Gdiplus::Color(238, 28, 28, 30));
        Gdiplus::Pen panelPen(Gdiplus::Color(80, 255, 255, 255), 1.0f);
        graphics.FillPath(&panelBrush, &panel);
        graphics.DrawPath(&panelPen, &panel);

        const float textLeft = static_cast<float>(t.panel.left + LayoutEditPx(18));
        const float textWidth = static_cast<float>(t.panel.right - t.panel.left - LayoutEditPx(36));
        graphics.DrawString(L"Desktop layout editor", -1, &titleFont,
                            Gdiplus::PointF(textLeft, static_cast<float>(t.panel.top + LayoutEditPx(12))),
                            &whiteBrush);
        graphics.DrawString(
            L"Drag the visualizer, album widget or lyrics. Arrow keys nudge the selected item "
            L"(Shift = 10 px). Enter applies, Esc cancels. Applied positions override the Windhawk "
            L"settings until those settings are changed again.",
            -1, &bodyFont,
            Gdiplus::RectF(textLeft, static_cast<float>(t.panel.top + LayoutEditPx(38)),
                           textWidth, static_cast<float>(LayoutEditPx(36))),
            nullptr, &mutedBrush);
        graphics.DrawString(coords.c_str(), -1, &labelFont,
                            Gdiplus::PointF(static_cast<float>(t.cancel.right + LayoutEditPx(18)),
                                            static_cast<float>(t.apply.top + LayoutEditPx(8))),
                            &accentBrush);

        LayoutEditDrawButton(graphics, t.apply, L"Apply", true,
                             g_layoutEdit.hotButton == LAYOUT_BUTTON_APPLY, buttonFont);
        LayoutEditDrawButton(graphics, t.reset, L"Reset to settings", false,
                             g_layoutEdit.hotButton == LAYOUT_BUTTON_RESET, buttonFont);
        LayoutEditDrawButton(graphics, t.cancel, L"Cancel", false,
                             g_layoutEdit.hotButton == LAYOUT_BUTTON_CANCEL, buttonFont);
    }

    POINT dstPos{g_layoutEdit.screenRect.left, g_layoutEdit.screenRect.top};
    SIZE size{w, h};
    POINT srcPos{0, 0};
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    HDC screen = GetDC(nullptr);
    UpdateLayeredWindow(g_layoutEdit.hwnd, screen, &dstPos, &size,
                        g_layoutEdit.memDC, &srcPos, 0, &blend, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
}

static void LayoutEditRenderOverlays() {
    std::shared_lock<std::shared_mutex> lock(g_settingsMutex);
    for (HWND overlay : g_overlayWindows)
        RenderOverlay(overlay);
}

static void LayoutEditResumeOverlay() {
    if (!g_hwndOverlay || !IsWindow(g_hwndOverlay))
        return;
    const VisualizerSettings settings = GetSettingsSnapshot();
    g_currentOverlayTimerMs = GetRenderIntervalMs(settings);
    SetTimer(g_hwndOverlay, 1, g_currentOverlayTimerMs, nullptr);
    LayoutEditRenderOverlays();
}

static void LayoutEditResetToSettings() {
    const int vx = Wh_GetIntSetting(L"Visualizer.positionX");
    const int vy = Wh_GetIntSetting(L"Visualizer.positionY");
    const int lx = Wh_GetIntSetting(L"Lyrics.positionX");
    const int ly = Wh_GetIntSetting(L"Lyrics.positionY");
    std::unique_lock<std::shared_mutex> lock(g_settingsMutex);
    g_settings.positionX = vx;
    g_settings.positionY = vy;
    g_settings.lyricsX = lx;
    g_settings.lyricsY = ly;
}

static void LayoutEditBegin() {
    if (g_layoutEdit.hwnd && IsWindow(g_layoutEdit.hwnd)) {
        SetForegroundWindow(g_layoutEdit.hwnd);
        return;
    }
    if (!g_hwndOverlay || !IsWindow(g_hwndOverlay)) {
        Wh_Log(L"LayoutEditBegin: overlay window not ready");
        return;
    }

    HINSTANCE instance = GetCurrentModModuleHandle();
    if (!g_layoutEditClassRegistered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = LayoutEditProc;
        wc.hInstance = instance;
        wc.lpszClassName = kLayoutEditClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        if (!RegisterClassW(&wc)) {
            Wh_Log(L"LayoutEditBegin: RegisterClass failed, error=%lu", GetLastError());
            return;
        }
        g_layoutEditClassRegistered = true;
    }

    RECT rect{};
    if (!GetWindowRect(g_hwndOverlay, &rect))
        return;

    g_layoutEdit = LayoutEditState{};
    g_layoutEdit.screenRect = rect;
    g_layoutEdit.width = std::max(1L, rect.right - rect.left);
    g_layoutEdit.height = std::max(1L, rect.bottom - rect.top);
    {
        std::shared_lock<std::shared_mutex> lock(g_settingsMutex);
        g_layoutEdit.origVisualizerX = g_settings.positionX;
        g_layoutEdit.origVisualizerY = g_settings.positionY;
        g_layoutEdit.origLyricsX = g_settings.lyricsX;
        g_layoutEdit.origLyricsY = g_settings.lyricsY;
    }
    g_layoutEdit.origAlbumSeparateX = g_albumWidgetSeparateX.load(std::memory_order_acquire);
    g_layoutEdit.origAlbumSeparateY = g_albumWidgetSeparateY.load(std::memory_order_acquire);
    g_layoutEdit.origAlbumLyricsOffsetX = g_albumWidgetLyricsOffsetX.load(std::memory_order_acquire);
    g_layoutEdit.origAlbumLyricsOffsetY = g_albumWidgetLyricsOffsetY.load(std::memory_order_acquire);
    g_layoutEdit.origAlbumVisualizerOffsetX = g_albumWidgetVisualizerOffsetX.load(std::memory_order_acquire);
    g_layoutEdit.origAlbumVisualizerOffsetY = g_albumWidgetVisualizerOffsetY.load(std::memory_order_acquire);

    // Freeze the desktop overlay for the whole edit session.
    g_layoutEditActive.store(true, std::memory_order_release);
    KillTimer(g_hwndOverlay, 1);

    g_layoutEdit.hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kLayoutEditClass, L"Desktop layout editor", WS_POPUP,
        rect.left, rect.top, g_layoutEdit.width, g_layoutEdit.height,
        nullptr, nullptr, instance, nullptr);
    if (!g_layoutEdit.hwnd) {
        Wh_Log(L"LayoutEditBegin: CreateWindowEx failed, error=%lu", GetLastError());
        g_layoutEditActive.store(false, std::memory_order_release);
        LayoutEditResumeOverlay();
        return;
    }

    const UINT dpi = GetDpiForWindow(g_layoutEdit.hwnd);
    g_layoutEdit.scale = std::max(0.5, (dpi ? dpi : 96) / 96.0);

    LayoutEditRender();
    ShowWindow(g_layoutEdit.hwnd, SW_SHOWNOACTIVATE);
    SetWindowPos(g_layoutEdit.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
    SetForegroundWindow(g_layoutEdit.hwnd);
    SetFocus(g_layoutEdit.hwnd);
    SetTimer(g_layoutEdit.hwnd, kLayoutEditTimerId, kLayoutEditTimerMs, nullptr);
    Wh_Log(L"Layout editor opened on (%ld,%ld)-(%ld,%ld)",
           rect.left, rect.top, rect.right, rect.bottom);
}

static void LayoutEditEnd(bool apply) {
    HWND hwnd = g_layoutEdit.hwnd;
    if (!hwnd)
        return;
    g_layoutEdit.hwnd = nullptr;

    if (g_layoutEdit.dragTarget != LAYOUT_TARGET_NONE) {
        g_layoutEdit.dragTarget = LAYOUT_TARGET_NONE;
        if (GetCapture() == hwnd)
            ReleaseCapture();
    }

    VisualizerSettings snapshot{};
    if (apply) {
        snapshot = GetSettingsSnapshot();
    } else {
        std::unique_lock<std::shared_mutex> lock(g_settingsMutex);
        g_settings.positionX = g_layoutEdit.origVisualizerX;
        g_settings.positionY = g_layoutEdit.origVisualizerY;
        g_settings.lyricsX = g_layoutEdit.origLyricsX;
        g_settings.lyricsY = g_layoutEdit.origLyricsY;
        g_albumWidgetSeparateX.store(g_layoutEdit.origAlbumSeparateX, std::memory_order_release);
        g_albumWidgetSeparateY.store(g_layoutEdit.origAlbumSeparateY, std::memory_order_release);
        g_albumWidgetLyricsOffsetX.store(g_layoutEdit.origAlbumLyricsOffsetX, std::memory_order_release);
        g_albumWidgetLyricsOffsetY.store(g_layoutEdit.origAlbumLyricsOffsetY, std::memory_order_release);
        g_albumWidgetVisualizerOffsetX.store(g_layoutEdit.origAlbumVisualizerOffsetX, std::memory_order_release);
        g_albumWidgetVisualizerOffsetY.store(g_layoutEdit.origAlbumVisualizerOffsetY, std::memory_order_release);
    }

    if (IsWindow(hwnd))
        DestroyWindow(hwnd);
    g_layoutEditActive.store(false, std::memory_order_release);
    LayoutEditResumeOverlay();

    if (!apply) {
        Wh_Log(L"Layout editor: cancelled");
        return;
    }

    // Persist last, after the editor is gone and the overlay is live again:
    // storage writes may re-enter Wh_ModSettingsChanged synchronously on
    // some builds, which reloads settings and rebuilds the overlay.
    StoreLayoutOverride(kVisualizerLayoutKeys, snapshot.positionX, snapshot.positionY,
                        Wh_GetIntSetting(L"Visualizer.positionX"),
                        Wh_GetIntSetting(L"Visualizer.positionY"));
    if (snapshot.lyricsEnabled) {
        StoreLayoutOverride(kLyricsLayoutKeys, snapshot.lyricsX, snapshot.lyricsY,
                            Wh_GetIntSetting(L"Lyrics.positionX"),
                            Wh_GetIntSetting(L"Lyrics.positionY"));
    }
    if (snapshot.albumWidgetEnabled)
        StoreThirdAlbumWidgetRect(snapshot, GetThirdAlbumWidgetRect(snapshot));
    Wh_Log(L"Layout editor: applied visualizer (%d,%d), lyrics (%d,%d)",
           snapshot.positionX, snapshot.positionY, snapshot.lyricsX, snapshot.lyricsY);
}

static LRESULT CALLBACK LayoutEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_ACTIVATE;
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_SETCURSOR: {
        POINT p{};
        GetCursorPos(&p);
        ScreenToClient(hwnd, &p);
        LPCWSTR cursor = IDC_ARROW;
        if (g_layoutEdit.dragTarget != LAYOUT_TARGET_NONE ||
            LayoutEditHitTarget(p.x, p.y) != LAYOUT_TARGET_NONE)
            cursor = IDC_SIZEALL;
        else if (LayoutEditHitButton(p.x, p.y) != LAYOUT_BUTTON_NONE)
            cursor = IDC_HAND;
        SetCursor(LoadCursorW(nullptr, cursor));
        return TRUE;
    }
    case WM_MOUSEMOVE: {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        if (g_layoutEdit.dragTarget != LAYOUT_TARGET_NONE) {
            POINT cursor{};
            if (!GetCursorPos(&cursor))
                return 0;
            const int nx = g_layoutEdit.dragStartOrigin.x + (cursor.x - g_layoutEdit.dragStartCursor.x);
            const int ny = g_layoutEdit.dragStartOrigin.y + (cursor.y - g_layoutEdit.dragStartCursor.y);
            {
                std::unique_lock<std::shared_mutex> lock(g_settingsMutex);
                LayoutEditSetTargetOrigin(g_layoutEdit.dragTarget, nx, ny);
            }
            g_layoutEdit.dirty = true;
            return 0;
        }
        const int hover = LayoutEditHitTarget(x, y);
        const int hot = hover == LAYOUT_TARGET_NONE ? LayoutEditHitButton(x, y) : LAYOUT_BUTTON_NONE;
        if (hover != g_layoutEdit.hoverTarget || hot != g_layoutEdit.hotButton) {
            g_layoutEdit.hoverTarget = hover;
            g_layoutEdit.hotButton = hot;
            g_layoutEdit.dirty = true;
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        SetFocus(hwnd);
        const int button = LayoutEditHitButton(x, y);
        if (button == LAYOUT_BUTTON_APPLY) {
            LayoutEditEnd(true);
            return 0;
        }
        if (button == LAYOUT_BUTTON_CANCEL) {
            LayoutEditEnd(false);
            return 0;
        }
        if (button == LAYOUT_BUTTON_RESET) {
            LayoutEditResetToSettings();
            g_layoutEdit.dirty = true;
            return 0;
        }
        const int target = LayoutEditHitTarget(x, y);
        if (target != LAYOUT_TARGET_NONE) {
            g_layoutEdit.selectedTarget = target;
            g_layoutEdit.dragTarget = target;
            GetCursorPos(&g_layoutEdit.dragStartCursor);
            {
                std::shared_lock<std::shared_mutex> lock(g_settingsMutex);
                g_layoutEdit.dragStartOrigin = LayoutEditTargetOrigin(target);
            }
            SetCapture(hwnd);
            g_layoutEdit.dirty = true;
        }
        return 0;
    }
    case WM_LBUTTONUP:
        if (g_layoutEdit.dragTarget != LAYOUT_TARGET_NONE) {
            g_layoutEdit.dragTarget = LAYOUT_TARGET_NONE;
            ReleaseCapture();
            g_layoutEdit.dirty = true;
        }
        return 0;
    case WM_CAPTURECHANGED:
        if (g_layoutEdit.dragTarget != LAYOUT_TARGET_NONE) {
            g_layoutEdit.dragTarget = LAYOUT_TARGET_NONE;
            g_layoutEdit.dirty = true;
        }
        return 0;
    case WM_KEYDOWN: {
        if (wParam == VK_ESCAPE) {
            LayoutEditEnd(false);
            return 0;
        }
        if (wParam == VK_RETURN) {
            LayoutEditEnd(true);
            return 0;
        }
        if (wParam == VK_TAB) {
            // Cycle the selection through the available items.
            std::shared_lock<std::shared_mutex> lock(g_settingsMutex);
            for (int i = 1; i <= 3; ++i) {
                const int candidate = (g_layoutEdit.selectedTarget + i) % 3;
                if (LayoutEditTargetAvailable(candidate)) {
                    g_layoutEdit.selectedTarget = candidate;
                    break;
                }
            }
            g_layoutEdit.dirty = true;
            return 0;
        }
        int dx = 0;
        int dy = 0;
        switch (wParam) {
        case VK_LEFT: dx = -1; break;
        case VK_RIGHT: dx = 1; break;
        case VK_UP: dy = -1; break;
        case VK_DOWN: dy = 1; break;
        default: return 0;
        }
        const int step = (GetKeyState(VK_SHIFT) & 0x8000) ? 10 : 1;
        {
            std::unique_lock<std::shared_mutex> lock(g_settingsMutex);
            if (LayoutEditTargetAvailable(g_layoutEdit.selectedTarget)) {
                const POINT o = LayoutEditTargetOrigin(g_layoutEdit.selectedTarget);
                LayoutEditSetTargetOrigin(g_layoutEdit.selectedTarget,
                                          o.x + dx * step, o.y + dy * step);
            }
        }
        g_layoutEdit.dirty = true;
        return 0;
    }
    case WM_TIMER:
        if (wParam == kLayoutEditTimerId && g_layoutEdit.dirty) {
            // Coalesce mouse moves: at most one overlay + editor frame per tick.
            g_layoutEdit.dirty = false;
            LayoutEditRenderOverlays();
            LayoutEditRender();
        }
        return 0;
    case WM_CLOSE:
        LayoutEditEnd(false);
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, kLayoutEditTimerId);
        LayoutEditReleaseSurface();
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void LayoutEditUnregisterClass(HINSTANCE instance) {
    if (!g_layoutEditClassRegistered)
        return;
    UnregisterClassW(kLayoutEditClass, instance);
    g_layoutEditClassRegistered = false;
}

static DWORD WINAPI OverlayThreadProc(LPVOID) {
    Gdiplus::GdiplusStartupInput gdiplusInput{};
    const Gdiplus::Status gdiplusStatus =
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);
    if (gdiplusStatus != Gdiplus::Ok) {
        g_gdiplusToken = 0;
        return 0;
    }

    MSG bootstrapMsg{};
    PeekMessageW(&bootstrapMsg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    WNDCLASSW wc{};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetCurrentModModuleHandle();
    if (!wc.hInstance) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return 0;
    }
    wc.lpszClassName = L"WindhawkVisualizerOverlay";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    bool classRegistered = false;
    if (RegisterClassW(&wc)) {
        classRegistered = true;
    } else {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return 0;
    }

    // Register the EQ window classes now, but do not create process-owned tray
    // windows or worker threads until this Explorer process actually owns the
    // desktop and its taskbar. Secondary folder Explorers can otherwise stay
    // alive without a desktop and steal the Win10 tray icon from the real shell.
    RegisterEqWindowClasses(wc.hInstance);

    HWND hParent = nullptr;
    HWND taskbar = nullptr;
    MSG startupMsg{};
    bool startupAborted = false;

    // Explorer can load the injected module before the desktop shell hierarchy
    // exists. Wait for both the real desktop and this process's taskbar. Pumping
    // the queue here prevents the startup wait from blocking sent messages.
    for (;;) {
        HANDLE stopEvent = g_hOverlayStopEvent;
        DWORD waitResult = MsgWaitForMultipleObjects(
            stopEvent ? 1 : 0, stopEvent ? &stopEvent : nullptr,
            FALSE, 250, QS_ALLINPUT);

        if (stopEvent && waitResult == WAIT_OBJECT_0) {
            startupAborted = true;
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + (stopEvent ? 1 : 0)) {
            while (PeekMessageW(&startupMsg, nullptr, 0, 0, PM_REMOVE)) {
                if (startupMsg.message == WM_QUIT) {
                    g_running.store(false, std::memory_order_release);
                    startupAborted = true;
                    break;
                }
                TranslateMessage(&startupMsg);
                DispatchMessageW(&startupMsg);
            }
            if (startupAborted)
                break;
        }

        hParent = ResolveOverlayDesktopParent(GetSettingsSnapshot().desktopPlacement);
        taskbar = FindCurrentProcessTaskbarWnd();
        if (hParent && taskbar)
            break;
    }

    if (startupAborted) {
        EqCleanupIntegration();
        if (classRegistered)
            UnregisterClassW(wc.lpszClassName, wc.hInstance);
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return 0;
    }

    g_eqTaskbarHwnd = taskbar;

    if (!g_eqIsWindows11) {
        if (!EqCreateNativeTrayMessageWindow(wc.hInstance)) {
            Wh_Log(L"OverlayThreadProc: failed to create Win10 EQ tray message window");
        } else {
            EqEnsureNativeTrayIcon();
        }
    } else {
        EqEnsureXamlButton();
    }

    const VisualizerSettings initialSettings = GetSettingsSnapshot();
    if (!CreateOverlayWindowsForSettings(
            initialSettings, wc.hInstance, wc.lpszClassName, hParent)) {
        Wh_Log(L"OverlayThreadProc: initial overlay creation failed");
        EqCleanupIntegration();
        ReleaseOverlayGraphics();
        UnregisterOverlayHostClass(wc.hInstance);
        if (classRegistered)
            UnregisterClassW(wc.lpszClassName, wc.hInstance);
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return 0;
    }

    // All shell-dependent workers start only after the desktop/taskbar
    // ownership check. Secondary Explorer processes can load the mod too, but
    // they must not create audio/album/lyrics/SMTC workers of their own.
    StartAudioCapture();

    const VisualizerSettings startupSettings = GetSettingsSnapshot();
    if (IsAlbumColorMode())
        StartAlbumColorCapture();
    if (startupSettings.lyricsEnabled ||
        EqIsLayoutWidgetPresent(EQ_LAYOUT_LYRICS)) {
        StartLyricsCapture();
    }
    if (startupSettings.showMediaEqTrayButton)
        StartEqMediaCapture();
    g_shellServicesStarted.store(true, std::memory_order_release);

    {
        g_currentOverlayTimerMs = GetOverlayTimerIntervalMs(initialSettings);
        SetTimer(g_hwndOverlay, 1, g_currentOverlayTimerMs, nullptr);
    }

    MSG msg{};
    HANDLE stopEvent = g_hOverlayStopEvent;
    ULONGLONG lastShellCheckMs = GetTickCount64() - 1000;
    g_lastFullscreenCheckMs = GetTickCount64() - 1000;

    for (;;) {
        DWORD waitResult = MsgWaitForMultipleObjects(
            stopEvent ? 1 : 0, stopEvent ? &stopEvent : nullptr,
            FALSE, 250, QS_ALLINPUT);

        if (stopEvent && waitResult == WAIT_OBJECT_0)
            break;

        if (g_running.load(std::memory_order_acquire)) {
            // Check fullscreen state independently of the slower shell repair
            // pass so entering/leaving a game changes the render rate quickly.
            UpdateFullscreenThrottle();

            const ULONGLONG nowMs = GetTickCount64();
            if (nowMs - lastShellCheckMs >= 1000) {
                lastShellCheckMs = nowMs;

                // The EQ integration must stay alive while a fullscreen app is
                // active, but desktop overlay repair/rebuild work is unnecessary
                // there and can interfere with games/video playback.
                const bool fullscreen =
                    g_fullscreenThrottleActive.load(std::memory_order_acquire);
                if (g_eqIsWindows11)
                    EqEnsureXamlButton();
                else
                    EqEnsureNativeTrayIcon();

                if (!fullscreen &&
                    !g_layoutEditActive.load(std::memory_order_acquire)) {
                    const VisualizerSettings settings = GetSettingsSnapshot();

                    // Re-resolve each pass: Explorer can rebuild the desktop
                    // hierarchy (wallpaper change, theme change, shell
                    // restart) and the overlay must follow it.
                    HWND currentDesktopParent =
                        ResolveOverlayDesktopParent(settings.desktopPlacement);
                    if (currentDesktopParent)
                        hParent = currentDesktopParent;

                    const bool rebuilt = RebuildOverlayWindows(
                        settings, wc.hInstance, wc.lpszClassName, hParent);
                    // Explorer may restack the desktop layers (wallpaper or
                    // theme change); keep the overlay below the icons.
                    if (!rebuilt)
                        ReassertOverlayZOrder();
                    if (g_hwndOverlay) {
                        const UINT interval = GetOverlayTimerIntervalMs(settings);
                        // A rebuilt overlay is a brand new window with no
                        // timer, so always re-arm it in that case even when
                        // the interval is unchanged. Otherwise rendering
                        // silently stops after a rebuild.
                        if (rebuilt || interval != g_currentOverlayTimerMs) {
                            g_currentOverlayTimerMs = interval;
                            SetTimer(g_hwndOverlay, 1, interval, nullptr);
                        }
                        if (rebuilt) {
                            for (HWND overlay : g_overlayWindows)
                                RenderOverlay(overlay);
                        }
                    }

                    // The opaque raised-desktop layer shows a copy of the
                    // wallpaper; refresh it when the wallpaper changes (and
                    // every few minutes, for sources the signature misses).
                    if (!rebuilt &&
                        g_overlayBuiltRenderMode == OVERLAY_RENDER_OPAQUE_HOLDER &&
                        !g_overlayWindows.empty() &&
                        (nowMs - g_overlayWallpaperCapturedMs >= 5 * 60 * 1000 ||
                         CurrentWallpaperSignature() != g_overlayWallpaperSignature)) {
                        RecaptureOverlayWallpapers();
                        std::shared_lock<std::shared_mutex> settingsLock(g_settingsMutex);
                        for (HWND overlay : g_overlayWindows)
                            RenderOverlay(overlay);
                    }
                }
            }
        }

        if (waitResult == WAIT_OBJECT_0 + (stopEvent ? 1 : 0)) {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    g_running.store(false, std::memory_order_release);
                    goto overlay_exit;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

overlay_exit:
    g_shellServicesStarted.store(false, std::memory_order_release);
    if (g_layoutEditActive.load(std::memory_order_acquire))
        LayoutEditEnd(false);
    LayoutEditUnregisterClass(wc.hInstance);
    EqCleanupIntegration();
    DestroyAllOverlayWindows();
    ReleaseOverlayGraphics();
    UnregisterOverlayHostClass(wc.hInstance);

    DestroyRenderTarget();

    if (g_pForegroundImage) {
        delete g_pForegroundImage;
        g_pForegroundImage = nullptr;
    }

    DestroyAlbumArtworkImage();
    ClearAlbumArtworkCache();
    DestroyAlbumWidgetCache();

    DestroyBackgroundBlurBitmap();
    g_backgroundBlurNeedsReload.store(true, std::memory_order_release);

    g_lyricsFontCache.Clear();
    g_lyricsMeasureCache.Clear();
    g_lyricsWrapMeasureCache.Clear();

    if (classRegistered)
        UnregisterClassW(wc.lpszClassName, wc.hInstance);

    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    g_gdiplusToken = 0;
    return 0;
}

static void EqUnregisterWindowClasses() {
    if (!g_eqClassesRegistered)
        return;

    if (g_eqPopupHwnd && IsWindow(g_eqPopupHwnd))
        EqClosePopup();

    UnregisterClassW(kEqNativeTrayWindowClass, g_eqModuleHandle);
    UnregisterClassW(kEqPopupClass, g_eqModuleHandle);
    g_eqClassesRegistered = false;
    g_eqModuleHandle = nullptr;
}

void Wh_ModAfterInit() {
    if (g_eqIsWindows11 && g_eqTaskbarSymbolsHooked)
        EqEnsureXamlButton();
}

BOOL Wh_ModInit() {
    g_eqStorageInitialized = false;
    g_eqSelectedPreset = -1;
    g_eqHotPreset = -1;
    g_eqHotPresetDelete = -1;
    g_eqHotMediaButton = -1;
    g_eqHotPageButton = false;
    g_eqHotBand = -1;
    g_eqDraggingBand = -1;
    g_eqPresetAnimationActive = false;
    g_eqLastAnimationTick = 0;
    g_eqMediaSeeking = false;
    g_eqMediaSeekPreviewSeconds = 0.0;
    g_eqLyricsScrollOffset = 0;
    g_eqHotLyricsLine = -1;
    g_eqLyricsScrollTrackKey.clear();
    g_eqLyricsScrollbarDragging = false;
    g_eqLyricsScrollbarDragStartY = 0;
    g_eqLyricsScrollbarStartOffset = 0;

    LoadSettings();

    // One OS-version probe for the whole mod lifetime. Windows 11 is build
    // 22000+, so it selects the existing XAML implementation; older supported
    // builds use only the native Windows 10 notification-area implementation.
    g_eqIsWindows11 = EqDetectWindows11Once();
    g_eqTaskbarSymbolsHooked = false;
    if (g_eqIsWindows11) {
        g_eqTaskbarSymbolsHooked = EqHookTaskbarSymbols();
        if (!g_eqTaskbarSymbolsHooked) {
            Wh_Log(L"Wh_ModInit: Windows 11 taskbar XAML EQ integration hooks could not be installed; visualizer will continue without EQ button");
        }
    } else {
        Wh_Log(L"Wh_ModInit: using Windows 10 native notification-area EQ button (build %u)",
               g_eqWindowsBuildNumber);
    }
    RegisterEqWindowClasses(GetCurrentModModuleHandle());
    g_shellServicesStarted.store(false, std::memory_order_release);
    g_running.store(true, std::memory_order_release);

    BuildHannWindow();
    BuildTwiddleFactors();
    ClearAudioBands();

    g_hOverlayStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hOverlayStopEvent) {
        g_running.store(false, std::memory_order_release);
        EqUnregisterWindowClasses();
        return FALSE;
    }

    g_hOverlayThread = CreateThread(
        nullptr, 0, OverlayThreadProc, nullptr, 0, &g_overlayThreadId);

    if (!g_hOverlayThread) {
        g_running.store(false, std::memory_order_release);
        CloseHandle(g_hOverlayStopEvent);
        g_hOverlayStopEvent = nullptr;
        EqUnregisterWindowClasses();
        return FALSE;
    }

    // Wh_ModInit can run in secondary Explorer processes. Shell-dependent
    // workers are started by OverlayThreadProc after it verifies desktop/taskbar
    // ownership; keep these checks here as an additional lifecycle guard.
    if (g_shellServicesStarted.load(std::memory_order_acquire)) {
        if (IsAlbumColorMode())
            StartAlbumColorCapture();
        if (g_settings.lyricsEnabled ||
            EqIsLayoutWidgetPresent(EQ_LAYOUT_LYRICS)) {
            StartLyricsCapture();
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_shellServicesStarted.load(std::memory_order_acquire))
        SaveCustomEQSettings();

    g_running.store(false, std::memory_order_release);

    // The overlay thread owns the Win10 tray message window and the overlay
    // cleanup path. Signal and join it before stopping workers or unregistering
    // classes, so Win10 cleanup never SendMessage's into a thread stuck in startup.
    if (g_hOverlayStopEvent)
        SetEvent(g_hOverlayStopEvent);

    if (g_hOverlayThread) {
        WaitForSingleObject(g_hOverlayThread, INFINITE);
        CloseHandle(g_hOverlayThread);
        g_hOverlayThread = nullptr;
    }

    g_overlayThreadId = 0;

    StopLyricsCapture();
    StopAlbumColorCapture();
    StopEqMediaCapture();
    StopAudioCapture();

    if (g_hOverlayStopEvent) {
        CloseHandle(g_hOverlayStopEvent);
        g_hOverlayStopEvent = nullptr;
    }

    EqUnregisterWindowClasses();
}

void Wh_ModSettingsChanged() {
    int oldTargetMonitor = 0;
    int oldTargetFps = 0;
    bool oldShowMediaEqTrayButton = true;
    int oldAudioSource = 0;
    std::wstring oldAudioApplicationName;
    bool oldLyricsEnabled = false;
    bool oldLyricsUseLocalLrcFiles = true;
    std::wstring oldLyricsLocalLrcFolder;
    bool oldAlbumColorMode = false;

    {
        std::unique_lock<std::shared_mutex> settingsLock(g_settingsMutex);
        oldTargetMonitor = g_settings.targetMonitor;
        oldTargetFps = g_settings.targetFps;
        oldShowMediaEqTrayButton = g_settings.showMediaEqTrayButton;
        oldAudioSource = g_settings.audioSource;
        oldAudioApplicationName = g_settings.audioApplicationName;
        oldLyricsEnabled = g_settings.lyricsEnabled;
        oldLyricsUseLocalLrcFiles = g_settings.lyricsUseLocalLrcFiles;
        oldLyricsLocalLrcFolder = g_settings.lyricsLocalLrcFolder;
        oldAlbumColorMode = IsAlbumColorMode();
        LoadSettings();
    }

    int newAudioSource = 0;
    std::wstring newAudioApplicationName;
    bool newShowMediaEqTrayButton = true;
    bool newLyricsEnabled = false;
    bool newLyricsUseLocalLrcFiles = true;
    std::wstring newLyricsLocalLrcFolder;
    bool newInteractiveLyrics = false;
    bool newAlbumColorMode = false;
    {
        std::shared_lock<std::shared_mutex> settingsLock(g_settingsMutex);
        newAudioSource = g_settings.audioSource;
        newAudioApplicationName = g_settings.audioApplicationName;
        newShowMediaEqTrayButton = g_settings.showMediaEqTrayButton;
        newLyricsEnabled = g_settings.lyricsEnabled;
        newLyricsUseLocalLrcFiles = g_settings.lyricsUseLocalLrcFiles;
        newLyricsLocalLrcFolder = g_settings.lyricsLocalLrcFolder;
        newInteractiveLyrics = EqIsLayoutWidgetPresent(EQ_LAYOUT_LYRICS);
        newAlbumColorMode = IsAlbumColorMode();
    }


    if (oldShowMediaEqTrayButton != newShowMediaEqTrayButton) {
        if (g_eqIsWindows11)
            EqEnsureXamlButton();
        else
            EqEnsureNativeTrayIcon();

        if (g_shellServicesStarted.load(std::memory_order_acquire)) {
            if (newShowMediaEqTrayButton)
                StartEqMediaCapture();
            else
                StopEqMediaCapture();
        }
    }

    const bool audioSourceChanged =
        oldAudioSource != newAudioSource ||
        oldAudioApplicationName != newAudioApplicationName;

    if (oldAlbumColorMode != newAlbumColorMode ||
        audioSourceChanged) {
        if (g_shellServicesStarted.load(std::memory_order_acquire)) {
            if (newAlbumColorMode)
                StartAlbumColorCapture();
            else
                StopAlbumColorCapture();
        } else if (!newAlbumColorMode) {
            StopAlbumColorCapture();
        }
    }

    const bool lyricsSourceChanged =
        oldLyricsUseLocalLrcFiles != newLyricsUseLocalLrcFiles ||
        oldLyricsLocalLrcFolder != newLyricsLocalLrcFolder;

    if (lyricsSourceChanged)
        StopLyricsCapture();

    if (oldLyricsEnabled != newLyricsEnabled ||
        audioSourceChanged ||
        lyricsSourceChanged) {
        if (g_shellServicesStarted.load(std::memory_order_acquire)) {
            if (newLyricsEnabled || newInteractiveLyrics)
                StartLyricsCapture();
            else
                StopLyricsCapture();
        } else if (!newLyricsEnabled && !newInteractiveLyrics) {
            StopLyricsCapture();
        }
    }

    if (audioSourceChanged &&
        g_shellServicesStarted.load(std::memory_order_acquire)) {
        StopAudioCapture();
        StartAudioCapture();
    }

    if (g_hwndOverlay) {
        PostMessageW(g_hwndOverlay, WM_VIZ_REBUILD_OVERLAYS, 0, 0);
        PostMessageW(g_hwndOverlay, WM_VIZ_AUDIO_WAKE, 0, 0);
    }
}
