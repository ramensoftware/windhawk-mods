// ==WindhawkMod==
// @id              calendar-live-overlay
// @name            Calendar Live Overlay
// @description     Display a customizable monthly calendar on the desktop behind icons.
// @version         0.0.1
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldxgi -ld2d1 -ldwrite -ld3d11 -ldcomp -ldwmapi -lgdi32 -lshcore
// @license         GPL-3.0
// ==/WindhawkMod==
// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Calendar Live Overlay

Displays a customizable monthly calendar directly on the desktop behind the desktop icons.

## Preview

![Calendar Live Overlay preview 1](https://i.imgur.com/A3gcKe0.png)

![Calendar Live Overlay preview 2](https://i.imgur.com/AuCGcoP.png)


## Features

- Shows the current month as a clean desktop overlay
- Runs behind desktop icons, based on the Desktop Live Overlay method
- Designed to visually match Desktop Live Overlay Mod https://windhawk.net/mods/desktop-live-overlay
- Custom absolute X/Y positioning
- Optional percentage-based positioning
- Multi-monitor support
- Adjustable font size, font family, font weight and font style
- Custom text color with alpha transparency
- Optional German or English month/day names
- Optional Monday or Sunday week start
- Optional month header
- Optional week numbers
- Optional leading zero day numbers
- Optional box drawing border
- Adjustable border content offset
- Highlight today's date with custom color
- Optional bold style for today's date
- Optional underline for today's date
- Highlight weekends with custom color and opacity
- Optional custom color for week numbers
- Optional custom color for weekday names
- Adjustable column and row spacing
- Header alignment: left, center or right
- Optional background with color, padding, rounded corners, border and blur
- Optional background border with custom color and size
- Automatic refresh shortly after each minute change

## Usage

Enable the mod and adjust the position and appearance in the mod settings.

For best alignment, use a monospaced font such as Cascadia Mono, Consolas, JetBrains Mono, or Courier New.

## Color Format
Colors use hexadecimal format:

- `#RRGGBB`
- `#AARRGGBB`

Examples:

- `#FFFFFFFF` = white, fully visible
- `#80FFFFFF` = white, 50% transparent
- `#FFFF6060` = red, fully visible
- `#FFFFB060` = orange, fully visible
- `#30000000` = black, very transparent

In `#AARRGGBB`:

- `AA` = alpha / transparency
- `RR` = red
- `GG` = green
- `BB` = blue

## Settings

### Calendar

- Show week numbers
- Week starts on Monday or Sunday
- Use German names
- Show month header
- Leading zero day numbers
- Use box drawing border
- Border content offset
- Column spacing
- Row spacing
- Header alignment

### Highlighting

- Highlight today
- Today color
- Today bold
- Today underline
- Highlight weekends
- Weekend color
- Weekend opacity
- Highlight week numbers
- Week number color
- Highlight weekday names
- Weekday name color

### Position

- Use absolute X/Y position
- X-Position
- Y-Position
- Vertical position percent
- Horizontal position percent
- Monitor

### Font

- Font size
- Text color
- Font family
- Font weight
- Font style

For best alignment, use a monospaced font.

Recommended:

- Cascadia Mono
- Consolas
- JetBrains Mono
- Courier New

Proportional fonts such as Arial, Calibri or Verdana can distort the calendar layout because the calendar uses fixed-width text alignment.

### Background

- Enable background
- Background color
- Background padding
- Background corner radius
- Background blur
- Background border size
- Background border color

## Credits

Based on and inspired by Desktop Live Overlay by m417z.

Special thanks to m417z for the original overlay implementation and the Windhawk ecosystem.

Additional development help and debugging guidance: ChatGPT by OpenAI.

## Notes

This mod is injected into `explorer.exe` and creates the calendar overlay as part of the desktop window structure.

If the desktop does not refresh correctly after changing settings, restart Explorer or disable and re-enable the mod.

The calendar refreshes automatically shortly after each minute change, which is enough to update the highlighted day after midnight.

For best alignment, use a monospaced font such as:

- Cascadia Mono
- Consolas
- JetBrains Mono
- Courier New

The mod does not open a normal application window and does not appear in the taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- calendarFontSize: 36
  $name: Font size
  $description: "Font size in points. Values are internally limited to 16-200. Default is 36"
- calendarTextColor: "#A0FFFFFF"
  $name: Text color
  $description: "Color in #AARRGGBB or #RRGGBB format. AA is opacity in hex: FF=100%, 80≈50%, 40≈25%, 00=transparent. Default is #A0FFFFFF"
- calendarFontFamily: "Cascadia Mono"
  $name: Font family
  $description: "Recommended: Cascadia Mono, Consolas, JetBrains Mono or Courier New. Proportional fonts such as Arial, Calibri or Verdana can distort the calendar layout. Default is Cascadia Mono. Default is Cascadia Mono"
- calendarFontWeight: "Normal"
  $name: Font weight
  $description: "Default is Normal"
  $options:
  - Thin: Thin
  - Light: Light
  - Normal: Normal
  - Medium: Medium
  - SemiBold: SemiBold
  - Bold: Bold
  - ExtraBold: ExtraBold
- calendarFontStyle: "Normal"
  $name: Font style
  $description: "Default is Normal"
  $options:
  - Normal: Normal
  - Italic: Italic
  - Oblique: Oblique

- calendarShowWeekNumbers: true
  $name: Show week numbers
  $description: "Default is true"
- calendarWeekStartDay: "Monday"
  $name: Week starts on
  $description: "Default is Monday"
  $options:
  - Monday: Monday
  - Sunday: Sunday
- calendarUseGermanNames: false
  $name: Use German names
  $description: "Default is false"
- calendarShowHeader: true
  $name: Show month header
  $description: "Default is true"
- calendarLeadingZeroDayNumbers: true
  $name: Leading zero day numbers
  $description: "Show day numbers as 01, 02, 03 instead of 1, 2, 3. Default is true"
- calendarUseBoxDrawing: true
  $name: Use box drawing border
  $description: "Default is true"
- calendarBorderLeftOffset: 2
  $name: Border left offset
  $description: "Moves the calendar content right inside the box drawing border by this many spaces. Default is 2"

- calendarHighlightToday: true
  $name: Highlight today
  $description: "Default is true"
- calendarTodayColor: "#FFFF6060"
  $name: Today color
  $description: "#AARRGGBB or #RRGGBB. Default is #FFFF6060"
- calendarTodayBold: true
  $name: Today bold
  $description: "Default is true"
- calendarTodayUnderline: true
  $name: Today underline
  $description: "Underline today's date. Uses the same color as today's date. Default is true"

- calendarHighlightWeekends: true
  $name: Highlight weekends
  $description: "Default is true"
- calendarWeekendColor: "#FFFFB060"
  $name: Weekend color
  $description: "Color in #AARRGGBB or #RRGGBB format. AA is opacity in hex. Weekend opacity percent multiplies this alpha value. Default is #FFFFB060"
- calendarWeekendOpacity: 100
  $name: Weekend opacity percent
  $description: "Multiplies the alpha value of the weekend color. 100 keeps the color unchanged, 50 makes it half as opaque. Default is 100"

- calendarHighlightWeekNumbers: true
  $name: Highlight week numbers
  $description: "Default is true"
- calendarWeekNumberColor: "#80B8E8FF"
  $name: Week number color
  $description: "Color for week number label and week numbers in #AARRGGBB or #RRGGBB format. Default is #80B8E8FF"

- calendarHighlightDayNames: true
  $name: Highlight day names
  $description: "Default is true"
- calendarDayNameColor: "#80FFF0B0"
  $name: Day name color
  $description: "Color for weekday names in #AARRGGBB or #RRGGBB format. Default is #A0FFF0B0"

- calendarColumnSpacing: 2
  $name: Column spacing
  $description: "Spaces between calendar columns. Default is 2"
- calendarRowSpacing: 0
  $name: Row spacing
  $description: "Extra empty lines between calendar rows. Default is 0"
- calendarHeaderAlignment: "Center"
  $name: Header alignment
  $description: "Default is Center"
  $options:
  - Left: Left
  - Center: Center
  - Right: Right

- useAbsolutePosition: false
  $name: Use absolute X/Y position
  $description: "When enabled, X-Position and Y-Position are used. When disabled, horizontal and vertical percentage positions are used. Default is Off."
- xPosition: 220
  $name: X-Position
  $description: "Default is 220"
- yPosition: 120
  $name: Y-Position
  $description: "Default is 120"
- verticalPosition: 4
  $name: Vertical position percent
  $description: "Default is 4"
- horizontalPosition: 96
  $name: Horizontal position percent
  $description: "Default is 96"
- monitor: 1
  $name: Monitor
  $description: "Monitor number starting from 1. Usually 1 is the primary display. Use 2, 3, etc. for additional monitors. Values are limited to 1-16."

- backgroundEnabled: false
  $name: Enable background
  $description: "Default is false"
- backgroundColor: "#30000000"
  $name: Background color
  $description: "Default is #30000000"
- backgroundPadding: 20
  $name: Background padding
  $description: "Default is 20"
- backgroundCornerRadius: 16
  $name: Background corner radius
  $description: "Default is 16"
- backgroundBlur: 0
  $name: Background blur
  $description: "Default is 0"
- backgroundBorderSize: 0
  $name: Background border size
  $description: "Default is 0"
- backgroundBorderColor: "#50FFFFFF"
  $name: Background border color
  $description: "Default is #50FFFFFF"
*/
// ==/WindhawkModSettings==

// Credits:
// This mod is based on and inspired by Desktop Live Overlay by m417z.
// Parts of the desktop overlay / WorkerW rendering approach were adapted
// from Desktop Live Overlay.

#include <windhawk_utils.h>

#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi1_3.h>
#include <shellscalingapi.h>
#include <wrl/client.h>
#include <algorithm>
#include <atomic>
#include <functional>
#include <string>
#include <string_view>
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

int ClampInt(int value, int minValue, int maxValue) {
    return (std::max)(minValue, (std::min)(maxValue, value));
}

struct LineSettings {
    int fontSize;
    BYTE colorA;
    BYTE colorR;
    BYTE colorG;
    BYTE colorB;
    WindhawkUtils::StringSetting fontFamily;
    int fontWeight;
    DWRITE_FONT_STYLE fontStyle;
};

struct Settings {
    LineSettings topLine;
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
    bool useAbsolutePosition;
    int xPosition;
    int yPosition;
    int monitor;
    bool calendarShowWeekNumbers;
    bool calendarWeekStartsMonday;
    bool calendarUseGermanNames;
    bool calendarShowHeader;
    bool calendarLeadingZeroDayNumbers;
    bool calendarUseBoxDrawing;
    bool calendarHighlightToday;
    BYTE calendarTodayColorA;
    BYTE calendarTodayColorR;
    BYTE calendarTodayColorG;
    BYTE calendarTodayColorB;
    bool calendarTodayBold;
    bool calendarTodayUnderline;
    bool calendarHighlightWeekends;
    BYTE calendarWeekendColorA;
    BYTE calendarWeekendColorR;
    BYTE calendarWeekendColorG;
    BYTE calendarWeekendColorB;
    bool calendarHighlightWeekNumbers;
    BYTE calendarWeekNumberColorA;
    BYTE calendarWeekNumberColorR;
    BYTE calendarWeekNumberColorG;
    BYTE calendarWeekNumberColorB;
    bool calendarHighlightDayNames;
    BYTE calendarDayNameColorA;
    BYTE calendarDayNameColorR;
    BYTE calendarDayNameColorG;
    BYTE calendarDayNameColorB;
    int calendarColumnSpacing;
    int calendarRowSpacing;
    int calendarBorderLeftOffset;
    enum class CalendarHeaderAlignment { Left, Center, Right } calendarHeaderAlignment;
    int calendarWeekendOpacity;
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

// Initialization/unloading state.
std::atomic<bool> g_lazyInitialized{false};
std::atomic<bool> g_initSucceeded{false};
std::atomic<bool> g_unloading{false};

// Format state.
SYSTEMTIME g_formatTime;
DWORD g_formatIndex = 0;

FormattedString<FORMATTED_BUFFER_SIZE> g_weekdayFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weekdayNumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_weeknumFormatted;
FormattedString<INTEGER_BUFFER_SIZE> g_dayOfYearFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_timezoneFormatted;

struct CalendarColorRange {
    UINT32 start;
    UINT32 length;
};
std::vector<CalendarColorRange> g_calendarTodayRanges;
std::vector<CalendarColorRange> g_calendarWeekendRanges;
std::vector<CalendarColorRange> g_calendarWeekNumberRanges;
std::vector<CalendarColorRange> g_calendarDayNameRanges;

// Last known wallpaper file time for change detection.
FILETIME g_lastWallpaperTime = {};

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
ComPtr<ID2D1SolidColorBrush> g_backgroundBrush;
ComPtr<ID2D1SolidColorBrush> g_borderBrush;
ComPtr<ID2D1Bitmap> g_wallpaperBitmap;
ComPtr<ID2D1Effect> g_blurEffect;

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

static bool StrEqSafe(PCWSTR a, PCWSTR b) {
    return a && b && wcscmp(a, b) == 0;
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

PCWSTR GetWeekdayFormatted() {
    if (g_weekdayFormatted.formatIndex != g_formatIndex) {
        GetDateFormat(LOCALE_USER_DEFAULT, DATE_AUTOLAYOUT, &g_formatTime,
                      L"dddd", g_weekdayFormatted.buffer,
                      ARRAYSIZE(g_weekdayFormatted.buffer));
        g_weekdayFormatted.formatIndex = g_formatIndex;
    }
    return g_weekdayFormatted.buffer;
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


////////////////////////////////////////////////////////////////////////////////
// Calendar formatting

int CalendarGetDayOfWeek(int year, int month, int day) {
    // Zeller's congruence. Return 0=Sunday ... 6=Saturday.
    if (month < 3) {
        month += 12;
        year--;
    }
    int q = day;
    int m = month;
    int k = year % 100;
    int j = year / 100;
    int h = (q + ((13 * (m + 1)) / 5) + k + (k / 4) + (j / 4) -
             (2 * j)) % 7;
    return (h + 6) % 7;
}

int CalendarGetDaysInMonth(int year, int month) {
    static const int daysPerMonth[] = {31, 28, 31, 30, 31, 30,
                                       31, 31, 30, 31, 30, 31};
    if (month == 2 &&
        ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))) {
        return 29;
    }
    return daysPerMonth[month - 1];
}

int CalendarDayOfYear(int year, int month, int day) {
    int result = day;
    for (int m = 1; m < month; m++) {
        result += CalendarGetDaysInMonth(year, m);
    }
    return result;
}

int CalendarIsoWeeksInYear(int year) {
    int jan1 = CalendarGetDayOfWeek(year, 1, 1);
    bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    return (jan1 == 4 || (jan1 == 3 && leap)) ? 53 : 52;
}

int CalendarIsoWeekNumber(int year, int month, int day) {
    int dow = CalendarGetDayOfWeek(year, month, day); // Sunday=0
    int isoDow = dow == 0 ? 7 : dow;                 // Monday=1 ... Sunday=7
    int doy = CalendarDayOfYear(year, month, day);
    int week = (doy - isoDow + 10) / 7;
    if (week < 1) {
        return CalendarIsoWeeksInYear(year - 1);
    }
    int weeksThisYear = CalendarIsoWeeksInYear(year);
    if (week > weeksThisYear) {
        return 1;
    }
    return week;
}

std::wstring CalendarSpaces(int count) {
    return std::wstring((std::max)(0, count), L' ');
}

std::wstring CalendarAlignLine(const std::wstring& text, int width) {
    int len = static_cast<int>(text.size());
    if (len >= width) {
        return text;
    }

    int extra = width - len;
    switch (g_settings.calendarHeaderAlignment) {
        case Settings::CalendarHeaderAlignment::Left:
            return text + CalendarSpaces(extra);
        case Settings::CalendarHeaderAlignment::Right:
            return CalendarSpaces(extra) + text;
        case Settings::CalendarHeaderAlignment::Center:
        default:
            return CalendarSpaces(extra / 2) + text + CalendarSpaces(extra - extra / 2);
    }
}

void CalendarAppendRowSpacing(std::wstring& out) {
    int rowSpacing = (std::max)(0, (std::min)(5, g_settings.calendarRowSpacing));
    for (int i = 0; i < rowSpacing; i++) {
        out += L"\n";
    }
}

PCWSTR GetCalendarFormatted() {
    static DWORD calendarFormatIndex = 0;
    static WCHAR calendarBuffer[4096] = {};

    if (calendarFormatIndex == g_formatIndex) {
        return calendarBuffer;
    }

    SYSTEMTIME now;
    GetLocalTime(&now);

    const WCHAR* monthNamesDe[] = {L"Januar", L"Februar", L"März",
                                   L"April", L"Mai", L"Juni",
                                   L"Juli", L"August", L"September",
                                   L"Oktober", L"November", L"Dezember"};
    const WCHAR* monthNamesEn[] = {L"January", L"February", L"March",
                                   L"April", L"May", L"June",
                                   L"July", L"August", L"September",
                                   L"October", L"November", L"December"};
    const WCHAR* dayNamesMonDe[] = {L"Mo", L"Di", L"Mi", L"Do", L"Fr", L"Sa", L"So"};
    const WCHAR* dayNamesSunDe[] = {L"So", L"Mo", L"Di", L"Mi", L"Do", L"Fr", L"Sa"};
    const WCHAR* dayNamesMonEn[] = {L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa", L"Su"};
    const WCHAR* dayNamesSunEn[] = {L"Su", L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa"};

    const WCHAR** monthNames = g_settings.calendarUseGermanNames ? monthNamesDe : monthNamesEn;
    const WCHAR** dayNames = g_settings.calendarUseGermanNames
                                  ? (g_settings.calendarWeekStartsMonday ? dayNamesMonDe : dayNamesSunDe)
                                  : (g_settings.calendarWeekStartsMonday ? dayNamesMonEn : dayNamesSunEn);

    int year = now.wYear;
    int month = now.wMonth;
    int today = now.wDay;
    int daysInMonth = CalendarGetDaysInMonth(year, month);
    int firstDow = CalendarGetDayOfWeek(year, month, 1); // Sunday=0
    int firstCol = g_settings.calendarWeekStartsMonday ? (firstDow + 6) % 7 : firstDow;

    int borderLeftOffset = (std::max)(0, (std::min)(10, g_settings.calendarBorderLeftOffset));
    std::wstring contentPrefix = g_settings.calendarUseBoxDrawing
                                     ? CalendarSpaces(borderLeftOffset)
                                     : L"";

    std::wstring out;
    g_calendarTodayRanges.clear();
    g_calendarWeekendRanges.clear();
    g_calendarWeekNumberRanges.clear();
    g_calendarDayNameRanges.clear();

    auto appendf = [&out](PCWSTR fmt, auto... args) {
        WCHAR tmp[256];
        swprintf_s(tmp, fmt, args...);
        out += tmp;
    };

    int columnSpacing = (std::max)(1, (std::min)(8, g_settings.calendarColumnSpacing));
    std::wstring colSpace = CalendarSpaces(columnSpacing);
    int visibleColumns = 7 + (g_settings.calendarShowWeekNumbers ? 1 : 0);
    int contentWidth = visibleColumns * 2 + (visibleColumns - 1) * columnSpacing;
    int boxWidth = contentWidth + (g_settings.calendarUseBoxDrawing ? borderLeftOffset : 0);

    auto appendGapIfNeeded = [&out, &colSpace](int index, int total) {
        if (index + 1 < total) {
            out += colSpace;
        }
    };

    auto appendCell = [&out](PCWSTR cellText) {
        out += cellText;
    };

    if (g_settings.calendarUseBoxDrawing) {
        out += L"┌";
        for (int i = 0; i < boxWidth; i++) out += L"─";
        out += L"┐\n";
    }

    if (g_settings.calendarShowHeader) {
        WCHAR title[128];
        swprintf_s(title, L"%s %04d", monthNames[month - 1], year);
        out += contentPrefix;
        out += CalendarAlignLine(title, contentWidth);
        out += L"\n";
    }

    out += contentPrefix;

    int headerCol = 0;
    if (g_settings.calendarShowWeekNumbers) {
        UINT32 rangeStart = (UINT32)out.size();
        appendCell(g_settings.calendarUseGermanNames ? L"KW" : L"Wk");
        g_calendarWeekNumberRanges.push_back({rangeStart, 2});
        appendGapIfNeeded(headerCol++, visibleColumns);
    }

    for (int i = 0; i < 7; i++) {
        UINT32 rangeStart = (UINT32)out.size();
        appendCell(dayNames[i]);
        g_calendarDayNameRanges.push_back({rangeStart, 2});
        appendGapIfNeeded(headerCol++, visibleColumns);
    }
    out += L"\n";
    CalendarAppendRowSpacing(out);

    int day = 1;
    for (int row = 0; row < 6 && day <= daysInMonth; row++) {
        int firstDayInRow = row * 7 - firstCol + 1;
        int weekDay = firstDayInRow < 1 ? 1 : firstDayInRow;
        int rowCol = 0;

        out += contentPrefix;

        if (g_settings.calendarShowWeekNumbers) {
            UINT32 rangeStart = (UINT32)out.size();

            appendf(L"%02d", CalendarIsoWeekNumber(year, month, weekDay));

            g_calendarWeekNumberRanges.push_back({rangeStart, 2});
            appendGapIfNeeded(rowCol++, visibleColumns);
        }

        for (int col = 0; col < 7; col++) {
            if (row == 0 && col < firstCol) {
                out += L"  ";
            } else if (day <= daysInMonth) {
            UINT32 rangeStart = (UINT32)out.size();

            if (g_settings.calendarLeadingZeroDayNumbers) {
                appendf(L"%02d", day);
            } else {
                appendf(L"%2d", day);

            if (day < 10) {
                rangeStart++;
            }
        }

        UINT32 rangeLength = (!g_settings.calendarLeadingZeroDayNumbers && day < 10) ? 1 : 2;

                int actualDow = CalendarGetDayOfWeek(year, month, day); // 0=Sunday, 6=Saturday
                bool isToday = (day == today);
                bool isWeekend = (actualDow == 0 || actualDow == 6);

                if (isToday) {
                    g_calendarTodayRanges.push_back({rangeStart, rangeLength});
                }

                if (isWeekend && !(isToday && g_settings.calendarHighlightToday)) {
                    g_calendarWeekendRanges.push_back({rangeStart, rangeLength});
                }

                day++;
            } else {
                out += L"  ";
            }

            appendGapIfNeeded(rowCol++, visibleColumns);
        }
        out += L"\n";
        CalendarAppendRowSpacing(out);
    }

    if (g_settings.calendarUseBoxDrawing) {
        out += L"└";
        for (int i = 0; i < boxWidth; i++) out += L"─";
        out += L"┘";
    }

    bool truncated = false;
    StringCopyTruncated(calendarBuffer, ARRAYSIZE(calendarBuffer), out.c_str(), &truncated);
    calendarFormatIndex = g_formatIndex;
    return calendarBuffer;
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
        {L"%weekday%"sv, GetWeekdayFormatted},
        {L"%weekday_num%"sv, GetWeekdayNumFormatted},
        {L"%weeknum%"sv, GetWeeknumFormatted},
        {L"%dayofyear%"sv, GetDayOfYearFormatted},
        {L"%timezone%"sv, GetTimezoneFormatted},
        {L"%calendar%"sv, GetCalendarFormatted},
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

    // Ensure Progman is in the current process.
    DWORD progmanProcessId = 0;
    GetWindowThreadProcessId(hProgman, &progmanProcessId);
    if (progmanProcessId != GetCurrentProcessId()) {
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
    HRESULT hr;

    // Create top line text format.
    PCWSTR topFontFamily = g_settings.topLine.fontFamily.get();
    if (!topFontFamily || !*topFontFamily) {
        topFontFamily = L"Cascadia Mono";
    }

    hr = g_dwriteFactory->CreateTextFormat(
        topFontFamily, nullptr,
        GetDWriteFontWeight(g_settings.topLine.fontWeight),
        g_settings.topLine.fontStyle,
        DWRITE_FONT_STRETCH_NORMAL,
        (FLOAT)g_settings.topLine.fontSize * g_dpiScale,
        L"",
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


void ApplyCalendarColorRanges(IDWriteTextLayout* layout, PCWSTR formattedText) {
    if (!layout || !formattedText || wcscmp(formattedText, GetCalendarFormatted()) != 0) {
        return;
    }

    ComPtr<ID2D1SolidColorBrush> weekNumberBrush;
    ComPtr<ID2D1SolidColorBrush> dayNameBrush;
    ComPtr<ID2D1SolidColorBrush> todayBrush;
    ComPtr<ID2D1SolidColorBrush> weekendBrush;

    if (g_settings.calendarHighlightWeekNumbers && !g_calendarWeekNumberRanges.empty()) {
        g_dc->CreateSolidColorBrush(
            D2D1::ColorF(g_settings.calendarWeekNumberColorR / 255.0f,
                        g_settings.calendarWeekNumberColorG / 255.0f,
                        g_settings.calendarWeekNumberColorB / 255.0f,
                        g_settings.calendarWeekNumberColorA / 255.0f),
            &weekNumberBrush);

        if (weekNumberBrush) {
            for (const auto& r : g_calendarWeekNumberRanges) {
                layout->SetDrawingEffect(weekNumberBrush.Get(), {r.start, r.length});
            }
        }
    }

    if (g_settings.calendarHighlightDayNames && !g_calendarDayNameRanges.empty()) {
        g_dc->CreateSolidColorBrush(
            D2D1::ColorF(g_settings.calendarDayNameColorR / 255.0f,
                        g_settings.calendarDayNameColorG / 255.0f,
                        g_settings.calendarDayNameColorB / 255.0f,
                        g_settings.calendarDayNameColorA / 255.0f),
            &dayNameBrush);

        if (dayNameBrush) {
            for (const auto& r : g_calendarDayNameRanges) {
                layout->SetDrawingEffect(dayNameBrush.Get(), {r.start, r.length});
            }
        }
    }

    if (!g_calendarTodayRanges.empty()) {
        if (g_settings.calendarHighlightToday) {
            g_dc->CreateSolidColorBrush(
                D2D1::ColorF(g_settings.calendarTodayColorR / 255.0f,
                             g_settings.calendarTodayColorG / 255.0f,
                             g_settings.calendarTodayColorB / 255.0f,
                             g_settings.calendarTodayColorA / 255.0f),
                &todayBrush);
        }

        for (const auto& r : g_calendarTodayRanges) {
            DWRITE_TEXT_RANGE range{r.start, r.length};

            if (todayBrush) {
                layout->SetDrawingEffect(todayBrush.Get(), range);
            }

            if (g_settings.calendarTodayBold) {
                layout->SetFontWeight(DWRITE_FONT_WEIGHT_BLACK, range);
            }

            if (g_settings.calendarTodayUnderline) {
                layout->SetUnderline(TRUE, range);
            }
        }
    }

    if (g_settings.calendarHighlightWeekends && !g_calendarWeekendRanges.empty()) {
        int weekendOpacity = (std::max)(0, (std::min)(100, g_settings.calendarWeekendOpacity));
        float weekendAlpha = (g_settings.calendarWeekendColorA / 255.0f) * (weekendOpacity / 100.0f);

        g_dc->CreateSolidColorBrush(
            D2D1::ColorF(g_settings.calendarWeekendColorR / 255.0f,
                         g_settings.calendarWeekendColorG / 255.0f,
                         g_settings.calendarWeekendColorB / 255.0f,
                         weekendAlpha),
            &weekendBrush);

        if (weekendBrush) {
            for (const auto& r : g_calendarWeekendRanges) {
                layout->SetDrawingEffect(weekendBrush.Get(), {r.start, r.length});
            }
        }
    }
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
    bool topLineIsCalendar = true;

    WCHAR formattedTopText[8192] = {};
    bool truncated = false;
    StringCopyTruncated(
        formattedTopText,
        ARRAYSIZE(formattedTopText),
        GetCalendarFormatted(),
        &truncated
    );

    bool hasTopLine = *formattedTopText && g_topLineTextFormat;

    if (hasTopLine) {
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
            float topWidth = 0, topHeight = 0;

            if (hasTopLine) {
                g_dwriteFactory->CreateTextLayout(
                    formattedTopText, (UINT32)wcslen(formattedTopText),
                    g_topLineTextFormat.Get(), (FLOAT)width, (FLOAT)height,
                    &topLayout);
                if (topLayout) {
                    ApplyCalendarColorRanges(topLayout.Get(), formattedTopText);
                    if (topLineIsCalendar) {
                        topLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    }
                    DWRITE_TEXT_METRICS metrics;
                    topLayout->GetMetrics(&metrics);
                    topWidth = topLineIsCalendar
                                   ? metrics.widthIncludingTrailingWhitespace
                                   : metrics.width;
                    topHeight = metrics.height;
                    topLayout->SetMaxWidth(topWidth);
                }
            }

              // Calculate combined dimensions.
            float totalWidth = topWidth;
            float totalHeight = topHeight;
            float workWidth = (float)(workArea.right - workArea.left);
            float workHeight = (float)(workArea.bottom - workArea.top);

            // Calculate position for the combined block.
            float blockX;
            float blockY;
            if (g_settings.useAbsolutePosition) {
                blockX = workArea.left + (float)g_settings.xPosition * g_dpiScale;
                blockY = workArea.top + (float)g_settings.yPosition * g_dpiScale;
            } else {
                blockX = workArea.left + (workWidth - totalWidth) *
                                        (g_settings.horizontalPosition / 100.0f);
                blockY = workArea.top + (workHeight - totalHeight) *
                                       (g_settings.verticalPosition / 100.0f);
            }

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
        }
    }

    g_dc->EndDraw();
    g_swapChain->Present(1, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Refresh timer

UINT GetNextUpdateTimeout() {
    SYSTEMTIME time;
    GetLocalTime(&time);

    // Add a small extra delay to make sure we are past the second/minute
    // change.
    constexpr UINT kExtraDelayMs = 200;

    // Refresh shortly after each minute change.
    // This is enough for the calendar and updates today's highlight after midnight.
    return (60 - time.wSecond) * 1000 - time.wMilliseconds + kExtraDelayMs;
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
        CreateSwapChainResources(rc.right - rc.left, rc.bottom - rc.top);
        RenderOverlay();
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
    }
}

////////////////////////////////////////////////////////////////////////////////
// Message window for system notifications

#define MESSAGE_WINDOW_CLASS L"CalendarLiveOverlay_Message_" WH_MOD_ID

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

    swprintf_s(settingName, L"%sText", prefix);
    swprintf_s(settingName, L"%sFontSize", prefix);
    line.fontSize = Wh_GetIntSetting(settingName);
    if (line.fontSize <= 0) {
        line.fontSize = defaultFontSize;
    }

    swprintf_s(settingName, L"%sTextColor", prefix);
    PCWSTR textColor = Wh_GetStringSetting(settingName);
    if (!ParseColor(textColor, &line.colorA, &line.colorR, &line.colorG,
                    &line.colorB)) {
        line.colorA = 0xC0;
        line.colorR = 0xFF;
        line.colorG = 0xFF;
        line.colorB = 0xFF;
    }
    Wh_FreeStringSetting(textColor);

    swprintf_s(settingName, L"%sFontFamily", prefix);
    line.fontFamily = WindhawkUtils::StringSetting::make(settingName);

    swprintf_s(settingName, L"%sFontWeight", prefix);
    PCWSTR fontWeight = Wh_GetStringSetting(settingName);
    line.fontWeight = 400;
    if (StrEqSafe(fontWeight, L"Thin")) {
        line.fontWeight = 100;
    } else if (StrEqSafe(fontWeight, L"Light")) {
        line.fontWeight = 300;
    } else if (StrEqSafe(fontWeight, L"Normal")) {
        line.fontWeight = 400;
    } else if (StrEqSafe(fontWeight, L"Medium")) {
        line.fontWeight = 500;
    } else if (StrEqSafe(fontWeight, L"SemiBold")) {
        line.fontWeight = 600;
    } else if (StrEqSafe(fontWeight, L"Bold")) {
        line.fontWeight = 700;
    } else if (StrEqSafe(fontWeight, L"ExtraBold")) {
        line.fontWeight = 800;
    }
    Wh_FreeStringSetting(fontWeight);

    swprintf_s(settingName, L"%sFontStyle", prefix);
    PCWSTR fontStyle = Wh_GetStringSetting(L"calendarFontStyle");

if (StrEqSafe(fontStyle, L"Italic")) {
    g_settings.topLine.fontStyle = DWRITE_FONT_STYLE_ITALIC;
} else if (StrEqSafe(fontStyle, L"Oblique")) {
    g_settings.topLine.fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
} else {
    g_settings.topLine.fontStyle = DWRITE_FONT_STYLE_NORMAL;
}
    Wh_FreeStringSetting(fontStyle);
}

void LoadSettings() {
    LoadLineSettings(g_settings.topLine, L"calendar", 36);

    g_settings.topLine.fontSize =
    ClampInt(Wh_GetIntSetting(L"calendarFontSize"), 16, 200);

    g_settings.backgroundEnabled = Wh_GetIntSetting(L"backgroundEnabled");

    PCWSTR backgroundColor = Wh_GetStringSetting(L"backgroundColor");
    if (!ParseColor(backgroundColor, &g_settings.backgroundColorA,
                    &g_settings.backgroundColorR, &g_settings.backgroundColorG,
                    &g_settings.backgroundColorB)) {
        g_settings.backgroundColorA = 0x80;
        g_settings.backgroundColorR = 0x00;
        g_settings.backgroundColorG = 0x00;
        g_settings.backgroundColorB = 0x00;
    }
    Wh_FreeStringSetting(backgroundColor);

    g_settings.backgroundPadding = Wh_GetIntSetting(L"backgroundPadding");
    if (g_settings.backgroundPadding < 0) {
        g_settings.backgroundPadding = 10;
    }

    g_settings.backgroundCornerRadius =
        Wh_GetIntSetting(L"backgroundCornerRadius");
    if (g_settings.backgroundCornerRadius < 0) {
        g_settings.backgroundCornerRadius = 8;
    }

    g_settings.backgroundBlur = Wh_GetIntSetting(L"backgroundBlur");
    if (g_settings.backgroundBlur < 0) {
        g_settings.backgroundBlur = 0;
    }

    g_settings.backgroundBorderSize =
        Wh_GetIntSetting(L"backgroundBorderSize");
    if (g_settings.backgroundBorderSize < 0) {
        g_settings.backgroundBorderSize = 0;
    }

    PCWSTR borderColor = Wh_GetStringSetting(L"backgroundBorderColor");
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

    g_settings.verticalPosition = Wh_GetIntSetting(L"verticalPosition");
    g_settings.horizontalPosition = Wh_GetIntSetting(L"horizontalPosition");

    g_settings.useAbsolutePosition = Wh_GetIntSetting(L"useAbsolutePosition");
    g_settings.xPosition = Wh_GetIntSetting(L"xPosition");
    g_settings.yPosition = Wh_GetIntSetting(L"yPosition");

    g_settings.calendarHighlightWeekNumbers =
        Wh_GetIntSetting(L"calendarHighlightWeekNumbers");

    PCWSTR weekNumberColor = Wh_GetStringSetting(L"calendarWeekNumberColor");
    if (!ParseColor(weekNumberColor,
                    &g_settings.calendarWeekNumberColorA,
                    &g_settings.calendarWeekNumberColorR,
                    &g_settings.calendarWeekNumberColorG,
                    &g_settings.calendarWeekNumberColorB)) {
        g_settings.calendarWeekNumberColorA = 0xA0;
        g_settings.calendarWeekNumberColorR = 0xB8;
        g_settings.calendarWeekNumberColorG = 0xE8;
        g_settings.calendarWeekNumberColorB = 0xFF;
    }
    Wh_FreeStringSetting(weekNumberColor);

    g_settings.calendarHighlightDayNames =
        Wh_GetIntSetting(L"calendarHighlightDayNames");

    PCWSTR dayNameColor = Wh_GetStringSetting(L"calendarDayNameColor");
    if (!ParseColor(dayNameColor,
                    &g_settings.calendarDayNameColorA,
                    &g_settings.calendarDayNameColorR,
                    &g_settings.calendarDayNameColorG,
                    &g_settings.calendarDayNameColorB)) {
        g_settings.calendarDayNameColorA = 0xA0;
        g_settings.calendarDayNameColorR = 0xFF;
        g_settings.calendarDayNameColorG = 0xF0;
        g_settings.calendarDayNameColorB = 0xB0;
    }
    Wh_FreeStringSetting(dayNameColor);

    g_settings.calendarShowWeekNumbers = Wh_GetIntSetting(L"calendarShowWeekNumbers");
    PCWSTR weekStartDay = Wh_GetStringSetting(L"calendarWeekStartDay");
    g_settings.calendarWeekStartsMonday = !StrEqSafe(weekStartDay, L"Sunday");
    Wh_FreeStringSetting(weekStartDay);
    g_settings.calendarUseGermanNames = Wh_GetIntSetting(L"calendarUseGermanNames");
    g_settings.calendarShowHeader = Wh_GetIntSetting(L"calendarShowHeader");
    g_settings.calendarUseBoxDrawing = Wh_GetIntSetting(L"calendarUseBoxDrawing");
    g_settings.calendarBorderLeftOffset = Wh_GetIntSetting(L"calendarBorderLeftOffset");
    g_settings.calendarHighlightToday = Wh_GetIntSetting(L"calendarHighlightToday");
    PCWSTR todayColor = Wh_GetStringSetting(L"calendarTodayColor");
    if (!ParseColor(todayColor, &g_settings.calendarTodayColorA,
                    &g_settings.calendarTodayColorR,
                    &g_settings.calendarTodayColorG,
                    &g_settings.calendarTodayColorB)) {
        g_settings.calendarTodayColorA = 0xFF;
        g_settings.calendarTodayColorR = 0xFF;
        g_settings.calendarTodayColorG = 0x60;
        g_settings.calendarTodayColorB = 0x60;
    }
    Wh_FreeStringSetting(todayColor);

    g_settings.calendarTodayBold = Wh_GetIntSetting(L"calendarTodayBold");
    g_settings.calendarTodayUnderline = Wh_GetIntSetting(L"calendarTodayUnderline");

    g_settings.calendarHighlightWeekends = Wh_GetIntSetting(L"calendarHighlightWeekends");
    PCWSTR weekendColor = Wh_GetStringSetting(L"calendarWeekendColor");
    if (!ParseColor(weekendColor, &g_settings.calendarWeekendColorA,
                    &g_settings.calendarWeekendColorR,
                    &g_settings.calendarWeekendColorG,
                    &g_settings.calendarWeekendColorB)) {
        g_settings.calendarWeekendColorA = 0xFF;
        g_settings.calendarWeekendColorR = 0xFF;
        g_settings.calendarWeekendColorG = 0xB0;
        g_settings.calendarWeekendColorB = 0x60;
    }
    Wh_FreeStringSetting(weekendColor);

    g_settings.calendarColumnSpacing = Wh_GetIntSetting(L"calendarColumnSpacing");
    if (g_settings.calendarColumnSpacing < 1) {
        g_settings.calendarColumnSpacing = 1;
    } else if (g_settings.calendarColumnSpacing > 8) {
        g_settings.calendarColumnSpacing = 8;
    }

    g_settings.calendarRowSpacing = Wh_GetIntSetting(L"calendarRowSpacing");
    if (g_settings.calendarRowSpacing < 0) {
        g_settings.calendarRowSpacing = 0;
    } else if (g_settings.calendarRowSpacing > 5) {
        g_settings.calendarRowSpacing = 5;
    }

    PCWSTR headerAlignment = Wh_GetStringSetting(L"calendarHeaderAlignment");
    if (StrEqSafe(headerAlignment, L"Left")) {
        g_settings.calendarHeaderAlignment = Settings::CalendarHeaderAlignment::Left;
    } else if (StrEqSafe(headerAlignment, L"Right")) {
        g_settings.calendarHeaderAlignment = Settings::CalendarHeaderAlignment::Right;
    } else {
        g_settings.calendarHeaderAlignment = Settings::CalendarHeaderAlignment::Center;
    }
    Wh_FreeStringSetting(headerAlignment);

    g_settings.calendarLeadingZeroDayNumbers = Wh_GetIntSetting(L"calendarLeadingZeroDayNumbers");

    g_settings.calendarWeekendOpacity = Wh_GetIntSetting(L"calendarWeekendOpacity");
    if (g_settings.calendarWeekendOpacity < 0) {
        g_settings.calendarWeekendOpacity = 0;
    } else if (g_settings.calendarWeekendOpacity > 100) {
        g_settings.calendarWeekendOpacity = 100;
    }

    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    if (g_settings.monitor < 1) {
        g_settings.monitor = 1;
    }
    if (g_settings.monitor > 16) {
        g_settings.monitor = 16;
    }

    PCWSTR fontStyleSetting = Wh_GetStringSetting(L"calendarFontStyle");
    if (StrEqSafe(fontStyleSetting, L"Italic")) {
        g_settings.topLine.fontStyle = DWRITE_FONT_STYLE_ITALIC;
    } else if (StrEqSafe(fontStyleSetting, L"Oblique")) {
        g_settings.topLine.fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
    } else {
        g_settings.topLine.fontStyle = DWRITE_FONT_STYLE_NORMAL;
    }
    Wh_FreeStringSetting(fontStyleSetting);
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L">");

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
    UninitDirectX();
}

void ApplySettingsChanged() {
    Wh_Log(L">");

    // Save values needed for comparison before loading new settings.
    int oldMonitor = g_settings.monitor;

    // Load new settings.
    LoadSettings();

    // If lazy init hasn't completed successfully, just return.
    if (!g_lazyInitialized || !g_initSucceeded) {
        return;
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
