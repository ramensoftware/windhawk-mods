// ==WindhawkMod==
// @id              explorer-title-bar-label
// @name            Explorer Title Bar Label
// @description     Add custom text, date and time to the Windows 11 File Explorer title bar.
// @version         1.0.0
// @author          digART
// @github          https://github.com/digart11
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi
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

This mod uses XAML diagnostics, which allows only one diagnostics consumer per Explorer process. Known conflicts include **Windows 11 File Explorer Styler**, **ExplorerBlurMica**, **TranslucentTB**, and other tools/mods that attach to File Explorer XAML diagnostics. If another consumer blocks the connection, the label will not appear.
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

- textColor: ""
  $name: Text color
  $description: "Hex color such as #FFFFFF or #A0A0A0. Leave empty to follow the system theme."

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
#include <dwmapi.h>

#undef GetCurrentTime

#include <xamlom.h>
#include <Unknwn.h>
#include <ocidl.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>

#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <atomic>
#include <chrono>
#include <cwctype>
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

enum class WeekdayStyle
{
    None,
    Short,
    Long,
};

enum class DayStyle
{
    Number,
    TwoDigit,
};

enum class MonthStyle
{
    Number,
    TwoDigit,
    Short,
    Long,
};

enum class YearStyle
{
    None,
    Short,
    Long,
};

enum class DateOrder
{
    MDY,
    DMY,
    YMD,
};

enum class NumericDateSeparator
{
    Slash,
    Dash,
    Dot,
};

enum class FontPreset
{
    SegoeVariable,
    Segoe,
    Arial,
    Calibri,
    Consolas,
    Tahoma,
    Verdana,
    Custom,
};

enum class FontWeightSetting
{
    Normal,
    Semibold,
    Bold,
};

struct Settings
{
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
    std::wstring textColor;
    int opacity = 100;

    int leftMargin = 12;
    int rightMargin = 12;
    int verticalOffset = 0;
};

struct SettingsSnapshot
{
    Settings settings;
    uint64_t generation = 0;
};

static Settings g_settings;
static std::atomic<uint64_t> g_settingsGeneration{1};
static std::mutex g_settingsMutex;

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_diagnosticsConnected{false};
static std::mutex g_connectorMutex;
static HANDLE g_connectorThread = nullptr;
static HANDLE g_tapReadyEvent = nullptr;

// ============================================================================
// Settings helpers
// ============================================================================

static std::wstring ReadStringSetting(PCWSTR name, PCWSTR fallback)
{
    PCWSTR value = Wh_GetStringSetting(name);
    if (!value)
    {
        return fallback;
    }

    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return result;
}

static Settings ReadSettingsFromWindhawk()
{
    Settings settings;

    settings.customText = ReadStringSetting(L"customText", L"");
    settings.showDate = Wh_GetIntSetting(L"showDate") != 0;

    {
        std::wstring value = ReadStringSetting(L"dateWeekday", L"short");
        if (value == L"none")
        {
            settings.dateWeekday = WeekdayStyle::None;
        }
        else if (value == L"long")
        {
            settings.dateWeekday = WeekdayStyle::Long;
        }
        else
        {
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
        if (value == L"number")
        {
            settings.dateMonth = MonthStyle::Number;
        }
        else if (value == L"twoDigit")
        {
            settings.dateMonth = MonthStyle::TwoDigit;
        }
        else if (value == L"long")
        {
            settings.dateMonth = MonthStyle::Long;
        }
        else
        {
            settings.dateMonth = MonthStyle::Short;
        }
    }

    {
        std::wstring value = ReadStringSetting(L"dateYear", L"none");
        if (value == L"short")
        {
            settings.dateYear = YearStyle::Short;
        }
        else if (value == L"long")
        {
            settings.dateYear = YearStyle::Long;
        }
        else
        {
            settings.dateYear = YearStyle::None;
        }
    }

    {
        std::wstring value = ReadStringSetting(L"dateOrder", L"mdy");
        if (value == L"dmy")
        {
            settings.dateOrder = DateOrder::DMY;
        }
        else if (value == L"ymd")
        {
            settings.dateOrder = DateOrder::YMD;
        }
        else
        {
            settings.dateOrder = DateOrder::MDY;
        }
    }

    {
        std::wstring value =
            ReadStringSetting(L"numericDateSeparator", L"slash");
        if (value == L"dash")
        {
            settings.numericDateSeparator = NumericDateSeparator::Dash;
        }
        else if (value == L"dot")
        {
            settings.numericDateSeparator = NumericDateSeparator::Dot;
        }
        else
        {
            settings.numericDateSeparator = NumericDateSeparator::Slash;
        }
    }

    settings.showTime = Wh_GetIntSetting(L"showTime") != 0;
    settings.use24Hour = Wh_GetIntSetting(L"use24Hour") != 0;
    settings.showSeconds = Wh_GetIntSetting(L"showSeconds") != 0;
    settings.separator = ReadStringSetting(L"separator", L"   |   ");

    {
        std::wstring value = ReadStringSetting(L"fontPreset", L"segoeVariable");
        if (value == L"segoe")
        {
            settings.fontPreset = FontPreset::Segoe;
        }
        else if (value == L"arial")
        {
            settings.fontPreset = FontPreset::Arial;
        }
        else if (value == L"calibri")
        {
            settings.fontPreset = FontPreset::Calibri;
        }
        else if (value == L"consolas")
        {
            settings.fontPreset = FontPreset::Consolas;
        }
        else if (value == L"tahoma")
        {
            settings.fontPreset = FontPreset::Tahoma;
        }
        else if (value == L"verdana")
        {
            settings.fontPreset = FontPreset::Verdana;
        }
        else if (value == L"custom")
        {
            settings.fontPreset = FontPreset::Custom;
        }
        else
        {
            settings.fontPreset = FontPreset::SegoeVariable;
        }
    }

    settings.customFontFamily = ReadStringSetting(L"customFontFamily", L"");
    settings.fontSize = Wh_GetIntSetting(L"fontSize");

    {
        std::wstring value = ReadStringSetting(L"fontWeight", L"normal");
        if (value == L"bold")
        {
            settings.fontWeight = FontWeightSetting::Bold;
        }
        else if (value == L"semibold")
        {
            settings.fontWeight = FontWeightSetting::Semibold;
        }
        else
        {
            settings.fontWeight = FontWeightSetting::Normal;
        }
    }

    settings.italic = Wh_GetIntSetting(L"italic") != 0;
    settings.textColor = ReadStringSetting(L"textColor", L"");
    settings.opacity = Wh_GetIntSetting(L"opacity");
    settings.leftMargin = Wh_GetIntSetting(L"leftMargin");
    settings.rightMargin = Wh_GetIntSetting(L"rightMargin");
    settings.verticalOffset = Wh_GetIntSetting(L"verticalOffset");

    if (settings.fontSize < 6)
    {
        settings.fontSize = 6;
    }
    else if (settings.fontSize > 72)
    {
        settings.fontSize = 72;
    }

    if (settings.opacity < 0)
    {
        settings.opacity = 0;
    }
    else if (settings.opacity > 100)
    {
        settings.opacity = 100;
    }

    if (settings.leftMargin < 0)
    {
        settings.leftMargin = 0;
    }
    else if (settings.leftMargin > 500)
    {
        settings.leftMargin = 500;
    }

    if (settings.rightMargin < 0)
    {
        settings.rightMargin = 0;
    }
    else if (settings.rightMargin > 500)
    {
        settings.rightMargin = 500;
    }

    if (settings.verticalOffset < -50)
    {
        settings.verticalOffset = -50;
    }
    else if (settings.verticalOffset > 50)
    {
        settings.verticalOffset = 50;
    }

    return settings;
}

static void LoadSettings(bool incrementGeneration)
{
    Settings settings = ReadSettingsFromWindhawk();

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(settings);
    }

    if (incrementGeneration)
    {
        g_settingsGeneration.fetch_add(1, std::memory_order_release);
    }
}

static SettingsSnapshot GetSettingsSnapshot()
{
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return {g_settings,
            g_settingsGeneration.load(std::memory_order_acquire)};
}

static std::wstring GetSelectedFontFamily(const Settings &settings)
{
    switch (settings.fontPreset)
    {
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

static int HexDigit(wchar_t c)
{
    if (c >= L'0' && c <= L'9')
    {
        return c - L'0';
    }

    c = towupper(c);
    if (c >= L'A' && c <= L'F')
    {
        return 10 + c - L'A';
    }

    return -1;
}

static bool ParseHexByte(wchar_t a, wchar_t b, uint8_t *result)
{
    int hi = HexDigit(a);
    int lo = HexDigit(b);
    if (hi < 0 || lo < 0)
    {
        return false;
    }

    *result = static_cast<uint8_t>((hi << 4) | lo);
    return true;
}

static winrt::Windows::UI::Color ParseColor(const std::wstring &input)
{
    winrt::Windows::UI::Color color{255, 255, 255, 255};
    std::wstring text = input;

    if (!text.empty() && text.front() == L'#')
    {
        text.erase(text.begin());
    }

    if (text.length() == 6)
    {
        uint8_t r{};
        uint8_t g{};
        uint8_t b{};

        if (ParseHexByte(text[0], text[1], &r) &&
            ParseHexByte(text[2], text[3], &g) &&
            ParseHexByte(text[4], text[5], &b))
        {
            color.A = 255;
            color.R = r;
            color.G = g;
            color.B = b;
        }
    }
    else if (text.length() == 8)
    {
        uint8_t a{};
        uint8_t r{};
        uint8_t g{};
        uint8_t b{};

        if (ParseHexByte(text[0], text[1], &a) &&
            ParseHexByte(text[2], text[3], &r) &&
            ParseHexByte(text[4], text[5], &g) &&
            ParseHexByte(text[6], text[7], &b))
        {
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

static std::wstring FormatLocaleDatePart(const SYSTEMTIME &st, PCWSTR format)
{
    wchar_t buffer[128]{};

    if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, format, buffer,
                         ARRAYSIZE(buffer), nullptr))
    {
        return L"";
    }

    return buffer;
}

static std::wstring BuildWeekdayText(const SYSTEMTIME &st,
                                     const Settings &settings)
{
    switch (settings.dateWeekday)
    {
    case WeekdayStyle::Short:
        return FormatLocaleDatePart(st, L"ddd");
    case WeekdayStyle::Long:
        return FormatLocaleDatePart(st, L"dddd");
    case WeekdayStyle::None:
    default:
        return L"";
    }
}

static std::wstring BuildDayText(const SYSTEMTIME &st,
                                 const Settings &settings)
{
    wchar_t buffer[16]{};
    if (settings.dateDay == DayStyle::TwoDigit)
    {
        swprintf_s(buffer, L"%02u", st.wDay);
    }
    else
    {
        swprintf_s(buffer, L"%u", st.wDay);
    }
    return buffer;
}

static std::wstring BuildMonthText(const SYSTEMTIME &st,
                                   const Settings &settings)
{
    wchar_t buffer[32]{};

    switch (settings.dateMonth)
    {
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

static std::wstring BuildYearText(const SYSTEMTIME &st,
                                  const Settings &settings)
{
    wchar_t buffer[16]{};

    switch (settings.dateYear)
    {
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

static bool IsNumericMonth(const Settings &settings)
{
    return settings.dateMonth == MonthStyle::Number ||
           settings.dateMonth == MonthStyle::TwoDigit;
}

static wchar_t GetNumericDateSeparator(const Settings &settings)
{
    switch (settings.numericDateSeparator)
    {
    case NumericDateSeparator::Dash:
        return L'-';
    case NumericDateSeparator::Dot:
        return L'.';
    case NumericDateSeparator::Slash:
    default:
        return L'/';
    }
}

static std::wstring BuildDateText(const SYSTEMTIME &st,
                                  const Settings &settings)
{
    std::wstring weekday = BuildWeekdayText(st, settings);
    std::wstring day = BuildDayText(st, settings);
    std::wstring month = BuildMonthText(st, settings);
    std::wstring year = BuildYearText(st, settings);
    std::wstring date;

    if (IsNumericMonth(settings))
    {
        wchar_t separator = GetNumericDateSeparator(settings);

        auto appendPart = [&date, separator](const std::wstring &part)
        {
            if (part.empty())
            {
                return;
            }
            if (!date.empty())
            {
                date += separator;
            }
            date += part;
        };

        switch (settings.dateOrder)
        {
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
    }
    else
    {
        switch (settings.dateOrder)
        {
        case DateOrder::DMY:
            date = day + L" " + month;
            if (!year.empty())
            {
                date += L" " + year;
            }
            break;

        case DateOrder::YMD:
            if (!year.empty())
            {
                date = year + L" ";
            }
            date += month + L" " + day;
            break;

        case DateOrder::MDY:
        default:
            date = month + L" " + day;
            if (!year.empty())
            {
                date += L", " + year;
            }
            break;
        }
    }

    if (!weekday.empty())
    {
        date = weekday + L", " + date;
    }

    return date;
}

static std::wstring BuildTimeText(const SYSTEMTIME &st,
                                  const Settings &settings)
{
    wchar_t buffer[128]{};

    if (settings.use24Hour)
    {
        if (settings.showSeconds)
        {
            swprintf_s(buffer, L"%02u:%02u:%02u", st.wHour, st.wMinute,
                       st.wSecond);
        }
        else
        {
            swprintf_s(buffer, L"%02u:%02u", st.wHour, st.wMinute);
        }
    }
    else
    {
        unsigned hour = st.wHour % 12;
        if (hour == 0)
        {
            hour = 12;
        }

        if (settings.showSeconds)
        {
            swprintf_s(buffer, L"%u:%02u:%02u %s", hour, st.wMinute,
                       st.wSecond, st.wHour >= 12 ? L"PM" : L"AM");
        }
        else
        {
            swprintf_s(buffer, L"%u:%02u %s", hour, st.wMinute,
                       st.wHour >= 12 ? L"PM" : L"AM");
        }
    }

    return buffer;
}

static std::wstring BuildDisplayText(const Settings &settings)
{
    SYSTEMTIME st{};
    GetLocalTime(&st);

    std::vector<std::wstring> parts;

    if (!settings.customText.empty())
    {
        parts.emplace_back(settings.customText);
    }

    if (settings.showDate)
    {
        std::wstring date = BuildDateText(st, settings);
        if (!date.empty())
        {
            parts.emplace_back(std::move(date));
        }
    }

    if (settings.showTime)
    {
        std::wstring time = BuildTimeText(st, settings);
        if (!time.empty())
        {
            parts.emplace_back(std::move(time));
        }
    }

    std::wstring result;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (i != 0)
        {
            result += settings.separator;
        }
        result += parts[i];
    }

    return result;
}

// ============================================================================
// Explorer HWND
// ============================================================================

static HWND GetExplorerWindowForElement(mux::FrameworkElement const &element)
{
    try
    {
        if (auto xamlRoot = element.XamlRoot())
        {
            if (auto environment = xamlRoot.ContentIslandEnvironment())
            {
                return reinterpret_cast<HWND>(
                    static_cast<uintptr_t>(environment.AppWindowId().Value));
            }
        }
    }
    catch (...)
    {
        Wh_Log(L"Failed to get Explorer window from XamlRoot hr=0x%08X",
               winrt::to_hresult());
    }

    return nullptr;
}

// ============================================================================
// Vertical positioning
// ============================================================================

static double GetCaptionButtonsWidthDip(HWND hwnd)
{
    if (!hwnd)
    {
        return 0.0;
    }

    RECT captionBounds{};
    HRESULT hr = DwmGetWindowAttribute(
        hwnd, DWMWA_CAPTION_BUTTON_BOUNDS,
        &captionBounds, sizeof(captionBounds));
    if (FAILED(hr))
    {
        return 0.0;
    }

    LONG widthPx = captionBounds.right - captionBounds.left;
    if (widthPx <= 0)
    {
        return 0.0;
    }

    UINT dpi = GetDpiForWindow(hwnd);
    if (!dpi)
    {
        dpi = 96;
    }

    return static_cast<double>(widthPx) * 96.0 /
           static_cast<double>(dpi);
}

static void UpdateLabelPosition(muxc::TextBlock const &text,
                                muxc::Grid const &grid,
                                int verticalOffset)
{
    double automaticHorizontalCorrection = 0.0;
    double automaticVerticalCorrection = 0.0;

    try
    {
        auto currentTransform =
            text.RenderTransform().try_as<muxm::TranslateTransform>();

        muxm::TranslateTransform translate{nullptr};
        if (currentTransform)
        {
            translate = currentTransform;
        }
        else
        {
            translate = muxm::TranslateTransform();
            text.RenderTransform(translate);
        }

        // Always measure from the native XAML position.
        translate.X(0.0);

        HWND hwnd = GetExplorerWindowForElement(grid);

        if (hwnd && text.ActualWidth() > 0.0)
        {
            auto xamlRoot = text.XamlRoot();
            if (xamlRoot)
            {
                double captionButtonsWidth =
                    GetCaptionButtonsWidthDip(hwnd);

                if (captionButtonsWidth > 0.0)
                {
                    wf::Point origin{0.0f, 0.0f};
                    auto textTransform = text.TransformToVisual(nullptr);
                    wf::Point textPosition =
                        textTransform.TransformPoint(origin);

                    double textRight =
                        static_cast<double>(textPosition.X) +
                        text.ActualWidth();

                    double safeRight =
                        static_cast<double>(xamlRoot.Size().Width) -
                        captionButtonsWidth;

                    // Explorer builds differ in whether this XAML region
                    // already excludes the native caption buttons. Only shift
                    // when the label actually enters the DWM-reported button
                    // bounds.
                    if (textRight > safeRight)
                    {
                        automaticHorizontalCorrection =
                            safeRight - textRight;
                    }
                }
            }
        }

        if (hwnd && IsZoomed(hwnd))
        {
            auto transform = grid.TransformToVisual(nullptr);
            wf::Point origin{0.0f, 0.0f};
            wf::Point position = transform.TransformPoint(origin);

            if (position.Y < 0.0f)
            {
                automaticVerticalCorrection =
                    -static_cast<double>(position.Y) + 0.0;
            }
        }

        translate.X(automaticHorizontalCorrection);
        translate.Y(static_cast<double>(verticalOffset) +
                    automaticVerticalCorrection);
    }
    catch (...)
    {
        Wh_Log(L"UpdateLabelPosition exception hr=0x%08X",
               winrt::to_hresult());
    }
}

// ============================================================================
// Appearance
// ============================================================================

static void ApplyTextSettings(muxc::TextBlock const &text,
                              const Settings &settings)
{
    text.Text(BuildDisplayText(settings));

    try
    {
        muxm::FontFamily family(GetSelectedFontFamily(settings));
        text.FontFamily(family);
    }
    catch (...)
    {
        Wh_Log(L"Invalid font family, using Segoe UI Variable Text");
        try
        {
            text.FontFamily(muxm::FontFamily(L"Segoe UI Variable Text"));
        }
        catch (...)
        {
        }
    }

    text.FontSize(static_cast<double>(settings.fontSize));

    winrt::Windows::UI::Text::FontWeight weight{};
    switch (settings.fontWeight)
    {
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

    if (settings.textColor.empty())
    {
        // Follow Explorer's theme foreground when no custom color is set.
        text.ClearValue(muxc::TextBlock::ForegroundProperty());
        text.Opacity(static_cast<double>(settings.opacity) / 100.0);
    }
    else
    {
        // Apply opacity through the custom foreground brush instead of
        // compositing the entire TextBlock.
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
    }
    text.HorizontalAlignment(mux::HorizontalAlignment::Right);
    text.VerticalAlignment(mux::VerticalAlignment::Center);
    text.Margin(mux::Thickness{static_cast<double>(settings.leftMargin), 0.0,
                               static_cast<double>(settings.rightMargin), 0.0});
    text.IsHitTestVisible(false);
}

// ============================================================================
// Module helper
// ============================================================================

static HMODULE GetCurrentModuleHandle()
{
    HMODULE module = nullptr;

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &module))
    {
        return nullptr;
    }

    return module;
}

// DispatcherTimer is rooted by the dispatcher queue while running, and its Tick
// handler lives in this DLL. Keep each timer and every XAML event token in
// thread-local state so teardown can stop and revoke them on the owning UI
// thread before the mod is unloaded.
struct LabelEntry
{
    winrt::weak_ref<muxc::TextBlock> text;
    winrt::weak_ref<muxc::Grid> grid;
    mux::DispatcherTimer timer{nullptr};
    winrt::event_token tickToken{};
    winrt::event_token sizeChangedToken{};
    bool tickRegistered = false;
    bool sizeChangedRegistered = false;
    bool cleaned = false;

    uint64_t seenGeneration = 0;
    Settings currentSettings;
    std::wstring lastText;
};

thread_local std::vector<std::shared_ptr<LabelEntry>> g_labelEntries;

static void ReleaseLabelEntry(const std::shared_ptr<LabelEntry> &entry,
                              bool removeElement)
{
    if (!entry || entry->cleaned)
    {
        return;
    }

    entry->cleaned = true;

    try
    {
        if (entry->timer)
        {
            entry->timer.Stop();
            if (entry->tickRegistered)
            {
                entry->timer.Tick(entry->tickToken);
                entry->tickRegistered = false;
            }
        }
    }
    catch (...)
    {
        Wh_Log(L"Failed to release title-bar timer hr=0x%08X",
               winrt::to_hresult());
    }

    auto grid = entry->grid.get();
    if (grid && entry->sizeChangedRegistered)
    {
        try
        {
            grid.SizeChanged(entry->sizeChangedToken);
            entry->sizeChangedRegistered = false;
        }
        catch (...)
        {
            Wh_Log(L"Failed to revoke SizeChanged hr=0x%08X",
                   winrt::to_hresult());
        }
    }

    if (removeElement)
    {
        auto text = entry->text.get();
        if (grid && text)
        {
            try
            {
                auto children = grid.Children();
                uint32_t index{};
                if (children.IndexOf(text, index))
                {
                    children.RemoveAt(index);
                }
            }
            catch (...)
            {
                Wh_Log(L"Failed to remove title-bar label hr=0x%08X",
                       winrt::to_hresult());
            }
        }
    }
}

static void PruneReleasedLabelEntries()
{
    for (auto it = g_labelEntries.begin(); it != g_labelEntries.end();)
    {
        auto &entry = *it;

        if (!entry->cleaned && !entry->text.get())
        {
            ReleaseLabelEntry(entry, false);
        }

        if (entry->cleaned)
        {
            it = g_labelEntries.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

static void RemoveLabelsForCurrentThread()
{
    std::vector<std::shared_ptr<LabelEntry>> taken;
    taken.swap(g_labelEntries);

    for (auto &entry : taken)
    {
        ReleaseLabelEntry(entry, true);
    }
}

static bool IsFileExplorerWindow(HWND hwnd)
{
    if (!hwnd)
    {
        return false;
    }

    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hwnd, &processId) ||
        processId != GetCurrentProcessId())
    {
        return false;
    }

    wchar_t className[64]{};
    return GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, L"CabinetWClass") == 0;
}

static std::vector<HWND> GetFileExplorerWindows()
{
    std::vector<HWND> windows;

    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL
        {
            auto &windows =
                *reinterpret_cast<std::vector<HWND> *>(lParam);
            if (IsFileExplorerWindow(hwnd))
            {
                windows.push_back(hwnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));

    return windows;
}

using RunFromWindowThreadProc_t = void(WINAPI *)(PVOID parameter);

static bool RunFromWindowThread(HWND hwnd,
                                RunFromWindowThreadProc_t proc,
                                PVOID procParam)
{
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParam
    {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId)
    {
        return false;
    }

    if (threadId == GetCurrentThreadId())
    {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (code == HC_ACTION)
            {
                const auto *cwp = reinterpret_cast<const CWPSTRUCT *>(lParam);
                if (cwp->message == message)
                {
                    auto *param =
                        reinterpret_cast<RunParam *>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook)
    {
        return false;
    }

    RunParam param{proc, procParam};
    SendMessageW(hwnd, message, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);

    return true;
}

static void EnsureConnectorStarted();

// Connect only after a real File Explorer top-level window exists. This avoids
// occupying the XAML diagnostics slot in shell-only explorer.exe processes and
// means a File Explorer window opened long after login still triggers setup.
using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle,
                                        LPCWSTR className,
                                        LPCWSTR windowName,
                                        DWORD style,
                                        int x,
                                        int y,
                                        int width,
                                        int height,
                                        HWND parent,
                                        HMENU menu,
                                        HINSTANCE instance,
                                        PVOID param)
{
    HWND hwnd = CreateWindowExW_Original(
        exStyle, className, windowName, style, x, y, width, height, parent,
        menu, instance, param);

    if (hwnd && !g_unloading.load() && IsFileExplorerWindow(hwnd))
    {
        EnsureConnectorStarted();
    }

    return hwnd;
}

// ============================================================================
// Visual Tree Watcher
// ============================================================================

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2,
                               winrt::non_agile>
{
public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_xamlDiagnostics(site.as<IXamlDiagnostics>()),
          m_visualTreeService(site.as<IVisualTreeService3>())
    {
        // AdviseVisualTreeChange must be made from a separate thread in
        // Explorer. Keep the handle so shutdown can wait for it safely.
        AddRef();
        m_adviseThread = CreateThread(
            nullptr, 0,
            [](LPVOID parameter) -> DWORD
            {
                auto watcher = static_cast<VisualTreeWatcher *>(parameter);
                HRESULT hr =
                    watcher->m_visualTreeService->AdviseVisualTreeChange(watcher);
                if (FAILED(hr))
                {
                    Wh_Log(L"AdviseVisualTreeChange failed hr=0x%08X", hr);
                }
                watcher->Release();
                return 0;
            },
            this, 0, nullptr);

        if (!m_adviseThread)
        {
            Wh_Log(L"CreateThread for XAML visual tree watcher failed: %u",
                   GetLastError());
            Release();
        }
    }

    ~VisualTreeWatcher()
    {
        // Normally Disconnect() has already joined and closed this handle. If
        // destruction happens on the advise thread itself, only close it here.
        if (m_adviseThread)
        {
            CloseHandle(m_adviseThread);
            m_adviseThread = nullptr;
        }
    }

    void Disconnect()
    {
        if (m_disconnected)
        {
            return;
        }
        m_disconnected = true;

        WaitForAdviseThread();

        if (!m_visualTreeService)
        {
            return;
        }

        HRESULT hr = m_visualTreeService->UnadviseVisualTreeChange(this);
        if (FAILED(hr))
        {
            Wh_Log(L"UnadviseVisualTreeChange failed hr=0x%08X", hr);
        }
    }

private:
    void WaitForAdviseThread()
    {
        if (!m_adviseThread)
        {
            return;
        }

        DWORD threadId = GetThreadId(m_adviseThread);
        if (threadId != 0 && threadId != GetCurrentThreadId())
        {
            WaitForSingleObject(m_adviseThread, INFINITE);
        }

        CloseHandle(m_adviseThread);
        m_adviseThread = nullptr;
    }

    wf::IInspectable FromHandle(InstanceHandle handle)
    {
        wf::IInspectable object{nullptr};

        HRESULT hr = m_xamlDiagnostics->GetIInspectableFromHandle(
            handle, reinterpret_cast<::IInspectable **>(winrt::put_abi(object)));
        if (FAILED(hr))
        {
            return nullptr;
        }

        return object;
    }

    void TryInsertTitleText(InstanceHandle handle)
    {
        auto inspectable = FromHandle(handle);
        if (!inspectable)
        {
            return;
        }

        auto frameworkElement = inspectable.try_as<mux::FrameworkElement>();
        if (!frameworkElement ||
            frameworkElement.Name() != L"TabContainerGrid")
        {
            return;
        }

        auto grid = inspectable.try_as<muxc::Grid>();
        if (!grid)
        {
            return;
        }

        auto children = grid.Children();
        mux::FrameworkElement rightAnchor{nullptr};

        for (uint32_t i = 0; i < children.Size(); ++i)
        {
            auto child = children.GetAt(i).try_as<mux::FrameworkElement>();
            if (!child)
            {
                continue;
            }

            if (child.Name() == L"WindhawkExplorerTitleBarLabel")
            {
                return;
            }

            if (child.Name() == L"RightContentPresenter")
            {
                rightAnchor = child;
            }
        }

        if (!rightAnchor)
        {
            Wh_Log(L"RightContentPresenter not found");
            return;
        }

        PruneReleasedLabelEntries();

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

        try
        {
            grid.UpdateLayout();
        }
        catch (...)
        {
            Wh_Log(L"Initial title-bar layout update failed hr=0x%08X",
                   winrt::to_hresult());
        }

        UpdateLabelPosition(text, grid,
                            initialSnapshot.settings.verticalOffset);

        auto entry = std::make_shared<LabelEntry>();
        entry->text = winrt::make_weak(text);
        entry->grid = winrt::make_weak(grid);
        entry->seenGeneration = initialSnapshot.generation;
        entry->currentSettings = initialSnapshot.settings;
        entry->lastText = BuildDisplayText(initialSnapshot.settings);

        // Track the entry before registering any delegate whose callback lives
        // in this DLL, so shutdown can always revoke partially initialized
        // handlers.
        g_labelEntries.push_back(entry);
        std::weak_ptr<LabelEntry> weakEntry = entry;

        try
        {
            entry->sizeChangedToken = grid.SizeChanged(
                [weakEntry](auto const &, mux::SizeChangedEventArgs const &)
                {
                    auto entry = weakEntry.lock();
                    if (!entry || entry->cleaned || g_unloading.load())
                    {
                        return;
                    }

                    auto text = entry->text.get();
                    auto grid = entry->grid.get();
                    if (!text || !grid)
                    {
                        return;
                    }

                    SettingsSnapshot snapshot = GetSettingsSnapshot();
                    UpdateLabelPosition(text, grid,
                                        snapshot.settings.verticalOffset);
                });
            entry->sizeChangedRegistered = true;

            mux::DispatcherTimer timer;
            timer.Interval(std::chrono::seconds(1));

            entry->timer = timer;
            entry->tickToken = timer.Tick(
                [weakEntry](auto const &, auto const &)
                {
                    auto entry = weakEntry.lock();
                    if (!entry || entry->cleaned)
                    {
                        return;
                    }

                    auto text = entry->text.get();
                    if (!text)
                    {
                        ReleaseLabelEntry(entry, false);
                        return;
                    }

                    if (g_unloading.load())
                    {
                        ReleaseLabelEntry(entry, true);
                        return;
                    }

                    uint64_t generation =
                        g_settingsGeneration.load(std::memory_order_acquire);
                    bool settingsChanged =
                        generation != entry->seenGeneration;

                    if (settingsChanged)
                    {
                        SettingsSnapshot snapshot = GetSettingsSnapshot();
                        entry->seenGeneration = snapshot.generation;
                        entry->currentSettings = snapshot.settings;

                        ApplyTextSettings(text, entry->currentSettings);
                        entry->lastText =
                            BuildDisplayText(entry->currentSettings);

                        if (auto grid = entry->grid.get())
                        {
                            UpdateLabelPosition(
                                text, grid,
                                entry->currentSettings.verticalOffset);
                        }
                    }

                    std::wstring current =
                        BuildDisplayText(entry->currentSettings);
                    if (current != entry->lastText)
                    {
                        text.Text(current);
                        entry->lastText = std::move(current);

                        if (auto grid = entry->grid.get())
                        {
                            UpdateLabelPosition(
                                text, grid,
                                entry->currentSettings.verticalOffset);
                        }
                    }
                });
            entry->tickRegistered = true;

            timer.Start();
        }
        catch (...)
        {
            ReleaseLabelEntry(entry, true);
            PruneReleasedLabelEntries();
            throw;
        }
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) override
    {
        try
        {
            if (!g_unloading.load() && mutationType == Add)
            {
                TryInsertTitleText(element.Handle);
            }
        }
        catch (...)
        {
            Wh_Log(L"OnVisualTreeChange exception hr=0x%08X",
                   winrt::to_hresult());
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,
                                                    VisualElementState,
                                                    LPCWSTR) noexcept override
    {
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
                               winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *site) override
    {
        try
        {
            if (g_visualTreeWatcher)
            {
                g_visualTreeWatcher->Disconnect();
                g_visualTreeWatcher = nullptr;
            }
            g_diagnosticsConnected.store(false, std::memory_order_release);

            m_site.copy_from(site);

            if (m_site)
            {
                // Balance the module reference added by
                // InitializeXamlDiagnosticsEx even during shutdown.
                HMODULE module = GetCurrentModuleHandle();
                if (module)
                {
                    FreeLibrary(module);
                }

                if (!g_unloading.load())
                {
                    g_visualTreeWatcher =
                        winrt::make_self<VisualTreeWatcher>(m_site);
                    g_diagnosticsConnected.store(
                        true, std::memory_order_release);

                    if (g_tapReadyEvent)
                    {
                        SetEvent(g_tapReadyEvent);
                    }
                }
            }

            return S_OK;
        }
        catch (...)
        {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"SetSite exception hr=0x%08X", hr);
            return hr;
        }
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **result) noexcept override
    {
        if (!result)
        {
            return E_POINTER;
        }

        *result = nullptr;
        if (!m_site)
        {
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
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *outer,
                                             REFIID riid,
                                             void **object) override
    {
        if (!object)
        {
            return E_POINTER;
        }

        *object = nullptr;
        if (outer)
        {
            return CLASS_E_NOAGGREGATION;
        }

        try
        {
            return winrt::make<T>().as(riid, object);
        }
        catch (...)
        {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

extern "C" __declspec(dllexport) HRESULT __stdcall DllGetClassObject(
    REFCLSID clsid,
    REFIID riid,
    LPVOID *result)
{
    if (!result)
    {
        return E_POINTER;
    }

    *result = nullptr;
    if (clsid != CLSID_WindhawkTitleBarLabelTAP)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    try
    {
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, result);
    }
    catch (...)
    {
        return winrt::to_hresult();
    }
}

extern "C" __declspec(dllexport) HRESULT __stdcall DllCanUnloadNow()
{
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

// ============================================================================
// XAML diagnostics connection
// ============================================================================

using InitializeXamlDiagnosticsEx_t = decltype(&InitializeXamlDiagnosticsEx);

static HRESULT ConnectToExplorerXaml()
{
    HMODULE self = GetCurrentModuleHandle();
    if (!self)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    wchar_t modulePath[MAX_PATH]{};
    DWORD length =
        GetModuleFileNameW(self, modulePath, ARRAYSIZE(modulePath));
    if (!length || length >= ARRAYSIZE(modulePath))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HMODULE framework =
        GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");
    if (!framework)
    {
        return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    }

    auto initialize = reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
        GetProcAddress(framework, "InitializeXamlDiagnosticsEx"));
    if (!initialize)
    {
        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

    for (int i = 1; i <= 10000 && !g_unloading.load(); ++i)
    {
        wchar_t connection[128]{};
        swprintf_s(connection, L"WinUIVisualDiagConnection%d", i);

        hr = initialize(connection, GetCurrentProcessId(), L"", modulePath,
                        CLSID_WindhawkTitleBarLabelTAP, nullptr);

        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    return hr;
}

// ============================================================================
// Connector thread
// ============================================================================

static bool SleepWhileLoaded(DWORD milliseconds)
{
    constexpr DWORD kSliceMs = 50;

    while (milliseconds > 0 && !g_unloading.load())
    {
        DWORD slice = milliseconds < kSliceMs ? milliseconds : kSliceMs;
        Sleep(slice);
        milliseconds -= slice;
    }

    return !g_unloading.load();
}

static DWORD WINAPI ConnectorThread(LPVOID)
{
    // This thread starts only after a real File Explorer window exists. Give
    // WinUI a bounded startup window instead of polling forever in unsupported
    // Explorer processes.
    constexpr int kMaxAttempts = 120;
    constexpr DWORD kRetryDelayMs = 500;

    for (int attempt = 0;
         attempt < kMaxAttempts && !g_unloading.load();
         ++attempt)
    {
        HMODULE framework =
            GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");

        if (framework)
        {
            HRESULT hr = ConnectToExplorerXaml();
            if (SUCCEEDED(hr))
            {
                // Some XAML-diagnostics consumers intentionally return S_OK
                // while blocking the caller. Only report success after our
                // TAP's SetSite actually runs.
                DWORD waitResult =
                    g_tapReadyEvent
                        ? WaitForSingleObject(g_tapReadyEvent, 2000)
                        : WAIT_FAILED;

                if (waitResult == WAIT_OBJECT_0)
                {
                    Wh_Log(L"Connected to Explorer XAML diagnostics");
                }
                else if (!g_unloading.load())
                {
                    Wh_Log(
                        L"XAML diagnostics returned success, but the title-bar "
                        L"TAP was not initialized. Another XAML diagnostics "
                        L"consumer may have blocked the connection.");
                }

                return 0;
            }

            if (g_unloading.load())
            {
                return 0;
            }

            if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND) &&
                hr != HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND))
            {
                Wh_Log(L"Diagnostics connection failed hr=0x%08X", hr);
                return 0;
            }
        }

        if (!SleepWhileLoaded(kRetryDelayMs))
        {
            return 0;
        }
    }

    if (!g_unloading.load())
    {
        Wh_Log(L"Explorer WinUI diagnostics did not become available");
    }

    return 0;
}

static void EnsureConnectorStarted()
{
    if (g_unloading.load() ||
        g_diagnosticsConnected.load(std::memory_order_acquire))
    {
        return;
    }

    std::lock_guard<std::mutex> lock(g_connectorMutex);
    if (g_unloading.load() ||
        g_diagnosticsConnected.load(std::memory_order_acquire))
    {
        return;
    }

    if (g_connectorThread)
    {
        if (WaitForSingleObject(g_connectorThread, 0) == WAIT_OBJECT_0)
        {
            CloseHandle(g_connectorThread);
            g_connectorThread = nullptr;
        }
        else
        {
            return;
        }
    }

    if (g_tapReadyEvent)
    {
        ResetEvent(g_tapReadyEvent);
    }

    g_connectorThread =
        CreateThread(nullptr, 0, ConnectorThread, nullptr, 0, nullptr);
    if (!g_connectorThread)
    {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

static void StopConnectorThread()
{
    HANDLE thread = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_connectorMutex);
        thread = g_connectorThread;
    }

    if (!thread)
    {
        return;
    }

    if (GetThreadId(thread) != GetCurrentThreadId())
    {
        WaitForSingleObject(thread, INFINITE);
    }

    std::lock_guard<std::mutex> lock(g_connectorMutex);
    if (g_connectorThread == thread)
    {
        CloseHandle(g_connectorThread);
        g_connectorThread = nullptr;
    }
}

// ============================================================================
// Windhawk
// ============================================================================

BOOL Wh_ModInit()
{
    Wh_Log(L"Explorer Title Bar Label 1.0.0 init");

    g_unloading.store(false);
    g_diagnosticsConnected.store(false, std::memory_order_release);
    LoadSettings(false);

    g_tapReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_tapReadyEvent)
    {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void *>(CreateWindowExW),
            reinterpret_cast<void *>(CreateWindowExW_Hook),
            reinterpret_cast<void **>(&CreateWindowExW_Original)))
    {
        Wh_Log(L"Failed to hook CreateWindowExW");
        CloseHandle(g_tapReadyEvent);
        g_tapReadyEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    // Hooks become active after Wh_ModInit. Handle windows which were already
    // open before that point.
    if (!GetFileExplorerWindows().empty())
    {
        EnsureConnectorStarted();
    }
}

void Wh_ModSettingsChanged()
{
    LoadSettings(true);
}

// Function hooks are removed before Wh_ModUninit, but XAML delegates can still
// be alive until explicit UI-thread teardown below. Stop them from doing new
// work as early as Windhawk allows.
void Wh_ModBeforeUninit()
{
    g_unloading.store(true);
    g_diagnosticsConnected.store(false, std::memory_order_release);
}

void Wh_ModUninit()
{
    Wh_Log(L"Explorer Title Bar Label 1.0.0 uninit");

    g_unloading.store(true);

    StopConnectorThread();

    // Every XAML delegate is revoked synchronously on the thread which owns it
    // before this DLL can be unloaded.
    for (HWND hwnd : GetFileExplorerWindows())
    {
        if (!RunFromWindowThread(
                hwnd, [](PVOID)
                { RemoveLabelsForCurrentThread(); },
                nullptr))
        {
            Wh_Log(L"Couldn't reach Explorer UI thread for window %08X",
                   static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hwnd)));
        }
    }

    if (g_visualTreeWatcher)
    {
        g_visualTreeWatcher->Disconnect();
        g_visualTreeWatcher = nullptr;
    }
    g_diagnosticsConnected.store(false, std::memory_order_release);

    if (g_tapReadyEvent)
    {
        CloseHandle(g_tapReadyEvent);
        g_tapReadyEvent = nullptr;
    }
}
