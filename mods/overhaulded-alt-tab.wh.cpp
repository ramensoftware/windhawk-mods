// ==WindhawkMod==
// @id              overhaulded-alt-tab
// @name            OverhauldedWin Alt+Tab
// @description     Replaces the boring Windows Alt+Tab with a modern and elegant window switcher.
// @version         1.3.1
// @author          IMiloDev
// @github          https://github.com/IMiloDev
// @homepage        https://github.com/IMiloDev/OverhauldedWin
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -lshell32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Overhaulded Task Switcher 

A modern, fluid and highly visual replacement for the native Windows Alt+Tab experience.

Built from scratch in native C++ as a project to explore Windows APIs, graphics, animation systems and desktop customization. You can also see the original [GITHUB REPOSITORY](https://github.com/IMiloDev/OverhauldedWin) to send me issues.

## Screenshot

![OverhauldedWin Task Switcher](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/task-manager.jpg)

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

![OverhauldedWin Open Animation](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/open-close.webp)

### Navigation

![OverhauldedWin](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/desplace.webp)

### Window Close

![OverhauldedWin](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/window-close.webp)

## Background Execution

Overhaulded remains prepared in the background instead of creating the entire task-switcher interface from scratch every time Alt+Tab is pressed.

This allows the selector to appear immediately while keeping the visual opening animation smooth.

The architecture separates Explorer's shell-hotkey forwarding, modifier-state polling, selector-window keyboard handling, and UI/rendering. The Explorer hook only forwards the hotkey; rendering and window management run on the switcher's UI thread.

---


## Controls

| Shortcut                                     | Action                        |
| -------------------------------------------- | ----------------------------- |
| `Alt + Tab`                                  | Move to the next window       |
| `Alt + Shift + Tab`                          | Move to the previous window   |
| `AltGr + Tab`                                | Move to the next window       |
| `Alt + Tab` + release `Alt`                  | Activate the selected window  |
| `Esc`                                        | Cancel the switcher           |
| `Alt + Tab` + arrows                       | Navigate through the selector  |
| `AltGr + Tab` + arrows                      | Navigate through the selector  |

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
Overhaulded is still a pre-release project.

Some applications may behave differently than standard desktop windows, especially those using unusual window structures, custom rendering, or multiple processes.

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
      - "100": 100% (default)
      - "110": 110%
      - "125": 125%
    - Cards:
        - VisibleCards: "5"
          $name: Visible cards
          $description: Number of cards shown in the carousel.
          $options:
          - "3": 3 cards
          - "5": 5 cards (default)
        - SingleWindowCard: true
          $name: Single card when one window is open
          $description: Show only the center card when there is just one eligible window.
        - NormalColor: "#050507"
          $name: Cards color
          $description: Base color of unselected cards.
        - SelectedColor: "#090A0B"
          $name: Selected card color
          $description: Base color of the selected card.
        - ExpandSelectedWindow: true
          $name: Expand selected window
          $description: Animate the selected card toward the selected window when confirmed.
        - MotionStyle: "Smooth"
          $name: Card motion style
          $description: Controls card-to-card navigation independently from the background opening and closing animation.
          $options:
          - "Smooth": Smooth (no overshoot)
          - "Linear": Linear
          - "Bouncy": Bouncy
        - MotionSpeed: "100"
          $name: Card motion speed
          $description: Controls horizontal drag sensitivity and card navigation duration. Higher values move faster.
          $options:
          - "75": 75% (slower)
          - "100": 100% (default)
          - "125": 125%
          - "150": 150%
          - "175": 175%
          - "200": 200% (fastest)
      $name: Cards
  $name: Appearance
- Animation:
    - Style: "Cinematic"
      $name: Background and selector animation style
      $description: Select the style for opening and closing the selector/background. Card navigation is configured separately under Appearance > Cards.
      $options:
      - "Linear": Linear
      - "Bouncy": Bouncy
      - "RCT": RCT (horizontal line reveal)
      - "Cinematic": Cinematic (depth and controlled overshoot)
    - Speed: "100"
      $name: Animation speed
      $description: Relative speed of animations.
      $options:
      - "75": 75%
      - "100": 100% (default)
      - "125": 125%
      - "150": 150%
    - PerformanceMode: "smooth"
      $name: Performance mode
      $description: Controls the animation update frequency.
      $options:
      - "smooth": Smooth
      - "balanced": Balanced
  $name: Animation
- Background: "dwm"
  $name: Background
  $description: Background layer behind the cards.
  $options:
  - "none": None
  - "dwm": Transparent - DWM
  - "blurred": Fullscreen
- Dwm:
    - BackgroundColor: "#141A22"
      $name: Background color
      $description: Color of the transparent DWM background behind the cards.
    - Opacity: 20
      $name: Opacity
      $description: DWM background opacity (0-100).
  $name: DWM background
- ExperimentalSettings:
    - GeneralBackground: "none"
      $name: General Background
      $description: Experimental background overlay. Pattern Waves may require additional GPU resources.
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
          $description: Pattern spacing (8-48).
        - Light: 120
          $name: Light
          $description: Apparent depth and illumination (0-200).
        - Contrast: 100
          $name: Contrast
          $description: Pattern contrast (0-200).
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
  $name: Experimental settings
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <utility>
#include <cwchar>
#include <cstring>
#include <tlhelp32.h>

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
    Dwm,
    Fullscreen
};

static bool g_blurredBackgroundReady = false;
static RECT g_blurredMonitorRect = {};
static RECT g_blurredInitialRect = {};
static POINT g_selectorSceneOffset = {};
static std::vector<DWORD> g_capturedDesktopPixels;
static std::vector<DWORD> g_blurredDesktopPixels;
static bool g_fullscreenHdcDrawFailureLogged = false;

// Captura de escritorio y superficies auxiliares legacy. La escena Fullscreen se
// presenta ahora en un HWND render target Direct2D de hardware, sin BitBlt final.
struct FsSurface
{
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    int width = 0;
    int height = 0;
};
static FsSurface g_fsSharp;
static FsSurface g_fsBlur;
static FsSurface g_fsBack;
static FsSurface g_fsScene;                      // superficie del área sucia (Direct2D)
static ID2D1Bitmap* g_fsSharpD2D = nullptr;      // escritorio nítido (GPU)
static ID2D1Bitmap* g_fsBlurD2D = nullptr;       // escritorio desenfocado (GPU)
static RECT g_paintDirty = {};                   // rcPaint del WM_PAINT actual
static float g_lastBackdropPresence = -1.0f;

// Sombra suave de las cards en GDI: sprite premultiplicado (solo alfa, negro)
// que se dibuja con AlphaBlend en 9 porciones. msimg32 se carga bajo demanda.
struct ShadowSprite
{
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    int size = 0;
    int margin = 0;
    int radius = 0;
};
static ShadowSprite g_shadowSprite;
typedef BOOL (WINAPI* AlphaBlendProc)(HDC, int, int, int, int, HDC, int, int,
                                      int, int, BLENDFUNCTION);
static HMODULE g_msimg32Module = nullptr;
static AlphaBlendProc g_pAlphaBlend = nullptr;
static BITMAPINFO g_blurredDesktopBitmapInfo = {};
// Equivalent black overlay multiplier; 0.58 leaves 58% of the blurred RGB values.
static const float kFullscreenBackdropBrightness = 0.58f;
static const float kFullscreenBlurStandardDeviation = 46.0f;
// Blur is computed at quarter resolution per axis (1/16 of the pixels).
// DWM/D2D scales it to the monitor during composition; the sharp reveal stays full-res.
static const int kFullscreenBlurDownsample = 4;
static void GetFullscreenBlurDimensions(int width, int height, int& blurWidth, int& blurHeight)
{
    blurWidth = std::max(1, (width + kFullscreenBlurDownsample - 1) / kFullscreenBlurDownsample);
    blurHeight = std::max(1, (height + kFullscreenBlurDownsample - 1) / kFullscreenBlurDownsample);
}

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
           !g_blurredBackgroundReady &&
           g_experimentalBackgroundMode == ExperimentalBackgroundMode::PatternWaves;
}

// The mouse spotlight is an independent background interaction. It remains
// available even when an experimental background such as Pattern Waves is active.
static bool IsBackgroundSpotlightAllowed()
{
    return g_baseBackgroundMode == BaseBackgroundMode::Dwm ||
           (g_baseBackgroundMode == BaseBackgroundMode::Fullscreen &&
            !g_blurredBackgroundReady);
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
// Temporary dimensions for rendering the same field in selector or monitor space.
static int g_patternFieldRenderWidth = 0;
static int g_patternFieldRenderHeight = 0;
static const int kCounterWidth = 140;
static const int kCounterHeight = 28;
static const int kCounterTop = 378;
// Opacidad común de las superficies Black Obsidian; los textos y thumbnails no se alteran.
static const float kCardSurfaceOpacity = 0.75f;
static COLORREF g_cardNormalColor = RGB(5, 5, 7);
static COLORREF g_cardSelectedColor = RGB(9, 10, 11);
// Timer del failsafe: solo existe mientras el selector está abierto.
static const UINT_PTR kActivityTimerId = 77;
static const UINT_PTR kTabRepeatTimerId = 89;
static const UINT_PTR kSelectorMotionTimerId = 90;
static const float kNavigationDuration = 150.0f; // Linear/Smooth default, reduced from 180 ms.
static const float kBouncyNavigationDuration = 180.0f; // Preserve the original spring timing.
static const int kDefaultAnimationFps = 120;
enum class AnimationStyle { Linear, Bouncy, Rct, Cinematic };
static AnimationStyle g_animationStyle = AnimationStyle::Linear;
enum class CardMotionStyle { Smooth, Linear, Bouncy };
static CardMotionStyle g_cardMotionStyle = CardMotionStyle::Smooth;
static int g_cardMotionSpeedPercent = 100;
static bool g_expandSelectedWindow = true;
static bool g_singleWindowCard = true;
static int g_animationFps = kDefaultAnimationFps;

static int GetCarouselCenterSlot() { return g_visibleCardCount / 2; }
static int GetCarouselSlotCount() { return g_visibleCardCount; }
static float AnimationDuration(float baseMs)
{
    return baseMs * 100.0f / static_cast<float>(g_animationSpeedPercent);
}
static float CardNavigationDuration()
{
    const float baseDuration = g_cardMotionStyle == CardMotionStyle::Bouncy
        ? kBouncyNavigationDuration : kNavigationDuration;
    return baseDuration * 100.0f /
        static_cast<float>(g_animationSpeedPercent) *
        100.0f / static_cast<float>(g_cardMotionSpeedPercent);
}
static void LoadTaskSwitcherSettings()
{
    auto read = [](PCWSTR key) {
        PCWSTR value = Wh_GetStringSetting(key);
        std::wstring result(value);
        Wh_FreeStringSetting(value);
        return result;
    };

    // Numeric options use integer settings and are clamped to their valid ranges.
    auto readNumber = [](PCWSTR key, int low, int high) {
        const int value = Wh_GetIntSetting(key);
        return std::max(low, std::min(high, value));
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

    std::wstring speed = read(L"Animation.Speed");
    if (speed.empty()) speed = read(L"Appearance.AnimationSpeed");
    if (speed.empty()) speed = read(L"AnimationSpeed");
    g_animationSpeedPercent = (speed == L"75" ? 75 : speed == L"125" ? 125 : speed == L"150" ? 150 : 100);
    std::wstring mode = read(L"Animation.PerformanceMode");
    if (mode.empty()) mode = read(L"Appearance.PerformanceMode");
    if (mode.empty()) mode = read(L"PerformanceMode");
    g_animationFps = mode == L"balanced" ? 90 : kDefaultAnimationFps;

    std::wstring style = read(L"Animation.Style");
    if (style.empty()) style = read(L"Appearance.AnimationStyle");
    g_animationStyle = style == L"Bouncy" ? AnimationStyle::Bouncy
                     : style == L"RCT" ? AnimationStyle::Rct
                     : style == L"Cinematic" ? AnimationStyle::Cinematic
                     : AnimationStyle::Linear;

    std::wstring cardMotion = read(L"Appearance.Cards.MotionStyle");
    g_cardMotionStyle = cardMotion == L"Linear" ? CardMotionStyle::Linear
                      : cardMotion == L"Bouncy" ? CardMotionStyle::Bouncy
                      : CardMotionStyle::Smooth;
    const int cardSpeed = readNumber(L"Appearance.Cards.MotionSpeed", 75, 200);
    g_cardMotionSpeedPercent = (cardSpeed == 75 || cardSpeed == 100 ||
        cardSpeed == 125 || cardSpeed == 150 || cardSpeed == 175 ||
        cardSpeed == 200) ? cardSpeed : 100;
    g_expandSelectedWindow =
        Wh_GetIntSetting(L"Appearance.Cards.ExpandSelectedWindow") != 0;
    g_singleWindowCard =
        Wh_GetIntSetting(L"Appearance.Cards.SingleWindowCard") != 0;

    // Background is the base layer and ExperimentalSettings.GeneralBackground
    // controls the optional overlay. Blur falls back to the regular DWM surface
    // for the current session if capture or processing cannot be prepared.
    std::wstring background = read(L"Background");
    Wh_Log(L"[FS] Background leido = '%s'", background.c_str());
    g_baseBackgroundMode = background == L"none"
        ? BaseBackgroundMode::None
        : background == L"blurred"
            ? BaseBackgroundMode::Fullscreen
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
// Cinematic ease-out: a small, controlled overshoot gives the scene a sense
// of momentum without changing the timing or spring model used by Bouncy.
static float EaseOutBack(float t, float overshoot = 1.15f)
{
    t = Clamp01(t);
    const float c1 = overshoot;
    const float c3 = c1 + 1.0f;
    const float u = t - 1.0f;
    return 1.0f + c3 * u * u * u + c1 * u * u;
}
static float EaseInCubic(float t)
{
    t = Clamp01(t);
    return t * t * t;
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
// Offset temporal usado cuando el HWND se amplía al área de trabajo.
static POINT g_selectionExpansionSceneOffset = {};
// Estado de la transición de confirmación. La ventana del selector se reutiliza
// y se amplía al área de trabajo; las miniaturas DWM se interpolan desde la
// tarjeta visible hasta ese destino, sin crear otro renderer ni otra ventana.
static bool g_selectionExpansionActive = false;
static bool g_selectionExpansionFinishPending = false;
// La superficie expandida permanece encima hasta que la ventana destino ya es
// foreground (o vence el timeout), evitando un frame vacío entre ambas.
static bool g_selectionActivationRequested = false;
static ULONGLONG g_selectionActivationRequestTick = 0;
static const ULONGLONG kSelectionActivationWaitMaxMs = 400;
static float g_selectionExpansionDurationMs = 300.0f;
static HWND g_selectionExpansionTarget = nullptr;
static RECT g_selectionExpansionStartRect = {};
static RECT g_selectionExpansionRect = {};
static RECT g_selectionExpansionDestinationRect = {};
static RECT g_selectionExpansionWorkRect = {};
static double g_selectionExpansionStart = 0.0;
static float g_selectionExpansionProgress = 0.0f;
// Progreso común de la escena: 0 = oculta/colapsada, 1 = completamente abierta.
// Escala, opacidad y apertura del carrusel se derivan de este único valor.
static float g_scenePresence = 1.0f;
static float g_closeStartPresence = 1.0f;
static float g_carouselSpread = 1.0f;
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
    // El cambio de tamaño/movimiento del HWND no debe mover la escena de la
    // tarjeta durante el primer frame de la expansión.
    transform.dx += static_cast<float>(g_selectorSceneOffset.x +
                                      g_selectionExpansionSceneOffset.x);
    transform.dy += static_cast<float>(g_selectorSceneOffset.y +
                                      g_selectionExpansionSceneOffset.y);
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
    OffsetRect(&result, g_selectorSceneOffset.x, g_selectorSceneOffset.y);
    return result;
}

typedef HRESULT (WINAPI* DwmGetCompositionTimingInfoFn)(HWND, DWM_TIMING_INFO*);
typedef HRESULT (WINAPI* DwmRegisterThumbnailFn)(HWND, HWND, HTHUMBNAIL*);
typedef HRESULT (WINAPI* DwmUnregisterThumbnailFn)(HTHUMBNAIL);
typedef HRESULT (WINAPI* DwmQueryThumbnailSourceSizeFn)(HTHUMBNAIL, PSIZE);
typedef HRESULT (WINAPI* DwmUpdateThumbnailPropertiesFn)(
    HTHUMBNAIL, const DWM_THUMBNAIL_PROPERTIES*);
typedef HRESULT (WINAPI* DwmSetWindowAttributeFn)(HWND, DWORD,
                                                   const void*, DWORD);
typedef HRESULT (WINAPI* DwmGetWindowAttributeFn)(HWND, DWORD,
                                                   void*, DWORD);

// Atributos y flags DWM usados por el glass y las miniaturas.
static const DWORD kDwmWaExtendedFrameBounds = 9;
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
    SelectionExpansion,
    Open,
    Closing
};

enum class ModifierSession
{
    None,
    LeftAlt,
    RightAlt,
    ControlAlt
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
    std::wstring windowTitle;
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
    std::wstring windowTitle;
    ULONGLONG lastActivated;
};

// ARQUITECTURA DE HILOS
//
//  Input thread : dueño de WH_MOUSE_LL (solo mientras el selector está
//                 interactivo), del hotkey reenviado y del polling de
//                 modificadores. El teclado del selector se procesa en su WndProc.
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
static HHOOK g_mouseHook = nullptr;
static HWND volatile g_inputControlWnd = nullptr;   // ventana message-only

// Estado compartido entre hooks y UI.
static volatile LONG g_nextSessionId = 0;
// Id de la sesión de selector actualmente activa (0 = ninguna). Lo publica el
// input thread al abrir; el UI thread solo lo limpia con CAS sobre su propio id.
static volatile LONG g_sessionActive = 0;
// 1 mientras el selector acepta interacción de ratón (el UI thread lo gobierna).
static volatile LONG g_uiSelectorOpen = 0;
// 1 durante la expansión de confirmación; input y UI deben rechazar nuevas
// sesiones para que otra pulsación no interrumpa la transición de 300 ms.
static volatile LONG g_selectionTransitionActive = 0;
static volatile LONG64 g_lastMousePt = 0;
static volatile LONG g_mouseMovePending = 0;

// Comandos input -> UI (PostThreadMessage) y UI -> input (ventana de control).
static const UINT WM_UI_OPEN = WM_APP + 1;         // wParam=sessionId, lParam=shift
static const UINT WM_UI_NAVIGATE = WM_APP + 4;     // wParam=sessionId, lParam=+1/-1
static const UINT WM_UI_CONFIRM = WM_APP + 5;      // wParam=sessionId
static const UINT WM_UI_CANCEL = WM_APP + 6;       // wParam=sessionId
static const UINT WM_UI_MOUSE_MOVE = WM_APP + 7;   // posición en g_lastMousePt
static const UINT WM_UI_MOUSE_BUTTON = WM_APP + 8; // wParam=mensaje, lParam=pt (16 bit)
static const UINT WM_UI_MOUSE_WHEEL = WM_APP + 9;  // wParam=delta (short)
static const UINT WM_UI_SETTINGS = WM_APP + 10;
static const UINT WM_IN_MOUSEHOOK = WM_APP + 20;   // wParam=1 instalar, 0 quitar
static const UINT WM_IN_ACTIVATE = WM_APP + 21;     // wParam=target HWND

// WM_UI_OPEN lParam: bit 0 = Shift pulsado (dirección inversa),
// bit 1 = sesión iniciada por el hotkey reenviado desde Explorer.
static const LPARAM kUiOpenShift = 1;
static const LPARAM kUiOpenFromHotkey = 2;

// Hotkey de Alt+Tab reenviado desde explorer.exe (ver sección EXPLORER).
// Explorer localiza la ventana de control del input thread por clase y título:
// FindWindow compara títulos sin enviar mensajes, así que el título publica si
// la herramienta está lista para aceptar sesiones.
static const WCHAR kInputControlClass[] = L"OverhauldedAltTabInput";
static const WCHAR kInputControlTitleIdle[] = L"Overhaulded Alt+Tab";
static const WCHAR kInputControlTitleReady[] = L"Overhaulded Alt+Tab (ready)";
static const WCHAR kHotkeyMessageName[] = L"OverhauldedAltTab_Hotkey";
static const WPARAM kHotkeyFlagBackwards = 1;
static const WPARAM kHotkeyFlagCtrl = 2;
static UINT g_hotkeyMessage = 0;
static void InitHotkeyMessage();
// Sondeo del Alt físico durante sesiones Alt/AltGr: el hook puede perder el
// key-up cuando el foreground es una ventana elevada.
static const UINT_PTR kModifierReleasePollTimerId = 1;
static const UINT kModifierReleasePollMs = 30;
static const int kModifierReleaseTicks = 5;
static const UINT_PTR kActivationRetryTimerId = 2;
static const UINT kActivationRetryMs = 50;
static const int kMaxActivationRetries = 20;
static bool g_modifierReleasePolling = false;     // solo input thread
static int g_modifierReleaseCount = 0;     // solo input thread
static HWND g_inputActivationTarget = nullptr; // solo input thread
static int g_inputActivationRetryCount = 0;     // solo input thread
static BOOL g_inputActivationLastSetForegroundResult = FALSE; // solo input thread

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
static DwmQueryThumbnailSourceSizeFn g_dwmQueryThumbnailSourceSize = nullptr;
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
// Mouse carousel drag is tracked on the UI thread. Selection is committed only
// on release, while g_animOffset provides continuous Direct2D/DWM card motion.
static bool g_mousePressActive = false;
static bool g_mouseDragging = false;
static int g_mousePressedSlot = -1;
static POINT g_mousePressScreenPoint = {};
static float g_mousePressAnimOffset = 0.0f;
static int g_mousePressSelected = 0;
static bool g_tabRepeatStarted = false;
static bool g_cleanupInProgress = false;
static bool g_selectorOpening = false;
static ULONGLONG g_lastSelectorActivity = 0;
static SelectorAnimationState g_selectorAnimation = SelectorAnimationState::None;
static double g_selectorAnimationStart = 0.0;
static HWND g_pendingActivationTarget = nullptr;
static HWND g_selectorOriginWindow = nullptr;
// El selector tomó el foco (solo sesiones iniciadas por el hotkey de Explorer).
static bool g_selectorFocusTaken = false;
static bool g_cardExitActive = false;
static int g_cardExitSlot = -1;
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
static ID2D1DCRenderTarget* g_d2dDCRenderTarget = nullptr; // Target GDI para los modos no fullscreen.
static ID2D1RenderTarget* g_d2dRenderTarget = nullptr;     // Target activo usado por los helpers D2D.
static ID2D1HwndRenderTarget* g_fsHwndRenderTarget = nullptr; // Fullscreen: presentacion D2D por GPU, sin HDC/BitBlt.
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
static void StartSelectionExpansion(HWND target, bool targetWasMinimized);
static void UpdateSelectionExpansion();
static bool IsSelectionExpansionActive();
static void UpdateSelectorMotion();
static void UpdateCarouselAnimation(HWND hwnd);
static float GetCardExitProgress();
static float GetCardExitOpacity();
static float GetCardExitOffsetY();
static void DestroySelector();
static bool InitializePersistentSelector();
static void HideSelectorForSession();
static void ActivateWindow(HWND target);
static HWND FocusRestoreTarget();
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
static void InvalidateSelectorForFrame();
static void ArmFramePump();
static void DisarmFramePump();
static void RunFrame();
static void ReleaseD2DDeviceResources();
static void ApplyScenePresence(float presence);
static bool PrepareFullscreenBackdrop(const RECT& monitorRect, const RECT& selectorRect);
static void HideFullscreenBackdrop(bool releasePixels);
static void UpdateFullscreenBackdrop();
static bool IsFullscreenBackdropActive();
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
    if (!g_d2dRenderTarget)
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
    if (FAILED(g_d2dRenderTarget->CreateGradientStopCollection(
            stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection)) ||
        !collection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};
    props.center = D2D1::Point2F(0.0f, 0.0f);
    props.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    props.radiusX = DpiPx(kSpotlightRadiusDip);
    props.radiusY = DpiPx(kSpotlightRadiusDip);

    HRESULT hr = g_d2dRenderTarget->CreateRadialGradientBrush(
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
        g_d2dRenderTarget->FillRectangle(&square, g_d2dBrush);
        return;
    }

    g_d2dRenderTarget->FillEllipse(
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
static bool g_altGrActive = false;
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
// instala WH_MOUSE_LL solo mientras el selector acepta interacción.
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

// Estado físico de los modificadores, actualizado al iniciar y por el sondeo
// periódico de la sesión (no depende de eventos de teclado globales).
static bool IsAltGrPhysicallyDown();
static void InputResetKeyboardState()
{
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    g_altGrActive = IsAltGrPhysicallyDown();
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
    g_sessionModifier = ModifierSession::None;
    g_inputSessionId = 0;
    g_altGrActive = IsAltGrPhysicallyDown();
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

// A 32-bit tool process is redirected from System32 to SysWOW64. Use the
// Sysnative alias for metadata/icon APIs when the target path names a native
// System32 executable, so 64-bit-only applications keep their real names/icons.
static std::wstring GetNativeSystemPathForApi(const std::wstring& path)
{
#if defined(_WIN64)
    return path;
#else
    BOOL wow64 = FALSE;
    if (!IsWow64Process(GetCurrentProcess(), &wow64) || !wow64)
        return path;

    wchar_t windowsDirectory[MAX_PATH + 1] = {};
    const UINT length = GetWindowsDirectoryW(windowsDirectory, ARRAYSIZE(windowsDirectory));
    if (!length || length >= ARRAYSIZE(windowsDirectory))
        return path;

    std::wstring systemPrefix(windowsDirectory, length);
    if (!systemPrefix.empty() && systemPrefix.back() != L'\\')
        systemPrefix.push_back(L'\\');
    systemPrefix += L"System32\\";
    if (path.size() < systemPrefix.size() ||
        _wcsnicmp(path.c_str(), systemPrefix.c_str(), systemPrefix.size()) != 0)
        return path;

    std::wstring nativePrefix(windowsDirectory, length);
    if (!nativePrefix.empty() && nativePrefix.back() != L'\\')
        nativePrefix.push_back(L'\\');
    nativePrefix += L"Sysnative\\";
    return nativePrefix + path.substr(systemPrefix.size());
#endif
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

    const std::wstring apiPath = GetNativeSystemPathForApi(path);
    DWORD handle = 0;
    DWORD size = getSize(apiPath.c_str(), &handle);
    if (size == 0 || size > 1024 * 1024)
    {
        FreeLibrary(version);
        return std::wstring();
    }
    std::vector<BYTE> data(size);
    std::wstring description;
    if (getInfo(apiPath.c_str(), 0, size, data.data()))
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

    int titleLength = GetWindowTextLengthW(hwnd);
    std::wstring windowTitle(titleLength > 0 ? static_cast<size_t>(titleLength) + 1 : 1, L'\0');
    int copiedTitle = GetWindowTextW(hwnd, windowTitle.data(), static_cast<int>(windowTitle.size()));
    if (copiedTitle > 0)
        windowTitle.resize(static_cast<size_t>(copiedTitle));
    else
        windowTitle.clear();

    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (item.hwnd == hwnd)
        {
            item.windowTitle = windowTitle;
            item.appName = appName;
            item.processPath = processPath;
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
    item.windowTitle = windowTitle;
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

    // Cada HWND válido es una tarjeta independiente, incluso si varias
    // ventanas pertenecen al mismo proceso.
    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (!IsRealUserApplicationWindow(item.hwnd, nullptr, nullptr, nullptr))
            continue;

        bool duplicate = false;
        for (size_t j = 0; j < g_groups.size(); ++j)
        {
            if (g_groups[j].representativeWindow == item.hwnd)
            {
                duplicate = true;
                break;
            }
        }
        if (duplicate)
            continue;

        AppGroup group = {};
        group.processId = item.processId;
        group.processPath = item.processPath;
        group.appName = item.appName;
        group.windowTitle = item.windowTitle;
        group.representativeWindow = item.hwnd;
        group.icon = GetApplicationIcon(item.hwnd, item.processPath, &group.ownIcon);
        group.iconBitmap = nullptr;
        group.windows.push_back(item.hwnd);
        g_groups.push_back(group);
    }

    // La ventana en primer plano va primero; el resto conserva el orden MRU.
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

static const std::wstring& MakeSlotText(const AppGroup& group)
{
    return group.windowTitle.empty() ? group.appName : group.windowTitle;
}

static const std::wstring& MakeSlotDisplayText(const AppGroup& group)
{
    return MakeSlotText(group);
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
    g_dwmQueryThumbnailSourceSize = reinterpret_cast<DwmQueryThumbnailSourceSizeFn>(
        GetProcAddress(g_dwmApi, "DwmQueryThumbnailSourceSize"));
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
    g_dwmQueryThumbnailSourceSize = nullptr;
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

    const bool useTransparentDwmBackground = !IsFullscreenBackdropActive();
    MARGINS margins = useTransparentDwmBackground
        ? MARGINS{ -1, -1, -1, -1 } : MARGINS{ 0, 0, 0, 0 };
    typedef HRESULT (WINAPI* DwmExtendFrameIntoClientAreaFn)(HWND, const MARGINS*);
    DwmExtendFrameIntoClientAreaFn pDwmExtendFrameIntoClientArea =
        reinterpret_cast<DwmExtendFrameIntoClientAreaFn>(
            GetProcAddress(g_dwmApi, "DwmExtendFrameIntoClientArea"));
    if (pDwmExtendFrameIntoClientArea)
    {
        HRESULT frameResult = pDwmExtendFrameIntoClientArea(hwnd, &margins);
        g_dwmGlassEnabled = useTransparentDwmBackground && SUCCEEDED(frameResult);
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
                const std::wstring apiPath = GetNativeSystemPathForApi(processPath);
                if (pExtractIconExW(apiPath.c_str(), 0, &hLarge, nullptr, 1) > 0 && hLarge)
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

// Captura fullscreen estática. La imagen nítida conserva resolución nativa para
// la revelación inicial; el blur se reduce antes del efecto y se escala al dibujar.
static const IID kIidD2D1Factory1 = {
    0xbb12d362, 0xdaee, 0x4b9a, { 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f }
};
static const IID kIidDxgiDevice = {
    0x54ec77fa, 0x1377, 0x44e6, { 0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c }
};
static const GUID kGaussianBlurEffectClsid = {
    0x1feb6d69, 0x2fe6, 0x4ac9, { 0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5 }
};

template <typename T> static void ReleaseComObject(T*& object)
{
    if (object) { object->Release(); object = nullptr; }
}

typedef HRESULT (WINAPI* D3D11CreateDeviceFn)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT,
    UINT, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

static bool IsFullscreenBackdropActive()
{
    return g_blurredBackgroundReady;
}

static void FsSurfaceRelease(FsSurface& s)
{
    if (s.dc && s.oldBitmap)
        SelectObject(s.dc, s.oldBitmap);
    if (s.dc)
        DeleteDC(s.dc);
    if (s.bitmap)
        DeleteObject(s.bitmap);
    s = FsSurface();
}

static bool FsSurfaceCreate(FsSurface& s, HDC ref, int width, int height,
                            const DWORD* source)
{
    FsSurfaceRelease(s);
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    s.bitmap = CreateDIBSection(ref, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!s.bitmap || !bits)
    {
        FsSurfaceRelease(s);
        return false;
    }
    s.dc = CreateCompatibleDC(ref);
    if (!s.dc)
    {
        FsSurfaceRelease(s);
        return false;
    }
    s.oldBitmap = SelectObject(s.dc, s.bitmap);
    if (source)
        std::memcpy(bits, source, static_cast<size_t>(width) * height * sizeof(DWORD));
    s.width = width;
    s.height = height;
    return true;
}

static void ReleaseFullscreenD2DBitmaps()
{
    ReleaseComObject(g_fsSharpD2D);
    ReleaseComObject(g_fsBlurD2D);
}

static void ReleaseShadowSprite()
{
    if (g_shadowSprite.dc && g_shadowSprite.oldBitmap)
        SelectObject(g_shadowSprite.dc, g_shadowSprite.oldBitmap);
    if (g_shadowSprite.dc)
        DeleteDC(g_shadowSprite.dc);
    if (g_shadowSprite.bitmap)
        DeleteObject(g_shadowSprite.bitmap);
    g_shadowSprite = ShadowSprite();
}

static void HideFullscreenBackdrop(bool releasePixels)
{
    g_blurredBackgroundReady = false;
    g_selectorSceneOffset = POINT{};
    g_lastBackdropPresence = -1.0f;
    if (releasePixels)
    {
        FsSurfaceRelease(g_fsSharp);
        FsSurfaceRelease(g_fsBlur);
        FsSurfaceRelease(g_fsBack);
        FsSurfaceRelease(g_fsScene);
        ReleaseFullscreenD2DBitmaps();
        ReleaseShadowSprite();
        std::vector<DWORD>().swap(g_capturedDesktopPixels);
        std::vector<DWORD>().swap(g_blurredDesktopPixels);
        g_blurredDesktopBitmapInfo = BITMAPINFO();
        g_blurredMonitorRect = {};
        g_blurredInitialRect = {};
    }
}

static bool CaptureAndBlurMonitor(const RECT& monitorRect)
{
    const int width = monitorRect.right - monitorRect.left;
    const int height = monitorRect.bottom - monitorRect.top;
    if (width <= 0 || height <= 0 || width > 16384 || height > 16384 ||
        static_cast<unsigned long long>(width) * height > 20000000ULL)
    {
        Wh_Log(L"[FS] tamano de monitor no valido: %d x %d", width, height);
        return false;
    }

    HDC screenDc = GetDC(nullptr);
    HDC captureDc = screenDc ? CreateCompatibleDC(screenDc) : nullptr;
    if (!screenDc || !captureDc)
    {
        Wh_Log(L"[FS] GetDC/CreateCompatibleDC fallo (%lu)", GetLastError());
        if (captureDc) DeleteDC(captureDc);
        if (screenDc) ReleaseDC(nullptr, screenDc);
        return false;
    }
    BITMAPINFO captureInfo = {};
    captureInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    captureInfo.bmiHeader.biWidth = width;
    captureInfo.bmiHeader.biHeight = -height; // Top-down; matches D2D row order.
    captureInfo.bmiHeader.biPlanes = 1;
    captureInfo.bmiHeader.biBitCount = 32;
    captureInfo.bmiHeader.biCompression = BI_RGB;
    void* captureBits = nullptr;
    HBITMAP captureBitmap = CreateDIBSection(screenDc, &captureInfo, DIB_RGB_COLORS,
                                              &captureBits, nullptr, 0);
    HGDIOBJ oldBitmap = captureBitmap ? SelectObject(captureDc, captureBitmap) : nullptr;
    const bool captured = captureBitmap && captureBits &&
        BitBlt(captureDc, 0, 0, width, height, screenDc,
               monitorRect.left, monitorRect.top, SRCCOPY | CAPTUREBLT);
    std::vector<DWORD> input;
    if (captured)
    {
        try
        {
            input.resize(static_cast<size_t>(width) * height);
            const DWORD* pixels = static_cast<const DWORD*>(captureBits);
            for (size_t i = 0; i < input.size(); ++i)
                input[i] = pixels[i] | 0xff000000u;
        }
        catch (...)
        {
            input.clear();
        }
    }
    if (oldBitmap) SelectObject(captureDc, oldBitmap);
    if (captureBitmap) DeleteObject(captureBitmap);
    DeleteDC(captureDc);
    ReleaseDC(nullptr, screenDc);
    if (input.empty())
    {
        Wh_Log(L"[FS] BitBlt/captura fallo (%d x %d)", width, height);
        return false;
    }
    g_capturedDesktopPixels.swap(input);

    HMODULE d3d11 = LoadLibraryExW(L"d3d11.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE d2d1 = LoadLibraryExW(L"d2d1.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    D3D11CreateDeviceFn createD3D = d3d11 ? reinterpret_cast<D3D11CreateDeviceFn>(
        GetProcAddress(d3d11, "D3D11CreateDevice")) : nullptr;
    D2D1CreateFactoryFn createD2D = d2d1 ? reinterpret_cast<D2D1CreateFactoryFn>(
        GetProcAddress(d2d1, "D2D1CreateFactory")) : nullptr;

    ID3D11Device* d3dDevice = nullptr;
    ID3D11DeviceContext* d3dContext = nullptr;
    IDXGIDevice* dxgiDevice = nullptr;
    ID2D1Factory1* factory1 = nullptr;
    ID2D1Device* d2dDevice = nullptr;
    ID2D1DeviceContext* d2dContext = nullptr;
    ID2D1Bitmap1* inputBitmap = nullptr;
    ID2D1Bitmap1* outputBitmap = nullptr;
    ID2D1Effect* blurEffect = nullptr;
    ID2D1Bitmap1* cpuBitmap = nullptr;
    std::vector<DWORD> blurInput;
    int blurWidth = 0, blurHeight = 0;
    GetFullscreenBlurDimensions(width, height, blurWidth, blurHeight);
    bool success = false;

    if (!createD3D || !createD2D)
        { Wh_Log(L"[FS] CaptureAndBlurMonitor fallo en la linea %d", __LINE__); goto cleanup; }
    {
        const UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        HRESULT hr = createD3D(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                               nullptr, 0, D3D11_SDK_VERSION,
                               &d3dDevice, nullptr, &d3dContext);
        if (FAILED(hr))
        {
            ReleaseComObject(d3dContext);
            ReleaseComObject(d3dDevice);
            hr = createD3D(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                           nullptr, 0, D3D11_SDK_VERSION,
                           &d3dDevice, nullptr, &d3dContext);
        }
        if (FAILED(hr) || !d3dDevice || !d3dContext)
            { Wh_Log(L"[FS] CaptureAndBlurMonitor fallo en la linea %d", __LINE__); goto cleanup; }
    }
    if (FAILED(d3dDevice->QueryInterface(kIidDxgiDevice,
                                         reinterpret_cast<void**>(&dxgiDevice))))
        { Wh_Log(L"[FS] CaptureAndBlurMonitor fallo en la linea %d", __LINE__); goto cleanup; }
    if (FAILED(createD2D(D2D1_FACTORY_TYPE_SINGLE_THREADED, kIidD2D1Factory1,
                         nullptr, reinterpret_cast<void**>(&factory1))) || !factory1)
        { Wh_Log(L"[FS] CaptureAndBlurMonitor fallo en la linea %d", __LINE__); goto cleanup; }
    if (FAILED(factory1->CreateDevice(dxgiDevice, &d2dDevice)) || !d2dDevice ||
        FAILED(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                               &d2dContext)) || !d2dContext)
        { Wh_Log(L"[FS] CaptureAndBlurMonitor fallo en la linea %d", __LINE__); goto cleanup; }

    {
        // Downsample on CPU before uploading: the Gaussian blur processes only
        // 1/16 of the monitor pixels, greatly reducing GPU work and readback cost.
        try
        {
            blurInput.resize(static_cast<size_t>(blurWidth) * blurHeight);
            for (int y = 0; y < blurHeight; ++y)
            {
                const int sourceY = std::min(height - 1, y * kFullscreenBlurDownsample);
                const DWORD* sourceRow = g_capturedDesktopPixels.data() +
                                         static_cast<size_t>(sourceY) * width;
                DWORD* targetRow = blurInput.data() + static_cast<size_t>(y) * blurWidth;
                for (int x = 0; x < blurWidth; ++x)
                {
                    const int sourceX = std::min(width - 1, x * kFullscreenBlurDownsample);
                    targetRow[x] = sourceRow[sourceX];
                }
            }
        }
        catch (...)
        {
            blurInput.clear();
            Wh_Log(L"[FS] No se pudo reservar el buffer reducido del blur");
            goto cleanup;
        }

        const D2D1_BITMAP_PROPERTIES1 inputProps = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_NONE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
        if (FAILED(d2dContext->CreateBitmap(D2D1::SizeU(blurWidth, blurHeight),
                blurInput.data(),
                static_cast<UINT32>(blurWidth * sizeof(DWORD)), &inputProps, &inputBitmap)) ||
            !inputBitmap)
            { Wh_Log(L"[FS] CaptureAndBlurMonitor fallo en la linea %d", __LINE__); goto cleanup; }
    }
    if (FAILED(d2dContext->CreateEffect(kGaussianBlurEffectClsid, &blurEffect)) ||
        !blurEffect)
        { Wh_Log(L"[FS] CaptureAndBlurMonitor fallo en la linea %d", __LINE__); goto cleanup; }
    blurEffect->SetInput(0, inputBitmap);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
                         kFullscreenBlurStandardDeviation /
                             static_cast<float>(kFullscreenBlurDownsample));
    // Gaussian Blur's default optimization is Balanced. Leave it at that default
    // to remain compatible with SDK headers that omit the enum symbol.
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
                         D2D1_BORDER_MODE_HARD);

    {
        // Ruta solo Direct2D: el destino del blur y la copia legible por CPU
        // son bitmaps de D2D. Se evita CreateBitmapFromDxgiSurface, que fallaba
        // en este equipo.
        const D2D1_SIZE_U bitmapSize = D2D1::SizeU(blurWidth, blurHeight);
        const D2D1_PIXEL_FORMAT bitmapFormat = D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

        HRESULT hr = d2dContext->CreateBitmap(
            bitmapSize, nullptr, 0,
            D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, bitmapFormat,
                                    96.0f, 96.0f),
            &outputBitmap);
        if (FAILED(hr) || !outputBitmap)
        {
            Wh_Log(L"[FS] CreateBitmap(target) hr=0x%08X", static_cast<unsigned>(hr));
            goto cleanup;
        }

        d2dContext->SetTarget(outputBitmap);
        d2dContext->BeginDraw();
        d2dContext->Clear(D2D1::ColorF(0, 0, 0, 0));
        d2dContext->DrawImage(blurEffect);
        hr = d2dContext->EndDraw();
        d2dContext->SetTarget(nullptr);
        if (FAILED(hr))
        {
            Wh_Log(L"[FS] EndDraw hr=0x%08X", static_cast<unsigned>(hr));
            goto cleanup;
        }

        hr = d2dContext->CreateBitmap(
            bitmapSize, nullptr, 0,
            D2D1::BitmapProperties1(
                static_cast<D2D1_BITMAP_OPTIONS>(
                    static_cast<UINT>(D2D1_BITMAP_OPTIONS_CPU_READ) |
                    static_cast<UINT>(D2D1_BITMAP_OPTIONS_CANNOT_DRAW)),
                bitmapFormat, 96.0f, 96.0f),
            &cpuBitmap);
        if (FAILED(hr) || !cpuBitmap)
        {
            Wh_Log(L"[FS] CreateBitmap(cpu) hr=0x%08X", static_cast<unsigned>(hr));
            goto cleanup;
        }
        hr = cpuBitmap->CopyFromBitmap(nullptr, outputBitmap, nullptr);
        if (FAILED(hr))
        {
            Wh_Log(L"[FS] CopyFromBitmap hr=0x%08X", static_cast<unsigned>(hr));
            goto cleanup;
        }
        D2D1_MAPPED_RECT mapped = {};
        hr = cpuBitmap->Map(D2D1_MAP_OPTIONS_READ, &mapped);
        if (FAILED(hr) || !mapped.bits)
        {
            Wh_Log(L"[FS] Map hr=0x%08X", static_cast<unsigned>(hr));
            goto cleanup;
        }
        try
        {
            g_blurredDesktopPixels.resize(static_cast<size_t>(blurWidth) * blurHeight);
            for (int y = 0; y < blurHeight; ++y)
            {
                const BYTE* src = mapped.bits + static_cast<size_t>(y) * mapped.pitch;
                DWORD* dst = g_blurredDesktopPixels.data() +
                             static_cast<size_t>(y) * blurWidth;
                for (int x = 0; x < blurWidth; ++x)
                {
                    const BYTE b = static_cast<BYTE>(src[x * 4 + 0] * kFullscreenBackdropBrightness);
                    const BYTE g = static_cast<BYTE>(src[x * 4 + 1] * kFullscreenBackdropBrightness);
                    const BYTE r = static_cast<BYTE>(src[x * 4 + 2] * kFullscreenBackdropBrightness);
                    dst[x] = 0xff000000u | (static_cast<DWORD>(r) << 16) |
                             (static_cast<DWORD>(g) << 8) | b;
                }
            }
        }
        catch (...)
        {
            g_blurredDesktopPixels.clear();
        }
        cpuBitmap->Unmap();
        if (g_blurredDesktopPixels.size() == static_cast<size_t>(blurWidth) * blurHeight)
            success = true;
    }

cleanup:
    if (d2dContext) d2dContext->SetTarget(nullptr);
    ReleaseComObject(cpuBitmap);
    ReleaseComObject(blurEffect);
    ReleaseComObject(outputBitmap);
    ReleaseComObject(inputBitmap);
    ReleaseComObject(d2dContext);
    ReleaseComObject(d2dDevice);
    ReleaseComObject(factory1);
    ReleaseComObject(dxgiDevice);
    ReleaseComObject(d3dContext);
    ReleaseComObject(d3dDevice);
    if (d2d1) FreeLibrary(d2d1);
    if (d3d11) FreeLibrary(d3d11);
    if (!success)
    {
        g_capturedDesktopPixels.clear();
        g_blurredDesktopPixels.clear();
    }
    return success;
}

// Draw the captured desktop directly to the selector's HDC. This keeps the
// fullscreen background independent of Direct2D's ability to allocate a
// monitor-sized bitmap; the GDI scene renderer draws the cards over it.
static bool DrawFullscreenBackgroundGdi(HDC hdc)
{
    if (!hdc)
        return false;

    const int width = g_blurredMonitorRect.right - g_blurredMonitorRect.left;
    const int height = g_blurredMonitorRect.bottom - g_blurredMonitorRect.top;
    if (width <= 0 || height <= 0)
        return false;

    if (!g_fsSharp.dc || !g_fsBlur.dc)
    {
        int blurWidth = 0, blurHeight = 0;
        GetFullscreenBlurDimensions(width, height, blurWidth, blurHeight);
        const size_t expectedPixels = static_cast<size_t>(width) * height;
        const size_t expectedBlurPixels = static_cast<size_t>(blurWidth) * blurHeight;
        if (g_capturedDesktopPixels.size() != expectedPixels ||
            g_blurredDesktopPixels.size() != expectedBlurPixels)
            return false;
        if (!FsSurfaceCreate(g_fsSharp, hdc, width, height,
                             g_capturedDesktopPixels.data()) ||
            !FsSurfaceCreate(g_fsBlur, hdc, blurWidth, blurHeight,
                             g_blurredDesktopPixels.data()))
        {
            FsSurfaceRelease(g_fsSharp);
            FsSurfaceRelease(g_fsBlur);
            return false;
        }
    }

    if (!BitBlt(hdc, 0, 0, width, height, g_fsSharp.dc, 0, 0, SRCCOPY))
        return false;

    // La revelación del desenfoque nace del centro del monitor (no de los
    // bordes del rectángulo del selector) y crece hasta cubrir todo el monitor,
    // con el mismo progreso (g_scenePresence) que anima el resto de la escena.
    const float progress = Clamp01(g_scenePresence);
    const int centerX = width / 2;
    const int centerY = height / 2;
    int left = 0;
    int right = width;
    int top = 0;
    int bottom = height;
    if (g_animationStyle == AnimationStyle::Rct)
    {
        // RCT fullscreen reveal: begin as a horizontal line across the screen,
        // then expand vertically from the center until the blur covers the monitor.
        const int halfHeight = std::max(1, static_cast<int>(centerY * progress));
        top = std::max(0, centerY - halfHeight);
        bottom = std::min(height, centerY + halfHeight);
    }
    else
    {
        left = std::max(0, static_cast<int>(centerX - centerX * progress));
        top = std::max(0, static_cast<int>(centerY - centerY * progress));
        right = std::min(width, static_cast<int>(centerX + (width - centerX) * progress));
        bottom = std::min(height, static_cast<int>(centerY + (height - centerY) * progress));
    }
    if (right <= left || bottom <= top)
        return true;

    const float zoom = 1.0f + std::min(0.02f,
        std::max(0.0f, g_scenePresence - 1.0f) * 0.10f);
    const int drawW = static_cast<int>(width * zoom);
    const int drawH = static_cast<int>(height * zoom);

    bool success;
    if (drawW == width && drawH == height)
    {
        success = StretchBlt(hdc, left, top, right - left, bottom - top,
                             g_fsBlur.dc,
                             left * g_fsBlur.width / width,
                             top * g_fsBlur.height / height,
                             std::max(1, (right - left) * g_fsBlur.width / width),
                             std::max(1, (bottom - top) * g_fsBlur.height / height),
                             SRCCOPY) != FALSE;
    }
    else
    {
        const int clipState = SaveDC(hdc);
        if (!clipState)
            return false;
        IntersectClipRect(hdc, left, top, right, bottom);
        SetStretchBltMode(hdc, COLORONCOLOR);
        success = StretchBlt(hdc, (width - drawW) / 2, (height - drawH) / 2,
                             drawW, drawH, g_fsBlur.dc, 0, 0,
                             g_fsBlur.width, g_fsBlur.height,
                             SRCCOPY) != FALSE;
        RestoreDC(hdc, clipState);
    }
    if (success)
        g_fullscreenHdcDrawFailureLogged = false;
    return success;
}

// Direct2D: el escritorio nítido conserva resolución nativa; el bitmap de blur
// ocupa 1/16 de los píxeles. Ambos se reconstruyen desde RAM si cambia el target.
static bool EnsureFullscreenD2DBitmaps()
{
    if (g_fsSharpD2D && g_fsBlurD2D)
        return true;
    if (!g_d2dRenderTarget)
        return false;
    const int width = g_blurredMonitorRect.right - g_blurredMonitorRect.left;
    const int height = g_blurredMonitorRect.bottom - g_blurredMonitorRect.top;
    int blurWidth = 0, blurHeight = 0;
    GetFullscreenBlurDimensions(width, height, blurWidth, blurHeight);
    const size_t expectedSharp = static_cast<size_t>(width) * height;
    const size_t expectedBlur = static_cast<size_t>(blurWidth) * blurHeight;
    if (width <= 0 || height <= 0 ||
        g_capturedDesktopPixels.size() != expectedSharp ||
        g_blurredDesktopPixels.size() != expectedBlur)
        return false;

    ReleaseFullscreenD2DBitmaps();
    const D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
    HRESULT hr = g_d2dRenderTarget->CreateBitmap(
        D2D1::SizeU(width, height), g_capturedDesktopPixels.data(),
        static_cast<UINT32>(width * sizeof(DWORD)), &props, &g_fsSharpD2D);
    if (SUCCEEDED(hr) && g_fsSharpD2D)
        hr = g_d2dRenderTarget->CreateBitmap(
            D2D1::SizeU(blurWidth, blurHeight), g_blurredDesktopPixels.data(),
            static_cast<UINT32>(blurWidth * sizeof(DWORD)), &props, &g_fsBlurD2D);
    if (FAILED(hr) || !g_fsSharpD2D || !g_fsBlurD2D)
    {
        Wh_Log(L"[FS] CreateBitmap D2D hr=0x%08X", static_cast<unsigned>(hr));
        ReleaseFullscreenD2DBitmaps();
        return false;
    }
    return true;
}

// Fondo fullscreen dentro de la escena D2D. Usa g_scenePresence, el mismo progreso
// que apertura/cierre y carrusel; no crea un reloj ni una animación independiente.
// Se dibuja antes del transform de escena para mantener la continuidad espacial.
static void DrawFullscreenBackgroundD2D()
{
    if (!g_d2dRenderTarget || !g_fsSharpD2D || !g_fsBlurD2D)
        return;
    const float width = static_cast<float>(g_blurredMonitorRect.right - g_blurredMonitorRect.left);
    const float height = static_cast<float>(g_blurredMonitorRect.bottom - g_blurredMonitorRect.top);

    g_d2dRenderTarget->DrawBitmap(
        g_fsSharpD2D, D2D1::RectF(0.0f, 0.0f, width, height), 1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

    // Revelación desde el centro del monitor hasta cubrirlo.
    const float progress = Clamp01(g_scenePresence);
    const float cx = width * 0.5f;
    const float cy = height * 0.5f;
    float left = 0.0f;
    float top = 0.0f;
    float right = width;
    float bottom = height;
    if (g_animationStyle == AnimationStyle::Rct)
    {
        // RCT starts as a full-width horizontal line and reveals vertically.
        const float halfHeight = std::max(1.0f, cy * progress);
        top = cy - halfHeight;
        bottom = cy + halfHeight;
    }
    else
    {
        left = cx - cx * progress;
        top = cy - cy * progress;
        right = cx + (width - cx) * progress;
        bottom = cy + (height - cy) * progress;
    }
    if (right <= left || bottom <= top)
        return;

    const float zoom = 1.0f + std::min(0.02f,
        std::max(0.0f, g_scenePresence - 1.0f) * 0.10f);
    const float dw = width * zoom;
    const float dh = height * zoom;
    g_d2dRenderTarget->PushAxisAlignedClip(
        D2D1::RectF(left, top, right, bottom), D2D1_ANTIALIAS_MODE_ALIASED);
    g_d2dRenderTarget->DrawBitmap(
        g_fsBlurD2D,
        D2D1::RectF((width - dw) * 0.5f, (height - dh) * 0.5f,
                    (width + dw) * 0.5f, (height + dh) * 0.5f),
        1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    // Pattern Waves shares the exact fullscreen reveal clip and presence curve.
    // Its procedural field is evaluated in monitor coordinates, not selector space.
    if (IsPatternWavesActive())
    {
        RECT fullscreenPatternRect = { 0, 0,
            static_cast<LONG>(width), static_cast<LONG>(height) };
        DrawPatternWaves(fullscreenPatternRect);
    }
    g_d2dRenderTarget->PopAxisAlignedClip();
}

static void LogFullscreenHdcDrawFailure()
{
    if (g_fullscreenHdcDrawFailureLogged)
        return;
    Wh_Log(L"Fullscreen background HDC draw failed");
    g_fullscreenHdcDrawFailureLogged = true;
}

static bool PrepareFullscreenBackdrop(const RECT& monitorRect, const RECT& selectorRect)
{
    HideFullscreenBackdrop(true);
    if (g_baseBackgroundMode != BaseBackgroundMode::Fullscreen)
    {
        Wh_Log(L"[FS] modo de fondo no es Fullscreen (modo=%d)", static_cast<int>(g_baseBackgroundMode));
        return false;
    }
    if (!CaptureAndBlurMonitor(monitorRect))
    {
        Wh_Log(L"Fullscreen background capture/blur failed; falling back to DWM");
        return false;
    }

    const int width = monitorRect.right - monitorRect.left;
    const int height = monitorRect.bottom - monitorRect.top;
    g_blurredMonitorRect = monitorRect;
    g_blurredInitialRect = RECT{
        selectorRect.left - monitorRect.left,
        selectorRect.top - monitorRect.top,
        selectorRect.right - monitorRect.left,
        selectorRect.bottom - monitorRect.top
    };
    g_blurredDesktopBitmapInfo = BITMAPINFO();
    g_blurredDesktopBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    int blurWidth = 0, blurHeight = 0;
    GetFullscreenBlurDimensions(width, height, blurWidth, blurHeight);
    g_blurredDesktopBitmapInfo.bmiHeader.biWidth = blurWidth;
    g_blurredDesktopBitmapInfo.bmiHeader.biHeight = -blurHeight;
    g_blurredDesktopBitmapInfo.bmiHeader.biPlanes = 1;
    g_blurredDesktopBitmapInfo.bmiHeader.biBitCount = 32;
    g_blurredDesktopBitmapInfo.bmiHeader.biCompression = BI_RGB;
    // The pixel buffers are rendered directly into the selector HDC. Do not
    // make readiness depend on creating monitor-sized Direct2D bitmaps.
    g_blurredBackgroundReady = true;
    Wh_Log(L"Fullscreen background ready (%d x %d)", width, height);
    return true;
}

static void UpdateFullscreenBackdrop()
{
    // El fondo solo cambia cuando cambia la presencia: mientras está quieto no
    // se invalida el monitor entero (el frame loop invalida solo las cards).
    if (g_blurredBackgroundReady && g_selector && IsWindow(g_selector) &&
        g_scenePresence != g_lastBackdropPresence)
    {
        g_lastBackdropPresence = g_scenePresence;
        InvalidateRect(g_selector, nullptr, FALSE);
    }
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
    if (g_d2dFactory && g_d2dRenderTarget && g_dwriteFactory &&
        g_dwriteFormatsScale == g_uiScale.value)
    {
        if (!g_d2dBrush)
            g_d2dRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White), &g_d2dBrush);
        return g_d2dBrush != nullptr;
    }

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
            g_d2dRenderTarget = g_d2dDCRenderTarget;
            g_d2dRenderTarget->CreateSolidColorBrush(
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
        // Match GDI's DT_END_ELLIPSIS while clipping to each card's true text area.
        IDWriteInlineObject* ellipsisSign = nullptr;
        IDWriteTextFormat* trimmingFormat = g_dwriteNormalFormat
            ? g_dwriteNormalFormat : g_dwriteSelectedFormat;
        if (trimmingFormat &&
            SUCCEEDED(g_dwriteFactory->CreateEllipsisTrimmingSign(
                trimmingFormat, &ellipsisSign)) &&
            ellipsisSign)
        {
            const DWRITE_TRIMMING trimming = {
                DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
            if (g_dwriteSelectedFormat)
                g_dwriteSelectedFormat->SetTrimming(&trimming, ellipsisSign);
            if (g_dwriteNormalFormat)
                g_dwriteNormalFormat->SetTrimming(&trimming, ellipsisSign);
            ellipsisSign->Release();
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
    g_d2dRenderTarget = nullptr;
    if (g_fsHwndRenderTarget) { g_fsHwndRenderTarget->Release(); g_fsHwndRenderTarget = nullptr; }
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
    ReleaseFullscreenD2DBitmaps();
    if (g_closeAccentBrush) { g_closeAccentBrush->Release(); g_closeAccentBrush = nullptr; }
    if (g_backgroundSpotlightBrush) { g_backgroundSpotlightBrush->Release(); g_backgroundSpotlightBrush = nullptr; }
    if (g_ambientShadowBrush) { g_ambientShadowBrush->Release(); g_ambientShadowBrush = nullptr; }
    g_backgroundSpotlightBrushColor = CLR_INVALID;
    if (g_d2dBrush) { g_d2dBrush->Release(); g_d2dBrush = nullptr; }
    if (g_d2dRenderTarget == g_fsHwndRenderTarget)
    {
        if (g_fsHwndRenderTarget) { g_fsHwndRenderTarget->Release(); g_fsHwndRenderTarget = nullptr; }
    }
    else if (g_d2dRenderTarget == g_d2dDCRenderTarget)
    {
        if (g_d2dDCRenderTarget) { g_d2dDCRenderTarget->Release(); g_d2dDCRenderTarget = nullptr; }
    }
    g_d2dRenderTarget = nullptr;
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

    // Avoid rendering/registering the same window in every carousel slot.
    // Keep the center slot so the normal card geometry and selection behavior remain intact.
    if (g_singleWindowCard && count == 1 && slot != GetCarouselCenterSlot())
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
        if (ResolveGroupIndex(slot) < 0)
            continue;
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
    if (!g_d2dRenderTarget || !g_d2dBrush)
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
        g_d2dRenderTarget->FillRoundedRectangle(&shadowRect, g_d2dBrush);
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

// SOMBRAS SUAVES DE LAS CARDS
//
// Perfil de sombra: alfa(d) = fuerza * 0.5 * erfc(d / (sigma * sqrt2)), donde d es
// la distancia con signo al borde de la card. Es la caída de un borde desenfocado
// con gaussiana, sin bandas visibles. La sombra rodea toda la card y se desplaza
// hacia abajo.

static float ShadowSigmaPx() { return static_cast<float>(ScaleLayoutPx(9.0f)); }
static float ShadowOffsetPx() { return static_cast<float>(ScaleLayoutPx(7.0f)); }
static float ShadowProfile(float distance, float sigma, float strength)
{
    return strength * 0.5f *
           static_cast<float>(std::erfc(distance / (sigma * 1.41421356f)));
}

static bool EnsureAlphaBlend()
{
    if (g_pAlphaBlend)
        return true;
    if (!g_msimg32Module)
        g_msimg32Module = LoadLibraryExW(L"msimg32.dll", nullptr,
                                         LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_msimg32Module)
        return false;
    g_pAlphaBlend = reinterpret_cast<AlphaBlendProc>(
        GetProcAddress(g_msimg32Module, "AlphaBlend"));
    return g_pAlphaBlend != nullptr;
}

static bool EnsureShadowSprite(HDC ref, int radius, int margin)
{
    if (g_shadowSprite.dc && g_shadowSprite.radius == radius &&
        g_shadowSprite.margin == margin)
        return true;
    ReleaseShadowSprite();

    const int size = 2 * (margin + radius) + 2;
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = size;
    info.bmiHeader.biHeight = -size;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(ref, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits)
    {
        if (bitmap) DeleteObject(bitmap);
        return false;
    }
    HDC dc = CreateCompatibleDC(ref);
    if (!dc)
    {
        DeleteObject(bitmap);
        return false;
    }

    // La forma interior mide 2*radius+2: el centro (2 px) se estira luego, de
    // modo que el perfil de los lados rectos es exacto a cualquier tamaño.
    const float sigma = std::max(1.0f, static_cast<float>(margin) / 3.0f);
    const float center = static_cast<float>(size) * 0.5f;
    const float r = static_cast<float>(radius);
    DWORD* pixels = static_cast<DWORD*>(bits);
    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            const float qx = fabsf(static_cast<float>(x) + 0.5f - center) - 1.0f;
            const float qy = fabsf(static_cast<float>(y) + 0.5f - center) - 1.0f;
            const float ox = std::max(qx, 0.0f);
            const float oy = std::max(qy, 0.0f);
            const float sd = sqrtf(ox * ox + oy * oy) +
                             std::min(std::max(qx, qy), 0.0f) - r;
            const float a = std::max(0.0f, std::min(1.0f, ShadowProfile(sd, sigma, 1.0f)));
            // Premultiplicado: el color es negro, solo cuenta el alfa.
            pixels[y * size + x] = static_cast<DWORD>(a * 255.0f + 0.5f) << 24;
        }
    }

    g_shadowSprite.dc = dc;
    g_shadowSprite.bitmap = bitmap;
    g_shadowSprite.oldBitmap = SelectObject(dc, bitmap);
    g_shadowSprite.size = size;
    g_shadowSprite.margin = margin;
    g_shadowSprite.radius = radius;
    return true;
}

static void PaintCardShadowGDI(HDC hdc, const RECT& cardRect, int radius,
                               float strength)
{
    if (!hdc || strength <= 0.0f)
        return;
    const int margin = std::max(6, static_cast<int>(ShadowSigmaPx() * 3.0f));
    const int offsetY = static_cast<int>(ShadowOffsetPx());
    const int corner = margin + radius;

    const RECT outer = {
        cardRect.left - margin, cardRect.top + offsetY - margin,
        cardRect.right + margin, cardRect.bottom + offsetY + margin
    };
    const int w = outer.right - outer.left;
    const int h = outer.bottom - outer.top;
    if (!EnsureAlphaBlend() || w <= 2 * corner + 1 || h <= 2 * corner + 1 ||
        !EnsureShadowSprite(hdc, radius, margin))
    {
        PaintBottomShadowGDI(hdc, cardRect, radius);
        return;
    }

    const int s = g_shadowSprite.size;
    const int mid = s - 2 * corner;  // 2 px estirables
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = static_cast<BYTE>(
        std::max(0.0f, std::min(1.0f, strength)) * 255.0f + 0.5f);
    blend.AlphaFormat = AC_SRC_ALPHA;
    HDC src = g_shadowSprite.dc;
    auto blit = [&](int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh) {
        if (dw > 0 && dh > 0)
            g_pAlphaBlend(hdc, dx, dy, dw, dh, src, sx, sy, sw, sh, blend);
    };
    // Esquinas 1:1.
    blit(outer.left, outer.top, corner, corner, 0, 0, corner, corner);
    blit(outer.right - corner, outer.top, corner, corner, s - corner, 0, corner, corner);
    blit(outer.left, outer.bottom - corner, corner, corner, 0, s - corner, corner, corner);
    blit(outer.right - corner, outer.bottom - corner, corner, corner,
         s - corner, s - corner, corner, corner);
    // Lados estirados.
    blit(outer.left + corner, outer.top, w - 2 * corner, corner,
         corner, 0, mid, corner);
    blit(outer.left + corner, outer.bottom - corner, w - 2 * corner, corner,
         corner, s - corner, mid, corner);
    blit(outer.left, outer.top + corner, corner, h - 2 * corner,
         0, corner, corner, mid);
    blit(outer.right - corner, outer.top + corner, corner, h - 2 * corner,
         s - corner, corner, corner, mid);
    // Centro (queda bajo la card).
    blit(outer.left + corner, outer.top + corner, w - 2 * corner, h - 2 * corner,
         corner, corner, mid, mid);
}

// Direct2D: capas de rectángulos redondeados cuyo alfa se calcula para que el
// resultado acumulado reproduzca el perfil gaussiano. El interior de la card se
// excluye con una máscara geométrica, porque la superficie de la card es
// translúcida y la sombra no debe verse a través de ella.
static void PaintCardShadowD2D(const RECT& cardRect, float radius, float strength)
{
    if (!g_d2dRenderTarget || !g_d2dBrush || !g_d2dFactory || strength <= 0.0f)
        return;

    const float sigma = ShadowSigmaPx();
    const float offsetY = ShadowOffsetPx();
    const int kLayers = 12;
    const float spreadMin = -0.6f * sigma;
    const float spreadMax = 2.6f * sigma;
    const float step = (spreadMax - spreadMin) / static_cast<float>(kLayers - 1);

    float alphas[kLayers];
    for (int k = 0; k < kLayers; ++k)
    {
        const float here = ShadowProfile(spreadMin + step * k, sigma, strength);
        const float outer = (k + 1 < kLayers)
            ? ShadowProfile(spreadMin + step * (k + 1), sigma, strength) : 0.0f;
        alphas[k] = std::max(0.0f, 1.0f - (1.0f - here) / (1.0f - outer));
    }

    const float left = static_cast<float>(cardRect.left);
    const float top = static_cast<float>(cardRect.top);
    const float right = static_cast<float>(cardRect.right);
    const float bottom = static_cast<float>(cardRect.bottom);

    ID2D1RectangleGeometry* outerGeometry = nullptr;
    ID2D1RoundedRectangleGeometry* cardGeometry = nullptr;
    ID2D1GeometryGroup* maskGroup = nullptr;
    ID2D1Layer* layer = nullptr;
    const float reach = spreadMax + offsetY + 2.0f;
    if (SUCCEEDED(g_d2dFactory->CreateRectangleGeometry(
            D2D1::RectF(left - reach, top - reach, right + reach, bottom + reach),
            &outerGeometry)) && outerGeometry &&
        SUCCEEDED(g_d2dFactory->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(D2D1::RectF(left + 0.5f, top + 0.5f,
                                          right - 0.5f, bottom - 0.5f),
                              radius, radius), &cardGeometry)) && cardGeometry)
    {
        ID2D1Geometry* geometries[2] = { outerGeometry, cardGeometry };
        if (SUCCEEDED(g_d2dFactory->CreateGeometryGroup(
                D2D1_FILL_MODE_ALTERNATE, geometries, 2, &maskGroup)) && maskGroup &&
            SUCCEEDED(g_d2dRenderTarget->CreateLayer(nullptr, &layer)) && layer)
        {
            D2D1_LAYER_PARAMETERS parameters = D2D1::LayerParameters(
                D2D1::InfiniteRect(), maskGroup,
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::Matrix3x2F::Identity(),
                1.0f, nullptr, D2D1_LAYER_OPTIONS_NONE);
            g_d2dRenderTarget->PushLayer(&parameters, layer);
        }
        else
        {
            if (maskGroup) { maskGroup->Release(); maskGroup = nullptr; }
            if (layer) { layer->Release(); layer = nullptr; }
        }
    }

    // De fuera hacia dentro: cada capa se compone "over" con el alfa calculado.
    for (int k = kLayers - 1; k >= 0; --k)
    {
        if (alphas[k] < 0.002f)
            continue;
        const float spread = spreadMin + step * k;
        const float rr = std::max(0.0f, radius + spread);
        g_d2dBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, alphas[k]));
        D2D1_ROUNDED_RECT shape = D2D1::RoundedRect(
            D2D1::RectF(left - spread, top + offsetY - spread,
                        right + spread, bottom + offsetY + spread), rr, rr);
        g_d2dRenderTarget->FillRoundedRectangle(&shape, g_d2dBrush);
    }

    if (layer)
        g_d2dRenderTarget->PopLayer();
    if (layer) layer->Release();
    if (maskGroup) maskGroup->Release();
    if (cardGeometry) cardGeometry->Release();
    if (outerGeometry) outerGeometry->Release();
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

    DWM_THUMBNAIL_PROPERTIES properties = {};
    properties.dwFlags = kDwmTnpRectDestination | kDwmTnpOpacity |
                         kDwmTnpVisible;
    RECT destination = g_selectionExpansionActive
        ? area : ContainedRect(area, sourceWidth, sourceHeight);
    RECT viewport = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
    if ((g_selectionExpansionActive || IsFullscreenBackdropActive()) &&
        g_selector && IsWindow(g_selector))
        GetClientRect(g_selector, &viewport);
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
                // Use the dimensions DWM actually exposes for its thumbnail,
                // rather than GetClientRect/GetWindowRect which can differ by
                // invisible resize borders and cause a few pixels of offset.
                if (g_dwmQueryThumbnailSourceSize)
                {
                    SIZE thumbnailSize = {};
                    if (SUCCEEDED(g_dwmQueryThumbnailSourceSize(
                            thumbnail, &thumbnailSize)) &&
                        thumbnailSize.cx > 0 && thumbnailSize.cy > 0)
                    {
                        g_thumbnailSlots[slot].sourceWidth = thumbnailSize.cx;
                        g_thumbnailSlots[slot].sourceHeight = thumbnailSize.cy;
                    }
                }
            }
        }

        RECT area = TransformSceneRect(GetSlotPreviewRect(slot));
        if (g_selectionExpansionActive)
        {
            if (slot == GetCarouselCenterSlot())
                area = g_selectionExpansionRect;
            else
                area = RECT{ 0, 0, 0, 0 };
        }
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
    g_cardSnapScale = 1.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitTarget = nullptr;
}

static void StartSelectorClose(HWND target)
{
    if (!g_selector || !IsWindow(g_selector) ||
        g_selectorAnimation == SelectorAnimationState::Closing)
        return;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
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
        UpdateFullscreenBackdrop();
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
    const bool cinematic = g_animationStyle == AnimationStyle::Cinematic;
    presence = (bouncy || cinematic) ? std::max(0.0f, presence) : Clamp01(presence);
    g_scenePresence = presence;
    UpdateFullscreenBackdrop();
    const float startScale = bouncy ? 0.88f : (cinematic ? 0.86f : 0.92f);
    const float scale = startScale + (1.0f - startScale) * presence;
    g_sceneScaleX = scale;
    g_sceneScaleY = scale;
    g_sceneOpacity = cinematic
        ? EaseSmooth(std::min(1.0f, presence) / 0.38f)
        : EaseSmooth(std::min(1.0f, presence) / 0.45f);
    g_carouselSpread = cinematic
        ? 0.93f + 0.07f * presence
        : 0.96f + 0.04f * presence;
}

// Resorte subamortiguado existente para Bouncy: se conserva su modelo físico
// (respuesta de segundo orden), con amortiguación ligeramente menor y mayor
// velocidad inicial. La normalización al valor en t=1 evita un salto de valor
// al terminar; el overshoot de la presencia queda alrededor del 20 %.
static float SpringResponseRaw(float t)
{
    const float zeta = 0.48f, omega = 32.0f, v0 = 14.0f, span = 0.30f;
    const float wd = omega * sqrtf(1.0f - zeta * zeta);
    const float tau = std::max(0.0f, t) * span;
    const float e = expf(-zeta * omega * tau) *
                    (cosf(wd * tau) + ((zeta * omega - v0) / wd) * sinf(wd * tau));
    return 1.0f - e;
}

static float SpringOvershoot(float t)
{
    if (t <= 0.0f)
        return 0.0f;
    if (t >= 1.0f)
        return 1.0f;
    const float endpoint = SpringResponseRaw(1.0f);
    return endpoint > 0.0f ? SpringResponseRaw(t) / endpoint : t;
}

static float OpenDurationMs()
{
    // Fullscreen's backdrop reveal is intentionally much snappier than the
    // regular selector. Keep Bouncy's existing timing as requested.
    if (g_baseBackgroundMode == BaseBackgroundMode::Fullscreen)
    {
        switch (g_animationStyle)
        {
        case AnimationStyle::Bouncy: return 380.0f;
        case AnimationStyle::Rct: return 120.0f;
        case AnimationStyle::Cinematic: return 180.0f;
        default: return 120.0f;
        }
    }

    switch (g_animationStyle)
    {
    case AnimationStyle::Bouncy: return 380.0f;
    case AnimationStyle::Rct: return 320.0f;
    case AnimationStyle::Cinematic: return 260.0f;
    default: return 200.0f;
    }
}

static float CloseDurationMs()
{
    if (g_baseBackgroundMode == BaseBackgroundMode::Fullscreen)
    {
        switch (g_animationStyle)
        {
        case AnimationStyle::Bouncy: return 150.0f;
        case AnimationStyle::Rct: return 120.0f;
        case AnimationStyle::Cinematic: return 180.0f;
        default: return 120.0f;
        }
    }
    if (g_animationStyle == AnimationStyle::Rct) return 320.0f;
    if (g_animationStyle == AnimationStyle::Cinematic) return 220.0f;
    return 150.0f;
}

static float OpenPresenceAt(float t)
{
    switch (g_animationStyle)
    {
    case AnimationStyle::Bouncy: return SpringOvershoot(t);
    case AnimationStyle::Rct: return t;
    case AnimationStyle::Cinematic: return EaseOutBack(t, 1.15f);
    default: return EaseOutCubic(t);
    }
}

// Fracción de presencia que queda al cerrar (1 -> 0).
static float CloseRemainingAt(float t)
{
    if (g_animationStyle == AnimationStyle::Rct)
        return 1.0f - t;
    if (g_animationStyle == AnimationStyle::Linear)
        return 1.0f - EaseInCubic(t);
    if (g_animationStyle == AnimationStyle::Cinematic)
        return 1.0f - EaseInCubic(t);
    return 1.0f - EaseOutCubic(t);
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

static bool IsSelectionExpansionActive()
{
    return g_selectionExpansionActive &&
           g_selectorAnimation == SelectorAnimationState::SelectionExpansion;
}
static RECT LerpRect(const RECT& a, const RECT& b, float t)
{
    RECT result = {};
    result.left = static_cast<LONG>(roundf(a.left + (b.left - a.left) * t));
    result.top = static_cast<LONG>(roundf(a.top + (b.top - a.top) * t));
    result.right = static_cast<LONG>(roundf(a.right + (b.right - a.right) * t));
    result.bottom = static_cast<LONG>(roundf(a.bottom + (b.bottom - a.bottom) * t));
    return result;
}

// Duración base fija de la expansión hasta llenar la pantalla: 120 ms.
// AnimationDuration conserva el ajuste global de velocidad del usuario.
static float CalculateSelectionExpansionDurationMs(
    const RECT& source, const RECT& destination, const RECT& work)
{
    (void)source;
    (void)destination;
    (void)work;
    return 120.0f;
}

static void StartSelectionExpansion(HWND target, bool targetWasMinimized)
{
    if (!target || !IsWindow(target) || !g_selector ||
        !IsWindow(g_selector) || g_selectionExpansionActive)
    {
        // El llamador conserva la finalización inmediata como fallback.
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    HMONITOR monitor = MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo))
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    RECT selectorRect = {};
    if (!GetWindowRect(g_selector, &selectorRect))
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    // El rectángulo de las tarjetas está en coordenadas cliente. No asumir
    // que coincide con el borde externo del HWND: esa diferencia puede ser de
    // unos píxeles en DPI por monitor o cuando DWM aplica sus bordes.
    POINT selectorClientOrigin = { 0, 0 };
    if (!ClientToScreen(g_selector, &selectorClientOrigin))
    {
        selectorClientOrigin.x = selectorRect.left;
        selectorClientOrigin.y = selectorRect.top;
    }
    const int selectedSlot = GetCarouselCenterSlot();
    const RECT preview = TransformSceneRect(GetSlotPreviewRect(selectedSlot));
    RECT targetWindowRect = {};
    if (!GetWindowRect(target, &targetWindowRect))
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    // GetWindowRect puede incluir los bordes invisibles de redimensionado.
    // DWMWA_EXTENDED_FRAME_BOUNDS da los límites físicos que realmente se ven,
    // evitando que el thumbnail termine desplazado respecto a la ventana.
    if (!IsIconic(target) && LoadDwmFunctions() && g_dwmGetWindowAttribute)
    {
        RECT visibleFrame = {};
        if (SUCCEEDED(g_dwmGetWindowAttribute(
                target, kDwmWaExtendedFrameBounds,
                &visibleFrame, sizeof(visibleFrame))) &&
            visibleFrame.left < visibleFrame.right &&
            visibleFrame.top < visibleFrame.bottom)
        {
            targetWindowRect = visibleFrame;
        }
    }
    if (targetWasMinimized && IsIconic(target))
    {
        // La restauración ocurre en paralelo a la animación: no esperamos a que
        // el hilo de la aplicación atienda ShowWindowAsync. GetWindowRect puede
        // devolver el rectángulo del icono mientras tanto, así que usamos la
        // posición normal guardada por Windows como destino temporal.
        WINDOWPLACEMENT placement = {};
        placement.length = sizeof(placement);
        bool haveNormalRect = false;
        if (GetWindowPlacement(target, &placement))
        {
            RECT normalRect = placement.rcNormalPosition;
            if ((GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) == 0)
            {
                // rcNormalPosition usa coordenadas de workspace. Trasladar el
                // origen de workspace del monitor principal a coordenadas de pantalla.
                POINT origin = { 0, 0 };
                HMONITOR primary = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
                MONITORINFO primaryInfo = {};
                primaryInfo.cbSize = sizeof(primaryInfo);
                if (primary && GetMonitorInfoW(primary, &primaryInfo))
                {
                    const LONG offsetX = primaryInfo.rcWork.left - primaryInfo.rcMonitor.left;
                    const LONG offsetY = primaryInfo.rcWork.top - primaryInfo.rcMonitor.top;
                    OffsetRect(&normalRect, offsetX, offsetY);
                }
            }

            if (placement.flags & WPF_RESTORETOMAXIMIZED)
            {
                targetWindowRect = monitorInfo.rcWork;
                haveNormalRect = true;
            }
            else if (normalRect.left < normalRect.right &&
                     normalRect.top < normalRect.bottom)
            {
                targetWindowRect = normalRect;
                haveNormalRect = true;
            }
        }
        if (!haveNormalRect)
            targetWindowRect = monitorInfo.rcWork;

        HMONITOR restoreMonitor = MonitorFromRect(
            &targetWindowRect, MONITOR_DEFAULTTONEAREST);
        MONITORINFO restoreMonitorInfo = {};
        restoreMonitorInfo.cbSize = sizeof(restoreMonitorInfo);
        if (restoreMonitor && GetMonitorInfoW(restoreMonitor, &restoreMonitorInfo))
        {
            monitorInfo = restoreMonitorInfo;
            if (haveNormalRect && (placement.flags & WPF_RESTORETOMAXIMIZED))
                targetWindowRect = monitorInfo.rcWork;
        }
    }
    RECT expansionSourceRect = preview;
    if (selectedSlot >= 0 && selectedSlot < kMaxCarouselSlots)
    {
        const ThumbnailSlot& previewThumbnail = g_thumbnailSlots[selectedSlot];
        if (previewThumbnail.sourceWidth > 0 && previewThumbnail.sourceHeight > 0)
        {
            // Reproduce exactamente el letterboxing usado al pintar la miniatura
            // dentro de la tarjeta, para que el primer frame no salte lateralmente.
            expansionSourceRect = ContainedRect(
                preview, previewThumbnail.sourceWidth, previewThumbnail.sourceHeight);
        }
    }
    RECT work = monitorInfo.rcWork;
    const int workWidth = work.right - work.left;
    const int workHeight = work.bottom - work.top;
    if (workWidth <= 0 || workHeight <= 0 ||
        expansionSourceRect.right <= expansionSourceRect.left ||
        expansionSourceRect.bottom <= expansionSourceRect.top)
    {
        g_pendingActivationTarget = target;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    g_selectionExpansionActive = true;
    g_selectionExpansionFinishPending = false;
    g_selectionActivationRequested = false;
    g_selectionActivationRequestTick = 0;
    g_selectionExpansionTarget = target;
    g_selectionExpansionWorkRect = work;
    g_selectionExpansionSceneOffset = POINT{
        selectorClientOrigin.x - work.left, selectorClientOrigin.y - work.top };
    g_selectionExpansionStartRect = RECT{
        selectorClientOrigin.x + expansionSourceRect.left - work.left,
        selectorClientOrigin.y + expansionSourceRect.top - work.top,
        selectorClientOrigin.x + expansionSourceRect.right - work.left,
        selectorClientOrigin.y + expansionSourceRect.bottom - work.top };
    g_selectionExpansionDestinationRect = RECT{
        targetWindowRect.left - work.left,
        targetWindowRect.top - work.top,
        targetWindowRect.right - work.left,
        targetWindowRect.bottom - work.top };
    g_selectionExpansionRect = g_selectionExpansionStartRect;
    g_selectionExpansionProgress = 0.0f;
    g_pendingActivationTarget = target;
    InterlockedExchange(&g_selectionTransitionActive, 1);
    g_uiTabDown = false;
    g_tabRepeatStarted = false;
    KillTimer(g_selector, kTabRepeatTimerId);
    // Pattern Waves and the cursor spotlight belong to the task-switcher
    // backdrop, which is removed for the selected-window expansion.
    const bool wasFullscreenBackdrop = IsFullscreenBackdropActive();
    HideFullscreenBackdrop(true);
    if (wasFullscreenBackdrop)
    {
        // Blur mode disabled DWM glass on the selector itself. Restore it before
        // clearing the expanded HWND so the selected thumbnail reveals desktop.
        ApplySelectorVisuals(g_selector);
    }
    KillTimer(g_selector, kPatternTimerId);
    g_backgroundSpotlightActive = false;
    UiSetInteractive(false);
    g_selectorAnimation = SelectorAnimationState::SelectionExpansion;
    // El HWND pasa a cubrir el monitor sin activar ni cambiar el orden Z.
    SetWindowPos(g_selector, HWND_TOPMOST, work.left, work.top,
                 workWidth, workHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    // Recalibrar contra el cliente real después del resize. Así el destino
    // ocupa exactamente todos los píxeles que el renderer puede dibujar, sin
    // desplazamiento lateral por marcos, DPI o coordenadas de monitor.
    RECT clientAfterResize = {};
    POINT clientAfterOrigin = { 0, 0 };
    if (GetClientRect(g_selector, &clientAfterResize) &&
        ClientToScreen(g_selector, &clientAfterOrigin))
    {
        g_selectionExpansionWorkRect = RECT{
            0, 0, clientAfterResize.right, clientAfterResize.bottom };
        g_selectionExpansionSceneOffset = POINT{
            selectorClientOrigin.x - clientAfterOrigin.x,
            selectorClientOrigin.y - clientAfterOrigin.y };
        g_selectionExpansionStartRect = RECT{
            selectorClientOrigin.x + expansionSourceRect.left - clientAfterOrigin.x,
            selectorClientOrigin.y + expansionSourceRect.top - clientAfterOrigin.y,
            selectorClientOrigin.x + expansionSourceRect.right - clientAfterOrigin.x,
            selectorClientOrigin.y + expansionSourceRect.bottom - clientAfterOrigin.y };
        g_selectionExpansionDestinationRect = RECT{
            targetWindowRect.left - clientAfterOrigin.x,
            targetWindowRect.top - clientAfterOrigin.y,
            targetWindowRect.right - clientAfterOrigin.x,
            targetWindowRect.bottom - clientAfterOrigin.y };
        g_selectionExpansionRect = g_selectionExpansionStartRect;
    }
    g_selectionExpansionDurationMs = CalculateSelectionExpansionDurationMs(
        g_selectionExpansionStartRect, g_selectionExpansionDestinationRect,
        g_selectionExpansionWorkRect);
    g_selectionExpansionStart = FrameClockNow();
    UpdateSelectorSurfaceTransform();
    UpdateThumbnailSlots();
    RequestFrame();
}
static void UpdateSelectionExpansion()
{
    if (!IsSelectionExpansionActive())
        return;
    if (!g_selectionExpansionFinishPending)
    {
        const float raw = ElapsedFraction(g_selectionExpansionStart,
                                           g_selectionExpansionDurationMs);
        // Misma estrategia de desplazamiento agresivo usada por el renderer:
        // respuesta inmediata, avance fuerte al inicio y llegada exacta al
        // destino sin overshoot ni rebote adicional.
        const float eased = EaseOutExp(raw, 7.0f);
        g_selectionExpansionProgress = eased;
        g_selectionExpansionRect = LerpRect(g_selectionExpansionStartRect,
                                             g_selectionExpansionDestinationRect,
                                             eased);
        if (raw >= 1.0f)
            g_selectionExpansionFinishPending = true;
    }
    else
    {
        HWND target = g_selectionExpansionTarget;
        if (!g_selectionActivationRequested)
        {
            // Pide activar la ventana mientras el selector sigue cubriendo la
            // pantalla. La expansión final permanece dibujada durante la espera.
            g_selectionActivationRequested = true;
            g_selectionActivationRequestTick = GetTickCount64();
            ActivateWindow(target);
        }

        const ULONGLONG activationWaitMs =
            GetTickCount64() - g_selectionActivationRequestTick;
        const bool targetReady = target && IsWindow(target) &&
            !IsIconic(target) && GetForegroundWindow() == target;
        // Deja al menos un ciclo de composición para que DWM actualice la
        // ventana destino detrás de la cubierta antes de retirarla.
        const bool targetFrameSettled = targetReady && activationWaitMs >= 20;
        const bool activationTimedOut =
            activationWaitMs >= kSelectionActivationWaitMaxMs;
        if (!targetFrameSettled && !activationTimedOut)
        {
            UpdateSelectorSurfaceTransform();
            UpdateSelectorControls();
            return;
        }

        // El destino ya está foreground (o agotó el tiempo máximo de espera):
        // retirar ahora la cubierta expandida evita enseñar un hueco intermedio.
        g_selectionExpansionActive = false;
        g_selectionActivationRequested = false;
        g_selectionActivationRequestTick = 0;
        InterlockedExchange(&g_selectionTransitionActive, 0);
        g_selectionExpansionTarget = nullptr;
        g_pendingActivationTarget = nullptr;
        g_selectorAnimation = SelectorAnimationState::None;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        return;
    }
    UpdateSelectorSurfaceTransform();
    UpdateSelectorControls();
}
static void UpdateSelectorMotion()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    if (g_selectorAnimation == SelectorAnimationState::Open && !g_animActive)
        return;
    if (g_selectorAnimation == SelectorAnimationState::SelectionExpansion)
    {
        UpdateSelectionExpansion();
        return;
    }
    if (g_selectorAnimation == SelectorAnimationState::CardExit)
    {
        g_cardExitProgress = ElapsedFraction(g_cardExitStart, AnimationDuration(360.0f));
        if (g_cardExitProgress >= 1.0f)
        {
            HWND target = g_cardExitTarget;
            g_cardExitActive = false;
            g_cardExitProgress = 0.0f;
            g_cardExitSlot = -1;
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
                            CardNavigationDuration()) >= 1.0f)
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
    if (!g_dwmGetCompositionTimingInfo)
        return false;
    DWM_TIMING_INFO info = {};
    info.cbSize = sizeof(info);
    // Windows 8.1 and newer require a null HWND for this query.
    if (FAILED(g_dwmGetCompositionTimingInfo(nullptr, &info)))
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

// Con el fondo fullscreen quieto (presencia estable, sin expansión de
// selección), un frame solo cambia la zona de las cards y sus sombras: se
// invalida únicamente esa zona en vez del monitor completo.
static void InvalidateSelectorForFrame()
{
    if (IsFullscreenBackdropActive() && g_fsSharpD2D && g_fsBlurD2D &&
        !g_selectionExpansionActive &&
        g_scenePresence >= 1.0f && g_scenePresence <= 1.0001f)
    {
        RECT client = {};
        GetClientRect(g_selector, &client);
        const int m = ScaleLayoutPx(64.0f);
        RECT dirty = {
            g_selectorSceneOffset.x - m,
            g_selectorSceneOffset.y - m,
            g_selectorSceneOffset.x + g_runtimeSelectorWidth + m,
            g_selectorSceneOffset.y + g_runtimeSelectorHeight + m
        };
        if (IntersectRect(&dirty, &dirty, &client))
        {
            InvalidateRect(g_selector, &dirty, FALSE);
            return;
        }
    }
    InvalidateRect(g_selector, nullptr, FALSE);
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
           g_selectorAnimation == SelectorAnimationState::SelectionExpansion ||
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
        InvalidateSelectorForFrame();
        UpdateWindow(g_selector);
    }

    if (SceneIsAnimating() || g_frameDirty)
        ArmFramePump();
}

static void EnsureIconBitmap(int groupIndex)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()))
        return;
    if (!g_d2dRenderTarget)
        return;
    if (g_groups[groupIndex].icon && !g_groups[groupIndex].iconBitmap)
        g_groups[groupIndex].iconBitmap = CreateD2DBitmapFromHIcon(
            g_d2dRenderTarget, g_groups[groupIndex].icon);
}

static ID2D1LinearGradientBrush* EnsureSurfaceGradientBrush(int groupIndex,
                                                            bool selected)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()) ||
        !g_d2dRenderTarget)
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
    HRESULT hr = g_d2dRenderTarget->CreateGradientStopCollection(
        stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.startPoint = D2D1::Point2F(0.0f, 0.5f);
    properties.endPoint = D2D1::Point2F(1.0f, 0.5f);
    hr = g_d2dRenderTarget->CreateLinearGradientBrush(
        properties, stopCollection, &brush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? brush : nullptr;
}

static ID2D1RadialGradientBrush* EnsureCloseAccentBrush()
{
    if (g_closeAccentBrush || !g_d2dRenderTarget)
        return g_closeAccentBrush;

    D2D1_GRADIENT_STOP stops[3] = {};
    stops[0].position = 0.0f;
    stops[0].color = D2D1::ColorF(0.48f, 0.055f, 0.065f, 0.42f);
    stops[1].position = 0.38f;
    stops[1].color = D2D1::ColorF(0.30f, 0.030f, 0.040f, 0.20f);
    stops[2].position = 1.0f;
    stops[2].color = D2D1::ColorF(0.18f, 0.015f, 0.020f, 0.0f);

    ID2D1GradientStopCollection* stopCollection = nullptr;
    HRESULT hr = g_d2dRenderTarget->CreateGradientStopCollection(
        stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.center = D2D1::Point2F(0.5f, 0.5f);
    properties.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    properties.radiusX = 1.0f;
    properties.radiusY = 1.0f;
    hr = g_d2dRenderTarget->CreateRadialGradientBrush(
        properties, stopCollection, &g_closeAccentBrush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? g_closeAccentBrush : nullptr;
}

static void PaintSlotHeaderD2D(int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    const std::wstring& text = MakeSlotDisplayText(group);

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
        g_d2dRenderTarget->DrawBitmap(
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
        g_d2dRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
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
        g_d2dRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
                                      g_dwriteCounterFormat, &textRectD2D, g_d2dBrush);
    }
}

static void PaintSlotHeaderGDI(HDC hdc, int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    const std::wstring& text = MakeSlotDisplayText(group);

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
    if (!g_d2dRenderTarget || !g_d2dBrush || groupIndex < 0 ||
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
    g_d2dRenderTarget->DrawRoundedRectangle(&outer, g_d2dBrush,
                                               static_cast<float>(ScaleLayoutPx(2.0f)));

    D2D1_ROUNDED_RECT inner = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(cardRect.left) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.top) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.right) + ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.bottom) + ScaleLayoutPx(2.0f)),
        radius + ScaleLayoutPx(2.0f), radius + ScaleLayoutPx(2.0f));
    g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, 0.18f));
    g_d2dRenderTarget->DrawRoundedRectangle(&inner, g_d2dBrush,
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

static int PatternFieldRenderWidth()
{
    return g_patternFieldRenderWidth > 0 ? g_patternFieldRenderWidth : g_runtimeSelectorWidth;
}

static int PatternFieldRenderHeight()
{
    return g_patternFieldRenderHeight > 0 ? g_patternFieldRenderHeight : g_runtimeSelectorHeight;
}

static float PatternBaseHeight(float pixelX, float pixelY, float time)
{
    const int renderWidth = PatternFieldRenderWidth();
    const int renderHeight = PatternFieldRenderHeight();
    if (renderWidth <= 0 || renderHeight <= 0)
        return 0.0f;
    // X is dominant, producing vertical crests. Y and time bend them organically.
    float nx = pixelX / static_cast<float>(renderWidth);
    float ny = pixelY / static_cast<float>(renderHeight);
    // Reduce all procedural phase motion by 60% while preserving the Speed control.
    const float phaseTime = time * 0.40f;
    const float speed = 0.55f + g_patternSpeed * 1.35f;
    float organic = 0.20f * sinf(ny * 7.0f + phaseTime * 0.31f +
                                  sinf(nx * 4.0f + phaseTime * 0.17f));
    float ridges = sinf(nx * 13.5f + phaseTime * speed + organic);
    float fine = 0.28f * sinf(nx * 27.0f - phaseTime * speed * 0.55f + ny * 2.5f);
    float height = (ridges + fine) / 1.28f;
    // Slightly larger field amplitude gives the influence more spatial body;
    // visual brightness remains controlled in the dot stage below.
    return height * std::max(0.0f, std::min(2.0f, g_patternWaveStrength)) * 0.68f;
}

static void UpdatePatternRenderCache(float time)
{
    const float rippleScale = 0.72f + g_patternWaveStrength * 0.38f;
    const float width = static_cast<float>(std::max(1, PatternFieldRenderWidth()));
    const float height = static_cast<float>(std::max(1, PatternFieldRenderHeight()));
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
    const int renderWidth = PatternFieldRenderWidth();
    const int renderHeight = PatternFieldRenderHeight();
    if (renderWidth <= 0 || renderHeight <= 0)
        return 0.0f;
    float fx = WrapPatternCoordinate(pixelX,
        static_cast<float>(renderWidth), kPatternFieldWidth);
    float fy = WrapPatternCoordinate(pixelY,
        static_cast<float>(renderHeight), kPatternFieldHeight);
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
    if (!IsPatternWavesActive() || !g_d2dRenderTarget || !g_d2dBrush)
        return;
    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0)
        return;

    const int previousFieldWidth = g_patternFieldRenderWidth;
    const int previousFieldHeight = g_patternFieldRenderHeight;
    g_patternFieldRenderWidth = width;
    g_patternFieldRenderHeight = height;
    const double drawStartMs = QpcNowMs();
    UpdatePatternRenderCache(static_cast<float>(drawStartMs / 1000.0));
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
    g_d2dRenderTarget->FillRectangle(&background, g_d2dBrush);

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

    // QPC gives sub-millisecond draw-cost samples; hysteresis prevents isolated
    // slow frames from changing the background quality and causing visual jumps.
    // This only adjusts the background's own density, never the UI layout.
    const double drawMs = QpcNowMs() - drawStartMs;
    const ULONGLONG now = GetTickCount64();
    if (drawMs >= 12.0)
    {
        ++g_patternSlowSamples;
        g_patternFastSamples = 0;
    }
    else if (drawMs <= 4.0)
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
    g_patternFieldRenderWidth = previousFieldWidth;
    g_patternFieldRenderHeight = previousFieldHeight;
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
    if (!g_d2dRenderTarget)
        return nullptr;

    D2D1_GRADIENT_STOP stops[4] = {};
    stops[0].position = 0.00f; stops[0].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.24f);
    stops[1].position = 0.45f; stops[1].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.14f);
    stops[2].position = 0.78f; stops[2].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.04f);
    stops[3].position = 1.00f; stops[3].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);

    ID2D1GradientStopCollection* collection = nullptr;
    if (FAILED(g_d2dRenderTarget->CreateGradientStopCollection(
            stops, 4, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection)) ||
        !collection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props = {};
    props.center = D2D1::Point2F(0.0f, 0.0f);
    props.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    props.radiusX = 1.0f;
    props.radiusY = 1.0f;
    HRESULT hr = g_d2dRenderTarget->CreateRadialGradientBrush(
        &props, nullptr, collection, &g_ambientShadowBrush);
    collection->Release();
    return SUCCEEDED(hr) ? g_ambientShadowBrush : nullptr;
}

static ID2D1Layer* AcquireD2DLayer(ID2D1Layer*& slot)
{
    if (!slot && g_d2dRenderTarget)
        g_d2dRenderTarget->CreateLayer(nullptr, &slot);
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

static void PaintSelectorSceneImpl(HWND hwnd, HDC hdc);

static void ReleaseD2DResourcesForTargetSwitch()
{
    // D2D bitmaps, brushes and layers belong to the render target that created
    // them. Preserve HICON handles, but discard all target-bound caches.
    for (auto& group : g_groups)
    {
        if (group.iconBitmap) { group.iconBitmap->Release(); group.iconBitmap = nullptr; }
        if (group.surfaceBrushNormal) { group.surfaceBrushNormal->Release(); group.surfaceBrushNormal = nullptr; }
        if (group.surfaceBrushSelected) { group.surfaceBrushSelected->Release(); group.surfaceBrushSelected = nullptr; }
    }
    ReleaseCachedD2DObjects();
    ReleaseFullscreenD2DBitmaps();
    if (g_closeAccentBrush) { g_closeAccentBrush->Release(); g_closeAccentBrush = nullptr; }
    if (g_backgroundSpotlightBrush) { g_backgroundSpotlightBrush->Release(); g_backgroundSpotlightBrush = nullptr; }
    if (g_ambientShadowBrush) { g_ambientShadowBrush->Release(); g_ambientShadowBrush = nullptr; }
    g_backgroundSpotlightBrushColor = CLR_INVALID;
    if (g_d2dBrush) { g_d2dBrush->Release(); g_d2dBrush = nullptr; }
}

static bool SetActiveD2DTarget(ID2D1RenderTarget* target)
{
    if (!target)
        return false;
    if (g_d2dRenderTarget != target)
    {
        ReleaseD2DResourcesForTargetSwitch();
        g_d2dRenderTarget = target;
    }
    return true;
}

static bool EnsureFullscreenGpuTarget(HWND hwnd)
{
    if (!g_d2dFactory || !hwnd || !IsWindow(hwnd))
        return false;
    RECT rc = {};
    if (!GetClientRect(hwnd, &rc))
        return false;
    const UINT width = static_cast<UINT>(std::max<LONG>(1, rc.right - rc.left));
    const UINT height = static_cast<UINT>(std::max<LONG>(1, rc.bottom - rc.top));
    if (!g_fsHwndRenderTarget)
    {
        const D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_HARDWARE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_IGNORE),
            96.0f, 96.0f, D2D1_RENDER_TARGET_USAGE_NONE,
            D2D1_FEATURE_LEVEL_DEFAULT);
        const D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps =
            D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(width, height));
        const HRESULT hr = g_d2dFactory->CreateHwndRenderTarget(
            props, hwndProps, &g_fsHwndRenderTarget);
        if (FAILED(hr) || !g_fsHwndRenderTarget)
        {
            Wh_Log(L"[FS-GPU] No se pudo crear el target D2D por hardware: 0x%08X",
                   static_cast<unsigned>(hr));
            if (g_fsHwndRenderTarget) { g_fsHwndRenderTarget->Release(); g_fsHwndRenderTarget = nullptr; }
            return false; // No fallback a GDI: Fullscreen debe ser acelerado por hardware.
        }
    }
    else
    {
        const D2D1_SIZE_U current = g_fsHwndRenderTarget->GetPixelSize();
        if (current.width != width || current.height != height)
        {
            const HRESULT hr = g_fsHwndRenderTarget->Resize(D2D1::SizeU(width, height));
            if (FAILED(hr))
            {
                Wh_Log(L"[FS-GPU] Resize del target fallo: 0x%08X", static_cast<unsigned>(hr));
                if (g_d2dRenderTarget == g_fsHwndRenderTarget)
                    g_d2dRenderTarget = nullptr;
                g_fsHwndRenderTarget->Release();
                g_fsHwndRenderTarget = nullptr;
                return false;
            }
        }
    }
    return SetActiveD2DTarget(g_fsHwndRenderTarget);
}

// En Fullscreen, Direct2D dibuja directamente en un HWND render target por GPU;
// no se hace presentacion final por GDI/BitBlt.
static void PaintSelectorScene(HWND hwnd, HDC hdc)
{
    if (IsFullscreenBackdropActive())
    {
        // Fullscreen is rendered directly to a hardware-only Direct2D HWND target.
        // This route never binds a GDI HDC and never presents with BitBlt.
        if (!LoadD2DAndDWrite() || !EnsureFullscreenGpuTarget(hwnd) ||
            !LoadD2DAndDWrite() || !g_d2dRenderTarget || !g_d2dBrush ||
            !EnsureFullscreenD2DBitmaps())
        {
            Wh_Log(L"[FS-GPU] Frame omitido: no hay un target de hardware listo.");
            return;
        }
        PaintSelectorSceneImpl(hwnd, hdc);
        return;
    }

    if (g_d2dDCRenderTarget)
        SetActiveD2DTarget(g_d2dDCRenderTarget);
    PaintSelectorSceneImpl(hwnd, hdc);
}

static void PaintSelectorSceneImpl(HWND hwnd, HDC hdc)
{
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
        return;

    const bool fullscreenBackdrop = IsFullscreenBackdropActive();
    const bool d2dLoaded = LoadD2DAndDWrite() && g_d2dRenderTarget && g_d2dBrush;

    // Fullscreen con Direct2D: fondo y escena se dibujan juntos en un único
    // BeginDraw/EndDraw sobre una superficie del tamaño del área sucia
    // (rcPaint). El render target DC vuelca toda su superficie al EndDraw, así
    // que el fondo no puede pintarse antes con GDI: debe ir dentro de D2D.
    bool useSceneDc = false;
    RECT dirty = clientRect;
    if (fullscreenBackdrop && d2dLoaded && EnsureFullscreenD2DBitmaps())
    {
        // HWND render targets are redrawn as a complete frame. This avoids
        // clearing untouched areas when D2D Clear() resets the whole target.
        dirty = clientRect;
        useSceneDc = false;
    }
    const D2D1::Matrix3x2F baseTransform = useSceneDc
        ? D2D1::Matrix3x2F::Translation(-static_cast<float>(dirty.left),
                                        -static_cast<float>(dirty.top))
        : D2D1::Matrix3x2F::Identity();
    RECT bindRect = clientRect;
    HDC d2dDc = hdc;
    if (useSceneDc)
    {
        bindRect = RECT{ 0, 0, dirty.right - dirty.left, dirty.bottom - dirty.top };
        d2dDc = g_fsScene.dc;
    }
    auto presentScene = [&]() {
        if (useSceneDc)
            BitBlt(hdc, dirty.left, dirty.top, dirty.right - dirty.left,
                   dirty.bottom - dirty.top, g_fsScene.dc, 0, 0, SRCCOPY);
    };
    const bool usingFullscreenGpu = fullscreenBackdrop &&
        g_fsHwndRenderTarget && g_d2dRenderTarget == g_fsHwndRenderTarget;
    bool d2dActive = d2dLoaded && (usingFullscreenGpu ||
        (!fullscreenBackdrop && g_d2dDCRenderTarget &&
         SUCCEEDED(g_d2dDCRenderTarget->BindDC(d2dDc, &bindRect))));

    if (fullscreenBackdrop && !usingFullscreenGpu)
        return;

    if (d2dActive)
    {
        g_d2dRenderTarget->BeginDraw();
        g_d2dRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        g_d2dRenderTarget->SetTransform(baseTransform);
        g_d2dRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        if (fullscreenBackdrop)
            DrawFullscreenBackgroundD2D();
        if (g_selectionExpansionActive)
        {
            // The selected window is a DWM thumbnail composed independently
            // over this HWND. Clear the task-switcher UI immediately after
            // selection, while that thumbnail continues its expansion.
            const HRESULT drawResult = g_d2dRenderTarget->EndDraw();
            if (drawResult == static_cast<HRESULT>(0x8899000C))
                ReleaseD2DDeviceResources();
            else if (SUCCEEDED(drawResult))
                presentScene();
            return;
        }
        RECT sceneRect = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
        if (g_selectionExpansionActive)
            GetClientRect(hwnd, &sceneRect);
        D2D1_RECT_F sceneClip = fullscreenBackdrop
            ? D2D1::RectF(0.0f, 0.0f,
                          static_cast<float>(clientRect.right),
                          static_cast<float>(clientRect.bottom))
            : D2D1::RectF(0.0f, 0.0f,
                          static_cast<float>(sceneRect.right),
                          static_cast<float>(sceneRect.bottom));
        g_d2dRenderTarget->PushAxisAlignedClip(
            sceneClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
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
            g_d2dRenderTarget->PushLayer(&opacityParameters, sceneOpacityLayer);
        }

        // El background y el contenedor forman parte de la misma escena que
        // las cards: deben acompañar la escala de apertura/cierre y compartir
        // su pivote central.
        if (sceneScaleX != 1.0f || sceneScaleY != 1.0f ||
            g_selectionExpansionActive || fullscreenBackdrop)
        {
            g_d2dRenderTarget->SetTransform(GetSceneTransform() * baseTransform);
        }

        // Base background layer. None deliberately leaves the surface
        // transparent so whatever is underneath remains visible.
        D2D1_RECT_F bgRect = D2D1::RectF(
            static_cast<float>(sceneRect.left),
            static_cast<float>(sceneRect.top),
            static_cast<float>(sceneRect.right),
            static_cast<float>(sceneRect.bottom));
        if (!fullscreenBackdrop && g_baseBackgroundMode != BaseBackgroundMode::None &&
            g_dwmGlassEnabled)
        {
            float r = GetRValue(g_dwmBackgroundColor) / 255.0f;
            float g = GetGValue(g_dwmBackgroundColor) / 255.0f;
            float b = GetBValue(g_dwmBackgroundColor) / 255.0f;
            g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, g_dwmBackgroundOpacity));
            g_d2dRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }
        else if (!fullscreenBackdrop && g_baseBackgroundMode != BaseBackgroundMode::None)
        {
            // Fallback for DWM mode if glass is unavailable.
            g_d2dBrush->SetColor(D2D1::ColorF(0.70f, 0.74f, 0.82f, 0.18f));
            g_d2dRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }

        // Cursor spotlight: only the background is affected. Cards are drawn
        // afterwards, so the spotlight never bleeds across their surfaces.
        if (!fullscreenBackdrop && IsBackgroundSpotlightAllowed() &&
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
                g_d2dRenderTarget->FillRectangle(&bgRect, spotlight);
            }
        }

        // Experimental background is an independent layer over the base.
        if (!fullscreenBackdrop && IsPatternWavesActive())
        {
            // Pattern Waves is content inside the selector's existing DWM
            // container. The window region clips it; no second rounded mask
            // or Pattern-specific container is created here.
            // The caller already owns the scene transform, so Pattern Waves
            // draws in scene coordinates without a second scale.
            DrawPatternWaves(sceneRect);
            if (sceneScaleX != 1.0f || sceneScaleY != 1.0f)
            {
                g_d2dRenderTarget->SetTransform(GetSceneTransform() * baseTransform);
            }
        }

        // The global container border/shadow is itself a background visual.
        // With no base and no experimental layer, omit it so cards float over
        // the desktop. Pattern Waves keeps its own mask and reactive border.
        const bool hasVisualBackground =
            !fullscreenBackdrop && g_baseBackgroundMode != BaseBackgroundMode::None;
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
            g_d2dRenderTarget->DrawRoundedRectangle(&containerRect,
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
        if (!fullscreenBackdrop)
        {
            if (ID2D1RadialGradientBrush* ambient = EnsureAmbientShadowBrush())
            {
                const float w = static_cast<float>(sceneRect.right);
                const float h = static_cast<float>(sceneRect.bottom);
                ambient->SetCenter(D2D1::Point2F(w * 0.5f, h * 0.54f));
                ambient->SetRadiusX(w * 0.55f);
                ambient->SetRadiusY(h * 0.70f);
                g_d2dRenderTarget->FillRectangle(
                    D2D1::RectF(0.0f, 0.0f, w, h), ambient);
            }
        }

    // Todas las sombras antes que las cards: la sombra de una card nunca queda
    // por encima de otra card ya pintada.
    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
        {
            if (ResolveGroupIndex(slot) < 0)
                continue;
            const float shadowDist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
            const bool shadowSelected = (shadowDist < 0.5f);
            const RECT shadowCard = GetSlotCardRect(slot);
            const float shadowRadius = shadowSelected
                ? static_cast<float>(g_cardStyle.selectedCornerRadius) * g_uiScale.value
                : static_cast<float>(g_cardStyle.cornerRadius) * g_uiScale.value;
            float shadowStrength = shadowSelected ? 0.55f : 0.42f;
            if (g_cardExitActive && slot == g_cardExitSlot)
                shadowStrength *= GetCardExitOpacity();
            PaintCardShadowD2D(shadowCard, shadowRadius, shadowStrength);
        }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
        {
            int index = ResolveGroupIndex(slot);
            if (index < 0)
                continue;

            ID2D1Layer* cardExitLayer = nullptr;
            if (g_cardExitActive && slot == g_cardExitSlot && g_d2dRenderTarget)
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
                    g_d2dRenderTarget->PushLayer(&layerParameters, cardExitLayer);
                }
            }

            float distFromCenter = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
            bool selected = (distFromCenter < 0.5f);
            RECT cardRect = GetSlotCardRect(slot);
            float radius = selected ? static_cast<float>(g_cardStyle.selectedCornerRadius) * g_uiScale.value
                                    : static_cast<float>(g_cardStyle.cornerRadius) * g_uiScale.value;
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
                g_d2dRenderTarget->FillRoundedRectangle(&cr, surfaceBrush);
            }
            else
            {
                g_d2dBrush->SetColor(bgCol);
                g_d2dRenderTarget->FillRoundedRectangle(&cr, g_d2dBrush);
            }

            D2D1_COLOR_F borderCol = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.92f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.82f);
            g_d2dBrush->SetColor(borderCol);
            g_d2dRenderTarget->DrawRoundedRectangle(&cr, g_d2dBrush,
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
            g_d2dRenderTarget->DrawRoundedRectangle(&pr, g_d2dBrush, DpiPx(1.0f));

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
                    g_d2dRenderTarget->DrawBitmap(
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
                g_d2dRenderTarget->DrawRoundedRectangle(&hoverRect, g_d2dBrush, DpiPx(1.25f));

                if (g_hoveredCloseButton)
                {
                    RECT closeRect = GetCloseHoverButtonRect(slot);
                    ID2D1RadialGradientBrush* closeBrush = EnsureCloseAccentBrush();
                    ID2D1RoundedRectangleGeometry* cardMask = GetCachedCardMask(cr);
                    ID2D1Layer* closeLayer =
                        cardMask ? AcquireD2DLayer(g_closeLayer) : nullptr;
                    if (cardMask && closeLayer)
                    {
                        g_d2dRenderTarget->PushLayer(
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
                        g_d2dRenderTarget->FillEllipse(&glowEllipse, closeBrush);
                    }
                    if (closeLayer)
                    {
                        g_d2dRenderTarget->PopLayer();
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
                        g_d2dRenderTarget->FillGeometry(closeGeometry, g_d2dBrush);
                        g_d2dBrush->SetColor(D2D1::ColorF(0.45f, 0.10f, 0.12f, 0.62f));
                        g_d2dRenderTarget->DrawGeometry(closeGeometry, g_d2dBrush, DpiPx(1.0f));
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
                    g_d2dRenderTarget->DrawText(closeGlyph, 1, g_dwriteCloseFormat,
                                                  &closeTextRect, g_d2dBrush);
                }
            }
            if (cardExitLayer)
            {
                g_d2dRenderTarget->PopLayer();
            }
        }

        if (g_selectionExpansionActive && g_d2dRenderTarget)
        {
            // El rectángulo de expansión ya está expresado en coordenadas del
            // cliente ampliado, no en la escena original trasladada.
            g_d2dRenderTarget->SetTransform(baseTransform);
            const float p = g_selectionExpansionProgress;
            const float radius = std::max(0.0f, DpiPx(14.0f) * (1.0f - p));
            D2D1_ROUNDED_RECT expansionRect = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(g_selectionExpansionRect.left),
                            static_cast<float>(g_selectionExpansionRect.top),
                            static_cast<float>(g_selectionExpansionRect.right),
                            static_cast<float>(g_selectionExpansionRect.bottom)),
                radius, radius);
            g_d2dBrush->SetColor(D2D1::ColorF(0.18f, 0.20f, 0.24f,
                                               0.70f * (1.0f - p) + 0.18f));
            g_d2dRenderTarget->DrawRoundedRectangle(&expansionRect,
                                                       g_d2dBrush,
                                                       DpiPx(1.5f));
        }
        if (!g_selectionExpansionActive)
            PaintCounterD2D();
        if (sceneOpacityLayer)
        {
            g_d2dRenderTarget->PopLayer();
        }
        g_d2dRenderTarget->SetTransform(baseTransform);
        g_d2dRenderTarget->PopAxisAlignedClip();
        const HRESULT drawResult = g_d2dRenderTarget->EndDraw();
        if (drawResult == static_cast<HRESULT>(0x8899000C))  // D2DERR_RECREATE_TARGET
            ReleaseD2DDeviceResources();
        else if (SUCCEEDED(drawResult))
            presentScene();
        return;
    }

    if (fullscreenBackdrop)
    {
        // Strict hardware-only policy: never switch Fullscreen back to GDI.
        return;
    }

    if (g_selectionExpansionActive)
    {
        // Keep the selected fullscreen backdrop behind the expanding DWM
        // thumbnail. For regular DWM mode, black remains transparent glass.
        if (IsFullscreenBackdropActive())
        {
            if (!DrawFullscreenBackgroundGdi(hdc))
                LogFullscreenHdcDrawFailure();
        }
        else
        {
            HGDIOBJ stockBrush = LoadGdiFunctions() && g_getStockObject
                ? g_getStockObject(BLACK_BRUSH) : nullptr;
            HBRUSH glassClearBrush = reinterpret_cast<HBRUSH>(stockBrush);
            if (glassClearBrush)
                FillRect(hdc, &clientRect, glassClearBrush);
        }
        return;
    }

    const bool fullscreenBackdropGdi = IsFullscreenBackdropActive();
    int fullscreenGdiState = 0;
    if (fullscreenBackdropGdi)
    {
        fullscreenGdiState = SaveDC(hdc);
        if (!DrawFullscreenBackgroundGdi(hdc))
            LogFullscreenHdcDrawFailure();
        SetViewportOrgEx(hdc, g_selectorSceneOffset.x,
                         g_selectorSceneOffset.y, nullptr);
    }

    const bool hasVisualBackground = !fullscreenBackdropGdi &&
        (g_baseBackgroundMode != BaseBackgroundMode::None ||
         IsPatternWavesActive());
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

    // Todas las sombras antes que las cards (ver ruta Direct2D).
    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
    {
        if (ResolveGroupIndex(slot) < 0)
            continue;
        float shadowDist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
        bool shadowSelected = (shadowDist < 0.5f);
        int shadowRadius = shadowSelected
            ? ScaleLayoutPx(static_cast<float>(g_cardStyle.selectedCornerRadius))
            : ScaleLayoutPx(static_cast<float>(g_cardStyle.cornerRadius));
        PaintCardShadowGDI(hdc, GetSlotCardRect(slot), shadowRadius,
                           shadowSelected ? 0.65f : 0.50f);
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
    if (fullscreenGdiState)
        RestoreDC(hdc, fullscreenGdiState);
}

static LRESULT CALLBACK SelectorWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        UpdateSelectorControls();
        return 0;

    // Keyboard input is handled only while this selector owns the foreground.
    // Explorer forwards Alt+Tab to start/navigate sessions; these window messages
    // handle subsequent navigation and confirmation without a desktop-wide hook.
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        if (!IsSelectorActuallyActive() || g_cleanupInProgress)
            break;

        MarkSelectorActivity();
        switch (wParam)
        {
        case VK_TAB:
            if (!g_uiTabDown)
            {
                const bool shiftDown =
                    (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                if (shiftDown) SelectPrevious();
                else SelectNext();
                g_uiTabDown = true;
                g_tabRepeatStarted = false;
                SetTimer(hwnd, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
            }
            return 0;
        case VK_LEFT:
            SelectPrevious();
            return 0;
        case VK_RIGHT:
            SelectNext();
            return 0;
        case VK_RETURN:
            ConfirmSelection();
            return 0;
        case VK_ESCAPE:
            CancelSelection();
            return 0;
        default:
            // While the selector is active, do not let unrelated keys trigger
            // default window/menu behavior.
            return 0;
        }
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        if (!IsSelectorActuallyActive())
            break;
        MarkSelectorActivity();
        if (wParam == VK_TAB)
        {
            g_uiTabDown = false;
            g_tabRepeatStarted = false;
            KillTimer(hwnd, kTabRepeatTimerId);
        }
        // Alt/AltGr release is confirmed by the input thread's physical-state
        // polling, which also works when the former foreground was elevated.
        return 0;
    }

    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_DEADCHAR:
    case WM_SYSDEADCHAR:
        if (IsSelectorActuallyActive())
            return 0;
        break;

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
        g_paintDirty = ps.rcPaint;
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

static HWND g_selectorRegionAppliedWindow = nullptr;
static HWND g_selectorRegionClearedWindow = nullptr;
static int g_selectorRegionAppliedWidth = 0;
static int g_selectorRegionAppliedHeight = 0;
static int g_selectorRegionAppliedRadius = 0;

static void UpdateSelectorSurfaceTransform()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    if (g_selectionExpansionActive || IsFullscreenBackdropActive())
    {
        // El mismo HWND del selector cubre temporalmente el monitor para el blur
        // y la expansión de selección; esas superficies deben quedar sin región.
        if (g_selectorRegionClearedWindow != g_selector &&
            SetWindowRgn(g_selector, nullptr, TRUE))
        {
            // SetWindowRgn(NULL) elimina la región; recordar el estado evita
            // forzar una recomposición completa repetidamente durante frames.
            g_selectorRegionClearedWindow = g_selector;
            g_selectorRegionAppliedWindow = nullptr;
            g_selectorRegionAppliedWidth = 0;
            g_selectorRegionAppliedHeight = 0;
            g_selectorRegionAppliedRadius = 0;
        }
        return;
    }

    // Fuera del modo fullscreen, el HWND conserva su geometría delimitada.
    // La escena lógica permanece igual; el blur sólo cambia el viewport físico
    // del mismo HWND y aplica una traslación a tarjetas, thumbnails e input.
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
    int radius = GetSelectorCornerRadiusPx(width, height);
    if (g_selectorRegionAppliedWindow == hwnd &&
        g_selectorRegionAppliedWidth == width &&
        g_selectorRegionAppliedHeight == height &&
        g_selectorRegionAppliedRadius == radius)
        return;

    HRGN region = g_createRoundRectRgn(0, 0, width + 1, height + 1,
                                       radius * 2, radius * 2);
    if (!region)
        return;
    if (SetWindowRgn(hwnd, region, TRUE))
    {
        g_selectorRegionClearedWindow = nullptr;
        g_selectorRegionAppliedWindow = hwnd;
        g_selectorRegionAppliedWidth = width;
        g_selectorRegionAppliedHeight = height;
        g_selectorRegionAppliedRadius = radius;
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
    const RECT selectorScreenRect = { x, y, x + g_runtimeSelectorWidth,
                                      y + g_runtimeSelectorHeight };
    const bool fullscreenBackdropReady = PrepareFullscreenBackdrop(
        monitorInfo.rcMonitor, selectorScreenRect);
    if (fullscreenBackdropReady)
    {
        g_selectorSceneOffset = POINT{ g_blurredInitialRect.left,
                                       g_blurredInitialRect.top };
        SetWindowPos(g_selector, HWND_TOPMOST, monitorInfo.rcMonitor.left,
                     monitorInfo.rcMonitor.top,
                     monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                     monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                     SWP_NOACTIVATE | SWP_HIDEWINDOW);
    }
    else
    {
        HideFullscreenBackdrop(true);
        g_selectorSceneOffset = POINT{};
        ApplySelectorRoundedRegion(g_selector);
    }
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

    // Allows the selector to take foreground after Explorer forwards Alt+Tab.
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
    InterlockedExchange(&g_selectionTransitionActive, 0);
    KillTimer(g_selector, kTabRepeatTimerId);
    DisarmFramePump();
    KillTimer(g_selector, kPatternTimerId);
    KillTimer(g_selector, kActivityTimerId);
    g_tabRepeatStarted = false;
    g_uiTabDown = false;
    g_backgroundSpotlightActive = false;
    const bool wasFullscreen = IsFullscreenBackdropActive();
    RECT restoreRect = {};
    if (wasFullscreen)
    {
        restoreRect = RECT{
            g_blurredMonitorRect.left + g_blurredInitialRect.left,
            g_blurredMonitorRect.top + g_blurredInitialRect.top,
            g_blurredMonitorRect.left + g_blurredInitialRect.right,
            g_blurredMonitorRect.top + g_blurredInitialRect.bottom
        };
    }
    HideFullscreenBackdrop(true);

    // No conservar superficies DWM registradas mientras el selector está oculto:
    // así ninguna miniatura puede sobrevivir visualmente al cierre. La siguiente
    // sesión las registra de nuevo mediante UpdateThumbnailSlots.
    UnregisterAllThumbnails();
    ShowWindow(g_selector, SW_HIDE);
    if (wasFullscreen)
    {
        SetWindowPos(g_selector, HWND_TOPMOST, restoreRect.left, restoreRect.top,
                     g_runtimeSelectorWidth, g_runtimeSelectorHeight,
                     SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
        UnregisterAllThumbnails();
    }
    if (g_selectorFocusTaken)
    {
        SetWindowLongPtrW(g_selector, GWL_EXSTYLE,
                          GetWindowLongPtrW(g_selector, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
        g_selectorFocusTaken = false;
    }
    UiSetInteractive(false);
    g_selectorAnimation = SelectorAnimationState::None;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    g_sceneOpacity = 1.0f;
    g_scenePresence = 1.0f;
    g_carouselSpread = 1.0f;
    g_cardSnapScale = 1.0f;
    g_animActive = false;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitTarget = nullptr;
    g_selectionExpansionActive = false;
    g_selectionExpansionFinishPending = false;
    g_selectionActivationRequested = false;
    g_selectionActivationRequestTick = 0;
    g_selectionExpansionTarget = nullptr;
    g_selectionExpansionDestinationRect = {};
    g_selectionExpansionSceneOffset = {};
    g_selectionExpansionProgress = 0.0f;
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
            KillTimer(g_selector, kTabRepeatTimerId);
        DisarmFramePump();
        KillTimer(g_selector, kPatternTimerId);
        KillTimer(g_selector, kActivityTimerId);
        UnregisterAllThumbnails();
        HWND selector = g_selector;
        g_selector = nullptr;
        DestroyWindow(selector);
    }

    HideFullscreenBackdrop(true);
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
    HWND restoreFocus = FocusRestoreTarget();
    DestroySelector();
    g_state = SelectorState::Idle;
    g_cleanupInProgress = false;
    CleanupKeyboardState();
    if (restoreFocus)
        ActivateWindow(restoreFocus);
}

static void UpdateCarouselAnimation(HWND)
{
    if (!g_animActive)
        return;
    const float progress = ElapsedFraction(g_animStartTime,
                                           CardNavigationDuration());
    // Card navigation has its own style, independent from selector/background
    // opening and closing. Linear moves at constant velocity; Smooth uses a
    // symmetric smoothstep curve (ease-in/ease-out) over the SAME duration.
    // Neither style overshoots; Bouncy keeps its spring response.
    const float eased = g_cardMotionStyle == CardMotionStyle::Bouncy
        ? SpringOvershoot(progress)
        : g_cardMotionStyle == CardMotionStyle::Linear
            ? progress : EaseSmooth(progress);
    g_animOffset = g_animStartOffset * (1.0f - eased);

    const float remaining = fabsf(g_animOffset);
    const float bump = Clamp01(1.0f - remaining / 0.34f);
    const float scaleAccent = g_cardMotionStyle == CardMotionStyle::Bouncy ? 0.045f
                            : g_cardMotionStyle == CardMotionStyle::Smooth ? 0.025f : 0.0f;
    g_cardSnapScale = 1.0f + scaleAccent * sinf(bump * 3.14159265358979323846f);

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

    // With a single visible card there is nowhere to navigate. Ignore every
    // navigation source (arrows, Tab/repeat, forwarded hotkeys and mouse wheel)
    // so the lone card stays centered and no selection animation is triggered.
    if (g_singleWindowCard && g_groups.size() == 1)
        return;
    // Solo se marca SelectionChange desde Open: no se toca la escena si la
    // apertura o el cierre están en curso.
    if (g_selectorAnimation == SelectorAnimationState::Open)
    {
        g_selectorAnimation = SelectorAnimationState::SelectionChange;
        g_selectorAnimationStart = FrameClockNow();
    }

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

// Esta función solo se ejecuta desde el input thread. Repite la ruta completa
// de activación porque algunas ventanas elevadas aún están restaurándose cuando
// termina la expansión del selector.
static bool TryActivateWindowOnInputThread(HWND target)
{
    if (!target || !IsWindow(target))
        return false;

    if (IsIconic(target))
        ShowWindowAsync(target, SW_RESTORE);

    g_inputActivationLastSetForegroundResult = SetForegroundWindow(target);
    if (GetForegroundWindow() == target)
        return true;

    // No se bloquea el input thread esperando al hilo de la ventana destino.
    // SWP_ASYNCWINDOWPOS entrega el cambio de z-order de forma asíncrona.
    SetWindowPos(target, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                     SWP_ASYNCWINDOWPOS);
    g_inputActivationLastSetForegroundResult = SetForegroundWindow(target);
    return GetForegroundWindow() == target;
}

// Esta función solo se ejecuta desde InputThreadProc / su WndProc.
static void ActivateWindowOnInputThread(HWND control, HWND target)
{
    if (TryActivateWindowOnInputThread(target))
    {
        g_inputActivationTarget = nullptr;
        g_inputActivationRetryCount = 0;
        KillTimer(control, kActivationRetryTimerId);
        return;
    }

    // Restore/activation of an elevated or busy application may complete
    // asynchronously. Retry briefly from the input thread instead of dropping
    // the request after the expansion has hidden the selector.
    g_inputActivationTarget = target;
    g_inputActivationRetryCount = 0;
    SetTimer(control, kActivationRetryTimerId, kActivationRetryMs, nullptr);
}

static void InputRetryActivation(HWND control)
{
    HWND target = g_inputActivationTarget;
    if (!target || !IsWindow(target) ||
        ++g_inputActivationRetryCount > kMaxActivationRetries)
    {
        g_inputActivationTarget = nullptr;
        g_inputActivationRetryCount = 0;
        KillTimer(control, kActivationRetryTimerId);
        return;
    }

    if (TryActivateWindowOnInputThread(target))
    {
        g_inputActivationTarget = nullptr;
        g_inputActivationRetryCount = 0;
        KillTimer(control, kActivationRetryTimerId);
    }
    else if (g_inputActivationRetryCount == kMaxActivationRetries)
    {
        Wh_Log(L"Foreground failed target=%p current=%p set=%d after %d retries",
               target, GetForegroundWindow(),
               g_inputActivationLastSetForegroundResult,
               kMaxActivationRetries);
    }
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
        StartSelectorClose(FocusRestoreTarget());
        return;
    }

    // La confirmación conserva el selector visible y comienza la expansión de
    // la tarjeta seleccionada. Si el monitor, el HWND o el renderer no pueden
    // sostenerla, StartSelectionExpansion aplica el fallback inmediato.
    if (!target || !IsWindow(target))
    {
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    if (!g_expandSelectedWindow)
    {
        // Modo directo: conservar la confirmación y la activación originales,
        // pero sin ejecutar la expansión visual de 240 ms.
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(target);
        return;
    }
    const bool targetWasMinimized = IsIconic(target);
    // No restaurar aquí: la ventana real podría empezar a aparecer detrás de
    // la miniatura antes de que concluya la expansión. ActivateWindow la restaura
    // cuando el selector ya cubre toda la pantalla.
    StartSelectionExpansion(target, targetWasMinimized);
}

static void CancelSelection()
{
    if (IsSelectionExpansionActive())
    {
        HWND restore = FocusRestoreTarget();
        g_selectionExpansionActive = false;
        InterlockedExchange(&g_selectionTransitionActive, 0);
        g_selectionExpansionTarget = nullptr;
        g_pendingActivationTarget = nullptr;
        g_cleanupInProgress = true;
        DestroySelector();
        g_state = SelectorState::Idle;
        g_cleanupInProgress = false;
        CleanupKeyboardState();
        ActivateWindow(restore);
        return;
    }
    if ((!IsSelectorActuallyActive() && !g_selector) || g_cleanupInProgress)
        return;

    LeaveActiveState(SelectorState::Canceling);
    StartSelectorClose(FocusRestoreTarget());
}

// COMANDOS EN EL UI THREAD

static bool UiCommandMatchesSession(WPARAM sessionId)
{
    return static_cast<LONG>(sessionId) == g_uiSessionId && IsSelectorActive();
}

// Sesiones iniciadas desde Explorer: este permite al proceso del selector tomar
// el foco aunque lo tuviera una ventana elevada. El selector procesa la
// navegación en su WndProc; el polling de modificadores confirma Alt/Ctrl.
// WS_EX_NOACTIVATE se retira solo durante la sesión; HideSelectorForSession lo
// restablece. Si no logra tomar el foco, el hotkey reenviado sigue navegando.
static void UiTakeForeground()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    LONG_PTR exStyle = GetWindowLongPtrW(g_selector, GWL_EXSTYLE);
    SetWindowLongPtrW(g_selector, GWL_EXSTYLE, exStyle & ~WS_EX_NOACTIVATE);
    g_selectorFocusTaken = SetForegroundWindow(g_selector) != FALSE;
    if (!g_selectorFocusTaken)
    {
        SetWindowLongPtrW(g_selector, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE);
        Wh_Log(L"Selector could not take the foreground");
    }
}

// Si el selector tomó el foco, cancelar debe devolverlo a la ventana de origen.
static HWND FocusRestoreTarget()
{
    if (g_selectorFocusTaken && g_selectorOriginWindow &&
        IsWindow(g_selectorOriginWindow))
        return g_selectorOriginWindow;
    return nullptr;
}

static void UiOpen(LONG sessionId, bool shiftHeld, bool fromHotkey)
{
    if (InterlockedCompareExchange(&g_selectionTransitionActive, 0, 0) != 0)
        return;
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
    if (fromHotkey)
    {
        // El hook no vio este Tab: no hay keyup que detenga un autorepeat.
        UiTakeForeground();
        return;
    }
    g_uiTabDown = true;
    g_tabRepeatStarted = false;
    SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
}

static float GetCarouselDragSlotSpacing();

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
    pt.x -= g_selectorSceneOffset.x;
    pt.y -= g_selectorSceneOffset.y;
    g_backgroundSpotlightPoint = pt;
    if (g_mousePressActive && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
    {
        POINT screenPt = {};
        screenPt.x = static_cast<LONG>(static_cast<ULONG>(static_cast<ULONG64>(packed) & 0xFFFFFFFFULL));
        screenPt.y = static_cast<LONG>(static_cast<ULONG>((static_cast<ULONG64>(packed) >> 32) & 0xFFFFFFFFULL));
        const int dx = screenPt.x - g_mousePressScreenPoint.x;
        if (g_groups.size() > 1 && !g_singleWindowCard &&
            (g_mouseDragging || std::abs(dx) >= 7))
        {
            g_mouseDragging = true;
            const float spacing = GetCarouselDragSlotSpacing();
            const float maxOffset = std::max(1.0f, static_cast<float>(g_groups.size() - 1));
            g_animOffset = std::max(-maxOffset, std::min(maxOffset,
                g_mousePressAnimOffset + static_cast<float>(dx) / spacing *
                    (static_cast<float>(g_cardMotionSpeedPercent) / 100.0f)));
            g_animStartOffset = g_animOffset;
            g_cardSnapScale = 1.0f;
            UpdateSelectorControls();
            RequestFrame();
        }
    }
    RECT clientRect = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
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

static float GetCarouselDragSlotSpacing()
{
    const int center = GetCarouselCenterSlot();
    if (center > 0)
    {
        RECT a = GetSlotCardRect(center - 1);
        RECT b = GetSlotCardRect(center);
        const float spacing = static_cast<float>(
            ((b.left + b.right) - (a.left + a.right)) / 2.0);
        if (spacing > 24.0f)
            return spacing;
    }
    if (center + 1 < GetCarouselSlotCount())
    {
        RECT a = GetSlotCardRect(center);
        RECT b = GetSlotCardRect(center + 1);
        const float spacing = static_cast<float>(
            ((b.left + b.right) - (a.left + a.right)) / 2.0);
        if (spacing > 24.0f)
            return spacing;
    }
    return 300.0f;
}

static void FinishMouseCarouselDrag()
{
    if (!g_mousePressActive)
        return;

    const bool dragged = g_mouseDragging;
    const int pressedSlot = g_mousePressedSlot;
    g_mousePressActive = false;
    g_mouseDragging = false;
    g_mousePressedSlot = -1;

    if (dragged)
    {
        const int count = static_cast<int>(g_groups.size());
        if (count <= 1 || g_singleWindowCard)
        {
            g_animOffset = 0.0f;
            g_animActive = false;
            g_cardSnapScale = 1.0f;
        }
        else
        {
            int steps = static_cast<int>(roundf(-g_animOffset));
            // Avoid circling more than once during one gesture.
            steps = std::max(-(count - 1), std::min(count - 1, steps));
            g_selected = (g_mousePressSelected + steps) % count;
            if (g_selected < 0)
                g_selected += count;

            // Rebase the dragged geometry to the newly selected logical item;
            // the residual offset then eases back to the canonical carousel.
            g_animStartOffset = g_animOffset + static_cast<float>(steps);
            g_animOffset = g_animStartOffset;
            g_animStartTime = FrameClockNow();
            g_animActive = fabsf(g_animStartOffset) > 0.015f;
            if (!g_animActive)
            {
                g_animOffset = 0.0f;
                g_animStartOffset = 0.0f;
                g_cardSnapScale = 1.0f;
            }
            UpdateSelectorControls();
        }
        RequestFrame();
        return;
    }

    // A press that did not cross the drag threshold keeps the existing click
    // behavior: close button, activate center card, or navigate to a side card.
    if (pressedSlot < 0)
        return;
    if (pressedSlot == GetCarouselCenterSlot())
        ConfirmSelection();
    else
        StartSlide(pressedSlot - GetCarouselCenterSlot());
}

static void UiHandleMouseButton(UINT message, POINT screenPt)
{
    if (!IsSelectorActive() || !g_selector || !IsWindow(g_selector))
        return;
    MarkSelectorActivity();

    POINT scenePt = screenPt;
    ScreenToClient(g_selector, &scenePt);
    scenePt.x -= g_selectorSceneOffset.x;
    scenePt.y -= g_selectorSceneOffset.y;

    if (message == WM_LBUTTONUP)
    {
        FinishMouseCarouselDrag();
        return;
    }
    if (message != WM_LBUTTONDOWN)
        return;

    int clickedSlot = GetSlotAtPoint(scenePt);
    bool onCloseButton = false;
    if (clickedSlot < 0 && g_hoveredSlot >= 0 && g_hoveredCloseButton)
    {
        RECT hoveredClose = GetCloseHoverButtonRect(g_hoveredSlot);
        if (PtInRect(&hoveredClose, scenePt))
        {
            clickedSlot = g_hoveredSlot;
            onCloseButton = true;
        }
    }
    if (clickedSlot < 0)
    {
        g_mousePressActive = false;
        g_mouseDragging = false;
        g_mousePressedSlot = -1;
        g_hoveredSlot = -1;
        g_hoveredCloseButton = false;
        CancelSelection();
        return;
    }

    RECT closeRect = GetCloseHoverButtonRect(clickedSlot);
    if (onCloseButton ||
        (g_hoveredSlot == clickedSlot && g_hoveredCloseButton &&
         PtInRect(&closeRect, scenePt)))
    {
        CloseWindowForSlot(clickedSlot);
        return;
    }

    // Defer click activation/navigation until release so a normal press can
    // become a drag without accidentally activating the center window.
    g_mousePressActive = true;
    g_mouseDragging = false;
    g_mousePressedSlot = clickedSlot;
    g_mousePressScreenPoint = screenPt;
    g_mousePressSelected = g_selected;
    g_mousePressAnimOffset = g_animOffset;
    g_animActive = false;
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
        UiOpen(static_cast<LONG>(msg.wParam), (msg.lParam & kUiOpenShift) != 0,
               (msg.lParam & kUiOpenFromHotkey) != 0);
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

// INPUT THREAD. El mouse hook solo publica eventos; teclado y modifiers siguen
// rutas separadas por Explorer, el selector y el polling físico.

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
    if (g_modifierReleasePolling)
    {
        g_modifierReleasePolling = false;
        if (g_inputControlWnd)
            KillTimer(g_inputControlWnd, kModifierReleasePollTimerId);
    }
    ClearSelectorKeyboardSession();
    PostUiCommand(command, static_cast<WPARAM>(id), 0);
}

static bool SessionModifierReleased()
{
    if (g_sessionModifier == ModifierSession::LeftAlt)
        return !g_altLeftDown;

    if (g_sessionModifier == ModifierSession::RightAlt)
        return !g_altRightDown;

    if (g_sessionModifier == ModifierSession::ControlAlt)
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

// Todas las sesiones de modificador se confirman al soltarse. El polling
// cubre los key-up que pueden perderse ante ventanas con privilegios elevados.
static void OnModifierReleased()
{
    if (InputSessionActive() && SessionModifierReleased())
        InputEndSession(WM_UI_CONFIRM);
    g_altGrActive = IsAltGrPhysicallyDown();
}

// Abre una sesión a partir del hotkey reenviado desde Explorer. Solo input thread.
// Devuelve false (con el estado ya limpio) si el UI thread no aceptó la orden.
static bool StartInputSession(bool leftAlt, bool rightAlt, bool ctrlHeld,
                              bool shift, bool fromHotkey)
{
    const bool controlAltHeld = ctrlHeld;
    LONG id = InterlockedIncrement(&g_nextSessionId);
    if (id == 0)
        id = InterlockedIncrement(&g_nextSessionId);

    g_inputSessionId = id;
    InterlockedExchange(&g_sessionActive, id);
    // AltGr is physically Ctrl + RightAlt. Track its RightAlt as the primary
    // release key; other Ctrl+Alt sessions complete when both modifiers are up.
    const bool altGrSession = rightAlt && ctrlHeld && !leftAlt;
    g_sessionModifier = altGrSession
        ? ModifierSession::RightAlt
        : (controlAltHeld ? ModifierSession::ControlAlt
                          : (leftAlt ? ModifierSession::LeftAlt
                                     : ModifierSession::RightAlt));
    g_altGrActive = rightAlt && ctrlHeld;

    const bool posted = PostUiCommand(
        WM_UI_OPEN, static_cast<WPARAM>(id),
        (shift ? kUiOpenShift : 0) | (fromHotkey ? kUiOpenFromHotkey : 0));
    if (posted)
    {
        g_altMenuMaskPending = !g_altGrActive;
        // Poll every Alt-based session, not only forwarded Explorer hotkeys.
        // This is the release fallback when an elevated foreground prevents the
        // low-level hook from observing Alt/AltGr key-up.
        if (g_inputControlWnd)
        {
            g_modifierReleasePolling = true;
            g_modifierReleaseCount = 0;
            if (!SetTimer(g_inputControlWnd, kModifierReleasePollTimerId,
                          kModifierReleasePollMs, nullptr))
            {
                g_modifierReleasePolling = false;
                Wh_Log(L"SetTimer for modifier-release polling failed: %lu",
                       GetLastError());
            }
        }
        return true;
    }

    InterlockedCompareExchange(&g_sessionActive, 0, id);
    ClearSelectorKeyboardSession();
    return false;
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

    if (wParam == WM_LBUTTONUP)
    {
        LPARAM packedPt = MAKELPARAM(static_cast<WORD>(static_cast<short>(mouse->pt.x)),
                                     static_cast<WORD>(static_cast<short>(mouse->pt.y)));
        if (!PostUiCommand(WM_UI_MOUSE_BUTTON, WM_LBUTTONUP, packedPt))
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        return 1;
    }

    if (wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP ||
        wParam == WM_NCLBUTTONUP || wParam == WM_NCRBUTTONUP ||
        wParam == WM_NCMBUTTONUP)
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

// HOTKEY REENVIADO DESDE EXPLORER Y SONDEO DE MODIFICADORES (input thread)
//
// Explorer entrega el WM_HOTKEY a la ventana de control. El selector maneja
// sus propias teclas cuando toma el foco; este timer detecta la liberación de
// Alt/Ctrl mediante el estado físico, sin un hook global de teclado.

static void InputSyncModifiersFromSystem()
{
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    g_altGrActive = IsAltGrPhysicallyDown();
}

static void InputHandleForwardedHotkey(WPARAM flags)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;
    if (InterlockedCompareExchange(&g_selectionTransitionActive, 0, 0) != 0)
        return;

    SyncInputSession();
    const bool shift = (flags & kHotkeyFlagBackwards) != 0;

    if (InputSessionActive())
    {
        // La sesión ya existe y el hook no vio esta pulsación (por eso llegó el
        // hotkey): un único paso de navegación, sin segunda sesión.
        PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                      static_cast<LPARAM>(shift ? -1 : 1));
        return;
    }

// El hotkey llega desde Explorer; tomar los modificadores físicos para
    // distinguir Alt, AltGr y Ctrl+Alt.
    InputSyncModifiersFromSystem();
    bool leftAlt = g_altLeftDown;
    const bool rightAlt = g_altRightDown;
    if (!leftAlt && !rightAlt)
    {
        // El hotkey garantiza que Alt está pulsado aunque no se lea todavía.
        g_altLeftDown = true;
        leftAlt = true;
    }
    const bool ctrlHeld = g_ctrlLeftDown || g_ctrlRightDown ||
                          (flags & kHotkeyFlagCtrl) != 0;

    if (!StartInputSession(leftAlt, rightAlt, ctrlHeld, shift, true))
        return;

    // StartInputSession owns the shared modifier-release polling setup.
}

// Confirma la liberación del modificador leyendo su estado físico periódicamente;
// no depende de un hook global de teclado ni de recibir key-up del foreground.
static void InputPollModifierRelease(HWND control)
{
    if (!g_modifierReleasePolling || !InputSessionActive())
    {
        g_modifierReleasePolling = false;
        KillTimer(control, kModifierReleasePollTimerId);
        SyncInputSession();
        return;
    }

    // Do not depend on low-level key-up delivery: query the physical key state
    // on each tick, including both Ctrl and Alt for AltGr/Ctrl+Alt sessions.
    InputSyncModifiersFromSystem();
    if (!SessionModifierReleased())
    {
        g_modifierReleaseCount = 0;
        return;
    }
    if (++g_modifierReleaseCount < kModifierReleaseTicks)
        return;

    g_modifierReleasePolling = false;
    KillTimer(control, kModifierReleasePollTimerId);
    SendAltMenuMaskIfNeeded();
    OnModifierReleased();
}

static LRESULT CALLBACK InputControlProc(HWND hwnd, UINT message, WPARAM wParam,
                                         LPARAM lParam)
{
    if (g_hotkeyMessage != 0 && message == g_hotkeyMessage)
    {
        InputHandleForwardedHotkey(wParam);
        return 0;
    }
    if (message == WM_TIMER && wParam == kModifierReleasePollTimerId)
    {
        InputPollModifierRelease(hwnd);
        return 0;
    }
    if (message == WM_TIMER && wParam == kActivationRetryTimerId)
    {
        InputRetryActivation(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
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
    WNDCLASSEXW controlClass = {};
    controlClass.cbSize = sizeof(controlClass);
    controlClass.lpfnWndProc = InputControlProc;
    controlClass.hInstance = g_hModule;
    controlClass.lpszClassName = kInputControlClass;
    const bool controlClassRegistered =
        RegisterClassExW(&controlClass) != 0 ||
        GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    HWND control = controlClassRegistered
        ? CreateWindowExW(0, kInputControlClass, kInputControlTitleIdle, 0,
                          0, 0, 0, 0, HWND_MESSAGE, nullptr, g_hModule, nullptr)
        : nullptr;
    if (control && g_hotkeyMessage != 0)
    {
        // Explorer publica el hotkey desde otro proceso: permitirlo aunque esta
        // herramienta tenga otro nivel de integridad.
        ChangeWindowMessageFilterEx(control, g_hotkeyMessage, MSGFLT_ALLOW,
                                    nullptr);
    }
    if (!control)
    {
        Wh_Log(L"Input control window creation failed: %lu", GetLastError());
    }
    else
    {
        g_inputControlWnd = control;
        ok = true;
    }
    if (!ok && control)
        Wh_Log(L"Input control window initialization failed");

    // UI thread ya listo: Explorer puede empezar a reenviar el hotkey.
    if (ok && control)
        SetWindowTextW(control, kInputControlTitleReady);

    InterlockedExchange(&g_inputInitOk, ok ? 1 : 0);
    SetEvent(g_inputReadyEvent);

    if (ok)
    {
        while (GetMessageW(&message, nullptr, 0, 0) > 0)
        {
            if (message.message == WM_IN_ACTIVATE)
            {
                ActivateWindowOnInputThread(
                    control,
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

    // El propio hilo retira sus hooks y destruye su ventana. Primero se vuelve a
    // publicar "no listo" para que Explorer deje el hotkey al sistema.
    if (control)
        SetWindowTextW(control, kInputControlTitleIdle);
    g_modifierReleasePolling = false;
    if (g_mouseHook)
    {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    g_inputControlWnd = nullptr;
    if (control)
        DestroyWindow(control);
    if (controlClassRegistered)
        UnregisterClassW(kInputControlClass, g_hModule);
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
    InitHotkeyMessage();

    // 1) UI thread: crea el selector y precalienta el renderer.
    DWORD uiThreadId = 0;
    if (!StartWorkerThread(UiThreadProc, &g_uiThread, &uiThreadId,
                           &g_uiReadyEvent, &g_uiInitOk))
        return FALSE;
    g_uiThreadId = uiThreadId;

    // 2) Input thread: crea la ventana de control y el polling. Se publica
    //    cuando el selector ya existe.
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
// EXPLORER (solo en explorer.exe)
//
// El selector nativo lo abre un hotkey que explorer registra para sí mismo.
// El selector nativo registra el hotkey de Alt+Tab en el shell. Este componente
// observa la cola del hilo "Immersive Shell" de Explorer y reenvía el hotkey al
// switcher en lugar de abrir el selector nativo. Explorer mantiene la ruta de
// Alt+Tab cuando el foreground es una ventana elevada; el selector procesa sus
// propias teclas una vez toma el foco. Esta es la única parte que corre en
// Explorer: no crea ventanas ni instala hooks globales; solo usa un hook de
// hilo WH_GETMESSAGE.

static const WCHAR kAltTabThreadName[] = L"Immersive Shell";
static const DWORD kFindAltTabThreadIntervalMs = 2000;
static const DWORD kFindAltTabThreadMaxIntervalMs = 60000;
static const DWORD kWaitForTaskbarIntervalMs = 1000;

// Hilo de explorer (hook) y su thread de vigilancia.
static HHOOK g_messageHook = nullptr;
static HANDLE g_messageHookThread = nullptr;    // para saber cuándo termina
static volatile LONG g_messageHookCalls = 0;
static HANDLE g_explorerStopEvent = nullptr;
static HANDLE g_explorerThread = nullptr;
static bool g_isExplorer = false;

static void InitHotkeyMessage()
{
    if (!g_hotkeyMessage)
        g_hotkeyMessage = RegisterWindowMessageW(kHotkeyMessageName);
}

// Entrega el hotkey a Overhaulded. Devuelve false si la herramienta no está
// corriendo o no puede aceptar una sesión ahora; entonces abre el selector
// nativo (comportamiento seguro).
static bool ForwardHotkey(WPARAM flags)
{
    if (!g_hotkeyMessage)
        return false;

    HWND control = FindWindowExW(HWND_MESSAGE, nullptr, kInputControlClass,
                                 kInputControlTitleReady);
    if (!control)
        return false;

    // Explorer acaba de recibir el hotkey, así que puede ceder el foreground.
    // El selector lo necesita para tomar el foco si una ventana elevada lo tenía.
    DWORD processId = 0;
    GetWindowThreadProcessId(control, &processId);
    if (processId && !AllowSetForegroundWindow(processId))
        Wh_Log(L"Explorer could not grant foreground permission to process %lu",
               processId);

    return PostMessageW(control, g_hotkeyMessage, flags, 0) != FALSE;
}

static LRESULT CALLBACK GetMessageProc(int code, WPARAM wParam, LPARAM lParam)
{
    InterlockedIncrement(&g_messageHookCalls);

    MSG* msg = reinterpret_cast<MSG*>(lParam);
    if (code == HC_ACTION && wParam == PM_REMOVE && msg &&
        msg->message == WM_HOTKEY && HIWORD(msg->lParam) == VK_TAB)
    {
        const UINT modifiers = LOWORD(msg->lParam);
        // El mismo criterio que el hook: cualquier Tab con Alt (Shift invierte,
        // Ctrl se reenvía para que se resuelva AltGr con el estado real). Win+Tab
        // no es de Overhaulded.
        if ((modifiers & MOD_ALT) != 0 && (modifiers & MOD_WIN) == 0)
        {
            WPARAM flags = 0;
            if (modifiers & MOD_SHIFT)
                flags |= kHotkeyFlagBackwards;
            if (modifiers & MOD_CONTROL)
                flags |= kHotkeyFlagCtrl;
            if (ForwardHotkey(flags))
            {
                // Explorer recibe un mensaje vacío en lugar del hotkey, solo
                // tras confirmar que la entrega se realizó.
                msg->message = WM_NULL;
            }
        }
    }

    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    InterlockedDecrement(&g_messageHookCalls);
    return result;
}

// ID del hilo de explorer que recibe el hotkey, o 0 si ahora no existe.
static DWORD FindAltTabThread()
{
    DWORD found = 0;
    const DWORD processId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return found;

    THREADENTRY32 entry = {};
    entry.dwSize = sizeof(entry);
    for (BOOL more = Thread32First(snapshot, &entry); more && !found;
         more = Thread32Next(snapshot, &entry))
    {
        if (entry.th32OwnerProcessID != processId)
            continue;
        HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE,
                                   entry.th32ThreadID);
        if (!thread)
            continue;
        PWSTR name = nullptr;
        if (SUCCEEDED(GetThreadDescription(thread, &name)) && name &&
            wcscmp(name, kAltTabThreadName) == 0)
        {
            found = entry.th32ThreadID;
        }
        if (name)
            LocalFree(name);
        CloseHandle(thread);
    }

    CloseHandle(snapshot);
    return found;
}

// Engancha el hilo que recibe el hotkey, si existe ahora.
static bool HookAltTabThread()
{
    const DWORD threadId = FindAltTabThread();
    if (!threadId)
        return false;
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread)
        return false;
    // Hilo del mismo proceso: sin módulo (documentación de SetWindowsHookEx).
    HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, GetMessageProc, nullptr, threadId);
    if (!hook)
    {
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %lu", GetLastError());
        CloseHandle(thread);
        return false;
    }
    Wh_Log(L"Hooked explorer thread %lu (%s)", threadId, kAltTabThreadName);
    g_messageHook = hook;
    g_messageHookThread = thread;
    return true;
}

static void UnhookAltTabThread()
{
    if (g_messageHook)
    {
        UnhookWindowsHookEx(g_messageHook);
        g_messageHook = nullptr;
    }
    if (g_messageHookThread)
    {
        CloseHandle(g_messageHookThread);
        g_messageHookThread = nullptr;
    }

    // Las llamadas ya en curso en el hilo de Explorer deben salir del módulo
    // antes de descargarlo. Tras retirar el hook no pueden empezar otras; el
    // callback solo hace trabajo no bloqueante, por lo que esperamos a que
    // finalicen todas antes de permitir la descarga.
    while (InterlockedCompareExchange(&g_messageHookCalls, 0, 0) > 0)
        Sleep(1);
}

static DWORD WINAPI ExplorerThreadProc(LPVOID)
{
    // Solo el proceso de shell (el de la barra de tareas) recibe el hotkey. Con
    // "carpetas en proceso separado" otros explorer.exe no tienen nada que hacer.
    for (;;)
    {
        if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr))
        {
            DWORD processId = 0;
            GetWindowThreadProcessId(taskbar, &processId);
            if (processId != GetCurrentProcessId())
            {
                Wh_Log(L"Not the shell process, nothing to do");
                return 0;
            }
            break;
        }
        // Explorer aún está arrancando.
        if (WaitForSingleObject(g_explorerStopEvent, kWaitForTaskbarIntervalMs) != WAIT_TIMEOUT)
            return 0;
    }

    // Engancha el hilo y vuelve a hacerlo si se recrea (reinicio de shell). Hasta
    // que existe, se busca cada vez con menos frecuencia.
    DWORD interval = kFindAltTabThreadIntervalMs;
    bool loggedMissing = false;
    for (;;)
    {
        if (!g_messageHook && !HookAltTabThread() && !loggedMissing)
        {
            Wh_Log(L"Explorer thread \"%s\" not found yet", kAltTabThreadName);
            loggedMissing = true;
        }

        const HANDLE events[] = { g_explorerStopEvent, g_messageHookThread };
        const DWORD result = WaitForMultipleObjects(
            g_messageHook ? 2 : 1, events, FALSE,
            g_messageHook ? INFINITE : interval);
        if (result == WAIT_TIMEOUT)
        {
            interval = std::min(interval * 2, kFindAltTabThreadMaxIntervalMs);
        }
        else if (result == WAIT_OBJECT_0 + 1)
        {
            // El hilo terminó (su ID puede reutilizarse).
            UnhookAltTabThread();
            interval = kFindAltTabThreadIntervalMs;
            loggedMissing = false;
        }
        else
        {
            break;
        }
    }

    UnhookAltTabThread();
    return 0;
}

static void StopExplorerPart()
{
    if (g_explorerThread)
    {
        SetEvent(g_explorerStopEvent);
        WaitForSingleObject(g_explorerThread, INFINITE);
        CloseHandle(g_explorerThread);
        g_explorerThread = nullptr;
    }
    if (g_explorerStopEvent)
    {
        CloseHandle(g_explorerStopEvent);
        g_explorerStopEvent = nullptr;
    }
}

// No espera nada, para no retrasar el arranque de explorer.
static void StartExplorerPart()
{
    if (g_explorerThread)
        return;  // nunca hilos de vigilancia duplicados

    g_explorerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_explorerStopEvent)
        return;
    g_explorerThread = CreateThread(nullptr, 0, ExplorerThreadProc, nullptr, 0, nullptr);
    if (!g_explorerThread)
    {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        StopExplorerPart();
    }
}

static bool IsExplorerProcess()
{
    WCHAR path[MAX_PATH];
    const DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path))
        return false;
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

static BOOL ExplorerModInit()
{
    InitHotkeyMessage();
    StartExplorerPart();
    return TRUE;
}

static void ExplorerModUninit()
{
    StopExplorerPart();
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
    // explorer.exe: solo el componente que reenvía el hotkey. No se crea
    // selector, renderer ni hooks globales, y no se lanza el tool mod.
    if (IsExplorerProcess()) {
        g_isExplorer = true;
        return ExplorerModInit();
    }

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
    // El componente de explorer no usa ajustes: no se reinicia nada.
    if (g_isExplorer) {
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        ExplorerModUninit();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
