// ==WindhawkMod==
// @id            taskbar-media-player
// @name          Taskbar Media Player
// @description   A sleek, floating media player on your taskbar with native volume and playback controls.
// @version       6.6.0
// @author        GR0UD
// @github        https://github.com/GR0UD
// @license       MIT
// @include       explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -lksuser
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## 💎 Taskbar Media Player
A sleek, floating media player that lives right on your Windows 11 taskbar — no extra windows, no clutter. Album art, scrolling track info, playback controls, a live audio visualizer, and smooth volume control all in one tight panel.

Got a feature idea or ran into something weird?
Hit me up on Discord at **@fkwr** — suggestions are always welcome.

---

## ⚠️ Requirements
- Windows 11
- Taskbar widgets panel turned **off** (right-click taskbar → Taskbar settings → Widgets: Off)

---

## ✨ Features

### 🎨 Themes & Appearance
- **12 background themes** — Transparent, Acrylic Glass, Windows Adaptive, Neon Glass, Sharp Split, Aurora Glow, Album Art Blur, and six gradient modes (fade, left→right, right→left, top→bottom, bottom→top, radial glow)
- **Album art adaptive colors** — background gradients pull their palette straight from the current cover art
- **Auto text color** — automatically switches between white and black text based on your system light/dark mode
- **Dynamic hover color** — button highlights can match the album art's primary color
- **Rounded corners** with adjustable radius
- **Optional border**, mask overlay, and per-container debug outlines

### 📐 Layout & Sizing
- **Fully dynamic layout** — disable any container and the others expand to fill the space automatically
- **Container order control** — rearrange Media, Info, Controls, and Visualizer in any order with a 4-digit code (e.g. `4213`)
- **Fixed or flexible Info width** — lock the title/artist area to a set pixel width or let it grow freely
- **Per-container padding** — independent left/right padding on every container plus top/bottom padding on the main wrapper
- **Global container gap** — uniform spacing between all active containers
- **Panel height, width, and position offset** fully configurable
- **Mini Mode** — compact view showing only Album Art and/or Visualizer; great for small taskbars

### 🎵 Media Info
- **Scrolling title & artist** — text smoothly loops when it overflows the container (3 speed settings)
- **Truncation fallback** — adds an ellipsis when scrolling is disabled and text overflows
- **Album art** with configurable corner radius and a music-note placeholder when no art is available
- **Supports 16 media sources**: Spotify, Apple Music, YouTube Music, Chrome, Edge, Firefox, Opera, Brave, Discord, VLC, Foobar2000, MusicBee, iTunes, Tidal, Amazon Music, or auto-detect all

### ⏯️ Playback Controls
- **Previous / Play-Pause / Next** buttons
- **Shuffle** and **Repeat** toggle buttons (optional)
- **4 icon themes**: Custom drawn, Segoe MDL2 Assets, Segoe Fluent Icons Outlined, Segoe Fluent Icons Filled, Segoe Fluent Icons Outlined Alt
- **Configurable icon size** and **button spacing**

### 🔊 Volume
- **Speaker icon** with a hover-to-expand inline volume slider — click and drag or scroll to set level
- **Mouse wheel volume control** — scroll anywhere on the panel to raise or lower volume, no icon needed
- **Per-app volume** — controls the volume of the playing media app only, not the system master

### 📊 Live Audio Visualizer
- **5 bar shapes**: Stereo, Mountain, Mirror, Wave, Breathe
- **5 color modes**: Solid, Dynamic Album Color, Dynamic Gradient, Custom Gradient, Acrylic
- **6 EQ presets**: Balanced, Bass, Rock, Pop, Jazz, Electronic
- **3 vertical anchors**: Top, Middle, Bottom
- **Configurable bar count** (1–20), **bar width**, **bar gap**, and **idle bar height**
- **Full-width background mode** — renders the visualizer behind all other containers as a full-panel backdrop
- **Sensitivity control** (0–100)

### 📏 Progress Bar
- **5 position modes**: Below Everything, Above Everything, Under Info & Controls, Under Info Only, Under Controls Only
- **Dynamic color** (matches album art) or custom hex color
- **Configurable height** (1–6 px), padding, and vertical offset
- **Smooth live interpolation** — position updates every frame between GSMTC polls so the bar never jumps

### 🙈 Behavior
- **Auto-hide on fullscreen** — panel hides when a fullscreen app or game is detected, with optional fade animation
- **Idle timeout** — hide the panel after N seconds of no media
- **Idle screen mode** — shows placeholder content instead of disappearing entirely
- Clicking the art/title area **focuses the media app's window**

---

## 📐 Default Layout
```
[ Album Art ]  [ Song Title / Artist ]  [ ⏮️  ▶️  ⏭️ ]  [ Visualizer ]
```
All sections are optional and reorderable. The Info container expands into any freed space automatically.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- AppearanceGroup:
  - Theme: transparent
    $name: Theme
    $options:
      - transparent: Transparent
      - blurred: Acrylic Glass
      - windows: Windows Adaptive
      - neon: Neon Glass
      - gradient: "Gradient: Fade to Transparent"
      - gradient_lr: "Gradient: Left to Right"
      - gradient_rl: "Gradient: Right to Left"
      - gradient_vertical: "Gradient: Top to Bottom"
      - gradient_vertical_inv: "Gradient: Bottom to Top"
      - gradient_radial: "Gradient: Radial Glow"
      - split: Sharp Split
      - aurora: Aurora Glow
      - album_blur: Album Art Blur
  - ThemeOpacity: 200
    $name: Opacity (0-255)
  - ThemeBlur: 60
    $name: Blur Strength (0-100)
  - EnableMask: false
    $name: Mask Overlay
  - MaskOpacity: 120
    $name: Mask Opacity (0-255)
  - RoundedCorners: false
    $name: Rounded Corners
  - CornerRadius: 10
    $name: Corner Radius (2-32)
  - ShowBorder: false
    $name: Show Border
  - ShowContainerBorders: false
    $name: Show Container Borders (Debug)
    $description: Draws a colored outline around each active container so you can see exact boundaries and spacing.
  $name: Appearance

- LayoutGroup:
  - PanelHeight: 51
    $name: Height (px)
  - PanelWidth: 360
    $name: Width (px)
    $description: Total panel width. Only applies when Info container is in fixed width mode.
  - OffsetX: 0
    $name: Horizontal Offset
  - OffsetY: 0
    $name: Vertical Offset
  - WrapPadTop: 7
    $name: Padding Top
  - WrapPadBottom: 7
    $name: Padding Bottom
  - WrapPadLeft: 12
    $name: Padding Left
  - WrapPadRight: 12
    $name: Padding Right
  - GlobalGap: 12
    $name: Container Gap (px)
    $description: Gap between each active container.
  - ContainerOrder: 1234
    $name: Container Order
    $description: "Four-digit code sets left-to-right order. 1=Media 2=Info 3=Controls 4=Visualizer. Example: 4123 puts Visualizer first."
  $name: Layout

- BehaviorGroup:
  - FocusedApp: all
    $name: Media Source
    $options:
      - all: All Sources (Auto)
      - spotify: Spotify
      - applemusic: Apple Music
      - ytmusic: YouTube Music
      - chrome: Google Chrome
      - msedge: Microsoft Edge
      - firefox: Firefox
      - opera: Opera
      - brave: Brave
      - discord: Discord
      - vlc: VLC
      - foobar2000: Foobar2000
      - musicbee: MusicBee
      - itunes: iTunes
      - tidal: Tidal
      - amazonmusic: Amazon Music
  - HideFullscreen: true
    $name: Hide on Fullscreen
  - FadeFullscreen: false
    $name: Fade on Fullscreen Hide
  - DisableScroll: false
    $name: Disable Title Scrolling
  - IdleScreenEnabled: false
    $name: Idle Screen
  - IdleTimeout: 0
    $name: Idle Timeout (seconds)
    $description: Seconds of no media before hiding. 0 = never.
  - IdleScreenDelay: 5
    $name: Idle Screen Delay (seconds)
  $name: Behavior

- MiniModeGroup:
  - MiniMode: false
    $name: Enable Mini Mode
    $description: "Compact mode — only Album Art and/or Visualizer are shown. Padding and theme follow the main Layout and Appearance settings."
  - MiniShowMedia: true
    $name: Show Album Art
  - MiniShowVisualizer: true
    $name: Show Visualizer
    $description: Requires Visualizer to be enabled in the Visualizer section.
  $name: Mini Mode

- MediaContainerGroup:
  - MediaEnabled: true
    $name: Enabled
  - MediaPadLeft: 0
    $name: Padding Left
  - MediaPadRight: 0
    $name: Padding Right
  - MediaWidth: 0
    $name: Width (px)
    $description: "Custom width for the art area. 0 = auto (square, or matches art aspect ratio when Auto-Fit is on). Set a fixed value to override."
  - MediaAutoSize: true
    $name: Auto-Fit Art Shape
    $description: "Automatically resizes the art area to match the image's native aspect ratio — square for Spotify/Apple Music, wider for YouTube thumbnails, etc. Uses the manual Width above as a cap when set."
  - MediaCornerRadius: 6
    $name: Album Art Corners (0-32)
    $description: Corner radius applied to the album art. 0 = square.
  - MediaPlaceholderIcon: true
    $name: Show Placeholder Icon
    $description: Shows a music note icon when no album art is available.
  $name: Media

- InfoContainerGroup:
  - InfoEnabled: true
    $name: Enabled
  - InfoFixedWidth: true
    $name: Fixed Width
    $description: When on, Info uses exactly the set pixel width. When off it expands to fill remaining space.
  - InfoWidth: 160
    $name: Fixed Width (px)
  - InfoPadLeft: 0
    $name: Padding Left
  - InfoPadRight: 0
    $name: Padding Right
  - ArtTextGap: 12
    $name: Gap — Art to Text (px)
    $description: Horizontal gap between the album art and the start of the text.
  - FontSize: 11
    $name: Font Size (px)
  - TextGap: 2
    $name: Gap — Title to Artist (px)
  - ScrollSpeed: 1
    $name: Scroll Speed
    $options:
      - 0: Slow
      - 1: Normal
      - 2: Fast
  - AutoTheme: true
    $name: Auto Text Color
    $description: Automatically picks white or black text based on system theme.
  - TextColor: FFFFFF
    $name: Text Color
    $description: Used when Auto Text Color is off.
  $name: Info

- ControlsContainerGroup:
  - ControlsEnabled: true
    $name: Enabled
  - CtrlPadLeft: 0
    $name: Padding Left
  - CtrlPadRight: 0
    $name: Padding Right
  - ShowPlaybackControls: true
    $name: Show Playback Controls
    $description: Shows Previous, Play/Pause and Next buttons.
  - ShowShuffleButton: false
    $name: Show Shuffle Button
  - ShowRepeatButton: false
    $name: Show Repeat Button
  - ShowSpeakerIcon: false
    $name: Show Volume Control
    $description: Speaker icon with hover-to-expand volume slider.
  - ScrollVolume: true
    $name: Mouse Wheel Controls Volume
    $description: "Scroll anywhere on the panel to adjust volume. You can hover over the panel and scroll up/down to turn the volume up or down — no need to click anything."
  - IconTheme: mdl2
    $name: Icon Theme
    $options:
      - default: Default (Custom Drawn)
      - mdl2: Segoe MDL2 Assets
      - fluent: Segoe Fluent Icons - Outlined
      - fluent_filled: Segoe Fluent Icons - Filled
      - fluent_outline2: Segoe Fluent Icons - Outlined Alt
  - IconSize: 14
    $name: Icon Size (px)
  - ButtonSpacing: 34
    $name: Button Spacing (px)
  - CtrlIconDynamic: false
    $name: Dynamic Icon Color
    $description: Uses the album art primary color for the default icon color.
  - CtrlIconColor: FFFFFF
    $name: Icon Color
    $description: Used when Dynamic Icon Color is off.
  - HoverColor: 1ED760
    $name: Button Hover Color
  - DynamicHover: false
    $name: Dynamic Hover Color
    $description: Uses the album art primary color for button hover states.
  - TextControlGap: 0
    $name: Gap — Info to Controls (px)
  $name: Controls

- ProgressBarGroup:
  - ShowProgressBar: true
    $name: Enabled
  - ProgressBarLayer: under_both
    $name: Position
    $options:
      - under_both: Below Everything
      - above_both: Above Everything
      - under_text: Under Info and Controls
      - under_text_only: Under Info Only
      - under_controls_only: Under Controls Only
  - ProgressBarHeight: 2
    $name: Height (1-6 px)
  - ProgressBarDynamic: true
    $name: Dynamic Color
    $description: Uses album art primary color.
  - ProgressBarColor: 1ED760
    $name: Custom Color
  - ProgressBarPadLeft: 12
    $name: Padding Left
  - ProgressBarPadRight: 0
    $name: Padding Right
  - ProgressBarOffsetY: 0
    $name: Vertical Offset (px)
  $name: Progress Bar

- VisualizerContainerGroup:
  - VizEnabled: true
    $name: Enabled
  - VizAsBackground: false
    $name: Full-Width Background Mode
    $description: Renders behind all containers with no width slot. Spans the full panel width.
  - VizWidth: 0
    $name: Container Width (px)
    $description: "0 = auto-size from bar count and spacing."
  - VizPadLeft: 0
    $name: Padding Left
  - VizPadRight: 0
    $name: Padding Right
  - VizShape: stereo
    $name: Bar Shape
    $options:
      - stereo: Stereo
      - mountain: Mountain
      - mirror: Mirror
      - wave: Wave
      - breathe: Breathe
  - VizAnchor: middle
    $name: Vertical Anchor
    $options:
      - top: Top
      - middle: Middle
      - bottom: Bottom
  - VizMode: solid
    $name: Color Mode
    $options:
      - solid: Solid
      - dynamic_album: Dynamic Album Color
      - dynamic_gradient: Dynamic Gradient
      - custom_gradient: Custom Gradient
      - acrylic: Acrylic
  - VizColor: FFFFFF
    $name: Bar Color
    $description: Used in Solid mode.
  - VizColor1: 1ED760
    $name: Gradient Color 1
  - VizColor2: 00B4FF
    $name: Gradient Color 2
  - VizBars: 7
    $name: Bar Count (1-20)
  - VizBarWidth: 4
    $name: Bar Width (px)
    $description: "0 = auto-size to fill. Non-zero = fixed px per bar."
  - VizBarGap: 5
    $name: Bar Gap (px)
  - IdleBarSize: 10
    $name: Idle Bar Height (%)
    $description: Minimum bar height when no audio is playing.
  - VizSensitivity: 100
    $name: Sensitivity (0-100)
  - VizEQ: default
    $name: EQ Preset
    $options:
      - default: Balanced
      - bass: Bass
      - rock: Rock
      - pop: Pop
      - jazz: Jazz
      - electronic: Electronic
  $name: Visualizer

*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <algorithm>

#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <cmath>
#include <vector>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Media;
using namespace Windows::Storage::Streams;

const WCHAR* FONT_NAME = L"Segoe UI Variable Display";
const WCHAR* ICON_FONT = L"Segoe MDL2 Assets";

// ── Shared Layout Math ───────────────────────────────────────────────────────
// Extended to carry shuffle/repeat positions and support dynamic layout.
// Positions are computed right-to-left from padRight edge.
// Buttons that are disabled are skipped - the text area then expands to fill the gap.
struct ControlLayout {
    float nX, ppX, pX, vX, cY;      // playback: next, play/pause, prev, volume
    float shuffleX, repeatX;         // extra buttons (0 = not shown)
    float firstControlX;             // leftmost active button x (for text clipping)
};

inline ControlLayout CalcLayout(int W, int H, int padRight, int buttonSpacing = 34,
    bool showSpeaker = true, bool showPlayback = true,
    bool showShuffle = false, bool showRepeat = false,
    int padTop = 0, int padBottom = 0, int padLeft = 0,
    int ctrlPadLeft = 0, int ctrlPadRight = 0,
    int ctrlCellRight = -1)   // right edge of the controls cell (ctrlCellX + containerWidth)
{
    // Vertical center within the wrapper's padded content area
    float cY    = padTop + (H - padTop - padBottom) / 2.f;
    float scale = H / 52.f;
    float step  = (float)buttonSpacing * scale;

    // Count active buttons
    int btnCount = 0;
    if (showPlayback) btnCount += 3; // prev, play/pause, next
    if (showSpeaker)  btnCount += 1;
    if (showRepeat)   btnCount += 1;
    if (showShuffle)  btnCount += 1;

    float nX = 0, ppX = 0, pX = 0, vX = 0, shuffleX = 0, repeatX = 0;
    float firstX = (float)(W - padRight); // fallback if no buttons

    if (btnCount > 0) {
        float totalSpan = step * (btnCount - 1);

        // Content area bounds, respecting container inner padding
        float cellLeft  = (float)(padLeft + ctrlPadLeft);
        float cellRight = (ctrlCellRight >= 0)
            ? (float)ctrlCellRight - (float)ctrlPadRight
            : (float)(W - padRight) - (float)ctrlPadRight;
        float contentW = cellRight - cellLeft;

        // Expand step so buttons fill the container edge-to-edge.
        // Treat the container as btnCount equal slots; each button centre sits
        // at the middle of its slot, so effective step = contentW / btnCount
        // and there is a natural half-slot margin on each side.
        float filledStep  = contentW / (float)btnCount;
        float filledStart = cellLeft + filledStep * 0.5f; // first button centre

        float cursor = filledStart + filledStep * (btnCount - 1); // rightmost first
        if (showPlayback) {
            nX   = cursor; cursor -= filledStep;
            ppX  = cursor; cursor -= filledStep;
            pX   = cursor; cursor -= filledStep;
        }
        if (showSpeaker) {
            vX = cursor; cursor -= filledStep;
        }
        if (showRepeat) {
            repeatX = cursor; cursor -= filledStep;
        }
        if (showShuffle) {
            shuffleX = cursor; cursor -= filledStep;
        }

        firstX = filledStart;
    }

    return { nX, ppX, pX, vX, cY, shuffleX, repeatX, firstX };
}

// ── DWM / Accent ─────────────────────────────────────────────────────────────
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTBACKDROP = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
} ACCENT_STATE;
typedef struct { ACCENT_STATE AccentState; DWORD AccentFlags; DWORD GradientColor; DWORD AnimationId; } ACCENT_POLICY;
typedef struct { WINDOWCOMPOSITIONATTRIB Attribute; PVOID Data; SIZE_T SizeOfData; } WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// ── Z-Band ───────────────────────────────────────────────────────────────────
enum ZBID { ZBID_IMMERSIVE_NOTIFICATION = 4 };
typedef HWND(WINAPI* pCreateWindowInBand)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID,DWORD);

// ── Settings ─────────────────────────────────────────────────────────────────
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif
#ifndef DWMWA_COLOR_DEFAULT
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
#endif

struct Container {
    int  id;               // 1=Media  2=Info  3=Controls  4=Visualizer
    bool enabled;
    int  padLeft;
    int  padRight;
    int  padTop;
    int  padBottom;
    int  calculatedWidth;  // filled by GetContainerWidth() at layout time
};

struct LayoutWrapper {
    int padTop, padBottom, padLeft, padRight;
    int globalGap;
    std::vector<int> order;   // render sequence, e.g. {1, 2, 4, 3}
    // Visualizer z-layer: false = sequential container, true = background layer
    bool vizAsBackground;
    // Info container fixed-width toggle
    bool infoFixedWidth;
    int  infoWidth;           // used when infoFixedWidth == true
};

struct ModSettings {
    // ── Wrapper (replaces scattered padTop/padBottom on every component) ────
    LayoutWrapper wrapper = {
        /*padTop*/7, /*padBottom*/7, /*padLeft*/12, /*padRight*/12,
        /*globalGap*/12,
        /*order*/{1, 2, 3},       // default: Media → Info → Controls
        /*vizAsBackground*/false,
        /*infoFixedWidth*/true,
        /*infoWidth*/160
    };

    // ── Containers ──────────────────────────────────────────────────────────
    // (calculatedWidth is computed at runtime - not a user setting)
    Container containers[4] = {
        {1, true,  0, 0, 0, 0, 0},   // Media / Album Art
        {2, true,  0, 0, 0, 0, 0},   // Info  (title + artist + progress)
        {3, true,  0, 0, 0, 0, 0},   // Controls (buttons)
        {4, true,  0, 0, 0, 0, 0},   // Visualizer
    };

    // ── General ─────────────────────────────────────────────────────────────
    int   height        = 51;
    int   fontSize      = 11;
    int   textGap       = 2;
    int   artTextGap    = 12;
    int   offsetX       = 0;
    int   offsetY       = 0;
    int   background    = 1;   // transparent
    bool  roundedCorners = false;
    int   cornerRadius  = 10;
    int   mediaCornerRadius = 6;
    int   mediaWidth        = 0;     // 0 = auto square; >0 = explicit px width
    bool  mediaAutoSize     = true;  // auto-detect art aspect ratio and resize accordingly
    bool  showBorder    = false;
    bool  showContainerBorders = false;
    bool  autoTheme     = true;
    bool  dynamicHover  = false;
    DWORD manualText    = 0xFFFFFFFF;
    DWORD hoverColor    = 0xFF1ED760;
    int   themeOpacity  = 200;
    int   themeBlur     = 60;
    bool  showSpeakerIcon = false;
    bool  hideFullscreen = true;
    int   idleTimeout   = 0;
    bool  idleScreenEnabled = false;
    int   idleScreenDelay   = 5;
    bool  showVisualizer    = true;
    int   vizEQ             = 0;
    int   vizShape          = 0;
    int   vizMode           = 0;
    int   vizAnchor         = 1;
    int   idleBarSize       = 10;
    int   vizSensitivity    = 100;
    DWORD vizColor          = 0xFFFFFFFF;
    DWORD vizColor1         = 0xFF1ED760;
    DWORD vizColor2         = 0xFF00B4FF;
    int   vizBarGap         = 5;
    int   vizBars           = 7;
    int   vizBarWidth       = 4;
    int   iconSize          = 14;
    int   buttonSpacing     = 34;
    std::wstring focusedApp = L"";
    int   iconTheme         = 1;   // mdl2
    bool  enableMask        = false;
    int   maskOpacity       = 120;
    Bitmap* blurredBg       = nullptr;
    int     blurBgW         = 0;
    int     blurBgH         = 0;
    int     blurArtVersion  = -1;
    bool    isDarkCover     = true;
    bool    miniMode           = false;
    bool    miniShowMedia      = true;
    bool    miniShowVisualizer = true;
    bool    mediaEnabled       = true;
    bool    infoEnabled        = true;
    bool    controlsEnabled    = true;
    int     scrollSpeed        = 1;
    int     vizContainerWidth  = 0;
    bool    scrollVolume       = true;
    bool    fadeFullscreen  = false;
    bool    disableScroll   = false;
    int     textControlGap  = 0;
    bool    showPlaybackControls = true;
    bool    showShuffleButton    = false;
    bool    showRepeatButton     = false;
    bool    ctrlIconDynamic      = false;
    DWORD   ctrlIconColor        = 0xFFFFFFFF;
    bool    showProgressBar      = true;
    int     progressBarLayer     = 0;
    int     progressBarHeight    = 2;
    bool    progressBarDynamic   = true;
    DWORD   progressBarColor     = 0xFF1ED760;
    int     progressBarPadLeft   = 12;
    int     progressBarPadRight  = 0;
    int     progressBarOffsetY   = 0;

    // ── Derived / computed at LoadSettings ──────────────────────────────────
    // Total panel width - computed from wrapper + active containers
    int width = 360;
} g_S;

static int CalcMediaWidth();
static int CalcInfoWidth();
static int CalcControlsWidth();
static int CalcVisualizerWidth();
static float g_ArtAspectRatio = 1.0f;

static int CalcMediaWidth() {
    int artS = g_S.height - g_S.wrapper.padTop - g_S.wrapper.padBottom;
    if (artS < 8) artS = 8;
    int artW;
    if (g_S.mediaWidth > 0) {
        artW = g_S.mediaWidth;
    } else if (g_S.mediaAutoSize) {
        float ratio = max(0.25f, min(3.0f, g_ArtAspectRatio));
        artW = max(8, (int)roundf(artS * ratio));
    } else {
        artW = artS;
    }
    return g_S.containers[0].padLeft + artW + g_S.containers[0].padRight;
}

static int CalcInfoWidth() {
    if (g_S.wrapper.infoFixedWidth) {
        return g_S.containers[1].padLeft + g_S.wrapper.infoWidth + g_S.containers[1].padRight;
    }
    // Flexible: will be set during UpdateBoundsAndRegion after all other widths are known.
    // Returning 0 here signals "fill remaining space."
    return 0;
}

static int CalcControlsWidth() {
    int btnCount = 0;
    if (g_S.showPlaybackControls) btnCount += 3;
    if (g_S.showSpeakerIcon)      btnCount += 1;
    if (g_S.showShuffleButton)    btnCount += 1;
    if (g_S.showRepeatButton)     btnCount += 1;
    if (btnCount == 0) return 0;
    float scale = g_S.height / 52.f;
    float step  = g_S.buttonSpacing * scale;
    // Span = (btnCount-1) steps between centres, plus half-step margin on each side
    int w = (int)((btnCount - 1) * step + step);  // == btnCount * step, one step = left+right margins
    return g_S.containers[2].padLeft + w + g_S.containers[2].padRight;
}

static int CalcVisualizerWidth() {
    // If the user set a manual VizWidth container override, honour it directly.
    if (g_S.vizContainerWidth > 0)
        return g_S.containers[3].padLeft + g_S.vizContainerWidth + g_S.containers[3].padRight;

    // Determine per-bar pixel width.
    // VizBarWidth == 0 means "auto" — use a sensible minimum of 4 px so the
    // container doesn't collapse to nothing when bar count is low.
    int barCount = max(1, g_S.vizBars);
    int barW     = (g_S.vizBarWidth > 0) ? g_S.vizBarWidth : 4;
    int gap      = max(0, g_S.vizBarGap);

    // Total content width: bars + gaps between them
    int contentW = barW * barCount + gap * (barCount - 1);

    return g_S.containers[3].padLeft + contentW + g_S.containers[3].padRight;
}

// Returns the effective (possibly clamped) width for container `id`.
static int GetContainerWidth(int id) {
    for (auto& c : g_S.containers)
        if (c.id == id) return c.calculatedWidth;
    return 0;
}

static bool IsContainerEnabled(int id) {
    for (auto& c : g_S.containers)
        if (c.id == id) return c.enabled;
    return false;
}

// Recompute all container widths and total panel width.
// Info (id=2) uses whatever space is left after the others.
// Call this at the end of LoadSettings().
static void RecomputeLayout() {
    const bool miniMode = g_S.miniMode;

    if (miniMode) {
        // Mini: only Media and/or Visualizer visible; Info & Controls always hidden.
        bool showMedia = g_S.miniShowMedia && g_S.mediaEnabled;
        bool showViz   = g_S.miniShowVisualizer && g_S.showVisualizer;

        // If neither is enabled, fall back to showing media
        if (!showMedia && !showViz) showMedia = true;

        g_S.containers[0].enabled = showMedia;
        g_S.containers[1].enabled = false;   // Info
        g_S.containers[2].enabled = false;   // Controls
        g_S.containers[3].enabled = showViz;

        // Art size uses same wrapper top/bottom padding as normal mode
        int artS = g_S.height - g_S.wrapper.padTop - g_S.wrapper.padBottom;
        if (artS < 8) artS = 8;
        int artW;
        if (g_S.mediaWidth > 0) {
            artW = g_S.mediaWidth;
        } else if (g_S.mediaAutoSize) {
            float ratio = max(0.25f, min(3.0f, g_ArtAspectRatio));
            artW = max(8, (int)roundf(artS * ratio));
        } else {
            artW = artS;
        }

        // Each active container uses its natural width
        g_S.containers[0].calculatedWidth = showMedia ? artW : 0;
        g_S.containers[3].calculatedWidth = showViz   ? CalcVisualizerWidth() : 0;

        // Total width: wrapper left+right + active container widths + gap between them if both shown
        int total = g_S.wrapper.padLeft + g_S.wrapper.padRight;
        int activeCount = (showMedia ? 1 : 0) + (showViz ? 1 : 0);
        if (showMedia) total += g_S.containers[0].calculatedWidth;
        if (showViz)   total += g_S.containers[3].calculatedWidth;
        if (activeCount > 1) total += g_S.wrapper.globalGap;

        g_S.width = total;
        return;
    }

    // Normal mode:
    g_S.containers[0].enabled = g_S.mediaEnabled;
    g_S.containers[1].enabled = g_S.infoEnabled;
    g_S.containers[2].enabled = g_S.controlsEnabled &&
                                (g_S.showPlaybackControls ||
                                 g_S.showSpeakerIcon      ||
                                 g_S.showShuffleButton    ||
                                 g_S.showRepeatButton);
    g_S.containers[3].enabled = g_S.showVisualizer;

    // Compute fixed-width containers first
    g_S.containers[0].calculatedWidth = CalcMediaWidth();
    g_S.containers[2].calculatedWidth = CalcControlsWidth();
    g_S.containers[3].calculatedWidth = CalcVisualizerWidth();

    // Count active non-info containers + gaps
    int fixedTotal = 0;
    int activeCount = 0;
    for (int id : g_S.wrapper.order) {
        if (id == 2) continue; // Info handled separately
        if (!IsContainerEnabled(id))       continue;
        if (id == 4 && g_S.wrapper.vizAsBackground) continue; // bg layer, no width
        fixedTotal += GetContainerWidth(id);
        activeCount++;
    }
    // Include Info in active count for gap calculation
    if (IsContainerEnabled(2)) activeCount++;
    int totalGaps = (activeCount > 1) ? (activeCount - 1) * g_S.wrapper.globalGap : 0;

    // Info gets remaining space
    int remaining = g_S.width - g_S.wrapper.padLeft - g_S.wrapper.padRight
                    - fixedTotal - totalGaps;

    if (g_S.wrapper.infoFixedWidth) {
        g_S.containers[1].calculatedWidth = CalcInfoWidth();
        // Recompute total width from all containers
        int total = g_S.wrapper.padLeft + g_S.wrapper.padRight + totalGaps;
        for (int id : g_S.wrapper.order) {
            if (!IsContainerEnabled(id)) continue;
            if (id == 4 && g_S.wrapper.vizAsBackground) continue;
            total += GetContainerWidth(id);
        }
        g_S.width = total;
    } else {
        if (remaining < 40) remaining = 40; // enforce a minimum readable info width
        g_S.containers[1].calculatedWidth = remaining;
        // width stays as user-set (containers fill it)
    }
}


void LoadSettings() {
    // ── Wrapper ─────────────────────────────────────────────────────────────
    g_S.height            = Wh_GetIntSetting(L"LayoutGroup.PanelHeight");
    g_S.wrapper.padTop    = Wh_GetIntSetting(L"LayoutGroup.WrapPadTop");
    g_S.wrapper.padBottom = Wh_GetIntSetting(L"LayoutGroup.WrapPadBottom");
    g_S.wrapper.padLeft   = Wh_GetIntSetting(L"LayoutGroup.WrapPadLeft");
    g_S.wrapper.padRight  = Wh_GetIntSetting(L"LayoutGroup.WrapPadRight");
    g_S.wrapper.globalGap = Wh_GetIntSetting(L"LayoutGroup.GlobalGap");
    g_S.wrapper.vizAsBackground = Wh_GetIntSetting(L"VisualizerContainerGroup.VizAsBackground") != 0;
    g_S.offsetX           = Wh_GetIntSetting(L"LayoutGroup.OffsetX");
    g_S.offsetY           = Wh_GetIntSetting(L"LayoutGroup.OffsetY");

    // Clamp
    if (g_S.height < 32)           g_S.height           = 52;
    if (g_S.wrapper.padTop < 0)    g_S.wrapper.padTop    = 0;
    if (g_S.wrapper.padBottom < 0) g_S.wrapper.padBottom = 0;
    if (g_S.wrapper.padLeft < 0)   g_S.wrapper.padLeft   = 0;
    if (g_S.wrapper.padRight < 0)  g_S.wrapper.padRight  = 0;
    if (g_S.wrapper.globalGap < 0) g_S.wrapper.globalGap = 0;

    // Container order from 4-digit integer (e.g. 1234, 1243)
    {
        int orderCode = Wh_GetIntSetting(L"LayoutGroup.ContainerOrder");
        if (orderCode <= 0) orderCode = 1234;
        g_S.wrapper.order.clear();
        g_S.wrapper.order.push_back((orderCode / 1000) % 10);
        g_S.wrapper.order.push_back((orderCode /  100) % 10);
        g_S.wrapper.order.push_back((orderCode /   10) % 10);
        g_S.wrapper.order.push_back((orderCode       ) % 10);
        // Remove duplicates and out-of-range values
        std::vector<int> seen;
        std::vector<int> clean;
        for (int id : g_S.wrapper.order) {
            if (id < 1 || id > 4) continue;
            if (std::find(seen.begin(), seen.end(), id) != seen.end()) continue;
            seen.push_back(id);
            clean.push_back(id);
        }
        // Append any missing ids at the end
        for (int id : {1,2,3,4}) {
            if (std::find(seen.begin(), seen.end(), id) == seen.end())
                clean.push_back(id);
        }
        g_S.wrapper.order = clean;
    }

    // ── Panel base width (before RecomputeLayout) ───────────────────────────
    g_S.width = Wh_GetIntSetting(L"LayoutGroup.PanelWidth");
    if (g_S.width < 150) g_S.width = 360;

    // ── Info container ──────────────────────────────────────────────────────
    g_S.wrapper.infoFixedWidth = Wh_GetIntSetting(L"InfoContainerGroup.InfoFixedWidth") != 0;
    g_S.wrapper.infoWidth      = Wh_GetIntSetting(L"InfoContainerGroup.InfoWidth");
    if (g_S.wrapper.infoWidth < 40) g_S.wrapper.infoWidth = 40;

    g_S.fontSize       = Wh_GetIntSetting(L"InfoContainerGroup.FontSize");
    g_S.textGap        = Wh_GetIntSetting(L"InfoContainerGroup.TextGap");
    g_S.artTextGap     = Wh_GetIntSetting(L"InfoContainerGroup.ArtTextGap");
    if (g_S.artTextGap < 0)  g_S.artTextGap = 0;
    if (g_S.artTextGap > 80) g_S.artTextGap = 80;
    if (g_S.fontSize < 7)    g_S.fontSize    = 11;

    g_S.textControlGap = Wh_GetIntSetting(L"ControlsContainerGroup.TextControlGap");
    if (g_S.textControlGap < 0)  g_S.textControlGap = 0;
    if (g_S.textControlGap > 80) g_S.textControlGap = 80;

    // ── Container padding slots ─────────────────────────────────────────────
    g_S.containers[0].padLeft   = Wh_GetIntSetting(L"MediaContainerGroup.MediaPadLeft");
    g_S.containers[0].padRight  = Wh_GetIntSetting(L"MediaContainerGroup.MediaPadRight");
    g_S.containers[0].padTop    = 0;
    g_S.containers[0].padBottom = 0;
    g_S.mediaCornerRadius       = Wh_GetIntSetting(L"MediaContainerGroup.MediaCornerRadius");
    if (g_S.mediaCornerRadius < 0)  g_S.mediaCornerRadius = 0;
    if (g_S.mediaCornerRadius > 32) g_S.mediaCornerRadius = 32;
    g_S.mediaWidth              = Wh_GetIntSetting(L"MediaContainerGroup.MediaWidth");
    if (g_S.mediaWidth < 0) g_S.mediaWidth = 0;
    g_S.mediaAutoSize           = Wh_GetIntSetting(L"MediaContainerGroup.MediaAutoSize") != 0;
    g_S.containers[1].padLeft   = Wh_GetIntSetting(L"InfoContainerGroup.InfoPadLeft");
    g_S.containers[1].padRight  = Wh_GetIntSetting(L"InfoContainerGroup.InfoPadRight");
    g_S.containers[1].padTop    = 0;
    g_S.containers[1].padBottom = 0;
    g_S.containers[2].padLeft   = Wh_GetIntSetting(L"ControlsContainerGroup.CtrlPadLeft");
    g_S.containers[2].padRight  = Wh_GetIntSetting(L"ControlsContainerGroup.CtrlPadRight");
    g_S.containers[2].padTop    = 0;
    g_S.containers[2].padBottom = 0;
    g_S.containers[3].padLeft   = Wh_GetIntSetting(L"VisualizerContainerGroup.VizPadLeft");
    g_S.containers[3].padRight  = Wh_GetIntSetting(L"VisualizerContainerGroup.VizPadRight");
    g_S.containers[3].padTop    = 0;
    g_S.containers[3].padBottom = 0;

    // Clamp all container padding to non-negative
    for (auto& c : g_S.containers) {
        if (c.padLeft   < 0) c.padLeft   = 0;
        if (c.padRight  < 0) c.padRight  = 0;
        if (c.padTop    < 0) c.padTop    = 0;
        if (c.padBottom < 0) c.padBottom = 0;
    }

    // ── Theme ───────────────────────────────────────────────────────────────
    PCWSTR themeStr = Wh_GetStringSetting(L"AppearanceGroup.Theme");
    if (themeStr) {
        if      (!wcscmp(themeStr, L"windows"))               g_S.background = 0;
        else if (!wcscmp(themeStr, L"transparent"))           g_S.background = 1;
        else if (!wcscmp(themeStr, L"blurred"))               g_S.background = 2;
        else if (!wcscmp(themeStr, L"gradient"))              g_S.background = 3;
        else if (!wcscmp(themeStr, L"gradient_lr"))           g_S.background = 4;
        else if (!wcscmp(themeStr, L"gradient_rl"))           g_S.background = 12;
        else if (!wcscmp(themeStr, L"gradient_vertical"))     g_S.background = 5;
        else if (!wcscmp(themeStr, L"gradient_radial"))       g_S.background = 6;
        else if (!wcscmp(themeStr, L"gradient_vertical_inv")) g_S.background = 7;
        else if (!wcscmp(themeStr, L"split"))                 g_S.background = 9;
        else if (!wcscmp(themeStr, L"neon"))                  g_S.background = 10;
        else if (!wcscmp(themeStr, L"aurora"))                g_S.background = 11;
        else if (!wcscmp(themeStr, L"album_blur"))            g_S.background = 13;
        else                                                  g_S.background = 4;
        Wh_FreeStringSetting(themeStr);
    }

    g_S.hideFullscreen    = Wh_GetIntSetting(L"BehaviorGroup.HideFullscreen") != 0;
    g_S.idleTimeout       = Wh_GetIntSetting(L"BehaviorGroup.IdleTimeout");
    g_S.idleScreenEnabled = Wh_GetIntSetting(L"BehaviorGroup.IdleScreenEnabled") != 0;
    g_S.idleScreenDelay   = Wh_GetIntSetting(L"BehaviorGroup.IdleScreenDelay");
    if (g_S.idleScreenDelay < 1)   g_S.idleScreenDelay = 1;
    if (g_S.idleScreenDelay > 300) g_S.idleScreenDelay = 300;
    g_S.showVisualizer    = Wh_GetIntSetting(L"VisualizerContainerGroup.VizEnabled") != 0;

    // EQ
    PCWSTR vizEQStr = Wh_GetStringSetting(L"VisualizerContainerGroup.VizEQ");
    g_S.vizEQ = 0;
    if (vizEQStr) {
        if      (!wcscmp(vizEQStr, L"bass"))       g_S.vizEQ = 1;
        else if (!wcscmp(vizEQStr, L"rock"))       g_S.vizEQ = 2;
        else if (!wcscmp(vizEQStr, L"pop"))        g_S.vizEQ = 3;
        else if (!wcscmp(vizEQStr, L"jazz"))       g_S.vizEQ = 4;
        else if (!wcscmp(vizEQStr, L"electronic")) g_S.vizEQ = 5;
        Wh_FreeStringSetting(vizEQStr);
    }

    // Shape
    PCWSTR vizShapeStr = Wh_GetStringSetting(L"VisualizerContainerGroup.VizShape");
    g_S.vizShape = 0;
    if (vizShapeStr) {
        if      (!wcscmp(vizShapeStr, L"mountain")) g_S.vizShape = 1;
        else if (!wcscmp(vizShapeStr, L"mirror"))   g_S.vizShape = 2;
        else if (!wcscmp(vizShapeStr, L"wave"))     g_S.vizShape = 3;
        else if (!wcscmp(vizShapeStr, L"breathe"))  g_S.vizShape = 4;
        Wh_FreeStringSetting(vizShapeStr);
    }

    // Mode
    PCWSTR vizModeStr = Wh_GetStringSetting(L"VisualizerContainerGroup.VizMode");
    g_S.vizMode = 0;
    if (vizModeStr) {
        if      (!wcscmp(vizModeStr, L"dynamic_album"))    g_S.vizMode = 1;
        else if (!wcscmp(vizModeStr, L"dynamic_gradient")) g_S.vizMode = 2;
        else if (!wcscmp(vizModeStr, L"custom_gradient"))  g_S.vizMode = 3;
        else if (!wcscmp(vizModeStr, L"acrylic"))          g_S.vizMode = 4;
        Wh_FreeStringSetting(vizModeStr);
    }

    // Anchor
    PCWSTR vizAnchorStr = Wh_GetStringSetting(L"VisualizerContainerGroup.VizAnchor");
    g_S.vizAnchor = 1;
    if (vizAnchorStr) {
        if      (!wcscmp(vizAnchorStr, L"top"))    g_S.vizAnchor = 0;
        else if (!wcscmp(vizAnchorStr, L"bottom")) g_S.vizAnchor = 2;
        Wh_FreeStringSetting(vizAnchorStr);
    }

    PCWSTR vizHex = Wh_GetStringSetting(L"VisualizerContainerGroup.VizColor");
    DWORD vizRgb = 0xFFFFFF;
    if (vizHex) { if (wcslen(vizHex) > 0) vizRgb = wcstoul(vizHex, nullptr, 16); Wh_FreeStringSetting(vizHex); }
    g_S.vizColor = 0xFF000000 | vizRgb;

    PCWSTR viz1Hex = Wh_GetStringSetting(L"VisualizerContainerGroup.VizColor1");
    DWORD viz1Rgb = 0x1ED760;
    if (viz1Hex) { if (wcslen(viz1Hex) > 0) viz1Rgb = wcstoul(viz1Hex, nullptr, 16); Wh_FreeStringSetting(viz1Hex); }
    g_S.vizColor1 = 0xFF000000 | viz1Rgb;

    PCWSTR viz2Hex = Wh_GetStringSetting(L"VisualizerContainerGroup.VizColor2");
    DWORD viz2Rgb = 0x00B4FF;
    if (viz2Hex) { if (wcslen(viz2Hex) > 0) viz2Rgb = wcstoul(viz2Hex, nullptr, 16); Wh_FreeStringSetting(viz2Hex); }
    g_S.vizColor2 = 0xFF000000 | viz2Rgb;

    g_S.vizBarGap = Wh_GetIntSetting(L"VisualizerContainerGroup.VizBarGap");
    if (g_S.vizBarGap < 0)  g_S.vizBarGap = 0;
    if (g_S.vizBarGap > 20) g_S.vizBarGap = 20;

    g_S.vizBars = Wh_GetIntSetting(L"VisualizerContainerGroup.VizBars");
    if (g_S.vizBars < 1)  g_S.vizBars = 7;
    if (g_S.vizBars > 20) g_S.vizBars = 20;

    g_S.vizBarWidth = Wh_GetIntSetting(L"VisualizerContainerGroup.VizBarWidth");
    if (g_S.vizBarWidth < 0)  g_S.vizBarWidth = 0;
    if (g_S.vizBarWidth > 64) g_S.vizBarWidth = 64;

    g_S.idleBarSize = Wh_GetIntSetting(L"VisualizerContainerGroup.IdleBarSize");
    if (g_S.idleBarSize < 0)   g_S.idleBarSize = 0;
    if (g_S.idleBarSize > 100) g_S.idleBarSize = 100;

    g_S.vizSensitivity = Wh_GetIntSetting(L"VisualizerContainerGroup.VizSensitivity");
    if (g_S.vizSensitivity < 0)   g_S.vizSensitivity = 0;
    if (g_S.vizSensitivity > 100) g_S.vizSensitivity = 100;

    // Style
    g_S.roundedCorners       = Wh_GetIntSetting(L"AppearanceGroup.RoundedCorners") != 0;
    g_S.cornerRadius         = Wh_GetIntSetting(L"AppearanceGroup.CornerRadius");
    if (g_S.cornerRadius < 2)  g_S.cornerRadius = 2;
    if (g_S.cornerRadius > 32) g_S.cornerRadius = 32;
    g_S.showBorder           = Wh_GetIntSetting(L"AppearanceGroup.ShowBorder") != 0;
    g_S.showContainerBorders = Wh_GetIntSetting(L"AppearanceGroup.ShowContainerBorders") != 0;
    g_S.autoTheme            = Wh_GetIntSetting(L"InfoContainerGroup.AutoTheme") != 0;
    g_S.dynamicHover         = Wh_GetIntSetting(L"ControlsContainerGroup.DynamicHover") != 0;

    PCWSTR hex = Wh_GetStringSetting(L"InfoContainerGroup.TextColor");
    DWORD rgb = 0xFFFFFF;
    if (hex) { if (wcslen(hex) > 0) rgb = wcstoul(hex, nullptr, 16); Wh_FreeStringSetting(hex); }
    g_S.manualText = 0xFF000000 | rgb;

    PCWSTR hoverHex = Wh_GetStringSetting(L"ControlsContainerGroup.HoverColor");
    DWORD hoverRgb = 0x1ED760;
    if (hoverHex) { if (wcslen(hoverHex) > 0) hoverRgb = wcstoul(hoverHex, nullptr, 16); Wh_FreeStringSetting(hoverHex); }
    g_S.hoverColor = 0xFF000000 | hoverRgb;

    // Icon
    g_S.iconSize      = Wh_GetIntSetting(L"ControlsContainerGroup.IconSize");
    g_S.buttonSpacing = Wh_GetIntSetting(L"ControlsContainerGroup.ButtonSpacing");
    if (g_S.iconSize < 6)        g_S.iconSize      = 6;
    if (g_S.iconSize > 28)       g_S.iconSize      = 28;
    if (g_S.buttonSpacing < 18)  g_S.buttonSpacing = 18;
    if (g_S.buttonSpacing > 60)  g_S.buttonSpacing = 60;

    PCWSTR iconThemeStr = Wh_GetStringSetting(L"ControlsContainerGroup.IconTheme");
    g_S.iconTheme = 0;
    if (iconThemeStr) {
        if      (!wcscmp(iconThemeStr, L"mdl2"))            g_S.iconTheme = 1;
        else if (!wcscmp(iconThemeStr, L"fluent"))          g_S.iconTheme = 2;
        else if (!wcscmp(iconThemeStr, L"fluent_filled"))   g_S.iconTheme = 3;
        else if (!wcscmp(iconThemeStr, L"fluent_outline2")) g_S.iconTheme = 4;
        Wh_FreeStringSetting(iconThemeStr);
    }

    // General misc
    g_S.enableMask   = Wh_GetIntSetting(L"AppearanceGroup.EnableMask") != 0;
    g_S.maskOpacity  = Wh_GetIntSetting(L"AppearanceGroup.MaskOpacity");
    if (g_S.maskOpacity < 0)   g_S.maskOpacity = 0;
    if (g_S.maskOpacity > 255) g_S.maskOpacity = 255;
    g_S.themeOpacity = Wh_GetIntSetting(L"AppearanceGroup.ThemeOpacity");
    if (g_S.themeOpacity < 0)   g_S.themeOpacity = 0;
    if (g_S.themeOpacity > 255) g_S.themeOpacity = 255;
    g_S.themeBlur    = Wh_GetIntSetting(L"AppearanceGroup.ThemeBlur");
    if (g_S.themeBlur < 0)   g_S.themeBlur = 0;
    if (g_S.themeBlur > 100) g_S.themeBlur = 100;

    g_S.miniMode      = Wh_GetIntSetting(L"MiniModeGroup.MiniMode") != 0;

    g_S.fadeFullscreen = Wh_GetIntSetting(L"BehaviorGroup.FadeFullscreen") != 0;
    g_S.disableScroll  = Wh_GetIntSetting(L"BehaviorGroup.DisableScroll")  != 0;

    // Mini mode show flags
    g_S.miniShowMedia      = Wh_GetIntSetting(L"MiniModeGroup.MiniShowMedia")      != 0;
    g_S.miniShowVisualizer = Wh_GetIntSetting(L"MiniModeGroup.MiniShowVisualizer") != 0;

    // Per-container enable toggles
    g_S.mediaEnabled    = Wh_GetIntSetting(L"MediaContainerGroup.MediaEnabled")       != 0;
    g_S.infoEnabled     = Wh_GetIntSetting(L"InfoContainerGroup.InfoEnabled")         != 0;
    g_S.controlsEnabled = Wh_GetIntSetting(L"ControlsContainerGroup.ControlsEnabled") != 0;

    // Info scroll speed
    g_S.scrollSpeed = Wh_GetIntSetting(L"InfoContainerGroup.ScrollSpeed");
    if (g_S.scrollSpeed < 0) g_S.scrollSpeed = 0;
    if (g_S.scrollSpeed > 2) g_S.scrollSpeed = 2;

    // Visualizer container width override
    g_S.vizContainerWidth = Wh_GetIntSetting(L"VisualizerContainerGroup.VizWidth");
    if (g_S.vizContainerWidth < 0) g_S.vizContainerWidth = 0;

    // Focused app
    PCWSTR focusApp = Wh_GetStringSetting(L"BehaviorGroup.FocusedApp");
    if (focusApp && wcscmp(focusApp, L"all") != 0) {
        if (!wcscmp(focusApp, L"applemusic")) g_S.focusedApp = L"applemusic";
        else if (!wcscmp(focusApp, L"ytmusic")) g_S.focusedApp = L"ytmusic";
        else g_S.focusedApp = focusApp;
    } else {
        g_S.focusedApp = L"";
    }
    if (focusApp) Wh_FreeStringSetting(focusApp);

    // Controls
    g_S.showPlaybackControls = Wh_GetIntSetting(L"ControlsContainerGroup.ShowPlaybackControls") != 0;
    g_S.showShuffleButton    = Wh_GetIntSetting(L"ControlsContainerGroup.ShowShuffleButton")    != 0;
    g_S.showRepeatButton     = Wh_GetIntSetting(L"ControlsContainerGroup.ShowRepeatButton")     != 0;
    g_S.showSpeakerIcon      = Wh_GetIntSetting(L"ControlsContainerGroup.ShowSpeakerIcon")      != 0;
    g_S.scrollVolume         = Wh_GetIntSetting(L"ControlsContainerGroup.ScrollVolume")         != 0;
    g_S.ctrlIconDynamic      = Wh_GetIntSetting(L"ControlsContainerGroup.CtrlIconDynamic")      != 0;

    PCWSTR ctrlIconHex = Wh_GetStringSetting(L"ControlsContainerGroup.CtrlIconColor");
    DWORD ctrlIconRgb = 0xFFFFFF;
    if (ctrlIconHex) { if (wcslen(ctrlIconHex) > 0) ctrlIconRgb = wcstoul(ctrlIconHex, nullptr, 16); Wh_FreeStringSetting(ctrlIconHex); }
    g_S.ctrlIconColor = 0xFF000000 | ctrlIconRgb;

    g_S.showProgressBar      = Wh_GetIntSetting(L"ProgressBarGroup.ShowProgressBar")            != 0;
    g_S.progressBarHeight    = Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarHeight");
    if (g_S.progressBarHeight < 1) g_S.progressBarHeight = 1;
    if (g_S.progressBarHeight > 6) g_S.progressBarHeight = 6;
    g_S.progressBarDynamic   = Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarDynamic") != 0;

    PCWSTR pbHex = Wh_GetStringSetting(L"ProgressBarGroup.ProgressBarColor");
    DWORD pbRgb = 0x1ED760;
    if (pbHex) { if (wcslen(pbHex) > 0) pbRgb = wcstoul(pbHex, nullptr, 16); Wh_FreeStringSetting(pbHex); }
    g_S.progressBarColor = 0xFF000000 | pbRgb;

    g_S.progressBarPadLeft  = Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarPadLeft");
    g_S.progressBarPadRight = Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarPadRight");
    g_S.progressBarOffsetY  = Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarOffsetY");
    if (g_S.progressBarPadLeft  < 0)  g_S.progressBarPadLeft  = 0;
    if (g_S.progressBarPadLeft  > 80) g_S.progressBarPadLeft  = 80;
    if (g_S.progressBarPadRight < 0)  g_S.progressBarPadRight = 0;
    if (g_S.progressBarPadRight > 80) g_S.progressBarPadRight = 80;
    if (g_S.progressBarOffsetY < -20) g_S.progressBarOffsetY  = -20;
    if (g_S.progressBarOffsetY >  20) g_S.progressBarOffsetY  =  20;

    PCWSTR pbLayerStr = Wh_GetStringSetting(L"ProgressBarGroup.ProgressBarLayer");
    g_S.progressBarLayer = 0;
    if (pbLayerStr) {
        if      (!wcscmp(pbLayerStr, L"above_both"))          g_S.progressBarLayer = 1;
        else if (!wcscmp(pbLayerStr, L"under_text"))          g_S.progressBarLayer = 2;
        else if (!wcscmp(pbLayerStr, L"under_text_only"))     g_S.progressBarLayer = 3;
        else if (!wcscmp(pbLayerStr, L"under_controls_only")) g_S.progressBarLayer = 4;
        Wh_FreeStringSetting(pbLayerStr);
    }

    // ── Final: compute all container widths and g_S.width ───────────────────
    RecomputeLayout();
}

// ── Globals ───────────────────────────────────────────────────────────────────
HWND          g_hWnd             = NULL;
int           g_Hover            = 0;    // 0=none 1=prev 2=pp 3=next 4=volume 5=shuffle 6=repeat
bool          g_VolumeHover      = false;
HWINEVENTHOOK g_TbHook           = nullptr;
UINT          g_TbCreatedMsg     = RegisterWindowMessage(L"TaskbarCreated");
int           g_IdleSecs         = 0;
bool          g_IdleHide         = false;
bool          g_AnimTimerRunning = false;
float         g_FadeAlpha        = 1.f;   // 0.0 = fully hidden, 1.0 = fully visible
bool          g_FadeIn           = true;  // direction of current fade
bool          g_FadeActive       = false;
bool          g_FullscreenHidden = false; // true while hidden due to fullscreen
#define IDT_FADE           1004
#define IDT_FULLSCREEN     1007  // 100ms fullscreen poll timer
#define IDT_POLL           1001
#define IDT_ANIM           1002
#define IDT_POS            1008  // 50ms position polling for live progress bar

// Text Crossfade Animation
float g_TextFadeAlpha    = 1.f;   // current displayed text opacity
bool  g_TextFadeOut      = false; // true = fading out old text
wstring g_PendingTitle   = L"";
wstring g_PendingArtist  = L"";
#define IDT_TEXT_FADE 1006

// Hover Animation States
float         g_HoverProgress[7] = {0.f};
bool          g_HoverAnimRunning = false;
#define IDT_HOVER_ANIM 1003

// ── Idle State ────────────────────────────────────────────────────────────────
int  g_NoMediaSecs = 0;   // seconds since any media session was last seen
bool g_IdleState   = false; // true once no media for long enough



// ── Visualizer ────────────────────────────────────────────────────────────────
#define IDT_VIZ    1005
// VIZ_BARS is now runtime-dynamic. Arrays are sized to max (20).
#define VIZ_BARS_MAX 20
// OPT: moved up so twiddle/Hann arrays can use FFT_SIZE before CaptureThreadProc
#define FFT_SIZE   1024
#define NUM_BANDS  7

float g_VizPeak[VIZ_BARS_MAX]   = {};
float g_VizTarget[VIZ_BARS_MAX] = {};
bool  g_VizTimerRunning      = false;

// Per-bar random seeds
static const float VIZ_SEEDS[VIZ_BARS_MAX] = {
    0.83f, 0.41f, 1.27f, 0.61f, 1.09f, 0.37f, 0.95f,
    0.52f, 1.18f, 0.74f, 0.29f, 1.03f, 0.66f, 0.88f,
    0.45f, 1.21f, 0.57f, 0.93f, 0.31f, 1.15f
};

// ── OPT: Global art version - incremented only when art actually changes ──────
// Replaces the buggy s_artVer local static that incremented every frame,
// causing the blur cache to miss on every single WM_PAINT.
static int   g_ArtVersion     = 0;

// ── OPT: Pre-baked FFT twiddle factors ────────────────────────────────────────
// Avoids cosf/sinf inside the butterfly loop (~10240 trig calls per FFT pass).
static float g_TwiddleRe[FFT_SIZE / 2] = {};
static float g_TwiddleIm[FFT_SIZE / 2] = {};

static void BuildTwiddleFactors() {
    for (int i = 0; i < FFT_SIZE / 2; i++) {
        float ang = -2.0f * 3.14159265f * i / FFT_SIZE;
        g_TwiddleRe[i] = cosf(ang);
        g_TwiddleIm[i] = sinf(ang);
    }
}

// ── OPT: IsLight() result cache ───────────────────────────────────────────────
// Avoids repeated RegGetValueW kernel trips inside DrawPanel.
// Re-checked every 5 seconds; negligible staleness for a theme toggle.
static bool  g_CachedIsLight     = false;
static DWORD g_CachedIsLightTick = 0;

static bool IsLightCached() {
    DWORD now = GetTickCount();
    if (now - g_CachedIsLightTick > 5000) {
        DWORD v = 0, sz = sizeof(v);
        g_CachedIsLight = (RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS && v);
        g_CachedIsLightTick = now;
    }
    return g_CachedIsLight;
}

// ── OPT: Cached layout struct ─────────────────────────────────────────────────
// CalcLayout was called in WM_MOUSEMOVE (every pixel), WM_LBUTTONUP,
// WM_MOUSEWHEEL, and DrawPanel. Results only change on settings/resize.
struct CachedLayout {
    ControlLayout ctrl;
    int   W = 0, H = 0;
    int   aX = 0, aY = 0, aS = 0, aW = 0, aR = 0;  // art: x, y, height, width, corner-radius
    float barZoneY = 0.f, barZoneH = 0.f;
    float barW = 0.f, barStep = 0.f, startX = 0.f;
    float maxBarH = 0.f, minBarH = 0.f;
    bool  valid = false;
} g_Layout;
static bool g_LayoutDirty = true;

struct ContainerOrigin {
    int id;
    int x;          // left edge of content area inside the container
    int width;      // content width (calculatedWidth minus padLeft/padRight)
    int cellX;      // left edge of the container cell (before internal padding)
    int cellWidth;  // = calculatedWidth
};

// We store the computed origins for DrawPanel to use without recalculating.
static ContainerOrigin g_Origins[4];
static int             g_OriginCount = 0;

static void RebuildLayout(int W, int H) {
    // ── Step 1: Walk the order vector, assign X to each container cell ───────
    g_OriginCount = 0;
    int curX = g_S.wrapper.padLeft;
    bool first = true;

    for (int id : g_S.wrapper.order) {
        if (!IsContainerEnabled(id)) continue;
        if (id == 4 && g_S.wrapper.vizAsBackground) continue;

        if (!first) curX += g_S.wrapper.globalGap;
        first = false;

        int cellW = GetContainerWidth(id);
        // Determine per-container internal padding
        int cPadL = 0, cPadR = 0;
        for (auto& c : g_S.containers)
            if (c.id == id) { cPadL = c.padLeft; cPadR = c.padRight; break; }

        ContainerOrigin co;
        co.id        = id;
        co.cellX     = curX;
        co.cellWidth = cellW;
        co.x         = curX + cPadL;
        co.width     = cellW - cPadL - cPadR;
        if (co.width < 0) co.width = 0;
        g_Origins[g_OriginCount++] = co;

        curX += cellW;
    }

    // ── Step 2: Media / Visualizer geometry (stored in g_Layout) ────────────
    // Find the Media container origin (id=1)
    int aX = g_S.wrapper.padLeft, aY, aS, aW, aR;

    aY = g_S.wrapper.padTop;
    aS = max(8, H - g_S.wrapper.padTop - g_S.wrapper.padBottom);

    // aW: determine art width
    if (g_S.mediaWidth > 0) {
        // Manual override always wins
        aW = max(8, g_S.mediaWidth);
    } else if (g_S.mediaAutoSize) {
        // Scale to native art aspect ratio, clamped to 3:1 max to prevent absurdly wide panels
        float ratio = max(0.25f, min(3.0f, g_ArtAspectRatio));
        aW = max(8, (int)roundf(aS * ratio));
    } else {
        // Default square
        aW = aS;
    }

    if (g_S.miniMode) {
        aX = g_S.wrapper.padLeft;
    } else {
        // Override aX from container origin map
        for (int i = 0; i < g_OriginCount; i++)
            if (g_Origins[i].id == 1) { aX = g_Origins[i].x; break; }
    }
    // Corner radius clamped to the shorter of width and height so it never exceeds either side
    aR = min(g_S.mediaCornerRadius, min(aS, aW) / 2);

    g_Layout.aX = aX;
    g_Layout.aY = aY;
    g_Layout.aS = aS;
    g_Layout.aW = aW;
    g_Layout.aR = aR;

    // ── Step 3: Visualizer bar geometry ──────────────────────────────────────
    // The Visualizer occupies either its own container slot (id=4) or the same
    // cell as Media (background layer or vizAsBackground).
    int vCellX = aX, vCellW = aW; // default: same cell as art (width not necessarily square)
    if (!g_S.wrapper.vizAsBackground) {
        for (int i = 0; i < g_OriginCount; i++) {
            if (g_Origins[i].id == 4) {
                vCellX = g_Origins[i].x;
                vCellW = g_Origins[i].width;
                break;
            }
        }
    }

    // Responsive: if Controls container is disabled, Visualizer shifts automatically
    // because the order vector already skips disabled containers - no extra logic needed.

    // Compute bar zone from the visualizer cell
    // padLeft/padRight for visualizer container accessed via containers[3] in DrawPanel
    float barZoneX = (float)(vCellX);
    float barZoneY = (float)aY;
    float barZoneH = (float)aS;
    float barZoneW = (float)vCellW;

    int barCount = max(1, g_S.vizBars);
    float gap    = max(0.f, (float)g_S.vizBarGap);
    g_Layout.barW    = max(1.5f, (barZoneW - gap * (barCount - 1)) / (float)barCount);
    g_Layout.barStep = g_Layout.barW + gap;
    g_Layout.startX  = barZoneX + (barZoneW - (g_Layout.barStep * barCount - gap)) * 0.5f;

    g_Layout.barZoneY = barZoneY;
    g_Layout.barZoneH = barZoneH;
    g_Layout.maxBarH  = barZoneH * 0.92f;
    g_Layout.minBarH  = barZoneH * 0.035f;

    // ── Step 4: Controls layout ───────────────────────────────────────────────
    // Find the Controls container x-origin so CalcLayout can anchor correctly.
    int ctrlCellX = W - g_S.wrapper.padRight; // fallback
    for (int i = 0; i < g_OriginCount; i++)
        if (g_Origins[i].id == 3) { ctrlCellX = g_Origins[i].cellX; break; }

    // CalcLayout still needs padRight expressed from W; derive it.
    int effectivePadRight = W - ctrlCellX - GetContainerWidth(3);
    if (effectivePadRight < 0) effectivePadRight = 0;

    int ctrlCellRight = ctrlCellX + GetContainerWidth(3);
    g_Layout.ctrl = CalcLayout(W, H,
        effectivePadRight,
        g_S.buttonSpacing,
        g_S.showSpeakerIcon,
        g_S.showPlaybackControls,
        g_S.showShuffleButton,
        g_S.showRepeatButton,
        g_S.wrapper.padTop,
        g_S.wrapper.padBottom,
        ctrlCellX,
        g_S.containers[2].padLeft,
        g_S.containers[2].padRight,
        ctrlCellRight);

    g_Layout.W    = W;
    g_Layout.H    = H;
    g_Layout.valid = true;
    g_LayoutDirty  = false;
}

// ── OPT: Adaptive timer rates ──────────────────────────────────────────────────
// IDT_POLL slows to 4 s when paused; IDT_POS is killed entirely when paused.
static constexpr UINT POLL_RATE_PLAYING = 1000;
static constexpr UINT POLL_RATE_PAUSED  = 4000;
static constexpr UINT POS_RATE_PLAYING  = 50;
// NOTE: ApplyTimerRates() is defined after g_M (MediaSnap) below.

// ── Real FFT (Cooley-Tukey, in-place, power-of-2) ─────────────────────────────
// OPT: butterfly now indexes pre-baked twiddle tables (g_TwiddleRe/Im) instead
// of calling cosf/sinf per stage - eliminates ~10 240 trig calls per FFT pass.
static void FFT(std::vector<float>& re, std::vector<float>& im) {
    int n = (int)re.size();
    // Bit-reversal permutation
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) { std::swap(re[i], re[j]); std::swap(im[i], im[j]); }
    }
    // FFT butterfly - twiddle factors from pre-baked table
    for (int len = 2; len <= n; len <<= 1) {
        int halfLen = len / 2;
        int stride  = n / len;   // step through the twiddle table
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < halfLen; j++) {
                float wRe = g_TwiddleRe[j * stride];
                float wIm = g_TwiddleIm[j * stride];
                float uRe = re[i+j],          uIm = im[i+j];
                float vRe = re[i+j+halfLen]*wRe - im[i+j+halfLen]*wIm;
                float vIm = re[i+j+halfLen]*wIm + im[i+j+halfLen]*wRe;
                re[i+j]         = uRe + vRe; im[i+j]         = uIm + vIm;
                re[i+j+halfLen] = uRe - vRe; im[i+j+halfLen] = uIm - vIm;
            }
        }
    }
}

// ── Audio Capture ────────────────────────────────────────────────────────────
// Loopback FFT visualizer - always captures all system audio output.
// Focused App setting only affects the media session (title/art/controls).

static std::atomic<float> g_VizBands[NUM_BANDS] = {};
static std::atomic<bool>  g_CaptureRunning { false };
static std::thread        g_CaptureThread;
// Event handle for WASAPI event-driven capture (wakes thread only when data arrives)
static HANDLE             g_hCaptureEvent = nullptr;

static float g_HannWindow[FFT_SIZE] = {};

static void BuildHannWindow() {
    for (int i = 0; i < FFT_SIZE; i++)
        g_HannWindow[i] = 0.5f * (1.f - cosf(2.f * 3.14159265f * i / (FFT_SIZE - 1)));
}

// ── Logarithmic FFT bin boundaries ───────────────────────────────────────────
// Human hearing is logarithmic (octave-based). Linear bin boundaries compress
// bass into too few bars and spread highs across too many, causing jitter.
// These are recomputed at runtime from the actual sample rate.
static int g_LogBinStart[NUM_BANDS + 1] = {};

static void BuildLogBins(UINT32 sampleRate) {
    // 7 bands: sub-bass, bass, low-mid, mid, high-mid, presence, air
    static const float FREQ_EDGES[NUM_BANDS + 1] = {
        20.f, 120.f, 300.f, 800.f, 2500.f, 6000.f, 14000.f, 20000.f
    };
    for (int b = 0; b <= NUM_BANDS; b++) {
        int bin = (int)(FREQ_EDGES[b] * FFT_SIZE / (float)sampleRate);
        g_LogBinStart[b] = max(1, min(FFT_SIZE / 2 - 1, bin));
    }
}

static void CaptureThreadProc() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    BuildHannWindow();
    BuildTwiddleFactors(); // OPT: pre-bake twiddle table once per thread start

    com_ptr<IMMDeviceEnumerator> pEnum;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_ALL, IID_PPV_ARGS(&pEnum)))) { CoUninitialize(); return; }

    com_ptr<IMMDevice> pDev;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, pDev.put()))) {
        CoUninitialize(); return;
    }

    // ── Event-driven loopback setup ───────────────────────────────────────────
    // AUDCLNT_STREAMFLAGS_EVENTCALLBACK lets the OS signal our event when a
    // buffer is ready instead of us busy-polling with Sleep(). Idle CPU ~0%.
    com_ptr<IAudioClient>        pClient;
    com_ptr<IAudioCaptureClient> pCapture;
    UINT32  sampleRate = 48000;
    UINT32  channels   = 2;
    bool    isFloat    = true;

    {
        com_ptr<IAudioClient> pC;
        if (SUCCEEDED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
            nullptr, reinterpret_cast<void**>(pC.put())))) {
            WAVEFORMATEX* pwfx = nullptr;
            pC->GetMixFormat(&pwfx);
            if (pwfx) {
                sampleRate = pwfx->nSamplesPerSec;
                channels   = pwfx->nChannels;
                isFloat    = (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
                             (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
                              reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->SubFormat ==
                              KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);

                // Use event-callback flag - SetEventHandle must be called before Initialize
                if (SUCCEEDED(pC->Initialize(AUDCLNT_SHAREMODE_SHARED,
                    AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                    200000, 0, pwfx, nullptr))) {
                    if (g_hCaptureEvent)
                        pC->SetEventHandle(g_hCaptureEvent);
                    com_ptr<IAudioCaptureClient> pCap;
                    if (SUCCEEDED(pC->GetService(IID_PPV_ARGS(&pCap)))) {
                        pClient  = pC;
                        pCapture = pCap;
                    }
                }
                CoTaskMemFree(pwfx);
            }
        }
    }

    BuildLogBins(sampleRate);

    static const int RING_CAP = FFT_SIZE * 4;
    std::vector<float> ringBuf(RING_CAP, 0.f);
    int ringHead = 0, ringCount = 0;
    std::vector<float> re(FFT_SIZE), im(FFT_SIZE);

    // Per-band envelope - gravity-based decay: constant speed fall, instant attack
    // This matches how professional VU meters behave.
    float bandEnv[NUM_BANDS] = {};
    static const float GRAVITY[NUM_BANDS] = {
        0.018f, 0.020f, 0.022f, 0.025f, 0.030f, 0.036f, 0.042f
    };

    if (pClient) pClient->Start();

    while (g_CaptureRunning.load(std::memory_order_relaxed)) {

        // Block until the audio engine signals new data (or 20ms timeout so we
        // can still check g_CaptureRunning and exit cleanly).
        if (g_hCaptureEvent)
            WaitForSingleObject(g_hCaptureEvent, 20);
        else
            Sleep(8); // fallback if event creation failed

        if (!pCapture) continue;

        UINT32 packetSize = 0;
        if (FAILED(pCapture->GetNextPacketSize(&packetSize)) || packetSize == 0) {
            // No data this wake - apply gravity decay and publish zeros movement
            for (int b = 0; b < NUM_BANDS; b++) {
                bandEnv[b] = max(0.f, bandEnv[b] - GRAVITY[b]);
                g_VizBands[b].store(bandEnv[b], std::memory_order_relaxed);
            }
            continue;
        }

        while (packetSize > 0) {
            BYTE*  pData     = nullptr;
            UINT32 numFrames = 0;
            DWORD  flags     = 0;
            if (FAILED(pCapture->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr))) break;

            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData && numFrames > 0) {
                if (isFloat) {
                    float* src = reinterpret_cast<float*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++) mono += src[f * channels + c];
                        ringBuf[ringHead] = mono / (float)channels;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP) ringCount++;
                    }
                } else {
                    INT16* src = reinterpret_cast<INT16*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++) mono += src[f * channels + c] / 32768.f;
                        ringBuf[ringHead] = mono / (float)channels;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP) ringCount++;
                    }
                }
            }
            pCapture->ReleaseBuffer(numFrames);
            if (FAILED(pCapture->GetNextPacketSize(&packetSize))) break;
        }

        while (ringCount >= FFT_SIZE) {
            int readStart = (ringHead - ringCount + RING_CAP) % RING_CAP;
            for (int i = 0; i < FFT_SIZE; i++) {
                re[i] = ringBuf[(readStart + i) % RING_CAP] * g_HannWindow[i];
                im[i] = 0.f;
            }
            ringCount -= FFT_SIZE / 2;
            FFT(re, im);

            float t_sens    = g_S.vizSensitivity / 100.0f;
            float sliderGain = 0.25f + t_sens * t_sens * 2.75f;
            float lowM = 1.0f, midM = 1.0f, highM = 1.0f;
            switch (g_S.vizEQ) {
                case 1: lowM = 2.0f; midM = 0.6f;  highM = 0.4f;  break;
                case 2: lowM = 1.3f; midM = 1.5f;  highM = 1.2f;  break;
                case 3: lowM = 0.8f; midM = 1.2f;  highM = 1.8f;  break;
                case 4: lowM = 1.1f; midM = 0.8f;  highM = 0.6f;  break;
                case 5: lowM = 1.7f; midM = 0.6f;  highM = 1.7f;  break;
                default: break;
            }
            // Per-band sensitivity calibrated for log-spaced bins.
            // Lower bands have far more FFT bins so raw magnitude is naturally
            // higher - these scalars re-balance them perceptually.
            static const float BAND_SENSITIVITY[NUM_BANDS] = {
                0.30f, 0.22f, 0.12f, 0.06f, 0.030f, 0.018f, 0.010f
            };
            static const int BAND_EQ_ZONE[NUM_BANDS] = { 0, 0, 1, 1, 2, 2, 2 };

            for (int b = 0; b < NUM_BANDS; b++) {
                int bStart = g_LogBinStart[b];
                int bEnd   = g_LogBinStart[b + 1];
                if (bEnd <= bStart) bEnd = bStart + 1;

                // RMS across the band - smoother and more accurate than peak-pick
                float sumSq = 0.f;
                int   count = 0;
                for (int k = bStart; k < bEnd; k++) {
                    sumSq += re[k]*re[k] + im[k]*im[k];
                    count++;
                }
                float rms  = (count > 0) ? sqrtf(sumSq / (float)count) : 0.f;
                float eqM  = (BAND_EQ_ZONE[b] == 0) ? lowM : (BAND_EQ_ZONE[b] == 1) ? midM : highM;
                float mag  = max(0.f, min(1.f,
                    (rms / (FFT_SIZE * 0.5f)) / BAND_SENSITIVITY[b] * sliderGain * eqM));

                // Instant attack, gravity-based constant-speed decay
                if (mag >= bandEnv[b]) {
                    bandEnv[b] = mag;
                } else {
                    bandEnv[b] = max(0.f, bandEnv[b] - GRAVITY[b]);
                }
                g_VizBands[b].store(bandEnv[b], std::memory_order_relaxed);
            }
        }
    }

    if (pClient) pClient->Stop();
    CoUninitialize();
}

static void StartCaptureThread() {
    if (g_CaptureRunning.load()) return;
    if (!g_hCaptureEvent)
        g_hCaptureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_CaptureRunning.store(true);
    g_CaptureThread = std::thread(CaptureThreadProc);
}

static void StopCaptureThread() {
    if (!g_CaptureRunning.load()) return;
    g_CaptureRunning.store(false);
    // Signal the event so the thread wakes immediately and sees the stop flag
    if (g_hCaptureEvent) SetEvent(g_hCaptureEvent);
    if (g_CaptureThread.joinable()) g_CaptureThread.join();
    if (g_hCaptureEvent) { CloseHandle(g_hCaptureEvent); g_hCaptureEvent = nullptr; }
    for (int i = 0; i < NUM_BANDS; i++) g_VizBands[i].store(0.f);
}

// ── UpdateVisualizerPeaks: read FFT bands, apply Shape layout ─────────────────
// Fully dynamic: remaps NUM_BANDS (7) FFT bands onto g_S.vizBars bars using
// a fractional interpolation so every bar count from 1-20 looks correct.
// EQ is applied per-bar based on its frequency position (0=low, 1=high).
static void UpdateVisualizerPeaks() {
    const int vizBars = max(1, min(g_S.vizBars, VIZ_BARS_MAX));

    // Read real FFT band energies from capture thread
    float bands[NUM_BANDS];
    float masterPeak = 0.f;
    for (int i = 0; i < NUM_BANDS; i++) {
        bands[i]   = g_VizBands[i].load(std::memory_order_relaxed);
        masterPeak = max(masterPeak, bands[i]);
    }

    // EQ multipliers from vizEQ setting (same as capture thread zones)
    float lowM = 1.0f, midM = 1.0f, highM = 1.0f;
    switch (g_S.vizEQ) {
        case 1: lowM = 2.0f; midM = 0.6f;  highM = 0.4f;  break; // Bass
        case 2: lowM = 1.3f; midM = 1.5f;  highM = 1.2f;  break; // Rock
        case 3: lowM = 0.8f; midM = 1.2f;  highM = 1.8f;  break; // Pop
        case 4: lowM = 1.1f; midM = 0.8f;  highM = 0.6f;  break; // Jazz
        case 5: lowM = 1.7f; midM = 0.6f;  highM = 1.7f;  break; // Electronic
        default: break;
    }

    // Interpolate a value from the bands[] array at fractional position t in [0,1]
    // t=0 = sub-bass (band 0), t=1 = air (band 6)
    auto sampleBands = [&](float t) -> float {
        float pos = t * (NUM_BANDS - 1);
        int   lo  = (int)pos;
        int   hi  = min(lo + 1, NUM_BANDS - 1);
        float frac = pos - (float)lo;
        return bands[lo] * (1.f - frac) + bands[hi] * frac;
    };

    // Per-bar EQ multiplier based on frequency position [0=low, 1=high]
    auto eqForT = [&](float t) -> float {
        if      (t < 0.33f) return lowM;
        else if (t < 0.66f) return midM;
        else                return highM;
    };

    float t      = (float)GetTickCount64() * 0.001f;
    float center = (vizBars - 1) * 0.5f;
    float idleFloor = g_S.idleBarSize / 100.0f;

    for (int i = 0; i < vizBars; i++) {
        // Normalized frequency position for this bar [0..1]
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;

        switch (g_S.vizShape) {

        case 0: // Stereo: each bar maps to its frequency slice, EQ applied
            target = sampleBands(freqT) * eqForT(freqT);
            break;

        case 1: { // Mountain: center=bass, outer=highs, tapered
            float dist = fabsf((float)i - center) / max(1.f, center);
            // dist=0 at center (bass/low freq), dist=1 at edges (highs)
            // sampleBands(dist): center samples t=0 (sub-bass), edges sample t=1 (air)
            float energy = sampleBands(dist) * eqForT(dist);
            float taper = 1.6f - dist * 0.9f;
            target = max(0.f, min(1.f, (energy + masterPeak * (0.2f - dist * 0.12f)) * taper));
            break;
        }

        case 2: { // Mirror: symmetric, edges=low, center=high
            float mirrorT = 1.f - fabsf((float)i - center) / max(1.f, center); // 0=edge, 1=center
            float energy = sampleBands(mirrorT) * eqForT(mirrorT);
            target = max(0.f, min(1.f, (energy + masterPeak * (0.1f + mirrorT * 0.12f)) * 1.3f));
            break;
        }

        case 3: { // Wave: time-delayed sine modulates per-bar band energy
            float phaseOffset = (float)i * (6.283f / (float)vizBars);
            float wave = 0.55f + 0.45f * sinf(t * 3.5f - phaseOffset);
            float energy = sampleBands(freqT) * eqForT(freqT);
            target = max(0.f, min(1.f, energy * wave + masterPeak * 0.15f));
            break;
        }

        case 4: { // Breathe: all bars pulse in unison with per-bar variation
            static float s_breatheEnv = 0.f;
            if (i == 0) {
                float k = (masterPeak > s_breatheEnv) ? 0.04f : 0.015f;
                s_breatheEnv += (masterPeak - s_breatheEnv) * k;
            }
            float breathRate = 0.55f + VIZ_SEEDS[i % VIZ_BARS_MAX] * 0.18f;
            float inhale     = 0.5f + 0.5f * sinf(t * breathRate + VIZ_SEEDS[i % VIZ_BARS_MAX] * 1.2f);
            target = max(0.f, min(1.f, inhale * (0.12f + s_breatheEnv * 0.88f)));
            break;
        }

        default:
            target = masterPeak;
            break;
        }

        g_VizTarget[i] = max(idleFloor, min(1.f, target));
    }
}




// ── Live Progress Interpolation ───────────────────────────────────────────────
// GSMTC only fires position updates ~once per second. We interpolate between
// polls using a high-resolution tick so the bar moves smoothly every frame.
INT64  g_PosLastRaw   = 0;      // last raw position from GSMTC (100-ns units)
INT64  g_PosEndRaw    = 0;      // last raw end/duration from GSMTC (100-ns units)
DWORD  g_PosUpdateTick = 0;     // GetTickCount() when last raw position was captured
bool   g_PosIsPlaying  = false; // was media playing at last capture?

// Returns an interpolated position ratio [0..1] for drawing the progress bar.
static float GetLiveProgress() {
    INT64 pos = g_PosLastRaw;
    INT64 end = g_PosEndRaw;
    if (end <= 0) return 0.f;

    if (g_PosIsPlaying && g_PosUpdateTick > 0) {
        DWORD elapsed = GetTickCount() - g_PosUpdateTick;
        // Cap interpolation at 6 seconds to avoid runaway if poll stalls
        if (elapsed > 6000) elapsed = 6000;
        // 100-ns units: elapsed ms * 10000
        pos += (INT64)elapsed * 10000LL;
    }

    if (pos < 0)   pos = 0;
    if (pos > end) pos = end;
    return (float)pos / (float)end;
}

float g_Volume = 0.5f;
std::wstring g_currentMediaAppAumid;

// Cached volume interface - avoids re-enumerating all audio sessions every tick.
// Invalidated when the AUMID changes or when a call on the pointer fails.
com_ptr<ISimpleAudioVolume> g_cachedVolume;
std::wstring                g_cachedVolumeAumid;

// Walk all audio sessions and find the one matching the current media app.
// Returns nullptr if not found.
static com_ptr<ISimpleAudioVolume> FindAppVolumeInterface(const std::wstring& aumid) {
    com_ptr<IMMDeviceEnumerator> pEnumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator)))) return nullptr;

    com_ptr<IMMDevice> pDevice;
    if (FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, pDevice.put()))) return nullptr;

    com_ptr<IAudioSessionManager2> pSessionManager;
    if (FAILED(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(pSessionManager.put())))) return nullptr;

    com_ptr<IAudioSessionEnumerator> pSessionEnumerator;
    if (FAILED(pSessionManager->GetSessionEnumerator(pSessionEnumerator.put()))) return nullptr;

    int sessionCount = 0;
    pSessionEnumerator->GetCount(&sessionCount);

    wstring lowerAumid = aumid;
    transform(lowerAumid.begin(), lowerAumid.end(), lowerAumid.begin(), ::towlower);

    for (int i = 0; i < sessionCount; i++) {
        com_ptr<IAudioSessionControl> pSessionControl;
        if (FAILED(pSessionEnumerator->GetSession(i, pSessionControl.put()))) continue;

        com_ptr<IAudioSessionControl2> pSessionControl2 = pSessionControl.as<IAudioSessionControl2>();
        if (!pSessionControl2) continue;

        DWORD processId = 0;
        pSessionControl2->GetProcessId(&processId);
        if (processId == 0) continue;

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (!hProcess) continue;

        WCHAR processImagePath[MAX_PATH];
        DWORD pathSize = MAX_PATH;
        bool matched = false;
        if (QueryFullProcessImageNameW(hProcess, 0, processImagePath, &pathSize)) {
            wstring wPath = processImagePath;
            size_t lastSlash = wPath.find_last_of(L"\\");
            wstring exeName = (lastSlash == wstring::npos) ? wPath : wPath.substr(lastSlash + 1);
            size_t dot = exeName.find_last_of(L".");
            if (dot != wstring::npos) exeName = exeName.substr(0, dot);
            transform(exeName.begin(), exeName.end(), exeName.begin(), ::towlower);
            matched = (lowerAumid.find(exeName) != wstring::npos || exeName.find(lowerAumid) != wstring::npos);
        }
        CloseHandle(hProcess);

        if (matched) {
            auto pAppVol = pSessionControl2.as<ISimpleAudioVolume>();
            if (pAppVol) return pAppVol;
        }
    }
    return nullptr;
}

// Get the cached volume pointer, re-enumerating if the app changed or the pointer went stale.
static com_ptr<ISimpleAudioVolume> GetVolume(bool forceRefresh = false) {
    if (g_currentMediaAppAumid.empty()) return nullptr;

    // Re-enumerate if the app switched or caller says the pointer is stale
    if (forceRefresh || g_cachedVolumeAumid != g_currentMediaAppAumid || !g_cachedVolume) {
        g_cachedVolume     = FindAppVolumeInterface(g_currentMediaAppAumid);
        g_cachedVolumeAumid = g_currentMediaAppAumid;
    }
    return g_cachedVolume;
}

void FetchAppVolume() {
    auto pVol = GetVolume();
    if (!pVol) return;
    if (FAILED(pVol->GetMasterVolume(&g_Volume))) {
        // Pointer went stale (app restarted, session recreated) - retry once
        pVol = GetVolume(/*forceRefresh=*/true);
        if (pVol) pVol->GetMasterVolume(&g_Volume);
    }
}

void SetVolume(float level) {
    g_Volume = max(0.f, min(1.f, level));
    auto pVol = GetVolume();
    if (!pVol) return;
    if (FAILED(pVol->SetMasterVolume(g_Volume, NULL))) {
        // Stale pointer - re-enumerate and retry
        pVol = GetVolume(/*forceRefresh=*/true);
        if (pVol) pVol->SetMasterVolume(g_Volume, NULL);
    }
}

// ── Media ─────────────────────────────────────────────────────────────────────
struct MediaSnap {
    wstring title  = L"No Media";
    wstring artist = L"";
    bool    playing  = false;
    bool    hasMedia = false;
    bool    shuffleOn = false;
    int     repeatMode = 0; // 0=None, 1=Track, 2=List
    INT64   timelinePos    = 0; // current position in 100-ns units
    INT64   timelineEnd    = 0; // duration in 100-ns units
    std::unique_ptr<Bitmap> art;
    Color   primaryColor   = Color(255, 18, 18, 18);
    Color   secondaryColor = Color(255, 45, 45, 45);
    mutex   mtx;
} g_M;

// OPT: defined here (after g_M) so it can safely lock g_M.mtx.
// IDT_POLL slows to 4 s when paused; IDT_POS is killed entirely when paused.
static void ApplyTimerRates(HWND hwnd) {
    bool playing = false;
    { scoped_lock lk(g_M.mtx); playing = g_M.playing; }
    SetTimer(hwnd, IDT_POLL, playing ? POLL_RATE_PLAYING : POLL_RATE_PAUSED, NULL);
    if (playing) {
        SetTimer(hwnd, IDT_POS, POS_RATE_PLAYING, NULL);
    } else {
        KillTimer(hwnd, IDT_POS);
    }
}

GlobalSystemMediaTransportControlsSessionManager g_Mgr = nullptr;

struct AlbumPalette { Color primary; Color secondary; };

AlbumPalette GetAlbumPalette(Bitmap* bmp) {
    const Color fallbackPrimary(255, 18, 18, 18);
    const Color fallbackSecondary(255, 45, 45, 45);
    
    if (!bmp || bmp->GetLastStatus() != Ok) 
        return { fallbackPrimary, fallbackSecondary };
    
    UINT w = bmp->GetWidth(), h = bmp->GetHeight();
    if (w == 0 || h == 0) return { fallbackPrimary, fallbackSecondary };

    BitmapData data;
    Rect r(0, 0, (INT)w, (INT)h);
    if (bmp->LockBits(&r, ImageLockModeRead, PixelFormat32bppARGB, &data) != Ok)
        return { fallbackPrimary, fallbackSecondary };

    long long r1=0,g1=0,b1=0,s1=0;
    long long r2=0,g2=0,b2=0,s2=0;
    DWORD* pixels = (DWORD*)data.Scan0;
    int stride = data.Stride / 4;
    UINT midX = w / 2;

    for (UINT y = 0; y < h; y += 4) {
        for (UINT x = 0; x < w; x += 4) {
            DWORD p = pixels[y * stride + x];
            BYTE pr = (p >> 16) & 0xFF;
            BYTE pg = (p >> 8)  & 0xFF;
            BYTE pb =  p        & 0xFF;
            if (x < midX) { r1+=pr; g1+=pg; b1+=pb; s1++; }
            else          { r2+=pr; g2+=pg; b2+=pb; s2++; }
        }
    }
    bmp->UnlockBits(&data);

    Color primary   = s1 > 0 ? Color(255, (BYTE)(r1/s1), (BYTE)(g1/s1), (BYTE)(b1/s1)) : fallbackPrimary;
    Color secondary = s2 > 0 ? Color(255, (BYTE)(r2/s2), (BYTE)(g2/s2), (BYTE)(b2/s2)) : fallbackSecondary;

    int dr = abs((int)primary.GetR() - (int)secondary.GetR());
    int dg = abs((int)primary.GetG() - (int)secondary.GetG());
    int db = abs((int)primary.GetB() - (int)secondary.GetB());
    if (dr + dg + db < 60) {
        secondary = Color(255, (BYTE)(primary.GetR() * 0.35f), (BYTE)(primary.GetG() * 0.35f), (BYTE)(primary.GetB() * 0.35f));
    }
    return { primary, secondary };
}

static void UpdateAlbumBlurBg(Bitmap* art, int w, int h, int artVersion) {
    if (!art || w <= 0 || h <= 0) {
        if (g_S.blurredBg) { delete g_S.blurredBg; g_S.blurredBg = nullptr; }
        g_S.blurBgW = g_S.blurBgH = 0; g_S.blurArtVersion = -1;
        return;
    }
    if (g_S.blurredBg && g_S.blurBgW == w && g_S.blurBgH == h && g_S.blurArtVersion == artVersion) return;

    int srcW = art->GetWidth(), srcH = art->GetHeight();
    int smallW = max(1, srcW / 64), smallH = max(1, srcH / 64);
    Bitmap small(smallW, smallH, PixelFormat32bppPARGB);
    { Graphics sg(&small); sg.SetInterpolationMode(InterpolationModeBilinear); sg.DrawImage(art, 0, 0, smallW, smallH); }

    Bitmap* bg = new Bitmap(w, h, PixelFormat32bppPARGB);
    if (bg && bg->GetLastStatus() == Ok) {
        Graphics gg(bg);
        gg.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        float scaleW = (float)w / smallW, scaleH = (float)h / smallH;
        float sc = max(scaleW, scaleH) * 1.02f;
        float dw = smallW * sc, dh = smallH * sc;
        float ox = (w - dw) / 2.f, oy = (h - dh) / 2.f;
        ImageAttributes attr; attr.SetWrapMode(WrapModeTileFlipXY);
        gg.DrawImage(&small, RectF(ox, oy, dw, dh), 0.f, 0.f, (REAL)smallW, (REAL)smallH, UnitPixel, &attr);
        if (g_S.blurredBg) delete g_S.blurredBg;
        g_S.blurredBg = bg;
        g_S.blurBgW = w; g_S.blurBgH = h; g_S.blurArtVersion = artVersion;
    } else { delete bg; }
}

Bitmap* ToBitmap(IRandomAccessStreamWithContentType const& s) {
    if (!s) return nullptr;
    try {
        IStream* ns = nullptr;
        if (SUCCEEDED(CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(winrt::get_abi(s)), IID_PPV_ARGS(&ns)))) {
            Bitmap* b = Bitmap::FromStream(ns); 
            ns->Release();
            if (b && b->GetLastStatus() == Ok) return b;
            if (b) delete b;
        }
    } catch (...) {}
    return nullptr;
}

void FetchMedia() {
    try {
        if (!g_Mgr) g_Mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        if (!g_Mgr) return;

        GlobalSystemMediaTransportControlsSession ses = nullptr;
        // 1. Priority: Find the session that is actually PLAYING (e.g. YouTube while Spotify is paused)
        auto sessions = g_Mgr.GetSessions();
        for (auto const& s : sessions) {
            try {
                if (s && s.GetPlaybackInfo().PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                    ses = s; break;
                }
            } catch (...) {}
        }
        
        // 0. If user has set a focused app, filter to that exe only
        if (!g_S.focusedApp.empty()) {
            ses = nullptr;
            std::wstring lowerFocus = g_S.focusedApp;
            transform(lowerFocus.begin(), lowerFocus.end(), lowerFocus.begin(), ::towlower);
            if (lowerFocus.size() > 4 && lowerFocus.substr(lowerFocus.size()-4) == L".exe")
                lowerFocus = lowerFocus.substr(0, lowerFocus.size()-4);
            for (auto const& s : sessions) {
                try {
                    wstring aumid = s.SourceAppUserModelId().c_str();
                    transform(aumid.begin(), aumid.end(), aumid.begin(), ::towlower);
                    if (aumid.find(lowerFocus) != wstring::npos) { ses = s; break; }
                } catch (...) {}
            }
        }
        
        // 2. Fallback: If nothing is playing, grab the last active session (paused Spotify/browser)
        if (!ses) try { ses = g_Mgr.GetCurrentSession(); } catch (...) {}
        
        if (ses) {
            // Use explicit type to avoid compiler errors
            GlobalSystemMediaTransportControlsSessionMediaProperties props = nullptr;
            try { props = ses.TryGetMediaPropertiesAsync().get(); } catch (...) { 
                // If fetching properties fails (app closing), just set playing to false and keep last info
                scoped_lock lk(g_M.mtx); g_M.playing = false; return; 
            }
            if (!props) { scoped_lock lk(g_M.mtx); g_M.playing = false; return; }

            try { g_currentMediaAppAumid = ses.SourceAppUserModelId().c_str(); } catch (...) {}
            FetchAppVolume();

            scoped_lock lk(g_M.mtx);
            wstring nt = L"Unknown";
            try { nt = props.Title().c_str(); } catch (...) {}

            // Only update artwork and colors if the song actually changed
            if (nt != g_M.title || !g_M.art) {
                // Trigger text crossfade when track title changes (not on first load)
                if (nt != g_M.title && g_M.title != L"No Media") {
                    g_PendingTitle  = nt;
                    g_PendingArtist = props.Artist().c_str();
                    g_TextFadeOut   = true;
                    if (g_hWnd) SetTimer(g_hWnd, IDT_TEXT_FADE, 16, NULL);
                }
                g_M.art.reset();
                try {
                    auto ref = props.Thumbnail();
                    if (ref) {
                        auto stream = ref.OpenReadAsync().get();
                        if (stream) g_M.art.reset(ToBitmap(stream));
                    }
                } catch (...) {}
                ++g_ArtVersion; // OPT: bump version so blur cache knows art changed

                // Capture native aspect ratio for auto-size feature
                if (g_M.art) {
                    UINT artNatW = g_M.art->GetWidth();
                    UINT artNatH = g_M.art->GetHeight();
                    g_ArtAspectRatio = (artNatH > 0) ? (float)artNatW / (float)artNatH : 1.0f;
                } else {
                    g_ArtAspectRatio = 1.0f; // no art → square placeholder
                }
                // Layout must rebuild because aW may have changed (panel width changes)
                g_LayoutDirty = true;

                // Detect if cover is dark or light for mask adaptation
                if (g_M.art) {
                    Bitmap px1(1, 1, PixelFormat32bppARGB);
                    Graphics pg(&px1);
                    pg.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                    pg.DrawImage(g_M.art.get(), 0, 0, 1, 1);
                    Color avg; px1.GetPixel(0, 0, &avg);
                    double lum = 0.299*avg.GetR() + 0.587*avg.GetG() + 0.114*avg.GetB();
                    g_S.isDarkCover = (lum < 135.0);
                }

                auto palette = GetAlbumPalette(g_M.art.get());
                g_M.primaryColor   = palette.primary;
                g_M.secondaryColor = palette.secondary;
            }
            g_M.title = nt;
            try { g_M.artist = props.Artist().c_str(); } catch (...) { g_M.artist = L""; }
            
            try {
                auto pbi = ses.GetPlaybackInfo();
                g_M.playing = (pbi.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                // Keep live progress tracker in sync with play/pause transitions
                if (g_M.playing != g_PosIsPlaying) {
                    g_PosIsPlaying  = g_M.playing;
                    g_PosUpdateTick = GetTickCount();
                    g_PosLastRaw    = g_M.timelinePos;
                }
                try { g_M.shuffleOn  = pbi.IsShuffleActive() ? pbi.IsShuffleActive().Value() : false; } catch (...) {}
                try {
                    auto rm = pbi.AutoRepeatMode();
                    if (rm) {
                        auto v = rm.Value();
                        using RM = Windows::Media::MediaPlaybackAutoRepeatMode;
                        g_M.repeatMode = (v == RM::Track) ? 1 : (v == RM::List) ? 2 : 0;
                    }
                } catch (...) {}
            } catch (...) { g_M.playing = false; }

            try {
                auto tl = ses.GetTimelineProperties();
                INT64 newPos = tl.Position().count();
                INT64 newEnd = tl.EndTime().count();
                g_M.timelinePos = newPos;
                g_M.timelineEnd = newEnd;
                // Update live interpolation state whenever position changes
                bool isNowPlaying = g_M.playing;
                if (newPos != g_PosLastRaw || newEnd != g_PosEndRaw || isNowPlaying != g_PosIsPlaying) {
                    g_PosLastRaw    = newPos;
                    g_PosEndRaw     = newEnd;
                    g_PosUpdateTick = GetTickCount();
                    g_PosIsPlaying  = isNowPlaying;
                }
            } catch (...) { g_M.timelinePos = 0; g_M.timelineEnd = 0; }
            
            g_M.hasMedia = true;
        } else {
            // No session: keep last track info visible (idle screen overrides in DrawPanel if enabled)
            scoped_lock lk(g_M.mtx);
            g_M.playing  = false;
            g_M.hasMedia = false;
            g_cachedVolume = nullptr;
            g_cachedVolumeAumid.clear();
        }
    } catch (...) {
        scoped_lock lk(g_M.mtx);
        g_M.playing  = false;
        g_M.hasMedia = false;
    }
}

void Cmd(int c) {
    try {
        if (!g_Mgr) return;
        auto s = g_Mgr.GetCurrentSession(); if (!s) return;
        if (c==1) s.TrySkipPreviousAsync();
        else if (c==2) s.TryTogglePlayPauseAsync();
        else if (c==3) s.TrySkipNextAsync();
        else if (c==5) {
            // Toggle shuffle
            try {
                bool cur = false;
                { std::scoped_lock lk(g_M.mtx); cur = g_M.shuffleOn; }
                s.TryChangeShuffleActiveAsync(!cur);
            } catch (...) {}
        } else if (c==6) {
            // Cycle repeat: None -> Track -> List -> None
            try {
                int cur = 0;
                { std::scoped_lock lk(g_M.mtx); cur = g_M.repeatMode; }
                using RM = Windows::Media::MediaPlaybackAutoRepeatMode;
                RM next = (cur == 0) ? RM::Track : (cur == 1) ? RM::List : RM::None;
                s.TryChangeAutoRepeatModeAsync(next);
            } catch (...) {}
        }
    } catch (...) {}
}

// ── Scroll ────────────────────────────────────────────────────────────────────
int  g_ScOff   = 0;
int  g_ScTxtW  = 0;
int  g_ScVisW  = 0;
bool g_Scroll  = false;
bool g_ScDir   = true;
int  g_ScWait  = 80;

// ── Helpers ───────────────────────────────────────────────────────────────────
bool IsLight() {
    DWORD v=0,sz=sizeof(v);
    return RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme",RRF_RT_DWORD,nullptr,&v,&sz)==ERROR_SUCCESS && v;
}

ARGB TxtARGB() {
    if (g_S.autoTheme) return IsLightCached() ? 0xFF1a1a1a : 0xFFFFFFFF;
    return g_S.manualText;
}

void RoundRect(GraphicsPath& p, int x, int y, int w, int h, int r) {
    if (r <= 0) {
        p.AddRectangle(Rect(x, y, w, h));
        p.CloseFigure();
        return;
    }
    int d=r*2;
    p.AddArc(x,y,d,d,180,90); p.AddArc(x+w-d,y,d,d,270,90);
    p.AddArc(x+w-d,y+h-d,d,d,0,90); p.AddArc(x,y+h-d,d,d,90,90);
    p.CloseFigure();
}

// ── Custom Per-Corner Rounded Path ────────────────────────────────────────────
// Draws a quadrilateral with four independently sized rounded corners using
// GDI+ arcs. Each radius can be 0 (sharp corner) up to 32px.
// rTL=top-left, rTR=top-right, rBR=bottom-right, rBL=bottom-left.
void CustomRoundedPath(GraphicsPath& p, float x, float y, float w, float h,
                       int rTL, int rTR, int rBR, int rBL) {
    // Clamp each radius so two adjacent radii don't exceed the edge length
    int maxW = (int)(w / 2.f), maxH = (int)(h / 2.f);
    rTL = min(rTL, min(maxW, maxH));
    rTR = min(rTR, min(maxW, maxH));
    rBR = min(rBR, min(maxW, maxH));
    rBL = min(rBL, min(maxW, maxH));
    rTL = max(0, rTL); rTR = max(0, rTR);
    rBR = max(0, rBR); rBL = max(0, rBL);

    // Top-left arc
    if (rTL > 0) p.AddArc(x, y, rTL*2.f, rTL*2.f, 180, 90);
    else         p.AddLine(PointF(x, y), PointF(x, y));

    // Top edge to top-right arc
    if (rTR > 0) p.AddArc(x+w-rTR*2.f, y, rTR*2.f, rTR*2.f, 270, 90);
    else         p.AddLine(PointF(x+w, y), PointF(x+w, y));

    // Right edge to bottom-right arc
    if (rBR > 0) p.AddArc(x+w-rBR*2.f, y+h-rBR*2.f, rBR*2.f, rBR*2.f, 0, 90);
    else         p.AddLine(PointF(x+w, y+h), PointF(x+w, y+h));

    // Bottom edge to bottom-left arc
    if (rBL > 0) p.AddArc(x, y+h-rBL*2.f, rBL*2.f, rBL*2.f, 90, 90);
    else         p.AddLine(PointF(x, y+h), PointF(x, y+h));

    p.CloseFigure();
}

void ApplyAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE cp = g_S.roundedCorners ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cp, sizeof(cp));

    COLORREF borderCol = g_S.showBorder ? DWMWA_COLOR_DEFAULT : DWMWA_COLOR_NONE;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    HMODULE hu = GetModuleHandle(L"user32.dll");
    if (!hu) return;
    auto fn = (pSetWindowCompositionAttribute)GetProcAddress(hu, "SetWindowCompositionAttribute");
    if (!fn) return;

    ACCENT_POLICY pol = {};
    WINDOWCOMPOSITIONATTRIBDATA d = { WCA_ACCENT_POLICY, &pol, sizeof(pol) };

    if (g_S.background == 1 || (g_S.background >= 3 && g_S.background <= 7) || g_S.background == 9 || g_S.background == 11 || g_S.background == 12 || g_S.background == 13) {
        pol = { ACCENT_DISABLED, 0, 0, 0 };
        fn(hwnd, &d);
        MARGINS margins = { -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    } else {
        MARGINS margins = { 0 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        if (g_S.background == 0) {
            pol = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
        } else if (g_S.background == 2 || g_S.background == 10) {
            // Unified acrylic-glass: themeBlur controls tint density, themeOpacity scales overall alpha.
            // Set ThemeBlur low + ThemeOpacity high for a frosted-glass look;
            // ThemeBlur high + ThemeOpacity lower for a dense acrylic look.
            bool light = g_S.autoTheme && IsLightCached();
            BYTE blurAlpha = (BYTE)(max(8, min(180, (int)(g_S.themeBlur * 1.4f))));
            float opacityScale = g_S.themeOpacity / 255.f;
            BYTE finalAlpha = (BYTE)(blurAlpha * opacityScale);
            DWORD baseColor = light ? 0x00FFFFFF : 0x00000000;
            DWORD tint = ((DWORD)finalAlpha << 24) | baseColor;
            pol = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
        }
        fn(hwnd, &d);
    }
}

// ── Taskbar Visibility (autohide + 4K DPI aware) ─────────────────────────────
// Instead of IsWindowVisible() which lies when the taskbar is autohidden
// off-screen, we intersect the taskbar rect with the monitor rect. If the
// visible strip is <= 4 physical pixels the taskbar is effectively hidden.
// This also naturally handles 4K/HiDPI without needing explicit DPI math.
bool IsTaskbarEffectivelyVisible(HWND hTaskbar) {
    if (!hTaskbar || !IsWindowVisible(hTaskbar)) return false;
    RECT rc; GetWindowRect(hTaskbar, &rc);
    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(hMon, &mi)) return true; // fallback: assume visible
    RECT intersect;
    if (!IntersectRect(&intersect, &rc, &mi.rcMonitor)) return false;
    int visW = intersect.right  - intersect.left;
    int visH = intersect.bottom - intersect.top;
    // Autohidden taskbar leaves a 1-2px sliver; 4px threshold is safe at any DPI
    return (visW > 4 && visH > 4);
}

void UpdateBoundsAndRegion(HWND hwnd) {
    HWND tb = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!tb) return;
    RECT tbRect; GetWindowRect(tb, &tbRect);

    UINT dpi = GetDpiForWindow(hwnd);
    if (dpi == 0) dpi = 96;
    float scale = dpi / 96.f;

    int tbX = tbRect.left, tbY = tbRect.top;
    int tbH = tbRect.bottom - tbRect.top;

    int panelH = (int)(g_S.height * scale);

    // ── Dynamic total width from wrapper + active containers ─────────────────
    int panelW;
    if (g_S.miniMode) {
        // Mini: wrapper left+right + active container widths + gap if both shown
        int totalWidth = g_S.wrapper.padLeft + g_S.wrapper.padRight;
        bool first = true;
        for (int id : {1, 4}) { // Only Media and Visualizer in mini
            if (!IsContainerEnabled(id)) continue;
            if (!first) totalWidth += g_S.wrapper.globalGap;
            totalWidth += GetContainerWidth(id);
            first = false;
        }
        panelW = (int)(totalWidth * scale);
    } else {
        // Sum active containers
        int totalWidth = g_S.wrapper.padLeft + g_S.wrapper.padRight;
        bool first = true;
        for (int id : g_S.wrapper.order) {
            if (!IsContainerEnabled(id)) continue;
            if (id == 4 && g_S.wrapper.vizAsBackground) continue;
            if (!first) totalWidth += g_S.wrapper.globalGap;
            totalWidth += GetContainerWidth(id);
            first = false;
        }
        panelW = (int)(totalWidth * scale);
    }

    int posX = tbX + (int)(g_S.offsetX * scale);
    int posY = tbY + (tbH / 2) - (panelH / 2) + (int)(g_S.offsetY * scale);

    int cr = (int)(g_S.cornerRadius * scale);
    HRGN hRgn;
    if (g_S.roundedCorners) hRgn = CreateRoundRectRgn(0, 0, panelW+1, panelH+1, cr*2, cr*2);
    else                     hRgn = CreateRectRgn(0, 0, panelW, panelH);

    SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, panelW, panelH, SWP_SHOWWINDOW);
    SetWindowRgn(hwnd, hRgn, TRUE);
    g_LayoutDirty = true;
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void DrawPanel(HDC hdc, int W, int H) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    g.Clear(Color(0,0,0,0));

    // ── Palette ───────────────────────────────────────────────────────────────
    Color bgPrimary, bgSecondary;
    {
        scoped_lock lk(g_M.mtx);
        bgPrimary   = g_M.primaryColor;
        bgSecondary = g_M.secondaryColor;
    }

    // ── Background fill ───────────────────────────────────────────────────────
    auto FillShape = [&](Brush* br) {
        if (g_S.roundedCorners) {
            GraphicsPath bp;
            CustomRoundedPath(bp, 0.f, 0.f, (float)W, (float)H,
                g_S.cornerRadius, g_S.cornerRadius,
                g_S.cornerRadius, g_S.cornerRadius);
            g.FillPath(br, &bp);
        } else {
            g.FillRectangle(br, 0, 0, W, H);
        }
    };

    // (All existing background cases preserved verbatim - only showing the switch here)
    if (g_S.background == 3) {
        Color left(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
        Color right(0,  bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
        LinearGradientBrush br(Rect(0,0,W,H), left, right, LinearGradientModeHorizontal);
        FillShape(&br);
    } else if (g_S.background == 4) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgSecondary, LinearGradientModeHorizontal);
        FillShape(&br);
    } else if (g_S.background == 12) {
        LinearGradientBrush br(Rect(0,0,W,H), bgSecondary, bgPrimary, LinearGradientModeHorizontal);
        FillShape(&br);
    } else if (g_S.background == 5) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgSecondary, LinearGradientModeVertical);
        FillShape(&br);
    } else if (g_S.background == 6) {
        GraphicsPath gp;
        if (g_S.roundedCorners)
            CustomRoundedPath(gp, 0.f, 0.f, (float)W, (float)H,
                g_S.cornerRadius, g_S.cornerRadius, g_S.cornerRadius, g_S.cornerRadius);
        else gp.AddRectangle(Rect(0, 0, W, H));
        PathGradientBrush pgb(&gp);
        int count = 1;
        pgb.SetCenterColor(bgPrimary);
        pgb.SetSurroundColors(&bgSecondary, &count);
        pgb.SetCenterPoint(PointF(W * 0.3f, H * 0.5f));
        g.FillPath(&pgb, &gp);
    } else if (g_S.background == 7) {
        LinearGradientBrush br(Rect(0,0,W,H), bgSecondary, bgPrimary, LinearGradientModeVertical);
        FillShape(&br);
    } else if (g_S.background == 9) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgSecondary, LinearGradientModeHorizontal);
        Color blend[] = { bgPrimary, bgPrimary, bgSecondary, bgSecondary };
        REAL pos[]    = { 0.0f, 0.49f, 0.51f, 1.0f };
        br.SetInterpolationColors(blend, pos, 4);
        FillShape(&br);
    } else if (g_S.background == 10) {
        BYTE neonAlpha = (BYTE)(max(10, (int)(40.f * g_S.themeOpacity / 255.f)));
        Color glassOverlay = IsLightCached()
            ? Color(neonAlpha, 255, 255, 255)
            : Color(neonAlpha, 20, 20, 20);
        SolidBrush b(glassOverlay);
        FillShape(&b);
        Pen p(bgPrimary, 2.0f);
        if (g_S.roundedCorners) {
            GraphicsPath bp;
            CustomRoundedPath(bp, 1.f, 1.f, (float)(W-2), (float)(H-2),
                max(0, g_S.cornerRadius-1), max(0, g_S.cornerRadius-1),
                max(0, g_S.cornerRadius-1), max(0, g_S.cornerRadius-1));
            g.DrawPath(&p, &bp);
        } else {
            g.DrawRectangle(&p, 1, 1, W-2, H-2);
        }
    } else if (g_S.background == 11) {
        LinearGradientBrush br(Rect(0,0,W,H), bgPrimary, bgPrimary, LinearGradientModeHorizontal);
        Color darkPrimary(255,
            max(0, (int)bgPrimary.GetR()-50),
            max(0, (int)bgPrimary.GetG()-50),
            max(0, (int)bgPrimary.GetB()-50));
        Color blend[] = { bgPrimary, bgSecondary, darkPrimary };
        REAL pos[] = { 0.0f, 0.5f, 1.0f };
        br.SetInterpolationColors(blend, pos, 3);
        FillShape(&br);
    } else if (g_S.background == 0) {
        Color bg(200, 20, 20, 20);
        SolidBrush b(bg);
        FillShape(&b);
    }

    if (g_S.background == 13) {
        scoped_lock lk(g_M.mtx);
        if (g_M.art) UpdateAlbumBlurBg(g_M.art.get(), W, H, g_ArtVersion);
        if (g_S.blurredBg) g.DrawImage(g_S.blurredBg, 0, 0);
        if (g_S.enableMask) {
            BYTE tone = g_S.isDarkCover ? 20 : 235;
            SolidBrush maskBr(Color((BYTE)g_S.maskOpacity, tone, tone, tone));
            if (g_S.roundedCorners) {
                GraphicsPath mp;
                CustomRoundedPath(mp, 0.f, 0.f, (float)W, (float)H,
                    g_S.cornerRadius, g_S.cornerRadius, g_S.cornerRadius, g_S.cornerRadius);
                g.FillPath(&maskBr, &mp);
            } else g.FillRectangle(&maskBr, 0, 0, W, H);
        }
    }

    if (g_S.enableMask && g_S.background != 13) {
        bool light = IsLightCached();
        BYTE tone = light ? 235 : 20;
        SolidBrush maskBr(Color((BYTE)g_S.maskOpacity, tone, tone, tone));
        if (g_S.roundedCorners) {
            GraphicsPath mp;
            CustomRoundedPath(mp, 0.f, 0.f, (float)W, (float)H,
                g_S.cornerRadius, g_S.cornerRadius, g_S.cornerRadius, g_S.cornerRadius);
            g.FillPath(&maskBr, &mp);
        } else g.FillRectangle(&maskBr, 0, 0, W, H);
    }

    // ── Media data ────────────────────────────────────────────────────────────
    wstring title, artist;
    bool playing = false;
    Bitmap* art  = nullptr;
    {
        scoped_lock lk(g_M.mtx);
        title  = g_M.title;
        artist = g_M.artist;
        playing= g_M.playing;
        art    = g_M.art.get();
    }

    BYTE panelAlpha = 255;
    if (g_IdleState) {
        title  = L"Title";
        artist = L"Author";
        art    = nullptr;
    }

    Color tc(TxtARGB());
    tc = Color(panelAlpha, tc.GetR(), tc.GetG(), tc.GetB());
    BYTE artistBase = IsLightCached() ? 0 : 255;
    Color artistColor(min((int)panelAlpha, 153), artistBase, artistBase, artistBase);

    // ── Ensure layout is fresh ────────────────────────────────────────────────
    if (g_LayoutDirty || g_Layout.W != W || g_Layout.H != H) RebuildLayout(W, H);

    // Unpack frequently-used layout values
    const int   aX           = g_Layout.aX;
    const int   aY           = g_Layout.aY;
    const int   aS           = g_Layout.aS;
    const int   aW           = g_Layout.aW;
    const int   aR           = g_Layout.aR;
    const float barZoneY     = g_Layout.barZoneY;
    const float barZoneH     = g_Layout.barZoneH;
    const float barW         = g_Layout.barW;
    const float barStep      = g_Layout.barStep;
    const float startX       = g_Layout.startX;
    const float maxBarH      = g_Layout.maxBarH;
    const float minBarH      = g_Layout.minBarH;
    const float nX           = g_Layout.ctrl.nX;
    const float ppX          = g_Layout.ctrl.ppX;
    const float pX           = g_Layout.ctrl.pX;
    const float vX           = g_Layout.ctrl.vX;
    const float cY           = g_Layout.ctrl.cY;
    const float shuffleX     = g_Layout.ctrl.shuffleX;
    const float repeatX      = g_Layout.ctrl.repeatX;
    const float firstControlX= g_Layout.ctrl.firstControlX;

    float _scale    = H / 52.f;
    float iconScale = (g_S.iconSize / 14.f) * _scale;
    const float ppBigR   = 14.f * iconScale;
    const float pnSmallR =  9.f * iconScale;

    // ── Gradient LerpDWORD helper (used in both Viz and Info lambdas) ─────────
    auto LerpDWORD = [](DWORD c1, DWORD c2, float t) -> Color {
        BYTE r1=(c1>>16)&0xFF, g1=(c1>>8)&0xFF, b1=c1&0xFF;
        BYTE r2=(c2>>16)&0xFF, g2=(c2>>8)&0xFF, b2=c2&0xFF;
        return Color(255, (BYTE)(r1+(r2-r1)*t), (BYTE)(g1+(g2-g1)*t), (BYTE)(b1+(b2-b1)*t));
    };

    // ── Hover-color helpers (Controls container uses these too) ───────────────
    Color targetHoverColor;
    if (g_S.dynamicHover)
        targetHoverColor = Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    else
        targetHoverColor = Color(255,
            (g_S.hoverColor>>16)&0xFF,
            (g_S.hoverColor>>8)&0xFF,
             g_S.hoverColor    &0xFF);

    auto LerpColor = [](Color a, Color b, float t) -> Color {
        return Color(255,
            (BYTE)(a.GetR() + (b.GetR()-a.GetR())*t),
            (BYTE)(a.GetG() + (b.GetG()-a.GetG())*t),
            (BYTE)(a.GetB() + (b.GetB()-a.GetB())*t));
    };
    // ── Controls icon base color (respects custom/dynamic icon color settings) ─
    Color ctrlBaseColor;
    if (g_S.ctrlIconDynamic)
        ctrlBaseColor = Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    else if (g_S.ctrlIconColor != 0xFF000000) // user set a custom color
        ctrlBaseColor = Color(255,
            (BYTE)((g_S.ctrlIconColor>>16)&0xFF),
            (BYTE)((g_S.ctrlIconColor>>8)&0xFF),
            (BYTE)(g_S.ctrlIconColor&0xFF));
    else
        ctrlBaseColor = tc; // fallback to text color

    auto IB = [&](int btn) -> Color {
        float t = g_HoverProgress[btn];
        if (t <= 0.f) return ctrlBaseColor;
        if (t >= 1.f) return targetHoverColor;
        return LerpColor(ctrlBaseColor, targetHoverColor, t);
    };

    // =========================================================================
    // C-2  Visualizer as BACKGROUND LAYER (z-order: rendered before containers)
    // =========================================================================
    if (g_S.showVisualizer && g_S.wrapper.vizAsBackground && !g_S.miniMode) {
        // Full-panel background layer: same geometry as art square but stretched
        int barCount = max(1, g_S.vizBars);
        float idleH  = g_S.idleBarSize / 100.f;
        Color baseBarCol;
        if (g_S.vizMode == 1)
            baseBarCol = Color(panelAlpha, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
        else
            baseBarCol = Color(panelAlpha,
                (BYTE)((g_S.vizColor>>16)&0xFF),
                (BYTE)((g_S.vizColor>>8)&0xFF),
                (BYTE)(g_S.vizColor&0xFF));

        SolidBrush loopBrush(baseBarCol);
        GraphicsPath bp2;
        // Use full-panel bar zone
        float bgZoneY = (float)aY;
        float bgZoneW = (float)W,  bgZoneH = (float)aS;
        float bgGap   = max(0.f, (float)g_S.vizBarGap);
        float bgBarW  = max(1.5f, (bgZoneW - bgGap*(barCount-1)) / barCount);
        float bgStep  = bgBarW + bgGap;
        float bgStartX= (bgZoneW - (bgStep*barCount - bgGap)) * 0.5f;

        for (int i = 0; i < barCount; i++) {
            float barFactor = max(idleH, g_VizPeak[i]);
            float bh = (bgZoneH * 0.035f) + barFactor * (bgZoneH * 0.92f - bgZoneH * 0.035f);
            float bx = bgStartX + i * bgStep;
            float by;
            if      (g_S.vizAnchor == 0) by = bgZoneY;
            else if (g_S.vizAnchor == 2) by = bgZoneY + bgZoneH - bh;
            else                          by = bgZoneY + (bgZoneH - bh) / 2.f;

            int br2 = max(1, (int)(bgBarW * 0.5f));
            bp2.Reset();
            RoundRect(bp2, (int)bx, (int)by, (int)bgBarW, (int)bh, br2);

            if (g_S.vizMode == 3) {
                float t = (barCount > 1) ? (float)i / (barCount-1) : 0.f;
                Color gc = LerpDWORD(g_S.vizColor1, g_S.vizColor2, t);
                loopBrush.SetColor(Color(panelAlpha, gc.GetR(), gc.GetG(), gc.GetB()));
            } else {
                loopBrush.SetColor(baseBarCol);
            }
            g.FillPath(&loopBrush, &bp2);
        }
    }

    // =========================================================================
    // C-3  Sequential container rendering loop
    // =========================================================================
    for (int ci = 0; ci < g_OriginCount; ci++) {
        const ContainerOrigin& co = g_Origins[ci];

        // ── Container 1: MEDIA (Album Art) ────────────────────────────────────
        if (co.id == 1) {
            GraphicsPath ap;
            CustomRoundedPath(ap, (float)aX, (float)aY, (float)aW, (float)aS, aR, aR, aR, aR);

            auto DrawArtOrPlaceholder = [&]() {
                if (art) {
                    g.SetClip(&ap);
                    g.DrawImage(art, aX, aY, aW, aS);
                    g.ResetClip();
                } else {
                    bool _light = IsLightCached();
                    Color phBg = _light ? Color(panelAlpha, 200,200,200) : Color(panelAlpha, 55,55,60);
                    Color phFg = _light ? Color(panelAlpha, 130,130,135) : Color(panelAlpha, 100,100,108);
                    SolidBrush bgBr(phBg); g.FillPath(&bgBr, &ap);
                    int minDim = min(aW, aS);
                    if (minDim >= 14) {
                        FontFamily noteFF(ICON_FONT, nullptr);
                        Font noteF(&noteFF, (REAL)(minDim * 0.45f), FontStyleRegular, UnitPixel);
                        RectF noteBounds(0,0,4000,200), noteMeas;
                        const wchar_t* noteGlyph = L"\uE8D6";
                        g.MeasureString(noteGlyph, -1, &noteF, noteBounds, &noteMeas);
                        float nX2 = aX + (aW - noteMeas.Width)  / 2.f;
                        float nY2 = aY + (aS - noteMeas.Height) / 2.f;
                        SolidBrush fgBr(phFg);
                        g.DrawString(noteGlyph, -1, &noteF, PointF(nX2, nY2), &fgBr);
                    }
                }
            };

            DrawArtOrPlaceholder();
            continue;
        }

        // ── Container 4: VISUALIZER (sequential slot) ─────────────────────────
        if (co.id == 4) {
            // In mini mode the viz always renders in its own slot.
            // In normal mode, skip if it's set to the full-width background layer.
            if (!g_S.showVisualizer) continue;
            if (!g_S.miniMode && g_S.wrapper.vizAsBackground) continue;

            // Use barZone from layout (already computed from this slot's x/w)
            int barCount       = max(1, g_S.vizBars);
            float idleH        = g_S.idleBarSize / 100.f;
            Color baseBarCol;
            if (g_S.vizMode == 1)
                baseBarCol = Color(panelAlpha, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
            else
                baseBarCol = Color(panelAlpha,
                    (BYTE)((g_S.vizColor>>16)&0xFF),
                    (BYTE)((g_S.vizColor>>8)&0xFF),
                    (BYTE)(g_S.vizColor&0xFF));

            SolidBrush loopBrush(baseBarCol);
            GraphicsPath bp;

            for (int i = 0; i < barCount; i++) {
                float barFactor = max(idleH, g_VizPeak[i]);
                float bh = minBarH + barFactor * (maxBarH - minBarH);
                float bx = startX + i * barStep;
                float by;
                if      (g_S.vizAnchor == 0) by = barZoneY;
                else if (g_S.vizAnchor == 2) by = barZoneY + barZoneH - bh;
                else                          by = barZoneY + (barZoneH - bh) / 2.f;

                int br2 = max(1, (int)(barW * 0.5f));
                bp.Reset();
                RoundRect(bp, (int)bx, (int)by, (int)barW, (int)bh, br2);

                if (g_S.vizMode == 2) {
                    float freqT = (float)i / (barCount-1);
                    float t     = freqT * 0.6f + barFactor * 0.4f;
                    Color gc(panelAlpha,
                        (BYTE)(bgPrimary.GetR() + (int)(bgSecondary.GetR()-bgPrimary.GetR())*t),
                        (BYTE)(bgPrimary.GetG() + (int)(bgSecondary.GetG()-bgPrimary.GetG())*t),
                        (BYTE)(bgPrimary.GetB() + (int)(bgSecondary.GetB()-bgPrimary.GetB())*t));
                    loopBrush.SetColor(gc);
                    g.FillPath(&loopBrush, &bp);
                } else if (g_S.vizMode == 3) {
                    float t = (barCount > 1) ? (float)i / (barCount-1) : 0.f;
                    Color gc = LerpDWORD(g_S.vizColor1, g_S.vizColor2, t);
                    loopBrush.SetColor(Color(panelAlpha, gc.GetR(), gc.GetG(), gc.GetB()));
                    g.FillPath(&loopBrush, &bp);
                } else if (g_S.vizMode == 4) {
                    BYTE acrAlpha = (BYTE)(max(30, min(180, (int)(panelAlpha*0.55f*barFactor+30))));
                    Color ac(acrAlpha, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
                    loopBrush.SetColor(ac);
                    g.FillPath(&loopBrush, &bp);
                    Color rimCol(min(255, acrAlpha+60),
                        min(255,(int)ac.GetR()+80),
                        min(255,(int)ac.GetG()+80),
                        min(255,(int)ac.GetB()+80));
                    Pen rimPen(rimCol, 0.8f);
                    g.DrawPath(&rimPen, &bp);
                } else {
                    loopBrush.SetColor(baseBarCol);
                    g.FillPath(&loopBrush, &bp);
                }
            }
            continue;
        }

        // ── Container 2: INFO (Title + Artist [+ Progress bar under artist]) ──
        if (co.id == 2 && !g_S.miniMode) {
            // tX: cell left edge + container padLeft (internal inset).
            // artTextGap shrinks tW from the left so the gap between art and text
            // is respected without shifting the text's starting position.
            int infoPadL = g_S.containers[1].padLeft;
            int infoPadR = g_S.containers[1].padRight;
            int tX = co.cellX + infoPadL;
            int tW = co.cellWidth - infoPadL - infoPadR - g_S.artTextGap;
            if (tW < 8) tW = 8;

            FontFamily ff(FONT_NAME, nullptr);
            FontFamily artFF(L"Segoe UI Semibold", nullptr);
            Font titleF(&ff,   (REAL)(g_S.fontSize+1), FontStyleBold,    UnitPixel);
            Font artF  (&artFF,(REAL)(g_S.fontSize-1), FontStyleRegular, UnitPixel);

            RectF lay(0,0,4000,200), tb2, ab2;
            g.MeasureString(title.c_str(),  -1, &titleF, lay, &tb2);
            if (!artist.empty()) g.MeasureString(artist.c_str(), -1, &artF, lay, &ab2);

            float gap     = (float)g_S.textGap;
            float totalH  = tb2.Height + (artist.empty() ? 0.f : ab2.Height + gap);

            // Reserve vertical space for progress bar only when it draws
            // inside the info container area (under_text=2, under_text_only=3).
            // under_both(0), above_both(1), under_controls_only(4) don't affect text position.
            float pbReserve = 0.f;
            if (g_S.showProgressBar &&
                (g_S.progressBarLayer == 2 || g_S.progressBarLayer == 3))
                pbReserve = (float)g_S.progressBarHeight + 4.f;

            // Vertical center within wrapper padding (not container-local)
            float sY = g_S.wrapper.padTop
                + (H - g_S.wrapper.padTop - g_S.wrapper.padBottom - totalH - pbReserve) / 2.f;

            Region clip(Rect(tX, 0, tW, H));
            g.SetClip(&clip);

            g_ScTxtW = (int)tb2.Width;
            g_ScVisW = tW;
            g_Scroll = (!g_S.disableScroll && g_ScTxtW > tW);

            // When scrolling is disabled and text overflows, build a truncated
            // string ending in "…" that fits within tW.
            auto TruncateToFit = [&](const wstring& str, const Font& fnt, int maxW) -> wstring {
                if (maxW <= 0) return L"…";
                RectF bounds(0,0,4000,200), meas;
                g.MeasureString(str.c_str(), -1, &fnt, bounds, &meas);
                if ((int)meas.Width <= maxW) return str;
                // Binary-search for the longest prefix that fits with "…" appended
                int lo = 0, hi = (int)str.size();
                while (lo < hi) {
                    int mid = (lo + hi + 1) / 2;
                    wstring candidate = str.substr(0, mid) + L"…";
                    g.MeasureString(candidate.c_str(), -1, &fnt, bounds, &meas);
                    if ((int)meas.Width <= maxW) lo = mid;
                    else                          hi = mid - 1;
                }
                return str.substr(0, lo) + L"…";
            };

            wstring displayTitle  = (g_S.disableScroll && g_ScTxtW > tW)
                ? TruncateToFit(title, titleF, tW) : title;
            RectF ab2_artist; // re-measure artist for layout
            if (!artist.empty()) g.MeasureString(artist.c_str(), -1, &artF, RectF(0,0,4000,200), &ab2_artist);
            wstring displayArtist = (g_S.disableScroll && !artist.empty() && (int)ab2_artist.Width > tW)
                ? TruncateToFit(artist, artF, tW) : artist;

            Color tcFaded(max(0, (int)(tc.GetA() * g_TextFadeAlpha)),
                tc.GetR(), tc.GetG(), tc.GetB());
            Color acFaded(max(0, (int)(artistColor.GetA() * g_TextFadeAlpha)),
                artistColor.GetR(), artistColor.GetG(), artistColor.GetB());
            SolidBrush tb3(tcFaded), ab3(acFaded);

            if (g_Scroll) {
                g.DrawString(displayTitle.c_str(), -1, &titleF,
                    PointF((float)tX - g_ScOff, sY), &tb3);
            } else {
                g_ScOff = 0; g_ScDir = true;
                g.DrawString(displayTitle.c_str(), -1, &titleF,
                    PointF((float)tX, sY), &tb3);
            }
            if (!artist.empty())
                g.DrawString(displayArtist.c_str(), -1, &artF,
                    PointF((float)tX, sY + tb2.Height + gap), &ab3);

            g.ResetClip();

            // Fade edge for Windows-adaptive theme
            if (g_S.background == 0 && tW > 20) {
                int fw=24, fx=tX+tW-fw;
                Color c0(0, 20,20,20), c1(200,20,20,20);
                LinearGradientBrush fade(PointF((float)fx,0),PointF((float)(fx+fw),0),c0,c1);
                g.FillRectangle(&fade, fx, 0, fw, H);
            }

            // ── Progress bar under artist line ────────────────────────────────
            if (g_S.showProgressBar) {
                float progress = GetLiveProgress();
                if (g_PosIsPlaying && g_hWnd && !g_AnimTimerRunning) {
                    SetTimer(g_hWnd, IDT_ANIM, 16, NULL);
                    g_AnimTimerRunning = true;
                }
                Color pbFillCol;
                if (g_S.progressBarDynamic) {
                    pbFillCol = Color(220, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
                } else {
                    pbFillCol = Color(220,
                        (BYTE)((g_S.progressBarColor>>16)&0xFF),
                        (BYTE)((g_S.progressBarColor>>8)&0xFF),
                        (BYTE)(g_S.progressBarColor&0xFF));
                }

                int pbH = g_S.progressBarHeight;
                int pbY, pbX, pbW;

                switch (g_S.progressBarLayer) {
                    case 1: // above_both
                        pbY = 0 + g_S.progressBarOffsetY;
                        pbX = 0; pbW = W;
                        break;
                    case 2: // under_text: spans info+controls area
                        pbX = aX + aW + g_S.progressBarPadLeft;
                        pbW = W - pbX - g_S.progressBarPadRight;
                        pbY = H - pbH - 4 - g_S.progressBarOffsetY;
                        break;
                    case 3: // under text box only
                        pbX = tX + g_S.progressBarPadLeft;
                        pbW = tW - g_S.progressBarPadLeft - g_S.progressBarPadRight;
                        pbY = H - pbH - 4 - g_S.progressBarOffsetY;
                        break;
                    case 4: // under controls only
                        pbX = (int)(firstControlX - pnSmallR) + g_S.progressBarPadLeft;
                        pbW = (int)(nX + pnSmallR) - pbX - g_S.progressBarPadRight;
                        pbY = H - pbH - 4 - g_S.progressBarOffsetY;
                        break;
                    default: // under_both
                        pbY = H - pbH - g_S.progressBarOffsetY;
                        pbX = 0; pbW = W;
                        break;
                }

                if (pbW > 4) {
                    Color trackCol(35, tc.GetR(), tc.GetG(), tc.GetB());
                    SolidBrush trackBr(trackCol);
                    GraphicsPath trackPath;
                    RoundRect(trackPath, pbX, pbY, pbW, pbH, pbH/2);
                    g.FillPath(&trackBr, &trackPath);

                    int filledW = (int)(progress * pbW);
                    if (filledW > 1) {
                        SolidBrush fillBr(pbFillCol);
                        GraphicsPath fillPath;
                        RoundRect(fillPath, pbX, pbY, filledW, pbH, pbH/2);
                        g.FillPath(&fillBr, &fillPath);
                    }
                }
            }
            continue;
        }

        // ── Container 3: CONTROLS ─────────────────────────────────────────────
        if (co.id == 3 && !g_S.miniMode) {

            // Smooth hover circles
            for (int i = 1; i <= 6; i++) {
                if (g_HoverProgress[i] <= 0.f) continue;
                float hX = 0;
                float hR = (i == 2) ? ppBigR : (pnSmallR + 2.f);
                if      (i == 1) hX = pX;
                else if (i == 2) hX = ppX;
                else if (i == 3) hX = nX;
                else if (i == 5) { if (shuffleX > 0.f) hX = shuffleX; else continue; }
                else if (i == 6) { if (repeatX  > 0.f) hX = repeatX;  else continue; }
                else continue;
                int alpha = (int)(28.f * g_HoverProgress[i]);
                SolidBrush hb(Color(alpha, ctrlBaseColor.GetR(), ctrlBaseColor.GetG(), ctrlBaseColor.GetB()));
                float animR = hR * (0.8f + 0.2f * g_HoverProgress[i]);
                g.FillEllipse(&hb, hX-animR, cY-animR, animR*2.f, animR*2.f);
            }

            // Icon drawing helpers (same lambdas as original)
            auto DrawIcon = [&](int type, float cx, float cy, float s, Color col) {
                SolidBrush b(col);
                auto X   = [&](float v){ return cx + v*s; };
                auto Y   = [&](float v){ return cy + v*s; };
                auto W_f = [&](float v){ return v*s; };

                if (type == 1) {
                    PointF pts[] = { {X(4.5f),Y(-5.f)},{X(-2.5f),Y(0)},{X(4.5f),Y(5.f)} };
                    g.FillPolygon(&b, pts, 3);
                    g.FillRectangle(&b, X(-5.5f), Y(-5.f), W_f(3.f), W_f(10.f));
                } else if (type == 2) {
                    PointF pts[] = { {X(-2.5f),Y(-6.f)},{X(6.5f),Y(0)},{X(-2.5f),Y(6.f)} };
                    g.FillPolygon(&b, pts, 3);
                } else if (type == 3) {
                    g.FillRectangle(&b, X(-4.f), Y(-6.f), W_f(3.f), W_f(12.f));
                    g.FillRectangle(&b, X( 1.f), Y(-6.f), W_f(3.f), W_f(12.f));
                } else if (type == 4) {
                    PointF pts[] = { {X(-4.5f),Y(-5.f)},{X(2.5f),Y(0)},{X(-4.5f),Y(5.f)} };
                    g.FillPolygon(&b, pts, 3);
                    g.FillRectangle(&b, X(2.5f), Y(-5.f), W_f(3.f), W_f(10.f));
                } else if (type == 5) {
                    PointF cone[] = {
                        {X(-7.f),Y(-3.f)},{X(-4.f),Y(-3.f)},
                        {X(0.f),Y(-7.f)},{X(0.f),Y(7.f)},
                        {X(-4.f),Y(3.f)},{X(-7.f),Y(3.f)}
                    };
                    g.FillPolygon(&b, cone, 6);
                    Pen p2(col, W_f(1.5f));
                    p2.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
                    g.DrawArc(&p2, X(-5.5f), Y(-4.f), W_f(8.f),  W_f(8.f),  -50, 100);
                    g.DrawArc(&p2, X(-8.5f), Y(-7.f), W_f(14.f), W_f(14.f), -50, 100);
                }
            };

            if (g_S.iconTheme >= 1 && g_S.iconTheme <= 4) {
                // ── Glyph themes ──────────────────────────────────────────────
                const WCHAR* glyphFont = (g_S.iconTheme >= 2)
                    ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";

                const wchar_t* glyphPrev, *glyphPlay, *glyphPause, *glyphNext;
                if (g_S.iconTheme == 1) {
                    glyphPrev=L"\uE892"; glyphPlay=L"\uE768"; glyphPause=L"\uE769"; glyphNext=L"\uE893";
                } else if (g_S.iconTheme == 2) {
                    glyphPrev=L"\uE892"; glyphPlay=L"\uE768"; glyphPause=L"\uE769"; glyphNext=L"\uE893";
                } else if (g_S.iconTheme == 3) {
                    glyphPrev=L"\uF8AC"; glyphPlay=L"\uF5B0"; glyphPause=L"\uF8AE"; glyphNext=L"\uF8AD";
                } else {
                    glyphPrev=L"\uEB9E"; glyphPlay=L"\uEE4A"; glyphPause=L"\uEDB4"; glyphNext=L"\uEB9D";
                }

                const wchar_t* glyphVolume;
                if      (g_Volume <= 0.f)   glyphVolume = L"\uE74F";
                else if (g_Volume < 0.34f)  glyphVolume = L"\uE993";
                else if (g_Volume < 0.67f)  glyphVolume = L"\uE994";
                else                         glyphVolume = L"\uE995";

                FontFamily iconFF(glyphFont, nullptr);
                float gSize = 14.f * iconScale;
                Font iconFont(&iconFF, gSize, FontStyleRegular, UnitPixel);

                auto DrawGlyph = [&](const wchar_t* glyph, float cx, Color col) {
                    SolidBrush br(col);
                    RectF bounds(0,0,4000,200), measured;
                    g.MeasureString(glyph, -1, &iconFont, bounds, &measured);
                    g.DrawString(glyph, -1, &iconFont,
                        PointF(cx - measured.Width/2.f, cY - measured.Height/2.f), &br);
                };

                if (g_S.showPlaybackControls) {
                    DrawGlyph(glyphPrev, pX, IB(1));
                    DrawGlyph(playing ? glyphPause : glyphPlay, ppX, IB(2));
                    DrawGlyph(glyphNext, nX, IB(3));
                }
                if (g_S.showSpeakerIcon) DrawGlyph(glyphVolume, vX, IB(4));

                // Shuffle / Repeat glyphs - codepoints per icon theme
                const wchar_t* glyphShuffle, *glyphRepeatNone, *glyphRepeatTrack, *glyphRepeatList;
                if (g_S.iconTheme == 3) {
                    // Segoe Fluent Icons - Filled (different codepoints for filled variants)
                    glyphShuffle     = L"\xF5E7";
                    glyphRepeatNone  = L"\xF8A8";
                    glyphRepeatTrack = L"\xF8A7";
                    glyphRepeatList  = L"\xF8A8";
                } else {
                    // MDL2 Assets (1), Fluent Outlined (2), Fluent Outlined Alt (4) - same codepoints
                    glyphShuffle     = L"\xE8B1";
                    glyphRepeatNone  = L"\xE8EE";
                    glyphRepeatTrack = L"\xE8ED";
                    glyphRepeatList  = L"\xE8EE";
                }

                bool shOn = false; int repMode = 0;
                { std::scoped_lock lk2(g_M.mtx); shOn = g_M.shuffleOn; repMode = g_M.repeatMode; }

                if (g_S.showShuffleButton && shuffleX > 0.f) {
                    Color shCol = shOn ? targetHoverColor : IB(5);
                    DrawGlyph(glyphShuffle, shuffleX, shCol);
                    if (shOn) {
                        float dotR = 1.8f * iconScale;
                        float dotY = cY + 10.f * iconScale;
                        SolidBrush dotBr(targetHoverColor);
                        g.FillEllipse(&dotBr, shuffleX-dotR, dotY-dotR, dotR*2.f, dotR*2.f);
                    }
                }
                if (g_S.showRepeatButton && repeatX > 0.f) {
                    const wchar_t* rg = (repMode==1)?glyphRepeatTrack:(repMode==2)?glyphRepeatList:glyphRepeatNone;
                    Color repCol = (repMode>0) ? targetHoverColor : IB(6);
                    DrawGlyph(rg, repeatX, repCol);
                    if (repMode > 0) {
                        float dotR = 1.8f * iconScale;
                        float dotY = cY + 10.f * iconScale;
                        SolidBrush dotBr(targetHoverColor);
                        g.FillEllipse(&dotBr, repeatX-dotR, dotY-dotR, dotR*2.f, dotR*2.f);
                    }
                }
            } else {
                // ── Default custom-drawn icons ─────────────────────────────────
                if (g_S.showPlaybackControls) {
                    DrawIcon(playing ? 3 : 2, ppX, cY, 1.15f*iconScale, IB(2));
                    DrawIcon(1, pX, cY, 0.9f*iconScale, IB(1));
                    DrawIcon(4, nX, cY, 0.9f*iconScale, IB(3));
                }
            }

            // ── Volume slider ──────────────────────────────────────────────────
            if (g_S.showSpeakerIcon) {
                if (g_S.iconTheme == 0) DrawIcon(5, vX, cY, 0.9f*iconScale, IB(4));

                if (g_VolumeHover) {
                    float sliderBgX = vX + 18.f;
                    float sliderBgW = 74.f, sliderBgH = 12.f;
                    float sliderBgY = cY - sliderBgH/2.f;
                    GraphicsPath vp;
                    RoundRect(vp, sliderBgX, sliderBgY, sliderBgW, sliderBgH, sliderBgH/2.f);
                    Color popupBg = IsLightCached()
                        ? Color(230, 245,245,245) : Color(200, 30,30,30);
                    SolidBrush pBr(popupBg); g.FillPath(&pBr, &vp);

                    float sliderW = 60.f, sliderH = 4.f;
                    float sliderX = sliderBgX + 7.f;
                    float sliderTop = cY - sliderH/2.f;
                    SolidBrush track(Color(60, tc.GetR(), tc.GetG(), tc.GetB()));
                    GraphicsPath trackP;
                    RoundRect(trackP, sliderX, sliderTop, sliderW, sliderH, sliderH/2.f);
                    g.FillPath(&track, &trackP);

                    float filledW2 = g_Volume * sliderW;
                    SolidBrush fill(tc);
                    if (filledW2 > 0) {
                        GraphicsPath fillP;
                        RoundRect(fillP, sliderX, sliderTop, filledW2, sliderH, sliderH/2.f);
                        g.FillPath(&fill, &fillP);
                    }
                    float thumbR = 4.f, thumbX = sliderX + filledW2 - thumbR;
                    g.FillEllipse(&fill, thumbX, cY-thumbR, thumbR*2.f, thumbR*2.f);
                }
            }

            // ── Default-theme shuffle/repeat icons (drawn after ResetClip) ─────
            if (g_S.iconTheme == 0) {
                bool shOn2 = false; int repMode2 = 0;
                { std::scoped_lock lk2(g_M.mtx); shOn2 = g_M.shuffleOn; repMode2 = g_M.repeatMode; }

                if (g_S.showShuffleButton && shuffleX > 0.f) {
                    Color shCol = shOn2 ? targetHoverColor : IB(5);
                    float ss = 0.75f * iconScale;
                    Pen shPen(shCol, 1.5f*ss);
                    shPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
                    g.DrawLine(&shPen, shuffleX-5.f*ss, cY-4.f*ss, shuffleX+5.f*ss, cY+4.f*ss);
                    g.DrawLine(&shPen, shuffleX-5.f*ss, cY+4.f*ss, shuffleX+5.f*ss, cY-4.f*ss);
                    PointF at2[] = { {shuffleX+3.f*ss,cY-6.f*ss},{shuffleX+5.f*ss,cY-4.f*ss},{shuffleX+5.f*ss,cY-2.f*ss} };
                    g.DrawLines(&shPen, at2, 3);
                    PointF ab3[] = { {shuffleX+3.f*ss,cY+6.f*ss},{shuffleX+5.f*ss,cY+4.f*ss},{shuffleX+5.f*ss,cY+2.f*ss} };
                    g.DrawLines(&shPen, ab3, 3);
                    if (shOn2) {
                        float dotR = 2.5f*iconScale;
                        float dotY = (float)H - dotR*2.f - 2.f;
                        SolidBrush dotBr(targetHoverColor);
                        g.FillEllipse(&dotBr, shuffleX-dotR, dotY, dotR*2.f, dotR*2.f);
                    }
                }
                if (g_S.showRepeatButton && repeatX > 0.f) {
                    Color repCol = (repMode2>0) ? targetHoverColor : IB(6);
                    Pen repPen(repCol, 1.5f*iconScale);
                    repPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
                    float rr = 5.5f*iconScale;
                    g.DrawArc(&repPen, repeatX-rr, cY-rr, rr*2.f, rr*2.f, 30, 300);
                    PointF ra2[] = { {repeatX+rr*0.5f,cY-rr},{repeatX+rr,cY-rr*0.5f},{repeatX+rr*0.2f,cY-rr*0.2f} };
                    g.DrawLines(&repPen, ra2, 3);
                    if (repMode2 == 1) {
                        FontFamily ff1(L"Segoe UI Variable Display", nullptr);
                        Font f1(&ff1, 5.5f*iconScale, FontStyleBold, UnitPixel);
                        SolidBrush rb(repCol);
                        g.DrawString(L"1", -1, &f1,
                            PointF(repeatX-2.5f*iconScale, cY-3.5f*iconScale), &rb);
                    }
                    if (repMode2 > 0) {
                        float dotR = 2.5f*iconScale;
                        float dotY = (float)H - dotR*2.f - 2.f;
                        SolidBrush dotBr(repCol);
                        g.FillEllipse(&dotBr, repeatX-dotR, dotY, dotR*2.f, dotR*2.f);
                    }
                }
            }
            continue;
        }

    } // end container loop

    // ── Debug: container border outlines ─────────────────────────────────────
    // Draws a thin 1px border around each active container cell so you can see
    // exact boundaries and spacing. Toggle with ShowContainerBorders in Panel settings.
    if (g_S.showContainerBorders) {
        // One distinct colour per container id so they're easy to tell apart
        static const Color kBorderColors[5] = {
            Color(0,0,0,0),           // id 0 unused
            Color(255, 255, 80,  80), // id 1 Media   – red
            Color(255,  80, 200, 80), // id 2 Info    – green
            Color(255,  80, 140, 255),// id 3 Controls – blue
            Color(255, 255, 180,  40),// id 4 Visualizer – orange
        };
        for (int ci = 0; ci < g_OriginCount; ci++) {
            const ContainerOrigin& co = g_Origins[ci];
            if (co.id < 1 || co.id > 4) continue;
            Pen borderPen(kBorderColors[co.id], 1.f);
            // Draw full cell rect (cellX, 0, cellWidth, H)
            g.DrawRectangle(&borderPen, co.cellX, 0, co.cellWidth - 1, H - 1);
            // Also draw the inner content area (after padLeft/padRight) as a dashed line
            borderPen.SetDashStyle(DashStyleDash);
            int padL = 0, padR = 0;
            for (auto& c : g_S.containers)
                if (c.id == co.id) { padL = c.padLeft; padR = c.padRight; break; }
            int innerX = co.cellX + padL;
            int innerW = co.cellWidth - padL - padR;
            if (innerW > 0)
                g.DrawRectangle(&borderPen, innerX, 0, innerW - 1, H - 1);
        }
    }


} // end DrawPanel

// ── Cached back-buffer ────────────────────────────────────────────────────────
// Recreated only when panel size changes, not every frame.
static HBITMAP g_hBackBmp = nullptr;
static HDC     g_hBackDC  = nullptr;
static int     g_BackW    = 0;
static int     g_BackH    = 0;

static void EnsureBackBuffer(HDC hdc, int w, int h) {
    if (g_hBackDC && g_BackW == w && g_BackH == h) return;
    if (g_hBackBmp) { SelectObject(g_hBackDC, (HBITMAP)nullptr); DeleteObject(g_hBackBmp); g_hBackBmp = nullptr; }
    if (g_hBackDC)  { DeleteDC(g_hBackDC); g_hBackDC = nullptr; }
    g_hBackDC = CreateCompatibleDC(hdc);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
    void* bits;
    g_hBackBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    SelectObject(g_hBackDC, g_hBackBmp);
    g_BackW = w; g_BackH = h;
}

static void DestroyBackBuffer() {
    if (g_hBackBmp) { DeleteObject(g_hBackBmp); g_hBackBmp = nullptr; }
    if (g_hBackDC)  { DeleteDC(g_hBackDC);      g_hBackDC  = nullptr; }
    g_BackW = g_BackH = 0;
}

// ── Hook ──────────────────────────────────────────────────────────────────────
bool IsTbWnd(HWND h){ WCHAR c[64]{}; GetClassNameW(h,c,64); return !wcscmp(c,L"Shell_TrayWnd"); }
void CALLBACK TbEvt(HWINEVENTHOOK,DWORD,HWND h,LONG,LONG,DWORD,DWORD) { if(IsTbWnd(h)&&g_hWnd) PostMessage(g_hWnd,WM_APP+10,0,0); }

void HookTaskbar(HWND hwnd){
    HWND tb=FindWindow(L"Shell_TrayWnd",nullptr);
    if(tb){ DWORD pid=0,tid=GetWindowThreadProcessId(tb,&pid);
        if(tid) g_TbHook=SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,EVENT_OBJECT_LOCATIONCHANGE,
            nullptr,TbEvt,pid,tid,WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS); }
    UpdateBoundsAndRegion(hwnd);
}

// ── Window Proc ───────────────────────────────────────────────────────────────
#define WM_APPCLOSE WM_APP

struct FocusData { const wstring* aumid; HWND found; };

// This helper function identifies the media app's window
static BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM lp) {
    auto* fd = reinterpret_cast<FocusData*>(lp);
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);

    if (pid && IsWindowVisible(h) && GetWindow(h, GW_OWNER) == nullptr) {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProc) {
            WCHAR path[MAX_PATH] = {}; DWORD sz = MAX_PATH;
            if (QueryFullProcessImageNameW(hProc, 0, path, &sz)) {
                wstring exe = path;
                size_t sl = exe.find_last_of(L"\\");
                if (sl != wstring::npos) exe = exe.substr(sl + 1);
                size_t dot = exe.find_last_of(L".");
                if (dot != wstring::npos) exe = exe.substr(0, dot);

                wstring lAumid = *fd->aumid, lExe = exe;
                transform(lAumid.begin(), lAumid.end(), lAumid.begin(), ::towlower);
                transform(lExe.begin(), lExe.end(), lExe.begin(), ::towlower);

                if (lAumid.find(lExe) != wstring::npos || lExe.find(lAumid) != wstring::npos) {
                    fd->found = h;
                    CloseHandle(hProc);
                    return FALSE; // Found it, stop searching
                }
            }
            CloseHandle(hProc);
        }
    }
    return TRUE; // Keep looking
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
    switch(msg){
    case WM_CREATE:
        ApplyAppearance(hwnd);
        if (g_S.hideFullscreen) SetTimer(hwnd, IDT_FULLSCREEN, 100, NULL);
        HookTaskbar(hwnd);
        FetchMedia();
        // OPT: set adaptive timer rates AFTER FetchMedia so we know whether
        // playback is active; avoids spinning IDT_POS at 250 ms when paused at startup.
        ApplyTimerRates(hwnd);
        if (g_S.showVisualizer) {
            StartCaptureThread();
            SetTimer(hwnd, IDT_VIZ, 16, NULL); g_VizTimerRunning = true;
        }
        return 0;
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE:      return 0;
    case WM_APPCLOSE:   DestroyWindow(hwnd); return 0;
    case WM_DESTROY:
        // OPT: kill ALL timers before anything else - stray WM_TIMER on a destroyed
        // window is UB and can crash explorer.exe.
        KillTimer(hwnd, IDT_POLL);
        KillTimer(hwnd, IDT_POS);
        KillTimer(hwnd, IDT_ANIM);
        KillTimer(hwnd, IDT_VIZ);
        KillTimer(hwnd, IDT_HOVER_ANIM);
        KillTimer(hwnd, IDT_TEXT_FADE);
        KillTimer(hwnd, IDT_FADE);
        KillTimer(hwnd, IDT_FULLSCREEN);
        g_AnimTimerRunning = false;
        g_VizTimerRunning  = false;
        g_HoverAnimRunning = false;
        g_FadeActive       = false;
        g_FullscreenHidden = false;

        if(g_TbHook){UnhookWinEvent(g_TbHook);g_TbHook=nullptr;}

        // OPT: bounded capture thread join - prevents explorer hang if WASAPI deadlocks.
        if (g_CaptureRunning.load()) {
            g_CaptureRunning.store(false);
            if (g_hCaptureEvent) SetEvent(g_hCaptureEvent);
            if (g_CaptureThread.joinable()) {
                HANDLE hRaw = (HANDLE)g_CaptureThread.native_handle();
                if (WaitForSingleObject(hRaw, 500) == WAIT_OBJECT_0)
                    g_CaptureThread.join();
                else
                    g_CaptureThread.detach(); // abandon - thread will exit on next check
            }
            if (g_hCaptureEvent) { CloseHandle(g_hCaptureEvent); g_hCaptureEvent = nullptr; }
        }

        // OPT: explicit COM pointer release before apartment teardown
        g_cachedVolume = nullptr;
        g_cachedVolumeAumid.clear();

        // OPT: free blurred background bitmap (was leaking on uninit without art change)
        if (g_S.blurredBg) {
            delete g_S.blurredBg;
            g_S.blurredBg      = nullptr;
            g_S.blurBgW        = 0;
            g_S.blurBgH        = 0;
            g_S.blurArtVersion = -1;
        }

        // Release art bitmap under lock
        { scoped_lock lk(g_M.mtx); g_M.art.reset(); }

        DestroyBackBuffer();
        g_Mgr=nullptr; PostQuitMessage(0); return 0;
    case WM_SETTINGCHANGE:
        ApplyAppearance(hwnd); UpdateBoundsAndRegion(hwnd);
        g_LayoutDirty = true; // OPT: invalidate cached layout on settings change
        g_CachedIsLightTick = 0; // OPT: force IsLight re-check after theme change
        DestroyBackBuffer();
        InvalidateRect(hwnd,NULL,TRUE);
        if (g_S.hideFullscreen) SetTimer(hwnd, IDT_FULLSCREEN, 100, NULL);
        else { KillTimer(hwnd, IDT_FULLSCREEN); g_FullscreenHidden = false; }
        if (g_S.showVisualizer && !g_VizTimerRunning) {
            StartCaptureThread();
            SetTimer(hwnd, IDT_VIZ, 16, NULL); g_VizTimerRunning = true;
        } else if (!g_S.showVisualizer && g_VizTimerRunning) {
            KillTimer(hwnd, IDT_VIZ); g_VizTimerRunning = false;
            StopCaptureThread();
            memset(g_VizPeak,   0, sizeof(g_VizPeak));
            memset(g_VizTarget, 0, sizeof(g_VizTarget));
        }
        return 0;

case WM_TIMER:
        if(wp==IDT_POLL){
            float prevRatio = g_ArtAspectRatio;
            FetchMedia();
            // When auto-size is on, a ratio change means the panel width changed —
            // resize the window immediately so it doesn't flash at the old size.
            if (g_S.mediaAutoSize && g_S.mediaEnabled && g_ArtAspectRatio != prevRatio) {
                RecomputeLayout();
                UpdateBoundsAndRegion(hwnd);
            }
            // Track how long we've had zero media sessions, enter idle after user-defined delay
            {
                scoped_lock lk(g_M.mtx);
                if (g_S.idleScreenEnabled && !g_M.hasMedia) {
                    g_NoMediaSecs++;
                    if (g_NoMediaSecs >= g_S.idleScreenDelay && !g_IdleState) {
                        g_IdleState = true;
                    }
                } else {
                    g_NoMediaSecs = 0;
                    g_IdleState   = false;
                }
            }
            
            // Idle timeout: hide after N seconds of not playing (0 = never)
            if(g_S.idleTimeout > 0) {
                bool isPlaying = false;
                { scoped_lock lk(g_M.mtx); isPlaying = g_M.playing; }
                if(!isPlaying) {
                    g_IdleSecs++;
                    if(g_IdleSecs >= g_S.idleTimeout) g_IdleHide = true;
                } else {
                    g_IdleSecs = 0;
                    g_IdleHide = false;
                }
            } else {
                g_IdleSecs = 0;
                g_IdleHide = false;
            }

            bool shouldShow = !(g_FullscreenHidden || g_IdleHide);
            HWND tb = FindWindow(L"Shell_TrayWnd", NULL);
            bool tbVisible = IsTaskbarEffectivelyVisible(tb);

            // Stop the capture thread when the panel is hidden
            if (g_S.showVisualizer) {
                bool panelVisible = shouldShow && tbVisible;
                if (!panelVisible && g_CaptureRunning.load())  StopCaptureThread();
                if ( panelVisible && !g_CaptureRunning.load()) StartCaptureThread();
            }

            if (g_FullscreenHidden) {
                // Fullscreen hide is managed entirely by IDT_FULLSCREEN -- don't touch visibility here.
            } else if (shouldShow && tbVisible) {
                if (!g_FadeActive) {
                    g_FadeAlpha = 1.f;
                    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                }
            } else if (!shouldShow) {
                // Idle hide
                KillTimer(hwnd, IDT_FADE); g_FadeActive = false;
                g_FadeAlpha = 0.f;
                SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                ShowWindow(hwnd, SW_HIDE);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            ApplyTimerRates(hwnd); // OPT: slow poll when paused, kill IDT_POS when not needed

        } else if (wp == IDT_FULLSCREEN) {
            // Runs every 100ms when Hide on Fullscreen is enabled.
            // SHQueryUserNotificationState is the same API Windows uses internally
            // to suppress notifications during fullscreen apps and games.
            // QUNS_BUSY = regular fullscreen app, QUNS_RUNNING_D3D_FULL_SCREEN = game.
            bool isFullscreen = false;
            QUERY_USER_NOTIFICATION_STATE qs;
            if (SUCCEEDED(SHQueryUserNotificationState(&qs)))
                isFullscreen = (qs == QUNS_BUSY || qs == QUNS_RUNNING_D3D_FULL_SCREEN);

            if (isFullscreen && !g_FullscreenHidden) {
                g_FullscreenHidden = true;
                if (g_S.showVisualizer && g_CaptureRunning.load()) StopCaptureThread();
                if (g_S.fadeFullscreen) {
                    KillTimer(hwnd, IDT_FADE); g_FadeActive = false;
                    g_FadeIn = false; g_FadeActive = true;
                    SetTimer(hwnd, IDT_FADE, 16, NULL);
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                } else {
                    KillTimer(hwnd, IDT_FADE); g_FadeActive = false;
                    g_FadeAlpha = 0.f;
                    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                    ShowWindow(hwnd, SW_HIDE);
                }
            } else if (!isFullscreen && g_FullscreenHidden) {
                g_FullscreenHidden = false;
                if (g_S.showVisualizer && !g_CaptureRunning.load()) StartCaptureThread();
                if (g_S.fadeFullscreen) {
                    KillTimer(hwnd, IDT_FADE); g_FadeActive = false;
                    g_FadeAlpha = 0.f;
                    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    g_FadeIn = true; g_FadeActive = true;
                    SetTimer(hwnd, IDT_FADE, 16, NULL);
                } else {
                    KillTimer(hwnd, IDT_FADE); g_FadeActive = false;
                    g_FadeAlpha = 1.f;
                    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                }
            }
        } else if(wp==IDT_POS){
            // Poll timeline position directly every 50ms for smooth live progress bar.
            // This mirrors what taskbar-media-bar does with its IDT_LYRICS timer.
            if (g_S.showProgressBar && g_Mgr) {
                try {
                    GlobalSystemMediaTransportControlsSession ses = nullptr;
                    auto sessions = g_Mgr.GetSessions();
                    if (!g_currentMediaAppAumid.empty()) {
                        for (auto const& s : sessions) {
                            if (wstring(s.SourceAppUserModelId().c_str()) == g_currentMediaAppAumid) {
                                ses = s; break;
                            }
                        }
                    }
                    if (!ses) try { ses = g_Mgr.GetCurrentSession(); } catch (...) {}
                    if (ses) {
                        auto tl  = ses.GetTimelineProperties();
                        auto pbi = ses.GetPlaybackInfo();
                        using S  = GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                        INT64 newPos = tl.Position().count();
                        INT64 newEnd = tl.EndTime().count();
                        bool  nowPlaying = (pbi.PlaybackStatus() == S::Playing);
                        // Update interpolation baseline whenever position or playing state changes
                        if (newPos != g_PosLastRaw || newEnd != g_PosEndRaw || nowPlaying != g_PosIsPlaying) {
                            g_PosLastRaw    = newPos;
                            g_PosEndRaw     = newEnd;
                            g_PosUpdateTick = GetTickCount();
                            g_PosIsPlaying  = nowPlaying;
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                } catch (...) {}
            }
            if(g_Scroll){
                if(g_ScWait>0){
                    g_ScWait--;
                } else { 
                    int maxOff = g_ScTxtW - g_ScVisW + 10;
                    if (maxOff < 0) maxOff = 0;

                    int scrollStep = g_S.scrollSpeed == 0 ? 1 : g_S.scrollSpeed == 1 ? 2 : 3;
                    if (g_ScDir) {
                        g_ScOff += scrollStep;
                        if (g_ScOff >= maxOff) {
                            g_ScOff = maxOff; g_ScDir = false; g_ScWait = 80;
                        }
                    } else {
                        g_ScOff -= scrollStep;
                        if (g_ScOff <= 0) {
                            g_ScOff = 0; g_ScDir = true; g_ScWait = 80;
                        }
                    }
                    InvalidateRect(hwnd,NULL,FALSE); 
                }
            } else if (g_S.showProgressBar && g_PosIsPlaying) {
                // Progress bar needs continuous repaints while playing
                InvalidateRect(hwnd, NULL, FALSE);
            } else {
                KillTimer(hwnd, IDT_ANIM);
                g_AnimTimerRunning = false;
            }
        } else if (wp == IDT_HOVER_ANIM) {
            bool needsAnim = false;
            for (int i = 1; i <= 6; i++) {
                float target = 0.f;
                if (i == 4) target = (g_Hover == 4 || g_VolumeHover) ? 1.f : 0.f;
                else target = (g_Hover == i && !g_VolumeHover) ? 1.f : 0.f;
                
                if (g_HoverProgress[i] != target) {
                    float step = 0.15f; // Animation speed
                    if (g_HoverProgress[i] < target) g_HoverProgress[i] = min(target, g_HoverProgress[i] + step);
                    else g_HoverProgress[i] = max(target, g_HoverProgress[i] - step);
                    needsAnim = true;
                }
            }
        if (needsAnim) InvalidateRect(hwnd, NULL, FALSE);
        else { KillTimer(hwnd, IDT_HOVER_ANIM); g_HoverAnimRunning = false; }
        } else if (wp == IDT_FADE) {
            float speed = 0.08f;
            if (g_FadeIn) {
                g_FadeAlpha = min(1.f, g_FadeAlpha + speed);
                SetLayeredWindowAttributes(hwnd, 0, (BYTE)(g_FadeAlpha * 255), LWA_ALPHA);
                if (g_FadeAlpha >= 1.f) { KillTimer(hwnd, IDT_FADE); g_FadeActive = false; }
            } else {
                g_FadeAlpha = max(0.f, g_FadeAlpha - speed);
                SetLayeredWindowAttributes(hwnd, 0, (BYTE)(g_FadeAlpha * 255), LWA_ALPHA);
                if (g_FadeAlpha <= 0.f) { KillTimer(hwnd, IDT_FADE); g_FadeActive = false; ShowWindow(hwnd, SW_HIDE); }
            }
        // IDT_FULLSCREEN polls SHQueryUserNotificationState every 100ms
        } else if (wp == IDT_TEXT_FADE) {
            if (g_TextFadeOut) {
                g_TextFadeAlpha -= 0.12f;
                if (g_TextFadeAlpha <= 0.f) {
                    g_TextFadeAlpha = 0.f;
                    g_TextFadeOut = false;
                    { scoped_lock lk(g_M.mtx); g_M.title = g_PendingTitle; g_M.artist = g_PendingArtist; }
                }
            } else {
                g_TextFadeAlpha += 0.12f;
                if (g_TextFadeAlpha >= 1.f) {
                    g_TextFadeAlpha = 1.f;
                    KillTimer(hwnd, IDT_TEXT_FADE);
                }
            }
            InvalidateRect(hwnd, NULL, FALSE);
        } else if (wp == IDT_VIZ) {
            if (!g_S.showVisualizer) {
                KillTimer(hwnd, IDT_VIZ); g_VizTimerRunning = false;
                StopCaptureThread();
            } else {
                UpdateVisualizerPeaks();
                bool changed = false;

                // Per-shape smoothing
                float attack = 0.55f, decay = 0.18f;
                switch (g_S.vizShape) {
                    case 0: attack = 0.70f; decay = 0.20f; break; // Stereo: snappy
                    case 2: attack = 0.50f; decay = 0.18f; break; // Mirror
                    case 3: attack = 0.32f; decay = 0.16f; break; // Wave
                    case 4: attack = 0.18f; decay = 0.10f; break; // Breathe: slow
                    default: break;
                }

                for (int i = 0; i < max(1, min(g_S.vizBars, VIZ_BARS_MAX)); i++) {
                    float target = g_VizTarget[i];
                    float cur    = g_VizPeak[i];

                    float a = attack, d = decay;
                    if (g_S.vizShape == 1) { // Mountain: per-bar physics
                        float dist = fabsf((float)i - ((max(1, min(g_S.vizBars, VIZ_BARS_MAX)) - 1) * 0.5f));
                        if (dist < 0.5f)      { a = 0.85f; d = 0.24f; }
                        else if (dist < 1.5f) { a = 0.60f; d = 0.18f; }
                        else                  { a = 0.92f; d = 0.32f; }
                    }

                    float step = (target > cur) ? a : d;
                    float next = cur + (target - cur) * step;
                    if (fabsf(next - cur) > 0.0005f) { g_VizPeak[i] = next; changed = true; }
                    else g_VizPeak[i] = target;
                }
                if (changed) InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (g_S.showSpeakerIcon && g_Hover == 4 && g_VolumeHover) {
            SetCapture(hwnd);
            RECT _rc; GetClientRect(hwnd, &_rc);
            if (g_LayoutDirty || g_Layout.W != _rc.right || g_Layout.H != _rc.bottom)
                RebuildLayout(_rc.right, _rc.bottom); // OPT: use cache
            float sliderX = g_Layout.ctrl.vX + 18.f + 7.f;
            float newVolume = (float)(LOWORD(lp) - sliderX) / 60.f;
            SetVolume(newVolume);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;

    case WM_MOUSEMOVE:{
        int mx=LOWORD(lp),my=HIWORD(lp);
        RECT _rc; GetClientRect(hwnd, &_rc);
        // OPT: use cached layout - rebuilds only when settings/size changed
        if (g_LayoutDirty || g_Layout.W != _rc.right || g_Layout.H != _rc.bottom)
            RebuildLayout(_rc.right, _rc.bottom);
        const float nX=g_Layout.ctrl.nX, ppX=g_Layout.ctrl.ppX, pX=g_Layout.ctrl.pX;
        const float vX=g_Layout.ctrl.vX, cY=g_Layout.ctrl.cY;
        const float shuffleX=g_Layout.ctrl.shuffleX, repeatX=g_Layout.ctrl.repeatX;
        float _scale = _rc.bottom / 52.f;
        float iconScale = (g_S.iconSize / 14.f) * _scale;
        const float ppBigR = 14.f * iconScale, pnSmallR = 9.f * iconScale;
        int ns=0; bool newVolumeHover = g_VolumeHover;

        float hitTop = cY - 14.f, hitBot = cY + 14.f;
        
        if (!g_S.miniMode) {
        if (g_S.showSpeakerIcon) {
            float volHitLeft = vX - 12.f;
            float volHitRight = g_VolumeHover ? (vX + 95.f) : (vX + 14.f);
            if (mx >= volHitLeft && mx <= volHitRight && my >= hitTop && my <= hitBot) {
                newVolumeHover = true; ns = 4;
            } else newVolumeHover = false;
        }

        if (ns != 4) {
            if (g_S.showPlaybackControls) {
                if      (mx >= pX  - (pnSmallR+4) && mx <= pX  + (pnSmallR+4) && my >= cY-(pnSmallR+4) && my <= cY+(pnSmallR+4)) ns = 1;
                else if (mx >= ppX - ppBigR        && mx <= ppX + ppBigR        && my >= cY-ppBigR        && my <= cY+ppBigR)        ns = 2;
                else if (mx >= nX  - (pnSmallR+4) && mx <= nX  + (pnSmallR+4) && my >= cY-(pnSmallR+4) && my <= cY+(pnSmallR+4)) ns = 3;
            }
            if (ns == 0 && g_S.showShuffleButton && shuffleX > 0.f &&
                mx >= shuffleX - (pnSmallR+4) && mx <= shuffleX + (pnSmallR+4) && my >= cY-(pnSmallR+4) && my <= cY+(pnSmallR+4)) ns = 5;
            if (ns == 0 && g_S.showRepeatButton && repeatX > 0.f &&
                mx >= repeatX  - (pnSmallR+4) && mx <= repeatX  + (pnSmallR+4) && my >= cY-(pnSmallR+4) && my <= cY+(pnSmallR+4)) ns = 6;
        }

        } // end !miniMode

        if (g_S.showSpeakerIcon && g_VolumeHover && (wp & MK_LBUTTON) && GetCapture() == hwnd) {
            float sliderX = vX + 18.f + 7.f;
            float newVolume = (float)(mx - sliderX) / 60.f;
            SetVolume(newVolume);
        }

        if (ns != g_Hover || newVolumeHover != g_VolumeHover) {
            g_Hover = ns; g_VolumeHover = newVolumeHover;
            if (!g_HoverAnimRunning) { SetTimer(hwnd, IDT_HOVER_ANIM, 16, NULL); g_HoverAnimRunning = true; }
            InvalidateRect(hwnd,NULL,FALSE);
        }
        TRACKMOUSEEVENT tme={sizeof(tme),TME_LEAVE,hwnd};TrackMouseEvent(&tme);
        return 0;}

    case WM_MOUSELEAVE:
        if (GetCapture() != hwnd) {
            g_Hover = 0; g_VolumeHover = false;
            if (!g_HoverAnimRunning) { SetTimer(hwnd, IDT_HOVER_ANIM, 16, NULL); g_HoverAnimRunning = true; }
            InvalidateRect(hwnd,NULL,FALSE); 
        }
        break;

    case WM_LBUTTONUP:
        if (GetCapture() == hwnd) ReleaseCapture();
        if (g_Hover > 0 && g_Hover <= 6 && g_Hover != 4) {
            Cmd(g_Hover);
        } else if (g_Hover == 0) {
            int mx = LOWORD(lp);
            RECT _rc; GetClientRect(hwnd, &_rc);
            // OPT: use cached layout
            if (g_LayoutDirty || g_Layout.W != _rc.right || g_Layout.H != _rc.bottom)
                RebuildLayout(_rc.right, _rc.bottom);
            float firstControlX = g_Layout.ctrl.firstControlX;
            // If user clicks the left side (art/title), try to focus the app
            if (mx < (int)(firstControlX - 12) && !g_currentMediaAppAumid.empty()) {
                FocusData fd { &g_currentMediaAppAumid, nullptr };
                EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&fd));

                if (fd.found) {
                    if (IsIconic(fd.found)) ShowWindow(fd.found, SW_RESTORE);
                    SetForegroundWindow(fd.found);
                }
            }
        }
        return 0;

    case WM_MOUSEWHEEL:{
        POINT pt = { LOWORD(lp), HIWORD(lp) }; ScreenToClient(hwnd, &pt);
        // scrollVolume: scroll anywhere on the panel to adjust volume (speaker icon not required)
        if (g_S.scrollVolume) {
            short d=GET_WHEEL_DELTA_WPARAM(wp);
            SetVolume(g_Volume + (d > 0 ? 0.05f : -0.05f));
            InvalidateRect(hwnd, NULL, FALSE);
        } else if (g_S.showSpeakerIcon) {
            // Speaker icon visible but scrollVolume off: only scroll near the icon
            RECT _rc; GetClientRect(hwnd, &_rc);
            if (g_LayoutDirty || g_Layout.W != _rc.right || g_Layout.H != _rc.bottom)
                RebuildLayout(_rc.right, _rc.bottom);
            float vX2w = g_Layout.ctrl.vX;
            if (pt.x >= vX2w - 15 && pt.x <= vX2w + 95) {
                short d=GET_WHEEL_DELTA_WPARAM(wp);
                SetVolume(g_Volume + (d > 0 ? 0.05f : -0.05f));
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;}

    case WM_PAINT:{
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        EnsureBackBuffer(hdc, rc.right, rc.bottom);
        DrawPanel(g_hBackDC, rc.right, rc.bottom);
        if(g_Scroll && !g_AnimTimerRunning) { SetTimer(hwnd, IDT_ANIM, 16, NULL); g_AnimTimerRunning = true; }
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, g_hBackDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;}

    case WM_APP+10: {
        HWND tb = FindWindow(L"Shell_TrayWnd", nullptr);
        if (IsTaskbarEffectivelyVisible(tb)) {
            UpdateBoundsAndRegion(hwnd);
        } else {
            // Taskbar slid off-screen - hide the panel so it doesn't float
            if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
        }
        return 0;
    }
    default:
        if(msg==g_TbCreatedMsg){ if(g_TbHook){UnhookWinEvent(g_TbHook);g_TbHook=nullptr;} HookTaskbar(hwnd); return 0; }
        // Forward media key AppCommands (mouse hotkeys, keyboard media keys) to the shell.
        // Without this, WM_APPCOMMAND dies here because this window is parentless and topmost,
        // breaking the normal bubble chain that would reach the media app or shell handler.
        if (msg == WM_APPCOMMAND) {
            HWND hShell = FindWindow(L"Shell_TrayWnd", nullptr);
            if (hShell) SendMessage(hShell, msg, wp, lp);
            return 0;
        }
    }
    return DefWindowProc(hwnd,msg,wp,lp);
}

// ── Media Thread ──────────────────────────────────────────────────────────────
void MediaThread(){
    winrt::init_apartment();
    GdiplusStartupInput gsi; ULONG_PTR tok;
    GdiplusStartup(&tok,&gsi,NULL);

    WNDCLASS wc={}; wc.lpfnWndProc=WndProc; wc.hInstance=GetModuleHandle(NULL);
    wc.lpszClassName=L"WindhawkMusicLounge_v5"; wc.hCursor=LoadCursor(NULL,IDC_HAND);
    RegisterClass(&wc);

    HMODULE hu32=GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CWB=hu32?(pCreateWindowInBand)GetProcAddress(hu32,"CreateWindowInBand"):nullptr;
    if(CWB) g_hWnd=CWB(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST, wc.lpszClassName,L"MusicLounge",WS_POPUP|WS_VISIBLE, 0,0,g_S.width,g_S.height,NULL,NULL,wc.hInstance,NULL,ZBID_IMMERSIVE_NOTIFICATION);
    if(!g_hWnd) g_hWnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST, wc.lpszClassName,L"MusicLounge",WS_POPUP|WS_VISIBLE, 0,0,g_S.width,g_S.height,NULL,NULL,wc.hInstance,NULL);
    SetLayeredWindowAttributes(g_hWnd,0,255,LWA_ALPHA);

    MSG m; while(GetMessage(&m,NULL,0,0)){TranslateMessage(&m);DispatchMessage(&m);}
    UnregisterClass(wc.lpszClassName,wc.hInstance); GdiplusShutdown(tok); winrt::uninit_apartment();
}

std::unique_ptr<std::thread> g_pThread = nullptr;

BOOL  WhTool_ModInit()           { SetCurrentProcessExplicitAppUserModelID(L"taskbar-media-player"); LoadSettings(); g_pThread = std::make_unique<std::thread>(MediaThread); return TRUE; }
void  WhTool_ModUninit()         { if(g_hWnd)SendMessage(g_hWnd,WM_APPCLOSE,0,0); if(g_pThread){if(g_pThread->joinable())g_pThread->join(); g_pThread.reset();} }
void  WhTool_ModSettingsChanged(){ LoadSettings(); if(g_hWnd){SendMessage(g_hWnd,WM_TIMER,IDT_POLL,0);SendMessage(g_hWnd,WM_SETTINGCHANGE,0,0);} }

// ── Boilerplate ───────────────────────────────────────────────────────────────
bool   g_isLauncher=false;
HANDLE g_toolMutex =nullptr;
void WINAPI EntryHook(){ Wh_Log(L">"); ExitThread(0); }

BOOL Wh_ModInit(){
    bool isSvc=false,isTool=false,isCur=false;
    int argc; LPWSTR* argv=CommandLineToArgvW(GetCommandLine(),&argc);
    if(!argv) return FALSE;
    for(int i=1;i<argc;i++) if(!wcscmp(argv[i],L"-service")){isSvc=true;break;}
    for(int i=1;i<argc-1;i++) if(!wcscmp(argv[i],L"-tool-mod")){isTool=true;if(!wcscmp(argv[i+1],WH_MOD_ID))isCur=true;break;}
    LocalFree(argv);
    if(isSvc) return FALSE;
    if(isCur){
        g_toolMutex=CreateMutex(nullptr,TRUE,L"windhawk-tool-mod_" WH_MOD_ID);
        if(!g_toolMutex) ExitProcess(1);
        if(GetLastError()==ERROR_ALREADY_EXISTS) ExitProcess(1);
        if(!WhTool_ModInit()) ExitProcess(1);
        auto* dos=(IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        auto* nt=(IMAGE_NT_HEADERS*)((BYTE*)dos+dos->e_lfanew);
        Wh_SetFunctionHook((BYTE*)dos+nt->OptionalHeader.AddressOfEntryPoint,(void*)EntryHook,nullptr);
        return TRUE;
    }
    if(isTool) return FALSE;
    g_isLauncher=true; return TRUE;
}

void Wh_ModAfterInit(){
    if(!g_isLauncher) return;
    WCHAR path[MAX_PATH]; if(!GetModuleFileName(nullptr,path,MAX_PATH)) return;
    WCHAR cmd[MAX_PATH+64]; swprintf_s(cmd,L"\"%s\" -tool-mod \"%s\"",path,WH_MOD_ID);
    HMODULE km=GetModuleHandle(L"kernelbase.dll"); if(!km) km=GetModuleHandle(L"kernel32.dll"); if(!km) return;
    using CPIW=BOOL(WINAPI*)(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,WINBOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION,PHANDLE);
    auto fn=(CPIW)GetProcAddress(km,"CreateProcessInternalW"); if(!fn) return;
    STARTUPINFO si{.cb=sizeof(si),.dwFlags=STARTF_FORCEOFFFEEDBACK}; PROCESS_INFORMATION pi;
    if(!fn(nullptr,path,cmd,nullptr,nullptr,FALSE,NORMAL_PRIORITY_CLASS,nullptr,nullptr,&si,&pi,nullptr)) return;
    CloseHandle(pi.hProcess);CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged(){ if(!g_isLauncher) WhTool_ModSettingsChanged(); }
void Wh_ModUninit()         { if(!g_isLauncher){ WhTool_ModUninit(); ExitProcess(0); } }