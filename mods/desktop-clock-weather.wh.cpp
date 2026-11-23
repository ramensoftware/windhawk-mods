// ==WindhawkMod==
// @id              desktop-clock-weather
// @name            Desktop Clock & Weather
// @description     Time, weekday, date and weekly weather on the desktop - behind icons, not clickable
// @version         1.0.6
// @author          DineroOne
// @license         MIT
// @github          https://github.com/DineroOne
// @include         explorer.exe
// @compilerOptions -lgdi32 -lgdiplus -lwinhttp -luser32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Clock & Weather
![Illustration of the widget](https://raw.githubusercontent.com/DineroOne/windhawk-desktop-clock-weather/main/screenshot-small.png)

The widget is drawn directly on the desktop: **above the wallpaper but below icons and folders**,
so it never intercepts clicks.

Shows time, weekday, date and a 3-7 day weather forecast.

Weather comes from [Open-Meteo](https://open-meteo.com) (free, no API key needed).
Set the city name in the settings; coordinates are found automatically.

Everything (position, size, font, colors, shadow, background, date format, etc.) is configurable in the settings tab.

The optional **Adjust font color to wallpaper** switch picks a light or a dark text color automatically,
depending on how bright the wallpaper is behind the widget. It reads the wallpaper file locally; nothing is sent anywhere.

## Network use
Weather is the only feature that uses the network, and only after you enter a city in the settings.
The mod then sends HTTPS requests to two Open-Meteo hosts:
- `geocoding-api.open-meteo.com` - once whenever the city changes, to turn the city name into coordinates (the city name and the language code are sent).
- `api.open-meteo.com` - at start and then every N minutes (30 by default, see **Update weather every**), to get the forecast (the coordinates are sent).

Like any web server, Open-Meteo can see your IP address. No API key, account or other personal data is sent.

No requests are made while the city setting is empty or **Show weather** is off.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: topright
  $name: Position on screen
  $options:
  - topleft: Top left
  - topcenter: Top center
  - topright: Top right
  - middleleft: Middle left
  - center: Center
  - middleright: Middle right
  - bottomleft: Bottom left
  - bottomcenter: Bottom center
  - bottomright: Bottom right
- marginX: 40
  $name: Horizontal margin from screen edge (px)
- marginY: 40
  $name: Vertical margin from screen edge (px)
- monitor: 0
  $name: Monitor
  $description: 0 = primary, 1 = next monitor, and so on.
- scale: 100
  $name: Widget size (%)
  $description: Resizes the whole widget at once (text, icons, spacing). 100 = normal, 150 = one and a half times bigger, 50 = half size. Range 25-400.
- textAlign: right
  $name: Text alignment
  $options:
  - left: Left
  - center: Center
  - right: Right
- order: time,weekday,date,weather
  $name: Order of elements (top to bottom)
  $description: Comma-separated list of time, weekday, date, weather. For example "weather,time,weekday,date" puts the weather on top. Elements you leave out are hidden.
- showTime: true
  $name: Show time
- showSeconds: false
  $name: Show seconds
- use24h: true
  $name: 24-hour clock
- showWeekday: true
  $name: Show weekday
- uppercaseWeekday: false
  $name: UPPERCASE weekday
- showDate: true
  $name: Show date
- dateFormat: d MMMM yyyy
  $name: Date format
  $description: Windows date pattern, e.g. "dd.MM.yyyy", "d MMMM yyyy", "ddd, d MMM".
- locale: ""
  $name: Language for dates and weekdays
  $description: Leave empty to use the system language. Examples - en-US, pl-PL, ru-RU.
- fontFamily: Segoe UI
  $name: Default font
  $description: Used for every element whose own font below is empty. For example "Segoe UI Light", "Segoe UI Semilight", "Bahnschrift", "Consolas".
- timeFont: ""
  $name: Time font
  $description: Leave empty to use the default font.
- weekdayFont: ""
  $name: Weekday font
  $description: Leave empty to use the default font.
- dateFont: ""
  $name: Date font
  $description: Leave empty to use the default font.
- weatherFont: ""
  $name: Weather font
  $description: Leave empty to use the default font. Also applies to the day names in the forecast.
- bold: false
  $name: Bold
- italic: false
  $name: Italic
- timeSize: 96
  $name: Font size - time (px)
- weekdaySize: 36
  $name: Font size - weekday (px)
- dateSize: 24
  $name: Font size - date (px)
- weatherSize: 18
  $name: Font size - weather (px)
- iconSize: 30
  $name: Weather icon size (px)
- timeColor: "#FFFFFF"
  $name: Time color
- weekdayColor: "#FFD36B"
  $name: Weekday color
- dateColor: "#FFFFFF"
  $name: Date color
- weatherColor: "#FFFFFF"
  $name: Weather color
- adaptColor: false
  $name: Adjust font color to wallpaper
  $description: Chooses one of the two text colors below automatically, depending on how bright the wallpaper is behind the widget. While it is on, it replaces the individual text colors above.
- adaptLightText: "#FFFFFF"
  $name: Text color on dark wallpaper
- adaptDarkText: "#1E1E1E"
  $name: Text color on light wallpaper
- textOpacity: 100
  $name: Text opacity (0-100)
- shadow: true
  $name: Text shadow
- shadowColor: "#000000"
  $name: Shadow color
- shadowOpacity: 60
  $name: Shadow opacity (0-100)
- shadowOffset: 2
  $name: Shadow offset (px)
- bgColor: "#000000"
  $name: Background panel color
- bgOpacity: 0
  $name: Background panel opacity (0 = no panel)
- cornerRadius: 16
  $name: Background corner radius (px)
- padding: 12
  $name: Inner padding (px)
- lineGap: 6
  $name: Gap between lines (px)
- showWeather: true
  $name: Show weather
- city: ""
  $name: City for weather
  $description: Enter your city, in any language (for example Warsaw), to enable weather. No network requests are made until a city is set. If it cannot be found, "city not found" is shown.
- tempUnit: celsius
  $name: Temperature unit
  $options:
  - celsius: Celsius
  - fahrenheit: Fahrenheit
- forecastDays: 7
  $name: Forecast days (3-7)
- weatherLayout: horizontal
  $name: Forecast layout
  $options:
  - horizontal: Row (days as columns)
  - vertical: Column (days as rows)
- showIcons: true
  $name: Weather icons
- showPrecip: true
  $name: Precipitation probability
- showCityLine: true
  $name: City and current temperature line
- iconFont: Segoe UI Symbol
  $name: Weather icon font
- updateMinutes: 30
  $name: Update weather every (minutes)
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>

#undef min
#undef max
using std::min;
using std::max;

#include <gdiplus.h>
#include <winhttp.h>
using namespace Gdiplus;

#define WM_APP_RENDER (WM_APP + 1)

// ─────────────────────────────────────────────────────────────────────────────
//  Settings
// ─────────────────────────────────────────────────────────────────────────────

struct Settings {
    std::wstring position    = L"topright";
    std::wstring textAlign   = L"right";
    std::wstring locale;
    std::wstring dateFormat  = L"d MMMM yyyy";
    std::wstring fontFamily  = L"Segoe UI";
    std::wstring iconFont    = L"Segoe UI Symbol";
    std::wstring timeFont, weekdayFont, dateFont, weatherFont;
    std::wstring order       = L"time,weekday,date,weather";
    std::wstring timeColor   = L"#FFFFFF";
    std::wstring weekdayColor = L"#FFD36B";
    std::wstring dateColor   = L"#FFFFFF";
    std::wstring weatherColor = L"#FFFFFF";
    std::wstring shadowColor = L"#000000";
    std::wstring bgColor     = L"#000000";
    std::wstring adaptLightText = L"#FFFFFF";
    std::wstring adaptDarkText  = L"#1E1E1E";
    std::wstring city;
    std::wstring tempUnit      = L"celsius";
    std::wstring weatherLayout = L"horizontal";

    bool adaptColor       = false;
    int  marginX          = 40;
    int  marginY          = 40;
    int  monitor          = 0;
    int  scale            = 100;
    int  timeSize         = 96;
    int  weekdaySize      = 36;
    int  dateSize         = 24;
    int  weatherSize      = 18;
    int  iconSize         = 30;
    int  textOpacity      = 100;
    int  shadowOpacity    = 60;
    int  shadowOffset     = 2;
    int  bgOpacity        = 0;
    int  cornerRadius     = 16;
    int  padding          = 12;
    int  lineGap          = 6;
    int  forecastDays     = 7;
    int  updateMinutes    = 30;

    bool showTime         = true;
    bool showSeconds      = false;
    bool use24h           = true;
    bool showWeekday      = true;
    bool uppercaseWeekday = false;
    bool showDate         = true;
    bool bold             = false;
    bool italic           = false;
    bool shadow           = true;
    bool showWeather      = true;
    bool showIcons        = true;
    bool showPrecip       = true;
    bool showCityLine     = true;
};

struct Day {
    int    y = 0, m = 0, d = 0;
    int    code = 0;
    int    precip = -1;
    double tmax = NAN, tmin = NAN;
};

struct Weather {
    bool         valid  = false;
    bool         hasCur = false;
    double       cur    = 0;
    std::wstring place;
    std::wstring status = L"Weather: loading...";
    std::vector<Day> days;
};

static CRITICAL_SECTION g_cs;
static Settings         g_s;
static Weather          g_w;
static HANDLE           g_evStop, g_evRefresh, g_thUi, g_thWx;
static std::atomic<HWND> g_hwnd{nullptr};
static PVOID volatile   g_req = nullptr;  // in-flight WinHTTP request; closed by Wh_ModUninit to abort it
static volatile LONG    g_dirty = 1;
static LONG             g_lastKey = -1;

static const wchar_t kClass[] = L"WhDesktopClockWeatherWnd";

static std::wstring GetS(PCWSTR n) {
    PCWSTR v = Wh_GetStringSetting(n);
    std::wstring r = v ? v : L"";
    Wh_FreeStringSetting(v);
    return r;
}

static int GetI(PCWSTR n) {
    return Wh_GetIntSetting(n);
}

static void LoadSettings() {
    Settings s;

    s.position       = GetS(L"position");
    s.textAlign      = GetS(L"textAlign");
    s.locale         = GetS(L"locale");
    s.dateFormat     = GetS(L"dateFormat");
    s.fontFamily     = GetS(L"fontFamily");
    s.iconFont       = GetS(L"iconFont");
    s.timeFont       = GetS(L"timeFont");
    s.weekdayFont    = GetS(L"weekdayFont");
    s.dateFont       = GetS(L"dateFont");
    s.weatherFont    = GetS(L"weatherFont");
    s.order          = GetS(L"order");
    s.timeColor      = GetS(L"timeColor");
    s.weekdayColor   = GetS(L"weekdayColor");
    s.dateColor      = GetS(L"dateColor");
    s.weatherColor   = GetS(L"weatherColor");
    s.shadowColor    = GetS(L"shadowColor");
    s.bgColor        = GetS(L"bgColor");
    s.adaptLightText = GetS(L"adaptLightText");
    s.adaptDarkText  = GetS(L"adaptDarkText");
    s.adaptColor     = GetI(L"adaptColor") != 0;
    s.city           = GetS(L"city");
    s.tempUnit       = GetS(L"tempUnit");
    s.weatherLayout  = GetS(L"weatherLayout");
    s.marginX        = GetI(L"marginX");
    s.marginY        = GetI(L"marginY");
    s.monitor        = GetI(L"monitor");
    s.scale          = GetI(L"scale");
    s.timeSize       = GetI(L"timeSize");
    s.weekdaySize    = GetI(L"weekdaySize");
    s.dateSize       = GetI(L"dateSize");
    s.weatherSize    = GetI(L"weatherSize");
    s.iconSize       = GetI(L"iconSize");
    s.textOpacity    = GetI(L"textOpacity");
    s.shadowOpacity  = GetI(L"shadowOpacity");
    s.shadowOffset   = GetI(L"shadowOffset");
    s.bgOpacity      = GetI(L"bgOpacity");
    s.cornerRadius   = GetI(L"cornerRadius");
    s.padding        = GetI(L"padding");
    s.lineGap        = GetI(L"lineGap");
    s.forecastDays   = std::clamp(GetI(L"forecastDays"), 3, 7);
    s.updateMinutes  = std::clamp(GetI(L"updateMinutes"), 5, 1440);
    s.showTime       = GetI(L"showTime") != 0;
    s.showSeconds    = GetI(L"showSeconds") != 0;
    s.use24h         = GetI(L"use24h") != 0;
    s.showWeekday    = GetI(L"showWeekday") != 0;
    s.uppercaseWeekday = GetI(L"uppercaseWeekday") != 0;
    s.showDate       = GetI(L"showDate") != 0;
    s.bold           = GetI(L"bold") != 0;
    s.italic         = GetI(L"italic") != 0;
    s.shadow         = GetI(L"shadow") != 0;
    s.showWeather    = GetI(L"showWeather") != 0;
    s.showIcons      = GetI(L"showIcons") != 0;
    s.showPrecip     = GetI(L"showPrecip") != 0;
    s.showCityLine   = GetI(L"showCityLine") != 0;

    EnterCriticalSection(&g_cs);
    g_s = s;
    LeaveCriticalSection(&g_cs);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────────────────────

static std::wstring Widen(const std::string& s) {
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

static std::string Narrow(const std::wstring& s) {
    if (s.empty()) return "";
    int n = WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0, nullptr, nullptr);
    std::string r(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.size(), r.data(), n, nullptr, nullptr);
    return r;
}

static std::string UrlEnc(const std::string& s) {
    static const char* hex = "0123456789ABCDEF";
    std::string o;
    for (unsigned char c : s) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            o += (char)c;
        } else {
            o += '%';
            o += hex[c >> 4];
            o += hex[c & 15];
        }
    }
    return o;
}

static Color MakeColor(const std::wstring& str, int opacityPct) {
    unsigned v = 0xFFFFFF;
    const wchar_t* p = str.c_str();
    while (*p == L' ' || *p == L'#') p++;
    wchar_t* e = nullptr;
    unsigned long x = wcstoul(p, &e, 16);
    if (e != p) v = (unsigned)x;
    int a = std::clamp(opacityPct, 0, 100) * 255 / 100;
    return Color((BYTE)a, (BYTE)((v >> 16) & 255), (BYTE)((v >> 8) & 255), (BYTE)(v & 255));
}

static const wchar_t* IconFor(int c) {
    if (c == 0)                                     return L"\u2600";
    if (c == 1)                                     return L"\U0001F324";
    if (c == 2)                                     return L"\u26C5";
    if (c == 3)                                     return L"\u2601";
    if (c == 45 || c == 48)                         return L"\U0001F32B";
    if (c >= 51 && c <= 57)                         return L"\U0001F326";
    if ((c >= 61 && c <= 67) || (c >= 80 && c <= 82)) return L"\U0001F327";
    if ((c >= 71 && c <= 77) || c == 85 || c == 86) return L"\u2744";
    if (c >= 95)                                    return L"\u26C8";
    return L"\u2601";
}

// ─────────────────────────────────────────────────────────────────────────────
//  HTTP + JSON (Open-Meteo)
// ─────────────────────────────────────────────────────────────────────────────

static bool HttpGet(const wchar_t* host, const std::wstring& path, std::string& out) {
    if (WaitForSingleObject(g_evStop, 0) == WAIT_OBJECT_0) return false;

    bool ok = false;
    HINTERNET ses = WinHttpOpen(L"WindhawkDesktopClockWeather/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!ses) return false;

    WinHttpSetTimeouts(ses, 4000, 4000, 4000, 6000);

    HINTERNET con = WinHttpConnect(ses, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET req = con ? WinHttpOpenRequest(con, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                             WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE) : nullptr;
    if (req) InterlockedExchangePointer(&g_req, req);

    if (req &&
        WinHttpSendRequest(req, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(req, nullptr)) {
        DWORD code = 0, sz = sizeof(code);
        WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &code, &sz, WINHTTP_NO_HEADER_INDEX);
        if (code == 200) {
            ok = true;
            DWORD avail = 0;
            while (WinHttpQueryDataAvailable(req, &avail) && avail && out.size() < 262144) {
                std::string buf(avail, '\0');
                DWORD got = 0;
                if (!WinHttpReadData(req, buf.data(), avail, &got) || !got) break;
                out.append(buf.data(), got);
            }
        }
    }

    // otherwise an abort already closed it
    if (req && InterlockedExchangePointer(&g_req, nullptr) == req) WinHttpCloseHandle(req);
    if (con) WinHttpCloseHandle(con);
    WinHttpCloseHandle(ses);
    return ok && !out.empty();
}

static bool NumAfter(const std::string& j, const char* key, size_t from, double& v) {
    std::string k = std::string("\"") + key + "\":";
    size_t p = j.find(k, from);
    if (p == std::string::npos) return false;
    p += k.size();
    char* e = nullptr;
    v = strtod(j.c_str() + p, &e);
    return e != j.c_str() + p;
}

static bool JsonStr(const std::string& j, const char* key, size_t from, std::wstring& out) {
    std::string k = std::string("\"") + key + "\":\"";
    size_t p = j.find(k, from);
    if (p == std::string::npos) return false;
    p += k.size();

    std::string raw;
    out.clear();

    auto flush = [&] {
        out += Widen(raw);
        raw.clear();
    };

    for (; p < j.size(); p++) {
        char c = j[p];
        if (c == '"') break;
        if (c == '\\' && p + 1 < j.size()) {
            char n = j[++p];
            if (n == 'u' && p + 4 < j.size()) {
                flush();
                out += (wchar_t)strtoul(j.substr(p + 1, 4).c_str(), nullptr, 16);
                p += 4;
            } else {
                raw += (n == 'n' ? ' ' : n);
            }
        } else {
            raw += c;
        }
    }
    flush();
    return true;
}

static std::vector<std::string> Arr(const std::string& j, const char* key, size_t from) {
    std::vector<std::string> r;
    std::string k = std::string("\"") + key + "\":[";
    size_t p = j.find(k, from);
    if (p == std::string::npos) return r;
    p += k.size();
    size_t q = j.find(']', p);
    if (q == std::string::npos) return r;

    std::string cur;
    for (size_t i = p; i <= q; i++) {
        char c = i < q ? j[i] : ',';
        if (c == ',') {
            r.push_back(cur);
            cur.clear();
        } else if (c != '"' && c != ' ') {
            cur += c;
        }
    }
    return r;
}

static double Num(const std::string& s) {
    char* e = nullptr;
    double v = strtod(s.c_str(), &e);
    return e == s.c_str() ? NAN : v;
}

static std::wstring g_geoCity, g_geoPlace;
static double       g_lat = 0, g_lon = 0;
static bool         g_geoOk = false;

static std::string LangCode(const Settings& s) {
    wchar_t b[LOCALE_NAME_MAX_LENGTH] = {};
    if (!s.locale.empty()) wcsncpy(b, s.locale.c_str(), LOCALE_NAME_MAX_LENGTH - 1);
    else GetUserDefaultLocaleName(b, LOCALE_NAME_MAX_LENGTH);
    std::wstring r = b;
    size_t p = r.find(L'-');
    if (p != std::wstring::npos) r = r.substr(0, p);
    if (r.empty()) r = L"en";
    return Narrow(r);
}

static bool FetchWeather(const Settings& s) {
    auto fail = [&](const wchar_t* msg) {
        EnterCriticalSection(&g_cs);
        if (!g_w.valid) g_w.status = msg;
        LeaveCriticalSection(&g_cs);
        return false;
    };

    if (s.city.empty()) return fail(L"Weather: set a city in settings");

    if (!g_geoOk || g_geoCity != s.city) {
        std::string body;
        std::string path = "/v1/search?name=" + UrlEnc(Narrow(s.city)) +
                           "&count=1&format=json&language=" + LangCode(s);
        if (!HttpGet(L"geocoding-api.open-meteo.com", Widen(path), body))
            return fail(L"Weather: no connection");

        size_t r = body.find("\"results\":[");
        double la = 0, lo = 0;
        if (r == std::string::npos ||
            !NumAfter(body, "latitude", r, la) ||
            !NumAfter(body, "longitude", r, lo))
            return fail(L"Weather: city not found");

        std::wstring nm;
        if (!JsonStr(body, "name", r, nm) || nm.empty()) nm = s.city;
        g_lat = la;
        g_lon = lo;
        g_geoPlace = nm;
        g_geoCity  = s.city;
        g_geoOk    = true;
    }

    char q[512];
    snprintf(q, sizeof q,
             "/v1/forecast?latitude=%.4f&longitude=%.4f&current=temperature_2m,weather_code"
             "&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max"
             "&timezone=auto&forecast_days=7%s",
             g_lat, g_lon, s.tempUnit == L"fahrenheit" ? "&temperature_unit=fahrenheit" : "");

    std::string body;
    if (!HttpGet(L"api.open-meteo.com", Widen(q), body)) return fail(L"Weather: no connection");

    size_t d = body.find("\"daily\":{");
    if (d == std::string::npos) return fail(L"Weather: bad response");

    auto t  = Arr(body, "time", d);
    auto wc = Arr(body, "weather_code", d);
    auto mx = Arr(body, "temperature_2m_max", d);
    auto mn = Arr(body, "temperature_2m_min", d);
    auto pp = Arr(body, "precipitation_probability_max", d);

    Weather nw;
    nw.valid = true;
    nw.place = g_geoPlace;

    for (size_t i = 0; i < t.size() && i < 7; i++) {
        Day dy;
        if (sscanf(t[i].c_str(), "%d-%d-%d", &dy.y, &dy.m, &dy.d) != 3) continue;

        double v = i < wc.size() ? Num(wc[i]) : NAN;
        dy.code  = std::isnan(v) ? 0 : (int)v;
        dy.tmax  = i < mx.size() ? Num(mx[i]) : NAN;
        dy.tmin  = i < mn.size() ? Num(mn[i]) : NAN;
        v        = i < pp.size() ? Num(pp[i]) : NAN;
        dy.precip = std::isnan(v) ? -1 : (int)std::lround(v);

        nw.days.push_back(dy);
    }

    if (nw.days.empty()) return fail(L"Weather: bad response");

    size_t c = body.find("\"current\":{");
    if (c != std::string::npos) {
        double a = 0;
        if (NumAfter(body, "temperature_2m", c, a)) {
            nw.hasCur = true;
            nw.cur = a;
        }
    }

    EnterCriticalSection(&g_cs);
    g_w = nw;
    LeaveCriticalSection(&g_cs);
    return true;
}

static DWORD WINAPI WeatherThread(LPVOID) {
    for (;;) {
        Settings s;
        EnterCriticalSection(&g_cs);
        s = g_s;
        LeaveCriticalSection(&g_cs);

        DWORD wait = INFINITE;
        if (s.showWeather && (!g_hwnd || GetSystemMetrics(SM_SHUTTINGDOWN))) {
            // no window yet, or not the shell explorer.exe, or shutting down: no network use
            wait = 3000;
        } else if (s.showWeather) {
            bool ok = FetchWeather(s);
            wait = ok ? (DWORD)s.updateMinutes * 60000
                      : (s.city.empty() ? INFINITE : 60000);
            HWND h = g_hwnd;
            if (h) PostMessageW(h, WM_APP_RENDER, 0, 0);
        }

        HANDLE hs[2] = {g_evStop, g_evRefresh};
        if (WaitForMultipleObjects(2, hs, FALSE, wait) == WAIT_OBJECT_0) return 0;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Rendering
// ─────────────────────────────────────────────────────────────────────────────

struct MonList {
    std::vector<MONITORINFO> v;
};

static BOOL CALLBACK MonEnum(HMONITOR m, HDC, LPRECT, LPARAM l) {
    auto* ml = (MonList*)l;
    MONITORINFO mi;
    memset(&mi, 0, sizeof mi);
    mi.cbSize = sizeof mi;
    if (GetMonitorInfoW(m, &mi)) {
        if (mi.dwFlags & MONITORINFOF_PRIMARY) ml->v.insert(ml->v.begin(), mi);
        else ml->v.push_back(mi);
    }
    return TRUE;
}

static std::unique_ptr<Font> MakeFont(const std::wstring& fam, float size, int style) {
    FontFamily ff(fam.c_str());
    if (ff.GetLastStatus() != Ok) {
        FontFamily fb(L"Segoe UI");
        return std::make_unique<Font>(&fb, size, style, UnitPixel);
    }
    auto f = std::make_unique<Font>(&ff, size, style, UnitPixel);
    if (!f->IsAvailable()) f = std::make_unique<Font>(&ff, size, (int)FontStyleRegular, UnitPixel);
    return f;
}

static LONG TimeKey(const SYSTEMTIME& st, bool sec) {
    return ((st.wDay * 24 + st.wHour) * 60 + st.wMinute) * 60 + (sec ? st.wSecond : 0);
}

static std::vector<std::wstring> ParseOrder(const std::wstring& in) {
    std::vector<std::wstring> r;
    std::wstring t;
    for (size_t i = 0; i <= in.size(); i++) {
        wchar_t c = i < in.size() ? in[i] : L',';
        if (c == L',' || c == L' ' || c == L';') {
            bool known = t == L"time" || t == L"weekday" || t == L"date" || t == L"weather";
            if (known && std::find(r.begin(), r.end(), t) == r.end()) r.push_back(t);
            t.clear();
        } else {
            t += (c >= L'A' && c <= L'Z') ? (wchar_t)(c + 32) : c;
        }
    }
    if (r.empty()) r = {L"time", L"weekday", L"date", L"weather"};
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Wallpaper brightness (for "Adjust font color to wallpaper")
// ─────────────────────────────────────────────────────────────────────────────

static const int kTw = 192, kTh = 108;  // luminance map of the wallpaper, 192x108 samples

struct WpMap {
    std::wstring       key;
    int                iw = 0, ih = 0;
    std::vector<float> luma;
};

static WpMap g_wp;
static bool  g_wpLight = false;  // last decision, kept for hysteresis

static std::wstring RegStr(const wchar_t* sub, const wchar_t* name) {
    wchar_t b[MAX_PATH * 2] = {};
    DWORD sz = sizeof(b) - sizeof(wchar_t);
    if (RegGetValueW(HKEY_CURRENT_USER, sub, name, RRF_RT_REG_SZ, nullptr, b, &sz) != ERROR_SUCCESS)
        return L"";
    return b;
}

static float Luma(float r, float g, float b) {
    return (0.2126f * r + 0.7152f * g + 0.0722f * b) / 255.f;
}

// Fallback: brightness of the pixels actually on screen behind the widget. Without CAPTUREBLT the layered
// widget itself is left out of the copy, but windows covering the desktop are not.
// Returns -1 when the capture fails.
static float ScreenBrightness(const RECT& wr) {
    const int w = wr.right - wr.left, h = wr.bottom - wr.top;
    if (w < 4 || h < 4) return -1.f;

    // Ignore screen pixels covered by application windows. Otherwise a bright
    // window over the widget could be mistaken for bright wallpaper. Returning
    // -1 keeps the last decision until the desktop is visible again.
    const int N = 12;
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            POINT pt = {
                wr.left + (int)((wr.right - wr.left) * (i + 0.5f) / N),
                wr.top  + (int)((wr.bottom - wr.top) * (j + 0.5f) / N)
            };
            HWND under = WindowFromPoint(pt);
            HWND root = under ? GetAncestor(under, GA_ROOT) : nullptr;
            if (!root) root = under;
            if (!root) continue;
            wchar_t cls[64] = {};
            GetClassNameW(root, cls, (int)(sizeof(cls) / sizeof(cls[0])));
            if (wcscmp(cls, L"Progman") != 0 && wcscmp(cls, L"WorkerW") != 0)
                return -2.f;  // distinct from capture failure
        }
    }

    HDC scr = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(scr);
    HBITMAP bmp = CreateCompatibleBitmap(scr, w, h);
    float res = -1.f;

    if (bmp) {
        HGDIOBJ old = SelectObject(mem, bmp);
        if (BitBlt(mem, 0, 0, w, h, scr, wr.left, wr.top, SRCCOPY)) {
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth       = w;
            bi.bmiHeader.biHeight      = -h;
            bi.bmiHeader.biPlanes      = 1;
            bi.bmiHeader.biBitCount    = 32;
            bi.bmiHeader.biCompression = BI_RGB;

            std::vector<DWORD> px((size_t)w * h);
            if (GetDIBits(mem, bmp, 0, h, px.data(), &bi, DIB_RGB_COLORS) == h) {
                const int N2 = 12;
                float sum = 0;
                for (int j = 0; j < N2; j++) {
                    for (int i = 0; i < N2; i++) {
                        DWORD c = px[(size_t)(h * (j + 0.5f) / N2) * w + (size_t)(w * (i + 0.5f) / N2)];
                        sum += Luma((float)((c >> 16) & 255), (float)((c >> 8) & 255), (float)(c & 255));
                    }
                }
                res = sum / (N2 * N2);
            }
        }
        SelectObject(mem, old);
        DeleteObject(bmp);
    }

    DeleteDC(mem);
    ReleaseDC(nullptr, scr);
    return res;
}

// Decodes the wallpaper file into a small luminance map; cached until the file changes.
static void LoadWallpaper(const std::wstring& path) {
    std::wstring key = path;
    WIN32_FILE_ATTRIBUTE_DATA fa = {};
    if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fa))
        key += L"|" + std::to_wstring(fa.ftLastWriteTime.dwLowDateTime) + L"|" + std::to_wstring(fa.nFileSizeLow);
    if (key == g_wp.key) return;

    g_wp = WpMap();
    g_wp.key = key;  // stored even on failure, so a bad file is not retried every second
    Wh_Log(L"Wallpaper: %s", path.c_str());

    Bitmap src(path.c_str(), FALSE);
    if (src.GetLastStatus() != Ok || src.GetWidth() == 0 || src.GetHeight() == 0) {
        Wh_Log(L"Wallpaper cannot be decoded by GDI+, using the screen pixels instead");
        return;
    }

    g_wp.iw = (int)src.GetWidth();
    g_wp.ih = (int)src.GetHeight();

    Bitmap th(kTw, kTh, PixelFormat32bppARGB);
    {
        Graphics g(&th);
        g.SetInterpolationMode(InterpolationModeHighQualityBilinear);
        g.DrawImage(&src, Rect(0, 0, kTw, kTh), 0, 0, g_wp.iw, g_wp.ih, UnitPixel);
    }

    g_wp.luma.resize((size_t)kTw * kTh);
    for (int y = 0; y < kTh; y++) {
        for (int x = 0; x < kTw; x++) {
            Color c;
            th.GetPixel(x, y, &c);
            g_wp.luma[(size_t)y * kTw + x] = Luma(c.GetR(), c.GetG(), c.GetB());
        }
    }
}

// Average brightness (0..1) of the wallpaper behind screen rectangle `wr`.
// `m` is the rectangle the wallpaper is laid out on (the monitor, or the whole desktop for "span").
// style = WallpaperStyle registry value: 0 center/tile, 2 stretch, 6 fit, 10 fill, 22 span.
static float SampleBrightness(const std::vector<float>& map, int iw, int ih, int style, bool tile,
                              const RECT& wr, const RECT& m, float bg) {
    const float mw = (float)(m.right - m.left), mh = (float)(m.bottom - m.top);
    float sx, sy;

    if (style == 2)      { sx = mw / iw; sy = mh / ih; }
    else if (style == 6) { sx = sy = std::min(mw / iw, mh / ih); }
    else if (style == 0) { sx = sy = 1.f; }
    else                 { sx = sy = std::max(mw / iw, mh / ih); }  // fill, span, unknown

    const float ox = tile ? 0.f : (mw - iw * sx) / 2;
    const float oy = tile ? 0.f : (mh - ih * sy) / 2;

    const int N = 12;
    float sum = 0;
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            float px = wr.left + (wr.right - wr.left) * (i + 0.5f) / N - m.left;
            float py = wr.top  + (wr.bottom - wr.top) * (j + 0.5f) / N - m.top;
            float u = (px - ox) / (iw * sx), v = (py - oy) / (ih * sy);
            if (tile) {
                u -= std::floor(u);
                v -= std::floor(v);
            }
            if (u < 0 || u >= 1 || v < 0 || v >= 1) sum += bg;  // outside the picture: desktop background color
            else sum += map[(size_t)(v * kTh) * kTw + (size_t)(u * kTw)];
        }
    }
    return sum / (N * N);
}

// Returns -1 when the wallpaper cannot be read.
static float WallpaperBrightness(const RECT& wr, const RECT& mon, const RECT& virt) {
    // Prefer the pixels Windows is actually displaying behind this widget.
    // This avoids mismatches caused by wallpaper fit/fill/span modes, theme
    // slideshow paths, per-monitor layouts, and other shell wallpaper effects.
    const float screen = ScreenBrightness(wr);
    if (screen >= 0.f) return screen;
    if (screen == -2.f) return -1.f;  // covered by a window: keep prior decision

    // If screen capture is unavailable, estimate brightness from the wallpaper
    // file using the configured layout as a fallback.
    int r = 0, g = 0, b = 0;
    swscanf(RegStr(L"Control Panel\\Colors", L"Background").c_str(), L"%d %d %d", &r, &g, &b);
    const float bg = Luma((float)r, (float)g, (float)b);

    const std::wstring path = RegStr(L"Control Panel\\Desktop", L"WallPaper");
    if (path.empty()) return bg;

    LoadWallpaper(path);
    if (g_wp.luma.empty()) return bg;

    const int style = _wtoi(RegStr(L"Control Panel\\Desktop", L"WallpaperStyle").c_str());
    const bool tile = _wtoi(RegStr(L"Control Panel\\Desktop", L"TileWallpaper").c_str()) != 0 && style == 0;
    return SampleBrightness(g_wp.luma, g_wp.iw, g_wp.ih, style, tile, wr, style == 22 ? virt : mon, bg);
}

static void Render() {
    HWND hw = g_hwnd;
    if (!hw || !IsWindow(hw) || GetSystemMetrics(SM_SHUTTINGDOWN)) return;  // no GDI+ work while the session ends

    Settings s;
    Weather  w;
    EnterCriticalSection(&g_cs);
    s = g_s;
    w = g_w;
    LeaveCriticalSection(&g_cs);

    InterlockedExchange(&g_dirty, 0);

    const float k = std::clamp(s.scale, 25, 400) / 100.0f;
    const wchar_t* loc = nullptr;
    if (!s.locale.empty()) loc = s.locale.c_str();

    SYSTEMTIME st;
    GetLocalTime(&st);
    g_lastKey = TimeKey(st, s.showSeconds);

    // time/date strings
    wchar_t buf[160];
    std::wstring tTime, tWd, tDate;

    if (s.showTime) {
        const wchar_t* pat = s.use24h
            ? (s.showSeconds ? L"HH:mm:ss" : L"HH:mm")
            : (s.showSeconds ? L"h:mm:ss tt" : L"h:mm tt");
        if (GetTimeFormatEx(loc, 0, &st, pat, buf, 160)) tTime = buf;
    }

    if (s.showWeekday && GetDateFormatEx(loc, 0, &st, L"dddd", buf, 160, nullptr)) {
        tWd = buf;
        if (s.uppercaseWeekday) CharUpperBuffW(tWd.data(), (DWORD)tWd.size());
    }

    if (s.showDate && GetDateFormatEx(loc, 0, &st, s.dateFormat.c_str(), buf, 160, nullptr))
        tDate = buf;

    // fonts
    int style = (s.bold ? FontStyleBold : 0) | (s.italic ? FontStyleItalic : 0);
    auto fam = [&](const std::wstring& f) { return f.empty() ? s.fontFamily : f; };

    auto fTime = MakeFont(fam(s.timeFont),    std::max(8, s.timeSize)    * k, style);
    auto fWd   = MakeFont(fam(s.weekdayFont), std::max(8, s.weekdaySize) * k, style);
    auto fDate = MakeFont(fam(s.dateFont),    std::max(8, s.dateSize)    * k, style);
    auto fWx   = MakeFont(fam(s.weatherFont), std::max(8, s.weatherSize) * k, style);
    auto fIcon = MakeFont(s.iconFont,         std::max(8, s.iconSize)    * k, (int)FontStyleRegular);

    const std::vector<std::wstring> order = ParseOrder(s.order);
    auto inOrder = [&](const wchar_t* id) {
        return std::find(order.begin(), order.end(), id) != order.end();
    };

    Bitmap   mb(1, 1, PixelFormat32bppPARGB);
    Graphics mg(&mb);
    mg.SetTextRenderingHint(TextRenderingHintAntiAlias);

    StringFormat sf(StringFormat::GenericTypographic());
    sf.SetFormatFlags(sf.GetFormatFlags() | StringFormatFlagsMeasureTrailingSpaces | StringFormatFlagsNoWrap);

    auto measure = [&](const std::wstring& t, Font* f) {
        RectF r;
        mg.MeasureString(t.c_str(), -1, f, PointF(0, 0), &sf, &r);
        return SizeF(r.Width, f->GetHeight(&mg));
    };

    const Color cTime = MakeColor(s.timeColor,    s.textOpacity);
    const Color cWd   = MakeColor(s.weekdayColor, s.textOpacity);
    const Color cDate = MakeColor(s.dateColor,    s.textOpacity);
    const Color cWx   = MakeColor(s.weatherColor, s.textOpacity);

    struct Line {
        std::wstring id, t;
        Font*        f;
        Color        c;
        float        w, h;
    };

    std::vector<Line> lines;
    auto addLine = [&](const wchar_t* id, const std::wstring& t, Font* f, Color c) {
        if (t.empty() || !inOrder(id)) return;
        SizeF z = measure(t, f);
        lines.push_back({id, t, f, c, z.Width, z.Height});
    };

    addLine(L"time",    tTime, fTime.get(), cTime);
    addLine(L"weekday", tWd,   fWd.get(),   cWd);
    addLine(L"date",    tDate, fDate.get(), cDate);

    // weather
    auto fmtT = [&](double v) {
        return std::isnan(v) ? std::wstring(L"-")
                             : std::to_wstring((int)std::lround(v)) + L"\u00B0";
    };

    struct Cell {
        std::wstring t;
        int          kind = 0;
        bool         acc  = false;
        SizeF        z;
    };

    std::vector<std::vector<Cell>> grid;
    std::wstring head;

    if (s.showWeather && inOrder(L"weather")) {
        if (w.valid) {
            if (s.showCityLine) {
                head = w.place;
                if (w.hasCur)
                    head += L" \u00B7 " + fmtT(w.cur) + (s.tempUnit == L"fahrenheit" ? L"F" : L"C");
            }

            int nd = std::min<int>((int)w.days.size(), s.forecastDays);
            std::vector<std::vector<Cell>> perDay;

            for (int i = 0; i < nd; i++) {
                const Day& d = w.days[i];
                SYSTEMTIME ds = {};
                ds.wYear  = (WORD)d.y;
                ds.wMonth = (WORD)d.m;
                ds.wDay   = (WORD)d.d;

                std::wstring wd;
                if (GetDateFormatEx(loc, 0, &ds, L"ddd", buf, 160, nullptr)) wd = buf;
                if (s.uppercaseWeekday) CharUpperBuffW(wd.data(), (DWORD)wd.size());

                std::vector<Cell> c;
                c.push_back({wd, 0, true});
                if (s.showIcons) c.push_back({IconFor(d.code), 1, false});
                c.push_back({fmtT(d.tmax) + L" / " + fmtT(d.tmin), 0, false});
                if (s.showPrecip && d.precip >= 0)
                    c.push_back({std::to_wstring(d.precip) + L"%", 0, false});

                perDay.push_back(c);
            }

            if (s.weatherLayout == L"vertical") {
                grid = perDay;
            } else {
                size_t rows = 0;
                for (auto& c : perDay) rows = std::max(rows, c.size());
                grid.assign(rows, {});
                for (size_t r = 0; r < rows; r++) {
                    for (auto& c : perDay)
                        grid[r].push_back(r < c.size() ? c[r] : Cell());
                }
            }
        } else {
            head = w.status;
        }
    }

    float Wc = 0;
    for (auto& l : lines) Wc = std::max(Wc, l.w);

    float headW = 0, headH = 0;
    if (!head.empty()) {
        SizeF z = measure(head, fWx.get());
        headW = z.Width;
        headH = z.Height;
        Wc = std::max(Wc, headW);
    }

    size_t nr = grid.size(), nc = 0;
    for (auto& r : grid) nc = std::max(nc, r.size());

    std::vector<float> cw(nc, 0.f), rh(nr, 0.f);
    const float gapX = 16 * k, gapY = 4 * k;

    for (size_t r = 0; r < nr; r++) {
        for (size_t c = 0; c < grid[r].size(); c++) {
            Cell& cell = grid[r][c];
            cell.z = cell.t.empty()
                ? SizeF(0, 0)
                : measure(cell.t, cell.kind == 1 ? fIcon.get() : fWx.get());
            cw[c] = std::max(cw[c], cell.z.Width);
            rh[r] = std::max(rh[r], cell.z.Height);
        }
    }

    float gridW = 0, gridH = 0;
    for (float v : cw) gridW += v;
    for (float v : rh) gridH += v;
    if (nc > 1) gridW += gapX * (nc - 1);
    if (nr > 1) gridH += gapY * (nr - 1);
    Wc = std::max(Wc, gridW);

    auto ax = [&](float width) {
        if (s.textAlign == L"left")  return 0.f;
        if (s.textAlign == L"right") return Wc - width;
        return (Wc - width) / 2;
    };

    struct Op {
        std::wstring t;
        Font*        f;
        Color        c;
        float        x, y;
    };

    std::vector<Op> ops;
    const float gap = std::max(0, s.lineGap) * k;

    float y = 0;
    bool  any = false;
    auto gapIf = [&](float g) {
        if (any) y += g;
        any = true;
    };

    bool prevWx = false;
    for (auto& tok : order) {
        if (tok != L"weather") {
            for (auto& l : lines) {
                if (l.id == tok) {
                    gapIf(prevWx ? gap * 2 : gap);
                    prevWx = false;
                    ops.push_back({l.t, l.f, l.c, ax(l.w), y});
                    y += l.h;
                }
            }
            continue;
        }

        if (!head.empty()) {
            gapIf(gap * 2);
            ops.push_back({head, fWx.get(), cWx, ax(headW), y});
            y += headH;
            prevWx = true;
        }

        if (!grid.empty()) {
            gapIf(head.empty() ? gap * 2 : gap);
            prevWx = true;

            float cy = y;
            for (size_t r = 0; r < nr; r++) {
                float cx = ax(gridW);
                for (size_t c = 0; c < nc; c++) {
                    if (c < grid[r].size() && !grid[r][c].t.empty()) {
                        Cell& cell = grid[r][c];
                        ops.push_back({
                            cell.t,
                            cell.kind == 1 ? fIcon.get() : fWx.get(),
                            cell.acc ? cWd : cWx,
                            cx + (cw[c] - cell.z.Width) / 2,
                            cy + (rh[r] - cell.z.Height) / 2
                        });
                    }
                    cx += cw[c] + gapX;
                }
                cy += rh[r] + gapY;
            }
            y += gridH;
        }
    }

    const float pad = std::max(0, s.padding) * k;
    const float sd  = s.shadow ? std::max(0, s.shadowOffset) * k : 0.f;

    int W = std::clamp((int)std::ceil(Wc + 2 * pad + sd) + 2, 4, 4000);
    int H = std::clamp((int)std::ceil(y  + 2 * pad + sd) + 2, 4, 4000);

    // position on screen (applied after drawing)
    MonList ml;
    EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&ml);

    bool  placed = false;
    int   px = 0, py = 0;
    RECT  monRc = {};

    if (!ml.v.empty()) {
        size_t idx = (size_t)std::max(0, s.monitor);
        if (idx >= ml.v.size()) idx = 0;

        RECT rc = ml.v[idx].rcWork;
        monRc = ml.v[idx].rcMonitor;

        int mx = s.marginX, my = s.marginY;
        const std::wstring& p = s.position;

        if (p.find(L"left") != std::wstring::npos)       px = rc.left + mx;
        else if (p.find(L"right") != std::wstring::npos) px = rc.right - mx - W;
        else                                             px = (rc.left + rc.right - W) / 2;

        if (p.find(L"top") != std::wstring::npos)        py = rc.top + my;
        else if (p.find(L"bottom") != std::wstring::npos) py = rc.bottom - my - H;
        else                                             py = (rc.top + rc.bottom - H) / 2;

        placed = true;
    }

    // "Adjust font color to wallpaper": light text on a dark wallpaper, dark text on a light one
    std::wstring shadowCol = s.shadowColor;
    if (s.adaptColor && placed) {
        RECT wr = {px, py, px + W, py + H};

        RECT virt;
        virt.left   = GetSystemMetrics(SM_XVIRTUALSCREEN);
        virt.top    = GetSystemMetrics(SM_YVIRTUALSCREEN);
        virt.right  = virt.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        virt.bottom = virt.top  + GetSystemMetrics(SM_CYVIRTUALSCREEN);

        float b = WallpaperBrightness(wr, monRc, virt);

        if (b > 0.55f)      g_wpLight = true;   // hysteresis: no flicker around the threshold
        else if (b >= 0 && b < 0.45f) g_wpLight = false;

        static int lastLogged = -1;
        if ((int)g_wpLight != lastLogged) {
            lastLogged = (int)g_wpLight;
            Wh_Log(L"Adjust font color: wallpaper brightness %d%%, using %s text",
                   (int)(b * 100), g_wpLight ? L"dark" : L"light");
        }

        const Color tc = MakeColor(g_wpLight ? s.adaptDarkText : s.adaptLightText, s.textOpacity);
        for (auto& op : ops) op.c = tc;
        if (g_wpLight) shadowCol = L"#FFFFFF";  // a black shadow under dark text only smudges it
    }

    // DIB section for UpdateLayeredWindow
    HDC scr = GetDC(nullptr);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = W;
    bi.bmiHeader.biHeight      = -H;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hb = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hb) {
        ReleaseDC(nullptr, scr);
        return;
    }

    HDC mem = CreateCompatibleDC(scr);
    HGDIOBJ old = SelectObject(mem, hb);
    memset(bits, 0, (size_t)W * H * 4);

    {
        Bitmap bmp(W, H, W * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAlias);

        if (s.bgOpacity > 0) {
            SolidBrush br(MakeColor(s.bgColor, s.bgOpacity));
            RectF R(0, 0, (REAL)(W - 1), (REAL)(H - 1));
            float r = std::min(s.cornerRadius * k, std::min(R.Width, R.Height) / 2);
            if (r > 0.5f) {
                float d = r * 2;
                GraphicsPath gp;
                gp.AddArc(R.X, R.Y, d, d, 180, 90);
                gp.AddArc(R.GetRight() - d, R.Y, d, d, 270, 90);
                gp.AddArc(R.GetRight() - d, R.GetBottom() - d, d, d, 0, 90);
                gp.AddArc(R.X, R.GetBottom() - d, d, d, 90, 90);
                gp.CloseFigure();
                g.FillPath(&br, &gp);
            } else {
                g.FillRectangle(&br, R);
            }
        }

        SolidBrush shBr(MakeColor(shadowCol, s.shadowOpacity * s.textOpacity / 100));
        for (auto& op : ops) {
            if (sd > 0)
                g.DrawString(op.t.c_str(), -1, op.f,
                             PointF(op.x + pad + sd, op.y + pad + sd), &sf, &shBr);
            SolidBrush br(op.c);
            g.DrawString(op.t.c_str(), -1, op.f,
                         PointF(op.x + pad, op.y + pad), &sf, &br);
        }
    }

    SIZE  sz  = {W, H};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hw, scr, nullptr, &sz, mem, &src, 0, &bf, ULW_ALPHA);

    // position
    if (placed) {
        POINT pt = {px, py};
        HWND host = GetParent(hw);
        if (host) ScreenToClient(host, &pt);
        SetWindowPos(hw, nullptr, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    SelectObject(mem, old);
    DeleteObject(hb);
    DeleteDC(mem);
    ReleaseDC(nullptr, scr);
}

static void Tick() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    bool sec;
    EnterCriticalSection(&g_cs);
    sec = g_s.showSeconds;
    LeaveCriticalSection(&g_cs);

    if (g_dirty || TimeKey(st, sec) != g_lastKey) Render();
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_TIMER:      Tick(); return 0;
        case WM_APP_RENDER: InterlockedExchange(&g_dirty, 1); Render(); return 0;
        case WM_NCHITTEST:  return HTTRANSPARENT;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, m, w, l);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Placement behind the icons
// ─────────────────────────────────────────────────────────────────────────────

// True once the desktop icon list exists, i.e. the shell has finished creating the desktop.
static bool DesktopReady(HWND pm) {
    HWND dv = FindWindowExW(pm, nullptr, L"SHELLDLL_DefView", nullptr);
    for (HWND ww = nullptr; !dv && (ww = FindWindowExW(nullptr, ww, L"WorkerW", nullptr)) != nullptr;)
        dv = FindWindowExW(ww, nullptr, L"SHELLDLL_DefView", nullptr);
    return dv && FindWindowExW(dv, nullptr, L"SysListView32", nullptr);
}

static BOOL CALLBACK FindWorkerProc(HWND h, LPARAM l) {
    if (FindWindowExW(h, nullptr, L"SHELLDLL_DefView", nullptr))
        *(HWND*)l = FindWindowExW(nullptr, h, L"WorkerW", nullptr);
    return TRUE;
}

static bool TryFindHost(HWND pm, HWND& host, HWND& after) {
    // Windows 11 24H2+: SHELLDLL_DefView and WorkerW both inside Progman
    HWND ww = FindWindowExW(pm, nullptr, L"WorkerW", nullptr);
    if (ww) {
        HWND dv = FindWindowExW(pm, nullptr, L"SHELLDLL_DefView", nullptr);
        if (dv) {
            host = pm;
            after = dv;
            return true;
        }
    }

    // Windows 10 / early 11: the WorkerW right after the one that holds the icons
    HWND found = nullptr;
    EnumWindows(FindWorkerProc, (LPARAM)&found);
    if (found) {
        host = found;
        after = HWND_TOP;
        return true;
    }
    return false;
}

static bool FindHost(HWND& host, HWND& after) {
    HWND pm = FindWindowW(L"Progman", nullptr);
    if (!pm) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(pm, &pid);
    if (pid != GetCurrentProcessId()) return false;  // not the shell's explorer.exe

    // never touch a shell that is still starting up or already closing
    if (GetSystemMetrics(SM_SHUTTINGDOWN) || !DesktopReady(pm)) return false;

    if (TryFindHost(pm, host, after)) return true;

    static bool poked = false;  // send the "create WorkerW" message once per process, never repeatedly
    if (poked) return false;
    poked = true;

    DWORD_PTR res = 0;
    SendMessageTimeoutW(pm, 0x052C, 0xD, 1, SMTO_NORMAL, 1000, &res);
    return TryFindHost(pm, host, after);
}

static DWORD WINAPI UiThread(LPVOID) {
    if (WaitForSingleObject(g_evStop, 3000) == WAIT_OBJECT_0) return 0;  // let explorer finish starting first

    GdiplusStartupInput gi;
    ULONG_PTR tok = 0;
    if (GdiplusStartup(&tok, &gi, nullptr) != Ok) return 0;

    HINSTANCE hi = GetModuleHandleW(nullptr);
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hi;
    wc.lpszClassName = kClass;
    RegisterClassW(&wc);

    int tries = 0;
    while (WaitForSingleObject(g_evStop, 0) != WAIT_OBJECT_0) {
        HWND host = nullptr, after = nullptr;
        if (!FindHost(host, after)) {
            DWORD ms = tries++ < 120 ? 1000 : 10000;  // the desktop may appear late during logon
            if (WaitForSingleObject(g_evStop, ms) == WAIT_OBJECT_0) break;
            continue;
        }

        HWND h = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY,
            kClass, L"",
            WS_CHILD | WS_VISIBLE,
            0, 0, 1, 1,
            host, nullptr, hi, nullptr);

        if (!h) {
            Wh_Log(L"CreateWindowEx failed: %lu", GetLastError());
            if (WaitForSingleObject(g_evStop, 5000) == WAIT_OBJECT_0) break;
            continue;
        }

        g_hwnd = h;
        SetWindowPos(h, after, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        SetTimer(h, 1, 1000, nullptr);
        InterlockedExchange(&g_dirty, 1);
        Render();

        while (IsWindow(h)) {
            DWORD r = MsgWaitForMultipleObjects(1, &g_evStop, FALSE, INFINITE, QS_ALLINPUT);
            if (r == WAIT_OBJECT_0) {
                DestroyWindow(h);
                break;
            }
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        g_hwnd = nullptr;
        break;  // the desktop is gone (shell exiting): never recreate the window or poke the shell again
    }

    UnregisterClassW(kClass, hi);
    GdiplusShutdown(tok);
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Windhawk entry points
// ─────────────────────────────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_cs);
    LoadSettings();

    g_evStop    = CreateEventW(nullptr, TRUE,  FALSE, nullptr);
    g_evRefresh = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    g_thUi = CreateThread(nullptr, 0, UiThread,      nullptr, 0, nullptr);
    g_thWx = CreateThread(nullptr, 0, WeatherThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Settings old;
    EnterCriticalSection(&g_cs);
    old = g_s;
    LeaveCriticalSection(&g_cs);

    LoadSettings();

    Settings now;
    EnterCriticalSection(&g_cs);
    now = g_s;
    LeaveCriticalSection(&g_cs);

    if (old.city          != now.city          ||
        old.tempUnit      != now.tempUnit      ||
        old.showWeather   != now.showWeather   ||
        old.locale        != now.locale        ||
        old.updateMinutes != now.updateMinutes) {
        SetEvent(g_evRefresh);
    }

    HWND h = g_hwnd;
    if (h) PostMessageW(h, WM_APP_RENDER, 0, 0);
}

void Wh_ModUninit() {
    SetEvent(g_evStop);

    HINTERNET pending = (HINTERNET)InterlockedExchangePointer(&g_req, nullptr);  // abort an in-flight HTTP request
    if (pending) WinHttpCloseHandle(pending);

    if (g_thUi) { WaitForSingleObject(g_thUi, INFINITE); CloseHandle(g_thUi); }
    if (g_thWx) { WaitForSingleObject(g_thWx, INFINITE); CloseHandle(g_thWx); }

    CloseHandle(g_evStop);
    CloseHandle(g_evRefresh);
    DeleteCriticalSection(&g_cs);
}
