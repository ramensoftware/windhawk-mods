// ==WindhawkMod==
// @id              alembiq-weather-desktop-overlay
// @name            Desktop Weather & Particles Overlay
// @description     Atmospheric effects and live weather forecast visualization for the Windows desktop (Direct2D overlay)
// @version         1.8.4.3
// @author          alembiq
// @github          https://github.com/alembiqspace
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lwininet -luser32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
A lightweight **Direct2D** weather and atmosphere overlay for the Windows desktop. It brings live weather precipitation, volumetric-style drifting clouds, a UV-reactive sun and lunar-phase moon, atmospheric layers, air quality, pollen, flood-level water, and a draggable temperature button with an interactive control & forecast table without cluttering icons or windows.

### Preview
![Desktop Weather & Particles Overlay preview](https://raw.githubusercontent.com/alembiqspace/windhawk-mods/main/screenshots/desktop-weather-overlay-preview.png)

### Features
- **Performance Quality Scaling:** `[System] Performance mode` (`low`, `medium`, `high`) scales particle counts, cloud layers, sun rays, moon detail, lightning branches, wave smoothness, rain splashes on `high`, and HUD detail.
- **Precipitation (`[Precipitation]`):** Supports `off`, `auto`, `rain`, `snow`, and `hail` with configurable fall direction, `auto` (forecast precipitation-driven speed scaled per snow/rain/hail) or `custom` fall speed, and intensity.
- **Sky (`[Sky]`):** Intensity-based drifting clouds with edge fade at monitor borders, a UV-reactive sun with soft rays, and a real-phase moon with surface shading, independent movement modes, per-monitor visibility (`primary` / `all`), and cloud occlusion toggle. Hovering the cursor over clouds, sun, or moon fades them to the configured cursor-hover opacity percentage (default `5%`) so desktop icons remain visible.
- **Effects (`[Effects]`):** Wind gusts, frost/heat tint, night stars, air quality haze/particles, floating pollen, flood-level water, and atmospheric layers (`dust`, `fog`, `smog`, `thunderstorm`, `aurora`, `mirage`, `rainbow`).
- **Custom & Playful Units:** Configure standard or playful units in settings for temperature, wind, cloud cover, precipitation, UV index, and air quality.
- **Draggable Temperature Button & Interactive Table:** Drag the circular button (shows a gear icon when no temperature forecast is available yet) or click it to open an auto-sized table with unified forecast fields, `auto` multi-provider aggregation, italic/50%-opacity styling for unsupported fields, and `(i)` hover tooltips showing source and sync time.

### Network & Privacy
- **No requests until configured:** By default `coords` is empty (`""`) and no automatic fallback coordinates are used, so no network requests are made until you enter valid `lat/lon` coordinates (or configure `customApiUrl` / `localWeatherPath`).
- **External endpoints & what is sent:**
  - `www.7timer.info` (`/bin/api.pl`): sends `lat` and `lon` coordinates.
  - `api.met.no` (`/weatherapi/locationforecast/2.0/compact`): sends `lat` and `lon` coordinates.
  - `api.open-meteo.com` (`/v1/forecast`) and `air-quality-api.open-meteo.com` (`/v1/air-quality`): sends `lat` and `lon` coordinates.
  - `api.weatherapi.com`, `api.openweathermap.org`, `weather.visualcrossing.com`, `api.tomorrow.io`: contacted only when their respective API key is configured; sends `lat`, `lon`, and the API key in the query string.
  - Custom JSON API (`customApiUrl`) / Local file (`localWeatherPath`): contacts the configured URL (substituting `{lat}`, `{lon}`, and `{key}` from `customApiKey`) or reads the local JSON file.
- **When requests happen:** At startup when configured, on the configured `updateIntervalMin` interval (default `30` min), on retry after a failed fetch, or when the forecast source, coordinates, keys, or endpoint paths change.
- **How to disable:** Leave `coords` empty (and clear `customApiUrl` / `localWeatherPath`) or set `[Weather] Primary source` to `off`.
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
- taskbarAsBorder: true
  $name: "[System] Taskbar as a border"
  $description: "When enabled in above_taskbar mode, uses the taskbar work area bottom edge as the lower screen border. Default: true."
- bottomInsetPx: 10
  $name: "[System] Bottom inset (px)"
  $description: "Bottom margin offset in pixels applied in custom_offset mode (0 to 1000). Default: 10."
- multiMonitorMode: primary
  $name: "[System] Monitors"
  $description: "Render on primary monitor or all monitors. Allowed: primary, all. Default: primary."
  $options:
    - primary: Primary monitor
    - all: All monitors
- targetFps: 60
  $name: "[System] Target FPS (30 or 60)"
  $description: "Target frame rate for active fast motion (30 or 60). Any other value resets to 60."
- pauseOnFullscreen: true
  $name: "[System] Pause on fullscreen apps"
  $description: "Pauses rendering while a fullscreen app is active. Default: true."

# --- 2. WEATHER SOURCES & UNITS ---
- primarySource: auto
  $name: "[Weather] Primary source"
  $description: "Primary forecast provider or 'auto' to aggregate across providers in autoProviderOrder. Default: auto."
  $options:
    - auto: Auto (aggregate across providers)
    - seven_timer: 7Timer! (civil)
    - met_norway: MET Norway (Yr.no)
    - open_meteo: Open-Meteo API
    - weatherapi: WeatherAPI.com (*)
    - openweathermap: OpenWeatherMap (*)
    - visual_crossing: Visual Crossing (*)
    - tomorrow_io: Tomorrow.io (*)
    - custom_api: Custom JSON URL
    - local_file: Local file
    - off: Off (disable forecast fetching)
- fallbackSource: off
  $name: "[Weather] Fallback source"
  $description: "Backup forecast source if primary fails. Default: off."
  $options:
    - off: Off
    - seven_timer: 7Timer! (civil)
    - met_norway: MET Norway (Yr.no)
    - open_meteo: Open-Meteo API
    - weatherapi: WeatherAPI.com (*)
    - openweathermap: OpenWeatherMap (*)
    - visual_crossing: Visual Crossing (*)
    - tomorrow_io: Tomorrow.io (*)
    - local_file: Local file
- autoProviderOrder: "1/2/3/4/5/6/7/8/9"
  $name: "[Weather] Auto provider order"
  $description: "Slash-separated ranks (ascending) for 1=7Timer!, 2=Yr.no, 3=Open-Meteo, 4=WeatherAPI, 5=OWM, 6=VisualCrossing, 7=Tomorrow.io, 8=CustomAPI, 9=LocalFile. Default: '1/2/3/4/5/6/7/8/9'."
- weatherapiKey: ""
  $name: "[Weather] WeatherAPI.com API key (*)"
  $description: "API key for WeatherAPI.com."
- openweathermapKey: ""
  $name: "[Weather] OpenWeatherMap API key (*)"
  $description: "API key for OpenWeatherMap."
- visualCrossingKey: ""
  $name: "[Weather] Visual Crossing API key (*)"
  $description: "API key for Visual Crossing."
- tomorrowIoKey: ""
  $name: "[Weather] Tomorrow.io API key (*)"
  $description: "API key for Tomorrow.io."
- coords: ""
  $name: "[Weather] Latitude/Longitude"
  $description: "Format: lat/lon (e.g. 44.7866/20.4489). Latitude -90..90, longitude -180..180. Empty by default (no requests until set)."
- customApiUrl: ""
  $name: "[Weather] Custom JSON API URL"
  $description: "HTTP/HTTPS JSON endpoint (supports {lat}, {lon}, and {key} from customApiKey)."
- customApiKey: ""
  $name: "[Weather] Custom JSON API key"
  $description: "Key substituted into {key} placeholder for customApiUrl."
- localWeatherPath: ""
  $name: "[Weather] Local weather file path"
  $description: "Path to local JSON weather file. Default: empty."
- updateIntervalMin: 30
  $name: "[Weather] Update interval (min)"
  $description: "Forecast refresh period in minutes (1 to 1440). Invalid/out-of-range resets to 30."
- tempUnit: celsius
  $name: "[Units] Temperature"
  $description: "Temperature unit in HUD and button."
  $options:
    - celsius: Celsius (°C)
    - fahrenheit: Fahrenheit (°F)
    - kelvin: Kelvin (K)
    - body_ratio: Body temp ratio (vs 36.6°C)
    - water_freeze: Frost delta (vs 0°C)
    - pizza_oven: Oven scale (vs 20°C)
- windUnit: kmh
  $name: "[Units] Wind speed"
  $description: "Wind speed unit in HUD."
  $options:
    - kmh: Kilometers per hour (km/h)
    - ms: Meters per second (m/s)
    - mph: Miles per hour (mph)
    - knots: Knots (kn)
    - beaufort: Beaufort scale (Bft)
    - breeze_ratio: Fresh breeze ratio (vs 25 km/h)
- cloudUnit: percent
  $name: "[Units] Cloud cover"
  $description: "Cloud cover unit in HUD."
  $options:
    - percent: Percent (%)
    - oktas: Oktas (0..8)
    - blanket_ratio: Sky blanket ratio (0..1)
    - sky_clear: Clear sky share (%)
- precipUnit: mm
  $name: "[Units] Precipitation"
  $description: "Precipitation unit in HUD."
  $options:
    - mm: Millimeters (mm)
    - inches: Inches (in)
    - microns: Microns (µm)
    - tea_spoons: Teaspoons / m² (~5 ml)
    - puddle_cm: Puddle depth (cm x10)
- uvUnit: index
  $name: "[Units] UV index"
  $description: "UV index unit in HUD."
  $options:
    - index: Standard UV index
    - percent_max: Share of UV 11 (%)
    - sunburn_units: Sunburn units (/3)
    - spf_hint: SPF load score
- aqiUnit: aqi
  $name: "[Units] Air quality"
  $description: "Air quality unit in HUD."
  $options:
    - aqi: Air Quality Index (AQI)
    - pm25: PM2.5 (µg/m³)
    - cig_equiv: Smoke units
    - lung_load: Lung load (%)

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
- hudButtonGeometry: "92/88"
  $name: "[HUD] Fallback button center X/Y (%)"
  $description: "Format: centerX/centerY in % (0..100; X left->right, Y bottom->top) when no dragged position is set. Default: 92/88."
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
- cursorHoverOpacityPct: 5
  $name: "[Sky] Opacity under cursor (%)"
  $description: "Opacity percentage (0 to 100) for clouds, sun, and moon when hovered by the cursor. Default: 5."
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
- sunMonitorMode: primary
  $name: "[Sky] Sun monitors"
  $description: "Show sun on primary monitor or all monitors. Allowed: primary, all. Default: primary."
  $options:
    - primary: Primary monitor
    - all: All monitors
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
- moonMonitorMode: primary
  $name: "[Sky] Moon monitors"
  $description: "Show moon on primary monitor or all monitors. Allowed: primary, all. Default: primary."
  $options:
    - primary: Primary monitor
    - all: All monitors
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

# --- 6. EFFECTS ---
- effectsIntensity: 25
  $name: "[Effects] General intensity (%)"
  $description: "Strength/opacity for effects (0..100). Outside 0..100 or invalid resets to 25."
- effectDust: auto
  $name: "[Effects] Dust"
  $description: "Airborne dust haze (auto triggers on strong dry wind or low visibility). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectFog: auto
  $name: "[Effects] Fog"
  $description: "Soft mist/fog layer (auto triggers on fog code or high humidity with low visibility). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectSmog: auto
  $name: "[Effects] Smog"
  $description: "Smog layer (auto triggers on calm wind with reduced visibility/haze). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectThunderstorm: auto
  $name: "[Effects] Thunderstorm"
  $description: "Storm atmosphere with branched lightning flashes. Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectAurora: auto
  $name: "[Effects] Aurora"
  $description: "Smooth animated aurora curtains in upper screen area (auto triggers at night with clear/low clouds). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectMirage: auto
  $name: "[Effects] Mirage"
  $description: "Soft horizon shimmer wave band (auto triggers on hot daytime weather). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectRainbow: auto
  $name: "[Effects] Rainbow"
  $description: "Multi-band rainbow arc (auto triggers on daytime partly-cloudy/post-rain/misty conditions). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- rainbowGeometry: "55-50/15"
  $name: "[Effects] Rainbow radius-center X/Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius 0 = off, 100 = min(width, height); X left->right, Y bottom->top). Default: 55-50/15."
- effectWindStreams: auto
  $name: "[Effects] Wind gusts & streams"
  $description: "Animated wind streaks on strong gusts (auto triggers when wind gusts >= 32 km/h or wind >= 24 km/h). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectThermal: auto
  $name: "[Effects] Frost / Heat tint"
  $description: "Subtle frost rim at <= 0 C or warm heat haze at >= 30 C apparent temperature. Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectNightStars: auto
  $name: "[Effects] Night stars"
  $description: "Twinkling stars in upper sky at night with low cloud cover. Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectAirQuality: auto
  $name: "[Effects] Air quality (AQI / PM2.5)"
  $description: "Air quality haze & micro-particles (auto triggers on elevated AQI / PM2.5). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectPollen: auto
  $name: "[Effects] Pollen particles"
  $description: "Floating pollen motes (auto triggers on elevated pollen index in air quality data). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectFloodWater: auto
  $name: "[Effects] Rising water / flood level"
  $description: "Animated water band along the bottom boundary (auto triggers on heavy rain/precipitation). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off

# --- 7. COLORS (CONFIG ONLY, '/' LISTS) ---
- skyPrecipColors: "#6BB8FF/#FBFDFF/#C2E6FF/#E6ECF7/#FFD152/#DCE7FF"
  $name: "[Sky + Precipitation] Colors (#RRGGBB/...)"
  $description: "6 HEX colors separated by '/': rain/snow/hail/clouds/sun/moon (empty segment = default)."
- effectsColors: "#D6AE73/#D1D9E0/#8F7F6B/#D6EBFF/#38EB9E/#FADBA8/#FFC759/#D1A857/#F7E36B/#2980C2"
  $name: "[Effects] Colors (#RRGGBB/...)"
  $description: "10 HEX colors separated by '/': dust/fog/smog/thunderstorm/aurora/mirage/rainbow/airQuality/pollen/floodWater (empty segment = default)."
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
enum class CelestialMotionMode {
    Fixed, LtrLine, LtrLineForecast, LtrArc, LtrArcForecast,
    RtlLine, RtlLineForecast, RtlArc, RtlArcForecast, Vertical, VerticalForecast
};
enum class CelestialMonitorMode { Primary, All };
enum class FallDirMode { Auto, Down, DownLeft, DownRight };
enum class ExpandedSection { None, Sky, Precipitation, Effects };
enum class HudFieldId {
    None, Coords, CloudsIntensity, SunUvOverride,
    MoonPhaseOverride, PrecipSpeedMps, PrecipIntensity, EffectsIntensity
};

enum class TempUnit { Celsius, Fahrenheit, Kelvin, BodyRatio, WaterFreeze, PizzaOven };
enum class WindUnit { Kmh, Ms, Mph, Knots, Beaufort, BreezeRatio };
enum class CloudUnit { Percent, Oktas, BlanketRatio, SkyClear };
enum class PrecipUnit { Mm, Inches, Microns, TeaSpoons, PuddleCm };
enum class UvUnit { Index, PercentMax, SunburnUnits, SpfHint };
enum class AqiUnit { Aqi, Pm25, CigEquiv, LungLoad };

struct GeneralEffectsState {
    bool dust = false;
    bool fog = false;
    bool smog = false;
    bool thunderstorm = false;
    bool aurora = false;
    bool mirage = false;
    bool rainbow = false;
    bool windStreams = false;
    bool frost = false;
    bool heat = false;
    bool nightStars = false;
    bool airQuality = false;
    bool pollen = false;
    bool floodWater = false;
};

struct MetricMeta {
    bool available = false;
    std::wstring sourceName;
    std::wstring syncTime;
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

struct MonitorCanvasRect {
    float left = 0.0f;
    float top = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float bottomLimitRelY = 0.0f;
};

struct ForecastSnapshot {
    bool valid = false;
    bool lastFetchFailed = false;
    int64_t lastSuccessEpochSec = 0;
    int weatherCode = -1;
    double tempC = 0.0;
    double apparentTempC = 15.0;
    double dewPointC = 8.0;
    double humidityPct = 55.0;
    double pressureHpa = 1013.25;
    double windSpeedKmh = 0.0;
    double windGustsKmh = 0.0;
    double windDirDeg = 0.0;
    int windDirSign = 0;
    double cloudCoverPct = 20.0;
    double visibilityM = 12000.0;
    double uvIndex = 2.0;
    double precipMm = 0.0;
    double aqiValue = 25.0;
    double pm25Value = 8.0;
    double pollenMaxValue = 0.0;
    bool hasSunTimes = false;
    double sunriseHour = 6.5;
    double sunsetHour = 18.0;
    bool isDay = true;
    bool hasPrecip = false;
    ObjectKind precipKind = ObjectKind::Rain;
    GeneralEffectsState effects;

    MetricMeta mCondition;
    MetricMeta mTemp;
    MetricMeta mFeelsLike;
    MetricMeta mHumidity;
    MetricMeta mPressure;
    MetricMeta mWind;
    MetricMeta mGusts;
    MetricMeta mClouds;
    MetricMeta mPrecip;
    MetricMeta mUv;
    MetricMeta mAqi;
    MetricMeta mPollen;
    MetricMeta mVisibility;
    MetricMeta mDewPoint;
    MetricMeta mDayNight;
    MetricMeta mSunCycle;

    std::wstring sunriseText = L"--:--";
    std::wstring sunsetText = L"--:--";
    std::wstring lastUpdateText = L"--:--";
    std::wstring sourceLabel = L"none";
};

struct Settings {
    std::wstring layerAttachMode = L"auto";
    PerformanceMode performanceMode = PerformanceMode::High;
    std::wstring bottomBoundaryMode = L"above_taskbar";
    bool taskbarAsBorder = true;
    int bottomInsetPx = 10;
    std::wstring multiMonitorMode = L"primary";
    int targetFps = 60;
    bool pauseOnFullscreen = true;

    std::wstring primarySource = L"auto";
    std::wstring fallbackSource = L"off";
    std::wstring autoProviderOrder = L"1/2/3/4/5/6/7/8/9";
    std::wstring weatherapiKey;
    std::wstring openweathermapKey;
    std::wstring visualCrossingKey;
    std::wstring tomorrowIoKey;
    bool hasValidCoords = false;
    std::wstring coordsRaw;
    double latitude = 0.0;
    double longitude = 0.0;
    std::wstring customApiUrl;
    std::wstring customApiKey;
    std::wstring localWeatherPath;
    int updateIntervalMin = 30;

    TempUnit tempUnit = TempUnit::Celsius;
    WindUnit windUnit = WindUnit::Kmh;
    CloudUnit cloudUnit = CloudUnit::Percent;
    PrecipUnit precipUnit = PrecipUnit::Mm;
    UvUnit uvUnit = UvUnit::Index;
    AqiUnit aqiUnit = AqiUnit::Aqi;

    bool forecastHudEnabled = false;
    float hudButtonRadiusPx = 24.0f;
    float hudFontSizePx = 12.0f;
    ArcGeometry hudButtonGeom{24, 92, 88};
    bool hasCustomBtnPos = false;
    double hudBtnRelX = 0.92;
    double hudBtnRelY = 0.12;
    int forecastHudMarginPx = 14;
    int forecastHudOpacity = 84;

    int cursorHoverOpacityPct = 5;

    TriStateMode cloudsMode = TriStateMode::Auto;
    int cloudsIntensity = 35;
    TriStateMode sunMode = TriStateMode::Auto;
    CelestialMonitorMode sunMonitorMode = CelestialMonitorMode::Primary;
    CelestialMotionMode sunMotionMode = CelestialMotionMode::RtlArc;
    ArcGeometry sunGeom{7, 82, 82};
    double sunUvOverride = -1.0;
    TriStateMode moonMode = TriStateMode::Auto;
    CelestialMonitorMode moonMonitorMode = CelestialMonitorMode::Primary;
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
    TriStateMode effectDust = TriStateMode::Auto;
    TriStateMode effectFog = TriStateMode::Auto;
    TriStateMode effectSmog = TriStateMode::Auto;
    TriStateMode effectThunderstorm = TriStateMode::Auto;
    TriStateMode effectAurora = TriStateMode::Auto;
    TriStateMode effectMirage = TriStateMode::Auto;
    TriStateMode effectRainbow = TriStateMode::Auto;
    TriStateMode effectWindStreams = TriStateMode::Auto;
    TriStateMode effectThermal = TriStateMode::Auto;
    TriStateMode effectNightStars = TriStateMode::Auto;
    TriStateMode effectAirQuality = TriStateMode::Auto;
    TriStateMode effectPollen = TriStateMode::Auto;
    TriStateMode effectFloodWater = TriStateMode::Auto;
    ArcGeometry rainbowGeom{55, 50, 15};

    bool hasCustomSnowColor = false;
    D2D1_COLOR_F colorRain = D2D1::ColorF(0.42f, 0.72f, 1.0f, 0.78f);
    D2D1_COLOR_F colorSnow = D2D1::ColorF(0.98f, 0.99f, 1.0f, 0.96f);
    D2D1_COLOR_F colorHail = D2D1::ColorF(0.76f, 0.90f, 1.0f, 0.95f);

    bool hasCustomCloudsColor = false;
    D2D1_COLOR_F colorClouds = D2D1::ColorF(0.90f, 0.93f, 0.97f, 0.34f);
    bool hasCustomSunColor = false;
    D2D1_COLOR_F colorSun = D2D1::ColorF(1.0f, 0.82f, 0.32f, 0.88f);
    bool hasCustomMoonColor = false;
    D2D1_COLOR_F colorMoon = D2D1::ColorF(0.86f, 0.91f, 1.0f, 0.94f);

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
    bool hasCustomAirQualityColor = false;
    D2D1_COLOR_F colorAirQuality = D2D1::ColorF(0.78f, 0.68f, 0.38f, 0.24f);
    bool hasCustomPollenColor = false;
    D2D1_COLOR_F colorPollen = D2D1::ColorF(0.96f, 0.88f, 0.42f, 0.72f);
    bool hasCustomFloodWaterColor = false;
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
    std::wstring tooltipText;
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
    int depthTier = 1;
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
ForecastSnapshot g_forecast;
bool g_forecastDirty = true;
std::wstring g_lastFetchedSource;
double g_lastFetchedLat = 0.0;
double g_lastFetchedLon = 0.0;

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
bool g_draggingHudBtn = false;
bool g_movedDuringDrag = false;
int g_dragStartScreenX = 0;
int g_dragStartScreenY = 0;
float g_dragStartBtnCx = 0.0f;
float g_dragStartBtnCy = 0.0f;
int g_hudScrollPx = 0;
int g_hudMaxScrollPx = 0;
ExpandedSection g_expandedSection = ExpandedSection::None;
HudFieldId g_focusedField = HudFieldId::None;
std::wstring g_editBuffer;
float g_hoverRelX = -1000.0f;
float g_hoverRelY = -1000.0f;
std::vector<InteractiveControl> g_hudControls;

ID2D1Factory* g_d2dFactory = nullptr;
IDWriteFactory* g_dwriteFactory = nullptr;
IDWriteTextFormat* g_hudTitleFormat = nullptr;
IDWriteTextFormat* g_hudBodyFormat = nullptr;
IDWriteTextFormat* g_hudItalicFormat = nullptr;
IDWriteTextFormat* g_hudCircleFormat = nullptr;
IDWriteTextFormat* g_hudPillFormat = nullptr;
float g_cachedFontSizePx = 0.0f;
float g_cachedCircleFontPx = 0.0f;
CachedSurface g_surface;
CachedArcLayout g_rainbowCache;

int g_canvasLeft = 0;
int g_canvasTop = 0;
int g_canvasWidth = 0;
int g_canvasHeight = 0;
std::vector<MonitorCanvasRect> g_monitorRects;
std::vector<FallingItem> g_falling;
LightningFlash g_lightning;

float GetQualityScale(PerformanceMode mode) {
    if (mode == PerformanceMode::Low) {
        return 0.55f;
    }
    if (mode == PerformanceMode::High) {
        return 1.35f;
    }
    return 1.0f;
}

bool HasAnyForecastConsumer(const Settings& cfg) {
    if (cfg.primarySource == L"off") {
        return false;
    }
    if (cfg.primarySource == L"custom_api") {
        return !cfg.customApiUrl.empty();
    }
    if (cfg.primarySource == L"local_file") {
        return !cfg.localWeatherPath.empty();
    }
    return cfg.hasValidCoords;
}

bool NeedsAirQualityFetch(const Settings& cfg) {
    return cfg.hasValidCoords &&
           (cfg.effectAirQuality == TriStateMode::Auto ||
            cfg.effectPollen == TriStateMode::Auto ||
            cfg.forecastHudEnabled);
}

void RefreshArcCache(CachedArcLayout& cache, const ArcGeometry& geom, bool forceDirty) {
    if (!forceDirty &&
        cache.valid &&
        cache.canvasW == g_canvasWidth &&
        cache.canvasH == g_canvasHeight &&
        cache.pctRadius == geom.radius &&
        cache.pctX == geom.centerX &&
        cache.pctY == geom.centerY) {
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
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::wstring ReplaceAllSubstrings(std::wstring s, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) {
        return s;
    }
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::wstring::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

std::wstring FormatCoordDot(double v) {
    wchar_t buf[32] = {};
    swprintf_s(buf, L"%.4f", v);
    std::wstring s(buf);
    for (wchar_t& c : s) {
        if (c == L',') {
            c = L'.';
        }
    }
    return s;
}

std::wstring HourToHhMm(double hour) {
    double hNorm = std::fmod(hour, 24.0);
    if (hNorm < 0.0) {
        hNorm += 24.0;
    }
    int hh = static_cast<int>(hNorm) % 24;
    int mm = static_cast<int>(std::llround((hNorm - hh) * 60.0)) % 60;
    wchar_t buf[16] = {};
    swprintf_s(buf, L"%02d:%02d", hh, mm);
    return buf;
}

std::string ExtractJsonStringField(const std::string& json, size_t scope, const std::string& key) {
    size_t pos = json.find("\"" + key + "\"", scope);
    if (pos == std::string::npos) {
        return {};
    }
    pos = json.find(':', pos);
    if (pos == std::string::npos) {
        return {};
    }
    size_t q1 = json.find('"', pos);
    if (q1 == std::string::npos) {
        return {};
    }
    size_t q2 = json.find('"', q1 + 1);
    if (q2 == std::string::npos) {
        return {};
    }
    return json.substr(q1 + 1, q2 - q1 - 1);
}

bool TryParseDoubleStrict(const std::wstring& raw, double& outVal) {
    std::wstring s = Trim(raw);
    if (s.empty()) {
        return false;
    }
    for (wchar_t& c : s) {
        if (c == L',') {
            c = L'.';
        }
    }
    wchar_t* endPtr = nullptr;
    errno = 0;
    double v = std::wcstod(s.c_str(), &endPtr);
    if (errno != 0 || endPtr == s.c_str()) {
        return false;
    }
    while (*endPtr != L'\0') {
        if (!iswspace(*endPtr)) {
            return false;
        }
        ++endPtr;
    }
    if (!std::isfinite(v)) {
        return false;
    }
    outVal = v;
    return true;
}

int ParseIntRangeOrDefault(int rawVal, int minVal, int maxVal, int defVal) {
    if (rawVal < minVal || rawVal > maxVal) {
        return defVal;
    }
    return rawVal;
}

double ParseDoubleRangeOrDefault(const std::wstring& raw, double minVal, double maxVal, double defVal) {
    double v = 0.0;
    if (!TryParseDoubleStrict(raw, v) || v < minVal || v > maxVal) {
        return defVal;
    }
    return v;
}

int SanitizePercent(int rawVal) {
    return ParseIntRangeOrDefault(rawVal, 0, 100, 25);
}

bool TryParseCoordsString(const std::wstring& raw, double& outLat, double& outLon) {
    std::wstring s = Trim(raw);
    if (s.empty()) {
        return false;
    }
    size_t sep = s.find(L'/');
    if (sep == std::wstring::npos) {
        sep = s.find(L',');
    }
    if (sep == std::wstring::npos) {
        return false;
    }
    double lat = 0.0;
    double lon = 0.0;
    if (!TryParseDoubleStrict(s.substr(0, sep), lat) ||
        !TryParseDoubleStrict(s.substr(sep + 1), lon) ||
        lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
        return false;
    }
    outLat = lat;
    outLon = lon;
    return true;
}

void ParseHudButtonGeometryString(const std::wstring& raw, ArcGeometry defGeom, ArcGeometry& outGeom) {
    outGeom = defGeom;
    std::wstring s = Trim(raw);
    size_t dash = s.find(L'-');
    size_t slash = s.find(L'/');
    if (slash == std::wstring::npos) {
        return;
    }
    std::wstring xPart = (dash != std::wstring::npos && slash > dash)
        ? Trim(s.substr(dash + 1, slash - dash - 1))
        : Trim(s.substr(0, slash));
    std::wstring yPart = Trim(s.substr(slash + 1));

    auto isIntStr = [](const std::wstring& p) -> bool {
        if (p.empty()) {
            return false;
        }
        for (wchar_t c : p) {
            if (!iswdigit(c)) {
                return false;
            }
        }
        return true;
    };
    if (!isIntStr(xPart) || !isIntStr(yPart)) {
        return;
    }
    try {
        int x = std::stoi(xPart);
        int y = std::stoi(yPart);
        if (x < 0 || x > 100 || y < 0 || y > 100) {
            return;
        }
        outGeom = {defGeom.radius, x, y};
    } catch (...) {
    }
}

void ParseArcGeometryString(const std::wstring& raw, ArcGeometry defGeom, ArcGeometry& outGeom) {
    outGeom = defGeom;
    std::wstring s = Trim(raw);
    size_t dash = s.find(L'-');
    size_t slash = s.find(L'/');
    if (dash == std::wstring::npos || slash == std::wstring::npos || slash <= dash) {
        return;
    }

    std::wstring rPart = Trim(s.substr(0, dash));
    std::wstring xPart = Trim(s.substr(dash + 1, slash - dash - 1));
    std::wstring yPart = Trim(s.substr(slash + 1));

    auto isIntStr = [](const std::wstring& p) -> bool {
        if (p.empty()) {
            return false;
        }
        for (wchar_t c : p) {
            if (!iswdigit(c)) {
                return false;
            }
        }
        return true;
    };
    if (!isIntStr(rPart) || !isIntStr(xPart) || !isIntStr(yPart)) {
        return;
    }

    try {
        int r = std::stoi(rPart);
        int x = std::stoi(xPart);
        int y = std::stoi(yPart);
        if (r < 0 || r > 100 || x < 0 || x > 100 || y < 0 || y > 100) {
            return;
        }
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

std::vector<std::wstring> BuildOrderedAutoProviders(const std::wstring& orderStr) {
    const std::wstring defaultProviders[9] = {
        L"seven_timer", L"met_norway", L"open_meteo", L"weatherapi",
        L"openweathermap", L"visual_crossing", L"tomorrow_io",
        L"custom_api", L"local_file"
    };
    std::vector<std::wstring> tokens = SplitSlashTokens(orderStr);
    if (tokens.size() != 9) {
        return std::vector<std::wstring>(defaultProviders, defaultProviders + 9);
    }

    struct RankEntry {
        int rank = 0;
        int idx = 0;
    };
    std::vector<RankEntry> entries;
    entries.reserve(9);
    for (int i = 0; i < 9; ++i) {
        double v = 0.0;
        if (!TryParseDoubleStrict(tokens[i], v)) {
            return std::vector<std::wstring>(defaultProviders, defaultProviders + 9);
        }
        entries.push_back(RankEntry{static_cast<int>(std::llround(v)), i});
    }
    std::stable_sort(entries.begin(), entries.end(), [](const RankEntry& a, const RankEntry& b) {
        return a.rank < b.rank;
    });

    std::vector<std::wstring> ordered;
    ordered.reserve(9);
    for (const auto& e : entries) {
        ordered.push_back(defaultProviders[e.idx]);
    }
    return ordered;
}

bool ProviderRequiresApiKey(const std::wstring& src) {
    return src == L"weatherapi" ||
           src == L"openweathermap" ||
           src == L"visual_crossing" ||
           src == L"tomorrow_io";
}

std::wstring GetProviderApiKey(const std::wstring& src, const Settings& cfg) {
    if (src == L"weatherapi") {
        return cfg.weatherapiKey;
    }
    if (src == L"openweathermap") {
        return cfg.openweathermapKey;
    }
    if (src == L"visual_crossing") {
        return cfg.visualCrossingKey;
    }
    if (src == L"tomorrow_io") {
        return cfg.tomorrowIoKey;
    }
    return L"";
}

std::wstring FormatProviderHudBadge(const std::wstring& src) {
    return ProviderRequiresApiKey(src) ? (src + L"*") : src;
}

PerformanceMode ParsePerformanceMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"low") {
        return PerformanceMode::Low;
    }
    if (v == L"medium") {
        return PerformanceMode::Medium;
    }
    return PerformanceMode::High;
}

PrecipMode ParsePrecipMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"off") {
        return PrecipMode::Off;
    }
    if (v == L"rain") {
        return PrecipMode::Rain;
    }
    if (v == L"snow") {
        return PrecipMode::Snow;
    }
    if (v == L"hail") {
        return PrecipMode::Hail;
    }
    return PrecipMode::Auto;
}

PrecipSpeedMode ParsePrecipSpeedMode(const std::wstring& s) {
    return ToLower(Trim(s)) == L"custom" ? PrecipSpeedMode::Custom : PrecipSpeedMode::Auto;
}

TriStateMode ParseTriState(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"on") {
        return TriStateMode::On;
    }
    if (v == L"off") {
        return TriStateMode::Off;
    }
    return TriStateMode::Auto;
}

CelestialMonitorMode ParseCelestialMonitorMode(const std::wstring& s) {
    return ToLower(Trim(s)) == L"all" ? CelestialMonitorMode::All : CelestialMonitorMode::Primary;
}

CelestialMotionMode ParseCelestialMotion(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"fixed") {
        return CelestialMotionMode::Fixed;
    }
    if (v == L"ltr_line") {
        return CelestialMotionMode::LtrLine;
    }
    if (v == L"ltr_line_forecast") {
        return CelestialMotionMode::LtrLineForecast;
    }
    if (v == L"ltr_arc") {
        return CelestialMotionMode::LtrArc;
    }
    if (v == L"ltr_arc_forecast") {
        return CelestialMotionMode::LtrArcForecast;
    }
    if (v == L"rtl_line") {
        return CelestialMotionMode::RtlLine;
    }
    if (v == L"rtl_line_forecast") {
        return CelestialMotionMode::RtlLineForecast;
    }
    if (v == L"rtl_arc") {
        return CelestialMotionMode::RtlArc;
    }
    if (v == L"rtl_arc_forecast") {
        return CelestialMotionMode::RtlArcForecast;
    }
    if (v == L"vertical") {
        return CelestialMotionMode::Vertical;
    }
    if (v == L"vertical_forecast") {
        return CelestialMotionMode::VerticalForecast;
    }
    return CelestialMotionMode::RtlArc;
}

FallDirMode ParseFallDir(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"down") {
        return FallDirMode::Down;
    }
    if (v == L"down_left" || v == L"left") {
        return FallDirMode::DownLeft;
    }
    if (v == L"down_right" || v == L"right") {
        return FallDirMode::DownRight;
    }
    return FallDirMode::Auto;
}

TempUnit ParseTempUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"fahrenheit") {
        return TempUnit::Fahrenheit;
    }
    if (v == L"kelvin") {
        return TempUnit::Kelvin;
    }
    if (v == L"body_ratio") {
        return TempUnit::BodyRatio;
    }
    if (v == L"water_freeze") {
        return TempUnit::WaterFreeze;
    }
    if (v == L"pizza_oven") {
        return TempUnit::PizzaOven;
    }
    return TempUnit::Celsius;
}

WindUnit ParseWindUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"ms") {
        return WindUnit::Ms;
    }
    if (v == L"mph") {
        return WindUnit::Mph;
    }
    if (v == L"knots") {
        return WindUnit::Knots;
    }
    if (v == L"beaufort") {
        return WindUnit::Beaufort;
    }
    if (v == L"breeze_ratio") {
        return WindUnit::BreezeRatio;
    }
    return WindUnit::Kmh;
}

CloudUnit ParseCloudUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"oktas") {
        return CloudUnit::Oktas;
    }
    if (v == L"blanket_ratio") {
        return CloudUnit::BlanketRatio;
    }
    if (v == L"sky_clear") {
        return CloudUnit::SkyClear;
    }
    return CloudUnit::Percent;
}

PrecipUnit ParsePrecipUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"inches") {
        return PrecipUnit::Inches;
    }
    if (v == L"microns") {
        return PrecipUnit::Microns;
    }
    if (v == L"tea_spoons") {
        return PrecipUnit::TeaSpoons;
    }
    if (v == L"puddle_cm") {
        return PrecipUnit::PuddleCm;
    }
    return PrecipUnit::Mm;
}

UvUnit ParseUvUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"percent_max") {
        return UvUnit::PercentMax;
    }
    if (v == L"sunburn_units") {
        return UvUnit::SunburnUnits;
    }
    if (v == L"spf_hint") {
        return UvUnit::SpfHint;
    }
    return UvUnit::Index;
}

AqiUnit ParseAqiUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"pm25") {
        return AqiUnit::Pm25;
    }
    if (v == L"cig_equiv") {
        return AqiUnit::CigEquiv;
    }
    if (v == L"lung_load") {
        return AqiUnit::LungLoad;
    }
    return AqiUnit::Aqi;
}

std::wstring FormatTempValue(double tempC, TempUnit unit, bool shortButton) {
    wchar_t buf[64] = {};
    if (unit == TempUnit::Fahrenheit) {
        double f = tempC * 1.8 + 32.0;
        if (shortButton) {
            swprintf_s(buf, L"%d°F", static_cast<int>(std::llround(f)));
        } else {
            swprintf_s(buf, L"%.1f °F", f);
        }
        return buf;
    }
    if (unit == TempUnit::Kelvin) {
        double k = tempC + 273.15;
        if (shortButton) {
            swprintf_s(buf, L"%dK", static_cast<int>(std::llround(k)));
        } else {
            swprintf_s(buf, L"%.1f K", k);
        }
        return buf;
    }
    if (unit == TempUnit::BodyRatio) {
        double r = tempC / 36.6;
        if (shortButton) {
            swprintf_s(buf, L"%.2f×", r);
        } else {
            swprintf_s(buf, L"%.2f body (36.6°)", r);
        }
        return buf;
    }
    if (unit == TempUnit::WaterFreeze) {
        if (shortButton) {
            swprintf_s(buf, L"%+d°", static_cast<int>(std::llround(tempC)));
        } else {
            swprintf_s(buf, L"%+.1f vs 0°C", tempC);
        }
        return buf;
    }
    if (unit == TempUnit::PizzaOven) {
        double ov = tempC / 20.0;
        if (shortButton) {
            swprintf_s(buf, L"%.1f", ov);
        } else {
            swprintf_s(buf, L"%.2f warm-up", ov);
        }
        return buf;
    }
    if (shortButton) {
        swprintf_s(buf, L"%d°", static_cast<int>(std::llround(tempC)));
    } else {
        swprintf_s(buf, L"%.1f °C", tempC);
    }
    return buf;
}

std::wstring FormatWindValue(double windKmh, WindUnit unit) {
    wchar_t buf[96] = {};
    if (unit == WindUnit::Ms) {
        swprintf_s(buf, L"%.1f m/s", windKmh / 3.6);
    } else if (unit == WindUnit::Mph) {
        swprintf_s(buf, L"%.1f mph", windKmh * 0.621371);
    } else if (unit == WindUnit::Knots) {
        swprintf_s(buf, L"%.1f kn", windKmh * 0.539957);
    } else if (unit == WindUnit::Beaufort) {
        double vMs = windKmh / 3.6;
        int bft = (vMs < 0.3) ? 0 : ((vMs < 1.6) ? 1 : ((vMs < 3.4) ? 2 : ((vMs < 5.5) ? 3 : ((vMs < 8.0) ? 4 : ((vMs < 10.8) ? 5 : ((vMs < 13.9) ? 6 : 7))))));
        swprintf_s(buf, L"Bft %d", bft);
    } else if (unit == WindUnit::BreezeRatio) {
        swprintf_s(buf, L"%.2f breeze", windKmh / 25.0);
    } else {
        swprintf_s(buf, L"%.1f km/h", windKmh);
    }
    return buf;
}

std::wstring FormatCloudValue(double cloudPct, CloudUnit unit) {
    wchar_t buf[64] = {};
    if (unit == CloudUnit::Oktas) {
        swprintf_s(buf, L"%.1f/8 okta", std::clamp(cloudPct / 12.5, 0.0, 8.0));
    } else if (unit == CloudUnit::BlanketRatio) {
        swprintf_s(buf, L"%.2f blanket", std::clamp(cloudPct / 100.0, 0.0, 1.0));
    } else if (unit == CloudUnit::SkyClear) {
        swprintf_s(buf, L"%.0f%% clear", std::clamp(100.0 - cloudPct, 0.0, 100.0));
    } else {
        swprintf_s(buf, L"%.0f%%", std::clamp(cloudPct, 0.0, 100.0));
    }
    return buf;
}

std::wstring FormatPrecipValue(double mm, PrecipUnit unit) {
    wchar_t buf[64] = {};
    if (unit == PrecipUnit::Inches) {
        swprintf_s(buf, L"%.2f in", mm / 25.4);
    } else if (unit == PrecipUnit::Microns) {
        swprintf_s(buf, L"%.0f µm", mm * 1000.0);
    } else if (unit == PrecipUnit::TeaSpoons) {
        swprintf_s(buf, L"%.1f tsp/m²", mm / 5.0);
    } else if (unit == PrecipUnit::PuddleCm) {
        swprintf_s(buf, L"%.2f cm", mm * 0.1);
    } else {
        swprintf_s(buf, L"%.1f mm", mm);
    }
    return buf;
}

std::wstring FormatUvValue(double uv, UvUnit unit) {
    wchar_t buf[64] = {};
    if (unit == UvUnit::PercentMax) {
        swprintf_s(buf, L"%.0f%% max", (uv / 11.0) * 100.0);
    } else if (unit == UvUnit::SunburnUnits) {
        swprintf_s(buf, L"%.2f burn", uv / 3.0);
    } else if (unit == UvUnit::SpfHint) {
        swprintf_s(buf, L"SPF %d+", std::max(15, static_cast<int>(std::llround(uv * 5.0))));
    } else {
        swprintf_s(buf, L"%.1f", uv);
    }
    return buf;
}

std::wstring FormatAqiValue(double aqi, double pm25, AqiUnit unit) {
    wchar_t buf[64] = {};
    if (unit == AqiUnit::Pm25) {
        swprintf_s(buf, L"%.1f µg/m³", pm25);
    } else if (unit == AqiUnit::CigEquiv) {
        swprintf_s(buf, L"%.2f smoke", pm25 / 22.0);
    } else if (unit == AqiUnit::LungLoad) {
        swprintf_s(buf, L"%.0f%% load", std::clamp((aqi / 150.0) * 100.0, 0.0, 300.0));
    } else {
        swprintf_s(buf, L"%.0f AQI", aqi);
    }
    return buf;
}

int WindDegToSign(double deg) {
    double norm = std::fmod(deg, 360.0);
    if (norm < 0.0) {
        norm += 360.0;
    }
    return (norm >= 45.0 && norm < 225.0) ? 1 : -1;
}

const wchar_t* WindDegToCompass(double deg) {
    double norm = std::fmod(deg, 360.0);
    if (norm < 0.0) {
        norm += 360.0;
    }
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

int64_t GetNowEpochSec() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

double ComputeRealMoonPhase() {
    int64_t nowSec = GetNowEpochSec();
    double daysSinceNew = (static_cast<double>(nowSec) - kReferenceNewMoonEpochSec) / 86400.0;
    double cycle = std::fmod(daysSinceNew, kSynodicMonthDays);
    if (cycle < 0.0) {
        cycle += kSynodicMonthDays;
    }
    return std::clamp(cycle / kSynodicMonthDays, 0.0, 1.0);
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
    if (pos == std::string::npos) {
        return false;
    }
    size_t arrStart = json.find('[', pos);
    if (arrStart == std::string::npos) {
        return false;
    }
    size_t q1 = json.find('"', arrStart);
    if (q1 == std::string::npos) {
        return false;
    }
    size_t q2 = json.find('"', q1 + 1);
    if (q2 == std::string::npos) {
        return false;
    }
    std::string iso = json.substr(q1 + 1, q2 - q1 - 1);
    size_t tPos = iso.find('T');
    if (tPos == std::string::npos || tPos + 5 >= iso.size()) {
        return false;
    }
    int hh = std::atoi(iso.substr(tPos + 1, 2).c_str());
    int mm = std::atoi(iso.substr(tPos + 4, 2).c_str());
    if (hh < 0 || hh > 23 || mm < 0 || mm > 59) {
        return false;
    }
    outHour = static_cast<double>(hh) + static_cast<double>(mm) / 60.0;
    return true;
}

float ComputeMonitorEdgeFade(const MonitorCanvasRect& mon, float cx, float cy, float padPx) {
    float fadeDist = std::max(24.0f, padPx);
    float minEdge = std::min({
        cx - mon.left,
        (mon.left + mon.width) - cx,
        cy - mon.top,
        (mon.top + mon.height) - cy
    });
    if (minEdge <= 0.0f) {
        return 0.0f;
    }
    if (minEdge >= fadeDist) {
        return 1.0f;
    }
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
    if (duration <= 0.5) {
        duration += 24.0;
    }
    double elapsed = hour - startH;
    if (elapsed < 0.0) {
        elapsed += 24.0;
    }
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
    if (s.empty()) {
        return false;
    }
    if (s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() != 6) {
        return false;
    }
    for (wchar_t c : s) {
        if (!iswxdigit(c)) {
            return false;
        }
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
    if (parts.size() > 0) cfg.hasCustomDustColor = TryParseHexColorStrict(parts[0], 0.28f, cfg.colorDust);
    if (parts.size() > 1) cfg.hasCustomFogColor = TryParseHexColorStrict(parts[1], 0.35f, cfg.colorFog);
    if (parts.size() > 2) cfg.hasCustomSmogColor = TryParseHexColorStrict(parts[2], 0.36f, cfg.colorSmog);
    if (parts.size() > 3) cfg.hasCustomThunderstormColor = TryParseHexColorStrict(parts[3], 0.95f, cfg.colorThunderstorm);
    if (parts.size() > 4) cfg.hasCustomAuroraColor = TryParseHexColorStrict(parts[4], 0.32f, cfg.colorAurora);
    if (parts.size() > 5) cfg.hasCustomMirageColor = TryParseHexColorStrict(parts[5], 0.25f, cfg.colorMirage);
    if (parts.size() > 6) cfg.hasCustomRainbowColor = TryParseHexColorStrict(parts[6], 0.35f, cfg.colorRainbow);
    if (parts.size() > 7) cfg.hasCustomAirQualityColor = TryParseHexColorStrict(parts[7], 0.24f, cfg.colorAirQuality);
    if (parts.size() > 8) cfg.hasCustomPollenColor = TryParseHexColorStrict(parts[8], 0.72f, cfg.colorPollen);
    if (parts.size() > 9) cfg.hasCustomFloodWaterColor = TryParseHexColorStrict(parts[9], 0.42f, cfg.colorFloodWater);
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
    next.taskbarAsBorder = Wh_GetIntSetting(L"taskbarAsBorder") != 0;
    next.bottomInsetPx = ParseIntRangeOrDefault(Wh_GetIntSetting(L"bottomInsetPx"), 0, 1000, 10);
    next.multiMonitorMode = ToLower(Trim(readStr(L"multiMonitorMode", L"primary")));
    int fpsRaw = Wh_GetIntSetting(L"targetFps");
    next.targetFps = (fpsRaw == 30 || fpsRaw == 60) ? fpsRaw : 60;
    next.pauseOnFullscreen = Wh_GetIntSetting(L"pauseOnFullscreen") != 0;

    next.primarySource = ToLower(Trim(readStr(L"primarySource", L"auto")));
    next.fallbackSource = ToLower(Trim(readStr(L"fallbackSource", L"off")));
    next.autoProviderOrder = Trim(readStr(L"autoProviderOrder", L"1/2/3/4/5/6/7/8/9"));
    next.weatherapiKey = Trim(readStr(L"weatherapiKey", L""));
    next.openweathermapKey = Trim(readStr(L"openweathermapKey", L""));
    next.visualCrossingKey = Trim(readStr(L"visualCrossingKey", L""));
    next.tomorrowIoKey = Trim(readStr(L"tomorrowIoKey", L""));
    next.coordsRaw = Trim(readStr(L"coords", L""));
    next.hasValidCoords = TryParseCoordsString(next.coordsRaw, next.latitude, next.longitude);
    if (!next.hasValidCoords) {
        next.latitude = 0.0;
        next.longitude = 0.0;
    }
    next.customApiUrl = Trim(readStr(L"customApiUrl", L""));
    next.customApiKey = Trim(readStr(L"customApiKey", L""));
    next.localWeatherPath = Trim(readStr(L"localWeatherPath", L""));
    next.updateIntervalMin = ParseIntRangeOrDefault(Wh_GetIntSetting(L"updateIntervalMin"), 1, 1440, 30);

    next.tempUnit = ParseTempUnit(readStr(L"tempUnit", L"celsius"));
    next.windUnit = ParseWindUnit(readStr(L"windUnit", L"kmh"));
    next.cloudUnit = ParseCloudUnit(readStr(L"cloudUnit", L"percent"));
    next.precipUnit = ParsePrecipUnit(readStr(L"precipUnit", L"mm"));
    next.uvUnit = ParseUvUnit(readStr(L"uvUnit", L"index"));
    next.aqiUnit = ParseAqiUnit(readStr(L"aqiUnit", L"aqi"));

    next.forecastHudEnabled = ToLower(Trim(readStr(L"forecastHudMode", L"off"))) == L"on";
    next.hudButtonRadiusPx = static_cast<float>(ParseIntRangeOrDefault(Wh_GetIntSetting(L"hudButtonRadiusPx"), 14, 96, 24));
    next.hudFontSizePx = static_cast<float>(ParseIntRangeOrDefault(Wh_GetIntSetting(L"hudFontSizePx"), 9, 22, 12));
    ParseHudButtonGeometryString(readStr(L"hudButtonGeometry", L"92/88"), ArcGeometry{24, 92, 88}, next.hudButtonGeom);

    std::wstring savedBtnPos = readStr(L"hudButtonPos", L"");
    size_t slashPos = savedBtnPos.find(L'/');
    if (slashPos != std::wstring::npos) {
        double rx = 0.0;
        double ry = 0.0;
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

    next.cursorHoverOpacityPct = ParseIntRangeOrDefault(Wh_GetIntSetting(L"cursorHoverOpacityPct"), 0, 100, 5);

    next.cloudsMode = ParseTriState(readStr(L"cloudsMode", L"auto"));
    next.cloudsIntensity = ParseIntRangeOrDefault(Wh_GetIntSetting(L"cloudsIntensity"), 0, 100, 35);
    next.sunMode = ParseTriState(readStr(L"sunMode", L"auto"));
    next.sunMonitorMode = ParseCelestialMonitorMode(readStr(L"sunMonitorMode", L"primary"));
    next.sunMotionMode = ParseCelestialMotion(readStr(L"sunMotionMode", L"rtl_arc"));
    ParseArcGeometryString(readStr(L"sunGeometry", L"7-82/82"), ArcGeometry{7, 82, 82}, next.sunGeom);
    double uvVal = -1.0;
    next.sunUvOverride = (TryParseDoubleStrict(readStr(L"sunUvOverride", L"-1"), uvVal) &&
                          ((uvVal >= 0.0 && uvVal <= 20.0) || std::abs(uvVal + 1.0) < 1e-6)) ? uvVal : -1.0;
    next.moonMode = ParseTriState(readStr(L"moonMode", L"auto"));
    next.moonMonitorMode = ParseCelestialMonitorMode(readStr(L"moonMonitorMode", L"primary"));
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
    ApplyEffectsColors(readStr(L"effectsColors", L"#D6AE73/#D1D9E0/#8F7F6B/#D6EBFF/#38EB9E/#FADBA8/#FFC759/#D1A857/#F7E36B/#2980C2"), next);

    float bgAlpha = std::clamp(static_cast<float>(next.forecastHudOpacity) / 100.0f, 0.15f, 1.0f);
    if (!TryParseHexColorStrict(readStr(L"colorHudBg", L"#141A26"), bgAlpha, next.colorHudBg)) {
        next.colorHudBg = D2D1::ColorF(0.08f, 0.10f, 0.15f, bgAlpha);
    }
    TryParseHexColorStrict(readStr(L"colorHudText", L"#F0F5FC"), 0.96f, next.colorHudText);

    bool wakeWeather = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        bool coordsChanged = next.hasValidCoords &&
                             (!g_settings.hasValidCoords ||
                              std::abs(next.latitude - g_lastFetchedLat) > 1e-6 ||
                              std::abs(next.longitude - g_lastFetchedLon) > 1e-6);
        bool feedConfigChanged = (next.primarySource != g_lastFetchedSource) ||
                                 (next.fallbackSource != g_settings.fallbackSource) ||
                                 (next.autoProviderOrder != g_settings.autoProviderOrder) ||
                                 (next.customApiUrl != g_settings.customApiUrl) ||
                                 (next.customApiKey != g_settings.customApiKey) ||
                                 (next.localWeatherPath != g_settings.localWeatherPath) ||
                                 (next.weatherapiKey != g_settings.weatherapiKey) ||
                                 (next.openweathermapKey != g_settings.openweathermapKey) ||
                                 (next.visualCrossingKey != g_settings.visualCrossingKey) ||
                                 (next.tomorrowIoKey != g_settings.tomorrowIoKey);
        if (!HasAnyForecastConsumer(next)) {
            g_forecast = ForecastSnapshot{};
        }
        if (coordsChanged || feedConfigChanged) {
            g_forecastDirty = true;
            wakeWeather = true;
        }
        g_settings = next;
        g_arcCacheDirty = true;
    }
    if (wakeWeather && g_weatherWakeEvent) {
        SetEvent(g_weatherWakeEvent);
    }
}

void ApplyRuntimeButtonPosition(float cx, float cy) {
    if (g_canvasWidth <= 1 || g_canvasHeight <= 1) {
        return;
    }
    double rx = std::clamp(static_cast<double>(cx) / static_cast<double>(g_canvasWidth), 0.02, 0.98);
    double ry = std::clamp(static_cast<double>(cy) / static_cast<double>(g_canvasHeight), 0.02, 0.98);
    std::lock_guard<std::mutex> lock(g_mutex);
    g_settings.hasCustomBtnPos = true;
    g_settings.hudBtnRelX = rx;
    g_settings.hudBtnRelY = ry;
}

void CommitEditedFieldValue(HudFieldId fieldId, const std::wstring& val) {
    std::wstring trimmed = Trim(val);
    bool wakeWeather = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        switch (fieldId) {
            case HudFieldId::Coords: {
                double newLat = 0.0;
                double newLon = 0.0;
                bool validNow = TryParseCoordsString(trimmed, newLat, newLon);
                bool coordsChanged = validNow &&
                                     (!g_settings.hasValidCoords ||
                                      std::abs(newLat - g_lastFetchedLat) > 1e-6 ||
                                      std::abs(newLon - g_lastFetchedLon) > 1e-6);
                g_settings.coordsRaw = trimmed;
                g_settings.hasValidCoords = validNow;
                if (validNow) {
                    g_settings.latitude = newLat;
                    g_settings.longitude = newLon;
                } else if (!HasAnyForecastConsumer(g_settings)) {
                    g_forecast = ForecastSnapshot{};
                }
                if (coordsChanged) {
                    g_forecastDirty = true;
                    wakeWeather = true;
                }
                break;
            }
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
    if (wakeWeather && g_weatherWakeEvent) {
        SetEvent(g_weatherWakeEvent);
    }
}

void ToggleExclusiveSection(ExpandedSection sec) {
    std::lock_guard<std::mutex> lock(g_hudUiMutex);
    g_expandedSection = (g_expandedSection == sec) ? ExpandedSection::None : sec;
    g_hudScrollPx = 0;
}

void CycleSettingOption(const std::wstring& actionId) {
    if (actionId == L"sec_sky") {
        ToggleExclusiveSection(ExpandedSection::Sky);
        return;
    }
    if (actionId == L"sec_precip") {
        ToggleExclusiveSection(ExpandedSection::Precipitation);
        return;
    }
    if (actionId == L"sec_effects") {
        ToggleExclusiveSection(ExpandedSection::Effects);
        return;
    }

    auto nextTri = [](TriStateMode m) -> TriStateMode {
        if (m == TriStateMode::Auto) return TriStateMode::On;
        if (m == TriStateMode::On) return TriStateMode::Off;
        return TriStateMode::Auto;
    };

    bool wakeWeather = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        Settings& cur = g_settings;
        if (actionId == L"cycle_perf") {
            cur.performanceMode = (cur.performanceMode == PerformanceMode::Low)
                ? PerformanceMode::Medium
                : ((cur.performanceMode == PerformanceMode::Medium) ? PerformanceMode::High : PerformanceMode::Low);
        } else if (actionId == L"cycle_source") {
            const std::wstring order[] = {
                L"auto", L"seven_timer", L"met_norway", L"open_meteo", L"weatherapi",
                L"openweathermap", L"visual_crossing", L"tomorrow_io",
                L"custom_api", L"local_file", L"off"
            };
            constexpr size_t kOrderLen = sizeof(order) / sizeof(order[0]);
            size_t idx = 0;
            for (size_t i = 0; i < kOrderLen; ++i) {
                if (cur.primarySource == order[i]) {
                    idx = (i + 1) % kOrderLen;
                    break;
                }
            }
            cur.primarySource = order[idx];
            g_forecastDirty = true;
            wakeWeather = true;
        } else if (actionId == L"cycle_clouds") {
            cur.cloudsMode = nextTri(cur.cloudsMode);
        } else if (actionId == L"cycle_sun") {
            cur.sunMode = nextTri(cur.sunMode);
        } else if (actionId == L"cycle_moon") {
            cur.moonMode = nextTri(cur.moonMode);
        } else if (actionId == L"toggle_celestial_front") {
            cur.celestialInFrontOfClouds = !cur.celestialInFrontOfClouds;
        } else if (actionId == L"cycle_precip_mode") {
            cur.precipitationMode = (cur.precipitationMode == PrecipMode::Auto)
                ? PrecipMode::Rain
                : ((cur.precipitationMode == PrecipMode::Rain)
                    ? PrecipMode::Snow
                    : ((cur.precipitationMode == PrecipMode::Snow)
                        ? PrecipMode::Hail
                        : ((cur.precipitationMode == PrecipMode::Hail) ? PrecipMode::Off : PrecipMode::Auto)));
        } else if (actionId == L"cycle_precip_dir") {
            cur.precipFallDirection = (cur.precipFallDirection == FallDirMode::Auto)
                ? FallDirMode::Down
                : ((cur.precipFallDirection == FallDirMode::Down)
                    ? FallDirMode::DownLeft
                    : ((cur.precipFallDirection == FallDirMode::DownLeft) ? FallDirMode::DownRight : FallDirMode::Auto));
        } else if (actionId == L"cycle_precip_speed") {
            cur.precipSpeedMode = (cur.precipSpeedMode == PrecipSpeedMode::Auto)
                ? PrecipSpeedMode::Custom
                : PrecipSpeedMode::Auto;
        } else if (actionId == L"cycle_fx_dust") {
            cur.effectDust = nextTri(cur.effectDust);
        } else if (actionId == L"cycle_fx_fog") {
            cur.effectFog = nextTri(cur.effectFog);
        } else if (actionId == L"cycle_fx_smog") {
            cur.effectSmog = nextTri(cur.effectSmog);
        } else if (actionId == L"cycle_fx_storm") {
            cur.effectThunderstorm = nextTri(cur.effectThunderstorm);
        } else if (actionId == L"cycle_fx_aurora") {
            cur.effectAurora = nextTri(cur.effectAurora);
        } else if (actionId == L"cycle_fx_mirage") {
            cur.effectMirage = nextTri(cur.effectMirage);
        } else if (actionId == L"cycle_fx_rainbow") {
            cur.effectRainbow = nextTri(cur.effectRainbow);
        } else if (actionId == L"cycle_fx_wind") {
            cur.effectWindStreams = nextTri(cur.effectWindStreams);
        } else if (actionId == L"cycle_fx_thermal") {
            cur.effectThermal = nextTri(cur.effectThermal);
        } else if (actionId == L"cycle_fx_stars") {
            cur.effectNightStars = nextTri(cur.effectNightStars);
        } else if (actionId == L"cycle_fx_aq") {
            cur.effectAirQuality = nextTri(cur.effectAirQuality);
        } else if (actionId == L"cycle_fx_pollen") {
            cur.effectPollen = nextTri(cur.effectPollen);
        } else if (actionId == L"cycle_fx_flood") {
            cur.effectFloodWater = nextTri(cur.effectFloodWater);
        }
        g_arcCacheDirty = true;
    }
    if (wakeWeather && g_weatherWakeEvent) {
        SetEvent(g_weatherWakeEvent);
    }
}

std::string HttpGetUtf8(const std::wstring& url,
                        const std::wstring& userAgent = L"WindhawkWeatherOverlay/2.9") {
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

    HINTERNET hSession = InternetOpenW(userAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hSession) {
        Wh_Log(L"InternetOpenW failed: %u", GetLastError());
        return {};
    }

    DWORD timeoutMs = 8000;
    InternetSetOptionW(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionW(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));

    DWORD flags = INTERNET_FLAG_RELOAD |
                  INTERNET_FLAG_NO_CACHE_WRITE |
                  ((parts.nScheme == INTERNET_SCHEME_HTTPS) ? INTERNET_FLAG_SECURE : 0);

    HINTERNET hConnect = InternetConnectW(hSession, host, parts.nPort, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        Wh_Log(L"InternetConnectW failed: %u (%s)", GetLastError(), host);
        InternetCloseHandle(hSession);
        return {};
    }

    const wchar_t* acceptTypes[] = {L"application/json", L"*/*", nullptr};
    HINTERNET hRequest = HttpOpenRequestW(hConnect, L"GET", path, nullptr, nullptr, acceptTypes, flags, 0);
    std::string response;
    if (hRequest) {
        const wchar_t* extraHeaders = L"Accept: application/json\r\n";
        if (HttpSendRequestW(hRequest, extraHeaders, static_cast<DWORD>(wcslen(extraHeaders)), nullptr, 0)) {
            DWORD statusCode = 0;
            DWORD statusLen = sizeof(statusCode);
            if (HttpQueryInfoW(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusLen, nullptr) &&
                statusCode == 200) {
                char buf[4096];
                DWORD bytesRead = 0;
                while (InternetReadFile(hRequest, buf, sizeof(buf), &bytesRead) && bytesRead > 0) {
                    response.append(buf, bytesRead);
                }
                Wh_Log(L"HTTP GET 200 OK (%zu bytes) from %s", response.size(), host);
            } else {
                Wh_Log(L"HTTP GET unexpected status: %u (err=%u) from %s", statusCode, GetLastError(), host);
            }
        } else {
            Wh_Log(L"HttpSendRequestW failed: %u for %s", GetLastError(), host);
        }
    }

    if (hRequest) {
        InternetCloseHandle(hRequest);
    }
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

void StampMetricMeta(MetricMeta& meta, bool available, const std::wstring& srcLabel, const std::wstring& syncTime) {
    if (available) {
        meta.available = true;
        meta.sourceName = srcLabel;
        meta.syncTime = syncTime;
    }
}

void StampForecastUpdateTime(ForecastSnapshot& snap) {
    SYSTEMTIME st = {};
    GetLocalTime(&st);
    wchar_t timeBuf[32] = {};
    swprintf_s(timeBuf, L"%02u:%02u", st.wHour, st.wMinute);
    snap.lastUpdateText = timeBuf;
    snap.lastSuccessEpochSec = GetNowEpochSec();
    snap.lastFetchFailed = false;
    if (snap.hasSunTimes) {
        snap.sunriseText = HourToHhMm(snap.sunriseHour);
        snap.sunsetText = HourToHhMm(snap.sunsetHour);
    }
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

    if (isFogCode || (humidityPct >= 92.0 && visibilityM > 0.0 && visibilityM < 3500.0)) {
        fx.fog = true;
    }

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
    if (isDay && !hasPrecip && !isStormCode && (tempC >= 28.0 || apparentTempC >= 30.0) && cloudCover <= 35) {
        fx.mirage = true;
    }
    if (isDay && !isStormCode && (isRainCode || isFogCode || (cloudCover >= 15 && cloudCover <= 65 && !hasPrecip))) {
        fx.rainbow = true;
    }
    if (!isDay && !hasPrecip && !isStormCode && cloudCover <= 35) {
        fx.aurora = true;
        fx.nightStars = true;
    } else if (!isDay && !hasPrecip && cloudCover <= 25) {
        fx.nightStars = true;
    }
    if (windGustsKmh >= 32.0 || windKmh >= 24.0) {
        fx.windStreams = true;
    }
    if (tempC <= 0.0) {
        fx.frost = true;
    } else if (isDay && apparentTempC >= 30.0) {
        fx.heat = true;
    }
    if (aqiVal >= 50.0 || pm25Val >= 15.0) {
        fx.airQuality = true;
    }
    if (pollenMax >= 25.0) {
        fx.pollen = true;
    }
    if (precipMm >= 2.5 || (isRainCode && precipMm >= 1.2)) {
        fx.floodWater = true;
    }
}

void FinalizeParsedForecast(ForecastSnapshot& snap, const std::wstring& srcLabel) {
    if (snap.precipMm <= 0.0 && snap.weatherCode > 0) {
        snap.precipMm = EstimatePrecipMmFromWmo(snap.weatherCode);
    }
    snap.windDirSign = (snap.windSpeedKmh > 0.2) ? WindDegToSign(snap.windDirDeg) : 0;
    ApplyForecastMetrics(
        snap.weatherCode, snap.windSpeedKmh, snap.windGustsKmh,
        snap.tempC, snap.apparentTempC, snap.humidityPct,
        static_cast<int>(snap.cloudCoverPct), snap.visibilityM,
        snap.precipMm, snap.aqiValue, snap.pm25Value,
        snap.pollenMaxValue, snap.isDay, snap.hasPrecip,
        snap.precipKind, snap.effects);
    snap.valid = snap.mTemp.available || snap.mCondition.available;
    snap.sourceLabel = srcLabel;
    StampForecastUpdateTime(snap);

    const std::wstring& t = snap.lastUpdateText;
    if (snap.mCondition.available) StampMetricMeta(snap.mCondition, true, srcLabel, t);
    if (snap.mTemp.available) StampMetricMeta(snap.mTemp, true, srcLabel, t);
    if (snap.mFeelsLike.available) StampMetricMeta(snap.mFeelsLike, true, srcLabel, t);
    if (snap.mHumidity.available) StampMetricMeta(snap.mHumidity, true, srcLabel, t);
    if (snap.mPressure.available) StampMetricMeta(snap.mPressure, true, srcLabel, t);
    if (snap.mWind.available) StampMetricMeta(snap.mWind, true, srcLabel, t);
    if (snap.mGusts.available) StampMetricMeta(snap.mGusts, true, srcLabel, t);
    if (snap.mClouds.available) StampMetricMeta(snap.mClouds, true, srcLabel, t);
    if (snap.mPrecip.available) StampMetricMeta(snap.mPrecip, true, srcLabel, t);
    if (snap.mUv.available) StampMetricMeta(snap.mUv, true, srcLabel, t);
    if (snap.mAqi.available) StampMetricMeta(snap.mAqi, true, srcLabel, t);
    if (snap.mPollen.available) StampMetricMeta(snap.mPollen, true, srcLabel, t);
    if (snap.mVisibility.available) StampMetricMeta(snap.mVisibility, true, srcLabel, t);
    if (snap.mDewPoint.available) StampMetricMeta(snap.mDewPoint, true, srcLabel, t);
    if (snap.mDayNight.available) StampMetricMeta(snap.mDayNight, true, srcLabel, t);
    if (snap.mSunCycle.available) StampMetricMeta(snap.mSunCycle, true, srcLabel, t);
}

bool ExtractNumberScoped(const std::string& json, size_t scope, const std::string& key, double& val) {
    size_t pos = json.find("\"" + key + "\"", scope);
    if (pos == std::string::npos) {
        return false;
    }
    pos = json.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }
    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) {
        pos++;
    }
    if (pos >= json.size()) {
        return false;
    }
    char* endPtr = nullptr;
    errno = 0;
    double parsed = std::strtod(json.c_str() + pos, &endPtr);
    if (errno != 0 || endPtr == json.c_str() + pos || !std::isfinite(parsed)) {
        return false;
    }
    val = parsed;
    return true;
}

bool ExtractNumberAnyScope(const std::string& json, size_t primaryScope, const std::string& key, double& val) {
    if (ExtractNumberScoped(json, primaryScope, key, val)) {
        return true;
    }
    return ExtractNumberScoped(json, 0, key, val);
}

void ParseAirQualityJson(const std::string& json, ForecastSnapshot& outSnap) {
    if (json.empty()) {
        return;
    }
    size_t scope = json.find("\"current\":");
    if (scope == std::string::npos) {
        scope = 0;
    }
    double v = 0.0;
    if (ExtractNumberAnyScope(json, scope, "european_aqi", v) || ExtractNumberAnyScope(json, scope, "us_aqi", v)) {
        outSnap.aqiValue = std::max(0.0, v);
        outSnap.mAqi.available = true;
    }
    if (ExtractNumberAnyScope(json, scope, "pm2_5", v)) {
        outSnap.pm25Value = std::max(0.0, v);
        outSnap.mAqi.available = true;
    }
    double maxPollen = 0.0;
    bool anyPollen = false;
    const char* pollenKeys[] = {
        "alder_pollen", "birch_pollen", "grass_pollen",
        "mugwort_pollen", "olive_pollen", "ragweed_pollen"
    };
    for (const char* pk : pollenKeys) {
        double pv = 0.0;
        if (ExtractNumberAnyScope(json, scope, pk, pv)) {
            anyPollen = true;
            if (pv > maxPollen) {
                maxPollen = pv;
            }
        }
    }
    if (anyPollen) {
        outSnap.pollenMaxValue = maxPollen;
        outSnap.mPollen.available = true;
    }
}

bool ParseWeatherJson(const std::string& json, ForecastSnapshot& outSnap, const std::wstring& srcLabel) {
    size_t scope = json.find("\"current\":");
    if (scope == std::string::npos) {
        scope = json.find("\"current_weather\":");
    }
    if (scope == std::string::npos) {
        scope = 0;
    }

    double spd = 0.0;
    if (ExtractNumberAnyScope(json, scope, "wind_speed_10m", spd) ||
        ExtractNumberAnyScope(json, scope, "windspeed", spd) ||
        ExtractNumberAnyScope(json, scope, "wind_speed", spd)) {
        outSnap.windSpeedKmh = std::max(0.0, spd);
        outSnap.mWind.available = true;
    }
    double gusts = 0.0;
    if (ExtractNumberAnyScope(json, scope, "wind_gusts_10m", gusts) ||
        ExtractNumberAnyScope(json, scope, "wind_gusts", gusts)) {
        outSnap.windGustsKmh = std::max(0.0, gusts);
        outSnap.mGusts.available = true;
    } else {
        outSnap.windGustsKmh = outSnap.windSpeedKmh;
    }
    double deg = -1.0;
    if (ExtractNumberAnyScope(json, scope, "wind_direction_10m", deg) ||
        ExtractNumberAnyScope(json, scope, "winddirection", deg) ||
        ExtractNumberAnyScope(json, scope, "wind_deg", deg)) {
        outSnap.windDirDeg = deg;
    }

    double tempC = 15.0;
    if (ExtractNumberAnyScope(json, scope, "temperature_2m", tempC) ||
        ExtractNumberAnyScope(json, scope, "temperature", tempC)) {
        outSnap.tempC = tempC;
        outSnap.mTemp.available = true;
    }
    double appTemp = outSnap.tempC;
    if (ExtractNumberAnyScope(json, scope, "apparent_temperature", appTemp)) {
        outSnap.apparentTempC = appTemp;
        outSnap.mFeelsLike.available = true;
    } else {
        outSnap.apparentTempC = outSnap.tempC;
    }
    double dewC = 8.0;
    if (ExtractNumberAnyScope(json, scope, "dew_point_2m", dewC) ||
        ExtractNumberAnyScope(json, scope, "dewpoint", dewC)) {
        outSnap.dewPointC = dewC;
        outSnap.mDewPoint.available = true;
    }
    double hum = 55.0;
    if (ExtractNumberAnyScope(json, scope, "relative_humidity_2m", hum) ||
        ExtractNumberAnyScope(json, scope, "humidity", hum)) {
        outSnap.humidityPct = std::clamp(hum, 0.0, 100.0);
        outSnap.mHumidity.available = true;
    }
    double press = 1013.25;
    if (ExtractNumberAnyScope(json, scope, "pressure_msl", press) ||
        ExtractNumberAnyScope(json, scope, "surface_pressure", press) ||
        ExtractNumberAnyScope(json, scope, "pressure", press)) {
        outSnap.pressureHpa = press;
        outSnap.mPressure.available = true;
    }
    double cloud = 20.0;
    if (ExtractNumberAnyScope(json, scope, "cloud_cover", cloud)) {
        outSnap.cloudCoverPct = std::clamp(cloud, 0.0, 100.0);
        outSnap.mClouds.available = true;
    }
    double vis = 12000.0;
    if (ExtractNumberAnyScope(json, scope, "visibility", vis)) {
        outSnap.visibilityM = std::max(0.0, vis);
        outSnap.mVisibility.available = true;
    }
    double isDayNum = 1.0;
    if (ExtractNumberAnyScope(json, scope, "is_day", isDayNum)) {
        outSnap.isDay = (isDayNum > 0.5);
        outSnap.mDayNight.available = true;
    }
    double uv = 2.0;
    if (ExtractNumberAnyScope(json, scope, "uv_index", uv) ||
        ExtractNumberAnyScope(json, scope, "uv", uv)) {
        outSnap.uvIndex = std::max(0.0, uv);
        outSnap.mUv.available = true;
    }

    double sr = 6.5;
    double ss = 18.0;
    if (TryExtractIsoTimeHour(json, "sunrise", sr) && TryExtractIsoTimeHour(json, "sunset", ss)) {
        outSnap.hasSunTimes = true;
        outSnap.sunriseHour = sr;
        outSnap.sunsetHour = ss;
        outSnap.mSunCycle.available = true;
    }
    double aqiNum = 0.0;
    if (ExtractNumberAnyScope(json, scope, "european_aqi", aqiNum) ||
        ExtractNumberAnyScope(json, scope, "us_aqi", aqiNum)) {
        outSnap.aqiValue = std::max(0.0, aqiNum);
        outSnap.mAqi.available = true;
    }
    double pmNum = 0.0;
    if (ExtractNumberAnyScope(json, scope, "pm2_5", pmNum)) {
        outSnap.pm25Value = std::max(0.0, pmNum);
        outSnap.mAqi.available = true;
    }

    double wmo = -999.0;
    bool hasCode = ExtractNumberAnyScope(json, scope, "weather_code", wmo) ||
                   ExtractNumberAnyScope(json, scope, "weathercode", wmo);
    if (hasCode || outSnap.mTemp.available) {
        outSnap.mCondition.available = hasCode;
        outSnap.weatherCode = hasCode ? static_cast<int>(wmo) : 0;
        double mm = 0.0;
        if (ExtractNumberAnyScope(json, scope, "precipitation", mm) ||
            ExtractNumberAnyScope(json, scope, "rain", mm)) {
            outSnap.precipMm = std::max(0.0, mm);
            outSnap.mPrecip.available = true;
        } else {
            outSnap.precipMm = EstimatePrecipMmFromWmo(outSnap.weatherCode);
            outSnap.mPrecip.available = hasCode;
        }
        FinalizeParsedForecast(outSnap, srcLabel);
        return true;
    }
    return false;
}

int MetNorwaySymbolToWmo(const std::string& sym) {
    if (sym.find("thunder") != std::string::npos) return 95;
    if (sym.find("snow") != std::string::npos) return 71;
    if (sym.find("sleet") != std::string::npos) return 66;
    if (sym.find("heavyrain") != std::string::npos) return 65;
    if (sym.find("rain") != std::string::npos) return 61;
    if (sym.find("fog") != std::string::npos) return 45;
    if (sym.find("cloudy") != std::string::npos && sym.find("partly") == std::string::npos) return 3;
    if (sym.find("partlycloudy") != std::string::npos || sym.find("fair") != std::string::npos) return 2;
    if (sym.find("clearsky") != std::string::npos) return 0;
    return 1;
}

bool ParseMetNorwayJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t instantPos = json.find("\"instant\":");
    if (instantPos == std::string::npos) {
        return false;
    }
    size_t detailsPos = json.find("\"details\":", instantPos);
    if (detailsPos == std::string::npos) {
        return false;
    }

    double tempC = 0.0;
    if (!ExtractNumberScoped(json, detailsPos, "air_temperature", tempC)) {
        return false;
    }
    outSnap.tempC = tempC;
    outSnap.apparentTempC = tempC;
    outSnap.mTemp.available = true;

    double press = 1013.25;
    if (ExtractNumberScoped(json, detailsPos, "air_pressure_at_sea_level", press)) {
        outSnap.pressureHpa = press;
        outSnap.mPressure.available = true;
    }
    double cloud = 20.0;
    if (ExtractNumberScoped(json, detailsPos, "cloud_area_fraction", cloud)) {
        outSnap.cloudCoverPct = std::clamp(cloud, 0.0, 100.0);
        outSnap.mClouds.available = true;
    }
    double hum = 55.0;
    if (ExtractNumberScoped(json, detailsPos, "relative_humidity", hum)) {
        outSnap.humidityPct = std::clamp(hum, 0.0, 100.0);
        outSnap.mHumidity.available = true;
    }
    double windMs = 0.0;
    if (ExtractNumberScoped(json, detailsPos, "wind_speed", windMs)) {
        outSnap.windSpeedKmh = std::max(0.0, windMs * 3.6);
        outSnap.windGustsKmh = outSnap.windSpeedKmh;
        outSnap.mWind.available = true;
    }
    double windDeg = 0.0;
    if (ExtractNumberScoped(json, detailsPos, "wind_from_direction", windDeg)) {
        outSnap.windDirDeg = windDeg;
    }

    std::string symCode = ExtractJsonStringField(json, detailsPos, "symbol_code");
    size_t next1h = json.find("\"next_1_hours\":", detailsPos);
    if (symCode.empty() && next1h != std::string::npos) {
        symCode = ExtractJsonStringField(json, next1h, "symbol_code");
    }
    if (!symCode.empty()) {
        outSnap.weatherCode = MetNorwaySymbolToWmo(symCode);
        outSnap.mCondition.available = true;
    } else {
        outSnap.weatherCode = (outSnap.cloudCoverPct >= 80.0) ? 3 : ((outSnap.cloudCoverPct >= 25.0) ? 2 : 0);
        outSnap.mCondition.available = outSnap.mClouds.available;
    }

    double mm = 0.0;
    if (ExtractNumberScoped(json, detailsPos, "precipitation_amount", mm) ||
        (next1h != std::string::npos && ExtractNumberScoped(json, next1h, "precipitation_amount", mm))) {
        outSnap.precipMm = std::max(0.0, mm);
        outSnap.mPrecip.available = true;
    }

    double hr = GetLocalHourFloat();
    outSnap.isDay = (hr >= 6.0 && hr < 19.0);
    FinalizeParsedForecast(outSnap, L"Yr.no");
    return true;
}

double SevenTimerDirToDeg(const std::string& dir) {
    if (dir == "N") return 0.0;
    if (dir == "NE") return 45.0;
    if (dir == "E") return 90.0;
    if (dir == "SE") return 135.0;
    if (dir == "S") return 180.0;
    if (dir == "SW") return 225.0;
    if (dir == "W") return 270.0;
    if (dir == "NW") return 315.0;
    return 0.0;
}

double SevenTimerWindSpeedCatToKmh(int cat) {
    if (cat <= 1) return 2.0;
    if (cat == 2) return 8.0;
    if (cat == 3) return 18.0;
    if (cat == 4) return 30.0;
    if (cat == 5) return 45.0;
    if (cat == 6) return 62.0;
    if (cat == 7) return 80.0;
    return 100.0;
}

bool ParseSevenTimerJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t seriesPos = json.find("\"dataseries\"");
    if (seriesPos == std::string::npos) {
        return false;
    }
    size_t arrStart = json.find('[', seriesPos);
    if (arrStart == std::string::npos) {
        return false;
    }

    int initUtcHour = 0;
    std::string initStr = ExtractJsonStringField(json, 0, "init");
    if (initStr.size() >= 10) {
        initUtcHour = std::atoi(initStr.substr(8, 2).c_str());
    }
    SYSTEMTIME stUtc = {};
    GetSystemTime(&stUtc);
    double elapsedHours = static_cast<double>(stUtc.wHour - initUtcHour) + static_cast<double>(stUtc.wMinute) / 60.0;
    if (elapsedHours < 0.0) {
        elapsedHours += 24.0;
    }

    size_t bestStart = std::string::npos;
    size_t bestEnd = std::string::npos;
    double bestDiff = 1e9;
    int braceDepth = 0;
    bool inString = false;
    bool escaped = false;
    size_t itemStart = std::string::npos;

    for (size_t i = arrStart + 1; i < json.size(); ++i) {
        char c = json[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '"') {
            inString = !inString;
            continue;
        }
        if (inString) {
            continue;
        }
        if (c == '{') {
            if (braceDepth == 0) {
                itemStart = i;
            }
            braceDepth++;
        } else if (c == '}') {
            braceDepth--;
            if (braceDepth == 0 && itemStart != std::string::npos) {
                std::string elem = json.substr(itemStart, i - itemStart + 1);
                double tp = 3.0;
                if (ExtractNumberScoped(elem, 0, "timepoint", tp)) {
                    double diff = std::abs(tp - elapsedHours);
                    if (diff < bestDiff) {
                        bestDiff = diff;
                        bestStart = itemStart;
                        bestEnd = i + 1;
                    }
                } else if (bestStart == std::string::npos) {
                    bestStart = itemStart;
                    bestEnd = i + 1;
                }
                itemStart = std::string::npos;
            }
        } else if (c == ']' && braceDepth == 0) {
            break;
        }
    }

    if (bestStart == std::string::npos || bestEnd <= bestStart) {
        return false;
    }

    std::string bestItem = json.substr(bestStart, bestEnd - bestStart);
    double tempC = 0.0;
    if (!ExtractNumberScoped(bestItem, 0, "temp2m", tempC)) {
        return false;
    }
    outSnap.tempC = tempC;
    outSnap.apparentTempC = tempC;
    outSnap.mTemp.available = true;

    double cloudCat = 2.0;
    if (ExtractNumberScoped(bestItem, 0, "cloudcover", cloudCat)) {
        outSnap.cloudCoverPct = std::clamp(((cloudCat - 1.0) / 8.0) * 100.0, 0.0, 100.0);
        outSnap.mClouds.available = true;
    }

    size_t windObj = bestItem.find("\"wind10m\"");
    if (windObj != std::string::npos) {
        double spdCat = 2.0;
        if (ExtractNumberScoped(bestItem, windObj, "speed", spdCat)) {
            outSnap.windSpeedKmh = SevenTimerWindSpeedCatToKmh(static_cast<int>(spdCat));
            outSnap.windGustsKmh = outSnap.windSpeedKmh;
            outSnap.mWind.available = true;
        }
        std::string dirStr = ExtractJsonStringField(bestItem, windObj, "direction");
        outSnap.windDirDeg = SevenTimerDirToDeg(dirStr);
    }

    std::string weatherStr = ExtractJsonStringField(bestItem, 0, "weather");
    outSnap.mCondition.available = !weatherStr.empty();
    if (weatherStr.find("ts") != std::string::npos) {
        outSnap.weatherCode = 95;
    } else if (weatherStr.find("snow") != std::string::npos) {
        outSnap.weatherCode = 71;
    } else if (weatherStr.find("rain") != std::string::npos || weatherStr.find("shower") != std::string::npos) {
        outSnap.weatherCode = 61;
    } else if (weatherStr.find("cloudy") != std::string::npos) {
        outSnap.weatherCode = 3;
    } else if (weatherStr.find("pcloudy") != std::string::npos || weatherStr.find("mcloudy") != std::string::npos) {
        outSnap.weatherCode = 2;
    } else {
        outSnap.weatherCode = 0;
    }
    outSnap.isDay = (weatherStr.find("night") == std::string::npos);
    outSnap.mDayNight.available = !weatherStr.empty();
    FinalizeParsedForecast(outSnap, L"7Timer!");
    return true;
}

bool ParseWeatherApiJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t curPos = json.find("\"current\":");
    if (curPos == std::string::npos) {
        return false;
    }
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, curPos, "temp_c", tempC)) {
        return false;
    }
    outSnap.tempC = tempC;
    outSnap.mTemp.available = true;

    double appC = tempC;
    if (ExtractNumberScoped(json, curPos, "feelslike_c", appC)) {
        outSnap.apparentTempC = appC;
        outSnap.mFeelsLike.available = true;
    } else {
        outSnap.apparentTempC = tempC;
    }
    double dewC = 8.0;
    if (ExtractNumberScoped(json, curPos, "dewpoint_c", dewC)) {
        outSnap.dewPointC = dewC;
        outSnap.mDewPoint.available = true;
    }
    double press = 1013.25;
    if (ExtractNumberScoped(json, curPos, "pressure_mb", press)) {
        outSnap.pressureHpa = press;
        outSnap.mPressure.available = true;
    }
    double windKmh = 0.0;
    if (ExtractNumberScoped(json, curPos, "wind_kph", windKmh)) {
        outSnap.windSpeedKmh = std::max(0.0, windKmh);
        outSnap.mWind.available = true;
    }
    double gustKmh = outSnap.windSpeedKmh;
    if (ExtractNumberScoped(json, curPos, "gust_kph", gustKmh)) {
        outSnap.windGustsKmh = std::max(0.0, gustKmh);
        outSnap.mGusts.available = true;
    } else {
        outSnap.windGustsKmh = outSnap.windSpeedKmh;
    }
    double deg = 0.0;
    if (ExtractNumberScoped(json, curPos, "wind_degree", deg)) {
        outSnap.windDirDeg = deg;
    }
    double hum = 55.0;
    if (ExtractNumberScoped(json, curPos, "humidity", hum)) {
        outSnap.humidityPct = std::clamp(hum, 0.0, 100.0);
        outSnap.mHumidity.available = true;
    }
    double cloud = 20.0;
    if (ExtractNumberScoped(json, curPos, "cloud", cloud)) {
        outSnap.cloudCoverPct = std::clamp(cloud, 0.0, 100.0);
        outSnap.mClouds.available = true;
    }
    double visKm = 12.0;
    if (ExtractNumberScoped(json, curPos, "vis_km", visKm)) {
        outSnap.visibilityM = std::max(0.0, visKm * 1000.0);
        outSnap.mVisibility.available = true;
    }
    double isDay = 1.0;
    if (ExtractNumberScoped(json, curPos, "is_day", isDay)) {
        outSnap.isDay = (isDay > 0.5);
        outSnap.mDayNight.available = true;
    }
    double uv = 2.0;
    if (ExtractNumberScoped(json, curPos, "uv", uv)) {
        outSnap.uvIndex = std::max(0.0, uv);
        outSnap.mUv.available = true;
    }
    double mm = 0.0;
    if (ExtractNumberScoped(json, curPos, "precip_mm", mm)) {
        outSnap.precipMm = std::max(0.0, mm);
        outSnap.mPrecip.available = true;
    }
    double pm25 = 0.0;
    if (ExtractNumberScoped(json, curPos, "pm2_5", pm25)) {
        outSnap.pm25Value = std::max(0.0, pm25);
        outSnap.aqiValue = std::max(outSnap.aqiValue, outSnap.pm25Value * 2.5);
        outSnap.mAqi.available = true;
    }

    outSnap.mCondition.available = true;
    outSnap.weatherCode = (outSnap.precipMm > 0.2) ? 61 : ((outSnap.cloudCoverPct >= 80.0) ? 3 : ((outSnap.cloudCoverPct >= 25.0) ? 2 : 0));
    FinalizeParsedForecast(outSnap, L"WeatherAPI");
    return true;
}

bool ParseOpenWeatherMapJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t mainPos = json.find("\"main\":");
    if (mainPos == std::string::npos) {
        return false;
    }
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, mainPos, "temp", tempC)) {
        return false;
    }
    outSnap.tempC = tempC;
    outSnap.mTemp.available = true;

    double feelsC = tempC;
    if (ExtractNumberScoped(json, mainPos, "feels_like", feelsC)) {
        outSnap.apparentTempC = feelsC;
        outSnap.mFeelsLike.available = true;
    } else {
        outSnap.apparentTempC = tempC;
    }
    double hum = 55.0;
    if (ExtractNumberScoped(json, mainPos, "humidity", hum)) {
        outSnap.humidityPct = std::clamp(hum, 0.0, 100.0);
        outSnap.mHumidity.available = true;
    }
    double press = 1013.25;
    if (ExtractNumberScoped(json, mainPos, "pressure", press)) {
        outSnap.pressureHpa = press;
        outSnap.mPressure.available = true;
    }
    double vis = 12000.0;
    if (ExtractNumberScoped(json, 0, "visibility", vis)) {
        outSnap.visibilityM = std::max(0.0, vis);
        outSnap.mVisibility.available = true;
    }
    size_t cloudsPos = json.find("\"clouds\":");
    double cloud = 20.0;
    if (ExtractNumberScoped(json, (cloudsPos != std::string::npos) ? cloudsPos : 0, "all", cloud)) {
        outSnap.cloudCoverPct = std::clamp(cloud, 0.0, 100.0);
        outSnap.mClouds.available = true;
    }
    size_t windPos = json.find("\"wind\":");
    if (windPos != std::string::npos) {
        double windMs = 0.0;
        if (ExtractNumberScoped(json, windPos, "speed", windMs)) {
            outSnap.windSpeedKmh = std::max(0.0, windMs * 3.6);
            outSnap.windGustsKmh = outSnap.windSpeedKmh;
            outSnap.mWind.available = true;
        }
        double gustMs = 0.0;
        if (ExtractNumberScoped(json, windPos, "gust", gustMs)) {
            outSnap.windGustsKmh = std::max(0.0, gustMs * 3.6);
            outSnap.mGusts.available = true;
        }
        double deg = 0.0;
        if (ExtractNumberScoped(json, windPos, "deg", deg)) {
            outSnap.windDirDeg = deg;
        }
    }
    size_t rainPos = json.find("\"rain\":");
    double rainMm = 0.0;
    if (rainPos != std::string::npos && ExtractNumberScoped(json, rainPos, "1h", rainMm)) {
        outSnap.precipMm = std::max(0.0, rainMm);
        outSnap.mPrecip.available = true;
    }
    size_t snowPos = json.find("\"snow\":");
    double snowMm = 0.0;
    if (snowPos != std::string::npos && ExtractNumberScoped(json, snowPos, "1h", snowMm)) {
        outSnap.precipMm = std::max(outSnap.precipMm, std::max(0.0, snowMm));
        outSnap.mPrecip.available = true;
    }

    double hr = GetLocalHourFloat();
    outSnap.isDay = (hr >= 6.0 && hr < 19.0);
    outSnap.mCondition.available = true;
    if (snowMm > 0.1) {
        outSnap.weatherCode = 71;
    } else if (outSnap.precipMm > 0.1) {
        outSnap.weatherCode = 61;
    } else {
        outSnap.weatherCode = (outSnap.cloudCoverPct >= 80.0) ? 3 : ((outSnap.cloudCoverPct >= 25.0) ? 2 : 0);
    }
    FinalizeParsedForecast(outSnap, L"OWM");
    return true;
}

bool ParseVisualCrossingJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t curPos = json.find("\"currentConditions\":");
    if (curPos == std::string::npos) {
        curPos = 0;
    }
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, curPos, "temp", tempC)) {
        return false;
    }
    outSnap.tempC = tempC;
    outSnap.mTemp.available = true;

    double feelsC = tempC;
    if (ExtractNumberScoped(json, curPos, "feelslike", feelsC)) {
        outSnap.apparentTempC = feelsC;
        outSnap.mFeelsLike.available = true;
    } else {
        outSnap.apparentTempC = tempC;
    }
    double dewC = 8.0;
    if (ExtractNumberScoped(json, curPos, "dew", dewC)) {
        outSnap.dewPointC = dewC;
        outSnap.mDewPoint.available = true;
    }
    double hum = 55.0;
    if (ExtractNumberScoped(json, curPos, "humidity", hum)) {
        outSnap.humidityPct = std::clamp(hum, 0.0, 100.0);
        outSnap.mHumidity.available = true;
    }
    double press = 1013.25;
    if (ExtractNumberScoped(json, curPos, "pressure", press)) {
        outSnap.pressureHpa = press;
        outSnap.mPressure.available = true;
    }
    double windKmh = 0.0;
    if (ExtractNumberScoped(json, curPos, "windspeed", windKmh)) {
        outSnap.windSpeedKmh = std::max(0.0, windKmh);
        outSnap.windGustsKmh = outSnap.windSpeedKmh;
        outSnap.mWind.available = true;
    }
    double gustKmh = outSnap.windSpeedKmh;
    if (ExtractNumberScoped(json, curPos, "windgust", gustKmh)) {
        outSnap.windGustsKmh = std::max(0.0, gustKmh);
        outSnap.mGusts.available = true;
    }
    double deg = 0.0;
    if (ExtractNumberScoped(json, curPos, "winddir", deg)) {
        outSnap.windDirDeg = deg;
    }
    double cloud = 20.0;
    if (ExtractNumberScoped(json, curPos, "cloudcover", cloud)) {
        outSnap.cloudCoverPct = std::clamp(cloud, 0.0, 100.0);
        outSnap.mClouds.available = true;
    }
    double visKm = 12.0;
    if (ExtractNumberScoped(json, curPos, "visibility", visKm)) {
        outSnap.visibilityM = std::max(0.0, visKm * 1000.0);
        outSnap.mVisibility.available = true;
    }
    double uv = 2.0;
    if (ExtractNumberScoped(json, curPos, "uvindex", uv)) {
        outSnap.uvIndex = std::max(0.0, uv);
        outSnap.mUv.available = true;
    }
    double mm = 0.0;
    if (ExtractNumberScoped(json, curPos, "precip", mm)) {
        outSnap.precipMm = std::max(0.0, mm);
        outSnap.mPrecip.available = true;
    }

    double hr = GetLocalHourFloat();
    outSnap.isDay = (hr >= 6.0 && hr < 19.0);
    outSnap.mCondition.available = true;
    outSnap.weatherCode = (outSnap.precipMm > 0.1) ? 61 : ((outSnap.cloudCoverPct >= 80.0) ? 3 : ((outSnap.cloudCoverPct >= 25.0) ? 2 : 0));
    FinalizeParsedForecast(outSnap, L"VisCross");
    return true;
}

bool ParseTomorrowIoJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t valPos = json.find("\"values\":");
    if (valPos == std::string::npos) {
        valPos = 0;
    }
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, valPos, "temperature", tempC)) {
        return false;
    }
    outSnap.tempC = tempC;
    outSnap.mTemp.available = true;

    double appC = tempC;
    if (ExtractNumberScoped(json, valPos, "temperatureApparent", appC)) {
        outSnap.apparentTempC = appC;
        outSnap.mFeelsLike.available = true;
    } else {
        outSnap.apparentTempC = tempC;
    }
    double dewC = 8.0;
    if (ExtractNumberScoped(json, valPos, "dewPoint", dewC)) {
        outSnap.dewPointC = dewC;
        outSnap.mDewPoint.available = true;
    }
    double hum = 55.0;
    if (ExtractNumberScoped(json, valPos, "humidity", hum)) {
        outSnap.humidityPct = std::clamp(hum, 0.0, 100.0);
        outSnap.mHumidity.available = true;
    }
    double press = 1013.25;
    if (ExtractNumberScoped(json, valPos, "pressureSeaLevel", press) ||
        ExtractNumberScoped(json, valPos, "pressureSurfaceLevel", press)) {
        outSnap.pressureHpa = press;
        outSnap.mPressure.available = true;
    }
    double windMs = 0.0;
    if (ExtractNumberScoped(json, valPos, "windSpeed", windMs)) {
        outSnap.windSpeedKmh = std::max(0.0, windMs * 3.6);
        outSnap.windGustsKmh = outSnap.windSpeedKmh;
        outSnap.mWind.available = true;
    }
    double gustMs = 0.0;
    if (ExtractNumberScoped(json, valPos, "windGust", gustMs)) {
        outSnap.windGustsKmh = std::max(0.0, gustMs * 3.6);
        outSnap.mGusts.available = true;
    }
    double deg = 0.0;
    if (ExtractNumberScoped(json, valPos, "windDirection", deg)) {
        outSnap.windDirDeg = deg;
    }
    double cloud = 20.0;
    if (ExtractNumberScoped(json, valPos, "cloudCover", cloud)) {
        outSnap.cloudCoverPct = std::clamp(cloud, 0.0, 100.0);
        outSnap.mClouds.available = true;
    }
    double visKm = 12.0;
    if (ExtractNumberScoped(json, valPos, "visibility", visKm)) {
        outSnap.visibilityM = std::max(0.0, visKm * 1000.0);
        outSnap.mVisibility.available = true;
    }
    double uv = 2.0;
    if (ExtractNumberScoped(json, valPos, "uvIndex", uv)) {
        outSnap.uvIndex = std::max(0.0, uv);
        outSnap.mUv.available = true;
    }
    double mm = 0.0;
    if (ExtractNumberScoped(json, valPos, "precipitationIntensity", mm)) {
        outSnap.precipMm = std::max(0.0, mm);
        outSnap.mPrecip.available = true;
    }

    double hr = GetLocalHourFloat();
    outSnap.isDay = (hr >= 6.0 && hr < 19.0);
    outSnap.mCondition.available = true;
    outSnap.weatherCode = (outSnap.precipMm > 0.1) ? 61 : ((outSnap.cloudCoverPct >= 80.0) ? 3 : ((outSnap.cloudCoverPct >= 25.0) ? 2 : 0));
    FinalizeParsedForecast(outSnap, L"Tomorrow");
    return true;
}

bool FetchSingleWeatherSource(const std::wstring& src, const Settings& cfg, ForecastSnapshot& outSnap) {
    if (src == L"off" || src == L"auto") {
        return false;
    }
    const std::wstring amp(1, static_cast<wchar_t>(38));
    std::wstring latStr = cfg.hasValidCoords ? FormatCoordDot(cfg.latitude) : L"0.0000";
    std::wstring lonStr = cfg.hasValidCoords ? FormatCoordDot(cfg.longitude) : L"0.0000";
    std::wstring apiKey = GetProviderApiKey(src, cfg);

    if (src == L"custom_api") {
        if (cfg.customApiUrl.empty()) {
            return false;
        }
        std::wstring resolvedUrl = cfg.customApiUrl;
        resolvedUrl = ReplaceAllSubstrings(resolvedUrl, L"{lat}", latStr);
        resolvedUrl = ReplaceAllSubstrings(resolvedUrl, L"{lon}", lonStr);
        resolvedUrl = ReplaceAllSubstrings(resolvedUrl, L"{key}", cfg.customApiKey);
        std::string body = HttpGetUtf8(resolvedUrl);
        return !body.empty() && ParseWeatherJson(body, outSnap, L"Custom API");
    }
    if (src == L"local_file") {
        if (cfg.localWeatherPath.empty()) {
            return false;
        }
        HANDLE hFile = CreateFileW(
            cfg.localWeatherPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
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
        return !content.empty() && ParseWeatherJson(content, outSnap, L"Local File");
    }

    if (!cfg.hasValidCoords) {
        return false;
    }

    if (src == L"open_meteo") {
        std::wstring url =
            L"https://api.open-meteo.com/v1/forecast?latitude=" + latStr +
            amp + L"longitude=" + lonStr +
            amp + L"current=temperature_2m,relative_humidity_2m,apparent_temperature,precipitation,weather_code,cloud_cover,wind_speed_10m,wind_direction_10m,wind_gusts_10m,is_day,uv_index,pressure_msl,dew_point_2m,visibility" +
            amp + L"daily=sunrise,sunset" +
            amp + L"timezone=auto";
        std::string body = HttpGetUtf8(url);
        if (body.empty() || !ParseWeatherJson(body, outSnap, L"Open-Meteo")) {
            return false;
        }
        if (NeedsAirQualityFetch(cfg)) {
            std::wstring aqUrl =
                L"https://air-quality-api.open-meteo.com/v1/air-quality?latitude=" + latStr +
                amp + L"longitude=" + lonStr +
                amp + L"current=european_aqi,pm2_5,alder_pollen,birch_pollen,grass_pollen,ragweed_pollen" +
                amp + L"timezone=auto";
            std::string aqBody = HttpGetUtf8(aqUrl);
            if (!aqBody.empty()) {
                ParseAirQualityJson(aqBody, outSnap);
                FinalizeParsedForecast(outSnap, L"Open-Meteo");
            }
        }
        return true;
    }
    if (src == L"met_norway") {
        std::wstring url = L"https://api.met.no/weatherapi/locationforecast/2.0/compact?lat=" +
                           latStr + amp + L"lon=" + lonStr;
        std::string body = HttpGetUtf8(url, L"WindhawkWeatherOverlay/2.9 github.com/alembiqspace");
        return !body.empty() && ParseMetNorwayJson(body, outSnap);
    }
    if (src == L"seven_timer") {
        std::wstring url = L"https://www.7timer.info/bin/api.pl?lon=" + lonStr +
                           amp + L"lat=" + latStr + amp + L"product=civil" + amp + L"output=json";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseSevenTimerJson(body, outSnap);
    }
    if (src == L"weatherapi") {
        if (apiKey.empty()) {
            return false;
        }
        std::wstring url = L"https://api.weatherapi.com/v1/current.json?key=" + apiKey +
                           amp + L"q=" + latStr + L"," + lonStr + amp + L"aqi=yes";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseWeatherApiJson(body, outSnap);
    }
    if (src == L"openweathermap") {
        if (apiKey.empty()) {
            return false;
        }
        std::wstring url = L"https://api.openweathermap.org/data/2.5/weather?lat=" + latStr +
                           amp + L"lon=" + lonStr + amp + L"appid=" + apiKey + amp + L"units=metric";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseOpenWeatherMapJson(body, outSnap);
    }
    if (src == L"visual_crossing") {
        if (apiKey.empty()) {
            return false;
        }
        std::wstring url = L"https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
                           latStr + L"," + lonStr +
                           L"?unitGroup=metric" + amp + L"include=current" + amp + L"key=" + apiKey + amp + L"contentType=json";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseVisualCrossingJson(body, outSnap);
    }
    if (src == L"tomorrow_io") {
        if (apiKey.empty()) {
            return false;
        }
        std::wstring url = L"https://api.tomorrow.io/v4/weather/realtime?location=" +
                           latStr + L"," + lonStr + amp + L"apikey=" + apiKey;
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseTomorrowIoJson(body, outSnap);
    }
    return false;
}

void MergeMissingSnapshotFields(ForecastSnapshot& dst, const ForecastSnapshot& src) {
    if (!dst.mCondition.available && src.mCondition.available) {
        dst.weatherCode = src.weatherCode;
        dst.mCondition = src.mCondition;
    }
    if (!dst.mTemp.available && src.mTemp.available) {
        dst.tempC = src.tempC;
        dst.mTemp = src.mTemp;
    }
    if (!dst.mFeelsLike.available && src.mFeelsLike.available) {
        dst.apparentTempC = src.apparentTempC;
        dst.mFeelsLike = src.mFeelsLike;
    }
    if (!dst.mHumidity.available && src.mHumidity.available) {
        dst.humidityPct = src.humidityPct;
        dst.mHumidity = src.mHumidity;
    }
    if (!dst.mPressure.available && src.mPressure.available) {
        dst.pressureHpa = src.pressureHpa;
        dst.mPressure = src.mPressure;
    }
    if (!dst.mWind.available && src.mWind.available) {
        dst.windSpeedKmh = src.windSpeedKmh;
        dst.windDirDeg = src.windDirDeg;
        dst.windDirSign = src.windDirSign;
        dst.mWind = src.mWind;
    }
    if (!dst.mGusts.available && src.mGusts.available) {
        dst.windGustsKmh = src.windGustsKmh;
        dst.mGusts = src.mGusts;
    }
    if (!dst.mClouds.available && src.mClouds.available) {
        dst.cloudCoverPct = src.cloudCoverPct;
        dst.mClouds = src.mClouds;
    }
    if (!dst.mPrecip.available && src.mPrecip.available) {
        dst.precipMm = src.precipMm;
        dst.mPrecip = src.mPrecip;
    }
    if (!dst.mUv.available && src.mUv.available) {
        dst.uvIndex = src.uvIndex;
        dst.mUv = src.mUv;
    }
    if (!dst.mAqi.available && src.mAqi.available) {
        dst.aqiValue = src.aqiValue;
        dst.pm25Value = src.pm25Value;
        dst.mAqi = src.mAqi;
    }
    if (!dst.mPollen.available && src.mPollen.available) {
        dst.pollenMaxValue = src.pollenMaxValue;
        dst.mPollen = src.mPollen;
    }
    if (!dst.mVisibility.available && src.mVisibility.available) {
        dst.visibilityM = src.visibilityM;
        dst.mVisibility = src.mVisibility;
    }
    if (!dst.mDewPoint.available && src.mDewPoint.available) {
        dst.dewPointC = src.dewPointC;
        dst.mDewPoint = src.mDewPoint;
    }
    if (!dst.mDayNight.available && src.mDayNight.available) {
        dst.isDay = src.isDay;
        dst.mDayNight = src.mDayNight;
    }
    if (!dst.mSunCycle.available && src.mSunCycle.available) {
        dst.hasSunTimes = src.hasSunTimes;
        dst.sunriseHour = src.sunriseHour;
        dst.sunsetHour = src.sunsetHour;
        dst.sunriseText = src.sunriseText;
        dst.sunsetText = src.sunsetText;
        dst.mSunCycle = src.mSunCycle;
    }
}

bool HasAllSnapshotFields(const ForecastSnapshot& s) {
    return s.mCondition.available && s.mTemp.available && s.mFeelsLike.available &&
           s.mHumidity.available && s.mPressure.available && s.mWind.available &&
           s.mGusts.available && s.mClouds.available && s.mPrecip.available &&
           s.mUv.available && s.mAqi.available && s.mPollen.available &&
           s.mVisibility.available && s.mDewPoint.available &&
           s.mDayNight.available && s.mSunCycle.available;
}

bool FetchAutoAggregatedForecast(const Settings& cfg, ForecastSnapshot& outSnap) {
    std::vector<std::wstring> ordered = BuildOrderedAutoProviders(cfg.autoProviderOrder);
    bool anyOk = false;
    int sourcesUsed = 0;

    for (const auto& src : ordered) {
        if (ProviderRequiresApiKey(src) && GetProviderApiKey(src, cfg).empty()) {
            continue;
        }
        if (src == L"custom_api" && cfg.customApiUrl.empty()) {
            continue;
        }
        if (src == L"local_file" && cfg.localWeatherPath.empty()) {
            continue;
        }
        ForecastSnapshot part;
        if (FetchSingleWeatherSource(src, cfg, part)) {
            if (!anyOk) {
                outSnap = part;
                anyOk = true;
            } else {
                MergeMissingSnapshotFields(outSnap, part);
            }
            sourcesUsed++;
            if (HasAllSnapshotFields(outSnap)) {
                break;
            }
        }
    }

    if (!anyOk) {
        return false;
    }
    ApplyForecastMetrics(
        outSnap.weatherCode, outSnap.windSpeedKmh, outSnap.windGustsKmh,
        outSnap.tempC, outSnap.apparentTempC, outSnap.humidityPct,
        static_cast<int>(outSnap.cloudCoverPct), outSnap.visibilityM,
        outSnap.precipMm, outSnap.aqiValue, outSnap.pm25Value,
        outSnap.pollenMaxValue, outSnap.isDay, outSnap.hasPrecip,
        outSnap.precipKind, outSnap.effects);
    outSnap.valid = outSnap.mTemp.available || outSnap.mCondition.available;
    StampForecastUpdateTime(outSnap);
    if (sourcesUsed > 1) {
        outSnap.sourceLabel = L"AUTO (" + std::to_wstring(sourcesUsed) + L")";
    }
    return true;
}

void WeatherWorkerLoop() {
    while (g_running.load()) {
        Settings cfg;
        bool needFetch = false;
        int intervalSec = 1800;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            cfg = g_settings;
            intervalSec = cfg.updateIntervalMin * 60;
            needFetch = HasAnyForecastConsumer(cfg) && g_forecastDirty;
            if (needFetch) {
                g_forecastDirty = false;
                g_lastFetchedSource = cfg.primarySource;
                g_lastFetchedLat = cfg.latitude;
                g_lastFetchedLon = cfg.longitude;
            }
        }
        DWORD waitMs = static_cast<DWORD>(intervalSec) * 1000;
        if (needFetch) {
            ForecastSnapshot snap;
            bool ok = (cfg.primarySource == L"auto")
                ? FetchAutoAggregatedForecast(cfg, snap)
                : FetchSingleWeatherSource(cfg.primarySource, cfg, snap);
            if (!ok && cfg.fallbackSource != L"off" && cfg.fallbackSource != cfg.primarySource) {
                ok = FetchSingleWeatherSource(cfg.fallbackSource, cfg, snap);
            }
            if (ok) {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_forecast = snap;
            } else {
                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    g_forecast.lastFetchFailed = true;
                    g_forecastDirty = true;
                }
                waitMs = std::min<DWORD>(waitMs, 60000);
            }
        }
        DWORD waitRes = WAIT_TIMEOUT;
        if (g_weatherWakeEvent) {
            waitRes = WaitForSingleObject(g_weatherWakeEvent, waitMs);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
        }
        if (waitRes == WAIT_TIMEOUT) {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (HasAnyForecastConsumer(g_settings)) {
                g_forecastDirty = true;
            }
        }
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
        baseBottom -= cfg.bottomInsetPx;
    }
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
            static_cast<float>(g_canvasHeight - cfg.bottomInsetPx)
        });
    }
}

int ComputeBottomLimitY(const Settings& cfg) {
    if (!g_monitorRects.empty()) {
        return static_cast<int>(g_canvasTop + g_monitorRects.front().bottomLimitRelY);
    }
    return std::clamp(g_canvasTop + g_canvasHeight - cfg.bottomInsetPx, g_canvasTop + 10, g_canvasTop + g_canvasHeight);
}

bool IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_overlayHwnd) {
        return false;
    }
    wchar_t cls[128] = {};
    GetClassNameW(fg, cls, _countof(cls));
    std::wstring clsStr = cls;
    if (clsStr == L"Progman" || clsStr == L"WorkerW" || clsStr == L"Shell_TrayWnd") {
        return false;
    }

    RECT fgRect = {};
    if (!GetWindowRect(fg, &fgRect)) {
        return false;
    }
    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) {
        return false;
    }

    return (fgRect.left <= mi.rcMonitor.left && fgRect.top <= mi.rcMonitor.top &&
            fgRect.right >= mi.rcMonitor.right && fgRect.bottom >= mi.rcMonitor.bottom);
}

bool IsWindowCloaked(HWND hwnd) {
    constexpr DWORD kDwmwaCloaked = 14;
    using DwmGetWindowAttributeFn = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);
    static HMODULE hDwmapi = GetModuleHandleW(L"dwmapi.dll")
        ? GetModuleHandleW(L"dwmapi.dll")
        : LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hDwmapi) {
        return false;
    }
    static auto pfn = reinterpret_cast<DwmGetWindowAttributeFn>(GetProcAddress(hDwmapi, "DwmGetWindowAttribute"));
    DWORD cloaked = 0;
    return pfn && SUCCEEDED(pfn(hwnd, kDwmwaCloaked, &cloaked, sizeof(cloaked))) && cloaked != 0;
}

bool IsDesktopOccluded() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defView = nullptr;
    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr)) != nullptr) {
        if ((defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr)) != nullptr) {
            break;
        }
    }
    if (!defView && progman) {
        defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    }

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
        if ((defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr)) != nullptr) {
            break;
        }
    }
    if (!defView && progman) {
        defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    }
    return defView;
}

HWND FindDesktopInsertAfter(const std::wstring& mode, HWND overlay) {
    if (mode == L"desktop_bottom") {
        return HWND_BOTTOM;
    }
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (mode == L"progman_direct") {
        return progman ? progman : HWND_BOTTOM;
    }

    HWND defView = FindDesktopIconsWindow();
    if (defView) {
        HWND parent = GetParent(defView);
        if (parent && parent != progman) {
            if (parent != overlay) {
                return parent;
            }
            return HWND_BOTTOM;
        }
        if (progman && progman != overlay) {
            return progman;
        }
    }
    return HWND_BOTTOM;
}

void PositionDesktopOverlay(HWND overlay, const std::wstring& mode, bool forceShow) {
    if (!overlay) {
        return;
    }
    HWND insertAfter = FindDesktopInsertAfter(mode, overlay);
    SetWindowPos(overlay, insertAfter, g_canvasLeft, g_canvasTop, g_canvasWidth, g_canvasHeight,
                 SWP_NOACTIVATE | (forceShow ? SWP_SHOWWINDOW : 0));
}

bool IsRelPointInsideHud(float relX, float relY) {
    float r = g_hudBtnR.load();
    if (r <= 2.0f) {
        return false;
    }
    float dx = relX - g_hudBtnCx.load();
    float dy = relY - g_hudBtnCy.load();
    if ((dx * dx + dy * dy) <= (r * r)) {
        return true;
    }

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
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    LONG curEx = GetWindowLongW(hwnd, GWL_EXSTYLE);
    bool isTransparent = (curEx & WS_EX_TRANSPARENT) != 0;
    if (allowInteractive && isTransparent) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, curEx & ~WS_EX_TRANSPARENT);
    } else if (!allowInteractive && !isTransparent) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, curEx | WS_EX_TRANSPARENT);
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) {
        return 1;
    }
    if (msg == WM_NCHITTEST) {
        bool hitHud = IsPointInsideAnyHudRegion(lParam);
        SyncClickThroughExStyle(hwnd, hitHud);
        return hitHud ? HTCLIENT : HTTRANSPARENT;
    }
    if (msg == WM_LBUTTONDOWN) {
        int relX = static_cast<short>(LOWORD(lParam));
        int relY = static_cast<short>(HIWORD(lParam));
        float btnCx = g_hudBtnCx.load();
        float btnCy = g_hudBtnCy.load();
        float btnR = g_hudBtnR.load();
        float dx = static_cast<float>(relX) - btnCx;
        float dy = static_cast<float>(relY) - btnCy;
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
                        if (!ctrl.actionId.empty()) {
                            actionToRun = ctrl.actionId;
                        }
                        if (ctrl.fieldId != HudFieldId::None) {
                            fieldToFocus = ctrl.fieldId;
                            Settings cur;
                            {
                                std::lock_guard<std::mutex> slock(g_mutex);
                                cur = g_settings;
                            }
                            wchar_t numBuf[64] = {};
                            if (fieldToFocus == HudFieldId::Coords) {
                                initialFieldText = cur.coordsRaw;
                            } else if (fieldToFocus == HudFieldId::CloudsIntensity) {
                                swprintf_s(numBuf, L"%d", cur.cloudsIntensity);
                                initialFieldText = numBuf;
                            } else if (fieldToFocus == HudFieldId::SunUvOverride) {
                                swprintf_s(numBuf, L"%.1f", cur.sunUvOverride);
                                initialFieldText = numBuf;
                            } else if (fieldToFocus == HudFieldId::MoonPhaseOverride) {
                                swprintf_s(numBuf, L"%.2f", cur.moonPhaseOverride);
                                initialFieldText = numBuf;
                            } else if (fieldToFocus == HudFieldId::PrecipSpeedMps) {
                                swprintf_s(numBuf, L"%.1f", cur.precipFallSpeedMps);
                                initialFieldText = numBuf;
                            } else if (fieldToFocus == HudFieldId::PrecipIntensity) {
                                swprintf_s(numBuf, L"%d", cur.precipIntensity);
                                initialFieldText = numBuf;
                            } else if (fieldToFocus == HudFieldId::EffectsIntensity) {
                                swprintf_s(numBuf, L"%d", cur.effectsIntensity);
                                initialFieldText = numBuf;
                            }
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
            if (!actionToRun.empty()) {
                CycleSettingOption(actionToRun);
            }
            return 0;
        }
    } else if (msg == WM_MOUSEMOVE) {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hoverRelX = static_cast<float>(static_cast<short>(LOWORD(lParam)));
        g_hoverRelY = static_cast<float>(static_cast<short>(HIWORD(lParam)));
        if (g_draggingHudBtn) {
            int deltaX = (g_canvasLeft + static_cast<short>(LOWORD(lParam))) - g_dragStartScreenX;
            int deltaY = (g_canvasTop + static_cast<short>(HIWORD(lParam))) - g_dragStartScreenY;
            if (std::abs(deltaX) > 5 || std::abs(deltaY) > 5) {
                g_movedDuringDrag = true;
            }
            float r = g_hudBtnR.load();
            float clampedX = std::clamp(g_dragStartBtnCx + static_cast<float>(deltaX),
                                        r + 6.0f, static_cast<float>(g_canvasWidth) - r - 6.0f);
            float clampedY = std::clamp(g_dragStartBtnCy + static_cast<float>(deltaY),
                                        r + 6.0f, static_cast<float>(g_canvasHeight) - r - 6.0f);
            g_hudBtnCx.store(clampedX);
            g_hudBtnCy.store(clampedY);
            return 0;
        }
    } else if (msg == WM_LBUTTONUP) {
        bool wasDragging = false;
        bool didMove = false;
        float finalCx = 0.0f;
        float finalCy = 0.0f;
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
        if (shouldCommit) {
            CommitEditedFieldValue(activeField, bufCopy);
        }
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
    std::uniform_real_distribution<float> phaseDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> alphaDist(0.65f, 0.96f);
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

    float depthMul = (perf == PerformanceMode::Low)
        ? 1.0f
        : ((item.depthTier == 0) ? 0.75f : ((item.depthTier == 2) ? 1.25f : 1.0f));
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
    if (!surf.rt) {
        return false;
    }
    if (surf.flakeSprite) {
        surf.flakeSprite->Release();
        surf.flakeSprite = nullptr;
    }
    auto shade = [&](float r, float g, float b, float a) -> D2D1_COLOR_F {
        if (!hasCustomColor) {
            return D2D1::ColorF(r, g, b, a);
        }
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
    bmpRt->Clear(D2D1::ColorF(0, 0, 0, 0));
    if (radBrush) {
        bmpRt->FillRectangle(D2D1::RectF(0.0f, 0.0f, 32.0f, 32.0f), radBrush);
    }
    bmpRt->EndDraw();
    if (radBrush) {
        radBrush->Release();
    }
    if (stopCol) {
        stopCol->Release();
    }
    bmpRt->GetBitmap(&surf.flakeSprite);
    bmpRt->Release();
    surf.spriteColor = snowCol;
    return surf.flakeSprite != nullptr;
}

bool EnsureDirectWrite(float fontSizePx, float circleFontPx) {
    if (!g_dwriteFactory) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_dwriteFactory));
    }
    if (!g_dwriteFactory) {
        return false;
    }
    if (std::abs(g_cachedFontSizePx - fontSizePx) > 0.1f) {
        if (g_hudPillFormat) {
            g_hudPillFormat->Release();
            g_hudPillFormat = nullptr;
        }
        if (g_hudTitleFormat) {
            g_hudTitleFormat->Release();
            g_hudTitleFormat = nullptr;
        }
        if (g_hudBodyFormat) {
            g_hudBodyFormat->Release();
            g_hudBodyFormat = nullptr;
        }
        if (g_hudItalicFormat) {
            g_hudItalicFormat->Release();
            g_hudItalicFormat = nullptr;
        }
        g_cachedFontSizePx = fontSizePx;
    }
    if (std::abs(g_cachedCircleFontPx - circleFontPx) > 0.1f) {
        if (g_hudCircleFormat) {
            g_hudCircleFormat->Release();
            g_hudCircleFormat = nullptr;
        }
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
    if (!g_hudItalicFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                                        DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL, fontSizePx, L"en-US", &g_hudItalicFormat);
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
    return g_hudTitleFormat && g_hudBodyFormat && g_hudItalicFormat && g_hudCircleFormat && g_hudPillFormat;
}

void CleanupDirectWrite() {
    if (g_hudPillFormat) {
        g_hudPillFormat->Release();
        g_hudPillFormat = nullptr;
    }
    if (g_hudCircleFormat) {
        g_hudCircleFormat->Release();
        g_hudCircleFormat = nullptr;
    }
    if (g_hudTitleFormat) {
        g_hudTitleFormat->Release();
        g_hudTitleFormat = nullptr;
    }
    if (g_hudBodyFormat) {
        g_hudBodyFormat->Release();
        g_hudBodyFormat = nullptr;
    }
    if (g_hudItalicFormat) {
        g_hudItalicFormat->Release();
        g_hudItalicFormat = nullptr;
    }
    if (g_dwriteFactory) {
        g_dwriteFactory->Release();
        g_dwriteFactory = nullptr;
    }
    g_cachedFontSizePx = 0.0f;
    g_cachedCircleFontPx = 0.0f;
}

bool EnsureSurface(CachedSurface& surf, int width, int height, D2D1_COLOR_F snowCol, bool hasCustomSnow) {
    if (!g_d2dFactory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory);
    }
    if (!g_d2dFactory) {
        return false;
    }
    if (!surf.rt || !surf.memDC || !surf.dib || surf.width != width || surf.height != height) {
        CleanupSurface(surf);
        HDC screenDC = GetDC(nullptr);
        surf.memDC = CreateCompatibleDC(screenDC);
        ReleaseDC(nullptr, screenDC);
        if (!surf.memDC) {
            return false;
        }
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

void DrawRainSplashesHigh(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                          const Settings& cfg, const FallingItem& p, float bottomRelY, double timeSec) {
    if (cfg.performanceMode != PerformanceMode::High || p.y < bottomRelY - 14.0f || p.y > bottomRelY + 4.0f) {
        return;
    }
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
    if (!hwnd || !IsWindow(hwnd) || !surf.memDC) {
        return;
    }
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
    if (topCol.a <= 0.003f && botCol.a <= 0.003f) {
        return;
    }

    D2D1_GRADIENT_STOP stops[2] = {{0.0f, topCol}, {1.0f, botCol}};
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

float ComputeCursorHoverFadeMultiplier(float cx, float cy, float hitRadius, int hoverOpacityPct,
                                     bool hasCursor, float curRelX, float curRelY) {
    if (!hasCursor) {
        return 1.0f;
    }
    float minMul = std::clamp(static_cast<float>(hoverOpacityPct) / 100.0f, 0.0f, 1.0f);
    float dx = curRelX - cx;
    float dy = curRelY - cy;
    float distSq = dx * dx + dy * dy;
    float rSq = hitRadius * hitRadius;
    if (distSq <= rSq) {
        return minMul;
    }
    float softOuter = hitRadius * 1.35f;
    float outerSq = softOuter * softOuter;
    if (distSq < outerSq) {
        float dist = std::sqrt(distSq);
        float t = (dist - hitRadius) / std::max(1.0f, softOuter - hitRadius);
        return minMul + (1.0f - minMul) * std::clamp(t, 0.0f, 1.0f);
    }
    return 1.0f;
}

void DrawDriftingCloudsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             const Settings& cfg, float cloudPct01, float windVx, double timeSec,
                             bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    if (cloudPct01 <= 0.01f) {
        return;
    }
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
            if (rawOffset < 0.0f) {
                rawOffset += spanW;
            }
            float cx = mon.left + rawOffset - 130.0f;
            float cy = mon.top + 22.0f + ((i * 37) % 100) * 0.01f * bandH;
            float rx = (68.0f + (i % 4) * 30.0f) * (0.65f + cloudPct01 * 0.55f);
            float ry = (22.0f + (i % 3) * 10.0f) * (0.65f + cloudPct01 * 0.50f);
            float edgeFade = ComputeMonitorEdgeFade(mon, cx, cy, rx + 45.0f);
            float hoverFade = ComputeCursorHoverFadeMultiplier(
                cx, cy, std::max(rx, ry), cfg.cursorHoverOpacityPct, hasCursor, curRelX, curRelY);
            if (hoverFade < 0.999f) {
                outCursorNearSky = true;
            }
            float totalFade = edgeFade * hoverFade;
            if (totalFade <= 0.002f) {
                continue;
            }

            float alpha = std::clamp(
                base.a * (0.25f + cloudPct01 * 0.85f) *
                (0.88f + 0.12f * static_cast<float>(std::sin(timeSec * 0.7 + fi))) * totalFade,
                0.002f, 0.68f);

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
                  float uvIndex, float cloudDimFactor, double timeSec,
                  bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    if (!active || cfg.sunGeom.radius <= 0 || g_monitorRects.empty()) {
        return;
    }
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

    size_t monCount = (cfg.sunMonitorMode == CelestialMonitorMode::All) ? g_monitorRects.size() : 1;
    for (size_t mIdx = 0; mIdx < monCount; ++mIdx) {
        const auto& mon = g_monitorRects[mIdx];
        float radius = 0.0f;
        float cx = 0.0f;
        float cy = 0.0f;
        ComputeCelestialCoordsInMonitor(mon, true, cfg.sunMotionMode, cfg.sunGeom, hour, snap, radius, cx, cy);
        if (radius < 3.0f) {
            continue;
        }
        float hoverFade = ComputeCursorHoverFadeMultiplier(
            cx, cy, radius * 1.9f, cfg.cursorHoverOpacityPct, hasCursor, curRelX, curRelY);
        if (hoverFade < 0.999f) {
            outCursorNearSky = true;
        }
        float visFactor = baseVis * ComputeMonitorEdgeFade(mon, cx, cy, radius * 2.4f + 24.0f) * hoverFade;
        if (visFactor <= 0.002f) {
            continue;
        }

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
            brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.32f * visFactor, 0.002f, 0.55f)));
            for (int rIdx = 0; rIdx < rayCount; ++rIdx) {
                double ang = (2.0 * kPi * rIdx) / rayCount + timeSec * 0.08;
                float innerR = radius * 1.18f;
                float outerR = radius * ((rIdx % 2 == 0) ? 1.78f : 1.50f);
                rt->DrawLine(
                    D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * innerR, cy + static_cast<float>(std::sin(ang)) * innerR),
                    D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * outerR, cy + static_cast<float>(std::sin(ang)) * outerR),
                    brush, std::max(1.5f, radius * 0.045f));
            }
        }
        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * visFactor, 0.002f, 1.0f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius), brush);
    }
}

void DrawMoonLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg,
                   bool active, double hour, const ForecastSnapshot& snap,
                   double moonPhase01, float cloudDimFactor,
                   bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    if (!active || cfg.moonGeom.radius <= 0 || g_monitorRects.empty()) {
        return;
    }
    D2D1_COLOR_F base = cfg.hasCustomMoonColor ? cfg.colorMoon : D2D1::ColorF(0.88f, 0.92f, 1.0f, 0.95f);
    float baseVis = std::clamp(cloudDimFactor, 0.18f, 1.0f);
    double phase = std::clamp(moonPhase01, 0.0, 1.0);
    double illum = 0.5 * (1.0 - std::cos(phase * 2.0 * kPi));
    float sideSign = (phase <= 0.5) ? 1.0f : -1.0f;

    size_t monCount = (cfg.moonMonitorMode == CelestialMonitorMode::All) ? g_monitorRects.size() : 1;
    for (size_t mIdx = 0; mIdx < monCount; ++mIdx) {
        const auto& mon = g_monitorRects[mIdx];
        float r = 0.0f;
        float cx = 0.0f;
        float cy = 0.0f;
        ComputeCelestialCoordsInMonitor(mon, false, cfg.moonMotionMode, cfg.moonGeom, hour, snap, r, cx, cy);
        if (r < 3.0f) {
            continue;
        }
        float hoverFade = ComputeCursorHoverFadeMultiplier(
            cx, cy, r * 1.6f, cfg.cursorHoverOpacityPct, hasCursor, curRelX, curRelY);
        if (hoverFade < 0.999f) {
            outCursorNearSky = true;
        }
        float visFactor = baseVis * ComputeMonitorEdgeFade(mon, cx, cy, r * 2.0f + 24.0f) * hoverFade;
        if (visFactor <= 0.002f) {
            continue;
        }

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
        brush->SetColor(D2D1::ColorF(0.14f, 0.17f, 0.25f, std::clamp(0.58f * visFactor, 0.002f, 0.78f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), brush);

        float litWidth = std::max(1.5f, static_cast<float>(illum) * r * 1.92f);
        brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * visFactor, 0.002f, 1.0f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + sideSign * (r - litWidth * 0.5f), cy), litWidth * 0.55f, r), brush);

        if (cfg.performanceMode != PerformanceMode::Low && illum > 0.18 && r >= 10.0f) {
            brush->SetColor(D2D1::ColorF(base.r * 0.72f, base.g * 0.75f, base.b * 0.84f, std::clamp(0.28f * visFactor, 0.002f, 0.42f)));
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
    if (cfg.rainbowGeom.radius <= 0 || strength <= 0.004f) {
        return;
    }
    RefreshArcCache(g_rainbowCache, cfg.rainbowGeom, forceDirty);
    if (g_rainbowCache.radius < 4.0f) {
        return;
    }

    const D2D1_COLOR_F spectrum[6] = {
        D2D1::ColorF(1.0f, 0.22f, 0.28f, 0.24f),
        D2D1::ColorF(1.0f, 0.58f, 0.15f, 0.24f),
        D2D1::ColorF(1.0f, 0.90f, 0.20f, 0.24f),
        D2D1::ColorF(0.20f, 0.85f, 0.42f, 0.24f),
        D2D1::ColorF(0.22f, 0.62f, 1.0f, 0.24f),
        D2D1::ColorF(0.60f, 0.34f, 0.96f, 0.24f)
    };
    brush->SetOpacity(1.0f);
    float shimmer = (cfg.performanceMode == PerformanceMode::Low)
        ? 0.95f
        : (0.92f + 0.08f * static_cast<float>(std::sin(timeSec * 1.4)));
    int bands = (cfg.performanceMode == PerformanceMode::Low) ? 3 : 6;
    for (int i = 0; i < bands; ++i) {
        D2D1_COLOR_F c = cfg.hasCustomRainbowColor ? cfg.colorRainbow : spectrum[i];
        float alpha = std::clamp(c.a * strength * (1.0f - std::abs(i - (bands - 1) * 0.5f) * 0.14f) * shimmer, 0.0f, 0.65f);
        if (alpha <= 0.003f) {
            continue;
        }
        float r = g_rainbowCache.radius - i * (g_rainbowCache.bandThickness * 0.85f);
        if (r <= 2.0f) {
            break;
        }
        brush->SetColor(D2D1::ColorF(c.r, c.g, c.b, alpha));
        rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_rainbowCache.cx, g_rainbowCache.cy), r, r), brush, g_rainbowCache.bandThickness);
    }
}

void DrawSmoothAurora(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                    const Settings& cfg, float strength, double timeSec) {
    if (strength <= 0.004f) {
        return;
    }
    D2D1_COLOR_F base = cfg.hasCustomAuroraColor ? cfg.colorAurora : D2D1::ColorF(0.20f, 0.94f, 0.62f, 0.28f);
    float bandHeight = std::max(90.0f, static_cast<float>(g_canvasHeight) * 0.22f);
    int ribbons = (cfg.performanceMode == PerformanceMode::Low) ? 1 : ((cfg.performanceMode == PerformanceMode::High) ? 3 : 2);
    float stepX = (cfg.performanceMode == PerformanceMode::High) ? 12.0f : ((cfg.performanceMode == PerformanceMode::Low) ? 40.0f : 24.0f);

    brush->SetOpacity(1.0f);
    for (int ribbon = 0; ribbon < ribbons; ++ribbon) {
        D2D1_COLOR_F cTop = base;
        cTop.a = std::clamp(base.a * (ribbon == 0 ? 1.0f : 0.60f) * strength, 0.0f, 0.65f);
        D2D1_COLOR_F cBot = cTop;
        cBot.a = 0.0f;
        float y0 = 16.0f + ribbon * 26.0f;
        DrawVerticalGradientTint(rt, cTop, cBot, y0, y0 + bandHeight, 1.0f);
        brush->SetColor(D2D1::ColorF(cTop.r, cTop.g, cTop.b, std::clamp(cTop.a * 1.15f, 0.0f, 0.75f)));
        float prevX = 0.0f;
        float prevY = y0 + static_cast<float>(std::sin(timeSec * 1.1 + ribbon)) * 14.0f;
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
    if (strength <= 0.004f) {
        return;
    }
    float botY = bottomRelY;
    float bandH = std::max(50.0f, static_cast<float>(g_canvasHeight) * 0.14f);
    float topY = std::max(0.0f, botY - bandH);
    D2D1_COLOR_F base = cfg.hasCustomMirageColor ? cfg.colorMirage : D2D1::ColorF(1.0f, 0.88f, 0.68f, 0.25f);
    D2D1_COLOR_F topCol = base;
    topCol.a = 0.0f;
    D2D1_COLOR_F botCol = base;
    botCol.a = std::clamp(base.a * strength, 0.0f, 0.55f);
    DrawVerticalGradientTint(rt, topCol, botCol, topY, botY, 1.0f);

    int lines = (cfg.performanceMode == PerformanceMode::High) ? 4 : ((cfg.performanceMode == PerformanceMode::Low) ? 2 : 3);
    float stepX = (cfg.performanceMode == PerformanceMode::High) ? 12.0f : 20.0f;
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * 0.75f * strength, 0.0f, 0.5f)));
    for (int line = 0; line < lines; ++line) {
        float baseLineY = topY + bandH * (0.25f + line * (0.65f / std::max(1, lines - 1)));
        float prevX = 0.0f;
        float prevY = baseLineY + static_cast<float>(std::sin(timeSec * 2.4 + line * 1.7)) * 3.0f;
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
    float curX = flash.xStart;
    float curY = 0.0f;
    int segs = (cfg.performanceMode == PerformanceMode::Low) ? 10 : 14;
    float stepY = std::max(20.0f, maxHeight * 0.72f / segs);
    for (int i = 0; i < segs; ++i) {
        float t = static_cast<float>(i + 1) / segs;
        float nextX = flash.xStart + (flash.xEnd - flash.xStart) * t + ((i % 2 == 0) ? 16.0f : -16.0f);
        float nextY = curY + stepY;
        rt->DrawLine(D2D1::Point2F(curX, curY), D2D1::Point2F(nextX, nextY), brush, 2.8f);
        if (cfg.performanceMode != PerformanceMode::Low &&
            (i == 4 || i == 8 || (cfg.performanceMode == PerformanceMode::High && i == 11))) {
            float branchDir = (i == 4 || i == 11) ? 1.0f : -1.0f;
            float b1x = nextX + branchDir * 28.0f;
            float b1y = nextY + stepY * 1.4f;
            rt->DrawLine(D2D1::Point2F(nextX, nextY), D2D1::Point2F(b1x, b1y), brush, 1.6f);
            rt->DrawLine(D2D1::Point2F(b1x, b1y), D2D1::Point2F(b1x + branchDir * 16.0f, b1y + stepY * 1.1f), brush, 1.1f);
        }
        curX = nextX;
        curY = nextY;
    }
}

void DrawWindStreamsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                          const Settings& cfg, const ForecastSnapshot& snap, float strength, double timeSec) {
    if (strength <= 0.004f) {
        return;
    }
    float gustKmh = static_cast<float>(std::max({snap.windGustsKmh, snap.windSpeedKmh, 28.0}));
    float speedPx = std::clamp(gustKmh * 6.5f, 160.0f, 520.0f);
    float dir = (snap.windDirSign != 0) ? static_cast<float>(snap.windDirSign) : 1.0f;
    int streaks = std::clamp(static_cast<int>(8 * GetQualityScale(cfg.performanceMode)), 5, 18);
    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        for (int i = 0; i < streaks; ++i) {
            float fi = static_cast<float>(i);
            float spanW = mon.width + 220.0f;
            float rawX = std::fmod(fi * (mon.width / static_cast<float>(streaks)) +
                                   static_cast<float>(timeSec) * speedPx * (0.75f + 0.15f * (i % 3)) * dir, spanW);
            if (rawX < 0.0f) {
                rawX += spanW;
            }
            float x0 = mon.left + rawX - 110.0f;
            float y0 = mon.top + 40.0f + ((i * 53) % 100) * 0.0075f * mon.height;
            float len = 45.0f + (i % 4) * 18.0f;
            float fade = ComputeMonitorEdgeFade(mon, x0 + len * 0.5f, y0, 40.0f);
            brush->SetColor(D2D1::ColorF(0.86f, 0.93f, 1.0f, std::clamp(0.22f * strength * fade, 0.02f, 0.45f)));
            rt->DrawLine(D2D1::Point2F(x0, y0), D2D1::Point2F(x0 + len * dir, y0 + static_cast<float>(std::sin(timeSec * 4.0 + fi)) * 5.0f), brush, 1.4f);
        }
    }
}

void DrawThermalLayer(ID2D1DCRenderTarget* rt, bool frostOn, bool heatOn, float strength) {
    if (strength <= 0.004f) {
        return;
    }
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
    if (strength <= 0.004f) {
        return;
    }
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
    if (strength <= 0.004f) {
        return;
    }
    float aqiNorm = std::clamp(static_cast<float>(std::max(snap.aqiValue, snap.pm25Value * 2.5)) / 150.0f, 0.15f, 1.0f);
    D2D1_COLOR_F base = cfg.hasCustomAirQualityColor ? cfg.colorAirQuality : D2D1::ColorF(0.82f, 0.66f, 0.34f, 0.22f);
    D2D1_COLOR_F topCol = base;
    topCol.a = base.a * 0.45f * aqiNorm;
    D2D1_COLOR_F botCol = base;
    botCol.a = base.a * (0.75f + 0.45f * aqiNorm);
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
    if (strength <= 0.004f) {
        return;
    }
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
    if (strength <= 0.004f) {
        return;
    }
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
            float prevX = mon.left;
            float prevY = topY + static_cast<float>(std::sin(timeSec * 3.0 + mon.left * 0.01)) * 4.0f;
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
    brush->SetColor(D2D1::ColorF(ctrl.tint.r, ctrl.tint.g, ctrl.tint.b, 0.28f));
    rt->FillRoundedRectangle(rr, brush);
    brush->SetColor(D2D1::ColorF(ctrl.tint.r, ctrl.tint.g, ctrl.tint.b, 0.88f));
    rt->DrawRoundedRectangle(rr, brush, 1.2f);
    brush->SetColor(cfg.colorHudText);
    rt->DrawTextW(ctrl.btnText.c_str(), static_cast<UINT32>(ctrl.btnText.size()), g_hudPillFormat, ctrl.rect, brush);
}

void DrawGearSettingsIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                        float cx, float cy, float radius, D2D1_COLOR_F col) {
    brush->SetOpacity(1.0f);
    brush->SetColor(col);
    float outerR = radius * 0.56f;
    float innerR = radius * 0.38f;
    float holeR = radius * 0.20f;
    int teeth = 8;
    for (int i = 0; i < teeth; ++i) {
        double ang = (2.0 * kPi * i) / teeth;
        float tx = cx + static_cast<float>(std::cos(ang)) * outerR;
        float ty = cy + static_cast<float>(std::sin(ang)) * outerR;
        float tSize = std::max(2.0f, radius * 0.14f);
        rt->FillRectangle(D2D1::RectF(tx - tSize * 0.5f, ty - tSize * 0.5f, tx + tSize * 0.5f, ty + tSize * 0.5f), brush);
    }
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), innerR, innerR), brush);
    brush->SetColor(D2D1::ColorF(0.08f, 0.10f, 0.15f, 1.0f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), holeR, holeR), brush);
}

void DrawInfoBadgeIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                       D2D1_RECT_F rect, D2D1_COLOR_F textCol, bool hovered) {
    float cx = (rect.left + rect.right) * 0.5f;
    float cy = (rect.top + rect.bottom) * 0.5f;
    float r = std::max(5.5f, std::min(rect.right - rect.left, rect.bottom - rect.top) * 0.45f);
    brush->SetOpacity(1.0f);
    brush->SetColor(hovered
        ? D2D1::ColorF(0.35f, 0.72f, 1.0f, 0.35f)
        : D2D1::ColorF(textCol.r, textCol.g, textCol.b, 0.12f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), brush);
    brush->SetColor(hovered
        ? D2D1::ColorF(0.45f, 0.80f, 1.0f, 0.95f)
        : D2D1::ColorF(textCol.r, textCol.g, textCol.b, 0.55f));
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), brush, 1.0f);
    brush->SetColor(textCol);
    rt->DrawTextW(L"i", 1, g_hudPillFormat, rect, brush);
}

D2D1_COLOR_F ComputeProviderStatusTint(const ForecastSnapshot& snap) {
    if (snap.lastFetchFailed) {
        return D2D1::ColorF(0.92f, 0.30f, 0.30f, 0.95f);
    }
    if (!snap.valid || snap.lastSuccessEpochSec <= 0) {
        return D2D1::ColorF(0.95f, 0.78f, 0.24f, 0.95f);
    }
    int64_t ageSec = std::max<int64_t>(0, GetNowEpochSec() - snap.lastSuccessEpochSec);
    if (ageSec >= 86400) {
        return D2D1::ColorF(0.95f, 0.78f, 0.24f, 0.95f);
    }
    if (ageSec >= 21600) {
        return D2D1::ColorF(0.60f, 0.65f, 0.72f, 0.95f);
    }
    return D2D1::ColorF(0.24f, 0.85f, 0.48f, 0.95f);
}

std::wstring BuildMetricTooltip(const MetricMeta& meta) {
    if (!meta.available || meta.sourceName.empty()) {
        return L"Not provided by active source";
    }
    return meta.sourceName + L"  •  " + meta.syncTime;
}

void DrawForecastHudWidget(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, const ForecastSnapshot& snap,
                           float uvIndex, double moonPhase01, int bottomLimitY) {
    float btnR = cfg.hudButtonRadiusPx;
    bool expanded = g_hudExpanded.load();
    bool showGearIcon = (!expanded && !snap.valid);
    std::wstring labelText;
    if (expanded) {
        labelText = L"×";
    } else if (snap.valid) {
        labelText = FormatTempValue(snap.tempC, cfg.tempUnit, true);
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
    float btnCx = 0.0f;
    float btnCy = 0.0f;
    bool isDragging = false;
    ExpandedSection expSec = ExpandedSection::None;
    float hoverX = -1000.0f;
    float hoverY = -1000.0f;
    {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        isDragging = g_draggingHudBtn;
        expSec = g_expandedSection;
        hoverX = g_hoverRelX;
        hoverY = g_hoverRelY;
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
        if (m == TriStateMode::On) {
            return {L"ON", D2D1::ColorF(0.25f, 0.85f, 0.48f, 0.9f)};
        }
        if (m == TriStateMode::Off) {
            return {L"OFF", D2D1::ColorF(0.85f, 0.35f, 0.35f, 0.9f)};
        }
        return {L"AUTO", D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f)};
    };

    D2D1_COLOR_F sectionGrayTint = D2D1::ColorF(0.58f, 0.63f, 0.70f, 0.90f);

    struct HudRow {
        std::wstring label;
        std::wstring value;
        bool isHeader = false;
        bool isMetric = false;
        bool available = true;
        std::wstring tooltip;
        std::wstring actionId;
        std::wstring btnLabel;
        D2D1_COLOR_F btnTint = D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.90f);
        HudFieldId editField = HudFieldId::None;
    };

    std::vector<HudRow> rows;
    wchar_t buf[96] = {};
    rows.push_back({
        L"FORECAST", snap.valid ? (snap.sourceLabel + L"  " + snap.lastUpdateText) : L"No data",
        true, false, true, L"", L"", L"", D2D1::ColorF(0, 0, 0, 0), HudFieldId::None
    });
    rows.push_back({
        L"Coords", cfg.coordsRaw.empty() ? L"(empty)" : cfg.coordsRaw, false, false, true, L"",
        L"cycle_source", FormatProviderHudBadge(cfg.primarySource), ComputeProviderStatusTint(snap),
        cfg.primarySource != L"off" ? HudFieldId::Coords : HudFieldId::None
    });

    auto addMetricRow = [&](const wchar_t* name, const MetricMeta& meta, const std::wstring& valStr) {
        bool ok = snap.valid && meta.available;
        rows.push_back({
            name,
            ok ? valStr : L"--",
            false,
            true,
            ok,
            BuildMetricTooltip(meta),
            L"",
            L"",
            D2D1::ColorF(0, 0, 0, 0),
            HudFieldId::None
        });
    };

    addMetricRow(L"Condition", snap.mCondition, WmoCodeToLabel(snap.weatherCode));
    addMetricRow(L"Temperature", snap.mTemp, FormatTempValue(snap.tempC, cfg.tempUnit, false));
    addMetricRow(L"Feels like", snap.mFeelsLike, FormatTempValue(snap.apparentTempC, cfg.tempUnit, false));
    swprintf_s(buf, L"%.0f%%", snap.humidityPct);
    addMetricRow(L"Humidity", snap.mHumidity, buf);
    swprintf_s(buf, L"%.0f hPa", snap.pressureHpa);
    addMetricRow(L"Pressure", snap.mPressure, buf);
    addMetricRow(L"Wind", snap.mWind, FormatWindValue(snap.windSpeedKmh, cfg.windUnit) + L" (" + WindDegToCompass(snap.windDirDeg) + L")");
    addMetricRow(L"Wind gusts", snap.mGusts, FormatWindValue(snap.windGustsKmh, cfg.windUnit));
    addMetricRow(L"Cloud cover", snap.mClouds, FormatCloudValue(snap.cloudCoverPct, cfg.cloudUnit));
    addMetricRow(L"Precipitation", snap.mPrecip, FormatPrecipValue(snap.precipMm, cfg.precipUnit));
    addMetricRow(L"UV index", snap.mUv, FormatUvValue(snap.uvIndex, cfg.uvUnit));
    addMetricRow(L"Air quality", snap.mAqi, FormatAqiValue(snap.aqiValue, snap.pm25Value, cfg.aqiUnit));
    swprintf_s(buf, L"%.0f idx", snap.pollenMaxValue);
    addMetricRow(L"Pollen", snap.mPollen, buf);
    swprintf_s(buf, L"%.1f km", snap.visibilityM / 1000.0);
    addMetricRow(L"Visibility", snap.mVisibility, buf);
    addMetricRow(L"Dew point", snap.mDewPoint, FormatTempValue(snap.dewPointC, cfg.tempUnit, false));
    addMetricRow(L"Day / Night", snap.mDayNight, snap.isDay ? L"Day" : L"Night");
    addMetricRow(L"Sun cycle", snap.mSunCycle, snap.sunriseText + L" / " + snap.sunsetText);

    rows.push_back({
        L"[Sky]", expSec == ExpandedSection::Sky ? L"Expanded" : L"Collapsed", true, false, true, L"",
        L"sec_sky", expSec == ExpandedSection::Sky ? L"HIDE" : L"OPEN", sectionGrayTint, HudFieldId::None
    });
    if (expSec == ExpandedSection::Sky) {
        auto cB = triBadge(cfg.cloudsMode);
        swprintf_s(buf, L"%d%%", cfg.cloudsIntensity);
        rows.push_back({
            L"Clouds",
            cfg.cloudsMode == TriStateMode::On ? buf : L"",
            false, false, true, L"",
            L"cycle_clouds", cB.first, cB.second,
            cfg.cloudsMode == TriStateMode::On ? HudFieldId::CloudsIntensity : HudFieldId::None
        });

        auto sB = triBadge(cfg.sunMode);
        swprintf_s(buf, L"%.1f", uvIndex);
        rows.push_back({
            L"Sun",
            cfg.sunMode == TriStateMode::On ? buf : L"",
            false, false, true, L"",
            L"cycle_sun", sB.first, sB.second,
            cfg.sunMode == TriStateMode::On ? HudFieldId::SunUvOverride : HudFieldId::None
        });

        auto mB = triBadge(cfg.moonMode);
        swprintf_s(buf, L"%.2f", moonPhase01);
        rows.push_back({
            L"Moon",
            cfg.moonMode == TriStateMode::On ? buf : L"",
            false, false, true, L"",
            L"cycle_moon", mB.first, mB.second,
            cfg.moonMode == TriStateMode::On ? HudFieldId::MoonPhaseOverride : HudFieldId::None
        });

        rows.push_back({
            L"Over clouds", L"", false, false, true, L"",
            L"toggle_celestial_front",
            cfg.celestialInFrontOfClouds ? L"FRONT" : L"DIM",
            D2D1::ColorF(0.65f, 0.78f, 0.95f, 0.9f),
            HudFieldId::None
        });
    }

    rows.push_back({
        L"[Precipitation]", expSec == ExpandedSection::Precipitation ? L"Expanded" : L"Collapsed", true, false, true, L"",
        L"sec_precip", expSec == ExpandedSection::Precipitation ? L"HIDE" : L"OPEN", sectionGrayTint, HudFieldId::None
    });
    if (expSec == ExpandedSection::Precipitation) {
        const wchar_t* pModeStr = (cfg.precipitationMode == PrecipMode::Auto)
            ? L"AUTO"
            : ((cfg.precipitationMode == PrecipMode::Rain)
                ? L"RAIN"
                : ((cfg.precipitationMode == PrecipMode::Snow)
                    ? L"SNOW"
                    : ((cfg.precipitationMode == PrecipMode::Hail) ? L"HAIL" : L"OFF")));
        std::wstring providerPrecipStr = L"none";
        if (snap.valid && snap.hasPrecip) {
            if (snap.precipKind == ObjectKind::Snow) {
                providerPrecipStr = L"snow";
            } else if (snap.precipKind == ObjectKind::Hail) {
                providerPrecipStr = L"hail";
            } else {
                providerPrecipStr = L"rain";
            }
        }
        rows.push_back({L"Mode", providerPrecipStr, false, false, true, L"", L"cycle_precip_mode", pModeStr, D2D1::ColorF(0.35f, 0.72f, 1.0f, 0.9f), HudFieldId::None});

        const wchar_t* dStr = (cfg.precipFallDirection == FallDirMode::Auto)
            ? L"AUTO"
            : ((cfg.precipFallDirection == FallDirMode::Down)
                ? L"DOWN"
                : ((cfg.precipFallDirection == FallDirMode::DownLeft) ? L"LEFT" : L"RIGHT"));
        rows.push_back({L"Direction", L"", false, false, true, L"", L"cycle_precip_dir", dStr, D2D1::ColorF(0.45f, 0.78f, 1.0f, 0.9f), HudFieldId::None});

        swprintf_s(buf, L"%.1f m/s", cfg.precipFallSpeedMps);
        rows.push_back({
            L"Fall speed",
            cfg.precipSpeedMode == PrecipSpeedMode::Custom ? buf : L"",
            false, false, true, L"",
            L"cycle_precip_speed",
            cfg.precipSpeedMode == PrecipSpeedMode::Auto ? L"AUTO" : L"CUSTOM",
            D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f),
            cfg.precipSpeedMode == PrecipSpeedMode::Custom ? HudFieldId::PrecipSpeedMps : HudFieldId::None
        });

        swprintf_s(buf, L"%d%%", cfg.precipIntensity);
        bool showPrecipInt = (cfg.precipitationMode != PrecipMode::Off && cfg.precipitationMode != PrecipMode::Auto);
        rows.push_back({
            L"Intensity",
            showPrecipInt ? buf : L"",
            false, false, true, L"", L"", L"", D2D1::ColorF(0, 0, 0, 0),
            showPrecipInt ? HudFieldId::PrecipIntensity : HudFieldId::None
        });
    }

    rows.push_back({
        L"[Effects]", expSec == ExpandedSection::Effects ? L"Expanded" : L"Collapsed", true, false, true, L"",
        L"sec_effects", expSec == ExpandedSection::Effects ? L"HIDE" : L"OPEN", sectionGrayTint, HudFieldId::None
    });
    if (expSec == ExpandedSection::Effects) {
        swprintf_s(buf, L"%d%%", cfg.effectsIntensity);
        bool anyManualFx = (cfg.effectDust == TriStateMode::On ||
                            cfg.effectFog == TriStateMode::On || cfg.effectSmog == TriStateMode::On ||
                            cfg.effectThunderstorm == TriStateMode::On || cfg.effectAurora == TriStateMode::On ||
                            cfg.effectMirage == TriStateMode::On || cfg.effectRainbow == TriStateMode::On ||
                            cfg.effectWindStreams == TriStateMode::On || cfg.effectThermal == TriStateMode::On ||
                            cfg.effectNightStars == TriStateMode::On || cfg.effectAirQuality == TriStateMode::On ||
                            cfg.effectPollen == TriStateMode::On || cfg.effectFloodWater == TriStateMode::On);
        rows.push_back({
            L"Intensity",
            anyManualFx ? buf : L"",
            false, false, true, L"", L"", L"", D2D1::ColorF(0, 0, 0, 0),
            anyManualFx ? HudFieldId::EffectsIntensity : HudFieldId::None
        });

        auto addFxRow = [&](const wchar_t* name, TriStateMode m, bool providerVal, bool hasProvData, const wchar_t* act) {
            auto b = triBadge(m);
            std::wstring provState = (snap.valid && hasProvData) ? (providerVal ? L"active" : L"none") : L"--";
            rows.push_back({name, provState, false, false, true, L"", act, b.first, b.second, HudFieldId::None});
        };
        addFxRow(L"Dust", cfg.effectDust, snap.effects.dust, snap.mWind.available || snap.mVisibility.available, L"cycle_fx_dust");
        addFxRow(L"Fog", cfg.effectFog, snap.effects.fog, snap.mCondition.available || snap.mHumidity.available, L"cycle_fx_fog");
        addFxRow(L"Smog", cfg.effectSmog, snap.effects.smog, snap.mWind.available || snap.mVisibility.available, L"cycle_fx_smog");
        addFxRow(L"Thunderstorm", cfg.effectThunderstorm, snap.effects.thunderstorm, snap.mCondition.available, L"cycle_fx_storm");
        addFxRow(L"Aurora", cfg.effectAurora, snap.effects.aurora, snap.mClouds.available, L"cycle_fx_aurora");
        addFxRow(L"Mirage", cfg.effectMirage, snap.effects.mirage, snap.mTemp.available, L"cycle_fx_mirage");
        addFxRow(L"Rainbow", cfg.effectRainbow, snap.effects.rainbow, snap.mCondition.available || snap.mClouds.available, L"cycle_fx_rainbow");
        addFxRow(L"Wind streams", cfg.effectWindStreams, snap.effects.windStreams, snap.mWind.available || snap.mGusts.available, L"cycle_fx_wind");
        addFxRow(L"Frost / Heat", cfg.effectThermal, snap.effects.frost || snap.effects.heat, snap.mTemp.available || snap.mFeelsLike.available, L"cycle_fx_thermal");
        addFxRow(L"Night stars", cfg.effectNightStars, snap.effects.nightStars, snap.mClouds.available, L"cycle_fx_stars");
        addFxRow(L"Air quality", cfg.effectAirQuality, snap.effects.airQuality, snap.mAqi.available, L"cycle_fx_aq");
        addFxRow(L"Pollen", cfg.effectPollen, snap.effects.pollen, snap.mPollen.available, L"cycle_fx_pollen");
        addFxRow(L"Flood water", cfg.effectFloodWater, snap.effects.floodWater, snap.mPrecip.available, L"cycle_fx_flood");
    }

    const wchar_t* perfStr = (cfg.performanceMode == PerformanceMode::High)
        ? L"HIGH"
        : ((cfg.performanceMode == PerformanceMode::Medium) ? L"MEDIUM" : L"LOW");
    rows.push_back({L"Performance", L"", true, false, true, L"", L"cycle_perf", perfStr, D2D1::ColorF(0.30f, 0.85f, 0.65f, 0.9f), HudFieldId::None});

    std::vector<InteractiveControl> nextControls;
    std::wstring activeTooltip;
    float tooltipX = 0.0f;
    float tooltipY = 0.0f;

    if (expanded) {
        const float padX = 12.0f;
        const float padY = 10.0f;
        const float rowH = std::max(22.0f, cfg.hudFontSizePx + 11.0f);
        const float headerH = std::max(24.0f, cfg.hudFontSizePx + 13.0f);
        const float pillW = std::max(108.0f, cfg.hudFontSizePx * 9.0f);
        const float pillH = std::max(18.0f, cfg.hudFontSizePx + 6.0f);
        const float infoSize = std::max(15.0f, cfg.hudFontSizePx + 3.0f);
        const float valW = std::max(132.0f, cfg.hudFontSizePx * 11.5f);
        const float labelW = std::max(118.0f, cfg.hudFontSizePx * 10.2f);
        const float gap = static_cast<float>(cfg.forecastHudMarginPx);
        float cardW = padX * 2.0f + labelW + valW + pillW + 24.0f;
        float totalContentH = padY * 2.0f;
        for (const auto& r : rows) {
            totalContentH += r.isHeader ? headerH : rowH;
        }
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

        int maxScroll = needsScroll ? static_cast<int>(std::ceil(totalContentH - cardH)) : 0;
        int scrollPx = 0;
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
        D2D1_COLOR_F sepCol = cfg.colorHudText;
        sepCol.a = 0.14f;
        D2D1_COLOR_F mutedText = cfg.colorHudText;
        mutedText.a = std::clamp(cfg.colorHudText.a * 0.78f, 0.42f, 0.92f);

        float curY = top + padY - static_cast<float>(scrollPx);
        for (size_t i = 0; i < rows.size(); ++i) {
            const auto& r = rows[i];
            float h = r.isHeader ? headerH : rowH;
            if (curY + h >= top && curY <= top + cardH) {
                if (r.isHeader && i > 0) {
                    brush->SetColor(sepCol);
                    rt->DrawLine(D2D1::Point2F(left + padX, curY - 2.0f), D2D1::Point2F(left + cardW - padX, curY - 2.0f), brush, 1.0f);
                }
                float rowAlphaMul = r.available ? 1.0f : 0.50f;
                IDWriteTextFormat* fmt = r.isHeader
                    ? g_hudTitleFormat
                    : (r.available ? g_hudBodyFormat : g_hudItalicFormat);
                D2D1_COLOR_F lblCol = r.isHeader ? cfg.colorHudText : mutedText;
                lblCol.a *= rowAlphaMul;
                brush->SetColor(lblCol);
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
                } else if (r.isMetric) {
                    D2D1_RECT_F infoRect = D2D1::RectF(cursorX - infoSize, curY + (h - infoSize) * 0.5f, cursorX, curY + (h + infoSize) * 0.5f);
                    bool isHovered = (hoverX >= infoRect.left && hoverX <= infoRect.right &&
                                      hoverY >= infoRect.top && hoverY <= infoRect.bottom);
                    DrawInfoBadgeIcon(rt, brush, infoRect, lblCol, isHovered);
                    InteractiveControl ic;
                    ic.rect = infoRect;
                    ic.tooltipText = r.tooltip;
                    nextControls.push_back(ic);
                    if (isHovered && !r.tooltip.empty()) {
                        activeTooltip = r.tooltip;
                        tooltipX = infoRect.left;
                        tooltipY = infoRect.top;
                    }
                    cursorX -= (infoSize + 8.0f);
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
                    D2D1_COLOR_F valCol = cfg.colorHudText;
                    valCol.a *= rowAlphaMul;
                    brush->SetColor(valCol);
                    IDWriteTextFormat* valFmt = r.available ? g_hudBodyFormat : g_hudItalicFormat;
                    valFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    rt->DrawTextW(dispVal.c_str(), static_cast<UINT32>(dispVal.size()), valFmt,
                                  D2D1::RectF(valRect.left + 4.0f, curY, valRect.right - 4.0f, curY + h), brush);
                    valFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                }
            }
            curY += h;
        }
        if (needsScroll && maxScroll > 0) {
            float trackTop = top + 8.0f;
            float trackH = cardH - 16.0f;
            float thumbH = std::max(24.0f, trackH * (cardH / totalContentH));
            float thumbTop = trackTop + (trackH - thumbH) * (static_cast<float>(scrollPx) / static_cast<float>(maxScroll));
            brush->SetOpacity(1.0f);
            brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.32f));
            rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(left + cardW - 7.0f, thumbTop, left + cardW - 3.0f, thumbTop + thumbH), 2.0f, 2.0f), brush);
        }
        rt->PopAxisAlignedClip();

        if (!activeTooltip.empty()) {
            float tipW = std::max(145.0f, static_cast<float>(activeTooltip.size()) * (cfg.hudFontSizePx * 0.58f) + 18.0f);
            float tipH = std::max(22.0f, cfg.hudFontSizePx + 10.0f);
            float tipLeft = std::clamp(tooltipX - tipW + 12.0f, primaryMon.left + 6.0f, primaryMon.left + primaryMon.width - tipW - 6.0f);
            float tipTop = std::max(primaryMon.top + 6.0f, tooltipY - tipH - 6.0f);
            D2D1_ROUNDED_RECT tipRr = D2D1::RoundedRect(D2D1::RectF(tipLeft, tipTop, tipLeft + tipW, tipTop + tipH), 6.0f, 6.0f);
            brush->SetOpacity(1.0f);
            brush->SetColor(D2D1::ColorF(0.05f, 0.07f, 0.11f, 0.96f));
            rt->FillRoundedRectangle(tipRr, brush);
            brush->SetColor(D2D1::ColorF(0.45f, 0.78f, 1.0f, 0.85f));
            rt->DrawRoundedRectangle(tipRr, brush, 1.0f);
            brush->SetColor(cfg.colorHudText);
            rt->DrawTextW(activeTooltip.c_str(), static_cast<UINT32>(activeTooltip.size()), g_hudPillFormat,
                          D2D1::RectF(tipLeft + 4.0f, tipTop, tipLeft + tipW - 4.0f, tipTop + tipH), brush);
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudControls = std::move(nextControls);
    }

    brush->SetOpacity(1.0f);
    brush->SetColor(cfg.colorHudBg);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(btnCx, btnCy), btnR, btnR), brush);
    brush->SetColor(isDragging
        ? D2D1::ColorF(0.35f, 0.78f, 1.0f, 0.90f)
        : D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.50f));
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(btnCx, btnCy), btnR, btnR), brush, isDragging ? 2.2f : 1.6f);

    if (showGearIcon) {
        DrawGearSettingsIcon(rt, brush, btnCx, btnCy, btnR, cfg.colorHudText);
    } else {
        brush->SetColor(cfg.colorHudText);
        rt->DrawTextW(labelText.c_str(), static_cast<UINT32>(labelText.size()), g_hudCircleFormat,
                      D2D1::RectF(btnCx - btnR, btnCy - btnR, btnCx + btnR, btnCy + btnR), brush);
    }
}

void RenderFrame(const Settings& cfg, const ForecastSnapshot& snap, const GeneralEffectsState& fx,
                 float cloudPct01, float uvIndex, double moonPhase01,
                 int bottomLimitY, float precipVx, double timeSec, bool forceArcDirty,
                 bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    outCursorNearSky = false;
    if (!g_overlayHwnd || !EnsureSurface(g_surface, g_canvasWidth, g_canvasHeight, cfg.colorSnow, cfg.hasCustomSnowColor)) {
        return;
    }
    float bottomRelY = static_cast<float>(bottomLimitY - g_canvasTop);
    RECT bindRect = {0, 0, g_canvasWidth, g_canvasHeight};
    g_surface.rt->BindDC(g_surface.memDC, &bindRect);
    g_surface.rt->BeginDraw();
    g_surface.rt->Clear(D2D1::ColorF(0, 0, 0, 0));

    float k = static_cast<float>(cfg.effectsIntensity) / 25.0f;
    float hFloat = static_cast<float>(g_canvasHeight);
    double hour = GetLocalHourFloat();
    bool isNightClock = (hour < 5.5 || hour >= 19.5);
    if (fx.nightStars) {
        DrawNightStarsLayer(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec);
    }

    float cloudDimFactor = cfg.celestialInFrontOfClouds ? 1.0f : (1.0f - cloudPct01 * 0.65f);
    bool sunOn = (cfg.sunMode == TriStateMode::On) ||
                 (cfg.sunMode == TriStateMode::Auto && !isNightClock && (cfg.primarySource == L"off" || !snap.valid || snap.isDay));
    bool moonOn = (cfg.moonMode == TriStateMode::On) ||
                  (cfg.moonMode == TriStateMode::Auto && (isNightClock || (cfg.primarySource != L"off" && snap.valid && !snap.isDay)));

    if (!cfg.celestialInFrontOfClouds) {
        DrawSunLayer(g_surface.rt, g_surface.sharedBrush, cfg, sunOn, hour, snap, uvIndex, cloudDimFactor, timeSec, hasCursor, curRelX, curRelY, outCursorNearSky);
        DrawMoonLayer(g_surface.rt, g_surface.sharedBrush, cfg, moonOn, hour, snap, moonPhase01, cloudDimFactor, hasCursor, curRelX, curRelY, outCursorNearSky);
    }
    DrawDriftingCloudsLayer(g_surface.rt, g_surface.sharedBrush, cfg, cloudPct01, precipVx, timeSec, hasCursor, curRelX, curRelY, outCursorNearSky);
    if (cfg.celestialInFrontOfClouds) {
        DrawSunLayer(g_surface.rt, g_surface.sharedBrush, cfg, sunOn, hour, snap, uvIndex, cloudDimFactor, timeSec, hasCursor, curRelX, curRelY, outCursorNearSky);
        DrawMoonLayer(g_surface.rt, g_surface.sharedBrush, cfg, moonOn, hour, snap, moonPhase01, cloudDimFactor, hasCursor, curRelX, curRelY, outCursorNearSky);
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
    if (fx.airQuality) {
        DrawAirQualityLayer(g_surface.rt, g_surface.sharedBrush, cfg, snap, k, timeSec);
    }
    if (fx.frost || fx.heat) {
        DrawThermalLayer(g_surface.rt, fx.frost, fx.heat, k);
    }
    if (fx.aurora) {
        DrawSmoothAurora(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec);
    }
    if (fx.mirage) {
        DrawSmoothMirage(g_surface.rt, g_surface.sharedBrush, cfg, k, bottomRelY, timeSec);
    }
    if (fx.rainbow) {
        DrawSmoothRainbow(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec, forceArcDirty);
    }
    if (fx.windStreams) {
        DrawWindStreamsLayer(g_surface.rt, g_surface.sharedBrush, cfg, snap, k, timeSec);
    }
    if (fx.thunderstorm && g_lightning.timer > 0.0f) {
        D2D1_COLOR_F tc = cfg.hasCustomThunderstormColor ? cfg.colorThunderstorm : D2D1::ColorF(0.86f, 0.93f, 1.0f, 0.95f);
        float tAlpha = std::clamp(g_lightning.timer * 4.2f * std::max(0.25f, k), 0.0f, 1.0f);
        g_surface.sharedBrush->SetColor(D2D1::ColorF(tc.r, tc.g, tc.b, tAlpha));
        DrawBranchedLightning(g_surface.rt, g_surface.sharedBrush, cfg, g_lightning, bottomRelY, tAlpha);
    }
    for (const auto& p : g_falling) {
        DrawItemShape(g_surface, cfg, p, precipVx, bottomRelY, timeSec);
    }
    if (fx.pollen) {
        DrawPollenLayer(g_surface.rt, g_surface.sharedBrush, cfg, k, timeSec);
    }
    if (fx.floodWater) {
        DrawFloodWaterLayer(g_surface.rt, g_surface.sharedBrush, cfg, snap, k, timeSec);
    }
    DrawForecastHudWidget(g_surface.rt, g_surface.sharedBrush, cfg, snap, uvIndex, moonPhase01, bottomLimitY);

    if (FAILED(g_surface.rt->EndDraw())) {
        CleanupSurface(g_surface);
        return;
    }
    PresentSurface(g_overlayHwnd, g_surface);
}

float ComputeHorizontalSpeedPxPerSec(FallDirMode dirMode, double mpsSpeed, int forecastSign, double forecastWindKmh) {
    float basePx = static_cast<float>(mpsSpeed) * 26.0f;
    if (dirMode == FallDirMode::Down) {
        return 0.0f;
    }
    if (dirMode == FallDirMode::DownLeft) {
        return -basePx;
    }
    if (dirMode == FallDirMode::DownRight) {
        return basePx;
    }
    if (forecastSign == 0 || forecastWindKmh <= 0.2) {
        return 0.0f;
    }
    return static_cast<float>(forecastSign) * basePx * std::clamp(static_cast<float>(forecastWindKmh) / 18.0f, 0.30f, 2.8f);
}

float ComputeEffectiveFallSpeedMps(const Settings& cfg, const ForecastSnapshot& snap, ObjectKind kind) {
    if (cfg.precipSpeedMode == PrecipSpeedMode::Custom) {
        return static_cast<float>(cfg.precipFallSpeedMps);
    }
    double rainFactor = std::clamp((snap.valid ? snap.precipMm : 1.0) / 4.0, 0.45, 2.6);
    if (kind == ObjectKind::Snow) {
        return static_cast<float>(std::clamp(1.6 + rainFactor * 1.1, 1.2, 4.5));
    }
    if (kind == ObjectKind::Hail) {
        return static_cast<float>(std::clamp(11.0 + rainFactor * 5.0, 9.0, 22.0));
    }
    return static_cast<float>(std::clamp(5.5 + rainFactor * 4.2, 4.0, 16.0));
}

void WaitWithMessagePump(DWORD ms) {
    DWORD startTick = GetTickCount();
    while (g_running.load()) {
        DWORD elapsed = GetTickCount() - startTick;
        if (elapsed >= ms) {
            break;
        }
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
    auto startTime = std::chrono::steady_clock::now();
    auto lastTime = startTime;
    int prevLeft = INT32_MIN;
    int prevTop = INT32_MIN;
    int prevW = -1;
    int prevH = -1;
    bool prevCursorNearSky = false;

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
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) {
                ShowWindow(g_overlayHwnd, SW_HIDE);
            }
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

        bool hasCursor = false;
        float curRelX = 0.0f;
        float curRelY = 0.0f;
        POINT ptCur = {};
        if (GetCursorPos(&ptCur)) {
            hasCursor = true;
            curRelX = static_cast<float>(ptCur.x - g_canvasLeft);
            curRelY = static_cast<float>(ptCur.y - g_canvasTop);
            {
                std::lock_guard<std::mutex> hlock(g_hudUiMutex);
                g_hoverRelX = curRelX;
                g_hoverRelY = curRelY;
            }
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) {
                SyncClickThroughExStyle(g_overlayHwnd, IsRelPointInsideHud(curRelX, curRelY));
            }
        }

        auto now = std::chrono::steady_clock::now();
        float dt = std::clamp(std::chrono::duration<float>(now - lastTime).count(), 0.001f, 0.25f);
        double timeSec = std::chrono::duration<double>(now - startTime).count();
        lastTime = now;

        bool activePrecip = false;
        ObjectKind precipKind = ObjectKind::Rain;
        if (cfg.precipitationMode == PrecipMode::Rain) {
            activePrecip = true;
            precipKind = ObjectKind::Rain;
        } else if (cfg.precipitationMode == PrecipMode::Snow) {
            activePrecip = true;
            precipKind = ObjectKind::Snow;
        } else if (cfg.precipitationMode == PrecipMode::Hail) {
            activePrecip = true;
            precipKind = ObjectKind::Hail;
        } else if (cfg.precipitationMode == PrecipMode::Auto && snap.hasPrecip) {
            activePrecip = true;
            precipKind = snap.precipKind;
        }

        auto resolveFx = [&](TriStateMode m, bool autoVal) -> bool {
            return (m == TriStateMode::On) ? true : ((m == TriStateMode::Off) ? false : autoVal);
        };
        GeneralEffectsState activeFx;
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
        bool isNightClock = (hour < 5.5 || hour >= 19.5);

        float cloudPct01 = 0.0f;
        if (cfg.cloudsMode == TriStateMode::On) {
            cloudPct01 = static_cast<float>(cfg.cloudsIntensity) / 100.0f;
        } else if (cfg.cloudsMode == TriStateMode::Auto && snap.valid) {
            cloudPct01 = static_cast<float>(snap.cloudCoverPct / 100.0);
        }
        cloudPct01 = std::clamp(cloudPct01, 0.0f, 1.0f);

        bool isDaytime = !isNightClock && (cfg.primarySource == L"off" || !snap.valid || snap.isDay);
        bool sunActive = (cfg.sunMode == TriStateMode::On) || (cfg.sunMode == TriStateMode::Auto && isDaytime);
        bool moonActive = (cfg.moonMode == TriStateMode::On) || (cfg.moonMode == TriStateMode::Auto && !isDaytime);
        float uvIndex = (cfg.sunUvOverride >= 0.0) ? static_cast<float>(cfg.sunUvOverride) : static_cast<float>(snap.uvIndex);
        double moonPhase01 = (cfg.moonPhaseOverride >= 0.0) ? cfg.moonPhaseOverride : ComputeRealMoonPhase();

        bool hasAnyState = activePrecip || !g_falling.empty() || cfg.forecastHudEnabled ||
                           (cloudPct01 > 0.01f) || sunActive || moonActive ||
                           activeFx.dust || activeFx.fog || activeFx.smog ||
                           activeFx.thunderstorm || activeFx.aurora || activeFx.mirage ||
                           activeFx.rainbow || activeFx.windStreams || activeFx.frost ||
                           activeFx.heat || activeFx.nightStars || activeFx.airQuality ||
                           activeFx.pollen || activeFx.floodWater;

        if (!hasAnyState) {
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) {
                ShowWindow(g_overlayHwnd, SW_HIDE);
            }
            WaitWithMessagePump(200);
            lastTime = std::chrono::steady_clock::now();
            continue;
        }
        if (g_overlayHwnd && !IsWindowVisible(g_overlayHwnd)) {
            ShowWindow(g_overlayHwnd, SW_SHOWNA);
            PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode, true);
        }

        if (activeFx.thunderstorm) {
            float flashProb = (cfg.performanceMode == PerformanceMode::High)
                ? 0.45f
                : ((cfg.performanceMode == PerformanceMode::Low) ? 0.20f : 0.35f);
            if (g_lightning.timer > 0.0f) {
                g_lightning.timer = std::max(0.0f, g_lightning.timer - dt);
            } else if ((static_cast<float>(rng() % 10000) / 10000.0f) < flashProb * dt) {
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
        if (g_falling.size() != targetP) {
            g_falling.resize(targetP);
        }
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

        bool cursorNearSky = false;
        RenderFrame(cfg, snap, activeFx, cloudPct01, uvIndex, moonPhase01,
                    ComputeBottomLimitY(cfg), windCloudVx, timeSec, forceArcDirty,
                    hasCursor, curRelX, curRelY, cursorNearSky);

        bool hoverTransitioning = cursorNearSky || prevCursorNearSky;
        prevCursorNearSky = cursorNearSky;

        bool hasFastMotion = !g_falling.empty() || activeFx.thunderstorm || activeFx.aurora || activeFx.mirage ||
                             activeFx.windStreams || activeFx.nightStars || activeFx.airQuality ||
                             activeFx.pollen || activeFx.floodWater || g_hudExpanded.load() || hoverTransitioning;
        bool hasSlowMotion = (cloudPct01 > 0.01f) || sunActive || moonActive;
        DWORD waitMs = hasFastMotion
            ? static_cast<DWORD>(1000 / cfg.targetFps)
            : (hasSlowMotion ? 300 : 500);
        WaitWithMessagePump(waitMs);
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

// The mod is going to be injected into windhawk.exe, which is a tool mod
// process. Create a process with the same command line, passing the
// `-tool-mod` parameter, which will be injected into a new non-tool-mod
// process, and then exit the tool mod process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");

    // Exit the tool mod process.
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

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
        g_toolModProcessMutex = CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod process is already running");
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRva = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRva;
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

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hNewToken);

    HMODULE kernelbaseModule = GetModuleHandleW(L"kernelbase.dll");
    auto pCreateProcessInternalW = kernelbaseModule
        ? reinterpret_cast<CreateProcessInternalW_t>(GetProcAddress(kernelbaseModule, "CreateProcessInternalW"))
        : nullptr;

    BOOL created = pCreateProcessInternalW
        ? pCreateProcessInternalW(nullptr, windhawkPath, commandLine, nullptr, nullptr, FALSE,
                                  NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)
        : CreateProcessW(windhawkPath, commandLine, nullptr, nullptr, FALSE,
                         NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);
    if (!created) {
        Wh_Log(L"CreateProcess failed: %u", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModSettingsChanged();
    }
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}