// ==WindhawkMod==
// @id              taskbar-audio-media-suite
// @name            Taskbar Audio & Media Suite
// @description     Integrated taskbar audio spectrum visualizer, media info/controls, and media/volume flyouts with 8 styles and 6 color modes.
// @version         0.1.0
// @author          MrSpy00
// @github          https://github.com/MrSpy00
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lole32 -loleaut32 -ldwmapi -lgdi32 -luser32 -lwtsapi32 -lwindowsapp -lruntimeobject -lshcore -lgdiplus -lshell32 -luuid -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Audio & Media Suite v0.1.0

A real-time audio spectrum visualizer, media player control bar, and custom media/volume flyouts for the Windows taskbar.
Captures system audio (WASAPI Loopback or microphone input) and integrates with Windows media players (Spotify, browsers, etc.) via GSMTC.

## Visualizer Styles
| ID | Style        | Description                                |
|----|--------------|--------------------------------------------|
| 0  | Bars         | Classic rounded spectrum bars              |
| 1  | Mirror       | Symmetrical top/bottom center reflection   |
| 2  | Blocks       | Segmented LED-style block meter            |
| 3  | Wave         | Smooth waveform curve                      |
| 4  | Oscilloscope | Triggered raw audio waveform               |
| 5  | Curve        | Smooth spline curve with floating peaks    |
| 6  | Symmetrical  | Centered mirrored waveform trail           |
| 7  | Particles    | Spectrum with reactive rising particles    |

## Color Modes
| ID | Mode           | Description                                |
|----|----------------|--------------------------------------------|
| 0  | Gradient       | Vertical transition between Color1 & Color2|
| 1  | Rainbow        | Animated horizontal spectrum hue sweep     |
| 2  | Solid          | Single static color (Color1)               |
| 3  | Beat-Reactive  | Color shift driven by low-frequency bass   |
| 4  | Adaptive       | Matches album art dominant color           |
| 5  | Pitch-Reactive | Hue driven by dominant audio frequency     |

## Key Features
- **DWM Acrylic Blur**: Uses native DWM acrylic composition without `WS_EX_LAYERED` on the main panel
- **Taskbar Z-Band Placement**: Uses `CreateWindowInBand` (`ZBID_IMMERSIVE_NOTIFICATION`) inside `explorer.exe` so the panel stays above the taskbar
- **Media Info & Controls**: Scrolling track title, artist, album art, source badge, progress bar, and playback/volume buttons
- **Custom Flyouts**: Acrylic media-change flyout with seekable progress bar and volume / Lock-key OSD flyout
- **Smart Click-Through**: Playback buttons and trackbar are interactive while the visualizer region stays click-through
- **Auto-Hide**: Automatically hides on fullscreen apps, audio silence, or idle media timeout
- **Presets**: Built-in layout presets plus 3 customizable user preset slots stored in Windhawk mod storage

## Notes
- **All-in-one design**: Combines a 8-style spectrum visualizer, GSMTC media controls, acrylic track-change flyout, volume/lock-key OSD, and layout presets into one cohesive taskbar-band capsule.
- **Native Windows OSD**: Showing the custom volume flyout does not suppress the built-in Windows volume OSD; both may appear together.
- **Microphone mode**: Setting Audio Capture Source to Microphone causes Windows to show the microphone-in-use indicator on Explorer.
- **Presets**: Selecting Preset 1–6 overrides individual layout size settings; keeping `Save Current Settings` active overwrites that slot on settings changes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ShowVisualizer: true
  $name: "[1. Visualizer] Show Spectrum Visualizer"
- BarCount: 32
  $name: "[1. Visualizer] Bar Count (4-128)"
- Style: "0"
  $name: "[1. Visualizer] Style"
  $options:
    - "0": "Bars"
    - "1": "Mirror"
    - "2": "Blocks"
    - "3": "Wave"
    - "4": "Oscilloscope"
    - "5": "Curve"
    - "6": "Symmetrical"
    - "7": "Particles"
- ColorMode: "0"
  $name: "[1. Visualizer] Color Mode"
  $options:
    - "0": "Gradient"
    - "1": "Rainbow"
    - "2": "Solid"
    - "3": "Beat-Reactive"
    - "4": "Adaptive"
    - "5": "Pitch-Reactive"
- Color1: 00CCFF
  $name: "[1. Visualizer] Color1 Primary/Top (Hex RRGGBB)"
- Color2: FF3300
  $name: "[1. Visualizer] Color2 Secondary/Bottom (Hex RRGGBB)"
- Smoothing: 70
  $name: "[1. Visualizer] Smoothing (0-95)"
- Sensitivity: 200
  $name: "[1. Visualizer] Sensitivity % (10-1000)"
- ShowPeaks: true
  $name: "[1. Visualizer] Show Peak Hold Indicators"
- PeakFallSpeed: 2
  $name: "[1. Visualizer] Peak Fall Speed (1-10)"
- GlowEffect: false
  $name: "[1. Visualizer] Glow Effect"
- GlowIntensity: 40
  $name: "[1. Visualizer] Glow Intensity (5-150)"
- BarSpacing: 1
  $name: "[1. Visualizer] Bar Spacing (px)"
- BlockSize: 4
  $name: "[1. Visualizer] Block Height (px)"
- BlockGap: 1
  $name: "[1. Visualizer] Block Gap (px)"
- FreqMin: 20
  $name: "[1. Visualizer] Min Frequency (Hz)"
- FreqMax: 18000
  $name: "[1. Visualizer] Max Frequency (Hz)"
- WaveLineWidth: 2
  $name: "[1. Visualizer] Line Width px (Wave/Oscilloscope)"
- WaveFillAlpha: 50
  $name: "[1. Visualizer] Wave Fill Opacity (0-200)"
- MirrorGap: 2
  $name: "[1. Visualizer] Mirror Center Gap (px)"
- VisWidthPercent: 50
  $name: "[1. Visualizer] Visualizer Width % (15-85)"
- ShowMediaInfo: true
  $name: "[2. Media Info] Show Media Info"
- ShowMediaControls: true
  $name: "[3. Media Controls] Show Media Controls"
- ShowMediaFlyout: true
  $name: "[4. Media Flyout] Show Track Change Flyout"
- FlyoutOnFullscreen: true
  $name: "[4. Media Flyout] Show Flyout Even in Fullscreen"
- MediaFlyoutPosition: "0"
  $name: "[4. Media Flyout] Screen Position"
  $options:
    - "0": "Relative to Taskbar (Default)"
    - "1": "Top Left"
    - "2": "Top Center"
    - "3": "Top Right"
    - "4": "Bottom Left"
    - "5": "Bottom Center"
    - "6": "Bottom Right"
- MediaFlyoutDuration: 4
  $name: "[4. Media Flyout] Display Duration (sec)"
- MediaFlyoutWidth: 520
  $name: "[4. Media Flyout] Width (px)"
- MediaFlyoutHeight: 180
  $name: "[4. Media Flyout] Height (px)"
- MediaFlyoutOffsetY: 18
  $name: "[4. Media Flyout] Vertical Offset (px)"
- MediaFlyoutArtSize: 100
  $name: "[4. Media Flyout] Album Art Size (px)"
- MediaFlyoutBtnSize: 36
  $name: "[4. Media Flyout] Button Size (px)"
- MediaFlyoutTitleSize: 14
  $name: "[4. Media Flyout] Title Font Size (pt)"
- MediaFlyoutArtistSize: 11
  $name: "[4. Media Flyout] Artist Font Size (pt)"
- MediaFlyoutMarginX: 24
  $name: "[4. Media Flyout] Horizontal Margin (px)"
- MediaFlyoutMarginY: 24
  $name: "[4. Media Flyout] Vertical Margin (px)"
- MediaFlyoutLabelY: 16
  $name: "[4. Media Flyout] Header Label Y Position (px)"
- MediaFlyoutLabelFontSize: 9
  $name: "[4. Media Flyout] Header Label Font Size (pt)"
- MediaFlyoutArtCornerRadius: 12
  $name: "[4. Media Flyout] Album Art Corner Radius (px)"
- MediaFlyoutBadgeSize: 22
  $name: "[4. Media Flyout] Source App Badge Size (px)"
- MediaFlyoutPbGap: 14
  $name: "[4. Media Flyout] Button-to-Progress Bar Gap (px)"
- MediaFlyoutPbHeight: 6
  $name: "[4. Media Flyout] Progress Bar Height (px)"
- MediaFlyoutTextGap: 24
  $name: "[4. Media Flyout] Text-to-Art Gap (px)"
- MediaFlyoutLineGap: 10
  $name: "[4. Media Flyout] Title-to-Artist Gap (px)"
- MediaFlyoutBtnSpacing: 14
  $name: "[4. Media Flyout] Button Spacing (px)"
- MediaFlyoutKnobSize: 14
  $name: "[4. Media Flyout] Progress Knob Size (px)"
- MediaFlyoutTimeTextSize: 10
  $name: "[4. Media Flyout] Timestamp Font Size (pt)"
- MediaFlyoutCornerRadius: 18
  $name: "[4. Media Flyout] Corner Radius (px)"
- MediaFlyoutBorderThickness: 1
  $name: "[4. Media Flyout] Border Thickness (px)"
- ShowVolumeFlyout: true
  $name: "[5. Volume Flyout] Show Volume & Lock Key Flyout"
- VolumeFlyoutDuration: 3
  $name: "[5. Volume Flyout] Display Duration (sec)"
- VolumeFlyoutWidth: 360
  $name: "[5. Volume Flyout] Width (px)"
- VolumeFlyoutHeight: 80
  $name: "[5. Volume Flyout] Height (px)"
- VolumeFlyoutOffsetY: 18
  $name: "[5. Volume Flyout] Vertical Offset (px)"
- VolumeFlyoutIconSize: 36
  $name: "[5. Volume Flyout] Speaker Icon Size (px)"
- VolumeFlyoutTextSize: 13
  $name: "[5. Volume Flyout] Text Font Size (pt)"
- VolumeFlyoutMarginX: 24
  $name: "[5. Volume Flyout] Horizontal Margin (px)"
- VolumeFlyoutTextGap: 18
  $name: "[5. Volume Flyout] Icon-to-Text Gap (px)"
- VolumeFlyoutSliderGap: 14
  $name: "[5. Volume Flyout] Text-to-Slider Gap (px)"
- VolumeFlyoutSliderHeight: 8
  $name: "[5. Volume Flyout] Slider Height (px)"
- VolumeFlyoutPaddingRight: 24
  $name: "[5. Volume Flyout] Right Padding (px)"
- VolumeFlyoutCornerRadius: 14
  $name: "[5. Volume Flyout] Corner Radius (px)"
- VolumeFlyoutBorderThickness: 1
  $name: "[5. Volume Flyout] Border Thickness (px)"
- PanelWidth: 360
  $name: "[6. Panel Layout & Size] Panel Width (px)"
- PanelHeight: 48
  $name: "[6. Panel Layout & Size] Panel Height (px)"
- Position: "1"
  $name: "[6. Panel Layout & Size] Position"
  $options:
    - "0": "Left"
    - "1": "Center"
    - "2": "Right"
- OffsetX: 0
  $name: "[6. Panel Layout & Size] X Offset (px)"
- OffsetY: 0
  $name: "[6. Panel Layout & Size] Y Offset (px)"
- PanelMargin: 2
  $name: "[6. Panel Layout & Size] Outer Margin (px)"
- PanelPadding: 4
  $name: "[6. Panel Layout & Size] Inner Padding (px)"
- PanelGap: 8
  $name: "[6. Panel Layout & Size] Module Gap (px)"
- PanelCornerRadius: 7
  $name: "[6. Panel Layout & Size] Corner Radius (px)"
- PanelBorderThickness: 1
  $name: "[6. Panel Layout & Size] Border Thickness (px)"
- PanelBtnSize: 22
  $name: "[6. Panel Layout & Size] Media Button Size (px)"
- MediaArtCornerRadius: 5
  $name: "[6. Panel Layout & Size] Album Art Corner Radius (px)"
- AppIconSize: 12
  $name: "[6. Panel Layout & Size] Source Badge Size (px)"
- DividerPadding: 4
  $name: "[6. Panel Layout & Size] Divider Padding (px)"
- ProgressBarHeight: 2
  $name: "[6. Panel Layout & Size] Progress Bar Height (px)"
- PanelTextGap: 8
  $name: "[6. Panel Layout & Size] Text-to-Art Gap (px)"
- PanelPbGap: 4
  $name: "[6. Panel Layout & Size] Text-to-Progress Bar Gap (px)"
- MediaArtSize: 0
  $name: "[6. Panel Layout & Size] Custom Album Art Size px (0 = Auto)"
- TargetFPS: 30
  $name: "[7. Power & Performance] Target FPS (10-60)"
- HideOnFullscreen: true
  $name: "[7. Power & Performance] Auto-Hide in Fullscreen"
- SilenceTimeout: 0
  $name: "[7. Power & Performance] Silence Auto-Hide Timeout sec (0=Off)"
- IdleTimeout: 0
  $name: "[7. Power & Performance] Media Pause Auto-Hide Timeout sec (0=Off)"
- BgOpacity: 0
  $name: "[8. Theme & Typography] Acrylic Tint Opacity 0-255 (0=Clear Glass)"
- FlyoutBgOpacity: 185
  $name: "[8. Theme & Typography] Flyout Acrylic Opacity (0-255)"
- AutoTheme: true
  $name: "[8. Theme & Typography] Auto Light/Dark Theme"
- ScrollSpeed: 2
  $name: "[8. Theme & Typography] Text Scroll Speed (1-5)"
- TitleFontSize: 10
  $name: "[8. Theme & Typography] Track Title Font Size (pt)"
- ArtistFontSize: 8
  $name: "[8. Theme & Typography] Artist Font Size (pt)"
- TitleFontBold: true
  $name: "[8. Theme & Typography] Bold Track Title"
- ArtistFontItalic: false
  $name: "[8. Theme & Typography] Italic Artist Name"
- TextOffsetX: 0
  $name: "[8. Theme & Typography] Text X Offset (px)"
- TextOffsetY: 0
  $name: "[8. Theme & Typography] Text Y Offset (px)"
- LineGap: 1
  $name: "[8. Theme & Typography] Line Gap (px)"
- ScrollPadding: 30
  $name: "[8. Theme & Typography] Scroll Loop Gap (px)"
- UseCustomTextColors: false
  $name: "[8. Theme & Typography] Use Custom Text Colors"
- TitleColor: FFFFFF
  $name: "[8. Theme & Typography] Custom Title Color (Hex)"
- ArtistColor: AAAAAA
  $name: "[8. Theme & Typography] Custom Artist Color (Hex)"
- ShowTextShadow: true
  $name: "[8. Theme & Typography] Show Text Shadow"
- CaptureMode: "0"
  $name: "[9. Audio Input] Audio Capture Source"
  $options:
    - "0": "System Audio (Loopback)"
    - "1": "Microphone"
- PresetMode: "0"
  $name: "[10. Presets] Active Layout Preset"
  $options:
    - "0": "Default"
    - "1": "Wide & Spacious"
    - "2": "Mini & Compact"
    - "3": "Large Screen"
    - "4": "Custom Preset 1"
    - "5": "Custom Preset 2"
    - "6": "Custom Preset 3"
- PresetAction: "0"
  $name: "[10. Presets] Save Current Settings to Preset Slot"
  $options:
    - "0": "No Action"
    - "1": "Save Current Settings as Custom Preset 1"
    - "2": "Save Current Settings as Custom Preset 2"
    - "3": "Save Current Settings as Custom Preset 3"
*/
// ==/WindhawkModSettings==

#define INITGUID
#define _USE_MATH_DEFINES

#include <cmath>
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include <windows.h>
#include <windowsx.h>
#include <wtsapi32.h>
#include <tlhelp32.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <shellapi.h>
#include <shobjidl.h>

#include <string>
#include <vector>
#include <complex>
#include <mutex>
#include <thread>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <optional>
#include <cstring>
#include <cstdio>

// WinRT GSMTC
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

// =========================================================================
// CONSTANTS
// =========================================================================
static const int FFT_SIZE   = 2048;
static const int MAX_BARS   = 128;
static const int RING_SIZE  = FFT_SIZE * 4;   // Must be a power of 2
static const int SCOPE_N    = 512;
static const int IDT_RENDER = 2001;
static const int IDT_RETRY  = 2002;
static const int IDT_IDLE   = 2003;           // 1-second idle counter timer
static const WCHAR* FONT_NAME = L"Segoe UI Variable Display";

#define TIMER_MEDIA_FADE 3001
#define TIMER_MEDIA_SHOW 3002
#define TIMER_VOLUME_FADE 4001
#define TIMER_VOLUME_SHOW 4002

static_assert((RING_SIZE & (RING_SIZE - 1)) == 0, "RING_SIZE must be power of 2");

// =========================================================================
// DWM Acrylic Blur API (undocumented - WS_EX_LAYERED not needed)
// =========================================================================
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED                   = 0,
    ACCENT_ENABLE_BLURBEHIND          = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND   = 4
} ACCENT_STATE;
typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD        AccentFlags;
    DWORD        GradientColor;
    DWORD        AnimationId;
} ACCENT_POLICY;
typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID                   Data;
    SIZE_T                  SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// =========================================================================
// Z-Band API (undocumented - create window in taskbar z-band)
// =========================================================================
enum ZBID {
    ZBID_DEFAULT               = 0,
    ZBID_IMMERSIVE_NOTIFICATION = 4
};
typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
    LPVOID lpParam, DWORD dwBand);

// =========================================================================
// SETTINGS
// =========================================================================
struct ModSettings {
    bool  showVisualizer    = true;
    bool  showMediaInfo     = true;
    bool  showMediaControls = true;
    int   barCount          = 48;
    int   style             = 0;
    int   colorMode         = 0;
    DWORD color1            = 0xFF00CCFF;
    DWORD color2            = 0xFFFF3300;
    int   smoothing         = 70;
    int   sensitivity       = 200;
    int   panelW            = 320;
    int   panelH            = 48;
    int   position          = 1;
    int   offsetX           = 0;
    int   offsetY           = 0;
    int   targetFPS         = 30;
    bool  hideOnFullscreen  = true;
    int   silenceTimeout    = 0;
    int   idleTimeout       = 0;
    bool  showPeaks         = true;
    int   peakFallSpeed     = 2;
    bool  glowEffect        = false;
    int   glowIntensity     = 40;
    int   barSpacing        = 1;
    int   blockSize         = 4;
    int   blockGap          = 1;
    int   bgOpacity         = 0;
    int   freqMin           = 20;
    int   freqMax           = 18000;
    float waveLineW         = 2.0f;
    int   waveFillAlpha     = 50;
    int   mirrorGap         = 2;
    bool  autoTheme         = true;
    int   scrollSpeed       = 2;
    int   visWidthPercent   = 50;
    int   titleFontSize     = 10;
    int   artistFontSize    = 8;
    bool  titleFontBold     = true;
    bool  artistFontItalic  = false;
    int   textOffsetX       = 0;
    int   textOffsetY       = 0;
    int   lineGap           = 1;
    int   scrollPadding     = 30;
    bool  useCustomTextColors = false;
    DWORD titleColor        = 0xFFFFFFFF;
    DWORD artistColor       = 0xFFAAAAAA;
    bool  showTextShadow    = true;
    int   captureMode       = 0;
    bool  showMediaFlyout   = true;
    bool  flyoutOnFullscreen = true;
    int   mediaFlyoutPosition = 0;
    int   mediaFlyoutDuration = 4;
    int   mediaFlyoutW          = 520;
    int   mediaFlyoutH          = 180;
    int   mediaFlyoutOffsetY    = 18;
    int   mediaFlyoutArtSize    = 100;
    int   mediaFlyoutBtnSize    = 36;
    int   mediaFlyoutTitleSize  = 14;
    int   mediaFlyoutArtistSize = 11;
    bool  showVolumeFlyout  = true;
    int   volumeFlyoutDuration = 3;
    int   volumeFlyoutW         = 360;
    int   volumeFlyoutH         = 80;
    int   volumeFlyoutOffsetY   = 18;
    int   volumeFlyoutIconSize  = 36;
    int   volumeFlyoutTextSize  = 13;
    int   flyoutBgOpacity       = 185;

    // Customizable Layout Settings
    int   panelMargin           = 2;
    int   panelPadding          = 4;
    int   panelGap              = 8;
    int   panelCornerRadius     = 7;
    int   panelBorderThickness  = 1;
    int   panelBtnSize          = 22;
    int   mediaArtCornerRadius  = 5;
    int   appIconSize           = 12;
    int   dividerPadding        = 4;
    int   progressBarHeight     = 2;
    int   panelTextGap          = 8;
    int   panelPbGap            = 4;
    int   mediaArtSize          = 0;

    int   mediaFlyoutMarginX       = 24;
    int   mediaFlyoutMarginY       = 24;
    int   mediaFlyoutLabelY        = 16;
    int   mediaFlyoutLabelFontSize = 9;
    int   mediaFlyoutArtCornerRadius = 12;
    int   mediaFlyoutBadgeSize     = 22;
    int   mediaFlyoutPbGap         = 14;
    int   mediaFlyoutPbHeight      = 6;
    int   mediaFlyoutTextGap       = 24;
    int   mediaFlyoutLineGap       = 10;
    int   mediaFlyoutBtnSpacing    = 14;
    int   mediaFlyoutKnobSize      = 14;
    int   mediaFlyoutTimeTextSize  = 10;
    int   mediaFlyoutCornerRadius  = 18;
    int   mediaFlyoutBorderThickness = 1;

    int   volumeFlyoutMarginX      = 24;
    int   volumeFlyoutTextGap      = 18;
    int   volumeFlyoutSliderGap    = 14;
    int   volumeFlyoutSliderHeight = 8;
    int   volumeFlyoutPaddingRight = 24;
    int   volumeFlyoutCornerRadius = 14;
    int   volumeFlyoutBorderThickness = 1;

    int   presetMode               = 0;
    int   presetAction             = 0;
} g_Settings;

static void SavePreset(int index) {
    auto writeNum = [&](const wchar_t* key, int val) {
        wchar_t name[96];
        swprintf_s(name, L"Preset%d_%s", index, key);
        Wh_SetIntValue(name, val);
    };

    writeNum(L"panelW", g_Settings.panelW);
    writeNum(L"panelH", g_Settings.panelH);
    writeNum(L"panelMargin", g_Settings.panelMargin);
    writeNum(L"panelPadding", g_Settings.panelPadding);
    writeNum(L"panelGap", g_Settings.panelGap);
    writeNum(L"panelCornerRadius", g_Settings.panelCornerRadius);
    writeNum(L"panelBorderThickness", g_Settings.panelBorderThickness);
    writeNum(L"panelBtnSize", g_Settings.panelBtnSize);
    writeNum(L"mediaArtCornerRadius", g_Settings.mediaArtCornerRadius);
    writeNum(L"appIconSize", g_Settings.appIconSize);
    writeNum(L"dividerPadding", g_Settings.dividerPadding);
    writeNum(L"progressBarHeight", g_Settings.progressBarHeight);
    writeNum(L"panelTextGap", g_Settings.panelTextGap);
    writeNum(L"panelPbGap", g_Settings.panelPbGap);
    writeNum(L"mediaArtSize", g_Settings.mediaArtSize);

    writeNum(L"mediaFlyoutW", g_Settings.mediaFlyoutW);
    writeNum(L"mediaFlyoutH", g_Settings.mediaFlyoutH);
    writeNum(L"mediaFlyoutOffsetY", g_Settings.mediaFlyoutOffsetY);
    writeNum(L"mediaFlyoutArtSize", g_Settings.mediaFlyoutArtSize);
    writeNum(L"mediaFlyoutBtnSize", g_Settings.mediaFlyoutBtnSize);
    writeNum(L"mediaFlyoutTitleSize", g_Settings.mediaFlyoutTitleSize);
    writeNum(L"mediaFlyoutArtistSize", g_Settings.mediaFlyoutArtistSize);
    writeNum(L"mediaFlyoutMarginX", g_Settings.mediaFlyoutMarginX);
    writeNum(L"mediaFlyoutMarginY", g_Settings.mediaFlyoutMarginY);
    writeNum(L"mediaFlyoutLabelY", g_Settings.mediaFlyoutLabelY);
    writeNum(L"mediaFlyoutLabelFontSize", g_Settings.mediaFlyoutLabelFontSize);
    writeNum(L"mediaFlyoutArtCornerRadius", g_Settings.mediaFlyoutArtCornerRadius);
    writeNum(L"mediaFlyoutBadgeSize", g_Settings.mediaFlyoutBadgeSize);
    writeNum(L"mediaFlyoutPbGap", g_Settings.mediaFlyoutPbGap);
    writeNum(L"mediaFlyoutPbHeight", g_Settings.mediaFlyoutPbHeight);
    writeNum(L"mediaFlyoutTextGap", g_Settings.mediaFlyoutTextGap);
    writeNum(L"mediaFlyoutLineGap", g_Settings.mediaFlyoutLineGap);
    writeNum(L"mediaFlyoutBtnSpacing", g_Settings.mediaFlyoutBtnSpacing);
    writeNum(L"mediaFlyoutKnobSize", g_Settings.mediaFlyoutKnobSize);
    writeNum(L"mediaFlyoutTimeTextSize", g_Settings.mediaFlyoutTimeTextSize);
    writeNum(L"mediaFlyoutCornerRadius", g_Settings.mediaFlyoutCornerRadius);
    writeNum(L"mediaFlyoutBorderThickness", g_Settings.mediaFlyoutBorderThickness);

    writeNum(L"volumeFlyoutW", g_Settings.volumeFlyoutW);
    writeNum(L"volumeFlyoutH", g_Settings.volumeFlyoutH);
    writeNum(L"volumeFlyoutOffsetY", g_Settings.volumeFlyoutOffsetY);
    writeNum(L"volumeFlyoutIconSize", g_Settings.volumeFlyoutIconSize);
    writeNum(L"volumeFlyoutTextSize", g_Settings.volumeFlyoutTextSize);
    writeNum(L"volumeFlyoutMarginX", g_Settings.volumeFlyoutMarginX);
    writeNum(L"volumeFlyoutTextGap", g_Settings.volumeFlyoutTextGap);
    writeNum(L"volumeFlyoutSliderGap", g_Settings.volumeFlyoutSliderGap);
    writeNum(L"volumeFlyoutSliderHeight", g_Settings.volumeFlyoutSliderHeight);
    writeNum(L"volumeFlyoutPaddingRight", g_Settings.volumeFlyoutPaddingRight);
    writeNum(L"volumeFlyoutCornerRadius", g_Settings.volumeFlyoutCornerRadius);
    writeNum(L"volumeFlyoutBorderThickness", g_Settings.volumeFlyoutBorderThickness);

    writeNum(L"titleFontSize", g_Settings.titleFontSize);
    writeNum(L"artistFontSize", g_Settings.artistFontSize);
}

static void LoadPreset(int index) {
    auto readNum = [&](const wchar_t* key, int def) -> int {
        wchar_t name[96];
        swprintf_s(name, L"Preset%d_%s", index, key);
        return Wh_GetIntValue(name, def);
    };

    g_Settings.panelW = readNum(L"panelW", g_Settings.panelW);
    g_Settings.panelH = readNum(L"panelH", g_Settings.panelH);
    g_Settings.panelMargin = readNum(L"panelMargin", g_Settings.panelMargin);
    g_Settings.panelPadding = readNum(L"panelPadding", g_Settings.panelPadding);
    g_Settings.panelGap = readNum(L"panelGap", g_Settings.panelGap);
    g_Settings.panelCornerRadius = readNum(L"panelCornerRadius", g_Settings.panelCornerRadius);
    g_Settings.panelBorderThickness = readNum(L"panelBorderThickness", g_Settings.panelBorderThickness);
    g_Settings.panelBtnSize = readNum(L"panelBtnSize", g_Settings.panelBtnSize);
    g_Settings.mediaArtCornerRadius = readNum(L"mediaArtCornerRadius", g_Settings.mediaArtCornerRadius);
    g_Settings.appIconSize = readNum(L"appIconSize", g_Settings.appIconSize);
    g_Settings.dividerPadding = readNum(L"dividerPadding", g_Settings.dividerPadding);
    g_Settings.progressBarHeight = readNum(L"progressBarHeight", g_Settings.progressBarHeight);
    g_Settings.panelTextGap = readNum(L"panelTextGap", g_Settings.panelTextGap);
    g_Settings.panelPbGap = readNum(L"panelPbGap", g_Settings.panelPbGap);
    g_Settings.mediaArtSize = readNum(L"mediaArtSize", g_Settings.mediaArtSize);

    g_Settings.mediaFlyoutW = readNum(L"mediaFlyoutW", g_Settings.mediaFlyoutW);
    g_Settings.mediaFlyoutH = readNum(L"mediaFlyoutH", g_Settings.mediaFlyoutH);
    g_Settings.mediaFlyoutOffsetY = readNum(L"mediaFlyoutOffsetY", g_Settings.mediaFlyoutOffsetY);
    g_Settings.mediaFlyoutArtSize = readNum(L"mediaFlyoutArtSize", g_Settings.mediaFlyoutArtSize);
    g_Settings.mediaFlyoutBtnSize = readNum(L"mediaFlyoutBtnSize", g_Settings.mediaFlyoutBtnSize);
    g_Settings.mediaFlyoutTitleSize = readNum(L"mediaFlyoutTitleSize", g_Settings.mediaFlyoutTitleSize);
    g_Settings.mediaFlyoutArtistSize = readNum(L"mediaFlyoutArtistSize", g_Settings.mediaFlyoutArtistSize);
    g_Settings.mediaFlyoutMarginX = readNum(L"mediaFlyoutMarginX", g_Settings.mediaFlyoutMarginX);
    g_Settings.mediaFlyoutMarginY = readNum(L"mediaFlyoutMarginY", g_Settings.mediaFlyoutMarginY);
    g_Settings.mediaFlyoutLabelY = readNum(L"mediaFlyoutLabelY", g_Settings.mediaFlyoutLabelY);
    g_Settings.mediaFlyoutLabelFontSize = readNum(L"mediaFlyoutLabelFontSize", g_Settings.mediaFlyoutLabelFontSize);
    g_Settings.mediaFlyoutArtCornerRadius = readNum(L"mediaFlyoutArtCornerRadius", g_Settings.mediaFlyoutArtCornerRadius);
    g_Settings.mediaFlyoutBadgeSize = readNum(L"mediaFlyoutBadgeSize", g_Settings.mediaFlyoutBadgeSize);
    g_Settings.mediaFlyoutPbGap = readNum(L"mediaFlyoutPbGap", g_Settings.mediaFlyoutPbGap);
    g_Settings.mediaFlyoutPbHeight = readNum(L"mediaFlyoutPbHeight", g_Settings.mediaFlyoutPbHeight);
    g_Settings.mediaFlyoutTextGap = readNum(L"mediaFlyoutTextGap", g_Settings.mediaFlyoutTextGap);
    g_Settings.mediaFlyoutLineGap = readNum(L"mediaFlyoutLineGap", g_Settings.mediaFlyoutLineGap);
    g_Settings.mediaFlyoutBtnSpacing = readNum(L"mediaFlyoutBtnSpacing", g_Settings.mediaFlyoutBtnSpacing);
    g_Settings.mediaFlyoutKnobSize = readNum(L"mediaFlyoutKnobSize", g_Settings.mediaFlyoutKnobSize);
    g_Settings.mediaFlyoutTimeTextSize = readNum(L"mediaFlyoutTimeTextSize", g_Settings.mediaFlyoutTimeTextSize);
    g_Settings.mediaFlyoutCornerRadius = readNum(L"mediaFlyoutCornerRadius", g_Settings.mediaFlyoutCornerRadius);
    g_Settings.mediaFlyoutBorderThickness = readNum(L"mediaFlyoutBorderThickness", g_Settings.mediaFlyoutBorderThickness);

    g_Settings.volumeFlyoutW = readNum(L"volumeFlyoutW", g_Settings.volumeFlyoutW);
    g_Settings.volumeFlyoutH = readNum(L"volumeFlyoutH", g_Settings.volumeFlyoutH);
    g_Settings.volumeFlyoutOffsetY = readNum(L"volumeFlyoutOffsetY", g_Settings.volumeFlyoutOffsetY);
    g_Settings.volumeFlyoutIconSize = readNum(L"volumeFlyoutIconSize", g_Settings.volumeFlyoutIconSize);
    g_Settings.volumeFlyoutTextSize = readNum(L"volumeFlyoutTextSize", g_Settings.volumeFlyoutTextSize);
    g_Settings.volumeFlyoutMarginX = readNum(L"volumeFlyoutMarginX", g_Settings.volumeFlyoutMarginX);
    g_Settings.volumeFlyoutTextGap = readNum(L"volumeFlyoutTextGap", g_Settings.volumeFlyoutTextGap);
    g_Settings.volumeFlyoutSliderGap = readNum(L"volumeFlyoutSliderGap", g_Settings.volumeFlyoutSliderGap);
    g_Settings.volumeFlyoutSliderHeight = readNum(L"volumeFlyoutSliderHeight", g_Settings.volumeFlyoutSliderHeight);
    g_Settings.volumeFlyoutPaddingRight = readNum(L"volumeFlyoutPaddingRight", g_Settings.volumeFlyoutPaddingRight);
    g_Settings.volumeFlyoutCornerRadius = readNum(L"volumeFlyoutCornerRadius", g_Settings.volumeFlyoutCornerRadius);
    g_Settings.volumeFlyoutBorderThickness = readNum(L"volumeFlyoutBorderThickness", g_Settings.volumeFlyoutBorderThickness);

    g_Settings.titleFontSize = readNum(L"titleFontSize", g_Settings.titleFontSize);
    g_Settings.artistFontSize = readNum(L"artistFontSize", g_Settings.artistFontSize);
}

static void LoadSettings() {
    auto clamp = [](int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; };
    auto readOptionInt = [](const wchar_t* key, int def) -> int {
        PCWSTR s = Wh_GetStringSetting(key);
        int v = (s[0] != L'\0') ? _wtoi(s) : def;
        Wh_FreeStringSetting(s);
        return v;
    };

    g_Settings.showVisualizer    = Wh_GetIntSetting(L"ShowVisualizer")    != 0;
    g_Settings.showMediaInfo     = Wh_GetIntSetting(L"ShowMediaInfo")     != 0;
    g_Settings.showMediaControls = Wh_GetIntSetting(L"ShowMediaControls") != 0;
    g_Settings.barCount          = clamp(Wh_GetIntSetting(L"BarCount"),       4,  MAX_BARS);
    g_Settings.style             = clamp(readOptionInt(L"Style", 0),          0,  7);
    g_Settings.colorMode         = clamp(readOptionInt(L"ColorMode", 0),      0,  5);
    g_Settings.smoothing         = clamp(Wh_GetIntSetting(L"Smoothing"),      0,  95);
    g_Settings.sensitivity       = clamp(Wh_GetIntSetting(L"Sensitivity"),    10, 1000);
    g_Settings.panelW            = clamp(Wh_GetIntSetting(L"PanelWidth"),     60, 2000);
    g_Settings.panelH            = clamp(Wh_GetIntSetting(L"PanelHeight"),    16, 400);
    g_Settings.position          = clamp(readOptionInt(L"Position", 1),       0,  2);
    g_Settings.offsetX           = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY           = Wh_GetIntSetting(L"OffsetY");
    g_Settings.targetFPS         = clamp(Wh_GetIntSetting(L"TargetFPS"),      10, 60);
    g_Settings.hideOnFullscreen  = Wh_GetIntSetting(L"HideOnFullscreen")  != 0;
    g_Settings.silenceTimeout    = clamp(Wh_GetIntSetting(L"SilenceTimeout"), 0,  60);
    g_Settings.idleTimeout       = clamp(Wh_GetIntSetting(L"IdleTimeout"),    0,  3600);
    g_Settings.showPeaks         = Wh_GetIntSetting(L"ShowPeaks")         != 0;
    g_Settings.peakFallSpeed     = clamp(Wh_GetIntSetting(L"PeakFallSpeed"),  1,  10);
    g_Settings.glowEffect        = Wh_GetIntSetting(L"GlowEffect")        != 0;
    g_Settings.glowIntensity     = clamp(Wh_GetIntSetting(L"GlowIntensity"),  5,  150);
    g_Settings.barSpacing        = clamp(Wh_GetIntSetting(L"BarSpacing"),     0,  20);
    g_Settings.blockSize         = clamp(Wh_GetIntSetting(L"BlockSize"),      2,  32);
    g_Settings.blockGap          = clamp(Wh_GetIntSetting(L"BlockGap"),       0,  8);
    g_Settings.bgOpacity         = clamp(Wh_GetIntSetting(L"BgOpacity"),      0,  255);
    g_Settings.freqMin           = clamp(Wh_GetIntSetting(L"FreqMin"),        1,  1000);
    g_Settings.freqMax           = clamp(Wh_GetIntSetting(L"FreqMax"),        1000, 24000);
    g_Settings.waveFillAlpha     = clamp(Wh_GetIntSetting(L"WaveFillAlpha"),  0,  200);
    g_Settings.mirrorGap         = clamp(Wh_GetIntSetting(L"MirrorGap"),      0,  20);
    g_Settings.autoTheme         = Wh_GetIntSetting(L"AutoTheme")         != 0;
    g_Settings.scrollSpeed       = clamp(Wh_GetIntSetting(L"ScrollSpeed"),    1,  5);
    g_Settings.visWidthPercent   = clamp(Wh_GetIntSetting(L"VisWidthPercent"), 15, 85);
    g_Settings.titleFontSize     = clamp(Wh_GetIntSetting(L"TitleFontSize"),  6, 20);
    g_Settings.artistFontSize    = clamp(Wh_GetIntSetting(L"ArtistFontSize"), 6, 18);
    g_Settings.titleFontBold     = Wh_GetIntSetting(L"TitleFontBold")     != 0;
    g_Settings.artistFontItalic  = Wh_GetIntSetting(L"ArtistFontItalic")  != 0;
    g_Settings.textOffsetX       = Wh_GetIntSetting(L"TextOffsetX");
    g_Settings.textOffsetY       = Wh_GetIntSetting(L"TextOffsetY");
    g_Settings.lineGap           = clamp(Wh_GetIntSetting(L"LineGap"),       -10, 50);
    g_Settings.scrollPadding     = clamp(Wh_GetIntSetting(L"ScrollPadding"), 10, 500);
    g_Settings.useCustomTextColors = Wh_GetIntSetting(L"UseCustomTextColors") != 0;
    g_Settings.showTextShadow    = Wh_GetIntSetting(L"ShowTextShadow")    != 0;
    g_Settings.captureMode       = clamp(readOptionInt(L"CaptureMode", 0), 0, 1);

    g_Settings.showMediaFlyout   = Wh_GetIntSetting(L"ShowMediaFlyout") != 0;
    g_Settings.flyoutOnFullscreen = Wh_GetIntSetting(L"FlyoutOnFullscreen") != 0;
    g_Settings.mediaFlyoutPosition = clamp(readOptionInt(L"MediaFlyoutPosition", 0), 0, 6);
    g_Settings.mediaFlyoutDuration = clamp(Wh_GetIntSetting(L"MediaFlyoutDuration"), 1, 60);
    g_Settings.mediaFlyoutW      = clamp(Wh_GetIntSetting(L"MediaFlyoutWidth"), 200, 1000);
    g_Settings.mediaFlyoutH      = clamp(Wh_GetIntSetting(L"MediaFlyoutHeight"), 60, 400);
    g_Settings.mediaFlyoutOffsetY = Wh_GetIntSetting(L"MediaFlyoutOffsetY");
    g_Settings.mediaFlyoutArtSize = clamp(Wh_GetIntSetting(L"MediaFlyoutArtSize"), 20, 300);
    g_Settings.mediaFlyoutBtnSize = clamp(Wh_GetIntSetting(L"MediaFlyoutBtnSize"), 10, 100);
    g_Settings.mediaFlyoutTitleSize = clamp(Wh_GetIntSetting(L"MediaFlyoutTitleSize"), 5, 30);
    g_Settings.mediaFlyoutArtistSize = clamp(Wh_GetIntSetting(L"MediaFlyoutArtistSize"), 5, 30);
    g_Settings.showVolumeFlyout  = Wh_GetIntSetting(L"ShowVolumeFlyout") != 0;
    g_Settings.volumeFlyoutDuration = clamp(Wh_GetIntSetting(L"VolumeFlyoutDuration"), 1, 60);
    g_Settings.volumeFlyoutW     = clamp(Wh_GetIntSetting(L"VolumeFlyoutWidth"), 100, 800);
    g_Settings.volumeFlyoutH     = clamp(Wh_GetIntSetting(L"VolumeFlyoutHeight"), 20, 300);
    g_Settings.volumeFlyoutOffsetY = Wh_GetIntSetting(L"VolumeFlyoutOffsetY");
    g_Settings.volumeFlyoutIconSize = clamp(Wh_GetIntSetting(L"VolumeFlyoutIconSize"), 8, 100);
    g_Settings.volumeFlyoutTextSize = clamp(Wh_GetIntSetting(L"VolumeFlyoutTextSize"), 5, 30);
    g_Settings.flyoutBgOpacity   = clamp(Wh_GetIntSetting(L"FlyoutBgOpacity"), 0, 255);

    // Load Layout Parameters
    g_Settings.panelMargin           = clamp(Wh_GetIntSetting(L"PanelMargin"), 0, 100);
    g_Settings.panelPadding          = clamp(Wh_GetIntSetting(L"PanelPadding"), 0, 100);
    g_Settings.panelGap              = clamp(Wh_GetIntSetting(L"PanelGap"), 0, 200);
    g_Settings.panelCornerRadius     = clamp(Wh_GetIntSetting(L"PanelCornerRadius"), 0, 100);
    g_Settings.panelBorderThickness  = clamp(Wh_GetIntSetting(L"PanelBorderThickness"), 0, 20);
    g_Settings.panelBtnSize          = clamp(Wh_GetIntSetting(L"PanelBtnSize"), 5, 100);
    g_Settings.mediaArtCornerRadius  = clamp(Wh_GetIntSetting(L"MediaArtCornerRadius"), 0, 100);
    g_Settings.appIconSize           = clamp(Wh_GetIntSetting(L"AppIconSize"), 4, 100);
    g_Settings.dividerPadding        = clamp(Wh_GetIntSetting(L"DividerPadding"), 0, 100);
    g_Settings.progressBarHeight     = clamp(Wh_GetIntSetting(L"ProgressBarHeight"), 1, 50);
    g_Settings.panelTextGap          = clamp(Wh_GetIntSetting(L"PanelTextGap"), 0, 100);
    g_Settings.panelPbGap            = clamp(Wh_GetIntSetting(L"PanelPbGap"), 0, 100);
    g_Settings.mediaArtSize          = clamp(Wh_GetIntSetting(L"MediaArtSize"), 0, 300);

    g_Settings.mediaFlyoutMarginX       = clamp(Wh_GetIntSetting(L"MediaFlyoutMarginX"), 0, 200);
    g_Settings.mediaFlyoutMarginY       = clamp(Wh_GetIntSetting(L"MediaFlyoutMarginY"), 0, 200);
    g_Settings.mediaFlyoutLabelY        = clamp(Wh_GetIntSetting(L"MediaFlyoutLabelY"), 0, 150);
    g_Settings.mediaFlyoutLabelFontSize = clamp(Wh_GetIntSetting(L"MediaFlyoutLabelFontSize"), 5, 50);
    g_Settings.mediaFlyoutArtCornerRadius = clamp(Wh_GetIntSetting(L"MediaFlyoutArtCornerRadius"), 0, 100);
    g_Settings.mediaFlyoutBadgeSize     = clamp(Wh_GetIntSetting(L"MediaFlyoutBadgeSize"), 4, 100);
    g_Settings.mediaFlyoutPbGap         = clamp(Wh_GetIntSetting(L"MediaFlyoutPbGap"), 0, 100);
    g_Settings.mediaFlyoutPbHeight      = clamp(Wh_GetIntSetting(L"MediaFlyoutPbHeight"), 1, 50);
    g_Settings.mediaFlyoutTextGap       = clamp(Wh_GetIntSetting(L"MediaFlyoutTextGap"), 0, 100);
    g_Settings.mediaFlyoutLineGap       = clamp(Wh_GetIntSetting(L"MediaFlyoutLineGap"), -10, 100);
    g_Settings.mediaFlyoutBtnSpacing    = clamp(Wh_GetIntSetting(L"MediaFlyoutBtnSpacing"), 0, 100);
    g_Settings.mediaFlyoutKnobSize      = clamp(Wh_GetIntSetting(L"MediaFlyoutKnobSize"), 2, 50);
    g_Settings.mediaFlyoutTimeTextSize  = clamp(Wh_GetIntSetting(L"MediaFlyoutTimeTextSize"), 5, 50);
    g_Settings.mediaFlyoutCornerRadius  = clamp(Wh_GetIntSetting(L"MediaFlyoutCornerRadius"), 0, 100);
    g_Settings.mediaFlyoutBorderThickness = clamp(Wh_GetIntSetting(L"MediaFlyoutBorderThickness"), 0, 20);

    g_Settings.volumeFlyoutMarginX      = clamp(Wh_GetIntSetting(L"VolumeFlyoutMarginX"), 0, 200);
    g_Settings.volumeFlyoutTextGap      = clamp(Wh_GetIntSetting(L"VolumeFlyoutTextGap"), 0, 100);
    g_Settings.volumeFlyoutSliderGap    = clamp(Wh_GetIntSetting(L"VolumeFlyoutSliderGap"), 0, 100);
    g_Settings.volumeFlyoutSliderHeight = clamp(Wh_GetIntSetting(L"VolumeFlyoutSliderHeight"), 1, 50);
    g_Settings.volumeFlyoutPaddingRight = clamp(Wh_GetIntSetting(L"VolumeFlyoutPaddingRight"), 0, 200);
    g_Settings.volumeFlyoutCornerRadius = clamp(Wh_GetIntSetting(L"VolumeFlyoutCornerRadius"), 0, 100);
    g_Settings.volumeFlyoutBorderThickness = clamp(Wh_GetIntSetting(L"VolumeFlyoutBorderThickness"), 0, 20);

    float lw = (float)Wh_GetIntSetting(L"WaveLineWidth");
    g_Settings.waveLineW = (lw < 0.5f) ? 0.5f : (lw > 8.0f) ? 8.0f : lw;

    auto readColor = [](const wchar_t* key, DWORD def) -> DWORD {
        PCWSTR s = Wh_GetStringSetting(key);
        DWORD  v = def;
        if (s && wcslen(s) > 0) v = 0xFF000000 | wcstoul(s, nullptr, 16);
        if (s) Wh_FreeStringSetting(s);
        return v;
    };
    g_Settings.color1 = readColor(L"Color1", 0xFF00CCFF);
    g_Settings.color2 = readColor(L"Color2", 0xFFFF3300);
    g_Settings.titleColor = readColor(L"TitleColor", 0xFFFFFFFF);
    g_Settings.artistColor = readColor(L"ArtistColor", 0xFFAAAAAA);

    g_Settings.presetMode   = clamp(readOptionInt(L"PresetMode", 0), 0, 6);
    g_Settings.presetAction = clamp(readOptionInt(L"PresetAction", 0), 0, 3);

    if (g_Settings.presetAction >= 1 && g_Settings.presetAction <= 3) {
        SavePreset(g_Settings.presetAction);
    }

    if (g_Settings.presetMode == 1) { // Wide & Spacious
        g_Settings.panelW = 460;
        g_Settings.panelH = 56;
        g_Settings.panelMargin = 4;
        g_Settings.panelPadding = 8;
        g_Settings.panelGap = 12;
        g_Settings.panelCornerRadius = 12;
        g_Settings.panelBtnSize = 28;
        g_Settings.mediaArtCornerRadius = 8;
        g_Settings.appIconSize = 16;
        g_Settings.dividerPadding = 6;
        g_Settings.progressBarHeight = 4;
        g_Settings.panelTextGap = 12;
        g_Settings.panelPbGap = 6;
        g_Settings.titleFontSize = 11;
        g_Settings.artistFontSize = 9;

        g_Settings.mediaFlyoutW = 520;
        g_Settings.mediaFlyoutH = 180;
        g_Settings.mediaFlyoutOffsetY = 18;
        g_Settings.mediaFlyoutArtSize = 100;
        g_Settings.mediaFlyoutBtnSize = 36;
        g_Settings.mediaFlyoutTitleSize = 14;
        g_Settings.mediaFlyoutArtistSize = 11;
        g_Settings.mediaFlyoutMarginX = 24;
        g_Settings.mediaFlyoutMarginY = 24;
        g_Settings.mediaFlyoutLabelY = 16;
        g_Settings.mediaFlyoutLabelFontSize = 9;
        g_Settings.mediaFlyoutArtCornerRadius = 12;
        g_Settings.mediaFlyoutBadgeSize = 22;
        g_Settings.mediaFlyoutPbGap = 14;
        g_Settings.mediaFlyoutPbHeight = 6;
        g_Settings.mediaFlyoutTextGap = 24;
        g_Settings.mediaFlyoutLineGap = 10;
        g_Settings.mediaFlyoutBtnSpacing = 14;
        g_Settings.mediaFlyoutKnobSize = 14;
        g_Settings.mediaFlyoutTimeTextSize = 10;
        g_Settings.mediaFlyoutCornerRadius = 18;
        g_Settings.mediaFlyoutBorderThickness = 1;

        g_Settings.volumeFlyoutW = 360;
        g_Settings.volumeFlyoutH = 80;
        g_Settings.volumeFlyoutOffsetY = 18;
        g_Settings.volumeFlyoutIconSize = 36;
        g_Settings.volumeFlyoutTextSize = 13;
        g_Settings.volumeFlyoutMarginX = 24;
        g_Settings.volumeFlyoutTextGap = 18;
        g_Settings.volumeFlyoutSliderGap = 14;
        g_Settings.volumeFlyoutSliderHeight = 8;
        g_Settings.volumeFlyoutPaddingRight = 24;
        g_Settings.volumeFlyoutCornerRadius = 14;
        g_Settings.volumeFlyoutBorderThickness = 1;
    }
    else if (g_Settings.presetMode == 2) { // Mini & Compact
        g_Settings.panelW = 300;
        g_Settings.panelH = 38;
        g_Settings.panelMargin = 2;
        g_Settings.panelPadding = 4;
        g_Settings.panelGap = 6;
        g_Settings.panelCornerRadius = 6;
        g_Settings.panelBtnSize = 20;
        g_Settings.mediaArtCornerRadius = 4;
        g_Settings.appIconSize = 10;
        g_Settings.dividerPadding = 3;
        g_Settings.progressBarHeight = 2;
        g_Settings.panelTextGap = 6;
        g_Settings.panelPbGap = 3;
        g_Settings.titleFontSize = 9;
        g_Settings.artistFontSize = 7;

        g_Settings.mediaFlyoutW = 320;
        g_Settings.mediaFlyoutH = 110;
        g_Settings.mediaFlyoutOffsetY = 10;
        g_Settings.mediaFlyoutArtSize = 64;
        g_Settings.mediaFlyoutBtnSize = 26;
        g_Settings.mediaFlyoutTitleSize = 11;
        g_Settings.mediaFlyoutArtistSize = 9;
        g_Settings.mediaFlyoutMarginX = 14;
        g_Settings.mediaFlyoutMarginY = 14;
        g_Settings.mediaFlyoutLabelY = 8;
        g_Settings.mediaFlyoutLabelFontSize = 7;
        g_Settings.mediaFlyoutArtCornerRadius = 4;
        g_Settings.mediaFlyoutBadgeSize = 12;
        g_Settings.mediaFlyoutPbGap = 6;
        g_Settings.mediaFlyoutPbHeight = 2;
        g_Settings.mediaFlyoutTextGap = 14;
        g_Settings.mediaFlyoutLineGap = 4;
        g_Settings.mediaFlyoutBtnSpacing = 8;
        g_Settings.mediaFlyoutKnobSize = 8;
        g_Settings.mediaFlyoutTimeTextSize = 8;
        g_Settings.mediaFlyoutCornerRadius = 8;
        g_Settings.mediaFlyoutBorderThickness = 1;

        g_Settings.volumeFlyoutW = 260;
        g_Settings.volumeFlyoutH = 56;
        g_Settings.volumeFlyoutOffsetY = 10;
        g_Settings.volumeFlyoutIconSize = 24;
        g_Settings.volumeFlyoutTextSize = 10;
        g_Settings.volumeFlyoutMarginX = 16;
        g_Settings.volumeFlyoutTextGap = 12;
        g_Settings.volumeFlyoutSliderGap = 8;
        g_Settings.volumeFlyoutSliderHeight = 4;
        g_Settings.volumeFlyoutPaddingRight = 16;
        g_Settings.volumeFlyoutCornerRadius = 8;
        g_Settings.volumeFlyoutBorderThickness = 1;
    }
    else if (g_Settings.presetMode == 3) { // Large Screen
        g_Settings.panelW = 560;
        g_Settings.panelH = 72;
        g_Settings.panelMargin = 6;
        g_Settings.panelPadding = 12;
        g_Settings.panelGap = 18;
        g_Settings.panelCornerRadius = 16;
        g_Settings.panelBtnSize = 38;
        g_Settings.mediaArtCornerRadius = 12;
        g_Settings.appIconSize = 22;
        g_Settings.dividerPadding = 10;
        g_Settings.progressBarHeight = 6;
        g_Settings.panelTextGap = 18;
        g_Settings.panelPbGap = 10;
        g_Settings.titleFontSize = 14;
        g_Settings.artistFontSize = 11;

        g_Settings.mediaFlyoutW = 600;
        g_Settings.mediaFlyoutH = 210;
        g_Settings.mediaFlyoutOffsetY = 20;
        g_Settings.mediaFlyoutArtSize = 120;
        g_Settings.mediaFlyoutBtnSize = 48;
        g_Settings.mediaFlyoutTitleSize = 18;
        g_Settings.mediaFlyoutArtistSize = 13;
        g_Settings.mediaFlyoutMarginX = 32;
        g_Settings.mediaFlyoutMarginY = 32;
        g_Settings.mediaFlyoutLabelY = 20;
        g_Settings.mediaFlyoutLabelFontSize = 11;
        g_Settings.mediaFlyoutArtCornerRadius = 14;
        g_Settings.mediaFlyoutBadgeSize = 28;
        g_Settings.mediaFlyoutPbGap = 18;
        g_Settings.mediaFlyoutPbHeight = 8;
        g_Settings.mediaFlyoutTextGap = 32;
        g_Settings.mediaFlyoutLineGap = 12;
        g_Settings.mediaFlyoutBtnSpacing = 18;
        g_Settings.mediaFlyoutKnobSize = 18;
        g_Settings.mediaFlyoutTimeTextSize = 12;
        g_Settings.mediaFlyoutCornerRadius = 24;
        g_Settings.mediaFlyoutBorderThickness = 2;

        g_Settings.volumeFlyoutW = 440;
        g_Settings.volumeFlyoutH = 96;
        g_Settings.volumeFlyoutOffsetY = 20;
        g_Settings.volumeFlyoutIconSize = 44;
        g_Settings.volumeFlyoutTextSize = 16;
        g_Settings.volumeFlyoutMarginX = 32;
        g_Settings.volumeFlyoutTextGap = 20;
        g_Settings.volumeFlyoutSliderGap = 18;
        g_Settings.volumeFlyoutSliderHeight = 10;
        g_Settings.volumeFlyoutPaddingRight = 32;
        g_Settings.volumeFlyoutCornerRadius = 18;
        g_Settings.volumeFlyoutBorderThickness = 2;
    }
    else if (g_Settings.presetMode >= 4 && g_Settings.presetMode <= 6) {
        LoadPreset(g_Settings.presetMode - 3);
    }
}

// =========================================================================
// GLOBAL STATE
// =========================================================================
static HWND          g_hVisWnd    = nullptr;
static HWND          g_hMediaFlyoutWnd = nullptr;
static HWND          g_hVolumeFlyoutWnd = nullptr;

static IAudioEndpointVolume* g_endpointVolume = nullptr;

static std::atomic<int> g_volumeLevel{0};
static std::atomic<bool> g_volumeMuted{false};

// Lock state variables
static std::atomic<int> g_volumeFlyoutType{0}; // 0=Volume, 1=Caps Lock, 2=Num Lock, 3=Scroll Lock
static std::atomic<bool> g_volumeFlyoutActive{false}; // true=On, false=Off

static HHOOK g_keyboardHook = nullptr;
static DWORD g_hookThreadId = 0;

static HMODULE GetOwnModuleHandle() {
    HMODULE hMod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)GetOwnModuleHandle,
        &hMod);
    return hMod ? hMod : GetModuleHandleW(nullptr);
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKbd = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            DWORD vk = pKbd->vkCode;
            if (vk == VK_CAPITAL || vk == VK_NUMLOCK || vk == VK_SCROLL) {
                bool active = (GetKeyState(vk) & 0x0001) != 0;

                int type = 0;
                if (vk == VK_CAPITAL) type = 1;
                else if (vk == VK_NUMLOCK) type = 2;
                else if (vk == VK_SCROLL) type = 3;

                if (g_hVolumeFlyoutWnd && IsWindow(g_hVolumeFlyoutWnd)) {
                    PostMessageW(g_hVolumeFlyoutWnd, WM_APP + 41, type, active ? 1 : 0);
                }
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static void AdjustSystemVolume(int delta) {
    if (g_endpointVolume) {
        float currentVol = 0.0f;
        g_endpointVolume->GetMasterVolumeLevelScalar(&currentVol);
        float newVol = currentVol + delta * 0.02f;
        if (newVol < 0.0f) newVol = 0.0f;
        if (newVol > 1.0f) newVol = 1.0f;
        g_endpointVolume->SetMasterVolumeLevelScalar(newVol, nullptr);
    }
}

// Volume Callback Class definition
class VolumeCallback : public IAudioEndpointVolumeCallback {
public:
    VolumeCallback() : _refCount(1) {}
    virtual ~VolumeCallback() {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppvInterface) override {
        if (IID_IUnknown == riid) {
            *ppvInterface = static_cast<IUnknown*>(this);
        } else if (__uuidof(IAudioEndpointVolumeCallback) == riid) {
            *ppvInterface = static_cast<IAudioEndpointVolumeCallback*>(this);
        } else {
            *ppvInterface = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&_refCount);
    }

    STDMETHODIMP_(ULONG) Release() override {
        ULONG ulRef = InterlockedDecrement(&_refCount);
        if (0 == ulRef) {
            delete this;
        }
        return ulRef;
    }

    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) override {
        if (pNotify == nullptr) return E_POINTER;
        
        int volPct = (int)(pNotify->fMasterVolume * 100.0f + 0.5f);
        bool muted = pNotify->bMuted != FALSE;
        
        int oldVol = g_volumeLevel.exchange(volPct);
        bool oldMuted = g_volumeMuted.exchange(muted);
        
        if (g_Settings.showVolumeFlyout) {
            // Only trigger volume flyout if the volume level or mute state actually changed
            if ((volPct != oldVol || muted != oldMuted) && g_hVolumeFlyoutWnd && IsWindow(g_hVolumeFlyoutWnd)) {
                PostMessageW(g_hVolumeFlyoutWnd, WM_APP + 41, 0, muted ? 1 : 0);
            }
        }

        return S_OK;
    }

private:
    ULONG _refCount;
};

static VolumeCallback* g_volumeCallback = nullptr;

// Flyout states
static int g_mediaFlyoutAlpha = 0;
static int g_mediaFlyoutState = 0; // 0=Hidden, 1=FadeIn, 2=Show, 3=FadeOut
static DWORD g_mediaFlyoutShowTimeLeft = 0;
static bool g_mediaFlyoutTrackingMouse = false;
static bool g_mediaFlyoutIsSeeking = false;
static float g_mediaFlyoutSeekProgress = 0.0f;

static int g_volumeFlyoutAlpha = 0;
static int g_volumeFlyoutState = 0; // 0=Hidden, 1=FadeIn, 2=Show, 3=FadeOut
static DWORD g_volumeFlyoutShowTimeLeft = 0;
static bool g_volumeFlyoutIsDragging = false;

struct FlyoutButton {
    int id; // 1=Prev, 2=Play/Pause, 3=Next
    RECT rect;
    bool isHovered;
    bool isPressed;
};
static FlyoutButton g_mediaFlyoutButtons[3] = {};

static HWINEVENTHOOK g_barHook    = nullptr;
static UINT          g_barCreated = 0;
static DWORD         g_visThreadId = 0;

// Spectrum data
static float g_spectrum [MAX_BARS] = {};
static float g_peakHold [MAX_BARS] = {};
static float g_peakTimer[MAX_BARS] = {};
static float g_beatEnergy          = 0.0f;
static float g_rainbowHue          = 0.0f;
static float g_pitchHue            = 0.0f;
static mutex g_specMutex;

// Audio ring buffer
static float        g_ringBuf[RING_SIZE] = {};
static int          g_ringWrite          = 0;
static mutex        g_ringMutex;
static atomic<bool> g_audioRunning{false};
[[clang::no_destroy]] static std::optional<std::thread> g_audioThread;
static UINT32       g_sampleRate = 44100;

// Auto-hide state
static atomic<int>  g_silenceFrames{0};
// Idle counter in seconds, incremented by IDT_IDLE (1s interval)
static atomic<int>  g_idleSeconds{0};
static bool         g_lastFsState      = false;
static int          g_fsCheckTick      = 0;
static const int    FS_CHECK_INTERVAL  = 30; // render frame

// Media state
struct MediaState {
    wstring   title       = L"";
    wstring   artist      = L"";
    bool      isPlaying   = false;
    bool      hasMedia    = false;
    Bitmap*   albumArt    = nullptr;
    Bitmap*   appIcon     = nullptr;  // Source app badge (updated only when sourceAppId changes)
    Color     dominantColor = Color(255, 0, 204, 255);
    double    positionSec = 0.0;
    double    durationSec = 0.0;
    ULONGLONG lastUpdateTick = 0;
    wstring   sourceAppId = L"";
    uint64_t  albumArtGeneration = 0;  // monotonically incremented on every art change
    bool      isPaused    = false;     // explicit pause flag (different from !isPlaying when no session)
    mutex     lock;
} g_MediaState;

static winrt::event_token g_tokenSessionsChanged;
static winrt::event_token g_tokenPlaybackInfoChanged;
static winrt::event_token g_tokenMediaPropertiesChanged;
static winrt::event_token g_tokenTimelinePropertiesChanged;
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSession g_CurrentRegisteredSession = nullptr;

static std::mutex g_mediaUpdateMutex;
static std::condition_variable g_mediaUpdateCv;
static std::atomic<bool> g_mediaUpdatePending{false};
static std::atomic<double> g_pendingSeekSec{-1.0};

static void TriggerInstantMediaUpdate() {
    g_mediaUpdatePending.store(true);
    g_mediaUpdateCv.notify_all();
}

static mutex        g_sessionMgrMutex;
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;
static atomic<bool> g_mediaRunning{false};
[[clang::no_destroy]] static std::optional<std::thread> g_mediaThread;

[[clang::no_destroy]] static std::optional<std::thread> g_visThread;
[[clang::no_destroy]] static std::optional<std::thread> g_hookThread;
static atomic<bool> g_threadFinished{false};
static atomic<bool> g_uninitRequested{false};

// Button info
struct Button {
    int  id;
    RECT rect;
    bool isHovered;
    bool isPressed;
};
static Button g_buttons[3]       = {};
static bool   g_isTrackingMouse  = false;

// Scrolling text state
static float g_scrollOffset = 0.0f; // in pixels
static int   g_scrollWait   = 90;   // initial pause (frames)

// Global flyout & particle state
static bool         g_compactMode      = false;
static bool         g_isSeeking        = false;
static float        g_seekProgress     = 0.0f;
static atomic<bool> g_lowFPSMode{false};
static atomic<bool> g_systemSuspended{false};
static atomic<bool> g_audioSuspended{false};

// Render-side smooth position interpolation (panel render thread only, no lock needed)
static double    g_smoothedPos      = 0.0;   // smoothed position for progress bar
static double    g_smoothedDur      = 0.0;   // last known duration
static ULONGLONG g_smoothedTick     = 0;     // tick when positionSec was last written
static bool      g_smoothedPlaying  = false; // last known playing state

// Render-side cached album art (panel render thread only, avoids Bitmap::Clone under lock)
static Bitmap*   g_cachedAlbumArt   = nullptr;
static uint64_t  g_cachedArtGen     = UINT64_MAX;  // generation of currently cached art

// Flyout render-side cached album art (flyout WndProc thread, separate from panel)
static Bitmap*   g_flyoutCachedAlbumArt = nullptr;
static uint64_t  g_flyoutCachedArtGen   = UINT64_MAX;

static void CalculateLayout(int W, int H, float& mediaX, float& mediaW, float& visX, float& visW, float& ctrlX, float& ctrlW);
static void WakeUpAudio(HWND hwnd);
static void RedrawMediaFlyout();
static void RedrawVolumeFlyout();

struct Particle {
    float x, y;
    float vx, vy;
    float life; // 1.0f -> 0.0f
    Color color;
};
static vector<Particle> g_particles;
static mutex            g_particleMutex;

struct AppIconCache {
    wstring aumid;
    Bitmap* iconBmp = nullptr;
};
static vector<AppIconCache> g_iconCache;
static mutex                g_iconCacheMutex;

// =========================================================================
// VOLUME NOTIFICATIONS
// =========================================================================
static void RegisterVolumeNotification(HWND hwnd) {
    IMMDeviceEnumerator* pEnum = nullptr;
    static const GUID CLSID_MMDevEnum =
        {0xBCDE0395,0xE52F,0x467C,{0x8E,0x3D,0xC4,0x57,0x92,0x91,0x69,0x2E}};
    static const GUID IID_IMMDevEnum =
        {0xA95664D2,0x9614,0x4F35,{0xA7,0x46,0xDE,0x8D,0xB6,0x36,0x17,0xE6}};
    HRESULT hr = CoCreateInstance(CLSID_MMDevEnum, nullptr, CLSCTX_ALL, IID_IMMDevEnum, (void**)&pEnum);
    if (SUCCEEDED(hr) && pEnum) {
        IMMDevice* pDev = nullptr;
        hr = pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDev);
        if (SUCCEEDED(hr) && pDev) {
            hr = pDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&g_endpointVolume);
            if (SUCCEEDED(hr) && g_endpointVolume) {
                g_volumeCallback = new VolumeCallback();
                g_endpointVolume->RegisterControlChangeNotify(g_volumeCallback);
                
                // Capture initial values
                float currentVol = 0.0f;
                BOOL currentMuted = FALSE;
                g_endpointVolume->GetMasterVolumeLevelScalar(&currentVol);
                g_endpointVolume->GetMute(&currentMuted);
                g_volumeLevel.store((int)(currentVol * 100.0f + 0.5f));
                g_volumeMuted.store(currentMuted != FALSE);
            }
            pDev->Release();
        }
        pEnum->Release();
    }
}

static void UnregisterVolumeNotification() {
    if (g_endpointVolume && g_volumeCallback) {
        g_endpointVolume->UnregisterControlChangeNotify(g_volumeCallback);
        g_volumeCallback->Release();
        g_volumeCallback = nullptr;
    }
    if (g_endpointVolume) {
        g_endpointVolume->Release();
        g_endpointVolume = nullptr;
    }
}

// =========================================================================
// HELPER FUNCTIONS
// =========================================================================
static void AddRoundedRectToPath(GraphicsPath& path, float x, float y,
                                  float w, float h, float r) {
    float d = r * 2.0f;
    if (d > w) d = w;
    if (d > h) d = h;
    RectF rc(x, y, d, d);
    path.AddArc(rc, 180.0f, 90.0f);
    rc.X = x + w - d;
    path.AddArc(rc, 270.0f, 90.0f);
    rc.Y = y + h - d;
    path.AddArc(rc, 0.0f, 90.0f);
    rc.X = x;
    path.AddArc(rc, 90.0f, 90.0f);
    path.CloseFigure();
}

static Color HsvToColor(float h, float s, float v, BYTE a = 255) {
    h = fmodf(h, 360.0f);
    if (h < 0) h += 360.0f;
    float r, g, b;
    if (s < 0.0001f) {
        r = g = b = v;
    } else {
        float hh = h / 60.0f;
        int   i  = (int)hh;
        float f  = hh - i;
        float p  = v * (1.0f - s);
        float q  = v * (1.0f - s * f);
        float t  = v * (1.0f - s * (1.0f - f));
        switch (i % 6) {
            case 0: r=v; g=t; b=p; break;
            case 1: r=q; g=v; b=p; break;
            case 2: r=p; g=v; b=t; break;
            case 3: r=p; g=q; b=v; break;
            case 4: r=t; g=p; b=v; break;
            default:r=v; g=p; b=q; break;
        }
    }
    return Color(a, (BYTE)(r*255), (BYTE)(g*255), (BYTE)(b*255));
}

static Color LerpColor(DWORD ca, DWORD cb, float t) {
    t = max(0.0f, min(1.0f, t));
    int r1=(ca>>16)&0xFF, g1=(ca>>8)&0xFF, b1=ca&0xFF;
    int r2=(cb>>16)&0xFF, g2=(cb>>8)&0xFF, b2=cb&0xFF;
    return Color(255,
        (BYTE)(r1+(r2-r1)*t), (BYTE)(g1+(g2-g1)*t), (BYTE)(b1+(b2-b1)*t));
}

static Color DWORDtoColor(DWORD d, BYTE a = 255) {
    return Color(a, (d>>16)&0xFF, (d>>8)&0xFF, d&0xFF);
}

static std::atomic<bool> g_cachedLightMode{false};

static void RefreshSystemLightMode() {
    DWORD value = 0; DWORD size = sizeof(value);
    bool light = false;
    if (RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        light = value != 0;
    }
    g_cachedLightMode.store(light, memory_order_relaxed);
}

static bool IsSystemLightMode() {
    return g_cachedLightMode.load(memory_order_relaxed);
}

static Color ExtractVibrantColor(Bitmap* bmp) {
    if (!bmp) return Color(255, 0, 204, 255);
    UINT w = bmp->GetWidth();
    UINT h = bmp->GetHeight();
    if (w == 0 || h == 0) return Color(255, 0, 204, 255);

    struct ColorSample {
        BYTE r, g, b;
        float s, v;
    };
    vector<ColorSample> samples;
    samples.reserve(25);

    for (int y = 1; y <= 5; y++) {
        for (int x = 1; x <= 5; x++) {
            Color c;
            if (bmp->GetPixel(w * x / 6, h * y / 6, &c) == Ok) {
                BYTE r = c.GetR();
                BYTE g = c.GetG();
                BYTE b = c.GetB();
                
                float rf = r / 255.0f;
                float gf = g / 255.0f;
                float bf = b / 255.0f;
                float mx = max({rf, gf, bf});
                float mn = min({rf, gf, bf});
                float df = mx - mn;
                float s = (mx == 0.0f) ? 0.0f : (df / mx);
                float v = mx;

                samples.push_back({r, g, b, s, v});
            }
        }
    }

    if (samples.empty()) return Color(255, 0, 204, 255);

    ColorSample bestSample = samples[0];
    float bestScore = -1.0f;
    for (auto const& s : samples) {
        if (s.v < 0.15f || s.v > 0.95f || s.s < 0.15f) continue;
        float score = s.s * s.v;
        if (score > bestScore) {
            bestScore = score;
            bestSample = s;
        }
    }

    if (bestScore < 0.05f) {
        unsigned long long rSum = 0, gSum = 0, bSum = 0;
        for (auto const& s : samples) {
            rSum += s.r; gSum += s.g; bSum += s.b;
        }
        return Color(255, (BYTE)(rSum / samples.size()), (BYTE)(gSum / samples.size()), (BYTE)(bSum / samples.size()));
    }

    return Color(255, bestSample.r, bestSample.g, bestSample.b);
}

static Color AdjustColorForTheme(Color c, bool lightMode) {
    float r = c.GetR() / 255.0f;
    float g = c.GetG() / 255.0f;
    float b = c.GetB() / 255.0f;
    float mx = max({r, g, b});
    float mn = min({r, g, b});
    float df = mx - mn;
    float h = 0.0f;
    if (df > 0.0f) {
        if (mx == r) h = fmodf((g - b) / df, 6.0f);
        else if (mx == g) h = (b - r) / df + 2.0f;
        else h = (r - g) / df + 4.0f;
        h *= 60.0f;
        if (h < 0.0f) h += 360.0f;
    }
    float s = (mx == 0.0f) ? 0.0f : (df / mx);
    float v = mx;

    if (lightMode) {
        v = min(v, 0.7f);
        s = max(s, 0.6f);
    } else {
        v = max(v, 0.85f);
        s = max(s, 0.5f);
    }
    return HsvToColor(h, s, v, c.GetA());
}

static Color GetBarColor(int barIdx, float val, BYTE alpha = 255) {
    Color c;
    bool lightMode = g_Settings.autoTheme && IsSystemLightMode();
    switch (g_Settings.colorMode) {
        case 0: c = LerpColor(g_Settings.color2, g_Settings.color1, val); break;
        case 1: {
            float hue = fmodf(g_rainbowHue + barIdx * (360.0f / max(1, g_Settings.barCount)), 360.0f);
            c = HsvToColor(hue, 1.0f, 0.85f + val * 0.15f);
            break;
        }
        case 2: c = DWORDtoColor(g_Settings.color1); break;
        case 3: {
            float beat = min(g_beatEnergy * 2.5f, 1.0f);
            c = HsvToColor(220.0f - beat * 220.0f, 0.8f + beat * 0.2f, 0.6f + val * 0.4f);
            break;
        }
        case 4: { // Adaptif
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                c = g_MediaState.dominantColor;
            }
            c = AdjustColorForTheme(c, lightMode);
            break;
        }
        case 5: { // Pitch-Reaktif
            c = HsvToColor(g_pitchHue, 1.0f, 0.85f + val * 0.15f);
            break;
        }
        default: c = Color(255, 0, 200, 255);
    }
    return Color(alpha, c.GetR(), c.GetG(), c.GetB());
}

static bool IsFullscreenActive() {
    // 1. SHQueryUserNotificationState check
    QUERY_USER_NOTIFICATION_STATE quns = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns))) {
        if (quns == QUNS_RUNNING_D3D_FULL_SCREEN ||
            quns == QUNS_PRESENTATION_MODE ||
            quns == QUNS_BUSY)
            return true;
    }
    
    // 2. Active Window Dimensions check (Fallback & Chrome/Game full screen fix)
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        // Exclude taskbar itself and desktop
        WCHAR className[256];
        if (GetClassNameW(hwnd, className, 256)) {
            if (wcscmp(className, L"Shell_TrayWnd") != 0 &&
                wcscmp(className, L"WorkerW") != 0 &&
                wcscmp(className, L"Progman") != 0) {
                
                RECT rcApp;
                if (GetWindowRect(hwnd, &rcApp)) {
                    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                    MONITORINFO mi = {};
                    mi.cbSize = sizeof(mi);
                    if (GetMonitorInfoW(hMonitor, &mi)) {
                        // Compare window rect with monitor rect (must match exactly for fullscreen)
                        if (rcApp.left <= mi.rcMonitor.left &&
                            rcApp.top <= mi.rcMonitor.top &&
                            rcApp.right >= mi.rcMonitor.right &&
                            rcApp.bottom >= mi.rcMonitor.bottom) {
                            
                            // Make sure it also has styles typical for full screen (not just maximized)
                            LONG style = GetWindowLongW(hwnd, GWL_STYLE);
                            if (!(style & WS_CHILD)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// =========================================================================
// DWM APPEARANCE - WITHOUT WS_EX_LAYERED
// Use SetWindowCompositionAttribute only for acrylic blur.
// =========================================================================
static void UpdateAppearance(HWND hwnd) {
    // Rounded corners (DWM)
    DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));

    // Extend frame into client area for transparency/acrylic
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // Acrylic blur - works without WS_EX_LAYERED
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (!hUser) return;

    auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
    if (!SetComp) return;

    DWORD tint;
    if (g_Settings.autoTheme) {
        tint = IsSystemLightMode() ? 0x30FFFFFF : 0x30000000;
    } else {
        tint = ((DWORD)g_Settings.bgOpacity << 24) | 0x000000;
    }

    ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
    SetComp(hwnd, &data);
}

static void UpdateFlyoutAppearance(HWND hwnd) {
    // Acrylic blur - skip DwmExtendFrameIntoClientArea on WS_EX_LAYERED windows
    // Use SetWindowCompositionAttribute only for acrylic blur.
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (!hUser) return;

    auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
    if (!SetComp) return;

    bool lightMode = g_Settings.autoTheme && IsSystemLightMode();
    // Low-alpha background tint for acrylic; GDI+ draws panel opacity
    DWORD tint = lightMode ? 0x01FFFFFF : 0x0116161A;

    ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
    SetComp(hwnd, &data);
}

static Bitmap* GetAppIconFromAUMID(const wstring& aumid) {
    if (aumid.empty()) return nullptr;
    
    lock_guard<mutex> lk(g_iconCacheMutex);
    for (auto& item : g_iconCache) {
        if (item.aumid == aumid) return item.iconBmp;
    }
    
    wstring procName = L"";
    if (aumid.find(L".exe") != wstring::npos) {
        size_t pos = aumid.rfind(L"\\");
        if (pos == wstring::npos) pos = aumid.rfind(L"!");
        if (pos == wstring::npos) {
            procName = aumid;
        } else {
            procName = aumid.substr(pos + 1);
        }
        size_t bang = procName.find(L"!");
        if (bang != wstring::npos) procName = procName.substr(0, bang);
    } else {
        wstring lower = aumid;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
        if (lower.find(L"spotify") != wstring::npos) procName = L"Spotify.exe";
        else if (lower.find(L"chrome") != wstring::npos) procName = L"chrome.exe";
        else if (lower.find(L"edge") != wstring::npos) procName = L"msedge.exe";
        else if (lower.find(L"firefox") != wstring::npos) procName = L"firefox.exe";
        else if (lower.find(L"opera") != wstring::npos) procName = L"opera.exe";
        else if (lower.find(L"vlc") != wstring::npos) procName = L"vlc.exe";
        else {
            procName = aumid + L".exe";
        }
    }
    
    HICON hIcon = nullptr;
    wstring path = L"";
    
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe = {};
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, procName.c_str()) == 0) {
                    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                    if (hProc) {
                        wchar_t imgPath[MAX_PATH] = {};
                        DWORD size = MAX_PATH;
                        if (QueryFullProcessImageNameW(hProc, 0, imgPath, &size)) {
                            path = imgPath;
                        }
                        CloseHandle(hProc);
                    }
                    break;
                }
            } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }
    
    if (!path.empty()) {
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON)) {
            hIcon = sfi.hIcon;
        }
    }
    
    Bitmap* bmp = nullptr;
    if (hIcon) {
        bmp = Bitmap::FromHICON(hIcon);
        DestroyIcon(hIcon);
    }
    
    g_iconCache.push_back({ aumid, bmp });
    return bmp;
}

static void ClearAppIconCache() {
    lock_guard<mutex> lk(g_iconCacheMutex);
    for (auto& item : g_iconCache) {
        if (item.iconBmp) delete item.iconBmp;
    }
    g_iconCache.clear();
}

static RECT GetProgressBarRect(HWND hwnd) {
    RECT clientRc; GetClientRect(hwnd, &clientRc);
    int W = clientRc.right, H = clientRc.bottom;
    
    float mediaX, mediaW, visX, visW, ctrlX, ctrlW;
    CalculateLayout(W, H, mediaX, mediaW, visX, visW, ctrlX, ctrlW);
    
    RECT pbRect = {};
    if (g_Settings.showMediaInfo && mediaW > 0.0f) {
        float margin = (float)g_Settings.panelMargin;
        float pad = (float)g_Settings.panelPadding;
        float containerY = margin;
        float containerH = (float)H - margin * 2.0f;
        float availH = containerH;
        
        float artSize = (float)g_Settings.mediaArtSize;
        if (artSize <= 0.0f) {
            artSize = availH - pad * 2.0f;
        }
        if (artSize > availH - pad * 2.0f) {
            artSize = availH - pad * 2.0f;
        }
        
        float textX = mediaX + artSize + (float)g_Settings.panelTextGap + g_Settings.textOffsetX;
        float textW = mediaX + mediaW - textX;
        float pbH = (float)g_Settings.progressBarHeight;
        float pbY = containerY + availH - pad - pbH;
        
        pbRect.left = (long)textX;
        pbRect.top = (long)(pbY - 6.0f);
        pbRect.right = (long)(textX + textW);
        pbRect.bottom = (long)(pbY + pbH + 6.0f);
    }
    return pbRect;
}

static void WakeUpAudio(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    if (g_systemSuspended.load(memory_order_relaxed)) return;
    
    g_silenceFrames.store(0, memory_order_relaxed);
    
    bool wasSuspended = g_audioSuspended.exchange(false, memory_order_relaxed);
    if (wasSuspended) {
        Wh_Log(L"Audio awakened from dynamic sleep.");
    }
    
    if (g_lowFPSMode.exchange(false, memory_order_relaxed)) {
        int interval = max(16, 1000 / max(1, g_Settings.targetFPS));
        SetTimer(hwnd, IDT_RENDER, interval, nullptr);
    }
}

static void SeekToPosition(double seconds) {
    if (seconds < 0.0) seconds = 0.0;
    g_pendingSeekSec.store(seconds, memory_order_relaxed);
    TriggerInstantMediaUpdate();
}


static void UpdateAndDrawParticles(Graphics& g, float visualizerX, float visualizerW, float yStart, float hVis, int bars, float barW, const float* spec, float spc) {
    lock_guard<mutex> lk(g_particleMutex);
    
    for (auto it = g_particles.begin(); it != g_particles.end(); ) {
        it->x += it->vx;
        it->y += it->vy;
        it->vy += 0.15f;
        it->life -= 0.025f;
        
        if (it->life <= 0.0f || it->y > yStart + hVis) {
            it = g_particles.erase(it);
        } else {
            SolidBrush pb(Color((BYTE)(it->life * 255), it->color.GetR(), it->color.GetG(), it->color.GetB()));
            float size = 1.5f + it->life * 2.0f;
            g.FillEllipse(&pb, it->x - size/2.0f, it->y - size/2.0f, size, size);
            ++it;
        }
    }
    
    static float lastSpec[MAX_BARS] = {};
    for (int b = 0; b < bars; b++) {
        float val = spec[b];
        float diff = val - lastSpec[b];
        if (diff > 0.15f && g_particles.size() < 120) {
            float x = visualizerX + spc + b * (barW + spc) + barW * 0.5f;
            float y = yStart + hVis - val * hVis;
            
            int spawnCount = min(2, (int)(diff * 6.0f));
            for (int p = 0; p < spawnCount; p++) {
                float vx = ((float)(rand() % 100) / 100.0f - 0.5f) * 1.2f;
                float vy = -((float)(rand() % 100) / 100.0f) * 2.0f - 0.5f;
                g_particles.push_back({
                    x, y, vx, vy, 1.0f, GetBarColor(b, val)
                });
            }
        }
        lastSpec[b] = val;
    }
}

// =========================================================================
// FFT (Cooley-Tukey iterative, Radix-2)
// =========================================================================
static void fft_inplace(complex<float>* a, int n) {
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        const float ang = -(float)M_PI * 2.0f / len;
        const complex<float> wlen(cosf(ang), sinf(ang));
        for (int i = 0; i < n; i += len) {
            complex<float> w(1.0f, 0.0f);
            for (int j = 0; j < len / 2; j++) {
                complex<float> u = a[i + j];
                complex<float> v = a[i + j + len / 2] * w;
                a[i + j]         = u + v;
                a[i + j + len/2] = u - v;
                w *= wlen;
            }
        }
    }
}

// =========================================================================
// WASAPI LOOPBACK AUDIO CAPTURE
// =========================================================================
static void AudioCaptureThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    static const GUID CLSID_MMDevEnum =
        {0xBCDE0395,0xE52F,0x467C,{0x8E,0x3D,0xC4,0x57,0x92,0x91,0x69,0x2E}};
    static const GUID IID_IMMDevEnum =
        {0xA95664D2,0x9614,0x4F35,{0xA7,0x46,0xDE,0x8D,0xB6,0x36,0x17,0xE6}};
    static const GUID IID_IAudioClient =
        {0x1CB9AD4C,0xDBFA,0x4c32,{0xB1,0x78,0xC2,0xF5,0x68,0xA7,0x03,0xB2}};
    static const GUID IID_IAudioCapture =
        {0xC8ADBD64,0xE71E,0x48a0,{0xA4,0xDE,0x18,0x5C,0x39,0x5C,0xD3,0x17}};
    static const GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT =
        {0x00000003,0x0000,0x0010,{0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};

    IMMDeviceEnumerator* pEnum = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_MMDevEnum, nullptr, CLSCTX_ALL,
                                  IID_IMMDevEnum, (void**)&pEnum);
    if (FAILED(hr)) {
        CoUninitialize();
        return;
    }

    int activeMode = -1;

    while (g_audioRunning.load(memory_order_relaxed)) {
        int targetMode = g_Settings.captureMode;
        if (activeMode != targetMode) {
            activeMode = targetMode;

            IMMDevice*           pDev  = nullptr;
            IAudioClient*        pAC   = nullptr;
            IAudioCaptureClient* pCC   = nullptr;
            WAVEFORMATEX*        pwfx  = nullptr;

            bool captureEntered = false;
            int retrySleepMs = 100;

            EDataFlow dataFlow = (activeMode == 1) ? eCapture : eRender;
            DWORD streamFlags = (activeMode == 1) ? 0 : AUDCLNT_STREAMFLAGS_LOOPBACK;

            hr = pEnum->GetDefaultAudioEndpoint(dataFlow, eConsole, &pDev);
            if (FAILED(hr)) goto loop_cleanup;

            hr = pDev->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pAC);
            if (FAILED(hr)) goto loop_cleanup;

            hr = pAC->GetMixFormat(&pwfx);
            if (FAILED(hr)) goto loop_cleanup;

            g_sampleRate = pwfx->nSamplesPerSec;

            hr = pAC->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                 streamFlags,
                                 5000000LL, 0, pwfx, nullptr);
            if (FAILED(hr)) goto loop_cleanup;

            hr = pAC->GetService(IID_IAudioCapture, (void**)&pCC);
            if (FAILED(hr)) goto loop_cleanup;

            hr = pAC->Start();
            if (FAILED(hr)) goto loop_cleanup;

            captureEntered = true;

            {
                bool isFloat = false, isPCM16 = false, isPCM32 = false;
                int  chCount = pwfx->nChannels;

                if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
                    isFloat = true;
                } else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
                    auto* ext = (WAVEFORMATEXTENSIBLE*)pwfx;
                    if (IsEqualGUID(ext->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
                        isFloat = true;
                    else {
                        isPCM16 = (ext->Format.wBitsPerSample == 16);
                        isPCM32 = (ext->Format.wBitsPerSample == 32);
                    }
                } else if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
                    isPCM16 = (pwfx->wBitsPerSample == 16);
                }

                const int   ch  = min(chCount, 2);
                const float chF = (float)max(ch, 1);

                int consecutiveErrors = 0;
                while (g_audioRunning.load(memory_order_relaxed) && g_Settings.captureMode == activeMode) {
                    if (g_audioSuspended.load(memory_order_relaxed)) {
                        pAC->Stop();
                        while (g_audioSuspended.load(memory_order_relaxed) && g_audioRunning.load(memory_order_relaxed)) {
                            Sleep(100);
                        }
                        if (g_audioRunning.load(memory_order_relaxed)) {
                            pAC->Start();
                        }
                        continue;
                    }

                    UINT32 packetSize = 0;
                    HRESULT hrGet = pCC->GetNextPacketSize(&packetSize);
                    if (FAILED(hrGet)) {
                        consecutiveErrors++;
                        if (hrGet == AUDCLNT_E_DEVICE_INVALIDATED || consecutiveErrors > 10) {
                            Wh_Log(L"Audio client invalidated or too many errors. Resetting capture thread.");
                            activeMode = -1; // Force re-initialization
                            break;
                        }
                        Sleep(10);
                        continue;
                    }
                    consecutiveErrors = 0;

                    if (packetSize == 0) {
                        Sleep(5);
                        continue;
                    }

                    BYTE*  pData  = nullptr;
                    UINT32 frames = 0;
                    DWORD  flags  = 0;
                    HRESULT hrBuf = pCC->GetBuffer(&pData, &frames, &flags, nullptr, nullptr);
                    if (FAILED(hrBuf)) {
                        if (hrBuf == AUDCLNT_E_DEVICE_INVALIDATED) {
                            Wh_Log(L"Audio client buffer get failed with device invalidated. Resetting.");
                            activeMode = -1; // Force re-initialization
                            break;
                        }
                        Sleep(5);
                        continue;
                    }

                    if (frames > 0) {
                        lock_guard<mutex> lk(g_ringMutex);
                        for (UINT32 i = 0; i < frames; i++) {
                            float s = 0.0f;
                            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData) {
                                if (isFloat) {
                                    float* fp = (float*)pData + (size_t)i * chCount;
                                    for (int c = 0; c < ch; c++) s += fp[c];
                                } else if (isPCM16) {
                                    short* sp = (short*)pData + (size_t)i * chCount;
                                    for (int c = 0; c < ch; c++) s += sp[c] / 32768.0f;
                                } else if (isPCM32) {
                                    int* ip = (int*)pData + (size_t)i * chCount;
                                    for (int c = 0; c < ch; c++) s += ip[c] / 2147483648.0f;
                                }
                                s /= chF;
                                if (std::abs(s) > 0.005f && g_lowFPSMode.load(memory_order_relaxed)) {
                                    if (g_hVisWnd && IsWindow(g_hVisWnd)) {
                                        PostMessageW(g_hVisWnd, WM_APP + 25, 0, 0);
                                    }
                                }
                            }
                            g_ringBuf[g_ringWrite] = s;
                            g_ringWrite = (g_ringWrite + 1) & (RING_SIZE - 1);
                        }
                    }
                    pCC->ReleaseBuffer(frames);
                }
            }

        loop_cleanup:
            if (!captureEntered) {
                activeMode = -1;
                retrySleepMs = 500;
            }
            if (pAC)  pAC->Stop();
            if (pCC)  { pCC->Release(); }
            if (pAC)  { pAC->Release(); }
            if (pwfx) { CoTaskMemFree(pwfx); }
            if (pDev) { pDev->Release(); }
            for (int slept = 0; slept < retrySleepMs && g_audioRunning.load(memory_order_relaxed); slept += 100) {
                Sleep(100);
            }
            continue;
        }
        Sleep(100);
    }

    if (pEnum) { pEnum->Release(); }
    CoUninitialize();
}

// =========================================================================
// SPECTRUM PROCESSING
// =========================================================================
static void ProcessSpectrum() {
    static complex<float> buf[FFT_SIZE];
    {
        lock_guard<mutex> lk(g_ringMutex);
        const int rw       = g_ringWrite;
        const int startIdx = (rw - FFT_SIZE + RING_SIZE) & (RING_SIZE - 1);
        for (int i = 0; i < FFT_SIZE; i++)
            buf[i] = complex<float>(g_ringBuf[(startIdx + i) & (RING_SIZE - 1)], 0.0f);
    }

    // Hann window
    for (int i = 0; i < FFT_SIZE; i++) {
        const float w = 0.5f * (1.0f - cosf((float)(2.0 * M_PI) * i / (FFT_SIZE - 1)));
        buf[i] *= w;
    }

    fft_inplace(buf, FFT_SIZE);

    const int halfN = FFT_SIZE / 2;
    static float mag[FFT_SIZE / 2];
    memset(mag, 0, sizeof(mag));
    for (int i = 1; i < halfN; i++)
        mag[i] = std::abs(buf[i]) * 2.0f / FFT_SIZE;

    const int   bars   = g_Settings.barCount;
    const float sr     = (float)g_sampleRate;
    const float fMin   = (float)max(g_Settings.freqMin, 1);
    const float fMax   = (float)min(g_Settings.freqMax, (int)(sr / 2));
    const float logMin = log10f(fMin);
    const float logMax = log10f(fMax);
    const float sens   = g_Settings.sensitivity / 100.0f;
    const float smooth = g_Settings.smoothing   / 100.0f;

    float newSpec[MAX_BARS] = {};
    for (int b = 0; b < bars; b++) {
        const float t1 = (float) b      / bars;
        const float t2 = (float)(b + 1) / bars;
        int bin1 = (int)(powf(10.0f, logMin + (logMax - logMin) * t1) * FFT_SIZE / sr);
        int bin2 = (int)(powf(10.0f, logMin + (logMax - logMin) * t2) * FFT_SIZE / sr);
        bin1 = max(bin1, 1);
        bin2 = min(bin2, halfN - 1);
        if (bin2 < bin1) bin2 = bin1;

        float sum = 0.0f;
        for (int k = bin1; k <= bin2; k++) sum += mag[k];
        const float raw = (sum / (bin2 - bin1 + 1)) * sens;
        newSpec[b] = min((raw > 0.0f) ? log10f(1.0f + raw * 9.0f) : 0.0f, 1.0f);
    }

    float beatSum  = 0.0f;
    int   beatBars = max(1, bars / 6);
    float maxVal   = 0.0f;

    {
        lock_guard<mutex> lk(g_specMutex);
        const float peakDrop = g_Settings.peakFallSpeed * 0.005f;

        for (int b = 0; b < bars; b++) {
            const float prev = g_spectrum[b];
            const float next = newSpec[b];
            g_spectrum[b] = (next > prev)
                ? prev * (smooth * 0.5f) + next * (1.0f - smooth * 0.5f)
                : prev * smooth          + next * (1.0f - smooth);
            g_spectrum[b] = min(g_spectrum[b], 1.0f);

            if (g_spectrum[b] > maxVal) maxVal = g_spectrum[b];

            if (g_Settings.showPeaks) {
                if (g_spectrum[b] >= g_peakHold[b]) {
                    g_peakHold[b]  = g_spectrum[b];
                    g_peakTimer[b] = 45.0f;
                } else if (g_peakTimer[b] > 0) {
                    g_peakTimer[b]--;
                } else {
                    g_peakHold[b] -= peakDrop;
                    if (g_peakHold[b] < 0.0f) g_peakHold[b] = 0.0f;
                }
            }
            if (b < beatBars) beatSum += g_spectrum[b];
        }

        const float newBeat = beatSum / beatBars;
        g_beatEnergy = (newBeat > g_beatEnergy)
            ? g_beatEnergy * 0.3f + newBeat * 0.7f
            : g_beatEnergy * 0.92f + newBeat * 0.08f;

        g_rainbowHue += 0.8f + g_beatEnergy * 3.0f;
        if (g_rainbowHue >= 360.0f) g_rainbowHue -= 360.0f;

        // Pitch detection
        int maxBin = 3;
        float maxMag = 0.0f;
        for (int i = 3; i < halfN - 1; i++) {
            if (mag[i] > maxMag) {
                maxMag = mag[i];
                maxBin = i;
            }
        }
        if (maxMag > 0.0001f) {
            float peakFreq = maxBin * sr / FFT_SIZE;
            float logFreq = log10f(max(20.0f, peakFreq));
            float norm = (logFreq - 1.8f) / (3.8f - 1.8f);
            norm = max(0.0f, min(1.0f, norm));
            float targetHue = norm * 300.0f;
            g_pitchHue = g_pitchHue * 0.85f + targetHue * 0.15f;
        }
    }

    if (maxVal < 0.02f)
        g_silenceFrames.fetch_add(1, memory_order_relaxed);
    else
        g_silenceFrames.store(0, memory_order_relaxed);
}

// =========================================================================
// MEDIA INFO UPDATE
// =========================================================================
static Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    IStream* nativeStream = nullptr;
    HRESULT hr = CreateStreamOverRandomAccessStream(
        reinterpret_cast<IUnknown*>(winrt::get_abi(stream)),
        IID_PPV_ARGS(&nativeStream));
    if (SUCCEEDED(hr) && nativeStream) {
        Bitmap* tempBmp = Bitmap::FromStream(nativeStream);
        Bitmap* bmp = nullptr;
        if (tempBmp && tempBmp->GetLastStatus() == Ok) {
            UINT w = tempBmp->GetWidth();
            UINT h = tempBmp->GetHeight();
            if (w > 0 && h > 0) {
                bmp = new Bitmap(w, h, PixelFormat32bppARGB);
                if (bmp && bmp->GetLastStatus() == Ok) {
                    Graphics g(bmp);
                    g.DrawImage(tempBmp, 0.0f, 0.0f, (REAL)w, (REAL)h);
                } else {
                    delete bmp;
                    bmp = nullptr;
                }
            }
        }
        delete tempBmp;
        nativeStream->Release();
        return bmp;
    }
    return nullptr;
}

static void RegisterManagerEvents(GlobalSystemMediaTransportControlsSessionManager const& manager) {
    try {
        if (!manager) return;
        g_tokenSessionsChanged = manager.SessionsChanged([](auto const&, auto const&) {
            TriggerInstantMediaUpdate();
        });
    } catch (...) {}
}

static void RegisterSessionEvents(GlobalSystemMediaTransportControlsSession const& session) {
    try {
        if (!session) return;
        
        if (g_CurrentRegisteredSession) {
            try {
                g_CurrentRegisteredSession.PlaybackInfoChanged(g_tokenPlaybackInfoChanged);
                g_CurrentRegisteredSession.MediaPropertiesChanged(g_tokenMediaPropertiesChanged);
                g_CurrentRegisteredSession.TimelinePropertiesChanged(g_tokenTimelinePropertiesChanged);
            } catch (...) {}
        }
        
        g_CurrentRegisteredSession = session;
        
        g_tokenPlaybackInfoChanged = session.PlaybackInfoChanged([](auto const&, auto const&) {
            TriggerInstantMediaUpdate();
        });
        
        g_tokenMediaPropertiesChanged = session.MediaPropertiesChanged([](auto const&, auto const&) {
            TriggerInstantMediaUpdate();
        });
        
        g_tokenTimelinePropertiesChanged = session.TimelinePropertiesChanged([](auto const&, auto const&) {
            TriggerInstantMediaUpdate();
        });
    } catch (...) {}
}

static void UpdateMediaInfo() {
    // Static debounce / state guards — only on the media update thread
    static int     s_noSessionCount      = 0;
    static int     s_errorCount          = 0;
    static wstring s_lastTitle           = L"";
    static wstring s_lastSourceAppId     = L"";  // deduplicate icon fetches at the source
    static int     s_albumArtFetchAttempts = 0;

    try {
        {
            lock_guard<mutex> lk(g_sessionMgrMutex);
            if (!g_SessionManager) {
                g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
                if (g_SessionManager) {
                    RegisterManagerEvents(g_SessionManager);
                }
            }
            if (!g_SessionManager) return;
        }

        GlobalSystemMediaTransportControlsSession session = nullptr;
        bool foundActive = false;

        GlobalSystemMediaTransportControlsSessionManager mgr = nullptr;
        {
            lock_guard<mutex> lk(g_sessionMgrMutex);
            mgr = g_SessionManager;
        }

        auto sessionsList = mgr.GetSessions();
        for (auto const& s : sessionsList) {
            try {
                auto pb = s.GetPlaybackInfo();
                if (pb && pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                    session    = s;
                    foundActive = true;
                    break;
                }
            } catch (...) { continue; }
        }

        if (!foundActive) {
            session = mgr.GetCurrentSession();
            if (!session) {
                // Fallback: check if our currently registered session is still in the sessions list,
                // or pick the first available session, so we don't drop the session on transient OS glitches.
                if (g_CurrentRegisteredSession) {
                    wstring currentAumid = L"";
                    try {
                        auto h = g_CurrentRegisteredSession.SourceAppUserModelId();
                        currentAumid = h.empty() ? L"" : wstring(h.c_str());
                    } catch (...) {}
                    if (!currentAumid.empty()) {
                        for (auto const& s : sessionsList) {
                            try {
                                auto h = s.SourceAppUserModelId();
                                wstring sAumid = h.empty() ? L"" : wstring(h.c_str());
                                if (sAumid == currentAumid) {
                                    session = s;
                                    break;
                                }
                            } catch (...) {}
                        }
                    }
                }
                if (!session) {
                    for (auto const& s : sessionsList) {
                        session = s;
                        break; // Fallback to first available
                    }
                }
            }
        }

        if (session) {
            s_noSessionCount = 0;
            s_errorCount = 0;

            if (g_CurrentRegisteredSession != session) {
                RegisterSessionEvents(session);
            }

            GlobalSystemMediaTransportControlsSessionMediaProperties props = nullptr;
            try {
                props = session.TryGetMediaPropertiesAsync().get();
            } catch (...) {}

            auto info  = session.GetPlaybackInfo();

            // ------ Fetch basic metadata ------
            wstring newTitle, newArtist, newSourceAppId;
            if (props) {
                try { auto h = props.Title();  newTitle       = h.empty() ? L"" : wstring(h.c_str()); } catch (...) {}
                try { auto h = props.Artist(); newArtist      = h.empty() ? L"" : wstring(h.c_str()); } catch (...) {}
            } else {
                // Transient COM error: retain last known title/artist to prevent flickering to empty
                newTitle = s_lastTitle;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    newArtist = g_MediaState.artist;
                }
            }
            try { auto h = session.SourceAppUserModelId(); newSourceAppId = h.empty() ? L"" : wstring(h.c_str()); } catch (...) {}

            // ---- Stabilize sourceAppId: never accept a blank id if we already have one ----
            if (newSourceAppId.empty() && !s_lastSourceAppId.empty()) {
                newSourceAppId = s_lastSourceAppId;
            }

            // ------ Resolve app icon (once per actual source change) ------
            if (newSourceAppId != s_lastSourceAppId) {
                s_lastSourceAppId = newSourceAppId;
                Bitmap* newIcon = newSourceAppId.empty() ? nullptr : GetAppIconFromAUMID(newSourceAppId);
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    g_MediaState.appIcon     = newIcon;
                    g_MediaState.sourceAppId = newSourceAppId;
                }
            }

            // ------ Playback state ------
            bool isPlaying = false;
            try {
                isPlaying = (info && info.PlaybackStatus() ==
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            } catch (...) {}

            // ------ Timeline (position) — only fetch when we really need it ------
            // Reading it every 500ms is fine. But we ONLY write it to MediaState if:
            //   a) The title changed (song switch), OR
            //   b) The position changed by more than 1.5s from our estimated value (seek)
            //   c) Play/pause state changed
            double newPos  = 0.0;
            double newDur  = 0.0;
            try {
                auto timeline = session.GetTimelineProperties();
                if (timeline) {
                    newPos = timeline.Position().count() / 10000000.0;
                    newDur = timeline.EndTime().count() / 10000000.0;
                }
            } catch (...) {}

            // ------ Title change detection ------
            bool titleChanged = (newTitle != s_lastTitle);
            if (titleChanged) {
                s_lastTitle = newTitle;
                s_albumArtFetchAttempts = 0;
            }

            bool hasAlbumArt = false;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                hasAlbumArt = (g_MediaState.albumArt != nullptr);
            }

            // Retry fetching album art up to 3 times if it failed initially
            bool needAlbumArt = (!hasAlbumArt && !newTitle.empty() && s_albumArtFetchAttempts < 3);

            if (titleChanged || needAlbumArt) {
                // ---- SLOW PATH: title changed or album art missing ----
                s_albumArtFetchAttempts++;
                Bitmap* newAlbumArt = nullptr;
                try {
                    auto thumbRef = props.Thumbnail();
                    if (thumbRef) {
                        auto stream = thumbRef.OpenReadAsync().get();
                        newAlbumArt = StreamToBitmap(stream);
                    }
                } catch (...) { newAlbumArt = nullptr; }

                Color newDomColor;
                if (newAlbumArt) {
                    newDomColor = ExtractVibrantColor(newAlbumArt);
                } else {
                    newDomColor = DWORDtoColor(g_Settings.color1);
                }

                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    if (g_MediaState.albumArt) {
                        delete g_MediaState.albumArt;
                    }
                    g_MediaState.albumArt      = newAlbumArt;
                    g_MediaState.albumArtGeneration++;  // signal render thread that art changed
                    // appIcon already updated above (via s_lastSourceAppId check)
                    g_MediaState.dominantColor = newDomColor;
                    g_MediaState.title         = newTitle;
                    g_MediaState.artist        = newArtist;
                    g_MediaState.isPlaying     = isPlaying;
                    g_MediaState.hasMedia      = true;
                    g_MediaState.positionSec   = newPos;
                    g_MediaState.durationSec   = newDur;
                    g_MediaState.lastUpdateTick = GetTickCount64();
                }

                g_scrollOffset = 0.0f;
                g_scrollWait   = 90;

                if (titleChanged && !newTitle.empty()) {
                    if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
                        PostMessageW(g_hMediaFlyoutWnd, WM_APP + 35, 0, 0);
                    }
                }
            } else {
                // ---- FAST PATH: only update playback state and position if meaningful ----
                lock_guard<mutex> guard(g_MediaState.lock);

                bool wasPlaying = g_MediaState.isPlaying;
                g_MediaState.isPlaying = isPlaying;
                g_MediaState.durationSec = newDur;

                // Compute what our interpolated position would be right now
                double estimatedPos = g_MediaState.positionSec;
                if (wasPlaying && g_MediaState.lastUpdateTick > 0) {
                    estimatedPos += (GetTickCount64() - g_MediaState.lastUpdateTick) / 1000.0;
                }
                if (estimatedPos > g_MediaState.durationSec) estimatedPos = g_MediaState.durationSec;

                // Only accept new position from GSMTC if it differs by more than 2s (seek detection)
                // or if play/pause state changed, or if our estimate is too far off
                double posDiff = newPos - estimatedPos;
                bool posJumped = (posDiff > 2.5 || posDiff < -2.5);
                bool stateChanged = (isPlaying != wasPlaying);

                if (posJumped || stateChanged || newPos < 0.1) {
                    // Accept the new position (seek, pause, or start)
                    g_MediaState.positionSec    = newPos;
                    g_MediaState.lastUpdateTick = GetTickCount64();
                } else if (isPlaying) {
                    // Keep interpolating — just refresh the tick so we don't drift
                    // Recalibrate every ~5s to prevent long-term drift
                    double elapsed = (GetTickCount64() - g_MediaState.lastUpdateTick) / 1000.0;
                    if (elapsed >= 5.0) {
                        // Accept GSMTC position as calibration point
                        g_MediaState.positionSec    = newPos;
                        g_MediaState.lastUpdateTick = GetTickCount64();
                    }
                    // else: do nothing — interpolation continues smoothly
                } else {
                    // Paused: accept GSMTC position (it's authoritative when paused)
                    g_MediaState.positionSec    = newPos;
                    g_MediaState.lastUpdateTick = GetTickCount64();
                }

                g_MediaState.hasMedia  = true;
                // Note: sourceAppId and appIcon already handled above via s_lastSourceAppId
            }

            if (isPlaying && g_hVisWnd && IsWindow(g_hVisWnd)) {
                PostMessageW(g_hVisWnd, WM_APP + 25, 0, 0);
            }

            if (g_hVisWnd && IsWindow(g_hVisWnd)) {
                InvalidateRect(g_hVisWnd, nullptr, FALSE);
            }
            if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
                PostMessageW(g_hMediaFlyoutWnd, WM_APP + 36, 0, 0);
            }
        } else {
            s_noSessionCount++;
            if (s_noSessionCount >= 3) { // Debounce session loss
                s_noSessionCount = 0;
                if (g_CurrentRegisteredSession) {
                    try {
                        g_CurrentRegisteredSession.PlaybackInfoChanged(g_tokenPlaybackInfoChanged);
                        g_CurrentRegisteredSession.MediaPropertiesChanged(g_tokenMediaPropertiesChanged);
                        g_CurrentRegisteredSession.TimelinePropertiesChanged(g_tokenTimelinePropertiesChanged);
                    } catch (...) {}
                    g_CurrentRegisteredSession = nullptr;
                }

                // Reset statics so next session starts fresh
                s_lastTitle       = L"";
                s_lastSourceAppId = L"";
                s_albumArtFetchAttempts = 0;

                bool hadMedia = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    hadMedia = g_MediaState.hasMedia;
                    if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
                    g_MediaState.albumArtGeneration++;  // invalidate render-side art cache
                    g_MediaState.appIcon     = nullptr;
                    g_MediaState.sourceAppId = L"";
                    g_MediaState.title       = L"";
                    g_MediaState.artist      = L"";
                    g_MediaState.isPlaying   = false;
                    g_MediaState.hasMedia    = false;
                    g_MediaState.dominantColor = DWORDtoColor(g_Settings.color1);
                    g_MediaState.positionSec   = 0.0;
                    g_MediaState.durationSec   = 0.0;
                    g_MediaState.lastUpdateTick = 0;
                }

                if (hadMedia) {
                    if (g_hVisWnd && IsWindow(g_hVisWnd)) {
                        InvalidateRect(g_hVisWnd, nullptr, FALSE);
                    }
                    if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
                        PostMessageW(g_hMediaFlyoutWnd, WM_APP + 36, 0, 0);
                    }
                }
            }
        }
    } catch (...) {
        s_errorCount++;
        if (s_errorCount >= 3) { // Debounce transient RPC/COM errors
            s_errorCount = 0;
            s_lastTitle       = L"";
            s_lastSourceAppId = L"";
            s_albumArtFetchAttempts = 0;
            if (g_CurrentRegisteredSession) {
                try {
                    g_CurrentRegisteredSession.PlaybackInfoChanged(g_tokenPlaybackInfoChanged);
                    g_CurrentRegisteredSession.MediaPropertiesChanged(g_tokenMediaPropertiesChanged);
                    g_CurrentRegisteredSession.TimelinePropertiesChanged(g_tokenTimelinePropertiesChanged);
                } catch (...) {}
                g_CurrentRegisteredSession = nullptr;
            }
            lock_guard<mutex> guard(g_MediaState.lock);
            if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
            g_MediaState.appIcon     = nullptr;
            g_MediaState.sourceAppId = L"";
            g_MediaState.hasMedia    = false;
            g_MediaState.isPlaying   = false;
            g_MediaState.dominantColor = DWORDtoColor(g_Settings.color1);
            g_MediaState.positionSec   = 0.0;
            g_MediaState.durationSec   = 0.0;
            g_MediaState.lastUpdateTick = 0;

            if (g_hVisWnd && IsWindow(g_hVisWnd)) {
                InvalidateRect(g_hVisWnd, nullptr, FALSE);
            }
            if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
                PostMessageW(g_hMediaFlyoutWnd, WM_APP + 36, 0, 0);
            }
        }
    }
}


static void MediaUpdateThread() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    while (g_mediaRunning.load(memory_order_relaxed)) {
        double seekSec = g_pendingSeekSec.exchange(-1.0, memory_order_relaxed);
        if (seekSec >= 0.0) {
            try {
                GlobalSystemMediaTransportControlsSessionManager mgr = nullptr;
                {
                    lock_guard<mutex> lk(g_sessionMgrMutex);
                    mgr = g_SessionManager;
                }
                if (mgr) {
                    auto session = mgr.GetCurrentSession();
                    if (session) {
                        long long ticks = (long long)(seekSec * 10000000.0);
                        session.TryChangePlaybackPositionAsync(ticks);
                    }
                }
            } catch (...) {}
        }

        UpdateMediaInfo();
        
        std::unique_lock<std::mutex> lk(g_mediaUpdateMutex);
        g_mediaUpdateCv.wait_for(lk, std::chrono::milliseconds(500), []() {
            return g_mediaUpdatePending.exchange(false) || !g_mediaRunning.load(memory_order_relaxed);
        });
    }

    try {
        if (g_SessionManager) {
            g_SessionManager.SessionsChanged(g_tokenSessionsChanged);
        }
        if (g_CurrentRegisteredSession) {
            g_CurrentRegisteredSession.PlaybackInfoChanged(g_tokenPlaybackInfoChanged);
            g_CurrentRegisteredSession.MediaPropertiesChanged(g_tokenMediaPropertiesChanged);
            g_CurrentRegisteredSession.TimelinePropertiesChanged(g_tokenTimelinePropertiesChanged);
        }
    } catch (...) {}
    g_CurrentRegisteredSession = nullptr;

    {
        lock_guard<mutex> lk(g_sessionMgrMutex);
        g_SessionManager = nullptr;
    }
    winrt::uninit_apartment();
}

static void SendMediaCommand(int cmd) {
    try {
        GlobalSystemMediaTransportControlsSessionManager mgr = nullptr;
        {
            lock_guard<mutex> lk(g_sessionMgrMutex);
            mgr = g_SessionManager;
        }
        if (!mgr) return;
        auto session = mgr.GetCurrentSession();
        if (!session) return;

        if      (cmd == 1) session.TrySkipPreviousAsync();
        else if (cmd == 3) session.TrySkipNextAsync();
        else if (cmd == 2) session.TryTogglePlayPauseAsync();
    } catch (...) {}
}

static void CalculateLayout(int W, int H, float& mediaX, float& mediaW, float& visX, float& visW, float& ctrlX, float& ctrlW) {
    const float margin = (float)g_Settings.panelMargin; // window edge margin
    const float pad = (float)g_Settings.panelPadding;    // container inner padding
    const float gap = (float)g_Settings.panelGap;    // gap between sections
    
    float containerW = (float)W - margin * 2.0f;
    float availW = containerW - pad * 2.0f;
    if (availW < 0.0f) availW = 0.0f;
    
    mediaX = 0.0f; mediaW = 0.0f;
    visX = 0.0f; visW = 0.0f;
    ctrlX = 0.0f; ctrlW = 0.0f;
    
    bool hasMediaInfo = g_Settings.showMediaInfo;
    bool hasVisualizer = g_Settings.showVisualizer && !g_compactMode;
    bool hasControls = g_Settings.showMediaControls;
    
    float btnW = (float)g_Settings.panelBtnSize;
    float minCtrlW = btnW * 2.0f + 10.0f;
    if (hasControls && availW < minCtrlW) {
        hasControls = false;
    }
    
    int activeSections = 0;
    if (hasMediaInfo) activeSections++;
    if (hasVisualizer) activeSections++;
    if (hasControls) activeSections++;
    
    float remainingW = availW;
    if (activeSections > 1) {
        remainingW -= (activeSections - 1) * gap;
    }
    if (remainingW < 0.0f) remainingW = 0.0f;
    
    if (hasControls) {
        ctrlW = btnW * 3.0f + 18.0f;
        if (remainingW < ctrlW) {
            ctrlW = remainingW;
        }
        if (ctrlW < minCtrlW) {
            ctrlW = 0.0f;
            hasControls = false;
            activeSections = 0;
            if (hasMediaInfo) activeSections++;
            if (hasVisualizer) activeSections++;
            remainingW = availW;
            if (activeSections > 1) {
                remainingW -= (activeSections - 1) * gap;
            }
            if (remainingW < 0.0f) remainingW = 0.0f;
        } else {
            remainingW -= ctrlW;
        }
    }
    
    if (hasMediaInfo && hasVisualizer) {
        float ratio = g_Settings.visWidthPercent / 100.0f;
        visW = remainingW * ratio;
        mediaW = remainingW - visW;
        
        float minMediaW = 100.0f;
        float minVisW = 80.0f;
        
        if (remainingW < minMediaW + minVisW) {
            mediaW = remainingW * 0.50f;
            visW = remainingW - mediaW;
        } else {
            if (mediaW < minMediaW) {
                mediaW = minMediaW;
                visW = remainingW - mediaW;
            }
            if (visW < minVisW) {
                visW = minVisW;
                mediaW = remainingW - visW;
            }
        }
    } else if (hasMediaInfo) {
        mediaW = remainingW;
    } else if (hasVisualizer) {
        visW = remainingW;
    }
    
    if (mediaW < 0.0f) mediaW = 0.0f;
    if (visW < 0.0f) visW = 0.0f;
    if (ctrlW < 0.0f) ctrlW = 0.0f;
    
    float curX = margin + pad;
    if (hasMediaInfo && mediaW > 0.0f) {
        mediaX = curX;
        curX += mediaW + gap;
    }
    if (hasControls && ctrlW > 0.0f) {
        ctrlX = curX;
        curX += ctrlW + gap;
    }
    if (hasVisualizer && visW > 0.0f) {
        visX = curX;
    }
}

static void UpdateButtonRects(int W, int H) {
    float mediaX, mediaW, visX, visW, ctrlX, ctrlW;
    CalculateLayout(W, H, mediaX, mediaW, visX, visW, ctrlX, ctrlW);
    
    float btnW = (float)g_Settings.panelBtnSize;
    float minCtrlW = btnW * 2.0f + 10.0f;
    if (g_Settings.showMediaControls && ctrlW >= minCtrlW) {
        float drawBtnW = btnW;
        const float btnH = btnW;
        if (ctrlW < btnW * 3.0f + 12.0f) {
            drawBtnW = ctrlW / 4.0f;
        }
        const float spacing = (ctrlW - (drawBtnW * 3.0f)) / 4.0f;
        const float startY = (H - btnH) / 2.0f;
        
        g_buttons[0] = { 1, { (long)(ctrlX + spacing),                  (long)startY, (long)(ctrlX + spacing + drawBtnW),                  (long)(startY + btnH) }, false, false };
        g_buttons[1] = { 2, { (long)(ctrlX + spacing * 2.0f + drawBtnW),        (long)startY, (long)(ctrlX + spacing * 2.0f + drawBtnW * 2.0f),        (long)(startY + btnH) }, false, false };
        g_buttons[2] = { 3, { (long)(ctrlX + spacing * 3.0f + drawBtnW * 2.0f),  (long)startY, (long)(ctrlX + spacing * 3.0f + drawBtnW * 3.0f),  (long)(startY + btnH) }, false, false };
    } else {
        memset(g_buttons, 0, sizeof(g_buttons));
    }
}

static void DrawButton(Graphics& g, const Button& btn, Color color) {
    if (btn.rect.right - btn.rect.left <= 0) return;
    SolidBrush brush(color);
    float cx = btn.rect.left + (btn.rect.right  - btn.rect.left) / 2.0f;
    float cy = btn.rect.top  + (btn.rect.bottom - btn.rect.top)  / 2.0f;
    float btnW = (float)(btn.rect.right - btn.rect.left);
    float btnH = (float)(btn.rect.bottom - btn.rect.top);
    float scale = btnW / 22.0f;

    if (btn.isPressed) {
        SolidBrush bg(Color(60, color.GetR(), color.GetG(), color.GetB()));
        g.FillEllipse(&bg, (float)btn.rect.left, (float)btn.rect.top, btnW, btnH);
        Pen border(Color(100, color.GetR(), color.GetG(), color.GetB()), 1.0f);
        g.DrawEllipse(&border, (float)btn.rect.left, (float)btn.rect.top, btnW, btnH);
    } else if (btn.isHovered) {
        SolidBrush bg(Color(30, color.GetR(), color.GetG(), color.GetB()));
        g.FillEllipse(&bg, (float)btn.rect.left, (float)btn.rect.top, btnW, btnH);
        Pen border(Color(50, color.GetR(), color.GetG(), color.GetB()), 1.0f);
        g.DrawEllipse(&border, (float)btn.rect.left, (float)btn.rect.top, btnW, btnH);
    }

    if (btn.id == 1) { // Prev - two triangles + bar
        PointF t1[] = { { cx - 1.0f * scale, cy }, { cx + 5.0f * scale, cy - 5.0f * scale }, { cx + 5.0f * scale, cy + 5.0f * scale } };
        g.FillPolygon(&brush, t1, 3);
        PointF t2[] = { { cx - 7.0f * scale, cy }, { cx - 1.0f * scale, cy - 5.0f * scale }, { cx - 1.0f * scale, cy + 5.0f * scale } };
        g.FillPolygon(&brush, t2, 3);
        g.FillRectangle(&brush, cx - 9.0f * scale, cy - 5.0f * scale, 2.0f * scale, 10.0f * scale);
    }
    else if (btn.id == 2) { // Play/Pause
        bool playing = false;
        { lock_guard<mutex> guard(g_MediaState.lock); playing = g_MediaState.isPlaying; }
        if (playing) {
            g.FillRectangle(&brush, cx - 4.0f * scale, cy - 5.0f * scale, 2.5f * scale, 10.0f * scale);
            g.FillRectangle(&brush, cx + 1.5f * scale, cy - 5.0f * scale, 2.5f * scale, 10.0f * scale);
        } else {
            PointF t[] = { { cx + 6.0f * scale, cy }, { cx - 4.0f * scale, cy - 6.0f * scale }, { cx - 4.0f * scale, cy + 6.0f * scale } };
            g.FillPolygon(&brush, t, 3);
        }
    }
    else if (btn.id == 3) { // Next - two triangles + bar
        PointF t1[] = { { cx + 1.0f * scale, cy }, { cx - 5.0f * scale, cy - 5.0f * scale }, { cx - 5.0f * scale, cy + 5.0f * scale } };
        g.FillPolygon(&brush, t1, 3);
        PointF t2[] = { { cx + 7.0f * scale, cy }, { cx + 1.0f * scale, cy - 5.0f * scale }, { cx + 1.0f * scale, cy + 5.0f * scale } };
        g.FillPolygon(&brush, t2, 3);
        g.FillRectangle(&brush, cx + 7.0f * scale, cy - 5.0f * scale, 2.0f * scale, 10.0f * scale);
    }
}

static void FillUpperRoundedBar(Graphics& g, Brush* brush, float x, float y, float w, float h, float r) {
    if (h <= 0.0f || w <= 0.0f) return;
    float diameter = w;
    if (h <= diameter) {
        g.FillEllipse(brush, x, y, w, h);
    } else {
        g.FillEllipse(brush, x, y, w, diameter);
        g.FillRectangle(brush, x, y + diameter / 2.0f, w, h - diameter / 2.0f);
    }
}

static void FillLowerRoundedBar(Graphics& g, Brush* brush, float x, float y, float w, float h, float r) {
    if (h <= 0.0f || w <= 0.0f) return;
    float diameter = w;
    if (h <= diameter) {
        g.FillEllipse(brush, x, y, w, h);
    } else {
        g.FillRectangle(brush, x, y, w, h - diameter / 2.0f);
        g.FillEllipse(brush, x, y + h - diameter, w, diameter);
    }
}

static void DrawPlaceholderArt(Graphics& g, float x, float y, float size, bool lightMode, Color mainColor) {
    // 1. Draw rounded card background with a premium gradient
    Color bgColTop, bgColBot;
    if (lightMode) {
        bgColTop = Color(255, 245, 247, 250);
        bgColBot = Color(255, 225, 230, 240);
    } else {
        bgColTop = Color(255, 35, 38, 48);
        bgColBot = Color(255, 20, 22, 30);
    }
    LinearGradientBrush bgBrush(PointF(x, y), PointF(x, y + size), bgColTop, bgColBot);
    
    GraphicsPath cardPath;
    float cornerRadius = size * 0.22f;
    AddRoundedRectToPath(cardPath, x, y, size, size, cornerRadius);
    g.FillPath(&bgBrush, &cardPath);
    
    // Draw an inner subtle glow/border
    Pen innerBorderPen(lightMode ? Color(40, 0, 0, 0) : Color(40, 255, 255, 255), 1.0f);
    g.DrawPath(&innerBorderPen, &cardPath);

    // 2. Draw a beautiful, modern music note in the center
    float cx = x + size / 2.0f;
    float cy = y + size / 2.0f;
    
    // Scale everything based on the size
    float scale = size / 64.0f; 
    
    // We will draw a double eighth note
    Color noteColor;
    if (lightMode) {
        noteColor = Color(220, 74, 85, 104);
    } else {
        noteColor = Color(220, 210, 225, 255);
    }
    
    // Mix in a bit of mainColor to make it adapt to the theme!
    if (mainColor.GetA() > 0) {
        noteColor = Color(220, 
            (BYTE)(noteColor.GetR() * 0.5f + mainColor.GetR() * 0.5f),
            (BYTE)(noteColor.GetG() * 0.5f + mainColor.GetG() * 0.5f),
            (BYTE)(noteColor.GetB() * 0.5f + mainColor.GetB() * 0.5f)
        );
    }

    SolidBrush noteBrush(noteColor);
    SolidBrush shadowBrush(lightMode ? Color(30, 0, 0, 0) : Color(20, 0, 0, 0));
    
    // Draw twice: first shadow, then the actual note
    for (int step = 0; step < 2; step++) {
        float ox = (step == 0) ? 1.0f * scale : 0.0f;
        float oy = (step == 0) ? 2.0f * scale : 0.0f;
        Brush* currentBrush = (step == 0) ? (Brush*)&shadowBrush : (Brush*)&noteBrush;
        
        // Note heads (inclined ellipses)
        // Left head
        g.FillEllipse(currentBrush, cx - 14.0f * scale + ox, cy + 5.0f * scale + oy, 11.0f * scale, 8.0f * scale);
        // Right head
        g.FillEllipse(currentBrush, cx + 3.0f * scale + ox, cy + 0.0f * scale + oy, 11.0f * scale, 8.0f * scale);
        
        // Stems
        Pen stemPen(currentBrush, 2.2f * scale);
        stemPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
        // Left stem
        g.DrawLine(&stemPen, cx - 4.0f * scale + ox, cy + 8.0f * scale + oy, cx - 4.0f * scale + ox, cy - 14.0f * scale + oy);
        // Right stem
        g.DrawLine(&stemPen, cx + 13.0f * scale + ox, cy + 3.0f * scale + oy, cx + 13.0f * scale + ox, cy - 19.0f * scale + oy);
        
        // Connecting beam
        Pen beamPen(currentBrush, 4.5f * scale);
        beamPen.SetLineJoin(LineJoinRound);
        g.DrawLine(&beamPen, cx - 4.0f * scale + ox, cy - 12.0f * scale + oy, cx + 13.0f * scale + ox, cy - 17.0f * scale + oy);
    }
}

static void DrawPanel(HDC hdc, int W, int H) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);

    bool lightMode = g_Settings.autoTheme && IsSystemLightMode();
    Color mainColor = lightMode ? Color(255, 10, 10, 10) : Color(255, 245, 245, 245);

    // Container bounds (outer capsule holding all modules)
    const float margin = (float)g_Settings.panelMargin; // Window outer margin
    const float pad = (float)g_Settings.panelPadding;    // Capsule inner padding
    const float gap = (float)g_Settings.panelGap;    // Module gap

    float containerX = margin;
    float containerY = margin;
    float containerW = (float)W - margin * 2.0f;
    float containerH = (float)H - margin * 2.0f;

    GraphicsPath containerPath;
    AddRoundedRectToPath(containerPath, containerX, containerY, containerW, containerH, (float)g_Settings.panelCornerRadius);

    // Semi-transparent glass fill matching acrylic effect
    Color tintColor = lightMode
        ? Color(210, 255, 255, 255) // light mode frosted white
        : Color(160, 22, 22, 26);     // dark mode dark translucent gray
    // Dynamic Ambient Glow (Aura)
    if (g_Settings.showMediaInfo) {
        float beatVal = 0.0f;
        {
            lock_guard<mutex> lk(g_specMutex);
            beatVal = g_beatEnergy;
        }
        float glowMargin = 16.0f + beatVal * 8.0f;
        GraphicsPath glowPath;
        float gx = containerX - glowMargin;
        float gy = containerY - glowMargin;
        float gw = containerW + glowMargin * 2.0f;
        float gh = containerH + glowMargin * 2.0f;
        float gr = (float)g_Settings.panelCornerRadius + glowMargin;
        AddRoundedRectToPath(glowPath, gx, gy, gw, gh, gr);
        
        PathGradientBrush pgb(&glowPath);
        Color centerCol;
        {
            lock_guard<mutex> guard(g_MediaState.lock);
            centerCol = g_MediaState.dominantColor;
        }
        BYTE glowAlpha = (BYTE)(45 + beatVal * 35);
        pgb.SetCenterColor(Color(glowAlpha, centerCol.GetR(), centerCol.GetG(), centerCol.GetB()));
        
        int boundaryCount = glowPath.GetPointCount();
        if (boundaryCount > 0) {
            vector<Color> surroundColors(boundaryCount, Color(0, centerCol.GetR(), centerCol.GetG(), centerCol.GetB()));
            pgb.SetSurroundColors(surroundColors.data(), &boundaryCount);
            g.FillPath(&pgb, &glowPath);
        }
    }

    SolidBrush containerBrush(tintColor);
    g.FillPath(&containerBrush, &containerPath);

    float beat = 0.0f;
    {
        lock_guard<mutex> lk(g_specMutex);
        beat = g_beatEnergy;
    }
    
    // Ambient Beat Pulse border calculations
    float borderThickness = (float)g_Settings.panelBorderThickness + beat * 1.5f;
    BYTE baseAlpha = lightMode ? 45 : 55;
    BYTE targetAlpha = (BYTE)(baseAlpha + beat * (lightMode ? 60 : 100));
    
    Color borderColor = lightMode
        ? Color(45, 0, 0, 0)
        : Color(55, 255, 255, 255);
        
    Color pulseBorderCol;
    if (g_Settings.colorMode == 4) { // Adaptif
        Color domColor;
        {
            lock_guard<mutex> guard(g_MediaState.lock);
            domColor = g_MediaState.dominantColor;
        }
        Color adapted = AdjustColorForTheme(domColor, lightMode);
        pulseBorderCol = Color(targetAlpha, 
                               (BYTE)(borderColor.GetR() + (adapted.GetR() - borderColor.GetR()) * beat * 0.5f),
                               (BYTE)(borderColor.GetG() + (adapted.GetG() - borderColor.GetG()) * beat * 0.5f),
                               (BYTE)(borderColor.GetB() + (adapted.GetB() - borderColor.GetB()) * beat * 0.5f));
    } else {
        Color mainCol = GetBarColor(0, 1.0f);
        pulseBorderCol = Color(targetAlpha,
                               (BYTE)(borderColor.GetR() + (mainCol.GetR() - borderColor.GetR()) * beat * 0.5f),
                               (BYTE)(borderColor.GetG() + (mainCol.GetG() - borderColor.GetG()) * beat * 0.5f),
                               (BYTE)(borderColor.GetB() + (mainCol.GetB() - borderColor.GetB()) * beat * 0.5f));
    }
    
    Pen containerPen(pulseBorderCol, borderThickness);
    g.DrawPath(&containerPen, &containerPath);

    float mediaX, mediaW, visX, visW, ctrlX, ctrlW;
    CalculateLayout(W, H, mediaX, mediaW, visX, visW, ctrlX, ctrlW);

    const float availH = containerH;

    // 1. MEDIA SECTION
    if (g_Settings.showMediaInfo && mediaW > 0.0f) {
        float artX = mediaX;
        float artSize = (float)g_Settings.mediaArtSize;
        if (artSize <= 0.0f) {
            artSize = availH - pad * 2.0f;
        }
        if (artSize > availH - pad * 2.0f) {
            artSize = availH - pad * 2.0f;
        }
        float artY = containerY + pad;

        // Album Art (rounded corners)
        GraphicsPath artPath;
        AddRoundedRectToPath(artPath, artX, artY, artSize, artSize, (float)g_Settings.mediaArtCornerRadius);
        
        // -------- Read MediaState under MINIMAL lock (no Bitmap ops inside lock) --------
        wstring title, artist;
        bool hasMedia = false;
        bool isPlaying = false;
        Color domColor = DWORDtoColor(g_Settings.color1);
        Bitmap* appIcon = nullptr;
        double statePos = 0.0, stateDur = 0.0;
        ULONGLONG stateTick = 0;
        uint64_t artGen = 0;
        Bitmap* rawAlbumArt = nullptr;  // non-owning, just to check if art exists

        {
            lock_guard<mutex> guard(g_MediaState.lock);
            title     = g_MediaState.title;
            artist    = g_MediaState.artist;
            hasMedia  = g_MediaState.hasMedia;
            isPlaying = g_MediaState.isPlaying;
            domColor  = g_MediaState.dominantColor;
            statePos  = g_MediaState.positionSec;
            stateDur  = g_MediaState.durationSec;
            stateTick = g_MediaState.lastUpdateTick;
            artGen    = g_MediaState.albumArtGeneration;
            appIcon   = hasMedia ? g_MediaState.appIcon : nullptr;
            rawAlbumArt = g_MediaState.albumArt;  // non-owning peek
        }

        // -------- Smooth Position Interpolation (render-side, no jumps) --------
        // Update smooth state when MediaState provides new authoritative values
        if (stateTick != g_smoothedTick || isPlaying != g_smoothedPlaying) {
            g_smoothedPos     = statePos;
            g_smoothedDur     = stateDur;
            g_smoothedTick    = stateTick;
            g_smoothedPlaying = isPlaying;
        } else {
            g_smoothedDur = stateDur;
        }
        // Interpolate forward from the last authoritative position
        double estPos = g_smoothedPos;
        if (g_smoothedPlaying && g_smoothedTick > 0) {
            estPos += (GetTickCount64() - g_smoothedTick) / 1000.0;
        }
        if (g_smoothedDur > 0.0 && estPos > g_smoothedDur) estPos = g_smoothedDur;
        if (estPos < 0.0) estPos = 0.0;
        double duration = g_smoothedDur;

        // -------- Render-Side Cached Album Art (Clone only on art change) --------
        // g_cachedAlbumArt is owned by the render thread — no lock needed for reading it
        if (artGen != g_cachedArtGen) {
            // Art changed — do Clone NOW (outside lock, but we need to do it under lock briefly)
            Bitmap* newCache = nullptr;
            if (rawAlbumArt) {
                lock_guard<mutex> guard(g_MediaState.lock);
                // Re-check: art might have changed again while we waited for the lock
                if (g_MediaState.albumArt) {
                    newCache = g_MediaState.albumArt->Clone(
                        0, 0, g_MediaState.albumArt->GetWidth(), g_MediaState.albumArt->GetHeight(),
                        g_MediaState.albumArt->GetPixelFormat());
                    if (newCache && newCache->GetLastStatus() != Ok) {
                        delete newCache;
                        newCache = nullptr;
                    }
                }
                artGen = g_MediaState.albumArtGeneration;
            }
            // Replace cached copy
            if (g_cachedAlbumArt) { delete g_cachedAlbumArt; }
            g_cachedAlbumArt = newCache;
            g_cachedArtGen   = artGen;
        }
        Bitmap* albumArtCopy = g_cachedAlbumArt;  // render-thread owned, valid until next art change


        g.SetClip(&artPath);
        if (hasMedia && albumArtCopy) {
            g.DrawImage(albumArtCopy, artX, artY, artSize, artSize);
        } else {
            DrawPlaceholderArt(g, artX, artY, artSize, lightMode, GetBarColor(0, 1.0f));
        }
        g.ResetClip();
        // Note: albumArtCopy = g_cachedAlbumArt, owned by render thread — do NOT delete here

        // Thin subtle border around album art
        Pen artPen(lightMode ? Color(35, 0, 0, 0) : Color(45, 255, 255, 255), 1.0f);
        g.DrawPath(&artPen, &artPath);

        // Source App Badge Overlay — appIcon was already read from MediaState in the minimal lock above
        if (appIcon) {

            float badgeSize = (float)g_Settings.appIconSize;
            float badgePad  = 2.0f;
            float badgeX = artX + artSize - badgeSize - badgePad;
            float badgeY = artY + artSize - badgeSize - badgePad;

            // Badge background pill
            GraphicsPath badgeBgPath;
            float bgR = badgeSize * 0.35f;
            AddRoundedRectToPath(badgeBgPath, badgeX - 2.0f, badgeY - 2.0f, badgeSize + 4.0f, badgeSize + 4.0f, bgR);
            // subtle shadow
            SolidBrush shadowBrush2(Color(60, 0, 0, 0));
            GraphicsPath shadowPath;
            AddRoundedRectToPath(shadowPath, badgeX - 1.0f, badgeY + 1.0f, badgeSize + 4.0f, badgeSize + 4.0f, bgR);
            g.FillPath(&shadowBrush2, &shadowPath);
            // background
            Color badgeBgCol = lightMode ? Color(235, 250, 250, 252) : Color(235, 18, 18, 22);
            SolidBrush badgeBgBrush(badgeBgCol);
            g.FillPath(&badgeBgBrush, &badgeBgPath);
            // thin border
            Pen badgeBorderPen(lightMode ? Color(60, 0, 0, 0) : Color(80, 255, 255, 255), 0.75f);
            g.DrawPath(&badgeBorderPen, &badgeBgPath);

            // Icon
            ImageAttributes attr;
            ColorMatrix matrix = { {
                { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.92f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
            } };
            attr.SetColorMatrix(&matrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

            g.DrawImage(appIcon,
                        RectF(badgeX, badgeY, badgeSize, badgeSize),
                        0.0f, 0.0f, (REAL)appIcon->GetWidth(), (REAL)appIcon->GetHeight(),
                        UnitPixel, &attr);
        }

        // Track text and progress bar
        float textX = artX + artSize + (float)g_Settings.panelTextGap;
        float textW = mediaX + mediaW - textX;

        g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        FontFamily fontFamily(FONT_NAME);
        FontFamily fallbackFamily(L"Segoe UI");
        const FontFamily* activeFamily = nullptr;
        if (fontFamily.GetLastStatus() == Ok) {
            activeFamily = &fontFamily;
        } else if (fallbackFamily.GetLastStatus() == Ok) {
            activeFamily = &fallbackFamily;
        } else {
            activeFamily = FontFamily::GenericSansSerif();
        }

        if (activeFamily && textW > 20.0f) {
            float titlePt  = (float)g_Settings.titleFontSize;
            float artistPt = (float)g_Settings.artistFontSize;

            int titleStyle = g_Settings.titleFontBold ? FontStyleBold : FontStyleRegular;
            int artistStyle = g_Settings.artistFontItalic ? FontStyleItalic : FontStyleRegular;

            Font titleFont (activeFamily, titlePt,  titleStyle,  UnitPoint);
            Font artistFont(activeFamily, artistPt, artistStyle, UnitPoint);

            Color tColor = mainColor;
            Color aColor = Color(180, mainColor.GetR(), mainColor.GetG(), mainColor.GetB());

            if (g_Settings.useCustomTextColors) {
                tColor = DWORDtoColor(g_Settings.titleColor);
                aColor = DWORDtoColor(g_Settings.artistColor);
            }

            SolidBrush textBrush  (tColor);
            SolidBrush artistBrush(aColor);
            SolidBrush shadowBrush(lightMode ? Color(120, 255, 255, 255) : Color(140, 0, 0, 0));

            StringFormat format(StringFormat::GenericTypographic());
            format.SetAlignment(StringAlignmentNear);
            format.SetLineAlignment(StringAlignmentNear);
            format.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsNoClip);
            format.SetTrimming(StringTrimmingEllipsisCharacter);

            StringFormat scrollFormat(StringFormat::GenericTypographic());
            scrollFormat.SetAlignment(StringAlignmentNear);
            scrollFormat.SetLineAlignment(StringAlignmentNear);
            scrollFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsNoClip);

            wstring displayTitle  = hasMedia ? title  : L"No Media Playing";
            wstring displayArtist = hasMedia ? artist : L"";

            // Measure text width
            RectF measureRect(0.0f, 0.0f, 4000.0f, availH);
            RectF boundRect;
            if (!displayTitle.empty()) {
                g.MeasureString(displayTitle.c_str(), -1, &titleFont, measureRect, &format, &boundRect);
            }
            
            RectF artistBoundRect;
            if (!displayArtist.empty()) {
                g.MeasureString(displayArtist.c_str(), -1, &artistFont, measureRect, &format, &artistBoundRect);
            }

            bool needsScroll = (boundRect.Width > textW && !displayTitle.empty());
            
            // Progress bar size and position
            bool hasProgress = (duration > 0.0 && hasMedia) || g_isSeeking;
            float pbH = (float)g_Settings.progressBarHeight;
            float pbY = containerY + availH - pad - pbH;
            float textH = hasProgress ? (availH - pad * 2.0f - pbH - (float)g_Settings.panelPbGap) : (availH - pad * 2.0f);
            float textY = containerY + pad + g_Settings.textOffsetY;

            Region clipRgn(RectF(textX, textY, textW, textH + 2.0f));
            g.SetClip(&clipRgn);

            if (displayArtist.empty()) {
                float drawY = textY + (textH - boundRect.Height) / 2.0f;
                if (drawY < textY) drawY = textY;
                if (needsScroll) {
                    float drawX = textX - g_scrollOffset;
                    float gapPad = (float)g_Settings.scrollPadding;
                    if (g_Settings.showTextShadow) {
                        g.DrawString(displayTitle.c_str(), -1, &titleFont, PointF(drawX + 1.0f, drawY + 1.0f), &scrollFormat, &shadowBrush);
                        g.DrawString(displayTitle.c_str(), -1, &titleFont, PointF(drawX + boundRect.Width + gapPad + 1.0f, drawY + 1.0f), &scrollFormat, &shadowBrush);
                    }
                    g.DrawString(displayTitle.c_str(), -1, &titleFont, PointF(drawX, drawY), &scrollFormat, &textBrush);
                    g.DrawString(displayTitle.c_str(), -1, &titleFont,
                                 PointF(drawX + boundRect.Width + gapPad, drawY), &scrollFormat, &textBrush);
                } else {
                    RectF layoutRect(textX, drawY, textW, boundRect.Height + 5.0f);
                    if (g_Settings.showTextShadow) {
                        RectF shadowRect(textX + 1.0f, drawY + 1.0f, textW, boundRect.Height + 5.0f);
                        g.DrawString(displayTitle.c_str(), -1, &titleFont, shadowRect, &format, &shadowBrush);
                    }
                    g.DrawString(displayTitle.c_str(), -1, &titleFont, layoutRect, &format, &textBrush);
                }
            } else {
                float gapBetweenLines = (float)g_Settings.lineGap;
                float totalTextHeight = boundRect.Height + artistBoundRect.Height + gapBetweenLines;
                float drawY = textY + (textH - totalTextHeight) / 2.0f;
                if (drawY < textY) drawY = textY;
                
                // Draw Title (Row 1)
                float titleY = drawY;
                if (needsScroll) {
                    float drawX = textX - g_scrollOffset;
                    float gapPad = (float)g_Settings.scrollPadding;
                    if (g_Settings.showTextShadow) {
                        g.DrawString(displayTitle.c_str(), -1, &titleFont, PointF(drawX + 1.0f, titleY + 1.0f), &scrollFormat, &shadowBrush);
                        g.DrawString(displayTitle.c_str(), -1, &titleFont, PointF(drawX + boundRect.Width + gapPad + 1.0f, titleY + 1.0f), &scrollFormat, &shadowBrush);
                    }
                    g.DrawString(displayTitle.c_str(), -1, &titleFont, PointF(drawX, titleY), &scrollFormat, &textBrush);
                    g.DrawString(displayTitle.c_str(), -1, &titleFont,
                                 PointF(drawX + boundRect.Width + gapPad, titleY), &scrollFormat, &textBrush);
                } else {
                    RectF titleRect(textX, titleY, textW, boundRect.Height + 5.0f);
                    if (g_Settings.showTextShadow) {
                        RectF shadowRect(textX + 1.0f, titleY + 1.0f, textW, boundRect.Height + 5.0f);
                        g.DrawString(displayTitle.c_str(), -1, &titleFont, shadowRect, &format, &shadowBrush);
                    }
                    g.DrawString(displayTitle.c_str(), -1, &titleFont, titleRect, &format, &textBrush);
                }

                // Draw Artist (Row 2)
                float artistY = titleY + boundRect.Height + gapBetweenLines;
                RectF artistRect(textX, artistY, textW, artistBoundRect.Height + 5.0f);
                if (g_Settings.showTextShadow) {
                    RectF artistShadowRect(textX + 1.0f, artistY + 1.0f, textW, artistBoundRect.Height + 5.0f);
                    g.DrawString(displayArtist.c_str(), -1, &artistFont, artistShadowRect, &format, &shadowBrush);
                }
                g.DrawString(displayArtist.c_str(), -1, &artistFont, artistRect, &format, &artistBrush);
            }

            g.ResetClip();

            // Progress bar draw
            if (hasProgress) {
                float progressRatio = 0.0f;
                if (g_isSeeking) {
                    progressRatio = g_seekProgress;
                } else if (duration > 0.0) {
                    progressRatio = (float)(estPos / duration);
                }
                if (progressRatio > 1.0f) progressRatio = 1.0f;
                if (progressRatio < 0.0f) progressRatio = 0.0f;

                float fillW = textW * progressRatio;

                // Track Background
                GraphicsPath pbTrackPath;
                AddRoundedRectToPath(pbTrackPath, textX, pbY, textW, pbH, pbH / 2.0f);
                SolidBrush trackBg(lightMode ? Color(40, 0, 0, 0) : Color(40, 255, 255, 255));
                g.FillPath(&trackBg, &pbTrackPath);

                // Fill
                if (fillW > 1.0f) {
                    GraphicsPath pbFillPath;
                    AddRoundedRectToPath(pbFillPath, textX, pbY, fillW, pbH, pbH / 2.0f);
                    Color fillCol = (g_Settings.colorMode == 4) ? AdjustColorForTheme(domColor, lightMode) : GetBarColor(0, 1.0f);
                    SolidBrush fillBr(fillCol);
                    g.FillPath(&fillBr, &pbFillPath);
                }
            }

            // Scrolling text timing
            if (needsScroll) {
                if (g_scrollWait > 0) {
                    g_scrollWait--;
                } else {
                    g_scrollOffset += g_Settings.scrollSpeed * 0.4f;
                    float gapPad = (float)g_Settings.scrollPadding;
                    if (g_scrollOffset > boundRect.Width + gapPad) {
                        g_scrollOffset = 0.0f;
                        g_scrollWait = 80;
                    }
                }
            } else {
                g_scrollOffset = 0.0f;
                g_scrollWait = 90;
            }
        }
    }

    // 2. VISUALIZER SECTION
    if (g_Settings.showVisualizer && visW > 0.0f) {
        float visualizerX = visX + 1.0f;
        float visualizerW = visW - 2.0f;
        float yStart = containerY + pad;
        float hVis = availH - pad * 2.0f;

        if (visualizerW >= 15.0f) {
            int bars = min(g_Settings.barCount, (int)(visualizerW / (float)max(1, 1 + g_Settings.barSpacing)));
            bars = max(2, min(bars, min(MAX_BARS, g_Settings.barCount)));
            const int spc = g_Settings.barSpacing;
            const float totalSp = (float)(spc * (bars + 1));
            float barW = (visualizerW - totalSp) / bars;
            if (barW < 1.0f) barW = 1.0f;

            float spec[MAX_BARS], peak[MAX_BARS];
            {
                lock_guard<mutex> lk(g_specMutex);
                memcpy(spec, g_spectrum, sizeof(float) * bars);
                memcpy(peak, g_peakHold, sizeof(float) * bars);
            }

            // ===== STYLE 0: CLASSIC BARS =====
            if (g_Settings.style == 0) {
                for (int b = 0; b < bars; b++) {
                    const float val = spec[b];
                    const float x   = visualizerX + spc + b * (barW + spc);
                    const float bh  = max(2.0f, val * hVis);
                    const float by  = yStart + hVis - bh;

                    if (g_Settings.glowEffect && bh > 2.0f) {
                        Color gc = GetBarColor(b, val, (BYTE)g_Settings.glowIntensity);
                        SolidBrush gb(gc);
                        g.FillRectangle(&gb, x - barW * 0.4f, by - 2.0f, barW * 1.8f, bh + 4.0f);
                    }
                    if (bh > 0) {
                        Color cTop = GetBarColor(b, val);
                        Color cBot = DWORDtoColor(g_Settings.color2, 200);
                        LinearGradientBrush lb(PointF(x, yStart + hVis), PointF(x, by), cBot, cTop);
                        FillUpperRoundedBar(g, &lb, x, by, barW, bh, 2.0f);
                    }
                    if (g_Settings.showPeaks && peak[b] > 0.03f) {
                        float py = yStart + hVis - peak[b] * hVis - 2.0f;
                        SolidBrush pb(GetBarColor(b, peak[b], 220));
                        g.FillRectangle(&pb, x, py, barW, 2.5f);
                    }
                }
            }

            // ===== STYLE 1: MIRROR =====
            else if (g_Settings.style == 1) {
                const float cy    = yStart + hVis / 2.0f;
                const float halfH = cy - yStart - g_Settings.mirrorGap / 2.0f;
                const float gap   = (float)g_Settings.mirrorGap;
                for (int b = 0; b < bars; b++) {
                    const float val = spec[b];
                    const float x   = visualizerX + spc + b * (barW + spc);
                    const float bh  = max(1.0f, val * halfH);
                    Color       c   = GetBarColor(b, val);
                    if (g_Settings.glowEffect) {
                        Color gc((BYTE)g_Settings.glowIntensity, c.GetR(), c.GetG(), c.GetB());
                        SolidBrush gb(gc);
                        g.FillRectangle(&gb, x-1.0f, cy-bh-2.0f, barW+2.0f, bh+2.0f);
                        g.FillRectangle(&gb, x-1.0f, cy+gap/2.0f, barW+2.0f, bh+2.0f);
                    }
                    { Color cBase = DWORDtoColor(g_Settings.color2, 180);
                      LinearGradientBrush lb(PointF(x,cy), PointF(x,cy-bh), cBase, c);
                      FillUpperRoundedBar(g, &lb, x, cy-bh, barW, bh, 1.5f); }
                    { float ay = cy + gap / 2.0f;
                      Color cBase = DWORDtoColor(g_Settings.color2, 180);
                      LinearGradientBrush lb2(PointF(x,ay), PointF(x,ay+bh), cBase, c);
                      FillLowerRoundedBar(g, &lb2, x, ay, barW, bh, 1.5f); }
                    if (g_Settings.mirrorGap > 0) {
                        SolidBrush lb(Color(50, c.GetR(), c.GetG(), c.GetB()));
                        g.FillRectangle(&lb, x, cy - gap/2.0f, barW, gap);
                    }
                    if (g_Settings.showPeaks && peak[b] > 0.03f) {
                        float ph = peak[b] * halfH;
                        SolidBrush pb(GetBarColor(b, peak[b], 220));
                        g.FillRectangle(&pb, x, cy-ph-2.0f, barW, 2.5f);
                        g.FillRectangle(&pb, x, cy+ph+gap/2.0f, barW, 2.5f);
                    }
                }
            }

            // ===== STYLE 2: BLOCKS =====
            else if (g_Settings.style == 2) {
                const int step = g_Settings.blockSize + g_Settings.blockGap;
                if (step > 0) {
                    const int maxBlocks = (int)(hVis / step);
                    for (int b = 0; b < bars; b++) {
                        const float val = spec[b];
                        const float x   = visualizerX + spc + b * (barW + spc);
                        int numBlocks   = (int)(val * hVis / step);
                        numBlocks = min(numBlocks, maxBlocks);
                        for (int blk = 0; blk < numBlocks; blk++) {
                            const float by = yStart + hVis - (blk + 1) * step;
                            if (by < yStart) break;
                            const float t  = (float)blk / max(1, maxBlocks);
                            Color bc = LerpColor(g_Settings.color2, g_Settings.color1, t);
                            if (blk == numBlocks - 1) {
                                Color bright(255, min(255,(int)bc.GetR()+50), min(255,(int)bc.GetG()+50), min(255,(int)bc.GetB()+50));
                                SolidBrush sb(bright);
                                g.FillRectangle(&sb, x, by, barW, (float)g_Settings.blockSize);
                            } else {
                                SolidBrush sb(bc);
                                g.FillRectangle(&sb, x, by, barW, (float)g_Settings.blockSize);
                            }
                        }
                        if (g_Settings.showPeaks && peak[b] > 0.03f) {
                            int   pkBlk = (int)(peak[b] * hVis / step);
                            float py    = yStart + hVis - (pkBlk + 1) * step;
                            if (py >= yStart) {
                                SolidBrush pb(GetBarColor(b, peak[b], 230));
                                g.FillRectangle(&pb, x, py, barW, (float)g_Settings.blockSize);
                            }
                        }
                    }
                }
            }

            // ===== STYLE 3: WAVE =====
            else if (g_Settings.style == 3 && bars >= 2) {
                PointF pts[MAX_BARS];
                for (int b = 0; b < bars; b++) {
                    pts[b] = PointF(
                        visualizerX + spc + b * (barW + spc) + barW * 0.5f,
                        yStart + hVis - spec[b] * (hVis - 4) - 2.0f);
                }
                {
                    PointF fp[MAX_BARS + 2]; int fpCount = 0;
                    fp[fpCount++] = PointF(pts[0].X, yStart + hVis);
                    for (int i = 0; i < bars; i++) fp[fpCount++] = pts[i];
                    fp[fpCount++] = PointF(pts[bars-1].X, yStart + hVis);
                    Color cF1 = DWORDtoColor(g_Settings.color2, (BYTE)min(200, g_Settings.waveFillAlpha));
                    Color cF2 = DWORDtoColor(g_Settings.color1, (BYTE)min(200, g_Settings.waveFillAlpha + 40));
                    LinearGradientBrush fillBr(PointF(0.0f, yStart + hVis), PointF(0.0f, yStart), cF1, cF2);
                    g.FillPolygon(&fillBr, fp, fpCount);
                }
                if (g_Settings.glowEffect) {
                    Pen gp(GetBarColor(bars/2, spec[bars/2], (BYTE)g_Settings.glowIntensity), g_Settings.waveLineW * 2.5f);
                    gp.SetLineJoin(LineJoinRound); gp.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
                    g.DrawCurve(&gp, pts, bars, 0.5f);
                }
                { Pen lp(GetBarColor(bars/2, spec[bars/2]), g_Settings.waveLineW);
                  lp.SetLineJoin(LineJoinRound); lp.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
                  g.DrawCurve(&lp, pts, bars, 0.5f); }
                if (g_Settings.showPeaks) {
                    float r = max(1.5f, barW * 0.6f);
                    for (int b = 0; b < bars; b++) {
                        if (spec[b] > 0.1f) {
                            SolidBrush pb(GetBarColor(b, spec[b], 200));
                            g.FillEllipse(&pb, pts[b].X-r*0.5f, pts[b].Y-r*0.5f, r, r);
                        }
                    }
                }
            }

            // ===== STYLE 4: OSCILLOSCOPE =====
            else if (g_Settings.style == 4) {
                const int EXTRA = 128;
                float tmp[SCOPE_N + EXTRA];
                {
                    lock_guard<mutex> lk(g_ringMutex);
                    const int rw = g_ringWrite;
                    for (int i = 0; i < EXTRA + SCOPE_N; i++) {
                        int idx = (rw - EXTRA - SCOPE_N + i + RING_SIZE) & (RING_SIZE - 1);
                        tmp[i] = g_ringBuf[idx];
                    }
                }
                int trig = EXTRA / 2;
                for (int i = EXTRA / 2; i < EXTRA - 1; i++) {
                    if (tmp[i] < 0.0f && tmp[i+1] >= 0.0f) { trig = i + 1; break; }
                }
                if (trig + SCOPE_N > SCOPE_N + EXTRA) trig = EXTRA;

                const float gain  = g_Settings.sensitivity / 100.0f * 0.8f;
                const float cy    = yStart + hVis / 2.0f;
                const float amp   = (hVis / 2.0f) * 0.85f;
                const float xStep = visualizerW / SCOPE_N;

                PointF pts[SCOPE_N];
                for (int i = 0; i < SCOPE_N; i++) {
                    int srcIdx = trig + i;
                    if (srcIdx >= SCOPE_N + EXTRA) srcIdx = SCOPE_N + EXTRA - 1;
                    float s = max(-1.0f, min(1.0f, tmp[srcIdx] * gain));
                    pts[i] = PointF(visualizerX + i * xStep, cy - s * amp);
                }
                {
                    PointF fp[SCOPE_N + 2]; int fpCount = 0;
                    fp[fpCount++] = PointF(visualizerX, cy);
                    for (int i = 0; i < SCOPE_N; i++) fp[fpCount++] = pts[i];
                    fp[fpCount++] = PointF(visualizerX + visualizerW, cy);
                    Color cF1 = DWORDtoColor(g_Settings.color2, (BYTE)min(200, g_Settings.waveFillAlpha));
                    Color cF2 = DWORDtoColor(g_Settings.color1, (BYTE)min(200, g_Settings.waveFillAlpha + 40));
                    LinearGradientBrush fillBr(PointF(0.0f, yStart + hVis), PointF(0.0f, yStart), cF1, cF2);
                    g.FillPolygon(&fillBr, fp, fpCount);
                }
                { Color rc(30, mainColor.GetR(), mainColor.GetG(), mainColor.GetB());
                  Pen rp(rc, 1.0f);
                  g.DrawLine(&rp, visualizerX, cy, visualizerX + visualizerW, cy); }
                if (g_Settings.glowEffect) {
                    Color gc = GetBarColor(SCOPE_N/2, min(g_beatEnergy*2.0f,1.0f), (BYTE)g_Settings.glowIntensity);
                    Pen gp(gc, g_Settings.waveLineW * 2.5f); gp.SetLineJoin(LineJoinRound);
                    g.DrawLines(&gp, pts, SCOPE_N);
                }
                { float midEnergy = min(g_beatEnergy * 2.0f, 1.0f);
                  Color lc = GetBarColor(SCOPE_N/2, midEnergy);
                  Pen lp(lc, g_Settings.waveLineW); lp.SetLineJoin(LineJoinRound);
                  lp.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
                  g.DrawLines(&lp, pts, SCOPE_N); }
            }

            // ===== STYLE 5: SMOOTH CURVE & FLOATING PEAKS =====
            else if (g_Settings.style == 5 && bars >= 2) {
                PointF pts[MAX_BARS];
                for (int b = 0; b < bars; b++) {
                    pts[b] = PointF(
                        visualizerX + spc + b * (barW + spc) + barW * 0.5f,
                        yStart + hVis - spec[b] * hVis);
                }
                {
                    PointF fp[MAX_BARS + 2]; int fpCount = 0;
                    fp[fpCount++] = PointF(pts[0].X, yStart + hVis);
                    for (int i = 0; i < bars; i++) fp[fpCount++] = pts[i];
                    fp[fpCount++] = PointF(pts[bars-1].X, yStart + hVis);
                    Color cF1 = DWORDtoColor(g_Settings.color2, (BYTE)min(200, g_Settings.waveFillAlpha));
                    Color cF2 = DWORDtoColor(g_Settings.color1, (BYTE)min(200, g_Settings.waveFillAlpha + 40));
                    LinearGradientBrush fillBr(PointF(0.0f, yStart + hVis), PointF(0.0f, yStart), cF1, cF2);
                    g.FillPolygon(&fillBr, fp, fpCount);
                }
                Color lc = GetBarColor(bars/2, spec[bars/2]);
                if (g_Settings.glowEffect) {
                    Color gc = GetBarColor(bars/2, spec[bars/2], (BYTE)g_Settings.glowIntensity);
                    Pen gp(gc, g_Settings.waveLineW * 2.5f); gp.SetLineJoin(LineJoinRound);
                    g.DrawCurve(&gp, pts, bars, 0.5f);
                }
                Pen lp(lc, g_Settings.waveLineW); lp.SetLineJoin(LineJoinRound);
                g.DrawCurve(&lp, pts, bars, 0.5f);

                if (g_Settings.showPeaks) {
                    float r = max(2.0f, barW * 0.8f);
                    for (int b = 0; b < bars; b++) {
                        if (peak[b] > 0.03f) {
                            float py = yStart + hVis - peak[b] * hVis;
                            SolidBrush pb(GetBarColor(b, peak[b], 220));
                            g.FillEllipse(&pb, pts[b].X-r*0.5f, py-r*0.5f, r, r);
                        }
                    }
                }
            }

            // ===== STYLE 6: SYMMETRICAL (SOUND TRAIL) =====
            else if (g_Settings.style == 6) {
                const float cy = yStart + hVis / 2.0f;
                const float maxH = hVis / 2.0f - 1.0f;
                for (int b = 0; b < bars; b++) {
                    const float val = spec[b];
                    const float x   = visualizerX + spc + b * (barW + spc);
                    const float bh  = max(1.0f, val * maxH);
                    Color       c   = GetBarColor(b, val);

                    if (g_Settings.glowEffect) {
                        Color gc((BYTE)g_Settings.glowIntensity, c.GetR(), c.GetG(), c.GetB());
                        SolidBrush gb(gc);
                        g.FillRectangle(&gb, x - barW*0.4f, cy - bh - 2.0f, barW*1.8f, bh*2.0f + 4.0f);
                    }

                    Color cBot = DWORDtoColor(g_Settings.color2, 200);
                    LinearGradientBrush lb(PointF(x, cy), PointF(x, cy - bh), cBot, c);
                    FillUpperRoundedBar(g, &lb, x, cy - bh, barW, bh, 1.5f);

                    LinearGradientBrush lb2(PointF(x, cy), PointF(x, cy + bh), cBot, c);
                    FillLowerRoundedBar(g, &lb2, x, cy, barW, bh, 1.5f);

                    if (g_Settings.showPeaks && peak[b] > 0.03f) {
                        float py1 = cy - peak[b] * maxH - 1.5f;
                        float py2 = cy + peak[b] * maxH;
                        SolidBrush pb(GetBarColor(b, peak[b], 220));
                        g.FillRectangle(&pb, x, py1, barW, 1.5f);
                        g.FillRectangle(&pb, x, py2, barW, 1.5f);
                    }
                }
            }
            
            // ===== STYLE 7: PARTICLE SPECTRUM =====
            else if (g_Settings.style == 7) {
                for (int b = 0; b < bars; b++) {
                    const float val = spec[b];
                    const float x   = visualizerX + spc + b * (barW + spc);
                    const float bh  = max(2.0f, val * hVis);
                    const float by  = yStart + hVis - bh;

                    if (g_Settings.glowEffect && bh > 2.0f) {
                        Color gc = GetBarColor(b, val, (BYTE)(g_Settings.glowIntensity * 0.7f));
                        SolidBrush gb(gc);
                        g.FillRectangle(&gb, x - barW * 0.4f, by - 2.0f, barW * 1.8f, bh + 4.0f);
                    }
                    if (bh > 0) {
                        Color cTop = GetBarColor(b, val);
                        Color cBot = DWORDtoColor(g_Settings.color2, 120);
                        LinearGradientBrush lb(PointF(x, yStart + hVis), PointF(x, by), cBot, cTop);
                        FillUpperRoundedBar(g, &lb, x, by, barW, bh, 2.0f);
                    }
                }
                UpdateAndDrawParticles(g, visualizerX, visualizerW, yStart, hVis, bars, barW, spec, spc);
            }

            if (g_Settings.style != 7) {
                lock_guard<mutex> lk(g_particleMutex);
                if (!g_particles.empty()) g_particles.clear();
            }
        }
    }

    // 3. MEDIA CONTROLS SECTION
    if (g_Settings.showMediaControls && ctrlW > 0.0f) {
        for (int i = 0; i < 3; i++) DrawButton(g, g_buttons[i], mainColor);
    }

    // Draw vertical dividers between active modules
    Pen divPen(lightMode ? Color(35, 0, 0, 0) : Color(45, 255, 255, 255), 1.0f);
    
    // Divider after Media module if Controls or Visualizer follows
    if (g_Settings.showMediaInfo && mediaW > 0.0f) {
        if ((g_Settings.showMediaControls && ctrlW > 0.0f) || (g_Settings.showVisualizer && visW > 0.0f)) {
            float div1_X = mediaX + mediaW + gap / 2.0f;
            g.DrawLine(&divPen, div1_X, containerY + (float)g_Settings.dividerPadding, div1_X, containerY + containerH - (float)g_Settings.dividerPadding);
        }
    }
    
    // Divider after Controls module if Visualizer follows
    if (g_Settings.showMediaControls && ctrlW > 0.0f) {
        if (g_Settings.showVisualizer && visW > 0.0f) {
            float div2_X = ctrlX + ctrlW + gap / 2.0f;
            g.DrawLine(&divPen, div2_X, containerY + (float)g_Settings.dividerPadding, div2_X, containerY + containerH - (float)g_Settings.dividerPadding);
        }
    }
}

// =========================================================================
// POSITION & VISIBILITY
// =========================================================================
static POINT CalculateFlyoutPosition(int flyoutW, int flyoutH, int offsetY);
static POINT CalculateMediaFlyoutPosition(int flyoutW, int flyoutH);

static void RepositionWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    HWND hBar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hBar || !IsWindowVisible(hBar)) {
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    RECT rc; GetWindowRect(hBar, &rc);
    int barW = rc.right  - rc.left;
    int barH = rc.bottom - rc.top;
    if (barW <= 0 || barH <= 0) { ShowWindow(hwnd, SW_HIDE); return; }

    int W = g_compactMode ? max(160, g_Settings.panelW / 2) : g_Settings.panelW;
    int H = g_Settings.panelH;
    int x, y;

    switch (g_Settings.position) {
        case 0:  x = rc.left + g_Settings.offsetX;              break;
        case 2:  x = rc.right - W + g_Settings.offsetX;         break;
        default: x = rc.left + (barW - W) / 2 + g_Settings.offsetX; break;
    }
    y = rc.top + (barH - H) / 2 + g_Settings.offsetY;

    RECT myRc; GetWindowRect(hwnd, &myRc);
    if (myRc.left != x || myRc.top != y ||
        (myRc.right - myRc.left) != W || (myRc.bottom - myRc.top) != H) {
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, W, H,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
    UpdateButtonRects(W, H);

    // Reposition active flyouts if the main window moves
    if (hwnd == g_hVisWnd) {
        if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd) && IsWindowVisible(g_hMediaFlyoutWnd)) {
            int flyoutW = g_Settings.mediaFlyoutW;
            int flyoutH = g_Settings.mediaFlyoutH;
            POINT pt = CalculateMediaFlyoutPosition(flyoutW, flyoutH);
            SetWindowPos(g_hMediaFlyoutWnd, HWND_TOPMOST, pt.x, pt.y, flyoutW, flyoutH, SWP_NOACTIVATE);
        }
        if (g_hVolumeFlyoutWnd && IsWindow(g_hVolumeFlyoutWnd) && IsWindowVisible(g_hVolumeFlyoutWnd)) {
            int volW = g_Settings.volumeFlyoutW;
            int volH = g_Settings.volumeFlyoutH;
            POINT pt = CalculateFlyoutPosition(volW, volH, g_Settings.volumeFlyoutOffsetY);
            SetWindowPos(g_hVolumeFlyoutWnd, HWND_TOPMOST, pt.x, pt.y, volW, volH, SWP_NOACTIVATE);
        }
    }

    WakeUpAudio(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
}

static void UpdateVisibility(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    // Fullscreen check (throttled when visible, every frame while hidden)
    bool isVisible = IsWindowVisible(hwnd) != 0;
    g_fsCheckTick++;
    if (!isVisible || g_fsCheckTick >= FS_CHECK_INTERVAL) {
        g_fsCheckTick = 0;
        g_lastFsState = g_Settings.hideOnFullscreen && IsFullscreenActive();
    }

    bool fsHide = g_lastFsState;

    // Silence auto-hide (frame-based from WASAPI)
    bool silenceHide = false;
    if (g_Settings.silenceTimeout > 0) {
        int sf = g_silenceFrames.load(memory_order_relaxed);
        // Hide after targetFPS * silenceTimeout silent frames
        silenceHide = (sf > g_Settings.silenceTimeout * max(1, g_Settings.targetFPS));
    }

    // Idle auto-hide counted per second by IDT_IDLE
    bool idleHide = false;
    if (g_Settings.idleTimeout > 0) {
        bool playing = false;
        { lock_guard<mutex> guard(g_MediaState.lock); playing = g_MediaState.isPlaying; }
        if (playing) {
            g_idleSeconds.store(0, memory_order_relaxed);
        } else {
            if (g_idleSeconds.load(memory_order_relaxed) >= g_Settings.idleTimeout)
                idleHide = true;
        }
    }

    bool shouldHide = fsHide || silenceHide || idleHide;

    // Is the taskbar hidden?
    HWND hBar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hBar || !IsWindowVisible(hBar)) shouldHide = true;

    if (shouldHide) {
        if (IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
            if (!g_lowFPSMode.load(memory_order_relaxed)) {
                g_lowFPSMode.store(true, memory_order_relaxed);
                SetTimer(hwnd, IDT_RENDER, 150, nullptr);
            }
        }
        // Force hide active flyouts on fullscreen/silence/idle
        // Keep media flyout if flyoutOnFullscreen is active and we are in fullscreen
        bool inFullscreen = IsFullscreenActive();
        bool forceHideMediaFlyout = silenceHide || idleHide || !g_Settings.showMediaFlyout ||
                                    (!g_Settings.flyoutOnFullscreen && inFullscreen) ||
                                    (!inFullscreen && (!hBar || !IsWindowVisible(hBar)));
        if (forceHideMediaFlyout && g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd) && IsWindowVisible(g_hMediaFlyoutWnd)) {
            g_mediaFlyoutAlpha = 0;
            g_mediaFlyoutState = 0;
            KillTimer(g_hMediaFlyoutWnd, TIMER_MEDIA_FADE);
            KillTimer(g_hMediaFlyoutWnd, TIMER_MEDIA_SHOW);
            ShowWindow(g_hMediaFlyoutWnd, SW_HIDE);
        }
        if (g_hVolumeFlyoutWnd && IsWindow(g_hVolumeFlyoutWnd) && IsWindowVisible(g_hVolumeFlyoutWnd)) {
            g_volumeFlyoutAlpha = 0;
            g_volumeFlyoutState = 0;
            g_volumeFlyoutIsDragging = false;
            ReleaseCapture();
            KillTimer(g_hVolumeFlyoutWnd, TIMER_VOLUME_FADE);
            KillTimer(g_hVolumeFlyoutWnd, TIMER_VOLUME_SHOW);
            ShowWindow(g_hVolumeFlyoutWnd, SW_HIDE);
        }
    }
    else if (!shouldHide && !IsWindowVisible(hwnd)) {
        RepositionWindow(hwnd);
    }
}

// =========================================================================
// TASKBAR HOOK
// =========================================================================
static void CALLBACK BarHookProc(HWINEVENTHOOK,DWORD,HWND hwnd,LONG,LONG,DWORD,DWORD) {
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Shell_TrayWnd") == 0 && g_hVisWnd && IsWindow(g_hVisWnd))
        PostMessage(g_hVisWnd, WM_APP + 20, 0, 0);
}

static void RegisterBarHook(HWND hwnd) {
    HWND hBar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hBar) {
        DWORD pid = 0, tid = GetWindowThreadProcessId(hBar, &pid);
        if (tid)
            g_barHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, BarHookProc, pid, tid,
                WINEVENT_OUTOFCONTEXT);
    }
    PostMessage(hwnd, WM_APP + 20, 0, 0);
}

// =========================================================================
// WINDOW PROCEDURE
// =========================================================================

struct MediaFlyoutLayout {
    float marginX;
    float marginY;
    float labelY;
    float labelFontSize;
    float artSize;
    float artX;
    float artY;
    float badgeSize;
    float badgeX;
    float badgeY;
    float textX;
    float textW;
    float btnSize;
    float btnY;
    float pbHeight;
    float pbY;
    float pbX;
    float pbW;
    float btnX1;
    float btnX2;
    float btnX3;
    RECT rectBtn1;
    RECT rectBtn2;
    RECT rectBtn3;
    float titleY;
    float titleH;
    float artistH;
};

static MediaFlyoutLayout CalculateMediaFlyoutLayout(int W, int H) {
    MediaFlyoutLayout l;
    l.marginX = (float)g_Settings.mediaFlyoutMarginX;
    l.marginY = (float)g_Settings.mediaFlyoutMarginY;
    l.labelY = (float)g_Settings.mediaFlyoutLabelY;
    l.labelFontSize = (float)g_Settings.mediaFlyoutLabelFontSize;
    l.artSize = (float)g_Settings.mediaFlyoutArtSize;
    if (l.artSize > H - l.marginY * 2.0f) {
        l.artSize = H - l.marginY * 2.0f;
    }
    l.artX = l.marginX;
    l.artY = (H - l.artSize) / 2.0f;
    if (l.artY < l.marginY) l.artY = l.marginY;

    l.badgeSize = (float)g_Settings.mediaFlyoutBadgeSize;
    l.badgeX = l.artX + l.artSize - l.badgeSize + 2.0f;
    l.badgeY = l.artY + l.artSize - l.badgeSize + 2.0f;

    l.textX = l.artX + l.artSize + (float)g_Settings.mediaFlyoutTextGap;
    l.textW = W - l.textX - l.marginX;

    l.btnSize = (float)g_Settings.mediaFlyoutBtnSize;
    l.btnY = (float)H - l.marginY - l.btnSize;
    l.pbHeight = (float)g_Settings.mediaFlyoutPbHeight;
    l.pbY = l.btnY - (float)g_Settings.mediaFlyoutPbGap - l.pbHeight;

    l.pbX = l.textX;
    l.pbW = l.textW;

    float btnSpacing = (float)g_Settings.mediaFlyoutBtnSpacing;
    l.btnX1 = l.textX;
    l.btnX2 = l.textX + l.btnSize + btnSpacing;
    l.btnX3 = l.textX + (l.btnSize + btnSpacing) * 2.0f;

    l.rectBtn1 = { (long)l.btnX1, (long)l.btnY, (long)(l.btnX1 + l.btnSize), (long)(l.btnY + l.btnSize) };
    l.rectBtn2 = { (long)l.btnX2, (long)l.btnY, (long)(l.btnX2 + l.btnSize), (long)(l.btnY + l.btnSize) };
    l.rectBtn3 = { (long)l.btnX3, (long)l.btnY, (long)(l.btnX3 + l.btnSize), (long)(l.btnY + l.btnSize) };

    float textSpaceH = l.pbY - l.artY - 4.0f;
    l.titleH = (float)g_Settings.mediaFlyoutTitleSize * 1.3f;
    l.artistH = (float)g_Settings.mediaFlyoutArtistSize * 1.3f;
    float totalTextH = l.titleH + l.artistH + (float)g_Settings.mediaFlyoutLineGap;
    l.titleY = l.artY + (textSpaceH - totalTextH) / 2.0f;
    if (l.titleY < l.artY) l.titleY = l.artY;

    return l;
}

struct VolumeFlyoutLayout {
    float marginX;
    float iconSize;
    float iconX;
    float iconY;
    float textX;
    float textW;
    float sliderX;
    float sliderY;
    float sliderW;
    float sliderH;
};

static VolumeFlyoutLayout CalculateVolumeFlyoutLayout(int W, int H) {
    VolumeFlyoutLayout l;
    l.marginX = (float)g_Settings.volumeFlyoutMarginX;
    l.iconSize = (float)g_Settings.volumeFlyoutIconSize;
    if (l.iconSize > H - 16.0f) l.iconSize = H - 16.0f;
    l.iconX = l.marginX;
    l.iconY = (H - l.iconSize) / 2.0f;

    l.textX = l.iconX + l.iconSize + (float)g_Settings.volumeFlyoutTextGap;
    float fontSize = (float)g_Settings.volumeFlyoutTextSize;
    l.textW = fontSize * 4.0f + 10.0f;

    l.sliderX = l.textX + l.textW + (float)g_Settings.volumeFlyoutSliderGap;
    l.sliderH = (float)g_Settings.volumeFlyoutSliderHeight;
    l.sliderY = (float)H / 2.0f - l.sliderH / 2.0f;
    l.sliderW = W - l.sliderX - (float)g_Settings.volumeFlyoutPaddingRight;
    if (l.sliderW < 0.0f) l.sliderW = 0.0f;

    return l;
}

static void DrawSpeakerIcon(Graphics& g, float x, float y, float size, Color color, int vol, bool muted) {
    SolidBrush brush(color);
    Pen pen(color, 1.5f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    pen.SetLineJoin(LineJoinRound);

    float cx = x + size / 2.0f;
    float cy = y + size / 2.0f;

    // Draw speaker body
    float scale = size / 24.0f;
    float sx = x + 2.0f * scale;
    float sy = cy - 3.0f * scale;
    float sw = 4.0f * scale;
    float sh = 6.0f * scale;
    g.FillRectangle(&brush, (REAL)sx, (REAL)sy, (REAL)sw, (REAL)sh);

    // Polygon for cone part of speaker
    PointF conePts[4] = {
        { sx + sw, sy },
        { sx + sw + 5.0f * scale, sy - 4.0f * scale },
        { sx + sw + 5.0f * scale, sy + sh + 4.0f * scale },
        { sx + sw, sy + sh }
    };
    g.FillPolygon(&brush, conePts, 4);

    if (muted) {
        float offset = 4.0f * scale;
        g.DrawLine(&pen, (REAL)(cx + 1.0f * scale), (REAL)(cy - offset), (REAL)(cx + 1.0f * scale + 6.0f * scale), (REAL)(cy + offset));
        g.DrawLine(&pen, (REAL)(cx + 1.0f * scale), (REAL)(cy + offset), (REAL)(cx + 1.0f * scale + 6.0f * scale), (REAL)(cy - offset));
    } else {
        if (vol > 0) {
            RectF wave1(cx - 3.0f * scale, cy - 4.0f * scale, 8.0f * scale, 8.0f * scale);
            g.DrawArc(&pen, wave1, -45.0f, 90.0f);
        }
        if (vol > 33) {
            RectF wave2(cx - 6.0f * scale, cy - 8.0f * scale, 16.0f * scale, 16.0f * scale);
            g.DrawArc(&pen, wave2, -45.0f, 90.0f);
        }
        if (vol > 66) {
            RectF wave3(cx - 9.0f * scale, cy - 12.0f * scale, 24.0f * scale, 24.0f * scale);
            g.DrawArc(&pen, wave3, -45.0f, 90.0f);
        }
    }
}

static void DrawLockIcon(Graphics& g, float x, float y, float size, Color color, int type, bool active) {
    float r = size * 0.2f;
    GraphicsPath path;
    AddRoundedRectToPath(path, x, y, size, size, r);
    
    Color keycapBg = active ? Color(50, color.GetR(), color.GetG(), color.GetB()) : Color(15, color.GetR(), color.GetG(), color.GetB());
    SolidBrush bgBrush(keycapBg);
    g.FillPath(&bgBrush, &path);
    
    Color keycapBorder = active ? color : Color(100, color.GetR(), color.GetG(), color.GetB());
    Pen borderPen(keycapBorder, 1.5f);
    g.DrawPath(&borderPen, &path);
    
    FontFamily fontFamily(FONT_NAME);
    FontFamily fallbackFamily(L"Segoe UI");
    const FontFamily* activeFamily = (fontFamily.GetLastStatus() == Ok) ? &fontFamily : 
                                    ((fallbackFamily.GetLastStatus() == Ok) ? &fallbackFamily : FontFamily::GenericSansSerif());
    
    float fontSize = size * 0.45f;
    Font font(activeFamily, fontSize, FontStyleBold, UnitPoint);
    SolidBrush textBrush(color);
    
    const wchar_t* letter = L"";
    if (type == 1) letter = L"A";
    else if (type == 2) letter = L"1";
    else if (type == 3) letter = L"S";
    
    StringFormat format(StringFormat::GenericTypographic());
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    
    g.DrawString(letter, -1, &font, RectF(x, y + size * 0.05f, size, size), &format, &textBrush);
    
    float ledSize = size * 0.22f;
    float ledX = x + size - ledSize * 0.7f;
    float ledY = y - ledSize * 0.3f;
    
    Color ledColor = active ? Color(255, 0, 230, 118) : Color(255, 239, 83, 80);
    SolidBrush ledBrush(ledColor);
    g.FillEllipse(&ledBrush, ledX, ledY, ledSize, ledSize);
    
    Pen ledBorderPen(active ? Color(255, 255, 255, 255) : Color(150, 0, 0, 0), 1.0f);
    g.DrawEllipse(&ledBorderPen, ledX, ledY, ledSize, ledSize);
}

static void DrawFlyoutButton(Graphics& g, const FlyoutButton& btn, Color color) {
    if (btn.rect.right - btn.rect.left <= 0) return;
    SolidBrush brush(color);
    float cx = btn.rect.left + (btn.rect.right  - btn.rect.left) / 2.0f;
    float cy = btn.rect.top  + (btn.rect.bottom - btn.rect.top)  / 2.0f;
    float btnW = (float)(btn.rect.right - btn.rect.left);
    float scale = btnW / 26.0f;

    if (btn.isPressed) {
        SolidBrush bg(Color(60, color.GetR(), color.GetG(), color.GetB()));
        g.FillEllipse(&bg, (float)btn.rect.left, (float)btn.rect.top,
                      (float)(btn.rect.right - btn.rect.left), (float)(btn.rect.bottom - btn.rect.top));
        Pen border(Color(100, color.GetR(), color.GetG(), color.GetB()), 1.0f);
        g.DrawEllipse(&border, (float)btn.rect.left, (float)btn.rect.top,
                      (float)(btn.rect.right - btn.rect.left), (float)(btn.rect.bottom - btn.rect.top));
    } else if (btn.isHovered) {
        SolidBrush bg(Color(30, color.GetR(), color.GetG(), color.GetB()));
        g.FillEllipse(&bg, (float)btn.rect.left, (float)btn.rect.top,
                      (float)(btn.rect.right - btn.rect.left), (float)(btn.rect.bottom - btn.rect.top));
        Pen border(Color(50, color.GetR(), color.GetG(), color.GetB()), 1.0f);
        g.DrawEllipse(&border, (float)btn.rect.left, (float)btn.rect.top,
                      (float)(btn.rect.right - btn.rect.left), (float)(btn.rect.bottom - btn.rect.top));
    }

    if (btn.id == 1) { // Prev
        PointF t1[] = { { cx - 1.0f * scale, cy }, { cx + 4.0f * scale, cy - 4.0f * scale }, { cx + 4.0f * scale, cy + 4.0f * scale } };
        g.FillPolygon(&brush, t1, 3);
        PointF t2[] = { { cx - 5.0f * scale, cy }, { cx - 0.0f * scale, cy - 4.0f * scale }, { cx - 0.0f * scale, cy + 4.0f * scale } };
        g.FillPolygon(&brush, t2, 3);
        g.FillRectangle(&brush, (REAL)(cx - 7.0f * scale), (REAL)(cy - 4.0f * scale), 1.5f * scale, 8.0f * scale);
    }
    else if (btn.id == 2) { // Play/Pause
        bool playing = false;
        { lock_guard<mutex> guard(g_MediaState.lock); playing = g_MediaState.isPlaying; }
        if (playing) {
            g.FillRectangle(&brush, (REAL)(cx - 3.0f * scale), (REAL)(cy - 4.0f * scale), 2.0f * scale, 8.0f * scale);
            g.FillRectangle(&brush, (REAL)(cx + 1.0f * scale), (REAL)(cy - 4.0f * scale), 2.0f * scale, 8.0f * scale);
        } else {
            PointF t[] = { { cx + 5.0f * scale, cy }, { cx - 3.0f * scale, cy - 5.0f * scale }, { cx - 3.0f * scale, cy + 5.0f * scale } };
            g.FillPolygon(&brush, t, 3);
        }
    }
    else if (btn.id == 3) { // Next
        PointF t1[] = { { cx + 1.0f * scale, cy }, { cx - 4.0f * scale, cy - 4.0f * scale }, { cx - 4.0f * scale, cy + 4.0f * scale } };
        g.FillPolygon(&brush, t1, 3);
        PointF t2[] = { { cx + 5.0f * scale, cy }, { cx + 0.0f * scale, cy - 4.0f * scale }, { cx + 0.0f * scale, cy + 4.0f * scale } };
        g.FillPolygon(&brush, t2, 3);
        g.FillRectangle(&brush, (REAL)(cx + 6.0f * scale), (REAL)(cy - 4.0f * scale), 1.5f * scale, 8.0f * scale);
    }
}

static void DrawMediaFlyout(HDC hdc, int W, int H) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

    bool lightMode = g_Settings.autoTheme && IsSystemLightMode();
    Color mainColor = lightMode ? Color(255, 10, 10, 10) : Color(255, 245, 245, 245);

    // Modern glass overlay background with customizable opacity
    BYTE opacity = (BYTE)g_Settings.flyoutBgOpacity;
    Color tintColor = lightMode ? Color(opacity, 255, 255, 255) : Color(opacity, 22, 22, 26);
    
    float cornerRadius = (float)g_Settings.mediaFlyoutCornerRadius;
    float borderThickness = (float)g_Settings.mediaFlyoutBorderThickness;
    float halfBorder = borderThickness / 2.0f;
    
    GraphicsPath containerPath;
    AddRoundedRectToPath(containerPath, halfBorder + 1.0f, halfBorder + 1.0f, (float)W - borderThickness - 2.0f, (float)H - borderThickness - 2.0f, cornerRadius);
    
    SolidBrush bgBrush(tintColor);
    g.FillPath(&bgBrush, &containerPath);

    // Dominant color adapted border
    Color domColor = Color(255, 0, 204, 255);
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        domColor = g_MediaState.dominantColor;
    }
    Color adaptedBorder = AdjustColorForTheme(domColor, lightMode);
    
    Pen borderPen(Color(180, adaptedBorder.GetR(), adaptedBorder.GetG(), adaptedBorder.GetB()), borderThickness);
    g.DrawPath(&borderPen, &containerPath);

    // Fonts setup
    FontFamily fontFamily(FONT_NAME);
    FontFamily fallbackFamily(L"Segoe UI");
    const FontFamily* activeFamily = (fontFamily.GetLastStatus() == Ok) ? &fontFamily : 
                                    ((fallbackFamily.GetLastStatus() == Ok) ? &fallbackFamily : FontFamily::GenericSansSerif());

    MediaFlyoutLayout l = CalculateMediaFlyoutLayout(W, H);

    // 1. Draw "NOW PLAYING" Label
    Font labelFont(activeFamily, l.labelFontSize, FontStyleBold, UnitPoint);
    SolidBrush labelBrush(lightMode ? Color(150, 60, 60, 60) : Color(150, 180, 180, 180));
    g.DrawString(L"NOW PLAYING", -1, &labelFont, PointF(l.marginX, l.labelY), nullptr, &labelBrush);

    // 2. Draw Album Cover
    GraphicsPath artPath;
    AddRoundedRectToPath(artPath, l.artX, l.artY, l.artSize, l.artSize, (float)g_Settings.mediaFlyoutArtCornerRadius);
    
    // -------- Read MediaState under MINIMAL lock (no Bitmap ops inside lock) --------
    wstring title = L"";
    wstring artist = L"";
    bool hasMedia = false;
    bool flyIsPlaying = false;
    double statePos2 = 0.0, stateDur2 = 0.0;
    ULONGLONG stateTick2 = 0;
    uint64_t flyArtGen = 0;
    Bitmap* flyRawAlbumArt = nullptr;

    {
        lock_guard<mutex> guard(g_MediaState.lock);
        title        = g_MediaState.title;
        artist       = g_MediaState.artist;
        hasMedia     = g_MediaState.hasMedia;
        flyIsPlaying = g_MediaState.isPlaying;
        statePos2    = g_MediaState.positionSec;
        stateDur2    = g_MediaState.durationSec;
        stateTick2   = g_MediaState.lastUpdateTick;
        flyArtGen    = g_MediaState.albumArtGeneration;
        flyRawAlbumArt = g_MediaState.albumArt;  // non-owning peek
    }

    // -------- Smooth Position for Flyout (independent from panel smooth state) --------
    static double    s_flySmoothedPos  = 0.0;
    static double    s_flySmoothedDur  = 0.0;
    static ULONGLONG s_flySmoothedTick = 0;
    static bool      s_flySmoothedPlay = false;

    if (stateTick2 != s_flySmoothedTick || flyIsPlaying != s_flySmoothedPlay) {
        s_flySmoothedPos  = statePos2;
        s_flySmoothedDur  = stateDur2;
        s_flySmoothedTick = stateTick2;
        s_flySmoothedPlay = flyIsPlaying;
    } else {
        s_flySmoothedDur = stateDur2;
    }
    double estPos = s_flySmoothedPos;
    if (s_flySmoothedPlay && s_flySmoothedTick > 0) {
        estPos += (GetTickCount64() - s_flySmoothedTick) / 1000.0;
    }
    double duration = s_flySmoothedDur;
    if (duration > 0.0 && estPos > duration) estPos = duration;
    if (estPos < 0.0) estPos = 0.0;

    // -------- Flyout Cached Album Art (Clone only on art change) --------
    if (flyArtGen != g_flyoutCachedArtGen) {
        Bitmap* newFlyCache = nullptr;
        if (flyRawAlbumArt) {
            lock_guard<mutex> guard(g_MediaState.lock);
            if (g_MediaState.albumArt) {
                newFlyCache = g_MediaState.albumArt->Clone(
                    0, 0, g_MediaState.albumArt->GetWidth(), g_MediaState.albumArt->GetHeight(),
                    g_MediaState.albumArt->GetPixelFormat());
                if (newFlyCache && newFlyCache->GetLastStatus() != Ok) {
                    delete newFlyCache;
                    newFlyCache = nullptr;
                }
            }
            flyArtGen = g_MediaState.albumArtGeneration;
        }
        if (g_flyoutCachedAlbumArt) { delete g_flyoutCachedAlbumArt; }
        g_flyoutCachedAlbumArt = newFlyCache;
        g_flyoutCachedArtGen   = flyArtGen;
    }
    Bitmap* albumArtCopy = g_flyoutCachedAlbumArt;  // flyout-thread owned, do NOT delete after use

    g.SetClip(&artPath);
    if (hasMedia && albumArtCopy) {
        g.DrawImage(albumArtCopy, (REAL)l.artX, (REAL)l.artY, (REAL)l.artSize, (REAL)l.artSize);
    } else {
        DrawPlaceholderArt(g, (float)l.artX, (float)l.artY, (float)l.artSize, lightMode, GetBarColor(0, 1.0f));
    }
    g.ResetClip();
    // Note: albumArtCopy = g_flyoutCachedAlbumArt, owned by flyout thread — do NOT delete here


    Pen artBorder(lightMode ? Color(40, 0, 0, 0) : Color(50, 255, 255, 255), 1.0f);
    g.DrawPath(&artBorder, &artPath);

    // Source App Badge Overlay — use pre-cached icon from media thread (no per-frame lookup)
    {
        Bitmap* appIcon = nullptr;
        {
            lock_guard<mutex> guard(g_MediaState.lock);
            appIcon = hasMedia ? g_MediaState.appIcon : nullptr;
        }
        if (appIcon) {
            float bgR = l.badgeSize * 0.35f;
            // shadow
            GraphicsPath badgeShadowPath;
            AddRoundedRectToPath(badgeShadowPath, l.badgeX - 1.0f, l.badgeY + 1.5f, l.badgeSize + 2.0f, l.badgeSize + 2.0f, bgR);
            SolidBrush badgeShadowBrush(Color(55, 0, 0, 0));
            g.FillPath(&badgeShadowBrush, &badgeShadowPath);
            // background
            GraphicsPath badgeBgPath;
            AddRoundedRectToPath(badgeBgPath, l.badgeX - 1.0f, l.badgeY - 1.0f, l.badgeSize + 2.0f, l.badgeSize + 2.0f, bgR);
            Color badgeBgCol = lightMode ? Color(240, 250, 250, 252) : Color(240, 18, 18, 22);
            SolidBrush badgeBgBrush(badgeBgCol);
            g.FillPath(&badgeBgBrush, &badgeBgPath);
            // border
            Pen badgeBorderPen(lightMode ? Color(55, 0, 0, 0) : Color(80, 255, 255, 255), 0.75f);
            g.DrawPath(&badgeBorderPen, &badgeBgPath);

            ImageAttributes attr;
            ColorMatrix matrix = { {
                { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.92f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
            } };
            attr.SetColorMatrix(&matrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

            g.DrawImage(appIcon,
                        RectF(l.badgeX, l.badgeY, l.badgeSize, l.badgeSize),
                        0.0f, 0.0f, (REAL)appIcon->GetWidth(), (REAL)appIcon->GetHeight(),
                        UnitPixel, &attr);
        }
    }

    Font titleFont(activeFamily, (float)g_Settings.mediaFlyoutTitleSize, FontStyleBold, UnitPoint);
    Font artistFont(activeFamily, (float)g_Settings.mediaFlyoutArtistSize, FontStyleRegular, UnitPoint);
    
    Color tColor = mainColor;
    Color aColor = Color(180, mainColor.GetR(), mainColor.GetG(), mainColor.GetB());
    if (g_Settings.useCustomTextColors) {
        tColor = DWORDtoColor(g_Settings.titleColor);
        aColor = DWORDtoColor(g_Settings.artistColor);
    }
    SolidBrush tBrush(tColor);
    SolidBrush aBrush(aColor);
    
    StringFormat format(StringFormat::GenericTypographic());
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentNear);
    format.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsNoClip);
    format.SetTrimming(StringTrimmingEllipsisCharacter);

    wstring displayTitle  = hasMedia ? title  : L"No Media Playing";
    wstring displayArtist = hasMedia ? artist : L"";
    
    RectF titleRect(l.textX, l.titleY, l.textW, l.titleH);
    g.DrawString(displayTitle.c_str(), -1, &titleFont, titleRect, &format, &tBrush);

    RectF artistRect(l.textX, l.titleY + l.titleH + (float)g_Settings.mediaFlyoutLineGap, l.textW, l.artistH);
    g.DrawString(displayArtist.c_str(), -1, &artistFont, artistRect, &format, &aBrush);

    // 4. Draw Progress Bar
    bool hasProgress = (duration > 0.0 && hasMedia) || g_mediaFlyoutIsSeeking;
    float progressRatio = 0.0f;
    if (g_mediaFlyoutIsSeeking) {
        progressRatio = g_mediaFlyoutSeekProgress;
    } else if (duration > 0.0) {
        progressRatio = (float)(estPos / duration);
    }
    if (progressRatio > 1.0f) progressRatio = 1.0f;
    if (progressRatio < 0.0f) progressRatio = 0.0f;

    // Track Background
    GraphicsPath pbTrackPath;
    AddRoundedRectToPath(pbTrackPath, l.pbX, l.pbY, l.pbW, l.pbHeight, l.pbHeight / 2.0f);
    SolidBrush trackBg(lightMode ? Color(40, 0, 0, 0) : Color(40, 255, 255, 255));
    g.FillPath(&trackBg, &pbTrackPath);

    // Fill
    if (hasProgress && progressRatio > 0.0f) {
        float fillW = l.pbW * progressRatio;
        if (fillW > 1.0f) {
            GraphicsPath pbFillPath;
            AddRoundedRectToPath(pbFillPath, l.pbX, l.pbY, fillW, l.pbHeight, l.pbHeight / 2.0f);
            SolidBrush fillBr(adaptedBorder);
            g.FillPath(&fillBr, &pbFillPath);
            
            // Draw small knob/handle if mouse is over progress bar or seeking
            POINT pt; GetCursorPos(&pt); ScreenToClient(g_hMediaFlyoutWnd, &pt);
            float knobSize = (float)g_Settings.mediaFlyoutKnobSize;
            RECT pbSensRect = { (long)l.pbX, (long)(l.pbY - 6.0f), (long)(l.pbX + l.pbW), (long)(l.pbY + l.pbHeight + 6.0f) };
            if (PtInRect(&pbSensRect, pt) || g_mediaFlyoutIsSeeking) {
                SolidBrush knobBrush(mainColor);
                g.FillEllipse(&knobBrush, l.pbX + fillW - knobSize / 2.0f, l.pbY + l.pbHeight / 2.0f - knobSize / 2.0f, knobSize, knobSize);
            }
        }
    }

    // 5. Draw Action Buttons
    g_mediaFlyoutButtons[0].id = 1;
    g_mediaFlyoutButtons[0].rect = l.rectBtn1;
    g_mediaFlyoutButtons[1].id = 2;
    g_mediaFlyoutButtons[1].rect = l.rectBtn2;
    g_mediaFlyoutButtons[2].id = 3;
    g_mediaFlyoutButtons[2].rect = l.rectBtn3;

    for (int i = 0; i < 3; i++) {
        DrawFlyoutButton(g, g_mediaFlyoutButtons[i], mainColor);
    }

    // 6. Draw Timeline Text (aligned right next to buttons)
    if (hasMedia && duration > 0.0) {
        int curMin = (int)(estPos / 60.0);
        int curSec = (int)fmod(estPos, 60.0);
        int durMin = (int)(duration / 60.0);
        int durSec = (int)fmod(duration, 60.0);
        
        wchar_t timeBuf[64];
        swprintf_s(timeBuf, L"%02d:%02d / %02d:%02d", curMin, curSec, durMin, durSec);
        
        Font timeFont(activeFamily, (float)g_Settings.mediaFlyoutTimeTextSize, FontStyleRegular, UnitPoint);
        SolidBrush timeBrush(aColor);
        
        StringFormat timeFormat(StringFormat::GenericTypographic());
        timeFormat.SetAlignment(StringAlignmentFar);
        timeFormat.SetLineAlignment(StringAlignmentCenter);
        
        g.DrawString(timeBuf, -1, &timeFont, RectF(l.pbX, l.btnY, l.pbW, l.btnSize), &timeFormat, &timeBrush);
    }
}

static void DrawVolumeFlyout(HDC hdc, int W, int H) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

    bool lightMode = g_Settings.autoTheme && IsSystemLightMode();
    Color mainColor = lightMode ? Color(255, 10, 10, 10) : Color(255, 245, 245, 245);

    // Glass overlay background with customizable opacity
    BYTE opacity = (BYTE)g_Settings.flyoutBgOpacity;
    Color tintColor = lightMode ? Color(opacity, 255, 255, 255) : Color(opacity, 22, 22, 26);
    
    float cornerRadius = (float)g_Settings.volumeFlyoutCornerRadius;
    float borderThickness = (float)g_Settings.volumeFlyoutBorderThickness;
    float halfBorder = borderThickness / 2.0f;
    
    GraphicsPath containerPath;
    AddRoundedRectToPath(containerPath, halfBorder + 1.0f, halfBorder + 1.0f, (float)W - borderThickness - 2.0f, (float)H - borderThickness - 2.0f, cornerRadius);
    
    SolidBrush bgBrush(tintColor);
    g.FillPath(&bgBrush, &containerPath);

    // Border adjusted to dominant color of playing media to make it consistent, or default accent
    Color domColor = Color(255, 0, 204, 255);
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        domColor = g_MediaState.dominantColor;
    }
    Color adaptedBorder = AdjustColorForTheme(domColor, lightMode);
    Pen borderPen(Color(180, adaptedBorder.GetR(), adaptedBorder.GetG(), adaptedBorder.GetB()), borderThickness);
    g.DrawPath(&borderPen, &containerPath);

    int type = g_volumeFlyoutType.load();
    if (type != 0) {
        bool active = g_volumeFlyoutActive.load();
        VolumeFlyoutLayout l = CalculateVolumeFlyoutLayout(W, H);
        
        DrawLockIcon(g, l.iconX, l.iconY, l.iconSize, mainColor, type, active);
        
        FontFamily fontFamily(FONT_NAME);
        FontFamily fallbackFamily(L"Segoe UI");
        const FontFamily* activeFamily = (fontFamily.GetLastStatus() == Ok) ? &fontFamily : 
                                        ((fallbackFamily.GetLastStatus() == Ok) ? &fallbackFamily : FontFamily::GenericSansSerif());
        
        float fontSize = (float)g_Settings.volumeFlyoutTextSize;
        Font lockFont(activeFamily, fontSize * 1.1f, FontStyleBold, UnitPoint);
        SolidBrush lockBrush(mainColor);
        
        wchar_t txt[64];
        const wchar_t* keyName = L"";
        if (type == 1) keyName = L"Caps Lock";
        else if (type == 2) keyName = L"Num Lock";
        else if (type == 3) keyName = L"Scroll Lock";
        
        swprintf_s(txt, L"%s %s", keyName, active ? L"On" : L"Off");
        
        StringFormat format(StringFormat::GenericTypographic());
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        
        float textW = W - l.textX - (float)g_Settings.volumeFlyoutPaddingRight;
        g.DrawString(txt, -1, &lockFont, RectF(l.textX, 0.0f, textW, (float)H), &format, &lockBrush);
    }
    else {
        int vol = g_volumeLevel.load();
        bool muted = g_volumeMuted.load();

        VolumeFlyoutLayout l = CalculateVolumeFlyoutLayout(W, H);

        DrawSpeakerIcon(g, l.iconX, l.iconY, l.iconSize, mainColor, vol, muted);

        FontFamily fontFamily(FONT_NAME);
        FontFamily fallbackFamily(L"Segoe UI");
        const FontFamily* activeFamily = (fontFamily.GetLastStatus() == Ok) ? &fontFamily : 
                                        ((fallbackFamily.GetLastStatus() == Ok) ? &fallbackFamily : FontFamily::GenericSansSerif());
        
        float fontSize = (float)g_Settings.volumeFlyoutTextSize;
        Font volFont(activeFamily, fontSize, FontStyleBold, UnitPoint);
        SolidBrush volBrush(mainColor);
        
        wchar_t txt[32];
        if (muted) {
            txt[0] = L'\0';
        } else {
            swprintf_s(txt, L"%d%%", vol);
        }
        
        StringFormat format(StringFormat::GenericTypographic());
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentCenter);
        
        if (!muted) {
            g.DrawString(txt, -1, &volFont, RectF(l.textX, 2.0f, l.textW, (float)H - 4.0f), &format, &volBrush);
        }

        if (l.sliderW > 10.0f) {
            GraphicsPath sliderTrackPath;
            AddRoundedRectToPath(sliderTrackPath, l.sliderX, l.sliderY, l.sliderW, l.sliderH, l.sliderH / 2.0f);
            SolidBrush trackBg(lightMode ? Color(40, 0, 0, 0) : Color(40, 255, 255, 255));
            g.FillPath(&trackBg, &sliderTrackPath);

            float fillW = l.sliderW * (vol / 100.0f);
            if (!muted && fillW > 1.0f) {
                GraphicsPath sliderFillPath;
                AddRoundedRectToPath(sliderFillPath, l.sliderX, l.sliderY, fillW, l.sliderH, l.sliderH / 2.0f);
                SolidBrush fillBr(adaptedBorder);
                g.FillPath(&fillBr, &sliderFillPath);
                
                SolidBrush dotBrush(mainColor);
                float dotSize = l.sliderH * 2.0f;
                if (dotSize < 6.0f) dotSize = 6.0f;
                g.FillEllipse(&dotBrush, l.sliderX + fillW - dotSize / 2.0f, l.sliderY + l.sliderH / 2.0f - dotSize / 2.0f, dotSize, dotSize);
            }
        }
    }
}

static POINT CalculateFlyoutPosition(int flyoutW, int flyoutH, int offsetY) {
    POINT pt = {0, 0};
    
    HWND hBar = FindWindow(L"Shell_TrayWnd", nullptr);
    RECT barRc = {};
    if (hBar && IsWindow(hBar)) {
        GetWindowRect(hBar, &barRc);
    } else {
        barRc.left = 0;
        barRc.top = GetSystemMetrics(SM_CYSCREEN) - 40;
        barRc.right = GetSystemMetrics(SM_CXSCREEN);
        barRc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    
    int barW = barRc.right - barRc.left;
    int barH = barRc.bottom - barRc.top;
    
    bool isBottom = true;
    bool isTop = false;
    bool isLeft = false;
    bool isRight = false;
    
    if (barW > barH) {
        if (barRc.top < 100) {
            isTop = true;
            isBottom = false;
        }
    } else {
        if (barRc.left < 100) {
            isLeft = true;
            isBottom = false;
        } else {
            isRight = true;
            isBottom = false;
        }
    }
    
    RECT mainRc = {};
    bool hasMainRc = false;
    if (g_hVisWnd && IsWindow(g_hVisWnd) && IsWindowVisible(g_hVisWnd)) {
        GetWindowRect(g_hVisWnd, &mainRc);
        // Safety check: if g_hVisWnd is at its default creation position (0, 0)
        // and the taskbar is not at the top, it is not positioned correctly yet.
        bool isAtDefaultZero = (mainRc.left == 0 && mainRc.top == 0);
        if (isAtDefaultZero && !isTop) {
            hasMainRc = false;
        } else if (mainRc.left > -10000 && mainRc.left < 20000 && mainRc.top > -10000 && mainRc.top < 20000) {
            hasMainRc = true;
        }
    }
    
    if (hasMainRc) {
        if (isBottom) {
            pt.x = mainRc.left + ((mainRc.right - mainRc.left) - flyoutW) / 2;
            pt.y = mainRc.top - flyoutH - offsetY;
        } else if (isTop) {
            pt.x = mainRc.left + ((mainRc.right - mainRc.left) - flyoutW) / 2;
            pt.y = mainRc.bottom + offsetY;
        } else if (isLeft) {
            pt.x = mainRc.right + offsetY;
            pt.y = mainRc.top + ((mainRc.bottom - mainRc.top) - flyoutH) / 2;
        } else if (isRight) {
            pt.x = mainRc.left - flyoutW - offsetY;
            pt.y = mainRc.top + ((mainRc.bottom - mainRc.top) - flyoutH) / 2;
        }
    } else {
        if (isBottom) {
            pt.x = barRc.left + (barW - flyoutW) / 2;
            pt.y = barRc.top - flyoutH - offsetY;
        } else if (isTop) {
            pt.x = barRc.left + (barW - flyoutW) / 2;
            pt.y = barRc.bottom + offsetY;
        } else if (isLeft) {
            pt.x = barRc.right + offsetY;
            pt.y = barRc.top + (barH - flyoutH) / 2;
        } else if (isRight) {
            pt.x = barRc.left - flyoutW - offsetY;
            pt.y = barRc.top + (barH - flyoutH) / 2;
        }
    }
    
    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(hMonitor, &mi)) {
        if (pt.x < mi.rcWork.left) pt.x = mi.rcWork.left + 8;
        if (pt.x + flyoutW > mi.rcWork.right) pt.x = mi.rcWork.right - flyoutW - 8;
        if (pt.y < mi.rcWork.top) pt.y = mi.rcWork.top + 8;
        if (pt.y + flyoutH > mi.rcWork.bottom) pt.y = mi.rcWork.bottom - flyoutH - 8;
    }
    
    return pt;
}

static POINT CalculateMediaFlyoutPosition(int flyoutW, int flyoutH) {
    POINT pt = {0, 0};
    
    HWND hTarget = g_hVisWnd;
    if (!hTarget || !IsWindow(hTarget)) {
        hTarget = GetForegroundWindow();
    }
    HMONITOR hMonitor = MonitorFromWindow(hTarget, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMonitor, &mi)) {
        mi.rcWork.left = 0;
        mi.rcWork.top = 0;
        mi.rcWork.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcWork.bottom = GetSystemMetrics(SM_CYSCREEN);
        mi.rcMonitor = mi.rcWork;
    }
    
    if (g_Settings.mediaFlyoutPosition == 0) {
        return CalculateFlyoutPosition(flyoutW, flyoutH, g_Settings.mediaFlyoutOffsetY);
    }
    
    RECT rcRef = IsFullscreenActive() ? mi.rcMonitor : mi.rcWork;
    int marginX = g_Settings.mediaFlyoutMarginX;
    int marginY = g_Settings.mediaFlyoutMarginY;
    int offsetY = g_Settings.mediaFlyoutOffsetY;
    
    switch (g_Settings.mediaFlyoutPosition) {
        case 1: // Top Left
            pt.x = rcRef.left + marginX;
            pt.y = rcRef.top + marginY + offsetY;
            break;
        case 2: // Top Center
            pt.x = rcRef.left + (rcRef.right - rcRef.left - flyoutW) / 2;
            pt.y = rcRef.top + marginY + offsetY;
            break;
        case 3: // Top Right
            pt.x = rcRef.right - flyoutW - marginX;
            pt.y = rcRef.top + marginY + offsetY;
            break;
        case 4: // Bottom Left
            pt.x = rcRef.left + marginX;
            pt.y = rcRef.bottom - flyoutH - marginY - offsetY;
            break;
        case 5: // Bottom Center
            pt.x = rcRef.left + (rcRef.right - rcRef.left - flyoutW) / 2;
            pt.y = rcRef.bottom - flyoutH - marginY - offsetY;
            break;
        case 6: // Bottom Right
        default:
            pt.x = rcRef.right - flyoutW - marginX;
            pt.y = rcRef.bottom - flyoutH - marginY - offsetY;
            break;
    }
    
    if (pt.x < rcRef.left) pt.x = rcRef.left + 8;
    if (pt.x + flyoutW > rcRef.right) pt.x = rcRef.right - flyoutW - 8;
    if (pt.y < rcRef.top) pt.y = rcRef.top + 8;
    if (pt.y + flyoutH > rcRef.bottom) pt.y = rcRef.bottom - flyoutH - 8;
    
    return pt;
}

static void UpdateLayeredWindowHelper(HWND hwnd, int alpha, int type) {
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int W = rc.right - rc.left;
    int H = rc.bottom - rc.top;
    
    if (W <= 0 || H <= 0) return;
    
    HDC hdcScreen = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(hdcScreen);
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = W;
    bmi.bmiHeader.biHeight = -H;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    void* pBits = nullptr;
    HBITMAP memBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    if (!memBmp) {
        DeleteDC(memDC);
        ReleaseDC(nullptr, hdcScreen);
        return;
    }
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);
    
    {
        Graphics g(memDC);
        g.Clear(Color(0, 0, 0, 0));
    }
    
    if (type == 1) {
        DrawMediaFlyout(memDC, W, H);
    } else if (type == 2) {
        DrawVolumeFlyout(memDC, W, H);
    }
    
    POINT ptSrc = {0, 0};
    POINT ptDst = {rc.left, rc.top};
    SIZE sizeWnd = {W, H};
    
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = (BYTE)alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;
    
    BOOL ulwRes = UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);
    if (!ulwRes) {
        Wh_Log(L"UpdateLayeredWindow failed! HWND=%p, Error=%d", hwnd, GetLastError());
    }
    
    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
    ReleaseDC(nullptr, hdcScreen);
}

static void RedrawMediaFlyout() {
    if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
        UpdateLayeredWindowHelper(g_hMediaFlyoutWnd, g_mediaFlyoutAlpha, 1);
    }
}

static void RedrawVolumeFlyout() {
    if (g_hVolumeFlyoutWnd && IsWindow(g_hVolumeFlyoutWnd)) {
        UpdateLayeredWindowHelper(g_hVolumeFlyoutWnd, g_volumeFlyoutAlpha, 2);
    }
}

static LRESULT CALLBACK MediaFlyoutWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_NCHITTEST: {
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            ScreenToClient(hwnd, &pt);
            
            RECT clientRc; GetClientRect(hwnd, &clientRc);
            MediaFlyoutLayout l = CalculateMediaFlyoutLayout(clientRc.right, clientRc.bottom);
            
            g_mediaFlyoutButtons[0].rect = l.rectBtn1;
            g_mediaFlyoutButtons[1].rect = l.rectBtn2;
            g_mediaFlyoutButtons[2].rect = l.rectBtn3;
            
            for (int i = 0; i < 3; i++) {
                if (PtInRect(&g_mediaFlyoutButtons[i].rect, pt)) return HTCLIENT;
            }
            
            RECT pbSensRect = { (long)l.pbX, (long)(l.pbY - 6.0f), (long)(l.pbX + l.pbW), (long)(l.pbY + l.pbHeight + 6.0f) };
            if (PtInRect(&pbSensRect, pt)) return HTCLIENT;

            return HTTRANSPARENT;
        }

        case WM_MOUSEMOVE: {
            RECT rc; GetClientRect(hwnd, &rc);
            int W = rc.right;
            int H = rc.bottom;
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            
            MediaFlyoutLayout l = CalculateMediaFlyoutLayout(W, H);
            
            g_mediaFlyoutButtons[0].rect = l.rectBtn1;
            g_mediaFlyoutButtons[1].rect = l.rectBtn2;
            g_mediaFlyoutButtons[2].rect = l.rectBtn3;
            
            if (g_mediaFlyoutIsSeeking) {
                float ratio = (float)(pt.x - l.pbX) / l.pbW;
                g_mediaFlyoutSeekProgress = max(0.0f, min(1.0f, ratio));
                RedrawMediaFlyout();
                return 0;
            }
            
            if (!g_mediaFlyoutTrackingMouse) {
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                g_mediaFlyoutTrackingMouse = true;
            }
            
            bool changed = false;
            for (int i = 0; i < 3; i++) {
                bool hover = PtInRect(&g_mediaFlyoutButtons[i].rect, pt) != 0;
                if (g_mediaFlyoutButtons[i].isHovered != hover) {
                    g_mediaFlyoutButtons[i].isHovered = hover;
                    changed = true;
                }
            }
            if (changed) RedrawMediaFlyout();
            return 0;
        }

        case WM_MOUSELEAVE: {
            g_mediaFlyoutTrackingMouse = false;
            bool changed = false;
            for (int i = 0; i < 3; i++) {
                if (g_mediaFlyoutButtons[i].isHovered || g_mediaFlyoutButtons[i].isPressed) {
                    g_mediaFlyoutButtons[i].isHovered = false;
                    g_mediaFlyoutButtons[i].isPressed = false;
                    changed = true;
                }
            }
            if (changed) RedrawMediaFlyout();
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            RECT rc; GetClientRect(hwnd, &rc);
            int W = rc.right;
            int H = rc.bottom;
            
            MediaFlyoutLayout l = CalculateMediaFlyoutLayout(W, H);
            
            g_mediaFlyoutButtons[0].rect = l.rectBtn1;
            g_mediaFlyoutButtons[1].rect = l.rectBtn2;
            g_mediaFlyoutButtons[2].rect = l.rectBtn3;
            
            RECT pbSensRect = { (long)l.pbX, (long)(l.pbY - 6.0f), (long)(l.pbX + l.pbW), (long)(l.pbY + l.pbHeight + 6.0f) };
            if (PtInRect(&pbSensRect, pt)) {
                g_mediaFlyoutIsSeeking = true;
                SetCapture(hwnd);
                float ratio = (float)(pt.x - l.pbX) / l.pbW;
                g_mediaFlyoutSeekProgress = max(0.0f, min(1.0f, ratio));
                RedrawMediaFlyout();
                return 0;
            }
            
            for (int i = 0; i < 3; i++) {
                if (PtInRect(&g_mediaFlyoutButtons[i].rect, pt)) {
                    g_mediaFlyoutButtons[i].isPressed = true;
                    RedrawMediaFlyout();
                    SetCapture(hwnd);
                    break;
                }
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            RECT rc; GetClientRect(hwnd, &rc);
            int W = rc.right;
            int H = rc.bottom;
            
            MediaFlyoutLayout l = CalculateMediaFlyoutLayout(W, H);
            
            g_mediaFlyoutButtons[0].rect = l.rectBtn1;
            g_mediaFlyoutButtons[1].rect = l.rectBtn2;
            g_mediaFlyoutButtons[2].rect = l.rectBtn3;
            
            if (g_mediaFlyoutIsSeeking) {
                ReleaseCapture();
                g_mediaFlyoutIsSeeking = false;
                double duration = 0.0;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    duration = g_MediaState.durationSec;
                }
                if (duration > 0.0) {
                    double targetSec = g_mediaFlyoutSeekProgress * duration;
                    {
                        lock_guard<mutex> guard(g_MediaState.lock);
                        g_MediaState.positionSec = targetSec;
                        g_MediaState.lastUpdateTick = GetTickCount64();
                    }
                    SeekToPosition(targetSec);
                }
                RedrawMediaFlyout();
                if (g_hVisWnd && IsWindow(g_hVisWnd)) {
                    InvalidateRect(g_hVisWnd, nullptr, FALSE);
                }
                return 0;
            }
            
            ReleaseCapture();
            for (int i = 0; i < 3; i++) {
                if (g_mediaFlyoutButtons[i].isPressed) {
                    g_mediaFlyoutButtons[i].isPressed = false;
                    RedrawMediaFlyout();
                    if (PtInRect(&g_mediaFlyoutButtons[i].rect, pt)) {
                        if (g_mediaFlyoutButtons[i].id == 2) {
                            lock_guard<mutex> guard(g_MediaState.lock);
                            g_MediaState.isPlaying = !g_MediaState.isPlaying;
                            g_MediaState.lastUpdateTick = GetTickCount64();
                        }
                        SendMediaCommand(g_mediaFlyoutButtons[i].id);
                        TriggerInstantMediaUpdate();
                        if (g_hVisWnd && IsWindow(g_hVisWnd)) {
                            InvalidateRect(g_hVisWnd, nullptr, FALSE);
                        }
                    }
                    break;
                }
            }
            return 0;
        }

        case WM_APP + 35: { // Song changed trigger
            if (!g_Settings.showMediaFlyout) return 0;
            if (g_Settings.hideOnFullscreen && IsFullscreenActive() && !g_Settings.flyoutOnFullscreen) return 0; // Don't show in fullscreen
            g_mediaFlyoutState = 1;
            g_mediaFlyoutShowTimeLeft = g_Settings.mediaFlyoutDuration * 1000;
            
            int flyoutW = g_Settings.mediaFlyoutW;
            int flyoutH = g_Settings.mediaFlyoutH;
            POINT pt = CalculateMediaFlyoutPosition(flyoutW, flyoutH);
            
            SetWindowPos(hwnd, HWND_TOPMOST, pt.x, pt.y, flyoutW, flyoutH, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            UpdateFlyoutAppearance(hwnd);
            
            SetTimer(hwnd, TIMER_MEDIA_FADE, 16, nullptr);
            SetTimer(hwnd, TIMER_MEDIA_SHOW, 100, nullptr);
            return 0;
        }

        case WM_APP + 36: { // Redraw media flyout on UI thread only
            if (g_mediaFlyoutState != 0) {
                RedrawMediaFlyout();
            }
            return 0;
        }

        case WM_TIMER: {
            if (wp == TIMER_MEDIA_SHOW) {
                POINT pt; GetCursorPos(&pt);
                RECT winRc; GetWindowRect(hwnd, &winRc);
                bool mouseOver = PtInRect(&winRc, pt) != 0;
                
                if (mouseOver || g_mediaFlyoutIsSeeking) {
                    g_mediaFlyoutShowTimeLeft = g_Settings.mediaFlyoutDuration * 1000;
                } else {
                    if (g_mediaFlyoutShowTimeLeft >= 100) {
                        g_mediaFlyoutShowTimeLeft -= 100;
                    } else {
                        g_mediaFlyoutShowTimeLeft = 0;
                        g_mediaFlyoutState = 3; // FadeOut
                    }
                }
                RedrawMediaFlyout();
            }
            else if (wp == TIMER_MEDIA_FADE) {
                if (g_mediaFlyoutState == 1) {
                    g_mediaFlyoutAlpha += 25;
                    if (g_mediaFlyoutAlpha >= 255) {
                        g_mediaFlyoutAlpha = 255;
                        g_mediaFlyoutState = 2;
                    }
                    RedrawMediaFlyout();
                }
                else if (g_mediaFlyoutState == 3) {
                    g_mediaFlyoutAlpha -= 20;
                    if (g_mediaFlyoutAlpha <= 0) {
                        g_mediaFlyoutAlpha = 0;
                        g_mediaFlyoutState = 0;
                        KillTimer(hwnd, TIMER_MEDIA_FADE);
                        KillTimer(hwnd, TIMER_MEDIA_SHOW);
                        ShowWindow(hwnd, SW_HIDE);
                    } else {
                        RedrawMediaFlyout();
                    }
                }
                else if (g_mediaFlyoutState == 2) {
                    if (g_mediaFlyoutAlpha != 255) {
                        g_mediaFlyoutAlpha = 255;
                        RedrawMediaFlyout();
                    }
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK VolumeFlyoutWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_NCHITTEST:
            return HTCLIENT; // Volume flyout is now interactive!

        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lp);
            int y = GET_Y_LPARAM(lp);
            RECT rc; GetClientRect(hwnd, &rc);
            int W = rc.right, H = rc.bottom;
            
            VolumeFlyoutLayout l = CalculateVolumeFlyoutLayout(W, H);
            
            // Check if clicked speaker icon (to toggle mute)
            if (g_volumeFlyoutType.load() == 0 && x >= l.iconX && x <= l.iconX + l.iconSize && y >= l.iconY && y <= l.iconY + l.iconSize) {
                if (g_endpointVolume) {
                    BOOL muted = FALSE;
                    g_endpointVolume->GetMute(&muted);
                    g_endpointVolume->SetMute(!muted, nullptr);
                }
                g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;
                RedrawVolumeFlyout();
                return 0;
            }
            
            // Check if clicked slider track (to drag volume)
            if (g_volumeFlyoutType.load() == 0 && l.sliderW > 10.0f && x >= l.sliderX - 5.0f && x <= l.sliderX + l.sliderW + 5.0f) {
                g_volumeFlyoutIsDragging = true;
                SetCapture(hwnd);
                
                float pct = (x - l.sliderX) / l.sliderW;
                if (pct < 0.0f) pct = 0.0f;
                if (pct > 1.0f) pct = 1.0f;
                if (g_endpointVolume) {
                    g_endpointVolume->SetMasterVolumeLevelScalar(pct, nullptr);
                }
                g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;
                RedrawVolumeFlyout();
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (g_volumeFlyoutIsDragging) {
                int x = GET_X_LPARAM(lp);
                RECT rc; GetClientRect(hwnd, &rc);
                int W = rc.right, H = rc.bottom;
                
                VolumeFlyoutLayout l = CalculateVolumeFlyoutLayout(W, H);
                if (l.sliderW > 10.0f) {
                    float pct = (x - l.sliderX) / l.sliderW;
                    if (pct < 0.0f) pct = 0.0f;
                    if (pct > 1.0f) pct = 1.0f;
                    if (g_endpointVolume) {
                        g_endpointVolume->SetMasterVolumeLevelScalar(pct, nullptr);
                    }
                }
                g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;
                RedrawVolumeFlyout();
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_volumeFlyoutIsDragging) {
                g_volumeFlyoutIsDragging = false;
                ReleaseCapture();
                g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;
                RedrawVolumeFlyout();
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wp);
            AdjustSystemVolume(zDelta > 0 ? 1 : -1);
            g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;
            RedrawVolumeFlyout();
            return 0;
        }

        case WM_APP + 41: { // Lock key state changed trigger (wp = type, lp = active)
            int prevType = g_volumeFlyoutType.load();
            int newType = (int)wp;
            g_volumeFlyoutType.store(newType);
            g_volumeFlyoutActive.store(lp != 0);
            
            if (g_Settings.hideOnFullscreen && IsFullscreenActive()) return 0; // Don't show in fullscreen
            
            g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;

            // If the flyout is already visible/active and the type did not change,
            // just redraw smoothly. If it was fading out (state == 3), abort fade out and show fully.
            // Also refresh display timer
            if (g_volumeFlyoutState != 0 && prevType == newType) {
                g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;
                if (g_volumeFlyoutState == 3) {
                    g_volumeFlyoutState = 2; // Abort FadeOut, keep showing
                    g_volumeFlyoutAlpha = 255;
                }
                RedrawVolumeFlyout();
                return 0;
            }
            
            g_volumeFlyoutState = 1;
            g_volumeFlyoutAlpha = 0; // Force reset to transparent to start fade-in cleanly
            
            int volW = g_Settings.volumeFlyoutW;
            int volH = g_Settings.volumeFlyoutH;
            POINT pt = CalculateFlyoutPosition(volW, volH, g_Settings.volumeFlyoutOffsetY);
            
            SetWindowPos(hwnd, HWND_TOPMOST, pt.x, pt.y, volW, volH, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            UpdateFlyoutAppearance(hwnd);
            
            SetTimer(hwnd, TIMER_VOLUME_FADE, 16, nullptr);
            SetTimer(hwnd, TIMER_VOLUME_SHOW, 100, nullptr);
            RedrawVolumeFlyout();
            return 0;
        }

        case WM_TIMER: {
            if (wp == TIMER_VOLUME_SHOW) {
                POINT pt; GetCursorPos(&pt);
                RECT winRc; GetWindowRect(hwnd, &winRc);
                if (PtInRect(&winRc, pt)) {
                    g_volumeFlyoutShowTimeLeft = g_Settings.volumeFlyoutDuration * 1000;
                } else {
                    if (g_volumeFlyoutShowTimeLeft >= 100) {
                        g_volumeFlyoutShowTimeLeft -= 100;
                    } else {
                        g_volumeFlyoutShowTimeLeft = 0;
                        g_volumeFlyoutState = 3; // FadeOut
                    }
                }
                RedrawVolumeFlyout();
            }
            else if (wp == TIMER_VOLUME_FADE) {
                if (g_volumeFlyoutState == 1) {
                    g_volumeFlyoutAlpha += 30;
                    if (g_volumeFlyoutAlpha >= 255) {
                        g_volumeFlyoutAlpha = 255;
                        g_volumeFlyoutState = 2;
                    }
                    RedrawVolumeFlyout();
                }
                else if (g_volumeFlyoutState == 3) {
                    g_volumeFlyoutAlpha -= 20;
                    if (g_volumeFlyoutAlpha <= 0) {
                        g_volumeFlyoutAlpha = 0;
                        g_volumeFlyoutState = 0;
                        g_volumeFlyoutIsDragging = false;
                        ReleaseCapture();
                        KillTimer(hwnd, TIMER_VOLUME_FADE);
                        KillTimer(hwnd, TIMER_VOLUME_SHOW);
                        ShowWindow(hwnd, SW_HIDE);
                    } else {
                        RedrawVolumeFlyout();
                    }
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK VisWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            RefreshSystemLightMode();
            UpdateAppearance(hwnd);
            int interval = max(16, 1000 / max(1, g_Settings.targetFPS));
            SetTimer(hwnd, IDT_RENDER, interval, nullptr);
            SetTimer(hwnd, IDT_RETRY,  600,      nullptr);
            SetTimer(hwnd, IDT_IDLE,   1000,     nullptr); // 1-second idle counter
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        // Ignore WM_CLOSE; allow shutdown only via Windhawk WM_APP
        case WM_CLOSE:
            // Ignore user close
            return 0;

        case WM_APP:
            // Windhawk uninit signal
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            WTSUnRegisterSessionNotification(hwnd);
            KillTimer(hwnd, IDT_RENDER);
            KillTimer(hwnd, IDT_RETRY);
            KillTimer(hwnd, IDT_IDLE);
            if (g_barHook) { UnhookWinEvent(g_barHook); g_barHook = nullptr; }
            g_hVisWnd = nullptr;
            PostThreadMessage(g_visThreadId, WM_QUIT, 0, 0);
            return 0;

        case WM_SETTINGCHANGE:
            RefreshSystemLightMode();
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        // Click-through: interactive for buttons/progress/media, transparent for visualizer
        case WM_NCHITTEST: {
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            ScreenToClient(hwnd, &pt);
            
            if (g_Settings.showMediaControls) {
                for (int i = 0; i < 3; i++) {
                    if (PtInRect(&g_buttons[i].rect, pt)) return HTCLIENT;
                }
            }
            
            RECT pbRect = GetProgressBarRect(hwnd);
            if (PtInRect(&pbRect, pt)) return HTCLIENT;
            
            if (g_Settings.showMediaInfo) {
                float mediaX, mediaW, visX, visW, ctrlX, ctrlW;
                RECT rc; GetClientRect(hwnd, &rc);
                CalculateLayout(rc.right, rc.bottom, mediaX, mediaW, visX, visW, ctrlX, ctrlW);
                if (mediaW > 0.0f) {
                    RECT mediaRc = { (long)mediaX, (long)(rc.top + 2), (long)(mediaX + mediaW), (long)(rc.bottom - 2) };
                    if (PtInRect(&mediaRc, pt)) return HTCLIENT;
                }
            }
            return HTTRANSPARENT;
        }

        case WM_MOUSEMOVE: {
            WakeUpAudio(hwnd);
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            if (g_isSeeking) {
                RECT pbRect = GetProgressBarRect(hwnd);
                float ratio = (float)(pt.x - pbRect.left) / (pbRect.right - pbRect.left);
                g_seekProgress = max(0.0f, min(1.0f, ratio));
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            
            if (!g_isTrackingMouse) {
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                g_isTrackingMouse = true;
            }
            bool changed = false;
            for (int i = 0; i < 3; i++) {
                bool hover = PtInRect(&g_buttons[i].rect, pt) != 0;
                if (g_buttons[i].isHovered != hover) {
                    g_buttons[i].isHovered = hover;
                    changed = true;
                }
            }
            if (changed) InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_MOUSELEAVE: {
            g_isTrackingMouse = false;
            bool changed = false;
            for (int i = 0; i < 3; i++) {
                if (g_buttons[i].isHovered || g_buttons[i].isPressed) {
                    g_buttons[i].isHovered = false;
                    g_buttons[i].isPressed = false;
                    changed = true;
                }
            }
            if (changed) InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            WakeUpAudio(hwnd);
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            
            RECT pbRect = GetProgressBarRect(hwnd);
            if (PtInRect(&pbRect, pt)) {
                g_isSeeking = true;
                SetCapture(hwnd);
                float ratio = (float)(pt.x - pbRect.left) / (pbRect.right - pbRect.left);
                g_seekProgress = max(0.0f, min(1.0f, ratio));
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            
            for (int i = 0; i < 3; i++) {
                if (PtInRect(&g_buttons[i].rect, pt)) {
                    g_buttons[i].isPressed = true;
                    InvalidateRect(hwnd, NULL, FALSE);
                    SetCapture(hwnd);
                    break;
                }
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            if (g_isSeeking) {
                ReleaseCapture();
                g_isSeeking = false;
                double duration = 0.0;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    duration = g_MediaState.durationSec;
                }
                if (duration > 0.0) {
                    double targetSec = g_seekProgress * duration;
                    {
                        lock_guard<mutex> guard(g_MediaState.lock);
                        g_MediaState.positionSec = targetSec;
                        g_MediaState.lastUpdateTick = GetTickCount64();
                    }
                    SeekToPosition(targetSec);
                }
                InvalidateRect(hwnd, NULL, FALSE);
                if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
                    RedrawMediaFlyout();
                }
                return 0;
            }
            
            ReleaseCapture();
            for (int i = 0; i < 3; i++) {
                if (g_buttons[i].isPressed) {
                    g_buttons[i].isPressed = false;
                    InvalidateRect(hwnd, NULL, FALSE);
                    if (PtInRect(&g_buttons[i].rect, pt)) {
                        if (g_buttons[i].id == 2) {
                            lock_guard<mutex> guard(g_MediaState.lock);
                            g_MediaState.isPlaying = !g_MediaState.isPlaying;
                            g_MediaState.lastUpdateTick = GetTickCount64();
                        }
                        SendMediaCommand(g_buttons[i].id);
                        TriggerInstantMediaUpdate();
                        if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
                            RedrawMediaFlyout();
                        }
                    }
                    break;
                }
            }
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            WakeUpAudio(hwnd);
            g_compactMode = !g_compactMode;
            RepositionWindow(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_MOUSEWHEEL: {
            WakeUpAudio(hwnd);
            short zDelta = GET_WHEEL_DELTA_WPARAM(wp);
            AdjustSystemVolume(zDelta > 0 ? 1 : -1);
            return 0;
        }

        case WM_POWERBROADCAST: {
            if (wp == PBT_APMSUSPEND) {
                g_systemSuspended.store(true, memory_order_relaxed);
                g_audioSuspended.store(true, memory_order_relaxed);
                KillTimer(hwnd, IDT_RENDER);
                Wh_Log(L"System suspend detected. Audio suspended and render timer killed.");
            } else if (wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMESUSPEND) {
                g_systemSuspended.store(false, memory_order_relaxed);
                g_audioSuspended.store(false, memory_order_relaxed);
                g_lowFPSMode.store(false, memory_order_relaxed);
                int interval = max(16, 1000 / max(1, g_Settings.targetFPS));
                SetTimer(hwnd, IDT_RENDER, interval, nullptr);
                Wh_Log(L"System resume detected. Audio resumed and render timer restarted.");
            }
            return TRUE;
        }

        case WM_WTSSESSION_CHANGE: {
            if (wp == 0x7 /* WTS_SESSION_LOCK */) {
                g_systemSuspended.store(true, memory_order_relaxed);
                g_audioSuspended.store(true, memory_order_relaxed);
                KillTimer(hwnd, IDT_RENDER);
                Wh_Log(L"Session lock detected. Audio suspended and render timer killed.");
            } else if (wp == 0x8 /* WTS_SESSION_UNLOCK */) {
                g_systemSuspended.store(false, memory_order_relaxed);
                g_audioSuspended.store(false, memory_order_relaxed);
                g_lowFPSMode.store(false, memory_order_relaxed);
                int interval = max(16, 1000 / max(1, g_Settings.targetFPS));
                SetTimer(hwnd, IDT_RENDER, interval, nullptr);
                Wh_Log(L"Session unlock detected. Audio resumed and render timer restarted.");
            }
            return 0;
        }

        case WM_APP + 25: {
            WakeUpAudio(hwnd);
            return 0;
        }

        case WM_TIMER:
            if (wp == IDT_RENDER) {
                ProcessSpectrum();
                UpdateVisibility(hwnd);
                
                // Silence detection / Audio Dynamic Sleep (ADS) Engine
                bool playing = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    playing = g_MediaState.isPlaying;
                }
                int sf = g_silenceFrames.load(memory_order_relaxed);
                if (!playing && sf > g_Settings.targetFPS * 15) { // 15 seconds of silence
                    if (!g_audioSuspended.load(memory_order_relaxed)) {
                        g_audioSuspended.store(true, memory_order_relaxed);
                        g_lowFPSMode.store(true, memory_order_relaxed);
                        SetTimer(hwnd, IDT_RENDER, 300, nullptr); // slow down rendering during sleep
                        Wh_Log(L"Audio entered dynamic sleep due to 15 seconds of silence.");
                    }
                }
                else if (!playing && sf > max(150, g_Settings.targetFPS * 5)) { // 5 seconds of silence
                    if (!g_lowFPSMode.load(memory_order_relaxed) && !g_audioSuspended.load(memory_order_relaxed)) {
                        g_lowFPSMode.store(true, memory_order_relaxed);
                        SetTimer(hwnd, IDT_RENDER, 150, nullptr);
                    }
                }
                
                if (IsWindowVisible(hwnd)) {
                    RECT rc; GetClientRect(hwnd, &rc);
                    float mediaX, mediaW, visX, visW, ctrlX, ctrlW;
                    CalculateLayout(rc.right, rc.bottom, mediaX, mediaW, visX, visW, ctrlX, ctrlW);
                    
                    // Region Invalidation: only invalidate active regions
                    if (visW > 0.0f) {
                        RECT visRc = { (long)visX - 2, 0, (long)(visX + visW) + 2, rc.bottom };
                        InvalidateRect(hwnd, &visRc, FALSE);
                    }
                    if (mediaW > 0.0f && (playing || g_scrollOffset > 0.0f)) {
                        RECT mediaRc = { (long)mediaX - 2, 0, (long)(mediaX + mediaW) + 2, rc.bottom };
                        InvalidateRect(hwnd, &mediaRc, FALSE);
                    }
                }
            }
            else if (wp == IDT_IDLE) {
                // Increment idle counter every second while media is not playing
                bool playing = false;
                { lock_guard<mutex> guard(g_MediaState.lock); playing = g_MediaState.isPlaying; }
                if (playing) {
                    g_idleSeconds.store(0, memory_order_relaxed);
                } else {
                    int cur = g_idleSeconds.load(memory_order_relaxed);
                    if (g_Settings.idleTimeout > 0 && cur < g_Settings.idleTimeout + 5)
                        g_idleSeconds.store(cur + 1, memory_order_relaxed);
                }
            }
            else if (wp == IDT_RETRY) {
                KillTimer(hwnd, IDT_RETRY);
                HWND hBar = FindWindow(L"Shell_TrayWnd", nullptr);
                if (hBar && IsWindowVisible(hBar)) {
                    if (!g_barHook) RegisterBarHook(hwnd);
                    PostMessage(hwnd, WM_APP + 20, 0, 0);
                } else {
                    SetTimer(hwnd, IDT_RETRY, 600, nullptr);
                }
            }
            return 0;

        case WM_APP + 20:
            RepositionWindow(hwnd);
            return 0;

        case WM_APP + 21:
            // Settings changed: reload on panel thread, refresh timer interval and window position
            LoadSettings();
            KillTimer(hwnd, IDT_RENDER);
            SetTimer(hwnd, IDT_RENDER, max(16, 1000 / max(1, g_Settings.targetFPS)), nullptr);
            UpdateAppearance(hwnd);
            RepositionWindow(hwnd);
            return 0;

        // WM_PAINT: Double-buffered draw
        // No WS_EX_LAYERED - DWM acrylic background is applied behind the window.
        // Content drawn in memDC composites over the DWM backdrop.
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            int W = rc.right, H = rc.bottom;

            if (W > 0 && H > 0) {
                int clipX = ps.rcPaint.left;
                int clipY = ps.rcPaint.top;
                int clipW = ps.rcPaint.right - ps.rcPaint.left;
                int clipH = ps.rcPaint.bottom - ps.rcPaint.top;

                if (clipW > 0 && clipH > 0) {
                    HDC     memDC  = CreateCompatibleDC(hdc);
                    
                    BITMAPINFO bmi = {};
                    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmi.bmiHeader.biWidth = W;
                    bmi.bmiHeader.biHeight = -H;
                    bmi.bmiHeader.biPlanes = 1;
                    bmi.bmiHeader.biBitCount = 32;
                    bmi.bmiHeader.biCompression = BI_RGB;
                    
                    void* pBits = nullptr;
                    HBITMAP memBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
                    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

                    {
                        Graphics g(memDC);
                        g.Clear(Color(0, 0, 0, 0));
                        g.SetClip(Rect(clipX, clipY, clipW, clipH));
                    }

                    DrawPanel(memDC, W, H);
                    BitBlt(hdc, clipX, clipY, clipW, clipH, memDC, clipX, clipY, SRCCOPY);

                    SelectObject(memDC, oldBmp);
                    DeleteObject(memBmp);
                    DeleteDC(memDC);
                }
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        default:
            if (g_barCreated != 0 && msg == g_barCreated) {
                if (g_barHook) { UnhookWinEvent(g_barHook); g_barHook = nullptr; }
                RegisterBarHook(hwnd);
                return 0;
            }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// =========================================================================
// MAIN THREAD
// =========================================================================
static void CleanupModGdiplusResources() {
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        if (g_MediaState.albumArt) {
            delete g_MediaState.albumArt;
            g_MediaState.albumArt = nullptr;
        }
    }
    ClearAppIconCache();
    if (g_cachedAlbumArt)       { delete g_cachedAlbumArt;       g_cachedAlbumArt = nullptr; }
    if (g_flyoutCachedAlbumArt) { delete g_flyoutCachedAlbumArt; g_flyoutCachedAlbumArt = nullptr; }
    {
        lock_guard<mutex> lk(g_particleMutex);
        g_particles.clear();
    }
}

static void HookThreadProc() {
    g_hookThreadId = GetCurrentThreadId();
    HMODULE hMod = GetOwnModuleHandle();
    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
    if (!g_keyboardHook) {
        Wh_Log(L"Failed to register keyboard hook. Error: %d", GetLastError());
    } else {
        Wh_Log(L"Keyboard hook registered successfully on dedicated thread. hMod=%p", hMod);
    }

    MSG msg;
    while (!g_uninitRequested.load(memory_order_relaxed) && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    g_hookThreadId = 0;
}

static void VisThread() {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Wait for taskbar window
    HWND hBar = nullptr;
    for (int i = 0; i < 50 && !g_uninitRequested.load(memory_order_relaxed); i++) {
        hBar = FindWindow(L"Shell_TrayWnd", nullptr);
        if (hBar) break;
        Sleep(100);
    }

    if (g_uninitRequested.load(memory_order_relaxed)) {
        CoUninitialize();
        g_threadFinished = true;
        return;
    }

    if (hBar) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hBar, &pid);
        if (pid != GetCurrentProcessId()) {
            Wh_Log(L"Shell_TrayWnd belongs to different PID. Exiting.");
            CoUninitialize();
            g_threadFinished = true;
            return;
        }
    } else {
        Wh_Log(L"Shell_TrayWnd not found. Exiting.");
        CoUninitialize();
        g_threadFinished = true;
        return;
    }

    // Initialize GDI+
    GdiplusStartupInput gsi;
    ULONG_PTR tok = 0;
    if (GdiplusStartup(&tok, &gsi, nullptr) != Ok) {
        Wh_Log(L"GdiplusStartup failed.");
        CoUninitialize();
        g_threadFinished = true;
        return;
    }

    HINSTANCE hOwnInst = GetOwnModuleHandle();

    // Register window classes under the mod's own HINSTANCE (fatal on any failure)
    static const WCHAR* WNDCLASS_NAME = L"WindhawkAudioMediaSuite_v7_0_0";
    WNDCLASSW wc     = {};
    wc.lpfnWndProc   = VisWndProc;
    wc.hInstance     = hOwnInst;
    wc.lpszClassName = WNDCLASS_NAME;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.style         = CS_DBLCLKS;
    wc.hbrBackground = nullptr;

    WNDCLASSW wcMedia = {};
    wcMedia.lpfnWndProc = MediaFlyoutWndProc;
    wcMedia.hInstance = hOwnInst;
    wcMedia.lpszClassName = L"WindhawkMediaFlyout_v7";
    wcMedia.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcMedia.style = CS_DBLCLKS;
    wcMedia.hbrBackground = nullptr;

    WNDCLASSW wcVolume = {};
    wcVolume.lpfnWndProc = VolumeFlyoutWndProc;
    wcVolume.hInstance = hOwnInst;
    wcVolume.lpszClassName = L"WindhawkVolumeFlyout_v7";
    wcVolume.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcVolume.style = 0;
    wcVolume.hbrBackground = nullptr;

    bool regVis = RegisterClassW(&wc) != 0;
    bool regMedia = regVis && (RegisterClassW(&wcMedia) != 0);
    bool regVol = regMedia && (RegisterClassW(&wcVolume) != 0);
    if (!regVol) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        if (regMedia) UnregisterClassW(L"WindhawkMediaFlyout_v7", hOwnInst);
        if (regVis) UnregisterClassW(WNDCLASS_NAME, hOwnInst);
        GdiplusShutdown(tok);
        CoUninitialize();
        g_threadFinished = true;
        return;
    }

    // IMPORTANT: No WS_EX_LAYERED on main bar - DWM acrylic handles compositing.
    // Combining WS_EX_LAYERED with acrylic breaks Explorer DWM compositing.
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE;

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand = nullptr;
    if (hUser32) {
        CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(hUser32, "CreateWindowInBand");
    }

    // Create hidden window
    if (CreateWindowInBand) {
        g_hVisWnd = CreateWindowInBand(
            exStyle, WNDCLASS_NAME, L"AudioMediaSuiteWnd",
            WS_POPUP,
            0, 0, g_Settings.panelW, g_Settings.panelH,
            nullptr, nullptr, hOwnInst, nullptr,
            ZBID_IMMERSIVE_NOTIFICATION);
        if (g_hVisWnd) {
            Wh_Log(L"Created window in ZBID_IMMERSIVE_NOTIFICATION band");
            WTSRegisterSessionNotification(g_hVisWnd, 0);
        }
    }

    if (!g_hVisWnd) {
        Wh_Log(L"Falling back to CreateWindowEx");
        g_hVisWnd = CreateWindowExW(
            exStyle, WNDCLASS_NAME, L"AudioMediaSuiteWnd",
            WS_POPUP,
            0, 0, g_Settings.panelW, g_Settings.panelH,
            nullptr, nullptr, hOwnInst, nullptr);
        if (g_hVisWnd) {
            WTSRegisterSessionNotification(g_hVisWnd, 0);
        }
    }

    if (!g_hVisWnd) {
        Wh_Log(L"CreateWindow failed: %u", GetLastError());
        CleanupModGdiplusResources();
        GdiplusShutdown(tok);
        UnregisterClassW(WNDCLASS_NAME, hOwnInst);
        UnregisterClassW(L"WindhawkMediaFlyout_v7", hOwnInst);
        UnregisterClassW(L"WindhawkVolumeFlyout_v7", hOwnInst);
        CoUninitialize();
        g_threadFinished = true;
        return;
    }

    // Create flyouts hidden
    g_hMediaFlyoutWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        L"WindhawkMediaFlyout_v7", L"WindhawkMediaFlyout",
        WS_POPUP,
        0, 0, g_Settings.mediaFlyoutW, g_Settings.mediaFlyoutH,
        nullptr, nullptr, hOwnInst, nullptr);
    if (g_hMediaFlyoutWnd) {
        UpdateLayeredWindowHelper(g_hMediaFlyoutWnd, 0, 1);
        MediaFlyoutLayout l = CalculateMediaFlyoutLayout(g_Settings.mediaFlyoutW, g_Settings.mediaFlyoutH);
        g_mediaFlyoutButtons[0] = { 1, l.rectBtn1, false, false };
        g_mediaFlyoutButtons[1] = { 2, l.rectBtn2, false, false };
        g_mediaFlyoutButtons[2] = { 3, l.rectBtn3, false, false };
        Wh_Log(L"Media Flyout window created successfully. HWND=%p", g_hMediaFlyoutWnd);
    } else {
        Wh_Log(L"Failed to create Media Flyout window. Error: %d", GetLastError());
    }

    g_hVolumeFlyoutWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        L"WindhawkVolumeFlyout_v7", L"WindhawkVolumeFlyout",
        WS_POPUP,
        0, 0, g_Settings.volumeFlyoutW, g_Settings.volumeFlyoutH,
        nullptr, nullptr, hOwnInst, nullptr);
    if (g_hVolumeFlyoutWnd) {
        UpdateLayeredWindowHelper(g_hVolumeFlyoutWnd, 0, 2);
        Wh_Log(L"Volume Flyout window created successfully. HWND=%p", g_hVolumeFlyoutWnd);
    } else {
        Wh_Log(L"Failed to create Volume Flyout window. Error: %d", GetLastError());
    }

    // Save thread ID
    g_visThreadId = GetCurrentThreadId();

    if (!g_uninitRequested.load(memory_order_relaxed)) {
        // Start worker threads
        g_mediaRunning = true;
        g_mediaThread.emplace(MediaUpdateThread);

        g_audioRunning = true;
        g_audioThread.emplace(AudioCaptureThread);

        g_hookThread.emplace(HookThreadProc);

        // Register volume callback
        RegisterVolumeNotification(g_hVisWnd);

        // Message loop
        MSG msg;
        while (!g_uninitRequested.load(memory_order_relaxed) && GetMessage(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Cleanup
    if (g_hookThreadId != 0) {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread && g_hookThread->joinable()) {
        g_hookThread->join();
    }
    g_hookThread.reset();

    UnregisterVolumeNotification();

    g_audioRunning = false;
    if (g_audioThread && g_audioThread->joinable()) {
        g_audioThread->join();
    }
    g_audioThread.reset();

    g_mediaRunning = false;
    g_mediaUpdateCv.notify_all();
    if (g_mediaThread && g_mediaThread->joinable()) {
        g_mediaThread->join();
    }
    g_mediaThread.reset();

    if (g_hVisWnd && IsWindow(g_hVisWnd)) {
        DestroyWindow(g_hVisWnd);
        g_hVisWnd = nullptr;
    }
    if (g_hMediaFlyoutWnd && IsWindow(g_hMediaFlyoutWnd)) {
        DestroyWindow(g_hMediaFlyoutWnd);
        g_hMediaFlyoutWnd = nullptr;
    }
    if (g_hVolumeFlyoutWnd && IsWindow(g_hVolumeFlyoutWnd)) {
        DestroyWindow(g_hVolumeFlyoutWnd);
        g_hVolumeFlyoutWnd = nullptr;
    }

    UnregisterClassW(WNDCLASS_NAME, hOwnInst);
    UnregisterClassW(L"WindhawkMediaFlyout_v7", hOwnInst);
    UnregisterClassW(L"WindhawkVolumeFlyout_v7", hOwnInst);

    CleanupModGdiplusResources();
    GdiplusShutdown(tok);
    CoUninitialize();
    g_threadFinished = true;
}

// =========================================================================
// WINDHAWK CALLBACKS
// =========================================================================
BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0)
        return FALSE;

    // Filter out separate/factory explorer processes
    int    argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (_wcsicmp(argv[i], L"/separate") == 0 ||
                _wcsnicmp(argv[i], L"/factory", 8) == 0) {
                LocalFree(argv);
                return FALSE;
            }
        }
        LocalFree(argv);
    }

    g_barCreated = RegisterWindowMessage(L"TaskbarCreated");
    LoadSettings();

    g_uninitRequested = false;
    g_threadFinished  = false;
    g_hVisWnd         = nullptr;
    g_visThreadId     = 0;
    g_idleSeconds.store(0, memory_order_relaxed);
    g_silenceFrames.store(0, memory_order_relaxed);

    g_visThread.emplace(VisThread);
    return TRUE;
}

void Wh_ModUninit() {
    g_uninitRequested = true;
    g_audioRunning = false;
    g_mediaRunning = false;
    g_mediaUpdateCv.notify_all();

    if (g_hookThreadId != 0) {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
    }

    if (g_hVisWnd && IsWindow(g_hVisWnd)) {
        PostMessage(g_hVisWnd, WM_APP, 0, 0);
    } else if (g_visThreadId != 0) {
        PostThreadMessage(g_visThreadId, WM_QUIT, 0, 0);
    }

    if (g_visThread) {
        for (int i = 0; i < 60 && !g_threadFinished.load(memory_order_relaxed); i++) {
            if (g_hVisWnd && IsWindow(g_hVisWnd)) {
                PostMessage(g_hVisWnd, WM_APP, 0, 0);
            } else if (g_visThreadId != 0) {
                PostThreadMessage(g_visThreadId, WM_QUIT, 0, 0);
            }
            Sleep(50);
        }
        if (g_visThread->joinable()) g_visThread->join();
        g_visThread.reset();
    }

    {
        lock_guard<mutex> lk(g_sessionMgrMutex);
        g_SessionManager = nullptr;
    }

    g_visThreadId = 0;
}

void Wh_ModSettingsChanged() {
    if (g_hVisWnd && IsWindow(g_hVisWnd)) {
        PostMessage(g_hVisWnd, WM_APP + 21, 0, 0);
    }
}
