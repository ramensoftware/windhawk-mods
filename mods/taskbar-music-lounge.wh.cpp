// ==WindhawkMod==
// @id              taskbar-music-lounge
// @name            Taskbar Music Lounge
// @description     A sleek, modern music widget with fluid animations, custom transitions, and media controls.
// @version         4.0
// @author          Hashah2311
// @github          https://github.com/Hashah2311
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Lounge

A sleek, modern media widget that sits on your taskbar with fluid animations, advanced transitions, and a native Windows 11 aesthetic. 

## ✨ Features
* **Fluent Design Engine:** Native Windows 11 hardware-accelerated rounding, acrylic blur, and Segoe Fluent Icons.
* **Transition Effects:** Multiple custom track switch animations (Slide, Zoom, Crossfade).
* **Dynamic Accent Effects:** Smart extraction pulls the most vibrant dominant color from album art to drive deep visual effects (Pulse Ring, Neon, Frost, etc.).
* **Auto-Resizing:** The panel gracefully and smoothly animates its width to fit the track info.
* **Two-Line Display:** Bold Title + Artist text with automatic fade-edge smooth scrolling.
* **Universal Support:** Instant synchronization with any Windows media player via Event-Driven GSMTC (0% idle CPU).
* **Smart Controls:** Auto-Hide smoothly fades controls on hover, and volume scroll adjusts system volume natively.

## ⚠️ Requirements
* **Disable Widgets:** Taskbar Settings -> Widgets -> Off.
* **Windows 11:** Required for rounded corners, acrylic blur, and full styling capabilities.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Dimensions:
  - PanelWidth: 100
    $name: Base Panel Width
    $description: Default width of the music widget in pixels.
  - AutoWidth: true
    $name: Auto Width
    $description: Automatically adjust panel width based on song title and artist.
  - MaxWidth: 300
    $name: Max Width
    $description: Maximum panel width when Auto Width is enabled.
  - PanelHeight: 46
    $name: Panel Height
    $description: Height of the music widget in pixels.
- Positioning:
  - OffsetX: 12
    $name: X Offset
    $description: Horizontal position offset from the left edge of the taskbar.
  - OffsetY: 0
    $name: Y Offset
    $description: Vertical position offset from the center of the taskbar.
- Typography:
  - FontFamily: Segoe UI
    $name: Font Family
    $description: Font used for track title and artist name.
    $options:
    - Segoe UI Variable Display: Segoe UI Variable Display
    - Segoe UI: Segoe UI
    - Segoe UI Variable: Segoe UI Variable
    - Segoe UI Variable Small: Segoe UI Variable Small
    - Cascadia Code: Cascadia Code
    - Cascadia Mono: Cascadia Mono
    - Consolas: Consolas
    - Arial: Arial
    - Calibri: Calibri
    - Cambria: Cambria
    - Candara: Candara
    - Century Gothic: Century Gothic
    - Comic Sans MS: Comic Sans MS
    - Corbel: Corbel
    - Courier New: Courier New
    - Franklin Gothic Medium: Franklin Gothic Medium
    - Garamond: Garamond
    - Georgia: Georgia
    - Impact: Impact
    - Lucida Console: Lucida Console
    - Lucida Sans Unicode: Lucida Sans Unicode
    - Palatino Linotype: Palatino Linotype
    - Tahoma: Tahoma
    - Times New Roman: Times New Roman
    - Trebuchet MS: Trebuchet MS
    - Verdana: Verdana
  - FontSize: 16
    $name: Title Font Size
    $description: Text size for the track title (in pixels).
  - ArtistFontSize: 12
    $name: Artist Font Size
    $description: Text size for the artist name (in pixels).
  - TextOffsetY: -2
    $name: Text Y Offset
    $description: >-
      Vertical offset for the text in pixels.
      Positive values move text down, negative values move text up.
- Appearance:
  - AutoTheme: true
    $name: Auto Theme
    $description: >-
      Automatically match Windows light/dark mode.
      When enabled, text color and acrylic tint adjust to your system theme.
      Disable this to use a manual text color instead.
  - TextColor: 0xFFFFFF
    $name: Manual Text Color
    $description: >-
      Hex color code for the text (e.g. FFFFFF for white, 000000 for black).
      Only used when Auto Theme is disabled.
  - BgOpacity: 0
    $name: Acrylic Tint Opacity
    $description: >-
      Controls how opaque the acrylic glass tint is (0-255).
      Keep at 0 for a clean, pure glass effect.
      Higher values make the background more solid.
- Accent:
  - AccentMode: auto
    $name: Accent Color Source
    $description: >-
      Where to source the accent color for visual effects.
    $options:
    - auto: Auto (extracted from album art)
    - theme: System Theme
    - manual: Manual (custom color)
  - AccentColor: 0x1DB954
    $name: Manual Accent Color
    $description: >-
      Hex color code for the accent (e.g. 1DB954 for Spotify green).
      Only used when Accent Color Source is set to Manual.
  - AccentStyle: ambient
    $name: Accent Style
    $description: >-
      Visual style for how the accent color is applied to the widget.
    $options:
    - none: None (no accent effect)
    - glow_border: Glow Border (animated pulsing border)
    - ambient: Ambient (soft color wash over background)
    - edge_light: Edge Light (vertical bar on the left edge)
    - underglow: Underglow (bloom beneath the panel)
    - neon: Neon (double-outline neon tube effect)
    - spectrum: Spectrum (rainbow hue-cycling border)
    - pulse_ring: Pulse Ring (expanding circle from album art)
    - corner_glow: Corner Glow (glowing corners)
    - frost_edge: Frost Edge (top-down frost gradient)
  - AccentStrength: neon
    $name: Accent Strength
    $description: >-
      Controls the overall look and feel of the accent color effect.
    $options:
    - subtle: Subtle (soft and understated)
    - default: Default (balanced look)
    - vivid: Vivid (rich and saturated)
    - neon: Neon (intense and electric)
    - pastel: Pastel (light and soft)
    - muted: Muted (dark and subdued)
- Behavior:
  - AnimationSpeed: dynamic
    $name: Animation Speed
    $description: Controls the speed of all widget animations.
    $options:
    - subtle: Subtle (slow and gentle)
    - smooth: Smooth (balanced)
    - dynamic: Dynamic (fast and snappy)
  - TransitionStyle: crossfade
    $name: Transition Style
    $description: Animation style when changing songs.
    $options:
    - crossfade: Crossfade
    - slide_left: Slide Left
    - slide_right: Slide Right
    - slide_up: Slide Up
    - slide_down: Slide Down
    - zoom_in: Zoom In
    - zoom_out: Zoom Out
  - AutoHideControls: true
    $name: Auto-Hide Controls
    $description: >-
      When enabled, media controls are hidden by default and
      smoothly fade in when you hover over the widget.
      Disable to always show controls.
  - AlwaysOnTop: false
    $name: Always on Top
    $description: >-
      Keep the widget above all other windows.
      Disable this if the widget covers fullscreen games or apps.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <string>
#include <thread>
#include <mutex>
#include <cstdio>
#include <cmath>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

// --- Constants ---
const WCHAR* ICON_FONT_NAME  = L"Segoe Fluent Icons";
WCHAR g_FontName[128] = L"Segoe UI Variable Display";

// Segoe Fluent Icons codepoints
const WCHAR ICON_PREV[]      = L"\xE892";
const WCHAR ICON_PLAY[]      = L"\xE768";
const WCHAR ICON_PAUSE[]     = L"\xE769";
const WCHAR ICON_NEXT[]      = L"\xE893";

// --- DWM API ---
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;
typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;
typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// --- Z-Band API ---
enum ZBID {
    ZBID_DEFAULT = 0, ZBID_DESKTOP = 1, ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3, ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5, ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7, ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9, ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11, ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13, ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15, ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17, ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam,
    DWORD dwBand
);
typedef BOOL(WINAPI* pSetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);
typedef BOOL(WINAPI* pGetWindowBand)(HWND hWnd, PDWORD pdwBand);

// ============================================================================
// Animation Engine
// ============================================================================

struct AnimFloat {
    float current = 0.0f;
    float target  = 0.0f;
    float speed   = 8.0f;

    void Tick(float dt) {
        float diff = target - current;
        current += diff * min(1.0f, speed * dt);
        if (fabsf(diff) < 0.001f) current = target;
    }
    bool Done() const { return fabsf(current - target) < 0.5f; }
    void Set(float v) { current = target = v; }
    void SetTarget(float v) { target = v; }
};

// ============================================================================
// Configurable State
// ============================================================================

struct ModSettings {
    int width       = 280;
    int height      = 48;
    int fontSize    = 12;
    int artistFontSize = 11;
    int offsetX     = 12;
    int offsetY     = 0;
    int textOffsetY = 0;
    bool autoTheme  = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    int bgOpacity   = 0;
    int animSpeed   = 2;      // 1=Subtle, 2=Smooth, 3=Dynamic
    int accentMode  = 0;      // 0=Auto, 1=Theme, 2=Manual
    DWORD accentColor = 0xFF1DB954;
    int accentStyle = 1;      // 0=None, 1-9 for each style
    int accentIntensity = 100; // 0-200%
    int accentSaturation = 100; // 0-200%
    int accentBrightness = 100; // 0-200%
    int transitionStyle = 0;    // 0=fade, 1=slideL, 2=slideR, 3=slideU, 4=slideD, 5=zoomIn, 6=zoomOut
    bool autoHideControls = true;
    bool alwaysOnTop = true;
    bool autoWidth  = false;
    int maxWidth    = 400;
    int currentWidth = 280;
} g_Settings;

// ============================================================================
// Global State
// ============================================================================

HWND g_hMediaWindow = NULL;
bool g_Running = true;
int  g_HoverState = 0;
pSetWindowBand g_SetWindowBand = nullptr;
bool g_CurrentTopmost = true; // tracks current z-band state

// Animation values
AnimFloat g_HoverOpacity[4];    // index 1-3 for prev/play/next
AnimFloat g_ContentOpacity;     // crossfade on track change
AnimFloat g_AccentGlowOpacity;  // accent glow border animated opacity
AnimFloat g_AccentPulse;        // subtle pulse while playing
AnimFloat g_ScrollPos;          // smooth text scroll position
AnimFloat g_ControlsVisibility; // controls fade in/out on panel hover
bool g_PanelHovered = false;
float g_AnimSpeedMultiplier = 1.0f;

// New Animations for Smooth Transitions
AnimFloat g_WidthAnim;          // smooth width transition
AnimFloat g_CrossfadeAnim;      // crossfade between old and new song

// Scroll state
int   g_TextWidth       = 0;
bool  g_IsScrolling     = false;
int   g_ScrollPauseMs   = 0;
bool  g_ScrollPaused    = true;
const int SCROLL_PAUSE_DURATION = 2000; // 2 seconds pause at edges

// Track change detection
wstring g_LastTrackTitle = L"";

// Pulse direction
bool g_PulseRising = true;

// Animation timer for time-based effects (Spectrum, Pulse Ring)
float g_AnimTimer = 0.0f;

// Color Helpers
Color LerpColor(Color c1, Color c2, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return Color(
        (BYTE)(c1.GetAlpha() + (c2.GetAlpha() - c1.GetAlpha()) * t),
        (BYTE)(c1.GetRed()   + (c2.GetRed()   - c1.GetRed()) * t),
        (BYTE)(c1.GetGreen() + (c2.GetGreen() - c1.GetGreen()) * t),
        (BYTE)(c1.GetBlue()  + (c2.GetBlue()  - c1.GetBlue()) * t)
    );
}

// Accent colors
Color g_ExtractedAccent(255, 29, 185, 84); // Default green
Color g_OldAccent(255, 29, 185, 84);

// Data Model
struct MediaState {
    wstring title  = L"Waiting for media...";
    wstring artist = L"";
    bool isPlaying = false;
    bool hasMedia  = false;
    Bitmap* albumArt = nullptr;
    mutex lock;
} g_MediaState;

MediaState g_OldMediaState;

// ============================================================================
// Settings
// ============================================================================

void LoadSettings() {
    g_Settings.width     = Wh_GetIntSetting(L"Dimensions.PanelWidth");
    g_Settings.height    = Wh_GetIntSetting(L"Dimensions.PanelHeight");
    g_Settings.autoWidth = Wh_GetIntSetting(L"Dimensions.AutoWidth") != 0;
    g_Settings.maxWidth  = Wh_GetIntSetting(L"Dimensions.MaxWidth");
    if (g_Settings.maxWidth < g_Settings.width) g_Settings.maxWidth = g_Settings.width;
    
    // Only initialize it once, otherwise let the animation handle it
    if (g_Settings.currentWidth == 0) {
        g_Settings.currentWidth = g_Settings.width;
    }
    
    g_Settings.fontSize  = Wh_GetIntSetting(L"Typography.FontSize");
    g_Settings.artistFontSize = Wh_GetIntSetting(L"Typography.ArtistFontSize");
    if (g_Settings.artistFontSize < 6) g_Settings.artistFontSize = 11;
    g_Settings.offsetX   = Wh_GetIntSetting(L"Positioning.OffsetX");
    g_Settings.offsetY   = Wh_GetIntSetting(L"Positioning.OffsetY");
    g_Settings.textOffsetY = Wh_GetIntSetting(L"Typography.TextOffsetY");
    g_Settings.autoTheme = Wh_GetIntSetting(L"Appearance.AutoTheme") != 0;

    // Font Family
    PCWSTR fontFamilyStr = Wh_GetStringSetting(L"Typography.FontFamily");
    if (fontFamilyStr && wcslen(fontFamilyStr) > 0) {
        wcsncpy_s(g_FontName, fontFamilyStr, _TRUNCATE);
    } else {
        wcscpy_s(g_FontName, L"Segoe UI");
    }
    if (fontFamilyStr) Wh_FreeStringSetting(fontFamilyStr);

    PCWSTR textHex = Wh_GetStringSetting(L"Appearance.TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) {
        if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
        Wh_FreeStringSetting(textHex);
    }
    g_Settings.manualTextColor = 0xFF000000 | textRGB;

    g_Settings.bgOpacity = Wh_GetIntSetting(L"Appearance.BgOpacity");
    if (g_Settings.bgOpacity < 0) g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255) g_Settings.bgOpacity = 255;

    if (g_Settings.width < 100) g_Settings.width = 100;
    if (g_Settings.height < 32) g_Settings.height = 46;

    // Animation Speed (dropdown)
    PCWSTR animSpeedStr = Wh_GetStringSetting(L"Behavior.AnimationSpeed");
    g_Settings.animSpeed = 3; // default: dynamic
    if (animSpeedStr) {
        if (wcscmp(animSpeedStr, L"subtle") == 0) g_Settings.animSpeed = 1;
        else if (wcscmp(animSpeedStr, L"smooth") == 0) g_Settings.animSpeed = 2;
        else if (wcscmp(animSpeedStr, L"dynamic") == 0) g_Settings.animSpeed = 3;
        Wh_FreeStringSetting(animSpeedStr);
    }

    // Accent Color Mode (dropdown)
    PCWSTR accentModeStr = Wh_GetStringSetting(L"Accent.AccentMode");
    g_Settings.accentMode = 0; // default: auto
    if (accentModeStr) {
        if (wcscmp(accentModeStr, L"auto") == 0) g_Settings.accentMode = 0;
        else if (wcscmp(accentModeStr, L"theme") == 0) g_Settings.accentMode = 1;
        else if (wcscmp(accentModeStr, L"manual") == 0) g_Settings.accentMode = 2;
        Wh_FreeStringSetting(accentModeStr);
    }

    PCWSTR accentHex = Wh_GetStringSetting(L"Accent.AccentColor");
    DWORD accentRGB = 0x1DB954;
    if (accentHex) {
        if (wcslen(accentHex) > 0) accentRGB = wcstoul(accentHex, nullptr, 16);
        Wh_FreeStringSetting(accentHex);
    }
    g_Settings.accentColor = 0xFF000000 | accentRGB;

    // Accent Style (dropdown)
    PCWSTR accentStyleStr = Wh_GetStringSetting(L"Accent.AccentStyle");
    g_Settings.accentStyle = 2; // default: ambient
    if (accentStyleStr) {
        if (wcscmp(accentStyleStr, L"none") == 0) g_Settings.accentStyle = 0;
        else if (wcscmp(accentStyleStr, L"glow_border") == 0) g_Settings.accentStyle = 1;
        else if (wcscmp(accentStyleStr, L"ambient") == 0) g_Settings.accentStyle = 2;
        else if (wcscmp(accentStyleStr, L"edge_light") == 0) g_Settings.accentStyle = 3;
        else if (wcscmp(accentStyleStr, L"underglow") == 0) g_Settings.accentStyle = 4;
        else if (wcscmp(accentStyleStr, L"neon") == 0) g_Settings.accentStyle = 5;
        else if (wcscmp(accentStyleStr, L"spectrum") == 0) g_Settings.accentStyle = 6;
        else if (wcscmp(accentStyleStr, L"pulse_ring") == 0) g_Settings.accentStyle = 7;
        else if (wcscmp(accentStyleStr, L"corner_glow") == 0) g_Settings.accentStyle = 8;
        else if (wcscmp(accentStyleStr, L"frost_edge") == 0) g_Settings.accentStyle = 9;
        Wh_FreeStringSetting(accentStyleStr);
    }
    // Transition Style
    PCWSTR transStr = Wh_GetStringSetting(L"Behavior.TransitionStyle");
    g_Settings.transitionStyle = 0;
    if (transStr) {
        if (wcscmp(transStr, L"slide_left") == 0) g_Settings.transitionStyle = 1;
        else if (wcscmp(transStr, L"slide_right") == 0) g_Settings.transitionStyle = 2;
        else if (wcscmp(transStr, L"slide_up") == 0) g_Settings.transitionStyle = 3;
        else if (wcscmp(transStr, L"slide_down") == 0) g_Settings.transitionStyle = 4;
        else if (wcscmp(transStr, L"zoom_in") == 0) g_Settings.transitionStyle = 5;
        else if (wcscmp(transStr, L"zoom_out") == 0) g_Settings.transitionStyle = 6;
        Wh_FreeStringSetting(transStr);
    }

    g_Settings.autoHideControls = Wh_GetIntSetting(L"Behavior.AutoHideControls") != 0;
    g_Settings.alwaysOnTop = Wh_GetIntSetting(L"Behavior.AlwaysOnTop") != 0;

    // Accent Strength (dropdown → mapped to intensity/saturation/brightness)
    PCWSTR strengthStr = Wh_GetStringSetting(L"Accent.AccentStrength");
    // Defaults ("neon" preset)
    g_Settings.accentIntensity = 200;
    g_Settings.accentSaturation = 200;
    g_Settings.accentBrightness = 150;
    if (strengthStr) {
        if (wcscmp(strengthStr, L"subtle") == 0) {
            g_Settings.accentIntensity = 80; g_Settings.accentSaturation = 90; g_Settings.accentBrightness = 100;
        } else if (wcscmp(strengthStr, L"vivid") == 0) {
            g_Settings.accentIntensity = 160; g_Settings.accentSaturation = 180; g_Settings.accentBrightness = 120;
        } else if (wcscmp(strengthStr, L"neon") == 0) {
            g_Settings.accentIntensity = 200; g_Settings.accentSaturation = 200; g_Settings.accentBrightness = 150;
        } else if (wcscmp(strengthStr, L"pastel") == 0) {
            g_Settings.accentIntensity = 120; g_Settings.accentSaturation = 40; g_Settings.accentBrightness = 180;
        } else if (wcscmp(strengthStr, L"muted") == 0) {
            g_Settings.accentIntensity = 90; g_Settings.accentSaturation = 60; g_Settings.accentBrightness = 70;
        }
        // "default" keeps the 100/100/100 values
        Wh_FreeStringSetting(strengthStr);
    }

    // Animation speed multiplier
    switch (g_Settings.animSpeed) {
        case 1: g_AnimSpeedMultiplier = 0.5f; break;
        case 2: g_AnimSpeedMultiplier = 1.0f; break;
        case 3: g_AnimSpeedMultiplier = 1.8f; break;
    }

    // Update animation speeds
    float baseSpeed = 8.0f * g_AnimSpeedMultiplier;
    for (int i = 0; i < 4; i++) g_HoverOpacity[i].speed = baseSpeed;
    g_ContentOpacity.speed = 6.0f * g_AnimSpeedMultiplier;
    g_WidthAnim.speed = 12.0f * g_AnimSpeedMultiplier;
    g_CrossfadeAnim.speed = 5.0f * g_AnimSpeedMultiplier; // Slower crossfade
    g_AccentGlowOpacity.speed = 4.0f * g_AnimSpeedMultiplier;
    g_AccentPulse.speed = 1.5f * g_AnimSpeedMultiplier;
    g_ScrollPos.speed = 2.0f * g_AnimSpeedMultiplier;
    g_ControlsVisibility.speed = 10.0f * g_AnimSpeedMultiplier;
}

// ============================================================================
// WinRT / GSMTC
// ============================================================================

GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;

Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    IStream* nativeStream = nullptr;
    if (SUCCEEDED(CreateStreamOverRandomAccessStream(
            reinterpret_cast<IUnknown*>(winrt::get_abi(stream)),
            IID_PPV_ARGS(&nativeStream)))) {
        Bitmap* bmp = Bitmap::FromStream(nativeStream);
        nativeStream->Release();
        if (bmp && bmp->GetLastStatus() == Ok) return bmp;
        delete bmp;
    }
    return nullptr;
}

// ============================================================================
// Album Art Color Extraction
// ============================================================================

Color ExtractDominantColor(Bitmap* bmp) {
    if (!bmp) return Color(255, 29, 185, 84); // default green

    int w = bmp->GetWidth();
    int h = bmp->GetHeight();
    if (w == 0 || h == 0) return Color(255, 29, 185, 84);

    float bestSat = 0;
    BYTE bestR = 29, bestG = 185, bestB = 84;

    // Sample a 7x7 grid
    for (int gy = 0; gy < 7; gy++) {
        for (int gx = 0; gx < 7; gx++) {
            int px = (w * (gx + 1)) / 8;
            int py = (h * (gy + 1)) / 8;
            if (px >= w) px = w - 1;
            if (py >= h) py = h - 1;

            Color c;
            bmp->GetPixel(px, py, &c);

            BYTE r = c.GetRed(), g = c.GetGreen(), b = c.GetBlue();
            BYTE maxC = max(r, max(g, b));
            BYTE minC = min(r, min(g, b));
            float sat = (maxC == 0) ? 0 : (float)(maxC - minC) / maxC;
            float brightness = maxC / 255.0f;

            // Prefer saturated, mid-brightness colors
            float score = sat * (0.3f + 0.7f * (1.0f - fabsf(brightness - 0.6f)));
            if (score > bestSat) {
                bestSat = score;
                bestR = r; bestG = g; bestB = b;
            }
        }
    }

    // If nothing saturated found, return a muted theme color
    if (bestSat < 0.1f) return Color(255, 120, 120, 140);

    return Color(255, bestR, bestG, bestB);
}

Color GetAccentColor(Color baseAccent) {
    Color raw;
    if (g_Settings.accentMode == 2) {
        raw = Color(g_Settings.accentColor);
    } else if (g_Settings.accentMode == 1) {
        raw = Color(255, 96, 165, 250);
    } else {
        raw = baseAccent;
    }

    // Apply saturation and brightness in RGB space
    // This approach works even for already-saturated colors
    float r = raw.GetRed() / 255.0f;
    float g = raw.GetGreen() / 255.0f;
    float b = raw.GetBlue() / 255.0f;

    // Luminance (perceptual gray point)
    float gray = 0.299f * r + 0.587f * g + 0.114f * b;

    // Saturation: lerp/extrapolate between gray and original color
    // At 0% → full gray, 100% → original, 200% → pushed away from gray
    float satMul = g_Settings.accentSaturation / 100.0f;
    r = gray + (r - gray) * satMul;
    g = gray + (g - gray) * satMul;
    b = gray + (b - gray) * satMul;

    // Brightness: scale all channels
    float briMul = g_Settings.accentBrightness / 100.0f;
    r *= briMul;
    g *= briMul;
    b *= briMul;

    // Clamp to [0, 1]
    auto clamp01 = [](float v) -> float { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); };
    r = clamp01(r);
    g = clamp01(g);
    b = clamp01(b);

    return Color(255, (BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));
}

// ============================================================================
// Media Info
// ============================================================================

void UpdateMediaInfo() {
    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        if (!g_SessionManager) return;

        auto session = g_SessionManager.GetCurrentSession();
        if (session) {
            auto props = session.TryGetMediaPropertiesAsync().get();
            auto info  = session.GetPlaybackInfo();
            wstring newTitle = props.Title().c_str();

            lock_guard<mutex> guard(g_MediaState.lock);

            if (newTitle != g_MediaState.title) {
                // Save old state for crossfade
                g_OldMediaState.title = g_MediaState.title;
                g_OldMediaState.artist = g_MediaState.artist;
                g_OldMediaState.hasMedia = g_MediaState.hasMedia;
                if (g_OldMediaState.albumArt) delete g_OldMediaState.albumArt;
                g_OldMediaState.albumArt = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
                g_OldAccent = g_ExtractedAccent;

                // Load new state
                g_MediaState.title = newTitle;
                g_MediaState.artist = props.Artist().c_str();
                g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                g_MediaState.hasMedia = true;
                
                if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
                
                auto thumbRef = props.Thumbnail();
                if (thumbRef) {
                    auto stream = thumbRef.OpenReadAsync().get();
                    g_MediaState.albumArt = StreamToBitmap(stream);
                    if (g_MediaState.albumArt && g_Settings.accentMode == 0) {
                        g_ExtractedAccent = ExtractDominantColor(g_MediaState.albumArt);
                    }
                }

                // Initiate Crossfade
                g_CrossfadeAnim.Set(0.0f);
                g_CrossfadeAnim.SetTarget(1.0f);
                
                g_ScrollPos.Set(0.0f);
                g_ScrollPaused = true;
                g_ScrollPauseMs = SCROLL_PAUSE_DURATION;
                g_AccentPulse.Set(0.0f);
                g_PulseRising = true;
                g_LastTrackTitle = newTitle;
            } else {
                // Not transitioning, just update playing state
                g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                g_MediaState.hasMedia = true;
                g_MediaState.title = newTitle;
                g_MediaState.artist = props.Artist().c_str();

                // If thumbnail was missing, try fetching it
                if (g_MediaState.albumArt == nullptr) {
                    auto thumbRef = props.Thumbnail();
                    if (thumbRef) {
                        auto stream = thumbRef.OpenReadAsync().get();
                        g_MediaState.albumArt = StreamToBitmap(stream);
                        if (g_MediaState.albumArt && g_Settings.accentMode == 0) {
                            g_ExtractedAccent = ExtractDominantColor(g_MediaState.albumArt);
                        }
                    }
                }
            }
        } else {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
            g_MediaState.title = L"No Media";
            g_MediaState.artist = L"";
            if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
            g_LastTrackTitle = L"";
        }
    } catch (...) {
        lock_guard<mutex> guard(g_MediaState.lock);
        g_MediaState.hasMedia = false;
    }
}

void SendMediaCommand(int cmd) {
    try {
        if (!g_SessionManager) return;
        auto session = g_SessionManager.GetCurrentSession();
        if (session) {
            if (cmd == 1) session.TrySkipPreviousAsync();
            else if (cmd == 2) session.TryTogglePlayPauseAsync();
            else if (cmd == 3) session.TrySkipNextAsync();
        }
    } catch (...) {}
}

// ============================================================================
// Visuals
// ============================================================================

bool IsSystemLightMode() {
    DWORD value = 0; DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

DWORD GetCurrentTextColor() {
    if (g_Settings.autoTheme) return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    return g_Settings.manualTextColor;
}

void UpdateAppearance(HWND hwnd) {
    // Native Windows 11 Rounding
    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    // Acrylic Blur
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            DWORD tint = 0;
            if (g_Settings.autoTheme) {
                tint = IsSystemLightMode() ? 0x40FFFFFF : 0x40000000;
            } else {
                tint = (g_Settings.bgOpacity << 24) | (0xFFFFFF);
            }
            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
        }
    }
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void DrawRoundedRect(Graphics& g, Brush* brush, int x, int y, int w, int h, int radius) {
    GraphicsPath path;
    int d = radius * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
    g.FillPath(brush, &path);
}

void SetRoundedRectClip(Graphics& g, int x, int y, int w, int h, int radius) {
    GraphicsPath path;
    int d = radius * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
    g.SetClip(&path);
}

// ============================================================================
// Main Drawing
// ============================================================================

void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.Clear(Color(0, 0, 0, 0));

    Color mainColor{GetCurrentTextColor()};
    BYTE contentAlpha = (BYTE)(g_ContentOpacity.current * 255);

    // Snapshot media state
    MediaState state, oldState;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title    = g_MediaState.title;
        state.artist   = g_MediaState.artist;
        state.albumArt = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;

        oldState.title    = g_OldMediaState.title;
        oldState.artist   = g_OldMediaState.artist;
        oldState.albumArt = g_OldMediaState.albumArt ? g_OldMediaState.albumArt->Clone() : nullptr;
        oldState.hasMedia = g_OldMediaState.hasMedia;
        oldState.isPlaying = g_OldMediaState.isPlaying;
    }

    // ─── Layout Constants (compact) ───
    int padding    = 6;
    int artSize    = height - (padding * 2);
    int artX       = padding;
    int artY       = padding;
    int artRadius  = 5;
    int controlGap = 4;
    int iconSize   = 13;

    // ─── 1. Accent Effects (drawn FIRST, behind content) ───
    if (g_Settings.accentStyle != 0 && (state.hasMedia || oldState.hasMedia)) {
        Color newAccent = GetAccentColor(g_ExtractedAccent);
        Color oldAccent = GetAccentColor(g_OldAccent);
        Color accent = LerpColor(oldAccent, newAccent, g_CrossfadeAnim.current);
        
        float glowAlpha = g_AccentGlowOpacity.current;
        float pulse = g_AccentPulse.current;
        float intensityMul = g_Settings.accentIntensity / 100.0f;
        float baseAlpha = glowAlpha * (0.7f + 0.3f * pulse) * g_ContentOpacity.current * intensityMul;

        if (g_Settings.accentStyle == 1) {
            // ── Glow Border ──
            BYTE borderAlpha = (BYTE)(baseAlpha * 255);
            if (borderAlpha > 2) {
                Pen glowPen(Color((BYTE)(borderAlpha / 3),
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()), 3.0f);
                GraphicsPath glowPath;
                int gm = 1, gd = 14;
                glowPath.AddArc(gm, gm, gd, gd, 180, 90);
                glowPath.AddArc(width - gd - gm, gm, gd, gd, 270, 90);
                glowPath.AddArc(width - gd - gm, height - gd - gm, gd, gd, 0, 90);
                glowPath.AddArc(gm, height - gd - gm, gd, gd, 90, 90);
                glowPath.CloseFigure();
                graphics.DrawPath(&glowPen, &glowPath);

                Pen borderPen(Color(borderAlpha,
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()), 1.2f);
                GraphicsPath borderPath;
                int bm = 2, bd = 12;
                borderPath.AddArc(bm, bm, bd, bd, 180, 90);
                borderPath.AddArc(width - bd - bm, bm, bd, bd, 270, 90);
                borderPath.AddArc(width - bd - bm, height - bd - bm, bd, bd, 0, 90);
                borderPath.AddArc(bm, height - bd - bm, bd, bd, 90, 90);
                borderPath.CloseFigure();
                graphics.DrawPath(&borderPen, &borderPath);
            }
        }
        else if (g_Settings.accentStyle == 2) {
            // ── Ambient ──
            BYTE ambientAlpha = (BYTE)(baseAlpha * 255);
            if (ambientAlpha > 1) {
                LinearGradientBrush ambientBrush(
                    Point(0, 0), Point(width, height),
                    Color(ambientAlpha, accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillRectangle(&ambientBrush, 0, 0, width, height);

                Pen edgePen(Color((BYTE)(ambientAlpha / 2),
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()), 0.8f);
                GraphicsPath edgePath;
                int em = 2, ed = 12;
                edgePath.AddArc(em, em, ed, ed, 180, 90);
                edgePath.AddArc(width - ed - em, em, ed, ed, 270, 90);
                edgePath.AddArc(width - ed - em, height - ed - em, ed, ed, 0, 90);
                edgePath.AddArc(em, height - ed - em, ed, ed, 90, 90);
                edgePath.CloseFigure();
                graphics.DrawPath(&edgePen, &edgePath);
            }
        }
        else if (g_Settings.accentStyle == 3) {
            // ── Edge Light ──
            BYTE stripAlpha = (BYTE)(baseAlpha * 255);
            if (stripAlpha > 2) {
                int stripW = 3;
                int fadeW = 30;

                SolidBrush stripBrush(Color(stripAlpha,
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillRectangle(&stripBrush, 0, 4, stripW, height - 8);

                LinearGradientBrush fadeBrush(
                    Point(stripW, 0), Point(stripW + fadeW, 0),
                    Color((BYTE)(stripAlpha / 3), accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillRectangle(&fadeBrush, stripW, 4, fadeW, height - 8);

                SolidBrush dotBrush(Color((BYTE)(stripAlpha / 2),
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillEllipse(&dotBrush, 0, 3, 5, 5);
                graphics.FillEllipse(&dotBrush, 0, height - 8, 5, 5);
            }
        }
        else if (g_Settings.accentStyle == 4) {
            // ── Underglow ──
            BYTE glowA = (BYTE)(baseAlpha * 255);
            if (glowA > 2) {
                int glowH = 6;
                LinearGradientBrush underBrush(
                    Point(0, height - glowH), Point(0, height),
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                    Color(glowA, accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillRectangle(&underBrush, 8, height - glowH, width - 16, glowH);

                int hotW = width / 3;
                int hotX = (width - hotW) / 2;
                LinearGradientBrush hotBrush(
                    Point(hotX, height - 3), Point(hotX + hotW, height - 3),
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                Color hotColors[] = {
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                    Color(glowA, accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue())
                };
                REAL hotPositions[] = { 0.0f, 0.5f, 1.0f };
                hotBrush.SetInterpolationColors(hotColors, hotPositions, 3);
                graphics.FillRectangle(&hotBrush, hotX, height - 3, hotW, 3);

                SolidBrush lineBrush(Color((BYTE)(glowA / 2),
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillRectangle(&lineBrush, 12, height - 1, width - 24, 1);
            }
        }
        else if (g_Settings.accentStyle == 5) {
            // ── Neon ──
            BYTE neonAlpha = (BYTE)(baseAlpha * 255);
            if (neonAlpha > 2) {
                Pen outerPen(Color((BYTE)(neonAlpha / 3),
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()), 5.0f);
                GraphicsPath outerPath;
                int om = 0, od = 16;
                outerPath.AddArc(om, om, od, od, 180, 90);
                outerPath.AddArc(width - od - om, om, od, od, 270, 90);
                outerPath.AddArc(width - od - om, height - od - om, od, od, 0, 90);
                outerPath.AddArc(om, height - od - om, od, od, 90, 90);
                outerPath.CloseFigure();
                graphics.DrawPath(&outerPen, &outerPath);

                Pen midPen(Color((BYTE)(neonAlpha / 2),
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()), 2.5f);
                GraphicsPath midPath;
                int mm = 2, md = 13;
                midPath.AddArc(mm, mm, md, md, 180, 90);
                midPath.AddArc(width - md - mm, mm, md, md, 270, 90);
                midPath.AddArc(width - md - mm, height - md - mm, md, md, 0, 90);
                midPath.AddArc(mm, height - md - mm, md, md, 90, 90);
                midPath.CloseFigure();
                graphics.DrawPath(&midPen, &midPath);

                Pen innerPen(Color(neonAlpha,
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()), 1.0f);
                GraphicsPath innerPath;
                int im = 3, id2 = 11;
                innerPath.AddArc(im, im, id2, id2, 180, 90);
                innerPath.AddArc(width - id2 - im, im, id2, id2, 270, 90);
                innerPath.AddArc(width - id2 - im, height - id2 - im, id2, id2, 0, 90);
                innerPath.AddArc(im, height - id2 - im, id2, id2, 90, 90);
                innerPath.CloseFigure();
                graphics.DrawPath(&innerPen, &innerPath);
            }
        }
        else if (g_Settings.accentStyle == 6) {
            // ── Spectrum ──
            BYTE specAlpha = (BYTE)(baseAlpha * 255);
            if (specAlpha > 2) {
                float hueOffset = fmodf(g_AnimTimer * 30.0f, 360.0f);
                GraphicsPath specPath;
                int sm = 2, sd = 12;
                specPath.AddArc(sm, sm, sd, sd, 180, 90);
                specPath.AddArc(width - sd - sm, sm, sd, sd, 270, 90);
                specPath.AddArc(width - sd - sm, height - sd - sm, sd, sd, 0, 90);
                specPath.AddArc(sm, height - sd - sm, sd, sd, 90, 90);
                specPath.CloseFigure();

                for (int seg = 0; seg < 6; seg++) {
                    float segHue = fmodf(hueOffset + seg * 60.0f, 360.0f);
                    float h = segHue / 60.0f;
                    float x2 = 1.0f - fabsf(fmodf(h, 2.0f) - 1.0f);
                    float r = 0, g = 0, b = 0;
                    if (h < 1) { r = 1; g = x2; }
                    else if (h < 2) { r = x2; g = 1; }
                    else if (h < 3) { g = 1; b = x2; }
                    else if (h < 4) { g = x2; b = 1; }
                    else if (h < 5) { r = x2; b = 1; }
                    else { r = 1; b = x2; }
                    Pen segPen(Color(specAlpha,
                        (BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255)), 1.5f);
                    segPen.SetDashOffset((float)(seg * 8));
                    graphics.DrawPath(&segPen, &specPath);
                }
            }
        }
        else if (g_Settings.accentStyle == 7) {
            // ── Pulse Ring ──
            BYTE ringAlphaMax = (BYTE)(baseAlpha * 255);
            if (ringAlphaMax > 2) {
                int cx = padding + artSize / 2;
                int cy = height / 2;
                float maxR = (float)(width);

                float progress = fmodf(g_AnimTimer * 0.5f, 1.0f);
                float ringR = (float)artSize / 2.0f + progress * (maxR - artSize / 2.0f);
                float ringAlpha = (1.0f - progress) * ringAlphaMax;

                if (ringAlpha > 2) {
                    Pen ringPen(Color((BYTE)ringAlpha,
                        accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                        2.0f - progress * 1.5f);
                    graphics.DrawEllipse(&ringPen,
                        (float)cx - ringR, (float)cy - ringR, ringR * 2, ringR * 2);

                    float progress2 = fmodf(g_AnimTimer * 0.5f + 0.5f, 1.0f);
                    float ringR2 = (float)artSize / 2.0f + progress2 * (maxR - artSize / 2.0f);
                    float ringAlpha2 = (1.0f - progress2) * ringAlphaMax;
                    if (ringAlpha2 > 2) {
                        Pen ringPen2(Color((BYTE)ringAlpha2,
                            accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                            2.0f - progress2 * 1.5f);
                        graphics.DrawEllipse(&ringPen2,
                            (float)cx - ringR2, (float)cy - ringR2, ringR2 * 2, ringR2 * 2);
                    }
                }
            }
        }
        else if (g_Settings.accentStyle == 8) {
            // ── Corner Glow ──
            BYTE cornerAlpha = (BYTE)(baseAlpha * 255);
            if (cornerAlpha > 2) {
                int glowR = 20;
                GraphicsPath tl; tl.AddEllipse(-glowR / 2, -glowR / 2, glowR * 2, glowR * 2);
                PathGradientBrush tlBr(&tl);
                Color tlCenter(cornerAlpha, accent.GetRed(), accent.GetGreen(), accent.GetBlue());
                Color tlSurround(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue());
                tlBr.SetCenterColor(tlCenter); int cnt = 1; tlBr.SetSurroundColors(&tlSurround, &cnt);
                graphics.FillPath(&tlBr, &tl);

                GraphicsPath tr; tr.AddEllipse((float)(width - glowR * 3.0f / 2.0f), (float)(-glowR / 2.0f), (float)(glowR * 2.0f), (float)(glowR * 2.0f));
                PathGradientBrush trBr(&tr);
                trBr.SetCenterColor(tlCenter); trBr.SetSurroundColors(&tlSurround, &cnt);
                graphics.FillPath(&trBr, &tr);

                GraphicsPath bl; bl.AddEllipse((float)(-glowR / 2.0f), (float)(height - glowR * 3.0f / 2.0f), (float)(glowR * 2.0f), (float)(glowR * 2.0f));
                PathGradientBrush blBr(&bl);
                blBr.SetCenterColor(tlCenter); blBr.SetSurroundColors(&tlSurround, &cnt);
                graphics.FillPath(&blBr, &bl);

                GraphicsPath br2; br2.AddEllipse((float)(width - glowR * 3.0f / 2.0f), (float)(height - glowR * 3.0f / 2.0f), (float)(glowR * 2.0f), (float)(glowR * 2.0f));
                PathGradientBrush brBr(&br2);
                brBr.SetCenterColor(tlCenter); brBr.SetSurroundColors(&tlSurround, &cnt);
                graphics.FillPath(&brBr, &br2);
            }
        }
        else if (g_Settings.accentStyle == 9) {
            // ── Frost Edge ──
            BYTE frostAlpha = (BYTE)(baseAlpha * 255);
            if (frostAlpha > 1) {
                int frostH = height / 2;
                LinearGradientBrush frostBrush(
                    Point(0, 0), Point(0, frostH),
                    Color(frostAlpha, accent.GetRed(), accent.GetGreen(), accent.GetBlue()),
                    Color(0, accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillRectangle(&frostBrush, 0, 0, width, frostH);

                SolidBrush topLine(Color((BYTE)(frostAlpha * 2 > 255 ? 255 : frostAlpha * 2),
                    accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
                graphics.FillRectangle(&topLine, 6, 0, width - 12, 1);

                int shimmerW = width / 4;
                int shimmerX = (int)(pulse * (width - shimmerW));
                LinearGradientBrush shimmer(
                    Point(shimmerX, 0), Point(shimmerX + shimmerW, 0),
                    Color(0, 255, 255, 255),
                    Color(0, 255, 255, 255));
                Color shimmerColors[] = {
                    Color(0, 255, 255, 255),
                    Color((BYTE)(frostAlpha / 3), 255, 255, 255),
                    Color(0, 255, 255, 255)
                };
                REAL shimmerPos[] = { 0.0f, 0.5f, 1.0f };
                shimmer.SetInterpolationColors(shimmerColors, shimmerPos, 3);
                graphics.FillRectangle(&shimmer, shimmerX, 0, shimmerW, 3);
            }
        }
    }

    // ─── 2. Album Art (Rounded with Shadow) ───
    auto DrawArtLayer = [&](const MediaState& s, float alpha, float tr_x, float tr_y, float scale) {
        if (!s.hasMedia || alpha <= 0.01f) return;
        BYTE layerAlpha = (BYTE)(alpha * 255);

        GraphicsState gState = graphics.Save();
        
        // Setup transform axis relative to the center of the image
        float cx = artX + artSize / 2.0f;
        float cy = artY + artSize / 2.0f;
        graphics.TranslateTransform(cx + tr_x, cy + tr_y);
        graphics.ScaleTransform(scale, scale);
        graphics.TranslateTransform(-cx, -cy);

        // Shadow
        SolidBrush shadowBrush(Color((BYTE)(30 * alpha), 0, 0, 0));
        DrawRoundedRect(graphics, &shadowBrush, artX + 2, artY + 2, artSize, artSize, artRadius);

        // Art clip
        SetRoundedRectClip(graphics, artX, artY, artSize, artSize, artRadius);

        if (s.albumArt) {
            ColorMatrix cm = {{
                { 1.0f, 0.0f, 0.0f,  0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f,  0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f,  0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, alpha, 0.0f },
                { 0.0f, 0.0f, 0.0f,  0.0f, 1.0f }
            }};
            ImageAttributes ia;
            ia.SetColorMatrix(&cm);
            Rect artRect(artX, artY, artSize, artSize);
            graphics.DrawImage(s.albumArt, artRect,
                0, 0, s.albumArt->GetWidth(), s.albumArt->GetHeight(),
                UnitPixel, &ia);
        } else {
            SolidBrush placeBrush(Color((BYTE)(40 * alpha), 128, 128, 128));
            graphics.FillRectangle(&placeBrush, artX, artY, artSize, artSize);

            FontFamily iconFam(ICON_FONT_NAME);
            Font iconFont(&iconFam, 16.0f, FontStyleRegular, UnitPixel);
            SolidBrush iconBr(Color(layerAlpha / 2, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
            RectF artRectF((float)artX, (float)artY, (float)artSize, (float)artSize);
            graphics.DrawString(L"\xE8D6", -1, &iconFont, artRectF, &sf, &iconBr);
        }
        graphics.Restore(gState);
    };

    // Apply Transform logic for layers
    float t = g_CrossfadeAnim.current;
    
    float oldX = 0, oldY = 0, oldS = 1.0f;
    float newX = 0, newY = 0, newS = 1.0f;
    
    // Configurable transition displacement amount
    float d = 25.0f;
    float z = 0.8f; // scale down amount 
    
    int transitionStyle = g_Settings.transitionStyle;
    if (transitionStyle == 1) { // slide left
        oldX = -d * t;
        newX = d * (1.0f - t);
    } else if (transitionStyle == 2) { // slide right
        oldX = d * t;
        newX = -d * (1.0f - t);
    } else if (transitionStyle == 3) { // slide up
        oldY = -d * t;
        newY = d * (1.0f - t);
    } else if (transitionStyle == 4) { // slide down
        oldY = d * t;
        newY = -d * (1.0f - t);
    } else if (transitionStyle == 5) { // zoom in
        oldS = 1.0f + (1.0f / z - 1.0f) * t;
        newS = z + (1.0f - z) * t;
    } else if (transitionStyle == 6) { // zoom out
        oldS = 1.0f - (1.0f - z) * t;
        newS = (1.0f / z) - ((1.0f / z) - 1.0f) * t;
    }

    DrawArtLayer(oldState, g_ContentOpacity.current * (1.0f - t), oldX, oldY, oldS);
    DrawArtLayer(state, g_ContentOpacity.current * t, newX, newY, newS);

    // Free Old Album Art clone since we're done drawing it
    if (oldState.albumArt) delete oldState.albumArt;
    if (state.albumArt) delete state.albumArt;

    // ─── 3. Media Controls (Segoe Fluent Icons) — visible on hover ───
    float ctrlVis = g_ControlsVisibility.current;
    int controlsX = artX + artSize + 8;
    int controlCenterY = height / 2;

    FontFamily iconFamily(ICON_FONT_NAME);
    Font iconFont(&iconFamily, (REAL)(iconSize), FontStyleRegular, UnitPixel);
    StringFormat iconSF;
    iconSF.SetAlignment(StringAlignmentCenter);
    iconSF.SetLineAlignment(StringAlignmentCenter);

    struct ControlDef {
        const WCHAR* icon;
        int index;
    };
    ControlDef controls[] = {
        { ICON_PREV,  1 },
        { state.isPlaying ? ICON_PAUSE : ICON_PLAY, 2 },
        { ICON_NEXT,  3 },
    };

    int btnSize = 20;
    int totalControlsW = 3 * btnSize + 2 * controlGap;
    int btnY = controlCenterY - btnSize / 2;

    if (ctrlVis > 0.01f) {
        for (int i = 0; i < 3; i++) {
            int btnX = controlsX + i * (btnSize + controlGap);
            float hoverAlpha = g_HoverOpacity[controls[i].index].current;

            if (hoverAlpha > 0.01f) {
                SolidBrush hlBrush(Color(
                    (BYTE)(40 * hoverAlpha * ctrlVis * g_ContentOpacity.current),
                    mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
                graphics.FillEllipse(&hlBrush, btnX - 2, btnY - 2, btnSize + 4, btnSize + 4);
            }

            float baseIconAlpha = (0.55f + 0.45f * hoverAlpha) * ctrlVis;
            SolidBrush iconBrush(Color(
                (BYTE)(contentAlpha * baseIconAlpha),
                mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            RectF iconRect((float)btnX, (float)btnY, (float)btnSize, (float)btnSize);
            graphics.DrawString(controls[i].icon, -1, &iconFont, iconRect, &iconSF, &iconBrush);
        }
    }

    // ─── 4. Text (Two-Line Layout with Fade Edges) ───
    int controlsSpace = (int)(totalControlsW * ctrlVis + 6 * ctrlVis);
    int textX    = controlsX + controlsSpace;
    int textMaxW = width - textX - 10;

    auto DrawTextLayer = [&](const MediaState& s, float alpha, bool isNew, float tr_x, float tr_y, float scale) {
        if (!s.hasMedia || alpha <= 0.01f || textMaxW <= 20) return;
        BYTE layerAlpha = (BYTE)(alpha * 255);

        GraphicsState gState = graphics.Save();

        FontFamily fontFamily(g_FontName, nullptr);
        REAL titleSize  = (REAL)g_Settings.fontSize;
        REAL artistSize = (REAL)g_Settings.artistFontSize;

        Font titleFont(&fontFamily, titleSize, FontStyleBold, UnitPixel);
        Font artistFont(&fontFamily, artistSize, FontStyleRegular, UnitPixel);

        RectF measureRect(0, 0, 10000, 100);
        RectF titleBound, artistBound;
        graphics.MeasureString(s.title.c_str(), -1, &titleFont, measureRect, &titleBound);

        float lineHeight = titleSize + 2;
        float totalTextH = lineHeight + artistSize + 2;
        float textBaseY  = (height - totalTextH) / 2.0f + g_Settings.textOffsetY;

        if (isNew) {
            g_TextWidth = (int)titleBound.Width;
            int targetWidth = (int)g_WidthAnim.target;
            int availableW = targetWidth - textX - 10;
            bool needsScroll = (g_TextWidth > availableW);
            
            if (!needsScroll && g_IsScrolling) {
                g_ScrollPos.Set(0.0f);
                g_ScrollPaused = true;
                g_ScrollPauseMs = SCROLL_PAUSE_DURATION;
            }
            g_IsScrolling = needsScroll;
        }

        float cx = textX + (min(g_TextWidth, textMaxW) / 2.0f);
        float cy = textBaseY + totalTextH / 2.0f;
        
        graphics.TranslateTransform(cx + tr_x, cy + tr_y);
        graphics.ScaleTransform(scale, scale);
        graphics.TranslateTransform(-cx, -cy);

        Region textClip(Rect(textX, 0, textMaxW, height));
        graphics.SetClip(&textClip);

        SolidBrush titleBrush(Color(layerAlpha, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));

        bool doScroll = isNew && g_IsScrolling;

        if (doScroll) {
            float scrollX = g_ScrollPos.current;
            float drawX = (float)textX - scrollX;

            graphics.DrawString(s.title.c_str(), -1, &titleFont, PointF(drawX, textBaseY), &titleBrush);
            graphics.DrawString(s.title.c_str(), -1, &titleFont, PointF(drawX + g_TextWidth + 60, textBaseY), &titleBrush);
        } else {
            graphics.DrawString(s.title.c_str(), -1, &titleFont, PointF((float)textX, textBaseY), &titleBrush);
        }

        if (!s.artist.empty()) {
            SolidBrush artistBrush(Color(
                (BYTE)(layerAlpha * 0.6f),
                mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            StringFormat artistSF;
            artistSF.SetTrimming(StringTrimmingEllipsisCharacter);
            artistSF.SetFormatFlags(StringFormatFlagsNoWrap);
            RectF artistRect((float)textX, textBaseY + lineHeight, (float)textMaxW, artistSize + 4);
            graphics.DrawString(s.artist.c_str(), -1, &artistFont, artistRect, &artistSF, &artistBrush);
        }

        if (doScroll) {
            int fadeW = 20;
            LinearGradientBrush leftFade(
                Point(textX, 0), Point(textX + fadeW, 0),
                Color(255, 0, 0, 0), Color(0, 0, 0, 0));
            graphics.ResetClip();
            // Note: In original code, leftFade/rightFade didn't actually draw anything, they just reset clip. 
            // A separate overlay function is needed to draw actual fade gradients if desired, 
            // but we leave it as originally authored for now.
        }

        graphics.Restore(gState);
    };

    DrawTextLayer(oldState, g_ContentOpacity.current * (1.0f - t), false, oldX, oldY, oldS);
    DrawTextLayer(state, g_ContentOpacity.current * t, true, newX, newY, newS);
}

// ============================================================================
// Window Procedure
// ============================================================================

void CalculateDynamicWidth() {
    if (!g_Settings.autoWidth) {
        g_WidthAnim.SetTarget((float)g_Settings.width);
        return;
    }
    
    HDC hdc = GetDC(NULL);
    Graphics graphics(hdc);

    FontFamily fontFamily(g_FontName, nullptr);
    REAL titleSize  = (REAL)g_Settings.fontSize;
    REAL artistSize = (REAL)g_Settings.artistFontSize;

    Font titleFont(&fontFamily, titleSize, FontStyleBold, UnitPixel);
    Font artistFont(&fontFamily, artistSize, FontStyleRegular, UnitPixel);

    RectF measureRect(0, 0, 10000, 100);
    RectF titleBound, artistBound;
    
    wstring title, artist;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        title = g_MediaState.title;
        artist = g_MediaState.artist;
        if (!g_MediaState.hasMedia) {
            title = L"No Media";
            artist = L"";
        }
    }
    
    graphics.MeasureString(title.c_str(), -1, &titleFont, measureRect, &titleBound);
    graphics.MeasureString(artist.c_str(), -1, &artistFont, measureRect, &artistBound);

    ReleaseDC(NULL, hdc);

    int maxTextW = max((int)titleBound.Width, (int)artistBound.Width);

    int padding = 6;
    int artSize = g_Settings.height - (padding * 2);
    int totalControlsW = 68; // 3 btns * 20 + 2 gaps * 4
    
    // When auto-hide controls is on, smoothly interpolate controls space
    // based on the current controls visibility animation state.
    float ctrlVis = g_Settings.autoHideControls ? g_ControlsVisibility.current : 1.0f;
    int controlsSpace = (int)((totalControlsW + 6) * ctrlVis);
    
    int textX = padding + artSize + 8 + controlsSpace;
    
    int requiredWidth = textX + maxTextW + 15; // 15px grace padding
    
    int minW = g_Settings.width;
    if (requiredWidth < minW) requiredWidth = minW;
    if (requiredWidth > g_Settings.maxWidth) requiredWidth = g_Settings.maxWidth;

    g_WidthAnim.SetTarget((float)requiredWidth);
}

#define IDT_POLL_MEDIA 1001
#define IDT_ANIMATION  1002
#define APP_WM_CLOSE   WM_APP

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            UpdateAppearance(hwnd);
            SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL);
            SetTimer(hwnd, IDT_ANIMATION, 16, NULL); // ~60fps animation
            g_ContentOpacity.Set(1.0f);
            g_AccentGlowOpacity.Set(1.0f);
            g_CrossfadeAnim.Set(1.0f);
            g_WidthAnim.Set((float)g_Settings.currentWidth);
            g_AccentPulse.Set(0.0f);
            for (int i = 0; i < 4; i++) g_HoverOpacity[i].Set(0.0f);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_SessionManager = nullptr;
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL_MEDIA) {
                UpdateMediaInfo();
                CalculateDynamicWidth();
                InvalidateRect(hwnd, NULL, FALSE);

                // Reposition on taskbar + enforce z-order
                HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
                if (hTaskbar) {
                    RECT rc; GetWindowRect(hTaskbar, &rc);
                    int x = rc.left + g_Settings.offsetX;
                    int taskbarHeight = rc.bottom - rc.top;
                    int y = rc.top + (taskbarHeight / 2) - (g_Settings.height / 2) + g_Settings.offsetY;

                    // Fullscreen detection when AlwaysOnTop is disabled
                    if (!g_Settings.alwaysOnTop) {
                        HWND hFg = GetForegroundWindow();
                        bool fullscreenActive = false;

                        if (hFg && hFg != hTaskbar && hFg != hwnd) {
                            RECT fgRect;
                            GetWindowRect(hFg, &fgRect);
                            HMONITOR hMon = MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST);
                            MONITORINFO mi = { sizeof(MONITORINFO) };
                            if (GetMonitorInfo(hMon, &mi)) {
                                // Foreground window covers entire monitor = fullscreen
                                if (fgRect.left <= mi.rcMonitor.left &&
                                    fgRect.top <= mi.rcMonitor.top &&
                                    fgRect.right >= mi.rcMonitor.right &&
                                    fgRect.bottom >= mi.rcMonitor.bottom) {
                                    fullscreenActive = true;
                                }
                            }
                        }

                        if (fullscreenActive) {
                            if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
                        } else {
                            if (!IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                            SetWindowPos(hwnd, HWND_NOTOPMOST, x, y,
                                g_Settings.currentWidth, g_Settings.height, SWP_NOACTIVATE);
                        }

                        // Switch z-band to DEFAULT if needed
                        if (g_CurrentTopmost) {
                            g_CurrentTopmost = false;
                            if (g_SetWindowBand) {
                                g_SetWindowBand(hwnd, HWND_NOTOPMOST, ZBID_DEFAULT);
                            }
                        }
                    } else {
                        // AlwaysOnTop enabled — ensure visible and topmost
                        if (!IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                        SetWindowPos(hwnd, HWND_TOPMOST, x, y,
                            g_Settings.currentWidth, g_Settings.height, SWP_NOACTIVATE);

                        if (!g_CurrentTopmost) {
                            g_CurrentTopmost = true;
                            if (g_SetWindowBand) {
                                g_SetWindowBand(hwnd, HWND_TOPMOST, ZBID_IMMERSIVE_NOTIFICATION);
                            }
                        }
                    }
                }
            }
            else if (wParam == IDT_ANIMATION) {
                float dt = 0.016f; // ~60fps
                bool needsRedraw = false;

                // Tick all animations
                g_ContentOpacity.Tick(dt);
                g_AccentGlowOpacity.Tick(dt);
                
                float prevCrossfade = g_CrossfadeAnim.current;
                g_CrossfadeAnim.Tick(dt);
                if (fabsf(g_CrossfadeAnim.current - prevCrossfade) > 0.001f) needsRedraw = true;

                // Controls visibility fade (respects AutoHideControls setting)
                float prevCtrlVis = g_ControlsVisibility.current;
                if (g_Settings.autoHideControls) {
                    g_ControlsVisibility.SetTarget(g_PanelHovered ? 1.0f : 0.0f);
                } else {
                    g_ControlsVisibility.SetTarget(1.0f); // always visible
                }
                g_ControlsVisibility.Tick(dt);
                if (fabsf(g_ControlsVisibility.current - prevCtrlVis) > 0.001f) needsRedraw = true;

                // Pulse oscillation while playing (bounces 0 ↔ 1)
                g_AccentPulse.Tick(dt);
                if (g_AccentPulse.Done()) {
                    g_PulseRising = !g_PulseRising;
                    g_AccentPulse.SetTarget(g_PulseRising ? 1.0f : 0.0f);
                }

                for (int i = 1; i <= 3; i++) {
                    float prevVal = g_HoverOpacity[i].current;
                    g_HoverOpacity[i].SetTarget(g_HoverState == i ? 1.0f : 0.0f);
                    g_HoverOpacity[i].Tick(dt);
                    if (fabsf(g_HoverOpacity[i].current - prevVal) > 0.001f) needsRedraw = true;
                }

                // Smooth scrolling with pause
                if (g_IsScrolling) {
                    if (g_ScrollPaused) {
                        g_ScrollPauseMs -= 16;
                        if (g_ScrollPauseMs <= 0) {
                            g_ScrollPaused = false;
                        }
                    } else {
                        float scrollSpeed = 40.0f * g_AnimSpeedMultiplier;
                        g_ScrollPos.current += scrollSpeed * dt;

                        if (g_ScrollPos.current > g_TextWidth + 60) {
                            g_ScrollPos.current = 0;
                            g_ScrollPaused = true;
                            g_ScrollPauseMs = SCROLL_PAUSE_DURATION;
                        }
                    }
                    needsRedraw = true;
                }

                // Content fade / accent glow / pulse animation
                if (!g_ContentOpacity.Done() || !g_AccentGlowOpacity.Done() || !g_AccentPulse.Done()) {
                    needsRedraw = true;
                }

                // Advance animation timer for time-based accent effects
                g_AnimTimer += dt;
                // Spectrum (7), Pulse Ring (9), Frost Edge (11) need continuous redraws
                int as = g_Settings.accentStyle;
                if (as == 7 || as == 9 || as == 11) {
                    needsRedraw = true;
                }

                // Smooth width transition
                CalculateDynamicWidth(); // ensure target is up to date based on control visibility or new text
                float prevWidth = g_WidthAnim.current;
                g_WidthAnim.Tick(dt);
                if (fabsf(g_WidthAnim.current - prevWidth) > 0.1f) {
                    g_Settings.currentWidth = (int)roundf(g_WidthAnim.current);
                    
                    HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
                    if (hTaskbar) {
                        RECT rc; GetWindowRect(hTaskbar, &rc);
                        int x = rc.left + g_Settings.offsetX;
                        int taskbarH = rc.bottom - rc.top;
                        int y = rc.top + (taskbarH / 2) - (g_Settings.height / 2) + g_Settings.offsetY;
                        SetWindowPos(hwnd, NULL, x, y,
                            g_Settings.currentWidth, g_Settings.height,
                            SWP_NOACTIVATE | SWP_NOZORDER);
                    }
                    needsRedraw = true;
                }

                if (needsRedraw) {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            // Mark panel as hovered (shows controls)
            if (!g_PanelHovered) {
                g_PanelHovered = true;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            int artSize = g_Settings.height - 12;
            int controlsX = 6 + artSize + 8;
            int btnSize = 20;
            int controlGap = 4;
            int newState = 0;

            // Only hit-test controls when they're sufficiently visible
            if (g_ControlsVisibility.current > 0.5f) {
                for (int i = 0; i < 3; i++) {
                    int btnX = controlsX + i * (btnSize + controlGap);
                    int btnY = g_Settings.height / 2 - btnSize / 2;
                    if (x >= btnX - 4 && x <= btnX + btnSize + 4 &&
                        y >= btnY - 4 && y <= btnY + btnSize + 4) {
                        newState = i + 1;
                        break;
                    }
                }
            }

            if (newState != g_HoverState) {
                g_HoverState = newState;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            g_PanelHovered = false;
            g_HoverState = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            break;

        case WM_LBUTTONUP:
            if (g_HoverState > 0) SendMediaCommand(g_HoverState);
            return 0;

        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, 0, 0);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

            DrawMediaPanel(memDC, rc.right, rc.bottom);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap); DeleteObject(memBitmap); DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ============================================================================
// Main Thread
// ============================================================================

void MediaThread() {
    winrt::init_apartment();

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc   = MediaWndProc;
    wc.hInstance      = GetModuleHandle(NULL);
    wc.lpszClassName  = TEXT("WindhawkMusicLounge_GSMTC");
    wc.hCursor        = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);

    // Try to use CreateWindowInBand for proper z-ordering
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand = nullptr;
    if (hUser32) {
        CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(hUser32, "CreateWindowInBand");
        g_SetWindowBand = (pSetWindowBand)GetProcAddress(hUser32, "SetWindowBand");
    }

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
    if (g_Settings.alwaysOnTop) exStyle |= WS_EX_TOPMOST;

    if (CreateWindowInBand) {
        g_hMediaWindow = CreateWindowInBand(
            exStyle,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.currentWidth, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL,
            ZBID_IMMERSIVE_NOTIFICATION
        );
        if (g_hMediaWindow) {
            Wh_Log(L"Created window in ZBID_IMMERSIVE_NOTIFICATION band");
        }
    }

    // Fallback
    if (!g_hMediaWindow) {
        Wh_Log(L"CreateWindowInBand failed or unavailable, falling back to CreateWindowEx");
        g_hMediaWindow = CreateWindowEx(
            exStyle,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.currentWidth, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL
        );
    }

    SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
    winrt::uninit_apartment();
}

std::thread* g_pMediaThread = nullptr;

// ============================================================================
// Windhawk Tool Mod Callbacks
// ============================================================================

BOOL WhTool_ModInit() {
    LoadSettings();
    g_Running = true;
    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    g_Running = false;
    if (g_hMediaWindow) SendMessage(g_hMediaWindow, APP_WM_CLOSE, 0, 0);
    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        delete g_pMediaThread;
        g_pMediaThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hMediaWindow) {
        SendMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
        SendMessage(g_hMediaWindow, WM_SETTINGCHANGE, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
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
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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
