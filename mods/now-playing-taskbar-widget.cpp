// ==WindhawkMod==
// @id            now-playing-taskbar-widget
// @name          Now Playing — Taskbar Widget
// @description   A premium music widget for the Windows 11 taskbar — featuring album art, Chameleon background, media controls, progress bar scrubbing, and smooth animations.
// @version       1.0.0
// @author        eliwulfy
// @github        https://github.com/eliwulfy
// @license       MIT
// @include       explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lshell32 -ld2d1 -ldwrite -lwindowscodecs -lurlmon -lwininet
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Now Playing — Taskbar Widget

A premium music widget for the Windows 11 taskbar. Displays your currently playing track with album art, an animated dual-color Chameleon background, media controls, progress bar scrubbing, and smooth transitions — all rendered natively and efficiently with Direct2D.

![Preview](https://i.imgur.com/FIzoTrq.jpeg)

## Features
- 🎨 **Dual Chameleon Background** — extracts the dominant colors from the album art to create a glowing, animated gradient.
- 📊 **Chameleon Progress Bar** — a sleek, floating progress bar that adapts its gradient to your album colors, featuring click-to-scrub support and a dynamic hover indicator.
- 🖼️ **Album Art** — smooth crossfade transitions when the track changes, featuring intelligent stale-cache detection for platforms like Spotify.
- ⏯️ **Media Controls** — previous, play/pause, next with smooth hover fade animations.
- 🔊 **Volume Scroll** — scroll your mouse wheel over the widget to instantly adjust system volume.
- 🔀 **Shuffle & Repeat Icons** — tiny dynamic indicators matching your media player state.
- ⚙️ **Highly Customizable (37 Settings)** — deeply personalize the widget. Tweak everything from dimensions, corner radius, and button scaling to animation speeds, text scrolling, and background opacity.
- 💡 **Smart Sleep & Performance** — automatically pauses rendering when your PC goes to sleep or the taskbar auto-hides, ensuring zero background resource usage.
- 🌓 **Auto Theme** — text and tints seamlessly adapt to Windows Light/Dark mode.
- 🖥️ **Multi-monitor Support**

## 💡 NOTE
If a setting doesn't apply immediately, just disable and re-enable the mod to force a refresh.

## Credits
Inspired by and originally forked from **Taskbar Music Lounge** by [Hashah2311](https://github.com/Hashah2311).
Heavily rewritten and extended with new UI designs, Direct2D performance improvements, advanced image processing, and a fully redesigned settings page.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- _NoticeV2: "Have fun!"
  $name: "💡 NOTE"
  $description: "NOTE: If a setting doesn't apply immediately, just disable and re-enable the mod to force a refresh."
- HideWhenNoMedia: true
  $name: Hide When No Media
  $description: "Toggle to completely hide the widget when no music is playing or available."
- HideFullscreen: true
  $name: Hide in Fullscreen
  $description: "Toggle to hide the widget when another app is in full screen (like a game or video)."
- IdleTimeout: 0
  $name: Auto-Hide on Pause
  $description: "Time in seconds to wait before hiding the widget after music is paused. Set to 0 to disable. (Default: 0)"
- ClickToOpen: true
  $name: Click to Open App
  $description: "Toggle to open the active media player (e.g., Spotify) when you click the widget background."
- ClickToPause: false
  $name: Click to Pause/Play
  $description: "Toggle to use a background click to pause/play instead of opening the app. (Overrides ClickToOpen)"
- EnableVolumeScroll: true
  $name: Enable Volume Scrolling
  $description: "Toggle to control the system volume by scrolling your mouse wheel over the widget."

# ── Window & Appearance ────────────────────────────────────────────────────
- PanelWidth: 300
  $name: Panel Width
  $description: "Total width of the widget in pixels. (Default: 300, Minimum: 100)"
- PanelHeight: 48
  $name: Panel Height
  $description: "Vertical height of the widget in pixels. (Default: 48, Minimum: 24)"
- OffsetX: 80
  $name: Horizontal Position
  $description: "Distance in pixels from the left edge of the taskbar. (Default: 80)"
- OffsetY: 0
  $name: Vertical Position
  $description: "Vertical offset in pixels from the center of the taskbar. 0 is perfectly centered. (Default: 0)"
- PositionAnchor: 0
  $name: Position Anchor
  $description: "Which edge of the taskbar the widget is anchored to (0 = Left, 1 = Center, 2 = Right). Left/Right use Horizontal Position as an offset from that edge. Center uses it as an offset from the middle."
  $options:
  - 0: Left
  - 1: Center
  - 2: Right

# ── Multi-Monitor ──────────────────────────────────────────────────────────
- TargetTaskbar: 0
  $name: Target Taskbar Monitor
  $description: "Choose which monitor the widget should appear on."
  $options:
  - 0: Primary Monitor always
  - 1: Monitor with active Mouse Cursor

# ── Background ─────────────────────────────────────────────────────────────
- GlassType: 1
  $name: Background Glass Effect
  $description: "Visual style of the widget background. Note: Mica/Acrylic rely on Windows 11 features. (Default: Blur)"
  $options:
  - 0: Solid Tint
  - 1: Blur
  - 2: Acrylic
  - 3: Mica
- GlassTintColor: "000000"
  $name: Background Tint Color
  $description: "Background tint color in hex format (e.g. 1a1a1a or 000000). Only applies if AutoTheme is disabled."
- GlassOpacity: 0
  $name: Background Opacity
  $description: "Opacity of the tint layer. Range: 0 (invisible) to 255 (opaque). (Default: 0)"
- AutoTheme: true
  $name: Follow System Theme
  $description: "Toggle to automatically adjust text and tint colors based on Windows Light/Dark mode."
- TextColor: "FFFFFF"
  $name: Manual Text Color
  $description: "Font color in hex format (e.g. FFFFFF). Only applies if Follow System Theme is disabled."

# ── Chameleon Background ────────────────────────────────────────────────────
- ChameleonBg: true
  $name: Enable Chameleon
  $description: "Toggle to extract the dominant color from the album art and create a glowing gradient background."
- DualChameleonBg: true
  $name: Dual Chameleon Effect
  $description: "Extracts two contrasting vibrant colors from the album art and creates a gradient between them. When disabled, fades one color into transparency."
- ChameleonVibrancy: 50
  $name: Chameleon Color Vibrancy
  $description: "Adjusts the saturation of the extracted colors. Lower values produce softer, pastel-like gradients. Range: 0 (grayscale) to 200 (oversaturated). (Default: 50)"
- ChameleonIntensity: 175
  $name: Chameleon Intensity
  $description: "Base opacity/intensity of the album art glow. Range: 0 to 255. (Default: 175)"
- AnimateChameleon: true
  $name: Animate Chameleon
  $description: "Toggle to enable the smooth breathing animation for the background glow."
- ChameleonPulse: 20
  $name: Chameleon Animation Distance
  $description: "How far the gradient colors shift left and right during the animation. Range: 0 to 100. (Default: 20)"
- ChameleonRightSide: false
  $name: Glow from Right Side
  $description: "Toggle to reverse the gradient direction (originates from the right instead of the left)."
- ChameleonTransition: true
  $name: Smooth Color Transition
  $description: "Toggle to smoothly crossfade background colors when the track changes."

# ── Buttons & Controls ──────────────────────────────────────────────────────
- ButtonScale: "1"
  $name: Button Scale
  $description: "Multiplier for button size. (Default: 1.0)"
- HoverFadeSpeed: 15
  $name: Hover Fade Speed
  $description: "Speed of the hover animation on buttons. Higher values mean faster transitions. (Default: 15)"
- ShowShuffleRepeat: true
  $name: Show Shuffle/Repeat Icons
  $description: "Toggle to display tiny shuffle and repeat icons on the right side if the media player supports them."
- EnableScrubbing: true
  $name: Progress Bar Scrubbing
  $description: "Toggle to allow clicking on the progress bar to seek/scrub through the track."

# ── Progress Bar ───────────────────────────────────────────────────────────
- ShowProgressBar: true
  $name: Show Progress Bar
  $description: "Toggle to display a thin progress line at the bottom of the widget."
- ProgressBarColor: "1DB954"
  $name: Progress Bar Color
  $description: "Color of the progress bar in hex format. (Default: 1DB954 - Spotify Green). Ignored when Chameleon Progress Bar is enabled."
- ProgressBarChameleon: true
  $name: Chameleon Progress Bar
  $description: "When enabled, the progress bar uses the album art colors as a gradient instead of the fixed color above."

# ── Text & Scrolling ─────────────────────────────────────────────────────────
- FontSize: 13
  $name: Font Size
  $description: "Size of the track title and artist text. (Default: 13)"
- ScrollSpeed: 1
  $name: Text Scroll Speed
  $description: "How fast long text scrolls. Set to 0 to disable scrolling entirely. Range: 0 to 10. (Default: 1)"
- ScrollPause: 150
  $name: Text Scroll Pause
  $description: "How long to pause (in frames) before scrolling repeats. (Default: 150)"

# ── Animation & Performance ─────────────────────────────────────────────────────
- FadeAnimation: true
  $name: Fade In/Out Animation
  $description: "Toggle to smoothly fade the widget in and out when it appears or hides."
- TargetFPS: 60
  $name: Animation FPS
  $description: "Target frames per second for animations. Range: 10 to 144. (Default: 60)"
- ProgressBarFPS: 30
  $name: Progress Bar FPS
  $description: "Target frames per second exclusively for the progress bar. Set to 1 to tick every second (0% GPU usage). Range: 1 to 144. (Default: 30)"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <shcore.h>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <vector>
#include <map>
#include <wininet.h>
#include <urlmon.h>

// Direct2D and WIC headers (replaces GDI+)
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

const WCHAR* FONT_NAME = L"Segoe UI Variable Display";

typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
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

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

enum ZBID2 { ZBID2_IMMERSIVE_NOTIFICATION = 4 };
typedef HWND(WINAPI* pCreateWindowInBand)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID,DWORD);

struct ModSettings {
    int width = 300, height = 48, fontSize = 13;
    double buttonScale = 1.0;
    int hoverSpeed = 15;
    bool hideFullscreen = false;
    int idleTimeout = 0, offsetX = 80, offsetY = 0;
    int targetTaskbar = 0;
    int positionAnchor = 0;   // 0 = Left, 1 = Center, 2 = Right
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    int glassType = 1;
    DWORD glassTintColor = 0x000000;
    int glassOpacity = 0;
    int scrollSpeed = 1, scrollPause = 150;
    bool clickToOpen = true;
    bool clickToPause = false;
    bool showProgressBar = true;
    DWORD progressBarColor = 0xFF1DB954;
    bool chameleonProgressBar = false;
    bool hideWhenNoMedia = true;
    bool chameleonBg = true;
    bool dualChameleonBg = true;
    int chameleonVibrancy = 50;
    int chameleonPulse = 20;         // NEW: Amplitude of the breathing animation
    int chameleonIntensity = 175;
    bool animateChameleon = true;
    bool chameleonRightSide = false;
    bool chameleonTransition = true;
    int targetFPS = 60;
    int progressBarFPS = 30;
    bool showShuffleRepeat = false;
    bool fadeAnimation = true;
    bool enableScrubbing = true;
    bool enableVolumeScroll = true;
} g_Settings;

HWND g_hMediaWindow = NULL;
int g_HoverState = 0;
float g_HoverAlphas[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // 0: None, 1: Prev, 2: Play, 3: Next, 4: Progress
bool g_IsSuspended = false; // Smart Sleep Flag

HHOOK g_MouseHook = NULL;
DWORD g_MouseHookThreadId = 0; // Thread ID of the dedicated hook thread
HWINEVENTHOOK g_TaskbarHook = nullptr;
UINT g_TaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
int g_IdleSecondsCounter = 0;
bool g_IsHiddenByIdle = false;
bool g_LastVisibleState = true;
HWND g_CurrentTargetTaskbar = nullptr;

float g_FadeAlpha = 255.0f;
bool  g_FadingIn  = false;
bool  g_FadingOut = false;
#define IDT_FADE       1003
#define IDT_POLL_MEDIA 1001   
#define IDT_ANIMATION  1002   
#define APP_WM_CLOSE   WM_APP

// Direct2D Globals
ID2D1Factory* g_pD2DFactory    = nullptr;
IDWriteFactory* g_pDWriteFactory = nullptr;
IWICImagingFactory* g_pWICFactory    = nullptr;
ID2D1HwndRenderTarget* g_pRT            = nullptr;

// Memory optimization: Load TextFormats once
IDWriteTextFormat* g_pIconFormat      = nullptr;
IDWriteTextFormat* g_pSmallIconFormat = nullptr;
IDWriteTextFormat* g_pTextFormat      = nullptr;

D2D1_COLOR_F g_ChamFromColor{ 0, 0, 0, 1 };
D2D1_COLOR_F g_ChamToColor{ 0, 0, 0, 1 };
D2D1_COLOR_F g_ChamFromColor2{ 0, 0, 0, 1 };
D2D1_COLOR_F g_ChamToColor2{ 0, 0, 0, 1 };
float g_ChamTransitionT = 1.0f;
float g_ArtTransitionT = 1.0f;
float g_DpiScale = 1.0f;  // Set after window creation and refreshed on DPI change events

struct MediaState {
    wstring title  = L"Waiting for media...";
    wstring artist;
    wstring aumid;
    bool isPlaying = false, hasMedia = false;
    bool isShuffle = false;
    int  repeatMode = 0;
    IWICBitmapSource* albumArtWic = nullptr;
    bool artChanged = false;
    int64_t positionTicks   = 0;
    int64_t endTimeTicks    = 0;
    int64_t lastUpdatedTicks = 0;
    D2D1_COLOR_F chameleonColor{ 0, 0, 0, 1 };
    D2D1_COLOR_F chameleonColor2{ 0, 0, 0, 1 };
    int artSource = 0; // 0=None, 1=Loading, 2=Spotify, 3=iTunes
    mutex lock;
} g_MediaState;

float g_ScrollOffset = 0.0f;
float g_TextWidth    = 0.0f;
bool  g_IsScrolling  = false;
int   g_ScrollWait   = 0;
float g_AnimFrameCounter = 0.0f;
bool  g_PendingArtRefresh = false;      // Track changed while widget was off-screen; art needs to be pre-fetched immediately.
bool  g_SkipNextArtTransition = false;  // When set, DrawMediaPanelD2D will show new art instantly without crossfade.

#define APP_WM_MEDIA_UPDATE (WM_APP + 11)
winrt::event_token g_SessionChangedToken{};
winrt::event_token g_PlaybackInfoToken{};
winrt::event_token g_MediaPropsToken{};
GlobalSystemMediaTransportControlsSession g_CurrentSession = nullptr;
GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;


// --- Direct2D Helper Functions ---
void RecreateTextFormats() {
    if (g_pIconFormat) { g_pIconFormat->Release(); g_pIconFormat = nullptr; }
    if (g_pSmallIconFormat) { g_pSmallIconFormat->Release(); g_pSmallIconFormat = nullptr; }
    if (g_pTextFormat) { g_pTextFormat->Release(); g_pTextFormat = nullptr; }
    if (!g_pDWriteFactory) return;

    // Font sizes are specified in the user's settings as physical pixels — no DPI multiplier needed.
    g_pDWriteFactory->CreateTextFormat(L"Segoe Fluent Icons", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f * (float)g_Settings.buttonScale, L"en-US", &g_pIconFormat);
    if (g_pIconFormat) {
        g_pIconFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        g_pIconFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    g_pDWriteFactory->CreateTextFormat(L"Segoe Fluent Icons", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 9.0f, L"en-US", &g_pSmallIconFormat);
    if (g_pSmallIconFormat) {
        g_pSmallIconFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    g_pDWriteFactory->CreateTextFormat(FONT_NAME, NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (float)g_Settings.fontSize, L"en-US", &g_pTextFormat);
    if (g_pTextFormat) {
        g_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}

void InitD2D() {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_pDWriteFactory);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pWICFactory));
    RecreateTextFormats();
}

void CleanupD2D() {
    if (g_pIconFormat) { g_pIconFormat->Release(); g_pIconFormat = nullptr; }
    if (g_pSmallIconFormat) { g_pSmallIconFormat->Release(); g_pSmallIconFormat = nullptr; }
    if (g_pTextFormat) { g_pTextFormat->Release(); g_pTextFormat = nullptr; }
    if (g_pRT) { g_pRT->Release(); g_pRT = nullptr; }
    if (g_pWICFactory) { g_pWICFactory->Release(); g_pWICFactory = nullptr; }
    if (g_pDWriteFactory) { g_pDWriteFactory->Release(); g_pDWriteFactory = nullptr; }
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
}

// --- Color Extraction Helpers ---
float Min3(float a, float b, float c) { return min(min(a, b), c); }
float Max3(float a, float b, float c) { return max(max(a, b), c); }
void RGBtoHSL(float r, float g, float b, float& h, float& s, float& l) {
    float mx = Max3(r, g, b), mn = Min3(r, g, b);
    l = (mx + mn) / 2.0f;
    if (mx == mn) { h = 0.0f; s = 0.0f; } else {
        float d = mx - mn;
        s = l > 0.5f ? d / (2.0f - mx - mn) : d / (mx + mn);
        if (mx == r) h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        else if (mx == g) h = (b - r) / d + 2.0f;
        else h = (r - g) / d + 4.0f;
        h /= 6.0f;
    }
}
void HSLtoRGB(float h, float s, float l, float& r, float& g, float& b) {
    auto hue2rgb = [](float p, float q, float t) {
        if (t < 0.0f) t += 1.0f;
        if (t > 1.0f) t -= 1.0f;
        if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f) return q;
        if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        return p;
    };
    if (s == 0.0f) { r = g = b = l; } else {
        float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;
        r = hue2rgb(p, q, h + 1.0f / 3.0f);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0f / 3.0f);
    }
}

D2D1_COLOR_F AdjustColorVibrancy(D2D1_COLOR_F c) {
    if (g_Settings.chameleonVibrancy == 100) return c;
    float h, s, l;
    RGBtoHSL(c.r, c.g, c.b, h, s, l);
    s *= ((float)g_Settings.chameleonVibrancy / 100.0f);
    s = max(0.0f, min(1.0f, s));
    float r, g, b;
    HSLtoRGB(h, s, l, r, g, b);
    return D2D1::ColorF(r, g, b, c.a);
}

D2D1_COLOR_F GetFallbackAccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        float r = ((color >> 16) & 0xFF) / 255.f;
        float g = ((color >> 8) & 0xFF) / 255.f;
        float b = (color & 0xFF) / 255.f;
        return D2D1::ColorF(r, g, b, 1.0f);
    }
    return D2D1::ColorF(0.5f, 0.5f, 0.5f, 1.0f);
}

D2D1_COLOR_F ShiftHue(D2D1_COLOR_F c, float hueShift) {
    float h, s, l;
    RGBtoHSL(c.r, c.g, c.b, h, s, l);
    h += hueShift;
    if (h > 1.0f) h -= 1.0f;
    if (h < 0.0f) h += 1.0f;
    float r, g, b;
    HSLtoRGB(h, s, l, r, g, b);
    return D2D1::ColorF(r, g, b, c.a);
}

std::pair<D2D1_COLOR_F, D2D1_COLOR_F> ExtractChameleonColorWIC(IWICBitmapSource* pSource) {
    D2D1_COLOR_F fallback = GetFallbackAccentColor();
    if (!pSource || !g_pWICFactory) return {fallback, ShiftHue(fallback, 0.1f)};
    UINT w = 0, h = 0;
    pSource->GetSize(&w, &h);
    if (w == 0 || h == 0) return {fallback, ShiftHue(fallback, 0.1f)};

    IWICFormatConverter* pConverter = nullptr;
    g_pWICFactory->CreateFormatConverter(&pConverter);
    pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);

    UINT stride = w * 4;
    UINT size = stride * h;
    std::vector<BYTE> buffer(size);
    pConverter->CopyPixels(NULL, stride, size, buffer.data());
    pConverter->Release();

    // Denser sampling (32x32 grid approx) to build a color frequency histogram
    UINT stepY = max(1u, h / 32);
    UINT stepX = max(1u, w / 32);
    int hueCounts[36] = {0}; // 36 bins of 10 degrees each
    int totalValid = 0;

    // Pass 1: Build Hue Histogram to find dominant color families
    for (UINT y = 0; y < h; y += stepY) {
        for (UINT x = 0; x < w; x += stepX) {
            UINT idx = (y * stride) + (x * 4);
            float b_val = buffer[idx] / 255.f, g_val = buffer[idx + 1] / 255.f, r_val = buffer[idx + 2] / 255.f;
            float h_val, s_val, l_val;
            RGBtoHSL(r_val, g_val, b_val, h_val, s_val, l_val);
            if (l_val < 0.15f || l_val > 0.85f || s_val < 0.1f) continue;
            int h_bin = (int)(h_val * 36.0f) % 36;
            hueCounts[h_bin]++;
            totalValid++;
        }
    }

    float bestScore1 = -1.0f;
    D2D1_COLOR_F bestColor1 = fallback;
    float bestHue1 = 0.0f;

    // Pass 2: Find most vibrant DOMINANT color
    for (UINT y = 0; y < h; y += stepY) {
        for (UINT x = 0; x < w; x += stepX) {
            UINT idx = (y * stride) + (x * 4);
            float b_val = buffer[idx] / 255.f, g_val = buffer[idx + 1] / 255.f, r_val = buffer[idx + 2] / 255.f;
            float h_val, s_val, l_val;
            RGBtoHSL(r_val, g_val, b_val, h_val, s_val, l_val);
            
            if (l_val < 0.15f || l_val > 0.85f) continue;
            
            int h_bin = (int)(h_val * 36.0f) % 36;
            float freq = totalValid > 0 ? (float)hueCounts[h_bin] / (float)totalValid : 0.0f;
            
            // Score = Saturation * Lightness balance * Frequency.
            // A color MUST be somewhat frequent to beat a slightly more saturated outlier.
            float score = s_val * (1.0f - abs(l_val - 0.5f) * 2.0f) * (freq + 0.05f);
            
            if (score > bestScore1) { 
                bestScore1 = score; 
                bestColor1 = D2D1::ColorF(r_val, g_val, b_val, 1.f); 
                bestHue1 = h_val; 
            }
        }
    }
    
    // If image is mostly black/white (no vibrant colors), average the image to get a fallback
    if (bestScore1 == -1.0f) {
        long long rSum = 0, gSum = 0, bSum = 0, count = 0;
        for (UINT y = 0; y < h; y += stepY) {
            for (UINT x = 0; x < w; x += stepX) {
                UINT idx = (y * stride) + (x * 4);
                bSum += buffer[idx]; gSum += buffer[idx + 1]; rSum += buffer[idx + 2]; count++;
            }
        }
        if (count > 0) {
            bestColor1 = D2D1::ColorF((float)(rSum/count)/255.f, (float)(gSum/count)/255.f, (float)(bSum/count)/255.f, 1.0f);
            float h_val, s_val, l_val;
            RGBtoHSL(bestColor1.r, bestColor1.g, bestColor1.b, h_val, s_val, l_val);
            if (s_val < 0.1f || l_val < 0.15f || l_val > 0.85f) bestColor1 = fallback;
        }
    }

    float bestScore2 = -1.0f;
    D2D1_COLOR_F bestColor2 = ShiftHue(bestColor1, 0.1f); 
    
    // Pass 3: Find contrasting secondary color
    if (bestScore1 != -1.0f) {
        for (UINT y = 0; y < h; y += stepY) {
            for (UINT x = 0; x < w; x += stepX) {
                UINT idx = (y * stride) + (x * 4);
                float b_val = buffer[idx] / 255.f, g_val = buffer[idx + 1] / 255.f, r_val = buffer[idx + 2] / 255.f;
                float h_val, s_val, l_val;
                RGBtoHSL(r_val, g_val, b_val, h_val, s_val, l_val);
                
                if (l_val < 0.15f || l_val > 0.85f) continue;
                
                float hueDist = abs(h_val - bestHue1);
                if (hueDist > 0.5f) hueDist = 1.0f - hueDist;
                if (hueDist < 0.15f) continue; // Require minimum hue distance
                
                int h_bin = (int)(h_val * 36.0f) % 36;
                float freq = totalValid > 0 ? (float)hueCounts[h_bin] / (float)totalValid : 0.0f;
                
                float score = s_val * (1.0f - abs(l_val - 0.5f) * 2.0f) * (freq + 0.05f) * (hueDist + 0.2f);
                if (score > bestScore2) { 
                    bestScore2 = score; 
                    bestColor2 = D2D1::ColorF(r_val, g_val, b_val, 1.f); 
                }
            }
        }
    }
    
    return {bestColor1, bestColor2};
}

// Compute a lightweight fingerprint from 9 sampled pixels across the bitmap.
// Used to detect when Spotify returns a cached thumbnail from the previous song.
uint32_t QuickFingerprint(IWICBitmapSource* pSrc) {
    if (!pSrc) return 0;
    UINT w = 0, h = 0;
    pSrc->GetSize(&w, &h);
    if (w < 4 || h < 4) return 0;
    uint32_t fp = 0;
    UINT xs[] = { 0, w/2, w-1 };
    UINT ys[] = { 0, h/2, h-1 };
    for (UINT yi = 0; yi < 3; yi++) {
        for (UINT xi = 0; xi < 3; xi++) {
            WICRect rc = { (INT)xs[xi], (INT)ys[yi], 1, 1 };
            BYTE px[4] = {};
            if (SUCCEEDED(pSrc->CopyPixels(&rc, 4, 4, px)))
                fp ^= ((uint32_t)px[0] | ((uint32_t)px[1] << 8) | ((uint32_t)px[2] << 16)) + (xi + yi * 3 + 1) * 0x9e3779b9u;
        }
    }
    return fp;
}

// Decode and scale a thumbnail stream to targetSize x targetSize using the WIC Fant filter.
// Fant is an area-averaging filter — the best choice for large downscales (e.g. 640→36px).
// ─── Fallback Art via iTunes API ───────────────────────────────────────────────────
#include <sstream>
#include <iomanip>

#define APP_WM_FALLBACK_ART_READY (WM_APP + 5)

struct FallbackArtResult {
    wstring title;
    wstring artist;
    wstring imagePath;
};

// URL-encodes a UTF-8 string for iTunes Search API
string URLEncode(const string& value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;
    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else if (c == ' ') {
            escaped << '+';
        } else {
            escaped << uppercase << '%' << setw(2) << int((unsigned char)c) << nouppercase;
        }
    }
    return escaped.str();
}

wstring FetchUrlContent(const wstring& url) {
    HINTERNET hInternet = InternetOpenW(L"WinINet", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return L"";
    HINTERNET hUrl = InternetOpenUrlW(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hUrl) { InternetCloseHandle(hInternet); return L""; }

    string response;
    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    if (response.empty()) return L"";
    int wlen = MultiByteToWideChar(CP_UTF8, 0, response.c_str(), -1, NULL, 0);
    wstring wresponse(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, response.c_str(), -1, &wresponse[0], wlen);
    return wresponse;
}

void FetchFallbackArtAsync(wstring title, wstring artist) {
    thread([title, artist]() {
        if (title.empty() && artist.empty()) return;
        
        string query = "";
        int len = WideCharToMultiByte(CP_UTF8, 0, artist.c_str(), -1, NULL, 0, NULL, NULL);
        if (len > 1) {
            string a(len, 0);
            WideCharToMultiByte(CP_UTF8, 0, artist.c_str(), -1, &a[0], len, NULL, NULL);
            if (a.back() == '\0') a.pop_back();
            query += a + " ";
        }
        len = WideCharToMultiByte(CP_UTF8, 0, title.c_str(), -1, NULL, 0, NULL, NULL);
        if (len > 1) {
            string t(len, 0);
            WideCharToMultiByte(CP_UTF8, 0, title.c_str(), -1, &t[0], len, NULL, NULL);
            if (t.back() == '\0') t.pop_back();
            query += t;
        }

        string encodedQuery = URLEncode(query);
        wstring url = L"https://itunes.apple.com/search?term=";
        int wlen = MultiByteToWideChar(CP_UTF8, 0, encodedQuery.c_str(), -1, NULL, 0);
        if (wlen > 1) {
            wstring wquery(wlen, 0);
            MultiByteToWideChar(CP_UTF8, 0, encodedQuery.c_str(), -1, &wquery[0], wlen);
            if (wquery.back() == '\0') wquery.pop_back();
            url += wquery;
        }
        url += L"&entity=song&limit=1";

        wstring json = FetchUrlContent(url);
        if (json.empty()) return;

        wstring search = L"\"artworkUrl100\":\"";
        size_t pos = json.find(search);
        if (pos == wstring::npos) return;
        pos += search.length();
        size_t endPos = json.find(L"\"", pos);
        if (endPos == wstring::npos) return;
        
        wstring artUrl = json.substr(pos, endPos - pos);
        // Replace 100x100bb with 512x512bb for high-res art
        size_t rep = artUrl.find(L"100x100bb");
        if (rep != wstring::npos) {
            artUrl.replace(rep, 9, L"512x512bb");
        }

        WCHAR tempPath[MAX_PATH];
        GetTempPathW(MAX_PATH, tempPath);
        WCHAR tempFile[MAX_PATH];
        GetTempFileNameW(tempPath, L"ART", 0, tempFile);

        HRESULT hr = URLDownloadToFileW(NULL, artUrl.c_str(), tempFile, 0, NULL);
        if (SUCCEEDED(hr)) {
            FallbackArtResult* res = new FallbackArtResult{title, artist, tempFile};
            PostMessage(g_hMediaWindow, APP_WM_FALLBACK_ART_READY, 0, (LPARAM)res);
        }
    }).detach();
}

IWICBitmapSource* LoadWICBitmapFromFile(LPCWSTR filename, UINT targetSize) {
    if (!g_pWICFactory || targetSize == 0) return nullptr;
    
    IWICBitmapDecoder* pDecoder = nullptr;
    if (FAILED(g_pWICFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder))) return nullptr;

    IWICBitmap* pResult = nullptr;
    IWICBitmapFrameDecode* pFrame = nullptr;
    if (SUCCEEDED(pDecoder->GetFrame(0, &pFrame))) {
        IWICBitmapScaler* pScaler = nullptr;
        g_pWICFactory->CreateBitmapScaler(&pScaler);
        if (pScaler && SUCCEEDED(pScaler->Initialize(pFrame, targetSize, targetSize, WICBitmapInterpolationModeFant))) {
            IWICFormatConverter* pConverter = nullptr;
            g_pWICFactory->CreateFormatConverter(&pConverter);
            if (pConverter) {
                pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
                g_pWICFactory->CreateBitmapFromSource(pConverter, WICBitmapCacheOnLoad, &pResult);
                pConverter->Release();
            }
        }
        if (pScaler) pScaler->Release();
        pFrame->Release();
    }
    pDecoder->Release();
    return pResult;
}

// The result is fully materialized in RAM (WICBitmapCacheOnLoad) so no lazy-decode overhead
// at render time. Caller must Release() the returned pointer.
IWICBitmapSource* StreamToWICBitmap(IRandomAccessStreamWithContentType const& stream, UINT targetSize) {
    if (!stream || !g_pWICFactory || targetSize == 0) return nullptr;
    try {
        uint64_t size = stream.Size();
        if (size == 0 || size > 32 * 1024 * 1024) return nullptr;

        DataReader reader(stream);
        reader.LoadAsync((uint32_t)size).get();
        std::vector<uint8_t> buf(size);
        reader.ReadBytes(buf);
        reader.Close();

        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
        if (!hMem) return nullptr;
        memcpy(GlobalLock(hMem), buf.data(), size);
        GlobalUnlock(hMem);

        IStream* istream = nullptr;
        if (FAILED(CreateStreamOnHGlobal(hMem, TRUE, &istream))) { GlobalFree(hMem); return nullptr; }

        IWICBitmap* pResult = nullptr;
        IWICBitmapDecoder* pDecoder = nullptr;
        if (SUCCEEDED(g_pWICFactory->CreateDecoderFromStream(istream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder))) {
            IWICBitmapFrameDecode* pFrame = nullptr;
            if (SUCCEEDED(pDecoder->GetFrame(0, &pFrame))) {
                IWICBitmapScaler* pScaler = nullptr;
                g_pWICFactory->CreateBitmapScaler(&pScaler);
                if (pScaler && SUCCEEDED(pScaler->Initialize(pFrame, targetSize, targetSize, WICBitmapInterpolationModeFant))) {
                    IWICFormatConverter* pConverter = nullptr;
                    g_pWICFactory->CreateFormatConverter(&pConverter);
                    if (pConverter) {
                        pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
                        g_pWICFactory->CreateBitmapFromSource(pConverter, WICBitmapCacheOnLoad, &pResult);
                        pConverter->Release();
                    }
                }
                if (pScaler) pScaler->Release();
                pFrame->Release();
            }
            pDecoder->Release();
        }
        istream->Release();
        return pResult;
    } catch (...) { return nullptr; }
}


// --- Multi-Monitor Helper ---
struct MonitorSearch { HMONITOR target; HWND result; };
BOOL CALLBACK EnumMonWnd(HWND hwnd, LPARAM lParam) {
    MonitorSearch* ms = (MonitorSearch*)lParam;
    WCHAR cls[64]; GetClassNameW(hwnd, cls, 64);
    if (wcscmp(cls, L"Shell_TrayWnd") == 0 || wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) == ms->target) {
            ms->result = hwnd; return FALSE;
        }
    }
    return TRUE;
}

HWND GetTargetTaskbar() {
    if (g_Settings.targetTaskbar == 1) {
        POINT pt; GetCursorPos(&pt);
        MonitorSearch ms = { MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), nullptr };
        EnumWindows(EnumMonWnd, (LPARAM)&ms);
        if (ms.result) return ms.result;
    }
    HWND p = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!p) p = FindWindow(L"Shell_SecondaryTrayWnd", nullptr);
    return p;
}


// --- Existing Settings Logic ---
void LoadSettings() {
    g_Settings.width         = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height        = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize      = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX       = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY       = Wh_GetIntSetting(L"OffsetY");
    g_Settings.targetTaskbar = Wh_GetIntSetting(L"TargetTaskbar");
    g_Settings.positionAnchor = Wh_GetIntSetting(L"PositionAnchor");
    g_Settings.autoTheme     = Wh_GetIntSetting(L"AutoTheme") != 0;
    g_Settings.glassType     = Wh_GetIntSetting(L"GlassType");
    g_Settings.glassOpacity  = max(0, min(255, Wh_GetIntSetting(L"GlassOpacity")));
    g_Settings.scrollSpeed   = max(0, min(10, Wh_GetIntSetting(L"ScrollSpeed")));
    g_Settings.scrollPause   = max(0, Wh_GetIntSetting(L"ScrollPause"));
    g_Settings.enableVolumeScroll = Wh_GetIntSetting(L"EnableVolumeScroll") != 0;
    g_Settings.clickToOpen     = Wh_GetIntSetting(L"ClickToOpen") != 0;
    g_Settings.clickToPause    = Wh_GetIntSetting(L"ClickToPause") != 0;
    g_Settings.showProgressBar = Wh_GetIntSetting(L"ShowProgressBar") != 0;
    g_Settings.chameleonProgressBar = Wh_GetIntSetting(L"ProgressBarChameleon") != 0;
    g_Settings.hideWhenNoMedia = Wh_GetIntSetting(L"HideWhenNoMedia") != 0;
    g_Settings.chameleonBg     = Wh_GetIntSetting(L"ChameleonBg") != 0;
    g_Settings.dualChameleonBg = Wh_GetIntSetting(L"DualChameleonBg") != 0;
    g_Settings.chameleonVibrancy = max(0, min(200, Wh_GetIntSetting(L"ChameleonVibrancy")));
    g_Settings.chameleonIntensity = max(0, min(255, Wh_GetIntSetting(L"ChameleonIntensity")));
    g_Settings.chameleonPulse  = max(0, min(255, Wh_GetIntSetting(L"ChameleonPulse"))); // NEW
    g_Settings.animateChameleon = Wh_GetIntSetting(L"AnimateChameleon") != 0;
    g_Settings.chameleonRightSide = Wh_GetIntSetting(L"ChameleonRightSide") != 0;
    g_Settings.chameleonTransition = Wh_GetIntSetting(L"ChameleonTransition") != 0;
    g_Settings.showShuffleRepeat = Wh_GetIntSetting(L"ShowShuffleRepeat") != 0;
    g_Settings.fadeAnimation   = Wh_GetIntSetting(L"FadeAnimation") != 0;
    g_Settings.enableScrubbing = Wh_GetIntSetting(L"EnableScrubbing") != 0;

    int fps = Wh_GetIntSetting(L"TargetFPS");
    g_Settings.targetFPS = (fps >= 10 && fps <= 144) ? fps : 60;
    
    int pFps = Wh_GetIntSetting(L"ProgressBarFPS");
    g_Settings.progressBarFPS = (pFps >= 1 && pFps <= 144) ? pFps : 30;
    
    int hSpeed = Wh_GetIntSetting(L"HoverFadeSpeed");
    g_Settings.hoverSpeed = hSpeed > 0 ? hSpeed : 15;

    PCWSTR s = Wh_GetStringSetting(L"ButtonScale");
    g_Settings.buttonScale = s ? max(0.5, min(4.0, _wtof(s))) : 1.0;
    if (s) Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"TextColor");
    g_Settings.manualTextColor = 0xFF000000 | (s && wcslen(s) ? wcstoul(s, nullptr, 16) : 0xFFFFFF);
    if (s) Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"GlassTintColor");
    g_Settings.glassTintColor = (s && wcslen(s)) ? wcstoul(s, nullptr, 16) : 0x000000;
    if (s) Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"ProgressBarColor");
    g_Settings.progressBarColor = 0xFF000000 | (s && wcslen(s) ? wcstoul(s, nullptr, 16) : 0x1DB954);
    if (s) Wh_FreeStringSetting(s);

    if (g_Settings.width  < 100) g_Settings.width  = 300;
    if (g_Settings.height < 24)  g_Settings.height = 48;
}

bool IsSystemLightMode() {
    DWORD v = 0, sz = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &v, &sz);
    return v != 0;
}

D2D1_COLOR_F GetTextColorD2D() {
    DWORD hex = g_Settings.autoTheme ? (IsSystemLightMode() ? 0xFF010101 : 0xFFFFFFFF) : g_Settings.manualTextColor;
    return D2D1::ColorF(((hex >> 16) & 0xFF) / 255.f, ((hex >> 8) & 0xFF) / 255.f, (hex & 0xFF) / 255.f, 1.0f);
}

DWORD MakeTint(DWORD rgb, BYTE alpha) {
    BYTE r = (rgb >> 16) & 0xFF, g2 = (rgb >> 8) & 0xFF, b = rgb & 0xFF;
    return ((DWORD)alpha << 24) | ((DWORD)b << 16) | ((DWORD)g2 << 8) | r;
}

void UpdateAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    int backdropNone = 1;
    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropNone, sizeof(backdropNone));

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    auto SetComp = hUser ? (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute") : nullptr;

    DWORD tintRGB = g_Settings.autoTheme ? (IsSystemLightMode() ? 0xEEEEEE : 0x1a1a1a) : g_Settings.glassTintColor;
    BYTE opacity = (BYTE)g_Settings.glassOpacity;

    if (g_Settings.glassType == 3) {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        int backdropMica = 2; // DWMSBT_MAINWINDOW
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropMica, sizeof(backdropMica));
        BOOL dark = !IsSystemLightMode();
        DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    } else if (g_Settings.glassType == 2 || g_Settings.glassType == 1) {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        if (g_Settings.glassType == 2) {
            int backdropAcrylic = 3; // DWMSBT_TRANSIENTWINDOW (Win11 Acrylic)
            DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropAcrylic, sizeof(backdropAcrylic));
        }
        if (SetComp) {
            ACCENT_STATE st = (g_Settings.glassType == 2) ? ACCENT_ENABLE_ACRYLICBLURBEHIND : ACCENT_ENABLE_BLURBEHIND;
            ACCENT_POLICY policy = { st, 2, MakeTint(tintRGB, opacity), 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
            SetComp(hwnd, &data);
        }
    } else {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        int backdropNone = 1; // DWMSBT_NONE
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropNone, sizeof(backdropNone));
    }

    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, (BYTE)(UINT)g_FadeAlpha, LWA_ALPHA);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}


// --- Media Logic ---
void SubscribeToSession(GlobalSystemMediaTransportControlsSession const& session) {
    if (g_CurrentSession == session) return;
    if (g_CurrentSession) {
        try {
            if (g_PlaybackInfoToken.value != 0) g_CurrentSession.PlaybackInfoChanged(g_PlaybackInfoToken);
            if (g_MediaPropsToken.value != 0) g_CurrentSession.MediaPropertiesChanged(g_MediaPropsToken);
        } catch (...) {}
        // Fix: reset tokens after unsubscribing so they are never reused as stale handles.
        g_PlaybackInfoToken = {};
        g_MediaPropsToken = {};
    }
    g_CurrentSession = session;
    if (g_CurrentSession) {
        g_PlaybackInfoToken = g_CurrentSession.PlaybackInfoChanged([](auto&&, auto&&) {
            if (g_hMediaWindow) PostMessage(g_hMediaWindow, APP_WM_MEDIA_UPDATE, 0, 0);
        });
        g_MediaPropsToken = g_CurrentSession.MediaPropertiesChanged([](auto&&, auto&&) {
            if (g_hMediaWindow) PostMessage(g_hMediaWindow, APP_WM_MEDIA_UPDATE, 0, 0);
        });
    }
}

void RegisterMediaEvents() {
    if (!g_SessionManager) return;
    if (g_SessionChangedToken.value != 0) return;  // Fix: event_token has no operator bool; check .value explicitly
    g_SessionChangedToken = g_SessionManager.CurrentSessionChanged([](auto&&, auto&&) {
        if (g_hMediaWindow) PostMessage(g_hMediaWindow, APP_WM_MEDIA_UPDATE, 0, 0);
    });
}

void UpdateMediaInfo() {
    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            // Fix: register the CurrentSessionChanged event as soon as the manager is available.
            // Previously this was only done once from WM_CREATE, so if the manager initialized
            // late (via the poll timer), new media sessions were never detected reactively.
            RegisterMediaEvents();
        }
        if (!g_SessionManager) return;

        GlobalSystemMediaTransportControlsSession session = nullptr;
        for (auto const& s : g_SessionManager.GetSessions()) {
            auto pb = s.GetPlaybackInfo();
            if (pb && pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) { session = s; break; }
        }
        if (!session) session = g_SessionManager.GetCurrentSession();

        if (session) {
            SubscribeToSession(session);
            auto props = session.TryGetMediaPropertiesAsync().get();
            auto info = session.GetPlaybackInfo();
            auto timeline = session.GetTimelineProperties();

            lock_guard<mutex> guard(g_MediaState.lock);
            wstring newTitle = props.Title().c_str();
            bool titleChanged = (newTitle != g_MediaState.title);

            static auto lastTitleChange = std::chrono::steady_clock::now();
            static bool artNeedsUpdate = false;
            static int artRetryCount = 0;
            static uint32_t prevAcceptedFingerprint = 0;
            static bool prevFingerprintValid = false; // false at startup: no previous art to compare against

            if (titleChanged) {
                lastTitleChange = std::chrono::steady_clock::now();
                artNeedsUpdate = true;
                artRetryCount = 0;
                g_MediaState.artSource = 1; // 1 = Loading
                // Capture the fingerprint of the CURRENT art before the song changes.
                // This is what we'll compare against to detect Spotify's stale thumbnail cache.
                if (g_MediaState.albumArtWic) {
                    prevAcceptedFingerprint = QuickFingerprint(g_MediaState.albumArtWic);
                    prevFingerprintValid = true;
                } else {
                    prevFingerprintValid = false;
                }
            }

            auto now = std::chrono::steady_clock::now();
            bool isOffScreen = (MonitorFromWindow(g_hMediaWindow, MONITOR_DEFAULTTONULL) == NULL);
            
            bool delayPassed = false;
            if (artNeedsUpdate) {
                // Aggressive polling phase (every ~500ms) for the first 5 seconds
                delayPassed = isOffScreen
                    ? (now - lastTitleChange) > std::chrono::milliseconds(artRetryCount * 500)
                    : (now - lastTitleChange) > std::chrono::milliseconds(150 + artRetryCount * 500);
            } else if (!g_MediaState.albumArtWic) {
                // Passive polling phase (every 1.5s) if we gave up but still have no art
                delayPassed = (now - lastTitleChange) > std::chrono::milliseconds(1500 + artRetryCount * 1500);
            }

            if (delayPassed) {
                IWICBitmapSource* newArt = nullptr;
                D2D1_COLOR_F newChamColor = GetFallbackAccentColor();
                D2D1_COLOR_F newChamColor2 = ShiftHue(newChamColor, 0.1f);

                try {
                    auto thumbRef = props.Thumbnail();
                    if (thumbRef) {
                        auto streamOp = thumbRef.OpenReadAsync();
                        auto stream = streamOp.get();
                        if (stream && stream.Size() > 0) {
                            UINT artSize = max(24u, (UINT)g_Settings.height - 12);
                            UINT artLoadSize = min(512u, artSize * 4);
                            newArt = StreamToWICBitmap(stream, artLoadSize);
                        }
                        if (newArt && g_Settings.chameleonBg) {
                            auto colors = ExtractChameleonColorWIC(newArt);
                            newChamColor = colors.first;
                            newChamColor2 = colors.second;
                        }
                    }
                } catch (...) {
                    // Spotify can throw during OpenReadAsync().get() if the thumbnail
                    // isn't ready yet. newArt stays nullptr → retry logic below handles it.
                }

                // Stale-thumbnail detection
                if (newArt) {
                    uint32_t fp = QuickFingerprint(newArt);
                    // Reject stale covers for up to 10 retries (5 seconds)
                    bool isStale = prevFingerprintValid && (fp == prevAcceptedFingerprint);
                    if (isStale && artRetryCount < 10) {
                        newArt->Release();
                        newArt = nullptr;
                    } else {
                        prevAcceptedFingerprint = fp;
                        prevFingerprintValid = true;
                    }
                }

                // Accept or retry
                if (newArt) {
                    if (g_MediaState.albumArtWic) g_MediaState.albumArtWic->Release();
                    g_MediaState.albumArtWic = newArt;
                    g_MediaState.artChanged = true;

                    if (g_Settings.chameleonTransition &&
                        (newChamColor.r != g_MediaState.chameleonColor.r || newChamColor.g != g_MediaState.chameleonColor.g || newChamColor.b != g_MediaState.chameleonColor.b)) {
                        g_ChamFromColor = g_MediaState.chameleonColor;
                        g_ChamToColor = newChamColor;
                        g_ChamFromColor2 = g_MediaState.chameleonColor2;
                        g_ChamToColor2 = newChamColor2;
                        g_ChamTransitionT = 0.0f;
                    } else { g_ChamTransitionT = 1.0f; }
                    g_MediaState.chameleonColor = newChamColor;
                    g_MediaState.chameleonColor2 = newChamColor2;
                    g_MediaState.artSource = 2; // 2 = Spotify
                    
                    artNeedsUpdate = false;
                    artRetryCount = 0;
                    if (isOffScreen) g_PendingArtRefresh = true;
                } else {
                    artRetryCount++;
                    
                    // CLEAR the wrong cover after 3 retries (~1.5 seconds).
                    // This is the creative fix: it prevents the user from staring at the wrong cover.
                    if (artRetryCount == 3) {
                        if (g_MediaState.albumArtWic) {
                            g_MediaState.albumArtWic->Release();
                            g_MediaState.albumArtWic = nullptr;
                            g_MediaState.artChanged = true;
                            g_MediaState.chameleonColor = GetFallbackAccentColor();
                            g_MediaState.chameleonColor2 = ShiftHue(g_MediaState.chameleonColor, 0.1f);
                            g_MediaState.artSource = 0; // 0 = None
                        }
                        // The REAL fix: bypass Spotify entirely by downloading high-res art from iTunes!
                        FetchFallbackArtAsync(g_MediaState.title, g_MediaState.artist);
                    }

                    // After 10 aggressive retries (~5 seconds), switch to passive polling.
                    if (artRetryCount >= 10) {
                        artNeedsUpdate = false; 
                    }
                }
            }

            g_MediaState.title = newTitle;
            g_MediaState.artist = props.Artist().c_str();
            g_MediaState.aumid = session.SourceAppUserModelId().c_str();
            g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            g_MediaState.hasMedia = true;

            try { auto shuffle = info.IsShuffleActive(); g_MediaState.isShuffle = shuffle && shuffle.Value(); } catch (...) { g_MediaState.isShuffle = false; }
            try {
                auto repeat = info.AutoRepeatMode();
                if (repeat) {
                    using R = Windows::Media::MediaPlaybackAutoRepeatMode;
                    switch (repeat.Value()) {
                        case R::List: g_MediaState.repeatMode = 1; break;
                        case R::Track: g_MediaState.repeatMode = 2; break;
                        default: g_MediaState.repeatMode = 0; break;
                    }
                } else g_MediaState.repeatMode = 0;
            } catch (...) { g_MediaState.repeatMode = 0; }

            if (timeline) {
                g_MediaState.positionTicks = timeline.Position().count();
                g_MediaState.endTimeTicks = timeline.EndTime().count();
                g_MediaState.lastUpdatedTicks = timeline.LastUpdatedTime().time_since_epoch().count();
            } else g_MediaState.endTimeTicks = 0;

        } else {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
            g_MediaState.title = L"No Media";
            g_MediaState.artist = L"";
            if (g_MediaState.albumArtWic) { g_MediaState.albumArtWic->Release(); g_MediaState.albumArtWic = nullptr; }
            g_MediaState.artChanged = true;
            g_MediaState.chameleonColor = GetFallbackAccentColor();
            g_MediaState.chameleonColor2 = ShiftHue(g_MediaState.chameleonColor, 0.1f);
            g_MediaState.isShuffle = false;
            g_MediaState.repeatMode = 0;
            SubscribeToSession(nullptr);
        }
    } catch (...) { 
        lock_guard<mutex> guard(g_MediaState.lock); 
        g_MediaState.hasMedia = false; 
        
        // If a WinRT COM exception occurs (e.g. after sleep or a media player restart),
        // the session manager becomes permanently invalid. We MUST nullify it to force
        // a clean re-initialization on the next poll timer tick.
        if (g_SessionManager) {
            if (g_SessionChangedToken.value != 0) {
                try { g_SessionManager.CurrentSessionChanged(g_SessionChangedToken); } catch (...) {}
                g_SessionChangedToken = {};
            }
            g_SessionManager = nullptr;
        }
        SubscribeToSession(nullptr);
    }
}

void SendMediaCommand(int cmd) {
    try {
        if (!g_SessionManager) return;
        auto s = g_SessionManager.GetCurrentSession();
        if (!s) return;
        if (cmd == 1) s.TrySkipPreviousAsync();
        else if (cmd == 2) s.TryTogglePlayPauseAsync();
        else if (cmd == 3) s.TrySkipNextAsync();
    } catch (...) {}
}
void SeekToPosition(float fraction) {
    try {
        if (!g_SessionManager) return;
        auto s = g_SessionManager.GetCurrentSession();
        if (!s) return;
        int64_t endTicks = 0;
        { lock_guard<mutex> guard(g_MediaState.lock); endTicks = g_MediaState.endTimeTicks; }
        if (endTicks <= 0) return;
        s.TryChangePlaybackPositionAsync((int64_t)(fraction * (double)endTicks));
    } catch (...) {}
}
D2D1_COLOR_F GetDynamicTextColor(D2D1_COLOR_F bg) {
    float luminance = 0.2126f * bg.r + 0.7152f * bg.g + 0.0722f * bg.b;
    // Increased threshold so text stays white unless the background is very bright (e.g. nearly white)
    if (luminance > 0.75f) { 
        return D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f); 
    } else { 
        return D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f); 
    }
}

struct ButtonLayout { float pX, plX, nX, cY, cR; };
ButtonLayout CalcButtonLayout(int height) {
    float scale = (float)g_Settings.buttonScale;
    float gap = 28.0f * scale;
    float pX = 6.0f + (height - 12.0f) + gap;
    return { pX, pX + gap, pX + gap * 2.0f, (float)height * 0.5f, 12.0f * scale };
}

// --- DIRECT2D DRAWING FUNCTION ---
void DrawMediaPanelD2D(HWND hwnd) {
    if (!g_pD2DFactory || !g_pDWriteFactory) return;
    
    RECT rc; GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width == 0 || height == 0) return;

    if (!g_pRT) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT, 
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f 
        );
        g_pD2DFactory->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(width, height)), &g_pRT);
        if (!g_pRT) return;
    } else {
        D2D1_SIZE_U size = g_pRT->GetPixelSize();
        if (size.width != width || size.height != height) g_pRT->Resize(D2D1::SizeU(width, height));
    }

    wstring title, artist;
    bool isPlaying = false, isShuffle = false;
    int repeatMode = 0;
    int64_t posTicks = 0, endTicks = 0, lastUpdate = 0;
    D2D1_COLOR_F chamColor;
    D2D1_COLOR_F chamColor2;
    
    static ID2D1Bitmap* cachedAlbumArtD2D = nullptr;
    static ID2D1Bitmap* oldCachedAlbumArtD2D = nullptr;

    {
        lock_guard<mutex> guard(g_MediaState.lock);
        title = g_MediaState.title; artist = g_MediaState.artist;
        isPlaying = g_MediaState.isPlaying; isShuffle = g_MediaState.isShuffle; repeatMode = g_MediaState.repeatMode;
        posTicks = g_MediaState.positionTicks; endTicks = g_MediaState.endTimeTicks; lastUpdate = g_MediaState.lastUpdatedTicks;
        chamColor = g_MediaState.chameleonColor;
        chamColor2 = g_MediaState.chameleonColor2;

        if (g_MediaState.artChanged) {
            if (g_SkipNextArtTransition) {
                // Art was pre-loaded while hidden — show instantly, no crossfade.
                if (oldCachedAlbumArtD2D) { oldCachedAlbumArtD2D->Release(); oldCachedAlbumArtD2D = nullptr; }
                if (cachedAlbumArtD2D)    { cachedAlbumArtD2D->Release();    cachedAlbumArtD2D = nullptr;    }
                g_ArtTransitionT = 1.0f;
                g_SkipNextArtTransition = false;
            } else {
                // Normal track change — crossfade from old to new.
                if (oldCachedAlbumArtD2D) { oldCachedAlbumArtD2D->Release(); oldCachedAlbumArtD2D = nullptr; }
                oldCachedAlbumArtD2D = cachedAlbumArtD2D;
                cachedAlbumArtD2D = nullptr;
                g_ArtTransitionT = 0.0f;
            }

            if (g_MediaState.albumArtWic) g_pRT->CreateBitmapFromWicBitmap(g_MediaState.albumArtWic, &cachedAlbumArtD2D);
            g_MediaState.artChanged = false;
        }
    }

    g_pRT->BeginDraw();
    g_pRT->Clear(D2D1::ColorF(0, 0, 0, 0));

    // Background
    // Feature: Draw Chameleon Gradient Background
    if (g_Settings.glassType >= 1 && g_Settings.chameleonBg && chamColor.a != 0) {
        
        // 1. Calculate the base alpha (opacity) from user settings
        float startAlpha = (float)g_Settings.chameleonIntensity / 255.f;
        float shiftOffset = 0.0f;
        
        // 2. Apply panning animation if enabled and music is actively playing
        if (g_Settings.animateChameleon && isPlaying) {
            
            // Adjust animation speed so it stays consistent regardless of the target FPS
            float speedAdjust = 30.0f / (float)g_Settings.targetFPS;
            
            // Pulse slider acts as distance multiplier: 0 to 100% of widget width
            float pulseScale = ((float)g_Settings.chameleonPulse / 100.f) * width;
            
            // Apply a sine wave to create a smooth back-and-forth panning effect
            shiftOffset = sin(g_AnimFrameCounter * 0.015f * speedAdjust) * pulseScale;
        }

        // 3. Handle smooth color transition when the track changes
        D2D1_COLOR_F drawColor = AdjustColorVibrancy(chamColor);
        D2D1_COLOR_F drawColor2 = AdjustColorVibrancy(chamColor2);
        if (g_Settings.chameleonTransition && g_ChamTransitionT < 1.0f) {
            // Linearly interpolate (Lerp) between the old color and the new color
            float t = g_ChamTransitionT;
            D2D1_COLOR_F from1 = AdjustColorVibrancy(g_ChamFromColor);
            D2D1_COLOR_F from2 = AdjustColorVibrancy(g_ChamFromColor2);
            
            drawColor.r = from1.r + (drawColor.r - from1.r) * t;
            drawColor.g = from1.g + (drawColor.g - from1.g) * t;
            drawColor.b = from1.b + (drawColor.b - from1.b) * t;
            
            drawColor2.r = from2.r + (drawColor2.r - from2.r) * t;
            drawColor2.g = from2.g + (drawColor2.g - from2.g) * t;
            drawColor2.b = from2.b + (drawColor2.b - from2.b) * t;
        }

        // 4. Create and apply the Linear Gradient Brush
        ID2D1GradientStopCollection* pStops = nullptr;
        D2D1_GRADIENT_STOP stops[2];
        stops[0].position = 0.0f; stops[0].color = D2D1::ColorF(drawColor.r, drawColor.g, drawColor.b, startAlpha);
        
        if (g_Settings.dualChameleonBg) {
            stops[1].position = 1.0f; stops[1].color = D2D1::ColorF(drawColor2.r, drawColor2.g, drawColor2.b, startAlpha);
        } else {
            stops[1].position = 1.0f; stops[1].color = D2D1::ColorF(drawColor.r, drawColor.g, drawColor.b, 0.0f);
        }
        g_pRT->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops);
        
        if (pStops) {
            ID2D1LinearGradientBrush* pGradBrush = nullptr;
            D2D1_POINT_2F p1 = D2D1::Point2F(shiftOffset, 0);
            D2D1_POINT_2F p2 = D2D1::Point2F((float)width * 1.5f + shiftOffset, 0);
            
            // Flip the gradient direction if the user selected 'Right side'
            if (g_Settings.chameleonRightSide) { 
                p1.x = width - shiftOffset; 
                p2.x = -(float)width * 0.5f - shiftOffset; 
            }
            
            g_pRT->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(p1, p2), pStops, &pGradBrush);
            if (pGradBrush) {
                g_pRT->FillRectangle(D2D1::RectF(0, 0, width, height), pGradBrush);
                pGradBrush->Release();
            }
            pStops->Release();
        }
    } else if (g_Settings.glassType == 0 || !g_Settings.chameleonBg) {
        DWORD tintHex = g_Settings.autoTheme ? (IsSystemLightMode() ? 0xEEEEEE : 0x1a1a1a) : g_Settings.glassTintColor;
        D2D1_COLOR_F baseBg = D2D1::ColorF(((tintHex >> 16) & 0xFF) / 255.f, ((tintHex >> 8) & 0xFF) / 255.f, (tintHex & 0xFF) / 255.f, (float)g_Settings.glassOpacity / 255.f);
        if (g_Settings.glassType == 0) baseBg.a = max(0.4f, baseBg.a); // Ensure Solid tint is visible if users set opacity too low
        ID2D1SolidColorBrush* pBgBrush = nullptr;
        g_pRT->CreateSolidColorBrush(baseBg, &pBgBrush);
        if (pBgBrush) {
            g_pRT->FillRectangle(D2D1::RectF(0, 0, width, height), pBgBrush);
            pBgBrush->Release();
        }
    }

    // Album Cover
    // The bitmap is pre-scaled by WIC Fant to exactly physArtSize (physical pixels, DPI-aware).
    // artRect uses logical coords — at 1:1 DPI they match. At HiDPI the GPU maps them correctly.
    float artSize = (float)height - 12.0f;
    D2D1_RECT_F artRect = D2D1::RectF(6, 6, 6 + artSize, 6 + artSize);
    D2D1_ROUNDED_RECT roundedArt = D2D1::RoundedRect(artRect, 6.f, 6.f);

    if (oldCachedAlbumArtD2D && g_ArtTransitionT < 1.0f) {
        ID2D1RoundedRectangleGeometry* pRoundGeom = nullptr;
        g_pD2DFactory->CreateRoundedRectangleGeometry(roundedArt, &pRoundGeom);
        if (pRoundGeom) {
            g_pRT->PushLayer(D2D1::LayerParameters(artRect, pRoundGeom), nullptr);
            // Bitmap is already Fant-pre-scaled; LINEAR here is lossless at 1:1.
            g_pRT->DrawBitmap(oldCachedAlbumArtD2D, artRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            g_pRT->PopLayer();
            pRoundGeom->Release();
        }
    }

    if (cachedAlbumArtD2D) {
        ID2D1RoundedRectangleGeometry* pRoundGeom = nullptr;
        g_pD2DFactory->CreateRoundedRectangleGeometry(roundedArt, &pRoundGeom);
        if (pRoundGeom) {
            g_pRT->PushLayer(D2D1::LayerParameters(artRect, pRoundGeom), nullptr);
            // Bitmap is Fant pre-scaled to exact physical display size. LINEAR here is lossless.
            g_pRT->DrawBitmap(cachedAlbumArtD2D, artRect, g_ArtTransitionT, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            g_pRT->PopLayer();
            pRoundGeom->Release();
        }
    } else {
        ID2D1SolidColorBrush* pGray = nullptr;
        g_pRT->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.5f * g_ArtTransitionT), &pGray);
        if (pGray) { g_pRT->FillRoundedRectangle(roundedArt, pGray); pGray->Release(); }
    }

    // Text Rendering & Buttons
    D2D1_COLOR_F mainColor = GetTextColorD2D(); 

    if (g_Settings.glassType >= 1 && g_Settings.chameleonBg && chamColor.a != 0) {
        DWORD tintHex = g_Settings.autoTheme ? (IsSystemLightMode() ? 0xEEEEEE : 0x1a1a1a) : g_Settings.glassTintColor;
        D2D1_COLOR_F baseBg = D2D1::ColorF(((tintHex >> 16) & 0xFF) / 255.f, ((tintHex >> 8) & 0xFF) / 255.f, (tintHex & 0xFF) / 255.f, 1.0f);
        float chamAlpha = (float)g_Settings.chameleonIntensity / 255.f;

        auto blend = [](D2D1_COLOR_F c, D2D1_COLOR_F b, float a) {
            return D2D1::ColorF(c.r * a + b.r * (1.f - a), c.g * a + b.g * (1.f - a), c.b * a + b.b * (1.f - a), 1.f);
        };
        D2D1_COLOR_F effectiveFromBg = blend(g_ChamFromColor, baseBg, chamAlpha);
        D2D1_COLOR_F effectiveToBg   = blend(chamColor, baseBg, chamAlpha);

        D2D1_COLOR_F textFrom = GetDynamicTextColor(effectiveFromBg);
        D2D1_COLOR_F textTo   = GetDynamicTextColor(effectiveToBg);
        
        float t = g_ChamTransitionT;
        mainColor.r = textFrom.r + (textTo.r - textFrom.r) * t;
        mainColor.g = textFrom.g + (textTo.g - textFrom.g) * t;
        mainColor.b = textFrom.b + (textTo.b - textFrom.b) * t;
        mainColor.a = 1.0f;
    }

    ID2D1SolidColorBrush* pTextBrush = nullptr;
    g_pRT->CreateSolidColorBrush(mainColor, &pTextBrush);
    
    if (pTextBrush && g_pIconFormat) {
        auto bl = CalcButtonLayout(height);
        auto drawBtn = [&](float cx, int hoverIdx, const WCHAR* glyph) {
            float hoverAlpha = g_HoverAlphas[hoverIdx]; // Lerp hover value
            if (hoverAlpha > 0.0f) {
                ID2D1SolidColorBrush* pHoverBg = nullptr;
                g_pRT->CreateSolidColorBrush(D2D1::ColorF(mainColor.r, mainColor.g, mainColor.b, 0.2f * hoverAlpha), &pHoverBg);
                if (pHoverBg) {
                    g_pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, bl.cY), bl.cR, bl.cR), pHoverBg);
                    pHoverBg->Release();
                }
            }
            D2D1_RECT_F r = D2D1::RectF(cx - bl.cR, bl.cY - bl.cR, cx + bl.cR, bl.cY + bl.cR);
            g_pRT->DrawText(glyph, 1, g_pIconFormat, r, pTextBrush);
        };
        drawBtn(bl.pX, 1, L"\xE892");
        drawBtn(bl.plX, 2, isPlaying ? L"\xE769" : L"\xE768");
        drawBtn(bl.nX, 3, L"\xE893");

        // Shuffle / Repeat
        if (g_Settings.showShuffleRepeat && g_pSmallIconFormat) {
            float iconX = (float)width - 10.0f;
            ID2D1SolidColorBrush* pDimBrush = nullptr;
            g_pRT->CreateSolidColorBrush(D2D1::ColorF(mainColor.r, mainColor.g, mainColor.b, 0.85f), &pDimBrush);
            if (pDimBrush) {
                if (repeatMode > 0) {
                    g_pRT->DrawText(repeatMode == 2 ? L"\xE8ED" : L"\xE8EE", 1, g_pSmallIconFormat, D2D1::RectF(iconX-10, 5, iconX+10, 20), pDimBrush);
                    iconX -= 14.0f;
                }
                if (isShuffle) g_pRT->DrawText(L"\xE8B1", 1, g_pSmallIconFormat, D2D1::RectF(iconX-10, 5, iconX+10, 20), pDimBrush);
                pDimBrush->Release();
            }
        }
    }

    // Text (Title / Artist)
    if (pTextBrush && g_pTextFormat) {
        wstring fullText = title;
        if (!artist.empty()) fullText += L" \u2022 " + artist;

        static wstring lastText = L"";
        static float cachedW = 0;
        if (fullText != lastText) {
            IDWriteTextLayout* pLayout = nullptr;
            g_pDWriteFactory->CreateTextLayout(fullText.c_str(), fullText.length(), g_pTextFormat, 4000.f, 100.f, &pLayout);
            if (pLayout) {
                DWRITE_TEXT_METRICS tm; pLayout->GetMetrics(&tm);
                cachedW = tm.width;
                pLayout->Release();
            }
            lastText = fullText;
            g_ScrollOffset = 0.0f;
            g_ScrollWait = g_Settings.scrollPause;
        }
        g_TextWidth = cachedW;

        auto bl = CalcButtonLayout(height);
        float textX = bl.nX + 20.0f * (float)g_Settings.buttonScale;
        float textMaxW = width - textX - 12.0f;
        
        g_pRT->PushAxisAlignedClip(D2D1::RectF(textX, 0, width, height), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        
        if (g_TextWidth > textMaxW && g_Settings.scrollSpeed > 0) {
            g_IsScrolling = true;
            float drawX = textX - g_ScrollOffset;
            g_pRT->DrawText(fullText.c_str(), fullText.length(), g_pTextFormat, D2D1::RectF(drawX, 0, drawX + g_TextWidth, height), pTextBrush);
            g_pRT->DrawText(fullText.c_str(), fullText.length(), g_pTextFormat, D2D1::RectF(drawX + g_TextWidth + 60.0f, 0, drawX + g_TextWidth*2 + 60.0f, height), pTextBrush);
        } else {
            g_IsScrolling = false;
            g_ScrollOffset = 0;
            g_pRT->DrawText(fullText.c_str(), fullText.length(), g_pTextFormat, D2D1::RectF(textX, 0, textX + g_TextWidth, height), pTextBrush);
        }
        g_pRT->PopAxisAlignedClip();
    }

    // Progress Bar
    if (g_Settings.showProgressBar && endTicks > 0) {
        FILETIME ft; GetSystemTimePreciseAsFileTime(&ft);
        LARGE_INTEGER li; li.LowPart = ft.dwLowDateTime; li.HighPart = ft.dwHighDateTime;
        int64_t currentPos = posTicks + (isPlaying ? (li.QuadPart - lastUpdate) : 0);
        currentPos = max(0LL, min(currentPos, endTicks));
        float progress = (float)((double)currentPos / (double)endTicks);

        ID2D1SolidColorBrush* pBgBar = nullptr;
        // Een lichtere donkere achtergrondbalk (0.20 ipv 0.35)
        g_pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.20f), &pBgBar);

        // Boost chameleon colors for the progress bar:
        // Keep the hue from the album art, but force a high lightness (0.72) and
        // minimum saturation (0.65) so the bar is always vivid and readable
        // regardless of how dark or muted the extracted album colors are.
        auto BoostForBar = [](D2D1_COLOR_F c) -> D2D1_COLOR_F {
            float h, s, l;
            RGBtoHSL(c.r, c.g, c.b, h, s, l);
            s = max(s, 0.65f);  // ensure minimum vibrancy
            l = 0.72f;           // fixed bright lightness — always visible on dark backgrounds
            float r, g, b;
            HSLtoRGB(h, s, l, r, g, b);
            return D2D1::ColorF(r, g, b, 1.0f);
        };
        D2D1_COLOR_F pc1 = BoostForBar(chamColor);
        D2D1_COLOR_F pc2 = BoostForBar(chamColor2);

        // Fallback solid color (user-defined hex)
        DWORD pc = g_Settings.progressBarColor;
        D2D1_COLOR_F solidColor = D2D1::ColorF(((pc>>16)&0xFF)/255.f, ((pc>>8)&0xFF)/255.f, (pc&0xFF)/255.f, 1.0f);

        if (pBgBar) {
            // Balk weer helemaal vanonder.
            float trackBottom = (float)height;
            float trackTop = trackBottom - 3.0f;

            // Achtergrondbalk 4px hoog (1px donkere lijn bovenaan voor contrast)
            g_pRT->FillRectangle(D2D1::RectF(0, trackTop - 1.0f, width, trackBottom), pBgBar);

            bool useChameleon = g_Settings.chameleonProgressBar && g_Settings.chameleonBg && chamColor.a != 0;

            if (useChameleon) {
                D2D1_GRADIENT_STOP stops[2];
                stops[0] = { 0.0f, D2D1::ColorF(pc1.r, pc1.g, pc1.b, 1.0f) };
                stops[1] = { 1.0f, D2D1::ColorF(pc2.r, pc2.g, pc2.b, 1.0f) };
                ID2D1GradientStopCollection* pStops = nullptr;
                g_pRT->CreateGradientStopCollection(stops, 2, &pStops);
                if (pStops) {
                    ID2D1LinearGradientBrush* pGrad = nullptr;
                    g_pRT->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(
                            D2D1::Point2F(0, trackTop + 1.5f),
                            D2D1::Point2F((float)width, trackTop + 1.5f)),
                        pStops, &pGrad);
                    if (pGrad) {
                        g_pRT->FillRectangle(D2D1::RectF(0, trackTop, width * progress, trackBottom), pGrad);
                        // Scrub dot: geplaatst op de bovenste rand van de balk zodat hij niet afgekapt wordt
                        float progHoverAlpha = g_HoverAlphas[4];
                        if (g_Settings.enableScrubbing && progHoverAlpha > 0.0f) {
                            ID2D1SolidColorBrush* pDot = nullptr;
                            g_pRT->CreateSolidColorBrush(D2D1::ColorF(pc1.r, pc1.g, pc1.b, progHoverAlpha), &pDot);
                            if (pDot) {
                                g_pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(width * progress, trackTop), 3.0f, 3.0f), pDot);
                                pDot->Release();
                            }
                        }
                        pGrad->Release();
                    }
                    pStops->Release();
                }
            } else {
                ID2D1SolidColorBrush* pProgBar = nullptr;
                g_pRT->CreateSolidColorBrush(solidColor, &pProgBar);
                if (pProgBar) {
                    g_pRT->FillRectangle(D2D1::RectF(0, trackTop, width * progress, trackBottom), pProgBar);
                    float progHoverAlpha = g_HoverAlphas[4];
                    if (g_Settings.enableScrubbing && progHoverAlpha > 0.0f) {
                        ID2D1SolidColorBrush* pDot = nullptr;
                        g_pRT->CreateSolidColorBrush(D2D1::ColorF(solidColor.r, solidColor.g, solidColor.b, progHoverAlpha), &pDot);
                        if (pDot) {
                            g_pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(width * progress, trackTop), 3.0f, 3.0f), pDot);
                            pDot->Release();
                        }
                    }
                    pProgBar->Release();
                }
            }
            pBgBar->Release();
        }
    }

    if (pTextBrush) pTextBrush->Release();

    HRESULT hr = g_pRT->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) { g_pRT->Release(); g_pRT = nullptr; }
}


// --- Hooks & Callbacks ---
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL) {
        if (g_Settings.enableVolumeScroll && g_hMediaWindow && IsWindowVisible(g_hMediaWindow)) {
            MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;
            RECT rc; GetWindowRect(g_hMediaWindow, &rc);
            if (PtInRect(&rc, ms->pt)) {
                short z = (short)HIWORD(ms->mouseData);
                BYTE vk = z > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN;
                keybd_event(vk, 0, 0, 0); keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
                return 1; 
            }
        }
    }
    return CallNextHookEx(g_MouseHook, nCode, wParam, lParam);
}

// --- Mouse Hook Thread ---
// Runs on a dedicated thread so the hook is always responsive,
// even when the rendering thread is busy drawing frames.
void MouseHookThread() {
    g_MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);
    g_MouseHookThreadId = GetCurrentThreadId();
    // This message loop ONLY services the mouse hook.
    // It does nothing else, ensuring the hook never times out.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    if (g_MouseHook) { UnhookWindowsHookEx(g_MouseHook); g_MouseHook = NULL; }
}

void StartMouseHookThread();
std::thread* g_pMouseHookThread = nullptr;
void StartMouseHookThread() {
    g_pMouseHookThread = new std::thread(MouseHookThread);
}
void StopMouseHookThread() {
    if (g_MouseHookThreadId) PostThreadMessage(g_MouseHookThreadId, WM_QUIT, 0, 0);
    if (g_pMouseHookThread) {
        if (g_pMouseHookThread->joinable()) g_pMouseHookThread->join();
        delete g_pMouseHookThread; g_pMouseHookThread = nullptr;
    }
    g_MouseHookThreadId = 0;
}

void CALLBACK TaskbarEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (g_hMediaWindow) PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
}

void RegisterTaskbarHook(HWND hwnd) {
    if (g_TaskbarHook) { UnhookWinEvent(g_TaskbarHook); g_TaskbarHook = nullptr; }
    HWND hTb = GetTargetTaskbar();
    if (hTb) {
        DWORD pid = 0, tid = GetWindowThreadProcessId(hTb, &pid);
        if (tid) g_TaskbarHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, TaskbarEventProc, pid, tid, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    }
    PostMessage(hwnd, WM_APP + 10, 0, 0);
}

// --- Widget Positioning Helpers ---
// Calculates the X position of the widget on the taskbar based on the PositionAnchor setting.
int CalcWidgetX(const RECT& rc) {
    int taskbarW = rc.right - rc.left;
    if (g_Settings.positionAnchor == 1) return rc.left + taskbarW / 2 - g_Settings.width / 2 + g_Settings.offsetX; // Center
    if (g_Settings.positionAnchor == 2) return rc.right - g_Settings.width - g_Settings.offsetX;                    // Right
    return rc.left + g_Settings.offsetX;                                                                              // Left (default)
}
int CalcWidgetY(const RECT& rc) {
    return rc.top + (rc.bottom - rc.top) / 2 - g_Settings.height / 2 + g_Settings.offsetY;
}

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        InitD2D();
        UpdateAppearance(hwnd);
        // Fix: defer the first media poll via PostMessage instead of calling UpdateMediaInfo()
        // synchronously here. WM_CREATE runs before the message loop starts, which makes
        // blocking WinRT async calls (RequestAsync().get()) unreliable and causes the
        // session manager to return an incomplete/empty state on first load.
        PostMessage(hwnd, APP_WM_MEDIA_UPDATE, 0, 0);
        SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL);
        RegisterTaskbarHook(hwnd);
        return 0;
    case WM_ERASEBKGND: return 1; 
    case WM_CLOSE: return 0; 
    case APP_WM_CLOSE: DestroyWindow(hwnd); return 0;
    
    // SMART SLEEP
    case WM_POWERBROADCAST:
        if (wParam == PBT_APMSUSPEND) {
            g_IsSuspended = true;
        } else if (wParam == PBT_APMRESUMEAUTOMATIC || wParam == PBT_APMRESUMESUSPEND) {
            g_IsSuspended = false;
            
            // Force recreation of the WinRT session manager after sleep, 
            // because the old COM object often silently drops events or breaks.
            if (g_SessionManager) {
                if (g_SessionChangedToken.value != 0) {
                    try { g_SessionManager.CurrentSessionChanged(g_SessionChangedToken); } catch (...) {}
                    g_SessionChangedToken = {};
                }
                g_SessionManager = nullptr;
            }
            SubscribeToSession(nullptr);

            PostMessage(hwnd, WM_TIMER, IDT_POLL_MEDIA, 0);
        }
        return 0;

    case APP_WM_MEDIA_UPDATE:
        UpdateMediaInfo();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    case APP_WM_FALLBACK_ART_READY: {
        FallbackArtResult* res = (FallbackArtResult*)lParam;
        if (res) {
            lock_guard<mutex> guard(g_MediaState.lock);
            // Only apply if the song hasn't changed since the download started, 
            // AND we haven't already received the art from Spotify in the meantime.
            if (res->title == g_MediaState.title && res->artist == g_MediaState.artist && !g_MediaState.albumArtWic) {
                UINT artSize = max(24u, (UINT)g_Settings.height - 12);
                UINT artLoadSize = min(512u, artSize * 4);
                IWICBitmapSource* newArt = LoadWICBitmapFromFile(res->imagePath.c_str(), artLoadSize);
                if (newArt) {
                    g_MediaState.albumArtWic = newArt;
                    g_MediaState.artChanged = true;
                    if (g_Settings.chameleonBg) {
                        auto colors = ExtractChameleonColorWIC(newArt);
                        D2D1_COLOR_F newChamColor = colors.first;
                        D2D1_COLOR_F newChamColor2 = colors.second;
                        if (g_Settings.chameleonTransition) {
                            g_ChamFromColor = g_MediaState.chameleonColor;
                            g_ChamToColor = newChamColor;
                            g_ChamFromColor2 = g_MediaState.chameleonColor2;
                            g_ChamToColor2 = newChamColor2;
                            g_ChamTransitionT = 0.0f;
                        } else { g_ChamTransitionT = 1.0f; }
                        g_MediaState.chameleonColor = newChamColor;
                        g_MediaState.chameleonColor2 = newChamColor2;
                    }
                    g_MediaState.artSource = 3; // 3 = iTunes
                    g_PendingArtRefresh = true;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            DeleteFileW(res->imagePath.c_str());
            delete res;
        }
        return 0;
    }
    case WM_DESTROY:
        if (g_TaskbarHook) { UnhookWinEvent(g_TaskbarHook); g_TaskbarHook = nullptr; }
        // Fix: use .value != 0 — event_token has no operator bool, same as the RegisterMediaEvents fix.
        if (g_SessionManager && g_SessionChangedToken.value != 0) { g_SessionManager.CurrentSessionChanged(g_SessionChangedToken); g_SessionChangedToken = {}; }
        if (g_CurrentSession) {
            if (g_PlaybackInfoToken.value != 0) { g_CurrentSession.PlaybackInfoChanged(g_PlaybackInfoToken); g_PlaybackInfoToken = {}; }
            if (g_MediaPropsToken.value != 0) { g_CurrentSession.MediaPropertiesChanged(g_MediaPropsToken); g_MediaPropsToken = {}; }
        }
        g_SessionManager = nullptr;
        CleanupD2D();
        PostQuitMessage(0);
        return 0;
    case WM_SETTINGCHANGE:
        g_DpiScale = (float)GetDpiForWindow(hwnd) / 96.0f;  // DPI may have changed
        RecreateTextFormats(); // Rebuild fonts to apply scale/size and DPI changes
        UpdateAppearance(hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    case WM_TIMER: {
        if (g_IsSuspended) return 0; // No renders or polls during sleep mode

        if (wParam == IDT_FADE) {
            float step = 255.0f / (float)g_Settings.targetFPS * 2.0f;
            if (g_FadingIn) {
                g_FadeAlpha = min(255.0f, g_FadeAlpha + step);
                SetLayeredWindowAttributes(hwnd, 0, (BYTE)(UINT)g_FadeAlpha, LWA_ALPHA);
                if (g_FadeAlpha >= 255.0f) { g_FadingIn = false; KillTimer(hwnd, IDT_FADE); }
            } else if (g_FadingOut) {
                g_FadeAlpha = max(0.0f, g_FadeAlpha - step);
                SetLayeredWindowAttributes(hwnd, 0, (BYTE)(UINT)g_FadeAlpha, LWA_ALPHA);
                if (g_FadeAlpha <= 0.0f) { g_FadingOut = false; KillTimer(hwnd, IDT_FADE); ShowWindow(hwnd, SW_HIDE); g_FadeAlpha = 0.0f; }
            }
            return 0;
        }

        if (wParam == IDT_POLL_MEDIA) {
            UpdateMediaInfo();
            bool shouldHide = false, isPlaying = false, hasMedia = false;
            { lock_guard<mutex> guard(g_MediaState.lock); isPlaying = g_MediaState.isPlaying; hasMedia = g_MediaState.hasMedia; }
            if (g_Settings.hideWhenNoMedia && !hasMedia) shouldHide = true;
            if (g_Settings.hideFullscreen && !shouldHide) {
                QUERY_USER_NOTIFICATION_STATE ns;
                if (SUCCEEDED(SHQueryUserNotificationState(&ns)))
                    if (ns == QUNS_BUSY || ns == QUNS_RUNNING_D3D_FULL_SCREEN || ns == QUNS_PRESENTATION_MODE) shouldHide = true;
            }
            if (g_Settings.idleTimeout > 0 && !shouldHide) {
                if (isPlaying) { g_IdleSecondsCounter = 0; g_IsHiddenByIdle = false; }
                else if (++g_IdleSecondsCounter >= g_Settings.idleTimeout) g_IsHiddenByIdle = true;
            } else g_IsHiddenByIdle = false;
            if (g_IsHiddenByIdle) shouldHide = true;

            if (shouldHide && IsWindowVisible(hwnd) && !g_FadingOut) {
                if (g_Settings.fadeAnimation) { g_FadingOut = true; g_FadingIn = false; SetTimer(hwnd, IDT_FADE, 1000 / g_Settings.targetFPS, NULL); }
                else ShowWindow(hwnd, SW_HIDE);
            } else if (!shouldHide && !IsWindowVisible(hwnd) && !g_FadingIn) {
                HWND hTb = GetTargetTaskbar();
                if (hTb && IsWindowVisible(hTb)) {
                    PostMessage(hwnd, WM_APP + 10, 0, 0);
                    if (g_Settings.fadeAnimation) {
                        g_FadeAlpha = 0.0f; SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                        g_FadingIn = true; g_FadingOut = false; SetTimer(hwnd, IDT_FADE, 1000 / g_Settings.targetFPS, NULL);
                    } else ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                }
            }

            bool isActVis = IsWindowVisible(hwnd);
            // Auto-hide optimization: check if the widget is actually on a monitor.
            // When the taskbar auto-hides, it drags the widget off-screen. We detect this
            // and skip animation/render work to save CPU and GPU cycles.
            bool onScreen = (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != NULL);
            bool needsFastAnim = g_IsScrolling || (g_Settings.chameleonBg && g_Settings.animateChameleon && isPlaying) || (g_Settings.chameleonBg && g_Settings.chameleonTransition && g_ChamTransitionT < 1.0f) || g_ArtTransitionT < 1.0f;
            
            // Check if we need to trigger hover animations
            for (int i = 1; i <= 4; i++) {
                if (abs(g_HoverAlphas[i] - ((g_HoverState == i) ? 1.0f : 0.0f)) > 0.01f) needsFastAnim = true;
            }

            bool needsSlowAnim = (g_Settings.showProgressBar && isPlaying);

            int currentFPS = 0;
            if (needsFastAnim) currentFPS = g_Settings.targetFPS;
            else if (needsSlowAnim) currentFPS = g_Settings.progressBarFPS;

            if (isActVis && onScreen && currentFPS > 0 && !shouldHide) {
                SetTimer(hwnd, IDT_ANIMATION, 1000 / currentFPS, NULL);
            } else if (!isActVis || !onScreen || shouldHide) {
                KillTimer(hwnd, IDT_ANIMATION);
            }

            if (isActVis && onScreen) InvalidateRect(hwnd, NULL, FALSE);
        }

        else if (wParam == IDT_ANIMATION) {
            // Stop animating entirely when the widget is off-screen (e.g. taskbar auto-hidden).
            // This is the main energy saving for auto-hide: no CPU/GPU work at ~60Hz when invisible.
            if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) == NULL) {
                KillTimer(hwnd, IDT_ANIMATION);
                return 0;
            }
            bool needsRedraw = false;
            bool isPlaying = false; { lock_guard<mutex> guard(g_MediaState.lock); isPlaying = g_MediaState.isPlaying; }

            // LERP Hover Animations
            float fadeStep = (float)g_Settings.hoverSpeed / (float)g_Settings.targetFPS;
            for (int i = 1; i <= 4; i++) {
                float target = (g_HoverState == i) ? 1.0f : 0.0f;
                if (abs(g_HoverAlphas[i] - target) > 0.01f) {
                    if (g_HoverAlphas[i] < target) g_HoverAlphas[i] = min(target, g_HoverAlphas[i] + fadeStep);
                    else g_HoverAlphas[i] = max(target, g_HoverAlphas[i] - fadeStep);
                    needsRedraw = true;
                } else {
                    g_HoverAlphas[i] = target; 
                }
            }

            // Chameleon Breathing
            if (g_Settings.chameleonBg && g_Settings.animateChameleon && isPlaying) { 
                g_AnimFrameCounter += 1.0f; 
                needsRedraw = true; 
            }
            if (g_Settings.chameleonBg && g_Settings.chameleonTransition && g_ChamTransitionT < 1.0f) {
                g_ChamTransitionT = min(1.0f, g_ChamTransitionT + (1.0f / ((float)g_Settings.targetFPS * 1.5f)));
                needsRedraw = true;
            }
            if (g_ArtTransitionT < 1.0f) {
                g_ArtTransitionT = min(1.0f, g_ArtTransitionT + (1.0f / ((float)g_Settings.targetFPS * 0.35f))); // 350ms crossfade
                needsRedraw = true;
            }

            // Scrolling
            if (g_IsScrolling) {
                if (g_ScrollWait > 0) g_ScrollWait--;
                else {
                    g_ScrollOffset += ((float)g_Settings.scrollSpeed * (30.0f / (float)g_Settings.targetFPS));
                    if (g_ScrollOffset > g_TextWidth + 60.0f) { g_ScrollOffset = 0; g_ScrollWait = g_Settings.scrollPause; }
                    needsRedraw = true;
                }
            }
            
            if (g_Settings.showProgressBar && isPlaying) needsRedraw = true;
            
            if (needsRedraw && IsWindowVisible(hwnd)) InvalidateRect(hwnd, NULL, FALSE);
            else KillTimer(hwnd, IDT_ANIMATION);
        }
        return 0;
    }
    case WM_APP + 10: {
        HWND hTb = GetTargetTaskbar();
        if (hTb != g_CurrentTargetTaskbar) {
            g_CurrentTargetTaskbar = hTb;
            RegisterTaskbarHook(hwnd);
        }
        if (!hTb) break;

        bool currentVis = IsWindowVisible(hTb);
        if (currentVis != g_LastVisibleState) {
            g_LastVisibleState = currentVis;
            bool hasMedia = false; { lock_guard<mutex> guard(g_MediaState.lock); hasMedia = g_MediaState.hasMedia; }
            if ((g_Settings.hideWhenNoMedia && !hasMedia) || g_IsHiddenByIdle || !currentVis) {
                if (IsWindowVisible(hwnd)) {
                    if (g_Settings.fadeAnimation && !g_FadingOut) { g_FadingOut = true; g_FadingIn = false; SetTimer(hwnd, IDT_FADE, 1000 / g_Settings.targetFPS, NULL); }
                    else ShowWindow(hwnd, SW_HIDE);
                }
            } else {
                if (g_Settings.fadeAnimation && !g_FadingIn) {
                    g_FadeAlpha = 0.0f; SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    g_FadingIn = true; g_FadingOut = false; SetTimer(hwnd, IDT_FADE, 1000 / g_Settings.targetFPS, NULL);
                } else ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }
        }
        if (currentVis) {
            RECT rc; GetWindowRect(hTb, &rc);
            SetWindowPos(hwnd, HWND_TOPMOST, CalcWidgetX(rc), CalcWidgetY(rc), g_Settings.width, g_Settings.height, SWP_NOACTIVATE);

            if (g_PendingArtRefresh) {
                // Art was pre-loaded while the taskbar was hidden.
                // Skip both the color crossfade and the art crossfade so everything
                // is instantly correct when the taskbar slides up.
                g_PendingArtRefresh = false;
                g_ChamTransitionT = 1.0f;
                // Only skip the art transition if there's actually new art waiting in the state.
                // If art hasn't loaded yet (still retrying), let the normal crossfade play when it arrives.
                bool hasNewArt = false;
                { lock_guard<mutex> guard(g_MediaState.lock); hasNewArt = g_MediaState.artChanged; }
                if (hasNewArt) g_SkipNextArtTransition = true;
            }

            // Immediately re-evaluate animation state so scrolling and other animations
            // resume in the same message cycle, instead of waiting up to 1s for IDT_POLL_MEDIA.
            PostMessage(hwnd, WM_TIMER, IDT_POLL_MEDIA, 0);
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        int mx = LOWORD(lParam), my = HIWORD(lParam);
        auto bl = CalcButtonLayout(g_Settings.height);
        int newState = 0;
        if (pow(mx - bl.pX, 2) + pow(my - bl.cY, 2) <= pow(bl.cR, 2)) newState = 1;
        else if (pow(mx - bl.plX, 2) + pow(my - bl.cY, 2) <= pow(bl.cR, 2)) newState = 2;
        else if (pow(mx - bl.nX, 2) + pow(my - bl.cY, 2) <= pow(bl.cR, 2)) newState = 3;
        else if (g_Settings.enableScrubbing && g_Settings.showProgressBar && my >= g_Settings.height - 8) newState = 4;

        if (newState != g_HoverState) { g_HoverState = newState; InvalidateRect(hwnd, NULL, FALSE); }
        TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
        TrackMouseEvent(&tme);
        
        if (g_Settings.targetTaskbar == 1) PostMessage(hwnd, WM_APP + 10, 0, 0);
        return 0;
    }
    case WM_MOUSELEAVE:
        g_HoverState = 0; InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_LBUTTONUP: {
        int mx = LOWORD(lParam), my = HIWORD(lParam);
        if (g_Settings.enableScrubbing && g_Settings.showProgressBar && my >= g_Settings.height - 8) {
            SeekToPosition(max(0.0f, min(1.0f, (float)mx / (float)g_Settings.width)));
            return 0;
        }
        if (g_HoverState > 0 && g_HoverState < 4) SendMediaCommand(g_HoverState);
        else if (g_Settings.clickToPause) SendMediaCommand(2);
        else if (g_Settings.clickToOpen) {
            wstring aumid; { lock_guard<mutex> guard(g_MediaState.lock); aumid = g_MediaState.aumid; }
            if (!aumid.empty()) {
                AllowSetForegroundWindow(ASFW_ANY);
                const CLSID CLSID_AAM = { 0x45BA127D, 0x10A8, 0x46EA, { 0x8A, 0xB7, 0x56, 0xEA, 0x90, 0x78, 0x94, 0x3C } };
                const IID IID_AAM = { 0x2e941141, 0x7f97, 0x4756, { 0xba, 0x1d, 0x9d, 0xec, 0xde, 0x89, 0x4a, 0x3d } };
                IApplicationActivationManager* aam = nullptr;
                if (SUCCEEDED(CoCreateInstance(CLSID_AAM, NULL, CLSCTX_LOCAL_SERVER, IID_AAM, (void**)&aam))) {
                    DWORD pid = 0; aam->ActivateApplication(aumid.c_str(), nullptr, AO_NONE, &pid); aam->Release();
                } else {
                    wstring uri = L"shell:AppsFolder\\" + aumid; ShellExecute(NULL, L"open", uri.c_str(), NULL, NULL, SW_RESTORE);
                }
            }
        }
        return 0;
    }
    case WM_PAINT: {
        ValidateRect(hwnd, NULL);
        // Skip D2D rendering when the widget is off-screen (taskbar auto-hidden) to save GPU work.
        if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != NULL) {
            DrawMediaPanelD2D(hwnd);
        }
        return 0;
    }
    default:
        if (msg == g_TaskbarCreatedMsg) { RegisterTaskbarHook(hwnd); return 0; }
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void MediaThread() {
    winrt::init_apartment();
    WNDCLASS wc = {};
    wc.lpfnWndProc   = MediaWndProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("WindhawkMusicLounge_D2D");
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HMODULE hU32 = GetModuleHandle(L"user32.dll");
    auto CWiB = hU32 ? (pCreateWindowInBand)GetProcAddress(hU32, "CreateWindowInBand") : nullptr;
    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

    if (CWiB) g_hMediaWindow = CWiB(exStyle, wc.lpszClassName, TEXT("MusicLounge"), WS_POPUP | WS_VISIBLE, 0, 0, g_Settings.width, g_Settings.height, NULL, NULL, wc.hInstance, NULL, (DWORD)ZBID2_IMMERSIVE_NOTIFICATION);
    if (!g_hMediaWindow) g_hMediaWindow = CreateWindowEx(exStyle, wc.lpszClassName, TEXT("MusicLounge"), WS_POPUP | WS_VISIBLE, 0, 0, g_Settings.width, g_Settings.height, NULL, NULL, wc.hInstance, NULL);

    // InitD2D/RecreateTextFormats ran during WM_CREATE when g_DpiScale was still 1.0.
    // Now that the window exists, query the actual DPI and rebuild fonts at the correct scale.
    if (g_hMediaWindow) {
        g_DpiScale = (float)GetDpiForWindow(g_hMediaWindow) / 96.0f;
        RecreateTextFormats();
    }
    SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);
    // Mouse hook is now on its own dedicated thread (see StartMouseHookThread)

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }

    if (g_MouseHook) { UnhookWindowsHookEx(g_MouseHook); g_MouseHook = NULL; }
    // Mouse hook thread is stopped separately in WhTool_ModUninit
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    winrt::uninit_apartment();
}

std::thread* g_pMediaThread = nullptr;

BOOL WhTool_ModInit() {
    SetCurrentProcessExplicitAppUserModelID(L"taskbar-music-lounge");
    LoadSettings();
    StartMouseHookThread(); // Start hook on its own thread FIRST
    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hMediaWindow) SendMessage(g_hMediaWindow, APP_WM_CLOSE, 0, 0);
    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        delete g_pMediaThread; g_pMediaThread = nullptr;
    }
    StopMouseHookThread(); // Stop dedicated hook thread
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hMediaWindow) {
        // Fix: also resize the window when PanelWidth/PanelHeight settings change.
        // Previously the window kept its old size until the mod was restarted.
        HWND hTb = GetTargetTaskbar();
        if (hTb) {
            RECT rc; GetWindowRect(hTb, &rc);
            SetWindowPos(g_hMediaWindow, HWND_TOPMOST, rc.left + g_Settings.offsetX, rc.top + (rc.bottom - rc.top) / 2 - g_Settings.height / 2 + g_Settings.offsetY, g_Settings.width, g_Settings.height, SWP_NOACTIVATE);
        }
        RecreateTextFormats(); // Font size may have changed too
        UpdateAppearance(g_hMediaWindow);
        InvalidateRect(g_hMediaWindow, NULL, TRUE);
        UpdateWindow(g_hMediaWindow);
    }
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;
void WINAPI EntryPoint_Hook() { Wh_Log(L">"); ExitThread(0); }

BOOL Wh_ModInit() {
    bool isService = false, isToolMod = false, isCurrent = false;
    int argc; LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) return FALSE;
    for (int i = 1; i < argc; i++)
        if (wcscmp(argv[i], L"-service") == 0) { isService = true; break; }
    for (int i = 1; i < argc-1; i++)
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolMod = true;
            if (wcscmp(argv[i+1], WH_MOD_ID) == 0) isCurrent = true;
            break;
        }
    LocalFree(argv);
    if (isService) return FALSE;
    if (isCurrent) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) ExitProcess(1);
        if (!WhTool_ModInit()) ExitProcess(1);
        IMAGE_DOS_HEADER* dh = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* nh = (IMAGE_NT_HEADERS*)((BYTE*)dh + dh->e_lfanew);
        Wh_SetFunctionHook((BYTE*)dh + nh->OptionalHeader.AddressOfEntryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolMod) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;
    WCHAR path[MAX_PATH]; if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) return;
    WCHAR cmd[MAX_PATH + 64]; swprintf_s(cmd, L"\"%s\" -tool-mod \"%s\"", path, WH_MOD_ID);
    HMODULE km = GetModuleHandle(L"kernelbase.dll"); if (!km) km = GetModuleHandle(L"kernel32.dll"); if (!km) return;
    using CPIW = BOOL(WINAPI*)(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,WINBOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION,PHANDLE);
    auto fn = (CPIW)GetProcAddress(km, "CreateProcessInternalW"); if (!fn) return;
    STARTUPINFO si{}; si.cb = sizeof(si); si.dwFlags = STARTF_FORCEOFFFEEDBACK; PROCESS_INFORMATION pi;
    if (fn(nullptr, path, cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() { if (!g_isToolModProcessLauncher) WhTool_ModSettingsChanged(); }
void Wh_ModUninit()          { if (!g_isToolModProcessLauncher) { WhTool_ModUninit(); ExitProcess(0); } }

