// ==WindhawkMod==
// @id              explorer-title-bar-label
// @name            Explorer Title Bar Label
// @description     Add custom text, date and time to the Windows 11 File Explorer title bar.
// @version         1.0.0
// @author          digitalART
// @github          https://github.com/digart11
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Explorer Title Bar Label

Add custom text, date and time to the right side of the Windows 11 File Explorer title bar.

![Explorer Title Bar Label](https://raw.githubusercontent.com/digart11/explorer-title-bar-label/main/images/screenshot.png)

## Features

- Custom text, date and time
- Flexible date display
- 12-hour or 24-hour time with optional seconds
- Font, size, weight and color
- Opacity and spacing
- Live updates

## Date and time

Choose the date parts and order you prefer:

- **Weekday:** None, Mon, Monday
- **Day number:** 1, 01
- **Month:** 8, 08, Aug, August
- **Year:** None, 26, 2026
- **Date order:** Month-Day-Year, Day-Month-Year, Year-Month-Day
- **Numeric separator:** `/`, `-`, `.`

Time can use **12-hour or 24-hour format**, with optional seconds.

## Compatibility

This mod uses XAML diagnostics and may conflict with other File Explorer mods that also use XAML diagnostics.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- customText: ""
  $name: Custom text
  $description: "Optional text displayed before the date and time."

- showDate: true
  $name: Show date

- dateWeekday: short
  $name: Weekday
  $options:
  - none: None
  - short: Mon
  - long: Monday

- dateDay: number
  $name: Day number
  $options:
  - number: "1"
  - twoDigit: "01"

- dateMonth: short
  $name: Month
  $options:
  - number: "8"
  - twoDigit: "08"
  - short: Aug
  - long: August

- dateYear: none
  $name: Year
  $options:
  - none: None
  - short: "26"
  - long: "2026"

- dateOrder: mdy
  $name: Date order
  $options:
  - mdy: Month - Day - Year
  - dmy: Day - Month - Year
  - ymd: Year - Month - Day

- numericDateSeparator: slash
  $name: Numeric date separator
  $options:
  - slash: "/"
  - dash: "-"
  - dot: "."

- showTime: true
  $name: Show time

- use24Hour: false
  $name: 24-hour time

- showSeconds: false
  $name: Show seconds

- separator: "   |   "
  $name: Label separator

- fontPreset: segoeVariable
  $name: Font family
  $options:
  - segoeVariable: Segoe UI Variable Text
  - segoe: Segoe UI
  - arial: Arial
  - calibri: Calibri
  - consolas: Consolas
  - tahoma: Tahoma
  - verdana: Verdana
  - custom: Custom

- customFontFamily: ""
  $name: Custom font family
  $description: "Used only when Font family is set to Custom."

- fontSize: 12
  $name: Font size

- fontWeight: normal
  $name: Font weight
  $options:
  - normal: Normal
  - semibold: Semibold
  - bold: Bold

- italic: false
  $name: Italic

- textColor: "#FFFFFF"
  $name: Text color
  $description: "Hex color such as #FFFFFF or #A0A0A0."

- opacity: 100
  $name: Opacity
  $description: "0 to 100."

- leftMargin: 12
  $name: Left spacing

- rightMargin: 12
  $name: Right spacing

- verticalOffset: 0
  $name: Vertical offset
  $description: "Positive values move the label down. Negative values move it up."
*/
// ==/WindhawkModSettings==

#include <windows.h>

#undef GetCurrentTime

#include <xamlom.h>
#include <Unknwn.h>
#include <ocidl.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cwctype>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;

// ============================================================================
// Settings
// ============================================================================

enum class WeekdayStyle {
    None,
    Short,
    Long,
};

enum class DayStyle {
    Number,
    TwoDigit,
};

enum class MonthStyle {
    Number,
    TwoDigit,
    Short,
    Long,
};

enum class YearStyle {
    None,
    Short,
    Long,
};

enum class DateOrder {
    MDY,
    DMY,
    YMD,
};

enum class NumericDateSeparator {
    Slash,
    Dash,
    Dot,
};

enum class FontPreset {
    SegoeVariable,
    Segoe,
    Arial,
    Calibri,
    Consolas,
    Tahoma,
    Verdana,
    Custom,
};

enum class FontWeightSetting {
    Normal,
    Semibold,
    Bold,
};

struct Settings {
    std::wstring customText;
    bool showDate = true;
    WeekdayStyle dateWeekday = WeekdayStyle::Short;
    DayStyle dateDay = DayStyle::Number;
    MonthStyle dateMonth = MonthStyle::Short;
    YearStyle dateYear = YearStyle::None;
    DateOrder dateOrder = DateOrder::MDY;
    NumericDateSeparator numericDateSeparator = NumericDateSeparator::Slash;

    bool showTime = true;
    bool use24Hour = false;
    bool showSeconds = false;

    std::wstring separator = L"   |   ";

    FontPreset fontPreset = FontPreset::SegoeVariable;
    std::wstring customFontFamily;
    int fontSize = 12;
    FontWeightSetting fontWeight = FontWeightSetting::Normal;
    bool italic = false;
    std::wstring textColor = L"#FFFFFF";
    int opacity = 100;

    int leftMargin = 12;
    int rightMargin = 12;
    int verticalOffset = 0;
};

struct SettingsSnapshot {
    Settings settings;
    uint64_t generation = 0;
};

static Settings g_settings;
static std::atomic<uint64_t> g_settingsGeneration{1};
static std::mutex g_settingsMutex;

static std::atomic<bool> g_unloading{false};
static std::atomic<int> g_activeTextBlocks{0};
static HANDLE g_connectorThread = nullptr;

// ============================================================================
// Settings helpers
// ============================================================================

static std::wstring ReadStringSetting(PCWSTR name, PCWSTR fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    if (!value) {
        return fallback;
    }

    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return result;
}

static Settings ReadSettingsFromWindhawk() {
    Settings settings;

    settings.customText = ReadStringSetting(L"customText", L"");
    settings.showDate = Wh_GetIntSetting(L"showDate") != 0;

    {
        std::wstring value = ReadStringSetting(L"dateWeekday", L"short");
        if (value == L"none") {
            settings.dateWeekday = WeekdayStyle::None;
        } else if (value == L"long") {
            settings.dateWeekday = WeekdayStyle::Long;
        } else {
            settings.dateWeekday = WeekdayStyle::Short;
        }
    }

    {
        std::wstring value = ReadStringSetting(L"dateDay", L"number");
        settings.dateDay = value == L"twoDigit" ? DayStyle::TwoDigit
                                                 : DayStyle::Number;
    }

    {
        std::wstring value = ReadStringSetting(L"dateMonth", L"short");
        if (value == L"number") {
            settings.dateMonth = MonthStyle::Number;
        } else if (value == L"twoDigit") {
            settings.dateMonth = MonthStyle::TwoDigit;
        } else if (value == L"long") {
            settings.dateMonth = MonthStyle::Long;
        } else {
            settings.dateMonth = MonthStyle::Short;
        }
    }

    {
        std::wstring value = ReadStringSetting(L"dateYear", L"none");
        if (value == L"short") {
            settings.dateYear = YearStyle::Short;
        } else if (value == L"long") {
            settings.dateYear = YearStyle::Long;
        } else {
            settings.dateYear = YearStyle::None;
        }
    }

    {
        std::wstring value = ReadStringSetting(L"dateOrder", L"mdy");
        if (value == L"dmy") {
            settings.dateOrder = DateOrder::DMY;
        } else if (value == L"ymd") {
            settings.dateOrder = DateOrder::YMD;
        } else {
            settings.dateOrder = DateOrder::MDY;
        }
    }

    {
        std::wstring value =
            ReadStringSetting(L"numericDateSeparator", L"slash");
        if (value == L"dash") {
            settings.numericDateSeparator = NumericDateSeparator::Dash;
        } else if (value == L"dot") {
            settings.numericDateSeparator = NumericDateSeparator::Dot;
        } else {
            settings.numericDateSeparator = NumericDateSeparator::Slash;
        }
    }

    settings.showTime = Wh_GetIntSetting(L"showTime") != 0;
    settings.use24Hour = Wh_GetIntSetting(L"use24Hour") != 0;
    settings.showSeconds = Wh_GetIntSetting(L"showSeconds") != 0;
    settings.separator = ReadStringSetting(L"separator", L"   |   ");

    {
        std::wstring value = ReadStringSetting(L"fontPreset", L"segoeVariable");
        if (value == L"segoe") {
            settings.fontPreset = FontPreset::Segoe;
        } else if (value == L"arial") {
            settings.fontPreset = FontPreset::Arial;
        } else if (value == L"calibri") {
            settings.fontPreset = FontPreset::Calibri;
        } else if (value == L"consolas") {
            settings.fontPreset = FontPreset::Consolas;
        } else if (value == L"tahoma") {
            settings.fontPreset = FontPreset::Tahoma;
        } else if (value == L"verdana") {
            settings.fontPreset = FontPreset::Verdana;
        } else if (value == L"custom") {
            settings.fontPreset = FontPreset::Custom;
        } else {
            settings.fontPreset = FontPreset::SegoeVariable;
        }
    }

    settings.customFontFamily = ReadStringSetting(L"customFontFamily", L"");
    settings.fontSize = Wh_GetIntSetting(L"fontSize");

    {
        std::wstring value = ReadStringSetting(L"fontWeight", L"normal");
        if (value == L"bold") {
            settings.fontWeight = FontWeightSetting::Bold;
        } else if (value == L"semibold") {
            settings.fontWeight = FontWeightSetting::Semibold;
        } else {
            settings.fontWeight = FontWeightSetting::Normal;
        }
    }

    settings.italic = Wh_GetIntSetting(L"italic") != 0;
    settings.textColor = ReadStringSetting(L"textColor", L"#FFFFFF");
    settings.opacity = Wh_GetIntSetting(L"opacity");
    settings.leftMargin = Wh_GetIntSetting(L"leftMargin");
    settings.rightMargin = Wh_GetIntSetting(L"rightMargin");
    settings.verticalOffset = Wh_GetIntSetting(L"verticalOffset");

    if (settings.fontSize < 6) {
        settings.fontSize = 6;
    } else if (settings.fontSize > 72) {
        settings.fontSize = 72;
    }

    if (settings.opacity < 0) {
        settings.opacity = 0;
    } else if (settings.opacity > 100) {
        settings.opacity = 100;
    }

    if (settings.leftMargin < 0) {
        settings.leftMargin = 0;
    } else if (settings.leftMargin > 500) {
        settings.leftMargin = 500;
    }

    if (settings.rightMargin < 0) {
        settings.rightMargin = 0;
    } else if (settings.rightMargin > 500) {
        settings.rightMargin = 500;
    }

    if (settings.verticalOffset < -50) {
        settings.verticalOffset = -50;
    } else if (settings.verticalOffset > 50) {
        settings.verticalOffset = 50;
    }

    return settings;
}

static void LoadSettings(bool incrementGeneration) {
    Settings settings = ReadSettingsFromWindhawk();

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(settings);
    }

    if (incrementGeneration) {
        g_settingsGeneration.fetch_add(1, std::memory_order_release);
    }
}

static SettingsSnapshot GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return {g_settings,
            g_settingsGeneration.load(std::memory_order_acquire)};
}

static std::wstring GetSelectedFontFamily(const Settings& settings) {
    switch (settings.fontPreset) {
        case FontPreset::Segoe:
            return L"Segoe UI";
        case FontPreset::Arial:
            return L"Arial";
        case FontPreset::Calibri:
            return L"Calibri";
        case FontPreset::Consolas:
            return L"Consolas";
        case FontPreset::Tahoma:
            return L"Tahoma";
        case FontPreset::Verdana:
            return L"Verdana";
        case FontPreset::Custom:
            return settings.customFontFamily.empty()
                       ? L"Segoe UI Variable Text"
                       : settings.customFontFamily;
        case FontPreset::SegoeVariable:
        default:
            return L"Segoe UI Variable Text";
    }
}

// ============================================================================
// Color parsing
// ============================================================================

static int HexDigit(wchar_t c) {
    if (c >= L'0' && c <= L'9') {
        return c - L'0';
    }

    c = towupper(c);
    if (c >= L'A' && c <= L'F') {
        return 10 + c - L'A';
    }

    return -1;
}

static bool ParseHexByte(wchar_t a, wchar_t b, uint8_t* result) {
    int hi = HexDigit(a);
    int lo = HexDigit(b);
    if (hi < 0 || lo < 0) {
        return false;
    }

    *result = static_cast<uint8_t>((hi << 4) | lo);
    return true;
}

static winrt::Windows::UI::Color ParseColor(const std::wstring& input) {
    winrt::Windows::UI::Color color{255, 255, 255, 255};
    std::wstring text = input;

    if (!text.empty() && text.front() == L'#') {
        text.erase(text.begin());
    }

    if (text.length() == 6) {
        uint8_t r{};
        uint8_t g{};
        uint8_t b{};

        if (ParseHexByte(text[0], text[1], &r) &&
            ParseHexByte(text[2], text[3], &g) &&
            ParseHexByte(text[4], text[5], &b)) {
            color.A = 255;
            color.R = r;
            color.G = g;
            color.B = b;
        }
    } else if (text.length() == 8) {
        uint8_t a{};
        uint8_t r{};
        uint8_t g{};
        uint8_t b{};

        if (ParseHexByte(text[0], text[1], &a) &&
            ParseHexByte(text[2], text[3], &r) &&
            ParseHexByte(text[4], text[5], &g) &&
            ParseHexByte(text[6], text[7], &b)) {
            color.A = a;
            color.R = r;
            color.G = g;
            color.B = b;
        }
    }

    return color;
}

// ============================================================================
// Date and time formatting
// ============================================================================

static std::wstring FormatLocaleDatePart(const SYSTEMTIME& st, PCWSTR format) {
    wchar_t buffer[128]{};

    if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, format, buffer,
                         ARRAYSIZE(buffer), nullptr)) {
        return L"";
    }

    return buffer;
}

static std::wstring BuildWeekdayText(const SYSTEMTIME& st,
                                     const Settings& settings) {
    switch (settings.dateWeekday) {
        case WeekdayStyle::Short:
            return FormatLocaleDatePart(st, L"ddd");
        case WeekdayStyle::Long:
            return FormatLocaleDatePart(st, L"dddd");
        case WeekdayStyle::None:
        default:
            return L"";
    }
}

static std::wstring BuildDayText(const SYSTEMTIME& st,
                                 const Settings& settings) {
    wchar_t buffer[16]{};
    if (settings.dateDay == DayStyle::TwoDigit) {
        swprintf_s(buffer, L"%02u", st.wDay);
    } else {
        swprintf_s(buffer, L"%u", st.wDay);
    }
    return buffer;
}

static std::wstring BuildMonthText(const SYSTEMTIME& st,
                                   const Settings& settings) {
    wchar_t buffer[32]{};

    switch (settings.dateMonth) {
        case MonthStyle::Number:
            swprintf_s(buffer, L"%u", st.wMonth);
            return buffer;
        case MonthStyle::TwoDigit:
            swprintf_s(buffer, L"%02u", st.wMonth);
            return buffer;
        case MonthStyle::Long:
            return FormatLocaleDatePart(st, L"MMMM");
        case MonthStyle::Short:
        default:
            return FormatLocaleDatePart(st, L"MMM");
    }
}

static std::wstring BuildYearText(const SYSTEMTIME& st,
                                  const Settings& settings) {
    wchar_t buffer[16]{};

    switch (settings.dateYear) {
        case YearStyle::Short:
            swprintf_s(buffer, L"%02u", st.wYear % 100);
            return buffer;
        case YearStyle::Long:
            swprintf_s(buffer, L"%04u", st.wYear);
            return buffer;
        case YearStyle::None:
        default:
            return L"";
    }
}

static bool IsNumericMonth(const Settings& settings) {
    return settings.dateMonth == MonthStyle::Number ||
           settings.dateMonth == MonthStyle::TwoDigit;
}

static wchar_t GetNumericDateSeparator(const Settings& settings) {
    switch (settings.numericDateSeparator) {
        case NumericDateSeparator::Dash:
            return L'-';
        case NumericDateSeparator::Dot:
            return L'.';
        case NumericDateSeparator::Slash:
        default:
            return L'/';
    }
}

static std::wstring BuildDateText(const SYSTEMTIME& st,
                                  const Settings& settings) {
    std::wstring weekday = BuildWeekdayText(st, settings);
    std::wstring day = BuildDayText(st, settings);
    std::wstring month = BuildMonthText(st, settings);
    std::wstring year = BuildYearText(st, settings);
    std::wstring date;

    if (IsNumericMonth(settings)) {
        wchar_t separator = GetNumericDateSeparator(settings);

        auto appendPart = [&date, separator](const std::wstring& part) {
            if (part.empty()) {
                return;
            }
            if (!date.empty()) {
                date += separator;
            }
            date += part;
        };

        switch (settings.dateOrder) {
            case DateOrder::DMY:
                appendPart(day);
                appendPart(month);
                appendPart(year);
                break;
            case DateOrder::YMD:
                appendPart(year);
                appendPart(month);
                appendPart(day);
                break;
            case DateOrder::MDY:
            default:
                appendPart(month);
                appendPart(day);
                appendPart(year);
                break;
        }
    } else {
        switch (settings.dateOrder) {
            case DateOrder::DMY:
                date = day + L" " + month;
                if (!year.empty()) {
                    date += L" " + year;
                }
                break;

            case DateOrder::YMD:
                if (!year.empty()) {
                    date = year + L" ";
                }
                date += month + L" " + day;
                break;

            case DateOrder::MDY:
            default:
                date = month + L" " + day;
                if (!year.empty()) {
                    date += L", " + year;
                }
                break;
        }
    }

    if (!weekday.empty()) {
        date = weekday + L", " + date;
    }

    return date;
}

static std::wstring BuildTimeText(const SYSTEMTIME& st,
                                  const Settings& settings) {
    wchar_t buffer[128]{};

    if (settings.use24Hour) {
        if (settings.showSeconds) {
            swprintf_s(buffer, L"%02u:%02u:%02u", st.wHour, st.wMinute,
                       st.wSecond);
        } else {
            swprintf_s(buffer, L"%02u:%02u", st.wHour, st.wMinute);
        }
    } else {
        unsigned hour = st.wHour % 12;
        if (hour == 0) {
            hour = 12;
        }

        if (settings.showSeconds) {
            swprintf_s(buffer, L"%u:%02u:%02u %s", hour, st.wMinute,
                       st.wSecond, st.wHour >= 12 ? L"PM" : L"AM");
        } else {
            swprintf_s(buffer, L"%u:%02u %s", hour, st.wMinute,
                       st.wHour >= 12 ? L"PM" : L"AM");
        }
    }

    return buffer;
}

static std::wstring BuildDisplayText(const Settings& settings) {
    SYSTEMTIME st{};
    GetLocalTime(&st);

    std::vector<std::wstring> parts;

    if (!settings.customText.empty()) {
        parts.emplace_back(settings.customText);
    }

    if (settings.showDate) {
        std::wstring date = BuildDateText(st, settings);
        if (!date.empty()) {
            parts.emplace_back(std::move(date));
        }
    }

    if (settings.showTime) {
        std::wstring time = BuildTimeText(st, settings);
        if (!time.empty()) {
            parts.emplace_back(std::move(time));
        }
    }

    std::wstring result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i != 0) {
            result += settings.separator;
        }
        result += parts[i];
    }

    return result;
}

// ============================================================================
// Explorer HWND matching
// ============================================================================

struct ExplorerWindowMatchContext {
    double targetWidthDip = 0.0;
    double targetHeightDip = 0.0;
    HWND bestHwnd = nullptr;
    double bestScore = std::numeric_limits<double>::max();
};

static BOOL CALLBACK EnumExplorerWindowProc(HWND hwnd, LPARAM lParam) {
    auto* context =
        reinterpret_cast<ExplorerWindowMatchContext*>(lParam);
    if (!context) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    wchar_t className[128]{};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"CabinetWClass") != 0 || !IsWindowVisible(hwnd)) {
        return TRUE;
    }

    RECT clientRect{};
    if (!GetClientRect(hwnd, &clientRect)) {
        return TRUE;
    }

    UINT dpi = GetDpiForWindow(hwnd);
    if (dpi == 0) {
        dpi = 96;
    }

    double widthPx = static_cast<double>(clientRect.right - clientRect.left);
    double heightPx = static_cast<double>(clientRect.bottom - clientRect.top);
    double widthDip = widthPx * 96.0 / static_cast<double>(dpi);
    double heightDip = heightPx * 96.0 / static_cast<double>(dpi);

    double widthDiff = std::abs(widthDip - context->targetWidthDip);
    double heightDiff = std::abs(heightDip - context->targetHeightDip);
    double score = widthDiff + heightDiff * 0.25;

    if (score < context->bestScore) {
        context->bestScore = score;
        context->bestHwnd = hwnd;
    }

    return TRUE;
}

static HWND FindExplorerWindowForGrid(muxc::Grid const& grid) {
    try {
        auto xamlRoot = grid.XamlRoot();
        if (!xamlRoot) {
            return nullptr;
        }

        auto rootSize = xamlRoot.Size();
        ExplorerWindowMatchContext context;
        context.targetWidthDip = rootSize.Width;
        context.targetHeightDip = rootSize.Height;

        EnumWindows(EnumExplorerWindowProc,
                    reinterpret_cast<LPARAM>(&context));
        return context.bestHwnd;
    } catch (...) {
        return nullptr;
    }
}

// ============================================================================
// Vertical positioning
// ============================================================================

static void UpdateVerticalPosition(muxc::TextBlock const& text,
                                   muxc::Grid const& grid,
                                   int verticalOffset) {
    double automaticCorrection = 0.0;

    try {
        HWND hwnd = FindExplorerWindowForGrid(grid);

        if (hwnd && IsZoomed(hwnd)) {
            auto transform = grid.TransformToVisual(nullptr);
            wf::Point origin{0.0f, 0.0f};
            wf::Point position = transform.TransformPoint(origin);

            if (position.Y < 0.0f) {
                // Explorer's maximized title grid sits slightly above the
                // client origin. The extra 1.5 DIP matches the restored and
                // maximized visual center on current Windows 11 builds.
                automaticCorrection =
                    -static_cast<double>(position.Y) + 1.5;
            }
        }

        auto currentTransform =
            text.RenderTransform().try_as<muxm::TranslateTransform>();

        muxm::TranslateTransform translate{nullptr};
        if (currentTransform) {
            translate = currentTransform;
        } else {
            translate = muxm::TranslateTransform();
            text.RenderTransform(translate);
        }

        translate.Y(static_cast<double>(verticalOffset) + automaticCorrection);
    } catch (...) {
        Wh_Log(L"UpdateVerticalPosition exception hr=0x%08X",
               winrt::to_hresult());
    }
}

// ============================================================================
// Appearance
// ============================================================================

static void ApplyTextSettings(muxc::TextBlock const& text,
                              const Settings& settings) {
    text.Text(BuildDisplayText(settings));

    try {
        muxm::FontFamily family(GetSelectedFontFamily(settings));
        text.FontFamily(family);
    } catch (...) {
        Wh_Log(L"Invalid font family, using Segoe UI Variable Text");
        try {
            text.FontFamily(muxm::FontFamily(L"Segoe UI Variable Text"));
        } catch (...) {
        }
    }

    text.FontSize(static_cast<double>(settings.fontSize));

    winrt::Windows::UI::Text::FontWeight weight{};
    switch (settings.fontWeight) {
        case FontWeightSetting::Bold:
            weight.Weight = 700;
            break;
        case FontWeightSetting::Semibold:
            weight.Weight = 600;
            break;
        case FontWeightSetting::Normal:
        default:
            weight.Weight = 400;
            break;
    }
    text.FontWeight(weight);

    text.FontStyle(settings.italic
                       ? winrt::Windows::UI::Text::FontStyle::Italic
                       : winrt::Windows::UI::Text::FontStyle::Normal);

    // Apply opacity through the foreground brush instead of compositing the
    // entire TextBlock. This keeps low-opacity glyph edges cleaner.
    auto color = ParseColor(settings.textColor);
    color.A = static_cast<uint8_t>(
        (static_cast<unsigned>(color.A) *
             static_cast<unsigned>(settings.opacity) +
         50u) /
        100u);

    muxm::SolidColorBrush brush;
    brush.Color(color);
    text.Foreground(brush);
    text.Opacity(1.0);
    text.HorizontalAlignment(mux::HorizontalAlignment::Right);
    text.VerticalAlignment(mux::VerticalAlignment::Center);
    text.Margin(mux::Thickness{static_cast<double>(settings.leftMargin), 0.0,
                               static_cast<double>(settings.rightMargin), 0.0});
    text.IsHitTestVisible(false);
}

// ============================================================================
// Module helper
// ============================================================================

static HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &module)) {
        return nullptr;
    }

    return module;
}

// Keep the active-label count accurate even when an Explorer window is closed
// before mod unload. The timer owns one shared instance of this helper, so the
// counter is released when that timer/delegate is destroyed.
struct ActiveLabelLifetime {
    ActiveLabelLifetime() {
        g_activeTextBlocks.fetch_add(1);
    }

    ~ActiveLabelLifetime() {
        g_activeTextBlocks.fetch_sub(1);
    }
};

// ============================================================================
// Visual Tree Watcher
// ============================================================================

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2,
                               winrt::non_agile> {
public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_xamlDiagnostics(site.as<IXamlDiagnostics>()),
          m_visualTreeService(site.as<IVisualTreeService3>()) {
        // AdviseVisualTreeChange must be made from a separate thread in
        // Explorer. Keep the handle so shutdown can wait for it safely.
        AddRef();
        m_adviseThread = CreateThread(
            nullptr, 0,
            [](LPVOID parameter) -> DWORD {
                auto watcher = static_cast<VisualTreeWatcher*>(parameter);
                HRESULT hr =
                    watcher->m_visualTreeService->AdviseVisualTreeChange(watcher);
                if (FAILED(hr)) {
                    Wh_Log(L"AdviseVisualTreeChange failed hr=0x%08X", hr);
                }
                watcher->Release();
                return 0;
            },
            this, 0, nullptr);

        if (!m_adviseThread) {
            Wh_Log(L"CreateThread for XAML visual tree watcher failed: %u",
                   GetLastError());
            Release();
        }
    }

    ~VisualTreeWatcher() {
        // Normally Disconnect() has already joined and closed this handle. If
        // destruction happens on the advise thread itself, only close it here.
        if (m_adviseThread) {
            CloseHandle(m_adviseThread);
            m_adviseThread = nullptr;
        }
    }

    void Disconnect() {
        if (m_disconnected) {
            return;
        }
        m_disconnected = true;

        WaitForAdviseThread();

        if (!m_visualTreeService) {
            return;
        }

        HRESULT hr = m_visualTreeService->UnadviseVisualTreeChange(this);
        if (FAILED(hr)) {
            Wh_Log(L"UnadviseVisualTreeChange failed hr=0x%08X", hr);
        }
    }

private:
    void WaitForAdviseThread() {
        if (!m_adviseThread) {
            return;
        }

        DWORD threadId = GetThreadId(m_adviseThread);
        if (threadId != 0 && threadId != GetCurrentThreadId()) {
            WaitForSingleObject(m_adviseThread, INFINITE);
        }

        CloseHandle(m_adviseThread);
        m_adviseThread = nullptr;
    }

    wf::IInspectable FromHandle(InstanceHandle handle) {
        wf::IInspectable object{nullptr};

        HRESULT hr = m_xamlDiagnostics->GetIInspectableFromHandle(
            handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(object)));
        if (FAILED(hr)) {
            return nullptr;
        }

        return object;
    }

    void TryInsertTitleText(InstanceHandle handle) {
        auto inspectable = FromHandle(handle);
        if (!inspectable) {
            return;
        }

        auto frameworkElement = inspectable.try_as<mux::FrameworkElement>();
        if (!frameworkElement || frameworkElement.Name() != L"TabContainerGrid") {
            return;
        }

        auto grid = inspectable.try_as<muxc::Grid>();
        if (!grid) {
            return;
        }

        auto children = grid.Children();
        mux::FrameworkElement rightAnchor{nullptr};

        for (uint32_t i = 0; i < children.Size(); ++i) {
            auto child = children.GetAt(i).try_as<mux::FrameworkElement>();
            if (!child) {
                continue;
            }

            if (child.Name() == L"WindhawkExplorerTitleBarLabel") {
                return;
            }

            if (child.Name() == L"RightContentPresenter") {
                rightAnchor = child;
            }
        }

        if (!rightAnchor) {
            Wh_Log(L"RightContentPresenter not found");
            return;
        }

        int32_t targetColumn = muxc::Grid::GetColumn(rightAnchor);
        int32_t targetRow = muxc::Grid::GetRow(rightAnchor);

        SettingsSnapshot initialSnapshot = GetSettingsSnapshot();

        muxc::TextBlock text;
        text.Name(L"WindhawkExplorerTitleBarLabel");
        ApplyTextSettings(text, initialSnapshot.settings);
        muxc::Grid::SetColumn(text, targetColumn);
        muxc::Grid::SetRow(text, targetRow);
        muxc::Canvas::SetZIndex(text, 100);
        children.Append(text);

        UpdateVerticalPosition(text, grid,
                               initialSnapshot.settings.verticalOffset);

        auto weakSizeText = winrt::make_weak(text);
        auto weakSizeGrid = winrt::make_weak(grid);

        winrt::event_token sizeChangedToken = grid.SizeChanged(
            [weakSizeText, weakSizeGrid](
                auto const&, mux::SizeChangedEventArgs const&) {
                auto text = weakSizeText.get();
                auto grid = weakSizeGrid.get();

                if (!text || !grid || g_unloading.load()) {
                    return;
                }

                SettingsSnapshot snapshot = GetSettingsSnapshot();
                UpdateVerticalPosition(text, grid,
                                       snapshot.settings.verticalOffset);
            });

        auto sizeChangedTokenHolder =
            std::make_shared<winrt::event_token>(sizeChangedToken);

        mux::DispatcherTimer timer;

        // Poll unload quickly so disabling the mod removes the injected label
        // almost immediately. Normal date/time rendering is still throttled to
        // once per second below, so the faster timer doesn't rebuild the label
        // text ten times per second.
        timer.Interval(std::chrono::milliseconds(100));

        auto weakText = winrt::make_weak(text);
        auto weakGrid = winrt::make_weak(grid);
        auto weakTimer = winrt::make_weak(timer);
        auto seenGeneration =
            std::make_shared<uint64_t>(initialSnapshot.generation);
        auto currentSettings =
            std::make_shared<Settings>(initialSnapshot.settings);
        auto lastText = std::make_shared<std::wstring>(
            BuildDisplayText(initialSnapshot.settings));
        auto lastClockSecond = std::make_shared<ULONGLONG>(
            GetTickCount64() / 1000);
        auto activeLifetime = std::make_shared<ActiveLabelLifetime>();

        timer.Tick([weakText, weakGrid, weakTimer, seenGeneration,
                    currentSettings, lastText, lastClockSecond,
                    activeLifetime, sizeChangedTokenHolder](auto const&,
                                                            auto const&) {
            // The capture itself owns the active-label lifetime token.
            (void)activeLifetime;

            auto text = weakText.get();
            auto grid = weakGrid.get();
            auto timer = weakTimer.get();

            if (!text) {
                if (timer) {
                    timer.Stop();
                }
                return;
            }

            if (g_unloading.load()) {
                if (timer) {
                    timer.Stop();
                }

                if (grid) {
                    try {
                        grid.SizeChanged(*sizeChangedTokenHolder);
                    } catch (...) {
                    }

                    try {
                        auto children = grid.Children();
                        uint32_t index{};
                        if (children.IndexOf(text, index)) {
                            children.RemoveAt(index);
                        }
                    } catch (...) {
                    }
                }

                try {
                    text.Tag(nullptr);
                } catch (...) {
                }

                return;
            }

            uint64_t generation =
                g_settingsGeneration.load(std::memory_order_acquire);
            bool settingsChanged = generation != *seenGeneration;

            if (settingsChanged) {
                SettingsSnapshot snapshot = GetSettingsSnapshot();
                *seenGeneration = snapshot.generation;
                *currentSettings = snapshot.settings;

                ApplyTextSettings(text, *currentSettings);
                *lastText = BuildDisplayText(*currentSettings);

                if (grid) {
                    UpdateVerticalPosition(
                        text, grid, currentSettings->verticalOffset);
                }
            }

            ULONGLONG clockSecond = GetTickCount64() / 1000;
            if (clockSecond == *lastClockSecond && !settingsChanged) {
                return;
            }
            *lastClockSecond = clockSecond;

            std::wstring current = BuildDisplayText(*currentSettings);
            if (current != *lastText) {
                text.Text(current);
                *lastText = std::move(current);
            }
        });

        timer.Start();

        // Keep the DispatcherTimer alive for as long as the injected label is
        // alive. The Tick handler only keeps weak references back to the XAML
        // objects, avoiding a strong reference cycle.
        text.Tag(timer);
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) override {
        try {
            if (!g_unloading.load() && mutationType == Add) {
                TryInsertTitleText(element.Handle);
            }
        } catch (...) {
            Wh_Log(L"OnVisualTreeChange exception hr=0x%08X",
                   winrt::to_hresult());
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,
                                                    VisualElementState,
                                                    LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_xamlDiagnostics;
    winrt::com_ptr<IVisualTreeService3> m_visualTreeService;
    HANDLE m_adviseThread = nullptr;
    bool m_disconnected = false;
};

static winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// ============================================================================
// TAP
// ============================================================================

static constexpr CLSID CLSID_WindhawkTitleBarLabelTAP = {
    0x48b7eb40,
    0xd62d,
    0x49c0,
    {0x9f, 0x13, 0x27, 0x41, 0xa7, 0x9b, 0xb4, 0x11},
};

class WindhawkTAP
    : public winrt::implements<WindhawkTAP, IObjectWithSite,
                               winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override {
        try {
            if (g_visualTreeWatcher) {
                g_visualTreeWatcher->Disconnect();
                g_visualTreeWatcher = nullptr;
            }

            m_site.copy_from(site);

            if (m_site) {
                HMODULE module = GetCurrentModuleHandle();
                if (module) {
                    FreeLibrary(module);
                }

                g_visualTreeWatcher =
                    winrt::make_self<VisualTreeWatcher>(m_site);
            }

            return S_OK;
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"SetSite exception hr=0x%08X", hr);
            return hr;
        }
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** result) noexcept override {
        if (!result) {
            return E_POINTER;
        }

        *result = nullptr;
        if (!m_site) {
            return E_FAIL;
        }

        return m_site.as(riid, result);
    }

private:
    winrt::com_ptr<IUnknown> m_site;
};

// ============================================================================
// COM factory and exports
// ============================================================================

template <typename T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer,
                                             REFIID riid,
                                             void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }

        try {
            return winrt::make<T>().as(riid, object);
        } catch (...) {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

extern "C" __declspec(dllexport) HRESULT __stdcall DllGetClassObject(
    REFCLSID clsid,
    REFIID riid,
    LPVOID* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;
    if (clsid != CLSID_WindhawkTitleBarLabelTAP) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    try {
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, result);
    } catch (...) {
        return winrt::to_hresult();
    }
}

extern "C" __declspec(dllexport) HRESULT __stdcall DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

// ============================================================================
// XAML diagnostics connection
// ============================================================================

using InitializeXamlDiagnosticsEx_t = decltype(&InitializeXamlDiagnosticsEx);

static HRESULT ConnectToExplorerXaml() {
    HMODULE self = GetCurrentModuleHandle();
    if (!self) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    wchar_t modulePath[MAX_PATH]{};
    DWORD length =
        GetModuleFileNameW(self, modulePath, ARRAYSIZE(modulePath));
    if (!length || length >= ARRAYSIZE(modulePath)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HMODULE framework =
        GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");
    if (!framework) {
        return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    }

    auto initialize = reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
        GetProcAddress(framework, "InitializeXamlDiagnosticsEx"));
    if (!initialize) {
        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

    for (int i = 1; i <= 10000 && !g_unloading.load(); ++i) {
        wchar_t connection[128]{};
        swprintf_s(connection, L"WinUIVisualDiagConnection%d", i);

        hr = initialize(connection, GetCurrentProcessId(), L"", modulePath,
                        CLSID_WindhawkTitleBarLabelTAP, nullptr);

        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    return hr;
}

// ============================================================================
// Connector thread
// ============================================================================

static bool SleepWhileLoaded(DWORD milliseconds) {
    constexpr DWORD kSliceMs = 50;

    while (milliseconds > 0 && !g_unloading.load()) {
        DWORD slice = milliseconds < kSliceMs ? milliseconds : kSliceMs;
        Sleep(slice);
        milliseconds -= slice;
    }

    return !g_unloading.load();
}

static DWORD WINAPI ConnectorThread(LPVOID) {
    for (int attempt = 1; attempt <= 60 && !g_unloading.load(); ++attempt) {
        HMODULE framework =
            GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");

        if (!framework) {
            if (!SleepWhileLoaded(500)) {
                break;
            }
            continue;
        }

        HRESULT hr = ConnectToExplorerXaml();
        if (SUCCEEDED(hr)) {
            Wh_Log(L"Connected to Explorer XAML diagnostics");
            return 0;
        }

        if (g_unloading.load()) {
            break;
        }

        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            if (!SleepWhileLoaded(500)) {
                break;
            }
            continue;
        }

        Wh_Log(L"Diagnostics connection failed hr=0x%08X", hr);
        if (!SleepWhileLoaded(1000)) {
            break;
        }
    }

    return 0;
}

// ============================================================================
// Windhawk
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Explorer Title Bar Label 1.0.0 init");

    g_unloading.store(false);
    g_activeTextBlocks.store(0);
    LoadSettings(false);

    g_connectorThread =
        CreateThread(nullptr, 0, ConnectorThread, nullptr, 0, nullptr);
    if (!g_connectorThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings(true);
}

void Wh_ModUninit() {
    Wh_Log(L"Explorer Title Bar Label 1.0.0 uninit");

    g_unloading.store(true);

    // Do not let the connector continue executing mod code after unload.
    if (g_connectorThread) {
        WaitForSingleObject(g_connectorThread, INFINITE);
        CloseHandle(g_connectorThread);
        g_connectorThread = nullptr;
    }

    // The UI timer checks unload every 100 ms, so labels normally disappear
    // within a fraction of a second. Keep a bounded wait as a safety margin
    // before disconnecting XAML diagnostics.
    for (int i = 0; i < 15; ++i) {
        if (g_activeTextBlocks.load() <= 0) {
            break;
        }
        Sleep(100);
    }

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->Disconnect();
        g_visualTreeWatcher = nullptr;
    }
}
