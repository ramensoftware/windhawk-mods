// ==WindhawkMod==
// @id              overhaulded-alt-tab
// @name            OverhauldedWin Alt+Tab
// @description     Replaces the boring Windows Alt+Tab with a modern and elegant window switcher.
// @version         1.3.0
// @author          IMiloDev
// @github          https://github.com/IMiloDev
// @homepage        https://github.com/IMiloDev/OverhauldedWin
// @include         windhawk.exe
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Overhaulded Task Switcher
 
A modern, fluid and highly visual replacement for the native Windows Alt+Tab experience.

Built from scratch in native C++ as a project to explore Windows APIs, graphics, animation systems and desktop customization. You can also see the original [GITHUB REPOSITORY](https://github.com/IMiloDev/OverhauldedWin) to send me issues.

> **⚠️ DISCLAIMER**
>
> If the **Windhawk window is in the foreground**, the native Windows Task Switcher may appear above Overhaulded during `Alt + Tab`.
> 
> **Workaround:** Minimize or close the Windhawk window before using Overhaulded.
>
> [Read more about this limitation →](#native-windows-task-switcher)
> 

## Screenshot

![OverhauldedWin Task Switcher](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/TaskManager.jpg)

## Features

- Modern horizontal task switcher interface
- App grouping by application/process
- Real DWM window previews
- Dynamic Obsidian visual system
- Rounded cards with subtle downward shadows
- Floating task switcher surface
- Fluid opening animation
- Smooth horizontal navigation
- Interruptible navigation animations
- Subtle center-card snap animation
- Hover interactions
- Window closing
- Resolution-aware UI scaling
- High-DPI support
- Alt + Tab support
- Alt + Shift + Tab support
- AltGr + Tab support
- Lightweight native C++ implementation
- Background execution for fast activation

### Another Features

- Visual continuity between selections
- Smoothest carousel-style navigation
- Real window previews
- Application-aware grouping
- Subtle depth and lighting
- Fast activation through background execution

Overhaulded also aims to remain visually distinct from other Windows task-switcher projects while exploring its own interaction and visual language.

## Design

Overhaulded focuses on a dark, minimal interface inspired by modern desktop UI design while keeping the selector feeling native to Windows.

The visual system uses a Black Obsidian surface with:

- Rounded cards
- Subtle downward shadows
- Content-based illumination
- Soft depth separation
- Smooth transitions
- A fixed central visualizer

The central visualizer remains fixed while the cards move through the carousel, creating the impression of navigating through a physical stack of windows rather than moving the entire interface.

## Smooth Animations

### Open / Close

![OverhauldedWin Open Animation](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/Open_n'_Close.webp)

### Navigation

![OverhauldedWin](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/Desplacement.webp)

### Window Close

![OverhauldedWin](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/abort.webp)

## Background Execution

Overhaulded remains prepared in the background instead of creating the entire task-switcher interface from scratch every time Alt+Tab is pressed.

This allows the selector to appear immediately while keeping the visual opening animation smooth.

The architecture separates global input handling from the UI/rendering system so that heavy graphics and window-management operations do not run directly inside low-level keyboard hooks.

---

# IMPORTANT

Overhaulded is still a pre-release project.

Some applications may behave differently than standard desktop windows, especially those using unusual window structures, custom rendering, or multiple processes.

### Native Windows Task Switcher

When the **Windhawk window itself is in the foreground**, Windows keeps the native task switcher on top of Overhaulded when an `Alt + Tab` session is initiated.

In this situation, the native Windows task switcher may appear over the Overhaulded interface, even though Overhaulded uses a "topmost" window and correctly handles keyboard input.

This behavior is related to how the Windows Shell manages its native task switcher and window/composition priority during the `Alt + Tab` operation. Windows does not provide a reliable, documented, and guaranteed public API that allows forcing a third-party window (which does not take focus) to remain visually above the native task switcher.

Behavior may vary depending on the Windows version and system configuration. This is a limitation of the Windows Shell, not a failure to detect or handle `Alt + Tab` input.

I apologize in advance for the inconvenience. I am constantly looking for a way to fix this issue, but public documentation hasn't yielded a solution so far; while it isn't necessarily impossible, it is highly likely that this issue cannot be resolved without delving into sensitive Windows ecosystem code.

---

## Controls

| Shortcut                                     | Action                        |
| -------------------------------------------- | ----------------------------- |
| `Alt + Tab`                                  | Move to the next window       |
| `Alt gr+ Tab`                                | Move to the next window       |
| `Alt + Tab` + release `Alt`                  | Activate the selected window  |
| `Esc`                                        | Cancel the switcher           |
| `Alt gr + Tab + Arrows` `Alt + Tab + Arrows` | Additional navigation/control |

## Compatibility

Overhaulded is currently designed specifically for Windows 11.

The project is still under active development, so behavior may vary depending on:

- Display scaling
- Multiple-monitor configurations
- Windows configuration
- Application window types
- Other Alt+Tab/task-switcher modifications

Running multiple applications that replace the native Windows task switcher at the same time may cause conflicts.

## Development

Overhaulded is primarily a C++ project focused on exploring:

- Win32 APIs
- Windows hooks
- DWM
- Direct2D
- DirectWrite
- GDI
- Window management
- High-DPI rendering
- Desktop animation systems
- Native Windows UI

The project is continuously evolving as new ideas and technical improvements are explored.

## License

This project is licensed under the **MIT License**.

See the https://github.com/IMiloDev/OverhauldedWin/blob/main/LICENSE file for the complete license text.

---

> Made by Milo.

![OverhauldedWin Task Switcher](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/preview.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
    - Scale: "100"
      $name: Scale
      $description: Overall task switcher scale.
      $options:
      - "50": 50%
      - "75": 75%
      - "90": 90%
      - "100": 100%
      - "110": 110%
      - "125": 125%
    - PerformanceMode: "smooth"
      $name: Performance mode
      $description: Controls the animation update frequency.
      $options:
      - "smooth": Smooth
      - "balanced": Balanced
    - AnimationSpeed: "100"
      $name: Animation speed
      $description: Relative speed of the task switcher animations.
      $options:
      - "75": 75%
      - "100": 100%
      - "125": 125%
      - "150": 150%
    - Cards:
        - VisibleCards: "5"
          $name: Visible cards
          $description: Number of cards shown in the carousel.
          $options:
          - "3": 3 cards
          - "5": 5 cards
        - NormalColor: "#050507"
          $name: Cards color
          $description: Base color of the cards that are not selected. Default Hex - #050507. An empty or invalid value uses the default.
        - SelectedColor: "#090A0B"
          $name: Selected card color
          $description: Base color of the selected card. Default Hex - #090A0B. An empty or invalid value uses the default.
        - AnimationStyle: "Linear"
          $name: Animation style
          $description: Open and close animation. Linear is the standard animation, Bouncy opens with a pronounced bounce and RCT is a CRT-style collapse and expansion.
          $options:
          - "Linear": Linear
          - "Bouncy": Bouncy
          - "RCT": RCT
      $name: Cards
  $name: Appearance
- Background: "dwm"
  $name: Background
  $description: Background layer behind the cards.
  $options:
  - "none": None
  - "dwm": Transparent - DWM
- Dwm:
    - BackgroundColor: "#141A22"
      $name: Background color
      $description: Color of the transparent DWM background behind the cards. Default Hex - #141A22. An empty or invalid value uses the default.
    - Opacity: 20
      $name: Opacity
      $description: DWM background opacity (0-100). Default - 20.
  $name: DWM background
- ExperimentalSettings:
    - GeneralBackground: "none"
      $name: General Background
      $description: Experimental features may require additional GPU, CPU and RAM resources and may affect performance. Pattern Waves requires a background other than None.
      $options:
      - "none": None
      - "pattern": Pattern Waves
    - PatternWaves:
        - Shape: "dots"
          $name: Shape
          $description: Pattern shape.
          $options:
          - "dots": Dots
          - "squares": Squares
        - Speed: 100
          $name: Speed
          $description: Procedural wave speed (0-200).
        - WaveStrength: 65
          $name: Wave strength
          $description: Surface deformation intensity (0-200).
        - Density: 24
          $name: Density
          $description: Dot spacing (8-48).
        - Light: 120
          $name: Light
          $description: Apparent depth and illumination (0-200).
        - Contrast: 100
          $name: Contrast
          $description: Dot contrast (0-200).
        - Opacity: 100
          $name: Opacity
          $description: Pattern opacity (0-100).
        - CursorInteraction: true
          $name: Cursor interaction
          $description: Enable cursor-driven ripples.
        - CursorStrength: 100
          $name: Cursor strength
          $description: Cursor ripple intensity (0-200).
      $name: Pattern Waves
  $name: Experimental
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <utility>
#include <cwchar>
#include <cstring>

typedef DWORD (WINAPI* GetFileVersionInfoSizeWFn)(LPCWSTR, LPDWORD);
typedef BOOL (WINAPI* GetFileVersionInfoWFn)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI* VerQueryValueWFn)(LPCVOID, LPCWSTR, LPVOID*, PUINT);

// CONFIGURACIÓN

static const wchar_t kWindowClassName[] = L"OverhauldedAltTabSelector";

static const int kSelectorWidth = 1220;
static const int kSelectorHeight = 430;
static const int kMaxCarouselSlots = 5;
static int g_visibleCardCount = 5;
static int g_uiScalePercent = 100;
static int g_animationSpeedPercent = 100;

enum class BaseBackgroundMode
{
    None,
    Dwm
};

enum class ExperimentalBackgroundMode
{
    None,
    PatternWaves
};

static BaseBackgroundMode g_baseBackgroundMode = BaseBackgroundMode::Dwm;
static ExperimentalBackgroundMode g_experimentalBackgroundMode =
    ExperimentalBackgroundMode::None;

// Pattern Waves is available only as an overlay over an active base
// background. Its saved setting is preserved while Background is None, but it
// must not render or run its simulation in that state.
static bool IsPatternWavesActive()
{
    return g_baseBackgroundMode != BaseBackgroundMode::None &&
           g_experimentalBackgroundMode == ExperimentalBackgroundMode::PatternWaves;
}

// The mouse spotlight is an independent background interaction. It remains
// available even when an experimental background such as Pattern Waves is active.
static bool IsBackgroundSpotlightAllowed()
{
    return g_baseBackgroundMode == BaseBackgroundMode::Dwm;
}

static COLORREF g_dwmBackgroundColor = RGB(20, 26, 34);
static float g_dwmBackgroundOpacity = 0.20f;

// Cursor spotlight over the background only. The cards are rendered later,
// so their surfaces naturally occlude the spotlight without extra masks.
// Radio del spotlight en DIP (antes 310; el diámetro pasa al 37,5 %).
static const float kSpotlightRadiusDip = 116.0f;
static ID2D1RadialGradientBrush* g_ambientShadowBrush = nullptr;
static ID2D1RadialGradientBrush* g_backgroundSpotlightBrush = nullptr;
static COLORREF g_backgroundSpotlightBrushColor = CLR_INVALID;
static POINT g_backgroundSpotlightPoint = {};
static bool g_backgroundSpotlightActive = false;
enum class PatternShape
{
    Dots,
    Squares
};

static PatternShape g_patternShape = PatternShape::Dots;
static float g_patternSpeed = 1.0f;
static float g_patternWaveStrength = 0.65f;
static float g_patternDensity = 24.0f;
static float g_patternLight = 0.80f;
static float g_patternContrast = 1.0f;
static float g_patternOpacity = 0.55f;
static bool g_patternCursorInteraction = true;
static float g_patternCursorStrength = 1.0f;
static const int kPatternFieldWidth = 96;
static const int kPatternFieldHeight = 36;
static const int kPatternFieldCount = kPatternFieldWidth * kPatternFieldHeight;
static const UINT kPatternTimerId = 91;
// Pattern Waves tiene un presupuesto independiente del renderer principal.
static float g_patternQualityScale = 1.0f;
// La calidad del fondo no debe oscilar con la granularidad del reloj de Windows.
// Se requieren varias muestras consecutivas y existe un periodo de permanencia.
static ULONGLONG g_patternQualityCooldownUntil = 0;
static int g_patternSlowSamples = 0;
static int g_patternFastSamples = 0;

// One low-resolution height field drives every visible part of Pattern Waves:
// fixed-grid dot scale, field-derived lighting and opacity.
// Ripple simulation buffers. The procedural base surface is sampled separately;
// these buffers contain only the transient disturbance caused by the cursor.
static float g_patternRippleA[kPatternFieldCount] = {};
static float g_patternRippleB[kPatternFieldCount] = {};
static float g_patternRippleC[kPatternFieldCount] = {};
static float* g_patternRippleCurrent = g_patternRippleA;
static float* g_patternRipplePrevious = g_patternRippleB;
static float* g_patternRippleNext = g_patternRippleC;
static POINT g_patternCursor = {};
static POINT g_patternPreviousCursor = {};
static bool g_patternHasCursor = false;
static bool g_patternCursorInside = false;
static ULONGLONG g_patternLastTick = 0;
// One shared combined-field cache per rendered frame. Dots and border sample
// this cache instead of recomputing procedural trigonometry independently.
static float g_patternCombinedFieldCache[kPatternFieldCount] = {};
static const int kCounterWidth = 140;
static const int kCounterHeight = 28;
static const int kCounterTop = 378;
// Opacidad común de las superficies Black Obsidian; los textos y thumbnails no se alteran.
static const float kCardSurfaceOpacity = 0.75f;
static COLORREF g_cardNormalColor = RGB(5, 5, 7);
static COLORREF g_cardSelectedColor = RGB(9, 10, 11);
// Timer del failsafe: solo existe mientras el selector está abierto.
static const UINT_PTR kActivityTimerId = 77;
static const UINT_PTR kAnimTimerId = 88;
static const UINT_PTR kTabRepeatTimerId = 89;
static const UINT_PTR kSelectorMotionTimerId = 90;
static const float kNavigationDuration = 180.0f;
static const int kDefaultAnimationFps = 120;
enum class AnimationStyle { Linear, Bouncy, Rct };
static AnimationStyle g_animationStyle = AnimationStyle::Linear;
static int g_animationFps = kDefaultAnimationFps;

static int GetCarouselCenterSlot() { return g_visibleCardCount / 2; }
static int GetCarouselSlotCount() { return g_visibleCardCount; }
static float AnimationDuration(float baseMs)
{
    return baseMs * 100.0f / static_cast<float>(g_animationSpeedPercent);
}
static void LoadTaskSwitcherSettings()
{
    auto read = [](PCWSTR key) {
        PCWSTR value = Wh_GetStringSetting(key);
        std::wstring result(value ? value : L"");
        if (value)
            Wh_FreeStringSetting(value);
        return result;
    };

    // Numeric options are integers in the settings schema. Installs that saved
    // them while they were still declared as strings keep their stored value:
    // a string value is parsed first, otherwise the integer is read. Either
    // way the result is clamped to the valid range of the option.
    auto readNumber = [&](PCWSTR key, int low, int high) {
        std::wstring stored = read(key);
        long value = 0;
        bool parsed = false;
        if (!stored.empty())
        {
            wchar_t* end = nullptr;
            value = wcstol(stored.c_str(), &end, 10);
            parsed = end != stored.c_str();
        }
        if (!parsed)
            value = Wh_GetIntSetting(key);
        return static_cast<int>(std::max(static_cast<long>(low),
                                         std::min(static_cast<long>(high), value)));
    };

    // The three 1.2.1 settings were flat keys. They are only consulted when the
    // current key reads empty.
    std::wstring visible = read(L"Appearance.Cards.VisibleCards");
    if (visible.empty())
        visible = read(L"VisibleCards");
    g_visibleCardCount = visible == L"3" ? 3 : 5;

    std::wstring scale = read(L"Appearance.Scale");
    g_uiScalePercent = 100;
    if (!scale.empty())
    {
        wchar_t* end = nullptr;
        long parsed = wcstol(scale.c_str(), &end, 10);
        if (end != scale.c_str())
            g_uiScalePercent = static_cast<int>(std::max(50L, std::min(125L, parsed)));
    }

    std::wstring speed = read(L"Appearance.AnimationSpeed");
    if (speed.empty())
        speed = read(L"AnimationSpeed");
    g_animationSpeedPercent = (speed == L"75" ? 75 : speed == L"125" ? 125 : speed == L"150" ? 150 : 100);
    std::wstring mode = read(L"Appearance.PerformanceMode");
    if (mode.empty())
        mode = read(L"PerformanceMode");
    g_animationFps = mode == L"balanced" ? 90 : kDefaultAnimationFps;

    std::wstring style = read(L"Appearance.Cards.AnimationStyle");
    g_animationStyle = style == L"Bouncy" ? AnimationStyle::Bouncy
                     : style == L"RCT" ? AnimationStyle::Rct
                     : AnimationStyle::Linear;

    // Background is the base layer and ExperimentalSettings.GeneralBackground
    // controls the optional overlay. Any base value other than "none" uses the
    // DWM background.
    std::wstring background = read(L"Background");
    g_baseBackgroundMode = background == L"none"
        ? BaseBackgroundMode::None
        : BaseBackgroundMode::Dwm;

    std::wstring experimentalBackground = read(L"ExperimentalSettings.GeneralBackground");
    g_experimentalBackgroundMode = experimentalBackground == L"pattern"
        ? ExperimentalBackgroundMode::PatternWaves
        : ExperimentalBackgroundMode::None;

    // Accepts "#RRGGBB" or "RRGGBB" (surrounding spaces ignored). Anything else,
    // including an empty string, returns the fallback (the default color).
    auto parseColor = [](const std::wstring& raw, COLORREF fallback) {
        size_t first = raw.find_first_not_of(L" \t");
        if (first == std::wstring::npos)
            return fallback;
        size_t last = raw.find_last_not_of(L" \t");
        std::wstring value = raw.substr(first, last - first + 1);
        if (!value.empty() && value[0] == L'#')
            value.erase(0, 1);
        if (value.size() != 6)
            return fallback;
        unsigned long rgb = 0;
        for (wchar_t ch : value)
        {
            unsigned long digit;
            if (ch >= L'0' && ch <= L'9') digit = ch - L'0';
            else if (ch >= L'a' && ch <= L'f') digit = ch - L'a' + 10;
            else if (ch >= L'A' && ch <= L'F') digit = ch - L'A' + 10;
            else return fallback;
            rgb = rgb * 16 + digit;
        }
        return RGB((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
    };

    g_cardNormalColor = parseColor(read(L"Appearance.Cards.NormalColor"), RGB(5, 5, 7));
    g_cardSelectedColor = parseColor(read(L"Appearance.Cards.SelectedColor"), RGB(9, 10, 11));
    g_dwmBackgroundColor = parseColor(read(L"Dwm.BackgroundColor"), RGB(20, 26, 34));
    g_dwmBackgroundOpacity =
        static_cast<float>(readNumber(L"Dwm.Opacity", 0, 100)) / 100.0f;

    g_patternShape = read(L"ExperimentalSettings.PatternWaves.Shape") == L"squares"
        ? PatternShape::Squares
        : PatternShape::Dots;

    // Free 0-200 control with recalibrated scale: the 100 value reproduces the
    // previous internal 200 behavior exactly.
    g_patternSpeed = static_cast<float>(
        readNumber(L"ExperimentalSettings.PatternWaves.Speed", 0, 200)) * 2.0f / 100.0f;
    g_patternWaveStrength = static_cast<float>(
        readNumber(L"ExperimentalSettings.PatternWaves.WaveStrength", 0, 200)) / 100.0f;
    g_patternDensity = static_cast<float>(
        readNumber(L"ExperimentalSettings.PatternWaves.Density", 8, 48));
    g_patternLight = static_cast<float>(
        readNumber(L"ExperimentalSettings.PatternWaves.Light", 0, 200)) / 100.0f;
    g_patternContrast = static_cast<float>(
        readNumber(L"ExperimentalSettings.PatternWaves.Contrast", 0, 200)) / 100.0f;
    g_patternOpacity = static_cast<float>(
        readNumber(L"ExperimentalSettings.PatternWaves.Opacity", 0, 100)) / 100.0f;
    g_patternCursorInteraction =
        Wh_GetIntSetting(L"ExperimentalSettings.PatternWaves.CursorInteraction") != 0;
    g_patternCursorStrength = static_cast<float>(
        readNumber(L"ExperimentalSettings.PatternWaves.CursorStrength", 0, 200)) / 100.0f;
}

static UINT GetPatternTimerInterval()
{
    // El efecto experimental no comparte la frecuencia de navegación/cards.
    // 30 Hz es suficiente para un fondo y deja margen a DWM/input en equipos antiguos.
    return g_patternQualityScale < 0.72f ? 50u : 33u;
}

// RELOJ DE ANIMACIÓN Y CURVAS
// Todas las animaciones se evalúan contra el mismo instante (g_frameTimeMs) por
// frame. Es el instante previsto de presentación del frame (ver RunFrame), no
// el momento en que el código llega a ejecutarse, de modo que el jitter del
// timer no afecta a la velocidad percibida. QPC en lugar de GetTickCount64
// (granularidad de ~15.6 ms).
static LONGLONG g_qpcFrequency = 0;
static double QpcNowMs()
{
    if (g_qpcFrequency == 0)
    {
        LARGE_INTEGER frequency = {};
        QueryPerformanceFrequency(&frequency);
        g_qpcFrequency = frequency.QuadPart > 0 ? frequency.QuadPart : 1;
    }
    LARGE_INTEGER counter = {};
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) * 1000.0 /
           static_cast<double>(g_qpcFrequency);
}

static double g_frameTimeMs = 0.0;
static bool g_inFrame = false;
static double FrameClockNow()
{
    return g_inFrame ? g_frameTimeMs : QpcNowMs();
}

static float Clamp01(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static float EaseSmooth(float t)
{
    t = Clamp01(t);
    return t * t * (3.0f - 2.0f * t);
}
static float EaseOutCubic(float t)
{
    float u = 1.0f - Clamp01(t);
    return 1.0f - u * u * u;
}
// Salida exponencial normalizada: velocidad inicial máxima (respuesta inmediata),
// llega exactamente a 1.0 en t=1 y no tiene overshoot. k controla la agresividad.
static float EaseOutExp(float t, float k)
{
    t = Clamp01(t);
    const float tail = expf(-k);
    return 1.0f - (expf(-k * t) - tail) / (1.0f - tail);
}
static float ElapsedFraction(double startMs, float durationMs)
{
    double elapsed = FrameClockNow() - startMs;
    if (elapsed < 0.0)
        elapsed = 0.0;
    return durationMs > 0.0f
        ? Clamp01(static_cast<float>(elapsed / durationMs)) : 1.0f;
}

static const UINT kTabRepeatInitialDelayMs = 325;
static const UINT kTabRepeatIntervalMs = 125;
static const ULONGLONG kSelectorFailsafeMs = 5000;
static const float kReferenceScreenWidth = 1366.0f;
static const float kReferenceScreenHeight = 768.0f;
static const float kMinimumUiScale = 0.55f;

struct UIScaleState
{
    float value = 1.0f;
    float dpiScale = 1.0f;
    int screenWidth = 1366;
    int screenHeight = 768;
};
static UIScaleState g_uiScale;
static int g_runtimeSelectorWidth = kSelectorWidth;
static int g_runtimeSelectorHeight = kSelectorHeight;
static float g_sceneScaleX = 1.0f;
static float g_sceneScaleY = 1.0f;
static float g_sceneOpacity = 1.0f;
// Progreso común de la escena: 0 = oculta/colapsada, 1 = completamente abierta.
// Escala, opacidad y apertura del carrusel se derivan de este único valor.
static float g_scenePresence = 1.0f;
static float g_closeStartPresence = 1.0f;
static float g_carouselSpread = 1.0f;
static float g_selectionTiltDirection = 0.0f;
static float g_cardSnapScale = 1.0f;

static int ScaleLayoutPx(float value)
{
    return static_cast<int>(roundf(value * g_uiScale.value));
}

// Constantes fijas en píxeles (bordes, radios pequeños) que deben crecer con el
// DPI del monitor pero no con el ajuste "fit" por resolución. A 100% DPI es 1:1.
static float DpiPx(float value)
{
    return value * g_uiScale.dpiScale;
}

// El UI thread es Per-Monitor-V2: todo el layout está en píxeles físicos. La
// escala final combina el DPI real del monitor con el ajuste por área de trabajo
// (medida en DIPs), de modo que a 100% el resultado es idéntico al anterior.
static void UpdateUIScaleForWorkArea(const RECT& work, float dpiScale)
{
    dpiScale = std::max(0.5f, std::min(8.0f, dpiScale));
    g_uiScale.screenWidth = std::max(1, static_cast<int>(work.right - work.left));
    g_uiScale.screenHeight = std::max(1, static_cast<int>(work.bottom - work.top));
    g_uiScale.dpiScale = dpiScale;
    float dipWidth = static_cast<float>(g_uiScale.screenWidth) / dpiScale;
    float dipHeight = static_cast<float>(g_uiScale.screenHeight) / dpiScale;
    float scaleX = dipWidth / kReferenceScreenWidth;
    float scaleY = dipHeight / kReferenceScreenHeight;
    float fit = std::max(kMinimumUiScale, std::min(1.0f, std::min(scaleX, scaleY)));
    const float userScale = static_cast<float>(g_uiScalePercent) / 100.0f;
    g_uiScale.value = fit * dpiScale * userScale;
    g_runtimeSelectorWidth = ScaleLayoutPx(static_cast<float>(kSelectorWidth));
    g_runtimeSelectorHeight = ScaleLayoutPx(static_cast<float>(kSelectorHeight));
}

static void GetSceneScale(float& scaleX, float& scaleY)
{
    scaleX = g_sceneScaleX;
    scaleY = g_sceneScaleY;
}

static D2D1::Matrix3x2F GetSceneTransform()
{
    float sx = 1.0f, sy = 1.0f;
    GetSceneScale(sx, sy);
    D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Scale(
        D2D1::SizeF(sx, sy),
        D2D1::Point2F(static_cast<float>(g_runtimeSelectorWidth) * 0.5f,
                      static_cast<float>(g_runtimeSelectorHeight) * 0.5f));
    return transform;
}

static RECT TransformSceneRect(const RECT& source)
{
    float sx = 1.0f, sy = 1.0f;
    GetSceneScale(sx, sy);
    float cx = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
    float cy = static_cast<float>(g_runtimeSelectorHeight) * 0.5f;
    RECT result = {};
    result.left = static_cast<int>(roundf(cx + (source.left - cx) * sx));
    result.right = static_cast<int>(roundf(cx + (source.right - cx) * sx));
    result.top = static_cast<int>(roundf(cy + (source.top - cy) * sy));
    result.bottom = static_cast<int>(roundf(cy + (source.bottom - cy) * sy));
    return result;
}

typedef HANDLE HTHUMBNAIL;

// Diseño de DWM_TIMING_INFO (dwmapi.h). Solo se leen qpcRefreshPeriod y
// qpcVBlank; si el diseño no coincidiese, DwmGetCompositionTimingInfo falla y
// se usa el pacing de intervalo fijo.
struct DwmUnsignedRatioLocal
{
    UINT32 numerator;
    UINT32 denominator;
};
struct DwmTimingInfoLocal
{
    UINT32 cbSize;
    DwmUnsignedRatioLocal rateRefresh;
    ULONGLONG qpcRefreshPeriod;
    DwmUnsignedRatioLocal rateCompose;
    ULONGLONG qpcVBlank;
    ULONGLONG cRefresh;
    UINT cDXRefresh;
    ULONGLONG qpcCompose;
    ULONGLONG cFrame;
    UINT cDXPresent;
    ULONGLONG cRefreshFrame;
    ULONGLONG cFrameSubmitted;
    UINT cDXPresentSubmitted;
    ULONGLONG cFrameConfirmed;
    UINT cDXPresentConfirmed;
    ULONGLONG cRefreshConfirmed;
    UINT cDXRefreshConfirmed;
    ULONGLONG cFramesLate;
    UINT cFramesOutstanding;
    ULONGLONG cFrameDisplayed;
    ULONGLONG qpcFrameDisplayed;
    ULONGLONG cRefreshFrameDisplayed;
    ULONGLONG cFrameComplete;
    ULONGLONG qpcFrameComplete;
    ULONGLONG cFramePending;
    ULONGLONG qpcFramePending;
    ULONGLONG cFramesDisplayed;
    ULONGLONG cFramesComplete;
    ULONGLONG cFramesPending;
    ULONGLONG cFramesAvailable;
    ULONGLONG cFramesDropped;
    ULONGLONG cFramesMissed;
    ULONGLONG cRefreshNextDisplayed;
    ULONGLONG cRefreshNextPresented;
    ULONGLONG cRefreshesDisplayed;
    ULONGLONG cRefreshesPresented;
    ULONGLONG cRefreshStarted;
    ULONGLONG cPixelsReceived;
    ULONGLONG cPixelsDrawn;
    ULONGLONG cBuffersEmpty;
};
typedef HRESULT (WINAPI* DwmGetCompositionTimingInfoFn)(HWND, DwmTimingInfoLocal*);

struct DwmThumbnailPropertiesLocal
{
    DWORD dwFlags;
    RECT rcDestination;
    RECT rcSource;
    BYTE opacity;
    BOOL fVisible;
    BOOL fSourceClientAreaOnly;
};

typedef HRESULT (WINAPI* DwmRegisterThumbnailFn)(HWND, HWND, HTHUMBNAIL*);
typedef HRESULT (WINAPI* DwmUnregisterThumbnailFn)(HTHUMBNAIL);
typedef HRESULT (WINAPI* DwmUpdateThumbnailPropertiesFn)(
    HTHUMBNAIL, const DwmThumbnailPropertiesLocal*);
typedef HRESULT (WINAPI* DwmSetWindowAttributeFn)(HWND, DWORD,
                                                   const void*, DWORD);
typedef HRESULT (WINAPI* DwmGetWindowAttributeFn)(HWND, DWORD,
                                                   void*, DWORD);

struct MARGINS
{
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
};

// Atributos y flags DWM usados por el glass y las miniaturas.
static const DWORD kDwmWindowCornerPreference = 33;
static const DWORD kDwmBorderColor = 34;
static const DWORD kDwmColorNone = 0xFFFFFFFE;
static const DWORD kDwmCornerPreferenceDoNotRound = 1;
static const DWORD kDwmNcRenderingPolicy = 2;
static const DWORD kDwmNcRenderingPolicyUseWindowStyle = 0;
static const DWORD kDwmUseImmersiveDarkMode = 20;
static const DWORD kDwmWaSystemBackdropType = 38;
static const DWORD kDwmSbtNone = 1;

static const DWORD kDwmTnpRectDestination = 0x00000001;
static const DWORD kDwmTnpRectSource = 0x00000002;
static const DWORD kDwmTnpOpacity = 0x00000004;
static const DWORD kDwmTnpVisible = 0x00000008;

struct CardStyleConfig
{
    bool glowEnabled = true;
    int glowIntensity = 1;
    COLORREF borderColor = RGB(10, 11, 13);
    COLORREF selectedBorderColor = RGB(20, 21, 23);
    COLORREF cardBackground = RGB(5, 6, 7);
    COLORREF selectedCardBackground = RGB(10, 10, 11);
    int cornerRadius = 12;
    int selectedCornerRadius = 14;
};

static CardStyleConfig g_cardStyle;

// ESTADO

enum class SelectorState
{
    Idle,
    SelectorActive,
    Confirming,
    Canceling
};
enum class SelectorAnimationState
{
    None,
    Opening,
    SelectionChange,
    CardExit,
    Open,
    Closing
};

enum class ModifierSession
{
    None,
    LeftAlt,
    RightAlt,
    AltGr
};

struct MediaAccent
{
    COLORREF color = RGB(0, 0, 0);
    float strength = 0.0f;
    bool valid = false;
};

struct AppGroup
{
    DWORD processId;
    std::wstring appName;
    std::wstring processPath;
    std::vector<HWND> windows;
    HWND representativeWindow;
    HICON icon;
    ID2D1Bitmap* iconBitmap;
    ID2D1LinearGradientBrush* surfaceBrushNormal;
    ID2D1LinearGradientBrush* surfaceBrushSelected;
    bool ownIcon;
    MediaAccent mediaAccent;
};

struct UserWindowInfo
{
    HWND hwnd;
    DWORD processId;
    std::wstring processPath;
    std::wstring appName;
    ULONGLONG lastActivated;
};

// ARQUITECTURA DE HILOS
//
//  Input thread : dueño de WH_KEYBOARD_LL y WH_MOUSE_LL. Los callbacks solo
//                 actualizan estado atómico/local y publican comandos al UI
//                 thread con PostThreadMessage. Nada más.
//  UI thread    : dueño del HWND del selector, D2D/DWrite/GDI/DWM,
//                 animaciones, timers, WinEvent hook, enumeración de ventanas,
//                 iconos, información de procesos y activación de ventanas.
//
// Variables de un solo dueño: las "input" solo las toca el input thread y las
// "ui" solo las toca el UI thread. Lo compartido usa Interlocked*.

static HMODULE g_hModule = nullptr;

// Workers (creados/cerrados por WhTool_ModInit / WhTool_ModUninit).
static HANDLE g_inputThread = nullptr;
static HANDLE g_uiThread = nullptr;
static HANDLE g_inputReadyEvent = nullptr;
static HANDLE g_uiReadyEvent = nullptr;
static DWORD g_inputThreadId = 0;
static volatile DWORD g_uiThreadId = 0;
static volatile LONG g_inputInitOk = 0;
static volatile LONG g_uiInitOk = 0;
static volatile LONG g_shutdownRequested = 0;

// Solo input thread.
static HHOOK g_keyboardHook = nullptr;
static HHOOK g_mouseHook = nullptr;
static HWND volatile g_inputControlWnd = nullptr;   // ventana message-only

// Estado compartido entre hooks y UI.
static volatile LONG g_nextSessionId = 0;
// Id de la sesión de selector actualmente activa (0 = ninguna). Lo publica el
// input thread al abrir; el UI thread solo lo limpia con CAS sobre su propio id.
static volatile LONG g_sessionActive = 0;
// 1 mientras el selector acepta interacción de ratón (el UI thread lo gobierna).
static volatile LONG g_uiSelectorOpen = 0;
static volatile LONG64 g_lastMousePt = 0;
static volatile LONG g_mouseMovePending = 0;

// Comandos input -> UI (PostThreadMessage) y UI -> input (ventana de control).
static const UINT WM_UI_OPEN = WM_APP + 1;         // wParam=sessionId, lParam=shift
static const UINT WM_UI_TAB_DOWN = WM_APP + 2;     // wParam=sessionId, lParam=shift
static const UINT WM_UI_TAB_UP = WM_APP + 3;       // wParam=sessionId
static const UINT WM_UI_NAVIGATE = WM_APP + 4;     // wParam=sessionId, lParam=+1/-1
static const UINT WM_UI_CONFIRM = WM_APP + 5;      // wParam=sessionId
static const UINT WM_UI_CANCEL = WM_APP + 6;       // wParam=sessionId
static const UINT WM_UI_MOUSE_MOVE = WM_APP + 7;   // posición en g_lastMousePt
static const UINT WM_UI_MOUSE_BUTTON = WM_APP + 8; // wParam=mensaje, lParam=pt (16 bit)
static const UINT WM_UI_MOUSE_WHEEL = WM_APP + 9;  // wParam=delta (short)
static const UINT WM_UI_SETTINGS = WM_APP + 10;
static const UINT WM_IN_MOUSEHOOK = WM_APP + 20;   // wParam=1 instalar, 0 quitar
static const UINT WM_IN_ACTIVATE = WM_APP + 21;     // wParam=target HWND

// Solo UI thread.
static HWND g_selector = nullptr;
// El contenido D2D y la superficie visual viven en g_selector.
static bool g_classesRegistered = false;
static LONG g_uiSessionId = 0;
static bool g_uiTabDown = false;
static HWINEVENTHOOK g_winEventHook = nullptr;
static HWND g_foregroundRetryWindow = nullptr;
static UINT_PTR g_foregroundRetryTimer = 0;
static HMODULE g_shcoreApi = nullptr;
typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
static GetDpiForMonitorFn g_getDpiForMonitor = nullptr;

// Colección visual estable: una ranura = un thumbnail DWM reutilizado.
struct ThumbnailSlot
{
    HTHUMBNAIL thumbnail;
    HWND source;
    int sourceWidth = 0;
    int sourceHeight = 0;
    RECT lastDestination = {};
    RECT lastSource = {};
    BYTE lastOpacity = 0;
    BOOL lastVisible = FALSE;
    bool hasProperties = false;
};

static ThumbnailSlot g_thumbnailSlots[kMaxCarouselSlots] = {};
static HMODULE g_dwmApi = nullptr;
static DwmRegisterThumbnailFn g_dwmRegisterThumbnail = nullptr;
static DwmUnregisterThumbnailFn g_dwmUnregisterThumbnail = nullptr;
static DwmUpdateThumbnailPropertiesFn g_dwmUpdateThumbnailProperties = nullptr;
static DwmSetWindowAttributeFn g_dwmSetWindowAttribute = nullptr;
static DwmGetWindowAttributeFn g_dwmGetWindowAttribute = nullptr;
static DwmGetCompositionTimingInfoFn g_dwmGetCompositionTimingInfo = nullptr;
static bool g_dwmTimingResolved = false;
static bool g_dwmGlassEnabled = false;

// Selección lógica vs. posición animada. La colección de AppGroups no se toca.
static float g_animOffset = 0.0f;
static float g_animStartOffset = 0.0f;
static double g_animStartTime = 0.0;
static bool g_animActive = false;

// Hover es exclusivamente visual y nunca sustituye a g_selected.
static int g_hoveredSlot = -1;
static bool g_hoveredCloseButton = false;
static bool g_tabRepeatStarted = false;
static bool g_cleanupInProgress = false;
static bool g_selectorOpening = false;
static ULONGLONG g_lastSelectorActivity = 0;
static SelectorAnimationState g_selectorAnimation = SelectorAnimationState::None;
static double g_selectorAnimationStart = 0.0;
static HWND g_pendingActivationTarget = nullptr;
static HWND g_selectorOriginWindow = nullptr;
static bool g_cardExitActive = false;
static int g_cardExitSlot = -1;
static int g_cardExitGroupIndex = -1;
static HWND g_cardExitTarget = nullptr;
static float g_cardExitDirection = 1.0f;
static double g_cardExitStart = 0.0;
static float g_cardExitProgress = 0.0f;

// DIRECT2D Y DIRECTWRITE

typedef HRESULT (WINAPI* D2D1CreateFactoryFn)(
    D2D1_FACTORY_TYPE factoryType,
    REFIID riid,
    const D2D1_FACTORY_OPTIONS* pFactoryOptions,
    void** ppIFactory
);

typedef HRESULT (WINAPI* DWriteCreateFactoryFn)(
    DWRITE_FACTORY_TYPE factoryType,
    REFIID iid,
    IUnknown** factory
);

static const IID kIidD2D1Factory = {
    0x06152247, 0x6f50, 0x465a, { 0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07 }
};

static const IID kIidDWriteFactory = {
    0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 }
};

static HMODULE g_d2dApi = nullptr;
static HMODULE g_dwriteApi = nullptr;

static ID2D1Factory* g_d2dFactory = nullptr;
static ID2D1DCRenderTarget* g_d2dDCRenderTarget = nullptr;
static ID2D1SolidColorBrush* g_d2dBrush = nullptr;
static ID2D1RadialGradientBrush* g_closeAccentBrush = nullptr;
static ID2D1PathGeometry* g_closeButtonGeometry = nullptr;
// Capas y máscara reutilizadas entre frames (antes se creaban por frame).
static ID2D1Layer* g_sceneLayer = nullptr;
static ID2D1Layer* g_cardExitLayer = nullptr;
static ID2D1Layer* g_closeLayer = nullptr;
static ID2D1RoundedRectangleGeometry* g_cardMaskGeometry = nullptr;
static D2D1_ROUNDED_RECT g_cardMaskKey = {};
static RECT g_closeButtonGeometryRect = {};

static IDWriteFactory* g_dwriteFactory = nullptr;
static IDWriteTextFormat* g_dwriteSelectedFormat = nullptr;
static IDWriteTextFormat* g_dwriteNormalFormat = nullptr;
static IDWriteTextFormat* g_dwriteCounterFormat = nullptr;
static IDWriteTextFormat* g_dwriteCloseFormat = nullptr;
// Escala con la que se construyeron los TextFormat / fuentes GDI actuales.
static float g_dwriteFormatsScale = 0.0f;
static float g_gdiFontsScale = 0.0f;

typedef HFONT (WINAPI* CreateFontIndirectWFn)(const LOGFONTW*);
typedef BOOL (WINAPI* DeleteObjectFn)(HGDIOBJ);
typedef HRGN (WINAPI* CreateRoundRectRgnFn)(int, int, int, int, int, int);
typedef HGDIOBJ (WINAPI* GetStockObjectFn)(int);
typedef int (WINAPI* SetBkModeFn)(HDC, int);
typedef COLORREF (WINAPI* SetTextColorFn)(HDC, COLORREF);
typedef HBRUSH (WINAPI* CreateSolidBrushFn)(COLORREF);
typedef HPEN (WINAPI* CreatePenFn)(int, int, COLORREF);
typedef HGDIOBJ (WINAPI* SelectObjectFn)(HDC, HGDIOBJ);
typedef BOOL (WINAPI* RoundRectFn)(HDC, int, int, int, int, int, int);
typedef int (WINAPI* GetTextFaceWFn)(HDC, int, LPWSTR);
typedef int (WINAPI* GetDIBitsFn)(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);
typedef int (WINAPI* GetObjectWFn)(HGDIOBJ, int, LPVOID);

static HMODULE g_gdiApi = nullptr;
static CreateFontIndirectWFn g_createFontIndirectW = nullptr;
static DeleteObjectFn g_deleteObject = nullptr;
static CreateRoundRectRgnFn g_createRoundRectRgn = nullptr;
static GetStockObjectFn g_getStockObject = nullptr;
static SetBkModeFn g_setBkMode = nullptr;
static SetTextColorFn g_setTextColor = nullptr;
static CreateSolidBrushFn g_createSolidBrush = nullptr;
static CreatePenFn g_createPen = nullptr;
static SelectObjectFn g_selectObject = nullptr;
static RoundRectFn g_roundRect = nullptr;
static GetTextFaceWFn g_getTextFaceW = nullptr;
static GetDIBitsFn g_getDIBits = nullptr;
static GetObjectWFn g_getObjectW = nullptr;

static HFONT g_nameFont = nullptr;
static HFONT g_selectedNameFont = nullptr;
static HFONT g_secondaryFont = nullptr;

static std::vector<AppGroup> g_groups;
static std::vector<UserWindowInfo> g_userWindows;
// UI thread only. A window remains here after WM_CLOSE is posted until it
// actually disappears, so a synchronous refresh cannot re-add its card.
static std::vector<HWND> g_pendingCloseWindows;
static std::vector<std::pair<DWORD, size_t>> g_groupWindowCursors;
static int g_selected = 0;
static SelectorState g_state = SelectorState::Idle;


static bool LoadGdiFunctions();
static bool LoadDwmFunctions();
static HICON GetApplicationIcon(HWND hwnd, const std::wstring& processPath, bool* outOwnIcon);
static void ReleaseGroupResources();
static void CancelSelection();
static void ConfirmSelection();
static void CleanupKeyboardState();
static void EmergencyCloseSelector();
static void StartSelectorClose(HWND target);
static void UpdateSelectorMotion();
static void UpdateCarouselAnimation(HWND hwnd);
static float GetCardExitProgress();
static float GetCardExitOpacity();
static float GetCardExitOffsetY();
static void DestroySelector();
static bool InitializePersistentSelector();
static void HideSelectorForSession();
static void ActivateWindow(HWND target);
static HWND GetNextGroupWindow(AppGroup& group);
static void StartSlide(int steps);
static void SelectNextSmooth(int steps = 1);
static void SelectPreviousSmooth(int steps = 1);
static void SelectNext();
static void SelectPrevious();
static void StopCarouselAnimation();
static void UpdateSelectorControls();
static void UpdateThumbnailSlots();
static void RequestFrame();
static void ArmFramePump();
static void DisarmFramePump();
static void RunFrame();
static void ReleaseD2DDeviceResources();
static void ApplyScenePresence(float presence);
struct CardSlotGeometry
{
    float left;
    float top;
    float right;
    float bottom;
    float cornerRadius;
    float headerHeight;
    float padding;
};

static CardSlotGeometry GetCanonicalSlotKeyframe(int slot)
{
    CardSlotGeometry g = {};
    if (slot <= -1)
    {
        g.left = -115.0f;
        g.top = 160.0f;
        g.right = -5.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 10.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else if (slot == 0)
    {
        g.left = 15.0f;
        g.top = 160.0f;
        g.right = 125.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else if (slot == 1)
    {
        g.left = 135.0f;
        g.top = 125.0f;
        g.right = 375.0f;
        g.bottom = 315.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 28.0f;
        g.padding = 8.0f;
    }
    else if (slot == 2)
    {
        g.left = 390.0f;
        g.top = 55.0f;
        g.right = 830.0f;
        g.bottom = 355.0f;
        g.cornerRadius = 14.0f;
        g.headerHeight = 34.0f;
        g.padding = 10.0f;
    }
    else if (slot == 3)
    {
        g.left = 845.0f;
        g.top = 125.0f;
        g.right = 1085.0f;
        g.bottom = 315.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 28.0f;
        g.padding = 8.0f;
    }
    else if (slot == 4)
    {
        g.left = 1095.0f;
        g.top = 160.0f;
        g.right = 1205.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else
    {
        g.left = 1225.0f;
        g.top = 160.0f;
        g.right = 1335.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 10.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    return g;
}

static CardSlotGeometry GetSlotKeyframe(int slot)
{
    CardSlotGeometry g = {};
    if (g_visibleCardCount == 5)
    {
        // Keep the original five-card trajectory, including its off-screen
        // keyframes used while a navigation animation is in progress.
        g = GetCanonicalSlotKeyframe(slot);
    }
    else
    {
        // The three-card layout reuses the center and adjacent keyframes.
        const int relative = slot - GetCarouselCenterSlot();
        const int canonical = std::max(-1, std::min(1, relative)) + 2;
        g = GetCanonicalSlotKeyframe(canonical);
    }
    return g;
}

static inline float LerpFloat(float a, float b, float t)
{
    return a + (b - a) * t;
}


static CardSlotGeometry GetInterpolatedSlotGeometry(float virtualSlot)
{
    int k0 = static_cast<int>(floorf(virtualSlot));
    int k1 = k0 + 1;
    float f = virtualSlot - static_cast<float>(k0);

    CardSlotGeometry g0 = GetSlotKeyframe(k0);
    CardSlotGeometry g1 = GetSlotKeyframe(k1);

    CardSlotGeometry res = {};
    res.left = LerpFloat(g0.left, g1.left, f);
    res.top = LerpFloat(g0.top, g1.top, f);
    res.right = LerpFloat(g0.right, g1.right, f);
    res.bottom = LerpFloat(g0.bottom, g1.bottom, f);
    res.cornerRadius = LerpFloat(g0.cornerRadius, g1.cornerRadius, f);
    res.headerHeight = LerpFloat(g0.headerHeight, g1.headerHeight, f);
    res.padding = LerpFloat(g0.padding, g1.padding, f);
    res.left *= g_uiScale.value;
    res.top *= g_uiScale.value;
    res.right *= g_uiScale.value;
    res.bottom *= g_uiScale.value;
    res.cornerRadius *= g_uiScale.value;
    res.headerHeight *= g_uiScale.value;
    res.padding *= g_uiScale.value;

    if (g_carouselSpread != 1.0f)
    {
        // Apertura/cierre: las cards se abren desde el centro de la superficie
        // con el mismo progreso que escala el fondo.
        const float spreadCenterX = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
        const float shift = ((res.left + res.right) * 0.5f - spreadCenterX) *
                            (g_carouselSpread - 1.0f);
        res.left += shift;
        res.right += shift;
    }

    return res;
}

static RECT GetSlotCardRect(int slot);
static RECT GetSlotHeaderRect(int slot);
static RECT GetSlotPreviewRect(int slot);
static RECT GetCounterRect();
static RECT GetCloseButtonRect(int slot);
static RECT GetCloseHoverButtonRect(int slot);
static ID2D1PathGeometry* CreateCloseButtonGeometry(const RECT& rect);
static int GetSlotAtPoint(POINT ptClient);
static int ResolveGroupIndex(int slot);
static void UpdateHoveredSlot(POINT ptClient);
static void CloseWindowForSlot(int slot);
static ID2D1RadialGradientBrush* EnsureBackgroundSpotlightBrush()
{
    if (!g_d2dDCRenderTarget)
        return nullptr;

    if (g_backgroundSpotlightBrush &&
        g_backgroundSpotlightBrushColor == g_dwmBackgroundColor)
        return g_backgroundSpotlightBrush;

    if (g_backgroundSpotlightBrush)
    {
        g_backgroundSpotlightBrush->Release();
        g_backgroundSpotlightBrush = nullptr;
    }

    const float r = GetRValue(g_dwmBackgroundColor) / 255.0f;
    const float g = GetGValue(g_dwmBackgroundColor) / 255.0f;
    const float b = GetBValue(g_dwmBackgroundColor) / 255.0f;

    const float brightR = std::min(1.0f, r * 1.65f + 0.035f);
    const float brightG = std::min(1.0f, g * 1.65f + 0.035f);
    const float brightB = std::min(1.0f, b * 1.65f + 0.035f);

    D2D1_GRADIENT_STOP stops[3] = {};
    stops[0].position = 0.0f;
    stops[0].color = D2D1::ColorF(brightR, brightG, brightB, 0.34f);
    stops[1].position = 0.42f;
    stops[1].color = D2D1::ColorF(brightR, brightG, brightB, 0.13f);
    stops[2].position = 1.0f;
    stops[2].color = D2D1::ColorF(brightR, brightG, brightB, 0.0f);

    ID2D1GradientStopCollection* collection = nullptr;
    if (FAILED(g_d2dDCRenderTarget->CreateGradientStopCollection(
            stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection)) ||
        !collection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};
    props.center = D2D1::Point2F(0.0f, 0.0f);
    props.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    props.radiusX = DpiPx(kSpotlightRadiusDip);
    props.radiusY = DpiPx(kSpotlightRadiusDip);

    HRESULT hr = g_d2dDCRenderTarget->CreateRadialGradientBrush(
        &props, nullptr, collection, &g_backgroundSpotlightBrush);
    collection->Release();
    if (FAILED(hr) || !g_backgroundSpotlightBrush)
        return nullptr;

    g_backgroundSpotlightBrushColor = g_dwmBackgroundColor;
    return g_backgroundSpotlightBrush;
}

static ID2D1RadialGradientBrush* EnsureCloseAccentBrush();
static ID2D1RadialGradientBrush* EnsureBackgroundSpotlightBrush();
static void PaintSelectorScene(HWND hwnd, HDC hdc);
static void UpdateSelectorSurfaceTransform();
static void ApplySelectorRoundedRegion(HWND hwnd);
static void ResetPatternWaves();
static void UpdatePatternWaves();
static void DrawPatternShape(float x, float y, float radius)
{
    const float drawX = x;
    const float drawY = y;
    const float rx = std::max(0.5f, radius);
    const float ry = rx;

    if (g_patternShape == PatternShape::Squares)
    {
        D2D1_RECT_F square = D2D1::RectF(
            drawX - rx, drawY - ry, drawX + rx, drawY + ry);
        g_d2dDCRenderTarget->FillRectangle(&square, g_d2dBrush);
        return;
    }

    g_d2dDCRenderTarget->FillEllipse(
        D2D1::Ellipse(D2D1::Point2F(drawX, drawY), rx, ry),
        g_d2dBrush);
}

static void DrawPatternWaves(const RECT& clientRect);
static bool IsPointInsidePatternArea(POINT point);
static void PaintBottomShadowD2D(const RECT& objectRect, float radius,
                                 float strength);
static MediaAccent ExtractMediaAccentFromImage(HICON icon);
static D2D1_COLOR_F AddMediaAccent(const D2D1_COLOR_F& base,
                                   const MediaAccent& accent, float influence);
static void UiEndSession();
static void LeaveActiveState(SelectorState newState);

// ESTADO DEL TECLADO (solo input thread)

static ModifierSession g_sessionModifier = ModifierSession::None;
static LONG g_inputSessionId = 0;
static bool g_altLeftDown = false;
static bool g_altRightDown = false;
static bool g_ctrlLeftDown = false;
static bool g_ctrlRightDown = false;
static bool g_tabDown = false;
static bool g_altGrActive = false;
static bool g_tabSuppressed = false;
// Se arma únicamente cuando AltGr+Tab pertenece a nuestro selector; permite
// bloquear Ctrl+Alt+Flecha antes de que el selector termine de mostrarse.
static bool g_altGrTaskSwitcherArmed = false;
static bool g_altMenuMaskPending = false;

// UTILIDADES DE ESTADO

// Los hooks nunca llaman a las APIs de ventana desde aquí: solo publican.
static bool PostUiCommand(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return false;
    DWORD threadId = g_uiThreadId;
    if (!threadId)
        return false;
    return PostThreadMessageW(threadId, message, wParam, lParam) != FALSE;
}

static void PostInputControl(UINT message, WPARAM wParam)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;
    HWND control = g_inputControlWnd;
    if (control)
        PostMessageW(control, message, wParam, 0);
}

// El UI thread activa/desactiva la interacción de ratón; el input thread
// instala el WH_MOUSE_LL solo mientras el selector acepta interacción.
static void UiSetInteractive(bool on)
{
    LONG next = on ? 1 : 0;
    LONG previous = InterlockedExchange(&g_uiSelectorOpen, next);
    if (previous != next)
        PostInputControl(WM_IN_MOUSEHOOK, static_cast<WPARAM>(next));
}

// Termina la sesión desde el lado UI. CAS: nunca pisa una sesión más nueva.
static void UiEndSession()
{
    UiSetInteractive(false);
    LONG id = g_uiSessionId;
    if (id != 0)
        InterlockedCompareExchange(&g_sessionActive, 0, id);
}

static void LeaveActiveState(SelectorState newState)
{
    g_state = newState;
    UiEndSession();
}

static bool IsSelectorActuallyActive()
{
    return g_state == SelectorState::SelectorActive &&
           g_selector != nullptr && IsWindow(g_selector) != FALSE;
}

static bool IsSelectorActive()
{
    return IsSelectorActuallyActive();
}

static void MarkSelectorActivity()
{
    g_lastSelectorActivity = GetTickCount64();
}

// Estado del teclado del input thread. Se sincroniza de GetAsyncKeyState solo al
// iniciar; después se mantiene únicamente con los eventos del hook.
static void InputResetKeyboardState()
{
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    g_tabDown = false;
    g_altGrActive = false;
    g_tabSuppressed = false;
    g_altGrTaskSwitcherArmed = false;
    g_altMenuMaskPending = false;
    g_sessionModifier = ModifierSession::None;
    g_inputSessionId = 0;
}

static bool IsAltGrPhysicallyDown()
{
    return g_altRightDown && (g_ctrlLeftDown || g_ctrlRightDown);
}

static void ClearSelectorKeyboardSession()
{
    g_tabDown = false;
    g_tabSuppressed = false;
    g_sessionModifier = ModifierSession::None;
    g_inputSessionId = 0;
    g_altGrActive = IsAltGrPhysicallyDown();
    // Si AltGr sigue físicamente pulsado, Ctrl+Alt+Flecha debe seguir bloqueado
    // hasta soltarlo (p. ej. tras Esc); se desarma en el keyup del modificador.
    g_altGrTaskSwitcherArmed = g_altGrTaskSwitcherArmed && g_altGrActive;
}

// Solo UI thread: reconcilia el estado visual/temporizado al cerrar.
static void CleanupKeyboardState()
{
    if (g_cleanupInProgress)
        return;

    g_cleanupInProgress = true;
    g_uiTabDown = false;
    g_tabRepeatStarted = false;
    g_lastSelectorActivity = 0;
    g_cleanupInProgress = false;
}

// ENUMERACIÓN DE VENTANAS Y FILTROS

static bool IsShellWindow(HWND hwnd)
{
    if (hwnd == GetShellWindow())
        return true;
    if (hwnd == FindWindowW(L"Progman", nullptr))
        return true;
    if (hwnd == FindWindowW(L"WorkerW", nullptr))
        return true;
    if (hwnd == FindWindowW(L"Shell_TrayWnd", nullptr))
        return true;
    if (hwnd == FindWindowW(L"Shell_SecondaryTrayWnd", nullptr))
        return true;
    return false;
}

static std::wstring BaseNameWithoutExtension(const std::wstring& path)
{
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos)
        name.resize(dot);
    return name.empty() ? L"Application" : name;
}

static std::wstring LowerAscii(std::wstring value)
{
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (value[i] >= L'A' && value[i] <= L'Z')
            value[i] = static_cast<wchar_t>(value[i] + (L'a' - L'A'));
    }
    return value;
}

static bool ContainsInsensitive(const std::wstring& value, const wchar_t* fragment)
{
    return LowerAscii(value).find(LowerAscii(fragment)) != std::wstring::npos;
}

static std::wstring TrimDisplayName(const std::wstring& value)
{
    size_t first = 0;
    while (first < value.size() && (value[first] == L' ' || value[first] == L'\t' ||
                                    value[first] == L'\r' || value[first] == L'\n'))
        ++first;
    size_t last = value.size();
    while (last > first && (value[last - 1] == L' ' || value[last - 1] == L'\t' ||
                            value[last - 1] == L'\r' || value[last - 1] == L'\n'))
        --last;
    return value.substr(first, last - first);
}

static std::wstring GetFileDescription(const std::wstring& path)
{
    if (path.empty())
        return std::wstring();

    HMODULE version = LoadLibraryExW(L"version.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!version)
        return std::wstring();
    GetFileVersionInfoSizeWFn getSize = reinterpret_cast<GetFileVersionInfoSizeWFn>(
        GetProcAddress(version, "GetFileVersionInfoSizeW"));
    GetFileVersionInfoWFn getInfo = reinterpret_cast<GetFileVersionInfoWFn>(
        GetProcAddress(version, "GetFileVersionInfoW"));
    VerQueryValueWFn query = reinterpret_cast<VerQueryValueWFn>(
        GetProcAddress(version, "VerQueryValueW"));
    if (!getSize || !getInfo || !query)
    {
        FreeLibrary(version);
        return std::wstring();
    }

    DWORD handle = 0;
    DWORD size = getSize(path.c_str(), &handle);
    if (size == 0 || size > 1024 * 1024)
    {
        FreeLibrary(version);
        return std::wstring();
    }
    std::vector<BYTE> data(size);
    std::wstring description;
    if (getInfo(path.c_str(), 0, size, data.data()))
    {
        struct Translation { WORD language; WORD codePage; };
        Translation* translations = nullptr;
        UINT translationBytes = 0;
        if (query(data.data(), L"\\VarFileInfo\\Translation",
                  reinterpret_cast<LPVOID*>(&translations), &translationBytes) &&
            translationBytes >= sizeof(Translation))
        {
            const UINT count = translationBytes / sizeof(Translation);
            for (UINT i = 0; i < count && description.empty(); ++i)
            {
                wchar_t subBlock[128] = {};
                wsprintfW(subBlock, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                          translations[i].language, translations[i].codePage);
                LPWSTR value = nullptr;
                UINT valueChars = 0;
                if (query(data.data(), subBlock, reinterpret_cast<LPVOID*>(&value), &valueChars) &&
                    value && valueChars > 0)
                    description.assign(value, valueChars > 0 && value[valueChars - 1] == L'\0' ? valueChars - 1 : valueChars);
            }
        }
        if (description.empty())
        {
            LPWSTR value = nullptr;
            UINT valueChars = 0;
            if (query(data.data(), L"\\StringFileInfo\\040904b0\\FileDescription",
                      reinterpret_cast<LPVOID*>(&value), &valueChars) &&
                value && valueChars > 0)
                description.assign(value, valueChars > 0 && value[valueChars - 1] == L'\0' ? valueChars - 1 : valueChars);
        }
    }
    FreeLibrary(version);
    return TrimDisplayName(description);
}

static std::wstring GetHumanApplicationName(const std::wstring& path)
{
    static std::vector<std::pair<std::wstring, std::wstring>> nameCache;
    for (size_t i = 0; i < nameCache.size(); ++i)
    {
        if (nameCache[i].first == path)
            return nameCache[i].second;
    }

    std::wstring description = GetFileDescription(path);
    std::wstring result = description.empty() ? BaseNameWithoutExtension(path) : description;
    nameCache.push_back(std::make_pair(path, result));
    return result;
}

static bool GetProcessDetails(DWORD processId, std::wstring* path, std::wstring* appName)
{
    if (path)
        path->clear();
    if (appName)
        appName->clear();

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process)
        return false;

    wchar_t buffer[1024] = {};
    DWORD length = ARRAYSIZE(buffer);
    BOOL ok = QueryFullProcessImageNameW(process, 0, buffer, &length);
    CloseHandle(process);
    if (!ok || length == 0)
        return false;

    std::wstring fullPath(buffer, length);
    if (path)
        *path = fullPath;
    if (appName)
        *appName = GetHumanApplicationName(fullPath);
    return true;
}

enum class WindowClassification
{
    RealApplication,
    AuxiliaryWindow,
    SystemWindow,
    Unknown
};

static bool IsFullscreenApplicationWindow(HWND hwnd)
{
    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect))
        return false;

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor)
        return false;

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo))
        return false;

    // Borderless fullscreen windows are commonly WS_POPUP windows without
    // WS_EX_APPWINDOW or a normal frame. Accept only a top-level window whose
    // bounds cover an entire monitor (or its work area), not every popup.
    const LONG tolerance = 4;
    const RECT* candidates[] = { &monitorInfo.rcMonitor, &monitorInfo.rcWork };
    for (size_t i = 0; i < ARRAYSIZE(candidates); ++i)
    {
        const RECT& bounds = *candidates[i];
        if (windowRect.left <= bounds.left + tolerance &&
            windowRect.top <= bounds.top + tolerance &&
            windowRect.right >= bounds.right - tolerance &&
            windowRect.bottom >= bounds.bottom - tolerance)
            return true;
    }
    return false;
}

static WindowClassification ClassifyApplicationWindow(
    HWND hwnd, DWORD processId, const std::wstring& processPath,
    const std::wstring& appName, const std::wstring& windowClass,
    bool isCodeProcess)
{
    (void)processId;

    std::wstring name = LowerAscii(appName);
    std::wstring executableName = LowerAscii(BaseNameWithoutExtension(processPath));
    std::wstring cls = LowerAscii(windowClass);

    if (isCodeProcess)
    {
        LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                        WS_SYSMENU)) != 0;
        bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;

        if (!GetWindow(hwnd, GW_OWNER) && (hasNormalFrame || hasApplicationStyle))
            return WindowClassification::RealApplication;
    }

    if (name == L"textinputhost" || executableName == L"textinputhost" ||
        name == L"spotifyxboxgamebarweb" || executableName == L"spotifyxboxgamebarweb" ||
        name == L"searchhost" || executableName == L"searchhost" ||
        name == L"startmenuexperiencehost" || executableName == L"startmenuexperiencehost" ||
        name == L"shellexperiencehost" || executableName == L"shellexperiencehost" ||
        name == L"runtimebroker" || executableName == L"runtimebroker" ||
        name == L"lockapp" || executableName == L"lockapp" ||
        name == L"xboxgamebar" || executableName == L"xboxgamebar" ||
        name == L"gamebar" || executableName == L"gamebar")
        return WindowClassification::SystemWindow;

    if (cls == L"windows.ui.core.corewindow" ||
        cls == L"inputhost" || cls == L"textservicesframework")
        return WindowClassification::AuxiliaryWindow;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_NOACTIVATE) != 0)
        return WindowClassification::AuxiliaryWindow;

    bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;
    bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                    WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                    WS_SYSMENU)) != 0;
    bool isPopup = (style & WS_POPUP) != 0;

    if (name == L"systeminfo" && !hasApplicationStyle && !hasNormalFrame)
        return WindowClassification::AuxiliaryWindow;
    if (!hasApplicationStyle && !hasNormalFrame &&
        IsFullscreenApplicationWindow(hwnd))
        return WindowClassification::RealApplication;
    if (!hasApplicationStyle && !hasNormalFrame && isPopup)
        return WindowClassification::AuxiliaryWindow;

    if (!GetWindow(hwnd, GW_OWNER) && (hasApplicationStyle || hasNormalFrame))
        return WindowClassification::RealApplication;

    return WindowClassification::Unknown;
}

static int FindAppGroup(DWORD processId)
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].processId == processId)
            return static_cast<int>(i);
    }
    return -1;
}

// Ventanas legítimas que Windows/host marcan como owned o ToolWindow y que el
// filtro estricto descartaba (consolas, terminales, visor/editor de Windhawk).
// Solo se consulta para ventanas que ya iban a ser rechazadas por GW_OWNER o
// WS_EX_TOOLWINDOW: no convierte todas las ToolWindow en aplicaciones.
static bool IsConsoleOrWindhawkViewerWindow(HWND hwnd, DWORD processId,
                                            const std::wstring& processPath,
                                            const std::wstring& appName,
                                            const std::wstring& windowClass)
{
    // Nunca ventanas de nuestro propio proceso (selector, ventanas internas).
    if (processId == GetCurrentProcessId())
        return false;

    std::wstring name = LowerAscii(appName);
    std::wstring executableName = LowerAscii(BaseNameWithoutExtension(processPath));
    std::wstring cls = LowerAscii(windowClass);

    // Consolas clásicas (cmd/PowerShell en conhost) y Windows Terminal.
    if (cls == L"consolewindowclass" || cls == L"cascadia_hosting_window_class")
        return true;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                    WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                    WS_SYSMENU)) != 0;
    bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;
    // Exigir marco/AppWindow descarta ventanas auxiliares (IME, tooltips...)
    // que pertenezcan al mismo proceso.
    if (!hasNormalFrame && !hasApplicationStyle)
        return false;

    if (name == L"cmd" || executableName == L"cmd" ||
        name == L"powershell" || executableName == L"powershell" ||
        name == L"pwsh" || executableName == L"pwsh" ||
        name == L"openconsole" || executableName == L"openconsole" ||
        name == L"windowsterminal" || executableName == L"windowsterminal" ||
        name == L"wt" || executableName == L"wt")
        return true;

    // Visor/editor de código de Windhawk (ventana Chromium del proceso UI).
    if ((name == L"windhawk" || executableName == L"windhawk") && cls == L"chrome_widgetwin_1")
        return true;

    return false;
}

static bool IsRealUserApplicationWindow(HWND hwnd, DWORD* processId,
                                        std::wstring* processPath,
                                        std::wstring* appName)
{
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd))
        return false;
    if (LoadDwmFunctions() && g_dwmGetWindowAttribute)
    {
        constexpr DWORD kDwmWaCloaked = 14;
        BOOL cloaked = FALSE;
        if (SUCCEEDED(g_dwmGetWindowAttribute(hwnd, kDwmWaCloaked,
                                               &cloaked, sizeof(cloaked))) &&
            cloaked)
            return false;
    }
    if (hwnd == g_selector || IsShellWindow(hwnd))
        return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_NOACTIVATE) != 0)
        return false;

    // Owned / ToolWindow: rechazo por defecto, salvo la allowlist de abajo.
    const bool ownedOrToolWindow =
        GetWindow(hwnd, GW_OWNER) != nullptr ||
        (exStyle & WS_EX_TOOLWINDOW) != 0;

    wchar_t title[512] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title) - 1);
    if (title[0] == L'\0')
        return false;

    wchar_t windowClass[256] = {};
    GetClassNameW(hwnd, windowClass, ARRAYSIZE(windowClass) - 1);

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || pid == 0)
        return false;

    if (ownedOrToolWindow)
    {
        std::wstring allowPath;
        std::wstring allowName;
        GetProcessDetails(pid, &allowPath, &allowName);
        if (!IsConsoleOrWindhawkViewerWindow(hwnd, pid, allowPath, allowName, windowClass))
            return false;

        // Allowlist: se acepta directamente. El resto de filtros (que exigen
        // ventana sin owner) no aplican a estas ventanas.
        if (allowName.empty())
            allowName = title;
        if (processId)
            *processId = pid;
        if (processPath)
            *processPath = allowPath;
        if (appName)
            *appName = allowName;
        return true;
    }

    std::wstring path;
    std::wstring name;
    GetProcessDetails(pid, &path, &name);

    std::wstring lowerTitle = LowerAscii(title);
    std::wstring lowerClass = LowerAscii(windowClass);

    bool isCode = (LowerAscii(name) == L"code" ||
                   LowerAscii(name) == L"code - insiders" ||
                   LowerAscii(name) == L"vscodium" ||
                   ContainsInsensitive(path, L"code.exe") ||
                   ContainsInsensitive(path, L"vscodium.exe") ||
                   lowerTitle.find(L"visual studio code") != std::wstring::npos ||
                   lowerTitle.find(L"vscodium") != std::wstring::npos ||
                   (lowerClass == L"chrome_widgetwin_1" && lowerTitle.find(L"code") != std::wstring::npos));

    if (name.empty())
    {
        name = isCode ? L"Visual Studio Code" : title;
    }
    else if (isCode && name == L"Code")
    {
        name = L"Visual Studio Code";
    }

    WindowClassification classification = ClassifyApplicationWindow(
        hwnd, pid, path, name, windowClass, isCode);
    if (classification != WindowClassification::RealApplication)
        return false;

    std::wstring displayName = name;
    if (LowerAscii(BaseNameWithoutExtension(path)) == L"applicationframehost")
    {
        std::wstring titleName = TrimDisplayName(title);
        if (!titleName.empty())
            displayName = titleName;
    }

    if (processId)
        *processId = pid;
    if (processPath)
        *processPath = path;
    if (appName)
        *appName = displayName;
    return true;
}

static bool IsWindowPendingClose(HWND hwnd);
static void MarkWindowPendingClose(HWND hwnd);
static void UnmarkWindowPendingClose(HWND hwnd);

// Devuelve true si la ventana quedó (o ya estaba) en el registro MRU.
static bool RegisterUserWindow(HWND hwnd, bool activity)
{
    if (IsWindowPendingClose(hwnd))
        return false;

    DWORD processId = 0;
    std::wstring processPath;
    std::wstring appName;
    if (!IsRealUserApplicationWindow(hwnd, &processId, &processPath, &appName))
        return false;

    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (item.hwnd == hwnd)
        {
            if (activity)
                item.lastActivated = now;
            return true;
        }
    }

    UserWindowInfo item = {};
    item.hwnd = hwnd;
    item.processId = processId;
    item.processPath = processPath;
    item.appName = appName;
    item.lastActivated = activity ? now : 0;
    g_userWindows.push_back(item);
    return true;
}

static bool IsWindowPendingClose(HWND hwnd)
{
    for (size_t i = 0; i < g_pendingCloseWindows.size(); ++i)
    {
        if (g_pendingCloseWindows[i] == hwnd)
            return true;
    }
    return false;
}

static void MarkWindowPendingClose(HWND hwnd)
{
    if (!hwnd || IsWindowPendingClose(hwnd))
        return;
    g_pendingCloseWindows.push_back(hwnd);
}

static void UnmarkWindowPendingClose(HWND hwnd)
{
    std::erase(g_pendingCloseWindows, hwnd);
}

static void PrunePendingCloseWindows()
{
    for (size_t i = 0; i < g_pendingCloseWindows.size();)
    {
        if (!g_pendingCloseWindows[i] || !IsWindow(g_pendingCloseWindows[i]))
        {
            g_pendingCloseWindows.erase(g_pendingCloseWindows.begin() + i);
            continue;
        }
        ++i;
    }
}

static void PruneUserWindowRegistry()
{
    PrunePendingCloseWindows();
    for (size_t i = 0; i < g_userWindows.size();)
    {
        HWND hwnd = g_userWindows[i].hwnd;
        if (!hwnd || !IsWindow(hwnd) || hwnd == g_selector ||
            IsWindowPendingClose(hwnd))
        {
            g_userWindows.erase(g_userWindows.begin() + i);
            continue;
        }
        ++i;
    }
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM)
{
    RegisterUserWindow(hwnd, false);
    return TRUE;
}

// MRU por evento: reemplaza el polling de GetForegroundWindow cada 250 ms.
static void CALLBACK ForegroundRetryProc(HWND, UINT, UINT_PTR id, DWORD)
{
    KillTimer(nullptr, id);
    g_foregroundRetryTimer = 0;
    HWND target = g_foregroundRetryWindow;
    g_foregroundRetryWindow = nullptr;
    if (target && IsWindow(target) && GetForegroundWindow() == target)
        RegisterUserWindow(target, true);
}

static void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                            LONG idObject, LONG, DWORD, DWORD)
{
    if (event != EVENT_SYSTEM_FOREGROUND || !hwnd || idObject != OBJID_WINDOW)
        return;
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;

    PruneUserWindowRegistry();
    UnmarkWindowPendingClose(hwnd);
    if (RegisterUserWindow(hwnd, true))
        return;

    // Algunas ventanas llegan a primer plano antes de tener título/estilos
    // finales. Un único reintento diferido (no es polling permanente).
    g_foregroundRetryWindow = hwnd;
    if (!g_foregroundRetryTimer)
        g_foregroundRetryTimer = SetTimer(nullptr, 0, 250, ForegroundRetryProc);
}

static void RefreshWindowList()
{
    ReleaseGroupResources();
    PruneUserWindowRegistry();
    RegisterUserWindow(GetForegroundWindow(), true);
    EnumWindows(EnumWindowsProc, 0);

    g_groups.clear();
    HWND foreground = GetForegroundWindow();

    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (!IsRealUserApplicationWindow(item.hwnd, nullptr, nullptr, nullptr))
            continue;

        int groupIndex = FindAppGroup(item.processId);
        if (groupIndex < 0)
        {
            AppGroup group = {};
            group.processId = item.processId;
            group.processPath = item.processPath;
            group.appName = item.appName;
            group.representativeWindow = item.hwnd;
            group.icon = GetApplicationIcon(item.hwnd, item.processPath, &group.ownIcon);
            group.iconBitmap = nullptr;
            group.windows.push_back(item.hwnd);
            g_groups.push_back(group);
        }
        else
        {
            AppGroup& group = g_groups[groupIndex];
            bool duplicate = false;
            for (size_t j = 0; j < group.windows.size(); ++j)
            {
                if (group.windows[j] == item.hwnd)
                {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate)
                group.windows.push_back(item.hwnd);

            if (item.lastActivated > 0)
            {
                UserWindowInfo current = {};
                for (size_t j = 0; j < g_userWindows.size(); ++j)
                {
                    if (g_userWindows[j].hwnd == group.representativeWindow)
                    {
                        current = g_userWindows[j];
                        break;
                    }
                }
                if (item.lastActivated > current.lastActivated)
                {
                    if (group.iconBitmap)
                    {
                        group.iconBitmap->Release();
                        group.iconBitmap = nullptr;
                    }
                    if (group.ownIcon && group.icon)
                    {
                        DestroyIcon(group.icon);
                        group.icon = nullptr;
                    }
                    group.representativeWindow = item.hwnd;
                    group.icon = GetApplicationIcon(item.hwnd, item.processPath, &group.ownIcon);
                }
            }
        }
    }

    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].representativeWindow == foreground)
        {
            if (i != 0)
            {
                AppGroup first = g_groups[0];
                g_groups[0] = g_groups[i];
                g_groups[i] = first;
            }
            break;
        }
    }

    for (size_t i = 1; i < g_groups.size(); ++i)
    {
        size_t newest = i;
        ULONGLONG newestTime = 0;
        for (size_t j = 0; j < g_userWindows.size(); ++j)
        {
            if (g_userWindows[j].hwnd == g_groups[newest].representativeWindow)
            {
                newestTime = g_userWindows[j].lastActivated;
                break;
            }
        }

        for (size_t j = i + 1; j < g_groups.size(); ++j)
        {
            ULONGLONG candidateTime = 0;
            for (size_t k = 0; k < g_userWindows.size(); ++k)
            {
                if (g_userWindows[k].hwnd == g_groups[j].representativeWindow)
                {
                    candidateTime = g_userWindows[k].lastActivated;
                    break;
                }
            }
            if (candidateTime > newestTime)
            {
                newest = j;
                newestTime = candidateTime;
            }
        }

        if (newest != i)
        {
            AppGroup ordered = g_groups[i];
            g_groups[i] = g_groups[newest];
            g_groups[newest] = ordered;
        }
    }

    g_selected = 0;
    for (size_t i = 0; i < g_groups.size(); ++i)
        g_groups[i].mediaAccent = ExtractMediaAccentFromImage(g_groups[i].icon);
}

static const std::wstring& MakeSlotText(const AppGroup& group, bool selected)
{
    (void)selected;
    return group.appName;
}

// Título que se dibuja en una card. Las cards de los extremos de la disposición
// visual muestran como máximo kEdgeTitleChars caracteres seguidos de "...". La
// posición se obtiene de la posición visual continua (slot + g_animOffset), no
// del índice de grupo, así que sigue siendo correcta al navegar en cualquier
// sentido. El texto original del grupo no se modifica: se devuelve una cadena
// temporal reutilizada (sin asignaciones por frame una vez calentada).
static const size_t kEdgeTitleChars = 5;
static const std::wstring& MakeSlotDisplayText(const AppGroup& group, bool selected, int slot)
{
    const std::wstring& full = MakeSlotText(group, selected);
    const int lastSlot = GetCarouselSlotCount() - 1;
    const int visualSlot = static_cast<int>(
        floorf(static_cast<float>(slot) + g_animOffset + 0.5f));
    const bool isEdge = visualSlot <= 0 || visualSlot >= lastSlot;
    if (!isEdge || full.length() <= kEdgeTitleChars)
        return full;

    static std::wstring shortened;
    size_t cut = kEdgeTitleChars;
    if (IS_HIGH_SURROGATE(full[cut - 1]) && IS_LOW_SURROGATE(full[cut]))
        --cut;  // no partir un par sustituto
    while (cut > 0 && full[cut - 1] == L' ')
        --cut;
    shortened.assign(full, 0, cut);
    shortened.append(L"...");
    return shortened;
}

static const std::wstring& MakeCounterText()
{
    static std::wstring cached = L"0 / 0";
    static int cachedSelected = -1;
    static int cachedCount = -1;
    const int count = static_cast<int>(g_groups.size());
    if (cachedSelected != g_selected || cachedCount != count)
    {
        cachedSelected = g_selected;
        cachedCount = count;
        if (count <= 0)
            cached = L"0 / 0";
        else
        {
            wchar_t buffer[64] = {};
            wsprintfW(buffer, L"%d / %d", g_selected + 1, count);
            cached = buffer;
        }
    }
    return cached;
}

// DPI POR MONITOR

// DPI efectivo del monitor (shcore.dll, resuelto dinámicamente). 1.0 = 100%.
static float GetMonitorDpiScale(HMONITOR monitor)
{
    if (!g_shcoreApi)
        g_shcoreApi = LoadLibraryW(L"shcore.dll");
    if (g_shcoreApi && !g_getDpiForMonitor)
        g_getDpiForMonitor = reinterpret_cast<GetDpiForMonitorFn>(
            GetProcAddress(g_shcoreApi, "GetDpiForMonitor"));

    UINT dpiX = 96, dpiY = 96;
    const int kMdtEffectiveDpi = 0;
    if (monitor && g_getDpiForMonitor &&
        SUCCEEDED(g_getDpiForMonitor(monitor, kMdtEffectiveDpi, &dpiX, &dpiY)) &&
        dpiX > 0)
        return static_cast<float>(dpiX) / 96.0f;
    return 1.0f;
}

static void UpdateUIScaleForMonitor(HMONITOR monitor)
{
    MONITORINFO info = {};
    info.cbSize = sizeof(info);
    if (monitor && GetMonitorInfoW(monitor, &info))
        UpdateUIScaleForWorkArea(info.rcWork, GetMonitorDpiScale(monitor));
}

// Marca el hilo actual como Per-Monitor-V2 (solo afecta a este hilo).
static void SetThreadPerMonitorAwareV2()
{
    typedef HANDLE (WINAPI* SetThreadDpiAwarenessContextFn)(HANDLE);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32)
        return;
    SetThreadDpiAwarenessContextFn pSetThreadDpiAwarenessContext =
        reinterpret_cast<SetThreadDpiAwarenessContextFn>(
            GetProcAddress(user32, "SetThreadDpiAwarenessContext"));
    if (!pSetThreadDpiAwarenessContext)
        return;
#ifdef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    HANDLE context = reinterpret_cast<HANDLE>(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#else
    HANDLE context = reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4));
#endif
    pSetThreadDpiAwarenessContext(context);
}

// DWM Y EFECTOS DE FONDO

static bool LoadDwmFunctions()
{
    if (g_dwmRegisterThumbnail && g_dwmUnregisterThumbnail &&
        g_dwmUpdateThumbnailProperties && g_dwmGetWindowAttribute)
        return true;

    g_dwmApi = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dwmApi)
        return false;

    g_dwmRegisterThumbnail = reinterpret_cast<DwmRegisterThumbnailFn>(
        GetProcAddress(g_dwmApi, "DwmRegisterThumbnail"));
    g_dwmUnregisterThumbnail = reinterpret_cast<DwmUnregisterThumbnailFn>(
        GetProcAddress(g_dwmApi, "DwmUnregisterThumbnail"));
    g_dwmUpdateThumbnailProperties = reinterpret_cast<DwmUpdateThumbnailPropertiesFn>(
        GetProcAddress(g_dwmApi, "DwmUpdateThumbnailProperties"));
    g_dwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFn>(
        GetProcAddress(g_dwmApi, "DwmSetWindowAttribute"));
    g_dwmGetWindowAttribute = reinterpret_cast<DwmGetWindowAttributeFn>(
        GetProcAddress(g_dwmApi, "DwmGetWindowAttribute"));

    if (!g_dwmRegisterThumbnail || !g_dwmUnregisterThumbnail || !g_dwmUpdateThumbnailProperties)
    {
        FreeLibrary(g_dwmApi);
        g_dwmApi = nullptr;
        return false;
    }
    return true;
}

static void UnloadDwmFunctions()
{
    g_dwmRegisterThumbnail = nullptr;
    g_dwmUnregisterThumbnail = nullptr;
    g_dwmUpdateThumbnailProperties = nullptr;
    g_dwmSetWindowAttribute = nullptr;
    g_dwmGetWindowAttribute = nullptr;
    g_dwmGetCompositionTimingInfo = nullptr;
    g_dwmTimingResolved = false;
    g_dwmGlassEnabled = false;
    if (g_dwmApi)
    {
        FreeLibrary(g_dwmApi);
        g_dwmApi = nullptr;
    }
}

static void RemoveNativeSelectorFrame(HWND hwnd)
{
    if (!hwnd)
        return;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    style &= ~(static_cast<LONG_PTR>(WS_CAPTION | WS_THICKFRAME | WS_BORDER |
                                     WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX |
                                     WS_MAXIMIZEBOX));
    style |= WS_POPUP;
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    exStyle &= ~(static_cast<LONG_PTR>(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME |
                                       WS_EX_CLIENTEDGE | WS_EX_STATICEDGE |
                                       WS_EX_APPWINDOW));
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static void ApplySelectorVisuals(HWND hwnd)
{
    if (!hwnd)
        return;

    g_dwmGlassEnabled = false;
    if (!LoadDwmFunctions() || !g_dwmSetWindowAttribute)
        return;

    MARGINS margins = { -1, -1, -1, -1 };
    typedef HRESULT (WINAPI* DwmExtendFrameIntoClientAreaFn)(HWND, const MARGINS*);
    DwmExtendFrameIntoClientAreaFn pDwmExtendFrameIntoClientArea =
        reinterpret_cast<DwmExtendFrameIntoClientAreaFn>(
            GetProcAddress(g_dwmApi, "DwmExtendFrameIntoClientArea"));
    if (pDwmExtendFrameIntoClientArea)
    {
        HRESULT frameResult = pDwmExtendFrameIntoClientArea(hwnd, &margins);
        g_dwmGlassEnabled = SUCCEEDED(frameResult);
    }

    DWORD backdrop = kDwmSbtNone;
    g_dwmSetWindowAttribute(hwnd, kDwmWaSystemBackdropType,
                            &backdrop, sizeof(backdrop));

    BOOL useDarkMode = FALSE;
    g_dwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode,
                            &useDarkMode, sizeof(useDarkMode));
    DWORD borderColor = kDwmColorNone;
    g_dwmSetWindowAttribute(hwnd, kDwmBorderColor,
                            &borderColor, sizeof(borderColor));
    DWORD ncPolicy = kDwmNcRenderingPolicyUseWindowStyle;
    g_dwmSetWindowAttribute(hwnd, kDwmNcRenderingPolicy,
                            &ncPolicy, sizeof(ncPolicy));
    DWORD preference = kDwmCornerPreferenceDoNotRound;
    g_dwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference,
                            &preference, sizeof(preference));
}

static HICON GetApplicationIcon(HWND hwnd, const std::wstring& processPath, bool* outOwnIcon)
{
    if (outOwnIcon)
        *outOwnIcon = false;

    if (!hwnd || !IsWindow(hwnd))
        return LoadIconW(nullptr, MAKEINTRESOURCEW(32512));

    ULONG_PTR result = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_BIG, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    HICON icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON));
    if (icon)
        return icon;

    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL2, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM));
    if (icon)
        return icon;

    if (!processPath.empty())
    {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (!shell32)
            shell32 = LoadLibraryW(L"shell32.dll");
        if (shell32)
        {
            typedef UINT (WINAPI* ExtractIconExWFn)(LPCWSTR, int, HICON*, HICON*, UINT);
            ExtractIconExWFn pExtractIconExW = reinterpret_cast<ExtractIconExWFn>(
                GetProcAddress(shell32, "ExtractIconExW"));
            if (pExtractIconExW)
            {
                HICON hLarge = nullptr;
                if (pExtractIconExW(processPath.c_str(), 0, &hLarge, nullptr, 1) > 0 && hLarge)
                {
                    if (outOwnIcon)
                        *outOwnIcon = true;
                    return hLarge;
                }
            }
        }
    }

    return LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
}

// GESTIÓN DE DIRECT2D Y DIRECTWRITE

static void ReleaseDWriteFormats()
{
    if (g_dwriteCounterFormat) { g_dwriteCounterFormat->Release(); g_dwriteCounterFormat = nullptr; }
    if (g_dwriteCloseFormat) { g_dwriteCloseFormat->Release(); g_dwriteCloseFormat = nullptr; }
    if (g_dwriteNormalFormat) { g_dwriteNormalFormat->Release(); g_dwriteNormalFormat = nullptr; }
    if (g_dwriteSelectedFormat) { g_dwriteSelectedFormat->Release(); g_dwriteSelectedFormat = nullptr; }
    g_dwriteFormatsScale = 0.0f;
}

static bool LoadD2DAndDWrite()
{
    // Los TextFormat dependen de g_uiScale: si la escala cambió (otro monitor /
    // otro DPI) se reconstruyen en lugar de reutilizar formatos obsoletos.
    if (g_d2dFactory && g_d2dDCRenderTarget && g_dwriteFactory &&
        g_dwriteFormatsScale == g_uiScale.value)
        return true;

    if (!g_d2dApi)
        g_d2dApi = LoadLibraryExW(L"d2d1.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dwriteApi)
        g_dwriteApi = LoadLibraryExW(L"dwrite.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (g_d2dApi && !g_d2dFactory)
    {
        D2D1CreateFactoryFn pD2D1CreateFactory = reinterpret_cast<D2D1CreateFactoryFn>(
            GetProcAddress(g_d2dApi, "D2D1CreateFactory"));
        if (pD2D1CreateFactory)
        {
            D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_NONE };
            pD2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, kIidD2D1Factory, &options,
                               reinterpret_cast<void**>(&g_d2dFactory));
        }
    }

    if (g_d2dFactory && !g_d2dDCRenderTarget)
    {
        // El layout está expresado en píxeles físicos: el render target debe
        // trabajar a 96 DPI (1 DIP = 1 px) y no heredar el DPI del sistema.
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
        );
        if (SUCCEEDED(g_d2dFactory->CreateDCRenderTarget(&props, &g_d2dDCRenderTarget)))
        {
            g_d2dDCRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White), &g_d2dBrush);
        }
    }

    if (g_dwriteApi && !g_dwriteFactory)
    {
        DWriteCreateFactoryFn pDWriteCreateFactory = reinterpret_cast<DWriteCreateFactoryFn>(
            GetProcAddress(g_dwriteApi, "DWriteCreateFactory"));
        if (pDWriteCreateFactory)
        {
            pDWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, kIidDWriteFactory,
                                 reinterpret_cast<IUnknown**>(&g_dwriteFactory));
        }
    }

    if (g_dwriteFactory && (!g_dwriteSelectedFormat ||
                            g_dwriteFormatsScale != g_uiScale.value))
    {
        ReleaseDWriteFormats();

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(10.0f, 16.0f * g_uiScale.value), L"", &g_dwriteSelectedFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(9.0f, 13.0f * g_uiScale.value), L"", &g_dwriteNormalFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(9.0f, 13.0f * g_uiScale.value), L"", &g_dwriteCounterFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(11.0f, 18.0f * g_uiScale.value), L"", &g_dwriteCloseFormat);

        if (g_dwriteSelectedFormat)
        {
            g_dwriteSelectedFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteSelectedFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteNormalFormat)
        {
            g_dwriteNormalFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteNormalFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteCounterFormat)
        {
            g_dwriteCounterFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteCounterFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_dwriteCounterFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteCloseFormat)
        {
            g_dwriteCloseFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteCloseFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_dwriteCloseFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        g_dwriteFormatsScale = g_uiScale.value;
    }

    return (g_d2dFactory != nullptr && g_d2dDCRenderTarget != nullptr);
}

static ID2D1Bitmap* CreateD2DBitmapFromHIcon(ID2D1RenderTarget* renderTarget, HICON hIcon)
{
    if (!renderTarget || !hIcon)
        return nullptr;

    if (!LoadGdiFunctions())
        return nullptr;

    ICONINFO iconInfo = {};
    if (!GetIconInfo(hIcon, &iconInfo))
        return nullptr;

    HDC screenDC = GetDC(nullptr);
    if (!screenDC)
    {
        if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    BITMAP bm = {};
    g_getObjectW(iconInfo.hbmColor ? iconInfo.hbmColor : iconInfo.hbmMask,
                 sizeof(BITMAP), &bm);

    int width  = bm.bmWidth;
    int height = iconInfo.hbmColor ? bm.bmHeight : (bm.bmHeight / 2);

    if (width <= 0 || height <= 0 || width > 1024 || height > 1024)
    {
        ReleaseDC(nullptr, screenDC);
        if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    std::vector<DWORD> pixels(static_cast<size_t>(width * height), 0);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    bool hasAlpha = false;

    if (iconInfo.hbmColor)
    {
        g_getDIBits(screenDC, iconInfo.hbmColor, 0,
                    static_cast<UINT>(height),
                    pixels.data(), &bmi, DIB_RGB_COLORS);

        for (int i = 0; i < width * height; ++i)
        {
            if ((pixels[i] & 0xFF000000) != 0)
            {
                hasAlpha = true;
                break;
            }
        }
    }

    if (hasAlpha)
    {
        for (int i = 0; i < width * height; ++i)
        {
            DWORD c = pixels[i];
            BYTE  a = static_cast<BYTE>((c >> 24) & 0xFF);
            if (a == 0)
            {
                pixels[i] = 0;
            }
            else
            {
                BYTE r = static_cast<BYTE>((c >> 16) & 0xFF);
                BYTE g = static_cast<BYTE>((c >>  8) & 0xFF);
                BYTE b = static_cast<BYTE>( c        & 0xFF);

                r = static_cast<BYTE>((static_cast<UINT>(r) * a + 127) / 255);
                g = static_cast<BYTE>((static_cast<UINT>(g) * a + 127) / 255);
                b = static_cast<BYTE>((static_cast<UINT>(b) * a + 127) / 255);

                pixels[i] = (static_cast<DWORD>(a) << 24) |
                            (static_cast<DWORD>(r) << 16) |
                            (static_cast<DWORD>(g) <<  8) |
                             static_cast<DWORD>(b);
            }
        }
    }
    else if (iconInfo.hbmMask)
    {
        std::vector<DWORD> maskPixels(static_cast<size_t>(width * height), 0);
        g_getDIBits(screenDC, iconInfo.hbmMask, 0,
                    static_cast<UINT>(height),
                    maskPixels.data(), &bmi, DIB_RGB_COLORS);

        for (int i = 0; i < width * height; ++i)
        {
            bool isTransparent = (maskPixels[i] & 0x00FFFFFF) != 0;
            if (isTransparent)
            {
                pixels[i] = 0;
            }
            else
            {
                DWORD c = pixels[i];
                pixels[i] = 0xFF000000 | (c & 0x00FFFFFF);
            }
        }
    }

    ReleaseDC(nullptr, screenDC);
    if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);

    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f
    );

    ID2D1Bitmap* pBitmap = nullptr;
    HRESULT hr = renderTarget->CreateBitmap(
        D2D1::SizeU(static_cast<UINT32>(width), static_cast<UINT32>(height)),
        pixels.data(),
        static_cast<UINT32>(width * sizeof(DWORD)),
        &props,
        &pBitmap
    );

    return SUCCEEDED(hr) ? pBitmap : nullptr;
}

static void ReleaseGroupResources()
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].iconBitmap)
        {
            g_groups[i].iconBitmap->Release();
            g_groups[i].iconBitmap = nullptr;
        }
        if (g_groups[i].surfaceBrushNormal)
        {
            g_groups[i].surfaceBrushNormal->Release();
            g_groups[i].surfaceBrushNormal = nullptr;
        }
        if (g_groups[i].surfaceBrushSelected)
        {
            g_groups[i].surfaceBrushSelected->Release();
            g_groups[i].surfaceBrushSelected = nullptr;
        }
        if (g_groups[i].ownIcon && g_groups[i].icon)
        {
            DestroyIcon(g_groups[i].icon);
            g_groups[i].icon = nullptr;
        }
    }
}

static void ReleaseCachedD2DObjects()
{
    if (g_sceneLayer) { g_sceneLayer->Release(); g_sceneLayer = nullptr; }
    if (g_cardExitLayer) { g_cardExitLayer->Release(); g_cardExitLayer = nullptr; }
    if (g_closeLayer) { g_closeLayer->Release(); g_closeLayer = nullptr; }
    if (g_cardMaskGeometry) { g_cardMaskGeometry->Release(); g_cardMaskGeometry = nullptr; }
    g_cardMaskKey = D2D1_ROUNDED_RECT();
}

static void UnloadD2DAndDWrite()
{
    ReleaseGroupResources();
    ReleaseCachedD2DObjects();

    ReleaseDWriteFormats();
    if (g_dwriteFactory) { g_dwriteFactory->Release(); g_dwriteFactory = nullptr; }

    if (g_closeAccentBrush) { g_closeAccentBrush->Release(); g_closeAccentBrush = nullptr; }
    if (g_backgroundSpotlightBrush) { g_backgroundSpotlightBrush->Release(); g_backgroundSpotlightBrush = nullptr; }
    if (g_ambientShadowBrush) { g_ambientShadowBrush->Release(); g_ambientShadowBrush = nullptr; }
    g_backgroundSpotlightBrushColor = CLR_INVALID;
    if (g_closeButtonGeometry) { g_closeButtonGeometry->Release(); g_closeButtonGeometry = nullptr; }
    g_closeButtonGeometryRect = {};
    if (g_d2dBrush) { g_d2dBrush->Release(); g_d2dBrush = nullptr; }
    if (g_d2dDCRenderTarget) { g_d2dDCRenderTarget->Release(); g_d2dDCRenderTarget = nullptr; }
    if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = nullptr; }

    if (g_dwriteApi) { FreeLibrary(g_dwriteApi); g_dwriteApi = nullptr; }
    if (g_d2dApi) { FreeLibrary(g_d2dApi); g_d2dApi = nullptr; }
}

// D2DERR_RECREATE_TARGET: los recursos dependientes del dispositivo se liberan
// y se recrean de forma perezosa en el siguiente frame. Los iconos HICON y los
// datos de grupos se conservan.
static void ReleaseD2DDeviceResources()
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].iconBitmap)
        {
            g_groups[i].iconBitmap->Release();
            g_groups[i].iconBitmap = nullptr;
        }
        if (g_groups[i].surfaceBrushNormal)
        {
            g_groups[i].surfaceBrushNormal->Release();
            g_groups[i].surfaceBrushNormal = nullptr;
        }
        if (g_groups[i].surfaceBrushSelected)
        {
            g_groups[i].surfaceBrushSelected->Release();
            g_groups[i].surfaceBrushSelected = nullptr;
        }
    }
    ReleaseCachedD2DObjects();
    if (g_closeAccentBrush) { g_closeAccentBrush->Release(); g_closeAccentBrush = nullptr; }
    if (g_backgroundSpotlightBrush) { g_backgroundSpotlightBrush->Release(); g_backgroundSpotlightBrush = nullptr; }
    if (g_ambientShadowBrush) { g_ambientShadowBrush->Release(); g_ambientShadowBrush = nullptr; }
    g_backgroundSpotlightBrushColor = CLR_INVALID;
    if (g_d2dBrush) { g_d2dBrush->Release(); g_d2dBrush = nullptr; }
    if (g_d2dDCRenderTarget) { g_d2dDCRenderTarget->Release(); g_d2dDCRenderTarget = nullptr; }
    RequestFrame();
}

static bool LoadGdiFunctions()
{
    if (g_createFontIndirectW && g_deleteObject && g_createRoundRectRgn && g_getStockObject &&
        g_setBkMode && g_setTextColor && g_createSolidBrush &&
        g_createPen && g_selectObject && g_roundRect && g_getTextFaceW &&
        g_getDIBits && g_getObjectW)
        return true;

    if (!g_gdiApi)
        g_gdiApi = LoadLibraryW(L"gdi32.dll");
    if (!g_gdiApi)
        return false;

    g_createFontIndirectW = reinterpret_cast<CreateFontIndirectWFn>(
        GetProcAddress(g_gdiApi, "CreateFontIndirectW"));
    g_deleteObject = reinterpret_cast<DeleteObjectFn>(
        GetProcAddress(g_gdiApi, "DeleteObject"));
    g_createRoundRectRgn = reinterpret_cast<CreateRoundRectRgnFn>(
        GetProcAddress(g_gdiApi, "CreateRoundRectRgn"));
    g_getStockObject = reinterpret_cast<GetStockObjectFn>(
        GetProcAddress(g_gdiApi, "GetStockObject"));
    g_setBkMode = reinterpret_cast<SetBkModeFn>(
        GetProcAddress(g_gdiApi, "SetBkMode"));
    g_setTextColor = reinterpret_cast<SetTextColorFn>(
        GetProcAddress(g_gdiApi, "SetTextColor"));
    g_createSolidBrush = reinterpret_cast<CreateSolidBrushFn>(
        GetProcAddress(g_gdiApi, "CreateSolidBrush"));
    g_createPen = reinterpret_cast<CreatePenFn>(
        GetProcAddress(g_gdiApi, "CreatePen"));
    g_selectObject = reinterpret_cast<SelectObjectFn>(
        GetProcAddress(g_gdiApi, "SelectObject"));
    g_roundRect = reinterpret_cast<RoundRectFn>(
        GetProcAddress(g_gdiApi, "RoundRect"));
    g_getTextFaceW = reinterpret_cast<GetTextFaceWFn>(
        GetProcAddress(g_gdiApi, "GetTextFaceW"));
    g_getDIBits = reinterpret_cast<GetDIBitsFn>(
        GetProcAddress(g_gdiApi, "GetDIBits"));
    g_getObjectW = reinterpret_cast<GetObjectWFn>(
        GetProcAddress(g_gdiApi, "GetObjectW"));

    if (!g_createFontIndirectW || !g_deleteObject || !g_createRoundRectRgn || !g_getStockObject ||
        !g_setBkMode || !g_setTextColor || !g_createSolidBrush ||
        !g_createPen || !g_selectObject || !g_roundRect ||
        !g_getDIBits || !g_getObjectW)
    {
        FreeLibrary(g_gdiApi);
        g_gdiApi = nullptr;
        return false;
    }
    return true;
}

static MediaAccent ExtractMediaAccentFromImage(HICON icon)
{
    MediaAccent result;
    if (!icon || !LoadGdiFunctions())
        return result;

    ICONINFO iconInfo = {};
    if (!GetIconInfo(icon, &iconInfo) || !iconInfo.hbmColor)
    {
        if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
        return result;
    }

    BITMAP bitmap = {};
    if (!g_getObjectW(iconInfo.hbmColor, sizeof(bitmap), &bitmap) ||
        bitmap.bmWidth <= 0 || bitmap.bmHeight <= 0)
    {
        g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
        return result;
    }

    const int width = std::min(static_cast<int>(bitmap.bmWidth), 128);
    const int height = std::min(static_cast<int>(bitmap.bmHeight), 128);
    std::vector<DWORD> pixels(static_cast<size_t>(width) * height, 0);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screenDC = GetDC(nullptr);
    if (screenDC && g_getDIBits(screenDC, iconInfo.hbmColor, 0,
                                static_cast<UINT>(height), pixels.data(),
                                &bmi, DIB_RGB_COLORS) != 0)
    {
        double weightedR = 0.0, weightedG = 0.0, weightedB = 0.0;
        double totalWeight = 0.0;
        for (DWORD pixel : pixels)
        {
            int r = static_cast<int>((pixel >> 16) & 0xFF);
            int g = static_cast<int>((pixel >> 8) & 0xFF);
            int b = static_cast<int>(pixel & 0xFF);
            int maximum = std::max(r, std::max(g, b));
            int minimum = std::min(r, std::min(g, b));
            if (maximum < 28)
                continue;

            // Favorecer tonos con información cromática sin dejar que dominen.
            double saturation = static_cast<double>(maximum - minimum) / 255.0;
            double brightness = static_cast<double>(maximum) / 255.0;
            double weight = 0.25 + saturation * 0.75;
            weight *= 0.35 + brightness * 0.65;
            weightedR += r * weight;
            weightedG += g * weight;
            weightedB += b * weight;
            totalWeight += weight;
        }

        if (totalWeight > 0.0)
        {
            int r = static_cast<int>(weightedR / totalWeight);
            int g = static_cast<int>(weightedG / totalWeight);
            int b = static_cast<int>(weightedB / totalWeight);
            int maximum = std::max(r, std::max(g, b));
            int minimum = std::min(r, std::min(g, b));
            if (maximum > 0 && maximum - minimum >= 8)
            {
                // La card sigue siendo Obsidian: el color solo ilumina su borde.
                const float scale = 0.78f;
                result.color = RGB(static_cast<BYTE>(r * scale),
                                   static_cast<BYTE>(g * scale),
                                   static_cast<BYTE>(b * scale));
                result.strength = 1.0f;
                result.valid = true;
            }
            else
            {
                // Imagen casi monocroma/negra: producir un tono frío mínimo,
                // determinista y oscuro para que el contorno no desaparezca.
                BYTE level = static_cast<BYTE>(std::max(12, std::min(24, maximum)));
                result.color = RGB(level, static_cast<BYTE>(level + 2),
                                   static_cast<BYTE>(level + 5));
                result.strength = 0.75f;
                result.valid = true;
            }
        }
        else
        {
            // No se encontraron píxeles suficientemente luminosos: fallback
            // cromático fijo, sobrio y no aleatorio para artwork casi negro.
            result.color = RGB(12, 14, 18);
            result.strength = 0.65f;
            result.valid = true;
        }
    }

    if (screenDC)
        ReleaseDC(nullptr, screenDC);
    g_deleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
    return result;
}

static D2D1_COLOR_F AddMediaAccent(const D2D1_COLOR_F& base,
                                   const MediaAccent& accent, float influence)
{
    if (!accent.valid || influence <= 0.0f)
        return base;

    const float ar = static_cast<float>(GetRValue(accent.color)) / 255.0f;
    const float ag = static_cast<float>(GetGValue(accent.color)) / 255.0f;
    const float ab = static_cast<float>(GetBValue(accent.color)) / 255.0f;
    return D2D1::ColorF(
        std::min(1.0f, base.r + ar * influence),
        std::min(1.0f, base.g + ag * influence),
        std::min(1.0f, base.b + ab * influence), base.a);
}

static HFONT CreateModernFont(const wchar_t* preferredFace, const wchar_t* fallbackFace, LONG height, LONG weight)
{
    if (!LoadGdiFunctions())
        return nullptr;

    LOGFONTW font = {};
    font.lfHeight = height;
    font.lfWeight = weight;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    font.lfQuality = 6;
    font.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

    if (preferredFace && preferredFace[0] != L'\0')
    {
        lstrcpynW(font.lfFaceName, preferredFace, LF_FACESIZE);
        HFONT hFont = g_createFontIndirectW(&font);
        if (hFont)
        {
            if (g_getTextFaceW)
            {
                HDC screenDC = GetDC(nullptr);
                if (screenDC)
                {
                    HGDIOBJ old = g_selectObject(screenDC, hFont);
                    wchar_t actualFace[LF_FACESIZE] = {};
                    g_getTextFaceW(screenDC, LF_FACESIZE, actualFace);
                    g_selectObject(screenDC, old);
                    ReleaseDC(nullptr, screenDC);

                    if (_wcsicmp(actualFace, preferredFace) == 0)
                        return hFont;
                }
            }
            else
            {
                return hFont;
            }
            g_deleteObject(hFont);
        }
    }

    lstrcpynW(font.lfFaceName, (fallbackFace && fallbackFace[0] != L'\0') ? fallbackFace : L"Segoe UI", LF_FACESIZE);
    return g_createFontIndirectW(&font);
}

static void ReleaseUiFontObjects()
{
    if (g_deleteObject)
    {
        if (g_nameFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_nameFont));
        if (g_selectedNameFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_selectedNameFont));
        if (g_secondaryFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_secondaryFont));
    }
    g_nameFont = nullptr;
    g_selectedNameFont = nullptr;
    g_secondaryFont = nullptr;
    g_gdiFontsScale = 0.0f;
}

static void CreateUiFonts()
{
    if (g_nameFont || g_selectedNameFont || g_secondaryFont)
    {
        // Las fuentes GDI del fallback también dependen de la escala.
        if (g_gdiFontsScale == g_uiScale.value)
            return;
        ReleaseUiFontObjects();
    }

    g_selectedNameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                          -std::max(10, ScaleLayoutPx(16.0f)), FW_SEMIBOLD);
    g_nameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                  -std::max(9, ScaleLayoutPx(13.0f)), 500);
    g_secondaryFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                       -std::max(9, ScaleLayoutPx(13.0f)), 500);
    g_gdiFontsScale = g_uiScale.value;
}

static void DestroyUiFonts()
{
    ReleaseUiFontObjects();
    g_getTextFaceW = nullptr;
    g_getDIBits = nullptr;
    g_getObjectW = nullptr;
    if (g_gdiApi)
    {
        FreeLibrary(g_gdiApi);
        g_gdiApi = nullptr;
    }
}

static RECT ContainedRect(const RECT& area, int sourceWidth, int sourceHeight)
{
    RECT result = area;
    if (sourceWidth <= 0 || sourceHeight <= 0)
        return result;

    int areaWidth = area.right - area.left;
    int areaHeight = area.bottom - area.top;
    if (areaWidth <= 0 || areaHeight <= 0)
        return result;

    long long widthByHeight = static_cast<long long>(areaHeight) * sourceWidth / sourceHeight;
    long long heightByWidth = static_cast<long long>(areaWidth) * sourceHeight / sourceWidth;
    int width = widthByHeight <= areaWidth ? static_cast<int>(widthByHeight) : areaWidth;
    int height = widthByHeight <= areaWidth ? areaHeight : static_cast<int>(heightByWidth);
    result.left = area.left + (areaWidth - width) / 2;
    result.top = area.top + (areaHeight - height) / 2;
    result.right = result.left + width;
    result.bottom = result.top + height;
    return result;
}


// GEOMETRÍA Y TRAYECTORIA DEL CARRUSEL

static RECT ScaleRectAroundCenter(const RECT& rect, float scale)
{
    if (fabsf(scale - 1.0f) < 0.0001f)
        return rect;
    float cx = (static_cast<float>(rect.left) + static_cast<float>(rect.right)) * 0.5f;
    float cy = (static_cast<float>(rect.top) + static_cast<float>(rect.bottom)) * 0.5f;
    RECT result = {};
    result.left = static_cast<LONG>(roundf(cx + (rect.left - cx) * scale));
    result.right = static_cast<LONG>(roundf(cx + (rect.right - cx) * scale));
    result.top = static_cast<LONG>(roundf(cy + (rect.top - cy) * scale));
    result.bottom = static_cast<LONG>(roundf(cy + (rect.bottom - cy) * scale));
    return result;
}

static RECT ScaleRectAroundPoint(const RECT& rect, float scale,
                                 float pivotX, float pivotY)
{
    if (fabsf(scale - 1.0f) < 0.0001f)
        return rect;
    RECT result = {};
    result.left = static_cast<LONG>(roundf(pivotX + (rect.left - pivotX) * scale));
    result.right = static_cast<LONG>(roundf(pivotX + (rect.right - pivotX) * scale));
    result.top = static_cast<LONG>(roundf(pivotY + (rect.top - pivotY) * scale));
    result.bottom = static_cast<LONG>(roundf(pivotY + (rect.bottom - pivotY) * scale));
    return result;
}

static RECT GetSlotCardRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    RECT rc = {};
    rc.left = static_cast<int>(roundf(g.left));
    rc.top = static_cast<int>(roundf(g.top));
    rc.right = static_cast<int>(roundf(g.right));
    rc.bottom = static_cast<int>(roundf(g.bottom));
    if (slot == GetCarouselCenterSlot())
        rc = ScaleRectAroundCenter(rc, g_cardSnapScale);
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        rc.top += offset;
        rc.bottom += offset;
    }
    return rc;
}

static RECT GetSlotHeaderRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    int pad = static_cast<int>(roundf(g.padding));
    RECT header = {};
    header.left = static_cast<int>(roundf(g.left)) + pad;
    header.right = static_cast<int>(roundf(g.right)) - pad;
    header.top = static_cast<int>(roundf(g.top)) + pad;
    header.bottom = header.top + static_cast<int>(roundf(g.headerHeight));
    // El título se mantiene en su geometría normal. El bounce se separa
    // lógicamente y se aplica únicamente al icono en PaintSlotHeader*.
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        header.top += offset;
        header.bottom += offset;
    }
    return header;
}

static RECT GetSlotPreviewRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    int pad = static_cast<int>(roundf(g.padding));
    int hHeight = static_cast<int>(roundf(g.headerHeight));
    float dist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
    int gap = ScaleLayoutPx(static_cast<float>((dist < 0.5f) ? 8 : (dist < 1.5f ? 6 : 4)));

    RECT preview = {};
    preview.left = static_cast<int>(roundf(g.left)) + pad;
    preview.right = static_cast<int>(roundf(g.right)) - pad;
    preview.top = static_cast<int>(roundf(g.top)) + pad + hHeight + gap;
    preview.bottom = static_cast<int>(roundf(g.bottom)) - pad;
    if (slot == GetCarouselCenterSlot())
        preview = ScaleRectAroundCenter(preview, g_cardSnapScale);
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        preview.top += offset;
        preview.bottom += offset;
    }
    return preview;
}

static RECT GetCounterRect()
{
    RECT rc = {};
    int width = ScaleLayoutPx(static_cast<float>(kCounterWidth));
    int height = ScaleLayoutPx(static_cast<float>(kCounterHeight));
    rc.left = (g_runtimeSelectorWidth - width) / 2;
    rc.top = ScaleLayoutPx(static_cast<float>(kCounterTop));
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;
    return rc;
}

static RECT GetCloseButtonRect(int slot)
{
    RECT card = GetSlotCardRect(slot);
    const int hitSize = ScaleLayoutPx(28.0f);
    RECT rc = {};
    rc.right = card.right - static_cast<int>(roundf(DpiPx(5.0f)));
    rc.left = rc.right - hitSize;
    rc.top = card.top + ScaleLayoutPx(3.0f);
    rc.bottom = rc.top + hitSize;
    return rc;
}

static RECT GetCloseHoverButtonRect(int slot)
{
    RECT rc = GetCloseButtonRect(slot);
    int buttonOffsetY = ScaleLayoutPx(3.0f);
    rc.top += buttonOffsetY;
    rc.bottom += buttonOffsetY;
    return rc;
}

static ID2D1PathGeometry* BuildCloseButtonGeometry(const RECT& rect)
{
    if (!g_d2dFactory)
        return nullptr;

    ID2D1PathGeometry* geometry = nullptr;
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geometry)) || !geometry ||
        FAILED(geometry->Open(&sink)) || !sink)
    {
        if (geometry) geometry->Release();
        return nullptr;
    }

    float left = static_cast<float>(rect.left);
    float top = static_cast<float>(rect.top);
    float right = static_cast<float>(rect.right);
    float bottom = static_cast<float>(rect.bottom);
    float r = static_cast<float>(ScaleLayoutPx(6.0f));
    float topRightX = static_cast<float>(ScaleLayoutPx(12.0f));
    float topRightY = static_cast<float>(ScaleLayoutPx(6.0f));

    sink->BeginFigure(D2D1::Point2F(left + r, top), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(right - topRightX, top));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(right, top + topRightY),
        D2D1::SizeF(topRightX, topRightY), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(right, bottom - r));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(right - r, bottom), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(left + r, bottom));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(left, bottom - r), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(left, top + r));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(left + r, top), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geometry;
}

static ID2D1PathGeometry* CreateCloseButtonGeometry(const RECT& rect)
{
    if (g_closeButtonGeometry &&
        std::memcmp(&g_closeButtonGeometryRect, &rect, sizeof(RECT)) == 0)
        return g_closeButtonGeometry;
    if (g_closeButtonGeometry)
    {
        g_closeButtonGeometry->Release();
        g_closeButtonGeometry = nullptr;
    }
    g_closeButtonGeometry = BuildCloseButtonGeometry(rect);
    if (g_closeButtonGeometry)
        g_closeButtonGeometryRect = rect;
    return g_closeButtonGeometry;
}

static int ResolveGroupIndex(int slot)
{
    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
        return -1;

    int index = g_selected + (slot - GetCarouselCenterSlot());
    while (index < 0)
        index += count;
    while (index >= count)
        index -= count;
    return index;
}

static void UpdateHoveredSlot(POINT ptClient)
{
    int next = GetSlotAtPoint(ptClient);
    bool nextClose = false;
    if (next >= 0)
    {
        RECT closeRect = GetCloseHoverButtonRect(next);
        nextClose = PtInRect(&closeRect, ptClient) != FALSE;
    }
    if (next == g_hoveredSlot && nextClose == g_hoveredCloseButton)
        return;
    g_hoveredSlot = next;
    g_hoveredCloseButton = nextClose;
    RequestFrame();
}

static int GetSlotAtPoint(POINT ptClient)
{
    RECT rcCenter = GetSlotCardRect(GetCarouselCenterSlot());
    if (PtInRect(&rcCenter, ptClient))
        return GetCarouselCenterSlot();

    for (int i = 0; i < GetCarouselSlotCount(); ++i)
    {
        int slot = i;
        RECT rc = GetSlotCardRect(slot);
        if (PtInRect(&rc, ptClient))
            return slot;
    }
    return -1;
}

static void CloseWindowForSlot(int slot)
{
    if (!IsSelectorActive())
        return;

    int groupIndex = ResolveGroupIndex(slot);
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()))
        return;

    AppGroup& group = g_groups[groupIndex];
    HWND target = group.representativeWindow;
    if (!target || !IsWindow(target))
        return;

    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_cardExitActive = true;
    g_cardExitSlot = slot;
    g_cardExitGroupIndex = groupIndex;
    g_cardExitTarget = target;
    g_cardExitStart = FrameClockNow();
    g_cardExitProgress = 0.0f;
    g_cardExitDirection = (static_cast<ULONGLONG>(g_cardExitStart) & 1ULL) ? 1.0f : -1.0f;
    g_selectorAnimation = SelectorAnimationState::CardExit;
    g_selectorAnimationStart = g_cardExitStart;
    ApplyScenePresence(1.0f);
    UpdateSelectorControls();
}

static void DrawCardGlowAndBorderGDI(HDC hdc, const RECT& rect, int radius, bool selected)
{
    COLORREF bgCol = selected ? g_cardStyle.selectedCardBackground : g_cardStyle.cardBackground;
    COLORREF borderCol = selected ? g_cardStyle.selectedBorderColor : g_cardStyle.borderColor;

    HBRUSH bgBrush = g_createSolidBrush ? g_createSolidBrush(bgCol) : nullptr;
    HPEN borderPen = g_createPen ? g_createPen(PS_SOLID, selected ? 2 : 1, borderCol) : nullptr;

    if (bgBrush && borderPen && g_roundRect && g_deleteObject)
    {
        HGDIOBJ oldBrush = g_selectObject(hdc, bgBrush);
        HGDIOBJ oldPen = g_selectObject(hdc, borderPen);
        g_roundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
        g_selectObject(hdc, oldBrush);
        g_selectObject(hdc, oldPen);
        g_deleteObject(bgBrush);
        g_deleteObject(borderPen);
    }
}

static void PaintBottomShadowD2D(const RECT& objectRect, float radius,
                                 float strength)
{
    if (!g_d2dDCRenderTarget || !g_d2dBrush)
        return;
    const int layers = 3;
    for (int i = 0; i < layers; ++i)
    {
        int spread = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int offset = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int depth = ScaleLayoutPx(static_cast<float>(4 + i * 3));
        RECT shadow = {
            objectRect.left - spread,
            objectRect.bottom - ScaleLayoutPx(1.0f) + offset,
            objectRect.right + spread,
            objectRect.bottom + offset + depth
        };
        float alpha = strength * (i == 0 ? 0.42f : (i == 1 ? 0.22f : 0.09f));
        g_d2dBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, alpha));
        D2D1_ROUNDED_RECT shadowRect = D2D1::RoundedRect(
            D2D1::RectF(static_cast<float>(shadow.left),
                        static_cast<float>(shadow.top),
                        static_cast<float>(shadow.right),
                        static_cast<float>(shadow.bottom)),
            radius + static_cast<float>(spread), radius + static_cast<float>(spread));
        g_d2dDCRenderTarget->FillRoundedRectangle(&shadowRect, g_d2dBrush);
    }
}

static void PaintBottomShadowGDI(HDC hdc, const RECT& objectRect, int radius)
{
    if (!hdc || !g_createSolidBrush || !g_selectObject || !g_getStockObject ||
        !g_roundRect || !g_deleteObject)
        return;
    const COLORREF colors[] = { RGB(10, 10, 13), RGB(16, 16, 20), RGB(23, 23, 28) };
    for (int i = 0; i < 3; ++i)
    {
        int spread = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int offset = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int depth = ScaleLayoutPx(static_cast<float>(3 + i * 2));
        RECT shadow = { objectRect.left - spread,
                        objectRect.bottom - 1 + offset,
                        objectRect.right + spread,
                        objectRect.bottom + offset + depth };
        HBRUSH brush = g_createSolidBrush(colors[i]);
        HGDIOBJ oldBrush = g_selectObject(hdc, brush);
        HGDIOBJ oldPen = g_selectObject(hdc, g_getStockObject(NULL_PEN));
        g_roundRect(hdc, shadow.left, shadow.top, shadow.right, shadow.bottom,
                    radius + spread, radius + spread);
        g_selectObject(hdc, oldPen);
        g_selectObject(hdc, oldBrush);
        g_deleteObject(brush);
    }
}

static void UnregisterThumbnailSlot(int slot)
{
    if (slot < 0 || slot >= kMaxCarouselSlots)
        return;
    if (g_thumbnailSlots[slot].thumbnail && g_dwmUnregisterThumbnail)
        g_dwmUnregisterThumbnail(g_thumbnailSlots[slot].thumbnail);
    g_thumbnailSlots[slot].thumbnail = nullptr;
    g_thumbnailSlots[slot].source = nullptr;
    g_thumbnailSlots[slot].sourceWidth = 0;
    g_thumbnailSlots[slot].sourceHeight = 0;
    g_thumbnailSlots[slot].lastDestination = {};
    g_thumbnailSlots[slot].lastSource = {};
    g_thumbnailSlots[slot].lastOpacity = 0;
    g_thumbnailSlots[slot].lastVisible = FALSE;
    g_thumbnailSlots[slot].hasProperties = false;
}

static void UnregisterAllThumbnails()
{
    for (int i = 0; i < kMaxCarouselSlots; ++i)
        UnregisterThumbnailSlot(i);
}

static void UpdateThumbnailProperties(int slot, const RECT& area, HWND source)
{
    if (slot < 0 || slot >= kMaxCarouselSlots ||
        !g_thumbnailSlots[slot].thumbnail || !source ||
        !g_dwmUpdateThumbnailProperties)
        return;

    ThumbnailSlot& slotState = g_thumbnailSlots[slot];
    int sourceWidth = slotState.sourceWidth;
    int sourceHeight = slotState.sourceHeight;
    if (slotState.source != source || sourceWidth <= 0 || sourceHeight <= 0)
    {
        sourceWidth = 0;
        sourceHeight = 0;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
    if (IsIconic(source))
    {
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(WINDOWPLACEMENT);
        if (GetWindowPlacement(source, &wp))
        {
            sourceWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
            sourceHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
        }
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        RECT sourceRect = {};
        GetClientRect(source, &sourceRect);
        sourceWidth = sourceRect.right - sourceRect.left;
        sourceHeight = sourceRect.bottom - sourceRect.top;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        RECT windowRect = {};
        GetWindowRect(source, &windowRect);
        sourceWidth = windowRect.right - windowRect.left;
        sourceHeight = windowRect.bottom - windowRect.top;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        sourceWidth = 1600;
        sourceHeight = 1000;
    }
    }
    slotState.sourceWidth = sourceWidth;
    slotState.sourceHeight = sourceHeight;

    DwmThumbnailPropertiesLocal properties = {};
    properties.dwFlags = kDwmTnpRectDestination | kDwmTnpOpacity |
                         kDwmTnpVisible;
    RECT destination = ContainedRect(area, sourceWidth, sourceHeight);
    RECT viewport = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
    RECT clipped = {
        std::max(destination.left, viewport.left),
        std::max(destination.top, viewport.top),
        std::min(destination.right, viewport.right),
        std::min(destination.bottom, viewport.bottom)
    };
    bool visible = clipped.left < clipped.right && clipped.top < clipped.bottom;
    properties.rcDestination = clipped;
    if (!visible)
        properties.rcDestination = RECT{ 0, 0, 0, 0 };
    else
    {
        // Crop the matching source pixels when a card crosses the selector
        // edge. Without rcSource, DWM scales the complete source into the
        // remaining destination rectangle and deforms the preview.
        int destinationWidth = destination.right - destination.left;
        int destinationHeight = destination.bottom - destination.top;
        RECT sourceClip = {
            static_cast<LONG>((static_cast<long long>(clipped.left - destination.left) * sourceWidth) /
                              destinationWidth),
            static_cast<LONG>((static_cast<long long>(clipped.top - destination.top) * sourceHeight) /
                              destinationHeight),
            static_cast<LONG>((static_cast<long long>(clipped.right - destination.left) * sourceWidth) /
                              destinationWidth),
            static_cast<LONG>((static_cast<long long>(clipped.bottom - destination.top) * sourceHeight) /
                              destinationHeight)
        };
        sourceClip.left = std::max<LONG>(0, std::min<LONG>(sourceWidth, sourceClip.left));
        sourceClip.top = std::max<LONG>(0, std::min<LONG>(sourceHeight, sourceClip.top));
        sourceClip.right = std::max<LONG>(sourceClip.left + 1,
                                          std::min<LONG>(sourceWidth, sourceClip.right));
        sourceClip.bottom = std::max<LONG>(sourceClip.top + 1,
                                           std::min<LONG>(sourceHeight, sourceClip.bottom));
        properties.dwFlags |= kDwmTnpRectSource;
        properties.rcSource = sourceClip;
    }
    properties.fVisible = visible ? TRUE : FALSE;
    // Previews DWM siempre nítidas; la translucidez solo pertenece a la card.
    float thumbnailOpacity = g_sceneOpacity;
    if (g_cardExitActive && slot == g_cardExitSlot)
        thumbnailOpacity *= GetCardExitOpacity();
    properties.opacity = static_cast<BYTE>(std::max(0, std::min(255,
        static_cast<int>(roundf(thumbnailOpacity * 255.0f)))));

    // UpdateThumbnailProperties puede ejecutarse en cada frame de una animación.
    // DWM no necesita recibir otra orden si geometría, visibilidad y opacidad no cambiaron.
    bool unchanged = slotState.hasProperties &&
        slotState.lastVisible == properties.fVisible &&
        slotState.lastOpacity == properties.opacity &&
        std::memcmp(&slotState.lastDestination, &properties.rcDestination, sizeof(RECT)) == 0 &&
        std::memcmp(&slotState.lastSource, &properties.rcSource, sizeof(RECT)) == 0;
    if (unchanged)
        return;
    g_dwmUpdateThumbnailProperties(slotState.thumbnail, &properties);
    slotState.lastDestination = properties.rcDestination;
    slotState.lastSource = properties.rcSource;
    slotState.lastOpacity = properties.opacity;
    slotState.lastVisible = properties.fVisible;
    slotState.hasProperties = true;
}

static void UpdateThumbnailSlots()
{
    if (!g_selector || !LoadDwmFunctions())
        return;

    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
    {
        UnregisterAllThumbnails();
        return;
    }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
    {
        int index = ResolveGroupIndex(slot);
        HWND source = (index >= 0) ? g_groups[index].representativeWindow : nullptr;
        if (!source || !IsWindow(source))
        {
            UnregisterThumbnailSlot(slot);
            continue;
        }

        if (g_thumbnailSlots[slot].source != source)
        {
            UnregisterThumbnailSlot(slot);
            HTHUMBNAIL thumbnail = nullptr;
            if (SUCCEEDED(g_dwmRegisterThumbnail(g_selector, source, &thumbnail)))
            {
                g_thumbnailSlots[slot].thumbnail = thumbnail;
                g_thumbnailSlots[slot].source = source;
            }
        }

        RECT area = TransformSceneRect(GetSlotPreviewRect(slot));
        UpdateThumbnailProperties(slot, area, source);
    }
}

static void StopCarouselAnimation()
{
    g_animActive = false;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_animStartTime = 0.0;
    if (g_selector && IsWindow(g_selector))
    {
        KillTimer(g_selector, kAnimTimerId);
        KillTimer(g_selector, kTabRepeatTimerId);
        DisarmFramePump();
    }
    g_tabRepeatStarted = false;
    g_selectorAnimation = SelectorAnimationState::None;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    g_sceneOpacity = 1.0f;
    g_scenePresence = 1.0f;
    g_carouselSpread = 1.0f;
    g_selectionTiltDirection = 0.0f;
    g_cardSnapScale = 1.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitGroupIndex = -1;
    g_cardExitTarget = nullptr;
}

static void StartSelectorClose(HWND target)
{
    if (!g_selector || !IsWindow(g_selector) ||
        g_selectorAnimation == SelectorAnimationState::Closing)
        return;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitGroupIndex = -1;
    g_cardExitTarget = nullptr;
    g_pendingActivationTarget = target;
    // El cierre parte del progreso actual de la escena (incluso si la apertura
    // aún no terminó), así que no hay saltos de escala u opacidad.
    g_closeStartPresence = g_scenePresence;
    g_selectorAnimation = SelectorAnimationState::Closing;
    g_selectorAnimationStart = FrameClockNow();
    RequestFrame();
}

// Único punto donde se traduce el progreso de escena a escala, opacidad y
// apertura del carrusel. Apertura y cierre solo difieren en cómo evoluciona
// "presence", por lo que convergen exactamente al mismo estado.
static void ApplyScenePresence(float presence)
{
    if (g_animationStyle == AnimationStyle::Rct)
    {
        // CRT: al cerrar, primero colapsa en vertical hacia una línea y luego
        // en horizontal hacia un punto, con la opacidad bajando a 0. Al abrir
        // es la transición inversa (el mismo mapeo recorrido al revés).
        presence = Clamp01(presence);
        g_scenePresence = presence;
        const float c = 1.0f - presence;
        const float vertical = std::min(1.0f, c / 0.72f);
        const float horizontal = std::max(0.0f, (c - 0.72f) / 0.28f);
        g_sceneScaleY = 1.0f - 0.97f * vertical * vertical * vertical;
        g_sceneScaleX = 1.0f - 0.92f * horizontal * horizontal;
        g_sceneOpacity = presence;
        g_carouselSpread = 1.0f;
        return;
    }

    // Linear y Bouncy comparten el mapeo. Bouncy empieza más pequeño y deja
    // que "presence" supere 1.0 un instante (overshoot), de modo que escala,
    // opacidad y apertura del carrusel siguen un único progreso.
    const bool bouncy = g_animationStyle == AnimationStyle::Bouncy;
    presence = bouncy ? std::max(0.0f, presence) : Clamp01(presence);
    g_scenePresence = presence;
    const float startScale = bouncy ? 0.88f : 0.92f;
    const float scale = startScale + (1.0f - startScale) * presence;
    g_sceneScaleX = scale;
    g_sceneScaleY = scale;
    g_sceneOpacity = EaseSmooth(std::min(1.0f, presence) / 0.45f);
    g_carouselSpread = 0.96f + 0.04f * presence;
}

// Resorte subamortiguado normalizado (0 -> 1) para la apertura Bouncy:
// zeta 0.5, omega 32 rad/s, velocidad inicial 12/s sobre 0.30 s de tiempo de
// resorte. Overshoot de ~18 %, un único retroceso de ~3 % y asentamiento
// dentro del 0.3 % al final; en t=1 se fuerza exactamente 1.0.
static float SpringOvershoot(float t)
{
    if (t >= 1.0f)
        return 1.0f;
    const float zeta = 0.5f, omega = 32.0f, v0 = 12.0f, span = 0.30f;
    const float wd = omega * sqrtf(1.0f - zeta * zeta);
    const float tau = std::max(0.0f, t) * span;
    const float e = expf(-zeta * omega * tau) *
                    (cosf(wd * tau) + ((zeta * omega - v0) / wd) * sinf(wd * tau));
    return 1.0f - e;
}

static float OpenDurationMs()
{
    switch (g_animationStyle)
    {
    case AnimationStyle::Bouncy: return 290.0f;  // Linear 200 ms + 90 ms
    case AnimationStyle::Rct: return 320.0f;
    default: return 200.0f;
    }
}

static float CloseDurationMs()
{
    return g_animationStyle == AnimationStyle::Rct ? 320.0f : 150.0f;
}

static float OpenPresenceAt(float t)
{
    switch (g_animationStyle)
    {
    case AnimationStyle::Bouncy: return SpringOvershoot(t);
    case AnimationStyle::Rct: return t;
    default: return EaseOutExp(t, 5.0f);
    }
}

// Fracción de presencia que queda al cerrar (1 -> 0).
static float CloseRemainingAt(float t)
{
    return g_animationStyle == AnimationStyle::Rct
        ? 1.0f - t
        : 1.0f - EaseOutCubic(t);
}

static float GetCardExitProgress()
{
    // Se evalúa una vez por frame en UpdateSelectorMotion; paint y thumbnails
    // leen el mismo valor.
    return g_cardExitActive ? g_cardExitProgress : 0.0f;
}

static float GetCardExitOpacity()
{
    float t = GetCardExitProgress();
    float fadeT = std::min(1.0f, t / 0.72f);
    float eased = fadeT * fadeT * (3.0f - 2.0f * fadeT);
    return 1.0f - eased;
}

static float GetCardExitOffsetY()
{
    if (!g_cardExitActive)
        return 0.0f;
    float t = GetCardExitProgress();
    return g_cardExitDirection * ScaleLayoutPx(250.0f) * t * t * t;
}

static void UpdateSelectorMotion()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    if (g_selectorAnimation == SelectorAnimationState::Open && !g_animActive)
        return;
    if (g_selectorAnimation == SelectorAnimationState::CardExit)
    {
        g_cardExitProgress = ElapsedFraction(g_cardExitStart, AnimationDuration(360.0f));
        if (g_cardExitProgress >= 1.0f)
        {
            HWND target = g_cardExitTarget;
            g_cardExitActive = false;
            g_cardExitProgress = 0.0f;
            g_cardExitSlot = -1;
            g_cardExitGroupIndex = -1;
            g_cardExitTarget = nullptr;
            g_selectorAnimation = SelectorAnimationState::Open;
            if (target && IsWindow(target) && PostMessageW(target, WM_CLOSE, 0, 0))
                MarkWindowPendingClose(target);
            for (size_t i = 0; i < g_userWindows.size(); ++i)
            {
                if (g_userWindows[i].hwnd == target)
                {
                    g_userWindows.erase(g_userWindows.begin() + i);
                    break;
                }
            }
            RefreshWindowList();
            if (g_groups.empty())
            {
                LeaveActiveState(SelectorState::Canceling);
                StartSelectorClose(nullptr);
                return;
            }
            if (g_selected >= static_cast<int>(g_groups.size()))
                g_selected = static_cast<int>(g_groups.size()) - 1;
            g_hoveredSlot = -1;
            g_hoveredCloseButton = false;
            UpdateSelectorControls();
            return;
        }
        UpdateSelectorControls();
        return;
    }
    if (g_selectorAnimation == SelectorAnimationState::Opening)
    {
        // Una sola curva de salida exponencial: respuesta inmediata, sin
        // overshoot y asentamiento limpio. Fondo, cards y opacidad salen de
        // ApplyScenePresence con el mismo progreso.
        const float t = ElapsedFraction(g_selectorAnimationStart,
                                        AnimationDuration(OpenDurationMs()));
        ApplyScenePresence(OpenPresenceAt(t));
        if (t >= 1.0f)
        {
            ApplyScenePresence(1.0f);
            g_selectorAnimation = SelectorAnimationState::Open;
        }
    }
    else if (g_selectorAnimation == SelectorAnimationState::SelectionChange)
    {
        // La navegación se anima exclusivamente mediante g_animOffset; la
        // superficie permanece fija.
        ApplyScenePresence(1.0f);
        if (ElapsedFraction(g_selectorAnimationStart,
                            AnimationDuration(kNavigationDuration)) >= 1.0f)
            g_selectorAnimation = SelectorAnimationState::Open;
    }
    else if (g_selectorAnimation == SelectorAnimationState::Closing)
    {
        // Inverso de la apertura: la escena converge al mismo estado inicial
        // (escala reducida, opacidad 0, cards recogidas) con salida inmediata.
        const float t = ElapsedFraction(g_selectorAnimationStart,
                                        AnimationDuration(CloseDurationMs()));
        ApplyScenePresence(g_closeStartPresence * CloseRemainingAt(t));
        if (t >= 1.0f)
        {
            HWND target = g_pendingActivationTarget;
            g_pendingActivationTarget = nullptr;
            g_sceneScaleX = 1.0f;
            g_sceneScaleY = 1.0f;
            g_selectorAnimation = SelectorAnimationState::None;
            g_cleanupInProgress = true;
            DestroySelector();
            g_state = SelectorState::Idle;
            g_cleanupInProgress = false;
            CleanupKeyboardState();
            ActivateWindow(target);
            return;
        }
    }
    UpdateSelectorSurfaceTransform();
    UpdateSelectorControls();
}

// "La escena cambió": solo marca el frame como sucio. El render ocurre una vez
// por frame en RunFrame, nunca de forma síncrona por cada cambio de estado.
static void UpdateSelectorControls()
{
    if (!g_selector || !IsWindow(g_selector))
        return;

    CreateUiFonts();
    RequestFrame();
}

// FRAME PUMP
//
// Sustituye al SetTimer (resolución ~15.6 ms, jitter alto) como reloj de
// animación. Un waitable timer de alta resolución despierta al UI thread justo
// después de un vblank de DWM; la animación se evalúa en el instante previsto
// de presentación (el siguiente vblank), así que el espaciado entre muestras es
// constante aunque el timer tenga jitter. Si DWM no ofrece timing, se usa un
// intervalo fijo. Sin timer de alta resolución se usa SetTimer como último
// recurso. Cuando no hay animación ni cambios pendientes el pump no se arma y
// el hilo duerme en MsgWaitForMultipleObjectsEx.
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

static HANDLE g_frameTimer = nullptr;
static bool g_framePumpArmed = false;
static bool g_frameDirty = false;
static double g_lastSampleMs = 0.0;
static double g_lastWakeMs = 0.0;

static bool InitFramePump()
{
    if (g_frameTimer)
        return true;
    g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr,
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!g_frameTimer)
        g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    QpcNowMs();
    return g_frameTimer != nullptr;
}

static void ShutdownFramePump()
{
    DisarmFramePump();
    if (g_frameTimer)
    {
        CloseHandle(g_frameTimer);
        g_frameTimer = nullptr;
    }
}

static bool QueryVBlank(double& vblankMs, double& periodMs)
{
    if (!g_dwmTimingResolved)
    {
        g_dwmTimingResolved = true;
        if (LoadDwmFunctions() && g_dwmApi)
            g_dwmGetCompositionTimingInfo =
                reinterpret_cast<DwmGetCompositionTimingInfoFn>(
                    GetProcAddress(g_dwmApi, "DwmGetCompositionTimingInfo"));
    }
    if (!g_dwmGetCompositionTimingInfo || !g_selector)
        return false;
    DwmTimingInfoLocal info = {};
    info.cbSize = sizeof(info);
    if (FAILED(g_dwmGetCompositionTimingInfo(g_selector, &info)))
        return false;
    const double toMs = 1000.0 / static_cast<double>(g_qpcFrequency > 0 ? g_qpcFrequency : 1);
    periodMs = static_cast<double>(info.qpcRefreshPeriod) * toMs;
    vblankMs = static_cast<double>(info.qpcVBlank) * toMs;
    return info.qpcVBlank != 0 && periodMs > 3.0 && periodMs < 40.0;
}

// Devuelve el instante de muestreo de la animación (presentación prevista) y,
// opcionalmente, cuándo hay que despertar para renderizarlo.
static double NextSampleTimeMs(double now, double* wakeMs)
{
    double vblank = 0.0, period = 0.0;
    if (QueryVBlank(vblank, period))
    {
        // "Balanced" renderiza cada N vblanks para no exceder el objetivo; el
        // paso entero mantiene un espaciado uniforme (p. ej. 144 Hz -> 72 Hz).
        const double refreshHz = 1000.0 / period;
        const int step = std::max(1, static_cast<int>(
            floor(refreshHz / static_cast<double>(g_animationFps) + 0.5)));
        const double minimum = std::max(now + 1.0,
            g_lastSampleMs + (static_cast<double>(step) - 0.5) * period);
        const double sample = vblank + ceil((minimum - vblank) / period) * period;
        if (wakeMs)
            *wakeMs = std::max(now, sample - period + 1.0);
        return sample;
    }
    const double interval = 1000.0 / static_cast<double>(g_animationFps);
    if (wakeMs)
        *wakeMs = std::max(now, g_lastWakeMs + interval);
    return now;
}

static void ArmFramePump()
{
    if (g_framePumpArmed || !g_selector)
        return;
    const double now = QpcNowMs();
    double wake = now;
    NextSampleTimeMs(now, &wake);
    const double delay = std::max(0.0, wake - now);
    g_framePumpArmed = true;
    if (g_frameTimer)
    {
        LARGE_INTEGER due = {};
        due.QuadPart = -static_cast<LONGLONG>(delay * 10000.0);
        if (SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr, FALSE))
            return;
    }
    SetTimer(g_selector, kSelectorMotionTimerId,
             std::max<UINT>(1, static_cast<UINT>(delay)), nullptr);
}

static void DisarmFramePump()
{
    g_framePumpArmed = false;
    g_frameDirty = false;
    if (g_frameTimer)
        CancelWaitableTimer(g_frameTimer);
    if (g_selector && IsWindow(g_selector))
        KillTimer(g_selector, kSelectorMotionTimerId);
}

static void RequestFrame()
{
    if (!g_selector || !IsWindowVisible(g_selector))
        return;
    g_frameDirty = true;
    if (!g_inFrame && !g_framePumpArmed)
        ArmFramePump();
}

static bool SceneIsAnimating()
{
    return g_animActive ||
           g_selectorAnimation == SelectorAnimationState::Opening ||
           g_selectorAnimation == SelectorAnimationState::SelectionChange ||
           g_selectorAnimation == SelectorAnimationState::CardExit ||
           g_selectorAnimation == SelectorAnimationState::Closing;
}

static void RunFrame()
{
    if (!g_framePumpArmed)
        return;  // disparo obsoleto de un pump ya desarmado
    g_framePumpArmed = false;
    if (!g_selector || !IsWindow(g_selector) || !IsWindowVisible(g_selector))
    {
        g_frameDirty = false;
        return;
    }

    const double now = QpcNowMs();
    g_lastWakeMs = now;
    g_frameTimeMs = NextSampleTimeMs(now, nullptr);
    g_lastSampleMs = g_frameTimeMs;

    // Un solo instante para toda la escena: apertura/cierre, carrusel y salida
    // de cards se evalúan con el mismo reloj.
    g_inFrame = true;
    UpdateSelectorMotion();
    if (g_selector && IsWindow(g_selector))
        UpdateCarouselAnimation(g_selector);
    g_inFrame = false;

    if (!g_selector || !IsWindow(g_selector) || !IsWindowVisible(g_selector))
    {
        g_frameDirty = false;
        return;
    }

    if (g_frameDirty)
    {
        g_frameDirty = false;
        // Thumbnails DWM y superficie D2D se actualizan juntos, con el mismo
        // estado de escena, dentro del mismo frame.
        UpdateThumbnailSlots();
        InvalidateRect(g_selector, nullptr, FALSE);
        UpdateWindow(g_selector);
    }

    if (SceneIsAnimating() || g_frameDirty)
        ArmFramePump();
}

static void EnsureIconBitmap(int groupIndex)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()))
        return;
    if (!g_d2dDCRenderTarget)
        return;
    if (g_groups[groupIndex].icon && !g_groups[groupIndex].iconBitmap)
        g_groups[groupIndex].iconBitmap = CreateD2DBitmapFromHIcon(
            g_d2dDCRenderTarget, g_groups[groupIndex].icon);
}

static ID2D1LinearGradientBrush* EnsureSurfaceGradientBrush(int groupIndex,
                                                            bool selected)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()) ||
        !g_d2dDCRenderTarget)
        return nullptr;

    AppGroup& group = g_groups[groupIndex];
    ID2D1LinearGradientBrush*& brush = selected
        ? group.surfaceBrushSelected : group.surfaceBrushNormal;
    if (brush)
        return brush;

    COLORREF cardColor = selected ? g_cardSelectedColor : g_cardNormalColor;
    D2D1_COLOR_F base = D2D1::ColorF(
        GetRValue(cardColor) / 255.0f,
        GetGValue(cardColor) / 255.0f,
        GetBValue(cardColor) / 255.0f,
        kCardSurfaceOpacity);
    // Columna ambiental horizontalmente modulada: el centro recibe una
    // influencia moderada y los extremos recuperan Black Obsidian.
    D2D1_COLOR_F edge = base;
    D2D1_COLOR_F soft = AddMediaAccent(base, group.mediaAccent,
                                         selected ? 0.145f : 0.115f);
    D2D1_COLOR_F center = AddMediaAccent(base, group.mediaAccent,
                                           selected ? 0.300f : 0.235f);

    D2D1_GRADIENT_STOP stops[5] = {};
    stops[0].position = 0.0f;
    stops[0].color = edge;
    stops[1].position = 0.22f;
    stops[1].color = soft;
    stops[2].position = 0.50f;
    stops[2].color = center;
    stops[3].position = 0.78f;
    stops[3].color = soft;
    stops[4].position = 1.0f;
    stops[4].color = edge;

    ID2D1GradientStopCollection* stopCollection = nullptr;
    HRESULT hr = g_d2dDCRenderTarget->CreateGradientStopCollection(
        stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.startPoint = D2D1::Point2F(0.0f, 0.5f);
    properties.endPoint = D2D1::Point2F(1.0f, 0.5f);
    hr = g_d2dDCRenderTarget->CreateLinearGradientBrush(
        properties, stopCollection, &brush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? brush : nullptr;
}

static ID2D1RadialGradientBrush* EnsureCloseAccentBrush()
{
    if (g_closeAccentBrush || !g_d2dDCRenderTarget)
        return g_closeAccentBrush;

    D2D1_GRADIENT_STOP stops[3] = {};
    stops[0].position = 0.0f;
    stops[0].color = D2D1::ColorF(0.48f, 0.055f, 0.065f, 0.42f);
    stops[1].position = 0.38f;
    stops[1].color = D2D1::ColorF(0.30f, 0.030f, 0.040f, 0.20f);
    stops[2].position = 1.0f;
    stops[2].color = D2D1::ColorF(0.18f, 0.015f, 0.020f, 0.0f);

    ID2D1GradientStopCollection* stopCollection = nullptr;
    HRESULT hr = g_d2dDCRenderTarget->CreateGradientStopCollection(
        stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.center = D2D1::Point2F(0.5f, 0.5f);
    properties.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    properties.radiusX = 1.0f;
    properties.radiusY = 1.0f;
    hr = g_d2dDCRenderTarget->CreateRadialGradientBrush(
        properties, stopCollection, &g_closeAccentBrush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? g_closeAccentBrush : nullptr;
}

static void PaintSlotHeaderD2D(int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    const std::wstring& text = MakeSlotDisplayText(group, selected, slot);

    int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 20 : (distFromCenter < 1.5f ? 18 : 16)));
    int paddingLeft = ScaleLayoutPx(static_cast<float>(selected ? 10 : (distFromCenter < 1.5f ? 8 : 6)));
    int iconX = rc.left + paddingLeft;
    int iconY = rc.top + (rc.bottom - rc.top - iconSize) / 2;
    int textX = iconX + iconSize + ScaleLayoutPx(8.0f);
    RECT iconRect = { iconX, iconY, iconX + iconSize, iconY + iconSize };
    if (slot == GetCarouselCenterSlot())
    {
        RECT cardRect = GetSlotCardRect(slot);
        float pivotX = (static_cast<float>(cardRect.left) + cardRect.right) * 0.5f;
        float pivotY = (static_cast<float>(cardRect.top) + cardRect.bottom) * 0.5f;
        iconRect = ScaleRectAroundPoint(iconRect, g_cardSnapScale, pivotX, pivotY);
    }

    // El encabezado es deliberadamente transparente: no crea una superficie
    // oscura independiente sobre el gradiente Dynamic Obsidian de la card.
    // El icono y el texto se dibujan directamente sobre la superficie iluminada.

    EnsureIconBitmap(groupIndex);
    if (group.iconBitmap)
    {
        D2D1_RECT_F iconDest = D2D1::RectF(
            static_cast<float>(iconRect.left),
            static_cast<float>(iconRect.top),
            static_cast<float>(iconRect.right),
            static_cast<float>(iconRect.bottom));
        g_d2dDCRenderTarget->DrawBitmap(
            group.iconBitmap, &iconDest, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    IDWriteTextFormat* format = selected ? g_dwriteSelectedFormat : g_dwriteNormalFormat;
    if (format)
    {
        D2D1_RECT_F textRectD2D = D2D1::RectF(
            static_cast<float>(textX), static_cast<float>(rc.top),
            static_cast<float>(rc.right - ScaleLayoutPx(8.0f)), static_cast<float>(rc.bottom));
        D2D1_COLOR_F textColor = selected
            ? D2D1::ColorF(0.965f, 0.968f, 0.98f, 1.0f)
            : D2D1::ColorF(0.815f, 0.823f, 0.855f, 1.0f);
        g_d2dBrush->SetColor(textColor);
        g_d2dDCRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
                                      format, &textRectD2D, g_d2dBrush);
    }
}

static void PaintCounterD2D()
{
    RECT rc = GetCounterRect();
    const std::wstring& text = MakeCounterText();

    // El contador no tiene superficie propia: el texto flota directamente
    // sobre la misma card translúcida, sin cápsula, rectángulo ni borde.

    if (g_dwriteCounterFormat)
    {
        D2D1_RECT_F textRectD2D = D2D1::RectF(
            static_cast<float>(rc.left), static_cast<float>(rc.top),
            static_cast<float>(rc.right), static_cast<float>(rc.bottom));
        g_d2dBrush->SetColor(D2D1::ColorF(0.73f, 0.74f, 0.77f, 1.0f));
        g_d2dDCRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
                                      g_dwriteCounterFormat, &textRectD2D, g_d2dBrush);
    }
}

static void PaintSlotHeaderGDI(HDC hdc, int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    const std::wstring& text = MakeSlotDisplayText(group, selected, slot);

    int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 20 : (distFromCenter < 1.5f ? 18 : 16)));
    int paddingLeft = ScaleLayoutPx(static_cast<float>(selected ? 10 : (distFromCenter < 1.5f ? 8 : 6)));
    int iconX = rc.left + paddingLeft;
    int iconY = rc.top + (rc.bottom - rc.top - iconSize) / 2;
    int textX = iconX + iconSize + ScaleLayoutPx(8.0f);
    RECT iconRect = { iconX, iconY, iconX + iconSize, iconY + iconSize };
    if (slot == GetCarouselCenterSlot())
    {
        RECT cardRect = GetSlotCardRect(slot);
        float pivotX = (static_cast<float>(cardRect.left) + cardRect.right) * 0.5f;
        float pivotY = (static_cast<float>(cardRect.top) + cardRect.bottom) * 0.5f;
        iconRect = ScaleRectAroundPoint(iconRect, g_cardSnapScale, pivotX, pivotY);
    }

    // Sin panel, relleno ni borde propio: el encabezado deja ver la card.

    RECT textRect = rc;
    textRect.left = textX;
    textRect.right = rc.right - ScaleLayoutPx(8.0f);
    HFONT font = selected ? g_selectedNameFont : g_nameFont;
    COLORREF textColor = selected ? RGB(246, 247, 250) : RGB(208, 210, 218);
    if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
    if (g_setTextColor) g_setTextColor(hdc, textColor);
    HGDIOBJ oldFont = font && g_selectObject ? g_selectObject(hdc, font) : nullptr;
    DrawTextW(hdc, text.c_str(), -1, &textRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);

    if (group.icon)
    {
        DrawIconEx(hdc, iconRect.left, iconRect.top, group.icon,
                   iconRect.right - iconRect.left, iconRect.bottom - iconRect.top,
                   0, nullptr, DI_NORMAL);
    }
}

static void PaintCounterGDI(HDC hdc)
{
    RECT rc = GetCounterRect();
    const std::wstring& text = MakeCounterText();

    // GDI no ofrece alpha blending con el pincel sólido usado por este
    // fallback; no dibujamos una superficie opaca independiente detrás del texto.
    if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
    if (g_setTextColor) g_setTextColor(hdc, RGB(186, 188, 196));
    HGDIOBJ oldFont = g_secondaryFont && g_selectObject ?
        g_selectObject(hdc, g_secondaryFont) : nullptr;
    DrawTextW(hdc, text.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);
}

static void PaintCenterAccentMarginD2D(const RECT& cardRect, int groupIndex, float radius)
{
    if (!g_d2dDCRenderTarget || !g_d2dBrush || groupIndex < 0 ||
        groupIndex >= static_cast<int>(g_groups.size()))
        return;

    COLORREF accent = g_groups[groupIndex].mediaAccent.valid
        ? g_groups[groupIndex].mediaAccent.color
        : RGB(18, 22, 28);
    float r = GetRValue(accent) / 255.0f;
    float g = GetGValue(accent) / 255.0f;
    float b = GetBValue(accent) / 255.0f;

    const float margin = static_cast<float>(ScaleLayoutPx(5.0f));
    D2D1_ROUNDED_RECT outer = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(cardRect.left) - margin,
                    static_cast<float>(cardRect.top) - margin,
                    static_cast<float>(cardRect.right) + margin,
                    static_cast<float>(cardRect.bottom) + margin),
        radius + margin, radius + margin);

    // Marco vectorial: no es un bitmap ni una sombra rasterizada. Sigue
    // exactamente el contorno redondeado de la card central y toma el color
    // dominante extraído de su contenido.
    g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, 0.34f));
    g_d2dDCRenderTarget->DrawRoundedRectangle(&outer, g_d2dBrush,
                                               static_cast<float>(ScaleLayoutPx(2.0f)));

    D2D1_ROUNDED_RECT inner = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(cardRect.left) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.top) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.right) + ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.bottom) + ScaleLayoutPx(2.0f)),
        radius + ScaleLayoutPx(2.0f), radius + ScaleLayoutPx(2.0f));
    g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, 0.18f));
    g_d2dDCRenderTarget->DrawRoundedRectangle(&inner, g_d2dBrush,
                                               static_cast<float>(ScaleLayoutPx(1.0f)));
}

static void ResetPatternWaves()
{
    ZeroMemory(g_patternRippleA, sizeof(g_patternRippleA));
    ZeroMemory(g_patternRippleB, sizeof(g_patternRippleB));
    ZeroMemory(g_patternRippleC, sizeof(g_patternRippleC));
    g_patternRippleCurrent = g_patternRippleA;
    g_patternRipplePrevious = g_patternRippleB;
    g_patternRippleNext = g_patternRippleC;
    g_patternHasCursor = false;
    g_patternCursorInside = false;
    ZeroMemory(g_patternCombinedFieldCache, sizeof(g_patternCombinedFieldCache));
    g_patternLastTick = GetTickCount64();
    g_patternQualityScale = 1.0f;
    g_patternQualityCooldownUntil = 0;
    g_patternSlowSamples = 0;
    g_patternFastSamples = 0;
}

static int PatternFieldWrappedIndex(int x, int y)
{
    x %= kPatternFieldWidth;
    y %= kPatternFieldHeight;
    if (x < 0) x += kPatternFieldWidth;
    if (y < 0) y += kPatternFieldHeight;
    return y * kPatternFieldWidth + x;
}

static float SmoothStep01(float value)
{
    value = std::max(0.0f, std::min(1.0f, value));
    return value * value * (3.0f - 2.0f * value);
}

static float WrapPatternCoordinate(float pixel, float extent, int fieldSize)
{
    if (extent <= 0.0f || fieldSize <= 1)
        return 0.0f;
    float period = static_cast<float>(fieldSize - 1);
    float coordinate = pixel * period / extent;
    coordinate = fmodf(coordinate, period);
    if (coordinate < 0.0f)
        coordinate += period;
    return coordinate;
}

static void AddPatternFieldImpulse(float pixelX, float pixelY, float amplitude)
{
    if (g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return;
    float fx = pixelX * (kPatternFieldWidth - 1) /
               static_cast<float>(g_runtimeSelectorWidth);
    float fy = pixelY * (kPatternFieldHeight - 1) /
               static_cast<float>(g_runtimeSelectorHeight);
    int centerX = static_cast<int>(roundf(fx));
    int centerY = static_cast<int>(roundf(fy));
    for (int oy = -2; oy <= 2; ++oy)
    {
        for (int ox = -2; ox <= 2; ++ox)
        {
            float distance = sqrtf(static_cast<float>(ox * ox + oy * oy));
            if (distance > 2.2f)
                continue;
            float weight = 1.0f - distance / 2.6f;
            g_patternRippleCurrent[PatternFieldWrappedIndex(centerX + ox,
                                                              centerY + oy)] +=
                amplitude * weight;
        }
    }
}

static void UpdatePatternCursor(POINT point)
{
    const bool inside = IsPointInsidePatternArea(point);
    if (!g_patternHasCursor)
    {
        g_patternCursor = point;
        g_patternPreviousCursor = point;
        g_patternHasCursor = true;
        g_patternCursorInside = inside;
        return;
    }
    const bool wasInside = g_patternCursorInside;
    float dx = static_cast<float>(point.x - g_patternCursor.x);
    float dy = static_cast<float>(point.y - g_patternCursor.y);
    g_patternPreviousCursor = g_patternCursor;
    g_patternCursor = point;
    g_patternCursorInside = inside;
    if (inside && wasInside && IsPatternWavesActive() &&
        g_patternCursorInteraction && (dx * dx + dy * dy) > 1.0f)
    {
        float distance = sqrtf(dx * dx + dy * dy);
        float velocity = std::min(1.8f, distance / 42.0f);
        float impulse = velocity * g_patternCursorStrength *
                        (0.40f + g_patternWaveStrength * 0.30f);
        int segments = std::max(1, std::min(8, static_cast<int>(distance / 18.0f)));
        for (int i = 1; i <= segments; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(segments);
            float x = static_cast<float>(g_patternPreviousCursor.x) + dx * t;
            float y = static_cast<float>(g_patternPreviousCursor.y) + dy * t;
            AddPatternFieldImpulse(x, y, impulse * (1.0f - 0.10f * t));
        }
    }
}

static void UpdatePatternWaves()
{
    if (!g_selector || !IsWindow(g_selector) || !IsPatternWavesActive())
        return;
    ULONGLONG now = GetTickCount64();
    float dt = g_patternLastTick == 0 ? 0.016f :
        std::min(0.05f, static_cast<float>(now - g_patternLastTick) / 1000.0f);
    g_patternLastTick = now;

    // Only cursor energy is simulated here. The base surface is procedural and
    // sampled separately, so every ripple naturally returns to that base.
    int steps = dt > 0.026f ? 2 : 1;
    float propagation = std::max(0.10f, std::min(0.42f,
        0.18f + g_patternWaveStrength * 0.16f));
    float damping = powf(0.985f, 60.0f * dt / static_cast<float>(steps));
    for (int step = 0; step < steps; ++step)
    {
        for (int y = 0; y < kPatternFieldHeight; ++y)
        {
            for (int x = 0; x < kPatternFieldWidth; ++x)
            {
                int index = y * kPatternFieldWidth + x;
                int left = PatternFieldWrappedIndex(x - 1, y);
                int right = PatternFieldWrappedIndex(x + 1, y);
                int up = PatternFieldWrappedIndex(x, y - 1);
                int down = PatternFieldWrappedIndex(x, y + 1);
                float center = g_patternRippleCurrent[index];
                float neighbors = g_patternRippleCurrent[left] +
                    g_patternRippleCurrent[right] +
                    g_patternRippleCurrent[up] +
                    g_patternRippleCurrent[down];
                float laplacian = neighbors - center * 4.0f;
                float value = 2.0f * center - g_patternRipplePrevious[index] +
                              laplacian * propagation;
                value *= damping;
                g_patternRippleNext[index] =
                    std::max(-1.0f, std::min(1.0f, value));
            }
        }
        float* oldPrevious = g_patternRipplePrevious;
        g_patternRipplePrevious = g_patternRippleCurrent;
        g_patternRippleCurrent = g_patternRippleNext;
        g_patternRippleNext = oldPrevious;
    }
    RequestFrame();
}

// Stable spatial variation: it is tied to the fixed grid coordinates, not to
// time, so dots have a subtle hierarchy without behaving independently.
static float PatternDotVariation(float pixelX, float pixelY, float spacing)
{
    int gridX = static_cast<int>(floorf(pixelX / std::max(1.0f, spacing)));
    int gridY = static_cast<int>(floorf(pixelY / std::max(1.0f, spacing)));
    unsigned int hash = static_cast<unsigned int>(gridX) * 73856093u ^
                        static_cast<unsigned int>(gridY) * 19349663u;
    hash ^= hash >> 13;
    hash *= 1274126177u;
    return static_cast<float>(hash & 0xffffu) / 65535.0f;
}

static float PatternBaseHeight(float pixelX, float pixelY, float time)
{
    if (g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return 0.0f;
    // X is dominant, producing vertical crests. Y and time bend them organically.
    float nx = pixelX / static_cast<float>(g_runtimeSelectorWidth);
    float ny = pixelY / static_cast<float>(g_runtimeSelectorHeight);
    // Moderately faster phase motion keeps the vertical crests visibly moving
    // without turning the pattern into noisy flicker.
    const float speed = 0.55f + g_patternSpeed * 1.35f;
    float organic = 0.20f * sinf(ny * 7.0f + time * 0.31f +
                                  sinf(nx * 4.0f + time * 0.17f));
    float ridges = sinf(nx * 13.5f + time * speed + organic);
    float fine = 0.28f * sinf(nx * 27.0f - time * speed * 0.55f + ny * 2.5f);
    float height = (ridges + fine) / 1.28f;
    // Slightly larger field amplitude gives the influence more spatial body;
    // visual brightness remains controlled in the dot stage below.
    return height * std::max(0.0f, std::min(2.0f, g_patternWaveStrength)) * 0.68f;
}

static void UpdatePatternRenderCache(float time)
{
    const float rippleScale = 0.72f + g_patternWaveStrength * 0.38f;
    const float width = static_cast<float>(std::max(1, g_runtimeSelectorWidth));
    const float height = static_cast<float>(std::max(1, g_runtimeSelectorHeight));
    for (int y = 0; y < kPatternFieldHeight; ++y)
    {
        float pixelY = static_cast<float>(y) * height /
                       static_cast<float>(kPatternFieldHeight - 1);
        for (int x = 0; x < kPatternFieldWidth; ++x)
        {
            float pixelX = static_cast<float>(x) * width /
                           static_cast<float>(kPatternFieldWidth - 1);
            int index = y * kPatternFieldWidth + x;
            g_patternCombinedFieldCache[index] =
                PatternBaseHeight(pixelX, pixelY, time) +
                g_patternRippleCurrent[index] * rippleScale;
        }
    }
}

static float PatternFieldSample(float pixelX, float pixelY)
{
    if (g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return 0.0f;
    float fx = WrapPatternCoordinate(pixelX,
        static_cast<float>(g_runtimeSelectorWidth), kPatternFieldWidth);
    float fy = WrapPatternCoordinate(pixelY,
        static_cast<float>(g_runtimeSelectorHeight), kPatternFieldHeight);
    int x0 = static_cast<int>(floorf(fx));
    int y0 = static_cast<int>(floorf(fy));
    float tx = fx - x0;
    float ty = fy - y0;
    int x1 = (x0 + 1) % kPatternFieldWidth;
    int y1 = (y0 + 1) % kPatternFieldHeight;
    float a = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x0, y0)];
    float b = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x1, y0)];
    float c = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x0, y1)];
    float d = g_patternCombinedFieldCache[PatternFieldWrappedIndex(x1, y1)];
    float top = a + (b - a) * tx;
    float bottom = c + (d - c) * tx;
    return top + (bottom - top) * ty;
}

static void DrawPatternWaves(const RECT& clientRect)
{
    if (!IsPatternWavesActive() || !g_d2dDCRenderTarget || !g_d2dBrush)
        return;
    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0)
        return;

    const ULONGLONG drawStart = GetTickCount64();
    UpdatePatternRenderCache(static_cast<float>(drawStart) / 1000.0f);
    // La calidad adaptativa controla la cadencia, no la geometría del patrón.
    // Así una variación de rendimiento no cambia tamaño/posición de los dots.
    // Density controls the visual grid independently from the low-resolution
    // simulation. At the default value this produces a considerably denser
    // micro-dot surface while keeping the ripple buffer small.
    const float spacing = std::max(6.0f,
        g_patternDensity * g_uiScale.value * 0.55f);
    const float renderMargin = spacing * 1.5f;
    const float light = std::max(0.0f, std::min(2.0f, g_patternLight));
    const float contrast = std::max(0.0f, std::min(2.0f, g_patternContrast));
    const float effectOpacity = std::max(0.0f, std::min(1.0f, g_patternOpacity));

    // Keep DWM visible. This is a translucent tint plus one dot field, not an
    // opaque second background.
    D2D1_RECT_F background = D2D1::RectF(0.0f, 0.0f,
                                          static_cast<float>(width),
                                          static_cast<float>(height));
    g_d2dBrush->SetColor(D2D1::ColorF(0.025f, 0.035f, 0.055f,
                                      0.08f * effectOpacity));
    g_d2dDCRenderTarget->FillRectangle(&background, g_d2dBrush);

    // The caller already applies the scene transform used by the cards and
    // DWM background. Pattern Waves is deliberately rendered in that same
    // coordinate space, so opening/closing animations affect the whole field
    // without introducing a second animation system.

    for (float y = -renderMargin; y < height + renderMargin; y += spacing)
    {
        for (float x = -renderMargin; x < width + renderMargin; x += spacing)
        {
            // The grid is fixed. The combined field is converted into a smooth
            // influence profile; it changes only the visual state of each dot.
            float field = std::max(-1.0f, std::min(1.0f,
                PatternFieldSample(x, y)));
            float dx = (PatternFieldSample(x + 3.0f, y) -
                        PatternFieldSample(x - 3.0f, y)) * 0.1667f;
            float dy = (PatternFieldSample(x, y + 3.0f) -
                        PatternFieldSample(x, y - 3.0f)) * 0.1667f;
            float normalLight = std::max(0.0f, std::min(1.0f,
                0.50f + (-dx - dy) * 0.36f * light));
            // A shared-field falloff controls every visual property. The stable
            // variation only supplies subtle scale/opacity hierarchy on the
            // fixed grid; it is never an animation of individual dots.
            float waveEnergy = std::max(0.0f, std::min(1.0f,
                fabsf(field) * (1.25f + 0.20f * light)));
            // Broaden the soft band first, then use a gentle nonlinear remap
            // so the crest grows visibly without creating hard rings.
            float influence = SmoothStep01((waveEnergy - 0.10f) / 0.82f);
            influence = powf(influence, 0.78f);
            float litInfluence = std::max(0.0f, std::min(1.0f,
                influence * (0.50f + normalLight * 0.50f)));
            float brightness = std::max(0.0f, std::min(1.0f,
                0.12f + litInfluence * 0.70f));
            brightness = std::max(0.0f, std::min(1.0f,
                0.16f + (brightness - 0.16f) * contrast));
            float variation = PatternDotVariation(x, y, spacing);
            float baseScale = 0.78f + variation * 0.22f;
            // Most dots remain micro-sized; only the shared-field crest adds
            // substantial scale. This widens the wave without adding dots.
            float radius = std::max(0.85f,
                spacing * (0.026f * baseScale + influence * 0.185f));
            // Lift visibility mainly through opacity, while retaining a dim
            // but clearly present base layer over the Obsidian background.
            float dotAlpha = effectOpacity *
                std::min(0.90f, 0.095f + 0.17f * brightness +
                         0.45f * influence + variation * 0.030f);
            // Neutral white/gray palette: the field changes tone, not hue.
            // Neutral luminance ramp: small dots become light gray, while
            // wave-driven dots approach white without introducing saturation.
            float tone = std::max(0.0f, std::min(0.985f,
                0.62f + brightness * 0.25f + influence * 0.20f +
                variation * 0.020f));
            g_d2dBrush->SetColor(D2D1::ColorF(tone, tone, tone, dotAlpha));
            DrawPatternShape(x, y, radius);
        }
    }

    // No restablecer aquí el transform: el caller conserva el mismo transform
    // de escena para continuar dibujando cards/UI de forma consistente.

    // Ajuste con histéresis fuerte. GetTickCount64 tiene una granularidad que
    // puede convertir un render de 0 ms en 15/16 ms de una muestra a otra;
    // reaccionar a una sola medición cambia el grid y produce saltos visuales.
    // Esto solo modifica la densidad propia del fondo, nunca layout/transform UI.
    ULONGLONG drawMs = GetTickCount64() - drawStart;
    ULONGLONG now = GetTickCount64();
    if (drawMs >= 12)
    {
        ++g_patternSlowSamples;
        g_patternFastSamples = 0;
    }
    else if (drawMs <= 4)
    {
        ++g_patternFastSamples;
        g_patternSlowSamples = 0;
    }
    else
    {
        g_patternSlowSamples = 0;
        g_patternFastSamples = 0;
    }

    bool qualityChanged = false;
    // En idle, el grid elegido para el fondo permanece estable. La simulación
    // y el repaint continúan; solo se evita un cambio compositivo no solicitado.
    const bool layoutIdle = g_selectorAnimation == SelectorAnimationState::Open &&
                            !g_animActive && !g_cardExitActive;
    if (layoutIdle)
    {
        g_patternSlowSamples = 0;
        g_patternFastSamples = 0;
    }
    if (!layoutIdle && now >= g_patternQualityCooldownUntil)
    {
        // Cuatro muestras lentas consecutivas para degradar.
        if (g_patternSlowSamples >= 4)
        {
            float next = std::max(0.55f, g_patternQualityScale - 0.15f);
            qualityChanged = next != g_patternQualityScale;
            g_patternQualityScale = next;
            g_patternSlowSamples = 0;
            g_patternFastSamples = 0;
            g_patternQualityCooldownUntil = now + 2500;
        }
        // La recuperación es deliberadamente mucho más lenta que la degradación.
        else if (g_patternFastSamples >= 24 && g_patternQualityScale < 1.0f)
        {
            g_patternQualityScale = std::min(1.0f, g_patternQualityScale + 0.05f);
            qualityChanged = true;
            g_patternSlowSamples = 0;
            g_patternFastSamples = 0;
            g_patternQualityCooldownUntil = now + 2500;
        }
    }
    if (qualityChanged && g_selector && IsWindow(g_selector))
        SetTimer(g_selector, kPatternTimerId, GetPatternTimerInterval(), nullptr);
}
static bool IsPointInsideRoundedRect(const D2D1_ROUNDED_RECT& rect, float x, float y)
{
    if (x < rect.rect.left || x > rect.rect.right ||
        y < rect.rect.top || y > rect.rect.bottom)
        return false;

    const float radiusX = std::min(rect.radiusX,
        (rect.rect.right - rect.rect.left) * 0.5f);
    const float radiusY = std::min(rect.radiusY,
        (rect.rect.bottom - rect.rect.top) * 0.5f);
    if ((x >= rect.rect.left + radiusX && x <= rect.rect.right - radiusX) ||
        (y >= rect.rect.top + radiusY && y <= rect.rect.bottom - radiusY))
        return true;

    float cx = x < rect.rect.left + radiusX
        ? rect.rect.left + radiusX : rect.rect.right - radiusX;
    float cy = y < rect.rect.top + radiusY
        ? rect.rect.top + radiusY : rect.rect.bottom - radiusY;
    float nx = (x - cx) / std::max(1.0f, radiusX);
    float ny = (y - cy) / std::max(1.0f, radiusY);
    return nx * nx + ny * ny <= 1.0f;
}

// Radio único del contenedor principal. La región Win32 es la autoridad de
// clipping; cualquier hit-test o capa visual debe reutilizar exactamente este
// cálculo para no introducir una segunda curvatura.
static int GetSelectorCornerRadiusPx(int width, int height)
{
    if (width <= 0 || height <= 0)
        return 0;
    int radius = std::max(ScaleLayoutPx(27.0f), 16);
    return std::min(radius, std::min(width, height) / 2);
}

static bool IsPointInsidePatternArea(POINT point)
{
    if (!IsPatternWavesActive() ||
        g_runtimeSelectorWidth <= 0 || g_runtimeSelectorHeight <= 0)
        return false;

    // Reuse the exact dimensions and radius of ApplySelectorRoundedRegion.
    // The final pixels are clipped by that same SetWindowRgn region.
    RECT clientRect = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
    int radiusPx = GetSelectorCornerRadiusPx(clientRect.right, clientRect.bottom);
    D2D1_ROUNDED_RECT area = D2D1::RoundedRect(
        D2D1::RectF(0.0f, 0.0f,
                    static_cast<float>(clientRect.right),
                    static_cast<float>(clientRect.bottom)),
        static_cast<float>(radiusPx), static_cast<float>(radiusPx));
    return IsPointInsideRoundedRect(area,
                                    static_cast<float>(point.x),
                                    static_cast<float>(point.y));
}

// Sombra ambiental del fondo general: un pozo oscuro y suave detrás de las
// cards que se desvanece a alfa 0 antes de llegar a los bordes. Es un degradado
// radial (no un rectángulo), así que no tapa la transparencia de DWM ni dibuja
// un borde. El brush se crea una vez; por frame solo se ajustan centro y radios.
static ID2D1RadialGradientBrush* EnsureAmbientShadowBrush()
{
    if (g_ambientShadowBrush)
        return g_ambientShadowBrush;
    if (!g_d2dDCRenderTarget)
        return nullptr;

    D2D1_GRADIENT_STOP stops[4] = {};
    stops[0].position = 0.00f; stops[0].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.24f);
    stops[1].position = 0.45f; stops[1].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.14f);
    stops[2].position = 0.78f; stops[2].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.04f);
    stops[3].position = 1.00f; stops[3].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);

    ID2D1GradientStopCollection* collection = nullptr;
    if (FAILED(g_d2dDCRenderTarget->CreateGradientStopCollection(
            stops, 4, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection)) ||
        !collection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};
    props.center = D2D1::Point2F(0.0f, 0.0f);
    props.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    props.radiusX = 1.0f;
    props.radiusY = 1.0f;
    HRESULT hr = g_d2dDCRenderTarget->CreateRadialGradientBrush(
        &props, nullptr, collection, &g_ambientShadowBrush);
    collection->Release();
    return SUCCEEDED(hr) ? g_ambientShadowBrush : nullptr;
}

static ID2D1Layer* AcquireD2DLayer(ID2D1Layer*& slot)
{
    if (!slot && g_d2dDCRenderTarget)
        g_d2dDCRenderTarget->CreateLayer(nullptr, &slot);
    return slot;
}

static ID2D1RoundedRectangleGeometry* GetCachedCardMask(const D2D1_ROUNDED_RECT& rect)
{
    if (!g_d2dFactory)
        return nullptr;
    if (g_cardMaskGeometry &&
        std::memcmp(&g_cardMaskKey, &rect, sizeof(rect)) == 0)
        return g_cardMaskGeometry;
    if (g_cardMaskGeometry)
    {
        g_cardMaskGeometry->Release();
        g_cardMaskGeometry = nullptr;
    }
    if (SUCCEEDED(g_d2dFactory->CreateRoundedRectangleGeometry(&rect, &g_cardMaskGeometry)) &&
        g_cardMaskGeometry)
        g_cardMaskKey = rect;
    return g_cardMaskGeometry;
}

static void PaintSelectorScene(HWND hwnd, HDC hdc)
{
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
        return;

    bool d2dActive = LoadD2DAndDWrite() && g_d2dDCRenderTarget && g_d2dBrush &&
                     SUCCEEDED(g_d2dDCRenderTarget->BindDC(hdc, &clientRect));

    if (d2dActive)
    {
        g_d2dDCRenderTarget->BeginDraw();
        g_d2dDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        // Clear completamente para dejar que DWM glass sea visible
        g_d2dDCRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        RECT sceneRect = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
        g_d2dDCRenderTarget->PushAxisAlignedClip(
            D2D1::RectF(0.0f, 0.0f,
                        static_cast<float>(sceneRect.right),
                        static_cast<float>(sceneRect.bottom)),
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        float sceneScaleX = 1.0f;
        float sceneScaleY = 1.0f;
        GetSceneScale(sceneScaleX, sceneScaleY);
        ID2D1Layer* sceneOpacityLayer = nullptr;
        if (g_sceneOpacity < 0.999f)
            sceneOpacityLayer = AcquireD2DLayer(g_sceneLayer);
        if (sceneOpacityLayer)
        {
            D2D1_LAYER_PARAMETERS opacityParameters = D2D1::LayerParameters(
                D2D1::InfiniteRect(), nullptr,
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                D2D1::Matrix3x2F::Identity(), g_sceneOpacity, nullptr,
                D2D1_LAYER_OPTIONS_NONE);
            g_d2dDCRenderTarget->PushLayer(&opacityParameters, sceneOpacityLayer);
        }

        // El background y el contenedor forman parte de la misma escena que
        // las cards: deben acompañar la escala de apertura/cierre y compartir
        // su pivote central.
        if (sceneScaleX != 1.0f || sceneScaleY != 1.0f)
        {
            g_d2dDCRenderTarget->SetTransform(GetSceneTransform());
        }

        // Base background layer. None deliberately leaves the surface
        // transparent so whatever is underneath remains visible.
        D2D1_RECT_F bgRect = D2D1::RectF(
            static_cast<float>(sceneRect.left),
            static_cast<float>(sceneRect.top),
            static_cast<float>(sceneRect.right),
            static_cast<float>(sceneRect.bottom));
        if (g_baseBackgroundMode == BaseBackgroundMode::Dwm && g_dwmGlassEnabled)
        {
            float r = GetRValue(g_dwmBackgroundColor) / 255.0f;
            float g = GetGValue(g_dwmBackgroundColor) / 255.0f;
            float b = GetBValue(g_dwmBackgroundColor) / 255.0f;
            g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, g_dwmBackgroundOpacity));
            g_d2dDCRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }
        else if (g_baseBackgroundMode == BaseBackgroundMode::Dwm)
        {
            // Fallback for DWM mode if glass is unavailable.
            g_d2dBrush->SetColor(D2D1::ColorF(0.70f, 0.74f, 0.82f, 0.18f));
            g_d2dDCRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }

        // Cursor spotlight: only the background is affected. Cards are drawn
        // afterwards, so the spotlight never bleeds across their surfaces.
        if (IsBackgroundSpotlightAllowed() &&
            g_dwmGlassEnabled && g_backgroundSpotlightActive)
        {
            ID2D1RadialGradientBrush* spotlight = EnsureBackgroundSpotlightBrush();
            if (spotlight)
            {
                float sx = 1.0f, sy = 1.0f;
                GetSceneScale(sx, sy);
                float cx = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
                float cy = static_cast<float>(g_runtimeSelectorHeight) * 0.5f;
                float sceneX = cx +
                    (static_cast<float>(g_backgroundSpotlightPoint.x) - cx) /
                    std::max(0.001f, sx);
                float sceneY = cy +
                    (static_cast<float>(g_backgroundSpotlightPoint.y) - cy) /
                    std::max(0.001f, sy);
                spotlight->SetCenter(D2D1::Point2F(sceneX, sceneY));
                spotlight->SetGradientOriginOffset(D2D1::Point2F(0.0f, 0.0f));
                spotlight->SetRadiusX(DpiPx(kSpotlightRadiusDip) / std::max(0.001f, sx));
                spotlight->SetRadiusY(DpiPx(kSpotlightRadiusDip) / std::max(0.001f, sy));
                g_d2dDCRenderTarget->FillRectangle(&bgRect, spotlight);
            }
        }

        // Experimental background is an independent layer over the base.
        if (IsPatternWavesActive())
        {
            // Pattern Waves is content inside the selector's existing DWM
            // container. The window region clips it; no second rounded mask
            // or Pattern-specific container is created here.
            // The caller already owns the scene transform, so Pattern Waves
            // draws in scene coordinates without a second scale.
            DrawPatternWaves(sceneRect);
            if (sceneScaleX != 1.0f || sceneScaleY != 1.0f)
            {
                g_d2dDCRenderTarget->SetTransform(GetSceneTransform());
            }
        }

        // The global container border/shadow is itself a background visual.
        // With no base and no experimental layer, omit it so cards float over
        // the desktop. Pattern Waves keeps its own mask and reactive border.
        const bool hasVisualBackground =
            g_baseBackgroundMode != BaseBackgroundMode::None;
        if (hasVisualBackground)
        {
            // DWM owns the single rounded container. Pattern Waves does not
            // replace or add another outline around the same area.
            D2D1_ROUNDED_RECT containerRect = D2D1::RoundedRect(
                D2D1::RectF(DpiPx(0.75f), DpiPx(0.75f),
                            static_cast<float>(sceneRect.right) - DpiPx(0.75f),
                            static_cast<float>(sceneRect.bottom) - DpiPx(0.75f)),
                static_cast<float>(ScaleLayoutPx(27.0f)),
                static_cast<float>(ScaleLayoutPx(27.0f)));
            g_d2dBrush->SetColor(D2D1::ColorF(0.32f, 0.36f, 0.43f, 0.30f));
            g_d2dDCRenderTarget->DrawRoundedRectangle(&containerRect,
                                                       g_d2dBrush, DpiPx(1.0f));

            RECT containerShadow = {
                ScaleLayoutPx(12.0f),
                sceneRect.bottom - ScaleLayoutPx(8.0f),
                sceneRect.right - ScaleLayoutPx(12.0f),
                sceneRect.bottom
            };
            PaintBottomShadowD2D(containerShadow, ScaleLayoutPx(12.0f), 0.34f);
        }

        // Sombra ambiental general: por encima del fondo (DWM/Pattern) y por
        // detrás de las cards. Se dibuja con ambos modos de fondo.
        if (ID2D1RadialGradientBrush* ambient = EnsureAmbientShadowBrush())
        {
            const float w = static_cast<float>(sceneRect.right);
            const float h = static_cast<float>(sceneRect.bottom);
            ambient->SetCenter(D2D1::Point2F(w * 0.5f, h * 0.54f));
            ambient->SetRadiusX(w * 0.55f);
            ambient->SetRadiusY(h * 0.70f);
            g_d2dDCRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), ambient);
        }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
        {
            int index = ResolveGroupIndex(slot);
            if (index < 0)
                continue;

            ID2D1Layer* cardExitLayer = nullptr;
            if (g_cardExitActive && slot == g_cardExitSlot && g_d2dDCRenderTarget)
            {
                float opacity = GetCardExitOpacity();
                cardExitLayer = AcquireD2DLayer(g_cardExitLayer);
                if (cardExitLayer)
                {
                    D2D1_LAYER_PARAMETERS layerParameters = D2D1::LayerParameters(
                        D2D1::InfiniteRect(), nullptr,
                        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::Matrix3x2F::Identity(), opacity, nullptr,
                        D2D1_LAYER_OPTIONS_NONE);
                    g_d2dDCRenderTarget->PushLayer(&layerParameters, cardExitLayer);
                }
            }

            float distFromCenter = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
            bool selected = (distFromCenter < 0.5f);
            RECT cardRect = GetSlotCardRect(slot);
            float radius = selected ? static_cast<float>(g_cardStyle.selectedCornerRadius) * g_uiScale.value
                                    : static_cast<float>(g_cardStyle.cornerRadius) * g_uiScale.value;
            PaintBottomShadowD2D(cardRect, radius, selected ? 0.28f : 0.20f);
            if (selected && slot == GetCarouselCenterSlot())
                PaintCenterAccentMarginD2D(cardRect, index, radius);

            COLORREF cardColor = selected ? g_cardSelectedColor : g_cardNormalColor;
            D2D1_COLOR_F bgCol = D2D1::ColorF(
                GetRValue(cardColor) / 255.0f,
                GetGValue(cardColor) / 255.0f,
                GetBValue(cardColor) / 255.0f,
                kCardSurfaceOpacity);
            D2D1_ROUNDED_RECT cr = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(cardRect.left) + 0.5f, static_cast<float>(cardRect.top) + 0.5f,
                            static_cast<float>(cardRect.right) - 0.5f, static_cast<float>(cardRect.bottom) - 0.5f),
                radius, radius);
            ID2D1LinearGradientBrush* surfaceBrush =
                EnsureSurfaceGradientBrush(index, selected);
            if (surfaceBrush)
            {
                float cardHeight = static_cast<float>(cardRect.bottom - cardRect.top);
                surfaceBrush->SetStartPoint(D2D1::Point2F(
                    static_cast<float>(cardRect.left),
                    static_cast<float>(cardRect.top) + cardHeight * 0.50f));
                surfaceBrush->SetEndPoint(D2D1::Point2F(
                    static_cast<float>(cardRect.right),
                    static_cast<float>(cardRect.top) + cardHeight * 0.50f));
                g_d2dDCRenderTarget->FillRoundedRectangle(&cr, surfaceBrush);
            }
            else
            {
                g_d2dBrush->SetColor(bgCol);
                g_d2dDCRenderTarget->FillRoundedRectangle(&cr, g_d2dBrush);
            }

            D2D1_COLOR_F borderCol = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.92f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.82f);
            g_d2dBrush->SetColor(borderCol);
            g_d2dDCRenderTarget->DrawRoundedRectangle(&cr, g_d2dBrush,
                                                      DpiPx(selected ? 1.5f : 1.0f));

            RECT prevRect = GetSlotPreviewRect(slot);
            D2D1_ROUNDED_RECT pr = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(prevRect.left) + 0.5f, static_cast<float>(prevRect.top) + 0.5f,
                            static_cast<float>(prevRect.right) - 0.5f, static_cast<float>(prevRect.bottom) - 0.5f),
                DpiPx(6.0f), DpiPx(6.0f));
            // No se pinta un panel opaco detrás del thumbnail. El thumbnail
            // DWM permanece nítido y con opacity=255; los huecos dejan ver la
            // superficie translúcida de la card.
            D2D1_COLOR_F prevBorder = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.60f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.50f);
            g_d2dBrush->SetColor(prevBorder);
            g_d2dDCRenderTarget->DrawRoundedRectangle(&pr, g_d2dBrush, DpiPx(1.0f));

            if (g_groups[index].representativeWindow &&
                IsIconic(g_groups[index].representativeWindow) &&
                g_groups[index].icon)
            {
                EnsureIconBitmap(index);
                int relative = slot - GetCarouselCenterSlot();
                int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 56 : (abs(relative) == 1 ? 40 : 28)));
                int cx = prevRect.left + (prevRect.right - prevRect.left - iconSize) / 2;
                int cy = prevRect.top + (prevRect.bottom - prevRect.top - iconSize) / 2;
                if (g_groups[index].iconBitmap)
                {
                    D2D1_RECT_F iconDest = D2D1::RectF(
                        static_cast<float>(cx),
                        static_cast<float>(cy),
                        static_cast<float>(cx + iconSize),
                        static_cast<float>(cy + iconSize));
                    g_d2dDCRenderTarget->DrawBitmap(
                        g_groups[index].iconBitmap, &iconDest, 1.0f,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            }

            PaintSlotHeaderD2D(slot, index, selected, distFromCenter);

            if (g_hoveredSlot == slot)
            {
                const float hoverOffset = DpiPx(3.0f);
                D2D1_ROUNDED_RECT hoverRect = D2D1::RoundedRect(
                    D2D1::RectF(static_cast<float>(cardRect.left) - hoverOffset,
                                static_cast<float>(cardRect.top) - hoverOffset,
                                static_cast<float>(cardRect.right) + hoverOffset,
                                static_cast<float>(cardRect.bottom) + hoverOffset),
                    radius + DpiPx(2.0f), radius + DpiPx(2.0f));
                COLORREF accent = g_groups[index].mediaAccent.valid
                    ? g_groups[index].mediaAccent.color : RGB(110, 116, 130);
                g_d2dBrush->SetColor(D2D1::ColorF(
                    GetRValue(accent) / 255.0f, GetGValue(accent) / 255.0f,
                    GetBValue(accent) / 255.0f, 0.42f));
                g_d2dDCRenderTarget->DrawRoundedRectangle(&hoverRect, g_d2dBrush, DpiPx(1.25f));

                if (g_hoveredCloseButton)
                {
                    RECT closeRect = GetCloseHoverButtonRect(slot);
                    ID2D1RadialGradientBrush* closeBrush = EnsureCloseAccentBrush();
                    ID2D1RoundedRectangleGeometry* cardMask = GetCachedCardMask(cr);
                    ID2D1Layer* closeLayer =
                        cardMask ? AcquireD2DLayer(g_closeLayer) : nullptr;
                    if (cardMask && closeLayer)
                    {
                        g_d2dDCRenderTarget->PushLayer(
                            D2D1::LayerParameters(D2D1::InfiniteRect(), cardMask), closeLayer);
                    }
                    if (closeBrush)
                    {
                        closeBrush->SetCenter(D2D1::Point2F(
                            static_cast<float>(closeRect.left + closeRect.right) * 0.5f,
                            static_cast<float>(closeRect.top + closeRect.bottom) * 0.5f));
                        float glowRadiusX = static_cast<float>(ScaleLayoutPx(58.0f));
                        float glowRadiusY = static_cast<float>(ScaleLayoutPx(52.0f));
                        closeBrush->SetRadiusX(glowRadiusX);
                        closeBrush->SetRadiusY(glowRadiusY);
                        D2D1_ELLIPSE glowEllipse = D2D1::Ellipse(
                            D2D1::Point2F(
                                static_cast<float>(closeRect.left + closeRect.right) * 0.5f,
                                static_cast<float>(closeRect.top + closeRect.bottom) * 0.5f),
                            glowRadiusX, glowRadiusY);
                        g_d2dDCRenderTarget->FillEllipse(&glowEllipse, closeBrush);
                    }
                    if (closeLayer)
                    {
                        g_d2dDCRenderTarget->PopLayer();
                    }

                    int buttonPad = ScaleLayoutPx(2.0f);
                    RECT buttonRect = {
                        closeRect.left - buttonPad, closeRect.top - buttonPad,
                        closeRect.right + buttonPad, closeRect.bottom + buttonPad
                    };
                    ID2D1PathGeometry* closeGeometry =
                        CreateCloseButtonGeometry(buttonRect);
                    if (closeGeometry)
                    {
                        g_d2dBrush->SetColor(D2D1::ColorF(0.035f, 0.018f, 0.022f, 0.75f));
                        g_d2dDCRenderTarget->FillGeometry(closeGeometry, g_d2dBrush);
                        g_d2dBrush->SetColor(D2D1::ColorF(0.45f, 0.10f, 0.12f, 0.62f));
                        g_d2dDCRenderTarget->DrawGeometry(closeGeometry, g_d2dBrush, DpiPx(1.0f));
                    }
                }

                if (g_dwriteCloseFormat)
                {
                    RECT closeRect = GetCloseHoverButtonRect(slot);
                    D2D1_RECT_F closeTextRect = D2D1::RectF(
                        static_cast<float>(closeRect.left), static_cast<float>(closeRect.top),
                        static_cast<float>(closeRect.right), static_cast<float>(closeRect.bottom));
                    g_d2dBrush->SetColor(g_hoveredCloseButton
                        ? D2D1::ColorF(0.86f, 0.38f, 0.40f, 1.0f)
                        : D2D1::ColorF(0.62f, 0.24f, 0.25f, 0.92f));
                    const wchar_t closeGlyph[] = L"\x00D7";
                    g_d2dDCRenderTarget->DrawText(closeGlyph, 1, g_dwriteCloseFormat,
                                                  &closeTextRect, g_d2dBrush);
                }
            }
            if (cardExitLayer)
            {
                g_d2dDCRenderTarget->PopLayer();
            }
        }

        PaintCounterD2D();
        if (sceneOpacityLayer)
        {
            g_d2dDCRenderTarget->PopLayer();
        }
        g_d2dDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        g_d2dDCRenderTarget->PopAxisAlignedClip();
        const HRESULT drawResult = g_d2dDCRenderTarget->EndDraw();
        if (drawResult == static_cast<HRESULT>(0x8899000C))  // D2DERR_RECREATE_TARGET
            ReleaseD2DDeviceResources();
        return;
    }

    const bool hasVisualBackground =
        g_baseBackgroundMode != BaseBackgroundMode::None ||
        IsPatternWavesActive();
    if (hasVisualBackground)
    {
        RECT containerShadow = {
            ScaleLayoutPx(12.0f),
            clientRect.bottom - ScaleLayoutPx(8.0f),
            clientRect.right - ScaleLayoutPx(12.0f),
            clientRect.bottom
        };
        PaintBottomShadowGDI(hdc, containerShadow, ScaleLayoutPx(12.0f));
    }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
    {
        int index = ResolveGroupIndex(slot);
        if (index < 0)
            continue;
        float dist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
        bool selected = (dist < 0.5f);
        RECT cardRect = GetSlotCardRect(slot);
        int radius = selected ? ScaleLayoutPx(static_cast<float>(g_cardStyle.selectedCornerRadius))
                              : ScaleLayoutPx(static_cast<float>(g_cardStyle.cornerRadius));
        PaintBottomShadowGDI(hdc, cardRect, radius);
        DrawCardGlowAndBorderGDI(hdc, cardRect, radius, selected);
        PaintSlotHeaderGDI(hdc, slot, index, selected, dist);
        if (g_hoveredSlot == slot)
        {
            HPEN hoverPen = g_createPen ? g_createPen(PS_SOLID, 1, RGB(110, 116, 130)) : nullptr;
            if (hoverPen && g_selectObject && g_getStockObject && g_roundRect && g_deleteObject)
            {
                RECT hoverRect = cardRect;
                hoverRect.left -= 3; hoverRect.top -= 3;
                hoverRect.right += 3; hoverRect.bottom += 3;
                HGDIOBJ oldPen = g_selectObject(hdc, hoverPen);
                HGDIOBJ oldBrush = g_selectObject(hdc, g_getStockObject(NULL_BRUSH));
                g_roundRect(hdc, hoverRect.left, hoverRect.top, hoverRect.right, hoverRect.bottom, radius + 2, radius + 2);
                g_selectObject(hdc, oldBrush);
                g_selectObject(hdc, oldPen);
                g_deleteObject(hoverPen);
            }
            RECT closeRect = GetCloseHoverButtonRect(slot);
            if (g_hoveredCloseButton && g_createSolidBrush && g_createPen &&
                g_selectObject && g_roundRect && g_deleteObject)
            {
                RECT buttonRect = closeRect;
                int buttonPad = ScaleLayoutPx(2.0f);
                buttonRect.left -= buttonPad; buttonRect.top -= buttonPad;
                buttonRect.right += buttonPad; buttonRect.bottom += buttonPad;
                HBRUSH buttonBrush = g_createSolidBrush(RGB(24, 8, 10));
                HPEN buttonPen = g_createPen(PS_SOLID, 1, RGB(122, 36, 42));
                HGDIOBJ oldBrush = g_selectObject(hdc, buttonBrush);
                HGDIOBJ oldPen = g_selectObject(hdc, buttonPen);
                g_roundRect(hdc, buttonRect.left, buttonRect.top,
                            buttonRect.right, buttonRect.bottom,
                            ScaleLayoutPx(12.0f), ScaleLayoutPx(6.0f));
                g_selectObject(hdc, oldBrush);
                g_selectObject(hdc, oldPen);
                g_deleteObject(buttonBrush);
                g_deleteObject(buttonPen);
            }
            if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
            if (g_setTextColor)
                g_setTextColor(hdc, g_hoveredCloseButton ? RGB(220, 100, 104) : RGB(158, 61, 64));
            HGDIOBJ oldFont = g_selectedNameFont && g_selectObject ?
                g_selectObject(hdc, g_selectedNameFont) : nullptr;
            DrawTextW(hdc, L"\x00D7", -1, &closeRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);
        }
    }
    PaintCounterGDI(hdc);
}

static LRESULT CALLBACK SelectorWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        UpdateSelectorControls();
        return 0;

    case WM_TIMER:
    {
        if (wParam == kTabRepeatTimerId)
        {
            if (!IsSelectorActive() || !g_uiTabDown)
            {
                KillTimer(hwnd, kTabRepeatTimerId);
                g_tabRepeatStarted = false;
                return 0;
            }
            bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            if (!g_tabRepeatStarted)
                g_tabRepeatStarted = true;
            if (shiftDown) SelectPrevious();
            else SelectNext();
            SetTimer(hwnd, kTabRepeatTimerId, kTabRepeatIntervalMs, nullptr);
            return 0;
        }

        if (wParam == kActivityTimerId)
        {
            // Failsafe: existe únicamente mientras el selector está abierto.
            if (g_state == SelectorState::SelectorActive)
            {
                ULONGLONG now = GetTickCount64();
                bool invalidWindow = !g_selector || !IsWindow(g_selector);
                bool modifiersDown =
                    (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
                bool staleWithoutPhysicalSession =
                    g_lastSelectorActivity != 0 &&
                    now - g_lastSelectorActivity > kSelectorFailsafeMs &&
                    !g_uiTabDown && !modifiersDown;
                if (invalidWindow || staleWithoutPhysicalSession)
                    EmergencyCloseSelector();
            }
            return 0;
        }

        if (wParam == kSelectorMotionTimerId)
        {
            // Solo en el camino de reserva (sin waitable timer): disparo único.
            KillTimer(hwnd, kSelectorMotionTimerId);
            RunFrame();
            return 0;
        }

        if (wParam == kPatternTimerId)
        {
            if (!IsPatternWavesActive() || !IsSelectorActive())
            {
                KillTimer(hwnd, kPatternTimerId);
                return 0;
            }
            UpdatePatternWaves();
            return 0;
        }

        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        PaintSelectorScene(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_COMMAND:
        return 0;

    case WM_DPICHANGED:
        // El tamaño y la posición los fija CreateSelector en píxeles físicos
        // para el monitor de destino; no se acepta el rectángulo sugerido.
        return 0;

    case WM_SIZE:
        UpdateThumbnailSlots();
        return 0;

    case WM_CLOSE:
        return 0;

    case WM_DESTROY:
        StopCarouselAnimation();
        KillTimer(hwnd, kActivityTimerId);
        UnregisterAllThumbnails();
        if (g_selector == hwnd)
            g_selector = nullptr;
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static bool RegisterSelectorClasses()
{
    if (g_classesRegistered)
        return true;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = SelectorWndProc;
    wc.hInstance = g_hModule;
    wc.lpszClassName = kWindowClassName;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.hbrBackground = nullptr;
    if (!RegisterClassW(&wc))
    {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        return false;
    }

    g_classesRegistered = true;
    return true;
}

static void UnregisterSelectorClasses()
{
    if (!g_classesRegistered)
        return;
    UnregisterClassW(kWindowClassName, g_hModule);
    g_classesRegistered = false;
}

static void UpdateSelectorSurfaceTransform()
{
    if (!g_selector || !IsWindow(g_selector))
        return;

    // The selector HWND remains physically fixed. Opening/closing animations
    // are rendered entirely by D2D; no separate blur surface is used.
    ApplySelectorRoundedRegion(g_selector);
}

static void ApplySelectorRoundedRegion(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd) || !LoadGdiFunctions() || !g_createRoundRectRgn)
        return;

    RECT rc = {};
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0)
        return;

    // La región solo depende del tamaño y del radio. Reaplicarla en cada frame
    // creaba un HRGN y forzaba una recomposición completa de la ventana.
    static HWND appliedWindow = nullptr;
    static int appliedWidth = 0;
    static int appliedHeight = 0;
    static int appliedRadius = 0;
    int radius = GetSelectorCornerRadiusPx(width, height);
    if (appliedWindow == hwnd && appliedWidth == width &&
        appliedHeight == height && appliedRadius == radius)
        return;

    HRGN region = g_createRoundRectRgn(0, 0, width + 1, height + 1,
                                       radius * 2, radius * 2);
    if (!region)
        return;
    if (SetWindowRgn(hwnd, region, TRUE))
    {
        appliedWindow = hwnd;
        appliedWidth = width;
        appliedHeight = height;
        appliedRadius = radius;
    }
    else if (g_deleteObject)
        g_deleteObject(region);
}

static bool InitializePersistentSelector()
{
    if (g_selector && IsWindow(g_selector))
        return true;

    if (g_selectorAnimation == SelectorAnimationState::Closing)
        g_selectorAnimation = SelectorAnimationState::None;

    if (!RegisterSelectorClasses())
        return false;

    // Creada por el UI thread (Per-Monitor-V2) con el HMODULE del mod.
    HWND selector = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWindowClassName, L"",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hModule, nullptr);

    if (!selector)
    {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        return false;
    }

    g_selector = selector;
    RemoveNativeSelectorFrame(g_selector);
    ApplySelectorVisuals(g_selector);


    // Precalentar el renderer mientras el selector está oculto. Esto elimina
    // la creación de D2D/DWrite/fonts del camino crítico de Alt+Tab.
    LoadD2DAndDWrite();
    CreateUiFonts();
    UpdateSelectorControls();
    ShowWindow(g_selector, SW_HIDE);
    return true;
}

// Se ejecuta siempre en el UI thread, nunca en un hook.
static bool CreateSelector(bool shiftHeld)
{
    if (g_state != SelectorState::SelectorActive || g_cleanupInProgress)
        return false;

    // El selector se crea y precalienta únicamente durante la inicialización
    // del UI thread. Nunca se crea ni se carga en el camino de Alt+Tab.
    if (!g_selector || !IsWindow(g_selector))
        return false;

    g_selectorOpening = true;
    g_selectorOriginWindow = GetForegroundWindow();
    MarkSelectorActivity();

    g_pendingCloseWindows.clear();
    RefreshWindowList();
    if (g_groups.empty())
    {
        g_selectorOpening = false;
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(g_selectorOriginWindow,
                                         MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo))
    {
        g_selectorOpening = false;
        return false;
    }

    const RECT& work = monitorInfo.rcWork;
    UpdateUIScaleForWorkArea(work, GetMonitorDpiScale(monitor));
    // Si el monitor tiene otra escala, reconstruir TextFormat y fuentes GDI
    // antes de mostrar (no reutilizar formatos de otra escala).
    LoadD2DAndDWrite();
    CreateUiFonts();
    int x = work.left + ((work.right - work.left) - g_runtimeSelectorWidth) / 2;
    int y = work.top + ((work.bottom - work.top) - g_runtimeSelectorHeight) / 2;

    StopCarouselAnimation();

    // El HWND principal mantiene una superficie física estable durante la animación.
    SetWindowPos(g_selector, HWND_TOPMOST, x, y,
                 g_runtimeSelectorWidth, g_runtimeSelectorHeight,
                 SWP_NOACTIVATE | SWP_HIDEWINDOW);
    ApplySelectorRoundedRegion(g_selector);
    ApplySelectorVisuals(g_selector);
    // RefreshWindowList coloca la ventana que estaba en foreground en índice 0.
    // La primera pulsación respeta Shift: Alt+Tab avanza y Alt+Shift+Tab
    // retrocede desde la ventana actualmente activa.
    if (g_groups.size() > 1)
        g_selected = shiftHeld ? static_cast<int>(g_groups.size()) - 1 : 1;
    else
        g_selected = 0;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_animActive = false;
    ApplyScenePresence(0.0f);
    g_cardSnapScale = 1.0f;
    g_selectorAnimation = SelectorAnimationState::Opening;
    g_selectorAnimationStart = FrameClockNow();
    UpdateSelectorSurfaceTransform();

    CreateUiFonts();
    UpdateThumbnailSlots();

    // Allows the Alt+Tab window to take foreground when it is shown from
    // the WH_KEYBOARD_LL hook context.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    ShowWindow(g_selector, SW_SHOWNOACTIVATE);
    UpdateWindow(g_selector);
    UpdateSelectorSurfaceTransform();

    g_selectorOpening = false;
    ArmFramePump();
    ResetPatternWaves();
    if (IsPatternWavesActive())
        SetTimer(g_selector, kPatternTimerId, GetPatternTimerInterval(), nullptr);
    // Failsafe solo mientras el selector está abierto (sin polling permanente).
    SetTimer(g_selector, kActivityTimerId, 250, nullptr);
    return true;
}

static void HideSelectorForSession()
{
    if (!g_selector || !IsWindow(g_selector))
        return;

    KillTimer(g_selector, kAnimTimerId);
    KillTimer(g_selector, kTabRepeatTimerId);
    DisarmFramePump();
    KillTimer(g_selector, kPatternTimerId);
    KillTimer(g_selector, kActivityTimerId);
    g_tabRepeatStarted = false;
    g_uiTabDown = false;

    // No conservar superficies DWM registradas mientras el selector está oculto:
    // así ninguna miniatura puede sobrevivir visualmente al cierre. La siguiente
    // sesión las registra de nuevo mediante UpdateThumbnailSlots.
    UnregisterAllThumbnails();
    ShowWindow(g_selector, SW_HIDE);
    UiSetInteractive(false);
    g_selectorAnimation = SelectorAnimationState::None;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    g_sceneOpacity = 1.0f;
    g_scenePresence = 1.0f;
    g_carouselSpread = 1.0f;
    g_selectionTiltDirection = 0.0f;
    g_cardSnapScale = 1.0f;
    g_animActive = false;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitGroupIndex = -1;
    g_cardExitTarget = nullptr;
    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_selectorOpening = false;
    g_lastSelectorActivity = 0;
    g_pendingActivationTarget = nullptr;
    g_selectorOriginWindow = nullptr;

}

static void DestroySelector()
{
    // Esta función mantiene el HWND y el renderer vivos entre sesiones. El
    // desmontaje real ocurre únicamente al terminar el UI thread. No se toca
    // ClipCursor ni ningún estado global que Overhaulded no haya creado.
    ReleaseCapture();
    HideSelectorForSession();
}

// Solo UI thread, al terminar el hilo.
static void ShutdownPersistentSelector()
{

    if (g_selector && IsWindow(g_selector))
    {
        KillTimer(g_selector, kAnimTimerId);
        KillTimer(g_selector, kTabRepeatTimerId);
        DisarmFramePump();
        KillTimer(g_selector, kPatternTimerId);
        KillTimer(g_selector, kActivityTimerId);
        UnregisterAllThumbnails();
        HWND selector = g_selector;
        g_selector = nullptr;
        DestroyWindow(selector);
    }

    ReleaseGroupResources();
    UnloadD2DAndDWrite();
    UnloadDwmFunctions();
    DestroyUiFonts();
    g_groups.clear();
    g_selected = 0;
    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_tabRepeatStarted = false;
    g_uiTabDown = false;
    g_selectorOpening = false;
    g_lastSelectorActivity = 0;
    g_pendingActivationTarget = nullptr;
    g_selectorOriginWindow = nullptr;
}

static void EmergencyCloseSelector()
{
    if (g_cleanupInProgress)
        return;

    g_cleanupInProgress = true;
    LeaveActiveState(SelectorState::Canceling);
    if (g_selector && IsWindow(g_selector))
        KillTimer(g_selector, kTabRepeatTimerId);
    DestroySelector();
    g_state = SelectorState::Idle;
    g_cleanupInProgress = false;
    CleanupKeyboardState();
}

static void UpdateCarouselAnimation(HWND)
{
    if (!g_animActive)
        return;
    const float progress = ElapsedFraction(g_animStartTime,
                                           AnimationDuration(kNavigationDuration));
    // Posición lógica continua. La salida exponencial normalizada arranca a
    // máxima velocidad (sin el arranque lento de smoothstep) y llega a 0 sin
    // overshoot. Como cada pulsación suma a la posición actual, la navegación
    // interrumpida es continua en posición.
    g_animOffset = g_animStartOffset * (1.0f - EaseOutExp(progress, 5.0f));

    // Snap de la card que llega al centro: se deriva de la distancia restante
    // (no de un temporizador propio), por lo que siempre está sincronizado con
    // el movimiento de las cards.
    const float remaining = fabsf(g_animOffset);
    const float bump = Clamp01(1.0f - remaining / 0.30f);
    g_cardSnapScale = 1.0f + 0.045f * sinf(bump * 3.14159265358979323846f);

    if (progress >= 1.0f)
    {
        g_animActive = false;
        g_animOffset = 0.0f;
        g_animStartOffset = 0.0f;
        g_cardSnapScale = 1.0f;
    }
    UpdateSelectorControls();
}

static void StartSlide(int steps)
{
    if (steps == 0 || g_groups.empty())
        return;
    // Solo se marca SelectionChange desde Open: no se toca la escena si la
    // apertura o el cierre están en curso.
    if (g_selectorAnimation == SelectorAnimationState::Open)
    {
        g_selectorAnimation = SelectorAnimationState::SelectionChange;
        g_selectorAnimationStart = FrameClockNow();
    }
    g_selectionTiltDirection = steps > 0 ? -1.0f : 1.0f;

    int count = static_cast<int>(g_groups.size());
    g_selected = (g_selected + steps) % count;
    while (g_selected < 0)
        g_selected += count;

    g_animStartOffset = g_animOffset + static_cast<float>(steps);
    if (g_animStartOffset > 2.0f) g_animStartOffset = 2.0f;
    if (g_animStartOffset < -2.0f) g_animStartOffset = -2.0f;

    g_animOffset = g_animStartOffset;
    g_animStartTime = FrameClockNow();
    g_animActive = true;

    UpdateSelectorControls();
}

static void SelectNextSmooth(int steps)
{
    StartSlide(steps);
}

static void SelectPreviousSmooth(int steps)
{
    StartSlide(-steps);
}

static void SelectNext()
{
    SelectNextSmooth(1);
}

static void SelectPrevious()
{
    SelectPreviousSmooth(1);
}

static HWND GetNextGroupWindow(AppGroup& group)
{
    if (group.windows.empty())
        return nullptr;

    size_t cursor = group.windows.size();
    for (size_t i = 0; i < g_groupWindowCursors.size(); ++i)
    {
        if (g_groupWindowCursors[i].first == group.processId)
        {
            cursor = g_groupWindowCursors[i].second % group.windows.size();
            break;
        }
    }

    if (cursor == group.windows.size())
    {
        for (size_t i = 0; i < group.windows.size(); ++i)
        {
            if (group.windows[i] == group.representativeWindow)
            {
                cursor = i;
                break;
            }
        }
        if (cursor == group.windows.size())
            cursor = 0;
    }

    HWND target = nullptr;
    for (size_t attempt = 0; attempt < group.windows.size(); ++attempt)
    {
        size_t index = (cursor + attempt) % group.windows.size();
        if (group.windows[index] && IsWindow(group.windows[index]))
        {
            target = group.windows[index];
            cursor = (index + 1) % group.windows.size();
            break;
        }
    }

    bool stored = false;
    for (size_t i = 0; i < g_groupWindowCursors.size(); ++i)
    {
        if (g_groupWindowCursors[i].first == group.processId)
        {
            g_groupWindowCursors[i].second = cursor;
            stored = true;
            break;
        }
    }
    if (!stored)
        g_groupWindowCursors.push_back({ group.processId, cursor });
    return target;
}

// Solicita la activación al input thread sin bloquear el UI thread.
static void ActivateWindow(HWND target)
{
    if (!target || !IsWindow(target))
        return;

    DWORD inputThreadId = g_inputThreadId;
    if (inputThreadId)
        PostThreadMessageW(inputThreadId, WM_IN_ACTIVATE,
                           reinterpret_cast<WPARAM>(target), 0);
}

// Esta función solo se ejecuta desde InputThreadProc.
static void ActivateWindowOnInputThread(HWND target)
{
    if (!target || !IsWindow(target))
        return;

    if (IsIconic(target))
        ShowWindowAsync(target, SW_RESTORE);

    SetForegroundWindow(target);
}

static void ConfirmSelection()
{
    if (!IsSelectorActuallyActive() || g_cleanupInProgress)
        return;

    LeaveActiveState(SelectorState::Confirming);
    HWND target = nullptr;
    if (g_selected >= 0 && g_selected < static_cast<int>(g_groups.size()))
    {
        AppGroup& group = g_groups[g_selected];
        target = GetNextGroupWindow(group);
    }

    if (target && target == g_selectorOriginWindow)
    {
        LeaveActiveState(SelectorState::Canceling);
        StartSelectorClose(nullptr);
        return;
    }

    // Confirmar una ventana no es una cancelación: no ejecutar el CRT.
    // El selector se limpia inmediatamente y la ventana destino se activa.
    g_cleanupInProgress = true;
    DestroySelector();
    g_state = SelectorState::Idle;
    g_cleanupInProgress = false;
    CleanupKeyboardState();
    ActivateWindow(target);
}

static void CancelSelection()
{
    if ((!IsSelectorActuallyActive() && !g_selector) || g_cleanupInProgress)
        return;

    LeaveActiveState(SelectorState::Canceling);
    StartSelectorClose(nullptr);
}

// COMANDOS EN EL UI THREAD

static bool UiCommandMatchesSession(WPARAM sessionId)
{
    return static_cast<LONG>(sessionId) == g_uiSessionId && IsSelectorActive();
}

static void UiOpen(LONG sessionId, bool shiftHeld)
{
    g_uiSessionId = sessionId;
    g_uiTabDown = false;
    g_state = SelectorState::SelectorActive;

    if (!CreateSelector(shiftHeld))
    {
        // Nada que mostrar (o precarga no disponible): terminar la sesión.
        g_state = SelectorState::Idle;
        UiEndSession();
        return;
    }

    UiSetInteractive(true);
    g_uiTabDown = true;
    g_tabRepeatStarted = false;
    SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
}

static void UiHandleMouseMove()
{
    InterlockedExchange(&g_mouseMovePending, 0);
    LONG64 packed = InterlockedCompareExchange64(&g_lastMousePt, 0, 0);
    POINT pt = {};
    pt.x = static_cast<LONG>(static_cast<ULONG>(static_cast<ULONG64>(packed) & 0xFFFFFFFFULL));
    pt.y = static_cast<LONG>(static_cast<ULONG>((static_cast<ULONG64>(packed) >> 32) & 0xFFFFFFFFULL));

    if (!IsSelectorActive() || !g_selector || !IsWindow(g_selector))
        return;
    ScreenToClient(g_selector, &pt);
    g_backgroundSpotlightPoint = pt;
    RECT clientRect = {};
    GetClientRect(g_selector, &clientRect);
    g_backgroundSpotlightActive = PtInRect(&clientRect, pt) &&
                                   IsBackgroundSpotlightAllowed();
    UpdatePatternCursor(pt);
    // Apply the cursor impulse immediately on the UI thread instead of waiting
    // for the next 33/50 Hz Pattern timer tick. This keeps the wave response
    // synchronized with mouse movement while the timer continues propagating
    // the already-created wave field between mouse events.
    if (IsPatternWavesActive() && g_patternCursorInteraction)
        UpdatePatternWaves();
    UpdateHoveredSlot(pt);
    // Los cambios de hover piden su propio frame; solo el spotlight y Pattern
    // Waves dependen de cada movimiento del cursor. Se agrupan por frame.
    if (g_backgroundSpotlightActive || IsPatternWavesActive())
        RequestFrame();
    MarkSelectorActivity();
}

static void UiHandleMouseButton(UINT message, POINT screenPt)
{
    if (!IsSelectorActive() || !g_selector || !IsWindow(g_selector))
        return;
    MarkSelectorActivity();

    RECT selRect = {};
    GetWindowRect(g_selector, &selRect);

    if (!PtInRect(&selRect, screenPt))
    {
        g_hoveredSlot = -1;
        g_hoveredCloseButton = false;
        CancelSelection();
        return;
    }

    if (message == WM_LBUTTONDOWN)
    {
        POINT clientPt = screenPt;
        ScreenToClient(g_selector, &clientPt);
        int clickedSlot = GetSlotAtPoint(clientPt);
        if (clickedSlot >= 0)
        {
            RECT closeRect = GetCloseHoverButtonRect(clickedSlot);
            if (g_hoveredSlot == clickedSlot && g_hoveredCloseButton &&
                PtInRect(&closeRect, clientPt))
            {
                CloseWindowForSlot(clickedSlot);
            }
            else if (clickedSlot == GetCarouselCenterSlot())
            {
                ConfirmSelection();
            }
            else
            {
                StartSlide(clickedSlot - GetCarouselCenterSlot());
            }
        }
    }
}

static void UiHandleSettingsChanged()
{
    LoadTaskSwitcherSettings();
    for (auto& group : g_groups)
    {
        if (group.surfaceBrushNormal)
        {
            group.surfaceBrushNormal->Release();
            group.surfaceBrushNormal = nullptr;
        }
        if (group.surfaceBrushSelected)
        {
            group.surfaceBrushSelected->Release();
            group.surfaceBrushSelected = nullptr;
        }
    }
    if (g_selector && IsWindow(g_selector))
    {
        // A visible-card count change can invalidate existing DWM slot mapping.
        UnregisterAllThumbnails();
        ResetPatternWaves();
        if (IsPatternWavesActive())
        {
            if (IsSelectorActive())
                SetTimer(g_selector, kPatternTimerId, GetPatternTimerInterval(), nullptr);
            else
                KillTimer(g_selector, kPatternTimerId);
        }
        else
        {
            KillTimer(g_selector, kPatternTimerId);
        }
        ApplySelectorVisuals(g_selector);
        UpdateSelectorSurfaceTransform();
        UpdateSelectorControls();
    }
}

static void HandleUiCommand(const MSG& msg)
{
    switch (msg.message)
    {
    case WM_UI_OPEN:
        UiOpen(static_cast<LONG>(msg.wParam), msg.lParam != 0);
        break;

    case WM_UI_TAB_DOWN:
        if (!UiCommandMatchesSession(msg.wParam))
            break;
        MarkSelectorActivity();
        if (msg.lParam != 0) SelectPrevious();
        else SelectNext();
        g_uiTabDown = true;
        g_tabRepeatStarted = false;
        SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
        break;

    case WM_UI_TAB_UP:
        if (static_cast<LONG>(msg.wParam) != g_uiSessionId)
            break;
        g_uiTabDown = false;
        g_tabRepeatStarted = false;
        if (g_selector && IsWindow(g_selector))
            KillTimer(g_selector, kTabRepeatTimerId);
        break;

    case WM_UI_NAVIGATE:
        if (!UiCommandMatchesSession(msg.wParam))
            break;
        MarkSelectorActivity();
        if (msg.lParam < 0) SelectPrevious();
        else SelectNext();
        break;

    case WM_UI_CONFIRM:
        if (static_cast<LONG>(msg.wParam) == g_uiSessionId)
            ConfirmSelection();
        break;

    case WM_UI_CANCEL:
        if (static_cast<LONG>(msg.wParam) == g_uiSessionId)
            CancelSelection();
        break;

    case WM_UI_MOUSE_MOVE:
        UiHandleMouseMove();
        break;

    case WM_UI_MOUSE_BUTTON:
    {
        POINT pt = {};
        pt.x = static_cast<short>(LOWORD(msg.lParam));
        pt.y = static_cast<short>(HIWORD(msg.lParam));
        UiHandleMouseButton(static_cast<UINT>(msg.wParam), pt);
        break;
    }

    case WM_UI_MOUSE_WHEEL:
    {
        if (!IsSelectorActive())
            break;
        MarkSelectorActivity();
        short delta = static_cast<short>(LOWORD(msg.wParam));
        if (delta > 0)
            SelectNextSmooth(1);
        else if (delta < 0)
            SelectPreviousSmooth(1);
        break;
    }

    case WM_UI_SETTINGS:
        UiHandleSettingsChanged();
        break;
    }
}

// HOOKS (input thread). Callbacks mínimos: estado local/atómico + PostThreadMessage.

static bool InputSessionActive()
{
    return g_inputSessionId != 0 &&
           InterlockedCompareExchange(&g_sessionActive, 0, 0) == g_inputSessionId;
}

// Si el UI cerró la sesión (clic, timeout, ventana inválida), reconciliar.
static void SyncInputSession()
{
    if (g_inputSessionId != 0 && !InputSessionActive())
        ClearSelectorKeyboardSession();
}

static void InputEndSession(UINT command)
{
    LONG id = g_inputSessionId;
    if (id == 0)
        return;
    InterlockedCompareExchange(&g_sessionActive, 0, id);
    ClearSelectorKeyboardSession();
    PostUiCommand(command, static_cast<WPARAM>(id), 0);
}

static bool IsKeyDownMessage(WPARAM message)
{
    return message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
}

static bool IsKeyUpMessage(WPARAM message)
{
    return message == WM_KEYUP || message == WM_SYSKEYUP;
}

static bool SessionModifierReleased()
{
    if (g_sessionModifier == ModifierSession::LeftAlt)
        return !g_altLeftDown;

    if (g_sessionModifier == ModifierSession::RightAlt)
        return !g_altRightDown;

    if (g_sessionModifier == ModifierSession::AltGr)
        return !g_altLeftDown && !g_altRightDown &&
               !g_ctrlLeftDown && !g_ctrlRightDown;

    return false;
}

static void SendAltMenuMaskIfNeeded()
{
    if (!g_altMenuMaskPending)
        return;

    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = 0xE8;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0xE8;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
    g_altMenuMaskPending = false;
}

// Al soltar modificadores: confirmar si termina la sesión y desarmar el bloqueo
// de Ctrl+Alt+Flecha cuando AltGr ya no está pulsado.
static void OnModifierReleased()
{
    if (InputSessionActive() && SessionModifierReleased())
        InputEndSession(WM_UI_CONFIRM);
    g_altGrActive = IsAltGrPhysicallyDown();
    if (!InputSessionActive() && !g_altGrActive)
        g_altGrTaskSwitcherArmed = false;
}

static LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode != HC_ACTION || !lParam)
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    // En layouts con AltGr el sistema genera un Ctrl izquierdo "falso" (scancode
    // 0x21D). Se procesa aunque venga marcado como inyectado para no perder el
    // estado de AltGr; el resto de teclas inyectadas se ignoran.
    const bool fakeAltGrCtrl = key->vkCode == VK_LCONTROL && key->scanCode == 0x21D;
    if ((key->flags & LLKHF_INJECTED) != 0 && !fakeAltGrCtrl)
    {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    SyncInputSession();

    bool up = IsKeyUpMessage(wParam) || ((key->flags & LLKHF_UP) != 0);
    bool down = !up && IsKeyDownMessage(wParam);
    DWORD vk = key->vkCode;

    bool isLeftAlt = (vk == VK_LMENU) || (vk == VK_MENU && (key->flags & LLKHF_EXTENDED) == 0);
    bool isRightAlt = (vk == VK_RMENU) || (vk == VK_MENU && (key->flags & LLKHF_EXTENDED) != 0);
    bool isLeftCtrl = (vk == VK_LCONTROL) || (vk == VK_CONTROL && (key->flags & LLKHF_EXTENDED) == 0);
    bool isRightCtrl = (vk == VK_RCONTROL) || (vk == VK_CONTROL && (key->flags & LLKHF_EXTENDED) != 0);

    // This tool mod runs inside windhawk.exe. Consume Alt only while its
    // process owns the foreground window; all other applications retain the
    // normal Alt behavior.
    bool windhawkForeground = false;
    HWND foreground = GetForegroundWindow();
    DWORD foregroundProcessId = 0;
    if (foreground && GetWindowThreadProcessId(foreground, &foregroundProcessId))
        windhawkForeground = foregroundProcessId == GetCurrentProcessId();

    if (isLeftAlt)
    {
        if (down)
            g_altLeftDown = true;
        if (up)
        {
            g_altLeftDown = false;
            SendAltMenuMaskIfNeeded();
            if (InputSessionActive() && SessionModifierReleased())
                OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        if (windhawkForeground)
        {
            return 1;
        }
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isRightAlt)
    {
        if (down)
            g_altRightDown = true;
        if (up)
        {
            g_altRightDown = false;
            SendAltMenuMaskIfNeeded();
            if (InputSessionActive() && SessionModifierReleased())
                OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        if (windhawkForeground)
        {
            return 1;
        }
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isRightCtrl)
    {
        if (down)
            g_ctrlRightDown = true;
        if (up)
        {
            g_ctrlRightDown = false;
            OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isLeftCtrl)
    {
        if (down)
            g_ctrlLeftDown = true;
        if (up)
        {
            g_ctrlLeftDown = false;
            OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    const bool sessionActive = InputSessionActive();

    if (vk == VK_ESCAPE && down && sessionActive)
    {
        InputEndSession(WM_UI_CANCEL);
        return 1;
    }

    // AltGr se representa como Ctrl + Alt derecho. Cuando AltGr+Tab ya armó
    // nuestro selector, nunca dejamos que Ctrl+Alt+Flecha alcance al sistema
    // y active, por ejemplo, la rotación de pantalla. El bloqueo está limitado
    // a esta sesión; Ctrl+Alt+Flecha normal fuera del selector sigue intacto.
    if (g_altGrTaskSwitcherArmed &&
        (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN))
    {
        if (sessionActive && down)
        {
            if (vk == VK_LEFT)
                PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                              static_cast<LPARAM>(-1));
            else if (vk == VK_RIGHT)
                PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                              static_cast<LPARAM>(1));
        }
        // Up/Down and any key-up are simply consumed so Ctrl+Alt+Arrow
        // never reaches the Windows display-rotation shortcut.
        return 1;
    }

    if (sessionActive && down && vk == VK_LEFT)
    {
        PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                      static_cast<LPARAM>(-1));
        return 1;
    }

    if (sessionActive && down && vk == VK_RIGHT)
    {
        PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                      static_cast<LPARAM>(1));
        return 1;
    }

    if (vk == VK_RETURN && sessionActive)
    {
        if (down)
        {
            InputEndSession(WM_UI_CONFIRM);
        }
        return 1;
    }

    if (vk == VK_TAB)
    {
        if (down)
        {
            if (!sessionActive)
            {
                // Alt izquierdo, Alt derecho y AltGr se distinguen a partir del
                // estado rastreado por eventos (más GetAsyncKeyState como
                // respaldo para modificadores pulsados antes del hook). Ya no
                // se usa un flag AltGr "pegajoso": AltGr+otras teclas no abre
                // el selector ni deja estado residual.
                bool leftAlt = g_altLeftDown ||
                               ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0);
                bool rightAlt = g_altRightDown ||
                                ((GetAsyncKeyState(VK_RMENU) & 0x8000) != 0);
                bool ctrlHeld = g_ctrlLeftDown || g_ctrlRightDown ||
                                ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0);
                bool altGrHeld = rightAlt && ctrlHeld;

                if (leftAlt || rightAlt)
                {
                    LONG id = InterlockedIncrement(&g_nextSessionId);
                    if (id == 0)
                        id = InterlockedIncrement(&g_nextSessionId);
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

                    g_inputSessionId = id;
                    InterlockedExchange(&g_sessionActive, id);
                    g_sessionModifier = altGrHeld
                        ? ModifierSession::AltGr
                        : (leftAlt ? ModifierSession::LeftAlt
                                   : ModifierSession::RightAlt);
                    g_tabDown = true;
                    g_tabSuppressed = true;
                    g_altGrActive = altGrHeld;
                    g_altGrTaskSwitcherArmed = altGrHeld;

                    // La primera pulsación se consume EN ESTE MISMO callback.
                    // No esperamos a que el UI thread abra el selector antes de
                    // bloquearla: Windows nunca recibe este Tab como Alt+Tab.
                    const bool posted = PostUiCommand(
                        WM_UI_OPEN, static_cast<WPARAM>(id), shift ? 1 : 0);

                    if (posted)
                    {
                        g_altMenuMaskPending = !altGrHeld;
                        return 1;
                    }

                    // Para AltGr el bloqueo es deliberado incluso si el UI thread
                    // no está disponible. Es preferible perder esa pulsación a
                    // dejar que Windows abra su Task Switcher nativo.
                    if (altGrHeld)
                    {
                        InterlockedCompareExchange(&g_sessionActive, 0, id);
                        ClearSelectorKeyboardSession();
                        g_altGrTaskSwitcherArmed = false;
                        return 1;
                    }

                    // Alt/Alt+Tab tradicional conserva su fallback anterior si
                    // nuestro UI no está disponible.
                    InterlockedCompareExchange(&g_sessionActive, 0, id);
                    ClearSelectorKeyboardSession();
                    g_altGrTaskSwitcherArmed = false;
                    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
                }
            }
            else
            {
                if (!g_tabDown)
                {
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    g_tabDown = true;
                    PostUiCommand(WM_UI_TAB_DOWN, static_cast<WPARAM>(g_inputSessionId),
                                  shift ? 1 : 0);
                }
                return 1;
            }
        }
        else if (up)
        {
            g_tabDown = false;
            if (sessionActive)
            {
                PostUiCommand(WM_UI_TAB_UP, static_cast<WPARAM>(g_inputSessionId), 0);
            }
            if (sessionActive || g_tabSuppressed)
            {
                g_tabSuppressed = false;
                return 1;
            }
        }
    }

    if (sessionActive)
    {
        if (vk == VK_LWIN || vk == VK_RWIN || vk == VK_APPS)
        {
            return 1;
        }

        if (vk != VK_SHIFT && vk != VK_LSHIFT && vk != VK_RSHIFT &&
            !isLeftAlt && !isRightAlt && !isLeftCtrl && !isRightCtrl)
        {
            return 1;
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Solo interviene mientras el UI thread declara el selector interactivo.
    if (nCode != HC_ACTION || !lParam ||
        InterlockedCompareExchange(&g_uiSelectorOpen, 0, 0) == 0)
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);

    const MSLLHOOKSTRUCT* mouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

    if (wParam == WM_MOUSEMOVE)
    {
        LONG64 packed = static_cast<LONG64>(
            (static_cast<ULONG64>(static_cast<ULONG>(mouse->pt.y)) << 32) |
            static_cast<ULONG64>(static_cast<ULONG>(mouse->pt.x)));
        InterlockedExchange64(&g_lastMousePt, packed);
        // Coalescencia: como máximo un WM_UI_MOUSE_MOVE pendiente. El hook
        // observa el movimiento pero no lo suprime.
        if (InterlockedExchange(&g_mouseMovePending, 1) == 0)
        {
            if (!PostUiCommand(WM_UI_MOUSE_MOVE, 0, 0))
                InterlockedExchange(&g_mouseMovePending, 0);
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
        wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN ||
        wParam == WM_NCRBUTTONDOWN || wParam == WM_NCMBUTTONDOWN)
    {
        LPARAM packedPt = MAKELPARAM(static_cast<WORD>(static_cast<short>(mouse->pt.x)),
                                     static_cast<WORD>(static_cast<short>(mouse->pt.y)));
        if (!PostUiCommand(WM_UI_MOUSE_BUTTON, wParam, packedPt))
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        return 1;
    }

    if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP ||
        wParam == WM_MBUTTONUP || wParam == WM_NCLBUTTONUP ||
        wParam == WM_NCRBUTTONUP || wParam == WM_NCMBUTTONUP)
    {
        return 1;
    }

    if (wParam == WM_MOUSEWHEEL)
    {
        short delta = static_cast<short>(HIWORD(mouse->mouseData));
        if (!PostUiCommand(WM_UI_MOUSE_WHEEL,
                           static_cast<WPARAM>(static_cast<USHORT>(delta)), 0))
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        return 1;
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// HILOS

static DWORD WINAPI InputThreadProc(LPVOID)
{
    SetThreadPerMonitorAwareV2();

    // Crear la cola de mensajes antes de publicar la disponibilidad.
    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    InputResetKeyboardState();

    bool ok = false;
    HWND control = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0,
                                   HWND_MESSAGE, nullptr, g_hModule, nullptr);
    if (!control)
    {
        Wh_Log(L"Input control window creation failed: %lu", GetLastError());
    }
    else
    {
        g_inputControlWnd = control;

        // A low-level hook callback executes on the installing input thread.
        // Prefer the mod module, but retry without an hMod for tool-process
        // loaders that cannot resolve the injected module for a global hook.
        SetLastError(ERROR_SUCCESS);
        g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook,
                                           g_hModule, 0);
        DWORD hookError = g_keyboardHook ? ERROR_SUCCESS : GetLastError();
        if (!g_keyboardHook && hookError == ERROR_MOD_NOT_FOUND)
        {
            SetLastError(ERROR_SUCCESS);
            g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook,
                                               nullptr, 0);
            if (!g_keyboardHook)
                hookError = GetLastError();
        }
        ok = g_keyboardHook != nullptr;
        if (!ok)
            Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) failed: %lu", hookError);
    }
    if (!ok && control)
        Wh_Log(L"Input thread initialization failed");

    InterlockedExchange(&g_inputInitOk, ok ? 1 : 0);
    SetEvent(g_inputReadyEvent);

    if (ok)
    {
        while (GetMessageW(&message, nullptr, 0, 0) > 0)
        {
            if (message.message == WM_IN_ACTIVATE)
            {
                ActivateWindowOnInputThread(
                    reinterpret_cast<HWND>(message.wParam));
                continue;
            }
            if (message.message == WM_IN_MOUSEHOOK)
            {
                if (message.wParam != 0)
                {
                    if (!g_mouseHook)
                    {
                        SetLastError(ERROR_SUCCESS);
                        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook,
                                                        g_hModule, 0);
                        if (!g_mouseHook && GetLastError() == ERROR_MOD_NOT_FOUND)
                        {
                            SetLastError(ERROR_SUCCESS);
                            g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook,
                                                            nullptr, 0);
                        }
                        if (!g_mouseHook)
                            Wh_Log(L"SetWindowsHookEx(WH_MOUSE_LL) failed: %lu",
                                   GetLastError());
                    }
                }
                else if (g_mouseHook)
                {
                    UnhookWindowsHookEx(g_mouseHook);
                    g_mouseHook = nullptr;
                }
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    // El propio hilo retira sus hooks y destruye su ventana.
    if (g_keyboardHook)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    if (g_mouseHook)
    {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    g_inputControlWnd = nullptr;
    if (control)
        DestroyWindow(control);
    return 0;
}

static bool InitializeUiThreadState()
{
    LoadTaskSwitcherSettings();
    if (!RegisterSelectorClasses())
        return false;

    // Escala del monitor principal para que el precalentado (D2D/DWrite/fuentes)
    // ya use la escala correcta en el caso habitual.
    POINT origin = { 0, 0 };
    UpdateUIScaleForMonitor(MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY));

    InitFramePump();
    if (!InitializePersistentSelector())
        return false;

    RegisterUserWindow(GetForegroundWindow(), true);

    // MRU por evento, sin polling. OUTOFCONTEXT: se entrega en este hilo.
    g_winEventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                     nullptr, ForegroundWinEventProc, 0, 0,
                                     WINEVENT_OUTOFCONTEXT);
    if (!g_winEventHook)
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
    return true;
}

static void ShutdownUiThreadState()
{
    if (g_winEventHook)
    {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    if (g_foregroundRetryTimer)
    {
        KillTimer(nullptr, g_foregroundRetryTimer);
        g_foregroundRetryTimer = 0;
    }
    g_foregroundRetryWindow = nullptr;

    if (g_selector || g_state != SelectorState::Idle)
        EmergencyCloseSelector();
    else
        CleanupKeyboardState();

    // El hilo propietario destruye la ventana y libera D2D/DWrite/GDI/DWM.
    ShutdownPersistentSelector();
    ShutdownFramePump();
    UnregisterSelectorClasses();

    g_userWindows.clear();
    g_groups.clear();
    g_selected = 0;
    g_state = SelectorState::Idle;

    g_getDpiForMonitor = nullptr;
    if (g_shcoreApi)
    {
        FreeLibrary(g_shcoreApi);
        g_shcoreApi = nullptr;
    }
}

static DWORD WINAPI UiThreadProc(LPVOID)
{
    // Debe hacerse antes de crear cualquier ventana de este hilo.
    SetThreadPerMonitorAwareV2();

    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    bool ok = InitializeUiThreadState();
    InterlockedExchange(&g_uiInitOk, ok ? 1 : 0);
    SetEvent(g_uiReadyEvent);

    if (ok)
    {
        // Duerme hasta que llegue un mensaje o venza el frame timer. Sin
        // animación ni cambios pendientes el timer no está armado y el hilo no
        // consume CPU.
        bool running = true;
        while (running)
        {
            const DWORD handleCount = g_frameTimer ? 1 : 0;
            const DWORD wait = MsgWaitForMultipleObjectsEx(
                handleCount, handleCount ? &g_frameTimer : nullptr,
                INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (wait == WAIT_FAILED)
                break;
            if (handleCount && wait == WAIT_OBJECT_0)
                RunFrame();
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    running = false;
                    break;
                }
                if (message.hwnd == nullptr &&
                    message.message >= WM_UI_OPEN && message.message <= WM_UI_SETTINGS)
                {
                    HandleUiCommand(message);
                    continue;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }

    ShutdownUiThreadState();
    return 0;
}

// Arranca un hilo y espera sin timeout a que esté listo o haya terminado.
// El evento no se cierra mientras el hilo pueda seguir usándolo.
static bool StartWorkerThread(LPTHREAD_START_ROUTINE proc, HANDLE* outThread,
                              DWORD* outThreadId, HANDLE* outEvent,
                              volatile LONG* okFlag)
{
    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent)
        return false;
    *outEvent = readyEvent;

    DWORD threadId = 0;
    HANDLE thread = CreateThread(nullptr, 0, proc, nullptr, 0, &threadId);
    if (!thread)
    {
        CloseHandle(readyEvent);
        *outEvent = nullptr;
        return false;
    }

    HANDLE waits[2] = { readyEvent, thread };
    DWORD result = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    bool ok = (result == WAIT_OBJECT_0) &&
              InterlockedCompareExchange(okFlag, 0, 0) == 1;
    if (!ok)
    {
        // El hilo ya terminó o está terminando por sí mismo: esperar su fin
        // antes de cerrar handles.
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
        CloseHandle(readyEvent);
        *outEvent = nullptr;
        return false;
    }

    *outThread = thread;
    *outThreadId = threadId;
    return true;
}

// TOOL MOD

BOOL WhTool_ModInit()
{

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&SelectorWndProc), &g_hModule);
    if (!g_hModule)
        return FALSE;

    InterlockedExchange(&g_shutdownRequested, 0);
    InterlockedExchange(&g_sessionActive, 0);
    InterlockedExchange(&g_uiSelectorOpen, 0);
    InterlockedExchange(&g_mouseMovePending, 0);
    g_state = SelectorState::Idle;

    // 1) UI thread: crea el selector y precalienta el renderer.
    DWORD uiThreadId = 0;
    if (!StartWorkerThread(UiThreadProc, &g_uiThread, &uiThreadId,
                           &g_uiReadyEvent, &g_uiInitOk))
        return FALSE;
    g_uiThreadId = uiThreadId;

    // 2) Input thread: solo hooks. Se publica cuando el selector ya existe.
    if (!StartWorkerThread(InputThreadProc, &g_inputThread, &g_inputThreadId,
                           &g_inputReadyEvent, &g_inputInitOk))
    {
        InterlockedExchange(&g_shutdownRequested, 1);
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
        g_uiThreadId = 0;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    // Los ajustes los carga exclusivamente el UI thread.
    PostUiCommand(WM_UI_SETTINGS, 0, 0);
}

void WhTool_ModUninit()
{
    InterlockedExchange(&g_shutdownRequested, 1);

    // 1) Input thread: retira sus hooks, destruye su ventana y termina.
    if (g_inputThread)
    {
        PostThreadMessageW(g_inputThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_inputThread, INFINITE);
        CloseHandle(g_inputThread);
        g_inputThread = nullptr;
    }

    // 2) UI thread: retira el WinEvent hook, destruye la ventana y libera
    //    D2D/DWrite/GDI/DWM antes de terminar.
    if (g_uiThread)
    {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    // Ningún hilo puede usar ya los eventos: cerrarlos ahora es seguro.
    if (g_inputReadyEvent)
    {
        CloseHandle(g_inputReadyEvent);
        g_inputReadyEvent = nullptr;
    }
    if (g_uiReadyEvent)
    {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
    }
    g_inputThreadId = 0;
    g_uiThreadId = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
