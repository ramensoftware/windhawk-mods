// ==WindhawkMod==
// @id              desktop-live-overlay
// @name            Desktop Live Overlay
// @description     Display live, customizable content on the desktop behind icons. Perfect for showing time, date, system metrics, weather, and more.
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldxgi -ld2d1 -ldwrite -ld3d11 -ldcomp -ldwmapi -lgdi32 -lwininet -lpdh -lpowrprof -lshcore -lshlwapi
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
* `%cpu_temp%` - CPU temperature in °C (average of all ACPI thermal zones)
* `%cpu_temp_f%` - CPU temperature in °F (average of all ACPI thermal zones)
* `%ram%` - RAM usage percentage
* `%ram_used%` - Used RAM amount in GB
* `%ram_total%` - Total RAM amount in GB
* `%ram_committed%` - Committed RAM usage percentage (memory committed by apps,
  backed by RAM or the page file)
* `%ram_committed_used%` - Used committed RAM amount in GB
* `%ram_committed_total%` - Total committed RAM amount in GB
* `%battery%` - Battery percentage
* `%battery_time%` - battery time remaining (charging time left / discharging
  time left, in h:mm format). If the value is always zero, you might need to
  enable battery time reporting in the BIOS.
* `%power%` - battery power in watts (negative when discharging, positive when
  charging). If the value is always zero, you might need to enable battery time
  reporting in the BIOS.
* `%upload_speed%` - Network upload speed
* `%download_speed%` - Network download speed
* `%total_speed%` - Combined network speed
* `%disk_read%` - Disk read speed
* `%disk_write%` - Disk write speed
* `%disk_total%` - Combined disk I/O speed
* `%gpu%` - GPU usage percentage
* `%gpu_temp%` - GPU temperature in °C
* `%gpu_temp_f%` - GPU temperature in °F
* `%vram%` - VRAM usage as a percentage of total dedicated VRAM
* `%vram_used%` - Used dedicated VRAM amount in GB
* `%vram_total%` - Total dedicated VRAM amount in GB
* `%vram_shared%` - Shared VRAM usage as a percentage of total shared VRAM
  (system RAM used as extra GPU memory)
* `%vram_shared_used%` - Used shared VRAM amount in GB
* `%vram_shared_total%` - Total shared VRAM pool size in GB
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
- gpuAdapterName: ""
  $name: GPU adapter name
  $description: >-
    The GPU adapter to use for the GPU usage, VRAM and temperature patterns.
    Leave empty to auto-detect (uses the adapter with the most dedicated VRAM).
    Partial match is supported. To list adapters, run:

    wmic path win32_videocontroller get Name
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
  - blur: 0
    $name: Blur
    $description: >-
      Blur radius for the wallpaper behind the background area. Set to 0 to
      disable.
  - borderSize: 0
    $name: Border size
    $description: Border width in pixels. Set to 0 to disable.
  - borderColor: "#C0FFFFFF"
    $name: Border color
    $description: >-
      Border color in ARGB hex format. Default is semi-transparent white.
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

#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
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
#include <unordered_set>
#include <vector>

using namespace std::literals;
using Microsoft::WRL::ComPtr;

// Overlay window timer IDs.
#define TIMER_ID_OVERLAY_REFRESH 1
#define TIMER_ID_WALLPAPER_REFRESH 2

// Message window timer IDs.
#define TIMER_ID_MSG_DISPLAY_CHANGE 1
#define TIMER_ID_MSG_RECREATE_OVERLAY 2
#define TIMER_ID_MSG_WALLPAPER_REFRESH 3

#define WM_APP_CLEANUP (WM_APP + 1)
#define WM_APP_SETTINGS_CHANGED (WM_APP + 2)
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
    WindhawkUtils::StringSetting gpuAdapterName;
    bool backgroundEnabled;
    BYTE backgroundColorA;
    BYTE backgroundColorR;
    BYTE backgroundColorG;
    BYTE backgroundColorB;
    int backgroundPadding;
    int backgroundCornerRadius;
    int backgroundBlur;
    int backgroundBorderSize;
    BYTE backgroundBorderColorA;
    BYTE backgroundBorderColorR;
    BYTE backgroundBorderColorG;
    BYTE backgroundBorderColorB;
    int verticalPosition;
    int horizontalPosition;
    int monitor;
    WindhawkUtils::StringSetting weatherLocation;
    WindhawkUtils::StringSetting weatherFormat;
    WeatherUnits weatherUnits;
};

constexpr size_t FORMATTED_BUFFER_SIZE = 256;
constexpr size_t INTEGER_BUFFER_SIZE = sizeof("-2147483648");
constexpr double kGBInBytes = 1024.0 * 1024.0 * 1024.0;

template <size_t N>
struct FormattedString {
    DWORD formatIndex = 0;
    WCHAR buffer[N] = {};
};

////////////////////////////////////////////////////////////////////////////////
// Globals

Settings g_settings;

// Initialization/unloading state.
std::atomic<bool> g_lazyInitialized{false};
std::atomic<bool> g_initSucceeded{false};
std::atomic<bool> g_unloading{false};

// Format state.
std::mutex g_formatLineMutex;
bool g_formattingInitialized = false;
SYSTEMTIME g_formatTime;
DWORD g_formatIndex = 0;
DWORD g_metricsLastFormatIndex = 0;

FormattedString<FORMATTED_BUFFER_SIZE> g_timeFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_dateFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_weekdayFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weekdayNumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weeknumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_dayOfYearFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_timezoneFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_cpuFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_ramFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_ramUsedFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_ramTotalFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_ramCommittedFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_ramCommittedUsedFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_ramCommittedTotalFormatted;
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
FormattedString<INTEGER_BUFFER_SIZE> g_vramFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_vramUsedFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_vramTotalFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_vramSharedFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_vramSharedUsedFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_vramSharedTotalFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_cpuTempFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_cpuTempFFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_gpuTempFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_gpuTempFFormatted;

// Performance metrics.
PDH_HQUERY g_metricsQuery = nullptr;
PDH_HCOUNTER g_cpuCounter = nullptr;
PDH_HCOUNTER g_diskReadCounter = nullptr;
PDH_HCOUNTER g_diskWriteCounter = nullptr;

struct CounterEntry {
    std::wstring path;
    PDH_HCOUNTER counter;
};

struct WildcardMetric {
    std::vector<CounterEntry> counters;
    PCWSTR wildcardPath = nullptr;  // English wildcard path for re-expansion.
    // Substring the expanded paths must contain, empty to accept all.
    std::wstring pathFilter;
    // Counter values below this are ignored.
    double minValue = 0;
};

WildcardMetric g_uploadMetric;
WildcardMetric g_downloadMetric;
WildcardMetric g_gpuMetric;
WildcardMetric g_vramMetric;
WildcardMetric g_vramSharedMetric;
WildcardMetric g_cpuTempMetric;

// Weather web content.
HANDLE g_weatherUpdateThread = nullptr;
HANDLE g_weatherUpdateStopEvent = nullptr;
HANDLE g_weatherUpdateRefreshEvent = nullptr;
std::mutex g_weatherMutex;
std::atomic<bool> g_weatherLoaded{false};
std::optional<std::wstring> g_weatherContent;
// The wttr.in request URL, built from the settings so that the weather thread
// never reads g_settings, which the settings thread is free to replace.
std::wstring g_weatherUrl;

// Whether system metrics/weather are used, derived from the line settings.
bool g_systemMetricsUsed = false;
bool g_weatherUsed = false;

// Last known wallpaper file time for change detection.
FILETIME g_lastWallpaperTime = {};

// DirectX device objects (shared).
[[clang::no_destroy]] ComPtr<ID3D11Device> g_d3dDevice;
[[clang::no_destroy]] ComPtr<IDXGIDevice> g_dxgiDevice;
[[clang::no_destroy]] ComPtr<IDXGIFactory2> g_dxgiFactory;
[[clang::no_destroy]] ComPtr<ID2D1Factory1> g_d2dFactory;
[[clang::no_destroy]] ComPtr<ID2D1Device> g_d2dDevice;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_dwriteFactory;

// Message-only window for receiving system notifications.
HWND g_messageWnd;

// Pending timer for the delayed overlay creation, owned by the desktop thread.
UINT_PTR g_createOverlayTimer = 0;

// Overlay window and resources.
HWND g_overlayWnd;
[[clang::no_destroy]] ComPtr<IDXGISwapChain1> g_swapChain;
[[clang::no_destroy]] ComPtr<ID2D1DeviceContext> g_dc;
[[clang::no_destroy]] ComPtr<IDCompositionDevice> g_compositionDevice;
[[clang::no_destroy]] ComPtr<IDCompositionTarget> g_compositionTarget;
[[clang::no_destroy]] ComPtr<IDCompositionVisual> g_compositionVisual;
[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_topLineTextFormat;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_topLineTextBrush;
[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_bottomLineTextFormat;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_bottomLineTextBrush;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_backgroundBrush;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_borderBrush;
[[clang::no_destroy]] ComPtr<ID2D1Bitmap> g_wallpaperBitmap;
[[clang::no_destroy]] ComPtr<ID2D1Effect> g_blurEffect;

// D2D1 Gaussian Blur effect CLSID.
// {1FEB6D69-2FE6-4AC9-8C58-1D7F93E7A6A5}
static const IID kCLSID_D2D1GaussianBlur = {
    0x1feb6d69,
    0x2fe6,
    0x4ac9,
    {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

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
    if (fontWeight <= 100) {
        return DWRITE_FONT_WEIGHT_THIN;
    }
    if (fontWeight <= 200) {
        return DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
    }
    if (fontWeight <= 300) {
        return DWRITE_FONT_WEIGHT_LIGHT;
    }
    if (fontWeight <= 400) {
        return DWRITE_FONT_WEIGHT_NORMAL;
    }
    if (fontWeight <= 500) {
        return DWRITE_FONT_WEIGHT_MEDIUM;
    }
    if (fontWeight <= 600) {
        return DWRITE_FONT_WEIGHT_SEMI_BOLD;
    }
    if (fontWeight <= 700) {
        return DWRITE_FONT_WEIGHT_BOLD;
    }
    if (fontWeight <= 800) {
        return DWRITE_FONT_WEIGHT_EXTRA_BOLD;
    }
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

// Expand an English wildcard counter path into localized paths.
// Implemented according to the note here:
// https://learn.microsoft.com/en-us/windows/win32/api/pdh/nf-pdh-pdhaddenglishcounterw
std::vector<std::wstring> ExpandEnglishWildcard(PCWSTR wildcardPath) {
    // Step 1: Add English counter with wildcards to get localized path.
    PDH_HCOUNTER tempCounter;
    PDH_STATUS status =
        PdhAddEnglishCounter(g_metricsQuery, wildcardPath, 0, &tempCounter);
    if (FAILED(status)) {
        Wh_Log(L"PdhAddEnglishCounter error %08X", status);
        return {};
    }

    // Step 2: Get counter info to obtain localized full path.
    DWORD required = 0;
    status = PdhGetCounterInfo(tempCounter, FALSE, &required, nullptr);
    if (FAILED(status) && status != static_cast<PDH_STATUS>(PDH_MORE_DATA)) {
        Wh_Log(L"PdhGetCounterInfo (size) error %08X", status);
        PdhRemoveCounter(tempCounter);
        return {};
    }

    if (required == 0) {
        PdhRemoveCounter(tempCounter);
        return {};
    }

    std::vector<BYTE> counterInfoBuffer(required);
    PDH_COUNTER_INFO* counterInfo =
        reinterpret_cast<PDH_COUNTER_INFO*>(counterInfoBuffer.data());

    status = PdhGetCounterInfo(tempCounter, FALSE, &required, counterInfo);
    PdhRemoveCounter(tempCounter);
    if (FAILED(status)) {
        Wh_Log(L"PdhGetCounterInfo error %08X", status);
        return {};
    }

    // Step 3: Expand wildcards using the localized path.
    required = 0;
    status = PdhExpandWildCardPathW(nullptr, counterInfo->szFullPath, nullptr,
                                    &required, 0);
    if (FAILED(status) && status != static_cast<PDH_STATUS>(PDH_MORE_DATA)) {
        Wh_Log(L"PdhExpandWildCardPath (localized, size) error %08X", status);
        return {};
    }

    if (required == 0) {
        return {};
    }

    std::vector<WCHAR> pathList(required);
    status = PdhExpandWildCardPathW(nullptr, counterInfo->szFullPath,
                                    pathList.data(), &required, 0);
    if (FAILED(status)) {
        Wh_Log(L"PdhExpandWildCardPath (localized) error %08X", status);
        return {};
    }

    std::vector<std::wstring> result;
    PCWSTR p = pathList.data();
    while (*p) {
        result.push_back(p);
        p += wcslen(p) + 1;
    }

    return result;
}

std::vector<std::wstring> FilterMetricPaths(const WildcardMetric& metric,
                                            std::vector<std::wstring> paths) {
    if (metric.pathFilter.empty()) {
        return paths;
    }

    std::vector<std::wstring> filtered;
    for (const auto& path : paths) {
        if (StrStrIW(path.c_str(), metric.pathFilter.c_str())) {
            filtered.push_back(path);
        }
    }

    return filtered;
}

void UpdateWildcardMetric(WildcardMetric& metric) {
    if (!metric.wildcardPath) {
        return;
    }

    auto currentPaths =
        FilterMetricPaths(metric, ExpandEnglishWildcard(metric.wildcardPath));

    std::unordered_set<std::wstring> currentPathSet(currentPaths.begin(),
                                                    currentPaths.end());

    // Remove counters that are no longer valid.
    for (auto it = metric.counters.begin(); it != metric.counters.end();) {
        if (currentPathSet.find(it->path) == currentPathSet.end()) {
            Wh_Log(L"Removing outdated counter: %s", it->path.c_str());
            PdhRemoveCounter(it->counter);
            it = metric.counters.erase(it);
        } else {
            ++it;
        }
    }

    // Build a set of existing paths.
    std::unordered_set<std::wstring> existingPaths;
    for (const auto& entry : metric.counters) {
        existingPaths.insert(entry.path);
    }

    // Add new counters.
    for (const auto& path : currentPaths) {
        if (existingPaths.find(path) == existingPaths.end()) {
            PDH_HCOUNTER counter;
            PDH_STATUS status =
                PdhAddCounter(g_metricsQuery, path.c_str(), 0, &counter);
            if (status == ERROR_SUCCESS) {
                Wh_Log(L"Adding new counter: %s", path.c_str());
                metric.counters.push_back({path, counter});
            }
        }
    }
}

// The WinINet defaults are minutes long, which would hold up whoever waits for
// the requesting thread to finish.
constexpr DWORD kUrlRequestTimeoutMs = 5000;

std::mutex g_urlRequestMutex;
HINTERNET g_urlRequestOpenHandle = nullptr;
HINTERNET g_urlRequestUrlHandle = nullptr;
bool g_urlRequestsCanceled = false;

// Requires g_urlRequestMutex to be held.
void CloseUrlRequestHandles() {
    if (g_urlRequestUrlHandle) {
        InternetCloseHandle(g_urlRequestUrlHandle);
        g_urlRequestUrlHandle = nullptr;
    }

    if (g_urlRequestOpenHandle) {
        InternetCloseHandle(g_urlRequestOpenHandle);
        g_urlRequestOpenHandle = nullptr;
    }
}

// Closes the handles of the request in flight, if any, which makes the blocking
// WinINet call using them return at once. Further requests fail until
// ResumeUrlRequests is called.
void CancelUrlRequests() {
    std::lock_guard<std::mutex> guard(g_urlRequestMutex);
    g_urlRequestsCanceled = true;
    CloseUrlRequestHandles();
}

void ResumeUrlRequests() {
    std::lock_guard<std::mutex> guard(g_urlRequestMutex);
    g_urlRequestsCanceled = false;
}

// Scope of a single request. At most one request runs at a time, so its handles
// live in globals where CancelUrlRequests can reach them.
struct UrlRequestScope {
    UrlRequestScope() = default;
    UrlRequestScope(const UrlRequestScope&) = delete;
    UrlRequestScope& operator=(const UrlRequestScope&) = delete;

    ~UrlRequestScope() {
        std::lock_guard<std::mutex> guard(g_urlRequestMutex);
        CloseUrlRequestHandles();
    }

    bool PublishOpenHandle(HINTERNET handle) {
        return Publish(&g_urlRequestOpenHandle, handle);
    }

    bool PublishUrlHandle(HINTERNET handle) {
        return Publish(&g_urlRequestUrlHandle, handle);
    }

   private:
    // Hands the handle over to the globals. Returns false if requests are
    // canceled, in which case the handle is closed and must not be used.
    static bool Publish(HINTERNET* slot, HINTERNET handle) {
        std::lock_guard<std::mutex> guard(g_urlRequestMutex);
        if (g_urlRequestsCanceled) {
            InternetCloseHandle(handle);
            return false;
        }

        *slot = handle;
        return true;
    }
};

std::optional<std::wstring> GetUrlContent(PCWSTR lpUrl) {
    UrlRequestScope requestScope;

    HINTERNET hOpenHandle = InternetOpen(
        L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hOpenHandle) {
        return std::nullopt;
    }

    if (!requestScope.PublishOpenHandle(hOpenHandle)) {
        return std::nullopt;
    }

    DWORD timeout = kUrlRequestTimeoutMs;
    InternetSetOption(hOpenHandle, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                      sizeof(timeout));
    InternetSetOption(hOpenHandle, INTERNET_OPTION_SEND_TIMEOUT, &timeout,
                      sizeof(timeout));
    InternetSetOption(hOpenHandle, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                      sizeof(timeout));

    HINTERNET hUrlHandle =
        InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
                        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
                        0);
    if (!hUrlHandle) {
        return std::nullopt;
    }

    if (!requestScope.PublishUrlHandle(hUrlHandle)) {
        return std::nullopt;
    }

    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    if (!HttpQueryInfo(hUrlHandle,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &dwStatusCode, &dwStatusCodeSize, nullptr) ||
        dwStatusCode != 200) {
        return std::nullopt;
    }

    LPBYTE pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (!pUrlContent) {
        return std::nullopt;
    }

    DWORD dwNumberOfBytesRead;
    if (!InternetReadFile(hUrlHandle, pUrlContent, 0x400,
                          &dwNumberOfBytesRead)) {
        HeapFree(GetProcessHeap(), 0, pUrlContent);
        return std::nullopt;
    }

    DWORD dwLength = dwNumberOfBytesRead;

    while (dwNumberOfBytesRead) {
        LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(
            GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
        if (!pNewUrlContent) {
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        pUrlContent = pNewUrlContent;
        if (!InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400,
                              &dwNumberOfBytesRead)) {
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        dwLength += dwNumberOfBytesRead;
    }

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

std::wstring MakeWeatherUrl() {
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

    return weatherUrl;
}

bool UpdateWeatherContent() {
    std::wstring weatherUrl;
    {
        std::lock_guard<std::mutex> guard(g_weatherMutex);
        weatherUrl = g_weatherUrl;
    }

    if (weatherUrl.empty()) {
        return false;
    }

    Wh_Log(L"Fetching weather from URL: %s", weatherUrl.c_str());

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

    HANDLE handles[] = {
        g_weatherUpdateStopEvent,
        g_weatherUpdateRefreshEvent,
    };

    while (true) {
        UpdateWeatherContent();

        DWORD seconds = kSecondsForNormalUpdate;
        if (!g_weatherLoaded && seconds > kSecondsForQuickRetry) {
            seconds = kSecondsForQuickRetry;
        }

        DWORD dwWaitResult = WaitForMultipleObjects(ARRAYSIZE(handles), handles,
                                                    FALSE, seconds * 1000);
        if (dwWaitResult == WAIT_FAILED) {
            Wh_Log(L"WAIT_FAILED");
            break;
        }

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
           IsPatternUsed(L"%gpu%") || IsPatternUsed(L"%ram_used%") ||
           IsPatternUsed(L"%ram_committed%") ||
           IsPatternUsed(L"%ram_committed_used%") ||
           IsPatternUsed(L"%ram_committed_total%") ||
           IsPatternUsed(L"%vram%") || IsPatternUsed(L"%vram_used%") ||
           IsPatternUsed(L"%vram_shared%") ||
           IsPatternUsed(L"%vram_shared_used%") ||
           IsPatternUsed(L"%cpu_temp%") || IsPatternUsed(L"%cpu_temp_f%") ||
           IsPatternUsed(L"%gpu_temp%") || IsPatternUsed(L"%gpu_temp_f%");
}

bool IsWeatherUsed() {
    return IsPatternUsed(L"%weather%");
}

void WeatherUpdateThreadInit() {
    if (!g_weatherUsed || g_weatherUpdateThread) {
        return;
    }

    g_weatherUpdateStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_weatherUpdateRefreshEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_weatherUpdateThread =
        CreateThread(nullptr, 0, WeatherUpdateThread, nullptr, 0, nullptr);
}

// Requires g_formatLineMutex to be held, like the other weather thread
// functions.
void WeatherUpdateThreadRefresh() {
    if (!g_weatherUpdateRefreshEvent) {
        return;
    }

    // Fall back to the quick retry interval if the request fails, which it may
    // well do while the network is still coming up.
    g_weatherLoaded = false;
    SetEvent(g_weatherUpdateRefreshEvent);
}

void WeatherUpdateThreadUninit() {
    if (g_weatherUpdateThread) {
        SetEvent(g_weatherUpdateStopEvent);
        CancelUrlRequests();
        WaitForSingleObject(g_weatherUpdateThread, INFINITE);
        CloseHandle(g_weatherUpdateThread);
        g_weatherUpdateThread = nullptr;
        CloseHandle(g_weatherUpdateRefreshEvent);
        g_weatherUpdateRefreshEvent = nullptr;
        CloseHandle(g_weatherUpdateStopEvent);
        g_weatherUpdateStopEvent = nullptr;
        ResumeUrlRequests();
    }

    std::lock_guard<std::mutex> guard(g_weatherMutex);
    g_weatherLoaded = false;
    g_weatherContent.reset();
}

// D3DKMT is used to read the GPU temperature, which has no PDH counter. The
// types and gdi32 exports below are declared directly to avoid depending on the
// d3dkmthk.h header, which isn't available in all build environments.
using D3DKMT_HANDLE = UINT32;

typedef struct _D3DKMT_OPENADAPTERFROMLUID {
    LUID AdapterLuid;
    D3DKMT_HANDLE hAdapter;
} D3DKMT_OPENADAPTERFROMLUID;

typedef struct _D3DKMT_CLOSEADAPTER {
    D3DKMT_HANDLE hAdapter;
} D3DKMT_CLOSEADAPTER;

typedef struct _D3DKMT_QUERYADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    UINT Type;  // KMTQUERYADAPTERINFOTYPE
    VOID* pPrivateDriverData;
    UINT PrivateDriverDataSize;
} D3DKMT_QUERYADAPTERINFO;

typedef struct _D3DKMT_ADAPTER_PERFDATA {
    UINT PhysicalAdapterIndex;
    ULONGLONG MemoryFrequency;
    ULONGLONG MaxMemoryFrequency;
    ULONGLONG MaxMemoryFrequencyOC;
    ULONGLONG MemoryBandwidth;
    ULONGLONG PCIEBandwidth;
    ULONG FanRPM;
    ULONG Power;
    ULONG Temperature;
    UCHAR PowerStateOverride;
} D3DKMT_ADAPTER_PERFDATA;

// KMTQAITYPE_ADAPTERPERFDATA.
constexpr UINT kAdapterPerfDataQueryType = 62;

using D3DKMTOpenAdapterFromLuid_t =
    NTSTATUS(WINAPI*)(D3DKMT_OPENADAPTERFROMLUID*);
D3DKMTOpenAdapterFromLuid_t pD3DKMTOpenAdapterFromLuid;

using D3DKMTQueryAdapterInfo_t = NTSTATUS(WINAPI*)(D3DKMT_QUERYADAPTERINFO*);
D3DKMTQueryAdapterInfo_t pD3DKMTQueryAdapterInfo;

using D3DKMTCloseAdapter_t = NTSTATUS(WINAPI*)(const D3DKMT_CLOSEADAPTER*);
D3DKMTCloseAdapter_t pD3DKMTCloseAdapter;

// The adapter the GPU patterns report on: the configured one, or the one with
// the most dedicated video memory. Static hardware information, queried lazily
// and cached, keyed by the configured name so that changing it re-queries.
struct DxgiAdapterInfo {
    LUID luid;
    // The LUID as it appears in GPU counter instances, e.g.
    // "luid_0x00000000_0x0000C40C_".
    std::wstring luidPathFilter;
    SIZE_T dedicatedVideoMemory;
    SIZE_T sharedSystemMemory;
};

std::optional<DxgiAdapterInfo> QueryDxgiAdapterInfo(PCWSTR gpuAdapterName) {
    ComPtr<IDXGIFactory1> factory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        Wh_Log(L"CreateDXGIFactory1 failed: 0x%08X", hr);
        return std::nullopt;
    }

    DXGI_ADAPTER_DESC bestDesc{};
    bool found = false;

    for (UINT i = 0;; i++) {
        ComPtr<IDXGIAdapter> adapter;
        if (factory->EnumAdapters(i, &adapter) == DXGI_ERROR_NOT_FOUND) {
            break;
        }

        DXGI_ADAPTER_DESC desc;
        if (FAILED(adapter->GetDesc(&desc))) {
            continue;
        }

        Wh_Log(L"DXGI adapter %u: %s (LUID: 0x%08X_0x%08X, VRAM: %zu)", i,
               desc.Description, desc.AdapterLuid.HighPart,
               desc.AdapterLuid.LowPart, desc.DedicatedVideoMemory);

        if (*gpuAdapterName) {
            if (StrStrIW(desc.Description, gpuAdapterName)) {
                bestDesc = desc;
                found = true;
                break;
            }
        } else if (!found ||
                   desc.DedicatedVideoMemory > bestDesc.DedicatedVideoMemory) {
            bestDesc = desc;
            found = true;
        }
    }

    if (!found) {
        Wh_Log(L"No GPU adapter found");
        return std::nullopt;
    }

    Wh_Log(L"Using GPU adapter %s", bestDesc.Description);

    WCHAR pathFilter[32];
    swprintf_s(pathFilter, L"luid_0x%08X_0x%08X_",
               bestDesc.AdapterLuid.HighPart, bestDesc.AdapterLuid.LowPart);

    return DxgiAdapterInfo{
        .luid = bestDesc.AdapterLuid,
        .luidPathFilter = pathFilter,
        .dedicatedVideoMemory = bestDesc.DedicatedVideoMemory,
        .sharedSystemMemory = bestDesc.SharedSystemMemory,
    };
}

const std::optional<DxgiAdapterInfo>& GetDxgiAdapterInfo() {
    static std::optional<std::wstring> queriedName;
    static std::optional<DxgiAdapterInfo> info;

    PCWSTR gpuAdapterName = g_settings.gpuAdapterName.get();
    if (!queriedName || *queriedName != gpuAdapterName) {
        info = QueryDxgiAdapterInfo(gpuAdapterName);
        queriedName = gpuAdapterName;
    }

    return info;
}

std::optional<double> GetDedicatedVramTotalGb() {
    const auto& info = GetDxgiAdapterInfo();
    if (!info || info->dedicatedVideoMemory == 0) {
        return std::nullopt;
    }

    return (double)info->dedicatedVideoMemory / kGBInBytes;
}

std::optional<double> GetSharedVramTotalGb() {
    const auto& info = GetDxgiAdapterInfo();
    if (!info || info->sharedSystemMemory == 0) {
        return std::nullopt;
    }

    return (double)info->sharedSystemMemory / kGBInBytes;
}

// Queries the GPU temperature via D3DKMT. Returns nullopt if the driver doesn't
// report a temperature.
std::optional<double> GetGpuTemperatureCelsius() {
    if (!pD3DKMTOpenAdapterFromLuid || !pD3DKMTQueryAdapterInfo ||
        !pD3DKMTCloseAdapter) {
        return std::nullopt;
    }

    const auto& info = GetDxgiAdapterInfo();
    if (!info) {
        return std::nullopt;
    }

    D3DKMT_OPENADAPTERFROMLUID openAdapter{.AdapterLuid = info->luid};
    if (pD3DKMTOpenAdapterFromLuid(&openAdapter) != 0) {
        return std::nullopt;
    }

    D3DKMT_ADAPTER_PERFDATA perfData{};
    D3DKMT_QUERYADAPTERINFO queryInfo{
        .hAdapter = openAdapter.hAdapter,
        .Type = kAdapterPerfDataQueryType,
        .pPrivateDriverData = &perfData,
        .PrivateDriverDataSize = sizeof(perfData),
    };

    NTSTATUS status = pD3DKMTQueryAdapterInfo(&queryInfo);

    D3DKMT_CLOSEADAPTER closeAdapter{.hAdapter = openAdapter.hAdapter};
    pD3DKMTCloseAdapter(&closeAdapter);

    if (status != 0) {
        return std::nullopt;
    }

    // A zero reading means the driver doesn't expose a temperature; treat it as
    // unavailable rather than showing an implausible 0 degrees.
    if (perfData.Temperature == 0) {
        return std::nullopt;
    }

    // Temperature is reported in tenths of a degree Celsius.
    return perfData.Temperature / 10.0;
}

bool InitMetrics() {
    // Determine which metrics are needed based on patterns used.
    bool needCpu = IsPatternUsed(L"%cpu%");
    bool needUpload = IsPatternUsed(L"%upload_speed%");
    bool needDownload = IsPatternUsed(L"%download_speed%");
    bool needDiskRead = IsPatternUsed(L"%disk_read%");
    bool needDiskWrite = IsPatternUsed(L"%disk_write%");
    bool needGpu = IsPatternUsed(L"%gpu%");
    bool needVram = IsPatternUsed(L"%vram%") || IsPatternUsed(L"%vram_used%");
    bool needVramShared =
        IsPatternUsed(L"%vram_shared%") || IsPatternUsed(L"%vram_shared_used%");
    bool needCpuTemp =
        IsPatternUsed(L"%cpu_temp%") || IsPatternUsed(L"%cpu_temp_f%");

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
        !needDiskWrite && !needGpu && !needVram && !needVramShared &&
        !needCpuTemp) {
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
        g_uploadMetric.wildcardPath = L"\\Network Interface(*)\\Bytes Sent/sec";
        UpdateWildcardMetric(g_uploadMetric);
    }

    // Network download counters (wildcard expansion).
    if (needDownload) {
        g_downloadMetric.wildcardPath =
            L"\\Network Interface(*)\\Bytes Received/sec";
        UpdateWildcardMetric(g_downloadMetric);
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

    // GPU counters (wildcard expansion). They have an instance for every
    // adapter in the system (and, for GPU Engine, per engine per process), so
    // they are filtered down to the selected adapter, and are skipped when
    // there is no adapter to report on.
    if (needGpu || needVram || needVramShared) {
        if (const auto& adapterInfo = GetDxgiAdapterInfo()) {
            if (needGpu) {
                g_gpuMetric.pathFilter = adapterInfo->luidPathFilter;
                g_gpuMetric.wildcardPath =
                    L"\\GPU Engine(*)\\Utilization Percentage";
                UpdateWildcardMetric(g_gpuMetric);
            }

            if (needVram) {
                g_vramMetric.pathFilter = adapterInfo->luidPathFilter;
                g_vramMetric.wildcardPath =
                    L"\\GPU Adapter Memory(*)\\Dedicated Usage";
                UpdateWildcardMetric(g_vramMetric);
            }

            if (needVramShared) {
                g_vramSharedMetric.pathFilter = adapterInfo->luidPathFilter;
                g_vramSharedMetric.wildcardPath =
                    L"\\GPU Adapter Memory(*)\\Shared Usage";
                UpdateWildcardMetric(g_vramSharedMetric);
            }
        }
    }

    // CPU temperature counters (wildcard expansion). Thermal zones that are
    // present but not functional report implausibly low values that would skew
    // the average, so require at least 200 Kelvin.
    if (needCpuTemp) {
        g_cpuTempMetric.minValue = 200;
        g_cpuTempMetric.wildcardPath =
            L"\\Thermal Zone Information(*)\\Temperature";
        UpdateWildcardMetric(g_cpuTempMetric);
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
        g_diskReadCounter = nullptr;
        g_diskWriteCounter = nullptr;
        g_uploadMetric = {};
        g_downloadMetric = {};
        g_gpuMetric = {};
        g_vramMetric = {};
        g_vramSharedMetric = {};
        g_cpuTempMetric = {};
    }

    g_metricsLastFormatIndex = 0;
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

constexpr ULONGLONG kSecondIn100Ns = 10000000ULL;

ULARGE_INTEGER SystemTimeTo100Ns(const SYSTEMTIME* time) {
    FILETIME ft{};
    SystemTimeToFileTime(time, &ft);
    return ULARGE_INTEGER{
        .LowPart = ft.dwLowDateTime,
        .HighPart = ft.dwHighDateTime,
    };
}

// Metrics are sampled once per refresh interval, and the formatted values only
// change when they are.
DWORD GetMetricsFormatIndex() {
    ULONGLONG intervalIn100Ns = kSecondIn100Ns * g_settings.refreshInterval;
    return static_cast<DWORD>(SystemTimeTo100Ns(&g_formatTime).QuadPart /
                              intervalIn100Ns);
}

void UpdateAllWildcardMetrics() {
    UpdateWildcardMetric(g_uploadMetric);
    UpdateWildcardMetric(g_downloadMetric);
    UpdateWildcardMetric(g_gpuMetric);
    UpdateWildcardMetric(g_vramMetric);
    UpdateWildcardMetric(g_vramSharedMetric);
    UpdateWildcardMetric(g_cpuTempMetric);
}

void CollectMetricsDataIfNeeded() {
    DWORD metricsFormatIndex = GetMetricsFormatIndex();
    if (g_metricsLastFormatIndex != metricsFormatIndex) {
        if (g_metricsQuery) {
            UpdateAllWildcardMetrics();
            PdhCollectQueryData(g_metricsQuery);
        }
        g_metricsLastFormatIndex = metricsFormatIndex;
    }
}

// Counters can report more than 100%: the CPU utility counter exceeds 100 with
// turbo boost, and the GPU value is a sum over all engines.
int CapPercent(double value) {
    return (int)(std::min)(value, 100.0);
}

std::wstring FormatLocaleNum(double val, unsigned int digitsAfterDecimal) {
    int valStrLen = _scwprintf(L"%.17f", val);
    if (valStrLen < 0) {
        return std::wstring();
    }

    std::wstring valStr(valStrLen + 1, L'\0');
    if (swprintf_s(valStr.data(), valStr.size(), L"%.17f", val) < 0) {
        return std::wstring();
    }

    WCHAR decSep[4];
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, decSep,
                         ARRAYSIZE(decSep))) {
        // Fallback.
        decSep[0] = L'.';
        decSep[1] = L'\0';
    }

    NUMBERFMTW fmt{
        .NumDigits = digitsAfterDecimal,
        .LeadingZero = 1,
        .lpDecimalSep = const_cast<LPWSTR>(decSep),
        .lpThousandSep = const_cast<LPWSTR>(L""),
    };

    // Query required size.
    int needed = GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, valStr.c_str(),
                                   &fmt, nullptr, 0);
    if (needed == 0) {
        return std::wstring();
    }

    // Format.
    std::wstring out(needed - 1, L'\0');
    if (GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, valStr.c_str(), &fmt,
                          out.data(), needed) == 0) {
        return std::wstring();
    }

    return out;
}

void FormatGbValue(double val, PWSTR buffer, size_t bufferSize) {
    wcscpy_s(buffer, bufferSize, FormatLocaleNum(val, 1).c_str());
}

void FormatTransferSpeed(double bytesPerSec, PWSTR buffer, size_t bufferSize) {
    constexpr double kKBInBytes = 1024.0;
    constexpr double kMBInBytes = 1024.0 * kKBInBytes;

    // Use KB/s for values < 1 MB/s, otherwise MB/s.
    double valUnit;
    PCWSTR unit;
    if (bytesPerSec < kMBInBytes) {
        valUnit = bytesPerSec / kKBInBytes;
        unit = L" KB/s";
    } else {
        valUnit = bytesPerSec / kMBInBytes;
        unit = L" MB/s";
    }

    // Keep identical width for values below 1000.
    unsigned int digitsAfterDecimal = 0;
    if (valUnit < 10) {
        digitsAfterDecimal = 2;
    } else if (valUnit < 100) {
        digitsAfterDecimal = 1;
    }

    swprintf_s(buffer, bufferSize, L"%s%s",
               FormatLocaleNum(valUnit, digitsAfterDecimal).c_str(), unit);
}

// Formats a metric into its cached buffer, or "-" if it's unavailable.
template <size_t N, typename Formatter>
PCWSTR GetMetricFormatted(FormattedString<N>& formattedString,
                          Formatter formatter) {
    DWORD metricsFormatIndex = GetMetricsFormatIndex();
    if (formattedString.formatIndex != metricsFormatIndex) {
        if (!formatter(formattedString.buffer,
                       ARRAYSIZE(formattedString.buffer))) {
            wcscpy_s(formattedString.buffer, L"-");
        }

        formattedString.formatIndex = metricsFormatIndex;
    }

    return formattedString.buffer;
}

std::optional<double> QueryCounter(PDH_HCOUNTER counter) {
    if (!counter) {
        return std::nullopt;
    }

    PDH_FMT_COUNTERVALUE val;
    if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &val) !=
        ERROR_SUCCESS) {
        return std::nullopt;
    }

    return val.doubleValue;
}

struct WildcardMetricValue {
    double sum;
    size_t count;
};

std::optional<WildcardMetricValue> QueryWildcardMetric(
    const WildcardMetric& metric) {
    double sum = 0.0;
    size_t count = 0;

    for (const auto& entry : metric.counters) {
        PDH_FMT_COUNTERVALUE val;
        if (PdhGetFormattedCounterValue(entry.counter, PDH_FMT_DOUBLE, nullptr,
                                        &val) == ERROR_SUCCESS &&
            val.doubleValue >= metric.minValue) {
            sum += val.doubleValue;
            count++;
        }
    }

    if (count == 0) {
        return std::nullopt;
    }

    return WildcardMetricValue{sum, count};
}

std::optional<double> QueryWildcardMetricSum(const WildcardMetric& metric) {
    auto value = QueryWildcardMetric(metric);
    if (!value) {
        return std::nullopt;
    }

    return value->sum;
}

std::optional<double> QueryWildcardMetricAvg(const WildcardMetric& metric) {
    auto value = QueryWildcardMetric(metric);
    if (!value) {
        return std::nullopt;
    }

    return value->sum / value->count;
}

PCWSTR GetCpuFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_cpuFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto val = QueryCounter(g_cpuCounter);
            if (!val) {
                return false;
            }
            swprintf_s(buffer, bufferSize, L"%d%%", CapPercent(*val));
            return true;
        });
}

// System memory status, sampled at most once per refresh interval. Empty if
// the query failed.
std::optional<MEMORYSTATUSEX> GetRamStatus() {
    static MEMORYSTATUSEX status{};
    static bool valid = false;
    static DWORD lastFormatIndex = 0xFFFFFFFF;

    DWORD formatIndex = GetMetricsFormatIndex();
    if (lastFormatIndex != formatIndex) {
        status.dwLength = sizeof(status);
        valid = GlobalMemoryStatusEx(&status);
        lastFormatIndex = formatIndex;
    }

    return valid ? std::optional<MEMORYSTATUSEX>(status) : std::nullopt;
}

PCWSTR GetRamFormatted() {
    return GetMetricFormatted(
        g_ramFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status) {
                return false;
            }
            swprintf_s(buffer, bufferSize, L"%d%%", (int)status->dwMemoryLoad);
            return true;
        });
}

PCWSTR GetRamUsedFormatted() {
    return GetMetricFormatted(g_ramUsedFormatted, [](PWSTR buffer,
                                                     size_t bufferSize) {
        auto status = GetRamStatus();
        if (!status) {
            return false;
        }
        double usedGb =
            (double)(status->ullTotalPhys - status->ullAvailPhys) / kGBInBytes;
        FormatGbValue(usedGb, buffer, bufferSize);
        return true;
    });
}

PCWSTR GetRamTotalFormatted() {
    return GetMetricFormatted(
        g_ramTotalFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status) {
                return false;
            }
            FormatGbValue((double)status->ullTotalPhys / kGBInBytes, buffer,
                          bufferSize);
            return true;
        });
}

PCWSTR GetRamCommittedFormatted() {
    return GetMetricFormatted(
        g_ramCommittedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status || status->ullTotalPageFile == 0) {
                return false;
            }
            int committed =
                (int)(((status->ullTotalPageFile - status->ullAvailPageFile) *
                       100) /
                      status->ullTotalPageFile);
            swprintf_s(buffer, bufferSize, L"%d%%", committed);
            return true;
        });
}

PCWSTR GetRamCommittedUsedFormatted() {
    return GetMetricFormatted(
        g_ramCommittedUsedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status) {
                return false;
            }
            double usedGb =
                (double)(status->ullTotalPageFile - status->ullAvailPageFile) /
                kGBInBytes;
            FormatGbValue(usedGb, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetRamCommittedTotalFormatted() {
    return GetMetricFormatted(
        g_ramCommittedTotalFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto status = GetRamStatus();
            if (!status) {
                return false;
            }
            FormatGbValue((double)status->ullTotalPageFile / kGBInBytes, buffer,
                          bufferSize);
            return true;
        });
}

PCWSTR GetBatteryFormatted() {
    return GetMetricFormatted(
        g_batteryFormatted, [](PWSTR buffer, size_t bufferSize) {
            SYSTEM_POWER_STATUS ps;
            if (!GetSystemPowerStatus(&ps) || ps.BatteryLifePercent == 255) {
                return false;
            }
            swprintf_s(buffer, bufferSize, L"%d%%", (int)ps.BatteryLifePercent);
            return true;
        });
}

PCWSTR GetBatteryTimeFormatted() {
    return GetMetricFormatted(g_batteryTimeFormatted, [](PWSTR buffer,
                                                         size_t bufferSize) {
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
        swprintf_s(buffer, bufferSize, L"%u:%02u", hours, minutes);
        return true;
    });
}

PCWSTR GetPowerFormatted() {
    return GetMetricFormatted(
        g_powerFormatted, [](PWSTR buffer, size_t bufferSize) {
            SYSTEM_BATTERY_STATE batteryState{};
            NTSTATUS status =
                CallNtPowerInformation(SystemBatteryState, nullptr, 0,
                                       &batteryState, sizeof(batteryState));
            if (status != 0 || batteryState.MaxCapacity == 0) {
                return false;
            }

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

            swprintf_s(buffer, bufferSize, L"%+ldW", powerWatts);
            return true;
        });
}

PCWSTR GetUploadSpeedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_uploadSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto speed = QueryWildcardMetricSum(g_uploadMetric);
            if (!speed) {
                return false;
            }
            FormatTransferSpeed(*speed, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetDownloadSpeedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_downloadSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto speed = QueryWildcardMetricSum(g_downloadMetric);
            if (!speed) {
                return false;
            }
            FormatTransferSpeed(*speed, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetTotalSpeedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(g_totalSpeedFormatted, [](PWSTR buffer,
                                                        size_t bufferSize) {
        auto uploadSpeed = QueryWildcardMetricSum(g_uploadMetric);
        auto downloadSpeed = QueryWildcardMetricSum(g_downloadMetric);
        if (!uploadSpeed || !downloadSpeed) {
            return false;
        }
        FormatTransferSpeed(*uploadSpeed + *downloadSpeed, buffer, bufferSize);
        return true;
    });
}

PCWSTR GetDiskReadSpeedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_diskReadSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto speed = QueryCounter(g_diskReadCounter);
            if (!speed) {
                return false;
            }
            FormatTransferSpeed(*speed, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetDiskWriteSpeedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_diskWriteSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto speed = QueryCounter(g_diskWriteCounter);
            if (!speed) {
                return false;
            }
            FormatTransferSpeed(*speed, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetDiskTotalSpeedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_diskTotalSpeedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto readSpeed = QueryCounter(g_diskReadCounter);
            auto writeSpeed = QueryCounter(g_diskWriteCounter);
            if (!readSpeed || !writeSpeed) {
                return false;
            }
            FormatTransferSpeed(*readSpeed + *writeSpeed, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetGpuFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_gpuFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto usage = QueryWildcardMetricSum(g_gpuMetric);
            if (!usage) {
                return false;
            }
            swprintf_s(buffer, bufferSize, L"%d%%", CapPercent(*usage));
            return true;
        });
}

PCWSTR GetVramFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_vramFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto usedBytes = QueryWildcardMetricSum(g_vramMetric);
            auto totalGb = GetDedicatedVramTotalGb();
            if (!usedBytes || !totalGb) {
                return false;
            }
            double usedGb = *usedBytes / kGBInBytes;
            swprintf_s(buffer, bufferSize, L"%d%%",
                       CapPercent(usedGb / *totalGb * 100.0));
            return true;
        });
}

PCWSTR GetVramUsedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_vramUsedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto usedBytes = QueryWildcardMetricSum(g_vramMetric);
            if (!usedBytes) {
                return false;
            }
            FormatGbValue(*usedBytes / kGBInBytes, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetVramTotalFormatted() {
    return GetMetricFormatted(g_vramTotalFormatted,
                              [](PWSTR buffer, size_t bufferSize) {
                                  auto totalGb = GetDedicatedVramTotalGb();
                                  if (!totalGb) {
                                      return false;
                                  }
                                  FormatGbValue(*totalGb, buffer, bufferSize);
                                  return true;
                              });
}

PCWSTR GetVramSharedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_vramSharedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto usedBytes = QueryWildcardMetricSum(g_vramSharedMetric);
            auto totalGb = GetSharedVramTotalGb();
            if (!usedBytes || !totalGb) {
                return false;
            }
            double usedGb = *usedBytes / kGBInBytes;
            swprintf_s(buffer, bufferSize, L"%d%%",
                       CapPercent(usedGb / *totalGb * 100.0));
            return true;
        });
}

PCWSTR GetVramSharedUsedFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_vramSharedUsedFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto usedBytes = QueryWildcardMetricSum(g_vramSharedMetric);
            if (!usedBytes) {
                return false;
            }
            FormatGbValue(*usedBytes / kGBInBytes, buffer, bufferSize);
            return true;
        });
}

PCWSTR GetVramSharedTotalFormatted() {
    return GetMetricFormatted(g_vramSharedTotalFormatted,
                              [](PWSTR buffer, size_t bufferSize) {
                                  auto totalGb = GetSharedVramTotalGb();
                                  if (!totalGb) {
                                      return false;
                                  }
                                  FormatGbValue(*totalGb, buffer, bufferSize);
                                  return true;
                              });
}

PCWSTR GetCpuTempFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_cpuTempFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto kelvin = QueryWildcardMetricAvg(g_cpuTempMetric);
            if (!kelvin) {
                return false;
            }
            swprintf_s(buffer, bufferSize, L"%d\u00B0C",
                       static_cast<int>(*kelvin - 273.15));
            return true;
        });
}

PCWSTR GetCpuTempFFormatted() {
    CollectMetricsDataIfNeeded();
    return GetMetricFormatted(
        g_cpuTempFFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto kelvin = QueryWildcardMetricAvg(g_cpuTempMetric);
            if (!kelvin) {
                return false;
            }
            double celsius = *kelvin - 273.15;
            swprintf_s(buffer, bufferSize, L"%d\u00B0F",
                       static_cast<int>(celsius * 9.0 / 5.0 + 32.0));
            return true;
        });
}

PCWSTR GetGpuTempFormatted() {
    return GetMetricFormatted(g_gpuTempFormatted,
                              [](PWSTR buffer, size_t bufferSize) {
                                  auto celsius = GetGpuTemperatureCelsius();
                                  if (!celsius) {
                                      return false;
                                  }
                                  swprintf_s(buffer, bufferSize, L"%d\u00B0C",
                                             static_cast<int>(*celsius));
                                  return true;
                              });
}

PCWSTR GetGpuTempFFormatted() {
    return GetMetricFormatted(
        g_gpuTempFFormatted, [](PWSTR buffer, size_t bufferSize) {
            auto celsius = GetGpuTemperatureCelsius();
            if (!celsius) {
                return false;
            }
            swprintf_s(buffer, bufferSize, L"%d\u00B0F",
                       static_cast<int>(*celsius * 9.0 / 5.0 + 32.0));
            return true;
        });
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
        swprintf_s(g_weeknumFormatted.buffer, L"%02d",
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
        {L"%cpu_temp%"sv, GetCpuTempFormatted},
        {L"%cpu_temp_f%"sv, GetCpuTempFFormatted},
        {L"%ram%"sv, GetRamFormatted},
        {L"%ram_used%"sv, GetRamUsedFormatted},
        {L"%ram_total%"sv, GetRamTotalFormatted},
        {L"%ram_committed%"sv, GetRamCommittedFormatted},
        {L"%ram_committed_used%"sv, GetRamCommittedUsedFormatted},
        {L"%ram_committed_total%"sv, GetRamCommittedTotalFormatted},
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
        {L"%gpu_temp%"sv, GetGpuTempFormatted},
        {L"%gpu_temp_f%"sv, GetGpuTempFFormatted},
        {L"%vram%"sv, GetVramFormatted},
        {L"%vram_used%"sv, GetVramUsedFormatted},
        {L"%vram_total%"sv, GetVramTotalFormatted},
        {L"%vram_shared%"sv, GetVramSharedFormatted},
        {L"%vram_shared_used%"sv, GetVramSharedUsedFormatted},
        {L"%vram_shared_total%"sv, GetVramSharedTotalFormatted},
        {L"%newline%"sv, []() { return L"\n"; }},
        {L"%n%"sv, []() { return L"\n"; }},
    };

    for (const auto& t : tokens) {
        if (format.starts_with(t.token)) {
            resolvedCallback(t.getter());
            return t.token.size();
        }
    }

    // Check for weather pattern (requires mutex).
    if (auto token = L"%weather%"sv; format.starts_with(token)) {
        std::lock_guard<std::mutex> guard(g_weatherMutex);
        resolvedCallback(g_weatherContent ? g_weatherContent->c_str()
                                          : L"Loading...");
        return token.size();
    }

    return 0;  // Not a recognized token
}

void EnsureFormattingInitialized() {
    if (g_formattingInitialized) {
        return;
    }

    g_formattingInitialized = true;

    InitMetrics();
    WeatherUpdateThreadInit();
}

int FormatLine(PWSTR buffer, size_t bufferSize, std::wstring_view format) {
    if (bufferSize == 0) {
        return 0;
    }

    std::lock_guard<std::mutex> guard(g_formatLineMutex);

    EnsureFormattingInitialized();

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

// The Progman window of this process, which owns the desktop thread.
HWND GetProgmanWnd() {
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (!hProgman) {
        return nullptr;
    }

    DWORD progmanProcessId = 0;
    GetWindowThreadProcessId(hProgman, &progmanProcessId);
    if (progmanProcessId != GetCurrentProcessId()) {
        return nullptr;
    }

    return hProgman;
}

// Find the WorkerW window behind desktop icons.
// Based on weebp: https://github.com/Francesco149/weebp
HWND GetWorkerW() {
    HWND hProgman = GetProgmanWnd();
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

void RunDxgiWorkaroundForExplorerPatcher() {
    auto dxgiModule = GetModuleHandle(L"dxgi.dll");
    if (!dxgiModule) {
        return;
    }

    WCHAR dxgiPath[MAX_PATH];
    switch (GetModuleFileName(dxgiModule, dxgiPath, ARRAYSIZE(dxgiPath))) {
        case 0:
        case ARRAYSIZE(dxgiPath):
            Wh_Log(L"GetModuleFileName failed: %u", GetLastError());
            return;
    }

    WCHAR explorerPatcherDxgiPath[MAX_PATH];
    GetWindowsDirectory(explorerPatcherDxgiPath,
                        ARRAYSIZE(explorerPatcherDxgiPath));
    wcscat_s(explorerPatcherDxgiPath, L"\\dxgi.dll");

    if (_wcsicmp(dxgiPath, explorerPatcherDxgiPath) != 0) {
        return;
    }

    Wh_Log(L"Detected ExplorerPatcher dxgi.dll");

    auto dxgiDeclareAdapterRemovalSupport = (HRESULT(WINAPI*)())GetProcAddress(
        dxgiModule, "DXGIDeclareAdapterRemovalSupport");
    if (!dxgiDeclareAdapterRemovalSupport) {
        Wh_Log(L"DXGIDeclareAdapterRemovalSupport not found");
        return;
    }

    Wh_Log(L"Calling DXGIDeclareAdapterRemovalSupport");
    dxgiDeclareAdapterRemovalSupport();
}

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

bool RecreateTextResources();

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

    if (!RecreateTextResources()) {
        return false;
    }

    return true;
}

FILETIME GetWallpaperFileTime() {
    WCHAR path[MAX_PATH] = {};
    SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    FILETIME ft = {};
    HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        GetFileTime(hFile, nullptr, nullptr, &ft);
        CloseHandle(hFile);
    }
    return ft;
}

void CaptureWallpaperBitmap() {
    g_wallpaperBitmap.Reset();

    if (!g_overlayWnd || !g_dc || !g_swapChain) {
        return;
    }

    g_lastWallpaperTime = GetWallpaperFileTime();

    // Clear the overlay to transparent so the capture doesn't include
    // our own previously rendered content.
    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_dc->EndDraw();
    g_swapChain->Present(1, 0);
    DwmFlush();

    HWND hParent = GetParent(g_overlayWnd);
    if (!hParent) {
        return;
    }

    RECT rc;
    GetClientRect(hParent, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) {
        return;
    }

    // Capture from Progman which paints the wallpaper. WorkerW's GDI
    // surface is empty because DWM composites the wallpaper.
    HWND hSource = FindWindow(L"Progman", nullptr);
    if (!hSource) {
        hSource = hParent;
    }

    HDC hdcScreen = GetDC(nullptr);
    if (!hdcScreen) {
        return;
    }

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) {
        ReleaseDC(nullptr, hdcScreen);
        return;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // Top-down.
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hBmp =
        CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    if (!hBmp) {
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return;
    }

    HGDIOBJ hOldBmp = SelectObject(hdcMem, hBmp);

    // PW_RENDERFULLCONTENT (0x02) asks the window to render directly,
    // bypassing the DWM redirect bitmap.
    if (!PrintWindow(hSource, hdcMem, 0x02 /*PW_RENDERFULLCONTENT*/)) {
        Wh_Log(L"PrintWindow failed: %u", GetLastError());
        SelectObject(hdcMem, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return;
    }
    GdiFlush();

    // PrintWindow may produce alpha=0; set to 255 for premultiplied alpha.
    BYTE* pixels = static_cast<BYTE*>(pvBits);
    for (int i = 0; i < w * h; i++) {
        pixels[i * 4 + 3] = 255;
    }

    D2D1_BITMAP_PROPERTIES bitmapProps =
        D2D1::BitmapProperties(D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    HRESULT hr = g_dc->CreateBitmap(D2D1::SizeU(w, h), pvBits, w * 4,
                                    bitmapProps, &g_wallpaperBitmap);
    if (FAILED(hr)) {
        Wh_Log(L"CreateBitmap (wallpaper) failed: 0x%08X", hr);
    }

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

void ReleaseTextResources() {
    g_blurEffect.Reset();
    g_wallpaperBitmap.Reset();
    g_borderBrush.Reset();
    g_backgroundBrush.Reset();
    g_bottomLineTextBrush.Reset();
    g_bottomLineTextFormat.Reset();
    g_topLineTextBrush.Reset();
    g_topLineTextFormat.Reset();
}

void ReleaseSwapChainResources() {
    ReleaseTextResources();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
    g_dc.Reset();
    g_swapChain.Reset();
}

bool RecreateTextResources() {
    if (!g_dc) {
        return false;
    }

    HRESULT hr;

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

        // Create blur effect.
        if (g_settings.backgroundBlur > 0) {
            CaptureWallpaperBitmap();
            if (g_wallpaperBitmap) {
                hr = g_dc->CreateEffect(kCLSID_D2D1GaussianBlur, &g_blurEffect);
                if (SUCCEEDED(hr)) {
                    g_blurEffect->SetInput(0, g_wallpaperBitmap.Get());
                    g_blurEffect->SetValue(
                        0,  // D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION
                        (FLOAT)g_settings.backgroundBlur);
                    g_blurEffect->SetValue(
                        2,           // D2D1_GAUSSIANBLUR_PROP_BORDER_MODE
                        (UINT32)1);  // D2D1_BORDER_MODE_HARD
                } else {
                    Wh_Log(L"CreateEffect (blur) failed: 0x%08X", hr);
                }
            }
        }

        // Create border brush.
        if (g_settings.backgroundBorderSize > 0) {
            D2D1_COLOR_F borderColor =
                D2D1::ColorF(g_settings.backgroundBorderColorR / 255.0f,
                             g_settings.backgroundBorderColorG / 255.0f,
                             g_settings.backgroundBorderColorB / 255.0f,
                             g_settings.backgroundBorderColorA / 255.0f);
            hr = g_dc->CreateSolidColorBrush(borderColor, &g_borderBrush);
            if (FAILED(hr)) {
                Wh_Log(L"CreateSolidColorBrush (border) failed: 0x%08X", hr);
            }
        }
    }

    return true;
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
                float bgWidth = totalWidth + 2 * padding;
                float bgHeight = totalHeight + 2 * padding;
                float radius = std::min(
                    (float)g_settings.backgroundCornerRadius * g_dpiScale,
                    std::min(bgWidth, bgHeight) / 2.0f);
                D2D1_ROUNDED_RECT backgroundRect = D2D1::RoundedRect(
                    D2D1::RectF(blockX - padding, blockY - padding,
                                blockX + totalWidth + padding,
                                blockY + totalHeight + padding),
                    radius, radius);

                // Draw blurred wallpaper behind background.
                if (g_blurEffect) {
                    ComPtr<ID2D1RoundedRectangleGeometry> clipGeo;
                    g_d2dFactory->CreateRoundedRectangleGeometry(backgroundRect,
                                                                 &clipGeo);
                    if (clipGeo) {
                        g_dc->PushLayer(
                            D2D1::LayerParameters(D2D1::InfiniteRect(),
                                                  clipGeo.Get()),
                            nullptr);
                        g_dc->DrawImage(g_blurEffect.Get());
                        g_dc->PopLayer();
                    }
                }

                g_dc->FillRoundedRectangle(backgroundRect,
                                           g_backgroundBrush.Get());

                // Draw border inside the background using a geometry
                // ring (outer minus inner rounded rect) so corners
                // match the fill exactly.
                if (g_borderBrush) {
                    float bw = std::min(
                        (float)g_settings.backgroundBorderSize * g_dpiScale,
                        std::min(bgWidth, bgHeight) / 2.0f);
                    float innerRadius = std::max(0.0f, radius - bw);
                    D2D1_ROUNDED_RECT innerRect = D2D1::RoundedRect(
                        D2D1::RectF(backgroundRect.rect.left + bw,
                                    backgroundRect.rect.top + bw,
                                    backgroundRect.rect.right - bw,
                                    backgroundRect.rect.bottom - bw),
                        innerRadius, innerRadius);

                    ComPtr<ID2D1RoundedRectangleGeometry> outerGeo;
                    ComPtr<ID2D1RoundedRectangleGeometry> innerGeo;
                    g_d2dFactory->CreateRoundedRectangleGeometry(backgroundRect,
                                                                 &outerGeo);
                    g_d2dFactory->CreateRoundedRectangleGeometry(innerRect,
                                                                 &innerGeo);
                    if (outerGeo && innerGeo) {
                        ID2D1Geometry* geos[] = {outerGeo.Get(),
                                                 innerGeo.Get()};
                        ComPtr<ID2D1GeometryGroup> ring;
                        g_d2dFactory->CreateGeometryGroup(
                            D2D1_FILL_MODE_ALTERNATE, geos, 2, &ring);
                        if (ring) {
                            g_dc->FillGeometry(ring.Get(), g_borderBrush.Get());
                        }
                    }
                }
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

// Milliseconds until the next metrics sample, matching the bucketing of
// GetMetricsFormatIndex.
UINT GetNextMetricsSampleTimeout(const SYSTEMTIME* time) {
    ULONGLONG timeIn100Ns = SystemTimeTo100Ns(time).QuadPart;
    ULONGLONG intervalIn100Ns = kSecondIn100Ns * g_settings.refreshInterval;
    ULONGLONG nextIn100Ns =
        (timeIn100Ns / intervalIn100Ns + 1) * intervalIn100Ns;
    return static_cast<UINT>((nextIn100Ns - timeIn100Ns) / 10000);
}

UINT GetNextUpdateTimeout() {
    SYSTEMTIME time;
    GetLocalTime(&time);

    // Add a small extra delay to make sure we are past the second/minute
    // change.
    constexpr UINT kExtraDelayMs = 200;

    // Refresh every second when seconds are shown or weather is pending.
    if (g_settings.showSeconds || (g_weatherUsed && !g_weatherLoaded)) {
        return 1000 - time.wMilliseconds + kExtraDelayMs;
    }

    // Otherwise the time display only changes on the minute.
    UINT timeout = (60 - time.wSecond) * 1000 - time.wMilliseconds;

    if (g_systemMetricsUsed) {
        timeout = (std::min)(timeout, GetNextMetricsSampleTimeout(&time));
    }

    return timeout + kExtraDelayMs;
}

void ScheduleNextUpdate() {
    if (!g_overlayWnd) {
        return;
    }

    UINT timeout = GetNextUpdateTimeout();
    SetTimer(g_overlayWnd, TIMER_ID_OVERLAY_REFRESH, timeout, nullptr);
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
        if (CreateSwapChainResources(rc.right - rc.left, rc.bottom - rc.top)) {
            RenderOverlay();
        } else {
            ReleaseSwapChainResources();
        }
    }

    // Schedule a delayed wallpaper recapture so the system has time to
    // repaint the wallpaper at the new resolution.
    if (g_messageWnd && g_settings.backgroundEnabled &&
        g_settings.backgroundBlur > 0) {
        SetTimer(g_messageWnd, TIMER_ID_MSG_WALLPAPER_REFRESH, 500, nullptr);
    }
}

// Forward declarations.
void CreateOverlayWindow();
void ApplySettingsChanged();

LRESULT CALLBACK OverlayWndProc(HWND hWnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (uMsg) {
        case WM_TIMER:
            if (!g_unloading && wParam == TIMER_ID_OVERLAY_REFRESH) {
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

        case WM_DESTROY:
            Wh_Log(L"Overlay WM_DESTROY");
            ReleaseSwapChainResources();
            g_overlayWnd = nullptr;
            // Schedule recreation if not unloading.
            if (!g_unloading && g_messageWnd) {
                SetTimer(g_messageWnd, TIMER_ID_MSG_RECREATE_OVERLAY, 200,
                         nullptr);
            }
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;

        case WM_APP_SETTINGS_CHANGED:
            ApplySettingsChanged();
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MessageWndProc(HWND hWnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (uMsg) {
        case WM_DISPLAYCHANGE:
            Wh_Log(L"WM_DISPLAYCHANGE received");
            if (!g_unloading) {
                // Delay handling to allow WorkerW to resize first.
                SetTimer(hWnd, TIMER_ID_MSG_DISPLAY_CHANGE, 200, nullptr);
            }
            return 0;

        case WM_SETTINGCHANGE: {
            if (g_overlayWnd && !g_unloading && g_settings.backgroundEnabled &&
                g_settings.backgroundBlur > 0) {
                FILETIME ft = GetWallpaperFileTime();
                if (CompareFileTime(&ft, &g_lastWallpaperTime) != 0) {
                    SetTimer(hWnd, TIMER_ID_MSG_WALLPAPER_REFRESH, 2000,
                             nullptr);
                }
            }
            return 0;
        }

        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMRESUMECRITICAL:
                case PBT_APMRESUMESUSPEND:
                case PBT_APMRESUMEAUTOMATIC:
                    if (!g_unloading) {
                        Wh_Log(L"Resumed, refreshing weather");
                        std::lock_guard<std::mutex> guard(g_formatLineMutex);
                        WeatherUpdateThreadRefresh();
                    }
                    break;
            }
            break;

        case WM_TIMER:
            if (g_unloading) {
                return 0;
            }
            if (wParam == TIMER_ID_MSG_DISPLAY_CHANGE) {
                KillTimer(hWnd, TIMER_ID_MSG_DISPLAY_CHANGE);
                HandleDisplayChange();
            } else if (wParam == TIMER_ID_MSG_RECREATE_OVERLAY) {
                KillTimer(hWnd, TIMER_ID_MSG_RECREATE_OVERLAY);
                Wh_Log(L"Recreating overlay window");
                CreateOverlayWindow();
            } else if (wParam == TIMER_ID_MSG_WALLPAPER_REFRESH) {
                KillTimer(hWnd, TIMER_ID_MSG_WALLPAPER_REFRESH);
                if (g_overlayWnd && g_settings.backgroundEnabled &&
                    g_settings.backgroundBlur > 0) {
                    ReleaseTextResources();
                    RecreateTextResources();
                    RenderOverlay();
                }
            }
            return 0;

        case WM_DESTROY:
            g_messageWnd = nullptr;
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Overlay window management

bool g_overlayClassRegistered = false;

bool RegisterOverlayWindowClass() {
    if (g_overlayClassRegistered) {
        return true;
    }

    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = OVERLAY_WINDOW_CLASS;
    if (!RegisterClass(&wc)) {
        Wh_Log(L"Failed to register overlay window class: %u", GetLastError());
        return false;
    }

    g_overlayClassRegistered = true;
    return true;
}

void UnregisterOverlayWindowClass() {
    if (g_overlayClassRegistered) {
        UnregisterClass(OVERLAY_WINDOW_CLASS, GetCurrentModuleHandle());
        g_overlayClassRegistered = false;
    }
}

bool EnsureLazyInitialized() {
    if (g_lazyInitialized.exchange(true)) {
        return g_initSucceeded;
    }

    RunDxgiWorkaroundForExplorerPatcher();

    if (!InitDirectX()) {
        Wh_Log(L"InitDirectX failed");
        return false;
    }

    g_initSucceeded = true;
    return true;
}

void CreateOverlayWindow() {
    if (g_overlayWnd) {
        return;
    }

    if (!EnsureLazyInitialized()) {
        return;
    }

    HWND hWorkerW = GetWorkerW();
    if (!hWorkerW) {
        Wh_Log(L"Failed to find WorkerW");
        return;
    }

    if (!RegisterOverlayWindowClass()) {
        return;
    }

    HINSTANCE hInstance = GetCurrentModuleHandle();

    RECT rc;
    GetWindowRect(hWorkerW, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    g_overlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        OVERLAY_WINDOW_CLASS, nullptr, WS_CHILD | WS_VISIBLE, 0, 0, width,
        height, hWorkerW, nullptr, hInstance, nullptr);

    if (!g_overlayWnd) {
        Wh_Log(L"Failed to create overlay window: %u", GetLastError());
        return;
    }

    if (CreateSwapChainResources(width, height)) {
        RenderOverlay();
        ScheduleNextUpdate();
    } else {
        ReleaseSwapChainResources();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Message window for system notifications

#define MESSAGE_WINDOW_CLASS L"DesktopLiveOverlay_Message_" WH_MOD_ID

bool g_messageClassRegistered = false;

bool RegisterMessageWindowClass() {
    if (g_messageClassRegistered) {
        return true;
    }

    WNDCLASS wc = {};
    wc.lpfnWndProc = MessageWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = MESSAGE_WINDOW_CLASS;
    if (!RegisterClass(&wc)) {
        Wh_Log(L"Failed to register message window class: %u", GetLastError());
        return false;
    }

    g_messageClassRegistered = true;
    return true;
}

void UnregisterMessageWindowClass() {
    if (g_messageClassRegistered) {
        UnregisterClass(MESSAGE_WINDOW_CLASS, GetCurrentModuleHandle());
        g_messageClassRegistered = false;
    }
}

void CreateMessageWindow() {
    if (g_messageWnd) {
        return;
    }

    if (!RegisterMessageWindowClass()) {
        return;
    }

    HINSTANCE hInstance = GetCurrentModuleHandle();

    // Create a hidden top-level window (not message-only) to receive
    // WM_DISPLAYCHANGE which is only sent to top-level windows.
    g_messageWnd = CreateWindowEx(0, MESSAGE_WINDOW_CLASS, nullptr, 0, 0, 0, 0,
                                  0, nullptr, nullptr, hInstance, nullptr);
    if (!g_messageWnd) {
        Wh_Log(L"Failed to create message window: %u", GetLastError());
    }
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
    g_createOverlayTimer =
        SetTimer(nullptr, g_createOverlayTimer, 1000,
                 [](HWND, UINT, UINT_PTR idEvent, DWORD) {
                     KillTimer(nullptr, idEvent);
                     g_createOverlayTimer = 0;
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

    g_settings.refreshInterval =
        std::clamp(Wh_GetIntSetting(L"refreshInterval"), 1, 60);

    g_settings.gpuAdapterName =
        WindhawkUtils::StringSetting::make(L"gpuAdapterName");

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
        g_settings.backgroundPadding = 20;
    }

    g_settings.backgroundCornerRadius =
        Wh_GetIntSetting(L"background.cornerRadius");
    if (g_settings.backgroundCornerRadius < 0) {
        g_settings.backgroundCornerRadius = 8;
    }

    g_settings.backgroundBlur = Wh_GetIntSetting(L"background.blur");
    if (g_settings.backgroundBlur < 0) {
        g_settings.backgroundBlur = 0;
    }

    g_settings.backgroundBorderSize =
        Wh_GetIntSetting(L"background.borderSize");
    if (g_settings.backgroundBorderSize < 0) {
        g_settings.backgroundBorderSize = 0;
    }

    PCWSTR borderColor = Wh_GetStringSetting(L"background.borderColor");
    if (!ParseColor(borderColor, &g_settings.backgroundBorderColorA,
                    &g_settings.backgroundBorderColorR,
                    &g_settings.backgroundBorderColorG,
                    &g_settings.backgroundBorderColorB)) {
        g_settings.backgroundBorderColorA = 0x80;
        g_settings.backgroundBorderColorR = 0xFF;
        g_settings.backgroundBorderColorG = 0xFF;
        g_settings.backgroundBorderColorB = 0xFF;
    }
    Wh_FreeStringSetting(borderColor);

    g_settings.verticalPosition =
        std::clamp(Wh_GetIntSetting(L"verticalPosition"), 0, 100);
    g_settings.horizontalPosition =
        std::clamp(Wh_GetIntSetting(L"horizontalPosition"), 0, 100);

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

    g_systemMetricsUsed = IsSystemMetricsUsed();
    g_weatherUsed = IsWeatherUsed();

    {
        std::lock_guard<std::mutex> guard(g_weatherMutex);
        g_weatherUrl = MakeWeatherUrl();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (HMODULE hGdi32 = LoadLibraryEx(L"gdi32.dll", nullptr,
                                       LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pD3DKMTOpenAdapterFromLuid =
            (D3DKMTOpenAdapterFromLuid_t)GetProcAddress(
                hGdi32, "D3DKMTOpenAdapterFromLuid");
        pD3DKMTQueryAdapterInfo = (D3DKMTQueryAdapterInfo_t)GetProcAddress(
            hGdi32, "D3DKMTQueryAdapterInfo");
        pD3DKMTCloseAdapter =
            (D3DKMTCloseAdapter_t)GetProcAddress(hGdi32, "D3DKMTCloseAdapter");
    }

    LoadSettings();

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

    // The timer callback lives in the mod image, so it has to be gone before
    // the image is unmapped. KillTimer only works from the thread that set the
    // timer, which is the thread the desktop folder view belongs to.
    if (HWND hProgman = GetProgmanWnd()) {
        RunFromWindowThread(
            hProgman,
            [](void*) {
                if (g_createOverlayTimer) {
                    KillTimer(nullptr, g_createOverlayTimer);
                    g_createOverlayTimer = 0;
                }
            },
            nullptr);
    }

    // Destroy windows from their owning thread.
    if (g_overlayWnd) {
        SendMessage(g_overlayWnd, WM_APP_CLEANUP, 0, 0);
    }
    if (g_messageWnd) {
        SendMessage(g_messageWnd, WM_APP_CLEANUP, 0, 0);
    }

    // Unregister classes regardless of whether windows still exist.
    UnregisterOverlayWindowClass();
    UnregisterMessageWindowClass();

    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);
        WeatherUpdateThreadUninit();
        UninitMetrics();
    }

    UninitDirectX();
}

void ApplySettingsChanged() {
    Wh_Log(L">");

    // Save values needed for comparison before loading new settings.
    bool oldWeatherUsed = g_weatherUsed;
    std::wstring oldWeatherLocation = g_settings.weatherLocation.get();
    std::wstring oldWeatherFormat = g_settings.weatherFormat.get();
    WeatherUnits oldWeatherUnits = g_settings.weatherUnits;
    int oldMonitor = g_settings.monitor;

    // Load new settings.
    LoadSettings();

    // If lazy init hasn't completed successfully, just return.
    if (!g_lazyInitialized || !g_initSucceeded) {
        return;
    }

    // The metrics and the weather thread are recreated by the next FormatLine.
    {
        std::lock_guard<std::mutex> guard(g_formatLineMutex);

        UninitMetrics();

        // The thread reads the request URL from g_weatherUrl, so changed
        // weather settings only call for a new request, not a new thread.
        bool weatherSettingsChanged =
            oldWeatherLocation != g_settings.weatherLocation.get() ||
            oldWeatherFormat != g_settings.weatherFormat.get() ||
            oldWeatherUnits != g_settings.weatherUnits;
        if (oldWeatherUsed && !g_weatherUsed) {
            WeatherUpdateThreadUninit();
        } else if (g_weatherUsed && weatherSettingsChanged) {
            {
                // Don't keep showing weather for the old settings.
                std::lock_guard<std::mutex> weatherGuard(g_weatherMutex);
                g_weatherContent.reset();
            }

            WeatherUpdateThreadRefresh();
        }

        g_formattingInitialized = false;
    }

    // If overlay not created yet, skip visual updates.
    if (!g_overlayWnd) {
        return;
    }

    // Recreate text resources (fonts, brushes).
    // This is cheap enough to always do.
    ReleaseTextResources();
    RecreateTextResources();

    // Handle monitor change.
    if (oldMonitor != g_settings.monitor) {
        HandleDisplayChange();
    }

    // Reschedule timer and trigger immediate re-render.
    RenderOverlay();
    ScheduleNextUpdate();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Marshal to overlay window thread to avoid races with rendering/timers.
    if (g_overlayWnd) {
        SendMessage(g_overlayWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else {
        // No overlay yet, safe to apply directly.
        ApplySettingsChanged();
    }
}
