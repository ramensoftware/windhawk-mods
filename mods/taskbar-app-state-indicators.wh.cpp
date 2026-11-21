// ==WindhawkMod==
// @id              taskbar-app-state-indicators
// @name            Taskbar App State Indicators
// @description     Marks taskbar buttons by window state on a frame around the icon: corner marks for open and maximized windows, an underline for minimized apps, dots for several windows or a badge, and progress, all animated along the frame.
// @version         1.0.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -ld2d1
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar App State Indicators

See at a glance which taskbar apps are open, maximized, minimized, busy or
waiting for you. Small marks are drawn on an invisible rounded frame around
each icon, and they move along that frame whenever something changes.

![Demo](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/taskbar-app-state-indicators/demo.gif)

## What the marks mean

- **Open window**: the two bottom corners, joined into a cup under the icon.
- **Maximized window**: all four corners, with a cap over the icon too, or
  only the top corners if you prefer. Optionally, the corners an open or
  maximized app doesn't get are drawn dimmed, so the whole frame shows.
- **Minimized app**: an underline. It's dimmed when the app's windows are
  on another virtual desktop, or suspended like some Store apps.
- **Several windows**: a grey dot on the left edge.
- **Badge** (unread messages, notifications, overlay icons): a red dot on
  the right edge, in every window state.
- **Progress** (downloads, copies, installs): the marks fill from left to
  right in green, yellow while paused, red on error. When the end isn't
  known, the marks run around the frame, or the underline sweeps across
  for a minimized app.
- **Attention**: the marks turn orange while the app's button flashes.
- **Colors**: the app in front gets your accent color, every other app
  grey.
- **Pinned apps** (optional, off by default): a dot at the top center,
  whether the app is running, minimized or not running. A pinned app's top
  corners then stay apart instead of forming a cap, leaving the dot its
  place.

The taskbar's own running indicator and progress bar are replaced by these
marks. Its multi-window mark and badges are replaced too while the
matching dot is on. Everything comes back when the mod is disabled.

## Animations

A minimized app's underline splits into the corners when it's restored, a
maximized window's top corners travel up from the top center, and the dots
grow from their centers. Animations follow the Windows animation setting,
except progress, which keeps moving like the native progress bar.

## Settings

- **Frame**: how far the frame sits from the icon, its corner radius, its
  line thickness (which grows inwards), and whether the corners join into a
  cup and a cap.
- **Colors**: the colors and opacity of the corner marks and the underline,
  for the app in front and for every other app. Empty means your accent color; `#Light|#Dark` sets a color per
  theme, and `#FF0000,#00FF00` a gradient.
- **Open and Maximized Windows**: the marks' shape, whether maximized
  windows keep their bottom corners, and the dimmed missing corners.
- **Minimized Apps**: the underline's length and ends, and its dimmed
  variant for apps running off screen.
- **Apps With Several Windows**, **Apps With a Badge**, **Pinned Apps**:
  turn each dot on or off, and set its size and color.
- **Progress**, **Attention**: their colors.
- **Animations**: on or off, and how long a movement takes.
- **Window Rules**: what counts as an open window.

## Notes

- Colors follow the system live: a new accent color, light and dark
  themes, high contrast and right-to-left taskbars.
- Taskbar Open Window Underline can also hide the running indicator. If you
  use both mods, turn that option off there: both use the same switch.
- For bug reports, turn on **Debug logging** under Advanced and include the
  mod's log.

How it works, in short: each button's windows are found through the
taskbar's view models and judged with `IsIconic`, `IsZoomed`, DWM cloaking
and the virtual desktop manager, refreshed on window events. Progress,
attention and badges are read from the button's own XAML. The marks are
composition shapes on one Direct2D path around the icon, moved by animating
their trims along it on the render thread; the taskbar's layout is never
touched. On builds where the needed symbols can't be resolved, the mod
stays inactive and the log says so.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Frame:
  - Offsets: '5, 5, 5, 5'
    $name: Distance from the icon (Left, Top, Right, Bottom)
    $description: How far the frame's outer edge sits from the icon's edges, in pixels. 0 puts it on the icon's edges, positive values move it outwards, negative values over the icon. One value for all sides, two for horizontal and vertical, or four.
  - CornerRadius: '4'
    $name: Corner radius
    $description: One value for all corners, two for the top and bottom corners, or four for top-left, top-right, bottom-right and bottom-left.
  - Thickness: '2'
    $name: Thickness
    $description: Line thickness of the corner marks and the underline, in pixels. It grows inwards from the frame's outer edge.
  - ConnectCorners: true
    $name: Connect the corners
    $description: Joins the bottom corners into a cup under the icon and the top corners into a cap over it. A pinned app showing the pinned-app dot keeps its top corners apart.
  $name: Frame
  $description: The rounded rectangle around each icon that all indicators are drawn on. The frame itself is never drawn.
- Colors:
  - ActiveColor: ''
    $name: Active app color
    $description: Color of the indicators on the button of the app in front. Leave empty for the system accent color. Hex color code, e.g. '#0078D4'; a different color per theme as '#Light|#Dark'; a left-to-right gradient as '#FF0000,#00FF00'.
  - ActiveOpacity: '1.0, 1.0'
    $name: Active app opacity (Light, Dark)
    $description: From 0 to 1, for the light and the dark theme. One value applies to both.
  - InactiveColor: '#878787|#AEAEAE'
    $name: Other apps color
    $description: Color of the indicators on every other button, same format. Leave empty for the system accent color.
  - InactiveOpacity: '1.0, 1.0'
    $name: Other apps opacity (Light, Dark)
    $description: From 0 to 1, for the light and the dark theme. One value applies to both.
  - AccentShading: true
    $name: Theme-shaded accent
    $description: Where the system accent color is used, a darker shade of it in the light theme and a lighter one in the dark theme, instead of the plain accent in both.
  $name: Colors
  $description: For the corner marks and the underline. The dots, progress and attention have colors of their own. In a high contrast theme, the system's colors are used instead.
- Corners:
  - MaximizedBottom: true
    $name: Bottom corners on maximized windows too
    $description: On, maximized windows get all four corners. Off, only the two top corners (or the cap), so maximized and open windows are told apart at a glance.
  - DimEnabled: false
    $name: Dimmed missing corners
    $description: Completes the frame with dimmed corners where an app has none - the top corners of open windows, and the bottom ones of maximized windows when those are off above.
  - DimColor: '#878787|#AEAEAE'
    $name: Dimmed corners color
    $description: Hex color code, with '#Light|#Dark' for a color per theme, or a gradient. Leave empty for the system accent color.
  - DimOpacity: '0.2, 0.2'
    $name: Dimmed corners opacity (Light, Dark)
    $description: From 0 to 1, for the light and the dark theme. One value applies to both.
  - LegLength: '5'
    $name: Leg length
    $description: Length of each mark's straight part beyond the corner, in pixels. 0 draws just the corners.
  - RoundedEnds: true
    $name: Rounded ends
    $description: Rounds the ends of each mark; off, they are square.
  - CornerStyle: rounded
    $name: Corner style
    $options:
    - rounded: Rounded (a quarter circle)
    - diagonal: Diagonal (a straight cut)
  $name: Open and Maximized Windows
  $description: Open windows get the frame's two bottom corners, maximized windows the two top ones as well.
- Minimized:
  - Length: '12'
    $name: Underline length
    $description: In pixels. 0 spans the whole straight part of the bottom edge; longer values are capped to it.
  - Rounded: true
    $name: Rounded ends
    $description: Rounds the underline's ends; off, they are square.
  - HiddenEnabled: true
    $name: Dimmed underline for apps running off screen
    $description: A dimmed underline for apps whose windows are on other virtual desktops, or cloaked like suspended Store apps.
  - HiddenOpacity: '0.4'
    $name: Dimmed underline opacity
    $description: From 0 to 1, applied to the underline's color.
  $name: Minimized Apps
  $description: Minimized apps get a line centered on the frame's bottom edge.
- Multi:
  - Enabled: true
    $name: Dot for apps with several windows
    $description: A dot on the frame's left edge for apps with two or more windows. The taskbar's own multi-window mark is hidden while this is on.
  - Diameter: '5'
    $name: Dot size
    $description: Diameter of the dot, in pixels.
  - Color: '#878787|#AEAEAE'
    $name: Dot color
    $description: Hex color code, with '#Light|#Dark' for a color per theme. Leave empty for the system accent color.
  $name: Apps With Several Windows
- Badge:
  - Enabled: true
    $name: Dot for apps with a badge
    $description: A dot on the frame's right edge for apps with a notification badge or an overlay icon, in every window state. The taskbar's own badges and overlay icons are hidden while this is on.
  - Diameter: '5'
    $name: Dot size
    $description: Diameter of the dot, in pixels.
  - Color: '#EB5757'
    $name: Dot color
    $description: Hex color code, with '#Light|#Dark' for a color per theme. Leave empty for the system accent color.
  $name: Apps With a Badge
- Pinned:
  - Enabled: false
    $name: Dot for pinned apps
    $description: A dot at the top center of the frame for pinned apps, running or not. The top corners of a pinned app aren't joined into a cap then, leaving the dot its place; other apps keep their cap.
  - Diameter: '5'
    $name: Dot size
    $description: Diameter of the dot, in pixels.
  - Color: '#878787|#AEAEAE'
    $name: Dot color
    $description: Hex color code, with '#Light|#Dark' for a color per theme. Leave empty for the system accent color.
  $name: Pinned Apps
- Progress:
  - FillColor: '#0F7B0F|#6CCB5F'
    $name: Progress color
    $description: Hex color code, with '#Light|#Dark' for a color per theme. Leave empty for the system accent color.
  - PausedColor: '#9D5D00|#FCE100'
    $name: Paused progress color
    $description: Same format.
  - ErrorColor: '#C42B1C|#FF99A4'
    $name: Error progress color
    $description: Same format.
  $name: Progress
  $description: Progress is shown on the indicators instead of the taskbar's own progress bar. A download, copy or install fills them from left to right in these colors; a busy app with no known end makes them run around the frame.
- Attention:
  - Enabled: true
    $name: Color apps requesting attention
    $description: While an app's taskbar button flashes for attention, its corner marks or underline take the attention color.
  - Color: '#DA3B01|#F7630C'
    $name: Attention color
    $description: Hex color code, with '#Light|#Dark' for a color per theme. Leave empty for the system accent color.
  $name: Attention
- Animations:
  - Enabled: true
    $name: Animate changes
    $description: Indicators move along the frame when an app's state changes. Off, or with Windows animations turned off, they change at once. Progress keeps animating either way.
  - Duration: 160
    $name: Duration (ms)
    $description: Length of one movement, in milliseconds.
  $name: Animations
- Rules:
  - GroupRule: any
    $name: Apps with several windows count as open
    $options:
    - any: When any of their windows is shown
    - all: Only when every window is shown
  - CloakedIsHidden: true
    $name: Count cloaked windows as not shown
    $description: Cloaked windows include windows on other virtual desktops and suspended Store apps.
  $name: Window Rules
  $description: What counts as an open window.
- Advanced:
  - DebugLog: false
    $name: Debug logging
    $description: For troubleshooting only. Logs what each taskbar button shows whenever it changes, and a summary of what triggered updates. Leave off otherwise.
  $name: Advanced
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/base.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <inspectable.h>
#include <unordered_map>
// The taskbar items live in a WinUI 2 ItemsRepeater built on system XAML;
// its projection is needed to ask the repeater which item a button shows.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <algorithm>
#include <atomic>
#include <cwctype>
#include <string>
#include <string_view>
#include <mutex>
#include <vector>
#include <functional>
#include <optional>
#include <utility>
#include <cmath>
#include <memory>
#include <chrono>

struct Settings {
    bool AccentShading = false;

    // Colors of every indicator: active (the foreground app's button; empty
    // = accent) and inactive (every other button).
    std::vector<winrt::Windows::UI::Color> ActiveLight, ActiveDark;
    double ActiveOpacityLight = 1.0, ActiveOpacityDark = 1.0;
    std::vector<winrt::Windows::UI::Color> InactiveLight, InactiveDark;
    double InactiveOpacityLight = 1.0, InactiveOpacityDark = 1.0;

    // The shared frame every indicator is drawn on: the distance of its
    // stroke's outer edge from the icon's edges, its corner radii (TL, TR,
    // BR, BL, on the stroke's centerline) and its stroke thickness (inwards
    // from the outer edge).
    winrt::Windows::UI::Xaml::Thickness FrameOffsets = {5, 5, 5, 5};
    double FrameRadius[4] = {4.0, 4.0, 4.0, 4.0};
    double FrameThickness = 2.0;

    // Corner marks (open / maximized windows).
    bool IndEnabled = true;
    bool IndConnect = false; // bottom corners joined along the bottom edge, top ones along the top
    bool IndMaximized = true;
    bool IndMaximizedBottom = true;  // maximized windows keep the bottom corners too
    // The corners an open or maximized app doesn't get, drawn dimmed.
    bool IndDimEnabled = false;
    std::vector<winrt::Windows::UI::Color> IndDimLight, IndDimDark;
    double IndDimOpacityLight = 0.2, IndDimOpacityDark = 0.2;
    double IndLeg = 5.0;
    bool IndDiagonal = false;
    bool IndRoundedEnds = false;

    // Window rules.
    bool IndGroupAll = false;
    bool IndIncludeActive = true;
    bool IndCloakedIsHidden = true;

    // Hides the taskbar's own running indicator (Rectangle#RunningIndicator).
    bool HideNativeIndicator = false;
    // Hides the taskbar's own progress bar (ProgressBar#ProgressIndicator),
    // determinate and indeterminate.
    bool HideNativeProgress = false;
    // Animates the indicators while the app shows indeterminate progress.
    bool ProgressAnimation = true;
    // Fills the indicators with determinate progress, in these colors.
    bool ProgressFill = true;
    std::vector<winrt::Windows::UI::Color> FillLight, FillDark;
    std::vector<winrt::Windows::UI::Color> FillPausedLight, FillPausedDark;
    std::vector<winrt::Windows::UI::Color> FillErrorLight, FillErrorDark;

    // Apps with a badge: native badges hidden, a dot centered on the
    // frame's right edge instead.
    bool BadgeEnabled = false;
    double BadgeDiameter = 4.0;
    std::vector<winrt::Windows::UI::Color> BadgeLight, BadgeDark;

    // Minimized apps: a centered piece of the frame's bottom edge.
    bool MinEnabled = false;
    double MinLength = 12.0; // 0 = the whole straight part of the bottom edge
    bool MinRounded = true;
    // Running off screen (cloaked, not minimized): a dimmed underline.
    bool MinHiddenEnabled = true;
    double MinHiddenOpacity = 0.4;
    // Requesting attention: the attention color.
    bool AttentionEnabled = true;
    std::vector<winrt::Windows::UI::Color> AttentionLight, AttentionDark;

    // Apps with several windows: a dot centered on the frame's left edge.
    // Pinned apps that aren't running: a dot centered on the frame's bottom
    // edge (and no cup for anyone, so its place stays free).
    bool PinnedEnabled = false;
    double PinnedDiameter = 5.0;
    std::vector<winrt::Windows::UI::Color> PinnedLight, PinnedDark;

    bool MultiEnabled = false;
    double MultiDiameter = 4.0;
    std::vector<winrt::Windows::UI::Color> MultiLight, MultiDark;

    // One log line per button whenever its summary changes.
    bool DebugLog = false;

    // Animations along the frame.
    bool Animations = true;
    int AnimationDurationMs = 160;

    bool AnyEnabled() const { return IndEnabled || MinEnabled || MultiEnabled || BadgeEnabled; }
} g_settings;

std::mutex g_settingsMutex;
// Mirrors g_settings.DebugLog for code that runs without a settings copy.
std::atomic<bool> g_debugLog{false};

// Debug logging: what triggers refreshes, reported every 10 s or so while
// refreshes happen (see ReportRefreshStats).
struct RefreshStats {
    std::atomic<int> total{0}, hook{0}, posted{0}, passes{0}, passButtons{0};
    std::atomic<int> evForeground{0}, evShowHide{0}, evMinimize{0}, evCloak{0}, evMaximize{0};
    std::atomic<ULONGLONG> since{0};
};
RefreshStats g_refreshStats;


namespace wuc = winrt::Windows::UI::Composition;

// The frame as a track: one clockwise closed path on the stroke's
// centerline, starting at the top center. Every point of it has a position
// s, from 0 to the path's length P; the corner arcs (their chords in the
// diagonal style) and the edge midpoints used by the indicators are given as
// such positions. Box coordinates: (0, 0) is the frame's outer top-left.
struct FrameMetrics {
    bool valid = false;
    double W = 0, H = 0, t = 0;
    double x0 = 0, y0 = 0, x1 = 0, y1 = 0, cx = 0, cy = 0;
    double half = 0;          // half the shorter side, between centerlines
    double r[4] = {};         // corner radii, TL, TR, BR, BL
    double arc[4] = {};       // length of each corner's arc (or chord)
    double arcStart[4] = {};  // s where each corner's arc starts
    double P = 0;             // whole length
    double sBottomMid = 0;    // middle of the bottom edge
    double sLeftMid = 0;      // middle of the left edge
    double sRightMid = 0;     // middle of the right edge
    bool diagonal = false;

    double CornerMid(int k) const { return arcStart[k] + arc[k] / 2.0; }
};

// The indicator shapes, one composition sprite shape each, all stroking the
// same frame path and trimmed to their piece of it. The underline is the BL
// and BR shapes parked side by side as its two halves.
constexpr int kSlotTL = 0, kSlotTR = 1, kSlotBR = 2, kSlotBL = 3, kSlotDot = 4, kSlotBadge = 5, kSlotPinned = 6;
constexpr int kSlotCount = 7;

// Where an animation of ours is, worked out from how it was started (see
// ScalarTweenNow): a restarted animation begins there, explicitly. A
// keyframe of this.StartingValue begins at the property's last value set
// directly instead, not where a running animation is, so an interrupted
// movement or fade jumped.
struct ScalarTween {
    float from = 0.0f, to = 0.0f;
    ULONGLONG start = 0;
    int delayMs = 0, durationMs = 0;  // 0: a value set, not animated
};

struct ColorTween {
    winrt::Windows::UI::Color from{}, to{};
    ULONGLONG start = 0;
    int durationMs = 0;               // 0: a value set, not animated
    bool fadeOut = false;             // held, then faded out (see AnimateSlotColors)
};

struct IndicatorSlot {
    wuc::CompositionSpriteShape shape{nullptr};
    wuc::CompositionPathGeometry geometry{nullptr};
    wuc::CompositionColorBrush colorBrush{nullptr};             // one color
    wuc::CompositionLinearGradientBrush gradientBrush{nullptr}; // several colors
    std::vector<wuc::CompositionColorGradientStop> stops;

    // The last target the shape was sent to.
    bool placed = false;      // given a position at least once
    bool shown = false;
    double a = 0.0, b = 0.0;  // its piece of the path, in pixels along it
    std::vector<winrt::Windows::UI::Color> colors;  // its colors while shown
    wuc::CompositionStrokeCap startCap = wuc::CompositionStrokeCap::Flat;
    wuc::CompositionStrokeCap endCap = wuc::CompositionStrokeCap::Flat;
    double thickness = -1.0;
    // Its ends are driven by the indeterminate sweep (see StartSweep); when
    // that ends, the next placement moves it from wherever it got to.
    bool forceMove = false;

    ScalarTween trimStartTween, trimEndTween;
    ScalarTween thicknessTween;            // the dots' growing and shrinking
    std::vector<ColorTween> colorTweens;   // one per brush color
};

// The composition content of one button: a shape visual set as the child
// visual of a small, layout-neutral XAML host in the icon's panel.
struct IndicatorRenderer {
    wuc::Compositor compositor{nullptr};
    wuc::ShapeVisual visual{nullptr};
    wuc::CompositionEasingFunction easing{nullptr};
    wuc::CompositionEasingFunction linear{nullptr};
    IndicatorSlot slots[kSlotCount];
    FrameMetrics metrics;     // of the current path
    double pad = 0.0;         // room around the box for caps and the dot

    // Indeterminate progress: 0 none, 1 the underline's sweep, 2 the
    // corners' clockwise lap.
    int8_t progressMode = 0;
    wuc::CompositionPropertySet progress{nullptr};   // the sweep's bar positions
    wuc::CompositionEasingFunction progressEasing{nullptr};
    wuc::CompositionEasingFunction stepEasing{nullptr};
    double sweepKey[6] = {-1, -1, -1, -1, -1, -1};   // the track, bar and direction it was started for
    ULONGLONG lapStartTick = 0;

    // Determinate progress: a twin of each corner shape, in the fill color,
    // whose trim is an expression of its base shape's live trim and of the
    // fill fraction, so it follows every movement of the base.
    struct FillTwin {
        wuc::CompositionSpriteShape shape{nullptr};
        wuc::CompositionPathGeometry geometry{nullptr};
        wuc::CompositionColorBrush brush{nullptr};
        wuc::CompositionStrokeCap startCap = wuc::CompositionStrokeCap::Flat;
        wuc::CompositionStrokeCap endCap = wuc::CompositionStrokeCap::Flat;
        double thickness = -1.0;
    };
    FillTwin fills[4];                              // TL, TR, BR, BL
    wuc::CompositionPropertySet fillProps{nullptr}; // f (fraction), alpha
    bool fillActive = false;
    bool fillBound = false;     // twins' trims bound, for fillRtl
    bool fillRtl = false;
    bool fillColorSet = false;
    winrt::Windows::UI::Color fillColor{};
    double fillFraction = 0.0;
    ULONGLONG fillTick = 0;   // when the last value came
};

// One entry per task list button. The indicators are composition shapes
// hosted under one XAML element in the icon's parent panel, glued to the
// icon with a composition expression; the entry caches lookups, event
// tokens and last-applied parameters.
struct ButtonEntry {
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> button;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> icon;

    // Subscriptions: icon SizeChanged, button Loaded/Unloaded/ActualThemeChanged.
    winrt::event_token iconSizeToken{};
    winrt::event_token unloadToken{};
    winrt::event_token loadedToken{};
    winrt::event_token themeToken{};
    bool iconSizeAttached = false;
    bool unloadAttached = false;
    bool loadedAttached = false;
    bool themeAttached = false;

    // The XAML host of the indicators and its binding to the icon.
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> host;
    bool hostBound = false;
    float hostAdjX = -1e9f, hostAdjY = -1e9f;
    IndicatorRenderer renderer;

    int8_t indState = -1; // -1 unknown, 0 none, 1 open, 2 maximized, 3 minimized
    int windowCount = -1; // last evaluated, -1 unknown
    int8_t isActive = -1; // last evaluated foreground state, -1 unknown

    // Native elements while this mod hides them (their handout visuals'
    // opacity is 0): the running indicator and the progress bar.
    struct HiddenNative {
        winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
        bool hidden = false;
    };
    HiddenNative nativeRunning;
    HiddenNative nativeProgress;
    HiddenNative nativeMulti;  // Border#MultiWindowElement, with the multi-window dot on

    // Border#BackgroundElement's right margin, 0 while the native
    // multi-window mark is hidden (see UpdateBackgroundMargin): the margin
    // it had before (re-read whenever the taskbar changes it), the one set,
    // and its own local value, put back when the adjustment ends.
    struct MarginAdjust {
        winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
        bool applied = false;
        winrt::Windows::UI::Xaml::Thickness base{}, value{};
        bool hadLocal = false;
        winrt::Windows::Foundation::IInspectable originalLocal{nullptr};
    };
    MarginAdjust backgroundMargin;

    // The app's pinned state as last read (-1: unknown), see ReadButtonPinned.
    int8_t pinnedState = -1;

    // The taskbar's own badge elements while badge support hides them: each
    // with the property-changed callbacks that refresh the button when the
    // badge appears or goes away.
    struct NativeBadge {
        winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
        int64_t visibilityToken = 0, opacityToken = 0, sourceToken = 0;
        winrt::event_token sizeToken{};
        bool sizeAttached = false;
        bool isImage = false;
        // Callbacks on its parents and inner elements too (see WatchBadgeTree).
        struct Watch {
            winrt::weak_ref<winrt::Windows::UI::Xaml::DependencyObject> object;
            winrt::Windows::UI::Xaml::DependencyProperty property{nullptr};
            int64_t token = 0;
        };
        std::vector<Watch> treeWatches;
    };
    std::vector<NativeBadge> nativeBadges;

    // The taskbar's own progress bar, watched for indeterminate progress:
    // the property-changed callbacks that refresh the button when it starts
    // or stops.
    struct ProgressWatch {
        winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
        std::vector<std::pair<winrt::Windows::UI::Xaml::DependencyProperty, int64_t>> tokens;
    };
    ProgressWatch progressWatch;

    std::wstring debugLast;   // the last debug summary logged for it

    // A refresh queued by a property-changed callback (see
    // PostCoalescedRefresh). UI thread only.
    bool refreshPosted = false;

    // The icon panel's child count, watched after every layout pass: badges
    // and overlay icons are added to and removed from it as they come and
    // go (see WatchIconPanel).
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> layoutPanel;
    winrt::event_token layoutToken{};
    bool layoutAttached = false;
    int layoutChildCount = -1;      // raw, for the cheap check on every pass
    int layoutForeignCount = -1;    // without the mod's own host

    // Debug logging: the button's view model, if it notifies property
    // changes, and the app name last logged for the button.
    winrt::Windows::UI::Xaml::Data::INotifyPropertyChanged debugViewModel{nullptr};
    winrt::event_token debugViewModelToken{};
    std::wstring debugApp;
    HWND debugHwnd = nullptr;     // the window debugProcess was looked up for
    std::wstring debugProcess;
};
std::mutex g_entriesMutex;
// Set under g_entriesMutex by the uninit snapshot; checked under the same
// lock wherever new work (entries, posts) could start.
std::atomic<bool> g_unloading{false};
// Kept out of the CRT's global destructors on principle (Wh_ModUninit does
// not run when explorer.exe itself terminates); the explicit reset() in
// Wh_ModBeforeUninit remains the release path for mod unload.
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<ButtonEntry>>>
    g_entries{std::in_place};

// Every RunAsync post whose lambda lives in this DLL takes a ticket here
// under g_entriesMutex, next to the same g_unloading check the uninit
// snapshot uses. The ticket is an object the lambda owns, so it is released
// when the lambda is destroyed - after it ran, or when the dispatcher drops
// it without running it. Wh_ModBeforeUninit waits for the count to drain,
// without a timeout. The rule: nothing from the mod image may be running or
// scheduled once Wh_ModUninit returns.
std::atomic<int> g_pendingPosts{0};
HANDLE g_postsDrained = nullptr; // manual-reset, created in Wh_ModInit

bool PostToUiThread(winrt::Windows::UI::Core::CoreDispatcher const& dispatcher,
                    std::function<void()> fn) {
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        if (g_unloading) return false; // no new posts after the uninit snapshot
        g_pendingPosts.fetch_add(1);
    }
    auto ticket = std::shared_ptr<void>(nullptr, [](void*) {
        if (g_pendingPosts.fetch_sub(1) == 1 && g_postsDrained) SetEvent(g_postsDrained);
    });
    try {
        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                            // The ticket is captured first, so it is destroyed
                            // last (Clang destroys captures in reverse order):
                            // fn's destructor runs before the drain is signaled.
                            [ticket, fn = std::move(fn)]() {
                                try { fn(); } catch (...) {}
                            });
        return true;
    } catch (...) {
        return false;  // the ticket goes with the lambda that wasn't posted
    }
}

std::atomic<bool> g_taskbarViewDllLoaded{false};

// ---------------------------------------------------------------------------
// Settings parsing
// ---------------------------------------------------------------------------

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring_view hexView) {
    if (hexView.empty()) return std::nullopt;
    std::wstring hex(hexView);
    hex.erase(0, hex.find_first_not_of(L" \t\r\n"));
    if (hex.empty()) return std::nullopt;
    hex.erase(hex.find_last_not_of(L" \t\r\n") + 1);
    if (hex[0] == L'#') hex.erase(0, 1);
    if (hex.length() == 6) hex = L"FF" + hex;
    if (hex.length() != 8) return std::nullopt;
    for (wchar_t ch : hex) {
        if (!iswxdigit(ch)) return std::nullopt;
    }
    try {
        uint32_t val = std::stoul(hex, nullptr, 16);
        return winrt::Windows::UI::Color{
            (uint8_t)((val >> 24) & 0xFF),
            (uint8_t)((val >> 16) & 0xFF),
            (uint8_t)((val >> 8) & 0xFF),
            (uint8_t)(val & 0xFF)
        };
    } catch (...) {
        return std::nullopt;
    }
}

std::vector<double> ParseDoubleList(PCWSTR str) {
    std::vector<double> vals;
    if (!str) return vals;
    std::wstring ws(str);
    size_t pos = 0;
    while (pos < ws.length()) {
        while (pos < ws.length() && (ws[pos] == L' ' || ws[pos] == L',')) pos++;
        if (pos >= ws.length()) break;
        size_t nextComma = ws.find(L',', pos);
        if (nextComma == std::wstring::npos) nextComma = ws.length();
        try {
            double val = std::stod(ws.substr(pos, nextComma - pos));
            if (std::isnan(val)) val = 0.0;
            vals.push_back(val);
        } catch (...) {}
        pos = nextComma + 1;
    }
    return vals;
}

void ParseDoublePair(PCWSTR str, double& outLight, double& outDark, double defLight, double defDark) {
    outLight = defLight; outDark = defDark;
    auto vals = ParseDoubleList(str);
    if (vals.size() == 1) {
        outLight = outDark = vals[0];
    } else if (vals.size() >= 2) {
        outLight = vals[0];
        outDark = vals[1];
    }
}

double ParseDouble(PCWSTR str, double defaultVal, double minVal = 0.0) {
    if (!str || !str[0]) return defaultVal;
    try {
        double val = std::stod(str);
        if (std::isnan(val)) return defaultVal;
        return std::max(val, minVal);
    } catch (...) {
        return defaultVal;
    }
}

// Frame offsets in XAML Thickness order: "all", "horizontal, vertical" or
// "left, top, right, bottom". Any other count is invalid and yields zeros
// (the frame's outer edge on the icon's edges) rather than silently
// discarding part of the input.
void ParseFrameOffsets(PCWSTR str, winrt::Windows::UI::Xaml::Thickness& outThickness) {
    double outL = 0.0, outT = 0.0, outR = 0.0, outB = 0.0;
    auto vals = ParseDoubleList(str);
    if (vals.size() == 1) {
        outL = outT = outR = outB = vals[0];
    } else if (vals.size() == 2) {
        outL = outR = vals[0];
        outT = outB = vals[1];
    } else if (vals.size() == 4) {
        outL = vals[0]; outT = vals[1]; outR = vals[2]; outB = vals[3];
    }
    outThickness = winrt::Windows::UI::Xaml::ThicknessHelper::FromLengths(outL, outT, outR, outB);
}

void ParseCornerRadius(PCWSTR str, double out[4], double defaultVal) {
    for (int i = 0; i < 4; i++) out[i] = defaultVal;
    auto vals = ParseDoubleList(str);
    for (auto& v : vals) v = std::max(v, 0.0);
    if (vals.size() == 1) {
        for (int i = 0; i < 4; i++) out[i] = vals[0];
    } else if (vals.size() == 2) {
        out[0] = out[1] = vals[0];
        out[2] = out[3] = vals[1];
    } else if (vals.size() == 4) {
        for (int i = 0; i < 4; i++) out[i] = vals[i];
    }
}

void ParseGradientColorPair(PCWSTR str, std::vector<winrt::Windows::UI::Color>& light, std::vector<winrt::Windows::UI::Color>& dark) {
    light.clear(); dark.clear();
    if (!str || !str[0]) return;
    std::wstring ws(str);

    size_t pipe = ws.find(L'|');
    std::wstring lightStr = (pipe != std::wstring::npos) ? ws.substr(0, pipe) : ws;
    std::wstring darkStr = (pipe != std::wstring::npos) ? ws.substr(pipe + 1) : ws;

    auto parseColors = [](std::wstring s, std::vector<winrt::Windows::UI::Color>& outList) {
        size_t pos = 0;
        while (pos < s.length()) {
            size_t next = s.find(L',', pos);
            std::wstring part = (next == std::wstring::npos) ? s.substr(pos) : s.substr(pos, next - pos);
            size_t start = part.find_first_not_of(L" \t\r\n");
            if (start != std::wstring::npos) part.erase(0, start);
            size_t end = part.find_last_not_of(L" \t\r\n");
            if (end != std::wstring::npos) part.erase(end + 1);
            if (!part.empty()) {
                auto c = ParseHexColor(part);
                if (c.has_value()) outList.push_back(c.value());
            }
            if (next == std::wstring::npos) break;
            pos = next + 1;
        }
    };

    parseColors(lightStr, light);
    parseColors(darkStr, dark);
}

void LoadSettings() {
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);

    g_settings.AccentShading = Wh_GetIntSetting(L"Colors.AccentShading") != 0;
    WindhawkUtils::StringSetting activeColStr(Wh_GetStringSetting(L"Colors.ActiveColor"));
    ParseGradientColorPair(activeColStr.get(), g_settings.ActiveLight, g_settings.ActiveDark);
    WindhawkUtils::StringSetting activeOpStr(Wh_GetStringSetting(L"Colors.ActiveOpacity"));
    ParseDoublePair(activeOpStr.get(), g_settings.ActiveOpacityLight, g_settings.ActiveOpacityDark, 1.0, 1.0);
    WindhawkUtils::StringSetting inactiveColStr(Wh_GetStringSetting(L"Colors.InactiveColor"));
    ParseGradientColorPair(inactiveColStr.get(), g_settings.InactiveLight, g_settings.InactiveDark);
    WindhawkUtils::StringSetting inactiveOpStr(Wh_GetStringSetting(L"Colors.InactiveOpacity"));
    ParseDoublePair(inactiveOpStr.get(), g_settings.InactiveOpacityLight, g_settings.InactiveOpacityDark, 1.0, 1.0);
    WindhawkUtils::StringSetting groupRuleStr(Wh_GetStringSetting(L"Rules.GroupRule"));
    g_settings.IndGroupAll = groupRuleStr.get() && wcscmp(groupRuleStr.get(), L"all") == 0;
    g_settings.IndIncludeActive = true;  // always: the app in front counts as open
    g_settings.IndCloakedIsHidden = Wh_GetIntSetting(L"Rules.CloakedIsHidden") != 0;
    // Always on: the mod's indicators take the place of the native running
    // indicator and progress bar, and show progress themselves.
    g_settings.HideNativeIndicator = true;
    g_settings.HideNativeProgress = true;
    g_settings.ProgressAnimation = true;
    g_settings.ProgressFill = true;
    WindhawkUtils::StringSetting fillColStr(Wh_GetStringSetting(L"Progress.FillColor"));
    ParseGradientColorPair(fillColStr.get(), g_settings.FillLight, g_settings.FillDark);
    WindhawkUtils::StringSetting fillPausedStr(Wh_GetStringSetting(L"Progress.PausedColor"));
    ParseGradientColorPair(fillPausedStr.get(), g_settings.FillPausedLight, g_settings.FillPausedDark);
    WindhawkUtils::StringSetting fillErrorStr(Wh_GetStringSetting(L"Progress.ErrorColor"));
    ParseGradientColorPair(fillErrorStr.get(), g_settings.FillErrorLight, g_settings.FillErrorDark);
    g_settings.BadgeEnabled = Wh_GetIntSetting(L"Badge.Enabled") != 0;
    WindhawkUtils::StringSetting badgeDiaStr(Wh_GetStringSetting(L"Badge.Diameter"));
    g_settings.BadgeDiameter = std::max(1.0, ParseDouble(badgeDiaStr.get(), 5.0));
    WindhawkUtils::StringSetting badgeColStr(Wh_GetStringSetting(L"Badge.Color"));
    ParseGradientColorPair(badgeColStr.get(), g_settings.BadgeLight, g_settings.BadgeDark);

    WindhawkUtils::StringSetting frameOffStr(Wh_GetStringSetting(L"Frame.Offsets"));
    ParseFrameOffsets(frameOffStr.get(), g_settings.FrameOffsets);
    WindhawkUtils::StringSetting frameRadStr(Wh_GetStringSetting(L"Frame.CornerRadius"));
    ParseCornerRadius(frameRadStr.get(), g_settings.FrameRadius, 4.0);
    WindhawkUtils::StringSetting frameThickStr(Wh_GetStringSetting(L"Frame.Thickness"));
    g_settings.FrameThickness = std::max(0.5, ParseDouble(frameThickStr.get(), 2.0));

    // Always on: corner marks for open windows, all four for maximized ones,
    // and the underline for minimized apps.
    g_settings.IndEnabled = true;
    g_settings.IndMaximized = true;
    g_settings.IndMaximizedBottom = Wh_GetIntSetting(L"Corners.MaximizedBottom") != 0;
    g_settings.IndDimEnabled = Wh_GetIntSetting(L"Corners.DimEnabled") != 0;
    WindhawkUtils::StringSetting dimColStr(Wh_GetStringSetting(L"Corners.DimColor"));
    ParseGradientColorPair(dimColStr.get(), g_settings.IndDimLight, g_settings.IndDimDark);
    WindhawkUtils::StringSetting dimOpStr(Wh_GetStringSetting(L"Corners.DimOpacity"));
    ParseDoublePair(dimOpStr.get(), g_settings.IndDimOpacityLight, g_settings.IndDimOpacityDark, 0.2, 0.2);
    g_settings.IndConnect = Wh_GetIntSetting(L"Frame.ConnectCorners") != 0;
    WindhawkUtils::StringSetting indLegStr(Wh_GetStringSetting(L"Corners.LegLength"));
    g_settings.IndLeg = ParseDouble(indLegStr.get(), 5.0);
    WindhawkUtils::StringSetting indStyleStr(Wh_GetStringSetting(L"Corners.CornerStyle"));
    g_settings.IndDiagonal = indStyleStr.get() && wcscmp(indStyleStr.get(), L"diagonal") == 0;
    g_settings.IndRoundedEnds = Wh_GetIntSetting(L"Corners.RoundedEnds") != 0;

    g_settings.MinEnabled = true;
    WindhawkUtils::StringSetting minLenStr(Wh_GetStringSetting(L"Minimized.Length"));
    g_settings.MinLength = ParseDouble(minLenStr.get(), 12.0);
    g_settings.MinRounded = Wh_GetIntSetting(L"Minimized.Rounded") != 0;
    g_settings.MinHiddenEnabled = Wh_GetIntSetting(L"Minimized.HiddenEnabled") != 0;
    WindhawkUtils::StringSetting minHiddenOpStr(Wh_GetStringSetting(L"Minimized.HiddenOpacity"));
    g_settings.MinHiddenOpacity = std::clamp(ParseDouble(minHiddenOpStr.get(), 0.4), 0.0, 1.0);
    g_settings.AttentionEnabled = Wh_GetIntSetting(L"Attention.Enabled") != 0;
    WindhawkUtils::StringSetting attentionColStr(Wh_GetStringSetting(L"Attention.Color"));
    ParseGradientColorPair(attentionColStr.get(), g_settings.AttentionLight, g_settings.AttentionDark);

    g_settings.PinnedEnabled = Wh_GetIntSetting(L"Pinned.Enabled") != 0;
    WindhawkUtils::StringSetting pinnedColStr(Wh_GetStringSetting(L"Pinned.Color"));
    ParseGradientColorPair(pinnedColStr.get(), g_settings.PinnedLight, g_settings.PinnedDark);
    WindhawkUtils::StringSetting pinnedDiaStr(Wh_GetStringSetting(L"Pinned.Diameter"));
    g_settings.PinnedDiameter = std::max(1.0, ParseDouble(pinnedDiaStr.get(), 5.0));

    g_settings.MultiEnabled = Wh_GetIntSetting(L"Multi.Enabled") != 0;
    WindhawkUtils::StringSetting multiDiaStr(Wh_GetStringSetting(L"Multi.Diameter"));
    g_settings.MultiDiameter = std::max(1.0, ParseDouble(multiDiaStr.get(), 5.0));
    WindhawkUtils::StringSetting multiColStr(Wh_GetStringSetting(L"Multi.Color"));
    ParseGradientColorPair(multiColStr.get(), g_settings.MultiLight, g_settings.MultiDark);

    g_settings.Animations = Wh_GetIntSetting(L"Animations.Enabled") != 0;
    g_settings.DebugLog = Wh_GetIntSetting(L"Advanced.DebugLog") != 0;
    g_debugLog = g_settings.DebugLog;
    if (!g_settings.DebugLog) g_refreshStats.since = 0;
    {
        int duration = Wh_GetIntSetting(L"Animations.Duration");
        g_settings.AnimationDurationMs = std::clamp(duration > 0 ? duration : 160, 30, 2000);
    }
}

// ---------------------------------------------------------------------------
// Colors and geometry
// ---------------------------------------------------------------------------

inline winrt::Windows::UI::Color ApplyOpacity(winrt::Windows::UI::Color c, double opacity) {
    if (opacity < 0.0) opacity = 0.0;
    if (opacity > 1.0) opacity = 1.0;
    c.A = static_cast<uint8_t>(c.A * opacity);
    return c;
}

bool IsElementLightMode(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
    if (!element) return false;
    try {
        auto theme = element.ActualTheme();
        if (theme == winrt::Windows::UI::Xaml::ElementTheme::Light) return true;
        if (theme == winrt::Windows::UI::Xaml::ElementTheme::Dark) return false;
        return winrt::Windows::UI::Xaml::Application::Current().RequestedTheme() == winrt::Windows::UI::Xaml::ApplicationTheme::Light;
    } catch (...) {
        return false;
    }
}

std::vector<winrt::Windows::UI::Color> ResolveThemedColors(
    const std::vector<winrt::Windows::UI::Color>& light, const std::vector<winrt::Windows::UI::Color>& dark,
    double opacityLight, double opacityDark, bool isLight, bool accentShading) {
    double opacity = isLight ? opacityLight : opacityDark;

    auto c = isLight ? light : dark;
    if (!c.empty()) {
        for (auto& col : c) col = ApplyOpacity(col, opacity);
        return c;
    }

    try {
        auto res = winrt::Windows::UI::Xaml::Application::Current().Resources();
        auto resName = !accentShading ? L"SystemAccentColor"
                     : isLight        ? L"SystemAccentColorDark1"
                                      : L"SystemAccentColorLight2";
        if (res.HasKey(winrt::box_value(resName))) {
            return {ApplyOpacity(winrt::unbox_value<winrt::Windows::UI::Color>(res.Lookup(winrt::box_value(resName))), opacity)};
        }
    } catch (...) {}
    return {ApplyOpacity({255, 0, 120, 212}, opacity)};
}

// High contrast themes: every color of the mod gives way to the system's
// own: the highlight color for the active app, the badge and attention,
// button text for the others, the hyperlink color for progress.
bool HighContrastOn() {
    HIGHCONTRASTW hc{};
    hc.cbSize = sizeof(hc);
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0)) return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
    return false;
}

winrt::Windows::UI::Color SystemColor(int index) {
    COLORREF c = GetSysColor(index);
    return {255, GetRValue(c), GetGValue(c), GetBValue(c)};
}

// The color of every indicator on a button: the active color for the
// foreground app's button, the inactive color for all others.
std::vector<winrt::Windows::UI::Color> GetIndicatorColors(const Settings& localSettings, bool isLight, bool isActive) {
    if (HighContrastOn()) return {SystemColor(isActive ? COLOR_HIGHLIGHT : COLOR_BTNTEXT)};
    return isActive
        ? ResolveThemedColors(localSettings.ActiveLight, localSettings.ActiveDark,
                              localSettings.ActiveOpacityLight, localSettings.ActiveOpacityDark, isLight,
                              localSettings.AccentShading)
        : ResolveThemedColors(localSettings.InactiveLight, localSettings.InactiveDark,
                              localSettings.InactiveOpacityLight, localSettings.InactiveOpacityDark, isLight,
                              localSettings.AccentShading);
}

bool SameColors(const std::vector<winrt::Windows::UI::Color>& a, const std::vector<winrt::Windows::UI::Color>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].A != b[i].A || a[i].R != b[i].R || a[i].G != b[i].G || a[i].B != b[i].B) return false;
    }
    return true;
}

// The frame's box in the icon's coordinates: it spans the frame's outer
// edges, `offset` away from each icon edge, and the stroke (drawn on the
// centerline, half a thickness in) grows inwards from there, so a thicker
// frame keeps its outer size. Both outer edges are rounded to whole pixels
// (not just the origin), so a fractional offset can't shift the frame to
// one side.
struct FrameBox {
    double W, H;      // box size
    float adjX, adjY; // box origin relative to the icon's top-left
};

FrameBox ComputeFrameBox(double iw, double ih, const Settings& localSettings) {
    auto off = localSettings.FrameOffsets;
    double left = std::round(-off.Left), right = std::round(iw + off.Right);
    double top = std::round(-off.Top), bottom = std::round(ih + off.Bottom);
    FrameBox box;
    box.W = std::max(1.0, right - left);
    box.H = std::max(1.0, bottom - top);
    box.adjX = (float)left;
    box.adjY = (float)top;
    return box;
}

// Lays the frame path out as positions along it (see FrameMetrics). The
// order is the one BuildFramePathD2D draws: from the top center, clockwise,
// TR arc, right edge, BR arc, bottom edge, BL arc, left edge, TL arc, back to
// the top center.
FrameMetrics ComputeFrameMetrics(double W, double H, double t, const double radius[4], bool diagonal) {
    FrameMetrics m;
    m.W = W; m.H = H; m.t = t; m.diagonal = diagonal;
    double inset = std::max(0.0, t) / 2.0;
    m.x0 = inset; m.y0 = inset;
    m.x1 = std::max(m.x0 + 1.0, W - inset);
    m.y1 = std::max(m.y0 + 1.0, H - inset);
    m.cx = (m.x0 + m.x1) / 2.0;
    m.cy = (m.y0 + m.y1) / 2.0;
    m.half = std::min(m.x1 - m.x0, m.y1 - m.y0) / 2.0;
    for (int k = 0; k < 4; k++) {
        m.r[k] = std::clamp(radius[k], 0.0, m.half);
        m.arc[k] = diagonal ? m.r[k] * std::sqrt(2.0) : m.r[k] * 3.14159265358979323846 / 2.0;
    }

    double s = 0.0;
    s += (m.x1 - m.r[1]) - m.cx;                    // top edge, right half
    m.arcStart[1] = s; s += m.arc[1];               // TR
    m.sRightMid = s + (m.cy - (m.y0 + m.r[1]));
    s += (m.y1 - m.r[2]) - (m.y0 + m.r[1]);         // right edge
    m.arcStart[2] = s; s += m.arc[2];               // BR
    m.sBottomMid = s + ((m.x1 - m.r[2]) - m.cx);
    s += (m.x1 - m.r[2]) - (m.x0 + m.r[3]);         // bottom edge
    m.arcStart[3] = s; s += m.arc[3];               // BL
    m.sLeftMid = s + ((m.y1 - m.r[3]) - m.cy);
    s += (m.y1 - m.r[3]) - (m.y0 + m.r[0]);         // left edge
    m.arcStart[0] = s; s += m.arc[0];               // TL
    s += m.cx - (m.x0 + m.r[0]);                    // top edge, left half
    m.P = std::max(1.0, s);
    m.valid = true;
    return m;
}

// ---------------------------------------------------------------------------
// The frame path for composition. A CompositionPath is made from an
// IGeometrySource2D whose IGeometrySource2DInterop hands out a Direct2D
// geometry; without Win2D in explorer.exe, this mod implements that small
// COM object itself. Composition may keep the object for as long as the
// path lives, so every one of them is counted, and unloading waits until
// the count is back to 0 (the paths are closed by then).
// ---------------------------------------------------------------------------

struct IGeometrySource2DInteropAbi : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetGeometry(ID2D1Geometry** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetGeometryUsingFactory(ID2D1Factory* factory, ID2D1Geometry** value) = 0;
};

constexpr GUID kIID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID kIID_IInspectable = {0xAF86E2E0, 0xB12D, 0x4C6A, {0x9C, 0x5A, 0xD7, 0xAA, 0x65, 0x10, 0x1E, 0x90}};
constexpr GUID kIID_IGeometrySource2DInterop = {0x0657AF73, 0x53FD, 0x47CF, {0x84, 0xFF, 0xC8, 0x49, 0x2D, 0x2A, 0x80, 0xA3}};
constexpr GUID kIID_ID2D1Factory = {0x06152247, 0x6F50, 0x465A, {0x92, 0x45, 0x11, 0x8B, 0xFD, 0x3B, 0x60, 0x07}};

GUID IID_IGeometrySource2D() {
    winrt::guid g = winrt::guid_of<winrt::Windows::Graphics::IGeometrySource2D>();
    return reinterpret_cast<GUID const&>(g);
}

std::atomic<int> g_liveGeometrySources{0};

class FrameGeometrySource final : public IInspectable, public IGeometrySource2DInteropAbi {
public:
    explicit FrameGeometrySource(winrt::com_ptr<ID2D1Geometry> geometry) : m_geometry(std::move(geometry)) {
        g_liveGeometrySources.fetch_add(1);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == kIID_IUnknown || riid == kIID_IInspectable || riid == IID_IGeometrySource2D()) {
            *ppv = static_cast<IInspectable*>(this);
        } else if (riid == kIID_IGeometrySource2DInterop) {
            *ppv = static_cast<IGeometrySource2DInteropAbi*>(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return m_refs.fetch_add(1) + 1; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG refs = m_refs.fetch_sub(1) - 1;
        if (refs == 0) delete this;
        return refs;
    }

    HRESULT STDMETHODCALLTYPE GetIids(ULONG* count, IID** iids) override {
        if (!count || !iids) return E_POINTER;
        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));
        if (!*iids) return E_OUTOFMEMORY;
        (*iids)[0] = IID_IGeometrySource2D();
        *count = 1;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className) override {
        if (!className) return E_POINTER;
        *className = nullptr;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* level) override {
        if (!level) return E_POINTER;
        *level = BaseTrust;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetGeometry(ID2D1Geometry** value) override {
        if (!value) return E_POINTER;
        *value = m_geometry.get();
        (*value)->AddRef();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE TryGetGeometryUsingFactory(ID2D1Factory*, ID2D1Geometry** value) override {
        if (value) *value = nullptr;
        return E_NOTIMPL;
    }

private:
    ~FrameGeometrySource() { g_liveGeometrySources.fetch_sub(1); }

    std::atomic<ULONG> m_refs{1};
    winrt::com_ptr<ID2D1Geometry> m_geometry;
};

std::mutex g_d2dFactoryMutex;
// Released by Wh_ModUninit; kept out of the CRT's destructors, which run at
// Explorer's own exit under the loader lock, with Wh_ModUninit not called.
[[clang::no_destroy]] winrt::com_ptr<ID2D1Factory> g_d2dFactory;

winrt::com_ptr<ID2D1Factory> GetD2DFactory() {
    std::lock_guard<std::mutex> lock(g_d2dFactoryMutex);
    if (!g_d2dFactory) {
        D2D1_FACTORY_OPTIONS options{};
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, kIID_ID2D1Factory, &options,
                                       g_d2dFactory.put_void());
        if (FAILED(hr)) {
            Wh_Log(L"D2D1CreateFactory failed: %08X", (unsigned)hr);
            g_d2dFactory = nullptr;
        }
    }
    return g_d2dFactory;
}

// The frame path in box coordinates, as laid out by ComputeFrameMetrics.
winrt::com_ptr<ID2D1PathGeometry> BuildFramePathD2D(const FrameMetrics& m) {
    auto factory = GetD2DFactory();
    if (!factory) return nullptr;
    winrt::com_ptr<ID2D1PathGeometry> path;
    if (FAILED(factory->CreatePathGeometry(path.put()))) return nullptr;
    winrt::com_ptr<ID2D1GeometrySink> sink;
    if (FAILED(path->Open(sink.put()))) return nullptr;

    auto pt = [](double x, double y) { return D2D1_POINT_2F{(float)x, (float)y}; };
    auto corner = [&](double rad, double x, double y) {
        if (rad > 0.0 && !m.diagonal) {
            D2D1_ARC_SEGMENT arc{};
            arc.point = pt(x, y);
            arc.size = D2D1_SIZE_F{(float)rad, (float)rad};
            arc.rotationAngle = 0.0f;
            arc.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
            arc.arcSize = D2D1_ARC_SIZE_SMALL;
            sink->AddArc(&arc);
        } else {
            sink->AddLine(pt(x, y));
        }
    };

    sink->BeginFigure(pt(m.cx, m.y0), D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(pt(m.x1 - m.r[1], m.y0));
    corner(m.r[1], m.x1, m.y0 + m.r[1]);
    sink->AddLine(pt(m.x1, m.y1 - m.r[2]));
    corner(m.r[2], m.x1 - m.r[2], m.y1);
    sink->AddLine(pt(m.x0 + m.r[3], m.y1));
    corner(m.r[3], m.x0, m.y1 - m.r[3]);
    sink->AddLine(pt(m.x0, m.y0 + m.r[0]));
    corner(m.r[0], m.x0 + m.r[0], m.y0);
    sink->AddLine(pt(m.cx, m.y0));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    if (FAILED(sink->Close())) return nullptr;
    return path;
}

wuc::CompositionPath CreateFramePath(const FrameMetrics& m) {
    auto d2dPath = BuildFramePathD2D(m);
    if (!d2dPath) return nullptr;
    // A path geometry is a geometry: an upcast, no QueryInterface needed.
    winrt::com_ptr<ID2D1Geometry> geometry;
    geometry.copy_from(static_cast<ID2D1Geometry*>(d2dPath.get()));
    auto* source = new (std::nothrow) FrameGeometrySource(std::move(geometry));
    if (!source) return nullptr;
    // Takes over the construction reference.
    winrt::Windows::Graphics::IGeometrySource2D projected{static_cast<IInspectable*>(source),
                                                         winrt::take_ownership_from_abi};
    try {
        return wuc::CompositionPath(projected);
    } catch (...) {
        Wh_Log(L"Creating the frame's composition path failed");
        return nullptr;
    }
}

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Hosting;

// ---------------------------------------------------------------------------
// XAML tree helpers
// ---------------------------------------------------------------------------

FrameworkElement GetFrameworkElementFromNative(void* pThis) {
    if (!pThis) return nullptr;
    try {
        void* iUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iUnknown;
        winrt::copy_from_abi(iUnknown, iUnknownPtr);
        return iUnknown.try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement FindChildByName(FrameworkElement const& parent, std::wstring_view name, int depth = 0);

// The first element under `root` whose class name contains `part` (a
// fallback for templates that don't name the element).
FrameworkElement FindChildByClassPart(FrameworkElement const& root, std::wstring_view part, int depth = 0) {
    if (!root || depth > 6) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        std::wstring_view className = winrt::get_class_name(child);
        if (className.find(part) != std::wstring_view::npos) return child;
        if (auto result = FindChildByClassPart(child, part, depth + 1)) return result;
    }
    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement const& parent, std::wstring_view name, int depth) {
    if (!parent || depth > 5) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (child.Name() == name) return child;
            auto result = FindChildByName(child, name, depth + 1);
            if (result) return result;
        }
    }
    return nullptr;
}

// The name of the XAML element each button's indicators live under.
constexpr wchar_t kHostName[] = L"ButtonStateIndicatorHost";

// Retires an element of ours in place while the mod runs: hidden, inert and
// unbound, but left in its panel, to be adopted again by the next pass (a
// recycled button, an icon that moved panels). On unload every host is
// removed instead (RemoveHostsFromTaskbarThread).
void RetireElement(FrameworkElement const& element) {
    try {
        ElementCompositionPreview::GetElementVisual(element).Properties().StopAnimation(L"Translation");
    } catch (...) {}
    try {
        element.Opacity(0.0);
        element.Visibility(Visibility::Collapsed);
    } catch (...) {}
}

FrameworkElement FindRetiredElement(Panel const& panel, std::wstring_view name) {
    try {
        auto children = panel.Children();
        uint32_t n = children.Size();
        for (uint32_t i = 0; i < n; i++) {
            auto element = children.GetAt(i).try_as<FrameworkElement>();
            if (element && element.Name() == name && element.Visibility() == Visibility::Collapsed) return element;
        }
    } catch (...) {}
    return nullptr;
}

// ---------------------------------------------------------------------------
// Open window indicators, part 1: window state. Ported from Taskbar Open
// Window Underline: a button's windows come from the taskbar's own view
// model and are judged with IsIconic / IsZoomed plus DWM cloaking.
// ---------------------------------------------------------------------------

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        return cloaked != 0;
    }
    return false;
}

bool IsWindowShown(HWND hwnd, const Settings& localSettings) {
    // Covers hidden windows and windows that were just closed but are still
    // listed in their group until the taskbar catches up: for a destroyed
    // HWND, IsIconic returns FALSE and DWMWA_CLOAKED fails, which would
    // count it as shown.
    if (!IsWindowVisible(hwnd)) return false;
    // IsIconic is the source of truth. Win+D minimizes windows for real.
    if (IsIconic(hwnd)) return false;
    if (localSettings.IndCloakedIsHidden && IsWindowCloaked(hwnd)) return false;
    return true;
}

bool IsWindowShownMaximized(HWND hwnd, const Settings& localSettings) {
    return IsWindowShown(hwnd, localSettings) && IsZoomed(hwnd);
}

bool IsWindowMinimized(HWND hwnd) {
    // Visible excludes windows just closed but still listed in their group.
    return IsWindowVisible(hwnd) && IsIconic(hwnd);
}

// Virtual desktops, through the documented IVirtualDesktopManager. Windows
// cloaks the windows of other desktops that would be on screen, but not
// the minimized ones, so cloaking alone can't tell a window minimized on
// another desktop from one minimized here. The manager is created once, on
// the taskbar thread where every refresh runs, and released there by the
// unload cleanup; windows shown on all desktops count as on this one.
struct IVirtualDesktopManagerAbi : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(HWND topLevelWindow, BOOL* onCurrentDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(HWND topLevelWindow, GUID* desktopId) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(HWND topLevelWindow, REFGUID desktopId) = 0;
};

constexpr GUID kCLSID_VirtualDesktopManager = {0xAA509086, 0x5CA9, 0x4C25, {0x8F, 0x95, 0x58, 0x9D, 0x3C, 0x07, 0xB4, 0x8A}};
constexpr GUID kIID_IVirtualDesktopManager = {0xA5CD92FF, 0x29BE, 0x454C, {0x8D, 0x04, 0xD8, 0x28, 0x79, 0xFB, 0x3F, 0x1B}};

// Created and released on the taskbar thread, where every refresh runs.
[[clang::no_destroy]] winrt::com_ptr<IVirtualDesktopManagerAbi> g_virtualDesktopManager;
bool g_virtualDesktopManagerTried = false;

bool IsWindowOnOtherDesktop(HWND hwnd) {
    if (!g_virtualDesktopManagerTried) {
        g_virtualDesktopManagerTried = true;
        HRESULT hr = CoCreateInstance(kCLSID_VirtualDesktopManager, nullptr, CLSCTX_ALL, kIID_IVirtualDesktopManager,
                                      g_virtualDesktopManager.put_void());
        if (FAILED(hr)) {
            g_virtualDesktopManager = nullptr;
            Wh_Log(L"VirtualDesktopManager unavailable: %08X", (unsigned)hr);
        }
    }
    if (!g_virtualDesktopManager) return false;
    BOOL onCurrent = TRUE;
    if (FAILED(g_virtualDesktopManager->IsWindowOnCurrentVirtualDesktop(hwnd, &onCurrent))) return false;
    return !onCurrent;
}

// Running off screen: on another virtual desktop (minimized there or not),
// or cloaked (a suspended Store app).
bool IsWindowRunningOffScreen(HWND hwnd) {
    return IsWindowVisible(hwnd) && (IsWindowCloaked(hwnd) || IsWindowOnOtherDesktop(hwnd));
}

// Minimized here: minimized on the current desktop and, when cloaked
// windows count as hidden, not cloaked.
bool IsWindowMinimizedHere(HWND hwnd, const Settings& localSettings) {
    if (!IsWindowMinimized(hwnd)) return false;
    if (!localSettings.IndCloakedIsHidden) return true;
    return !IsWindowCloaked(hwnd) && !IsWindowOnOtherDesktop(hwnd);
}

// The foreground window as a button sees it: it may be an owned dialog or a
// child of the button's window, so its root and root owner count too.
struct ForegroundInfo {
    HWND window = nullptr, root = nullptr, rootOwner = nullptr;
};

ForegroundInfo GetForegroundInfo() {
    ForegroundInfo info;
    info.window = GetForegroundWindow();
    if (info.window) {
        info.root = GetAncestor(info.window, GA_ROOT);
        info.rootOwner = GetAncestor(info.window, GA_ROOTOWNER);
    }
    return info;
}

bool IsForegroundWindowOf(HWND hwnd, const ForegroundInfo& fg) {
    return fg.window && (hwnd == fg.window || hwnd == fg.root || hwnd == fg.rootOwner);
}

// Counts the windows that satisfy `qualifies` and applies the rules for
// buttons with several windows and, unless `includeActive`, for the
// foreground window, which then can't be the sole reason for a mark.
bool MatchesWindowRules(const std::vector<HWND>& windows, const Settings& localSettings,
                        bool (*qualifies)(HWND, const Settings&), bool includeActive) {
    if (windows.empty()) return false;

    size_t count = 0;
    for (HWND hwnd : windows) {
        if (qualifies(hwnd, localSettings)) count++;
    }

    bool foregroundCounts = false;
    if (!includeActive) {
        ForegroundInfo fg = GetForegroundInfo();
        for (HWND hwnd : windows) {
            if (IsForegroundWindowOf(hwnd, fg) && qualifies(hwnd, localSettings)) {
                foregroundCounts = true;
                break;
            }
        }
    }
    size_t besidesForeground = count - (foregroundCounts ? 1 : 0);

    if (localSettings.IndGroupAll) {
        return count == windows.size() && (includeActive || besidesForeground > 0);
    }
    return besidesForeground > 0;
}

// ---------------------------------------------------------------------------
// Open window indicators, part 2: button -> windows, through the taskbar's
// view model. Every TaskListButton is created by an ItemsRepeater for an
// item, and the item is its view model: a TaskListWindowViewModel for one
// window, or a TaskListGroupViewModel for a combined group. Both are backed
// by WindowsUdk.UI.Shell objects that know their HWNDs. The interface
// layouts and symbols are the ones the Taskbar Labels mod uses.
// ---------------------------------------------------------------------------

// {b081d9d6-9b45-5363-8a4d-854add6abe7e}
constexpr winrt::guid IID_ITaskItem{0xb081d9d6, 0x9b45, 0x5363, {0x8a, 0x4d, 0x85, 0x4a, 0xdd, 0x6a, 0xbe, 0x7e}};

struct ITaskItem : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowId(HWND* value) = 0;
};

// {a152e779-93df-5be2-af2c-d342452b0ce0}
constexpr winrt::guid IID_ITaskGroup{0xa152e779, 0x93df, 0x5be2, {0xaf, 0x2c, 0xd3, 0x42, 0x45, 0x2b, 0x0c, 0xe0}};

struct ITaskGroup : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessibleName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Icon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIcon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIconDescription() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Badge() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRequestingAttention() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Progress() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Items(void** value) = 0;
};

// Taskbar.View.dll symbols, all optional: without them the indicators are
// inactive.
void* TaskListWindowViewModel_ITaskListWindowViewModel_vftable;
void* TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable;
void* TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable;
void* TaskListGroupViewModel_ITaskListGroupViewModel_vftable;

using ITaskListWindowViewModel_get_TaskItem_t = HRESULT(WINAPI*)(void* pThis, void** taskItem);
ITaskListWindowViewModel_get_TaskItem_t ITaskListWindowViewModel_get_TaskItem;

using TaskListGroupViewModel_get_TaskGroup_t = HRESULT(WINAPI*)(void* pThis, void** taskGroup);
TaskListGroupViewModel_get_TaskGroup_t TaskListGroupViewModel_get_TaskGroup;

// Whether the button's app is pinned, from its view model (optional; the
// interface that has it is tried for both kinds of view model).
using ViewModel_get_IsPinned_t = HRESULT(WINAPI*)(void* pThis, bool* value);
ViewModel_get_IsPinned_t TaskListWindowViewModel_AppItem_get_IsPinned;
ViewModel_get_IsPinned_t TaskListWindowViewModel_Window_get_IsPinned;
ViewModel_get_IsPinned_t TaskListGroupViewModel_AppItem_get_IsPinned;
ViewModel_get_IsPinned_t TaskListGroupViewModel_Group_get_IsPinned;

bool IsPinnedStateAvailable() {
    return TaskListWindowViewModel_AppItem_get_IsPinned || TaskListWindowViewModel_Window_get_IsPinned ||
           TaskListGroupViewModel_AppItem_get_IsPinned || TaskListGroupViewModel_Group_get_IsPinned;
}

bool IsViewModelBridgeAvailable() {
    return TaskListWindowViewModel_ITaskListWindowViewModel_vftable && ITaskListWindowViewModel_get_TaskItem &&
           TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable && TaskListGroupViewModel_get_TaskGroup;
}

HWND GetTaskItemWindow(::IUnknown* taskItemUnknown) {
    winrt::com_ptr<ITaskItem> taskItem;
    if (FAILED(taskItemUnknown->QueryInterface(IID_ITaskItem, taskItem.put_void()))) return nullptr;

    HWND hWnd = nullptr;
    if (FAILED(taskItem->get_WindowId(&hWnd)) || !hWnd) return nullptr;

    // The taskbar may hand back a non-root window (a UWP core window inside
    // its frame, say); the minimized state lives on the top-level window.
    if (HWND root = GetAncestor(hWnd, GA_ROOT)) hWnd = root;
    return hWnd;
}

// Reads one pointer-sized value without faulting on a bad address.
bool SafeReadPointer(const void* address, void** value) {
    SIZE_T read = 0;
    return ReadProcessMemory(GetCurrentProcess(), address, value, sizeof(*value), &read) && read == sizeof(*value);
}

enum class ViewModelKind { kNone, kWindow, kGroup };

// Decides whether the object behind `object` is a TaskListWindowViewModel or
// a TaskListGroupViewModel from the known vtable it already points to. A
// C++/WinRT object starts with one vtable pointer per implemented
// interface, so one of the known vtables is among the first slots whichever
// interface the pointer refers to.
ViewModelKind GetViewModelKind(void* object) {
    auto slots = static_cast<void**>(object);
    for (int i = 0; i < 12; i++) {
        void* vftable = nullptr;
        if (!SafeReadPointer(&slots[i], &vftable)) break;
        if (!vftable) continue;
        if (vftable == TaskListWindowViewModel_ITaskListWindowViewModel_vftable ||
            vftable == TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable) {
            return ViewModelKind::kWindow;
        }
        if (vftable == TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable ||
            vftable == TaskListGroupViewModel_ITaskListGroupViewModel_vftable) {
            return ViewModelKind::kGroup;
        }
    }
    return ViewModelKind::kNone;
}

// Finds the interface sub-object whose vtable is `vftable`, which must be a
// vtable this object's kind actually has. The pointer the items source hands
// out may be an upcast interface pointer rather than the object's first
// slot, so the wanted interface can sit on either side of it. Nearest slot
// first, so a slot of this object is always found before anything outside.
void* FindInterfaceByVtable(void* object, void* vftable) {
    if (!object || !vftable) return nullptr;
    auto slots = static_cast<void**>(object);
    for (int distance = 0; distance <= 16; distance++) {
        for (int i : {distance, -distance}) {
            void* value = nullptr;
            if (SafeReadPointer(&slots[i], &value) && value == vftable) return &slots[i];
        }
    }
    return nullptr;
}

// The index of the item a realized button shows, or -1 if the repeater
// doesn't currently associate the button with any item: an ItemsRepeater
// keeps recycled elements in its visual tree for reuse, and those carry
// stale content but aren't shown.
int GetRepeaterItemIndex(const winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater& repeater,
                         const FrameworkElement& button, int count) {
    int index = repeater.GetElementIndex(button);
    if (index >= 0) return index < count ? index : -1;

    // GetElementIndex can decline for elements it does hold; the inverse
    // query is authoritative. Compare COM identities.
    void* buttonIdentity = winrt::get_abi(button.as<winrt::Windows::Foundation::IUnknown>());
    for (int i = 0; i < count; i++) {
        auto element = repeater.TryGetElement(i);
        if (element && winrt::get_abi(element.as<winrt::Windows::Foundation::IUnknown>()) == buttonIdentity) return i;
    }
    return -1;
}

enum class ButtonWindowsState {
    kUnrealized,   // A recycled repeater child: not shown, nothing to do.
    kUnavailable,  // No view model reachable.
    kResolved,     // `windows` holds the button's windows (possibly none).
};

struct ButtonWindows {
    ButtonWindowsState state;
    std::vector<HWND> windows;
};

// The windows behind a button, from its view model.
ButtonWindows ResolveButtonWindows(const FrameworkElement& button) {
    ButtonWindows result{ButtonWindowsState::kUnavailable, {}};

    winrt::com_ptr<::IUnknown> viewModel;
    ViewModelKind kind = ViewModelKind::kNone;
    try {
        auto parent = VisualTreeHelper::GetParent(button);
        auto repeater = parent ? parent.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>() : nullptr;
        if (repeater) {
            auto itemsSourceView = repeater.ItemsSourceView();
            int count = itemsSourceView ? itemsSourceView.Count() : 0;
            int index = GetRepeaterItemIndex(repeater, button, count);
            if (index < 0) {
                result.state = ButtonWindowsState::kUnrealized;
                return result;
            }
            auto item = itemsSourceView.GetAt(index);
            if (item) kind = GetViewModelKind(winrt::get_abi(item));
            if (kind != ViewModelKind::kNone) {
                viewModel.copy_from(static_cast<::IUnknown*>(winrt::get_abi(item)));
            } else if (item) {
                static std::atomic<bool> logged;
                if (!logged.exchange(true)) {
                    Wh_Log(L"Repeater item not recognized: %s", winrt::get_class_name(item).c_str());
                }
            }
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Repeater lookup error: %08X", static_cast<unsigned int>(e.code()));
    }

    if (!viewModel || !IsViewModelBridgeAvailable()) return result;

    if (kind == ViewModelKind::kWindow) {
        void* windowViewModel = FindInterfaceByVtable(viewModel.get(), TaskListWindowViewModel_ITaskListWindowViewModel_vftable);
        if (!windowViewModel) return result;
        winrt::com_ptr<::IUnknown> taskItem;
        if (SUCCEEDED(ITaskListWindowViewModel_get_TaskItem(windowViewModel, taskItem.put_void())) && taskItem) {
            if (HWND hWnd = GetTaskItemWindow(taskItem.get())) result.windows.push_back(hWnd);
        }
        result.state = ButtonWindowsState::kResolved;
        return result;
    }

    if (kind == ViewModelKind::kGroup) {
        void* groupViewModel = FindInterfaceByVtable(viewModel.get(), TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable);
        if (!groupViewModel) return result;
        result.state = ButtonWindowsState::kResolved;

        winrt::com_ptr<::IUnknown> taskGroupUnknown;
        if (FAILED(TaskListGroupViewModel_get_TaskGroup(groupViewModel, taskGroupUnknown.put_void())) || !taskGroupUnknown) return result;
        winrt::com_ptr<ITaskGroup> taskGroup;
        if (FAILED(taskGroupUnknown->QueryInterface(IID_ITaskGroup, taskGroup.put_void()))) return result;

        // An IVectorView<TaskItem>, which has the same ABI.
        winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Foundation::IInspectable> items;
        if (SUCCEEDED(taskGroup->get_Items(winrt::put_abi(items))) && items) {
            uint32_t count = items.Size();
            for (uint32_t i = 0; i < count; i++) {
                auto item = items.GetAt(i);
                if (HWND hWnd = GetTaskItemWindow(static_cast<::IUnknown*>(winrt::get_abi(item)))) result.windows.push_back(hWnd);
            }
        }
        return result;
    }
    return result;
}

// Whether one of a button's windows is the foreground one, which picks the
// active color. A minimized window can briefly stay foreground while it
// animates down; it doesn't count.
bool IsAnyWindowForeground(const std::vector<HWND>& windows) {
    ForegroundInfo fg = GetForegroundInfo();
    for (HWND hwnd : windows) {
        if (IsForegroundWindowOf(hwnd, fg) && !IsIconic(hwnd)) return true;
    }
    return false;
}

// The indicator a button should show: -1 leave as is (recycled container),
// 0 none, 1 open corners, 2 all four (maximized), 3 minimized underline,
// 4 dimmed underline (running off screen: cloaked, none minimized here).
// "Open" follows the window rules whether or not the corner marks are on, so
// the underline means the same thing in both cases: the button has a
// minimized window and isn't open. Buttons without any window
// (pinned apps that aren't running) get nothing.
// `isActive` receives whether the button is the foreground app's: -1
// unknown (recycled container), 0 no, 1 yes.
int EvaluateIndicatorState(const FrameworkElement& button, const Settings& localSettings, int* windowCount,
                           int* isActive) {
    auto resolved = ResolveButtonWindows(button);
    bool resolvedOk = resolved.state == ButtonWindowsState::kResolved;
    if (windowCount) *windowCount = resolvedOk ? (int)resolved.windows.size() : -1;
    if (isActive) *isActive = resolvedOk ? (IsAnyWindowForeground(resolved.windows) ? 1 : 0) : -1;
    if (resolved.state == ButtonWindowsState::kUnrealized) return -1;
    if (!resolvedOk) return 0;
    if (resolved.windows.empty()) return 0;
    if (!localSettings.IndEnabled && !localSettings.MinEnabled) return 0;

    if (localSettings.IndEnabled) {
        bool includeActive = localSettings.IndIncludeActive;
        if (localSettings.IndMaximized &&
            MatchesWindowRules(resolved.windows, localSettings, IsWindowShownMaximized, includeActive)) {
            return 2;
        }
        if (MatchesWindowRules(resolved.windows, localSettings, IsWindowShown, includeActive)) return 1;
    }
    // "Open" for the underline always counts the foreground window as shown:
    // that option only keeps the corner marks off the active button, it
    // doesn't make its window hidden. Otherwise an app with one window in
    // the foreground and another minimized would get the underline.
    if (localSettings.MinEnabled && !MatchesWindowRules(resolved.windows, localSettings, IsWindowShown, true)) {
        if (std::any_of(resolved.windows.begin(), resolved.windows.end(),
                        [&](HWND hwnd) { return IsWindowMinimizedHere(hwnd, localSettings); })) {
            return 3;
        }
        if (localSettings.MinHiddenEnabled && localSettings.IndCloakedIsHidden &&
            std::any_of(resolved.windows.begin(), resolved.windows.end(), IsWindowRunningOffScreen)) {
            return 4;
        }
    }
    return 0;
}

// Glues an element's Translation to the visual offset chain from `start` (the
// icon) up to its host panel with a composition ExpressionAnimation on both
// axes:
//   Translation = sum(chain position up to the host) + adj - self.Offset
// so the mark follows the icon through reordering, reflow, entrance
// animations and dock changes on the render thread. Each chain visual's
// position is its layout Offset plus the translation part of its
// TransformMatrix (a XAML RenderTransform) plus, when the element has the
// Translation facade enabled, its Translation. Returns false while the
// chain can't be resolved (element not fully in the tree yet).
bool BindToElement(
    winrt::Windows::UI::Xaml::UIElement const& pill,
    Panel const& host,
    winrt::Windows::UI::Xaml::FrameworkElement const& start,
    float adjX, float adjY)
{
    try {
        FrameworkElement gridElem = host;
        std::vector<winrt::Windows::UI::Composition::Visual> chain;
        FrameworkElement e = start;
        int depth = 0;
        while (e && e != gridElem && depth < 15) {
            chain.push_back(ElementCompositionPreview::GetElementVisual(e));
            auto parent = VisualTreeHelper::GetParent(e);
            e = parent ? parent.try_as<FrameworkElement>() : nullptr;
            depth++;
        }
        if (!e || e != gridElem || chain.empty()) return false;

        auto vis = ElementCompositionPreview::GetElementVisual(pill);

        std::wstring exprX, exprY;
        for (size_t i = 0; i < chain.size(); i++) {
            std::wstring pn = L"p" + std::to_wstring(i);
            exprX += pn + L".Offset.X + " + pn + L".TransformMatrix._41 + ";
            exprY += pn + L".Offset.Y + " + pn + L".TransformMatrix._42 + ";
            // "Translation" exists in the visual's property set only once
            // XAML has enabled the facade for that element; referencing a
            // missing property would invalidate the whole expression.
            winrt::Windows::Foundation::Numerics::float3 t{};
            bool hasTranslation = false;
            try {
                hasTranslation = chain[i].Properties().TryGetVector3(L"Translation", t) ==
                                 winrt::Windows::UI::Composition::CompositionGetValueStatus::Succeeded;
            } catch (...) {}
            if (hasTranslation) {
                exprX += pn + L".Translation.X + ";
                exprY += pn + L".Translation.Y + ";
            }
        }
        std::wstring expr = L"Vector3(" + exprX + L"adjX - self.Offset.X, " +
                            exprY + L"adjY - self.Offset.Y, 0.0f)";

        auto exp = vis.Compositor().CreateExpressionAnimation(winrt::hstring(expr));
        for (size_t i = 0; i < chain.size(); i++) {
            exp.SetReferenceParameter(winrt::hstring(L"p" + std::to_wstring(i)), chain[i]);
        }
        exp.SetScalarParameter(L"adjX", adjX);
        exp.SetScalarParameter(L"adjY", adjY);
        exp.SetReferenceParameter(L"self", vis);
        vis.Properties().StartAnimation(L"Translation", exp);
        return true;
    } catch (...) {
        return false;
    }
}

// ---------------------------------------------------------------------------
// Entries
// ---------------------------------------------------------------------------

void RefreshButton(FrameworkElement const& button, const Settings& localSettings);

// Property-changed callbacks queue a refresh rather than run one: the
// taskbar changes several properties in a row (IsIndeterminate, ShowPaused,
// ShowError, the value; a badge's visibility and source), and a refresh after
// each saw the states in between — progress gone for a moment (a lap
// restarted), or neither paused nor in error (the fill flashed its normal
// color). One queued refresh per burst runs after it, on the final state.
void ReportRefreshStats() {
    ULONGLONG now = GetTickCount64();
    ULONGLONG since = g_refreshStats.since.load();
    auto take = [](std::atomic<int>& v) { return v.exchange(0); };
    if (!since) {
        // Debug logging just turned on: start counting from now.
        for (auto* v : {&g_refreshStats.total, &g_refreshStats.hook, &g_refreshStats.posted,
                        &g_refreshStats.passes, &g_refreshStats.passButtons, &g_refreshStats.evForeground,
                        &g_refreshStats.evShowHide, &g_refreshStats.evMinimize, &g_refreshStats.evCloak,
                        &g_refreshStats.evMaximize}) {
            take(*v);
        }
        g_refreshStats.since = now;
        return;
    }
    if (now - since < 10000) return;
    g_refreshStats.since = now;
    int total = take(g_refreshStats.total), hook = take(g_refreshStats.hook), posted = take(g_refreshStats.posted);
    int passes = take(g_refreshStats.passes), passButtons = take(g_refreshStats.passButtons);
    int fg = take(g_refreshStats.evForeground), sh = take(g_refreshStats.evShowHide);
    int mn = take(g_refreshStats.evMinimize), cl = take(g_refreshStats.evCloak), mx = take(g_refreshStats.evMaximize);
    Wh_Log(L"[debug] refreshes in the last %llu s: %d (visual states %d, property/badge callbacks %d, "
           L"window-event passes %d covering %d buttons, other %d); window events: foreground %d, "
           L"show/hide %d, minimize/restore %d, cloak %d, maximize/restore %d",
           (now - since) / 1000, total, hook, posted, passes, passButtons,
           total - hook - posted - passButtons, fg, sh, mn, cl, mx);
}

void PostCoalescedRefresh(std::weak_ptr<ButtonEntry> const& weakEntry) {
    if (g_unloading) return;
    auto e = weakEntry.lock();
    if (!e || e->refreshPosted) return;
    auto btn = e->button.get();
    if (!btn) return;
    auto dispatcher = btn.Dispatcher();
    if (!dispatcher) return;
    e->refreshPosted = true;
    if (!PostToUiThread(dispatcher, [weakEntry]() {
            auto e = weakEntry.lock();
            if (!e) return;
            e->refreshPosted = false;
            if (g_unloading) return;
            auto btn = e->button.get();
            if (!btn) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            if (localSettings.DebugLog) g_refreshStats.posted++;
            try { RefreshButton(btn, localSettings); } catch (...) {}
        })) {
        e->refreshPosted = false;
    }
}

// The taskbar's own running indicator (the line under a running app's icon,
// Rectangle#RunningIndicator in the button template) and progress bar (the
// ProgressBar#ProgressIndicator that shows determinate progress and, with
// IsIndeterminate, the indeterminate one) are hidden through the opacity of
// their composition handout visuals. That opacity is composed with what XAML
// renders but never written by XAML, so the button's visual-state
// storyboards (active, inactive, attention, progress states, which animate
// the elements' own properties) can't bring them back, and the layout is
// untouched.
//
// Opacity alone doesn't stop what the element keeps animating, though: the
// progress bar's indeterminate state runs a storyboard that repeats forever
// and keeps Explorer rendering frames for a bar nobody sees. So while an
// element is hidden, the looping storyboards of its template's current
// visual states are paused, and resumed when it's shown again. When the bar
// changes state, its VisualStateManager stops the paused storyboard and
// begins the next one; the refresh that change triggers (the progress
// properties are watched) pauses that one in turn.
bool RepeatsForever(winrt::Windows::UI::Xaml::Media::Animation::Timeline const& timeline) {
    using winrt::Windows::UI::Xaml::Media::Animation::RepeatBehaviorType;
    if (timeline.RepeatBehavior().Type == RepeatBehaviorType::Forever) return true;
    if (auto board = timeline.try_as<winrt::Windows::UI::Xaml::Media::Animation::Storyboard>()) {
        for (auto const& child : board.Children()) {
            if (RepeatsForever(child)) return true;
        }
    }
    return false;
}

void SetLoopingAnimationsPaused(FrameworkElement const& element, bool paused) {
    if (VisualTreeHelper::GetChildrenCount(element) < 1) return;  // no template (a Rectangle)
    auto root = VisualTreeHelper::GetChild(element, 0).try_as<FrameworkElement>();
    if (!root) return;
    for (auto const& group : VisualStateManager::GetVisualStateGroups(root)) {
        auto state = group.CurrentState();
        auto board = state ? state.Storyboard() : nullptr;
        if (!board || !RepeatsForever(board)) continue;
        using winrt::Windows::UI::Xaml::Media::Animation::ClockState;
        if (board.GetCurrentState() != ClockState::Active) continue;
        // Pause and Resume are no-ops on a storyboard already in that state,
        // so this runs on every refresh without tracking which are paused.
        if (paused) board.Pause(); else board.Resume();
        // At most once every 10 s: this runs on every refresh of the button.
        static std::atomic<ULONGLONG> lastLog{0};
        ULONGLONG now = GetTickCount64();
        if (paused && g_debugLog && now - lastLog.load() >= 10000) {
            lastLog = now;
            Wh_Log(L"[debug] Pausing the looping animation of the hidden native %s (state %s)",
                   element.Name().c_str(), state.Name().c_str());
        }
    }
}

void SetNativeIndicatorHidden(FrameworkElement const& element, bool hidden) {
    try {
        ElementCompositionPreview::GetElementVisual(element).Opacity(hidden ? 0.0f : 1.0f);
    } catch (...) {}
    try { SetLoopingAnimationsPaused(element, hidden); } catch (...) {}
}

void RestoreNativeElement(ButtonEntry::HiddenNative& native) {
    if (!native.hidden) return;
    if (auto element = native.element.get()) SetNativeIndicatorHidden(element, false);
    native.hidden = false;
    native.element = nullptr;
}

std::wstring ThicknessText(winrt::Windows::UI::Xaml::Thickness const& t) {
    wchar_t buffer[96];
    swprintf_s(buffer, L"%g,%g,%g,%g", t.Left, t.Top, t.Right, t.Bottom);
    return buffer;
}

bool SameThickness(winrt::Windows::UI::Xaml::Thickness const& a, winrt::Windows::UI::Xaml::Thickness const& b) {
    return std::abs(a.Left - b.Left) < 0.01 && std::abs(a.Top - b.Top) < 0.01 &&
           std::abs(a.Right - b.Right) < 0.01 && std::abs(a.Bottom - b.Bottom) < 0.01;
}

// Puts the background's margin back. The taskbar's 5 px for the
// multi-window mark doesn't come from the element's local value (that one
// reads 0 while the margin shows 5), so writing the local value back pins
// the margin at 0. Clearing ours lets the taskbar's own value apply again;
// only if that doesn't give back the margin the taskbar last showed is the
// local value it had written back as well.
void RestoreBackgroundMargin(std::shared_ptr<ButtonEntry> const& e) {
    auto& m = e->backgroundMargin;
    if (m.applied) {
        if (auto element = m.element.get()) {
            try {
                auto before = element.Margin();
                element.ClearValue(FrameworkElement::MarginProperty());
                bool wroteLocal = false;
                if (m.hadLocal && !SameThickness(element.Margin(), m.base)) {
                    element.SetValue(FrameworkElement::MarginProperty(), m.originalLocal);
                    wroteLocal = true;
                }
                if (g_debugLog) {
                    Wh_Log(L"[debug] background margin restored (%s): %s -> %s (the taskbar's was %s)",
                           wroteLocal ? L"local value written back" : L"cleared", ThicknessText(before).c_str(),
                           ThicknessText(element.Margin()).c_str(), ThicknessText(m.base).c_str());
                }
            } catch (...) {}
        }
    }
    m = {};
}

// While the multi-window dot is on, the taskbar's own multi-window mark is
// hidden. The taskbar makes room for that mark by giving the button's
// background (Border#BackgroundElement) a right margin, which would leave
// the background off-center without it: its right margin is set to 0, as
// on a button with one window. Only on buttons the taskbar shows the mark
// on (it creates the element for buttons with several windows only). The
// margin the taskbar gives is re-read whenever it differs from the one set,
// so a visual state that changes the other sides is followed.
void UpdateBackgroundMargin(FrameworkElement const& button, std::shared_ptr<ButtonEntry> const& entry,
                            FrameworkElement const& iconPanel, const Settings& localSettings) {
    auto& m = entry->backgroundMargin;
    if (!localSettings.MultiEnabled && !m.applied) return;  // nothing to do, skip the lookups

    auto multi = FindChildByName(iconPanel, L"MultiWindowElement");
    bool want = localSettings.MultiEnabled && multi && multi.Visibility() == Visibility::Visible;
    auto background = FindChildByName(iconPanel, L"BackgroundElement");
    if (m.applied && m.element.get() != background) RestoreBackgroundMargin(entry);  // a new template
    if (!want || !background) {
        RestoreBackgroundMargin(entry);
        return;
    }

    auto current = background.Margin();
    if (!m.applied) {
        auto local = background.ReadLocalValue(FrameworkElement::MarginProperty());
        m.hadLocal = local != DependencyProperty::UnsetValue();
        m.originalLocal = m.hadLocal ? local : nullptr;
        m.base = current;
        m.element = winrt::make_weak(background);
        if (g_debugLog) {
            std::wstring localText = L"no local value";
            if (m.hadLocal) {
                if (auto boxed = local.try_as<winrt::Windows::Foundation::IReference<winrt::Windows::UI::Xaml::Thickness>>()) {
                    localText = L"local value " + ThicknessText(boxed.Value());
                } else {
                    localText = L"local value of another kind";
                }
            }
            Wh_Log(L"[debug] background margin: %s (%s)", ThicknessText(current).c_str(), localText.c_str());
        }
    } else if (!SameThickness(current, m.value)) {
        if (g_debugLog) {
            Wh_Log(L"[debug] background margin changed by the taskbar: %s", ThicknessText(current).c_str());
        }
        m.base = current;  // the taskbar changed it since
    }
    auto target = m.base;
    target.Right = 0.0;
    if (!SameThickness(current, target)) background.Margin(target);
    m.value = target;
    m.applied = true;
}

void RestoreNativeIndicators(std::shared_ptr<ButtonEntry> const& e) {
    RestoreNativeElement(e->nativeRunning);
    RestoreNativeElement(e->nativeProgress);
    RestoreNativeElement(e->nativeMulti);
    RestoreBackgroundMargin(e);
}

// Keeps one native element hidden while `hide` is on, restored otherwise.
// Re-resolved every pass: a re-applied template replaces the element.
//
// While `hide` is on, nothing is ever shown again: an element the lookup
// misses (out of the tree for a moment, as during a window's close) or that
// a new instance replaced simply stays hidden. Showing it in those cases is
// what flashed the native running indicator on close. Only turning the
// option off (here, or by the sweep in RestoreNativeElements) or unloading
// the mod shows the elements again.
void UpdateHiddenNative(FrameworkElement const& button, FrameworkElement const& iconPanel,
                        ButtonEntry::HiddenNative& native, bool hide, std::wstring_view name,
                        std::wstring_view classPart) {
    if (!hide && !native.hidden) return; // nothing to do, skip the lookup

    auto found = FindChildByName(iconPanel, name);
    if (!found && iconPanel != button) found = FindChildByName(button, name);
    if (!found && !classPart.empty()) found = FindChildByClassPart(button, classPart);

    if (hide) {
        if (!found) return; // keep tracking what we have, hidden
        SetNativeIndicatorHidden(found, true);
        native.element = winrt::make_weak(found);
        native.hidden = true;
    } else {
        RestoreNativeElement(native);
        if (found) SetNativeIndicatorHidden(found, false);
    }
}

void UpdateNativeIndicators(FrameworkElement const& button, std::shared_ptr<ButtonEntry> const& entry,
                            FrameworkElement const& iconPanel, const Settings& localSettings) {
    UpdateHiddenNative(button, iconPanel, entry->nativeRunning, localSettings.HideNativeIndicator,
                       L"RunningIndicator", L"");
    UpdateHiddenNative(button, iconPanel, entry->nativeProgress, localSettings.HideNativeProgress,
                       L"ProgressIndicator", L"ProgressBar");
    // The taskbar's own mark for a button with several windows
    // (Border#MultiWindowElement) gives way to the multi-window dot.
    UpdateHiddenNative(button, iconPanel, entry->nativeMulti, localSettings.MultiEnabled,
                       L"MultiWindowElement", L"");
    try { UpdateBackgroundMargin(button, entry, iconPanel, localSettings); } catch (...) {}
}

// ---------------------------------------------------------------------------
// Native badges. The button template shows a notification badge (counts,
// glyphs; with small taskbar icons a dot instead, Taskbar.Beacon#
// BeaconControl) and the window's overlay icon (Image#OverlayIcon) on top of
// the icon. With badge support on, every such element is found in the icon's
// panel — by name or class containing "Badge" or "Beacon", plus OverlayIcon —
// and hidden through its handout visual's opacity, like the running
// indicator. Whether the button "has a badge" is read from the elements' own
// XAML state, which that opacity doesn't touch: shown, sized and, for an
// image, with a source.
// Property-changed callbacks on those properties, plus SizeChanged, refresh
// the button when a badge comes or goes.
// ---------------------------------------------------------------------------

bool ContainsNoCase(std::wstring_view haystack, std::wstring_view needle) {
    if (needle.empty() || haystack.size() < needle.size()) return false;
    auto it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                          [](wchar_t a, wchar_t b) { return std::towlower(a) == std::towlower(b); });
    return it != haystack.end();
}

void CollectBadgeElements(FrameworkElement const& root, std::vector<FrameworkElement>& out, int depth = 0) {
    if (!root || depth > 4) return;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        auto name = child.Name();
        if (name == kHostName) continue;  // ours
        // Badge (a count or glyph), Beacon (the dot that replaces it with
        // small taskbar icons) or the window's overlay icon.
        auto className = winrt::get_class_name(child);
        if (name == L"OverlayIcon" || ContainsNoCase(name, L"Badge") || ContainsNoCase(className, L"Badge") ||
            ContainsNoCase(name, L"Beacon") || ContainsNoCase(className, L"Beacon")) {
            out.push_back(child);
            continue; // the whole badge, not its parts
        }
        CollectBadgeElements(child, out, depth + 1);
    }
}

// Whether a parent up to (not including) the button hides the element:
// collapsed or transparent.
bool IsHiddenByParent(FrameworkElement const& element) {
    try {
        auto parent = VisualTreeHelper::GetParent(element);
        for (int depth = 0; parent && depth < 8; depth++) {
            auto ui = parent.try_as<UIElement>();
            if (!ui) break;
            if (winrt::get_class_name(ui) == L"Taskbar.TaskListButton") break;
            if (ui.Visibility() != Visibility::Visible || ui.Opacity() < 0.01) return true;
            parent = VisualTreeHelper::GetParent(parent);
        }
    } catch (...) {}
    return false;
}

// The innermost elements of a badge (its count, glyph or image).
void CollectLeaves(FrameworkElement const& root, std::vector<FrameworkElement>& out, int depth = 0) {
    if (!root || depth > 5) return;
    int count = VisualTreeHelper::GetChildrenCount(root);
    if (count == 0) {
        if (depth > 0) out.push_back(root);
        return;
    }
    for (int i = 0; i < count; i++) {
        if (auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>()) {
            CollectLeaves(child, out, depth + 1);
        }
    }
}

// Whether a leaf shows something: visible down from the badge, sized, and,
// for text, not empty.
bool IsLeafShowing(FrameworkElement const& leaf, FrameworkElement const& badgeRoot) {
    try {
        if (leaf.ActualWidth() < 0.5 || leaf.ActualHeight() < 0.5) return false;
        if (auto text = leaf.try_as<TextBlock>()) {
            if (text.Text().empty()) return false;
        }
        for (DependencyObject node = leaf; node; node = VisualTreeHelper::GetParent(node)) {
            auto ui = node.try_as<UIElement>();
            if (!ui) break;
            if (ui.Visibility() != Visibility::Visible || ui.Opacity() < 0.01) return false;
            if (node == badgeRoot) break;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool IsNativeBadgeShown(ButtonEntry::NativeBadge const& badge) {
    auto element = badge.element.get();
    if (!element) return false;
    try {
        if (element.Visibility() != Visibility::Visible) return false;
        if (element.Opacity() < 0.01) return false;
        if (element.ActualWidth() < 0.5 || element.ActualHeight() < 0.5) return false;
        if (badge.isImage) {
            auto image = element.try_as<Image>();
            if (image && !image.Source()) return false;
        }
        if (IsHiddenByParent(element)) return false;
        // A badge control shows something only through its inner elements:
        // the control itself can stay visible with nothing in it.
        if (!badge.isImage) {
            std::vector<FrameworkElement> leaves;
            CollectLeaves(element, leaves);
            if (!leaves.empty() &&
                std::none_of(leaves.begin(), leaves.end(), [&](auto const& leaf) { return IsLeafShowing(leaf, element); })) {
                return false;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

// The badge elements' XAML state, for the debug log.
std::wstring NativeBadgeDetail(std::shared_ptr<ButtonEntry> const& entry) {
    std::wstring detail;
    for (auto const& badge : entry->nativeBadges) {
        auto element = badge.element.get();
        if (!element) continue;
        try {
            wchar_t buffer[160];
            bool source = true;
            if (badge.isImage) {
                auto image = element.try_as<Image>();
                source = image && image.Source();
            }
            swprintf_s(buffer, L" [%s vis=%s op=%.2f %gx%g src=%s parent=%s", element.Name().c_str(),
                       element.Visibility() == Visibility::Visible ? L"y" : L"n", element.Opacity(),
                       element.ActualWidth(), element.ActualHeight(), source ? L"y" : L"n",
                       IsHiddenByParent(element) ? L"hides" : L"ok");
            detail += buffer;
            if (!badge.isImage) {
                std::vector<FrameworkElement> leaves;
                CollectLeaves(element, leaves);
                for (auto const& leaf : leaves) {
                    std::wstring className(winrt::get_class_name(leaf));
                    size_t dot = className.find_last_of(L'.');
                    std::wstring text;
                    if (auto block = leaf.try_as<TextBlock>()) text = L" \"" + std::wstring(block.Text()) + L"\"";
                    swprintf_s(buffer, L" {%s %gx%g%s %s}", className.substr(dot == std::wstring::npos ? 0 : dot + 1).c_str(),
                               leaf.ActualWidth(), leaf.ActualHeight(), text.c_str(),
                               IsLeafShowing(leaf, element) ? L"shows" : L"empty");
                    detail += buffer;
                }
            }
            detail += L"]";
        } catch (...) {}
    }
    return detail;
}

// Unregisters a badge element's callbacks and, with `restore`, shows it
// again (see RetireEntryPaths for when it is left hidden).
void ReleaseNativeBadge(ButtonEntry::NativeBadge const& badge, bool restore = true) {
    for (auto const& watch : badge.treeWatches) {
        if (auto object = watch.object.get()) {
            try { object.UnregisterPropertyChangedCallback(watch.property, watch.token); } catch (...) {}
        }
    }
    auto element = badge.element.get();
    if (!element) return;
    try {
        if (badge.visibilityToken) element.UnregisterPropertyChangedCallback(UIElement::VisibilityProperty(), badge.visibilityToken);
        if (badge.opacityToken) element.UnregisterPropertyChangedCallback(UIElement::OpacityProperty(), badge.opacityToken);
        if (badge.sourceToken) element.UnregisterPropertyChangedCallback(Image::SourceProperty(), badge.sourceToken);
        if (badge.sizeAttached) element.SizeChanged(badge.sizeToken);
    } catch (...) {}
    if (restore) SetNativeIndicatorHidden(element, false);
}

// Watches what can hide or change a badge without touching the badge
// element itself: its parents up to the button (visibility, opacity), and
// its inner elements (visibility, opacity, text).
template <typename Callback>
void WatchBadgeTree(ButtonEntry::NativeBadge& badge, FrameworkElement const& element, Callback const& onChanged) {
    auto watch = [&](DependencyObject const& object, DependencyProperty const& property) {
        try {
            int64_t token = object.RegisterPropertyChangedCallback(property, onChanged);
            badge.treeWatches.push_back({winrt::make_weak(object), property, token});
        } catch (...) {}
    };
    try {
        auto parent = VisualTreeHelper::GetParent(element);
        for (int depth = 0; parent && depth < 8; depth++) {
            auto ui = parent.try_as<UIElement>();
            if (!ui || winrt::get_class_name(ui) == L"Taskbar.TaskListButton") break;
            watch(ui, UIElement::VisibilityProperty());
            watch(ui, UIElement::OpacityProperty());
            parent = VisualTreeHelper::GetParent(parent);
        }
        std::function<void(FrameworkElement const&, int)> down = [&](FrameworkElement const& node, int depth) {
            if (depth > 5) return;
            int count = VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(node, i).try_as<FrameworkElement>();
                if (!child) continue;
                watch(child, UIElement::VisibilityProperty());
                watch(child, UIElement::OpacityProperty());
                if (child.try_as<TextBlock>()) watch(child, TextBlock::TextProperty());
                down(child, depth + 1);
            }
        };
        down(element, 0);
    } catch (...) {}
}

void ReleaseNativeBadges(std::shared_ptr<ButtonEntry> const& e, bool restore = true) {
    for (auto& badge : e->nativeBadges) ReleaseNativeBadge(badge, restore);
    e->nativeBadges.clear();
}


// Keeps the entry's native badge list in sync with the template, hides them,
// and returns whether any of them is currently showing a badge.
bool UpdateNativeBadges(std::shared_ptr<ButtonEntry> const& entry, FrameworkElement const& iconPanel,
                        const Settings& localSettings) {
    if (!localSettings.BadgeEnabled) {
        ReleaseNativeBadges(entry);
        return false;
    }

    std::vector<FrameworkElement> found;
    CollectBadgeElements(iconPanel, found);

    auto isFound = [&](FrameworkElement const& element) {
        return std::any_of(found.begin(), found.end(), [&](auto const& f) { return f == element; });
    };
    // Drop elements that left the template (re-applied, recycled), still
    // hidden: one that comes back must not show for a frame before it is
    // found again (see UpdateHiddenNative).
    auto& list = entry->nativeBadges;
    for (auto it = list.begin(); it != list.end(); ) {
        auto element = it->element.get();
        if (!element || !isFound(element)) {
            ReleaseNativeBadge(*it, false);
            it = list.erase(it);
        } else {
            ++it;
        }
    }
    // Adopt new ones.
    for (auto& element : found) {
        bool known = std::any_of(list.begin(), list.end(), [&](auto const& b) { return b.element.get() == element; });
        if (known) continue;

        ButtonEntry::NativeBadge badge;
        badge.element = winrt::make_weak(element);
        badge.isImage = element.try_as<Image>() != nullptr;
        std::weak_ptr<ButtonEntry> weakEntry = entry;
        auto onChanged = [weakEntry](DependencyObject const&, DependencyProperty const&) {
            PostCoalescedRefresh(weakEntry);
        };
        try {
            badge.visibilityToken = element.RegisterPropertyChangedCallback(UIElement::VisibilityProperty(), onChanged);
            badge.opacityToken = element.RegisterPropertyChangedCallback(UIElement::OpacityProperty(), onChanged);
            if (badge.isImage) badge.sourceToken = element.RegisterPropertyChangedCallback(Image::SourceProperty(), onChanged);
            // A badge that just turned visible has no size until the next
            // layout pass, so the Visibility callback alone sees it as not
            // shown; its first SizeChanged completes the picture.
            badge.sizeToken = element.SizeChanged([onChanged](auto const&, auto const&) {
                onChanged(nullptr, nullptr);
            });
            badge.sizeAttached = true;
        } catch (...) {}
        WatchBadgeTree(badge, element, onChanged);
        {
            static std::atomic<int> logged{0};
            if (logged.fetch_add(1) < 4) {
                Wh_Log(L"Native badge element: %s#%s", winrt::get_class_name(element).c_str(), element.Name().c_str());
            }
        }
        list.push_back(std::move(badge));
    }

    bool shown = false;
    for (auto& badge : list) {
        if (auto element = badge.element.get()) SetNativeIndicatorHidden(element, true);
        if (IsNativeBadgeShown(badge)) shown = true;
    }
    return shown;
}

// ---------------------------------------------------------------------------
// Indeterminate progress. The taskbar's own progress bar (found like the one
// hidden by HideNativeProgress) says it: shown (by its XAML state, which the
// hiding doesn't touch), IsIndeterminate, and neither paused nor in error.
// Property-changed callbacks on those properties refresh the button when it
// starts or stops.
// ---------------------------------------------------------------------------

FrameworkElement FindNativeProgress(FrameworkElement const& button, FrameworkElement const& iconPanel) {
    auto found = FindChildByName(iconPanel, L"ProgressIndicator");
    if (!found && iconPanel != button) found = FindChildByName(button, L"ProgressIndicator");
    if (!found) found = FindChildByClassPart(button, L"ProgressBar");
    return found;
}

// What the native progress bar shows.
struct ProgressInfo {
    bool indeterminate = false;  // moving, neither paused nor in error
    bool determinate = false;    // a value
    double fraction = 0.0;       // of the determinate value, 0 to 1
    bool paused = false;
    bool error = false;
};

ProgressInfo ReadProgress(FrameworkElement const& element) {
    ProgressInfo info;
    try {
        if (element.Visibility() != Visibility::Visible || element.Opacity() < 0.01) return info;
        bool isIndeterminate = false;
        if (auto bar = element.try_as<winrt::Microsoft::UI::Xaml::Controls::ProgressBar>()) {
            isIndeterminate = bar.IsIndeterminate();
            info.paused = bar.ShowPaused();
            info.error = bar.ShowError();
        } else if (auto bar = element.try_as<winrt::Windows::UI::Xaml::Controls::ProgressBar>()) {
            isIndeterminate = bar.IsIndeterminate();
            info.paused = bar.ShowPaused();
            info.error = bar.ShowError();
        } else {
            return info;
        }
        info.indeterminate = isIndeterminate && !info.paused && !info.error;
        if (!isIndeterminate) {
            if (auto range = element.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase>()) {
                double minimum = range.Minimum(), maximum = range.Maximum();
                if (maximum > minimum) {
                    info.determinate = true;
                    info.fraction = std::clamp((range.Value() - minimum) / (maximum - minimum), 0.0, 1.0);
                }
            }
        }
    } catch (...) {}
    return info;
}

void ReleaseProgressWatch(std::shared_ptr<ButtonEntry> const& e) {
    auto& watch = e->progressWatch;
    if (auto element = watch.element.get()) {
        for (auto& [property, token] : watch.tokens) {
            try { element.UnregisterPropertyChangedCallback(property, token); } catch (...) {}
        }
    }
    watch = {};
}

void ChaseFill(IndicatorRenderer& r, double fraction, bool animate);
bool SystemAnimationsEnabled();

// Keeps the watch on the button's progress bar and returns what it shows.
ProgressInfo UpdateProgressWatch(FrameworkElement const& button, std::shared_ptr<ButtonEntry> const& entry,
                                 FrameworkElement const& iconPanel, const Settings& localSettings) {
    if (!localSettings.ProgressAnimation && !localSettings.ProgressFill) {
        ReleaseProgressWatch(entry);
        return {};
    }
    auto found = FindNativeProgress(button, iconPanel);
    if (!found) {
        ReleaseProgressWatch(entry);
        return {};
    }

    auto& watch = entry->progressWatch;
    if (watch.element.get() != found) {
        ReleaseProgressWatch(entry);
        watch.element = winrt::make_weak(found);
        std::weak_ptr<ButtonEntry> weakEntry = entry;
        auto onChanged = [weakEntry](DependencyObject const&, DependencyProperty const&) {
            PostCoalescedRefresh(weakEntry);
        };
        std::vector<DependencyProperty> properties = {UIElement::VisibilityProperty(), UIElement::OpacityProperty()};
        if (found.try_as<winrt::Microsoft::UI::Xaml::Controls::ProgressBar>()) {
            using MuxProgressBar = winrt::Microsoft::UI::Xaml::Controls::ProgressBar;
            properties.push_back(MuxProgressBar::IsIndeterminateProperty());
            properties.push_back(MuxProgressBar::ShowPausedProperty());
            properties.push_back(MuxProgressBar::ShowErrorProperty());
        } else if (found.try_as<winrt::Windows::UI::Xaml::Controls::ProgressBar>()) {
            using WuxProgressBar = winrt::Windows::UI::Xaml::Controls::ProgressBar;
            properties.push_back(WuxProgressBar::IsIndeterminateProperty());
            properties.push_back(WuxProgressBar::ShowPausedProperty());
            properties.push_back(WuxProgressBar::ShowErrorProperty());
        }
        for (auto& property : properties) {
            try {
                watch.tokens.emplace_back(property, found.RegisterPropertyChangedCallback(property, onChanged));
            } catch (...) {}
        }
        // The value changes all the time: it only moves the fill, without a
        // refresh of the button (a refresh only when the fill isn't running).
        if (found.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase>()) {
            using RangeBase = winrt::Windows::UI::Xaml::Controls::Primitives::RangeBase;
            auto onValue = [weakEntry, onChanged](DependencyObject const& sender, DependencyProperty const& property) {
                if (g_unloading) return;
                auto e = weakEntry.lock();
                if (!e) return;
                auto& r = e->renderer;
                if (!r.fillActive || !r.fillProps) {
                    onChanged(sender, property);
                    return;
                }
                auto element = sender.try_as<FrameworkElement>();
                if (!element) return;
                auto info = ReadProgress(element);
                if (!info.determinate) {
                    onChanged(sender, property);
                    return;
                }
                bool animate;
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    animate = g_settings.Animations;
                }
                try { ChaseFill(r, info.fraction, animate && SystemAnimationsEnabled()); } catch (...) {}
            };
            for (auto property : {RangeBase::ValueProperty(), RangeBase::MinimumProperty(), RangeBase::MaximumProperty()}) {
                try {
                    watch.tokens.emplace_back(property, found.RegisterPropertyChangedCallback(property, onValue));
                } catch (...) {}
            }
        }
        static std::atomic<bool> logged;
        if (!logged.exchange(true)) {
            Wh_Log(L"Native progress element: %s#%s", winrt::get_class_name(found).c_str(), found.Name().c_str());
        }
    }
    return ReadProgress(found);
}

// Badges and overlay icons appear and go as whole elements added to and
// removed from the icon panel, which no property change of an element we
// watch reports. A LayoutUpdated handler on the panel (raised after every
// layout pass, so it only compares one number) notices a change in its
// children: it hides a new native badge at once, before the frame is drawn,
// and queues a refresh for the badge dot.
// Debug logging: a panel's children in order, each as Name (or class) and
// its ZIndex, e.g. "Icon BadgeControl ButtonStateIndicatorHost/z1000".
std::wstring PanelOrderText(FrameworkElement const& panel) {
    std::wstring text;
    try {
        int count = VisualTreeHelper::GetChildrenCount(panel);
        for (int i = 0; i < count; i++) {
            auto child = VisualTreeHelper::GetChild(panel, i).try_as<FrameworkElement>();
            if (!child) continue;
            std::wstring label(child.Name());
            if (label.empty()) {
                label = winrt::get_class_name(child);
                size_t dot = label.find_last_of(L'.');
                if (dot != std::wstring::npos) label = label.substr(dot + 1);
            }
            int z = Canvas::GetZIndex(child);
            if (z != 0) label += L"/z" + std::to_wstring(z);
            if (child.Visibility() != Visibility::Visible) label += L"/collapsed";
            if (!text.empty()) text += L" ";
            text += label;
        }
    } catch (...) {}
    return text;
}

// The panel's children other than the mod's own host: the host coming or
// going (added on the first pass, removed on unload) is no badge.
int CountForeignChildren(FrameworkElement const& panel) {
    int count = VisualTreeHelper::GetChildrenCount(panel), foreign = 0;
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(panel, i).try_as<FrameworkElement>();
        if (!child || child.Name() != kHostName) foreign++;
    }
    return foreign;
}

void UnwatchIconPanel(std::shared_ptr<ButtonEntry> const& e) {
    if (e->layoutAttached) {
        if (auto panel = e->layoutPanel.get()) {
            try { panel.LayoutUpdated(e->layoutToken); } catch (...) {}
        }
    }
    e->layoutAttached = false;
    e->layoutPanel = nullptr;
    e->layoutChildCount = -1;
    e->layoutForeignCount = -1;
}

void WatchIconPanel(std::shared_ptr<ButtonEntry> const& entry, FrameworkElement const& iconPanel,
                    const Settings& localSettings) {
    if (!localSettings.BadgeEnabled) {
        UnwatchIconPanel(entry);
        return;
    }
    if (entry->layoutAttached && entry->layoutPanel.get() == iconPanel) return;
    UnwatchIconPanel(entry);

    entry->layoutPanel = winrt::make_weak(iconPanel);
    entry->layoutChildCount = VisualTreeHelper::GetChildrenCount(iconPanel);
    entry->layoutForeignCount = CountForeignChildren(iconPanel);
    std::weak_ptr<ButtonEntry> weakEntry = entry;
    entry->layoutToken = iconPanel.LayoutUpdated([weakEntry](auto const&, auto const&) {
        if (g_unloading) return;
        auto e = weakEntry.lock();
        if (!e) return;
        auto panel = e->layoutPanel.get();
        if (!panel) return;
        int count = VisualTreeHelper::GetChildrenCount(panel);
        if (count == e->layoutChildCount) return;  // the usual case: one number compared
        e->layoutChildCount = count;
        int foreign = CountForeignChildren(panel);
        if (foreign == e->layoutForeignCount) return;  // only the mod's own host came or went
        Settings localSettings;
        { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
        if (localSettings.DebugLog) {
            Wh_Log(L"[debug] %s icon panel: %d -> %d children [%s]", e->debugApp.empty() ? L"?" : e->debugApp.c_str(),
                   e->layoutForeignCount, foreign, PanelOrderText(panel).c_str());
        }
        e->layoutForeignCount = foreign;
        if (!localSettings.BadgeEnabled) return;
        try { UpdateNativeBadges(e, panel, localSettings); } catch (...) {}
        PostCoalescedRefresh(weakEntry);
    });
    entry->layoutAttached = true;
}

void RetireHost(std::shared_ptr<ButtonEntry> const& entry);
void UnwatchViewModelForDebug(std::shared_ptr<ButtonEntry> const& e);

// Retires the indicators of an entry (their host and composition content)
// and revokes the icon subscription (the button subscriptions are revoked by
// the callers that still have the button). The native elements this mod
// hides are given back, unless `keepNativeHidden`: on a button's Unloaded
// they stay hidden, since the taskbar keeps an unloaded button for its
// closing animation or reuses it for another item, and showing them there
// flashed the native running indicator on every close. A reused button
// gets a new entry, which takes them over; what is left behind is shown
// again by a sweep when the option is turned off or the mod unloads
// (RestoreNativeElements).
void RetireEntryPaths(std::shared_ptr<ButtonEntry> const& e, bool keepNativeHidden = false) {
    if (keepNativeHidden) {
        e->nativeRunning = {};
        e->nativeProgress = {};
        e->nativeMulti = {};
        // The margin goes back even here: a reused button's new entry would
        // otherwise read the adjusted margin as the taskbar's.
        RestoreBackgroundMargin(e);
        ReleaseNativeBadges(e, false);
    } else {
        RestoreNativeIndicators(e);
        ReleaseNativeBadges(e);
    }
    ReleaseProgressWatch(e);
    UnwatchIconPanel(e);
    UnwatchViewModelForDebug(e);
    RetireHost(e);
    if (e->iconSizeAttached) {
        if (auto icon = e->icon.get()) { try { icon.SizeChanged(e->iconSizeToken); } catch (...) {} }
    }
}

std::shared_ptr<ButtonEntry> FindOrCreateEntry(FrameworkElement const& button, bool createIfMissing = true) {
    std::vector<std::shared_ptr<ButtonEntry>> orphans;
    std::shared_ptr<ButtonEntry> result;
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        if (g_unloading || !g_entries) return nullptr;

        // Prune entries whose button died without an Unloaded (rare). Their
        // paths live in the icon's panel, which usually died with them;
        // retire whatever is left.
        for (auto it = g_entries->begin(); it != g_entries->end(); ) {
            auto btn = (*it)->button.get();
            if (!btn) {
                orphans.push_back(*it);
                it = g_entries->erase(it);
                continue;
            }
            if (btn == button) result = *it;
            ++it;
        }
        if (!result && createIfMissing) {
            result = std::make_shared<ButtonEntry>();
            result->button = winrt::make_weak(button);
            g_entries->push_back(result);
        }
    }

    // Their host lives in the icon's panel, which usually died with them;
    // retire whatever is left, on its own thread. Without a host, only the
    // composition objects and the subscriptions outside the button are left.
    for (auto& orphan : orphans) {
        auto host = orphan->host.get();
        auto dispatcher = host ? host.Dispatcher() : nullptr;
        // Fully: the debug view-model subscription lives on the view
        // model, which can outlive the button (a recycled container), and
        // the unload cleanup no longer reaches a pruned entry. Everything
        // else RetireEntryPaths revokes lives inside the button and is
        // skipped once it's gone.
        if (!dispatcher || dispatcher.HasThreadAccess()) {
            RetireEntryPaths(orphan, true);
        } else {
            PostToUiThread(dispatcher, [orphan]() { RetireEntryPaths(orphan, true); });
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// The indicators of one button: composition shapes on the frame path, under
// one small XAML host in the icon's parent panel (the IconPanel). The host
// has a high ZIndex, so they render above the icon and its badge, and is
// glued to the icon with a composition expression. Every shape is trimmed to
// its piece of the frame path; animating TrimStart and TrimEnd moves those
// pieces along the path, around the corners, on the render thread.
// Called from every refresh of a button (hook, sweep, window-event pass,
// Loaded, theme change).
// ---------------------------------------------------------------------------


// Drops the composition content: the host's child visual first, then every
// object closed, so the frame paths (and the geometry sources of this mod
// they hold) are released right away.
void ReleaseRenderer(std::shared_ptr<ButtonEntry> const& entry) {
    auto& r = entry->renderer;
    if (auto host = entry->host.get()) {
        try { ElementCompositionPreview::SetElementChildVisual(host, nullptr); } catch (...) {}
    }
    for (auto& slot : r.slots) {
        try { if (slot.shape) slot.shape.Close(); } catch (...) {}
        try { if (slot.geometry) slot.geometry.Close(); } catch (...) {}
        try { if (slot.colorBrush) slot.colorBrush.Close(); } catch (...) {}
        try { if (slot.gradientBrush) slot.gradientBrush.Close(); } catch (...) {}
        for (auto& stop : slot.stops) { try { stop.Close(); } catch (...) {} }
        slot = IndicatorSlot{};
    }
    try { if (r.visual) r.visual.Close(); } catch (...) {}
    try { if (r.easing) r.easing.Close(); } catch (...) {}
    try { if (r.linear) r.linear.Close(); } catch (...) {}
    try { if (r.progress) r.progress.Close(); } catch (...) {}
    for (auto& twin : r.fills) {
        try { if (twin.shape) twin.shape.Close(); } catch (...) {}
        try { if (twin.geometry) twin.geometry.Close(); } catch (...) {}
        try { if (twin.brush) twin.brush.Close(); } catch (...) {}
    }
    try { if (r.fillProps) r.fillProps.Close(); } catch (...) {}
    try { if (r.progressEasing) r.progressEasing.Close(); } catch (...) {}
    try { if (r.stepEasing) r.stepEasing.Close(); } catch (...) {}
    r = IndicatorRenderer{};
}

void RetireHost(std::shared_ptr<ButtonEntry> const& entry) {
    ReleaseRenderer(entry);
    if (auto host = entry->host.get()) RetireElement(host);
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        entry->host = nullptr;
    }
    entry->hostBound = false;
}

// The host in `panel`: the entry's own, a retired one adopted from an
// earlier session or a recycled container, or a new one. Null when the icon
// moved to another panel: the old host is retired and the next pass starts
// over in the new one.
FrameworkElement EnsureHost(std::shared_ptr<ButtonEntry> const& entry, Panel const& panel) {
    auto host = entry->host.get();
    if (host) {
        auto parent = VisualTreeHelper::GetParent(host);
        auto parentPanel = parent ? parent.try_as<Panel>() : nullptr;
        if (parentPanel != panel) {
            RetireHost(entry);
            return nullptr;
        }
        return host;
    }

    host = FindRetiredElement(panel, kHostName);
    bool adopted = host != nullptr;
    if (!adopted) {
        Canvas canvas;
        canvas.Name(kHostName);
        host = canvas;
    }
    host.IsHitTestVisible(false);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Top);
    host.Width(1.0);
    host.Height(1.0);
    // Layout-neutral: its desired size is 0 whatever the panel offers.
    host.Margin(winrt::Windows::UI::Xaml::ThicknessHelper::FromLengths(0, 0, -1000, -1000));
    host.Opacity(0.0);
    host.Visibility(Visibility::Visible);
    // Above everything in the panel — the icon, its badge, the label and
    // whatever a theme adds — whatever ZIndex those carry. (The panel sorts
    // by ZIndex, then by child order; the child order alone only decides
    // among ZIndex-0 siblings.)
    Canvas::SetZIndex(host, 1000);
    // Inserted first, not appended: the taskbar inserts a badge, the
    // small-icons dot or an overlay icon at a position counted from the end
    // of the panel's children, so a host at the end shifted it - in front
    // of the icon, behind which it then showed once the mod was off. In
    // front of every native child, the host shifts nothing that is counted
    // from the end or from a sibling, and its ZIndex still draws it on top.
    if (!adopted) panel.Children().InsertAt(0, host);
    ElementCompositionPreview::SetIsTranslationEnabled(host, true);
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        entry->host = winrt::make_weak(host);
    }
    entry->hostBound = false;
    return host;
}

// ---------------------------------------------------------------------------
// Determinate progress: the fill. The corners form two tracks, both filled
// left to right at the same fraction: the bottom one (BL, then BR — the
// underline, the cup or the two bottom corners), which along the path is
// counterclockwise, from each piece's end to its start; and the top one
// (TL, then TR — the cap or the two top corners), clockwise, from each
// piece's start to its end. Each piece's twin covers its part of the filled
// length: the first piece fills up before the second starts. Lengths are
// read live from the base shapes' trims, so the fill follows every movement.
// ---------------------------------------------------------------------------

// A twin's track: which pieces it fills after one another, and whether it
// fills along the path (start to end) or against it. Left to right, the
// bottom track is BL then BR against the path, the top one TL then TR along
// it; right to left, BR then BL along it, and TR then TL against it.
void FillTrackOf(int k, bool rtl, int& first, int& second, bool& forward) {
    bool top = k == kSlotTL || k == kSlotTR;
    if (!rtl) {
        first = top ? kSlotTL : kSlotBL;
        second = top ? kSlotTR : kSlotBR;
        forward = top;
    } else {
        first = top ? kSlotTR : kSlotBR;
        second = top ? kSlotTL : kSlotBL;
        forward = !top;
    }
}

std::wstring FillPortionExpression(int k, int first, int second) {
    const std::wstring len1 = L"(b1.TrimEnd - b1.TrimStart)";
    const std::wstring len2 = L"(b2.TrimEnd - b2.TrimStart)";
    // Each track fills only while its corners are marks, not dimmed ones.
    const std::wstring track = (k == kSlotTL || k == kSlotTR) ? L"props.top" : L"props.bottom";
    const std::wstring filled = L"(props.f * " + track + L" * (" + len1 + L" + " + len2 + L"))";
    return k == first ? L"Clamp(" + filled + L", 0, " + len1 + L")"
                      : L"Clamp(" + filled + L" - " + len1 + L", 0, " + len2 + L")";
}

void BindFillTwins(IndicatorRenderer& r, bool rtl) {
    if (!r.fillProps) {
        r.fillProps = r.compositor.CreatePropertySet();
        r.fillProps.InsertScalar(L"f", 0.0f);
        r.fillProps.InsertScalar(L"alpha", 0.0f);
        r.fillProps.InsertScalar(L"top", 1.0f);
        r.fillProps.InsertScalar(L"bottom", 1.0f);
    }
    for (int k = 0; k < 4; k++) {
        int first, second;
        bool forward;
        FillTrackOf(k, rtl, first, second, forward);
        const std::wstring portion = FillPortionExpression(k, first, second);

        auto& twin = r.fills[k];
        auto bindTrim = [&](wchar_t const* property, std::wstring const& text) {
            auto expression = r.compositor.CreateExpressionAnimation(winrt::hstring(text));
            expression.SetReferenceParameter(L"b1", r.slots[first].geometry);
            expression.SetReferenceParameter(L"b2", r.slots[second].geometry);
            expression.SetReferenceParameter(L"bk", r.slots[k].geometry);
            expression.SetReferenceParameter(L"props", r.fillProps);
            twin.geometry.StartAnimation(property, expression);
        };
        if (forward) {
            bindTrim(L"TrimStart", L"bk.TrimStart");
            bindTrim(L"TrimEnd", L"bk.TrimStart + " + portion);
        } else {
            bindTrim(L"TrimStart", L"bk.TrimEnd - " + portion);
            bindTrim(L"TrimEnd", L"bk.TrimEnd");
        }
        bindTrim(L"TrimOffset", L"bk.TrimOffset");
    }
    r.fillBound = true;
    r.fillRtl = rtl;
}

// The fill color: shown where a twin covers something, faded by the fill's
// alpha. An empty twin is transparent, so its round caps can't leave a dot.
void BindFillColors(IndicatorRenderer& r, winrt::Windows::UI::Color color) {
    auto clear = color;
    clear.A = 0;
    for (int k = 0; k < 4; k++) {
        int first, second;
        bool forward;
        FillTrackOf(k, r.fillRtl, first, second, forward);
        const std::wstring portion = FillPortionExpression(k, first, second);
        auto expression = r.compositor.CreateExpressionAnimation(
            winrt::hstring(L"ColorLerp(clear, fill, Clamp(" + portion + L" * 400, 0, 1) * props.alpha)"));
        expression.SetReferenceParameter(L"b1", r.slots[first].geometry);
        expression.SetReferenceParameter(L"b2", r.slots[second].geometry);
        expression.SetReferenceParameter(L"props", r.fillProps);
        expression.SetColorParameter(L"fill", color);
        expression.SetColorParameter(L"clear", clear);
        r.fills[k].brush.StartAnimation(L"Color", expression);
    }
    r.fillColor = color;
    r.fillColorSet = true;
}

void SetFillScalar(IndicatorRenderer& r, wchar_t const* name, float value) {
    r.fillProps.StopAnimation(name);
    r.fillProps.InsertScalar(name, value);
}

// Moves the fill to a new value. Values that come fast (apps often report
// every 10 ms or so) are set as they come: the fill moves with them, frame
// by frame. Values further apart are animated linearly over the time since
// the previous one, so a slow stream of values still reads as steady motion:
// each move ends about when the next value comes. Every move starts from the
// last value set, never from `this.StartingValue`, which in a property set
// is the last value set directly, not where a running animation is — a move
// restarted from it jumped back on every value, stuttered, and stopped about
// halfway.
constexpr ULONGLONG kFillSnapMs = 33;
constexpr ULONGLONG kFillMaxMoveMs = 300;

void ChaseFill(IndicatorRenderer& r, double fraction, bool animate) {
    if (!r.fillProps) return;
    ULONGLONG now = GetTickCount64();
    ULONGLONG interval = r.fillTick ? now - r.fillTick : 0;
    r.fillTick = now;
    if (std::abs(fraction - r.fillFraction) <= 0.0005) return;
    if (animate && interval > kFillSnapMs) {
        auto animation = r.compositor.CreateScalarKeyFrameAnimation();
        animation.InsertKeyFrame(0.0f, (float)r.fillFraction);
        animation.InsertKeyFrame(1.0f, (float)fraction, r.linear);
        animation.Duration(std::chrono::milliseconds(std::min(interval, kFillMaxMoveMs)));
        r.fillProps.StartAnimation(L"f", animation);
    } else {
        SetFillScalar(r, L"f", (float)fraction);
    }
    r.fillFraction = fraction;
}

// Fades the fill in or out, from an explicit value (see ChaseFill).
void FadeFill(IndicatorRenderer& r, float from, float to, int durationMs) {
    auto animation = r.compositor.CreateScalarKeyFrameAnimation();
    animation.InsertKeyFrame(0.0f, from);
    animation.InsertKeyFrame(1.0f, to, r.linear);
    animation.Duration(std::chrono::milliseconds(std::max(1, durationMs)));
    r.fillProps.StartAnimation(L"alpha", animation);
}

// Creates the shape visual and its shapes on first use. Returns true when
// they were just created (the first placement is then not animated).
bool EnsureRenderer(std::shared_ptr<ButtonEntry> const& entry, FrameworkElement const& host) {
    auto& r = entry->renderer;
    if (r.visual) return false;
    r.compositor = ElementCompositionPreview::GetElementVisual(host).Compositor();
    r.visual = r.compositor.CreateShapeVisual();
    // Fast out, soft landing: most of the way in the first third.
    r.easing = r.compositor.CreateCubicBezierEasingFunction({0.1f, 0.9f}, {0.2f, 1.0f});
    r.linear = r.compositor.CreateLinearEasingFunction();
    for (auto& slot : r.slots) {
        slot.geometry = r.compositor.CreatePathGeometry();
        slot.shape = r.compositor.CreateSpriteShape(slot.geometry);
    }
    for (auto& twin : r.fills) {
        twin.geometry = r.compositor.CreatePathGeometry();
        twin.shape = r.compositor.CreateSpriteShape(twin.geometry);
        twin.brush = r.compositor.CreateColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
        twin.shape.StrokeBrush(twin.brush);
    }
    // Draw order: the corners, their fills over them, then the dots.
    auto shapes = r.visual.Shapes();
    for (int k = kSlotTL; k <= kSlotBL; k++) shapes.Append(r.slots[k].shape);
    for (auto& twin : r.fills) shapes.Append(twin.shape);
    shapes.Append(r.slots[kSlotDot].shape);
    shapes.Append(r.slots[kSlotBadge].shape);
    shapes.Append(r.slots[kSlotPinned].shape);
    BindFillTwins(r, false);
    ElementCompositionPreview::SetElementChildVisual(host, r.visual);
    return true;
}

// Rebuilds the frame path when the box, thickness, radii or corner style
// changed. Returns true when it did (everything is then placed without
// animation, since every position along the path changed meaning).
bool UpdateFramePath(IndicatorRenderer& r, const FrameBox& box, const Settings& localSettings) {
    FrameMetrics m = ComputeFrameMetrics(box.W, box.H, localSettings.FrameThickness, localSettings.FrameRadius,
                                         localSettings.IndDiagonal);
    double pad = std::max({localSettings.FrameThickness, localSettings.MultiDiameter, localSettings.BadgeDiameter,
                           localSettings.PinnedDiameter}) + 2.0;

    bool same = r.metrics.valid && std::abs(r.metrics.W - m.W) < 0.01 && std::abs(r.metrics.H - m.H) < 0.01 &&
                std::abs(r.metrics.t - m.t) < 0.001 && r.metrics.diagonal == m.diagonal;
    for (int k = 0; k < 4 && same; k++) {
        if (std::abs(r.metrics.r[k] - m.r[k]) > 0.01) same = false;
    }
    if (same && std::abs(r.pad - pad) < 0.01) return false;

    if (!same) {
        auto path = CreateFramePath(m);
        if (!path) return false;
        for (auto& slot : r.slots) slot.geometry.Path(path);
        for (auto& twin : r.fills) twin.geometry.Path(path);
        r.metrics = m;
        static std::atomic<bool> logged;
        if (!logged.exchange(true)) {
            Wh_Log(L"Frame path created: %gx%g, length %g; geometry sources alive: %d", m.W, m.H, m.P,
                   g_liveGeometrySources.load());
        }
    }

    // The shapes draw in box coordinates, offset by `pad` inside a visual
    // that much larger on every side, so round caps and a dot thicker than
    // the frame aren't cut at its edges.
    r.pad = pad;
    r.visual.Size({(float)(m.W + 2.0 * pad), (float)(m.H + 2.0 * pad)});
    r.visual.Offset({(float)-pad, (float)-pad, 0.0f});
    for (auto& slot : r.slots) slot.shape.Offset({(float)pad, (float)pad});
    for (auto& twin : r.fills) twin.shape.Offset({(float)pad, (float)pad});
    return true;
}

// Where each shape should be.
struct SlotTarget {
    bool shown = false;
    double a = 0.0, b = 0.0;       // its piece of the path while shown
    double hidden = 0.0;           // where it comes from and goes to
    bool collapseInPlace = false;  // goes away where it is instead
    bool grow = false;             // a dot: grows from its center and shrinks back into it
    bool dim = false;              // a dimmed missing corner (no fill, no attention color)
    bool trail = true;             // a moving piece draws its path (tail follows)
    bool driven = false;           // its ends follow the indeterminate sweep
    wuc::CompositionStrokeCap startCap = wuc::CompositionStrokeCap::Flat;
    wuc::CompositionStrokeCap endCap = wuc::CompositionStrokeCap::Flat;
    double thickness = 1.0;
    std::vector<winrt::Windows::UI::Color> colors;
};

// A corner mark: the arc and a leg on each side. Round caps reach half a
// thickness past each end, so the ends are pulled in by that much (as far as
// the leg allows) to keep the visible length.
void CornerExtent(const FrameMetrics& m, int k, double leg, bool roundedEnds, double& a, double& b) {
    double L = std::clamp(leg, 0.0, std::max(0.0, m.half - m.r[k]));
    double pull = roundedEnds ? std::min(L, m.t / 2.0) : 0.0;
    a = m.arcStart[k] - L + pull;
    b = m.arcStart[k] + m.arc[k] + L - pull;
}

void ComputeSlotTargets(SlotTarget targets[kSlotCount], const FrameMetrics& m, const Settings& localSettings,
                        int state, uint8_t mask, bool hasBadge, int windowCount, bool isActive, bool isLight,
                        bool attention, int pinnedState) {
    using wuc::CompositionStrokeCap;
    auto cap = [](bool round) { return round ? CompositionStrokeCap::Round : CompositionStrokeCap::Flat; };
    auto colors = GetIndicatorColors(localSettings, isLight, isActive);
    bool badgeShown = localSettings.BadgeEnabled && hasBadge;

    // Corners. Where they come from and go to when not shown: the top ones
    // the top center (the path's start and end: TR grows out from s = 0, TL
    // from s = P, each drawing its path outwards along the top edge and
    // around its arc, and they go back there), the bottom ones the bottom
    // center (the underline's middle, where they also split from). A button
    // left with no mark at all lets every piece shrink where it is.
    // The pinned-app dot, at the top center: on pinned apps, running or not
    // (a button with no window at all is a pinned app that isn't running).
    bool pinnedDot = localSettings.PinnedEnabled && ((state == 0 && windowCount == 0) || pinnedState == 1);
    const double hiddenAt[4] = {m.P, 0.0, m.sBottomMid, m.sBottomMid};
    for (int k = 0; k < 4; k++) {
        auto& t = targets[k];
        t.thickness = m.t;
        t.colors = colors;
        t.startCap = t.endCap = cap(localSettings.IndRoundedEnds);
        t.hidden = hiddenAt[k];
        t.collapseInPlace = state == 0;
        if ((state == 1 || state == 2) && (mask & (1 << k))) {
            t.shown = true;
            CornerExtent(m, k, localSettings.IndLeg, localSettings.IndRoundedEnds, t.a, t.b);
        }
    }

    // Dimmed missing corners: an open or maximized app's corners that aren't
    // marked are drawn too, dimmed, so the whole frame shows.
    if (localSettings.IndDimEnabled && (state == 1 || state == 2)) {
        auto dimColors = ResolveThemedColors(localSettings.IndDimLight, localSettings.IndDimDark,
                                             localSettings.IndDimOpacityLight, localSettings.IndDimOpacityDark, isLight,
                                             localSettings.AccentShading);
        if (HighContrastOn()) {
            dimColors = {SystemColor(COLOR_BTNTEXT)};
            for (auto& c : dimColors) {
                c.A = (uint8_t)std::lround(c.A * std::clamp(isLight ? localSettings.IndDimOpacityLight
                                                                    : localSettings.IndDimOpacityDark, 0.0, 1.0));
            }
        }
        for (int k = 0; k < 4; k++) {
            auto& t = targets[k];
            if (t.shown) continue;
            t.shown = true;
            t.dim = true;
            t.colors = dimColors;
            CornerExtent(m, k, localSettings.IndLeg, localSettings.IndRoundedEnds, t.a, t.b);
        }
    }

    // Connected corners: each corner reaches along its edge to the middle,
    // where it meets its pair flat, so the two read as one line — a cup
    // along the bottom (BR ends and BL starts at the bottom center) and a
    // cap along the top (TR starts at the top center, where the path
    // starts, and TL ends there, where it ends). The underline's halves
    // meet in the same place, so a restored app's underline just grows
    // into the cup.
    if (localSettings.IndConnect) {
        auto& tl = targets[kSlotTL];
        auto& tr = targets[kSlotTR];
        auto& br = targets[kSlotBR];
        auto& bl = targets[kSlotBL];
        if (br.shown) { br.b = m.sBottomMid; br.endCap = CompositionStrokeCap::Flat; }
        if (bl.shown) { bl.a = m.sBottomMid; bl.startCap = CompositionStrokeCap::Flat; }
        // On a pinned app with the dot on, the top center is the dot's
        // place: the top corners stay apart, like the side corners around
        // the other dots.
        if (!pinnedDot) {
            if (tr.shown) { tr.a = 0.0; tr.startCap = CompositionStrokeCap::Flat; }
            if (tl.shown) { tl.b = m.P; tl.endCap = CompositionStrokeCap::Flat; }
        }
    }

    // The underline: the BL and BR shapes as its two halves, meeting flat at
    // the bottom center. Along the path the bottom edge runs right to left,
    // so the BL half starts at the center and the BR half ends there.
    if (state == 3 || state == 4) {
        double maxHalf = std::max(0.0, std::min(m.cx - (m.x0 + m.r[kSlotBL]), (m.x1 - m.r[kSlotBR]) - m.cx));
        double halfLen = localSettings.MinLength > 0.0 ? std::min(localSettings.MinLength / 2.0, maxHalf) : maxHalf;
        double outer = localSettings.MinRounded ? std::max(0.0, halfLen - m.t / 2.0) : halfLen;
        auto& bl = targets[kSlotBL];
        bl.shown = true;
        bl.a = m.sBottomMid;
        bl.b = m.sBottomMid + outer;
        bl.startCap = CompositionStrokeCap::Flat;
        bl.endCap = cap(localSettings.MinRounded);
        auto& br = targets[kSlotBR];
        br.shown = true;
        br.a = m.sBottomMid - outer;
        br.b = m.sBottomMid;
        br.startCap = cap(localSettings.MinRounded);
        br.endCap = CompositionStrokeCap::Flat;
        // Running off screen: the same underline, dimmed.
        if (state == 4) {
            for (auto* t : {&bl, &br}) {
                for (auto& c : t->colors) c.A = (uint8_t)std::lround(c.A * localSettings.MinHiddenOpacity);
            }
        }
    }

    // Requesting attention: the corners or the underline take the attention
    // color (the dots don't), fading in and out like any color change.
    if (attention) {
        std::vector<winrt::Windows::UI::Color> attentionColors;
        if (HighContrastOn()) attentionColors = {SystemColor(COLOR_HIGHLIGHT)};
        else attentionColors = ResolveThemedColors(localSettings.AttentionLight, localSettings.AttentionDark, 1.0, 1.0,
                                                   isLight, localSettings.AccentShading);
        for (int k = kSlotTL; k <= kSlotBL; k++) {
            if (!targets[k].dim) targets[k].colors = attentionColors;
        }
    }

    // The multi-window dot: a near-zero piece with round caps, as thick as
    // its diameter, so it is a circle sitting on the path, in its own color.
    // It grows from its center when it appears and shrinks back into it when
    // it goes.
    {
        auto& t = targets[kSlotDot];
        constexpr double kDotEpsilon = 0.05;
        t.thickness = localSettings.MultiDiameter;
        t.colors = HighContrastOn()
            ? std::vector<winrt::Windows::UI::Color>{SystemColor(COLOR_BTNTEXT)}
            : ResolveThemedColors(localSettings.MultiLight, localSettings.MultiDark, 1.0, 1.0, isLight,
                                  localSettings.AccentShading);
        t.startCap = t.endCap = CompositionStrokeCap::Round;
        t.hidden = m.sLeftMid;
        t.grow = true;
        t.trail = false;
        t.shown = localSettings.MultiEnabled && windowCount >= 2;
        t.a = m.sLeftMid - kDotEpsilon;
        t.b = m.sLeftMid + kDotEpsilon;
    }

    // The badge dot: the multi-window dot's mirror, centered on the right
    // edge, in its own color. It grows and shrinks like that one, and shows
    // in every window state, even with no window at all.
    {
        auto& t = targets[kSlotBadge];
        constexpr double kDotEpsilon = 0.05;
        t.thickness = localSettings.BadgeDiameter;
        t.colors = HighContrastOn()
            ? std::vector<winrt::Windows::UI::Color>{SystemColor(COLOR_HIGHLIGHT)}
            : ResolveThemedColors(localSettings.BadgeLight, localSettings.BadgeDark, 1.0, 1.0, isLight,
                                  localSettings.AccentShading);
        t.startCap = t.endCap = CompositionStrokeCap::Round;
        t.hidden = m.sRightMid;
        t.grow = true;
        t.trail = false;
        t.shown = badgeShown;
        t.a = m.sRightMid - kDotEpsilon;
        t.b = m.sRightMid + kDotEpsilon;
    }

    // The pinned-app dot: centered on the top edge, where the path starts
    // (s = 0), for pinned apps. It grows and shrinks like the other dots, and
    // stays clear of the underline, so a minimized pinned app shows both.
    {
        auto& t = targets[kSlotPinned];
        constexpr double kDotEpsilon = 0.05;
        t.thickness = localSettings.PinnedDiameter;
        t.colors = HighContrastOn()
            ? std::vector<winrt::Windows::UI::Color>{SystemColor(COLOR_BTNTEXT)}
            : ResolveThemedColors(localSettings.PinnedLight, localSettings.PinnedDark, 1.0, 1.0, isLight,
                                  localSettings.AccentShading);
        t.startCap = t.endCap = CompositionStrokeCap::Round;
        t.hidden = 0.0;
        t.grow = true;
        t.trail = false;
        t.shown = pinnedDot;
        t.a = 0.0;  // just after the path's start, so the piece doesn't wrap
        t.b = 2.0 * kDotEpsilon;
    }
}

struct AnimationContext {
    bool animate = false;
    int durationMs = 160;
    int trailMs = 60;
};

bool SystemAnimationsEnabled() {
    BOOL enabled = TRUE;
    if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0)) return true;
    return enabled != FALSE;
}

std::vector<winrt::Windows::UI::Color> Transparent(std::vector<winrt::Windows::UI::Color> colors) {
    for (auto& c : colors) c.A = 0;
    return colors;
}

// The y of a cubic Bezier easing curve (0,0)-(x1,y1)-(x2,y2)-(1,1) at x.
double CubicBezierAt(double x, double x1, double y1, double x2, double y2) {
    auto coord = [](double t, double p1, double p2) {
        double u = 1.0 - t;
        return 3.0 * u * u * t * p1 + 3.0 * u * t * t * p2 + t * t * t;
    };
    double lo = 0.0, hi = 1.0, t = x;
    for (int i = 0; i < 40; i++) {  // x(t) is monotonic for easing curves
        t = (lo + hi) / 2.0;
        if (coord(t, x1, x2) < x) lo = t; else hi = t;
    }
    return coord(t, y1, y2);
}

// Where a tween is now: before its delay, its start; during it, along the
// fast-out curve the trims use (the renderer's `easing`); after, its end.
float ScalarTweenNow(const ScalarTween& t) {
    if (t.durationMs <= 0) return t.to;
    double elapsed = (double)(GetTickCount64() - t.start) - t.delayMs;
    if (elapsed <= 0.0) return t.from;
    double x = std::min(1.0, elapsed / t.durationMs);
    return (float)(t.from + (t.to - t.from) * CubicBezierAt(x, 0.1, 0.9, 0.2, 1.0));
}

winrt::Windows::UI::Color LerpColor(winrt::Windows::UI::Color a, winrt::Windows::UI::Color b, double p) {
    auto mix = [&](uint8_t x, uint8_t y) { return (uint8_t)std::lround(x + (y - x) * std::clamp(p, 0.0, 1.0)); };
    return {mix(a.A, b.A), mix(a.R, b.R), mix(a.G, b.G), mix(a.B, b.B)};
}

winrt::Windows::UI::Color ColorTweenNow(const ColorTween& t) {
    if (t.durationMs <= 0) return t.to;
    double x = std::clamp((double)(GetTickCount64() - t.start) / t.durationMs, 0.0, 1.0);
    if (t.fadeOut) {
        auto gone = t.to;
        gone.A = 0;
        return x < 0.7 ? LerpColor(t.from, t.to, x / 0.7) : LerpColor(t.to, gone, (x - 0.7) / 0.3);
    }
    return LerpColor(t.from, t.to, CubicBezierAt(x, 0.1, 0.9, 0.2, 1.0));
}

// Moves one end of a slot's piece, from where it is now.
void AnimateTrim(IndicatorRenderer& r, IndicatorSlot& slot, bool start, float to, int durationMs, int delayMs) {
    auto& tween = start ? slot.trimStartTween : slot.trimEndTween;
    auto animation = r.compositor.CreateScalarKeyFrameAnimation();
    float from;
    if (slot.forceMove) {
        // Leaving the sweep: its position is an expression's, not ours to
        // work out; this one start is left to the compositor.
        animation.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
        from = to;
    } else {
        from = ScalarTweenNow(tween);
        animation.InsertKeyFrame(0.0f, from);
    }
    animation.InsertKeyFrame(1.0f, to, r.easing);
    animation.Duration(std::chrono::milliseconds(std::max(1, durationMs)));
    if (delayMs > 0) {
        animation.DelayTime(std::chrono::milliseconds(delayMs));
        // Hold the start during the delay (the property's own value may be
        // stale, see ScalarTween).
        animation.DelayBehavior(wuc::AnimationDelayBehavior::SetInitialValueBeforeDelay);
    }
    slot.geometry.StartAnimation(start ? L"TrimStart" : L"TrimEnd", animation);
    tween = {from, to, GetTickCount64(), std::max(0, delayMs), std::max(1, durationMs)};
}

void SetTrim(IndicatorSlot& slot, bool start, float value) {
    if (start) {
        slot.geometry.StopAnimation(L"TrimStart");
        slot.geometry.TrimStart(value);
        slot.trimStartTween = {value, value, 0, 0, 0};
    } else {
        slot.geometry.StopAnimation(L"TrimEnd");
        slot.geometry.TrimEnd(value);
        slot.trimEndTween = {value, value, 0, 0, 0};
    }
}

// The brush's color objects (the color brush, or each gradient stop).
std::vector<wuc::CompositionObject> SlotColorTargets(IndicatorSlot& slot) {
    std::vector<wuc::CompositionObject> targets;
    if (slot.colorBrush) targets.push_back(slot.colorBrush);
    else for (auto& stop : slot.stops) targets.push_back(stop);
    return targets;
}

void SetColorOf(wuc::CompositionObject const& object, winrt::Windows::UI::Color color) {
    object.StopAnimation(L"Color");
    if (auto brush = object.try_as<wuc::CompositionColorBrush>()) brush.Color(color);
    else if (auto stop = object.try_as<wuc::CompositionColorGradientStop>()) stop.Color(color);
}

// The colors of a slot's brush, snapped. `transparent` hides the slot.
void SetSlotColors(IndicatorSlot& slot, const std::vector<winrt::Windows::UI::Color>& colors, bool transparent) {
    auto c = transparent ? Transparent(colors) : colors;
    auto targets = SlotColorTargets(slot);
    slot.colorTweens.assign(targets.size(), ColorTween{});
    for (size_t i = 0; i < targets.size() && i < c.size(); i++) {
        SetColorOf(targets[i], c[i]);
        slot.colorTweens[i] = {c[i], c[i], 0, 0, false};
    }
}

// The colors of a slot's brush, animated from where they are: to `colors`,
// or, with `fadeOut`, to `colors` and then faded out at the end (a piece
// going away stays visible while it travels, then disappears where it
// arrived).
void AnimateSlotColors(IndicatorRenderer& r, IndicatorSlot& slot, const std::vector<winrt::Windows::UI::Color>& colors,
                       bool fadeOut, int durationMs) {
    auto targets = SlotColorTargets(slot);
    if (slot.colorTweens.size() != targets.size()) slot.colorTweens.assign(targets.size(), ColorTween{});
    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < targets.size() && i < colors.size(); i++) {
        auto& tween = slot.colorTweens[i];
        auto from = tween.start || tween.durationMs == 0 ? ColorTweenNow(tween) : colors[i];
        auto to = colors[i];
        auto animation = r.compositor.CreateColorKeyFrameAnimation();
        animation.InterpolationColorSpace(wuc::CompositionColorSpace::Rgb);
        animation.InsertKeyFrame(0.0f, from);
        if (fadeOut) {
            auto gone = to;
            gone.A = 0;
            animation.InsertKeyFrame(0.7f, to, r.linear);
            animation.InsertKeyFrame(1.0f, gone, r.linear);
        } else {
            animation.InsertKeyFrame(1.0f, to, r.easing);
        }
        animation.Duration(std::chrono::milliseconds(std::max(1, durationMs)));
        targets[i].StartAnimation(L"Color", animation);
        tween = {from, to, now, std::max(1, durationMs), fadeOut};
    }
}

// Makes the slot's brush fit the number of colors: a color brush for one, a
// horizontal gradient for several. Returns true when it made a new one (its
// colors are then set, not animated).
bool EnsureSlotBrush(IndicatorRenderer& r, IndicatorSlot& slot, const std::vector<winrt::Windows::UI::Color>& colors) {
    size_t n = colors.size();
    if (n == 0) return false;
    bool fits = n == 1 ? (bool)slot.colorBrush : (slot.gradientBrush && slot.stops.size() == n);
    if (fits) return false;

    auto start = slot.shown ? colors : Transparent(colors);
    if (n == 1) {
        auto brush = r.compositor.CreateColorBrush(start[0]);
        slot.shape.StrokeBrush(brush);
        if (slot.gradientBrush) { try { slot.gradientBrush.Close(); } catch (...) {} }
        slot.gradientBrush = nullptr;
        slot.stops.clear();
        slot.colorBrush = brush;
    } else {
        auto brush = r.compositor.CreateLinearGradientBrush();
        brush.StartPoint({0.0f, 0.5f});
        brush.EndPoint({1.0f, 0.5f});
        std::vector<wuc::CompositionColorGradientStop> stops;
        for (size_t i = 0; i < n; i++) {
            auto stop = r.compositor.CreateColorGradientStop((float)i / (float)(n - 1), start[i]);
            brush.ColorStops().Append(stop);
            stops.push_back(stop);
        }
        slot.shape.StrokeBrush(brush);
        if (slot.colorBrush) { try { slot.colorBrush.Close(); } catch (...) {} }
        slot.colorBrush = nullptr;
        slot.gradientBrush = brush;
        slot.stops = std::move(stops);
    }
    slot.colorTweens.clear();
    for (auto& c : start) slot.colorTweens.push_back({c, c, 0, 0, false});
    return true;
}

// Moves one shape to its target. A piece appearing starts at its `hidden`
// point (as a circle, with round caps) and moves out from there; a piece
// going away moves to it (or shrinks where it is) and fades at the end. When
// both ends move the same way, the trailing end starts a little later, so
// the piece draws its path like a circle rolling along the frame and then
// settles to its length. Every movement and fade starts from where the
// last one is (see ScalarTween), so an interrupted one flows on.
// A dot's size: its stroke thickness, set or animated from wherever an
// interrupted change had got to (tracked like the trims, see ScalarTween).
void SetSlotThickness(IndicatorSlot& slot, double value) {
    slot.shape.StopAnimation(L"StrokeThickness");
    slot.shape.StrokeThickness((float)value);
    slot.thicknessTween = {(float)value, (float)value, 0, 0, 0};
    slot.thickness = value;
}

void AnimateSlotThickness(IndicatorRenderer& r, IndicatorSlot& slot, double to, int durationMs) {
    float from = ScalarTweenNow(slot.thicknessTween);
    auto animation = r.compositor.CreateScalarKeyFrameAnimation();
    animation.InsertKeyFrame(0.0f, from);
    animation.InsertKeyFrame(1.0f, (float)to, r.easing);
    animation.Duration(std::chrono::milliseconds(std::max(1, durationMs)));
    slot.shape.StartAnimation(L"StrokeThickness", animation);
    slot.thicknessTween = {from, (float)to, GetTickCount64(), 0, std::max(1, durationMs)};
    slot.thickness = to;
}

void AnimateSlot(IndicatorRenderer& r, IndicatorSlot& slot, const SlotTarget& target, const AnimationContext& ctx) {
    const double P = r.metrics.P;
    auto frac = [&](double s) { return (float)std::clamp(s / P, 0.0, 1.0); };

    if (slot.startCap != target.startCap) {
        slot.shape.StrokeStartCap(target.startCap);
        slot.startCap = target.startCap;
    }
    if (slot.endCap != target.endCap) {
        slot.shape.StrokeEndCap(target.endCap);
        slot.endCap = target.endCap;
    }
    bool brushMade = EnsureSlotBrush(r, slot, target.colors);
    bool appearing = target.shown && !slot.shown;
    bool snapAll = !ctx.animate || !slot.placed;

    // Thickness. A dot's is its size: 0 while hidden, grown to its diameter
    // when it appears and shrunk back to 0 when it goes, around its center.
    if (target.grow) {
        double want = target.shown ? target.thickness : 0.0;
        if (snapAll) {
            if (std::abs(slot.thickness - want) > 0.001 || slot.thicknessTween.durationMs > 0) {
                SetSlotThickness(slot, want);
            }
        } else if (std::abs(slot.thickness - want) > 0.001) {
            AnimateSlotThickness(r, slot, want, ctx.durationMs);
        }
    } else if (std::abs(slot.thickness - target.thickness) > 0.001) {
        slot.shape.StrokeThickness((float)target.thickness);
        slot.thickness = target.thickness;
    }

    // Ends. The sweep owns a driven slot's ends.
    double a1 = slot.a, b1 = slot.b;
    if (!target.driven && target.grow) {
        // A dot stays where it is: placed there at once when it appears (its
        // size does the animating), moved only if the frame changed under a
        // shown one.
        a1 = target.a;
        b1 = target.b;
        if (snapAll || appearing || !slot.shown) {
            if (std::abs(slot.a - a1) > 0.001 || std::abs(slot.b - b1) > 0.001 || !slot.placed || slot.forceMove) {
                SetTrim(slot, true, frac(a1));
                SetTrim(slot, false, frac(b1));
            }
        } else if (std::abs(a1 - slot.a) > 0.01 || std::abs(b1 - slot.b) > 0.01) {
            AnimateTrim(r, slot, true, frac(a1), ctx.durationMs, 0);
            AnimateTrim(r, slot, false, frac(b1), ctx.durationMs, 0);
        }
    } else if (!target.driven) {
        double hiddenPoint = target.collapseInPlace && slot.placed ? (slot.a + slot.b) / 2.0 : target.hidden;
        a1 = target.shown ? target.a : hiddenPoint;
        b1 = target.shown ? target.b : hiddenPoint;
        if (snapAll) {
            SetTrim(slot, true, frac(a1));
            SetTrim(slot, false, frac(b1));
        } else {
            if (appearing) {
                // Start from the hidden point.
                SetTrim(slot, true, frac(target.hidden));
                SetTrim(slot, false, frac(target.hidden));
                slot.a = slot.b = target.hidden;
            }
            double da = a1 - slot.a, db = b1 - slot.b;
            if (slot.forceMove || std::abs(da) > 0.01 || std::abs(db) > 0.01) {
                int delayA = 0, delayB = 0;
                if (target.trail && !slot.forceMove) {
                    if (da > 0.01 && db > 0.01) delayA = ctx.trailMs;        // moving forward: the start trails
                    else if (da < -0.01 && db < -0.01) delayB = ctx.trailMs;  // moving back: the end trails
                }
                AnimateTrim(r, slot, true, frac(a1), ctx.durationMs, delayA);
                AnimateTrim(r, slot, false, frac(b1), ctx.durationMs, delayB);
            }
        }
    }

    // Colors.
    bool shownNow = target.driven || target.shown;
    if (target.grow && !shownNow && !snapAll && !brushMade) {
        // A dot shrinking away keeps its color; at size 0 it shows nothing.
    } else if (snapAll || brushMade) {
        SetSlotColors(slot, target.colors, !shownNow);
    } else if (appearing || (target.driven && !slot.shown)) {
        SetSlotColors(slot, target.colors, false);
    } else if (shownNow && !SameColors(target.colors, slot.colors)) {
        AnimateSlotColors(r, slot, target.colors, false, ctx.durationMs);
    } else if (!shownNow && slot.shown) {
        AnimateSlotColors(r, slot, slot.colors.empty() ? target.colors : slot.colors, true,
                          ctx.durationMs + (target.trail ? ctx.trailMs : 0));
    } else if (!shownNow) {
        SetSlotColors(slot, target.colors, true);
    }

    slot.placed = true;
    slot.shown = shownNow;
    slot.colors = target.colors;
    if (target.driven) {
        slot.forceMove = true;  // when the sweep ends, move on from wherever it got to
    } else {
        slot.forceMove = false;
        slot.a = a1;
        slot.b = b1;
    }
}

// ---------------------------------------------------------------------------
// Indeterminate progress animations, timed like the native ProgressBar's
// indeterminate storyboard (WinUI): a 2 s cycle, repeated; the first bar
// runs over the first 1.5 s, from just off the track's left end to 120% of
// the track; the second waits 0.75 s, then runs from 90% of the track off
// its left end (or further, for a longer bar) to the track's right end,
// until the end of the cycle; both with the key spline (0.4, 0, 0.6, 1).
// The native bars are 40% and 60% of the track wide; here both are as long
// as the underline itself, so the line keeps its length while it sweeps.
// ---------------------------------------------------------------------------

constexpr int kProgressCycleMs = 2000;
constexpr float kProgressMoveEnd = 0.75f; // 1.5 s of 2 s

void EnsureProgressObjects(IndicatorRenderer& r) {
    if (!r.progressEasing) r.progressEasing = r.compositor.CreateCubicBezierEasingFunction({0.4f, 0.0f}, {0.6f, 1.0f});
    if (!r.stepEasing) r.stepEasing = r.compositor.CreateStepEasingFunction(1);
}

// The underline's sweep: the BL and BR shapes become the two bars, moving
// left to right along the bottom edge's straight part, cut to it.
// Their ends are expressions of the bars' positions, kept in a property set
// that keyframe animations move. Along the path the bottom edge runs right
// to left (s = sBottomMid + cx - x), so a bar's right end is its TrimStart.
void StartSweep(IndicatorRenderer& r, double trackL, double trackR, double barW, bool rtl) {
    EnsureProgressObjects(r);
    const auto& m = r.metrics;
    if (!r.progress) {
        r.progress = r.compositor.CreatePropertySet();
        r.progress.InsertScalar(L"p1", 0.0f);
        r.progress.InsertScalar(L"p2", 0.0f);
    }
    double T = std::max(1.0, trackR - trackL);
    double w = std::clamp(barW, 1.0, T);
    double w1 = w, w2 = w;

    // Positions of a bar's left end, from the track's left end.
    auto bar = [&](wchar_t const* name, double w, bool second) {
        auto animation = r.compositor.CreateScalarKeyFrameAnimation();
        if (second) {
            float start = (float)std::min(-0.9 * T, -w);
            animation.InsertKeyFrame(0.0f, start);
            animation.InsertKeyFrame(0.375f, start);
            animation.InsertKeyFrame(1.0f, (float)T, r.progressEasing);
        } else {
            animation.InsertKeyFrame(0.0f, (float)(-w));
            animation.InsertKeyFrame(kProgressMoveEnd, (float)(1.2 * T), r.progressEasing);
            animation.InsertKeyFrame(1.0f, (float)(1.2 * T));
        }
        animation.Duration(std::chrono::milliseconds(kProgressCycleMs));
        animation.IterationBehavior(wuc::AnimationIterationBehavior::Forever);
        r.progress.StartAnimation(name, animation);
    };
    bar(L"p1", w1, false);
    bar(L"p2", w2, true);

    auto bind = [&](IndicatorSlot& slot, wchar_t const* position, double w) {
        // A sweep has no rotation: drop any left over from the corners' lap.
        slot.geometry.StopAnimation(L"TrimOffset");
        slot.geometry.TrimOffset(0.0f);
        std::wstring pos = std::wstring(L"props.") + position;
        auto edge = [&](wchar_t const* property, std::wstring const& x) {
            auto expression = r.compositor.CreateExpressionAnimation(
                winrt::hstring(L"(sMid + cx - Clamp(" + x + L", trackL, trackR)) / pathLen"));
            expression.SetScalarParameter(L"sMid", (float)m.sBottomMid);
            expression.SetScalarParameter(L"cx", (float)m.cx);
            expression.SetScalarParameter(L"trackL", (float)trackL);
            expression.SetScalarParameter(L"trackR", (float)trackR);
            expression.SetScalarParameter(L"w", (float)w);
            expression.SetScalarParameter(L"pathLen", (float)m.P);
            expression.SetReferenceParameter(L"props", r.progress);
            slot.geometry.StartAnimation(property, expression);
        };
        if (rtl) {
            // Mirrored: positions run from the track's right end, leftwards.
            edge(L"TrimStart", L"trackR - " + pos);
            edge(L"TrimEnd", L"trackR - " + pos + L" - w");
        } else {
            edge(L"TrimStart", L"trackL + " + pos + L" + w");
            edge(L"TrimEnd", L"trackL + " + pos);
        }
    };
    bind(r.slots[kSlotBL], L"p1", w1);
    bind(r.slots[kSlotBR], L"p2", w2);

    r.sweepKey[0] = trackL; r.sweepKey[1] = trackR; r.sweepKey[2] = m.P; r.sweepKey[3] = m.sBottomMid;
    r.sweepKey[4] = barW;
    r.sweepKey[5] = rtl ? 1.0 : 0.0;
}

// Ends the sweep: the bars stay where they are, and the next placement moves
// them on from there (forceMove, set while they were driven).
void StopSweep(IndicatorRenderer& r) {
    for (int k : {kSlotBL, kSlotBR}) {
        r.slots[k].geometry.StopAnimation(L"TrimStart");
        r.slots[k].geometry.StopAnimation(L"TrimEnd");
    }
    if (r.progress) {
        r.progress.StopAnimation(L"p1");
        r.progress.StopAnimation(L"p2");
    }
    for (auto& v : r.sweepKey) v = -1;
}

// The lap: every shape's TrimOffset (the corners' and the two dots') makes
// one full turn clockwise per cycle, with the native bar's easing over its
// first 1.5 s and a rest for the other 0.5 s, so the corners run around the
// frame like snakes, the dots run along with them instead of being run
// over, and everything settles back where it belongs. A full turn per cycle brings any
// arrangement of corners back onto itself, so the cycle repeats seamlessly
// and a state change during it (open <-> maximized) doesn't need a restart.
void StartLap(IndicatorRenderer& r) {
    EnsureProgressObjects(r);
    auto animation = r.compositor.CreateScalarKeyFrameAnimation();
    animation.InsertKeyFrame(0.0f, 0.0f);
    animation.InsertKeyFrame(kProgressMoveEnd, 1.0f, r.progressEasing);
    animation.InsertKeyFrame(1.0f, 1.0f);
    animation.Duration(std::chrono::milliseconds(kProgressCycleMs));
    animation.IterationBehavior(wuc::AnimationIterationBehavior::Forever);
    for (int k = 0; k < kSlotCount; k++) {
        r.slots[k].geometry.StopAnimation(L"TrimOffset");
        r.slots[k].geometry.TrimOffset(0.0f);
        r.slots[k].geometry.StartAnimation(L"TrimOffset", animation);
    }
    r.lapStartTick = GetTickCount64();
}

// Ends the lap without a jump: a turn under way is finished, then the offset
// drops back to 0, which after a full turn looks the same. It starts from
// where the turn is, worked out from the time it started and the native
// curve (not this.StartingValue, see ChaseFill).
void FinishLap(IndicatorRenderer& r) {
    double elapsed = (double)(GetTickCount64() - r.lapStartTick);
    double phase = std::fmod(elapsed, (double)kProgressCycleMs) / kProgressCycleMs;
    for (int k = 0; k < kSlotCount; k++) {
        auto& geometry = r.slots[k].geometry;
        if (phase >= kProgressMoveEnd) {
            geometry.StopAnimation(L"TrimOffset");
            geometry.TrimOffset(0.0f);
            continue;
        }
        int remainingMs = std::max(1, (int)std::lround((kProgressMoveEnd - phase) * kProgressCycleMs));
        float current = (float)CubicBezierAt(phase / kProgressMoveEnd, 0.4, 0.0, 0.6, 1.0);
        auto animation = r.compositor.CreateScalarKeyFrameAnimation();
        animation.InsertKeyFrame(0.0f, current);
        animation.InsertKeyFrame(0.99f, 1.0f, r.easing);
        animation.InsertKeyFrame(1.0f, 0.0f, r.stepEasing);
        animation.Duration(std::chrono::milliseconds(remainingMs + 10));
        geometry.StartAnimation(L"TrimOffset", animation);
    }
}

// Whether the button is flashing for attention: one of its template's
// visual state groups has a current state named for it (the taskbar's own
// state, so it starts and stops exactly with the native flash; every state
// change runs UpdateVisualStates, which refreshes the button).
bool IsRequestingAttention(FrameworkElement const& button) {
    try {
        if (VisualTreeHelper::GetChildrenCount(button) < 1) return false;
        auto root = VisualTreeHelper::GetChild(button, 0).try_as<FrameworkElement>();
        if (!root) return false;
        auto groups = VisualStateManager::GetVisualStateGroups(root);
        static std::atomic<bool> logged;
        bool log = !logged.exchange(true);
        bool attention = false;
        for (auto const& group : groups) {
            auto state = group.CurrentState();
            if (log) Wh_Log(L"Visual state group %s: %s", group.Name().c_str(), state ? state.Name().c_str() : L"(none)");
            if (state && ContainsNoCase(state.Name(), L"Attention")) attention = true;
        }
        return attention;
    } catch (...) {
        return false;
    }
}

// ---------------------------------------------------------------------------
// Debug logging: one line per button whenever its summary changes. The app
// is named by its process (window titles change too often to compare).
// ---------------------------------------------------------------------------

std::wstring ProcessNameOf(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return L"?";
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return L"pid " + std::to_wstring(pid);
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    std::wstring name = L"pid " + std::to_wstring(pid);
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        std::wstring full(path, size);
        size_t slash = full.find_last_of(L"\\/");
        name = slash == std::wstring::npos ? full : full.substr(slash + 1);
    }
    CloseHandle(process);
    return name;
}

// The item (view model) a button shows, from its repeater, or null for a
// recycled button.
winrt::Windows::Foundation::IInspectable GetButtonItem(FrameworkElement const& button) {
    try {
        auto parent = VisualTreeHelper::GetParent(button);
        auto repeater = parent ? parent.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>() : nullptr;
        if (!repeater) return nullptr;
        auto view = repeater.ItemsSourceView();
        int count = view ? view.Count() : 0;
        int index = GetRepeaterItemIndex(repeater, button, count);
        return index >= 0 ? view.GetAt(index) : nullptr;
    } catch (...) {
        return nullptr;
    }
}

// Whether the button's app is pinned: 1, 0, or -1 when it can't be told
// (a recycled button, or no IsPinned symbol on this build).
int ReadButtonPinned(FrameworkElement const& button) {
    if (!IsPinnedStateAvailable()) return -1;
    try {
        auto item = GetButtonItem(button);
        if (!item) return -1;
        void* raw = winrt::get_abi(item);
        struct Candidate { void* vftable; ViewModel_get_IsPinned_t get; };
        Candidate candidates[2] = {};
        switch (GetViewModelKind(raw)) {
            case ViewModelKind::kWindow:
                candidates[0] = {TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable,
                                 TaskListWindowViewModel_AppItem_get_IsPinned};
                candidates[1] = {TaskListWindowViewModel_ITaskListWindowViewModel_vftable,
                                 TaskListWindowViewModel_Window_get_IsPinned};
                break;
            case ViewModelKind::kGroup:
                candidates[0] = {TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable,
                                 TaskListGroupViewModel_AppItem_get_IsPinned};
                candidates[1] = {TaskListGroupViewModel_ITaskListGroupViewModel_vftable,
                                 TaskListGroupViewModel_Group_get_IsPinned};
                break;
            default:
                return -1;
        }
        for (auto const& c : candidates) {
            if (!c.vftable || !c.get) continue;
            void* iface = FindInterfaceByVtable(raw, c.vftable);
            if (!iface) continue;
            bool value = false;
            if (SUCCEEDED(c.get(iface, &value))) return value ? 1 : 0;
        }
    } catch (...) {}
    return -1;
}

void UnwatchViewModelForDebug(std::shared_ptr<ButtonEntry> const& e) {
    if (e->debugViewModel) {
        try { e->debugViewModel.PropertyChanged(e->debugViewModelToken); } catch (...) {}
    }
    e->debugViewModel = nullptr;
}

// Debug logging: every property the button's view model reports changed.
void WatchViewModelForDebug(FrameworkElement const& button, std::shared_ptr<ButtonEntry> const& entry,
                            const Settings& localSettings) {
    if (!localSettings.DebugLog) {
        UnwatchViewModelForDebug(entry);
        return;
    }
    auto item = GetButtonItem(button);
    if (!item) return;  // recycled: keep what it had
    auto notify = item.try_as<winrt::Windows::UI::Xaml::Data::INotifyPropertyChanged>();
    {
        static std::atomic<int> logged{0};
        if (logged.fetch_add(1) < 2) {
            auto className = winrt::get_class_name(item);
            Wh_Log(L"[debug] view model %s: INotifyPropertyChanged %s",
                   className.empty() ? L"(no runtime class name)" : className.c_str(),
                   notify ? L"available" : L"not implemented");
        }
    }
    if (entry->debugViewModel == notify) return;
    UnwatchViewModelForDebug(entry);
    if (!notify) return;
    std::weak_ptr<ButtonEntry> weakEntry = entry;
    try {
        entry->debugViewModelToken = notify.PropertyChanged(
            [weakEntry](auto const&, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
                if (g_unloading) return;
                auto e = weakEntry.lock();
                Wh_Log(L"[debug] %s view model: %s changed", e && !e->debugApp.empty() ? e->debugApp.c_str() : L"?",
                       args.PropertyName().c_str());
            });
        entry->debugViewModel = notify;
    } catch (...) {}
}

void LogButtonSummary(FrameworkElement const& button, std::shared_ptr<ButtonEntry> const& entry, int state,
                      int windowCount, bool isActive, bool hasBadge, bool attention, const ProgressInfo& progress,
                      int progressMode, bool fillActive, bool rtl, int pinnedState) {
    // A recycled button (kept hidden by the taskbar for reuse) keeps its
    // last state; say so rather than "no window".
    std::wstring app = L"(no window)";
    auto resolved = ResolveButtonWindows(button);
    if (resolved.state == ButtonWindowsState::kUnrealized) {
        app = L"(recycled)";
    } else if (resolved.state != ButtonWindowsState::kResolved) {
        app = L"(unresolved)";
    } else if (!resolved.windows.empty()) {
        // Looked up once per window: opening the process on every refresh
        // made debug logging cost more than the indicators themselves.
        HWND hwnd = resolved.windows[0];
        if (hwnd != entry->debugHwnd || entry->debugProcess.empty()) {
            entry->debugHwnd = hwnd;
            entry->debugProcess = ProcessNameOf(hwnd);
        }
        app = entry->debugProcess;
    }
    static const wchar_t* kStates[] = {L"none", L"open", L"maximized", L"minimized", L"offscreen"};
    std::wstring progressText = L"none";
    if (progress.indeterminate) {
        progressText = L"indeterminate";
    } else if (progress.determinate) {
        // In 10% steps, so a running progress doesn't log every value.
        progressText = std::to_wstring((int)std::lround(progress.fraction * 10.0) * 10) + L"%";
        if (progress.paused) progressText += L" paused";
        if (progress.error) progressText += L" error";
    }
    static const wchar_t* kModes[] = {L"none", L"sweep", L"lap"};
    std::wstring summary = app + L": windows=" + std::to_wstring(windowCount) +
        L" state=" + (state >= 0 && state <= 4 ? kStates[state] : L"?") +
        L" active=" + (isActive ? L"yes" : L"no") +
        L" pinned=" + (pinnedState == 1 ? L"yes" : pinnedState == 0 ? L"no" : L"?") +
        L" badge=" + (hasBadge ? L"yes" : L"no") +
        L" attention=" + (attention ? L"yes" : L"no") +
        L" progress=" + progressText +
        L" animation=" + (progressMode >= 0 && progressMode <= 2 ? kModes[progressMode] : L"?") +
        (fillActive ? L"+fill" : L"") +
        (rtl ? L" rtl" : L"") +
        L" bound=" + (entry->hostBound ? L"yes" : L"no") +
        NativeBadgeDetail(entry);
    // The icon panel's children, back to front, when the button has a badge
    // element: where the taskbar put it relative to the icon.
    if (!entry->nativeBadges.empty()) {
        auto iconPanel = FindChildByName(button, L"IconPanel");
        if (iconPanel) summary += L" order=[" + PanelOrderText(iconPanel) + L"]";
    }
    entry->debugApp = app;
    if (summary == entry->debugLast) return;
    entry->debugLast = summary;
    Wh_Log(L"[debug] %s", summary.c_str());
}

void UpdateIndicator(FrameworkElement const& button, std::shared_ptr<ButtonEntry> const& entry,
                     FrameworkElement const& iconPanel, const Settings& localSettings) {
    if (!localSettings.AnyEnabled() || !IsViewModelBridgeAvailable()) {
        ReleaseNativeBadges(entry);
        RetireHost(entry);
        if (entry->iconSizeAttached) {
            if (auto icon = entry->icon.get()) { try { icon.SizeChanged(entry->iconSizeToken); } catch (...) {} }
            entry->iconSizeAttached = false;
        }
        entry->indState = -1;
        return;
    }

    // The icon element (Image#Icon), re-resolved when it expired or was
    // detached (template re-applied).
    auto icon = entry->icon.get();
    if (!icon || !VisualTreeHelper::GetParent(icon)) {
        if (icon && entry->iconSizeAttached) {
            try { icon.SizeChanged(entry->iconSizeToken); } catch (...) {}
        }
        entry->iconSizeAttached = false;
        icon = FindChildByName(iconPanel, L"Icon");
        if (!icon) icon = FindChildByName(button, L"Icon");
        entry->icon = icon ? winrt::make_weak(icon) : winrt::weak_ref<FrameworkElement>{};
        entry->hostBound = false;
    }
    if (!icon) return;
    auto iconParent = VisualTreeHelper::GetParent(icon);
    auto panel = iconParent ? iconParent.try_as<Panel>() : nullptr;
    if (!panel) return;

    // Window state: -1 (recycled container) keeps the last known state. A
    // state change (open -> minimized -> open, say) always rebinds: launch,
    // close and minimize transitions can animate and re-template the icon,
    // which can enable its Translation facade or move it, and a binding made
    // before that would miss the new terms.
    int windowCount = -1;
    int activeState = -1;
    int state = EvaluateIndicatorState(button, localSettings, &windowCount, &activeState);
    if (state < 0) state = entry->indState < 0 ? 0 : entry->indState;
    if (windowCount < 0) windowCount = entry->windowCount < 0 ? 0 : entry->windowCount;
    entry->windowCount = windowCount;
    if (activeState < 0) activeState = entry->isActive < 0 ? 0 : entry->isActive;
    entry->isActive = (int8_t)activeState;
    bool isActive = activeState == 1;
    if (state != entry->indState) entry->hostBound = false;
    entry->indState = (int8_t)state;

    // Native badges: hidden, and whether one is showing.
    bool hasBadge = UpdateNativeBadges(entry, iconPanel, localSettings);

    // Progress, as the native progress bar shows it.
    ProgressInfo progress = UpdateProgressWatch(button, entry, iconPanel, localSettings);
    bool indeterminate = localSettings.ProgressAnimation && progress.indeterminate;

    // Follow the icon's size (taskbar size changes, small icons).
    if (!entry->iconSizeAttached) {
        entry->iconSizeAttached = true;
        std::weak_ptr<ButtonEntry> weakEntry = entry;
        entry->iconSizeToken = icon.SizeChanged([weakEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            e->hostBound = false;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { RefreshButton(btn, localSettings); } catch (...) {}
        });
    }

    // Open windows: the two bottom corners; maximized: all four, or only the
    // two top ones with "Bottom corners on maximized windows too" off.
    constexpr uint8_t kBottom = (uint8_t)((1 << kSlotBR) | (1 << kSlotBL));
    constexpr uint8_t kTop = (uint8_t)((1 << kSlotTL) | (1 << kSlotTR));
    uint8_t mask = state == 2 ? (localSettings.IndMaximizedBottom ? (uint8_t)(kTop | kBottom) : kTop)
                 : state == 1 ? kBottom : 0;

    auto host = EnsureHost(entry, panel);
    if (!host) return;

    // Without a size (the icon is being swapped), the indicators stay hidden
    // rather than showing at a stale place; the icon's SizeChanged brings
    // them back.
    double iw = icon.ActualWidth(), ih = icon.ActualHeight();
    if (iw <= 0.001 || ih <= 0.001) {
        host.Opacity(0.0);
        return;
    }

    bool created = EnsureRenderer(entry, host);
    auto& r = entry->renderer;
    if (!r.visual) return;
    FrameBox box = ComputeFrameBox(iw, ih, localSettings);
    bool rebuilt = UpdateFramePath(r, box, localSettings);
    if (!r.metrics.valid) return;

    if (!entry->hostBound || std::abs(entry->hostAdjX - box.adjX) > 0.5f || std::abs(entry->hostAdjY - box.adjY) > 0.5f) {
        if (BindToElement(host, panel, icon, box.adjX, box.adjY)) {
            entry->hostBound = true;
            entry->hostAdjX = box.adjX;
            entry->hostAdjY = box.adjY;
            if (localSettings.DebugLog) try {
                auto io = icon.ActualOffset();
                Wh_Log(L"Indicators bound: state=%d box=%gx%g adj=(%g,%g) icon=%gx%g@(%g,%g)",
                       state, box.W, box.H, box.adjX, box.adjY, iw, ih, io.x, io.y);
            } catch (...) {}
        }
    }
    host.Opacity(entry->hostBound ? 1.0 : 0.0);

    bool attention = localSettings.AttentionEnabled && state != 0 && IsRequestingAttention(button);

    // Right-to-left layouts fill and sweep from the right, like the native bar.
    bool rtl = false;
    try { rtl = button.FlowDirection() == FlowDirection::RightToLeft; } catch (...) {}
    {
        static std::atomic<bool> logged;
        if (rtl && !logged.exchange(true)) Wh_Log(L"Right-to-left taskbar: progress runs from the right");
    }

    SlotTarget targets[kSlotCount];
    // Read for the pinned-app dot, and for the debug log, which shows it.
    int pinnedState = localSettings.PinnedEnabled || localSettings.DebugLog ? ReadButtonPinned(button) : -1;
    if (pinnedState < 0) pinnedState = entry->pinnedState;  // recycled: keep the last known
    entry->pinnedState = (int8_t)pinnedState;
    ComputeSlotTargets(targets, r.metrics, localSettings, state, mask, hasBadge, windowCount, isActive,
                       IsElementLightMode(button), attention, pinnedState);

    // Indeterminate progress: a minimized app's underline sweeps (the dots
    // stay, clear of it on the side edges); an open or maximized app's
    // corners and dots run around the frame together. It runs with Windows
    // animations off too: like the native moving bar it stands in for, it
    // shows information (often the only sign of the progress, with the
    // native bar hidden), not a transition.
    int progressMode = 0;
    if (indeterminate) {
        if (state == 3 || state == 4) progressMode = 1;
        else if (state == 1 || state == 2) progressMode = 2;
    }
    // The sweep runs along the whole straight part of the bottom edge, with
    // bars as long as the underline (its visible length, caps included).
    double trackL = 0.0, trackR = 0.0, barW = 0.0;
    if (progressMode == 1) {
        const auto& m = r.metrics;
        double maxHalf = std::max(0.0, std::min(m.cx - (m.x0 + m.r[kSlotBL]), (m.x1 - m.r[kSlotBR]) - m.cx));
        double halfLen = localSettings.MinLength > 0.0 ? std::min(localSettings.MinLength / 2.0, maxHalf) : maxHalf;
        trackL = m.cx - maxHalf;
        trackR = m.cx + maxHalf;
        barW = 2.0 * halfLen;
        for (int k : {kSlotBL, kSlotBR}) {
            targets[k].driven = true;
            targets[k].startCap = targets[k].endCap = wuc::CompositionStrokeCap::Flat;
        }
    }
    bool sweepChanged = std::abs(r.sweepKey[0] - trackL) > 0.01 || std::abs(r.sweepKey[1] - trackR) > 0.01 ||
                        std::abs(r.sweepKey[2] - r.metrics.P) > 0.01 ||
                        std::abs(r.sweepKey[3] - r.metrics.sBottomMid) > 0.01 ||
                        std::abs(r.sweepKey[4] - barW) > 0.01 || r.sweepKey[5] != (rtl ? 1.0 : 0.0);
    try {
        if (r.progressMode == 2 && progressMode != 2) FinishLap(r);
        if (r.progressMode == 1 && (progressMode != 1 || sweepChanged)) StopSweep(r);
        if (progressMode == 2 && r.progressMode != 2) StartLap(r);
        if (progressMode == 1 && (r.progressMode != 1 || sweepChanged)) StartSweep(r, trackL, trackR, barW, rtl);
    } catch (...) {
        Wh_Log(L"Exception while switching the progress animation");
    }
    r.progressMode = (int8_t)progressMode;

    AnimationContext ctx;
    ctx.animate = localSettings.Animations && !created && !rebuilt && entry->hostBound && SystemAnimationsEnabled();
    ctx.durationMs = localSettings.AnimationDurationMs;
    ctx.trailMs = std::max(1, (int)std::lround(localSettings.AnimationDurationMs * 0.4));
    for (int k = 0; k < kSlotCount; k++) {
        try { AnimateSlot(r, r.slots[k], targets[k], ctx); } catch (...) {
            Wh_Log(L"Exception while placing indicator %d", k);
        }
    }

    // Determinate progress: the fill, over whichever corners are shown.
    try {
        // The twins draw like their base shapes.
        for (int k = 0; k < 4; k++) {
            auto& twin = r.fills[k];
            const auto& slot = r.slots[k];
            if (twin.startCap != slot.startCap) { twin.shape.StrokeStartCap(slot.startCap); twin.startCap = slot.startCap; }
            if (twin.endCap != slot.endCap) { twin.shape.StrokeEndCap(slot.endCap); twin.endCap = slot.endCap; }
            if (std::abs(twin.thickness - slot.thickness) > 0.001) {
                twin.shape.StrokeThickness((float)slot.thickness);
                twin.thickness = slot.thickness;
            }
        }

        bool fillOn = localSettings.ProgressFill && progress.determinate && state >= 1 && state <= 4;
        if (!r.fillBound || r.fillRtl != rtl) {
            BindFillTwins(r, rtl);
            if (r.fillColorSet) BindFillColors(r, r.fillColor);
        }
        // Dimmed missing corners don't take the fill.
        r.fillProps.InsertScalar(L"top", targets[kSlotTL].dim ? 0.0f : 1.0f);
        r.fillProps.InsertScalar(L"bottom", targets[kSlotBL].dim ? 0.0f : 1.0f);
        bool isLight = IsElementLightMode(button);
        const auto& light = progress.error ? localSettings.FillErrorLight
                          : progress.paused ? localSettings.FillPausedLight : localSettings.FillLight;
        const auto& dark = progress.error ? localSettings.FillErrorDark
                         : progress.paused ? localSettings.FillPausedDark : localSettings.FillDark;
        auto colors = HighContrastOn() ? std::vector<winrt::Windows::UI::Color>{SystemColor(COLOR_HOTLIGHT)}
                                       : ResolveThemedColors(light, dark, 1.0, 1.0, isLight, localSettings.AccentShading);
        auto color = colors.empty() ? winrt::Windows::UI::Color{255, 16, 124, 16} : colors[0];
        bool fillAnimate = ctx.animate;

        if (fillOn) {
            bool colorChanged = !r.fillColorSet || color.A != r.fillColor.A || color.R != r.fillColor.R ||
                                color.G != r.fillColor.G || color.B != r.fillColor.B;
            if (colorChanged) BindFillColors(r, color);
            float fraction = (float)progress.fraction;
            if (!r.fillActive) {
                // Starts at its value, fading in.
                SetFillScalar(r, L"f", fraction);
                if (fillAnimate) FadeFill(r, 0.0f, 1.0f, ctx.durationMs);
                else SetFillScalar(r, L"alpha", 1.0f);
                r.fillFraction = progress.fraction;
                r.fillTick = GetTickCount64();
            } else {
                ChaseFill(r, progress.fraction, fillAnimate);
            }
            r.fillActive = true;
        } else if (r.fillActive) {
            // Done (or gone): the fill fades out where it is.
            if (fillAnimate) FadeFill(r, 1.0f, 0.0f, ctx.durationMs * 2);
            else SetFillScalar(r, L"alpha", 0.0f);
            r.fillActive = false;
        }
    } catch (...) {
        Wh_Log(L"Exception while updating the progress fill");
    }

    if (localSettings.DebugLog) {
        try {
            LogButtonSummary(button, entry, state, windowCount, isActive, hasBadge, attention, progress, progressMode,
                             r.fillActive, rtl, pinnedState);
        } catch (...) {}
    }
    try { WatchViewModelForDebug(button, entry, localSettings); } catch (...) {}
}

// The single entry point for every trigger: resolves the entry, attaches
// the button's lifecycle subscriptions once, finds the icon's panel and
// applies the marks.
void EnsureColorSubscription();

void RefreshButton(FrameworkElement const& button, const Settings& localSettings) {
    if (g_unloading) return;

    if (localSettings.DebugLog) {
        g_refreshStats.total++;
        ReportRefreshStats();
    }

    // Subscribes to system color changes on the first refresh if startup
    // couldn't (after an Explorer restart the mod starts before the taskbar
    // window exists). Refreshes run on the taskbar's thread.
    EnsureColorSubscription();

    auto entry = FindOrCreateEntry(button);
    if (!entry) return;

    // Button removal (window moved to another monitor, app closed, container
    // recycled) is a lifecycle event. On a genuine Unloaded, retire the
    // marks, revoke everything and drop the entry; a reused container gets
    // a fresh entry that adopts the retired paths.
    if (!entry->unloadAttached) {
        entry->unloadAttached = true;
        std::weak_ptr<ButtonEntry> weakEntry = entry;
        entry->unloadToken = button.Unloaded([weakEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakEntry.lock();
            if (!e) return;
            auto btn = e->button.get();

            // XAML delivers Unloaded asynchronously: a container that was
            // detached and immediately re-attached (recycling, cross-monitor
            // moves, pinned->running item swaps) receives a STALE Unloaded
            // while it is already live again. If the button is still loaded,
            // re-resolve and rebind instead of tearing down.
            bool stillLoaded = false;
            if (btn) {
                try { stillLoaded = btn.IsLoaded(); } catch (...) {}
            }
            if (stillLoaded) {
                e->hostBound = false;
                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                try { RefreshButton(btn, localSettings); } catch (...) {}
                return;
            }

            RetireEntryPaths(e, true);
            if (btn) {
                try { btn.Unloaded(e->unloadToken); } catch (...) {}
                if (e->loadedAttached) { try { btn.Loaded(e->loadedToken); } catch (...) {} }
                if (e->themeAttached) { try { btn.ActualThemeChanged(e->themeToken); } catch (...) {} }
            }
            std::lock_guard<std::mutex> lock(g_entriesMutex);
            if (!g_entries) return; // reset by Wh_ModBeforeUninit
            g_entries->erase(
                std::remove_if(g_entries->begin(), g_entries->end(), [&](auto& x) { return x == e; }),
                g_entries->end());
        });
    }

    // Loaded fires after a (re)attached button has been measured and
    // arranged — the moment to rebind against the settled tree.
    if (!entry->loadedAttached) {
        entry->loadedAttached = true;
        std::weak_ptr<ButtonEntry> weakLoadedEntry = entry;
        entry->loadedToken = button.Loaded([weakLoadedEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakLoadedEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            e->hostBound = false;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { RefreshButton(btn, localSettings); } catch (...) {}
        });
    }

    // Repaint on theme switches: the theme is the one each button is
    // rendered with, so a per-taskbar override by another mod is honored.
    if (!entry->themeAttached) {
        entry->themeAttached = true;
        std::weak_ptr<ButtonEntry> weakThemeEntry = entry;
        entry->themeToken = button.ActualThemeChanged([weakThemeEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakThemeEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { RefreshButton(btn, localSettings); } catch (...) {}
        });
    }

    // The icon's lookup root: the IconPanel (a Grid with hidden labels, a
    // TaskListLabeledButtonPanel with labels shown), else the button.
    auto iconPanel = FindChildByName(button, L"IconPanel");
    if (!iconPanel) iconPanel = button;

    try { WatchIconPanel(entry, iconPanel, localSettings); } catch (...) {}

    try { UpdateNativeIndicators(button, entry, iconPanel, localSettings); } catch (...) {
        Wh_Log(L"Exception while updating the native indicators");
    }

    try { UpdateIndicator(button, entry, iconPanel, localSettings); } catch (...) {
        Wh_Log(L"Exception while applying the marks of a button");
    }
}

// ---------------------------------------------------------------------------
// Hooks (Taskbar.View.dll): every TaskListButton runs UpdateVisualStates
// when it is created and on every state change, on every taskbar island.
// ---------------------------------------------------------------------------

void DispatchButtonRefresh(FrameworkElement const& elem) {
    auto dispatcher = elem.Dispatcher();
    if (!dispatcher) return;
    auto weakElem = winrt::make_weak(elem);
    PostToUiThread(dispatcher, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            auto button = weakElem.get();
            if (!button) return;
            // Counted when it runs, so a report's numbers add up.
            if (localSettings.DebugLog) g_refreshStats.hook++;
            RefreshButton(button, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in refresh");
        }
    });
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

// Hides the native elements right away, on the UI thread, inside the
// button's own visual-state update: a button created or re-templated (as on
// a window's close) gets new native elements, and waiting for the posted
// refresh let them render for a frame.
void HideNativeElementsNow(FrameworkElement const& button) {
    Settings localSettings;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        if (!g_settings.HideNativeIndicator && !g_settings.HideNativeProgress && !g_settings.BadgeEnabled &&
            !g_settings.MultiEnabled) {
            return;
        }
        localSettings = g_settings;
    }
    auto entry = FindOrCreateEntry(button);
    if (!entry) return;
    auto iconPanel = FindChildByName(button, L"IconPanel");
    if (!iconPanel) iconPanel = button;
    UpdateNativeIndicators(button, entry, iconPanel, localSettings);
    if (localSettings.BadgeEnabled) UpdateNativeBadges(entry, iconPanel, localSettings);
}

// The earliest point a new or re-templated button's native elements exist:
// right after its template is applied, before its first frame renders.
// Optional; without it, UpdateVisualStates does the same a little later.
using TaskListButton_OnApplyTemplate_t = void(WINAPI*)(void*);
TaskListButton_OnApplyTemplate_t TaskListButton_OnApplyTemplate_Original;

void WINAPI TaskListButton_OnApplyTemplate_Hook(void* pThis) {
    TaskListButton_OnApplyTemplate_Original(pThis);
    if (g_unloading) return;
    if (auto elem = GetFrameworkElementFromNative(pThis)) {
        try { HideNativeElementsNow(elem); } catch (...) {}
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_unloading) return;
    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) {
        Wh_Log(L"TaskListButton::UpdateVisualStates: element resolution failed");
        return;
    }
    try { HideNativeElementsNow(elem); } catch (...) {}
    DispatchButtonRefresh(elem);
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            false
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnApplyTemplate(void))"},
            &TaskListButton_OnApplyTemplate_Original,
            TaskListButton_OnApplyTemplate_Hook,
            true, // optional
        },
        // View-model bridge (addresses only, nothing is hooked). All
        // optional: without them the mod is inactive.
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::`vftable')"},
            &TaskListWindowViewModel_ITaskListWindowViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
            &TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &ITaskListWindowViewModel_get_TaskItem,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
            &TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_TaskGroup(void * *))"},
            &TaskListGroupViewModel_get_TaskGroup,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskListGroupViewModel>::`vftable')"},
            &TaskListGroupViewModel_ITaskListGroupViewModel_vftable,
            nullptr,
            true,
        },
        // The pinned state (for the pinned-app dot), on whichever interface
        // has it. All optional.
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_IsPinned(bool *))"},
            &TaskListWindowViewModel_AppItem_get_IsPinned,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_IsPinned(bool *))"},
            &TaskListWindowViewModel_Window_get_IsPinned,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_IsPinned(bool *))"},
            &TaskListGroupViewModel_AppItem_get_IsPinned,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskListGroupViewModel>::get_IsPinned(bool *))"},
            &TaskListGroupViewModel_Group_get_IsPinned,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
    }
    Wh_Log(L"OnApplyTemplate hook: %s", TaskListButton_OnApplyTemplate_Original ? L"available" : L"unavailable");
    Wh_Log(L"View-model bridge: %s",
           IsViewModelBridgeAvailable() ? L"available" : L"unavailable, the mod is inactive");
    Wh_Log(L"Pinned state: %s", IsPinnedStateAvailable()
               ? L"available"
               : L"unavailable (the pinned-app dot shows on pinned apps that aren't running only)");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
    return module;
}

// ---------------------------------------------------------------------------
// Init-time sweep (taskbar.dll): reaches every taskbar island's XamlRoot the
// moment the mod is enabled, so buttons that already exist get their marks
// immediately. The task band of each taskbar window exposes its
// TaskbarHost, which holds the taskbar's root XAML element — the same route
// m417z's taskbar mods use. All lookups are optional: if any symbol is
// missing, existing buttons get their marks on their next state change.
// ---------------------------------------------------------------------------

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

std::atomic<bool> g_taskbarDllSymbolsResolved{false};

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll; init-time sweep unavailable");
        return false;
    }

    // Address lookups only (no hook functions).
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Failed to resolve taskbar.dll symbols; init-time sweep unavailable");
        return false;
    }
    return true;
}

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(::IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                               winrt::put_abi(taskbarElement));
    }

    XamlRoot result = nullptr;
    try { result = taskbarElement ? taskbarElement.XamlRoot() : nullptr; } catch (...) {}

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CSecondaryTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// Collects every TaskListButton under `root` without descending into the
// buttons themselves.
void CollectTaskListButtons(FrameworkElement const& root, std::vector<FrameworkElement>& out, int depth = 0) {
    if (!root || depth > 40) return;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (winrt::get_class_name(child) == L"Taskbar.TaskListButton") {
            out.push_back(child);
            continue;
        }
        CollectTaskListButtons(child, out, depth + 1);
    }
}

using TaskListButtonVisitor = std::function<void(FrameworkElement const&)>;

// Runs on the taskbar thread: every taskbar window (primary and secondary)
// lives on it, so one enumeration reaches every island's task list buttons.
void ForEachTaskListButtonFromTaskbarThread(TaskListButtonVisitor const& visit) {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto const& visit = *reinterpret_cast<TaskListButtonVisitor const*>(lParam);
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) return TRUE;

            XamlRoot xamlRoot = nullptr;
            try {
                if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                    xamlRoot = GetTaskbarXamlRoot(hWnd);
                } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                    xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
                } else {
                    return TRUE;
                }
            } catch (...) {}
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed for %s", szClassName);
                return TRUE;
            }

            try {
                auto content = xamlRoot.Content().try_as<FrameworkElement>();
                if (!content) return TRUE;
                std::vector<FrameworkElement> buttons;
                CollectTaskListButtons(content, buttons);
                for (auto& b : buttons) {
                    try { visit(b); } catch (...) {}
                }
            } catch (...) {
                Wh_Log(L"Exception during the taskbar sweep");
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&visit));
}

void SweepAllTaskbarsFromTaskbarThread() {
    if (g_unloading) return;
    Settings localSettings;
    { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
    ForEachTaskListButtonFromTaskbarThread([&](FrameworkElement const& button) {
        RefreshButton(button, localSettings);
    });
}

// Which kinds of native elements to show again.
struct NativeRestore {
    bool running = false, progress = false, badges = false, multi = false;
    bool Any() const { return running || progress || badges || multi; }
};

// Shows the native elements of every task list button again, the ones no
// entry tracks any more included (see RetireEntryPaths). Runs on the taskbar
// thread.
void RestoreNativeElementsFromTaskbarThread(NativeRestore const& what) {
    ForEachTaskListButtonFromTaskbarThread([&](FrameworkElement const& button) {
        auto iconPanel = FindChildByName(button, L"IconPanel");
        if (!iconPanel) iconPanel = button;
        auto restoreOne = [&](std::wstring_view name, std::wstring_view classPart) {
            auto found = FindChildByName(iconPanel, name);
            if (!found && iconPanel != button) found = FindChildByName(button, name);
            if (!found && !classPart.empty()) found = FindChildByClassPart(button, classPart);
            if (found) SetNativeIndicatorHidden(found, false);
        };
        if (what.running) restoreOne(L"RunningIndicator", L"");
        if (what.progress) restoreOne(L"ProgressIndicator", L"ProgressBar");
        if (what.multi) restoreOne(L"MultiWindowElement", L"");
        if (what.badges) {
            std::vector<FrameworkElement> found;
            CollectBadgeElements(iconPanel, found);
            for (auto& element : found) SetNativeIndicatorHidden(element, false);
        }
    });
}

void SweepAllTaskbars() {
    if (!g_taskbarDllSymbolsResolved || !g_taskbarViewDllLoaded) return;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar window found; marks will appear on the first button activity");
        return;
    }
    RunFromWindowThread(hTaskbarWnd, [](void*) { SweepAllTaskbarsFromTaskbarThread(); }, nullptr);
}

// Unload: takes this mod's host out of every button's icon panel, the ones
// retired in place earlier included, so nothing of the mod stays in the
// taskbar's tree. Its ZIndex is reset first: the host is raised to 1000
// while in use, and removing it with that value is what made XAML reorder
// the icon above its badge. Runs on the taskbar thread.
void RemoveHostsFromTaskbarThread() {
    ForEachTaskListButtonFromTaskbarThread([&](FrameworkElement const& button) {
        auto iconPanel = FindChildByName(button, L"IconPanel");
        auto panel = iconPanel ? iconPanel.try_as<Panel>() : nullptr;
        if (!panel) return;
        std::wstring before = g_debugLog ? PanelOrderText(iconPanel) : std::wstring();
        auto children = panel.Children();
        for (int32_t i = (int32_t)children.Size() - 1; i >= 0; i--) {
            auto element = children.GetAt(i).try_as<FrameworkElement>();
            if (!element || element.Name() != kHostName) continue;
            try {
                ElementCompositionPreview::SetElementChildVisual(element, nullptr);
                Canvas::SetZIndex(element, 0);
                children.RemoveAt(i);
            } catch (...) {}
        }
        if (g_debugLog) {
            Wh_Log(L"[debug] unload, icon panel: [%s] -> [%s]", before.c_str(), PanelOrderText(iconPanel).c_str());
        }
    });
}

struct UnloadCleanup {
    std::vector<std::shared_ptr<ButtonEntry>>* entries = nullptr;
    NativeRestore restore;
};

// Unload, on the taskbar thread (every button and every composition object
// of the mod lives there), synchronously: every entry is retired - entries
// whose button is gone included, so their composition objects are closed
// too and the frame paths release this mod's geometry sources - its
// subscriptions revoked, the native elements shown again, the hosts
// removed and the virtual desktop manager released.
void UnsubscribeColorChanges();  // defined with the system color code

void WINAPI UnloadCleanupOnTaskbarThread(void* param) {
    // RefreshButton subscribes on this thread: catch a first refresh that
    // subscribed after the revoke in Wh_ModBeforeUninit. g_unloading is set
    // by now, so no later refresh subscribes. Safe to call twice.
    UnsubscribeColorChanges();
    auto& cleanup = *static_cast<UnloadCleanup*>(param);
    for (auto& entry : *cleanup.entries) {
        try {
            RetireEntryPaths(entry);
            if (auto btn = entry->button.get()) {
                if (entry->unloadAttached) { try { btn.Unloaded(entry->unloadToken); } catch (...) {} }
                if (entry->loadedAttached) { try { btn.Loaded(entry->loadedToken); } catch (...) {} }
                if (entry->themeAttached) { try { btn.ActualThemeChanged(entry->themeToken); } catch (...) {} }
            }
        } catch (...) {
            Wh_Log(L"Exception during cleanup");
        }
    }
    if (g_taskbarDllSymbolsResolved && g_taskbarViewDllLoaded) {
        try {
            if (cleanup.restore.Any()) RestoreNativeElementsFromTaskbarThread(cleanup.restore);
            RemoveHostsFromTaskbarThread();
        } catch (...) {
            Wh_Log(L"Exception while restoring the taskbars");
        }
    }
    g_virtualDesktopManager = nullptr;
    g_virtualDesktopManagerTried = false;
}

void RestoreNativeElements(NativeRestore what) {
    if (!what.Any()) return;
    if (!g_taskbarDllSymbolsResolved || !g_taskbarViewDllLoaded) {
        Wh_Log(L"Can't reach the taskbars; native elements of closed buttons may stay hidden");
        return;
    }
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) return;
    RunFromWindowThread(hTaskbarWnd, [](void* param) {
        RestoreNativeElementsFromTaskbarThread(*static_cast<NativeRestore*>(param));
    }, &what);
}

// ---------------------------------------------------------------------------
// The WinEvent thread. Window-state events
// (minimize, restore, show, hide, cloak; foreground when it matters;
// maximize/restore seen through location changes) are coalesced into refresh
// passes that re-evaluate every task list button on the taskbar thread.
// Ported from Taskbar Open Window Underline.
// ---------------------------------------------------------------------------

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

#define WM_APP_RECONFIGURE_HOOKS (WM_APP + 1)
#define WM_APP_COLORS_CHANGED (WM_APP + 2)
#define COLOR_REFRESH_DELAY_MS 150
#define INDICATOR_REFRESH_DELAY_MS 60
#define INDICATOR_FOLLOW_UP_DELAY_MS 300

HANDLE g_eventThread = nullptr;
HANDLE g_eventThreadReady = nullptr;
std::atomic<DWORD> g_eventThreadId{0};

// Only touched on the event thread.
UINT_PTR g_refreshTimer = 0;
UINT_PTR g_followUpTimer = 0;
std::vector<HWINEVENTHOOK> g_winEventHooks;
// The last known maximized state of top-level windows, so that of the many
// location changes only the ones that maximize or restore schedule a pass.
std::unordered_map<HWND, bool> g_zoomStates;

// Runs on the taskbar thread: refreshes every task list button entry, which
// re-evaluates its indicator.
void WINAPI RefreshIndicatorsProc(void*) {
    if (g_unloading) return;
    std::vector<std::shared_ptr<ButtonEntry>> entries;
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        if (g_unloading || !g_entries) return;
        entries = *g_entries;
    }
    Settings localSettings;
    { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
    if (localSettings.DebugLog) g_refreshStats.passes++;
    for (auto& e : entries) {
        auto btn = e->button.get();
        if (!btn) continue;
        if (localSettings.DebugLog) g_refreshStats.passButtons++;
        try { RefreshButton(btn, localSettings); } catch (...) {}
    }
}

// Posts a refresh pass to the taskbar thread through a known button's
// dispatcher (every taskbar lives on that thread). With no button known yet
// there is nothing to refresh: the first one is found by the hooks.
void PostRefreshPass() {
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        if (g_unloading || !g_entries) return;
        for (auto& e : *g_entries) {
            if (auto btn = e->button.get()) {
                try { dispatcher = btn.Dispatcher(); } catch (...) {}
                if (dispatcher) break;
            }
        }
    }
    if (dispatcher) PostToUiThread(dispatcher, [] { RefreshIndicatorsProc(nullptr); });
}

void CALLBACK IndicatorRefreshTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    if (id == g_refreshTimer) {
        g_refreshTimer = 0;
        PostRefreshPass();
        // Second pass a bit later: minimize/restore state can still be
        // changing during the first one.
        if (!g_followUpTimer) {
            g_followUpTimer = SetTimer(nullptr, 0, INDICATOR_FOLLOW_UP_DELAY_MS, IndicatorRefreshTimerProc);
        }
    } else if (id == g_followUpTimer) {
        g_followUpTimer = 0;
        PostRefreshPass();
    }
}

// System color changes (see SubscribeColorChangesFromTaskbarThread): one
// refresh of every button a moment after the first change of a burst, from
// this thread, which StopEventThread joins at unload.
bool RefreshAllEntries(bool rebind);
UINT_PTR g_colorRefreshTimer = 0;  // only touched on the event thread

void CALLBACK ColorRefreshTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    if (id == g_colorRefreshTimer) g_colorRefreshTimer = 0;
    if (!g_unloading) {
        try { RefreshAllEntries(false); } catch (...) {}
    }
}

void ArmIndicatorRefreshTimer() {
    if (!g_refreshTimer) {
        g_refreshTimer = SetTimer(nullptr, 0, INDICATOR_REFRESH_DELAY_MS, IndicatorRefreshTimerProc);
    }
}

// Keeps only events that can change a taskbar button: top-level, unowned
// windows that aren't tool windows.
void CALLBACK IndicatorWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    // Any foreground change can move the active color, including one to an
    // owned dialog or a tool window (the previous app's button loses it).
    if (event == EVENT_SYSTEM_FOREGROUND) {
        g_refreshStats.evForeground++;
        ArmIndicatorRefreshTimer();
        return;
    }
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) return;
    if (GetWindow(hwnd, GW_OWNER) && !(exStyle & WS_EX_APPWINDOW)) return;

    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        bool zoomed = IsZoomed(hwnd) != FALSE;
        auto [it, inserted] = g_zoomStates.try_emplace(hwnd, zoomed);
        if (inserted) {
            if (g_zoomStates.size() > 4096) g_zoomStates.clear(); // closed windows are never removed one by one
            if (!zoomed) return;
        } else if (it->second == zoomed) {
            return;
        } else {
            it->second = zoomed;
        }
    }

    switch (event) {
        case EVENT_OBJECT_LOCATIONCHANGE: g_refreshStats.evMaximize++; break;
        case EVENT_OBJECT_SHOW: case EVENT_OBJECT_HIDE: g_refreshStats.evShowHide++; break;
        case EVENT_OBJECT_CLOAKED: case EVENT_OBJECT_UNCLOAKED: g_refreshStats.evCloak++; break;
        default: g_refreshStats.evMinimize++; break;
    }
    ArmIndicatorRefreshTimer();
}

void SeedZoomStates() {
    g_zoomStates.clear();
    EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
        g_zoomStates[hwnd] = IsZoomed(hwnd) != FALSE;
        return TRUE;
    }, 0);
}

void UnhookWinEvents() {
    for (HWINEVENTHOOK hook : g_winEventHooks) UnhookWinEvent(hook);
    g_winEventHooks.clear();
}

// Registers the WinEvent ranges the current settings need: none with the
// indicators off; otherwise minimize, restore, show, hide, cloaking and
// foreground changes always (the foreground picks the active color),
// location changes only when maximized windows are marked.
void HookWinEvents() {
    UnhookWinEvents();

    Settings localSettings;
    { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
    if (!localSettings.AnyEnabled()) {
        g_zoomStates.clear();
        return;
    }

    struct Range { DWORD min, max; };
    std::vector<Range> ranges = {
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    // Foreground changes pick the active color (and, with the foreground
    // window not counted as shown, decide "open" too).
    ranges.push_back({EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND});
    if (localSettings.IndEnabled && localSettings.IndMaximized) {
        SeedZoomStates();
        ranges.push_back({EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE});
    } else {
        g_zoomStates.clear();
    }

    for (const Range& range : ranges) {
        HWINEVENTHOOK hook = SetWinEventHook(range.min, range.max, nullptr, IndicatorWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (hook) {
            g_winEventHooks.push_back(hook);
        } else {
            Wh_Log(L"SetWinEventHook failed for range %04X-%04X", range.min, range.max);
        }
    }
}

DWORD WINAPI IndicatorEventThreadProc(LPVOID) {
    // Force the message queue into existence before signaling readiness, so
    // PostThreadMessage from other threads can't be lost.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_eventThreadId = GetCurrentThreadId();
    SetEvent(g_eventThreadReady);

    HookWinEvents();

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == WM_APP_RECONFIGURE_HOOKS) {
            HookWinEvents();
            continue;
        }
        if (!msg.hwnd && msg.message == WM_APP_COLORS_CHANGED) {
            // The first change of a burst arms the timer; the rest join it.
            if (!g_colorRefreshTimer) {
                g_colorRefreshTimer = SetTimer(nullptr, 0, COLOR_REFRESH_DELAY_MS, ColorRefreshTimerProc);
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_refreshTimer) { KillTimer(nullptr, g_refreshTimer); g_refreshTimer = 0; }
    if (g_followUpTimer) { KillTimer(nullptr, g_followUpTimer); g_followUpTimer = 0; }
    if (g_colorRefreshTimer) { KillTimer(nullptr, g_colorRefreshTimer); g_colorRefreshTimer = 0; }
    UnhookWinEvents();
    return 0;
}

void StartEventThread() {
    if (g_eventThread) return;
    g_eventThreadReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_eventThreadReady) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return;
    }
    g_eventThread = CreateThread(nullptr, 0, IndicatorEventThreadProc, nullptr, 0, nullptr);
    if (!g_eventThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_eventThreadReady);
        g_eventThreadReady = nullptr;
    }
}

// Nothing in the event thread blocks (it posts to the taskbar thread through
// PostToUiThread and never sends), so the waits are unbounded: the thread
// runs code of this DLL and must be gone before it is unmapped.
void StopEventThread() {
    if (!g_eventThread) return;
    WaitForSingleObject(g_eventThreadReady, INFINITE);
    PostThreadMessage(g_eventThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_eventThread, INFINITE);
    CloseHandle(g_eventThread);
    g_eventThread = nullptr;
    g_eventThreadId = 0;
    CloseHandle(g_eventThreadReady);
    g_eventThreadReady = nullptr;
}

void ReconfigureWinEvents() {
    if (DWORD threadId = g_eventThreadId) {
        PostThreadMessage(threadId, WM_APP_RECONFIGURE_HOOKS, 0, 0);
    }
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

// Posts a refresh of every known button to its UI thread; `rebind` also
// re-attaches each to its icon (for settings changes). Returns false when
// there are none.
bool RefreshAllEntries(bool rebind = true) {
    std::vector<std::shared_ptr<ButtonEntry>> localEntries;
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        if (g_entries) localEntries = *g_entries;
    }
    for (auto& entry : localEntries) {
        FrameworkElement btn{nullptr};
        {
            std::lock_guard<std::mutex> lock(g_entriesMutex);
            btn = entry->button.get();
        }
        if (!btn) continue;
        auto dispatcher = btn.Dispatcher();
        if (!dispatcher) continue;
        std::weak_ptr<ButtonEntry> weakEntry = entry;
        PostToUiThread(dispatcher, [weakEntry, rebind]() {
            if (g_unloading) return;
            auto e = weakEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            if (rebind) e->hostBound = false;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { RefreshButton(btn, localSettings); } catch (...) {}
        });
    }
    return !localEntries.empty();
}

// ---------------------------------------------------------------------------
// System colors: the accent color and high contrast can change at any time.
// UISettings reports both (ColorValuesChanged, on a background thread);
// every button is then refreshed, which re-reads them. The handler runs DLL
// code on a thread of its own, so it is counted, and unloading revokes it
// and waits for any run in flight.
// ---------------------------------------------------------------------------

// Released by UnsubscribeColorChanges; kept out of the CRT's destructors
// (see g_d2dFactory).
[[clang::no_destroy]] winrt::Windows::UI::ViewManagement::UISettings g_uiSettings{nullptr};
winrt::event_token g_colorValuesToken{};
std::atomic<int> g_colorHandlersInFlight{0};
// Windows reports one color change several times in a row: the handler only
// tells the event thread, which refreshes once a moment after the first of
// them (ColorRefreshTimerProc). So the handler itself does almost nothing.

void SubscribeColorChangesFromTaskbarThread() {
    try {
        g_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
        g_colorValuesToken = g_uiSettings.ColorValuesChanged([](auto const&, auto const&) {
            g_colorHandlersInFlight.fetch_add(1);
            if (!g_unloading) {
                if (DWORD threadId = g_eventThreadId) PostThreadMessage(threadId, WM_APP_COLORS_CHANGED, 0, 0);
            }
            g_colorHandlersInFlight.fetch_sub(1);
        });
        Wh_Log(L"Subscribed to system color changes");
    } catch (...) {
        Wh_Log(L"Couldn't subscribe to system color changes");
        g_uiSettings = nullptr;
    }
}

std::atomic<bool> g_colorSubscribeTried{false};

void EnsureColorSubscription() {
    if (g_colorSubscribeTried.exchange(true)) return;
    SubscribeColorChangesFromTaskbarThread();
}

void UnsubscribeColorChanges() {
    if (g_uiSettings) {
        try { g_uiSettings.ColorValuesChanged(g_colorValuesToken); } catch (...) {}
    }
    // A handler in flight runs this DLL's code (one PostThreadMessage):
    // wait for it, without a timeout.
    while (g_colorHandlersInFlight.load() > 0) Sleep(10);
    g_uiSettings = nullptr;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar App State Indicators Mod");

    LoadSettings();

    HMODULE m = GetTaskbarViewModuleHandle();
    if (m) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(m)) return FALSE;
    } else {
        HMODULE kb = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = kb ? (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW") : nullptr;
        if (!pLoadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
            Wh_Log(L"Failed to hook LoadLibraryExW");
            return FALSE;
        }
    }

    // Non-fatal: without these, existing buttons get their marks on their
    // next state change.
    g_taskbarDllSymbolsResolved = HookTaskbarDllSymbols();

    // Created only once init can no longer fail.
    g_postsDrained = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"After init");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE m = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar View DLL after init");
                if (HookTaskbarViewDllSymbols(m)) Wh_ApplyHookOperations();
            }
        }
    }

    SweepAllTaskbars();
    StartEventThread();

    // UISettings is created on the taskbar's thread, where WinRT is set up;
    // without a taskbar window yet, the first button refresh does it.
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, [](void*) { EnsureColorSubscription(); }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Uninitializing Taskbar App State Indicators Mod (Before)");

    UnsubscribeColorChanges();

    // The window-event source first: after this no refresh pass of ours can
    // be scheduled, and the thread (which runs this DLL's code) is gone.
    StopEventThread();

    std::vector<std::shared_ptr<ButtonEntry>> localEntries;
    {
        // The flag is set under the entries mutex, and FindOrCreateEntry
        // checks it under the same lock: a callback that raced past an
        // earlier g_unloading check can no longer create an entry AFTER this
        // snapshot.
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        g_unloading = true;
        if (g_entries) localEntries = std::move(*g_entries);
        g_entries.reset();
    }

    // From here no new refresh can be posted (PostToUiThread checks
    // g_unloading under the same mutex), so the pending-post count is
    // monotonically non-increasing.
    if (g_postsDrained) ResetEvent(g_postsDrained);

    // The cleanup runs synchronously on the taskbar thread. Without a
    // taskbar window (Explorer's taskbar gone) it runs here: the XAML parts
    // then fail harmlessly, and the composition objects are closed anyway.
    UnloadCleanup cleanup;
    cleanup.entries = &localEntries;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        cleanup.restore = {g_settings.HideNativeIndicator, g_settings.HideNativeProgress,
                           g_settings.BadgeEnabled, g_settings.MultiEnabled};
    }
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd || !RunFromWindowThread(hTaskbarWnd, UnloadCleanupOnTaskbarThread, &cleanup)) {
        UnloadCleanupOnTaskbarThread(&cleanup);
    }
    localEntries.clear();

    // Refreshes posted before g_unloading was set: each holds a ticket that
    // is released when its lambda is destroyed, whether the dispatcher ran
    // it (it returns at once now) or dropped it. Wait for all of them.
    if (g_pendingPosts.load() > 0 && g_postsDrained) {
        WaitForSingleObject(g_postsDrained, INFINITE);
    }
    // The last ticket is released inside the dispatcher's callback, before
    // C++/WinRT's delegate is torn down. A message sent to the taskbar thread
    // returns only once that thread has processed it, so by then it has left
    // the callback.
    if (hTaskbarWnd) RunFromWindowThread(hTaskbarWnd, [](void*) {}, nullptr);

    // The frame paths' geometry sources are objects of this DLL that
    // composition holds for as long as a path lives. The cleanup above
    // closed every geometry using a path on the taskbar thread, which
    // releases them; wait until the last one is gone.
    ULONGLONG waitStart = GetTickCount64(), lastLog = waitStart;
    while (g_liveGeometrySources.load() > 0) {
        Sleep(10);
        ULONGLONG now = GetTickCount64();
        if (now - lastLog >= 2000) {
            lastLog = now;
            Wh_Log(L"Still waiting for %d frame geometry sources (%llu ms)", g_liveGeometrySources.load(),
                   now - waitStart);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar App State Indicators Mod");
    {
        std::lock_guard<std::mutex> lock(g_d2dFactoryMutex);
        g_d2dFactory = nullptr;
    }
    if (g_postsDrained) {
        CloseHandle(g_postsDrained);
        g_postsDrained = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    NativeRestore wasHidden;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        wasHidden = {g_settings.HideNativeIndicator, g_settings.HideNativeProgress, g_settings.BadgeEnabled,
                     g_settings.MultiEnabled};
    }
    LoadSettings();
    ReconfigureWinEvents();

    // Native elements whose option was just turned off come back everywhere,
    // on buttons no entry tracks any more too.
    {
        NativeRestore turnedOff;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            turnedOff = {wasHidden.running && !g_settings.HideNativeIndicator,
                         wasHidden.progress && !g_settings.HideNativeProgress,
                         wasHidden.badges && !g_settings.BadgeEnabled,
                         wasHidden.multi && !g_settings.MultiEnabled};
        }
        RestoreNativeElements(turnedOff);
    }

    // Re-apply to every known button; buttons that have no entry yet (none,
    // normally — every button is seen at creation) are covered by a sweep.
    if (!RefreshAllEntries()) SweepAllTaskbars();
}
