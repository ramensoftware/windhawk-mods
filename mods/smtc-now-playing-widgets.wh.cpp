// ==WindhawkMod==
// @id              smtc-now-playing-widgets
// @name            SMTC Now Playing Desktop Widgets
// @description     Multi-instance, multi-monitor Windows 11 Acrylic Now Playing widgets powered natively by SMTC with Standard, Album, Cardstock, and Simplified themes.
// @version         1.0.10
// @author          minster586
// @github          https://github.com/minster586
// @homepage        https://links.smartcraft.me/@minster586
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lole32 -loleaut32 -ld2d1 -ldwrite -lwindowscodecs -ldwmapi -lshcore -lruntimeobject -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# SMTC Now Playing Desktop Widgets

Displays up to 5 customizable, multi-monitor Now Playing widgets on your Windows 11 desktop using the native Windows System Media Transport Controls (SMTC) API.

![SMTC Now Playing Desktop Widgets Preview](https://raw.githubusercontent.com/minster586/windhawk-assets/main/Screenshot%202026-11-16%20170854.png)

When album art is unavailable, the widgets display an embedded default image.

## Themes
- **Standard**: Wide horizontal Acrylic card with rounded album art, bold track/artist text, 5 outlined playback controls, and a progress bar with timestamps.
- **Album**: Full-bleed dimmed album art square with centered track/artist text, corner timestamps, and a bottom-edge progress bar.
- **Cardstock**: Vertical portrait Acrylic card with full-brightness album art, centered track/artist, and progress bar. Automatically shrinks uniformly when playback controls are disabled.
- **Simplified**: Sleek single-row horizontal Acrylic bar with compact art, track/artist text, 5 outlined playback controls, and a clean progress bar without timestamps.

## Usage
- **Move Widgets**: Left-click and drag any widget across any monitor (unless "Lock Position" is enabled for that instance). Coordinates save automatically.
- **Display-Only Mode**: Enable "Disable Playback Controls" on any instance to hide all transport buttons and disable progress bar seeking.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WidgetCount: 1
  $name: Active Widget Count (1-5)
  $description: Number of simultaneous desktop widgets to display.
- AlwaysOnTop: false
  $name: Float Always on Top
  $description: Keep widgets above normal application windows. When off, widgets behave like desktop widgets.
- AccentTintHex: "0B192C"
  $name: Acrylic Tint Color (Hex RGB)
  $description: Base dark tint color behind the Windows 11 Acrylic blur (default 0B192C).
- AcrylicOpacity: 215
  $name: Acrylic Tint Opacity (0-255)
  $description: Alpha intensity of the Acrylic tint overlay (0 = pure glass, 255 = solid).
- ProgressFillHex: "95D59B"
  $name: Progress Bar Fill Color (Hex RGB)
  $description: Active elapsed bar color (default 95D59B).

- Widget1:
  - Theme: standard
    $name: Theme
    $options:
      - standard: Standard
      - album: Album
      - cardstock: Cardstock
      - simplified: Simplified
  - ScalePercent: 100
    $name: Scale (%)
    $description: Widget scale percentage (50 to 250).
  - DisableControls: false
    $name: Disable Playback Controls (Display-Only)
  - LockPosition: false
    $name: Lock Widget Position (Disable Dragging)

- Widget2:
  - Theme: album
    $name: Theme
    $options:
      - standard: Standard
      - album: Album
      - cardstock: Cardstock
      - simplified: Simplified
  - ScalePercent: 100
    $name: Scale (%)
    $description: Widget scale percentage (50 to 250).
  - DisableControls: true
    $name: Disable Playback Controls (Display-Only)
  - LockPosition: false
    $name: Lock Widget Position (Disable Dragging)

- Widget3:
  - Theme: cardstock
    $name: Theme
    $options:
      - standard: Standard
      - album: Album
      - cardstock: Cardstock
      - simplified: Simplified
  - ScalePercent: 100
    $name: Scale (%)
    $description: Widget scale percentage (50 to 250).
  - DisableControls: true
    $name: Disable Playback Controls (Display-Only)
  - LockPosition: false
    $name: Lock Widget Position (Disable Dragging)

- Widget4:
  - Theme: simplified
    $name: Theme
    $options:
      - standard: Standard
      - album: Album
      - cardstock: Cardstock
      - simplified: Simplified
  - ScalePercent: 100
    $name: Scale (%)
    $description: Widget scale percentage (50 to 250).
  - DisableControls: false
    $name: Disable Playback Controls (Display-Only)
  - LockPosition: false
    $name: Lock Widget Position (Disable Dragging)

- Widget5:
  - Theme: standard
    $name: Theme
    $options:
      - standard: Standard
      - album: Album
      - cardstock: Cardstock
      - simplified: Simplified
  - ScalePercent: 100
    $name: Scale (%)
    $description: Widget scale percentage (50 to 250).
  - DisableControls: false
    $name: Disable Playback Controls (Display-Only)
  - LockPosition: false
    $name: Lock Widget Position (Disable Dragging)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

using winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession;
using winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
using winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;
using winrt::Windows::Storage::Streams::DataReader;

// ============================================================================
// Undocumented Win11 Acrylic Composition API
// ============================================================================
enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor; // AABBGGRR
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using pfnSetWindowCompositionAttribute = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
static pfnSetWindowCompositionAttribute g_SetWindowCompositionAttribute = nullptr;

// ============================================================================
// Data Structures & Settings
// ============================================================================
enum class WidgetTheme {
    Standard = 0,
    Album = 1,
    Cardstock = 2,
    Simplified = 3
};

struct WidgetConfig {
    WidgetTheme theme = WidgetTheme::Standard;
    int scalePercent = 100;
    bool disableControls = false;
    bool lockPosition = false;
};

struct GlobalSettings {
    int widgetCount = 1;
    bool alwaysOnTop = false;
    DWORD tintBGR = 0x2C190B;      // Default #0B192C in BGR
    BYTE acrylicAlpha = 215;
    D2D1_COLOR_F progressFill = D2D1::ColorF(0.584f, 0.835f, 0.608f, 1.0f); // #95D59B
    WidgetConfig widgets[5];
};

enum class ControlAction {
    None = 0,
    Shuffle,
    Previous,
    PlayPause,
    Next,
    Repeat,
    Seek
};

struct HitZone {
    D2D1_RECT_F rect;
    ControlAction action;
};

struct WidgetWindowState {
    int index = 0;
    HWND hwnd = nullptr;

    IWICBitmap* wicCanvas = nullptr;
    ID2D1RenderTarget* renderTarget = nullptr;
    UINT canvasW = 0;
    UINT canvasH = 0;

    ID2D1Bitmap* cachedArtBitmap = nullptr;
    uint64_t cachedArtVersion = 0;
    bool artCacheValid = false;

    bool isDragging = false;
    POINT dragStartCursor = {0, 0};
    POINT dragStartWinPos = {0, 0};

    ControlAction hoveredAction = ControlAction::None;
    std::vector<HitZone> hitZones;
    D2D1_RECT_F seekBarRect = {0, 0, 0, 0};
    bool hasSeekBar = false;
};

struct MediaState {
    std::wstring title = L"No Data";
    std::wstring artist = L"";
    bool isPlaying = false;
    bool isShuffleActive = false;
    int repeatMode = 0; // 0 = None, 1 = Track, 2 = List

    int64_t positionMs = 0;
    int64_t durationMs = 0;
    std::chrono::steady_clock::time_point lastTimelineSync;

    std::vector<uint8_t> artBytes;
    uint64_t artVersion = 0;
};

// ============================================================================
// Globals
// ============================================================================
static HINSTANCE g_hInstance = nullptr;
static std::mutex g_settingsMutex;
static GlobalSettings g_settings;

static std::mutex g_mediaMutex;
static MediaState g_media;

static ID2D1Factory* g_d2dFactory = nullptr;
static IDWriteFactory* g_dwriteFactory = nullptr;
static IWICImagingFactory* g_wicFactory = nullptr;

static std::atomic<HWND> g_uiThreadHwnd{nullptr};
static HANDLE g_uiThreadHandle = nullptr;
static HANDLE g_uiReadyEvent = nullptr;
static WidgetWindowState g_widgetWindows[5];

static HANDLE g_smtcThreadHandle = nullptr;
static HANDLE g_smtcStopEvent = nullptr;
static HANDLE g_smtcRefreshEvent = nullptr;

enum class PendingCommandType {
    TogglePlayPause,
    Next,
    Previous,
    ToggleShuffle,
    CycleRepeat,
    SeekToRatio
};

struct PendingCommand {
    PendingCommandType type;
    double seekRatio = 0.0;
};

static std::mutex g_cmdMutex;
static std::vector<PendingCommand> g_cmdQueue;
static HANDLE g_cmdEvent = nullptr;

#define WM_APP_RELOAD_SETTINGS (WM_APP + 1)
#define WM_APP_MEDIA_UPDATED   (WM_APP + 2)
#define WM_APP_DESTROY_UI      (WM_APP + 3)
#define TIMER_INTERPOLATE_ID   1001

static const wchar_t* kWindowClassName = L"WindhawkSMTCWidgetClass_v2";
static const wchar_t* kControllerClassName = L"WindhawkSMTCController_v2";

// ============================================================================
// Helpers: Color & Formatting
// ============================================================================
static DWORD ParseHexRGBtoBGR(const wchar_t* hexStr, DWORD fallbackBGR) {
    if (!hexStr) return fallbackBGR;
    if (hexStr[0] == L'#') hexStr++;
    unsigned int rgb = 0;
    if (swscanf_s(hexStr, L"%x", &rgb) != 1) return fallbackBGR;
    BYTE r = (rgb >> 16) & 0xFF;
    BYTE g = (rgb >> 8) & 0xFF;
    BYTE b = rgb & 0xFF;
    return (static_cast<DWORD>(b) << 16) | (static_cast<DWORD>(g) << 8) | static_cast<DWORD>(r);
}

static D2D1_COLOR_F ParseHexRGBtoD2D(const wchar_t* hexStr, D2D1_COLOR_F fallback) {
    if (!hexStr) return fallback;
    if (hexStr[0] == L'#') hexStr++;
    unsigned int rgb = 0;
    if (swscanf_s(hexStr, L"%x", &rgb) != 1) return fallback;
    float r = ((rgb >> 16) & 0xFF) / 255.0f;
    float g = ((rgb >> 8) & 0xFF) / 255.0f;
    float b = (rgb & 0xFF) / 255.0f;
    return D2D1::ColorF(r, g, b, 1.0f);
}

static std::wstring FormatTimeMs(int64_t ms) {
    if (ms < 0) ms = 0;
    int64_t totalSec = ms / 1000;
    int64_t mins = totalSec / 60;
    int64_t secs = totalSec % 60;
    wchar_t buf[32];
    swprintf_s(buf, L"%lld:%02lld", mins, secs);
    return buf;
}

static WidgetTheme ParseThemeName(const wchar_t* str) {
    if (!str) return WidgetTheme::Standard;
    if (_wcsicmp(str, L"album") == 0) return WidgetTheme::Album;
    if (_wcsicmp(str, L"cardstock") == 0) return WidgetTheme::Cardstock;
    if (_wcsicmp(str, L"simplified") == 0) return WidgetTheme::Simplified;
    return WidgetTheme::Standard;
}

// ============================================================================
// Settings Loader & Position Persistence
// ============================================================================
static void LoadAllSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    int count = Wh_GetIntSetting(L"WidgetCount");
    if (count < 1) count = 1;
    if (count > 5) count = 5;
    g_settings.widgetCount = count;

    g_settings.alwaysOnTop = Wh_GetIntSetting(L"AlwaysOnTop") != 0;

    auto tintStr = WindhawkUtils::StringSetting::make(L"AccentTintHex");
    g_settings.tintBGR = ParseHexRGBtoBGR(tintStr.get(), 0x2C190B);

    int alpha = Wh_GetIntSetting(L"AcrylicOpacity");
    if (alpha < 0) alpha = 0;
    if (alpha > 255) alpha = 255;
    g_settings.acrylicAlpha = static_cast<BYTE>(alpha);

    auto progStr = WindhawkUtils::StringSetting::make(L"ProgressFillHex");
    g_settings.progressFill = ParseHexRGBtoD2D(progStr.get(), D2D1::ColorF(0.584f, 0.835f, 0.608f, 1.0f));

    for (int i = 0; i < 5; i++) {
        wchar_t key[64];
        swprintf_s(key, L"Widget%d.Theme", i + 1);
        auto tStr = WindhawkUtils::StringSetting::make(key);
        g_settings.widgets[i].theme = ParseThemeName(tStr.get());

        swprintf_s(key, L"Widget%d.ScalePercent", i + 1);
        int sc = Wh_GetIntSetting(key);
        if (sc < 50) sc = 50;
        if (sc > 250) sc = 250;
        g_settings.widgets[i].scalePercent = sc;

        swprintf_s(key, L"Widget%d.DisableControls", i + 1);
        g_settings.widgets[i].disableControls = Wh_GetIntSetting(key) != 0;

        swprintf_s(key, L"Widget%d.LockPosition", i + 1);
        g_settings.widgets[i].lockPosition = Wh_GetIntSetting(key) != 0;
    }
}

static POINT LoadSavedWidgetPos(int index) {
    wchar_t keyX[32], keyY[32];
    swprintf_s(keyX, L"Pos_%d_X", index);
    swprintf_s(keyY, L"Pos_%d_Y", index);

    int x = Wh_GetIntValue(keyX, INT_MIN);
    int y = Wh_GetIntValue(keyY, INT_MIN);

    if (x == INT_MIN || y == INT_MIN) {
        x = 80 + (index * 40);
        y = 80 + (index * 40);
    } else {
        POINT pt = {x + 20, y + 20};
        if (!MonitorFromPoint(pt, MONITOR_DEFAULTTONULL)) {
            x = 80 + (index * 40);
            y = 80 + (index * 40);
        }
    }
    return {x, y};
}

static void SaveWidgetPos(int index, int x, int y) {
    wchar_t keyX[32], keyY[32];
    swprintf_s(keyX, L"Pos_%d_X", index);
    swprintf_s(keyY, L"Pos_%d_Y", index);
    Wh_SetIntValue(keyX, x);
    Wh_SetIntValue(keyY, y);
}

// ============================================================================
// Theme Logical Dimensions
// ============================================================================
static D2D1_SIZE_F GetLogicalBaseSize(const WidgetConfig& cfg) {
    switch (cfg.theme) {
        case WidgetTheme::Standard:
            return D2D1::SizeF(660.0f, 106.0f);
        case WidgetTheme::Album:
            return D2D1::SizeF(260.0f, 260.0f);
        case WidgetTheme::Cardstock:
            return cfg.disableControls ? D2D1::SizeF(260.0f, 356.0f)
                                       : D2D1::SizeF(260.0f, 392.0f);
        case WidgetTheme::Simplified:
            return D2D1::SizeF(660.0f, 74.0f);
    }
    return D2D1::SizeF(660.0f, 106.0f);
}

static float GetWindowDpiScale(HWND hwnd) {
    UINT dpi = 96;
    if (hwnd) {
        HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            dpi = dpiX;
        }
    }
    return static_cast<float>(dpi) / 96.0f;
}

// ============================================================================
// Window Composition: Acrylic Blur & Rounded Corners
// ============================================================================
static void ApplyWindowBackdrop(HWND hwnd, const WidgetConfig& cfg, const GlobalSettings& gs) {
    if (!hwnd) return;

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

    if (!g_SetWindowCompositionAttribute) {
        HMODULE hUser = GetModuleHandleW(L"user32.dll");
        if (hUser) {
            g_SetWindowCompositionAttribute = reinterpret_cast<pfnSetWindowCompositionAttribute>(
                GetProcAddress(hUser, "SetWindowCompositionAttribute"));
        }
    }

    if (g_SetWindowCompositionAttribute) {
        ACCENT_POLICY policy = {};
        if (cfg.theme == WidgetTheme::Album) {
            policy.AccentState = ACCENT_DISABLED;
        } else {
            policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
            policy.AccentFlags = 2;
            BYTE alpha = (gs.acrylicAlpha < 1) ? 1 : gs.acrylicAlpha;
            policy.GradientColor = (static_cast<DWORD>(alpha) << 24) | (gs.tintBGR & 0x00FFFFFF);
        }

        WINDOWCOMPOSITIONATTRIBDATA data = {};
        data.Attrib = 19; // WCA_ACCENT_POLICY
        data.pvData = &policy;
        data.cbData = sizeof(policy);
        g_SetWindowCompositionAttribute(hwnd, &data);
    }
}

// ============================================================================
// WIC Album Art Decoder -> D2D Bitmap
// ============================================================================
static const std::vector<uint8_t> kDefaultArtBytes = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x08, 0x03, 0x00, 0x00, 0x00, 0xC3, 0xA6, 0x24,
    0xC8, 0x00, 0x00, 0x05, 0x44, 0x69, 0x54, 0x58, 0x74, 0x58, 0x4D, 0x4C, 0x3A, 0x63, 0x6F, 0x6D,
    0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x78, 0x6D, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C,
    0x3F, 0x78, 0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x62, 0x65, 0x67, 0x69, 0x6E, 0x3D, 0x22,
    0xEF, 0xBB, 0xBF, 0x22, 0x20, 0x69, 0x64, 0x3D, 0x22, 0x57, 0x35, 0x4D, 0x30, 0x4D, 0x70, 0x43,
    0x65, 0x68, 0x69, 0x48, 0x7A, 0x72, 0x65, 0x53, 0x7A, 0x4E, 0x54, 0x63, 0x7A, 0x6B, 0x63, 0x39,
    0x64, 0x22, 0x3F, 0x3E, 0x0A, 0x3C, 0x78, 0x3A, 0x78, 0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61, 0x20,
    0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x3D, 0x22, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x3A, 0x6E,
    0x73, 0x3A, 0x6D, 0x65, 0x74, 0x61, 0x2F, 0x22, 0x20, 0x78, 0x3A, 0x78, 0x6D, 0x70, 0x74, 0x6B,
    0x3D, 0x22, 0x58, 0x4D, 0x50, 0x20, 0x43, 0x6F, 0x72, 0x65, 0x20, 0x35, 0x2E, 0x35, 0x2E, 0x30,
    0x22, 0x3E, 0x0A, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x52, 0x44, 0x46, 0x20, 0x78, 0x6D, 0x6C,
    0x6E, 0x73, 0x3A, 0x72, 0x64, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77,
    0x77, 0x77, 0x2E, 0x77, 0x33, 0x2E, 0x6F, 0x72, 0x67, 0x2F, 0x31, 0x39, 0x39, 0x39, 0x2F, 0x30,
    0x32, 0x2F, 0x32, 0x32, 0x2D, 0x72, 0x64, 0x66, 0x2D, 0x73, 0x79, 0x6E, 0x74, 0x61, 0x78, 0x2D,
    0x6E, 0x73, 0x23, 0x22, 0x3E, 0x0A, 0x20, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x44, 0x65, 0x73,
    0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x72, 0x64, 0x66, 0x3A, 0x61, 0x62, 0x6F,
    0x75, 0x74, 0x3D, 0x22, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A,
    0x65, 0x78, 0x69, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E,
    0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x65, 0x78, 0x69, 0x66, 0x2F, 0x31,
    0x2E, 0x30, 0x2F, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x70,
    0x68, 0x6F, 0x74, 0x6F, 0x73, 0x68, 0x6F, 0x70, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F,
    0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x70, 0x68,
    0x6F, 0x74, 0x6F, 0x73, 0x68, 0x6F, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x22, 0x0A, 0x20, 0x20,
    0x20, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x74, 0x69, 0x66, 0x66, 0x3D, 0x22, 0x68, 0x74,
    0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F,
    0x6D, 0x2F, 0x74, 0x69, 0x66, 0x66, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x22, 0x0A, 0x20, 0x20, 0x20,
    0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x6D, 0x70, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70,
    0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F,
    0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6D,
    0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A,
    0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x78,
    0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x6D, 0x6D, 0x2F, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20,
    0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x73, 0x74, 0x45, 0x76, 0x74, 0x3D, 0x22, 0x68, 0x74, 0x74,
    0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 0x6D,
    0x2F, 0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x73, 0x54, 0x79, 0x70, 0x65, 0x2F, 0x52,
    0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x45, 0x76, 0x65, 0x6E, 0x74, 0x23, 0x22, 0x0A, 0x20,
    0x20, 0x20, 0x65, 0x78, 0x69, 0x66, 0x3A, 0x43, 0x6F, 0x6C, 0x6F, 0x72, 0x53, 0x70, 0x61, 0x63,
    0x65, 0x3D, 0x22, 0x31, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x65, 0x78, 0x69, 0x66, 0x3A, 0x50, 0x69,
    0x78, 0x65, 0x6C, 0x58, 0x44, 0x69, 0x6D, 0x65, 0x6E, 0x73, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x35,
    0x31, 0x32, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x65, 0x78, 0x69, 0x66, 0x3A, 0x50, 0x69, 0x78, 0x65,
    0x6C, 0x59, 0x44, 0x69, 0x6D, 0x65, 0x6E, 0x73, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x35, 0x31, 0x32,
    0x22, 0x0A, 0x20, 0x20, 0x20, 0x70, 0x68, 0x6F, 0x74, 0x6F, 0x73, 0x68, 0x6F, 0x70, 0x3A, 0x43,
    0x6F, 0x6C, 0x6F, 0x72, 0x4D, 0x6F, 0x64, 0x65, 0x3D, 0x22, 0x33, 0x22, 0x0A, 0x20, 0x20, 0x20,
    0x70, 0x68, 0x6F, 0x74, 0x6F, 0x73, 0x68, 0x6F, 0x70, 0x3A, 0x49, 0x43, 0x43, 0x50, 0x72, 0x6F,
    0x66, 0x69, 0x6C, 0x65, 0x3D, 0x22, 0x73, 0x52, 0x47, 0x42, 0x20, 0x49, 0x45, 0x43, 0x36, 0x31,
    0x39, 0x36, 0x36, 0x2D, 0x32, 0x2E, 0x31, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x74, 0x69, 0x66, 0x66,
    0x3A, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x4C, 0x65, 0x6E, 0x67, 0x74, 0x68, 0x3D, 0x22, 0x35, 0x31,
    0x32, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x74, 0x69, 0x66, 0x66, 0x3A, 0x49, 0x6D, 0x61, 0x67, 0x65,
    0x57, 0x69, 0x64, 0x74, 0x68, 0x3D, 0x22, 0x35, 0x31, 0x32, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x74,
    0x69, 0x66, 0x66, 0x3A, 0x52, 0x65, 0x73, 0x6F, 0x6C, 0x75, 0x74, 0x69, 0x6F, 0x6E, 0x55, 0x6E,
    0x69, 0x74, 0x3D, 0x22, 0x32, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x74, 0x69, 0x66, 0x66, 0x3A, 0x58,
    0x52, 0x65, 0x73, 0x6F, 0x6C, 0x75, 0x74, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x33, 0x30, 0x30, 0x2F,
    0x31, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x74, 0x69, 0x66, 0x66, 0x3A, 0x59, 0x52, 0x65, 0x73, 0x6F,
    0x6C, 0x75, 0x74, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x33, 0x30, 0x30, 0x2F, 0x31, 0x22, 0x0A, 0x20,
    0x20, 0x20, 0x78, 0x6D, 0x70, 0x3A, 0x4D, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x44, 0x61,
    0x74, 0x65, 0x3D, 0x22, 0x32, 0x30, 0x32, 0x34, 0x2D, 0x30, 0x32, 0x2D, 0x31, 0x30, 0x54, 0x31,
    0x34, 0x3A, 0x31, 0x34, 0x3A, 0x30, 0x34, 0x2D, 0x30, 0x35, 0x3A, 0x30, 0x30, 0x22, 0x0A, 0x20,
    0x20, 0x20, 0x78, 0x6D, 0x70, 0x3A, 0x4D, 0x6F, 0x64, 0x69, 0x66, 0x79, 0x44, 0x61, 0x74, 0x65,
    0x3D, 0x22, 0x32, 0x30, 0x32, 0x34, 0x2D, 0x30, 0x32, 0x2D, 0x31, 0x30, 0x54, 0x31, 0x34, 0x3A,
    0x31, 0x34, 0x3A, 0x30, 0x34, 0x2D, 0x30, 0x35, 0x3A, 0x30, 0x30, 0x22, 0x3E, 0x0A, 0x20, 0x20,
    0x20, 0x3C, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3A, 0x48, 0x69, 0x73, 0x74, 0x6F, 0x72, 0x79, 0x3E,
    0x0A, 0x20, 0x20, 0x20, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x53, 0x65, 0x71, 0x3E, 0x0A, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x6C, 0x69, 0x0A, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3A, 0x61, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x3D, 0x22,
    0x70, 0x72, 0x6F, 0x64, 0x75, 0x63, 0x65, 0x64, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3A, 0x73, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x41, 0x67,
    0x65, 0x6E, 0x74, 0x3D, 0x22, 0x41, 0x66, 0x66, 0x69, 0x6E, 0x69, 0x74, 0x79, 0x20, 0x44, 0x65,
    0x73, 0x69, 0x67, 0x6E, 0x65, 0x72, 0x20, 0x31, 0x2E, 0x38, 0x2E, 0x33, 0x22, 0x0A, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3A, 0x77, 0x68, 0x65, 0x6E, 0x3D, 0x22,
    0x32, 0x30, 0x32, 0x30, 0x2D, 0x30, 0x36, 0x2D, 0x31, 0x39, 0x54, 0x32, 0x32, 0x3A, 0x30, 0x34,
    0x3A, 0x31, 0x35, 0x2D, 0x30, 0x34, 0x3A, 0x30, 0x30, 0x22, 0x2F, 0x3E, 0x0A, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x6C, 0x69, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x73, 0x74, 0x45, 0x76, 0x74, 0x3A, 0x61, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x70, 0x72,
    0x6F, 0x64, 0x75, 0x63, 0x65, 0x64, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74,
    0x45, 0x76, 0x74, 0x3A, 0x73, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x41, 0x67, 0x65, 0x6E,
    0x74, 0x3D, 0x22, 0x41, 0x66, 0x66, 0x69, 0x6E, 0x69, 0x74, 0x79, 0x20, 0x50, 0x68, 0x6F, 0x74,
    0x6F, 0x20, 0x31, 0x2E, 0x31, 0x30, 0x2E, 0x36, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x73, 0x74, 0x45, 0x76, 0x74, 0x3A, 0x77, 0x68, 0x65, 0x6E, 0x3D, 0x22, 0x32, 0x30, 0x32, 0x34,
    0x2D, 0x30, 0x32, 0x2D, 0x31, 0x30, 0x54, 0x31, 0x34, 0x3A, 0x31, 0x34, 0x3A, 0x30, 0x34, 0x2D,
    0x30, 0x35, 0x3A, 0x30, 0x30, 0x22, 0x2F, 0x3E, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x3C, 0x2F, 0x72,
    0x64, 0x66, 0x3A, 0x53, 0x65, 0x71, 0x3E, 0x0A, 0x20, 0x20, 0x20, 0x3C, 0x2F, 0x78, 0x6D, 0x70,
    0x4D, 0x4D, 0x3A, 0x48, 0x69, 0x73, 0x74, 0x6F, 0x72, 0x79, 0x3E, 0x0A, 0x20, 0x20, 0x3C, 0x2F,
    0x72, 0x64, 0x66, 0x3A, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x3E,
    0x0A, 0x20, 0x3C, 0x2F, 0x72, 0x64, 0x66, 0x3A, 0x52, 0x44, 0x46, 0x3E, 0x0A, 0x3C, 0x2F, 0x78,
    0x3A, 0x78, 0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61, 0x3E, 0x0A, 0x3C, 0x3F, 0x78, 0x70, 0x61, 0x63,
    0x6B, 0x65, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x3D, 0x22, 0x72, 0x22, 0x3F, 0x3E, 0x2C, 0x5B, 0xC5,
    0xE9, 0x00, 0x00, 0x01, 0x81, 0x69, 0x43, 0x43, 0x50, 0x73, 0x52, 0x47, 0x42, 0x20, 0x49, 0x45,
    0x43, 0x36, 0x31, 0x39, 0x36, 0x36, 0x2D, 0x32, 0x2E, 0x31, 0x00, 0x00, 0x28, 0x91, 0x75, 0x91,
    0xCF, 0x2B, 0x44, 0x51, 0x14, 0xC7, 0x3F, 0x33, 0x43, 0xC4, 0x88, 0xB0, 0xB0, 0xB0, 0x78, 0x31,
    0xAC, 0x90, 0x1F, 0x35, 0x91, 0xB2, 0x98, 0xC9, 0xAF, 0xC2, 0x62, 0x66, 0x94, 0xC1, 0xE6, 0xCD,
    0xF3, 0x66, 0x46, 0xCD, 0x8C, 0xD7, 0x7B, 0x4F, 0x92, 0xAD, 0xB2, 0x55, 0x94, 0xD8, 0xF8, 0xB5,
    0xE0, 0x2F, 0x60, 0xAB, 0xAC, 0x95, 0x22, 0x52, 0xB2, 0x9E, 0x35, 0xB1, 0x41, 0xCF, 0x79, 0x46,
    0x8D, 0x64, 0xCE, 0xED, 0xDC, 0xF3, 0xB9, 0xDF, 0x7B, 0xCF, 0xE9, 0xDE, 0x73, 0xC1, 0x1B, 0xCB,
    0x68, 0x59, 0xAB, 0xAC, 0x1B, 0xB2, 0x39, 0xDB, 0x8C, 0x8C, 0x86, 0x94, 0x99, 0xF8, 0xAC, 0x52,
    0x91, 0xC7, 0x4B, 0x15, 0xF5, 0xB4, 0x32, 0xA8, 0x6A, 0x96, 0x31, 0x19, 0x1D, 0x89, 0x51, 0xD2,
    0xDE, 0xEE, 0xF0, 0xB8, 0xF1, 0xA6, 0xD3, 0xAD, 0x55, 0xFA, 0xDC, 0xBF, 0x56, 0xBD, 0xA0, 0x5B,
    0x1A, 0x78, 0x2A, 0x85, 0x87, 0x34, 0xC3, 0xB4, 0x85, 0xC7, 0x84, 0x27, 0x56, 0x6C, 0xC3, 0xE5,
    0x6D, 0xE1, 0x46, 0x2D, 0xAD, 0x2E, 0x08, 0x9F, 0x0A, 0x77, 0x98, 0x72, 0x41, 0xE1, 0x5B, 0x57,
    0x4F, 0x14, 0x38, 0xEF, 0x72, 0xAA, 0xC0, 0x1F, 0x2E, 0x9B, 0xB1, 0x48, 0x18, 0xBC, 0x75, 0xC2,
    0x4A, 0xEA, 0x17, 0x27, 0x7E, 0xB1, 0x96, 0x36, 0xB3, 0xC2, 0xF2, 0x72, 0x02, 0xD9, 0xCC, 0xB2,
    0xF6, 0x73, 0x1F, 0xF7, 0x25, 0x7E, 0x3D, 0x37, 0x1D, 0x95, 0xD8, 0x22, 0xDE, 0x8C, 0x45, 0x84,
    0x51, 0x42, 0x28, 0x8C, 0x33, 0x4C, 0x98, 0x20, 0x3D, 0x0C, 0xC8, 0x1C, 0xA4, 0x93, 0x5E, 0xBA,
    0x64, 0x45, 0x89, 0xFC, 0xEE, 0xEF, 0xFC, 0x29, 0x96, 0x24, 0x57, 0x93, 0xD9, 0x60, 0x15, 0x93,
    0x45, 0x52, 0xA4, 0xB1, 0xE9, 0x10, 0x75, 0x59, 0xAA, 0xEB, 0x12, 0x93, 0xA2, 0xEB, 0x32, 0x32,
    0xAC, 0xBA, 0xFD, 0xFF, 0xDB, 0x57, 0x2B, 0xD9, 0xD7, 0x5B, 0xA8, 0xEE, 0x0F, 0x41, 0xF9, 0x93,
    0xE3, 0xBC, 0xB4, 0x41, 0xC5, 0x16, 0x7C, 0x6E, 0x3A, 0xCE, 0xFB, 0xA1, 0xE3, 0x7C, 0x1E, 0x81,
    0xEF, 0x11, 0x2E, 0x72, 0xC5, 0xFC, 0xA5, 0x03, 0xE8, 0x7F, 0x15, 0x7D, 0xB3, 0xA8, 0x05, 0xF6,
    0xA1, 0x76, 0x1D, 0xCE, 0x2E, 0x8B, 0x5A, 0x62, 0x07, 0xCE, 0x37, 0xA0, 0xE9, 0xC1, 0x50, 0x4D,
    0xF5, 0x5B, 0xF2, 0x89, 0x7B, 0x93, 0x49, 0x78, 0x3E, 0x81, 0x9A, 0x38, 0x34, 0x5C, 0x43, 0xD5,
    0x5C, 0xA1, 0x67, 0x3F, 0xFB, 0x1C, 0xDF, 0x43, 0x6C, 0x4D, 0xBE, 0xEA, 0x0A, 0x76, 0xF7, 0xA0,
    0x5D, 0xCE, 0xD7, 0xCE, 0x7F, 0x01, 0xA8, 0xE7, 0x68, 0x04, 0x6D, 0x7E, 0x5E, 0x6D, 0x00, 0x00,
    0x00, 0x93, 0x50, 0x4C, 0x54, 0x45, 0x1C, 0x1C, 0x1C, 0x10, 0x10, 0x19, 0x0D, 0x0D, 0x1A, 0x00,
    0x00, 0x2A, 0x11, 0x11, 0x1C, 0x12, 0x12, 0x1E, 0x0F, 0x0F, 0x1D, 0x14, 0x14, 0x1B, 0x00, 0x00,
    0x33, 0x0F, 0x0F, 0x1B, 0x15, 0x15, 0x15, 0x0D, 0x0D, 0x1E, 0x00, 0x00, 0x24, 0x16, 0x16, 0x19,
    0x0D, 0x0D, 0x14, 0x17, 0x17, 0x17, 0x0C, 0x0C, 0x13, 0x0F, 0x0F, 0x19, 0x20, 0x20, 0x20, 0x14,
    0x14, 0x1C, 0x15, 0x15, 0x20, 0x0F, 0x0F, 0x1E, 0x12, 0x12, 0x19, 0x0A, 0x0A, 0x22, 0x12, 0x12,
    0x1C, 0x7F, 0x63, 0x2A, 0x0C, 0x0C, 0x17, 0x01, 0x01, 0x02, 0x10, 0x10, 0x20, 0x0D, 0x0D, 0x20,
    0x1A, 0x1A, 0x1A, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x17, 0x12, 0x12, 0x1B, 0x16, 0x16, 0x1D, 0x11,
    0x11, 0x22, 0x02, 0x02, 0x28, 0x1F, 0x1F, 0x1F, 0x0B, 0x0B, 0x1F, 0x12, 0x12, 0x14, 0x0D, 0x0D,
    0x0D, 0x10, 0x10, 0x16, 0x0C, 0x0C, 0x18, 0x14, 0x14, 0x1F, 0x11, 0x11, 0x1E, 0x09, 0x09, 0x1F,
    0x12, 0x12, 0x12, 0x11, 0x11, 0x1B, 0xC9, 0x45, 0x26, 0x4A, 0xE9, 0x79, 0x1C, 0x00, 0x00, 0x00,
    0x31, 0x74, 0x52, 0x4E, 0x53, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x1F, 0x9A, 0x4E, 0x21, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48,
    0x59, 0x73, 0x00, 0x00, 0x2E, 0x23, 0x00, 0x00, 0x2E, 0x23, 0x01, 0x78, 0xA5, 0x3F, 0x76, 0x00,
    0x00, 0x09, 0x99, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xED, 0xDD, 0x5D, 0x8E, 0x1C, 0x37, 0x16,
    0x05, 0x61, 0xEF, 0x68, 0xBC, 0xFF, 0xD5, 0x0D, 0x0C, 0x03, 0x83, 0x51, 0x37, 0xA5, 0xCE, 0x4A,
    0x9E, 0x43, 0x32, 0x33, 0xE2, 0x7B, 0xB6, 0xB2, 0x78, 0x79, 0xC3, 0x0F, 0x82, 0xEA, 0xE7, 0xAF,
    0xBF, 0x85, 0xF6, 0xD7, 0xEE, 0x03, 0x68, 0x2F, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33,
    0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00,
    0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38,
    0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03,
    0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80,
    0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33,
    0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00,
    0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38,
    0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03,
    0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80,
    0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33,
    0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00,
    0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38,
    0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03,
    0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80,
    0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33,
    0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00,
    0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38,
    0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03,
    0x88, 0xF8, 0xCF, 0xA4, 0x7D, 0x27, 0x37, 0x80, 0x49, 0xB3, 0xAB, 0xDF, 0x9D, 0x81, 0x01, 0x4C,
    0x49, 0xAE, 0x7F, 0x4F, 0x01, 0x06, 0x30, 0x21, 0xBC, 0xFE, 0x2D, 0x09, 0x18, 0xC0, 0x7D, 0x85,
    0xFD, 0xAF, 0x2F, 0xC0, 0x00, 0xEE, 0xAB, 0x04, 0xB0, 0xBA, 0x00, 0x03, 0xB8, 0xAD, 0xB3, 0xFF,
    0xD5, 0x05, 0x18, 0xC0, 0x5D, 0xAD, 0xFD, 0x1B, 0xC0, 0x43, 0xD4, 0x02, 0x58, 0x5B, 0x80, 0x01,
    0xDC, 0xD4, 0xDB, 0xBF, 0x01, 0x3C, 0x42, 0x31, 0x80, 0xA5, 0x05, 0x18, 0xC0, 0x4D, 0x06, 0xC0,
    0xD6, 0xDC, 0xBF, 0x01, 0x3C, 0x80, 0x01, 0xC0, 0x19, 0x00, 0x9C, 0x01, 0x7C, 0xA6, 0x7A, 0x5F,
    0x29, 0xC7, 0x0C, 0xD4, 0xDA, 0xC2, 0xC8, 0x8A, 0x00, 0xAA, 0x97, 0x15, 0x76, 0xC4, 0x4C, 0xCD,
    0x65, 0x7C, 0xB5, 0x20, 0x80, 0xEA, 0x5D, 0xC5, 0x9D, 0x30, 0x54, 0x75, 0x1D, 0x5F, 0xD4, 0x03,
    0xA8, 0xDE, 0x54, 0xC3, 0x01, 0x63, 0x75, 0x37, 0xF2, 0xAB, 0x76, 0x00, 0xD5, 0x8B, 0x2A, 0xD9,
    0x3D, 0x57, 0x79, 0x25, 0xBF, 0x2A, 0x07, 0xD0, 0xBC, 0xA7, 0x9A, 0xDD, 0x83, 0x75, 0x57, 0xF2,
    0x85, 0x01, 0x0C, 0x6C, 0x1E, 0xAC, 0xBB, 0x92, 0x2F, 0xBA, 0x01, 0x34, 0xAF, 0xA9, 0x69, 0xEB,
    0x64, 0xD5, 0x8D, 0x7C, 0x63, 0x00, 0x23, 0x3B, 0x27, 0xAB, 0x2E, 0xE4, 0xBB, 0x6A, 0x00, 0xBD,
    0x5B, 0x6A, 0xDB, 0x38, 0x5A, 0x73, 0x21, 0x03, 0x06, 0x30, 0xB4, 0x6D, 0xB4, 0xE6, 0x3A, 0x86,
    0x0C, 0x60, 0x68, 0xD7, 0x68, 0xCD, 0x6D, 0x8C, 0x35, 0x03, 0x28, 0x5D, 0xD2, 0x0A, 0x9B, 0x66,
    0x2B, 0x2E, 0xE3, 0x77, 0x0C, 0x60, 0x68, 0xC3, 0x6C, 0xC5, 0x45, 0xFC, 0x89, 0x01, 0x0C, 0xCD,
    0xCE, 0x56, 0xBC, 0xD5, 0x30, 0x03, 0x18, 0x9A, 0x9D, 0xAD, 0x78, 0xAB, 0x61, 0x06, 0x30, 0x34,
    0x3B, 0x5B, 0xF1, 0x56, 0xC3, 0x0C, 0x60, 0x68, 0x76, 0xB6, 0xE2, 0xAD, 0x86, 0xF9, 0xD7, 0xC0,
    0xA1, 0xD9, 0xD1, 0x9A, 0xB7, 0x9A, 0x65, 0x00, 0x43, 0xB3, 0xA3, 0x35, 0x6F, 0x35, 0xCB, 0x00,
    0x86, 0x66, 0x47, 0x6B, 0xDE, 0x6A, 0x96, 0xFF, 0x18, 0x34, 0x32, 0x3D, 0x59, 0xF5, 0x56, 0xA3,
    0x0C, 0x60, 0x60, 0x7E, 0xB2, 0xEA, 0xAD, 0x46, 0x19, 0xC0, 0xC0, 0xFC, 0x64, 0xD5, 0x5B, 0x8D,
    0xF2, 0x3D, 0x81, 0xDF, 0x05, 0xE6, 0x2A, 0xDF, 0x6A, 0x90, 0xEF, 0x0A, 0xFE, 0x26, 0x31, 0x56,
    0xF9, 0x52, 0x83, 0x0C, 0xE0, 0xAB, 0xC8, 0x58, 0xDD, 0x3B, 0x4D, 0xF2, 0x83, 0x21, 0x5F, 0x64,
    0x86, 0xAA, 0x5E, 0x68, 0xD4, 0x92, 0xCF, 0x06, 0x2E, 0xD9, 0x5C, 0x44, 0x6A, 0xA4, 0xE2, 0x65,
    0x86, 0xAD, 0xFB, 0x74, 0xF0, 0x92, 0x05, 0xDE, 0x97, 0x9D, 0xA6, 0x72, 0x83, 0x15, 0x7E, 0x3C,
    0xFC, 0x1E, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00,
    0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38,
    0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03,
    0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80,
    0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33, 0x00, 0x38, 0x03, 0x80, 0x33,
    0x00, 0x38, 0x03, 0xF8, 0xC8, 0x0F, 0xD7, 0x75, 0x8C, 0xD8, 0x44, 0xBD, 0xAB, 0x4C, 0xF3, 0xE7,
    0xE3, 0x7F, 0x15, 0x9A, 0xA9, 0x79, 0x9D, 0x59, 0x7E, 0x4B, 0xD8, 0x17, 0x99, 0xA1, 0xAA, 0x17,
    0x1A, 0xE5, 0xF7, 0x04, 0x7E, 0x93, 0x18, 0xAB, 0x7C, 0xA9, 0x41, 0x7E, 0x55, 0xEC, 0x77, 0x81,
    0xB9, 0xCA, 0xB7, 0x1A, 0xE4, 0xAF, 0x87, 0x7F, 0x17, 0x18, 0xAC, 0x7B, 0xAB, 0x49, 0x7E, 0x5B,
    0xF8, 0xC0, 0xFC, 0x64, 0xD5, 0x5B, 0x8D, 0x32, 0x80, 0x91, 0xE9, 0xC9, 0xAA, 0xB7, 0x1A, 0x65,
    0x00, 0x43, 0xB3, 0x93, 0x55, 0x6F, 0x35, 0xCA, 0xDF, 0x0C, 0x1A, 0x9A, 0x1D, 0xAD, 0x79, 0xAB,
    0x59, 0x06, 0x30, 0x34, 0x3B, 0x5A, 0xF3, 0x56, 0xB3, 0x0C, 0x60, 0x68, 0x76, 0xB4, 0xE6, 0xAD,
    0x66, 0xF9, 0xCB, 0xA1, 0x43, 0xB3, 0xB3, 0x15, 0x6F, 0x35, 0xCC, 0x00, 0x86, 0x66, 0x67, 0x2B,
    0xDE, 0x6A, 0x98, 0x01, 0x0C, 0xCD, 0xCE, 0x56, 0xBC, 0xD5, 0x30, 0x03, 0x18, 0x9A, 0x9D, 0xAD,
    0x78, 0xAB, 0x61, 0x06, 0x30, 0x34, 0x3B, 0x5B, 0xF1, 0x56, 0xC3, 0xFC, 0x5B, 0xC0, 0xD0, 0xEC,
    0x68, 0xCD, 0x5B, 0xCD, 0x32, 0x80, 0xA1, 0xD9, 0xD1, 0x9A, 0xB7, 0x9A, 0x65, 0x00, 0x43, 0xB3,
    0xA3, 0x35, 0x6F, 0x35, 0xCB, 0x7F, 0x0B, 0x18, 0x9A, 0x9D, 0xAC, 0x7A, 0xAB, 0x51, 0x06, 0x30,
    0x32, 0x3D, 0x59, 0xF5, 0x56, 0xA3, 0x0C, 0x60, 0x60, 0x7E, 0xB2, 0xEA, 0xAD, 0x46, 0xF9, 0x8E,
    0xA0, 0x81, 0xF9, 0xC1, 0xBA, 0xB7, 0x9A, 0xE4, 0x7B, 0x02, 0xBF, 0x0B, 0xCC, 0x55, 0xBE, 0xD5,
    0x20, 0xDF, 0x15, 0xFC, 0x4D, 0x62, 0xAC, 0xF2, 0xA5, 0x06, 0xF5, 0x3F, 0x17, 0xB0, 0x64, 0x69,
    0x41, 0x91, 0xA9, 0xBA, 0x57, 0x9A, 0xE4, 0x07, 0x43, 0xBE, 0xC8, 0x0C, 0x55, 0xBD, 0xD0, 0x28,
    0x3F, 0x1B, 0xF8, 0xFF, 0x62, 0x13, 0xF5, 0xAE, 0x32, 0x6D, 0xD9, 0xA7, 0x83, 0x97, 0x2C, 0x70,
    0x4A, 0x72, 0x9C, 0xD2, 0x25, 0x16, 0xF8, 0xF1, 0xF0, 0x7B, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C,
    0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00,
    0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE,
    0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00,
    0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0,
    0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0x3E, 0xF7, 0xC3, 0x9D, 0x4D, 0x5A, 0x37,
    0xC7, 0x95, 0x61, 0x16, 0x9F, 0x66, 0xC2, 0xBB, 0xBE, 0x22, 0x66, 0xC9, 0x30, 0x57, 0x26, 0x5A,
    0x77, 0x90, 0x59, 0xAF, 0xFB, 0x92, 0xA8, 0xFE, 0x3C, 0x57, 0x86, 0x5A, 0x74, 0x8A, 0x80, 0x17,
    0x7E, 0x4D, 0x5C, 0x7D, 0xA2, 0x0B, 0x53, 0x2D, 0x39, 0x43, 0xC4, 0x1B, 0xBF, 0x28, 0xB2, 0x3D,
    0xD2, 0x85, 0xB1, 0x56, 0x1C, 0x21, 0xE3, 0x95, 0x5F, 0x15, 0x5B, 0x9E, 0xE9, 0xC2, 0x5C, 0x0B,
    0x4E, 0x10, 0xF2, 0xCE, 0x2F, 0x8B, 0xEE, 0x0E, 0x75, 0x61, 0xB0, 0xFE, 0x01, 0x52, 0xDE, 0xF9,
    0x75, 0xF1, 0xD5, 0xA1, 0xAE, 0x4C, 0xD6, 0x3F, 0x40, 0xCA, 0x3B, 0x03, 0xE8, 0x2F, 0x60, 0xF7,
    0xEB, 0xC7, 0x18, 0x40, 0x65, 0xB2, 0xFA, 0xEB, 0xC7, 0xBC, 0xF5, 0x47, 0xA3, 0x9A, 0x63, 0x5D,
    0x18, 0xAD, 0xFD, 0xF2, 0x39, 0x06, 0x50, 0x19, 0xAD, 0xFD, 0xF2, 0x39, 0x06, 0x50, 0x19, 0xAD,
    0xFD, 0xF2, 0x39, 0xAF, 0xFD, 0xE9, 0xD8, 0xE2, 0x5C, 0x17, 0x66, 0x2B, 0xBF, 0x7A, 0x90, 0x01,
    0x54, 0x66, 0x2B, 0xBF, 0x7A, 0x90, 0x01, 0x54, 0x66, 0x2B, 0xBF, 0x7A, 0x90, 0x01, 0xFC, 0xF1,
    0xD0, 0x77, 0x67, 0xBB, 0xFB, 0xEA, 0xEB, 0x19, 0xC0, 0xCF, 0xE7, 0xBD, 0x31, 0xDB, 0xE4, 0xCD,
    0x2D, 0xE4, 0xDF, 0x02, 0xAE, 0x9D, 0x75, 0xF2, 0x3F, 0x3F, 0x97, 0x01, 0x5C, 0x3F, 0xE6, 0x07,
    0x7F, 0x26, 0x7B, 0x8F, 0x4D, 0x06, 0x70, 0xEF, 0xA9, 0xB1, 0x97, 0xDF, 0x8D, 0xFE, 0x6F, 0x01,
    0x77, 0x1F, 0x9C, 0x7A, 0xFD, 0xED, 0xD0, 0x01, 0x4C, 0x3C, 0x3B, 0xF2, 0xFA, 0x27, 0x78, 0x67,
    0x00, 0xE5, 0xD3, 0xFD, 0xFC, 0x67, 0xAB, 0xB7, 0x1A, 0xC5, 0x7D, 0x47, 0xD0, 0xDC, 0xF3, 0x03,
    0x07, 0x38, 0x03, 0xF5, 0x3D, 0x81, 0xFB, 0x4F, 0x70, 0x08, 0xE8, 0xBB, 0x82, 0x0F, 0x38, 0xC2,
    0x21, 0x90, 0x01, 0x1C, 0x70, 0x84, 0x63, 0xAC, 0xF8, 0x68, 0x58, 0xFF, 0xBE, 0x3F, 0xBB, 0xFA,
    0x33, 0x4E, 0x71, 0x88, 0x25, 0x9F, 0x0D, 0x5C, 0x70, 0xE3, 0xD7, 0x6F, 0xFE, 0x90, 0x63, 0x9C,
    0x02, 0xF7, 0xF1, 0xF0, 0x25, 0xFB, 0x37, 0x80, 0x63, 0xAD, 0xD9, 0xBF, 0x01, 0x9C, 0x6A, 0xD1,
    0xFE, 0x0D, 0xE0, 0x4C, 0xAB, 0xD6, 0x6F, 0x00, 0x87, 0x32, 0x80, 0xEF, 0x48, 0x01, 0xAC, 0xDB,
    0xBF, 0x01, 0x1C, 0x68, 0xE1, 0xFA, 0x0D, 0xE0, 0x44, 0x06, 0x30, 0x84, 0x09, 0x60, 0xE9, 0xFE,
    0x0D, 0xE0, 0x38, 0x6B, 0xF7, 0x6F, 0x00, 0xA7, 0x59, 0xBC, 0x7F, 0x03, 0x38, 0x8D, 0x01, 0xFC,
    0x0E, 0x23, 0x80, 0xD5, 0xFB, 0x37, 0x80, 0xB3, 0x2C, 0xDF, 0xBF, 0x01, 0x1C, 0x65, 0xFD, 0xFE,
    0x0D, 0xE0, 0x28, 0x06, 0xF0, 0x07, 0x80, 0x00, 0x36, 0xEC, 0xDF, 0x00, 0x0E, 0x32, 0xBB, 0x37,
    0x03, 0x78, 0xB8, 0xF9, 0xAD, 0x19, 0xC0, 0xA3, 0x25, 0x76, 0x66, 0x00, 0xCF, 0x95, 0xDA, 0x98,
    0x01, 0x3C, 0x54, 0x6C, 0x5F, 0x06, 0xF0, 0x48, 0xC1, 0x75, 0x19, 0xC0, 0x03, 0x65, 0x97, 0x65,
    0x00, 0x8F, 0x13, 0x5E, 0x96, 0x01, 0x3C, 0x4D, 0x7A, 0x57, 0x06, 0xF0, 0x2C, 0xF9, 0x4D, 0x19,
    0xC0, 0xA3, 0x18, 0xC0, 0x8F, 0x5E, 0x1D, 0x40, 0x65, 0x51, 0x06, 0xF0, 0x1C, 0x9D, 0x3D, 0x19,
    0xC0, 0x63, 0x18, 0xC0, 0xCF, 0xDE, 0x1C, 0x40, 0x6B, 0x4D, 0x06, 0xF0, 0x10, 0xAD, 0x2D, 0x19,
    0xC0, 0x43, 0xD4, 0x96, 0x64, 0x00, 0xCF, 0x60, 0x00, 0x17, 0x18, 0x40, 0xFE, 0xC1, 0x06, 0x70,
    0x86, 0xE2, 0x92, 0x0C, 0xE0, 0x09, 0x8A, 0x3B, 0x32, 0x80, 0x27, 0x68, 0xAE, 0xC8, 0x00, 0x1E,
    0xC0, 0x00, 0xAE, 0x30, 0x80, 0xC6, 0xC3, 0x0D, 0xE0, 0x00, 0xDD, 0x0D, 0x19, 0xC0, 0xF1, 0x0C,
    0xE0, 0x12, 0x03, 0x68, 0x3C, 0xDD, 0x00, 0x0E, 0x60, 0x00, 0x97, 0x18, 0x40, 0xE3, 0xE9, 0x06,
    0x70, 0x00, 0x03, 0xB8, 0xC4, 0x00, 0x1A, 0x4F, 0x37, 0x80, 0x03, 0x18, 0xC0, 0x25, 0x06, 0xD0,
    0x78, 0xBA, 0x01, 0x1C, 0x60, 0x67, 0x00, 0x89, 0xF3, 0x2F, 0x62, 0x00, 0x27, 0x3E, 0x7D, 0x21,
    0x03, 0x38, 0xF1, 0xE9, 0x0B, 0x19, 0xC0, 0x89, 0x4F, 0x5F, 0xC8, 0x00, 0x4E, 0x7C, 0xFA, 0x42,
    0x06, 0x70, 0xE2, 0xD3, 0x17, 0x7A, 0x6F, 0x00, 0x3B, 0xDF, 0x10, 0x92, 0x19, 0x60, 0x09, 0x03,
    0x38, 0xEE, 0xD9, 0x6B, 0x19, 0xC0, 0x71, 0xCF, 0x5E, 0x0B, 0x1B, 0x40, 0xF3, 0x5D, 0xC1, 0xA9,
    0x09, 0x56, 0x78, 0x71, 0x00, 0xC5, 0xFF, 0x4D, 0x0D, 0xE0, 0x11, 0x6A, 0x6B, 0xFA, 0xE1, 0xC1,
    0x06, 0x70, 0x88, 0x5D, 0x01, 0xE4, 0x26, 0x58, 0xC0, 0x00, 0x4E, 0x7A, 0xF0, 0x06, 0xE0, 0x00,
    0x6A, 0xDF, 0x0F, 0x90, 0x9C, 0xA1, 0x8E, 0x1C, 0xC0, 0xCD, 0x4D, 0x95, 0x1E, 0xBB, 0xC9, 0x9B,
    0x03, 0xD8, 0xF5, 0x1D, 0x41, 0xE1, 0x29, 0xBA, 0x0C, 0xE0, 0x88, 0x87, 0xEE, 0xF3, 0xEA, 0x00,
    0x2A, 0xCB, 0x32, 0x80, 0x07, 0xF9, 0x71, 0x59, 0x9F, 0x6F, 0xEB, 0x65, 0xFB, 0xC7, 0x07, 0xF0,
    0xE9, 0xBE, 0x0A, 0x49, 0xED, 0xF5, 0xEE, 0x00, 0xE2, 0xFB, 0xCA, 0x17, 0xB5, 0x1B, 0x3E, 0x80,
    0xF4, 0xEF, 0x05, 0xD4, 0x46, 0x29, 0x31, 0x80, 0x0F, 0x76, 0x16, 0xCE, 0xE9, 0x08, 0x2F, 0x0F,
    0x60, 0xF9, 0x6F, 0x06, 0x75, 0xA7, 0x29, 0x30, 0x80, 0xAB, 0x6B, 0xCB, 0x3D, 0xE9, 0x28, 0x6F,
    0x0F, 0x20, 0xF5, 0x23, 0x2F, 0x99, 0xA7, 0x1C, 0xE8, 0xF5, 0x01, 0xAC, 0xFD, 0xE5, 0xD0, 0x55,
    0x43, 0xE5, 0x18, 0xC0, 0xCF, 0xDB, 0xBB, 0xFE, 0x04, 0x03, 0x38, 0xD1, 0x07, 0xFB, 0xFB, 0xBE,
    0xC1, 0x8F, 0xFE, 0xF0, 0x03, 0xF7, 0x6F, 0x00, 0x7F, 0x58, 0xE3, 0xED, 0x3F, 0xF8, 0x24, 0x80,
    0x00, 0xEE, 0xFC, 0xFC, 0xFB, 0x2D, 0xBB, 0xE7, 0xBC, 0xC5, 0x00, 0x0C, 0xE0, 0xFD, 0xDC, 0xFF,
    0xEF, 0x21, 0x02, 0x58, 0x52, 0xC0, 0xEE, 0x19, 0x6F, 0x62, 0x04, 0xB0, 0xA0, 0x80, 0xDD, 0x13,
    0xDE, 0x65, 0x00, 0x06, 0x80, 0xE0, 0xFE, 0x7F, 0x83, 0x12, 0x40, 0xB9, 0x80, 0xDD, 0xD3, 0xDD,
    0x87, 0x09, 0xA0, 0x5A, 0xC0, 0xEE, 0xD9, 0x26, 0x70, 0x02, 0x28, 0x16, 0xB0, 0x7B, 0xB2, 0x19,
    0xA0, 0x00, 0x6A, 0x05, 0xEC, 0x9E, 0x6B, 0x0A, 0x29, 0x80, 0x56, 0x01, 0xBB, 0xC7, 0x9A, 0x82,
    0x0A, 0xA0, 0x52, 0xC0, 0xEE, 0x99, 0x26, 0xB1, 0x02, 0x28, 0x14, 0xB0, 0x7B, 0xA2, 0x59, 0xB0,
    0x00, 0xE2, 0x05, 0xEC, 0x9E, 0x67, 0x1A, 0x2D, 0x80, 0x70, 0x01, 0xBB, 0xA7, 0x99, 0x87, 0x0B,
    0x20, 0x5A, 0xC0, 0xEE, 0x59, 0x02, 0x78, 0x01, 0xE4, 0x12, 0xD8, 0x3D, 0x47, 0x04, 0x31, 0x80,
    0x50, 0x01, 0xBB, 0xA7, 0xC8, 0x40, 0x06, 0x10, 0x29, 0x60, 0xF7, 0x0C, 0x21, 0xCC, 0x00, 0xE6,
    0x13, 0xD8, 0x7D, 0xFE, 0x18, 0x6A, 0x00, 0x73, 0x09, 0xEC, 0x3E, 0x7B, 0x10, 0x37, 0x80, 0x89,
    0x02, 0x76, 0x9F, 0x3C, 0x09, 0x1C, 0xC0, 0xDF, 0xF7, 0x1A, 0xD8, 0x7D, 0xE6, 0x30, 0x76, 0x00,
    0x9F, 0x27, 0xB0, 0xFB, 0xBC, 0x71, 0xF4, 0x00, 0x26, 0x3F, 0x39, 0xF6, 0x7C, 0x06, 0xF0, 0x0F,
    0xE8, 0xF2, 0xFF, 0x61, 0x00, 0xFF, 0x62, 0x6E, 0xFF, 0x6F, 0x03, 0xF8, 0x15, 0x6A, 0xF5, 0xFF,
    0x32, 0x80, 0xB1, 0xB7, 0xEF, 0xFD, 0x7F, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00,
    0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0,
    0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C,
    0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00,
    0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE,
    0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00,
    0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0,
    0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C,
    0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00,
    0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE,
    0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00,
    0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0,
    0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C,
    0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00,
    0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE,
    0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00,
    0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0,
    0x0C, 0x00, 0xCE, 0x00, 0xE0, 0x0C, 0x00, 0xCE, 0x00, 0xE0, 0xFE, 0x0B, 0x56, 0x62, 0x93, 0xBD,
    0x6E, 0xFF, 0xF3, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};

static ID2D1Bitmap* CreateD2DBitmapFromBytes(ID2D1RenderTarget* rt, const std::vector<uint8_t>& bytes) {
    if (!rt || bytes.empty() || !g_wicFactory) return nullptr;

    IWICStream* stream = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    ID2D1Bitmap* bitmap = nullptr;

    HRESULT hr = g_wicFactory->CreateStream(&stream);
    if (SUCCEEDED(hr)) {
        hr = stream->InitializeFromMemory(const_cast<BYTE*>(bytes.data()), static_cast<DWORD>(bytes.size()));
    }
    if (SUCCEEDED(hr)) {
        hr = g_wicFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = g_wicFactory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(
            frame,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeMedianCut);
    }
    if (SUCCEEDED(hr)) {
        hr = rt->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);
    }

    if (converter) converter->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (stream) stream->Release();

    return bitmap;
}

// ============================================================================
// Direct2D Vector Icons & Drawing Helpers
// ============================================================================
static void DrawRoundedAlbumArt(
    ID2D1RenderTarget* rt,
    ID2D1Bitmap* artBitmap,
    D2D1_RECT_F rect,
    float cornerRadius,
    float dimOverlayAlpha = 0.0f)
{
    D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(rect, cornerRadius, cornerRadius);

    ID2D1RoundedRectangleGeometry* clipGeo = nullptr;
    HRESULT hrGeo = g_d2dFactory->CreateRoundedRectangleGeometry(rrect, &clipGeo);
    if (SUCCEEDED(hrGeo) && clipGeo) {
        ID2D1Layer* layer = nullptr;
        HRESULT hrLayer = rt->CreateLayer(&layer);
        if (SUCCEEDED(hrLayer) && layer) {
            rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeo), layer);

            if (artBitmap) {
                D2D1_SIZE_F bmpSize = artBitmap->GetSize();
                float targetW = rect.right - rect.left;
                float targetH = rect.bottom - rect.top;
                float scale = (targetW / bmpSize.width > targetH / bmpSize.height)
                    ? (targetW / bmpSize.width)
                    : (targetH / bmpSize.height);
                float drawW = bmpSize.width * scale;
                float drawH = bmpSize.height * scale;
                float offsetX = rect.left + (targetW - drawW) * 0.5f;
                float offsetY = rect.top + (targetH - drawH) * 0.5f;

                rt->DrawBitmap(
                    artBitmap,
                    D2D1::RectF(offsetX, offsetY, offsetX + drawW, offsetY + drawH),
                    1.0f,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            } else {
                ID2D1SolidColorBrush* bgBrush = nullptr;
                ID2D1SolidColorBrush* noteBrush = nullptr;
                rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f), &bgBrush);
                rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.55f), &noteBrush);
                if (bgBrush) {
                    rt->FillRectangle(rect, bgBrush);
                    bgBrush->Release();
                }
                if (noteBrush) {
                    float cx = (rect.left + rect.right) * 0.5f;
                    float cy = (rect.top + rect.bottom) * 0.5f;
                    float minDim = (rect.right - rect.left < rect.bottom - rect.top)
                        ? (rect.right - rect.left)
                        : (rect.bottom - rect.top);
                    float s = minDim * 0.19f;
                    float stroke = (minDim * 0.038f > 1.8f) ? (minDim * 0.038f) : 1.8f;

                    // Beamed eighth-note logo
                    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - s * 0.55f, cy + s * 0.65f), s * 0.42f, s * 0.32f), noteBrush);
                    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + s * 0.45f, cy + s * 0.48f), s * 0.42f, s * 0.32f), noteBrush);
                    rt->DrawLine(D2D1::Point2F(cx - s * 0.18f, cy + s * 0.65f), D2D1::Point2F(cx - s * 0.18f, cy - s * 0.68f), noteBrush, stroke);
                    rt->DrawLine(D2D1::Point2F(cx + s * 0.82f, cy + s * 0.48f), D2D1::Point2F(cx + s * 0.82f, cy - s * 0.85f), noteBrush, stroke);
                    rt->DrawLine(D2D1::Point2F(cx - s * 0.22f, cy - s * 0.68f), D2D1::Point2F(cx + s * 0.86f, cy - s * 0.85f), noteBrush, stroke * 1.45f);
                    noteBrush->Release();
                }
            }

            if (dimOverlayAlpha > 0.001f) {
                ID2D1SolidColorBrush* dimBrush = nullptr;
                D2D1_COLOR_F dimColor = D2D1::ColorF(0.04f, 0.04f, 0.04f, dimOverlayAlpha);
                HRESULT hrDim = rt->CreateSolidColorBrush(dimColor, &dimBrush);
                if (SUCCEEDED(hrDim) && dimBrush) {
                    rt->FillRectangle(rect, dimBrush);
                    dimBrush->Release();
                }
            }

            rt->PopLayer();
            layer->Release();
        }
        clipGeo->Release();
    }
}

static void DrawProgressBar(
    ID2D1RenderTarget* rt,
    D2D1_RECT_F barRect,
    float progressRatio,
    D2D1_COLOR_F fillColor,
    D2D1_COLOR_F trackColor)
{
    if (progressRatio < 0.0f) progressRatio = 0.0f;
    if (progressRatio > 1.0f) progressRatio = 1.0f;

    float h = barRect.bottom - barRect.top;
    float r = h * 0.5f;

    ID2D1SolidColorBrush* trackBrush = nullptr;
    ID2D1SolidColorBrush* fillBrush = nullptr;
    rt->CreateSolidColorBrush(trackColor, &trackBrush);
    rt->CreateSolidColorBrush(fillColor, &fillBrush);

    if (trackBrush) {
        rt->FillRoundedRectangle(D2D1::RoundedRect(barRect, r, r), trackBrush);
        trackBrush->Release();
    }

    float fillW = (barRect.right - barRect.left) * progressRatio;
    if (fillBrush && fillW > 0.5f) {
        D2D1_RECT_F fillRect = D2D1::RectF(barRect.left, barRect.top, barRect.left + fillW, barRect.bottom);
        rt->FillRoundedRectangle(D2D1::RoundedRect(fillRect, r, r), fillBrush);
    }
    if (fillBrush) fillBrush->Release();
}

static void DrawTransportIcons(
    ID2D1RenderTarget* rt,
    float centerX,
    float centerY,
    float iconScale,
    bool isPlaying,
    bool isShuffle,
    int repeatMode,
    ControlAction hovered,
    D2D1_COLOR_F activeGreen,
    std::vector<HitZone>& outHitZones)
{
    const float cy = centerY;
    const float spacing = 46.0f * iconScale;
    const float stroke = 1.75f * iconScale;

    ID2D1SolidColorBrush* whiteBrush = nullptr;
    ID2D1SolidColorBrush* hoverBrush = nullptr;
    ID2D1SolidColorBrush* activeBrush = nullptr;

    rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.92f), &whiteBrush);
    rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &hoverBrush);
    rt->CreateSolidColorBrush(activeGreen, &activeBrush);

    auto getBrush = [&](ControlAction act, bool isActiveState) -> ID2D1SolidColorBrush* {
        if (isActiveState && activeBrush) return activeBrush;
        if (hovered == act && hoverBrush) return hoverBrush;
        return whiteBrush;
    };

    auto addHit = [&](float cx, float yPos, float halfSize, ControlAction act) {
        outHitZones.push_back({
            D2D1::RectF(cx - halfSize, yPos - halfSize, cx + halfSize, yPos + halfSize),
            act
        });
    };

    // 1. Shuffle
    {
        float cx = centerX - 2.0f * spacing;
        float s = 8.5f * iconScale;
        ID2D1SolidColorBrush* b = getBrush(ControlAction::Shuffle, isShuffle);
        if (b) {
            rt->DrawLine(D2D1::Point2F(cx - s, cy - s * 0.65f), D2D1::Point2F(cx - s * 0.25f, cy - s * 0.65f), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx - s * 0.25f, cy - s * 0.65f), D2D1::Point2F(cx + s * 0.35f, cy + s * 0.65f), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx + s * 0.35f, cy + s * 0.65f), D2D1::Point2F(cx + s, cy + s * 0.65f), b, stroke);

            rt->DrawLine(D2D1::Point2F(cx - s, cy + s * 0.65f), D2D1::Point2F(cx - s * 0.25f, cy + s * 0.65f), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx - s * 0.25f, cy + s * 0.65f), D2D1::Point2F(cx + s * 0.35f, cy - s * 0.65f), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx + s * 0.35f, cy - s * 0.65f), D2D1::Point2F(cx + s, cy - s * 0.65f), b, stroke);

            float a = 3.2f * iconScale;
            rt->DrawLine(D2D1::Point2F(cx + s - a, cy - s * 0.65f - a), D2D1::Point2F(cx + s, cy - s * 0.65f), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx + s - a, cy - s * 0.65f + a), D2D1::Point2F(cx + s, cy - s * 0.65f), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx + s - a, cy + s * 0.65f - a), D2D1::Point2F(cx + s, cy + s * 0.65f), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx + s - a, cy + s * 0.65f + a), D2D1::Point2F(cx + s, cy + s * 0.65f), b, stroke);
        }
        addHit(cx, cy, 18.0f * iconScale, ControlAction::Shuffle);
    }

    // 2. Previous
    {
        float cx = centerX - spacing;
        float s = 8.5f * iconScale;
        ID2D1SolidColorBrush* b = getBrush(ControlAction::Previous, false);
        if (b) {
            rt->DrawRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(cx - s, cy - s, cx - s + 2.4f * iconScale, cy + s), 1.0f, 1.0f),
                b, stroke * 0.9f);
            ID2D1PathGeometry* tri = nullptr;
            HRESULT hrTri = g_d2dFactory->CreatePathGeometry(&tri);
            if (SUCCEEDED(hrTri) && tri) {
                ID2D1GeometrySink* sink = nullptr;
                HRESULT hrSink = tri->Open(&sink);
                if (SUCCEEDED(hrSink) && sink) {
                    sink->BeginFigure(D2D1::Point2F(cx - s * 0.35f, cy), D2D1_FIGURE_BEGIN_HOLLOW);
                    sink->AddLine(D2D1::Point2F(cx + s, cy - s));
                    sink->AddLine(D2D1::Point2F(cx + s, cy + s));
                    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    sink->Close();
                    sink->Release();
                }
                rt->DrawGeometry(tri, b, stroke);
                tri->Release();
            }
        }
        addHit(cx, cy, 18.0f * iconScale, ControlAction::Previous);
    }

    // 3. Play / Pause
    {
        float cx = centerX;
        float s = 12.0f * iconScale;
        ID2D1SolidColorBrush* b = getBrush(ControlAction::PlayPause, false);
        if (b) {
            if (isPlaying) {
                float barW = 4.6f * iconScale;
                float gap = 3.2f * iconScale;
                rt->DrawRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(cx - gap - barW, cy - s, cx - gap, cy + s), 1.5f, 1.5f),
                    b, stroke);
                rt->DrawRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(cx + gap, cy - s, cx + gap + barW, cy + s), 1.5f, 1.5f),
                    b, stroke);
            } else {
                ID2D1PathGeometry* tri = nullptr;
                HRESULT hrTri = g_d2dFactory->CreatePathGeometry(&tri);
                if (SUCCEEDED(hrTri) && tri) {
                    ID2D1GeometrySink* sink = nullptr;
                    HRESULT hrSink = tri->Open(&sink);
                    if (SUCCEEDED(hrSink) && sink) {
                        sink->BeginFigure(D2D1::Point2F(cx - s * 0.65f, cy - s), D2D1_FIGURE_BEGIN_HOLLOW);
                        sink->AddLine(D2D1::Point2F(cx + s * 0.95f, cy));
                        sink->AddLine(D2D1::Point2F(cx - s * 0.65f, cy + s));
                        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                        sink->Close();
                        sink->Release();
                    }
                    rt->DrawGeometry(tri, b, stroke);
                    tri->Release();
                }
            }
        }
        addHit(cx, cy, 21.0f * iconScale, ControlAction::PlayPause);
    }

    // 4. Next
    {
        float cx = centerX + spacing;
        float s = 8.5f * iconScale;
        ID2D1SolidColorBrush* b = getBrush(ControlAction::Next, false);
        if (b) {
            rt->DrawRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(cx + s - 2.4f * iconScale, cy - s, cx + s, cy + s), 1.0f, 1.0f),
                b, stroke * 0.9f);
            ID2D1PathGeometry* tri = nullptr;
            HRESULT hrTri = g_d2dFactory->CreatePathGeometry(&tri);
            if (SUCCEEDED(hrTri) && tri) {
                ID2D1GeometrySink* sink = nullptr;
                HRESULT hrSink = tri->Open(&sink);
                if (SUCCEEDED(hrSink) && sink) {
                    sink->BeginFigure(D2D1::Point2F(cx + s * 0.35f, cy), D2D1_FIGURE_BEGIN_HOLLOW);
                    sink->AddLine(D2D1::Point2F(cx - s, cy - s));
                    sink->AddLine(D2D1::Point2F(cx - s, cy + s));
                    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    sink->Close();
                    sink->Release();
                }
                rt->DrawGeometry(tri, b, stroke);
                tri->Release();
            }
        }
        addHit(cx, cy, 18.0f * iconScale, ControlAction::Next);
    }

    // 5. Repeat
    {
        float cx = centerX + 2.0f * spacing;
        float w = 9.0f * iconScale;
        float h = 6.5f * iconScale;
        float a = 3.0f * iconScale;
        ID2D1SolidColorBrush* b = getBrush(ControlAction::Repeat, repeatMode != 0);
        if (b) {
            rt->DrawRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(cx - w, cy - h, cx + w, cy + h), 3.0f * iconScale, 3.0f * iconScale),
                b, stroke);
            rt->DrawLine(D2D1::Point2F(cx + w - a, cy - h - a), D2D1::Point2F(cx + w, cy - h), b, stroke);
            rt->DrawLine(D2D1::Point2F(cx - w + a, cy + h + a), D2D1::Point2F(cx - w, cy + h), b, stroke);
            if (repeatMode == 1) {
                rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 2.0f * iconScale, 2.0f * iconScale), b);
            }
        }
        addHit(cx, cy, 18.0f * iconScale, ControlAction::Repeat);
    }

    if (whiteBrush) whiteBrush->Release();
    if (hoverBrush) hoverBrush->Release();
    if (activeBrush) activeBrush->Release();
}

static void DrawTextAligned(
    ID2D1RenderTarget* rt,
    const std::wstring& text,
    D2D1_RECT_F rect,
    float fontSize,
    DWRITE_FONT_WEIGHT weight,
    DWRITE_TEXT_ALIGNMENT alignment,
    D2D1_COLOR_F color)
{
    if (!g_dwriteFactory || text.empty()) return;

    IDWriteTextFormat* fmt = nullptr;
    HRESULT hrFmt = g_dwriteFactory->CreateTextFormat(
        L"Segoe UI", nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        fontSize, L"en-US", &fmt);
    if (SUCCEEDED(hrFmt) && fmt) {
        fmt->SetTextAlignment(alignment);
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

        DWRITE_TRIMMING trim = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        IDWriteInlineObject* ellipsis = nullptr;
        HRESULT hrEllip = g_dwriteFactory->CreateEllipsisTrimmingSign(fmt, &ellipsis);
        if (SUCCEEDED(hrEllip) && ellipsis) {
            fmt->SetTrimming(&trim, ellipsis);
            ellipsis->Release();
        }

        ID2D1SolidColorBrush* brush = nullptr;
        HRESULT hrBrush = rt->CreateSolidColorBrush(color, &brush);
        if (SUCCEEDED(hrBrush) && brush) {
            rt->DrawText(text.c_str(), static_cast<UINT32>(text.size()), fmt, rect, brush);
            brush->Release();
        }
        fmt->Release();
    }
}

// ============================================================================
// Theme Renderers
// ============================================================================
static void RenderTheme_Standard(
    WidgetWindowState& ws,
    const WidgetConfig& cfg,
    const GlobalSettings& gs,
    const MediaState& media,
    int64_t currentPosMs)
{
    ID2D1RenderTarget* rt = ws.renderTarget;
    const float H = 106.0f;

    const float pad = 12.0f;
    const float artSize = H - pad * 2.0f;
    DrawRoundedAlbumArt(rt, ws.cachedArtBitmap, D2D1::RectF(pad, pad, pad + artSize, pad + artSize), 8.0f);

    const float textLeft = pad + artSize + 16.0f;
    const float textRight = 252.0f;
    if (media.artist.empty()) {
        DrawTextAligned(rt, media.title, D2D1::RectF(textLeft, 36.0f, textRight, 68.0f),
                        18.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f));
    } else {
        DrawTextAligned(rt, media.title, D2D1::RectF(textLeft, 24.0f, textRight, 52.0f),
                        18.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f));
        DrawTextAligned(rt, media.artist, D2D1::RectF(textLeft, 53.0f, textRight, 78.0f),
                        14.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.68f));
    }

    const float rightCenterX = 415.0f;
    const float ratio = (media.durationMs > 0) ? (static_cast<float>(currentPosMs) / static_cast<float>(media.durationMs)) : 0.0f;

    if (!cfg.disableControls) {
        DrawTransportIcons(
            rt, rightCenterX, 42.0f, 1.0f,
            media.isPlaying, media.isShuffleActive, media.repeatMode,
            ws.hoveredAction, gs.progressFill, ws.hitZones);
    }

    const float barY = cfg.disableControls ? (H * 0.5f) : 77.0f;
    const float barHalfW = 108.0f;
    D2D1_RECT_F barRect = D2D1::RectF(rightCenterX - barHalfW, barY - 2.2f, rightCenterX + barHalfW, barY + 2.2f);
    DrawProgressBar(rt, barRect, ratio, gs.progressFill, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.35f));

    if (!cfg.disableControls) {
        ws.hasSeekBar = true;
        ws.seekBarRect = D2D1::RectF(barRect.left, barRect.top - 8.0f, barRect.right, barRect.bottom + 8.0f);
    }

    std::wstring elapsedStr = FormatTimeMs(currentPosMs);
    std::wstring totalStr = FormatTimeMs(media.durationMs);
    DrawTextAligned(rt, elapsedStr, D2D1::RectF(barRect.left - 54.0f, barY - 10.0f, barRect.left - 8.0f, barY + 10.0f),
                    12.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_TRAILING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.90f));
    DrawTextAligned(rt, totalStr, D2D1::RectF(barRect.right + 8.0f, barY - 10.0f, barRect.right + 54.0f, barY + 10.0f),
                    12.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.90f));
}

static void RenderTheme_Album(
    WidgetWindowState& ws,
    const WidgetConfig& cfg,
    const GlobalSettings& gs,
    const MediaState& media,
    int64_t currentPosMs)
{
    ID2D1RenderTarget* rt = ws.renderTarget;
    const float W = 260.0f;
    const float H = 260.0f;

    DrawRoundedAlbumArt(rt, ws.cachedArtBitmap, D2D1::RectF(0.0f, 0.0f, W, H), 12.0f, 0.44f);

    ID2D1SolidColorBrush* borderBrush = nullptr;
    D2D1_COLOR_F borderColor = D2D1::ColorF(0.14f, 0.14f, 0.14f, 0.85f);
    HRESULT hrBorder = rt->CreateSolidColorBrush(borderColor, &borderBrush);
    if (SUCCEEDED(hrBorder) && borderBrush) {
        rt->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(1.5f, 1.5f, W - 1.5f, H - 1.5f), 11.0f, 11.0f), borderBrush, 3.0f);
        borderBrush->Release();
    }

    if (media.artist.empty()) {
        DrawTextAligned(rt, media.title, D2D1::RectF(16.0f, 180.0f, W - 16.0f, 212.0f),
                        18.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f));
    } else {
        DrawTextAligned(rt, media.title, D2D1::RectF(16.0f, 168.0f, W - 16.0f, 198.0f),
                        18.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f));
        DrawTextAligned(rt, media.artist, D2D1::RectF(16.0f, 196.0f, W - 16.0f, 222.0f),
                        15.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.72f));
    }

    std::wstring elapsedStr = FormatTimeMs(currentPosMs);
    std::wstring totalStr = FormatTimeMs(media.durationMs);
    DrawTextAligned(rt, elapsedStr, D2D1::RectF(14.0f, 232.0f, 80.0f, 250.0f),
                    12.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.92f));
    DrawTextAligned(rt, totalStr, D2D1::RectF(W - 80.0f, 232.0f, W - 14.0f, 250.0f),
                    12.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_TRAILING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.92f));

    const float ratio = (media.durationMs > 0) ? (static_cast<float>(currentPosMs) / static_cast<float>(media.durationMs)) : 0.0f;
    D2D1_RECT_F barRect = D2D1::RectF(4.0f, H - 5.5f, W - 4.0f, H - 2.0f);
    DrawProgressBar(rt, barRect, ratio, gs.progressFill, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.15f));

    if (!cfg.disableControls) {
        ws.hasSeekBar = true;
        ws.seekBarRect = D2D1::RectF(barRect.left, barRect.top - 10.0f, barRect.right, barRect.bottom + 4.0f);
    }
}

static void RenderTheme_Cardstock(
    WidgetWindowState& ws,
    const WidgetConfig& cfg,
    const GlobalSettings& gs,
    const MediaState& media,
    int64_t currentPosMs)
{
    ID2D1RenderTarget* rt = ws.renderTarget;
    const float W = 260.0f;

    const float pad = 18.0f;
    const float artSize = W - pad * 2.0f;
    DrawRoundedAlbumArt(rt, ws.cachedArtBitmap, D2D1::RectF(pad, pad, pad + artSize, pad + artSize), 8.0f, 0.0f);

    const float titleTop = pad + artSize + 18.0f;
    if (media.artist.empty()) {
        DrawTextAligned(rt, media.title, D2D1::RectF(16.0f, titleTop + 10.0f, W - 16.0f, titleTop + 38.0f),
                        17.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f));
    } else {
        DrawTextAligned(rt, media.title, D2D1::RectF(16.0f, titleTop, W - 16.0f, titleTop + 26.0f),
                        17.5f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f));
        DrawTextAligned(rt, media.artist, D2D1::RectF(16.0f, titleTop + 24.0f, W - 16.0f, titleTop + 46.0f),
                        14.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.72f));
    }

    const float barY = titleTop + 56.0f;
    const float ratio = (media.durationMs > 0) ? (static_cast<float>(currentPosMs) / static_cast<float>(media.durationMs)) : 0.0f;
    D2D1_RECT_F barRect = D2D1::RectF(pad, barY - 2.0f, W - pad, barY + 2.0f);
    DrawProgressBar(rt, barRect, ratio, gs.progressFill, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.35f));

    if (!cfg.disableControls) {
        ws.hasSeekBar = true;
        ws.seekBarRect = D2D1::RectF(barRect.left, barRect.top - 8.0f, barRect.right, barRect.bottom + 8.0f);
    }

    std::wstring elapsedStr = FormatTimeMs(currentPosMs);
    std::wstring totalStr = FormatTimeMs(media.durationMs);
    DrawTextAligned(rt, elapsedStr, D2D1::RectF(pad + 2.0f, barY + 5.0f, pad + 65.0f, barY + 22.0f),
                    12.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.90f));
    DrawTextAligned(rt, totalStr, D2D1::RectF(W - pad - 65.0f, barY + 5.0f, W - pad - 2.0f, barY + 22.0f),
                    12.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_ALIGNMENT_TRAILING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.90f));

    if (!cfg.disableControls) {
        DrawTransportIcons(
            rt, W * 0.5f, 362.0f, 0.88f,
            media.isPlaying, media.isShuffleActive, media.repeatMode,
            ws.hoveredAction, gs.progressFill, ws.hitZones);
    }
}

static void RenderTheme_Simplified(
    WidgetWindowState& ws,
    const WidgetConfig& cfg,
    const GlobalSettings& gs,
    const MediaState& media,
    int64_t currentPosMs)
{
    ID2D1RenderTarget* rt = ws.renderTarget;
    const float W = 660.0f;
    const float H = 74.0f;

    const float pad = 11.0f;
    const float artSize = H - pad * 2.0f;
    DrawRoundedAlbumArt(rt, ws.cachedArtBitmap, D2D1::RectF(pad, pad, pad + artSize, pad + artSize), 6.0f);

    const float textLeft = pad + artSize + 14.0f;
    const float textRight = 232.0f;
    if (media.artist.empty()) {
        DrawTextAligned(rt, media.title, D2D1::RectF(textLeft, 24.0f, textRight, 50.0f),
                        15.5f, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.94f));
    } else {
        DrawTextAligned(rt, media.title, D2D1::RectF(textLeft, 14.0f, textRight, 38.0f),
                        15.5f, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.94f));
        DrawTextAligned(rt, media.artist, D2D1::RectF(textLeft, 37.0f, textRight, 60.0f),
                        14.5f, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.65f));
    }

    if (!cfg.disableControls) {
        DrawTransportIcons(
            rt, 315.0f, H * 0.5f, 0.85f,
            media.isPlaying, media.isShuffleActive, media.repeatMode,
            ws.hoveredAction, gs.progressFill, ws.hitZones);
    }

    const float ratio = (media.durationMs > 0) ? (static_cast<float>(currentPosMs) / static_cast<float>(media.durationMs)) : 0.0f;
    const float barLeft = cfg.disableControls ? 255.0f : 430.0f;
    const float barRight = W - 26.0f;
    D2D1_RECT_F barRect = D2D1::RectF(barLeft, H * 0.5f - 1.5f, barRight, H * 0.5f + 1.5f);
    DrawProgressBar(rt, barRect, ratio, gs.progressFill, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.72f));

    if (!cfg.disableControls) {
        ws.hasSeekBar = true;
        ws.seekBarRect = D2D1::RectF(barRect.left, barRect.top - 10.0f, barRect.right, barRect.bottom + 10.0f);
    }
}

// ============================================================================
// Layered WIC + Direct2D Composition Renderer
// ============================================================================
static void ReleaseCanvasResources(WidgetWindowState& ws) {
    if (ws.cachedArtBitmap) {
        ws.cachedArtBitmap->Release();
        ws.cachedArtBitmap = nullptr;
    }
    ws.artCacheValid = false;
    if (ws.renderTarget) {
        ws.renderTarget->Release();
        ws.renderTarget = nullptr;
    }
    if (ws.wicCanvas) {
        ws.wicCanvas->Release();
        ws.wicCanvas = nullptr;
    }
    ws.canvasW = 0;
    ws.canvasH = 0;
}

static bool EnsureWicRenderTarget(WidgetWindowState& ws, UINT pxW, UINT pxH) {
    if (!g_wicFactory || !g_d2dFactory) return false;

    if (ws.renderTarget && ws.wicCanvas && ws.canvasW == pxW && ws.canvasH == pxH) {
        return true;
    }

    ReleaseCanvasResources(ws);

    HRESULT hr = g_wicFactory->CreateBitmap(
        pxW, pxH,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapCacheOnDemand,
        &ws.wicCanvas);
    if (FAILED(hr) || !ws.wicCanvas) return false;

    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f);

    hr = g_d2dFactory->CreateWicBitmapRenderTarget(ws.wicCanvas, rtProps, &ws.renderTarget);
    if (FAILED(hr) || !ws.renderTarget) {
        ReleaseCanvasResources(ws);
        return false;
    }

    ws.canvasW = pxW;
    ws.canvasH = pxH;
    return true;
}

static void CommitWicCanvasToLayeredWindow(HWND hwnd, IWICBitmap* wicBitmap, UINT pxW, UINT pxH) {
    if (!hwnd || !wicBitmap) return;

    WICRect lockRect = {0, 0, static_cast<INT>(pxW), static_cast<INT>(pxH)};
    IWICBitmapLock* pLock = nullptr;
    if (FAILED(wicBitmap->Lock(&lockRect, WICBitmapLockRead, &pLock)) || !pLock) return;

    UINT cbStride = 0;
    UINT cbBufferSize = 0;
    BYTE* pvPixels = nullptr;
    pLock->GetStride(&cbStride);
    pLock->GetDataPointer(&cbBufferSize, &pvPixels);

    if (pvPixels && cbBufferSize > 0) {
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = static_cast<LONG>(pxW);
        bmi.bmiHeader.biHeight = -static_cast<LONG>(pxH); // Top-down DIB
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        HDC hdcScreen = GetDC(nullptr);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        void* dibBits = nullptr;
        HBITMAP hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &dibBits, nullptr, 0);

        if (hBitmap && dibBits) {
            UINT dstStride = pxW * 4;
            for (UINT y = 0; y < pxH; y++) {
                memcpy(
                    static_cast<BYTE*>(dibBits) + y * dstStride,
                    pvPixels + y * cbStride,
                    dstStride);
            }

            HGDIOBJ oldObj = SelectObject(hdcMem, hBitmap);
            POINT ptSrc = {0, 0};
            SIZE sizeWnd = {static_cast<LONG>(pxW), static_cast<LONG>(pxH)};
            BLENDFUNCTION bf = {};
            bf.BlendOp = AC_SRC_OVER;
            bf.SourceConstantAlpha = 255;
            bf.AlphaFormat = AC_SRC_ALPHA;

            UpdateLayeredWindow(hwnd, hdcScreen, nullptr, &sizeWnd, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);

            SelectObject(hdcMem, oldObj);
            DeleteObject(hBitmap);
        }

        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
    }

    pLock->Release();
}

static void RenderWidget(WidgetWindowState& ws) {
    if (!ws.hwnd || !g_d2dFactory) return;

    GlobalSettings gs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        gs = g_settings;
    }
    const WidgetConfig& cfg = gs.widgets[ws.index];

    RECT rcWin;
    GetWindowRect(ws.hwnd, &rcWin);
    UINT pxW = static_cast<UINT>((rcWin.right - rcWin.left > 1) ? (rcWin.right - rcWin.left) : 1);
    UINT pxH = static_cast<UINT>((rcWin.bottom - rcWin.top > 1) ? (rcWin.bottom - rcWin.top) : 1);

    if (!EnsureWicRenderTarget(ws, pxW, pxH)) return;

    MediaState mediaSnapshot;
    std::vector<uint8_t> newArtBytes;
    bool needArtUpdate = false;
    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        mediaSnapshot.title = g_media.title;
        mediaSnapshot.artist = g_media.artist;
        mediaSnapshot.isPlaying = g_media.isPlaying;
        mediaSnapshot.isShuffleActive = g_media.isShuffleActive;
        mediaSnapshot.repeatMode = g_media.repeatMode;
        mediaSnapshot.positionMs = g_media.positionMs;
        mediaSnapshot.durationMs = g_media.durationMs;
        mediaSnapshot.lastTimelineSync = g_media.lastTimelineSync;
        mediaSnapshot.artVersion = g_media.artVersion;
        if (!ws.artCacheValid || ws.cachedArtVersion != g_media.artVersion) {
            needArtUpdate = true;
            newArtBytes = g_media.artBytes;
        }
    }

    if (needArtUpdate) {
        if (ws.cachedArtBitmap) {
            ws.cachedArtBitmap->Release();
            ws.cachedArtBitmap = nullptr;
        }
        ws.cachedArtBitmap = CreateD2DBitmapFromBytes(ws.renderTarget, newArtBytes);
        if (!ws.cachedArtBitmap) {
            if (!newArtBytes.empty()) {
                Wh_Log(L"Failed to decode SMTC album art; using default image");
            }
            ws.cachedArtBitmap = CreateD2DBitmapFromBytes(ws.renderTarget, kDefaultArtBytes);
            if (!ws.cachedArtBitmap) {
                Wh_Log(L"Failed to decode default album art");
            }
        }
        ws.cachedArtVersion = mediaSnapshot.artVersion;
        ws.artCacheValid = true;
    }

    int64_t currentPosMs = mediaSnapshot.positionMs;
    if (mediaSnapshot.isPlaying && mediaSnapshot.durationMs > 0) {
        auto now = std::chrono::steady_clock::now();
        int64_t deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - mediaSnapshot.lastTimelineSync).count();
        currentPosMs += deltaMs;
        if (currentPosMs > mediaSnapshot.durationMs) currentPosMs = mediaSnapshot.durationMs;
    }

    D2D1_SIZE_F logicalSize = GetLogicalBaseSize(cfg);
    float scaleX = static_cast<float>(pxW) / logicalSize.width;
    float scaleY = static_cast<float>(pxH) / logicalSize.height;

    ws.hitZones.clear();
    ws.hasSeekBar = false;

    ws.renderTarget->BeginDraw();
    ws.renderTarget->SetTransform(D2D1::Matrix3x2F::Scale(scaleX, scaleY));
    if (cfg.theme == WidgetTheme::Album) {
        ws.renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    } else {
        ws.renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.01f));
    }

    switch (cfg.theme) {
        case WidgetTheme::Standard:
            RenderTheme_Standard(ws, cfg, gs, mediaSnapshot, currentPosMs);
            break;
        case WidgetTheme::Album:
            RenderTheme_Album(ws, cfg, gs, mediaSnapshot, currentPosMs);
            break;
        case WidgetTheme::Cardstock:
            RenderTheme_Cardstock(ws, cfg, gs, mediaSnapshot, currentPosMs);
            break;
        case WidgetTheme::Simplified:
            RenderTheme_Simplified(ws, cfg, gs, mediaSnapshot, currentPosMs);
            break;
    }

    if (cfg.theme != WidgetTheme::Album) {
        ID2D1SolidColorBrush* edgeBrush = nullptr;
        D2D1_COLOR_F edgeColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f);
        HRESULT hrEdge = ws.renderTarget->CreateSolidColorBrush(edgeColor, &edgeBrush);
        if (SUCCEEDED(hrEdge) && edgeBrush) {
            ws.renderTarget->DrawRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(0.5f, 0.5f, logicalSize.width - 0.5f, logicalSize.height - 0.5f), 8.0f, 8.0f),
                edgeBrush, 1.0f);
            edgeBrush->Release();
        }
    }

    HRESULT hrEnd = ws.renderTarget->EndDraw();
    if (SUCCEEDED(hrEnd)) {
        CommitWicCanvasToLayeredWindow(ws.hwnd, ws.wicCanvas, pxW, pxH);
    } else if (hrEnd == D2DERR_RECREATE_TARGET) {
        ReleaseCanvasResources(ws);
    }
}

// ============================================================================
// Hit Testing & SMTC Command Dispatch
// ============================================================================
static void EnqueueSmtcCommand(PendingCommandType type, double seekRatio = 0.0) {
    {
        std::lock_guard<std::mutex> lock(g_cmdMutex);
        g_cmdQueue.push_back({type, seekRatio});
    }
    if (g_cmdEvent) SetEvent(g_cmdEvent);
}

static ControlAction HitTestWidget(WidgetWindowState& ws, int pxX, int pxY, double& outSeekRatio) {
    outSeekRatio = 0.0;
    if (!ws.hwnd) return ControlAction::None;

    WidgetConfig cfg;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        cfg = g_settings.widgets[ws.index];
    }
    if (cfg.disableControls) return ControlAction::None;

    RECT rc;
    GetWindowRect(ws.hwnd, &rc);
    float pxW = static_cast<float>((rc.right - rc.left > 1) ? (rc.right - rc.left) : 1);
    float pxH = static_cast<float>((rc.bottom - rc.top > 1) ? (rc.bottom - rc.top) : 1);
    D2D1_SIZE_F logical = GetLogicalBaseSize(cfg);

    float lx = (static_cast<float>(pxX) / pxW) * logical.width;
    float ly = (static_cast<float>(pxY) / pxH) * logical.height;

    for (const auto& hz : ws.hitZones) {
        if (lx >= hz.rect.left && lx <= hz.rect.right && ly >= hz.rect.top && ly <= hz.rect.bottom) {
            return hz.action;
        }
    }

    if (ws.hasSeekBar &&
        lx >= ws.seekBarRect.left && lx <= ws.seekBarRect.right &&
        ly >= ws.seekBarRect.top && ly <= ws.seekBarRect.bottom) {
        float w = ws.seekBarRect.right - ws.seekBarRect.left;
        if (w > 1.0f) {
            outSeekRatio = (lx - ws.seekBarRect.left) / w;
            if (outSeekRatio < 0.0) outSeekRatio = 0.0;
            if (outSeekRatio > 1.0) outSeekRatio = 1.0;
            return ControlAction::Seek;
        }
    }

    return ControlAction::None;
}

// ============================================================================
// Widget Window Procedure & Layout Sync
// ============================================================================
static LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WidgetWindowState* pWs = reinterpret_cast<WidgetWindowState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_NCCREATE: {
            CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_MOUSEMOVE: {
            if (!pWs) break;
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            if (pWs->isDragging) {
                POINT curScreen;
                GetCursorPos(&curScreen);
                int newX = pWs->dragStartWinPos.x + (curScreen.x - pWs->dragStartCursor.x);
                int newY = pWs->dragStartWinPos.y + (curScreen.y - pWs->dragStartCursor.y);
                SetWindowPos(hwnd, nullptr, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                return 0;
            }

            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);

            double dummyRatio = 0.0;
            ControlAction act = HitTestWidget(*pWs, x, y, dummyRatio);
            if (act != pWs->hoveredAction) {
                pWs->hoveredAction = act;
                RenderWidget(*pWs);
            }
            SetCursor(LoadCursorW(nullptr, (act != ControlAction::None) ? IDC_HAND : IDC_ARROW));
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (pWs && pWs->hoveredAction != ControlAction::None) {
                pWs->hoveredAction = ControlAction::None;
                RenderWidget(*pWs);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (!pWs) break;
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            double seekRatio = 0.0;
            ControlAction act = HitTestWidget(*pWs, x, y, seekRatio);
            if (act != ControlAction::None) {
                switch (act) {
                    case ControlAction::Shuffle:
                        EnqueueSmtcCommand(PendingCommandType::ToggleShuffle);
                        break;
                    case ControlAction::Previous:
                        EnqueueSmtcCommand(PendingCommandType::Previous);
                        break;
                    case ControlAction::PlayPause:
                        EnqueueSmtcCommand(PendingCommandType::TogglePlayPause);
                        break;
                    case ControlAction::Next:
                        EnqueueSmtcCommand(PendingCommandType::Next);
                        break;
                    case ControlAction::Repeat:
                        EnqueueSmtcCommand(PendingCommandType::CycleRepeat);
                        break;
                    case ControlAction::Seek:
                        EnqueueSmtcCommand(PendingCommandType::SeekToRatio, seekRatio);
                        break;
                    default:
                        break;
                }
                return 0;
            }

            bool locked = false;
            {
                std::lock_guard<std::mutex> lock(g_settingsMutex);
                locked = g_settings.widgets[pWs->index].lockPosition;
            }

            if (!locked) {
                pWs->isDragging = true;
                GetCursorPos(&pWs->dragStartCursor);
                RECT winRc;
                GetWindowRect(hwnd, &winRc);
                pWs->dragStartWinPos = {winRc.left, winRc.top};
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (pWs && pWs->isDragging) {
                ReleaseCapture(); // WM_CAPTURECHANGED clears isDragging and saves position.
            }
            return 0;
        }

        case WM_CAPTURECHANGED: {
            if (pWs && pWs->isDragging) {
                pWs->isDragging = false;
                RECT winRc;
                GetWindowRect(hwnd, &winRc);
                SaveWidgetPos(pWs->index, winRc.left, winRc.top);

                GlobalSettings gs;
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    gs = g_settings;
                }
                const WidgetConfig& cfg = gs.widgets[pWs->index];
                D2D1_SIZE_F logical = GetLogicalBaseSize(cfg);
                float totalScale = GetWindowDpiScale(hwnd) * (static_cast<float>(cfg.scalePercent) / 100.0f);
                int newW = static_cast<int>(logical.width * totalScale + 0.5f);
                int newH = static_cast<int>(logical.height * totalScale + 0.5f);
                SetWindowPos(hwnd, nullptr, 0, 0, newW, newH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                RenderWidget(*pWs);
            }
            return 0;
        }

        case WM_DPICHANGED: {
            RECT* suggested = reinterpret_cast<RECT*>(lParam);
            if (suggested && pWs) {
                SetWindowPos(
                    hwnd, nullptr,
                    suggested->left, suggested->top,
                    suggested->right - suggested->left,
                    suggested->bottom - suggested->top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
                RenderWidget(*pWs);
            }
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void SyncWidgetWindowsWithSettings() {
    GlobalSettings gs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        gs = g_settings;
    }

    for (int i = 0; i < 5; i++) {
        WidgetWindowState& ws = g_widgetWindows[i];
        ws.index = i;

        if (i >= gs.widgetCount) {
            if (ws.hwnd) {
                ReleaseCanvasResources(ws);
                DestroyWindow(ws.hwnd);
                ws.hwnd = nullptr;
            }
            continue;
        }

        const WidgetConfig& cfg = gs.widgets[i];
        bool justCreated = false;

        if (!ws.hwnd) {
            POINT pt = LoadSavedWidgetPos(i);
            DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE;
            if (gs.alwaysOnTop) exStyle |= WS_EX_TOPMOST;

            ws.hwnd = CreateWindowExW(
                exStyle,
                kWindowClassName,
                L"SMTC Now Playing Widget",
                WS_POPUP,
                pt.x, pt.y, 200, 100,
                nullptr, nullptr, g_hInstance, &ws);
            justCreated = true;
        }

        if (ws.hwnd) {
            D2D1_SIZE_F logical = GetLogicalBaseSize(cfg);
            float totalScale = GetWindowDpiScale(ws.hwnd) * (static_cast<float>(cfg.scalePercent) / 100.0f);
            int pxW = static_cast<int>(logical.width * totalScale + 0.5f);
            int pxH = static_cast<int>(logical.height * totalScale + 0.5f);

            HWND insertAfter = gs.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST;
            SetWindowPos(ws.hwnd, insertAfter, 0, 0, pxW, pxH, SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            ApplyWindowBackdrop(ws.hwnd, cfg, gs);

            if (justCreated) {
                ShowWindow(ws.hwnd, SW_SHOWNOACTIVATE);
            }
            RenderWidget(ws);
        }
    }
}

static LRESULT CALLBACK HiddenControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_RELOAD_SETTINGS:
            LoadAllSettings();
            SyncWidgetWindowsWithSettings();
            return 0;

        case WM_APP_MEDIA_UPDATED:
            for (int i = 0; i < 5; i++) {
                if (g_widgetWindows[i].hwnd) {
                    RenderWidget(g_widgetWindows[i]);
                }
            }
            return 0;

        case WM_TIMER:
            if (wParam == TIMER_INTERPOLATE_ID) {
                bool playing = false;
                {
                    std::lock_guard<std::mutex> lock(g_mediaMutex);
                    playing = g_media.isPlaying;
                }
                if (playing) {
                    for (int i = 0; i < 5; i++) {
                        if (g_widgetWindows[i].hwnd) {
                            RenderWidget(g_widgetWindows[i]);
                        }
                    }
                }
            }
            return 0;

        case WM_APP_DESTROY_UI:
            KillTimer(hwnd, TIMER_INTERPOLATE_ID);
            for (int i = 0; i < 5; i++) {
                ReleaseCanvasResources(g_widgetWindows[i]);
                if (g_widgetWindows[i].hwnd) {
                    DestroyWindow(g_widgetWindows[i].hwnd);
                    g_widgetWindows[i].hwnd = nullptr;
                }
            }
            g_uiThreadHwnd.store(nullptr);
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI UiThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_dwriteFactory));
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_wicFactory));

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WidgetWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = kWindowClassName;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    ATOM atomWidget = RegisterClassExW(&wc);

    WNDCLASSEXW ctrlWc = {};
    ctrlWc.cbSize = sizeof(ctrlWc);
    ctrlWc.lpfnWndProc = HiddenControllerWndProc;
    ctrlWc.hInstance = g_hInstance;
    ctrlWc.lpszClassName = kControllerClassName;
    ATOM atomCtrl = RegisterClassExW(&ctrlWc);

    HWND ctrlHwnd = nullptr;
    if (atomWidget && atomCtrl && g_d2dFactory && g_dwriteFactory && g_wicFactory) {
        ctrlHwnd = CreateWindowExW(
            0, kControllerClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, g_hInstance, nullptr);
    }

    if (!ctrlHwnd) {
        Wh_Log(L"Failed to initialize UI thread window/factories");
        if (atomWidget) UnregisterClassW(kWindowClassName, g_hInstance);
        if (atomCtrl) UnregisterClassW(kControllerClassName, g_hInstance);
        if (g_wicFactory) { g_wicFactory->Release(); g_wicFactory = nullptr; }
        if (g_dwriteFactory) { g_dwriteFactory->Release(); g_dwriteFactory = nullptr; }
        if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = nullptr; }
        if (SUCCEEDED(hrCom)) CoUninitialize();
        if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
        return 1;
    }

    g_uiThreadHwnd.store(ctrlHwnd);
    if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);

    LoadAllSettings();
    SyncWidgetWindowsWithSettings();
    SetTimer(ctrlHwnd, TIMER_INTERPOLATE_ID, 250, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_uiThreadHwnd.store(nullptr);
    UnregisterClassW(kWindowClassName, g_hInstance);
    UnregisterClassW(kControllerClassName, g_hInstance);

    if (g_wicFactory) { g_wicFactory->Release(); g_wicFactory = nullptr; }
    if (g_dwriteFactory) { g_dwriteFactory->Release(); g_dwriteFactory = nullptr; }
    if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = nullptr; }

    if (SUCCEEDED(hrCom)) CoUninitialize();
    return 0;
}

// ============================================================================
// WinRT SMTC Listener & Control Thread (MTA, zero-capture callbacks)
// ============================================================================
static void NotifyUiMediaUpdated() {
    HWND hwnd = g_uiThreadHwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_MEDIA_UPDATED, 0, 0);
    }
}

static void ClearMediaState() {
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        if (g_media.title != L"No Data" || !g_media.artist.empty() || g_media.isPlaying || !g_media.artBytes.empty()) {
            g_media.isPlaying = false;
            g_media.isShuffleActive = false;
            g_media.repeatMode = 0;
            g_media.title = L"No Data";
            g_media.artist = L"";
            g_media.positionMs = 0;
            g_media.durationMs = 0;
            g_media.artBytes.clear();
            g_media.artVersion++;
            changed = true;
        }
    }
    if (changed) {
        NotifyUiMediaUpdated();
    }
}

static void RefreshFromSession(
    const GlobalSystemMediaTransportControlsSession& session,
    std::wstring& lastArtKey)
{
    if (!session) {
        lastArtKey.clear();
        ClearMediaState();
        return;
    }

    try {
        auto pb = session.GetPlaybackInfo();
        bool playing = false;
        bool shuffle = false;
        int rep = 0;

        if (pb) {
            playing = (pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            shuffle = pb.IsShuffleActive() ? pb.IsShuffleActive().Value() : false;
            if (pb.AutoRepeatMode()) {
                auto mode = pb.AutoRepeatMode().Value();
                if (mode == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track) rep = 1;
                else if (mode == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List) rep = 2;
            }
        }

        auto tl = session.GetTimelineProperties();
        int64_t dur = 0;
        int64_t relPos = 0;
        if (tl) {
            int64_t startMs = std::chrono::duration_cast<std::chrono::milliseconds>(tl.StartTime()).count();
            int64_t endMs = std::chrono::duration_cast<std::chrono::milliseconds>(tl.EndTime()).count();
            int64_t posMs = std::chrono::duration_cast<std::chrono::milliseconds>(tl.Position()).count();

            dur = (endMs - startMs > 0) ? (endMs - startMs) : 0;
            relPos = (posMs - startMs > 0) ? (posMs - startMs) : 0;

            auto lastUpdated = tl.LastUpdatedTime();
            if (playing && lastUpdated.time_since_epoch().count() > 0) {
                auto elapsedSinceUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                    winrt::clock::now() - lastUpdated).count();
                if (elapsedSinceUpdate > 0) {
                    relPos += elapsedSinceUpdate;
                    if (dur > 0 && relPos > dur) relPos = dur;
                }
            }
        }

        auto props = session.TryGetMediaPropertiesAsync().get();
        std::wstring newTitle = L"No Data";
        std::wstring newArtist;
        bool hasThumb = false;
        if (props) {
            newTitle = props.Title().c_str();
            newArtist = props.Artist().c_str();
            if (newTitle.empty()) newTitle = L"No Data";
            hasThumb = (props.Thumbnail() != nullptr);
        }

        std::wstring artKey = newTitle + L"\n" + newArtist + L"\n" + std::to_wstring(dur) + (hasThumb ? L"\n1" : L"\n0");
        bool artStateChanged = (artKey != lastArtKey);
        std::vector<uint8_t> newArt;

        if (artStateChanged) {
            lastArtKey = artKey;
            if (hasThumb && props) {
                try {
                    auto thumbRef = props.Thumbnail();
                    if (thumbRef) {
                        auto stream = thumbRef.OpenReadAsync().get();
                        if (stream) {
                            uint64_t size = stream.Size();
                            if (size > 0 && size < 15 * 1024 * 1024) {
                                DataReader reader(stream.GetInputStreamAt(0));
                                reader.LoadAsync(static_cast<uint32_t>(size)).get();
                                newArt.resize(static_cast<size_t>(size));
                                reader.ReadBytes(newArt);
                                reader.Close();
                            }
                            stream.Close();
                        }
                    }
                } catch (const winrt::hresult_error& ex) {
                    Wh_Log(L"SMTC thumbnail read error: 0x%08X", ex.code().value);
                } catch (...) {}
            }
        }

        bool uiChanged = false;
        {
            std::lock_guard<std::mutex> lock(g_mediaMutex);
            auto now = std::chrono::steady_clock::now();
            int64_t prevExpectedPos = g_media.positionMs;
            if (g_media.isPlaying && g_media.durationMs > 0) {
                prevExpectedPos += std::chrono::duration_cast<std::chrono::milliseconds>(now - g_media.lastTimelineSync).count();
                if (prevExpectedPos > g_media.durationMs) prevExpectedPos = g_media.durationMs;
            }

            if (g_media.title != newTitle ||
                g_media.artist != newArtist ||
                g_media.isPlaying != playing ||
                g_media.isShuffleActive != shuffle ||
                g_media.repeatMode != rep ||
                g_media.durationMs != dur ||
                std::llabs(relPos - prevExpectedPos) > 750 ||
                artStateChanged) {
                uiChanged = true;
            }

            g_media.title = std::move(newTitle);
            g_media.artist = std::move(newArtist);
            g_media.isPlaying = playing;
            g_media.isShuffleActive = shuffle;
            g_media.repeatMode = rep;
            g_media.positionMs = relPos;
            g_media.durationMs = dur;
            g_media.lastTimelineSync = now;
            if (artStateChanged) {
                g_media.artBytes = std::move(newArt);
                g_media.artVersion++;
            }
        }
        if (uiChanged) {
            NotifyUiMediaUpdated();
        }
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"SMTC refresh error: 0x%08X (%s)", ex.code().value, ex.message().c_str());
    } catch (...) {
        Wh_Log(L"SMTC refresh unknown error");
    }
}

static GlobalSystemMediaTransportControlsSession ResolveBestSession(
    const GlobalSystemMediaTransportControlsSessionManager& mgr)
{
    if (!mgr) return nullptr;
    auto cur = mgr.GetCurrentSession();
    if (cur) return cur;

    auto sessions = mgr.GetSessions();
    if (sessions && sessions.Size() > 0) {
        return sessions.GetAt(0);
    }
    return nullptr;
}

static void SignalSmtcRefresh() {
    if (g_smtcRefreshEvent) {
        SetEvent(g_smtcRefreshEvent);
    }
}

static DWORD WINAPI SmtcWorkerProc(LPVOID) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    try {
        auto mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();

        GlobalSystemMediaTransportControlsSession activeSession{nullptr};
        winrt::event_token tokMedia{};
        winrt::event_token tokPlayback{};
        winrt::event_token tokTimeline{};
        std::wstring lastArtKey;

        auto bindSession = [&](const GlobalSystemMediaTransportControlsSession& newSess) {
            if (activeSession == newSess) return;
            if (activeSession) {
                try {
                    activeSession.MediaPropertiesChanged(tokMedia);
                    activeSession.PlaybackInfoChanged(tokPlayback);
                    activeSession.TimelinePropertiesChanged(tokTimeline);
                } catch (...) {}
            }
            activeSession = newSess;
            lastArtKey.clear();
            if (activeSession) {
                tokMedia = activeSession.MediaPropertiesChanged([](auto&&, auto&&) { SignalSmtcRefresh(); });
                tokPlayback = activeSession.PlaybackInfoChanged([](auto&&, auto&&) { SignalSmtcRefresh(); });
                tokTimeline = activeSession.TimelinePropertiesChanged([](auto&&, auto&&) { SignalSmtcRefresh(); });
            }
        };

        auto tokCurrent = mgr.CurrentSessionChanged([](auto&&, auto&&) { SignalSmtcRefresh(); });
        auto tokSessions = mgr.SessionsChanged([](auto&&, auto&&) { SignalSmtcRefresh(); });

        bindSession(ResolveBestSession(mgr));
        RefreshFromSession(activeSession, lastArtKey);

        HANDLE waitHandles[3] = {g_smtcStopEvent, g_cmdEvent, g_smtcRefreshEvent};
        while (true) {
            DWORD waitRes = WaitForMultipleObjects(3, waitHandles, FALSE, INFINITE);
            if (waitRes == WAIT_OBJECT_0) {
                break;
            }

            if (waitRes == WAIT_OBJECT_0 + 1) {
                std::vector<PendingCommand> cmds;
                {
                    std::lock_guard<std::mutex> lock(g_cmdMutex);
                    cmds.swap(g_cmdQueue);
                }

                if (activeSession) {
                    for (const auto& c : cmds) {
                        try {
                            switch (c.type) {
                                case PendingCommandType::TogglePlayPause:
                                    activeSession.TryTogglePlayPauseAsync().get();
                                    break;
                                case PendingCommandType::Next:
                                    activeSession.TrySkipNextAsync().get();
                                    break;
                                case PendingCommandType::Previous:
                                    activeSession.TrySkipPreviousAsync().get();
                                    break;
                                case PendingCommandType::ToggleShuffle: {
                                    auto pb = activeSession.GetPlaybackInfo();
                                    bool cur = pb.IsShuffleActive() ? pb.IsShuffleActive().Value() : false;
                                    activeSession.TryChangeShuffleActiveAsync(!cur).get();
                                    break;
                                }
                                case PendingCommandType::CycleRepeat: {
                                    auto pb = activeSession.GetPlaybackInfo();
                                    auto cur = pb.AutoRepeatMode()
                                        ? pb.AutoRepeatMode().Value()
                                        : winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                                    auto nextMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List;
                                    if (cur == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List)
                                        nextMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track;
                                    else if (cur == winrt::Windows::Media::MediaPlaybackAutoRepeatMode::Track)
                                        nextMode = winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                                    activeSession.TryChangeAutoRepeatModeAsync(nextMode).get();
                                    break;
                                }
                                case PendingCommandType::SeekToRatio: {
                                    auto tl = activeSession.GetTimelineProperties();
                                    int64_t startTicks = tl.StartTime().count();
                                    int64_t endTicks = tl.EndTime().count();
                                    int64_t targetTicks = startTicks + static_cast<int64_t>((endTicks - startTicks) * c.seekRatio);
                                    activeSession.TryChangePlaybackPositionAsync(targetTicks).get();
                                    break;
                                }
                            }
                        } catch (const winrt::hresult_error& ex) {
                            Wh_Log(L"SMTC command error: 0x%08X", ex.code().value);
                        } catch (...) {}
                    }
                }
            }

            bindSession(ResolveBestSession(mgr));
            RefreshFromSession(activeSession, lastArtKey);
        }

        mgr.CurrentSessionChanged(tokCurrent);
        mgr.SessionsChanged(tokSessions);
        if (activeSession) {
            try {
                activeSession.MediaPropertiesChanged(tokMedia);
                activeSession.PlaybackInfoChanged(tokPlayback);
                activeSession.TimelinePropertiesChanged(tokTimeline);
            } catch (...) {}
        }
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"SMTC worker init error: 0x%08X (%s)", ex.code().value, ex.message().c_str());
    } catch (...) {
        Wh_Log(L"SMTC worker unknown error");
    }

    winrt::uninit_apartment();
    return 0;
}

// ============================================================================
// Windhawk Tool Entry Points
// ============================================================================
void WhTool_ModUninit();

BOOL WhTool_ModInit() {
    g_hInstance = GetModuleHandleW(nullptr);

    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_smtcStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_smtcRefreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_cmdEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    if (!g_uiReadyEvent || !g_smtcStopEvent || !g_smtcRefreshEvent || !g_cmdEvent) {
        if (g_uiReadyEvent) { CloseHandle(g_uiReadyEvent); g_uiReadyEvent = nullptr; }
        if (g_smtcStopEvent) { CloseHandle(g_smtcStopEvent); g_smtcStopEvent = nullptr; }
        if (g_smtcRefreshEvent) { CloseHandle(g_smtcRefreshEvent); g_smtcRefreshEvent = nullptr; }
        if (g_cmdEvent) { CloseHandle(g_cmdEvent); g_cmdEvent = nullptr; }
        return FALSE;
    }

    g_uiThreadHandle = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    g_smtcThreadHandle = CreateThread(nullptr, 0, SmtcWorkerProc, nullptr, 0, nullptr);

    if (!g_uiThreadHandle || !g_smtcThreadHandle) {
        WhTool_ModUninit();
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    if (g_smtcStopEvent) SetEvent(g_smtcStopEvent);
    if (g_smtcThreadHandle) {
        WaitForSingleObject(g_smtcThreadHandle, INFINITE);
        CloseHandle(g_smtcThreadHandle);
        g_smtcThreadHandle = nullptr;
    }

    if (g_uiReadyEvent && g_uiThreadHandle) {
        HANDLE waits[2] = {g_uiReadyEvent, g_uiThreadHandle};
        WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    }
    if (HWND hwnd = g_uiThreadHwnd.load()) {
        PostMessageW(hwnd, WM_APP_DESTROY_UI, 0, 0);
    }
    if (g_uiThreadHandle) {
        WaitForSingleObject(g_uiThreadHandle, INFINITE);
        CloseHandle(g_uiThreadHandle);
        g_uiThreadHandle = nullptr;
    }

    if (g_uiReadyEvent) { CloseHandle(g_uiReadyEvent); g_uiReadyEvent = nullptr; }
    if (g_smtcStopEvent) { CloseHandle(g_smtcStopEvent); g_smtcStopEvent = nullptr; }
    if (g_smtcRefreshEvent) { CloseHandle(g_smtcRefreshEvent); g_smtcRefreshEvent = nullptr; }
    if (g_cmdEvent) { CloseHandle(g_cmdEvent); g_cmdEvent = nullptr; }
}

void WhTool_ModSettingsChanged() {
    HWND hwnd = g_uiThreadHwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
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

    using CreateProcessInternalW_t = WINBOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
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