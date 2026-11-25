// ==WindhawkMod==
// @id              alembiq-weather-desktop-overlay
// @name            Desktop Weather & Particles Overlay
// @description     Atmospheric effects and live weather forecast visualization for the Windows desktop (Direct2D weather and seasonal particle overlay)
// @version         1.6.5
// @author          alembiq
// @github          https://github.com/alembiqspace
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lwininet -luser32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
A lightweight **Direct2D** weather and atmosphere overlay for the Windows desktop. It brings live weather precipitation, time-of-day tones, volumetric-style drifting clouds, a UV-reactive sun and lunar-phase moon, atmospheric layers, air quality, pollen, flood-level water, and a draggable temperature button with an interactive control & forecast table without cluttering icons or windows.

### Features
- **Performance Quality Scaling:** `[System] Performance mode` (`low`, `medium`, `high`) scales particle counts, cloud layers, sun rays, moon detail, lightning branches, wave smoothness, rain splashes on `high`, and HUD detail.
- **Precipitation (`[Precipitation]`):** Supports `off`, `auto`, `rain`, `snow`, and `hail` with configurable fall direction, `auto` (forecast precipitation-driven speed scaled per snow/rain/hail) or `custom` fall speed, and intensity.
- **Sky (`[Sky]`):** Intensity-based drifting clouds with edge fade at monitor borders, a UV-reactive sun with soft rays, and a real-phase moon with surface shading, independent movement modes, and cloud occlusion toggle.
- **Effects (`[Effects]`):** System-clock time-of-day tinting, wind gusts, frost/heat tint, night stars, air quality haze/particles, floating pollen, flood-level water, and atmospheric layers (`clear`, `partlyCloudy`, `overcast`, `dust`, `fog`, `smog`, `thunderstorm`, `aurora`, `mirage`, `rainbow`).
- **Draggable Temperature Button & Interactive Table:** Drag the circular button (sized in pixels with auto-fitting single-line font) or click it to open an auto-sized table below desktop icons without blocking icon clicks. Sections (`[Sky]`, `[Precipitation]`, `[Effects]`) start collapsed and expand one at a time; manual edit fields appear left of mode buttons only when active/manual (hidden on `auto` / `off`).

### Network & Privacy
- **No Unsolicited Requests by Default:** `[Weather] Latitude/Longitude` (`coords`) is empty by default. No external network requests are made until you configure valid coordinates (or a custom JSON API URL) and have at least one forecast-driven setting enabled.
- **What Is Sent & When:** When `primarySource` is set to `open_meteo` with valid `lat/lon` coordinates and forecast data is needed, the mod sends an HTTPS `GET` request to `https://api.open-meteo.com/v1/forecast` (and `https://air-quality-api.open-meteo.com/v1/air-quality` when air quality or pollen is in `auto`) containing only your configured coordinates and requested fields. Requests run on startup, on settings changes, on manual sync, every `[Weather] Update interval (min)` (default: `30` minutes), and every `60` seconds after a failed attempt.
- **How to Disable:** Set `[Weather] Primary source` (`primarySource`) to `off`, leave `[Weather] Latitude/Longitude` (`coords`) empty, or switch forecast-dependent settings away from `auto` / `on`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# --- 1. SYSTEM & LAYER ---
- layerAttachMode: auto
  $name: "[System] Layer attach mode"
  $description: "Controls Z-order placement of the desktop overlay. Allowed: auto, progman_direct, desktop_bottom. Default: auto."
  $options:
    - auto: Auto (below desktop icons)
    - progman_direct: Progman reference
    - desktop_bottom: Bottom of Z-order (HWND_BOTTOM)
- performanceMode: high
  $name: "[System] Performance mode"
  $description: "Scales visual detail and particle density across all layers. Allowed: low, medium, high. Default: high."
  $options:
    - low: Low (fastest)
    - medium: Medium (balanced)
    - high: High (highest detail + rain splashes)
- bottomBoundaryMode: above_taskbar
  $name: "[System] Bottom boundary mode"
  $description: "Sets bottom reference line for ground effects, vertical celestial rise, flood water, and bottom HUD placement. Allowed: above_taskbar, screen_bottom, custom_offset. Default: above_taskbar."
  $options:
    - above_taskbar: Above taskbar (Shell_TrayWnd)
    - screen_bottom: Bottom edge of screen
    - custom_offset: Custom offset from bottom (px)
- bottomOffsetPx: 10
  $name: "[System] Custom bottom offset (px)"
  $description: "Vertical offset in px above bottom edge when bottomBoundaryMode is custom_offset. Range: -500 to 1000. Default: 10."
- taskbarAsBorder: true
  $name: "[System] Taskbar as a border"
  $description: "When enabled in above_taskbar mode, uses the taskbar work area bottom edge as the lower screen border. Default: true."
- customOffset: "0/0/0/10"
  $name: "[System] Custom offset (px)"
  $description: "Insets in px formatted as 'left/right/top/bottom'. Default: '0/0/0/10'."
- multiMonitorMode: primary
  $name: "[System] Monitors"
  $description: "Render on primary monitor or all monitors. Allowed: primary, all. Default: primary."
  $options:
    - primary: Primary monitor
    - all: All monitors
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
  $description: "HTTP/HTTPS JSON endpoint (weather_code, wind_speed_10m, wind_gusts_10m, wind_direction_10m, temperature_2m, apparent_temperature, relative_humidity_2m, cloud_cover, visibility, is_day, uv_index, precipitation, european_aqi, pm2_5, alder_pollen, birch_pollen, grass_pollen, ragweed_pollen)."
- localWeatherPath: ""
  $name: "[Weather] Local weather file path"
  $description: "Path to local JSON weather file. Default: empty."
- updateIntervalMin: 30
  $name: "[Weather] Update interval (min)"
  $description: "Forecast refresh period in minutes (1 to 1440). Invalid/out-of-range resets to 30."

# --- 3. FLOATING TEMPERATURE BUTTON & TABLE ---
- forecastHudMode: off
  $name: "[HUD] Temperature button & table"
  $description: "Shows a draggable circular temperature button on the desktop; clicking it opens/closes the interactive forecast & settings table."
  $options:
    - off: Off
    - on: On
- hudButtonRadiusPx: 24
  $name: "[HUD] Button radius (px)"
  $description: "Circular temperature button radius in pixels (14 to 96). Default: 24."
- hudFontSizePx: 12
  $name: "[HUD] Font size (px)"
  $description: "Base font size in pixels for the HUD table (9 to 22). Default: 12."
- hudButtonGeometry: "4-92/88"
  $name: "[HUD] Fallback button center X/Y (%)"
  $description: "Format: radius-centerX/centerY in % (0..100; X left->right, Y bottom->top) when no dragged position is set. Default: 4-92/88."
- hudButtonPos: ""
  $name: "[HUD] Initial relative button X/Y (0..1)"
  $description: "Optional relative position 'x/y' for the button; empty uses hudButtonGeometry."
- forecastHudMarginPx: 14
  $name: "[HUD] Gap & screen margin (px)"
  $description: "Gap between table and button / screen edges in pixels (0 to 400). Invalid resets to 14."
- forecastHudOpacity: 84
  $name: "[HUD] Background opacity (%)"
  $description: "Button and table background opacity (15 to 100). Invalid resets to 84."

# --- 4. SKY (CLOUDS, SUN & MOON) ---
- cloudsMode: auto
  $name: "[Sky] Clouds mode"
  $description: "Drifting cloud layer driven by cloud intensity. Allowed: off, auto, on. Default: auto."
  $options:
    - off: Off
    - auto: Auto (forecast cloud_cover)
    - on: On (manual intensity)
- cloudsIntensity: 35
  $name: "[Sky] Clouds intensity (%)"
  $description: "Manual cloud coverage intensity when cloudsMode is on (0..100). Invalid resets to 35."
- sunMode: auto
  $name: "[Sky] Sun mode"
  $description: "Sun disc, rays & glow with UV-index color shift. Allowed: off, auto, on. Default: auto."
  $options:
    - off: Off
    - auto: Auto (daytime)
    - on: On
- sunMotionMode: rtl_arc
  $name: "[Sky] Sun movement"
  $description: "Movement mode for the Sun: fixed, ltr_line/ltr_arc, rtl_line/rtl_arc, or vertical (system clock or forecast sunrise/sunset)."
  $options:
    - fixed: fixed
    - ltr_line: ltr_line (system clock)
    - ltr_line_forecast: ltr_line (forecast)
    - ltr_arc: ltr_arc (system clock)
    - ltr_arc_forecast: ltr_arc (forecast)
    - rtl_line: rtl_line (system clock)
    - rtl_line_forecast: rtl_line (forecast)
    - rtl_arc: rtl_arc (system clock)
    - rtl_arc_forecast: rtl_arc (forecast)
    - vertical: vertical (system clock)
    - vertical_forecast: vertical (forecast)
- sunGeometry: "7-82/82"
  $name: "[Sky] Sun radius-center X/Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius sets size in all modes, X/Y used when movement is fixed or vertical X). Default: 7-82/82."
- sunUvOverride: "-1"
  $name: "[Sky] Sun UV index override (-1 = forecast)"
  $description: "Set -1 to use forecast uv_index, or 0..20 to force UV color shift (low = gold, high = red). Invalid resets to -1."
- moonMode: auto
  $name: "[Sky] Moon mode"
  $description: "Moon disc, surface shading & glow with real lunar phase. Allowed: off, auto, on. Default: auto."
  $options:
    - off: Off
    - auto: Auto (night / night time-of-day)
    - on: On
- moonMotionMode: rtl_arc
  $name: "[Sky] Moon movement"
  $description: "Movement mode for the Moon: fixed, ltr_line/ltr_arc, rtl_line/rtl_arc, or vertical (system clock or forecast sunset/sunrise)."
  $options:
    - fixed: fixed
    - ltr_line: ltr_line (system clock)
    - ltr_line_forecast: ltr_line (forecast)
    - ltr_arc: ltr_arc (system clock)
    - ltr_arc_forecast: ltr_arc (forecast)
    - rtl_line: rtl_line (system clock)
    - rtl_line_forecast: rtl_line (forecast)
    - rtl_arc: rtl_arc (system clock)
    - rtl_arc_forecast: rtl_arc (forecast)
    - vertical: vertical (system clock)
    - vertical_forecast: vertical (forecast)
- moonGeometry: "6-82/82"
  $name: "[Sky] Moon radius-center X/Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius sets size in all modes, X/Y used when movement is fixed or vertical X). Default: 6-82/82."
- moonPhaseOverride: "-1"
  $name: "[Sky] Moon phase override (-1 = real phase)"
  $description: "Set -1 for real date-based lunar phase (0.0 = new moon, 0.5 = full moon, 1.0 = next new moon), or 0.0..1.0 to force. Invalid resets to -1."
- celestialInFrontOfClouds: false
  $name: "[Sky] Render sun & moon in front of clouds"
  $description: "When false (default), clouds dim/occlude the sun and moon. When true, sun and moon render in front of clouds with full brightness."

# --- 5. PRECIPITATION (RAIN / SNOW / HAIL) ---
- precipitationMode: auto
  $name: "[Precipitation] Mode"
  $description: "Precipitation layer mode. Allowed: off, auto, rain, snow, hail. Default: auto."
  $options:
    - off: Off
    - auto: Auto (forecast)
    - rain: Rain
    - snow: Snow
    - hail: Hail
- precipFallDirection: auto
  $name: "[Precipitation] Fall direction"
  $description: "Fall direction for precipitation. Allowed: auto, down, down_left, down_right. Default: auto."
  $options:
    - auto: Auto (forecast)
    - down: Down
    - down_left: Down-left
    - down_right: Down-right
- precipSpeedMode: auto
  $name: "[Precipitation] Speed mode"
  $description: "auto = scale speed by forecast precipitation intensity & kind (snow slower, rain moderate, hail fastest); custom = use fall speed below."
  $options:
    - auto: Auto (forecast)
    - custom: Custom
- precipFallSpeedMps: "6.5"
  $name: "[Precipitation] Fall speed (m/s)"
  $description: "Custom fall speed in m/s when speed mode is custom (0.1 to 50.0). Invalid/out-of-range resets to 6.5."
- precipIntensity: 25
  $name: "[Precipitation] Intensity (%)"
  $description: "Shared intensity for rain, snow, and hail (0..100). Any value outside 0..100 or invalid resets to 25."

# --- 6. EFFECTS (INCLUDING TIME OF DAY) ---
- effectsIntensity: 25
  $name: "[Effects] General intensity (%)"
  $description: "Strength/opacity for effects (0..100). Outside 0..100 or invalid resets to 25."
- timeOfDayMode: auto
  $name: "[Effects] Time of day mode"
  $description: "System-clock time-of-day tinting. Allowed: off, auto, dawn, day, sunset, night. Default: auto."
  $options:
    - off: Off
    - auto: Auto (system clock)
    - dawn: Dawn
    - day: Day
    - sunset: Sunset
    - night: Night
- timeOfDayIntensity: 25
  $name: "[Effects] Time of day intensity (%)"
  $description: "Time-of-day tint strength (0..100). Invalid resets to 25."
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
  $description: "Soft mist/fog layer (auto triggers on fog code or high humidity with low visibility). Allowed: auto, on, off."
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
  $description: "Soft horizon shimmer wave band (auto triggers on hot daytime weather). Allowed: auto, on, off."
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
  $name: "[Effects] Rainbow radius-center X/Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius 0 = off, 100 = min(width, height); X left->right, Y bottom->top). Default: 55-50/15."
- effectWindStreams: auto
  $name: "[Effects] Wind gusts & streams"
  $description: "Animated wind streaks on strong gusts (auto triggers when wind gusts >= 32 km/h or wind >= 24 km/h). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectThermal: auto
  $name: "[Effects] Frost / Heat tint"
  $description: "Subtle frost rim at <= 0 C or warm heat haze at >= 30 C apparent temperature. Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectNightStars: auto
  $name: "[Effects] Night stars"
  $description: "Twinkling stars in upper sky at night with low cloud cover. Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectAirQuality: auto
  $name: "[Effects] Air quality (AQI / PM2.5)"
  $description: "Air quality haze & micro-particles (auto triggers on elevated AQI / PM2.5). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectPollen: auto
  $name: "[Effects] Pollen particles"
  $description: "Floating pollen motes (auto triggers on elevated pollen index in air quality data). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off
- effectFloodWater: auto
  $name: "[Effects] Rising water / flood level"
  $description: "Animated water band along the bottom boundary (auto triggers on heavy rain/precipitation). Allowed: auto, on, off."
  $options:
    - auto: Auto (forecast)
    - on: On
    - off: Off

# --- 7. COLORS (CONFIG ONLY, '/' LISTS) ---
- skyPrecipColors: "#6BB8FF/#FBFDFF/#C2E6FF/#E6ECF7/#FFD152/#DCE7FF"
  $name: "[Sky + Precipitation] Colors (#RRGGBB/...)"
  $description: "6 HEX colors separated by '/': rain/snow/hail/clouds/sun/moon (empty segment = default)."
- effectsColors: "#FFDA9E/#FFE9A8/#E3EAF2/#8C96A6/#D6AE73/#D1D9E0/#8F7F6B/#D6EBFF/#38EB9E/#FADBA8/#FFC759/#D1A857/#F7E36B/#2980C2"
  $name: "[Effects] Colors (#RRGGBB/...)"
  $description: "14 HEX colors separated by '/': timeOfDay/clear/partlyCloudy/overcast/dust/fog/smog/thunderstorm/aurora/mirage/rainbow/airQuality/pollen/floodWater (empty segment = default)."
- colorHudBg: "#141A26"
  $name: "[HUD] Background (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default dark slate card background. Invalid HEX resets to default."
- colorHudText: "#F0F5FC"
  $name: "[HUD] Text (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default crisp off-white text. Invalid HEX resets to default."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <wininet.h>
#include <d2d1.h>
#include <dwrite.h>
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
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

constexpr wchar_t kOverlayWndClass[] = L"WindhawkDesktopWeatherOverlayClass";
constexpr double kSynodicMonthDays = 29.530588853;
constexpr double kReferenceNewMoonEpochSec = 947182440.0;
constexpr double kPi = 3.14159265358979323846;

enum class ObjectKind { Rain, Snow, Hail };
enum class PerformanceMode { Low, Medium, High };
enum class PrecipMode { Off, Auto, Rain, Snow, Hail };
enum class PrecipSpeedMode { Auto, Custom };
enum class TriStateMode { Auto, On, Off };
enum class TimeOfDayMode { Off, Auto, Dawn, Day, Sunset, Night };
enum class CelestialMotionMode {
    Fixed, LtrLine, LtrLineForecast, LtrArc, LtrArcForecast,
    RtlLine, RtlLineForecast, RtlArc, RtlArcForecast, Vertical, VerticalForecast
};
enum class FallDirMode { Auto, Down, DownLeft, DownRight };
enum class ExpandedSection { None, Sky, Precipitation, Effects };
enum class HudFieldId {
    None, Coords, CloudsIntensity, SunUvOverride,
    MoonPhaseOverride, PrecipSpeedMps, PrecipIntensity, EffectsIntensity
};

struct GeneralEffectsState {
    bool clear = false, partlyCloudy = false, overcast = false, dust = false;
    bool fog = false, smog = false, thunderstorm = false, aurora = false;
    bool mirage = false, rainbow = false, windStreams = false, frost = false;
    bool heat = false, nightStars = false, airQuality = false, pollen = false, floodWater = false;
};

struct ArcGeometry { int radius = 50, centerX = 50, centerY = 50; };
struct CachedArcLayout {
    int canvasW = 0, canvasH = 0, pctRadius = -1, pctX = -1, pctY = -1;
    float radius = 0.0f, cx = 0.0f, cy = 0.0f, bandThickness = 0.0f;
    bool valid = false;
};
struct MonitorCanvasRect { float left = 0.0f, top = 0.0f, width = 0.0f, height = 0.0f, bottomLimitRelY = 0.0f; };
struct CustomInsetsPx { int left = 0, right = 0, top = 0, bottom = 10; };

struct ForecastSnapshot {
    bool valid = false;
    int weatherCode = -1;
    double tempC = 0.0, apparentTempC = 15.0, humidityPct = 55.0;
    double windSpeedKmh = 0.0, windGustsKmh = 0.0, windDirDeg = 0.0;
    int windDirSign = 0;
    double cloudCoverPct = 20.0, visibilityM = 12000.0, uvIndex = 2.0, precipMm = 0.0;
    double aqiValue = 25.0, pm25Value = 8.0, pollenMaxValue = 0.0;
    bool hasSunTimes = false;
    double sunriseHour = 6.5, sunsetHour = 18.0;
    bool isDay = true, hasPrecip = false;
    ObjectKind precipKind = ObjectKind::Rain;
    GeneralEffectsState effects;
    std::wstring lastUpdateText = L"--:--", sourceLabel = L"none";
};

struct Settings {
    std::wstring layerAttachMode = L"auto";
    PerformanceMode performanceMode = PerformanceMode::High;
    std::wstring bottomBoundaryMode = L"above_taskbar";
    int bottomOffsetPx = 10;
    bool taskbarAsBorder = true;
    CustomInsetsPx customOffset{0, 0, 0, 10};
    std::wstring multiMonitorMode = L"primary";
    int targetFps = 60;
    bool pauseOnFullscreen = true;

    std::wstring primarySource = L"open_meteo", fallbackSource = L"off";
    bool hasValidCoords = false;
    std::wstring coordsRaw;
    double latitude = 0.0, longitude = 0.0;
    std::wstring customApiUrl, localWeatherPath;
    int updateIntervalMin = 30;

    bool forecastHudEnabled = false;
    float hudButtonRadiusPx = 24.0f, hudFontSizePx = 12.0f;
    ArcGeometry hudButtonGeom{4, 92, 88};
    bool hasCustomBtnPos = false;
    double hudBtnRelX = 0.92, hudBtnRelY = 0.12;
    int forecastHudMarginPx = 14, forecastHudOpacity = 84;

    TimeOfDayMode timeOfDayMode = TimeOfDayMode::Auto;
    int timeOfDayIntensity = 25;
    TriStateMode cloudsMode = TriStateMode::Auto;
    int cloudsIntensity = 35;
    TriStateMode sunMode = TriStateMode::Auto;
    CelestialMotionMode sunMotionMode = CelestialMotionMode::RtlArc;
    ArcGeometry sunGeom{7, 82, 82};
    double sunUvOverride = -1.0;
    TriStateMode moonMode = TriStateMode::Auto;
    CelestialMotionMode moonMotionMode = CelestialMotionMode::RtlArc;
    ArcGeometry moonGeom{6, 82, 82};
    double moonPhaseOverride = -1.0;
    bool celestialInFrontOfClouds = false;

    PrecipMode precipitationMode = PrecipMode::Auto;
    FallDirMode precipFallDirection = FallDirMode::Auto;
    PrecipSpeedMode precipSpeedMode = PrecipSpeedMode::Auto;
    double precipFallSpeedMps = 6.5;
    int precipIntensity = 25;

    int effectsIntensity = 25;
    TriStateMode effectClear = TriStateMode::Auto, effectPartlyCloudy = TriStateMode::Auto, effectOvercast = TriStateMode::Auto;
    TriStateMode effectDust = TriStateMode::Auto, effectFog = TriStateMode::Auto, effectSmog = TriStateMode::Auto;
    TriStateMode effectThunderstorm = TriStateMode::Auto, effectAurora = TriStateMode::Auto, effectMirage = TriStateMode::Auto;
    TriStateMode effectRainbow = TriStateMode::Auto, effectWindStreams = TriStateMode::Auto, effectThermal = TriStateMode::Auto;
    TriStateMode effectNightStars = TriStateMode::Auto, effectAirQuality = TriStateMode::Auto, effectPollen = TriStateMode::Auto;
    TriStateMode effectFloodWater = TriStateMode::Auto;
    ArcGeometry rainbowGeom{55, 50, 15};

    bool hasCustomSnowColor = false;
    D2D1_COLOR_F colorRain = D2D1::ColorF(0.42f, 0.72f, 1.0f, 0.78f);
    D2D1_COLOR_F colorSnow = D2D1::ColorF(0.98f, 0.99f, 1.0f, 0.96f);
    D2D1_COLOR_F colorHail = D2D1::ColorF(0.76f, 0.90f, 1.0f, 0.95f);
    bool hasCustomTimeOfDayColor = false, hasCustomCloudsColor = false, hasCustomSunColor = false, hasCustomMoonColor = false;
    D2D1_COLOR_F colorTimeOfDay = D2D1::ColorF(1.0f, 0.85f, 0.65f, 0.16f);
    D2D1_COLOR_F colorClouds = D2D1::ColorF(0.90f, 0.93f, 0.97f, 0.34f);
    D2D1_COLOR_F colorSun = D2D1::ColorF(1.0f, 0.82f, 0.32f, 0.88f);
    D2D1_COLOR_F colorMoon = D2D1::ColorF(0.86f, 0.91f, 1.0f, 0.94f);
    bool hasCustomClearColor = false, hasCustomPartlyCloudyColor = false, hasCustomOvercastColor = false, hasCustomDustColor = false;
    D2D1_COLOR_F colorClear = D2D1::ColorF(1.0f, 0.92f, 0.65f, 0.25f);
    D2D1_COLOR_F colorPartlyCloudy = D2D1::ColorF(0.88f, 0.91f, 0.95f, 0.22f);
    D2D1_COLOR_F colorOvercast = D2D1::ColorF(0.55f, 0.59f, 0.65f, 0.35f);
    D2D1_COLOR_F colorDust = D2D1::ColorF(0.84f, 0.68f, 0.45f, 0.28f);
    bool hasCustomFogColor = false, hasCustomSmogColor = false, hasCustomThunderstormColor = false, hasCustomAuroraColor = false;
    D2D1_COLOR_F colorFog = D2D1::ColorF(0.82f, 0.86f, 0.90f, 0.35f);
    D2D1_COLOR_F colorSmog = D2D1::ColorF(0.56f, 0.50f, 0.42f, 0.36f);
    D2D1_COLOR_F colorThunderstorm = D2D1::ColorF(0.84f, 0.92f, 1.0f, 0.95f);
    D2D1_COLOR_F colorAurora = D2D1::ColorF(0.22f, 0.92f, 0.62f, 0.32f);
    bool hasCustomMirageColor = false, hasCustomRainbowColor = false, hasCustomAirQualityColor = false, hasCustomPollenColor = false, hasCustomFloodWaterColor = false;
    D2D1_COLOR_F colorMirage = D2D1::ColorF(0.98f, 0.86f, 0.66f, 0.25f);
    D2D1_COLOR_F colorRainbow = D2D1::ColorF(1.0f, 0.78f, 0.35f, 0.35f);
    D2D1_COLOR_F colorAirQuality = D2D1::ColorF(0.78f, 0.68f, 0.38f, 0.24f);
    D2D1_COLOR_F colorPollen = D2D1::ColorF(0.96f, 0.88f, 0.42f, 0.72f);
    D2D1_COLOR_F colorFloodWater = D2D1::ColorF(0.18f, 0.52f, 0.76f, 0.42f);
    D2D1_COLOR_F colorHudBg = D2D1::ColorF(0.08f, 0.10f, 0.15f, 0.84f);
    D2D1_COLOR_F colorHudText = D2D1::ColorF(0.94f, 0.96f, 0.99f, 0.96f);
};

struct InteractiveControl {
    D2D1_RECT_F rect{};
    std::wstring actionId;
    HudFieldId fieldId = HudFieldId::None;
    std::wstring btnText;
    D2D1_COLOR_F tint = D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.90f);
};

struct FallingItem {
    ObjectKind kind = ObjectKind::Snow;
    float x = 0.0f, y = 0.0f, size = 6.0f, angle = 0.0f, spin = 0.0f, swayPhase = 0.0f, alpha = 0.9f;
    int depthTier = 1;
};

struct LightningFlash { float timer = 0.0f, xStart = 0.0f, xEnd = 0.0f; };

struct CachedSurface {
    ID2D1DCRenderTarget* rt = nullptr;
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    ID2D1Bitmap* flakeSprite = nullptr;
    ID2D1SolidColorBrush* sharedBrush = nullptr;
    D2D1_COLOR_F spriteColor = D2D1::ColorF(0, 0, 0, 0);
    int width = 0, height = 0;
};

std::mutex g_mutex;
Settings g_settings;
bool g_arcCacheDirty = true;
ForecastSnapshot g_forecast;

HWND g_overlayHwnd = nullptr;
HANDLE g_weatherWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_renderThread;
[[clang::no_destroy]] std::optional<std::thread> g_weatherThread;
std::atomic<bool> g_running{false};

std::atomic<bool> g_hudExpanded{false};
std::atomic<float> g_hudBtnCx{0.0f};
std::atomic<float> g_hudBtnCy{0.0f};
std::atomic<float> g_hudBtnR{24.0f};

std::mutex g_hudUiMutex;
bool g_draggingHudBtn = false, g_movedDuringDrag = false;
int g_dragStartScreenX = 0, g_dragStartScreenY = 0;
float g_dragStartBtnCx = 0.0f, g_dragStartBtnCy = 0.0f;
int g_hudScrollPx = 0, g_hudMaxScrollPx = 0;
ExpandedSection g_expandedSection = ExpandedSection::None;
HudFieldId g_focusedField = HudFieldId::None;
std::wstring g_editBuffer;
std::vector<InteractiveControl> g_hudControls;

ID2D1Factory* g_d2dFactory = nullptr;
IDWriteFactory* g_dwriteFactory = nullptr;
IDWriteTextFormat* g_hudTitleFormat = nullptr;
IDWriteTextFormat* g_hudBodyFormat = nullptr;
IDWriteTextFormat* g_hudCircleFormat = nullptr;
IDWriteTextFormat* g_hudPillFormat = nullptr;
float g_cachedFontSizePx = 0.0f, g_cachedCircleFontPx = 0.0f;
CachedSurface g_surface;
CachedArcLayout g_rainbowCache;

int g_canvasLeft = 0, g_canvasTop = 0, g_canvasWidth = 0, g_canvasHeight = 0;
std::vector<MonitorCanvasRect> g_monitorRects;
std::vector<FallingItem> g_falling;
LightningFlash g_lightning;
float GetQualityScale(PerformanceMode mode) {
    if (mode == PerformanceMode::Low) return 0.55f;
    if (mode == PerformanceMode::High) return 1.35f;
    return 1.0f;
}

bool HasAnyForecastConsumer(const Settings& cfg) {
    return cfg.forecastHudEnabled ||
           cfg.precipitationMode == PrecipMode::Auto ||
           cfg.precipFallDirection == FallDirMode::Auto ||
           cfg.precipSpeedMode == PrecipSpeedMode::Auto ||
           cfg.cloudsMode == TriStateMode::Auto ||
           cfg.sunMode == TriStateMode::Auto ||
           cfg.sunMotionMode == CelestialMotionMode::LtrLineForecast ||
           cfg.sunMotionMode == CelestialMotionMode::LtrArcForecast ||
           cfg.sunMotionMode == CelestialMotionMode::RtlLineForecast ||
           cfg.sunMotionMode == CelestialMotionMode::RtlArcForecast ||
           cfg.sunMotionMode == CelestialMotionMode::VerticalForecast ||
           cfg.moonMotionMode == CelestialMotionMode::LtrLineForecast ||
           cfg.moonMotionMode == CelestialMotionMode::LtrArcForecast ||
           cfg.moonMotionMode == CelestialMotionMode::RtlLineForecast ||
           cfg.moonMotionMode == CelestialMotionMode::RtlArcForecast ||
           cfg.moonMotionMode == CelestialMotionMode::VerticalForecast ||
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
           cfg.effectWindStreams == TriStateMode::Auto ||
           cfg.effectThermal == TriStateMode::Auto ||
           cfg.effectNightStars == TriStateMode::Auto ||
           cfg.effectAirQuality == TriStateMode::Auto ||
           cfg.effectPollen == TriStateMode::Auto ||
           cfg.effectFloodWater == TriStateMode::Auto;
}

bool NeedsAirQualityFetch(const Settings& cfg) {
    return cfg.primarySource == L"open_meteo" && cfg.hasValidCoords &&
           (cfg.effectAirQuality == TriStateMode::Auto ||
            cfg.effectPollen == TriStateMode::Auto ||
            cfg.forecastHudEnabled);
}

void RefreshArcCache(CachedArcLayout& cache, const ArcGeometry& geom, bool forceDirty) {
    if (!forceDirty && cache.valid &&
        cache.canvasW == g_canvasWidth && cache.canvasH == g_canvasHeight &&
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
    for (auto& ch : s) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
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
    return (rawVal < minVal || rawVal > maxVal) ? defVal : rawVal;
}

double ParseDoubleRangeOrDefault(const std::wstring& raw, double minVal, double maxVal, double defVal) {
    double v = 0.0;
    if (!TryParseDoubleStrict(raw, v) || v < minVal || v > maxVal) return defVal;
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
    if (!TryParseDoubleStrict(s.substr(0, sep), lat) ||
        !TryParseDoubleStrict(s.substr(sep + 1), lon) ||
        lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
        return false;
    }
    outLat = lat;
    outLon = lon;
    return true;
}

void ParseCustomInsetsString(const std::wstring& raw, CustomInsetsPx& outInsets) {
    outInsets = CustomInsetsPx{0, 0, 0, 10};
    std::vector<std::wstring> parts;
    std::wstringstream ss(Trim(raw));
    std::wstring item;
    while (std::getline(ss, item, L'/')) {
        parts.push_back(Trim(item));
    }
    if (parts.size() != 4) return;

    auto parseSide = [](const std::wstring& p, int defVal, int& outV) -> bool {
        if (p.empty()) {
            outV = defVal;
            return true;
        }
        try {
            int v = std::stoi(p);
            if (v < -500 || v > 1000) return false;
            outV = v;
            return true;
        } catch (...) {
            return false;
        }
    };

    CustomInsetsPx parsed;
    if (parseSide(parts[0], 0, parsed.left) &&
        parseSide(parts[1], 0, parsed.right) &&
        parseSide(parts[2], 0, parsed.top) &&
        parseSide(parts[3], 10, parsed.bottom)) {
        outInsets = parsed;
    }
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
        for (wchar_t c : p) {
            if (!iswdigit(c)) return false;
        }
        return true;
    };
    if (!isIntStr(rPart) || !isIntStr(xPart) || !isIntStr(yPart)) return;

    try {
        int r = std::stoi(rPart);
        int x = std::stoi(xPart);
        int y = std::stoi(yPart);
        if (r < 0 || r > 100 || x < 0 || x > 100 || y < 0 || y > 100) return;
        outGeom = {r, x, y};
    } catch (...) {
    }
}

std::vector<std::wstring> SplitSlashTokens(const std::wstring& raw) {
    std::vector<std::wstring> tokens;
    std::wstringstream ss(raw);
    std::wstring item;
    while (std::getline(ss, item, L'/')) {
        tokens.push_back(Trim(item));
    }
    return tokens;
}

PerformanceMode ParsePerformanceMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"low") return PerformanceMode::Low;
    if (v == L"medium") return PerformanceMode::Medium;
    return PerformanceMode::High;
}

PrecipMode ParsePrecipMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"off") return PrecipMode::Off;
    if (v == L"rain") return PrecipMode::Rain;
    if (v == L"snow") return PrecipMode::Snow;
    if (v == L"hail") return PrecipMode::Hail;
    return PrecipMode::Auto;
}

PrecipSpeedMode ParsePrecipSpeedMode(const std::wstring& s) {
    return ToLower(Trim(s)) == L"custom" ? PrecipSpeedMode::Custom : PrecipSpeedMode::Auto;
}

TriStateMode ParseTriState(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"on") return TriStateMode::On;
    if (v == L"off") return TriStateMode::Off;
    return TriStateMode::Auto;
}

TimeOfDayMode ParseTimeOfDayMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"off") return TimeOfDayMode::Off;
    if (v == L"dawn") return TimeOfDayMode::Dawn;
    if (v == L"day") return TimeOfDayMode::Day;
    if (v == L"sunset") return TimeOfDayMode::Sunset;
    if (v == L"night") return TimeOfDayMode::Night;
    return TimeOfDayMode::Auto;
}

CelestialMotionMode ParseCelestialMotion(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"fixed") return CelestialMotionMode::Fixed;
    if (v == L"ltr_line") return CelestialMotionMode::LtrLine;
    if (v == L"ltr_line_forecast") return CelestialMotionMode::LtrLineForecast;
    if (v == L"ltr_arc") return CelestialMotionMode::LtrArc;
    if (v == L"ltr_arc_forecast") return CelestialMotionMode::LtrArcForecast;
    if (v == L"rtl_line") return CelestialMotionMode::RtlLine;
    if (v == L"rtl_line_forecast") return CelestialMotionMode::RtlLineForecast;
    if (v == L"rtl_arc") return CelestialMotionMode::RtlArc;
    if (v == L"rtl_arc_forecast") return CelestialMotionMode::RtlArcForecast;
    if (v == L"vertical") return CelestialMotionMode::Vertical;
    if (v == L"vertical_forecast") return CelestialMotionMode::VerticalForecast;
    return CelestialMotionMode::RtlArc;
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
    return (norm >= 45.0 && norm < 225.0) ? 1 : -1;
}

const wchar_t* WindDegToCompass(double deg) {
    double norm = std::fmod(deg, 360.0);
    if (norm < 0.0) norm += 360.0;
    if (norm >= 337.5 || norm < 22.5) return L"N";
    if (norm < 67.5) return L"NE";
    if (norm < 112.5) return L"E";
    if (norm < 157.5) return L"SE";
    if (norm < 202.5) return L"S";
    if (norm < 247.5) return L"SW";
    if (norm < 292.5) return L"W";
    return L"NW";
}

const wchar_t* WmoCodeToLabel(int code) {
    if (code < 0) return L"No forecast";
    if (code == 0) return L"Clear sky";
    if (code == 1) return L"Mainly clear";
    if (code == 2) return L"Partly cloudy";
    if (code == 3) return L"Overcast";
    if (code == 45 || code == 48) return L"Fog";
    if (code >= 51 && code <= 57) return L"Drizzle";
    if ((code >= 61 && code <= 67) || (code >= 80 && code <= 82)) return L"Rain";
    if (code == 71 || code == 73 || code == 75 || code == 77 || code == 85 || code == 86) return L"Snow";
    if (code == 95) return L"Thunderstorm";
    if (code == 96 || code == 99) return L"Thunderstorm + Hail";
    return L"Mixed weather";
}

double ComputeRealMoonPhase() {
    auto nowSec = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    double daysSinceNew = (static_cast<double>(nowSec) - kReferenceNewMoonEpochSec) / 86400.0;
    double cycle = std::fmod(daysSinceNew, kSynodicMonthDays);
    if (cycle < 0.0) cycle += kSynodicMonthDays;
    return std::clamp(cycle / kSynodicMonthDays, 0.0, 1.0);
}

const wchar_t* MoonPhaseToLabel(double phase01) {
    if (phase01 < 0.0625 || phase01 >= 0.9375) return L"New Moon";
    if (phase01 < 0.1875) return L"Waxing Crescent";
    if (phase01 < 0.3125) return L"First Quarter";
    if (phase01 < 0.4375) return L"Waxing Gibbous";
    if (phase01 < 0.5625) return L"Full Moon";
    if (phase01 < 0.6875) return L"Waning Gibbous";
    if (phase01 < 0.8125) return L"Last Quarter";
    return L"Waning Crescent";
}

double GetLocalHourFloat() {
    SYSTEMTIME st = {};
    GetLocalTime(&st);
    return static_cast<double>(st.wHour) +
           static_cast<double>(st.wMinute) / 60.0 +
           static_cast<double>(st.wSecond) / 3600.0;
}

bool TryExtractIsoTimeHour(const std::string& json, const std::string& key, double& outHour) {
    size_t pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return false;
    size_t arrStart = json.find('[', pos);
    if (arrStart == std::string::npos) return false;
    size_t q1 = json.find('"', arrStart);
    if (q1 == std::string::npos) return false;
    size_t q2 = json.find('"', q1 + 1);
    if (q2 == std::string::npos) return false;
    std::string iso = json.substr(q1 + 1, q2 - q1 - 1);
    size_t tPos = iso.find('T');
    if (tPos == std::string::npos || tPos + 5 >= iso.size()) return false;
    int hh = std::atoi(iso.substr(tPos + 1, 2).c_str());
    int mm = std::atoi(iso.substr(tPos + 4, 2).c_str());
    if (hh < 0 || hh > 23 || mm < 0 || mm > 59) return false;
    outHour = static_cast<double>(hh) + static_cast<double>(mm) / 60.0;
    return true;
}

void GetSystemTimeOfDayPalette(double hour, D2D1_COLOR_F& outTop, D2D1_COLOR_F& outBot,
                               bool& outIsNight, const wchar_t*& outPhaseLabel) {
    auto lerpCol = [](D2D1_COLOR_F a, D2D1_COLOR_F b, float t) -> D2D1_COLOR_F {
        t = std::clamp(t, 0.0f, 1.0f);
        return D2D1::ColorF(
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t);
    };
    D2D1_COLOR_F nightTop = D2D1::ColorF(0.06f, 0.10f, 0.24f, 0.24f);
    D2D1_COLOR_F nightBot = D2D1::ColorF(0.10f, 0.14f, 0.30f, 0.14f);
    D2D1_COLOR_F dawnTop  = D2D1::ColorF(0.95f, 0.58f, 0.45f, 0.20f);
    D2D1_COLOR_F dawnBot  = D2D1::ColorF(1.00f, 0.82f, 0.58f, 0.10f);
    D2D1_COLOR_F dayTop   = D2D1::ColorF(0.75f, 0.88f, 1.00f, 0.08f);
    D2D1_COLOR_F dayBot   = D2D1::ColorF(1.00f, 0.97f, 0.88f, 0.04f);
    D2D1_COLOR_F duskTop  = D2D1::ColorF(0.88f, 0.42f, 0.36f, 0.22f);
    D2D1_COLOR_F duskBot  = D2D1::ColorF(0.98f, 0.68f, 0.32f, 0.12f);

    outIsNight = (hour < 5.5 || hour >= 19.5);
    if (hour < 5.0 || hour >= 21.5) {
        outTop = nightTop; outBot = nightBot; outPhaseLabel = L"Night";
    } else if (hour < 7.5) {
        float t = static_cast<float>((hour - 5.0) / 2.5);
        outTop = lerpCol(nightTop, dawnTop, t);
        outBot = lerpCol(nightBot, dawnBot, t);
        outPhaseLabel = L"Dawn";
    } else if (hour < 10.0) {
        float t = static_cast<float>((hour - 7.5) / 2.5);
        outTop = lerpCol(dawnTop, dayTop, t);
        outBot = lerpCol(dawnBot, dayBot, t);
        outPhaseLabel = L"Day";
    } else if (hour < 16.5) {
        outTop = dayTop; outBot = dayBot; outPhaseLabel = L"Day";
    } else if (hour < 19.0) {
        float t = static_cast<float>((hour - 16.5) / 2.5);
        outTop = lerpCol(dayTop, duskTop, t);
        outBot = lerpCol(dayBot, duskBot, t);
        outPhaseLabel = L"Sunset";
    } else {
        float t = static_cast<float>((hour - 19.0) / 2.5);
        outTop = lerpCol(duskTop, nightTop, t);
        outBot = lerpCol(duskBot, nightBot, t);
        outPhaseLabel = L"Sunset";
    }
}
float ComputeMonitorEdgeFade(const MonitorCanvasRect& mon, float cx, float cy, float padPx) {
    float fadeDist = std::max(24.0f, padPx);
    float minEdge = std::min({
        cx - mon.left,
        (mon.left + mon.width) - cx,
        cy - mon.top,
        (mon.top + mon.height) - cy
    });
    if (minEdge <= 0.0f) return 0.0f;
    if (minEdge >= fadeDist) return 1.0f;
    float t = minEdge / fadeDist;
    return t * t * (3.0f - 2.0f * t);
}

void ComputeCelestialCoordsInMonitor(const MonitorCanvasRect& mon, bool isSun, CelestialMotionMode motion,
                                     const ArcGeometry& geom, double hour, const ForecastSnapshot& snap,
                                     float& outRadius, float& outCx, float& outCy) {
    float minSide = std::min(mon.width, mon.height);
    outRadius = minSide * (static_cast<float>(geom.radius) / 100.0f);
    if (motion == CelestialMotionMode::Fixed) {
        outCx = mon.left + mon.width * (static_cast<float>(geom.centerX) / 100.0f);
        outCy = mon.top + mon.height * (1.0f - static_cast<float>(geom.centerY) / 100.0f);
        return;
    }

    bool usesForecast = (motion == CelestialMotionMode::LtrLineForecast ||
                         motion == CelestialMotionMode::LtrArcForecast ||
                         motion == CelestialMotionMode::RtlLineForecast ||
                         motion == CelestialMotionMode::RtlArcForecast ||
                         motion == CelestialMotionMode::VerticalForecast);
    double startH = isSun ? 5.5 : 19.5;
    double endH = isSun ? 19.5 : 5.5;
    if (usesForecast && snap.hasSunTimes) {
        startH = isSun ? snap.sunriseHour : snap.sunsetHour;
        endH = isSun ? snap.sunsetHour : snap.sunriseHour;
    }
    double duration = endH - startH;
    if (duration <= 0.5) duration += 24.0;
    double elapsed = hour - startH;
    if (elapsed < 0.0) elapsed += 24.0;
    double progress = std::clamp(elapsed / duration, 0.0, 1.0);

    if (motion == CelestialMotionMode::Vertical || motion == CelestialMotionMode::VerticalForecast) {
        float fixedX = mon.left + mon.width * (static_cast<float>(geom.centerX) / 100.0f);
        float bottomY = mon.top + mon.bottomLimitRelY;
        float topY = mon.top + mon.height * 0.14f;
        outCx = fixedX;
        outCy = bottomY - static_cast<float>(std::sin(kPi * progress)) * std::max(20.0f, bottomY - topY);
        return;
    }

    bool rightToLeft = (motion == CelestialMotionMode::RtlLine ||
                        motion == CelestialMotionMode::RtlLineForecast ||
                        motion == CelestialMotionMode::RtlArc ||
                        motion == CelestialMotionMode::RtlArcForecast);
    bool useArc = (motion == CelestialMotionMode::LtrArc ||
                   motion == CelestialMotionMode::LtrArcForecast ||
                   motion == CelestialMotionMode::RtlArc ||
                   motion == CelestialMotionMode::RtlArcForecast);
    double xPct = rightToLeft ? (90.0 - 80.0 * progress) : (10.0 + 80.0 * progress);
    double yPct = useArc ? (22.0 + 62.0 * std::sin(kPi * progress)) : (78.0 - 8.0 * std::sin(kPi * progress));
    outCx = mon.left + mon.width * static_cast<float>(xPct / 100.0);
    outCy = mon.top + mon.height * static_cast<float>(1.0 - yPct / 100.0);
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
        outColor = D2D1::ColorF(
            ((rgb >> 16) & 0xFF) / 255.0f,
            ((rgb >> 8) & 0xFF) / 255.0f,
            (rgb & 0xFF) / 255.0f,
            defaultAlpha);
        return true;
    } catch (...) {
        return false;
    }
}

void ApplySkyPrecipColors(const std::wstring& raw, Settings& cfg) {
    std::vector<std::wstring> parts = SplitSlashTokens(raw);
    if (parts.size() > 0) TryParseHexColorStrict(parts[0], 0.78f, cfg.colorRain);
    if (parts.size() > 1) cfg.hasCustomSnowColor = TryParseHexColorStrict(parts[1], 0.96f, cfg.colorSnow);
    if (parts.size() > 2) TryParseHexColorStrict(parts[2], 0.95f, cfg.colorHail);
    if (parts.size() > 3) cfg.hasCustomCloudsColor = TryParseHexColorStrict(parts[3], 0.34f, cfg.colorClouds);
    if (parts.size() > 4) cfg.hasCustomSunColor = TryParseHexColorStrict(parts[4], 0.88f, cfg.colorSun);
    if (parts.size() > 5) cfg.hasCustomMoonColor = TryParseHexColorStrict(parts[5], 0.94f, cfg.colorMoon);
}

void ApplyEffectsColors(const std::wstring& raw, Settings& cfg) {
    std::vector<std::wstring> parts = SplitSlashTokens(raw);
    if (parts.size() > 0) cfg.hasCustomTimeOfDayColor = TryParseHexColorStrict(parts[0], 0.16f, cfg.colorTimeOfDay);
    if (parts.size() > 1) cfg.hasCustomClearColor = TryParseHexColorStrict(parts[1], 0.25f, cfg.colorClear);
    if (parts.size() > 2) cfg.hasCustomPartlyCloudyColor = TryParseHexColorStrict(parts[2], 0.22f, cfg.colorPartlyCloudy);
    if (parts.size() > 3) cfg.hasCustomOvercastColor = TryParseHexColorStrict(parts[3], 0.35f, cfg.colorOvercast);
    if (parts.size() > 4) cfg.hasCustomDustColor = TryParseHexColorStrict(parts[4], 0.28f, cfg.colorDust);
    if (parts.size() > 5) cfg.hasCustomFogColor = TryParseHexColorStrict(parts[5], 0.35f, cfg.colorFog);
    if (parts.size() > 6) cfg.hasCustomSmogColor = TryParseHexColorStrict(parts[6], 0.36f, cfg.colorSmog);
    if (parts.size() > 7) cfg.hasCustomThunderstormColor = TryParseHexColorStrict(parts[7], 0.95f, cfg.colorThunderstorm);
    if (parts.size() > 8) cfg.hasCustomAuroraColor = TryParseHexColorStrict(parts[8], 0.32f, cfg.colorAurora);
    if (parts.size() > 9) cfg.hasCustomMirageColor = TryParseHexColorStrict(parts[9], 0.25f, cfg.colorMirage);
    if (parts.size() > 10) cfg.hasCustomRainbowColor = TryParseHexColorStrict(parts[10], 0.35f, cfg.colorRainbow);
    if (parts.size() > 11) cfg.hasCustomAirQualityColor = TryParseHexColorStrict(parts[11], 0.24f, cfg.colorAirQuality);
    if (parts.size() > 12) cfg.hasCustomPollenColor = TryParseHexColorStrict(parts[12], 0.72f, cfg.colorPollen);
    if (parts.size() > 13) cfg.hasCustomFloodWaterColor = TryParseHexColorStrict(parts[13], 0.42f, cfg.colorFloodWater);
}

void LoadSettingsFromWh() {
    Settings next;
    auto readStr = [](LPCWSTR name, const std::wstring& defVal) -> std::wstring {
        WindhawkUtils::StringSetting raw(Wh_GetStringSetting(name));
        std::wstring out(raw.get());
        return out.empty() ? defVal : out;
    };

    next.layerAttachMode = ToLower(Trim(readStr(L"layerAttachMode", L"auto")));
    next.performanceMode = ParsePerformanceMode(readStr(L"performanceMode", L"high"));
    next.bottomBoundaryMode = ToLower(Trim(readStr(L"bottomBoundaryMode", L"above_taskbar")));
    next.bottomOffsetPx = ParseIntRangeOrDefault(Wh_GetIntSetting(L"bottomOffsetPx"), -500, 1000, 10);
    next.taskbarAsBorder = Wh_GetIntSetting(L"taskbarAsBorder") != 0;
    ParseCustomInsetsString(readStr(L"customOffset", L"0/0/0/10"), next.customOffset);
    next.multiMonitorMode = ToLower(Trim(readStr(L"multiMonitorMode", L"primary")));
    int fpsRaw = Wh_GetIntSetting(L"targetFps");
    next.targetFps = (fpsRaw == 30 || fpsRaw == 60) ? fpsRaw : 60;
    next.pauseOnFullscreen = Wh_GetIntSetting(L"pauseOnFullscreen") != 0;

    next.primarySource = ToLower(Trim(readStr(L"primarySource", L"open_meteo")));
    next.fallbackSource = ToLower(Trim(readStr(L"fallbackSource", L"off")));
    next.coordsRaw = Trim(readStr(L"coords", L""));
    next.hasValidCoords = TryParseCoordsString(next.coordsRaw, next.latitude, next.longitude);
    next.customApiUrl = Trim(readStr(L"customApiUrl", L""));
    next.localWeatherPath = Trim(readStr(L"localWeatherPath", L""));
    next.updateIntervalMin = ParseIntRangeOrDefault(Wh_GetIntSetting(L"updateIntervalMin"), 1, 1440, 30);

    next.forecastHudEnabled = ToLower(Trim(readStr(L"forecastHudMode", L"off"))) == L"on";
    next.hudButtonRadiusPx = static_cast<float>(ParseIntRangeOrDefault(Wh_GetIntSetting(L"hudButtonRadiusPx"), 14, 96, 24));
    next.hudFontSizePx = static_cast<float>(ParseIntRangeOrDefault(Wh_GetIntSetting(L"hudFontSizePx"), 9, 22, 12));
    ParseArcGeometryString(readStr(L"hudButtonGeometry", L"4-92/88"), ArcGeometry{4, 92, 88}, next.hudButtonGeom);
    std::wstring savedBtnPos = readStr(L"hudButtonPos", L"");
    size_t slashPos = savedBtnPos.find(L'/');
    if (slashPos != std::wstring::npos) {
        double rx = 0.0, ry = 0.0;
        if (TryParseDoubleStrict(savedBtnPos.substr(0, slashPos), rx) &&
            TryParseDoubleStrict(savedBtnPos.substr(slashPos + 1), ry) &&
            rx >= 0.02 && rx <= 0.98 && ry >= 0.02 && ry <= 0.98) {
            next.hasCustomBtnPos = true;
            next.hudBtnRelX = rx;
            next.hudBtnRelY = ry;
        }
    }
    next.forecastHudMarginPx = ParseIntRangeOrDefault(Wh_GetIntSetting(L"forecastHudMarginPx"), 0, 400, 14);
    next.forecastHudOpacity = ParseIntRangeOrDefault(Wh_GetIntSetting(L"forecastHudOpacity"), 15, 100, 84);

    next.timeOfDayMode = ParseTimeOfDayMode(readStr(L"timeOfDayMode", L"auto"));
    next.timeOfDayIntensity = SanitizePercent(Wh_GetIntSetting(L"timeOfDayIntensity"));
    next.cloudsMode = ParseTriState(readStr(L"cloudsMode", L"auto"));
    next.cloudsIntensity = ParseIntRangeOrDefault(Wh_GetIntSetting(L"cloudsIntensity"), 0, 100, 35);
    next.sunMode = ParseTriState(readStr(L"sunMode", L"auto"));
    next.sunMotionMode = ParseCelestialMotion(readStr(L"sunMotionMode", L"rtl_arc"));
    ParseArcGeometryString(readStr(L"sunGeometry", L"7-82/82"), ArcGeometry{7, 82, 82}, next.sunGeom);
    double uvVal = -1.0;
    next.sunUvOverride = (TryParseDoubleStrict(readStr(L"sunUvOverride", L"-1"), uvVal) &&
                          ((uvVal >= 0.0 && uvVal <= 20.0) || std::abs(uvVal + 1.0) < 1e-6)) ? uvVal : -1.0;
    next.moonMode = ParseTriState(readStr(L"moonMode", L"auto"));
    next.moonMotionMode = ParseCelestialMotion(readStr(L"moonMotionMode", L"rtl_arc"));
    ParseArcGeometryString(readStr(L"moonGeometry", L"6-82/82"), ArcGeometry{6, 82, 82}, next.moonGeom);
    double phaseVal = -1.0;
    next.moonPhaseOverride = (TryParseDoubleStrict(readStr(L"moonPhaseOverride", L"-1"), phaseVal) &&
                            ((phaseVal >= 0.0 && phaseVal <= 1.0) || std::abs(phaseVal + 1.0) < 1e-6)) ? phaseVal : -1.0;
    next.celestialInFrontOfClouds = Wh_GetIntSetting(L"celestialInFrontOfClouds") != 0;

    next.precipitationMode = ParsePrecipMode(readStr(L"precipitationMode", L"auto"));
    next.precipFallDirection = ParseFallDir(readStr(L"precipFallDirection", L"auto"));
    next.precipSpeedMode = ParsePrecipSpeedMode(readStr(L"precipSpeedMode", L"auto"));
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
    next.effectWindStreams = ParseTriState(readStr(L"effectWindStreams", L"auto"));
    next.effectThermal = ParseTriState(readStr(L"effectThermal", L"auto"));
    next.effectNightStars = ParseTriState(readStr(L"effectNightStars", L"auto"));
    next.effectAirQuality = ParseTriState(readStr(L"effectAirQuality", L"auto"));
    next.effectPollen = ParseTriState(readStr(L"effectPollen", L"auto"));
    next.effectFloodWater = ParseTriState(readStr(L"effectFloodWater", L"auto"));
    ParseArcGeometryString(readStr(L"rainbowGeometry", L"55-50/15"), ArcGeometry{55, 50, 15}, next.rainbowGeom);

    ApplySkyPrecipColors(readStr(L"skyPrecipColors", L"#6BB8FF/#FBFDFF/#C2E6FF/#E6ECF7/#FFD152/#DCE7FF"), next);
    ApplyEffectsColors(readStr(L"effectsColors", L"#FFDA9E/#FFE9A8/#E3EAF2/#8C96A6/#D6AE73/#D1D9E0/#8F7F6B/#D6EBFF/#38EB9E/#FADBA8/#FFC759/#D1A857/#F7E36B/#2980C2"), next);

    float bgAlpha = std::clamp(static_cast<float>(next.forecastHudOpacity) / 100.0f, 0.15f, 1.0f);
    if (!TryParseHexColorStrict(readStr(L"colorHudBg", L"#141A26"), bgAlpha, next.colorHudBg)) {
        next.colorHudBg = D2D1::ColorF(0.08f, 0.10f, 0.15f, bgAlpha);
    }
    TryParseHexColorStrict(readStr(L"colorHudText", L"#F0F5FC"), 0.96f, next.colorHudText);

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_settings = next;
        g_arcCacheDirty = true;
    }
    if (g_weatherWakeEvent) SetEvent(g_weatherWakeEvent);
}

void ApplyRuntimeButtonPosition(float cx, float cy) {
    if (g_canvasWidth <= 1 || g_canvasHeight <= 1) return;
    std::lock_guard<std::mutex> lock(g_mutex);
    g_settings.hasCustomBtnPos = true;
    g_settings.hudBtnRelX = std::clamp(static_cast<double>(cx) / static_cast<double>(g_canvasWidth), 0.02, 0.98);
    g_settings.hudBtnRelY = std::clamp(static_cast<double>(cy) / static_cast<double>(g_canvasHeight), 0.02, 0.98);
}

void CommitEditedFieldValue(HudFieldId fieldId, const std::wstring& val) {
    std::wstring trimmed = Trim(val);
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        switch (fieldId) {
            case HudFieldId::Coords:
                g_settings.coordsRaw = trimmed;
                g_settings.hasValidCoords = TryParseCoordsString(trimmed, g_settings.latitude, g_settings.longitude);
                break;
            case HudFieldId::CloudsIntensity: {
                double v = 35.0;
                if (TryParseDoubleStrict(trimmed, v)) {
                    g_settings.cloudsIntensity = std::clamp(static_cast<int>(std::llround(v)), 0, 100);
                }
                break;
            }
            case HudFieldId::SunUvOverride: {
                double v = -1.0;
                if (trimmed.empty() || (TryParseDoubleStrict(trimmed, v) && std::abs(v + 1.0) < 1e-6)) {
                    g_settings.sunUvOverride = -1.0;
                } else if (TryParseDoubleStrict(trimmed, v) && v >= 0.0 && v <= 20.0) {
                    g_settings.sunUvOverride = v;
                }
                break;
            }
            case HudFieldId::MoonPhaseOverride: {
                double v = -1.0;
                if (trimmed.empty() || (TryParseDoubleStrict(trimmed, v) && std::abs(v + 1.0) < 1e-6)) {
                    g_settings.moonPhaseOverride = -1.0;
                } else if (TryParseDoubleStrict(trimmed, v) && v >= 0.0 && v <= 1.0) {
                    g_settings.moonPhaseOverride = v;
                }
                break;
            }
            case HudFieldId::PrecipSpeedMps: {
                double v = 6.5;
                if (TryParseDoubleStrict(trimmed, v) && v >= 0.1 && v <= 50.0) {
                    g_settings.precipSpeedMode = PrecipSpeedMode::Custom;
                    g_settings.precipFallSpeedMps = v;
                }
                break;
            }
            case HudFieldId::PrecipIntensity: {
                double v = 25.0;
                if (TryParseDoubleStrict(trimmed, v)) {
                    g_settings.precipIntensity = std::clamp(static_cast<int>(std::llround(v)), 0, 100);
                }
                break;
            }
            case HudFieldId::EffectsIntensity: {
                double v = 25.0;
                if (TryParseDoubleStrict(trimmed, v)) {
                    g_settings.effectsIntensity = std::clamp(static_cast<int>(std::llround(v)), 0, 100);
                }
                break;
            }
            default:
                break;
        }
        g_arcCacheDirty = true;
    }
    if (g_weatherWakeEvent) SetEvent(g_weatherWakeEvent);
}
void ToggleExclusiveSection(ExpandedSection sec) {
    std::lock_guard<std::mutex> lock(g_hudUiMutex);
    g_expandedSection = (g_expandedSection == sec) ? ExpandedSection::None : sec;
    g_hudScrollPx = 0;
}

void CycleSettingOption(const std::wstring& actionId) {
    if (actionId == L"manual_sync") {
        if (g_weatherWakeEvent) SetEvent(g_weatherWakeEvent);
        return;
    }
    if (actionId == L"sec_sky") { ToggleExclusiveSection(ExpandedSection::Sky); return; }
    if (actionId == L"sec_precip") { ToggleExclusiveSection(ExpandedSection::Precipitation); return; }
    if (actionId == L"sec_effects") { ToggleExclusiveSection(ExpandedSection::Effects); return; }

    auto nextTri = [](TriStateMode m) -> TriStateMode {
        if (m == TriStateMode::Auto) return TriStateMode::On;
        if (m == TriStateMode::On) return TriStateMode::Off;
        return TriStateMode::Auto;
    };

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        Settings& cur = g_settings;
        if (actionId == L"cycle_perf") {
            cur.performanceMode = (cur.performanceMode == PerformanceMode::Low) ? PerformanceMode::Medium :
                                  ((cur.performanceMode == PerformanceMode::Medium) ? PerformanceMode::High : PerformanceMode::Low);
        } else if (actionId == L"cycle_source") {
            cur.primarySource = (cur.primarySource == L"open_meteo") ? L"custom_api" :
                                ((cur.primarySource == L"custom_api") ? L"local_file" :
                                 ((cur.primarySource == L"local_file") ? L"off" : L"open_meteo"));
        } else if (actionId == L"cycle_clouds") cur.cloudsMode = nextTri(cur.cloudsMode);
        else if (actionId == L"cycle_sun") cur.sunMode = nextTri(cur.sunMode);
        else if (actionId == L"cycle_moon") cur.moonMode = nextTri(cur.moonMode);
        else if (actionId == L"toggle_celestial_front") cur.celestialInFrontOfClouds = !cur.celestialInFrontOfClouds;
        else if (actionId == L"cycle_precip_mode") {
            cur.precipitationMode = (cur.precipitationMode == PrecipMode::Auto) ? PrecipMode::Rain :
                                    ((cur.precipitationMode == PrecipMode::Rain) ? PrecipMode::Snow :
                                     ((cur.precipitationMode == PrecipMode::Snow) ? PrecipMode::Hail :
                                      ((cur.precipitationMode == PrecipMode::Hail) ? PrecipMode::Off : PrecipMode::Auto)));
        } else if (actionId == L"cycle_precip_dir") {
            cur.precipFallDirection = (cur.precipFallDirection == FallDirMode::Auto) ? FallDirMode::Down :
                                        ((cur.precipFallDirection == FallDirMode::Down) ? FallDirMode::DownLeft :
                                         ((cur.precipFallDirection == FallDirMode::DownLeft) ? FallDirMode::DownRight : FallDirMode::Auto));
        } else if (actionId == L"cycle_precip_speed") {
            cur.precipSpeedMode = (cur.precipSpeedMode == PrecipSpeedMode::Auto) ? PrecipSpeedMode::Custom : PrecipSpeedMode::Auto;
        } else if (actionId == L"cycle_tod") {
            cur.timeOfDayMode = (cur.timeOfDayMode == TimeOfDayMode::Auto) ? TimeOfDayMode::Dawn :
                                ((cur.timeOfDayMode == TimeOfDayMode::Dawn) ? TimeOfDayMode::Day :
                                 ((cur.timeOfDayMode == TimeOfDayMode::Day) ? TimeOfDayMode::Sunset :
                                  ((cur.timeOfDayMode == TimeOfDayMode::Sunset) ? TimeOfDayMode::Night :
                                   ((cur.timeOfDayMode == TimeOfDayMode::Night) ? TimeOfDayMode::Off : TimeOfDayMode::Auto))));
        } else if (actionId == L"cycle_fx_clear") cur.effectClear = nextTri(cur.effectClear);
        else if (actionId == L"cycle_fx_partly") cur.effectPartlyCloudy = nextTri(cur.effectPartlyCloudy);
        else if (actionId == L"cycle_fx_overcast") cur.effectOvercast = nextTri(cur.effectOvercast);
        else if (actionId == L"cycle_fx_dust") cur.effectDust = nextTri(cur.effectDust);
        else if (actionId == L"cycle_fx_fog") cur.effectFog = nextTri(cur.effectFog);
        else if (actionId == L"cycle_fx_smog") cur.effectSmog = nextTri(cur.effectSmog);
        else if (actionId == L"cycle_fx_storm") cur.effectThunderstorm = nextTri(cur.effectThunderstorm);
        else if (actionId == L"cycle_fx_aurora") cur.effectAurora = nextTri(cur.effectAurora);
        else if (actionId == L"cycle_fx_mirage") cur.effectMirage = nextTri(cur.effectMirage);
        else if (actionId == L"cycle_fx_rainbow") cur.effectRainbow = nextTri(cur.effectRainbow);
        else if (actionId == L"cycle_fx_wind") cur.effectWindStreams = nextTri(cur.effectWindStreams);
        else if (actionId == L"cycle_fx_thermal") cur.effectThermal = nextTri(cur.effectThermal);
        else if (actionId == L"cycle_fx_stars") cur.effectNightStars = nextTri(cur.effectNightStars);
        else if (actionId == L"cycle_fx_aq") cur.effectAirQuality = nextTri(cur.effectAirQuality);
        else if (actionId == L"cycle_fx_pollen") cur.effectPollen = nextTri(cur.effectPollen);
        else if (actionId == L"cycle_fx_flood") cur.effectFloodWater = nextTri(cur.effectFloodWater);
        g_arcCacheDirty = true;
    }
    if (g_weatherWakeEvent) SetEvent(g_weatherWakeEvent);
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
        Wh_Log(L"InternetCrackUrlW failed: %u", GetLastError());
        return {};
    }

    HINTERNET hSession = InternetOpenW(L"WindhawkWeatherMod/2.9", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hSession) {
        Wh_Log(L"InternetOpenW failed: %u", GetLastError());
        return {};
    }

    DWORD timeoutMs = 5000;
    InternetSetOptionW(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionW(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                  ((parts.nScheme == INTERNET_SCHEME_HTTPS) ? INTERNET_FLAG_SECURE : 0);

    HINTERNET hConnect = InternetConnectW(hSession, host, parts.nPort, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        Wh_Log(L"InternetConnectW failed: %u", GetLastError());
        InternetCloseHandle(hSession);
        return {};
    }

    HINTERNET hRequest = HttpOpenRequestW(hConnect, L"GET", path, nullptr, nullptr, nullptr, flags, 0);
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
            Wh_Log(L"HTTP status code: %u", statusCode);
        }
    } else {
        Wh_Log(L"HttpSendRequestW failed: %u", GetLastError());
    }

    if (hRequest) InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);
    return response;
}

double EstimatePrecipMmFromWmo(int code) {
    if (code == 51 || code == 56) return 0.3;
    if (code == 53) return 0.8;
    if (code == 55 || code == 57) return 1.8;
    if (code == 61 || code == 80) return 1.5;
    if (code == 63 || code == 81) return 4.0;
    if (code == 65 || code == 82) return 8.5;
    if (code == 66) return 1.5;
    if (code == 67) return 5.0;
    if (code == 71 || code == 85) return 0.8;
    if (code == 73 || code == 77) return 2.2;
    if (code == 75 || code == 86) return 5.5;
    if (code == 95) return 6.0;
    if (code == 96 || code == 99) return 9.0;
    return 0.0;
}

void ApplyForecastMetrics(int code, double windKmh, double windGustsKmh, double tempC, double apparentTempC,
                          double humidityPct, int cloudCover, double visibilityM, double precipMm,
                          double aqiVal, double pm25Val, double pollenMax, bool isDay,
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
    else if (isFogCode || (humidityPct >= 92.0 && visibilityM > 0.0 && visibilityM < 3500.0)) fx.fog = true;

    if (isSnowCode) { hasPrecip = true; precipKind = ObjectKind::Snow; }
    else if (code == 96 || code == 99) { hasPrecip = true; precipKind = ObjectKind::Hail; fx.thunderstorm = true; }
    else if (code == 95) { hasPrecip = true; precipKind = ObjectKind::Rain; fx.thunderstorm = true; }
    else if (isRainCode) { hasPrecip = true; precipKind = ObjectKind::Rain; }

    if (!hasPrecip && !isStormCode && windKmh >= 28.0 && (visibilityM <= 0.0 || visibilityM < 8000.0)) fx.dust = true;
    if (!hasPrecip && windKmh < 12.0 && visibilityM > 0.0 && visibilityM < 6000.0) fx.smog = true;
    if (isDay && !hasPrecip && !isStormCode && (tempC >= 28.0 || apparentTempC >= 30.0) && cloudCover <= 35) fx.mirage = true;
    if (isDay && !isStormCode && (isRainCode || isFogCode || (cloudCover >= 15 && cloudCover <= 65 && !hasPrecip))) fx.rainbow = true;
    if (!isDay && !hasPrecip && !isStormCode && cloudCover <= 35) { fx.aurora = true; fx.nightStars = true; }
    else if (!isDay && !hasPrecip && cloudCover <= 25) fx.nightStars = true;
    if (windGustsKmh >= 32.0 || windKmh >= 24.0) fx.windStreams = true;
    if (tempC <= 0.0) fx.frost = true;
    else if (isDay && apparentTempC >= 30.0) fx.heat = true;
    if (aqiVal >= 50.0 || pm25Val >= 15.0) fx.airQuality = true;
    if (pollenMax >= 25.0) fx.pollen = true;
    if (precipMm >= 2.5 || (isRainCode && precipMm >= 1.2)) fx.floodWater = true;
}

bool ExtractNumberScoped(const std::string& json, size_t scope, const std::string& key, double& val) {
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
    if (errno != 0 || endPtr == json.c_str() + pos || !std::isfinite(parsed)) return false;
    val = parsed;
    return true;
}

void ParseAirQualityJson(const std::string& json, ForecastSnapshot& outSnap) {
    if (json.empty()) return;
    size_t scope = json.find("\"current\":");
    if (scope == std::string::npos) scope = 0;
    double v = 0.0;
    if (ExtractNumberScoped(json, scope, "european_aqi", v) || ExtractNumberScoped(json, scope, "us_aqi", v)) {
        outSnap.aqiValue = std::max(0.0, v);
    }
    if (ExtractNumberScoped(json, scope, "pm2_5", v)) {
        outSnap.pm25Value = std::max(0.0, v);
    }
    double maxPollen = 0.0;
    const char* pollenKeys[] = {"alder_pollen", "birch_pollen", "grass_pollen", "mugwort_pollen", "olive_pollen", "ragweed_pollen"};
    for (const char* pk : pollenKeys) {
        double pv = 0.0;
        if (ExtractNumberScoped(json, scope, pk, pv) && pv > maxPollen) maxPollen = pv;
    }
    outSnap.pollenMaxValue = maxPollen;
}

bool ParseWeatherJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t scope = json.find("\"current\":");
    if (scope == std::string::npos) scope = 0;
    double spd = 0.0;
    if (ExtractNumberScoped(json, scope, "wind_speed_10m", spd) ||
        ExtractNumberScoped(json, scope, "windspeed", spd) ||
        ExtractNumberScoped(json, scope, "wind_speed", spd)) {
        outSnap.windSpeedKmh = std::max(0.0, spd);
    }
    double gusts = 0.0;
    if (ExtractNumberScoped(json, scope, "wind_gusts_10m", gusts) || ExtractNumberScoped(json, scope, "wind_gusts", gusts)) {
        outSnap.windGustsKmh = std::max(0.0, gusts);
    } else {
        outSnap.windGustsKmh = outSnap.windSpeedKmh;
    }
    double deg = -1.0;
    if (ExtractNumberScoped(json, scope, "wind_direction_10m", deg) ||
        ExtractNumberScoped(json, scope, "winddirection", deg) ||
        ExtractNumberScoped(json, scope, "wind_deg", deg)) {
        outSnap.windDirDeg = deg;
        outSnap.windDirSign = (outSnap.windSpeedKmh > 0.2) ? WindDegToSign(deg) : 0;
    } else {
        outSnap.windDirDeg = 0.0;
        outSnap.windDirSign = 0;
    }
    double tempC = 15.0;
    if (ExtractNumberScoped(json, scope, "temperature_2m", tempC) || ExtractNumberScoped(json, scope, "temperature", tempC)) {
        outSnap.tempC = tempC;
    }
    double appTemp = outSnap.tempC;
    outSnap.apparentTempC = ExtractNumberScoped(json, scope, "apparent_temperature", appTemp) ? appTemp : outSnap.tempC;
    double hum = 55.0;
    if (ExtractNumberScoped(json, scope, "relative_humidity_2m", hum)) outSnap.humidityPct = std::clamp(hum, 0.0, 100.0);
    double cloud = 20.0;
    ExtractNumberScoped(json, scope, "cloud_cover", cloud);
    outSnap.cloudCoverPct = std::clamp(cloud, 0.0, 100.0);
    double vis = 12000.0;
    ExtractNumberScoped(json, scope, "visibility", vis);
    outSnap.visibilityM = std::max(0.0, vis);
    double isDayNum = 1.0;
    ExtractNumberScoped(json, scope, "is_day", isDayNum);
    outSnap.isDay = (isDayNum > 0.5);
    double uv = 2.0;
    if (ExtractNumberScoped(json, scope, "uv_index", uv) || ExtractNumberScoped(json, scope, "uv", uv)) {
        outSnap.uvIndex = std::max(0.0, uv);
    }
    double sr = 6.5, ss = 18.0;
    if (TryExtractIsoTimeHour(json, "sunrise", sr) && TryExtractIsoTimeHour(json, "sunset", ss)) {
        outSnap.hasSunTimes = true;
        outSnap.sunriseHour = sr;
        outSnap.sunsetHour = ss;
    }
    double aqiNum = 0.0;
    if (ExtractNumberScoped(json, scope, "european_aqi", aqiNum) || ExtractNumberScoped(json, scope, "us_aqi", aqiNum)) {
        outSnap.aqiValue = std::max(0.0, aqiNum);
    }
    double pmNum = 0.0;
    if (ExtractNumberScoped(json, scope, "pm2_5", pmNum)) {
        outSnap.pm25Value = std::max(0.0, pmNum);
    }
    double wmo = -999.0;
    if (ExtractNumberScoped(json, scope, "weather_code", wmo) || ExtractNumberScoped(json, scope, "weathercode", wmo)) {
        outSnap.weatherCode = static_cast<int>(wmo);
        double mm = 0.0;
        outSnap.precipMm = (ExtractNumberScoped(json, scope, "precipitation", mm) || ExtractNumberScoped(json, scope, "rain", mm))
            ? std::max(0.0, mm)
            : EstimatePrecipMmFromWmo(outSnap.weatherCode);
        ApplyForecastMetrics(outSnap.weatherCode, outSnap.windSpeedKmh, outSnap.windGustsKmh, outSnap.tempC,
                             outSnap.apparentTempC, outSnap.humidityPct, static_cast<int>(outSnap.cloudCoverPct),
                             outSnap.visibilityM, outSnap.precipMm, outSnap.aqiValue, outSnap.pm25Value,
                             outSnap.pollenMaxValue, outSnap.isDay, outSnap.hasPrecip, outSnap.precipKind, outSnap.effects);
        outSnap.valid = true;
        SYSTEMTIME st = {};
        GetLocalTime(&st);
        wchar_t timeBuf[32] = {};
        swprintf_s(timeBuf, L"%02u:%02u", st.wHour, st.wMinute);
        outSnap.lastUpdateText = timeBuf;
        return true;
    }
    Wh_Log(L"Failed to parse weather_code from JSON");
    return false;
}

bool FetchWeatherSource(const std::wstring& src, const Settings& cfg, ForecastSnapshot& outSnap) {
    if (src == L"off") return false;
    if (src == L"open_meteo") {
        if (!cfg.hasValidCoords) {
            Wh_Log(L"Open-Meteo skipped: coords empty or invalid ('%s')", cfg.coordsRaw.c_str());
            return false;
        }
        std::wstringstream url;
        url << L"https://api.open-meteo.com/v1/forecast?latitude=" << cfg.latitude
            << L"&longitude=" << cfg.longitude
            << L"¤t=weather_code,wind_speed_10m,wind_gusts_10m,wind_direction_10m,temperature_2m,apparent_temperature,relative_humidity_2m,cloud_cover,visibility,is_day,uv_index,precipitation"
            << L"&daily=sunrise,sunset&timezone=auto";
        std::string body = HttpGetUtf8(url.str());
        if (body.empty() || !ParseWeatherJson(body, outSnap)) return false;
        if (NeedsAirQualityFetch(cfg)) {
            std::wstringstream aqUrl;
            aqUrl << L"https://air-quality-api.open-meteo.com/v1/air-quality?latitude=" << cfg.latitude
                  << L"&longitude=" << cfg.longitude
                  << L"¤t=european_aqi,pm2_5,alder_pollen,birch_pollen,grass_pollen,ragweed_pollen&timezone=auto";
            std::string aqBody = HttpGetUtf8(aqUrl.str());
            if (!aqBody.empty()) {
                ParseAirQualityJson(aqBody, outSnap);
                ApplyForecastMetrics(outSnap.weatherCode, outSnap.windSpeedKmh, outSnap.windGustsKmh, outSnap.tempC,
                                     outSnap.apparentTempC, outSnap.humidityPct, static_cast<int>(outSnap.cloudCoverPct),
                                     outSnap.visibilityM, outSnap.precipMm, outSnap.aqiValue, outSnap.pm25Value,
                                     outSnap.pollenMaxValue, outSnap.isDay, outSnap.hasPrecip, outSnap.precipKind, outSnap.effects);
            }
        }
        outSnap.sourceLabel = L"Open-Meteo";
        return true;
    }
    if (src == L"custom_api") {
        if (cfg.customApiUrl.empty()) return false;
        std::string body = HttpGetUtf8(cfg.customApiUrl);
        if (body.empty() || !ParseWeatherJson(body, outSnap)) return false;
        outSnap.sourceLabel = L"Custom API";
        return true;
    }
    if (src == L"local_file") {
        if (cfg.localWeatherPath.empty()) return false;
        HANDLE hFile = CreateFileW(cfg.localWeatherPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                     nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) return false;
        DWORD size = GetFileSize(hFile, nullptr);
        std::string content;
        if (size > 0 && size < 1024 * 1024) {
            content.resize(size);
            DWORD read = 0;
            ReadFile(hFile, content.data(), size, &read, nullptr);
            content.resize(read);
        }
        CloseHandle(hFile);
        if (content.empty() || !ParseWeatherJson(content, outSnap)) return false;
        outSnap.sourceLabel = L"Local File";
        return true;
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
        DWORD waitMs = static_cast<DWORD>(intervalSec) * 1000;
        if ((cfg.primarySource != L"off") && HasAnyForecastConsumer(cfg)) {
            ForecastSnapshot snap;
            bool ok = FetchWeatherSource(cfg.primarySource, cfg, snap);
            if (!ok && cfg.fallbackSource != L"off" && cfg.fallbackSource != cfg.primarySource) {
                ok = FetchWeatherSource(cfg.fallbackSource, cfg, snap);
            }
            if (ok) {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_forecast = snap;
            } else {
                waitMs = std::min<DWORD>(waitMs, 60000);
            }
        }
        if (g_weatherWakeEvent) WaitForSingleObject(g_weatherWakeEvent, waitMs);
        else std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
    }
}

int ComputeBottomLimitYForRect(const Settings& cfg, int monTop, int monBottom, int monWorkBottom) {
    int baseBottom = monBottom;
    if (cfg.bottomBoundaryMode == L"above_taskbar") {
        if (cfg.taskbarAsBorder) {
            baseBottom = std::min(baseBottom, monWorkBottom);
        }
        HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
        RECT trayRect = {};
        if (hTray && GetWindowRect(hTray, &trayRect) && cfg.taskbarAsBorder) {
            baseBottom = std::min(baseBottom, static_cast<int>(trayRect.top));
        }
    } else if (cfg.bottomBoundaryMode == L"custom_offset") {
        baseBottom -= cfg.bottomOffsetPx;
    }
    baseBottom -= cfg.customOffset.bottom;
    return std::clamp(baseBottom, monTop + 10, monBottom);
}

BOOL CALLBACK CollectMonitorRectsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* rects = reinterpret_cast<std::vector<MONITORINFO>*>(lParam);
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(hMon, &mi)) {
        rects->push_back(mi);
    }
    return TRUE;
}

void UpdateCanvasGeometry(const Settings& cfg) {
    g_monitorRects.clear();
    if (cfg.multiMonitorMode == L"all") {
        g_canvasLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
        g_canvasTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
        g_canvasWidth = std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN));
        g_canvasHeight = std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN));

        std::vector<MONITORINFO> rawMons;
        EnumDisplayMonitors(nullptr, nullptr, CollectMonitorRectsProc, reinterpret_cast<LPARAM>(&rawMons));
        for (const auto& mi : rawMons) {
            MonitorCanvasRect m{
                static_cast<float>(mi.rcMonitor.left - g_canvasLeft),
                static_cast<float>(mi.rcMonitor.top - g_canvasTop),
                static_cast<float>(std::max(1L, mi.rcMonitor.right - mi.rcMonitor.left)),
                static_cast<float>(std::max(1L, mi.rcMonitor.bottom - mi.rcMonitor.top)),
                0.0f
            };
            m.bottomLimitRelY = static_cast<float>(
                ComputeBottomLimitYForRect(cfg, static_cast<int>(mi.rcMonitor.top),
                                           static_cast<int>(mi.rcMonitor.bottom),
                                           static_cast<int>(mi.rcWork.bottom)) - g_canvasTop);
            g_monitorRects.push_back(m);
        }
    } else {
        g_canvasLeft = 0;
        g_canvasTop = 0;
        g_canvasWidth = std::max(1, GetSystemMetrics(SM_CXSCREEN));
        g_canvasHeight = std::max(1, GetSystemMetrics(SM_CYSCREEN));

        HMONITOR primMon = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfoW(primMon, &mi)) {
            float botRel = static_cast<float>(
                ComputeBottomLimitYForRect(cfg, static_cast<int>(mi.rcMonitor.top),
                                           static_cast<int>(mi.rcMonitor.bottom),
                                           static_cast<int>(mi.rcWork.bottom)) - g_canvasTop);
            g_monitorRects.push_back(MonitorCanvasRect{
                0.0f, 0.0f, static_cast<float>(g_canvasWidth), static_cast<float>(g_canvasHeight), botRel
            });
        }
    }

    if (g_monitorRects.empty()) {
        g_monitorRects.push_back(MonitorCanvasRect{
            0.0f, 0.0f, static_cast<float>(g_canvasWidth), static_cast<float>(g_canvasHeight),
            static_cast<float>(g_canvasHeight - cfg.customOffset.bottom)
        });
    }
}

int ComputeBottomLimitY(const Settings& cfg) {
    if (!g_monitorRects.empty()) {
        return static_cast<int>(g_canvasTop + g_monitorRects.front().bottomLimitRelY);
    }
    return std::clamp(g_canvasTop + g_canvasHeight - cfg.customOffset.bottom, g_canvasTop + 10, g_canvasTop + g_canvasHeight);
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

    return (fgRect.left <= mi.rcMonitor.left && fgRect.top <= mi.rcMonitor.top &&
            fgRect.right >= mi.rcMonitor.right && fgRect.bottom >= mi.rcMonitor.bottom);
}

bool IsWindowCloaked(HWND hwnd) {
    constexpr DWORD kDwmwaCloaked = 14;
    using DwmGetWindowAttributeFn = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);
    static HMODULE hDwmapi = GetModuleHandleW(L"dwmapi.dll") ? GetModuleHandleW(L"dwmapi.dll") : LoadLibraryW(L"dwmapi.dll");
    if (!hDwmapi) return false;
    static auto pfn = reinterpret_cast<DwmGetWindowAttributeFn>(GetProcAddress(hDwmapi, "DwmGetWindowAttribute"));
    DWORD cloaked = 0;
    return pfn && SUCCEEDED(pfn(hwnd, kDwmwaCloaked, &cloaked, sizeof(cloaked))) && cloaked != 0;
}

bool IsDesktopOccluded() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defView = nullptr;
    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr)) != nullptr) {
        if ((defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr)) != nullptr) break;
    }
    if (!defView && progman) defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);

    HWND topWnd = GetTopWindow(nullptr);
    while (topWnd) {
        if (topWnd != g_overlayHwnd && topWnd != progman && topWnd != defView &&
            IsWindowVisible(topWnd) && !IsWindowCloaked(topWnd)) {
            LONG exStyle = GetWindowLongW(topWnd, GWL_EXSTYLE);
            bool isClickThrough = ((exStyle & WS_EX_TRANSPARENT) != 0) && ((exStyle & WS_EX_LAYERED) != 0);
            if (!(exStyle & WS_EX_TOOLWINDOW) && !isClickThrough) {
                wchar_t cls[128] = {};
                GetClassNameW(topWnd, cls, _countof(cls));
                std::wstring clsStr = cls;
                if (clsStr != L"WorkerW" && clsStr != L"Shell_TrayWnd" && clsStr != L"Shell_SecondaryTrayWnd") {
                    RECT r = {};
                    if (GetWindowRect(topWnd, &r) && r.left <= g_canvasLeft && r.top <= g_canvasTop &&
                        r.right >= g_canvasLeft + g_canvasWidth && r.bottom >= g_canvasTop + g_canvasHeight) {
                        return true;
                    }
                }
            }
        }
        topWnd = GetNextWindow(topWnd, GW_HWNDNEXT);
    }
    return false;
}

HWND FindDesktopIconsWindow() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defView = nullptr;
    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr)) != nullptr) {
        if ((defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr)) != nullptr) break;
    }
    if (!defView && progman) defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    return defView;
}

HWND FindDesktopInsertAfter(const std::wstring& mode, HWND overlay) {
    if (mode == L"desktop_bottom") return HWND_BOTTOM;
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (mode == L"progman_direct") return progman ? progman : HWND_BOTTOM;

    HWND defView = FindDesktopIconsWindow();
    if (defView) {
        HWND parent = GetParent(defView);
        if (parent && parent != progman) {
            HWND afterParent = GetWindow(parent, GW_HWNDNEXT);
            if (afterParent && afterParent != overlay) return afterParent;
            return HWND_BOTTOM;
        }
        HWND afterDef = GetWindow(defView, GW_HWNDNEXT);
        if (afterDef && afterDef != overlay) return afterDef;
    }
    return HWND_BOTTOM;
}

void PositionDesktopOverlay(HWND overlay, const std::wstring& mode, bool forceShow) {
    if (!overlay) return;
    HWND insertAfter = FindDesktopInsertAfter(mode, overlay);
    SetWindowPos(overlay, insertAfter, g_canvasLeft, g_canvasTop, g_canvasWidth, g_canvasHeight,
                 SWP_NOACTIVATE | (forceShow ? SWP_SHOWWINDOW : 0));
    HWND defView = FindDesktopIconsWindow();
    if (defView && mode == L"auto") {
        SetWindowPos(defView, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

bool IsRelPointInsideHud(float relX, float relY) {
    float r = g_hudBtnR.load();
    if (r <= 2.0f) return false;
    float dx = relX - g_hudBtnCx.load();
    float dy = relY - g_hudBtnCy.load();
    if ((dx * dx + dy * dy) <= (r * r)) return true;

    if (g_hudExpanded.load()) {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        for (const auto& ctrl : g_hudControls) {
            if (relX >= ctrl.rect.left && relX <= ctrl.rect.right &&
                relY >= ctrl.rect.top && relY <= ctrl.rect.bottom) {
                return true;
            }
        }
    }
    return false;
}

bool IsPointInsideAnyHudRegion(LPARAM lParam) {
    float relX = static_cast<float>(static_cast<short>(LOWORD(lParam)) - g_canvasLeft);
    float relY = static_cast<float>(static_cast<short>(HIWORD(lParam)) - g_canvasTop);
    return IsRelPointInsideHud(relX, relY);
}

void SyncClickThroughExStyle(HWND hwnd, bool allowInteractive) {
    if (!hwnd || !IsWindow(hwnd)) return;
    LONG curEx = GetWindowLongW(hwnd, GWL_EXSTYLE);
    bool isTransparent = (curEx & WS_EX_TRANSPARENT) != 0;
    if (allowInteractive && isTransparent) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, curEx & ~WS_EX_TRANSPARENT);
    } else if (!allowInteractive && !isTransparent) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, curEx | WS_EX_TRANSPARENT);
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_NCHITTEST) {
        bool hitHud = IsPointInsideAnyHudRegion(lParam);
        SyncClickThroughExStyle(hwnd, hitHud);
        return hitHud ? HTCLIENT : HTTRANSPARENT;
    }
    if (msg == WM_LBUTTONDOWN) {
        int relX = static_cast<short>(LOWORD(lParam));
        int relY = static_cast<short>(HIWORD(lParam));
        float btnCx = g_hudBtnCx.load(), btnCy = g_hudBtnCy.load(), btnR = g_hudBtnR.load();
        float dx = static_cast<float>(relX) - btnCx, dy = static_cast<float>(relY) - btnCy;
        if (btnR > 2.0f && (dx * dx + dy * dy) <= (btnR * btnR)) {
            std::lock_guard<std::mutex> lock(g_hudUiMutex);
            g_draggingHudBtn = true;
            g_movedDuringDrag = false;
            g_dragStartScreenX = g_canvasLeft + relX;
            g_dragStartScreenY = g_canvasTop + relY;
            g_dragStartBtnCx = btnCx;
            g_dragStartBtnCy = btnCy;
            SetCapture(hwnd);
            return 0;
        }
        if (g_hudExpanded.load()) {
            std::wstring actionToRun;
            HudFieldId fieldToFocus = HudFieldId::None;
            std::wstring initialFieldText;
            {
                std::lock_guard<std::mutex> lock(g_hudUiMutex);
                for (const auto& ctrl : g_hudControls) {
                    if (relX >= ctrl.rect.left && relX <= ctrl.rect.right &&
                        relY >= ctrl.rect.top && relY <= ctrl.rect.bottom) {
                        if (!ctrl.actionId.empty()) actionToRun = ctrl.actionId;
                        if (ctrl.fieldId != HudFieldId::None) {
                            fieldToFocus = ctrl.fieldId;
                            Settings cur;
                            { std::lock_guard<std::mutex> slock(g_mutex); cur = g_settings; }
                            wchar_t numBuf[64] = {};
                            if (fieldToFocus == HudFieldId::Coords) initialFieldText = cur.coordsRaw;
                            else if (fieldToFocus == HudFieldId::CloudsIntensity) { swprintf_s(numBuf, L"%d", cur.cloudsIntensity); initialFieldText = numBuf; }
                            else if (fieldToFocus == HudFieldId::SunUvOverride) { swprintf_s(numBuf, L"%.1f", cur.sunUvOverride); initialFieldText = numBuf; }
                            else if (fieldToFocus == HudFieldId::MoonPhaseOverride) { swprintf_s(numBuf, L"%.2f", cur.moonPhaseOverride); initialFieldText = numBuf; }
                            else if (fieldToFocus == HudFieldId::PrecipSpeedMps) { swprintf_s(numBuf, L"%.1f", cur.precipFallSpeedMps); initialFieldText = numBuf; }
                            else if (fieldToFocus == HudFieldId::PrecipIntensity) { swprintf_s(numBuf, L"%d", cur.precipIntensity); initialFieldText = numBuf; }
                            else if (fieldToFocus == HudFieldId::EffectsIntensity) { swprintf_s(numBuf, L"%d", cur.effectsIntensity); initialFieldText = numBuf; }
                        }
                        break;
                    }
                }
                if (fieldToFocus != HudFieldId::None) {
                    g_focusedField = fieldToFocus;
                    g_editBuffer = initialFieldText;
                } else if (actionToRun.empty()) {
                    if (!g_editBuffer.empty() && g_focusedField != HudFieldId::None) {
                        CommitEditedFieldValue(g_focusedField, g_editBuffer);
                    }
                    g_focusedField = HudFieldId::None;
                    g_editBuffer.clear();
                }
            }
            if (!actionToRun.empty()) CycleSettingOption(actionToRun);
            return 0;
        }
    } else if (msg == WM_MOUSEMOVE) {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        if (g_draggingHudBtn) {
            int deltaX = (g_canvasLeft + static_cast<short>(LOWORD(lParam))) - g_dragStartScreenX;
            int deltaY = (g_canvasTop + static_cast<short>(HIWORD(lParam))) - g_dragStartScreenY;
            if (std::abs(deltaX) > 5 || std::abs(deltaY) > 5) g_movedDuringDrag = true;
            float r = g_hudBtnR.load();
            g_hudBtnCx.store(std::clamp(g_dragStartBtnCx + static_cast<float>(deltaX), r + 6.0f, static_cast<float>(g_canvasWidth) - r - 6.0f));
            g_hudBtnCy.store(std::clamp(g_dragStartBtnCy + static_cast<float>(deltaY), r + 6.0f, static_cast<float>(g_canvasHeight) - r - 6.0f));
            return 0;
        }
    } else if (msg == WM_LBUTTONUP) {
        bool wasDragging = false, didMove = false;
        float finalCx = 0.0f, finalCy = 0.0f;
        {
            std::lock_guard<std::mutex> lock(g_hudUiMutex);
            wasDragging = g_draggingHudBtn;
            didMove = g_movedDuringDrag;
            g_draggingHudBtn = false;
            g_movedDuringDrag = false;
            finalCx = g_hudBtnCx.load();
            finalCy = g_hudBtnCy.load();
        }
        if (wasDragging) {
            ReleaseCapture();
            if (didMove) {
                ApplyRuntimeButtonPosition(finalCx, finalCy);
            } else {
                bool nextExp = !g_hudExpanded.load();
                g_hudExpanded.store(nextExp);
                if (!nextExp) {
                    std::lock_guard<std::mutex> lock(g_hudUiMutex);
                    g_hudScrollPx = 0;
                    g_expandedSection = ExpandedSection::None;
                    g_focusedField = HudFieldId::None;
                    g_editBuffer.clear();
                }
            }
            return 0;
        }
    } else if (msg == WM_MOUSEWHEEL && g_hudExpanded.load()) {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudScrollPx = (g_hudMaxScrollPx > 0)
            ? std::clamp(g_hudScrollPx + ((delta > 0) ? -48 : 48), 0, g_hudMaxScrollPx)
            : 0;
        return 0;
    } else if (msg == WM_KEYDOWN) {
        HudFieldId activeField = HudFieldId::None;
        std::wstring bufCopy;
        bool shouldCommit = false;
        {
            std::lock_guard<std::mutex> lock(g_hudUiMutex);
            activeField = g_focusedField;
            if (activeField != HudFieldId::None) {
                if (wParam == VK_RETURN) {
                    bufCopy = g_editBuffer;
                    shouldCommit = true;
                    g_focusedField = HudFieldId::None;
                    g_editBuffer.clear();
                } else if (wParam == VK_ESCAPE) {
                    g_focusedField = HudFieldId::None;
                    g_editBuffer.clear();
                } else if (wParam == VK_BACK && !g_editBuffer.empty()) {
                    g_editBuffer.pop_back();
                }
                return 0;
            }
        }
        if (shouldCommit) CommitEditedFieldValue(activeField, bufCopy);
    } else if (msg == WM_CHAR) {
        wchar_t ch = static_cast<wchar_t>(wParam);
        if (ch >= 32 && ch != 127) {
            std::lock_guard<std::mutex> lock(g_hudUiMutex);
            if (g_focusedField != HudFieldId::None && g_editBuffer.size() < 120) {
                g_editBuffer.push_back(ch);
                return 0;
            }
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void SpawnFallingItem(FallingItem& item, std::mt19937& rng, ObjectKind kind, float vx, bool randomY, PerformanceMode perf) {
    std::uniform_real_distribution<float> phaseDist(0.0f, 6.2831853f), alphaDist(0.65f, 0.96f);
    item.kind = kind;
    item.swayPhase = phaseDist(rng);
    item.angle = phaseDist(rng);
    item.alpha = alphaDist(rng);
    item.depthTier = (perf == PerformanceMode::Low) ? 1 : static_cast<int>(rng() % 3);

    if (randomY) {
        item.x = std::uniform_real_distribution<float>(-40.0f, static_cast<float>(g_canvasWidth) + 40.0f)(rng);
        item.y = std::uniform_real_distribution<float>(0.0f, static_cast<float>(g_canvasHeight))(rng);
    } else if (vx < -25.0f && ((rng() % 100) < 35)) {
        item.x = static_cast<float>(g_canvasWidth) + 20.0f;
        item.y = std::uniform_real_distribution<float>(0.0f, static_cast<float>(g_canvasHeight) * 0.75f)(rng);
    } else if (vx > 25.0f && ((rng() % 100) < 35)) {
        item.x = -20.0f;
        item.y = std::uniform_real_distribution<float>(0.0f, static_cast<float>(g_canvasHeight) * 0.75f)(rng);
    } else {
        item.x = std::uniform_real_distribution<float>(-60.0f, static_cast<float>(g_canvasWidth) + 60.0f)(rng);
        item.y = -24.0f;
    }

    float depthMul = (perf == PerformanceMode::Low) ? 1.0f : ((item.depthTier == 0) ? 0.75f : ((item.depthTier == 2) ? 1.25f : 1.0f));
    if (kind == ObjectKind::Rain) {
        item.size = 13.0f * depthMul;
        item.spin = 0.0f;
    } else if (kind == ObjectKind::Hail) {
        item.size = (5.0f + (rng() % 30) * 0.1f) * depthMul;
        item.spin = ((rng() % 100) - 50) * 0.05f * ((perf == PerformanceMode::High) ? 1.35f : 1.0f);
    } else {
        item.size = (2.4f + (static_cast<float>(rng() % 1000) / 1000.0f) * 5.2f) * depthMul;
        item.spin = 0.0f;
    }
}

void CleanupSurface(CachedSurface& surf) {
    if (surf.flakeSprite) { surf.flakeSprite->Release(); surf.flakeSprite = nullptr; }
    if (surf.sharedBrush) { surf.sharedBrush->Release(); surf.sharedBrush = nullptr; }
    if (surf.rt) { surf.rt->Release(); surf.rt = nullptr; }
    if (surf.memDC) {
        if (surf.oldBmp) { SelectObject(surf.memDC, surf.oldBmp); surf.oldBmp = nullptr; }
        DeleteDC(surf.memDC);
        surf.memDC = nullptr;
    }
    if (surf.dib) { DeleteObject(surf.dib); surf.dib = nullptr; }
    surf.width = 0;
    surf.height = 0;
}

bool RebuildFlakeSprite(CachedSurface& surf, D2D1_COLOR_F snowCol, bool hasCustomColor) {
    if (!surf.rt) return false;
    if (surf.flakeSprite) { surf.flakeSprite->Release(); surf.flakeSprite = nullptr; }
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
        {0.00f, shade(1.00f, 1.00f, 1.00f, 1.00f)}, {0.42f, shade(0.98f, 0.99f, 1.00f, 0.95f)},
        {0.62f, shade(0.75f, 0.82f, 0.95f, 0.36f)}, {1.00f, shade(0.60f, 0.70f, 0.85f, 0.00f)}
    };
    ID2D1GradientStopCollection* stopCol = nullptr;
    ID2D1RadialGradientBrush* radBrush = nullptr;
    if (SUCCEEDED(bmpRt->CreateGradientStopCollection(stops, 4, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCol)) && stopCol) {
        bmpRt->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(D2D1::Point2F(16.0f, 16.0f), D2D1::Point2F(0.0f, 0.0f), 16.0f, 16.0f),
            stopCol, &radBrush);
    }
    bmpRt->BeginDraw();
    bmpRt->Clear(D2D1::ColorF(0, 0, 0, 0));
    if (radBrush) bmpRt->FillRectangle(D2D1::RectF(0.0f, 0.0f, 32.0f, 32.0f), radBrush);
    bmpRt->EndDraw();
    if (radBrush) radBrush->Release();
    if (stopCol) stopCol->Release();
    bmpRt->GetBitmap(&surf.flakeSprite);
    bmpRt->Release();
    surf.spriteColor = snowCol;
    return surf.flakeSprite != nullptr;
}

bool EnsureDirectWrite(float fontSizePx, float circleFontPx) {
    if (!g_dwriteFactory) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_dwriteFactory));
    }
    if (!g_dwriteFactory) return false;
    if (std::abs(g_cachedFontSizePx - fontSizePx) > 0.1f) {
        if (g_hudPillFormat) { g_hudPillFormat->Release(); g_hudPillFormat = nullptr; }
        if (g_hudTitleFormat) { g_hudTitleFormat->Release(); g_hudTitleFormat = nullptr; }
        if (g_hudBodyFormat) { g_hudBodyFormat->Release(); g_hudBodyFormat = nullptr; }
        g_cachedFontSizePx = fontSizePx;
    }
    if (std::abs(g_cachedCircleFontPx - circleFontPx) > 0.1f) {
        if (g_hudCircleFormat) { g_hudCircleFormat->Release(); g_hudCircleFormat = nullptr; }
        g_cachedCircleFontPx = circleFontPx;
    }
    if (!g_hudTitleFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSizePx + 0.5f, L"en-US", &g_hudTitleFormat);
    }
    if (!g_hudBodyFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSizePx, L"en-US", &g_hudBodyFormat);
    }
    if (!g_hudCircleFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, std::max(8.0f, circleFontPx), L"en-US", &g_hudCircleFormat);
        if (g_hudCircleFormat) {
            g_hudCircleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_hudCircleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_hudCircleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
    }
    if (!g_hudPillFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, std::max(8.5f, fontSizePx - 1.5f), L"en-US", &g_hudPillFormat);
        if (g_hudPillFormat) {
            g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_hudPillFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_hudPillFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
    }
    return g_hudTitleFormat && g_hudBodyFormat && g_hudCircleFormat && g_hudPillFormat;
}

void CleanupDirectWrite() {
    if (g_hudPillFormat) { g_hudPillFormat->Release(); g_hudPillFormat = nullptr; }
    if (g_hudCircleFormat) { g_hudCircleFormat->Release(); g_hudCircleFormat = nullptr; }
    if (g_hudTitleFormat) { g_hudTitleFormat->Release(); g_hudTitleFormat = nullptr; }
    if (g_hudBodyFormat) { g_hudBodyFormat->Release(); g_hudBodyFormat = nullptr; }
    if (g_dwriteFactory) { g_dwriteFactory->Release(); g_dwriteFactory = nullptr; }
    g_cachedFontSizePx = 0.0f;
    g_cachedCircleFontPx = 0.0f;
}

bool EnsureSurface(CachedSurface& surf, int width, int height, D2D1_COLOR_F snowCol, bool hasCustomSnow) {
    if (!g_d2dFactory) D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory);
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
        if (!surf.dib) { CleanupSurface(surf); return false; }
        surf.oldBmp = SelectObject(surf.memDC, surf.dib);
        surf.width = width;
        surf.height = height;
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        if (FAILED(g_d2dFactory->CreateDCRenderTarget(&props, &surf.rt)) || !surf.rt) { CleanupSurface(surf); return false; }
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

void DrawRainSplashesHigh(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                          const Settings& cfg, const FallingItem& p, float bottomRelY, double timeSec) {
    if (cfg.performanceMode != PerformanceMode::High || p.y < bottomRelY - 14.0f || p.y > bottomRelY + 4.0f) return;
    float splashFade = 1.0f - std::clamp(std::abs(p.y - bottomRelY) / 14.0f, 0.0f, 1.0f);
    float phase = static_cast<float>(std::sin(timeSec * 18.0 + p.x * 0.05));
    float a = std::clamp(p.alpha * 0.65f * splashFade, 0.04f, 0.70f);
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(cfg.colorRain.r, cfg.colorRain.g, cfg.colorRain.b, a));
    float spread = 5.0f + std::abs(phase) * 5.0f;
    float lift = 4.0f + std::abs(phase) * 4.0f;
    rt->DrawLine(D2D1::Point2F(p.x, bottomRelY), D2D1::Point2F(p.x - spread, bottomRelY - lift), brush, 1.2f);
    rt->DrawLine(D2D1::Point2F(p.x, bottomRelY), D2D1::Point2F(p.x + spread, bottomRelY - lift), brush, 1.2f);
}

void DrawItemShape(CachedSurface& surf, const Settings& cfg, const FallingItem& p, float vx, float bottomRelY, double timeSec) {
    ID2D1SolidColorBrush* brush = surf.sharedBrush;
    float alpha = (cfg.performanceMode == PerformanceMode::Low) ? p.alpha * 0.85f : p.alpha;
    if (p.kind == ObjectKind::Rain) {
        float thick = (cfg.performanceMode == PerformanceMode::High && p.depthTier == 2) ? 2.0f : 1.5f;
        float lenScale = (cfg.performanceMode == PerformanceMode::Low) ? 0.78f : 1.0f;
        brush->SetColor(cfg.colorRain);
        brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
        surf.rt->DrawLine(D2D1::Point2F(p.x, p.y), D2D1::Point2F(p.x + vx * 0.02f, p.y + p.size * lenScale), brush, thick);
        brush->SetOpacity(1.0f);
        DrawRainSplashesHigh(surf.rt, brush, cfg, p, bottomRelY, timeSec);
        return;
    }
    if (p.kind == ObjectKind::Snow) {
        if (surf.flakeSprite) {
            float r = p.size * 1.35f;
            surf.rt->DrawBitmap(surf.flakeSprite, D2D1::RectF(p.x - r, p.y - r, p.x + r, p.y + r),
                                std::clamp(alpha, 0.05f, 1.0f), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            return;
        }
        brush->SetColor(cfg.colorSnow);
        brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
        surf.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(p.x, p.y), p.size * 0.5f, p.size * 0.5f), brush);
        brush->SetOpacity(1.0f);
        return;
    }
    D2D1::Matrix3x2F oldXform;
    surf.rt->GetTransform(&oldXform);
    surf.rt->SetTransform(D2D1::Matrix3x2F::Rotation(p.angle * 57.29578f, D2D1::Point2F(p.x, p.y)) * oldXform);
    brush->SetColor(cfg.colorHail);
    brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
    surf.rt->FillRectangle(D2D1::RectF(p.x - p.size * 0.45f, p.y - p.size * 0.45f, p.x + p.size * 0.45f, p.y + p.size * 0.45f), brush);
    if (cfg.performanceMode == PerformanceMode::High) {
        brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, std::clamp(alpha * 0.45f, 0.03f, 0.6f)));
        surf.rt->DrawLine(D2D1::Point2F(-p.size * 0.30f, -p.size * 0.30f), D2D1::Point2F(p.size * 0.15f, -p.size * 0.30f), brush, 1.0f);
    }
    brush->SetOpacity(1.0f);
    surf.rt->SetTransform(oldXform);
}

void PresentSurface(HWND hwnd, CachedSurface& surf) {
    if (!hwnd || !IsWindow(hwnd) || !surf.memDC) return;
    POINT ptDst = {g_canvasLeft, g_canvasTop};
    POINT ptSrc = {0, 0};
    SIZE sizeWnd = {g_canvasWidth, g_canvasHeight};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    if (!UpdateLayeredWindow(hwnd, nullptr, &ptDst, &sizeWnd, surf.memDC, &ptSrc, 0, &blend, ULW_ALPHA)) {
        Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
    }
}

void DrawVerticalGradientTint(ID2D1DCRenderTarget* rt, D2D1_COLOR_F topCol, D2D1_COLOR_F botCol,
                              float topY, float botY, float factor) {
    topCol.a = std::clamp(topCol.a * factor, 0.0f, 0.85f);
    botCol.a = std::clamp(botCol.a * factor, 0.0f, 0.85f);
    if (topCol.a <= 0.003f && botCol.a <= 0.003f) return;

    D2D1_GRADIENT_STOP stops[2] = {{0.0f, topCol}, {1.0f, botCol}};
    ID2D1GradientStopCollection* stopCol = nullptr;
    if (FAILED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCol)) || !stopCol) return;
    ID2D1LinearGradientBrush* grad = nullptr;
    if (SUCCEEDED(rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(0.0f, topY), D2D1::Point2F(0.0f, botY)),
            stopCol, &grad)) && grad) {
        rt->FillRectangle(D2D1::RectF(0.0f, topY, static_cast<float>(g_canvasWidth), botY), grad);
        grad->Release();
    }
    stopCol->Release();
}

void DrawTimeOfDayLayer(ID2D1DCRenderTarget* rt, const Settings& cfg, double hour,
                      bool& outIsNight, const wchar_t*& outPhaseLabel) {
    D2D1_COLOR_F topCol{}, botCol{};
    GetSystemTimeOfDayPalette(hour, topCol, botCol, outIsNight, outPhaseLabel);
    if (cfg.timeOfDayMode == TimeOfDayMode::Off) return;
    if (cfg.timeOfDayMode == TimeOfDayMode::Dawn) { topCol = D2D1::ColorF(0.95f, 0.58f, 0.45f, 0.20f); botCol = D2D1::ColorF(1.00f, 0.82f, 0.58f, 0.10f); outIsNight = false; outPhaseLabel = L"Dawn"; }
    else if (cfg.timeOfDayMode == TimeOfDayMode::Day) { topCol = D2D1::ColorF(0.75f, 0.88f, 1.00f, 0.08f); botCol = D2D1::ColorF(1.00f, 0.97f, 0.88f, 0.04f); outIsNight = false; outPhaseLabel = L"Day"; }
    else if (cfg.timeOfDayMode == TimeOfDayMode::Sunset) { topCol = D2D1::ColorF(0.88f, 0.42f, 0.36f, 0.22f); botCol = D2D1::ColorF(0.98f, 0.68f, 0.32f, 0.12f); outIsNight = false; outPhaseLabel = L"Sunset"; }
    else if (cfg.timeOfDayMode == TimeOfDayMode::Night) { topCol = D2D1::ColorF(0.06f, 0.10f, 0.24f, 0.24f); botCol = D2D1::ColorF(0.10f, 0.14f, 0.30f, 0.14f); outIsNight = true; outPhaseLabel = L"Night"; }
    if (cfg.hasCustomTimeOfDayColor) {
        topCol = cfg.colorTimeOfDay;
        botCol = cfg.colorTimeOfDay;
        botCol.a *= 0.5f;
    }
    float strength = (static_cast<float>(cfg.timeOfDayIntensity) / 25.0f) * ((cfg.performanceMode == PerformanceMode::Low) ? 0.75f : 1.0f);
    DrawVerticalGradientTint(rt, topCol, botCol, 0.0f, static_cast<float>(g_canvasHeight), strength);
}

void DrawDriftingCloudsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             const Settings& cfg, float cloudPct01, float windVx, double timeSec) {
    if (cloudPct01 <= 0.01f) return;
    D2D1_COLOR_F base = cfg.hasCustomCloudsColor
        ? cfg.colorClouds
        : D2D1::ColorF(0.93f - cloudPct01 * 0.25f, 0.95f - cloudPct01 * 0.25f, 0.98f - cloudPct01 * 0.20f, 0.36f);
    float driftSpeed = (std::abs(windVx) > 1.5f) ? windVx * 0.32f : 8.0f;
    float qScale = GetQualityScale(cfg.performanceMode);

    for (const auto& mon : g_monitorRects) {
        int puffCount = std::clamp(static_cast<int>((2.0f + cloudPct01 * 14.0f) * qScale), 1, 24);
        float bandH = std::max(70.0f, mon.height * (0.10f + cloudPct01 * 0.16f));
        for (int i = 0; i < puffCount; ++i) {
            float fi = static_cast<float>(i);
            float spanW = mon.width + 260.0f;
            float rawOffset = std::fmod(fi * (mon.width / static_cast<float>(puffCount)) +
                                        static_cast<float>(timeSec) * driftSpeed * (0.6f + 0.2f * (i % 3)), spanW);
            if (rawOffset < 0.0f) rawOffset += spanW;
            float cx = mon.left + rawOffset - 130.0f;
            float cy = mon.top + 22.0f + ((i * 37) % 100) * 0.01f * bandH;
            float rx = (68.0f + (i % 4) * 30.0f) * (0.65f + cloudPct01 * 0.55f);
            float ry = (22.0f + (i % 3) * 10.0f) * (0.65f + cloudPct01 * 0.50f);
            float edgeFade = ComputeMonitorEdgeFade(mon, cx, cy, rx + 45.0f);
            if (edgeFade <= 0.01f) continue;

            float alpha = std::clamp(
                base.a * (0.25f + cloudPct01 * 0.85f) *
                (0.88f + 0.12f * static_cast<float>(std::sin(timeSec * 0.7 + fi))) * edgeFade,
                0.01f, 0.68f);

            if (cfg.performanceMode != PerformanceMode::Low) {
                brush->SetOpacity(1.0f);
                brush->SetColor(D2D1::ColorF(base.r * 0.62f, base.g * 0.66f, base.b * 0.74f, alpha * 0.65f));
                rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + rx * 0.06f, cy + ry * 0.34f), rx * 0.92f, ry * 0.65f), brush);
            }
            D2D1_GRADIENT_STOP stops[2] = {
                {0.0f, D2D1::ColorF(std::min(1.0f, base.r * 1.05f), std::min(1.0f, base.g * 1.05f), std::min(1.0f, base.b * 1.05f), alpha)},
                {1.0f, D2D1::ColorF(base.r, base.g, base.b, 0.0f)}
            };
            ID2D1GradientStopCollection* stopsCol = nullptr;
            if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopsCol)) && stopsCol) {
                ID2D1RadialGradientBrush* rad = nullptr;
                if (SUCCEEDED(rt->CreateRadialGradientBrush(
                        D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, cy - ry * 0.15f), D2D1::Point2F(0.0f, 0.0f), rx, ry),
                        stopsCol, &rad)) && rad) {
                    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry), rad);
                    rad->Release();
                }
                stopsCol->Release();
            }
        }
    }
}

void DrawSunLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg,
                  bool active, double hour, const ForecastSnapshot& snap,
                  float uvIndex, float cloudDimFactor, double timeSec) {
    if (!active || cfg.sunGeom.radius <= 0) return;
    float uvT = std::clamp((uvIndex - 2.0f) / 9.0f, 0.0f, 1.0f);
    D2D1_COLOR_F base = cfg.hasCustomSunColor
        ? cfg.colorSun
        : D2D1::ColorF(1.0f, std::clamp(0.86f - uvT * 0.62f, 0.20f, 0.90f), std::clamp(0.38f - uvT * 0.30f, 0.08f, 0.45f), 0.92f);
    if (uvT > 0.01f) {
        base.r = std::clamp(base.r + uvT * 0.12f, 0.0f, 1.0f);
        base.g = std::clamp(base.g * (1.0f - uvT * 0.55f), 0.12f, 1.0f);
        base.b = std::clamp(base.b * (1.0f - uvT * 0.70f), 0.06f, 1.0f);
    }
    float baseVis = std::clamp(cloudDimFactor * (0.96f + 0.04f * static_cast<float>(std::sin(timeSec * (1.5 + uvT * 2.0)))), 0.18f, 1.0f);
    int rayCount = (cfg.performanceMode == PerformanceMode::Low) ? 0 : ((cfg.performanceMode == PerformanceMode::High) ? 16 : 12);
    float glowMult = (cfg.performanceMode == PerformanceMode::High) ? 3.2f : 2.7f;

    for (const auto& mon : g_monitorRects) {
        float radius = 0.0f, cx = 0.0f, cy = 0.0f;
        ComputeCelestialCoordsInMonitor(mon, true, cfg.sunMotionMode, cfg.sunGeom, hour, snap, radius, cx, cy);
        if (radius < 3.0f) continue;
        float visFactor = baseVis * ComputeMonitorEdgeFade(mon, cx, cy, radius * 2.4f + 24.0f);
        if (visFactor <= 0.01f) continue;

        float glowR = radius * glowMult;
        D2D1_GRADIENT_STOP stops[3] = {
            {0.00f, D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * 0.62f * visFactor, 0.0f, 0.78f))},
            {0.45f, D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * 0.22f * visFactor, 0.0f, 0.40f))},
            {1.00f, D2D1::ColorF(base.r, base.g, base.b, 0.0f)}
        };
        ID2D1GradientStopCollection* stopsCol = nullptr;
        if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopsCol)) && stopsCol) {
            ID2D1RadialGradientBrush* rad = nullptr;
            if (SUCCEEDED(rt->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, cy), D2D1::Point2F(0.0f, 0.0f), glowR, glowR),
                    stopsCol, &rad)) && rad) {
                rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), glowR, glowR), rad);
                rad->Release();
            }
            stopsCol->Release();
        }
        if (rayCount > 0) {
            brush->SetOpacity(1.0f);
            brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.32f * visFactor, 0.02f, 0.55f)));
            for (int rIdx = 0; rIdx < rayCount; ++rIdx) {
                double ang = (2.0 * kPi * rIdx) / rayCount + timeSec * 0.08;
                float innerR = radius * 1.18f, outerR = radius * ((rIdx % 2 == 0) ? 1.78f : 1.50f);
                rt->DrawLine(
                    D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * innerR, cy + static_cast<float>(std::sin(ang)) * innerR),
                    D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * outerR, cy + static_cast<float>(std::sin(ang)) * outerR),
                    brush, std::max(1.5f, radius * 0.045f));
            }
        }
        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * visFactor, 0.03f, 1.0f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius), brush);
    }
}

void DrawMoonLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg,
                   bool active, double hour, const ForecastSnapshot& snap,
                   double moonPhase01, float cloudDimFactor) {
    if (!active || cfg.moonGeom.radius <= 0) return;
    D2D1_COLOR_F base = cfg.hasCustomMoonColor ? cfg.colorMoon : D2D1::ColorF(0.88f, 0.92f, 1.0f, 0.95f);
    float baseVis = std::clamp(cloudDimFactor, 0.18f, 1.0f);
    double phase = std::clamp(moonPhase01, 0.0, 1.0), illum = 0.5 * (1.0 - std::cos(phase * 2.0 * kPi));
    float sideSign = (phase <= 0.5) ? 1.0f : -1.0f;

    for (const auto& mon : g_monitorRects) {
        float r = 0.0f, cx = 0.0f, cy = 0.0f;
        ComputeCelestialCoordsInMonitor(mon, false, cfg.moonMotionMode, cfg.moonGeom, hour, snap, r, cx, cy);
        if (r < 3.0f) continue;
        float visFactor = baseVis * ComputeMonitorEdgeFade(mon, cx, cy, r * 2.0f + 24.0f);
        if (visFactor <= 0.01f) continue;

        float glowR = r * 2.4f;
        D2D1_GRADIENT_STOP stops[2] = {
            {0.0f, D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.32f * visFactor, 0.0f, 0.50f))},
            {1.0f, D2D1::ColorF(base.r, base.g, base.b, 0.0f)}
        };
        ID2D1GradientStopCollection* stopsCol = nullptr;
        if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopsCol)) && stopsCol) {
            ID2D1RadialGradientBrush* rad = nullptr;
            if (SUCCEEDED(rt->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, cy), D2D1::Point2F(0.0f, 0.0f), glowR, glowR),
                    stopsCol, &rad)) && rad) {
                rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), glowR, glowR), rad);
                rad->Release();
            }
            stopsCol->Release();
        }
        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(0.14f, 0.17f, 0.25f, std::clamp(0.58f * visFactor, 0.02f, 0.78f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), brush);

        float litWidth = std::max(1.5f, static_cast<float>(illum) * r * 1.92f);
        brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * visFactor, 0.03f, 1.0f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + sideSign * (r - litWidth * 0.5f), cy), litWidth * 0.55f, r), brush);

        if (cfg.performanceMode != PerformanceMode::Low && illum > 0.18 && r >= 10.0f) {
            brush->SetColor(D2D1::ColorF(base.r * 0.72f, base.g * 0.75f, base.b * 0.84f, std::clamp(0.28f * visFactor, 0.01f, 0.42f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - r * 0.22f, cy - r * 0.18f), r * 0.24f, r * 0.18f), brush);
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.14f, cy + r * 0.22f), r * 0.20f, r * 0.14f), brush);
            if (cfg.performanceMode == PerformanceMode::High) {
                rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.26f, cy - r * 0.12f), r * 0.13f, r * 0.11f), brush);
            }
        }
    }
}

void DrawSmoothRainbow(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                     const Settings& cfg, float strength, double timeSec, bool forceDirty) {
    if (cfg.rainbowGeom.radius <= 0 || strength <= 0.004f) return;
    RefreshArcCache(g_rainbowCache, cfg.rainbowGeom, forceDirty);
    if (g_rainbowCache.radius < 4.0f) return;

    const D2D1_COLOR_F spectrum[6] = {
        D2D1::ColorF(1.0f, 0.22f, 0.28f, 0.24f), D2D1::ColorF(1.0f, 0.58f, 0.15f, 0.24f), D2D1::ColorF(1.0f, 0.90f, 0.20f, 0.24f),
        D2D1::ColorF(0.20f, 0.85f, 0.42f, 0.24f), D2D1::ColorF(0.22f, 0.62f, 1.0f, 0.24f), D2D1::ColorF(0.60f, 0.34f, 0.96f, 0.24f)
    };
    brush->SetOpacity(1.0f);
    float shimmer = (cfg.performanceMode == PerformanceMode::Low)
        ? 0.95f
        : (0.92f + 0.08f * static_cast<float>(std::sin(timeSec * 1.4)));
    int bands = (cfg.performanceMode == PerformanceMode::Low) ? 3 : 6;
    for (int i = 0; i < bands; ++i) {
        D2D1_COLOR_F c = cfg.hasCustomRainbowColor ? cfg.colorRainbow : spectrum[i];
        float alpha = std::clamp(c.a * strength * (1.0f - std::abs(i - (bands - 1) * 0.5f) * 0.14f) * shimmer, 0.0f, 0.65f);
        if (alpha <= 0.003f) continue;
        float r = g_rainbowCache.radius - i * (g_rainbowCache.bandThickness * 0.85f);
        if (r <= 2.0f) break;
        brush->SetColor(D2D1::ColorF(c.r, c.g, c.b, alpha));
        rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_rainbowCache.cx, g_rainbowCache.cy), r, r), brush, g_rainbowCache.bandThickness);
    }
}

void DrawSmoothAurora(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                    const Settings& cfg, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    D2D1_COLOR_F base = cfg.hasCustomAuroraColor ? cfg.colorAurora : D2D1::ColorF(0.20f, 0.94f, 0.62f, 0.28f);
    float bandHeight = std::max(90.0f, static_cast<float>(g_canvasHeight) * 0.22f);
    int ribbons = (cfg.performanceMode == PerformanceMode::Low) ? 1 : ((cfg.performanceMode == PerformanceMode::High) ? 3 : 2);
    float stepX = (cfg.performanceMode == PerformanceMode::High) ? 12.0f : ((cfg.performanceMode == PerformanceMode::Low) ? 40.0f : 24.0f);

    brush->SetOpacity(1.0f);
    for (int ribbon = 0; ribbon < ribbons; ++ribbon) {
        D2D1_COLOR_F cTop = base; cTop.a = std::clamp(base.a * (ribbon == 0 ? 1.0f : 0.60f) * strength, 0.0f, 0.65f);
        D2D1_COLOR_F cBot = cTop; cBot.a = 0.0f;
        float y0 = 16.0f + ribbon * 26.0f;
        DrawVerticalGradientTint(rt, cTop, cBot, y0, y0 + bandHeight, 1.0f);
        brush->SetColor(D2D1::ColorF(cTop.r, cTop.g, cTop.b, std::clamp(cTop.a * 1.15f, 0.0f, 0.75f)));
        float prevX = 0.0f, prevY = y0 + static_cast<float>(std::sin(timeSec * 1.1 + ribbon)) * 14.0f;
        for (float x = stepX; x <= static_cast<float>(g_canvasWidth); x += stepX) {
            float curY = y0 + static_cast<float>(std::sin(x * 0.0045f + timeSec * (1.1 + ribbon * 0.3))) * 18.0f;
            rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 3.0f);
            prevX = x;
            prevY = curY;
        }
    }
}

void DrawSmoothMirage(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                    const Settings& cfg, float strength, float bottomRelY, double timeSec) {
    if (strength <= 0.004f) return;
    float botY = bottomRelY, bandH = std::max(50.0f, static_cast<float>(g_canvasHeight) * 0.14f), topY = std::max(0.0f, botY - bandH);
    D2D1_COLOR_F base = cfg.hasCustomMirageColor ? cfg.colorMirage : D2D1::ColorF(1.0f, 0.88f, 0.68f, 0.25f);
    D2D1_COLOR_F topCol = base; topCol.a = 0.0f;
    D2D1_COLOR_F botCol = base; botCol.a = std::clamp(base.a * strength, 0.0f, 0.55f);
    DrawVerticalGradientTint(rt, topCol, botCol, topY, botY, 1.0f);

    int lines = (cfg.performanceMode == PerformanceMode::High) ? 4 : ((cfg.performanceMode == PerformanceMode::Low) ? 2 : 3);
    float stepX = (cfg.performanceMode == PerformanceMode::High) ? 12.0f : 20.0f;
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * 0.75f * strength, 0.0f, 0.5f)));
    for (int line = 0; line < lines; ++line) {
        float baseLineY = topY + bandH * (0.25f + line * (0.65f / std::max(1, lines - 1)));
        float prevX = 0.0f, prevY = baseLineY + static_cast<float>(std::sin(timeSec * 2.4 + line * 1.7)) * 3.0f;
        for (float x = stepX; x <= static_cast<float>(g_canvasWidth); x += stepX) {
            float curY = baseLineY + static_cast<float>(std::sin(x * 0.012f + timeSec * 2.4 + line * 1.7)) * 3.5f;
            rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 1.5f);
            prevX = x;
            prevY = curY;
        }
    }
}

void DrawBranchedLightning(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                         const Settings& cfg, const LightningFlash& flash, float maxHeight, float alpha) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(brush->GetColor().r, brush->GetColor().g, brush->GetColor().b, alpha));
    float curX = flash.xStart, curY = 0.0f;
    int segs = (cfg.performanceMode == PerformanceMode::Low) ? 10 : 14;
    float stepY = std::max(20.0f, maxHeight * 0.72f / segs);
    for (int i = 0; i < segs; ++i) {
        float t = static_cast<float>(i + 1) / segs;
        float nextX = flash.xStart + (flash.xEnd - flash.xStart) * t + ((i % 2 == 0) ? 16.0f : -16.0f);
        float nextY = curY + stepY;
        rt->DrawLine(D2D1::Point2F(curX, curY), D2D1::Point2F(nextX, nextY), brush, 2.8f);
        if (cfg.performanceMode != PerformanceMode::Low && (i == 4 || i == 8 || (cfg.performanceMode == PerformanceMode::High && i == 11))) {
            float branchDir = (i == 4 || i == 11) ? 1.0f : -1.0f;
            float b1x = nextX + branchDir * 28.0f, b1y = nextY + stepY * 1.4f;
            rt->DrawLine(D2D1::Point2F(nextX, nextY), D2D1::Point2F(b1x, b1y), brush, 1.6f);
            rt->DrawLine(D2D1::Point2F(b1x, b1y), D2D1::Point2F(b1x + branchDir * 16.0f, b1y + stepY * 1.1f), brush, 1.1f);
        }
        curX = nextX;
        curY = nextY;
    }
}

void DrawWindStreamsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                          const Settings& cfg, const ForecastSnapshot& snap, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    float gustKmh = static_cast<float>(std::max({snap.windGustsKmh, snap.windSpeedKmh, 28.0}));
    float speedPx = std::clamp(gustKmh * 6.5f, 160.0f, 520.0f);
    float dir = (snap.windDirSign != 0) ? static_cast<float>(snap.windDirSign) : 1.0f;
    int streaks = std::clamp(static_cast<int>(8 * GetQualityScale(cfg.performanceMode)), 5, 18);
    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        for (int i = 0; i < streaks; ++i) {
            float fi = static_cast<float>(i), spanW = mon.width + 220.0f;
            float rawX = std::fmod(fi * (mon.width / static_cast<float>(streaks)) +
                                   static_cast<float>(timeSec) * speedPx * (0.75f + 0.15f * (i % 3)) * dir, spanW);
            if (rawX < 0.0f) rawX += spanW;
            float x0 = mon.left + rawX - 110.0f, y0 = mon.top + 40.0f + ((i * 53) % 100) * 0.0075f * mon.height, len = 45.0f + (i % 4) * 18.0f;
            float fade = ComputeMonitorEdgeFade(mon, x0 + len * 0.5f, y0, 40.0f);
            brush->SetColor(D2D1::ColorF(0.86f, 0.93f, 1.0f, std::clamp(0.22f * strength * fade, 0.02f, 0.45f)));
            rt->DrawLine(D2D1::Point2F(x0, y0), D2D1::Point2F(x0 + len * dir, y0 + static_cast<float>(std::sin(timeSec * 4.0 + fi)) * 5.0f), brush, 1.4f);
        }
    }
}

void DrawThermalLayer(ID2D1DCRenderTarget* rt, bool frostOn, bool heatOn, float strength) {
    if (strength <= 0.004f) return;
    if (frostOn) {
        DrawVerticalGradientTint(rt, D2D1::ColorF(0.76f, 0.90f, 1.0f, 0.16f), D2D1::ColorF(0.84f, 0.94f, 1.0f, 0.22f),
                                 0.0f, static_cast<float>(g_canvasHeight), strength);
    }
    if (heatOn) {
        DrawVerticalGradientTint(rt, D2D1::ColorF(1.0f, 0.78f, 0.42f, 0.08f), D2D1::ColorF(1.0f, 0.58f, 0.26f, 0.18f),
                                 0.0f, static_cast<float>(g_canvasHeight), strength);
    }
}

void DrawNightStarsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    int starsCount = std::clamp(static_cast<int>(28 * GetQualityScale(cfg.performanceMode)), 12, 55);
    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        float skyH = mon.height * 0.45f;
        for (int i = 0; i < starsCount; ++i) {
            float sx = mon.left + (((i * 97 + 19) % 1000) / 1000.0f) * mon.width;
            float sy = mon.top + 12.0f + (((i * 61 + 7) % 1000) / 1000.0f) * skyH;
            float twinkle = 0.45f + 0.55f * static_cast<float>(std::sin(timeSec * 2.2 + i * 1.3));
            float r = (i % 5 == 0 && cfg.performanceMode == PerformanceMode::High) ? 2.0f : 1.35f;
            brush->SetColor(D2D1::ColorF(0.92f, 0.96f, 1.0f, std::clamp(0.55f * strength * twinkle, 0.04f, 0.85f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), r, r), brush);
        }
    }
}

void DrawAirQualityLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, const ForecastSnapshot& snap, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    float aqiNorm = std::clamp(static_cast<float>(std::max(snap.aqiValue, snap.pm25Value * 2.5)) / 150.0f, 0.15f, 1.0f);
    D2D1_COLOR_F base = cfg.hasCustomAirQualityColor ? cfg.colorAirQuality : D2D1::ColorF(0.82f, 0.66f, 0.34f, 0.22f);
    D2D1_COLOR_F topCol = base; topCol.a = base.a * 0.45f * aqiNorm;
    D2D1_COLOR_F botCol = base; botCol.a = base.a * (0.75f + 0.45f * aqiNorm);
    DrawVerticalGradientTint(rt, topCol, botCol, 0.0f, static_cast<float>(g_canvasHeight), strength);

    if (cfg.performanceMode != PerformanceMode::Low) {
        int motes = (cfg.performanceMode == PerformanceMode::High) ? 24 : 12;
        brush->SetOpacity(1.0f);
        for (const auto& mon : g_monitorRects) {
            for (int i = 0; i < motes; ++i) {
                uint32_t hx = static_cast<uint32_t>(i * 2654435761u + 1013904223u);
                uint32_t hy = static_cast<uint32_t>(i * 1664525u + 22695477u);
                float u01 = static_cast<float>(hx & 0xFFFFu) / 65535.0f;
                float v01 = static_cast<float>(hy & 0xFFFFu) / 65535.0f;
                float mx = mon.left + std::fmod(u01 * mon.width + static_cast<float>(timeSec) * 10.0f +
                                                static_cast<float>(std::sin(timeSec * 1.1 + i)) * 14.0f, mon.width);
                float my = mon.top + std::fmod(v01 * mon.height +
                                               static_cast<float>(std::cos(timeSec * 0.9 + i * 1.7)) * 12.0f, mon.height);
                float fade = ComputeMonitorEdgeFade(mon, mx, my, 24.0f);
                brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.28f * strength * aqiNorm * fade, 0.02f, 0.45f)));
                rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(mx, my), 2.2f, 2.2f), brush);
            }
        }
    }
}

void DrawPollenLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                     const Settings& cfg, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    int count = std::clamp(static_cast<int>(24 * GetQualityScale(cfg.performanceMode)), 10, 48);
    D2D1_COLOR_F base = cfg.hasCustomPollenColor ? cfg.colorPollen : D2D1::ColorF(0.97f, 0.89f, 0.42f, 0.72f);
    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        for (int i = 0; i < count; ++i) {
            uint32_t hx = static_cast<uint32_t>(i * 2246822519u + 3266489917u);
            uint32_t hy = static_cast<uint32_t>(i * 374761393u + 668265263u);
            float u01 = static_cast<float>(hx & 0xFFFFu) / 65535.0f;
            float v01 = static_cast<float>(hy & 0xFFFFu) / 65535.0f;
            float px = mon.left + std::fmod(u01 * mon.width + static_cast<float>(timeSec) * 11.0f +
                                            static_cast<float>(std::sin(timeSec * 1.2 + i * 1.9)) * 20.0f, mon.width);
            float py = mon.top + std::fmod(v01 * mon.height +
                                           static_cast<float>(std::cos(timeSec * 1.0 + i * 2.3)) * 18.0f, mon.height);
            float fade = ComputeMonitorEdgeFade(mon, px, py, 28.0f);
            float r = (cfg.performanceMode == PerformanceMode::High && i % 3 == 0) ? 2.8f : 2.0f;
            brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * strength * fade, 0.03f, 0.85f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), r, r), brush);
        }
    }
}

void DrawFloodWaterLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, const ForecastSnapshot& snap, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    float rainScale = std::clamp(static_cast<float>(snap.precipMm / 6.0), 0.35f, 2.2f);
    D2D1_COLOR_F base = cfg.hasCustomFloodWaterColor ? cfg.colorFloodWater : D2D1::ColorF(0.16f, 0.50f, 0.76f, 0.42f);
    float stepX = (cfg.performanceMode == PerformanceMode::High) ? 14.0f : ((cfg.performanceMode == PerformanceMode::Low) ? 40.0f : 24.0f);

    for (const auto& mon : g_monitorRects) {
        float botY = mon.top + mon.bottomLimitRelY;
        float waterH = std::clamp((18.0f + rainScale * 22.0f) * std::min(1.5f, 0.6f + strength * 0.5f), 12.0f, 95.0f);
        float topY = std::max(mon.top + 20.0f, botY - waterH);
        D2D1_GRADIENT_STOP stops[2] = {
            {0.0f, D2D1::ColorF(base.r * 1.15f, base.g * 1.15f, std::min(1.0f, base.b * 1.15f), std::clamp(base.a * 0.55f * strength, 0.03f, 0.65f))},
            {1.0f, D2D1::ColorF(base.r * 0.75f, base.g * 0.80f, base.b * 0.90f, std::clamp(base.a * 0.95f * strength, 0.05f, 0.80f))}
        };
        ID2D1GradientStopCollection* stopsCol = nullptr;
        if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopsCol)) && stopsCol) {
            ID2D1LinearGradientBrush* grad = nullptr;
            if (SUCCEEDED(rt->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(D2D1::Point2F(mon.left, topY), D2D1::Point2F(mon.left, botY)),
                    stopsCol, &grad)) && grad) {
                rt->FillRectangle(D2D1::RectF(mon.left, topY, mon.left + mon.width, botY), grad);
                grad->Release();
            }
            stopsCol->Release();
        }
        if (cfg.performanceMode != PerformanceMode::Low) {
            brush->SetOpacity(1.0f);
            brush->SetColor(D2D1::ColorF(0.82f, 0.94f, 1.0f, std::clamp(0.42f * strength, 0.04f, 0.65f)));
            float prevX = mon.left, prevY = topY + static_cast<float>(std::sin(timeSec * 3.0 + mon.left * 0.01)) * 4.0f;
            for (float x = mon.left + stepX; x <= mon.left + mon.width; x += stepX) {
                float curY = topY + static_cast<float>(std::sin(x * 0.022f + timeSec * 3.0)) * (3.5f + rainScale * 2.0f);
                rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 2.0f);
                prevX = x;
                prevY = curY;
            }
        }
    }
}

void DrawRoundedPillButton(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, const InteractiveControl& ctrl) {
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(ctrl.rect, 5.0f, 5.0f);
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(ctrl.tint.r, ctrl.tint.g, ctrl.tint.b, 0.24f));
    rt->FillRoundedRectangle(rr, brush);
    brush->SetColor(D2D1::ColorF(ctrl.tint.r, ctrl.tint.g, ctrl.tint.b, 0.85f));
    rt->DrawRoundedRectangle(rr, brush, 1.2f);
    brush->SetColor(cfg.colorHudText);
    rt->DrawTextW(ctrl.btnText.c_str(), static_cast<UINT32>(ctrl.btnText.size()), g_hudPillFormat, ctrl.rect, brush);
}

void DrawForecastHudWidget(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, const ForecastSnapshot& snap,
                           bool activePrecip, const GeneralEffectsState& activeFx,
                           const wchar_t* todLabel, float cloudPct01,
                           float uvIndex, double moonPhase01, int bottomLimitY) {
    float btnR = cfg.hudButtonRadiusPx;
    bool expanded = g_hudExpanded.load();
    std::wstring labelText = L"--°";
    if (expanded) {
        labelText = L"×";
    } else if (snap.valid) {
        wchar_t tBuf[24] = {};
        swprintf_s(tBuf, L"%d°", static_cast<int>(std::llround(snap.tempC)));
        labelText = tBuf;
    }

    int glyphCount = std::max(1, static_cast<int>(labelText.size()));
    float maxByHeight = btnR * 0.88f;
    float maxByWidth = (btnR * 1.55f) / static_cast<float>(glyphCount);
    float circleFontPx = std::clamp(std::min(maxByHeight, maxByWidth), 8.0f, 42.0f);

    if (!cfg.forecastHudEnabled || !EnsureDirectWrite(cfg.hudFontSizePx, circleFontPx)) {
        g_hudBtnR.store(0.0f);
        return;
    }

    const MonitorCanvasRect& primaryMon = g_monitorRects.front();
    float btnCx = 0.0f, btnCy = 0.0f;
    bool isDragging = false;
    ExpandedSection expSec = ExpandedSection::None;
    {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        isDragging = g_draggingHudBtn;
        expSec = g_expandedSection;
        if (isDragging) {
            btnCx = g_hudBtnCx.load();
            btnCy = g_hudBtnCy.load();
        } else if (cfg.hasCustomBtnPos) {
            btnCx = static_cast<float>(cfg.hudBtnRelX * static_cast<double>(g_canvasWidth));
            btnCy = static_cast<float>(cfg.hudBtnRelY * static_cast<double>(g_canvasHeight));
        } else {
            btnCx = primaryMon.left + primaryMon.width * (static_cast<float>(cfg.hudButtonGeom.centerX) / 100.0f);
            btnCy = primaryMon.top + primaryMon.height * (1.0f - static_cast<float>(cfg.hudButtonGeom.centerY) / 100.0f);
        }
    }

    float bottomRelY = static_cast<float>(bottomLimitY - g_canvasTop);
    btnCx = std::clamp(btnCx, primaryMon.left + btnR + 6.0f, primaryMon.left + primaryMon.width - btnR - 6.0f);
    btnCy = std::clamp(btnCy, primaryMon.top + btnR + 6.0f, primaryMon.top + bottomRelY - btnR - 6.0f);
    g_hudBtnCx.store(btnCx);
    g_hudBtnCy.store(btnCy);
    g_hudBtnR.store(btnR);

    auto triBadge = [](TriStateMode m) -> std::pair<std::wstring, D2D1_COLOR_F> {
        if (m == TriStateMode::On) return {L"ON", D2D1::ColorF(0.25f, 0.85f, 0.48f, 0.9f)};
        if (m == TriStateMode::Off) return {L"OFF", D2D1::ColorF(0.85f, 0.35f, 0.35f, 0.9f)};
        return {L"A", D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f)};
    };

    struct HudRow {
        std::wstring label, value;
        bool isHeader = false;
        std::wstring actionId, btnLabel;
        D2D1_COLOR_F btnTint = D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.90f);
        HudFieldId editField = HudFieldId::None;
    };

    std::vector<HudRow> rows;
    wchar_t buf[96] = {};
    rows.push_back({
        L"FORECAST", snap.valid ? (snap.sourceLabel + L"  " + snap.lastUpdateText) : L"No data",
        true, L"manual_sync", L"SYNC", D2D1::ColorF(0.30f, 0.78f, 0.95f, 0.9f), HudFieldId::None
    });
    rows.push_back({
        L"Coords", cfg.coordsRaw.empty() ? L"(empty)" : cfg.coordsRaw, false,
        L"cycle_source", cfg.primarySource, D2D1::ColorF(0.45f, 0.72f, 0.98f, 0.9f),
        cfg.primarySource != L"off" ? HudFieldId::Coords : HudFieldId::None
    });
    rows.push_back({L"Condition", snap.valid ? WmoCodeToLabel(snap.weatherCode) : L"N/A", false, L"", L"", D2D1::ColorF(0,0,0,0), HudFieldId::None});
    if (snap.valid) swprintf_s(buf, L"%.1f °C", snap.tempC); else swprintf_s(buf, L"-- °C");
    rows.push_back({L"Temperature", buf, false, L"", L"", D2D1::ColorF(0,0,0,0), HudFieldId::None});
    if (snap.valid) swprintf_s(buf, L"%.1f km/h (%s)", snap.windSpeedKmh, WindDegToCompass(snap.windDirDeg)); else swprintf_s(buf, L"-- km/h");
    rows.push_back({L"Wind", buf, false, L"", L"", D2D1::ColorF(0,0,0,0), HudFieldId::None});
    if (snap.valid) swprintf_s(buf, L"%.0f%%", snap.cloudCoverPct); else swprintf_s(buf, L"--%%");
    rows.push_back({L"Cloud cover", buf, false, L"", L"", D2D1::ColorF(0,0,0,0), HudFieldId::None});
    if (snap.valid) swprintf_s(buf, L"%.1f mm", snap.precipMm); else swprintf_s(buf, L"-- mm");
    rows.push_back({L"Precipitation", buf, false, L"", L"", D2D1::ColorF(0,0,0,0), HudFieldId::None});
    if (snap.valid) swprintf_s(buf, L"%.1f", snap.uvIndex); else swprintf_s(buf, L"--");
    rows.push_back({L"UV index", buf, false, L"", L"", D2D1::ColorF(0,0,0,0), HudFieldId::None});
    if (snap.valid) swprintf_s(buf, L"%.0f / PM2.5 %.1f", snap.aqiValue, snap.pm25Value); else swprintf_s(buf, L"-- AQI");
    rows.push_back({L"Air quality", buf, false, L"", L"", D2D1::ColorF(0,0,0,0), HudFieldId::None});

    rows.push_back({
        L"[Sky]", expSec == ExpandedSection::Sky ? L"Expanded" : L"Collapsed", true,
        L"sec_sky", expSec == ExpandedSection::Sky ? L"HIDE" : L"OPEN", D2D1::ColorF(0.45f, 0.75f, 1.0f, 0.9f), HudFieldId::None
    });
    if (expSec == ExpandedSection::Sky) {
        auto cB = triBadge(cfg.cloudsMode); swprintf_s(buf, L"%d%%", cfg.cloudsIntensity);
        rows.push_back({L"Clouds", cfg.cloudsMode == TriStateMode::On ? buf : L"", false, L"cycle_clouds", cB.first, cB.second, cfg.cloudsMode == TriStateMode::On ? HudFieldId::CloudsIntensity : HudFieldId::None});
        auto sB = triBadge(cfg.sunMode); swprintf_s(buf, L"%.1f", uvIndex);
        rows.push_back({L"Sun", cfg.sunMode == TriStateMode::On ? buf : L"", false, L"cycle_sun", sB.first, sB.second, cfg.sunMode == TriStateMode::On ? HudFieldId::SunUvOverride : HudFieldId::None});
        auto mB = triBadge(cfg.moonMode); swprintf_s(buf, L"%.2f", moonPhase01);
        rows.push_back({L"Moon", cfg.moonMode == TriStateMode::On ? buf : L"", false, L"cycle_moon", mB.first, mB.second, cfg.moonMode == TriStateMode::On ? HudFieldId::MoonPhaseOverride : HudFieldId::None});
        rows.push_back({L"Over clouds", L"", false, L"toggle_celestial_front", cfg.celestialInFrontOfClouds ? L"FRONT" : L"DIM", D2D1::ColorF(0.65f, 0.78f, 0.95f, 0.9f), HudFieldId::None});
    }

    rows.push_back({
        L"[Precipitation]", expSec == ExpandedSection::Precipitation ? L"Expanded" : L"Collapsed", true,
        L"sec_precip", expSec == ExpandedSection::Precipitation ? L"HIDE" : L"OPEN", D2D1::ColorF(0.40f, 0.78f, 1.0f, 0.9f), HudFieldId::None
    });
    if (expSec == ExpandedSection::Precipitation) {
        const wchar_t* pModeStr = (cfg.precipitationMode == PrecipMode::Auto) ? L"A" : ((cfg.precipitationMode == PrecipMode::Rain) ? L"RAIN" : ((cfg.precipitationMode == PrecipMode::Snow) ? L"SNOW" : ((cfg.precipitationMode == PrecipMode::Hail) ? L"HAIL" : L"OFF")));
        rows.push_back({L"Mode", L"", false, L"cycle_precip_mode", pModeStr, D2D1::ColorF(0.35f, 0.72f, 1.0f, 0.9f), HudFieldId::None});
        const wchar_t* dStr = (cfg.precipFallDirection == FallDirMode::Auto) ? L"A" : ((cfg.precipFallDirection == FallDirMode::Down) ? L"DOWN" : ((cfg.precipFallDirection == FallDirMode::DownLeft) ? L"LEFT" : L"RIGHT"));
        rows.push_back({L"Direction", L"", false, L"cycle_precip_dir", dStr, D2D1::ColorF(0.45f, 0.78f, 1.0f, 0.9f), HudFieldId::None});
        swprintf_s(buf, L"%.1f m/s", cfg.precipFallSpeedMps);
        rows.push_back({L"Fall speed", cfg.precipSpeedMode == PrecipSpeedMode::Custom ? buf : L"", false, L"cycle_precip_speed", cfg.precipSpeedMode == PrecipSpeedMode::Auto ? L"A" : L"CUSTOM", D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f), cfg.precipSpeedMode == PrecipSpeedMode::Custom ? HudFieldId::PrecipSpeedMps : HudFieldId::None});
        swprintf_s(buf, L"%d%%", cfg.precipIntensity);
        bool showPrecipInt = (cfg.precipitationMode != PrecipMode::Off && cfg.precipitationMode != PrecipMode::Auto);
        rows.push_back({L"Intensity", showPrecipInt ? buf : L"", false, L"", L"", D2D1::ColorF(0,0,0,0), showPrecipInt ? HudFieldId::PrecipIntensity : HudFieldId::None});
    }

    rows.push_back({
        L"[Effects]", expSec == ExpandedSection::Effects ? L"Expanded" : L"Collapsed", true,
        L"sec_effects", expSec == ExpandedSection::Effects ? L"HIDE" : L"OPEN", D2D1::ColorF(0.95f, 0.72f, 0.35f, 0.9f), HudFieldId::None
    });
    if (expSec == ExpandedSection::Effects) {
        swprintf_s(buf, L"%d%%", cfg.effectsIntensity);
        bool anyManualFx = (cfg.effectClear == TriStateMode::On || cfg.effectPartlyCloudy == TriStateMode::On || cfg.effectOvercast == TriStateMode::On ||
                            cfg.effectDust == TriStateMode::On || cfg.effectFog == TriStateMode::On || cfg.effectSmog == TriStateMode::On ||
                            cfg.effectThunderstorm == TriStateMode::On || cfg.effectAurora == TriStateMode::On || cfg.effectMirage == TriStateMode::On ||
                            cfg.effectRainbow == TriStateMode::On || cfg.effectWindStreams == TriStateMode::On || cfg.effectThermal == TriStateMode::On ||
                            cfg.effectNightStars == TriStateMode::On || cfg.effectAirQuality == TriStateMode::On || cfg.effectPollen == TriStateMode::On ||
                            cfg.effectFloodWater == TriStateMode::On);
        rows.push_back({L"Intensity", anyManualFx ? buf : L"", false, L"", L"", D2D1::ColorF(0,0,0,0), anyManualFx ? HudFieldId::EffectsIntensity : HudFieldId::None});
        const wchar_t* perfStr = (cfg.performanceMode == PerformanceMode::High) ? L"HIGH" : ((cfg.performanceMode == PerformanceMode::Medium) ? L"MEDIUM" : L"LOW");
        rows.push_back({L"Performance", L"", false, L"cycle_perf", perfStr, D2D1::ColorF(0.30f, 0.85f, 0.65f, 0.9f), HudFieldId::None});
        const wchar_t* todStr = (cfg.timeOfDayMode == TimeOfDayMode::Auto) ? L"A" : ((cfg.timeOfDayMode == TimeOfDayMode::Dawn) ? L"DAWN" : ((cfg.timeOfDayMode == TimeOfDayMode::Day) ? L"DAY" : ((cfg.timeOfDayMode == TimeOfDayMode::Sunset) ? L"SUNSET" : ((cfg.timeOfDayMode == TimeOfDayMode::Night) ? L"NIGHT" : L"OFF"))));
        rows.push_back({L"Time of day", L"", false, L"cycle_tod", todStr, D2D1::ColorF(0.95f, 0.72f, 0.35f, 0.9f), HudFieldId::None});
        auto addFxRow = [&](const wchar_t* name, TriStateMode m, const wchar_t* act) {
            auto b = triBadge(m);
            rows.push_back({name, L"", false, act, b.first, b.second, HudFieldId::None});
        };
        addFxRow(L"Clear glow", cfg.effectClear, L"cycle_fx_clear");
        addFxRow(L"Partly cloudy", cfg.effectPartlyCloudy, L"cycle_fx_partly");
        addFxRow(L"Overcast", cfg.effectOvercast, L"cycle_fx_overcast");
        addFxRow(L"Dust", cfg.effectDust, L"cycle_fx_dust");
        addFxRow(L"Fog", cfg.effectFog, L"cycle_fx_fog");
        addFxRow(L"Smog", cfg.effectSmog, L"cycle_fx_smog");
        addFxRow(L"Thunderstorm", cfg.effectThunderstorm, L"cycle_fx_storm");
        addFxRow(L"Aurora", cfg.effectAurora, L"cycle_fx_aurora");
        addFxRow(L"Mirage", cfg.effectMirage, L"cycle_fx_mirage");
        addFxRow(L"Rainbow", cfg.effectRainbow, L"cycle_fx_rainbow");
        addFxRow(L"Wind streams", cfg.effectWindStreams, L"cycle_fx_wind");
        addFxRow(L"Frost / Heat", cfg.effectThermal, L"cycle_fx_thermal");
        addFxRow(L"Night stars", cfg.effectNightStars, L"cycle_fx_stars");
        addFxRow(L"Air quality", cfg.effectAirQuality, L"cycle_fx_aq");
        addFxRow(L"Pollen", cfg.effectPollen, L"cycle_fx_pollen");
        addFxRow(L"Flood water", cfg.effectFloodWater, L"cycle_fx_flood");
    }

    std::vector<InteractiveControl> nextControls;
    if (expanded) {
        const float padX = 12.0f, padY = 10.0f;
        const float rowH = std::max(22.0f, cfg.hudFontSizePx + 11.0f);
        const float headerH = std::max(24.0f, cfg.hudFontSizePx + 13.0f);
        const float pillW = std::max(64.0f, cfg.hudFontSizePx * 5.6f);
        const float pillH = std::max(18.0f, cfg.hudFontSizePx + 6.0f);
        const float valW = std::max(120.0f, cfg.hudFontSizePx * 10.5f);
        const float labelW = std::max(115.0f, cfg.hudFontSizePx * 10.0f);
        const float gap = static_cast<float>(cfg.forecastHudMarginPx);
        float cardW = padX * 2.0f + labelW + valW + pillW + 24.0f;
        float totalContentH = padY * 2.0f;
        for (const auto& r : rows) totalContentH += r.isHeader ? headerH : rowH;
        float availH = std::max(120.0f, bottomRelY - gap * 2.0f);
        bool needsScroll = totalContentH > availH;
        float cardH = std::min(totalContentH, availH);

        float spaceRight = (primaryMon.left + primaryMon.width - gap) - (btnCx + btnR + gap);
        float spaceLeft = (btnCx - btnR - gap) - (primaryMon.left + gap);
        float left = (spaceRight >= cardW)
            ? (btnCx + btnR + gap)
            : ((spaceLeft >= cardW)
                ? (btnCx - btnR - gap - cardW)
                : std::clamp(btnCx - cardW * 0.5f, primaryMon.left + gap, primaryMon.left + primaryMon.width - cardW - gap));

        float spaceTop = (btnCy - btnR - gap) - (primaryMon.top + gap);
        float spaceBottom = (bottomRelY - gap) - (btnCy + btnR + gap);
        float top = (spaceTop >= cardH)
            ? (btnCy - btnR - gap - cardH)
            : ((spaceBottom >= cardH)
                ? (btnCy + btnR + gap)
                : std::clamp(btnCy - cardH * 0.5f, primaryMon.top + gap, bottomRelY - cardH - gap));

        int maxScroll = needsScroll ? static_cast<int>(std::ceil(totalContentH - cardH)) : 0, scrollPx = 0;
        HudFieldId focusedField = HudFieldId::None;
        std::wstring editBuf;
        {
            std::lock_guard<std::mutex> lock(g_hudUiMutex);
            g_hudMaxScrollPx = maxScroll;
            g_hudScrollPx = needsScroll ? std::clamp(g_hudScrollPx, 0, maxScroll) : 0;
            scrollPx = g_hudScrollPx;
            focusedField = g_focusedField;
            editBuf = g_editBuffer;
        }

        D2D1_ROUNDED_RECT cardRect = D2D1::RoundedRect(D2D1::RectF(left, top, left + cardW, top + cardH), 10.0f, 10.0f);
        brush->SetOpacity(1.0f);
        brush->SetColor(cfg.colorHudBg);
        rt->FillRoundedRectangle(cardRect, brush);
        D2D1_COLOR_F borderCol = cfg.colorHudText;
        borderCol.a = std::clamp(cfg.colorHudBg.a * 0.35f, 0.12f, 0.45f);
        brush->SetColor(borderCol);
        rt->DrawRoundedRectangle(cardRect, brush, 1.0f);

        rt->PushAxisAlignedClip(D2D1::RectF(left + 2.0f, top + 4.0f, left + cardW - 2.0f, top + cardH - 4.0f),
                               D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        D2D1_COLOR_F sepCol = cfg.colorHudText; sepCol.a = 0.14f;
        D2D1_COLOR_F mutedText = cfg.colorHudText; mutedText.a = std::clamp(cfg.colorHudText.a * 0.78f, 0.42f, 0.92f);

        float curY = top + padY - static_cast<float>(scrollPx);
        for (size_t i = 0; i < rows.size(); ++i) {
            const auto& r = rows[i];
            float h = r.isHeader ? headerH : rowH;
            if (curY + h >= top && curY <= top + cardH) {
                if (r.isHeader && i > 0) {
                    brush->SetColor(sepCol);
                    rt->DrawLine(D2D1::Point2F(left + padX, curY - 2.0f), D2D1::Point2F(left + cardW - padX, curY - 2.0f), brush, 1.0f);
                }
                IDWriteTextFormat* fmt = r.isHeader ? g_hudTitleFormat : g_hudBodyFormat;
                brush->SetColor(r.isHeader ? cfg.colorHudText : mutedText);
                rt->DrawTextW(r.label.c_str(), static_cast<UINT32>(r.label.size()), fmt,
                              D2D1::RectF(left + padX, curY, left + padX + labelW, curY + h), brush);

                float cursorX = left + cardW - padX - (needsScroll ? 8.0f : 0.0f);
                if (!r.actionId.empty()) {
                    D2D1_RECT_F pillRect = D2D1::RectF(cursorX - pillW, curY + (h - pillH) * 0.5f, cursorX, curY + (h + pillH) * 0.5f);
                    InteractiveControl ic;
                    ic.rect = pillRect;
                    ic.actionId = r.actionId;
                    ic.btnText = r.btnLabel;
                    ic.tint = r.btnTint;
                    DrawRoundedPillButton(rt, brush, cfg, ic);
                    nextControls.push_back(ic);
                    cursorX -= (pillW + 8.0f);
                }
                if (!r.value.empty() || r.editField != HudFieldId::None) {
                    bool isEditing = (r.editField != HudFieldId::None && focusedField == r.editField);
                    std::wstring dispVal = isEditing ? (editBuf + L"|") : r.value;
                    D2D1_RECT_F valRect = D2D1::RectF(cursorX - valW, curY + 2.0f, cursorX, curY + h - 2.0f);
                    if (r.editField != HudFieldId::None) {
                        brush->SetOpacity(1.0f);
                        brush->SetColor(isEditing
                            ? D2D1::ColorF(0.22f, 0.45f, 0.78f, 0.35f)
                            : D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.08f));
                        rt->FillRoundedRectangle(D2D1::RoundedRect(valRect, 4.0f, 4.0f), brush);
                        InteractiveControl ic;
                        ic.rect = valRect;
                        ic.fieldId = r.editField;
                        nextControls.push_back(ic);
                    }
                    brush->SetColor(cfg.colorHudText);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    rt->DrawTextW(dispVal.c_str(), static_cast<UINT32>(dispVal.size()), g_hudBodyFormat,
                                  D2D1::RectF(valRect.left + 4.0f, curY, valRect.right - 4.0f, curY + h), brush);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                }
            }
            curY += h;
        }
        if (needsScroll && maxScroll > 0) {
            float trackTop = top + 8.0f, trackH = cardH - 16.0f, thumbH = std::max(24.0f, trackH * (cardH / totalContentH));
            float thumbTop = trackTop + (trackH - thumbH) * (static_cast<float>(scrollPx) / static_cast<float>(maxScroll));
            brush->SetOpacity(1.0f);
            brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.32f));
            rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(left + cardW - 7.0f, thumbTop, left + cardW - 3.0f, thumbTop + thumbH), 2.0f, 2.0f), brush);
        }
        rt->PopAxisAlignedClip();
    }

    { std::lock_guard<std::mutex> lock(g_hudUiMutex); g_hudControls = std::move(nextControls); }

    brush->SetOpacity(1.0f);
    brush->SetColor(cfg.colorHudBg);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(btnCx, btnCy), btnR, btnR), brush);
    brush->SetColor(isDragging
        ? D2D1::ColorF(0.35f, 0.78f, 1.0f, 0.90f)
        : D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.50f));
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(btnCx, btnCy), btnR, btnR), brush, isDragging ? 2.2f : 1.6f);

    brush->SetColor(cfg.colorHudText);
    rt->DrawTextW(labelText.c_str(), static_cast<UINT32>(labelText.size()), g_hudCircleFormat,
                  D2D1::RectF(btnCx - btnR, btnCy - btnR, btnCx + btnR, btnCy + btnR), brush);
    (void)activePrecip; (void)activeFx; (void)todLabel; (void)cloudPct01; (void)uvIndex; (void)moonPhase01;
}

void RenderFrame(const Settings& cfg, const ForecastSnapshot& snap, bool activePrecip, const GeneralEffectsState& fx,
                 const wchar_t* todLabel, float cloudPct01, float uvIndex, double moonPhase01,
                 int bottomLimitY, float precipVx, double timeSec, bool forceArcDirty) {
    if (!g_overlayHwnd || !EnsureSurface(g_surface, g_canvasWidth, g_canvasHeight, cfg.colorSnow, cfg.hasCustomSnowColor)) return;
    float bottomRelY = static_cast<float>(bottomLimitY - g_canvasTop);
    RECT bindRect = {0, 0, g_canvasWidth, g_canvasHeight};
    g_surface.rt->BindDC(g_surface.memDC, &bindRect);
    g_surface.rt->BeginDraw();
    g_surface.rt->Clear(D2D1::ColorF(0, 0, 0, 0));

    float k = static_cast<float>(cfg.effectsIntensity) / 25.0f;
    float hFloat = static_cast<float>(g_canvasHeight);
    bool isNightClock = false;
    const wchar_t* ignoredLabel = nullptr;
    double hour = GetLocalHourFloat();
    DrawTimeOfDayLayer(g_surface.rt, cfg, hour, isNightClock, ignoredLabel);
    if (fx.nightStars) DrawNightStarsLayer(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec);

    float cloudDimFactor = cfg.celestialInFrontOfClouds ? 1.0f : (1.0f - cloudPct01 * 0.65f);
    bool sunOn = (cfg.sunMode == TriStateMode::On) ||
                 (cfg.sunMode == TriStateMode::Auto && !isNightClock && (cfg.primarySource == L"off" || !snap.valid || snap.isDay));
    bool moonOn = (cfg.moonMode == TriStateMode::On) ||
                  (cfg.moonMode == TriStateMode::Auto && (isNightClock || (cfg.primarySource != L"off" && snap.valid && !snap.isDay)));

    if (!cfg.celestialInFrontOfClouds) {
        DrawSunLayer(g_surface.rt, g_surface.sharedBrush, cfg, sunOn, hour, snap, uvIndex, cloudDimFactor, timeSec);
        DrawMoonLayer(g_surface.rt, g_surface.sharedBrush, cfg, moonOn, hour, snap, moonPhase01, cloudDimFactor);
    }
    DrawDriftingCloudsLayer(g_surface.rt, g_surface.sharedBrush, cfg, cloudPct01, precipVx, timeSec);
    if (cfg.celestialInFrontOfClouds) {
        DrawSunLayer(g_surface.rt, g_surface.sharedBrush, cfg, sunOn, hour, snap, uvIndex, cloudDimFactor, timeSec);
        DrawMoonLayer(g_surface.rt, g_surface.sharedBrush, cfg, moonOn, hour, snap, moonPhase01, cloudDimFactor);
    }

    if (fx.clear) { D2D1_COLOR_F cTop = cfg.hasCustomClearColor ? cfg.colorClear : D2D1::ColorF(1.0f, 0.94f, 0.72f, 0.16f); D2D1_COLOR_F cBot = cTop; cBot.a *= 0.2f; DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k); }
    if (fx.partlyCloudy) { D2D1_COLOR_F cTop = cfg.hasCustomPartlyCloudyColor ? cfg.colorPartlyCloudy : D2D1::ColorF(0.90f, 0.93f, 0.97f, 0.18f); D2D1_COLOR_F cBot = cTop; cBot.a *= 0.35f; DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k); }
    if (fx.overcast) { D2D1_COLOR_F cTop = cfg.hasCustomOvercastColor ? cfg.colorOvercast : D2D1::ColorF(0.50f, 0.55f, 0.62f, 0.28f); D2D1_COLOR_F cBot = cTop; cBot.a *= 0.65f; DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k); }
    if (fx.dust) { D2D1_COLOR_F cTop = cfg.hasCustomDustColor ? cfg.colorDust : D2D1::ColorF(0.84f, 0.68f, 0.45f, 0.10f); D2D1_COLOR_F cBot = cfg.hasCustomDustColor ? cfg.colorDust : D2D1::ColorF(0.84f, 0.68f, 0.45f, 0.26f); DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k); }
    if (fx.fog) { D2D1_COLOR_F cTop = cfg.hasCustomFogColor ? cfg.colorFog : D2D1::ColorF(0.84f, 0.88f, 0.92f, 0.14f); D2D1_COLOR_F cBot = cfg.hasCustomFogColor ? cfg.colorFog : D2D1::ColorF(0.84f, 0.88f, 0.92f, 0.34f); DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k); }
    if (fx.smog) { D2D1_COLOR_F cTop = cfg.hasCustomSmogColor ? cfg.colorSmog : D2D1::ColorF(0.54f, 0.48f, 0.40f, 0.16f); D2D1_COLOR_F cBot = cfg.hasCustomSmogColor ? cfg.colorSmog : D2D1::ColorF(0.54f, 0.48f, 0.40f, 0.36f); DrawVerticalGradientTint(g_surface.rt, cTop, cBot, 0.0f, hFloat, k); }
    if (fx.airQuality) DrawAirQualityLayer(g_surface.rt, g_surface.sharedBrush, cfg, snap, k, timeSec);
    if (fx.frost || fx.heat) DrawThermalLayer(g_surface.rt, fx.frost, fx.heat, k);
    if (fx.aurora) DrawSmoothAurora(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec);
    if (fx.mirage) DrawSmoothMirage(g_surface.rt, g_surface.sharedBrush, cfg, k, bottomRelY, timeSec);
    if (fx.rainbow) DrawSmoothRainbow(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec, forceArcDirty);
    if (fx.windStreams) DrawWindStreamsLayer(g_surface.rt, g_surface.sharedBrush, cfg, snap, k, timeSec);
    if (fx.thunderstorm && g_lightning.timer > 0.0f) {
        D2D1_COLOR_F tc = cfg.hasCustomThunderstormColor ? cfg.colorThunderstorm : D2D1::ColorF(0.86f, 0.93f, 1.0f, 0.95f);
        float tAlpha = std::clamp(g_lightning.timer * 4.2f * std::max(0.25f, k), 0.0f, 1.0f);
        g_surface.sharedBrush->SetColor(D2D1::ColorF(tc.r, tc.g, tc.b, tAlpha));
        DrawBranchedLightning(g_surface.rt, g_surface.sharedBrush, cfg, g_lightning, bottomRelY, tAlpha);
    }
    for (const auto& p : g_falling) DrawItemShape(g_surface, cfg, p, precipVx, bottomRelY, timeSec);
    if (fx.pollen) DrawPollenLayer(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec);
    if (fx.floodWater) DrawFloodWaterLayer(g_surface.rt, g_surface.sharedBrush, cfg, snap, k, timeSec);
    DrawForecastHudWidget(g_surface.rt, g_surface.sharedBrush, cfg, snap, activePrecip, fx, todLabel, cloudPct01, uvIndex, moonPhase01, bottomLimitY);

    if (FAILED(g_surface.rt->EndDraw())) { CleanupSurface(g_surface); return; }
    PresentSurface(g_overlayHwnd, g_surface);
}

float ComputeHorizontalSpeedPxPerSec(FallDirMode dirMode, double mpsSpeed, int forecastSign, double forecastWindKmh) {
    float basePx = static_cast<float>(mpsSpeed) * 26.0f;
    if (dirMode == FallDirMode::Down) return 0.0f;
    if (dirMode == FallDirMode::DownLeft) return -basePx;
    if (dirMode == FallDirMode::DownRight) return basePx;
    if (forecastSign == 0 || forecastWindKmh <= 0.2) return 0.0f;
    return static_cast<float>(forecastSign) * basePx * std::clamp(static_cast<float>(forecastWindKmh) / 18.0f, 0.30f, 2.8f);
}

float ComputeEffectiveFallSpeedMps(const Settings& cfg, const ForecastSnapshot& snap, ObjectKind kind) {
    if (cfg.precipSpeedMode == PrecipSpeedMode::Custom) return static_cast<float>(cfg.precipFallSpeedMps);
    double rainFactor = std::clamp((snap.valid ? snap.precipMm : 1.0) / 4.0, 0.45, 2.6);
    if (kind == ObjectKind::Snow) return static_cast<float>(std::clamp(1.6 + rainFactor * 1.1, 1.2, 4.5));
    if (kind == ObjectKind::Hail) return static_cast<float>(std::clamp(11.0 + rainFactor * 5.0, 9.0, 22.0));
    return static_cast<float>(std::clamp(5.5 + rainFactor * 4.2, 4.0, 16.0));
}

void WaitWithMessagePump(DWORD ms) {
    DWORD startTick = GetTickCount();
    while (g_running.load()) {
        DWORD elapsed = GetTickCount() - startTick;
        if (elapsed >= ms) break;
        if (MsgWaitForMultipleObjects(0, nullptr, FALSE, ms - elapsed, QS_ALLINPUT) == WAIT_OBJECT_0) {
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
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OverlayWndProc), &modInstance);
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
    auto startTime = std::chrono::steady_clock::now(), lastTime = startTime;
    int zOrderTick = 0, prevLeft = INT32_MIN, prevTop = INT32_MIN, prevW = -1, prevH = -1;

    while (g_running.load()) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        Settings cfg;
        ForecastSnapshot snap;
        bool forceArcDirty = false;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            cfg = g_settings;
            snap = g_forecast;
            forceArcDirty = g_arcCacheDirty;
            g_arcCacheDirty = false;
        }
        UpdateCanvasGeometry(cfg);

        bool geometryChanged = (g_canvasLeft != prevLeft || g_canvasTop != prevTop ||
                                g_canvasWidth != prevW || g_canvasHeight != prevH);
        if (geometryChanged) {
            prevLeft = g_canvasLeft;
            prevTop = g_canvasTop;
            prevW = g_canvasWidth;
            prevH = g_canvasHeight;
            forceArcDirty = true;
        }

        if ((cfg.pauseOnFullscreen && IsFullscreenAppActive()) || IsDesktopOccluded()) {
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) ShowWindow(g_overlayHwnd, SW_HIDE);
            WaitWithMessagePump(250);
            lastTime = std::chrono::steady_clock::now();
            continue;
        }

        if (!g_overlayHwnd) {
            g_overlayHwnd = CreateWindowExW(
                WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                wc.lpszClassName, L"DesktopWeatherOverlay", WS_POPUP,
                g_canvasLeft, g_canvasTop, g_canvasWidth, g_canvasHeight,
                nullptr, nullptr, wc.hInstance, nullptr);
            PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode, true);
        } else if (geometryChanged) {
            PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode, true);
        }

        if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) {
            POINT ptCur = {};
            if (GetCursorPos(&ptCur)) {
                float relX = static_cast<float>(ptCur.x - g_canvasLeft);
                float relY = static_cast<float>(ptCur.y - g_canvasTop);
                SyncClickThroughExStyle(g_overlayHwnd, IsRelPointInsideHud(relX, relY));
            }
        }

        auto now = std::chrono::steady_clock::now();
        float dt = std::clamp(std::chrono::duration<float>(now - lastTime).count(), 0.001f, 0.05f);
        double timeSec = std::chrono::duration<double>(now - startTime).count();
        lastTime = now;

        bool activePrecip = false;
        ObjectKind precipKind = ObjectKind::Rain;
        if (cfg.precipitationMode == PrecipMode::Rain) { activePrecip = true; precipKind = ObjectKind::Rain; }
        else if (cfg.precipitationMode == PrecipMode::Snow) { activePrecip = true; precipKind = ObjectKind::Snow; }
        else if (cfg.precipitationMode == PrecipMode::Hail) { activePrecip = true; precipKind = ObjectKind::Hail; }
        else if (cfg.precipitationMode == PrecipMode::Auto && snap.hasPrecip) { activePrecip = true; precipKind = snap.precipKind; }

        auto resolveFx = [&](TriStateMode m, bool autoVal) -> bool {
            return (m == TriStateMode::On) ? true : ((m == TriStateMode::Off) ? false : autoVal);
        };
        GeneralEffectsState activeFx;
        activeFx.clear = resolveFx(cfg.effectClear, snap.effects.clear);
        activeFx.partlyCloudy = resolveFx(cfg.effectPartlyCloudy, snap.effects.partlyCloudy);
        activeFx.overcast = resolveFx(cfg.effectOvercast, snap.effects.overcast);
        activeFx.dust = resolveFx(cfg.effectDust, snap.effects.dust);
        activeFx.fog = resolveFx(cfg.effectFog, snap.effects.fog);
        activeFx.smog = resolveFx(cfg.effectSmog, snap.effects.smog);
        activeFx.thunderstorm = resolveFx(cfg.effectThunderstorm, snap.effects.thunderstorm);
        activeFx.aurora = resolveFx(cfg.effectAurora, snap.effects.aurora);
        activeFx.mirage = resolveFx(cfg.effectMirage, snap.effects.mirage);
        activeFx.rainbow = resolveFx(cfg.effectRainbow, snap.effects.rainbow);
        activeFx.windStreams = resolveFx(cfg.effectWindStreams, snap.effects.windStreams);
        activeFx.nightStars = resolveFx(cfg.effectNightStars, snap.effects.nightStars);
        activeFx.airQuality = resolveFx(cfg.effectAirQuality, snap.effects.airQuality);
        activeFx.pollen = resolveFx(cfg.effectPollen, snap.effects.pollen);
        activeFx.floodWater = resolveFx(cfg.effectFloodWater, snap.effects.floodWater);
        if (cfg.effectThermal == TriStateMode::On) {
            activeFx.frost = snap.valid && snap.tempC <= 0.0;
            activeFx.heat = !activeFx.frost;
        } else if (cfg.effectThermal == TriStateMode::Auto) {
            activeFx.frost = snap.effects.frost;
            activeFx.heat = snap.effects.heat;
        }

        double hour = GetLocalHourFloat();
        D2D1_COLOR_F d1{}, d2{};
        bool isNightClock = false;
        const wchar_t* todLabel = L"Day";
        GetSystemTimeOfDayPalette(hour, d1, d2, isNightClock, todLabel);
        if (cfg.timeOfDayMode == TimeOfDayMode::Dawn) { isNightClock = false; todLabel = L"Dawn"; }
        else if (cfg.timeOfDayMode == TimeOfDayMode::Day) { isNightClock = false; todLabel = L"Day"; }
        else if (cfg.timeOfDayMode == TimeOfDayMode::Sunset) { isNightClock = false; todLabel = L"Sunset"; }
        else if (cfg.timeOfDayMode == TimeOfDayMode::Night) { isNightClock = true; todLabel = L"Night"; }

        float cloudPct01 = 0.0f;
        if (cfg.cloudsMode == TriStateMode::On) cloudPct01 = static_cast<float>(cfg.cloudsIntensity) / 100.0f;
        else if (cfg.cloudsMode == TriStateMode::Auto) {
            if (activeFx.overcast) cloudPct01 = 0.90f;
            else if (activeFx.partlyCloudy) cloudPct01 = 0.45f;
            else if (snap.valid) cloudPct01 = static_cast<float>(snap.cloudCoverPct / 100.0);
        }
        cloudPct01 = std::clamp(cloudPct01, 0.0f, 1.0f);

        bool isDaytime = !isNightClock && (cfg.primarySource == L"off" || !snap.valid || snap.isDay);
        bool sunActive = (cfg.sunMode == TriStateMode::On) || (cfg.sunMode == TriStateMode::Auto && isDaytime);
        bool moonActive = (cfg.moonMode == TriStateMode::On) || (cfg.moonMode == TriStateMode::Auto && !isDaytime);
        float uvIndex = (cfg.sunUvOverride >= 0.0) ? static_cast<float>(cfg.sunUvOverride) : static_cast<float>(snap.uvIndex);
        double moonPhase01 = (cfg.moonPhaseOverride >= 0.0) ? cfg.moonPhaseOverride : ComputeRealMoonPhase();
        bool timeOfDayActive = (cfg.timeOfDayMode != TimeOfDayMode::Off && cfg.timeOfDayIntensity > 0);

        bool hasAnyState = activePrecip || !g_falling.empty() || cfg.forecastHudEnabled || timeOfDayActive ||
                           (cloudPct01 > 0.01f) || sunActive || moonActive ||
                           activeFx.clear || activeFx.partlyCloudy || activeFx.overcast || activeFx.dust ||
                           activeFx.fog || activeFx.smog || activeFx.thunderstorm || activeFx.aurora ||
                           activeFx.mirage || activeFx.rainbow || activeFx.windStreams || activeFx.frost ||
                           activeFx.heat || activeFx.nightStars || activeFx.airQuality || activeFx.pollen || activeFx.floodWater;

        if (!hasAnyState) {
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) ShowWindow(g_overlayHwnd, SW_HIDE);
            WaitWithMessagePump(200);
            lastTime = std::chrono::steady_clock::now();
            continue;
        }
        if (g_overlayHwnd) {
            if (!IsWindowVisible(g_overlayHwnd)) {
                ShowWindow(g_overlayHwnd, SW_SHOWNA);
                PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode, true);
            } else if (++zOrderTick >= 20) {
                zOrderTick = 0;
                PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode, false);
            }
        }

        if (activeFx.thunderstorm) {
            float flashProb = (cfg.performanceMode == PerformanceMode::High) ? 0.45f : ((cfg.performanceMode == PerformanceMode::Low) ? 0.20f : 0.35f);
            if (g_lightning.timer > 0.0f) g_lightning.timer = std::max(0.0f, g_lightning.timer - dt);
            else if ((static_cast<float>(rng() % 10000) / 10000.0f) < flashProb * dt) {
                g_lightning.timer = 0.22f;
                g_lightning.xStart = static_cast<float>(100 + rng() % std::max(120, g_canvasWidth - 200));
                g_lightning.xEnd = g_lightning.xStart + static_cast<float>((rng() % 160) - 80);
            }
        } else {
            g_lightning.timer = 0.0f;
        }

        float effSpeedMps = ComputeEffectiveFallSpeedMps(cfg, snap, precipKind);
        float precipVx = ComputeHorizontalSpeedPxPerSec(cfg.precipFallDirection, effSpeedMps, snap.windDirSign, snap.windSpeedKmh);
        float precipVy = effSpeedMps * 42.0f;
        float windCloudVx = (snap.windDirSign != 0 && snap.windSpeedKmh > 0.2)
            ? static_cast<float>(snap.windDirSign) * static_cast<float>(snap.windSpeedKmh) * 1.4f
            : precipVx;

        size_t targetP = activePrecip
            ? static_cast<size_t>((g_canvasWidth / 1000.0) * cfg.precipIntensity * 1.4 * GetQualityScale(cfg.performanceMode))
            : 0;
        size_t prevCount = g_falling.size();
        if (g_falling.size() != targetP) g_falling.resize(targetP);
        for (size_t i = 0; i < targetP; ++i) {
            if (i >= prevCount || g_falling[i].kind != precipKind) {
                SpawnFallingItem(g_falling[i], rng, precipKind, precipVx, true, cfg.performanceMode);
            }
        }

        for (size_t i = 0; i < g_falling.size(); ++i) {
            FallingItem& p = g_falling[i];
            p.swayPhase += dt * ((cfg.performanceMode == PerformanceMode::Low) ? 1.5f : 3.0f);
            p.angle += p.spin * dt * 20.0f;
            float sway = (p.kind == ObjectKind::Rain)
                ? 0.0f
                : std::sin(p.swayPhase) * ((cfg.performanceMode == PerformanceMode::Low) ? 8.0f : 18.0f);
            p.x += (precipVx + sway) * dt;
            p.y += precipVy * dt;
            if (p.y > g_canvasHeight + 30.0f || p.x < -140.0f || p.x > g_canvasWidth + 140.0f) {
                SpawnFallingItem(p, rng, p.kind, precipVx, false, cfg.performanceMode);
            }
        }

        RenderFrame(cfg, snap, activePrecip, activeFx, todLabel, cloudPct01, uvIndex, moonPhase01,
                    ComputeBottomLimitY(cfg), windCloudVx, timeSec, forceArcDirty);

        bool hasAnimatedMotion = !g_falling.empty() || activeFx.thunderstorm || activeFx.aurora || activeFx.mirage ||
                                 activeFx.windStreams || activeFx.nightStars || activeFx.airQuality ||
                                 activeFx.pollen || activeFx.floodWater || (cloudPct01 > 0.01f);
        WaitWithMessagePump(hasAnimatedMotion ? static_cast<DWORD>(1000 / cfg.targetFps) : 120);
    }

    if (g_overlayHwnd) {
        DestroyWindow(g_overlayHwnd);
        g_overlayHwnd = nullptr;
    }
    CleanupSurface(g_surface);
    CleanupDirectWrite();
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
    if (g_weatherWakeEvent) SetEvent(g_weatherWakeEvent);
    if (g_weatherThread && g_weatherThread->joinable()) g_weatherThread->join();
    g_weatherThread.reset();
    if (g_renderThread && g_renderThread->joinable()) g_renderThread->join();
    g_renderThread.reset();
    if (g_weatherWakeEvent) {
        CloseHandle(g_weatherWakeEvent);
        g_weatherWakeEvent = nullptr;
    }
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;
void WINAPI EntryPoint_Hook() { ExitThread(0); }

BOOL Wh_ModInit() {
    bool isService = false, isToolModProcess = false, isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return FALSE;
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
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
            break;
        }
    }
    LocalFree(argv);
    if (isService) return FALSE;
    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) ExitProcess(1);
        if (!WhTool_ModInit()) ExitProcess(1);
        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        Wh_SetFunctionHook(reinterpret_cast<BYTE*>(dosHeader) + ntHeaders->OptionalHeader.AddressOfEntryPoint,
                           reinterpret_cast<void*>(EntryPoint_Hook), nullptr);
        return TRUE;
    }
    if (isToolModProcess) return FALSE;
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;
    WCHAR windhawkPath[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, windhawkPath, ARRAYSIZE(windhawkPath))) return;
    WCHAR commandLine[MAX_PATH * 2];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", windhawkPath, WH_MOD_ID);
    STARTUPINFO si = {sizeof(si)};
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (CreateProcessW(windhawkPath, commandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}