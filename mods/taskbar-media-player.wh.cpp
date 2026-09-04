// ==WindhawkMod==
// @id            taskbar-media-player
// @name          Taskbar Media Player
// @description   A sleek, floating media player on your taskbar with native volume and playback controls.
// @version       6.7.0
// @author        GR0UD
// @github        https://github.com/GR0UD
// @license       MIT
// @include       explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -lksuser -lwinhttp -ldcomp
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

## 🆕 What's new in 6.7.0
- **Taskbar-native positioning** — Position now attaches the panel to a real
taskbar element (Windows logo, Search, Task View, Widgets, tray, clock, and
more). Side = Before/After reserves real space so the system icons shift
over, like a native widget; Side = Under overlays the anchor element without
reserving anything. If the taskbar structure can't be reached, the panel
falls back to floating at the left edge
- **Smooth resize** — panel width changes glide instead of snapping when the
track text or auto-width changes
- **Smarter Auto Width** — the info area rests at the album-art width and
grows smoothly to fit the track text, up to the max
- **Snappier track changes** — the title/artist crossfade is about 3× faster
- **Windows accent color** — text, control icons, and visualizer bars can
follow the system accent color
- **Per-button toggles** — Previous, Play/Pause, and Next can each be shown
or hidden individually
- **Bigger Play button** — Play/Pause renders larger than its neighbours in
every icon theme, with a toggle to keep all icons equal instead
- **Pixel-perfect icons** — control icons are now centered on their actual
glyph shape, so they sit exactly in the middle of their hover highlight,
horizontally and vertically; a Smooth Icon Rendering toggle picks between
anti-aliased shapes and classic font rendering (centering is exact in both)
- **Title & artist opacity** — independent opacity sliders for both lines
- **More fonts** — Poppins and Inter auto-download, plus a wide list of
system fonts for the title and artist lines
- **Fixes** — the title marquee keeps scrolling while playback is paused,
buttons always control the app shown on the panel (not whichever app
Windows considers current), tracks without album art no longer cause a
repaint every poll, the volume slider scales correctly with DPI and panel
height, and Inline Mode rects are correct on very wide multi-monitor
setups

## 🕘 Previously in 6.6.0
- Taskbar placement presets (Left / Center / Right + offsets), Auto Width,
perfect pill shapes, per-corner radius, widget-style hover highlight,
play/pause on the album art, the "Rounded Filled" icon pack, Blur Strength
on every machine (GPU or software path), instant play/pause icon, and a
batch of stability fixes (Explorer crash on button spam, fullscreen
false-positives, auto-hide timing, visualizer recovery)

---

## ✨ Features

### 🎨 Themes & Appearance
- **12 background themes** — Transparent, Acrylic Glass, Windows Adaptive, Neon
Glass, Sharp Split, Aurora Glow, Album Art Blur, and six gradient modes
- **Working blur control** — Blur Strength drives the real blur radius on
Acrylic Glass, Neon Glass, and Album Art Blur (GPU when available, live
software blur otherwise)
- **Album art adaptive colors** — background gradients pull their palette
straight from the current cover art
- **Auto text color** — automatically switches between white and black text
based on system light/dark mode
- **Dynamic hover color** — button highlights can match the album art's primary
color
- **Rounded corners** — uniform or per-corner radius (0–100), with true pill
shapes at high values
- **Widget-style hover highlight** — Windows 11 (rounded) or Windows 10
(square), adaptive or custom color
- **Optional border**, mask overlay, and per-container debug outlines

### 📐 Layout & Sizing
- **Taskbar-native position** — attach the panel to any of 10 taskbar
elements (Windows logo, Search, tray, clock, …): Before / After reserves
real space and shifts the system icons over, Under overlays the anchor;
X/Y offsets fine-tune every mode
- **Auto Width** — panel hugs the track text up to a configurable max
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
- **Album art** with configurable corner radius (up to a full pill/circle) and
a music-note placeholder
- **Play/Pause overlay on hover** — click the cover to toggle playback
(optional)
- **Supports 16 media sources** or auto-detect all

### ⏯️ Playback Controls
- **Previous / Play-Pause / Next** buttons with instant visual feedback —
each one individually toggleable
- **Hero Play/Pause** — the play button renders larger than its neighbours
(toggleable)
- **Shuffle** and **Repeat** toggle buttons (optional)
- **6 icon themes** — including the new hand-drawn **Rounded Filled** pack
- **Configurable icon size** and **button spacing**

### 🔊 Volume
- **Speaker icon** with a hover-to-expand inline volume slider
- **Mouse wheel volume control** (multi-monitor safe)
- **Per-app volume** — controls the volume of the playing media app only

### 📊 Live Audio Visualizer
- **5 bar shapes**: Stereo, Mountain, Mirror, Wave, Breathe
- **5 color modes**: Solid, Dynamic Album Color, Dynamic Gradient, Custom
Gradient, Acrylic
- **6 EQ presets**: Balanced, Bass, Rock, Pop, Jazz, Electronic
- **3 vertical anchors**: Top, Middle, Bottom
- **Configurable bar count** (1–20), bar width, bar gap, and idle bar height
- **Full-width background mode**
- **Sensitivity control** (0–300)

### 📏 Progress Bar
- **5 position modes**
- **Dynamic color** (matches album art) or custom hex color
- **Smooth live interpolation**

### 🙈 Behavior
- **Auto-hide on fullscreen** — exclusive and borderless fullscreen, with no
multi-monitor false positives
- **Auto-hide on idle** — explicit toggle plus timeout (0 = hide instantly when
nothing plays)
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
    $description: "Only affects themes that have a blur: Acrylic Glass, Neon Glass, and Album Art Blur. Other themes ignore this setting."
  - EnableMask: false
    $name: Mask Overlay
  - MaskOpacity: 120
    $name: Mask Opacity (0-255)
  - CornerRadius: 5
    $name: Corner Radius (0-100)
    $description: "0 = sharp corners. Half the panel height or more = perfect pill shape."
  - CornerUniform: true
    $name: Uniform Corners
    $description: "On: Corner Radius above applies to all four corners. Off: the four per-corner values below are used instead."
  - CornerRadiusTL: 0
    $name: "Corner: Top Left"
  - CornerRadiusTR: 0
    $name: "Corner: Top Right"
  - CornerRadiusBR: 0
    $name: "Corner: Bottom Right"
  - CornerRadiusBL: 0
    $name: "Corner: Bottom Left"
  - ShowBorder: true
    $name: Show Border
  - AdaptiveTheme: true
    $name: Follow Windows Dark/Light Mode
    $description: "Automatically switches text, control icons, and visualizer bars between dark and light. Only applies when colors are still at their default white — set a custom color to opt out."
  - ShowContainerBorders: false
    $name: Show Container Borders (Debug)
    $description: "Draws a colored outline around each active container."
  $name: Appearance

- PanelHoverGroup:
  - PanelHoverEnabled: true
    $name: Enable Hover Highlight
    $description: "Shows a subtle background when you hover the panel — like the Windows taskbar weather widget or clock. Its shape always follows the panel's Corner Radius."
  - PanelHoverColor: ""
    $name: Color (hex RRGGBB)
    $description: "Leave empty for automatic — white in dark mode, black in light mode."
  - PanelHoverOpacity: 5
    $name: Opacity (0-100)
  $name: Panel Hover Highlight

- LayoutGroup:
  - PanelHeight: 45
    $name: Height (px)
  - PanelWidth: 360
    $name: Width (px)
  - PanelPosition: widgets
    $name: Position
    $description: "Which taskbar element the panel attaches to. Windows logo / Search / Task View sit on the app side of the bar; the rest are tray elements. With Side = Before/After, real space is reserved and the system icons shift over. Widgets pins the panel to the bar's far-left corner directly and ignores Side — it works the same whether the Widgets button is shown or hidden. Windows 11 taskbar only — if the taskbar structure can't be reached, the panel falls back to floating at the left edge."
    $options:
      - start: Windows logo
      - search: Search
      - taskview: Task View
      - widgets: Widgets (left corner)
      - tray: Tray (whole)
      - icons: Tray icons
      - chevron: Hidden-icons arrow
      - clock: Clock
      - volume: Network/Volume
      - language: Language
  - PanelSide: after
    $name: Side
    $description: "Before = left of the anchor, After = right of it. Under = on top of the anchor — no space is reserved, the panel simply overlays that element."
    $options:
      - before: Before
      - after: After
      - under: Under (overlap)
  - InlineMarginLeft: 2
    $name: Margin Left (px)
    $description: "Gap between the panel and its neighbours. Before/After only — Under ignores the margins."
  - InlineMarginRight: 2
    $name: Margin Right (px)
  - OffsetX: 0
    $name: Horizontal Offset
  - OffsetY: 0
    $name: Vertical Offset
  - WrapPadTop: 7
    $name: Padding Top
  - WrapPadBottom: 7
    $name: Padding Bottom
  - WrapPadLeft: 10
    $name: Padding Left
  - WrapPadRight: 10
    $name: Padding Right
  - GlobalGap: 16
    $name: Container Gap (px)
  - ContainerOrder: 1234
    $name: Container Order
    $description: "Four-digit code. 1=Media 2=Info 3=Controls 4=Visualizer."
  - SmoothResize: true
    $name: Smooth Resize
    $description: "Animates panel width changes (track text, auto-width) instead of snapping."
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
      - zen: Zen Browser
  - HideFullscreen: true
    $name: Hide on Fullscreen
  - FadeFullscreen: false
    $name: Fade on Fullscreen Hide
  - DisableScroll: false
    $name: Disable Title Scrolling
  - IdleScreenEnabled: false
    $name: Default Mode
    $description: "Shows placeholder title, artist, and art when no media is playing."
  - IdleHideEnabled: false
    $name: Enable Auto-Hide
    $description: "Hides the widget after a period with no media playing."
  - IdleTimeout: 5
    $name: Auto-Hide Timeout (seconds)
    $description: "Seconds with no media playing before the widget hides. Requires Enable Auto-Hide."
  - IdleScreenDelay: 5
    $name: Default Mode Delay (seconds)
    $description: "Seconds after media stops before switching to Default Mode."
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
    $name: Album Art Corners (0-100)
    $description: "Half the art height or more = perfect pill shape."
  - MediaPlaceholderIcon: true
    $name: Show Placeholder Icon
  - MediaHoverPlay: true
    $name: Play/Pause on Hover
    $description: "Hovering the album art shows a play/pause overlay; clicking the art toggles playback."
  $name: Media

- InfoContainerGroup:
  - InfoEnabled: true
    $name: Enabled
  - InfoAutoWidth: true
    $name: Auto Width (fit text)
    $description: "Panel shrinks and grows with the track text, up to Max Width. Longer titles scroll back and forth. Overrides Fixed Width."
  - InfoMaxWidth: 150
    $name: Auto Width Max (px)
  - InfoFixedWidth: false
    $name: Fixed Width
  - InfoWidth: 150
    $name: Fixed Width (px)
  - InfoPadLeft: 0
    $name: Padding Left
  - InfoPadRight: 0
    $name: Show Artist
    $description: "Toggle the artist line on or off."
  - ShowArtist: true
    $name: Show Artist
  - TitleFontSize: 13
    $name: Title Font Size (px)
  - ArtistFontSize: 11
    $name: Artist Font Size (px)
  - TitleOpacity: 100
    $name: Title Opacity (0-100)
  - ArtistOpacity: 50
    $name: Artist Opacity (0-100)
  - TitleFont: inter
    $name: Title Font
    $options:
      - segoe_ui_variable: Segoe UI Variable Display
      - segoe_ui: Segoe UI
      - segoe_ui_semibold: Segoe UI Semibold
      - segoe_ui_bold: Segoe UI Bold
      - segoe_ui_light: Segoe UI Light
      - segoe_ui_semilight: Segoe UI Semilight
      - poppins: Poppins (auto-download)
      - inter: Inter (auto-download)
      - aptos: Aptos
      - aptos_display: Aptos Display
      - calibri: Calibri
      - cambria: Cambria
      - candara: Candara
      - consolas: Consolas
      - corbel: Corbel
      - arial: Arial
      - trebuchet: Trebuchet MS
      - verdana: Verdana
      - tahoma: Tahoma
      - georgia: Georgia
      - times_new_roman: Times New Roman
  - ArtistFont: inter
    $name: Artist Font
    $options:
      - segoe_ui_variable: Segoe UI Variable Display
      - segoe_ui: Segoe UI
      - segoe_ui_semibold: Segoe UI Semibold
      - segoe_ui_bold: Segoe UI Bold
      - segoe_ui_light: Segoe UI Light
      - segoe_ui_semilight: Segoe UI Semilight
      - poppins: Poppins (auto-download)
      - inter: Inter (auto-download)
      - aptos: Aptos
      - aptos_display: Aptos Display
      - calibri: Calibri
      - cambria: Cambria
      - candara: Candara
      - consolas: Consolas
      - corbel: Corbel
      - arial: Arial
      - trebuchet: Trebuchet MS
      - verdana: Verdana
      - tahoma: Tahoma
      - georgia: Georgia
      - times_new_roman: Times New Roman
  - TextGap: 0
    $name: Gap — Title to Artist (px)
    $description: "Negative values overlap the lines closer together."
  - ScrollSpeed: 1
    $name: Scroll Speed
    $options:
      - 0: Slow
      - 1: Normal
      - 2: Fast
  - TextColor: FFFFFF
    $name: Text Color
    $description: "Set a custom color to override the adaptive theme for text."
  - TextAccentColor: false
    $name: Follow Windows Accent Color
    $description: "Uses the Windows accent color for text. Overrides Text Color and adaptive theme."
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
    $description: "Master toggle for the Previous / Play / Next buttons. Use the three toggles below to hide them individually."
  - ShowPrevButton: true
    $name: Show Previous Button
  - ShowPlayButton: true
    $name: Show Play/Pause Button
  - ShowNextButton: true
    $name: Show Next Button
  - BigPlayButton: true
    $name: Bigger Play/Pause Button
    $description: "Renders the Play/Pause icon about 25% larger than the other icons (its hover highlight and click area grow with it). Off: all icons share the same size."
  - ShowShuffleButton: false
    $name: Show Shuffle Button
  - ShowRepeatButton: false
    $name: Show Repeat Button
  - ShowSpeakerIcon: false
    $name: Show Volume Control
  - ScrollVolume: true
    $name: Mouse Wheel Controls Volume
    $description: Scroll anywhere on the taskbar panel to adjust volume. Works even when the Controls container is disabled.
  - IconTheme: rounded_filled
    $name: Icon Theme
    $options:
      - default: Default (Custom Drawn)
      - rounded_filled: Rounded Filled (Modern)
      - mdl2: Segoe MDL2 Assets
      - fluent: Segoe Fluent Icons - Outlined
      - fluent_filled: Segoe Fluent Icons - Filled
      - fluent_outline2: Segoe Fluent Icons - Outlined Alt
  - IconSmoothRendering: true
    $name: Smooth Icon Rendering
    $description: "On: icons are drawn as anti-aliased shapes — geometrically exact, smoothest edges. Off: classic font rendering (grid-fitted, can look crisper at small sizes). Icons stay perfectly centered either way."
  - IconSize: 14
    $name: Icon Size (px)
  - ButtonSpacing: 34
    $name: Icon Gap (px)
  - CtrlIconDynamic: false
    $name: Dynamic Icon Color
  - CtrlIconColor: FFFFFF
    $name: Icon Color
    $description: "Set a custom color to override the adaptive theme for icons."
  - CtrlIconAccentColor: false
    $name: Follow Windows Accent Color
    $description: "Uses the Windows accent color for button icons. Overrides Icon Color and adaptive theme."
  - HoverIconColor: 1ED760
    $name: Icon Hover Color
  - DynamicHover: false
    $name: Dynamic Icon Hover Color
    $description: "Icon hover color follows the album art primary color."
  - HoverBgEnabled: true
    $name: Button Hover Background
    $description: "Shows a background behind hovered buttons."
  - HoverBgRadius: 4
    $name: Hover Background Corner Radius (0-100)
    $description: "0 = sharp square. Half the box size or more = perfect circle."
  - HoverBgOpacity: 2
    $name: Hover Background Opacity (0-100)
  - HoverBgColor: FFFFFF
    $name: Background Hover Color
  - DynamicHoverBg: false
    $name: Dynamic Background Hover Color
    $description: "Hover background color follows the album art primary color."
  $name: Controls

- ProgressBarGroup:
  - ShowProgressBar: false
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
    $description: "Set a custom color to override the adaptive theme for bars (Solid mode only)."
  - VizAccentColor: false
    $name: Follow Windows Accent Color
    $description: "Uses the Windows accent color for visualizer bars (Solid mode only). Overrides Bar Color."
  - VizColor1: 1ED760
    $name: Gradient Color 1
  - VizColor2: 00B4FF
    $name: Gradient Color 2
  - VizBars: 7
    $name: Bar Count (1-20)
  - VizBarWidth: 5
    $name: Bar Width (px)
  - VizBarGap: 5
    $name: Bar Gap (px)
  - IdleBarSize: 15
    $name: Idle Bar Height (%)
  - VizSensitivity: 300
    $name: Sensitivity (0-300)
  $name: Visualizer

- InlineModeGroup:
  - InlineAnchor: tray
    $name: Inline Anchor
  - InlineSide: before
    $name: Inline Side
  - PlateMaterial: acrylic
    $name: Plate Material
  - PlateTint: ""
    $name: Plate Tint
  - PlateTintOpacity: 35
    $name: Plate Tint Opacity
  - PlateMarginLeft: 4
    $name: Plate Margin Left
  - PlateMarginRight: 4
    $name: Plate Margin Right
  $name: Inline Mode

*/
// ==/WindhawkModSettings==

// ============================================================================
//  Includes
// ============================================================================

#include <audioclient.h>
#include <winhttp.h>
#include <audiopolicy.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <mmdeviceapi.h>
#include <shcore.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <windows.h>
#include <dcomp.h>

// mingw-w64's dcomp.h is missing a few Win10 1709+ additions — fill them in.

// IDCompositionBackdropBrush has no methods beyond IUnknown.
struct IDCompositionBackdropBrush : public IUnknown {};

// mingw-w64's IDCompositionDevice3 omits CreateBackdropBrush (the 14th new
// method). Appending it here keeps the vtable layout identical to the real DLL.
struct IDCompositionDevice3Full : public IDCompositionDevice3 {
    virtual HRESULT STDMETHODCALLTYPE CreateBackdropBrush(
        IDCompositionBackdropBrush** backdropBrush) = 0;
};


#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

// Inline Mode (explorer-side XAML plate) dependencies. The XAML namespaces
// are NOT imported wholesale — Gdiplus::Color vs Windows::UI::Color would
// collide — everything is accessed through the wux*/wuxm aliases below.
// GetCurrentTime: winbase.h defines it as a macro, which mangles the XAML
// animation header's method of the same name — suspend it for the includes.
#include <windhawk_utils.h>
#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#pragma pop_macro("GetCurrentTime")

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
// Fully qualified with winrt:: — robuffer.h (pulled in by the XAML headers)
// declares a GLOBAL ::Windows namespace, which makes bare `Windows::`
// ambiguous against winrt::Windows under `using namespace winrt`.
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Storage::Streams;

// ============================================================================
//  Constants
// ============================================================================

static constexpr int VIZ_BARS_MAX = 20;
static constexpr int FFT_SIZE = 1024;
static constexpr int NUM_BANDS = 7;
// IDT_POLL is now a heartbeat only — SMTC events fire FetchMedia instantly.
// 5 s is enough to catch session switches that events don't cover.
static constexpr UINT POLL_RATE_HEARTBEAT = 5000;
static constexpr UINT POS_RATE_PLAYING = 50;

static constexpr WCHAR FONT_NAME[] = L"Segoe UI Variable Display";
static constexpr WCHAR ICON_FONT[] = L"Segoe MDL2 Assets";

// ── Web-font system ───────────────────────────────────────────────────────────────────────
// Each downloadable font is described by one WebFontDef.  LoadWebFont() handles
// the download, memory pinning, and PrivateFontCollection registration for all
// of them -- no per-font boilerplate needed when adding a new typeface.

struct WebFontFace {
    const wchar_t* path;   // URL path on fonts.gstatic.com
    int            style;  // GDI+ FontStyle constant this face is registered as
};

struct WebFontDef {
    const wchar_t*  name;
    WebFontFace     faces[2];  // [0]=Regular [1]=SemiBold
};

// Registry: add a new row here + a MapFontName entry + a settings YAML option.
static constexpr WebFontDef k_WebFonts[] = {
    {
        L"Poppins",
        {
            { L"/s/poppins/v21/pxiEyp8kv8JHgFVrFJDUc1NECPY.ttf",     FontStyleRegular },
            { L"/s/poppins/v21/pxiByp8kv8JHgFVrLGT9V1tvFP-KUEg.ttf", FontStyleBold    },
        }
    },
    {
        L"Inter",
        {
            { L"/s/inter/v13/UcCO3FwrK3iLTeHuS_fvQtMwCp50KnMw2boKoduKmMEVuLyfAZ9hiA.ttf", FontStyleRegular },
            { L"/s/inter/v13/UcCO3FwrK3iLTeHuS_fvQtMwCp50KnMw2boKoduKmMEVuGKZAZ9hiA.ttf", FontStyleBold    },
        }
    },
};

// Per-font runtime state indexed in the same order as k_WebFonts.
struct WebFontState {
    Gdiplus::PrivateFontCollection* collection = nullptr;
    std::vector<BYTE>               faceData[2]; // raw bytes for each face
    bool                            ready      = false;
};
static WebFontState g_WebFonts[std::size(k_WebFonts)];

// Returns the ready WebFontState for a given family name, or nullptr.
static WebFontState* FindWebFont(const std::wstring& name) {
    for (size_t i = 0; i < std::size(k_WebFonts); i++)
        if (name == k_WebFonts[i].name && g_WebFonts[i].ready)
            return &g_WebFonts[i];
    return nullptr;
}

// Downloads a URL via WinHTTP into a byte vector. Returns true on success.
static bool HttpDownloadToBuffer(const wchar_t* host, const wchar_t* path,
                                 std::vector<BYTE>& out) {
    out.clear();
    HINTERNET hSession = WinHttpOpen(L"TaskbarMediaPlayer/1.0",
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    HINTERNET hConn = WinHttpConnect(hSession, host,
                                     INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConn) { WinHttpCloseHandle(hSession); return false; }

    HINTERNET hReq = WinHttpOpenRequest(hConn, L"GET", path, nullptr,
                                        WINHTTP_NO_REFERER,
                                        WINHTTP_DEFAULT_ACCEPT_TYPES,
                                        WINHTTP_FLAG_SECURE);
    if (!hReq) {
        WinHttpCloseHandle(hConn);
        WinHttpCloseHandle(hSession);
        return false;
    }

    bool ok = false;
    if (WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hReq, nullptr)) {
        DWORD statusCode = 0;
        DWORD scSize = sizeof(statusCode);
        WinHttpQueryHeaders(hReq,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            &statusCode, &scSize, WINHTTP_NO_HEADER_INDEX);
        if (statusCode == 200) {
            DWORD read = 0;
            BYTE buf[8192];
            while (WinHttpReadData(hReq, buf, sizeof(buf), &read) && read > 0)
                out.insert(out.end(), buf, buf + read);
            ok = !out.empty();
        }
    }

    WinHttpCloseHandle(hReq);
    WinHttpCloseHandle(hConn);
    WinHttpCloseHandle(hSession);
    return ok;
}

// Downloads all faces for one entry in k_WebFonts and registers them into a
// PrivateFontCollection.  Called from a detached thread per font at startup.
static void LoadWebFont(size_t idx) {
    const WebFontDef&  def   = k_WebFonts[idx];
    WebFontState&      state = g_WebFonts[idx];
    const wchar_t*     host  = L"fonts.gstatic.com";

    static constexpr size_t NUM_FACES = std::size(def.faces);
    bool anyOk = false;
    for (size_t fi = 0; fi < NUM_FACES; fi++) {
        bool ok = HttpDownloadToBuffer(host, def.faces[fi].path, state.faceData[fi]);
        Wh_Log(L"[%s] face %zu: %s (%zu bytes)",
               def.name, fi, ok ? L"OK" : L"FAIL", state.faceData[fi].size());
        if (ok) anyOk = true;
    }

    if (!anyOk) {
        Wh_Log(L"[%s] All downloads failed", def.name);
        return;
    }

    state.collection = new Gdiplus::PrivateFontCollection();
    for (size_t fi = 0; fi < NUM_FACES; fi++) {
        if (!state.faceData[fi].empty())
            state.collection->AddMemoryFont(state.faceData[fi].data(),
                                            (INT)state.faceData[fi].size());
    }

    state.ready = true;
    Wh_Log(L"[%s] Loaded OK", def.name);
}

// Scaling reference — all icon/hit-test math is relative to this panel height.
static constexpr float PANEL_HEIGHT_REF = 52.f;
// Default icon size that ICON_SCALE_REF is relative to.
static constexpr float ICON_SIZE_REF = 14.f;
// Luminance threshold (0-255) below which album art is considered "dark".
static constexpr double LUM_DARK_THRESHOLD = 135.0;
// Pi — used in FFT twiddle factors and Hann window.
static constexpr float PI = 3.14159265f;

// ============================================================================
//  Diagnostic helpers
// ============================================================================

// Replaces silent catch(...) blocks throughout the file.
// In a shipped mod we still swallow exceptions, but now every failure
// leaves a trace in the Windhawk log so subtle bugs are actually findable.
//
// Usage:
//   WH_TRY { ... risky WinRT call ... } WH_CATCH("context description")
//
#define WH_CATCH(ctx)                                                      \
    catch (winrt::hresult_error const& e) {                                \
        Wh_Log(L"[" ctx L"] hresult 0x%08X: %s", (unsigned)e.code().value, \
               e.message().c_str());                                       \
    }                                                                      \
    catch (std::exception const& e) {                                      \
        Wh_Log(L"[" ctx L"] std::exception (see debug log)");              \
        (void)e;                                                           \
    }                                                                      \
    catch (...) {                                                          \
        Wh_Log(L"[" ctx L"] unknown exception");                           \
    }

// Convenience: single-expression try/catch that logs and returns a default.
// Use for expressions that return a value, e.g.:
//   wstring s = WH_TRY_OR(props.Title().c_str(), L"Unknown");
#define WH_TRY_OR(expr, fallback) \
    [&]() noexcept {              \
        try {                     \
            return (expr);        \
        } catch (...) {           \
            return (fallback);    \
        }                         \
    }()

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
    IDT_SOFTBLUR = 1009,  // periodic backdrop re-capture (software blur path)
    IDT_WIDTH_ANIM = 1010,  // smooth panel width easing
};

// WM_APP sub-messages
static constexpr UINT WM_APPCLOSE = WM_APP;
static constexpr UINT WM_TASKBAR_MOVED = WM_APP + 10;
// Posted by DrawInfoContainer when the measured text width changed and the
// info container is in auto-width mode — resizing mid-paint is unsafe, so
// the relayout happens on the next message pump iteration.
static constexpr UINT WM_INFO_RELAYOUT = WM_APP + 11;

// Context menu command IDs
static constexpr UINT IDM_REFRESH_THEME = 9001;
static constexpr UINT IDM_REFRESH_MEDIA = 9002;
static constexpr UINT IDM_PLAYPAUSE = 9003;
static constexpr UINT IDM_PREV_TRACK = 9004;
static constexpr UINT IDM_NEXT_TRACK = 9005;
static constexpr UINT IDM_MINI_MODE = 9006;
static constexpr UINT IDM_FOCUS_APP = 9007;

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

enum class Theme {
    Windows,
    Transparent,
    Blurred,
    GradientFade,
    GradientLR,
    GradientVertical,
    GradientRadial,
    GradientVertInv,
    Split,
    Neon,
    Aurora,
    GradientRL,
    AlbumBlur,
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
    FluentOutlined2,
    // Custom-drawn like Default, but with round joins/caps everywhere.
    // Keep at the end: the glyph branch tests `>= FluentOutlined` for font
    // selection, and this theme never reaches that branch.
    RoundedFilled,
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
    // Positioning is taskbar-native: the explorer-side spacer streams the
    // anchored rect (LayoutGroup.PanelPosition picks the anchor element,
    // read explorer-side only). The panel falls back to floating at the
    // taskbar's left edge while the rect isn't streaming.
    bool smoothResize = true;
    int offsetX = 0;
    int offsetY = 0;

    // ── Adaptive theme (master dark/light toggle)
    // ─────────────────────────────────────────
    bool adaptiveTheme = true;

    // ── Windows accent color follows ──────────────────────────────────────────
    bool textAccentColor = false;
    bool ctrlIconAccentColor = false;
    bool vizAccentColor = false;

    // ── Appearance
    // ────────────────────────────────────────────────────────────
    Theme theme = Theme::Transparent;
    int themeOpacity = 200;
    int themeBlur = 60;
    int cornerRadius = 0;
    bool cornerUniform = true;
    int cornerTL = 0;
    int cornerTR = 0;
    int cornerBR = 0;
    int cornerBL = 0;
    bool showBorder = false;
    bool showContainerBorders = false;
    bool enableMask = false;
    int maskOpacity = 120;
    DWORD manualTextColor = 0xFFFFFFFF;
    bool panelHoverEnabled = false;
    DWORD panelHoverColor = 0xFFFFFFFF;
    int panelHoverOpacity = 10;
    bool dynamicHover = false;
    DWORD hoverIconColor = 0xFF1ED760;
    bool hoverBgEnabled = true;
    int hoverBgRadius = 4;
    int hoverBgOpacity = 2;
    DWORD hoverBgColor = 0xFFFFFFFF;
    bool dynamicHoverBg = false;

    // ── Media container
    // ───────────────────────────────────────────────────────
    bool mediaEnabled = true;
    int mediaCornerRadius = 6;
    int mediaWidth = 0;
    bool mediaAutoSize = true;
    bool mediaPlaceholderIcon = true;
    bool mediaHoverPlay = false;

    // ── Info container
    // ────────────────────────────────────────────────────────
    bool infoEnabled = true;
    bool infoAutoWidth = false;
    int infoMaxWidth = 200;
    bool showArtist = true;
    int titleFontSize = 13;
    int artistFontSize = 11;
    int titleOpacity = 100;
    int artistOpacity = 50;
    std::wstring titleFont = L"Segoe UI Variable Display";
    std::wstring artistFont = L"Segoe UI Semibold";
    int textGap = 0;
    ScrollSpeed scrollSpeed = ScrollSpeed::Normal;
    bool disableScroll = false;

    // ── Controls container
    // ────────────────────────────────────────────────────
    bool controlsEnabled = true;
    // Effective per-button visibility — the ShowPlaybackControls master
    // toggle is ANDed in at load time, so every icon has its own on/off.
    bool showPrevBtn = true;
    bool showPlayBtn = true;
    bool showNextBtn = true;
    bool showShuffleButton = false;
    bool showRepeatButton = false;
    bool showSpeakerIcon = false;
    bool scrollVolume = true;
    IconTheme iconTheme = IconTheme::MDL2;
    bool bigPlayButton = true;
    bool iconSmoothRendering = true;
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
    bool idleHideEnabled = false;
    int idleTimeout = 5;
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
    int blurLevel = -1;

    void Invalidate() {
        delete bitmap;
        bitmap = nullptr;
        width = height = 0;
        artVersion = -1;
        blurLevel = -1;
    }

    ~BlurCache() { Invalidate(); }
};

struct ThemeCache {
    bool isLight = false;
    ULONGLONG tickStamp = 0;
};

struct ScreenCapCache {
    Bitmap* bitmap = nullptr;
    int width = 0, height = 0, blurLevel = -1;
    int winX = INT_MIN, winY = INT_MIN;

    void Invalidate() {
        delete bitmap; bitmap = nullptr;
        width = height = blurLevel = 0;
        winX = winY = INT_MIN;
    }
    ~ScreenCapCache() { Invalidate(); }
};

// ── Text crossfade ──────────────────────────────────────────────────────────
// Owns the contract between FetchMedia and IDT_TEXT_FADE.
//
// Ownership rules (all on the message-loop thread — no lock needed):
//   FetchMedia  → calls RequestSwap()  to arm a pending transition.
//   IDT_TEXT_FADE → calls Tick() each frame; commits the swap when alpha==0;
//                   clears fadeOut once the fade-in completes.
//   DrawInfoContainer → reads alpha and whichever title g_M currently holds.
//
// g_M.title / g_M.artist are ONLY written by CommitPending() (called from
// Tick), never by FetchMedia directly.  This removes the implicit coupling
// where FetchMedia needed to know whether an animation was already running.
struct TextCrossfade {
    float alpha = 1.f;     // 1=fully visible, 0=fully hidden
    bool fadeOut = false;  // true during fade-out; false during fade-in
    bool active = false;   // any crossfade phase in progress

    wstring pendingTitle;
    wstring pendingArtist;

    // Called by FetchMedia when a track change is detected.
    // Idempotent: re-arming for the same pending title is a no-op.
    void RequestSwap(const wstring& newTitle, const wstring& newArtist) {
        if (fadeOut && pendingTitle == newTitle)
            return;  // already fading to this title
        pendingTitle = newTitle;
        pendingArtist = newArtist;
        fadeOut = true;
        active = true;
    }

    // Called once per IDT_TEXT_FADE tick.
    // Returns true if a repaint is needed.
    // outCommit is set to {title,artist} when the swap should be written to
    // g_M.
    struct CommitData {
        wstring title, artist;
    };
    bool Tick(CommitData* outCommit) {
        // 0.2/frame at 16ms ≈ 80ms out + 80ms in — snappy track changes.
        // (Was 0.12 ≈ 270ms round trip, which read as "slow to update".)
        static constexpr float STEP = 0.2f;
        if (fadeOut) {
            alpha = max(0.f, alpha - STEP);
            if (alpha <= 0.f) {
                alpha = 0.f;
                fadeOut = false;
                // Signal caller to commit the pending strings to g_M.
                if (outCommit) {
                    outCommit->title = pendingTitle;
                    outCommit->artist = pendingArtist;
                }
            }
        } else {
            alpha = min(1.f, alpha + STEP);
            if (alpha >= 1.f) {
                alpha = 1.f;
                active = false;  // crossfade complete
            }
        }
        return true;  // always repaint while active
    }
};

struct AnimState {
    TextCrossfade crossfade;  // owns the full text-swap lifecycle

    // NOTE: hoverProgress and the window fade fields are accessed ONLY from
    // the message-loop thread (timer callbacks + WndProc).  No mutex is
    // needed; document this explicitly so reviewers don't add one later.
    float hoverProgress[7] = {};  // index 1-6, message-loop-only
    float panelHover = 0.f;       // whole-panel hover highlight progress
    float artHover = 0.f;         // album-art play/pause overlay progress
    bool hoverAnimRunning = false;

    float fadeAlpha = 1.f;  // message-loop-only
    bool fadeIn = true;
    bool fadeActive = false;

    // Smooth-resize eased panel width (logical px), message-loop-only.
    // File-scope state, not a function-local static, for the same lifecycle
    // visibility reasons g_BreatheEnv was hoisted.
    float widthEase = 0.f;
};

struct ScrollState {
    int offset = 0;
    int textW = 0;
    int visW = 0;
    bool active = false;
    bool forward = true;
    int waitTicks = 80;
    int tickDiv = 0;  // IDT_ANIM runs at 16ms; marquee advances every 3rd tick
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
static ScreenCapCache g_ScreenCapCache;

// ── DirectComposition backdrop blur ──────────────────────────────────────────
static IDCompositionDesktopDevice*    g_DCompDevice   = nullptr;
static IDCompositionDevice3Full*      g_DCompDevice3  = nullptr;
static IDCompositionTarget*           g_DCompTarget   = nullptr;
static IDCompositionVisual2*          g_DCompVisual   = nullptr;
static IDCompositionGaussianBlurEffect* g_DCompBlur   = nullptr;
static IDCompositionBackdropBrush*    g_DCompBackdrop = nullptr;
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
// Timestamps (GetTickCount64), 0 = condition not active. These are real-time
// based, NOT tick-counted: SMTC event handlers PostMessage(WM_TIMER, IDT_POLL)
// to refresh instantly, so IDT_POLL can fire many times within one heartbeat
// interval. Counting "ticks * 5s" inflated idle time and made auto-hide /
// idle-screen trigger far too early.
static ULONGLONG g_IdleSince = 0;     // last transition to "not playing"
static ULONGLONG g_NoMediaSince = 0;  // last transition to "no media"
static bool g_IdleHide = false;
static bool g_IdleState = false;
static bool g_FullscreenHidden = false;

// Hover tracking (which button the cursor is over, 0=none)
static MediaCmd g_HoverBtn = MediaCmd::None;
// Whole-panel hover (for the widget-style hover highlight)
static bool g_PanelHovered = false;
// Cursor over the album art (for the hover play/pause overlay)
static bool g_ArtHovered = false;

// Animation timer running flags
static bool g_AnimTimerRunning = false;
static bool g_VizTimerRunning = false;

// WinEvent hook for taskbar moves
static HWINEVENTHOOK g_TbHook = nullptr;
// Registered message sent by the shell when Explorer (and the taskbar) is
// recreated — e.g. after a crash.  We use it to re-hook the taskbar.
static UINT g_TbCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

// ── Inline Mode bridge (panel side) ─────────────────────────────────────────
// The explorer-side plate posts its screen rect through this registered
// message (same name registered in both processes → same id). wParam packs
// x|y, lParam packs w|h — one signed 32-bit value per half (both processes
// are x64, so the params are 64-bit). Full virtual-desktop range: the old
// signed-short packing corrupted rects on multi-monitor setups whose
// coordinates exceed ±32767. While fresh, the rect overrides the normal
// Position anchoring in UpdateBoundsAndRegion.
static UINT g_InlineRectMsg =
    RegisterWindowMessageW(L"TaskbarMediaPlayer_InlineRect");
struct InlineRectState {
    int x = 0, y = 0, w = 0, h = 0;
    ULONGLONG tick = 0;  // 0 = never received
};
static InlineRectState g_InlineRect;
// Fresh = the explorer-side spacer is alive and streaming. Goes stale (and
// the panel falls back to the left edge) within 5s of the stream stopping.
static bool InlineRectFresh() {
    return g_InlineRect.tick &&
           GetTickCount64() - g_InlineRect.tick < 5000;
}

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

// IsSystemLight() — single authoritative dark/light query.
// All code that needs to know the Windows color scheme calls this function.
// Results are cached for 5 seconds to avoid hammering the registry; the cache
// is invalidated immediately on WM_SETTINGCHANGE so changes feel instant.
static bool IsSystemLight() {
    ULONGLONG now = GetTickCount64();
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

// Returns the current Windows accent color as opaque GDI+ ARGB (0xFFRRGGBB).
// DwmGetColorizationColor gives 0xAARRGGBB; we force full alpha.
static DWORD GetWindowsAccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque)))
        return 0xFF000000 | (color & 0x00FFFFFF);
    return 0xFF0078D4;  // Windows blue fallback
}

// Forces a re-check on the next call (call after WM_SETTINGCHANGE).
static void InvalidateLightCache() {
    g_ThemeCache.tickStamp = 0ULL;
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

// Desired info content width (logical px) measured from the current title /
// artist strings by DrawInfoContainer. Only meaningful in auto-width mode.
static int g_InfoDesiredW = 0;

static int CalcInfoWidth() {
    if (g_US.infoAutoWidth) {
        const auto& c = g_US.containers[1];
        // Resting baseline = the album art width: the info area never gets
        // narrower than the media cell, then smoothly grows to fit longer
        // text (capped at infoMaxWidth, where the marquee takes over).
        int floorW = 24;
        if (g_US.mediaEnabled) {
            int aS, aW;
            CalcArtDims(g_US.panelHeight, aS, aW);
            floorW = max(floorW, aW);
        }
        int w = ClampInt(g_InfoDesiredW, floorW,
                         max(floorW, g_US.infoMaxWidth));
        return c.padLeft + w + c.padRight;
    }
    if (g_US.wrap.infoFixedWidth) {
        const auto& c = g_US.containers[1];
        return c.padLeft + g_US.wrap.infoWidth + c.padRight;
    }
    return 0;  // flexible: filled by RecomputeLayout
}

static int CalcControlsWidth() {
    int btnCount = 0;
    if (g_US.showPrevBtn)
        btnCount += 1;
    if (g_US.showPlayBtn)
        btnCount += 1;
    if (g_US.showNextBtn)
        btnCount += 1;
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
    float iconScale = (g_US.iconSize / ICON_SIZE_REF) * scale;
    float iconHalf = ICON_SIZE_REF * iconScale / 2.f;
    // Width = gaps between icon centers + one icon-half margin on each side
    // so the outermost icon glyphs sit flush inside the container edges.
    int gaps = (btnCount <= 1) ? 0 : (btnCount - 1);
    int w = (int)(gaps * step + 2.f * iconHalf);
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
                      (g_US.showPrevBtn || g_US.showPlayBtn ||
                       g_US.showNextBtn || g_US.showSpeakerIcon ||
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

    if (!g_ContainerEnabled[2]) {
        // Info is disabled — width must be exactly 0 so RebuildLayout and
        // UpdateBoundsAndRegion agree and no phantom space is allocated.
        g_ContainerWidths[2] = 0;
        // Recompute panel width from the actually-enabled containers only.
        int total = g_US.wrap.padLeft + g_US.wrap.padRight + totalGaps;
        for (int id : g_US.wrap.order) {
            if (!IsContainerEnabled(id))
                continue;
            if (id == 4 && g_US.wrap.vizAsBackground)
                continue;
            total += GetContainerWidth(id);
        }
        g_PanelWidth = total;
    } else if (g_US.wrap.infoFixedWidth || g_US.infoAutoWidth) {
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

    s.smoothResize = Wh_GetIntSetting(L"LayoutGroup.SmoothResize") != 0;

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

static void LoadAdaptiveThemeSettings(UserSettings& s) {
    s.adaptiveTheme =
        Wh_GetIntSetting(L"AppearanceGroup.AdaptiveTheme") != 0;
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
    s.cornerRadius =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.CornerRadius"), 0, 100);
    s.cornerUniform =
        Wh_GetIntSetting(L"AppearanceGroup.CornerUniform") != 0;
    s.cornerTL =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.CornerRadiusTL"), 0, 100);
    s.cornerTR =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.CornerRadiusTR"), 0, 100);
    s.cornerBR =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.CornerRadiusBR"), 0, 100);
    s.cornerBL =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.CornerRadiusBL"), 0, 100);
    s.showBorder = Wh_GetIntSetting(L"AppearanceGroup.ShowBorder") != 0;
    s.showContainerBorders =
        Wh_GetIntSetting(L"AppearanceGroup.ShowContainerBorders") != 0;
    s.enableMask = Wh_GetIntSetting(L"AppearanceGroup.EnableMask") != 0;
    s.maskOpacity =
        ClampInt(Wh_GetIntSetting(L"AppearanceGroup.MaskOpacity"), 0, 255);

    s.panelHoverEnabled =
        Wh_GetIntSetting(L"PanelHoverGroup.PanelHoverEnabled") != 0;
    // Default white doubles as the "automatic" sentinel (same convention as
    // the text/control colors): white on dark mode, black on light mode.
    s.panelHoverColor =
        ReadHexColor(L"PanelHoverGroup.PanelHoverColor", 0xFFFFFF);
    s.panelHoverOpacity =
        ClampInt(Wh_GetIntSetting(L"PanelHoverGroup.PanelHoverOpacity"), 0, 100);
}

static void LoadMediaSettings(UserSettings& s) {
    s.mediaEnabled = Wh_GetIntSetting(L"MediaContainerGroup.MediaEnabled") != 0;
    s.mediaCornerRadius = ClampInt(
        Wh_GetIntSetting(L"MediaContainerGroup.MediaCornerRadius"), 0, 100);
    s.mediaWidth = max(0, Wh_GetIntSetting(L"MediaContainerGroup.MediaWidth"));
    s.mediaAutoSize =
        Wh_GetIntSetting(L"MediaContainerGroup.MediaAutoSize") != 0;
    s.mediaPlaceholderIcon =
        Wh_GetIntSetting(L"MediaContainerGroup.MediaPlaceholderIcon") != 0;
    s.mediaHoverPlay =
        Wh_GetIntSetting(L"MediaContainerGroup.MediaHoverPlay") != 0;
    s.containers[0] = {
        1, s.mediaEnabled,
        max(0, Wh_GetIntSetting(L"MediaContainerGroup.MediaPadLeft")),
        max(0, Wh_GetIntSetting(L"MediaContainerGroup.MediaPadRight"))};
}

// Maps a font setting string to a GDI+ font family name.
static std::wstring MapFontName(const wchar_t* key, const wchar_t* defaultName) {
    PCWSTR val = Wh_GetStringSetting(key);
    std::wstring result = defaultName;
    if (val) {
        if      (!wcscmp(val, L"segoe_ui_variable")) result = L"Segoe UI Variable Display";
        else if (!wcscmp(val, L"segoe_ui"))          result = L"Segoe UI";
        else if (!wcscmp(val, L"segoe_ui_semibold")) result = L"Segoe UI Semibold";
        else if (!wcscmp(val, L"segoe_ui_bold"))     result = L"Segoe UI Bold";
        else if (!wcscmp(val, L"segoe_ui_light"))    result = L"Segoe UI Light";
        else if (!wcscmp(val, L"segoe_ui_semilight")) result = L"Segoe UI Semilight";
        else if (!wcscmp(val, L"poppins"))           result = L"Poppins";
        else if (!wcscmp(val, L"inter"))             result = L"Inter";
        else if (!wcscmp(val, L"aptos"))             result = L"Aptos";
        else if (!wcscmp(val, L"aptos_display"))     result = L"Aptos Display";
        else if (!wcscmp(val, L"calibri"))           result = L"Calibri";
        else if (!wcscmp(val, L"cambria"))           result = L"Cambria";
        else if (!wcscmp(val, L"candara"))           result = L"Candara";
        else if (!wcscmp(val, L"consolas"))          result = L"Consolas";
        else if (!wcscmp(val, L"corbel"))            result = L"Corbel";
        else if (!wcscmp(val, L"arial"))             result = L"Arial";
        else if (!wcscmp(val, L"trebuchet"))         result = L"Trebuchet MS";
        else if (!wcscmp(val, L"verdana"))           result = L"Verdana";
        else if (!wcscmp(val, L"tahoma"))            result = L"Tahoma";
        else if (!wcscmp(val, L"georgia"))           result = L"Georgia";
        else if (!wcscmp(val, L"times_new_roman"))   result = L"Times New Roman";
        Wh_FreeStringSetting(val);
    }
    return result;
}

static void LoadInfoSettings(UserSettings& s) {
    s.infoEnabled = Wh_GetIntSetting(L"InfoContainerGroup.InfoEnabled") != 0;
    s.showArtist  = Wh_GetIntSetting(L"InfoContainerGroup.ShowArtist")  != 0;
    s.infoAutoWidth =
        Wh_GetIntSetting(L"InfoContainerGroup.InfoAutoWidth") != 0;
    s.infoMaxWidth =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.InfoMaxWidth"), 40, 2000);
    s.wrap.infoFixedWidth =
        Wh_GetIntSetting(L"InfoContainerGroup.InfoFixedWidth") != 0;
    s.wrap.infoWidth =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.InfoWidth"), 40, 2000);
    s.titleFontSize =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.TitleFontSize"), 7, 48);
    s.artistFontSize =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.ArtistFontSize"), 7, 48);
    s.titleOpacity =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.TitleOpacity"), 0, 100);
    s.artistOpacity =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.ArtistOpacity"), 0, 100);
    s.textGap =
        ClampInt(Wh_GetIntSetting(L"InfoContainerGroup.TextGap"), -20, 40);
    s.scrollSpeed = (ScrollSpeed)ClampInt(
        Wh_GetIntSetting(L"InfoContainerGroup.ScrollSpeed"), 0, 2);
    s.manualTextColor = ReadHexColor(L"InfoContainerGroup.TextColor", 0xFFFFFF);
    s.textAccentColor =
        Wh_GetIntSetting(L"InfoContainerGroup.TextAccentColor") != 0;

    // Map font keys to actual font family names (handles "poppins" -> "Poppins")
    s.titleFont  = MapFontName(L"InfoContainerGroup.TitleFont",  L"Segoe UI Variable Display");
    s.artistFont = MapFontName(L"InfoContainerGroup.ArtistFont", L"Segoe UI Semibold");

    s.containers[1] = {
        2, s.infoEnabled,
        max(0, Wh_GetIntSetting(L"InfoContainerGroup.InfoPadLeft")),
        max(0, Wh_GetIntSetting(L"InfoContainerGroup.InfoPadRight"))};
}

static void LoadControlsSettings(UserSettings& s) {
    s.controlsEnabled =
        Wh_GetIntSetting(L"ControlsContainerGroup.ControlsEnabled") != 0;
    bool playbackMaster =
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowPlaybackControls") != 0;
    s.showPrevBtn =
        playbackMaster &&
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowPrevButton") != 0;
    s.showPlayBtn =
        playbackMaster &&
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowPlayButton") != 0;
    s.showNextBtn =
        playbackMaster &&
        Wh_GetIntSetting(L"ControlsContainerGroup.ShowNextButton") != 0;
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
    s.ctrlIconAccentColor =
        Wh_GetIntSetting(L"ControlsContainerGroup.CtrlIconAccentColor") != 0;
    s.dynamicHover =
        Wh_GetIntSetting(L"ControlsContainerGroup.DynamicHover") != 0;
    s.hoverIconColor = ReadHexColor(L"ControlsContainerGroup.HoverIconColor", 0x1ED760);
    s.hoverBgEnabled =
        Wh_GetIntSetting(L"ControlsContainerGroup.HoverBgEnabled") != 0;
    s.hoverBgRadius =
        ClampInt(Wh_GetIntSetting(L"ControlsContainerGroup.HoverBgRadius"), 0, 100);
    s.hoverBgOpacity =
        ClampInt(Wh_GetIntSetting(L"ControlsContainerGroup.HoverBgOpacity"), 0, 100);
    s.hoverBgColor = ReadHexColor(L"ControlsContainerGroup.HoverBgColor", 0xFFFFFF);
    s.dynamicHoverBg =
        Wh_GetIntSetting(L"ControlsContainerGroup.DynamicHoverBg") != 0;
    s.bigPlayButton =
        Wh_GetIntSetting(L"ControlsContainerGroup.BigPlayButton") != 0;
    s.iconSmoothRendering =
        Wh_GetIntSetting(L"ControlsContainerGroup.IconSmoothRendering") != 0;
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
        else if (!wcscmp(it, L"rounded_filled"))
            s.iconTheme = IconTheme::RoundedFilled;
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
    s.vizAccentColor =
        Wh_GetIntSetting(L"VisualizerContainerGroup.VizAccentColor") != 0;
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
        Wh_GetIntSetting(L"VisualizerContainerGroup.VizSensitivity"), 0, 300);
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
    s.idleHideEnabled =
        Wh_GetIntSetting(L"BehaviorGroup.IdleHideEnabled") != 0;
    s.idleTimeout =
        ClampInt(Wh_GetIntSetting(L"BehaviorGroup.IdleTimeout"), 0, 86400);
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
    LoadAdaptiveThemeSettings(s);
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
        // Clear the running flag on early exit, or StartCaptureThread would
        // forever believe a capture is active and never retry.
        g_CaptureRunning.store(false);
        CoUninitialize();
        return;
    }

    com_ptr<IMMDevice> pDev;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, pDev.put()))) {
        g_CaptureRunning.store(false);
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

            // t_sens=1.0 at slider 100 matches original behaviour.
            // Above 100 the gain grows linearly so 200 and 300 give real boosts
            // without going absurdly high.
            float t_sens = g_US.vizSensitivity / 100.0f;  // 0..3
            float sliderGain = (t_sens <= 1.0f)
                ? 0.25f + t_sens * t_sens * 2.75f          // original curve 0-100
                : 3.0f  + (t_sens - 1.0f) * 4.0f;         // linear boost 100-300
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
    // Reap a previously exited thread (e.g. the audio device was missing and
    // the proc bailed early). Assigning to a still-joinable std::thread calls
    // std::terminate — which here means crashing Explorer.
    if (g_CaptureThread.joinable())
        g_CaptureThread.join();
    if (!g_hCaptureEvent)
        g_hCaptureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_CaptureRunning.store(true);
    g_CaptureThread = thread(CaptureThreadProc);
}

static void StopCaptureThread() {
    // Joinable-based, not flag-based: the thread may have exited on its own
    // (clearing the flag) but still needs joining to release its handle.
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
// Shared envelope for the Breathe shape — file-scope so its lifecycle is
// visible and we can reset it alongside g_VizPeak/g_VizTarget. Previously a
// function-local static, which leaked across shape switches.
static float g_BreatheEnv = 0.f;

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
                if (i == 0) {
                    float k = (masterPeak > g_BreatheEnv) ? 0.04f : 0.015f;
                    g_BreatheEnv += (masterPeak - g_BreatheEnv) * k;
                }
                float rate = 0.55f + VIZ_SEEDS[i % VIZ_BARS_MAX] * 0.18f;
                float inhale =
                    0.5f +
                    0.5f * sinf(t * rate + VIZ_SEEDS[i % VIZ_BARS_MAX] * 1.2f);
                target = max(0.f, min(1.f,
                                      inhale * (0.12f + g_BreatheEnv * 0.88f)));
                break;
            }
        }

        g_VizTarget[i] = max(idleFloor, min(1.f, target));
    }
}

// ============================================================================
//  Media state
// ============================================================================

// Lock discipline for MediaSnap:
//   - FetchMedia runs on the message-loop thread and holds mtx while writing
//     title, artist, playing, hasMedia, shuffleOn, repeatMode.
//   - art, primaryColor, secondaryColor, isDarkCover are written only by
//     FetchMedia and read only by DrawPanel — both on the message-loop thread
//     — so they don't need the lock.  This is annotated per-field below.
//   - The crossfade commit (title/artist swap at alpha==0) also runs on the
//     message-loop thread and does take the lock for consistency with readers
//     that come from timer callbacks fired on the same thread.
//
// Because everything is message-loop-only the mutex never actually contends;
// it exists as a correctness backstop against future off-thread readers.
struct MediaSnap {
    // ── Lock-protected fields (use g_M.mtx) ─────────────────────────────────
    wstring title = L"No Media";  // current displayed title
    wstring artist = L"";
    bool playing = false;
    bool hasMedia = false;
    bool shuffleOn = false;
    int repeatMode = 0;  // 0=None 1=Track 2=List
    INT64 timelinePos = 0;
    INT64 timelineEnd = 0;
    mutex mtx;

    // ── Message-loop-only fields (no lock required) ──────────────────────────
    unique_ptr<Bitmap> art;                         // ml-only
    Color primaryColor = Color(255, 18, 18, 18);    // ml-only
    Color secondaryColor = Color(255, 45, 45, 45);  // ml-only
    bool isDarkCover = true;                        // ml-only
} g_M;

// RAII mutex lock for g_M.  Both flavours are exclusive locks (g_M.mtx is a
// plain mutex, not a shared_mutex) — the Read/Write names express intent, not
// lock kind.  MediaReader returns const so the compiler flags accidental writes.
struct MediaReader {
    explicit MediaReader() { g_M.mtx.lock(); }
    ~MediaReader() { g_M.mtx.unlock(); }
    const MediaSnap* operator->() const { return &g_M; }
};
struct MediaWriter {
    explicit MediaWriter() { g_M.mtx.lock(); }
    ~MediaWriter() { g_M.mtx.unlock(); }
    MediaSnap* operator->() { return &g_M; }
};

static wstring g_currentMediaAppAumid;

// Session manager — held for the duration of the mod.
static GlobalSystemMediaTransportControlsSessionManager g_Mgr = nullptr;

// ── SMTC event subscription state ────────────────────────────────────────────
// We subscribe to the three per-session events so UI updates are instant
// instead of waiting for IDT_POLL (which is kept as a 5s fallback heartbeat
// for session-switch detection).  Tokens are revoked before subscribing to a
// new session to avoid double-firing on the old one.
static GlobalSystemMediaTransportControlsSession g_SmtcSession = nullptr;
static winrt::event_token g_SmtcPropToken{};
static winrt::event_token g_SmtcPlayToken{};
static winrt::event_token g_SmtcTimeToken{};
static winrt::event_token g_SmtcMgrSessionToken{};

static void SmtcUnsubscribeSession() {
    if (g_SmtcSession) {
        try { g_SmtcSession.MediaPropertiesChanged(g_SmtcPropToken); } catch(...) {}
        try { g_SmtcSession.PlaybackInfoChanged(g_SmtcPlayToken);    } catch(...) {}
        try { g_SmtcSession.TimelinePropertiesChanged(g_SmtcTimeToken); } catch(...) {}
        g_SmtcSession = nullptr;
    }
}

static void SmtcSubscribeSession(
    GlobalSystemMediaTransportControlsSession ses) {
    SmtcUnsubscribeSession();
    if (!ses) return;
    g_SmtcSession = ses;
    auto trigger = [](auto&&, auto&&) {
        if (g_hWnd) PostMessage(g_hWnd, WM_TIMER, IDT_POLL, 0);
    };
    try { g_SmtcPropToken = ses.MediaPropertiesChanged(trigger); } catch(...) {}
    try { g_SmtcPlayToken = ses.PlaybackInfoChanged(trigger);    } catch(...) {}
    try { g_SmtcTimeToken = ses.TimelinePropertiesChanged(trigger); } catch(...) {}
}

static void SmtcSubscribeManager() {
    if (!g_Mgr) return;
    auto sessionTrigger = [](auto&&, auto&&) {
        if (g_hWnd) PostMessage(g_hWnd, WM_TIMER, IDT_POLL, 0);
    };
    try { g_SmtcMgrSessionToken = g_Mgr.CurrentSessionChanged(sessionTrigger); } catch(...) {}
}

// The session the panel currently displays and controls: the one
// SelectSession subscribed to, falling back to the OS-current session before
// the first FetchMedia has run. Cmd() and IDT_POS go through this so clicks
// and the progress bar always target the session shown on screen —
// GetCurrentSession alone can be a different app (focused-app filter, or a
// playing session that isn't the OS-current one).
static GlobalSystemMediaTransportControlsSession ActiveSession() {
    if (g_SmtcSession)
        return g_SmtcSession;
    if (!g_Mgr)
        return nullptr;
    try {
        return g_Mgr.GetCurrentSession();
    }
    WH_CATCH(L"ActiveSession")
    return nullptr;
}

// ============================================================================
//  Album palette extraction
// ============================================================================

struct AlbumPalette {
    Color primary;
    Color secondary;
};

// Bucket palette extraction: 4096 RGB buckets weighted by saturation*count.
// Skips near-black (luma<24) and near-white (luma>235) — no hue info there.
// Returns the top two distinct hues as primary/secondary.
// ~50x faster than k-means on a 64x64 thumbnail and gives richer colors than
// a plain left/right average.
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

    struct Bucket { uint32_t r=0, g=0, b=0, n=0; };
    Bucket buckets[16][16][16]{};

    const DWORD* pixels = (const DWORD*)data.Scan0;
    int stride = data.Stride / 4;

    for (UINT y = 0; y < h; y += 4) {
        for (UINT x = 0; x < w; x += 4) {
            DWORD p = pixels[y * stride + x];
            BYTE pr = (p >> 16) & 0xFF;
            BYTE pg = (p >> 8) & 0xFF;
            BYTE pb = p & 0xFF;
            int luma = (pr * 299 + pg * 587 + pb * 114) / 1000;
            if (luma < 24 || luma > 235) continue;
            auto& bk = buckets[pr >> 4][pg >> 4][pb >> 4];
            bk.r += pr; bk.g += pg; bk.b += pb; bk.n++;
        }
    }
    bmp->UnlockBits(&data);

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
                float mx = max({fr, fg, fb});
                float mn = min({fr, fg, fb});
                float sat = mx > 0 ? (mx - mn) / mx : 0;
                cands.push_back({bk.n * (0.3f + sat),
                                 (BYTE)(fr * 255), (BYTE)(fg * 255), (BYTE)(fb * 255)});
            }

    if (cands.empty())
        return {fallbackPrimary, fallbackSecondary};

    std::sort(cands.begin(), cands.end(),
              [](const Cand& a, const Cand& b){ return a.w > b.w; });

    Color primary(255, cands[0].r, cands[0].g, cands[0].b);
    Color secondary = primary;

    for (auto& c : cands) {
        int dr = (int)c.r - (int)cands[0].r;
        int dg = (int)c.g - (int)cands[0].g;
        int db = (int)c.b - (int)cands[0].b;
        // distinct if squared colour distance exceeds threshold (~0.05 in 0-1 space)
        if (dr*dr + dg*dg + db*db > 3264) {
            secondary = Color(255, c.r, c.g, c.b);
            break;
        }
    }

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
    // divisor scales with themeBlur (0→1 = no blur, 60→77, 100→128);
    // more = blurrier. min of 1 (not 4) so slider 0 really means sharp art.
    int blurDiv = max(1, (int)(g_US.themeBlur * 1.28f));
    if (g_BlurCache.bitmap && g_BlurCache.width == w &&
        g_BlurCache.height == h && g_BlurCache.artVersion == artVersion &&
        g_BlurCache.blurLevel == blurDiv)
        return;

    int srcW = art->GetWidth(), srcH = art->GetHeight();
    int smallW = max(1, srcW / blurDiv), smallH = max(1, srcH / blurDiv);
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
        g_BlurCache.blurLevel = blurDiv;
    } else {
        delete bg;
    }
}

// ============================================================================
//  Screen-capture blur background  (software fallback for Blurred/Neon)
// ============================================================================
//
// Preferred path: DirectComposition gaussian blur (live backdrop, GPU,
// slider-controlled via SetStandardDeviation). But InitDComp relies on the
// CreateBackdropBrush API, which is not available on every Windows build —
// when it fails, the old code fell back to ACCENT_ENABLE_ACRYLICBLURBEHIND,
// whose blur radius is FIXED by DWM. That made the Blur Strength slider a
// no-op on machines where DComp init fails.
//
// This software path replaces that fallback: capture the screen behind the
// panel (hide window → DwmFlush → BitBlt), then downsample/upsample by a
// divisor derived from the slider. DrawBackground composites the result for
// Blurred/Neon, so Blur Strength works everywhere. The backdrop is a static
// snapshot — refreshed on window creation, taskbar moves, DPI changes, and
// settings changes — which is fine for a taskbar widget whose backdrop is
// essentially wallpaper.

static bool UsingSoftBlur() {
    return (g_US.theme == Theme::Blurred || g_US.theme == Theme::Neon) &&
           !g_DCompDevice;
}

// WDA_EXCLUDEFROMCAPTURE (Win10 2004+) removes our window from screen
// captures WITHOUT affecting what the user sees on the monitor — so the
// backdrop can be re-captured periodically with zero visible blink. Flipped
// to false if the call ever fails, which drops us back to the legacy
// hide-for-one-frame capture (and disables the periodic refresh).
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif
static bool g_CaptureAffinityOK = true;

// Persistent capture resources, reused across refreshes so the per-tick cost
// is one small BitBlt + a memcmp — no allocations on the hot path.
struct SoftBlurCapture {
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HBITMAP oldSel = nullptr;
    void* bits = nullptr;
    int w = 0, h = 0;
    std::vector<BYTE> prev;  // last frame, for change detection

    void Release() {
        if (memDC && oldSel)
            SelectObject(memDC, oldSel);
        if (dib)
            DeleteObject(dib);
        if (memDC)
            DeleteDC(memDC);
        memDC = nullptr;
        dib = nullptr;
        oldSel = nullptr;
        bits = nullptr;
        w = h = 0;
        prev.clear();
        prev.shrink_to_fit();
    }
    ~SoftBlurCapture() { Release(); }
};
static SoftBlurCapture g_SoftCap;

// Returns true when the blurred backdrop cache was rebuilt (caller should
// repaint). force=true (periodic tick) recaptures even when geometry and
// settings are unchanged — the content BEHIND the panel may have changed —
// but the blur pipeline still only runs when the captured pixels differ.
static bool RefreshScreenBlurCache(HWND hwnd, bool force = false) {
    if (!UsingSoftBlur() || g_US.themeBlur <= 0) {
        g_ScreenCapCache.Invalidate();
        g_SoftCap.Release();
        return false;
    }

    RECT rc;
    GetWindowRect(hwnd, &rc);
    int W = rc.right - rc.left, H = rc.bottom - rc.top;
    if (W <= 0 || H <= 0)
        return false;

    // blurDiv: higher = blurrier.  At themeBlur=25 it approximates DWM acrylic.
    int blurDiv = max(2, 1 + g_US.themeBlur / 3);

    bool cacheValid =
        g_ScreenCapCache.bitmap &&
        g_ScreenCapCache.width == W && g_ScreenCapCache.height == H &&
        g_ScreenCapCache.blurLevel == blurDiv &&
        g_ScreenCapCache.winX == rc.left && g_ScreenCapCache.winY == rc.top;
    if (!force && cacheValid)
        return false;

    // ── (Re)create the persistent capture DIB on first use or resize ────────
    if (!g_SoftCap.memDC || g_SoftCap.w != W || g_SoftCap.h != H) {
        g_SoftCap.Release();
        HDC hScreen0 = GetDC(NULL);
        if (!hScreen0)
            return false;
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = W;
        bmi.bmiHeader.biHeight = -H;  // top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        g_SoftCap.memDC = CreateCompatibleDC(hScreen0);
        g_SoftCap.dib = CreateDIBSection(g_SoftCap.memDC, &bmi, DIB_RGB_COLORS,
                                         &g_SoftCap.bits, NULL, 0);
        ReleaseDC(NULL, hScreen0);
        if (!g_SoftCap.dib) {
            g_SoftCap.Release();
            return false;
        }
        g_SoftCap.oldSel =
            (HBITMAP)SelectObject(g_SoftCap.memDC, g_SoftCap.dib);
        g_SoftCap.w = W;
        g_SoftCap.h = H;
    }

    // ── Capture ──────────────────────────────────────────────────────────────
    // With display affinity the panel is persistently excluded from captures
    // (set once in ApplyAppearance), so this is just a BitBlt — no DWM
    // re-composition wait, nothing visible to the user. Legacy fallback for
    // builds without WDA_EXCLUDEFROMCAPTURE: hide for one composed frame.
    HDC hScreen = GetDC(NULL);
    if (!hScreen)
        return false;
    if (!g_CaptureAffinityOK) {
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
        DwmFlush();
    }
    BitBlt(g_SoftCap.memDC, 0, 0, W, H, hScreen, rc.left, rc.top, SRCCOPY);
    GdiFlush();
    if (!g_CaptureAffinityOK)
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ReleaseDC(NULL, hScreen);

    // GDI BitBlt leaves alpha bytes as 0; force them to 255 (fully opaque).
    // Done before the compare so prev/current are normalized identically.
    BYTE* px = (BYTE*)g_SoftCap.bits;
    const size_t byteCount = (size_t)W * H * 4;
    for (size_t i = 3; i < byteCount; i += 4)
        px[i] = 255;

    // ── Skip the blur rebuild when nothing behind the panel changed ─────────
    bool contentChanged = g_SoftCap.prev.size() != byteCount ||
                          memcmp(g_SoftCap.prev.data(), px, byteCount) != 0;
    if (!contentChanged && cacheValid)
        return false;
    g_SoftCap.prev.assign(px, px + byteCount);

    // Wrap in a GDI+ Bitmap (borrows the DIB bits — valid for this scope).
    Bitmap src(W, H, W * 4, PixelFormat32bppARGB, px);

    // Downsample (blurs by losing high-frequency detail).
    int sW = max(1, W / blurDiv), sH = max(1, H / blurDiv);
    Bitmap small(sW, sH, PixelFormat32bppPARGB);
    {
        Graphics sg(&small);
        sg.SetInterpolationMode(InterpolationModeBilinear);
        sg.DrawImage(&src, 0, 0, sW, sH);
    }

    // Upsample back to panel size (bicubic adds the soft, blurry look).
    Bitmap* result = new Bitmap(W, H, PixelFormat32bppPARGB);
    if (result && result->GetLastStatus() == Ok) {
        Graphics rg(result);
        rg.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        ImageAttributes ia;
        ia.SetWrapMode(WrapModeTileFlipXY);
        rg.DrawImage(&small, RectF(0.f, 0.f, (REAL)W, (REAL)H),
                     0.f, 0.f, (REAL)sW, (REAL)sH, UnitPixel, &ia);

        g_ScreenCapCache.Invalidate();
        g_ScreenCapCache.bitmap = result;
        g_ScreenCapCache.width = W;
        g_ScreenCapCache.height = H;
        g_ScreenCapCache.blurLevel = blurDiv;
        g_ScreenCapCache.winX = rc.left;
        g_ScreenCapCache.winY = rc.top;
        return true;
    }
    delete result;
    return false;
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
    }
    WH_CATCH(L"ToBitmap")
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
//  Media fetch — decomposed
// ============================================================================
//
// FetchMedia() is the public entry point called from IDT_POLL.  It delegates
// to four focused helpers so each concern can be read, tested, and reasoned
// about independently:
//
//   SelectSession()        — pick the right GSMTC session (priority + filter)
//   FetchAlbumArt()        — decode thumbnail, update aspect ratio & palette
//   FetchPlaybackState()   — playing flag, shuffle, repeat mode
//   FetchTimeline()        — position + duration → g_Progress
//
// g_M.mtx is held only for the fields that are written here and read by
// message-loop timer callbacks.  The lock comments above g_M's declaration
// are the authoritative ownership contract.

// ── 4a. Session selection ────────────────────────────────────────────────────
// Returns the session to use this poll cycle, or nullptr.
static GlobalSystemMediaTransportControlsSession SelectSession() {
    if (!g_Mgr) {
        try {
            g_Mgr = GlobalSystemMediaTransportControlsSessionManager ::
                        RequestAsync()
                            .get();
        }
        WH_CATCH(L"SelectSession/RequestAsync")
        if (!g_Mgr)
            return nullptr;
        SmtcSubscribeManager();  // one-time: CurrentSessionChanged on the manager
    }

    // NOTE on the null checks below: when a session is mid-teardown (e.g.
    // play/pause spammed while the media app reshuffles its session), the
    // GSMTC getters can return NULL objects rather than throwing. Calling a
    // method on a null winrt object is a null deref, and an access violation
    // is NOT a C++ exception in this toolchain — the try/catch blocks don't
    // save us and Explorer dies. This was the "crash when pressing the
    // buttons" bug (faulted inside FetchMedia, 0xc0000005).
    auto sessions = g_Mgr.GetSessions();
    if (!sessions)
        return nullptr;
    GlobalSystemMediaTransportControlsSession ses = nullptr;

    // Priority: a playing session.
    for (auto const& s : sessions) {
        try {
            using S = GlobalSystemMediaTransportControlsSessionPlaybackStatus;
            if (!s)
                continue;
            auto pbi = s.GetPlaybackInfo();
            if (pbi && pbi.PlaybackStatus() == S::Playing) {
                ses = s;
                break;
            }
        }
        WH_CATCH(L"SelectSession/playingLoop")
    }

    // Filter by focused app if configured (overrides the playing-first pick).
    if (!g_US.focusedApp.empty()) {
        wstring lf = g_US.focusedApp;
        transform(lf.begin(), lf.end(), lf.begin(), ::towlower);
        if (lf.size() > 4 && lf.substr(lf.size() - 4) == L".exe")
            lf = lf.substr(0, lf.size() - 4);
        ses = nullptr;  // must match the focused app or nothing
        for (auto const& s : sessions) {
            try {
                if (!s)
                    continue;
                wstring id = s.SourceAppUserModelId().c_str();
                transform(id.begin(), id.end(), id.begin(), ::towlower);
                if (id.find(lf) != wstring::npos) {
                    ses = s;
                    break;
                }
            }
            WH_CATCH(L"SelectSession/filterLoop")
        }
        // If the focused app isn't running, ses stays null — do NOT fall
        // back to whatever else is playing. No early return: the filtered
        // pick must still reach the re-subscribe below, or SMTC events never
        // fire for the focused app.
    } else if (!ses) {
        // Fallback: whatever the OS considers current (only when no
        // focusedApp is configured).
        try {
            ses = g_Mgr.GetCurrentSession();
        }
        WH_CATCH(L"SelectSession/GetCurrentSession")
    }

    // Re-subscribe when the session identity changes so event-driven updates
    // always target the session we're actually watching.
    bool sessionChanged = (!g_SmtcSession && ses) ||
                          (g_SmtcSession && !ses) ||
                          (g_SmtcSession && ses &&
                           g_SmtcSession.SourceAppUserModelId() !=
                               ses.SourceAppUserModelId());
    if (sessionChanged)
        SmtcSubscribeSession(ses);

    return ses;
}

// ── 4b. Album art + palette ──────────────────────────────────────────────────
// Precondition: called on the message-loop thread; g_M.mtx is NOT held.
// Writes to the ml-only fields (art, primaryColor, secondaryColor,
// isDarkCover) and to the unlocked globals g_ArtAspectRatio / g_ArtVersion.
static void FetchAlbumArt(
    GlobalSystemMediaTransportControlsSessionMediaProperties const& props) {
    bool hadArt = (g_M.art != nullptr);
    g_M.art.reset();
    try {
        auto ref = props.Thumbnail();
        if (ref) {
            auto stream = ref.OpenReadAsync().get();
            if (stream)
                g_M.art.reset(ToBitmap(stream));
        }
    }
    WH_CATCH(L"FetchAlbumArt/decode")

    // Nothing before, nothing now — visually identical. Skip the version
    // bump, or every retry for a track that simply has no thumbnail would
    // force a relayout + repaint on each poll heartbeat.
    if (!hadArt && !g_M.art)
        return;

    ++g_ArtVersion;

    // Aspect ratio for layout auto-sizing.
    if (g_M.art) {
        UINT natW = g_M.art->GetWidth(), natH = g_M.art->GetHeight();
        g_ArtAspectRatio = (natH > 0) ? (float)natW / (float)natH : 1.f;
    } else {
        g_ArtAspectRatio = 1.f;
    }
    g_LayoutDirty = true;

    // Cover luminance — determines whether overlaid text should be light/dark.
    g_M.isDarkCover = true;
    if (g_M.art) {
        try {
            Bitmap px1(1, 1, PixelFormat32bppARGB);
            Graphics pg(&px1);
            pg.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            pg.DrawImage(g_M.art.get(), 0, 0, 1, 1);
            Color avg;
            px1.GetPixel(0, 0, &avg);
            double lum =
                0.299 * avg.GetR() + 0.587 * avg.GetG() + 0.114 * avg.GetB();
            g_M.isDarkCover = (lum < LUM_DARK_THRESHOLD);
        }
        WH_CATCH(L"FetchAlbumArt/luminance")
    }

    auto pal = GetAlbumPalette(g_M.art.get());
    g_M.primaryColor = pal.primary;
    g_M.secondaryColor = pal.secondary;
}

// ── 4c. Playback state ───────────────────────────────────────────────────────
// Writes g_M.playing, g_M.shuffleOn, g_M.repeatMode under g_M.mtx.
// Also syncs g_Progress.isPlaying when the playing flag changes.
static void FetchPlaybackState(
    GlobalSystemMediaTransportControlsSession const& ses) {
    try {
        auto pbi = ses.GetPlaybackInfo();
        if (!pbi) {
            // Session mid-teardown — calling methods on the null object
            // would AV (not a catchable C++ exception). Treat as paused.
            Wh_Log(L"[FetchPlaybackState] null PlaybackInfo — marking paused");
            MediaWriter w;
            w->playing = false;
            return;
        }
        using S = GlobalSystemMediaTransportControlsSessionPlaybackStatus;
        bool nowPlaying = (pbi.PlaybackStatus() == S::Playing);

        {
            MediaWriter w;
            w->playing = nowPlaying;

            // Sync progress interpolation origin whenever play state toggles.
            if (nowPlaying != g_Progress.isPlaying) {
                g_Progress.isPlaying = nowPlaying;
                g_Progress.updateTick = GetTickCount();
                g_Progress.lastRawPos = w->timelinePos;
            }

            try {
                w->shuffleOn = pbi.IsShuffleActive()
                                   ? pbi.IsShuffleActive().Value()
                                   : false;
            }
            WH_CATCH(L"FetchPlaybackState/shuffle")

            try {
                auto rm = pbi.AutoRepeatMode();
                if (rm) {
                    using RM = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
                    auto v = rm.Value();
                    w->repeatMode = (v == RM::Track)  ? 1
                                    : (v == RM::List) ? 2
                                                      : 0;
                }
            }
            WH_CATCH(L"FetchPlaybackState/repeat")
        }
    } catch (...) {
        Wh_Log(L"[FetchPlaybackState] failed — marking paused");
        MediaWriter w;
        w->playing = false;
    }
}

// ── 4d. Timeline / progress ──────────────────────────────────────────────────
// Writes g_M.timelinePos/End and syncs g_Progress under g_M.mtx.
static void FetchTimeline(
    GlobalSystemMediaTransportControlsSession const& ses) {
    try {
        auto tl = ses.GetTimelineProperties();
        if (!tl) {
            // Null during session teardown — see FetchPlaybackState note.
            Wh_Log(L"[FetchTimeline] null TimelineProperties — resetting");
            MediaWriter w;
            w->timelinePos = w->timelineEnd = 0;
            return;
        }
        INT64 newPos = tl.Position().count();
        INT64 newEnd = tl.EndTime().count();

        MediaWriter w;
        w->timelinePos = newPos;
        w->timelineEnd = newEnd;
        bool nowP = w->playing;

        if (newPos != g_Progress.lastRawPos || newEnd != g_Progress.endRaw ||
            nowP != g_Progress.isPlaying) {
            g_Progress.lastRawPos = newPos;
            g_Progress.endRaw = newEnd;
            g_Progress.updateTick = GetTickCount();
            g_Progress.isPlaying = nowP;
        }
    } catch (...) {
        Wh_Log(L"[FetchTimeline] failed — resetting timeline");
        MediaWriter w;
        w->timelinePos = w->timelineEnd = 0;
    }
}

// ── 4e. Orchestrator ─────────────────────────────────────────────────────────
static void FetchMedia() {
    try {
        auto ses = SelectSession();

        if (!ses) {
            // No media source available (or focused app is configured but
            // not running).  Reset all displayed state so the player shows
            // its idle/default appearance instead of freezing on the last
            // track from whichever other app was previously active.
            // Skip if already reset — bumping g_ArtVersion every heartbeat
            // would force a repaint every 5s while idle.
            bool alreadyIdle;
            {
                MediaReader r;
                alreadyIdle = !r->hasMedia && !g_M.art;
            }
            if (alreadyIdle)
                return;
            {
                MediaWriter w;
                w->playing  = false;
                w->hasMedia = false;
                w->title    = L"No Media";
                w->artist   = L"";
            }
            // Clear art so the idle palette/background is used.
            g_M.art.reset();
            ++g_ArtVersion;
            g_M.primaryColor   = Color(255, 18, 18, 18);
            g_M.secondaryColor = Color(255, 45, 45, 45);
            g_M.isDarkCover    = true;
            g_Vol.cachedIface  = nullptr;
            g_Vol.cachedAumid.clear();
            return;
        }

        // ── Media properties ─────────────────────────────────────────────────
        GlobalSystemMediaTransportControlsSessionMediaProperties props =
            nullptr;
        try {
            props = ses.TryGetMediaPropertiesAsync().get();
        }
        WH_CATCH(L"FetchMedia/TryGetMediaPropertiesAsync")

        if (!props) {
            Wh_Log(L"[FetchMedia] null props — marking paused");
            MediaWriter w;
            w->playing = false;
            return;
        }

        try {
            g_currentMediaAppAumid = ses.SourceAppUserModelId().c_str();
        }
        WH_CATCH(L"FetchMedia/SourceAumid")
        FetchAppVolume();

        // ── Track-change detection & crossfade trigger ───────────────────────
        // All crossfade state lives in g_Anim.crossfade; FetchMedia only calls
        // RequestSwap() — it never touches g_Anim.crossfade.alpha/fadeOut
        // directly.  This is the ownership boundary that was previously
        // blurred.
        wstring nt =
            WH_TRY_OR(wstring(props.Title().c_str()), wstring(L"Unknown"));

        bool titleChanged;
        {
            MediaReader r;
            titleChanged = (nt != r->title);
        }

        if (titleChanged) {
            wstring newArtist =
                WH_TRY_OR(wstring(props.Artist().c_str()), wstring(L""));

            // Only arm a crossfade when there was a real previous track.
            // Gated on titleChanged (not the art retry below) — re-arming
            // for the same title made art-less tracks blink every poll.
            bool hadMedia;
            {
                MediaReader r;
                hadMedia = (r->title != L"No Media");
            }
            if (hadMedia) {
                g_Anim.crossfade.RequestSwap(nt, newArtist);
                // RequestSwap is idempotent; arming the timer on every call is
                // safe because TimerSet::Set() is idempotent too.
                if (g_hWnd)
                    g_Timers.Set(g_hWnd, IDT_TEXT_FADE, 16);
            }
        }

        // Decode art on track change, and keep retrying while the current
        // track has none — some apps publish the thumbnail a moment after
        // the title. FetchAlbumArt skips the version bump when nothing
        // actually changed, so the retries are free.
        if (titleChanged || !g_M.art)
            FetchAlbumArt(props);

        // Commit title/artist immediately when no crossfade is pending.
        // When a crossfade IS running, the IDT_TEXT_FADE tick commits the
        // pending strings once alpha reaches zero — never FetchMedia.
        // Only write title/artist immediately when no crossfade fade-out is
        // running.  If one IS running, TextCrossfade::Tick() will commit the
        // pending strings at the moment alpha reaches zero.
        if (!g_Anim.crossfade.active || !g_Anim.crossfade.fadeOut) {
            MediaWriter w;
            w->title = nt;
            w->artist =
                WH_TRY_OR(wstring(props.Artist().c_str()), wstring(L""));
        }

        // ── Playback + timeline
        // ───────────────────────────────────────────────
        FetchPlaybackState(ses);
        FetchTimeline(ses);

        {
            MediaWriter w;
            w->hasMedia = true;
        }
    }
    WH_CATCH(L"FetchMedia/outer")
}

static void Cmd(MediaCmd c) {
    try {
        // ActiveSession, not GetCurrentSession: the button must control the
        // app the panel is showing, which can differ from the OS-current
        // session (focused-app filter, multiple media apps open).
        auto s = ActiveSession();
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
                        MediaReader r;
                        cur = r->shuffleOn;
                    }
                    s.TryChangeShuffleActiveAsync(!cur);
                }
                WH_CATCH(L"Cmd/Shuffle")
                break;
            case MediaCmd::Repeat:
                try {
                    int cur;
                    {
                        MediaReader r;
                        cur = r->repeatMode;
                    }
                    using RM = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
                    RM next = (cur == 0)   ? RM::Track
                              : (cur == 1) ? RM::List
                                           : RM::None;
                    s.TryChangeAutoRepeatModeAsync(next);
                }
                WH_CATCH(L"Cmd/Repeat")
                break;
            default:
                break;
        }
    }
    WH_CATCH(L"Cmd/outer")
}

// ============================================================================

static void ApplyTimerRates(HWND hwnd) {
    bool playing;
    {
        MediaReader r;
        playing = r->playing;
    }
    g_Timers.Set(hwnd, IDT_POLL, POLL_RATE_HEARTBEAT);
    if (playing)
        g_Timers.Set(hwnd, IDT_POS, POS_RATE_PLAYING);
    else
        g_Timers.Kill(IDT_POS);
}

// ============================================================================
//  Layout rebuild
// ============================================================================

// Pixel-space icon scale for a panel of height H px — the single source of
// truth for control-icon geometry, shared by layout (BuildControlLayout),
// drawing (MakeDrawCtx), and hit-testing (HitTestControls).
static float IconScalePx(int H) {
    return (g_US.iconSize / ICON_SIZE_REF) * ((float)H / PANEL_HEIGHT_REF);
}

static ControlLayout BuildControlLayout(int W,
                                        int H,
                                        int ctrlCellX,
                                        int ctrlCellWidth) {
    // Scale the logical wrap padding to pixels before centering. Mixing
    // unscaled padding with the pixel H put cY off-center at DPI != 100%
    // whenever padTop != padBottom (with symmetric padding the error
    // cancelled, which is why it went unnoticed).
    float padScale =
        (g_US.panelHeight > 0) ? (float)H / (float)g_US.panelHeight : 1.f;
    float padT = g_US.wrap.padTop * padScale;
    float padB = g_US.wrap.padBottom * padScale;
    float cY = padT + (H - padT - padB) / 2.f;

    int btnCount = 0;
    if (g_US.showPrevBtn)
        btnCount += 1;
    if (g_US.showPlayBtn)
        btnCount += 1;
    if (g_US.showNextBtn)
        btnCount += 1;
    if (g_US.showSpeakerIcon)
        btnCount += 1;
    if (g_US.showRepeatButton)
        btnCount += 1;
    if (g_US.showShuffleButton)
        btnCount += 1;

    float nX = 0, ppX = 0, pX = 0, vX = 0, shuffleX = 0, repeatX = 0;
    float firstX = (float)(ctrlCellX + ctrlCellWidth);

    if (btnCount > 0) {
        // First icon center sits at padL + iconHalf so the glyph left edge
        // lands on the container pad boundary (matching CalcControlsWidth).
        float step = g_US.buttonSpacing * ((float)H / PANEL_HEIGHT_REF);
        float iconScale = IconScalePx(H);
        float iconHalf = ICON_SIZE_REF * iconScale / 2.f;
        float padL = (float)g_US.containers[2].padLeft;
        float start = (float)ctrlCellX + padL + iconHalf;

        float cursor = start + step * (btnCount - 1);
        if (g_US.showNextBtn) {
            nX = cursor;
            cursor -= step;
        }
        if (g_US.showPlayBtn) {
            ppX = cursor;
            cursor -= step;
        }
        if (g_US.showPrevBtn) {
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
    //
    // g_ContainerWidths are computed in logical pixels by RecomputeLayout, but
    // W is the actual client-rect pixel width (DPI-scaled by UpdateBoundsAndRegion
    // before SetWindowPos).  We must scale all container widths and gaps by the
    // same ratio so positions fill the real window exactly — otherwise at >96 DPI
    // the containers end before the right edge (gap), or if the panel width user
    // setting was set to a different value than the actual window, containers
    // overlap or clip each other.
    //
    // Compute the logical total so we can derive the scale factor.
    int logicalTotal = g_US.wrap.padLeft + g_US.wrap.padRight;
    {
        bool lf = true;
        for (int id : g_US.wrap.order) {
            if (!IsContainerEnabled(id))
                continue;
            if (id == 4 && g_US.wrap.vizAsBackground)
                continue;
            if (!lf)
                logicalTotal += g_US.wrap.globalGap;
            lf = false;
            logicalTotal += GetContainerWidth(id);
        }
    }
    // Scale factor: map logical widths → actual pixel widths.
    // Guard against divide-by-zero; if logicalTotal is 0 use identity.
    float scale = (logicalTotal > 0) ? (float)W / (float)logicalTotal : 1.f;

    // Helper lambdas: scale a logical dimension to pixel, rounding.
    auto Px = [&](int logical) { return (int)roundf((float)logical * scale); };

    g_Layout.originCount = 0;
    int curX = Px(g_US.wrap.padLeft);
    bool first = true;

    for (int id : g_US.wrap.order) {
        if (!IsContainerEnabled(id))
            continue;
        if (id == 4 && g_US.wrap.vizAsBackground)
            continue;

        if (!first)
            curX += Px(g_US.wrap.globalGap);
        first = false;

        int cellW = Px(GetContainerWidth(id));
        int padL = 0, padR = 0;
        for (const auto& c : g_US.containers)
            if (c.id == id) {
                padL = Px(c.padLeft);
                padR = Px(c.padRight);
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

    // ── Step 2: art geometry ──────────────────────────────────────────────────
    // aW MUST be taken from the media container's already-laid-out cell, not
    // recomputed independently.  CalcArtDims uses g_ArtAspectRatio which may
    // have been updated by FetchAlbumArt *after* RecomputeLayout last ran —
    // if they disagree, the art overflows into the next container.
    // The cell width IS the authoritative allocated width; we just subtract the
    // container padding to get the drawable area.
    int pxPadTop   = Px(g_US.wrap.padTop);
    int pxPadRight = Px(g_US.wrap.padRight);

    int aX, aW, aS;
    {
        // Height of the art square is always panelHeight minus vertical padding
        // (scaled to physical pixels), regardless of aspect ratio.
        int logicalS = max(8, g_US.panelHeight - g_US.wrap.padTop - g_US.wrap.padBottom);
        aS = Px(logicalS);

        const auto* mediaCO = g_Layout.FindOrigin(1);
        if (mediaCO && IsContainerEnabled(1)) {
            // Width = the cell's content area exactly — guaranteed no overflow.
            aX = mediaCO->contentX;
            aW = max(8, mediaCO->contentWidth);
        } else {
            // Media container disabled or absent — fall back to square.
            aX = Px(g_US.wrap.padLeft);
            aW = aS;
        }
    }

    // Pass the radius through unclamped — CustomRoundedPath clamps each
    // corner in float precision, so the old integer pre-clamp here
    // ((min(aS,aW)/2) truncates) would flatten true pill ends on the art.
    int aR = g_US.mediaCornerRadius;

    g_Layout.art = {aX, pxPadTop, aS, aW, aR};

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
        (float)pxPadTop, barZoneH,         bW, bStep, bStartX,
        barZoneH * 0.92f, barZoneH * 0.035f};

    // ── Step 4: controls layout ───────────────────────────────────────────────
    int ctrlCellX = W - pxPadRight;
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
    // Clamp so the corner arcs never overlap — radius >= half the smaller
    // dimension degenerates into broken geometry instead of a pill.
    r = min(r, min(w, h) / 2);
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
    // Float-precise clamp: with an odd pixel height the old integer clamp
    // ((int)(h/2)) cut the radius short of a true semicircle, so "pill"
    // settings rendered with subtly flattened ends.
    float maxR = min(w / 2.f, h / 2.f);
    auto clampf = [&](int r) { return max(0.f, min((float)r, maxR)); };
    float tl = clampf(rTL);
    float tr = clampf(rTR);
    float br = clampf(rBR);
    float bl = clampf(rBL);

    if (tl > 0)
        p.AddArc(x, y, tl * 2.f, tl * 2.f, 180, 90);
    else
        p.AddLine(PointF(x, y), PointF(x, y));
    if (tr > 0)
        p.AddArc(x + w - tr * 2.f, y, tr * 2.f, tr * 2.f, 270, 90);
    else
        p.AddLine(PointF(x + w, y), PointF(x + w, y));
    if (br > 0)
        p.AddArc(x + w - br * 2.f, y + h - br * 2.f, br * 2.f, br * 2.f, 0,
                 90);
    else
        p.AddLine(PointF(x + w, y + h), PointF(x + w, y + h));
    if (bl > 0)
        p.AddArc(x, y + h - bl * 2.f, bl * 2.f, bl * 2.f, 90, 90);
    else
        p.AddLine(PointF(x, y + h), PointF(x, y + h));
    p.CloseFigure();
}

// Resolves the effective per-corner radii: Uniform Corners on → the single
// Corner Radius everywhere; off → the four individual per-corner settings.
struct CornerRadii { int tl, tr, br, bl; };
static CornerRadii GetCornerRadii() {
    if (g_US.cornerUniform) {
        int r = g_US.cornerRadius;
        return {r, r, r, r};
    }
    return {g_US.cornerTL, g_US.cornerTR, g_US.cornerBR, g_US.cornerBL};
}
static bool HasRounding() {
    auto r = GetCornerRadii();
    return r.tl > 0 || r.tr > 0 || r.br > 0 || r.bl > 0;
}

// Full-panel rounded clip path using the current corner radius setting.
static void PanelPath(GraphicsPath& p, int W, int H) {
    auto r = GetCornerRadii();
    if (HasRounding())
        CustomRoundedPath(p, 0.f, 0.f, (float)W, (float)H, r.tl, r.tr, r.br, r.bl);
    else
        p.AddRectangle(Rect(0, 0, W, H));
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
    if (g_US.textAccentColor)
        return GetWindowsAccentColor();
    // Adaptive theme applies when the toggle is on AND text color is still
    // the default white — a custom color means the user opted out.
    bool isDefaultColor = (g_US.manualTextColor == 0xFFFFFFFF);
    if (g_US.adaptiveTheme && isDefaultColor)
        return IsSystemLight() ? 0xFF1a1a1a : 0xFFFFFFFF;
    return g_US.manualTextColor;
}

// ============================================================================
//  DirectComposition backdrop blur
// ============================================================================

static void ShutdownDComp() {
    if (g_DCompBackdrop) { g_DCompBackdrop->Release(); g_DCompBackdrop = nullptr; }
    if (g_DCompBlur)     { g_DCompBlur->Release();     g_DCompBlur     = nullptr; }
    if (g_DCompVisual)   { g_DCompVisual->Release();   g_DCompVisual   = nullptr; }
    if (g_DCompTarget)   { g_DCompTarget->Release();   g_DCompTarget   = nullptr; }
    if (g_DCompDevice3)  { g_DCompDevice3->Release();  g_DCompDevice3  = nullptr; }
    if (g_DCompDevice)   { g_DCompDevice->Release();   g_DCompDevice   = nullptr; }
}

static void UpdateDCompBlur() {
    if (!g_DCompBlur || !g_DCompDevice) {
        Wh_Log(L"[DComp] UpdateDCompBlur skipped (blur=%p device=%p)",
               (void*)g_DCompBlur, (void*)g_DCompDevice);
        return;
    }
    // 0.8 multiplier: ThemeBlur 0..100 -> stddev 0..80. IDCompositionGaussian
    // BlurEffect accepts 0..250, but anything above ~80 just looks like fog.
    // Old 0.4 multiplier (cap 40) was hard to tell from "doing nothing".
    float stddev = g_US.themeBlur * 0.8f;
    HRESULT hr = g_DCompBlur->SetStandardDeviation(stddev);
    HRESULT hrc = g_DCompDevice->Commit();
    Wh_Log(L"[DComp] SetStandardDeviation=%.1f hr=0x%08X commit=0x%08X",
           stddev, hr, hrc);
}

static bool InitDComp(HWND hwnd) {
    ShutdownDComp();

    // DCompositionCreateDevice is in mingw-w64's dcomp.h and accepts nullptr
    // (no D3D device needed — DWM compositor handles the GPU work).
    IDCompositionDevice* pBase = nullptr;
    HRESULT hr = DCompositionCreateDevice(nullptr, IID_PPV_ARGS(&pBase));
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] CreateDevice failed: 0x%08X", hr);
        return false;
    }

    hr = pBase->QueryInterface(__uuidof(IDCompositionDesktopDevice),
                               (void**)&g_DCompDevice);
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] QI IDCompositionDesktopDevice failed: 0x%08X", hr);
        pBase->Release(); return false;
    }

    hr = pBase->QueryInterface(__uuidof(IDCompositionDevice3),
                               (void**)&g_DCompDevice3);
    pBase->Release();
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] QI IDCompositionDevice3 failed: 0x%08X", hr);
        ShutdownDComp(); return false;
    }

    hr = g_DCompDevice->CreateTargetForHwnd(hwnd, FALSE, &g_DCompTarget);
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] CreateTargetForHwnd failed: 0x%08X", hr);
        ShutdownDComp(); return false;
    }

    hr = g_DCompDevice3->CreateBackdropBrush(&g_DCompBackdrop);
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] CreateBackdropBrush failed: 0x%08X", hr);
        ShutdownDComp(); return false;
    }

    hr = g_DCompDevice3->CreateGaussianBlurEffect(&g_DCompBlur);
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] CreateGaussianBlurEffect failed: 0x%08X", hr);
        ShutdownDComp(); return false;
    }

    // Chain: backdrop brush → gaussian blur → visual content.
    // SetInput feeds the live backdrop into the blur filter.
    // SetContent on the filter effect makes it the visual's bitmap source.
    g_DCompBlur->SetInput(0, g_DCompBackdrop, 0);
    g_DCompBlur->SetStandardDeviation(g_US.themeBlur * 0.8f);

    IDCompositionVisual2* pVis = nullptr;
    hr = g_DCompDevice3->CreateVisual(&pVis);
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] CreateVisual failed: 0x%08X", hr);
        ShutdownDComp(); return false;
    }
    g_DCompVisual = pVis;

    g_DCompVisual->SetContent(g_DCompBlur);
    g_DCompTarget->SetRoot(g_DCompVisual);

    hr = g_DCompDevice->Commit();
    if (FAILED(hr)) {
        Wh_Log(L"[DComp] Commit failed: 0x%08X", hr);
        ShutdownDComp(); return false;
    }

    return true;
}

// ============================================================================
//  Appearance
// ============================================================================

void ApplyAppearance(HWND hwnd) {
        Wh_Log(L"[Diag] ApplyAppearance: theme=%d themeBlur=%d dcompDevice=%p",
           (int)g_US.theme, g_US.themeBlur, (void*)g_DCompDevice);
    // For acrylic/blurred themes DWM composites the effect in the compositor
    // All themes now use SetWindowRgn for corner shaping; DWM rounding is
    // unused.  (Blurred/Neon switched to software capture blur, so no DWM
    // acrylic that would clip before the GDI region.)
    DWM_WINDOW_CORNER_PREFERENCE cp = DWMWCP_DONOTROUND;
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
        g_Timers.Kill(IDT_SOFTBLUR);
        SetWindowDisplayAffinity(hwnd, WDA_NONE);
        pol = {ACCENT_DISABLED, 0, 0, 0};
        fn(hwnd, &d);
        MARGINS m = {-1};
        DwmExtendFrameIntoClientArea(hwnd, &m);
    } else if (t == Theme::Blurred || t == Theme::Neon) {
        MARGINS m = {-1};
        DwmExtendFrameIntoClientArea(hwnd, &m);
        bool dcompReady = g_DCompDevice ? true : InitDComp(hwnd);
        if (dcompReady) {
            // DComp handles blur — disable DWM acrylic to avoid double-blur.
            pol = {ACCENT_DISABLED, 0, 0, 0};
            UpdateDCompBlur();
            g_Timers.Kill(IDT_SOFTBLUR);
            SetWindowDisplayAffinity(hwnd, WDA_NONE);
            Wh_Log(L"[Blur] PATH=DComp (live GPU blur, slider-controlled)");
        } else {
            // DComp unavailable — use the software capture blur instead
            // (drawn by DrawBackground from g_ScreenCapCache). DWM acrylic
            // is deliberately NOT used here: its radius is fixed by DWM and
            // ignores the slider, which made Blur Strength a no-op on
            // machines where DComp init fails.
            //
            // The exclusion stays set for the whole time the software path is
            // active so each periodic re-capture is a plain BitBlt — no DWM
            // re-composition wait per tick, nothing visible to the user.
            // Side effect: the panel won't appear in screenshots/recordings
            // while this path is active. Without affinity support the timer
            // stays off (each capture would blink the panel); only the
            // event-driven refreshes run there.
            Wh_Log(L"[Blur] PATH=software_capture (slider-controlled)");
            pol = {ACCENT_DISABLED, 0, 0, 0};
            if (g_US.themeBlur > 0 && g_CaptureAffinityOK &&
                !SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE))
                g_CaptureAffinityOK = false;
            if (g_US.themeBlur > 0 && g_CaptureAffinityOK)
                g_Timers.Set(hwnd, IDT_SOFTBLUR, 66);  // ~15 fps backdrop
            else
                g_Timers.Kill(IDT_SOFTBLUR);
        }
        fn(hwnd, &d);
    } else {
        ShutdownDComp();
        g_Timers.Kill(IDT_SOFTBLUR);
        SetWindowDisplayAffinity(hwnd, WDA_NONE);
        MARGINS m = {0};
        DwmExtendFrameIntoClientArea(hwnd, &m);
        if (t == Theme::Windows)
            pol = {ACCENT_ENABLE_BLURBEHIND, 0, 0, 0};
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
    // roundf, not truncation: at fractional DPI (125%/150%) truncating made
    // the panel up to 1px shorter than the padding math expects, biasing all
    // vertically-centered content 1px high.
    int panelH = (int)roundf(g_US.panelHeight * scale);

    // Logical panel width: RecomputeLayout already derived this from the
    // active containers (mini mode included) — reuse its result instead of
    // duplicating the container walk here, which had already started to
    // drift from the authoritative version.
    int totalW = g_PanelWidth;

    // Smooth width: ease the logical width toward the layout target instead
    // of snapping (the track-change "pop"). The IDT_WIDTH_ANIM timer re-runs
    // this function every 16ms until the target is reached. First show (or
    // smooth-resize off) snaps directly.
    float& easeW = g_Anim.widthEase;
    if (g_US.smoothResize && IsWindowVisible(hwnd) && easeW > 1.f &&
        fabsf((float)totalW - easeW) > 1.5f) {
        easeW += ((float)totalW - easeW) * 0.28f;
        if (fabsf((float)totalW - easeW) <= 1.5f) {
            easeW = (float)totalW;
        } else {
            g_Timers.Set(hwnd, IDT_WIDTH_ANIM, 16);
        }
        totalW = (int)roundf(easeW);
    } else {
        easeW = (float)totalW;
    }

    int panelW = (int)roundf(totalW * scale);

    int posX, posY;
    if (InlineRectFresh()) {
        // The explorer-side spacer dictates our rect (anchored placement).
        // The offsets apply as a fine-tune nudge from the streamed rect.
        posX = g_InlineRect.x + (int)roundf(g_US.offsetX * scale);
        posY = g_InlineRect.y + (g_InlineRect.h - panelH) / 2 +
               (int)roundf(g_US.offsetY * scale);
    } else {
        // Rect not (yet) streaming — taskbar XAML unreachable or explorer
        // restarting. Float at the taskbar's left edge until it arrives.
        posX = tbRect.left + (int)roundf(g_US.offsetX * scale);
        // (tbH - panelH) / 2 instead of tbH/2 - panelH/2: the two separate
        // integer divisions each truncate, which biases the panel 1px high
        // whenever taskbar and panel heights have different parities.
        posY = tbRect.top + (tbH - panelH) / 2 +
               (int)roundf(g_US.offsetY * scale);
    }

    HRGN hRgn = nullptr;
    if (HasRounding()) {
        // Build the region from the SAME path builder the paint code uses
        // (CustomRoundedPath → GDI+ Region → HRGN). This keeps the window
        // shape and the drawn shape in exact agreement on every corner, and
        // supports per-corner radii, which GDI RoundRect can't express.
        // CustomRoundedPath clamps oversized radii to true pill arcs.
        auto cr = GetCornerRadii();
        GraphicsPath rp;
        CustomRoundedPath(rp, 0.f, 0.f, (float)panelW, (float)panelH,
                          (int)roundf(cr.tl * scale),
                          (int)roundf(cr.tr * scale),
                          (int)roundf(cr.br * scale),
                          (int)roundf(cr.bl * scale));
        Region rgn(&rp);
        HDC hScreenDC = GetDC(NULL);
        if (hScreenDC) {
            Graphics gtmp(hScreenDC);
            hRgn = rgn.GetHRGN(&gtmp);
            ReleaseDC(NULL, hScreenDC);
        }
    }
    if (!hRgn)
        hRgn = CreateRectRgn(0, 0, panelW, panelH);

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
    Color hoverTarget, hoverBgTarget, ctrlBase;
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
        MediaReader _mr_;
        title_out = idleState ? L"Title" : g_M.title;
        artist_out = idleState ? L"Author" : g_M.artist;
        playing_out = g_M.playing;
        art_out = idleState ? nullptr : g_M.art.get();
    }

    Color tc(TextARGB());
    BYTE artistBase = IsSystemLight() ? 0 : 255;
    // Full alpha here — the Artist Opacity setting is applied at the draw
    // site in DrawInfoContainer (was a hardcoded 153 before).
    Color artistColor(255, artistBase, artistBase, artistBase);

    // Controls hover color (icon)
    Color hoverTarget;
    if (g_US.dynamicHover)
        hoverTarget =
            Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    else
        hoverTarget = Color(255, (BYTE)((g_US.hoverIconColor >> 16) & 0xFF),
                            (BYTE)((g_US.hoverIconColor >> 8) & 0xFF),
                            (BYTE)(g_US.hoverIconColor & 0xFF));

    // Hover background color (independently configurable)
    Color hoverBgTarget;
    if (g_US.dynamicHoverBg)
        hoverBgTarget =
            Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    else
        hoverBgTarget = Color(255, (BYTE)((g_US.hoverBgColor >> 16) & 0xFF),
                              (BYTE)((g_US.hoverBgColor >> 8) & 0xFF),
                              (BYTE)(g_US.hoverBgColor & 0xFF));

    // Controls icon base color
    // 0xFF000000 is ReadHexColor's sentinel for "key missing" — treat as white.
    // 0xFFFFFFFF is the default white. Both mean "not customized."
    Color ctrlBase;
    if (g_US.ctrlIconAccentColor) {
        DWORD ac = GetWindowsAccentColor();
        ctrlBase = Color(255, (BYTE)((ac >> 16) & 0xFF),
                         (BYTE)((ac >> 8) & 0xFF),
                         (BYTE)(ac & 0xFF));
    } else if (g_US.ctrlIconDynamic) {
        ctrlBase =
            Color(255, bgPrimary.GetR(), bgPrimary.GetG(), bgPrimary.GetB());
    } else {
        bool ctrlIsDefault = (g_US.ctrlIconColor == 0xFFFFFFFF ||
                              g_US.ctrlIconColor == 0xFF000000);
        if (g_US.adaptiveTheme && ctrlIsDefault) {
            // Follow system: dark icons on light mode, white icons on dark mode.
            BYTE v = IsSystemLight() ? 0x1a : 0xFF;
            ctrlBase = Color(255, v, v, v);
        } else {
            ctrlBase = Color(255, (BYTE)((g_US.ctrlIconColor >> 16) & 0xFF),
                             (BYTE)((g_US.ctrlIconColor >> 8) & 0xFF),
                             (BYTE)(g_US.ctrlIconColor & 0xFF));
        }
    }

    float iconScale = IconScalePx(H);

    return DrawCtx{g,
                   W,
                   H,
                   bgPrimary,
                   bgSecondary,
                   tc,
                   artistColor,
                   hoverTarget,
                   hoverBgTarget,
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
        if (HasRounding()) {
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
        case Theme::Blurred: {
            // Backdrop: DComp gaussian blur renders below this window when
            // available; otherwise composite the software-captured blur so
            // Blur Strength still works.
            if (UsingSoftBlur() && g_ScreenCapCache.bitmap) {
                GraphicsPath clipPath;
                PanelPath(clipPath, W, H);
                g.SetClip(&clipPath);
                g.DrawImage(g_ScreenCapCache.bitmap, 0, 0, W, H);
                g.ResetClip();
            }
            BYTE tintA = (BYTE)(g_US.themeOpacity / 3);
            bool light = g_US.adaptiveTheme && IsSystemLight();
            BYTE base = light ? 245 : 12;
            SolidBrush tintBr(Color(tintA, base, base, base));
            FillShape(&tintBr);
            break;
        }
        case Theme::Neon: {
            // Same backdrop strategy as Blurred, then overlay + neon border.
            if (UsingSoftBlur() && g_ScreenCapCache.bitmap) {
                GraphicsPath clipPath;
                PanelPath(clipPath, W, H);
                g.SetClip(&clipPath);
                g.DrawImage(g_ScreenCapCache.bitmap, 0, 0, W, H);
                g.ResetClip();
            }
            BYTE a = (BYTE)(max(10, (int)(40.f * g_US.themeOpacity / 255.f)));
            Color ov = IsSystemLight() ? Color(a, 255, 255, 255)
                                       : Color(a, 20, 20, 20);
            SolidBrush b(ov);
            FillShape(&b);
            Pen pen(p, 2.0f);
            if (HasRounding()) {
                GraphicsPath bp;
                auto cr = GetCornerRadii();
                CustomRoundedPath(bp, 1.f, 1.f, (float)(W - 2), (float)(H - 2),
                                  max(0, cr.tl - 1), max(0, cr.tr - 1),
                                  max(0, cr.br - 1), max(0, cr.bl - 1));
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
            MediaReader _mr_;
            if (g_M.art)
                UpdateAlbumBlurBg(g_M.art.get(), W, H, g_ArtVersion);
            else
                // No art → drop the previous track's blur instead of keeping
                // a stale cover on screen.
                g_BlurCache.Invalidate();

            // Diagnostic: log once whenever the reason this theme could look
            // broken changes (no art / cache build failed / opacity near 0).
            {
                static int s_lastDiag = -1;
                int diag = (g_M.art ? 1 : 0) | (g_BlurCache.bitmap ? 2 : 0) |
                           ((g_US.themeOpacity < 16) ? 4 : 0);
                if (diag != s_lastDiag) {
                    s_lastDiag = diag;
                    Wh_Log(L"[AlbumBlur] art=%d cache=%d opacity=%d blur=%d",
                           g_M.art ? 1 : 0, g_BlurCache.bitmap ? 1 : 0,
                           g_US.themeOpacity, g_US.themeBlur);
                }
            }

            if (g_BlurCache.bitmap) {
                float alpha = g_US.themeOpacity / 255.f;
                ColorMatrix cm = {{
                    {1, 0, 0, 0,     0},
                    {0, 1, 0, 0,     0},
                    {0, 0, 1, 0,     0},
                    {0, 0, 0, alpha, 0},
                    {0, 0, 0, 0,     1},
                }};
                ImageAttributes ia;
                ia.SetColorMatrix(&cm, ColorMatrixFlagsDefault,
                                  ColorAdjustTypeBitmap);
                if (HasRounding()) {
                    GraphicsPath clipPath;
                    PanelPath(clipPath, W, H);
                    Region clipRgn(&clipPath);
                    g.SetClip(&clipRgn);
                    g.DrawImage(g_BlurCache.bitmap,
                                Rect(0, 0, W, H), 0, 0, W, H,
                                UnitPixel, &ia);
                    g.ResetClip();
                } else {
                    g.DrawImage(g_BlurCache.bitmap,
                                Rect(0, 0, W, H), 0, 0, W, H,
                                UnitPixel, &ia);
                }
            } else {
                // No art to blur (nothing playing, or the source has no
                // thumbnail) — draw the adaptive palette gradient instead of
                // rendering fully transparent, which read as "theme broken".
                BYTE a = (BYTE)g_US.themeOpacity;
                Color top(a, p.GetR(), p.GetG(), p.GetB());
                Color bottom(a, s.GetR(), s.GetG(), s.GetB());
                LinearGradientBrush fb(Rect(0, 0, W, H), top, bottom,
                                       LinearGradientModeVertical);
                FillShape(&fb);
            }
            if (g_US.enableMask) {
                BYTE tone = g_M.isDarkCover ? 20 : 235;
                SolidBrush mb(Color((BYTE)g_US.maskOpacity, tone, tone, tone));
                if (HasRounding()) {
                    GraphicsPath mp;
                    PanelPath(mp, W, H);
                    g.FillPath(&mb, &mp);
                } else
                    g.FillRectangle(&mb, 0, 0, W, H);
            }
            return;  // mask already handled above for AlbumBlur
        }
        case Theme::Windows: {
            bool light = IsSystemLight();
            BYTE base = light ? 240 : 20;
            SolidBrush b(Color(200, base, base, base));
            FillShape(&b);
            break;
        }
        default:
            break;  // Transparent / Blurred — OS handles it
    }

    // Mask overlay for all non-AlbumBlur themes
    if (g_US.enableMask) {
        bool light = IsSystemLight();
        BYTE tone = light ? 235 : 20;
        SolidBrush mb(Color((BYTE)g_US.maskOpacity, tone, tone, tone));
        if (HasRounding()) {
            GraphicsPath mp;
            PanelPath(mp, W, H);
            g.FillPath(&mb, &mp);
        } else
            g.FillRectangle(&mb, 0, 0, W, H);
    }
}

// ── Media / Album Art
// ─────────────────────────────────────────────────────────

// Forward decls — defined in the Controls section below. The art hover
// overlay reuses them so its play/pause icon matches the selected theme.
static void DrawCustomIcon(Graphics& g, int type, float cx, float cy, float s,
                           Color col);
static void DrawRoundedIcon(Graphics& g, int type, float cx, float cy, float s,
                            Color col);
static void DrawGlyphButton(Graphics& g, const Font& fnt, const wchar_t* glyph,
                            float cx, float cy, Color col);

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
        bool light = IsSystemLight();
        Color phBg = light ? Color(255, 200, 200, 200) : Color(255, 55, 55, 60);
        Color phFg =
            light ? Color(255, 130, 130, 135) : Color(255, 100, 100, 108);
        SolidBrush bgBr(phBg);
        g.FillPath(&bgBr, &ap);
        int minDim = min(aW, aS);
        if (minDim >= 14) {
            FontFamily ff(ICON_FONT, nullptr);
            Font f(&ff, (REAL)(minDim * 0.45f), FontStyleRegular, UnitPixel);
            // Ink-centered like the control icons (see DrawGlyphButton).
            DrawGlyphButton(g, f, L"\uE8D6", aX + aW / 2.f, aY + aS / 2.f,
                            phFg);
        }
    } else {
        // Solid placeholder background with no icon
        bool light = IsSystemLight();
        Color phBg = light ? Color(255, 200, 200, 200) : Color(255, 55, 55, 60);
        SolidBrush bgBr(phBg);
        g.FillPath(&bgBr, &ap);
    }

    // ── Hover play/pause overlay ─────────────────────────────────────────────
    // Dark scrim clipped to the art shape + a centered play/pause icon that
    // matches the selected icon theme. Clicking the art toggles playback
    // (handled in WM_LBUTTONUP).
    if (g_US.mediaHoverPlay && g_Anim.artHover > 0.f) {
        float t = g_Anim.artHover;
        SolidBrush scrim(Color((BYTE)(115 * t), 0, 0, 0));
        g.FillPath(&scrim, &ap);

        Color ic((BYTE)(255 * t), 255, 255, 255);
        float cx = aX + aW / 2.f, cy = aY + aS / 2.f;
        float minDim = (float)min(aW, aS);
        int type = ctx.playing ? 3 : 2;  // 3=pause 2=play

        if (g_US.iconTheme == IconTheme::RoundedFilled) {
            DrawRoundedIcon(g, type, cx, cy, minDim / 26.f, ic);
        } else if (g_US.iconTheme == IconTheme::Default) {
            DrawCustomIcon(g, type, cx, cy, minDim / 26.f, ic);
        } else {
            const WCHAR* fontName =
                (g_US.iconTheme >= IconTheme::FluentOutlined)
                    ? L"Segoe Fluent Icons"
                    : L"Segoe MDL2 Assets";
            const wchar_t *gPlay, *gPause;
            switch (g_US.iconTheme) {
                case IconTheme::FluentFilled:
                    gPlay = L"\uF5B0";
                    gPause = L"\uF8AE";
                    break;
                case IconTheme::FluentOutlined2:
                    gPlay = L"\uEE4A";
                    gPause = L"\uEDB4";
                    break;
                default:  // MDL2 / FluentOutlined
                    gPlay = L"\uE768";
                    gPause = L"\uE769";
                    break;
            }
            FontFamily ff(fontName, nullptr);
            Font f(&ff, minDim * 0.4f, FontStyleRegular, UnitPixel);
            DrawGlyphButton(g, f, ctx.playing ? gPause : gPlay, cx, cy, ic);
        }
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

    // When corners are rounded and the bar is at the very top or bottom edge,
    // clip it to the panel shape so it doesn't bleed outside the corner arcs.
    bool needCornerClip = HasRounding() &&
                          (g_US.progressBarLayer == PBLayer::AboveBoth ||
                           g_US.progressBarLayer == PBLayer::UnderBoth);
    if (needCornerClip) {
        GraphicsPath clipPath;
        PanelPath(clipPath, W, H);
        Region panelRgn(&clipPath);
        g.SetClip(&panelRgn);
    }

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

    if (needCornerClip)
        g.ResetClip();
}

// ── Info font cache ──────────────────────────────────────────────────────────
// FontFamily/Font construction is not cheap, and DrawInfoContainer runs every
// frame during the marquee. Cache the objects and rebuild only when the
// settings change or a web font finishes downloading. Message-loop-only.

// Web fonts load SemiBold into the Bold slot of the PrivateFontCollection,
// hence the fixed styles.
static constexpr int kTitleFontStyle = FontStyleBold;
static constexpr int kArtistFontStyle = FontStyleRegular;

struct InfoFontCache {
    std::unique_ptr<FontFamily> titleFF, artistFF;
    std::unique_ptr<Font> title, artist;
    std::wstring titleName, artistName;
    int titleSize = 0, artistSize = 0;
    bool titleWeb = false, artistWeb = false;  // built from the web collection
};
static InfoFontCache g_InfoFonts;

// Resolution order: ready web font → system font of that name → hardcoded
// Segoe fallback. *usedWeb reports which branch won so the cache can rebuild
// the moment a still-downloading web font becomes available.
static std::unique_ptr<FontFamily> ResolveFontFamily(const std::wstring& name,
                                                     const wchar_t* fallback,
                                                     bool* usedWeb) {
    *usedWeb = false;
    if (WebFontState* wf = FindWebFont(name)) {
        auto ff = std::make_unique<FontFamily>(name.c_str(), wf->collection);
        if (ff->GetLastStatus() == Ok) {
            *usedWeb = true;
            return ff;
        }
    }
    auto ff = std::make_unique<FontFamily>(name.c_str(), nullptr);
    if (ff->GetLastStatus() == Ok)
        return ff;
    return std::make_unique<FontFamily>(fallback, nullptr);
}

static void EnsureInfoFonts() {
    bool titleStale =
        !g_InfoFonts.title || g_InfoFonts.titleName != g_US.titleFont ||
        g_InfoFonts.titleSize != g_US.titleFontSize ||
        (!g_InfoFonts.titleWeb && FindWebFont(g_US.titleFont) != nullptr);
    if (titleStale) {
        g_InfoFonts.titleFF = ResolveFontFamily(
            g_US.titleFont, L"Segoe UI Variable Display", &g_InfoFonts.titleWeb);
        g_InfoFonts.title = std::make_unique<Font>(
            g_InfoFonts.titleFF.get(), (REAL)g_US.titleFontSize,
            kTitleFontStyle, UnitPixel);
        g_InfoFonts.titleName = g_US.titleFont;
        g_InfoFonts.titleSize = g_US.titleFontSize;
    }

    bool artistStale =
        !g_InfoFonts.artist || g_InfoFonts.artistName != g_US.artistFont ||
        g_InfoFonts.artistSize != g_US.artistFontSize ||
        (!g_InfoFonts.artistWeb && FindWebFont(g_US.artistFont) != nullptr);
    if (artistStale) {
        g_InfoFonts.artistFF = ResolveFontFamily(
            g_US.artistFont, L"Segoe UI Semibold", &g_InfoFonts.artistWeb);
        g_InfoFonts.artist = std::make_unique<Font>(
            g_InfoFonts.artistFF.get(), (REAL)g_US.artistFontSize,
            kArtistFontStyle, UnitPixel);
        g_InfoFonts.artistName = g_US.artistFont;
        g_InfoFonts.artistSize = g_US.artistFontSize;
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

    // ── Fonts — from the message-loop cache (see EnsureInfoFonts above) ─────
    EnsureInfoFonts();
    FontFamily* pTitleFF = g_InfoFonts.titleFF.get();
    FontFamily* pArtistFF = g_InfoFonts.artistFF.get();
    int titleStyle = kTitleFontStyle;
    int artistStyle = kArtistFontStyle;
    const Font& titleF = *g_InfoFonts.title;
    const Font& artistF = *g_InfoFonts.artist;

    // ── Font metrics ──────────────────────────────────────────────────────────
    // GDI+ MeasureString embeds "internal leading" (extra whitespace) both
    // above and below every glyph run, so using tb.Height for line positioning
    // always leaves a visible gap even when textGap==0.
    //
    // We bypass MeasureString entirely for Y-placement and use FontFamily cell
    // metrics instead, which give us exactly ascent+descent in design units
    // that we can scale to pixels.  The formula is:
    //
    //   pixelH = fontSize * cellUnits / emHeight
    //
    // For the title we only care about its descent (distance from baseline to
    // bottom of glyph cell) and for the artist its ascent (baseline to top).
    // Placing artist at:
    //   titleBaselineY + titleDescent + gap + artistAscent ... and drawing at
    //   that coordinate minus artistAscent (so DrawString top = glyph top)
    // gives zero visible gap when gap==0.
    //
    // DrawString's origin is the top of the bounding box INCLUDING internal
    // leading, so we must shift each draw call UP by the internal leading
    // (= tb.Y, which is negative) to align the glyph top with our computed Y.

    float titlePx   = (float)g_US.titleFontSize;
    float artistPx  = (float)g_US.artistFontSize;

    // Helpers: convert design units -> pixels for a given font size
    auto CellAscPx = [](const FontFamily& fam, int style, float px) -> float {
        int em = fam.GetEmHeight(style);
        return em > 0 ? px * fam.GetCellAscent(style)  / (float)em : px * 0.8f;
    };
    auto CellDescPx = [](const FontFamily& fam, int style, float px) -> float {
        int em = fam.GetEmHeight(style);
        return em > 0 ? px * fam.GetCellDescent(style) / (float)em : px * 0.2f;
    };

    float titleAsc  = CellAscPx (*pTitleFF,  titleStyle,  titlePx);
    float titleDesc = CellDescPx(*pTitleFF,  titleStyle,  titlePx);
    float artistAsc = CellAscPx (*pArtistFF, artistStyle, artistPx);
    float artistDesc= CellDescPx(*pArtistFF, artistStyle, artistPx);

    // Cell height = ascent + descent (no internal leading)
    float titleCellH  = titleAsc  + titleDesc;
    float artistCellH = artistAsc + artistDesc;

    // showArtist toggle — treat artist as absent when disabled
    bool drawArtist = g_US.showArtist && !ctx.artist.empty();

    // Internal leading offsets (what DrawString adds above the glyph)
    // MeasureString gives us the full box height; leading = box - cell
    RectF tb, ab;
    g.MeasureString(ctx.title.c_str(),  -1, &titleF,  RectF(0,0,4000,200), &tb);
    if (drawArtist)
        g.MeasureString(ctx.artist.c_str(), -1, &artistF, RectF(0,0,4000,200), &ab);

    float titleLeading  = tb.Height - titleCellH;   // positive, extra top space
    float artistLeading = drawArtist ? ab.Height - artistCellH : 0.f;

    // Auto-width: report the measured text width so the next layout pass can
    // fit the container to it (capped at InfoMaxWidth — longer text scrolls).
    // Resizing the window mid-paint is unsafe, so post and let the message
    // pump do it. Clamping BEFORE the compare keeps long titles from
    // re-posting forever (desired width pins at the cap).
    if (g_US.infoAutoWidth) {
        float scaleF = (g_US.panelHeight > 0)
                           ? (float)ctx.H / (float)g_US.panelHeight
                           : 1.f;
        float wantPx = tb.Width;
        if (drawArtist)
            wantPx = max(wantPx, ab.Width);
        int wantLogical =
            ClampInt((int)ceilf(wantPx / max(0.01f, scaleF)) + 4, 24,
                     g_US.infoMaxWidth);
        if (abs(wantLogical - g_InfoDesiredW) > 1) {
            g_InfoDesiredW = wantLogical;
            if (g_hWnd)
                PostMessage(g_hWnd, WM_INFO_RELAYOUT, 0, 0);
        }
    }

    float gap    = (float)g_US.textGap;
    float totalH = titleCellH + (drawArtist ? artistCellH + gap : 0.f);

    float pbReserve = 0.f;
    if (g_US.showProgressBar &&
        (g_US.progressBarLayer == PBLayer::UnderText ||
         g_US.progressBarLayer == PBLayer::UnderTextOnly))
        pbReserve = (float)g_US.progressBarHeight + 4.f;

    // sY = top of title cell (glyph top, no internal leading).
    // Pads scaled to pixels first — same DPI-mixing fix as BuildControlLayout.
    float padScale =
        (g_US.panelHeight > 0) ? (float)ctx.H / (float)g_US.panelHeight : 1.f;
    float padT = g_US.wrap.padTop * padScale;
    float padB = g_US.wrap.padBottom * padScale;
    float sY = padT + (ctx.H - padT - padB - totalH - pbReserve) / 2.f;

    // DrawString expects the top of the FULL bounding box (cell + leading),
    // so shift UP by half the leading to center it, or just subtract leading/2.
    // Simplest: pass sY - titleLeading/2 so the cell is centred in the box.
    float titleDrawY  = sY - titleLeading / 2.f;
    // Artist glyph top = sY + titleCellH + gap
    float artistGlyphY = sY + titleCellH + gap;
    float artistDrawY  = artistGlyphY - artistLeading / 2.f;

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
    if (g_US.disableScroll && drawArtist) {
        RectF am;
        g.MeasureString(ctx.artist.c_str(), -1, &artistF,
                        RectF(0, 0, 4000, 200), &am);
        if ((int)am.Width > tW)
            dispArtist = TruncateToFit(ctx.artist, artistF, tW);
    }

    // Crossfade alpha multiplied by the user's per-line opacity settings.
    float alpha = g_Anim.crossfade.alpha;
    float titleA  = alpha * (g_US.titleOpacity / 100.f);
    float artistA = alpha * (g_US.artistOpacity / 100.f);
    Color tcFaded(
        (BYTE)ClampInt((int)(ctx.textColor.GetA() * titleA), 0, 255),
        ctx.textColor.GetR(), ctx.textColor.GetG(), ctx.textColor.GetB());
    Color acFaded(
        (BYTE)ClampInt((int)(ctx.artistColor.GetA() * artistA), 0, 255),
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

    g.DrawString(dispTitle.c_str(), -1, &titleF, PointF(drawX, titleDrawY), &titleBr);
    if (drawArtist)
        g.DrawString(dispArtist.c_str(), -1, &artistF,
                     PointF((float)tX, artistDrawY), &artistBr);

    g.ResetClip();

    // Fade edge for Windows-adaptive theme — matches system light/dark color
    if (g_US.theme == Theme::Windows && tW > 20) {
        bool light = IsSystemLight();
        BYTE fadeBase = light ? 240 : 20;
        int fw = 24, fx = tX + tW - fw;
        LinearGradientBrush fade(PointF((float)fx, 0),
                                 PointF((float)(fx + fw), 0),
                                 Color(0, fadeBase, fadeBase, fadeBase),
                                 Color(200, fadeBase, fadeBase, fadeBase));
        g.FillRectangle(&fade, fx, 0, fw, ctx.H);
    }

    // Progress bar (under-info positions)
    if (g_US.showProgressBar &&
        (g_US.progressBarLayer == PBLayer::UnderText ||
         g_US.progressBarLayer == PBLayer::UnderTextOnly)) {
        const ContainerOrigin* ctrlCO = g_Layout.FindOrigin(3);
        DrawProgressBar(ctx, co, ctrlCO);
    }

    // Keep the marquee timer alive while the text overflows. Progress-bar
    // repaints are driven by IDT_POS, so IDT_ANIM is scroll-only and
    // self-terminates in its handler when scrolling stops.
    if (g_Scroll.active && !g_AnimTimerRunning) {
        g_Timers.Set(g_hWnd, IDT_ANIM, 16);
        g_AnimTimerRunning = true;
    }
}

// ── Panel hover highlight (widget-style)
// ────────────────────────────────────────────────────────────────

// Draws the whole-panel hover background, like hovering the Windows taskbar
// weather widget (Win11: rounded fill + faint edge) or the Win10 clock/tray
// (square fill). Sits above the background theme, below all content.
static void DrawPanelHoverHighlight(DrawCtx& ctx) {
    if (!g_US.panelHoverEnabled)
        return;
    float t = g_Anim.panelHover;
    if (t <= 0.f)
        return;

    // Default white doubles as "automatic": white glow on dark mode, dark
    // tint on light mode — matching how the real widgets adapt.
    Color base;
    if (g_US.panelHoverColor == 0xFFFFFFFF)
        base = IsSystemLight() ? Color(255, 0, 0, 0)
                               : Color(255, 255, 255, 255);
    else
        base = Color(255, (BYTE)((g_US.panelHoverColor >> 16) & 0xFF),
                     (BYTE)((g_US.panelHoverColor >> 8) & 0xFF),
                     (BYTE)(g_US.panelHoverColor & 0xFF));

    BYTE a = (BYTE)min(255, (int)(g_US.panelHoverOpacity * 2.55f * t));
    if (a == 0)
        return;
    SolidBrush br(Color(a, base.GetR(), base.GetG(), base.GetB()));

    // The highlight is always the panel's exact shape — same per-corner
    // radii as everything else (square panel → square highlight, pill →
    // pill). Fill plus a faint inset edge stroke for the widget look.
    auto cr = GetCornerRadii();
    GraphicsPath hp;
    CustomRoundedPath(hp, 0.f, 0.f, (float)ctx.W, (float)ctx.H,
                      cr.tl, cr.tr, cr.br, cr.bl);
    ctx.g.FillPath(&br, &hp);
    Pen edge(Color((BYTE)(a / 2), base.GetR(), base.GetG(), base.GetB()),
             1.f);
    GraphicsPath ep;
    CustomRoundedPath(ep, 1.f, 1.f, (float)(ctx.W - 2), (float)(ctx.H - 2),
                      max(0, cr.tl - 1), max(0, cr.tr - 1),
                      max(0, cr.br - 1), max(0, cr.bl - 1));
    ctx.g.DrawPath(&edge, &ep);
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
    if (g_US.vizAccentColor && g_US.vizMode == VizMode::Solid) {
        DWORD ac = GetWindowsAccentColor();
        baseCol = Color(255, (BYTE)((ac >> 16) & 0xFF),
                        (BYTE)((ac >> 8) & 0xFF),
                        (BYTE)(ac & 0xFF));
    } else if (g_US.vizMode == VizMode::DynamicAlbum) {
        baseCol = Color(255, ctx.bgPrimary.GetR(), ctx.bgPrimary.GetG(),
                        ctx.bgPrimary.GetB());
    } else {
        bool vizIsDefault = (g_US.vizColor == 0xFFFFFFFF ||
                             g_US.vizColor == 0xFF000000);
        if (g_US.vizMode == VizMode::Solid &&
            g_US.adaptiveTheme && vizIsDefault) {
            // Follow system: dark bars on light mode, white bars on dark mode.
            BYTE v = IsSystemLight() ? 0x1a : 0xFF;
            baseCol = Color(255, v, v, v);
        } else {
            baseCol = Color(255, (BYTE)((g_US.vizColor >> 16) & 0xFF),
                            (BYTE)((g_US.vizColor >> 8) & 0xFF),
                            (BYTE)(g_US.vizColor & 0xFF));
        }
    }

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
                // fac is 0..1 — scale it into the alpha range, otherwise the
                // expression maxes out at ~30 and the bars never react.
                BYTE aa = (BYTE)(max(30, min(180, (int)(150.f * fac + 30.f))));
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

    if (HasRounding()) {
        GraphicsPath clipPath;
        PanelPath(clipPath, W, ctx.H);
        Region clipRgn(&clipPath);
        ctx.g.SetClip(&clipRgn);
    }
    DrawVisualizerBars(ctx, v.zoneY, v.zoneH, bgBarW, bgStep, bgStartX,
                       v.maxBarH, v.minBarH, barCount);
    if (HasRounding())
        ctx.g.ResetClip();
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
                {X(-3.5f), Y(-6.f)}, {X(5.5f), Y(0)}, {X(-3.5f), Y(6.f)}};
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

// Fills a polygon and re-strokes its outline with a round-join pen of width
// 2r — the stroke bulges r outward at every corner, turning sharp vertices
// into clean circular arcs. The polygon should be drawn r smaller than the
// intended final size so the stroked result lands on target.
static void FillRoundedPoly(Graphics& g,
                            Color col,
                            const PointF* pts,
                            int n,
                            float r) {
    GraphicsPath path;
    path.AddPolygon(pts, n);
    SolidBrush b(col);
    g.FillPath(&b, &path);
    if (r > 0.f) {
        Pen pen(col, r * 2.f);
        pen.SetLineJoin(LineJoinRound);
        g.DrawPath(&pen, &path);
    }
}

// "Rounded Filled" icon pack — same shapes as DrawCustomIcon but every
// corner is rounded (round joins) and every bar is a capsule (round caps).
// Same type ids: 1=Prev 2=Play 3=Pause 4=Next 5=Speaker.
static void DrawRoundedIcon(Graphics& g,
                            int type,
                            float cx,
                            float cy,
                            float s,
                            Color col) {
    auto X = [&](float v) { return cx + v * s; };
    auto Y = [&](float v) { return cy + v * s; };
    auto W = [&](float v) { return v * s; };

    // Capsule bar: a thick line with round caps.
    auto Bar = [&](float x, float y0, float y1, float thick) {
        Pen p(col, W(thick));
        p.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        g.DrawLine(&p, X(x), Y(y0), X(x), Y(y1));
    };

    // NOTE on coordinates: each icon's full extent INCLUDING the rounding
    // stroke is kept symmetric around (cx, cy) — a lone triangle's bounding
    // box is otherwise lopsided, which made the row look off-center next to
    // the perfectly symmetric pause capsules.
    switch (type) {
        case 1: {  // Prev — single rounded triangle pointing left, no bar
            PointF pts[] = {
                {X(3.3f), Y(-4.2f)}, {X(-3.3f), Y(0)}, {X(3.3f), Y(4.2f)}};
            FillRoundedPoly(g, col, pts, 3, W(1.7f));
            break;
        }
        case 2: {  // Play — rounded triangle
            PointF pts[] = {
                {X(-3.3f), Y(-4.2f)}, {X(3.3f), Y(0)}, {X(-3.3f), Y(4.2f)}};
            FillRoundedPoly(g, col, pts, 3, W(1.7f));
            break;
        }
        case 3: {  // Pause — two capsule bars
            Bar(-2.7f, -4.5f, 4.5f, 3.f);
            Bar(2.7f, -4.5f, 4.5f, 3.f);
            break;
        }
        case 4: {  // Next — single rounded triangle pointing right, no bar
            PointF pts[] = {
                {X(-3.3f), Y(-4.2f)}, {X(3.3f), Y(0)}, {X(-3.3f), Y(4.2f)}};
            FillRoundedPoly(g, col, pts, 3, W(1.7f));
            break;
        }
        case 5: {  // Speaker — rounded cone + round-cap sound arcs.
            // Shifted +0.7 vs the classic layout so cone + arcs together
            // balance around the center.
            PointF cone[] = {{X(-5.3f), Y(-2.2f)}, {X(-2.9f), Y(-2.2f)},
                             {X(-0.3f), Y(-5.6f)}, {X(-0.3f), Y(5.6f)},
                             {X(-2.9f), Y(2.2f)}, {X(-5.3f), Y(2.2f)}};
            FillRoundedPoly(g, col, cone, 6, W(1.f));
            Pen p2(col, W(1.6f));
            p2.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
            g.DrawArc(&p2, X(-4.8f), Y(-4.f), W(8.f), W(8.f), -50, 100);
            g.DrawArc(&p2, X(-7.8f), Y(-7.f), W(14.f), W(14.f), -50, 100);
            break;
        }
    }
}

// Draws an icon-font glyph optically centered at (cx, cy). The glyph is
// converted to a GraphicsPath and centered on its INK bounds — the old
// MeasureString approach centered the text cell, which includes invisible
// font padding (internal leading, side bearings) that varies per font and
// glyph, leaving icons visibly off-center in their hover boxes and biased
// vertically. Path fill also honours the panel's anti-aliasing mode.
static void DrawGlyphButton(Graphics& g,
                            const Font& fnt,
                            const wchar_t* glyph,
                            float cx,
                            float cy,
                            Color col) {
    FontFamily ff;
    if (fnt.GetFamily(&ff) != Ok)
        return;
    GraphicsPath path;
    if (path.AddString(glyph, -1, &ff, fnt.GetStyle(), fnt.GetSize(),
                       PointF(0.f, 0.f),
                       StringFormat::GenericTypographic()) != Ok)
        return;
    RectF b;
    path.GetBounds(&b);
    if (b.Width <= 0.f || b.Height <= 0.f)
        return;
    float dx = cx - (b.X + b.Width / 2.f);
    float dy = cy - (b.Y + b.Height / 2.f);
    SolidBrush br(col);
    if (!g_US.iconSmoothRendering) {
        // Classic mode: rasterize through the font engine (grid-fitted,
        // crisper at small sizes) but position with the SAME ink-bounds
        // offset, so centering is identical in both modes. Typographic
        // format here too — it must match the AddString layout origin.
        g.DrawString(glyph, -1, &fnt, PointF(dx, dy),
                     StringFormat::GenericTypographic(), &br);
        return;
    }
    Matrix m;
    m.Translate(dx, dy);
    path.Transform(&m);
    g.FillPath(&br, &path);
}

// Volume popup geometry, scaled by panel height like every other control.
// (Was hardcoded 96-DPI pixels, which misaligned the drawn slider against
// its hit/drag math at other panel heights and DPI scales.) Shared by
// DrawVolumeSlider and the WM_LBUTTONDOWN / WM_MOUSEMOVE drag handlers.
struct VolGeom {
    float popX, popW, popH;  // popup pill background
    float slX, slW, slH;     // slider track
    float thumbR;
};
static VolGeom GetVolGeom(float vX, int H) {
    float k = (float)H / PANEL_HEIGHT_REF;
    VolGeom v;
    v.popX = vX + 18.f * k;
    v.popW = 74.f * k;
    v.popH = 12.f * k;
    v.slX = v.popX + 7.f * k;
    v.slW = 60.f * k;
    v.slH = 4.f * k;
    v.thumbR = 4.f * k;
    return v;
}

static void DrawVolumeSlider(DrawCtx& ctx) {
    if (!g_US.showSpeakerIcon || !g_Vol.hovered)
        return;
    Graphics& g = ctx.g;
    float cY = g_Layout.ctrl.cY;
    VolGeom v = GetVolGeom(g_Layout.ctrl.vX, ctx.H);

    float bgY = cY - v.popH / 2.f;
    GraphicsPath vp;
    RoundRectPath(vp, v.popX, bgY, v.popW, v.popH, v.popH / 2.f);
    Color popupBg =
        IsSystemLight() ? Color(230, 245, 245, 245) : Color(200, 30, 30, 30);
    SolidBrush pBr(popupBg);
    g.FillPath(&pBr, &vp);

    float slTop = cY - v.slH / 2.f;
    SolidBrush track(Color(60, ctx.textColor.GetR(), ctx.textColor.GetG(),
                           ctx.textColor.GetB()));
    GraphicsPath tp;
    RoundRectPath(tp, v.slX, slTop, v.slW, v.slH, v.slH / 2.f);
    g.FillPath(&track, &tp);

    float filled = g_Vol.level * v.slW;
    if (filled > 0) {
        SolidBrush fill(ctx.textColor);
        GraphicsPath fp;
        RoundRectPath(fp, v.slX, slTop, filled, v.slH, v.slH / 2.f);
        g.FillPath(&fill, &fp);
    }
    SolidBrush fill2(ctx.textColor);
    g.FillEllipse(&fill2, v.slX + filled - v.thumbR, cY - v.thumbR,
                  v.thumbR * 2.f, v.thumbR * 2.f);
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
        MediaReader _mr_;
        shOn = g_M.shuffleOn;
        repMode = g_M.repeatMode;
    }

    // Hover background — square with configurable corner radius for every button.
    // Disable HoverBgEnabled to get icon color change only (no background).
    if (g_US.hoverBgEnabled) {
        struct BtnInfo { int idx; float x; float r; bool valid; };
        // Play/Pause hover box matches its icon size — big when the hero
        // toggle is on, identical to the others when it's off.
        float ppBoxR = g_US.bigPlayButton ? ppBigR + 1.f : pnSmallR + 3.f;
        BtnInfo buttons[] = {
            {1, pX,       pnSmallR + 3.f, g_US.showPrevBtn && pX  > 0.f},
            {2, ppX,      ppBoxR,         g_US.showPlayBtn && ppX > 0.f},
            {3, nX,       pnSmallR + 3.f, g_US.showNextBtn && nX  > 0.f},
            {4, vX,       pnSmallR + 3.f, g_US.showSpeakerIcon && vX > 0.f},
            {5, shuffleX, pnSmallR + 3.f, g_US.showShuffleButton && shuffleX > 0.f},
            {6, repeatX,  pnSmallR + 3.f, g_US.showRepeatButton  && repeatX  > 0.f},
        };
        int bgR = g_US.hoverBgRadius;
        Color htBg = ctx.hoverBgTarget;
        for (const auto& btn : buttons) {
            if (!btn.valid) continue;
            float t = g_Anim.hoverProgress[btn.idx];
            if (t <= 0.f) continue;
            int a = (int)(g_US.hoverBgOpacity * 2.55f * t);
            SolidBrush hb(Color((BYTE)a, htBg.GetR(), htBg.GetG(), htBg.GetB()));
            float side = btn.r * 2.f;
            float bx = btn.x - btn.r;
            float by = cY - btn.r;
            if (bgR <= 0) {
                g.FillRectangle(&hb, bx, by, side, side);
            } else {
                // Float-precise path: the old int-truncated RoundRectPath
                // shifted the box up-left by up to 1px relative to the
                // float-centered icon, which read as the icon sitting
                // off-center in its box. CustomRoundedPath also clamps
                // oversized radii, so bgR >= half the box = perfect circle.
                GraphicsPath hp;
                CustomRoundedPath(hp, bx, by, side, side, bgR, bgR, bgR, bgR);
                g.FillPath(&hb, &hp);
            }
        }
    }

    bool customDrawn = (g_US.iconTheme == IconTheme::Default ||
                        g_US.iconTheme == IconTheme::RoundedFilled);
    if (!customDrawn) {
        // ── Glyph-based icon themes
        // ───────────────────────────────────────────
        const WCHAR* fontName = (g_US.iconTheme >= IconTheme::FluentOutlined)
                                    ? L"Segoe Fluent Icons"
                                    : L"Segoe MDL2 Assets";
        FontFamily iconFF(fontName, nullptr);
        Font iconFont(&iconFF, 14.f * iconScale, FontStyleRegular, UnitPixel);
        // Hero Play/Pause (optional): ~25% larger than its neighbours,
        // matching the custom-drawn packs (1.15x vs 0.9x). Hover box and hit
        // box follow the same toggle.
        Font ppFont(&iconFF, (g_US.bigPlayButton ? 17.5f : 14.f) * iconScale,
                    FontStyleRegular, UnitPixel);

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

        if (g_US.showPrevBtn && pX > 0.f)
            DrawGlyphButton(g, iconFont, gPrev, pX, cY, ButtonColor(ctx, 1));
        if (g_US.showPlayBtn && ppX > 0.f)
            DrawGlyphButton(g, ppFont, ctx.playing ? gPause : gPlay, ppX, cY,
                            ButtonColor(ctx, 2));
        if (g_US.showNextBtn && nX > 0.f)
            DrawGlyphButton(g, iconFont, gNext, nX, cY, ButtonColor(ctx, 3));
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
        // ── Custom-drawn icon packs (Default and Rounded Filled) ─────────
        bool rounded = (g_US.iconTheme == IconTheme::RoundedFilled);
        auto DrawIcon = [&](int type, float x, float y, float sc, Color c) {
            if (rounded)
                DrawRoundedIcon(g, type, x, y, sc, c);
            else
                DrawCustomIcon(g, type, x, y, sc, c);
        };
        // Hero Play/Pause (optional): 1.15x vs 0.9x for its neighbours —
        // same ratio the glyph themes use via ppFont.
        if (g_US.showPlayBtn && ppX > 0.f)
            DrawIcon(ctx.playing ? 3 : 2, ppX, cY,
                     (g_US.bigPlayButton ? 1.15f : 0.9f) * iconScale,
                     ButtonColor(ctx, 2));
        if (g_US.showPrevBtn && pX > 0.f)
            DrawIcon(1, pX, cY, 0.9f * iconScale, ButtonColor(ctx, 1));
        if (g_US.showNextBtn && nX > 0.f)
            DrawIcon(4, nX, cY, 0.9f * iconScale, ButtonColor(ctx, 3));
        if (g_US.showSpeakerIcon)
            DrawIcon(5, vX, cY, 0.9f * iconScale, ButtonColor(ctx, 4));

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
    // AntiAliasGridFit renders clean text on transparent/layered windows.
    // ClearTypeGridFit requires a known opaque background to sub-pixel blend
    // against — on a transparent surface it produces colored fringe artifacts
    // and looks especially blurry/harsh on light or white backgrounds.
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.Clear(Color(0, 0, 0, 0));

    EnsureLayout(W, H);

    // Palette from album art
    Color bgPrimary, bgSecondary;
    {
        MediaReader _mr_;
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

    // 1b. Widget-style hover highlight (above background, below content)
    DrawPanelHoverHighlight(ctx);

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
    // Same gate as DrawControlsContainer: when the Controls container is
    // disabled, BuildControlLayout still computes fallback button positions
    // (anchored at the right edge — wherever another container may now sit),
    // so without this check invisible buttons stay clickable. That was the
    // "clicking the visualizer skips the track" bug.
    if (!IsContainerEnabled(3))
        return MediaCmd::None;

    EnsureLayout(W, H);
    const ControlLayout& cl = g_Layout.ctrl;
    float cY = cl.cY, nX = cl.nX, ppX = cl.ppX, pX = cl.pX, vX = cl.vX;
    float shuffleX = cl.shuffleX, repeatX = cl.repeatX;
    float ppBigR = ICON_SIZE_REF * IconScalePx(H);
    float pnSmR = 9.f * IconScalePx(H);

    if (g_US.showSpeakerIcon) {
        // Hover zone scaled like the popup itself (see GetVolGeom).
        float k = (float)H / PANEL_HEIGHT_REF;
        float volR = g_Vol.hovered ? (vX + 95.f * k) : (vX + 14.f * k);
        if (mx >= vX - 12.f * k && mx <= volR && my >= cY - 14.f * k &&
            my <= cY + 14.f * k)
            return MediaCmd::Volume;
    }
    if (g_US.showPrevBtn && pX > 0.f && mx >= pX - (pnSmR + 4) &&
        mx <= pX + (pnSmR + 4) && my >= cY - (pnSmR + 4) &&
        my <= cY + (pnSmR + 4))
        return MediaCmd::Prev;
    float ppHitR = g_US.bigPlayButton ? ppBigR : (pnSmR + 4);
    if (g_US.showPlayBtn && ppX > 0.f && mx >= ppX - ppHitR &&
        mx <= ppX + ppHitR && my >= cY - ppHitR && my <= cY + ppHitR)
        return MediaCmd::PlayPause;
    if (g_US.showNextBtn && nX > 0.f && mx >= nX - (pnSmR + 4) &&
        mx <= nX + (pnSmR + 4) && my >= cY - (pnSmR + 4) &&
        my <= cY + (pnSmR + 4))
        return MediaCmd::Next;
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

// Foreground-window fullscreen check. Catches borderless-fullscreen apps
// that don't go true D3D exclusive (most modern games), without the false
// positives QUNS_BUSY produces (Focus Assist, presentation mode, etc.).
//
// Important on multi-monitor setups: we only treat the foreground app as
// "fullscreening us" if it's on the SAME monitor as our panel. Otherwise
// clicking around on a second monitor (where a maximized window can plausibly
// look fullscreen) would hide the panel on the primary monitor — which was
// the regression after the first pass of this fix.
static bool IsForegroundFullscreen(HWND panelHwnd) {
    HWND fg = GetForegroundWindow();
    if (!fg || !IsWindowVisible(fg) || IsIconic(fg))
        return false;
    if (fg == panelHwnd)
        return false;

    WCHAR cls[64]{};
    GetClassNameW(fg, cls, 64);
    if (!wcscmp(cls, L"Shell_TrayWnd") ||
        !wcscmp(cls, L"Shell_SecondaryTrayWnd") ||
        !wcscmp(cls, L"WorkerW") ||
        !wcscmp(cls, L"Progman") ||
        !wcscmp(cls, L"Windows.UI.Core.CoreWindow"))
        return false;

    // Canonical fullscreen-style check. A real fullscreen app (game,
    // borderless video, F11 browser) strips WS_CAPTION and WS_THICKFRAME.
    // A normal maximized window keeps them, so this is what stops the
    // "any maximized app hides the panel" flicker.
    LONG style = GetWindowLongW(fg, GWL_STYLE);
    if (style & (WS_CAPTION | WS_THICKFRAME))
        return false;

    // Only hide if the fullscreen app is on the SAME monitor as our panel.
    HMONITOR fgMon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    HMONITOR panelMon = panelHwnd
        ? MonitorFromWindow(panelHwnd, MONITOR_DEFAULTTOPRIMARY)
        : MonitorFromWindow(GetShellWindow(), MONITOR_DEFAULTTOPRIMARY);
    if (fgMon != panelMon)
        return false;

    RECT wr;
    if (!GetWindowRect(fg, &wr))
        return false;
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(fgMon, &mi))
        return false;

    return wr.left   <= mi.rcMonitor.left   + 1 &&
           wr.top    <= mi.rcMonitor.top    + 1 &&
           wr.right  >= mi.rcMonitor.right  - 1 &&
           wr.bottom >= mi.rcMonitor.bottom - 1;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        // ── Lifecycle
        // ─────────────────────────────────────────────────────────────
        case WM_CREATE:
            ApplyAppearance(hwnd);
            HookTaskbar(hwnd);  // calls UpdateBoundsAndRegion once with ratio=1.0
            {
                float prevRatio = g_ArtAspectRatio;
                FetchMedia();
                // FetchAlbumArt may have updated g_ArtAspectRatio with the real
                // album art dimensions.  If it changed, RecomputeLayout ran with
                // ratio=1.0 but g_ContainerWidths[1] is now stale — the first
                // paint would draw art wider than its allocated cell and overflow
                // into the next container.  Re-layout before the first WM_PAINT.
                if (g_US.mediaAutoSize && g_US.mediaEnabled &&
                    g_ArtAspectRatio != prevRatio) {
                    RecomputeLayout();
                    UpdateBoundsAndRegion(hwnd);
                }
            }
            ApplyTimerRates(hwnd);
            RefreshScreenBlurCache(hwnd);
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

            // Joinable-based, not gated on g_CaptureRunning: the thread may
            // have exited on its own (clearing the flag) but must still be
            // joined, or the static std::thread destructor std::terminates
            // Explorer at mod unload.
            g_CaptureRunning.store(false);
            if (g_hCaptureEvent)
                SetEvent(g_hCaptureEvent);
            if (g_CaptureThread.joinable()) {
                HANDLE hRaw = (HANDLE)g_CaptureThread.native_handle();
                // Give the thread a genuine chance to unwind — it releases
                // WASAPI COM objects on exit. Re-signal the event and wait
                // long enough that a mid-transition audio device can still
                // respond; only detach as a last resort (a stuck device) so
                // Explorer shutdown isn't hung.
                if (g_hCaptureEvent)
                    SetEvent(g_hCaptureEvent);
                if (WaitForSingleObject(hRaw, 2000) == WAIT_OBJECT_0)
                    g_CaptureThread.join();
                else
                    g_CaptureThread.detach();
            }
            if (g_hCaptureEvent) {
                CloseHandle(g_hCaptureEvent);
                g_hCaptureEvent = nullptr;
            }

            SmtcUnsubscribeSession();
            if (g_Mgr) {
                try { g_Mgr.CurrentSessionChanged(g_SmtcMgrSessionToken); } catch(...) {}
                g_Mgr = nullptr;
            }
            g_Vol.cachedIface = nullptr;
            g_Vol.cachedAumid.clear();
            g_BlurCache.Invalidate();
            g_ScreenCapCache.Invalidate();
            g_SoftCap.Release();
            // Release cached GDI+ font objects while GDI+ is still alive
            // (MediaThread calls GdiplusShutdown after the message loop).
            g_InfoFonts = {};
            g_Anim.widthEase = 0.f;
            ShutdownDComp();

            {
                MediaWriter _mw_;
                _mw_->art.reset();
            }
            DestroyBackBuffer();
            PostQuitMessage(0);
            return 0;

        // Per-monitor DPI v2: Windows sends this when the panel moves to a
        // monitor with a different DPI.  LOWORD(lp)/HIWORD(lp) is the suggested
        // new position/size — we ignore it and let UpdateBoundsAndRegion
        // recompute from the taskbar rect at the new DPI instead.
        case WM_DPICHANGED:
            RecomputeLayout();
            UpdateBoundsAndRegion(hwnd);
            g_LayoutDirty = true;
            RefreshScreenBlurCache(hwnd);
            DestroyBackBuffer();
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_SETTINGCHANGE:
            ApplyAppearance(hwnd);
            UpdateBoundsAndRegion(hwnd);
            g_LayoutDirty = true;
            InvalidateLightCache();
            g_ScreenCapCache.Invalidate();
            RefreshScreenBlurCache(hwnd);
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
                g_BreatheEnv = 0.f;
            }
            return 0;

        // ── Timers
        // ────────────────────────────────────────────────────────────────
        case WM_TIMER:
            switch (wp) {
                case IDT_POLL: {
                    float prevRatio = g_ArtAspectRatio;
                    int prevArtVer = g_ArtVersion;
                    bool prevHasMedia, prevPlaying;
                    {
                        MediaReader r;
                        prevHasMedia = r->hasMedia;
                        prevPlaying  = r->playing;
                    }
                    bool prevIdleState = g_IdleState;
                    bool prevIdleHide  = g_IdleHide;
                    FetchMedia();
                    if (g_US.mediaAutoSize && g_US.mediaEnabled &&
                        g_ArtAspectRatio != prevRatio) {
                        RecomputeLayout();
                        UpdateBoundsAndRegion(hwnd);
                    }
                    // Idle screen — wall-clock based. IDT_POLL also fires from
                    // SMTC event posts, so counting ticks would inflate time.
                    {
                        ULONGLONG now = GetTickCount64();
                        MediaReader _mr_;
                        if (g_US.idleScreenEnabled && !g_M.hasMedia) {
                            if (!g_NoMediaSince)
                                g_NoMediaSince = now;
                            if (now - g_NoMediaSince >=
                                (ULONGLONG)g_US.idleScreenDelay * 1000)
                                g_IdleState = true;
                        } else {
                            g_NoMediaSince = 0;
                            g_IdleState = false;
                        }
                    }
                    // Idle timeout (hide) — gated on the explicit toggle so
                    // idleTimeout=0 means "hide immediately on idle", not
                    // "disabled". Same shape as IdleScreenEnabled/Delay.
                    if (g_US.idleHideEnabled) {
                        ULONGLONG now = GetTickCount64();
                        bool isPlaying;
                        {
                            MediaReader _mr_;
                            isPlaying = g_M.playing;
                        }
                        if (!isPlaying) {
                            if (!g_IdleSince)
                                g_IdleSince = now;
                            if (now - g_IdleSince >=
                                (ULONGLONG)g_US.idleTimeout * 1000)
                                g_IdleHide = true;
                        } else {
                            g_IdleSince = 0;
                            g_IdleHide = false;
                        }
                    } else {
                        g_IdleSince = 0;
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
                    {
                        bool changed = (g_ArtVersion != prevArtVer) ||
                                       (g_IdleState  != prevIdleState) ||
                                       (g_IdleHide   != prevIdleHide);
                        if (!changed) {
                            MediaReader r;
                            changed = (r->hasMedia != prevHasMedia) ||
                                      (r->playing  != prevPlaying);
                        }
                        if (changed)
                            InvalidateRect(hwnd, NULL, FALSE);
                    }
                    ApplyTimerRates(hwnd);
                    break;
                }

                case IDT_FULLSCREEN: {
                    // QUNS_BUSY is too broad — it fires for Focus Assist,
                    // presentation mode, notification-suppression states, and
                    // assorted background conditions that have nothing to do
                    // with a fullscreen app. Once tripped, the panel hides
                    // until the state clears, which sometimes never happens
                    // cleanly. That was the "vanishes after ~1h" bug.
                    // Use strict D3D fullscreen + a foreground-window bounds
                    // check to still cover borderless-fullscreen games.
                    bool isFS = false;
                    QUERY_USER_NOTIFICATION_STATE qs;
                    if (SUCCEEDED(SHQueryUserNotificationState(&qs)))
                        isFS = (qs == QUNS_RUNNING_D3D_FULL_SCREEN);
                    if (!isFS)
                        isFS = IsForegroundFullscreen(hwnd);

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
                    // Poll timeline position for the smooth progress bar —
                    // from the session the panel displays (ActiveSession),
                    // which made the old per-tick AUMID scan redundant.
                    if (g_US.showProgressBar && g_Mgr) {
                        try {
                            auto ses = ActiveSession();
                            // Null-guard the getters: they return null while
                            // the session tears down, and a method call on a
                            // null winrt object AVs (not catchable as a C++
                            // exception). See SelectSession note.
                            GlobalSystemMediaTransportControlsSessionTimelineProperties
                                tl = nullptr;
                            GlobalSystemMediaTransportControlsSessionPlaybackInfo
                                pbi = nullptr;
                            if (ses) {
                                tl = ses.GetTimelineProperties();
                                pbi = ses.GetPlaybackInfo();
                            }
                            if (tl && pbi) {
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
                        }
                        WH_CATCH(L"IDT_POS/timeline")
                    }
                    break;
                }

                case IDT_ANIM: {
                    // Marquee driver. Ticks at 16ms but advances on every 3rd
                    // tick (~50ms) to keep the classic scroll speed. Lives
                    // here — NOT in IDT_POS — so the text keeps scrolling
                    // while playback is paused (ApplyTimerRates kills IDT_POS
                    // when nothing is playing).
                    if (!g_Scroll.active) {
                        g_Timers.Kill(IDT_ANIM);
                        g_AnimTimerRunning = false;
                        break;
                    }
                    if (++g_Scroll.tickDiv < 3)
                        break;
                    g_Scroll.tickDiv = 0;
                    if (g_Scroll.waitTicks > 0) {
                        --g_Scroll.waitTicks;
                        break;
                    }
                    int step = (int)g_US.scrollSpeed == 0   ? 1
                               : (int)g_US.scrollSpeed == 1 ? 2
                                                            : 3;
                    int maxOff = max(0, g_Scroll.textW - g_Scroll.visW + 10);
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
                    break;
                }

                case IDT_HOVER_ANIM: {
                    bool needsAnim = false;
                    // Whole-panel hover highlight (widget-style)
                    if (g_US.panelHoverEnabled) {
                        float tgt = g_PanelHovered ? 1.f : 0.f;
                        float& ph = g_Anim.panelHover;
                        if (ph != tgt) {
                            float step = 0.15f;
                            ph = (ph < tgt) ? min(tgt, ph + step)
                                            : max(tgt, ph - step);
                            needsAnim = true;
                        }
                    }
                    // Album-art play/pause overlay
                    if (g_US.mediaHoverPlay) {
                        float tgt = g_ArtHovered ? 1.f : 0.f;
                        float& ah = g_Anim.artHover;
                        if (ah != tgt) {
                            float step = 0.2f;
                            ah = (ah < tgt) ? min(tgt, ah + step)
                                            : max(tgt, ah - step);
                            needsAnim = true;
                        }
                    }
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
                            float step = 0.25f;
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
                    // Delegate entirely to TextCrossfade::Tick().
                    // Tick() returns true if a repaint is needed (always while
                    // active).  When it reports a commit, we write those
                    // strings to g_M under its mutex — the only place that
                    // happens.
                    if (g_Anim.crossfade.active) {
                        TextCrossfade::CommitData commit;
                        bool needRepaint = g_Anim.crossfade.Tick(&commit);
                        if (!commit.title.empty()) {
                            MediaWriter w;
                            w->title = commit.title;
                            w->artist = commit.artist;
                        }
                        if (!g_Anim.crossfade.active)
                            g_Timers.Kill(IDT_TEXT_FADE);
                        if (needRepaint)
                            InvalidateRect(hwnd, NULL, FALSE);
                    } else {
                        g_Timers.Kill(IDT_TEXT_FADE);
                    }
                    break;
                }

                case IDT_WIDTH_ANIM: {
                    // One easing step per tick; UpdateBoundsAndRegion re-arms
                    // the timer itself while the width is still in motion.
                    g_Timers.Kill(IDT_WIDTH_ANIM);
                    UpdateBoundsAndRegion(hwnd);
                    g_LayoutDirty = true;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                }

                case IDT_SOFTBLUR: {
                    // Periodic backdrop re-capture for the software blur
                    // path. The panel is persistently excluded from capture,
                    // so each tick is a small BitBlt + memcmp; the blur
                    // pipeline and repaint only run when the pixels behind
                    // the panel actually changed.
                    if (!UsingSoftBlur() || g_US.themeBlur <= 0 ||
                        !g_CaptureAffinityOK) {
                        g_Timers.Kill(IDT_SOFTBLUR);
                        break;
                    }
                    if (IsWindowVisible(hwnd) &&
                        RefreshScreenBlurCache(hwnd, true))
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
                        // Sensitivity above 100 earns a snappier decay so the
                        // bars feel more reactive at high gain, not just taller.
                        float sensBoost = max(0.f, (g_US.vizSensitivity - 100) / 200.f) * 0.12f;
                        switch (g_US.vizShape) {
                            case VizShape::Stereo:
                                attack = 0.72f;
                                decay = 0.22f + sensBoost;
                                break;
                            case VizShape::Mirror:
                                attack = 0.52f;
                                decay = 0.20f + sensBoost;
                                break;
                            case VizShape::Wave:
                                attack = 0.34f;
                                decay = 0.17f + sensBoost;
                                break;
                            case VizShape::Breathe:
                                attack = 0.20f;
                                decay = 0.11f + sensBoost;
                                break;
                            default:
                                decay += sensBoost;
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
                                    d = 0.26f + sensBoost;
                                } else if (dist < 1.5f) {
                                    a = 0.62f;
                                    d = 0.20f + sensBoost;
                                } else {
                                    a = 0.92f;
                                    d = 0.34f + sensBoost;
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
                VolGeom vg = GetVolGeom(g_Layout.ctrl.vX, rc.bottom);
                SetVolume(((float)LOWORD(lp) - vg.slX) / vg.slW);
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
                VolGeom vg = GetVolGeom(g_Layout.ctrl.vX, rc.bottom);
                SetVolume(((float)mx - vg.slX) / vg.slW);
            }

            // Whole-panel hover (widget-style highlight) — first move after
            // entering arms the fade-in.
            bool panelHoverChanged =
                g_US.panelHoverEnabled && !g_PanelHovered;
            if (panelHoverChanged)
                g_PanelHovered = true;

            // Album-art hover (play/pause overlay)
            bool artHoverChanged = false;
            if (g_US.mediaHoverPlay && IsContainerEnabled(1)) {
                const ArtLayout& al = g_Layout.art;
                bool overArt = mx >= al.x && mx < al.x + al.width &&
                               my >= al.y && my < al.y + al.size;
                if (overArt != g_ArtHovered) {
                    g_ArtHovered = overArt;
                    artHoverChanged = true;
                }
            }

            if (ns != g_HoverBtn || newVolHover != wasVolHover ||
                panelHoverChanged || artHoverChanged) {
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
                g_PanelHovered = false;
                g_ArtHovered = false;
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
                if (g_HoverBtn == MediaCmd::PlayPause) {
                    // Optimistic flip: the media app takes 100-300ms to
                    // report its new state through SMTC. Flip the icon now;
                    // the next poll/event confirms it (or reverts it if the
                    // app rejected the toggle).
                    MediaWriter w;
                    w->playing = !w->playing;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                // Re-poll quickly so the UI reflects the action fast
                g_Timers.Set(hwnd, IDT_POLL, 150);
            } else if (g_HoverBtn == MediaCmd::None) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                EnsureLayout(rc.right, rc.bottom);
                int mx = LOWORD(lp);
                if (g_US.mediaHoverPlay && g_ArtHovered) {
                    // Click on the album art toggles playback (takes
                    // precedence over the focus-the-app click below).
                    Cmd(MediaCmd::PlayPause);
                    {
                        // Optimistic flip — see the button branch above.
                        MediaWriter w;
                        w->playing = !w->playing;
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                    g_Timers.Set(hwnd, IDT_POLL, 150);
                } else if (mx < (int)(g_Layout.ctrl.firstControlX - 12) &&
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

        case WM_RBUTTONUP: {
            bool playing;
            {
                MediaReader r;
                playing = r->playing;
            }
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, IDM_PLAYPAUSE,
                        playing ? L"Pause" : L"Play");
            AppendMenuW(hMenu, MF_STRING, IDM_PREV_TRACK, L"Previous Track");
            AppendMenuW(hMenu, MF_STRING, IDM_NEXT_TRACK, L"Next Track");
            AppendMenuW(hMenu, MF_STRING, IDM_FOCUS_APP, L"Open Media App");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu,
                        MF_STRING | (g_US.miniMode ? MF_CHECKED : 0),
                        IDM_MINI_MODE, L"Mini Mode");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu, MF_STRING, IDM_REFRESH_THEME, L"Refresh Theme");
            AppendMenuW(hMenu, MF_STRING, IDM_REFRESH_MEDIA, L"Refresh Media");
            POINT pt = {LOWORD(lp), HIWORD(lp)};
            ClientToScreen(hwnd, &pt);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_NONOTIFY,
                           pt.x, pt.y, 0, hwnd, nullptr);
            DestroyMenu(hMenu);
            return 0;
        }

        case WM_COMMAND:
            switch (LOWORD(wp)) {
                case IDM_PLAYPAUSE:
                    Cmd(MediaCmd::PlayPause);
                    {
                        // Optimistic flip — same as the button click path.
                        MediaWriter w;
                        w->playing = !w->playing;
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                    g_Timers.Set(hwnd, IDT_POLL, 150);
                    break;
                case IDM_PREV_TRACK:
                    Cmd(MediaCmd::Prev);
                    g_Timers.Set(hwnd, IDT_POLL, 150);
                    break;
                case IDM_NEXT_TRACK:
                    Cmd(MediaCmd::Next);
                    g_Timers.Set(hwnd, IDT_POLL, 150);
                    break;
                case IDM_FOCUS_APP:
                    if (!g_currentMediaAppAumid.empty()) {
                        FocusData fd{&g_currentMediaAppAumid, nullptr};
                        EnumWindows(EnumWindowsProc,
                                    reinterpret_cast<LPARAM>(&fd));
                        if (fd.found) {
                            if (IsIconic(fd.found))
                                ShowWindow(fd.found, SW_RESTORE);
                            SetForegroundWindow(fd.found);
                        }
                    }
                    break;
                case IDM_MINI_MODE:
                    // Runtime toggle. Note: reverts to the saved setting on
                    // the next settings reload — it's a quick-peek toggle,
                    // not a persisted preference (Windhawk has no API to
                    // write settings back).
                    g_US.miniMode = !g_US.miniMode;
                    RecomputeLayout();
                    UpdateBoundsAndRegion(hwnd);
                    g_LayoutDirty = true;
                    DestroyBackBuffer();
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case IDM_REFRESH_THEME:
                    ApplyAppearance(hwnd);
                    RefreshScreenBlurCache(hwnd);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case IDM_REFRESH_MEDIA:
                    FetchMedia();
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
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
                // WM_MOUSEWHEEL lParam is SCREEN coords and can be negative on
                // multi-monitor setups — LOWORD alone would wrap to ~65000 and
                // the volume zone check would never hit. Sign-extend.
                POINT pt = {(int)(short)LOWORD(lp), (int)(short)HIWORD(lp)};
                ScreenToClient(hwnd, &pt);
                // When controls container is disabled, vX is 0 -- fall back to
                // checking the full panel width so scroll still hits the zone.
                float vX2 = g_US.controlsEnabled ? g_Layout.ctrl.vX : (float)rc.right;
                float k = (float)rc.bottom / PANEL_HEIGHT_REF;
                if (pt.x >= vX2 - 15.f * k && pt.x <= vX2 + 95.f * k) {
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
            if (IsTaskbarEffectivelyVisible(tb)) {
                UpdateBoundsAndRegion(hwnd);
                g_ScreenCapCache.Invalidate();
                RefreshScreenBlurCache(hwnd);
            } else if (IsWindowVisible(hwnd)) {
                ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        }

        // Text width changed in auto-width mode — refit the panel.
        case WM_INFO_RELAYOUT:
            if (g_US.infoAutoWidth) {
                RecomputeLayout();
                UpdateBoundsAndRegion(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        default:
            if (msg == g_InlineRectMsg) {
                // Plate rect from the explorer-side Inline Mode module —
                // one signed 32-bit value per half (see g_InlineRectMsg).
                int nx = (int)(INT32)((UINT_PTR)wp >> 32);
                int ny = (int)(INT32)((UINT_PTR)wp & 0xFFFFFFFF);
                int nw = (int)(INT32)((UINT_PTR)lp >> 32);
                int nh = (int)(INT32)((UINT_PTR)lp & 0xFFFFFFFF);
                bool moved = nx != g_InlineRect.x || ny != g_InlineRect.y ||
                             nw != g_InlineRect.w || nh != g_InlineRect.h ||
                             !g_InlineRect.tick;
                g_InlineRect = {nx, ny, nw, nh, GetTickCount64()};
                if (moved)
                    UpdateBoundsAndRegion(hwnd);
                return 0;
            }
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
    // Kick off web font downloads in the background so they're ready before the
    // first repaint that actually needs them.  The MediaThread itself is also
    // started immediately so the window appears without waiting for downloads.
    // One detached thread per font entry -- all run in parallel.
    for (size_t i = 0; i < std::size(k_WebFonts); i++)
        std::thread([i]() { LoadWebFont(i); }).detach();
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
//  Taskbar-native positioning — explorer-side spacer (XAML injection)
// ============================================================================
//
// Runs ONLY inside explorer.exe (the launcher half of the tool-mod pattern;
// the panel itself lives in a separate process and can't touch the taskbar's
// XAML tree). This drives the panel's Position setting:
//
//   Side = Before/After — an invisible Border "spacer" is inserted (tray
//   anchors: a real grid column; app-side anchors: margin-push tracking) so
//   the taskbar's own layout engine reserves space and shifts the system
//   icons over. The spacer has no appearance of its own.
//
//   Side = Under — nothing is injected; the worker just tracks the anchor
//   element and the panel overlays it.
//
// A worker thread keeps the spacer width synced to the panel window and
// streams the anchored screen rect to the panel process via the registered
// g_InlineRectMsg message; the panel places itself on that rect.
//
// Everything here is fail-safe: if symbols don't resolve or the tree
// changes, nothing is injected and the panel floats at the taskbar's left
// edge. XAML-tree access technique adapted from "Taskbar Fluent Media
// Player — Fork" by Salyts (MIT).

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// taskbar.dll symbol lookups (resolved lazily; addresses only, no detours).
static void* g_TbxSiteVtable = nullptr;
using TbxGetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TbxFrameHeight_t = int(WINAPI*)(void*);
using TbxRefDecref_t = void(WINAPI*)(void*);
static TbxGetTaskbarHost_t g_TbxGetTaskbarHost = nullptr;
static TbxFrameHeight_t g_TbxFrameHeight = nullptr;
static TbxRefDecref_t g_TbxRefDecref = nullptr;
static bool g_TbxSymbolsOK = false;

// XAML state. Created/used on the taskbar's UI thread only (except the
// null-checks and Dispatcher() access, which are cross-thread safe).
static wux::FrameworkElement g_TbxRootElem{nullptr};
static wuxc::Border g_TbxPlate{nullptr};
static wuxc::Grid g_TbxTrayGrid{nullptr};
static int g_TbxPlateCol = -1;

static std::atomic<bool> g_TbxStop{false};
static std::atomic<bool> g_TbxReapply{false};
// True while the panel window's width is changing (smooth-resize animation).
// The worker syncs at 50ms instead of 250ms while set, so the plate — and
// the taskbar icons it pushes — glide with the panel instead of stepping.
static std::atomic<bool> g_TbxWidthMoving{false};
static HANDLE g_TbxThread = nullptr;
static HWND g_TbxTaskbarWnd = nullptr;

static bool TbxResolveSymbols() {
    if (g_TbxSymbolsOK)
        return true;
    HMODULE h =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h)
        return false;
    // clang-format off
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         (void**)&g_TbxSiteVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         (void**)&g_TbxGetTaskbarHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         (void**)&g_TbxFrameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         (void**)&g_TbxRefDecref},
    };
    // clang-format on
    if (!WindhawkUtils::HookSymbols(h, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"[Inline] taskbar.dll symbol resolution failed");
        return false;
    }
    g_TbxSymbolsOK = true;
    return true;
}

// Walks from the taskbar HWND to its XAML content root. Direct memory walk —
// valid only in-process (we ARE explorer here).
static wux::FrameworkElement TbxGetTaskbarRootElement(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd)
        return nullptr;
    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand)
        return nullptr;
    void* site = taskBand;
    for (int i = 0;; i++) {
        if (i == 20)
            return nullptr;
        if (*(void**)site == g_TbxSiteVtable)
            break;
        site = (void**)site + 1;
    }
    void* hostShared[2]{};
    g_TbxGetTaskbarHost(site, hostShared);
    if (!hostShared[0])
        return nullptr;

    // Find the element offset inside TaskbarHost by pattern-matching the
    // prologue of TaskbarHost::FrameHeight ("add rcx, imm8").
    size_t off = 0x10;
    const BYTE* b = (const BYTE*)g_TbxFrameHeight;
    if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F)
        off = b[7];

    IUnknown* unk = *(IUnknown**)((BYTE*)hostShared[0] + off);
    wux::FrameworkElement fe{nullptr};
    if (unk)
        unk->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                            winrt::put_abi(fe));
    if (hostShared[1] && g_TbxRefDecref)
        g_TbxRefDecref(hostShared[1]);
    return fe;
}

static wux::FrameworkElement TbxFindByName(wux::FrameworkElement const& root,
                                           std::wstring_view name,
                                           int depth = 16) {
    if (!root || depth < 0)
        return nullptr;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto c = wuxm::VisualTreeHelper::GetChild(root, i)
                     .try_as<wux::FrameworkElement>();
        if (!c)
            continue;
        if (c.Name() == name)
            return c;
        if (auto r = TbxFindByName(c, name, depth - 1))
            return r;
    }
    return nullptr;
}

// Nth direct child of the given runtime class (index 0 = first match).
static wux::FrameworkElement TbxFindNthByClass(
    wux::FrameworkElement const& parent, const wchar_t* className, int index) {
    if (!parent)
        return nullptr;
    int found = 0;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < n; i++) {
        auto c = wuxm::VisualTreeHelper::GetChild(parent, i)
                     .try_as<wux::FrameworkElement>();
        if (!c)
            continue;
        if (winrt::get_class_name(c) == className) {
            if (found == index)
                return c;
            found++;
        }
    }
    return nullptr;
}

static wux::FrameworkElement TbxFindByClassRecursive(
    wux::FrameworkElement const& parent, const wchar_t* className,
    int depth = 8) {
    if (!parent || depth < 0)
        return nullptr;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < n; i++) {
        auto c = wuxm::VisualTreeHelper::GetChild(parent, i)
                     .try_as<wux::FrameworkElement>();
        if (!c)
            continue;
        if (winrt::get_class_name(c) == className)
            return c;
        if (auto r = TbxFindByClassRecursive(c, className, depth - 1))
            return r;
    }
    return nullptr;
}

// Resolves the taskbar-side element a tracking anchor follows. Element and
// class names per the Windows 11 taskbar repeater (same set the Fluent
// reference mod uses).
static wux::FrameworkElement TbxResolveTrackedElement(
    wux::FrameworkElement const& content, std::wstring const& anchor) {
    auto repeater = TbxFindByName(content, L"TaskbarFrameRepeater", 20);
    if (anchor == L"start") {
        static const wchar_t* const kNames[] = {
            L"StartButton", L"StartMenuButton", L"StartMenuLaunchButton",
            L"LaunchListButton"};
        for (auto* nm : kNames) {
            auto e = repeater ? TbxFindByName(repeater, nm, 4) : nullptr;
            if (!e)
                e = TbxFindByName(content, nm, 20);
            if (e)
                return e;
        }
        return nullptr;
    }
    if (!repeater)
        return nullptr;
    if (anchor == L"search")
        return TbxFindNthByClass(repeater, L"Taskbar.TaskbarExtensionElement",
                                 0);
    if (anchor == L"taskview")
        return TbxFindNthByClass(repeater, L"Taskbar.ExperienceToggleButton",
                                 1);
    if (anchor == L"widgets") {
        auto e = TbxFindByName(repeater, L"AugmentedEntryPointButton", 4);
        if (!e)
            e = TbxFindByClassRecursive(repeater,
                                        L"Taskbar.AugmentedEntryPointButton",
                                        4);
        return e;
    }
    return nullptr;
}

// True for anchors that live on the app side of the bar (no tray grid
// columns there — they use the margin-push tracking technique instead).
static bool TbxIsTrackingAnchor(std::wstring const& anchor) {
    return anchor == L"start" || anchor == L"search" ||
           anchor == L"taskview" || anchor == L"widgets";
}

// Settings are read fresh on the explorer side — this process never runs
// LoadSettings (that's the panel process's loader). The spacer has no
// appearance settings of its own: it's an invisible space reservation, all
// visuals come from the panel's themes.
struct TbxSettings {
    std::wstring anchor = L"tray";
    std::wstring side = L"before";
    int marginL = 2, marginR = 2;
};

static TbxSettings TbxReadSettings() {
    TbxSettings s;
    if (PCWSTR p = Wh_GetStringSetting(L"LayoutGroup.PanelPosition")) {
        s.anchor = p;
        Wh_FreeStringSetting(p);
    }
    if (PCWSTR p = Wh_GetStringSetting(L"LayoutGroup.PanelSide")) {
        s.side = p;
        Wh_FreeStringSetting(p);
    }
    s.marginL = ClampInt(
        Wh_GetIntSetting(L"LayoutGroup.InlineMarginLeft"), 0, 100);
    s.marginR = ClampInt(
        Wh_GetIntSetting(L"LayoutGroup.InlineMarginRight"), 0, 100);
    return s;
}

// Maps anchor+side to a tray-grid column. Anchor-element names per the
// Windows 11 tray grid layout. Returns -1 when the anchor isn't a tray
// element (the Start anchor uses the margin-push path instead).
static int TbxResolveColumn(wuxc::Grid const& tray,
                            wux::FrameworkElement const& root,
                            TbxSettings const& s) {
    auto byName = [&](const wchar_t* nm) -> wux::FrameworkElement {
        auto e = TbxFindByName(tray, nm);
        if (!e)
            e = TbxFindByName(root, nm);
        return e;
    };
    bool after = (s.side == L"after");
    if (s.anchor == L"tray")
        return after ? (int)tray.ColumnDefinitions().Size() : 0;
    if (s.anchor == L"icons") {
        auto e = byName(L"NotificationAreaIcons");
        return e ? wuxc::Grid::GetColumn(e) + (after ? 1 : 0) : -1;
    }
    if (s.anchor == L"chevron") {
        auto e = byName(L"NotifyIconStack");
        return e ? wuxc::Grid::GetColumn(e) + (after ? 1 : 0) : -1;
    }
    if (s.anchor == L"clock") {
        if (!after) {
            auto e = byName(L"NotificationCenterButton");
            return e ? wuxc::Grid::GetColumn(e) : -1;
        }
        auto e = byName(L"ShowDesktopStack");
        return e ? wuxc::Grid::GetColumn(e) : -1;
    }
    if (s.anchor == L"volume") {
        auto e = byName(L"ControlCenterButton");
        return e ? wuxc::Grid::GetColumn(e) + (after ? 1 : 0) : -1;
    }
    if (s.anchor == L"language") {
        auto e = byName(L"NonActivatableStack");
        return e ? wuxc::Grid::GetColumn(e) + (after ? 1 : 0) : -1;
    }
    return -1;
}

// Resolves the element the panel overlays in Side = Under mode (no plate,
// no reserved space — the panel just sits on top of this element). App-side
// anchors reuse the tracked-element resolver; tray anchors map to the named
// tray-grid children, with "tray" meaning the whole tray grid.
static wux::FrameworkElement TbxResolveUnderTarget(
    wux::FrameworkElement const& content, std::wstring const& anchor) {
    if (TbxIsTrackingAnchor(anchor))
        return TbxResolveTrackedElement(content, anchor);
    auto tray = TbxFindByName(content, L"SystemTrayFrameGrid");
    if (!tray)
        return nullptr;
    if (anchor == L"tray")
        return tray;
    const wchar_t* nm = anchor == L"icons"      ? L"NotificationAreaIcons"
                        : anchor == L"chevron"  ? L"NotifyIconStack"
                        : anchor == L"clock"    ? L"NotificationCenterButton"
                        : anchor == L"volume"   ? L"ControlCenterButton"
                        : anchor == L"language" ? L"NonActivatableStack"
                                                : nullptr;
    if (!nm)
        return nullptr;
    auto e = TbxFindByName(tray, nm);
    if (!e)
        e = TbxFindByName(content, nm);
    return e;
}

// ── UI-thread operations ─────────────────────────────────────────────────────

// Start-anchor ("Windows logo") placement state: the taskbar frame has no
// grid columns to insert into, so space is opened by pushing the Start
// button's margin and the plate floats in the gap (Salyts' tracking
// technique, simplified to our sync cadence).
static bool g_TbxStartMode = false;
static bool g_TbxStartAfter = false;
static wux::FrameworkElement g_TbxStartBtn{nullptr};
// Side = Under: overlay tracking — no plate in the tree, the worker just
// streams the tracked element's rect and the panel covers it.
static bool g_TbxUnderMode = false;
static wux::FrameworkElement g_TbxUnderElem{nullptr};
// Widgets anchor: pin to the taskbar's far-left corner directly — no XAML
// element involved (the Widgets button is hidden on most setups, and
// margin-pushing it when present was glitchy). Side has no effect.
static bool g_TbxLeftMode = false;
static wux::Thickness g_TbxStartOrigMargin{};
static bool g_TbxHaveOrigMargin = false;
static wuxc::Grid g_TbxRootGrid{nullptr};
static TbxSettings g_TbxCfg;  // settings snapshot from the last inject

// Shallow search for the first Grid to host the floating plate (the
// taskbar's root grid stretches across the whole bar).
static wuxc::Grid TbxFindHostGrid(wux::FrameworkElement const& content,
                                  int depth = 3) {
    if (!content || depth < 0)
        return nullptr;
    if (auto g = content.try_as<wuxc::Grid>())
        return g;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(content);
    for (int i = 0; i < n; i++) {
        auto c = wuxm::VisualTreeHelper::GetChild(content, i)
                     .try_as<wux::FrameworkElement>();
        if (!c)
            continue;
        if (auto g = TbxFindHostGrid(c, depth - 1))
            return g;
    }
    return nullptr;
}

static void TbxRemoveOnUiThread() {
    // Restore the Start button's original margin (start-anchor mode).
    if (g_TbxStartBtn && g_TbxHaveOrigMargin) {
        try {
            g_TbxStartBtn.Margin(g_TbxStartOrigMargin);
        } catch (...) {
        }
    }
    if (g_TbxRootGrid && g_TbxPlate) {
        try {
            uint32_t idx;
            if (g_TbxRootGrid.Children().IndexOf(g_TbxPlate, idx))
                g_TbxRootGrid.Children().RemoveAt(idx);
        } catch (...) {
        }
    }
    if (g_TbxTrayGrid && g_TbxPlate) {
        try {
            uint32_t idx;
            if (g_TbxTrayGrid.Children().IndexOf(g_TbxPlate, idx))
                g_TbxTrayGrid.Children().RemoveAt(idx);
            auto cols = g_TbxTrayGrid.ColumnDefinitions();
            if (g_TbxPlateCol >= 0 && g_TbxPlateCol < (int)cols.Size()) {
                cols.RemoveAt(g_TbxPlateCol);
                for (uint32_t i = 0; i < g_TbxTrayGrid.Children().Size(); i++) {
                    auto ch = g_TbxTrayGrid.Children()
                                  .GetAt(i)
                                  .try_as<wux::FrameworkElement>();
                    if (!ch)
                        continue;
                    int cc = wuxc::Grid::GetColumn(ch);
                    if (cc > g_TbxPlateCol)
                        wuxc::Grid::SetColumn(ch, cc - 1);
                }
            }
        } catch (...) {
        }
    }
    g_TbxPlate = nullptr;
    g_TbxTrayGrid = nullptr;
    g_TbxRootGrid = nullptr;
    g_TbxStartBtn = nullptr;
    g_TbxHaveOrigMargin = false;
    g_TbxStartMode = false;
    g_TbxUnderElem = nullptr;
    g_TbxUnderMode = false;
    g_TbxLeftMode = false;
    g_TbxPlateCol = -1;
}

static void TbxSyncOnUiThread();  // defined below; used for instant placement

static bool TbxInjectOnUiThread() {
    auto root = g_TbxRootElem;
    if (!root)
        return false;
    auto xr = root.XamlRoot();
    auto content = xr ? xr.Content().try_as<wux::FrameworkElement>() : nullptr;
    if (!content)
        return false;
    TbxSettings s = TbxReadSettings();

    // ── Widgets anchor: pin to the bar's far-left corner, no XAML element
    // involved. Checked before everything else so Side (before/after/under)
    // makes no difference — the left corner is the left corner.
    if (s.anchor == L"widgets") {
        g_TbxLeftMode = true;
        g_TbxCfg = s;
        TbxSyncOnUiThread();
        Wh_Log(L"[Inline] left-corner mode (widgets anchor)");
        return true;
    }

    // ── Side = Under: overlay mode — nothing is injected into the tree.
    // The worker streams the tracked element's rect and the panel covers it.
    if (s.side == L"under") {
        auto target = TbxResolveUnderTarget(content, s.anchor);
        if (!target) {
            Wh_Log(L"[Inline] under target (%s) not found", s.anchor.c_str());
            return false;
        }
        g_TbxUnderElem = target;
        g_TbxUnderMode = true;
        g_TbxCfg = s;
        TbxSyncOnUiThread();
        Wh_Log(L"[Inline] under-overlay tracking (anchor=%s)",
               s.anchor.c_str());
        return true;
    }

    // The plate is a pure invisible spacer — it reserves real space so the
    // taskbar's layout engine shifts the system icons over. All visuals
    // come from the panel's own themes; the plate has no appearance.
    wuxc::Border plate;
    plate.Name(L"TMPInlinePlate");
    plate.VerticalAlignment(wux::VerticalAlignment::Stretch);
    plate.Margin(wux::Thickness{(double)s.marginL, 4.0, (double)s.marginR, 4.0});
    plate.Width(120.0);  // placeholder; synced to the panel width below
    plate.IsHitTestVisible(false);

    if (TbxIsTrackingAnchor(s.anchor)) {
        // ── App-side anchors (Windows logo / Search / Task View / Widgets):
        // float the plate + push the tracked element's margin to open the
        // gap. g_TbxStartBtn holds whichever element the anchor tracks.
        auto tracked = TbxResolveTrackedElement(content, s.anchor);
        auto host = TbxFindHostGrid(content);
        if (!tracked || !host) {
            Wh_Log(L"[Inline] tracked element (%s) or host grid not found",
                   s.anchor.c_str());
            return false;
        }
        plate.HorizontalAlignment(wux::HorizontalAlignment::Left);
        plate.Margin(wux::Thickness{0, 4.0, 0, 4.0});
        host.Children().Append(plate);

        g_TbxPlate = plate;
        g_TbxRootGrid = host;
        g_TbxStartMode = true;
        g_TbxStartAfter = (s.side == L"after");
        g_TbxStartBtn = tracked;
        g_TbxStartOrigMargin = tracked.Margin();
        g_TbxHaveOrigMargin = true;
        g_TbxCfg = s;
        // Position immediately instead of waiting for the first worker
        // sync tick — otherwise the plate flashes at margin 0 (the bar's
        // left edge) for up to a quarter second.
        TbxSyncOnUiThread();
        Wh_Log(L"[Inline] plate injected (anchor=%s side=%s, tracking)",
               s.anchor.c_str(), s.side.c_str());
        return true;
    }

    // ── Tray anchors: real grid column, the layout engine reserves space ──
    auto trayFE = TbxFindByName(content, L"SystemTrayFrameGrid");
    auto tray = trayFE ? trayFE.try_as<wuxc::Grid>() : nullptr;
    if (!tray) {
        Wh_Log(L"[Inline] SystemTrayFrameGrid not found");
        return false;
    }
    int col = TbxResolveColumn(tray, content, s);
    if (col < 0)
        col = 0;

    auto cols = tray.ColumnDefinitions();
    wuxc::ColumnDefinition cd;
    cd.Width(wux::GridLength{1.0, wux::GridUnitType::Auto});
    if (col >= (int)cols.Size()) {
        col = (int)cols.Size();
        cols.Append(cd);
    } else {
        cols.InsertAt(col, cd);
        for (uint32_t i = 0; i < tray.Children().Size(); i++) {
            auto ch =
                tray.Children().GetAt(i).try_as<wux::FrameworkElement>();
            if (!ch)
                continue;
            int cc = wuxc::Grid::GetColumn(ch);
            if (cc >= col)
                wuxc::Grid::SetColumn(ch, cc + 1);
        }
    }
    wuxc::Grid::SetColumn(plate, col);
    tray.Children().Append(plate);

    g_TbxPlate = plate;
    g_TbxTrayGrid = tray;
    g_TbxStartMode = false;
    g_TbxPlateCol = col;
    g_TbxCfg = s;
    Wh_Log(L"[Inline] plate injected (anchor=%s side=%s col=%d)",
           s.anchor.c_str(), s.side.c_str(), col);
    return true;
}

// Syncs plate width to the panel window and streams the plate's screen rect
// to the panel process.
static void TbxSyncOnUiThread() {
    HWND panel = FindWindowW(L"TaskbarMediaPlayer_v6", nullptr);
    if (!panel || !g_TbxTaskbarWnd)
        return;

    // One signed 32-bit value per half — see g_InlineRectMsg in the
    // panel-side code for the format contract.
    auto pack = [](int hi, int lo) -> UINT_PTR {
        return ((UINT_PTR)(UINT32)hi << 32) | (UINT32)lo;
    };

    // ── Widgets anchor: the taskbar's far-left corner, computed from plain
    // window rects — no XAML element, nothing that can disappear on us.
    if (g_TbxLeftMode) {
        RECT tb, pr;
        if (!GetWindowRect(g_TbxTaskbarWnd, &tb) || !GetWindowRect(panel, &pr))
            return;
        UINT dpi = GetDpiForWindow(g_TbxTaskbarWnd);
        float k = (dpi ? dpi : 96) / 96.f;
        int pw = pr.right - pr.left;
        if (pw > 0) {
            int x = tb.left + (int)roundf(g_TbxCfg.marginL * k);
            PostMessageW(panel, g_InlineRectMsg, (WPARAM)pack(x, tb.top),
                         (LPARAM)pack(pw, tb.bottom - tb.top));
        }
        return;
    }

    // ── Side = Under: stream the tracked element's rect, with the panel's
    // own width centered on it. Nothing reserved, nothing in the tree.
    if (g_TbxUnderMode && g_TbxUnderElem) {
        try {
            double scale = 1.0;
            if (auto xr = g_TbxUnderElem.XamlRoot())
                scale = xr.RasterizationScale();
            if (scale <= 0)
                scale = 1.0;
            auto pt = g_TbxUnderElem.TransformToVisual(nullptr)
                          .TransformPoint({0, 0});
            RECT tb, pr;
            if (!GetWindowRect(g_TbxTaskbarWnd, &tb) ||
                !GetWindowRect(panel, &pr))
                return;
            int ex = tb.left + (int)llround(pt.X * scale);
            int ey = tb.top + (int)llround(pt.Y * scale);
            int ew = (int)llround(g_TbxUnderElem.ActualWidth() * scale);
            int eh = (int)llround(g_TbxUnderElem.ActualHeight() * scale);
            int pw = pr.right - pr.left;
            if (ew > 0 && eh > 0 && pw > 0) {
                int x = ex + (ew - pw) / 2;  // center the panel on the anchor
                PostMessageW(panel, g_InlineRectMsg, (WPARAM)pack(x, ey),
                             (LPARAM)pack(pw, eh));
            }
        } catch (...) {
        }
        return;
    }

    if (!g_TbxPlate)
        return;
    try {
        double scale = 1.0;
        if (auto xr = g_TbxPlate.XamlRoot())
            scale = xr.RasterizationScale();
        if (scale <= 0)
            scale = 1.0;

        double wantW = g_TbxPlate.Width();
        RECT pr;
        if (GetWindowRect(panel, &pr)) {
            double w = (pr.right - pr.left) / scale;
            if (w >= 8) {
                wantW = w;
                if (fabs(g_TbxPlate.Width() - w) > 0.5) {
                    g_TbxPlate.Width(w);
                    g_TbxWidthMoving = true;  // panel is animating; sync fast
                } else {
                    g_TbxWidthMoving = false;
                }
            }
        }

        if (g_TbxStartMode && g_TbxStartBtn) {
            // Maintain the gap: push the Start button's margin and place the
            // plate in the opened space.
            double gap = wantW + g_TbxCfg.marginL + g_TbxCfg.marginR;
            wux::Thickness nm = g_TbxHaveOrigMargin ? g_TbxStartOrigMargin
                                                    : g_TbxStartBtn.Margin();
            if (!g_TbxStartAfter)
                nm.Left += gap;
            else
                nm.Right += gap;
            auto cur = g_TbxStartBtn.Margin();
            if (fabs(cur.Left - nm.Left) > 0.5 ||
                fabs(cur.Right - nm.Right) > 0.5)
                g_TbxStartBtn.Margin(nm);

            // Start's position in the HOST GRID's coordinate space — the
            // plate's margin is relative to that grid, not the XAML root.
            // (Root-space coords put the plate at the bar's edge instead of
            // hugging the logo when the grids don't share an origin.)
            auto st = g_TbxStartBtn.TransformToVisual(g_TbxRootGrid)
                          .TransformPoint({0, 0});
            double px = !g_TbxStartAfter
                            ? st.X - gap + g_TbxCfg.marginL
                            : st.X + g_TbxStartBtn.ActualWidth() +
                                  g_TbxCfg.marginL;
            if (px < 0)
                px = 0;
            auto pm = g_TbxPlate.Margin();
            if (fabs(pm.Left - px) > 0.5)
                g_TbxPlate.Margin(wux::Thickness{px, 4.0, 0, 4.0});
        }

        auto t = g_TbxPlate.TransformToVisual(nullptr);
        auto pt = t.TransformPoint({0, 0});
        RECT tb;
        if (!GetWindowRect(g_TbxTaskbarWnd, &tb))
            return;
        int x = tb.left + (int)llround(pt.X * scale);
        int y = tb.top + (int)llround(pt.Y * scale);
        int w = (int)llround(g_TbxPlate.ActualWidth() * scale);
        int h = (int)llround(g_TbxPlate.ActualHeight() * scale);
        if (w > 0 && h > 0)
            PostMessageW(panel, g_InlineRectMsg, (WPARAM)pack(x, y),
                         (LPARAM)pack(w, h));
    } catch (...) {
    }
}

// Schedules work onto the taskbar's XAML UI thread. Returns false when the
// dispatcher is unreachable (taskbar XAML torn down) so the caller can reset.
template <typename F>
static bool TbxRunOnUi(F&& fn) {
    try {
        auto disp = g_TbxRootElem.Dispatcher();
        if (!disp)
            return false;
        disp.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                      std::forward<F>(fn));
        return true;
    } catch (...) {
        return false;
    }
}

static DWORD WINAPI TbxWorkerProc(LPVOID) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    while (!g_TbxStop.load()) {
        // Settings changed — tear down and re-anchor with the new values.
        if (g_TbxReapply.exchange(false) &&
            (g_TbxPlate || g_TbxUnderElem || g_TbxLeftMode)) {
            TbxRunOnUi([] { TbxRemoveOnUiThread(); });
            Sleep(100);
        }

        if (!TbxResolveSymbols()) {
            Sleep(10000);
            continue;
        }

        if (!g_TbxTaskbarWnd || !IsWindow(g_TbxTaskbarWnd)) {
            g_TbxTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
            g_TbxRootElem = nullptr;  // taskbar (re)created — re-resolve
        }
        if (!g_TbxTaskbarWnd) {
            Sleep(2000);
            continue;
        }

        if (!g_TbxRootElem) {
            try {
                g_TbxRootElem = TbxGetTaskbarRootElement(g_TbxTaskbarWnd);
            } catch (...) {
            }
            if (!g_TbxRootElem) {
                Sleep(3000);
                continue;
            }
            g_TbxPlate = nullptr;  // tree changed; any old plate is gone
            g_TbxTrayGrid = nullptr;
            g_TbxRootGrid = nullptr;
            g_TbxStartBtn = nullptr;
            g_TbxHaveOrigMargin = false;
            g_TbxStartMode = false;
            g_TbxUnderElem = nullptr;
            g_TbxUnderMode = false;
            g_TbxLeftMode = false;
            g_TbxPlateCol = -1;
        }

        bool ok;
        if (!g_TbxPlate && !g_TbxUnderElem && !g_TbxLeftMode)
            ok = TbxRunOnUi([] {
                try {
                    TbxInjectOnUiThread();
                } catch (...) {
                }
            });
        else
            ok = TbxRunOnUi([] { TbxSyncOnUiThread(); });

        if (!ok) {
            // Dispatcher dead — taskbar XAML went away. Reset and retry.
            g_TbxRootElem = nullptr;
            g_TbxPlate = nullptr;
            g_TbxTrayGrid = nullptr;
            g_TbxRootGrid = nullptr;
            g_TbxStartBtn = nullptr;
            g_TbxHaveOrigMargin = false;
            g_TbxStartMode = false;
            g_TbxUnderElem = nullptr;
            g_TbxUnderMode = false;
            g_TbxLeftMode = false;
            g_TbxPlateCol = -1;
            Sleep(2000);
            continue;
        }

        // Fast cadence while the panel width is animating so the spacer
        // (and the icons it pushes) glide with it; relaxed otherwise.
        Sleep((g_TbxPlate || g_TbxUnderElem || g_TbxLeftMode)
                  ? (g_TbxWidthMoving.load() ? 50 : 250)
                  : 1500);
    }

    // Shutdown: best-effort spacer removal, then drop the XAML refs.
    TbxRunOnUi([] { TbxRemoveOnUiThread(); });
    Sleep(300);
    g_TbxPlate = nullptr;
    g_TbxTrayGrid = nullptr;
    g_TbxRootGrid = nullptr;
    g_TbxStartBtn = nullptr;
    g_TbxUnderElem = nullptr;
    g_TbxUnderMode = false;
    g_TbxLeftMode = false;
    g_TbxRootElem = nullptr;
    winrt::uninit_apartment();
    return 0;
}

static void TbxStartWorker() {
    if (g_TbxThread)
        return;
    g_TbxStop = false;
    g_TbxThread = CreateThread(nullptr, 0, TbxWorkerProc, nullptr, 0, nullptr);
}

static void TbxStopWorker() {
    if (!g_TbxThread)
        return;
    g_TbxStop = true;
    WaitForSingleObject(g_TbxThread, 3000);
    CloseHandle(g_TbxThread);
    g_TbxThread = nullptr;
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

    STARTUPINFO si = {};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (!CPI(nullptr, path, cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
             nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Inline Mode plate management runs here in explorer — the only process
    // with in-tree access to the taskbar's XAML. The worker self-disables
    // when the setting is off, so starting it unconditionally is cheap.
    TbxStartWorker();
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModSettingsChanged();
    } else {
        // Rebuild the plate with the new placement/material on next tick.
        g_TbxReapply = true;
    }
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
    TbxStopWorker();
}
