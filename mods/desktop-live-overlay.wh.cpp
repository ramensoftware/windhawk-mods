// ==WindhawkMod==
// @id              desktop-live-overlay
// @name            Desktop Live Overlay
// @description     Display live, customizable content on the desktop behind icons. Perfect for showing time, date, system metrics, weather, and more.
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldxgi -ld2d1 -ldwrite -ld3d11 -ldcomp -lwininet -lpdh -lpowrprof -lshcore -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Desktop Live Overlay

Display live, customizable content on the desktop behind icons. Perfect for
showing time, date, system metrics, weather, and more.

### Features

* Two customizable text lines with independent styling
* Dynamic patterns for time, date, system info, and weather
* Font family, size, weight, and style options per line
* Text color with transparency support
* Optional background with customizable color, padding, and corner radius
* Customizable positioning

### Supported Patterns

**Time/Date:**
* `%time%` - Current time
* `%date%` - Current date
* `%weekday%` - Day of week name
* `%weekday_num%` - Day of week as number (1-7)
* `%weeknum%` - Week number of year
* `%dayofyear%` - Day of year (1-366)
* `%timezone%` - Timezone offset (e.g., +02:00)

**System Metrics:**
* `%cpu%` - CPU usage percentage
* `%ram%` - RAM usage percentage
* `%battery%` - Battery percentage
* `%battery_time%` - Battery time remaining
* `%power%` - Power consumption in watts
* `%upload_speed%` - Network upload speed
* `%download_speed%` - Network download speed
* `%total_speed%` - Combined network speed
* `%disk_read%` - Disk read speed
* `%disk_write%` - Disk write speed
* `%disk_total%` - Combined disk I/O speed
* `%gpu%` - GPU usage percentage
* `%weather%` - Weather from [wttr.in](https://wttr.in/)

**Other:**
* `%newline%` or `%n%` - Line break

![Screenshot](https://i.imgur.com/BW52sWf.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- topLine:
  - text: "%time%"
    $name: Text
    $description: >-
      Text to display. Supported patterns are listed in the mod description.
  - fontSize: 48
    $name: Font size
    $description: Size of the text in points
  - textColor: "#C0FFFFFF"
    $name: Text color
    $description: >-
      Color in ARGB hex format. Examples: #80FFFFFF (semi-transparent white),
      #FF0000 (red), #80000000 (semi-transparent black)
  - fontFamily: Segoe UI
    $name: Font family
    $description: >-
      Font family name. For a list of fonts shipped with Windows, see:
      https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
  - fontWeight: Bold
    $name: Font weight
    $options:
    - "": Default
    - Thin: Thin
    - Light: Light
    - Normal: Normal
    - Medium: Medium
    - SemiBold: Semi bold
    - Bold: Bold
    - ExtraBold: Extra bold
  - fontStyle: ""
    $name: Font style
    $options:
    - "": Default
    - Normal: Normal
    - Italic: Italic
  $name: Top line
- bottomLine:
  - text: "%date%%n%%weather%"
    $name: Text
    $description: >-
      Text to display. Supported patterns are listed in the mod description.
  - fontSize: 32
    $name: Font size
    $description: Size of the text in points
  - textColor: "#80FFFFFF"
    $name: Text color
    $description: >-
      Color in ARGB hex format. Examples: #80FFFFFF (semi-transparent white),
      #FF0000 (red), #80000000 (semi-transparent black)
  - fontFamily: Segoe UI
    $name: Font family
    $description: >-
      Font family name. For a list of fonts shipped with Windows, see:
      https://learn.microsoft.com/en-us/typography/fonts/windows_11_font_list
  - fontWeight: ""
    $name: Font weight
    $options:
    - "": Default
    - Thin: Thin
    - Light: Light
    - Normal: Normal
    - Medium: Medium
    - SemiBold: Semi bold
    - Bold: Bold
    - ExtraBold: Extra bold
  - fontStyle: ""
    $name: Font style
    $options:
    - "": Default
    - Normal: Normal
    - Italic: Italic
  $name: Bottom line
- showSeconds: false
  $name: Show seconds
- timeFormat: ""
  $name: Time format
  $description: >-
    Leave empty for the default format. For syntax refer to the following page:

    https://docs.microsoft.com/en-us/windows/win32/api/datetimeapi/nf-datetimeapi-gettimeformatex#remarks
- dateFormat: ""
  $name: Date format
  $description: >-
    Leave empty for the default format. For syntax refer to the following page:

    https://docs.microsoft.com/en-us/windows/win32/intl/day--month--year--and-era-format-pictures
- refreshInterval: 1
  $name: Refresh interval (seconds)
  $description: How often to update dynamic content (1-60)
- background:
  - enabled: true
    $name: Enabled
  - color: "#80000000"
    $name: Color
    $description: >-
      Background color in ARGB hex format. Default is semi-transparent black.
  - padding: 20
    $name: Padding
    $description: Padding around the text in pixels
  - cornerRadius: 8
    $name: Corner radius
    $description: Border radius for rounded corners in pixels
  $name: Background
- verticalPosition: 20
  $name: Vertical position
  $description: Position in percentage (0 = top, 50 = center, 100 = bottom)
- horizontalPosition: 50
  $name: Horizontal position
  $description: Position in percentage (0 = left, 50 = center, 100 = right)
- monitor: 1
  $name: Monitor
  $description: The monitor number to display text on (1-based)
- weatherLocation: ""
  $name: Weather location
  $description: >-
    Location for weather (city name, coordinates, etc.). Empty uses auto-detect
    based on IP. For details, refer to the documentation of wttr.in.
- weatherFormat: "%c %t"
  $name: Weather format
  $description: >-
    The weather information format. For details, refer to the documentation of
    wttr.in.
- weatherUnits: autoDetect
  $name: Weather units
  $description: >-
    The weather units. For details, refer to the documentation of wttr.in.
  $options:
  - autoDetect: Auto (default)
  - uscs: USCS (used by default in US)
  - metric: Metric (SI) (used by default everywhere except US)
  - metricMsWind: Metric (SI), but show wind speed in m/s
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwrite.h>
#include <dxgi1_3.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <powrprof.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <wininet.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

using namespace std::literals;
using Microsoft::WRL::ComPtr;

#define TIMER_ID_REFRESH 1
#define OVERLAY_WINDOW_CLASS (L"DesktopOverlay_" WH_MOD_ID)

////////////////////////////////////////////////////////////////////////////////
// Types

enum class WeatherUnits {
    autoDetect,
    uscs,
    metric,
    metricMsWind,
};

struct LineSettings {
    WindhawkUtils::StringSetting text;
    int fontSize;
    BYTE colorA;
    BYTE colorR;
    BYTE colorG;
    BYTE colorB;
    WindhawkUtils::StringSetting fontFamily;
    int fontWeight;
    bool fontItalic;
};

struct Settings {
    LineSettings topLine;
    LineSettings bottomLine;
    bool showSeconds;
    WindhawkUtils::StringSetting timeFormat;
    WindhawkUtils::StringSetting dateFormat;
    int refreshInterval;
    bool backgroundEnabled;
    BYTE backgroundColorA;
    BYTE backgroundColorR;
    BYTE backgroundColorG;
    BYTE backgroundColorB;
    int backgroundPadding;
    int backgroundCornerRadius;
    int verticalPosition;
    int horizontalPosition;
    int monitor;
    WindhawkUtils::StringSetting weatherLocation;
    WindhawkUtils::StringSetting weatherFormat;
    WeatherUnits weatherUnits;
};

constexpr size_t FORMATTED_BUFFER_SIZE = 256;
constexpr size_t INTEGER_BUFFER_SIZE = sizeof("-2147483648");

template <size_t N>
struct FormattedString {
    DWORD formatIndex = 0;
    WCHAR buffer[N] = {};
};

////////////////////////////////////////////////////////////////////////////////
// Globals

Settings g_settings;

// Format state.
SYSTEMTIME g_formatTime;
DWORD g_formatIndex = 0;

FormattedString<FORMATTED_BUFFER_SIZE> g_timeFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_dateFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_weekdayFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weekdayNumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weeknumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_dayOfYearFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_timezoneFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_cpuFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_ramFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_batteryFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_batteryTimeFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_powerFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_uploadSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_downloadSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_totalSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_diskReadSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_diskWriteSpeedFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_diskTotalSpeedFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_gpuFormatted;

// Performance metrics.
PDH_HQUERY g_metricsQuery = nullptr;
PDH_HCOUNTER g_cpuCounter = nullptr;
std::vector<PDH_HCOUNTER> g_uploadCounters;
std::vector<PDH_HCOUNTER> g_downloadCounters;
PDH_HCOUNTER g_diskReadCounter = nullptr;
PDH_HCOUNTER g_diskWriteCounter = nullptr;
std::vector<PDH_HCOUNTER> g_gpuCounters;

// Weather web content.
HANDLE g_weatherUpdateThread = nullptr;
HANDLE g_weatherUpdateStopEvent = nullptr;
std::mutex g_weatherMutex;
std::atomic<bool> g_weatherLoaded{false};
std::atomic<bool> g_unloading{false};
std::optional<std::wstring> g_weatherContent;

// Cached result of whether system metrics are used (set during init).
bool g_systemMetricsUsed = false;

// DirectX device objects (shared).
ComPtr<ID3D11Device> g_d3dDevice;
ComPtr<IDXGIDevice> g_dxgiDevice;
ComPtr<IDXGIFactory2> g_dxgiFactory;
ComPtr<ID2D1Factory1> g_d2dFactory;
ComPtr<ID2D1Device> g_d2dDevice;
ComPtr<IDWriteFactory> g_dwriteFactory;

// Message-only window for receiving system notifications.
HWND g_messageWnd;

// Overlay window and resources.
HWND g_overlayWnd;
ComPtr<IDXGISwapChain1> g_swapChain;
ComPtr<ID2D1DeviceContext> g_dc;
ComPtr<IDCompositionDevice> g_compositionDevice;
ComPtr<IDCompositionTarget> g_compositionTarget;
ComPtr<IDCompositionVisual> g_compositionVisual;
ComPtr<IDWriteTextFormat> g_topLineTextFormat;
ComPtr<ID2D1SolidColorBrush> g_topLineTextBrush;
ComPtr<IDWriteTextFormat> g_bottomLineTextFormat;
ComPtr<ID2D1SolidColorBrush> g_bottomLineTextBrush;
ComPtr<ID2D1SolidColorBrush> g_backgroundBrush;

// Current DPI scale factor (1.0 = 96 DPI).
float g_dpiScale = 1.0f;

////////////////////////////////////////////////////////////////////////////////
// Utility functions

float GetMonitorDpiScale(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return dpiX / 96.0f;
    }
    return 1.0f;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

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
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
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
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR monitorResult = nullptr;
    int currentMonitorId = 0;

    auto monitorEnumProc = [&monitorResult, &currentMonitorId,
                            monitorId](HMONITOR hMonitor) -> BOOL {
        if (currentMonitorId == monitorId) {
            monitorResult = hMonitor;
            return FALSE;
        }
        currentMonitorId++;
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

bool ParseColor(PCWSTR colorStr, BYTE* a, BYTE* r, BYTE* g, BYTE* b) {
    if (!colorStr || !*colorStr) {
        return false;
    }

    if (*colorStr == L'#') {
        colorStr++;
    }

    size_t len = wcslen(colorStr);
    unsigned int value = 0;

    for (size_t i = 0; i < len; i++) {
        WCHAR c = colorStr[i];
        int digit;
        if (c >= L'0' && c <= L'9') {
            digit = c - L'0';
        } else if (c >= L'A' && c <= L'F') {
            digit = c - L'A' + 10;
        } else if (c >= L'a' && c <= L'f') {
            digit = c - L'a' + 10;
        } else {
            return false;
        }
        value = (value << 4) | digit;
    }

    if (len == 6) {
        *a = 255;
        *r = (value >> 16) & 0xFF;
        *g = (value >> 8) & 0xFF;
        *b = value & 0xFF;
    } else if (len == 8) {
        *a = (value >> 24) & 0xFF;
        *r = (value >> 16) & 0xFF;
        *g = (value >> 8) & 0xFF;
        *b = value & 0xFF;
    } else {
        return false;
    }

    return true;
}

DWRITE_FONT_WEIGHT GetDWriteFontWeight(int fontWeight) {
    if (fontWeight <= 100)
        return DWRITE_FONT_WEIGHT_THIN;
    if (fontWeight <= 200)
        return DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
    if (fontWeight <= 300)
        return DWRITE_FONT_WEIGHT_LIGHT;
    if (fontWeight <= 400)
        return DWRITE_FONT_WEIGHT_NORMAL;
    if (fontWeight <= 500)
        return DWRITE_FONT_WEIGHT_MEDIUM;
    if (fontWeight <= 600)
        return DWRITE_FONT_WEIGHT_SEMI_BOLD;
    if (fontWeight <= 700)
        return DWRITE_FONT_WEIGHT_BOLD;
    if (fontWeight <= 800)
        return DWRITE_FONT_WEIGHT_EXTRA_BOLD;
    return DWRITE_FONT_WEIGHT_BLACK;
}

int StringCopyTruncated(PWSTR dest,
                        size_t destSize,
                        PCWSTR src,
                        bool* truncated) {
    if (destSize == 0) {
        *truncated = *src;
        return 0;
    }

    size_t i;
    for (i = 0; i < destSize - 1 && *src; i++) {
        *dest++ = *src++;
    }

    *dest = L'\0';
    *truncated = *src;
    return static_cast<int>(i);
}

////////////////////////////////////////////////////////////////////////////////
// Pattern formatting

std::vector<std::wstring> ExpandPdhWildcard(PCWSTR wildcardPath) {
    std::vector<std::wstring> result;

    DWORD pathListLength = 0;
    PDH_STATUS status = PdhExpandWildCardPathW(nullptr, wildcardPath, nullptr,
                                               &pathListLength, 0);
    if (status != static_cast<PDH_STATUS>(PDH_MORE_DATA) ||
        pathListLength == 0) {
        return result;
    }

    std::wstring pathList(pathListLength, L'\0');
    status = PdhExpandWildCardPathW(nullptr, wildcardPath, pathList.data(),
                                    &pathListLength, 0);
    if (status != static_cast<PDH_STATUS>(ERROR_SUCCESS)) {
        return result;
    }

    // Parse null-terminated list of paths.
    PCWSTR p = pathList.c_str();
    while (*p) {
        result.push_back(p);
        p += wcslen(p) + 1;
    }

    return result;
}

std::optional<std::wstring> GetUrlContent(PCWSTR lpUrl) {
    HINTERNET hOpenHandle = InternetOpen(
        L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hOpenHandle) {
        return std::nullopt;
    }

    HINTERNET hUrlHandle =
        InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
                        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
                        0);
    if (!hUrlHandle) {
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    if (!HttpQueryInfo(hUrlHandle,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &dwStatusCode, &dwStatusCodeSize, nullptr) ||
        dwStatusCode != 200) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    LPBYTE pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (!pUrlContent) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwNumberOfBytesRead;
    InternetReadFile(hUrlHandle, pUrlContent, 0x400, &dwNumberOfBytesRead);
    DWORD dwLength = dwNumberOfBytesRead;

    while (dwNumberOfBytesRead) {
        LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(
            GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
        if (!pNewUrlContent) {
            InternetCloseHandle(hUrlHandle);
            InternetCloseHandle(hOpenHandle);
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        pUrlContent = pNewUrlContent;
        InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400,
                         &dwNumberOfBytesRead);
        dwLength += dwNumberOfBytesRead;
    }

    InternetCloseHandle(hUrlHandle);
    InternetCloseHandle(hOpenHandle);

    // Assume UTF-8.
    int charsNeeded = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent,
                                          dwLength, nullptr, 0);
    std::wstring unicodeContent(charsNeeded, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent, dwLength,
                        unicodeContent.data(), unicodeContent.size());

    HeapFree(GetProcessHeap(), 0, pUrlContent);

    return unicodeContent;
}

std::wstring ReplaceAll(std::wstring_view source,
                        std::wstring_view from,
                        std::wstring_view to) {
    std::wstring newString;

    size_t lastPos = 0;
    size_t findPos;

    while ((findPos = source.find(from, lastPos)) != source.npos) {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    newString += source.substr(lastPos);

    return newString;
}

std::wstring EscapeUrlComponent(PCWSTR input) {
    WCHAR outStack[256];
    DWORD needed = ARRAYSIZE(outStack);
    HRESULT hr = UrlEscape(input, outStack, &needed,
                           URL_ESCAPE_ASCII_URI_COMPONENT | URL_ESCAPE_AS_UTF8);
    if (SUCCEEDED(hr)) {
        return outStack;
    }

    if (hr != E_POINTER || needed < 1) {
        return std::wstring();
    }

    std::wstring out(needed - 1, L'\0');
    hr = UrlEscape(input, &out[0], &needed,
                   URL_ESCAPE_ASCII_URI_COMPONENT | URL_ESCAPE_AS_UTF8);
    if (FAILED(hr)) {
        return std::wstring();
    }

    return out;
}

std::wstring GetWeatherCacheKey() {
    // Change the URL every 10 minutes to avoid caching.
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER uli{
        .LowPart = ft.dwLowDateTime,
        .HighPart = ft.dwHighDateTime,
    };
    uli.QuadPart /= 10000000ULL * 60 * 10;
    return std::to_wstring(uli.QuadPart);
}

bool UpdateWeatherContent() {
    std::wstring format = g_settings.weatherFormat.get();
    if (format.empty()) {
        format = L"%c %t";
    }

    // Spaces are added after the weather emoji by the server. Add a marker
    // character after it to be able to remove the spaces.
    // https://github.com/chubin/wttr.in/issues/345
    format = ReplaceAll(format, L"%c", L"%c\uE000");

    std::wstring weatherUrl = L"https://wttr.in/";
    weatherUrl += EscapeUrlComponent(g_settings.weatherLocation.get());
    weatherUrl += L'?';
    switch (g_settings.weatherUnits) {
        case WeatherUnits::autoDetect:
            break;
        case WeatherUnits::uscs:
            weatherUrl += L"u&";
            break;
        case WeatherUnits::metric:
            weatherUrl += L"m&";
            break;
        case WeatherUnits::metricMsWind:
            weatherUrl += L"M&";
            break;
    }
    weatherUrl += L"format=";
    weatherUrl += EscapeUrlComponent(format.c_str());
    // Set a random language as a way to avoid caching the result.
    // https://github.com/chubin/wttr.in/issues/705#issuecomment-3109898903
    weatherUrl += L"&lang=_nocache_";
    weatherUrl += GetWeatherCacheKey();

    std::optional<std::wstring> urlContent = GetUrlContent(weatherUrl.c_str());
    if (!urlContent) {
        return false;
    }

    // Ignore non-weather responses.
    if (urlContent->empty() ||
        *urlContent == L"This query is already being processed") {
        return false;
    }

    // Remove spaces after the %c emoji.
    std::wstring weatherContent;

    size_t lastPos = 0;
    size_t findPos;

    while ((findPos = urlContent->find(L'\uE000', lastPos)) !=
           urlContent->npos) {
        size_t lastPosCount = findPos - lastPos;
        while (lastPosCount > 0 &&
               urlContent->at(lastPos + lastPosCount - 1) == L' ') {
            lastPosCount--;
        }

        weatherContent.append(*urlContent, lastPos, lastPosCount);
        lastPos = findPos + 1;
    }

    // Care for the rest after last occurrence.
    weatherContent += urlContent->substr(lastPos);

    {
        std::lock_guard<std::mutex> guard(g_weatherMutex);
        g_weatherContent = weatherContent;
        g_weatherLoaded = true;
    }

    return true;
}

DWORD WINAPI WeatherUpdateThread(LPVOID lpThreadParameter) {
    constexpr DWORD kSecondsForQuickRetry = 30;
    constexpr DWORD kSecondsForNormalUpdate = 600;  // 10 minutes

    while (true) {
        UpdateWeatherContent();

        DWORD seconds = kSecondsForNormalUpdate;
        if (!g_weatherLoaded && seconds > kSecondsForQuickRetry) {
            seconds = kSecondsForQuickRetry;
        }

        DWORD dwWaitResult =
            WaitForSingleObject(g_weatherUpdateStopEvent, seconds * 1000);
        if (dwWaitResult == WAIT_OBJECT_0) {
            break;  // Stop event signaled
        }
    }

    return 0;
}

bool IsPatternUsed(PCWSTR pattern) {
    PCWSTR topText = g_settings.topLine.text.get();
    PCWSTR bottomText = g_settings.bottomLine.text.get();
    return (topText && wcsstr(topText, pattern)) ||
           (bottomText && wcsstr(bottomText, pattern));
}

bool IsSystemMetricsUsed() {
    return IsPatternUsed(L"%cpu%") || IsPatternUsed(L"%ram%") ||
           IsPatternUsed(L"%battery%") || IsPatternUsed(L"%battery_time%") ||
           IsPatternUsed(L"%power%") || IsPatternUsed(L"%upload_speed%") ||
           IsPatternUsed(L"%download_speed%") ||
           IsPatternUsed(L"%total_speed%") || IsPatternUsed(L"%disk_read%") ||
           IsPatternUsed(L"%disk_write%") || IsPatternUsed(L"%disk_total%") ||
           IsPatternUsed(L"%gpu%");
}

void WeatherUpdateThreadInit() {
    // Check if weather pattern is used in the text.
    if (!IsPatternUsed(L"%weather%")) {
        return;
    }

    g_weatherUpdateStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_weatherUpdateThread =
        CreateThread(nullptr, 0, WeatherUpdateThread, nullptr, 0, nullptr);
}

void WeatherUpdateThreadUninit() {
    if (g_weatherUpdateThread) {
        SetEvent(g_weatherUpdateStopEvent);
        WaitForSingleObject(g_weatherUpdateThread, INFINITE);
        CloseHandle(g_weatherUpdateThread);
        g_weatherUpdateThread = nullptr;
        CloseHandle(g_weatherUpdateStopEvent);
        g_weatherUpdateStopEvent = nullptr;
    }

    std::lock_guard<std::mutex> guard(g_weatherMutex);
    g_weatherLoaded = false;
    g_weatherContent.reset();
}

bool InitMetrics() {
    // Determine which metrics are needed based on patterns used.
    bool needCpu = IsPatternUsed(L"%cpu%");
    bool needUpload = IsPatternUsed(L"%upload_speed%");
    bool needDownload = IsPatternUsed(L"%download_speed%");
    bool needDiskRead = IsPatternUsed(L"%disk_read%");
    bool needDiskWrite = IsPatternUsed(L"%disk_write%");
    bool needGpu = IsPatternUsed(L"%gpu%");

    // %total_speed% requires both upload and download.
    if (IsPatternUsed(L"%total_speed%")) {
        needUpload = true;
        needDownload = true;
    }

    // %disk_total% requires both read and write.
    if (IsPatternUsed(L"%disk_total%")) {
        needDiskRead = true;
        needDiskWrite = true;
    }

    // If no PDH metrics are needed, skip initialization.
    if (!needCpu && !needUpload && !needDownload && !needDiskRead &&
        !needDiskWrite && !needGpu) {
        return true;
    }

    if (PdhOpenQuery(nullptr, 0, &g_metricsQuery) != ERROR_SUCCESS) {
        return false;
    }

    // CPU counter.
    if (needCpu) {
        PdhAddEnglishCounter(
            g_metricsQuery,
            L"\\Processor Information(_Total)\\% Processor Utility", 0,
            &g_cpuCounter);
    }

    // Network upload counters (wildcard expansion).
    if (needUpload) {
        auto uploadPaths =
            ExpandPdhWildcard(L"\\Network Interface(*)\\Bytes Sent/sec");
        for (const auto& path : uploadPaths) {
            PDH_HCOUNTER counter;
            if (PdhAddCounter(g_metricsQuery, path.c_str(), 0, &counter) ==
                ERROR_SUCCESS) {
                g_uploadCounters.push_back(counter);
            }
        }
    }

    // Network download counters (wildcard expansion).
    if (needDownload) {
        auto downloadPaths =
            ExpandPdhWildcard(L"\\Network Interface(*)\\Bytes Received/sec");
        for (const auto& path : downloadPaths) {
            PDH_HCOUNTER counter;
            if (PdhAddCounter(g_metricsQuery, path.c_str(), 0, &counter) ==
                ERROR_SUCCESS) {
                g_downloadCounters.push_back(counter);
            }
        }
    }

    // Disk read/write counters.
    if (needDiskRead) {
        PdhAddEnglishCounter(g_metricsQuery,
                             L"\\PhysicalDisk(_Total)\\Disk Read Bytes/sec", 0,
                             &g_diskReadCounter);
    }
    if (needDiskWrite) {
        PdhAddEnglishCounter(g_metricsQuery,
                             L"\\PhysicalDisk(_Total)\\Disk Write Bytes/sec", 0,
                             &g_diskWriteCounter);
    }

    // GPU engine counters (wildcard expansion).
    if (needGpu) {
        auto gpuPaths =
            ExpandPdhWildcard(L"\\GPU Engine(*)\\Utilization Percentage");
        for (const auto& path : gpuPaths) {
            PDH_HCOUNTER counter;
            if (PdhAddCounter(g_metricsQuery, path.c_str(), 0, &counter) ==
                ERROR_SUCCESS) {
                g_gpuCounters.push_back(counter);
            }
        }
    }

    // First call initializes the counters.
    PdhCollectQueryData(g_metricsQuery);
    return true;
}

void UninitMetrics() {
    if (g_metricsQuery) {
        PdhCloseQuery(g_metricsQuery);
        g_metricsQuery = nullptr;
        g_cpuCounter = nullptr;
        g_uploadCounters.clear();
        g_downloadCounters.clear();
        g_diskReadCounter = nullptr;
        g_diskWriteCounter = nullptr;
        g_gpuCounters.clear();
    }
}

PCWSTR GetTimeFormatted() {
    if (g_timeFormatted.formatIndex != g_formatIndex) {
        PCWSTR format = g_settings.timeFormat.get();
        DWORD dwFlags = g_settings.showSeconds ? 0 : TIME_NOSECONDS;

        if (!g_settings.showSeconds && *format) {
            // Remove seconds from custom format.
            std::wstring formatNoSeconds = ReplaceAll(format, L"':'ss", L"");
            GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, dwFlags, &g_formatTime,
                            formatNoSeconds.c_str(), g_timeFormatted.buffer,
                            ARRAYSIZE(g_timeFormatted.buffer));
        } else {
            GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, dwFlags, &g_formatTime,
                            *format ? format : nullptr, g_timeFormatted.buffer,
                            ARRAYSIZE(g_timeFormatted.buffer));
        }
        g_timeFormatted.formatIndex = g_formatIndex;
    }
    return g_timeFormatted.buffer;
}

PCWSTR GetDateFormatted() {
    if (g_dateFormatted.formatIndex != g_formatIndex) {
        PCWSTR format = g_settings.dateFormat.get();
        GetDateFormat(LOCALE_USER_DEFAULT, DATE_AUTOLAYOUT, &g_formatTime,
                      (format && *format) ? format : nullptr,
                      g_dateFormatted.buffer,
                      ARRAYSIZE(g_dateFormatted.buffer));
        g_dateFormatted.formatIndex = g_formatIndex;
    }
    return g_dateFormatted.buffer;
}

PCWSTR GetWeekdayFormatted() {
    if (g_weekdayFormatted.formatIndex != g_formatIndex) {
        GetDateFormat(LOCALE_USER_DEFAULT, DATE_AUTOLAYOUT, &g_formatTime,
                      L"dddd", g_weekdayFormatted.buffer,
                      ARRAYSIZE(g_weekdayFormatted.buffer));
        g_weekdayFormatted.formatIndex = g_formatIndex;
    }
    return g_weekdayFormatted.buffer;
}

PCWSTR GetCpuFormatted() {
    if (g_cpuFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery) {
            PdhCollectQueryData(g_metricsQuery);
            PDH_FMT_COUNTERVALUE val;
            if (PdhGetFormattedCounterValue(g_cpuCounter, PDH_FMT_DOUBLE,
                                            nullptr, &val) == ERROR_SUCCESS) {
                swprintf_s(g_cpuFormatted.buffer, L"%d%%",
                           (int)val.doubleValue);
            } else {
                wcscpy_s(g_cpuFormatted.buffer, L"-");
            }
        } else {
            wcscpy_s(g_cpuFormatted.buffer, L"-");
        }
        g_cpuFormatted.formatIndex = g_formatIndex;
    }
    return g_cpuFormatted.buffer;
}

PCWSTR GetRamFormatted() {
    if (g_ramFormatted.formatIndex != g_formatIndex) {
        MEMORYSTATUSEX status{.dwLength = sizeof(status)};
        if (GlobalMemoryStatusEx(&status)) {
            swprintf_s(g_ramFormatted.buffer, L"%d%%",
                       (int)status.dwMemoryLoad);
        } else {
            wcscpy_s(g_ramFormatted.buffer, L"-");
        }
        g_ramFormatted.formatIndex = g_formatIndex;
    }
    return g_ramFormatted.buffer;
}

PCWSTR GetBatteryFormatted() {
    if (g_batteryFormatted.formatIndex != g_formatIndex) {
        SYSTEM_POWER_STATUS ps;
        if (GetSystemPowerStatus(&ps) && ps.BatteryLifePercent != 255) {
            swprintf_s(g_batteryFormatted.buffer, L"%d%%",
                       (int)ps.BatteryLifePercent);
        } else {
            wcscpy_s(g_batteryFormatted.buffer, L"-");
        }
        g_batteryFormatted.formatIndex = g_formatIndex;
    }
    return g_batteryFormatted.buffer;
}

PCWSTR GetBatteryTimeFormatted() {
    if (g_batteryTimeFormatted.formatIndex != g_formatIndex) {
        DWORD totalSeconds = 0;
        SYSTEM_POWER_STATUS ps;

        if (GetSystemPowerStatus(&ps)) {
            if (ps.BatteryLifeTime != (DWORD)-1) {
                // Discharging - use remaining time.
                totalSeconds = ps.BatteryLifeTime;
            } else if (ps.ACLineStatus == 1 && ps.BatteryLifePercent < 100) {
                // Charging - calculate time to full.
                SYSTEM_BATTERY_STATE bs{};
                NTSTATUS status = CallNtPowerInformation(
                    SystemBatteryState, nullptr, 0, &bs, sizeof(bs));
                if (status == 0 && bs.Rate > 0) {
                    DWORD remainingCapacity =
                        bs.MaxCapacity - bs.RemainingCapacity;
                    totalSeconds = (remainingCapacity * 3600) / bs.Rate;
                }
            }
        }

        DWORD hours = totalSeconds / 3600;
        DWORD minutes = (totalSeconds % 3600) / 60;
        swprintf_s(g_batteryTimeFormatted.buffer, L"%u:%02u", hours, minutes);
        g_batteryTimeFormatted.formatIndex = g_formatIndex;
    }
    return g_batteryTimeFormatted.buffer;
}

PCWSTR GetPowerFormatted() {
    if (g_powerFormatted.formatIndex != g_formatIndex) {
        SYSTEM_BATTERY_STATE batteryState{};
        NTSTATUS status =
            CallNtPowerInformation(SystemBatteryState, nullptr, 0,
                                   &batteryState, sizeof(batteryState));
        if (status == 0 && batteryState.MaxCapacity > 0) {
            DWORD rate = batteryState.Rate;

            // When some batteries charge the Rate is:
            // 0x80000000 == -2147483648 (LONG) == 2147483648 (DWORD)
            // https://github.com/jay/battstatus/blob/418d1872f6c4e560f6b46880d9577947f17cc414/battstatus.cpp#L265
            if (rate == 0x80000000) {
                rate = 0;
            }

            long powerMilliWatts = static_cast<long>(rate);
            long powerWatts =
                (powerMilliWatts + (powerMilliWatts >= 0 ? 500 : -500)) / 1000;

            swprintf_s(g_powerFormatted.buffer, L"%+ldW", powerWatts);
        } else {
            wcscpy_s(g_powerFormatted.buffer, L"-");
        }
        g_powerFormatted.formatIndex = g_formatIndex;
    }
    return g_powerFormatted.buffer;
}

void FormatTransferSpeed(double bytesPerSec, PWSTR buffer, size_t bufferSize) {
    constexpr double KB = 1024.0;
    constexpr double MB = 1024.0 * KB;

    // Use KB/s for values < 1 MB/s, otherwise MB/s.
    if (bytesPerSec < MB) {
        double kbps = bytesPerSec / KB;
        if (kbps < 10) {
            swprintf_s(buffer, bufferSize, L"%.2f KB/s", kbps);
        } else if (kbps < 100) {
            swprintf_s(buffer, bufferSize, L"%.1f KB/s", kbps);
        } else {
            swprintf_s(buffer, bufferSize, L"%.0f KB/s", kbps);
        }
    } else {
        double mbps = bytesPerSec / MB;
        if (mbps < 10) {
            swprintf_s(buffer, bufferSize, L"%.2f MB/s", mbps);
        } else if (mbps < 100) {
            swprintf_s(buffer, bufferSize, L"%.1f MB/s", mbps);
        } else {
            swprintf_s(buffer, bufferSize, L"%.0f MB/s", mbps);
        }
    }
}

double QueryNetworkSpeed(const std::vector<PDH_HCOUNTER>& counters) {
    double total = 0.0;
    for (PDH_HCOUNTER counter : counters) {
        PDH_FMT_COUNTERVALUE val;
        if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr,
                                        &val) == ERROR_SUCCESS) {
            total += val.doubleValue;
        }
    }
    return total;
}

PCWSTR GetUploadSpeedFormatted() {
    if (g_uploadSpeedFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery && !g_uploadCounters.empty()) {
            PdhCollectQueryData(g_metricsQuery);
            double speed = QueryNetworkSpeed(g_uploadCounters);
            FormatTransferSpeed(speed, g_uploadSpeedFormatted.buffer,
                                ARRAYSIZE(g_uploadSpeedFormatted.buffer));
        } else {
            wcscpy_s(g_uploadSpeedFormatted.buffer, L"-");
        }
        g_uploadSpeedFormatted.formatIndex = g_formatIndex;
    }
    return g_uploadSpeedFormatted.buffer;
}

PCWSTR GetDownloadSpeedFormatted() {
    if (g_downloadSpeedFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery && !g_downloadCounters.empty()) {
            PdhCollectQueryData(g_metricsQuery);
            double speed = QueryNetworkSpeed(g_downloadCounters);
            FormatTransferSpeed(speed, g_downloadSpeedFormatted.buffer,
                                ARRAYSIZE(g_downloadSpeedFormatted.buffer));
        } else {
            wcscpy_s(g_downloadSpeedFormatted.buffer, L"-");
        }
        g_downloadSpeedFormatted.formatIndex = g_formatIndex;
    }
    return g_downloadSpeedFormatted.buffer;
}

PCWSTR GetTotalSpeedFormatted() {
    if (g_totalSpeedFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery &&
            (!g_uploadCounters.empty() || !g_downloadCounters.empty())) {
            PdhCollectQueryData(g_metricsQuery);
            double uploadSpeed = QueryNetworkSpeed(g_uploadCounters);
            double downloadSpeed = QueryNetworkSpeed(g_downloadCounters);
            FormatTransferSpeed(uploadSpeed + downloadSpeed,
                                g_totalSpeedFormatted.buffer,
                                ARRAYSIZE(g_totalSpeedFormatted.buffer));
        } else {
            wcscpy_s(g_totalSpeedFormatted.buffer, L"-");
        }
        g_totalSpeedFormatted.formatIndex = g_formatIndex;
    }
    return g_totalSpeedFormatted.buffer;
}

double QueryDiskSpeed(PDH_HCOUNTER counter) {
    if (!counter) {
        return 0.0;
    }

    PDH_FMT_COUNTERVALUE counterVal;
    if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr,
                                    &counterVal) == ERROR_SUCCESS) {
        return counterVal.doubleValue;
    }
    return 0.0;
}

PCWSTR GetDiskReadSpeedFormatted() {
    if (g_diskReadSpeedFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery && g_diskReadCounter) {
            PdhCollectQueryData(g_metricsQuery);
            double speed = QueryDiskSpeed(g_diskReadCounter);
            FormatTransferSpeed(speed, g_diskReadSpeedFormatted.buffer,
                                ARRAYSIZE(g_diskReadSpeedFormatted.buffer));
        } else {
            wcscpy_s(g_diskReadSpeedFormatted.buffer, L"-");
        }
        g_diskReadSpeedFormatted.formatIndex = g_formatIndex;
    }
    return g_diskReadSpeedFormatted.buffer;
}

PCWSTR GetDiskWriteSpeedFormatted() {
    if (g_diskWriteSpeedFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery && g_diskWriteCounter) {
            PdhCollectQueryData(g_metricsQuery);
            double speed = QueryDiskSpeed(g_diskWriteCounter);
            FormatTransferSpeed(speed, g_diskWriteSpeedFormatted.buffer,
                                ARRAYSIZE(g_diskWriteSpeedFormatted.buffer));
        } else {
            wcscpy_s(g_diskWriteSpeedFormatted.buffer, L"-");
        }
        g_diskWriteSpeedFormatted.formatIndex = g_formatIndex;
    }
    return g_diskWriteSpeedFormatted.buffer;
}

PCWSTR GetDiskTotalSpeedFormatted() {
    if (g_diskTotalSpeedFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery && (g_diskReadCounter || g_diskWriteCounter)) {
            PdhCollectQueryData(g_metricsQuery);
            double readSpeed = QueryDiskSpeed(g_diskReadCounter);
            double writeSpeed = QueryDiskSpeed(g_diskWriteCounter);
            FormatTransferSpeed(readSpeed + writeSpeed,
                                g_diskTotalSpeedFormatted.buffer,
                                ARRAYSIZE(g_diskTotalSpeedFormatted.buffer));
        } else {
            wcscpy_s(g_diskTotalSpeedFormatted.buffer, L"-");
        }
        g_diskTotalSpeedFormatted.formatIndex = g_formatIndex;
    }
    return g_diskTotalSpeedFormatted.buffer;
}

double QueryGpuUsage() {
    double sum = 0.0;
    for (const auto& counter : g_gpuCounters) {
        PDH_FMT_COUNTERVALUE counterVal;
        if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr,
                                        &counterVal) == ERROR_SUCCESS) {
            sum += counterVal.doubleValue;
        }
    }
    return sum;
}

PCWSTR GetGpuFormatted() {
    if (g_gpuFormatted.formatIndex != g_formatIndex) {
        if (g_metricsQuery && !g_gpuCounters.empty()) {
            PdhCollectQueryData(g_metricsQuery);
            double usage = QueryGpuUsage();
            swprintf_s(g_gpuFormatted.buffer, L"%d", (int)usage);
        } else {
            wcscpy_s(g_gpuFormatted.buffer, L"-");
        }
        g_gpuFormatted.formatIndex = g_formatIndex;
    }
    return g_gpuFormatted.buffer;
}

// https://stackoverflow.com/a/39344961
DWORD GetStartDayOfWeek() {
    DWORD startDayOfWeek;
    GetLocaleInfoEx(
        LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK | LOCALE_RETURN_NUMBER,
        (PWSTR)&startDayOfWeek, sizeof(startDayOfWeek) / sizeof(WCHAR));

    // Start from Sunday instead of Monday.
    startDayOfWeek = (startDayOfWeek + 1) % 7;

    return startDayOfWeek;
}

int CalculateWeeknum(const SYSTEMTIME* time, DWORD startDayOfWeek) {
    SYSTEMTIME secondWeek{
        .wYear = time->wYear,
        .wMonth = 1,
        .wDay = 1,
    };

    // Calculate wDayOfWeek.
    FILETIME fileTime;
    SystemTimeToFileTime(&secondWeek, &fileTime);
    FileTimeToSystemTime(&fileTime, &secondWeek);

    do {
        secondWeek.wDay++;
        secondWeek.wDayOfWeek = (secondWeek.wDayOfWeek + 1) % 7;
    } while (secondWeek.wDayOfWeek != startDayOfWeek);

    FILETIME targetFileTime;
    SystemTimeToFileTime(time, &targetFileTime);
    ULARGE_INTEGER targetFileTimeInt{
        .LowPart = targetFileTime.dwLowDateTime,
        .HighPart = targetFileTime.dwHighDateTime,
    };

    FILETIME secondWeekFileTime;
    SystemTimeToFileTime(&secondWeek, &secondWeekFileTime);
    ULARGE_INTEGER secondWeekFileTimeInt{
        .LowPart = secondWeekFileTime.dwLowDateTime,
        .HighPart = secondWeekFileTime.dwHighDateTime,
    };

    int weeknum = 1;
    if (targetFileTimeInt.QuadPart >= secondWeekFileTimeInt.QuadPart) {
        ULONGLONG diff =
            targetFileTimeInt.QuadPart - secondWeekFileTimeInt.QuadPart;
        ULONGLONG weekIn100Ns = 10000000ULL * 60 * 60 * 24 * 7;
        weeknum += 1 + static_cast<int>(diff / weekIn100Ns);
    }

    return weeknum;
}

// Adopted from VMime:
// https://github.com/kisli/vmime/blob/fc69321d5304c73be685c890f3b30528aadcfeaf/src/vmime/utility/datetimeUtils.cpp#L239
int CalculateDayOfYearNumber(const SYSTEMTIME* time) {
    const int year = time->wYear;
    const int month = time->wMonth;
    const int day = time->wDay;

    const bool leapYear =
        ((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0;

    static const int DAY_OF_YEAR_NUMBER_MAP[12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    int DayOfYearNumber = day + DAY_OF_YEAR_NUMBER_MAP[month - 1];

    if (leapYear && month > 2) {
        DayOfYearNumber += 1;
    }

    return DayOfYearNumber;
}

// Adopted from:
// https://github.com/microsoft/cpp_client_telemetry/blob/25bc0806f21ecb2587154494f073bfa581cd5089/lib/pal/desktop/WindowsEnvironmentInfo.hpp#L39
void GetTimeZone(WCHAR* buffer, size_t bufferSize) {
    long bias;

    TIME_ZONE_INFORMATION timeZone = {};
    if (GetTimeZoneInformation(&timeZone) == TIME_ZONE_ID_DAYLIGHT) {
        bias = timeZone.Bias + timeZone.DaylightBias;
    } else {
        bias = timeZone.Bias + timeZone.StandardBias;
    }

    auto hours = (long long)abs(bias) / 60;
    auto minutes = (long long)abs(bias) % 60;

    // UTC = local time + bias; bias sign should be inverted.
    _snwprintf_s(buffer, bufferSize, _TRUNCATE, L"%c%02d:%02d",
                 bias <= 0 ? L'+' : L'-', static_cast<int>(hours),
                 static_cast<int>(minutes));
}

PCWSTR GetWeekdayNumFormatted() {
    if (g_weekdayNumFormatted.formatIndex != g_formatIndex) {
        DWORD startDayOfWeek = GetStartDayOfWeek();
        swprintf_s(g_weekdayNumFormatted.buffer, L"%d",
                   1 + (7 + g_formatTime.wDayOfWeek - startDayOfWeek) % 7);
        g_weekdayNumFormatted.formatIndex = g_formatIndex;
    }
    return g_weekdayNumFormatted.buffer;
}

PCWSTR GetWeeknumFormatted() {
    if (g_weeknumFormatted.formatIndex != g_formatIndex) {
        DWORD startDayOfWeek = GetStartDayOfWeek();
        swprintf_s(g_weeknumFormatted.buffer, L"%d",
                   CalculateWeeknum(&g_formatTime, startDayOfWeek));
        g_weeknumFormatted.formatIndex = g_formatIndex;
    }
    return g_weeknumFormatted.buffer;
}

PCWSTR GetDayOfYearFormatted() {
    if (g_dayOfYearFormatted.formatIndex != g_formatIndex) {
        swprintf_s(g_dayOfYearFormatted.buffer, L"%d",
                   CalculateDayOfYearNumber(&g_formatTime));
        g_dayOfYearFormatted.formatIndex = g_formatIndex;
    }
    return g_dayOfYearFormatted.buffer;
}

PCWSTR GetTimezoneFormatted() {
    if (g_timezoneFormatted.formatIndex != g_formatIndex) {
        GetTimeZone(g_timezoneFormatted.buffer,
                    ARRAYSIZE(g_timezoneFormatted.buffer));
        g_timezoneFormatted.formatIndex = g_formatIndex;
    }
    return g_timezoneFormatted.buffer;
}

// Returns the length of the format token consumed, or 0 if not a recognized
// token.
size_t ResolveFormatToken(
    std::wstring_view format,
    std::function<void(PCWSTR resolvedStr)> resolvedCallback) {
    struct TokenMapping {
        std::wstring_view token;
        PCWSTR (*getter)();
    };

    static const TokenMapping tokens[] = {
        {L"%time%"sv, GetTimeFormatted},
        {L"%date%"sv, GetDateFormatted},
        {L"%weekday%"sv, GetWeekdayFormatted},
        {L"%weekday_num%"sv, GetWeekdayNumFormatted},
        {L"%weeknum%"sv, GetWeeknumFormatted},
        {L"%dayofyear%"sv, GetDayOfYearFormatted},
        {L"%timezone%"sv, GetTimezoneFormatted},
        {L"%cpu%"sv, GetCpuFormatted},
        {L"%ram%"sv, GetRamFormatted},
        {L"%battery%"sv, GetBatteryFormatted},
        {L"%battery_time%"sv, GetBatteryTimeFormatted},
        {L"%power%"sv, GetPowerFormatted},
        {L"%upload_speed%"sv, GetUploadSpeedFormatted},
        {L"%download_speed%"sv, GetDownloadSpeedFormatted},
        {L"%total_speed%"sv, GetTotalSpeedFormatted},
        {L"%disk_read%"sv, GetDiskReadSpeedFormatted},
        {L"%disk_write%"sv, GetDiskWriteSpeedFormatted},
        {L"%disk_total%"sv, GetDiskTotalSpeedFormatted},
        {L"%gpu%"sv, GetGpuFormatted},
    };

    // Check for newline patterns first.
    if (format.starts_with(L"%newline%"sv)) {
        resolvedCallback(L"\n");
        return 9;  // length of "%newline%"
    }
    if (format.starts_with(L"%n%"sv)) {
        resolvedCallback(L"\n");
        return 3;  // length of "%n%"
    }

    // Check for weather pattern (requires mutex).
    if (auto token = L"%weather%"sv; format.starts_with(token)) {
        std::lock_guard<std::mutex> guard(g_weatherMutex);
        resolvedCallback(g_weatherContent ? g_weatherContent->c_str()
                                          : L"Loading...");
        return token.size();
    }

    // Check other tokens.
    for (const auto& t : tokens) {
        if (format.starts_with(t.token)) {
            resolvedCallback(t.getter());
            return t.token.size();
        }
    }

    return 0;  // Not a recognized token
}

int FormatLine(PWSTR buffer, size_t bufferSize, std::wstring_view format) {
    if (bufferSize == 0) {
        return 0;
    }

    std::wstring_view formatSuffix = format;
    PWSTR bufferStart = buffer;
    PWSTR bufferEnd = bufferStart + bufferSize;

    while (!formatSuffix.empty() && bufferEnd - buffer > 1) {
        if (formatSuffix[0] == L'%') {
            bool truncated = false;
            size_t formatTokenLen = ResolveFormatToken(
                formatSuffix,
                [&buffer, bufferEnd, &truncated](PCWSTR resolvedStr) {
                    buffer += StringCopyTruncated(buffer, bufferEnd - buffer,
                                                  resolvedStr, &truncated);
                });
            if (formatTokenLen > 0) {
                if (truncated) {
                    break;
                }
                formatSuffix = formatSuffix.substr(formatTokenLen);
                continue;
            }
        }

        *buffer++ = formatSuffix[0];
        formatSuffix = formatSuffix.substr(1);
    }

    // Add ellipsis if truncated.
    if (!formatSuffix.empty() && bufferSize >= 4) {
        buffer[-1] = L'.';
        buffer[-2] = L'.';
        buffer[-3] = L'.';
    }

    *buffer = L'\0';
    return static_cast<int>(buffer - bufferStart);
}

////////////////////////////////////////////////////////////////////////////////
// Desktop window detection

bool IsFolderViewWnd(HWND hWnd) {
    WCHAR buffer[64];

    if (!GetClassName(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SysListView32")) {
        return false;
    }

    if (!GetWindowText(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"FolderView")) {
        return false;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (!hParentWnd) {
        return false;
    }

    if (!GetClassName(hParentWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SHELLDLL_DefView")) {
        return false;
    }

    if (GetWindowTextLength(hParentWnd) > 0) {
        return false;
    }

    HWND hParentWnd2 = GetAncestor(hParentWnd, GA_PARENT);
    if (!hParentWnd2) {
        return false;
    }

    if ((!GetClassName(hParentWnd2, buffer, ARRAYSIZE(buffer)) ||
         _wcsicmp(buffer, L"Progman")) &&
        hParentWnd2 != GetShellWindow()) {
        return false;
    }

    return true;
}

// Find the WorkerW window behind desktop icons.
// Based on weebp: https://github.com/Francesco149/weebp
HWND GetWorkerW() {
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (!hProgman) {
        return nullptr;
    }

    // Send undocumented message to spawn WorkerW windows.
    SendMessage(hProgman, 0x052C, 0xD, 0);
    SendMessage(hProgman, 0x052C, 0xD, 1);

    // Find window with SHELLDLL_DefView, then get the next WorkerW sibling.
    HWND hWorkerW = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
                return TRUE;
            }
            HWND hWorker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
            if (hWorker) {
                *(HWND*)lParam = hWorker;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&hWorkerW);

    // Fallback with alternative message parameters.
    if (!hWorkerW) {
        SendMessage(hProgman, 0x052C, 0, 0);
        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView",
                                  nullptr)) {
                    return TRUE;
                }
                HWND hWorker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
                if (hWorker) {
                    *(HWND*)lParam = hWorker;
                    return FALSE;
                }
                return TRUE;
            },
            (LPARAM)&hWorkerW);
    }

    // Fallback: WorkerW as child of Progman.
    if (!hWorkerW) {
        hWorkerW = FindWindowEx(hProgman, nullptr, L"WorkerW", nullptr);
    }

    // Final fallback: use Progman itself.
    if (!hWorkerW) {
        hWorkerW = hProgman;
    }

    return hWorkerW;
}

////////////////////////////////////////////////////////////////////////////////
// DirectX initialization

bool InitDirectX() {
    HRESULT hr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                           nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr,
                           nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: 0x%08X", hr);
        return false;
    }

    hr = g_d3dDevice.As(&g_dxgiDevice);
    if (FAILED(hr)) {
        Wh_Log(L"QueryInterface IDXGIDevice failed: 0x%08X", hr);
        return false;
    }

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_dxgiFactory));
    if (FAILED(hr)) {
        Wh_Log(L"CreateDXGIFactory2 failed: 0x%08X", hr);
        return false;
    }

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                           IID_PPV_ARGS(&g_d2dFactory));
    if (FAILED(hr)) {
        Wh_Log(L"D2D1CreateFactory failed: 0x%08X", hr);
        return false;
    }

    hr = g_d2dFactory->CreateDevice(g_dxgiDevice.Get(), &g_d2dDevice);
    if (FAILED(hr)) {
        Wh_Log(L"D2D CreateDevice failed: 0x%08X", hr);
        return false;
    }

    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(g_dwriteFactory.GetAddressOf()));
    if (FAILED(hr)) {
        Wh_Log(L"DWriteCreateFactory failed: 0x%08X", hr);
        return false;
    }

    return true;
}

void UninitDirectX() {
    g_dwriteFactory.Reset();
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
}

////////////////////////////////////////////////////////////////////////////////
// Overlay rendering

bool CreateSwapChainResources(UINT width, UINT height) {
    HRESULT hr;

    // Create swap chain for composition with premultiplied alpha.
    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = width;
    scd.Height = height;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    hr = g_dxgiFactory->CreateSwapChainForComposition(g_dxgiDevice.Get(), &scd,
                                                      nullptr, &g_swapChain);
    if (FAILED(hr)) {
        Wh_Log(L"CreateSwapChainForComposition failed: 0x%08X", hr);
        return false;
    }

    // Create D2D device context.
    hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                          &g_dc);
    if (FAILED(hr)) {
        Wh_Log(L"CreateDeviceContext failed: 0x%08X", hr);
        return false;
    }

    // Create bitmap target from swap chain surface.
    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) {
        Wh_Log(L"GetBuffer failed: 0x%08X", hr);
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties,
                                           &targetBitmap);
    if (FAILED(hr)) {
        Wh_Log(L"CreateBitmapFromDxgiSurface failed: 0x%08X", hr);
        return false;
    }

    g_dc->SetTarget(targetBitmap.Get());

    // Create DirectComposition device and visual tree.
    hr = DCompositionCreateDevice(g_dxgiDevice.Get(),
                                  IID_PPV_ARGS(&g_compositionDevice));
    if (FAILED(hr)) {
        Wh_Log(L"DCompositionCreateDevice failed: 0x%08X", hr);
        return false;
    }

    hr = g_compositionDevice->CreateTargetForHwnd(g_overlayWnd, TRUE,
                                                  &g_compositionTarget);
    if (FAILED(hr)) {
        Wh_Log(L"CreateTargetForHwnd failed: 0x%08X", hr);
        return false;
    }

    hr = g_compositionDevice->CreateVisual(&g_compositionVisual);
    if (FAILED(hr)) {
        Wh_Log(L"CreateVisual failed: 0x%08X", hr);
        return false;
    }

    hr = g_compositionVisual->SetContent(g_swapChain.Get());
    if (FAILED(hr)) {
        Wh_Log(L"SetContent failed: 0x%08X", hr);
        return false;
    }

    hr = g_compositionTarget->SetRoot(g_compositionVisual.Get());
    if (FAILED(hr)) {
        Wh_Log(L"SetRoot failed: 0x%08X", hr);
        return false;
    }

    hr = g_compositionDevice->Commit();
    if (FAILED(hr)) {
        Wh_Log(L"Commit failed: 0x%08X", hr);
        return false;
    }

    // Get DPI scale for the selected monitor.
    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) {
        monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    }
    g_dpiScale = GetMonitorDpiScale(monitor);
    Wh_Log(L"DPI scale: %.2f", g_dpiScale);

    // Create top line text format.
    PCWSTR topFontFamily = g_settings.topLine.fontFamily.get();
    if (!topFontFamily || !*topFontFamily) {
        topFontFamily = L"Segoe UI";
    }

    hr = g_dwriteFactory->CreateTextFormat(
        topFontFamily, nullptr,
        GetDWriteFontWeight(g_settings.topLine.fontWeight),
        g_settings.topLine.fontItalic ? DWRITE_FONT_STYLE_ITALIC
                                      : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (FLOAT)g_settings.topLine.fontSize * g_dpiScale, L"",
        &g_topLineTextFormat);
    if (FAILED(hr)) {
        Wh_Log(L"CreateTextFormat (top) failed: 0x%08X", hr);
        return false;
    }

    g_topLineTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    // Create top line text brush.
    D2D1_COLOR_F topTextColor = D2D1::ColorF(
        g_settings.topLine.colorR / 255.0f, g_settings.topLine.colorG / 255.0f,
        g_settings.topLine.colorB / 255.0f, 1.0f);
    hr = g_dc->CreateSolidColorBrush(topTextColor, &g_topLineTextBrush);
    if (FAILED(hr)) {
        Wh_Log(L"CreateSolidColorBrush (top) failed: 0x%08X", hr);
        return false;
    }

    // Create bottom line text format.
    PCWSTR bottomFontFamily = g_settings.bottomLine.fontFamily.get();
    if (!bottomFontFamily || !*bottomFontFamily) {
        bottomFontFamily = L"Segoe UI";
    }

    hr = g_dwriteFactory->CreateTextFormat(
        bottomFontFamily, nullptr,
        GetDWriteFontWeight(g_settings.bottomLine.fontWeight),
        g_settings.bottomLine.fontItalic ? DWRITE_FONT_STYLE_ITALIC
                                         : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (FLOAT)g_settings.bottomLine.fontSize * g_dpiScale, L"",
        &g_bottomLineTextFormat);
    if (FAILED(hr)) {
        Wh_Log(L"CreateTextFormat (bottom) failed: 0x%08X", hr);
        return false;
    }

    g_bottomLineTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    // Create bottom line text brush.
    D2D1_COLOR_F bottomTextColor =
        D2D1::ColorF(g_settings.bottomLine.colorR / 255.0f,
                     g_settings.bottomLine.colorG / 255.0f,
                     g_settings.bottomLine.colorB / 255.0f, 1.0f);
    hr = g_dc->CreateSolidColorBrush(bottomTextColor, &g_bottomLineTextBrush);
    if (FAILED(hr)) {
        Wh_Log(L"CreateSolidColorBrush (bottom) failed: 0x%08X", hr);
        return false;
    }

    // Create background brush.
    if (g_settings.backgroundEnabled) {
        D2D1_COLOR_F backgroundColor =
            D2D1::ColorF(g_settings.backgroundColorR / 255.0f,
                         g_settings.backgroundColorG / 255.0f,
                         g_settings.backgroundColorB / 255.0f,
                         g_settings.backgroundColorA / 255.0f);
        hr = g_dc->CreateSolidColorBrush(backgroundColor, &g_backgroundBrush);
        if (FAILED(hr)) {
            Wh_Log(L"CreateSolidColorBrush (background) failed: 0x%08X", hr);
            return false;
        }
    }

    return true;
}

void ReleaseSwapChainResources() {
    g_backgroundBrush.Reset();
    g_bottomLineTextBrush.Reset();
    g_bottomLineTextFormat.Reset();
    g_topLineTextBrush.Reset();
    g_topLineTextFormat.Reset();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
    g_dc.Reset();
    g_swapChain.Reset();
}

bool ResizeSwapChain(UINT width, UINT height) {
    if (!g_swapChain || !g_dc) {
        return false;
    }

    Wh_Log(L"ResizeSwapChain: %ux%u", width, height);

    // Release the current render target.
    g_dc->SetTarget(nullptr);

    // Resize swap chain buffers.
    HRESULT hr =
        g_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        Wh_Log(L"ResizeBuffers failed: 0x%08X", hr);
        return false;
    }

    // Recreate bitmap target from resized swap chain surface.
    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) {
        Wh_Log(L"GetBuffer failed: 0x%08X", hr);
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties,
                                           &targetBitmap);
    if (FAILED(hr)) {
        Wh_Log(L"CreateBitmapFromDxgiSurface failed: 0x%08X", hr);
        return false;
    }

    g_dc->SetTarget(targetBitmap.Get());
    return true;
}

void RenderOverlay() {
    Wh_Log(L"RenderOverlay called");

    if (g_unloading || !g_dc || !g_swapChain || !g_dwriteFactory) {
        Wh_Log(L"RenderOverlay: resources not available or unloading");
        return;
    }

    RECT rc;
    GetClientRect(g_overlayWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    // Update format time.
    GetLocalTime(&g_formatTime);
    g_formatIndex++;

    // Format both lines.
    PCWSTR rawTopText = g_settings.topLine.text.get();
    WCHAR formattedTopText[1024] = {};
    if (rawTopText && *rawTopText) {
        FormatLine(formattedTopText, ARRAYSIZE(formattedTopText), rawTopText);
    }

    PCWSTR rawBottomText = g_settings.bottomLine.text.get();
    WCHAR formattedBottomText[1024] = {};
    if (rawBottomText && *rawBottomText) {
        FormatLine(formattedBottomText, ARRAYSIZE(formattedBottomText),
                   rawBottomText);
    }

    bool hasTopLine = *formattedTopText && g_topLineTextFormat;
    bool hasBottomLine = *formattedBottomText && g_bottomLineTextFormat;

    if (hasTopLine || hasBottomLine) {
        HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
        if (!monitor) {
            monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
        }

        MONITORINFO monitorInfo{.cbSize = sizeof(monitorInfo)};
        if (GetMonitorInfo(monitor, &monitorInfo)) {
            // Convert from virtual screen coordinates to overlay window
            // coordinates. The overlay window covers the entire virtual screen,
            // so we need to offset by the virtual screen origin.
            int virtualScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
            int virtualScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);

            RECT workArea;
            workArea.left = monitorInfo.rcWork.left - virtualScreenX;
            workArea.top = monitorInfo.rcWork.top - virtualScreenY;
            workArea.right = monitorInfo.rcWork.right - virtualScreenX;
            workArea.bottom = monitorInfo.rcWork.bottom - virtualScreenY;

            // Create text layouts for both lines.
            ComPtr<IDWriteTextLayout> topLayout;
            ComPtr<IDWriteTextLayout> bottomLayout;
            float topWidth = 0, topHeight = 0;
            float bottomWidth = 0, bottomHeight = 0;

            if (hasTopLine) {
                g_dwriteFactory->CreateTextLayout(
                    formattedTopText, (UINT32)wcslen(formattedTopText),
                    g_topLineTextFormat.Get(), (FLOAT)width, (FLOAT)height,
                    &topLayout);
                if (topLayout) {
                    DWRITE_TEXT_METRICS metrics;
                    topLayout->GetMetrics(&metrics);
                    topWidth = metrics.width;
                    topHeight = metrics.height;
                    topLayout->SetMaxWidth(topWidth);
                }
            }

            if (hasBottomLine) {
                g_dwriteFactory->CreateTextLayout(
                    formattedBottomText, (UINT32)wcslen(formattedBottomText),
                    g_bottomLineTextFormat.Get(), (FLOAT)width, (FLOAT)height,
                    &bottomLayout);
                if (bottomLayout) {
                    DWRITE_TEXT_METRICS metrics;
                    bottomLayout->GetMetrics(&metrics);
                    bottomWidth = metrics.width;
                    bottomHeight = metrics.height;
                    bottomLayout->SetMaxWidth(bottomWidth);
                }
            }

            // Calculate combined dimensions.
            float totalWidth = std::max(topWidth, bottomWidth);
            float totalHeight = topHeight + bottomHeight;
            float workWidth = (float)(workArea.right - workArea.left);
            float workHeight = (float)(workArea.bottom - workArea.top);

            // Calculate position for the combined block.
            float blockX =
                workArea.left + (workWidth - totalWidth) *
                                    (g_settings.horizontalPosition / 100.0f);
            float blockY =
                workArea.top + (workHeight - totalHeight) *
                                   (g_settings.verticalPosition / 100.0f);

            // Draw background if enabled.
            if (g_backgroundBrush) {
                float padding =
                    (float)g_settings.backgroundPadding * g_dpiScale;
                float radius =
                    (float)g_settings.backgroundCornerRadius * g_dpiScale;
                D2D1_ROUNDED_RECT backgroundRect = D2D1::RoundedRect(
                    D2D1::RectF(blockX - padding, blockY - padding,
                                blockX + totalWidth + padding,
                                blockY + totalHeight + padding),
                    radius, radius);
                g_dc->FillRoundedRectangle(backgroundRect,
                                           g_backgroundBrush.Get());
            }

            // Draw top line.
            if (topLayout) {
                float topX = blockX + (totalWidth - topWidth) / 2.0f;
                float topY = blockY;

                float opacity = g_settings.topLine.colorA / 255.0f;
                g_dc->PushLayer(
                    D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                          D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                          D2D1::IdentityMatrix(), opacity),
                    nullptr);

                g_dc->DrawTextLayout(D2D1::Point2F(topX, topY), topLayout.Get(),
                                     g_topLineTextBrush.Get(),
                                     D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

                g_dc->PopLayer();
            }

            // Draw bottom line.
            if (bottomLayout) {
                float bottomX = blockX + (totalWidth - bottomWidth) / 2.0f;
                float bottomY = blockY + topHeight;

                float opacity = g_settings.bottomLine.colorA / 255.0f;
                g_dc->PushLayer(
                    D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                                          D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                          D2D1::IdentityMatrix(), opacity),
                    nullptr);

                g_dc->DrawTextLayout(D2D1::Point2F(bottomX, bottomY),
                                     bottomLayout.Get(),
                                     g_bottomLineTextBrush.Get(),
                                     D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

                g_dc->PopLayer();
            }
        }
    }

    g_dc->EndDraw();
    g_swapChain->Present(1, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Refresh timer

UINT GetNextUpdateTimeout() {
    // Determine if we need per-second updates.
    bool needsSecondsUpdate =
        g_settings.showSeconds || g_systemMetricsUsed || !g_weatherLoaded;

    // Get current time for alignment calculations.
    SYSTEMTIME time;
    GetLocalTime(&time);

    int refreshInterval =
        (std::max)(1, (std::min)(60, g_settings.refreshInterval));

    if (needsSecondsUpdate) {
        if (refreshInterval == 1) {
            // Align to the next second boundary.
            return 1000 - time.wMilliseconds;
        } else {
            // Calculate time until next interval boundary.
            int currentSecondInInterval = time.wSecond % refreshInterval;
            int secondsUntilNext = refreshInterval - currentSecondInInterval;
            if (secondsUntilNext == refreshInterval) {
                // We're at the boundary, wait for next interval.
                secondsUntilNext = refreshInterval;
            }
            return secondsUntilNext * 1000 - time.wMilliseconds;
        }
    } else {
        // No seconds or system metrics - refresh every minute.
        // Align to the next minute boundary.
        return (60 - time.wSecond) * 1000 - time.wMilliseconds;
    }
}

void ScheduleNextUpdate() {
    if (!g_overlayWnd) {
        return;
    }

    UINT timeout = GetNextUpdateTimeout();
    SetTimer(g_overlayWnd, TIMER_ID_REFRESH, timeout, nullptr);
}

void StopRefreshTimer() {
    if (g_overlayWnd) {
        KillTimer(g_overlayWnd, TIMER_ID_REFRESH);
    }
}

void HandleDisplayChange() {
    Wh_Log(L"HandleDisplayChange");

    if (!g_overlayWnd) {
        return;
    }

    HWND hWorkerW = GetParent(g_overlayWnd);
    if (!hWorkerW) {
        return;
    }

    // Resize overlay window to match WorkerW.
    RECT rc;
    GetWindowRect(hWorkerW, &rc);
    SetWindowPos(g_overlayWnd, nullptr, 0, 0, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);

    // Check if DPI changed and recreate resources if needed.
    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) {
        monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    }
    float newDpiScale = GetMonitorDpiScale(monitor);
    if (newDpiScale != g_dpiScale) {
        Wh_Log(L"DPI changed: %.2f -> %.2f", g_dpiScale, newDpiScale);
        ReleaseSwapChainResources();
        GetClientRect(g_overlayWnd, &rc);
        CreateSwapChainResources(rc.right - rc.left, rc.bottom - rc.top);
        RenderOverlay();
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hWnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (uMsg) {
        case WM_TIMER:
            if (!g_unloading && wParam == TIMER_ID_REFRESH) {
                RenderOverlay();
                ScheduleNextUpdate();
                return 0;
            }
            break;

        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE) && !g_unloading) {
                Wh_Log(L"WM_WINDOWPOSCHANGED: %dx%d", wp->cx, wp->cy);
                ResizeSwapChain(wp->cx, wp->cy);
                RenderOverlay();
            }
            break;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#define TIMER_ID_DISPLAY_CHANGE 1

LRESULT CALLBACK MessageWndProc(HWND hWnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (uMsg) {
        case WM_DISPLAYCHANGE:
            Wh_Log(L"WM_DISPLAYCHANGE received");
            if (!g_unloading) {
                // Delay handling to allow WorkerW to resize first.
                SetTimer(hWnd, TIMER_ID_DISPLAY_CHANGE, 200, nullptr);
            }
            return 0;

        case WM_TIMER:
            if (wParam == TIMER_ID_DISPLAY_CHANGE && !g_unloading) {
                KillTimer(hWnd, TIMER_ID_DISPLAY_CHANGE);
                HandleDisplayChange();
            }
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Overlay window management

void CreateOverlayWindow() {
    HWND hWorkerW = GetWorkerW();
    if (!hWorkerW) {
        Wh_Log(L"Failed to find WorkerW");
        return;
    }

    HINSTANCE hInstance = GetCurrentModuleHandle();

    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = OVERLAY_WINDOW_CLASS;
    RegisterClass(&wc);

    RECT rc;
    GetWindowRect(hWorkerW, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    g_overlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName, nullptr, WS_CHILD | WS_VISIBLE, 0, 0, width, height,
        hWorkerW, nullptr, wc.hInstance, nullptr);

    if (!g_overlayWnd) {
        Wh_Log(L"Failed to create overlay window: %u", GetLastError());
        UnregisterClass(OVERLAY_WINDOW_CLASS, hInstance);
        return;
    }

    if (CreateSwapChainResources(width, height)) {
        RenderOverlay();
        ScheduleNextUpdate();
    }
}

void DestroyOverlayWindow() {
    StopRefreshTimer();
    ReleaseSwapChainResources();

    if (g_overlayWnd) {
        DestroyWindow(g_overlayWnd);
        g_overlayWnd = nullptr;
    }

    UnregisterClass(OVERLAY_WINDOW_CLASS, GetCurrentModuleHandle());
}

////////////////////////////////////////////////////////////////////////////////
// Message window for system notifications

#define MESSAGE_WINDOW_CLASS L"DesktopLiveOverlay_Message_" WH_MOD_ID

void CreateMessageWindow() {
    HINSTANCE hInstance = GetCurrentModuleHandle();

    WNDCLASS wc = {};
    wc.lpfnWndProc = MessageWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = MESSAGE_WINDOW_CLASS;
    RegisterClass(&wc);

    // Create a hidden top-level window (not message-only) to receive
    // WM_DISPLAYCHANGE which is only sent to top-level windows.
    g_messageWnd = CreateWindowEx(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0,
                                  nullptr, nullptr, hInstance, nullptr);
    if (!g_messageWnd) {
        Wh_Log(L"Failed to create message window: %u", GetLastError());
        UnregisterClass(MESSAGE_WINDOW_CLASS, hInstance);
    }
}

void DestroyMessageWindow() {
    if (g_messageWnd) {
        DestroyWindow(g_messageWnd);
        g_messageWnd = nullptr;
    }

    UnregisterClass(MESSAGE_WINDOW_CLASS, GetCurrentModuleHandle());
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !IsFolderViewWnd(hWnd)) {
        return hWnd;
    }

    Wh_Log(L"FolderView window created");

    // Delay overlay creation to let the desktop fully initialize.
    static UINT_PTR s_timer = 0;
    s_timer = SetTimer(nullptr, s_timer, 1000,
                       [](HWND, UINT, UINT_PTR idEvent, DWORD) {
                           KillTimer(nullptr, idEvent);
                           CreateOverlayWindow();
                           CreateMessageWindow();
                       });

    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

void LoadLineSettings(LineSettings& line, PCWSTR prefix, int defaultFontSize) {
    WCHAR settingName[64];

    swprintf_s(settingName, L"%s.text", prefix);
    line.text = WindhawkUtils::StringSetting::make(settingName);

    swprintf_s(settingName, L"%s.fontSize", prefix);
    line.fontSize = Wh_GetIntSetting(settingName);
    if (line.fontSize <= 0) {
        line.fontSize = defaultFontSize;
    }

    swprintf_s(settingName, L"%s.textColor", prefix);
    PCWSTR textColor = Wh_GetStringSetting(settingName);
    if (!ParseColor(textColor, &line.colorA, &line.colorR, &line.colorG,
                    &line.colorB)) {
        line.colorA = 0xC0;
        line.colorR = 0xFF;
        line.colorG = 0xFF;
        line.colorB = 0xFF;
    }
    Wh_FreeStringSetting(textColor);

    swprintf_s(settingName, L"%s.fontFamily", prefix);
    line.fontFamily = WindhawkUtils::StringSetting::make(settingName);

    swprintf_s(settingName, L"%s.fontWeight", prefix);
    PCWSTR fontWeight = Wh_GetStringSetting(settingName);
    line.fontWeight = 400;
    if (wcscmp(fontWeight, L"Thin") == 0) {
        line.fontWeight = 100;
    } else if (wcscmp(fontWeight, L"Light") == 0) {
        line.fontWeight = 300;
    } else if (wcscmp(fontWeight, L"Normal") == 0) {
        line.fontWeight = 400;
    } else if (wcscmp(fontWeight, L"Medium") == 0) {
        line.fontWeight = 500;
    } else if (wcscmp(fontWeight, L"SemiBold") == 0) {
        line.fontWeight = 600;
    } else if (wcscmp(fontWeight, L"Bold") == 0) {
        line.fontWeight = 700;
    } else if (wcscmp(fontWeight, L"ExtraBold") == 0) {
        line.fontWeight = 800;
    }
    Wh_FreeStringSetting(fontWeight);

    swprintf_s(settingName, L"%s.fontStyle", prefix);
    PCWSTR fontStyle = Wh_GetStringSetting(settingName);
    line.fontItalic = wcscmp(fontStyle, L"Italic") == 0;
    Wh_FreeStringSetting(fontStyle);
}

void LoadSettings() {
    LoadLineSettings(g_settings.topLine, L"topLine", 48);
    LoadLineSettings(g_settings.bottomLine, L"bottomLine", 32);

    g_settings.showSeconds = Wh_GetIntSetting(L"showSeconds");
    g_settings.timeFormat = WindhawkUtils::StringSetting::make(L"timeFormat");
    g_settings.dateFormat = WindhawkUtils::StringSetting::make(L"dateFormat");

    g_settings.refreshInterval = Wh_GetIntSetting(L"refreshInterval");
    if (g_settings.refreshInterval <= 0) {
        g_settings.refreshInterval = 1;
    }

    g_settings.backgroundEnabled = Wh_GetIntSetting(L"background.enabled");

    PCWSTR backgroundColor = Wh_GetStringSetting(L"background.color");
    if (!ParseColor(backgroundColor, &g_settings.backgroundColorA,
                    &g_settings.backgroundColorR, &g_settings.backgroundColorG,
                    &g_settings.backgroundColorB)) {
        g_settings.backgroundColorA = 0x80;
        g_settings.backgroundColorR = 0x00;
        g_settings.backgroundColorG = 0x00;
        g_settings.backgroundColorB = 0x00;
    }
    Wh_FreeStringSetting(backgroundColor);

    g_settings.backgroundPadding = Wh_GetIntSetting(L"background.padding");
    if (g_settings.backgroundPadding < 0) {
        g_settings.backgroundPadding = 10;
    }

    g_settings.backgroundCornerRadius =
        Wh_GetIntSetting(L"background.cornerRadius");
    if (g_settings.backgroundCornerRadius < 0) {
        g_settings.backgroundCornerRadius = 8;
    }

    g_settings.verticalPosition = Wh_GetIntSetting(L"verticalPosition");
    g_settings.horizontalPosition = Wh_GetIntSetting(L"horizontalPosition");

    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    if (g_settings.monitor <= 0) {
        g_settings.monitor = 1;
    }

    g_settings.weatherLocation =
        WindhawkUtils::StringSetting::make(L"weatherLocation");
    g_settings.weatherFormat =
        WindhawkUtils::StringSetting::make(L"weatherFormat");

    PCWSTR weatherUnits = Wh_GetStringSetting(L"weatherUnits");
    g_settings.weatherUnits = WeatherUnits::autoDetect;
    if (wcscmp(weatherUnits, L"uscs") == 0) {
        g_settings.weatherUnits = WeatherUnits::uscs;
    } else if (wcscmp(weatherUnits, L"metric") == 0) {
        g_settings.weatherUnits = WeatherUnits::metric;
    } else if (wcscmp(weatherUnits, L"metricMsWind") == 0) {
        g_settings.weatherUnits = WeatherUnits::metricMsWind;
    }
    Wh_FreeStringSetting(weatherUnits);
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    g_systemMetricsUsed = IsSystemMetricsUsed();

    if (!InitDirectX()) {
        Wh_Log(L"InitDirectX failed");
        return FALSE;
    }

    InitMetrics();
    WeatherUpdateThreadInit();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hWorkerW = GetWorkerW();
    if (hWorkerW) {
        RunFromWindowThread(
            hWorkerW,
            [](void*) {
                CreateOverlayWindow();
                CreateMessageWindow();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_overlayWnd) {
        RunFromWindowThread(
            g_overlayWnd,
            [](void*) {
                DestroyOverlayWindow();
                DestroyMessageWindow();
            },
            nullptr);
    }

    WeatherUpdateThreadUninit();
    UninitMetrics();
    UninitDirectX();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
