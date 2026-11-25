// ==WindhawkMod==
// @id              alembiq-weather-desktop-overlay
// @name            Desktop Weather & Particles Overlay
// @description     Weather effects and seasonal particles under desktop icons with window accumulation and shake physics.
// @version         1.4.4
// @author          alembiq
// @github          https://github.com/alembiqspace
// @include         explorer.exe
// @compilerOptions -ld2d1 -ldwrite -lwininet -luser32 -lgdi32 -lshell32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
A lightweight **Direct2D** weather and seasonal particle overlay for the Windows desktop. It brings live weather visuals, seasonal falling elements, and atmospheric layers to your desktop without cluttering icons or windows.

### Why Use It
- **Alive Desktop Atmosphere:** Match your desktop environment to real-world weather or switch modes manually whenever you want a specific mood.
- **Safe Desktop Integration:** Runs as a standalone Windhawk tool overlay without hooking `explorer.exe`, preventing duplicate overlays or shell instability.
- **Flexible Customization:** Adjust intensity, speed, scale, colors, screen bounds, and frame rate to fit multi-monitor setups and personal aesthetics.

### Features
- **Precipitation & Seasonal Layers:** Supports rain (with subtle bottom puddles), snow (with bottom accumulation and lifetime fade), hail, plus date-based or manual seasonal layers (autumn leaves, extra snow, and petals).
- **Atmospheric Effects:** Combines clear sunlight glow, partly cloudy, overcast, dust, fog, smog, thunderstorm flashes, aurora, mirage, and configurable rainbow and halo arcs.
- **Weather Sources:** Pulls live data from **Open-Meteo**, a custom JSON API endpoint, or a local file, with `auto` / `on` / `off` control per effect.
- **Cursor Interaction:** Choose `none`, `clear` (erase settled particles on contact), or `push` (gently nudge settled elements) with an adjustable radius when interacting directly with the desktop.

### Network & Privacy
- **No Unsolicited Requests by Default:** `[Weather] Latitude/Longitude` (`coords`) is empty by default. No external network requests are made until you configure valid coordinates (or a custom JSON API URL) and have at least one forecast-driven setting (`precipitationMode`, `precipFallDirection`, `seasonalFallDirection`, or any general effect) in `auto` mode.
- **What Is Sent & When:** When `primarySource` is set to `open_meteo` with valid `lat/lon` coordinates and at least one forecast-driven setting uses `auto`, the mod sends an HTTPS `GET` request to `https://api.open-meteo.com/v1/forecast` containing only your configured coordinates and requested weather fields (`weather_code`, `wind_speed_10m`, `wind_direction_10m`, `temperature_2m`, `cloud_cover`, `visibility`, `is_day`). Requests run on startup, on settings changes, every `[Weather] Update interval (min)` (default: `30` minutes), and every `60` seconds after a failed attempt.
- **How to Disable:** Set `[Weather] Primary source` (`primarySource`) to `off`, leave `[Weather] Latitude/Longitude` (`coords`) empty, or switch all forecast-driven modes (including `precipFallDirection` and `seasonalFallDirection`) away from `auto`.
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
  $description: "Sets bottom reference line for fallen items/puddles. Allowed: above_taskbar, screen_bottom, custom_offset. Default: above_taskbar."
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
- cursorAction: clear
  $name: "[System] Cursor action"
  $description: "none = ignore cursor; clear = erase settled items on touch; push = gently nudge or interact with settled items."
  $options:
    - none: None (ignore cursor)
    - clear: Clear (erase settled items on touch)
    - push: Push (gently nudge settled items)
- cursorRadiusPx: 95
  $name: "[System] Cursor radius (px)"
  $description: "Cursor action radius in pixels (10 to 500). Invalid/out-of-range resets to 95."

# --- 2. ACCUMULATION ---
- enableBottomAccumulation: true
  $name: "[Accum] Bottom accumulation"
  $description: "Enables/disables accumulation at the bottom boundary. When off, nothing accumulates even with side wind."

# --- 3. WEATHER SOURCES & SEASONS ---
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
- seasonsRange: "01.03-31.05/01.06-31.08/01.09-30.11/01.12-29.02"
  $name: "[Seasons] Spring/Summer/Autumn/Winter (DD.MM-DD.MM)"
  $description: "Four DD.MM-DD.MM ranges separated by '/'. Invalid format resets to 01.03-31.05/01.06-31.08/01.09-30.11/01.12-29.02."

# --- 4. PRECIPITATION (RAIN / SNOW / HAIL) ---
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
- precipMaxAccumulated: 220
  $name: "[Precip] Max accumulated"
  $description: "Shared max accumulated count/cap height scale for precipitation (0..2000; 0 disables). Invalid/out-of-range resets to 220."
- precipLifetimeMin: "10.0"
  $name: "[Precip] Lifetime (min)"
  $description: "Shared auto-disappear/melt time for accumulated precipitation in minutes (-1 to 1440; -1 = never). Invalid resets to 10.0."

# --- 5. SEASONAL OBJECTS (LEAVES / EXTRA SNOW / PETALS) ---
- seasonalObjectMode: auto
  $name: "[SeasonObj] Mode"
  $description: "Seasonal falling objects mode. Allowed: off, auto, autumn_leaves, extra_snow, petals. Default: auto."
  $options:
    - off: Off
    - auto: Auto (season)
    - autumn_leaves: Autumn leaves
    - extra_snow: Snow (extra)
    - petals: Petals
- seasonalFallDirection: auto
  $name: "[SeasonObj] Fall direction"
  $description: "Fall direction for seasonal objects. Allowed: auto, down, down_left, down_right. Default: auto."
  $options:
    - auto: Auto (forecast)
    - down: Down
    - down_left: Down-left
    - down_right: Down-right
- seasonalFallSpeedMps: "2.2"
  $name: "[SeasonObj] Fall speed (m/s)"
  $description: "Fall speed in m/s (0.1 to 30.0). Invalid/out-of-range resets to 2.2."
- seasonalIntensity: 25
  $name: "[SeasonObj] Intensity (%)"
  $description: "Shared intensity for autumn leaves, extra snow, and petals (0..100). Any value outside 0..100 or invalid resets to 25."
- seasonalMaxAccumulated: 220
  $name: "[SeasonObj] Max accumulated"
  $description: "Shared max accumulated count for seasonal objects (0..2000; 0 disables). Invalid/out-of-range resets to 220."
- seasonalLifetimeMin: "15.0"
  $name: "[SeasonObj] Lifetime (min)"
  $description: "Shared auto-disappear time for accumulated seasonal objects in minutes (-1 to 1440; -1 = never). Invalid resets to 15.0."

# --- 6. GENERAL EFFECTS ---
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

# --- 7. SNOWBALLS & SCALE ---
- snowballMaxRadiusPx: 34
  $name: "[Snowballs] Max radius (px)"
  $description: "Max radius for bottom seasonal snow formations (8 to 120). Invalid/out-of-range resets to 34."
- particleScale: "1.0"
  $name: "[Objects] Size scale"
  $description: "Size multiplier for falling and fallen objects (0.3 to 4.0). Invalid resets to 1.0."

# --- 8. COLORS ---
- colorRain: ""
  $name: "[Color] Rain (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default sky-blue. Invalid HEX resets to default."
- colorSnow: ""
  $name: "[Color] Snow (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default natural white-blue snow. Invalid HEX resets to default."
- colorHail: ""
  $name: "[Color] Hail (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default ice-blue. Invalid HEX resets to default."
- colorAutumnLeaves: ""
  $name: "[Color] Autumn leaves (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default maple orange-amber. Invalid HEX resets to default."
- colorExtraSnow: ""
  $name: "[Color] Extra snow (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default snow-white. Invalid HEX resets to default."
- colorPetals: ""
  $name: "[Color] Petals (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default soft pink. Invalid HEX resets to default."
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
#include <ctime>
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

constexpr float kGravity = 2200.0f;
constexpr float kCapEdgeTaper = 8.0f;
constexpr float kSnowKernel[] = {0.10f, 0.25f, 0.50f, 0.80f, 1.00f, 0.80f, 0.50f, 0.25f, 0.10f};
constexpr float kSnowKernelSum = 4.30f;
constexpr size_t kMaxSnowmanBalls = 3;
constexpr wchar_t kOverlayWndClass[] = L"WindhawkDesktopWeatherOverlayClass";

enum class ObjectKind {
    Rain,
    Snow,
    Hail,
    AutumnLeaves,
    ExtraSnow,
    Petals
};

enum class PrecipMode {
    Off,
    Auto,
    Rain,
    Snow,
    Hail
};

enum class SeasonalMode {
    Off,
    Auto,
    AutumnLeaves,
    ExtraSnow,
    Petals
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

enum class CursorActionMode {
    None,
    Clear,
    Push
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

struct DayMonth {
    int day = 1;
    int month = 1;
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

struct MonitorBoundEntry {
    int left = 0;
    int right = 0;
    int bottomY = 0;
};

struct FrameBottomBounds {
    int fallbackBottomY = 0;
    bool useMonitors = false;
    std::vector<MonitorBoundEntry> monitors;
};

struct Settings {
    std::wstring layerAttachMode = L"auto";
    std::wstring bottomBoundaryMode = L"above_taskbar";
    int bottomOffsetPx = 0;
    std::wstring multiMonitorMode = L"primary";

    int targetFps = 60;
    bool pauseOnFullscreen = true;

    CursorActionMode cursorAction = CursorActionMode::Clear;
    int cursorRadiusPx = 95;

    bool enableBottomAccumulation = true;

    std::wstring primarySource = L"open_meteo";
    std::wstring fallbackSource = L"off";
    bool hasValidCoords = false;
    double latitude = 0.0;
    double longitude = 0.0;
    std::wstring customApiUrl;
    std::wstring localWeatherPath;
    int updateIntervalMin = 30;

    DayMonth springStart{1, 3};
    DayMonth springEnd{31, 5};
    DayMonth summerStart{1, 6};
    DayMonth summerEnd{31, 8};
    DayMonth autumnStart{1, 9};
    DayMonth autumnEnd{30, 11};
    DayMonth winterStart{1, 12};
    DayMonth winterEnd{29, 2};

    PrecipMode precipitationMode = PrecipMode::Auto;
    FallDirMode precipFallDirection = FallDirMode::Auto;
    double precipFallSpeedMps = 6.5;

    int precipIntensity = 25;
    int precipMaxAccumulated = 220;
    double precipLifetimeMin = 10.0;

    SeasonalMode seasonalObjectMode = SeasonalMode::Auto;
    FallDirMode seasonalFallDirection = FallDirMode::Auto;
    double seasonalFallSpeedMps = 2.2;

    int seasonalIntensity = 25;
    int seasonalMaxAccumulated = 220;
    double seasonalLifetimeMin = 15.0;

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

    int snowballMaxRadiusPx = 34;
    double particleScale = 1.0;

    bool hasCustomSnowColor = false;
    D2D1_COLOR_F colorRain = D2D1::ColorF(0.42f, 0.72f, 1.0f, 0.78f);
    D2D1_COLOR_F colorSnow = D2D1::ColorF(0.98f, 0.99f, 1.0f, 0.96f);
    D2D1_COLOR_F colorHail = D2D1::ColorF(0.76f, 0.90f, 1.0f, 0.95f);
    D2D1_COLOR_F colorAutumnLeaves = D2D1::ColorF(0.88f, 0.44f, 0.12f, 0.94f);
    D2D1_COLOR_F colorExtraSnow = D2D1::ColorF(0.94f, 0.97f, 1.0f, 0.92f);
    D2D1_COLOR_F colorPetals = D2D1::ColorF(1.0f, 0.74f, 0.84f, 0.92f);

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

struct SnowColumn {
    float h = 0.0f;
};

struct SnowCap {
    uint32_t seed = 0;
    int offset = 0;
    float squash = 0.0f;
    float squashVel = 0.0f;
    std::vector<SnowColumn> cols;
};

struct SnowChunk {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float area = 4.0f;
    float size = 4.0f;
};

struct FallingItem {
    ObjectKind kind = ObjectKind::Snow;
    bool isSeasonalLayer = false;
    float x = 0.0f;
    float y = 0.0f;
    float size = 6.0f;
    float angle = 0.0f;
    float spin = 0.0f;
    float flip = 0.0f;
    float flipSpeed = 2.5f;
    float swayPhase = 0.0f;
    float alpha = 0.9f;
    int shadeIdx = 0;
};

struct FallenItem {
    ObjectKind kind = ObjectKind::Snow;
    bool isSeasonalLayer = false;
    float relX = 0.0f;
    float relY = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float size = 6.0f;
    float angle = 0.0f;
    float flip = 0.8f;
    float alpha = 0.92f;
    float ageSec = 0.0f;
    int shadeIdx = 0;
};

struct BottomSnowball {
    ObjectKind kind = ObjectKind::ExtraSnow;
    int slot = 0;
    float x = 0.0f;
    float vx = 0.0f;
    float radius = 8.0f;
    float lift = 0.0f;
    float liftVel = 0.0f;
    float squash = 0.0f;
    float squashVel = 0.0f;
    float alpha = 0.96f;
    float ageSec = 0.0f;
    bool stacked = false;
    bool full = false;
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

constexpr float kLeafPalette[][3] = {
    {0.93f, 0.49f, 0.13f},
    {0.82f, 0.24f, 0.12f},
    {0.95f, 0.73f, 0.20f},
    {0.68f, 0.36f, 0.14f},
    {0.88f, 0.38f, 0.11f},
};
constexpr float kPetalPalette[][3] = {
    {1.00f, 0.80f, 0.87f},
    {0.98f, 0.70f, 0.80f},
    {1.00f, 0.90f, 0.94f},
    {0.96f, 0.62f, 0.75f},
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
std::vector<FallenItem> g_fallen;
std::vector<BottomSnowball> g_bottomSnowballs;
std::vector<SnowChunk> g_chunks;
SnowCap g_bottomSnowCap;
SnowCap g_seasonalSnowCap;
LightningFlash g_lightning;

bool IsSnowmanFullyAssembled(const std::vector<BottomSnowball>& balls) {
    if (balls.size() < kMaxSnowmanBalls) return false;
    return balls[1].stacked && balls[2].stacked;
}

bool HasAnyAutoSetting(const Settings& cfg) {
    return cfg.precipitationMode == PrecipMode::Auto ||
           cfg.precipFallDirection == FallDirMode::Auto ||
           cfg.seasonalFallDirection == FallDirMode::Auto ||
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

uint32_t Hash32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

float SmoothNoise(uint32_t seed, float x) {
    float cell = std::floor(x);
    uint32_t i = static_cast<uint32_t>(static_cast<int>(cell));
    float t = x - cell;
    t = t * t * (3.0f - 2.0f * t);
    float a = (Hash32(seed + i * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    float b = (Hash32(seed + (i + 1) * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    return a + (b - a) * t;
}

float CapNoise(const SnowCap& cap, int col, uint32_t salt, float scale) {
    return SmoothNoise(cap.seed + salt * 0x51ED27u, static_cast<float>(col + cap.offset) / scale);
}

float SnowColumnLimit(const SnowCap& cap, int col, float maxHeight, float scale) {
    float n = 0.65f * CapNoise(cap, col, 1, 70.0f * scale) +
              0.35f * CapNoise(cap, col, 2, 23.0f * scale);
    n = std::clamp((n - 0.5f) * 1.8f + 0.5f, 0.0f, 1.0f);
    return maxHeight * (0.32f + 0.68f * n);
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

int DaysInMonthSafe(int month) {
    switch (month) {
        case 2: return 29;
        case 4: case 6: case 9: case 11: return 30;
        default: return 31;
    }
}

bool TryParseDayMonthStrict(const std::wstring& raw, DayMonth& outDm) {
    std::wstring s = Trim(raw);
    size_t sep = s.find(L'.');
    if (sep == std::wstring::npos) sep = s.find(L'/');
    if (sep == std::wstring::npos) return false;
    std::wstring dPart = Trim(s.substr(0, sep));
    std::wstring mPart = Trim(s.substr(sep + 1));
    if (dPart.empty() || mPart.empty()) return false;
    for (wchar_t c : dPart) if (!iswdigit(c)) return false;
    for (wchar_t c : mPart) if (!iswdigit(c)) return false;
    try {
        int d = std::stoi(dPart);
        int m = std::stoi(mPart);
        if (m < 1 || m > 12) return false;
        if (d < 1 || d > DaysInMonthSafe(m)) return false;
        outDm = DayMonth{d, m};
        return true;
    } catch (...) {
        return false;
    }
}

bool TryParseDateRangePair(const std::wstring& raw, DayMonth& outStart, DayMonth& outEnd) {
    size_t dash = raw.find(L'-');
    if (dash == std::wstring::npos) return false;
    return TryParseDayMonthStrict(raw.substr(0, dash), outStart) &&
           TryParseDayMonthStrict(raw.substr(dash + 1), outEnd);
}

void ParseSeasonsRangeString(const std::wstring& raw, Settings& cfg) {
    DayMonth defSpS{1, 3}, defSpE{31, 5};
    DayMonth defSuS{1, 6}, defSuE{31, 8};
    DayMonth defAuS{1, 9}, defAuE{30, 11};
    DayMonth defWiS{1, 12}, defWiE{29, 2};

    std::vector<std::wstring> parts;
    std::wstringstream ss(raw);
    std::wstring item;
    while (std::getline(ss, item, L'/')) {
        parts.push_back(Trim(item));
    }

    DayMonth spS = defSpS, spE = defSpE, suS = defSuS, suE = defSuE, auS = defAuS, auE = defAuE, wiS = defWiS, wiE = defWiE;
    if (parts.size() != 4 ||
        !TryParseDateRangePair(parts[0], spS, spE) ||
        !TryParseDateRangePair(parts[1], suS, suE) ||
        !TryParseDateRangePair(parts[2], auS, auE) ||
        !TryParseDateRangePair(parts[3], wiS, wiE)) {
        cfg.springStart = defSpS; cfg.springEnd = defSpE;
        cfg.summerStart = defSuS; cfg.summerEnd = defSuE;
        cfg.autumnStart = defAuS; cfg.autumnEnd = defAuE;
        cfg.winterStart = defWiS; cfg.winterEnd = defWiE;
        return;
    }

    cfg.springStart = spS; cfg.springEnd = spE;
    cfg.summerStart = suS; cfg.summerEnd = suE;
    cfg.autumnStart = auS; cfg.autumnEnd = auE;
    cfg.winterStart = wiS; cfg.winterEnd = wiE;
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

int ToMonthDayKey(int month, int day) {
    return month * 100 + day;
}

bool IsDateInRange(int curKey, DayMonth start, DayMonth end) {
    int sKey = ToMonthDayKey(start.month, start.day);
    int eKey = ToMonthDayKey(end.month, end.day);
    if (sKey <= eKey) return curKey >= sKey && curKey <= eKey;
    return curKey >= sKey || curKey <= eKey;
}

SeasonalMode DetectSeasonByDates(const Settings& cfg) {
    std::time_t now = std::time(nullptr);
    std::tm tmLocal = {};
    localtime_s(&tmLocal, &now);
    int curKey = ToMonthDayKey(tmLocal.tm_mon + 1, tmLocal.tm_mday);

    if (IsDateInRange(curKey, cfg.springStart, cfg.springEnd)) return SeasonalMode::Petals;
    if (IsDateInRange(curKey, cfg.summerStart, cfg.summerEnd)) return SeasonalMode::Off;
    if (IsDateInRange(curKey, cfg.autumnStart, cfg.autumnEnd)) return SeasonalMode::AutumnLeaves;
    if (IsDateInRange(curKey, cfg.winterStart, cfg.winterEnd)) return SeasonalMode::ExtraSnow;
    return SeasonalMode::Off;
}

PrecipMode ParsePrecipMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"off") return PrecipMode::Off;
    if (v == L"rain") return PrecipMode::Rain;
    if (v == L"snow") return PrecipMode::Snow;
    if (v == L"hail") return PrecipMode::Hail;
    return PrecipMode::Auto;
}

SeasonalMode ParseSeasonalMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"off") return SeasonalMode::Off;
    if (v == L"autumn_leaves" || v == L"leaves") return SeasonalMode::AutumnLeaves;
    if (v == L"extra_snow" || v == L"snow") return SeasonalMode::ExtraSnow;
    if (v == L"petals" || v == L"sakura") return SeasonalMode::Petals;
    return SeasonalMode::Auto;
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

CursorActionMode ParseCursorAction(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"none") return CursorActionMode::None;
    if (v == L"push") return CursorActionMode::Push;
    return CursorActionMode::Clear;
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

    next.cursorAction = ParseCursorAction(readStr(L"cursorAction", L"clear"));
    next.cursorRadiusPx = ParseIntRangeOrDefault(Wh_GetIntSetting(L"cursorRadiusPx"), 10, 500, 95);

    next.enableBottomAccumulation = Wh_GetIntSetting(L"enableBottomAccumulation") != 0;

    next.primarySource = ToLower(Trim(readStr(L"primarySource", L"open_meteo")));
    next.fallbackSource = ToLower(Trim(readStr(L"fallbackSource", L"off")));
    next.hasValidCoords = TryParseCoordsString(readStr(L"coords", L""), next.latitude, next.longitude);
    next.customApiUrl = Trim(readStr(L"customApiUrl", L""));
    next.localWeatherPath = Trim(readStr(L"localWeatherPath", L""));
    next.updateIntervalMin = ParseIntRangeOrDefault(Wh_GetIntSetting(L"updateIntervalMin"), 1, 1440, 30);

    ParseSeasonsRangeString(
        readStr(L"seasonsRange", L"01.03-31.05/01.06-31.08/01.09-30.11/01.12-29.02"),
        next);

    next.precipitationMode = ParsePrecipMode(readStr(L"precipitationMode", L"auto"));
    next.precipFallDirection = ParseFallDir(readStr(L"precipFallDirection", L"auto"));
    next.precipFallSpeedMps = ParseDoubleRangeOrDefault(readStr(L"precipFallSpeedMps", L"6.5"), 0.1, 50.0, 6.5);

    next.precipIntensity = SanitizePercent(Wh_GetIntSetting(L"precipIntensity"));
    next.precipMaxAccumulated = ParseIntRangeOrDefault(Wh_GetIntSetting(L"precipMaxAccumulated"), 0, 2000, 220);

    auto readLifetime = [&](LPCWSTR key, double defMin) -> double {
        double v = 0.0;
        if (!TryParseDoubleStrict(readStr(key, L""), v)) return defMin;
        if (v < 0.0) {
            return (std::abs(v + 1.0) < 1e-6) ? -1.0 : defMin;
        }
        if (v > 1440.0) return defMin;
        return std::max(0.05, v);
    };
    next.precipLifetimeMin = readLifetime(L"precipLifetimeMin", 10.0);

    next.seasonalObjectMode = ParseSeasonalMode(readStr(L"seasonalObjectMode", L"auto"));
    next.seasonalFallDirection = ParseFallDir(readStr(L"seasonalFallDirection", L"auto"));
    next.seasonalFallSpeedMps = ParseDoubleRangeOrDefault(readStr(L"seasonalFallSpeedMps", L"2.2"), 0.1, 30.0, 2.2);

    next.seasonalIntensity = SanitizePercent(Wh_GetIntSetting(L"seasonalIntensity"));
    next.seasonalMaxAccumulated = ParseIntRangeOrDefault(Wh_GetIntSetting(L"seasonalMaxAccumulated"), 0, 2000, 220);
    next.seasonalLifetimeMin = readLifetime(L"seasonalLifetimeMin", 15.0);

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

    next.snowballMaxRadiusPx = ParseIntRangeOrDefault(Wh_GetIntSetting(L"snowballMaxRadiusPx"), 8, 120, 34);
    next.particleScale = ParseDoubleRangeOrDefault(readStr(L"particleScale", L"1.0"), 0.3, 4.0, 1.0);

    TryParseHexColorStrict(readStr(L"colorRain", L""), 0.78f, next.colorRain);
    next.hasCustomSnowColor = TryParseHexColorStrict(readStr(L"colorSnow", L""), 0.96f, next.colorSnow);
    TryParseHexColorStrict(readStr(L"colorHail", L""), 0.95f, next.colorHail);
    TryParseHexColorStrict(readStr(L"colorAutumnLeaves", L""), 0.94f, next.colorAutumnLeaves);
    TryParseHexColorStrict(readStr(L"colorExtraSnow", L""), 0.92f, next.colorExtraSnow);
    TryParseHexColorStrict(readStr(L"colorPetals", L""), 0.92f, next.colorPetals);

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

    HINTERNET hSession = InternetOpenW(L"WindhawkWeatherMod/2.2", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
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

BOOL CALLBACK CollectMonitorBoundsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<MonitorBoundEntry>*>(lParam);
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(hMon, &mi)) {
        MonitorBoundEntry entry;
        entry.left = static_cast<int>(mi.rcMonitor.left);
        entry.right = static_cast<int>(mi.rcMonitor.right);
        entry.bottomY = static_cast<int>(mi.rcMonitor.bottom);
        list->push_back(entry);
    }
    return TRUE;
}

FrameBottomBounds BuildFrameBottomBounds(const Settings& cfg) {
    FrameBottomBounds bounds;
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
    bounds.fallbackBottomY = std::clamp(baseBottom, g_canvasTop + 10, g_canvasTop + g_canvasHeight);

    if (cfg.multiMonitorMode == L"all") {
        std::vector<MonitorBoundEntry> rawMons;
        EnumDisplayMonitors(nullptr, nullptr, CollectMonitorBoundsProc, reinterpret_cast<LPARAM>(&rawMons));
        for (const auto& m : rawMons) {
            MONITORINFO mi = {sizeof(mi)};
            POINT mid = {m.left + std::max(1, (m.right - m.left) / 2), g_canvasTop + g_canvasHeight / 2};
            HMONITOR hm = MonitorFromPoint(mid, MONITOR_DEFAULTTONEAREST);
            if (GetMonitorInfoW(hm, &mi)) {
                int monTop = static_cast<int>(mi.rcMonitor.top);
                int monBottom = static_cast<int>(mi.rcMonitor.bottom);
                int y = monBottom;
                if (cfg.bottomBoundaryMode == L"above_taskbar") {
                    y = static_cast<int>(mi.rcWork.bottom);
                } else if (cfg.bottomBoundaryMode == L"custom_offset") {
                    y -= cfg.bottomOffsetPx;
                }
                bounds.monitors.push_back(MonitorBoundEntry{
                    static_cast<int>(mi.rcMonitor.left),
                    static_cast<int>(mi.rcMonitor.right),
                    std::clamp(y, monTop + 10, monBottom)
                });
            }
        }
        bounds.useMonitors = !bounds.monitors.empty();
    }
    return bounds;
}

int LookupBottomLimitY(const FrameBottomBounds& bounds, float worldX) {
    if (bounds.useMonitors) {
        int ix = static_cast<int>(worldX);
        for (const auto& m : bounds.monitors) {
            if (ix >= m.left && ix < m.right) {
                return m.bottomY;
            }
        }
    }
    return bounds.fallbackBottomY;
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

bool IsDesktopWindowAtPoint(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) return false;
    if (hwnd == g_overlayHwnd) return true;
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, _countof(cls));
    std::wstring clsStr = cls;
    return clsStr == L"Progman" ||
           clsStr == L"WorkerW" ||
           clsStr == L"SHELLDLL_DefView" ||
           clsStr == L"SysListView32";
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

int GetIntensityForKind(const Settings& cfg, bool isSeasonal) {
    return isSeasonal ? cfg.seasonalIntensity : cfg.precipIntensity;
}

int GetMaxAccumulatedForKind(const Settings& cfg, bool isSeasonal) {
    return isSeasonal ? cfg.seasonalMaxAccumulated : cfg.precipMaxAccumulated;
}

double GetLifetimeMinForKind(const Settings& cfg, bool isSeasonal) {
    return isSeasonal ? cfg.seasonalLifetimeMin : cfg.precipLifetimeMin;
}

D2D1_COLOR_F GetColorForKind(const Settings& cfg, ObjectKind kind) {
    switch (kind) {
        case ObjectKind::Rain: return cfg.colorRain;
        case ObjectKind::Snow: return cfg.colorSnow;
        case ObjectKind::Hail: return cfg.colorHail;
        case ObjectKind::AutumnLeaves: return cfg.colorAutumnLeaves;
        case ObjectKind::ExtraSnow: return cfg.colorExtraSnow;
        case ObjectKind::Petals: return cfg.colorPetals;
    }
    return cfg.colorSnow;
}

float ComputeMaxCapHeightPx(int maxAcc, double scale) {
    if (maxAcc <= 0) return 0.0f;
    return std::clamp(10.0f + (maxAcc / 2000.0f) * 32.0f, 10.0f, 44.0f) * static_cast<float>(scale);
}

void OnSeasonalModeChanged(bool extraSnowActive) {
    g_fallen.erase(
        std::remove_if(g_fallen.begin(), g_fallen.end(),
                       [extraSnowActive](const FallenItem& fo) {
                           if (!fo.isSeasonalLayer) return false;
                           return !extraSnowActive || fo.kind != ObjectKind::ExtraSnow;
                       }),
        g_fallen.end());

    if (!extraSnowActive) {
        g_bottomSnowballs.clear();
        for (auto& c : g_seasonalSnowCap.cols) c.h = 0.0f;
    }
}

void AddSnowToCap(SnowCap& cap, int column, float amount, float maxCapHeight, float scale) {
    int width = static_cast<int>(cap.cols.size());
    if (width < 4 || maxCapHeight <= 0.1f) return;

    constexpr int kReach = 14;
    int from = std::max(column - kReach, 1);
    int to = std::min(column + kReach, width - 2);
    if (from > to) return;

    float limit[2 * kReach + 1];
    for (int c = from; c <= to; ++c) {
        limit[c - from] = SnowColumnLimit(cap, c, maxCapHeight, scale);
    }

    for (int i = 0; i < 9; ++i) {
        int c = column + i - 4;
        if (c >= from && c <= to) {
            cap.cols[c].h = std::min(limit[c - from], cap.cols[c].h + amount * kSnowKernel[i]);
        }
    }
    for (int pass = 0; pass < 2; ++pass) {
        for (int c = from; c <= to; ++c) {
            float h = cap.cols[c].h * 0.5f + (cap.cols[c - 1].h + cap.cols[c + 1].h) * 0.25f;
            cap.cols[c].h = std::min(h, limit[c - from]);
        }
    }
}

void SpawnSnowChunk(float x, float y, float area, float vx, float vy, float scale) {
    if (g_chunks.size() >= 900 || area < 0.5f) return;
    SnowChunk ch;
    ch.x = x;
    ch.y = y;
    ch.vx = vx;
    ch.vy = vy;
    ch.area = area;
    ch.size = std::clamp(std::sqrt(area) * 0.55f, 1.5f * scale, 5.0f * scale);
    g_chunks.push_back(ch);
}

void SpawnFallingItem(FallingItem& item, std::mt19937& rng, ObjectKind kind, bool isSeasonal,
                      double scale, float vx, bool randomY) {
    std::uniform_real_distribution<float> phaseDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> alphaDist(0.65f, 0.96f);

    item.kind = kind;
    item.isSeasonalLayer = isSeasonal;
    item.swayPhase = phaseDist(rng);
    item.angle = phaseDist(rng);
    item.flip = phaseDist(rng);
    item.alpha = alphaDist(rng);
    item.shadeIdx = (kind == ObjectKind::Petals)
        ? static_cast<int>(rng() % _countof(kPetalPalette))
        : static_cast<int>(rng() % _countof(kLeafPalette));

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

    float s = static_cast<float>(scale);
    if (kind == ObjectKind::Rain) {
        item.size = 13.0f * s;
        item.spin = 0.0f;
        item.flipSpeed = 0.0f;
    } else if (kind == ObjectKind::Hail) {
        item.size = (5.0f + (rng() % 30) * 0.1f) * s;
        item.spin = ((rng() % 100) - 50) * 0.05f;
        item.flipSpeed = 0.0f;
    } else if (kind == ObjectKind::Petals) {
        item.size = (6.5f + (rng() % 30) * 0.1f) * s;
        item.spin = ((rng() % 100) - 50) * 0.06f;
        item.flipSpeed = (2.5f + (rng() % 30) * 0.1f) * ((rng() % 2 == 0) ? 1.0f : -1.0f);
    } else if (kind == ObjectKind::AutumnLeaves) {
        item.size = (10.5f + (rng() % 45) * 0.1f) * s;
        item.spin = ((rng() % 100) - 50) * 0.04f;
        item.flipSpeed = (1.8f + (rng() % 25) * 0.1f) * ((rng() % 2 == 0) ? 1.0f : -1.0f);
    } else {
        float depth = static_cast<float>(rng() % 1000) / 1000.0f;
        item.size = (2.4f + depth * depth * 5.2f) * s;
        item.spin = 0.0f;
        item.flipSpeed = 0.0f;
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

void DrawMapleLeafWithFlip(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           float x, float y, float r, float angle, float flip,
                           D2D1_COLOR_F baseColor, float alpha) {
    float widthScale = std::cos(flip);
    float shade = (widthScale < 0.0f) ? 0.80f : 1.0f;
    float squashY = std::max(0.18f, std::abs(widthScale));

    D2D1::Matrix3x2F oldXform;
    rt->GetTransform(&oldXform);
    rt->SetTransform(
        D2D1::Matrix3x2F::Scale(1.0f, squashY) *
        D2D1::Matrix3x2F::Rotation(angle * 57.29578f) *
        D2D1::Matrix3x2F::Translation(x, y) * oldXform);

    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(baseColor.r * shade, baseColor.g * shade, baseColor.b * shade, std::clamp(alpha, 0.05f, 1.0f)));
    D2D1_POINT_2F pts[10] = {
        {0.0f, -r},
        {r * 0.30f, -r * 0.42f},
        {r * 0.92f, -r * 0.35f},
        {r * 0.52f, r * 0.14f},
        {r * 0.72f, r * 0.68f},
        {0.0f, r * 0.40f},
        {-r * 0.72f, r * 0.68f},
        {-r * 0.52f, r * 0.14f},
        {-r * 0.92f, -r * 0.35f},
        {-r * 0.30f, -r * 0.42f}
    };
    for (int i = 0; i < 10; ++i) {
        int next = (i + 1) % 10;
        rt->DrawLine(pts[i], pts[next], brush, std::max(1.4f, r * 0.22f));
    }
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(0.0f, 0.0f), r * 0.48f, r * 0.48f), brush);

    brush->SetColor(D2D1::ColorF(baseColor.r * 0.58f * shade, baseColor.g * 0.58f * shade, baseColor.b * 0.58f * shade, std::clamp(alpha * 0.85f, 0.05f, 1.0f)));
    rt->DrawLine(D2D1::Point2F(0.0f, -r * 0.55f), D2D1::Point2F(0.0f, r * 1.18f), brush, std::max(1.1f, r * 0.15f));

    rt->SetTransform(oldXform);
}

void DrawNotchedPetalWithFlip(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                              float x, float y, float r, float angle, float flip,
                              D2D1_COLOR_F baseColor, float alpha) {
    float widthScale = std::cos(flip);
    float shade = (widthScale < 0.0f) ? 0.84f : 1.0f;
    float squashY = std::max(0.20f, std::abs(widthScale));

    D2D1::Matrix3x2F oldXform;
    rt->GetTransform(&oldXform);
    rt->SetTransform(
        D2D1::Matrix3x2F::Scale(1.0f, squashY) *
        D2D1::Matrix3x2F::Rotation(angle * 57.29578f) *
        D2D1::Matrix3x2F::Translation(x, y) * oldXform);

    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(baseColor.r * shade, baseColor.g * shade, baseColor.b * shade, std::clamp(alpha, 0.05f, 1.0f)));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(0.0f, 0.0f), r * 0.92f, r * 0.54f), brush);
    brush->SetColor(D2D1::ColorF(std::min(1.0f, baseColor.r * 1.04f), baseColor.g * 0.90f, baseColor.b * 0.92f, std::clamp(alpha * 0.55f, 0.05f, 0.8f)));
    rt->DrawLine(D2D1::Point2F(-r * 0.55f, 0.0f), D2D1::Point2F(r * 0.45f, 0.0f), brush, std::max(0.9f, r * 0.10f));

    rt->SetTransform(oldXform);
}

void DrawItemShape(CachedSurface& surf, const Settings& cfg,
                   ObjectKind kind, float x, float y, float size, float angle, float flip,
                   float vx, float alpha, int shadeIdx) {
    ID2D1SolidColorBrush* brush = surf.sharedBrush;
    if (kind == ObjectKind::Rain) {
        brush->SetColor(cfg.colorRain);
        brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
        surf.rt->DrawLine(D2D1::Point2F(x, y), D2D1::Point2F(x + vx * 0.02f, y + size), brush, 1.5f);
        brush->SetOpacity(1.0f);
        return;
    }

    if (kind == ObjectKind::Snow || kind == ObjectKind::ExtraSnow) {
        if (surf.flakeSprite) {
            float r = size * 1.35f;
            surf.rt->DrawBitmap(
                surf.flakeSprite,
                D2D1::RectF(x - r, y - r, x + r, y + r),
                std::clamp(alpha, 0.05f, 1.0f),
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            return;
        }
        brush->SetColor(GetColorForKind(cfg, kind));
        brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
        surf.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), size * 0.5f, size * 0.5f), brush);
        brush->SetOpacity(1.0f);
        return;
    }

    if (kind == ObjectKind::AutumnLeaves) {
        D2D1_COLOR_F col = cfg.colorAutumnLeaves;
        if (std::abs(col.r - 0.88f) < 0.01f && std::abs(col.g - 0.44f) < 0.01f) {
            const float* rgb = kLeafPalette[shadeIdx % _countof(kLeafPalette)];
            col = D2D1::ColorF(rgb[0], rgb[1], rgb[2], col.a);
        }
        DrawMapleLeafWithFlip(surf.rt, brush, x, y, size * 0.65f, angle, flip, col, alpha);
        return;
    }

    if (kind == ObjectKind::Petals) {
        D2D1_COLOR_F col = cfg.colorPetals;
        if (std::abs(col.r - 1.0f) < 0.01f && std::abs(col.g - 0.74f) < 0.01f) {
            const float* rgb = kPetalPalette[shadeIdx % _countof(kPetalPalette)];
            col = D2D1::ColorF(rgb[0], rgb[1], rgb[2], col.a);
        }
        DrawNotchedPetalWithFlip(surf.rt, brush, x, y, size * 0.68f, angle, flip, col, alpha);
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

void DrawSnowCapSurface(ID2D1DCRenderTarget* rt, const SnowCap& cap, float top,
                        D2D1_COLOR_F snowCol, bool hasCustomColor, float scale) {
    int width = static_cast<int>(cap.cols.size());
    if (width < 4) return;

    float highest = 0.0f;
    for (const auto& c : cap.cols) {
        highest = std::max(highest, c.h);
    }
    if (highest < 0.25f) return;

    ID2D1PathGeometry* path = nullptr;
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&path)) || !path) return;
    if (FAILED(path->Open(&sink)) || !sink) {
        path->Release();
        return;
    }

    float squashScale = 1.0f + cap.squash;
    auto colH = [&](int idx) -> float {
        float edge = static_cast<float>(std::min(idx, width - 1 - idx));
        float taper = std::clamp(edge / (kCapEdgeTaper * scale), 0.0f, 1.0f);
        return cap.cols[idx].h * squashScale * taper;
    };

    constexpr float kMinH = 0.3f;
    for (int c = 0; c < width;) {
        if (colH(c) < kMinH) {
            ++c;
            continue;
        }
        int first = c;
        while (c < width && colH(c) >= kMinH) ++c;
        int last = c - 1;

        float x0 = static_cast<float>(first);
        float x1 = static_cast<float>(last + 1);
        sink->BeginFigure(D2D1::Point2F(x0, top), D2D1_FIGURE_BEGIN_FILLED);
        for (int k = first; k <= last; k += 2) {
            sink->AddLine(D2D1::Point2F(static_cast<float>(k) + 0.5f, top - colH(k)));
        }
        sink->AddLine(D2D1::Point2F(x1 - 0.5f, top - colH(last)));
        sink->AddLine(D2D1::Point2F(x1, top));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    }

    if (FAILED(sink->Close())) {
        sink->Release();
        path->Release();
        return;
    }
    sink->Release();

    D2D1_COLOR_F topCol = hasCustomColor ? snowCol : D2D1::ColorF(0.99f, 0.995f, 1.0f, 0.96f);
    D2D1_COLOR_F botCol = hasCustomColor
        ? D2D1::ColorF(snowCol.r * 0.82f, snowCol.g * 0.85f, snowCol.b * 0.92f, 0.96f)
        : D2D1::ColorF(0.78f, 0.85f, 0.94f, 0.96f);

    D2D1_GRADIENT_STOP stops[2] = {
        {0.0f, topCol},
        {1.0f, botCol}
    };
    ID2D1GradientStopCollection* stopCol = nullptr;
    if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCol)) && stopCol) {
        ID2D1LinearGradientBrush* grad = nullptr;
        if (SUCCEEDED(rt->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(D2D1::Point2F(0.0f, top - highest * squashScale), D2D1::Point2F(0.0f, top)),
                stopCol, &grad)) && grad) {
            rt->FillGeometry(path, grad);
            grad->Release();
        }
        stopCol->Release();
    }
    path->Release();
}

void DrawRainPuddle(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, D2D1_COLOR_F rainCol, float x, float y, float size, float alpha) {
    brush->SetColor(rainCol);
    brush->SetOpacity(std::clamp(alpha * 0.55f, 0.05f, 0.75f));
    float width = std::max(8.0f, size * 1.4f);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), width * 0.5f, 1.0f), brush);
    brush->SetOpacity(1.0f);
}

void ComputeSnowballCentersStack(const std::vector<BottomSnowball>& balls, float bottomRelY,
                                 float (&outX)[kMaxSnowmanBalls], float (&outY)[kMaxSnowmanBalls], size_t& count) {
    count = std::min(balls.size(), kMaxSnowmanBalls);
    for (size_t i = 0; i < count; ++i) {
        const auto& sb = balls[i];
        outX[i] = sb.x;
        if (i == 0 || !sb.stacked) {
            outY[i] = bottomRelY - sb.radius - sb.lift;
        } else {
            float prevTopY = outY[i - 1] - balls[i - 1].radius;
            float targetY = prevTopY - sb.radius * 0.72f - sb.lift;
            outY[i] = std::min(bottomRelY - sb.radius - sb.lift, targetY);
        }
    }
}

void DrawSnowballsAndSnowman(CachedSurface& surf, const Settings& cfg,
                             const std::vector<BottomSnowball>& balls, float bottomRelY) {
    if (balls.empty()) return;
    float bx[kMaxSnowmanBalls] = {};
    float by[kMaxSnowmanBalls] = {};
    size_t count = 0;
    ComputeSnowballCentersStack(balls, bottomRelY, bx, by, count);

    ID2D1SolidColorBrush* brush = surf.sharedBrush;
    brush->SetOpacity(1.0f);

    for (size_t i = 0; i < count; ++i) {
        const auto& sb = balls[i];
        float rx = sb.radius * (1.0f + sb.squash * 0.35f);
        float ry = std::max(2.0f, sb.radius * (1.0f - sb.squash * 0.35f));
        if (surf.flakeSprite) {
            surf.rt->DrawBitmap(
                surf.flakeSprite,
                D2D1::RectF(bx[i] - rx * 1.25f, by[i] - ry * 1.25f, bx[i] + rx * 1.25f, by[i] + ry * 1.25f),
                std::clamp(sb.alpha, 0.1f, 1.0f),
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
            D2D1_COLOR_F c = GetColorForKind(cfg, sb.kind);
            c.a = std::clamp(sb.alpha, 0.1f, 1.0f);
            brush->SetColor(c);
            surf.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(bx[i], by[i]), rx, ry), brush);
        }
    }

    bool hasBody = (count >= 2 && balls[1].stacked);
    bool hasHead = (count >= 3 && balls[2].stacked);

    if (hasBody) {
        size_t bodyIdx = 1;
        float bodyX = bx[bodyIdx];
        float bodyY = by[bodyIdx];
        float bodyR = balls[bodyIdx].radius;
        float alpha = std::clamp(balls[bodyIdx].alpha, 0.15f, 1.0f);

        brush->SetColor(D2D1::ColorF(0.14f, 0.16f, 0.20f, alpha));
        float btnR = std::max(2.0f, bodyR * 0.12f);
        surf.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(bodyX, bodyY), btnR, btnR), brush);

        brush->SetColor(D2D1::ColorF(0.36f, 0.21f, 0.10f, alpha));
        float armThick = std::max(2.0f, bodyR * 0.12f);
        float armLen = std::max(14.0f, bodyR * 1.15f);
        surf.rt->DrawLine(
            D2D1::Point2F(bodyX - bodyR * 0.85f, bodyY - bodyR * 0.1f),
            D2D1::Point2F(bodyX - bodyR * 0.85f - armLen, bodyY - bodyR * 0.65f),
            brush, armThick);
        surf.rt->DrawLine(
            D2D1::Point2F(bodyX + bodyR * 0.85f, bodyY - bodyR * 0.1f),
            D2D1::Point2F(bodyX + bodyR * 0.85f + armLen, bodyY - bodyR * 0.65f),
            brush, armThick);
    }

    if (hasHead) {
        size_t headIdx = 2;
        float headX = bx[headIdx];
        float headY = by[headIdx];
        float headR = balls[headIdx].radius;
        float alpha = std::clamp(balls[headIdx].alpha, 0.15f, 1.0f);

        brush->SetColor(D2D1::ColorF(0.10f, 0.12f, 0.16f, alpha));
        float eyeR = std::max(1.8f, headR * 0.13f);
        float eyeY = headY - headR * 0.22f;
        float eyeOffsetX = headR * 0.32f;
        surf.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(headX - eyeOffsetX, eyeY), eyeR, eyeR), brush);
        surf.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(headX + eyeOffsetX, eyeY), eyeR, eyeR), brush);

        brush->SetColor(D2D1::ColorF(0.95f, 0.42f, 0.08f, alpha));
        float noseLen = std::max(7.0f, headR * 0.65f);
        float noseDir = (balls[headIdx].vx < -2.0f) ? -1.0f : 1.0f;
        surf.rt->DrawLine(
            D2D1::Point2F(headX, headY + headR * 0.05f),
            D2D1::Point2F(headX + noseDir * noseLen, headY + headR * 0.16f),
            brush,
            std::max(2.2f, headR * 0.16f));
    }
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

void DrawSmoothMirage(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg, float strength, float defaultBottomRelY, double timeSec) {
    if (strength <= 0.004f) return;
    float botY = defaultBottomRelY;
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

void RenderFrame(const Settings& cfg, const GeneralEffectsState& fx, const FrameBottomBounds& bounds,
                 float precipVx, double timeSec, bool forceArcDirty) {
    if (!g_overlayHwnd || !EnsureSurface(g_surface, g_canvasWidth, g_canvasHeight, cfg.colorSnow, cfg.hasCustomSnowColor)) return;

    float defaultBottomRelY = static_cast<float>(bounds.fallbackBottomY - g_canvasTop);
    float s = static_cast<float>(cfg.particleScale);

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
    if (fx.mirage) DrawSmoothMirage(g_surface.rt, g_surface.sharedBrush, cfg, k, defaultBottomRelY, timeSec);
    if (fx.rainbow) DrawSmoothRainbow(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec, forceArcDirty);
    if (fx.halo) DrawSmoothHalo(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec, forceArcDirty);

    if (fx.thunderstorm && g_lightning.timer > 0.0f) {
        D2D1_COLOR_F tc = cfg.hasCustomThunderstormColor ? cfg.colorThunderstorm : D2D1::ColorF(0.86f, 0.93f, 1.0f, 0.95f);
        float tAlpha = std::clamp(g_lightning.timer * 4.2f * std::max(0.25f, k), 0.0f, 1.0f);
        g_surface.sharedBrush->SetColor(D2D1::ColorF(tc.r, tc.g, tc.b, tAlpha));
        DrawBranchedLightning(g_surface.rt, g_surface.sharedBrush, g_lightning, defaultBottomRelY, tAlpha);
    }

    for (const auto& p : g_falling) {
        DrawItemShape(g_surface, cfg, p.kind, p.x, p.y, p.size, p.angle, p.flip, precipVx, p.alpha, p.shadeIdx);
    }

    for (const auto& ch : g_chunks) {
        DrawItemShape(g_surface, cfg, ObjectKind::ExtraSnow, ch.x, ch.y, ch.size * 1.4f, 0.0f, 0.0f, ch.vx, 0.94f, 0);
    }

    if (cfg.enableBottomAccumulation) {
        DrawSnowCapSurface(g_surface.rt, g_bottomSnowCap, defaultBottomRelY, cfg.colorSnow, cfg.hasCustomSnowColor, s);
        DrawSnowCapSurface(g_surface.rt, g_seasonalSnowCap, defaultBottomRelY, cfg.colorExtraSnow, true, s);

        for (const auto& fo : g_fallen) {
            float itemBottomRelY = static_cast<float>(LookupBottomLimitY(bounds, static_cast<float>(g_canvasLeft) + fo.relX) - g_canvasTop);
            float drawY = itemBottomRelY + fo.relY;
            if (fo.kind == ObjectKind::Rain) {
                DrawRainPuddle(g_surface.rt, g_surface.sharedBrush, cfg.colorRain, fo.relX, itemBottomRelY - 1.0f, fo.size, fo.alpha);
            } else {
                DrawItemShape(g_surface, cfg, fo.kind, fo.relX, drawY, fo.size, fo.angle, fo.flip, fo.vx, fo.alpha, fo.shadeIdx);
            }
        }

        DrawSnowballsAndSnowman(g_surface, cfg, g_bottomSnowballs, defaultBottomRelY);
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

    POINT prevCursorPt = {};
    GetCursorPos(&prevCursorPt);
    bool hasPrevCursor = true;
    bool prevLeftBtnDown = false;

    SeasonalMode prevSeasonalKind = SeasonalMode::Off;
    bool hasPrevSeasonalKind = false;

    g_bottomSnowCap.seed = Hash32(0x12345678u);
    g_seasonalSnowCap.seed = Hash32(0x87654321u);

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
        float s = static_cast<float>(cfg.particleScale);

        bool activePrecip = false;
        ObjectKind precipKind = ObjectKind::Rain;
        if (cfg.precipitationMode == PrecipMode::Rain) { activePrecip = true; precipKind = ObjectKind::Rain; }
        else if (cfg.precipitationMode == PrecipMode::Snow) { activePrecip = true; precipKind = ObjectKind::Snow; }
        else if (cfg.precipitationMode == PrecipMode::Hail) { activePrecip = true; precipKind = ObjectKind::Hail; }
        else if (cfg.precipitationMode == PrecipMode::Auto && hasForecastP) { activePrecip = true; precipKind = forecastP; }

        bool activeSeasonal = false;
        ObjectKind seasonalKind = ObjectKind::AutumnLeaves;
        SeasonalMode effectiveSeason = SeasonalMode::Off;
        if (cfg.seasonalObjectMode == SeasonalMode::AutumnLeaves) {
            activeSeasonal = true; seasonalKind = ObjectKind::AutumnLeaves; effectiveSeason = SeasonalMode::AutumnLeaves;
        } else if (cfg.seasonalObjectMode == SeasonalMode::ExtraSnow) {
            activeSeasonal = true; seasonalKind = ObjectKind::ExtraSnow; effectiveSeason = SeasonalMode::ExtraSnow;
        } else if (cfg.seasonalObjectMode == SeasonalMode::Petals) {
            activeSeasonal = true; seasonalKind = ObjectKind::Petals; effectiveSeason = SeasonalMode::Petals;
        } else if (cfg.seasonalObjectMode == SeasonalMode::Auto) {
            SeasonalMode detected = DetectSeasonByDates(cfg);
            effectiveSeason = detected;
            if (detected == SeasonalMode::AutumnLeaves) { activeSeasonal = true; seasonalKind = ObjectKind::AutumnLeaves; }
            else if (detected == SeasonalMode::ExtraSnow) { activeSeasonal = true; seasonalKind = ObjectKind::ExtraSnow; }
            else if (detected == SeasonalMode::Petals) { activeSeasonal = true; seasonalKind = ObjectKind::Petals; }
        }

        bool extraSnowActive = activeSeasonal && (seasonalKind == ObjectKind::ExtraSnow);

        if (!hasPrevSeasonalKind || prevSeasonalKind != effectiveSeason) {
            hasPrevSeasonalKind = true;
            prevSeasonalKind = effectiveSeason;
            OnSeasonalModeChanged(extraSnowActive);
        } else if (!extraSnowActive && !g_bottomSnowballs.empty()) {
            g_bottomSnowballs.clear();
        }

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

        bool hasAnyState = activePrecip || activeSeasonal || anyFxActive ||
                           !g_falling.empty() || !g_fallen.empty() ||
                           !g_bottomSnowballs.empty() || !g_chunks.empty();
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
        float precipVy = static_cast<float>(cfg.precipFallSpeedMps) * 42.0f * s;

        float seasonalVx = ComputeHorizontalSpeedPxPerSec(cfg.seasonalFallDirection, cfg.seasonalFallSpeedMps, forecastWindSign, forecastWindSpd);
        float seasonalVy = static_cast<float>(cfg.seasonalFallSpeedMps) * 32.0f * s;

        size_t targetP = activePrecip
            ? static_cast<size_t>((g_canvasWidth / 1000.0) * GetIntensityForKind(cfg, false) * 1.4)
            : 0;
        size_t targetS = activeSeasonal
            ? static_cast<size_t>((g_canvasWidth / 1000.0) * GetIntensityForKind(cfg, true) * 1.2)
            : 0;

        size_t desiredCount = targetP + targetS;
        size_t prevCount = g_falling.size();
        if (g_falling.size() != desiredCount) {
            g_falling.resize(desiredCount);
        }
        for (size_t i = 0; i < desiredCount; ++i) {
            bool wantSeasonal = (i >= targetP);
            ObjectKind wantKind = wantSeasonal ? seasonalKind : precipKind;
            if (i >= prevCount || g_falling[i].isSeasonalLayer != wantSeasonal || g_falling[i].kind != wantKind) {
                SpawnFallingItem(g_falling[i], rng, wantKind, wantSeasonal, cfg.particleScale,
                                 wantSeasonal ? seasonalVx : precipVx, true);
            }
        }

        FrameBottomBounds bounds = BuildFrameBottomBounds(cfg);
        float defaultBottomRelY = static_cast<float>(bounds.fallbackBottomY - g_canvasTop);

        if (static_cast<int>(g_bottomSnowCap.cols.size()) != g_canvasWidth) {
            g_bottomSnowCap.cols.assign(g_canvasWidth, SnowColumn{});
        }
        if (static_cast<int>(g_seasonalSnowCap.cols.size()) != g_canvasWidth) {
            g_seasonalSnowCap.cols.assign(g_canvasWidth, SnowColumn{});
        }
        if (!cfg.enableBottomAccumulation || cfg.precipMaxAccumulated <= 0) {
            for (auto& c : g_bottomSnowCap.cols) c.h = 0.0f;
        }
        if (!cfg.enableBottomAccumulation || !extraSnowActive || cfg.seasonalMaxAccumulated <= 0) {
            for (auto& c : g_seasonalSnowCap.cols) c.h = 0.0f;
        }

        if (cfg.precipLifetimeMin >= 0.0) {
            float meltRate = ComputeMaxCapHeightPx(cfg.precipMaxAccumulated, cfg.particleScale) /
                             std::max(3.0f, static_cast<float>(cfg.precipLifetimeMin * 60.0));
            for (auto& c : g_bottomSnowCap.cols) {
                if (c.h > 0.0f) c.h = std::max(0.0f, c.h - meltRate * dt);
            }
        }
        if (cfg.seasonalLifetimeMin >= 0.0) {
            float sMeltRate = ComputeMaxCapHeightPx(cfg.seasonalMaxAccumulated, cfg.particleScale) /
                              std::max(3.0f, static_cast<float>(cfg.seasonalLifetimeMin * 60.0));
            for (auto& c : g_seasonalSnowCap.cols) {
                if (c.h > 0.0f) c.h = std::max(0.0f, c.h - sMeltRate * dt);
            }
        }

        auto countAccumulated = [&](bool isS) -> int {
            int c = 0;
            for (const auto& fo : g_fallen) {
                if (fo.isSeasonalLayer == isS) ++c;
            }
            return c;
        };

        for (size_t i = 0; i < g_falling.size(); ++i) {
            FallingItem& p = g_falling[i];
            float vx = p.isSeasonalLayer ? seasonalVx : precipVx;
            float vy = p.isSeasonalLayer ? seasonalVy : precipVy;
            p.swayPhase += dt * 3.0f;
            p.angle += p.spin * dt * 20.0f;
            p.flip += p.flipSpeed * dt;

            float sway = (p.kind == ObjectKind::Rain) ? 0.0f : std::sin(p.swayPhase) * 18.0f;
            p.x += (vx + sway) * dt;
            p.y += vy * dt;

            float itemBottomRelY = static_cast<float>(LookupBottomLimitY(bounds, static_cast<float>(g_canvasLeft) + p.x) - g_canvasTop);
            int maxAcc = GetMaxAccumulatedForKind(cfg, p.isSeasonalLayer);
            bool landed = false;

            if (cfg.enableBottomAccumulation && maxAcc > 0 && p.y >= itemBottomRelY - p.size * 0.30f) {
                if (p.kind == ObjectKind::Snow) {
                    int col = std::clamp(static_cast<int>(p.x), 0, static_cast<int>(g_bottomSnowCap.cols.size()) - 1);
                    AddSnowToCap(g_bottomSnowCap, col, 2.0f * s + 0.7f * p.size,
                                 ComputeMaxCapHeightPx(cfg.precipMaxAccumulated, cfg.particleScale), s);
                    landed = true;
                } else if (p.kind == ObjectKind::ExtraSnow) {
                    int col = std::clamp(static_cast<int>(p.x), 0, static_cast<int>(g_seasonalSnowCap.cols.size()) - 1);
                    AddSnowToCap(g_seasonalSnowCap, col, 2.0f * s + 0.7f * p.size,
                                 ComputeMaxCapHeightPx(cfg.seasonalMaxAccumulated, cfg.particleScale), s);
                    landed = true;
                } else if (countAccumulated(p.isSeasonalLayer) < maxAcc) {
                    FallenItem fo;
                    fo.kind = p.kind;
                    fo.isSeasonalLayer = p.isSeasonalLayer;
                    fo.relX = p.x;
                    fo.relY = (p.kind == ObjectKind::Rain) ? -1.0f : (-p.size * 0.28f);
                    fo.size = p.size;
                    fo.angle = (rng() % 100 - 50) * 0.01f;
                    fo.flip = 0.65f + (rng() % 30) * 0.01f;
                    fo.alpha = p.alpha;
                    fo.shadeIdx = p.shadeIdx;
                    g_fallen.push_back(fo);
                    landed = true;
                }
            }

            if (landed || p.y > g_canvasHeight + 30.0f || p.x < -140.0f || p.x > g_canvasWidth + 140.0f) {
                SpawnFallingItem(p, rng, p.kind, p.isSeasonalLayer, cfg.particleScale, vx, false);
            }
        }

        for (size_t i = 0; i < g_chunks.size();) {
            SnowChunk& ch = g_chunks[i];
            ch.vy = std::min(ch.vy + kGravity * 0.5f * s * dt, 700.0f * s);
            ch.vx *= std::exp(-2.0f * dt);
            ch.x += ch.vx * dt;
            ch.y += ch.vy * dt;

            float chBottomRelY = static_cast<float>(LookupBottomLimitY(bounds, static_cast<float>(g_canvasLeft) + ch.x) - g_canvasTop);
            bool settled = false;
            if (ch.y >= chBottomRelY && cfg.enableBottomAccumulation && extraSnowActive && cfg.seasonalMaxAccumulated > 0) {
                int col = std::clamp(static_cast<int>(ch.x), 0, static_cast<int>(g_seasonalSnowCap.cols.size()) - 1);
                AddSnowToCap(g_seasonalSnowCap, col, ch.area / kSnowKernelSum,
                             ComputeMaxCapHeightPx(cfg.seasonalMaxAccumulated, cfg.particleScale), s);
                settled = true;
            }
            if (settled || ch.y > g_canvasHeight + 20.0f || ch.x < -80.0f || ch.x > g_canvasWidth + 80.0f) {
                g_chunks.erase(g_chunks.begin() + i);
            } else {
                ++i;
            }
        }

        POINT cursorPt = {};
        float cX = 0.0f, cY = 0.0f, cDx = 0.0f, cMoveLen = 0.0f;
        bool cursorOverDesktop = false;
        bool needCursor = (cfg.cursorAction != CursorActionMode::None) || !g_bottomSnowballs.empty();
        if (needCursor && GetCursorPos(&cursorPt)) {
            cX = static_cast<float>(cursorPt.x - g_canvasLeft);
            cY = static_cast<float>(cursorPt.y - g_canvasTop);
            cDx = hasPrevCursor ? static_cast<float>(cursorPt.x - prevCursorPt.x) : 0.0f;
            prevCursorPt = cursorPt;
            hasPrevCursor = true;
            cMoveLen = std::abs(cDx);
            cursorOverDesktop = IsDesktopWindowAtPoint(cursorPt);
        }

        float cRad = static_cast<float>(cfg.cursorRadiusPx);
        float maxBallR = static_cast<float>(cfg.snowballMaxRadiusPx);
        bool snowmanAssembled = IsSnowmanFullyAssembled(g_bottomSnowballs);

        bool leftBtnDown = !g_bottomSnowballs.empty() && cursorOverDesktop && ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);
        if (leftBtnDown && !prevLeftBtnDown) {
            float bx[kMaxSnowmanBalls] = {};
            float by[kMaxSnowmanBalls] = {};
            size_t ballCount = 0;
            ComputeSnowballCentersStack(g_bottomSnowballs, defaultBottomRelY, bx, by, ballCount);

            bool hitBall = false;
            for (size_t i = 0; i < ballCount; ++i) {
                float dx = cX - bx[i];
                float dy = cY - by[i];
                float hitR = g_bottomSnowballs[i].radius * 1.65f;
                if (std::abs(dx) <= hitR && std::abs(dy) <= hitR && (dx * dx + dy * dy) <= hitR * hitR) {
                    hitBall = true;
                    break;
                }
            }
            if (hitBall) {
                for (size_t i = 0; i < ballCount; ++i) {
                    float dir = (i % 2 == 0) ? -1.0f : 1.0f;
                    int count = std::clamp(static_cast<int>(g_bottomSnowballs[i].radius / 2.5f), 5, 16);
                    for (int k = 0; k < count; ++k) {
                        SpawnSnowChunk(
                            bx[i] + static_cast<float>((rng() % 24) - 12),
                            by[i] + static_cast<float>((rng() % 18) - 9),
                            g_bottomSnowballs[i].radius * 0.45f,
                            dir * (80.0f + static_cast<float>(rng() % 160)),
                            -50.0f - static_cast<float>(rng() % 110),
                            s);
                    }
                }
                g_bottomSnowballs.clear();
                snowmanAssembled = false;
            }
        }
        prevLeftBtnDown = leftBtnDown;

        if (cursorOverDesktop && extraSnowActive && cfg.enableBottomAccumulation &&
            cfg.seasonalMaxAccumulated > 0 && !snowmanAssembled &&
            cfg.cursorAction == CursorActionMode::Push &&
            cMoveLen > 0.5f && std::abs(cY - defaultBottomRelY) < cRad && !g_seasonalSnowCap.cols.empty()) {
            int activeBallIdx = -1;
            for (size_t i = 0; i < g_bottomSnowballs.size(); ++i) {
                if (!g_bottomSnowballs[i].full) {
                    activeBallIdx = static_cast<int>(i);
                    break;
                }
            }

            float dirX = (cDx >= 0.0f) ? 1.0f : -1.0f;
            int maxColIdx = static_cast<int>(g_seasonalSnowCap.cols.size()) - 1;
            int from = std::clamp(static_cast<int>(cX - cRad * 0.6f), 0, maxColIdx);
            int to = std::clamp(static_cast<int>(cX + cRad * 0.6f), 0, maxColIdx);
            float scooped = 0.0f;
            for (int c = from; c <= to; ++c) {
                if (g_seasonalSnowCap.cols[c].h > 0.2f) {
                    scooped += g_seasonalSnowCap.cols[c].h;
                    g_seasonalSnowCap.cols[c].h = 0.0f;
                }
            }

            if (scooped > 1.5f) {
                if (activeBallIdx < 0 && g_bottomSnowballs.size() < kMaxSnowmanBalls) {
                    BottomSnowball sb;
                    sb.kind = ObjectKind::ExtraSnow;
                    sb.slot = static_cast<int>(g_bottomSnowballs.size());
                    sb.x = std::clamp(cX, maxBallR, std::max(maxBallR + 1.0f, static_cast<float>(g_canvasWidth) - maxBallR));
                    sb.vx = dirX * 55.0f;
                    float targetMaxForSlot = (sb.slot == 0) ? maxBallR : (maxBallR * (sb.slot == 1 ? 0.78f : 0.58f));
                    sb.radius = std::clamp(std::sqrt(scooped * 0.45f), 6.0f, targetMaxForSlot);
                    if (sb.radius >= targetMaxForSlot - 0.25f) sb.full = true;
                    g_bottomSnowballs.push_back(sb);
                    activeBallIdx = static_cast<int>(g_bottomSnowballs.size()) - 1;
                }
                if (activeBallIdx >= 0 && activeBallIdx < static_cast<int>(g_bottomSnowballs.size())) {
                    BottomSnowball& sb = g_bottomSnowballs[activeBallIdx];
                    float slotMaxR = (sb.slot == 0) ? maxBallR : (maxBallR * (sb.slot == 1 ? 0.78f : 0.58f));
                    if (!sb.full) {
                        sb.radius = std::min(slotMaxR, std::sqrt(sb.radius * sb.radius + scooped * 0.35f));
                        sb.vx += dirX * (sb.slot == 0 ? 14.0f : 24.0f);
                        if (sb.radius >= slotMaxR - 0.25f) {
                            sb.full = true;
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < g_fallen.size();) {
            FallenItem& fo = g_fallen[i];
            fo.ageSec += dt;

            double lifeMin = GetLifetimeMinForKind(cfg, fo.isSeasonalLayer);
            if (lifeMin >= 0.0) {
                float lifeSec = static_cast<float>(lifeMin * 60.0);
                if (fo.ageSec >= lifeSec) fo.alpha -= dt * 1.5f;
                if (fo.alpha <= 0.03f) {
                    g_fallen.erase(g_fallen.begin() + i);
                    continue;
                }
            }

            if (cursorOverDesktop && cfg.cursorAction != CursorActionMode::None) {
                float itemBottomRelY = static_cast<float>(LookupBottomLimitY(bounds, static_cast<float>(g_canvasLeft) + fo.relX) - g_canvasTop);
                float absX = fo.relX;
                float absY = itemBottomRelY + fo.relY;
                float dx = absX - cX;
                float dy = absY - cY;
                if (std::abs(dx) < cRad && std::abs(dy) < cRad) {
                    float distSq = dx * dx + dy * dy;
                    float radSq = cRad * cRad;
                    if (distSq < radSq) {
                        if (cfg.cursorAction == CursorActionMode::Clear) {
                            g_fallen.erase(g_fallen.begin() + i);
                            continue;
                        }
                        if (cfg.cursorAction == CursorActionMode::Push && fo.kind != ObjectKind::Rain) {
                            float dist = std::sqrt(distSq);
                            float awaySign = (dx >= 0.0f) ? 1.0f : -1.0f;
                            fo.vx += awaySign * 115.0f * (1.0f - dist / cRad) * dt;
                            fo.flip += awaySign * 1.8f * dt;
                        }
                    }
                }
            }

            if (std::abs(fo.vx) > 0.5f) {
                fo.relX = std::clamp(fo.relX + fo.vx * dt, 0.0f, static_cast<float>(g_canvasWidth));
                fo.vx *= std::pow(0.08f, dt);
            }

            ++i;
        }

        if (extraSnowActive && !g_bottomSnowballs.empty()) {
            float bx[kMaxSnowmanBalls] = {};
            float by[kMaxSnowmanBalls] = {};
            size_t ballCount = 0;
            ComputeSnowballCentersStack(g_bottomSnowballs, defaultBottomRelY, bx, by, ballCount);

            for (size_t i = 0; i < g_bottomSnowballs.size();) {
                BottomSnowball& sb = g_bottomSnowballs[i];
                sb.ageSec += dt;

                double lifeMin = GetLifetimeMinForKind(cfg, true);
                if (lifeMin >= 0.0 && sb.ageSec >= lifeMin * 60.0) {
                    sb.alpha -= dt * 1.5f;
                    if (sb.alpha <= 0.03f) {
                        g_bottomSnowballs.erase(g_bottomSnowballs.begin() + i);
                        continue;
                    }
                }

                if (sb.lift > 0.0f || sb.liftVel != 0.0f) {
                    sb.liftVel += (-260.0f * sb.lift - 18.0f * sb.liftVel) * dt;
                    sb.lift = std::max(0.0f, sb.lift + sb.liftVel * dt);
                    if (sb.lift <= 0.01f && std::abs(sb.liftVel) < 0.5f) {
                        sb.lift = 0.0f;
                        sb.liftVel = 0.0f;
                    }
                }

                if (sb.squash != 0.0f || sb.squashVel != 0.0f) {
                    sb.squashVel += (-220.0f * sb.squash - 16.0f * sb.squashVel) * dt;
                    sb.squash = std::clamp(sb.squash + sb.squashVel * dt, -0.35f, 0.35f);
                    if (std::abs(sb.squash) < 0.002f && std::abs(sb.squashVel) < 0.02f) {
                        sb.squash = 0.0f;
                        sb.squashVel = 0.0f;
                    }
                }

                float curX = bx[i];
                float curY = by[i];
                float dx = curX - cX;
                float dy = curY - cY;
                float reach = cRad + sb.radius;

                if (cursorOverDesktop && cfg.cursorAction == CursorActionMode::Push &&
                    std::abs(dx) < reach && std::abs(dy) < reach &&
                    (dx * dx + dy * dy) < reach * reach) {
                    if (snowmanAssembled) {
                        float awaySign = (dx >= 0.0f) ? 1.0f : -1.0f;
                        float dist = std::sqrt(dx * dx + dy * dy);
                        float pushForce = (1.0f - dist / reach) * 140.0f * dt;
                        sb.vx += awaySign * pushForce;
                    } else if (cMoveLen > 0.5f) {
                        float moveFactor = (sb.slot == 0) ? 75.0f : 185.0f;
                        sb.vx += (cDx / cMoveLen) * moveFactor * dt;
                    } else {
                        float awaySign = (dx >= 0.0f) ? 1.0f : -1.0f;
                        sb.vx += awaySign * 55.0f * dt;
                    }
                }

                if (i > 0 && sb.stacked) {
                    float targetX = g_bottomSnowballs[i - 1].x;
                    sb.x += (targetX - sb.x) * std::min(1.0f, dt * 12.0f);
                } else {
                    sb.x = std::clamp(sb.x + sb.vx * dt, sb.radius, std::max(sb.radius + 1.0f, static_cast<float>(g_canvasWidth) - sb.radius));
                }
                sb.vx *= std::pow(0.10f, dt);

                if (i > 0 && !sb.stacked) {
                    const BottomSnowball& lower = g_bottomSnowballs[i - 1];
                    float magnetRange = lower.radius * 2.2f + sb.radius;
                    float deltaX = lower.x - sb.x;
                    if (std::abs(deltaX) <= magnetRange) {
                        sb.x += deltaX * std::min(1.0f, dt * 8.0f);
                        if (std::abs(lower.x - sb.x) <= lower.radius * 1.15f) {
                            sb.stacked = true;
                            sb.x = lower.x;
                            sb.vx = 0.0f;
                            sb.liftVel = 155.0f;
                            sb.squashVel -= 0.85f;
                        }
                    }
                }

                ++i;
            }
        }

        RenderFrame(cfg, activeFx, bounds, precipVx, timeSec, forceArcDirty);

        bool hasAnimatedMotion = !g_falling.empty() || !g_fallen.empty() ||
                                 !g_bottomSnowballs.empty() || !g_chunks.empty() ||
                                 activeFx.thunderstorm || activeFx.aurora || activeFx.mirage;
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