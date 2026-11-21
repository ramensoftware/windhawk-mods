// ==WindhawkMod==
// @id              taskbar-weather
// @name            Taskbar Weather
// @description     Current weather on the taskbar with a details card on hover. No Widgets, browser or API key needed
// @version         1.5.2
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinhttp -lgdiplus -lgdi32 -luser32 -ld2d1 -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Weather

The current weather at the left end of the taskbar, with a details card when
you hover over it. It works on its own, without Windows Widgets.

![Taskbar Weather preview](https://raw.githubusercontent.com/DavidHiFi/davids-windhawk-mods/main/media/previews/taskbar-weather.png)

## Features

- **Weather icon, temperature and conditions** on the taskbar, updated every
  ten minutes by default.
- **Hover card** with the feels-like temperature, today's high and low,
  humidity and wind.
- **Click to refresh** at any time.
- **Follows the Windows theme:** dark text on a light taskbar, light text on a
  dark one. A setting can force either.
- **Celsius and km/h, or Fahrenheit and mph.**
- **No Widgets, MSN, Edge or location permission.** Data comes from
  Open-Meteo, with no account or API key.

## How it differs from other weather mods

[Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization)
can put weather text inside the clock, and
[Micrographics Taskbar Clock](https://windhawk.net/mods/micrographics-taskbar-clock)
replaces the clock with one that can show a weather line.
[TopBar for Windows](https://windhawk.net/mods/windhawk-topbar) has a weather
flyout in its own bar, and Desktop Live Overlay shows weather on the desktop.
This mod adds its own icon and label at the left end of the stock taskbar,
keeps the regular clock, shows a details card on hover, and needs no other
mod.

## Setup

Open the settings and enter your town's **latitude** and **longitude** in
decimal degrees with a dot, for example `51.5074` and `-0.1278` (in most map
apps, right-click a place to copy them). Add a **place name** to show it in the
hover card.

## Privacy and network use

The mod needs internet access. Your latitude and longitude, rounded to three
decimals (about 100 m), are sent to `api.open-meteo.com` with every request:
when the taskbar appears, after every settings change, at each update
interval, once a minute while requests fail, when you click the label, when
the PC wakes up and when the clock or time zone changes. Open-Meteo also sees
your IP address. Nothing else is sent. Nothing is sent until both coordinates
are filled in; clear them to stop all requests.

## Notes

- The label sits on top of the taskbar at the left offset you choose and does
  not push other buttons aside. With a centered taskbar, turn off Windows
  Widgets (it shows its own weather in the same place). With a left-aligned
  taskbar, raise the left offset so the label clears the Start button.
- Only the main taskbar shows the weather.
- If a request fails, the last reading stays and the card says when it was
  fetched. A reading is marked as stale once it is older than 30 minutes, or
  twice the update interval if that is longer.
- Vertical taskbars (left or right on Windows 10) are not supported. The
  label is not shown on them.

## Credits

Weather data by [Open-Meteo.com](https://open-meteo.com/) (CC BY 4.0).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- latitude: ""
  $name: Latitude
  $description: Decimal degrees with a dot, for example 51.5074. North is positive.
- longitude: ""
  $name: Longitude
  $description: Decimal degrees with a dot, for example -0.1278. East is positive.
- placeName: ""
  $name: Place name
  $description: Shown in the hover card. Leave empty to hide it.
- units: metric
  $name: Units
  $options:
  - metric: Celsius and km/h
  - imperial: Fahrenheit and mph
- textColor: auto
  $name: Text color
  $options:
  - auto: Follow the Windows theme
  - light: Light text, for dark taskbars
  - dark: Dark text, for light taskbars
- updateMinutes: 10
  $name: Update interval in minutes
  $description: 5 to 60. Open-Meteo refreshes current weather every 15 minutes.
- leftOffset: 16
  $name: Left offset
  $description: Pixels from the left edge of the taskbar at 100% scaling, 0 to 1000.
- width: 220
  $name: Maximum width
  $description: Pixels at 100% scaling, 120 to 400.
- fontFamily: Segoe UI
  $name: Font
- fontSize: 11
  $name: Font size
  $description: 9 to 22.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d2d1.h>
#include <dwrite.h>
#include <gdiplus.h>
#include <windows.h>
#include <winhttp.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <cwchar>
#include <mutex>
#include <sstream>
#include <string>

using namespace Gdiplus;
using Microsoft::WRL::ComPtr;

namespace {

constexpr wchar_t kClass[] = L"WindhawkTaskbarWeather";
constexpr wchar_t kCardClass[] = L"WindhawkTaskbarWeatherCard";
constexpr wchar_t kControllerClass[] = L"WindhawkTaskbarWeatherController";
constexpr wchar_t kCacheKey[] = L"lastWeatherV2";

constexpr UINT kDataMessage = WM_APP + 1;
constexpr UINT_PTR kCheckTimer = 1;
constexpr UINT_PTR kFadeTimer = 2;
constexpr float kCardWidth = 320, kShadow = 12, kPad = 18;
// Taskbar row layout in logical pixels: glyph slot, gap, trailing room.
constexpr float kIconLeft = 10, kIconGap = 8, kTextRightPad = 6;

// Taskbar label colors. Dark taskbar: Catppuccin Mocha. Light taskbar:
// Catppuccin Latte.
struct Palette {
    ARGB text, sun, cloud, drop, hover;
};
constexpr Palette kDarkBar{0xFFCDD6F4, 0xFFF9E2AF, 0xFFCDD6F4, 0xFF89B4FA,
                           0x9645475A};
constexpr Palette kLightBar{0xFF4C4F69, 0xFFDF8E1D, 0xFF8C8FA1, 0xFF1E66F5,
                            0x1E000000};

// The hover card always draws its own dark background.
constexpr ARGB kBase = 0xFF1E1E2E, kSurface1 = 0xFF45475A,
               kSubtext0 = 0xFFA6ADC8, kText = 0xFFCDD6F4, kPeach = 0xFFFAB387;

enum class TextColor { Auto, Light, Dark };

struct Settings {
    std::wstring latitude, longitude;  // Normalized, used in the URL.
    std::wstring place, fontFamily;
    bool imperial = false;
    TextColor textColor = TextColor::Auto;
    int interval = 10, left = 16, width = 220, fontSize = 11;
} settings;

struct Reading {
    double temperature = 0, feels = 0, humidity = 0, wind = 0,
           windDirection = 0, high = 0, low = 0;
    int code = 0;
    bool day = true, details = false, daily = false, valid = false,
         failed = false;
    long long fetched = 0;  // Unix time of the fetch.
};

HINSTANCE instance;
ULONG_PTR graphicsToken;
HANDLE stopEvent, refreshEvent, uiThread, fetchThread;
std::atomic<HWND> weatherWindow, controllerWindow;
std::mutex dataMutex;
Reading reading;

// UI thread only.
HWND cardWindow;
UINT taskbarCreatedMessage;
// UI thread only. Released at the end of UiWorker, never by a destructor at
// process exit, when the graphics threads are already gone.
[[clang::no_destroy]] ComPtr<ID2D1Factory> d2dFactory;
[[clang::no_destroy]] ComPtr<IDWriteFactory> writeFactory;
bool hover = false, lightTaskbar = false;
BYTE cardAlpha = 0;
int contentWidth = -1;
RECT lastClient{};
UINT lastDpi = 0;
int lastContentWidth = -1;

Color WithAlpha(ARGB c, BYTE a) {
    return Color(a, (c >> 16) & 255, (c >> 8) & 255, c & 255);
}

std::wstring StringSetting(PCWSTR key) {
    return WindhawkUtils::StringSetting::make(key).get();
}

// Accepts plain decimal degrees only, such as "51.5074" or "-0.1278", and
// returns them normalized to four decimals.
bool ParseCoordinate(std::wstring s, double bound, std::wstring& out) {
    s.erase(0, s.find_first_not_of(L" \t"));
    s.erase(s.find_last_not_of(L" \t") + 1);
    if (s.empty() || s.find_first_not_of(L"0123456789.-+") != s.npos) {
        return false;
    }
    wchar_t* end = nullptr;
    double v = wcstod(s.c_str(), &end);
    if (!end || *end || !std::isfinite(v) || std::abs(v) > bound) {
        return false;
    }
    wchar_t buffer[32];
    swprintf(buffer, 32, L"%.3f", v);
    out = buffer;
    return true;
}

bool LocationConfigured() {
    return !settings.latitude.empty() && !settings.longitude.empty();
}

void LoadSettings() {
    if (!ParseCoordinate(StringSetting(L"latitude"), 90, settings.latitude) ||
        !ParseCoordinate(StringSetting(L"longitude"), 180,
                         settings.longitude)) {
        settings.latitude.clear();
        settings.longitude.clear();
    }
    settings.place = StringSetting(L"placeName");
    settings.fontFamily = StringSetting(L"fontFamily");
    if (settings.fontFamily.empty()) {
        settings.fontFamily = L"Segoe UI";
    }
    settings.imperial = StringSetting(L"units") == L"imperial";
    std::wstring color = StringSetting(L"textColor");
    settings.textColor = color == L"light"  ? TextColor::Light
                         : color == L"dark" ? TextColor::Dark
                                            : TextColor::Auto;
    settings.interval = std::clamp(Wh_GetIntSetting(L"updateMinutes"), 5, 60);
    settings.left = std::clamp(Wh_GetIntSetting(L"leftOffset"), 0, 1000);
    settings.width = std::clamp(Wh_GetIntSetting(L"width"), 120, 400);
    settings.fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 9, 22);
}

bool SystemUsesLightTaskbar() {
    DWORD value = 0, size = sizeof(value);
    return RegGetValueW(
               HKEY_CURRENT_USER,
               L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
               L"Personalize",
               L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
               &size) == ERROR_SUCCESS &&
           value;
}

void UpdateTheme() {
    lightTaskbar = settings.textColor == TextColor::Dark ||
                   (settings.textColor == TextColor::Auto &&
                    SystemUsesLightTaskbar());
}

const Palette& BarPalette() {
    return lightTaskbar ? kLightBar : kDarkBar;
}

// Keep the last successful reading across a mod reload or a short network
// outage. Matching coordinates and units keep an old town's reading away.
void LoadCachedReading() {
    wchar_t buffer[1024]{};
    if (!Wh_GetStringValue(kCacheKey, buffer, ARRAYSIZE(buffer))) {
        return;
    }
    std::wistringstream input(buffer);
    std::wstring lat, lon;
    int imperial = 0;
    Reading cached;
    if (!(input >> lat >> lon >> imperial >> cached.fetched >>
          cached.temperature >> cached.feels >> cached.humidity >>
          cached.wind >> cached.windDirection >> cached.high >> cached.low >>
          cached.code >> cached.day >> cached.details >> cached.daily)) {
        return;
    }
    long long age = static_cast<long long>(std::time(nullptr)) - cached.fetched;
    if (lat != settings.latitude || lon != settings.longitude ||
        (imperial != 0) != settings.imperial || age < 0 || age > 86400 ||
        !std::isfinite(cached.temperature)) {
        return;
    }
    cached.valid = true;
    reading = cached;
}

void SaveCachedReading(const Reading& r) {
    std::wostringstream out;
    out.precision(15);
    out << settings.latitude << L' ' << settings.longitude << L' '
        << (settings.imperial ? 1 : 0) << L' ' << r.fetched << L' '
        << r.temperature << L' ' << r.feels << L' ' << r.humidity << L' '
        << r.wind << L' ' << r.windDirection << L' ' << r.high << L' '
        << r.low << L' ' << r.code << L' ' << r.day << L' ' << r.details
        << L' ' << r.daily;
    Wh_SetStringValue(kCacheKey, out.str().c_str());
}

const wchar_t* Condition(int code) {
    switch (code) {
        case 0:
            return L"Clear";
        case 1:
            return L"Mostly clear";
        case 2:
            return L"Partly cloudy";
        case 3:
            return L"Cloudy";
        case 45:
        case 48:
            return L"Fog";
        case 51:
        case 53:
        case 55:
        case 56:
        case 57:
            return L"Drizzle";
        case 61:
        case 63:
        case 65:
        case 66:
        case 67:
            return L"Rain";
        case 71:
        case 73:
        case 75:
        case 77:
            return L"Snow";
        case 80:
        case 81:
        case 82:
            return L"Rain showers";
        case 85:
        case 86:
            return L"Snow showers";
        case 95:
        case 96:
        case 99:
            return L"Thunderstorms";
        default:
            return L"Weather";
    }
}

enum class Precipitation { None, Rain, Snow, Storm };

Precipitation PrecipitationOf(int code) {
    if (code >= 95) {
        return Precipitation::Storm;
    }
    if ((code >= 71 && code <= 77) || code == 85 || code == 86) {
        return Precipitation::Snow;
    }
    if (code >= 51) {
        return Precipitation::Rain;
    }
    return Precipitation::None;
}

const wchar_t* Compass(double degrees) {
    static const wchar_t* names[]{L"N",  L"NNE", L"NE", L"ENE", L"E",  L"ESE",
                                  L"SE", L"SSE", L"S",  L"SSW", L"SW", L"WSW",
                                  L"W",  L"WNW", L"NW", L"NNW"};
    int index =
        (int)std::lround(std::fmod(std::abs(degrees), 360.) / 22.5) % 16;
    return names[index];
}

std::wstring Whole(double v) {
    return std::to_wstring((int)std::lround(v));
}

const wchar_t* TemperatureUnit() {
    return settings.imperial ? L"°F" : L"°C";
}

const wchar_t* SpeedUnit() {
    return settings.imperial ? L" mph " : L" km/h ";
}

std::wstring Label(const Reading& r) {
    if (!LocationConfigured()) {
        return L"Set weather location";
    }
    if (r.valid) {
        return Whole(r.temperature) + TemperatureUnit() + L"  " +
               Condition(r.code);
    }
    return r.failed ? L"Weather unavailable" : L"Loading weather...";
}

// Formats a Unix time as a clock time in the user's own locale.
std::wstring LocalClock(long long unixTime) {
    ULARGE_INTEGER value;
    value.QuadPart = (unsigned long long)(unixTime + 11644473600LL) * 10000000ULL;
    FILETIME utc{value.LowPart, value.HighPart}, local;
    SYSTEMTIME time;
    wchar_t buffer[64];
    if (!FileTimeToLocalFileTime(&utc, &local) ||
        !FileTimeToSystemTime(&local, &time) ||
        !GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &time,
                         nullptr, buffer, ARRAYSIZE(buffer))) {
        return L"";
    }
    return buffer;
}

bool Number(const std::string& s, const char* name, double& value) {
    size_t p = s.find(std::string("\"") + name + "\"");
    if (p == s.npos) {
        return false;
    }
    p = s.find(':', p);
    if (p == s.npos) {
        return false;
    }
    // Daily values arrive as one-element arrays.
    p = s.find_first_not_of(" [", p + 1);
    if (p == s.npos) {
        return false;
    }
    const char* begin = s.c_str() + p;
    char* end;
    value = strtod(begin, &end);
    return end != begin && std::isfinite(value);
}

bool Fetch(Reading& next) {
    const ULONGLONG deadline = GetTickCount64() + 30000;
    std::wstring path =
        L"/v1/forecast?latitude=" + settings.latitude +
        L"&longitude=" + settings.longitude +
        L"&current=temperature_2m,apparent_temperature,relative_humidity_2m,"
        L"weather_code,is_day,wind_speed_10m,wind_direction_10m"
        L"&daily=temperature_2m_max,temperature_2m_min&forecast_days=1"
        L"&timezone=auto";
    if (settings.imperial) {
        path += L"&temperature_unit=fahrenheit&wind_speed_unit=mph";
    }

    HINTERNET session =
        WinHttpOpen(L"WindhawkTaskbarWeather/" WH_MOD_VERSION,
                    WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, nullptr, nullptr, 0);
    if (!session) {
        return false;
    }
    WinHttpSetTimeouts(session, 5000, 5000, 5000, 5000);
    HINTERNET connection = WinHttpConnect(session, L"api.open-meteo.com",
                                          INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET request =
        connection ? WinHttpOpenRequest(connection, L"GET", path.c_str(),
                                        nullptr, WINHTTP_NO_REFERER,
                                        WINHTTP_DEFAULT_ACCEPT_TYPES,
                                        WINHTTP_FLAG_SECURE)
                   : nullptr;
    bool ok = request &&
              WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                 WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
              WinHttpReceiveResponse(request, nullptr);
    DWORD status = 0, statusSize = sizeof(status);
    if (ok) {
        ok = WinHttpQueryHeaders(
                 request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                 nullptr, &status, &statusSize, nullptr) &&
             status == 200;
    }
    std::string body;
    while (ok) {
        if (GetTickCount64() > deadline ||
            WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
            ok = false;
            break;
        }
        DWORD available = 0, read = 0;
        if (!WinHttpQueryDataAvailable(request, &available)) {
            ok = false;
            break;
        }
        if (!available) {
            break;
        }
        if (body.size() + available > 65536) {
            ok = false;
            break;
        }
        size_t old = body.size();
        body.resize(old + available);
        if (!WinHttpReadData(request, body.data() + old, available, &read)) {
            ok = false;
            break;
        }
        body.resize(old + read);
    }
    if (request) {
        WinHttpCloseHandle(request);
    }
    if (connection) {
        WinHttpCloseHandle(connection);
    }
    WinHttpCloseHandle(session);
    if (!ok || WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
        return false;
    }

    size_t p = body.find("\"current\":");
    if (p == body.npos) {
        return false;
    }
    std::string current = body.substr(p);
    double temperature, code, day;
    if (!Number(current, "temperature_2m", temperature) ||
        !Number(current, "weather_code", code) ||
        !Number(current, "is_day", day)) {
        return false;
    }
    next.temperature = temperature;
    next.code = (int)code;
    next.day = day != 0;
    next.valid = true;
    next.failed = false;
    next.fetched = std::time(nullptr);
    next.details = Number(current, "apparent_temperature", next.feels) &&
                   Number(current, "relative_humidity_2m", next.humidity) &&
                   Number(current, "wind_speed_10m", next.wind) &&
                   Number(current, "wind_direction_10m", next.windDirection);
    p = body.find("\"daily\":");
    if (p != body.npos) {
        std::string daily = body.substr(p);
        next.daily = Number(daily, "temperature_2m_max", next.high) &&
                     Number(daily, "temperature_2m_min", next.low);
    }
    return true;
}

void RoundedRect(GraphicsPath& path, float x, float y, float w, float h,
                 float r) {
    float d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

// Draws the weather glyph in a 24 by 22 logical-pixel box scaled by `scale`.
void DrawIcon(Graphics& g, float x, float y, float scale, int code, bool day,
              const Palette& colors) {
    auto state = g.Save();
    g.TranslateTransform(x, y);
    g.ScaleTransform(scale, scale);
    SolidBrush sun{Color(colors.sun)}, cloud{Color(colors.cloud)};
    if (code < 3) {
        // The moon is the sun with a disc clipped away, so the cut stays
        // transparent on any taskbar color.
        auto sunState = g.Save();
        if (!day) {
            GraphicsPath cut;
            cut.AddEllipse(5.f, -2.f, 12.f, 12.f);
            g.SetClip(&cut, CombineModeExclude);
        }
        g.FillEllipse(&sun, 0.f, 0.f, 14.f, 14.f);
        g.Restore(sunState);
    }
    if (code != 0) {
        g.FillEllipse(&cloud, 3.f, 6.f, 10.f, 10.f);
        g.FillEllipse(&cloud, 9.f, 3.f, 12.f, 12.f);
        g.FillEllipse(&cloud, 16.f, 7.f, 8.f, 9.f);
        g.FillRectangle(&cloud, 7.f, 11.f, 13.f, 5.f);
    }
    switch (PrecipitationOf(code)) {
        case Precipitation::Rain: {
            Pen drops(Color(colors.drop), 1.5f);
            g.DrawLine(&drops, 9.f, 18.f, 7.f, 21.f);
            g.DrawLine(&drops, 18.f, 18.f, 16.f, 21.f);
            break;
        }
        case Precipitation::Snow: {
            SolidBrush flakes{Color(colors.drop)};
            g.FillEllipse(&flakes, 6.5f, 18.f, 3.f, 3.f);
            g.FillEllipse(&flakes, 11.5f, 19.f, 3.f, 3.f);
            g.FillEllipse(&flakes, 16.5f, 18.f, 3.f, 3.f);
            break;
        }
        case Precipitation::Storm: {
            SolidBrush bolt{Color(colors.sun)};
            PointF points[]{{14.f, 14.f}, {9.5f, 19.f}, {12.5f, 19.f},
                            {10.5f, 23.f}, {16.f, 17.5f}, {13.f, 17.5f},
                            {15.f, 14.f}};
            g.FillPolygon(&bolt, points, ARRAYSIZE(points));
            break;
        }
        case Precipitation::None:
            break;
    }
    g.Restore(state);
}

// Ink bounds of the glyph inside the 24 by 22 box that DrawIcon fills, so the
// row can centre the glyph itself and keep one gap to the text.
float IconInkWidth(int code) {
    return code == 0 ? 14.f : 24.f;
}

float IconInkMid(int code) {
    if (code == 0) {
        return 7.f;  // Sun or moon: 0 to 14.
    }
    if (code < 3) {
        return 8.f;  // Sun behind cloud: 0 to 16.
    }
    // Cloud: 3 to 16. Drops, flakes and bolts reach about 22.
    return PrecipitationOf(code) != Precipitation::None ? 12.f : 9.5f;
}

float TextX(int code) {
    return kIconLeft + IconInkWidth(code) + kIconGap;
}

// Row text goes through DirectWrite. GDI+ DrawString hints small glyphs
// coarsely on a transparent layered window.
struct RowFont {
    ComPtr<IDWriteTextFormat> format;
    float ascent = 0, capHeight = 0, lineSpacing = 0;
};

RowFont ResolveRowFont() {
    RowFont result;
    if (!writeFactory &&
        FAILED(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf())))) {
        return result;
    }
    ComPtr<IDWriteFontCollection> fonts;
    if (FAILED(writeFactory->GetSystemFontCollection(&fonts))) {
        return result;
    }
    // Prefer a real semibold face. Older font installs expose it as a
    // separate "SemBd" family.
    std::wstring family;
    DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_SEMI_BOLD;
    ComPtr<IDWriteFont> font;
    auto find = [&](const std::wstring& name, DWRITE_FONT_WEIGHT want) {
        UINT32 index;
        BOOL exists = FALSE;
        ComPtr<IDWriteFontFamily> candidate;
        ComPtr<IDWriteFont> match;
        if (FAILED(fonts->FindFamilyName(name.c_str(), &index, &exists)) ||
            !exists || FAILED(fonts->GetFontFamily(index, &candidate)) ||
            FAILED(candidate->GetFirstMatchingFont(
                want, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                &match))) {
            return false;
        }
        family = name;
        weight = want;
        font = match;
        return true;
    };
    const std::wstring& name = settings.fontFamily;
    bool semibold = find(name, DWRITE_FONT_WEIGHT_SEMI_BOLD) &&
                    std::abs((int)font->GetWeight() - 600) <= 50;
    if (!semibold && !find(name + L" SemBd", DWRITE_FONT_WEIGHT_NORMAL) &&
        !find(name, DWRITE_FONT_WEIGHT_SEMI_BOLD)) {
        if (!find(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_SEMI_BOLD)) {
            find(L"Segoe UI", DWRITE_FONT_WEIGHT_SEMI_BOLD);
        }
    }
    if (!font ||
        FAILED(writeFactory->CreateTextFormat(
            family.c_str(), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, (FLOAT)settings.fontSize, L"",
            &result.format))) {
        return RowFont{};
    }
    result.format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    result.format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteInlineObject> ellipsis;
    DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    if (SUCCEEDED(writeFactory->CreateEllipsisTrimmingSign(
            result.format.Get(), &ellipsis))) {
        result.format->SetTrimming(&trimming, ellipsis.Get());
    }
    DWRITE_FONT_METRICS m;
    font->GetMetrics(&m);
    float unit = (float)settings.fontSize / m.designUnitsPerEm;
    result.ascent = m.ascent * unit;
    result.capHeight = m.capHeight * unit;
    result.lineSpacing = (m.ascent + m.descent + m.lineGap) * unit;
    return result;
}

float TextWidth(const RowFont& font, const std::wstring& s) {
    ComPtr<IDWriteTextLayout> layout;
    DWRITE_TEXT_METRICS m;
    if (!font.format ||
        FAILED(writeFactory->CreateTextLayout(s.c_str(), (UINT32)s.size(),
                                              font.format.Get(), 10000.f,
                                              100.f, &layout)) ||
        FAILED(layout->GetMetrics(&m))) {
        return 0;
    }
    return m.widthIncludingTrailingWhitespace;
}

// Draws the temperature and conditions over the GDI+ icon layer in `dc`.
void DrawRowText(HDC dc, int w, int h, float scale, const Reading& r) {
    RowFont font = ResolveRowFont();
    if (!font.format) {
        return;
    }
    if (!d2dFactory &&
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 d2dFactory.GetAddressOf()))) {
        return;
    }
    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.f * scale, 96.f * scale);
    ComPtr<ID2D1DCRenderTarget> target;
    RECT bounds{0, 0, w, h};
    if (FAILED(d2dFactory->CreateDCRenderTarget(&props, &target)) ||
        FAILED(target->BindDC(dc, &bounds))) {
        return;
    }
    // Transparent pixels cannot take ClearType, so use symmetric grayscale
    // smoothing.
    ComPtr<IDWriteRenderingParams> base, params;
    if (SUCCEEDED(writeFactory->CreateRenderingParams(&base)) &&
        SUCCEEDED(writeFactory->CreateCustomRenderingParams(
            base->GetGamma(), base->GetEnhancedContrast(), 0.f,
            DWRITE_PIXEL_GEOMETRY_FLAT, DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
            &params))) {
        target->SetTextRenderingParams(params.Get());
    }
    target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    ComPtr<ID2D1SolidColorBrush> brush;
    float height = h / scale, textX = TextX(r.code),
          textWidth = w / scale - textX - kTextRightPad;
    auto snap = [&](float v) { return std::round(v * scale) / scale; };
    auto line = [&](const std::wstring& s, float baseline) {
        ComPtr<IDWriteTextLayout> layout;
        if (FAILED(writeFactory->CreateTextLayout(
                s.c_str(), (UINT32)s.size(), font.format.Get(), textWidth,
                height, &layout))) {
            return;
        }
        DWRITE_LINE_METRICS m;
        UINT32 count = 0;
        float offset = font.ascent;
        if (SUCCEEDED(layout->GetLineMetrics(&m, 1, &count)) && count) {
            offset = m.baseline;
        }
        target->DrawTextLayout(D2D1::Point2F(textX, baseline - offset),
                               layout.Get(), brush.Get(),
                               D2D1_DRAW_TEXT_OPTIONS_NONE);
    };

    target->BeginDraw();
    if (SUCCEEDED(target->CreateSolidColorBrush(
            D2D1::ColorF(BarPalette().text & 0xFFFFFF), &brush))) {
        if (r.valid) {
            // Centre the ink, from the first line's cap height to the second
            // baseline, on whole pixels.
            float pitch = std::min(
                std::ceil(font.lineSpacing * scale) / scale, height / 2.f);
            float first = snap(height / 2.f + (font.capHeight - pitch) / 2.f);
            line(Whole(r.temperature) + TemperatureUnit(), first);
            line(Condition(r.code), first + pitch);
        } else {
            line(Label(r), snap((height + font.capHeight) / 2.f));
        }
    }
    target->EndDraw();
}

// A 32-bit top-down DIB selected into a memory DC, for UpdateLayeredWindow.
struct Canvas {
    HDC screen = nullptr, dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ old = nullptr;
    void* bits = nullptr;

    bool Create(int w, int h) {
        screen = GetDC(nullptr);
        dc = screen ? CreateCompatibleDC(screen) : nullptr;
        if (!dc) {
            return false;
        }
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = w;
        info.bmiHeader.biHeight = -h;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        bitmap =
            CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bitmap || !bits) {
            return false;
        }
        old = SelectObject(dc, bitmap);
        memset(bits, 0, (size_t)w * h * 4);
        return true;
    }

    ~Canvas() {
        if (old) {
            SelectObject(dc, old);
        }
        if (bitmap) {
            DeleteObject(bitmap);
        }
        if (dc) {
            DeleteDC(dc);
        }
        if (screen) {
            ReleaseDC(nullptr, screen);
        }
    }
};

Reading CurrentReading() {
    std::lock_guard lock(dataMutex);
    return reading;
}

void Paint() {
    HWND window = weatherWindow;
    if (!window) {
        return;
    }
    RECT client;
    GetClientRect(window, &client);
    int w = client.right, h = client.bottom;
    Canvas canvas;
    if (!w || !h || !canvas.Create(w, h)) {
        return;
    }
    Reading r = CurrentReading();
    const Palette& colors = BarPalette();
    float scale = GetDpiForWindow(window) / 96.f;
    {
        Bitmap bitmap(w, h, w * 4, PixelFormat32bppPARGB, (BYTE*)canvas.bits);
        Graphics g(&bitmap);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.ScaleTransform(scale, scale);
        float height = h / scale;
        if (hover) {
            // Inset the highlight so its edge stays inside the taskbar.
            GraphicsPath path;
            RoundedRect(path, 2.f, 3.f, w / scale - 4, height - 6, 6.f);
            SolidBrush background{Color(colors.hover)};
            g.FillPath(&background, &path);
        } else {
            // Alpha 1 is invisible but lets the whole label take the mouse.
            SolidBrush hitArea{Color(1, 0, 0, 0)};
            g.FillRectangle(&hitArea, 0.f, 0.f, w / scale, height);
        }
        DrawIcon(g, kIconLeft, height / 2 - IconInkMid(r.code), 1, r.code,
                 r.day, colors);
    }
    GdiFlush();
    DrawRowText(canvas.dc, w, h, scale, r);
    POINT src{};
    SIZE size{w, h};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(window, canvas.screen, nullptr, &size, canvas.dc, &src,
                        0, &blend, ULW_ALPHA);
}

// Lays out the hover card in logical pixels and returns its height. Draws
// only when `g` is set.
float Card(Graphics* g, const Reading& r) {
    FontFamily family(settings.fontFamily.c_str());
    const FontFamily* face =
        family.IsAvailable() ? &family : FontFamily::GenericSansSerif();
    Font hero(face, 38, FontStyleRegular, UnitPixel);
    Font heading(face, 16, FontStyleBold, UnitPixel);
    Font body(face, 14, FontStyleRegular, UnitPixel);
    Font small(face, 13, FontStyleRegular, UnitPixel);
    SolidBrush text{Color(kText)}, muted{Color(kSubtext0)},
        warn{Color(kPeach)};
    StringFormat format(StringFormat::GenericTypographic());
    format.SetLineAlignment(StringAlignmentCenter);
    format.SetFormatFlags(StringFormatFlagsNoWrap |
                          StringFormatFlagsMeasureTrailingSpaces);
    format.SetTrimming(StringTrimmingEllipsisCharacter);

    float y = kPad;
    auto put = [&](const std::wstring& s, const Font& font, Brush& brush,
                   float x, float height) -> float {
        if (!g) {
            return 0;
        }
        RectF box(x, y, kCardWidth - kPad - x, height), bounds;
        g->DrawString(s.c_str(), -1, &font, box, &format, &brush);
        g->MeasureString(s.c_str(), -1, &font, box, &format, &bounds);
        return bounds.Width;
    };
    auto divider = [&] {
        y += 8;
        if (g) {
            Pen line(Color(kSurface1), 1.f);
            g->DrawLine(&line, kPad, y, kCardWidth - kPad, y);
        }
        y += 10;
    };
    auto pair = [&](const std::wstring& label, const std::wstring& value,
                    float x) {
        float labelWidth = put(label, body, muted, x, 22);
        put(value, body, text, x + labelWidth + 8, 22);
    };

    if (r.valid) {
        if (g) {
            DrawIcon(*g, kPad, y + 6, 1.6f, r.code, r.day, kDarkBar);
        }
        float x = kPad + 50;
        float top = y;
        x += put(Whole(r.temperature) + L"°", hero, text, x, 50) + 12;
        put(Condition(r.code), heading, text, x, 26);
        y += 26;
        put(r.details ? L"Feels like " + Whole(r.feels) + L"°"
                      : std::wstring(TemperatureUnit()),
            body, muted, x, 22);
        y = top + 52;
        if (r.daily || r.details) {
            divider();
            if (r.daily) {
                pair(L"High", Whole(r.high) + L"°", kPad);
                pair(L"Low", Whole(r.low) + L"°", kCardWidth / 2 + 4);
                y += 26;
            }
            if (r.details) {
                pair(L"Humidity", Whole(r.humidity) + L"%", kPad);
                pair(L"Wind",
                     Whole(r.wind) + SpeedUnit() + Compass(r.windDirection),
                     kCardWidth / 2 + 4);
                y += 26;
            }
            y -= 4;
        }
    } else {
        put(Label(r), heading, text, kPad, 28);
        y += 28;
    }
    divider();

    std::wstring footer = settings.place.empty() ? L"" : settings.place + L"  ·  ";
    if (r.valid) {
        footer += L"Updated " + LocalClock(r.fetched);
    } else {
        footer += LocationConfigured() ? L"Waiting for Open-Meteo"
                                       : L"Choose your town in mod settings";
    }
    put(footer, small, muted, kPad, 20);
    y += 22;
    if (r.valid && r.failed) {
        put(L"Last update failed. Retrying in a minute.", small, warn, kPad,
            20);
        y += 22;
    }
    if (r.valid && std::time(nullptr) - r.fetched >
                       std::max(30, 2 * settings.interval) * 60) {
        put(L"Stale reading. Connection unavailable.", small, warn, kPad, 20);
        y += 22;
    }
    put(L"Open-Meteo  ·  Modeled weather", small, muted, kPad, 20);
    y += 22;
    put(LocationConfigured() ? L"Click weather to refresh"
                             : L"Set latitude and longitude in settings",
        small, muted, kPad, 20);
    y += 22;
    return y + kPad - 6;
}

void RenderCard() {
    HWND weather = weatherWindow;
    if (!cardWindow || !weather) {
        return;
    }
    Reading r = CurrentReading();
    float scale = GetDpiForWindow(weather) / 96.f, height = Card(nullptr, r);
    int w = (int)std::ceil((kCardWidth + 2 * kShadow) * scale);
    int h = (int)std::ceil((height + 2 * kShadow) * scale);
    RECT anchor, bar;
    GetWindowRect(weather, &anchor);
    HWND taskbar = GetParent(weather);
    if (!taskbar || !GetWindowRect(taskbar, &bar)) {
        bar = anchor;
    }
    MONITORINFO monitor{};
    monitor.cbSize = sizeof(monitor);
    GetMonitorInfoW(MonitorFromWindow(weather, MONITOR_DEFAULTTONEAREST),
                    &monitor);
    int margin = (int)std::lround(kShadow * scale);
    int gap = (int)std::lround(8 * scale);
    // Open above a bottom taskbar and below a top one, aligned with the
    // weather's left edge.
    bool bottom = bar.top > (monitor.rcMonitor.top + monitor.rcMonitor.bottom) / 2;
    POINT dest{std::clamp(anchor.left - margin, monitor.rcMonitor.left - margin,
                          monitor.rcMonitor.right - w + margin),
               bottom ? bar.top - gap - h + margin : bar.bottom + gap - margin};
    Canvas canvas;
    if (!canvas.Create(w, h)) {
        return;
    }
    {
        Bitmap bitmap(w, h, w * 4, PixelFormat32bppPARGB, (BYTE*)canvas.bits);
        Graphics g(&bitmap);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        g.ScaleTransform(scale, scale);
        g.TranslateTransform(kShadow, kShadow);
        for (int i = 10; i >= 1; i--) {
            GraphicsPath shade;
            RoundedRect(shade, -(float)i, 2.f - i, kCardWidth + 2 * i,
                        height + 2 * i, 8.f + i);
            SolidBrush brush(Color(6, 0, 0, 0));
            g.FillPath(&brush, &shade);
        }
        GraphicsPath fill;
        RoundedRect(fill, 0, 0, kCardWidth, height, 8);
        SolidBrush base(WithAlpha(kBase, 246));
        g.FillPath(&base, &fill);
        GraphicsPath edge;
        RoundedRect(edge, 0.5f, 0.5f, kCardWidth - 1, height - 1, 7.5f);
        Pen border(Color(kSurface1), 1.f);
        g.DrawPath(&border, &edge);
        Card(&g, r);
    }
    POINT src{};
    SIZE size{w, h};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, cardAlpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(cardWindow, canvas.screen, &dest, &size, canvas.dc,
                        &src, 0, &blend, ULW_ALPHA);
}

void ShowCard() {
    if (!cardWindow) {
        return;
    }
    bool visible = IsWindowVisible(cardWindow);
    if (!visible) {
        cardAlpha = 0;
    }
    RenderCard();
    if (!visible) {
        ShowWindow(cardWindow, SW_SHOWNOACTIVATE);
        SetWindowPos(cardWindow, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        SetTimer(weatherWindow, kFadeTimer, 15, nullptr);
    }
}

void HideCard() {
    if (cardWindow) {
        KillTimer(weatherWindow, kFadeTimer);
        ShowWindow(cardWindow, SW_HIDE);
    }
}

int ContentWidth() {
    Reading r = CurrentReading();
    RowFont font = ResolveRowFont();
    float textWidth = TextWidth(
        font, r.valid ? Whole(r.temperature) + TemperatureUnit() : Label(r));
    if (r.valid) {
        textWidth = std::max(textWidth, TextWidth(font, Condition(r.code)));
    }
    return std::clamp((int)std::ceil(textWidth) + (int)TextX(r.code) +
                          (int)kTextRightPad,
                      80, settings.width);
}

// Positions the label. Returns early when nothing it depends on changed, so
// the periodic check costs almost nothing.
void Layout(HWND hwnd, bool force = false) {
    HWND parent = GetParent(hwnd);
    if (!parent) {
        return;
    }
    RECT r;
    GetClientRect(parent, &r);
    UINT dpi = GetDpiForWindow(parent);
    if (contentWidth < 0) {
        contentWidth = ContentWidth();
    }
    if (!force && EqualRect(&r, &lastClient) && dpi == lastDpi &&
        contentWidth == lastContentWidth) {
        return;
    }
    lastClient = r;
    lastDpi = dpi;
    lastContentWidth = contentWidth;
    int x = MulDiv(settings.left, dpi, 96);
    int w = MulDiv(contentWidth, dpi, 96);
    int padding = MulDiv(4, dpi, 96);
    SetWindowPos(hwnd, HWND_TOP, x, padding, w,
                 std::max(1L, r.bottom - 2 * padding),
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    Paint();
}

void Relayout() {
    contentWidth = -1;
    if (HWND window = weatherWindow) {
        Layout(window, true);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEMOVE:
            // Re-arm hover on every move until the card shows: a hover
            // notice is sent once per request and can be lost, for example
            // when the taskbar redraws after a theme change.
            if (!hover || !IsWindowVisible(cardWindow)) {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE | TME_HOVER,
                                      hwnd, 350};
                TrackMouseEvent(&track);
            }
            if (!hover) {
                hover = true;
                Paint();
            }
            break;
        case WM_MOUSEHOVER:
            ShowCard();
            return 0;
        case WM_MOUSELEAVE:
            hover = false;
            HideCard();
            Paint();
            return 0;
        case WM_LBUTTONUP:
            if (LocationConfigured()) {
                SetEvent(refreshEvent);
            }
            return 0;
        case kDataMessage:
            Relayout();
            if (IsWindowVisible(cardWindow)) {
                RenderCard();
            }
            return 0;
        case WM_TIMER:
            if (wp == kFadeTimer) {
                cardAlpha = (BYTE)std::min(255, cardAlpha + 51);
                BLENDFUNCTION blend{AC_SRC_OVER, 0, cardAlpha, AC_SRC_ALPHA};
                UpdateLayeredWindow(cardWindow, nullptr, nullptr, nullptr,
                                    nullptr, nullptr, 0, &blend, ULW_ALPHA);
                if (cardAlpha == 255) {
                    KillTimer(hwnd, kFadeTimer);
                }
            }
            return 0;
        case WM_DESTROY:
            HideCard();
            weatherWindow = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Only a taskbar owned by this process. With "Launch folder windows in a
// separate process", other explorer.exe processes load the mod too, and they
// must not attach a label to the shell's taskbar from outside.
HWND FindOwnTaskbar() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (!taskbar || !GetWindowThreadProcessId(taskbar, &pid) ||
        pid != GetCurrentProcessId()) {
        return nullptr;
    }
    return taskbar;
}

void EnsureWeatherWindow() {
    if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
        return;
    }
    HWND window = weatherWindow;
    if (window && IsWindow(window)) {
        Layout(window);
        return;
    }
    HWND taskbar = FindOwnTaskbar();
    RECT bounds{};
    // Vertical taskbars aren't supported, so skip them.
    if (!taskbar || !GetClientRect(taskbar, &bounds) || bounds.bottom < 24 ||
        bounds.bottom > bounds.right) {
        return;
    }
    window = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE, kClass,
                             L"Taskbar weather", WS_CHILD | WS_VISIBLE, 16, 4,
                             220, 38, taskbar, nullptr, instance, nullptr);
    if (!window) {
        return;
    }
    weatherWindow = window;
    Wh_Log(L"Weather label created");
    lastContentWidth = -1;
    Relayout();
    // The first request waits for the label, so processes without a taskbar
    // never call Open-Meteo.
    if (LocationConfigured()) {
        SetEvent(refreshEvent);
    }
}

LRESULT CALLBACK ControllerProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == taskbarCreatedMessage && taskbarCreatedMessage) {
        EnsureWeatherWindow();
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            EnsureWeatherWindow();
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            Relayout();
            return 0;
        case WM_SETTINGCHANGE:
            if (lp && wcscmp(reinterpret_cast<PCWSTR>(lp),
                             L"ImmersiveColorSet") == 0) {
                UpdateTheme();
            }
            Relayout();
            return 0;
        case WM_POWERBROADCAST:
            if ((wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMESUSPEND) &&
                weatherWindow && LocationConfigured()) {
                SetEvent(refreshEvent);
            }
            return TRUE;
        case WM_TIMECHANGE:
            if (weatherWindow && LocationConfigured()) {
                SetEvent(refreshEvent);
            }
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI UiWorker(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    UpdateTheme();

    WNDCLASSW label{};
    label.lpfnWndProc = WindowProc;
    label.hInstance = instance;
    label.lpszClassName = kClass;
    label.hCursor = LoadCursor(nullptr, IDC_ARROW);
    WNDCLASSW card{};
    card.lpfnWndProc = DefWindowProcW;
    card.hInstance = instance;
    card.lpszClassName = kCardClass;
    WNDCLASSW controller{};
    controller.lpfnWndProc = ControllerProc;
    controller.hInstance = instance;
    controller.lpszClassName = kControllerClass;
    bool labelClass = RegisterClassW(&label) != 0;
    bool cardClass = labelClass && RegisterClassW(&card) != 0;
    bool controllerClass = cardClass && RegisterClassW(&controller) != 0;

    if (controllerClass) {
        // Hidden top-level window: receives taskbar, display, theme, resume
        // and clock broadcasts.
        controllerWindow = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kControllerClass,
            L"Taskbar weather controller", WS_POPUP, 0, 0, 0, 0, nullptr,
            nullptr, instance, nullptr);
    }
    if (controllerWindow) {
        // Click-through and never activated, so the card cannot take focus.
        cardWindow = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                WS_EX_TOPMOST | WS_EX_NOACTIVATE,
            kCardClass, L"Taskbar weather card", WS_POPUP, 0, 0, 1, 1, nullptr,
            nullptr, instance, nullptr);
        // A slow safety check for taskbar changes that send no broadcast.
        SetTimer(controllerWindow, kCheckTimer, 2000, nullptr);
        EnsureWeatherWindow();
        if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) {
            PostMessageW(controllerWindow, WM_CLOSE, 0, 0);
        }
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"Weather window setup failed");
    }

    if (HWND window = weatherWindow) {
        DestroyWindow(window);
    }
    if (cardWindow) {
        DestroyWindow(cardWindow);
    }
    cardWindow = nullptr;
    weatherWindow = nullptr;
    controllerWindow = nullptr;
    if (labelClass) {
        UnregisterClassW(kClass, instance);
    }
    if (cardClass) {
        UnregisterClassW(kCardClass, instance);
    }
    if (controllerClass) {
        UnregisterClassW(kControllerClass, instance);
    }
    d2dFactory.Reset();
    writeFactory.Reset();
    return 0;
}

// Waits for the first refresh signal, which comes once the taskbar label
// exists, then fetches at the update interval.
DWORD WINAPI FetchWorker(void*) {
    HANDLE events[]{stopEvent, refreshEvent};
    DWORD wait = INFINITE;
    while (WaitForMultipleObjects(2, events, FALSE, wait) != WAIT_OBJECT_0) {
        Reading next;
        bool ok = Fetch(next);
        {
            std::lock_guard lock(dataMutex);
            if (ok) {
                reading = next;
            } else {
                reading.failed = true;
            }
        }
        if (ok) {
            SaveCachedReading(next);
        }
        if (HWND window = weatherWindow) {
            PostMessageW(window, kDataMessage, 0, 0);
        }
        Wh_Log(L"%s", ok ? L"Weather update succeeded"
                         : L"Weather request failed, retrying in a minute");
        wait = ok ? settings.interval * 60000 : 60000;
    }
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    if (LocationConfigured()) {
        LoadCachedReading();
    } else {
        Wh_Log(L"Weather location is not set");
    }
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&Wh_ModInit),
                       reinterpret_cast<HMODULE*>(&instance));
    GdiplusStartupInput input;
    if (GdiplusStartup(&graphicsToken, &input, nullptr) != Ok) {
        return FALSE;
    }
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    refreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (stopEvent && refreshEvent) {
        uiThread = CreateThread(nullptr, 0, UiWorker, nullptr, 0, nullptr);
    }
    // No fetch loop without a location: changing settings reloads the mod.
    bool fetchOk = !LocationConfigured() ||
                   (uiThread && (fetchThread = CreateThread(
                                     nullptr, 0, FetchWorker, nullptr, 0,
                                     nullptr)) != nullptr);
    if (uiThread && fetchOk) {
        return TRUE;
    }
    if (stopEvent) {
        SetEvent(stopEvent);
    }
    if (HWND controller = controllerWindow) {
        PostMessageW(controller, WM_CLOSE, 0, 0);
    }
    if (uiThread) {
        WaitForSingleObject(uiThread, INFINITE);
        CloseHandle(uiThread);
        uiThread = nullptr;
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
    }
    if (refreshEvent) {
        CloseHandle(refreshEvent);
    }
    GdiplusShutdown(graphicsToken);
    return FALSE;
}

void Wh_ModUninit() {
    SetEvent(stopEvent);
    if (HWND controller = controllerWindow) {
        PostMessageW(controller, WM_CLOSE, 0, 0);
    }
    // A request in flight is synchronous and can't be cancelled, so this can
    // wait for it. Each WinHTTP step times out after 5 s and the body read
    // stops after 30 s, so the worst case is about 35 s on a stalled network.
    if (fetchThread) {
        WaitForSingleObject(fetchThread, INFINITE);
        CloseHandle(fetchThread);
    }
    if (uiThread) {
        WaitForSingleObject(uiThread, INFINITE);
        CloseHandle(uiThread);
    }
    CloseHandle(stopEvent);
    CloseHandle(refreshEvent);
    GdiplusShutdown(graphicsToken);
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
