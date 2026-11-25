// ==WindhawkMod==
// @id              alembiq-weather-desktop-overlay
// @name            Desktop Weather & Particles Overlay
// @description     Weather effects and seasonal particles under desktop icons with window accumulation and shake physics.
// @version         1.5.0
// @author          alembiq
// @github          https://github.com/alembiqspace
// @include         explorer.exe
// @compilerOptions -ld2d1 -ldwrite -lwininet -luser32 -lgdi32 -lshell32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
A lightweight **Direct2D** weather and atmosphere overlay for the Windows desktop. It brings live weather precipitation and atmospheric layers to your desktop without cluttering icons or windows.

### Why Use It
- **Alive Desktop Atmosphere:** Match your desktop environment to real-world weather or switch effects manually whenever you want a specific mood.
- **Safe Desktop Integration:** Runs as a standalone Windhawk tool overlay without hooking `explorer.exe`, preventing duplicate overlays or shell instability.
- **Flexible Customization:** Adjust precipitation intensity and speed, effect strength, custom HEX colors, screen bounds, and frame rate.

### Features
- **Weather Precipitation:** Supports `off`, `auto`, `rain`, `snow`, and `hail` with configurable fall speed and direction.
- **Atmospheric Effects:** Combines clear sunlight glow, partly cloudy, overcast, dust, fog, smog, thunderstorm flashes, aurora, mirage, and configurable rainbow and halo arcs.
- **Weather Sources:** Pulls live data from **Open-Meteo**, a custom JSON API endpoint, or a local file, with `auto` / `on` / `off` control per effect.

### Network & Privacy
- **No Unsolicited Requests by Default:** `[Weather] Latitude/Longitude` (`coords`) is empty by default. No external network requests are made until you configure valid coordinates (or a custom JSON API URL) and have at least one forecast-driven setting (`precipitationMode`, `precipFallDirection`, or any general effect) in `auto` mode.
- **What Is Sent & When:** When `primarySource` is set to `open_meteo` with valid `lat/lon` coordinates and at least one forecast-driven setting uses `auto`, the mod sends an HTTPS `GET` request to `https://api.open-meteo.com/v1/forecast` containing only your configured coordinates and requested weather fields (`weather_code`, `wind_speed_10m`, `wind_direction_10m`, `temperature_2m`, `cloud_cover`, `visibility`, `is_day`). Requests run on startup, on settings changes, every `[Weather] Update interval (min)` (default: `30` minutes), and every `60` seconds after a failed attempt.
- **How to Disable:** Set `[Weather] Primary source` (`primarySource`) to `off`, leave `[Weather] Latitude/Longitude` (`coords`) empty, or switch all forecast-driven modes (including `precipFallDirection`) away from `auto`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# --- 1. SYSTEM & LAYER ---
- layerAttachMode: auto
  $name: "[System] Layer attach mode"
  $description: "Controls Z-order placement of the desktop overlay. Allowed: auto, progman_direct, desktop_bottom. Default: auto."
  $options:
    - auto: Auto (desktop layer)
    - progman_direct: Progman reference
    - desktop_bottom: Bottom of Z-order (HWND_BOTTOM)
- bottomBoundaryMode: above_taskbar
  $name: "[System] Bottom boundary mode"
  $description: "Sets bottom reference line for ground effects such as mirage. Allowed: above_taskbar, screen_bottom, custom_offset. Default: above_taskbar."
  $options:
    - above_taskbar: Above taskbar (Shell_TrayWnd)
    - screen_bottom: Bottom edge of screen
    - custom_offset: Custom offset from bottom (px)
- bottomOffsetPx: 0
  $name: "[System] Custom bottom offset (px)"
  $description: "Vertical offset in px above bottom edge when bottomBoundaryMode is custom_offset. Range: -500 to 1000. Invalid resets to 0."
- multiMonitorMode: primary
  $name: "[System] Monitors"
  $description: "Render on primary monitor or full virtual screen. Allowed: primary, all. Default: primary."
  $options:
    - primary: Primary monitor
    - all: All monitors (virtual screen)
- targetFps: 60
  $name: "[System] Target FPS (30 or 60)"
  $description: "Target frame rate (30 or 60). Any other value resets to 60."
- pauseOnFullscreen: true
  $name: "[System] Pause on fullscreen apps"
  $description: "Pauses rendering while a fullscreen app is active. Default: true."

# --- 2. WEATHER SOURCES ---
- primarySource: open_meteo
  $name: "[Weather] Primary source"
  $description: "Primary forecast provider for auto modes. Allowed: off, open_meteo, custom_api, local_file. Default: open_meteo."
  $options:
    - off: Off (disable forecast fetching)
    - open_meteo: Open-Meteo API
    - custom_api: Custom JSON URL
    - local_file: Local file
- fallbackSource: off
  $name: "[Weather] Fallback source"
  $description: "Backup forecast source if primary fails. Allowed: off, open_meteo, local_file. Default: off."
  $options:
    - off: Off
    - open_meteo: Open-Meteo API
    - local_file: Local file
- coords: ""
  $name: "[Weather] Latitude/Longitude"
  $description: "Format: lat/lon (e.g. 44.7866/20.4489). Latitude -90..90, longitude -180..180. Empty disables Open-Meteo requests."
- customApiUrl: ""
  $name: "[Weather] Custom JSON API URL"
  $description: "HTTP/HTTPS JSON endpoint (weather_code, wind_speed_10m, wind_direction_10m, temperature_2m, cloud_cover, visibility, is_day)."
- localWeatherPath: ""
  $name: "[Weather] Local weather file path"
  $description: "Path to local JSON weather file. Default: empty."
- updateIntervalMin: 30
  $name: "[Weather] Update interval (min)"
  $description: "Forecast refresh period in minutes (1 to 1440). Invalid/out-of-range resets to 30."

# --- 3. PRECIPITATION (RAIN / SNOW / HAIL) ---
- precipitationMode: auto
  $name: "[Precip] Mode"
  $description: "Precipitation layer mode. Allowed: off, auto, rain, snow, hail. Default: auto."
  $options:
    - off: Off
    - auto: Auto (forecast)
    - rain: Rain
    - snow: Snow
    - hail: Hail
- precipFallDirection: auto
  $name: "[Precip] Fall direction"
  $description: "Fall direction for precipitation. Allowed: auto, down, down_left, down_right. Default: auto."
  $options:
    - auto: Auto (forecast)
    - down: Down
    - down_left: Down-left
    - down_right: Down-right
- precipFallSpeedMps: "6.5"
  $name: "[Precip] Fall speed (m/s)"
  $description: "Fall speed in m/s (0.1 to 50.0). Invalid/out-of-range resets to 6.5."
- precipIntensity: 25
  $name: "[Precip] Intensity (%)"
  $description: "Shared intensity for rain, snow, and hail (0..100). Any value outside 0..100 or invalid resets to 25."

# --- 4. GENERAL EFFECTS ---
- effectsIntensity: 25
  $name: "[Effects] General intensity (%)"
  $description: "Strength/opacity for general effects (0..100). Outside 0..100 or invalid resets to 25."
- effectClear: auto
  $name: "[Effects] Clear"
  $description: "Clear sky sunlight glow. Allowed: auto, on, off. Can stack with other effects."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectPartlyCloudy: auto
  $name: "[Effects] Partly cloudy"
  $description: "Soft cloud layer. Allowed: auto, on, off. Can stack with other effects."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectOvercast: auto
  $name: "[Effects] Overcast"
  $description: "Overcast sky veil. Allowed: auto, on, off. Can stack with other effects."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectDust: auto
  $name: "[Effects] Dust"
  $description: "Airborne dust haze (auto triggers on strong dry wind or low visibility). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectFog: auto
  $name: "[Effects] Fog"
  $description: "Soft mist/fog layer. Allowed: auto, on, off. Can stack with other effects."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectSmog: auto
  $name: "[Effects] Smog"
  $description: "Smog layer (auto triggers on calm wind with reduced visibility/haze). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectThunderstorm: auto
  $name: "[Effects] Thunderstorm"
  $description: "Storm atmosphere with branched lightning flashes. Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectAurora: auto
  $name: "[Effects] Aurora"
  $description: "Smooth animated aurora curtains in upper screen area (auto triggers at night with clear/low clouds). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectMirage: auto
  $name: "[Effects] Mirage"
  $description: "Soft horizon shimmer wave band (auto triggers on hot clear daytime weather). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectRainbow: auto
  $name: "[Effects] Rainbow"
  $description: "Multi-band rainbow arc (auto triggers on daytime partly-cloudy/post-rain/misty conditions). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- rainbowGeometry: "55-50/15"
  $name: "[Rainbow] Radius-Center X/Center Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius 0 = off, 100 = min(width, height); X left->right, Y bottom->top). Default: 55-50/15."
- effectHalo: auto
  $name: "[Effects] Halo"
  $description: "Soft luminous atmospheric halo ring (auto triggers on thin/mid cloud cover without heavy precip). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- haloGeometry: "24-50/72"
  $name: "[Halo] Radius-Center X/Center Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius 0 = off, 100 = min(width, height); X left->right, Y bottom->top). Default: 24-50/72."

# --- 5. COLORS ---
- colorRain: ""
  $name: "[Color] Rain (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default sky-blue. Invalid HEX resets to default."
- colorSnow: ""
  $name: "[Color] Snow (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default natural white-blue snow. Invalid HEX resets to default."
- colorHail: ""
  $name: "[Color] Hail (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default ice-blue. Invalid HEX resets to default."
- colorClear: ""
  $name: "[Color] Clear (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default warm sunlight."
- colorPartlyCloudy: ""
  $name: "[Color] Partly cloudy (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default soft cloud."
- colorOvercast: ""
  $name: "[Color] Overcast (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default slate gray."
- colorDust: ""
  $name: "[Color] Dust (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default sand dust."
- colorFog: ""
  $name: "[Color] Fog (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default mist."
- colorSmog: ""
  $name: "[Color] Smog (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default smog."
- colorThunderstorm: ""
  $name: "[Color] Thunderstorm (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default lightning white-blue."
- colorAurora: ""
  $name: "[Color] Aurora (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default aurora green."
- colorMirage: ""
  $name: "[Color] Mirage (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default warm shimmer."
- colorRainbow: ""
  $name: "[Color] Rainbow (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default multi-color spectrum."
- colorHalo: ""
  $name: "[Color] Halo (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default pale halo."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <wininet.h>
#include <d2d1.h>
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr wchar_t kOverlayWndClass[] = L"WindhawkDesktopWeatherOverlayClass";

enum class ObjectKind {
    Rain,
    Snow,
    Hail
};

enum class PrecipMode {
    Off,
    Auto,
    Rain,
    Snow,
    Hail
};

enum class TriStateMode {
    Auto,
    On,
    Off
};

enum class FallDirMode {
    Auto,
    Down,
    DownLeft,
    DownRight
};

struct GeneralEffectsState {
    bool clear = false;
    bool partlyCloudy = false;
    bool overcast = false;
    bool dust = false;
    bool fog = false;
    bool smog = false;
    bool thunderstorm = false;
    bool aurora = false;
    bool mirage = false;
    bool rainbow = false;
    bool halo = false;
};

struct ArcGeometry {
    int radius = 50;
    int centerX = 50;
    int centerY = 50;
};

struct CachedArcLayout {
    int canvasW = 0;
    int canvasH = 0;
    int pctRadius = -1;
    int pctX = -1;
    int pctY = -1;
    float radius = 0.0f;
    float cx = 0.0f;
    float cy = 0.0f;
    float bandThickness = 0.0f;
    bool valid = false;
};

struct Settings {
    std::wstring layerAttachMode = L"auto";
    std::wstring bottomBoundaryMode = L"above_taskbar";
    int bottomOffsetPx = 0;
    std::wstring multiMonitorMode = L"primary";

    int targetFps = 60;
    bool pauseOnFullscreen = true;

    std::wstring primarySource = L"open_meteo";
    std::wstring fallbackSource = L"off";
    bool hasValidCoords = false;
    double latitude = 0.0;
    double longitude = 0.0;
    std::wstring customApiUrl;
    std::wstring localWeatherPath;
    int updateIntervalMin = 30;

    PrecipMode precipitationMode = PrecipMode::Auto;
    FallDirMode precipFallDirection = FallDirMode::Auto;
    double precipFallSpeedMps = 6.5;
    int precipIntensity = 25;

    int effectsIntensity = 25;
    TriStateMode effectClear = TriStateMode::Auto;
    TriStateMode effectPartlyCloudy = TriStateMode::Auto;
    TriStateMode effectOvercast = TriStateMode::Auto;
    TriStateMode effectDust = TriStateMode::Auto;
    TriStateMode effectFog = TriStateMode::Auto;
    TriStateMode effectSmog = TriStateMode::Auto;
    TriStateMode effectThunderstorm = TriStateMode::Auto;
    TriStateMode effectAurora = TriStateMode::Auto;
    TriStateMode effectMirage = TriStateMode::Auto;
    TriStateMode effectRainbow = TriStateMode::Auto;
    TriStateMode effectHalo = TriStateMode::Auto;

    ArcGeometry rainbowGeom{55, 50, 15};
    ArcGeometry haloGeom{24, 50, 72};

    bool hasCustomSnowColor = false;
    D2D1_COLOR_F colorRain = D2D1::ColorF(0.42f, 0.72f, 1.0f, 0.78f);
    D2D1_COLOR_F colorSnow = D2D1::ColorF(0.98f, 0.99f, 1.0f, 0.96f);
    D2D1_COLOR_F colorHail = D2D1::ColorF(0.76f, 0.90f, 1.0f, 0.95f);

    bool hasCustomClearColor = false;
    D2D1_COLOR_F colorClear = D2D1::ColorF(1.0f, 0.92f, 0.65f, 0.25f);
    bool hasCustomPartlyCloudyColor = false;
    D2D1_COLOR_F colorPartlyCloudy = D2D1::ColorF(0.88f, 0.91f, 0.95f, 0.22f);
    bool hasCustomOvercastColor = false;
    D2D1_COLOR_F colorOvercast = D2D1::ColorF(0.55f, 0.59f, 0.65f, 0.35f);
    bool hasCustomDustColor = false;
    D2D1_COLOR_F colorDust = D2D1::ColorF(0.84f, 0.68f, 0.45f, 0.28f);
    bool hasCustomFogColor = false;
    D2D1_COLOR_F colorFog = D2D1::ColorF(0.82f, 0.86f, 0.90f, 0.35f);
    bool hasCustomSmogColor = false;
    D2D1_COLOR_F colorSmog = D2D1::ColorF(0.56f, 0.50f, 0.42f, 0.36f);
    bool hasCustomThunderstormColor = false;
    D2D1_COLOR_F colorThunderstorm = D2D1::ColorF(0.84f, 0.92f, 1.0f, 0.95f);
    bool hasCustomAuroraColor = false;
    D2D1_COLOR_F colorAurora = D2D1::ColorF(0.22f, 0.92f, 0.62f, 0.32f);
    bool hasCustomMirageColor = false;
    D2D1_COLOR_F colorMirage = D2D1::ColorF(0.98f, 0.86f, 0.66f, 0.25f);
    bool hasCustomRainbowColor = false;
    D2D1_COLOR_F colorRainbow = D2D1::ColorF(1.0f, 0.78f, 0.35f, 0.35f);
    bool hasCustomHaloColor = false;
    D2D1_COLOR_F colorHalo = D2D1::ColorF(0.98f, 0.95f, 0.82f, 0.32f);
};

struct FallingItem {
    ObjectKind kind = ObjectKind::Snow;
    float x = 0.0f;
    float y = 0.0f;
    float size = 6.0f;
    float angle = 0.0f;
    float spin = 0.0f;
    float swayPhase = 0.0f;
    float alpha = 0.9f;
};

struct LightningFlash {
    float timer = 0.0f;
    float xStart = 0.0f;
    float xEnd = 0.0f;
};

struct CachedSurface {
    ID2D1DCRenderTarget* rt = nullptr;
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    ID2D1Bitmap* flakeSprite = nullptr;
    ID2D1SolidColorBrush* sharedBrush = nullptr;
    D2D1_COLOR_F spriteColor = D2D1::ColorF(0, 0, 0, 0);
    int width = 0;
    int height = 0;
};

std::mutex g_mutex;
Settings g_settings;
bool g_arcCacheDirty = true;

bool g_hasForecastPrecip = false;
ObjectKind g_forecastPrecip = ObjectKind::Rain;
int g_forecastWindDirSign = 0;
double g_forecastWindSpeedKmh = 0.0;
GeneralEffectsState g_forecastEffects;

HWND g_overlayHwnd = nullptr;
HANDLE g_weatherWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_renderThread;
[[clang::no_destroy]] std::optional<std::thread> g_weatherThread;
std::atomic<bool> g_running{false};

ID2D1Factory* g_d2dFactory = nullptr;
CachedSurface g_surface;
CachedArcLayout g_rainbowCache;
CachedArcLayout g_haloCache;

int g_canvasLeft = 0;
int g_canvasTop = 0;
int g_canvasWidth = 0;
int g_canvasHeight = 0;

std::vector<FallingItem> g_falling;
LightningFlash g_lightning;

bool HasAnyAutoSetting(const Settings& cfg) {
    return cfg.precipitationMode == PrecipMode::Auto ||
           cfg.precipFallDirection == FallDirMode::Auto ||
           cfg.effectClear == TriStateMode::Auto ||
           cfg.effectPartlyCloudy == TriStateMode::Auto ||
           cfg.effectOvercast == TriStateMode::Auto ||
           cfg.effectDust == TriStateMode::Auto ||
           cfg.effectFog == TriStateMode::Auto ||
           cfg.effectSmog == TriStateMode::Auto ||
           cfg.effectThunderstorm == TriStateMode::Auto ||
           cfg.effectAurora == TriStateMode::Auto ||
           cfg.effectMirage == TriStateMode::Auto ||
           cfg.effectRainbow == TriStateMode::Auto ||
           cfg.effectHalo == TriStateMode::Auto;
}

void RefreshArcCache(CachedArcLayout& cache, const ArcGeometry& geom, bool forceDirty) {
    if (!forceDirty && cache.valid && cache.canvasW == g_canvasWidth && cache.canvasH == g_canvasHeight &&
        cache.pctRadius == geom.radius && cache.pctX == geom.centerX && cache.pctY == geom.centerY) {
        return;
    }
    cache.canvasW = g_canvasWidth;
    cache.canvasH = g_canvasHeight;
    cache.pctRadius = geom.radius;
    cache.pctX = geom.centerX;
    cache.pctY = geom.centerY;

    float minSide = static_cast<float>(std::min(g_canvasWidth, g_canvasHeight));
    cache.radius = minSide * (static_cast<float>(geom.radius) / 100.0f);
    cache.cx = static_cast<float>(g_canvasWidth) * (static_cast<float>(geom.centerX) / 100.0f);
    cache.cy = static_cast<float>(g_canvasHeight) * (1.0f - static_cast<float>(geom.centerY) / 100.0f);
    cache.bandThickness = std::max(4.0f, cache.radius * 0.035f);
    cache.valid = true;
}

std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = static_cast<wchar_t>(towlower(ch));
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool TryParseDoubleStrict(const std::wstring& raw, double& outVal) {
    std::wstring s = Trim(raw);
    if (s.empty()) return false;
    for (wchar_t& c : s) {
        if (c == L',') c = L'.';
    }
    wchar_t* endPtr = nullptr;
    errno = 0;
    double v = std::wcstod(s.c_str(), &endPtr);
    if (errno != 0 || endPtr == s.c_str()) return false;
    while (*endPtr != L'\0') {
        if (!iswspace(*endPtr)) return false;
        ++endPtr;
    }
    if (!std::isfinite(v)) return false;
    outVal = v;
    return true;
}

int ParseIntRangeOrDefault(int rawVal, int minVal, int maxVal, int defVal) {
    if (rawVal < minVal || rawVal > maxVal) return defVal;
    return rawVal;
}

double ParseDoubleRangeOrDefault(const std::wstring& raw, double minVal, double maxVal, double defVal) {
    double v = 0.0;
    if (!TryParseDoubleStrict(raw, v)) return defVal;
    if (v < minVal || v > maxVal) return defVal;
    return v;
}

int SanitizePercent(int rawVal) {
    return ParseIntRangeOrDefault(rawVal, 0, 100, 25);
}

bool TryParseCoordsString(const std::wstring& raw, double& outLat, double& outLon) {
    std::wstring s = Trim(raw);
    if (s.empty()) return false;
    size_t sep = s.find(L'/');
    if (sep == std::wstring::npos) sep = s.find(L',');
    if (sep == std::wstring::npos) return false;
    double lat = 0.0, lon = 0.0;
    if (!TryParseDoubleStrict(s.substr(0, sep), lat) || !TryParseDoubleStrict(s.substr(sep + 1), lon) ||
        lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
        return false;
    }
    outLat = lat;
    outLon = lon;
    return true;
}

void ParseArcGeometryString(const std::wstring& raw, ArcGeometry defGeom, ArcGeometry& outGeom) {
    outGeom = defGeom;
    std::wstring s = Trim(raw);
    size_t dash = s.find(L'-');
    size_t slash = s.find(L'/');
    if (dash == std::wstring::npos || slash == std::wstring::npos || slash <= dash) return;

    std::wstring rPart = Trim(s.substr(0, dash));
    std::wstring xPart = Trim(s.substr(dash + 1, slash - dash - 1));
    std::wstring yPart = Trim(s.substr(slash + 1));

    auto isIntStr = [](const std::wstring& p) -> bool {
        if (p.empty()) return false;
        for (wchar_t c : p) if (!iswdigit(c)) return false;
        return true;
    };
    if (!isIntStr(rPart) || !isIntStr(xPart) || !isIntStr(yPart)) return;

    try {
        int r = std::stoi(rPart);
        int x = std::stoi(xPart);
        int y = std::stoi(yPart);
        if (r < 0 || r > 100 || x < 0 || x > 100 || y < 0 || y > 100) return;
        outGeom.radius = r;
        outGeom.centerX = x;
        outGeom.centerY = y;
    } catch (...) {
    }
}

PrecipMode ParsePrecipMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"off") return PrecipMode::Off;
    if (v == L"rain") return PrecipMode::Rain;
    if (v == L"snow") return PrecipMode::Snow;
    if (v == L"hail") return PrecipMode::Hail;
    return PrecipMode::Auto;
}

TriStateMode ParseTriState(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"on") return TriStateMode::On;
    if (v == L"off") return TriStateMode::Off;
    return TriStateMode::Auto;
}

FallDirMode ParseFallDir(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"down") return FallDirMode::Down;
    if (v == L"down_left" || v == L"left") return FallDirMode::DownLeft;
    if (v == L"down_right" || v == L"right") return FallDirMode::DownRight;
    return FallDirMode::Auto;
}

int WindDegToSign(double deg) {
    double norm = std::fmod(deg, 360.0);
    if (norm < 0.0) norm += 360.0;
    if (norm >= 45.0 && norm < 225.0) return 1;
    return -1;
}

bool TryParseHexColorStrict(const std::wstring& hexRaw, float defaultAlpha, D2D1_COLOR_F& outColor) {
    std::wstring s = Trim(hexRaw);
    if (s.empty()) return false;
    if (s[0] == L'#') s.erase(0, 1);
    if (s.size() != 6) return false;
    for (wchar_t c : s) {
        if (!iswxdigit(c)) return false;
    }
    try {
        unsigned long rgb = std::stoul(s, nullptr, 16);
        float r = ((rgb >> 16) & 0xFF) / 255.0f;
        float g = ((rgb >> 8) & 0xFF) / 255.0f;
        float b = (rgb & 0xFF) / 255.0f;
        outColor = D2D1::ColorF(r, g, b, defaultAlpha);
        return true;
    } catch (...) {
        return false;
    }
}

void LoadSettingsFromWh() {
    Settings next;
    auto readStr = [](LPCWSTR name, const std::wstring& defVal) -> std::wstring {
        WindhawkUtils::StringSetting raw(Wh_GetStringSetting(name));
        std::wstring out(raw.get());
        return out.empty() ? defVal : out;
    };

    next.layerAttachMode = readStr(L"layerAttachMode", L"auto");
    next.bottomBoundaryMode = readStr(L"bottomBoundaryMode", L"above_taskbar");
    next.bottomOffsetPx = ParseIntRangeOrDefault(Wh_GetIntSetting(L"bottomOffsetPx"), -500, 1000, 0);
    next.multiMonitorMode = readStr(L"multiMonitorMode", L"primary");

    int fpsRaw = Wh_GetIntSetting(L"targetFps");
    next.targetFps = (fpsRaw == 30 || fpsRaw == 60) ? fpsRaw : 60;
    next.pauseOnFullscreen = Wh_GetIntSetting(L"pauseOnFullscreen") != 0;

    next.primarySource = ToLower(Trim(readStr(L"primarySource", L"open_meteo")));
    next.fallbackSource = ToLower(Trim(readStr(L"fallbackSource", L"off")));
    next.hasValidCoords = TryParseCoordsString(readStr(L"coords", L""), next.latitude, next.longitude);
    next.customApiUrl = Trim(readStr(L"customApiUrl", L""));
    next.localWeatherPath = Trim(readStr(L"localWeatherPath", L""));
    next.updateIntervalMin = ParseIntRangeOrDefault(Wh_GetIntSetting(L"updateIntervalMin"), 1, 1440, 30);

    next.precipitationMode = ParsePrecipMode(readStr(L"precipitationMode", L"auto"));
    next.precipFallDirection = ParseFallDir(readStr(L"precipFallDirection", L"auto"));
    next.precipFallSpeedMps = ParseDoubleRangeOrDefault(readStr(L"precipFallSpeedMps", L"6.5"), 0.1, 50.0, 6.5);
    next.precipIntensity = SanitizePercent(Wh_GetIntSetting(L"precipIntensity"));

    next.effectsIntensity = SanitizePercent(Wh_GetIntSetting(L"effectsIntensity"));
    next.effectClear = ParseTriState(readStr(L"effectClear", L"auto"));
    next.effectPartlyCloudy = ParseTriState(readStr(L"effectPartlyCloudy", L"auto"));
    next.effectOvercast = ParseTriState(readStr(L"effectOvercast", L"auto"));
    next.effectDust = ParseTriState(readStr(L"effectDust", L"auto"));
    next.effectFog = ParseTriState(readStr(L"effectFog", L"auto"));
    next.effectSmog = ParseTriState(readStr(L"effectSmog", L"auto"));
    next.effectThunderstorm = ParseTriState(readStr(L"effectThunderstorm", L"auto"));
    next.effectAurora = ParseTriState(readStr(L"effectAurora", L"auto"));
    next.effectMirage = ParseTriState(readStr(L"effectMirage", L"auto"));
    next.effectRainbow = ParseTriState(readStr(L"effectRainbow", L"auto"));
    next.effectHalo = ParseTriState(readStr(L"effectHalo", L"auto"));

    ParseArcGeometryString(readStr(L"rainbowGeometry", L"55-50/15"), ArcGeometry{55, 50, 15}, next.rainbowGeom);
    ParseArcGeometryString(readStr(L"haloGeometry", L"24-50/72"), ArcGeometry{24, 50, 72}, next.haloGeom);

    TryParseHexColorStrict(readStr(L"colorRain", L""), 0.78f, next.colorRain);
    next.hasCustomSnowColor = TryParseHexColorStrict(readStr(L"colorSnow", L""), 0.96f, next.colorSnow);
    TryParseHexColorStrict(readStr(L"colorHail", L""), 0.95f, next.colorHail);

    next.hasCustomClearColor = TryParseHexColorStrict(readStr(L"colorClear", L""), 0.25f, next.colorClear);
    next.hasCustomPartlyCloudyColor = TryParseHexColorStrict(readStr(L"colorPartlyCloudy", L""), 0.22f, next.colorPartlyCloudy);
    next.hasCustomOvercastColor = TryParseHexColorStrict(readStr(L"colorOvercast", L""), 0.35f, next.colorOvercast);
    next.hasCustomDustColor = TryParseHexColorStrict(readStr(L"colorDust", L""), 0.28f, next.colorDust);
    next.hasCustomFogColor = TryParseHexColorStrict(readStr(L"colorFog", L""), 0.35f, next.colorFog);
    next.hasCustomSmogColor = TryParseHexColorStrict(readStr(L"colorSmog", L""), 0.36f, next.colorSmog);
    next.hasCustomThunderstormColor = TryParseHexColorStrict(readStr(L"colorThunderstorm", L""), 0.95f, next.colorThunderstorm);
    next.hasCustomAuroraColor = TryParseHexColorStrict(readStr(L"colorAurora", L""), 0.32f, next.colorAurora);
    next.hasCustomMirageColor = TryParseHexColorStrict(readStr(L"colorMirage", L""), 0.25f, next.colorMirage);
    next.hasCustomRainbowColor = TryParseHexColorStrict(readStr(L"colorRainbow", L""), 0.35f, next.colorRainbow);
    next.hasCustomHaloColor = TryParseHexColorStrict(readStr(L"colorHalo", L""), 0.32f, next.colorHalo);

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_settings = next;
        g_arcCacheDirty = true;
    }
    if (g_weatherWakeEvent) {
        SetEvent(g_weatherWakeEvent);
    }
}

std::string HttpGetUtf8(const std::wstring& url) {
    URL_COMPONENTSW parts = {};
    parts.dwStructSize = sizeof(parts);
    wchar_t host[256] = {};
    wchar_t path[2048] = {};
    parts.lpszHostName = host;
    parts.dwHostNameLength = _countof(host);
    parts.lpszUrlPath = path;
    parts.dwUrlPathLength = _countof(path);

    if (!InternetCrackUrlW(url.c_str(), 0, 0, &parts)) {
        Wh_Log(L"InternetCrackUrlW failed for URL");
        return {};
    }

    HINTERNET hSession = InternetOpenW(L"WindhawkWeatherMod/2.3", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hSession) {
        Wh_Log(L"InternetOpenW failed: %u", GetLastError());
        return {};
    }

    DWORD timeoutMs = 5000;
    InternetSetOptionW(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionW(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    if (parts.nScheme == INTERNET_SCHEME_HTTPS) {
        flags |= INTERNET_FLAG_SECURE;
    }

    HINTERNET hConnect = InternetConnectW(
        hSession, host, parts.nPort, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        Wh_Log(L"InternetConnectW failed: %u", GetLastError());
        InternetCloseHandle(hSession);
        return {};
    }

    HINTERNET hRequest = HttpOpenRequestW(
        hConnect, L"GET", path, nullptr, nullptr, nullptr, flags, 0);
    std::string response;
    if (hRequest && HttpSendRequestW(hRequest, nullptr, 0, nullptr, 0)) {
        DWORD statusCode = 0;
        DWORD statusLen = sizeof(statusCode);
        if (HttpQueryInfoW(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusLen, nullptr) &&
            statusCode == 200) {
            char buf[4096];
            DWORD bytesRead = 0;
            while (InternetReadFile(hRequest, buf, sizeof(buf), &bytesRead) && bytesRead > 0) {
                response.append(buf, bytesRead);
            }
        } else {
            Wh_Log(L"HTTP request returned non-200 status: %u", statusCode);
        }
    } else {
        Wh_Log(L"HttpSendRequestW failed: %u", GetLastError());
    }

    if (hRequest) InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);
    return response;
}

void ApplyForecastMetrics(int code, double windKmh, double tempC, int cloudCover,
                          double visibilityM, bool isDay,
                          bool& hasPrecip, ObjectKind& precipKind, GeneralEffectsState& fx) {
    hasPrecip = false;
    fx = GeneralEffectsState{};

    bool isRainCode = (code >= 51 && code <= 67) || (code >= 80 && code <= 82);
    bool isSnowCode = (code == 71 || code == 73 || code == 75 || code == 77 || code == 85 || code == 86);
    bool isStormCode = (code == 95 || code == 96 || code == 99);
    bool isFogCode = (code == 45 || code == 48);

    if (code == 0) fx.clear = true;
    else if (code == 1 || code == 2) fx.partlyCloudy = true;
    else if (code == 3) fx.overcast = true;
    else if (isFogCode) fx.fog = true;

    if (isSnowCode) {
        hasPrecip = true;
        precipKind = ObjectKind::Snow;
    } else if (code == 96 || code == 99) {
        hasPrecip = true;
        precipKind = ObjectKind::Hail;
        fx.thunderstorm = true;
    } else if (code == 95) {
        hasPrecip = true;
        precipKind = ObjectKind::Rain;
        fx.thunderstorm = true;
    } else if (isRainCode) {
        hasPrecip = true;
        precipKind = ObjectKind::Rain;
    }

    if (!hasPrecip && !isStormCode && windKmh >= 28.0 && (visibilityM <= 0.0 || visibilityM < 8000.0)) {
        fx.dust = true;
    }
    if (!hasPrecip && windKmh < 12.0 && visibilityM > 0.0 && visibilityM < 6000.0) {
        fx.smog = true;
    }
    if (isDay && !hasPrecip && !isStormCode && tempC >= 28.0 && cloudCover <= 30) {
        fx.mirage = true;
    }
    if (isDay && !isStormCode && (isRainCode || isFogCode || (cloudCover >= 15 && cloudCover <= 65 && !hasPrecip))) {
        fx.rainbow = true;
    }
    if (!hasPrecip && !isStormCode && cloudCover >= 20 && cloudCover <= 75) {
        fx.halo = true;
    }
    if (!isDay && !hasPrecip && !isStormCode && cloudCover <= 35) {
        fx.aurora = true;
    }
}

bool ParseWeatherJson(const std::string& json, bool& hasPrecip, ObjectKind& precipKind,
                      GeneralEffectsState& fx, double& windSpeed, int& windSign) {
    size_t scope = json.find("\"current\":");
    if (scope == std::string::npos) scope = 0;

    auto extractNumber = [&](const std::string& key, double& val) -> bool {
        size_t pos = json.find("\"" + key + "\"", scope);
        if (pos == std::string::npos) return false;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return false;
        pos++;
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        if (pos >= json.size()) return false;
        char* endPtr = nullptr;
        errno = 0;
        double parsed = std::strtod(json.c_str() + pos, &endPtr);
        if (errno != 0 || endPtr == json.c_str() + pos || !std::isfinite(parsed)) {
            return false;
        }
        val = parsed;
        return true;
    };

    double spd = 0.0;
    if (extractNumber("wind_speed_10m", spd) || extractNumber("windspeed", spd) || extractNumber("wind_speed", spd)) {
        windSpeed = std::max(0.0, spd);
    }

    double deg = -1.0;
    if (extractNumber("wind_direction_10m", deg) || extractNumber("winddirection", deg) || extractNumber("wind_deg", deg)) {
        windSign = (windSpeed > 0.2) ? WindDegToSign(deg) : 0;
    } else {
        windSign = 0;
    }

    double tempC = 15.0;
    extractNumber("temperature_2m", tempC);
    double cloud = 20.0;
    extractNumber("cloud_cover", cloud);
    double vis = 12000.0;
    extractNumber("visibility", vis);
    double isDayNum = 1.0;
    extractNumber("is_day", isDayNum);

    double wmo = -999.0;
    if (extractNumber("weather_code", wmo) || extractNumber("weathercode", wmo)) {
        ApplyForecastMetrics(static_cast<int>(wmo), windSpeed, tempC, static_cast<int>(cloud),
                             vis, isDayNum > 0.5, hasPrecip, precipKind, fx);
        return true;
    }
    Wh_Log(L"Failed to parse weather_code from weather JSON");
    return false;
}

bool FetchWeatherSource(const std::wstring& src, const Settings& cfg, bool& hasPrecip,
                        ObjectKind& precipKind, GeneralEffectsState& fx,
                        double& windSpeed, int& windSign) {
    if (src == L"off") return false;
    if (src == L"open_meteo") {
        if (!cfg.hasValidCoords) return false;
        std::wstringstream url;
        url << L"https://api.open-meteo.com/v1/forecast?latitude=" << cfg.latitude
            << L"&longitude=" << cfg.longitude
            << L"&" L"current=weather_code,wind_speed_10m,wind_direction_10m,temperature_2m,cloud_cover,visibility,is_day";
        std::string body = HttpGetUtf8(url.str());
        if (body.empty()) return false;
        return ParseWeatherJson(body, hasPrecip, precipKind, fx, windSpeed, windSign);
    }
    if (src == L"custom_api") {
        if (cfg.customApiUrl.empty()) return false;
        std::string body = HttpGetUtf8(cfg.customApiUrl);
        if (body.empty()) return false;
        return ParseWeatherJson(body, hasPrecip, precipKind, fx, windSpeed, windSign);
    }
    if (src == L"local_file") {
        if (cfg.localWeatherPath.empty()) return false;
        HANDLE hFile = CreateFileW(cfg.localWeatherPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                     nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            Wh_Log(L"Failed to open local weather file: %u", GetLastError());
            return false;
        }
        DWORD size = GetFileSize(hFile, nullptr);
        std::string content;
        if (size > 0 && size < 1024 * 1024) {
            content.resize(size);
            DWORD read = 0;
            ReadFile(hFile, content.data(), size, &read, nullptr);
            content.resize(read);
        }
        CloseHandle(hFile);
        if (content.empty()) return false;
        return ParseWeatherJson(content, hasPrecip, precipKind, fx, windSpeed, windSign);
    }
    return false;
}

void WeatherWorkerLoop() {
    while (g_running.load()) {
        Settings cfg;
        int intervalSec = 1800;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            cfg = g_settings;
            intervalSec = cfg.updateIntervalMin * 60;
        }

        bool shouldFetch = (cfg.primarySource != L"off") && HasAnyAutoSetting(cfg);
        DWORD waitMs = static_cast<DWORD>(intervalSec) * 1000;

        if (shouldFetch) {
            bool hasP = false;
            ObjectKind pKind = ObjectKind::Rain;
            GeneralEffectsState fx;
            double wSpd = 0.0;
            int wSign = 0;

            bool ok = FetchWeatherSource(cfg.primarySource, cfg, hasP, pKind, fx, wSpd, wSign);
            if (!ok && cfg.fallbackSource != L"off" && cfg.fallbackSource != cfg.primarySource) {
                ok = FetchWeatherSource(cfg.fallbackSource, cfg, hasP, pKind, fx, wSpd, wSign);
            }

            if (ok) {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_hasForecastPrecip = hasP;
                g_forecastPrecip = pKind;
                g_forecastEffects = fx;
                g_forecastWindSpeedKmh = wSpd;
                g_forecastWindDirSign = wSign;
            } else if ((cfg.primarySource == L"open_meteo" && cfg.hasValidCoords) ||
                       (cfg.primarySource == L"custom_api" && !cfg.customApiUrl.empty()) ||
                       (cfg.primarySource == L"local_file" && !cfg.localWeatherPath.empty())) {
                waitMs = std::min<DWORD>(waitMs, 60000);
            }
        }

        if (g_weatherWakeEvent) {
            WaitForSingleObject(g_weatherWakeEvent, waitMs);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
        }
    }
}

void UpdateCanvasGeometry(const Settings& cfg) {
    if (cfg.multiMonitorMode == L"all") {
        g_canvasLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
        g_canvasTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
        g_canvasWidth = std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN));
        g_canvasHeight = std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN));
    } else {
        g_canvasLeft = 0;
        g_canvasTop = 0;
        g_canvasWidth = std::max(1, GetSystemMetrics(SM_CXSCREEN));
        g_canvasHeight = std::max(1, GetSystemMetrics(SM_CYSCREEN));
    }
}

int ComputeBottomLimitY(const Settings& cfg) {
    int baseBottom = g_canvasTop + g_canvasHeight;
    if (cfg.bottomBoundaryMode == L"above_taskbar") {
        HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
        RECT trayRect = {};
        if (hTray && GetWindowRect(hTray, &trayRect)) {
            baseBottom = std::min(baseBottom, static_cast<int>(trayRect.top));
        }
    } else if (cfg.bottomBoundaryMode == L"custom_offset") {
        baseBottom -= cfg.bottomOffsetPx;
    }
    return std::clamp(baseBottom, g_canvasTop + 10, g_canvasTop + g_canvasHeight);
}

bool IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_overlayHwnd) return false;
    wchar_t cls[128] = {};
    GetClassNameW(fg, cls, _countof(cls));
    std::wstring clsStr = cls;
    if (clsStr == L"Progman" || clsStr == L"WorkerW" || clsStr == L"Shell_TrayWnd") return false;

    RECT fgRect = {};
    if (!GetWindowRect(fg, &fgRect)) return false;
    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) return false;

    return (fgRect.left <= mi.rcMonitor.left &&
            fgRect.top <= mi.rcMonitor.top &&
            fgRect.right >= mi.rcMonitor.right &&
            fgRect.bottom >= mi.rcMonitor.bottom);
}

bool IsDesktopOccluded() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defView = nullptr;
    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr)) != nullptr) {
        HWND candidate = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (candidate) {
            defView = candidate;
            break;
        }
    }
    if (!defView && progman) {
        defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    }

    HWND topWnd = GetTopWindow(nullptr);
    while (topWnd) {
        if (topWnd != g_overlayHwnd && topWnd != progman && topWnd != defView && IsWindowVisible(topWnd)) {
            LONG exStyle = GetWindowLongW(topWnd, GWL_EXSTYLE);
            if (!(exStyle & WS_EX_TOOLWINDOW)) {
                wchar_t cls[128] = {};
                GetClassNameW(topWnd, cls, _countof(cls));
                std::wstring clsStr = cls;
                if (clsStr != L"WorkerW" && clsStr != L"Shell_TrayWnd" && clsStr != L"Shell_SecondaryTrayWnd") {
                    RECT wndRect = {};
                    if (GetWindowRect(topWnd, &wndRect) &&
                        wndRect.left <= g_canvasLeft &&
                        wndRect.top <= g_canvasTop &&
                        wndRect.right >= g_canvasLeft + g_canvasWidth &&
                        wndRect.bottom >= g_canvasTop + g_canvasHeight) {
                        return true;
                    }
                }
            }
        }
        topWnd = GetNextWindow(topWnd, GW_HWNDNEXT);
    }
    return false;
}

HWND FindDesktopInsertAfter(const std::wstring& mode, HWND overlay) {
    if (mode == L"desktop_bottom") {
        return HWND_BOTTOM;
    }
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defView = nullptr;
    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr)) != nullptr) {
        HWND candidate = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (candidate) {
            defView = candidate;
            break;
        }
    }
    if (!defView && progman) {
        defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    }

    if (mode == L"progman_direct") {
        return progman ? progman : HWND_BOTTOM;
    }

    if (defView) {
        HWND parent = GetParent(defView);
        if (parent && parent != progman) {
            HWND prev = GetWindow(parent, GW_HWNDPREV);
            if (prev && prev != overlay) return prev;
            return HWND_BOTTOM;
        }
        HWND prev = GetWindow(defView, GW_HWNDPREV);
        if (prev && prev != overlay) {
            return prev;
        }
    }
    return HWND_BOTTOM;
}

void PositionDesktopOverlay(HWND overlay, const std::wstring& mode) {
    if (!overlay) return;
    HWND insertAfter = FindDesktopInsertAfter(mode, overlay);
    SetWindowPos(
        overlay,
        insertAfter,
        g_canvasLeft,
        g_canvasTop,
        g_canvasWidth,
        g_canvasHeight,
        SWP_NOACTIVATE);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) return 1;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void SpawnFallingItem(FallingItem& item, std::mt19937& rng, ObjectKind kind, float vx, bool randomY) {
    std::uniform_real_distribution<float> phaseDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> alphaDist(0.65f, 0.96f);

    item.kind = kind;
    item.swayPhase = phaseDist(rng);
    item.angle = phaseDist(rng);
    item.alpha = alphaDist(rng);

    if (randomY) {
        std::uniform_real_distribution<float> xDist(-40.0f, static_cast<float>(g_canvasWidth) + 40.0f);
        std::uniform_real_distribution<float> yDist(0.0f, static_cast<float>(g_canvasHeight));
        item.x = xDist(rng);
        item.y = yDist(rng);
    } else if (vx < -25.0f && ((rng() % 100) < 35)) {
        std::uniform_real_distribution<float> ySide(0.0f, static_cast<float>(g_canvasHeight) * 0.75f);
        item.x = static_cast<float>(g_canvasWidth) + 20.0f;
        item.y = ySide(rng);
    } else if (vx > 25.0f && ((rng() % 100) < 35)) {
        std::uniform_real_distribution<float> ySide(0.0f, static_cast<float>(g_canvasHeight) * 0.75f);
        item.x = -20.0f;
        item.y = ySide(rng);
    } else {
        std::uniform_real_distribution<float> xTop(-60.0f, static_cast<float>(g_canvasWidth) + 60.0f);
        item.x = xTop(rng);
        item.y = -24.0f;
    }

    if (kind == ObjectKind::Rain) {
        item.size = 13.0f;
        item.spin = 0.0f;
    } else if (kind == ObjectKind::Hail) {
        item.size = 5.0f + (rng() % 30) * 0.1f;
        item.spin = ((rng() % 100) - 50) * 0.05f;
    } else {
        float depth = static_cast<float>(rng() % 1000) / 1000.0f;
        item.size = 2.4f + depth * depth * 5.2f;
        item.spin = 0.0f;
    }
}

void CleanupSurface(CachedSurface& surf) {
    if (surf.flakeSprite) {
        surf.flakeSprite->Release();
        surf.flakeSprite = nullptr;
    }
    if (surf.sharedBrush) {
        surf.sharedBrush->Release();
        surf.sharedBrush = nullptr;
    }
    if (surf.rt) {
        surf.rt->Release();
        surf.rt = nullptr;
    }
    if (surf.memDC) {
        if (surf.oldBmp) {
            SelectObject(surf.memDC, surf.oldBmp);
            surf.oldBmp = nullptr;
        }
        DeleteDC(surf.memDC);
        surf.memDC = nullptr;
    }
    if (surf.dib) {
        DeleteObject(surf.dib);
        surf.dib = nullptr;
    }
    surf.width = 0;
    surf.height = 0;
}

bool RebuildFlakeSprite(CachedSurface& surf, D2D1_COLOR_F snowCol, bool hasCustomColor) {
    if (!surf.rt) return false;
    if (surf.flakeSprite) {
        surf.flakeSprite->Release();
        surf.flakeSprite = nullptr;
    }

    auto shade = [&](float r, float g, float b, float a) -> D2D1_COLOR_F {
        if (!hasCustomColor) return D2D1::ColorF(r, g, b, a);
        float br = (r + g + b) / 3.0f;
        return D2D1::ColorF(snowCol.r * br, snowCol.g * br, snowCol.b * br, a);
    };

    constexpr UINT32 kSize = 32;
    ID2D1BitmapRenderTarget* bmpRt = nullptr;
    if (FAILED(surf.rt->CreateCompatibleRenderTarget(D2D1::SizeF(static_cast<float>(kSize), static_cast<float>(kSize)), &bmpRt)) || !bmpRt) {
        return false;
    }

    D2D1_GRADIENT_STOP stops[4] = {
        {0.00f, shade(1.00f, 1.00f, 1.00f, 1.00f)},
        {0.42f, shade(0.98f, 0.99f, 1.00f, 0.95f)},
        {0.62f, shade(0.75f, 0.82f, 0.95f, 0.36f)},
        {1.00f, shade(0.60f, 0.70f, 0.85f, 0.00f)}
    };
    ID2D1GradientStopCollection* stopCol = nullptr;
    ID2D1RadialGradientBrush* radBrush = nullptr;
    if (SUCCEEDED(bmpRt->CreateGradientStopCollection(stops, 4, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCol)) && stopCol) {
        bmpRt->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(D2D1::Point2F(16.0f, 16.0f), D2D1::Point2F(0.0f, 0.0f), 16.0f, 16.0f),
            stopCol, &radBrush);
    }

    bmpRt->BeginDraw();
    bmpRt->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    if (radBrush) {
        bmpRt->FillRectangle(D2D1::RectF(0.0f, 0.0f, 32.0f, 32.0f), radBrush);
    }
    bmpRt->EndDraw();

    if (radBrush) radBrush->Release();
    if (stopCol) stopCol->Release();

    bmpRt->GetBitmap(&surf.flakeSprite);
    bmpRt->Release();
    surf.spriteColor = snowCol;
    return surf.flakeSprite != nullptr;
}

bool EnsureSurface(CachedSurface& surf, int width, int height, D2D1_COLOR_F snowCol, bool hasCustomSnow) {
    if (!g_d2dFactory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory);
    }
    if (!g_d2dFactory) return false;

    if (!surf.rt || !surf.memDC || !surf.dib || surf.width != width || surf.height != height) {
        CleanupSurface(surf);

        HDC screenDC = GetDC(nullptr);
        surf.memDC = CreateCompatibleDC(screenDC);
        ReleaseDC(nullptr, screenDC);
        if (!surf.memDC) return false;

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        surf.dib = CreateDIBSection(surf.memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!surf.dib) {
            CleanupSurface(surf);
            return false;
        }

        surf.oldBmp = SelectObject(surf.memDC, surf.dib);
        surf.width = width;
        surf.height = height;

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        if (FAILED(g_d2dFactory->CreateDCRenderTarget(&props, &surf.rt)) || !surf.rt) {
            CleanupSurface(surf);
            return false;
        }
        surf.rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &surf.sharedBrush);
    }

    if (!surf.flakeSprite ||
        std::abs(surf.spriteColor.r - snowCol.r) > 0.01f ||
        std::abs(surf.spriteColor.g - snowCol.g) > 0.01f ||
        std::abs(surf.spriteColor.b - snowCol.b) > 0.01f) {
        RebuildFlakeSprite(surf, snowCol, hasCustomSnow);
    }
    return surf.sharedBrush != nullptr;
}

void DrawItemShape(CachedSurface& surf, const Settings& cfg,
                   ObjectKind kind, float x, float y, float size, float angle,
                   float vx, float alpha) {
    ID2D1SolidColorBrush* brush = surf.sharedBrush;
    if (kind == ObjectKind::Rain) {
        brush->SetColor(cfg.colorRain);
        brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
        surf.rt->DrawLine(D2D1::Point2F(x, y), D2D1::Point2F(x + vx * 0.02f, y + size), brush, 1.5f);
        brush->SetOpacity(1.0f);
        return;
    }

    if (kind == ObjectKind::Snow) {
        if (surf.flakeSprite) {
            float r = size * 1.35f;
            surf.rt->DrawBitmap(
                surf.flakeSprite,
                D2D1::RectF(x - r, y - r, x + r, y + r),
                std::clamp(alpha, 0.05f, 1.0f),
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            return;
        }
        brush->SetColor(cfg.colorSnow);
        brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
        surf.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), size * 0.5f, size * 0.5f), brush);
        brush->SetOpacity(1.0f);
        return;
    }

    D2D1::Matrix3x2F oldXform;
    surf.rt->GetTransform(&oldXform);
    surf.rt->SetTransform(D2D1::Matrix3x2F::Rotation(angle * 57.29578f, D2D1::Point2F(x, y)) * oldXform);
    brush->SetColor(cfg.colorHail);
    brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
    surf.rt->FillRectangle(D2D1::RectF(x - size * 0.45f, y - size * 0.45f, x + size * 0.45f, y + size * 0.45f), brush);
    brush->SetOpacity(1.0f);
    surf.rt->SetTransform(oldXform);
}

void PresentSurface(HWND hwnd, CachedSurface& surf) {
    if (!hwnd || !IsWindow(hwnd) || !surf.memDC) return;
    POINT ptDst = {g_canvasLeft, g_canvasTop};
    SIZE sizeWnd = {g_canvasWidth, g_canvasHeight};
    POINT ptSrc = {0, 0};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(hwnd, nullptr, &ptDst, &sizeWnd, surf.memDC, &ptSrc, 0, &blend, ULW_ALPHA);
}

void DrawVerticalGradientTint(ID2D1DCRenderTarget* rt, D2D1_COLOR_F topCol, D2D1_COLOR_F botCol,
                              float topY, float botY, float factor) {
    topCol.a = std::clamp(topCol.a * factor, 0.0f, 0.85f);
    botCol.a = std::clamp(botCol.a * factor, 0.0f, 0.85f);
    if (topCol.a <= 0.003f && botCol.a <= 0.003f) return;

    D2D1_GRADIENT_STOP stops[2] = {
        {0.0f, topCol},
        {1.0f, botCol}
    };
    ID2D1GradientStopCollection* stopCol = nullptr;
    if (FAILED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCol)) || !stopCol) {
        return;
    }
    ID2D1LinearGradientBrush* grad = nullptr;
    if (SUCCEEDED(rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(0.0f, topY), D2D1::Point2F(0.0f, botY)),
            stopCol, &grad)) && grad) {
        rt->FillRectangle(D2D1::RectF(0.0f, topY, static_cast<float>(g_canvasWidth), botY), grad);
        grad->Release();
    }
    stopCol->Release();
}

void DrawSmoothRainbow(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg, float strength, double timeSec, bool forceDirty) {
    if (cfg.rainbowGeom.radius <= 0 || strength <= 0.004f) return;
    RefreshArcCache(g_rainbowCache, cfg.rainbowGeom, forceDirty);
    if (g_rainbowCache.radius < 4.0f) return;

    const D2D1_COLOR_F spectrum[6] = {
        D2D1::ColorF(1.0f, 0.22f, 0.28f, 0.24f),
        D2D1::ColorF(1.0f, 0.58f, 0.15f, 0.24f),
        D2D1::ColorF(1.0f, 0.90f, 0.20f, 0.24f),
        D2D1::ColorF(0.20f, 0.85f, 0.42f, 0.24f),
        D2D1::ColorF(0.22f, 0.62f, 1.0f, 0.24f),
        D2D1::ColorF(0.60f, 0.34f, 0.96f, 0.24f)
    };

    brush->SetOpacity(1.0f);
    float shimmer = 0.92f + 0.08f * static_cast<float>(std::sin(timeSec * 1.4));
    for (int i = 0; i < 6; ++i) {
        D2D1_COLOR_F c = cfg.hasCustomRainbowColor ? cfg.colorRainbow : spectrum[i];
        float bandFade = 1.0f - std::abs(i - 2.5f) * 0.14f;
        float alpha = std::clamp(c.a * strength * bandFade * shimmer, 0.0f, 0.65f);
        if (alpha <= 0.003f) continue;

        float r = g_rainbowCache.radius - i * (g_rainbowCache.bandThickness * 0.85f);
        if (r <= 2.0f) break;

        brush->SetColor(D2D1::ColorF(c.r, c.g, c.b, alpha));
        rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_rainbowCache.cx, g_rainbowCache.cy), r, r), brush, g_rainbowCache.bandThickness);
    }
}

void DrawSmoothHalo(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg, float strength, double timeSec, bool forceDirty) {
    if (cfg.haloGeom.radius <= 0 || strength <= 0.004f) return;
    RefreshArcCache(g_haloCache, cfg.haloGeom, forceDirty);
    if (g_haloCache.radius < 4.0f) return;

    D2D1_COLOR_F base = cfg.hasCustomHaloColor ? cfg.colorHalo : D2D1::ColorF(1.0f, 0.96f, 0.84f, 0.30f);
    float pulse = 0.94f + 0.06f * static_cast<float>(std::sin(timeSec * 1.8));

    D2D1_COLOR_F innerCol = base;
    innerCol.a = std::clamp(base.a * 0.35f * strength * pulse, 0.0f, 0.5f);
    D2D1_COLOR_F outerCol = base;
    outerCol.a = 0.0f;

    D2D1_GRADIENT_STOP stops[2] = {
        {0.0f, innerCol},
        {1.0f, outerCol}
    };
    ID2D1GradientStopCollection* stopsCol = nullptr;
    if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopsCol)) && stopsCol) {
        ID2D1RadialGradientBrush* radBrush = nullptr;
        if (SUCCEEDED(rt->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(D2D1::Point2F(g_haloCache.cx, g_haloCache.cy), D2D1::Point2F(0.0f, 0.0f), g_haloCache.radius, g_haloCache.radius),
                stopsCol, &radBrush)) && radBrush) {
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_haloCache.cx, g_haloCache.cy), g_haloCache.radius, g_haloCache.radius), radBrush);
            radBrush->Release();
        }
        stopsCol->Release();
    }

    float ringAlpha = std::clamp(base.a * 0.85f * strength * pulse, 0.0f, 0.75f);
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, ringAlpha));
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_haloCache.cx, g_haloCache.cy), g_haloCache.radius, g_haloCache.radius),
                    brush, std::max(2.0f, g_haloCache.radius * 0.025f));
}

void DrawSmoothAurora(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    D2D1_COLOR_F base = cfg.hasCustomAuroraColor ? cfg.colorAurora : D2D1::ColorF(0.20f, 0.94f, 0.62f, 0.28f);
    float bandHeight = std::max(90.0f, static_cast<float>(g_canvasHeight) * 0.22f);

    brush->SetOpacity(1.0f);
    for (int ribbon = 0; ribbon < 2; ++ribbon) {
        D2D1_COLOR_F cTop = base;
        cTop.a = std::clamp(base.a * (ribbon == 0 ? 1.0f : 0.65f) * strength, 0.0f, 0.65f);
        D2D1_COLOR_F cBot = cTop;
        cBot.a = 0.0f;

        float y0 = (ribbon == 0) ? 16.0f : 44.0f;
        float y1 = y0 + bandHeight;
        DrawVerticalGradientTint(rt, cTop, cBot, y0, y1, 1.0f);

        float waveAlpha = std::clamp(cTop.a * 1.15f, 0.0f, 0.75f);
        brush->SetColor(D2D1::ColorF(cTop.r, cTop.g, cTop.b, waveAlpha));
        float prevX = 0.0f;
        float prevY = y0 + static_cast<float>(std::sin(timeSec * 1.1 + ribbon)) * 14.0f;
        for (float x = 24.0f; x <= static_cast<float>(g_canvasWidth); x += 24.0f) {
            float curY = y0 + static_cast<float>(std::sin(x * 0.0045f + timeSec * (1.1 + ribbon * 0.3))) * 18.0f;
            rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 3.0f);
            prevX = x;
            prevY = curY;
        }
    }
}

void DrawSmoothMirage(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg, float strength, float bottomRelY, double timeSec) {
    if (strength <= 0.004f) return;
    float botY = bottomRelY;
    float bandH = std::max(50.0f, static_cast<float>(g_canvasHeight) * 0.14f);
    float topY = std::max(0.0f, botY - bandH);

    D2D1_COLOR_F base = cfg.hasCustomMirageColor ? cfg.colorMirage : D2D1::ColorF(1.0f, 0.88f, 0.68f, 0.25f);
    D2D1_COLOR_F topCol = base;
    topCol.a = 0.0f;
    D2D1_COLOR_F botCol = base;
    botCol.a = std::clamp(base.a * strength, 0.0f, 0.55f);

    DrawVerticalGradientTint(rt, topCol, botCol, topY, botY, 1.0f);

    float shAlpha = std::clamp(base.a * 0.75f * strength, 0.0f, 0.5f);
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, shAlpha));
    for (int line = 0; line < 3; ++line) {
        float baseLineY = topY + bandH * (0.35f + line * 0.25f);
        float prevX = 0.0f;
        float prevY = baseLineY + static_cast<float>(std::sin(timeSec * 2.4 + line * 1.7)) * 3.0f;
        for (float x = 20.0f; x <= static_cast<float>(g_canvasWidth); x += 20.0f) {
            float curY = baseLineY + static_cast<float>(std::sin(x * 0.012f + timeSec * 2.4 + line * 1.7)) * 3.5f;
            rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 1.5f);
            prevX = x;
            prevY = curY;
        }
    }
}

void DrawBranchedLightning(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                         const LightningFlash& flash, float maxHeight, float alpha) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(brush->GetColor().r, brush->GetColor().g, brush->GetColor().b, alpha));

    float curX = flash.xStart;
    float curY = 0.0f;
    int segs = 14;
    float stepY = std::max(20.0f, maxHeight * 0.72f / segs);
    for (int i = 0; i < segs; ++i) {
        float t = static_cast<float>(i + 1) / segs;
        float nextX = flash.xStart + (flash.xEnd - flash.xStart) * t + ((i % 2 == 0) ? 16.0f : -16.0f);
        float nextY = curY + stepY;
        rt->DrawLine(D2D1::Point2F(curX, curY), D2D1::Point2F(nextX, nextY), brush, 2.8f);

        if (i == 4 || i == 8) {
            float branchDir = (i == 4) ? 1.0f : -1.0f;
            float b1x = nextX + branchDir * 28.0f;
            float b1y = nextY + stepY * 1.4f;
            rt->DrawLine(D2D1::Point2F(nextX, nextY), D2D1::Point2F(b1x, b1y), brush, 1.6f);
            rt->DrawLine(D2D1::Point2F(b1x, b1y), D2D1::Point2F(b1x + branchDir * 16.0f, b1y + stepY * 1.1f), brush, 1.1f);
        }

        curX = nextX;
        curY = nextY;
    }
}

void RenderFrame(const Settings& cfg, const GeneralEffectsState& fx, int bottomLimitY,
                 float precipVx, double timeSec, bool forceArcDirty) {
    if (!g_overlayHwnd || !EnsureSurface(g_surface, g_canvasWidth, g_canvasHeight, cfg.colorSnow, cfg.hasCustomSnowColor)) return;

    float bottomRelY = static_cast<float>(bottomLimitY - g_canvasTop);

    RECT bindRect = {0, 0, g_canvasWidth, g_canvasHeight};
    g_surface.rt->BindDC(g_surface.memDC, &bindRect);
    g_surface.rt->BeginDraw();
    g_surface.rt->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

    float k = static_cast<float>(cfg.effectsIntensity) / 25.0f;
    float hFloat = static_cast<float>(g_canvasHeight);

    if (fx.clear) {
        D2D1_COLOR_F cTop = cfg.hasCustomClearColor ? cfg.colorClear : D2D1::ColorF(1.0f, 0.94f, 0.72f, 0.16f);
        D2D1_COLOR_F cBot = cTop; cBot.a *= 0.2f;
        DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k);
    }
    if (fx.partlyCloudy) {
        D2D1_COLOR_F cTop = cfg.hasCustomPartlyCloudyColor ? cfg.colorPartlyCloudy : D2D1::ColorF(0.90f, 0.93f, 0.97f, 0.18f);
        D2D1_COLOR_F cBot = cTop; cBot.a *= 0.35f;
        DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k);
    }
    if (fx.overcast) {
        D2D1_COLOR_F cTop = cfg.hasCustomOvercastColor ? cfg.colorOvercast : D2D1::ColorF(0.50f, 0.55f, 0.62f, 0.28f);
        D2D1_COLOR_F cBot = cTop; cBot.a *= 0.65f;
        DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k);
    }
    if (fx.dust) {
        D2D1_COLOR_F cTop = cfg.hasCustomDustColor ? cfg.colorDust : D2D1::ColorF(0.84f, 0.68f, 0.45f, 0.10f);
        D2D1_COLOR_F cBot = cfg.hasCustomDustColor ? cfg.colorDust : D2D1::ColorF(0.84f, 0.68f, 0.45f, 0.26f);
        DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k);
    }
    if (fx.fog) {
        D2D1_COLOR_F cTop = cfg.hasCustomFogColor ? cfg.colorFog : D2D1::ColorF(0.84f, 0.88f, 0.92f, 0.14f);
        D2D1_COLOR_F cBot = cfg.hasCustomFogColor ? cfg.colorFog : D2D1::ColorF(0.84f, 0.88f, 0.92f, 0.34f);
        DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k);
    }
    if (fx.smog) {
        D2D1_COLOR_F cTop = cfg.hasCustomSmogColor ? cfg.colorSmog : D2D1::ColorF(0.54f, 0.48f, 0.40f, 0.16f);
        D2D1_COLOR_F cBot = cfg.hasCustomSmogColor ? cfg.colorSmog : D2D1::ColorF(0.54f, 0.48f, 0.40f, 0.36f);
        DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k);
    }
    if (fx.aurora) DrawSmoothAurora(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec);
    if (fx.mirage) DrawSmoothMirage(g_surface.rt, g_surface.sharedBrush, cfg, k, bottomRelY, timeSec);
    if (fx.rainbow) DrawSmoothRainbow(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec, forceArcDirty);
    if (fx.halo) DrawSmoothHalo(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec, forceArcDirty);

    if (fx.thunderstorm && g_lightning.timer > 0.0f) {
        D2D1_COLOR_F tc = cfg.hasCustomThunderstormColor ? cfg.colorThunderstorm : D2D1::ColorF(0.86f, 0.93f, 1.0f, 0.95f);
        float tAlpha = std::clamp(g_lightning.timer * 4.2f * std::max(0.25f, k), 0.0f, 1.0f);
        g_surface.sharedBrush->SetColor(D2D1::ColorF(tc.r, tc.g, tc.b, tAlpha));
        DrawBranchedLightning(g_surface.rt, g_surface.sharedBrush, g_lightning, bottomRelY, tAlpha);
    }

    for (const auto& p : g_falling) {
        DrawItemShape(g_surface, cfg, p.kind, p.x, p.y, p.size, p.angle, precipVx, p.alpha);
    }

    if (FAILED(g_surface.rt->EndDraw())) {
        CleanupSurface(g_surface);
        return;
    }

    PresentSurface(g_overlayHwnd, g_surface);
}

float ComputeHorizontalSpeedPxPerSec(FallDirMode dirMode, double mpsSpeed, int forecastSign, double forecastWindKmh) {
    float basePx = static_cast<float>(mpsSpeed) * 26.0f;
    if (dirMode == FallDirMode::Down) return 0.0f;
    if (dirMode == FallDirMode::DownLeft) return -basePx;
    if (dirMode == FallDirMode::DownRight) return basePx;

    if (forecastSign == 0 || forecastWindKmh <= 0.2) return 0.0f;
    float windFactor = std::clamp(static_cast<float>(forecastWindKmh) / 20.0f, 0.25f, 2.5f);
    return static_cast<float>(forecastSign) * basePx * windFactor;
}

void WaitWithMessagePump(DWORD ms) {
    DWORD startTick = GetTickCount();
    while (g_running.load()) {
        DWORD elapsed = GetTickCount() - startTick;
        if (elapsed >= ms) break;
        DWORD remain = ms - elapsed;
        DWORD res = MsgWaitForMultipleObjects(0, nullptr, FALSE, remain, QS_ALLINPUT);
        if (res == WAIT_OBJECT_0) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        } else {
            break;
        }
    }
}

void RenderWorkerLoop() {
    HMODULE modInstance = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&OverlayWndProc),
        &modInstance);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = modInstance;
    wc.lpszClassName = kOverlayWndClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        return;
    }

    std::mt19937 rng(static_cast<unsigned>(GetTickCount64()));
    auto startTime = std::chrono::steady_clock::now();
    auto lastTime = startTime;
    int zOrderTick = 0;

    while (g_running.load()) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        Settings cfg;
        bool hasForecastP = false;
        ObjectKind forecastP = ObjectKind::Rain;
        GeneralEffectsState forecastFx;
        double forecastWindSpd = 0.0;
        int forecastWindSign = 0;
        bool forceArcDirty = false;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            cfg = g_settings;
            hasForecastP = g_hasForecastPrecip;
            forecastP = g_forecastPrecip;
            forecastFx = g_forecastEffects;
            forecastWindSpd = g_forecastWindSpeedKmh;
            forecastWindSign = g_forecastWindDirSign;
            forceArcDirty = g_arcCacheDirty;
            g_arcCacheDirty = false;
        }

        UpdateCanvasGeometry(cfg);

        if ((cfg.pauseOnFullscreen && IsFullscreenAppActive()) || IsDesktopOccluded()) {
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) {
                ShowWindow(g_overlayHwnd, SW_HIDE);
            }
            WaitWithMessagePump(250);
            lastTime = std::chrono::steady_clock::now();
            continue;
        }

        DWORD exStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
        if (!g_overlayHwnd) {
            g_overlayHwnd = CreateWindowExW(
                exStyle, wc.lpszClassName, L"DesktopWeatherOverlay", WS_POPUP,
                g_canvasLeft, g_canvasTop, g_canvasWidth, g_canvasHeight,
                nullptr, nullptr, wc.hInstance, nullptr);
            PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode);
        }

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        double timeSec = std::chrono::duration<double>(now - startTime).count();
        lastTime = now;
        dt = std::clamp(dt, 0.001f, 0.05f);

        bool activePrecip = false;
        ObjectKind precipKind = ObjectKind::Rain;
        if (cfg.precipitationMode == PrecipMode::Rain) { activePrecip = true; precipKind = ObjectKind::Rain; }
        else if (cfg.precipitationMode == PrecipMode::Snow) { activePrecip = true; precipKind = ObjectKind::Snow; }
        else if (cfg.precipitationMode == PrecipMode::Hail) { activePrecip = true; precipKind = ObjectKind::Hail; }
        else if (cfg.precipitationMode == PrecipMode::Auto && hasForecastP) { activePrecip = true; precipKind = forecastP; }

        auto resolveEffect = [&](TriStateMode mode, bool autoVal) -> bool {
            if (mode == TriStateMode::On) return true;
            if (mode == TriStateMode::Off) return false;
            return autoVal;
        };
        GeneralEffectsState activeFx;
        activeFx.clear = resolveEffect(cfg.effectClear, forecastFx.clear);
        activeFx.partlyCloudy = resolveEffect(cfg.effectPartlyCloudy, forecastFx.partlyCloudy);
        activeFx.overcast = resolveEffect(cfg.effectOvercast, forecastFx.overcast);
        activeFx.dust = resolveEffect(cfg.effectDust, forecastFx.dust);
        activeFx.fog = resolveEffect(cfg.effectFog, forecastFx.fog);
        activeFx.smog = resolveEffect(cfg.effectSmog, forecastFx.smog);
        activeFx.thunderstorm = resolveEffect(cfg.effectThunderstorm, forecastFx.thunderstorm);
        activeFx.aurora = resolveEffect(cfg.effectAurora, forecastFx.aurora);
        activeFx.mirage = resolveEffect(cfg.effectMirage, forecastFx.mirage);
        activeFx.rainbow = resolveEffect(cfg.effectRainbow, forecastFx.rainbow);
        activeFx.halo = resolveEffect(cfg.effectHalo, forecastFx.halo);

        bool anyFxActive = activeFx.clear || activeFx.partlyCloudy || activeFx.overcast ||
                           activeFx.dust || activeFx.fog || activeFx.smog ||
                           activeFx.thunderstorm || activeFx.aurora || activeFx.mirage ||
                           activeFx.rainbow || activeFx.halo;

        bool hasAnyState = activePrecip || anyFxActive || !g_falling.empty();
        if (!hasAnyState) {
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) {
                ShowWindow(g_overlayHwnd, SW_HIDE);
            }
            WaitWithMessagePump(200);
            lastTime = std::chrono::steady_clock::now();
            continue;
        }
        if (g_overlayHwnd) {
            if (!IsWindowVisible(g_overlayHwnd)) {
                ShowWindow(g_overlayHwnd, SW_SHOWNA);
            }
            if (++zOrderTick >= 45) {
                zOrderTick = 0;
                PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode);
            }
        }

        if (activeFx.thunderstorm) {
            if (g_lightning.timer > 0.0f) {
                g_lightning.timer = std::max(0.0f, g_lightning.timer - dt);
            } else if ((static_cast<float>(rng() % 10000) / 10000.0f) < 0.35f * dt) {
                g_lightning.timer = 0.22f;
                g_lightning.xStart = static_cast<float>(100 + rng() % std::max(120, g_canvasWidth - 200));
                g_lightning.xEnd = g_lightning.xStart + static_cast<float>((rng() % 160) - 80);
            }
        } else {
            g_lightning.timer = 0.0f;
        }

        float precipVx = ComputeHorizontalSpeedPxPerSec(cfg.precipFallDirection, cfg.precipFallSpeedMps, forecastWindSign, forecastWindSpd);
        float precipVy = static_cast<float>(cfg.precipFallSpeedMps) * 42.0f;

        size_t targetP = activePrecip
            ? static_cast<size_t>((g_canvasWidth / 1000.0) * cfg.precipIntensity * 1.4)
            : 0;

        size_t prevCount = g_falling.size();
        if (g_falling.size() != targetP) {
            g_falling.resize(targetP);
        }
        for (size_t i = 0; i < targetP; ++i) {
            if (i >= prevCount || g_falling[i].kind != precipKind) {
                SpawnFallingItem(g_falling[i], rng, precipKind, precipVx, true);
            }
        }

        int bottomLimitY = ComputeBottomLimitY(cfg);

        for (size_t i = 0; i < g_falling.size(); ++i) {
            FallingItem& p = g_falling[i];
            p.swayPhase += dt * 3.0f;
            p.angle += p.spin * dt * 20.0f;

            float sway = (p.kind == ObjectKind::Rain) ? 0.0f : std::sin(p.swayPhase) * 18.0f;
            p.x += (precipVx + sway) * dt;
            p.y += precipVy * dt;

            if (p.y > g_canvasHeight + 30.0f || p.x < -140.0f || p.x > g_canvasWidth + 140.0f) {
                SpawnFallingItem(p, rng, p.kind, precipVx, false);
            }
        }

        RenderFrame(cfg, activeFx, bottomLimitY, precipVx, timeSec, forceArcDirty);

        bool hasAnimatedMotion = !g_falling.empty() || activeFx.thunderstorm || activeFx.aurora || activeFx.mirage;
        DWORD sleepMs = hasAnimatedMotion ? static_cast<DWORD>(1000 / cfg.targetFps) : 120;
        WaitWithMessagePump(sleepMs);
    }

    if (g_overlayHwnd) {
        DestroyWindow(g_overlayHwnd);
        g_overlayHwnd = nullptr;
    }
    CleanupSurface(g_surface);
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

} // namespace

BOOL WhTool_ModInit() {
    LoadSettingsFromWh();
    g_running.store(true);
    g_weatherWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_weatherThread.emplace(WeatherWorkerLoop);
    g_renderThread.emplace(RenderWorkerLoop);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettingsFromWh();
}

void WhTool_ModUninit() {
    g_running.store(false);
    if (g_weatherWakeEvent) {
        SetEvent(g_weatherWakeEvent);
    }
    if (g_weatherThread && g_weatherThread->joinable()) {
        g_weatherThread->join();
    }
    g_weatherThread.reset();

    if (g_renderThread && g_renderThread->joinable()) {
        g_renderThread->join();
    }
    g_renderThread.reset();

    if (g_weatherWakeEvent) {
        CloseHandle(g_weatherWakeEvent);
        g_weatherWakeEvent = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#running-a-tool-mod-in-windhawkexe
////////////////////////////////////////////////////////////////////////////////

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutexW failed");
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
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook),
                           nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR windhawkPath[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, windhawkPath, ARRAYSIZE(windhawkPath))) {
        Wh_Log(L"GetModuleFileNameW failed");
        return;
    }

    WCHAR commandLine[MAX_PATH * 2];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", windhawkPath, WH_MOD_ID);

    STARTUPINFO si = {sizeof(si)};
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (!CreateProcessInternalW(nullptr, windhawkPath, commandLine, nullptr,
                                nullptr, FALSE, 0, nullptr, nullptr, &si,
                                &pi)) {
        Wh_Log(L"CreateProcessInternalW failed");
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