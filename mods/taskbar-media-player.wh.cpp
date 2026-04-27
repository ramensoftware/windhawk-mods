// ==WindhawkMod==
// @id            taskbar-media-player
// @name          Taskbar Media Player
// @description   A sleek, floating media player on your taskbar with native volume and playback controls.
// @version       6.3.0
// @author        GR0UD
// @github        https://github.com/GR0UD
// @license       MIT
// @include       windhawk.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -lksuser
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## 💎 Taskbar Media Player
A sleek, floating media player that lives right on your Windows 11 taskbar — no
extra windows, no clutter. Album art, scrolling track info, playback controls, a
live audio visualizer, and smooth volume control all in one tight panel.

Got a feature idea or ran into something weird?
Hit me up on Discord at **@fkwr** — suggestions are always welcome.

---

## ⚠️ Requirements
- Windows 11
- Taskbar widgets panel turned **off** (right-click taskbar → Taskbar settings →
Widgets: Off)

---

## ✨ Features

### 🎨 Themes & Appearance
- **12 background themes** — Transparent, Acrylic Glass, Windows Adaptive, Neon
Glass, Sharp Split, Aurora Glow, Album Art Blur, and six gradient modes
- **Album art adaptive colors** — background gradients pull their palette
straight from the current cover art
- **Auto text color** — automatically switches between white and black text
based on system light/dark mode
- **Dynamic hover color** — button highlights can match the album art's primary
color
- **Rounded corners** with adjustable radius
- **Optional border**, mask overlay, and per-container debug outlines

### 📐 Layout & Sizing
- **Fully dynamic layout** — disable any container and the others expand to fill
the space automatically
- **Container order control** — rearrange Media, Info, Controls, and Visualizer
in any order
- **Fixed or flexible Info width**
- **Per-container padding** — independent left/right padding on every container
- **Global container gap** — uniform spacing between all active containers
- **Panel height, width, and position offset** fully configurable
- **Mini Mode** — compact view showing only Album Art and/or Visualizer

### 🎵 Media Info
- **Scrolling title & artist** — text smoothly loops when it overflows the
container
- **Truncation fallback** — adds an ellipsis when scrolling is disabled
- **Album art** with configurable corner radius and a music-note placeholder
- **Supports 16 media sources** or auto-detect all

### ⏯️ Playback Controls
- **Previous / Play-Pause / Next** buttons
- **Shuffle** and **Repeat** toggle buttons (optional)
- **5 icon themes**
- **Configurable icon size** and **button spacing**

### 🔊 Volume
- **Speaker icon** with a hover-to-expand inline volume slider
- **Mouse wheel volume control**
- **Per-app volume** — controls the volume of the playing media app only

### 📊 Live Audio Visualizer
- **5 bar shapes**: Stereo, Mountain, Mirror, Wave, Breathe
- **5 color modes**: Solid, Dynamic Album Color, Dynamic Gradient, Custom
Gradient, Acrylic
- **6 EQ presets**: Balanced, Bass, Rock, Pop, Jazz, Electronic
- **3 vertical anchors**: Top, Middle, Bottom
- **Configurable bar count** (1–20), bar width, bar gap, and idle bar height
- **Full-width background mode**
- **Sensitivity control** (0–100)

### 📏 Progress Bar
- **5 position modes**
- **Dynamic color** (matches album art) or custom hex color
- **Smooth live interpolation**

### 🙈 Behavior
- **Auto-hide on fullscreen**
- **Idle timeout**
- Clicking the art/title area **focuses the media app's window**
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
    $description: "Draws a colored outline around each active container."
  $name: Appearance

- LayoutGroup:
  - PanelHeight: 51
    $name: Height (px)
  - PanelWidth: 360
    $name: Width (px)
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
  - ContainerOrder: 1234
    $name: Container Order
    $description: "Four-digit code. 1=Media 2=Info 3=Controls 4=Visualizer."
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
  - MiniShowMedia: true
    $name: Show Album Art
  - MiniShowVisualizer: true
    $name: Show Visualizer
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
    $description: "0 = auto. Set a fixed value to override."
  - MediaAutoSize: true
    $name: Auto-Fit Art Shape
  - MediaCornerRadius: 6
    $name: Album Art Corners (0-32)
  - MediaPlaceholderIcon: true
    $name: Show Placeholder Icon
  $name: Media

- InfoContainerGroup:
  - InfoEnabled: true
    $name: Enabled
  - InfoFixedWidth: true
    $name: Fixed Width
  - InfoWidth: 160
    $name: Fixed Width (px)
  - InfoPadLeft: 0
    $name: Padding Left
  - InfoPadRight: 0
    $name: Padding Right
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
  - TextColor: FFFFFF
    $name: Text Color
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
  - ShowShuffleButton: false
    $name: Show Shuffle Button
  - ShowRepeatButton: false
    $name: Show Repeat Button
  - ShowSpeakerIcon: false
    $name: Show Volume Control
  - ScrollVolume: true
    $name: Mouse Wheel Controls Volume
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
  - CtrlIconColor: FFFFFF
    $name: Icon Color
  - HoverColor: 1ED760
    $name: Button Hover Color
  - DynamicHover: false
    $name: Dynamic Hover Color
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
  - VizWidth: 0
    $name: Container Width (px)
    $description: "0 = auto-size from bar count and spacing."
  - VizPadLeft: 0
    $name: Padding Left
  - VizPadRight: 0
    $name: Padding Right
  - VizEQ: default
    $name: EQ Preset
    $options:
      - default: Balanced
      - bass: Bass
      - rock: Rock
      - pop: Pop
      - jazz: Jazz
      - electronic: Electronic
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
  - VizColor1: 1ED760
    $name: Gradient Color 1
  - VizColor2: 00B4FF
    $name: Gradient Color 2
  - VizBars: 7
    $name: Bar Count (1-20)
  - VizBarWidth: 4
    $name: Bar Width (px)
  - VizBarGap: 5
    $name: Bar Gap (px)
  - IdleBarSize: 10
    $name: Idle Bar Height (%)
  - VizSensitivity: 100
    $name: Sensitivity (0-100)
  $name: Visualizer

*/
// ==/WindhawkModSettings==

// ============================================================================
//  Includes
// ============================================================================

#include <audioclient.h>
#include <audiopolicy.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <mmdeviceapi.h>
#include <shcore.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <windows.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Media;
using namespace Windows::Storage::Streams;

// ============================================================================
//  Constants
// ============================================================================

static constexpr int VIZ_BARS_MAX = 20;
static constexpr int FFT_SIZE = 1024;
static constexpr int NUM_BANDS = 7;
static constexpr UINT POLL_RATE_PLAYING = 1000;
static constexpr UINT POLL_RATE_PAUSED = 4000;
static constexpr UINT POS_RATE_PLAYING = 50;

static constexpr WCHAR FONT_NAME[] = L"Segoe UI Variable Display";
static constexpr WCHAR ICON_FONT[] = L"Segoe MDL2 Assets";

// Scaling reference — all icon/hit-test math is relative to this panel height.
static constexpr float PANEL_HEIGHT_REF = 52.f;
// Default icon size that ICON_SCALE_REF is relative to.
static constexpr float ICON_SIZE_REF = 14.f;
// Luminance threshold (0-255) below which album art is considered "dark".
static constexpr double LUM_DARK_THRESHOLD = 135.0;
// Pi — used in FFT twiddle factors and Hann window.
static constexpr float PI = 3.14159265f;

// Media command identifiers — used by Cmd() and HitTestControls().
enum class MediaCmd : int {
    None = 0,
    Prev = 1,
    PlayPause = 2,
    Next = 3,
    Volume = 4,
    Shuffle = 5,
    Repeat = 6,
};

// Timer IDs — collected in one place so they are never re-used accidentally.
enum TimerID : UINT_PTR {
    IDT_POLL = 1001,
    IDT_ANIM = 1002,
    IDT_HOVER_ANIM = 1003,
    IDT_FADE = 1004,
    IDT_VIZ = 1005,
    IDT_TEXT_FADE = 1006,
    IDT_FULLSCREEN = 1007,
    IDT_POS = 1008,
};

// WM_APP sub-messages
static constexpr UINT WM_APPCLOSE = WM_APP;
static constexpr UINT WM_TASKBAR_MOVED = WM_APP + 10;

// DWM extras
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif
#ifndef DWMWA_COLOR_DEFAULT
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
#endif

// ============================================================================
//  DWM / Z-Band
// ============================================================================

typedef enum _WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
} WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTBACKDROP = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
} ACCENT_STATE;
typedef struct {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;
typedef struct {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(
    WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

enum ZBID { ZBID_IMMERSIVE_NOTIFICATION = 4 };
typedef HWND(WINAPI* pCreateWindowInBand)(DWORD,
                                          LPCWSTR,
                                          LPCWSTR,
                                          DWORD,
                                          int,
                                          int,
                                          int,
                                          int,
                                          HWND,
                                          HMENU,
                                          HINSTANCE,
                                          LPVOID,
                                          DWORD);

// ============================================================================
//  Settings — user-facing values only, no runtime state
// ============================================================================

// Background/theme IDs
enum class Theme : int {
    Windows = 0,
    Transparent = 1,
    Blurred = 2,
    GradientFade = 3,
    GradientLR = 4,
    GradientVertical = 5,
    GradientRadial = 6,
    GradientVertInv = 7,
    Split = 9,
    Neon = 10,
    Aurora = 11,
    GradientRL = 12,
    AlbumBlur = 13,
};

enum class VizShape : int { Stereo = 0, Mountain, Mirror, Wave, Breathe };
enum class VizMode : int {
    Solid = 0,
    DynamicAlbum,
    DynamicGradient,
    CustomGradient,
    Acrylic
};
enum class VizAnchor : int { Top = 0, Middle, Bottom };
enum class VizEQ : int { Balanced = 0, Bass, Rock, Pop, Jazz, Electronic };
enum class IconTheme : int {
    Default = 0,
    MDL2,
    FluentOutlined,
    FluentFilled,
    FluentOutlined2
};
enum class PBLayer : int {
    UnderBoth = 0,
    AboveBoth,
    UnderText,
    UnderTextOnly,
    UnderControlsOnly
};
enum class ScrollSpeed : int { Slow = 0, Normal, Fast };

struct ContainerSettings {
    int id;
    bool enabled;
    int padLeft;
    int padRight;
};

struct WrapperSettings {
    int padTop, padBottom, padLeft, padRight;
    int globalGap;
    vector<int> order;  // render sequence, e.g. {1,2,3,4}
    bool vizAsBackground;
    bool infoFixedWidth;
    int infoWidth;
};

// All user-facing settings. No pointers, no computed state.
struct UserSettings {
    // ── Wrapper ──────────────────────────────────────────────────────────────
    WrapperSettings wrap = {7, 7, 12, 12, 12, {1, 2, 3, 4}, false, true, 160};
    ContainerSettings containers[4] = {
        {1, true, 0, 0},
        {2, true, 0, 0},
        {3, true, 0, 0},
        {4, true, 0, 0},
    };

    // ── Panel
    // ─────────────────────────────────────────────────────────────────
    int panelHeight = 51;
    int panelWidth = 360;  // base; RecomputeLayout may override
    int offsetX = 0;
    int offsetY = 0;

    // ── Appearance
    // ────────────────────────────────────────────────────────────
    Theme theme = Theme::Transparent;
    int themeOpacity = 200;
    int themeBlur = 60;
    bool roundedCorners = false;
    int cornerRadius = 10;
    bool showBorder = false;
    bool showContainerBorders = false;
    bool enableMask = false;
    int maskOpacity = 120;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    bool dynamicHover = false;
    DWORD hoverColor = 0xFF1ED760;

    // ── Media container
    // ───────────────────────────────────────────────────────
    bool mediaEnabled = true;
    int mediaCornerRadius = 6;
    int mediaWidth = 0;
    bool mediaAutoSize = true;
    bool mediaPlaceholderIcon = true;

    // ── Info container
    // ────────────────────────────────────────────────────────
    bool infoEnabled = true;
    int fontSize = 11;
    int textGap = 2;
    ScrollSpeed scrollSpeed = ScrollSpeed::Normal;
    bool disableScroll = false;

    // ── Controls container
    // ────────────────────────────────────────────────────
    bool controlsEnabled = true;
    bool showPlaybackControls = true;
    bool showShuffleButton = false;
    bool showRepeatButton = false;
    bool showSpeakerIcon = false;
    bool scrollVolume = true;
    IconTheme iconTheme = IconTheme::MDL2;
    int iconSize = 14;
    int buttonSpacing = 34;
    bool ctrlIconDynamic = false;
    DWORD ctrlIconColor = 0xFFFFFFFF;

    // ── Progress bar
    // ──────────────────────────────────────────────────────────
    bool showProgressBar = true;
    PBLayer progressBarLayer = PBLayer::UnderBoth;
    int progressBarHeight = 2;
    bool progressBarDynamic = true;
    DWORD progressBarColor = 0xFF1ED760;
    int progressBarPadLeft = 12;
    int progressBarPadRight = 0;
    int progressBarOffsetY = 0;

    // ── Visualizer
    // ────────────────────────────────────────────────────────────
    bool vizEnabled = true;
    int vizContainerWidth = 0;
    VizShape vizShape = VizShape::Stereo;
    VizMode vizMode = VizMode::Solid;
    VizAnchor vizAnchor = VizAnchor::Middle;
    VizEQ vizEQ = VizEQ::Balanced;
    DWORD vizColor = 0xFFFFFFFF;
    DWORD vizColor1 = 0xFF1ED760;
    DWORD vizColor2 = 0xFF00B4FF;
    int vizBars = 7;
    int vizBarWidth = 4;
    int vizBarGap = 5;
    int idleBarSize = 10;
    int vizSensitivity = 100;

    // ── Behavior
    // ──────────────────────────────────────────────────────────────
    wstring focusedApp = L"";
    bool hideFullscreen = true;
    bool fadeFullscreen = false;
    int idleTimeout = 0;
    bool idleScreenEnabled = false;
    int idleScreenDelay = 5;

    // ── Mini mode
    // ─────────────────────────────────────────────────────────────
    bool miniMode = false;
    bool miniShowMedia = true;
    bool miniShowVisualizer = true;
};

// ============================================================================
//  Runtime state — everything computed, cached, or transient
// ============================================================================

struct BlurCache {
    Bitmap* bitmap = nullptr;
    int width = 0;
    int height = 0;
    int artVersion = -1;

    void Invalidate() {
        delete bitmap;
        bitmap = nullptr;
        width = height = 0;
        artVersion = -1;
    }

    ~BlurCache() { Invalidate(); }
};

struct ThemeCache {
    bool isLight = false;
    DWORD tickStamp = 0;
};

struct AnimState {
    float textFadeAlpha = 1.f;
    bool textFadeOut = false;
    wstring pendingTitle;
    wstring pendingArtist;

    float hoverProgress[7] = {};  // index 1-6
    bool hoverAnimRunning = false;

    float fadeAlpha = 1.f;
    bool fadeIn = true;
    bool fadeActive = false;
};

struct ScrollState {
    int offset = 0;
    int textW = 0;
    int visW = 0;
    bool active = false;
    bool forward = true;
    int waitTicks = 80;
};

struct ProgressState {
    INT64 lastRawPos = 0;
    INT64 endRaw = 0;
    DWORD updateTick = 0;
    bool isPlaying = false;

    float LiveRatio() const {
        if (endRaw <= 0)
            return 0.f;
        INT64 pos = lastRawPos;
        if (isPlaying && updateTick > 0) {
            DWORD elapsed = min(GetTickCount() - updateTick, (DWORD)6000);
            pos += (INT64)elapsed * 10000LL;
        }
        pos = max(0LL, min(pos, endRaw));
        return (float)pos / (float)endRaw;
    }
};

struct VolumeState {
    float level = 0.5f;
    bool hovered = false;
    com_ptr<ISimpleAudioVolume> cachedIface;
    wstring cachedAumid;
};

// ============================================================================
//  Layout structures
// ============================================================================

// The x-position and width of a single container cell in the rendered panel.
struct ContainerOrigin {
    int id;
    int cellX;
    int cellWidth;
    int contentX;      // cellX + padLeft
    int contentWidth;  // cellWidth - padLeft - padRight
};

// Pre-computed geometry for the controls row.
struct ControlLayout {
    float nX, ppX, pX, vX, cY;
    float shuffleX, repeatX;
    float firstControlX;
};

// Pre-computed geometry for the visualizer bars.
struct VizLayout {
    float zoneY, zoneH;
    float barW, barStep, startX;
    float maxBarH, minBarH;
};

// Pre-computed geometry for the album art cell.
struct ArtLayout {
    int x, y, size, width, cornerRadius;
};

// All cached layout values rebuilt when the panel resizes or settings change.
struct PanelLayout {
    int W = 0, H = 0;
    ControlLayout ctrl = {};
    VizLayout viz = {};
    ArtLayout art = {};
    bool valid = false;

    ContainerOrigin origins[4] = {};
    int originCount = 0;

    // Helpers
    const ContainerOrigin* FindOrigin(int id) const {
        for (int i = 0; i < originCount; i++)
            if (origins[i].id == id)
                return &origins[i];
        return nullptr;
    }
};

// ============================================================================
//  Global singletons
// ============================================================================

static UserSettings g_US;  // user-facing settings (loaded from Windhawk)
static BlurCache g_BlurCache;
static ThemeCache g_ThemeCache;
static AnimState g_Anim;
static ScrollState g_Scroll;
static ProgressState g_Progress;
static VolumeState g_Vol;
static PanelLayout g_Layout;
static bool g_LayoutDirty = true;

static float g_ArtAspectRatio = 1.0f;
static int g_ArtVersion = 0;

// Computed panel width (derived from container widths; not a raw user setting)
static int g_PanelWidth = 360;

// Container calculated widths and enabled flags (filled by RecomputeLayout).
// Kept separate from UserSettings so RecomputeLayout never mutates user state.
static int g_ContainerWidths[5] = {};    // indexed by container id 1-4
static bool g_ContainerEnabled[5] = {};  // indexed by container id 1-4

// HWND
static HWND g_hWnd = nullptr;

// Idle / fullscreen state
static int g_IdleSecs = 0;
static bool g_IdleHide = false;
static bool g_IdleState = false;
static bool g_FullscreenHidden = false;
static int g_NoMediaSecs = 0;

// Hover tracking (which button the cursor is over, 0=none)
static MediaCmd g_HoverBtn = MediaCmd::None;

// Animation timer running flags
static bool g_AnimTimerRunning = false;
static bool g_VizTimerRunning = false;

// WinEvent hook for taskbar moves
static HWINEVENTHOOK g_TbHook = nullptr;
static UINT g_TbCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

// ============================================================================
//  RAII timer helper
// ============================================================================

// Wraps SetTimer/KillTimer so timers are impossible to leak.
// Usage: g_Timers.Set(hwnd, IDT_POLL, 1000);  g_Timers.Kill(IDT_POLL);
class TimerSet {
   public:
    void Set(HWND hwnd, UINT_PTR id, UINT ms) {
        SetTimer(hwnd, id, ms, nullptr);
        m_active[id] = hwnd;
    }

    void Kill(UINT_PTR id) {
        auto it = m_active.find(id);
        if (it != m_active.end()) {
            KillTimer(it->second, id);
            m_active.erase(it);
        }
    }

    void KillAll() {
        for (auto& [id, hwnd] : m_active)
            KillTimer(hwnd, id);
        m_active.clear();
    }

    bool IsActive(UINT_PTR id) const { return m_active.count(id) > 0; }

   private:
    std::unordered_map<UINT_PTR, HWND> m_active;
};

static TimerSet g_Timers;

// ============================================================================
//  IsLight cache
// ============================================================================

static bool IsLightCached() {
    DWORD now = GetTickCount();
    if (now - g_ThemeCache.tickStamp > 5000) {
        DWORD v = 0, sz = sizeof(v);
        g_ThemeCache.isLight =
            (RegGetValueW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Theme"
                          L"s\\Personalize",
                          L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &v,
                          &sz) == ERROR_SUCCESS &&
             v);
        g_ThemeCache.tickStamp = now;
    }
    return g_ThemeCache.isLight;
}

// Forces a re-check on the next call (call after WM_SETTINGCHANGE).
static void InvalidateLightCache() {
    g_ThemeCache.tickStamp = 0;
}

// ============================================================================
//  Settings — helpers
// ============================================================================

static DWORD ReadHexColor(const wchar_t* key, DWORD defaultRgb) {
    PCWSTR hex = Wh_GetStringSetting(key);
    DWORD rgb = defaultRgb;
    if (hex) {
        if (wcslen(hex) > 0)
            rgb = wcstoul(hex, nullptr, 16);
        Wh_FreeStringSetting(hex);
    }
    return 0xFF000000 | rgb;
}

static int ClampInt(int v, int lo, int hi) {
    return max(lo, min(hi, v));
}

// ============================================================================
//  Settings — container width calculations
// ============================================================================

// Single authoritative art-size calculation used by CalcMediaWidth,
// RecomputeLayout (mini mode), and RebuildLayout.  Outputs the logical
// art square size (aS) and its width (aW) for a given panel height.
static void CalcArtDims(int panelH, int& outS, int& outW) {
    outS = max(8, panelH - g_US.wrap.padTop - g_US.wrap.padBottom);
    if (g_US.mediaWidth > 0) {
        outW = g_US.mediaWidth;
    } else if (g_US.mediaAutoSize) {
        float ratio = max(0.25f, min(3.0f, g_ArtAspectRatio));
        outW = max(8, (int)roundf(outS * ratio));
    } else {
        outW = outS;
    }
}

static int CalcMediaWidth() {
    int aS, aW;
    CalcArtDims(g_US.panelHeight, aS, aW);
    const auto& c = g_US.containers[0];
    return c.padLeft + aW + c.padRight;
}

static int CalcInfoWidth() {
    if (g_US.wrap.infoFixedWidth) {
        const auto& c = g_US.containers[1];
        return c.padLeft + g_US.wrap.infoWidth + c.padRight;
    }
    return 0;  // flexible: filled by RecomputeLayout
}

static int CalcControlsWidth() {
    int btnCount = 0;
    if (g_US.showPlaybackControls)
        btnCount += 3;
    if (g_US.showSpeakerIcon)
        btnCount += 1;
    if (g_US.showShuffleButton)
        btnCount += 1;
    if (g_US.showRepeatButton)
        btnCount += 1;
    if (btnCount == 0)
        return 0;
    float scale = g_US.panelHeight / PANEL_HEIGHT_REF;
    float step = g_US.buttonSpacing * scale;
    int w = (int)(btnCount * step);
    const auto& c = g_US.containers[2];
    return c.padLeft + w + c.padRight;
}

static int CalcVisualizerWidth() {
    if (g_US.vizContainerWidth > 0) {
        const auto& c = g_US.containers[3];
        return c.padLeft + g_US.vizContainerWidth + c.padRight;
    }
    int barCount = max(1, g_US.vizBars);
    int barW = (g_US.vizBarWidth > 0) ? g_US.vizBarWidth : 4;
    int gap = max(0, g_US.vizBarGap);
    int contentW = barW * barCount + gap * (barCount - 1);
    const auto& c = g_US.containers[3];
    return c.padLeft + contentW + c.padRight;
}

static bool IsContainerEnabled(int id) {
    if (id >= 1 && id <= 4)
        return g_ContainerEnabled[id];
    return false;
}

static int GetContainerWidth(int id) {
    if (id >= 1 && id <= 4)
        return g_ContainerWidths[id];
    return 0;
}

// Recomputes g_ContainerWidths[1-4], g_ContainerEnabled[1-4], and g_PanelWidth.
// Never mutates g_US — enabled flags are written to g_ContainerEnabled so that
// user settings remain stable across reloads and ordering changes.
static void RecomputeLayout() {
    const bool mini = g_US.miniMode;

    if (mini) {
        bool showMedia = g_US.miniShowMedia && g_US.mediaEnabled;
        bool showViz = g_US.miniShowVisualizer && g_US.vizEnabled;
        if (!showMedia && !showViz)
            showMedia = true;

        g_ContainerEnabled[1] = showMedia;
        g_ContainerEnabled[2] = false;
        g_ContainerEnabled[3] = false;
        g_ContainerEnabled[4] = showViz;

        int aS, aW;
        CalcArtDims(g_US.panelHeight, aS, aW);

        g_ContainerWidths[1] = showMedia ? aW : 0;
        g_ContainerWidths[2] = 0;
        g_ContainerWidths[3] = 0;
        g_ContainerWidths[4] = showViz ? CalcVisualizerWidth() : 0;

        int total = g_US.wrap.padLeft + g_US.wrap.padRight;
        if (showMedia)
            total += g_ContainerWidths[1];
        if (showViz)
            total += g_ContainerWidths[4];
        if (showMedia && showViz)
            total += g_US.wrap.globalGap;

        g_PanelWidth = total;
        return;
    }

    // Normal mode: derive enabled flags from user settings — do NOT write to
    // g_US
    bool ctrlActive = g_US.controlsEnabled &&
                      (g_US.showPlaybackControls || g_US.showSpeakerIcon ||
                       g_US.showShuffleButton || g_US.showRepeatButton);
    g_ContainerEnabled[1] = g_US.mediaEnabled;
    g_ContainerEnabled[2] = g_US.infoEnabled;
    g_ContainerEnabled[3] = ctrlActive;
    g_ContainerEnabled[4] = g_US.vizEnabled;

    g_ContainerWidths[1] = CalcMediaWidth();
    g_ContainerWidths[3] = CalcControlsWidth();
    g_ContainerWidths[4] = CalcVisualizerWidth();

    // Sum fixed-width containers so Info can fill the remainder
    int fixedTotal = 0;
    int activeCount = 0;
    for (int id : g_US.wrap.order) {
        if (id == 2)
            continue;
        if (!IsContainerEnabled(id))
            continue;
        if (id == 4 && g_US.wrap.vizAsBackground)
            continue;
        fixedTotal += GetContainerWidth(id);
        activeCount++;
    }
    if (IsContainerEnabled(2))
        activeCount++;
    int totalGaps =
        (activeCount > 1) ? (activeCount - 1) * g_US.wrap.globalGap : 0;

    if (g_US.wrap.infoFixedWidth) {
        g_ContainerWidths[2] = CalcInfoWidth();
        int total = g_US.wrap.padLeft + g_US.wrap.padRight + totalGaps;
        for (int id : g_US.wrap.order) {
            if (!IsContainerEnabled(id))
                continue;
            if (id == 4 && g_US.wrap.vizAsBackground)
                continue;
            total += GetContainerWidth(id);
        }
        g_PanelWidth = total;
    } else {
        int remaining = g_US.panelWidth - g_US.wrap.padLeft -
                        g_US.wrap.padRight - fixedTotal - totalGaps;
        g_ContainerWidths[2] = max(40, remaining);
        g_PanelWidth = g_US.panelWidth;
    }
}

// ============================================================================
//  Settings — LoadSettings (split into per-section helpers)
// ============================================================================

static void LoadLayoutSettings(UserSettings& s) {
    s.panelHeight =
        ClampInt(Wh_GetIntSetting(L"LayoutGroup.PanelHeight"), 32, 200);
    s.wrap.padTop =
        ClampInt(Wh_GetIntSetting(L"LayoutGroup.WrapPadTop"), 0, 80);
    s.wrap.padBottom =
        ClampInt(Wh_GetIntSetting(L"LayoutGroup.WrapPadBottom"), 0, 80);
    s.wrap.padLeft =
        ClampInt(Wh_GetIntSetting(L"LayoutGroup.WrapPadLeft"), 0, 80);
    s.wrap.padRight =
        ClampInt(Wh_GetIntSetting(L"LayoutGroup.WrapPadRight"), 0, 80);
    s.wrap.globalGap =
        ClampInt(Wh_GetIntSetting(L"LayoutGroup.GlobalGap"), 0, 80);
    s.panelWidth =
        ClampInt(Wh_GetIntSetting(L"LayoutGroup.PanelWidth"), 150, 2000);
    s.offsetX = Wh_GetIntSetting(L"LayoutGroup.OffsetX");
    s.offsetY = Wh_GetIntSetting(L"LayoutGroup.OffsetY");

    // Container order from 4-digit integer (e.g. 1234)
    int code = Wh_GetIntSetting(L"LayoutGroup.ContainerOrder");
    if (code <= 0)
        code = 1234;
    vector<int> raw = {(code / 1000) % 10, (code / 100) % 10, (code / 10) % 10,
                       (code) % 10};
    vector<int> seen, clean;
    for (int id : raw) {
        if (id < 1 || id > 4)
            continue;
        if (find(seen.begin(), seen.end(), id) != seen.end())
            continue;
        seen.push_back(id);
        clean.push_back(id);
    }
    for (int id : {1, 2, 3, 4})
        if (find(seen.begin(), seen.end(), id) == seen.end())
            clean.push_back(id);
    s.wrap.order = clean;
}

static void LoadAppearanceSettings(UserSettings& s) {
    PCWSTR ts = Wh_GetStringSetting(L"AppearanceGroup.Theme");
    s.theme = Theme::Transparent;
    if (ts) {
        if (!wcscmp(ts, L"windows"))
            s.theme = Theme::Windows;
        else if (!wcscmp(ts, L"blurred"))
            s.theme = Theme::Blurred;
        else if (!wcscmp(ts, L"gradient"))
            s.theme = Theme::GradientFade;
        else if (!wcscmp(ts, L"gradient_lr"))
            s.theme = Theme::GradientLR;
        else if (!wcscmp(ts, L"gradient_rl"))
            s.theme = Theme::GradientRL;
        else if (!wcscmp(ts, L"gradient_vertical"))
            s.theme = Theme::GradientVertical;
        else if (!wcscmp(ts, L"gradient_vertical_inv"))
            s.theme = Theme::GradientVertInv;
        else if (!wcscmp(ts, L"gradient_radial"))
            s.theme = Theme::GradientRadial;
        else if (!wcscmp(ts, L"split"))
            s.theme = Theme::Split;
        else if (!wcscmp(ts, L"neon"))
            s.theme = Theme::Neon;
        else if (!wcscmp(ts, L"aurora"))
            s.theme = Theme::Aurora;
        else if (!wcscmp(ts, L"album_blur"))
            s.theme = Theme::AlbumBlur;
        Wh_FreeStringSetting(ts);
    }
    s.themeOpacity =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.ThemeOpacity"), 0, 255);
    s.themeBlur =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.ThemeBlur"), 0, 100);
    s.roundedCorners = Wh_GetIntSetting(L"AppearanceGroup.RoundedCorners") != 0;
    s.cornerRadius =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.CornerRadius"), 2, 32);
    s.showBorder = Wh_GetIntSetting(L"AppearanceGroup.ShowBorder") != 0;
    s.showContainerBorders =
        Wh_GetIntSetting(L"AppearanceGroup.ShowContainerBorders") != 0;
    s.enableMask = Wh_GetIntSetting(L"AppearanceGroup.EnableMask") != 0;
    s.maskOpacity =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.MaskOpacity"), 0, 255);
}

static void LoadMediaSettings(UserSettings& s) {
    s.mediaEnabled = Wh_GetIntSetting(L"MediaContainerGroup.MediaEnabled") != 0;
    s.mediaCornerRadius = ClampInt(
        Wh_GetIntSetting(L"MediaContainerGroup.MediaCornerRadius"), 0, 32);
    s.mediaWidth = max(0, Wh_GetIntSetting(L"MediaContainerGroup.MediaWidth"));
    s.mediaAutoSize =
        Wh_GetIntSetting(L"MediaContainerGroup.MediaAutoSize") != 0;
    s.mediaPlaceholderIcon =
        Wh_GetIntSetting(L"MediaContainerGroup.MediaPlaceholderIcon") != 0;
    s.containers[0] = {
        1, s.mediaEnabled,
        max(0, Wh_GetIntSetting(L"MediaContainerGroup.MediaPadLeft")),
        max(0, Wh_GetIntSetting(L"MediaContainerGroup.MediaPadRight"))};
}

static void LoadInfoSettings(UserSettings& s) {
    s.infoEnabled = Wh_GetIntSetting(L"InfoContainerGroup.InfoEnabled") != 0;
    s.wrap.infoFixedWidth =
        Wh_GetIntSetting(L"InfoContainerGroup.InfoFixedWidth") != 0;
    s.wrap.infoWidth =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.InfoWidth"), 40, 2000);
    s.fontSize =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.FontSize"), 7, 48);
    s.textGap =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.TextGap"), 0, 40);
    s.scrollSpeed = (ScrollSpeed)ClampInt(
        Wh_GetIntSetting(L"InfoContainerGroup.ScrollSpeed"), 0, 2);
    s.autoTheme = Wh_GetIntSetting(L"InfoContainerGroup.AutoTheme") != 0;
    s.manualTextColor = ReadHexColor(L"InfoContainerGroup.TextColor", 0xFFFFFF);
    s.containers[1] = {
        2, s.infoEnabled,
        max(0, Wh_GetIntSetting(L"InfoContainerGroup.InfoPadLeft")),
        max(0, Wh_GetIntSetting(L"InfoContainerGroup.InfoPadRight"))};
}

static void LoadControlsSettings(UserSettings& s) {
    s.controlsEnabled =
        Wh_GetIntSetting(L"ControlsContainerGroup.ControlsEnabled") != 0;
    s.showPlaybackControls =
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowPlaybackControls") != 0;
    s.showShuffleButton =
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowShuffleButton") != 0;
    s.showRepeatButton =
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowRepeatButton") != 0;
    s.showSpeakerIcon =
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowSpeakerIcon") != 0;
    s.scrollVolume =
        Wh_GetIntSetting(L"ControlsContainerGroup.ScrollVolume") != 0;
    s.ctrlIconDynamic =
        Wh_GetIntSetting(L"ControlsContainerGroup.CtrlIconDynamic") != 0;
    s.ctrlIconColor =
        ReadHexColor(L"ControlsContainerGroup.CtrlIconColor", 0xFFFFFF);
    s.dynamicHover =
        Wh_GetIntSetting(L"ControlsContainerGroup.DynamicHover") != 0;
    s.hoverColor = ReadHexColor(L"ControlsContainerGroup.HoverColor", 0x1ED760);
    s.iconSize =
        ClampInt(Wh_GetIntSetting(L"ControlsContainerGroup.IconSize"), 6, 28);
    s.buttonSpacing = ClampInt(
        Wh_GetIntSetting(L"ControlsContainerGroup.ButtonSpacing"), 18, 60);
    s.containers[2] = {
        3, s.controlsEnabled,
        max(0, Wh_GetIntSetting(L"ControlsContainerGroup.CtrlPadLeft")),
        max(0, Wh_GetIntSetting(L"ControlsContainerGroup.CtrlPadRight"))};

    PCWSTR it = Wh_GetStringSetting(L"ControlsContainerGroup.IconTheme");
    s.iconTheme = IconTheme::Default;
    if (it) {
        if (!wcscmp(it, L"mdl2"))
            s.iconTheme = IconTheme::MDL2;
        else if (!wcscmp(it, L"fluent"))
            s.iconTheme = IconTheme::FluentOutlined;
        else if (!wcscmp(it, L"fluent_filled"))
            s.iconTheme = IconTheme::FluentFilled;
        else if (!wcscmp(it, L"fluent_outline2"))
            s.iconTheme = IconTheme::FluentOutlined2;
        Wh_FreeStringSetting(it);
    }
}

static void LoadProgressBarSettings(UserSettings& s) {
    s.showProgressBar =
        Wh_GetIntSetting(L"ProgressBarGroup.ShowProgressBar") != 0;
    s.progressBarHeight =
        ClampInt(Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarHeight"), 1, 6);
    s.progressBarDynamic =
        Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarDynamic") != 0;
    s.progressBarColor =
        ReadHexColor(L"ProgressBarGroup.ProgressBarColor", 0x1ED760);
    s.progressBarPadLeft = ClampInt(
        Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarPadLeft"), 0, 80);
    s.progressBarPadRight = ClampInt(
        Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarPadRight"), 0, 80);
    s.progressBarOffsetY = ClampInt(
        Wh_GetIntSetting(L"ProgressBarGroup.ProgressBarOffsetY"), -20, 20);

    PCWSTR pl = Wh_GetStringSetting(L"ProgressBarGroup.ProgressBarLayer");
    s.progressBarLayer = PBLayer::UnderBoth;
    if (pl) {
        if (!wcscmp(pl, L"above_both"))
            s.progressBarLayer = PBLayer::AboveBoth;
        else if (!wcscmp(pl, L"under_text"))
            s.progressBarLayer = PBLayer::UnderText;
        else if (!wcscmp(pl, L"under_text_only"))
            s.progressBarLayer = PBLayer::UnderTextOnly;
        else if (!wcscmp(pl, L"under_controls_only"))
            s.progressBarLayer = PBLayer::UnderControlsOnly;
        Wh_FreeStringSetting(pl);
    }
}

static void LoadVisualizerSettings(UserSettings& s) {
    s.vizEnabled =
        Wh_GetIntSetting(L"VisualizerContainerGroup.VizEnabled") != 0;
    s.wrap.vizAsBackground =
        Wh_GetIntSetting(L"VisualizerContainerGroup.VizAsBackground") != 0;
    s.vizContainerWidth =
        max(0, Wh_GetIntSetting(L"VisualizerContainerGroup.VizWidth"));
    s.vizColor = ReadHexColor(L"VisualizerContainerGroup.VizColor", 0xFFFFFF);
    s.vizColor1 = ReadHexColor(L"VisualizerContainerGroup.VizColor1", 0x1ED760);
    s.vizColor2 = ReadHexColor(L"VisualizerContainerGroup.VizColor2", 0x00B4FF);
    s.vizBars =
        ClampInt(Wh_GetIntSetting(L"VisualizerContainerGroup.VizBars"), 1, 20);
    s.vizBarWidth = ClampInt(
        Wh_GetIntSetting(L"VisualizerContainerGroup.VizBarWidth"), 0, 64);
    s.vizBarGap = ClampInt(
        Wh_GetIntSetting(L"VisualizerContainerGroup.VizBarGap"), 0, 20);
    s.idleBarSize = ClampInt(
        Wh_GetIntSetting(L"VisualizerContainerGroup.IdleBarSize"), 0, 100);
    s.vizSensitivity = ClampInt(
        Wh_GetIntSetting(L"VisualizerContainerGroup.VizSensitivity"), 0, 100);
    s.containers[3] = {
        4, s.vizEnabled,
        max(0, Wh_GetIntSetting(L"VisualizerContainerGroup.VizPadLeft")),
        max(0, Wh_GetIntSetting(L"VisualizerContainerGroup.VizPadRight"))};

    PCWSTR vs = Wh_GetStringSetting(L"VisualizerContainerGroup.VizShape");
    s.vizShape = VizShape::Stereo;
    if (vs) {
        if (!wcscmp(vs, L"mountain"))
            s.vizShape = VizShape::Mountain;
        else if (!wcscmp(vs, L"mirror"))
            s.vizShape = VizShape::Mirror;
        else if (!wcscmp(vs, L"wave"))
            s.vizShape = VizShape::Wave;
        else if (!wcscmp(vs, L"breathe"))
            s.vizShape = VizShape::Breathe;
        Wh_FreeStringSetting(vs);
    }

    PCWSTR vm = Wh_GetStringSetting(L"VisualizerContainerGroup.VizMode");
    s.vizMode = VizMode::Solid;
    if (vm) {
        if (!wcscmp(vm, L"dynamic_album"))
            s.vizMode = VizMode::DynamicAlbum;
        else if (!wcscmp(vm, L"dynamic_gradient"))
            s.vizMode = VizMode::DynamicGradient;
        else if (!wcscmp(vm, L"custom_gradient"))
            s.vizMode = VizMode::CustomGradient;
        else if (!wcscmp(vm, L"acrylic"))
            s.vizMode = VizMode::Acrylic;
        Wh_FreeStringSetting(vm);
    }

    PCWSTR va = Wh_GetStringSetting(L"VisualizerContainerGroup.VizAnchor");
    s.vizAnchor = VizAnchor::Middle;
    if (va) {
        if (!wcscmp(va, L"top"))
            s.vizAnchor = VizAnchor::Top;
        else if (!wcscmp(va, L"bottom"))
            s.vizAnchor = VizAnchor::Bottom;
        Wh_FreeStringSetting(va);
    }

    PCWSTR ve = Wh_GetStringSetting(L"VisualizerContainerGroup.VizEQ");
    s.vizEQ = VizEQ::Balanced;
    if (ve) {
        if (!wcscmp(ve, L"bass"))
            s.vizEQ = VizEQ::Bass;
        else if (!wcscmp(ve, L"rock"))
            s.vizEQ = VizEQ::Rock;
        else if (!wcscmp(ve, L"pop"))
            s.vizEQ = VizEQ::Pop;
        else if (!wcscmp(ve, L"jazz"))
            s.vizEQ = VizEQ::Jazz;
        else if (!wcscmp(ve, L"electronic"))
            s.vizEQ = VizEQ::Electronic;
        Wh_FreeStringSetting(ve);
    }
}

static void LoadBehaviorSettings(UserSettings& s) {
    s.hideFullscreen = Wh_GetIntSetting(L"BehaviorGroup.HideFullscreen") != 0;
    s.fadeFullscreen = Wh_GetIntSetting(L"BehaviorGroup.FadeFullscreen") != 0;
    s.disableScroll = Wh_GetIntSetting(L"BehaviorGroup.DisableScroll") != 0;
    s.idleTimeout = max(0, Wh_GetIntSetting(L"BehaviorGroup.IdleTimeout"));
    s.idleScreenEnabled =
        Wh_GetIntSetting(L"BehaviorGroup.IdleScreenEnabled") != 0;
    s.idleScreenDelay =
        ClampInt(Wh_GetIntSetting(L"BehaviorGroup.IdleScreenDelay"), 1, 300);

    PCWSTR fa = Wh_GetStringSetting(L"BehaviorGroup.FocusedApp");
    if (fa && wcscmp(fa, L"all") != 0) {
        // Normalize well-known aliases that don't match exe names directly
        if (!wcscmp(fa, L"applemusic"))
            s.focusedApp = L"applemusic";
        else if (!wcscmp(fa, L"ytmusic"))
            s.focusedApp = L"ytmusic";
        else
            s.focusedApp = fa;
    } else {
        s.focusedApp = L"";
    }
    if (fa)
        Wh_FreeStringSetting(fa);

    s.miniMode = Wh_GetIntSetting(L"MiniModeGroup.MiniMode") != 0;
    s.miniShowMedia = Wh_GetIntSetting(L"MiniModeGroup.MiniShowMedia") != 0;
    s.miniShowVisualizer =
        Wh_GetIntSetting(L"MiniModeGroup.MiniShowVisualizer") != 0;
}

void LoadSettings() {
    UserSettings s;  // start fresh — no stale values survive a settings reload
    LoadLayoutSettings(s);
    LoadAppearanceSettings(s);
    LoadMediaSettings(s);
    LoadInfoSettings(s);
    LoadControlsSettings(s);
    LoadProgressBarSettings(s);
    LoadVisualizerSettings(s);
    LoadBehaviorSettings(s);
    g_US = s;
    RecomputeLayout();
}

// ============================================================================
//  FFT / audio capture
// ============================================================================

static atomic<float> g_VizBands[NUM_BANDS] = {};
static atomic<bool> g_CaptureRunning{false};
static thread g_CaptureThread;
static HANDLE g_hCaptureEvent = nullptr;

// Pre-baked tables
static float g_HannWindow[FFT_SIZE] = {};
static float g_TwiddleRe[FFT_SIZE / 2] = {};
static float g_TwiddleIm[FFT_SIZE / 2] = {};
static int g_LogBinStart[NUM_BANDS + 1] = {};

static void BuildHannWindow() {
    for (int i = 0; i < FFT_SIZE; i++)
        g_HannWindow[i] = 0.5f * (1.f - cosf(2.f * PI * i / (FFT_SIZE - 1)));
}

static void BuildTwiddleFactors() {
    for (int i = 0; i < FFT_SIZE / 2; i++) {
        float ang = -2.0f * PI * i / FFT_SIZE;
        g_TwiddleRe[i] = cosf(ang);
        g_TwiddleIm[i] = sinf(ang);
    }
}

static void BuildLogBins(UINT32 sampleRate) {
    static constexpr float FREQ_EDGES[NUM_BANDS + 1] = {
        20.f, 120.f, 300.f, 800.f, 2500.f, 6000.f, 14000.f, 20000.f};
    for (int b = 0; b <= NUM_BANDS; b++) {
        int bin = (int)(FREQ_EDGES[b] * FFT_SIZE / (float)sampleRate);
        g_LogBinStart[b] = max(1, min(FFT_SIZE / 2 - 1, bin));
    }
}

static void FFT(vector<float>& re, vector<float>& im) {
    int n = (int)re.size();
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) {
            swap(re[i], re[j]);
            swap(im[i], im[j]);
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
                float vRe =
                    re[i + j + halfLen] * wRe - im[i + j + halfLen] * wIm;
                float vIm =
                    re[i + j + halfLen] * wIm + im[i + j + halfLen] * wRe;
                re[i + j] = uRe + vRe;
                im[i + j] = uIm + vIm;
                re[i + j + halfLen] = uRe - vRe;
                im[i + j + halfLen] = uIm - vIm;
            }
        }
    }
}

// EQ multipliers shared between capture thread and UpdateVisualizerPeaks.
struct EQMultipliers {
    float low, mid, high;
};
static EQMultipliers GetEQMultipliers(VizEQ eq) {
    switch (eq) {
        case VizEQ::Bass:
            return {2.0f, 0.6f, 0.4f};
        case VizEQ::Rock:
            return {1.3f, 1.5f, 1.2f};
        case VizEQ::Pop:
            return {0.8f, 1.2f, 1.8f};
        case VizEQ::Jazz:
            return {1.1f, 0.8f, 0.6f};
        case VizEQ::Electronic:
            return {1.7f, 0.6f, 1.7f};
        default:
            return {1.0f, 1.0f, 1.0f};
    }
}

static void CaptureThreadProc() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    BuildHannWindow();
    BuildTwiddleFactors();

    com_ptr<IMMDeviceEnumerator> pEnum;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                CLSCTX_ALL, IID_PPV_ARGS(&pEnum)))) {
        CoUninitialize();
        return;
    }

    com_ptr<IMMDevice> pDev;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, pDev.put()))) {
        CoUninitialize();
        return;
    }

    com_ptr<IAudioClient> pClient;
    com_ptr<IAudioCaptureClient> pCapture;
    UINT32 sampleRate = 48000, channels = 2;
    bool isFloat = true;

    {
        com_ptr<IAudioClient> pC;
        if (SUCCEEDED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL,
                                     nullptr,
                                     reinterpret_cast<void**>(pC.put())))) {
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
                if (SUCCEEDED(
                        pC->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                       AUDCLNT_STREAMFLAGS_LOOPBACK |
                                           AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                       200000, 0, pwfx, nullptr))) {
                    if (g_hCaptureEvent)
                        pC->SetEventHandle(g_hCaptureEvent);
                    com_ptr<IAudioCaptureClient> pCap;
                    if (SUCCEEDED(pC->GetService(IID_PPV_ARGS(&pCap)))) {
                        pClient = pC;
                        pCapture = pCap;
                    }
                }
                CoTaskMemFree(pwfx);
            }
        }
    }

    BuildLogBins(sampleRate);

    static constexpr int RING_CAP = FFT_SIZE * 4;
    vector<float> ringBuf(RING_CAP, 0.f);
    int ringHead = 0, ringCount = 0;
    vector<float> re(FFT_SIZE), im(FFT_SIZE);

    float bandEnv[NUM_BANDS] = {};
    static constexpr float GRAVITY[NUM_BANDS] = {0.018f, 0.020f, 0.022f, 0.025f,
                                                 0.030f, 0.036f, 0.042f};

    if (pClient)
        pClient->Start();

    while (g_CaptureRunning.load(memory_order_relaxed)) {
        if (g_hCaptureEvent)
            WaitForSingleObject(g_hCaptureEvent, 20);
        else
            Sleep(8);

        if (!pCapture)
            continue;

        UINT32 packetSize = 0;
        if (FAILED(pCapture->GetNextPacketSize(&packetSize)) ||
            packetSize == 0) {
            for (int b = 0; b < NUM_BANDS; b++) {
                bandEnv[b] = max(0.f, bandEnv[b] - GRAVITY[b]);
                g_VizBands[b].store(bandEnv[b], memory_order_relaxed);
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

            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData &&
                numFrames > 0) {
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

        while (ringCount >= FFT_SIZE) {
            int readStart = (ringHead - ringCount + RING_CAP) % RING_CAP;
            for (int i = 0; i < FFT_SIZE; i++) {
                re[i] = ringBuf[(readStart + i) % RING_CAP] * g_HannWindow[i];
                im[i] = 0.f;
            }
            ringCount -= FFT_SIZE / 2;
            FFT(re, im);

            float t_sens = g_US.vizSensitivity / 100.0f;
            float sliderGain = 0.25f + t_sens * t_sens * 2.75f;
            auto eq = GetEQMultipliers(g_US.vizEQ);

            static constexpr float BAND_SENSITIVITY[NUM_BANDS] = {
                0.30f, 0.22f, 0.12f, 0.06f, 0.030f, 0.018f, 0.010f};
            static constexpr int BAND_EQ_ZONE[NUM_BANDS] = {0, 0, 1, 1,
                                                            2, 2, 2};

            for (int b = 0; b < NUM_BANDS; b++) {
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
                float mag = max(
                    0.f, min(1.f, (rms / (FFT_SIZE * 0.5f)) /
                                      BAND_SENSITIVITY[b] * sliderGain * eqM));

                bandEnv[b] = (mag >= bandEnv[b])
                                 ? mag
                                 : max(0.f, bandEnv[b] - GRAVITY[b]);
                g_VizBands[b].store(bandEnv[b], memory_order_relaxed);
            }
        }
    }

    if (pClient)
        pClient->Stop();
    CoUninitialize();
}

static void StartCaptureThread() {
    if (g_CaptureRunning.load())
        return;
    if (!g_hCaptureEvent)
        g_hCaptureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_CaptureRunning.store(true);
    g_CaptureThread = thread(CaptureThreadProc);
}

static void StopCaptureThread() {
    if (!g_CaptureRunning.load())
        return;
    g_CaptureRunning.store(false);
    if (g_hCaptureEvent)
        SetEvent(g_hCaptureEvent);
    if (g_CaptureThread.joinable())
        g_CaptureThread.join();
    if (g_hCaptureEvent) {
        CloseHandle(g_hCaptureEvent);
        g_hCaptureEvent = nullptr;
    }
    for (int i = 0; i < NUM_BANDS; i++)
        g_VizBands[i].store(0.f);
}

// ============================================================================
//  Visualizer peaks
// ============================================================================

static float g_VizPeak[VIZ_BARS_MAX] = {};
static float g_VizTarget[VIZ_BARS_MAX] = {};

static constexpr float VIZ_SEEDS[VIZ_BARS_MAX] = {
    0.83f, 0.41f, 1.27f, 0.61f, 1.09f, 0.37f, 0.95f, 0.52f, 1.18f, 0.74f,
    0.29f, 1.03f, 0.66f, 0.88f, 0.45f, 1.21f, 0.57f, 0.93f, 0.31f, 1.15f};

static void UpdateVisualizerPeaks() {
    const int vizBars = max(1, min(g_US.vizBars, VIZ_BARS_MAX));

    float bands[NUM_BANDS];
    float masterPeak = 0.f;
    for (int i = 0; i < NUM_BANDS; i++) {
        bands[i] = g_VizBands[i].load(memory_order_relaxed);
        masterPeak = max(masterPeak, bands[i]);
    }

    auto eq = GetEQMultipliers(g_US.vizEQ);

    auto sampleBands = [&](float t) -> float {
        float pos = t * (NUM_BANDS - 1);
        int lo = (int)pos;
        int hi = min(lo + 1, NUM_BANDS - 1);
        return bands[lo] * (1.f - (pos - (float)lo)) +
               bands[hi] * (pos - (float)lo);
    };

    auto eqForT = [&](float t) -> float {
        return (t < 0.33f) ? eq.low : (t < 0.66f) ? eq.mid : eq.high;
    };

    float t = (float)GetTickCount64() * 0.001f;
    float center = (vizBars - 1) * 0.5f;
    float idleFloor = g_US.idleBarSize / 100.0f;

    for (int i = 0; i < vizBars; i++) {
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;

        switch (g_US.vizShape) {
            case VizShape::Stereo:
                target = sampleBands(freqT) * eqForT(freqT);
                break;

            case VizShape::Mountain: {
                float dist = fabsf((float)i - center) / max(1.f, center);
                float energy = sampleBands(dist) * eqForT(dist);
                float taper = 1.6f - dist * 0.9f;
                target =
                    max(0.f,
                        min(1.f, (energy + masterPeak * (0.2f - dist * 0.12f)) *
                                     taper));
                break;
            }

            case VizShape::Mirror: {
                float mirT = 1.f - fabsf((float)i - center) / max(1.f, center);
                float energy = sampleBands(mirT) * eqForT(mirT);
                target =
                    max(0.f,
                        min(1.f, (energy + masterPeak * (0.1f + mirT * 0.12f)) *
                                     1.3f));
                break;
            }

            case VizShape::Wave: {
                float phase = (float)i * (2.f * PI / (float)vizBars);
                float wave = 0.55f + 0.45f * sinf(t * 3.5f - phase);
                float energy = sampleBands(freqT) * eqForT(freqT);
                target = max(0.f, min(1.f, energy * wave + masterPeak * 0.15f));
                break;
            }

            case VizShape::Breathe: {
                static float s_env = 0.f;
                if (i == 0) {
                    float k = (masterPeak > s_env) ? 0.04f : 0.015f;
                    s_env += (masterPeak - s_env) * k;
                }
                float rate = 0.55f + VIZ_SEEDS[i % VIZ_BARS_MAX] * 0.18f;
                float inhale =
                    0.5f +
                    0.5f * sinf(t * rate + VIZ_SEEDS[i % VIZ_BARS_MAX] * 1.2f);
                target = max(0.f, min(1.f, inhale * (0.12f + s_env * 0.88f)));
                break;
            }
        }

        g_VizTarget[i] = max(idleFloor, min(1.f, target));
    }
}

// ============================================================================
//  Media state
// ============================================================================

struct MediaSnap {
    wstring title = L"No Media";
    wstring artist = L"";
    bool playing = false;
    bool hasMedia = false;
    bool shuffleOn = false;
    int repeatMode = 0;  // 0=None 1=Track 2=List
    INT64 timelinePos = 0;
    INT64 timelineEnd = 0;
    unique_ptr<Bitmap> art;
    Color primaryColor = Color(255, 18, 18, 18);
    Color secondaryColor = Color(255, 45, 45, 45);
    bool isDarkCover = true;
    mutex mtx;
} g_M;

static wstring g_currentMediaAppAumid;

// Session manager — held for the duration of the mod.
static GlobalSystemMediaTransportControlsSessionManager g_Mgr = nullptr;

// ============================================================================
//  Album palette extraction
// ============================================================================

struct AlbumPalette {
    Color primary;
    Color secondary;
};

static AlbumPalette GetAlbumPalette(Bitmap* bmp) {
    const Color fallbackPrimary(255, 18, 18, 18);
    const Color fallbackSecondary(255, 45, 45, 45);

    if (!bmp || bmp->GetLastStatus() != Ok)
        return {fallbackPrimary, fallbackSecondary};
    UINT w = bmp->GetWidth(), h = bmp->GetHeight();
    if (w == 0 || h == 0)
        return {fallbackPrimary, fallbackSecondary};

    BitmapData data;
    Rect r(0, 0, (INT)w, (INT)h);
    if (bmp->LockBits(&r, ImageLockModeRead, PixelFormat32bppARGB, &data) != Ok)
        return {fallbackPrimary, fallbackSecondary};

    long long r1 = 0, g1 = 0, b1 = 0, s1 = 0, r2 = 0, g2 = 0, b2 = 0, s2 = 0;
    DWORD* pixels = (DWORD*)data.Scan0;
    int stride = data.Stride / 4;
    UINT midX = w / 2;

    for (UINT y = 0; y < h; y += 4) {
        for (UINT x = 0; x < w; x += 4) {
            DWORD p = pixels[y * stride + x];
            BYTE pr = (p >> 16) & 0xFF;
            BYTE pg = (p >> 8) & 0xFF;
            BYTE pb = p & 0xFF;
            if (x < midX) {
                r1 += pr;
                g1 += pg;
                b1 += pb;
                s1++;
            } else {
                r2 += pr;
                g2 += pg;
                b2 += pb;
                s2++;
            }
        }
    }
    bmp->UnlockBits(&data);

    Color primary =
        s1 > 0 ? Color(255, (BYTE)(r1 / s1), (BYTE)(g1 / s1), (BYTE)(b1 / s1))
               : fallbackPrimary;
    Color secondary =
        s2 > 0 ? Color(255, (BYTE)(r2 / s2), (BYTE)(g2 / s2), (BYTE)(b2 / s2))
               : fallbackSecondary;

    int diff = abs((int)primary.GetR() - (int)secondary.GetR()) +
               abs((int)primary.GetG() - (int)secondary.GetG()) +
               abs((int)primary.GetB() - (int)secondary.GetB());
    if (diff < 60)
        secondary = Color(255, (BYTE)(primary.GetR() * 0.35f),
                          (BYTE)(primary.GetG() * 0.35f),
                          (BYTE)(primary.GetB() * 0.35f));

    return {primary, secondary};
}

// ============================================================================
//  Blur background cache
// ============================================================================

static void UpdateAlbumBlurBg(Bitmap* art, int w, int h, int artVersion) {
    if (!art || w <= 0 || h <= 0) {
        g_BlurCache.Invalidate();
        return;
    }
    if (g_BlurCache.bitmap && g_BlurCache.width == w &&
        g_BlurCache.height == h && g_BlurCache.artVersion == artVersion)
        return;

    int srcW = art->GetWidth(), srcH = art->GetHeight();
    int smallW = max(1, srcW / 64), smallH = max(1, srcH / 64);
    Bitmap small(smallW, smallH, PixelFormat32bppPARGB);
    {
        Graphics sg(&small);
        sg.SetInterpolationMode(InterpolationModeBilinear);
        sg.DrawImage(art, 0, 0, smallW, smallH);
    }

    Bitmap* bg = new Bitmap(w, h, PixelFormat32bppPARGB);
    if (bg && bg->GetLastStatus() == Ok) {
        Graphics gg(bg);
        gg.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        float scaleW = (float)w / smallW, scaleH = (float)h / smallH;
        float sc = max(scaleW, scaleH) * 1.02f;
        float dw = smallW * sc, dh = smallH * sc;
        float ox = (w - dw) / 2.f, oy = (h - dh) / 2.f;
        ImageAttributes attr;
        attr.SetWrapMode(WrapModeTileFlipXY);
        gg.DrawImage(&small, RectF(ox, oy, dw, dh), 0.f, 0.f, (REAL)smallW,
                     (REAL)smallH, UnitPixel, &attr);

        delete g_BlurCache.bitmap;
        g_BlurCache.bitmap = bg;
        g_BlurCache.width = w;
        g_BlurCache.height = h;
        g_BlurCache.artVersion = artVersion;
    } else {
        delete bg;
    }
}

// ============================================================================
//  GSMTC helpers
// ============================================================================

static Bitmap* ToBitmap(IRandomAccessStreamWithContentType const& s) {
    if (!s)
        return nullptr;
    try {
        IStream* ns = nullptr;
        if (SUCCEEDED(CreateStreamOverRandomAccessStream(
                reinterpret_cast<IUnknown*>(winrt::get_abi(s)),
                IID_PPV_ARGS(&ns)))) {
            Bitmap* b = Bitmap::FromStream(ns);
            ns->Release();
            if (b && b->GetLastStatus() == Ok)
                return b;
            delete b;
        }
    } catch (...) {
    }
    return nullptr;
}

// ============================================================================
//  Volume management
// ============================================================================

static com_ptr<ISimpleAudioVolume> FindAppVolumeInterface(
    const wstring& aumid) {
    com_ptr<IMMDeviceEnumerator> pEnum;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                CLSCTX_ALL, IID_PPV_ARGS(&pEnum))))
        return nullptr;

    com_ptr<IMMDevice> pDevice;
    if (FAILED(
            pEnum->GetDefaultAudioEndpoint(eRender, eConsole, pDevice.put())))
        return nullptr;

    com_ptr<IAudioSessionManager2> pMgr;
    if (FAILED(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                 nullptr,
                                 reinterpret_cast<void**>(pMgr.put()))))
        return nullptr;

    com_ptr<IAudioSessionEnumerator> pEnum2;
    if (FAILED(pMgr->GetSessionEnumerator(pEnum2.put())))
        return nullptr;

    int count = 0;
    pEnum2->GetCount(&count);
    wstring lower = aumid;
    transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

    for (int i = 0; i < count; i++) {
        com_ptr<IAudioSessionControl> pCtrl;
        if (FAILED(pEnum2->GetSession(i, pCtrl.put())))
            continue;
        auto pCtrl2 = pCtrl.as<IAudioSessionControl2>();
        if (!pCtrl2)
            continue;

        DWORD pid = 0;
        pCtrl2->GetProcessId(&pid);
        if (!pid)
            continue;

        HANDLE hProc =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProc)
            continue;

        WCHAR path[MAX_PATH];
        DWORD sz = MAX_PATH;
        bool matched = false;
        if (QueryFullProcessImageNameW(hProc, 0, path, &sz)) {
            wstring exe = path;
            size_t sl = exe.find_last_of(L"\\");
            if (sl != wstring::npos)
                exe = exe.substr(sl + 1);
            size_t dot = exe.find_last_of(L".");
            if (dot != wstring::npos)
                exe = exe.substr(0, dot);
            transform(exe.begin(), exe.end(), exe.begin(), ::towlower);
            matched = (lower.find(exe) != wstring::npos ||
                       exe.find(lower) != wstring::npos);
        }
        CloseHandle(hProc);

        if (matched) {
            auto pVol = pCtrl2.as<ISimpleAudioVolume>();
            if (pVol)
                return pVol;
        }
    }
    return nullptr;
}

static com_ptr<ISimpleAudioVolume> GetVolumeIface(bool forceRefresh = false) {
    if (g_currentMediaAppAumid.empty())
        return nullptr;
    if (forceRefresh || g_Vol.cachedAumid != g_currentMediaAppAumid ||
        !g_Vol.cachedIface) {
        g_Vol.cachedIface = FindAppVolumeInterface(g_currentMediaAppAumid);
        g_Vol.cachedAumid = g_currentMediaAppAumid;
    }
    return g_Vol.cachedIface;
}

static void FetchAppVolume() {
    auto pVol = GetVolumeIface();
    if (!pVol)
        return;
    if (FAILED(pVol->GetMasterVolume(&g_Vol.level))) {
        pVol = GetVolumeIface(true);
        if (pVol)
            pVol->GetMasterVolume(&g_Vol.level);
    }
}

static void SetVolume(float level) {
    g_Vol.level = max(0.f, min(1.f, level));
    auto pVol = GetVolumeIface();
    if (!pVol)
        return;
    if (FAILED(pVol->SetMasterVolume(g_Vol.level, nullptr))) {
        pVol = GetVolumeIface(true);
        if (pVol)
            pVol->SetMasterVolume(g_Vol.level, nullptr);
    }
}

// ============================================================================
//  Media fetch
// ============================================================================

static void FetchMedia() {
    try {
        if (!g_Mgr)
            g_Mgr =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync()
                    .get();
        if (!g_Mgr)
            return;

        auto sessions = g_Mgr.GetSessions();
        GlobalSystemMediaTransportControlsSession ses = nullptr;

        // Priority: playing session
        for (auto const& s : sessions) {
            try {
                if (s &&
                    s.GetPlaybackInfo().PlaybackStatus() ==
                        GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                            Playing) {
                    ses = s;
                    break;
                }
            } catch (...) {
            }
        }

        // Filter by focused app if set
        if (!g_US.focusedApp.empty()) {
            ses = nullptr;
            wstring lf = g_US.focusedApp;
            transform(lf.begin(), lf.end(), lf.begin(), ::towlower);
            if (lf.size() > 4 && lf.substr(lf.size() - 4) == L".exe")
                lf = lf.substr(0, lf.size() - 4);
            for (auto const& s : sessions) {
                try {
                    wstring id = s.SourceAppUserModelId().c_str();
                    transform(id.begin(), id.end(), id.begin(), ::towlower);
                    if (id.find(lf) != wstring::npos) {
                        ses = s;
                        break;
                    }
                } catch (...) {
                }
            }
        }

        // Fallback: last active session
        if (!ses)
            try {
                ses = g_Mgr.GetCurrentSession();
            } catch (...) {
            }

        if (ses) {
            // ── Media properties ─────────────────────────────────────────────
            GlobalSystemMediaTransportControlsSessionMediaProperties props =
                nullptr;
            try {
                props = ses.TryGetMediaPropertiesAsync().get();
            } catch (...) {
                scoped_lock lk(g_M.mtx);
                g_M.playing = false;
                return;
            }
            if (!props) {
                scoped_lock lk(g_M.mtx);
                g_M.playing = false;
                return;
            }

            try {
                g_currentMediaAppAumid = ses.SourceAppUserModelId().c_str();
            } catch (...) {
            }
            FetchAppVolume();

            scoped_lock lk(g_M.mtx);
            wstring nt = L"Unknown";
            try {
                nt = props.Title().c_str();
            } catch (...) {
            }

            if (nt != g_M.title || !g_M.art) {
                // Track changed — trigger text crossfade.
                // Skip if a fade-out is already running for the same new title
                // (can happen because g_M.title is intentionally not updated
                // until the fade-out completes, so every subsequent poll would
                // see nt != g_M.title and re-trigger otherwise).
                bool alreadyFading =
                    g_Anim.textFadeOut && g_Anim.pendingTitle == nt;
                if (nt != g_M.title && g_M.title != L"No Media" &&
                    !alreadyFading) {
                    g_Anim.pendingTitle = nt;
                    try {
                        g_Anim.pendingArtist = props.Artist().c_str();
                    } catch (...) {
                    }
                    g_Anim.textFadeOut = true;
                    if (g_hWnd)
                        g_Timers.Set(g_hWnd, IDT_TEXT_FADE, 16);
                }
                g_M.art.reset();
                try {
                    auto ref = props.Thumbnail();
                    if (ref) {
                        auto stream = ref.OpenReadAsync().get();
                        if (stream)
                            g_M.art.reset(ToBitmap(stream));
                    }
                } catch (...) {
                }
                ++g_ArtVersion;

                // Aspect ratio for auto-size
                if (g_M.art) {
                    UINT natW = g_M.art->GetWidth(),
                         natH = g_M.art->GetHeight();
                    g_ArtAspectRatio =
                        (natH > 0) ? (float)natW / (float)natH : 1.0f;
                } else {
                    g_ArtAspectRatio = 1.0f;
                }
                g_LayoutDirty = true;

                // Cover brightness (for mask adaptation)
                if (g_M.art) {
                    Bitmap px1(1, 1, PixelFormat32bppARGB);
                    Graphics pg(&px1);
                    pg.SetInterpolationMode(
                        InterpolationModeHighQualityBicubic);
                    pg.DrawImage(g_M.art.get(), 0, 0, 1, 1);
                    Color avg;
                    px1.GetPixel(0, 0, &avg);
                    double lum = 0.299 * avg.GetR() + 0.587 * avg.GetG() +
                                 0.114 * avg.GetB();
                    g_M.isDarkCover = (lum < LUM_DARK_THRESHOLD);
                }

                auto pal = GetAlbumPalette(g_M.art.get());
                g_M.primaryColor = pal.primary;
                g_M.secondaryColor = pal.secondary;
            }

            // Only update title/artist immediately if no crossfade is running.
            // When a crossfade was just triggered the old text must stay
            // visible during the fade-out phase; the IDT_TEXT_FADE handler will
            // swap in the pending (new) values once the alpha reaches zero.
            if (!g_Anim.textFadeOut) {
                g_M.title = nt;
                try {
                    g_M.artist = props.Artist().c_str();
                } catch (...) {
                    g_M.artist = L"";
                }
            }

            // ── Playback state
            // ────────────────────────────────────────────────
            try {
                auto pbi = ses.GetPlaybackInfo();
                bool nowPlaying =
                    (pbi.PlaybackStatus() ==
                     GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                         Playing);
                g_M.playing = nowPlaying;
                if (nowPlaying != g_Progress.isPlaying) {
                    g_Progress.isPlaying = nowPlaying;
                    g_Progress.updateTick = GetTickCount();
                    g_Progress.lastRawPos = g_M.timelinePos;
                }
                try {
                    g_M.shuffleOn = pbi.IsShuffleActive()
                                        ? pbi.IsShuffleActive().Value()
                                        : false;
                } catch (...) {
                }
                try {
                    auto rm = pbi.AutoRepeatMode();
                    if (rm) {
                        using RM = Windows::Media::MediaPlaybackAutoRepeatMode;
                        auto v = rm.Value();
                        g_M.repeatMode = (v == RM::Track)  ? 1
                                         : (v == RM::List) ? 2
                                                           : 0;
                    }
                } catch (...) {
                }
            } catch (...) {
                g_M.playing = false;
            }

            // ── Timeline / progress
            // ───────────────────────────────────────────
            try {
                auto tl = ses.GetTimelineProperties();
                INT64 newPos = tl.Position().count();
                INT64 newEnd = tl.EndTime().count();
                g_M.timelinePos = newPos;
                g_M.timelineEnd = newEnd;
                bool nowP = g_M.playing;
                if (newPos != g_Progress.lastRawPos ||
                    newEnd != g_Progress.endRaw ||
                    nowP != g_Progress.isPlaying) {
                    g_Progress.lastRawPos = newPos;
                    g_Progress.endRaw = newEnd;
                    g_Progress.updateTick = GetTickCount();
                    g_Progress.isPlaying = nowP;
                }
            } catch (...) {
                g_M.timelinePos = g_M.timelineEnd = 0;
            }

            g_M.hasMedia = true;
        } else {
            scoped_lock lk(g_M.mtx);
            g_M.playing = false;
            g_M.hasMedia = false;
            g_Vol.cachedIface = nullptr;
            g_Vol.cachedAumid.clear();
        }
    } catch (...) {
        scoped_lock lk(g_M.mtx);
        g_M.playing = false;
        g_M.hasMedia = false;
    }
}

static void Cmd(MediaCmd c) {
    try {
        if (!g_Mgr)
            return;
        auto s = g_Mgr.GetCurrentSession();
        if (!s)
            return;
        switch (c) {
            case MediaCmd::Prev:
                s.TrySkipPreviousAsync();
                break;
            case MediaCmd::PlayPause:
                s.TryTogglePlayPauseAsync();
                break;
            case MediaCmd::Next:
                s.TrySkipNextAsync();
                break;
            case MediaCmd::Shuffle:
                try {
                    bool cur;
                    {
                        scoped_lock lk(g_M.mtx);
                        cur = g_M.shuffleOn;
                    }
                    s.TryChangeShuffleActiveAsync(!cur);
                } catch (...) {
                }
                break;
            case MediaCmd::Repeat:
                try {
                    int cur;
                    {
                        scoped_lock lk(g_M.mtx);
                        cur = g_M.repeatMode;
                    }
                    using RM = Windows::Media::MediaPlaybackAutoRepeatMode;
                    RM next = (cur == 0)   ? RM::Track
                              : (cur == 1) ? RM::List
                                           : RM::None;
                    s.TryChangeAutoRepeatModeAsync(next);
                } catch (...) {
                }
                break;
            default:
                break;
        }
    } catch (...) {
    }
}

// ============================================================================
//  Timer rate management
// ============================================================================

static void ApplyTimerRates(HWND hwnd) {
    bool playing = false;
    {
        scoped_lock lk(g_M.mtx);
        playing = g_M.playing;
    }
    g_Timers.Set(hwnd, IDT_POLL,
                 playing ? POLL_RATE_PLAYING : POLL_RATE_PAUSED);
    if (playing)
        g_Timers.Set(hwnd, IDT_POS, POS_RATE_PLAYING);
    else
        g_Timers.Kill(IDT_POS);
}

// ============================================================================
//  Layout rebuild
// ============================================================================

static ControlLayout BuildControlLayout(int W,
                                        int H,
                                        int ctrlCellX,
                                        int ctrlCellWidth) {
    const auto& c = g_US.containers[2];
    float cY =
        g_US.wrap.padTop + (H - g_US.wrap.padTop - g_US.wrap.padBottom) / 2.f;

    int btnCount = 0;
    if (g_US.showPlaybackControls)
        btnCount += 3;
    if (g_US.showSpeakerIcon)
        btnCount += 1;
    if (g_US.showRepeatButton)
        btnCount += 1;
    if (g_US.showShuffleButton)
        btnCount += 1;

    float nX = 0, ppX = 0, pX = 0, vX = 0, shuffleX = 0, repeatX = 0;
    float firstX = (float)(ctrlCellX + ctrlCellWidth);

    if (btnCount > 0) {
        float cellLeft = (float)(ctrlCellX + c.padLeft);
        float cellRight = (float)(ctrlCellX + ctrlCellWidth - c.padRight);
        float contentW = cellRight - cellLeft;
        float step = contentW / (float)btnCount;
        float start = cellLeft + step * 0.5f;

        float cursor = start + step * (btnCount - 1);
        if (g_US.showPlaybackControls) {
            nX = cursor;
            cursor -= step;
            ppX = cursor;
            cursor -= step;
            pX = cursor;
            cursor -= step;
        }
        if (g_US.showSpeakerIcon) {
            vX = cursor;
            cursor -= step;
        }
        if (g_US.showRepeatButton) {
            repeatX = cursor;
            cursor -= step;
        }
        if (g_US.showShuffleButton) {
            shuffleX = cursor;
            cursor -= step;
        }
        firstX = start;
    }

    return {nX, ppX, pX, vX, cY, shuffleX, repeatX, firstX};
}

static void RebuildLayout(int W, int H) {
    g_Layout = {};  // reset
    g_Layout.W = W;
    g_Layout.H = H;

    // ── Step 1: walk the order vector, assign X to each container ────────────
    g_Layout.originCount = 0;
    int curX = g_US.wrap.padLeft;
    bool first = true;

    for (int id : g_US.wrap.order) {
        if (!IsContainerEnabled(id))
            continue;
        if (id == 4 && g_US.wrap.vizAsBackground)
            continue;

        if (!first)
            curX += g_US.wrap.globalGap;
        first = false;

        int cellW = GetContainerWidth(id);
        int padL = 0, padR = 0;
        for (const auto& c : g_US.containers)
            if (c.id == id) {
                padL = c.padLeft;
                padR = c.padRight;
                break;
            }

        ContainerOrigin co;
        co.id = id;
        co.cellX = curX;
        co.cellWidth = cellW;
        co.contentX = curX + padL;
        co.contentWidth = max(0, cellW - padL - padR);
        g_Layout.origins[g_Layout.originCount++] = co;

        curX += cellW;
    }

    // ── Step 2: art geometry
    // ──────────────────────────────────────────────────
    int aS, aW;
    CalcArtDims(H, aS, aW);  // H is already the DPI-scaled pixel height

    int aX = g_US.wrap.padLeft;
    if (!g_US.miniMode)
        if (const auto* o = g_Layout.FindOrigin(1))
            aX = o->contentX;

    int aR = min(g_US.mediaCornerRadius, min(aS, aW) / 2);

    g_Layout.art = {aX, g_US.wrap.padTop, aS, aW, aR};

    // ── Step 3: visualizer bar geometry ──────────────────────────────────────
    int vCellX = aX, vCellW = aW;
    if (!g_US.wrap.vizAsBackground)
        if (const auto* o = g_Layout.FindOrigin(4)) {
            vCellX = o->contentX;
            vCellW = o->contentWidth;
        }

    float barZoneH = (float)aS;
    int barCount = max(1, g_US.vizBars);
    float gap = max(0.f, (float)g_US.vizBarGap);
    float bW = max(1.5f, (vCellW - gap * (barCount - 1)) / (float)barCount);
    float bStep = bW + gap;
    float bStartX = vCellX + (vCellW - (bStep * barCount - gap)) * 0.5f;

    g_Layout.viz = {
        (float)g_US.wrap.padTop, barZoneH,         bW, bStep, bStartX,
        barZoneH * 0.92f,        barZoneH * 0.035f};

    // ── Step 4: controls layout
    // ───────────────────────────────────────────────
    int ctrlCellX = W - g_US.wrap.padRight;
    int ctrlCellW = 0;
    if (const auto* o = g_Layout.FindOrigin(3)) {
        ctrlCellX = o->cellX;
        ctrlCellW = o->cellWidth;
    }
    g_Layout.ctrl = BuildControlLayout(W, H, ctrlCellX, ctrlCellW);

    g_Layout.valid = true;
    g_LayoutDirty = false;
}

static void EnsureLayout(int W, int H) {
    if (g_LayoutDirty || g_Layout.W != W || g_Layout.H != H)
        RebuildLayout(W, H);
}

// ============================================================================
//  GDI+ path helpers
// ============================================================================

static void RoundRectPath(GraphicsPath& p, int x, int y, int w, int h, int r) {
    if (r <= 0) {
        p.AddRectangle(Rect(x, y, w, h));
        p.CloseFigure();
        return;
    }
    int d = r * 2;
    p.AddArc(x, y, d, d, 180, 90);
    p.AddArc(x + w - d, y, d, d, 270, 90);
    p.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    p.AddArc(x, y + h - d, d, d, 90, 90);
    p.CloseFigure();
}

static void CustomRoundedPath(GraphicsPath& p,
                              float x,
                              float y,
                              float w,
                              float h,
                              int rTL,
                              int rTR,
                              int rBR,
                              int rBL) {
    int maxW = (int)(w / 2.f), maxH = (int)(h / 2.f);
    auto clamp = [&](int r) { return max(0, min(r, min(maxW, maxH))); };
    rTL = clamp(rTL);
    rTR = clamp(rTR);
    rBR = clamp(rBR);
    rBL = clamp(rBL);

    if (rTL > 0)
        p.AddArc(x, y, rTL * 2.f, rTL * 2.f, 180, 90);
    else
        p.AddLine(PointF(x, y), PointF(x, y));
    if (rTR > 0)
        p.AddArc(x + w - rTR * 2.f, y, rTR * 2.f, rTR * 2.f, 270, 90);
    else
        p.AddLine(PointF(x + w, y), PointF(x + w, y));
    if (rBR > 0)
        p.AddArc(x + w - rBR * 2.f, y + h - rBR * 2.f, rBR * 2.f, rBR * 2.f, 0,
                 90);
    else
        p.AddLine(PointF(x + w, y + h), PointF(x + w, y + h));
    if (rBL > 0)
        p.AddArc(x, y + h - rBL * 2.f, rBL * 2.f, rBL * 2.f, 90, 90);
    else
        p.AddLine(PointF(x, y + h), PointF(x, y + h));
    p.CloseFigure();
}

// Full-panel rounded clip path using the current corner radius setting.
static void PanelPath(GraphicsPath& p, int W, int H) {
    if (g_US.roundedCorners) {
        int r = g_US.cornerRadius;
        CustomRoundedPath(p, 0.f, 0.f, (float)W, (float)H, r, r, r, r);
    } else {
        p.AddRectangle(Rect(0, 0, W, H));
    }
}

// ============================================================================
//  Colour helpers
// ============================================================================

static Color LerpColor(Color a, Color b, float t) {
    return Color(255, (BYTE)(a.GetR() + (b.GetR() - a.GetR()) * t),
                 (BYTE)(a.GetG() + (b.GetG() - a.GetG()) * t),
                 (BYTE)(a.GetB() + (b.GetB() - a.GetB()) * t));
}

static Color LerpDWORD(DWORD c1, DWORD c2, float t) {
    return LerpColor(Color(255, (BYTE)((c1 >> 16) & 0xFF),
                           (BYTE)((c1 >> 8) & 0xFF), (BYTE)(c1 & 0xFF)),
                     Color(255, (BYTE)((c2 >> 16) & 0xFF),
                           (BYTE)((c2 >> 8) & 0xFF), (BYTE)(c2 & 0xFF)),
                     t);
}

static ARGB TextARGB() {
    if (g_US.autoTheme)
        return IsLightCached() ? 0xFF1a1a1a : 0xFFFFFFFF;
    return g_US.manualTextColor;
}

// ============================================================================
//  Appearance
// ============================================================================

void ApplyAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE cp =
        g_US.roundedCorners ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cp,
                          sizeof(cp));

    COLORREF borderCol =
        g_US.showBorder ? DWMWA_COLOR_DEFAULT : DWMWA_COLOR_NONE;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &borderCol,
                          sizeof(borderCol));

    HMODULE hu = GetModuleHandle(L"user32.dll");
    if (!hu)
        return;
    auto fn = (pSetWindowCompositionAttribute)GetProcAddress(
        hu, "SetWindowCompositionAttribute");
    if (!fn)
        return;

    ACCENT_POLICY pol = {};
    WINDOWCOMPOSITIONATTRIBDATA d = {WCA_ACCENT_POLICY, &pol, sizeof(pol)};

    Theme t = g_US.theme;
    bool softBackground =
        (t == Theme::Transparent || t == Theme::GradientFade ||
         t == Theme::GradientLR || t == Theme::GradientRL ||
         t == Theme::GradientVertical || t == Theme::GradientVertInv ||
         t == Theme::GradientRadial || t == Theme::Split ||
         t == Theme::Aurora || t == Theme::AlbumBlur);

    if (softBackground) {
        pol = {ACCENT_DISABLED, 0, 0, 0};
        fn(hwnd, &d);
        MARGINS m = {-1};
        DwmExtendFrameIntoClientArea(hwnd, &m);
    } else {
        MARGINS m = {0};
        DwmExtendFrameIntoClientArea(hwnd, &m);
        if (t == Theme::Windows) {
            pol = {ACCENT_ENABLE_BLURBEHIND, 0, 0, 0};
        } else if (t == Theme::Blurred || t == Theme::Neon) {
            bool light = g_US.autoTheme && IsLightCached();
            BYTE blurAlpha =
                (BYTE)(max(8, min(180, (int)(g_US.themeBlur * 1.4f))));
            BYTE finalAlpha = (BYTE)(blurAlpha * (g_US.themeOpacity / 255.f));
            DWORD baseColor = light ? 0x00FFFFFF : 0x00000000;
            pol = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 0,
                   ((DWORD)finalAlpha << 24) | baseColor, 0};
        }
        fn(hwnd, &d);
    }
}

// ============================================================================
//  Taskbar visibility
// ============================================================================

static bool IsTaskbarEffectivelyVisible(HWND hTaskbar) {
    if (!hTaskbar || !IsWindowVisible(hTaskbar))
        return false;
    RECT rc;
    GetWindowRect(hTaskbar, &rc);
    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return true;
    RECT intersect;
    if (!IntersectRect(&intersect, &rc, &mi.rcMonitor))
        return false;
    return (intersect.right - intersect.left > 4) &&
           (intersect.bottom - intersect.top > 4);
}

static void UpdateBoundsAndRegion(HWND hwnd) {
    HWND tb = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!tb)
        return;
    RECT tbRect;
    GetWindowRect(tb, &tbRect);

    UINT dpi = GetDpiForWindow(hwnd);
    if (dpi == 0)
        dpi = 96;
    float scale = dpi / 96.f;

    int tbH = tbRect.bottom - tbRect.top;
    int panelH = (int)(g_US.panelHeight * scale);

    // Recompute total panel width in screen pixels
    int totalW = g_US.wrap.padLeft + g_US.wrap.padRight;
    bool firstC = true;
    if (g_US.miniMode) {
        for (int id : {1, 4}) {
            if (!IsContainerEnabled(id))
                continue;
            if (!firstC)
                totalW += g_US.wrap.globalGap;
            totalW += GetContainerWidth(id);
            firstC = false;
        }
    } else {
        for (int id : g_US.wrap.order) {
            if (!IsContainerEnabled(id))
                continue;
            if (id == 4 && g_US.wrap.vizAsBackground)
                continue;
            if (!firstC)
                totalW += g_US.wrap.globalGap;
            totalW += GetContainerWidth(id);
            firstC = false;
        }
    }
    int panelW = (int)(totalW * scale);

    int posX = tbRect.left + (int)(g_US.offsetX * scale);
    int posY =
        tbRect.top + (tbH / 2) - (panelH / 2) + (int)(g_US.offsetY * scale);

    int cr = (int)(g_US.cornerRadius * scale);
    HRGN hRgn =
        g_US.roundedCorners
            ? CreateRoundRectRgn(0, 0, panelW + 1, panelH + 1, cr * 2, cr * 2)
            : CreateRectRgn(0, 0, panelW, panelH);

    SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, panelW, panelH,
                 SWP_SHOWWINDOW);
    SetWindowRgn(hwnd, hRgn, TRUE);
    g_LayoutDirty = true;
}

// ============================================================================
//  Draw — per-container functions
// ============================================================================

// Shared draw-context passed to each container function.
struct DrawCtx {
    Graphics& g;
    int W, H;
    Color bgPrimary, bgSecondary;
    Color textColor, artistColor;
    Color hoverTarget, ctrlBase;
    float iconScale;
    float ppBigR, pnSmallR;
    Bitmap* art;
    bool playing;
    wstring title, artist;
};

static DrawCtx MakeDrawCtx(Graphics& g,
                           int W,
                           int H,
                           Color bgPrimary,
                           Color bgSecondary,
                           bool idleState,
                           wstring& title_out,
                           wstring& artist_out,
                           Bitmap*& art_out,
                           bool& playing_out) {
    {
        scoped_lock lk(g_M.mtx);
        title_out = idleState ? L"Title" : g_M.title;
        artist_out = idleState ? L"Author" : g_M.artist;
        playing_out = g_M.playing;
        art_out = idleState ? nullptr : g_M.art.get();
    }

    Color tc(TextARGB());
    BYTE artistBase = IsLightCached() ? 0 : 255;
    Color artistColor(153, artistBase, artistBase, artistBase);

    // Controls hover color
    Color hoverTarget;
    if (g_US.dynamicHover)
        hoverTarget =
            Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    else
        hoverTarget = Color(255, (BYTE)((g_US.hoverColor >> 16) & 0xFF),
                            (BYTE)((g_US.hoverColor >> 8) & 0xFF),
                            (BYTE)(g_US.hoverColor & 0xFF));

    // Controls icon base color
    Color ctrlBase;
    if (g_US.ctrlIconDynamic)
        ctrlBase =
            Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    else if (g_US.ctrlIconColor != 0xFF000000)
        ctrlBase = Color(255, (BYTE)((g_US.ctrlIconColor >> 16) & 0xFF),
                         (BYTE)((g_US.ctrlIconColor >> 8) & 0xFF),
                         (BYTE)(g_US.ctrlIconColor & 0xFF));
    else
        ctrlBase = tc;

    float _scale = H / PANEL_HEIGHT_REF;
    float iconScale = (g_US.iconSize / ICON_SIZE_REF) * _scale;

    return DrawCtx{g,
                   W,
                   H,
                   bgPrimary,
                   bgSecondary,
                   tc,
                   artistColor,
                   hoverTarget,
                   ctrlBase,
                   iconScale,
                   ICON_SIZE_REF * iconScale,
                   9.f * iconScale,
                   art_out,
                   playing_out,
                   title_out,
                   artist_out};
}

// ── Background
// ────────────────────────────────────────────────────────────────

static void DrawBackground(DrawCtx& ctx) {
    Graphics& g = ctx.g;
    int W = ctx.W, H = ctx.H;
    Color& p = ctx.bgPrimary;
    Color& s = ctx.bgSecondary;

    auto FillShape = [&](Brush* br) {
        if (g_US.roundedCorners) {
            GraphicsPath bp;
            PanelPath(bp, W, H);
            g.FillPath(br, &bp);
        } else {
            g.FillRectangle(br, 0, 0, W, H);
        }
    };

    switch (g_US.theme) {
        case Theme::GradientFade: {
            Color l(255, p.GetR(), p.GetG(), p.GetB()),
                r(0, p.GetR(), p.GetG(), p.GetB());
            LinearGradientBrush br(Rect(0, 0, W, H), l, r,
                                   LinearGradientModeHorizontal);
            FillShape(&br);
            break;
        }
        case Theme::GradientLR: {
            LinearGradientBrush br(Rect(0, 0, W, H), p, s,
                                   LinearGradientModeHorizontal);
            FillShape(&br);
            break;
        }
        case Theme::GradientRL: {
            LinearGradientBrush br(Rect(0, 0, W, H), s, p,
                                   LinearGradientModeHorizontal);
            FillShape(&br);
            break;
        }
        case Theme::GradientVertical: {
            LinearGradientBrush br(Rect(0, 0, W, H), p, s,
                                   LinearGradientModeVertical);
            FillShape(&br);
            break;
        }
        case Theme::GradientVertInv: {
            LinearGradientBrush br(Rect(0, 0, W, H), s, p,
                                   LinearGradientModeVertical);
            FillShape(&br);
            break;
        }
        case Theme::GradientRadial: {
            GraphicsPath gp;
            PanelPath(gp, W, H);
            PathGradientBrush pgb(&gp);
            int cnt = 1;
            pgb.SetCenterColor(p);
            pgb.SetSurroundColors(&s, &cnt);
            pgb.SetCenterPoint(PointF(W * 0.3f, H * 0.5f));
            g.FillPath(&pgb, &gp);
            break;
        }
        case Theme::Split: {
            LinearGradientBrush br(Rect(0, 0, W, H), p, s,
                                   LinearGradientModeHorizontal);
            Color blend[] = {p, p, s, s};
            REAL pos[] = {0.0f, 0.49f, 0.51f, 1.0f};
            br.SetInterpolationColors(blend, pos, 4);
            FillShape(&br);
            break;
        }
        case Theme::Neon: {
            BYTE a = (BYTE)(max(10, (int)(40.f * g_US.themeOpacity / 255.f)));
            Color ov = IsLightCached() ? Color(a, 255, 255, 255)
                                       : Color(a, 20, 20, 20);
            SolidBrush b(ov);
            FillShape(&b);
            Pen pen(p, 2.0f);
            if (g_US.roundedCorners) {
                GraphicsPath bp;
                CustomRoundedPath(bp, 1.f, 1.f, (float)(W - 2), (float)(H - 2),
                                  max(0, g_US.cornerRadius - 1),
                                  max(0, g_US.cornerRadius - 1),
                                  max(0, g_US.cornerRadius - 1),
                                  max(0, g_US.cornerRadius - 1));
                g.DrawPath(&pen, &bp);
            } else {
                g.DrawRectangle(&pen, 1, 1, W - 2, H - 2);
            }
            break;
        }
        case Theme::Aurora: {
            Color dark(255, max(0, (int)p.GetR() - 50),
                       max(0, (int)p.GetG() - 50), max(0, (int)p.GetB() - 50));
            LinearGradientBrush br(Rect(0, 0, W, H), p, p,
                                   LinearGradientModeHorizontal);
            Color blend[] = {p, s, dark};
            REAL pos[] = {0.0f, 0.5f, 1.0f};
            br.SetInterpolationColors(blend, pos, 3);
            FillShape(&br);
            break;
        }
        case Theme::AlbumBlur: {
            scoped_lock lk(g_M.mtx);
            if (g_M.art)
                UpdateAlbumBlurBg(g_M.art.get(), W, H, g_ArtVersion);
            if (g_BlurCache.bitmap)
                g.DrawImage(g_BlurCache.bitmap, 0, 0);
            if (g_US.enableMask) {
                BYTE tone = g_M.isDarkCover ? 20 : 235;
                SolidBrush mb(Color((BYTE)g_US.maskOpacity, tone, tone, tone));
                if (g_US.roundedCorners) {
                    GraphicsPath mp;
                    PanelPath(mp, W, H);
                    g.FillPath(&mb, &mp);
                } else
                    g.FillRectangle(&mb, 0, 0, W, H);
            }
            return;  // mask already handled above for AlbumBlur
        }
        case Theme::Windows: {
            SolidBrush b(Color(200, 20, 20, 20));
            FillShape(&b);
            break;
        }
        default:
            break;  // Transparent / Blurred — OS handles it
    }

    // Mask overlay for all non-AlbumBlur themes
    if (g_US.enableMask) {
        bool light = IsLightCached();
        BYTE tone = light ? 235 : 20;
        SolidBrush mb(Color((BYTE)g_US.maskOpacity, tone, tone, tone));
        if (g_US.roundedCorners) {
            GraphicsPath mp;
            PanelPath(mp, W, H);
            g.FillPath(&mb, &mp);
        } else
            g.FillRectangle(&mb, 0, 0, W, H);
    }
}

// ── Media / Album Art
// ─────────────────────────────────────────────────────────

static void DrawMediaContainer(DrawCtx& ctx) {
    if (!IsContainerEnabled(1))
        return;
    Graphics& g = ctx.g;
    const ArtLayout& a = g_Layout.art;
    int aX = a.x, aY = a.y, aW = a.width, aS = a.size, aR = a.cornerRadius;

    GraphicsPath ap;
    CustomRoundedPath(ap, (float)aX, (float)aY, (float)aW, (float)aS, aR, aR,
                      aR, aR);

    if (ctx.art) {
        // Use TextureBrush + FillPath instead of SetClip + DrawImage.
        // SetClip produces a hard pixel-aligned edge regardless of
        // SmoothingMode; FillPath respects anti-aliasing and gives smooth
        // rounded corners.
        Bitmap scaled(aW, aS, PixelFormat32bppPARGB);
        {
            Graphics sg(&scaled);
            sg.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            sg.SetSmoothingMode(SmoothingModeAntiAlias);
            sg.DrawImage(ctx.art, 0, 0, aW, aS);
        }
        TextureBrush tb(&scaled, WrapModeClamp);
        Matrix m;
        m.Translate((float)aX, (float)aY);
        tb.SetTransform(&m);
        g.FillPath(&tb, &ap);
    } else if (g_US.mediaPlaceholderIcon) {
        bool light = IsLightCached();
        Color phBg = light ? Color(255, 200, 200, 200) : Color(255, 55, 55, 60);
        Color phFg =
            light ? Color(255, 130, 130, 135) : Color(255, 100, 100, 108);
        SolidBrush bgBr(phBg);
        g.FillPath(&bgBr, &ap);
        int minDim = min(aW, aS);
        if (minDim >= 14) {
            FontFamily ff(ICON_FONT, nullptr);
            Font f(&ff, (REAL)(minDim * 0.45f), FontStyleRegular, UnitPixel);
            RectF meas;
            g.MeasureString(L"\uE8D6", -1, &f, RectF(0, 0, 4000, 200), &meas);
            SolidBrush fgBr(phFg);
            g.DrawString(L"\uE8D6", -1, &f,
                         PointF(aX + (aW - meas.Width) / 2.f,
                                aY + (aS - meas.Height) / 2.f),
                         &fgBr);
        }
    } else {
        // Solid placeholder background with no icon
        bool light = IsLightCached();
        Color phBg = light ? Color(255, 200, 200, 200) : Color(255, 55, 55, 60);
        SolidBrush bgBr(phBg);
        g.FillPath(&bgBr, &ap);
    }
}

// ── Info (title + artist + progress bar) ─────────────────────────────────────

static void DrawProgressBar(DrawCtx& ctx,
                            const ContainerOrigin* infoCO,
                            const ContainerOrigin* ctrlCO) {
    if (!g_US.showProgressBar)
        return;

    float progress = g_Progress.LiveRatio();
    int W = ctx.W, H = ctx.H;
    const ArtLayout& a = g_Layout.art;
    Graphics& g = ctx.g;

    Color fillColor;
    if (g_US.progressBarDynamic)
        fillColor = Color(220, ctx.bgPrimary.GetR(), ctx.bgPrimary.GetG(),
                          ctx.bgPrimary.GetB());
    else
        fillColor = Color(220, (BYTE)((g_US.progressBarColor >> 16) & 0xFF),
                          (BYTE)((g_US.progressBarColor >> 8) & 0xFF),
                          (BYTE)(g_US.progressBarColor & 0xFF));

    int pbH = g_US.progressBarHeight;
    int pbX, pbW, pbY;

    const float pnR = ctx.pnSmallR;
    const float& nX = g_Layout.ctrl.nX;
    const float& fX = g_Layout.ctrl.firstControlX;

    switch (g_US.progressBarLayer) {
        case PBLayer::AboveBoth:
            pbX = 0;
            pbW = W;
            pbY = g_US.progressBarOffsetY;
            break;
        case PBLayer::UnderText:
            pbX = a.x + a.width + g_US.progressBarPadLeft;
            pbW = W - pbX - g_US.progressBarPadRight;
            pbY = H - pbH - 4 - g_US.progressBarOffsetY;
            break;
        case PBLayer::UnderTextOnly:
            pbX = infoCO ? (infoCO->contentX + g_US.progressBarPadLeft)
                         : g_US.progressBarPadLeft;
            pbW =
                infoCO
                    ? (infoCO->contentWidth - g_US.progressBarPadLeft -
                       g_US.progressBarPadRight)
                    : (W - g_US.progressBarPadLeft - g_US.progressBarPadRight);
            pbY = H - pbH - 4 - g_US.progressBarOffsetY;
            break;
        case PBLayer::UnderControlsOnly:
            pbX = (int)(fX - pnR) + g_US.progressBarPadLeft;
            pbW = (int)(nX + pnR) - pbX - g_US.progressBarPadRight;
            pbY = H - pbH - 4 - g_US.progressBarOffsetY;
            break;
        default:  // UnderBoth
            pbX = 0;
            pbW = W;
            pbY = H - pbH - g_US.progressBarOffsetY;
            break;
    }

    if (pbW <= 4)
        return;

    Color trackColor(35, ctx.textColor.GetR(), ctx.textColor.GetG(),
                     ctx.textColor.GetB());
    SolidBrush trackBr(trackColor);
    GraphicsPath trackPath;
    RoundRectPath(trackPath, pbX, pbY, pbW, pbH, pbH / 2);
    g.FillPath(&trackBr, &trackPath);

    int filledW = (int)(progress * pbW);
    if (filledW > 1) {
        SolidBrush fillBr(fillColor);
        GraphicsPath fillPath;
        RoundRectPath(fillPath, pbX, pbY, filledW, pbH, pbH / 2);
        g.FillPath(&fillBr, &fillPath);
    }
}

static void DrawInfoContainer(DrawCtx& ctx) {
    if (!IsContainerEnabled(2) || g_US.miniMode)
        return;

    const ContainerOrigin* co = g_Layout.FindOrigin(2);
    if (!co)
        return;

    Graphics& g = ctx.g;
    int tX = co->contentX;
    int tW = co->contentWidth;
    if (tW < 8)
        tW = 8;

    FontFamily ff(FONT_NAME, nullptr);
    FontFamily semiBold(L"Segoe UI Semibold", nullptr);
    Font titleF(&ff, (REAL)(g_US.fontSize + 1), FontStyleBold, UnitPixel);
    Font artistF(&semiBold, (REAL)(g_US.fontSize - 1), FontStyleRegular,
                 UnitPixel);

    RectF tb, ab;
    g.MeasureString(ctx.title.c_str(), -1, &titleF, RectF(0, 0, 4000, 200),
                    &tb);
    if (!ctx.artist.empty())
        g.MeasureString(ctx.artist.c_str(), -1, &artistF,
                        RectF(0, 0, 4000, 200), &ab);

    float gap = (float)g_US.textGap;
    float totalH = tb.Height + (ctx.artist.empty() ? 0.f : ab.Height + gap);

    float pbReserve = 0.f;
    if (g_US.showProgressBar &&
        (g_US.progressBarLayer == PBLayer::UnderText ||
         g_US.progressBarLayer == PBLayer::UnderTextOnly))
        pbReserve = (float)g_US.progressBarHeight + 4.f;

    float sY = g_US.wrap.padTop + (ctx.H - g_US.wrap.padTop -
                                   g_US.wrap.padBottom - totalH - pbReserve) /
                                      2.f;

    // Truncation helper for when scrolling is disabled
    auto TruncateToFit = [&](const wstring& str, const Font& fnt,
                             int maxW) -> wstring {
        if (maxW <= 0)
            return L"…";
        RectF m;
        g.MeasureString(str.c_str(), -1, &fnt, RectF(0, 0, 4000, 200), &m);
        if ((int)m.Width <= maxW)
            return str;
        int lo = 0, hi = (int)str.size();
        while (lo < hi) {
            int mid = (lo + hi + 1) / 2;
            wstring c = str.substr(0, mid) + L"…";
            g.MeasureString(c.c_str(), -1, &fnt, RectF(0, 0, 4000, 200), &m);
            if ((int)m.Width <= maxW)
                lo = mid;
            else
                hi = mid - 1;
        }
        return str.substr(0, lo) + L"…";
    };

    g_Scroll.textW = (int)tb.Width;
    g_Scroll.visW = tW;
    g_Scroll.active = (!g_US.disableScroll && g_Scroll.textW > tW);

    wstring dispTitle = (g_US.disableScroll && g_Scroll.textW > tW)
                            ? TruncateToFit(ctx.title, titleF, tW)
                            : ctx.title;

    wstring dispArtist = ctx.artist;
    if (g_US.disableScroll && !ctx.artist.empty()) {
        RectF am;
        g.MeasureString(ctx.artist.c_str(), -1, &artistF,
                        RectF(0, 0, 4000, 200), &am);
        if ((int)am.Width > tW)
            dispArtist = TruncateToFit(ctx.artist, artistF, tW);
    }

    float alpha = g_Anim.textFadeAlpha;
    Color tcFaded(max(0, (int)(ctx.textColor.GetA() * alpha)),
                  ctx.textColor.GetR(), ctx.textColor.GetG(),
                  ctx.textColor.GetB());
    Color acFaded(max(0, (int)(ctx.artistColor.GetA() * alpha)),
                  ctx.artistColor.GetR(), ctx.artistColor.GetG(),
                  ctx.artistColor.GetB());
    SolidBrush titleBr(tcFaded), artistBr(acFaded);

    Region clip(Rect(tX, 0, tW, ctx.H));
    g.SetClip(&clip);

    float drawX = g_Scroll.active ? (float)tX - g_Scroll.offset : (float)tX;
    if (!g_Scroll.active) {
        g_Scroll.offset = 0;
        g_Scroll.forward = true;
    }

    g.DrawString(dispTitle.c_str(), -1, &titleF, PointF(drawX, sY), &titleBr);
    if (!ctx.artist.empty())
        g.DrawString(dispArtist.c_str(), -1, &artistF,
                     PointF((float)tX, sY + tb.Height + gap), &artistBr);

    g.ResetClip();

    // Fade edge for Windows-adaptive theme
    if (g_US.theme == Theme::Windows && tW > 20) {
        int fw = 24, fx = tX + tW - fw;
        LinearGradientBrush fade(PointF((float)fx, 0),
                                 PointF((float)(fx + fw), 0),
                                 Color(0, 20, 20, 20), Color(200, 20, 20, 20));
        g.FillRectangle(&fade, fx, 0, fw, ctx.H);
    }

    // Progress bar (under-info positions)
    if (g_US.showProgressBar &&
        (g_US.progressBarLayer == PBLayer::UnderText ||
         g_US.progressBarLayer == PBLayer::UnderTextOnly)) {
        const ContainerOrigin* ctrlCO = g_Layout.FindOrigin(3);
        DrawProgressBar(ctx, co, ctrlCO);
    }

    // Keep animation timer alive while scrolling or playing
    if (g_Scroll.active || (g_US.showProgressBar && g_Progress.isPlaying)) {
        if (!g_AnimTimerRunning) {
            g_Timers.Set(g_hWnd, IDT_ANIM, 16);
            g_AnimTimerRunning = true;
        }
    }
}

// ── Visualizer
// ────────────────────────────────────────────────────────────────

// Renders bars into the given zone; used for both the slot and the bg-layer.
static void DrawVisualizerBars(DrawCtx& ctx,
                               float zoneY,
                               float zoneH,
                               float barW,
                               float barStep,
                               float startX,
                               float maxBH,
                               float minBH,
                               int barCount) {
    Graphics& g = ctx.g;
    float idleH = g_US.idleBarSize / 100.f;

    Color baseCol;
    if (g_US.vizMode == VizMode::DynamicAlbum)
        baseCol = Color(255, ctx.bgPrimary.GetR(), ctx.bgPrimary.GetG(),
                        ctx.bgPrimary.GetB());
    else
        baseCol = Color(255, (BYTE)((g_US.vizColor >> 16) & 0xFF),
                        (BYTE)((g_US.vizColor >> 8) & 0xFF),
                        (BYTE)(g_US.vizColor & 0xFF));

    SolidBrush br(baseCol);
    GraphicsPath bp;

    for (int i = 0; i < barCount; i++) {
        float fac = max(idleH, g_VizPeak[i]);
        float bh = minBH + fac * (maxBH - minBH);
        float bx = startX + i * barStep;
        float by;
        switch (g_US.vizAnchor) {
            case VizAnchor::Top:
                by = zoneY;
                break;
            case VizAnchor::Bottom:
                by = zoneY + zoneH - bh;
                break;
            default:
                by = zoneY + (zoneH - bh) / 2.f;
                break;
        }
        int cornerR = max(1, (int)(barW * 0.5f));
        bp.Reset();
        RoundRectPath(bp, (int)bx, (int)by, (int)barW, (int)bh, cornerR);

        switch (g_US.vizMode) {
            case VizMode::DynamicGradient: {
                float t = (float)i / max(1, barCount - 1);
                float freqT = t * 0.6f + fac * 0.4f;
                Color gc(
                    255,
                    (BYTE)(ctx.bgPrimary.GetR() + (int)(ctx.bgSecondary.GetR() -
                                                        ctx.bgPrimary.GetR()) *
                                                      freqT),
                    (BYTE)(ctx.bgPrimary.GetG() + (int)(ctx.bgSecondary.GetG() -
                                                        ctx.bgPrimary.GetG()) *
                                                      freqT),
                    (BYTE)(ctx.bgPrimary.GetB() + (int)(ctx.bgSecondary.GetB() -
                                                        ctx.bgPrimary.GetB()) *
                                                      freqT));
                br.SetColor(gc);
                g.FillPath(&br, &bp);
                break;
            }
            case VizMode::CustomGradient: {
                float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                Color gc = LerpDWORD(g_US.vizColor1, g_US.vizColor2, t);
                br.SetColor(Color(255, gc.GetR(), gc.GetG(), gc.GetB()));
                g.FillPath(&br, &bp);
                break;
            }
            case VizMode::Acrylic: {
                BYTE aa = (BYTE)(max(30, min(180, (int)(0.55f * fac + 30))));
                Color ac(aa, ctx.bgPrimary.GetR(), ctx.bgPrimary.GetG(),
                         ctx.bgPrimary.GetB());
                br.SetColor(ac);
                g.FillPath(&br, &bp);
                Color rim(min(255, aa + 60), min(255, (int)ac.GetR() + 80),
                          min(255, (int)ac.GetG() + 80),
                          min(255, (int)ac.GetB() + 80));
                Pen rimPen(rim, 0.8f);
                g.DrawPath(&rimPen, &bp);
                break;
            }
            default:
                br.SetColor(baseCol);
                g.FillPath(&br, &bp);
                break;
        }
    }
}

static void DrawVisualizerBackground(DrawCtx& ctx) {
    if (!g_US.vizEnabled || !g_US.wrap.vizAsBackground || g_US.miniMode)
        return;
    const VizLayout& v = g_Layout.viz;
    int barCount = max(1, g_US.vizBars);
    int W = ctx.W;
    float bgZoneW = (float)W;
    float bgGap = max(0.f, (float)g_US.vizBarGap);
    float bgBarW = max(1.5f, (bgZoneW - bgGap * (barCount - 1)) / barCount);
    float bgStep = bgBarW + bgGap;
    float bgStartX = (bgZoneW - (bgStep * barCount - bgGap)) * 0.5f;
    DrawVisualizerBars(ctx, v.zoneY, v.zoneH, bgBarW, bgStep, bgStartX,
                       v.maxBarH, v.minBarH, barCount);
}

static void DrawVisualizerContainer(DrawCtx& ctx) {
    if (!g_US.vizEnabled)
        return;
    if (!g_US.miniMode && g_US.wrap.vizAsBackground)
        return;  // drawn as background instead
    const VizLayout& v = g_Layout.viz;
    DrawVisualizerBars(ctx, v.zoneY, v.zoneH, v.barW, v.barStep, v.startX,
                       v.maxBarH, v.minBarH, max(1, g_US.vizBars));
}

// ── Controls
// ──────────────────────────────────────────────────────────────────

// Returns the animated color for a given button index.
static Color ButtonColor(const DrawCtx& ctx, int btn) {
    float t = g_Anim.hoverProgress[btn];
    if (t <= 0.f)
        return ctx.ctrlBase;
    if (t >= 1.f)
        return ctx.hoverTarget;
    return LerpColor(ctx.ctrlBase, ctx.hoverTarget, t);
}

static void DrawCustomIcon(Graphics& g,
                           int type,
                           float cx,
                           float cy,
                           float s,
                           Color col) {
    SolidBrush b(col);
    auto X = [&](float v) { return cx + v * s; };
    auto Y = [&](float v) { return cy + v * s; };
    auto W = [&](float v) { return v * s; };

    switch (type) {
        case 1: {  // Prev
            PointF pts[] = {
                {X(4.5f), Y(-5.f)}, {X(-2.5f), Y(0)}, {X(4.5f), Y(5.f)}};
            g.FillPolygon(&b, pts, 3);
            g.FillRectangle(&b, X(-5.5f), Y(-5.f), W(3.f), W(10.f));
            break;
        }
        case 2: {  // Play
            PointF pts[] = {
                {X(-2.5f), Y(-6.f)}, {X(6.5f), Y(0)}, {X(-2.5f), Y(6.f)}};
            g.FillPolygon(&b, pts, 3);
            break;
        }
        case 3: {  // Pause
            g.FillRectangle(&b, X(-4.f), Y(-6.f), W(3.f), W(12.f));
            g.FillRectangle(&b, X(1.f), Y(-6.f), W(3.f), W(12.f));
            break;
        }
        case 4: {  // Next
            PointF pts[] = {
                {X(-4.5f), Y(-5.f)}, {X(2.5f), Y(0)}, {X(-4.5f), Y(5.f)}};
            g.FillPolygon(&b, pts, 3);
            g.FillRectangle(&b, X(2.5f), Y(-5.f), W(3.f), W(10.f));
            break;
        }
        case 5: {  // Speaker
            PointF cone[] = {{X(-7.f), Y(-3.f)}, {X(-4.f), Y(-3.f)},
                             {X(0.f), Y(-7.f)},  {X(0.f), Y(7.f)},
                             {X(-4.f), Y(3.f)},  {X(-7.f), Y(3.f)}};
            g.FillPolygon(&b, cone, 6);
            Pen p2(col, W(1.5f));
            p2.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
            g.DrawArc(&p2, X(-5.5f), Y(-4.f), W(8.f), W(8.f), -50, 100);
            g.DrawArc(&p2, X(-8.5f), Y(-7.f), W(14.f), W(14.f), -50, 100);
            break;
        }
    }
}

static void DrawGlyphButton(Graphics& g,
                            const Font& fnt,
                            const wchar_t* glyph,
                            float cx,
                            float cy,
                            Color col) {
    SolidBrush br(col);
    RectF meas;
    g.MeasureString(glyph, -1, &fnt, RectF(0, 0, 4000, 200), &meas);
    g.DrawString(glyph, -1, &fnt,
                 PointF(cx - meas.Width / 2.f, cy - meas.Height / 2.f), &br);
}

static void DrawVolumeSlider(DrawCtx& ctx) {
    if (!g_US.showSpeakerIcon || !g_Vol.hovered)
        return;
    Graphics& g = ctx.g;
    float vX = g_Layout.ctrl.vX;
    float cY = g_Layout.ctrl.cY;

    float bgX = vX + 18.f, bgW = 74.f, bgH = 12.f, bgY = cY - bgH / 2.f;
    GraphicsPath vp;
    RoundRectPath(vp, bgX, bgY, bgW, bgH, bgH / 2.f);
    Color popupBg =
        IsLightCached() ? Color(230, 245, 245, 245) : Color(200, 30, 30, 30);
    SolidBrush pBr(popupBg);
    g.FillPath(&pBr, &vp);

    float slW = 60.f, slH = 4.f, slX = bgX + 7.f, slTop = cY - slH / 2.f;
    SolidBrush track(Color(60, ctx.textColor.GetR(), ctx.textColor.GetG(),
                           ctx.textColor.GetB()));
    GraphicsPath tp;
    RoundRectPath(tp, slX, slTop, slW, slH, slH / 2.f);
    g.FillPath(&track, &tp);

    float filled = g_Vol.level * slW;
    if (filled > 0) {
        SolidBrush fill(ctx.textColor);
        GraphicsPath fp;
        RoundRectPath(fp, slX, slTop, filled, slH, slH / 2.f);
        g.FillPath(&fill, &fp);
    }
    float thumbR = 4.f;
    SolidBrush fill2(ctx.textColor);
    g.FillEllipse(&fill2, slX + filled - thumbR, cY - thumbR, thumbR * 2.f,
                  thumbR * 2.f);
}

static void DrawControlsContainer(DrawCtx& ctx) {
    if (!IsContainerEnabled(3) || g_US.miniMode)
        return;

    Graphics& g = ctx.g;
    const ControlLayout& cl = g_Layout.ctrl;
    float cY = cl.cY;
    float nX = cl.nX, ppX = cl.ppX, pX = cl.pX, vX = cl.vX;
    float shuffleX = cl.shuffleX, repeatX = cl.repeatX;
    float iconScale = ctx.iconScale;
    float ppBigR = ctx.ppBigR, pnSmallR = ctx.pnSmallR;

    bool shOn = false;
    int repMode = 0;
    {
        scoped_lock lk(g_M.mtx);
        shOn = g_M.shuffleOn;
        repMode = g_M.repeatMode;
    }

    // Hover circles
    for (int i = 1; i <= 6; i++) {
        float t = g_Anim.hoverProgress[i];
        if (t <= 0.f)
            continue;
        float hX = 0, hR = (i == 2) ? ppBigR : (pnSmallR + 2.f);
        if (i == 1)
            hX = pX;
        else if (i == 2)
            hX = ppX;
        else if (i == 3)
            hX = nX;
        else if (i == 5 && shuffleX > 0.f)
            hX = shuffleX;
        else if (i == 6 && repeatX > 0.f)
            hX = repeatX;
        else
            continue;
        int a = (int)(28.f * t);
        SolidBrush hb(Color(a, ctx.ctrlBase.GetR(), ctx.ctrlBase.GetG(),
                            ctx.ctrlBase.GetB()));
        float aR = hR * (0.8f + 0.2f * t);
        g.FillEllipse(&hb, hX - aR, cY - aR, aR * 2.f, aR * 2.f);
    }

    if (g_US.iconTheme != IconTheme::Default) {
        // ── Glyph-based icon themes
        // ───────────────────────────────────────────
        const WCHAR* fontName = (g_US.iconTheme >= IconTheme::FluentOutlined)
                                    ? L"Segoe Fluent Icons"
                                    : L"Segoe MDL2 Assets";
        FontFamily iconFF(fontName, nullptr);
        Font iconFont(&iconFF, 14.f * iconScale, FontStyleRegular, UnitPixel);

        // Per-theme glyph codepoints
        const wchar_t *gPrev, *gPlay, *gPause, *gNext;
        const wchar_t *gShuffle, *gRepeatNone, *gRepeatTrack, *gRepeatList;
        const wchar_t *gVol0, *gVol1, *gVol2, *gVol3;

        switch (g_US.iconTheme) {
            case IconTheme::FluentFilled:
                gPrev = L"\uF8AC";
                gPlay = L"\uF5B0";
                gPause = L"\uF8AE";
                gNext = L"\uF8AD";
                gShuffle = L"\xF5E7";
                gRepeatNone = L"\xF8A8";
                gRepeatTrack = L"\xF8A7";
                gRepeatList = L"\xF8A8";
                break;
            case IconTheme::FluentOutlined2:
                gPrev = L"\uEB9E";
                gPlay = L"\uEE4A";
                gPause = L"\uEDB4";
                gNext = L"\uEB9D";
                gShuffle = L"\xE8B1";
                gRepeatNone = L"\xE8EE";
                gRepeatTrack = L"\xE8ED";
                gRepeatList = L"\xE8EE";
                break;
            default:  // MDL2 / FluentOutlined
                gPrev = L"\uE892";
                gPlay = L"\uE768";
                gPause = L"\uE769";
                gNext = L"\uE893";
                gShuffle = L"\xE8B1";
                gRepeatNone = L"\xE8EE";
                gRepeatTrack = L"\xE8ED";
                gRepeatList = L"\xE8EE";
                break;
        }

        // Volume glyph
        if (g_Vol.level <= 0.f)
            gVol0 = L"\uE74F", gVol1 = gVol2 = gVol3 = gVol0;
        else if (g_Vol.level < 0.34f)
            gVol1 = L"\uE993", gVol0 = gVol2 = gVol3 = gVol1;
        else if (g_Vol.level < 0.67f)
            gVol2 = L"\uE994", gVol0 = gVol1 = gVol3 = gVol2;
        else
            gVol3 = L"\uE995", gVol0 = gVol1 = gVol2 = gVol3;
        const wchar_t* gVol = (g_Vol.level <= 0.f)    ? gVol0
                              : (g_Vol.level < 0.34f) ? gVol1
                              : (g_Vol.level < 0.67f) ? gVol2
                                                      : gVol3;

        if (g_US.showPlaybackControls) {
            DrawGlyphButton(g, iconFont, gPrev, pX, cY, ButtonColor(ctx, 1));
            DrawGlyphButton(g, iconFont, ctx.playing ? gPause : gPlay, ppX, cY,
                            ButtonColor(ctx, 2));
            DrawGlyphButton(g, iconFont, gNext, nX, cY, ButtonColor(ctx, 3));
        }
        if (g_US.showSpeakerIcon)
            DrawGlyphButton(g, iconFont, gVol, vX, cY, ButtonColor(ctx, 4));

        if (g_US.showShuffleButton && shuffleX > 0.f) {
            Color shCol = shOn ? ctx.hoverTarget : ButtonColor(ctx, 5);
            DrawGlyphButton(g, iconFont, gShuffle, shuffleX, cY, shCol);
            if (shOn) {
                float dR = 1.8f * iconScale, dY = cY + 10.f * iconScale;
                SolidBrush db(ctx.hoverTarget);
                g.FillEllipse(&db, shuffleX - dR, dY - dR, dR * 2.f, dR * 2.f);
            }
        }
        if (g_US.showRepeatButton && repeatX > 0.f) {
            const wchar_t* rg = (repMode == 1)   ? gRepeatTrack
                                : (repMode == 2) ? gRepeatList
                                                 : gRepeatNone;
            Color repCol =
                (repMode > 0) ? ctx.hoverTarget : ButtonColor(ctx, 6);
            DrawGlyphButton(g, iconFont, rg, repeatX, cY, repCol);
            if (repMode > 0) {
                float dR = 1.8f * iconScale, dY = cY + 10.f * iconScale;
                SolidBrush db(ctx.hoverTarget);
                g.FillEllipse(&db, repeatX - dR, dY - dR, dR * 2.f, dR * 2.f);
            }
        }
    } else {
        // ── Custom-drawn icons
        // ────────────────────────────────────────────────
        float s = 1.15f * iconScale;
        if (g_US.showPlaybackControls) {
            DrawCustomIcon(g, ctx.playing ? 3 : 2, ppX, cY, s,
                           ButtonColor(ctx, 2));
            DrawCustomIcon(g, 1, pX, cY, 0.9f * iconScale, ButtonColor(ctx, 1));
            DrawCustomIcon(g, 4, nX, cY, 0.9f * iconScale, ButtonColor(ctx, 3));
        }
        if (g_US.showSpeakerIcon)
            DrawCustomIcon(g, 5, vX, cY, 0.9f * iconScale, ButtonColor(ctx, 4));

        // Shuffle (default drawn)
        if (g_US.showShuffleButton && shuffleX > 0.f) {
            Color shCol = shOn ? ctx.hoverTarget : ButtonColor(ctx, 5);
            float ss = 0.75f * iconScale;
            Pen shPen(shCol, 1.5f * ss);
            shPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
            g.DrawLine(&shPen, shuffleX - 5.f * ss, cY - 4.f * ss,
                       shuffleX + 5.f * ss, cY + 4.f * ss);
            g.DrawLine(&shPen, shuffleX - 5.f * ss, cY + 4.f * ss,
                       shuffleX + 5.f * ss, cY - 4.f * ss);
            PointF at[] = {{shuffleX + 3.f * ss, cY - 6.f * ss},
                           {shuffleX + 5.f * ss, cY - 4.f * ss},
                           {shuffleX + 5.f * ss, cY - 2.f * ss}};
            g.DrawLines(&shPen, at, 3);
            PointF ab[] = {{shuffleX + 3.f * ss, cY + 6.f * ss},
                           {shuffleX + 5.f * ss, cY + 4.f * ss},
                           {shuffleX + 5.f * ss, cY + 2.f * ss}};
            g.DrawLines(&shPen, ab, 3);
            if (shOn) {
                float dR = 2.5f * iconScale, dY = (float)ctx.H - dR * 2.f - 2.f;
                SolidBrush db(ctx.hoverTarget);
                g.FillEllipse(&db, shuffleX - dR, dY, dR * 2.f, dR * 2.f);
            }
        }

        // Repeat (default drawn)
        if (g_US.showRepeatButton && repeatX > 0.f) {
            Color repCol =
                (repMode > 0) ? ctx.hoverTarget : ButtonColor(ctx, 6);
            Pen repPen(repCol, 1.5f * iconScale);
            repPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
            float rr = 5.5f * iconScale;
            g.DrawArc(&repPen, repeatX - rr, cY - rr, rr * 2.f, rr * 2.f, 30,
                      300);
            PointF ra[] = {{repeatX + rr * 0.5f, cY - rr},
                           {repeatX + rr, cY - rr * 0.5f},
                           {repeatX + rr * 0.2f, cY - rr * 0.2f}};
            g.DrawLines(&repPen, ra, 3);
            if (repMode == 1) {
                FontFamily ff2(FONT_NAME, nullptr);
                Font f1(&ff2, 5.5f * iconScale, FontStyleBold, UnitPixel);
                SolidBrush rb(repCol);
                g.DrawString(
                    L"1", -1, &f1,
                    PointF(repeatX - 2.5f * iconScale, cY - 3.5f * iconScale),
                    &rb);
            }
            if (repMode > 0) {
                float dR = 2.5f * iconScale, dY = (float)ctx.H - dR * 2.f - 2.f;
                SolidBrush db(repCol);
                g.FillEllipse(&db, repeatX - dR, dY, dR * 2.f, dR * 2.f);
            }
        }
    }

    // Volume slider (shown above all icons when hovered)
    DrawVolumeSlider(ctx);

    // Progress bar under controls
    if (g_US.showProgressBar &&
        (g_US.progressBarLayer == PBLayer::UnderControlsOnly)) {
        DrawProgressBar(ctx, nullptr, g_Layout.FindOrigin(3));
    }
}

// ── Container debug borders
// ───────────────────────────────────────────────────

static void DrawContainerBorders(DrawCtx& ctx) {
    if (!g_US.showContainerBorders)
        return;
    static const Color kColors[5] = {
        Color(0, 0, 0, 0),        Color(255, 255, 80, 80),  // Media     – red
        Color(255, 80, 200, 80),                            // Info      – green
        Color(255, 80, 140, 255),                           // Controls  – blue
        Color(255, 255, 180, 40),  // Visualizer– orange
    };
    for (int i = 0; i < g_Layout.originCount; i++) {
        const ContainerOrigin& co = g_Layout.origins[i];
        if (co.id < 1 || co.id > 4)
            continue;
        Pen pen(kColors[co.id], 1.f);
        ctx.g.DrawRectangle(&pen, co.cellX, 0, co.cellWidth - 1, ctx.H - 1);
        pen.SetDashStyle(DashStyleDash);
        if (co.contentWidth > 0)
            ctx.g.DrawRectangle(&pen, co.contentX, 0, co.contentWidth - 1,
                                ctx.H - 1);
    }
}

// ── Main draw entry point
// ─────────────────────────────────────────────────────

static void DrawPanel(HDC hdc, int W, int H) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    g.Clear(Color(0, 0, 0, 0));

    EnsureLayout(W, H);

    // Palette from album art
    Color bgPrimary, bgSecondary;
    {
        scoped_lock lk(g_M.mtx);
        bgPrimary = g_M.primaryColor;
        bgSecondary = g_M.secondaryColor;
    }

    // Idle screen substitution
    bool idleState = g_IdleState;

    // Build shared context
    wstring title, artist;
    Bitmap* art = nullptr;
    bool playing = false;
    DrawCtx ctx = MakeDrawCtx(g, W, H, bgPrimary, bgSecondary, idleState, title,
                              artist, art, playing);

    // 1. Background
    DrawBackground(ctx);

    // 2. Visualizer as background layer (behind everything)
    DrawVisualizerBackground(ctx);

    // 3. Sequential containers
    for (int i = 0; i < g_Layout.originCount; i++) {
        int id = g_Layout.origins[i].id;
        switch (id) {
            case 1:
                DrawMediaContainer(ctx);
                break;
            case 2:
                DrawInfoContainer(ctx);
                break;
            case 3:
                DrawControlsContainer(ctx);
                break;
            case 4:
                DrawVisualizerContainer(ctx);
                break;
        }
    }

    // 4. Whole-panel progress bar positions
    if (g_US.showProgressBar && (g_US.progressBarLayer == PBLayer::UnderBoth ||
                                 g_US.progressBarLayer == PBLayer::AboveBoth)) {
        DrawProgressBar(ctx, nullptr, nullptr);
    }

    // 5. Container debug borders (on top of everything)
    DrawContainerBorders(ctx);
}

// ============================================================================
//  Back-buffer
// ============================================================================

static HBITMAP g_hBackBmp = nullptr;
static HDC g_hBackDC = nullptr;
static int g_BackW = 0, g_BackH = 0;

static void EnsureBackBuffer(HDC hdc, int w, int h) {
    if (g_hBackDC && g_BackW == w && g_BackH == h)
        return;
    if (g_hBackBmp) {
        SelectObject(g_hBackDC, (HBITMAP) nullptr);
        DeleteObject(g_hBackBmp);
        g_hBackBmp = nullptr;
    }
    if (g_hBackDC) {
        DeleteDC(g_hBackDC);
        g_hBackDC = nullptr;
    }
    g_hBackDC = CreateCompatibleDC(hdc);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits;
    g_hBackBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    SelectObject(g_hBackDC, g_hBackBmp);
    g_BackW = w;
    g_BackH = h;
}

static void DestroyBackBuffer() {
    if (g_hBackBmp) {
        DeleteObject(g_hBackBmp);
        g_hBackBmp = nullptr;
    }
    if (g_hBackDC) {
        DeleteDC(g_hBackDC);
        g_hBackDC = nullptr;
    }
    g_BackW = g_BackH = 0;
}

// ============================================================================
//  Hit testing helpers
// ============================================================================

// Returns button index (1-6) if (mx,my) is over a control button, else 0.
static MediaCmd HitTestControls(int mx, int my, int W, int H) {
    if (g_US.miniMode)
        return MediaCmd::None;

    EnsureLayout(W, H);
    const ControlLayout& cl = g_Layout.ctrl;
    float cY = cl.cY, nX = cl.nX, ppX = cl.ppX, pX = cl.pX, vX = cl.vX;
    float shuffleX = cl.shuffleX, repeatX = cl.repeatX;
    float ppBigR = ICON_SIZE_REF * (g_US.iconSize / ICON_SIZE_REF) *
                   (H / PANEL_HEIGHT_REF);
    float pnSmR =
        9.f * (g_US.iconSize / ICON_SIZE_REF) * (H / PANEL_HEIGHT_REF);

    if (g_US.showSpeakerIcon) {
        float volR = g_Vol.hovered ? (vX + 95.f) : (vX + 14.f);
        if (mx >= vX - 12.f && mx <= volR && my >= cY - 14.f && my <= cY + 14.f)
            return MediaCmd::Volume;
    }
    if (g_US.showPlaybackControls) {
        if (mx >= pX - (pnSmR + 4) && mx <= pX + (pnSmR + 4) &&
            my >= cY - (pnSmR + 4) && my <= cY + (pnSmR + 4))
            return MediaCmd::Prev;
        if (mx >= ppX - ppBigR && mx <= ppX + ppBigR && my >= cY - ppBigR &&
            my <= cY + ppBigR)
            return MediaCmd::PlayPause;
        if (mx >= nX - (pnSmR + 4) && mx <= nX + (pnSmR + 4) &&
            my >= cY - (pnSmR + 4) && my <= cY + (pnSmR + 4))
            return MediaCmd::Next;
    }
    if (g_US.showShuffleButton && shuffleX > 0.f &&
        mx >= shuffleX - (pnSmR + 4) && mx <= shuffleX + (pnSmR + 4) &&
        my >= cY - (pnSmR + 4) && my <= cY + (pnSmR + 4))
        return MediaCmd::Shuffle;
    if (g_US.showRepeatButton && repeatX > 0.f && mx >= repeatX - (pnSmR + 4) &&
        mx <= repeatX + (pnSmR + 4) && my >= cY - (pnSmR + 4) &&
        my <= cY + (pnSmR + 4))
        return MediaCmd::Repeat;
    return MediaCmd::None;
}

// ============================================================================
//  Taskbar hook
// ============================================================================

static bool IsTaskbarWindow(HWND h) {
    WCHAR c[64]{};
    GetClassNameW(h, c, 64);
    return !wcscmp(c, L"Shell_TrayWnd");
}

static void CALLBACK
TbEvt(HWINEVENTHOOK, DWORD, HWND h, LONG, LONG, DWORD, DWORD) {
    if (IsTaskbarWindow(h) && g_hWnd)
        PostMessage(g_hWnd, WM_TASKBAR_MOVED, 0, 0);
}

static void HookTaskbar(HWND hwnd) {
    HWND tb = FindWindow(L"Shell_TrayWnd", nullptr);
    if (tb) {
        DWORD pid = 0;
        DWORD tid = GetWindowThreadProcessId(tb, &pid);
        if (tid) {
            g_TbHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, TbEvt, pid, tid,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        }
    }
    UpdateBoundsAndRegion(hwnd);
}

// ============================================================================
//  Window focus helper
// ============================================================================

struct FocusData {
    const wstring* aumid;
    HWND found;
};

static BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM lp) {
    auto* fd = reinterpret_cast<FocusData*>(lp);
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid || !IsWindowVisible(h) || GetWindow(h, GW_OWNER))
        return TRUE;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc)
        return TRUE;

    WCHAR path[MAX_PATH] = {};
    DWORD sz = MAX_PATH;
    bool matched = false;
    if (QueryFullProcessImageNameW(hProc, 0, path, &sz)) {
        wstring exe = path;
        size_t sl = exe.find_last_of(L"\\");
        if (sl != wstring::npos)
            exe = exe.substr(sl + 1);
        size_t dot = exe.find_last_of(L".");
        if (dot != wstring::npos)
            exe = exe.substr(0, dot);
        transform(exe.begin(), exe.end(), exe.begin(), ::towlower);
        wstring la = *fd->aumid;
        transform(la.begin(), la.end(), la.begin(), ::towlower);
        matched =
            (la.find(exe) != wstring::npos || exe.find(la) != wstring::npos);
    }
    CloseHandle(hProc);
    if (matched) {
        fd->found = h;
        return FALSE;
    }
    return TRUE;
}

// ============================================================================
//  Window procedure
// ============================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        // ── Lifecycle
        // ─────────────────────────────────────────────────────────────
        case WM_CREATE:
            ApplyAppearance(hwnd);
            HookTaskbar(hwnd);
            FetchMedia();
            ApplyTimerRates(hwnd);
            if (g_US.hideFullscreen)
                g_Timers.Set(hwnd, IDT_FULLSCREEN, 100);
            if (g_US.vizEnabled) {
                StartCaptureThread();
                g_Timers.Set(hwnd, IDT_VIZ, 16);
                g_VizTimerRunning = true;
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            return 0;
        case WM_APPCLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_Timers.KillAll();
            g_AnimTimerRunning = false;
            g_VizTimerRunning = false;
            g_Anim.hoverAnimRunning = false;
            g_Anim.fadeActive = false;
            g_FullscreenHidden = false;

            if (g_TbHook) {
                UnhookWinEvent(g_TbHook);
                g_TbHook = nullptr;
            }

            if (g_CaptureRunning.load()) {
                g_CaptureRunning.store(false);
                if (g_hCaptureEvent)
                    SetEvent(g_hCaptureEvent);
                if (g_CaptureThread.joinable()) {
                    HANDLE hRaw = (HANDLE)g_CaptureThread.native_handle();
                    if (WaitForSingleObject(hRaw, 500) == WAIT_OBJECT_0)
                        g_CaptureThread.join();
                    else
                        g_CaptureThread.detach();
                }
                if (g_hCaptureEvent) {
                    CloseHandle(g_hCaptureEvent);
                    g_hCaptureEvent = nullptr;
                }
            }

            g_Vol.cachedIface = nullptr;
            g_Vol.cachedAumid.clear();
            g_BlurCache.Invalidate();

            {
                scoped_lock lk(g_M.mtx);
                g_M.art.reset();
            }
            DestroyBackBuffer();
            g_Mgr = nullptr;
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            ApplyAppearance(hwnd);
            UpdateBoundsAndRegion(hwnd);
            g_LayoutDirty = true;
            InvalidateLightCache();
            DestroyBackBuffer();
            InvalidateRect(hwnd, NULL, TRUE);
            if (g_US.hideFullscreen)
                g_Timers.Set(hwnd, IDT_FULLSCREEN, 100);
            else {
                g_Timers.Kill(IDT_FULLSCREEN);
                g_FullscreenHidden = false;
            }
            if (g_US.vizEnabled && !g_VizTimerRunning) {
                StartCaptureThread();
                g_Timers.Set(hwnd, IDT_VIZ, 16);
                g_VizTimerRunning = true;
            } else if (!g_US.vizEnabled && g_VizTimerRunning) {
                g_Timers.Kill(IDT_VIZ);
                g_VizTimerRunning = false;
                StopCaptureThread();
                memset(g_VizPeak, 0, sizeof(g_VizPeak));
                memset(g_VizTarget, 0, sizeof(g_VizTarget));
            }
            return 0;

        // ── Timers
        // ────────────────────────────────────────────────────────────────
        case WM_TIMER:
            switch (wp) {
                case IDT_POLL: {
                    float prevRatio = g_ArtAspectRatio;
                    FetchMedia();
                    if (g_US.mediaAutoSize && g_US.mediaEnabled &&
                        g_ArtAspectRatio != prevRatio) {
                        RecomputeLayout();
                        UpdateBoundsAndRegion(hwnd);
                    }
                    // Idle screen
                    {
                        scoped_lock lk(g_M.mtx);
                        if (g_US.idleScreenEnabled && !g_M.hasMedia) {
                            if (++g_NoMediaSecs >= g_US.idleScreenDelay)
                                g_IdleState = true;
                        } else {
                            g_NoMediaSecs = 0;
                            g_IdleState = false;
                        }
                    }
                    // Idle timeout (hide)
                    if (g_US.idleTimeout > 0) {
                        bool isPlaying;
                        {
                            scoped_lock lk(g_M.mtx);
                            isPlaying = g_M.playing;
                        }
                        if (!isPlaying) {
                            if (++g_IdleSecs >= g_US.idleTimeout)
                                g_IdleHide = true;
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
                    bool tbVis = IsTaskbarEffectivelyVisible(tb);

                    if (g_US.vizEnabled) {
                        bool panelVis = shouldShow && tbVis;
                        if (!panelVis && g_CaptureRunning.load())
                            StopCaptureThread();
                        if (panelVis && !g_CaptureRunning.load())
                            StartCaptureThread();
                    }
                    if (!g_FullscreenHidden) {
                        if (shouldShow && tbVis) {
                            if (!g_Anim.fadeActive) {
                                g_Anim.fadeAlpha = 1.f;
                                SetLayeredWindowAttributes(hwnd, 0, 255,
                                                           LWA_ALPHA);
                                ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                            }
                        } else if (!shouldShow) {
                            g_Timers.Kill(IDT_FADE);
                            g_Anim.fadeActive = false;
                            g_Anim.fadeAlpha = 0.f;
                            SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                            ShowWindow(hwnd, SW_HIDE);
                        }
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                    ApplyTimerRates(hwnd);
                    break;
                }

                case IDT_FULLSCREEN: {
                    bool isFS = false;
                    QUERY_USER_NOTIFICATION_STATE qs;
                    if (SUCCEEDED(SHQueryUserNotificationState(&qs)))
                        isFS = (qs == QUNS_BUSY ||
                                qs == QUNS_RUNNING_D3D_FULL_SCREEN);

                    if (isFS && !g_FullscreenHidden) {
                        g_FullscreenHidden = true;
                        if (g_US.vizEnabled && g_CaptureRunning.load())
                            StopCaptureThread();
                        if (g_US.fadeFullscreen) {
                            g_Timers.Kill(IDT_FADE);
                            g_Anim.fadeActive = true;
                            g_Anim.fadeIn = false;
                            g_Timers.Set(hwnd, IDT_FADE, 16);
                            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                        } else {
                            g_Timers.Kill(IDT_FADE);
                            g_Anim.fadeActive = false;
                            g_Anim.fadeAlpha = 0.f;
                            SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                            ShowWindow(hwnd, SW_HIDE);
                        }
                    } else if (!isFS && g_FullscreenHidden) {
                        g_FullscreenHidden = false;
                        if (g_US.vizEnabled && !g_CaptureRunning.load())
                            StartCaptureThread();
                        if (g_US.fadeFullscreen) {
                            g_Timers.Kill(IDT_FADE);
                            g_Anim.fadeAlpha = 0.f;
                            SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                            g_Anim.fadeActive = true;
                            g_Anim.fadeIn = true;
                            g_Timers.Set(hwnd, IDT_FADE, 16);
                        } else {
                            g_Timers.Kill(IDT_FADE);
                            g_Anim.fadeActive = false;
                            g_Anim.fadeAlpha = 1.f;
                            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                        }
                    }
                    break;
                }

                case IDT_POS: {
                    // Poll timeline position for smooth progress bar
                    if (g_US.showProgressBar && g_Mgr) {
                        try {
                            GlobalSystemMediaTransportControlsSession ses =
                                nullptr;
                            auto sessions = g_Mgr.GetSessions();
                            if (!g_currentMediaAppAumid.empty()) {
                                for (auto const& s : sessions)
                                    if (wstring(
                                            s.SourceAppUserModelId().c_str()) ==
                                        g_currentMediaAppAumid) {
                                        ses = s;
                                        break;
                                    }
                            }
                            if (!ses)
                                try {
                                    ses = g_Mgr.GetCurrentSession();
                                } catch (...) {
                                }
                            if (ses) {
                                auto tl = ses.GetTimelineProperties();
                                auto pbi = ses.GetPlaybackInfo();
                                using S =
                                    GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                                INT64 np = tl.Position().count(),
                                      ne = tl.EndTime().count();
                                bool nowP =
                                    (pbi.PlaybackStatus() == S::Playing);
                                if (np != g_Progress.lastRawPos ||
                                    ne != g_Progress.endRaw ||
                                    nowP != g_Progress.isPlaying) {
                                    g_Progress.lastRawPos = np;
                                    g_Progress.endRaw = ne;
                                    g_Progress.updateTick = GetTickCount();
                                    g_Progress.isPlaying = nowP;
                                }
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                        } catch (...) {
                        }
                    }
                    // Scroll animation
                    if (g_Scroll.active) {
                        if (g_Scroll.waitTicks > 0) {
                            --g_Scroll.waitTicks;
                        } else {
                            int step = (int)g_US.scrollSpeed == 0   ? 1
                                       : (int)g_US.scrollSpeed == 1 ? 2
                                                                    : 3;
                            int maxOff =
                                max(0, g_Scroll.textW - g_Scroll.visW + 10);
                            if (g_Scroll.forward) {
                                g_Scroll.offset += step;
                                if (g_Scroll.offset >= maxOff) {
                                    g_Scroll.offset = maxOff;
                                    g_Scroll.forward = false;
                                    g_Scroll.waitTicks = 80;
                                }
                            } else {
                                g_Scroll.offset -= step;
                                if (g_Scroll.offset <= 0) {
                                    g_Scroll.offset = 0;
                                    g_Scroll.forward = true;
                                    g_Scroll.waitTicks = 80;
                                }
                            }
                            InvalidateRect(hwnd, NULL, FALSE);
                        }
                    } else if (g_US.showProgressBar && g_Progress.isPlaying) {
                        InvalidateRect(hwnd, NULL, FALSE);
                    } else {
                        g_Timers.Kill(IDT_ANIM);
                        g_AnimTimerRunning = false;
                    }
                    break;
                }

                case IDT_ANIM: {
                    // Driven by IDT_POS for scroll/progress — kept alive via
                    // DrawInfoContainer
                    break;
                }

                case IDT_HOVER_ANIM: {
                    bool needsAnim = false;
                    for (int i = 1; i <= 6; i++) {
                        float tgt = 0.f;
                        if (i == static_cast<int>(MediaCmd::Volume))
                            tgt = (g_HoverBtn == MediaCmd::Volume ||
                                   g_Vol.hovered)
                                      ? 1.f
                                      : 0.f;
                        else
                            tgt = (g_HoverBtn == static_cast<MediaCmd>(i) &&
                                   !g_Vol.hovered)
                                      ? 1.f
                                      : 0.f;
                        float& p = g_Anim.hoverProgress[i];
                        if (p != tgt) {
                            float step = 0.15f;
                            p = (p < tgt) ? min(tgt, p + step)
                                          : max(tgt, p - step);
                            needsAnim = true;
                        }
                    }
                    if (needsAnim)
                        InvalidateRect(hwnd, NULL, FALSE);
                    else {
                        g_Timers.Kill(IDT_HOVER_ANIM);
                        g_Anim.hoverAnimRunning = false;
                    }
                    break;
                }

                case IDT_FADE: {
                    float speed = 0.08f;
                    if (g_Anim.fadeIn) {
                        g_Anim.fadeAlpha = min(1.f, g_Anim.fadeAlpha + speed);
                        SetLayeredWindowAttributes(
                            hwnd, 0, (BYTE)(g_Anim.fadeAlpha * 255), LWA_ALPHA);
                        if (g_Anim.fadeAlpha >= 1.f) {
                            g_Timers.Kill(IDT_FADE);
                            g_Anim.fadeActive = false;
                        }
                    } else {
                        g_Anim.fadeAlpha = max(0.f, g_Anim.fadeAlpha - speed);
                        SetLayeredWindowAttributes(
                            hwnd, 0, (BYTE)(g_Anim.fadeAlpha * 255), LWA_ALPHA);
                        if (g_Anim.fadeAlpha <= 0.f) {
                            g_Timers.Kill(IDT_FADE);
                            g_Anim.fadeActive = false;
                            ShowWindow(hwnd, SW_HIDE);
                        }
                    }
                    break;
                }

                case IDT_TEXT_FADE: {
                    if (g_Anim.textFadeOut) {
                        g_Anim.textFadeAlpha -= 0.12f;
                        if (g_Anim.textFadeAlpha <= 0.f) {
                            g_Anim.textFadeAlpha = 0.f;
                            g_Anim.textFadeOut = false;
                            {
                                scoped_lock lk(g_M.mtx);
                                g_M.title = g_Anim.pendingTitle;
                                g_M.artist = g_Anim.pendingArtist;
                            }
                        }
                    } else {
                        g_Anim.textFadeAlpha += 0.12f;
                        if (g_Anim.textFadeAlpha >= 1.f) {
                            g_Anim.textFadeAlpha = 1.f;
                            g_Timers.Kill(IDT_TEXT_FADE);
                        }
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                }

                case IDT_VIZ: {
                    if (!g_US.vizEnabled) {
                        g_Timers.Kill(IDT_VIZ);
                        g_VizTimerRunning = false;
                        StopCaptureThread();
                    } else {
                        UpdateVisualizerPeaks();
                        bool changed = false;

                        float attack = 0.55f, decay = 0.18f;
                        switch (g_US.vizShape) {
                            case VizShape::Stereo:
                                attack = 0.70f;
                                decay = 0.20f;
                                break;
                            case VizShape::Mirror:
                                attack = 0.50f;
                                decay = 0.18f;
                                break;
                            case VizShape::Wave:
                                attack = 0.32f;
                                decay = 0.16f;
                                break;
                            case VizShape::Breathe:
                                attack = 0.18f;
                                decay = 0.10f;
                                break;
                            default:
                                break;
                        }

                        int barCount = max(1, min(g_US.vizBars, VIZ_BARS_MAX));
                        for (int i = 0; i < barCount; i++) {
                            float tgt = g_VizTarget[i], cur = g_VizPeak[i];
                            float a = attack, d = decay;

                            if (g_US.vizShape == VizShape::Mountain) {
                                float dist =
                                    fabsf((float)i - ((barCount - 1) * 0.5f));
                                if (dist < 0.5f) {
                                    a = 0.85f;
                                    d = 0.24f;
                                } else if (dist < 1.5f) {
                                    a = 0.60f;
                                    d = 0.18f;
                                } else {
                                    a = 0.92f;
                                    d = 0.32f;
                                }
                            }

                            float next =
                                cur + (tgt - cur) * ((tgt > cur) ? a : d);
                            if (fabsf(next - cur) > 0.0005f) {
                                g_VizPeak[i] = next;
                                changed = true;
                            } else
                                g_VizPeak[i] = tgt;
                        }
                        if (changed)
                            InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                }

            }  // end WM_TIMER switch
            return 0;

        // ── Mouse
        // ─────────────────────────────────────────────────────────────────
        case WM_LBUTTONDOWN: {
            if (g_US.showSpeakerIcon && g_HoverBtn == MediaCmd::Volume &&
                g_Vol.hovered) {
                SetCapture(hwnd);
                RECT rc;
                GetClientRect(hwnd, &rc);
                EnsureLayout(rc.right, rc.bottom);
                float slX = g_Layout.ctrl.vX + 18.f + 7.f;
                SetVolume((float)(LOWORD(lp) - slX) / 60.f);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int mx = LOWORD(lp), my = HIWORD(lp);
            RECT rc;
            GetClientRect(hwnd, &rc);
            EnsureLayout(rc.right, rc.bottom);

            MediaCmd ns = HitTestControls(mx, my, rc.right, rc.bottom);
            bool newVolHover = (ns == MediaCmd::Volume);

            // Separate volume hover from button hover
            bool wasVolHover = g_Vol.hovered;
            g_Vol.hovered = newVolHover;

            // Drag volume slider
            if (g_US.showSpeakerIcon && g_Vol.hovered && (wp & MK_LBUTTON) &&
                GetCapture() == hwnd) {
                float slX = g_Layout.ctrl.vX + 18.f + 7.f;
                SetVolume((float)(mx - (int)slX) / 60.f);
            }

            if (ns != g_HoverBtn || newVolHover != wasVolHover) {
                g_HoverBtn = ns;
                if (!g_Anim.hoverAnimRunning) {
                    g_Timers.Set(hwnd, IDT_HOVER_ANIM, 16);
                    g_Anim.hoverAnimRunning = true;
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }

            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd};
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (GetCapture() != hwnd) {
                g_HoverBtn = MediaCmd::None;
                g_Vol.hovered = false;
                if (!g_Anim.hoverAnimRunning) {
                    g_Timers.Set(hwnd, IDT_HOVER_ANIM, 16);
                    g_Anim.hoverAnimRunning = true;
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;

        case WM_LBUTTONUP:
            if (GetCapture() == hwnd)
                ReleaseCapture();
            if (g_HoverBtn != MediaCmd::None &&
                g_HoverBtn != MediaCmd::Volume) {
                Cmd(g_HoverBtn);
            } else if (g_HoverBtn == MediaCmd::None) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                EnsureLayout(rc.right, rc.bottom);
                int mx = LOWORD(lp);
                if (mx < (int)(g_Layout.ctrl.firstControlX - 12) &&
                    !g_currentMediaAppAumid.empty()) {
                    FocusData fd{&g_currentMediaAppAumid, nullptr};
                    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&fd));
                    if (fd.found) {
                        if (IsIconic(fd.found))
                            ShowWindow(fd.found, SW_RESTORE);
                        SetForegroundWindow(fd.found);
                    }
                }
            }
            return 0;

        case WM_MOUSEWHEEL: {
            if (g_US.scrollVolume) {
                short d = GET_WHEEL_DELTA_WPARAM(wp);
                SetVolume(g_Vol.level + (d > 0 ? 0.05f : -0.05f));
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (g_US.showSpeakerIcon) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                EnsureLayout(rc.right, rc.bottom);
                POINT pt = {LOWORD(lp), HIWORD(lp)};
                ScreenToClient(hwnd, &pt);
                float vX2 = g_Layout.ctrl.vX;
                if (pt.x >= vX2 - 15 && pt.x <= vX2 + 95) {
                    short d = GET_WHEEL_DELTA_WPARAM(wp);
                    SetVolume(g_Vol.level + (d > 0 ? 0.05f : -0.05f));
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }

        // ── Paint
        // ─────────────────────────────────────────────────────────────────
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            EnsureBackBuffer(hdc, rc.right, rc.bottom);
            DrawPanel(g_hBackDC, rc.right, rc.bottom);
            if (g_Scroll.active && !g_AnimTimerRunning) {
                g_Timers.Set(hwnd, IDT_ANIM, 16);
                g_AnimTimerRunning = true;
            }
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, g_hBackDC, 0, 0, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;
        }

        // ── Taskbar moved
        // ─────────────────────────────────────────────────────────
        case WM_TASKBAR_MOVED: {
            HWND tb = FindWindow(L"Shell_TrayWnd", nullptr);
            if (IsTaskbarEffectivelyVisible(tb))
                UpdateBoundsAndRegion(hwnd);
            else if (IsWindowVisible(hwnd))
                ShowWindow(hwnd, SW_HIDE);
            return 0;
        }

        default:
            if (msg == g_TbCreatedMsg) {
                if (g_TbHook) {
                    UnhookWinEvent(g_TbHook);
                    g_TbHook = nullptr;
                }
                HookTaskbar(hwnd);
                return 0;
            }
            if (msg == WM_APPCOMMAND) {
                HWND hShell = FindWindow(L"Shell_TrayWnd", nullptr);
                if (hShell)
                    SendMessage(hShell, msg, wp, lp);
                return 0;
            }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// ============================================================================
//  Media thread
// ============================================================================

static void MediaThread() {
    winrt::init_apartment();

    GdiplusStartupInput gsi;
    ULONG_PTR tok;
    GdiplusStartup(&tok, &gsi, NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"TaskbarMediaPlayer_v6";
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);

    HMODULE hu32 = GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CWB =
        hu32 ? (pCreateWindowInBand)GetProcAddress(hu32, "CreateWindowInBand")
             : nullptr;

    if (CWB) {
        g_hWnd =
            CWB(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                wc.lpszClassName, L"Taskbar Media Player",
                WS_POPUP | WS_VISIBLE, 0, 0, g_PanelWidth, g_US.panelHeight,
                NULL, NULL, wc.hInstance, NULL, ZBID_IMMERSIVE_NOTIFICATION);
    }

    if (!g_hWnd) {
        g_hWnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, wc.lpszClassName,
            L"Taskbar Media Player", WS_POPUP | WS_VISIBLE, 0, 0, g_PanelWidth,
            g_US.panelHeight, NULL, NULL, wc.hInstance, NULL);
    }

    SetLayeredWindowAttributes(g_hWnd, 0, 255, LWA_ALPHA);

    MSG m;
    while (GetMessage(&m, NULL, 0, 0)) {
        TranslateMessage(&m);
        DispatchMessage(&m);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(tok);
    winrt::uninit_apartment();
}

static unique_ptr<thread> g_pThread;

// ============================================================================
//  Windhawk entry points
// ============================================================================

BOOL WhTool_ModInit() {
    SetCurrentProcessExplicitAppUserModelID(L"taskbar-media-player");
    LoadSettings();
    g_pThread = make_unique<thread>(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hWnd)
        SendMessage(g_hWnd, WM_APPCLOSE, 0, 0);
    if (g_pThread) {
        if (g_pThread->joinable())
            g_pThread->join();
        g_pThread.reset();
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hWnd) {
        SendMessage(g_hWnd, WM_TIMER, IDT_POLL, 0);
        SendMessage(g_hWnd, WM_SETTINGCHANGE, 0, 0);
    }
}

// ============================================================================
//  Windhawk tool-mod boilerplate (unchanged from original)
// ============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0)
        return FALSE;

    bool isExcluded = false, isToolModProcess = false,
         isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++)
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }

    for (int i = 1; i < argc - 1; i++)
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0)
                isCurrentToolModProcess = true;
            break;
        }
    LocalFree(argv);

    if (isExcluded)
        return FALSE;

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
        if (!WhTool_ModInit())
            ExitProcess(1);
        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((BYTE*)dos + dos->e_lfanew);
        void* ep = (BYTE*)dos + nt->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(ep, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess)
        return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher)
        return;

    WCHAR path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) {
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR cmd[MAX_PATH + 2 +
              (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(cmd, L"\"%s\" -tool-mod \"%s\"", path, WH_MOD_ID);

    HMODULE kmod = GetModuleHandle(L"kernelbase.dll");
    if (!kmod)
        kmod = GetModuleHandle(L"kernel32.dll");
    if (!kmod) {
        Wh_Log(L"No kernelbase/kernel32");
        return;
    }

    using CPI_t =
        BOOL(WINAPI*)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
                      LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCWSTR,
                      LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto CPI = (CPI_t)GetProcAddress(kmod, "CreateProcessInternalW");
    if (!CPI) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{.cb = sizeof(STARTUPINFO),
                   .dwFlags = STARTF_FORCEOFFFEEDBACK};
    PROCESS_INFORMATION pi;
    if (!CPI(nullptr, path, cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
             nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher)
        WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}