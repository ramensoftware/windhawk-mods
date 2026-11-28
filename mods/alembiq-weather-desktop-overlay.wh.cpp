// ==WindhawkMod==
// @id              alembiq-weather-desktop-overlay
// @name            Desktop Weather & Particles Overlay
// @description     Atmospheric effects and live weather forecast visualization for the Windows desktop (Direct2D overlay)
// @version         3.2.9
// @author          alembiq
// @github          https://github.com/alembiqspace
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lwininet -luser32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
A lightweight **Direct2D** weather and atmosphere overlay for the Windows desktop. It brings live weather precipitation (`auto`, `drizzle`, `rain`, `downpour`, `sleet`, `snow`, `blizzard`, `hail`), procedural multi-lobe clouds in `high` mode, a top-edge cloud-cover band that breathes in and out without wind drift, an optional ambient sky phase tint across night, blue hour, sunrise/sunset, and golden hour, a UV-reactive sun with smooth dynamic gradient cone rays in `high` mode, a real-phase moon with surface shading, parallel sun/moon sky presence with dawn/dusk horizon glow, solar/lunar/astronomical/space sources (SunriseSunset.io, NASA NeoWs, NASA DONKI, Open-Meteo, Yr.no, 7Timer! and more), particle & edge-based atmospheric effects, window collision physics and a draggable temperature button with an interactive visual-card HUD (`Meteo` / `Astro`, `Now` / `Hours` / `Days`).

### Preview
![Desktop Weather & Particles Overlay preview](https://raw.githubusercontent.com/alembiqspace/alembiq-weather-desktop-overlay/main/preview.gif)

### Features
- **Two-Window Desktop Model:** The full-screen weather/atmosphere overlay stays 100% click-through and placed below desktop icons according to `layerAttachMode`. The draggable temperature button and HUD panel live in a separate compact window on the desktop (above desktop icons, below normal app windows).
- **Operating Modes (`[System]`):** `full_screen_button` (desktop overlay + HUD window), `full_screen_only` (overlay without HUD), or `button_only` (HUD window only, no full-screen overlay created).
- **Adaptive Frame Rate & Performance (`low`, `medium`, `high`, `custom`):** Uses `targetFps` (30/60) only for falling precipitation, lightning, open HUD, dragging, or hover transitions; ~18 FPS for animated `medium`/`high` sun rays; ~3 FPS (300 ms) for slow/static layers.
- **DPI-Aware Scaling & Insets (`[System]`):** Scales HUD, button, and particle/cloud sizes by monitor DPI; `screenInsetsPx` (`top/right/bottom/left`) clamps effects and collision bounds per monitor.
- **Visual Card HUD (`[HUD]`):** Fixed-height unified layout across `Meteo` (`Now` / `Hours` / `Days`), `Astro`, `Settings`, and detail tables; persists dragged button position and selected astro source across restarts.

### Network & Privacy
- **Off by default:** `coords` is empty (`""`) and `nasaApiKey` is empty (`""`) by default. No network requests are made until you enter valid `lat/lon` coordinates, configure `customApiUrl` / `localWeatherPath`, or explicitly select a NASA source.
- **What is sent and where:**
  - `api.open-meteo.com`, `air-quality-api.open-meteo.com`, `api.sunrisesunset.io`, `api.met.no`, `www.7timer.info` – receive your configured `lat/lon` coordinates via HTTPS GET. Each provider is queried at most once per refresh.
  - `api.weatherapi.com`, `api.openweathermap.org`, `weather.visualcrossing.com`, `api.tomorrow.io` – receive your `lat/lon` coordinates and your API key, only if you enter a key for that provider and fields are still missing.
  - `api.nasa.gov` (NeoWs) – receives only today's UTC date and your `nasaApiKey`, only when you enter your own NASA API key and select or request NASA data.
  - `ccmc.gsfc.nasa.gov` (DONKI) – receives only a 2-day UTC date range (no key, no coordinates), only when explicitly selected as a source.
  - `customApiUrl` – HTTP/HTTPS GET to your custom URL with `{lat}`, `{lon}`, `{key}` substituted (skipped if `{lat}`/`{lon}` are present while `coords` is empty).
  - `localWeatherPath` – read-only local file access (up to 1 MB, no network).
- **When requests run:** On startup (if configured), every `updateIntervalMin` minutes, every 60 seconds if all providers fail, and immediately when coordinates/source/key settings change or you click `SYNC` in the HUD.
- **How to disable:** Leave `coords` empty, or set `primarySource` to `off` (and `Astro` source to `off`).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# --- 1. SYSTEM & LAYER ---
- forecastHudMode: full_screen_button
  $name: "[System] Widget operating mode"
  $description: "full_screen_button = desktop overlay + HUD window; full_screen_only = desktop overlay only; button_only = HUD window only."
  $options:
    - full_screen_button: Full screen + button
    - full_screen_only: Full screen only
    - button_only: Button only
- windowBounceEnabled: true
  $name: "[System] Window collision physics (full screen)"
  $description: "Top edge plays a brief hit animation and despawns the particle; side edges deflect particles downward. Default: true."
- layerAttachMode: auto
  $name: "[System] Layer attach mode"
  $description: "Controls Z-order placement of the full-screen weather overlay. Allowed: auto, progman_direct, desktop_bottom. Default: auto."
  $options:
    - auto: Auto (below desktop icons)
    - progman_direct: Progman reference
    - desktop_bottom: Bottom of Z-order (HWND_BOTTOM)
- performanceMode: high
  $name: "[System] Performance mode"
  $description: "Scales visual detail and particle density. Allowed: low, medium, high, custom. Default: high."
  $options:
    - low: Low (fastest)
    - medium: Medium (balanced)
    - high: High (ultra visual detail)
    - custom: Custom (per-category quality)
- perfCloudsMode: high
  $name: "[System - Custom] Clouds quality"
  $description: "Quality level for clouds when performanceMode is custom. Allowed: low, medium, high. Default: high."
  $options:
    - low: Low
    - medium: Medium
    - high: High
- perfCelestialMode: high
  $name: "[System - Custom] Sun & Moon quality"
  $description: "Quality level for sun and moon when performanceMode is custom. Allowed: low, medium, high. Default: high."
  $options:
    - low: Low
    - medium: Medium
    - high: High
- perfPrecipMode: high
  $name: "[System - Custom] Precipitation quality"
  $description: "Quality level for precipitation when performanceMode is custom. Allowed: low, medium, high. Default: high."
  $options:
    - low: Low
    - medium: Medium
    - high: High
- perfEffectsMode: high
  $name: "[System - Custom] Effects quality"
  $description: "Quality level for general weather effects when performanceMode is custom. Allowed: low, medium, high. Default: high."
  $options:
    - low: Low
    - medium: Medium
    - high: High
- bottomBoundaryMode: above_taskbar
  $name: "[System] Bottom boundary mode"
  $description: "Sets bottom reference line for ground effects and bottom HUD placement. Allowed: above_taskbar, screen_bottom, custom_offset. Default: above_taskbar."
  $options:
    - above_taskbar: Above taskbar (Shell_TrayWnd)
    - screen_bottom: Bottom edge of screen
    - custom_offset: Custom offset from bottom (px)
- screenInsetsPx: "0/0/10/0"
  $name: "[System] Screen insets top/right/bottom/left (px)"
  $description: "Format: top/right/bottom/left in pixels (0..1000 each). Bottom inset applies in custom_offset mode. Default: 0/0/10/0."
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
  $description: "Pauses rendering while a fullscreen or maximized app covers the desktop. Default: true."

# --- 2. WEATHER, ASTRONOMY SOURCES & UNITS ---
- primarySource: open_meteo
  $name: "[Weather] Primary source"
  $description: "Primary provider queried first; remaining missing fields are automatically aggregated from other available providers. Default: open_meteo."
  $options:
    - open_meteo: Open-Meteo API
    - met_norway: MET Norway (Yr.no)
    - seven_timer: 7Timer! (civil)
    - sunrise_sunset_io: SunriseSunset.io (free, no key)
    - nasa_neows: NASA NeoWs - Near-Earth Asteroids (*)
    - nasa_donki: NASA DONKI - Solar Flares & Storms
    - weatherapi: WeatherAPI.com (*)
    - openweathermap: OpenWeatherMap (*)
    - visual_crossing: Visual Crossing (*)
    - tomorrow_io: Tomorrow.io (*)
    - custom_api: Custom JSON URL
    - local_file: Local file
    - off: Off (disable forecast fetching)
- nasaApiKey: ""
  $name: "[Weather] NASA API key (*)"
  $description: "Personal API key for NASA NeoWs (empty by default; get a free key at api.nasa.gov)."
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
- humidityUnit: percent
  $name: "[Units] Humidity"
  $description: "Relative humidity unit in HUD."
  $options:
    - percent: Percent (%)
    - sponge_ratio: Sponge saturation ratio (0..1)
    - hair_frizz: Hair frizz index (%)
    - steam_room: Steam room proximity (%)
- pressureUnit: hpa
  $name: "[Units] Pressure"
  $description: "Atmospheric pressure unit in HUD."
  $options:
    - hpa: Hectopascals (hPa)
    - mmhg: Millimeters of mercury (mmHg)
    - inhg: Inches of mercury (inHg)
    - barometer_delta: Delta vs 1013.25 hPa
    - ear_pop: Ear pop score
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
- pollenUnit: index
  $name: "[Units] Pollen"
  $description: "Pollen index unit in HUD."
  $options:
    - index: Standard pollen index
    - sneeze_risk: Sneeze risk (%)
    - tissue_pack: Tissue packs / day
- visibilityUnit: km
  $name: "[Units] Visibility"
  $description: "Visibility distance unit in HUD."
  $options:
    - km: Kilometers (km)
    - meters: Meters (m)
    - miles: Miles (mi)
    - horizon_reach: Horizon reach ratio
    - blind_buff_hud: Fog-of-war view block (%)
- dayNightUnit: label
  $name: "[Units] Day / Night"
  $description: "Day/Night state label style in HUD."
  $options:
    - label: Day / Night
    - solar_state: Solar / Lunar
    - vampire_shift: Daylight / Nightfall

# --- 3. FLOATING TEMPERATURE BUTTON & TABLE ---
- hudButtonRadiusPx: 24
  $name: "[HUD] Button radius (px)"
  $description: "Circular temperature button radius in pixels (14 to 96). Default: 24."
- hudFontSizePx: 12
  $name: "[HUD] Base font size (px)"
  $description: "Base (1x) font size in pixels for HUD cards; medium callouts scale to 2x and hero temperature scales to 3x (9 to 20). Default: 12."
- hudButtonGeometry: "92/88"
  $name: "[HUD] Button center X/Y (%)"
  $description: "Format: centerX/centerY in % (0..100; X left->right, Y bottom->top). Default: 92/88."
- forecastHudMarginPx: 14
  $name: "[HUD] Gap & screen margin (px)"
  $description: "Gap between HUD panel and button / screen edges in pixels (0 to 400). Invalid resets to 14."
- forecastHudOpacity: 86
  $name: "[HUD] Background opacity (%)"
  $description: "Button and panel background opacity (15 to 100). Invalid resets to 86."

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
- topCloudCoverEnabled: false
  $name: "[Sky] Top-edge cloud cover band"
  $description: "Fills the top edge of monitors according to cloud cover percentage without wind drift, with dynamic fade-in/fade-out. Default: false."
- skyAmbientTintEnabled: false
  $name: "[Sky] Ambient sky phase tint"
  $description: "Subtle full-screen tint shifting across night, blue hour, sunrise/sunset, and golden hour (transparent during daytime). Default: false."
- skyAmbientTintOpacity: 15
  $name: "[Sky] Ambient tint intensity (%)"
  $description: "Maximum opacity for the ambient sky tint layer (0 to 50). Invalid/out-of-range resets to 15."
- celestialMonitorMode: primary
  $name: "[Sky] Sun & Moon monitors"
  $description: "Show sun and moon on primary monitor or all monitors. Allowed: primary, all. Default: primary."
  $options:
    - primary: Primary monitor
    - all: All monitors
- celestialTimelineMode: auto_forecast
  $name: "[Sky] Sun & Moon timeline"
  $description: "Time source for Sun and Moon schedules and positions. Default: auto_forecast."
  $options:
    - auto_forecast: auto (forecast)
    - auto_system: auto (system clock)
    - custom: custom
- customSunTimes: "06:30/18:30"
  $name: "[Sky] Custom sunrise/sunset (HH:MM/HH:MM)"
  $description: "Custom sunrise and sunset times in 24h format when timeline is custom (or fallback). Default: 06:30/18:30."
- customMoonTimes: "20:00/07:00"
  $name: "[Sky] Custom moonrise/moonset (HH:MM/HH:MM)"
  $description: "Custom moonrise and moonset times in 24h format when timeline is custom (or fallback). Default: 20:00/07:00."
- sunMode: auto
  $name: "[Sky] Sun mode"
  $description: "Sun disc, rays & glow with UV-index color shift. Allowed: off, auto, on. Default: auto."
  $options:
    - off: Off
    - auto: Auto (active schedule window)
    - on: On
- sunMotionMode: arc_ltr
  $name: "[Sky] Sun movement"
  $description: "Path shape and direction for the Sun. Default: arc_ltr."
  $options:
    - line_ltr: line (left to right)
    - line_rtl: line (right to left)
    - arc_ltr: arc (left to right)
    - arc_rtl: arc (right to left)
- sunGeometry: "7-82/82"
  $name: "[Sky] Sun radius-center X/Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius sets size in all modes). Default: 7-82/82."
- moonMode: auto
  $name: "[Sky] Moon mode"
  $description: "Moon disc, surface shading & glow with real lunar phase. Allowed: off, auto, on. Default: auto."
  $options:
    - off: Off
    - auto: Auto (active schedule window)
    - on: On
- moonMotionMode: arc_ltr
  $name: "[Sky] Moon movement"
  $description: "Path shape and direction for the Moon. Default: arc_ltr."
  $options:
    - line_ltr: line (left to right)
    - line_rtl: line (right to left)
    - arc_ltr: arc (left to right)
    - arc_rtl: arc (right to left)
- moonGeometry: "6-82/82"
  $name: "[Sky] Moon radius-center X/Y"
  $description: "Format: radius-centerX/centerY in % (0..100; radius sets size in all modes). Default: 6-82/82."
- celestialInFrontOfClouds: false
  $name: "[Sky] Render sun & moon in front of clouds"
  $description: "When false (default), clouds dim/occlude the sun and moon. When true, sun and moon render in front of clouds with full brightness."

# --- 5. PRECIPITATION ---
- precipitationMode: auto
  $name: "[Precipitation] Mode"
  $description: "Precipitation type: auto, drizzle, rain, downpour, sleet, snow, blizzard, hail, or off. Default: auto."
  $options:
    - off: Off
    - auto: Auto (forecast)
    - drizzle: Drizzle (light mist/drops)
    - rain: Rain
    - downpour: Downpour (heavy rain)
    - sleet: Sleet (rain + snow)
    - snow: Snow
    - blizzard: Blizzard (wind-driven snow)
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
  $description: "auto = scale speed by forecast precipitation intensity & kind; custom = use fall speed below."
  $options:
    - auto: Auto (forecast)
    - custom: Custom
- precipFallSpeedMps: "6.5"
  $name: "[Precipitation] Fall speed (m/s)"
  $description: "Custom fall speed in m/s when speed mode is custom (0.1 to 50.0). Invalid/out-of-range resets to 6.5."
- precipIntensity: 25
  $name: "[Precipitation] Intensity (%)"
  $description: "Shared intensity for precipitation types (0..100). Any value outside 0..100 or invalid resets to 25."

# --- 6. EFFECTS ---
- effectsIntensity: 25
  $name: "[Effects] General intensity (%)"
  $description: "Strength/opacity for effects (0..100). Outside 0..100 or invalid resets to 25."
- effectDust: auto
  $name: "[Effects] Dust"
  $description: "Airborne dust motes & low ground haze (auto triggers on strong dry wind or low visibility). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectFog: auto
  $name: "[Effects] Fog"
  $description: "Low-lying mist banks along bottom edge (auto triggers on fog code or high humidity with low visibility). Allowed: auto, on, off."
  $options:
    - auto: Auto
    - on: On
    - off: Off
- effectSmog: auto
  $name: "[Effects] Smog"
  $description: "Low smog band & drifting soot motes (auto triggers on calm wind with reduced visibility/haze). Allowed: auto, on, off."
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
- effectThermal: auto
  $name: "[Effects] Frost / Heat tint"
  $description: "Subtle frost perimeter rim at <= 0 C or bottom heat shimmer at >= 30 C apparent temperature. Allowed: auto, on, off."
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
  $description: "Suspended air quality micro-particles (auto triggers on elevated AQI / PM2.5). Allowed: auto, on, off."
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

# --- 7. COLORS (CONFIG ONLY, '/' LISTS) ---
- skyPrecipColors: "#6BB8FF/#FBFDFF/#C2E6FF/#E6ECF7/#FFD152/#DCE7FF"
  $name: "[Sky + Precipitation] Colors (#RRGGBB/...)"
  $description: "6 HEX colors separated by '/': rain/snow/hail/clouds/sun/moon (empty segment = default)."
- effectsColors: "#D6AE73/#D1D9E0/#8F7F6B/#D6EBFF/#38EB9E/#FADBA8/#D1A857/#F7E36B"
  $name: "[Effects] Colors (#RRGGBB/...)"
  $description: "8 HEX colors separated by '/': dust/fog/smog/thunderstorm/aurora/mirage/airQuality/pollen (empty segment = default)."
- colorHudBg: "#121824"
  $name: "[HUD] Background (#RRGGBB)"
  $description: "Custom HEX (#RRGGBB) or empty for default dark slate card background. Invalid HEX resets to default."
- colorHudText: "#F2F6FC"
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
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr wchar_t kOverlayWndClass[] = L"WindhawkDesktopWeatherOverlayClass";
constexpr wchar_t kHudWndClass[] = L"WindhawkDesktopWeatherHudClass";
constexpr double kSynodicMonthDays = 29.530588853;
constexpr double kReferenceNewMoonEpochSec = 947182440.0;
constexpr double kPi = 3.14159265358979323846;

enum class PrecipKind { None, Drizzle, Rain, Downpour, Sleet, Snow, Blizzard, Hail };
enum class PerformanceMode { Low, Medium, High, Custom };
enum class QualityTier { Low, Medium, High };
enum class HudWidgetMode { FullScreenButton, FullScreenOnly, ButtonOnly };
enum class HudCategory { Meteo, Astro };
enum class HudDataTab { Now, Hours, Days };
enum class PrecipMode { Off, Auto, Drizzle, Rain, Downpour, Sleet, Snow, Blizzard, Hail };
enum class PrecipSpeedMode { Auto, Custom };
enum class TriStateMode { Auto, On, Off };
enum class CelestialMotionMode { LineLtr, LineRtl, ArcLtr, ArcRtl };
enum class CelestialTimelineMode { AutoForecast, AutoSystem, Custom };
enum class CelestialMonitorMode { Primary, All };
enum class FallDirMode { Auto, Down, DownLeft, DownRight };
enum class ExpandedSection { None, Sky, Precipitation, Effects, Performance };
enum class DetailWidgetKind {
    None,
    WeatherAll,
    RiseSet,
    NowHero,
    NowWindPrecip,
    NowAirVis,
    AstroSun,
    AstroMoon,
    AstroUvEmi
};

enum class TempUnit { Celsius, Fahrenheit, Kelvin, BodyRatio, WaterFreeze, PizzaOven };
enum class HumidityUnit { Percent, SpongeRatio, HairFrizz, SteamRoom };
enum class PressureUnit { Hpa, Mmhg, Inhg, BarometerDelta, EarPop };
enum class WindUnit { Kmh, Ms, Mph, Knots, Beaufort, BreezeRatio };
enum class CloudUnit { Percent, Oktas, BlanketRatio, SkyClear };
enum class PrecipUnit { Mm, Inches, Microns, TeaSpoons, PuddleCm };
enum class UvUnit { Index, PercentMax, SunburnUnits, SpfHint };
enum class AqiUnit { Aqi, Pm25, CigEquiv, LungLoad };
enum class PollenUnit { Index, SneezeRisk, TissuePack };
enum class VisibilityUnit { Km, Meters, Miles, HorizonReach, BlindBuffHud };
enum class DayNightUnit { Label, SolarState, VampireShift };

struct GeneralEffectsState {
    bool dust = false;
    bool fog = false;
    bool smog = false;
    bool thunderstorm = false;
    bool aurora = false;
    bool mirage = false;
    bool frost = false;
    bool heat = false;
    bool nightStars = false;
    bool airQuality = false;
    bool pollen = false;
};

struct HourlyPoint {
    std::wstring timeLabel;
    int hourOfDay = 0;
    int64_t epochSec = 0;
    double tempC = 0.0;
    double tempMinC = 0.0;
    double tempMaxC = 0.0;
    double precipMm = 0.0;
    double windSpeedKmh = 0.0;
    double windDirDeg = 0.0;
    double pressureHpa = 1013.25;
    double uvIndex = 0.0;
    double aqiValue = 25.0;
    int weatherCode = 0;
};

struct DailyPoint {
    std::wstring dayLabel;
    int64_t epochSec = 0;
    double tempMinC = 0.0;
    double tempMaxC = 0.0;
    double precipMm = 0.0;
    double windSpeedKmh = 0.0;
    double windDirDeg = 0.0;
    double pressureHpa = 1013.25;
    double aqiValue = 25.0;
    double emiKp = 2.0;
    double sunriseHour = 6.5;
    double sunsetHour = 18.5;
    double moonriseHour = 20.0;
    double moonsetHour = 7.0;
    int weatherCode = 0;
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

struct MonitorCanvasRect {
    float left = 0.0f;
    float top = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float innerLeftRel = 0.0f;
    float innerTopRel = 0.0f;
    float innerRightRel = 0.0f;
    float bottomLimitRelY = 0.0f;
    float dpiScale = 1.0f;
};

struct WindowSurfaceBox {
    float left = 0.0f;
    float right = 0.0f;
    float top = 0.0f;
    float bottom = 0.0f;
};

struct ForecastSnapshot {
    bool valid = false;
    bool lastFetchFailed = false;
    int64_t lastSuccessEpochSec = 0;
    int weatherCode = -1;
    double tempC = 0.0;
    double todayMinC = 0.0;
    double todayMaxC = 0.0;
    bool hasTodayMinMax = false;
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
    double sunsetHour = 18.5;
    bool hasMoonTimes = false;
    double moonriseHour = 20.0;
    double moonsetHour = 7.0;
    bool hasGoldenHour = false;
    std::wstring goldenHourText;
    bool hasNeoData = false;
    int neoCountToday = 0;
    double neoClosestLd = 0.0;
    bool hasSpaceWeather = false;
    double flareKpEst = 2.0;
    std::wstring spaceWeatherText;
    bool isDay = true;
    PrecipKind precipKind = PrecipKind::None;
    GeneralEffectsState effects;

    std::vector<HourlyPoint> hourly;
    std::vector<DailyPoint> daily;
    MetricMeta mHourly;
    MetricMeta mDaily;

    MetricMeta mCondition;
    MetricMeta mTemp;
    MetricMeta mTodayMinMax;
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
    MetricMeta mSunTimes;
    MetricMeta mMoonTimes;
    MetricMeta mGoldenHour;
    MetricMeta mNeo;
    MetricMeta mSpaceWeather;

    std::wstring lastUpdateText = L"--:--";
    std::wstring sourceLabel = L"none";
};

struct Settings {
    HudWidgetMode hudWidgetMode = HudWidgetMode::FullScreenButton;
    bool windowBounceEnabled = true;
    std::wstring layerAttachMode = L"auto";
    PerformanceMode performanceMode = PerformanceMode::High;
    QualityTier perfClouds = QualityTier::High;
    QualityTier perfCelestial = QualityTier::High;
    QualityTier perfPrecip = QualityTier::High;
    QualityTier perfEffects = QualityTier::High;

    std::wstring bottomBoundaryMode = L"above_taskbar";
    int topInsetPx = 0;
    int rightInsetPx = 0;
    int bottomInsetPx = 10;
    int leftInsetPx = 0;
    std::wstring multiMonitorMode = L"primary";
    int targetFps = 60;
    bool pauseOnFullscreen = true;

    std::wstring primarySource = L"open_meteo";
    std::wstring astroPrimarySource = L"sunrise_sunset_io";
    std::wstring nasaApiKey;
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
    HumidityUnit humidityUnit = HumidityUnit::Percent;
    PressureUnit pressureUnit = PressureUnit::Hpa;
    WindUnit windUnit = WindUnit::Kmh;
    CloudUnit cloudUnit = CloudUnit::Percent;
    PrecipUnit precipUnit = PrecipUnit::Mm;
    UvUnit uvUnit = UvUnit::Index;
    AqiUnit aqiUnit = AqiUnit::Aqi;
    PollenUnit pollenUnit = PollenUnit::Index;
    VisibilityUnit visibilityUnit = VisibilityUnit::Km;
    DayNightUnit dayNightUnit = DayNightUnit::Label;

    float hudButtonRadiusPx = 24.0f;
    float hudFontSizePx = 12.0f;
    ArcGeometry hudButtonGeom{24, 92, 88};
    bool hasCustomBtnPos = false;
    double hudBtnRelX = 0.92;
    double hudBtnRelY = 0.12;
    int forecastHudMarginPx = 14;
    int forecastHudOpacity = 86;

    int cursorHoverOpacityPct = 5;

    TriStateMode cloudsMode = TriStateMode::Auto;
    int cloudsIntensity = 35;
    bool topCloudCoverEnabled = false;
    bool skyAmbientTintEnabled = false;
    int skyAmbientTintOpacity = 15;
    CelestialMonitorMode celestialMonitorMode = CelestialMonitorMode::Primary;
    CelestialTimelineMode celestialTimelineMode = CelestialTimelineMode::AutoForecast;
    double customSunriseHour = 6.5;
    double customSunsetHour = 18.5;
    double customMoonriseHour = 20.0;
    double customMoonsetHour = 7.0;
    TriStateMode sunMode = TriStateMode::Auto;
    CelestialMotionMode sunMotionMode = CelestialMotionMode::ArcLtr;
    ArcGeometry sunGeom{7, 82, 82};
    TriStateMode moonMode = TriStateMode::Auto;
    CelestialMotionMode moonMotionMode = CelestialMotionMode::ArcLtr;
    ArcGeometry moonGeom{6, 82, 82};
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
    TriStateMode effectThermal = TriStateMode::Auto;
    TriStateMode effectNightStars = TriStateMode::Auto;
    TriStateMode effectAirQuality = TriStateMode::Auto;
    TriStateMode effectPollen = TriStateMode::Auto;

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
    bool hasCustomAirQualityColor = false;
    D2D1_COLOR_F colorAirQuality = D2D1::ColorF(0.78f, 0.68f, 0.38f, 0.24f);
    bool hasCustomPollenColor = false;
    D2D1_COLOR_F colorPollen = D2D1::ColorF(0.96f, 0.88f, 0.42f, 0.72f);

    D2D1_COLOR_F colorHudBg = D2D1::ColorF(0.07f, 0.09f, 0.14f, 0.86f);
    D2D1_COLOR_F colorHudText = D2D1::ColorF(0.95f, 0.97f, 0.99f, 0.96f);
};

struct InteractiveControl {
    D2D1_RECT_F rect{};
    std::wstring actionId;
    std::wstring btnText;
    D2D1_COLOR_F tint = D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.90f);
    std::wstring tooltipText;
};

struct FallingItem {
    PrecipKind kind = PrecipKind::Snow;
    bool isFlakeSub = false;
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float size = 6.0f;
    float angle = 0.0f;
    float spin = 0.0f;
    float swayPhase = 0.0f;
    float alpha = 0.9f;
    int depthTier = 1;
    bool slidingSide = false;
    float hitAnimTimer = 0.0f;
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
ForecastSnapshot g_forecast;
bool g_forecastDirty = true;
std::wstring g_lastFetchedSource;
std::wstring g_lastFetchedAstroSource;
double g_lastFetchedLat = 0.0;
double g_lastFetchedLon = 0.0;

HWND g_overlayHwnd = nullptr;
HWND g_hudHwnd = nullptr;
HANDLE g_weatherWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_renderThread;
[[clang::no_destroy]] std::optional<std::thread> g_weatherThread;
std::atomic<bool> g_running{false};

std::atomic<bool> g_hudExpanded{false};
std::atomic<float> g_hudBtnCx{0.0f};
std::atomic<float> g_hudBtnCy{0.0f};
std::atomic<float> g_hudBtnR{24.0f};
std::atomic<int> g_hudWinLeft{0};
std::atomic<int> g_hudWinTop{0};
std::atomic<int> g_hudWinWidth{0};
std::atomic<int> g_hudWinHeight{0};

std::mutex g_hudUiMutex;
bool g_draggingHudBtn = false;
bool g_movedDuringDrag = false;
int g_dragStartScreenX = 0;
int g_dragStartScreenY = 0;
float g_dragStartBtnCx = 0.0f;
float g_dragStartBtnCy = 0.0f;
int g_hudScrollPx = 0;
int g_hudMaxScrollPx = 0;
bool g_hudSettingsView = false;
bool g_hudSourcesView = false;
HudCategory g_hudCategory = HudCategory::Meteo;
HudDataTab g_hudDataTab = HudDataTab::Now;
ExpandedSection g_expandedSection = ExpandedSection::None;
DetailWidgetKind g_nowDetail = DetailWidgetKind::None;
DetailWidgetKind g_hoursDetail = DetailWidgetKind::None;
DetailWidgetKind g_daysDetail = DetailWidgetKind::None;
DetailWidgetKind g_astroDetail = DetailWidgetKind::None;
int g_hoursPage = 0;
int g_daysPage = 0;
int g_settingsMeteoPage = 0;
int g_settingsAstroPage = 0;
float g_hoverRelX = -1000.0f;
float g_hoverRelY = -1000.0f;
std::vector<InteractiveControl> g_hudControls;

ID2D1Factory* g_d2dFactory = nullptr;
IDWriteFactory* g_dwriteFactory = nullptr;
IDWriteTextFormat* g_hudTitleFormat = nullptr;
IDWriteTextFormat* g_hudBodyFormat = nullptr;
IDWriteTextFormat* g_hudItalicFormat = nullptr;
IDWriteTextFormat* g_hudMediumFormat = nullptr;
IDWriteTextFormat* g_hudHeroFormat = nullptr;
IDWriteTextFormat* g_hudCircleFormat = nullptr;
IDWriteTextFormat* g_hudPillFormat = nullptr;
float g_cachedFontSizePx = 0.0f;
float g_cachedCircleFontPx = 0.0f;
CachedSurface g_surface;
CachedSurface g_hudSurface;

int g_canvasLeft = 0;
int g_canvasTop = 0;
int g_canvasWidth = 0;
int g_canvasHeight = 0;
float g_primaryDpiScale = 1.0f;
std::vector<MonitorCanvasRect> g_monitorRects;
std::vector<WindowSurfaceBox> g_bounceWindows;
std::vector<FallingItem> g_falling;
LightningFlash g_lightning;

QualityTier ResolveCategoryTier(const Settings& cfg, QualityTier customTier) {
    if (cfg.performanceMode == PerformanceMode::Low) return QualityTier::Low;
    if (cfg.performanceMode == PerformanceMode::Medium) return QualityTier::Medium;
    if (cfg.performanceMode == PerformanceMode::High) return QualityTier::High;
    return customTier;
}

float GetTierScale(QualityTier tier) {
    if (tier == QualityTier::Low) return 0.55f;
    if (tier == QualityTier::High) return 1.65f;
    return 1.35f;
}

bool HasAnyForecastConsumer(const Settings& cfg) {
    bool meteoOff = (cfg.primarySource == L"off");
    bool astroOff = (cfg.astroPrimarySource == L"off");
    if (meteoOff && astroOff) return false;
    if (cfg.primarySource == L"custom_api") {
        if (cfg.customApiUrl.empty()) return false;
        bool needsCoords = (cfg.customApiUrl.find(L"{lat}") != std::wstring::npos) ||
                           (cfg.customApiUrl.find(L"{lon}") != std::wstring::npos);
        return !needsCoords || cfg.hasValidCoords;
    }
    if (cfg.primarySource == L"local_file") return !cfg.localWeatherPath.empty();
    if (cfg.primarySource == L"nasa_donki" || cfg.astroPrimarySource == L"nasa_donki") return true;
    if ((cfg.primarySource == L"nasa_neows" || cfg.astroPrimarySource == L"nasa_neows") &&
        !cfg.nasaApiKey.empty()) {
        return true;
    }
    return cfg.hasValidCoords;
}

bool NeedsAirQualityFetch(const Settings& cfg) {
    return cfg.hasValidCoords &&
           (cfg.effectAirQuality == TriStateMode::Auto ||
            cfg.effectPollen == TriStateMode::Auto ||
            cfg.hudWidgetMode != HudWidgetMode::FullScreenOnly);
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

std::wstring ReplaceAllSubstrings(std::wstring s, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) return s;
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
        if (c == L',') c = L'.';
    }
    return s;
}

std::wstring FormatHourHhMm(double hourFloat) {
    double h = std::fmod(hourFloat, 24.0);
    if (h < 0.0) h += 24.0;
    int hh = static_cast<int>(h);
    int mm = static_cast<int>(std::llround((h - hh) * 60.0));
    if (mm >= 60) {
        mm = 0;
        hh = (hh + 1) % 24;
    }
    wchar_t buf[16] = {};
    swprintf_s(buf, L"%02d:%02d", hh, mm);
    return buf;
}

std::wstring FormatDurationHhMm(double hoursSpan) {
    double span = hoursSpan;
    if (span < 0.0) span += 24.0;
    int hh = static_cast<int>(span);
    int mm = static_cast<int>(std::llround((span - hh) * 60.0));
    if (mm >= 60) {
        mm = 0;
        hh++;
    }
    wchar_t buf[32] = {};
    swprintf_s(buf, L"%dh %02dm", hh, mm);
    return buf;
}

double ParseHourFromFlexibleTimeStr(const std::string& raw) {
    if (raw.empty()) return -1.0;
    size_t colon1 = raw.find(':');
    if (colon1 == std::string::npos || colon1 == 0) return -1.0;
    size_t startH = colon1;
    while (startH > 0 && std::isdigit(static_cast<unsigned char>(raw[startH - 1]))) {
        startH--;
    }
    int hh = std::atoi(raw.substr(startH, colon1 - startH).c_str());
    int mm = std::atoi(raw.substr(colon1 + 1, 2).c_str());
    if (hh < 0 || hh > 23 || mm < 0 || mm > 59) return -1.0;
    bool isPm = (raw.find("PM") != std::string::npos || raw.find("pm") != std::string::npos);
    bool isAm = (raw.find("AM") != std::string::npos || raw.find("am") != std::string::npos);
    if (isPm && hh < 12) hh += 12;
    if (isAm && hh == 12) hh = 0;
    return static_cast<double>(hh) + static_cast<double>(mm) / 60.0;
}

std::string ExtractJsonStringField(const std::string& json, size_t scope, const std::string& key) {
    size_t pos = json.find("\"" + key + "\"", scope);
    if (pos == std::string::npos) return {};
    pos = json.find(':', pos);
    if (pos == std::string::npos) return {};
    size_t q1 = json.find('"', pos);
    if (q1 == std::string::npos) return {};
    size_t q2 = json.find('"', q1 + 1);
    if (q2 == std::string::npos) return {};
    return json.substr(q1 + 1, q2 - q1 - 1);
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

double ParseTimeHhMmOrDefault(const std::wstring& raw, double defHour) {
    std::wstring s = Trim(raw);
    size_t colon = s.find(L':');
    if (colon == std::wstring::npos) return defHour;
    double hh = 0.0;
    double mm = 0.0;
    if (!TryParseDoubleStrict(s.substr(0, colon), hh) ||
        !TryParseDoubleStrict(s.substr(colon + 1), mm)) {
        return defHour;
    }
    int hInt = static_cast<int>(hh);
    int mInt = static_cast<int>(mm);
    if (hInt < 0 || hInt > 23 || mInt < 0 || mInt > 59) return defHour;
    return static_cast<double>(hInt) + static_cast<double>(mInt) / 60.0;
}

void ParsePairTimeHhMmOrDefault(const std::wstring& raw, double defA, double defB, double& outA, double& outB) {
    outA = defA;
    outB = defB;
    std::wstring s = Trim(raw);
    size_t slash = s.find(L'/');
    if (slash == std::wstring::npos) return;
    outA = ParseTimeHhMmOrDefault(s.substr(0, slash), defA);
    outB = ParseTimeHhMmOrDefault(s.substr(slash + 1), defB);
}

void ParseScreenInsetsString(const std::wstring& raw, int& outTop, int& outRight, int& outBottom, int& outLeft) {
    outTop = 0;
    outRight = 0;
    outBottom = 10;
    outLeft = 0;
    std::wstring s = Trim(raw);
    std::vector<int> vals;
    size_t start = 0;
    while (start <= s.size()) {
        size_t sl = s.find(L'/', start);
        std::wstring tok = (sl == std::wstring::npos) ? s.substr(start) : s.substr(start, sl - start);
        double dv = 0.0;
        if (TryParseDoubleStrict(tok, dv)) {
            vals.push_back(std::clamp(static_cast<int>(std::llround(dv)), 0, 1000));
        } else {
            vals.push_back(-1);
        }
        if (sl == std::wstring::npos) break;
        start = sl + 1;
    }
    if (vals.size() >= 1 && vals[0] >= 0) outTop = vals[0];
    if (vals.size() >= 2 && vals[1] >= 0) outRight = vals[1];
    if (vals.size() >= 3 && vals[2] >= 0) outBottom = vals[2];
    if (vals.size() >= 4 && vals[3] >= 0) outLeft = vals[3];
}

int ParseIntRangeOrDefault(int rawVal, int minVal, int maxVal, int defVal) {
    if (rawVal < minVal || rawVal > maxVal) return defVal;
    return rawVal;
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
    if (slash == std::wstring::npos) return;
    std::wstring xPart = (dash != std::wstring::npos && slash > dash)
        ? Trim(s.substr(dash + 1, slash - dash - 1))
        : Trim(s.substr(0, slash));
    std::wstring yPart = Trim(s.substr(slash + 1));

    auto isIntStr = [](const std::wstring& p) -> bool {
        if (p.empty()) return false;
        for (wchar_t c : p) {
            if (!iswdigit(c)) return false;
        }
        return true;
    };
    if (!isIntStr(xPart) || !isIntStr(yPart)) return;
    try {
        int x = std::stoi(xPart);
        int y = std::stoi(yPart);
        if (x < 0 || x > 100 || y < 0 || y > 100) return;
        outGeom = {defGeom.radius, x, y};
    } catch (...) {
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

std::vector<std::wstring> BuildOrderedProvidersForPrimary(const std::wstring& primarySrc) {
    const std::wstring allProviders[12] = {
        L"open_meteo", L"sunrise_sunset_io", L"met_norway", L"seven_timer",
        L"weatherapi", L"openweathermap", L"visual_crossing", L"tomorrow_io",
        L"custom_api", L"local_file", L"nasa_neows", L"nasa_donki"
    };
    std::vector<std::wstring> ordered;
    ordered.reserve(12);
    if (!primarySrc.empty() && primarySrc != L"off") {
        ordered.push_back(primarySrc);
    }
    for (const auto& p : allProviders) {
        if (p != primarySrc) ordered.push_back(p);
    }
    return ordered;
}

bool ProviderRequiresApiKey(const std::wstring& src) {
    return src == L"nasa_neows" ||
           src == L"weatherapi" ||
           src == L"openweathermap" ||
           src == L"visual_crossing" ||
           src == L"tomorrow_io";
}

std::wstring GetProviderApiKey(const std::wstring& src, const Settings& cfg) {
    if (src == L"nasa_neows") return cfg.nasaApiKey;
    if (src == L"weatherapi") return cfg.weatherapiKey;
    if (src == L"openweathermap") return cfg.openweathermapKey;
    if (src == L"visual_crossing") return cfg.visualCrossingKey;
    if (src == L"tomorrow_io") return cfg.tomorrowIoKey;
    return L"";
}

std::wstring FormatProviderHudBadge(const std::wstring& src) {
    if (src == L"open_meteo") return L"Open-Meteo";
    if (src == L"met_norway") return L"Yr.no";
    if (src == L"seven_timer") return L"7Timer!";
    if (src == L"weatherapi") return L"WeatherAPI*";
    if (src == L"openweathermap") return L"OWM*";
    if (src == L"visual_crossing") return L"VisCross*";
    if (src == L"tomorrow_io") return L"Tomorrow*";
    if (src == L"sunrise_sunset_io") return L"Sunrise.io";
    if (src == L"nasa_neows") return L"NASA NeoWs*";
    if (src == L"nasa_donki") return L"NASA DONKI";
    if (src == L"custom_api") return L"Custom API";
    if (src == L"local_file") return L"Local File";
    return ProviderRequiresApiKey(src) ? (src + L"*") : src;
}

PerformanceMode ParsePerformanceMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"low") return PerformanceMode::Low;
    if (v == L"medium") return PerformanceMode::Medium;
    if (v == L"custom") return PerformanceMode::Custom;
    return PerformanceMode::High;
}

QualityTier ParseQualityTier(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"low") return QualityTier::Low;
    if (v == L"medium") return QualityTier::Medium;
    return QualityTier::High;
}

HudWidgetMode ParseHudWidgetMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"full_screen_only") return HudWidgetMode::FullScreenOnly;
    if (v == L"button_only") return HudWidgetMode::ButtonOnly;
    return HudWidgetMode::FullScreenButton;
}

PrecipMode ParsePrecipMode(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"off") return PrecipMode::Off;
    if (v == L"drizzle") return PrecipMode::Drizzle;
    if (v == L"rain") return PrecipMode::Rain;
    if (v == L"downpour") return PrecipMode::Downpour;
    if (v == L"sleet") return PrecipMode::Sleet;
    if (v == L"snow") return PrecipMode::Snow;
    if (v == L"blizzard") return PrecipMode::Blizzard;
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

CelestialMonitorMode ParseCelestialMonitorMode(const std::wstring& s) {
    return ToLower(Trim(s)) == L"all" ? CelestialMonitorMode::All : CelestialMonitorMode::Primary;
}

CelestialMotionMode ParseCelestialMotion(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"line_ltr") return CelestialMotionMode::LineLtr;
    if (v == L"line_rtl") return CelestialMotionMode::LineRtl;
    if (v == L"arc_rtl") return CelestialMotionMode::ArcRtl;
    return CelestialMotionMode::ArcLtr;
}

CelestialTimelineMode ParseCelestialTimeline(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"auto_system") return CelestialTimelineMode::AutoSystem;
    if (v == L"custom") return CelestialTimelineMode::Custom;
    return CelestialTimelineMode::AutoForecast;
}

FallDirMode ParseFallDir(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"down") return FallDirMode::Down;
    if (v == L"down_left" || v == L"left") return FallDirMode::DownLeft;
    if (v == L"down_right" || v == L"right") return FallDirMode::DownRight;
    return FallDirMode::Auto;
}

TempUnit ParseTempUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"fahrenheit") return TempUnit::Fahrenheit;
    if (v == L"kelvin") return TempUnit::Kelvin;
    if (v == L"body_ratio") return TempUnit::BodyRatio;
    if (v == L"water_freeze") return TempUnit::WaterFreeze;
    if (v == L"pizza_oven") return TempUnit::PizzaOven;
    return TempUnit::Celsius;
}

HumidityUnit ParseHumidityUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"sponge_ratio") return HumidityUnit::SpongeRatio;
    if (v == L"hair_frizz") return HumidityUnit::HairFrizz;
    if (v == L"steam_room") return HumidityUnit::SteamRoom;
    return HumidityUnit::Percent;
}

PressureUnit ParsePressureUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"mmhg") return PressureUnit::Mmhg;
    if (v == L"inhg") return PressureUnit::Inhg;
    if (v == L"barometer_delta") return PressureUnit::BarometerDelta;
    if (v == L"ear_pop") return PressureUnit::EarPop;
    return PressureUnit::Hpa;
}

WindUnit ParseWindUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"ms") return WindUnit::Ms;
    if (v == L"mph") return WindUnit::Mph;
    if (v == L"knots") return WindUnit::Knots;
    if (v == L"beaufort") return WindUnit::Beaufort;
    if (v == L"breeze_ratio") return WindUnit::BreezeRatio;
    return WindUnit::Kmh;
}

CloudUnit ParseCloudUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"oktas") return CloudUnit::Oktas;
    if (v == L"blanket_ratio") return CloudUnit::BlanketRatio;
    if (v == L"sky_clear") return CloudUnit::SkyClear;
    return CloudUnit::Percent;
}

PrecipUnit ParsePrecipUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"inches") return PrecipUnit::Inches;
    if (v == L"microns") return PrecipUnit::Microns;
    if (v == L"tea_spoons") return PrecipUnit::TeaSpoons;
    if (v == L"puddle_cm") return PrecipUnit::PuddleCm;
    return PrecipUnit::Mm;
}

UvUnit ParseUvUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"percent_max") return UvUnit::PercentMax;
    if (v == L"sunburn_units") return UvUnit::SunburnUnits;
    if (v == L"spf_hint") return UvUnit::SpfHint;
    return UvUnit::Index;
}

AqiUnit ParseAqiUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"pm25") return AqiUnit::Pm25;
    if (v == L"cig_equiv") return AqiUnit::CigEquiv;
    if (v == L"lung_load") return AqiUnit::LungLoad;
    return AqiUnit::Aqi;
}

PollenUnit ParsePollenUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"sneeze_risk") return PollenUnit::SneezeRisk;
    if (v == L"tissue_pack") return PollenUnit::TissuePack;
    return PollenUnit::Index;
}

VisibilityUnit ParseVisibilityUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"meters") return VisibilityUnit::Meters;
    if (v == L"miles") return VisibilityUnit::Miles;
    if (v == L"horizon_reach") return VisibilityUnit::HorizonReach;
    if (v == L"blind_buff_hud") return VisibilityUnit::BlindBuffHud;
    return VisibilityUnit::Km;
}

DayNightUnit ParseDayNightUnit(const std::wstring& s) {
    std::wstring v = ToLower(Trim(s));
    if (v == L"solar_state") return DayNightUnit::SolarState;
    if (v == L"vampire_shift") return DayNightUnit::VampireShift;
    return DayNightUnit::Label;
}

const wchar_t* PrecipKindToLabel(PrecipKind k) {
    switch (k) {
        case PrecipKind::Drizzle: return L"Drizzle";
        case PrecipKind::Rain: return L"Rain";
        case PrecipKind::Downpour: return L"Downpour";
        case PrecipKind::Sleet: return L"Sleet";
        case PrecipKind::Snow: return L"Snow";
        case PrecipKind::Blizzard: return L"Blizzard";
        case PrecipKind::Hail: return L"Hail";
        default: return L"None";
    }
}

PrecipKind PrecipModeToKind(PrecipMode m) {
    switch (m) {
        case PrecipMode::Drizzle: return PrecipKind::Drizzle;
        case PrecipMode::Rain: return PrecipKind::Rain;
        case PrecipMode::Downpour: return PrecipKind::Downpour;
        case PrecipMode::Sleet: return PrecipKind::Sleet;
        case PrecipMode::Snow: return PrecipKind::Snow;
        case PrecipMode::Blizzard: return PrecipKind::Blizzard;
        case PrecipMode::Hail: return PrecipKind::Hail;
        default: return PrecipKind::None;
    }
}

std::wstring FormatTempValue(double tempC, TempUnit unit, bool shortButton) {
    wchar_t buf[64] = {};
    if (unit == TempUnit::Fahrenheit) {
        double f = tempC * 1.8 + 32.0;
        if (shortButton) swprintf_s(buf, L"%d°F", static_cast<int>(std::llround(f)));
        else swprintf_s(buf, L"%.1f °F", f);
        return buf;
    }
    if (unit == TempUnit::Kelvin) {
        double k = tempC + 273.15;
        if (shortButton) swprintf_s(buf, L"%dK", static_cast<int>(std::llround(k)));
        else swprintf_s(buf, L"%.1f K", k);
        return buf;
    }
    if (unit == TempUnit::BodyRatio) {
        double r = tempC / 36.6;
        if (shortButton) swprintf_s(buf, L"%.2f×", r);
        else swprintf_s(buf, L"%.2f body (36.6°)", r);
        return buf;
    }
    if (unit == TempUnit::WaterFreeze) {
        if (shortButton) swprintf_s(buf, L"%+d°", static_cast<int>(std::llround(tempC)));
        else swprintf_s(buf, L"%+.1f vs 0°C", tempC);
        return buf;
    }
    if (unit == TempUnit::PizzaOven) {
        double ov = tempC / 20.0;
        if (shortButton) swprintf_s(buf, L"%.1f", ov);
        else swprintf_s(buf, L"%.2f warm-up", ov);
        return buf;
    }
    if (shortButton) swprintf_s(buf, L"%d°", static_cast<int>(std::llround(tempC)));
    else swprintf_s(buf, L"%.1f °C", tempC);
    return buf;
}

void FormatTempSplit(double tempC, TempUnit unit, std::wstring& outVal, std::wstring& outUnit) {
    wchar_t b[32] = {};
    if (unit == TempUnit::Fahrenheit) {
        swprintf_s(b, L"%.0f", tempC * 1.8 + 32.0);
        outUnit = L"°F";
    } else if (unit == TempUnit::Kelvin) {
        swprintf_s(b, L"%.0f", tempC + 273.15);
        outUnit = L"K";
    } else if (unit == TempUnit::BodyRatio) {
        swprintf_s(b, L"%.2f", tempC / 36.6);
        outUnit = L"body";
    } else if (unit == TempUnit::WaterFreeze) {
        swprintf_s(b, L"%+.0f", tempC);
        outUnit = L"Δ0°C";
    } else if (unit == TempUnit::PizzaOven) {
        swprintf_s(b, L"%.1f", tempC / 20.0);
        outUnit = L"oven";
    } else {
        swprintf_s(b, L"%.1f", tempC);
        outUnit = L"°C";
    }
    outVal = b;
}

std::wstring FormatHumidityValue(double humPct, HumidityUnit unit) {
    wchar_t buf[64] = {};
    if (unit == HumidityUnit::SpongeRatio) swprintf_s(buf, L"%.2f sponge", std::max(0.0, humPct) / 100.0);
    else if (unit == HumidityUnit::HairFrizz) swprintf_s(buf, L"%.0f%% frizz", std::max(0.0, (humPct - 35.0) * 1.5));
    else if (unit == HumidityUnit::SteamRoom) swprintf_s(buf, L"%.0f%% steam", std::max(0.0, humPct));
    else swprintf_s(buf, L"%.0f%%", std::clamp(humPct, 0.0, 100.0));
    return buf;
}

void FormatHumiditySplit(double humPct, HumidityUnit unit, std::wstring& outVal, std::wstring& outUnit) {
    wchar_t b[32] = {};
    if (unit == HumidityUnit::SpongeRatio) {
        swprintf_s(b, L"%.2f", std::max(0.0, humPct) / 100.0);
        outUnit = L"sponge";
    } else if (unit == HumidityUnit::HairFrizz) {
        swprintf_s(b, L"%.0f", std::max(0.0, (humPct - 35.0) * 1.5));
        outUnit = L"% frizz";
    } else if (unit == HumidityUnit::SteamRoom) {
        swprintf_s(b, L"%.0f", std::max(0.0, humPct));
        outUnit = L"% steam";
    } else {
        swprintf_s(b, L"%.0f", std::clamp(humPct, 0.0, 100.0));
        outUnit = L"%";
    }
    outVal = b;
}

double ComputeHumidityFill01(double humPct, HumidityUnit unit) {
    if (unit == HumidityUnit::SpongeRatio) return std::clamp(humPct / 100.0, 0.0, 2.0);
    if (unit == HumidityUnit::HairFrizz) return std::clamp(((humPct - 35.0) * 1.5) / 100.0, 0.0, 2.0);
    if (unit == HumidityUnit::SteamRoom) return std::clamp(humPct / 100.0, 0.0, 2.0);
    return std::clamp(humPct / 100.0, 0.0, 2.0);
}

std::wstring FormatPressureCompact(double hpa, PressureUnit unit) {
    wchar_t buf[32] = {};
    if (unit == PressureUnit::Mmhg) swprintf_s(buf, L"%.0f", hpa * 0.750062);
    else if (unit == PressureUnit::Inhg) swprintf_s(buf, L"%.1f", hpa * 0.02953);
    else if (unit == PressureUnit::BarometerDelta) swprintf_s(buf, L"%+.0f", hpa - 1013.25);
    else if (unit == PressureUnit::EarPop) swprintf_s(buf, L"%.1f", std::abs(hpa - 1013.25) / 12.0);
    else swprintf_s(buf, L"%.0f", hpa);
    return buf;
}

void FormatPressureSplit(double hpa, PressureUnit unit, std::wstring& outMain, std::wstring& outSub, std::wstring& outNormRange) {
    wchar_t bMain[32] = {};
    wchar_t bNorm[48] = {};
    if (unit == PressureUnit::Mmhg) {
        swprintf_s(bMain, L"%.0f", hpa * 0.750062);
        outSub = L"mmHg";
        swprintf_s(bNorm, L"746-768 mmHg");
    } else if (unit == PressureUnit::Inhg) {
        swprintf_s(bMain, L"%.2f", hpa * 0.02953);
        outSub = L"inHg";
        swprintf_s(bNorm, L"29.38-30.24 inHg");
    } else if (unit == PressureUnit::BarometerDelta) {
        swprintf_s(bMain, L"%+.0f", hpa - 1013.25);
        outSub = L"hPa Δ";
        swprintf_s(bNorm, L"-18..+11 hPa");
    } else if (unit == PressureUnit::EarPop) {
        swprintf_s(bMain, L"%.2f", std::abs(hpa - 1013.25) / 12.0);
        outSub = L"pop";
        swprintf_s(bNorm, L"0.00-1.50 pop");
    } else {
        swprintf_s(bMain, L"%.0f", hpa);
        outSub = L"hPa";
        swprintf_s(bNorm, L"995-1024 hPa");
    }
    outMain = bMain;
    outNormRange = bNorm;
}

std::wstring FormatWindValue(double windKmh, WindUnit unit) {
    wchar_t buf[96] = {};
    if (unit == WindUnit::Ms) swprintf_s(buf, L"%.1f m/s", windKmh / 3.6);
    else if (unit == WindUnit::Mph) swprintf_s(buf, L"%.1f mph", windKmh * 0.621371);
    else if (unit == WindUnit::Knots) swprintf_s(buf, L"%.1f kn", windKmh * 0.539957);
    else if (unit == WindUnit::Beaufort) {
        double vMs = windKmh / 3.6;
        int bft = (vMs < 0.3) ? 0 : ((vMs < 1.6) ? 1 : ((vMs < 3.4) ? 2 : ((vMs < 5.5) ? 3 : ((vMs < 8.0) ? 4 : ((vMs < 10.8) ? 5 : ((vMs < 13.9) ? 6 : 7))))));
        swprintf_s(buf, L"Bft %d", bft);
    } else if (unit == WindUnit::BreezeRatio) swprintf_s(buf, L"%.2f breeze", windKmh / 25.0);
    else swprintf_s(buf, L"%.1f km/h", windKmh);
    return buf;
}

void FormatWindSplit(double windKmh, WindUnit unit, std::wstring& outMain, std::wstring& outSub) {
    wchar_t b[32] = {};
    if (unit == WindUnit::Ms) {
        swprintf_s(b, L"%.1f", windKmh / 3.6);
        outSub = L"m/s";
    } else if (unit == WindUnit::Mph) {
        swprintf_s(b, L"%.0f", windKmh * 0.621371);
        outSub = L"mph";
    } else if (unit == WindUnit::Knots) {
        swprintf_s(b, L"%.0f", windKmh * 0.539957);
        outSub = L"kn";
    } else if (unit == WindUnit::Beaufort) {
        double vMs = windKmh / 3.6;
        int bft = (vMs < 0.3) ? 0 : ((vMs < 1.6) ? 1 : ((vMs < 3.4) ? 2 : ((vMs < 5.5) ? 3 : ((vMs < 8.0) ? 4 : ((vMs < 10.8) ? 5 : ((vMs < 13.9) ? 6 : 7))))));
        swprintf_s(b, L"%d", bft);
        outSub = L"Bft";
    } else if (unit == WindUnit::BreezeRatio) {
        swprintf_s(b, L"%.1f", windKmh / 25.0);
        outSub = L"breeze";
    } else {
        swprintf_s(b, L"%.0f", windKmh);
        outSub = L"km/h";
    }
    outMain = b;
}

std::wstring FormatCloudValue(double cloudPct, CloudUnit unit) {
    wchar_t buf[64] = {};
    if (unit == CloudUnit::Oktas) swprintf_s(buf, L"%.1f/8 okta", std::clamp(cloudPct / 12.5, 0.0, 8.0));
    else if (unit == CloudUnit::BlanketRatio) swprintf_s(buf, L"%.2f blanket", std::clamp(cloudPct / 100.0, 0.0, 1.0));
    else if (unit == CloudUnit::SkyClear) swprintf_s(buf, L"%.0f%% clear", std::clamp(100.0 - cloudPct, 0.0, 100.0));
    else swprintf_s(buf, L"%.0f%%", std::clamp(cloudPct, 0.0, 100.0));
    return buf;
}

void FormatCloudSplit(double cloudPct, CloudUnit unit, std::wstring& outVal, std::wstring& outUnit) {
    wchar_t b[32] = {};
    if (unit == CloudUnit::Oktas) {
        swprintf_s(b, L"%.1f", std::clamp(cloudPct / 12.5, 0.0, 8.0));
        outUnit = L"oktas";
    } else if (unit == CloudUnit::BlanketRatio) {
        swprintf_s(b, L"%.2f", std::clamp(cloudPct / 100.0, 0.0, 1.0));
        outUnit = L"blanket";
    } else if (unit == CloudUnit::SkyClear) {
        swprintf_s(b, L"%.0f", std::clamp(100.0 - cloudPct, 0.0, 100.0));
        outUnit = L"% clear";
    } else {
        swprintf_s(b, L"%.0f", std::clamp(cloudPct, 0.0, 100.0));
        outUnit = L"%";
    }
    outVal = b;
}

std::wstring FormatPrecipValue(double mm, PrecipUnit unit) {
    wchar_t buf[64] = {};
    if (unit == PrecipUnit::Inches) swprintf_s(buf, L"%.2f in", mm / 25.4);
    else if (unit == PrecipUnit::Microns) swprintf_s(buf, L"%.0f µm", mm * 1000.0);
    else if (unit == PrecipUnit::TeaSpoons) swprintf_s(buf, L"%.1f tsp/m²", mm / 5.0);
    else if (unit == PrecipUnit::PuddleCm) swprintf_s(buf, L"%.2f cm", mm * 0.1);
    else swprintf_s(buf, L"%.1f mm", mm);
    return buf;
}

void FormatPrecipSplit(double mm, PrecipUnit unit, std::wstring& outVal, std::wstring& outUnit) {
    wchar_t b[32] = {};
    if (unit == PrecipUnit::Inches) {
        swprintf_s(b, L"%.2f", mm / 25.4);
        outUnit = L"in";
    } else if (unit == PrecipUnit::Microns) {
        swprintf_s(b, L"%.0f", mm * 1000.0);
        outUnit = L"µm";
    } else if (unit == PrecipUnit::TeaSpoons) {
        swprintf_s(b, L"%.1f", mm / 5.0);
        outUnit = L"tsp/m²";
    } else if (unit == PrecipUnit::PuddleCm) {
        swprintf_s(b, L"%.2f", mm * 0.1);
        outUnit = L"cm";
    } else {
        swprintf_s(b, L"%.1f", mm);
        outUnit = L"mm";
    }
    outVal = b;
}

std::wstring FormatUvValue(double uv, UvUnit unit) {
    wchar_t buf[64] = {};
    if (unit == UvUnit::PercentMax) swprintf_s(buf, L"%.0f%% max", (uv / 11.0) * 100.0);
    else if (unit == UvUnit::SunburnUnits) swprintf_s(buf, L"%.2f burn", uv / 3.0);
    else if (unit == UvUnit::SpfHint) swprintf_s(buf, L"SPF %d+", std::max(15, static_cast<int>(std::llround(uv * 5.0))));
    else swprintf_s(buf, L"%.1f", uv);
    return buf;
}

void FormatAqiSplit(double aqi, double pm25, AqiUnit unit, std::wstring& outMain, std::wstring& outSub) {
    wchar_t b[32] = {};
    if (unit == AqiUnit::Pm25) {
        swprintf_s(b, L"%.1f", pm25);
        outSub = L"µg/m³";
    } else if (unit == AqiUnit::CigEquiv) {
        swprintf_s(b, L"%.2f", pm25 / 22.0);
        outSub = L"smoke";
    } else if (unit == AqiUnit::LungLoad) {
        swprintf_s(b, L"%.0f%%", std::clamp((aqi / 150.0) * 100.0, 0.0, 300.0));
        outSub = L"load";
    } else {
        swprintf_s(b, L"%.0f", aqi);
        outSub = L"AQI";
    }
    outMain = b;
}

std::wstring FormatPollenValue(double pollenIdx, PollenUnit unit) {
    wchar_t buf[64] = {};
    double p = std::max(0.0, pollenIdx);
    if (unit == PollenUnit::SneezeRisk) swprintf_s(buf, L"%.0f%% sneeze", std::clamp(p * 1.6, 0.0, 100.0));
    else if (unit == PollenUnit::TissuePack) swprintf_s(buf, L"%.1f packs/d", p / 30.0);
    else swprintf_s(buf, L"%.0f idx", p);
    return buf;
}

void FormatPollenSplit(double pollenIdx, PollenUnit unit, std::wstring& outVal, std::wstring& outUnit) {
    wchar_t b[32] = {};
    double p = std::max(0.0, pollenIdx);
    if (unit == PollenUnit::SneezeRisk) {
        swprintf_s(b, L"%.0f", std::clamp(p * 1.6, 0.0, 100.0));
        outUnit = L"% risk";
    } else if (unit == PollenUnit::TissuePack) {
        swprintf_s(b, L"%.1f", p / 30.0);
        outUnit = L"packs/d";
    } else {
        swprintf_s(b, L"%.0f", p);
        outUnit = L"idx";
    }
    outVal = b;
}

std::wstring FormatVisibilityValue(double visM, VisibilityUnit unit) {
    wchar_t buf[64] = {};
    double v = std::max(0.0, visM);
    if (unit == VisibilityUnit::Meters) swprintf_s(buf, L"%.0f m", v);
    else if (unit == VisibilityUnit::Miles) swprintf_s(buf, L"%.1f mi", v / 1609.344);
    else if (unit == VisibilityUnit::HorizonReach) swprintf_s(buf, L"%.2f horizon", std::clamp(v / 12000.0, 0.0, 3.0));
    else if (unit == VisibilityUnit::BlindBuffHud) swprintf_s(buf, L"%.0f%% fog block", std::clamp((1.0 - v / 12000.0) * 100.0, 0.0, 100.0));
    else swprintf_s(buf, L"%.1f km", v / 1000.0);
    return buf;
}

std::wstring FormatDayNightValue(bool isDay, DayNightUnit unit) {
    if (unit == DayNightUnit::SolarState) return isDay ? L"Solar" : L"Lunar";
    if (unit == DayNightUnit::VampireShift) return isDay ? L"Daylight" : L"Nightfall";
    return isDay ? L"Day" : L"Night";
}

const wchar_t* MoonPhaseToName(double phase01) {
    double p = std::clamp(phase01, 0.0, 1.0);
    if (p < 0.0625 || p >= 0.9375) return L"New Moon";
    if (p < 0.1875) return L"Waxing Crescent";
    if (p < 0.3125) return L"First Quarter";
    if (p < 0.4375) return L"Waxing Gibbous";
    if (p < 0.5625) return L"Full Moon";
    if (p < 0.6875) return L"Waning Gibbous";
    if (p < 0.8125) return L"Last Quarter";
    return L"Waning Crescent";
}

bool IsHourInsideWindowStrict(double nowH, double startH, double endH, double marginH) {
    double s = startH - marginH;
    double e = endH + marginH;
    if (s < 0.0) s += 24.0;
    if (e >= 24.0) e -= 24.0;
    if (s <= e) {
        return (nowH >= s && nowH <= e);
    }
    return (nowH >= s || nowH <= e);
}

double ComputeStrictWindowProgress(double nowH, double startH, double endH, double marginH) {
    double s = startH - marginH;
    double e = endH + marginH;
    if (s < 0.0) s += 24.0;
    if (e >= 24.0) e -= 24.0;
    double span = e - s;
    if (span <= 0.1) span += 24.0;
    double elapsed = nowH - s;
    if (elapsed < 0.0) elapsed += 24.0;
    return std::clamp(elapsed / std::max(0.1, span), 0.0, 1.0);
}

int WindDegToSign(double deg) {
    double norm = std::fmod(deg, 360.0);
    if (norm < 0.0) norm += 360.0;
    return (norm >= 45.0 && norm < 225.0) ? 1 : -1;
}

const wchar_t* WindDegToCompass16(double deg) {
    double norm = std::fmod(deg, 360.0);
    if (norm < 0.0) norm += 360.0;
    static const wchar_t* kDirs[16] = {
        L"N", L"NNE", L"NE", L"ENE",
        L"E", L"ESE", L"SE", L"SSE",
        L"S", L"SSW", L"SW", L"WSW",
        L"W", L"NWW", L"NW", L"NNW"
    };
    int idx = static_cast<int>(std::llround(norm / 22.5)) % 16;
    return kDirs[idx];
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

void FormatIsoDateUtc(int offsetDays, char outBuf[16]) {
    int64_t sec = GetNowEpochSec() + static_cast<int64_t>(offsetDays) * 86400LL;
    time_t t = static_cast<time_t>(sec);
    tm utcTm = {};
    gmtime_s(&utcTm, &t);
    snprintf(outBuf, 16, "%04d-%02d-%02d", utcTm.tm_year + 1900, utcTm.tm_mon + 1, utcTm.tm_mday);
}

std::wstring FormatLocalDateOffsetDays(double addDays) {
    int64_t sec = GetNowEpochSec() + static_cast<int64_t>(std::llround(addDays * 86400.0));
    time_t t = static_cast<time_t>(sec);
    tm locTm = {};
    localtime_s(&locTm, &t);
    const wchar_t* months[12] = {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
                                 L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};
    int mIdx = std::clamp(locTm.tm_mon, 0, 11);
    wchar_t buf[24] = {};
    swprintf_s(buf, L"%02d %s", locTm.tm_mday, months[mIdx]);
    return buf;
}

std::vector<std::wstring> BuildUpcomingAstroEvents(const ForecastSnapshot& snap) {
    struct AstroCalEvent {
        int month;
        int day;
        const wchar_t* title;
    };
    static const AstroCalEvent kEvents[12] = {
        {1,  3,  L"Quadrantids peak"},
        {1,  4,  L"Earth at perihelion"},
        {3,  20, L"Spring equinox"},
        {4,  22, L"Lyrids peak"},
        {5,  6,  L"Eta Aquariids peak"},
        {6,  21, L"Summer solstice"},
        {7,  4,  L"Earth at aphelion"},
        {8,  12, L"Perseids peak"},
        {9,  22, L"Autumn equinox"},
        {10, 21, L"Orionids peak"},
        {11, 17, L"Leonids peak"},
        {12, 14, L"Geminids peak"}
    };
    const wchar_t* months[12] = {L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
                                 L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};

    time_t nowT = static_cast<time_t>(GetNowEpochSec());
    tm nowTm = {};
    localtime_s(&nowTm, &nowT);
    int curYear = nowTm.tm_year + 1900;
    int curKey = nowTm.tm_mon * 100 + nowTm.tm_mday;

    struct RankedEvent {
        int daysUntil;
        std::wstring line;
    };
    std::vector<RankedEvent> ranked;
    ranked.reserve(12);

    for (const auto& ev : kEvents) {
        int evKey = (ev.month - 1) * 100 + ev.day;
        int yr = (evKey >= curKey) ? curYear : (curYear + 1);
        tm target = {};
        target.tm_year = yr - 1900;
        target.tm_mon = ev.month - 1;
        target.tm_mday = ev.day;
        target.tm_hour = 12;
        time_t evT = mktime(&target);
        int diffDays = static_cast<int>(std::max<int64_t>(0, (static_cast<int64_t>(evT) - static_cast<int64_t>(nowT)) / 86400LL));
        wchar_t buf[96] = {};
        swprintf_s(buf, L"%02d %s - %s", ev.day, months[std::clamp(ev.month - 1, 0, 11)], ev.title);
        ranked.push_back(RankedEvent{diffDays, buf});
    }

    std::sort(ranked.begin(), ranked.end(), [](const RankedEvent& a, const RankedEvent& b) {
        return a.daysUntil < b.daysUntil;
    });

    std::vector<std::wstring> out;
    if (snap.hasNeoData && snap.neoCountToday > 0) {
        wchar_t neoBuf[96] = {};
        swprintf_s(neoBuf, L"Today - %d NEO (%.1f LD)", snap.neoCountToday, snap.neoClosestLd);
        out.push_back(neoBuf);
    }
    for (const auto& r : ranked) {
        if (static_cast<int>(out.size()) >= 5) break;
        out.push_back(r.line);
    }
    return out;
}

D2D1_COLOR_F ComputeProviderStatusTint(const ForecastSnapshot& snap) {
    if (snap.lastFetchFailed) return D2D1::ColorF(0.92f, 0.30f, 0.30f, 0.95f);
    if (!snap.valid || snap.lastSuccessEpochSec <= 0) return D2D1::ColorF(0.95f, 0.78f, 0.24f, 0.95f);
    int64_t ageSec = std::max<int64_t>(0, GetNowEpochSec() - snap.lastSuccessEpochSec);
    if (ageSec >= 86400) return D2D1::ColorF(0.95f, 0.78f, 0.24f, 0.95f);
    if (ageSec >= 21600) return D2D1::ColorF(0.60f, 0.65f, 0.72f, 0.95f);
    return D2D1::ColorF(0.24f, 0.85f, 0.48f, 0.95f);
}

double ComputeRealMoonPhaseAtEpoch(int64_t epochSec) {
    double daysSinceNew = (static_cast<double>(epochSec) - kReferenceNewMoonEpochSec) / 86400.0;
    double cycle = std::fmod(daysSinceNew, kSynodicMonthDays);
    if (cycle < 0.0) cycle += kSynodicMonthDays;
    return std::clamp(cycle / kSynodicMonthDays, 0.0, 1.0);
}

double ComputeRealMoonPhase() {
    return ComputeRealMoonPhaseAtEpoch(GetNowEpochSec());
}

void ComputeLunarEphemerisDisplay(double phase01, double hour, double moonriseH, double moonsetH, double latDeg,
                                  double& outAltDeg, double& outAzDeg, std::wstring& outNextNewDate, std::wstring& outNextFullDate) {
    double p = std::clamp(phase01, 0.0, 1.0);
    double daysToNew = (1.0 - p) * kSynodicMonthDays;
    if (daysToNew < 0.4) daysToNew += kSynodicMonthDays;
    double daysToFull = (p <= 0.5) ? ((0.5 - p) * kSynodicMonthDays) : ((1.5 - p) * kSynodicMonthDays);
    if (daysToFull < 0.4) daysToFull += kSynodicMonthDays;
    outNextNewDate = FormatLocalDateOffsetDays(daysToNew);
    outNextFullDate = FormatLocalDateOffsetDays(daysToFull);

    bool above = IsHourInsideWindowStrict(hour, moonriseH, moonsetH, 0.0);
    double prog = ComputeStrictWindowProgress(hour, moonriseH, moonsetH, 0.0);
    double maxAlt = std::clamp(90.0 - std::abs(latDeg) + 18.0 * std::sin(p * 2.0 * kPi), 18.0, 82.0);
    if (above) {
        outAltDeg = std::sin(kPi * prog) * maxAlt;
        outAzDeg = 75.0 + prog * 210.0;
    } else {
        outAltDeg = -14.0;
        outAzDeg = std::fmod(285.0 + prog * 150.0, 360.0);
    }
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

std::vector<std::string> ExtractJsonStringArray(const std::string& json, size_t scope, const std::string& key) {
    std::vector<std::string> res;
    size_t pos = json.find("\"" + key + "\"", scope);
    if (pos == std::string::npos) return res;
    size_t arrStart = json.find('[', pos);
    if (arrStart == std::string::npos) return res;
    bool inStr = false;
    bool esc = false;
    size_t itemStart = 0;
    for (size_t i = arrStart + 1; i < json.size(); ++i) {
        char c = json[i];
        if (esc) { esc = false; continue; }
        if (c == '\\') { esc = true; continue; }
        if (c == '"') {
            if (!inStr) { inStr = true; itemStart = i + 1; }
            else { inStr = false; res.push_back(json.substr(itemStart, i - itemStart)); }
            continue;
        }
        if (!inStr && c == ']') break;
    }
    return res;
}

std::vector<double> ExtractJsonDoubleArray(const std::string& json, size_t scope, const std::string& key) {
    std::vector<double> res;
    size_t pos = json.find("\"" + key + "\"", scope);
    if (pos == std::string::npos) return res;
    size_t arrStart = json.find('[', pos);
    if (arrStart == std::string::npos) return res;
    size_t i = arrStart + 1;
    while (i < json.size()) {
        while (i < json.size() && (json[i] == ' ' || json[i] == ',' || json[i] == '\n' || json[i] == '\r' || json[i] == '\t')) i++;
        if (i >= json.size() || json[i] == ']') break;
        if (json[i] == 'n') {
            res.push_back(0.0);
            while (i < json.size() && json[i] != ',' && json[i] != ']') i++;
            continue;
        }
        char* endPtr = nullptr;
        errno = 0;
        double v = std::strtod(json.c_str() + i, &endPtr);
        if (errno == 0 && endPtr != json.c_str() + i && std::isfinite(v)) {
            res.push_back(v);
            i = static_cast<size_t>(endPtr - json.c_str());
        } else {
            i++;
        }
    }
    return res;
}

int64_t ParseIsoLocalToEpochSec(const std::string& iso) {
    if (iso.size() < 10) return GetNowEpochSec();
    tm t = {};
    t.tm_year = std::atoi(iso.substr(0, 4).c_str()) - 1900;
    t.tm_mon = std::atoi(iso.substr(5, 2).c_str()) - 1;
    t.tm_mday = std::atoi(iso.substr(8, 2).c_str());
    size_t tPos = iso.find('T');
    if (tPos != std::string::npos && tPos + 5 <= iso.size()) {
        t.tm_hour = std::atoi(iso.substr(tPos + 1, 2).c_str());
        t.tm_min = std::atoi(iso.substr(tPos + 4, 2).c_str());
    }
    time_t res = mktime(&t);
    return (res > 0) ? static_cast<int64_t>(res) : GetNowEpochSec();
}

void ParseOpenMeteoSeriesArrays(const std::string& json, ForecastSnapshot& outSnap) {
    size_t hPos = json.find("\"hourly\":");
    if (hPos != std::string::npos) {
        auto times = ExtractJsonStringArray(json, hPos, "time");
        auto temps = ExtractJsonDoubleArray(json, hPos, "temperature_2m");
        auto precs = ExtractJsonDoubleArray(json, hPos, "precipitation");
        auto codes = ExtractJsonDoubleArray(json, hPos, "weather_code");
        auto winds = ExtractJsonDoubleArray(json, hPos, "wind_speed_10m");
        auto wDirs = ExtractJsonDoubleArray(json, hPos, "wind_direction_10m");
        auto press = ExtractJsonDoubleArray(json, hPos, "pressure_msl");
        auto uvs = ExtractJsonDoubleArray(json, hPos, "uv_index");

        if (!times.empty() && temps.size() == times.size()) {
            SYSTEMTIME st = {};
            GetLocalTime(&st);
            char nowPrefix[20] = {};
            snprintf(nowPrefix, sizeof(nowPrefix), "%04u-%02u-%02uT%02u:", st.wYear, st.wMonth, st.wDay, st.wHour);
            size_t startIdx = 0;
            for (size_t i = 0; i < times.size(); ++i) {
                if (times[i].rfind(nowPrefix, 0) == 0) {
                    startIdx = i;
                    break;
                }
            }
            outSnap.hourly.clear();
            for (size_t i = startIdx; i < times.size() && outSnap.hourly.size() < 24; ++i) {
                std::wstring lbl = L"+1h";
                int hrVal = static_cast<int>(st.wHour);
                size_t tPos = times[i].find('T');
                if (tPos != std::string::npos && tPos + 5 <= times[i].size()) {
                    std::string hhmm = times[i].substr(tPos + 1, 5);
                    lbl.assign(hhmm.begin(), hhmm.end());
                    hrVal = std::atoi(times[i].substr(tPos + 1, 2).c_str());
                }
                HourlyPoint hp;
                hp.timeLabel = lbl;
                hp.hourOfDay = hrVal;
                hp.epochSec = ParseIsoLocalToEpochSec(times[i]);
                hp.tempC = temps[i];
                hp.tempMinC = temps[i] - 0.6;
                hp.tempMaxC = temps[i] + 0.6;
                hp.precipMm = (i < precs.size()) ? std::max(0.0, precs[i]) : 0.0;
                hp.windSpeedKmh = (i < winds.size()) ? std::max(0.0, winds[i]) : outSnap.windSpeedKmh;
                hp.windDirDeg = (i < wDirs.size()) ? wDirs[i] : outSnap.windDirDeg;
                hp.pressureHpa = (i < press.size() && press[i] > 800.0) ? press[i] : outSnap.pressureHpa;
                hp.uvIndex = (i < uvs.size()) ? std::max(0.0, uvs[i]) : outSnap.uvIndex;
                hp.aqiValue = outSnap.aqiValue;
                hp.weatherCode = (i < codes.size()) ? static_cast<int>(codes[i]) : 0;
                outSnap.hourly.push_back(hp);
            }
            if (!outSnap.hourly.empty()) outSnap.mHourly.available = true;
        }
    }

    size_t dPos = json.find("\"daily\":");
    if (dPos != std::string::npos) {
        auto dTimes = ExtractJsonStringArray(json, dPos, "time");
        auto tMax = ExtractJsonDoubleArray(json, dPos, "temperature_2m_max");
        auto tMin = ExtractJsonDoubleArray(json, dPos, "temperature_2m_min");
        auto pSum = ExtractJsonDoubleArray(json, dPos, "precipitation_sum");
        auto dCodes = ExtractJsonDoubleArray(json, dPos, "weather_code");
        auto dWinds = ExtractJsonDoubleArray(json, dPos, "wind_speed_10m_max");
        auto dDirs = ExtractJsonDoubleArray(json, dPos, "wind_direction_10m_dominant");
        auto dSr = ExtractJsonStringArray(json, dPos, "sunrise");
        auto dSs = ExtractJsonStringArray(json, dPos, "sunset");

        if (!dTimes.empty() && tMax.size() == dTimes.size() && tMin.size() == dTimes.size()) {
            const wchar_t* wdays[7] = {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
            outSnap.daily.clear();
            for (size_t i = 0; i < dTimes.size() && outSnap.daily.size() < 14; ++i) {
                int64_t ep = ParseIsoLocalToEpochSec(dTimes[i]);
                time_t tt = static_cast<time_t>(ep);
                tm localDayTm = {};
                localtime_s(&localDayTm, &tt);
                wchar_t dLbl[24] = {};
                swprintf_s(dLbl, L"%s, %d", wdays[std::clamp(localDayTm.tm_wday, 0, 6)], localDayTm.tm_mday);

                DailyPoint dp;
                dp.dayLabel = dLbl;
                dp.epochSec = ep;
                dp.tempMinC = tMin[i];
                dp.tempMaxC = tMax[i];
                dp.precipMm = (i < pSum.size()) ? std::max(0.0, pSum[i]) : 0.0;
                dp.windSpeedKmh = (i < dWinds.size()) ? std::max(0.0, dWinds[i]) : outSnap.windSpeedKmh;
                dp.windDirDeg = (i < dDirs.size()) ? dDirs[i] : outSnap.windDirDeg;
                dp.pressureHpa = outSnap.pressureHpa + std::sin(static_cast<double>(i) * 1.1) * 3.5;
                dp.aqiValue = outSnap.aqiValue;
                dp.emiKp = std::clamp(outSnap.flareKpEst + std::cos(static_cast<double>(i) * 0.9) * 1.2, 1.0, 8.0);
                dp.sunriseHour = (i < dSr.size()) ? ParseHourFromFlexibleTimeStr(dSr[i]) : outSnap.sunriseHour;
                if (dp.sunriseHour < 0.0) dp.sunriseHour = outSnap.sunriseHour;
                dp.sunsetHour = (i < dSs.size()) ? ParseHourFromFlexibleTimeStr(dSs[i]) : outSnap.sunsetHour;
                if (dp.sunsetHour < 0.0) dp.sunsetHour = outSnap.sunsetHour;
                dp.moonriseHour = std::fmod(outSnap.moonriseHour + i * 0.85, 24.0);
                dp.moonsetHour = std::fmod(outSnap.moonsetHour + i * 0.85, 24.0);
                dp.weatherCode = (i < dCodes.size()) ? static_cast<int>(dCodes[i]) : 0;
                outSnap.daily.push_back(dp);
            }
            if (!outSnap.daily.empty()) {
                outSnap.mDaily.available = true;
                outSnap.todayMinC = outSnap.daily[0].tempMinC;
                outSnap.todayMaxC = outSnap.daily[0].tempMaxC;
                outSnap.hasTodayMinMax = true;
                outSnap.mTodayMinMax.available = true;
            }
        }
    }
}

float ComputeMonitorEdgeFade(const MonitorCanvasRect& mon, float cx, float cy, float padPx) {
    float fadeDist = std::max(24.0f, padPx);
    float leftBound = mon.left + mon.innerLeftRel;
    float rightBound = mon.left + mon.innerRightRel;
    float topBound = mon.top + mon.innerTopRel;
    float botBound = mon.top + mon.bottomLimitRelY;
    float minEdge = std::min({
        cx - leftBound,
        rightBound - cx,
        cy - topBound,
        botBound - cy
    });
    if (minEdge <= 0.0f) return 0.0f;
    if (minEdge >= fadeDist) return 1.0f;
    float t = minEdge / fadeDist;
    return t * t * (3.0f - 2.0f * t);
}

void ResolveCelestialWindow(const Settings& cfg, const ForecastSnapshot& snap, bool isSun, double hour,
                             double& outStartH, double& outEndH, bool& outInWindow) {
    CelestialTimelineMode tl = cfg.celestialTimelineMode;
    outInWindow = false;
    if (tl == CelestialTimelineMode::Custom) {
        outStartH = isSun ? cfg.customSunriseHour : cfg.customMoonriseHour;
        outEndH = isSun ? cfg.customSunsetHour : cfg.customMoonsetHour;
        outInWindow = IsHourInsideWindowStrict(hour, outStartH, outEndH, 0.35);
        return;
    }
    if (tl == CelestialTimelineMode::AutoSystem) {
        outStartH = isSun ? 6.0 : 19.0;
        outEndH = isSun ? 18.0 : 7.0;
        outInWindow = IsHourInsideWindowStrict(hour, outStartH, outEndH, 0.35);
        return;
    }
    if (isSun) {
        if (snap.valid && snap.hasSunTimes) {
            outStartH = snap.sunriseHour;
            outEndH = snap.sunsetHour;
            outInWindow = IsHourInsideWindowStrict(hour, outStartH, outEndH, 0.35);
        } else {
            outStartH = cfg.customSunriseHour;
            outEndH = cfg.customSunsetHour;
            outInWindow = snap.valid ? snap.isDay : IsHourInsideWindowStrict(hour, outStartH, outEndH, 0.35);
        }
    } else {
        if (snap.valid && snap.hasMoonTimes) {
            outStartH = snap.moonriseHour;
            outEndH = snap.moonsetHour;
            outInWindow = IsHourInsideWindowStrict(hour, outStartH, outEndH, 0.35);
        } else {
            outStartH = cfg.customMoonriseHour;
            outEndH = cfg.customMoonsetHour;
            outInWindow = snap.valid ? !snap.isDay : IsHourInsideWindowStrict(hour, outStartH, outEndH, 0.35);
        }
    }
}

void ComputeCelestialCoordsInMonitor(const MonitorCanvasRect& mon, bool isSun, CelestialMotionMode motion,
                                     const ArcGeometry& geom, double hour, const Settings& cfg, const ForecastSnapshot& snap,
                                     float& outRadius, float& outCx, float& outCy, float& outHorizonRatio01, bool& outInWindow) {
    float innerW = std::max(40.0f, mon.innerRightRel - mon.innerLeftRel);
    float innerH = std::max(40.0f, mon.bottomLimitRelY - mon.innerTopRel);
    float minSide = std::min(innerW, innerH);
    outRadius = minSide * (static_cast<float>(geom.radius) / 100.0f);
    outHorizonRatio01 = 1.0f;

    double startH = 6.5;
    double endH = 18.5;
    ResolveCelestialWindow(cfg, snap, isSun, hour, startH, endH, outInWindow);
    double progress = ComputeStrictWindowProgress(hour, startH, endH, 0.35);

    bool rightToLeft = (motion == CelestialMotionMode::LineRtl || motion == CelestialMotionMode::ArcRtl);
    bool useArc = (motion == CelestialMotionMode::ArcLtr || motion == CelestialMotionMode::ArcRtl);

    double xNorm = rightToLeft ? (0.90 - 0.80 * progress) : (0.10 + 0.80 * progress);
    outCx = mon.left + mon.innerLeftRel + innerW * static_cast<float>(xNorm);

    if (useArc) {
        float horizonY = mon.top + mon.bottomLimitRelY + outRadius * 0.90f;
        float apexY = mon.top + mon.innerTopRel + outRadius * 1.45f;
        float arcSin = static_cast<float>(std::sin(kPi * progress));
        outCy = horizonY - arcSin * std::max(30.0f, horizonY - apexY);
        float heightAboveBot = (mon.top + mon.bottomLimitRelY) - outCy;
        outHorizonRatio01 = std::clamp((heightAboveBot + outRadius) / std::max(24.0f, outRadius * 3.4f), 0.0f, 1.0f);
    } else {
        float lineY = mon.top + mon.innerTopRel + innerH * 0.24f - static_cast<float>(std::sin(kPi * progress)) * (innerH * 0.06f);
        outCy = lineY;
        outHorizonRatio01 = 1.0f;
    }
}

PrecipKind DetectPrecipKindFromForecast(int code, double precipMm, double windKmh, double tempC) {
    if (code == 96 || code == 99) return PrecipKind::Hail;
    if (code == 66 || code == 67) return PrecipKind::Sleet;
    if (code == 71 || code == 73 || code == 75 || code == 77 || code == 85 || code == 86) {
        return (windKmh >= 24.0 || code == 86) ? PrecipKind::Blizzard : PrecipKind::Snow;
    }
    if (code == 51 || code == 53 || code == 55 || code == 56 || code == 57) {
        return PrecipKind::Drizzle;
    }
    if (code == 82 || precipMm >= 6.5) {
        return PrecipKind::Downpour;
    }
    if (code == 61 || code == 63 || code == 65 || code == 80 || code == 81 || code == 95 || precipMm > 0.1) {
        if (tempC <= 1.5 && precipMm > 0.1) return PrecipKind::Sleet;
        return (precipMm <= 0.45 && code == 61) ? PrecipKind::Drizzle : PrecipKind::Rain;
    }
    return PrecipKind::None;
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
    if (parts.size() > 0) cfg.hasCustomDustColor = TryParseHexColorStrict(parts[0], 0.28f, cfg.colorDust);
    if (parts.size() > 1) cfg.hasCustomFogColor = TryParseHexColorStrict(parts[1], 0.35f, cfg.colorFog);
    if (parts.size() > 2) cfg.hasCustomSmogColor = TryParseHexColorStrict(parts[2], 0.36f, cfg.colorSmog);
    if (parts.size() > 3) cfg.hasCustomThunderstormColor = TryParseHexColorStrict(parts[3], 0.95f, cfg.colorThunderstorm);
    if (parts.size() > 4) cfg.hasCustomAuroraColor = TryParseHexColorStrict(parts[4], 0.32f, cfg.colorAurora);
    if (parts.size() > 5) cfg.hasCustomMirageColor = TryParseHexColorStrict(parts[5], 0.25f, cfg.colorMirage);
    if (parts.size() > 6) cfg.hasCustomAirQualityColor = TryParseHexColorStrict(parts[6], 0.24f, cfg.colorAirQuality);
    if (parts.size() > 7) cfg.hasCustomPollenColor = TryParseHexColorStrict(parts[7], 0.72f, cfg.colorPollen);
}

std::wstring ReadPersistedStringValue(LPCWSTR key) {
    wchar_t buf[256] = {};
    size_t len = Wh_GetStringValue(key, buf, _countof(buf));
    if (len == 0) return L"";
    return Trim(std::wstring(buf));
}

void WritePersistedStringValue(LPCWSTR key, const std::wstring& val) {
    Wh_SetStringValue(key, val.c_str());
}

void SavePersistedHudRuntimeState(const Settings& cur) {
    WritePersistedStringValue(L"hud_astroPrimarySource", cur.astroPrimarySource);
    WritePersistedStringValue(L"hud_primarySource", cur.primarySource);
    if (cur.hasCustomBtnPos) {
        wchar_t posBuf[64] = {};
        swprintf_s(posBuf, L"%.4f/%.4f", cur.hudBtnRelX, cur.hudBtnRelY);
        std::wstring posStr(posBuf);
        for (wchar_t& c : posStr) {
            if (c == L',') c = L'.';
        }
        WritePersistedStringValue(L"hud_btnRelPos", posStr);
    }
    auto triToStr = [](TriStateMode m) -> const wchar_t* {
        if (m == TriStateMode::On) return L"on";
        if (m == TriStateMode::Off) return L"off";
        return L"auto";
    };
    auto tierToStr = [](QualityTier t) -> const wchar_t* {
        if (t == QualityTier::Low) return L"low";
        if (t == QualityTier::Medium) return L"medium";
        return L"high";
    };
    const wchar_t* perfStr = (cur.performanceMode == PerformanceMode::Low)
        ? L"low"
        : ((cur.performanceMode == PerformanceMode::Medium)
            ? L"medium"
            : ((cur.performanceMode == PerformanceMode::Custom) ? L"custom" : L"high"));
    const wchar_t* dirStr = (cur.precipFallDirection == FallDirMode::Down)
        ? L"down"
        : ((cur.precipFallDirection == FallDirMode::DownLeft)
            ? L"down_left"
            : ((cur.precipFallDirection == FallDirMode::DownRight) ? L"down_right" : L"auto"));
    std::wstring precipStr = L"auto";
    if (cur.precipitationMode == PrecipMode::Off) precipStr = L"off";
    else if (cur.precipitationMode != PrecipMode::Auto) precipStr = ToLower(PrecipKindToLabel(PrecipModeToKind(cur.precipitationMode)));

    std::wstring packed =
        std::wstring(triToStr(cur.cloudsMode)) + L"|" +
        (cur.topCloudCoverEnabled ? L"1" : L"0") + L"|" +
        (cur.skyAmbientTintEnabled ? L"1" : L"0") + L"|" +
        triToStr(cur.sunMode) + L"|" +
        triToStr(cur.moonMode) + L"|" +
        (cur.celestialInFrontOfClouds ? L"1" : L"0") + L"|" +
        precipStr + L"|" +
        dirStr + L"|" +
        (cur.precipSpeedMode == PrecipSpeedMode::Custom ? L"custom" : L"auto") + L"|" +
        (cur.windowBounceEnabled ? L"1" : L"0") + L"|" +
        triToStr(cur.effectDust) + L"|" +
        triToStr(cur.effectFog) + L"|" +
        triToStr(cur.effectSmog) + L"|" +
        triToStr(cur.effectThunderstorm) + L"|" +
        triToStr(cur.effectAurora) + L"|" +
        triToStr(cur.effectMirage) + L"|" +
        triToStr(cur.effectThermal) + L"|" +
        triToStr(cur.effectNightStars) + L"|" +
        triToStr(cur.effectAirQuality) + L"|" +
        triToStr(cur.effectPollen) + L"|" +
        perfStr + L"|" +
        tierToStr(cur.perfClouds) + L"|" +
        tierToStr(cur.perfCelestial) + L"|" +
        tierToStr(cur.perfPrecip) + L"|" +
        tierToStr(cur.perfEffects);
    WritePersistedStringValue(L"hud_quickToggles", packed);
}

void ApplyPersistedHudRuntimeState(Settings& dst, bool isInitialLoad) {
    std::wstring persistedAstro = ToLower(ReadPersistedStringValue(L"hud_astroPrimarySource"));
    if (!persistedAstro.empty()) {
        dst.astroPrimarySource = persistedAstro;
    }

    std::wstring persistedPos = ReadPersistedStringValue(L"hud_btnRelPos");
    size_t slash = persistedPos.find(L'/');
    if (slash != std::wstring::npos) {
        double rx = 0.0;
        double ry = 0.0;
        if (TryParseDoubleStrict(persistedPos.substr(0, slash), rx) &&
            TryParseDoubleStrict(persistedPos.substr(slash + 1), ry) &&
            rx >= 0.01 && rx <= 0.99 && ry >= 0.01 && ry <= 0.99) {
            dst.hasCustomBtnPos = true;
            dst.hudBtnRelX = rx;
            dst.hudBtnRelY = ry;
        }
    }

    if (!isInitialLoad) return;

    std::wstring persistedPrimary = ToLower(ReadPersistedStringValue(L"hud_primarySource"));
    if (!persistedPrimary.empty()) {
        dst.primarySource = persistedPrimary;
    }

    std::wstring packed = ReadPersistedStringValue(L"hud_quickToggles");
    if (packed.empty()) return;
    std::vector<std::wstring> tok;
    std::wstringstream ss(packed);
    std::wstring item;
    while (std::getline(ss, item, L'|')) tok.push_back(Trim(item));
    if (tok.size() < 25) return;

    dst.cloudsMode = ParseTriState(tok[0]);
    dst.topCloudCoverEnabled = (tok[1] == L"1");
    dst.skyAmbientTintEnabled = (tok[2] == L"1");
    dst.sunMode = ParseTriState(tok[3]);
    dst.moonMode = ParseTriState(tok[4]);
    dst.celestialInFrontOfClouds = (tok[5] == L"1");
    dst.precipitationMode = ParsePrecipMode(tok[6]);
    dst.precipFallDirection = ParseFallDir(tok[7]);
    dst.precipSpeedMode = ParsePrecipSpeedMode(tok[8]);
    dst.windowBounceEnabled = (tok[9] == L"1");
    dst.effectDust = ParseTriState(tok[10]);
    dst.effectFog = ParseTriState(tok[11]);
    dst.effectSmog = ParseTriState(tok[12]);
    dst.effectThunderstorm = ParseTriState(tok[13]);
    dst.effectAurora = ParseTriState(tok[14]);
    dst.effectMirage = ParseTriState(tok[15]);
    dst.effectThermal = ParseTriState(tok[16]);
    dst.effectNightStars = ParseTriState(tok[17]);
    dst.effectAirQuality = ParseTriState(tok[18]);
    dst.effectPollen = ParseTriState(tok[19]);
    dst.performanceMode = ParsePerformanceMode(tok[20]);
    dst.perfClouds = ParseQualityTier(tok[21]);
    dst.perfCelestial = ParseQualityTier(tok[22]);
    dst.perfPrecip = ParseQualityTier(tok[23]);
    dst.perfEffects = ParseQualityTier(tok[24]);
}

void LoadSettingsFromWh(bool isInitialLoad = false) {
    Settings next;
    auto readStr = [](LPCWSTR name, const std::wstring& defVal) -> std::wstring {
        WindhawkUtils::StringSetting raw(Wh_GetStringSetting(name));
        std::wstring out(raw.get());
        return out.empty() ? defVal : out;
    };

    next.hudWidgetMode = ParseHudWidgetMode(readStr(L"forecastHudMode", L"full_screen_button"));
    next.windowBounceEnabled = Wh_GetIntSetting(L"windowBounceEnabled") != 0;
    next.layerAttachMode = ToLower(Trim(readStr(L"layerAttachMode", L"auto")));
    next.performanceMode = ParsePerformanceMode(readStr(L"performanceMode", L"high"));
    next.perfClouds = ParseQualityTier(readStr(L"perfCloudsMode", L"high"));
    next.perfCelestial = ParseQualityTier(readStr(L"perfCelestialMode", L"high"));
    next.perfPrecip = ParseQualityTier(readStr(L"perfPrecipMode", L"high"));
    next.perfEffects = ParseQualityTier(readStr(L"perfEffectsMode", L"high"));

    next.bottomBoundaryMode = ToLower(Trim(readStr(L"bottomBoundaryMode", L"above_taskbar")));
    ParseScreenInsetsString(readStr(L"screenInsetsPx", L"0/0/10/0"),
                            next.topInsetPx, next.rightInsetPx, next.bottomInsetPx, next.leftInsetPx);
    next.multiMonitorMode = ToLower(Trim(readStr(L"multiMonitorMode", L"primary")));
    int fpsRaw = Wh_GetIntSetting(L"targetFps");
    next.targetFps = (fpsRaw == 30 || fpsRaw == 60) ? fpsRaw : 60;
    next.pauseOnFullscreen = Wh_GetIntSetting(L"pauseOnFullscreen") != 0;

    next.primarySource = ToLower(Trim(readStr(L"primarySource", L"open_meteo")));
    next.astroPrimarySource = L"sunrise_sunset_io";
    next.nasaApiKey = Trim(readStr(L"nasaApiKey", L""));
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
    next.humidityUnit = ParseHumidityUnit(readStr(L"humidityUnit", L"percent"));
    next.pressureUnit = ParsePressureUnit(readStr(L"pressureUnit", L"hpa"));
    next.windUnit = ParseWindUnit(readStr(L"windUnit", L"kmh"));
    next.cloudUnit = ParseCloudUnit(readStr(L"cloudUnit", L"percent"));
    next.precipUnit = ParsePrecipUnit(readStr(L"precipUnit", L"mm"));
    next.uvUnit = ParseUvUnit(readStr(L"uvUnit", L"index"));
    next.aqiUnit = ParseAqiUnit(readStr(L"aqiUnit", L"aqi"));
    next.pollenUnit = ParsePollenUnit(readStr(L"pollenUnit", L"index"));
    next.visibilityUnit = ParseVisibilityUnit(readStr(L"visibilityUnit", L"km"));
    next.dayNightUnit = ParseDayNightUnit(readStr(L"dayNightUnit", L"label"));

    next.hudButtonRadiusPx = static_cast<float>(ParseIntRangeOrDefault(Wh_GetIntSetting(L"hudButtonRadiusPx"), 14, 96, 24));
    next.hudFontSizePx = static_cast<float>(ParseIntRangeOrDefault(Wh_GetIntSetting(L"hudFontSizePx"), 9, 20, 12));
    ParseHudButtonGeometryString(readStr(L"hudButtonGeometry", L"92/88"), ArcGeometry{24, 92, 88}, next.hudButtonGeom);
    next.forecastHudMarginPx = ParseIntRangeOrDefault(Wh_GetIntSetting(L"forecastHudMarginPx"), 0, 400, 14);
    next.forecastHudOpacity = ParseIntRangeOrDefault(Wh_GetIntSetting(L"forecastHudOpacity"), 15, 100, 86);

    next.cursorHoverOpacityPct = ParseIntRangeOrDefault(Wh_GetIntSetting(L"cursorHoverOpacityPct"), 0, 100, 5);

    next.cloudsMode = ParseTriState(readStr(L"cloudsMode", L"auto"));
    next.cloudsIntensity = ParseIntRangeOrDefault(Wh_GetIntSetting(L"cloudsIntensity"), 0, 100, 35);
    next.topCloudCoverEnabled = Wh_GetIntSetting(L"topCloudCoverEnabled") != 0;
    next.skyAmbientTintEnabled = Wh_GetIntSetting(L"skyAmbientTintEnabled") != 0;
    next.skyAmbientTintOpacity = ParseIntRangeOrDefault(Wh_GetIntSetting(L"skyAmbientTintOpacity"), 0, 50, 15);
    next.celestialMonitorMode = ParseCelestialMonitorMode(readStr(L"celestialMonitorMode", L"primary"));
    next.celestialTimelineMode = ParseCelestialTimeline(readStr(L"celestialTimelineMode", L"auto_forecast"));
    ParsePairTimeHhMmOrDefault(readStr(L"customSunTimes", L"06:30/18:30"), 6.5, 18.5, next.customSunriseHour, next.customSunsetHour);
    ParsePairTimeHhMmOrDefault(readStr(L"customMoonTimes", L"20:00/07:00"), 20.0, 7.0, next.customMoonriseHour, next.customMoonsetHour);
    next.sunMode = ParseTriState(readStr(L"sunMode", L"auto"));
    next.sunMotionMode = ParseCelestialMotion(readStr(L"sunMotionMode", L"arc_ltr"));
    ParseArcGeometryString(readStr(L"sunGeometry", L"7-82/82"), ArcGeometry{7, 82, 82}, next.sunGeom);
    next.moonMode = ParseTriState(readStr(L"moonMode", L"auto"));
    next.moonMotionMode = ParseCelestialMotion(readStr(L"moonMotionMode", L"arc_ltr"));
    ParseArcGeometryString(readStr(L"moonGeometry", L"6-82/82"), ArcGeometry{6, 82, 82}, next.moonGeom);
    next.celestialInFrontOfClouds = Wh_GetIntSetting(L"celestialInFrontOfClouds") != 0;

    next.precipitationMode = ParsePrecipMode(readStr(L"precipitationMode", L"auto"));
    next.precipFallDirection = ParseFallDir(readStr(L"precipFallDirection", L"auto"));
    next.precipSpeedMode = ParsePrecipSpeedMode(readStr(L"precipSpeedMode", L"auto"));
    double fallSpd = 6.5;
    if (TryParseDoubleStrict(readStr(L"precipFallSpeedMps", L"6.5"), fallSpd) && fallSpd >= 0.1 && fallSpd <= 50.0) {
        next.precipFallSpeedMps = fallSpd;
    } else {
        next.precipFallSpeedMps = 6.5;
    }
    next.precipIntensity = SanitizePercent(Wh_GetIntSetting(L"precipIntensity"));

    next.effectsIntensity = SanitizePercent(Wh_GetIntSetting(L"effectsIntensity"));
    next.effectDust = ParseTriState(readStr(L"effectDust", L"auto"));
    next.effectFog = ParseTriState(readStr(L"effectFog", L"auto"));
    next.effectSmog = ParseTriState(readStr(L"effectSmog", L"auto"));
    next.effectThunderstorm = ParseTriState(readStr(L"effectThunderstorm", L"auto"));
    next.effectAurora = ParseTriState(readStr(L"effectAurora", L"auto"));
    next.effectMirage = ParseTriState(readStr(L"effectMirage", L"auto"));
    next.effectThermal = ParseTriState(readStr(L"effectThermal", L"auto"));
    next.effectNightStars = ParseTriState(readStr(L"effectNightStars", L"auto"));
    next.effectAirQuality = ParseTriState(readStr(L"effectAirQuality", L"auto"));
    next.effectPollen = ParseTriState(readStr(L"effectPollen", L"auto"));

    ApplySkyPrecipColors(readStr(L"skyPrecipColors", L"#6BB8FF/#FBFDFF/#C2E6FF/#E6ECF7/#FFD152/#DCE7FF"), next);
    ApplyEffectsColors(readStr(L"effectsColors", L"#D6AE73/#D1D9E0/#8F7F6B/#D6EBFF/#38EB9E/#FADBA8/#D1A857/#F7E36B"), next);

    float bgAlpha = std::clamp(static_cast<float>(next.forecastHudOpacity) / 100.0f, 0.15f, 1.0f);
    if (!TryParseHexColorStrict(readStr(L"colorHudBg", L"#121824"), bgAlpha, next.colorHudBg)) {
        next.colorHudBg = D2D1::ColorF(0.07f, 0.09f, 0.14f, bgAlpha);
    }
    TryParseHexColorStrict(readStr(L"colorHudText", L"#F2F6FC"), 0.96f, next.colorHudText);

    ApplyPersistedHudRuntimeState(next, isInitialLoad);
    if (!isInitialLoad) {
        SavePersistedHudRuntimeState(next);
    }

    bool wakeWeather = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!isInitialLoad && !g_settings.astroPrimarySource.empty()) {
            next.astroPrimarySource = g_settings.astroPrimarySource;
        }
        if (!isInitialLoad && g_settings.hasCustomBtnPos) {
            next.hasCustomBtnPos = true;
            next.hudBtnRelX = g_settings.hudBtnRelX;
            next.hudBtnRelY = g_settings.hudBtnRelY;
        }
        bool coordsChanged = next.hasValidCoords &&
                             (!g_settings.hasValidCoords ||
                              std::abs(next.latitude - g_lastFetchedLat) > 1e-6 ||
                              std::abs(next.longitude - g_lastFetchedLon) > 1e-6);
        bool feedConfigChanged = (next.primarySource != g_lastFetchedSource) ||
                                 (next.astroPrimarySource != g_lastFetchedAstroSource) ||
                                 (next.nasaApiKey != g_settings.nasaApiKey) ||
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
    }
    if (wakeWeather && g_weatherWakeEvent) {
        SetEvent(g_weatherWakeEvent);
    }
}

void ApplyRuntimeButtonPosition(float cx, float cy) {
    if (g_canvasWidth <= 1 || g_canvasHeight <= 1) return;
    double rx = std::clamp(static_cast<double>(cx) / static_cast<double>(g_canvasWidth), 0.02, 0.98);
    double ry = std::clamp(static_cast<double>(cy) / static_cast<double>(g_canvasHeight), 0.02, 0.98);
    Settings snapshot;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_settings.hasCustomBtnPos = true;
        g_settings.hudBtnRelX = rx;
        g_settings.hudBtnRelY = ry;
        snapshot = g_settings;
    }
    SavePersistedHudRuntimeState(snapshot);
}

void ToggleExclusiveSection(ExpandedSection sec) {
    std::lock_guard<std::mutex> lock(g_hudUiMutex);
    g_expandedSection = (g_expandedSection == sec) ? ExpandedSection::None : sec;
    g_hudScrollPx = 0;
}

void ToggleDetailWidget(DetailWidgetKind& cur, DetailWidgetKind target) {
    std::lock_guard<std::mutex> lock(g_hudUiMutex);
    cur = (cur == target) ? DetailWidgetKind::None : target;
    g_hudSourcesView = false;
    g_hudScrollPx = 0;
}

QualityTier NextQualityTier(QualityTier t) {
    if (t == QualityTier::Low) return QualityTier::Medium;
    if (t == QualityTier::Medium) return QualityTier::High;
    return QualityTier::Low;
}

void CycleSettingOption(const std::wstring& actionId) {
    if (actionId == L"cat_meteo") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudCategory = HudCategory::Meteo;
        g_hudSettingsView = false;
        g_hudSourcesView = false;
        g_nowDetail = DetailWidgetKind::None;
        g_hoursDetail = DetailWidgetKind::None;
        g_daysDetail = DetailWidgetKind::None;
        g_astroDetail = DetailWidgetKind::None;
        g_hudScrollPx = 0;
        return;
    }
    if (actionId == L"cat_astro") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudCategory = HudCategory::Astro;
        g_hudSettingsView = false;
        g_hudSourcesView = false;
        g_nowDetail = DetailWidgetKind::None;
        g_hoursDetail = DetailWidgetKind::None;
        g_daysDetail = DetailWidgetKind::None;
        g_astroDetail = DetailWidgetKind::None;
        g_hudScrollPx = 0;
        return;
    }
    if (actionId == L"tab_now") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudCategory = HudCategory::Meteo;
        g_hudDataTab = HudDataTab::Now;
        g_hudSettingsView = false;
        g_hudSourcesView = false;
        g_nowDetail = DetailWidgetKind::None;
        g_hudScrollPx = 0;
        return;
    }
    if (actionId == L"tab_hours") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudCategory = HudCategory::Meteo;
        g_hudDataTab = HudDataTab::Hours;
        g_hudSettingsView = false;
        g_hudSourcesView = false;
        g_hoursDetail = DetailWidgetKind::None;
        g_hudScrollPx = 0;
        return;
    }
    if (actionId == L"tab_days") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudCategory = HudCategory::Meteo;
        g_hudDataTab = HudDataTab::Days;
        g_hudSettingsView = false;
        g_hudSourcesView = false;
        g_daysDetail = DetailWidgetKind::None;
        g_hudScrollPx = 0;
        return;
    }
    if (actionId == L"toggle_sources_view") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudSourcesView = !g_hudSourcesView;
        g_hudSettingsView = false;
        g_hudScrollPx = 0;
        return;
    }
    if (actionId == L"toggle_now_detail_hero") { ToggleDetailWidget(g_nowDetail, DetailWidgetKind::NowHero); return; }
    if (actionId == L"toggle_now_detail_wind") { ToggleDetailWidget(g_nowDetail, DetailWidgetKind::NowWindPrecip); return; }
    if (actionId == L"toggle_now_detail_air")  { ToggleDetailWidget(g_nowDetail, DetailWidgetKind::NowAirVis); return; }

    if (actionId == L"toggle_astro_detail_sun")   { ToggleDetailWidget(g_astroDetail, DetailWidgetKind::AstroSun); return; }
    if (actionId == L"toggle_astro_detail_moon")  { ToggleDetailWidget(g_astroDetail, DetailWidgetKind::AstroMoon); return; }
    if (actionId == L"toggle_astro_detail_uvemi") { ToggleDetailWidget(g_astroDetail, DetailWidgetKind::AstroUvEmi); return; }

    if (actionId == L"toggle_hours_detail_1") { ToggleDetailWidget(g_hoursDetail, DetailWidgetKind::WeatherAll); return; }
    if (actionId == L"toggle_days_detail_1")  { ToggleDetailWidget(g_daysDetail,  DetailWidgetKind::WeatherAll); return; }

    if (actionId == L"hours_page_prev") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hoursPage = std::max(0, g_hoursPage - 1);
        return;
    }
    if (actionId == L"hours_page_next") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hoursPage = std::min(2, g_hoursPage + 1);
        return;
    }
    if (actionId == L"days_page_prev") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_daysPage = std::max(0, g_daysPage - 1);
        return;
    }
    if (actionId == L"days_page_next") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_daysPage = g_daysPage + 1;
        return;
    }
    if (actionId == L"settings_meteo_prev") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_settingsMeteoPage = (g_settingsMeteoPage + 2) % 3;
        return;
    }
    if (actionId == L"settings_meteo_next") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_settingsMeteoPage = (g_settingsMeteoPage + 1) % 3;
        return;
    }
    if (actionId == L"settings_astro_prev") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_settingsAstroPage = (g_settingsAstroPage + 2) % 3;
        return;
    }
    if (actionId == L"settings_astro_next") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_settingsAstroPage = (g_settingsAstroPage + 1) % 3;
        return;
    }

    if (actionId == L"toggle_hud_settings_view") {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudSettingsView = !g_hudSettingsView;
        g_hudSourcesView = false;
        g_hudScrollPx = 0;
        return;
    }
    if (actionId == L"sec_sky") { ToggleExclusiveSection(ExpandedSection::Sky); return; }
    if (actionId == L"sec_precip") { ToggleExclusiveSection(ExpandedSection::Precipitation); return; }
    if (actionId == L"sec_effects") { ToggleExclusiveSection(ExpandedSection::Effects); return; }
    if (actionId == L"sec_perf") { ToggleExclusiveSection(ExpandedSection::Performance); return; }

    auto nextTri = [](TriStateMode m) -> TriStateMode {
        if (m == TriStateMode::Auto) return TriStateMode::On;
        if (m == TriStateMode::On) return TriStateMode::Off;
        return TriStateMode::Auto;
    };

    bool wakeWeather = false;
    bool shouldPersist = false;
    Settings persistedCopy;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        Settings& cur = g_settings;
        if (actionId.rfind(L"set_meteo_src_", 0) == 0) {
            cur.primarySource = actionId.substr(14);
            g_forecastDirty = true;
            wakeWeather = true;
            shouldPersist = true;
        } else if (actionId.rfind(L"set_astro_src_", 0) == 0) {
            cur.astroPrimarySource = actionId.substr(14);
            g_forecastDirty = true;
            wakeWeather = true;
            shouldPersist = true;
        } else if (actionId == L"cycle_perf") {
            cur.performanceMode = (cur.performanceMode == PerformanceMode::Low)
                ? PerformanceMode::Medium
                : ((cur.performanceMode == PerformanceMode::Medium)
                    ? PerformanceMode::High
                    : ((cur.performanceMode == PerformanceMode::High) ? PerformanceMode::Custom : PerformanceMode::Low));
            shouldPersist = true;
        } else if (actionId == L"cycle_perf_clouds") {
            cur.performanceMode = PerformanceMode::Custom;
            cur.perfClouds = NextQualityTier(cur.perfClouds);
            shouldPersist = true;
        } else if (actionId == L"cycle_perf_celestial") {
            cur.performanceMode = PerformanceMode::Custom;
            cur.perfCelestial = NextQualityTier(cur.perfCelestial);
            shouldPersist = true;
        } else if (actionId == L"cycle_perf_precip") {
            cur.performanceMode = PerformanceMode::Custom;
            cur.perfPrecip = NextQualityTier(cur.perfPrecip);
            shouldPersist = true;
        } else if (actionId == L"cycle_perf_effects") {
            cur.performanceMode = PerformanceMode::Custom;
            cur.perfEffects = NextQualityTier(cur.perfEffects);
            shouldPersist = true;
        } else if (actionId == L"forecast_refresh") {
            if (HasAnyForecastConsumer(cur)) {
                g_forecastDirty = true;
                wakeWeather = true;
            }
        } else if (actionId == L"forecast_reset") {
            g_forecast = ForecastSnapshot{};
            g_lastFetchedSource.clear();
            g_lastFetchedAstroSource.clear();
            g_lastFetchedLat = 0.0;
            g_lastFetchedLon = 0.0;
            g_forecastDirty = false;
        } else if (actionId == L"cycle_clouds") {
            cur.cloudsMode = nextTri(cur.cloudsMode);
            shouldPersist = true;
        } else if (actionId == L"toggle_top_clouds") {
            cur.topCloudCoverEnabled = !cur.topCloudCoverEnabled;
            shouldPersist = true;
        } else if (actionId == L"toggle_sky_ambient_tint") {
            cur.skyAmbientTintEnabled = !cur.skyAmbientTintEnabled;
            shouldPersist = true;
        } else if (actionId == L"cycle_sun") {
            cur.sunMode = nextTri(cur.sunMode);
            shouldPersist = true;
        } else if (actionId == L"cycle_moon") {
            cur.moonMode = nextTri(cur.moonMode);
            shouldPersist = true;
        } else if (actionId == L"toggle_celestial_front") {
            cur.celestialInFrontOfClouds = !cur.celestialInFrontOfClouds;
            shouldPersist = true;
        } else if (actionId == L"cycle_precip_mode") {
            const PrecipMode pOrder[] = {
                PrecipMode::Auto, PrecipMode::Drizzle, PrecipMode::Rain, PrecipMode::Downpour,
                PrecipMode::Sleet, PrecipMode::Snow, PrecipMode::Blizzard, PrecipMode::Hail, PrecipMode::Off
            };
            constexpr size_t kPOrderLen = sizeof(pOrder) / sizeof(pOrder[0]);
            size_t idx = 0;
            for (size_t i = 0; i < kPOrderLen; ++i) {
                if (cur.precipitationMode == pOrder[i]) {
                    idx = (i + 1) % kPOrderLen;
                    break;
                }
            }
            cur.precipitationMode = pOrder[idx];
            shouldPersist = true;
        } else if (actionId == L"cycle_precip_dir") {
            cur.precipFallDirection = (cur.precipFallDirection == FallDirMode::Auto)
                ? FallDirMode::Down
                : ((cur.precipFallDirection == FallDirMode::Down)
                    ? FallDirMode::DownLeft
                    : ((cur.precipFallDirection == FallDirMode::DownLeft) ? FallDirMode::DownRight : FallDirMode::Auto));
            shouldPersist = true;
        } else if (actionId == L"cycle_precip_speed") {
            cur.precipSpeedMode = (cur.precipSpeedMode == PrecipSpeedMode::Auto)
                ? PrecipSpeedMode::Custom
                : PrecipSpeedMode::Auto;
            shouldPersist = true;
        } else if (actionId == L"toggle_window_bounce") {
            cur.windowBounceEnabled = !cur.windowBounceEnabled;
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_dust") {
            cur.effectDust = nextTri(cur.effectDust);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_fog") {
            cur.effectFog = nextTri(cur.effectFog);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_smog") {
            cur.effectSmog = nextTri(cur.effectSmog);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_storm") {
            cur.effectThunderstorm = nextTri(cur.effectThunderstorm);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_aurora") {
            cur.effectAurora = nextTri(cur.effectAurora);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_mirage") {
            cur.effectMirage = nextTri(cur.effectMirage);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_thermal") {
            cur.effectThermal = nextTri(cur.effectThermal);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_stars") {
            cur.effectNightStars = nextTri(cur.effectNightStars);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_aq") {
            cur.effectAirQuality = nextTri(cur.effectAirQuality);
            shouldPersist = true;
        } else if (actionId == L"cycle_fx_pollen") {
            cur.effectPollen = nextTri(cur.effectPollen);
            shouldPersist = true;
        }
        if (shouldPersist) persistedCopy = cur;
    }
    if (shouldPersist) {
        SavePersistedHudRuntimeState(persistedCopy);
    }
    if (wakeWeather && g_weatherWakeEvent) {
        SetEvent(g_weatherWakeEvent);
    }
}

std::string HttpGetUtf8(const std::wstring& url,
                        const std::wstring& userAgent = L"WindhawkWeatherOverlay/3.2.9") {
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
}

void ApplyForecastMetrics(int code, double windKmh, double /*windGustsKmh*/, double tempC, double apparentTempC,
                          double humidityPct, int cloudCover, double visibilityM, double precipMm,
                          double aqiVal, double pm25Val, double pollenMax, bool isDay,
                          PrecipKind& outPrecipKind, GeneralEffectsState& fx) {
    fx = GeneralEffectsState{};
    bool isStormCode = (code == 95 || code == 96 || code == 99);
    bool isFogCode = (code == 45 || code == 48);

    if (isFogCode || (humidityPct >= 92.0 && visibilityM > 0.0 && visibilityM < 3500.0)) {
        fx.fog = true;
    }

    outPrecipKind = DetectPrecipKindFromForecast(code, precipMm, windKmh, tempC);
    if (code == 95 || code == 96 || code == 99) {
        fx.thunderstorm = true;
    }

    bool hasPrecip = (outPrecipKind != PrecipKind::None);
    if (!hasPrecip && !isStormCode && windKmh >= 28.0 && (visibilityM <= 0.0 || visibilityM < 8000.0)) {
        fx.dust = true;
    }
    if (!hasPrecip && windKmh < 12.0 && visibilityM > 0.0 && visibilityM < 6000.0) {
        fx.smog = true;
    }
    if (isDay && !hasPrecip && !isStormCode && (tempC >= 28.0 || apparentTempC >= 30.0) && cloudCover <= 35) {
        fx.mirage = true;
    }
    if (!isDay && !hasPrecip && !isStormCode && cloudCover <= 35) {
        fx.aurora = true;
        fx.nightStars = true;
    } else if (!isDay && !hasPrecip && cloudCover <= 25) {
        fx.nightStars = true;
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
}

void FinalizeParsedForecast(ForecastSnapshot& snap, const std::wstring& srcLabel) {
    if (snap.precipMm <= 0.0 && snap.weatherCode > 0) {
        snap.precipMm = EstimatePrecipMmFromWmo(snap.weatherCode);
    }
    if (!snap.hasTodayMinMax && snap.mTemp.available) {
        snap.todayMinC = snap.tempC;
        snap.todayMaxC = snap.tempC;
        snap.hasTodayMinMax = true;
        snap.mTodayMinMax.available = true;
    }
    snap.windDirSign = (snap.windSpeedKmh > 0.2) ? WindDegToSign(snap.windDirDeg) : 0;
    ApplyForecastMetrics(
        snap.weatherCode, snap.windSpeedKmh, snap.windGustsKmh,
        snap.tempC, snap.apparentTempC, snap.humidityPct,
        static_cast<int>(snap.cloudCoverPct), snap.visibilityM,
        snap.precipMm, snap.aqiValue, snap.pm25Value,
        snap.pollenMaxValue, snap.isDay, snap.precipKind, snap.effects);
    snap.valid = snap.mTemp.available || snap.mCondition.available || snap.mSunTimes.available ||
                 snap.mNeo.available || snap.mSpaceWeather.available;
    snap.sourceLabel = srcLabel;
    StampForecastUpdateTime(snap);

    const std::wstring& t = snap.lastUpdateText;
    if (snap.mCondition.available) StampMetricMeta(snap.mCondition, true, srcLabel, t);
    if (snap.mTemp.available) StampMetricMeta(snap.mTemp, true, srcLabel, t);
    if (snap.mTodayMinMax.available) StampMetricMeta(snap.mTodayMinMax, true, srcLabel, t);
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
    if (snap.mSunTimes.available) StampMetricMeta(snap.mSunTimes, true, srcLabel, t);
    if (snap.mMoonTimes.available) StampMetricMeta(snap.mMoonTimes, true, srcLabel, t);
    if (snap.mGoldenHour.available) StampMetricMeta(snap.mGoldenHour, true, srcLabel, t);
    if (snap.mNeo.available) StampMetricMeta(snap.mNeo, true, srcLabel, t);
    if (snap.mSpaceWeather.available) StampMetricMeta(snap.mSpaceWeather, true, srcLabel, t);
    if (snap.mHourly.available) StampMetricMeta(snap.mHourly, true, srcLabel, t);
    if (snap.mDaily.available) StampMetricMeta(snap.mDaily, true, srcLabel, t);
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

bool ExtractNumberAnyScope(const std::string& json, size_t primaryScope, const std::string& key, double& val) {
    if (ExtractNumberScoped(json, primaryScope, key, val)) return true;
    return ExtractNumberScoped(json, 0, key, val);
}

bool ParseSunriseSunsetIoJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t resPos = json.find("\"results\":");
    if (resPos == std::string::npos) return false;
    std::string srStr = ExtractJsonStringField(json, resPos, "sunrise");
    std::string ssStr = ExtractJsonStringField(json, resPos, "sunset");
    double sr = ParseHourFromFlexibleTimeStr(srStr);
    double ss = ParseHourFromFlexibleTimeStr(ssStr);
    if (sr < 0.0 || ss < 0.0) return false;

    outSnap.hasSunTimes = true;
    outSnap.sunriseHour = sr;
    outSnap.sunsetHour = ss;
    outSnap.mSunTimes.available = true;

    std::string ghStr = ExtractJsonStringField(json, resPos, "golden_hour");
    double ghEnd = ParseHourFromFlexibleTimeStr(ghStr);
    if (ghEnd >= 0.0) {
        outSnap.hasGoldenHour = true;
        outSnap.goldenHourText = FormatHourHhMm(ghEnd) + L"-" + FormatHourHhMm(ss);
        outSnap.mGoldenHour.available = true;
    }

    double nowH = GetLocalHourFloat();
    outSnap.isDay = IsHourInsideWindowStrict(nowH, sr, ss, 0.0);
    outSnap.mDayNight.available = true;
    FinalizeParsedForecast(outSnap, L"SunriseSunset.io");
    return true;
}

bool ParseNasaNeoWsJson(const std::string& json, ForecastSnapshot& outSnap) {
    double elemCount = 0.0;
    if (!ExtractNumberScoped(json, 0, "element_count", elemCount)) return false;
    int count = static_cast<int>(std::llround(elemCount));
    double minLd = 1e9;
    size_t pos = 0;
    while ((pos = json.find("\"lunar\"", pos)) != std::string::npos) {
        size_t q1 = json.find('"', pos + 7);
        if (q1 == std::string::npos) break;
        size_t q2 = json.find('"', q1 + 1);
        if (q2 == std::string::npos) break;
        double ld = std::strtod(json.substr(q1 + 1, q2 - q1 - 1).c_str(), nullptr);
        if (ld > 0.0 && ld < minLd) minLd = ld;
        pos = q2 + 1;
    }
    outSnap.hasNeoData = true;
    outSnap.neoCountToday = std::max(0, count);
    outSnap.neoClosestLd = (minLd < 1e8) ? minLd : 0.0;
    outSnap.mNeo.available = true;
    FinalizeParsedForecast(outSnap, L"NASA NeoWs");
    return true;
}

bool ParseNasaDonkiJson(const std::string& json, ForecastSnapshot& outSnap) {
    if (json.empty() || json[0] != '[') return false;
    std::string flareClass;
    size_t pos = 0;
    while ((pos = json.find("\"classType\"", pos)) != std::string::npos) {
        std::string ct = ExtractJsonStringField(json, pos, "classType");
        if (!ct.empty()) flareClass = ct;
        pos += 11;
    }
    outSnap.hasSpaceWeather = true;
    if (!flareClass.empty()) {
        std::wstring wFlare(flareClass.begin(), flareClass.end());
        outSnap.spaceWeatherText = L"Flare " + wFlare;
        if (flareClass.find('X') != std::string::npos) outSnap.flareKpEst = 6.8;
        else if (flareClass.find('M') != std::string::npos) outSnap.flareKpEst = 4.6;
        else outSnap.flareKpEst = 2.8;
    } else {
        outSnap.spaceWeatherText = L"Quiet solar";
        outSnap.flareKpEst = 1.8;
    }
    outSnap.mSpaceWeather.available = true;
    FinalizeParsedForecast(outSnap, L"NASA DONKI");
    return true;
}

void ParseAirQualityJson(const std::string& json, ForecastSnapshot& outSnap) {
    if (json.empty()) return;
    size_t scope = json.find("\"current\":");
    if (scope == std::string::npos) scope = 0;
    double v = 0.0;
    if (ExtractNumberAnyScope(json, scope, "european_aqi", v) || ExtractNumberAnyScope(json, scope, "us_aqi", v)) {
        outSnap.aqiValue = std::max(0.0, v);
        outSnap.mAqi.available = true;
    }
    if (ExtractNumberAnyScope(json, scope, "pm2_5", v)) {
        outSnap.pm25Value = std::max(0.0, v);
        outSnap.mAqi.available = true;
    }

    size_t hPos = json.find("\"hourly\":");
    if (hPos != std::string::npos && !outSnap.hourly.empty()) {
        auto hAqi = ExtractJsonDoubleArray(json, hPos, "european_aqi");
        if (hAqi.empty()) hAqi = ExtractJsonDoubleArray(json, hPos, "us_aqi");
        if (!hAqi.empty()) {
            size_t cnt = std::min(outSnap.hourly.size(), hAqi.size());
            for (size_t i = 0; i < cnt; ++i) {
                outSnap.hourly[i].aqiValue = std::max(0.0, hAqi[i]);
            }
        }
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
            if (pv > maxPollen) maxPollen = pv;
        }
    }
    if (anyPollen) {
        outSnap.pollenMaxValue = maxPollen;
        outSnap.mPollen.available = true;
    }
}

bool ParseWeatherJson(const std::string& json, ForecastSnapshot& outSnap, const std::wstring& srcLabel) {
    size_t scope = json.find("\"current\":");
    if (scope == std::string::npos) scope = json.find("\"current_weather\":");
    if (scope == std::string::npos) scope = 0;

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
        outSnap.humidityPct = std::max(0.0, hum);
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
    double ss = 18.5;
    if (TryExtractIsoTimeHour(json, "sunrise", sr) && TryExtractIsoTimeHour(json, "sunset", ss)) {
        outSnap.hasSunTimes = true;
        outSnap.sunriseHour = sr;
        outSnap.sunsetHour = ss;
        outSnap.mSunTimes.available = true;
    }
    double mr = 20.0;
    double ms = 7.0;
    if (TryExtractIsoTimeHour(json, "moonrise", mr) && TryExtractIsoTimeHour(json, "moonset", ms)) {
        outSnap.hasMoonTimes = true;
        outSnap.moonriseHour = mr;
        outSnap.moonsetHour = ms;
        outSnap.mMoonTimes.available = true;
    }

    ParseOpenMeteoSeriesArrays(json, outSnap);

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
    if (hasCode || outSnap.mTemp.available || outSnap.mSunTimes.available) {
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
    if (instantPos == std::string::npos) return false;
    size_t detailsPos = json.find("\"details\":", instantPos);
    if (detailsPos == std::string::npos) return false;

    double tempC = 0.0;
    if (!ExtractNumberScoped(json, detailsPos, "air_temperature", tempC)) return false;
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
        outSnap.humidityPct = std::max(0.0, hum);
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
    if (seriesPos == std::string::npos) return false;
    size_t arrStart = json.find('[', seriesPos);
    if (arrStart == std::string::npos) return false;

    int initUtcHour = 0;
    std::string initStr = ExtractJsonStringField(json, 0, "init");
    if (initStr.size() >= 10) initUtcHour = std::atoi(initStr.substr(8, 2).c_str());
    SYSTEMTIME stUtc = {};
    GetSystemTime(&stUtc);
    double elapsedHours = static_cast<double>(stUtc.wHour - initUtcHour) + static_cast<double>(stUtc.wMinute) / 60.0;
    if (elapsedHours < 0.0) elapsedHours += 24.0;

    size_t bestStart = std::string::npos;
    size_t bestEnd = std::string::npos;
    double bestDiff = 1e9;
    int braceDepth = 0;
    bool inString = false;
    bool escaped = false;
    size_t itemStart = std::string::npos;

    for (size_t i = arrStart + 1; i < json.size(); ++i) {
        char c = json[i];
        if (escaped) { escaped = false; continue; }
        if (c == '\\') { escaped = true; continue; }
        if (c == '"') { inString = !inString; continue; }
        if (inString) continue;
        if (c == '{') {
            if (braceDepth == 0) itemStart = i;
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

    if (bestStart == std::string::npos || bestEnd <= bestStart) return false;

    std::string bestItem = json.substr(bestStart, bestEnd - bestStart);
    double tempC = 0.0;
    if (!ExtractNumberScoped(bestItem, 0, "temp2m", tempC)) return false;
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
    if (weatherStr.find("ts") != std::string::npos) outSnap.weatherCode = 95;
    else if (weatherStr.find("snow") != std::string::npos) outSnap.weatherCode = 71;
    else if (weatherStr.find("rain") != std::string::npos || weatherStr.find("shower") != std::string::npos) outSnap.weatherCode = 61;
    else if (weatherStr.find("cloudy") != std::string::npos) outSnap.weatherCode = 3;
    else if (weatherStr.find("pcloudy") != std::string::npos || weatherStr.find("mcloudy") != std::string::npos) outSnap.weatherCode = 2;
    else outSnap.weatherCode = 0;
    outSnap.isDay = (weatherStr.find("night") == std::string::npos);
    outSnap.mDayNight.available = !weatherStr.empty();
    FinalizeParsedForecast(outSnap, L"7Timer!");
    return true;
}

bool ParseWeatherApiJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t curPos = json.find("\"current\":");
    if (curPos == std::string::npos) return false;
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, curPos, "temp_c", tempC)) return false;
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
    if (ExtractNumberScoped(json, curPos, "wind_degree", deg)) outSnap.windDirDeg = deg;
    double hum = 55.0;
    if (ExtractNumberScoped(json, curPos, "humidity", hum)) {
        outSnap.humidityPct = std::max(0.0, hum);
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

    size_t astroPos = json.find("\"astro\":");
    if (astroPos != std::string::npos) {
        double sr = ParseHourFromFlexibleTimeStr(ExtractJsonStringField(json, astroPos, "sunrise"));
        double ss = ParseHourFromFlexibleTimeStr(ExtractJsonStringField(json, astroPos, "sunset"));
        if (sr >= 0.0 && ss >= 0.0) {
            outSnap.hasSunTimes = true;
            outSnap.sunriseHour = sr;
            outSnap.sunsetHour = ss;
            outSnap.mSunTimes.available = true;
        }
        double mr = ParseHourFromFlexibleTimeStr(ExtractJsonStringField(json, astroPos, "moonrise"));
        double ms = ParseHourFromFlexibleTimeStr(ExtractJsonStringField(json, astroPos, "moonset"));
        if (mr >= 0.0 && ms >= 0.0) {
            outSnap.hasMoonTimes = true;
            outSnap.moonriseHour = mr;
            outSnap.moonsetHour = ms;
            outSnap.mMoonTimes.available = true;
        }
    }

    outSnap.mCondition.available = true;
    outSnap.weatherCode = (outSnap.precipMm > 0.2) ? 61 : ((outSnap.cloudCoverPct >= 80.0) ? 3 : ((outSnap.cloudCoverPct >= 25.0) ? 2 : 0));
    FinalizeParsedForecast(outSnap, L"WeatherAPI");
    return true;
}

bool ParseOpenWeatherMapJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t mainPos = json.find("\"main\":");
    if (mainPos == std::string::npos) return false;
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, mainPos, "temp", tempC)) return false;
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
        outSnap.humidityPct = std::max(0.0, hum);
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
        if (ExtractNumberScoped(json, windPos, "deg", deg)) outSnap.windDirDeg = deg;
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
    if (snowMm > 0.1) outSnap.weatherCode = 71;
    else if (outSnap.precipMm > 0.1) outSnap.weatherCode = 61;
    else outSnap.weatherCode = (outSnap.cloudCoverPct >= 80.0) ? 3 : ((outSnap.cloudCoverPct >= 25.0) ? 2 : 0);
    FinalizeParsedForecast(outSnap, L"OWM");
    return true;
}

bool ParseVisualCrossingJson(const std::string& json, ForecastSnapshot& outSnap) {
    size_t curPos = json.find("\"currentConditions\":");
    if (curPos == std::string::npos) curPos = 0;
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, curPos, "temp", tempC)) return false;
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
        outSnap.humidityPct = std::max(0.0, hum);
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
        outSnap.mWind.available = true;
    }
    double gustKmh = outSnap.windSpeedKmh;
    if (ExtractNumberScoped(json, curPos, "windgust", gustKmh)) {
        outSnap.windGustsKmh = std::max(0.0, gustKmh);
        outSnap.mGusts.available = true;
    }
    double deg = 0.0;
    if (ExtractNumberScoped(json, curPos, "winddir", deg)) outSnap.windDirDeg = deg;
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

    double sr = ParseHourFromFlexibleTimeStr(ExtractJsonStringField(json, curPos, "sunrise"));
    double ss = ParseHourFromFlexibleTimeStr(ExtractJsonStringField(json, curPos, "sunset"));
    if (sr >= 0.0 && ss >= 0.0) {
        outSnap.hasSunTimes = true;
        outSnap.sunriseHour = sr;
        outSnap.sunsetHour = ss;
        outSnap.mSunTimes.available = true;
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
    if (valPos == std::string::npos) valPos = 0;
    double tempC = 0.0;
    if (!ExtractNumberScoped(json, valPos, "temperature", tempC)) return false;
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
        outSnap.humidityPct = std::max(0.0, hum);
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
        outSnap.mWind.available = true;
    }
    double gustMs = 0.0;
    if (ExtractNumberScoped(json, valPos, "windGust", gustMs)) {
        outSnap.windGustsKmh = std::max(0.0, gustMs * 3.6);
        outSnap.mGusts.available = true;
    } else {
        outSnap.windGustsKmh = outSnap.windSpeedKmh;
    }
    double deg = 0.0;
    if (ExtractNumberScoped(json, valPos, "windDirection", deg)) outSnap.windDirDeg = deg;
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
    if (src == L"off" || src == L"auto") return false;
    const std::wstring amp(1, static_cast<wchar_t>(38));
    std::wstring apiKey = GetProviderApiKey(src, cfg);

    if (src == L"custom_api") {
        if (cfg.customApiUrl.empty()) return false;
        bool hasLatPh = (cfg.customApiUrl.find(L"{lat}") != std::wstring::npos);
        bool hasLonPh = (cfg.customApiUrl.find(L"{lon}") != std::wstring::npos);
        if ((hasLatPh || hasLonPh) && !cfg.hasValidCoords) return false;
        std::wstring resolvedUrl = cfg.customApiUrl;
        if (cfg.hasValidCoords) {
            resolvedUrl = ReplaceAllSubstrings(resolvedUrl, L"{lat}", FormatCoordDot(cfg.latitude));
            resolvedUrl = ReplaceAllSubstrings(resolvedUrl, L"{lon}", FormatCoordDot(cfg.longitude));
        }
        resolvedUrl = ReplaceAllSubstrings(resolvedUrl, L"{key}", cfg.customApiKey);
        std::string body = HttpGetUtf8(resolvedUrl);
        return !body.empty() && ParseWeatherJson(body, outSnap, L"Custom API");
    }
    if (src == L"local_file") {
        if (cfg.localWeatherPath.empty()) return false;
        HANDLE hFile = CreateFileW(
            cfg.localWeatherPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
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
        return !content.empty() && ParseWeatherJson(content, outSnap, L"Local File");
    }
    if (src == L"nasa_neows") {
        if (apiKey.empty()) return false;
        char dateBuf[16] = {};
        FormatIsoDateUtc(0, dateBuf);
        std::wstring wDate(dateBuf, dateBuf + strlen(dateBuf));
        std::wstring url = L"https://api.nasa.gov/neo/rest/v1/feed?start_date=" + wDate +
                           amp + L"end_date=" + wDate + amp + L"api_key=" + apiKey;
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseNasaNeoWsJson(body, outSnap);
    }
    if (src == L"nasa_donki") {
        char startBuf[16] = {};
        char endBuf[16] = {};
        FormatIsoDateUtc(-2, startBuf);
        FormatIsoDateUtc(0, endBuf);
        std::wstring wStart(startBuf, startBuf + strlen(startBuf));
        std::wstring wEnd(endBuf, endBuf + strlen(endBuf));
        std::wstring url = L"https://ccmc.gsfc.nasa.gov/DONKI-API/get/FLR?startDate=" + wStart +
                           amp + L"endDate=" + wEnd;
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseNasaDonkiJson(body, outSnap);
    }

    if (!cfg.hasValidCoords) return false;
    std::wstring latStr = FormatCoordDot(cfg.latitude);
    std::wstring lonStr = FormatCoordDot(cfg.longitude);

    if (src == L"sunrise_sunset_io") {
        std::wstring url = L"https://api.sunrisesunset.io/json?lat=" + latStr +
                           amp + L"lng=" + lonStr + amp + L"time_format=24";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseSunriseSunsetIoJson(body, outSnap);
    }
    if (src == L"open_meteo") {
        std::wstring url =
            L"https://api.open-meteo.com/v1/forecast?latitude=" + latStr +
            amp + L"longitude=" + lonStr +
            amp + L"current=temperature_2m,relative_humidity_2m,apparent_temperature,precipitation,weather_code,cloud_cover,wind_speed_10m,wind_direction_10m,wind_gusts_10m,is_day,uv_index,pressure_msl,dew_point_2m,visibility" +
            amp + L"hourly=temperature_2m,precipitation,weather_code,wind_speed_10m,wind_direction_10m,pressure_msl,uv_index" +
            amp + L"daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_sum,wind_speed_10m_max,wind_direction_10m_dominant,sunrise,sunset" +
            amp + L"forecast_days=14" +
            amp + L"timezone=auto";
        std::string body = HttpGetUtf8(url);
        if (body.empty() || !ParseWeatherJson(body, outSnap, L"Open-Meteo")) return false;
        if (NeedsAirQualityFetch(cfg)) {
            std::wstring aqUrl =
                L"https://air-quality-api.open-meteo.com/v1/air-quality?latitude=" + latStr +
                amp + L"longitude=" + lonStr +
                amp + L"current=european_aqi,pm2_5,alder_pollen,birch_pollen,grass_pollen,ragweed_pollen" +
                amp + L"hourly=european_aqi" +
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
        std::string body = HttpGetUtf8(url, L"WindhawkWeatherOverlay/3.2.9 github.com/alembiqspace");
        return !body.empty() && ParseMetNorwayJson(body, outSnap);
    }
    if (src == L"seven_timer") {
        std::wstring url = L"https://www.7timer.info/bin/api.pl?lon=" + lonStr +
                           amp + L"lat=" + latStr + amp + L"product=civil" + amp + L"output=json";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseSevenTimerJson(body, outSnap);
    }
    if (src == L"weatherapi") {
        if (apiKey.empty()) return false;
        std::wstring url = L"https://api.weatherapi.com/v1/current.json?key=" + apiKey +
                           amp + L"q=" + latStr + L"," + lonStr + amp + L"aqi=yes";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseWeatherApiJson(body, outSnap);
    }
    if (src == L"openweathermap") {
        if (apiKey.empty()) return false;
        std::wstring url = L"https://api.openweathermap.org/data/2.5/weather?lat=" + latStr +
                           amp + L"lon=" + lonStr + amp + L"appid=" + apiKey + amp + L"units=metric";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseOpenWeatherMapJson(body, outSnap);
    }
    if (src == L"visual_crossing") {
        if (apiKey.empty()) return false;
        std::wstring url = L"https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
                           latStr + L"," + lonStr +
                           L"?unitGroup=metric" + amp + L"include=current" + amp + L"key=" + apiKey + amp + L"contentType=json";
        std::string body = HttpGetUtf8(url);
        return !body.empty() && ParseVisualCrossingJson(body, outSnap);
    }
    if (src == L"tomorrow_io") {
        if (apiKey.empty()) return false;
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
    if (!dst.mTodayMinMax.available && src.mTodayMinMax.available) {
        dst.todayMinC = src.todayMinC;
        dst.todayMaxC = src.todayMaxC;
        dst.hasTodayMinMax = true;
        dst.mTodayMinMax = src.mTodayMinMax;
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
    if (!dst.hasSunTimes && src.hasSunTimes) {
        dst.hasSunTimes = true;
        dst.sunriseHour = src.sunriseHour;
        dst.sunsetHour = src.sunsetHour;
        dst.mSunTimes = src.mSunTimes;
    }
    if (!dst.hasMoonTimes && src.hasMoonTimes) {
        dst.hasMoonTimes = true;
        dst.moonriseHour = src.moonriseHour;
        dst.moonsetHour = src.moonsetHour;
        dst.mMoonTimes = src.mMoonTimes;
    }
    if (!dst.hasGoldenHour && src.hasGoldenHour) {
        dst.hasGoldenHour = true;
        dst.goldenHourText = src.goldenHourText;
        dst.mGoldenHour = src.mGoldenHour;
    }
    if (!dst.hasNeoData && src.hasNeoData) {
        dst.hasNeoData = true;
        dst.neoCountToday = src.neoCountToday;
        dst.neoClosestLd = src.neoClosestLd;
        dst.mNeo = src.mNeo;
    }
    if (!dst.hasSpaceWeather && src.hasSpaceWeather) {
        dst.hasSpaceWeather = true;
        dst.flareKpEst = src.flareKpEst;
        dst.spaceWeatherText = src.spaceWeatherText;
        dst.mSpaceWeather = src.mSpaceWeather;
    }
    if (!dst.mHourly.available && !src.hourly.empty()) {
        dst.hourly = src.hourly;
        dst.mHourly = src.mHourly;
    }
    if (!dst.mDaily.available && !src.daily.empty()) {
        dst.daily = src.daily;
        dst.mDaily = src.mDaily;
    }
}

bool HasAllCoreSnapshotFields(const ForecastSnapshot& s) {
    return s.mCondition.available && s.mTemp.available && s.mTodayMinMax.available && s.mFeelsLike.available &&
           s.mHumidity.available && s.mPressure.available && s.mWind.available &&
           s.mGusts.available && s.mClouds.available && s.mPrecip.available &&
           s.mUv.available && s.mAqi.available && s.mPollen.available &&
           s.mVisibility.available && s.mDewPoint.available && s.mDayNight.available &&
           s.mSunTimes.available && s.mHourly.available && s.mDaily.available;
}

bool HasAllAstroFields(const ForecastSnapshot& s, const std::wstring& astroSrc) {
    if (astroSrc == L"nasa_neows") return s.mNeo.available;
    if (astroSrc == L"nasa_donki") return s.mSpaceWeather.available;
    return s.hasSunTimes && s.hasGoldenHour;
}

bool FetchAggregatedFromPrimary(const std::wstring& primarySrc, const Settings& cfg,
                                std::set<std::wstring>& queried, bool astroPassOnly,
                                ForecastSnapshot& outSnap) {
    if (primarySrc == L"off") return false;
    std::vector<std::wstring> ordered = BuildOrderedProvidersForPrimary(primarySrc);
    bool anyOk = outSnap.valid;
    int sourcesUsed = 0;

    for (const auto& src : ordered) {
        if (queried.count(src) > 0) continue;

        bool isNasa = (src == L"nasa_neows" || src == L"nasa_donki");
        bool explicitlySelected = (src == primarySrc || src == cfg.primarySource || src == cfg.astroPrimarySource);
        if (isNasa && !explicitlySelected) continue;
        if (src == L"nasa_neows" && cfg.nasaApiKey.empty()) continue;

        if (!isNasa && ProviderRequiresApiKey(src) && GetProviderApiKey(src, cfg).empty()) continue;
        if (src == L"custom_api" && cfg.customApiUrl.empty()) continue;
        if (src == L"local_file" && cfg.localWeatherPath.empty()) continue;

        queried.insert(src);
        ForecastSnapshot part;
        if (FetchSingleWeatherSource(src, cfg, part)) {
            if (!anyOk) {
                outSnap = part;
                anyOk = true;
            } else {
                MergeMissingSnapshotFields(outSnap, part);
            }
            sourcesUsed++;
            if (astroPassOnly) {
                if (HasAllAstroFields(outSnap, primarySrc)) break;
            } else {
                if (HasAllCoreSnapshotFields(outSnap)) break;
            }
        }
    }

    if (!anyOk) return false;
    ApplyForecastMetrics(
        outSnap.weatherCode, outSnap.windSpeedKmh, outSnap.windGustsKmh,
        outSnap.tempC, outSnap.apparentTempC, outSnap.humidityPct,
        static_cast<int>(outSnap.cloudCoverPct), outSnap.visibilityM,
        outSnap.precipMm, outSnap.aqiValue, outSnap.pm25Value,
        outSnap.pollenMaxValue, outSnap.isDay, outSnap.precipKind, outSnap.effects);
    outSnap.valid = outSnap.mTemp.available || outSnap.mCondition.available || outSnap.mSunTimes.available ||
                    outSnap.mNeo.available || outSnap.mSpaceWeather.available;
    StampForecastUpdateTime(outSnap);
    if (!astroPassOnly && sourcesUsed > 1) {
        outSnap.sourceLabel = FormatProviderHudBadge(primarySrc) + L" +" + std::to_wstring(sourcesUsed - 1);
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
                g_lastFetchedAstroSource = cfg.astroPrimarySource;
                g_lastFetchedLat = cfg.latitude;
                g_lastFetchedLon = cfg.longitude;
            }
        }
        DWORD waitMs = static_cast<DWORD>(intervalSec) * 1000;
        if (needFetch) {
            std::set<std::wstring> queried;
            ForecastSnapshot snap;
            bool ok = FetchAggregatedFromPrimary(cfg.primarySource, cfg, queried, false, snap);
            if (cfg.astroPrimarySource != L"off" &&
                !HasAllAstroFields(snap, cfg.astroPrimarySource)) {
                if (FetchAggregatedFromPrimary(cfg.astroPrimarySource, cfg, queried, true, snap)) {
                    ok = true;
                }
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
            if (HasAnyForecastConsumer(g_settings)) g_forecastDirty = true;
        }
    }
}

float QueryMonitorDpiScale(HMONITOR hMon) {
    using GetDpiForMonitorFn = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static HMODULE hShcore = GetModuleHandleW(L"shcore.dll")
        ? GetModuleHandleW(L"shcore.dll")
        : LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    static auto pGetDpiForMonitor = hShcore
        ? reinterpret_cast<GetDpiForMonitorFn>(GetProcAddress(hShcore, "GetDpiForMonitor"))
        : nullptr;
    if (pGetDpiForMonitor && hMon) {
        UINT dpiX = 96;
        UINT dpiY = 96;
        if (SUCCEEDED(pGetDpiForMonitor(hMon, 0, &dpiX, &dpiY)) && dpiX >= 72 && dpiX <= 480) {
            return std::clamp(static_cast<float>(dpiX) / 96.0f, 1.0f, 3.0f);
        }
    }
    HDC hdc = GetDC(nullptr);
    int dpi = hdc ? GetDeviceCaps(hdc, LOGPIXELSX) : 96;
    if (hdc) ReleaseDC(nullptr, hdc);
    return std::clamp(static_cast<float>(dpi) / 96.0f, 1.0f, 3.0f);
}

int ComputeBottomLimitYForRect(const Settings& cfg, int monTop, int monBottom, int monWorkBottom) {
    int baseBottom = monBottom;
    if (cfg.bottomBoundaryMode == L"above_taskbar") {
        baseBottom = std::min(baseBottom, monWorkBottom);
        HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
        RECT trayRect = {};
        if (hTray && GetWindowRect(hTray, &trayRect)) {
            baseBottom = std::min(baseBottom, static_cast<int>(trayRect.top));
        }
    } else if (cfg.bottomBoundaryMode == L"custom_offset") {
        baseBottom -= cfg.bottomInsetPx;
    }
    return std::clamp(baseBottom, monTop + 20, monBottom);
}

struct EnumMonEntry {
    HMONITOR hMon;
    MONITORINFO mi;
};

BOOL CALLBACK CollectMonitorRectsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<EnumMonEntry>*>(lParam);
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(hMon, &mi)) {
        list->push_back(EnumMonEntry{hMon, mi});
    }
    return TRUE;
}

void UpdateCanvasGeometry(const Settings& cfg) {
    g_monitorRects.clear();
    auto buildMon = [&](HMONITOR hMon, const MONITORINFO& mi) -> MonitorCanvasRect {
        float left = static_cast<float>(mi.rcMonitor.left - g_canvasLeft);
        float top = static_cast<float>(mi.rcMonitor.top - g_canvasTop);
        float w = static_cast<float>(std::max(1L, mi.rcMonitor.right - mi.rcMonitor.left));
        float h = static_cast<float>(std::max(1L, mi.rcMonitor.bottom - mi.rcMonitor.top));
        float botRel = static_cast<float>(
            ComputeBottomLimitYForRect(cfg, static_cast<int>(mi.rcMonitor.top),
                                       static_cast<int>(mi.rcMonitor.bottom),
                                       static_cast<int>(mi.rcWork.bottom)) - g_canvasTop);
        float inL = std::min(w - 20.0f, static_cast<float>(std::max(0, cfg.leftInsetPx)));
        float inR = std::max(inL + 20.0f, w - static_cast<float>(std::max(0, cfg.rightInsetPx)));
        float inT = std::min(std::max(0.0f, botRel - 20.0f), static_cast<float>(std::max(0, cfg.topInsetPx)));
        float dpiScale = QueryMonitorDpiScale(hMon);
        return MonitorCanvasRect{left, top, w, h, inL, inT, inR, botRel, dpiScale};
    };

    if (cfg.multiMonitorMode == L"all") {
        g_canvasLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
        g_canvasTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
        g_canvasWidth = std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN));
        g_canvasHeight = std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN));

        std::vector<EnumMonEntry> rawMons;
        EnumDisplayMonitors(nullptr, nullptr, CollectMonitorRectsProc, reinterpret_cast<LPARAM>(&rawMons));
        for (const auto& e : rawMons) {
            if (e.mi.dwFlags & MONITORINFOF_PRIMARY) {
                g_monitorRects.insert(g_monitorRects.begin(), buildMon(e.hMon, e.mi));
            } else {
                g_monitorRects.push_back(buildMon(e.hMon, e.mi));
            }
        }
    } else {
        g_canvasLeft = 0;
        g_canvasTop = 0;
        g_canvasWidth = std::max(1, GetSystemMetrics(SM_CXSCREEN));
        g_canvasHeight = std::max(1, GetSystemMetrics(SM_CYSCREEN));

        HMONITOR primMon = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfoW(primMon, &mi)) {
            g_monitorRects.push_back(buildMon(primMon, mi));
        }
    }

    if (g_monitorRects.empty()) {
        float w = static_cast<float>(g_canvasWidth);
        float h = static_cast<float>(g_canvasHeight);
        float botRel = std::max(20.0f, h - static_cast<float>(cfg.bottomInsetPx));
        float inL = std::min(w - 20.0f, static_cast<float>(std::max(0, cfg.leftInsetPx)));
        float inR = std::max(inL + 20.0f, w - static_cast<float>(std::max(0, cfg.rightInsetPx)));
        float inT = std::min(botRel - 20.0f, static_cast<float>(std::max(0, cfg.topInsetPx)));
        g_monitorRects.push_back(MonitorCanvasRect{0.0f, 0.0f, w, h, inL, inT, inR, botRel, 1.0f});
    }
    g_primaryDpiScale = g_monitorRects.front().dpiScale;
}

int ComputeBottomLimitY(const Settings& cfg) {
    if (!g_monitorRects.empty()) {
        return static_cast<int>(g_canvasTop + g_monitorRects.front().bottomLimitRelY);
    }
    return std::clamp(g_canvasTop + g_canvasHeight - cfg.bottomInsetPx, g_canvasTop + 10, g_canvasTop + g_canvasHeight);
}

bool IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_overlayHwnd || fg == g_hudHwnd) return false;
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
    static HMODULE hDwmapi = GetModuleHandleW(L"dwmapi.dll")
        ? GetModuleHandleW(L"dwmapi.dll")
        : LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hDwmapi) return false;
    static auto pfn = reinterpret_cast<DwmGetWindowAttributeFn>(GetProcAddress(hDwmapi, "DwmGetWindowAttribute"));
    DWORD cloaked = 0;
    return pfn && SUCCEEDED(pfn(hwnd, kDwmwaCloaked, &cloaked, sizeof(cloaked))) && cloaked != 0;
}

ULONGLONG g_lastWindowScanTick = 0;
bool g_cachedDesktopOccluded = false;

bool RefreshDesktopOcclusionAndWindows(bool pauseOnFullscreen, bool collectWindows, bool forceRefresh) {
    ULONGLONG nowTick = GetTickCount64();
    if (!forceRefresh && (nowTick - g_lastWindowScanTick) < 250) {
        return g_cachedDesktopOccluded;
    }
    g_lastWindowScanTick = nowTick;
    g_bounceWindows.clear();
    g_cachedDesktopOccluded = false;

    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defView = nullptr;
    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr)) != nullptr) {
        if ((defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr)) != nullptr) break;
    }
    if (!defView && progman) defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);

    HMONITOR primMon = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO primMi = {sizeof(primMi)};
    bool hasPrimWork = GetMonitorInfoW(primMon, &primMi) != 0;

    std::vector<WindowSurfaceBox> occluders;
    HWND topWnd = GetTopWindow(nullptr);
    while (topWnd) {
        if (topWnd != g_overlayHwnd && topWnd != g_hudHwnd &&
            topWnd != progman && topWnd != defView &&
            IsWindowVisible(topWnd) && !IsIconic(topWnd) && !IsWindowCloaked(topWnd)) {
            LONG exStyle = GetWindowLongW(topWnd, GWL_EXSTYLE);
            bool isClickThrough = ((exStyle & WS_EX_TRANSPARENT) != 0) && ((exStyle & WS_EX_LAYERED) != 0);
            if (!(exStyle & WS_EX_TOOLWINDOW) && !isClickThrough) {
                wchar_t cls[128] = {};
                GetClassNameW(topWnd, cls, _countof(cls));
                std::wstring clsStr = cls;
                if (clsStr != L"WorkerW" && clsStr != L"Shell_TrayWnd" && clsStr != L"Shell_SecondaryTrayWnd") {
                    RECT r = {};
                    if (GetWindowRect(topWnd, &r)) {
                        bool coversFull = (r.left <= g_canvasLeft && r.top <= g_canvasTop &&
                                           r.right >= g_canvasLeft + g_canvasWidth &&
                                           r.bottom >= g_canvasTop + g_canvasHeight);
                        bool coversMaximized = hasPrimWork &&
                                               (r.left <= primMi.rcWork.left + 2 &&
                                                r.top <= primMi.rcWork.top + 2 &&
                                                r.right >= primMi.rcWork.right - 2 &&
                                                r.bottom >= primMi.rcWork.bottom - 2);
                        if (pauseOnFullscreen && (coversFull || (coversMaximized && g_monitorRects.size() == 1))) {
                            g_cachedDesktopOccluded = true;
                            return true;
                        }
                        if (collectWindows && (r.right - r.left) >= 40 && (r.bottom - r.top) >= 30) {
                            WindowSurfaceBox box{
                                static_cast<float>(r.left - g_canvasLeft),
                                static_cast<float>(r.right - g_canvasLeft),
                                static_cast<float>(r.top - g_canvasTop),
                                static_cast<float>(r.bottom - g_canvasTop)
                            };
                            bool fullyCovered = false;
                            for (const auto& above : occluders) {
                                if (above.left <= box.left && above.right >= box.right &&
                                    above.top <= box.top && above.bottom >= box.bottom) {
                                    fullyCovered = true;
                                    break;
                                }
                            }
                            if (!fullyCovered) {
                                g_bounceWindows.push_back(box);
                            }
                            occluders.push_back(box);
                        }
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
    if (mode == L"progman_direct") return (progman && progman != overlay) ? progman : HWND_BOTTOM;

    HWND defView = FindDesktopIconsWindow();
    if (defView) {
        HWND parent = GetParent(defView);
        if (parent && parent != progman) {
            return (parent != overlay) ? parent : HWND_BOTTOM;
        }
        if (progman && progman != overlay) return progman;
    }
    return HWND_BOTTOM;
}

void PositionDesktopOverlay(HWND overlay, const std::wstring& mode, bool forceShow) {
    if (!overlay) return;
    HWND insertAfter = FindDesktopInsertAfter(mode, overlay);
    UINT flags = SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS | (forceShow ? SWP_SHOWWINDOW : 0);
    SetWindowPos(overlay, insertAfter, g_canvasLeft, g_canvasTop, g_canvasWidth, g_canvasHeight, flags);
}

void PositionHudWindow(HWND hudWnd, int left, int top, int width, int height, bool forceShow) {
    if (!hudWnd) return;
    HWND defView = FindDesktopIconsWindow();
    HWND desktopHost = defView ? GetParent(defView) : FindWindowW(L"Progman", nullptr);
    HWND insertAfter = HWND_BOTTOM;
    if (desktopHost) {
        HWND aboveDesktop = GetWindow(desktopHost, GW_HWNDPREV);
        while (aboveDesktop && (aboveDesktop == hudWnd || aboveDesktop == g_overlayHwnd)) {
            aboveDesktop = GetWindow(aboveDesktop, GW_HWNDPREV);
        }
        if (aboveDesktop) insertAfter = aboveDesktop;
    }
    UINT flags = SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS | (forceShow ? SWP_SHOWWINDOW : 0);
    SetWindowPos(hudWnd, insertAfter, left, top, width, height, flags);
}

bool IsRelPointInsideHud(float relX, float relY) {
    float r = g_hudBtnR.load();
    if (r > 2.0f) {
        float dx = relX - g_hudBtnCx.load();
        float dy = relY - g_hudBtnCy.load();
        if ((dx * dx + dy * dy) <= (r * r)) return true;
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

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK HudWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_NCHITTEST) {
        float relX = static_cast<float>(static_cast<short>(LOWORD(lParam)) - g_canvasLeft);
        float relY = static_cast<float>(static_cast<short>(HIWORD(lParam)) - g_canvasTop);
        return IsRelPointInsideHud(relX, relY) ? HTCLIENT : HTTRANSPARENT;
    }
    if (msg == WM_LBUTTONDOWN) {
        int winOffX = g_hudWinLeft.load() - g_canvasLeft;
        int winOffY = g_hudWinTop.load() - g_canvasTop;
        int relX = static_cast<short>(LOWORD(lParam)) + winOffX;
        int relY = static_cast<short>(HIWORD(lParam)) + winOffY;
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
            {
                std::lock_guard<std::mutex> lock(g_hudUiMutex);
                for (auto it = g_hudControls.rbegin(); it != g_hudControls.rend(); ++it) {
                    const auto& ctrl = *it;
                    if (relX >= ctrl.rect.left && relX <= ctrl.rect.right &&
                        relY >= ctrl.rect.top && relY <= ctrl.rect.bottom) {
                        if (!ctrl.actionId.empty()) actionToRun = ctrl.actionId;
                        break;
                    }
                }
            }
            if (!actionToRun.empty()) CycleSettingOption(actionToRun);
            return 0;
        }
    } else if (msg == WM_MOUSEMOVE) {
        int winOffX = g_hudWinLeft.load() - g_canvasLeft;
        int winOffY = g_hudWinTop.load() - g_canvasTop;
        int relX = static_cast<short>(LOWORD(lParam)) + winOffX;
        int relY = static_cast<short>(HIWORD(lParam)) + winOffY;
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hoverRelX = static_cast<float>(relX);
        g_hoverRelY = static_cast<float>(relY);
        if (g_draggingHudBtn) {
            int deltaX = (g_canvasLeft + relX) - g_dragStartScreenX;
            int deltaY = (g_canvasTop + relY) - g_dragStartScreenY;
            if (std::abs(deltaX) > 5 || std::abs(deltaY) > 5) g_movedDuringDrag = true;
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
                std::lock_guard<std::mutex> lock(g_hudUiMutex);
                g_hudScrollPx = 0;
                g_hudSettingsView = false;
                g_hudSourcesView = false;
                g_expandedSection = ExpandedSection::None;
                g_nowDetail = DetailWidgetKind::None;
                g_hoursDetail = DetailWidgetKind::None;
                g_daysDetail = DetailWidgetKind::None;
                g_astroDetail = DetailWidgetKind::None;
                g_hoursPage = 0;
                g_daysPage = 0;
                if (nextExp) {
                    g_hudCategory = HudCategory::Meteo;
                    g_hudDataTab = HudDataTab::Now;
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
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void SpawnFallingItem(FallingItem& item, std::mt19937& rng, PrecipKind kind, float vx, float vy, bool randomY, QualityTier tier) {
    std::uniform_real_distribution<float> phaseDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> alphaDist(0.65f, 0.96f);
    std::uniform_real_distribution<float> u01Dist(0.0f, 1.0f);

    item.kind = kind;
    item.vx = vx;
    item.vy = vy;
    item.slidingSide = false;
    item.hitAnimTimer = 0.0f;
    item.swayPhase = phaseDist(rng);
    item.angle = phaseDist(rng);
    item.depthTier = (tier == QualityTier::Low) ? 1 : static_cast<int>(rng() % 3);

    if (kind == PrecipKind::Sleet) {
        item.isFlakeSub = (u01Dist(rng) < 0.5f);
    } else if (kind == PrecipKind::Snow || kind == PrecipKind::Blizzard) {
        item.isFlakeSub = true;
    } else {
        item.isFlakeSub = false;
    }

    if (kind == PrecipKind::Drizzle) {
        item.alpha = 0.34f + u01Dist(rng) * 0.22f;
    } else if (kind == PrecipKind::Downpour) {
        item.alpha = 0.78f + u01Dist(rng) * 0.22f;
    } else {
        item.alpha = alphaDist(rng);
    }

    const MonitorCanvasRect& mon = g_monitorRects.empty()
        ? MonitorCanvasRect{0.0f, 0.0f, static_cast<float>(g_canvasWidth), static_cast<float>(g_canvasHeight),
                            0.0f, 0.0f, static_cast<float>(g_canvasWidth), static_cast<float>(g_canvasHeight), 1.0f}
        : g_monitorRects[rng() % g_monitorRects.size()];

    float minX = mon.left + mon.innerLeftRel;
    float maxX = std::max(minX + 1.0f, mon.left + mon.innerRightRel);
    float minY = mon.top + mon.innerTopRel;
    float maxY = std::max(minY + 1.0f, mon.top + mon.bottomLimitRelY);
    float spanW = maxX - minX;
    float spanH = maxY - minY;

    if (randomY) {
        item.x = minX + u01Dist(rng) * spanW;
        item.y = minY + u01Dist(rng) * spanH;
    } else {
        float absVx = std::abs(vx);
        float absVy = std::max(20.0f, std::abs(vy));
        float topFlux = absVy * spanW;
        float sideFlux = absVx * spanH;
        float totalFlux = std::max(1.0f, topFlux + sideFlux);
        float pSide = sideFlux / totalFlux;
        float jitterDepth = u01Dist(rng) * 38.0f * mon.dpiScale;

        if (absVx > 5.0f && u01Dist(rng) < pSide) {
            item.x = (vx < 0.0f) ? (maxX + jitterDepth) : (minX - jitterDepth);
            item.y = minY + u01Dist(rng) * spanH;
        } else {
            item.x = minX + u01Dist(rng) * spanW;
            item.y = minY - jitterDepth;
        }
    }

    float depthMul = ((tier == QualityTier::Low)
        ? 1.0f
        : ((item.depthTier == 0) ? 0.75f : ((item.depthTier == 2) ? 1.30f : 1.0f))) * mon.dpiScale;

    if (kind == PrecipKind::Drizzle) {
        item.size = 7.5f * depthMul;
        item.spin = 0.0f;
    } else if (kind == PrecipKind::Downpour) {
        item.size = 21.0f * depthMul;
        item.spin = 0.0f;
    } else if (kind == PrecipKind::Rain) {
        item.size = 13.5f * depthMul;
        item.spin = 0.0f;
    } else if (kind == PrecipKind::Hail) {
        item.size = (5.0f + (rng() % 30) * 0.1f) * depthMul;
        item.spin = ((rng() % 100) - 50) * 0.05f * ((tier == QualityTier::High) ? 1.45f : 1.1f);
    } else if (kind == PrecipKind::Blizzard) {
        item.size = (1.8f + u01Dist(rng) * 3.8f) * depthMul;
        item.spin = 0.0f;
    } else if (kind == PrecipKind::Sleet) {
        if (item.isFlakeSub) {
            item.size = (2.0f + u01Dist(rng) * 3.4f) * depthMul;
            item.spin = 0.0f;
        } else {
            item.size = 10.5f * depthMul;
            item.vy = vy * 1.25f;
            item.spin = 0.0f;
        }
    } else {
        item.size = (2.4f + u01Dist(rng) * 5.2f) * depthMul;
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
        if (g_hudItalicFormat) { g_hudItalicFormat->Release(); g_hudItalicFormat = nullptr; }
        if (g_hudMediumFormat) { g_hudMediumFormat->Release(); g_hudMediumFormat = nullptr; }
        if (g_hudHeroFormat) { g_hudHeroFormat->Release(); g_hudHeroFormat = nullptr; }
        g_cachedFontSizePx = fontSizePx;
    }
    if (std::abs(g_cachedCircleFontPx - circleFontPx) > 0.1f) {
        if (g_hudCircleFormat) { g_hudCircleFormat->Release(); g_hudCircleFormat = nullptr; }
        g_cachedCircleFontPx = circleFontPx;
    }
    if (!g_hudTitleFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSizePx + 0.5f, L"en-US", &g_hudTitleFormat);
        if (g_hudTitleFormat) g_hudTitleFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    if (!g_hudBodyFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSizePx, L"en-US", &g_hudBodyFormat);
        if (g_hudBodyFormat) g_hudBodyFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    if (!g_hudItalicFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                                        DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL, fontSizePx, L"en-US", &g_hudItalicFormat);
        if (g_hudItalicFormat) g_hudItalicFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    if (!g_hudMediumFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSizePx * 2.0f, L"en-US", &g_hudMediumFormat);
        if (g_hudMediumFormat) {
            g_hudMediumFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_hudMediumFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_hudMediumFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
    }
    if (!g_hudHeroFormat) {
        g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD,
                                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSizePx * 3.0f, L"en-US", &g_hudHeroFormat);
        if (g_hudHeroFormat) {
            g_hudHeroFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_hudHeroFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            g_hudHeroFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
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
    return g_hudTitleFormat && g_hudBodyFormat && g_hudItalicFormat && g_hudMediumFormat &&
           g_hudHeroFormat && g_hudCircleFormat && g_hudPillFormat;
}

void CleanupDirectWrite() {
    if (g_hudPillFormat) { g_hudPillFormat->Release(); g_hudPillFormat = nullptr; }
    if (g_hudCircleFormat) { g_hudCircleFormat->Release(); g_hudCircleFormat = nullptr; }
    if (g_hudHeroFormat) { g_hudHeroFormat->Release(); g_hudHeroFormat = nullptr; }
    if (g_hudMediumFormat) { g_hudMediumFormat->Release(); g_hudMediumFormat = nullptr; }
    if (g_hudTitleFormat) { g_hudTitleFormat->Release(); g_hudTitleFormat = nullptr; }
    if (g_hudBodyFormat) { g_hudBodyFormat->Release(); g_hudBodyFormat = nullptr; }
    if (g_hudItalicFormat) { g_hudItalicFormat->Release(); g_hudItalicFormat = nullptr; }
    if (g_dwriteFactory) { g_dwriteFactory->Release(); g_dwriteFactory = nullptr; }
    g_cachedFontSizePx = 0.0f;
    g_cachedCircleFontPx = 0.0f;
}

bool EnsureSurface(CachedSurface& surf, int width, int height, D2D1_COLOR_F snowCol, bool hasCustomSnow) {
    if (width <= 0 || height <= 0) return false;
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
        if (!surf.dib) { CleanupSurface(surf); return false; }
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

void PresentSurfaceAt(HWND hwnd, CachedSurface& surf, int dstLeft, int dstTop, int width, int height) {
    if (!hwnd || !IsWindow(hwnd) || !surf.memDC || width <= 0 || height <= 0) return;
    POINT ptDst = {dstLeft, dstTop};
    POINT ptSrc = {0, 0};
    SIZE sizeWnd = {width, height};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    if (!UpdateLayeredWindow(hwnd, nullptr, &ptDst, &sizeWnd, surf.memDC, &ptSrc, 0, &blend, ULW_ALPHA)) {
        Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
    }
}

void DrawSurfaceSplash(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                       const Settings& cfg, QualityTier tier, PrecipKind kind, float x, float surfaceY, float progress01, float alpha) {
    if (kind == PrecipKind::Drizzle) return;
    float t = std::clamp(progress01, 0.0f, 1.0f);
    float fade = 1.0f - t;
    brush->SetOpacity(1.0f);
    if (kind == PrecipKind::Hail) {
        brush->SetColor(D2D1::ColorF(cfg.colorHail.r, cfg.colorHail.g, cfg.colorHail.b, std::clamp(alpha * 0.90f * fade, 0.02f, 0.90f)));
        float spread = 4.0f + t * 14.0f;
        float lift = (1.0f - (2.0f * t - 1.0f) * (2.0f * t - 1.0f)) * 11.0f;
        rt->FillRectangle(D2D1::RectF(x - spread - 1.5f, surfaceY - lift - 1.5f, x - spread + 1.5f, surfaceY - lift + 1.5f), brush);
        rt->FillRectangle(D2D1::RectF(x + spread - 1.5f, surfaceY - lift - 1.5f, x + spread + 1.5f, surfaceY - lift + 1.5f), brush);
    } else if (kind == PrecipKind::Snow || kind == PrecipKind::Blizzard) {
        brush->SetColor(D2D1::ColorF(cfg.colorSnow.r, cfg.colorSnow.g, cfg.colorSnow.b, std::clamp(alpha * 0.78f * fade, 0.02f, 0.80f)));
        float rx = 3.5f + t * 8.0f;
        float ry = 2.0f * fade + 0.6f;
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, surfaceY - ry * 0.5f), rx, ry), brush);
    } else {
        float mul = (kind == PrecipKind::Downpour) ? 1.45f : 1.0f;
        brush->SetColor(D2D1::ColorF(cfg.colorRain.r, cfg.colorRain.g, cfg.colorRain.b, std::clamp(alpha * 0.85f * fade, 0.02f, 0.85f)));
        float spread = (3.0f + t * 11.0f) * mul;
        float lift = (1.0f - (2.0f * t - 1.0f) * (2.0f * t - 1.0f)) * 7.5f * mul;
        rt->DrawLine(D2D1::Point2F(x - 1.0f, surfaceY), D2D1::Point2F(x - spread, surfaceY - lift), brush, 1.3f * mul);
        rt->DrawLine(D2D1::Point2F(x + 1.0f, surfaceY), D2D1::Point2F(x + spread, surfaceY - lift), brush, 1.3f * mul);
        if (tier != QualityTier::Low) {
            rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, surfaceY), spread * 1.1f, 1.8f + t * 1.5f), brush, 1.0f);
        }
    }
}

void DrawItemShape(CachedSurface& surf, const Settings& cfg, QualityTier tier, const FallingItem& p, float bottomRelY) {
    ID2D1SolidColorBrush* brush = surf.sharedBrush;
    if (!brush || !surf.rt) return;
    if (p.hitAnimTimer > 0.0f) {
        float progress = 1.0f - std::clamp(p.hitAnimTimer / 0.22f, 0.0f, 1.0f);
        DrawSurfaceSplash(surf.rt, brush, cfg, tier, p.kind, p.x, p.y, progress, p.alpha);
        return;
    }

    float alpha = (tier == QualityTier::Low) ? p.alpha * 0.85f : p.alpha;
    if (!p.isFlakeSub && p.kind != PrecipKind::Hail) {
        float thick = (p.kind == PrecipKind::Drizzle)
            ? 0.95f
            : ((p.kind == PrecipKind::Downpour) ? 2.25f : ((tier == QualityTier::Low) ? 1.4f : ((p.depthTier == 2) ? 2.1f : 1.55f)));
        float lenScale = (tier == QualityTier::Low) ? 0.82f : ((tier == QualityTier::High) ? 1.18f : 1.0f);
        float speedMag = std::hypot(p.vx, p.vy);
        float dirX = (speedMag > 0.01f) ? (p.vx / speedMag) : 0.0f;
        float dirY = (speedMag > 0.01f) ? (p.vy / speedMag) : 1.0f;
        float streakLen = p.size * lenScale;
        float x2 = p.x + dirX * streakLen;
        float y2 = p.y + dirY * streakLen;

        brush->SetColor(cfg.colorRain);
        brush->SetOpacity(std::clamp(alpha, 0.05f, 1.0f));
        surf.rt->DrawLine(D2D1::Point2F(p.x, p.y), D2D1::Point2F(x2, y2), brush, thick);
        if (tier == QualityTier::High && p.kind != PrecipKind::Drizzle) {
            brush->SetColor(D2D1::ColorF(0.88f, 0.96f, 1.0f, std::clamp(alpha * 0.55f, 0.04f, 0.70f)));
            surf.rt->DrawLine(D2D1::Point2F(p.x + dirX * streakLen * 0.55f, p.y + dirY * streakLen * 0.55f),
                              D2D1::Point2F(x2, y2), brush, thick * 0.65f);
        }
        brush->SetOpacity(1.0f);
        if (tier != QualityTier::Low && p.kind != PrecipKind::Drizzle && p.y >= bottomRelY - 14.0f && p.y <= bottomRelY + 4.0f) {
            float splashFade = 1.0f - std::clamp(std::abs(p.y - bottomRelY) / 14.0f, 0.0f, 1.0f);
            DrawSurfaceSplash(surf.rt, brush, cfg, tier, p.kind, p.x, bottomRelY, 1.0f - splashFade, p.alpha * 0.75f);
        }
        return;
    }
    if (p.isFlakeSub) {
        if (surf.flakeSprite) {
            float r = p.size * ((tier == QualityTier::High) ? 1.45f : 1.35f);
            surf.rt->DrawBitmap(surf.flakeSprite, D2D1::RectF(p.x - r, p.y - r, p.x + r, p.y + r),
                                std::clamp(alpha, 0.05f, 1.0f), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            if (tier == QualityTier::High && p.depthTier == 2 && p.size >= 4.2f && p.kind == PrecipKind::Snow) {
                brush->SetOpacity(std::clamp(alpha * 0.65f, 0.05f, 0.75f));
                brush->SetColor(cfg.colorSnow);
                float arm = r * 0.68f;
                for (int k = 0; k < 3; ++k) {
                    float a = p.swayPhase * 0.5f + k * 1.0471975f;
                    float ca = std::cos(a) * arm;
                    float sa = std::sin(a) * arm;
                    surf.rt->DrawLine(D2D1::Point2F(p.x - ca, p.y - sa), D2D1::Point2F(p.x + ca, p.y + sa), brush, 1.0f);
                }
                brush->SetOpacity(1.0f);
            }
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
    float half = p.size * 0.45f;
    surf.rt->FillRectangle(D2D1::RectF(p.x - half, p.y - half, p.x + half, p.y + half), brush);
    if (tier != QualityTier::Low) {
        brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, std::clamp(alpha * 0.48f, 0.03f, 0.65f)));
        surf.rt->DrawLine(D2D1::Point2F(p.x - half * 0.65f, p.y - half * 0.65f),
                          D2D1::Point2F(p.x + half * 0.35f, p.y - half * 0.65f), brush, 1.0f);
        if (tier == QualityTier::High) {
            brush->SetColor(D2D1::ColorF(0.45f, 0.68f, 0.92f, std::clamp(alpha * 0.45f, 0.03f, 0.60f)));
            surf.rt->DrawRectangle(D2D1::RectF(p.x - half, p.y - half, p.x + half, p.y + half), brush, 0.9f);
        }
    }
    brush->SetOpacity(1.0f);
    surf.rt->SetTransform(oldXform);
}

void DrawVerticalGradientTint(ID2D1DCRenderTarget* rt, D2D1_COLOR_F topCol, D2D1_COLOR_F botCol,
                              float leftX, float rightX, float topY, float botY, float factor) {
    topCol.a = std::clamp(topCol.a * factor, 0.0f, 0.85f);
    botCol.a = std::clamp(botCol.a * factor, 0.0f, 0.85f);
    if (topCol.a <= 0.003f && botCol.a <= 0.003f) return;

    D2D1_GRADIENT_STOP stops[2] = {{0.0f, topCol}, {1.0f, botCol}};
    ID2D1GradientStopCollection* stopCol = nullptr;
    if (FAILED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCol)) || !stopCol) {
        return;
    }
    ID2D1LinearGradientBrush* grad = nullptr;
    if (SUCCEEDED(rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(leftX, topY), D2D1::Point2F(leftX, botY)),
            stopCol, &grad)) && grad) {
        rt->FillRectangle(D2D1::RectF(leftX, topY, rightX, botY), grad);
        grad->Release();
    }
    stopCol->Release();
}

float ComputeCursorHoverFadeMultiplier(float cx, float cy, float hitRadius, int hoverOpacityPct,
                                     bool hasCursor, float curRelX, float curRelY) {
    if (!hasCursor) return 1.0f;
    float minMul = std::clamp(static_cast<float>(hoverOpacityPct) / 100.0f, 0.0f, 1.0f);
    float dx = curRelX - cx;
    float dy = curRelY - cy;
    float distSq = dx * dx + dy * dy;
    float rSq = hitRadius * hitRadius;
    if (distSq <= rSq) return minMul;
    float softOuter = hitRadius * 1.35f;
    float outerSq = softOuter * softOuter;
    if (distSq < outerSq) {
        float dist = std::sqrt(distSq);
        float t = (dist - hitRadius) / std::max(1.0f, softOuter - hitRadius);
        return minMul + (1.0f - minMul) * std::clamp(t, 0.0f, 1.0f);
    }
    return 1.0f;
}

void DrawProceduralCloudCluster(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                                QualityTier tier, D2D1_COLOR_F base,
                                float cx, float cy, float rx, float ry, float alpha,
                                int seed, double timeSec, int forcedLobes = -1) {
    if (tier == QualityTier::Low && forcedLobes < 0) {
        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, alpha * 0.85f));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry), brush);
        return;
    }

    if (tier == QualityTier::Medium && forcedLobes < 0) {
        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(base.r * 0.62f, base.g * 0.66f, base.b * 0.74f, alpha * 0.65f));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + rx * 0.06f, cy + ry * 0.34f), rx * 0.92f, ry * 0.65f), brush);
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
        return;
    }

    int lobes = (forcedLobes >= 1) ? forcedLobes : (6 + (seed % 4));
    brush->SetOpacity(1.0f);

    for (int l = 0; l < lobes; ++l) {
        uint32_t h = static_cast<uint32_t>((seed * 73856093u) ^ (l * 19349663u));
        float normPos = (lobes == 1) ? 0.0f : (-0.78f + 1.56f * (static_cast<float>(l) / static_cast<float>(lobes - 1)));
        float jitterX = ((static_cast<int>(h & 0xFFu) - 128) / 128.0f) * 0.18f;
        float jitterY = ((static_cast<int>((h >> 8) & 0xFFu) - 128) / 128.0f) * 0.26f;
        float morph = 0.86f + 0.22f * static_cast<float>(std::sin(timeSec * (0.45 + (l % 3) * 0.17) + l * 1.4 + seed));

        float lobeX = cx + (normPos + jitterX) * rx * 0.78f;
        float archLift = (1.0f - normPos * normPos * 0.85f) * ry * 0.38f;
        float lobeY = cy - archLift + jitterY * ry;
        float lobeRx = rx * (0.32f + ((h >> 16) & 0x1Fu) * 0.008f) * morph;
        float lobeRy = ry * (0.44f + ((h >> 21) & 0x1Fu) * 0.011f) * morph;
        float lobeAlpha = std::clamp(alpha * (0.72f + 0.28f * (1.0f - std::abs(normPos))), 0.002f, 0.82f);

        D2D1_GRADIENT_STOP stops[3] = {
            {0.00f, D2D1::ColorF(std::min(1.0f, base.r * 1.08f), std::min(1.0f, base.g * 1.08f), std::min(1.0f, base.b * 1.08f), lobeAlpha)},
            {0.62f, D2D1::ColorF(base.r, base.g, base.b, lobeAlpha * 0.55f)},
            {1.00f, D2D1::ColorF(base.r * 0.88f, base.g * 0.90f, base.b * 0.95f, 0.0f)}
        };
        ID2D1GradientStopCollection* stopsCol = nullptr;
        if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopsCol)) && stopsCol) {
            ID2D1RadialGradientBrush* rad = nullptr;
            if (SUCCEEDED(rt->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(D2D1::Point2F(lobeX, lobeY - lobeRy * 0.18f), D2D1::Point2F(0.0f, 0.0f), lobeRx, lobeRy),
                    stopsCol, &rad)) && rad) {
                rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(lobeX, lobeY), lobeRx, lobeRy), rad);
                rad->Release();
            }
            stopsCol->Release();
        }
        if (l % 2 == 0) {
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, lobeAlpha * 0.22f));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(lobeX - lobeRx * 0.14f, lobeY - lobeRy * 0.28f), lobeRx * 0.50f, lobeRy * 0.32f), brush);
        }
    }
}

void DrawHorizonDawnDuskGlow(ID2D1DCRenderTarget* rt, const MonitorCanvasRect& mon,
                              float cx, float horizonRatio01, D2D1_COLOR_F warmCol, float alphaScale) {
    float lowFactor = 1.0f - std::clamp(horizonRatio01 / 0.42f, 0.0f, 1.0f);
    if (lowFactor <= 0.02f || alphaScale <= 0.01f) return;

    float leftX = mon.left + mon.innerLeftRel;
    float rightX = mon.left + mon.innerRightRel;
    float botY = mon.top + mon.bottomLimitRelY;
    float glowH = std::max(60.0f, (botY - (mon.top + mon.innerTopRel)) * 0.32f);
    float topY = std::max(mon.top + mon.innerTopRel, botY - glowH);

    D2D1_COLOR_F cTop = warmCol;
    cTop.a = 0.0f;
    D2D1_COLOR_F cBot = warmCol;
    cBot.a = std::clamp(0.36f * lowFactor * alphaScale, 0.0f, 0.48f);
    DrawVerticalGradientTint(rt, cTop, cBot, leftX, rightX, topY, botY, 1.0f);

    float domeR = std::max(120.0f, (rightX - leftX) * 0.28f);
    D2D1_GRADIENT_STOP stops[2] = {
        {0.0f, D2D1::ColorF(warmCol.r, warmCol.g, warmCol.b, std::clamp(0.45f * lowFactor * alphaScale, 0.0f, 0.55f))},
        {1.0f, D2D1::ColorF(warmCol.r, warmCol.g, warmCol.b, 0.0f)}
    };
    ID2D1GradientStopCollection* stopsCol = nullptr;
    if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopsCol)) && stopsCol) {
        ID2D1RadialGradientBrush* rad = nullptr;
        if (SUCCEEDED(rt->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, botY), D2D1::Point2F(0.0f, 0.0f), domeR, domeR * 0.58f),
                stopsCol, &rad)) && rad) {
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, botY), domeR, domeR * 0.58f), rad);
            rad->Release();
        }
        stopsCol->Release();
    }
}

void DrawSunHighConeRays(ID2D1DCRenderTarget* rt, float cx, float cy, float radius,
                         D2D1_COLOR_F base, float visFactor, double timeSec) {
    if (!g_d2dFactory) return;
    constexpr int kConeCount = 14;
    for (int i = 0; i < kConeCount; ++i) {
        double fi = static_cast<double>(i);
        double pulse = 0.5 + 0.5 * std::sin(timeSec * (1.15 + (i % 4) * 0.28) + fi * 1.37);
        double widthWave = 0.5 + 0.5 * std::cos(timeSec * (0.85 + (i % 3) * 0.22) + fi * 2.11);
        double centerAng = (2.0 * kPi * fi) / kConeCount + timeSec * 0.055 + std::sin(timeSec * 0.4 + fi) * 0.03;

        float innerR = radius * 0.85f;
        float outerR = radius * static_cast<float>(2.15 + 1.45 * pulse);
        double halfSpread = (0.045 + 0.055 * widthWave);

        D2D1_POINT_2F pApex = D2D1::Point2F(
            cx + static_cast<float>(std::cos(centerAng)) * innerR,
            cy + static_cast<float>(std::sin(centerAng)) * innerR);
        D2D1_POINT_2F pLeft = D2D1::Point2F(
            cx + static_cast<float>(std::cos(centerAng - halfSpread)) * outerR,
            cy + static_cast<float>(std::sin(centerAng - halfSpread)) * outerR);
        D2D1_POINT_2F pRight = D2D1::Point2F(
            cx + static_cast<float>(std::cos(centerAng + halfSpread)) * outerR,
            cy + static_cast<float>(std::sin(centerAng + halfSpread)) * outerR);
        D2D1_POINT_2F pTipMid = D2D1::Point2F(
            cx + static_cast<float>(std::cos(centerAng)) * outerR,
            cy + static_cast<float>(std::sin(centerAng)) * outerR);

        ID2D1PathGeometry* coneGeo = nullptr;
        if (FAILED(g_d2dFactory->CreatePathGeometry(&coneGeo)) || !coneGeo) continue;
        ID2D1GeometrySink* sink = nullptr;
        if (SUCCEEDED(coneGeo->Open(&sink)) && sink) {
            sink->BeginFigure(pApex, D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(pLeft);
            sink->AddLine(pRight);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();
            sink->Release();

            float rayAlpha = std::clamp(static_cast<float>(0.16 + 0.22 * pulse) * visFactor, 0.002f, 0.42f);
            D2D1_GRADIENT_STOP stops[3] = {
                {0.00f, D2D1::ColorF(1.0f, std::min(1.0f, base.g + 0.12f), std::min(1.0f, base.b + 0.15f), rayAlpha)},
                {0.55f, D2D1::ColorF(base.r, base.g, base.b, rayAlpha * 0.42f)},
                {1.00f, D2D1::ColorF(base.r, base.g, base.b, 0.0f)}
            };
            ID2D1GradientStopCollection* stopCol = nullptr;
            if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCol)) && stopCol) {
                ID2D1LinearGradientBrush* linBrush = nullptr;
                if (SUCCEEDED(rt->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(pApex, pTipMid), stopCol, &linBrush)) && linBrush) {
                    rt->FillGeometry(coneGeo, linBrush);
                    linBrush->Release();
                }
                stopCol->Release();
            }
        }
        coneGeo->Release();
    }
}

void DrawSunBodyHighQuality(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                            float cx, float cy, float radius, D2D1_COLOR_F sunCol, float visFactor, double timeSec) {
    float glowR = radius * 3.4f;
    D2D1_GRADIENT_STOP stops[3] = {
        {0.00f, D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, std::clamp(sunCol.a * 0.62f * visFactor, 0.0f, 0.78f))},
        {0.45f, D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, std::clamp(sunCol.a * 0.22f * visFactor, 0.0f, 0.40f))},
        {1.00f, D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, 0.0f)}
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
    DrawSunHighConeRays(rt, cx, cy, radius, sunCol, visFactor, timeSec);
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, std::clamp(sunCol.a * visFactor, 0.002f, 1.0f)));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius), brush);
    brush->SetColor(D2D1::ColorF(1.0f, 0.99f, 0.90f, std::clamp(0.55f * visFactor, 0.002f, 0.72f)));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), radius * 0.70f, radius * 0.70f), brush);
}

void DrawMoonBodyHighQuality(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             float cx, float cy, float r, D2D1_COLOR_F base, double moonPhase01, float visFactor) {
    double phase = std::clamp(moonPhase01, 0.0, 1.0);
    double illum = 0.5 * (1.0 - std::cos(phase * 2.0 * kPi));
    float sideSign = (phase <= 0.5) ? 1.0f : -1.0f;

    float glowR = r * 2.8f;
    D2D1_GRADIENT_STOP stops[2] = {
        {0.0f, D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.34f * visFactor, 0.0f, 0.52f))},
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

    if (illum > 0.18 && r >= 8.0f) {
        brush->SetColor(D2D1::ColorF(base.r * 0.72f, base.g * 0.75f, base.b * 0.84f, std::clamp(0.30f * visFactor, 0.002f, 0.45f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - r * 0.22f, cy - r * 0.18f), r * 0.24f, r * 0.18f), brush);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.14f, cy + r * 0.22f), r * 0.20f, r * 0.14f), brush);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.26f, cy - r * 0.12f), r * 0.13f, r * 0.11f), brush);
        brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, std::clamp(0.24f * visFactor, 0.002f, 0.36f)));
        rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r * 0.96f, r * 0.96f), brush, 1.1f);
    }
}

void DrawTopStaticCloudsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                              const Settings& cfg, QualityTier tier, float cloudPct01, double timeSec,
                              bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    if (!cfg.topCloudCoverEnabled || cloudPct01 <= 0.01f) return;
    D2D1_COLOR_F base = cfg.hasCustomCloudsColor
        ? cfg.colorClouds
        : D2D1::ColorF(0.92f - cloudPct01 * 0.24f, 0.94f - cloudPct01 * 0.24f, 0.98f - cloudPct01 * 0.18f, 0.42f);

    for (const auto& mon : g_monitorRects) {
        float innerW = std::max(40.0f, mon.innerRightRel - mon.innerLeftRel);
        int slots = (tier == QualityTier::Low) ? 8 : ((tier == QualityTier::High) ? 16 : 12);
        int activeClusters = std::clamp(static_cast<int>(std::ceil(slots * cloudPct01)), 1, slots);
        float stepX = innerW / static_cast<float>(slots);
        for (int i = 0; i < slots; ++i) {
            uint32_t h = static_cast<uint32_t>(i * 2654435761u + 97531u);
            int slotOrder = static_cast<int>(h % static_cast<uint32_t>(slots));
            if (slotOrder >= activeClusters) continue;
            float phase = static_cast<float>((h >> 8) & 0xFFu) * 0.025f;
            float breathe = 0.5f + 0.5f * static_cast<float>(std::sin(timeSec * (0.45 + (i % 4) * 0.11) + phase));
            if (breathe <= 0.04f) continue;

            float cx = mon.left + mon.innerLeftRel + (static_cast<float>(i) + 0.5f) * stepX;
            float cy = mon.top + mon.innerTopRel + (8.0f + ((i % 3) * 8.0f)) * mon.dpiScale;
            float rx = stepX * (0.85f + 0.35f * breathe);
            float ry = (24.0f + cloudPct01 * 28.0f) * (0.80f + 0.25f * breathe) * mon.dpiScale;
            float hoverFade = ComputeCursorHoverFadeMultiplier(
                cx, cy, std::max(rx, ry), cfg.cursorHoverOpacityPct, hasCursor, curRelX, curRelY);
            if (hoverFade < 0.999f) outCursorNearSky = true;
            float alpha = std::clamp(base.a * (0.35f + cloudPct01 * 0.65f) * breathe * hoverFade, 0.002f, 0.72f);
            if (alpha <= 0.003f) continue;

            DrawProceduralCloudCluster(rt, brush, tier, base, cx, cy, rx, ry, alpha, i + 101, timeSec);
        }
    }
}

void DrawDriftingCloudsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             const Settings& cfg, QualityTier tier, float cloudPct01, float windVx, double timeSec,
                             bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    if (cloudPct01 <= 0.01f) return;
    D2D1_COLOR_F base = cfg.hasCustomCloudsColor
        ? cfg.colorClouds
        : D2D1::ColorF(0.93f - cloudPct01 * 0.25f, 0.95f - cloudPct01 * 0.25f, 0.98f - cloudPct01 * 0.20f, 0.36f);
    float driftSpeed = (std::abs(windVx) > 1.5f) ? windVx * 0.32f : 8.0f;
    float qScale = GetTierScale(tier);

    for (const auto& mon : g_monitorRects) {
        float innerW = std::max(40.0f, mon.innerRightRel - mon.innerLeftRel);
        float innerH = std::max(40.0f, mon.bottomLimitRelY - mon.innerTopRel);
        int puffCount = std::clamp(static_cast<int>((2.0f + cloudPct01 * 12.0f) * qScale), 1, 24);
        float bandH = std::max(60.0f, innerH * (0.10f + cloudPct01 * 0.16f));
        for (int i = 0; i < puffCount; ++i) {
            float fi = static_cast<float>(i);
            float spanW = innerW + 220.0f * mon.dpiScale;
            float rawOffset = std::fmod(fi * (innerW / static_cast<float>(puffCount)) +
                                        static_cast<float>(timeSec) * driftSpeed * (0.6f + 0.2f * (i % 3)), spanW);
            if (rawOffset < 0.0f) rawOffset += spanW;
            float cx = mon.left + mon.innerLeftRel + rawOffset - 110.0f * mon.dpiScale;
            float cy = mon.top + mon.innerTopRel + 22.0f * mon.dpiScale + ((i * 37) % 100) * 0.01f * bandH;
            float rx = (68.0f + (i % 4) * 30.0f) * (0.65f + cloudPct01 * 0.55f) * mon.dpiScale;
            float ry = (22.0f + (i % 3) * 10.0f) * (0.65f + cloudPct01 * 0.50f) * mon.dpiScale;
            float edgeFade = ComputeMonitorEdgeFade(mon, cx, cy, rx + 45.0f * mon.dpiScale);
            float hoverFade = ComputeCursorHoverFadeMultiplier(
                cx, cy, std::max(rx, ry), cfg.cursorHoverOpacityPct, hasCursor, curRelX, curRelY);
            if (hoverFade < 0.999f) outCursorNearSky = true;
            float totalFade = edgeFade * hoverFade;
            if (totalFade <= 0.002f) continue;

            float alpha = std::clamp(
                base.a * (0.25f + cloudPct01 * 0.85f) *
                (0.88f + 0.12f * static_cast<float>(std::sin(timeSec * 0.7 + fi))) * totalFade,
                0.002f, 0.68f);

            DrawProceduralCloudCluster(rt, brush, tier, base, cx, cy, rx, ry, alpha, i + 17, timeSec);
        }
    }
}

void DrawSunLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg,
                  QualityTier tier, bool active, double hour, const ForecastSnapshot& snap,
                  float uvIndex, float cloudDimFactor, double timeSec,
                  bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    if (!active || cfg.sunGeom.radius <= 0 || g_monitorRects.empty()) return;
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
    float glowMult = (tier == QualityTier::High) ? 3.8f : 3.2f;

    size_t monCount = (cfg.celestialMonitorMode == CelestialMonitorMode::All) ? g_monitorRects.size() : 1;
    for (size_t mIdx = 0; mIdx < monCount; ++mIdx) {
        const auto& mon = g_monitorRects[mIdx];
        float radius = 0.0f;
        float cx = 0.0f;
        float cy = 0.0f;
        float horizonRatio01 = 1.0f;
        bool inWin = false;
        ComputeCelestialCoordsInMonitor(mon, true, cfg.sunMotionMode, cfg.sunGeom, hour, cfg, snap, radius, cx, cy, horizonRatio01, inWin);
        if (radius < 3.0f) continue;

        float botY = mon.top + mon.bottomLimitRelY;
        float subHorizonFactor = std::clamp(((botY + radius * 1.15f) - cy) / std::max(4.0f, radius * 1.4f), 0.0f, 1.0f);
        if (subHorizonFactor <= 0.002f) continue;

        float hoverFade = ComputeCursorHoverFadeMultiplier(
            cx, cy, radius * 2.1f, cfg.cursorHoverOpacityPct, hasCursor, curRelX, curRelY);
        if (hoverFade < 0.999f) outCursorNearSky = true;
        float visFactor = baseVis * subHorizonFactor * hoverFade;
        if (visFactor <= 0.002f) continue;

        float lowWarmth = 1.0f - horizonRatio01;
        D2D1_COLOR_F sunCol = base;
        sunCol.r = std::min(1.0f, sunCol.r + lowWarmth * 0.08f);
        sunCol.g = std::clamp(sunCol.g * (1.0f - lowWarmth * 0.36f), 0.18f, 1.0f);
        sunCol.b = std::clamp(sunCol.b * (1.0f - lowWarmth * 0.62f), 0.05f, 1.0f);

        rt->PushAxisAlignedClip(
            D2D1::RectF(mon.left + mon.innerLeftRel, mon.top + mon.innerTopRel,
                        mon.left + mon.innerRightRel, botY),
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        DrawHorizonDawnDuskGlow(rt, mon, cx, horizonRatio01, D2D1::ColorF(1.0f, 0.52f, 0.18f, 1.0f), visFactor);

        float glowR = radius * glowMult;
        D2D1_GRADIENT_STOP stops[3] = {
            {0.00f, D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, std::clamp(sunCol.a * 0.62f * visFactor, 0.0f, 0.78f))},
            {0.45f, D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, std::clamp(sunCol.a * 0.22f * visFactor, 0.0f, 0.40f))},
            {1.00f, D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, 0.0f)}
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

        if (tier == QualityTier::High) {
            DrawSunHighConeRays(rt, cx, cy, radius, sunCol, visFactor, timeSec);
        } else if (tier == QualityTier::Medium) {
            brush->SetOpacity(1.0f);
            brush->SetColor(D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, std::clamp(0.32f * visFactor, 0.002f, 0.55f)));
            for (int rIdx = 0; rIdx < 16; ++rIdx) {
                double ang = (2.0 * kPi * rIdx) / 16.0 + timeSec * 0.08;
                float innerR = radius * 1.18f;
                float outerR = radius * ((rIdx % 2 == 0) ? 1.78f : 1.50f);
                rt->DrawLine(
                    D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * innerR, cy + static_cast<float>(std::sin(ang)) * innerR),
                    D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * outerR, cy + static_cast<float>(std::sin(ang)) * outerR),
                    brush, std::max(1.5f, radius * 0.045f));
            }
        }

        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(sunCol.r, sunCol.g, sunCol.b, std::clamp(sunCol.a * visFactor, 0.002f, 1.0f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius), brush);
        if (tier == QualityTier::High) {
            brush->SetColor(D2D1::ColorF(1.0f, 0.99f, 0.90f, std::clamp(0.55f * visFactor, 0.002f, 0.72f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), radius * 0.70f, radius * 0.70f), brush);
        }

        rt->PopAxisAlignedClip();
    }
}

void DrawMoonLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush, const Settings& cfg,
                   QualityTier tier, bool active, double hour, const ForecastSnapshot& snap,
                   double moonPhase01, float cloudDimFactor,
                   bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    if (!active || cfg.moonGeom.radius <= 0 || g_monitorRects.empty()) return;
    D2D1_COLOR_F base = cfg.hasCustomMoonColor ? cfg.colorMoon : D2D1::ColorF(0.88f, 0.92f, 1.0f, 0.95f);
    float baseVis = std::clamp(cloudDimFactor, 0.18f, 1.0f);
    double phase = std::clamp(moonPhase01, 0.0, 1.0);
    double illum = 0.5 * (1.0 - std::cos(phase * 2.0 * kPi));
    float sideSign = (phase <= 0.5) ? 1.0f : -1.0f;

    size_t monCount = (cfg.celestialMonitorMode == CelestialMonitorMode::All) ? g_monitorRects.size() : 1;
    for (size_t mIdx = 0; mIdx < monCount; ++mIdx) {
        const auto& mon = g_monitorRects[mIdx];
        float r = 0.0f;
        float cx = 0.0f;
        float cy = 0.0f;
        float horizonRatio01 = 1.0f;
        bool inWin = false;
        ComputeCelestialCoordsInMonitor(mon, false, cfg.moonMotionMode, cfg.moonGeom, hour, cfg, snap, r, cx, cy, horizonRatio01, inWin);
        if (r < 3.0f) continue;

        float botY = mon.top + mon.bottomLimitRelY;
        float subHorizonFactor = std::clamp(((botY + r * 1.15f) - cy) / std::max(4.0f, r * 1.4f), 0.0f, 1.0f);
        if (subHorizonFactor <= 0.002f) continue;

        float hoverFade = ComputeCursorHoverFadeMultiplier(
            cx, cy, r * 1.6f, cfg.cursorHoverOpacityPct, hasCursor, curRelX, curRelY);
        if (hoverFade < 0.999f) outCursorNearSky = true;
        float visFactor = baseVis * subHorizonFactor * hoverFade;
        if (visFactor <= 0.002f) continue;

        rt->PushAxisAlignedClip(
            D2D1::RectF(mon.left + mon.innerLeftRel, mon.top + mon.innerTopRel,
                        mon.left + mon.innerRightRel, botY),
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        DrawHorizonDawnDuskGlow(rt, mon, cx, horizonRatio01, D2D1::ColorF(0.42f, 0.56f, 0.88f, 1.0f), visFactor * 0.65f);

        float glowR = r * ((tier == QualityTier::High) ? 2.9f : 2.4f);
        D2D1_GRADIENT_STOP stops[2] = {
            {0.0f, D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.34f * visFactor, 0.0f, 0.52f))},
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

        if (tier != QualityTier::Low && illum > 0.18 && r >= 10.0f) {
            brush->SetColor(D2D1::ColorF(base.r * 0.72f, base.g * 0.75f, base.b * 0.84f, std::clamp(0.28f * visFactor, 0.002f, 0.42f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - r * 0.22f, cy - r * 0.18f), r * 0.24f, r * 0.18f), brush);
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.14f, cy + r * 0.22f), r * 0.20f, r * 0.14f), brush);
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.26f, cy - r * 0.12f), r * 0.13f, r * 0.11f), brush);
            if (tier == QualityTier::High) {
                brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, std::clamp(0.24f * visFactor, 0.002f, 0.36f)));
                rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r * 0.96f, r * 0.96f), brush, 1.1f);
            }
        }

        rt->PopAxisAlignedClip();
    }
}

void DrawSkyAmbientTintLayer(ID2D1DCRenderTarget* rt, const Settings& cfg,
                             const ForecastSnapshot& snap, double hour) {
    if (!cfg.skyAmbientTintEnabled || cfg.skyAmbientTintOpacity <= 0) return;

    double sr = cfg.customSunriseHour;
    double ss = cfg.customSunsetHour;
    bool dummy = false;
    ResolveCelestialWindow(cfg, snap, true, hour, sr, ss, dummy);

    auto lerpCol = [](D2D1_COLOR_F a, D2D1_COLOR_F b, float t) -> D2D1_COLOR_F {
        float u = std::clamp(t, 0.0f, 1.0f);
        return D2D1::ColorF(
            a.r + (b.r - a.r) * u,
            a.g + (b.g - a.g) * u,
            a.b + (b.b - a.b) * u,
            a.a + (b.a - a.a) * u);
    };

    const D2D1_COLOR_F kNightTop  = D2D1::ColorF(0.05f, 0.08f, 0.22f, 1.00f);
    const D2D1_COLOR_F kNightBot  = D2D1::ColorF(0.07f, 0.11f, 0.28f, 0.85f);
    const D2D1_COLOR_F kBlueTop   = D2D1::ColorF(0.10f, 0.24f, 0.56f, 0.95f);
    const D2D1_COLOR_F kBlueBot   = D2D1::ColorF(0.18f, 0.36f, 0.68f, 0.85f);
    const D2D1_COLOR_F kSunTop    = D2D1::ColorF(0.24f, 0.32f, 0.58f, 0.72f);
    const D2D1_COLOR_F kSunBot    = D2D1::ColorF(0.98f, 0.54f, 0.28f, 0.95f);
    const D2D1_COLOR_F kGoldTop   = D2D1::ColorF(0.96f, 0.72f, 0.32f, 0.50f);
    const D2D1_COLOR_F kGoldBot   = D2D1::ColorF(1.00f, 0.78f, 0.38f, 0.82f);
    const D2D1_COLOR_F kClearDay  = D2D1::ColorF(0.00f, 0.00f, 0.00f, 0.00f);

    D2D1_COLOR_F topCol = kClearDay;
    D2D1_COLOR_F botCol = kClearDay;

    if (hour >= sr - 1.0 && hour < sr - 0.45) {
        float t = static_cast<float>((hour - (sr - 1.0)) / 0.55);
        topCol = lerpCol(kNightTop, kBlueTop, t);
        botCol = lerpCol(kNightBot, kBlueBot, t);
    } else if (hour >= sr - 0.45 && hour < sr) {
        float t = static_cast<float>((hour - (sr - 0.45)) / 0.45);
        topCol = lerpCol(kBlueTop, kSunTop, t);
        botCol = lerpCol(kBlueBot, kSunBot, t);
    } else if (hour >= sr && hour < sr + 0.75) {
        float t = static_cast<float>((hour - sr) / 0.75);
        topCol = lerpCol(kSunTop, kGoldTop, t);
        botCol = lerpCol(kSunBot, kGoldBot, t);
    } else if (hour >= sr + 0.75 && hour < sr + 1.35) {
        float t = static_cast<float>((hour - (sr + 0.75)) / 0.60);
        topCol = lerpCol(kGoldTop, kClearDay, t);
        botCol = lerpCol(kGoldBot, kClearDay, t);
    } else if (hour >= ss - 1.35 && hour < ss - 0.75) {
        float t = static_cast<float>((hour - (ss - 1.35)) / 0.60);
        topCol = lerpCol(kClearDay, kGoldTop, t);
        botCol = lerpCol(kClearDay, kGoldBot, t);
    } else if (hour >= ss - 0.75 && hour < ss) {
        float t = static_cast<float>((hour - (ss - 0.75)) / 0.75);
        topCol = lerpCol(kGoldTop, kSunTop, t);
        botCol = lerpCol(kGoldBot, kSunBot, t);
    } else if (hour >= ss && hour < ss + 0.45) {
        float t = static_cast<float>((hour - ss) / 0.45);
        topCol = lerpCol(kSunTop, kBlueTop, t);
        botCol = lerpCol(kSunBot, kBlueBot, t);
    } else if (hour >= ss + 0.45 && hour < ss + 1.0) {
        float t = static_cast<float>((hour - (ss + 0.45)) / 0.55);
        topCol = lerpCol(kBlueTop, kNightTop, t);
        botCol = lerpCol(kBlueBot, kNightBot, t);
    } else if (hour >= sr + 1.35 && hour < ss - 1.35) {
        return;
    } else {
        topCol = kNightTop;
        botCol = kNightBot;
    }

    float maxAlpha = std::clamp(static_cast<float>(cfg.skyAmbientTintOpacity) / 100.0f, 0.0f, 0.50f);
    for (const auto& mon : g_monitorRects) {
        DrawVerticalGradientTint(rt, topCol, botCol,
                                 mon.left + mon.innerLeftRel, mon.left + mon.innerRightRel,
                                 mon.top + mon.innerTopRel, mon.top + mon.bottomLimitRelY, maxAlpha);
    }
}

void DrawDustLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                   const Settings& cfg, QualityTier tier, float strength, float windVx, double timeSec) {
    if (strength <= 0.004f) return;
    D2D1_COLOR_F base = cfg.hasCustomDustColor ? cfg.colorDust : D2D1::ColorF(0.84f, 0.68f, 0.45f, 0.28f);
    float driftVx = (std::abs(windVx) > 20.0f) ? (windVx * 0.9f) : 75.0f;
    int motes = std::clamp(static_cast<int>(36 * GetTierScale(tier)), 16, 80);

    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float rightX = mon.left + mon.innerRightRel;
        float topY = mon.top + mon.innerTopRel;
        float botY = mon.top + mon.bottomLimitRelY;
        float innerW = std::max(40.0f, rightX - leftX);
        float innerH = std::max(40.0f, botY - topY);

        float bandTop = std::max(topY, botY - innerH * 0.14f);
        D2D1_COLOR_F cTop = base;
        cTop.a = 0.0f;
        D2D1_COLOR_F cBot = base;
        cBot.a = std::clamp(base.a * 0.85f, 0.05f, 0.30f);
        DrawVerticalGradientTint(rt, cTop, cBot, leftX, rightX, bandTop, botY, strength);

        brush->SetOpacity(1.0f);
        for (int i = 0; i < motes; ++i) {
            uint32_t hx = static_cast<uint32_t>(i * 2654435761u + 131u);
            uint32_t hy = static_cast<uint32_t>(i * 19349663u + 577u);
            float u01 = static_cast<float>(hx & 0xFFFFu) / 65535.0f;
            float v01 = static_cast<float>(hy & 0xFFFFu) / 65535.0f;
            float spdMul = 0.65f + (i % 4) * 0.22f;
            float rawX = std::fmod(u01 * innerW + static_cast<float>(timeSec) * driftVx * spdMul, innerW);
            if (rawX < 0.0f) rawX += innerW;
            float rawY = std::fmod(v01 * innerH + static_cast<float>(timeSec) * 18.0f * spdMul +
                                   static_cast<float>(std::sin(timeSec * 1.8 + i)) * 8.0f, innerH);
            if (rawY < 0.0f) rawY += innerH;
            float px = leftX + rawX;
            float py = topY + rawY;
            float fade = ComputeMonitorEdgeFade(mon, px, py, 28.0f);
            float r = ((i % 3 == 0) ? 2.1f : 1.3f) * mon.dpiScale;
            brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.55f * strength * fade, 0.03f, 0.72f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), r * 1.4f, r * 0.85f), brush);
        }
    }
}

void DrawFogLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                  const Settings& cfg, QualityTier tier, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    D2D1_COLOR_F base = cfg.hasCustomFogColor ? cfg.colorFog : D2D1::ColorF(0.84f, 0.88f, 0.92f, 0.34f);
    int wisps = (tier == QualityTier::Low) ? 4 : ((tier == QualityTier::High) ? 10 : 7);

    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float rightX = mon.left + mon.innerRightRel;
        float topY = mon.top + mon.innerTopRel;
        float botY = mon.top + mon.bottomLimitRelY;
        float innerW = std::max(40.0f, rightX - leftX);
        float innerH = std::max(40.0f, botY - topY);

        float bandTop = std::max(topY, botY - innerH * 0.22f);
        D2D1_COLOR_F cTop = base;
        cTop.a = 0.0f;
        D2D1_COLOR_F cBot = base;
        cBot.a = std::clamp(base.a * 0.70f, 0.05f, 0.30f);
        DrawVerticalGradientTint(rt, cTop, cBot, leftX, rightX, bandTop, botY, strength);

        float spanW = innerW + 260.0f * mon.dpiScale;
        for (int i = 0; i < wisps; ++i) {
            float speed = 9.0f + (i % 3) * 4.5f;
            float rawX = std::fmod(i * (spanW / static_cast<float>(wisps)) + static_cast<float>(timeSec) * speed, spanW);
            float cx = leftX - 130.0f * mon.dpiScale + rawX;
            float cy = botY - (innerH * 0.04f) - ((i * 29) % 100) * 0.0014f * innerH;
            float rx = (110.0f + (i % 3) * 45.0f) * mon.dpiScale;
            float ry = (24.0f + (i % 2) * 14.0f) * mon.dpiScale;
            float edgeFade = ComputeMonitorEdgeFade(mon, cx, cy, rx * 0.85f);
            float alpha = std::clamp(base.a * 0.55f * strength * edgeFade, 0.003f, 0.32f);
            if (alpha <= 0.004f) continue;
            DrawProceduralCloudCluster(rt, brush, tier, base, cx, cy, rx, ry, alpha, i + 41, timeSec, 3);
        }
    }
}

void DrawSmogLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                   const Settings& cfg, QualityTier tier, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    D2D1_COLOR_F base = cfg.hasCustomSmogColor ? cfg.colorSmog : D2D1::ColorF(0.54f, 0.48f, 0.40f, 0.34f);
    int ashCount = std::clamp(static_cast<int>(22 * GetTierScale(tier)), 10, 48);

    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float rightX = mon.left + mon.innerRightRel;
        float topY = mon.top + mon.innerTopRel;
        float botY = mon.top + mon.bottomLimitRelY;
        float innerW = std::max(40.0f, rightX - leftX);
        float innerH = std::max(40.0f, botY - topY);

        float bandTop = std::max(topY, botY - innerH * 0.18f);
        D2D1_COLOR_F cTop = base;
        cTop.a = 0.0f;
        D2D1_COLOR_F cBot = base;
        cBot.a = std::clamp(base.a * 0.90f, 0.06f, 0.36f);
        DrawVerticalGradientTint(rt, cTop, cBot, leftX, rightX, bandTop, botY, strength);

        brush->SetOpacity(1.0f);
        for (int i = 0; i < ashCount; ++i) {
            uint32_t hx = static_cast<uint32_t>(i * 2246822519u + 73u);
            uint32_t hy = static_cast<uint32_t>(i * 374761393u + 199u);
            float u01 = static_cast<float>(hx & 0xFFFFu) / 65535.0f;
            float v01 = static_cast<float>(hy & 0xFFFFu) / 65535.0f;
            float px = leftX + std::fmod(u01 * innerW + static_cast<float>(timeSec) * 6.5f +
                                         static_cast<float>(std::sin(timeSec * 0.8 + i)) * 12.0f, innerW);
            float py = (topY + innerH * 0.50f) + std::fmod(v01 * (innerH * 0.50f) +
                                                           static_cast<float>(std::cos(timeSec * 0.7 + i * 1.4)) * 10.0f, innerH * 0.50f);
            float fade = ComputeMonitorEdgeFade(mon, px, py, 24.0f);
            float r = ((i % 4 == 0) ? 2.4f : 1.5f) * mon.dpiScale;
            brush->SetColor(D2D1::ColorF(base.r * 0.75f, base.g * 0.72f, base.b * 0.68f, std::clamp(0.45f * strength * fade, 0.02f, 0.55f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), r, r), brush);
        }
    }
}

void DrawSmoothAurora(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                    const Settings& cfg, QualityTier tier, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    D2D1_COLOR_F base = cfg.hasCustomAuroraColor ? cfg.colorAurora : D2D1::ColorF(0.20f, 0.94f, 0.62f, 0.28f);
    int ribbons = (tier == QualityTier::Low) ? 1 : ((tier == QualityTier::High) ? 4 : 3);
    float stepX = (tier == QualityTier::High) ? 10.0f : ((tier == QualityTier::Low) ? 40.0f : 12.0f);

    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float rightX = mon.left + mon.innerRightRel;
        float topBase = mon.top + mon.innerTopRel + 12.0f;
        float bandHeight = std::max(70.0f, (mon.bottomLimitRelY - mon.innerTopRel) * 0.22f);
        for (int ribbon = 0; ribbon < ribbons; ++ribbon) {
            D2D1_COLOR_F cTop = base;
            cTop.a = std::clamp(base.a * (ribbon == 0 ? 1.0f : 0.60f) * strength, 0.0f, 0.65f);
            D2D1_COLOR_F cBot = cTop;
            cBot.a = 0.0f;
            float y0 = topBase + ribbon * 22.0f;
            DrawVerticalGradientTint(rt, cTop, cBot, leftX, rightX, y0, y0 + bandHeight, 1.0f);
            brush->SetColor(D2D1::ColorF(cTop.r, cTop.g, cTop.b, std::clamp(cTop.a * 1.15f, 0.0f, 0.75f)));
            float prevX = leftX;
            float prevY = y0 + static_cast<float>(std::sin(timeSec * 1.1 + ribbon)) * 14.0f;
            for (float x = leftX + stepX; x <= rightX; x += stepX) {
                float curY = y0 + static_cast<float>(std::sin(x * 0.0045f + timeSec * (1.1 + ribbon * 0.3))) * 18.0f;
                rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 3.0f);
                prevX = x;
                prevY = curY;
            }
        }
    }
}

void DrawSmoothMirage(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                    const Settings& cfg, QualityTier tier, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    D2D1_COLOR_F base = cfg.hasCustomMirageColor ? cfg.colorMirage : D2D1::ColorF(1.0f, 0.88f, 0.68f, 0.25f);
    int lines = (tier == QualityTier::High) ? 5 : ((tier == QualityTier::Low) ? 2 : 4);
    float stepX = (tier == QualityTier::Low) ? 20.0f : 12.0f;

    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float rightX = mon.left + mon.innerRightRel;
        float botY = mon.top + mon.bottomLimitRelY;
        float bandH = std::max(45.0f, (mon.bottomLimitRelY - mon.innerTopRel) * 0.14f);
        float topY = std::max(mon.top + mon.innerTopRel, botY - bandH);
        D2D1_COLOR_F topCol = base;
        topCol.a = 0.0f;
        D2D1_COLOR_F botCol = base;
        botCol.a = std::clamp(base.a * strength, 0.0f, 0.55f);
        DrawVerticalGradientTint(rt, topCol, botCol, leftX, rightX, topY, botY, 1.0f);

        brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * 0.75f * strength, 0.0f, 0.5f)));
        for (int line = 0; line < lines; ++line) {
            float baseLineY = topY + bandH * (0.25f + line * (0.65f / std::max(1, lines - 1)));
            float prevX = leftX;
            float prevY = baseLineY + static_cast<float>(std::sin(timeSec * 2.4 + line * 1.7)) * 3.0f;
            for (float x = leftX + stepX; x <= rightX; x += stepX) {
                float curY = baseLineY + static_cast<float>(std::sin(x * 0.012f + timeSec * 2.4 + line * 1.7)) * 3.5f;
                rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 1.5f);
                prevX = x;
                prevY = curY;
            }
        }
    }
}

void DrawBranchedLightning(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                         QualityTier tier, const LightningFlash& flash, float topY, float maxHeight, float alpha) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(brush->GetColor().r, brush->GetColor().g, brush->GetColor().b, alpha));
    float curX = flash.xStart;
    float curY = topY;
    int segs = (tier == QualityTier::Low) ? 10 : ((tier == QualityTier::High) ? 18 : 14);
    float stepY = std::max(20.0f, (maxHeight - topY) * 0.72f / segs);
    for (int i = 0; i < segs; ++i) {
        float t = static_cast<float>(i + 1) / segs;
        float nextX = flash.xStart + (flash.xEnd - flash.xStart) * t + ((i % 2 == 0) ? 16.0f : -16.0f);
        float nextY = curY + stepY;
        rt->DrawLine(D2D1::Point2F(curX, curY), D2D1::Point2F(nextX, nextY), brush, 2.8f);
        if (tier != QualityTier::Low &&
            (i == 4 || i == 8 || i == 11 || (tier == QualityTier::High && i == 14))) {
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

void DrawThermalLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                      QualityTier tier, bool frostOn, bool heatOn, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float rightX = mon.left + mon.innerRightRel;
        float topY = mon.top + mon.innerTopRel;
        float botY = mon.top + mon.bottomLimitRelY;
        float innerW = std::max(40.0f, rightX - leftX);
        float innerH = std::max(40.0f, botY - topY);

        if (frostOn) {
            float rimV = std::min(72.0f * mon.dpiScale, innerH * 0.09f);
            float rimH = std::min(72.0f * mon.dpiScale, innerW * 0.07f);
            D2D1_COLOR_F fEdge = D2D1::ColorF(0.76f, 0.90f, 1.0f, 0.28f);
            D2D1_COLOR_F fZero = D2D1::ColorF(0.76f, 0.90f, 1.0f, 0.00f);

            DrawVerticalGradientTint(rt, fEdge, fZero, leftX, rightX, topY, topY + rimV, strength);
            DrawVerticalGradientTint(rt, fZero, fEdge, leftX, rightX, botY - rimV, botY, strength);

            auto drawHorizRim = [&](float x0, float x1, D2D1_COLOR_F c0, D2D1_COLOR_F c1) {
                c0.a = std::clamp(c0.a * strength, 0.0f, 0.65f);
                c1.a = std::clamp(c1.a * strength, 0.0f, 0.65f);
                D2D1_GRADIENT_STOP st[2] = {{0.0f, c0}, {1.0f, c1}};
                ID2D1GradientStopCollection* sc = nullptr;
                if (SUCCEEDED(rt->CreateGradientStopCollection(st, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &sc)) && sc) {
                    ID2D1LinearGradientBrush* lg = nullptr;
                    if (SUCCEEDED(rt->CreateLinearGradientBrush(
                            D2D1::LinearGradientBrushProperties(D2D1::Point2F(x0, topY), D2D1::Point2F(x1, topY)), sc, &lg)) && lg) {
                        rt->FillRectangle(D2D1::RectF(x0, topY, x1, botY), lg);
                        lg->Release();
                    }
                    sc->Release();
                }
            };
            drawHorizRim(leftX, leftX + rimH, fEdge, fZero);
            drawHorizRim(rightX - rimH, rightX, fZero, fEdge);

            if (tier != QualityTier::Low) {
                brush->SetOpacity(1.0f);
                int glints = (tier == QualityTier::High) ? 18 : 10;
                for (int i = 0; i < glints; ++i) {
                    bool sideEdge = (i % 2 == 0);
                    float gx = sideEdge
                        ? ((i % 4 == 0) ? (leftX + 8.0f + (i * 11 % 26)) : (rightX - 8.0f - (i * 11 % 26)))
                        : (leftX + ((i * 97) % 100) * 0.01f * innerW);
                    float gy = sideEdge
                        ? (topY + ((i * 73) % 100) * 0.01f * innerH)
                        : ((i % 4 == 1) ? (topY + 8.0f + (i * 9 % 24)) : (botY - 8.0f - (i * 9 % 24)));
                    float tw = 0.35f + 0.65f * static_cast<float>(std::sin(timeSec * 2.4 + i * 1.7));
                    brush->SetColor(D2D1::ColorF(0.92f, 0.97f, 1.0f, std::clamp(0.45f * strength * tw, 0.03f, 0.65f)));
                    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(gx, gy), 1.8f * mon.dpiScale, 1.8f * mon.dpiScale), brush);
                }
            }
        }

        if (heatOn) {
            float bandTop = std::max(topY, botY - innerH * 0.12f);
            DrawVerticalGradientTint(rt, D2D1::ColorF(1.0f, 0.64f, 0.24f, 0.00f),
                                     D2D1::ColorF(1.0f, 0.56f, 0.20f, 0.24f),
                                     leftX, rightX, bandTop, botY, strength);
            if (tier != QualityTier::Low) {
                brush->SetOpacity(1.0f);
                brush->SetColor(D2D1::ColorF(1.0f, 0.72f, 0.34f, std::clamp(0.24f * strength, 0.02f, 0.36f)));
                int waves = (tier == QualityTier::High) ? 3 : 2;
                float stepX = 18.0f;
                for (int w = 0; w < waves; ++w) {
                    float baseY = botY - (12.0f + w * 18.0f) * mon.dpiScale;
                    float prevX = leftX;
                    float prevY = baseY + static_cast<float>(std::sin(timeSec * 3.0 + w * 1.9)) * 3.0f;
                    for (float x = leftX + stepX; x <= rightX; x += stepX) {
                        float curY = baseY + static_cast<float>(std::sin(x * 0.015f + timeSec * 3.0 + w * 1.9)) * 3.2f;
                        rt->DrawLine(D2D1::Point2F(prevX, prevY), D2D1::Point2F(x, curY), brush, 1.4f);
                        prevX = x;
                        prevY = curY;
                    }
                }
            }
        }
    }
}

void DrawNightStarsLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           QualityTier tier, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    int starsCount = std::clamp(static_cast<int>(28 * GetTierScale(tier)), 12, 65);
    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        float innerW = std::max(40.0f, mon.innerRightRel - mon.innerLeftRel);
        float skyH = std::max(40.0f, (mon.bottomLimitRelY - mon.innerTopRel) * 0.45f);
        for (int i = 0; i < starsCount; ++i) {
            float sx = mon.left + mon.innerLeftRel + (((i * 97 + 19) % 1000) / 1000.0f) * innerW;
            float sy = mon.top + mon.innerTopRel + 12.0f + (((i * 61 + 7) % 1000) / 1000.0f) * skyH;
            float twinkle = 0.45f + 0.55f * static_cast<float>(std::sin(timeSec * 2.2 + i * 1.3));
            float r = ((i % 5 == 0 && tier != QualityTier::Low) ? 2.0f : 1.35f) * mon.dpiScale;
            brush->SetColor(D2D1::ColorF(0.92f, 0.96f, 1.0f, std::clamp(0.55f * strength * twinkle, 0.04f, 0.85f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), r, r), brush);
        }
    }
}

void DrawAirQualityLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, QualityTier tier, const ForecastSnapshot& snap, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    double aqiVal = std::max(snap.aqiValue, snap.pm25Value * 2.5);
    float aqiNorm = std::clamp(static_cast<float>(aqiVal / 150.0), 0.15f, 1.0f);

    D2D1_COLOR_F base = cfg.hasCustomAirQualityColor
        ? cfg.colorAirQuality
        : ((aqiVal <= 75.0)
            ? D2D1::ColorF(0.95f, 0.84f, 0.32f, 0.55f)
            : ((aqiVal <= 125.0)
                ? D2D1::ColorF(0.96f, 0.56f, 0.24f, 0.62f)
                : D2D1::ColorF(0.92f, 0.30f, 0.30f, 0.68f)));

    int motes = std::clamp(static_cast<int>((18.0f + aqiNorm * 38.0f) * GetTierScale(tier)), 12, 72);
    brush->SetOpacity(1.0f);

    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float rightX = mon.left + mon.innerRightRel;
        float topY = mon.top + mon.innerTopRel;
        float botY = mon.top + mon.bottomLimitRelY;
        float innerW = std::max(40.0f, rightX - leftX);
        float innerH = std::max(40.0f, botY - topY);

        for (int i = 0; i < motes; ++i) {
            uint32_t hx = static_cast<uint32_t>(i * 2654435761u + 1013904223u);
            uint32_t hy = static_cast<uint32_t>(i * 1664525u + 22695477u);
            float u01 = static_cast<float>(hx & 0xFFFFu) / 65535.0f;
            float v01 = static_cast<float>(hy & 0xFFFFu) / 65535.0f;
            float mx = leftX + std::fmod(u01 * innerW + static_cast<float>(timeSec) * 10.0f +
                                         static_cast<float>(std::sin(timeSec * 1.1 + i)) * 14.0f, innerW);
            float my = topY + std::fmod(v01 * innerH +
                                        static_cast<float>(std::cos(timeSec * 0.9 + i * 1.7)) * 12.0f, innerH);
            float fade = ComputeMonitorEdgeFade(mon, mx, my, 24.0f);
            float r = ((i % 4 == 0) ? 2.5f : 1.7f) * mon.dpiScale;
            brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(0.42f * strength * aqiNorm * fade, 0.03f, 0.68f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(mx, my), r, r), brush);
        }
    }
}

void DrawPollenLayer(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                     const Settings& cfg, QualityTier tier, float strength, double timeSec) {
    if (strength <= 0.004f) return;
    int count = std::clamp(static_cast<int>(24 * GetTierScale(tier)), 10, 56);
    D2D1_COLOR_F base = cfg.hasCustomPollenColor ? cfg.colorPollen : D2D1::ColorF(0.97f, 0.89f, 0.42f, 0.72f);
    brush->SetOpacity(1.0f);
    for (const auto& mon : g_monitorRects) {
        float leftX = mon.left + mon.innerLeftRel;
        float topY = mon.top + mon.innerTopRel;
        float innerW = std::max(40.0f, mon.innerRightRel - mon.innerLeftRel);
        float innerH = std::max(40.0f, mon.bottomLimitRelY - mon.innerTopRel);
        for (int i = 0; i < count; ++i) {
            uint32_t hx = static_cast<uint32_t>(i * 2246822519u + 3266489917u);
            uint32_t hy = static_cast<uint32_t>(i * 374761393u + 668265263u);
            float u01 = static_cast<float>(hx & 0xFFFFu) / 65535.0f;
            float v01 = static_cast<float>(hy & 0xFFFFu) / 65535.0f;
            float px = leftX + std::fmod(u01 * innerW + static_cast<float>(timeSec) * 11.0f +
                                         static_cast<float>(std::sin(timeSec * 1.2 + i * 1.9)) * 20.0f, innerW);
            float py = topY + std::fmod(v01 * innerH +
                                        static_cast<float>(std::cos(timeSec * 1.0 + i * 2.3)) * 18.0f, innerH);
            float fade = ComputeMonitorEdgeFade(mon, px, py, 28.0f);
            float r = ((tier != QualityTier::Low && i % 3 == 0) ? 2.8f : 2.0f) * mon.dpiScale;
            brush->SetColor(D2D1::ColorF(base.r, base.g, base.b, std::clamp(base.a * strength * fade, 0.03f, 0.85f)));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), r, r), brush);
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

void DrawModeTabButton(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                       const Settings& cfg, D2D1_RECT_F rect, const wchar_t* label,
                       const std::wstring& actionId, bool selected, std::vector<InteractiveControl>& outCtrls) {
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rect, 6.0f, 6.0f);
    brush->SetOpacity(1.0f);
    if (selected) {
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.22f));
        rt->FillRoundedRectangle(rr, brush);
        brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f));
        rt->DrawRoundedRectangle(rr, brush, 1.6f);
        brush->SetColor(cfg.colorHudText);
    } else {
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.06f));
        rt->FillRoundedRectangle(rr, brush);
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.22f));
        rt->DrawRoundedRectangle(rr, brush, 1.0f);
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.55f));
    }
    rt->DrawTextW(label, static_cast<UINT32>(wcslen(label)), g_hudPillFormat, rect, brush);
    outCtrls.push_back(InteractiveControl{rect, actionId, label, D2D1::ColorF(0, 0, 0, 0), L""});
}

void DrawInfoIconButton(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                        const Settings& cfg, D2D1_RECT_F cardRect,
                        std::vector<InteractiveControl>& outCtrls) {
    float btnSz = 18.0f * g_primaryDpiScale;
    D2D1_RECT_F r = D2D1::RectF(cardRect.right - btnSz - 6.0f, cardRect.top + 6.0f,
                                cardRect.right - 6.0f, cardRect.top + 6.0f + btnSz);
    float cx = (r.left + r.right) * 0.5f;
    float cy = (r.top + r.bottom) * 0.5f;
    float rad = btnSz * 0.5f;

    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.10f, 0.14f, 0.22f, 0.72f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad), brush);
    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.48f));
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad), brush, 1.1f);
    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.90f));
    rt->DrawTextW(L"i", 1, g_hudPillFormat, r, brush);

    outCtrls.push_back(InteractiveControl{r, L"toggle_sources_view", L"i", D2D1::ColorF(0, 0, 0, 0), L""});
}

void DrawOutlinedTextCentered(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                              IDWriteTextFormat* fmt, const std::wstring& txt,
                              D2D1_RECT_F rect, D2D1_COLOR_F fgCol) {
    if (txt.empty() || !fmt) return;
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.02f, 0.02f, 0.03f, 0.95f));
    const float dx[4] = {-1.0f, 1.0f, 0.0f, 0.0f};
    const float dy[4] = {0.0f, 0.0f, -1.0f, 1.0f};
    for (int k = 0; k < 4; ++k) {
        rt->DrawTextW(txt.c_str(), static_cast<UINT32>(txt.size()), fmt,
                      D2D1::RectF(rect.left + dx[k], rect.top + dy[k], rect.right + dx[k], rect.bottom + dy[k]),
                      brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    brush->SetColor(fgCol);
    rt->DrawTextW(txt.c_str(), static_cast<UINT32>(txt.size()), fmt, rect, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
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

void DrawArcSegment(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                    float cx, float cy, float r, float startDeg, float endDeg, float strokeW, D2D1_COLOR_F col) {
    if (!g_d2dFactory || r < 4.0f || std::abs(endDeg - startDeg) < 0.5f) return;
    double a0 = startDeg * (kPi / 180.0);
    double a1 = endDeg * (kPi / 180.0);
    D2D1_POINT_2F p0 = D2D1::Point2F(cx + static_cast<float>(std::cos(a0)) * r, cy + static_cast<float>(std::sin(a0)) * r);
    D2D1_POINT_2F p1 = D2D1::Point2F(cx + static_cast<float>(std::cos(a1)) * r, cy + static_cast<float>(std::sin(a1)) * r);
    ID2D1PathGeometry* geo = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geo)) || !geo) return;
    ID2D1GeometrySink* sink = nullptr;
    if (SUCCEEDED(geo->Open(&sink)) && sink) {
        sink->BeginFigure(p0, D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddArc(D2D1::ArcSegment(
            p1, D2D1::SizeF(r, r), 0.0f,
            (endDeg >= startDeg) ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
            (std::abs(endDeg - startDeg) > 180.0f) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        sink->Release();
        brush->SetOpacity(1.0f);
        brush->SetColor(col);
        ID2D1StrokeStyle* strokeStyle = nullptr;
        g_d2dFactory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND),
            nullptr, 0, &strokeStyle);
        rt->DrawGeometry(geo, brush, strokeW, strokeStyle);
        if (strokeStyle) strokeStyle->Release();
    }
    geo->Release();
}

void DrawSpeedometerGauge(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                          float cx, float cy, float r, float val01,
                          float normStart01, float normEnd01, D2D1_COLOR_F activeCol,
                          bool multiZoneAir = false) {
    const float kStartDeg = 135.0f;
    const float kSpanDeg = 270.0f;
    float strokeW = std::max(5.0f, r * 0.16f);

    DrawArcSegment(rt, brush, cx, cy, r, kStartDeg, kStartDeg + kSpanDeg, strokeW,
                   D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.15f));

    if (multiZoneAir) {
        DrawArcSegment(rt, brush, cx, cy, r, kStartDeg, kStartDeg + kSpanDeg * 0.25f, strokeW,
                       D2D1::ColorF(0.24f, 0.85f, 0.48f, 0.90f));
        DrawArcSegment(rt, brush, cx, cy, r, kStartDeg + kSpanDeg * 0.25f, kStartDeg + kSpanDeg * 0.50f, strokeW,
                       D2D1::ColorF(0.95f, 0.82f, 0.24f, 0.88f));
        DrawArcSegment(rt, brush, cx, cy, r, kStartDeg + kSpanDeg * 0.50f, kStartDeg + kSpanDeg * 0.75f, strokeW,
                       D2D1::ColorF(0.96f, 0.54f, 0.22f, 0.88f));
        DrawArcSegment(rt, brush, cx, cy, r, kStartDeg + kSpanDeg * 0.75f, kStartDeg + kSpanDeg, strokeW,
                       D2D1::ColorF(0.92f, 0.28f, 0.34f, 0.90f));
    } else {
        float n0 = kStartDeg + kSpanDeg * std::clamp(normStart01, 0.0f, 1.0f);
        float n1 = kStartDeg + kSpanDeg * std::clamp(normEnd01, 0.0f, 1.0f);
        DrawArcSegment(rt, brush, cx, cy, r, n0, n1, strokeW, D2D1::ColorF(0.26f, 0.86f, 0.48f, 0.92f));
    }

    float curDeg = kStartDeg + kSpanDeg * std::clamp(val01, 0.0f, 1.0f);
    double curRad = curDeg * (kPi / 180.0);
    float dotX = cx + static_cast<float>(std::cos(curRad)) * r;
    float dotY = cy + static_cast<float>(std::sin(curRad)) * r;
    float dotR = strokeW * 0.78f;
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.06f, 0.08f, 0.13f, 0.98f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR + 2.5f, dotR + 2.5f), brush);
    brush->SetColor(activeCol);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR, dotR), brush);
}

void DrawWindCompassWidget(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, float cx, float cy, float r, double windDirDeg,
                           const std::wstring& speedMain, const std::wstring& speedSub) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), brush);

    int ticks = 48;
    for (int i = 0; i < ticks; ++i) {
        double deg = (360.0 * i) / ticks;
        int degMod = static_cast<int>(std::llround(deg)) % 90;
        if (degMod < 12 || degMod > 78) continue;
        double rad = (deg - 90.0) * (kPi / 180.0);
        float r0 = r * 0.82f;
        float r1 = r * 0.93f;
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.36f));
        rt->DrawLine(
            D2D1::Point2F(cx + static_cast<float>(std::cos(rad)) * r0, cy + static_cast<float>(std::sin(rad)) * r0),
            D2D1::Point2F(cx + static_cast<float>(std::cos(rad)) * r1, cy + static_cast<float>(std::sin(rad)) * r1),
            brush, 1.3f);
    }

    float innerR = r * 0.56f;
    brush->SetColor(D2D1::ColorF(0.16f, 0.22f, 0.34f, 0.85f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), innerR, innerR), brush);

    struct CardLabel { const wchar_t* txt; float dx; float dy; };
    CardLabel cards[4] = {
        {L"N", 0.0f, -r * 0.87f},
        {L"E", r * 0.87f, 0.0f},
        {L"S", 0.0f, r * 0.87f},
        {L"W", -r * 0.87f, 0.0f}
    };
    brush->SetColor(cfg.colorHudText);
    for (const auto& cl : cards) {
        rt->DrawTextW(cl.txt, 1, g_hudPillFormat,
                      D2D1::RectF(cx + cl.dx - 11.0f, cy + cl.dy - 9.0f, cx + cl.dx + 11.0f, cy + cl.dy + 9.0f), brush);
    }

    double rad = (windDirDeg - 90.0) * (kPi / 180.0);
    float cosA = static_cast<float>(std::cos(rad));
    float sinA = static_cast<float>(std::sin(rad));
    float perpX = -sinA;
    float perpY = cosA;

    float tailR = r * 0.86f;
    float tipR = r * 0.90f;
    float shaftStart = innerR + 1.0f;

    brush->SetColor(D2D1::ColorF(0.96f, 0.98f, 1.0f, 0.95f));
    rt->DrawLine(D2D1::Point2F(cx - cosA * shaftStart, cy - sinA * shaftStart),
                 D2D1::Point2F(cx - cosA * tailR, cy - sinA * tailR), brush, 2.5f);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - cosA * tailR, cy - sinA * tailR), 4.0f, 4.0f), brush);

    rt->DrawLine(D2D1::Point2F(cx + cosA * shaftStart, cy + sinA * shaftStart),
                 D2D1::Point2F(cx + cosA * (tipR - 7.0f), cy + sinA * (tipR - 7.0f)), brush, 2.5f);
    if (g_d2dFactory) {
        ID2D1PathGeometry* arrowGeo = nullptr;
        if (SUCCEEDED(g_d2dFactory->CreatePathGeometry(&arrowGeo)) && arrowGeo) {
            ID2D1GeometrySink* sink = nullptr;
            if (SUCCEEDED(arrowGeo->Open(&sink)) && sink) {
                D2D1_POINT_2F pTip = D2D1::Point2F(cx + cosA * tipR, cy + sinA * tipR);
                D2D1_POINT_2F pBase = D2D1::Point2F(cx + cosA * (tipR - 10.0f), cy + sinA * (tipR - 10.0f));
                sink->BeginFigure(pTip, D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(D2D1::Point2F(pBase.x + perpX * 5.5f, pBase.y + perpY * 5.5f));
                sink->AddLine(D2D1::Point2F(pBase.x - perpX * 5.5f, pBase.y - perpY * 5.5f));
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                sink->Release();
                rt->FillGeometry(arrowGeo, brush);
            }
            arrowGeo->Release();
        }
    }

    float mainH = cfg.hudFontSizePx * g_primaryDpiScale * 2.15f;
    rt->DrawTextW(speedMain.c_str(), static_cast<UINT32>(speedMain.size()), g_hudMediumFormat,
                  D2D1::RectF(cx - innerR, cy - mainH * 0.68f, cx + innerR, cy + mainH * 0.34f), brush);
    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.78f));
    rt->DrawTextW(speedSub.c_str(), static_cast<UINT32>(speedSub.size()), g_hudPillFormat,
                  D2D1::RectF(cx - innerR, cy + mainH * 0.22f, cx + innerR, cy + innerR * 0.88f), brush);
}

void DrawSmallDirectionArrow(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             float cx, float cy, float len, double windDirDeg, D2D1_COLOR_F col) {
    double rad = (windDirDeg - 90.0) * (kPi / 180.0);
    float dx = static_cast<float>(std::cos(rad)) * len * 0.5f;
    float dy = static_cast<float>(std::sin(rad)) * len * 0.5f;
    float p0x = cx - dx, p0y = cy - dy;
    float p1x = cx + dx, p1y = cy + dy;

    brush->SetOpacity(1.0f);
    brush->SetColor(col);
    rt->DrawLine(D2D1::Point2F(p0x, p0y), D2D1::Point2F(p1x, p1y), brush, 1.5f);

    double head1 = rad + 2.55;
    double head2 = rad - 2.55;
    float hl = std::max(3.2f, len * 0.38f);
    rt->DrawLine(D2D1::Point2F(p1x, p1y),
                 D2D1::Point2F(p1x + static_cast<float>(std::cos(head1)) * hl, p1y + static_cast<float>(std::sin(head1)) * hl),
                 brush, 1.5f);
    rt->DrawLine(D2D1::Point2F(p1x, p1y),
                 D2D1::Point2F(p1x + static_cast<float>(std::cos(head2)) * hl, p1y + static_cast<float>(std::sin(head2)) * hl),
                 brush, 1.5f);
}

void DrawDynamicWindForceIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                              float x, float y, float w, float h, double windKmh, double timeSec) {
    float norm = std::clamp(static_cast<float>(windKmh / 45.0), 0.0f, 1.0f);
    int lines = (windKmh < 8.0) ? 1 : ((windKmh < 24.0) ? 2 : 3);
    float speed = 2.0f + norm * 7.0f;
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.45f, 0.82f, 1.0f, 0.92f));
    for (int i = 0; i < lines; ++i) {
        float ly = y + (h * (static_cast<float>(i) + 0.5f)) / static_cast<float>(lines);
        float len = w * (0.62f + 0.35f * norm - i * 0.08f);
        float shift = static_cast<float>(std::sin(timeSec * speed + i * 1.4)) * (2.0f + norm * 3.0f);
        rt->DrawLine(D2D1::Point2F(x + shift, ly), D2D1::Point2F(x + len + shift, ly), brush, 1.7f + norm * 0.7f);
        if (windKmh >= 16.0) {
            rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x + len + shift, ly - 2.2f), 2.3f, 2.3f), brush, 1.4f);
        }
    }
}

void DrawMiniUvSunIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                        float cx, float cy, float r, double uvVal, double timeSec) {
    float t = std::clamp(static_cast<float>(uvVal / 10.0), 0.0f, 1.0f);
    D2D1_COLOR_F sunCol = D2D1::ColorF(
        1.0f,
        std::clamp(0.88f - t * 0.58f, 0.24f, 0.92f),
        std::clamp(0.28f - t * 0.20f, 0.08f, 0.34f),
        0.96f);
    brush->SetOpacity(1.0f);
    brush->SetColor(sunCol);
    int rays = 8;
    float innerR = r * 0.62f;
    float outerR = r * (0.95f + 0.12f * static_cast<float>(std::sin(timeSec * 3.0)));
    for (int i = 0; i < rays; ++i) {
        double ang = (2.0 * kPi * i) / rays + timeSec * 0.25;
        rt->DrawLine(
            D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * innerR, cy + static_cast<float>(std::sin(ang)) * innerR),
            D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * outerR, cy + static_cast<float>(std::sin(ang)) * outerR),
            brush, 1.5f);
    }
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r * 0.45f, r * 0.45f), brush);
}

void DrawFillingRainDropIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             float cx, float topY, float h, float fill01, D2D1_COLOR_F col, double timeSec) {
    if (!g_d2dFactory || h < 8.0f) return;
    float w = h * 0.72f;
    float botY = topY + h;
    float tipY = topY;
    float bulbCy = topY + h * 0.68f;
    float bulbR = w * 0.48f;

    ID2D1PathGeometry* dropGeo = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&dropGeo)) || !dropGeo) return;
    ID2D1GeometrySink* sink = nullptr;
    if (SUCCEEDED(dropGeo->Open(&sink)) && sink) {
        sink->BeginFigure(D2D1::Point2F(cx, tipY), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddBezier(D2D1::BezierSegment(
            D2D1::Point2F(cx + bulbR * 0.95f, topY + h * 0.34f),
            D2D1::Point2F(cx + bulbR, bulbCy - bulbR * 0.4f),
            D2D1::Point2F(cx + bulbR, bulbCy)));
        sink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(cx - bulbR, bulbCy),
            D2D1::SizeF(bulbR, bulbR * 0.72f), 0.0f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        sink->AddBezier(D2D1::BezierSegment(
            D2D1::Point2F(cx - bulbR, bulbCy - bulbR * 0.4f),
            D2D1::Point2F(cx - bulbR * 0.95f, topY + h * 0.34f),
            D2D1::Point2F(cx, tipY)));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        sink->Release();

        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(col.r, col.g, col.b, 0.16f));
        rt->FillGeometry(dropGeo, brush);

        float clampedFill = std::clamp(fill01, 0.0f, 1.0f);
        if (clampedFill > 0.01f) {
            float waveShift = static_cast<float>(std::sin(timeSec * 3.0)) * 1.4f;
            float fillTopY = botY - h * clampedFill + waveShift;
            rt->PushAxisAlignedClip(D2D1::RectF(cx - bulbR - 2.0f, fillTopY, cx + bulbR + 2.0f, botY + 2.0f),
                                   D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            brush->SetColor(D2D1::ColorF(col.r, col.g, col.b, 0.92f));
            rt->FillGeometry(dropGeo, brush);
            rt->PopAxisAlignedClip();
        }

        brush->SetColor(D2D1::ColorF(col.r, col.g, col.b, 0.90f));
        rt->DrawGeometry(dropGeo, brush, 1.4f);
    }
    dropGeo->Release();
}

void DrawHumidityCircularGauge(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             float cx, float cy, float r, double fill01, double timeSec) {
    float strokeW = std::max(2.6f, r * 0.24f);
    brush->SetOpacity(1.0f);
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r),
                    (brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.16f)), brush), strokeW);

    double clamped = std::clamp(fill01, 0.0, 2.0);
    double firstLap = std::min(1.0, clamped);
    if (firstLap > 0.005) {
        float endDeg = -90.0f + static_cast<float>(firstLap * 360.0);
        DrawArcSegment(rt, brush, cx, cy, r, -90.0f, endDeg, strokeW, D2D1::ColorF(0.32f, 0.78f, 1.0f, 0.92f));
    }
    if (clamped > 1.005) {
        double secondLap = clamped - 1.0;
        float pulse = 0.88f + 0.12f * static_cast<float>(std::sin(timeSec * 3.5));
        float endDeg2 = -90.0f + static_cast<float>(secondLap * 360.0);
        DrawArcSegment(rt, brush, cx, cy, r, -90.0f, endDeg2, strokeW + 0.8f,
                       D2D1::ColorF(0.18f, 0.96f, 0.88f, pulse));
    }
}

void DrawFogIndicatorIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                          float x, float y, float w, float h, bool highRisk, double timeSec) {
    brush->SetOpacity(1.0f);
    D2D1_COLOR_F col = highRisk
        ? D2D1::ColorF(0.76f, 0.88f, 1.0f, 0.92f)
        : D2D1::ColorF(0.72f, 0.78f, 0.86f, 0.75f);
    brush->SetColor(col);
    int bars = highRisk ? 3 : 2;
    for (int i = 0; i < bars; ++i) {
        float by = y + (h * (static_cast<float>(i) + 0.5f)) / static_cast<float>(bars);
        float drift = static_cast<float>(std::sin(timeSec * (highRisk ? 2.2 : 1.2) + i * 1.3)) * 2.5f;
        float len = w * (0.85f - i * 0.14f);
        rt->DrawLine(D2D1::Point2F(x + drift, by), D2D1::Point2F(x + drift + len, by), brush, 1.7f);
    }
}

void DrawDewThermometerIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                            float cx, float topY, float h, double dewC) {
    if (h < 10.0f) return;
    float bulbR = std::max(3.5f, h * 0.20f);
    float bulbCy = topY + h - bulbR;
    float tubeTop = topY + 2.0f;
    float tubeBot = bulbCy - bulbR * 0.5f;
    float tubeW = std::max(2.4f, bulbR * 0.75f);

    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.85f, 0.90f, 0.98f, 0.75f));
    rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - tubeW * 0.5f, tubeTop, cx + tubeW * 0.5f, tubeBot), tubeW * 0.5f, tubeW * 0.5f), brush);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, bulbCy), bulbR, bulbR), brush);

    float fill01 = std::clamp(static_cast<float>((dewC + 10.0) / 40.0), 0.12f, 1.0f);
    float colTop = bulbCy - (bulbCy - tubeTop) * fill01;
    D2D1_COLOR_F liqCol = (dewC <= 2.0)
        ? D2D1::ColorF(0.32f, 0.78f, 1.0f, 0.95f)
        : D2D1::ColorF(0.98f, 0.56f, 0.24f, 0.95f);
    brush->SetColor(liqCol);
    rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - tubeW * 0.32f, colTop, cx + tubeW * 0.32f, bulbCy), tubeW * 0.32f, tubeW * 0.32f), brush);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, bulbCy), bulbR * 0.72f, bulbR * 0.72f), brush);
}

void DrawSolarFlarePulseIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             float cx, float cy, float r, double kpVal, double timeSec) {
    float act = std::clamp(static_cast<float>((kpVal - 1.0) / 6.0), 0.10f, 1.0f);
    float pulse = 0.5f + 0.5f * static_cast<float>(std::sin(timeSec * (2.5 + act * 6.0)));
    brush->SetOpacity(1.0f);

    float ringR = r * (0.55f + 0.42f * pulse);
    D2D1_COLOR_F ringCol = (kpVal >= 5.0)
        ? D2D1::ColorF(1.0f, 0.34f, 0.28f, 0.85f * (1.0f - pulse * 0.5f))
        : D2D1::ColorF(1.0f, 0.72f, 0.22f, 0.85f * (1.0f - pulse * 0.5f));
    brush->SetColor(ringCol);
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), ringR, ringR), brush, 1.6f);

    int spikes = 6;
    for (int i = 0; i < spikes; ++i) {
        double ang = (2.0 * kPi * i) / spikes + timeSec * (0.6 + act * 1.4);
        float r0 = r * 0.30f;
        float r1 = r * (0.62f + 0.28f * act * ((i % 2 == 0) ? 1.0f : 0.65f));
        rt->DrawLine(
            D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * r0, cy + static_cast<float>(std::sin(ang)) * r0),
            D2D1::Point2F(cx + static_cast<float>(std::cos(ang)) * r1, cy + static_cast<float>(std::sin(ang)) * r1),
            brush, 1.5f);
    }
    brush->SetColor(D2D1::ColorF(1.0f, 0.86f, 0.35f, 0.96f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r * 0.28f, r * 0.28f), brush);
}

void DrawPollenSwarmCluster(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             D2D1_COLOR_F col, D2D1_RECT_F box, double pollenVal, double timeSec) {
    float w = box.right - box.left;
    float h = box.bottom - box.top;
    if (w < 8.0f || h < 8.0f) return;
    float density = std::clamp(static_cast<float>(pollenVal / 60.0), 0.12f, 1.0f);
    int count = std::clamp(static_cast<int>(4.0f + density * 22.0f), 4, 26);

    brush->SetOpacity(1.0f);
    for (int i = 0; i < count; ++i) {
        uint32_t hx = static_cast<uint32_t>(i * 2654435761u + 17u);
        uint32_t hy = static_cast<uint32_t>(i * 19349663u + 89u);
        float bx = box.left + ((hx & 0xFFu) / 255.0f) * w;
        float by = box.top + ((hy & 0xFFu) / 255.0f) * h;
        float dx = static_cast<float>(std::sin(timeSec * (1.8 + density * 2.5) + i * 1.4)) * (2.5f + density * 3.5f);
        float dy = static_cast<float>(std::cos(timeSec * (1.5 + density * 2.2) + i * 2.1)) * (2.5f + density * 3.5f);
        float pr = 1.3f + density * 1.2f;
        brush->SetColor(D2D1::ColorF(col.r, col.g, col.b, std::clamp(0.45f + density * 0.50f, 0.45f, 0.96f)));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(bx + dx, by + dy), pr, pr), brush);
    }
}

void DrawDynamicEyeIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                        float cx, float cy, float rx, float ry, float open01, D2D1_COLOR_F col) {
    if (!g_d2dFactory) return;
    float t = std::clamp(open01, 0.0f, 1.0f);
    brush->SetOpacity(1.0f);

    if (t <= 0.12f) {
        brush->SetColor(D2D1::ColorF(col.r, col.g, col.b, 0.85f));
        rt->DrawLine(D2D1::Point2F(cx - rx, cy), D2D1::Point2F(cx + rx, cy), brush, 2.0f);
        for (int i = -1; i <= 1; ++i) {
            float lx = cx + i * rx * 0.45f;
            rt->DrawLine(D2D1::Point2F(lx, cy), D2D1::Point2F(lx + i * 2.0f, cy + 5.0f), brush, 1.5f);
        }
        return;
    }

    float curRy = ry * t;
    ID2D1PathGeometry* eyeGeo = nullptr;
    if (SUCCEEDED(g_d2dFactory->CreatePathGeometry(&eyeGeo)) && eyeGeo) {
        ID2D1GeometrySink* sink = nullptr;
        if (SUCCEEDED(eyeGeo->Open(&sink)) && sink) {
            D2D1_POINT_2F pL = D2D1::Point2F(cx - rx, cy);
            D2D1_POINT_2F pR = D2D1::Point2F(cx + rx, cy);
            sink->BeginFigure(pL, D2D1_FIGURE_BEGIN_FILLED);
            sink->AddBezier(D2D1::BezierSegment(
                D2D1::Point2F(cx - rx * 0.45f, cy - curRy * 1.35f),
                D2D1::Point2F(cx + rx * 0.45f, cy - curRy * 1.35f),
                pR));
            sink->AddBezier(D2D1::BezierSegment(
                D2D1::Point2F(cx + rx * 0.45f, cy + curRy * 1.35f),
                D2D1::Point2F(cx - rx * 0.45f, cy + curRy * 1.35f),
                pL));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();
            sink->Release();

            brush->SetColor(D2D1::ColorF(0.35f, 0.78f, 1.0f, 0.16f));
            rt->FillGeometry(eyeGeo, brush);
            brush->SetColor(D2D1::ColorF(col.r, col.g, col.b, 0.90f));
            rt->DrawGeometry(eyeGeo, brush, 1.9f);

            rt->PushAxisAlignedClip(D2D1::RectF(cx - rx, cy - curRy, cx + rx, cy + curRy), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            float irisR = std::min(rx * 0.44f, ry * 0.75f);
            brush->SetColor(D2D1::ColorF(0.35f, 0.78f, 1.0f, 0.88f));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), irisR, irisR), brush);
            brush->SetColor(D2D1::ColorF(0.06f, 0.09f, 0.14f, 0.95f));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), irisR * 0.46f, irisR * 0.46f), brush);
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.85f));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - irisR * 0.28f, cy - irisR * 0.28f), irisR * 0.20f, irisR * 0.20f), brush);
            rt->PopAxisAlignedClip();
        }
        eyeGeo->Release();
    }
}

void DrawAnimatedWaveColumn(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                            D2D1_RECT_F colRect, float fill01, D2D1_COLOR_F waveCol, double timeSec, int colIdx) {
    if (!g_d2dFactory || fill01 <= 0.01f) return;
    float h = colRect.bottom - colRect.top;
    float w = colRect.right - colRect.left;
    float topWaveY = colRect.bottom - h * std::clamp(fill01, 0.06f, 0.92f);

    ID2D1PathGeometry* geo = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geo)) || !geo) return;
    ID2D1GeometrySink* sink = nullptr;
    if (SUCCEEDED(geo->Open(&sink)) && sink) {
        sink->BeginFigure(D2D1::Point2F(colRect.left, colRect.bottom), D2D1_FIGURE_BEGIN_FILLED);
        int segs = 12;
        for (int i = 0; i <= segs; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(segs);
            float x = colRect.left + w * t;
            float y = topWaveY + static_cast<float>(std::sin(t * 5.5f + timeSec * 2.8 + colIdx * 0.9)) * 2.5f;
            sink->AddLine(D2D1::Point2F(x, y));
        }
        sink->AddLine(D2D1::Point2F(colRect.right, colRect.bottom));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        sink->Release();

        brush->SetOpacity(1.0f);
        brush->SetColor(D2D1::ColorF(waveCol.r, waveCol.g, waveCol.b, 0.42f));
        rt->FillGeometry(geo, brush);
    }
    geo->Release();
}

void DrawMiniMonitorPreview(CachedSurface& surf, const Settings& cfg, const GeneralEffectsState& fx,
                            float cloudPct01, double moonPhase01, bool sunOn, bool moonOn,
                            D2D1_RECT_F box, double timeSec) {
    ID2D1DCRenderTarget* rt = surf.rt;
    ID2D1SolidColorBrush* brush = surf.sharedBrush;
    float w = box.right - box.left;
    float h = box.bottom - box.top;
    if (!rt || !brush || w < 40.0f || h < 40.0f) return;

    rt->PushAxisAlignedClip(box, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    if (fx.nightStars) {
        for (int i = 0; i < 18; ++i) {
            float sx = box.left + (((i * 73 + 11) % 100) / 100.0f) * w;
            float sy = box.top + 8.0f + (((i * 41 + 5) % 100) / 100.0f) * (h * 0.55f);
            float tw = 0.45f + 0.55f * static_cast<float>(std::sin(timeSec * 2.4 + i));
            brush->SetColor(D2D1::ColorF(0.92f, 0.96f, 1.0f, tw * 0.85f));
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), 1.4f, 1.4f), brush);
        }
    }
    if (sunOn) {
        float scx = box.left + w * 0.22f;
        float scy = box.top + h * 0.30f;
        float sr = std::min(w, h) * 0.14f;
        DrawSunBodyHighQuality(rt, brush, scx, scy, sr, cfg.colorSun, 0.88f, timeSec);
    }
    if (moonOn) {
        float mcx = box.left + w * 0.20f;
        float mcy = box.top + h * 0.30f;
        float mr = std::min(w, h) * 0.13f;
        DrawMoonBodyHighQuality(rt, brush, mcx, mcy, mr, cfg.colorMoon, moonPhase01, 0.92f);
    }
    if (cloudPct01 > 0.01f) {
        QualityTier cTier = ResolveCategoryTier(cfg, cfg.perfClouds);
        int puffs = std::clamp(static_cast<int>(2.0f + cloudPct01 * 5.0f), 1, 7);
        float rx = 30.0f + cloudPct01 * 20.0f;
        float ry = 12.0f + cloudPct01 * 7.0f;
        float spanW = w + rx * 2.4f;
        float fadeDist = std::max(36.0f, rx * 1.4f);
        for (int i = 0; i < puffs; ++i) {
            float rawOff = std::fmod(i * (spanW / static_cast<float>(puffs)) + static_cast<float>(timeSec) * 14.0f, spanW);
            if (rawOff < 0.0f) rawOff += spanW;
            float cx = box.left - rx * 1.2f + rawOff;
            float cy = box.top + 18.0f + (i % 3) * 12.0f;
            float edgeDist = std::min(cx - (box.left - rx * 0.8f), (box.right + rx * 0.8f) - cx);
            float t = std::clamp(edgeDist / fadeDist, 0.0f, 1.0f);
            float edgeFade = t * t * (3.0f - 2.0f * t);
            if (edgeFade <= 0.01f) continue;
            DrawProceduralCloudCluster(rt, brush, cTier, cfg.colorClouds, cx, cy, rx, ry, 0.52f * edgeFade, i + 3, timeSec);
        }
    }
    if (fx.aurora) {
        brush->SetColor(D2D1::ColorF(cfg.colorAurora.r, cfg.colorAurora.g, cfg.colorAurora.b, 0.32f));
        rt->FillRectangle(D2D1::RectF(box.left, box.top + 4.0f, box.right, box.top + h * 0.34f), brush);
    }
    if (fx.fog || fx.smog || fx.dust) {
        D2D1_COLOR_F haze = fx.fog ? cfg.colorFog : (fx.smog ? cfg.colorSmog : cfg.colorDust);
        brush->SetColor(D2D1::ColorF(haze.r, haze.g, haze.b, 0.25f));
        rt->FillRectangle(D2D1::RectF(box.left, box.bottom - h * 0.22f, box.right, box.bottom), brush);
    }
    if (!g_falling.empty() && g_canvasWidth > 0 && g_canvasHeight > 0) {
        QualityTier pTier = ResolveCategoryTier(cfg, cfg.perfPrecip);
        size_t step = std::max<size_t>(1, g_falling.size() / 55);
        for (size_t i = 0; i < g_falling.size(); i += step) {
            FallingItem mp = g_falling[i];
            mp.x = box.left + (mp.x / static_cast<float>(g_canvasWidth)) * w;
            mp.y = box.top + (mp.y / static_cast<float>(g_canvasHeight)) * h;
            mp.size *= 0.62f;
            DrawItemShape(surf, cfg, pTier, mp, box.bottom - 2.0f);
        }
    }
    rt->PopAxisAlignedClip();
}

void DrawUniformMetricIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                           const Settings& cfg, int iconKind, float cx, float cy,
                           float boxSize, double metricVal, double timeSec) {
    float r = std::clamp(boxSize * 0.42f, 6.0f, 11.0f * g_primaryDpiScale);
    brush->SetOpacity(1.0f);

    if (iconKind == 0) {
        float cl01 = std::clamp(static_cast<float>(metricVal / 100.0), 0.15f, 1.0f);
        D2D1_COLOR_F col = cfg.hasCustomCloudsColor ? cfg.colorClouds : D2D1::ColorF(0.86f, 0.92f, 1.0f, 0.92f);
        col.a = std::clamp(0.45f + cl01 * 0.50f, 0.45f, 0.96f);
        brush->SetColor(col);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - r * 0.35f, cy + r * 0.14f), r * 0.55f, r * 0.40f), brush);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.35f, cy + r * 0.14f), r * 0.50f, r * 0.38f), brush);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy - r * 0.16f), r * 0.62f, r * 0.48f), brush);
        return;
    }
    if (iconKind == 1) {
        DrawMiniUvSunIcon(rt, brush, cx, cy, r, metricVal, timeSec);
        return;
    }
    if (iconKind == 2) {
        float fill01 = std::clamp(static_cast<float>(metricVal / 10.0), 0.0f, 1.0f);
        DrawFillingRainDropIcon(rt, brush, cx, cy - r, r * 2.0f, fill01, cfg.colorRain, timeSec);
        return;
    }
    if (iconKind == 3) {
        DrawHumidityCircularGauge(rt, brush, cx, cy, r * 0.85f, metricVal, timeSec);
        return;
    }
    if (iconKind == 4) {
        DrawFogIndicatorIcon(rt, brush, cx - r, cy - r * 0.7f, r * 2.0f, r * 1.4f, metricVal > 0.5, timeSec);
        return;
    }
    if (iconKind == 5) {
        DrawDewThermometerIcon(rt, brush, cx, cy - r, r * 2.0f, metricVal);
        return;
    }
    if (iconKind == 6) {
        DrawSolarFlarePulseIcon(rt, brush, cx, cy, r, metricVal, timeSec);
        return;
    }
    DrawPollenSwarmCluster(rt, brush, cfg.colorPollen,
                           D2D1::RectF(cx - r, cy - r, cx + r, cy + r), metricVal, timeSec);
}

void DrawFourRowMetricColumn(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             const Settings& cfg, D2D1_RECT_F colRect,
                             const wchar_t* name, const std::wstring& valStr, const std::wstring& unitStr,
                             int iconKind, double iconVal, double timeSec, float lineH) {
    float h = colRect.bottom - colRect.top;
    float cx = (colRect.left + colRect.right) * 0.5f;

    float y1 = colRect.top + h * 0.06f;
    float y2 = colRect.top + h * 0.25f;
    float y3 = colRect.top + h * 0.54f;
    float y4Cy = colRect.top + h * 0.82f;

    g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.68f));
    rt->DrawTextW(name, static_cast<UINT32>(wcslen(name)), g_hudPillFormat,
                  D2D1::RectF(colRect.left, y1, colRect.right, y1 + lineH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    brush->SetColor(cfg.colorHudText);
    rt->DrawTextW(valStr.c_str(), static_cast<UINT32>(valStr.size()), g_hudMediumFormat,
                  D2D1::RectF(colRect.left, y2, colRect.right, y3 + 2.0f), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.72f));
    rt->DrawTextW(unitStr.c_str(), static_cast<UINT32>(unitStr.size()), g_hudPillFormat,
                  D2D1::RectF(colRect.left, y3, colRect.right, y3 + lineH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    float iconSz = std::clamp(h * 0.22f, 16.0f, 24.0f * g_primaryDpiScale);
    DrawUniformMetricIcon(rt, brush, cfg, iconKind, cx, y4Cy, iconSz, iconVal, timeSec);
}

void DrawLargePrecipTypeIcon(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                             const Settings& cfg, int wmoCode, float cx, float cy, float sizePx) {
    float r = std::clamp(sizePx * 0.5f, 12.0f, 18.0f * g_primaryDpiScale);
    brush->SetOpacity(1.0f);

    if (wmoCode == 0 || wmoCode == 1) {
        brush->SetColor(D2D1::ColorF(cfg.colorSun.r, cfg.colorSun.g, cfg.colorSun.b, 0.96f));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r * 0.62f, r * 0.62f), brush);
        for (int i = 0; i < 8; ++i) {
            double a = (2.0 * kPi * i) / 8.0;
            rt->DrawLine(
                D2D1::Point2F(cx + static_cast<float>(std::cos(a)) * r * 0.72f,
                              cy + static_cast<float>(std::sin(a)) * r * 0.72f),
                D2D1::Point2F(cx + static_cast<float>(std::cos(a)) * r * 1.02f,
                              cy + static_cast<float>(std::sin(a)) * r * 1.02f),
                brush, 2.0f);
        }
        return;
    }

    if (wmoCode == 2) {
        brush->SetColor(D2D1::ColorF(cfg.colorSun.r, cfg.colorSun.g, cfg.colorSun.b, 0.92f));
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + r * 0.34f, cy - r * 0.28f), r * 0.44f, r * 0.44f), brush);
    }

    brush->SetColor(D2D1::ColorF(0.84f, 0.90f, 0.98f, 0.94f));
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy - r * 0.16f), r * 0.76f, r * 0.42f), brush);

    if ((wmoCode >= 51 && wmoCode <= 67) || (wmoCode >= 80 && wmoCode <= 82) || wmoCode >= 95) {
        brush->SetColor(D2D1::ColorF(cfg.colorRain.r, cfg.colorRain.g, cfg.colorRain.b, 0.98f));
        for (int i = -1; i <= 1; ++i) {
            float dx = cx + i * r * 0.42f;
            rt->DrawLine(D2D1::Point2F(dx, cy + r * 0.26f),
                         D2D1::Point2F(dx - r * 0.18f, cy + r * 0.95f), brush, 2.5f);
        }
        if (wmoCode >= 95) {
            brush->SetColor(D2D1::ColorF(1.0f, 0.86f, 0.25f, 0.98f));
            rt->DrawLine(D2D1::Point2F(cx, cy + r * 0.14f), D2D1::Point2F(cx - r * 0.20f, cy + r * 0.56f), brush, 2.2f);
            rt->DrawLine(D2D1::Point2F(cx - r * 0.20f, cy + r * 0.56f), D2D1::Point2F(cx + r * 0.14f, cy + r * 0.98f), brush, 2.2f);
        }
    } else if (wmoCode >= 71 && wmoCode <= 86) {
        brush->SetColor(D2D1::ColorF(cfg.colorSnow.r, cfg.colorSnow.g, cfg.colorSnow.b, 0.98f));
        for (int i = -1; i <= 1; ++i) {
            rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx + i * r * 0.44f, cy + r * 0.62f), r * 0.16f, r * 0.16f), brush);
        }
    }
}

struct SeriesSlotItem {
    std::wstring label;
    double tempMinC = 0.0;
    double tempMaxC = 0.0;
    double precipMm = 0.0;
    double windSpeedKmh = 0.0;
    double windDirDeg = 0.0;
    double pressureHpa = 1013.25;
    double aqiValue = 25.0;
    int weatherCode = 0;
    bool isCurrent = false;
};

void DrawUnifiedWeatherWindChart(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                                 const Settings& cfg, D2D1_RECT_F box, const std::wstring& title,
                                 const std::vector<SeriesSlotItem>& slots, float lineH, double timeSec,
                                 int pageIdx, int totalPages,
                                 const std::wstring& prevActionId, const std::wstring& nextActionId,
                                 std::vector<InteractiveControl>& outCtrls) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.20f, 0.45f, 0.78f, 0.14f));
    rt->FillRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush);
    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f));
    rt->DrawRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush, 1.0f);

    std::wstring hdrText = title;
    if (totalPages > 1) {
        wchar_t pBuf[24] = {};
        swprintf_s(pBuf, L" [%d/%d]", pageIdx + 1, totalPages);
        hdrText += pBuf;
    }
    brush->SetColor(cfg.colorHudText);
    rt->DrawTextW(hdrText.c_str(), static_cast<UINT32>(hdrText.size()), g_hudTitleFormat,
                  D2D1::RectF(box.left + 10.0f, box.top + 6.0f, box.right - 10.0f, box.top + 6.0f + lineH),
                  brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    if (slots.empty()) return;

    bool showArrows = (totalPages > 1);
    float arrowColW = showArrows ? (44.0f * g_primaryDpiScale) : 8.0f;
    float contentL = box.left + arrowColW;
    float contentR = box.right - arrowColW;

    float labelBandH = std::max(24.0f, lineH * 1.25f);
    float iconZoneH = std::max(26.0f, lineH * 1.35f);
    float subRowH = std::max(14.5f, lineH * 0.88f);
    float footerRowsH = subRowH * 4.0f + labelBandH + 6.0f;
    float chartTop = box.top + lineH + 12.0f;
    float chartBot = box.bottom - iconZoneH - footerRowsH;
    float chartH = std::max(54.0f, chartBot - chartTop);

    if (showArrows) {
        float arrowTop = box.top + lineH + 10.0f;
        float arrowBot = box.bottom - 8.0f;
        if (pageIdx > 0) {
            D2D1_RECT_F prevR = D2D1::RectF(box.left + 3.0f, arrowTop, contentL - 3.0f, arrowBot);
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f));
            rt->FillRoundedRectangle(D2D1::RoundedRect(prevR, 6.0f, 6.0f), brush);
            brush->SetColor(cfg.colorHudText);
            rt->DrawTextW(L"<", 1, g_hudMediumFormat, prevR, brush);
            outCtrls.push_back(InteractiveControl{prevR, prevActionId, L"<", D2D1::ColorF(0, 0, 0, 0), L""});
        }
        if (pageIdx + 1 < totalPages) {
            D2D1_RECT_F nextR = D2D1::RectF(contentR + 3.0f, arrowTop, box.right - 3.0f, arrowBot);
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f));
            rt->FillRoundedRectangle(D2D1::RoundedRect(nextR, 6.0f, 6.0f), brush);
            brush->SetColor(cfg.colorHudText);
            rt->DrawTextW(L">", 1, g_hudMediumFormat, nextR, brush);
            outCtrls.push_back(InteractiveControl{nextR, nextActionId, L">", D2D1::ColorF(0, 0, 0, 0), L""});
        }
    }

    int count = static_cast<int>(slots.size());
    float colW = (contentR - contentL) / static_cast<float>(count);

    double gMin = slots[0].tempMinC, gMax = slots[0].tempMaxC, maxPrecip = 0.2;
    for (const auto& s : slots) {
        gMin = std::min(gMin, s.tempMinC);
        gMax = std::max(gMax, s.tempMaxC);
        maxPrecip = std::max(maxPrecip, s.precipMm);
    }
    if (std::abs(gMax - gMin) < 1.5) { gMin -= 1.0; gMax += 1.0; }

    for (int i = 0; i < count; ++i) {
        const auto& s = slots[i];
        float x0 = contentL + i * colW;
        float x1 = x0 + colW;
        float cx = (x0 + x1) * 0.5f;

        if (s.isCurrent) {
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f));
            rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x0 + 2.0f, chartTop - 2.0f, x1 - 2.0f, box.bottom - 4.0f), 6.0f, 6.0f), brush);
        }

        float pFill = (s.precipMm > 0.01) ? std::clamp(static_cast<float>(s.precipMm / maxPrecip), 0.12f, 0.85f) : 0.0f;
        if (pFill > 0.0f) {
            DrawAnimatedWaveColumn(rt, brush, D2D1::RectF(x0 + 4.0f, chartTop, x1 - 4.0f, chartBot), pFill, cfg.colorRain, timeSec, i);
        }

        double tLoNorm = std::clamp((s.tempMinC - gMin) / (gMax - gMin), 0.0, 1.0);
        double tHiNorm = std::clamp((s.tempMaxC - gMin) / (gMax - gMin), 0.0, 1.0);
        float yHi = chartBot - static_cast<float>(tHiNorm) * (chartH - lineH * 1.9f) - lineH * 0.95f;
        float yLo = chartBot - static_cast<float>(tLoNorm) * (chartH - lineH * 1.9f) - lineH * 0.95f;
        if (std::abs(yLo - yHi) < 6.0f) yLo = yHi + 7.0f;

        float tBarW = std::clamp(colW * 0.14f, 4.0f, 7.5f);
        brush->SetColor(D2D1::ColorF(1.0f, 0.80f, 0.34f, 0.95f));
        rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - tBarW * 0.5f, yHi, cx + tBarW * 0.5f, yLo), tBarW * 0.5f, tBarW * 0.5f), brush);

        std::wstring hiStr = FormatTempValue(s.tempMaxC, cfg.tempUnit, true);
        std::wstring loStr = FormatTempValue(s.tempMinC, cfg.tempUnit, true);
        brush->SetColor(cfg.colorHudText);
        rt->DrawTextW(hiStr.c_str(), static_cast<UINT32>(hiStr.size()), g_hudPillFormat,
                      D2D1::RectF(x0, yHi - lineH * 0.90f, x1, yHi), brush);
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.80f));
        rt->DrawTextW(loStr.c_str(), static_cast<UINT32>(loStr.size()), g_hudPillFormat,
                      D2D1::RectF(x0, yLo + 1.0f, x1, yLo + lineH * 0.90f), brush);

        D2D1_RECT_F lblPill = D2D1::RectF(x0 + 2.0f, chartBot + 2.0f, x1 - 2.0f, chartBot + labelBandH);
        brush->SetColor(s.isCurrent
            ? D2D1::ColorF(0.32f, 0.72f, 1.0f, 0.28f)
            : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f));
        rt->FillRoundedRectangle(D2D1::RoundedRect(lblPill, 5.0f, 5.0f), brush);
        brush->SetColor(cfg.colorHudText);
        g_hudTitleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        rt->DrawTextW(s.label.c_str(), static_cast<UINT32>(s.label.size()), g_hudTitleFormat,
                      D2D1::RectF(lblPill.left + 2.0f, lblPill.top + 1.0f, lblPill.right - 2.0f, lblPill.bottom),
                      brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        g_hudTitleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

        float iconY = chartBot + labelBandH + 2.0f;
        DrawLargePrecipTypeIcon(rt, brush, cfg, s.weatherCode, cx, iconY + iconZoneH * 0.50f, lineH * 1.40f);

        float rowY = iconY + iconZoneH + 2.0f;

        DrawSmallDirectionArrow(rt, brush, cx - 14.0f, rowY + subRowH * 0.5f, 9.0f, s.windDirDeg, D2D1::ColorF(0.48f, 0.82f, 1.0f, 0.95f));
        std::wstring dirStr = WindDegToCompass16(s.windDirDeg);
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.92f));
        rt->DrawTextW(dirStr.c_str(), static_cast<UINT32>(dirStr.size()), g_hudPillFormat,
                      D2D1::RectF(cx - 6.0f, rowY, x1, rowY + subRowH), brush);
        rowY += subRowH;

        std::wstring spdMain, spdSub;
        FormatWindSplit(s.windSpeedKmh, cfg.windUnit, spdMain, spdSub);
        std::wstring spdStr = spdMain + L" " + spdSub;
        brush->SetColor(D2D1::ColorF(0.55f, 0.84f, 1.0f, 0.92f));
        rt->DrawTextW(spdStr.c_str(), static_cast<UINT32>(spdStr.size()), g_hudPillFormat,
                      D2D1::RectF(x0, rowY, x1, rowY + subRowH), brush);
        rowY += subRowH;

        std::wstring pStr = FormatPressureCompact(s.pressureHpa, cfg.pressureUnit);
        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.78f));
        rt->DrawTextW(pStr.c_str(), static_cast<UINT32>(pStr.size()), g_hudPillFormat,
                      D2D1::RectF(x0, rowY, x1, rowY + subRowH), brush);
        rowY += subRowH;

        D2D1_COLOR_F aqiCol = (s.aqiValue <= 50.0)
            ? D2D1::ColorF(0.24f, 0.85f, 0.48f, 0.95f)
            : ((s.aqiValue <= 100.0) ? D2D1::ColorF(0.95f, 0.82f, 0.24f, 0.95f) : D2D1::ColorF(0.92f, 0.34f, 0.34f, 0.95f));
        brush->SetColor(aqiCol);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - 14.0f, rowY + subRowH * 0.5f), 3.0f, 3.0f), brush);
        wchar_t aBuf[16] = {};
        swprintf_s(aBuf, L"%.0f", s.aqiValue);
        brush->SetColor(cfg.colorHudText);
        rt->DrawTextW(aBuf, static_cast<UINT32>(wcslen(aBuf)), g_hudPillFormat,
                      D2D1::RectF(cx - 8.0f, rowY, x1, rowY + subRowH), brush);
    }
}

void DrawAstroRiseSetBar(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                        const Settings& cfg, const ForecastSnapshot& snap, double hour,
                        D2D1_RECT_F box, float lineH) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.95f, 0.72f, 0.26f, 0.12f));
    rt->FillRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush);
    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f));
    rt->DrawRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush, 1.0f);

    double effSr = cfg.customSunriseHour, effSs = cfg.customSunsetHour;
    double effMr = cfg.customMoonriseHour, effMs = cfg.customMoonsetHour;
    bool dummy = false;
    ResolveCelestialWindow(cfg, snap, true, hour, effSr, effSs, dummy);
    ResolveCelestialWindow(cfg, snap, false, hour, effMr, effMs, dummy);

    float w = (box.right - box.left) / 4.0f;
    float cy = (box.top + box.bottom) * 0.5f;

    struct RiseSetEntry {
        bool isRise;
        std::wstring timeStr;
        D2D1_COLOR_F col;
    };
    RiseSetEntry items[4] = {
        {true,  FormatHourHhMm(effSr), D2D1::ColorF(1.0f, 0.80f, 0.32f, 0.95f)},
        {false, FormatHourHhMm(effSs), D2D1::ColorF(1.0f, 0.58f, 0.28f, 0.95f)},
        {true,  FormatHourHhMm(effMr), D2D1::ColorF(0.72f, 0.84f, 1.0f, 0.95f)},
        {false, FormatHourHhMm(effMs), D2D1::ColorF(0.56f, 0.70f, 0.96f, 0.95f)}
    };

    for (int i = 0; i < 4; ++i) {
        float x0 = box.left + i * w;
        float iconX = x0 + 18.0f;
        brush->SetColor(items[i].col);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(iconX, cy), 6.5f, 6.5f), brush);

        float arrX = iconX + 12.0f;
        float ay0 = cy + 5.0f;
        float ay1 = cy - 5.0f;
        rt->DrawLine(D2D1::Point2F(arrX, ay0), D2D1::Point2F(arrX, ay1), brush, 1.6f);
        float dirSign = items[i].isRise ? 1.0f : -1.0f;
        rt->DrawLine(D2D1::Point2F(arrX, ay1), D2D1::Point2F(arrX - 3.2f, ay1 + dirSign * 3.5f), brush, 1.6f);
        rt->DrawLine(D2D1::Point2F(arrX, ay1), D2D1::Point2F(arrX + 3.2f, ay1 + dirSign * 3.5f), brush, 1.6f);

        brush->SetColor(cfg.colorHudText);
        rt->DrawTextW(items[i].timeStr.c_str(), static_cast<UINT32>(items[i].timeStr.size()), g_hudBodyFormat,
                      D2D1::RectF(arrX + 8.0f, cy - lineH * 0.5f, x0 + w - 4.0f, cy + lineH * 0.5f), brush);
    }
}

void DrawDetailTableOverlay(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                            const Settings& cfg, const ForecastSnapshot& snap,
                            bool isDays, DetailWidgetKind mode, double hour, double moonPhase01,
                            D2D1_RECT_F box, float lineH) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.14f, 0.20f, 0.34f, 0.28f));
    rt->FillRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush);
    brush->SetColor(D2D1::ColorF(0.45f, 0.78f, 1.0f, 0.45f));
    rt->DrawRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush, 1.2f);

    float rowH = std::max(18.0f, lineH + 1.5f);
    std::wstring hdr = L"DETAIL TABLE (click to close)";
    if (mode == DetailWidgetKind::WeatherAll) hdr = isDays ? L"DAILY METEO TABLE (click to close)" : L"24H METEO TABLE (click to close)";
    else if (mode == DetailWidgetKind::NowHero) hdr = L"TEMPERATURE & SKY VALUES (click to close)";
    else if (mode == DetailWidgetKind::NowWindPrecip) hdr = L"WIND, PRECIP & PRESSURE VALUES (click to close)";
    else if (mode == DetailWidgetKind::NowAirVis) hdr = L"AIR, VISIBILITY & POLLEN VALUES (click to close)";
    else if (mode == DetailWidgetKind::AstroSun) hdr = L"SOLAR SCHEDULE & VALUES (click to close)";
    else if (mode == DetailWidgetKind::AstroMoon) hdr = L"LUNAR EPHEMERIS & VALUES (click to close)";
    else if (mode == DetailWidgetKind::AstroUvEmi) hdr = L"UV, EMI & UPCOMING EVENTS (click to close)";

    brush->SetColor(D2D1::ColorF(0.55f, 0.84f, 1.0f, 0.98f));
    rt->DrawTextW(hdr.c_str(), static_cast<UINT32>(hdr.size()), g_hudTitleFormat,
                  D2D1::RectF(box.left + 10.0f, box.top + 6.0f, box.right - 10.0f, box.top + 6.0f + rowH),
                  brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    float tblL = box.left + 10.0f;
    float tblW = std::max(100.0f, (box.right - 10.0f) - tblL);
    float y = box.top + 8.0f + rowH;

    auto drawRowCells = [&](const std::vector<float>& fracs, const std::vector<std::wstring>& vals, bool isHeader, bool zebra) {
        if (y + rowH > box.bottom - 4.0f) return;
        if (zebra) {
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.04f));
            rt->FillRectangle(D2D1::RectF(tblL - 4.0f, y, tblL + tblW + 4.0f, y + rowH), brush);
        }
        float curX = tblL;
        for (size_t c = 0; c < fracs.size() && c < vals.size(); ++c) {
            float cw = tblW * fracs[c];
            brush->SetColor(isHeader
                ? D2D1::ColorF(0.62f, 0.82f, 1.0f, 0.92f)
                : cfg.colorHudText);
            g_hudBodyFormat->SetTextAlignment((c == 0) ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER);
            rt->DrawTextW(vals[c].c_str(), static_cast<UINT32>(vals[c].size()),
                          isHeader ? g_hudTitleFormat : g_hudBodyFormat,
                          D2D1::RectF(curX, y, curX + cw - 2.0f, y + rowH),
                          brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            curX += cw;
        }
        g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        y += rowH;
        if (isHeader) {
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.18f));
            rt->DrawLine(D2D1::Point2F(tblL, y - 1.0f), D2D1::Point2F(tblL + tblW, y - 1.0f), brush, 1.0f);
            y += 2.0f;
        }
    };

    if (mode == DetailWidgetKind::NowHero || mode == DetailWidgetKind::NowWindPrecip ||
        mode == DetailWidgetKind::NowAirVis || mode == DetailWidgetKind::AstroSun ||
        mode == DetailWidgetKind::AstroMoon || mode == DetailWidgetKind::AstroUvEmi) {
        std::vector<float> fr = {0.42f, 0.34f, 0.24f};
        drawRowCells(fr, {L"Parameter", L"Value", L"Status / Note"}, true, false);
        int idx = 0;
        auto addKv = [&](const std::wstring& k, const std::wstring& v, const std::wstring& n) {
            drawRowCells(fr, {k, v, n}, false, (idx++ % 2 == 1));
        };

        if (mode == DetailWidgetKind::NowHero) {
            addKv(L"Current Temperature", snap.mTemp.available ? FormatTempValue(snap.tempC, cfg.tempUnit, false) : L"--", L"Live");
            addKv(L"Feels Like", snap.mFeelsLike.available ? FormatTempValue(snap.apparentTempC, cfg.tempUnit, false) : L"--", L"Apparent");
            addKv(L"Today Max / Min", snap.hasTodayMinMax ? (FormatTempValue(snap.todayMaxC, cfg.tempUnit, true) + L" / " + FormatTempValue(snap.todayMinC, cfg.tempUnit, true)) : L"--", L"Daily range");
            addKv(L"Condition", snap.mCondition.available ? WmoCodeToLabel(snap.weatherCode) : L"--", L"WMO " + std::to_wstring(snap.weatherCode));
            addKv(L"Day / Night", snap.mDayNight.available ? FormatDayNightValue(snap.isDay, cfg.dayNightUnit) : L"--", snap.isDay ? L"Daylight" : L"Night");
        } else if (mode == DetailWidgetKind::NowWindPrecip) {
            addKv(L"Wind Speed", snap.mWind.available ? FormatWindValue(snap.windSpeedKmh, cfg.windUnit) : L"--", WindDegToCompass16(snap.windDirDeg));
            addKv(L"Wind Gusts", snap.mGusts.available ? FormatWindValue(snap.windGustsKmh, cfg.windUnit) : L"--", L"Peak");
            addKv(L"Cloud Cover", snap.mClouds.available ? FormatCloudValue(snap.cloudCoverPct, cfg.cloudUnit) : L"--", L"Sky");
            addKv(L"Humidity", snap.mHumidity.available ? FormatHumidityValue(snap.humidityPct, cfg.humidityUnit) : L"--", L"Relative");
            addKv(L"Precipitation", snap.mPrecip.available ? FormatPrecipValue(snap.precipMm, cfg.precipUnit) : L"--", PrecipKindToLabel(snap.precipKind));
            std::wstring pMain, pSub, pNorm;
            FormatPressureSplit(snap.pressureHpa, cfg.pressureUnit, pMain, pSub, pNorm);
            addKv(L"Pressure", snap.mPressure.available ? (pMain + L" " + pSub) : L"--", pNorm);
        } else if (mode == DetailWidgetKind::NowAirVis) {
            addKv(L"Visibility", snap.mVisibility.available ? FormatVisibilityValue(snap.visibilityM, cfg.visibilityUnit) : L"--", L"Range");
            bool fogHigh = (snap.valid && snap.mTemp.available && snap.mDewPoint.available && (snap.tempC - snap.dewPointC <= 2.0));
            addKv(L"Fog Risk", fogHigh ? L"High" : ((snap.tempC - snap.dewPointC <= 5.0) ? L"Moderate" : L"Low"), L"T - Dew");
            addKv(L"Dew Point", snap.mDewPoint.available ? FormatTempValue(snap.dewPointC, cfg.tempUnit, false) : L"--", L"Condensation");
            addKv(L"Pollen Index", snap.mPollen.available ? FormatPollenValue(snap.pollenMaxValue, cfg.pollenUnit) : L"--", L"Max");
            std::wstring aMain, aSub;
            FormatAqiSplit(snap.aqiValue, snap.pm25Value, cfg.aqiUnit, aMain, aSub);
            addKv(L"Air Quality", snap.mAqi.available ? (aMain + L" " + aSub) : L"--", (snap.aqiValue <= 50.0) ? L"Good" : L"Elevated");
        } else if (mode == DetailWidgetKind::AstroSun) {
            double effSr = cfg.customSunriseHour, effSs = cfg.customSunsetHour;
            bool dummy = false;
            ResolveCelestialWindow(cfg, snap, true, hour, effSr, effSs, dummy);
            double dayLen = effSs - effSr; if (dayLen < 0.0) dayLen += 24.0;
            addKv(L"Sunrise", FormatHourHhMm(effSr), L"Local");
            addKv(L"Sunset", FormatHourHhMm(effSs), L"Local");
            addKv(L"Daylight Duration", FormatDurationHhMm(dayLen), L"Span");
            addKv(L"Golden Hour AM", FormatHourHhMm(effSr) + L"-" + FormatHourHhMm(effSr + 0.75), L"Morning");
            addKv(L"Golden Hour PM", FormatHourHhMm(effSs - 0.75) + L"-" + FormatHourHhMm(effSs), L"Evening");
            addKv(L"Blue Hour AM / PM", FormatHourHhMm(effSr - 0.45) + L" / " + FormatHourHhMm(effSs + 0.45), L"Twilight");
        } else if (mode == DetailWidgetKind::AstroMoon) {
            double effMr = cfg.customMoonriseHour, effMs = cfg.customMoonsetHour;
            bool dummy = false;
            ResolveCelestialWindow(cfg, snap, false, hour, effMr, effMs, dummy);
            double altDeg = 0.0, azDeg = 0.0;
            std::wstring nextNew, nextFull;
            ComputeLunarEphemerisDisplay(moonPhase01, hour, effMr, effMs, cfg.latitude, altDeg, azDeg, nextNew, nextFull);
            wchar_t b1[32] = {}, b2[32] = {}, b3[32] = {};
            swprintf_s(b1, L"%.0f%%", 50.0 * (1.0 - std::cos(moonPhase01 * 2.0 * kPi)));
            swprintf_s(b2, L"%.0f°", altDeg);
            swprintf_s(b3, L"%.0f° %s", azDeg, WindDegToCompass16(azDeg));
            addKv(L"Moon Phase", MoonPhaseToName(moonPhase01), b1);
            addKv(L"Moonrise / Moonset", FormatHourHhMm(effMr) + L" / " + FormatHourHhMm(effMs), L"Local");
            addKv(L"Altitude / Azimuth", std::wstring(b2) + L" / " + b3, L"Current");
            addKv(L"Next New / Full Moon", nextNew + L" / " + nextFull, L"Upcoming");
            if (snap.hasNeoData) {
                wchar_t nb[48] = {};
                swprintf_s(nb, L"%d objects (%.1f LD)", snap.neoCountToday, snap.neoClosestLd);
                addKv(L"Near-Earth Asteroids", nb, L"NASA NeoWs");
            }
        } else if (mode == DetailWidgetKind::AstroUvEmi) {
            wchar_t kpBuf[32] = {};
            swprintf_s(kpBuf, L"Kp %.1f", snap.flareKpEst);
            addKv(L"UV Index", snap.mUv.available ? FormatUvValue(snap.uvIndex, cfg.uvUnit) : L"--", (snap.uvIndex >= 6.0) ? L"High" : L"Normal");
            addKv(L"EMI / Geomagnetic", kpBuf, (snap.flareKpEst >= 5.0) ? L"Storm" : L"Quiet");
            addKv(L"Solar Activity", snap.hasSpaceWeather ? snap.spaceWeatherText : L"Quiet solar", L"NASA DONKI");
            auto evList = BuildUpcomingAstroEvents(snap);
            for (size_t i = 0; i < evList.size(); ++i) {
                addKv(L"Astro Event #" + std::to_wstring(i + 1), evList[i], L"Calendar");
            }
        }
        return;
    }

    std::vector<float> fr = {0.15f, 0.18f, 0.16f, 0.24f, 0.14f, 0.13f};
    drawRowCells(fr, {isDays ? L"Day" : L"Time", L"Temp", L"Precip", L"Wind", L"Press", L"AQI"}, true, false);

    if (!isDays) {
        for (size_t i = 0; i < snap.hourly.size(); ++i) {
            if (y + rowH > box.bottom - 4.0f) break;
            const auto& h = snap.hourly[i];
            wchar_t aBuf[16] = {};
            swprintf_s(aBuf, L"%.0f", h.aqiValue);
            drawRowCells(fr, {
                h.timeLabel,
                FormatTempValue(h.tempC, cfg.tempUnit, true),
                FormatPrecipValue(h.precipMm, cfg.precipUnit),
                FormatWindValue(h.windSpeedKmh, cfg.windUnit) + L" " + WindDegToCompass16(h.windDirDeg),
                FormatPressureCompact(h.pressureHpa, cfg.pressureUnit),
                aBuf
            }, false, (i % 2 == 1));
        }
    } else {
        for (size_t i = 0; i < snap.daily.size(); ++i) {
            if (y + rowH > box.bottom - 4.0f) break;
            const auto& d = snap.daily[i];
            wchar_t aBuf[16] = {};
            swprintf_s(aBuf, L"%.0f", d.aqiValue);
            drawRowCells(fr, {
                d.dayLabel,
                FormatTempValue(d.tempMinC, cfg.tempUnit, true) + L".." + FormatTempValue(d.tempMaxC, cfg.tempUnit, true),
                FormatPrecipValue(d.precipMm, cfg.precipUnit),
                FormatWindValue(d.windSpeedKmh, cfg.windUnit) + L" " + WindDegToCompass16(d.windDirDeg),
                FormatPressureCompact(d.pressureHpa, cfg.pressureUnit),
                aBuf
            }, false, (i % 2 == 1));
        }
    }
}

void DrawFullSourcesOverlay(ID2D1DCRenderTarget* rt, ID2D1SolidColorBrush* brush,
                            const Settings& cfg, const ForecastSnapshot& snap,
                            D2D1_RECT_F box, float lineH) {
    brush->SetOpacity(1.0f);
    brush->SetColor(D2D1::ColorF(0.10f, 0.15f, 0.26f, 0.94f));
    rt->FillRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush);
    brush->SetColor(D2D1::ColorF(0.45f, 0.78f, 1.0f, 0.55f));
    rt->DrawRoundedRectangle(D2D1::RoundedRect(box, 10.0f, 10.0f), brush, 1.2f);

    float rowH = std::max(18.0f, lineH + 1.5f);
    std::wstring hdr = L"DATA SOURCES & SYNC DIAGNOSTICS (click to return)";
    brush->SetColor(D2D1::ColorF(0.55f, 0.84f, 1.0f, 0.98f));
    rt->DrawTextW(hdr.c_str(), static_cast<UINT32>(hdr.size()), g_hudTitleFormat,
                  D2D1::RectF(box.left + 12.0f, box.top + 8.0f, box.right - 12.0f, box.top + 8.0f + rowH),
                  brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    float tblL = box.left + 12.0f;
    float tblW = std::max(100.0f, (box.right - 12.0f) - tblL);
    float y = box.top + 12.0f + rowH;

    auto drawSrcRow = [&](const std::wstring& metric, const MetricMeta& m, bool isHdr, int idx) {
        if (y + rowH > box.bottom - 6.0f) return;
        if (!isHdr && (idx % 2 == 1)) {
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.04f));
            rt->FillRectangle(D2D1::RectF(tblL - 4.0f, y, tblL + tblW + 4.0f, y + rowH), brush);
        }
        float c1 = tblW * 0.36f;
        float c2 = tblW * 0.40f;
        float c3 = tblW * 0.24f;
        std::wstring srcTxt = isHdr ? L"Source" : ((snap.valid && m.available && !m.sourceName.empty()) ? m.sourceName : L"unavailable");
        std::wstring timTxt = isHdr ? L"Synced" : ((snap.valid && m.available && !m.syncTime.empty()) ? m.syncTime : L"--:--");

        brush->SetColor(isHdr ? D2D1::ColorF(0.62f, 0.82f, 1.0f, 0.95f) : cfg.colorHudText);
        IDWriteTextFormat* fmt = isHdr ? g_hudTitleFormat : ((snap.valid && m.available) ? g_hudBodyFormat : g_hudItalicFormat);
        if (!isHdr && (!snap.valid || !m.available)) {
            brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.50f));
        }

        rt->DrawTextW(metric.c_str(), static_cast<UINT32>(metric.size()), fmt, D2D1::RectF(tblL, y, tblL + c1 - 4.0f, y + rowH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        rt->DrawTextW(srcTxt.c_str(), static_cast<UINT32>(srcTxt.size()), fmt, D2D1::RectF(tblL + c1, y, tblL + c1 + c2 - 4.0f, y + rowH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        rt->DrawTextW(timTxt.c_str(), static_cast<UINT32>(timTxt.size()), fmt, D2D1::RectF(tblL + c1 + c2, y, tblL + c1 + c2 + c3, y + rowH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        y += rowH;
        if (isHdr) {
            brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f));
            rt->DrawLine(D2D1::Point2F(tblL, y - 1.0f), D2D1::Point2F(tblL + tblW, y - 1.0f), brush, 1.0f);
            y += 2.0f;
        }
    };

    MetricMeta dummy;
    drawSrcRow(L"Metric", dummy, true, 0);
    int r = 0;
    drawSrcRow(L"Temperature", snap.mTemp, false, r++);
    drawSrcRow(L"Feels Like", snap.mFeelsLike, false, r++);
    drawSrcRow(L"Condition", snap.mCondition, false, r++);
    drawSrcRow(L"Wind & Gusts", snap.mWind, false, r++);
    drawSrcRow(L"Clouds", snap.mClouds, false, r++);
    drawSrcRow(L"Humidity", snap.mHumidity, false, r++);
    drawSrcRow(L"Precipitation", snap.mPrecip, false, r++);
    drawSrcRow(L"Pressure", snap.mPressure, false, r++);
    drawSrcRow(L"Visibility", snap.mVisibility, false, r++);
    drawSrcRow(L"Dew Point", snap.mDewPoint, false, r++);
    drawSrcRow(L"Air Quality (AQI)", snap.mAqi, false, r++);
    drawSrcRow(L"Pollen", snap.mPollen, false, r++);
    drawSrcRow(L"Sunrise / Sunset", snap.mSunTimes, false, r++);
    drawSrcRow(L"Golden Hour", snap.mGoldenHour, false, r++);
    drawSrcRow(L"Moonrise / Moonset", snap.mMoonTimes, false, r++);
    drawSrcRow(L"UV Index", snap.mUv, false, r++);
    drawSrcRow(L"EMI / Solar Flares", snap.mSpaceWeather, false, r++);
    drawSrcRow(L"Near-Earth NEO", snap.mNeo, false, r++);
    drawSrcRow(L"Hourly Series", snap.mHourly, false, r++);
    drawSrcRow(L"Daily Series", snap.mDaily, false, r++);
}

struct HudLayoutMetrics {
    bool showButton = false;
    bool expanded = false;
    float btnCx = 0.0f;
    float btnCy = 0.0f;
    float btnR = 0.0f;
    float cardL = 0.0f;
    float cardT = 0.0f;
    float cardW = 0.0f;
    float cardH = 0.0f;
    float totalContentH = 0.0f;
    bool needsScroll = false;
    int maxScroll = 0;
    int scrollPx = 0;
    int winLeft = 0;
    int winTop = 0;
    int winW = 0;
    int winH = 0;
    float baseF = 12.0f;
    float fontScale = 1.0f;
    float padX = 12.0f;
    float padY = 12.0f;
    float topBarH = 30.0f;
    float tabBarH = 25.0f;
    float subBarH = 38.0f;
    float lineH = 18.0f;
    float heroH = 126.0f;
    float row2H = 168.0f;
    float row3H = 154.0f;
    float bodyAreaH = 468.0f;
    float astroH = 149.0f;
    float fullBelowTabsH = 516.0f;
    float fixedCardH = 600.0f;
    bool isDragging = false;
    bool settingsView = false;
    bool sourcesView = false;
    HudCategory activeCat = HudCategory::Meteo;
    HudDataTab activeTab = HudDataTab::Now;
    ExpandedSection expSec = ExpandedSection::None;
    DetailWidgetKind nowDet = DetailWidgetKind::None;
    DetailWidgetKind hoursDet = DetailWidgetKind::None;
    DetailWidgetKind daysDet = DetailWidgetKind::None;
    DetailWidgetKind astroDet = DetailWidgetKind::None;
    int hoursPage = 0;
    int daysPage = 0;
    int setMeteoPage = 0;
    int setAstroPage = 0;
};

HudLayoutMetrics ComputeHudLayout(const Settings& cfg, const ForecastSnapshot& snap, int bottomLimitY) {
    HudLayoutMetrics m;
    float dpi = g_primaryDpiScale;
    m.showButton = (cfg.hudWidgetMode != HudWidgetMode::FullScreenOnly);
    m.btnR = m.showButton ? (cfg.hudButtonRadiusPx * dpi) : 0.0f;
    m.expanded = m.showButton && g_hudExpanded.load();

    const MonitorCanvasRect& primaryMon = g_monitorRects.front();
    {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        m.isDragging = g_draggingHudBtn;
        m.settingsView = g_hudSettingsView;
        m.sourcesView = g_hudSourcesView;
        m.activeCat = g_hudCategory;
        m.activeTab = g_hudDataTab;
        m.expSec = g_expandedSection;
        m.nowDet = g_nowDetail;
        m.hoursDet = g_hoursDetail;
        m.daysDet = g_daysDetail;
        m.astroDet = g_astroDetail;
        m.hoursPage = g_hoursPage;
        m.daysPage = g_daysPage;
        m.setMeteoPage = g_settingsMeteoPage;
        m.setAstroPage = g_settingsAstroPage;
        if (m.isDragging) {
            m.btnCx = g_hudBtnCx.load();
            m.btnCy = g_hudBtnCy.load();
        } else if (cfg.hasCustomBtnPos) {
            m.btnCx = static_cast<float>(cfg.hudBtnRelX * static_cast<double>(g_canvasWidth));
            m.btnCy = static_cast<float>(cfg.hudBtnRelY * static_cast<double>(g_canvasHeight));
        } else {
            m.btnCx = primaryMon.left + primaryMon.width * (static_cast<float>(cfg.hudButtonGeom.centerX) / 100.0f);
            m.btnCy = primaryMon.top + primaryMon.height * (1.0f - static_cast<float>(cfg.hudButtonGeom.centerY) / 100.0f);
        }
    }

    float bottomRelY = static_cast<float>(bottomLimitY - g_canvasTop);
    m.btnCx = std::clamp(m.btnCx, primaryMon.left + m.btnR + 6.0f, primaryMon.left + primaryMon.width - m.btnR - 6.0f);
    m.btnCy = std::clamp(m.btnCy, primaryMon.top + m.btnR + 6.0f, primaryMon.top + bottomRelY - m.btnR - 6.0f);
    g_hudBtnCx.store(m.btnCx);
    g_hudBtnCy.store(m.btnCy);
    g_hudBtnR.store(m.btnR);

    float minX = m.btnCx - m.btnR - 6.0f;
    float minY = m.btnCy - m.btnR - 6.0f;
    float maxX = m.btnCx + m.btnR + 6.0f;
    float maxY = m.btnCy + m.btnR + 6.0f;

    if (m.expanded) {
        m.baseF = cfg.hudFontSizePx * dpi;
        m.fontScale = std::max(1.0f, m.baseF / 12.0f);
        m.padX = 12.0f * dpi;
        m.padY = 12.0f * dpi;
        float gap = static_cast<float>(cfg.forecastHudMarginPx) * dpi;
        m.cardW = std::clamp((580.0f + (cfg.hudFontSizePx - 12.0f) * 14.0f) * dpi, 560.0f * dpi, std::min(760.0f * dpi, primaryMon.width - gap * 2.0f));
        m.topBarH = std::max(30.0f * dpi, m.baseF + 16.0f * dpi);
        m.tabBarH = std::max(25.0f * dpi, m.baseF + 11.0f * dpi);
        m.subBarH = std::max(38.0f * dpi, 40.0f * m.fontScale);
        m.lineH = std::max(18.0f * dpi, m.baseF + 6.0f * dpi);

        m.heroH = std::max(126.0f * dpi, m.baseF * 3.15f + m.lineH * 3.4f);
        m.row2H = std::max(168.0f * dpi, m.baseF * 2.0f + m.lineH * 5.3f);
        m.row3H = std::max(154.0f * dpi, m.baseF * 2.0f + m.lineH * 4.9f);
        m.bodyAreaH = m.heroH + 10.0f + m.row2H + 10.0f + m.row3H;
        m.astroH = (m.bodyAreaH - 20.0f) / 3.0f;
        m.fullBelowTabsH = m.subBarH + 10.0f + m.bodyAreaH;
        m.fixedCardH = m.padY * 2.0f + m.topBarH + 8.0f + m.tabBarH + 8.0f + m.fullBelowTabsH;
        m.totalContentH = m.fixedCardH;

        if (m.settingsView) {
            float srcSectionH = (m.expSec == ExpandedSection::None)
                ? (8.0f * (m.lineH + 4.0f) + 6.0f * (m.lineH + 2.0f) + 22.0f)
                : 0.0f;
            int sRows = 4;
            if (m.expSec == ExpandedSection::Sky) sRows += 6;
            if (m.expSec == ExpandedSection::Precipitation) sRows += 4;
            if (m.expSec == ExpandedSection::Effects) sRows += 10;
            if (m.expSec == ExpandedSection::Performance) sRows += (cfg.performanceMode == PerformanceMode::Custom) ? 5 : 1;
            float settingsContentH = m.padY * 2.0f + m.topBarH + 8.0f + m.tabBarH + 10.0f + srcSectionH + sRows * (m.lineH + 11.0f) + 12.0f;
            m.totalContentH = std::max(m.fixedCardH, settingsContentH);
        } else if (m.sourcesView) {
            float srcH = std::max(m.fullBelowTabsH, 22.0f * std::max(18.0f, m.lineH + 1.5f) + 20.0f);
            m.totalContentH = std::max(m.fixedCardH, m.padY * 2.0f + m.topBarH + 8.0f + m.tabBarH + 8.0f + srcH);
        } else if (m.activeCat == HudCategory::Meteo && m.activeTab == HudDataTab::Hours && m.hoursDet != DetailWidgetKind::None) {
            float detH = std::max(m.bodyAreaH, 26.0f * std::max(18.0f, m.lineH + 1.5f) + 16.0f);
            m.totalContentH = std::max(m.fixedCardH, m.padY * 2.0f + m.topBarH + 8.0f + m.tabBarH + 8.0f + m.subBarH + 10.0f + detH);
        } else if (m.activeCat == HudCategory::Meteo && m.activeTab == HudDataTab::Days && m.daysDet != DetailWidgetKind::None) {
            int dRows = std::max(8, static_cast<int>(snap.daily.size()) + 2);
            float detH = std::max(m.bodyAreaH, static_cast<float>(dRows) * std::max(18.0f, m.lineH + 1.5f) + 16.0f);
            m.totalContentH = std::max(m.fixedCardH, m.padY * 2.0f + m.topBarH + 8.0f + m.tabBarH + 8.0f + m.subBarH + 10.0f + detH);
        }

        float availH = std::max(160.0f, bottomRelY - gap * 2.0f);
        m.cardH = std::min(m.fixedCardH, availH);
        m.needsScroll = m.totalContentH > m.cardH + 0.5f;

        float spaceRight = (primaryMon.left + primaryMon.width - gap) - (m.btnCx + m.btnR + gap);
        float spaceLeft = (m.btnCx - m.btnR - gap) - (primaryMon.left + gap);
        m.cardL = (spaceRight >= m.cardW)
            ? (m.btnCx + m.btnR + gap)
            : ((spaceLeft >= m.cardW)
                ? (m.btnCx - m.btnR - gap - m.cardW)
                : std::clamp(m.btnCx - m.cardW * 0.5f, primaryMon.left + gap, primaryMon.left + primaryMon.width - m.cardW - gap));

        float spaceTop = (m.btnCy - m.btnR - gap) - (primaryMon.top + gap);
        float spaceBottom = (bottomRelY - gap) - (m.btnCy + m.btnR + gap);
        m.cardT = (spaceTop >= m.cardH)
            ? (m.btnCy - m.btnR - gap - m.cardH)
            : ((spaceBottom >= m.cardH)
                ? (m.btnCy + m.btnR + gap)
                : std::clamp(m.btnCy - m.cardH * 0.5f, primaryMon.top + gap, bottomRelY - m.cardH - gap));

        m.maxScroll = m.needsScroll ? static_cast<int>(std::ceil(m.totalContentH - m.cardH)) : 0;
        {
            std::lock_guard<std::mutex> lock(g_hudUiMutex);
            g_hudMaxScrollPx = m.maxScroll;
            g_hudScrollPx = m.needsScroll ? std::clamp(g_hudScrollPx, 0, m.maxScroll) : 0;
            m.scrollPx = g_hudScrollPx;
        }

        minX = std::min(minX, m.cardL - 4.0f);
        minY = std::min(minY, m.cardT - 4.0f);
        maxX = std::max(maxX, m.cardL + m.cardW + 4.0f);
        maxY = std::max(maxY, m.cardT + m.cardH + 4.0f);
    }

    int relL = std::max(0, static_cast<int>(std::floor(minX)));
    int relT = std::max(0, static_cast<int>(std::floor(minY)));
    int relR = std::min(g_canvasWidth, static_cast<int>(std::ceil(maxX)));
    int relB = std::min(g_canvasHeight, static_cast<int>(std::ceil(maxY)));
    m.winLeft = g_canvasLeft + relL;
    m.winTop = g_canvasTop + relT;
    m.winW = std::max(16, relR - relL);
    m.winH = std::max(16, relB - relT);
    return m;
}

void DrawForecastHudWidget(CachedSurface& surf, const Settings& cfg, const ForecastSnapshot& snap,
                           const GeneralEffectsState& fx, float cloudPct01, double moonPhase01,
                           bool sunOn, bool moonOn, double hour, const HudLayoutMetrics& m, double timeSec) {
    ID2D1DCRenderTarget* rt = surf.rt;
    ID2D1SolidColorBrush* brush = surf.sharedBrush;
    if (!rt || !brush || !m.showButton) return;

    bool showGearIcon = (!m.expanded && !snap.valid);
    std::wstring labelText;
    if (m.expanded) labelText = L"×";
    else if (snap.valid) labelText = FormatTempValue(snap.tempC, cfg.tempUnit, true);

    int glyphCount = std::max(1, static_cast<int>(labelText.size()));
    float maxByHeight = m.btnR * 0.88f;
    float maxByWidth = (m.btnR * 1.55f) / static_cast<float>(glyphCount);
    float circleFontPx = std::clamp(std::min(maxByHeight, maxByWidth), 8.0f, 48.0f);

    if (!EnsureDirectWrite(cfg.hudFontSizePx * g_primaryDpiScale, circleFontPx)) return;

    std::vector<InteractiveControl> nextControls;

    if (m.expanded) {
        float left = m.cardL;
        float top = m.cardT;
        float cardW = m.cardW;
        float cardH = m.cardH;
        float lineH = m.lineH;

        D2D1_ROUNDED_RECT outerRr = D2D1::RoundedRect(D2D1::RectF(left, top, left + cardW, top + cardH), 12.0f, 12.0f);
        brush->SetOpacity(1.0f);
        brush->SetColor(cfg.colorHudBg);
        rt->FillRoundedRectangle(outerRr, brush);
        D2D1_COLOR_F borderCol = cfg.colorHudText;
        borderCol.a = 0.28f;
        brush->SetColor(borderCol);
        rt->DrawRoundedRectangle(outerRr, brush, 1.0f);

        float innerLeft = left + m.padX;
        float innerRight = left + cardW - m.padX - (m.needsScroll ? 8.0f : 0.0f);
        float innerW = innerRight - innerLeft;

        rt->PushAxisAlignedClip(D2D1::RectF(left + 2.0f, top + 3.0f, left + cardW - 2.0f, top + cardH - 3.0f),
                               D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        float curY = top + m.padY - static_cast<float>(m.scrollPx);
        float btnCursorX = innerRight;
        float dpi = g_primaryDpiScale;

        D2D1_RECT_F gearRect = D2D1::RectF(btnCursorX - 28.0f * dpi, curY + 2.0f, btnCursorX, curY + m.topBarH - 2.0f);
        brush->SetColor(m.settingsView
            ? D2D1::ColorF(0.28f, 0.78f, 1.0f, 0.34f)
            : D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.12f));
        rt->FillRoundedRectangle(D2D1::RoundedRect(gearRect, 6.0f, 6.0f), brush);
        DrawGearSettingsIcon(rt, brush, (gearRect.left + gearRect.right) * 0.5f, (gearRect.top + gearRect.bottom) * 0.5f, 19.0f * dpi, cfg.colorHudText);
        nextControls.push_back(InteractiveControl{gearRect, L"toggle_hud_settings_view", L"", D2D1::ColorF(0, 0, 0, 0), L""});
        btnCursorX -= 33.0f * dpi;

        D2D1_RECT_F resetRect = D2D1::RectF(btnCursorX - 56.0f * dpi, curY + 2.0f, btnCursorX, curY + m.topBarH - 2.0f);
        InteractiveControl icReset{resetRect, L"forecast_reset", L"RESET", D2D1::ColorF(0.86f, 0.38f, 0.38f, 0.92f), L""};
        DrawRoundedPillButton(rt, brush, cfg, icReset);
        nextControls.push_back(icReset);
        btnCursorX -= 61.0f * dpi;

        D2D1_RECT_F syncRect = D2D1::RectF(btnCursorX - 52.0f * dpi, curY + 2.0f, btnCursorX, curY + m.topBarH - 2.0f);
        InteractiveControl icSync{syncRect, L"forecast_refresh", L"SYNC", ComputeProviderStatusTint(snap), L""};
        DrawRoundedPillButton(rt, brush, cfg, icSync);
        nextControls.push_back(icSync);
        btnCursorX -= 57.0f * dpi;

        std::wstring coordTxt = cfg.coordsRaw.empty() ? L"No coords" : cfg.coordsRaw;
        std::wstring svcLeft = coordTxt + L" • " + (snap.valid ? snap.lastUpdateText : L"--:--");
        brush->SetColor(cfg.colorHudText);
        rt->DrawTextW(svcLeft.c_str(), static_cast<UINT32>(svcLeft.size()), g_hudBodyFormat,
                      D2D1::RectF(innerLeft, curY + 5.0f, btnCursorX, curY + m.topBarH),
                      brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

        curY += m.topBarH + 8.0f;

        float catW = (innerW - 8.0f) * 0.5f;
        DrawModeTabButton(rt, brush, cfg, D2D1::RectF(innerLeft, curY, innerLeft + catW, curY + m.tabBarH),
                          L"Meteo", L"cat_meteo", (!m.settingsView && !m.sourcesView && m.activeCat == HudCategory::Meteo), nextControls);
        DrawModeTabButton(rt, brush, cfg, D2D1::RectF(innerLeft + catW + 8.0f, curY, innerRight, curY + m.tabBarH),
                          L"Astro", L"cat_astro", (!m.settingsView && !m.sourcesView && m.activeCat == HudCategory::Astro), nextControls);

        curY += m.tabBarH + 8.0f;

        if (m.settingsView) {
            if (m.expSec == ExpandedSection::None) {
                float colGap = 10.0f;
                float colW = (innerW - colGap) * 0.5f;
                float colLeftX[2] = {innerLeft, innerLeft + colW + colGap};

                const std::vector<std::wstring> meteoList = {
                    L"open_meteo", L"met_norway", L"seven_timer", L"weatherapi",
                    L"openweathermap", L"visual_crossing", L"tomorrow_io", L"custom_api"
                };
                const std::vector<std::wstring> astroList = {
                    L"sunrise_sunset_io", L"open_meteo", L"weatherapi",
                    L"nasa_donki", L"nasa_neows", L"visual_crossing", L"custom_api", L"local_file"
                };

                std::wstring pMain, pSub, pNorm;
                FormatPressureSplit(snap.pressureHpa, cfg.pressureUnit, pMain, pSub, pNorm);
                std::wstring aMain, aSub;
                FormatAqiSplit(snap.aqiValue, snap.pm25Value, cfg.aqiUnit, aMain, aSub);
                bool fogHigh = (snap.valid && snap.mTemp.available && snap.mDewPoint.available && (snap.tempC - snap.dewPointC <= 2.0));
                std::wstring fogTxt = (snap.valid && snap.mTemp.available && snap.mDewPoint.available)
                    ? (fogHigh ? L"High" : ((snap.tempC - snap.dewPointC <= 5.0) ? L"Med" : L"Low"))
                    : L"--";

                using ParamRow = std::pair<std::wstring, std::pair<std::wstring, bool>>;
                std::vector<ParamRow> meteoPages[3] = {
                    {
                        {L"Temp now",   {snap.valid && snap.mTemp.available ? FormatTempValue(snap.tempC, cfg.tempUnit, false) : L"--", snap.valid && snap.mTemp.available}},
                        {L"Wind speed", {snap.valid && snap.mWind.available ? FormatWindValue(snap.windSpeedKmh, cfg.windUnit) : L"--", snap.valid && snap.mWind.available}},
                        {L"Wind dir",   {snap.valid && snap.mWind.available ? WindDegToCompass16(snap.windDirDeg) : L"--", snap.valid && snap.mWind.available}},
                        {L"Pressure",   {snap.valid && snap.mPressure.available ? (pMain + L" " + pSub) : L"--", snap.valid && snap.mPressure.available}},
                        {L"Humidity",   {snap.valid && snap.mHumidity.available ? FormatHumidityValue(snap.humidityPct, cfg.humidityUnit) : L"--", snap.valid && snap.mHumidity.available}}
                    },
                    {
                        {L"Max temp",   {snap.valid && snap.hasTodayMinMax ? FormatTempValue(snap.todayMaxC, cfg.tempUnit, false) : L"--", snap.valid && snap.hasTodayMinMax}},
                        {L"Min temp",   {snap.valid && snap.hasTodayMinMax ? FormatTempValue(snap.todayMinC, cfg.tempUnit, false) : L"--", snap.valid && snap.hasTodayMinMax}},
                        {L"Clouds",     {snap.valid && snap.mClouds.available ? FormatCloudValue(snap.cloudCoverPct, cfg.cloudUnit) : L"--", snap.valid && snap.mClouds.available}},
                        {L"Precip",     {snap.valid && snap.mPrecip.available ? FormatPrecipValue(snap.precipMm, cfg.precipUnit) : L"--", snap.valid && snap.mPrecip.available}},
                        {L"Wind gusts", {snap.valid && snap.mGusts.available ? FormatWindValue(snap.windGustsKmh, cfg.windUnit) : L"--", snap.valid && snap.mGusts.available}}
                    },
                    {
                        {L"Fog",        {fogTxt, snap.valid && snap.mTemp.available && snap.mDewPoint.available}},
                        {L"Visibility", {snap.valid && snap.mVisibility.available ? FormatVisibilityValue(snap.visibilityM, cfg.visibilityUnit) : L"--", snap.valid && snap.mVisibility.available}},
                        {L"Dew point",  {snap.valid && snap.mDewPoint.available ? FormatTempValue(snap.dewPointC, cfg.tempUnit, false) : L"--", snap.valid && snap.mDewPoint.available}},
                        {L"Air quality",{snap.valid && snap.mAqi.available ? (aMain + L" " + aSub) : L"--", snap.valid && snap.mAqi.available}},
                        {L"Condition",  {snap.valid && snap.mCondition.available ? WmoCodeToLabel(snap.weatherCode) : L"--", snap.valid && snap.mCondition.available}}
                    }
                };

                double effSr = cfg.customSunriseHour, effSs = cfg.customSunsetHour;
                double effMr = cfg.customMoonriseHour, effMs = cfg.customMoonsetHour;
                bool dummyWin = false;
                ResolveCelestialWindow(cfg, snap, true, hour, effSr, effSs, dummyWin);
                ResolveCelestialWindow(cfg, snap, false, hour, effMr, effMs, dummyWin);
                double dayLenH = effSs - effSr; if (dayLenH < 0.0) dayLenH += 24.0;
                double altDeg = 0.0, azDeg = 0.0;
                std::wstring nextNewD, nextFullD;
                ComputeLunarEphemerisDisplay(moonPhase01, hour, effMr, effMs, cfg.latitude, altDeg, azDeg, nextNewD, nextFullD);

                wchar_t kpBuf[24] = {}, illumBuf[24] = {}, altBuf[24] = {}, azBuf[32] = {};
                swprintf_s(kpBuf, L"Kp %.1f", snap.flareKpEst);
                swprintf_s(illumBuf, L"%.0f%%", 50.0 * (1.0 - std::cos(moonPhase01 * 2.0 * kPi)));
                swprintf_s(altBuf, L"%.0f°", altDeg);
                swprintf_s(azBuf, L"%.0f° %s", azDeg, WindDegToCompass16(azDeg));

                std::vector<ParamRow> astroPages[3] = {
                    {
                        {L"Sunrise",    {FormatHourHhMm(effSr), snap.valid && snap.hasSunTimes}},
                        {L"Sunset",     {FormatHourHhMm(effSs), snap.valid && snap.hasSunTimes}},
                        {L"UV index",   {snap.valid && snap.mUv.available ? FormatUvValue(snap.uvIndex, cfg.uvUnit) : L"--", snap.valid && snap.mUv.available}},
                        {L"EMI index",  {kpBuf, snap.hasSpaceWeather}},
                        {L"Moon phase", {MoonPhaseToName(moonPhase01), true}}
                    },
                    {
                        {L"Moonrise",   {FormatHourHhMm(effMr), snap.valid && snap.hasMoonTimes}},
                        {L"Moonset",    {FormatHourHhMm(effMs), snap.valid && snap.hasMoonTimes}},
                        {L"Moon illum", {illumBuf, true}},
                        {L"Moon alt",   {altBuf, true}},
                        {L"Moon az",    {azBuf, true}}
                    },
                    {
                        {L"Golden start",{FormatHourHhMm(effSs - 0.75), snap.hasGoldenHour || snap.hasSunTimes}},
                        {L"Golden end",  {FormatHourHhMm(effSs), snap.hasGoldenHour || snap.hasSunTimes}},
                        {L"Blue start",  {FormatHourHhMm(effSs), snap.hasSunTimes}},
                        {L"Blue end",    {FormatHourHhMm(effSs + 0.45), snap.hasSunTimes}},
                        {L"Daylight",    {FormatDurationHhMm(dayLenH), snap.hasSunTimes}}
                    }
                };

                float colStartY = curY + 2.0f;
                float maxColEndY = colStartY;

                for (int cIdx = 0; cIdx < 2; ++cIdx) {
                    bool isAstroCol = (cIdx == 1);
                    float cx0 = colLeftX[cIdx];
                    float cx1 = cx0 + colW;
                    float cy = colStartY;

                    const auto& srcList = isAstroCol ? astroList : meteoList;
                    const std::wstring& selectedSrc = isAstroCol ? cfg.astroPrimarySource : cfg.primarySource;
                    int pIdx = std::clamp(isAstroCol ? m.setAstroPage : m.setMeteoPage, 0, 2);

                    for (const auto& sId : srcList) {
                        bool isSel = (selectedSrc == sId);
                        D2D1_RECT_F sRect = D2D1::RectF(cx0, cy, cx1, cy + lineH + 2.0f);
                        brush->SetColor(isSel ? D2D1::ColorF(0.28f, 0.72f, 1.0f, 0.28f) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.04f));
                        rt->FillRoundedRectangle(D2D1::RoundedRect(sRect, 5.0f, 5.0f), brush);
                        if (isSel) {
                            brush->SetColor(D2D1::ColorF(0.45f, 0.82f, 1.0f, 0.90f));
                            rt->DrawRoundedRectangle(D2D1::RoundedRect(sRect, 5.0f, 5.0f), brush, 1.1f);
                        }
                        std::wstring lbl = (isSel ? L"★ " : L"  ") + FormatProviderHudBadge(sId);
                        brush->SetColor(isSel ? cfg.colorHudText : D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.75f));
                        rt->DrawTextW(lbl.c_str(), static_cast<UINT32>(lbl.size()), g_hudBodyFormat,
                                      D2D1::RectF(sRect.left + 6.0f, sRect.top + 1.0f, sRect.right - 4.0f, sRect.bottom), brush);
                        nextControls.push_back(InteractiveControl{sRect, (isAstroCol ? L"set_astro_src_" : L"set_meteo_src_") + sId, L"", D2D1::ColorF(0, 0, 0, 0), L""});
                        cy += lineH + 4.0f;
                    }

                    cy += 6.0f;
                    D2D1_RECT_F prevBtn = D2D1::RectF(cx0, cy, cx0 + 24.0f * dpi, cy + lineH);
                    D2D1_RECT_F nextBtn = D2D1::RectF(cx1 - 24.0f * dpi, cy, cx1, cy + lineH);
                    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f));
                    rt->FillRoundedRectangle(D2D1::RoundedRect(prevBtn, 4.0f, 4.0f), brush);
                    rt->FillRoundedRectangle(D2D1::RoundedRect(nextBtn, 4.0f, 4.0f), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(L"<", 1, g_hudPillFormat, prevBtn, brush);
                    rt->DrawTextW(L">", 1, g_hudPillFormat, nextBtn, brush);
                    nextControls.push_back(InteractiveControl{prevBtn, isAstroCol ? L"settings_astro_prev" : L"settings_meteo_prev", L"<", D2D1::ColorF(0, 0, 0, 0), L""});
                    nextControls.push_back(InteractiveControl{nextBtn, isAstroCol ? L"settings_astro_next" : L"settings_meteo_next", L">", D2D1::ColorF(0, 0, 0, 0), L""});

                    wchar_t pgLbl[32] = {};
                    swprintf_s(pgLbl, L"Params %d/3", pIdx + 1);
                    brush->SetColor(D2D1::ColorF(0.62f, 0.82f, 1.0f, 0.90f));
                    rt->DrawTextW(pgLbl, static_cast<UINT32>(wcslen(pgLbl)), g_hudPillFormat,
                                  D2D1::RectF(prevBtn.right + 4.0f, cy, nextBtn.left - 4.0f, cy + lineH), brush);
                    cy += lineH + 4.0f;

                    const auto& tRows = isAstroCol ? astroPages[pIdx] : meteoPages[pIdx];
                    float midSplit = cx0 + (cx1 - cx0) * 0.48f;
                    for (const auto& tr : tRows) {
                        bool avail = tr.second.second;
                        IDWriteTextFormat* fmt = avail ? g_hudBodyFormat : g_hudItalicFormat;
                        float op = avail ? 1.0f : 0.50f;
                        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, op * 0.82f));
                        fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                        rt->DrawTextW(tr.first.c_str(), static_cast<UINT32>(tr.first.size()), fmt,
                                      D2D1::RectF(cx0 + 4.0f, cy, midSplit - 2.0f, cy + lineH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

                        brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, op));
                        fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                        rt->DrawTextW(tr.second.first.c_str(), static_cast<UINT32>(tr.second.first.size()), fmt,
                                      D2D1::RectF(midSplit, cy, cx1 - 4.0f, cy + lineH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                        fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                        cy += lineH + 1.5f;
                    }
                    if (cy > maxColEndY) maxColEndY = cy;
                }
                curY = maxColEndY + 12.0f;
            }

            auto triBadge = [](TriStateMode tm) -> std::pair<std::wstring, D2D1_COLOR_F> {
                if (tm == TriStateMode::On) return {L"ON", D2D1::ColorF(0.25f, 0.85f, 0.48f, 0.9f)};
                if (tm == TriStateMode::Off) return {L"OFF", D2D1::ColorF(0.85f, 0.35f, 0.35f, 0.9f)};
                return {L"AUTO", D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f)};
            };
            auto tierBadge = [](QualityTier t) -> std::pair<std::wstring, D2D1_COLOR_F> {
                if (t == QualityTier::Low) return {L"LOW", D2D1::ColorF(0.65f, 0.72f, 0.80f, 0.9f)};
                if (t == QualityTier::Medium) return {L"MEDIUM", D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f)};
                return {L"HIGH", D2D1::ColorF(0.25f, 0.85f, 0.55f, 0.9f)};
            };
            auto addSetRow = [&](const std::wstring& lbl, const std::wstring& actId, const std::wstring& bTxt, D2D1_COLOR_F tint) {
                D2D1_RECT_F rBox = D2D1::RectF(innerLeft, curY, innerRight, curY + lineH + 5.0f);
                brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.05f));
                rt->FillRoundedRectangle(D2D1::RoundedRect(rBox, 6.0f, 6.0f), brush);
                brush->SetColor(cfg.colorHudText);
                rt->DrawTextW(lbl.c_str(), static_cast<UINT32>(lbl.size()), g_hudBodyFormat,
                              D2D1::RectF(rBox.left + 8.0f, rBox.top + 2.0f, rBox.right - 108.0f * dpi, rBox.bottom - 2.0f), brush);
                InteractiveControl ic{D2D1::RectF(rBox.right - 100.0f * dpi, rBox.top + 2.0f, rBox.right - 6.0f, rBox.bottom - 2.0f), actId, bTxt, tint, L""};
                DrawRoundedPillButton(rt, brush, cfg, ic);
                nextControls.push_back(ic);
                curY += (rBox.bottom - rBox.top) + 6.0f;
            };

            addSetRow(L"Sky", L"sec_sky", m.expSec == ExpandedSection::Sky ? L"HIDE" : L"OPEN", D2D1::ColorF(0.58f, 0.63f, 0.70f, 0.90f));
            if (m.expSec == ExpandedSection::Sky) {
                auto cB = triBadge(cfg.cloudsMode);
                addSetRow(L"  Clouds", L"cycle_clouds", cB.first, cB.second);
                addSetRow(L"  Top cloud band", L"toggle_top_clouds", cfg.topCloudCoverEnabled ? L"ON" : L"OFF", D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f));
                addSetRow(L"  Ambient sky tint", L"toggle_sky_ambient_tint", cfg.skyAmbientTintEnabled ? L"ON" : L"OFF", D2D1::ColorF(0.45f, 0.72f, 1.0f, 0.9f));
                auto sB = triBadge(cfg.sunMode);
                addSetRow(L"  Sun", L"cycle_sun", sB.first, sB.second);
                auto mB = triBadge(cfg.moonMode);
                addSetRow(L"  Moon", L"cycle_moon", mB.first, mB.second);
                addSetRow(L"  Over clouds", L"toggle_celestial_front", cfg.celestialInFrontOfClouds ? L"FRONT" : L"DIM", D2D1::ColorF(0.65f, 0.78f, 0.95f, 0.9f));
            }

            addSetRow(L"Precipitation", L"sec_precip", m.expSec == ExpandedSection::Precipitation ? L"HIDE" : L"OPEN", D2D1::ColorF(0.58f, 0.63f, 0.70f, 0.90f));
            if (m.expSec == ExpandedSection::Precipitation) {
                std::wstring pTxt = ToLower(PrecipKindToLabel(PrecipModeToKind(cfg.precipitationMode)));
                if (cfg.precipitationMode == PrecipMode::Auto) pTxt = L"AUTO";
                else if (cfg.precipitationMode == PrecipMode::Off) pTxt = L"OFF";
                else for (auto& ch : pTxt) ch = static_cast<wchar_t>(towupper(ch));
                addSetRow(L"  Type", L"cycle_precip_mode", pTxt, D2D1::ColorF(0.35f, 0.72f, 1.0f, 0.9f));

                const wchar_t* dTxt = (cfg.precipFallDirection == FallDirMode::Auto) ? L"AUTO" : ((cfg.precipFallDirection == FallDirMode::Down) ? L"DOWN" : ((cfg.precipFallDirection == FallDirMode::DownLeft) ? L"LEFT" : L"RIGHT"));
                addSetRow(L"  Direction", L"cycle_precip_dir", dTxt, D2D1::ColorF(0.45f, 0.78f, 1.0f, 0.9f));
                addSetRow(L"  Speed mode", L"cycle_precip_speed", cfg.precipSpeedMode == PrecipSpeedMode::Auto ? L"AUTO" : L"CUSTOM", D2D1::ColorF(0.35f, 0.68f, 1.0f, 0.9f));
                addSetRow(L"  Window hit", L"toggle_window_bounce", cfg.windowBounceEnabled ? L"ON" : L"OFF", D2D1::ColorF(0.25f, 0.85f, 0.48f, 0.9f));
            }

            addSetRow(L"Atmospheric effects", L"sec_effects", m.expSec == ExpandedSection::Effects ? L"HIDE" : L"OPEN", D2D1::ColorF(0.58f, 0.63f, 0.70f, 0.90f));
            if (m.expSec == ExpandedSection::Effects) {
                auto addFxS = [&](const wchar_t* n, TriStateMode tm, const wchar_t* a) {
                    auto b = triBadge(tm);
                    addSetRow(n, a, b.first, b.second);
                };
                addFxS(L"  Dust", cfg.effectDust, L"cycle_fx_dust");
                addFxS(L"  Fog", cfg.effectFog, L"cycle_fx_fog");
                addFxS(L"  Smog", cfg.effectSmog, L"cycle_fx_smog");
                addFxS(L"  Thunderstorm", cfg.effectThunderstorm, L"cycle_fx_storm");
                addFxS(L"  Aurora", cfg.effectAurora, L"cycle_fx_aurora");
                addFxS(L"  Mirage", cfg.effectMirage, L"cycle_fx_mirage");
                addFxS(L"  Frost / Heat", cfg.effectThermal, L"cycle_fx_thermal");
                addFxS(L"  Night stars", cfg.effectNightStars, L"cycle_fx_stars");
                addFxS(L"  Air quality", cfg.effectAirQuality, L"cycle_fx_aq");
                addFxS(L"  Pollen", cfg.effectPollen, L"cycle_fx_pollen");
            }

            addSetRow(L"Performance", L"sec_perf", m.expSec == ExpandedSection::Performance ? L"HIDE" : L"OPEN", D2D1::ColorF(0.58f, 0.63f, 0.70f, 0.90f));
            if (m.expSec == ExpandedSection::Performance) {
                const wchar_t* perfStr = (cfg.performanceMode == PerformanceMode::High) ? L"HIGH" : ((cfg.performanceMode == PerformanceMode::Medium) ? L"MEDIUM" : ((cfg.performanceMode == PerformanceMode::Custom) ? L"CUSTOM" : L"LOW"));
                addSetRow(L"  Mode", L"cycle_perf", perfStr, D2D1::ColorF(0.30f, 0.85f, 0.65f, 0.9f));
                if (cfg.performanceMode == PerformanceMode::Custom) {
                    auto bC = tierBadge(cfg.perfClouds);
                    auto bS = tierBadge(cfg.perfCelestial);
                    auto bP = tierBadge(cfg.perfPrecip);
                    auto bE = tierBadge(cfg.perfEffects);
                    addSetRow(L"  Clouds quality", L"cycle_perf_clouds", bC.first, bC.second);
                    addSetRow(L"  Sun & Moon quality", L"cycle_perf_celestial", bS.first, bS.second);
                    addSetRow(L"  Precip quality", L"cycle_perf_precip", bP.first, bP.second);
                    addSetRow(L"  Effects quality", L"cycle_perf_effects", bE.first, bE.second);
                }
            }
        } else if (m.sourcesView) {
            float srcH = std::max(m.fullBelowTabsH, 22.0f * std::max(18.0f, lineH + 1.5f) + 20.0f);
            D2D1_RECT_F sBox = D2D1::RectF(innerLeft, curY, innerRight, curY + srcH);
            nextControls.push_back(InteractiveControl{sBox, L"toggle_sources_view", L"", D2D1::ColorF(0, 0, 0, 0), L""});
            DrawFullSourcesOverlay(rt, brush, cfg, snap, sBox, lineH);
        } else if (m.activeCat == HudCategory::Astro) {
            DrawAstroRiseSetBar(rt, brush, cfg, snap, hour, D2D1::RectF(innerLeft, curY, innerRight, curY + m.subBarH), lineH);
            curY += m.subBarH + 10.0f;

            if (m.astroDet != DetailWidgetKind::None) {
                D2D1_RECT_F detBox = D2D1::RectF(innerLeft, curY, innerRight, curY + m.bodyAreaH);
                const wchar_t* act = (m.astroDet == DetailWidgetKind::AstroSun)
                    ? L"toggle_astro_detail_sun"
                    : ((m.astroDet == DetailWidgetKind::AstroMoon) ? L"toggle_astro_detail_moon" : L"toggle_astro_detail_uvemi");
                nextControls.push_back(InteractiveControl{detBox, act, L"", D2D1::ColorF(0, 0, 0, 0), L""});
                DrawDetailTableOverlay(rt, brush, cfg, snap, false, m.astroDet, hour, moonPhase01, detBox, lineH);
            } else {
                wchar_t buf[128] = {};
                D2D1_RECT_F cSolar = D2D1::RectF(innerLeft, curY, innerRight, curY + m.astroH);
                nextControls.push_back(InteractiveControl{cSolar, L"toggle_astro_detail_sun", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                brush->SetColor(D2D1::ColorF(0.95f, 0.68f, 0.22f, 0.13f));
                rt->FillRoundedRectangle(D2D1::RoundedRect(cSolar, 10.0f, 10.0f), brush);
                brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f));
                rt->DrawRoundedRectangle(D2D1::RoundedRect(cSolar, 10.0f, 10.0f), brush, 1.0f);

                double effSunrise = cfg.customSunriseHour, effSunset = cfg.customSunsetHour;
                bool sWin = false;
                ResolveCelestialWindow(cfg, snap, true, hour, effSunrise, effSunset, sWin);

                float domeCx = (cSolar.left + cSolar.right) * 0.5f;
                float horizonY = cSolar.bottom - lineH * 1.9f;
                float domeR = std::min((innerW * 0.44f) * 0.5f, horizonY - cSolar.top - 12.0f);

                brush->SetColor(D2D1::ColorF(1.0f, 0.80f, 0.40f, 0.35f));
                rt->DrawLine(D2D1::Point2F(domeCx - domeR - 14.0f, horizonY), D2D1::Point2F(domeCx + domeR + 14.0f, horizonY), brush, 1.4f);

                if (g_d2dFactory && domeR > 16.0f) {
                    ID2D1PathGeometry* semiGeo = nullptr;
                    if (SUCCEEDED(g_d2dFactory->CreatePathGeometry(&semiGeo)) && semiGeo) {
                        ID2D1GeometrySink* sink = nullptr;
                        if (SUCCEEDED(semiGeo->Open(&sink)) && sink) {
                            sink->BeginFigure(D2D1::Point2F(domeCx - domeR, horizonY), D2D1_FIGURE_BEGIN_FILLED);
                            sink->AddArc(D2D1::ArcSegment(
                                D2D1::Point2F(domeCx + domeR, horizonY),
                                D2D1::SizeF(domeR, domeR), 0.0f,
                                D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                            sink->Close();
                            sink->Release();
                            brush->SetColor(D2D1::ColorF(1.0f, 0.78f, 0.32f, 0.14f));
                            rt->FillGeometry(semiGeo, brush);
                            ID2D1Layer* clipLayer = nullptr;
                            if (SUCCEEDED(rt->CreateLayer(nullptr, &clipLayer)) && clipLayer) {
                                rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), semiGeo), clipLayer);
                                DrawSunBodyHighQuality(rt, brush, domeCx - domeR * 0.72f, horizonY - domeR * 0.68f, domeR * 0.28f, cfg.colorSun, 0.90f, timeSec);
                                rt->PopLayer();
                                clipLayer->Release();
                            }
                        }
                        semiGeo->Release();
                    }
                }

                double dayLenH = effSunset - effSunrise;
                if (dayLenH < 0.0) dayLenH += 24.0;
                std::wstring dayDurStr = FormatDurationHhMm(dayLenH);
                DrawOutlinedTextCentered(rt, brush, g_hudPillFormat, L"Daylight",
                                         D2D1::RectF(domeCx - domeR, horizonY - domeR * 0.74f, domeCx + domeR, horizonY - domeR * 0.74f + lineH),
                                         D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.85f));
                DrawOutlinedTextCentered(rt, brush, g_hudMediumFormat, dayDurStr,
                                         D2D1::RectF(domeCx - domeR + 4.0f, horizonY - domeR * 0.50f, domeCx + domeR - 4.0f, horizonY - 4.0f),
                                         cfg.colorHudText);

                std::wstring srStr = FormatHourHhMm(effSunrise);
                std::wstring ssStr = FormatHourHhMm(effSunset);
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                brush->SetColor(cfg.colorHudText);
                rt->DrawTextW(srStr.c_str(), static_cast<UINT32>(srStr.size()), g_hudBodyFormat, D2D1::RectF(domeCx - domeR, horizonY + 3.0f, domeCx - 4.0f, horizonY + 3.0f + lineH), brush);
                brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.65f));
                rt->DrawTextW(L"Sunrise", 7, g_hudPillFormat, D2D1::RectF(domeCx - domeR, horizonY + 2.0f + lineH, domeCx - 4.0f, cSolar.bottom - 2.0f), brush);

                g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                brush->SetColor(cfg.colorHudText);
                rt->DrawTextW(ssStr.c_str(), static_cast<UINT32>(ssStr.size()), g_hudBodyFormat, D2D1::RectF(domeCx + 4.0f, horizonY + 3.0f, domeCx + domeR, horizonY + 3.0f + lineH), brush);
                brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.65f));
                rt->DrawTextW(L"Sunset", 6, g_hudPillFormat, D2D1::RectF(domeCx + 4.0f, horizonY + 2.0f + lineH, domeCx + domeR, cSolar.bottom - 2.0f), brush);
                g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

                float sideW = (domeCx - domeR - 18.0f) - (cSolar.left + 12.0f);
                if (sideW > 60.0f) {
                    std::wstring amGold = FormatHourHhMm(effSunrise) + L"-" + FormatHourHhMm(effSunrise + 0.75);
                    std::wstring amBlue = FormatHourHhMm(effSunrise - 0.45) + L"-" + FormatHourHhMm(effSunrise);
                    std::wstring pmGold = FormatHourHhMm(effSunset - 0.75) + L"-" + FormatHourHhMm(effSunset);
                    std::wstring pmBlue = FormatHourHhMm(effSunset) + L"-" + FormatHourHhMm(effSunset + 0.45);
                    float topBlockY = cSolar.top + 18.0f;
                    float botBlockY = cSolar.top + m.astroH * 0.56f;

                    g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    brush->SetColor(D2D1::ColorF(1.0f, 0.78f, 0.36f, 0.92f));
                    rt->DrawTextW(L"Golden hour", 11, g_hudPillFormat, D2D1::RectF(cSolar.left + 12.0f, topBlockY, cSolar.left + 12.0f + sideW, topBlockY + lineH), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(amGold.c_str(), static_cast<UINT32>(amGold.size()), g_hudBodyFormat, D2D1::RectF(cSolar.left + 12.0f, topBlockY + lineH, cSolar.left + 12.0f + sideW, topBlockY + lineH * 2.0f), brush);

                    brush->SetColor(D2D1::ColorF(0.45f, 0.76f, 1.0f, 0.92f));
                    rt->DrawTextW(L"Blue hour", 9, g_hudPillFormat, D2D1::RectF(cSolar.left + 12.0f, botBlockY, cSolar.left + 12.0f + sideW, botBlockY + lineH), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(amBlue.c_str(), static_cast<UINT32>(amBlue.size()), g_hudBodyFormat, D2D1::RectF(cSolar.left + 12.0f, botBlockY + lineH, cSolar.left + 12.0f + sideW, botBlockY + lineH * 2.0f), brush);

                    g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    brush->SetColor(D2D1::ColorF(1.0f, 0.78f, 0.36f, 0.92f));
                    rt->DrawTextW(L"Golden hour", 11, g_hudPillFormat, D2D1::RectF(cSolar.right - 28.0f - sideW, topBlockY, cSolar.right - 28.0f, topBlockY + lineH), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(pmGold.c_str(), static_cast<UINT32>(pmGold.size()), g_hudBodyFormat, D2D1::RectF(cSolar.right - 28.0f - sideW, topBlockY + lineH, cSolar.right - 28.0f, topBlockY + lineH * 2.0f), brush);

                    brush->SetColor(D2D1::ColorF(0.45f, 0.76f, 1.0f, 0.92f));
                    rt->DrawTextW(L"Blue hour", 9, g_hudPillFormat, D2D1::RectF(cSolar.right - 28.0f - sideW, botBlockY, cSolar.right - 28.0f, botBlockY + lineH), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(pmBlue.c_str(), static_cast<UINT32>(pmBlue.size()), g_hudBodyFormat, D2D1::RectF(cSolar.right - 28.0f - sideW, botBlockY + lineH, cSolar.right - 28.0f, botBlockY + lineH * 2.0f), brush);

                    g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                }
                DrawInfoIconButton(rt, brush, cfg, cSolar, nextControls);

                curY += m.astroH + 10.0f;

                D2D1_RECT_F cLunar = D2D1::RectF(innerLeft, curY, innerRight, curY + m.astroH);
                nextControls.push_back(InteractiveControl{cLunar, L"toggle_astro_detail_moon", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                brush->SetColor(D2D1::ColorF(0.42f, 0.52f, 0.92f, 0.14f));
                rt->FillRoundedRectangle(D2D1::RoundedRect(cLunar, 10.0f, 10.0f), brush);
                brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f));
                rt->DrawRoundedRectangle(D2D1::RoundedRect(cLunar, 10.0f, 10.0f), brush, 1.0f);

                double effMoonrise = cfg.customMoonriseHour, effMoonset = cfg.customMoonsetHour;
                bool mWin = false;
                ResolveCelestialWindow(cfg, snap, false, hour, effMoonrise, effMoonset, mWin);

                domeCx = (cLunar.left + cLunar.right) * 0.5f;
                horizonY = cLunar.top + lineH * 1.9f;
                domeR = std::min((innerW * 0.44f) * 0.5f, cLunar.bottom - horizonY - 12.0f);

                brush->SetColor(D2D1::ColorF(0.62f, 0.74f, 1.0f, 0.35f));
                rt->DrawLine(D2D1::Point2F(domeCx - domeR - 14.0f, horizonY), D2D1::Point2F(domeCx + domeR + 14.0f, horizonY), brush, 1.4f);

                std::wstring mrStr = FormatHourHhMm(effMoonrise);
                std::wstring msStr = FormatHourHhMm(effMoonset);
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                brush->SetColor(cfg.colorHudText);
                rt->DrawTextW(mrStr.c_str(), static_cast<UINT32>(mrStr.size()), g_hudBodyFormat, D2D1::RectF(domeCx - domeR, cLunar.top + 3.0f, domeCx - 4.0f, cLunar.top + 3.0f + lineH), brush);
                brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.65f));
                rt->DrawTextW(L"Moonrise", 8, g_hudPillFormat, D2D1::RectF(domeCx - domeR, cLunar.top + 3.0f + lineH, domeCx - 4.0f, horizonY - 1.0f), brush);

                g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                brush->SetColor(cfg.colorHudText);
                rt->DrawTextW(msStr.c_str(), static_cast<UINT32>(msStr.size()), g_hudBodyFormat, D2D1::RectF(domeCx + 4.0f, cLunar.top + 3.0f, domeCx + domeR, cLunar.top + 3.0f + lineH), brush);
                brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.65f));
                rt->DrawTextW(L"Moonset", 7, g_hudPillFormat, D2D1::RectF(domeCx + 4.0f, cLunar.top + 3.0f + lineH, domeCx + domeR, horizonY - 1.0f), brush);
                g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

                if (g_d2dFactory && domeR > 16.0f) {
                    ID2D1PathGeometry* semiGeo = nullptr;
                    if (SUCCEEDED(g_d2dFactory->CreatePathGeometry(&semiGeo)) && semiGeo) {
                        ID2D1GeometrySink* sink = nullptr;
                        if (SUCCEEDED(semiGeo->Open(&sink)) && sink) {
                            sink->BeginFigure(D2D1::Point2F(domeCx - domeR, horizonY), D2D1_FIGURE_BEGIN_FILLED);
                            sink->AddArc(D2D1::ArcSegment(
                                D2D1::Point2F(domeCx + domeR, horizonY),
                                D2D1::SizeF(domeR, domeR), 0.0f,
                                D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                            sink->Close();
                            sink->Release();
                            brush->SetColor(D2D1::ColorF(0.55f, 0.68f, 1.0f, 0.14f));
                            rt->FillGeometry(semiGeo, brush);
                            ID2D1Layer* clipLayer = nullptr;
                            if (SUCCEEDED(rt->CreateLayer(nullptr, &clipLayer)) && clipLayer) {
                                rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), semiGeo), clipLayer);
                                DrawMoonBodyHighQuality(rt, brush, domeCx + domeR * 0.66f, horizonY + domeR * 0.62f, domeR * 0.25f, cfg.colorMoon, moonPhase01, 0.95f);
                                rt->PopLayer();
                                clipLayer->Release();
                            }
                        }
                        semiGeo->Release();
                    }
                }

                std::wstring phaseName = MoonPhaseToName(moonPhase01);
                double illumPct = 50.0 * (1.0 - std::cos(moonPhase01 * 2.0 * kPi));
                swprintf_s(buf, L"Illumination %.0f%%", illumPct);
                DrawOutlinedTextCentered(rt, brush, g_hudMediumFormat, phaseName,
                                         D2D1::RectF(domeCx - domeR + 4.0f, horizonY + 4.0f, domeCx + domeR - 4.0f, horizonY + domeR * 0.54f),
                                         cfg.colorHudText);
                DrawOutlinedTextCentered(rt, brush, g_hudPillFormat, buf,
                                         D2D1::RectF(domeCx - domeR, horizonY + domeR * 0.56f, domeCx + domeR, horizonY + domeR * 0.56f + lineH),
                                         D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.85f));

                double altDeg = 0.0, azDeg = 0.0;
                std::wstring nextNewDate, nextFullDate;
                ComputeLunarEphemerisDisplay(moonPhase01, hour, effMoonrise, effMoonset, cfg.latitude, altDeg, azDeg, nextNewDate, nextFullDate);

                sideW = (domeCx - domeR - 18.0f) - (cLunar.left + 12.0f);
                if (sideW > 60.0f) {
                    float topBlockY = cLunar.top + 12.0f;
                    float midBlockY = cLunar.top + m.astroH * 0.44f;
                    float botBlockY = cLunar.top + m.astroH * 0.74f;

                    g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    brush->SetColor(D2D1::ColorF(0.65f, 0.78f, 1.0f, 0.90f));
                    rt->DrawTextW(L"Altitude", 8, g_hudPillFormat, D2D1::RectF(cLunar.left + 12.0f, topBlockY, cLunar.left + 12.0f + sideW, topBlockY + lineH), brush);
                    swprintf_s(buf, L"%.0f°", altDeg);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(buf, static_cast<UINT32>(wcslen(buf)), g_hudBodyFormat, D2D1::RectF(cLunar.left + 12.0f, topBlockY + lineH, cLunar.left + 12.0f + sideW, topBlockY + lineH * 2.0f), brush);

                    brush->SetColor(D2D1::ColorF(0.65f, 0.78f, 1.0f, 0.90f));
                    rt->DrawTextW(L"Azimuth", 7, g_hudPillFormat, D2D1::RectF(cLunar.left + 12.0f, midBlockY, cLunar.left + 12.0f + sideW, midBlockY + lineH), brush);
                    swprintf_s(buf, L"%.0f° %s", azDeg, WindDegToCompass16(azDeg));
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(buf, static_cast<UINT32>(wcslen(buf)), g_hudBodyFormat, D2D1::RectF(cLunar.left + 12.0f, midBlockY + lineH, cLunar.left + 12.0f + sideW, midBlockY + lineH * 2.0f), brush);

                    if (snap.hasNeoData) {
                        swprintf_s(buf, L"NEO: %d (%.1f LD)", snap.neoCountToday, snap.neoClosestLd);
                        brush->SetColor(D2D1::ColorF(0.80f, 0.74f, 1.0f, 0.92f));
                        rt->DrawTextW(buf, static_cast<UINT32>(wcslen(buf)), g_hudPillFormat,
                                      D2D1::RectF(cLunar.left + 12.0f, botBlockY, cLunar.left + 12.0f + sideW, cLunar.bottom - 4.0f), brush);
                    }

                    g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    brush->SetColor(D2D1::ColorF(0.65f, 0.78f, 1.0f, 0.90f));
                    rt->DrawTextW(L"New Moon", 8, g_hudPillFormat, D2D1::RectF(cLunar.right - 28.0f - sideW, topBlockY, cLunar.right - 28.0f, topBlockY + lineH), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(nextNewDate.c_str(), static_cast<UINT32>(nextNewDate.size()), g_hudBodyFormat, D2D1::RectF(cLunar.right - 28.0f - sideW, topBlockY + lineH, cLunar.right - 28.0f, topBlockY + lineH * 2.0f), brush);

                    brush->SetColor(D2D1::ColorF(0.65f, 0.78f, 1.0f, 0.90f));
                    rt->DrawTextW(L"Full Moon", 9, g_hudPillFormat, D2D1::RectF(cLunar.right - 28.0f - sideW, midBlockY, cLunar.right - 28.0f, midBlockY + lineH), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(nextFullDate.c_str(), static_cast<UINT32>(nextFullDate.size()), g_hudBodyFormat, D2D1::RectF(cLunar.right - 28.0f - sideW, midBlockY + lineH, cLunar.right - 28.0f, midBlockY + lineH * 2.0f), brush);

                    g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                }
                DrawInfoIconButton(rt, brush, cfg, cLunar, nextControls);

                curY += m.astroH + 10.0f;

                D2D1_RECT_F cUvEmi = D2D1::RectF(innerLeft, curY, innerRight, curY + m.astroH);
                nextControls.push_back(InteractiveControl{cUvEmi, L"toggle_astro_detail_uvemi", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                brush->SetColor(D2D1::ColorF(0.56f, 0.36f, 0.86f, 0.14f));
                rt->FillRoundedRectangle(D2D1::RoundedRect(cUvEmi, 10.0f, 10.0f), brush);
                brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f));
                rt->DrawRoundedRectangle(D2D1::RoundedRect(cUvEmi, 10.0f, 10.0f), brush, 1.0f);

                float colW3 = (innerW - 24.0f) / 3.0f;
                D2D1_RECT_F colUv = D2D1::RectF(cUvEmi.left + 8.0f, cUvEmi.top + 10.0f, cUvEmi.left + 8.0f + colW3, cUvEmi.bottom - 10.0f);
                D2D1_RECT_F colEmi = D2D1::RectF(colUv.right + 4.0f, cUvEmi.top + 10.0f, colUv.right + 4.0f + colW3, cUvEmi.bottom - 10.0f);
                D2D1_RECT_F colSched = D2D1::RectF(colEmi.right + 4.0f, cUvEmi.top + 10.0f, cUvEmi.right - 26.0f, cUvEmi.bottom - 10.0f);

                wchar_t uvValB[24] = {}, kpValB[24] = {};
                swprintf_s(uvValB, L"%.1f", snap.uvIndex);
                swprintf_s(kpValB, L"%.1f", snap.flareKpEst);
                DrawFourRowMetricColumn(rt, brush, cfg, colUv, L"UV Index",
                                        (snap.valid && snap.mUv.available) ? uvValB : L"--",
                                        (snap.uvIndex >= 6.0) ? L"High" : L"Index",
                                        1, snap.uvIndex, timeSec, lineH);
                DrawFourRowMetricColumn(rt, brush, cfg, colEmi, L"EMI / Solar",
                                        snap.valid ? kpValB : L"--",
                                        snap.hasSpaceWeather ? snap.spaceWeatherText : L"Kp index",
                                        6, snap.flareKpEst, timeSec, lineH);

                float sy = colSched.top + 4.0f;
                float evRowH = std::max(14.5f, lineH * 0.85f);
                brush->SetColor(D2D1::ColorF(0.75f, 0.68f, 1.0f, 0.92f));
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                rt->DrawTextW(L"UPCOMING", 8, g_hudPillFormat, D2D1::RectF(colSched.left, sy, colSched.right, sy + evRowH), brush);
                sy += evRowH + 2.0f;
                brush->SetColor(cfg.colorHudText);
                auto astroEvents = BuildUpcomingAstroEvents(snap);
                for (size_t eIdx = 0; eIdx < astroEvents.size() && eIdx < 5; ++eIdx) {
                    if (sy + evRowH > colSched.bottom) break;
                    rt->DrawTextW(astroEvents[eIdx].c_str(), static_cast<UINT32>(astroEvents[eIdx].size()), g_hudPillFormat,
                                  D2D1::RectF(colSched.left, sy, colSched.right, sy + evRowH), brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    sy += evRowH + 1.0f;
                }
                g_hudPillFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                DrawInfoIconButton(rt, brush, cfg, cUvEmi, nextControls);
            }
        } else {
            float tabW = (innerW - 16.0f) / 3.0f;
            DrawModeTabButton(rt, brush, cfg, D2D1::RectF(innerLeft, curY, innerLeft + tabW, curY + m.subBarH),
                              L"Now", L"tab_now", (m.activeTab == HudDataTab::Now), nextControls);
            DrawModeTabButton(rt, brush, cfg, D2D1::RectF(innerLeft + tabW + 8.0f, curY, innerLeft + tabW * 2.0f + 8.0f, curY + m.subBarH),
                              L"Hours", L"tab_hours", (m.activeTab == HudDataTab::Hours), nextControls);
            DrawModeTabButton(rt, brush, cfg, D2D1::RectF(innerLeft + tabW * 2.0f + 16.0f, curY, innerRight, curY + m.subBarH),
                              L"Days", L"tab_days", (m.activeTab == HudDataTab::Days), nextControls);
            curY += m.subBarH + 10.0f;

            if (m.activeTab == HudDataTab::Hours) {
                int totalH = static_cast<int>(snap.hourly.size());
                int totalPages = std::max(1, (totalH + 7) / 8);
                int hoursPage = std::clamp(m.hoursPage, 0, totalPages - 1);

                std::vector<SeriesSlotItem> hSlots;
                int startIdx = hoursPage * 8;
                int endIdx = std::min(totalH, startIdx + 8);
                for (int i = startIdx; i < endIdx; ++i) {
                    const auto& hp = snap.hourly[i];
                    SeriesSlotItem it;
                    it.label = hp.timeLabel;
                    it.tempMinC = hp.tempMinC;
                    it.tempMaxC = hp.tempMaxC;
                    it.precipMm = hp.precipMm;
                    it.windSpeedKmh = hp.windSpeedKmh;
                    it.windDirDeg = hp.windDirDeg;
                    it.pressureHpa = hp.pressureHpa;
                    it.aqiValue = hp.aqiValue;
                    it.weatherCode = hp.weatherCode;
                    it.isCurrent = (i == 0);
                    hSlots.push_back(it);
                }

                if (m.hoursDet != DetailWidgetKind::None) {
                    float detH = std::max(m.bodyAreaH, 26.0f * std::max(18.0f, lineH + 1.5f) + 16.0f);
                    D2D1_RECT_F detBox = D2D1::RectF(innerLeft, curY, innerRight, curY + detH);
                    nextControls.push_back(InteractiveControl{detBox, L"toggle_hours_detail_1", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                    DrawDetailTableOverlay(rt, brush, cfg, snap, false, m.hoursDet, hour, moonPhase01, detBox, lineH);
                } else {
                    D2D1_RECT_F r1 = D2D1::RectF(innerLeft, curY, innerRight, curY + m.bodyAreaH);
                    nextControls.push_back(InteractiveControl{r1, L"toggle_hours_detail_1", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                    DrawUnifiedWeatherWindChart(rt, brush, cfg, r1, L"HOURLY METEO (click for 24h table)", hSlots, lineH, timeSec,
                                                hoursPage, totalPages, L"hours_page_prev", L"hours_page_next", nextControls);
                }
            } else if (m.activeTab == HudDataTab::Days) {
                int totalD = static_cast<int>(snap.daily.size());
                int totalPages = std::max(1, (totalD + 6) / 7);
                int daysPage = std::clamp(m.daysPage, 0, totalPages - 1);
                {
                    std::lock_guard<std::mutex> lock(g_hudUiMutex);
                    g_daysPage = daysPage;
                }

                std::vector<SeriesSlotItem> dSlots;
                int startIdx = daysPage * 7;
                int endIdx = std::min(totalD, startIdx + 7);
                for (int i = startIdx; i < endIdx; ++i) {
                    const auto& dp = snap.daily[i];
                    SeriesSlotItem it;
                    it.label = dp.dayLabel;
                    it.tempMinC = dp.tempMinC;
                    it.tempMaxC = dp.tempMaxC;
                    it.precipMm = dp.precipMm;
                    it.windSpeedKmh = dp.windSpeedKmh;
                    it.windDirDeg = dp.windDirDeg;
                    it.pressureHpa = dp.pressureHpa;
                    it.aqiValue = dp.aqiValue;
                    it.weatherCode = dp.weatherCode;
                    it.isCurrent = (i == 0);
                    dSlots.push_back(it);
                }

                if (m.daysDet != DetailWidgetKind::None) {
                    int dRows = std::max(8, totalD + 2);
                    float detH = std::max(m.bodyAreaH, static_cast<float>(dRows) * std::max(18.0f, lineH + 1.5f) + 16.0f);
                    D2D1_RECT_F detBox = D2D1::RectF(innerLeft, curY, innerRight, curY + detH);
                    nextControls.push_back(InteractiveControl{detBox, L"toggle_days_detail_1", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                    DrawDetailTableOverlay(rt, brush, cfg, snap, true, m.daysDet, hour, moonPhase01, detBox, lineH);
                } else {
                    D2D1_RECT_F r1 = D2D1::RectF(innerLeft, curY, innerRight, curY + m.bodyAreaH);
                    nextControls.push_back(InteractiveControl{r1, L"toggle_days_detail_1", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                    DrawUnifiedWeatherWindChart(rt, brush, cfg, r1, L"DAILY METEO (click for table)", dSlots, lineH, timeSec,
                                                daysPage, totalPages, L"days_page_prev", L"days_page_next", nextControls);
                }
            } else {
                if (m.nowDet != DetailWidgetKind::None) {
                    D2D1_RECT_F detBox = D2D1::RectF(innerLeft, curY, innerRight, curY + m.bodyAreaH);
                    const wchar_t* act = (m.nowDet == DetailWidgetKind::NowHero)
                        ? L"toggle_now_detail_hero"
                        : ((m.nowDet == DetailWidgetKind::NowWindPrecip) ? L"toggle_now_detail_wind" : L"toggle_now_detail_air");
                    nextControls.push_back(InteractiveControl{detBox, act, L"", D2D1::ColorF(0, 0, 0, 0), L""});
                    DrawDetailTableOverlay(rt, brush, cfg, snap, false, m.nowDet, hour, moonPhase01, detBox, lineH);
                } else {
                    D2D1_RECT_F cHero = D2D1::RectF(innerLeft, curY, innerRight, curY + m.heroH);
                    nextControls.push_back(InteractiveControl{cHero, L"toggle_now_detail_hero", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                    brush->SetOpacity(1.0f);
                    brush->SetColor(D2D1::ColorF(0.05f, 0.08f, 0.14f, 0.92f));
                    rt->FillRoundedRectangle(D2D1::RoundedRect(cHero, 10.0f, 10.0f), brush);
                    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f));
                    rt->DrawRoundedRectangle(D2D1::RoundedRect(cHero, 10.0f, 10.0f), brush, 1.0f);

                    rt->PushAxisAlignedClip(D2D1::RectF(cHero.left + 1.0f, cHero.top + 1.0f, cHero.right - 1.0f, cHero.bottom - 1.0f),
                                           D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                    DrawMiniMonitorPreview(surf, cfg, fx, cloudPct01, moonPhase01, sunOn, moonOn, cHero, timeSec);

                    std::wstring heroTemp = (snap.valid && snap.mTemp.available) ? FormatTempValue(snap.tempC, cfg.tempUnit, true) : L"--°";
                    float bigH = m.baseF * 3.15f;
                    float bigTop = cHero.top + (m.heroH - bigH - lineH * 1.9f) * 0.48f + 6.0f;
                    brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f));
                    rt->DrawTextW(heroTemp.c_str(), static_cast<UINT32>(heroTemp.size()), g_hudHeroFormat,
                                  D2D1::RectF(cHero.left + 2.0f, bigTop + 2.0f, cHero.right + 2.0f, bigTop + bigH + 2.0f), brush);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(heroTemp.c_str(), static_cast<UINT32>(heroTemp.size()), g_hudHeroFormat,
                                  D2D1::RectF(cHero.left, bigTop, cHero.right, bigTop + bigH), brush);

                    std::wstring minMaxStr = (snap.valid && snap.hasTodayMinMax)
                        ? (L"Max " + FormatTempValue(snap.todayMaxC, cfg.tempUnit, true) + L"  Min " + FormatTempValue(snap.todayMinC, cfg.tempUnit, true))
                        : L"Max --  Min --";
                    std::wstring feelsStr = L"Feels like " + ((snap.valid && snap.mFeelsLike.available) ? FormatTempValue(snap.apparentTempC, cfg.tempUnit, true) : L"--");
                    std::wstring midMeta = minMaxStr + L"  •  " + feelsStr;
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.92f));
                    rt->DrawTextW(midMeta.c_str(), static_cast<UINT32>(midMeta.size()), g_hudBodyFormat,
                                  D2D1::RectF(cHero.left + 10.0f, cHero.bottom - lineH * 2.1f - 6.0f, cHero.right - 10.0f, cHero.bottom - lineH - 4.0f),
                                  brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

                    std::wstring condTxt = (snap.valid && snap.mCondition.available) ? WmoCodeToLabel(snap.weatherCode) : L"No forecast";
                    std::wstring dnTxt = (snap.valid && snap.mDayNight.available) ? FormatDayNightValue(snap.isDay, cfg.dayNightUnit) : L"--";
                    std::wstring botMeta = condTxt + L" • " + dnTxt;
                    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.70f));
                    rt->DrawTextW(botMeta.c_str(), static_cast<UINT32>(botMeta.size()), g_hudBodyFormat,
                                  D2D1::RectF(cHero.left + 10.0f, cHero.bottom - lineH - 6.0f, cHero.right - 10.0f, cHero.bottom - 4.0f),
                                  brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    rt->PopAxisAlignedClip();
                    DrawInfoIconButton(rt, brush, cfg, cHero, nextControls);

                    curY += m.heroH + 10.0f;

                    float symLeftW = std::clamp(150.0f * m.fontScale, 146.0f * dpi, 186.0f * dpi);
                    float gaugeZoneW = std::clamp(128.0f * m.fontScale, 122.0f * dpi, 156.0f * dpi);
                    D2D1_RECT_F cWindPrecip = D2D1::RectF(innerLeft, curY, innerRight, curY + m.row2H);
                    nextControls.push_back(InteractiveControl{cWindPrecip, L"toggle_now_detail_wind", L"", D2D1::ColorF(0, 0, 0, 0), L""});

                    brush->SetOpacity(1.0f);
                    brush->SetColor(D2D1::ColorF(0.22f, 0.47f, 0.82f, 0.14f));
                    rt->FillRoundedRectangle(D2D1::RoundedRect(cWindPrecip, 10.0f, 10.0f), brush);
                    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.11f));
                    rt->DrawRoundedRectangle(D2D1::RoundedRect(cWindPrecip, 10.0f, 10.0f), brush, 1.0f);

                    rt->PushAxisAlignedClip(D2D1::RectF(cWindPrecip.left + 1.0f, cWindPrecip.top + 1.0f, cWindPrecip.right - 1.0f, cWindPrecip.bottom - 1.0f),
                                           D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

                    float compZoneRight = cWindPrecip.left + symLeftW;
                    float footerH = lineH + 10.0f;
                    float compAvailH = m.row2H - footerH - 12.0f;
                    float compR = std::min((symLeftW - 20.0f) * 0.5f, compAvailH * 0.5f);
                    float compCx = (cWindPrecip.left + compZoneRight) * 0.5f;
                    float compCy = cWindPrecip.top + 8.0f + compR;

                    std::wstring wMain = L"--", wSub = L"";
                    if (snap.valid && snap.mWind.available) FormatWindSplit(snap.windSpeedKmh, cfg.windUnit, wMain, wSub);
                    DrawWindCompassWidget(rt, brush, cfg, compCx, compCy, compR, snap.windDirDeg, wMain, wSub);

                    float botRowY = cWindPrecip.bottom - footerH;
                    DrawDynamicWindForceIcon(rt, brush, cWindPrecip.left + 10.0f, botRowY + 2.0f, 24.0f * dpi, lineH - 2.0f, snap.windSpeedKmh, timeSec);
                    std::wstring gustTxt = (snap.valid && snap.mGusts.available)
                        ? (L"Gusts " + FormatWindValue(snap.windGustsKmh, cfg.windUnit))
                        : L"Gusts --";
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(gustTxt.c_str(), static_cast<UINT32>(gustTxt.size()), g_hudBodyFormat,
                                  D2D1::RectF(cWindPrecip.left + 36.0f * dpi, botRowY, compZoneRight - 4.0f, cWindPrecip.bottom - 4.0f),
                                  brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    g_hudBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

                    float gaugeCenterX = cWindPrecip.right - gaugeZoneW * 0.5f - 6.0f;
                    float gaugeR = std::min(gaugeZoneW * 0.40f, (m.row2H - lineH * 2.2f) * 0.50f);
                    float gaugeCy = cWindPrecip.top + 14.0f + gaugeR;

                    float colsL = compZoneRight + 4.0f;
                    float colsR = gaugeCenterX - gaugeR - 8.0f;
                    float colW3 = (colsR - colsL) / 3.0f;

                    std::wstring clV = L"--", clU = L"%", huV = L"--", huU = L"%", prV = L"--", prU = L"mm";
                    if (snap.valid && snap.mClouds.available) FormatCloudSplit(snap.cloudCoverPct, cfg.cloudUnit, clV, clU);
                    if (snap.valid && snap.mHumidity.available) FormatHumiditySplit(snap.humidityPct, cfg.humidityUnit, huV, huU);
                    if (snap.valid && snap.mPrecip.available) FormatPrecipSplit(snap.precipMm, cfg.precipUnit, prV, prU);
                    double humFill01 = (snap.valid && snap.mHumidity.available) ? ComputeHumidityFill01(snap.humidityPct, cfg.humidityUnit) : 0.0;

                    DrawFourRowMetricColumn(rt, brush, cfg, D2D1::RectF(colsL, cWindPrecip.top + 10.0f, colsL + colW3, cWindPrecip.bottom - 10.0f),
                                            L"Clouds", clV, clU, 0, snap.cloudCoverPct, timeSec, lineH);
                    DrawFourRowMetricColumn(rt, brush, cfg, D2D1::RectF(colsL + colW3, cWindPrecip.top + 10.0f, colsL + colW3 * 2.0f, cWindPrecip.bottom - 10.0f),
                                            L"Humidity", huV, huU, 3, humFill01, timeSec, lineH);
                    DrawFourRowMetricColumn(rt, brush, cfg, D2D1::RectF(colsL + colW3 * 2.0f, cWindPrecip.top + 10.0f, colsR, cWindPrecip.bottom - 10.0f),
                                            L"Precip", prV, prU, 2, snap.precipMm, timeSec, lineH);

                    double pHpa = snap.valid ? snap.pressureHpa : 1013.25;
                    float p01 = std::clamp(static_cast<float>((pHpa - 970.0) / 80.0), 0.0f, 1.0f);
                    float n01 = static_cast<float>((995.0 - 970.0) / 80.0);
                    float n11 = static_cast<float>((1024.0 - 970.0) / 80.0);
                    bool pNorm = (pHpa >= 995.0 && pHpa <= 1024.0);
                    D2D1_COLOR_F pDotCol = pNorm ? D2D1::ColorF(0.26f, 0.86f, 0.48f, 1.0f) : D2D1::ColorF(0.96f, 0.68f, 0.24f, 1.0f);
                    DrawSpeedometerGauge(rt, brush, gaugeCenterX, gaugeCy, gaugeR, p01, n01, n11, pDotCol, false);

                    std::wstring pMain = L"--", pSub = L"", pNormRange = L"995-1024 hPa";
                    if (snap.valid && snap.mPressure.available) {
                        FormatPressureSplit(pHpa, cfg.pressureUnit, pMain, pSub, pNormRange);
                    }
                    float medH = m.baseF * 2.0f;
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(pMain.c_str(), static_cast<UINT32>(pMain.size()), g_hudMediumFormat,
                                  D2D1::RectF(gaugeCenterX - gaugeR * 0.88f, gaugeCy - medH * 0.62f, gaugeCenterX + gaugeR * 0.88f, gaugeCy + medH * 0.36f), brush);
                    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.75f));
                    rt->DrawTextW(pSub.c_str(), static_cast<UINT32>(pSub.size()), g_hudPillFormat,
                                  D2D1::RectF(gaugeCenterX - gaugeR * 0.85f, gaugeCy + medH * 0.26f, gaugeCenterX + gaugeR * 0.85f, gaugeCy + gaugeR * 0.88f), brush);

                    const wchar_t* pStatus = !snap.valid ? L"--" : (pNorm ? L"Normal" : ((pHpa < 995.0) ? L"Low" : L"High"));
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(pStatus, static_cast<UINT32>(wcslen(pStatus)), g_hudPillFormat,
                                  D2D1::RectF(gaugeCenterX - gaugeZoneW * 0.5f, gaugeCy + gaugeR * 0.82f, gaugeCenterX + gaugeZoneW * 0.5f, gaugeCy + gaugeR * 0.82f + lineH), brush);

                    D2D1_RECT_F normPill = D2D1::RectF(gaugeCenterX - gaugeZoneW * 0.48f, cWindPrecip.bottom - lineH - 4.0f, gaugeCenterX + gaugeZoneW * 0.48f, cWindPrecip.bottom - 4.0f);
                    brush->SetColor(D2D1::ColorF(0.24f, 0.80f, 0.46f, 0.22f));
                    rt->FillRoundedRectangle(D2D1::RoundedRect(normPill, 6.0f, 6.0f), brush);
                    brush->SetColor(D2D1::ColorF(0.45f, 0.92f, 0.62f, 0.95f));
                    rt->DrawTextW(pNormRange.c_str(), static_cast<UINT32>(pNormRange.size()), g_hudPillFormat, normPill, brush);
                    rt->PopAxisAlignedClip();
                    DrawInfoIconButton(rt, brush, cfg, cWindPrecip, nextControls);

                    curY += m.row2H + 10.0f;

                    D2D1_RECT_F cAir = D2D1::RectF(innerLeft, curY, innerRight, curY + m.row3H);
                    nextControls.push_back(InteractiveControl{cAir, L"toggle_now_detail_air", L"", D2D1::ColorF(0, 0, 0, 0), L""});
                    brush->SetColor(D2D1::ColorF(0.20f, 0.68f, 0.54f, 0.14f));
                    rt->FillRoundedRectangle(D2D1::RoundedRect(cAir, 10.0f, 10.0f), brush);
                    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f));
                    rt->DrawRoundedRectangle(D2D1::RoundedRect(cAir, 10.0f, 10.0f), brush, 1.0f);

                    float eyeZoneRight = cAir.left + symLeftW;
                    float eyeCx = (cAir.left + eyeZoneRight) * 0.5f;
                    float eyeCy = cAir.top + m.row3H * 0.36f;
                    float visOpen01 = (snap.valid && snap.mVisibility.available)
                        ? std::clamp(static_cast<float>(snap.visibilityM / 12000.0), 0.0f, 1.0f)
                        : 0.75f;
                    DrawDynamicEyeIcon(rt, brush, eyeCx, eyeCy, symLeftW * 0.30f, m.row3H * 0.18f, visOpen01, cfg.colorHudText);

                    std::wstring visTxt = (snap.valid && snap.mVisibility.available) ? FormatVisibilityValue(snap.visibilityM, cfg.visibilityUnit) : L"--";
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(visTxt.c_str(), static_cast<UINT32>(visTxt.size()), g_hudMediumFormat,
                                  D2D1::RectF(cAir.left + 8.0f, cAir.top + m.row3H * 0.58f, eyeZoneRight - 8.0f, cAir.bottom - 8.0f),
                                  brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

                    gaugeCenterX = cAir.right - gaugeZoneW * 0.5f - 6.0f;
                    gaugeR = std::min(gaugeZoneW * 0.40f, (m.row3H - lineH * 2.2f) * 0.50f);
                    gaugeCy = cAir.top + 14.0f + gaugeR;

                    double aqiVal = snap.valid ? snap.aqiValue : 25.0;
                    float aqi01 = std::clamp(static_cast<float>(aqiVal / 200.0), 0.0f, 1.0f);
                    D2D1_COLOR_F aqiDotCol = (aqiVal <= 50.0)
                        ? D2D1::ColorF(0.24f, 0.85f, 0.48f, 1.0f)
                        : ((aqiVal <= 100.0) ? D2D1::ColorF(0.95f, 0.82f, 0.24f, 1.0f)
                                             : ((aqiVal <= 150.0) ? D2D1::ColorF(0.96f, 0.54f, 0.22f, 1.0f)
                                                                  : D2D1::ColorF(0.92f, 0.28f, 0.34f, 1.0f)));
                    DrawSpeedometerGauge(rt, brush, gaugeCenterX, gaugeCy, gaugeR, aqi01, 0.0f, 0.25f, aqiDotCol, true);

                    std::wstring aqiMain = L"--", aqiSub = L"";
                    if (snap.valid && snap.mAqi.available) FormatAqiSplit(snap.aqiValue, snap.pm25Value, cfg.aqiUnit, aqiMain, aqiSub);
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(aqiMain.c_str(), static_cast<UINT32>(aqiMain.size()), g_hudMediumFormat,
                                  D2D1::RectF(gaugeCenterX - gaugeR * 0.88f, gaugeCy - medH * 0.62f, gaugeCenterX + gaugeR * 0.88f, gaugeCy + medH * 0.36f), brush);
                    brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.75f));
                    rt->DrawTextW(aqiSub.c_str(), static_cast<UINT32>(aqiSub.size()), g_hudPillFormat,
                                  D2D1::RectF(gaugeCenterX - gaugeR * 0.85f, gaugeCy + medH * 0.26f, gaugeCenterX + gaugeR * 0.85f, gaugeCy + gaugeR * 0.88f), brush);

                    const wchar_t* aqiStatus = !snap.valid ? L"--" : ((aqiVal <= 50.0) ? L"Good" : ((aqiVal <= 100.0) ? L"Moderate" : ((aqiVal <= 150.0) ? L"Unhealthy" : L"Hazardous")));
                    brush->SetColor(cfg.colorHudText);
                    rt->DrawTextW(aqiStatus, static_cast<UINT32>(wcslen(aqiStatus)), g_hudPillFormat,
                                  D2D1::RectF(gaugeCenterX - gaugeZoneW * 0.5f, gaugeCy + gaugeR * 0.82f, gaugeCenterX + gaugeZoneW * 0.5f, gaugeCy + gaugeR * 0.82f + lineH), brush);

                    D2D1_RECT_F aqiNormPill = D2D1::RectF(gaugeCenterX - gaugeZoneW * 0.48f, cAir.bottom - lineH - 4.0f, gaugeCenterX + gaugeZoneW * 0.48f, cAir.bottom - 4.0f);
                    brush->SetColor(D2D1::ColorF(0.24f, 0.80f, 0.46f, 0.22f));
                    rt->FillRoundedRectangle(D2D1::RoundedRect(aqiNormPill, 6.0f, 6.0f), brush);
                    brush->SetColor(D2D1::ColorF(0.45f, 0.92f, 0.62f, 0.95f));
                    rt->DrawTextW(L"0-50 AQI", 8, g_hudPillFormat, aqiNormPill, brush);

                    colsL = eyeZoneRight + 4.0f;
                    colsR = gaugeCenterX - gaugeR - 8.0f;
                    colW3 = (colsR - colsL) / 3.0f;

                    bool fogHigh = (snap.valid && snap.mTemp.available && snap.mDewPoint.available && (snap.tempC - snap.dewPointC <= 2.0));
                    std::wstring fogVal = (snap.valid && snap.mTemp.available && snap.mDewPoint.available)
                        ? (fogHigh ? L"High" : ((snap.tempC - snap.dewPointC <= 5.0) ? L"Med" : L"Low"))
                        : L"--";
                    std::wstring dewV = L"--", dewU = L"°C", polV = L"--", polU = L"idx";
                    if (snap.valid && snap.mDewPoint.available) FormatTempSplit(snap.dewPointC, cfg.tempUnit, dewV, dewU);
                    if (snap.valid && snap.mPollen.available) FormatPollenSplit(snap.pollenMaxValue, cfg.pollenUnit, polV, polU);

                    DrawFourRowMetricColumn(rt, brush, cfg, D2D1::RectF(colsL, cAir.top + 10.0f, colsL + colW3, cAir.bottom - 10.0f),
                                            L"Fog", fogVal, L"risk", 4, fogHigh ? 1.0 : 0.0, timeSec, lineH);
                    DrawFourRowMetricColumn(rt, brush, cfg, D2D1::RectF(colsL + colW3, cAir.top + 10.0f, colsL + colW3 * 2.0f, cAir.bottom - 10.0f),
                                            L"Dew point", dewV, dewU, 5, snap.dewPointC, 0.0, lineH);
                    DrawFourRowMetricColumn(rt, brush, cfg, D2D1::RectF(colsL + colW3 * 2.0f, cAir.top + 10.0f, colsR, cAir.bottom - 10.0f),
                                            L"Pollen", polV, polU, 7, snap.pollenMaxValue, timeSec, lineH);
                    DrawInfoIconButton(rt, brush, cfg, cAir, nextControls);
                }
            }
        }

        if (m.needsScroll && m.maxScroll > 0) {
            float trackTop = top + 8.0f;
            float trackH = cardH - 16.0f;
            float thumbH = std::max(24.0f, trackH * (cardH / m.totalContentH));
            float thumbTop = trackTop + (trackH - thumbH) * (static_cast<float>(m.scrollPx) / static_cast<float>(m.maxScroll));
            brush->SetOpacity(1.0f);
            brush->SetColor(D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.32f));
            rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(left + cardW - 7.0f, thumbTop, left + cardW - 3.0f, thumbTop + thumbH), 2.0f, 2.0f), brush);
        }
        rt->PopAxisAlignedClip();
    }

    {
        std::lock_guard<std::mutex> lock(g_hudUiMutex);
        g_hudControls = std::move(nextControls);
    }

    brush->SetOpacity(1.0f);
    brush->SetColor(cfg.colorHudBg);
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(m.btnCx, m.btnCy), m.btnR, m.btnR), brush);
    brush->SetColor(m.isDragging
        ? D2D1::ColorF(0.35f, 0.78f, 1.0f, 0.90f)
        : D2D1::ColorF(cfg.colorHudText.r, cfg.colorHudText.g, cfg.colorHudText.b, 0.50f));
    rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(m.btnCx, m.btnCy), m.btnR, m.btnR), brush, m.isDragging ? 2.2f : 1.6f);

    if (showGearIcon) {
        DrawGearSettingsIcon(rt, brush, m.btnCx, m.btnCy, m.btnR, cfg.colorHudText);
    } else {
        brush->SetColor(cfg.colorHudText);
        rt->DrawTextW(labelText.c_str(), static_cast<UINT32>(labelText.size()), g_hudCircleFormat,
                      D2D1::RectF(m.btnCx - m.btnR, m.btnCy - m.btnR, m.btnCx + m.btnR, m.btnCy + m.btnR), brush);
    }
}

void RenderOverlayFrame(const Settings& cfg, const ForecastSnapshot& snap, const GeneralEffectsState& fx,
                        float cloudPct01, float uvIndex, double moonPhase01,
                        bool sunOn, bool moonOn, double hour, int bottomLimitY, float precipVx, double timeSec,
                        bool hasCursor, float curRelX, float curRelY, bool& outCursorNearSky) {
    outCursorNearSky = false;
    if (!g_overlayHwnd || !EnsureSurface(g_surface, g_canvasWidth, g_canvasHeight, cfg.colorSnow, cfg.hasCustomSnowColor)) {
        return;
    }
    float bottomRelY = static_cast<float>(bottomLimitY - g_canvasTop);
    RECT bindRect = {0, 0, g_canvasWidth, g_canvasHeight};
    g_surface.rt->BindDC(g_surface.memDC, &bindRect);
    g_surface.rt->BeginDraw();
    g_surface.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    g_surface.rt->Clear(D2D1::ColorF(0, 0, 0, 0));

    QualityTier tierClouds = ResolveCategoryTier(cfg, cfg.perfClouds);
    QualityTier tierCelestial = ResolveCategoryTier(cfg, cfg.perfCelestial);
    QualityTier tierPrecip = ResolveCategoryTier(cfg, cfg.perfPrecip);
    QualityTier tierEffects = ResolveCategoryTier(cfg, cfg.perfEffects);

    float k = static_cast<float>(cfg.effectsIntensity) / 25.0f;

    DrawSkyAmbientTintLayer(g_surface.rt, cfg, snap, hour);

    if (fx.nightStars) {
        DrawNightStarsLayer(g_surface.rt, g_surface.sharedBrush, tierEffects, k, timeSec);
    }

    float cloudDimFactor = cfg.celestialInFrontOfClouds ? 1.0f : (1.0f - cloudPct01 * 0.65f);
    if (!cfg.celestialInFrontOfClouds) {
        DrawSunLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierCelestial, sunOn, hour, snap, uvIndex, cloudDimFactor, timeSec, hasCursor, curRelX, curRelY, outCursorNearSky);
        DrawMoonLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierCelestial, moonOn, hour, snap, moonPhase01, cloudDimFactor, hasCursor, curRelX, curRelY, outCursorNearSky);
    }
    DrawTopStaticCloudsLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierClouds, cloudPct01, timeSec, hasCursor, curRelX, curRelY, outCursorNearSky);
    DrawDriftingCloudsLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierClouds, cloudPct01, precipVx, timeSec, hasCursor, curRelX, curRelY, outCursorNearSky);
    if (cfg.celestialInFrontOfClouds) {
        DrawSunLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierCelestial, sunOn, hour, snap, uvIndex, cloudDimFactor, timeSec, hasCursor, curRelX, curRelY, outCursorNearSky);
        DrawMoonLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierCelestial, moonOn, hour, snap, moonPhase01, cloudDimFactor, hasCursor, curRelX, curRelY, outCursorNearSky);
    }

    if (fx.dust) DrawDustLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierEffects, k, precipVx, timeSec);
    if (fx.fog) DrawFogLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierEffects, k, timeSec);
    if (fx.smog) DrawSmogLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierEffects, k, timeSec);
    if (fx.airQuality) DrawAirQualityLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierEffects, snap, k, timeSec);
    if (fx.frost || fx.heat) DrawThermalLayer(g_surface.rt, g_surface.sharedBrush, tierEffects, fx.frost, fx.heat, k, timeSec);
    if (fx.aurora) DrawSmoothAurora(g_surface.rt, g_surface.sharedBrush, cfg, tierEffects, k, timeSec);
    if (fx.mirage) DrawSmoothMirage(g_surface.rt, g_surface.sharedBrush, cfg, tierEffects, k, timeSec);
    if (fx.thunderstorm && g_lightning.timer > 0.0f) {
        D2D1_COLOR_F tc = cfg.hasCustomThunderstormColor ? cfg.colorThunderstorm : D2D1::ColorF(0.86f, 0.93f, 1.0f, 0.95f);
        float tAlpha = std::clamp(g_lightning.timer * 4.2f * std::max(0.25f, k), 0.0f, 1.0f);
        float topBound = g_monitorRects.empty() ? 0.0f : (g_monitorRects.front().top + g_monitorRects.front().innerTopRel);
        g_surface.sharedBrush->SetColor(D2D1::ColorF(tc.r, tc.g, tc.b, tAlpha));
        DrawBranchedLightning(g_surface.rt, g_surface.sharedBrush, tierEffects, g_lightning, topBound, bottomRelY, tAlpha);
    }
    for (const auto& p : g_falling) {
        DrawItemShape(g_surface, cfg, tierPrecip, p, bottomRelY);
    }
    if (fx.pollen) DrawPollenLayer(g_surface.rt, g_surface.sharedBrush, cfg, tierEffects, k, timeSec);

    if (FAILED(g_surface.rt->EndDraw())) {
        CleanupSurface(g_surface);
        return;
    }
    PresentSurfaceAt(g_overlayHwnd, g_surface, g_canvasLeft, g_canvasTop, g_canvasWidth, g_canvasHeight);
}

void RenderHudFrame(const Settings& cfg, const ForecastSnapshot& snap, const GeneralEffectsState& fx,
                    float cloudPct01, double moonPhase01, bool sunOn, bool moonOn,
                    double hour, int bottomLimitY, double timeSec) {
    if (!g_hudHwnd) return;
    HudLayoutMetrics m = ComputeHudLayout(cfg, snap, bottomLimitY);
    if (!m.showButton) return;

    bool boundsChanged = (m.winLeft != g_hudWinLeft.load() || m.winTop != g_hudWinTop.load() ||
                          m.winW != g_hudWinWidth.load() || m.winH != g_hudWinHeight.load());
    g_hudWinLeft.store(m.winLeft);
    g_hudWinTop.store(m.winTop);
    g_hudWinWidth.store(m.winW);
    g_hudWinHeight.store(m.winH);
    if (boundsChanged) {
        PositionHudWindow(g_hudHwnd, m.winLeft, m.winTop, m.winW, m.winH, true);
    }

    if (!EnsureSurface(g_hudSurface, m.winW, m.winH, cfg.colorSnow, cfg.hasCustomSnowColor)) {
        return;
    }

    float offX = static_cast<float>(m.winLeft - g_canvasLeft);
    float offY = static_cast<float>(m.winTop - g_canvasTop);
    RECT bindRect = {0, 0, m.winW, m.winH};
    g_hudSurface.rt->BindDC(g_hudSurface.memDC, &bindRect);
    g_hudSurface.rt->BeginDraw();
    g_hudSurface.rt->SetTransform(D2D1::Matrix3x2F::Identity());
    g_hudSurface.rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_hudSurface.rt->SetTransform(D2D1::Matrix3x2F::Translation(-offX, -offY));

    DrawForecastHudWidget(g_hudSurface, cfg, snap, fx, cloudPct01, moonPhase01, sunOn, moonOn, hour, m, timeSec);

    if (FAILED(g_hudSurface.rt->EndDraw())) {
        CleanupSurface(g_hudSurface);
        return;
    }
    PresentSurfaceAt(g_hudHwnd, g_hudSurface, m.winLeft, m.winTop, m.winW, m.winH);
}

float ComputeHorizontalSpeedPxPerSec(FallDirMode dirMode, PrecipKind kind, double mpsSpeed, int forecastSign, double forecastWindKmh) {
    float basePx = static_cast<float>(mpsSpeed) * 26.0f;
    if (kind == PrecipKind::Blizzard) basePx *= 1.85f;
    else if (kind == PrecipKind::Drizzle) basePx *= 1.25f;
    if (dirMode == FallDirMode::Down) return 0.0f;
    if (dirMode == FallDirMode::DownLeft) return -basePx;
    if (dirMode == FallDirMode::DownRight) return basePx;
    if (forecastSign == 0 || forecastWindKmh <= 0.2) return 0.0f;
    return static_cast<float>(forecastSign) * basePx * std::clamp(static_cast<float>(forecastWindKmh) / 18.0f, 0.30f, 2.8f);
}

float ComputeEffectiveFallSpeedMps(const Settings& cfg, const ForecastSnapshot& snap, PrecipKind kind) {
    if (cfg.precipSpeedMode == PrecipSpeedMode::Custom) {
        return static_cast<float>(cfg.precipFallSpeedMps);
    }
    double rainFactor = std::clamp((snap.valid ? snap.precipMm : 1.0) / 4.0, 0.45, 2.6);
    if (kind == PrecipKind::Drizzle) return static_cast<float>(std::clamp(1.5 + rainFactor * 0.6, 1.2, 2.8));
    if (kind == PrecipKind::Downpour) return static_cast<float>(std::clamp(10.5 + rainFactor * 5.0, 9.0, 22.0));
    if (kind == PrecipKind::Snow) return static_cast<float>(std::clamp(1.6 + rainFactor * 1.1, 1.2, 4.5));
    if (kind == PrecipKind::Blizzard) return static_cast<float>(std::clamp(2.4 + rainFactor * 1.4, 2.0, 6.0));
    if (kind == PrecipKind::Sleet) return static_cast<float>(std::clamp(3.8 + rainFactor * 2.2, 3.0, 8.5));
    if (kind == PrecipKind::Hail) return static_cast<float>(std::clamp(11.5 + rainFactor * 5.0, 9.5, 23.0));
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

    WNDCLASSEXW wcOverlay = {};
    wcOverlay.cbSize = sizeof(wcOverlay);
    wcOverlay.lpfnWndProc = OverlayWndProc;
    wcOverlay.hInstance = modInstance;
    wcOverlay.lpszClassName = kOverlayWndClass;
    if (!RegisterClassExW(&wcOverlay)) {
        Wh_Log(L"RegisterClassExW(overlay) failed: %u", GetLastError());
        return;
    }

    WNDCLASSEXW wcHud = {};
    wcHud.cbSize = sizeof(wcHud);
    wcHud.lpfnWndProc = HudWndProc;
    wcHud.hInstance = modInstance;
    wcHud.lpszClassName = kHudWndClass;
    if (!RegisterClassExW(&wcHud)) {
        Wh_Log(L"RegisterClassExW(hud) failed: %u", GetLastError());
        UnregisterClassW(wcOverlay.lpszClassName, modInstance);
        return;
    }

    std::mt19937 rng(static_cast<unsigned>(GetTickCount64()));
    auto startTime = std::chrono::steady_clock::now();
    auto lastTime = startTime;
    int prevLeft = INT32_MIN;
    int prevTop = INT32_MIN;
    int prevW = -1;
    int prevH = -1;
    HudWidgetMode prevWidgetMode = HudWidgetMode::FullScreenButton;
    std::wstring prevAttachMode;
    bool prevCursorNearSky = false;

    while (g_running.load()) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        Settings cfg;
        ForecastSnapshot snap;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            cfg = g_settings;
            snap = g_forecast;
        }
        UpdateCanvasGeometry(cfg);

        bool geometryChanged = (g_canvasLeft != prevLeft || g_canvasTop != prevTop ||
                                g_canvasWidth != prevW || g_canvasHeight != prevH ||
                                cfg.hudWidgetMode != prevWidgetMode ||
                                cfg.layerAttachMode != prevAttachMode);
        if (geometryChanged) {
            prevLeft = g_canvasLeft;
            prevTop = g_canvasTop;
            prevW = g_canvasWidth;
            prevH = g_canvasHeight;
            prevWidgetMode = cfg.hudWidgetMode;
            prevAttachMode = cfg.layerAttachMode;
        }

        bool fullScreenActive = (cfg.hudWidgetMode != HudWidgetMode::ButtonOnly);
        bool hudActive = (cfg.hudWidgetMode != HudWidgetMode::FullScreenOnly);

        bool occluded = (cfg.pauseOnFullscreen && IsFullscreenAppActive()) ||
                        RefreshDesktopOcclusionAndWindows(cfg.pauseOnFullscreen, fullScreenActive && cfg.windowBounceEnabled, geometryChanged);
        if (occluded) {
            if (g_overlayHwnd && IsWindowVisible(g_overlayHwnd)) ShowWindow(g_overlayHwnd, SW_HIDE);
            if (g_hudHwnd && IsWindowVisible(g_hudHwnd)) ShowWindow(g_hudHwnd, SW_HIDE);
            WaitWithMessagePump(300);
            lastTime = std::chrono::steady_clock::now();
            continue;
        }

        if (fullScreenActive) {
            if (!g_overlayHwnd) {
                g_overlayHwnd = CreateWindowExW(
                    WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                    wcOverlay.lpszClassName, L"DesktopWeatherOverlay", WS_POPUP,
                    g_canvasLeft, g_canvasTop, g_canvasWidth, g_canvasHeight,
                    nullptr, nullptr, modInstance, nullptr);
                PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode, true);
            } else if (geometryChanged || !IsWindowVisible(g_overlayHwnd)) {
                ShowWindow(g_overlayHwnd, SW_SHOWNA);
                PositionDesktopOverlay(g_overlayHwnd, cfg.layerAttachMode, true);
            }
        } else if (g_overlayHwnd) {
            DestroyWindow(g_overlayHwnd);
            g_overlayHwnd = nullptr;
            CleanupSurface(g_surface);
        }

        if (hudActive) {
            if (!g_hudHwnd) {
                g_hudHwnd = CreateWindowExW(
                    WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                    wcHud.lpszClassName, L"DesktopWeatherHud", WS_POPUP,
                    g_canvasLeft, g_canvasTop, 64, 64,
                    nullptr, nullptr, modInstance, nullptr);
                PositionHudWindow(g_hudHwnd, g_canvasLeft, g_canvasTop, 64, 64, true);
            } else if (geometryChanged || !IsWindowVisible(g_hudHwnd)) {
                ShowWindow(g_hudHwnd, SW_SHOWNA);
                PositionHudWindow(g_hudHwnd, g_hudWinLeft.load(), g_hudWinTop.load(),
                                  std::max(16, g_hudWinWidth.load()), std::max(16, g_hudWinHeight.load()), true);
            }
        } else if (g_hudHwnd) {
            DestroyWindow(g_hudHwnd);
            g_hudHwnd = nullptr;
            CleanupSurface(g_hudSurface);
        }

        bool hasCursor = false;
        float curRelX = 0.0f;
        float curRelY = 0.0f;
        POINT ptCur = {};
        if (GetCursorPos(&ptCur)) {
            hasCursor = true;
            curRelX = static_cast<float>(ptCur.x - g_canvasLeft);
            curRelY = static_cast<float>(ptCur.y - g_canvasTop);
        }

        auto now = std::chrono::steady_clock::now();
        float dt = std::clamp(std::chrono::duration<float>(now - lastTime).count(), 0.001f, 0.35f);
        double timeSec = std::chrono::duration<double>(now - startTime).count();
        lastTime = now;

        bool simEffects = fullScreenActive || (hudActive && g_hudExpanded.load());

        PrecipKind precipKind = PrecipKind::None;
        if (simEffects) {
            if (cfg.precipitationMode == PrecipMode::Auto) precipKind = snap.precipKind;
            else if (cfg.precipitationMode != PrecipMode::Off) precipKind = PrecipModeToKind(cfg.precipitationMode);
        }
        bool activePrecip = (precipKind != PrecipKind::None);

        auto resolveFx = [&](TriStateMode m, bool autoVal) -> bool {
            if (!simEffects) return false;
            return (m == TriStateMode::On) ? true : ((m == TriStateMode::Off) ? false : autoVal);
        };
        GeneralEffectsState activeFx;
        activeFx.dust = resolveFx(cfg.effectDust, snap.effects.dust);
        activeFx.fog = resolveFx(cfg.effectFog, snap.effects.fog);
        activeFx.smog = resolveFx(cfg.effectSmog, snap.effects.smog);
        activeFx.thunderstorm = resolveFx(cfg.effectThunderstorm, snap.effects.thunderstorm);
        activeFx.aurora = resolveFx(cfg.effectAurora, snap.effects.aurora);
        activeFx.mirage = resolveFx(cfg.effectMirage, snap.effects.mirage);
        activeFx.nightStars = resolveFx(cfg.effectNightStars, snap.effects.nightStars);
        activeFx.airQuality = resolveFx(cfg.effectAirQuality, snap.effects.airQuality);
        activeFx.pollen = resolveFx(cfg.effectPollen, snap.effects.pollen);
        if (simEffects) {
            if (cfg.effectThermal == TriStateMode::On) {
                activeFx.frost = snap.valid && snap.tempC <= 0.0;
                activeFx.heat = !activeFx.frost;
            } else if (cfg.effectThermal == TriStateMode::Auto) {
                activeFx.frost = snap.effects.frost;
                activeFx.heat = snap.effects.heat;
            }
        }

        double hour = GetLocalHourFloat();
        double effSunrise = cfg.customSunriseHour, effSunset = cfg.customSunsetHour;
        bool sunInSchedule = false;
        ResolveCelestialWindow(cfg, snap, true, hour, effSunrise, effSunset, sunInSchedule);

        double effMoonrise = cfg.customMoonriseHour, effMoonset = cfg.customMoonsetHour;
        bool moonInSchedule = false;
        ResolveCelestialWindow(cfg, snap, false, hour, effMoonrise, effMoonset, moonInSchedule);

        float cloudPct01 = 0.0f;
        if (simEffects) {
            if (cfg.cloudsMode == TriStateMode::On) cloudPct01 = static_cast<float>(cfg.cloudsIntensity) / 100.0f;
            else if (cfg.cloudsMode == TriStateMode::Auto && snap.valid) cloudPct01 = static_cast<float>(snap.cloudCoverPct / 100.0);
        }
        cloudPct01 = std::clamp(cloudPct01, 0.0f, 1.0f);

        bool sunActive = simEffects && ((cfg.sunMode == TriStateMode::On) || (cfg.sunMode == TriStateMode::Auto && sunInSchedule));
        bool moonActive = simEffects && ((cfg.moonMode == TriStateMode::On) || (cfg.moonMode == TriStateMode::Auto && moonInSchedule));
        float uvIndex = static_cast<float>(snap.uvIndex);
        double moonPhase01 = ComputeRealMoonPhase();

        QualityTier tierPrecip = ResolveCategoryTier(cfg, cfg.perfPrecip);
        QualityTier tierEffects = ResolveCategoryTier(cfg, cfg.perfEffects);
        QualityTier tierCelestial = ResolveCategoryTier(cfg, cfg.perfCelestial);

        if (activeFx.thunderstorm) {
            float flashProb = (tierEffects == QualityTier::High)
                ? 0.45f
                : ((tierEffects == QualityTier::Low) ? 0.20f : 0.35f);
            if (g_lightning.timer > 0.0f) {
                g_lightning.timer = std::max(0.0f, g_lightning.timer - dt);
            } else if ((static_cast<float>(rng() % 10000) / 10000.0f) < flashProb * dt) {
                g_lightning.timer = 0.22f;
                const MonitorCanvasRect& mon = g_monitorRects.front();
                float lMin = mon.left + mon.innerLeftRel + 20.0f;
                float lMax = std::max(lMin + 40.0f, mon.left + mon.innerRightRel - 20.0f);
                g_lightning.xStart = lMin + static_cast<float>(rng() % std::max(20, static_cast<int>(lMax - lMin)));
                g_lightning.xEnd = g_lightning.xStart + static_cast<float>((rng() % 160) - 80);
            }
        } else {
            g_lightning.timer = 0.0f;
        }

        float effSpeedMps = ComputeEffectiveFallSpeedMps(cfg, snap, precipKind);
        float precipVx = ComputeHorizontalSpeedPxPerSec(cfg.precipFallDirection, precipKind, effSpeedMps, snap.windDirSign, snap.windSpeedKmh);
        float precipVy = effSpeedMps * 42.0f;
        float windCloudVx = (snap.windDirSign != 0 && snap.windSpeedKmh > 0.2)
            ? static_cast<float>(snap.windDirSign) * static_cast<float>(snap.windSpeedKmh) * 1.4f
            : precipVx;

        float kindDensityMul = 1.0f;
        if (precipKind == PrecipKind::Drizzle) kindDensityMul = 0.65f;
        else if (precipKind == PrecipKind::Downpour) kindDensityMul = 1.65f;
        else if (precipKind == PrecipKind::Blizzard) kindDensityMul = 1.50f;

        size_t targetP = activePrecip
            ? static_cast<size_t>((g_canvasWidth / 1000.0) * cfg.precipIntensity * 1.4 * kindDensityMul * GetTierScale(tierPrecip))
            : 0;
        size_t prevCount = g_falling.size();
        if (g_falling.size() != targetP) g_falling.resize(targetP);
        for (size_t i = 0; i < targetP; ++i) {
            if (i >= prevCount || g_falling[i].kind != precipKind) {
                SpawnFallingItem(g_falling[i], rng, precipKind, precipVx, precipVy, true, tierPrecip);
            }
        }

        for (size_t i = 0; i < g_falling.size(); ++i) {
            FallingItem& p = g_falling[i];
            if (p.hitAnimTimer > 0.0f) {
                p.hitAnimTimer -= dt;
                if (p.hitAnimTimer <= 0.0f) {
                    SpawnFallingItem(p, rng, p.kind, precipVx, precipVy, false, tierPrecip);
                }
                continue;
            }
            if (!p.slidingSide) {
                p.vx = precipVx;
                p.vy = (p.kind == PrecipKind::Sleet && !p.isFlakeSub) ? (precipVy * 1.25f) : precipVy;
            } else {
                p.vy = std::max(precipVy * 0.75f, p.vy + 220.0f * dt);
            }
            float swayFreq = (p.kind == PrecipKind::Blizzard) ? 6.5f : ((tierPrecip == QualityTier::Low) ? 1.5f : 3.0f);
            p.swayPhase += dt * swayFreq;
            p.angle += p.spin * dt * 20.0f;
            float swayAmp = (p.kind == PrecipKind::Blizzard) ? 34.0f : ((tierPrecip == QualityTier::Low) ? 8.0f : 18.0f);
            float sway = (!p.isFlakeSub || p.slidingSide) ? 0.0f : std::sin(p.swayPhase) * swayAmp;
            float prevX = p.x;
            float prevY = p.y;
            p.x += (p.vx + sway) * dt;
            p.y += p.vy * dt;

            if (fullScreenActive && cfg.windowBounceEnabled && !p.slidingSide && p.vy > 0.0f) {
                for (const auto& wb : g_bounceWindows) {
                    if (p.x >= wb.left && p.x <= wb.right && prevY <= wb.top && p.y >= wb.top) {
                        p.y = wb.top;
                        p.hitAnimTimer = 0.22f;
                        break;
                    }
                    if (p.y >= wb.top && p.y <= wb.bottom) {
                        if (prevX < wb.left && p.x >= wb.left && p.vx > 0.0f) {
                            p.x = wb.left - 2.0f;
                            p.slidingSide = true;
                            p.vx = -std::max(14.0f, std::abs(p.vx) * 0.22f);
                            p.vy = std::max(90.0f, p.vy * 0.85f);
                            break;
                        }
                        if (prevX > wb.right && p.x <= wb.right && p.vx < 0.0f) {
                            p.x = wb.right + 2.0f;
                            p.slidingSide = true;
                            p.vx = std::max(14.0f, std::abs(p.vx) * 0.22f);
                            p.vy = std::max(90.0f, p.vy * 0.85f);
                            break;
                        }
                    }
                }
            }

            const MonitorCanvasRect& mon = g_monitorRects.front();
            float minX = mon.left + mon.innerLeftRel - 48.0f;
            float maxX = mon.left + mon.innerRightRel + 48.0f;
            float maxY = mon.top + mon.bottomLimitRelY + 6.0f;
            if (p.hitAnimTimer <= 0.0f && (p.y > maxY || p.x < minX || p.x > maxX)) {
                SpawnFallingItem(p, rng, p.kind, precipVx, precipVy, false, tierPrecip);
            }
        }

        int bottomLimitY = ComputeBottomLimitY(cfg);
        bool cursorNearSky = false;
        if (fullScreenActive) {
            RenderOverlayFrame(cfg, snap, activeFx, cloudPct01, uvIndex, moonPhase01,
                               sunActive, moonActive, hour, bottomLimitY, windCloudVx, timeSec,
                               hasCursor, curRelX, curRelY, cursorNearSky);
        }
        if (hudActive) {
            RenderHudFrame(cfg, snap, activeFx, cloudPct01, moonPhase01,
                           sunActive, moonActive, hour, bottomLimitY, timeSec);
        }

        bool hoverTransition = (cursorNearSky != prevCursorNearSky);
        prevCursorNearSky = cursorNearSky;
        bool draggingNow = false;
        {
            std::lock_guard<std::mutex> lock(g_hudUiMutex);
            draggingNow = g_draggingHudBtn;
        }

        bool needsFastFps = !g_falling.empty() ||
                            (g_lightning.timer > 0.0f) ||
                            (hudActive && (g_hudExpanded.load() || draggingNow)) ||
                            hoverTransition;
        bool needsMediumFps = fullScreenActive && sunActive && (tierCelestial != QualityTier::Low);

        DWORD waitMs = 300;
        if (needsFastFps) {
            waitMs = static_cast<DWORD>(1000 / cfg.targetFps);
        } else if (needsMediumFps) {
            waitMs = 55;
        }
        WaitWithMessagePump(waitMs);
    }

    if (g_overlayHwnd) {
        DestroyWindow(g_overlayHwnd);
        g_overlayHwnd = nullptr;
    }
    if (g_hudHwnd) {
        DestroyWindow(g_hudHwnd);
        g_hudHwnd = nullptr;
    }
    CleanupSurface(g_surface);
    CleanupSurface(g_hudSurface);
    CleanupDirectWrite();
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    UnregisterClassW(wcHud.lpszClassName, modInstance);
    UnregisterClassW(wcOverlay.lpszClassName, modInstance);
}

} // namespace

BOOL WhTool_ModInit() {
    LoadSettingsFromWh(true);
    g_running.store(true);
    g_weatherWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_weatherThread.emplace(WeatherWorkerLoop);
    g_renderThread.emplace(RenderWorkerLoop);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettingsFromWh(false);
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
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
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
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod process already running");
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