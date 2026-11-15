// ==WindhawkMod==
// @id              micrographics-taskbar-clock
// @name            Micrographics Taskbar Clock
// @description     Replaces the Windows 11 taskbar clock with a Nothing OS 5 "Micrographics" style clock: pill badges, a block cursor and a weather line
// @version         1.0
// @author          SAM2WOW
// @github          https://github.com/SAM2WOW
// @homepage        https://github.com/SAM2WOW/micrographics-taskbar-clock
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Micrographics Taskbar Clock

Replaces the Windows 11 taskbar clock with a clock face inspired by the
"Micrographics" clock of Nothing OS 5.

![Screenshot](https://raw.githubusercontent.com/SAM2WOW/micrographics-taskbar-clock/main/images/taskbar.png)

* Weekday, day of month in a filled pill, month in an outlined pill, the time
  and a block cursor.
* Temperature, a weather icon and the current conditions in a speech bubble.
* Follows the taskbar's light/dark text color automatically, on every monitor.
* Clicking the clock still opens the notification center and calendar.

## Weather

The weather line is off by default, and the mod makes no network requests until
it's turned on. To show it, enable **Show weather** in the settings and enter a
city name or `latitude,longitude` as the location. Weather then comes from
[Open-Meteo](https://open-meteo.com/) (free, no API key), and a city name is
looked up with Open-Meteo's geocoding service.

Alternatively, enable **Detect location from IP address** to leave the location
empty. Your public IP address is then sent to [ipwho.is](https://ipwho.is/), or
to [GeoJS](https://www.geojs.io/) if that fails, to approximate your location.

## Fonts

The default font is Bahnschrift, which ships with Windows. Any installed font
can be used, e.g. a monospaced one such as Cascadia Mono, or a dot matrix font.

## Other clock mods

This mod hides the original clock text and draws its own clock face, so changes
from other clock mods, such as Taskbar Clock Customization, aren't visible
while it's enabled.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- layout: twoRows
  $name: Layout
  $options:
  - twoRows: Two rows (date and time, weather below)
  - oneRow: One row
- alignment: left
  $name: Row alignment
  $options:
  - left: Left
  - center: Center
  - right: Right
- fontFamily: Bahnschrift
  $name: Font family
  $description: >-
    Any installed font, for example Bahnschrift, Cascadia Mono or Segoe UI
    Variable.
- fontWeight: medium
  $name: Font weight
  $options:
  - normal: Normal
  - medium: Medium
  - semibold: Semi bold
  - bold: Bold
- fontSize: 13
  $name: Font size
  $description: >-
    The clock shrinks automatically if the two rows don't fit the taskbar
    height.
- use24Hour: true
  $name: 24-hour time
- showSeconds: false
  $name: Show seconds
- cursor: blink
  $name: Block cursor after the time
  $options:
  - blink: Blinking
  - solid: Always visible
  - hidden: Hidden
- textColor: ""
  $name: Text color
  $description: >-
    #RRGGBB or #AARRGGBB. Leave empty to follow the taskbar's text color.
- accentColor: ""
  $name: Pill color
  $description: >-
    Fill color of the day pill and the weather bubble, e.g. #D71921 for Nothing
    red. Leave empty to use the text color.
- weather:
  - enabled: false
    $name: Show weather
    $description: >-
      Shows the current weather from Open-Meteo (open-meteo.com). Requires a
      location below.
  - location: ""
    $name: Location
    $description: >-
      A city name (e.g. Shanghai) or latitude,longitude (e.g. 31.23,121.47).
  - detectLocation: false
    $name: Detect location from IP address
    $description: >-
      If the location is empty, approximate it from your public IP address. The
      IP address is sent to ipwho.is, or to get.geojs.io if that fails.
  - fahrenheit: false
    $name: Use Fahrenheit
  - updateMinutes: 15
    $name: Update interval (minutes)
  $name: Weather
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cwchar>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <windows.h>
#include <winhttp.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class ClockLayout { TwoRows, OneRow };
enum class CursorMode { Blink, Solid, Hidden };

struct Settings {
    ClockLayout layout = ClockLayout::TwoRows;
    HorizontalAlignment alignment = HorizontalAlignment::Left;
    std::wstring fontFamily = L"Bahnschrift";
    uint16_t fontWeight = 500;
    double fontSize = 13;
    bool use24Hour = true;
    bool showSeconds = false;
    CursorMode cursor = CursorMode::Blink;
    std::optional<winrt::Windows::UI::Color> textColor;
    std::optional<winrt::Windows::UI::Color> accentColor;
    bool weatherEnabled = false;
    std::wstring weatherLocation;
    bool weatherDetectLocation = false;
    bool weatherFahrenheit = false;
    int weatherUpdateMinutes = 15;

    // Whether the weather can be fetched at all: it's opt-in, and the location
    // is only detected from the IP address if that's explicitly enabled.
    bool ShouldFetchWeather() const {
        return weatherEnabled &&
               (!weatherLocation.empty() || weatherDetectLocation);
    }
};

// Accessed from the taskbar UI thread once the mod is initialized.
Settings g_settings;
unsigned g_settingsGeneration = 0;

std::atomic<bool> g_unloading;

std::wstring Trim(std::wstring_view s) {
    size_t begin = s.find_first_not_of(L" \t\r\n");
    if (begin == std::wstring_view::npos) {
        return {};
    }
    size_t end = s.find_last_not_of(L" \t\r\n");
    return std::wstring(s.substr(begin, end - begin + 1));
}

std::optional<winrt::Windows::UI::Color> ParseColor(std::wstring_view text) {
    std::wstring s = Trim(text);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() != 6 && s.size() != 8) {
        return std::nullopt;
    }
    uint32_t value = 0;
    for (wchar_t c : s) {
        uint32_t digit;
        if (c >= L'0' && c <= L'9') {
            digit = c - L'0';
        } else if (c >= L'a' && c <= L'f') {
            digit = c - L'a' + 10;
        } else if (c >= L'A' && c <= L'F') {
            digit = c - L'A' + 10;
        } else {
            return std::nullopt;
        }
        value = (value << 4) | digit;
    }
    if (s.size() == 6) {
        value |= 0xFF000000;
    }
    return winrt::Windows::UI::Color{
        static_cast<uint8_t>(value >> 24), static_cast<uint8_t>(value >> 16),
        static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value)};
}

Settings LoadSettings() {
    Settings s;

    auto str = [](PCWSTR name) {
        return std::wstring(WindhawkUtils::StringSetting::make(name).get());
    };

    s.layout = str(L"layout") == L"oneRow" ? ClockLayout::OneRow
                                          : ClockLayout::TwoRows;

    std::wstring alignment = str(L"alignment");
    s.alignment = alignment == L"center"  ? HorizontalAlignment::Center
                  : alignment == L"right" ? HorizontalAlignment::Right
                                          : HorizontalAlignment::Left;

    s.fontFamily = Trim(str(L"fontFamily"));
    if (s.fontFamily.empty()) {
        s.fontFamily = L"Bahnschrift";
    }

    std::wstring weight = str(L"fontWeight");
    s.fontWeight = weight == L"normal"     ? 400
                   : weight == L"semibold" ? 600
                   : weight == L"bold"     ? 700
                                           : 500;

    int fontSize = Wh_GetIntSetting(L"fontSize");
    s.fontSize = fontSize < 6 ? 6 : fontSize > 40 ? 40 : fontSize;

    s.use24Hour = Wh_GetIntSetting(L"use24Hour");
    s.showSeconds = Wh_GetIntSetting(L"showSeconds");

    std::wstring cursor = str(L"cursor");
    s.cursor = cursor == L"solid"    ? CursorMode::Solid
               : cursor == L"hidden" ? CursorMode::Hidden
                                     : CursorMode::Blink;

    s.textColor = ParseColor(str(L"textColor"));
    s.accentColor = ParseColor(str(L"accentColor"));

    s.weatherEnabled = Wh_GetIntSetting(L"weather.enabled");
    s.weatherLocation = Trim(str(L"weather.location"));
    s.weatherDetectLocation = Wh_GetIntSetting(L"weather.detectLocation");
    s.weatherFahrenheit = Wh_GetIntSetting(L"weather.fahrenheit");
    int updateMinutes = Wh_GetIntSetting(L"weather.updateMinutes");
    s.weatherUpdateMinutes =
        updateMinutes < 5 ? 5 : updateMinutes > 720 ? 720 : updateMinutes;

    return s;
}

////////////////////////////////////////////////////////////////////////////////
// Weather

struct WeatherData {
    bool valid = false;
    int temperature = 0;
    int code = 0;  // WMO weather interpretation code.
    bool isDay = true;
    bool fahrenheit = false;
    ULONGLONG fetchedTick = 0;
};

struct Coordinates {
    double latitude;
    double longitude;
};

std::mutex g_weatherMutex;
WeatherData g_weather;

// Guards starting and stopping the weather thread. The thread object isn't
// destroyed at process exit, where Wh_ModUninit isn't called and destroying a
// joinable std::thread would terminate the process.
std::mutex g_weatherThreadMutex;
[[clang::no_destroy]] std::optional<std::thread> g_weatherThread;
HANDLE g_weatherStopEvent;
std::atomic<bool> g_weatherThreadStarted;

// State shared by the requests of the weather thread.
struct WeatherContext {
    HINTERNET session;
    HANDLE stopEvent;

    bool IsStopping() const {
        return WaitForSingleObject(stopEvent, 0) != WAIT_TIMEOUT;
    }
};

std::optional<std::string> HttpsGet(const WeatherContext& context,
                                    PCWSTR host,
                                    const std::wstring& pathAndQuery) {
    std::optional<std::string> result;

    // Each request can take a while on a bad network, so check for a pending
    // stop before starting a new one.
    if (context.IsStopping()) {
        return result;
    }

    HINTERNET connect = WinHttpConnect(context.session, host,
                                       INTERNET_DEFAULT_HTTPS_PORT, 0);
    HINTERNET request =
        connect ? WinHttpOpenRequest(connect, L"GET", pathAndQuery.c_str(),
                                     nullptr, WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     WINHTTP_FLAG_SECURE)
                : nullptr;

    if (request &&
        WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(request, nullptr)) {
        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        WinHttpQueryHeaders(
            request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
            WINHTTP_NO_HEADER_INDEX);
        if (status == 200) {
            constexpr size_t kMaxBodySize = 256 * 1024;
            std::string body;
            bool ok = true;
            for (;;) {
                char buffer[4096];
                DWORD read = 0;
                if (!WinHttpReadData(request, buffer, sizeof(buffer), &read)) {
                    ok = false;
                    break;
                }
                if (read == 0) {
                    break;
                }
                body.append(buffer, read);
                if (body.size() > kMaxBodySize) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                result = std::move(body);
            }
        } else {
            Wh_Log(L"%s returned HTTP status %u", host, status);
        }
    } else {
        Wh_Log(L"Request to %s failed: %u", host, GetLastError());
    }

    if (request) {
        WinHttpCloseHandle(request);
    }
    if (connect) {
        WinHttpCloseHandle(connect);
    }

    return result;
}

// Parses a JSON number without depending on the C locale.
std::optional<double> ParseNumber(std::string_view s, size_t* consumed) {
    size_t i = 0;
    bool negative = false;
    if (i < s.size() && (s[i] == '-' || s[i] == '+')) {
        negative = s[i] == '-';
        i++;
    }
    double value = 0;
    bool anyDigits = false;
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
        value = value * 10 + (s[i] - '0');
        anyDigits = true;
        i++;
    }
    if (i < s.size() && s[i] == '.') {
        i++;
        double scale = 0.1;
        while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
            value += (s[i] - '0') * scale;
            scale /= 10;
            anyDigits = true;
            i++;
        }
    }
    if (!anyDigits) {
        return std::nullopt;
    }
    if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
        size_t j = i + 1;
        bool expNegative = false;
        if (j < s.size() && (s[j] == '-' || s[j] == '+')) {
            expNegative = s[j] == '-';
            j++;
        }
        int exponent = 0;
        bool anyExpDigits = false;
        while (j < s.size() && s[j] >= '0' && s[j] <= '9' && exponent < 400) {
            exponent = exponent * 10 + (s[j] - '0');
            anyExpDigits = true;
            j++;
        }
        if (anyExpDigits) {
            value *= std::pow(10.0, expNegative ? -exponent : exponent);
            i = j;
        }
    }
    if (consumed) {
        *consumed = i;
    }
    return negative ? -value : value;
}

// Finds `"key": <number>` at or after `from`. The number may be quoted. Good
// enough for the small, flat responses used here.
std::optional<double> JsonNumber(std::string_view json,
                                 std::string_view key,
                                 size_t from = 0) {
    std::string pattern = "\"" + std::string(key) + "\"";
    size_t pos = json.find(pattern, from);
    if (pos == std::string_view::npos) {
        return std::nullopt;
    }
    pos += pattern.size();
    auto skipSpaces = [&]() {
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
                                     json[pos] == '\r' || json[pos] == '\n')) {
            pos++;
        }
    };
    skipSpaces();
    if (pos >= json.size() || json[pos] != ':') {
        return std::nullopt;
    }
    pos++;
    skipSpaces();
    if (pos < json.size() && json[pos] == '"') {
        pos++;
    }
    return ParseNumber(json.substr(pos), nullptr);
}

bool IsValidCoordinates(const Coordinates& c) {
    return c.latitude >= -90 && c.latitude <= 90 && c.longitude >= -180 &&
           c.longitude <= 180;
}

std::optional<Coordinates> CoordinatesFromJson(std::string_view json,
                                               size_t from = 0) {
    auto latitude = JsonNumber(json, "latitude", from);
    auto longitude = JsonNumber(json, "longitude", from);
    if (!latitude || !longitude) {
        return std::nullopt;
    }
    Coordinates c{*latitude, *longitude};
    if (!IsValidCoordinates(c)) {
        return std::nullopt;
    }
    return c;
}

// Accepts "latitude,longitude", e.g. "31.23, 121.47".
std::optional<Coordinates> ParseCoordinates(const std::wstring& text) {
    std::string s;
    for (wchar_t c : text) {
        if (c > 0x7F) {
            return std::nullopt;
        }
        if (c != L' ') {
            s += static_cast<char>(c);
        }
    }
    size_t consumed = 0;
    auto latitude = ParseNumber(s, &consumed);
    if (!latitude || consumed >= s.size() || s[consumed] != ',') {
        return std::nullopt;
    }
    std::string_view rest = std::string_view(s).substr(consumed + 1);
    auto longitude = ParseNumber(rest, &consumed);
    if (!longitude || consumed != rest.size()) {
        return std::nullopt;
    }
    Coordinates c{*latitude, *longitude};
    if (!IsValidCoordinates(c)) {
        return std::nullopt;
    }
    return c;
}

std::wstring UrlEncode(const std::wstring& text) {
    int size = WideCharToMultiByte(CP_UTF8, 0, text.data(),
                                   static_cast<int>(text.size()), nullptr, 0,
                                   nullptr, nullptr);
    std::string utf8(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                        utf8.data(), size, nullptr, nullptr);

    static const wchar_t kHex[] = L"0123456789ABCDEF";
    std::wstring result;
    for (unsigned char c : utf8) {
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') || c == '-' || c == '_' || c == '.' ||
            c == '~') {
            result += static_cast<wchar_t>(c);
        } else {
            result += L'%';
            result += kHex[c >> 4];
            result += kHex[c & 0xF];
        }
    }
    return result;
}

// Formats a number with up to 4 decimals without depending on the C locale.
std::wstring FormatNumber(double value, int decimals) {
    long long scale = 1;
    for (int i = 0; i < decimals; i++) {
        scale *= 10;
    }
    long long scaled = std::llround(value * static_cast<double>(scale));
    std::wstring result;
    if (scaled < 0) {
        result += L'-';
        scaled = -scaled;
    }
    result += std::to_wstring(scaled / scale);
    long long fraction = scaled % scale;
    if (fraction != 0) {
        std::wstring digits = std::to_wstring(fraction);
        digits.insert(0, decimals - digits.size(), L'0');
        while (!digits.empty() && digits.back() == L'0') {
            digits.pop_back();
        }
        result += L'.';
        result += digits;
    }
    return result;
}

std::optional<Coordinates> ResolveLocation(const WeatherContext& context,
                                           const std::wstring& location,
                                           bool detectLocation) {
    if (location.empty()) {
        if (!detectLocation) {
            return std::nullopt;
        }
        // Approximate the location from the public IP address, which the user
        // opted into.
        if (auto body = HttpsGet(context, L"ipwho.is",
                                 L"/?fields=success,latitude,longitude")) {
            if (auto c = CoordinatesFromJson(*body)) {
                return c;
            }
        }
        if (auto body =
                HttpsGet(context, L"get.geojs.io", L"/v1/ip/geo.json")) {
            if (auto c = CoordinatesFromJson(*body)) {
                return c;
            }
        }
        Wh_Log(L"Failed to detect the location");
        return std::nullopt;
    }

    if (auto c = ParseCoordinates(location)) {
        return c;
    }

    auto body = HttpsGet(context, L"geocoding-api.open-meteo.com",
                         L"/v1/search?count=1&format=json&name=" +
                             UrlEncode(location));
    if (body) {
        size_t results = body->find("\"results\"");
        if (results != std::string::npos) {
            if (auto c = CoordinatesFromJson(*body, results)) {
                return c;
            }
        }
    }
    Wh_Log(L"Failed to find the location: %s", location.c_str());
    return std::nullopt;
}

std::optional<WeatherData> FetchWeather(const WeatherContext& context,
                                        const Coordinates& coordinates,
                                        bool fahrenheit) {
    std::wstring path =
        L"/v1/forecast?current=temperature_2m,weather_code,is_day&latitude=" +
        FormatNumber(coordinates.latitude, 4) + L"&longitude=" +
        FormatNumber(coordinates.longitude, 4);
    if (fahrenheit) {
        path += L"&temperature_unit=fahrenheit";
    }

    auto body = HttpsGet(context, L"api.open-meteo.com", path);
    if (!body) {
        return std::nullopt;
    }

    // Skip "current_units", which has the same keys.
    size_t current = body->find("\"current\"");
    if (current == std::string::npos) {
        return std::nullopt;
    }
    auto temperature = JsonNumber(*body, "temperature_2m", current);
    auto code = JsonNumber(*body, "weather_code", current);
    auto isDay = JsonNumber(*body, "is_day", current);
    if (!temperature || !code) {
        return std::nullopt;
    }

    WeatherData data;
    data.valid = true;
    data.temperature = static_cast<int>(std::lround(*temperature));
    data.code = static_cast<int>(std::lround(*code));
    data.isDay = !isDay || *isDay != 0;
    data.fahrenheit = fahrenheit;
    data.fetchedTick = GetTickCount64();
    return data;
}

void WeatherThreadProc(HANDLE stopEvent,
                       std::wstring location,
                       bool detectLocation,
                       bool fahrenheit,
                       int updateMinutes) {
    constexpr ULONGLONG kMinRetryMs = 30 * 1000;
    constexpr ULONGLONG kMaxRetryMs = 10 * 60 * 1000;

    HINTERNET session =
        WinHttpOpen(L"MicrographicsClock-WindhawkMod/1.0",
                    WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
                    WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        Wh_Log(L"WinHttpOpen failed: %u", GetLastError());
        return;
    }
    WinHttpSetTimeouts(session, 5000, 5000, 5000, 8000);

    WeatherContext context{session, stopEvent};

    std::optional<Coordinates> coordinates;
    ULONGLONG retryMs = kMinRetryMs;

    for (;;) {
        bool ok = false;
        if (!coordinates) {
            coordinates = ResolveLocation(context, location, detectLocation);
        }
        if (coordinates) {
            if (auto data = FetchWeather(context, *coordinates, fahrenheit)) {
                std::lock_guard<std::mutex> lock(g_weatherMutex);
                g_weather = *data;
                ok = true;
            }
        }

        ULONGLONG delay;
        if (ok) {
            delay = static_cast<ULONGLONG>(updateMinutes) * 60 * 1000;
            retryMs = kMinRetryMs;
        } else {
            delay = retryMs;
            retryMs = retryMs * 2 > kMaxRetryMs ? kMaxRetryMs : retryMs * 2;
        }

        // Wait in slices and compare against the tick count, which keeps
        // advancing while the computer sleeps, so that the weather is
        // refreshed soon after waking up.
        bool stopping = false;
        ULONGLONG deadline = GetTickCount64() + delay;
        for (;;) {
            ULONGLONG now = GetTickCount64();
            if (now >= deadline) {
                break;
            }
            ULONGLONG remaining = deadline - now;
            DWORD slice =
                remaining > 30000 ? 30000 : static_cast<DWORD>(remaining);
            if (WaitForSingleObject(stopEvent, slice) != WAIT_TIMEOUT) {
                stopping = true;
                break;
            }
        }
        if (stopping) {
            break;
        }
    }

    WinHttpCloseHandle(session);
}

void StopWeatherThread() {
    if (g_weatherThread) {
        SetEvent(g_weatherStopEvent);
        g_weatherThread->join();
        g_weatherThread.reset();
    }
    if (g_weatherStopEvent) {
        CloseHandle(g_weatherStopEvent);
        g_weatherStopEvent = nullptr;
    }
}

void StartWeatherThread(const Settings& settings) {
    StopWeatherThread();
    if (!settings.ShouldFetchWeather()) {
        return;
    }
    g_weatherStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_weatherStopEvent) {
        return;
    }
    g_weatherThread.emplace(WeatherThreadProc, g_weatherStopEvent,
                            settings.weatherLocation,
                            settings.weatherDetectLocation,
                            settings.weatherFahrenheit,
                            settings.weatherUpdateMinutes);
}

WeatherData GetWeatherSnapshot() {
    constexpr ULONGLONG kMaxAgeMs = 3 * 60 * 60 * 1000;

    std::lock_guard<std::mutex> lock(g_weatherMutex);
    WeatherData data = g_weather;
    if (data.valid && GetTickCount64() - data.fetchedTick > kMaxAgeMs) {
        data.valid = false;
    }
    return data;
}

////////////////////////////////////////////////////////////////////////////////
// Weather icons

enum class WeatherIcon {
    Sun,
    Moon,
    Cloud,
    PartlyCloudyDay,
    PartlyCloudyNight,
    Rain,
    Snow,
    Storm,
    Fog,
};

std::wstring N(double value) {
    return FormatNumber(value, 2);
}

constexpr double kPi = 3.14159265358979323846;

// The icons are abstract shapes drawn on a grid which is 24 units high, as
// path markup.

// A star burst with `points` spikes.
std::wstring BurstPath(double cx,
                       double cy,
                       double outerRadius,
                       double innerRadius,
                       int points) {
    std::wstring d;
    for (int i = 0; i < points * 2; i++) {
        double angle = kPi * i / points - kPi / 2;
        double radius = i % 2 ? innerRadius : outerRadius;
        d += i ? L" L" : L"M";
        d += N(cx + radius * std::cos(angle)) + L"," +
             N(cy + radius * std::sin(angle));
    }
    return d + L" Z";
}

std::wstring CirclePath(double cx, double cy, double r) {
    return L"M" + N(cx - r) + L"," + N(cy) + L" A" + N(r) + L"," + N(r) +
           L" 0 1 1 " + N(cx + r) + L"," + N(cy) + L" A" + N(r) + L"," + N(r) +
           L" 0 1 1 " + N(cx - r) + L"," + N(cy) + L" Z";
}

// A scalloped disc, which stands for a cloud.
std::wstring BlobPath(double cx, double cy, double radius) {
    constexpr int kLobes = 8;
    double lobeRadius = radius * 0.32;
    double ringRadius = radius - lobeRadius;

    std::wstring d = CirclePath(cx, cy, ringRadius);
    for (int i = 0; i < kLobes; i++) {
        double angle = 2 * kPi * i / kLobes;
        d += L" " + CirclePath(cx + ringRadius * std::cos(angle),
                               cy + ringRadius * std::sin(angle), lobeRadius);
    }
    return d;
}

// A bar centered at (cx, cy), rotated by the given angle.
std::wstring BarPath(double cx,
                     double cy,
                     double halfLength,
                     double halfWidth,
                     double degrees) {
    double angle = degrees * kPi / 180;
    double c = std::cos(angle);
    double s = std::sin(angle);
    const double corners[][2] = {{-halfLength, -halfWidth},
                                 {halfLength, -halfWidth},
                                 {halfLength, halfWidth},
                                 {-halfLength, halfWidth}};
    std::wstring d;
    for (const auto& corner : corners) {
        d += d.empty() ? L"M" : L" L";
        d += N(cx + corner[0] * c - corner[1] * s) + L"," +
             N(cy + corner[0] * s + corner[1] * c);
    }
    return d + L" Z";
}

std::wstring WeatherIconPath(WeatherIcon icon) {
    static constexpr WCHAR kMoon[] =
        L"M14.49,0.26 A12,12 0 1 0 23.74,14.49 A9.6,9.6 0 0 1 14.49,0.26 Z";

    switch (icon) {
        case WeatherIcon::Sun:
            return BurstPath(12, 12, 12, 8.4, 12);
        case WeatherIcon::Moon:
            return kMoon;
        case WeatherIcon::Cloud:
            return BlobPath(12, 12, 12);
        // The two shapes sit side by side, touching at their tips.
        case WeatherIcon::PartlyCloudyDay:
            return BurstPath(12, 12, 12, 8.4, 12) + L" " + BlobPath(36, 12, 12);
        case WeatherIcon::PartlyCloudyNight:
            return kMoon + (L" " + BlobPath(36, 12, 12));
        case WeatherIcon::Rain:
            return L"M6,0 H10.5 L4.5,24 H0 Z M14.5,0 H19 L13,24 H8.5 Z"
                   L" M23,0 H27.5 L21.5,24 H17 Z";
        case WeatherIcon::Snow:
            return BarPath(12, 12, 12, 2.3, 90) + L" " +
                   BarPath(12, 12, 12, 2.3, 30) + L" " +
                   BarPath(12, 12, 12, 2.3, 150);
        case WeatherIcon::Storm:
            return L"M12,0 L2,14 H9.5 L6.5,24 L20,9.5 H12.5 L17,0 Z";
        case WeatherIcon::Fog:
            return L"M2,1.5 H18 A2.25,2.25 0 0 1 18,6 H2"
                   L" A2.25,2.25 0 0 1 2,1.5 Z"
                   L" M8,9.75 H24 A2.25,2.25 0 0 1 24,14.25 H8"
                   L" A2.25,2.25 0 0 1 8,9.75 Z"
                   L" M2,18 H18 A2.25,2.25 0 0 1 18,22.5 H2"
                   L" A2.25,2.25 0 0 1 2,18 Z";
    }
    return {};
}

struct WeatherDescription {
    WeatherIcon icon;
    PCWSTR label;
};

// https://open-meteo.com/en/docs#weather_variable_documentation
WeatherDescription DescribeWeather(int code, bool isDay) {
    switch (code) {
        case 0:
            return isDay ? WeatherDescription{WeatherIcon::Sun, L"SUNNY"}
                         : WeatherDescription{WeatherIcon::Moon, L"CLEAR"};
        case 1:
            return isDay
                       ? WeatherDescription{WeatherIcon::Sun, L"MOSTLY SUNNY"}
                       : WeatherDescription{WeatherIcon::Moon, L"MOSTLY CLEAR"};
        case 2:
            return isDay ? WeatherDescription{WeatherIcon::PartlyCloudyDay,
                                              L"PARTLY SUNNY"}
                         : WeatherDescription{WeatherIcon::PartlyCloudyNight,
                                              L"PARTLY CLOUDY"};
        case 3:
            return {WeatherIcon::Cloud, L"CLOUDY"};
        case 45:
        case 48:
            return {WeatherIcon::Fog, L"FOGGY"};
        case 51:
        case 53:
        case 55:
        case 56:
        case 57:
            return {WeatherIcon::Rain, L"DRIZZLE"};
        case 61:
        case 63:
        case 66:
            return {WeatherIcon::Rain, L"RAIN"};
        case 65:
        case 67:
            return {WeatherIcon::Rain, L"HEAVY RAIN"};
        case 71:
        case 73:
        case 77:
        case 85:
            return {WeatherIcon::Snow, L"SNOW"};
        case 75:
        case 86:
            return {WeatherIcon::Snow, L"HEAVY SNOW"};
        case 80:
        case 81:
        case 82:
            return {WeatherIcon::Rain, L"SHOWERS"};
        case 95:
        case 96:
        case 99:
            return {WeatherIcon::Storm, L"STORM"};
    }
    return {WeatherIcon::Cloud, L"CLOUDY"};
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar XAML access

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                          void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

// Only read as code bytes, never called.
void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

bool IsReadableMemory(const void* address, size_t size) {
    MEMORY_BASIC_INFORMATION info{};
    if (!address || !VirtualQuery(address, &info, sizeof(info)) ||
        info.State != MEM_COMMIT ||
        (info.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    auto start = reinterpret_cast<uintptr_t>(address);
    auto regionEnd =
        reinterpret_cast<uintptr_t>(info.BaseAddress) + info.RegionSize;
    return size <= regionEnd - start;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd, bool secondary) {
    HWND hTaskSwWnd =
        secondary ? FindWindowEx(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
                  : reinterpret_cast<HWND>(
                        GetProp(hTaskbarWnd, L"TaskbandHWND"));
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtr(hTaskSwWnd, 0));
    void* expectedVftable = secondary
                                ? CSecondaryTaskBand_ITaskListWndSite_vftable
                                : CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = secondary ? CSecondaryTaskBand_GetTaskbarHost_Original
                                    : CTaskBand_GetTaskbarHost_Original;
    if (!taskBand || !expectedVftable || !getTaskbarHost ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0;; i++) {
        if (i == 20 ||
            !IsReadableMemory(taskBandForTaskListWndSite, sizeof(void*))) {
            return nullptr;
        }
        if (*static_cast<void**>(taskBandForTaskListWndSite) ==
            expectedVftable) {
            break;
        }
        taskBandForTaskListWndSite =
            static_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    XamlRoot result = nullptr;

    // The offset of the taskbar element in TaskbarHost, decoded from the start
    // of TaskbarHost::FrameHeight.
    std::optional<size_t> taskbarElementIUnknownOffset;
#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = static_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
        if (IsReadableMemory(b, 8) && b[0] == 0x48 && b[1] == 0x83 &&
            b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 &&
            b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p =
            static_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
        if (IsReadableMemory(p, sizeof(DWORD) * 4) && p[0] == 0xD503237F &&
            (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        }
    }
#else
#error "Unsupported architecture"
#endif

    if (!taskbarElementIUnknownOffset) {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }

    if (taskbarHostSharedPtr[0] && taskbarElementIUnknownOffset) {
        void* slot = static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
                     *taskbarElementIUnknownOffset;
        if (IsReadableMemory(slot, sizeof(IUnknown*))) {
            IUnknown* taskbarElementIUnknown = *static_cast<IUnknown**>(slot);
            if (taskbarElementIUnknown) {
                FrameworkElement taskbarElement = nullptr;
                taskbarElementIUnknown->QueryInterface(
                    winrt::guid_of<FrameworkElement>(),
                    winrt::put_abi(taskbarElement));
                if (taskbarElement) {
                    result = taskbarElement.XamlRoot();
                }
            }
        }
    }

    if (taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

FrameworkElement FindChildByName(DependencyObject const& parent,
                                 std::wstring_view name,
                                 int maxDepth) {
    if (maxDepth <= 0) {
        return nullptr;
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<FrameworkElement>()) {
            if (element.Name() == name) {
                return element;
            }
        }
        if (auto found = FindChildByName(child, name, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

void FindChildrenByClassName(DependencyObject const& parent,
                             std::wstring_view className,
                             int maxDepth,
                             std::vector<FrameworkElement>& results) {
    if (maxDepth <= 0) {
        return;
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<FrameworkElement>()) {
            if (winrt::get_class_name(element) == className) {
                results.push_back(element);
                continue;
            }
        }
        FindChildrenByClassName(child, className, maxDepth - 1, results);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Clock face

constexpr WCHAR kClockRootName[] = L"MicrographicsClockRoot";

// One per taskbar clock, i.e. one per monitor that shows a clock.
struct ClockInstance {
    // The clock's "ContainerGrid", which hosts the original date and time
    // text blocks.
    Controls::Panel container{nullptr};
    // The original children of the container, hidden while the mod is loaded.
    std::vector<std::pair<UIElement, Visibility>> hiddenChildren;
    // The original time text block, used to follow the taskbar's text color.
    Controls::TextBlock colorSource{nullptr};

    Controls::StackPanel root{nullptr};
    Controls::TextBlock timeText{nullptr};
    Shapes::Rectangle cursor{nullptr};

    std::wstring renderedKey;
    std::wstring renderedTime;
};

// The following are only accessed from the taskbar UI thread. They hold XAML
// objects, which must not be released on another thread when the process
// exits, so they're never destroyed automatically. They're released in
// StopClockUi on unload.
[[clang::no_destroy]] std::optional<std::vector<ClockInstance>> g_instances{
    std::in_place};
[[clang::no_destroy]] DispatcherTimer g_timer{nullptr};
winrt::event_token g_timerTickToken;
unsigned g_tickCounter;
int g_startupScansLeft;

// Whether new clocks are reported by the DateTimeIconContent::OnApplyTemplate
// hook.
std::atomic<bool> g_clockHookApplied;

struct DisplayState {
    std::wstring weekday;
    std::wstring day;
    std::wstring month;
    std::wstring time;
    bool hasWeather = false;
    std::wstring temperature;
    std::wstring unit;
    std::wstring condition;
    WeatherIcon icon = WeatherIcon::Sun;
};

DisplayState GetDisplayState(const SYSTEMTIME& time,
                             const WeatherData& weather) {
    static constexpr PCWSTR kWeekdays[] = {L"SUN", L"MON", L"TUE", L"WED",
                                           L"THU", L"FRI", L"SAT"};
    static constexpr PCWSTR kMonths[] = {L"JAN", L"FEB", L"MAR", L"APR",
                                         L"MAY", L"JUN", L"JUL", L"AUG",
                                         L"SEP", L"OCT", L"NOV", L"DEC"};

    DisplayState state;
    state.weekday = kWeekdays[time.wDayOfWeek % 7];
    state.day = std::to_wstring(time.wDay);
    state.month = kMonths[(time.wMonth + 11) % 12];

    auto twoDigits = [](int value) {
        std::wstring s = std::to_wstring(value);
        return s.size() < 2 ? L"0" + s : s;
    };

    if (g_settings.use24Hour) {
        state.time = twoDigits(time.wHour);
    } else {
        int hour = time.wHour % 12;
        state.time = std::to_wstring(hour ? hour : 12);
    }
    state.time += L":" + twoDigits(time.wMinute);
    if (g_settings.showSeconds) {
        state.time += L":" + twoDigits(time.wSecond);
    }
    if (!g_settings.use24Hour) {
        state.time += time.wHour < 12 ? L" AM" : L" PM";
    }

    if (g_settings.ShouldFetchWeather() && weather.valid) {
        auto description = DescribeWeather(weather.code, weather.isDay);
        state.hasWeather = true;
        // U+00B0, the degree sign.
        state.temperature = std::to_wstring(weather.temperature) +
                            static_cast<wchar_t>(0xB0);
        state.unit = weather.fahrenheit ? L"F" : L"C";
        state.condition = description.label;
        state.icon = description.icon;
    }

    return state;
}

struct Palette {
    winrt::Windows::UI::Color text;
    winrt::Windows::UI::Color pill;
    winrt::Windows::UI::Color pillText;
};

Palette GetPalette(const ClockInstance& instance) {
    winrt::Windows::UI::Color text{0xFF, 0xFF, 0xFF, 0xFF};
    if (g_settings.textColor) {
        text = *g_settings.textColor;
    } else {
        // The original text block keeps following the taskbar theme, even
        // while it's hidden.
        Media::SolidColorBrush brush = nullptr;
        if (instance.colorSource) {
            brush = instance.colorSource.Foreground()
                        .try_as<Media::SolidColorBrush>();
        }
        if (brush) {
            text = brush.Color();
            text.A = 0xFF;
        } else if (instance.container.ActualTheme() == ElementTheme::Light) {
            text = {0xFF, 0x00, 0x00, 0x00};
        }
    }

    winrt::Windows::UI::Color pill =
        g_settings.accentColor ? *g_settings.accentColor : text;

    double luminance =
        (0.299 * pill.R + 0.587 * pill.G + 0.114 * pill.B) / 255.0;
    winrt::Windows::UI::Color pillText =
        luminance > 0.55 ? winrt::Windows::UI::Color{0xFF, 0x00, 0x00, 0x00}
                         : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF};

    return {text, pill, pillText};
}

// All sizes derive from the font size, mirroring the proportions of the
// original clock face.
struct Metrics {
    double fontSize;
    double pillHeight;
    double rowGap;
    double pillPadding;
    double stroke;
};

Metrics GetMetrics(double fontSize) {
    Metrics m;
    m.fontSize = fontSize;
    m.pillHeight = std::round(fontSize * 1.4);
    m.rowGap = 2;
    m.pillPadding = std::round(fontSize * 0.55);
    m.stroke = fontSize / 11 < 1 ? 1 : fontSize / 11;
    return m;
}

Metrics GetFittingMetrics(double availableHeight) {
    double fontSize = g_settings.fontSize;
    Metrics m = GetMetrics(fontSize);
    if (g_settings.layout == ClockLayout::TwoRows) {
        while (fontSize > 8 && m.pillHeight * 2 + m.rowGap > availableHeight) {
            fontSize -= 0.5;
            m = GetMetrics(fontSize);
        }
    } else {
        while (fontSize > 8 && m.pillHeight > availableHeight) {
            fontSize -= 0.5;
            m = GetMetrics(fontSize);
        }
    }
    return m;
}

struct BuildContext {
    Metrics metrics;
    Media::FontFamily fontFamily{nullptr};
    Media::SolidColorBrush textBrush{nullptr};
    Media::SolidColorBrush pillBrush{nullptr};
    Media::SolidColorBrush pillTextBrush{nullptr};
};

Controls::TextBlock MakeText(const BuildContext& context,
                             std::wstring_view text,
                             Media::SolidColorBrush const& brush,
                             double fontScale = 1) {
    Controls::TextBlock block;
    block.Text(text);
    block.FontFamily(context.fontFamily);
    block.FontSize(context.metrics.fontSize * fontScale);
    block.FontWeight(winrt::Windows::UI::Text::FontWeight{g_settings.fontWeight});
    block.Foreground(brush);
    // Trim the line box to the cap height, to center the text optically.
    block.TextLineBounds(TextLineBounds::Tight);
    block.IsTextScaleFactorEnabled(false);
    block.VerticalAlignment(VerticalAlignment::Center);
    block.HorizontalAlignment(HorizontalAlignment::Center);
    return block;
}

Controls::Border MakeFilledPill(const BuildContext& context,
                                std::wstring_view text) {
    const Metrics& m = context.metrics;
    double radius = m.pillHeight / 2;

    Controls::Border pill;
    pill.Height(m.pillHeight);
    pill.CornerRadius({radius, radius, radius, radius});
    pill.Background(context.pillBrush);
    pill.Padding({m.pillPadding, 0, m.pillPadding, 0});
    pill.VerticalAlignment(VerticalAlignment::Center);
    pill.Child(MakeText(context, text, context.pillTextBrush));
    return pill;
}

Controls::Border MakeOutlinedPill(const BuildContext& context,
                                  std::wstring_view text) {
    const Metrics& m = context.metrics;
    double radius = m.pillHeight / 2;
    double padding = m.pillPadding - m.stroke;

    Controls::Border pill;
    pill.Height(m.pillHeight);
    pill.CornerRadius({radius, radius, radius, radius});
    pill.BorderBrush(context.textBrush);
    pill.BorderThickness({m.stroke, m.stroke, m.stroke, m.stroke});
    pill.Padding({padding, 0, padding, 0});
    pill.VerticalAlignment(VerticalAlignment::Center);
    pill.Child(MakeText(context, text, context.textBrush));
    return pill;
}

// The temperature unit in a circle.
Controls::Border MakeUnitBadge(const BuildContext& context,
                               std::wstring_view unit) {
    const Metrics& m = context.metrics;
    double size = std::round(m.pillHeight * 0.83);

    Controls::Border badge;
    badge.Width(size);
    badge.Height(size);
    badge.CornerRadius({size / 2, size / 2, size / 2, size / 2});
    badge.BorderBrush(context.textBrush);
    badge.BorderThickness({m.stroke, m.stroke, m.stroke, m.stroke});
    badge.VerticalAlignment(VerticalAlignment::Center);
    badge.Margin({m.fontSize * 0.12, 0, 0, 0});
    badge.Child(MakeText(context, unit, context.textBrush, 0.74));
    return badge;
}

Shapes::Path MakeWeatherIcon(const BuildContext& context, WeatherIcon icon) {
    // "F1" selects the nonzero fill rule, so that the overlapping figures of
    // an icon merge into one silhouette.
    std::wstring xaml =
        L"<Path xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        L"presentation\" Stretch=\"Uniform\" Data=\"F1 " +
        WeatherIconPath(icon) + L"\"/>";

    auto path = Markup::XamlReader::Load(xaml).as<Shapes::Path>();
    path.Height(std::round(context.metrics.pillHeight * 0.95));
    path.Fill(context.textBrush);
    path.VerticalAlignment(VerticalAlignment::Center);
    return path;
}

// A filled pill with a speech bubble tail at its bottom left.
Controls::Grid MakeBubble(const BuildContext& context, std::wstring_view text) {
    const Metrics& m = context.metrics;
    double h = m.pillHeight;

    Media::PointCollection points;
    points.Append({0, static_cast<float>(h)});
    points.Append({static_cast<float>(h * 0.315), static_cast<float>(h * 0.52)});
    points.Append({static_cast<float>(h * 0.63), static_cast<float>(h * 0.93)});

    Shapes::Polygon tail;
    tail.Points(points);
    tail.Fill(context.pillBrush);
    tail.HorizontalAlignment(HorizontalAlignment::Left);
    tail.VerticalAlignment(VerticalAlignment::Top);

    Controls::Border pill = MakeFilledPill(context, text);
    pill.Margin({h * 0.16, 0, 0, 0});

    Controls::Grid bubble;
    bubble.VerticalAlignment(VerticalAlignment::Center);
    bubble.Children().Append(tail);
    bubble.Children().Append(pill);
    return bubble;
}

Controls::StackPanel MakeRow(const BuildContext& context) {
    Controls::StackPanel row;
    row.Orientation(Controls::Orientation::Horizontal);
    row.Height(context.metrics.pillHeight);
    row.HorizontalAlignment(g_settings.alignment);
    row.VerticalAlignment(VerticalAlignment::Center);
    return row;
}

void UpdateCursor(const ClockInstance& instance, const SYSTEMTIME& time) {
    if (instance.cursor) {
        bool visible =
            g_settings.cursor == CursorMode::Solid || time.wSecond % 2 == 0;
        instance.cursor.Opacity(visible ? 1 : 0);
    }
}

void BuildClockFace(ClockInstance& instance,
                    const DisplayState& state,
                    const Palette& palette,
                    double availableHeight) {
    BuildContext context;
    context.metrics = GetFittingMetrics(availableHeight);
    context.fontFamily = Media::FontFamily(g_settings.fontFamily);
    context.textBrush = Media::SolidColorBrush(palette.text);
    context.pillBrush = Media::SolidColorBrush(palette.pill);
    context.pillTextBrush = Media::SolidColorBrush(palette.pillText);

    const Metrics& m = context.metrics;
    double f = m.fontSize;

    instance.timeText = nullptr;
    instance.cursor = nullptr;

    auto root = instance.root;
    auto rows = root.Children();
    rows.Clear();

    bool oneRow = g_settings.layout == ClockLayout::OneRow;
    root.Orientation(oneRow ? Controls::Orientation::Horizontal
                            : Controls::Orientation::Vertical);
    root.Spacing(oneRow ? std::round(f * 0.9) : m.rowGap);

    // Date and time: FRI (11)(SEP) 17:13 []
    {
        auto row = MakeRow(context);
        auto items = row.Children();

        auto weekday = MakeText(context, state.weekday, context.textBrush);
        weekday.Margin({0, 0, f * 0.38, 0});
        items.Append(weekday);

        // The two pills touch at their tips.
        items.Append(MakeFilledPill(context, state.day));
        items.Append(MakeOutlinedPill(context, state.month));

        auto time = MakeText(context, state.time, context.textBrush);
        time.Margin({f * 0.5, 0, 0, 0});
        items.Append(time);
        instance.timeText = time;

        if (g_settings.cursor != CursorMode::Hidden) {
            Shapes::Rectangle cursor;
            cursor.Width(std::round(f * 0.42));
            cursor.Height(std::round(f * 0.92));
            cursor.Fill(context.textBrush);
            cursor.Margin({f * 0.3, 0, 0, 0});
            cursor.VerticalAlignment(VerticalAlignment::Center);
            items.Append(cursor);
            instance.cursor = cursor;
        }

        rows.Append(row);
    }

    // Weather: 24 (C) * (SUNNY)
    if (state.hasWeather) {
        auto row = MakeRow(context);
        auto items = row.Children();

        items.Append(MakeText(context, state.temperature, context.textBrush));
        items.Append(MakeUnitBadge(context, state.unit));

        try {
            auto icon = MakeWeatherIcon(context, state.icon);
            icon.Margin({f * 0.7, 0, f * 0.2, 0});
            items.Append(icon);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Failed to create the weather icon: %08X", e.code().value);
        }

        items.Append(MakeBubble(context, state.condition));

        rows.Append(row);
    }
}

void UpdateInstance(ClockInstance& instance,
                    const SYSTEMTIME& time,
                    const WeatherData& weather) {
    auto xamlRoot = instance.container.XamlRoot();
    if (!xamlRoot) {
        return;
    }

    // Leave some room above and below, the taskbar is 48 pixels high by
    // default.
    double taskbarHeight = xamlRoot.Size().Height;
    double availableHeight = taskbarHeight > 20 ? taskbarHeight - 10 : 38;

    DisplayState state = GetDisplayState(time, weather);
    Palette palette = GetPalette(instance);

    auto colorKey = [](winrt::Windows::UI::Color c) {
        return std::to_wstring((static_cast<uint32_t>(c.A) << 24) |
                               (c.R << 16) | (c.G << 8) | c.B);
    };

    std::wstring key = state.weekday + L'|' + state.day + L'|' + state.month +
                       L'|' + (state.hasWeather ? L"1" : L"0") + L'|' +
                       state.temperature + L'|' + state.unit + L'|' +
                       state.condition + L'|' +
                       std::to_wstring(static_cast<int>(state.icon)) + L'|' +
                       colorKey(palette.text) + L'|' + colorKey(palette.pill) +
                       L'|' + std::to_wstring(std::lround(availableHeight)) +
                       L'|' + std::to_wstring(g_settingsGeneration);

    if (key != instance.renderedKey) {
        BuildClockFace(instance, state, palette, availableHeight);
        instance.renderedKey = std::move(key);
        instance.renderedTime = state.time;
    } else if (state.time != instance.renderedTime) {
        if (instance.timeText) {
            instance.timeText.Text(state.time);
        }
        instance.renderedTime = state.time;
    }

    UpdateCursor(instance, time);
}

void RestoreInstance(ClockInstance& instance) {
    try {
        if (instance.container && instance.root) {
            auto children = instance.container.Children();
            uint32_t index;
            if (children.IndexOf(instance.root, index)) {
                children.RemoveAt(index);
            }
        }
        for (auto& [element, visibility] : instance.hiddenChildren) {
            element.Visibility(visibility);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to restore a clock: %08X", e.code().value);
    }
}

void InjectIntoClock(FrameworkElement const& dateTimeIconContent) {
    auto container = FindChildByName(dateTimeIconContent, L"ContainerGrid", 4)
                         .try_as<Controls::Panel>();
    if (!container) {
        // The template isn't applied yet, it will be picked up later.
        return;
    }

    for (const auto& instance : *g_instances) {
        if (instance.container == container) {
            return;
        }
    }

    auto children = container.Children();

    // Remove a leftover of a previous instance of the mod, if any.
    bool hadLeftover = false;
    for (uint32_t i = children.Size(); i-- > 0;) {
        auto element = children.GetAt(i).try_as<FrameworkElement>();
        if (element && element.Name() == kClockRootName) {
            children.RemoveAt(i);
            hadLeftover = true;
        }
    }

    ClockInstance instance;
    instance.container = container;
    instance.colorSource =
        FindChildByName(container, L"TimeInnerTextBlock", 4)
            .try_as<Controls::TextBlock>();

    for (auto child : children) {
        instance.hiddenChildren.emplace_back(
            child, hadLeftover ? Visibility::Visible : child.Visibility());
        child.Visibility(Visibility::Collapsed);
    }

    Controls::StackPanel root;
    root.Name(kClockRootName);
    root.VerticalAlignment(VerticalAlignment::Center);
    root.HorizontalAlignment(HorizontalAlignment::Center);
    root.Margin({2, 0, 2, 0});
    children.Append(root);
    instance.root = root;

    g_instances->push_back(std::move(instance));

    Wh_Log(L"Replaced a taskbar clock");
}

void ScanForClocks() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    std::vector<std::pair<HWND, bool>> taskbars{{hTaskbarWnd, false}};
    EnumThreadWindows(
        GetWindowThreadProcessId(hTaskbarWnd, nullptr),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR className[32];
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                auto& taskbars =
                    *reinterpret_cast<std::vector<std::pair<HWND, bool>>*>(
                        lParam);
                taskbars.emplace_back(hWnd, true);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbars));

    for (auto [hWnd, secondary] : taskbars) {
        XamlRoot xamlRoot = GetTaskbarXamlRoot(hWnd, secondary);
        if (!xamlRoot) {
            continue;
        }
        auto content = xamlRoot.Content();
        if (!content) {
            continue;
        }

        std::vector<FrameworkElement> clocks;
        FindChildrenByClassName(content, L"SystemTray.DateTimeIconContent", 64,
                                clocks);
        for (const auto& clock : clocks) {
            InjectIntoClock(clock);
        }
    }
}

// Drops clocks which are no longer part of a taskbar, e.g. after a monitor
// was disconnected.
void PruneInstances() {
    auto& instances = *g_instances;
    for (size_t i = instances.size(); i-- > 0;) {
        if (!instances[i].container.IsLoaded()) {
            RestoreInstance(instances[i]);
            instances.erase(instances.begin() + i);
        }
    }
}

void UpdateClocks() {
    SYSTEMTIME time;
    GetLocalTime(&time);
    WeatherData weather = GetWeatherSnapshot();

    for (auto& instance : *g_instances) {
        UpdateInstance(instance, time, weather);
    }
}

void RefreshClocks(bool forceScan) {
    PruneInstances();

    // With the DateTimeIconContent::OnApplyTemplate hook, new clocks are
    // reported as they're created, and the taskbar is only scanned at startup
    // for clocks which already exist, retrying for a few seconds in case the
    // taskbar isn't ready yet. Without the hook, e.g. if its symbol wasn't
    // found, look for new clocks every few seconds instead.
    bool scan = forceScan;
    if (g_instances->empty() && g_startupScansLeft > 0) {
        g_startupScansLeft--;
        scan = true;
    } else if (!g_clockHookApplied && g_tickCounter++ % 4 == 0) {
        scan = true;
    }

    if (scan) {
        ScanForClocks();
    }

    UpdateClocks();
}

void OnTimerTick() {
    if (g_unloading) {
        return;
    }

    try {
        RefreshClocks(false);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Refresh failed: %08X", e.code().value);
    } catch (...) {
        Wh_Log(L"Refresh failed");
    }

    // Tick again right after the next second starts.
    try {
        SYSTEMTIME time;
        GetLocalTime(&time);
        g_timer.Stop();
        g_timer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(1000 - time.wMilliseconds + 10)});
        g_timer.Start();
    } catch (...) {
    }
}

void StartClockUi() {
    if (g_unloading || g_timer) {
        return;
    }

    g_startupScansLeft = 10;

    // Started here rather than at initialization, to only make network
    // requests in the explorer process which hosts the taskbar.
    if (!g_weatherThreadStarted.exchange(true)) {
        std::lock_guard<std::mutex> lock(g_weatherThreadMutex);
        StartWeatherThread(g_settings);
    }

    try {
        DispatcherTimer timer;
        timer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(1000)});
        g_timerTickToken = timer.Tick(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::Foundation::IInspectable const&) {
                OnTimerTick();
            });
        timer.Start();
        g_timer = timer;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to start the timer: %08X", e.code().value);
        return;
    }

    OnTimerTick();
}

void StopClockUi() {
    if (g_timer) {
        try {
            g_timer.Stop();
            g_timer.Tick(g_timerTickToken);
        } catch (...) {
        }
        g_timer = nullptr;
    }

    if (g_instances) {
        for (auto& instance : *g_instances) {
            RestoreInstance(instance);
        }
        // Release the XAML objects here, on the taskbar thread.
        g_instances.reset();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Hooks and entry points

using RunFromWindowThreadProc_t = void (*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                            cwp->lParam);
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return true;
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;
void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L">");

    TrayUI_StartTaskbar_Original(pThis);

    if (g_unloading) {
        return;
    }

    // Explorer has just created the taskbar, e.g. after signing in.
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTaskbarWnd, [](void*) { StartClockUi(); }, nullptr);
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
        // Only needed for the clocks of secondary monitors.
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

using DateTimeIconContent_OnApplyTemplate_t = HRESULT(WINAPI*)(void* pThis);
DateTimeIconContent_OnApplyTemplate_t
    DateTimeIconContent_OnApplyTemplate_Original;
HRESULT WINAPI DateTimeIconContent_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");

    HRESULT ret = DateTimeIconContent_OnApplyTemplate_Original(pThis);

    if (g_unloading) {
        return ret;
    }

    FrameworkElement dateTimeIconContent = nullptr;
    static_cast<IUnknown*>(pThis)->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(dateTimeIconContent));
    if (!dateTimeIconContent) {
        return ret;
    }

    // A clock was created, e.g. for a new monitor or a rebuilt taskbar. This
    // runs on the taskbar thread.
    try {
        if (!g_timer) {
            // Scans for all clocks, including this one.
            StartClockUi();
        } else if (g_instances) {
            InjectIntoClock(dateTimeIconContent);
            UpdateClocks();
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to replace a clock: %08X", e.code().value);
    } catch (...) {
        Wh_Log(L"Failed to replace a clock");
    }

    return ret;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    HRSRC resource =
        FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    HGLOBAL global = resource ? LoadResource(module, resource) : nullptr;
    void* data = global ? LockResource(global) : nullptr;
    void* fixedFileInfo = nullptr;
    UINT length = 0;
    if (!data || !VerQueryValue(data, L"\\", &fixedFileInfo, &length) ||
        length == 0) {
        return nullptr;
    }
    return static_cast<VS_FIXEDFILEINFO*>(fixedFileInfo);
}

// The module which implements the system tray's XAML types. It depends on the
// Windows build.
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604, the system tray types are in
            // SystemTray.dll.
            VS_FIXEDFILEINFO* info = GetModuleVersionInfo(module);
            WORD major = info ? HIWORD(info->dwFileVersionMS) : 0;
            if (!major || major >= 2604) {
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

std::atomic<bool> g_systemTrayModuleHooked;

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        // Optional: without it, new clocks are found by scanning the taskbar.
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::DateTimeIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for the system tray module");
        return false;
    }

    if (!DateTimeIconContent_OnApplyTemplate_Original) {
        Wh_Log(L"DateTimeIconContent::OnApplyTemplate not found");
        return false;
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_systemTrayModuleHooked &&
        GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
            g_clockHookApplied = true;
        }
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_settings = LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    // The system tray module is usually loaded with the taskbar, which might
    // happen after the mod is loaded.
    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        // Hooks set here are applied once Wh_ModInit returns, before the clocks
        // are replaced in Wh_ModAfterInit.
        g_clockHookApplied = HookSystemTraySymbols(systemTrayModule);
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            reinterpret_cast<decltype(&LoadLibraryExW)>(
                GetProcAddress(kernelBaseModule, "LoadLibraryExW"));
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // If the taskbar already exists, replace its clocks right away. Otherwise,
    // it's done once the taskbar is created.
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTaskbarWnd, [](void*) { StartClockUi(); }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTaskbarWnd, [](void*) { StopClockUi(); }, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    std::lock_guard<std::mutex> lock(g_weatherThreadMutex);
    StopWeatherThread();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    Settings settings = LoadSettings();

    bool weatherChanged =
        settings.weatherEnabled != g_settings.weatherEnabled ||
        settings.weatherLocation != g_settings.weatherLocation ||
        settings.weatherDetectLocation != g_settings.weatherDetectLocation ||
        settings.weatherFahrenheit != g_settings.weatherFahrenheit ||
        settings.weatherUpdateMinutes != g_settings.weatherUpdateMinutes;

    auto apply = [](void* param) {
        g_settings = *static_cast<Settings*>(param);
        g_settingsGeneration++;

        if (g_timer) {
            try {
                RefreshClocks(false);
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Refresh failed: %08X", e.code().value);
            } catch (...) {
                Wh_Log(L"Refresh failed");
            }
        }
    };

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd || !RunFromWindowThread(hTaskbarWnd, apply, &settings)) {
        g_settings = settings;
        g_settingsGeneration++;
    }

    // Done here and not in the taskbar thread, as stopping the weather thread
    // might have to wait for a pending request.
    if (weatherChanged && g_weatherThreadStarted) {
        std::lock_guard<std::mutex> lock(g_weatherThreadMutex);
        StopWeatherThread();
        {
            std::lock_guard<std::mutex> weatherLock(g_weatherMutex);
            g_weather = {};
        }
        StartWeatherThread(settings);
    }
}
