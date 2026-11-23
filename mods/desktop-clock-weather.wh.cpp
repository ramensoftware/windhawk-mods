// ==WindhawkMod==
// @id              desktop-clock-weather
// @name            Desktop Clock & Weather
// @description     Time, weekday, date and weekly weather on the desktop - behind icons, not clickable
// @version         1.0.5
// @author          DineroOne
// @license         MIT
// @github          https://github.com/DineroOne
// @include         explorer.exe
// @compilerOptions -lgdi32 -lgdiplus -lwinhttp -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Clock & Weather

![Illustration of the widget (mockup)](https://raw.githubusercontent.com/DineroOne/windhawk-desktop-clock-weather/main/screenshot-small.png)

The widget is drawn directly on the desktop: **above the wallpaper but below icons and folders**,
so it never intercepts clicks.

Shows time, weekday, date and a 3-7 day weather forecast.
Weather comes from [Open-Meteo](https://open-meteo.com) (free, no API key needed).
Set the city name in the settings; coordinates are found automatically.

Everything (position, size, font, colors, shadow, background, date format, etc.) is configurable in the settings tab.

## Why this is a separate mod
Desktop Live Overlay is a general-purpose desktop overlay with configurable text patterns and system metrics.
This mod focuses on a structured 3-7 day forecast grid with weather icons, high/low temperatures and
precipitation probability, alongside separately configurable time, weekday, date and weather elements.
That forecast-oriented layout is the reason for keeping it separate.

## Network use

Weather is the only feature that uses the network, and only after you enter a city in the settings,
keep **Show weather** enabled and include `weather` in the element order. The mod sends HTTPS requests
to two Open-Meteo hosts:

- `geocoding-api.open-meteo.com` - when the city or language changes, to turn the city name into coordinates
  (the city name and a two-letter language code are sent).
- `api.open-meteo.com` - at startup and then every N minutes (30 by default), to get the current temperature
  and forecast (the coordinates and selected temperature unit are sent).

After a failed request, the mod retries after about 60 seconds. Like any web server, Open-Meteo can see
your IP address with each request. No API key, account or other personal data is sent. No weather requests
are made while the city setting is empty, **Show weather** is off, or `weather` is omitted from the element order.
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

// ───────────────────────── settings ─────────────────────────

struct Settings {
    std::wstring position = L"topright", textAlign = L"right", locale, dateFormat = L"d MMMM yyyy";
    std::wstring fontFamily = L"Segoe UI", iconFont = L"Segoe UI Symbol";
    std::wstring timeFont, weekdayFont, dateFont, weatherFont, order = L"time,weekday,date,weather";
    std::wstring timeColor = L"#FFFFFF", weekdayColor = L"#FFD36B", dateColor = L"#FFFFFF", weatherColor = L"#FFFFFF";
    std::wstring shadowColor = L"#000000", bgColor = L"#000000";
    std::wstring city, tempUnit = L"celsius", weatherLayout = L"horizontal";
    int marginX = 40, marginY = 40, monitor = 0, scale = 100;
    int timeSize = 96, weekdaySize = 36, dateSize = 24, weatherSize = 18, iconSize = 30;
    int textOpacity = 100, shadowOpacity = 60, shadowOffset = 2, bgOpacity = 0, cornerRadius = 16, padding = 12, lineGap = 6;
    int forecastDays = 7, updateMinutes = 30;
    bool showTime = true, showSeconds = false, use24h = true, showWeekday = true, uppercaseWeekday = false, showDate = true;
    bool bold = false, italic = false, shadow = true, showWeather = true, showIcons = true, showPrecip = true, showCityLine = true;
};

struct Day { int y = 0, m = 0, d = 0, code = 0, precip = -1; double tmax = NAN, tmin = NAN; };
struct Weather {
    bool valid = false, hasCur = false;
    std::wstring place, status = L"Weather: loading...";
    double cur = 0;
    std::vector<Day> days;
};

static CRITICAL_SECTION g_cs;
static Settings g_s;
static Weather g_w;
static HANDLE g_evStop, g_evRefresh, g_thWx;
static HWINEVENTHOOK g_winEventHook = nullptr;
static std::atomic<HWND> g_hwnd{nullptr};
static HWND g_host = nullptr;
static UINT g_runFromWindowThreadMsg = 0;
static ULONG_PTR g_gdiplusToken = 0;
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_uiCreating{false};
static std::atomic<LONG> g_activeWinEventCallbacks{0};
static std::atomic<bool> g_dirty{true};
static LONG g_lastKey = -1;
static const wchar_t kClass[] = L"WhDesktopClockWeatherWnd";

static std::wstring GetS(PCWSTR n) {
    PCWSTR v = Wh_GetStringSetting(n);
    std::wstring r = v ? v : L"";
    Wh_FreeStringSetting(v);
    return r;
}
static int GetI(PCWSTR n) { return Wh_GetIntSetting(n); }

static void LoadSettings() {
    Settings s;
    s.position = GetS(L"position");
    s.textAlign = GetS(L"textAlign");
    s.locale = GetS(L"locale");
    s.dateFormat = GetS(L"dateFormat");
    s.fontFamily = GetS(L"fontFamily");
    s.iconFont = GetS(L"iconFont");
    s.timeFont = GetS(L"timeFont");
    s.weekdayFont = GetS(L"weekdayFont");
    s.dateFont = GetS(L"dateFont");
    s.weatherFont = GetS(L"weatherFont");
    s.order = GetS(L"order");
    s.timeColor = GetS(L"timeColor");
    s.weekdayColor = GetS(L"weekdayColor");
    s.dateColor = GetS(L"dateColor");
    s.weatherColor = GetS(L"weatherColor");
    s.shadowColor = GetS(L"shadowColor");
    s.bgColor = GetS(L"bgColor");
    s.city = GetS(L"city");
    s.tempUnit = GetS(L"tempUnit");
    s.weatherLayout = GetS(L"weatherLayout");
    s.marginX = GetI(L"marginX");
    s.marginY = GetI(L"marginY");
    s.monitor = GetI(L"monitor");
    s.scale = GetI(L"scale");
    s.timeSize = GetI(L"timeSize");
    s.weekdaySize = GetI(L"weekdaySize");
    s.dateSize = GetI(L"dateSize");
    s.weatherSize = GetI(L"weatherSize");
    s.iconSize = GetI(L"iconSize");
    s.textOpacity = GetI(L"textOpacity");
    s.shadowOpacity = GetI(L"shadowOpacity");
    s.shadowOffset = GetI(L"shadowOffset");
    s.bgOpacity = GetI(L"bgOpacity");
    s.cornerRadius = GetI(L"cornerRadius");
    s.padding = GetI(L"padding");
    s.lineGap = GetI(L"lineGap");
    s.forecastDays = std::clamp(GetI(L"forecastDays"), 3, 7);
    s.updateMinutes = std::clamp(GetI(L"updateMinutes"), 5, 1440);
    s.showTime = GetI(L"showTime") != 0;
    s.showSeconds = GetI(L"showSeconds") != 0;
    s.use24h = GetI(L"use24h") != 0;
    s.showWeekday = GetI(L"showWeekday") != 0;
    s.uppercaseWeekday = GetI(L"uppercaseWeekday") != 0;
    s.showDate = GetI(L"showDate") != 0;
    s.bold = GetI(L"bold") != 0;
    s.italic = GetI(L"italic") != 0;
    s.shadow = GetI(L"shadow") != 0;
    s.showWeather = GetI(L"showWeather") != 0;
    s.showIcons = GetI(L"showIcons") != 0;
    s.showPrecip = GetI(L"showPrecip") != 0;
    s.showCityLine = GetI(L"showCityLine") != 0;
    EnterCriticalSection(&g_cs);
    g_s = s;
    LeaveCriticalSection(&g_cs);
}

// ───────────────────────── helpers ─────────────────────────

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
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') o += (char)c;
        else { o += '%'; o += hex[c >> 4]; o += hex[c & 15]; }
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
    if (c == 0) return L"\u2600";
    if (c == 1) return L"\U0001F324";
    if (c == 2) return L"\u26C5";
    if (c == 3) return L"\u2601";
    if (c == 45 || c == 48) return L"\U0001F32B";
    if (c >= 51 && c <= 57) return L"\U0001F326";
    if ((c >= 61 && c <= 67) || (c >= 80 && c <= 82)) return L"\U0001F327";
    if ((c >= 71 && c <= 77) || c == 85 || c == 86) return L"\u2744";
    if (c >= 95) return L"\u26C8";
    return L"\u2601";
}

// ───────────────────────── HTTP + JSON (Open-Meteo) ─────────────────────────

struct HttpAsyncContext {
    HANDLE done = nullptr;
    HANDLE closing = nullptr;
    std::string* output = nullptr;
    std::vector<char> buffer;
    bool success = false;
    std::atomic<bool> finished{false};
};

static void FinishHttpRequest(HttpAsyncContext* ctx, bool success) {
    if (!ctx) return;
    bool expected = false;
    if (!ctx->finished.compare_exchange_strong(expected, true)) return;
    ctx->success = success;
    SetEvent(ctx->done);
}

static void CALLBACK HttpStatusCallback(HINTERNET handle, DWORD_PTR context,
                                        DWORD status, LPVOID info, DWORD infoLength) {
    auto* ctx = reinterpret_cast<HttpAsyncContext*>(context);
    if (!ctx) return;

    switch (status) {
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
            if (!WinHttpReceiveResponse(handle, nullptr) && GetLastError() != ERROR_IO_PENDING)
                FinishHttpRequest(ctx, false);
            break;

        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE: {
            DWORD code = 0, size = sizeof(code);
            if (!WinHttpQueryHeaders(handle,
                    WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                    WINHTTP_HEADER_NAME_BY_INDEX, &code, &size, WINHTTP_NO_HEADER_INDEX) || code != 200) {
                FinishHttpRequest(ctx, false);
                break;
            }
            if (!WinHttpQueryDataAvailable(handle, nullptr) && GetLastError() != ERROR_IO_PENDING)
                FinishHttpRequest(ctx, false);
            break;
        }

        case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE: {
            if (infoLength < sizeof(DWORD)) {
                FinishHttpRequest(ctx, false);
                break;
            }
            DWORD available = *reinterpret_cast<DWORD*>(info);
            constexpr size_t kMaxResponse = 262144;
            size_t remaining = kMaxResponse - ctx->output->size();
            if (available == 0 || remaining < 16384) {
                FinishHttpRequest(ctx, !ctx->output->empty());
                break;
            }
            // Keep the asynchronous read buffer comfortably above WinHTTP's
            // internal 8 KB buffer to avoid synchronous callback recursion.
            DWORD toRead = static_cast<DWORD>(std::min<size_t>(
                std::max<size_t>(available, 16384), remaining));
            ctx->buffer.resize(toRead);
            if (!WinHttpReadData(handle, ctx->buffer.data(), toRead, nullptr) && GetLastError() != ERROR_IO_PENDING)
                FinishHttpRequest(ctx, false);
            break;
        }

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
            if (infoLength && info) {
                ctx->output->append(static_cast<const char*>(info), infoLength);
            }
            if (infoLength == 0 || ctx->output->size() >= 262144) {
                FinishHttpRequest(ctx, !ctx->output->empty());
                break;
            }
            if (!WinHttpQueryDataAvailable(handle, nullptr) && GetLastError() != ERROR_IO_PENDING)
                FinishHttpRequest(ctx, false);
            break;

        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
            FinishHttpRequest(ctx, false);
            break;

        case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
            SetEvent(ctx->closing);
            break;
    }
}

static bool HttpGet(const wchar_t* host, const std::wstring& path, std::string& out) {
    if (WaitForSingleObject(g_evStop, 0) == WAIT_OBJECT_0) return false;

    HttpAsyncContext ctx;
    ctx.done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ctx.closing = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ctx.output = &out;
    if (!ctx.done || !ctx.closing) {
        if (ctx.done) CloseHandle(ctx.done);
        if (ctx.closing) CloseHandle(ctx.closing);
        return false;
    }

    HINTERNET session = WinHttpOpen(L"WindhawkDesktopClockWeather/1.0",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
    if (!session) {
        CloseHandle(ctx.done);
        CloseHandle(ctx.closing);
        return false;
    }

    WinHttpSetTimeouts(session, 4000, 4000, 4000, 6000);
    auto oldCallback = WinHttpSetStatusCallback(session, HttpStatusCallback,
        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES, 0);
    if (oldCallback == WINHTTP_INVALID_STATUS_CALLBACK) {
        WinHttpCloseHandle(session);
        CloseHandle(ctx.done);
        CloseHandle(ctx.closing);
        return false;
    }

    HINTERNET connection = WinHttpConnect(session, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET request = connection ? WinHttpOpenRequest(connection, L"GET", path.c_str(), nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE) : nullptr;

    bool completed = false;
    if (request) {
        DWORD_PTR contextValue = reinterpret_cast<DWORD_PTR>(&ctx);
        bool contextSet = WinHttpSetOption(request, WINHTTP_OPTION_CONTEXT_VALUE,
            &contextValue, sizeof(contextValue)) != FALSE;
        if (contextSet) {
            BOOL sent = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                WINHTTP_NO_REQUEST_DATA, 0, 0, contextValue);
            if (sent || GetLastError() == ERROR_IO_PENDING) {
                HANDLE waits[] = {g_evStop, ctx.done};
                DWORD waitResult = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
                completed = waitResult == WAIT_OBJECT_0 + 1 && ctx.success;
            }
        }
        // In asynchronous mode closing the request cancels outstanding operations.
        WinHttpCloseHandle(request);
        if (contextSet) WaitForSingleObject(ctx.closing, INFINITE);
    }

    if (connection) WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);
    CloseHandle(ctx.done);
    CloseHandle(ctx.closing);
    return completed && !out.empty();
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
    auto flush = [&] { out += Widen(raw); raw.clear(); };
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
        if (c == ',') { r.push_back(cur); cur.clear(); }
        else if (c != '"' && c != ' ') cur += c;
    }
    return r;
}

static double Num(const std::string& s) {
    char* e = nullptr;
    double v = strtod(s.c_str(), &e);
    return e == s.c_str() ? NAN : v;
}

static std::wstring g_geoCity, g_geoPlace, g_geoLang;
static double g_lat = 0, g_lon = 0;
static bool g_geoOk = false;

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
        g_w.valid = false;
        g_w.days.clear();
        g_w.place.clear();
        g_w.hasCur = false;
        g_w.status = msg;
        LeaveCriticalSection(&g_cs);
        return false;
    };
    if (s.city.empty()) return fail(L"Weather: set a city in settings");

    const std::string lang = LangCode(s);
    const std::wstring langWide = Widen(lang);
    if (!g_geoOk || g_geoCity != s.city || g_geoLang != langWide) {
        std::string body;
        std::string path = "/v1/search?name=" + UrlEnc(Narrow(s.city)) + "&count=1&format=json&language=" + lang;
        if (!HttpGet(L"geocoding-api.open-meteo.com", Widen(path), body)) return fail(L"Weather: no connection");
        size_t r = body.find("\"results\":[");
        double la = 0, lo = 0;
        if (r == std::string::npos || !NumAfter(body, "latitude", r, la) || !NumAfter(body, "longitude", r, lo))
            return fail(L"Weather: city not found");
        std::wstring nm;
        if (!JsonStr(body, "name", r, nm) || nm.empty()) nm = s.city;
        g_lat = la; g_lon = lo; g_geoPlace = nm; g_geoCity = s.city; g_geoLang = langWide; g_geoOk = true;
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
    auto t = Arr(body, "time", d), wc = Arr(body, "weather_code", d), mx = Arr(body, "temperature_2m_max", d),
         mn = Arr(body, "temperature_2m_min", d), pp = Arr(body, "precipitation_probability_max", d);

    Weather nw;
    nw.valid = true;
    nw.place = g_geoPlace;
    for (size_t i = 0; i < t.size() && i < 7; i++) {
        Day dy;
        if (sscanf(t[i].c_str(), "%d-%d-%d", &dy.y, &dy.m, &dy.d) != 3) continue;
        double v = i < wc.size() ? Num(wc[i]) : NAN;
        dy.code = std::isnan(v) ? 0 : (int)v;
        dy.tmax = i < mx.size() ? Num(mx[i]) : NAN;
        dy.tmin = i < mn.size() ? Num(mn[i]) : NAN;
        v = i < pp.size() ? Num(pp[i]) : NAN;
        dy.precip = std::isnan(v) ? -1 : (int)std::lround(v);
        nw.days.push_back(dy);
    }
    if (nw.days.empty()) return fail(L"Weather: bad response");

    size_t c = body.find("\"current\":{");
    if (c != std::string::npos) {
        double a = 0;
        if (NumAfter(body, "temperature_2m", c, a)) { nw.hasCur = true; nw.cur = a; }
    }

    EnterCriticalSection(&g_cs);
    g_w = nw;
    LeaveCriticalSection(&g_cs);
    return true;
}

static bool WeatherIsInOrder(const std::wstring& order) {
    std::wstring token;
    for (size_t i = 0; i <= order.size(); ++i) {
        wchar_t c = i < order.size() ? order[i] : L',';
        if (c == L',' || c == L' ' || c == L';') {
            if (token == L"weather") return true;
            token.clear();
        } else {
            token += (c >= L'A' && c <= L'Z') ? static_cast<wchar_t>(c + 32) : c;
        }
    }
    return false;
}

static DWORD WINAPI WeatherThread(LPVOID) {
    for (;;) {
        Settings s;
        EnterCriticalSection(&g_cs);
        s = g_s;
        LeaveCriticalSection(&g_cs);

        DWORD wait = INFINITE;
        if (s.showWeather && WeatherIsInOrder(s.order)) {
            if (!g_hwnd || GetSystemMetrics(SM_SHUTTINGDOWN)) {
                wait = INFINITE; // UI creation signals g_evRefresh; shutdown signals g_evStop.
            } else {
                bool ok = FetchWeather(s); // An empty city only updates the on-screen prompt; it makes no request.
                wait = s.city.empty() ? INFINITE : (ok ? static_cast<DWORD>(s.updateMinutes) * 60000 : 60000);
                HWND h = g_hwnd.load();
                if (h) PostMessageW(h, WM_APP_RENDER, 0, 0);
            }
        }
        HANDLE hs[2] = {g_evStop, g_evRefresh};
        DWORD result = WaitForMultipleObjects(2, hs, FALSE, wait);
        if (result == WAIT_OBJECT_0) return 0;
    }
}
// ───────────────────────── rendering ─────────────────────────

struct MonList { std::vector<MONITORINFO> v; };
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

static void Render() {
    HWND hw = g_hwnd;
    if (!hw || !IsWindow(hw) || GetSystemMetrics(SM_SHUTTINGDOWN)) return;  // no GDI+ work while the session ends

    Settings s;
    Weather w;
    EnterCriticalSection(&g_cs);
    s = g_s;
    w = g_w;
    LeaveCriticalSection(&g_cs);
    g_dirty.store(false);

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
        const wchar_t* pat = s.use24h ? (s.showSeconds ? L"HH:mm:ss" : L"HH:mm") : (s.showSeconds ? L"h:mm:ss tt" : L"h:mm tt");
        if (GetTimeFormatEx(loc, 0, &st, pat, buf, 160)) tTime = buf;
    }
    if (s.showWeekday && GetDateFormatEx(loc, 0, &st, L"dddd", buf, 160, nullptr)) {
        tWd = buf;
        if (s.uppercaseWeekday) CharUpperBuffW(tWd.data(), (DWORD)tWd.size());
    }
    if (s.showDate && GetDateFormatEx(loc, 0, &st, s.dateFormat.c_str(), buf, 160, nullptr)) tDate = buf;

    // fonts
    int style = (s.bold ? FontStyleBold : 0) | (s.italic ? FontStyleItalic : 0);
    auto fam = [&](const std::wstring& f) { return f.empty() ? s.fontFamily : f; };
    auto fTime = MakeFont(fam(s.timeFont), std::max(8, s.timeSize) * k, style);
    auto fWd = MakeFont(fam(s.weekdayFont), std::max(8, s.weekdaySize) * k, style);
    auto fDate = MakeFont(fam(s.dateFont), std::max(8, s.dateSize) * k, style);
    auto fWx = MakeFont(fam(s.weatherFont), std::max(8, s.weatherSize) * k, style);
    const std::vector<std::wstring> order = ParseOrder(s.order);
    auto inOrder = [&](const wchar_t* id) { return std::find(order.begin(), order.end(), id) != order.end(); };
    auto fIcon = MakeFont(s.iconFont, std::max(8, s.iconSize) * k, (int)FontStyleRegular);

    Bitmap mb(1, 1, PixelFormat32bppPARGB);
    Graphics mg(&mb);
    mg.SetTextRenderingHint(TextRenderingHintAntiAlias);
    StringFormat sf(StringFormat::GenericTypographic());
    sf.SetFormatFlags(sf.GetFormatFlags() | StringFormatFlagsMeasureTrailingSpaces | StringFormatFlagsNoWrap);
    auto measure = [&](const std::wstring& t, Font* f) {
        RectF r;
        mg.MeasureString(t.c_str(), -1, f, PointF(0, 0), &sf, &r);
        return SizeF(r.Width, f->GetHeight(&mg));
    };

    const Color cTime = MakeColor(s.timeColor, s.textOpacity), cWd = MakeColor(s.weekdayColor, s.textOpacity),
                cDate = MakeColor(s.dateColor, s.textOpacity), cWx = MakeColor(s.weatherColor, s.textOpacity);

    struct Line { std::wstring id, t; Font* f; Color c; float w, h; };
    std::vector<Line> lines;
    auto addLine = [&](const wchar_t* id, const std::wstring& t, Font* f, Color c) {
        if (t.empty() || !inOrder(id)) return;
        SizeF z = measure(t, f);
        lines.push_back({id, t, f, c, z.Width, z.Height});
    };
    addLine(L"time", tTime, fTime.get(), cTime);
    addLine(L"weekday", tWd, fWd.get(), cWd);
    addLine(L"date", tDate, fDate.get(), cDate);

    // weather
    auto fmtT = [&](double v) { return std::isnan(v) ? std::wstring(L"-") : std::to_wstring((int)std::lround(v)) + L"\u00B0"; };
    struct Cell { std::wstring t; int kind = 0; bool acc = false; SizeF z; };
    std::vector<std::vector<Cell>> grid;
    std::wstring head;
    if (s.showWeather && inOrder(L"weather")) {
        if (w.valid) {
            if (s.showCityLine) {
                head = w.place;
                if (w.hasCur) head += L" \u00B7 " + fmtT(w.cur) + (s.tempUnit == L"fahrenheit" ? L"F" : L"C");
            }
            int nd = std::min<int>((int)w.days.size(), s.forecastDays);
            std::vector<std::vector<Cell>> perDay;
            for (int i = 0; i < nd; i++) {
                const Day& d = w.days[i];
                SYSTEMTIME ds = {};
                ds.wYear = (WORD)d.y; ds.wMonth = (WORD)d.m; ds.wDay = (WORD)d.d;
                std::wstring wd;
                if (GetDateFormatEx(loc, 0, &ds, L"ddd", buf, 160, nullptr)) wd = buf;
                if (s.uppercaseWeekday) CharUpperBuffW(wd.data(), (DWORD)wd.size());
                std::vector<Cell> c;
                c.push_back({wd, 0, true});
                if (s.showIcons) c.push_back({IconFor(d.code), 1, false});
                c.push_back({fmtT(d.tmax) + L" / " + fmtT(d.tmin), 0, false});
                if (s.showPrecip && d.precip >= 0) c.push_back({std::to_wstring(d.precip) + L"%", 0, false});
                perDay.push_back(c);
            }
            if (s.weatherLayout == L"vertical") {
                grid = perDay;
            } else {
                size_t rows = 0;
                for (auto& c : perDay) rows = std::max(rows, c.size());
                grid.assign(rows, {});
                for (size_t r = 0; r < rows; r++)
                    for (auto& c : perDay) grid[r].push_back(r < c.size() ? c[r] : Cell());
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
        headW = z.Width; headH = z.Height;
        Wc = std::max(Wc, headW);
    }
    size_t nr = grid.size(), nc = 0;
    for (auto& r : grid) nc = std::max(nc, r.size());
    std::vector<float> cw(nc, 0.f), rh(nr, 0.f);
    const float gapX = 16 * k, gapY = 4 * k;
    for (size_t r = 0; r < nr; r++)
        for (size_t c = 0; c < grid[r].size(); c++) {
            Cell& cell = grid[r][c];
            cell.z = cell.t.empty() ? SizeF(0, 0) : measure(cell.t, cell.kind == 1 ? fIcon.get() : fWx.get());
            cw[c] = std::max(cw[c], cell.z.Width);
            rh[r] = std::max(rh[r], cell.z.Height);
        }
    float gridW = 0, gridH = 0;
    for (float v : cw) gridW += v;
    for (float v : rh) gridH += v;
    if (nc > 1) gridW += gapX * (nc - 1);
    if (nr > 1) gridH += gapY * (nr - 1);
    Wc = std::max(Wc, gridW);

    auto ax = [&](float width) {
        if (s.textAlign == L"left") return 0.f;
        if (s.textAlign == L"right") return Wc - width;
        return (Wc - width) / 2;
    };

    struct Op { std::wstring t; Font* f; Color c; float x, y; };
    std::vector<Op> ops;
    const float gap = std::max(0, s.lineGap) * k;
    float y = 0;
    bool any = false;
    auto gapIf = [&](float g) { if (any) y += g; any = true; };
    bool prevWx = false;
    for (auto& tok : order) {
        if (tok != L"weather") {
            for (auto& l : lines)
                if (l.id == tok) {
                    gapIf(prevWx ? gap * 2 : gap);
                    prevWx = false;
                    ops.push_back({l.t, l.f, l.c, ax(l.w), y});
                    y += l.h;
                }
            continue;
        }
        if (!head.empty()) { gapIf(gap * 2); ops.push_back({head, fWx.get(), cWx, ax(headW), y}); y += headH; prevWx = true; }
        if (!grid.empty()) {
            gapIf(head.empty() ? gap * 2 : gap);
            prevWx = true;
            float cy = y;
            for (size_t r = 0; r < nr; r++) {
                float cx = ax(gridW);
                for (size_t c = 0; c < nc; c++) {
                    if (c < grid[r].size() && !grid[r][c].t.empty()) {
                        Cell& cell = grid[r][c];
                        ops.push_back({cell.t, cell.kind == 1 ? fIcon.get() : fWx.get(), cell.acc ? cWd : cWx,
                                       cx + (cw[c] - cell.z.Width) / 2, cy + (rh[r] - cell.z.Height) / 2});
                    }
                    cx += cw[c] + gapX;
                }
                cy += rh[r] + gapY;
            }
            y += gridH;
        }
    }

    const float pad = std::max(0, s.padding) * k;
    const float sd = s.shadow ? std::max(0, s.shadowOffset) * k : 0.f;
    int W = std::clamp((int)std::ceil(Wc + 2 * pad + sd) + 2, 4, 4000);
    int H = std::clamp((int)std::ceil(y + 2 * pad + sd) + 2, 4, 4000);

    // DIB section for UpdateLayeredWindow
    HDC scr = GetDC(nullptr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP hb = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hb) { ReleaseDC(nullptr, scr); return; }
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

        SolidBrush shBr(MakeColor(s.shadowColor, s.shadowOpacity * s.textOpacity / 100));
        for (auto& op : ops) {
            if (sd > 0) g.DrawString(op.t.c_str(), -1, op.f, PointF(op.x + pad + sd, op.y + pad + sd), &sf, &shBr);
            SolidBrush br(op.c);
            g.DrawString(op.t.c_str(), -1, op.f, PointF(op.x + pad, op.y + pad), &sf, &br);
        }
    }

    SIZE sz = {W, H};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hw, scr, nullptr, &sz, mem, &src, 0, &bf, ULW_ALPHA);

    // position
    MonList ml;
    EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&ml);
    if (!ml.v.empty()) {
        size_t idx = (size_t)std::max(0, s.monitor);
        if (idx >= ml.v.size()) idx = 0;
        RECT rc = ml.v[idx].rcWork;
        int mx = s.marginX, my = s.marginY;
        const std::wstring& p = s.position;
        int x, yy;
        if (p.find(L"left") != std::wstring::npos) x = rc.left + mx;
        else if (p.find(L"right") != std::wstring::npos) x = rc.right - mx - W;
        else x = (rc.left + rc.right - W) / 2;
        if (p.find(L"top") != std::wstring::npos) yy = rc.top + my;
        else if (p.find(L"bottom") != std::wstring::npos) yy = rc.bottom - my - H;
        else yy = (rc.top + rc.bottom - H) / 2;
        POINT pt = {x, yy};
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
    if (g_dirty.load() || TimeKey(st, sec) != g_lastKey) Render();
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_TIMER: Tick(); return 0;
        case WM_APP_RENDER: g_dirty.store(true); Render(); return 0;
        case WM_NCHITTEST: return HTTRANSPARENT;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_NCDESTROY:
            if (g_hwnd.load() == h) g_hwnd.store(nullptr);
            return DefWindowProcW(h, m, w, l);
    }
    return DefWindowProcW(h, m, w, l);
}

// ───────────────────────── placement behind the icons ─────────────────────────

// True once the desktop icon list exists, i.e. the shell has finished creating the desktop.
static bool DesktopReady(HWND pm) {
    HWND dv = FindWindowExW(pm, nullptr, L"SHELLDLL_DefView", nullptr);
    for (HWND ww = nullptr; !dv && (ww = FindWindowExW(nullptr, ww, L"WorkerW", nullptr)) != nullptr;) {
        DWORD pid = 0;
        GetWindowThreadProcessId(ww, &pid);
        if (pid == GetCurrentProcessId())
            dv = FindWindowExW(ww, nullptr, L"SHELLDLL_DefView", nullptr);
    }
    return dv && FindWindowExW(dv, nullptr, L"SysListView32", nullptr);
}

static BOOL CALLBACK FindWorkerProc(HWND h, LPARAM l) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    if (FindWindowExW(h, nullptr, L"SHELLDLL_DefView", nullptr)) {
        HWND candidate = FindWindowExW(nullptr, h, L"WorkerW", nullptr);
        if (candidate) {
            DWORD candidatePid = 0;
            GetWindowThreadProcessId(candidate, &candidatePid);
            if (candidatePid == GetCurrentProcessId()) *(HWND*)l = candidate;
        }
    }
    return TRUE;
}

static bool TryFindHost(HWND pm, HWND& host, HWND& after) {
    // Windows 11 24H2+: SHELLDLL_DefView and WorkerW both inside Progman
    HWND ww = FindWindowExW(pm, nullptr, L"WorkerW", nullptr);
    if (ww) {
        HWND dv = FindWindowExW(pm, nullptr, L"SHELLDLL_DefView", nullptr);
        if (dv) { host = pm; after = dv; return true; }
    }
    // Windows 10 / early 11: the WorkerW right after the one that holds the icons
    HWND found = nullptr;
    EnumWindows(FindWorkerProc, (LPARAM)&found);
    if (found) { host = found; after = HWND_TOP; return true; }
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

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

static HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            kClass, &module)) return nullptr;
    return module;
}

struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t proc;
    void* procParam;
    bool executed = false;
};

static LRESULT CALLBACK RunFromWindowThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (message->message == g_runFromWindowThreadMsg) {
            auto* param = reinterpret_cast<RunFromWindowThreadParam*>(message->lParam);
            if (param && param->proc && !param->executed) {
                param->executed = true;
                param->proc(param->procParam);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static bool RunFromWindowThread(HWND window, RunFromWindowThreadProc_t proc, void* procParam) {
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId || !g_runFromWindowThreadMsg) return false;
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHook,
        GetCurrentModuleHandle(), threadId);
    if (!hook) {
        // A thread-specific hook in the same process can also use a null HMODULE.
        hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHook, nullptr, threadId);
    }
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());
        return false;
    }

    RunFromWindowThreadParam param{proc, procParam};
    // SendMessage is intentionally synchronous: param is stack-allocated and must
    // remain alive until the shell-thread callback has completed.
    SendMessageW(window, g_runFromWindowThreadMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return param.executed;
}

struct UiCreateParams { HWND host; HWND after; bool created = false; };

static void WINAPI CreateUiOnShellThread(void* raw) {
    auto* params = static_cast<UiCreateParams*>(raw);
    HINSTANCE module = GetCurrentModuleHandle();
    if (!module) return;

    // If Explorer recreated its desktop host, the old child window has already
    // received WM_NCDESTROY. Release its class and GDI+ session before recreating.
    if (g_gdiplusToken) {
        UnregisterClassW(kClass, module);
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    GdiplusStartupInput input;
    if (GdiplusStartup(&g_gdiplusToken, &input, nullptr) != Ok) {
        g_gdiplusToken = 0;
        return;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = module;
    wc.lpszClassName = kClass;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return;
    }

    HWND window = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE |
        WS_EX_NOPARENTNOTIFY, kClass, L"", WS_CHILD | WS_VISIBLE,
        0, 0, 1, 1, params->host, nullptr, module, nullptr);
    if (!window) {
        Wh_Log(L"CreateWindowEx failed: %lu", GetLastError());
        UnregisterClassW(kClass, module);
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return;
    }

    g_host = params->host;
    g_hwnd.store(window);
    SetWindowPos(window, params->after, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetTimer(window, 1, 1000, nullptr);
    g_dirty.store(true);
    Render();
    params->created = true;
    SetEvent(g_evRefresh);
}

static void WINAPI DestroyUiOnShellThread(void*) {
    HWND window = g_hwnd.load();
    if (window && IsWindow(window)) {
        KillTimer(window, 1);
        DestroyWindow(window);
    }
    g_hwnd.store(nullptr);
    HINSTANCE module = GetCurrentModuleHandle();
    UnregisterClassW(kClass, module);
    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    g_host = nullptr;
}

static void TryCreateUi() {
    if (g_unloading.load() || g_hwnd.load()) return;
    bool expected = false;
    if (!g_uiCreating.compare_exchange_strong(expected, true)) return;

    if (!g_unloading.load() && !g_hwnd.load()) {
        HWND host = nullptr, after = nullptr;
        if (FindHost(host, after)) {
            UiCreateParams params{host, after};
            RunFromWindowThread(host, CreateUiOnShellThread, &params);
        }
    }
    g_uiCreating.store(false);
}

static void CALLBACK DesktopWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
        LONG idObject, LONG idChild, DWORD, DWORD) {
    g_activeWinEventCallbacks.fetch_add(1);
    if (!g_unloading.load() && event == EVENT_OBJECT_CREATE &&
        idObject == OBJID_WINDOW && idChild == CHILDID_SELF && hwnd) {
        wchar_t className[64] = {};
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            (wcscmp(className, L"Progman") == 0 ||
             wcscmp(className, L"WorkerW") == 0 ||
             wcscmp(className, L"SHELLDLL_DefView") == 0 ||
             wcscmp(className, L"SysListView32") == 0)) {
            TryCreateUi();
        }
    }
    g_activeWinEventCallbacks.fetch_sub(1);
}

// ───────────────────────── Windhawk entry points ─────────────────────────

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_cs);
    LoadSettings();
    g_evStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_evRefresh = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_evStop || !g_evRefresh) {
        if (g_evStop) CloseHandle(g_evStop);
        if (g_evRefresh) CloseHandle(g_evRefresh);
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    g_runFromWindowThreadMsg = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_desktop-clock-weather");
    if (!g_runFromWindowThreadMsg) {
        CloseHandle(g_evStop);
        CloseHandle(g_evRefresh);
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    g_thWx = CreateThread(nullptr, 0, WeatherThread, nullptr, 0, nullptr);
    if (!g_thWx) {
        CloseHandle(g_evStop);
        CloseHandle(g_evRefresh);
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }

    g_winEventHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE,
        GetCurrentModuleHandle(), DesktopWinEventProc, GetCurrentProcessId(), 0,
        WINEVENT_INCONTEXT);
    if (!g_winEventHook) {
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
        SetEvent(g_evStop);
        WaitForSingleObject(g_thWx, INFINITE);
        CloseHandle(g_thWx);
        g_thWx = nullptr;
        CloseHandle(g_evStop);
        CloseHandle(g_evRefresh);
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }
    return TRUE;
}

// Windhawk calls this after all mods have initialized. If the desktop already
// exists, create the widget immediately; otherwise the WinEvent hook catches
// the later creation of Progman/WorkerW/SHELLDLL_DefView without polling.
void Wh_ModAfterInit() {
    TryCreateUi();
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

    if (old.city != now.city || old.tempUnit != now.tempUnit || old.showWeather != now.showWeather ||
        old.locale != now.locale || old.updateMinutes != now.updateMinutes || old.order != now.order) {
        SetEvent(g_evRefresh);
    }
    HWND h = g_hwnd;
    if (h) PostMessageW(h, WM_APP_RENDER, 0, 0);
}

void Wh_ModUninit() {
    g_unloading.store(true);
    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    // Wait for callbacks and a possible in-flight shell-thread creation to finish.
    while (g_activeWinEventCallbacks.load() != 0 || g_uiCreating.load()) Sleep(1);
    SetEvent(g_evStop);

    // The asynchronous WinHTTP request observes g_evStop and closes its request
    // handle before the worker exits, so no callback can outlive this module.
    if (g_thWx) { WaitForSingleObject(g_thWx, INFINITE); CloseHandle(g_thWx); g_thWx = nullptr; }

    HWND host = g_host;
    if (host && IsWindow(host) && g_gdiplusToken) {
        if (!RunFromWindowThread(host, DestroyUiOnShellThread, nullptr)) {
            Wh_Log(L"Could not dispatch widget cleanup; sending WM_CLOSE to the child window");
            HWND window = g_hwnd.load();
            if (window && IsWindow(window)) SendMessageW(window, WM_CLOSE, 0, 0);
            UnregisterClassW(kClass, GetCurrentModuleHandle());
            GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
            g_host = nullptr;
        }
    } else if (g_gdiplusToken) {
        // The shell host may already have been destroyed during logoff/shutdown.
        // If the child is still alive, WM_CLOSE is processed by its owning thread.
        HWND window = g_hwnd.load();
        if (window && IsWindow(window)) SendMessageW(window, WM_CLOSE, 0, 0);
        UnregisterClassW(kClass, GetCurrentModuleHandle());
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    CloseHandle(g_evStop);
    CloseHandle(g_evRefresh);
    DeleteCriticalSection(&g_cs);
}
