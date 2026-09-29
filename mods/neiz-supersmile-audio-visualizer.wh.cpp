// ==WindhawkMod==
// @id              neiz-supersmile-audio-visualizer
// @name            Desktop Audio Visualizer Plus
// @description     A highly customizable audio visualizer with synced lyrics, featuring optional network access to fetch lyrics from lrclib.net
// @version         1.0.1
// @license         MIT
// @author          NeiZ
// @github          https://github.com/NeiZqwe
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lmmdevapi -lksuser -lgdi32 -luser32 -lgdiplus -ldwmapi -lruntimeobject -lwindowsapp -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Audio Visualizer Plus

Desktop Audio Visualizer Plus is a Windhawk desktop overlay that renders a highly customizable real-time audio visualizer directly on the Windows desktop.

## Features

### Audio

- WASAPI loopback capture for system audio
- Optional per-application audio source
- 1024-point FFT
- 32 logarithmic frequency bands
- 10-band custom EQ curve applied across the FFT bands ( Win 11 )
- Up to 256 rendered visual bars
- Auto-Gain normalization
- Attack / decay controls
- Optional CAVA-style smoothing

### Visualization

- Stereo, Mountain, Mirror, Wave, Circular, Dots and Area visualization types
- Square, Rounded, Segmented, Pointed, Continuous Curve and Battery bar styles
- Linear, Step, Cosine and Catmull-Rom interpolation
- Vertical and horizontal orientations
- Mirrored visualization pass
- Configurable bar count, width, spacing, height and corner radius
- Circular radius and start-angle controls

### Appearance

- Solid colors and gradients
- Album-derived colors and album gradients
- Dynamic Acrylic, Liquid Glass and Aero-style peak-cap rendering
- Configurable opacity and dynamic opacity curves
- Dynamic color curves
- Bar borders with visualizer, album and custom hex colors
- Optional visualizer backgrounds: solid, gradient, album color and album gradient

### Lyrics

Lyrics are optional and disabled by default. When enabled, the mod can retrieve synchronized lyrics for the currently playing track from LRCLIB and display them in a configurable desktop widget.

The widget supports artist/title metadata, synchronized highlighting, previous/upcoming lines, multiple fonts, alignment, long-line wrapping, fallback/collapse/hide behavior, album-derived backgrounds, gradients, rounded corners and configurable borders.

## Lyrics data source

When the Lyrics feature is enabled, the mod contacts LRCLIB only to retrieve lyrics for the currently playing track. The lyrics feature is optional and disabled by default; the network access is not used for telemetry, analytics, remote configuration, or any part of the audio visualization engine.

## Custom EQ ( Win 11 )

The Equalizer button is injected as a native Windows 11 XAML element directly into the system tray. Click it to open the graphical 10-band EQ flyout. Each band ranges from 0.0x to 2.0x gain, with 1.0x as the neutral/default value. EQ values are stored in the mod's local Windhawk storage so they survive reloads without changing the existing visualizer settings pipeline.

## Performance

The visualizer uses a lightweight WASAPI/FFT pipeline and dynamically reduces rendering frequency when the visualization has reached its resting state.

The visualizer frame rate can be configured independently. A value of `0` removes the software FPS cap and follows the current display refresh rate; any positive value requests that FPS, but it is still capped by the display refresh rate. Higher refresh rates can make the animation smoother, especially with fast visualizer styles, but may increase CPU/GPU usage and power consumption.

Actual CPU usage depends on display resolution, enabled effects, visualization style, lyrics, frame rate and other settings.


---

## Preview

![eq](https://i.imgur.com/IOwofqV.png)

![1](https://i.imgur.com/TU0iXkc.gif)

![2](https://i.imgur.com/8NllN1h.gif)

![3](https://i.imgur.com/VNz6GOi.gif)

![4](https://i.imgur.com/aqXU5Fa.gif)

---

# Credits

- **NeiZ** — Author and maintainer.
- **SuperSmile123** — Contributor.
- **Salyts** — Reference / upstream desktop audio visualizer implementation.
- **GR0UD** — Upstream audio capture / FFT implementation reference.

---

## Report a Bug

Please report bugs and feature requests through the repository issue tracker: [Report an Issue on GitHub](https://github.com/NeiZqwe/windhawk-mods/issues)


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Visualizer:
    - barCount: 96
      $name: Bar count
      $name:ru-RU: Количество полос
      $description: Number of bars displayed by the visualizer (0-256)
      $description:ru-RU: Количество полос, отображаемых визуализатором (0-256)

    - barWidth: 6
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
    - positionX: 50
      $name: X position (px)
      $description: Adjusts the horizontal position of the visualizer
      $name:ru-RU: X позиция
      $description:ru-RU: Изменяет горизонтальное положение визуализатора
    - positionY: 900
      $name: Y position (px)
      $description: Adjusts the vertical position of the visualizer
      $name:ru-RU: Y позиция
      $description:ru-RU: Изменяет вертикальное положение визуализатора
    - maxBarHeight: 200
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
          $description: Select whether the visualizer listens to the whole system or one application
          $description:ru-RU: Выберите, слушать ли всю систему или только одно приложение
          $options:
            - system: Whole system
            - application: Selected application
          $options:ru-RU:
            - system: Вся система
            - application: Выбранное приложение

        - audioApplicationName: ""
          $name: Application executable
          $name:ru-RU: Исполняемый файл приложения
          $description: Executable name, for example Spotify.exe
          $description:ru-RU: Имя .exe, например Spotify.exe
      $name: Audio source
      $name:ru-RU: Источник аудио

    - sensitivity: 150
      $name: Sensitivity
      $name:ru-RU: Чувствительность
      $description: Controls how strongly the visualizer reacts to audio (0-300)
      $description:ru-RU: Определяет, насколько сильно визуализатор реагирует на звук (0-300)

    - AutoGain:
        - autoGainEnabled: true
          $name: Auto-Gain
          $name:ru-RU: Автонормализация
          $description: Automatically adjusts the audio level for a more consistent response
          $description:ru-RU: Автоматически регулирует уровень сигнала для более стабильной реакции

        - autoGainStrength: 50
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
          $description: Controls how quickly the bars rise in response to louder audio (0-100)
          $description:ru-RU: Определяет, насколько быстро полосы поднимаются в ответ на усиление звука (0-100)

        - decaySpeed: 16
          $name: Decay speed
          $name:ru-RU: Скорость затухания
          $description: Controls how quickly the bars return toward their resting height (0-100)
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

        - segmentHeight: 0
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
        - colorMode: dynamic_acrylic
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
        - backgroundEnabled: false
          $name: Visualizer background
          $name:ru-RU: Фон визуализатора
          $description: Enables or disables the visualizer background
          $description:ru-RU: Включает или выключает фон визуализатора

        - backgroundMode: solid
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

    - unavailableText: "N/A"
      $name: Fallback
      $name:ru-RU: Текст-заглушка
      $description: "Text shown when the current track has no lyrics ( Default: N/A )"
      $description:ru-RU: "Текст, отображаемый если для текущего трека нет текста песни ( По умолчанию: N/A )"

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

  $name: Lyrics ( Beta )
  $name:ru-RU: Текст песни ( Бета )
  $description: Displays lyrics for the currently playing track
  $description:ru-RU: Показывает текст текущего трека

- Advanced:
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
    WH_PROCESS_LOOPBACK_MODE_EXCLUDE_TARGET_PROCESS_TREE = 1,
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
#include <array>
#include <memory>
// #include <new>
#include <mutex>
// #include <thread>
#include <shared_mutex>
#include <objidl.h>
#include <cwctype>

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
    int targetFps;
    int sensitivity;
    int audioSource; // 0 = system, 1 = application
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
} g_settings{};

struct AlbumPaletteGdi {
    DWORD primary = RGB(18, 18, 18);
    DWORD secondary = RGB(45, 45, 45);
};

static AlbumPaletteGdi g_albumPalette{};
static std::mutex g_albumPaletteMutex;
static size_t g_albumPaletteHash = 0;

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
static HANDLE g_hLyricsThread = nullptr;
static HANDLE g_hLyricsStopEvent = nullptr;
static std::atomic<bool> g_lyricsRunning{false};

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
static constexpr std::array<const wchar_t*, VIZ_EQ_BANDS> VIZ_EQ_STORAGE_KEYS = {
    L"customEqBand0", L"customEqBand1", L"customEqBand2", L"customEqBand3",
    L"customEqBand4", L"customEqBand5", L"customEqBand6", L"customEqBand7",
    L"customEqBand8", L"customEqBand9"
};
static std::array<std::atomic<float>, VIZ_EQ_BANDS> g_customEqGains{};

static std::atomic<float> g_audioBands[VIZ_NUM_BANDS] = {};
static std::atomic<ULONGLONG> g_lastAudioUpdateMs{0};
static float g_currentHeights[VIZ_BANDS_MAX] = {};

static float g_hannWindow[VIZ_FFT_SIZE] = {};
static float g_twiddleRe[VIZ_FFT_SIZE / 2] = {};
static float g_twiddleIm[VIZ_FFT_SIZE / 2] = {};
static int g_logBinStart[VIZ_NUM_BANDS + 1] = {};

static HWND g_hwndOverlay = nullptr;
static HANDLE g_hOverlayThread = nullptr;
static HANDLE g_hOverlayStopEvent = nullptr;
static DWORD g_overlayThreadId = 0;
static std::atomic<bool> g_overlayIdle{false};
static std::atomic<HWND> g_overlayWakeHwnd{nullptr};
static constexpr UINT WM_VIZ_AUDIO_WAKE = WM_APP + 0x2A1;
static ULONG_PTR g_gdiplusToken = 0;

static HDC g_renderMemDC = nullptr;
static HBITMAP g_renderBitmap = nullptr;
static HGDIOBJ g_renderOldBitmap = nullptr;
static void* g_renderBits = nullptr;
static int g_renderWidth = 0;
static int g_renderHeight = 0;
static RECT g_lastRenderDirtyRect{};
static bool g_renderNeedsFullClear = true;

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

static Gdiplus::Bitmap* g_pBackgroundBlurBitmap = nullptr;
static RECT g_backgroundBlurRect{};
static int g_backgroundBlurWidth = 0;
static int g_backgroundBlurHeight = 0;
static int g_backgroundBlurRadius = 0;
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
    if (g_pBackgroundBlurBitmap) {
        delete g_pBackgroundBlurBitmap;
        g_pBackgroundBlurBitmap = nullptr;
    }

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

    // Horizontal pass.
    for (UINT y = 0; y < height; ++y) {
        for (UINT x = 0; x < width; ++x) {
            uint32_t sum[4] = {};
            int count = 0;
            const int x0 = std::max<int>(0, static_cast<int>(x) - r);
            const int x1 = std::min<int>(static_cast<int>(width) - 1, static_cast<int>(x) + r);

            for (int sx = x0; sx <= x1; ++sx) {
                const BYTE* p = src.data() + (static_cast<size_t>(y) * width + sx) * 4;
                for (int c = 0; c < 4; ++c)
                    sum[c] += p[c];
                ++count;
            }

            BYTE* out = tmp.data() + (static_cast<size_t>(y) * width + x) * 4;
            for (int c = 0; c < 4; ++c)
                out[c] = static_cast<BYTE>(sum[c] / count);
        }
    }

    // Vertical pass.
    for (UINT y = 0; y < height; ++y) {
        for (UINT x = 0; x < width; ++x) {
            uint32_t sum[4] = {};
            int count = 0;
            const int y0 = std::max<int>(0, static_cast<int>(y) - r);
            const int y1 = std::min<int>(static_cast<int>(height) - 1, static_cast<int>(y) + r);

            for (int sy = y0; sy <= y1; ++sy) {
                const BYTE* p = tmp.data() + (static_cast<size_t>(sy) * width + x) * 4;
                for (int c = 0; c < 4; ++c)
                    sum[c] += p[c];
                ++count;
            }

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

    const float localX =
        static_cast<float>(rect.left + virtualX);
    const float localY =
        static_cast<float>(rect.top + virtualY);

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

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    if (!g_backgroundBlurNeedsReload.load(std::memory_order_acquire) &&
        g_pBackgroundBlurBitmap &&
        g_backgroundBlurRect.left == rect.left &&
        g_backgroundBlurRect.top == rect.top &&
        g_backgroundBlurRect.right == rect.right &&
        g_backgroundBlurRect.bottom == rect.bottom &&
        g_backgroundBlurWidth == width &&
        g_backgroundBlurHeight == height &&
        g_backgroundBlurRadius == g_settings.backgroundBlurRadius) {
        return;
    }

    DestroyBackgroundBlurBitmap();
    g_pBackgroundBlurBitmap = CreateBackgroundBlurBitmap(
        rect, g_settings.backgroundBlurRadius);
    if (g_pBackgroundBlurBitmap) {
        g_backgroundBlurRect = rect;
        g_backgroundBlurWidth = width;
        g_backgroundBlurHeight = height;
        g_backgroundBlurRadius = g_settings.backgroundBlurRadius;
        g_backgroundBlurNeedsReload.store(false, std::memory_order_release);
    }
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

static void LoadSettings() {
    g_settings.barCount = std::clamp(Wh_GetIntSetting(L"Visualizer.barCount"), 1, VIZ_BANDS_MAX);
    g_settings.barWidth = std::clamp(Wh_GetIntSetting(L"Visualizer.barWidth"), 1, 50);
    g_settings.barSpacing = std::clamp(Wh_GetIntSetting(L"Visualizer.barSpacing"), 0, 50);
    g_settings.positionX = Wh_GetIntSetting(L"Visualizer.positionX");
    g_settings.positionY = Wh_GetIntSetting(L"Visualizer.positionY");
    g_settings.maxBarHeight = std::clamp(Wh_GetIntSetting(L"Visualizer.maxBarHeight"), 1, 2000);
    g_settings.minBarHeight = std::clamp(Wh_GetIntSetting(L"Visualizer.minBarHeight"), 0, 1000);
    g_settings.targetFps = std::max(0, Wh_GetIntSetting(L"Performance.targetFps"));
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
        g_settings.lyricsUnavailableText = L"N/A";
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

static void LoadCustomEQSettings() {
    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const int stored = std::clamp(
            Wh_GetIntValue(VIZ_EQ_STORAGE_KEYS[static_cast<size_t>(i)], 100), 0, 200);
        g_customEqGains[static_cast<size_t>(i)].store(
            stored / 100.0f, std::memory_order_relaxed);
    }
}

static void SaveCustomEQSettings() {
    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const float gain = std::clamp(
            g_customEqGains[static_cast<size_t>(i)].load(std::memory_order_relaxed),
            0.0f, 2.0f);
        Wh_SetIntValue(
            VIZ_EQ_STORAGE_KEYS[static_cast<size_t>(i)],
            static_cast<int>(std::lround(gain * 100.0f)));
    }
}

static float GetVizEQMultiplier(int band) {
    band = std::clamp(band, 0, VIZ_NUM_BANDS - 1);

    // The existing FFT uses logarithmic 20 Hz..20 kHz bands, so interpolate
    // the user controls in log-frequency space. This keeps the EQ smooth while
    // leaving the established FFT/visualizer pipeline completely unchanged.
    constexpr float kMinHz = 20.0f;
    constexpr float kMaxHz = 20000.0f;
    const float bandT = (static_cast<float>(band) + 0.5f) /
        static_cast<float>(VIZ_NUM_BANDS);
    const float frequency = kMinHz *
        powf(kMaxHz / kMinHz, bandT);

    float previousLog = logf(std::max(kMinHz, VIZ_EQ_LOW_HZ[0]));
    float previousGain = g_customEqGains[0].load(std::memory_order_relaxed);

    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const float lowLog = logf(VIZ_EQ_LOW_HZ[static_cast<size_t>(i)]);
        const float highLog = logf(VIZ_EQ_HIGH_HZ[static_cast<size_t>(i)]);
        const float currentGain = g_customEqGains[static_cast<size_t>(i)].load(
            std::memory_order_relaxed);

        if (frequency <= VIZ_EQ_LOW_HZ[static_cast<size_t>(i)]) {
            if (i == 0)
                return std::clamp(currentGain, 0.0f, 2.0f);
            const float t = std::clamp(
                (logf(frequency) - previousLog) /
                    std::max(0.0001f, lowLog - previousLog),
                0.0f, 1.0f);
            return std::clamp(previousGain +
                (g_customEqGains[static_cast<size_t>(i)].load(std::memory_order_relaxed) -
                 previousGain) * t, 0.0f, 2.0f);
        }

        if (frequency <= VIZ_EQ_HIGH_HZ[static_cast<size_t>(i)]) {
            // Each slider controls its own frequency range. A small linear
            // interpolation inside that range avoids hard gain discontinuities
            // at the FFT-band boundaries.
            const float lowT = std::clamp(
                (logf(frequency) - lowLog) /
                    std::max(0.0001f, highLog - lowLog),
                0.0f, 1.0f);
            const float nextGain = (i + 1 < VIZ_EQ_BANDS)
                ? g_customEqGains[static_cast<size_t>(i + 1)].load(std::memory_order_relaxed)
                : currentGain;
            // Blend towards the next control only through the second half of
            // the range, preserving the intuitive meaning of the slider.
            const float blend = lowT < 0.5f ? 0.0f : (lowT - 0.5f) * 2.0f;
            return std::clamp(currentGain + (nextGain - currentGain) * blend,
                              0.0f, 2.0f);
        }

        previousLog = highLog;
        previousGain = currentGain;
    }

    return std::clamp(
        g_customEqGains[VIZ_EQ_BANDS - 1].load(std::memory_order_relaxed),
        0.0f, 2.0f);
}

static float GetBandGravity(int band) {
    const float t = (band + 0.5f) / static_cast<float>(VIZ_NUM_BANDS);
    return 0.012f + t * 0.030f;
}

static float GetBandSensitivity(int band) {
    const float t = (band + 0.5f) / static_cast<float>(VIZ_NUM_BANDS);
    return 0.28f * powf(0.045f / 0.28f, t);
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

static DWORD FindAudioProcessIdByExecutable(
    IMMDeviceEnumerator* enumerator,
    const std::wstring& executableName) {

    if (executableName.empty())
        return 0;

    std::wstring wanted = executableName;
    for (wchar_t& ch : wanted)
        ch = static_cast<wchar_t>(towlower(ch));

    DWORD activeSessionPid = 0;
    DWORD anySessionPid = 0;

    if (enumerator) {
        IMMDevice* device = nullptr;
        HRESULT hr = enumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, &device);

        if (SUCCEEDED(hr) && device) {
            IAudioSessionManager2* sessionManager = nullptr;
            hr = device->Activate(
                __uuidof(IAudioSessionManager2),
                CLSCTX_ALL,
                nullptr,
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
                                    reinterpret_cast<void**>(&control2))) &&
                                control2) {
                                DWORD pid = 0;
                                if (SUCCEEDED(control2->GetProcessId(&pid)) && pid != 0) {
                                    HANDLE process = OpenProcess(
                                        PROCESS_QUERY_LIMITED_INFORMATION,
                                        FALSE,
                                        pid);

                                    bool matches = false;
                                    bool isActive = false;

                                    if (process) {
                                        wchar_t exePath[MAX_PATH] = {};
                                        DWORD pathLen = ARRAYSIZE(exePath);
                                        if (QueryFullProcessImageNameW(
                                                process, 0, exePath, &pathLen)) {
                                            const wchar_t* base = wcsrchr(exePath, L'\\');
                                            base = base ? base + 1 : exePath;
                                            matches = EqualsExecutableName(base, wanted);
                                        }
                                        CloseHandle(process);
                                    }

                                    AudioSessionState state = AudioSessionStateInactive;
                                    if (matches &&
                                        SUCCEEDED(control->GetState(&state))) {
                                        isActive = (state == AudioSessionStateActive);
                                    }

                                    if (matches) {
                                        if (anySessionPid == 0)
                                            anySessionPid = pid;
                                        if (isActive && activeSessionPid == 0)
                                            activeSessionPid = pid;
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

    if (activeSessionPid != 0) {
        Wh_Log(L"Selected audio app '%s' -> active audio PID=%lu",
               executableName.c_str(),
               static_cast<unsigned long>(activeSessionPid));
        return activeSessionPid;
    }

    if (anySessionPid != 0) {
        Wh_Log(L"Selected audio app '%s' -> audio-session PID=%lu",
               executableName.c_str(),
               static_cast<unsigned long>(anySessionPid));
        return anySessionPid;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    DWORD result = 0;

    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (EqualsExecutableName(entry.szExeFile, wanted)) {
                result = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);

    if (result != 0) {
        Wh_Log(L"Selected audio app '%s' -> process snapshot PID=%lu",
               executableName.c_str(),
               static_cast<unsigned long>(result));
    }

    return result;
}

static bool InitSystemAudioClient(
    IMMDeviceEnumerator* enumerator,
    IAudioClient** outClient,
    IAudioCaptureClient** outCapture,
    UINT32* outSampleRate,
    UINT32* outChannels,
    bool* outIsFloat) {

    if (!enumerator || !outClient || !outCapture ||
        !outSampleRate || !outChannels || !outIsFloat) {
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
    *outIsFloat =
        format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
        (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
         reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format)->SubFormat ==
             KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);

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
    bool* outIsFloat) {

    if (!processId || !outClient || !outCapture ||
        !outSampleRate || !outChannels || !outIsFloat) {
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
        handler->Release();
        CloseHandle(completedEvent);
        return false;
    }

    for (;;) {
        const DWORD waitResult = WaitForSingleObject(completedEvent, 50);
        if (waitResult == WAIT_OBJECT_0)
            break;
        if (g_audioRunning.load(std::memory_order_acquire) == false) {
            handler->DisableEvent();
            handler->Release();
            CloseHandle(completedEvent);
            return false;
        }
    }

    const HRESULT activationResult = handler->Result();
    if (FAILED(activationResult)) {
        Wh_Log(L"Process loopback activation result failed: 0x%08X (pid=%lu)",
               static_cast<unsigned>(activationResult),
               static_cast<unsigned long>(processId));
        handler->Release();
        CloseHandle(completedEvent);
        return false;
    }

    IAudioClient* client = handler->TakeAudioClient();
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
    *outIsFloat = false;
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
    bool* outIsFloat) {

    if (settings.audioSource == 1) {
        DWORD pid = FindAudioProcessIdByExecutable(enumerator, settings.audioApplicationName);
        if (!pid) {
            Wh_Log(L"Selected audio application not found: %s",
                   settings.audioApplicationName.c_str());
            return false;
        }
        return InitProcessAudioClient(
            pid, outClient, outCapture,
            outSampleRate, outChannels, outIsFloat);
    }

    return InitSystemAudioClient(
        enumerator, outClient, outCapture,
        outSampleRate, outChannels, outIsFloat);
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
    bool isFloat = true;
    DWORD targetProcessId = 0;
    ULONGLONG lastTargetCheckMs = 0;
    DWORD targetCheckIntervalMs = 500;
    const VisualizerSettings initialSettings = GetSettingsSnapshot();

    if (initialSettings.audioSource == 1)
        targetProcessId = FindAudioProcessIdByExecutable(enumerator, initialSettings.audioApplicationName);

    if (InitAudioClient(
            enumerator, initialSettings,
            &client,
            &capture,
            &sampleRate,
            &channels,
            &isFloat)) {
        BuildLogBins(sampleRate);
    }

    static constexpr int RING_CAP = VIZ_FFT_SIZE * 4;
    std::vector<float> ringBuf(RING_CAP, 0.0f);
    int ringHead = 0;
    int ringCount = 0;

    std::vector<float> re(VIZ_FFT_SIZE, 0.0f);
    std::vector<float> im(VIZ_FFT_SIZE, 0.0f);

    float bandEnv[VIZ_NUM_BANDS] = {};
    

    while (g_audioRunning.load(std::memory_order_acquire)) {
        const VisualizerSettings settings = GetSettingsSnapshot();
        if (settings.audioSource == 1) {
            const ULONGLONG now = GetTickCount64();
            if (now - lastTargetCheckMs >= targetCheckIntervalMs) {
                lastTargetCheckMs = now;
                const DWORD newTargetProcessId =
                    FindAudioProcessIdByExecutable(enumerator, settings.audioApplicationName);

                if (newTargetProcessId == 0) {
                    // When the selected app is not playing, there is normally no
                    // audio session to query. Back off the process fallback so we
                    // do not create a system-wide snapshot every 500 ms forever.
                    targetCheckIntervalMs = std::min<DWORD>(
                        targetCheckIntervalMs * 2, 5000);
                } else {
                    targetCheckIntervalMs = 500;
                }

                if (newTargetProcessId != targetProcessId) {
                    if (client) {
                        client->Stop();
                        client->Release();
                        client = nullptr;
                    }
                    if (capture) {
                        capture->Release();
                        capture = nullptr;
                    }

                    targetProcessId = newTargetProcessId;
                    ringHead = 0;
                    ringCount = 0;
                    for (float& value : bandEnv)
                        value = 0.0f;
                    ClearAudioBands();
                }
            }
        }

        if (g_hAudioEvent) {
            WaitForSingleObject(g_hAudioEvent, 20);
        } else {
            Sleep(8);
        }

        if (!capture || !client) {
            if (client) {
                client->Stop();
                client->Release();
                client = nullptr;
            }
            if (capture) {
                capture->Release();
                capture = nullptr;
            }

            ringHead = 0;
            ringCount = 0;
            for (float& value : bandEnv)
                value = 0.0f;
            ClearAudioBands();

            if (InitAudioClient(
                    enumerator,
                    settings,
                    &client,
                    &capture,
                    &sampleRate,
                    &channels,
                    &isFloat)) {
                BuildLogBins(sampleRate);
            } else {
                Sleep(settings.audioSource == 1 ? 100 : 20);
                continue;
            }
        }

        UINT32 packetSize = 0;
        hr = capture->GetNextPacketSize(&packetSize);
        if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
            client->Stop();
            client->Release();
            client = nullptr;
            capture->Release();
            capture = nullptr;
            continue;
        }

        if (FAILED(hr)) {
            Sleep(5);
            continue;
        }

        if (packetSize == 0) {
            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                bandEnv[b] = std::max(0.0f, bandEnv[b] - GetBandGravity(b));
                g_audioBands[b].store(bandEnv[b], std::memory_order_relaxed);
            }
            g_lastAudioUpdateMs.store(GetTickCount64(), std::memory_order_release);
            continue;
        }

        while (packetSize > 0 &&
               g_audioRunning.load(std::memory_order_acquire)) {
            BYTE* data = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;

            hr = capture->GetBuffer(
                &data,
                &numFrames,
                &flags,
                nullptr,
                nullptr);

            if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                client->Stop();
                client->Release();
                client = nullptr;
                capture->Release();
                capture = nullptr;
                break;
            }

            if (FAILED(hr))
                break;

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                for (int b = 0; b < VIZ_NUM_BANDS; ++b)
                    bandEnv[b] = std::max(0.0f, bandEnv[b] - GetBandGravity(b));
                PublishAudioBands(bandEnv);
            } else if (data && numFrames > 0) {
                if (isFloat) {
                    const float* src = reinterpret_cast<const float*>(data);
                    for (UINT32 f = 0; f < numFrames; ++f) {
                        float mono = 0.0f;
                        for (UINT32 c = 0; c < channels; ++c)
                            mono += src[f * channels + c];

                        ringBuf[ringHead] = mono / static_cast<float>(channels);
                        ringHead = (ringHead + 1) % RING_CAP;
                        ringCount = std::min(ringCount + 1, RING_CAP);
                    }
                } else if (channels > 0) {
                    const INT16* src = reinterpret_cast<const INT16*>(data);
                    for (UINT32 f = 0; f < numFrames; ++f) {
                        float mono = 0.0f;
                        for (UINT32 c = 0; c < channels; ++c)
                            mono += src[f * channels + c] / 32768.0f;

                        ringBuf[ringHead] = mono / static_cast<float>(channels);
                        ringHead = (ringHead + 1) % RING_CAP;
                        ringCount = std::min(ringCount + 1, RING_CAP);
                    }
                }
            }

            capture->ReleaseBuffer(numFrames);

            hr = capture->GetNextPacketSize(&packetSize);
            if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                client->Stop();
                client->Release();
                client = nullptr;
                capture->Release();
                capture = nullptr;
                break;
            }
            if (FAILED(hr))
                break;
        }

        while (ringCount >= VIZ_FFT_SIZE &&
               g_audioRunning.load(std::memory_order_acquire)) {
            const int readStart =
                (ringHead - ringCount + RING_CAP) % RING_CAP;

            for (int i = 0; i < VIZ_FFT_SIZE; ++i) {
                re[i] = ringBuf[(readStart + i) % RING_CAP] * g_hannWindow[i];
                im[i] = 0.0f;
            }

            ringCount -= VIZ_FFT_SIZE / 2;
            VizFFT(re, im);

            const float tSens = settings.sensitivity / 100.0f;
            const float sliderGain =
                (tSens <= 1.0f)
                    ? 0.25f + tSens * tSens * 2.75f
                    : 3.0f + (tSens - 1.0f) * 4.0f;

                        float nextBands[VIZ_NUM_BANDS] = {};

            float rawMags[VIZ_NUM_BANDS] = {};
            float frameMax = 0.0f;

            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                int bStart = g_logBinStart[b];
                int bEnd = g_logBinStart[b + 1];
                if (bEnd <= bStart)
                    bEnd = bStart + 1;

                float sumSq = 0.0f;
                int count = 0;

                for (int k = bStart; k < bEnd && k < VIZ_FFT_SIZE / 2; ++k) {
                    sumSq += re[k] * re[k] + im[k] * im[k];
                    ++count;
                }

                const float rms = count > 0
                    ? sqrtf(sumSq / static_cast<float>(count))
                    : 0.0f;

                const float eqM = GetVizEQMultiplier(b);

                const float mag = (rms / (VIZ_FFT_SIZE * 0.5f)) /
                        GetBandSensitivity(b) * sliderGain * eqM;

                rawMags[b] = mag;
                frameMax = std::max(frameMax, mag);
            }

            static float s_autoGain = 1.0f;
            static float s_smoothMax = 0.01f;

            const float autoGainStrength =
                std::clamp(settings.autoGainStrength, 0, 100) / 100.0f;

            if (settings.autoGainEnabled && autoGainStrength > 0.0f) {
                if (frameMax > s_smoothMax) {
                    s_smoothMax += (frameMax - s_smoothMax) * 0.5f;
                } else {
                    s_smoothMax += (frameMax - s_smoothMax) * 0.02f;
                }
                s_smoothMax = std::max(0.0001f, s_smoothMax);

                float targetGain = 0.98f / s_smoothMax;
                targetGain = std::clamp(targetGain, 1.0f, 150.0f);

                targetGain = 1.0f + (targetGain - 1.0f) * autoGainStrength;

                if (targetGain > s_autoGain) {
                    s_autoGain += (targetGain - s_autoGain) * 0.035f;
                } else {
                    s_autoGain += (targetGain - s_autoGain) * 0.15f;
                }
            } else {
                s_autoGain = 1.0f;
                s_smoothMax = 0.01f;
            }

            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                float m = rawMags[b];

                if (settings.autoGainEnabled && s_smoothMax > 0.0001f) {
                    float ratio = std::clamp(m / s_smoothMax, 0.0f, 1.0f);
                    ratio = ratio * ratio * ratio;

                    const float appliedGain =
                        1.0f + (s_autoGain - 1.0f) * ratio;
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
        }
    }

    if (client) {
        client->Stop();
        client->Release();
    }
    if (capture)
        capture->Release();

    enumerator->Release();
    ClearAudioBands();
    CoUninitialize();
    return 0;
}

static void StartAudioCapture() {
    if (g_audioRunning.exchange(true, std::memory_order_acq_rel))
        return;

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
    const ULONGLONG nowMs = GetTickCount64();
    const ULONGLONG lastAudioMs =
        g_lastAudioUpdateMs.load(std::memory_order_acquire);

    if (fallbackLastMs == 0)
        fallbackLastMs = nowMs;

    if (lastAudioMs != 0 && nowMs > lastAudioMs + 120) {
        if (fallbackLastMs < lastAudioMs)
            fallbackLastMs = lastAudioMs;

        const float dtFrames = std::clamp(
            static_cast<float>(nowMs - fallbackLastMs) / 16.0f,
            0.0f, 8.0f);

        if (dtFrames > 0.0f) {
            for (int b = 0; b < VIZ_NUM_BANDS; ++b) {
                float v = g_audioBands[b].load(std::memory_order_relaxed);
                v = std::max(0.0f, v - GetBandGravity(b) * dtFrames);
                g_audioBands[b].store(v, std::memory_order_relaxed);
            }
            fallbackLastMs = nowMs;
        }
    } else {
        fallbackLastMs = nowMs;
    }

    static float g_wavePhase = 0.0f;
    g_wavePhase += 0.06f;

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
            const float response = target > current ? attack : decay;
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

static bool IsAlbumColorMode() {
    // Album colors are also consumed by the background, visualizer border,
    // lyrics background, and lyrics border. Keep the album worker alive only
    // when at least one enabled feature can actually use those colors.
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

    struct Bucket {
        uint64_t r = 0;
        uint64_t g = 0;
        uint64_t b = 0;
        uint32_t n = 0;
    };

    Bucket buckets[16][16][16]{};

    for (UINT y = 0; y < h; y += 4) {
        for (UINT x = 0; x < w; x += 4) {
            Gdiplus::Color c;
            if (bitmap.GetPixel(x, y, &c) != Gdiplus::Ok)
                continue;

            const int r = c.GetRed();
            const int g = c.GetGreen();
            const int b = c.GetBlue();
            const int luma = (r * 299 + g * 587 + b * 114) / 1000;


            if (luma < 24 || luma > 235)
                continue;

            auto& bucket = buckets[r >> 4][g >> 4][b >> 4];
            bucket.r += static_cast<uint64_t>(r);
            bucket.g += static_cast<uint64_t>(g);
            bucket.b += static_cast<uint64_t>(b);
            bucket.n++;
        }
    }

    struct Candidate {
        float weight;
        BYTE r;
        BYTE g;
        BYTE b;
    };

    std::vector<Candidate> candidates;
    candidates.reserve(64);

    for (int R = 0; R < 16; ++R) {
        for (int G = 0; G < 16; ++G) {
            for (int B = 0; B < 16; ++B) {
                const auto& bucket = buckets[R][G][B];
                if (bucket.n < 8)
                    continue;

                const float fr = static_cast<float>(bucket.r) /
                    static_cast<float>(bucket.n) / 255.0f;
                const float fg = static_cast<float>(bucket.g) /
                    static_cast<float>(bucket.n) / 255.0f;
                const float fb = static_cast<float>(bucket.b) /
                    static_cast<float>(bucket.n) / 255.0f;

                const float mx = std::max({fr, fg, fb});
                const float mn = std::min({fr, fg, fb});
                const float sat = mx > 0.0f ? (mx - mn) / mx : 0.0f;
                const float weight = static_cast<float>(bucket.n) * (0.3f + sat);

                candidates.push_back({
                    weight,
                    static_cast<BYTE>(fr * 255.0f),
                    static_cast<BYTE>(fg * 255.0f),
                    static_cast<BYTE>(fb * 255.0f)});
            }
        }
    }

    stream->Release();

    if (candidates.empty())
        return fallback;

    std::sort(candidates.begin(), candidates.end(),
        [](const Candidate& a, const Candidate& b) {
            return a.weight > b.weight;
        });

    AlbumPaletteGdi result{
        RGB(candidates[0].r, candidates[0].g, candidates[0].b),
        RGB(candidates[0].r, candidates[0].g, candidates[0].b)};

    const float primaryWeight = candidates[0].weight;
    float bestSecondaryScore = -1.0f;
    const Candidate* bestSecondary = nullptr;

    for (const auto& candidate : candidates) {
        const int dr = static_cast<int>(candidate.r) - candidates[0].r;
        const int dg = static_cast<int>(candidate.g) - candidates[0].g;
        const int db = static_cast<int>(candidate.b) - candidates[0].b;
        const float distance = std::sqrt(
            static_cast<float>(dr * dr + dg * dg + db * db));

        if (candidate.weight < primaryWeight * 0.08f || distance < 36.0f)
            continue;

        const float distanceT = std::clamp(distance / 255.0f, 0.0f, 1.0f);
        const float score = candidate.weight * (0.35f + distanceT * 1.65f);
        if (score > bestSecondaryScore) {
            bestSecondaryScore = score;
            bestSecondary = &candidate;
        }
    }

    if (bestSecondary) {
        result.secondary = RGB(
            bestSecondary->r, bestSecondary->g, bestSecondary->b);
    }

    return result;
}

static size_t HashAlbumBytes(const std::vector<BYTE>& bytes) {
    size_t hash = 0;
    for (size_t i = 0; i < bytes.size(); i += 1024)
        hash = hash * 31 + bytes[i];
    return hash;
}

static void UpdateAlbumPaletteFromBytes(const std::vector<BYTE>& thumbBytes, size_t hash) {
    if (thumbBytes.empty() || hash == 0) {
        std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
        g_albumPalette = AlbumPaletteGdi{};
        g_albumPaletteHash = 0;
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
    }
}

static DWORD GetAlbumPalettePrimary() {
    std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
    return g_albumPalette.primary;
}

static DWORD GetAlbumPaletteSecondary() {
    std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
    return g_albumPalette.secondary;
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

        // Keep this worker polling instead of wiring long-lived WinRT event
        // tokens. The mod is injected into Explorer and can be reconfigured or
        // unloaded while the shell is rebuilding; the existing cancellable
        // async waits make this lifecycle predictable without callback-token
        // revocation races. The interval is deliberately slow because album
        // artwork changes only when media metadata changes.
        while (g_albumColorRunning.load(std::memory_order_acquire)) {
            const VisualizerSettings settings = GetSettingsSnapshot();
            try {
                winrt::Windows::Media::Control::
                    GlobalSystemMediaTransportControlsSession session = nullptr;

                if (settings.audioSource == 1 &&
                    !settings.audioApplicationName.empty()) {
                    session = FindLyricsSession(
                        manager,
                        settings.audioApplicationName,
                        pinnedSourceAppUserModelId);
                } else {
                    session = manager.GetCurrentSession();
                    pinnedSourceAppUserModelId.clear();
                }

                std::vector<BYTE> thumbBytes;

                if (session) {
                    auto propsOperation = session.TryGetMediaPropertiesAsync();
                    if (!WaitWinrtAsync(propsOperation, g_hAlbumColorStopEvent, std::chrono::milliseconds(1500)))
                        throw winrt::hresult_canceled();
                    auto props = propsOperation.GetResults();
                    if (props) {
                        if (auto thumbRef = props.Thumbnail()) {
                            try {
                                auto streamOperation = thumbRef.OpenReadAsync();
                                if (!WaitWinrtAsync(streamOperation, g_hAlbumColorStopEvent, std::chrono::milliseconds(1500)))
                                    throw winrt::hresult_canceled();
                                auto stream = streamOperation.GetResults();
                                if (stream) {
                                    const UINT64 size = stream.Size();
                                    if (size > 0 && size < 4ULL * 1024ULL * 1024ULL) {
                                        winrt::Windows::Storage::Streams::DataReader reader(stream);
                                        auto loadOperation = reader.LoadAsync(static_cast<UINT32>(size));
                                        if (!WaitWinrtAsync(loadOperation, g_hAlbumColorStopEvent, std::chrono::milliseconds(1500)))
                                            throw winrt::hresult_canceled();
                                        thumbBytes.resize(static_cast<size_t>(size));
                                        reader.ReadBytes(winrt::array_view<BYTE>(thumbBytes));
                                        reader.DetachStream();
                                    }
                                }
                            } catch (...) {
                                thumbBytes.clear();
                            }
                        }
                    }
                }

                UpdateAlbumPaletteFromBytes(
                    thumbBytes,
                    HashAlbumBytes(thumbBytes));
            } catch (...) {
                UpdateAlbumPaletteFromBytes({}, 0);
            }

            if (g_hAlbumColorStopEvent) {
                if (WaitForSingleObject(g_hAlbumColorStopEvent, 700) == WAIT_OBJECT_0)
                    break;
            } else {
                Sleep(700);
            }
        }
    } catch (...) {
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

static void FetchLyricsForTrack(
    const std::wstring& title,
    const std::wstring& artist,
    const std::wstring& album,
    double durationSeconds,
    const std::wstring& trackKey) {

    if (title.empty() || artist.empty() || durationSeconds <= 0.0)
        return;

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

static winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession
FindLyricsSession(
    const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager& manager,
    const std::wstring& executableName,
    std::wstring& pinnedSourceAppUserModelId) {

    if (executableName.empty())
        return nullptr;

    using Session = winrt::Windows::Media::Control::
        GlobalSystemMediaTransportControlsSession;

    if (!pinnedSourceAppUserModelId.empty()) {
        try {
            const auto sessions = manager.GetSessions();
            for (const auto& session : sessions) {
                if (!session)
                    continue;

                const std::wstring sourceAppId =
                    NormalizeMediaAppIdentifier(session.SourceAppUserModelId().c_str());
                if (sourceAppId == pinnedSourceAppUserModelId)
                    return session;
            }
        } catch (...) {
        }

        pinnedSourceAppUserModelId.clear();
    }

    try {
        const auto sessions = manager.GetSessions();
        Session bestSession = nullptr;
        Session currentSession = nullptr;

        try {
            currentSession = manager.GetCurrentSession();
        } catch (...) {
        }

        for (const auto& session : sessions) {
            if (!session || !MediaSessionMatchesExecutable(session, executableName))
                continue;

            bool playing = false;
            try {
                using PlaybackStatus =
                    winrt::Windows::Media::Control::
                        GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                playing = session.GetPlaybackInfo().PlaybackStatus() ==
                          PlaybackStatus::Playing;
            } catch (...) {
            }

            if (playing) {
                bestSession = session;
                break;
            }

            if (!bestSession)
                bestSession = session;
        }

        // If GetSessions() failed to expose the target immediately, current
        // session is still safe to use only when it belongs to the configured app.
        if (!bestSession && currentSession &&
            MediaSessionMatchesExecutable(currentSession, executableName)) {
            bestSession = currentSession;
        }

        if (bestSession) {
            pinnedSourceAppUserModelId = NormalizeMediaAppIdentifier(
                bestSession.SourceAppUserModelId().c_str());

            if (!pinnedSourceAppUserModelId.empty()) {
                Wh_Log(L"Lyrics session pinned to AUMID '%s' for executable '%s'",
                       pinnedSourceAppUserModelId.c_str(),
                       executableName.c_str());
            }
        }

        return bestSession;
    } catch (...) {
        return nullptr;
    }
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

                if (settings.audioSource == 1 &&
                    !settings.audioApplicationName.empty()) {
                    session = FindLyricsSession(
                        manager,
                        settings.audioApplicationName,
                        pinnedSourceAppUserModelId);
                } else {
                    session = manager.GetCurrentSession();
                    pinnedSourceAppUserModelId.clear();
                }

                if (session) {
                    haveSession = true;

                    try {
                        auto propsOperation = session.TryGetMediaPropertiesAsync();
                        if (!WaitWinrtAsync(propsOperation, g_hLyricsStopEvent, std::chrono::milliseconds(1500)))
                            throw winrt::hresult_canceled();
                        auto props = propsOperation.GetResults();
                        if (props) {
                            title = props.Title().c_str();
                            artist = props.Artist().c_str();
                            album = props.AlbumTitle().c_str();
                        }
                    } catch (...) {
                        // Keep the rest of the media session alive if
                        // metadata retrieval temporarily fails.
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
                {
                    std::lock_guard<std::mutex> lock(g_lyricsMutex);
                    g_lyricsLines.reset();
                    g_lyricsTrackTitle.clear();
                    g_lyricsTrackArtist.clear();
                    g_lyricsTrackKey.clear();
                    g_lyricsHasSynced = false;
                    g_lyricsAvailable = false;
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

    const bool drawArtist = collapsedUnavailable
        ? false
        : (g_settings.lyricsShowArtist && !artist.empty());
    const bool drawTitle = collapsedUnavailable
        ? !title.empty()
        : (g_settings.lyricsShowTitle && !title.empty());

    if (drawArtist || drawTitle) {
        const std::wstring separator = L"  •  ";

        Gdiplus::RectF measureRect(0.0f, 0.0f, 10000.0f, 100.0f);
        Gdiplus::RectF artistBounds{}, titleBounds{}, separatorBounds{};
        Gdiplus::StringFormat measureFormat;
        measureFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        measureFormat.SetTrimming(Gdiplus::StringTrimmingNone);

        if (drawArtist) {
            graphics.MeasureString(artist.c_str(), -1, &artistFont, measureRect,
                                   &measureFormat, &artistBounds);
        }
        if (drawTitle) {
            graphics.MeasureString(title.c_str(), -1, &titleFont, measureRect,
                                   &measureFormat, &titleBounds);
        }
        if (drawArtist && drawTitle) {
            graphics.MeasureString(separator.c_str(), -1, &artistFont, measureRect,
                                   &measureFormat, &separatorBounds);
        }

        const float metadataWidth =
            (drawArtist ? artistBounds.Width : 0.0f) +
            (drawArtist && drawTitle ? separatorBounds.Width : 0.0f) +
            (drawTitle ? titleBounds.Width : 0.0f);
        float metadataLeft = contentLeft;
        if (g_settings.lyricsTextAlignment == 2) {
            metadataLeft = contentLeft + contentWidth - metadataWidth;
        } else if (g_settings.lyricsTextAlignment == 1) {
            metadataLeft = contentLeft + (contentWidth - metadataWidth) * 0.5f;
        }
        metadataLeft = std::max(contentLeft, metadataLeft);

        Gdiplus::SolidBrush metaBrush(
            Gdiplus::Color(static_cast<BYTE>(totalAlpha * 0.65f),
                           235, 235, 235));
        const float metadataY = static_cast<float>(y + 10);
        const float metadataHeight =
            static_cast<float>(g_settings.lyricsFontSize + 8);

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
            Gdiplus::RectF measured;
            const Gdiplus::RectF measureRect(0.0f, 0.0f, contentWidth, 10000.0f);
            INT codePointsFitted = 0;
            INT linesFilled = 0;
            if (graphics.MeasureString(
                    lines[current].text.c_str(), -1, &font, measureRect,
                    &wrapFormat, &measured, &codePointsFitted, &linesFilled) == Gdiplus::Ok &&
                linesFilled > 1) {
                currentBlockHeight = std::max(
                    lineHeight * static_cast<float>(linesFilled), measured.Height + 2.0f);
            }
        }

        if (!haveLyrics) {
            const std::wstring fallback = g_settings.lyricsUnavailableText.empty()
                ? L"N/A"
                : g_settings.lyricsUnavailableText;
            Gdiplus::SolidBrush fallbackBrush(
                Gdiplus::Color(static_cast<BYTE>(totalAlpha * 0.78f),
                               245, 245, 248));
            const float fallbackY = y + h *
                (static_cast<float>(std::clamp(g_settings.lyricsFocusY, 0, 100)) / 100.0f);
            Gdiplus::RectF fallbackRect(
                contentLeft,
                fallbackY - static_cast<float>(g_settings.lyricsFontSize) * 0.7f,
                contentWidth,
                static_cast<float>(g_settings.lyricsFontSize + 12));
            graphics.DrawString(
                fallback.c_str(), -1, &font, fallbackRect, &textFormat,
                &fallbackBrush);
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
    g_lastRenderDirtyRect = {};
    g_renderNeedsFullClear = true;
}

static bool g_mirrorRenderPass = false;
static int g_mirrorRenderAxis = 0; // 1 = vertical, 2 = horizontal
static bool g_mirrorCircularPass = false;

static bool EnsureRenderTarget(int w, int h) {
    if (w <= 0 || h <= 0)
        return false;

    if (g_renderMemDC && g_renderBitmap && g_renderBits &&
        g_renderWidth == w && g_renderHeight == h) {
        return true;
    }

    DestroyRenderTarget();

    g_renderMemDC = CreateCompatibleDC(nullptr);
    if (!g_renderMemDC)
        return false;

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
    g_renderNeedsFullClear = true;
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

    bool lyricsAvailable = false;
    {
        std::lock_guard<std::mutex> lock(g_lyricsMutex);
        lyricsAvailable = g_lyricsAvailable;
    }

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
    const DWORD primaryColor = (g_settings.colorMode == 7 || g_settings.colorMode == 8)
        ? GetAlbumPalettePrimary()
        : g_settings.color1;
        const DWORD secondaryColor = (g_settings.colorMode == 8)
            ? GetAlbumPaletteSecondary()
            : g_settings.color2;

    const int barCount = std::clamp(g_settings.barCount, 1, VIZ_BANDS_MAX);


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

        if (g_settings.barShape != 4) {
            const int barCrossPos = (g_settings.orientation <= 2)
                ? g_settings.positionX + currentOffset
                : g_settings.positionY + currentOffset;
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

        float heightRatio = static_cast<float>(barLen) / static_cast<float>(std::max(1, g_settings.maxBarHeight));
        heightRatio = std::clamp(heightRatio, 0.0f, 1.0f);

        const float colorT = ApplyHeightCurve(
            heightRatio,
            g_settings.gradientCurveEnabled,
            g_settings.gradientCurve);

        const float opacityT = ApplyHeightCurve(
            heightRatio,
            g_settings.opacityCurveEnabled,
            g_settings.opacityCurve);

        float gradientT = barCount > 1
            ? static_cast<float>(i) / static_cast<float>(barCount - 1)
            : 0.0f;

        DWORD color = primaryColor;
        if (g_settings.barShape == 4) {

            const float angle =
                (static_cast<float>(g_settings.circleStartAngle) +
                 (360.0f * static_cast<float>(i) /
                  static_cast<float>(std::max(1, barCount)))) *
                (VIZ_PI / 180.0f);
            const float circleX = 0.5f + 0.5f * circleCos;
            const float circleY = 0.5f + 0.5f * circleSin;

            if (g_settings.colorMode == 1) {
                color = LerpColor(primaryColor, secondaryColor, circleX);
            } else if (g_settings.colorMode == 6 || g_settings.colorMode == 8) {

                color = LerpColor(primaryColor, secondaryColor, circleY);
            } else if (g_settings.colorMode == 3) {
                color = LerpColor(primaryColor, secondaryColor, colorT); 
            }
        } else {
            if (g_settings.colorMode == 1) {
                color = LerpColor(primaryColor, secondaryColor, gradientT);
            } else if (g_settings.colorMode == 6 || g_settings.colorMode == 8) {
                color = primaryColor;
            } else if (g_settings.colorMode == 3) {
                color = LerpColor(primaryColor, secondaryColor, colorT); 
            }
        }

        int alphaValue = std::clamp(
            255 * g_settings.acrylicOpacity / 100, 10, 255);

        if (g_settings.colorMode == 3) {
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

static void AddDirtyRect(RECT& bounds, const RECT& rect, int renderWidth, int renderHeight) {
    RECT clipped = rect;
    clipped.left = std::clamp<LONG>(clipped.left, 0, renderWidth);
    clipped.top = std::clamp<LONG>(clipped.top, 0, renderHeight);
    clipped.right = std::clamp<LONG>(clipped.right, 0, renderWidth);
    clipped.bottom = std::clamp<LONG>(clipped.bottom, 0, renderHeight);
    if (clipped.right <= clipped.left || clipped.bottom <= clipped.top)
        return;

    if (bounds.right <= bounds.left || bounds.bottom <= bounds.top) {
        bounds = clipped;
    } else {
        bounds.left = std::min<LONG>(bounds.left, clipped.left);
        bounds.top = std::min<LONG>(bounds.top, clipped.top);
        bounds.right = std::max<LONG>(bounds.right, clipped.right);
        bounds.bottom = std::max<LONG>(bounds.bottom, clipped.bottom);
    }
}

static RECT MirrorDirtyRect(const RECT& rect, int axis, int renderWidth, int renderHeight) {
    RECT mirrored = rect;
    if (axis == 1) {
        mirrored.top = renderHeight - rect.bottom;
        mirrored.bottom = renderHeight - rect.top;
    } else if (axis == 2) {
        mirrored.left = renderWidth - rect.right;
        mirrored.right = renderWidth - rect.left;
    }
    return mirrored;
}

static RECT GetOverlayContentBounds(int renderWidth, int renderHeight) {
    RECT bounds{};
    const int barCount = std::clamp(g_settings.barCount, 1, VIZ_BANDS_MAX);

    // Use the maximum possible visualizer geometry. This is intentionally
    // conservative: the persistent DIB is only cleared/uploaded inside this
    // region, so underestimating it could leave stale pixels behind when bars
    // move or when settings change. The extra dynamic-width allowance covers
    // the cursor-driven widening without changing the rendering geometry.
    RECT visualizerRect = GetVisualizerBackgroundRect(
        barCount, 1.0f, g_settings.backgroundPadding,
        g_settings.backgroundHeightAdjustment);
    const int extraCross = g_settings.dynamicWidthEnabled
        ? barCount * std::max(0, g_settings.dynamicWidthMaxBonus)
        : 0;
    if (g_settings.orientation <= 2)
        visualizerRect.right += extraCross + std::max(2, g_settings.borderThickness + 1);
    else
        visualizerRect.bottom += extraCross + std::max(2, g_settings.borderThickness + 1);
    visualizerRect.left -= std::max(2, g_settings.borderThickness + 1);
    visualizerRect.top -= std::max(2, g_settings.borderThickness + 1);
    AddDirtyRect(bounds, visualizerRect, renderWidth, renderHeight);

    if (g_settings.mirroredVisualizer) {
        const int axis = (g_settings.barShape == 4)
            ? 2
            : ((g_settings.orientation > 2) ? 2 : 1);
        AddDirtyRect(
            bounds,
            MirrorDirtyRect(visualizerRect, axis, renderWidth, renderHeight),
            renderWidth, renderHeight);
    }

    if (g_settings.lyricsEnabled &&
        g_settings.lyricsOpacity > 0 &&
        g_settings.lyricsWidth > 0 && g_settings.lyricsHeight > 0) {
        RECT lyricsRect{
            g_settings.lyricsX,
            g_settings.lyricsY,
            g_settings.lyricsX + g_settings.lyricsWidth,
            g_settings.lyricsY + g_settings.lyricsHeight
        };
        const int margin = std::max(2, g_settings.lyricsBorderThickness + 1);
        lyricsRect.left -= margin;
        lyricsRect.top -= margin;
        lyricsRect.right += margin;
        lyricsRect.bottom += margin;
        AddDirtyRect(bounds, lyricsRect, renderWidth, renderHeight);
    }

    if (g_pForegroundImage) {
        const int drawW = (g_settings.imageWidth > 0)
            ? g_settings.imageWidth : static_cast<int>(g_pForegroundImage->GetWidth());
        const int drawH = (g_settings.imageHeight > 0)
            ? g_settings.imageHeight : static_cast<int>(g_pForegroundImage->GetHeight());
        if (drawW > 0 && drawH > 0) {
            RECT imageRect{
                g_settings.imageX,
                g_settings.imageY,
                g_settings.imageX + drawW,
                g_settings.imageY + drawH
            };
            AddDirtyRect(bounds, imageRect, renderWidth, renderHeight);
        }
    }

    return bounds;
}

static void ClearRenderRegion(RECT rect, int width, int height) {
    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;
    rect.left = std::clamp<LONG>(rect.left, 0, width);
    rect.top = std::clamp<LONG>(rect.top, 0, height);
    rect.right = std::clamp<LONG>(rect.right, 0, width);
    rect.bottom = std::clamp<LONG>(rect.bottom, 0, height);
    if (rect.right <= rect.left || rect.bottom <= rect.top)
        return;

    BYTE* bits = static_cast<BYTE*>(g_renderBits);
    const size_t stride = static_cast<size_t>(width) * sizeof(DWORD);
    const size_t rowBytes = static_cast<size_t>(rect.right - rect.left) * sizeof(DWORD);
    for (int y = rect.top; y < rect.bottom; ++y) {
        std::memset(
            bits + static_cast<size_t>(y) * stride +
                static_cast<size_t>(rect.left) * sizeof(DWORD),
            0,
            rowBytes);
    }
}

static void RenderOverlay(HWND hwnd) {
    EnsureForegroundImageLoaded();

    RECT rc{};
    GetClientRect(hwnd, &rc);
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0)
        return;

    if (!EnsureRenderTarget(w, h))
        return;

    // The persistent render target must be cleared completely before each
    // frame. Partial dirty-region clearing can leave stale pixels behind when
    // a bar shrinks, especially with mirrored/circular rendering or effects
    // whose geometry changes with the bar height. Those stale pixels appear as
    // "ghost" bars even though g_currentHeights is already falling normally.
    //
    // Keep the persistent DIB for allocation stability, but use a full-frame
    // clear/upload so every frame exactly represents the current visualizer.
    std::memset(
        g_renderBits, 0,
        static_cast<size_t>(w) * static_cast<size_t>(h) * sizeof(DWORD));

    const RECT dirtyRect{0, 0, w, h};
    g_renderNeedsFullClear = false;

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

    POINT dstPos{0, 0};
    SIZE size{w, h};
    POINT srcPos{0, 0};
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UPDATELAYEREDWINDOWINFO updateInfo{};
    updateInfo.cbSize = sizeof(updateInfo);
    updateInfo.pptDst = &dstPos;
    updateInfo.psize = &size;
    updateInfo.hdcSrc = g_renderMemDC;
    updateInfo.pptSrc = &srcPos;
    updateInfo.pblend = &blend;
    updateInfo.dwFlags = ULW_ALPHA;
    updateInfo.prcDirty = &dirtyRect;

    UpdateLayeredWindowIndirect(hwnd, &updateInfo);

    // Kept for compatibility with the existing render-target lifecycle.
    g_lastRenderDirtyRect = dirtyRect;
}

static UINT GetRenderIntervalMs(const VisualizerSettings& settings);

static UINT g_cachedDisplayRefreshRateHz = 0;
static UINT g_currentOverlayTimerMs = 0;


// popup implementation
static constexpr int EQ_POPUP_WIDTH = 520;
static constexpr int EQ_POPUP_HEIGHT = 300;
static constexpr int EQ_SLIDER_TOP = 76;
static constexpr int EQ_SLIDER_BOTTOM = 242;
static constexpr int EQ_SLIDER_HORIZONTAL_PADDING = 34;
static constexpr int EQ_BUTTON_WIDTH = 32;
static constexpr int EQ_BUTTON_HEIGHT = 32;
static constexpr int EQ_POPUP_TASKBAR_GAP = 16;
static HWND g_eqPopupHwnd = nullptr;
static HINSTANCE g_eqModuleHandle = nullptr;
static bool g_eqClassesRegistered = false;
static int g_eqHotBand = -1;
static int g_eqDraggingBand = -1;
static const wchar_t* kEqPopupClass = L"WindhawkVisualizerEQPopup";

static int EqPopupScalePx(int value, double scale);
static double EqGetPopupDpiScale();

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

static POINT GetEqSliderPoint(int index) {
    const int usableWidth = EQ_POPUP_WIDTH - EQ_SLIDER_HORIZONTAL_PADDING * 2;
    const float t = VIZ_EQ_BANDS > 1
        ? static_cast<float>(index) / static_cast<float>(VIZ_EQ_BANDS - 1)
        : 0.0f;
    return POINT{
        EQ_SLIDER_HORIZONTAL_PADDING +
            static_cast<int>(std::round(t * usableWidth)),
        EQ_SLIDER_TOP};
}

static int HitTestEqBand(int x) {
    const double scale = EqGetPopupDpiScale();
    const int logicalX = static_cast<int>(std::lround(
        static_cast<double>(x) / scale));

    int best = -1;
    int bestDistance = 17;
    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const POINT point = GetEqSliderPoint(i);
        const int distance = std::abs(logicalX - point.x);
        if (distance < bestDistance) {
            bestDistance = distance;
            best = i;
        }
    }
    return best;
}

static float GetEqGainAtBand(int index) {
    return std::clamp(
        g_customEqGains[static_cast<size_t>(index)].load(std::memory_order_relaxed),
        0.0f, 2.0f);
}

static void SetEqGainFromMouse(int index, int y) {
    if (index < 0 || index >= VIZ_EQ_BANDS)
        return;

    const double scale = EqGetPopupDpiScale();
    const float logicalY = static_cast<float>(
        static_cast<double>(y) / scale);
    const float t = std::clamp(
        (logicalY - static_cast<float>(EQ_SLIDER_TOP)) /
            static_cast<float>(std::max(1, EQ_SLIDER_BOTTOM - EQ_SLIDER_TOP)),
        0.0f, 1.0f);
    const float gain = std::clamp(2.0f * (1.0f - t), 0.0f, 2.0f);
    g_customEqGains[static_cast<size_t>(index)].store(
        gain, std::memory_order_relaxed);
}

static void RenderEqPopup(HWND hwnd) {
    if (!hwnd)
        return;

    const bool dark = IsDarkThemeEnabled();
    const double scale = EqGetPopupDpiScale();
    const int width = EqPopupScalePx(EQ_POPUP_WIDTH, scale);
    const int height = EqPopupScalePx(EQ_POPUP_HEIGHT, scale);

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
                          static_cast<float>(EQ_POPUP_WIDTH - 2),
                          static_cast<float>(EQ_POPUP_HEIGHT - 2), 16.0f);
    Gdiplus::SolidBrush bg(bgColor);
    graphics.FillPath(&bg, &bgPath);

    Gdiplus::Color titleColor(255,
                              dark ? 245 : 30,
                              dark ? 245 : 30,
                              dark ? 248 : 30);
    Gdiplus::FontFamily titleFamily(L"Segoe UI Semibold");
    Gdiplus::Font titleFont(&titleFamily, 17.0f,
                           Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush titleBrush(titleColor);
    graphics.DrawString(L"Equalizer", -1, &titleFont,
                        Gdiplus::PointF(20.0f, 15.0f), &titleBrush);

    Gdiplus::FontFamily smallFamily(L"Segoe UI");
    Gdiplus::Font smallFont(&smallFamily, 9.0f,
                            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush subBrush(Gdiplus::Color(
        190,
        dark ? 210 : 80,
        dark ? 210 : 80,
        dark ? 215 : 85));
    graphics.DrawString(L"Gain", -1, &smallFont,
                        Gdiplus::PointF(20.0f, 45.0f), &subBrush);

    const Gdiplus::Color trackColor(
        110,
        dark ? 150 : 125,
        dark ? 150 : 125,
        dark ? 155 : 130);
    const Gdiplus::Color accentColor(
        255, 105, 165, 255);
    const Gdiplus::Color labelColor(
        235,
        dark ? 225 : 55,
        dark ? 225 : 55,
        dark ? 230 : 60);

    Gdiplus::Pen trackPen(trackColor, 4.0f);
    trackPen.SetStartCap(Gdiplus::LineCapRound);
    trackPen.SetEndCap(Gdiplus::LineCapRound);

    for (int i = 0; i < VIZ_EQ_BANDS; ++i) {
        const POINT point = GetEqSliderPoint(i);
        graphics.DrawLine(&trackPen,
                          static_cast<float>(point.x),
                          static_cast<float>(EQ_SLIDER_TOP),
                          static_cast<float>(point.x),
                          static_cast<float>(EQ_SLIDER_BOTTOM));

        const float gain = GetEqGainAtBand(i);
        const float knobY = EQ_SLIDER_BOTTOM -
            (gain / 2.0f) * (EQ_SLIDER_BOTTOM - EQ_SLIDER_TOP);

        Gdiplus::Pen fillPen(accentColor, 4.0f);
        fillPen.SetStartCap(Gdiplus::LineCapRound);
        fillPen.SetEndCap(Gdiplus::LineCapRound);
        graphics.DrawLine(&fillPen,
                          static_cast<float>(point.x),
                          static_cast<float>(knobY),
                          static_cast<float>(point.x),
                          static_cast<float>(EQ_SLIDER_BOTTOM));

        Gdiplus::SolidBrush knobBrush(accentColor);
        graphics.FillEllipse(&knobBrush,
                             static_cast<float>(point.x) - 7.0f,
                             knobY - 7.0f,
                             14.0f, 14.0f);

        if (i == g_eqHotBand || i == g_eqDraggingBand) {
            wchar_t gainText[16]{};
            swprintf_s(gainText, L"%.2f", gain);
            Gdiplus::SolidBrush gainBrush(labelColor);
            Gdiplus::RectF gainRect(
                static_cast<float>(point.x) - 22.0f,
                knobY - 29.0f,
                44.0f, 18.0f);
            Gdiplus::StringFormat centered;
            centered.SetAlignment(Gdiplus::StringAlignmentCenter);
            centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            graphics.DrawString(gainText, -1, &smallFont,
                                gainRect, &centered, &gainBrush);
        }

        wchar_t label[32]{};
        const float high = VIZ_EQ_HIGH_HZ[static_cast<size_t>(i)];
        const float low = VIZ_EQ_LOW_HZ[static_cast<size_t>(i)];
        auto formatFreq = [](float hz, wchar_t* out, size_t count) {
            if (hz >= 1000.0f)
                swprintf_s(out, count, L"%.0fk", hz / 1000.0f);
            else
                swprintf_s(out, count, L"%.0f", hz);
        };
        wchar_t lowText[16]{}, highText[16]{};
        formatFreq(low, lowText, ARRAYSIZE(lowText));
        formatFreq(high, highText, ARRAYSIZE(highText));
        swprintf_s(label, L"%s-%s", lowText, highText);

        Gdiplus::RectF labelRect(
            static_cast<float>(point.x) - 29.0f,
            static_cast<float>(EQ_SLIDER_BOTTOM + 14),
            58.0f, 28.0f);
        Gdiplus::StringFormat centered;
        centered.SetAlignment(Gdiplus::StringAlignmentCenter);
        centered.SetLineAlignment(Gdiplus::StringAlignmentNear);
        Gdiplus::SolidBrush textBrush(labelColor);
        graphics.DrawString(label, -1, &smallFont,
                            labelRect, &centered, &textBrush);
    }

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
static bool EqIsCursorOverXamlButton();
static LRESULT CALLBACK EqPopupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_ACTIVATE;
    case WM_SETFOCUS:
        RenderEqPopup(hwnd);
        return 0;
    case WM_KILLFOCUS:
        if (g_eqDraggingBand >= 0) {
            SaveCustomEQSettings();
            g_eqDraggingBand = -1;
            ReleaseCapture();
        }
        // A click on our XAML tray button also causes the popup to lose focus.
        // Keep it alive for that click so EqShowPopupForXamlButton() can perform
        // a real toggle instead of reopening the popup immediately.
        if (!EqIsCursorOverXamlButton())
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
        return 0;
    case WM_MOUSEMOVE: {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        const int hot = HitTestEqBand(x);
        if (hot != g_eqHotBand) {
            g_eqHotBand = hot;
            RenderEqPopup(hwnd);
        }
        if (g_eqDraggingBand >= 0) {
            SetEqGainFromMouse(g_eqDraggingBand, y);
            RenderEqPopup(hwnd);
            return 0;
        }
        TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
        TrackMouseEvent(&tme);
        return 0;
    }
    case WM_MOUSELEAVE:
        if (g_eqDraggingBand < 0 && g_eqHotBand != -1) {
            g_eqHotBand = -1;
            RenderEqPopup(hwnd);
        }
        return 0;
    case WM_LBUTTONDOWN: {
        const int band = HitTestEqBand(GET_X_LPARAM(lParam));
        if (band >= 0) {
            g_eqDraggingBand = band;
            g_eqHotBand = band;
            SetEqGainFromMouse(band, GET_Y_LPARAM(lParam));
            SetCapture(hwnd);
            RenderEqPopup(hwnd);
        }
        return 0;
    }
    case WM_LBUTTONUP:
        if (g_eqDraggingBand >= 0) {
            SetEqGainFromMouse(g_eqDraggingBand, GET_Y_LPARAM(lParam));
            SaveCustomEQSettings();
            g_eqDraggingBand = -1;
            ReleaseCapture();
            RenderEqPopup(hwnd);
            PostMessageW(g_hwndOverlay, WM_VIZ_AUDIO_WAKE, 0, 0);
        }
        return 0;
    case WM_CANCELMODE:
    case WM_CAPTURECHANGED:
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

    HWND hwnd = g_eqPopupHwnd;
    g_eqPopupHwnd = nullptr;
    g_eqHotBand = -1;

    if (hwnd && IsWindow(hwnd))
        DestroyWindow(hwnd);
}

// -----------------------------------------------------------------------------
// Custom EQ taskbar integration.
//
// IMPORTANT: The EQ button is a real XAML child of the Windows 11
// system-tray XAML tree, rather than a separate HWND. The code adapts to
// Grid/StackPanel changes in recent Windows 11 builds and avoids changing the
// outer tray Grid's ColumnDefinitions, reducing conflicts with Taskbar Styler.
// -----------------------------------------------------------------------------
static constexpr wchar_t kEqXamlButtonName[] = L"WindhawkVisualizerEQButton";
static constexpr wchar_t kEqXamlTrayGridName[] = L"SystemTrayFrameGrid";
static constexpr wchar_t kEqButtonGlyph[] = L"\xE9E9"; // Segoe Fluent Icons: Equalizer.

static HWND g_eqTaskbarHwnd = nullptr;
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

static void EqShowPopupForXamlButton(Button const& button) {
    try {
        if (!button)
            return;

        if (g_eqPopupHwnd && IsWindow(g_eqPopupHwnd)) {
            EqClosePopup();
            return;
        }

        RECT taskbarRect{};
        if (!g_eqTaskbarHwnd || !GetWindowRect(g_eqTaskbarHwnd, &taskbarRect))
            return;

        const double scale = EqGetPopupDpiScale();
        const int popupWidth = EqPopupScalePx(EQ_POPUP_WIDTH, scale);
        const int popupHeight = EqPopupScalePx(EQ_POPUP_HEIGHT, scale);
        const int taskbarGap = EqPopupScalePx(EQ_POPUP_TASKBAR_GAP, scale);

        RECT buttonRect{};
        if (!EqGetXamlButtonScreenRect(button, &buttonRect))
            return;

        const int buttonLeft = buttonRect.left;
        const int buttonTop = buttonRect.top;
        const int buttonRight = buttonRect.right;
        const int buttonBottom = buttonRect.bottom;

        int x = buttonLeft - (popupWidth - (buttonRight - buttonLeft)) / 2;
        int y = buttonTop - popupHeight - taskbarGap;

        // Anchor the flyout to the actual taskbar edge instead of the XAML
        // button's transformed coordinates. This remains correct when the
        // taskbar is auto-hidden, and prevents the popup from extending under
        // the taskbar because of XAML/DPI coordinate differences.
        MONITORINFO preMi{};
        preMi.cbSize = sizeof(preMi);
        HMONITOR preMonitor = MonitorFromWindow(g_eqTaskbarHwnd, MONITOR_DEFAULTTONEAREST);
        if (preMonitor && GetMonitorInfoW(preMonitor, &preMi)) {
            const RECT& mr = preMi.rcMonitor;
            const int topEdgeDistance = std::abs(taskbarRect.top - mr.top);
            const int bottomEdgeDistance = std::abs(mr.bottom - taskbarRect.bottom);
            const int leftEdgeDistance = std::abs(taskbarRect.left - mr.left);
            const int rightEdgeDistance = std::abs(mr.right - taskbarRect.right);

            const int minVertical = std::min(topEdgeDistance, bottomEdgeDistance);
            const int minHorizontal = std::min(leftEdgeDistance, rightEdgeDistance);
            const bool bottomTaskbar = bottomEdgeDistance == minVertical &&
                                       bottomEdgeDistance <= leftEdgeDistance &&
                                       bottomEdgeDistance <= rightEdgeDistance;
            const bool topTaskbar = topEdgeDistance == minVertical &&
                                    topEdgeDistance < bottomEdgeDistance &&
                                    topEdgeDistance <= leftEdgeDistance &&
                                    topEdgeDistance <= rightEdgeDistance;
            const bool leftTaskbar = leftEdgeDistance == minHorizontal &&
                                     leftEdgeDistance < rightEdgeDistance &&
                                     leftEdgeDistance < topEdgeDistance &&
                                     leftEdgeDistance < bottomEdgeDistance;

            if (bottomTaskbar) {
                // Use the earlier of the work-area bottom and the physical
                // taskbar top. Both are above the taskbar in the visible and
                // auto-hide cases respectively.
                const int anchorY = std::min(taskbarRect.top, preMi.rcWork.bottom);
                y = anchorY - popupHeight - taskbarGap;
            } else if (topTaskbar) {
                const int anchorY = std::max(taskbarRect.bottom, preMi.rcWork.top);
                y = anchorY + taskbarGap;
            } else if (leftTaskbar) {
                x = taskbarRect.right + taskbarGap;
                y = buttonTop - popupHeight / 2;
            } else {
                x = taskbarRect.left - popupWidth - taskbarGap;
                y = buttonTop - popupHeight / 2;
            }
        }

        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        HMONITOR monitor = MonitorFromWindow(g_eqTaskbarHwnd, MONITOR_DEFAULTTONEAREST);
        if (monitor && GetMonitorInfoW(monitor, &mi)) {
            x = std::clamp<int>(x, mi.rcWork.left + taskbarGap / 2,
                           mi.rcWork.right - popupWidth - taskbarGap / 2);
            y = std::clamp<int>(y, mi.rcWork.top + taskbarGap / 2,
                           mi.rcWork.bottom - popupHeight - taskbarGap / 2);
        }

        g_eqPopupHwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_LAYERED,
            kEqPopupClass,
            L"Equalizer",
            WS_POPUP,
            x, y, popupWidth, popupHeight,
            nullptr, nullptr, g_eqModuleHandle, nullptr);
        if (!g_eqPopupHwnd)
            return;

        ApplyEqPopupWindowAttributes(g_eqPopupHwnd);
        ShowWindow(g_eqPopupHwnd, SW_SHOWNOACTIVATE);
        SetWindowPos(g_eqPopupHwnd, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
        SetForegroundWindow(g_eqPopupHwnd);
        SetFocus(g_eqPopupHwnd);
        RenderEqPopup(g_eqPopupHwnd);
    } catch (...) {
        g_eqPopupHwnd = nullptr;
    }
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
    tooltip.Content(winrt::box_value(winrt::hstring{L"Equalizer"}));
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
    if (!g_eqTaskbarSymbolsHooked)
        return;

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar || !IsWindow(taskbar))
        return;

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

    if (!g_running.load(std::memory_order_acquire))
        return;

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar)
        return;

    // StartTaskbar is called when Windows rebuilds the taskbar/tray XAML.
    // Immediately invalidate our cached object graph and reinject on the taskbar
    // UI thread instead of waiting for the 1-second polling fallback.
    EqHideLegacyNativeButtons(taskbar);
    g_eqTaskbarHwnd = taskbar;
    EqRunFromWindowThread(taskbar, [](void*) {
        EqRemoveXamlButtonOnUiThread();
        auto root = EqGetTaskbarXamlRoot(g_eqTaskbarHwnd);
        if (root)
            EqInjectXamlButtonOnUiThread(g_eqTaskbarHwnd, root);
    }, nullptr);
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

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TIMER:
        if (wParam == 1) {
            std::shared_lock<std::shared_mutex> settingsLock(g_settingsMutex);
            UpdateAnimationFromAudio();
            RenderOverlay(hwnd);

            const int barCount = std::clamp(g_settings.barCount, 1, VIZ_BANDS_MAX);
            const float peakRatio = GetCurrentVisualizerPeakRatio(barCount);
            const float minRatio = static_cast<float>(g_settings.minBarHeight) /
                static_cast<float>(std::max(1, g_settings.maxBarHeight));
            const bool shouldIdle =
                peakRatio <= std::min(1.0f, minRatio + 0.01f);
            if (shouldIdle != g_overlayIdle.load(std::memory_order_acquire))
                g_overlayIdle.store(shouldIdle, std::memory_order_release);

            const UINT intervalMs = shouldIdle ? 200 : GetRenderIntervalMs(g_settings);
            if (intervalMs != g_currentOverlayTimerMs) {
                g_currentOverlayTimerMs = intervalMs;
                SetTimer(hwnd, 1, intervalMs, nullptr);
            }
        }
        return 0;

    case WM_VIZ_AUDIO_WAKE:
        g_overlayIdle.store(false, std::memory_order_release);
        {
            const VisualizerSettings settings = GetSettingsSnapshot();
            const UINT intervalMs = GetRenderIntervalMs(settings);
            if (intervalMs != g_currentOverlayTimerMs) {
                g_currentOverlayTimerMs = intervalMs;
                SetTimer(hwnd, 1, intervalMs, nullptr);
            }
        }
        return 0;

    case WM_PAINT:
        {
            std::shared_lock<std::shared_mutex> settingsLock(g_settingsMutex);
            RenderOverlay(hwnd);
        }
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
        if (g_eqXamlInjectionReady.load(std::memory_order_acquire) &&
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
        g_backgroundBlurNeedsReload.store(true, std::memory_order_release);
        g_cachedDisplayRefreshRateHz = 0;
        g_currentOverlayTimerMs = 0;
        if (GetParent(hwnd) == nullptr) {
            SetWindowPos(hwnd, HWND_BOTTOM,
                         GetSystemMetrics(SM_XVIRTUALSCREEN),
                         GetSystemMetrics(SM_YVIRTUALSCREEN),
                         std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN)),
                         std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN)),
                         SWP_NOACTIVATE | SWP_NOZORDER);
        }
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_NCHITTEST:
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

static UINT GetDisplayRefreshRateHz() {
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
        GetMonitorInfoW(monitor, reinterpret_cast<MONITORINFO*>(&monitorInfo)) &&
        EnumDisplaySettingsW(
            monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode) &&
        devMode.dmDisplayFrequency > 1) {
        return static_cast<UINT>(devMode.dmDisplayFrequency);
    }

    // 60 Hz is a safe fallback when the current display mode cannot be queried.
    return 60;
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

static DWORD WINAPI OverlayThreadProc(LPVOID) {
    Gdiplus::GdiplusStartupInput gdiplusInput{};
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok) {
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

    // The taskbar helper is optional. If its small native windows cannot be
    // registered, the core visualizer continues normally rather than failing
    // the entire mod because of a UI convenience feature.
    RegisterEqWindowClasses(wc.hInstance);

    const int screenW = std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN));
    const int screenH = std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN));

    HWND hParent = nullptr;
    HWND lastProgman = nullptr;
    // Explorer can load the injected module before the desktop shell hierarchy
    // exists. Creating a popup here is unsafe: it can render into an incomplete
    // desktop and can also keep a stale surface after Explorer rebuilds itself.
    // Wait for the real SHELLDLL_DefView instead; the wait is stoppable and uses
    // the same child-window model as the normal desktop path.
    for (;;) {
        if (g_hOverlayStopEvent &&
            WaitForSingleObject(g_hOverlayStopEvent, 0) == WAIT_OBJECT_0) {
            EqCleanupIntegration();
            if (classRegistered)
                UnregisterClassW(wc.lpszClassName, wc.hInstance);
            Gdiplus::GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            return 0;
        }

        HWND hProgman = FindWindowW(L"Progman", nullptr);
        DWORD progmanProcessId = 0;
        if (hProgman)
            GetWindowThreadProcessId(hProgman, &progmanProcessId);

        if (hProgman && progmanProcessId == GetCurrentProcessId()) {
            if (hProgman != lastProgman) {
                lastProgman = hProgman;
                // Undocumented Progman message used by common desktop-overlay
                // implementations to create/reposition the WorkerW behind icons.
                // Send it once per Progman instance instead of repeatedly while
                // the shell is starting, avoiding unnecessary cross-thread work.
                SendMessageTimeoutW(hProgman, 0x052C, 0, 0,
                                    SMTO_ABORTIFHUNG, 1000, nullptr);
            }

            hParent = FindWindowExW(
                hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
            if (!hParent) {
                HWND hWorkerW = nullptr;
                while ((hWorkerW = FindWindowExW(
                            nullptr, hWorkerW, L"WorkerW", nullptr)) != nullptr) {
                    hParent = FindWindowExW(
                        hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
                    if (hParent)
                        break;
                }
            }
        }

        if (hParent)
            break;

        if (g_hOverlayStopEvent)
            WaitForSingleObject(g_hOverlayStopEvent, 250);
        else
            Sleep(250);
    }

    RECT parentRect{};
    const int initialW = GetClientRect(hParent, &parentRect)
        ? std::max(1L, parentRect.right - parentRect.left)
        : screenW;
    const int initialH = GetClientRect(hParent, &parentRect)
        ? std::max(1L, parentRect.bottom - parentRect.top)
        : screenH;

    g_hwndOverlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"Desktop Audio Visualizer",
        WS_CHILD | WS_VISIBLE,
        0, 0, initialW, initialH, hParent, nullptr, wc.hInstance, nullptr);

    if (!g_hwndOverlay) {
        if (classRegistered)
            UnregisterClassW(wc.lpszClassName, wc.hInstance);
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return 0;
    }

    g_overlayWakeHwnd.store(g_hwndOverlay, std::memory_order_release);

    BOOL excludeFromPeek = TRUE;
    DwmSetWindowAttribute(g_hwndOverlay, DWMWA_EXCLUDED_FROM_PEEK,
                          &excludeFromPeek, sizeof(excludeFromPeek));

    g_overlayIdle.store(false, std::memory_order_release);
    {
        const VisualizerSettings settings = GetSettingsSnapshot();
        g_currentOverlayTimerMs = GetRenderIntervalMs(settings);
        SetTimer(g_hwndOverlay, 1, g_currentOverlayTimerMs, nullptr);
    }

    MSG msg{};
    HANDLE stopEvent = g_hOverlayStopEvent;
    ULONGLONG lastShellCheckMs = GetTickCount64() - 1000;

    for (;;) {
        DWORD waitResult = MsgWaitForMultipleObjects(
            stopEvent ? 1 : 0, stopEvent ? &stopEvent : nullptr,
            FALSE, 250, QS_ALLINPUT);

        if (stopEvent && waitResult == WAIT_OBJECT_0)
            break;

        if (g_running.load(std::memory_order_acquire)) {
            const ULONGLONG nowMs = GetTickCount64();
            if (nowMs - lastShellCheckMs >= 1000) {
                lastShellCheckMs = nowMs;
                EqEnsureXamlButton();
                HWND expectedParent = nullptr;
                HWND currentProgman = FindWindowW(L"Progman", nullptr);
            if (currentProgman) {
                expectedParent = FindWindowExW(
                    currentProgman, nullptr, L"SHELLDLL_DefView", nullptr);
                if (!expectedParent) {
                    HWND worker = nullptr;
                    while ((worker = FindWindowExW(
                                nullptr, worker, L"WorkerW", nullptr)) != nullptr) {
                        expectedParent = FindWindowExW(
                            worker, nullptr, L"SHELLDLL_DefView", nullptr);
                        if (expectedParent)
                            break;
                    }
                }
            }
            const bool overlayMissing =
                !g_hwndOverlay || !IsWindow(g_hwndOverlay) ||
                (expectedParent && GetParent(g_hwndOverlay) != expectedParent);
            if (overlayMissing) {
                // The overlay is owned by this thread, so explicitly destroy a
                // stale child before attaching a new one to the rebuilt shell.
                if (g_hwndOverlay && IsWindow(g_hwndOverlay))
                    DestroyWindow(g_hwndOverlay);
                g_hwndOverlay = nullptr;
                g_overlayWakeHwnd.store(nullptr, std::memory_order_release);

                // During Explorer restart the old shell view may disappear for
                // several polling intervals. Do not fall back to a popup: wait
                // for the new SHELLDLL_DefView so the mod cannot render before
                // the desktop is ready or attach to the wrong window tree.
                HWND recoveryParent = nullptr;
                HWND recoveryProgman = FindWindowW(L"Progman", nullptr);
                DWORD recoveryPid = 0;
                if (recoveryProgman)
                    GetWindowThreadProcessId(recoveryProgman, &recoveryPid);
                if (recoveryProgman)
                    lastProgman = recoveryProgman;

                if (recoveryProgman && recoveryPid == GetCurrentProcessId()) {
                    recoveryParent = FindWindowExW(
                        recoveryProgman, nullptr, L"SHELLDLL_DefView", nullptr);
                    if (!recoveryParent) {
                        HWND worker = nullptr;
                        while ((worker = FindWindowExW(
                                    nullptr, worker, L"WorkerW", nullptr)) != nullptr) {
                            recoveryParent = FindWindowExW(
                                worker, nullptr, L"SHELLDLL_DefView", nullptr);
                            if (recoveryParent)
                                break;
                        }
                    }
                }

                if (recoveryParent) {
                    RECT recoveryRect{};
                    const int recoveryW = GetClientRect(recoveryParent, &recoveryRect)
                        ? std::max(1L, recoveryRect.right - recoveryRect.left)
                        : screenW;
                    const int recoveryH = GetClientRect(recoveryParent, &recoveryRect)
                        ? std::max(1L, recoveryRect.bottom - recoveryRect.top)
                        : screenH;

                    g_hwndOverlay = CreateWindowExW(
                        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                        wc.lpszClassName,
                        L"Desktop Audio Visualizer",
                        WS_CHILD | WS_VISIBLE,
                        0, 0, recoveryW, recoveryH,
                        recoveryParent, nullptr, wc.hInstance, nullptr);

                    if (g_hwndOverlay) {
                        g_overlayWakeHwnd.store(g_hwndOverlay, std::memory_order_release);
                        BOOL excludeFromPeek = TRUE;
                        DwmSetWindowAttribute(
                            g_hwndOverlay, DWMWA_EXCLUDED_FROM_PEEK,
                            &excludeFromPeek, sizeof(excludeFromPeek));
                        g_overlayIdle.store(false, std::memory_order_release);
                        g_lastRenderDirtyRect = {};
                        g_renderNeedsFullClear = true;
                        const VisualizerSettings settings = GetSettingsSnapshot();
                        g_currentOverlayTimerMs = GetRenderIntervalMs(settings);
                        SetTimer(g_hwndOverlay, 1, g_currentOverlayTimerMs, nullptr);
                    }
                }
            } else {
                HWND parent = GetParent(g_hwndOverlay);
                if (parent) {
                    RECT parentRect{};
                    if (GetClientRect(parent, &parentRect)) {
                        const int pw = std::max(1L, parentRect.right - parentRect.left);
                        const int ph = std::max(1L, parentRect.bottom - parentRect.top);
                        RECT currentRect{};
                        GetClientRect(g_hwndOverlay, &currentRect);
                        if (currentRect.right - currentRect.left != pw ||
                            currentRect.bottom - currentRect.top != ph) {
                            SetWindowPos(g_hwndOverlay, nullptr, 0, 0, pw, ph,
                                         SWP_NOACTIVATE | SWP_NOZORDER);
                        }
                    }
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
    EqCleanupIntegration();
    if (g_hwndOverlay) {
        g_overlayWakeHwnd.store(nullptr, std::memory_order_release);
        KillTimer(g_hwndOverlay, 1);
        DestroyWindow(g_hwndOverlay);
        g_hwndOverlay = nullptr;
    }

    DestroyRenderTarget();

    if (g_pForegroundImage) {
        delete g_pForegroundImage;
        g_pForegroundImage = nullptr;
    }

    DestroyBackgroundBlurBitmap();
    g_backgroundBlurNeedsReload.store(true, std::memory_order_release);

    g_lyricsFontCache.Clear();

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

    UnregisterClassW(kEqPopupClass, g_eqModuleHandle);
    g_eqClassesRegistered = false;
    g_eqModuleHandle = nullptr;
}

void Wh_ModAfterInit() {
    if (g_eqTaskbarSymbolsHooked)
        EqEnsureXamlButton();
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_eqTaskbarSymbolsHooked = EqHookTaskbarSymbols();
    if (!g_eqTaskbarSymbolsHooked) {
        Wh_Log(L"Wh_ModInit: taskbar XAML EQ integration hooks could not be installed; visualizer will continue without EQ button");
    }
    RegisterEqWindowClasses(GetCurrentModModuleHandle());
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

    StartAudioCapture();

    g_hOverlayThread = CreateThread(
        nullptr, 0, OverlayThreadProc, nullptr, 0, &g_overlayThreadId);

    if (!g_hOverlayThread) {
        StopAudioCapture();
        g_running.store(false, std::memory_order_release);
        CloseHandle(g_hOverlayStopEvent);
        g_hOverlayStopEvent = nullptr;
        EqUnregisterWindowClasses();
        return FALSE;
    }

    if (IsAlbumColorMode())
        StartAlbumColorCapture();
    if (g_settings.lyricsEnabled)
        StartLyricsCapture();
    return TRUE;
}

void Wh_ModUninit() {
    g_running.store(false, std::memory_order_release);
    EqCleanupIntegration();
    StopLyricsCapture();
    StopAlbumColorCapture();
    StopAudioCapture();

    if (g_hOverlayStopEvent)
        SetEvent(g_hOverlayStopEvent);

    if (g_hOverlayThread) {
        WaitForSingleObject(g_hOverlayThread, INFINITE);
        CloseHandle(g_hOverlayThread);
        g_hOverlayThread = nullptr;
    }

    g_overlayThreadId = 0;

    if (g_hOverlayStopEvent) {
        CloseHandle(g_hOverlayStopEvent);
        g_hOverlayStopEvent = nullptr;
    }

    EqUnregisterWindowClasses();
}

void Wh_ModSettingsChanged() {
    int oldAudioSource = 0;
    std::wstring oldAudioApplicationName;
    bool oldLyricsEnabled = false;
    bool oldAlbumColorMode = false;

    {
        std::unique_lock<std::shared_mutex> settingsLock(g_settingsMutex);
        oldAudioSource = g_settings.audioSource;
        oldAudioApplicationName = g_settings.audioApplicationName;
        oldLyricsEnabled = g_settings.lyricsEnabled;
        oldAlbumColorMode = IsAlbumColorMode();
        LoadSettings();
    }

    int newAudioSource = 0;
    std::wstring newAudioApplicationName;
    bool newLyricsEnabled = false;
    bool newAlbumColorMode = false;
    {
        std::shared_lock<std::shared_mutex> settingsLock(g_settingsMutex);
        newAudioSource = g_settings.audioSource;
        newAudioApplicationName = g_settings.audioApplicationName;
        newLyricsEnabled = g_settings.lyricsEnabled;
        newAlbumColorMode = IsAlbumColorMode();
    }

    if (oldAlbumColorMode != newAlbumColorMode) {
        if (newAlbumColorMode)
            StartAlbumColorCapture();
        else
            StopAlbumColorCapture();
    }

    if (oldLyricsEnabled != newLyricsEnabled) {
        if (newLyricsEnabled)
            StartLyricsCapture();
        else
            StopLyricsCapture();
    }

    const bool audioSourceChanged =
        oldAudioSource != newAudioSource ||
        oldAudioApplicationName != newAudioApplicationName;

    if (audioSourceChanged) {
        StopAudioCapture();
        StartAudioCapture();
    }

    if (g_hwndOverlay) {
        InvalidateRect(g_hwndOverlay, nullptr, TRUE);
        PostMessageW(g_hwndOverlay, WM_VIZ_AUDIO_WAKE, 0, 0);
    }
}
