// ==WindhawkMod==
// @id              vietnamese-lunar-calendar
// @name            Vietnamese Lunar Calendar
// @name:vi-VN      Lịch Âm Việt Nam
// @description     Display Vietnamese lunar calendar dates, full moons, and traditional holidays in Windows 11 calendar flyout
// @description:vi-VN Hiển thị ngày âm lịch, Mùng 1, Rằm và Lễ Tết Việt Nam trực tiếp trên bảng lịch Windows 11
// @version         1.0.0
// @author          PhatMT97
// @github          https://github.com/phatMT97
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vietnamese Lunar Calendar for Windows 11

Displays authentic Vietnamese lunar calendar dates, full moons (*Rằm*), new moons (*Mùng 1*), and traditional holidays directly inside the Windows 11 calendar flyout (Notification Center).

## Features
- **Authentic Vietnamese Lunar Calendar**: Accurately shows lunar day and month (e.g. `17/9`, `4/9`...).
- **Color Highlighting**: Highlights full moons (day 15), new moons (day 1), and traditional holidays in red (`#FF5A5A`).
- **Full Sexagenary Cycle Header**: Displays the complete lunar month and Can Chi year in the header: e.g. `4 Tháng 9 Năm Bính Ngọ`.
- **Standalone Astronomical Engine**: Embedded Hồ Ngọc Đức Vietnamese astronomical lunar algorithm runs completely offline in-memory.
- **High Contrast on Accent Circle**: Automatically renders today's lunar date with high-contrast color (black on light blue accent) so it is never washed out.
- **No Chinese Characters**: Clean, 100% Vietnamese localized calendar experience.

## How to activate
In order for Windows 11 to allocate the secondary subtitle line in the calendar flyout cells for this mod to display lunar dates:
1. Open Windows Settings (`Win + I`) > **Time & language** > **Date & time**.
2. Under **"Show additional calendars in the taskbar"**, select **"Simplified Chinese (Lunar)"**.
   *(This mod intercepts the text and replaces all Chinese characters with 100% Vietnamese lunar dates).*

---

# Lịch Âm Việt Nam cho Windows 11 (Tiếng Việt)

Hiển thị ngày Âm lịch Việt Nam, Mùng 1, Rằm và các dịp Lễ Tết truyền thống trực tiếp trên bảng Lịch (Notification Center Calendar Flyout) của Windows 11 khi nhấn vào đồng hồ ở Taskbar.

## Tính năng chính
- Hiển thị ngày Âm lịch Việt Nam thuần túy (ví dụ: `17/9`, `4/9`...).
- Tô màu đỏ nổi bật (`#FF5A5A`) cho ngày Rằm (15), Mùng 1 (1) và các dịp Lễ Tết truyền thống.
- Tiêu đề tháng hiển thị đầy đủ Năm Can Chi: `4 Tháng 9 Năm Bính Ngọ`.
- Tích hợp sẵn thuật toán thiên văn học Hồ Ngọc Đức chạy offline độc lập trong bộ nhớ.
- Tự động đổi màu tương phản cao (màu đen) cho ngày hôm nay trên nền tròn xanh để không bị chìm chữ.
- Hoàn toàn 100% Tiếng Việt, loại bỏ sạch chữ Hán mặc định.

## Yêu cầu kích hoạt
Để Windows cấp phát dòng chữ thứ hai trong từng ô ngày cho Mod hiển thị:
1. Mở Windows Settings (Win + I) > Time & language > Date & time.
2. Tại mục "Show additional calendars in the taskbar", chọn "Simplified Chinese (Lunar)".
   *(Mod sẽ tự động thay thế toàn bộ chữ này bằng Lịch Âm Việt Nam 100%).*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replaceText: true
  $name: "Replace with Vietnamese lunar text"
  $name:vi-VN: "Hiển thị ngày âm lịch Việt Nam"
  $description: "Replace the secondary subtitle line with Vietnamese lunar dates (e.g. 17/9, 4/9)."
  $description:vi-VN: "Thay thế dòng phụ trong ô ngày bằng ngày âm lịch Việt Nam (vd: 17/9, 4/9)."
- specialDayColor: "FF5A5A"
  $name: "Special days color (Full moon, New moon, Holidays)"
  $name:vi-VN: "Màu ngày Rằm, Mùng 1 & Lễ Tết (Mặc định đỏ)"
  $description: "Hex color code (RRGGBB) for full moon (day 15), new moon (day 1), and traditional holidays."
  $description:vi-VN: "Mã màu Hex (RRGGBB) cho ngày Rằm, Mùng 1 và các ngày Lễ Tết cổ truyền."
- normalDayColor: "A0A0A0"
  $name: "Normal lunar day color"
  $name:vi-VN: "Màu ngày âm thông thường (Mặc định xám sáng)"
  $description: "Hex color code (RRGGBB) for standard lunar days."
  $description:vi-VN: "Mã màu Hex (RRGGBB) cho các ngày âm lịch bình thường."
- todayDayColor: "000000"
  $name: "Today lunar day color (High contrast)"
  $name:vi-VN: "Màu ngày hôm nay (Tương phản cao)"
  $description: "Hex color code (RRGGBB) for today's cell with accent background circle (default black for optimal contrast)."
  $description:vi-VN: "Mã màu Hex (RRGGBB) trên ô ngày hôm nay có nền tròn xanh (mặc định đen để nhìn rõ nét)."
- todaySpecialDayColor: "8B0000"
  $name: "Today special day color"
  $name:vi-VN: "Màu ngày Lễ/Rằm/Mùng 1 khi trùng ngày hôm nay"
  $description: "Hex color code (RRGGBB) when today coincides with a full moon, new moon, or holiday."
  $description:vi-VN: "Mã màu Hex (RRGGBB) nếu ngày hôm nay trùng với Rằm, Mùng 1 hoặc ngày Lễ."
- dataSourceUrl: "https://raw.githubusercontent.com/cmduyeen/lich-am-viet-nam/main/output/viet_lunar_latest.ics"
  $name: "Optional ICS data source URL"
  $name:vi-VN: "Nguồn dữ liệu ICS tùy chọn"
  $description: "Optional iCalendar (.ics) URL or local file path for additional holiday definitions."
  $description:vi-VN: "Tùy chọn nạp thêm ngày lễ từ link iCalendar hoặc đường dẫn file .ics trong máy."
- refreshIntervalHours: 24
  $name: "Refresh interval (hours)"
  $name:vi-VN: "Chu kỳ cập nhật (giờ)"
  $description: "Automatically refresh the ICS calendar after this many hours."
  $description:vi-VN: "Tự động tải lại file lịch sau số giờ này (mặc định 24h)."
*/
// ==/WindhawkModSettings==

//
// The way the calendar window is found - hooking CreateWindowInBand(Ex), running on the
// window's own UI thread and walking the XAML tree from Window::Current() - follows the
// "Agenda in Calendar View" mod by lonfro, which is licensed under GPL-3.0:
//
//   Copyright (C) 2026 lonfro
//
// The helper which runs code on another window's thread is taken from the "Windows 11
// Notification Center Styler" mod by m417z, also GPL-3.0:
//
//   Copyright (C) 2026 m417z
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>

thread_local bool g_initializedForThread;

// What this mod deliberately does not do: it is not a consumer of the XAML Diagnostics
// TAP (InitializeXamlDiagnosticsEx). Only one consumer of it can be active in a process
// at a time, so a mod which uses it stops working as soon as the next one is enabled -
// and the popular styling mods of this very process (Windows 11 Notification Center
// Styler, for one) are consumers. The calendar window is reached the way "Agenda in
// Calendar View" does it instead: CreateWindowInBand(Ex) and ShowWindow are hooked, the
// mod runs on the window's own UI thread, and the XAML tree is read from
// Window::Current() there (see CollectDayItemsOnCurrentThread).

////////////////////////////////////////////////////////////////////////////////

#pragma region winrt_hpp

#include <Unknwn.h>

#include <winrt/base.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation {}
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;

#pragma endregion  // winrt_hpp

// WindhawkUtils::StringSetting and WindhawkUtils::SetFunctionHook. The header ships with
// the compiler Windhawk builds mods with.
#include <windhawk_utils.h>

////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

void LogDebug(const wchar_t* format, ...) {
    FILE* fp = nullptr;
    if (_wfopen_s(&fp, L"C:\\ProgramData\\Windhawk\\lunar_debug.log", L"a, ccs=UTF-8") == 0 && fp) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fwprintf(fp, L"[%02d:%02d:%02d.%03d][PID:%u][TID:%u] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentProcessId(), GetCurrentThreadId());
        va_list args;
        va_start(args, format);
        vfwprintf(fp, format, args);
        va_end(args);
        fputc(L'\n', fp);
        fclose(fp);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Settings

// The snapshot of the settings. The defaults are written twice on purpose: here, for
// a setting which cannot be read, and in the WindhawkModSettings YAML, which is what
// the settings UI starts from, so the two have to be changed together. LoadSettings
// publishes a whole new snapshot as a shared_ptr to an immutable object, and every
// caller takes that pointer rather than a copy of the struct: a cell is looked at on
// every pass of the sweep, and copying three strings for each of them is work with no
// purpose behind it.
struct Settings {
    std::wstring dataSourceUrl = L"https://raw.githubusercontent.com/cmduyeen/lich-am-viet-nam/main/output/viet_lunar_latest.ics";
    int refreshIntervalHours = 24;
    bool replaceText = true;
    winrt::Windows::UI::Color specialDayColor{255, 255, 90, 90}; // Red (#FF5A5A)
    winrt::Windows::UI::Color normalDayColor{255, 160, 160, 160}; // Grey (#A0A0A0)
    winrt::Windows::UI::Color todayDayColor{255, 0, 0, 0}; // Black (#000000) for high contrast on blue accent circle!
    winrt::Windows::UI::Color todaySpecialDayColor{255, 139, 0, 0}; // Deep Dark Red (#8B0000) for Mùng 1 / Rằm on blue accent circle!
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings = std::make_shared<Settings>();

std::shared_ptr<const Settings> GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// Wh_GetStringSetting never answers null (a setting which cannot be read comes back as an
// empty string, see windhawk_api.h), and the StringSetting wrapper frees the copy the
// engine hands out, so there is nothing to check here and nothing to free by hand.
std::wstring ReadStringSetting(PCWSTR name) {
    const WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(name);
    return std::wstring(value.get());
}

// Colour parsing, and why so many written forms are accepted: a colourRgb setting holds
// "RRGGBB" without a '#' (that is what Windhawk's colour picker stores), while a value
// typed by hand is often written with one, or as eight digits (AARRGGBB) by someone used
// to CSS. Both lengths, with or without the '#', are accepted; a value which does not
// parse returns nullopt and the caller falls back to the default colour instead of
// turning the calendar black.
std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring const& text) {
    std::wstring value;
    for (wchar_t c : text) {
        if (!iswspace(c)) {
            value += c;
        }
    }
    if (!value.empty() && value[0] == L'#') {
        value = value.substr(1);
    }
    if (value.size() != 6 && value.size() != 8) {
        return std::nullopt;
    }

    auto hexDigit = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') {
            return c - L'0';
        }
        if (c >= L'a' && c <= L'f') {
            return c - L'a' + 10;
        }
        if (c >= L'A' && c <= L'F') {
            return c - L'A' + 10;
        }
        return -1;
    };

    uint8_t bytes[4] = {0xFF, 0x00, 0x00, 0x00};
    size_t offset = value.size() == 8 ? 0 : 1;
    for (size_t i = 0; i + 1 < value.size(); i += 2) {
        int hi = hexDigit(value[i]);
        int lo = hexDigit(value[i + 1]);
        if (hi < 0 || lo < 0) {
            return std::nullopt;
        }
        bytes[offset + i / 2] = static_cast<uint8_t>((hi << 4) | lo);
    }

    return winrt::Windows::UI::Color{bytes[0], bytes[1], bytes[2], bytes[3]};
}

// A colour setting which can't be read (an empty value, or a configuration file
// from an older version of the mod) falls back to the default colour.
winrt::Windows::UI::Color ReadColorSetting(PCWSTR name,
                                           winrt::Windows::UI::Color fallback) {
    if (auto color = ParseHexColor(ReadStringSetting(name))) {
        return *color;
    }
    return fallback;
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();

    settings->dataSourceUrl = ReadStringSetting(L"dataSourceUrl");
    if (settings->dataSourceUrl.empty()) {
        settings->dataSourceUrl = L"https://raw.githubusercontent.com/cmduyeen/lich-am-viet-nam/main/output/viet_lunar_latest.ics";
    }
    settings->refreshIntervalHours = Wh_GetIntSetting(L"refreshIntervalHours");
    settings->replaceText = Wh_GetIntSetting(L"replaceText") != 0;
    settings->specialDayColor =
        ReadColorSetting(L"specialDayColor", Settings{}.specialDayColor);
    settings->normalDayColor =
        ReadColorSetting(L"normalDayColor", Settings{}.normalDayColor);
    settings->todayDayColor =
        ReadColorSetting(L"todayDayColor", Settings{}.todayDayColor);
    settings->todaySpecialDayColor =
        ReadColorSetting(L"todaySpecialDayColor", Settings{}.todaySpecialDayColor);

    if (settings->refreshIntervalHours < 1) {
        settings->refreshIntervalHours = 1;
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = settings;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Small helpers

std::wstring TrimWhitespace(std::wstring const& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && iswspace(text[begin])) {
        begin++;
    }
    while (end > begin && iswspace(text[end - 1])) {
        end--;
    }
    return text.substr(begin, end - begin);
}

std::wstring Utf8ToWide(const char* data, size_t length) {
    if (!data || !length) {
        return {};
    }
    int size =
        MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(length), nullptr, 0);
    if (size <= 0) {
        return {};
    }
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(length), result.data(),
                        size);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Holiday data

struct HolidayInfo {
    std::wstring name;       // "4/9" or "1/8"
    std::wstring fullDesc;   // "Ngày 4 tháng 9 âm lịch (Bính Ngọ)"
    std::wstring headerText; // "4 Tháng 9 Năm Bính Ngọ"
    bool isSpecial = false;  // true for Mung 1, Ram, Tet, Le
};

using HolidayMap = std::unordered_map<int32_t, HolidayInfo>;

// A date is an int32 "yyyymmdd", so that integer comparison is date comparison and the
// same value serves both as a map key and as a range endpoint (DTSTART < key < DTEND).
// The price is that the value is not continuous - 20260228 + 1 is not 20260301 - so
// every day-by-day step has to go through NextDateKey() and never through arithmetic.
int32_t MakeDateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

// The number of days of a month, or 0 when "month" is not a month. The leap rule
// lives here so that the expansion (NextDateKey) and the validation (IsValidDate)
// can never disagree about the 29th of February.
int DaysInMonth(int year, int month) {
    if (month < 1 || month > 12) {
        return 0;
    }
    static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
        return 29;
    }
    return kDays[month - 1];
}

// Whether a year/month/day triple is a date which exists. Every date the parser
// accepts goes through this, because a date which does not exist is not harmless
// here: a range is walked with NextDateKey(), which turns the "+1 day" of
// 2026-02-31 into 2026-03-03, so a typo in a feed would mark a real date which
// the feed never named. The year range keeps MakeDateKey() (year*10000+month*100
// +day) inside an int32 as well, and turns away a "19700101" placeholder.
bool IsValidDate(int year, int month, int day) {
    if (year < 1900 || year > 9999) {
        return false;
    }
    return day >= 1 && day <= DaysInMonth(year, month);
}

// Accepts "2026-01-01", "2026/1/1", "20260101" and "20260101T000000Z". A time
// which follows a date is a part of its own, and the date it belongs to is used
// as it is: the holidays of this source are whole days ("VALUE=DATE"), and a
// source which sends midnight UTC means the same day here.
int32_t ParseDateKey(std::wstring const& text) {
    std::vector<int> parts;
    std::wstring current;
    for (wchar_t c : text) {
        if (c >= L'0' && c <= L'9') {
            current += c;
        } else if (!current.empty()) {
            parts.push_back((int)wcstol(current.c_str(), nullptr, 10));
            current.clear();
        }
    }
    if (!current.empty()) {
        parts.push_back((int)wcstol(current.c_str(), nullptr, 10));
    }

    // The compact form is recognised by "the first part is eight digits" and not
    // by "there is exactly one part": "20260101T000000Z" comes out of the split
    // above as two parts. A run of digits too long for an int32 is returned as
    // LONG_MAX by wcstol and is then turned away by the year check below.
    int year = 0;
    int month = 0;
    int day = 0;
    if (!parts.empty() && parts[0] >= 10000101) {
        int value = parts[0];
        year = value / 10000;
        month = (value / 100) % 100;
        day = value % 100;
    } else if (parts.size() >= 3) {
        year = parts[0];
        month = parts[1];
        day = parts[2];
    } else {
        return 0;
    }

    if (!IsValidDate(year, month, day)) {
        return 0;
    }
    return MakeDateKey(year, month, day);
}

// The next day. The leap rule lives in DaysInMonth, so a 2024-02-29 in the feed is
// stepped over correctly.
int32_t NextDateKey(int32_t key) {
    int year = key / 10000;
    int month = (key / 100) % 100;
    int day = key % 100;

    int days = DaysInMonth(year, month);
    if (days == 0) {
        return 0;
    }

    day++;
    if (day > days) {
        day = 1;
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }

    return MakeDateKey(year, month, day);
}

// ============================================================================
// Built-in Vietnamese Lunar Calendar Calculation (Ho Ngoc Duc Algorithm)
// ============================================================================

namespace VietnameseLunar {

constexpr double kPI = 3.14159265358979323846;

inline int INT(double d) {
    return static_cast<int>(std::floor(d));
}

inline int jdFromDate(int dd, int mm, int yy) {
    int a = (14 - mm) / 12;
    int y = yy + 4800 - a;
    int m = mm + 12 * a - 3;
    int jd = dd + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    if (jd < 2299161) {
        jd = dd + (153 * m + 2) / 5 + 365 * y + y / 4 - 32083;
    }
    return jd;
}

inline double NewMoon(int k) {
    double T = k / 1236.85;
    double T2 = T * T;
    double T3 = T2 * T;
    double dr = kPI / 180.0;
    double Jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * T2 - 0.000000155 * T3;
    Jd1 += 0.00033 * std::sin((166.56 + 132.87 * T - 0.009173 * T2) * dr);
    double M = 359.2242 + 29.10535608 * k - 0.0000333 * T2 - 0.00000347 * T3;
    double Mpr = 306.0253 + 385.81691806 * k + 0.0107306 * T2 + 0.00001236 * T3;
    double F = 21.2964 + 390.67050646 * k - 0.0016528 * T2 - 0.00000239 * T3;
    double C1 = (0.1734 - 0.000393 * T) * std::sin(M * dr) + 0.0021 * std::sin(2.0 * dr * M);
    C1 -= 0.4068 * std::sin(Mpr * dr) + 0.0161 * std::sin(2.0 * dr * Mpr);
    C1 -= 0.0004 * std::sin(3.0 * dr * Mpr);
    C1 += 0.0104 * std::sin(2.0 * dr * F) - 0.0051 * std::sin(dr * (M + Mpr));
    C1 -= 0.0074 * std::sin(dr * (M - Mpr)) + 0.0004 * std::sin(dr * (2.0 * F + M));
    C1 -= 0.0004 * std::sin(dr * (2.0 * F - M)) - 0.0006 * std::sin(dr * (2.0 * F + Mpr));
    C1 += 0.0010 * std::sin(dr * (2.0 * F - Mpr)) + 0.0005 * std::sin(dr * (2.0 * Mpr + M));
    double deltat = (T < -11.0)
        ? (0.001 + 0.000839 * T + 0.0002261 * T2 - 0.00000845 * T3 - 0.000000081 * T * T3)
        : (-0.000278 + 0.000265 * T + 0.000262 * T2);
    return Jd1 + C1 - deltat;
}

inline double SunLongitude(double jdn) {
    double T = (jdn - 2451545.0) / 36525.0;
    double T2 = T * T;
    double dr = kPI / 180.0;
    double M = 357.52910 + 35999.05030 * T - 0.0001559 * T2 - 0.00000048 * T * T2;
    double L0 = 280.46645 + 36000.76983 * T + 0.0003032 * T2;
    double DL = (1.914600 - 0.004817 * T - 0.000014 * T2) * std::sin(dr * M);
    DL += (0.019993 - 0.000101 * T) * std::sin(dr * 2.0 * M) + 0.000290 * std::sin(dr * 3.0 * M);
    double L = (L0 + DL) * dr;
    L -= kPI * 2.0 * INT(L / (kPI * 2.0));
    return L;
}

inline int getSunLongitude(int dayNumber, double timeZone = 7.0) {
    return INT(SunLongitude(dayNumber - 0.5 - timeZone / 24.0) / kPI * 6.0);
}

inline int getNewMoonDay(int k, double timeZone = 7.0) {
    return INT(NewMoon(k) + 0.5 + timeZone / 24.0);
}

inline int getLunarMonth11(int yy, double timeZone = 7.0) {
    int off = jdFromDate(31, 12, yy) - 2415021;
    int k = INT(off / 29.530588853);
    int nm = getNewMoonDay(k, timeZone);
    int sunLong = getSunLongitude(nm, timeZone);
    if (sunLong >= 9) {
        nm = getNewMoonDay(k - 1, timeZone);
    }
    return nm;
}

inline int getLeapMonthOffset(int a11, double timeZone = 7.0) {
    int k = INT((a11 - 2415021.076998695) / 29.530588853 + 0.5);
    int last = 0;
    int i = 1;
    int arc = getSunLongitude(getNewMoonDay(k + i, timeZone), timeZone);
    do {
        last = arc;
        i++;
        arc = getSunLongitude(getNewMoonDay(k + i, timeZone), timeZone);
    } while (arc != last && i < 14);
    return i - 1;
}

inline void convertSolar2Lunar(int dd, int mm, int yy, int& lunarDay, int& lunarMonth, int& lunarYear, int& lunarLeap, double timeZone = 7.0) {
    int dayNumber = jdFromDate(dd, mm, yy);
    int k = INT((dayNumber - 2415021.076998695) / 29.530588853);
    int monthStart = getNewMoonDay(k + 1, timeZone);
    if (monthStart > dayNumber) {
        monthStart = getNewMoonDay(k, timeZone);
    }
    int a11 = getLunarMonth11(yy, timeZone);
    int b11 = a11;
    if (a11 >= monthStart) {
        lunarYear = yy;
        a11 = getLunarMonth11(yy - 1, timeZone);
    } else {
        lunarYear = yy + 1;
        b11 = getLunarMonth11(yy + 1, timeZone);
    }
    lunarDay = dayNumber - monthStart + 1;
    int diff = INT((monthStart - a11) / 29.0);
    lunarLeap = 0;
    lunarMonth = diff + 11;
    if (b11 - a11 > 365) {
        int leapMonthDiff = getLeapMonthOffset(a11, timeZone);
        if (diff >= leapMonthDiff) {
            lunarMonth = diff + 10;
            if (diff == leapMonthDiff) {
                lunarLeap = 1;
            }
        }
    }
    if (lunarMonth > 12) {
        lunarMonth -= 12;
    }
    if (lunarMonth >= 11 && diff < 4) {
        lunarYear -= 1;
    }
}

inline std::wstring getCanChiYear(int lunarYear) {
    static const wchar_t* const CAN[] = {
        L"Giáp", L"Ất", L"Bính", L"Đinh", L"Mậu", L"Kỷ", L"Canh", L"Tân", L"Nhâm", L"Quý"
    };
    static const wchar_t* const CHI[] = {
        L"Tý", L"Sửu", L"Dần", L"Mão", L"Thìn", L"Tỵ", L"Ngọ", L"Mùi", L"Thân", L"Dậu", L"Tuất", L"Hợi"
    };
    int canIdx = (lunarYear + 6) % 10;
    if (canIdx < 0) canIdx += 10;
    int chiIdx = (lunarYear + 8) % 12;
    if (chiIdx < 0) chiIdx += 12;
    return std::wstring(CAN[canIdx]) + L" " + CHI[chiIdx];
}

} // namespace VietnameseLunar

HolidayInfo CalculateVietnameseLunar(int32_t key) {
    int y = key / 10000;
    int m = (key / 100) % 100;
    int d = key % 100;

    int lDay = 0, lMonth = 0, lYear = 0, lLeap = 0;
    VietnameseLunar::convertSolar2Lunar(d, m, y, lDay, lMonth, lYear, lLeap);

    HolidayInfo info;
    info.name = std::to_wstring(lDay) + L"/" + std::to_wstring(lMonth);
    
    // Check if Special (Mùng 1, Rằm, or Holiday)
    if (lDay == 1 || lDay == 15) {
        info.isSpecial = true;
    }
    // Lunar Holidays:
    // Tết Nguyên Đán (1/1, 2/1, 3/1)
    if (lMonth == 1 && (lDay >= 1 && lDay <= 3)) {
        info.isSpecial = true;
    }
    // Tết Nguyên Tiêu (15/1)
    if (lMonth == 1 && lDay == 15) {
        info.isSpecial = true;
    }
    // Giỗ Tổ Hùng Vương (10/3)
    if (lMonth == 3 && lDay == 10) {
        info.isSpecial = true;
    }
    // Lễ Phật Đản (15/4)
    if (lMonth == 4 && lDay == 15) {
        info.isSpecial = true;
    }
    // Tết Đoan Ngọ (5/5)
    if (lMonth == 5 && lDay == 5) {
        info.isSpecial = true;
    }
    // Lễ Vu Lan (15/7)
    if (lMonth == 7 && lDay == 15) {
        info.isSpecial = true;
    }
    // Tết Trung Thu (15/8)
    if (lMonth == 8 && lDay == 15) {
        info.isSpecial = true;
    }
    // Ông Công Ông Táo (23/12)
    if (lMonth == 12 && lDay == 23) {
        info.isSpecial = true;
    }
    // Solar Holidays:
    // Tết Dương Lịch (1/1)
    if (m == 1 && d == 1) {
        info.isSpecial = true;
    }
    // 30/4
    if (m == 4 && d == 30) {
        info.isSpecial = true;
    }
    // 1/5
    if (m == 5 && d == 1) {
        info.isSpecial = true;
    }
    // Quốc Khánh (2/9)
    if (m == 9 && d == 2) {
        info.isSpecial = true;
    }

    std::wstring canChi = VietnameseLunar::getCanChiYear(lYear);
    info.headerText = std::to_wstring(lDay) + L" Tháng " + std::to_wstring(lMonth) + L" Năm " + canChi;
    
    wchar_t descBuf[256];
    swprintf_s(descBuf, L"Ngày %d tháng %d âm lịch (%s)", lDay, lMonth, canChi.c_str());
    info.fullDesc = descBuf;

    return info;
}

std::mutex g_holidaysMutex;
HolidayMap g_holidays;

bool LookupHoliday(int32_t key, HolidayInfo& out) {
    {
        std::lock_guard<std::mutex> lock(g_holidaysMutex);
        auto it = g_holidays.find(key);
        if (it != g_holidays.end()) {
            out = it->second;
            return true;
        }
    }
    out = CalculateVietnameseLunar(key);
    return true;
}

// The feed covers every year at once, so the data replaces the old data.
void StoreHolidays(HolidayMap parsed) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    g_holidays = std::move(parsed);

    // Update today's lunar text for Taskbar Clock
    SYSTEMTIME st;
    GetLocalTime(&st);
    int32_t todayKey = MakeDateKey(st.wYear, st.wMonth, st.wDay);
    auto it = g_holidays.find(todayKey);
    if (it != g_holidays.end()) {
        std::wstring lunarText;
        int d = 0, m = 0;
        wchar_t canChi[128] = { 0 };
        if (swscanf_s(it->second.fullDesc.c_str(), L"Ngày %d tháng %d âm lịch (%127[^)])", &d, &m, canChi, (unsigned)_countof(canChi)) == 3) {
            wchar_t buf[256];
            swprintf_s(buf, L"%02d/%02d %s", d, m, canChi);
            lunarText = buf;
        } else {
            lunarText = it->second.name;
        }
        FILE* fp = nullptr;
        if (_wfopen_s(&fp, L"C:\\ProgramData\\Windhawk\\lunar_today.txt", L"w, ccs=UTF-8") == 0 && fp) {
            fputws(lunarText.c_str(), fp);
            fclose(fp);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// iCalendar parsing
//
// The default data source is an .ics feed with one VEVENT per holiday period:
//
//   BEGIN:VEVENT
//   DTSTART;VALUE=DATE:20261013
//   DTEND;VALUE=DATE:20261014
//   SUMMARY:4/9
//   DESCRIPTION:Ngay 4 thang 9 am lich (Binh Ngo)
//   END:VEVENT
//
// DTEND is exclusive, as RFC 5545 requires.

std::vector<std::wstring> SplitIcsLines(std::wstring const& text) {
    std::vector<std::wstring> lines;
    std::wstring current;

    auto flush = [&]() {
        if (current.empty()) {
            return;
        }
        // A line starting with a space or a tab continues the previous one.
        if ((current[0] == L' ' || current[0] == L'\t') && !lines.empty()) {
            lines.back() += current.substr(1);
        } else {
            lines.push_back(current);
        }
        current.clear();
    };

    for (size_t i = 0; i < text.size(); i++) {
        wchar_t c = text[i];
        if (c == L'\r' || c == L'\n') {
            flush();
            if (c == L'\r' && i + 1 < text.size() && text[i + 1] == L'\n') {
                i++;
            }
        } else {
            current += c;
        }
    }
    flush();

    return lines;
}

std::wstring UnescapeIcsText(std::wstring const& text) {
    std::wstring result;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] != L'\\' || i + 1 >= text.size()) {
            result += text[i];
            continue;
        }
        switch (text[++i]) {
            case L'n':
            case L'N':
                result += L' ';
                break;
            case L'\\':
                result += L'\\';
                break;
            case L';':
                result += L';';
                break;
            case L',':
                result += L',';
                break;
            default:
                result += text[i];
                break;
        }
    }
    return result;
}

// Parses Vietnamese Lunar ICS entries:
// "4/9" -> normal lunar day.
// "1/10 Mùng 1" or "15/9 Rằm" -> special day.
// "Ngày 4 tháng 9 âm lịch (Bính Ngọ)" -> header "4 Tháng 9 Năm Bính Ngọ".
bool ClassifyIcsSummary(std::wstring const& summary,
                        std::wstring const& description,
                        std::wstring& name,
                        std::wstring& fullDesc,
                        std::wstring& headerText,
                        bool& isSpecial) {
    std::wstring text = UnescapeIcsText(summary);
    text = TrimWhitespace(text);
    if (text.empty()) {
        return false;
    }

    fullDesc = UnescapeIcsText(description);

    // Format in viet_lunar_latest.ics:
    // "4/9"
    // "1/12   Mùng 1"
    // "15/11   Rằm"
    // "13/11   Tết Dương Lịch"
    size_t spacePos = text.find_first_of(L" \t");
    if (spacePos != std::wstring::npos) {
        name = TrimWhitespace(text.substr(0, spacePos));
        isSpecial = true;
    } else {
        name = text;
        // Mung 1 or Ram without title
        if (name.starts_with(L"1/") || name.starts_with(L"15/")) {
            isSpecial = true;
        } else {
            isSpecial = false;
        }
    }

    // Generate clean headerText from description:
    // e.g. "Ngày 4 tháng 9 âm lịch (Bính Ngọ)" -> "4 Tháng 9 Năm Bính Ngọ"
    if (!fullDesc.empty()) {
        int d = 0, m = 0;
        wchar_t yearCanChi[128] = { 0 };
        if (swscanf_s(fullDesc.c_str(), L"Ngày %d tháng %d âm lịch (%127[^)])", &d, &m, yearCanChi, (unsigned)_countof(yearCanChi)) == 3) {
            wchar_t buf[256];
            swprintf_s(buf, L"%d Tháng %d Năm %s", d, m, yearCanChi);
            headerText = buf;
        } else {
            headerText = name;
        }
    } else {
        headerText = name;
    }

    return !name.empty();
}

std::atomic<bool> g_loggedRecurrence{false};

// The most dates a feed may describe. The address of the data source can be
// changed, and an event written as "from 1900 to 2100" would otherwise keep the
// worker thread busy for tens of thousands of dates, which would also hold up
// the next refresh and the shutdown.
constexpr size_t kMaxHolidayDates = 20000;

bool ParseIcs(std::wstring const& text, HolidayMap& out) {
    auto lines = SplitIcsLines(text);

    bool inEvent = false;
    bool hasRecurrence = false;
    bool hitDateLimit = false;
    bool hasEnd = false;
    size_t droppedRanges = 0;
    int32_t start = 0;
    int32_t end = 0;
    std::wstring summary;
    std::wstring description;
    size_t added = 0;

    auto flushEvent = [&]() {
        if (!start) {
            return;
        }
        if (added >= kMaxHolidayDates) {
            // An earlier event already filled the feed: this one is dropped.
            hitDateLimit = true;
            return;
        }

        std::wstring name;
        std::wstring fullDesc;
        std::wstring headerText;
        bool isSpecial = false;
        if (!ClassifyIcsSummary(summary, description, name, fullDesc, headerText, isSpecial)) {
            return;
        }

        if (!hasEnd) {
            // No DTEND at all: a zero-length event, which is the single day it
            // starts on. That is what a feed which spells out one day per event
            // sends, and the only case which is read that way.
            end = NextDateKey(start);
        } else if (end <= start) {
            // A DTEND which is there but does not make a range: it did not parse,
            // or it is not later than DTSTART. The event is dropped instead of being
            // read as a single day, because a wrong end would put the holiday name
            // on dates the feed never named.
            droppedRanges++;
            return;
        }

        for (int32_t key = start; key && key < end; key = NextDateKey(key)) {
            // The limit is looked at inside the loop as well: a single event
            // which spans millions of days is what a limit at the start of the
            // event does not catch.
            if (added >= kMaxHolidayDates) {
                hitDateLimit = true;
                break;
            }

            HolidayInfo info;
            info.name = name;
            info.fullDesc = fullDesc;
            info.headerText = headerText;
            info.isSpecial = isSpecial;
            out[key] = std::move(info);
            added++;
        }
    };

    for (const auto& line : lines) {
        if (line == L"BEGIN:VEVENT") {
            inEvent = true;
            start = 0;
            end = 0;
            hasEnd = false;
            summary.clear();
            description.clear();
            continue;
        }
        if (line == L"END:VEVENT") {
            if (inEvent) {
                flushEvent();
            }
            inEvent = false;
            continue;
        }
        if (!inEvent) {
            continue;
        }

        size_t colon = line.find(L':');
        if (colon == std::wstring::npos) {
            continue;
        }

        std::wstring name = line.substr(0, colon);
        std::wstring value = line.substr(colon + 1);
        size_t semicolon = name.find(L';');
        if (semicolon != std::wstring::npos) {
            name = name.substr(0, semicolon);
        }

        if (name == L"DTSTART") {
            start = ParseDateKey(value);
        } else if (name == L"DTEND") {
            end = ParseDateKey(value);
            hasEnd = true;
        } else if (name == L"SUMMARY") {
            summary = value;
        } else if (name == L"DESCRIPTION") {
            description = value;
        } else if (name == L"RRULE" || name == L"EXDATE" ||
                   name == L"RECURRENCE-ID") {
            // A recurring event is read as the one occurrence it names. The kind of
            // feed the README describes - one event per holiday period - doesn't use
            // any of these fields.
            hasRecurrence = true;
        }
    }

    if (hasRecurrence && !g_loggedRecurrence.exchange(true)) {
        Wh_Log(
            L"The data source uses recurrence rules (RRULE, EXDATE or "
            L"RECURRENCE-ID); only the occurrence each event names is read");
    }

    if (hitDateLimit) {
        Wh_Log(L"The data source describes more than %zu holiday dates; the "
               L"rest is ignored",
               kMaxHolidayDates);
    }

    if (droppedRanges) {
        Wh_Log(L"%zu events are dropped: their DTEND is missing a date, or does "
               L"not come after their DTSTART",
               droppedRanges);
    }

    return !out.empty();
}


// The data source is an iCalendar feed: the only format the mod reads.
bool ParseHolidayData(std::wstring const& body, HolidayMap& out) {
    if (body.find(L"BEGIN:VEVENT") == std::wstring::npos &&
        body.find(L"BEGIN:VCALENDAR") == std::wstring::npos) {
        Wh_Log(L"The data source did not return an iCalendar feed");
        return false;
    }

    return ParseIcs(body, out);
}

////////////////////////////////////////////////////////////////////////////////
// Background fetching

void SweepDayItems();
void StartSweepTimer();
void RefreshCalendarOnUiThreads();

// The worker thread exists only while a data source is set: it is created by the
// first refresh request which has something to fetch, and a failed start is retried
// by the next one (see RequestHolidayData). The mutex is what keeps two threads which
// ask at the same time from creating two threads or two event pairs.
std::mutex g_workerThreadMutex;
std::atomic<HANDLE> g_hWorkerThread{nullptr};
std::atomic<HANDLE> g_hWorkEvent{nullptr};
std::atomic<HANDLE> g_hStopEvent{nullptr};
std::atomic<bool> g_fetchPending{false};
// Set while the mod is unloading: a refresh request which arrives during the
// unload must not start a worker thread again.
std::atomic<bool> g_shuttingDown{false};

std::mutex g_fetchStateMutex;
ULONGLONG g_lastFetchAttemptTick = 0;
bool g_haveData = false;

// After a failed request, retry after this delay instead of waiting for the
// whole refresh interval.
constexpr ULONGLONG kFetchRetryDelayMs = 10 * 60 * 1000;

// Fetching and parsing both happen on the worker thread: Wh_GetUrlContent blocks on
// network I/O, and doing that on a UI thread would hold up the taskbar. A parse which
// fails does not overwrite the data which is already there, so the calendar keeps
// showing the last good result when the network or the API is down (g_haveData only
// decides which retry delay applies). An empty address is not a failure to report: it
// means the mod has no data source, and RequestHolidayData never gets here then.
bool FetchHolidayData() {
    const std::shared_ptr<const Settings> settings = GetSettings();

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_lastFetchAttemptTick = GetTickCount64();
    }

    if (settings->dataSourceUrl.empty()) {
        return false;
    }

    Wh_Log(L"Loading calendar data from %s", settings->dataSourceUrl.c_str());

    std::wstring body;
    bool ok = false;

    // 1. Try local file path if configured
    if (settings->dataSourceUrl.find(L":\\") != std::wstring::npos ||
        settings->dataSourceUrl.find(L":/") != std::wstring::npos) {
        FILE* fp = nullptr;
        if (_wfopen_s(&fp, settings->dataSourceUrl.c_str(), L"rb") == 0 && fp) {
            fseek(fp, 0, SEEK_END);
            long fsize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            if (fsize > 0 && fsize < 50 * 1024 * 1024) {
                std::vector<char> buf(fsize);
                fread(buf.data(), 1, fsize, fp);
                body = Utf8ToWide(buf.data(), fsize);
                ok = !body.empty();
            }
            fclose(fp);
        }
    }

    // 2. Try URL
    if (!ok && (settings->dataSourceUrl.starts_with(L"http://") || settings->dataSourceUrl.starts_with(L"https://"))) {
        const WH_URL_CONTENT* content =
            Wh_GetUrlContent(settings->dataSourceUrl.c_str(), nullptr);
        if (content) {
            if (content->statusCode == 200 && content->data && content->length) {
                body = Utf8ToWide(content->data, content->length);
                ok = !body.empty();
            } else {
                Wh_Log(L"Wh_GetUrlContent returned HTTP status %d", content->statusCode);
            }
            Wh_FreeUrlContent(content);
        } else {
            Wh_Log(L"Wh_GetUrlContent returned null");
        }
    }

    // 3. Fallback: check Downloads folder
    if (!ok) {
        std::wstring fallbackPath = L"C:\\Users\\Admin\\Downloads\\viet_lunar_latest.ics";
        FILE* fp = nullptr;
        if (_wfopen_s(&fp, fallbackPath.c_str(), L"rb") == 0 && fp) {
            fseek(fp, 0, SEEK_END);
            long fsize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            if (fsize > 0 && fsize < 50 * 1024 * 1024) {
                std::vector<char> buf(fsize);
                fread(buf.data(), 1, fsize, fp);
                body = Utf8ToWide(buf.data(), fsize);
                ok = !body.empty();
                if (ok) {
                    Wh_Log(L"Loaded fallback data from %s", fallbackPath.c_str());
                }
            }
            fclose(fp);
        }
    }

    if (!ok || body.empty()) {
        return false;
    }

    HolidayMap parsed;
    if (!ParseHolidayData(body, parsed)) {
        Wh_Log(L"Failed to parse the holiday data (%zu characters)", body.size());
        return false;
    }

    size_t entryCount = parsed.size();
    StoreHolidays(std::move(parsed));

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_haveData = true;
    }

    Wh_Log(L"Loaded %zu holiday dates from the data source", entryCount);
    return true;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    while (true) {
        HANDLE stopEvent = g_hStopEvent.load();
        HANDLE workEvent = g_hWorkEvent.load();
        if (!stopEvent || !workEvent) {
            break;
        }

        HANDLE events[2] = {stopEvent, workEvent};
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) {
            break;
        }
        if (result != WAIT_OBJECT_0 + 1) {
            break;
        }

        g_fetchPending = false;

        if (FetchHolidayData()) {
            RefreshCalendarOnUiThreads();
        }
    }

    return 0;
}

// Every step is checked here. A CreateEvent which fails would leave the worker
// thread with a null handle for that event (a thread which finds one exits at
// once), and a request handed to a thread which was never started would stay
// pending for good, so that every later refresh is dropped silently. On a failure
// the handles which were created are closed again and the mod simply runs without
// a worker thread: it keeps showing the data it already has, and RequestHolidayData
// asks for the thread again on every refresh, so a start which failed once is retried
// instead of lasting for the whole session.
void StartWorkerThread() {
    std::lock_guard<std::mutex> lock(g_workerThreadMutex);

    if (g_hWorkerThread.load() || g_shuttingDown) {
        return;
    }

    HANDLE workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!workEvent || !stopEvent) {
        Wh_Log(L"Failed to create the worker events (error %u)", GetLastError());
        if (workEvent) {
            CloseHandle(workEvent);
        }
        if (stopEvent) {
            CloseHandle(stopEvent);
        }
        return;
    }

    // The handles go in before the thread starts: the thread reads them, and a
    // thread which finds a null handle returns immediately.
    g_hWorkEvent.store(workEvent);
    g_hStopEvent.store(stopEvent);

    HANDLE thread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!thread) {
        Wh_Log(L"Failed to create the worker thread (error %u)", GetLastError());
        g_hWorkEvent.store(nullptr);
        g_hStopEvent.store(nullptr);
        CloseHandle(workEvent);
        CloseHandle(stopEvent);
        return;
    }

    g_hWorkerThread.store(thread);
}

// The handles are taken out of the globals and the lock is dropped before the wait.
// Waiting while holding g_workerThreadMutex is a deadlock: the worker thread may be
// inside RefreshCalendarOnUiThreads, which waits for a UI thread, and that UI thread
// may have passed the g_shuttingDown test of RequestHolidayData and be about to block
// on this mutex in StartWorkerThread. The window is narrow - the unload has to land
// between those two - but closing it costs nothing. Taking the handles first also
// keeps the worker from being handed a new request while it is being stopped.
void StopWorkerThread() {
    HANDLE workerThread;
    HANDLE workEvent;
    HANDLE stopEvent;

    {
        std::lock_guard<std::mutex> lock(g_workerThreadMutex);

        workerThread = g_hWorkerThread.exchange(nullptr);
        workEvent = g_hWorkEvent.exchange(nullptr);
        stopEvent = g_hStopEvent.exchange(nullptr);

        if (stopEvent) {
            SetEvent(stopEvent);
        }
    }

    // The worker is joined rather than detached. Wh_GetUrlContent cannot be cancelled,
    // so the unload waits for a request which is in flight (bounded by the engine's
    // own timeout), but WorkerThreadProc is mod code and must not be left running into
    // an image which is about to be unmapped.
    if (workerThread) {
        WaitForSingleObject(workerThread, INFINITE);
        CloseHandle(workerThread);
    }
    if (workEvent) {
        CloseHandle(workEvent);
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
    }
}

// The decision whether a fetch is due lives here: no repeated request inside the
// refresh interval, and a shorter 10 minute retry interval after a failure. A caller
// which has to re-read the data at once - a changed address, for example - does it by
// clearing the attempt tick through ResetHolidayData rather than by a flag here.
void RequestHolidayData() {
    const std::shared_ptr<const Settings> settings = GetSettings();
    if (g_shuttingDown) {
        return;
    }

    // No data source: nothing to fetch and no worker thread to create for it. This is
    // the default state of the mod, and it is what keeps the mod from doing anything
    // at all - no thread, no request - until an address is filled in.
    if (settings->dataSourceUrl.empty()) {
        return;
    }

    // The worker thread is started on the first request which has a data source; a
    // start which failed is retried here, on every request. This has to happen before
    // the "is the data still fresh" test below, because a request which could not be
    // answered must not consume the refresh interval. StartWorkerThread takes a lock,
    // so a request from another thread cannot create a second one.
    if (!g_hWorkerThread.load()) {
        StartWorkerThread();
    }

    const ULONGLONG refreshInterval =
        static_cast<ULONGLONG>(settings->refreshIntervalHours) * 60ULL * 60ULL * 1000ULL;

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        if (g_lastFetchAttemptTick) {
            ULONGLONG interval = g_haveData ? refreshInterval : kFetchRetryDelayMs;
            if (GetTickCount64() - g_lastFetchAttemptTick < interval) {
                return;
            }
        }
    }

    if (g_fetchPending.exchange(true)) {
        return;
    }

    // The request is only answered while the worker thread is there to take it:
    // an event which cannot be signalled (or a worker thread which was never
    // started) would otherwise leave the flag set, and every later refresh would
    // be dropped without a word.
    if (HANDLE workEvent = g_hWorkEvent.load()) {
        if (SetEvent(workEvent)) {
            return;
        }
        Wh_Log(L"Could not hand the refresh over to the worker thread (error %u)",
               GetLastError());
    } else {
        Wh_Log(L"There is no worker thread, so the holiday data cannot be fetched");
    }

    // Nothing is going to answer this request: taking the flag back keeps the next
    // refresh from being dropped, and the attempt is counted so that this path is
    // not taken again on every day cell the shell creates.
    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_lastFetchAttemptTick = GetTickCount64();
    }
    g_fetchPending = false;
}

void ResetHolidayData() {
    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_lastFetchAttemptTick = 0;
        g_haveData = false;
    }
    {
        std::lock_guard<std::mutex> lock(g_holidaysMutex);
        g_holidays.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Calendar UI

////////////////////////////////////////////////////////////////////////////////
// Looking inside a day cell

// The date a day cell stands for.
//
// CalendarViewDayItem.Date is a Windows.Foundation.DateTime: 100 ns ticks since 1601, with
// no zone attached, so the same value can be read in two ways, and the two differ by the
// local UTC offset:
//
//   * the ticks name local midnight of the date the cell was made for, which is what the
//     shell does when it fills a cell for a calendar date;
//   * the ticks name midnight UTC of that date.
//
// Which one Windows uses cannot be observed on this machine, and does not have to be: the
// choice below is right either way, in every time zone.
//
//   * On or east of UTC, the local reading is the cell's date under both readings of the
//     ticks. With local midnight it is the date itself, and with midnight UTC the local
//     time of 00:00Z is still the same date in the same zone (an hour or more of the same
//     day, never the day before).
//   * West of UTC, the UTC reading is the cell's date under both. With midnight UTC it is
//     the date itself, and with local midnight the two readings name the same date anyway:
//     local midnight lies behind UTC, but less than a day behind.
//
// So "west of UTC ? the UTC date : the local date" is right in all four combinations, and
// it is a property of the cell alone - nothing is remembered and nothing is guessed, which
// also means a day cell the shell is still filling in cannot be read against a day number
// left over from the date it had before.
//
// Which side of UTC the machine is on is taken from the two readings of this very value,
// not from the time zone of the process: the local wall clock is behind the UTC wall clock
// exactly when the offset is negative, and the difference carries the offset in force on
// that date, daylight saving included.
//
// A conversion which fails returns 0, and the caller skips that cell: writing nothing is
// better than writing the wrong date.
int32_t DateKeyFromDateTime(wf::DateTime const& dateTime) {
    ULARGE_INTEGER uli;
    uli.QuadPart = static_cast<ULONGLONG>(dateTime.time_since_epoch().count());

    FILETIME fileTime;
    fileTime.dwLowDateTime = uli.LowPart;
    fileTime.dwHighDateTime = uli.HighPart;

    SYSTEMTIME systemTimeUtc{};
    if (!FileTimeToSystemTime(&fileTime, &systemTimeUtc)) {
        return 0;
    }

    SYSTEMTIME systemTimeLocal{};
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &systemTimeUtc,
                                         &systemTimeLocal)) {
        return 0;
    }

    // Both wall-clock readings as ticks: the local one is behind the UTC one exactly when
    // this machine is west of UTC on that date.
    auto ticksOf = [](SYSTEMTIME const& systemTime, ULONGLONG& ticks) -> bool {
        FILETIME fileTimeOfSystemTime{};
        if (!SystemTimeToFileTime(&systemTime, &fileTimeOfSystemTime)) {
            return false;
        }
        ULARGE_INTEGER value;
        value.LowPart = fileTimeOfSystemTime.dwLowDateTime;
        value.HighPart = fileTimeOfSystemTime.dwHighDateTime;
        ticks = value.QuadPart;
        return true;
    };

    ULONGLONG localTicks = 0;
    ULONGLONG utcTicks = 0;
    if (!ticksOf(systemTimeLocal, localTicks) || !ticksOf(systemTimeUtc, utcTicks)) {
        return 0;
    }

    const SYSTEMTIME& date =
        localTicks < utcTicks ? systemTimeUtc : systemTimeLocal;
    return MakeDateKey(date.wYear, date.wMonth, date.wDay);
}

// The name the Windows 11 calendar gives the text block which holds the lunar
// date or the solar term.
// It is a strong hint rather than the only rule: another Windows build may name the
// block differently, which is why FindLabelTextBlock has fallbacks.
constexpr PCWSTR kLunarTextBlockName = L"LunarTextBlock";

struct FoundElement {
    wux::DependencyObject element{nullptr};
    int depth = 0;
};

// Walks the visual tree of a day cell. The limits (256 elements, 24 levels) are purely
// defensive and are never reached by a cell of three or four elements; what they guard
// against is an element being destroyed by the shell while the walk is in progress.
void CollectDescendants(wux::DependencyObject const& root,
                        std::vector<FoundElement>& out) {
    out.push_back({root, 0});

    for (size_t i = 0; i < out.size() && i < 256; i++) {
        wux::DependencyObject current = out[i].element;
        int depth = out[i].depth;
        if (!current || depth > 24) {
            continue;
        }

        int count = 0;
        try {
            count = wuxm::VisualTreeHelper::GetChildrenCount(current);
        } catch (...) {
            continue;
        }

        for (int childIndex = 0; childIndex < count; childIndex++) {
            wux::DependencyObject child = nullptr;
            try {
                child = wuxm::VisualTreeHelper::GetChild(current, childIndex);
            } catch (...) {
                continue;
            }
            if (!child) {
                continue;
            }
            out.push_back({child, depth + 1});
        }
    }
}

std::wstring ElementName(wux::DependencyObject const& element) {
    if (auto frameworkElement = element.try_as<wux::FrameworkElement>()) {
        try {
            return frameworkElement.Name().c_str();
        } catch (...) {
        }
    }
    return {};
}

std::wstring ElementText(wuxc::TextBlock const& textBlock) {
    try {
        return TrimWhitespace(textBlock.Text().c_str());
    } catch (...) {
        return {};
    }
}

// Prints what the day cell is made of, so that a cell which doesn't look the way
// the mod expects can be diagnosed from the log.
void LogDayItemStructure(wuxc::CalendarViewDayItem const& item) {
    std::vector<FoundElement> descendants;
    CollectDescendants(item, descendants);

    Wh_Log(L"Day cell structure (%zu elements):", descendants.size());
    for (const auto& found : descendants) {
        if (!found.element) {
            continue;
        }

        std::wstring line(found.depth * 2, L' ');
        line += winrt::get_class_name(found.element).c_str();

        if (std::wstring name = ElementName(found.element); !name.empty()) {
            line += L" #" + name;
        }

        if (auto textBlock = found.element.try_as<wuxc::TextBlock>()) {
            line += L" fontSize=" + std::to_wstring((int)textBlock.FontSize());
            if (std::wstring text = ElementText(textBlock); !text.empty()) {
                line += L" text=\"" + text + L"\"";
            }
        }

        if (auto frameworkElement = found.element.try_as<wux::FrameworkElement>()) {
            line += L" size=" + std::to_wstring((int)frameworkElement.ActualWidth()) +
                    L"x" + std::to_wstring((int)frameworkElement.ActualHeight());
        }

        Wh_Log(L"%s", line.c_str());
    }
}

// The day number is the only text in a day cell which is made of digits: the
// lunar date below it is "15/9", "4/9" or custom text. A text block which holds a
// number is therefore never the one the mod may write to.
bool IsNumberText(std::wstring const& text) {
    if (text.empty()) {
        return false;
    }
    for (wchar_t c : text) {
        if (c < L'0' || c > L'9') {
            return false;
        }
    }
    return true;
}

// Whether the shell has filled the day cell in at all. A day item which was just
// created has no content yet, and there is nothing to report about it.
bool DayItemHasContent(wuxc::CalendarViewDayItem const& item) {
    try {
        return wuxm::VisualTreeHelper::GetChildrenCount(item) > 0;
    } catch (...) {
    }
    return false;
}

// Finds the text block which holds the shell's lunar/solar-term text: the text
// block inside the day cell which is not the day number.
//
// This is done again on every pass rather than remembered: the shell takes the
// text blocks out of a day item and puts new ones in when it reuses the item for
// another month, and a text block which is no longer in the calendar takes an
// update to nowhere. A day cell has three or four children, so walking it is
// cheap.
wuxc::TextBlock FindLabelTextBlock(wuxc::CalendarViewDayItem const& item,
                                   void* preferredId) {
    std::vector<FoundElement> descendants;
    CollectDescendants(item, descendants);

    std::vector<wuxc::TextBlock> candidates;
    for (const auto& found : descendants) {
        auto textBlock = found.element.try_as<wuxc::TextBlock>();
        if (!textBlock || IsNumberText(ElementText(textBlock))) {
            continue;
        }
        candidates.push_back(textBlock);
    }

    if (candidates.empty()) {
        return nullptr;
    }

    // The text block the mod wrote to before, as long as the calendar still
    // shows it, so that what was written there is not written a second time.
    if (preferredId) {
        for (const auto& candidate : candidates) {
            if (winrt::get_abi(candidate) == preferredId) {
                return candidate;
            }
        }
    }

    // The name the Windows 11 calendar gives its lunar text block wins over the
    // heuristics below. Other builds may name it differently, and the heuristics
    // are what is used then.
    for (const auto& candidate : candidates) {
        if (ElementName(candidate) == kLunarTextBlockName) {
            return candidate;
        }
    }

    // The lunar text is the smaller of the texts: prefer a text block which
    // already has text, and among those the one with the smallest font.
    wuxc::TextBlock best = nullptr;
    double bestFontSize = 0;
    bool bestHasText = false;
    for (const auto& candidate : candidates) {
        bool hasText = !ElementText(candidate).empty();
        double fontSize = 0;
        try {
            fontSize = candidate.FontSize();
        } catch (...) {
        }

        if (!best || (hasText != bestHasText && hasText) ||
            (hasText == bestHasText && fontSize < bestFontSize)) {
            best = candidate;
            bestFontSize = fontSize;
            bestHasText = hasText;
        }
    }

    return best;
}

////////////////////////////////////////////////////////////////////////////////
// The state of the day cells

// The state of one day cell. The shell recycles day items and replaces the text
// blocks inside them, so the state is kept here and looked up by the day item's
// identity instead of by remembering the elements themselves.
struct DayCell {
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
    // The text block the mod writes to, and the callback which notices the shell
    // writing its own lunar text over the mod's holiday name.
    winrt::weak_ref<wuxc::TextBlock> label{nullptr};
    void* labelId = nullptr;
    int64_t labelChangedToken = 0;
    // The date this state was computed for. A day item which now shows another
    // date was filled in by the shell again, so its text belongs to the shell.
    int32_t key = 0;
    // The shell's own lunar/solar-term text, saved before the first change. Only
    // trusted while the same cell shows the same date.
    std::wstring savedText;
    bool hasSavedText = false;
    // The brush the label carried before the mod wrote its own colour, when it
    // carried one of its own, and whether it did at all. Held by a strong
    // reference: the shell drops its own reference to a brush once it is applied,
    // and a brush which is gone can't be put back.
    wuxm::Brush savedForeground{nullptr};
    bool hasSavedForeground = false;
    bool savedForegroundWasLocal = false;
    // The brush the mod itself wrote, if it wrote one. The colour on the label is
    // compared with this one to tell the mod's own colour from the colour of the
    // shell - two brushes which hold the same colour are still two brushes. The
    // reference stays alive, so the object it names can't be reused by another
    // brush and make a later comparison wrong.
    wuxm::Brush ownBrush{nullptr};
    // What the mod wrote into the cell, if anything.
    std::wstring appliedText;
    // Whether the mod's colour is on the label right now. This belongs to the
    // label element rather than to the date: an element the shell reuses for
    // another month still carries the mod's brush.
    bool hadColor = false;
    int64_t dateChangedToken = 0;
    bool busy = false;
};

// A day item of this thread, with the identity it is tracked by. The walk comes across
// the same day item on every pass - a month change puts the same ones back into the
// calendar - so an item is tracked here only once.
struct TrackedDayItem {
    void* id = nullptr;
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
};

// The most day items to keep an eye on. The walk finds every day item the calendar holds,
// and the calendar holds more than the six weeks a month view shows: the first limit this
// had (128, from when the calendar reported its cells one by one) was reached by a single
// pass of the walk on a live flyout. A full list makes a pass drop the cells it found
// first, so the limit has to leave room for the tree; what it is really for is the
// opposite case - a shell which keeps day items alive in a pool the calendar no longer
// shows. A day item which is gone is released and dropped on the next pass, and one which
// is shown again is found again by the walk, and tracked again.
constexpr size_t kMaxTrackedDayItems = 512;

// The state of every day cell of this thread, keyed by the day item's identity.
// Two references to the same XAML element compare unequal, and the element's Tag
// belongs to the shell, so the raw interface pointer is what identifies a cell.
// The cells are held behind a pointer: adding a cell while another one is being
// worked on must not move it.
thread_local std::unordered_map<void*, std::unique_ptr<DayCell>> t_cells;
thread_local std::vector<TrackedDayItem> t_dayItems;
// The day item which owns a text block, so that a text change can be traced back
// to the cell it belongs to. The parent chain is not usable for this: the text
// block of a day cell reports no parent at all.
thread_local std::unordered_map<void*, winrt::weak_ref<wuxc::CalendarViewDayItem>>
    t_labelOwners;
// A day cell the mod could not find the lunar text in is worth reporting once,
// with the structure of the cell, and not once per cell.
thread_local bool t_loggedMissingTextBlock = false;
// Hitting the limit of tracked day items says that the shell is holding on to
// day items which the calendar no longer shows, which is worth reporting once.
thread_local bool t_loggedTrackLimit = false;
// Set by whatever a pass over a cell actually writes - the text, the colour, or the
// colour being taken back. SweepDayItems() uses it to tell "the shell is still rewriting
// the cells" from "nothing has happened for a while", which is what stops the timer again.
thread_local bool t_applyWrote = false;

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item);

void* CellId(wuxc::CalendarViewDayItem const& item) {
    return winrt::get_abi(item);
}

DayCell& CellFor(wuxc::CalendarViewDayItem const& item) {
    std::unique_ptr<DayCell>& entry = t_cells[CellId(item)];
    if (entry && entry->item.get() != item) {
        // The day item this state was kept for is gone, and another one is
        // using its address: the state belongs to the old one.
        entry = nullptr;
    }
    if (!entry) {
        entry = std::make_unique<DayCell>();
        entry->item = winrt::make_weak(item);
    }
    return *entry;
}

// Remembers a day item the walk found. Finding a day item which is already tracked
// again only makes it the most recent one, so that the list holds the day items the
// calendar used last.
void TrackDayItem(wuxc::CalendarViewDayItem const& item) {
    void* id = winrt::get_abi(item);

    for (auto it = t_dayItems.begin(); it != t_dayItems.end(); ++it) {
        if (it->id != id) {
            continue;
        }
        if (it->item.get()) {
            TrackedDayItem tracked = *it;
            t_dayItems.erase(it);
            t_dayItems.push_back(tracked);
        } else {
            // The day item was released, and its address is in use again.
            it->item = winrt::make_weak(item);
        }
        return;
    }

    if (t_dayItems.size() >= kMaxTrackedDayItems) {
        if (!t_loggedTrackLimit) {
            t_loggedTrackLimit = true;
            Wh_Log(L"Tracking is at its limit of %zu day items",
                   kMaxTrackedDayItems);
        }
        t_dayItems.erase(t_dayItems.begin());
    }
    t_dayItems.push_back({id, winrt::make_weak(item)});
}

// Whether two brushes are the same object. This is what tells the mod's own colour
// apart from the colour of the shell: two brushes which hold the same colour are
// still two different brushes, and only the object shows which one it is.
bool SameBrush(wuxm::Brush const& left, wuxm::Brush const& right) {
    return left && right && winrt::get_abi(left) == winrt::get_abi(right);
}

// Whether the colour the label carries right now is the one the mod itself wrote.
// The effective value is compared rather than the local one: a local value which is
// not a brush (a binding, for example) resolves to the brush below and is never the
// mod's own, which is the answer this is used for.
bool IsOwnBrush(wuxc::TextBlock const& textBlock, wuxm::Brush const& ownBrush) {
    if (!ownBrush) {
        return false;
    }
    try {
        return SameBrush(textBlock.Foreground(), ownBrush);
    } catch (...) {
        return false;
    }
}

// The foreground is written only when it differs: a write which changes nothing
// still costs a layout pass, and it would count as a write for the sweep which is
// watching the shell rewrite the cells (see kSweepQuietTicksToStop).
// A new SolidColorBrush is created instead of the shell's brush being modified: the
// shell may share one brush between several elements of a template, and changing it
// would change them as well. The shell's own brush is kept as it is and put back on
// the day the mod stops marking the cell.
//
// The brush which is on the label afterwards is returned, so that the caller can
// tell the mod's own colour from the shell's later. A brush which was already
// there counts as the mod's own only when it is the very brush written before
// (previousOwn): claiming a brush of the shell's which happens to hold the same
// colour would clear the shell's own value on the day the mod stops marking.
wuxm::Brush SetForegroundIfDifferent(wuxc::TextBlock const& textBlock,
                                     winrt::Windows::UI::Color const& color,
                                     wuxm::Brush const& previousOwn) {
    try {
        if (auto current = textBlock.Foreground().try_as<wuxm::SolidColorBrush>()) {
            if (current.Color() == color) {
                return SameBrush(current, previousOwn) ? previousOwn : nullptr;
            }
        }
        wuxm::SolidColorBrush brush(color);
        textBlock.Foreground(brush);
        t_applyWrote = true;
        return brush;
    } catch (...) {
        return nullptr;
    }
}

// Remembers the colour the label carried before the mod wrote its own, once per
// label element. The value is read from the local-value store instead of by
// reading the property back: reading the property back gives the effective value,
// which is the mod's own brush once the shell has rewritten the text of a cell the
// mod has already coloured, and which would also be pinned as a local value of the
// label although it never was one.
//
// Why the local value is read: the shell's Foreground can be a local value written on
// the element, which is how the days of the neighbouring months are dimmed, or a style
// and inherited value, and telling those two apart is what keeps the restore from
// dropping the first one to the colour of its parent. A local value which is not a
// brush, a binding for instance, counts as no local value: the mod only has to take its
// own layer of colour off.
void SaveForeground(DayCell& cell, wuxc::TextBlock const& label) {
    if (cell.hasSavedForeground) {
        return;
    }
    try {
        // A brush of the mod's own is not an original colour: it is what an earlier
        // pass wrote, and saving it would make the mod put its own blue back on the
        // day the cell stops being a holiday. A recycled day item is what runs into
        // this, because the shell gives it its new date without touching a colour
        // the mod has already put on the label. Nothing is saved then, and a later
        // pass saves whatever the shell has set by the time it looks again; as long
        // as nothing was saved, the restore falls back to clearing the property.
        if (IsOwnBrush(label, cell.ownBrush)) {
            return;
        }
        wf::IInspectable value =
            label.ReadLocalValue(wuxc::TextBlock::ForegroundProperty());
        if (auto brush = value.try_as<wuxm::Brush>()) {
            cell.savedForeground = brush;
            cell.savedForegroundWasLocal = true;
        }
        cell.hasSavedForeground = true;
    } catch (...) {
        // Nothing was saved: the colour is then taken back by clearing the
        // property, which is what the cell falls back to anyway.
    }
}

// Puts the colour of the label back the way the shell had it. A label whose
// colour came from a local value gets that value back; a label which had none gets
// the property cleared, so that the style and the inheritance it used before apply
// again.
void RestoreForeground(wuxc::TextBlock const& label, DayCell const& cell) {
    try {
        // Only a colour the mod itself wrote is taken back. A value the shell has
        // set since - the dimmed colour of the days of the neighbouring months, for
        // example - is left alone: the mod's colour is already gone from the label
        // by then, and clearing the property would take the shell's own value away
        // with it.
        if (!IsOwnBrush(label, cell.ownBrush)) {
            return;
        }
        if (cell.savedForegroundWasLocal && cell.savedForeground) {
            label.Foreground(cell.savedForeground);
        } else {
            label.ClearValue(wuxc::TextBlock::ForegroundProperty());
        }
        t_applyWrote = true;
    } catch (...) {
    }
}

// The shell's lunar text of a cell which was reused for another month is written
// after the mod has put the holiday name there, and that would take the name away
// again. The text callback is what catches it: when the text is not the one the
// mod wrote, the cell is applied to once more.
// The callback only applies to the cell once more; the real decisions are all in
// ApplyToDayItem, which refuses to re-enter while it is running, so the write the mod
// does itself does not come back as another change.
void OnLabelTextChanged(wux::DependencyObject const& sender) {
    auto it = t_labelOwners.find(winrt::get_abi(sender));
    if (it == t_labelOwners.end()) {
        return;
    }
    if (auto item = it->second.get()) {
        ApplyToDayItem(item);
        // The shell has just written its own text over the mod's: the moment the sweep
        // exists for, so it gets the full quiet period again.
        StartSweepTimer();
    }
}

// Whether a text is one the mod could have written. A cell which the shell reused
// for another month still holds the mod's text of the old date, and that text is
// not worth remembering as the shell's own.
// appliedText cannot cover that window, because the shell recycles a cell by clearing
// what it holds while the mod's own holiday name is still on it; the name is then the
// only thing left to recognise. The price is that a lunar text which happens to equal a
// holiday name is not saved as the shell's own, which is harmless: the text the mod wants
// to write is the same one.
bool IsOwnText(std::wstring const& text, Settings const& settings) {
    if (text.empty()) {
        return false;
    }
    if (text.find(L'/') != std::wstring::npos) {
        return true;
    }
    return false;
}

std::wstring ExpandName(std::wstring const& text, std::wstring const& name) {
    if (text.empty()) {
        return {};
    }

    // Every "{name}" is replaced: a text which says it twice means it twice. The result
    // is built up instead of replacing in place so that a name which itself contains
    // "{name}" cannot send the loop around forever.
    std::wstring result;
    size_t pos = 0;
    for (;;) {
        size_t found = text.find(L"{name}", pos);
        if (found == std::wstring::npos) {
            result.append(text, pos, std::wstring::npos);
            break;
        }
        result.append(text, pos, found - pos);
        result += name;
        pos = found + 6;
    }
    return result;
}

// One day cell, in full. The holiday name and its colour go into the cell's lunar text
// block, which is all the shell's day cell has room for:
//   1) turn the cell's Date into a yyyymmdd key;
//   2) look the key up, which says whether the day is a day off and what the holiday
//      is called;
//   3) find the lunar text block, anew on every pass;
//   4) decide the text: keep it, use the holiday name, or use the fixed text from the
//      settings;
//   5) decide the colour: the day-off or the workday colour, and put the shell's own
//      colour back for a day which is not a holiday any more;
//   6) remember what was written, so that it can be restored later.
//
// Nothing is written while the cell already shows the wanted value: every write costs a
// layout pass, and a write which changed nothing would also keep the sweep running for
// another round (see kSweepQuietTicksToStop).
void ApplyToDayItem(wuxc::CalendarViewDayItem const& item) {
    const std::shared_ptr<const Settings> settings = GetSettings();
    if (!item) {
        return;
    }

    int32_t key = DateKeyFromDateTime(item.Date());
    if (!key) {
        return;
    }

    DayCell& cell = CellFor(item);
    if (cell.busy) {
        return;
    }
    cell.busy = true;
    struct BusyGuard {
        bool& busy;
        ~BusyGuard() { busy = false; }
    } guard{cell.busy};

    // A day item which now shows another date was filled in by the shell again,
    // so its text is the shell's own and there is nothing left to restore. The
    // colour state stays, because it belongs to the label element, which is still
    // the same one and still carries the mod's brush.
    //
    // The saved colour value is dropped, but the brush the mod wrote itself
    // (ownBrush) is kept: the shell re-templates a recycled cell for its new date
    // without touching a colour the mod has put there, so a value saved for the old
    // date may be stale, while ownBrush is what still tells the mod's own colour
    // from a colour the shell has set for the new date. That is what keeps
    // SaveForeground() from saving the mod's own blue as the original colour of the
    // new date, and RestoreForeground() from clearing a colour the shell has put
    // there in the meantime.
    if (cell.key != key) {
        cell.key = key;
        cell.savedText.clear();
        cell.appliedText.clear();
        cell.hasSavedText = false;
        cell.savedForeground = nullptr;
        cell.hasSavedForeground = false;
        cell.savedForegroundWasLocal = false;
    }

    HolidayInfo info;
    const bool hasInfo = LookupHoliday(key, info);

    // The shell recycles day items, and a recycled item keeps whatever the mod
    // put into it, so a new date is applied to as well.
    if (cell.dateChangedToken == 0) {
        try {
            cell.dateChangedToken = item.RegisterPropertyChangedCallback(
                wuxc::CalendarViewDayItem::DateProperty(),
                [](wux::DependencyObject const& sender,
                   wux::DependencyProperty const&) {
                    if (auto dayItem =
                            sender.try_as<wuxc::CalendarViewDayItem>()) {
                        ApplyToDayItem(dayItem);
                        StartSweepTimer();
                    }
                });
        } catch (...) {
            Wh_Log(L"Failed to register the date callback: %08X",
                   winrt::to_hresult());
        }
    }

    // The shell's lunar/solar-term text block.
    wuxc::TextBlock label = FindLabelTextBlock(item, cell.labelId);
    if (!label) {
        if (DayItemHasContent(item) && !t_loggedMissingTextBlock) {
            t_loggedMissingTextBlock = true;
            Wh_Log(L"No text block to replace in the day cell of %d - the cell has no "
                   L"second line, which is where the mod writes, so the additional "
                   L"calendar of the taskbar has to be switched on",
                   key);
            LogDayItemStructure(item);
        }
        return;
    }

    // A text block which wasn't seen before: register the callback which notices
    // the shell writing its own text, so that the holiday name survives it.
    if (cell.label.get() != label) {
        if (auto previous = cell.label.get()) {
            if (cell.labelChangedToken) {
                try {
                    previous.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::TextProperty(), cell.labelChangedToken);
                } catch (...) {
                }
            }
            t_labelOwners.erase(winrt::get_abi(previous));
        }
        cell.labelChangedToken = 0;

        cell.label = winrt::make_weak(label);
        cell.labelId = winrt::get_abi(label);
        cell.savedText.clear();
        cell.appliedText.clear();
        cell.hasSavedText = false;
        cell.savedForeground = nullptr;
        cell.hasSavedForeground = false;
        cell.savedForegroundWasLocal = false;
        // A text block the shell has replaced carries no colour of the mod's, so
        // nothing of the mod is left on it to recognise or to take back.
        cell.ownBrush = nullptr;
        cell.hadColor = false;
        t_labelOwners[cell.labelId] = winrt::make_weak(item);

        try {
            cell.labelChangedToken = label.RegisterPropertyChangedCallback(
                wuxc::TextBlock::TextProperty(),
                [](wux::DependencyObject const& sender,
                   wux::DependencyProperty const&) {
                    OnLabelTextChanged(sender);
                });
        } catch (...) {
            Wh_Log(L"Failed to register the text callback: %08X",
                   winrt::to_hresult());
        }

    }

    std::wstring desired;
    if (hasInfo && settings->replaceText) {
        desired = info.name;
    }

    std::wstring current = ElementText(label);

    if (!desired.empty() && current != desired) {
        if (current != cell.appliedText && !IsOwnText(current, *settings)) {
            // The shell's own text is being replaced: remember it.
            cell.savedText = current;
            cell.hasSavedText = true;
        }
        try {
            label.Text(desired);
            t_applyWrote = true;
            Wh_Log(L"Day %d: \"%s\" -> \"%s\"", key, current.c_str(),
                   desired.c_str());
            LogDebug(L"Day %d: '%s' -> '%s' (Special=%d)", key, current.c_str(), desired.c_str(), info.isSpecial ? 1 : 0);
        } catch (...) {
        }
    } else if (desired.empty() && !cell.appliedText.empty() &&
               current == cell.appliedText) {
        // The mod's text is not wanted any more: put the shell's own text back.
        try {
            if (cell.hasSavedText) {
                label.Text(cell.savedText);
                t_applyWrote = true;
            }
        } catch (...) {
        }
        cell.hasSavedText = false;
    }

    cell.appliedText = desired;

    // The colour is what marks the day, whichever text the cell ends up showing,
    // so it does not depend on the text having been replaced. The shell's own
    // colour is saved right before the first colour write: the colour is written
    // with "replace the lunar text" switched off as well, and reading it back
    // later would save the mod's own brush as the original one once the shell has
    // rewritten the text of a cell the mod has already coloured.
    if (hasInfo) {
        SaveForeground(cell, label);

        // Detect if this cell is today or selected (accent circle background)
        SYSTEMTIME stToday;
        GetLocalTime(&stToday);
        int32_t todayKey = MakeDateKey(stToday.wYear, stToday.wMonth, stToday.wDay);
        bool isAccentCircle = (key == todayKey);

        if (!isAccentCircle) {
            std::vector<FoundElement> descendants;
            CollectDescendants(item, descendants);
            for (const auto& found : descendants) {
                if (auto tb = found.element.try_as<wuxc::TextBlock>()) {
                    if (IsNumberText(ElementText(tb))) {
                        if (auto scb = tb.Foreground().try_as<wuxm::SolidColorBrush>()) {
                            auto c = scb.Color();
                            double luma = (0.299 * c.R + 0.587 * c.G + 0.114 * c.B) / 255.0;
                            if (luma < 0.35) {
                                isAccentCircle = true;
                            }
                        }
                        break;
                    }
                }
            }
        }

        winrt::Windows::UI::Color targetColor;
        if (isAccentCircle) {
            targetColor = info.isSpecial ? settings->todaySpecialDayColor : settings->todayDayColor;
        } else {
            targetColor = info.isSpecial ? settings->specialDayColor : settings->normalDayColor;
        }

        cell.ownBrush = SetForegroundIfDifferent(label, targetColor, cell.ownBrush);
        cell.hadColor = true;
    } else if (cell.hadColor) {
        RestoreForeground(label, cell);
        cell.hadColor = false;
    }

}

////////////////////////////////////////////////////////////////////////////////
// Keeping the calendar up to date

// The shell writes the lunar text of a day cell which it reused for another month
// asynchronously - after the mod has put the holiday name there - and it may
// replace the text block as well, in which case the mod's text and its text
// callback are gone. The cells are therefore looked at again a few times after
// everything which can start such a rewrite. Only a cell whose text the shell took
// back is written to, so a pass over an unchanged calendar costs a few comparisons.
thread_local winrt::Windows::System::DispatcherQueueTimer t_sweepTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    t_sweepTimerRevoker;

constexpr int kSweepIntervalMs = 1000;

// How many passes without a write end the sweep. One second per pass, so the calendar is
// watched for about five seconds after the last write. Why a limit at all: the shell keeps
// the day cells of the flyout alive after it is closed, so a timer which only stops when
// the tracked list becomes empty would wake the shell process once a second for the rest
// of the session. Why five seconds are enough: the rewrite the sweep exists for follows
// an event the mod already sees - a day item being put back into the calendar, the date
// of a cell changing, or the text of a cell changing - and each of those starts the timer
// again with the full period. Do not shorten the interval, which would touch the shell's
// elements more often, and do not drop the sweep, which is what covers a text block the
// shell replaced together with its callback.
constexpr int kSweepQuietTicksToStop = 5;

thread_local int t_sweepQuietTicks = 0;

void StopSweepTimer() {
    t_sweepQuietTicks = 0;

    if (t_sweepTimer) {
        try {
            t_sweepTimer.Stop();
        } catch (...) {
        }
    }
}

// Starts the periodic re-check again, if it isn't running already, and gives the quiet
// count a fresh start: this is called from everything which the shell's late rewrite
// follows (see kSweepQuietTicksToStop).
// The re-check is also what finds the day cells, so a caller which knows that the
// calendar can have changed - a window which appeared, an activation, a changed setting -
// starts it even when no day item is tracked yet, which is the state before a calendar
// was found for the first time. Nothing here reads the calendar: a caller which wants the
// marks right away rather than on the next pass calls SweepDayItems() as well.
void StartSweepTimer() {
    t_sweepQuietTicks = 0;

    try {
        if (!t_sweepTimer) {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
                Wh_Log(L"No dispatcher queue: the calendar cannot be watched");
                return;
            }

            t_sweepTimer = dispatcherQueue.CreateTimer();
            t_sweepTimer.Interval(std::chrono::milliseconds{kSweepIntervalMs});
            t_sweepTimerRevoker = t_sweepTimer.Tick(
                winrt::auto_revoke,
                [](winrt::Windows::System::DispatcherQueueTimer const&,
                   wf::IInspectable const&) { SweepDayItems(); });
        }

        if (!t_sweepTimer.IsRunning()) {
            t_sweepTimer.Start();
        }
    } catch (...) {
        Wh_Log(L"Failed to create the sweep timer: %08X", winrt::to_hresult());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Finding the calendar and its day cells

// The CoreWindow of this thread, with the two events which say that the calendar can
// have been opened. They are taken lazily and given back on the same thread: a handler
// is a delegate into this module's image, so one which is left behind makes the shell
// call into an unmapped module the next time the flyout is opened.
struct CoreWindowTriggers {
    wuc::CoreWindow coreWindow{nullptr};
    winrt::event_token activatedToken{};
    winrt::event_token visibilityChangedToken{};
};

thread_local CoreWindowTriggers t_windowTriggers;

// The calls above arrive in bursts - an activation, a change of visibility and the
// window being shown are one open - so they are debounced (see
// OnCalendarOpenedOnCurrentThread).
constexpr ULONGLONG kCalendarOpenedDebounceMs = 500;
thread_local ULONGLONG t_lastCalendarOpenedTick = 0;

void OnCalendarOpenedOnCurrentThread();

// Defined with the rest of the process plumbing.
void InitializeForCurrentThread();

// Takes the activation events of this thread's CoreWindow, once. A CoreWindow which is
// not there yet is neither an error nor the end of it: the ShellHost target has none at
// all, and when a CoreWindow is created the object is not necessarily there the moment
// its window is (the window hook runs before XAML gets that far), so this is asked for
// again on every pass of the sweep until it works.
void RegisterCalendarTriggersOnCurrentThread() {
    try {
        if (t_windowTriggers.coreWindow) {
            return;
        }

        auto coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) {
            return;
        }

        // The window is remembered before the events are registered, so that a
        // registration which throws halfway through still has the tokens it did get
        // revoked when the thread is uninitialized.
        t_windowTriggers.coreWindow = coreWindow;
        t_windowTriggers.activatedToken = coreWindow.Activated(
            [](wuc::CoreWindow const&, wuc::WindowActivatedEventArgs const& args) {
                if (args.WindowActivationState() !=
                    wuc::CoreWindowActivationState::Deactivated) {
                    OnCalendarOpenedOnCurrentThread();
                }
            });
        t_windowTriggers.visibilityChangedToken = coreWindow.VisibilityChanged(
            [](wuc::CoreWindow const&,
               wuc::VisibilityChangedEventArgs const& args) {
                if (args.Visible()) {
                    OnCalendarOpenedOnCurrentThread();
                }
            });

        Wh_Log(L"Watching the window of thread %u", GetCurrentThreadId());
    } catch (...) {
        Wh_Log(L"Failed to register the window events: %08X", winrt::to_hresult());
    }
}

// Gives the events back, on the thread which took them.
void UnregisterCalendarTriggersOnCurrentThread() {
    try {
        auto coreWindow = t_windowTriggers.coreWindow;
        if (coreWindow) {
            if (t_windowTriggers.activatedToken.value != 0) {
                coreWindow.Activated(t_windowTriggers.activatedToken);
            }
            if (t_windowTriggers.visibilityChangedToken.value != 0) {
                coreWindow.VisibilityChanged(
                    t_windowTriggers.visibilityChangedToken);
            }
        }
    } catch (...) {
    }

    t_windowTriggers = {};
}

// A window which was just shown does not have its calendar yet: the day cells come with the
// layout which follows. The root element is watched until they are found, which is what
// makes the marks appear together with the calendar rather than up to one pass of the sweep
// later - the walk of the sweep is the fallback for a window whose layout does not change
// again, and for a flyout which keeps the day cells it already has.
thread_local wux::FrameworkElement t_layoutUpdatedElement{nullptr};
thread_local winrt::event_token t_layoutUpdatedToken{};
thread_local ULONGLONG t_layoutUpdatedStartTick = 0;
thread_local ULONGLONG t_lastLayoutUpdatedPassTick = 0;

// How long a watch stays armed, and how often it is allowed to walk. The window is not
// watched forever: a calendar which is not found in this time is not there, and the walk
// is repeated by the sweep anyway.
constexpr ULONGLONG kLayoutUpdatedWatchMs = 5000;
constexpr ULONGLONG kLayoutUpdatedThrottleMs = 100;

void RevokeLayoutUpdatedWatch() {
    if (t_layoutUpdatedToken.value != 0) {
        auto element = t_layoutUpdatedElement;
        if (element) {
            try {
                element.LayoutUpdated(t_layoutUpdatedToken);
            } catch (...) {
            }
        }
    }

    t_layoutUpdatedToken = {};
    t_layoutUpdatedElement = nullptr;
}

void OnCalendarLayoutUpdatedOnCurrentThread() {
    const ULONGLONG now = GetTickCount64();
    if (now - t_layoutUpdatedStartTick > kLayoutUpdatedWatchMs) {
        RevokeLayoutUpdatedWatch();
        return;
    }

    // A layout pass of the shell arrives in bursts as well, and one walk per burst says
    // the same thing as one walk per pass.
    if (now - t_lastLayoutUpdatedPassTick < kLayoutUpdatedThrottleMs) {
        return;
    }
    t_lastLayoutUpdatedPassTick = now;

    SweepDayItems();

    // Found: the marks are on the calendar, and the sweep is what watches it from here.
    if (!t_dayItems.empty()) {
        RevokeLayoutUpdatedWatch();
    }
}

// Arms the watch above for the window of this thread. Called with every open, so that a
// flyout which was closed and shown again is watched again; an armed watch is left alone.
void WatchCalendarLayoutOnCurrentThread() {
    if (t_layoutUpdatedToken.value != 0) {
        return;
    }

    try {
        auto window = wux::Window::Current();
        if (!window) {
            return;
        }

        auto root = window.Content().try_as<wux::FrameworkElement>();
        if (!root) {
            return;
        }

        t_layoutUpdatedElement = root;
        t_layoutUpdatedStartTick = GetTickCount64();
        t_lastLayoutUpdatedPassTick = 0;
        t_layoutUpdatedToken = root.LayoutUpdated(
            [](wf::IInspectable const&, wf::IInspectable const&) {
                OnCalendarLayoutUpdatedOnCurrentThread();
            });
    } catch (...) {
        RevokeLayoutUpdatedWatch();
    }
}

// How far a walk over a window goes. The limits are wide for a tree the calendar is in -
// the notification list above it can be long - and what they guard against is the window
// which has no calendar at all (the hardware flyouts of the same process share this code)
// and a tree the shell is changing while it is being read. A walk which stops at these
// limits is a walk which found nothing, not a wrong answer, and the next pass tries again.
constexpr size_t kMaxScannedElements = 16384;
constexpr int kMaxScanDepth = 96;

// A thread without a XAML window is worth reporting once: "the mod does nothing" is
// otherwise hard to tell from "this month has no holidays".
thread_local bool t_loggedNoWindow = false;

void CollectDayItemsFromElement(wux::DependencyObject const& root,
                                size_t& budget,
                                int depth) {
    if (!root || budget == 0 || depth > kMaxScanDepth) {
        return;
    }
    budget--;

    // A day cell is neither looked into nor looked past: what is inside it is the day
    // number and the lunar text of that one day, and there is no calendar below it.
    if (auto dayItem = root.try_as<wuxc::CalendarViewDayItem>()) {
        TrackDayItem(dayItem);
        return;
    }

    if (auto tb = root.try_as<wuxc::TextBlock>()) {
        try {
            std::wstring elemName = tb.Name().c_str();
            std::wstring currText = ElementText(tb);
            bool isHeader = (elemName == L"SubtitleText" || elemName == L"LunarHeader" || elemName == L"LunarText");
            if (!isHeader && !currText.empty()) {
                for (wchar_t ch : currText) {
                    if (ch >= 0x4E00 && ch <= 0x9FFF) {
                        isHeader = true;
                        break;
                    }
                }
            }
            if (isHeader) {
                SYSTEMTIME st;
                GetLocalTime(&st);
                int32_t todayKey = MakeDateKey(st.wYear, st.wMonth, st.wDay);
                HolidayInfo hinfo;
                LookupHoliday(todayKey, hinfo);
                if (!hinfo.headerText.empty() && currText != hinfo.headerText) {
                    tb.Text(hinfo.headerText);
                    t_applyWrote = true;
                    LogDebug(L"Replaced header: '%s' -> '%s'", currText.c_str(), hinfo.headerText.c_str());
                }
                return;
            }
        } catch (...) {}
    }

    int count = 0;
    try {
        count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int childIndex = 0; childIndex < count; childIndex++) {
        wux::DependencyObject child = nullptr;
        try {
            child = wuxm::VisualTreeHelper::GetChild(root, childIndex);
        } catch (...) {
            continue;
        }

        CollectDayItemsFromElement(child, budget, depth + 1);
    }
}

// Finds the day cells of the calendar by walking the XAML tree of the window of this
// thread, and tracks them: the pass which follows applies the holiday data to them.
//
// This walk is what replaced the XAML diagnostics. The diagnostics reported every day
// cell as it appeared, which is precise, but it costs a diagnostics consumer for the
// whole session and that is a consumer no other mod of this process can have. A walk on
// every pass of the sweep costs a walk of the calendar tree for the few seconds the sweep
// runs after a calendar can have changed, and it does not miss a cell the shell recycles,
// because it does not remember cells: it looks at what is there.
//
// Runs on the UI thread of a calendar window.
void CollectDayItemsOnCurrentThread() {
    wux::Window window = nullptr;
    try {
        window = wux::Window::Current();
    } catch (...) {
        return;
    }

    if (!window) {
        if (!t_loggedNoWindow) {
            t_loggedNoWindow = true;
            Wh_Log(L"No XAML window on thread %u", GetCurrentThreadId());
        }
        return;
    }

    try {
        size_t budget = kMaxScannedElements;

        if (auto content = window.Content()) {
            CollectDayItemsFromElement(content, budget, 0);
        }

        // A flyout can be a popup rather than part of the window's content.
        for (auto popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
            if (budget == 0) {
                break;
            }
            if (auto child = popup.Child()) {
                CollectDayItemsFromElement(child, budget, 0);
            }
        }
    } catch (...) {
    }
}

// Everything which says that the calendar of this thread can have been opened or changed:
// a window which appeared, an activation, the window being shown, a changed setting. The
// cells are marked right away rather than on the next pass of the sweep, so that the
// flyout does not open with the marks missing for a second.
// Runs on the UI thread of a calendar window.
void OnCalendarOpenedOnCurrentThread() {
    // Every thread whose state this arms is initialized here, so that the unload takes that
    // state apart again: this path is reached from a window which was shown, and that is not
    // necessarily a window the CreateWindowInBand(Ex) hooks ever saw.
    InitializeForCurrentThread();

    const ULONGLONG now = GetTickCount64();
    if (now - t_lastCalendarOpenedTick < kCalendarOpenedDebounceMs) {
        return;
    }
    t_lastCalendarOpenedTick = now;

    RegisterCalendarTriggersOnCurrentThread();
    SweepDayItems();
    StartSweepTimer();
    WatchCalendarLayoutOnCurrentThread();
    RequestHolidayData();
}

// Applies the mod's data to every day item of this thread. Runs on every timer
// tick, and after the data or the settings changed.
// Again, only a cell which does not show what the mod wants is written to, so a pass
// over an unchanged calendar does nothing at all.
void SweepDayItems() {
    // Everything a pass writes sets this, so that a pass which only reads can be told
    // from one which changed something (see kSweepQuietTicksToStop).
    t_applyWrote = false;

    // A pass arms state on this thread - the window events, the tracked cells and their
    // callbacks, the timer which called it - so the thread is initialized first: the unload
    // only takes apart what it finds on a thread which is marked as initialized.
    InitializeForCurrentThread();

    // The day cells are looked for on every pass rather than remembered from a report, and
    // the window events are asked for again until the CoreWindow of this thread is there
    // to be watched.
    RegisterCalendarTriggersOnCurrentThread();
    CollectDayItemsOnCurrentThread();

    // The list is only indexed, never iterated over: applying to a day item can
    // put another one into it.
    for (size_t i = 0; i < t_dayItems.size();) {
        auto item = t_dayItems[i].item.get();
        if (!item) {
            t_dayItems.erase(t_dayItems.begin() + i);
            continue;
        }

        ApplyToDayItem(item);
        i++;
    }

    for (auto it = t_cells.begin(); it != t_cells.end();) {
        if (it->second->item.get()) {
            ++it;
        } else {
            it = t_cells.erase(it);
        }
    }

    for (auto it = t_labelOwners.begin(); it != t_labelOwners.end();) {
        if (it->second.get()) {
            ++it;
        } else {
            it = t_labelOwners.erase(it);
        }
    }

    // A pass which wrote nothing is one more reason to believe the shell is done with
    // rewriting the cells, and the timer stops once enough of them follow each other.
    // Without that the timer would run for the rest of the session, because the shell
    // keeps the day cells of a closed flyout alive.
    // A pass which found no day item at all counts as a quiet one instead of stopping the
    // timer on the spot: the calendar of a window which was just created is usually not
    // built yet when it is first asked, and the pass which finds it comes later.
    if (t_applyWrote) {
        t_sweepQuietTicks = 0;
    } else if (++t_sweepQuietTicks >= kSweepQuietTicksToStop) {
        StopSweepTimer();
    }
}

// Undoes everything the mod did to the day cells of this thread: unregisters the two
// property callbacks, writes the saved text and colour back, clears the state and stops
// the timer. Only cells which are still alive are handled; one the shell has destroyed
// is filled in by the shell itself. Nothing here changes the element tree, so the worst
// case is that the text and the colour are repainted a moment later.
void UninitializeDayItemsForCurrentThread() {
    for (auto& entry : t_cells) {
        DayCell& cell = *entry.second;
        auto item = cell.item.get();
        if (!item) {
            continue;
        }

        if (cell.dateChangedToken != 0) {
            try {
                item.UnregisterPropertyChangedCallback(
                    wuxc::CalendarViewDayItem::DateProperty(),
                    cell.dateChangedToken);
            } catch (...) {
            }
            cell.dateChangedToken = 0;
        }

        if (auto label = cell.label.get()) {
            if (cell.labelChangedToken) {
                try {
                    label.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::TextProperty(), cell.labelChangedToken);
                } catch (...) {
                }
                cell.labelChangedToken = 0;
            }
            try {
                // The text goes back only when the shell's own text was saved.
                // A cell whose text the mod wrote without saving it keeps the
                // holiday name until the shell repaints it, which it does when
                // the calendar is opened again.
                if (cell.hasSavedText) {
                    label.Text(cell.savedText);
                }
                if (cell.hadColor) {
                    RestoreForeground(label, cell);
                }
            } catch (...) {
            }
        }
    }

    // Swapped with an empty container instead of cleared: clear() keeps the buckets and
    // the capacity on a UI thread of the shell, which outlives the mod, so the memory of
    // a full calendar would stay allocated after every unload. (Assigning "{}" would do
    // the same, but for a map whose mapped type is a unique_ptr it picks the
    // initializer-list assignment, which does not compile.)
    decltype(t_cells)().swap(t_cells);
    decltype(t_dayItems)().swap(t_dayItems);
    decltype(t_labelOwners)().swap(t_labelOwners);
    t_sweepQuietTicks = 0;

    try {
        if (t_sweepTimer) {
            t_sweepTimer.Stop();
        }
    } catch (...) {
    }
    t_sweepTimerRevoker.revoke();
    t_sweepTimer = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Process plumbing

enum class Target {
    ShellExperienceHost,
    ShellHost,
};

Target g_target = Target::ShellExperienceHost;

// The message RunFromWindowThreadViaPostMessage posts. Registered by Wh_ModInit before any
// hook of this mod can post, and kept here rather than inside that function because the
// unload asks for the same message: a post which is still queued when the mod is unloaded
// has to be retrieved while this module is still mapped (see
// DrainPendingRunFromWindowThreadPostsOnCurrentThread). A read which saw the zero of a
// registration which never happened would skip that drain, so the variable is atomic: the
// hook procedures and the unload read it on other threads than the one which registers it.
std::atomic<UINT> g_runFromWindowThreadViaPostMessageMsg{0};

// Registers the message if it is not registered yet. Wh_ModInit calls this once, where there
// is a single thread and no hook of this mod can be running; the registration left here is
// the fallback for a load which never got that far.
UINT RunFromWindowThreadViaPostMessageMsg() {
    UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        message =
            RegisterWindowMessage(L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);
        g_runFromWindowThreadViaPostMessageMsg.store(message);
    }

    return message;
}

// The hooks of the posts which have not been retrieved yet. A post whose window is gone
// before its thread retrieves it is dropped with its hook still installed, and a hook whose
// procedure is in a module which is being unmapped takes the shell down with it, so
// Wh_ModUninit unhooks whatever is left.
//
// The parameter of a post which is unhooked that way is left to the process: whether the
// hook procedure is running at that moment cannot be told from here, and freeing a
// parameter which it is about to free itself would be a double free.
std::mutex g_pendingPostHooksMutex;
std::vector<HHOOK> g_pendingPostHooks;

void RememberPendingPostHook(HHOOK hook) {
    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    g_pendingPostHooks.push_back(hook);
}

// Called from the hook procedure, before the callback runs: the hook is the procedure's own
// from that point on, and an unload which runs at the same time must not unhook it as well.
void ForgetPendingPostHook(HHOOK hook) {
    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    for (auto it = g_pendingPostHooks.begin(); it != g_pendingPostHooks.end(); ++it) {
        if (*it == hook) {
            g_pendingPostHooks.erase(it);
            break;
        }
    }
}

void UnhookPendingPostHooks() {
    std::vector<HHOOK> hooks;
    {
        std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
        hooks.swap(g_pendingPostHooks);
    }

    for (HHOOK hook : hooks) {
        Wh_Log(L"Unhooking a post which was never retrieved: %08X",
               (DWORD)(ULONG_PTR)hook);
        UnhookWindowsHookEx(hook);
    }
}

// Retrieves the posts of RunFromWindowThreadViaPostMessage which are still queued for this
// thread, so that their callbacks run now rather than after this thread was torn down: a
// callback which ran then would arm the window events, a sweep timer, the layout watch and
// the day cells of a thread the unload has already taken apart, nothing would take them
// apart again, and the sweep timer it left behind would fire into the unmapped module.
// Retrieving the message runs the WH_GETMESSAGE hook of the post, and that hook procedure is
// what runs the callback, removes the hook and frees its parameter. The teardown which
// follows in UninitializeForCurrentThread then sees whatever the callback armed.
//
// PeekMessage dispatches the sent messages of this thread as well, which is harmless here:
// this runs on a thread which is alive, and the sent messages of this thread are the other
// RunFromWindowThread calls.
void DrainPendingRunFromWindowThreadPostsOnCurrentThread() {
    const UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        return;
    }

    MSG msg;
    while (PeekMessage(&msg, nullptr, message, message, PM_REMOVE)) {
    }
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }

    g_initializedForThread = true;

    // The events of this thread's window, taken here and asked for again on every pass of
    // the sweep: a CoreWindow is not necessarily there yet when its window is.
    RegisterCalendarTriggersOnCurrentThread();
}

void UninitializeForCurrentThread() {
    // Before the flag is looked at: a post which is still queued for this thread runs its
    // callback here, while this module is still mapped, and the state that callback arms is
    // then taken apart by the teardown below - which is why the callback initializes the
    // thread it runs on (see OnCalendarOpenedOnCurrentThread).
    DrainPendingRunFromWindowThreadPostsOnCurrentThread();

    if (!g_initializedForThread) {
        return;
    }

    // On this thread: the window events are delegates into this module, and the day cells
    // are XAML objects of the thread they were found on.
    UnregisterCalendarTriggersOnCurrentThread();
    RevokeLayoutUpdatedWatch();
    UninitializeDayItemsForCurrentThread();

    g_initializedForThread = false;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
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

bool RunFromWindowThreadViaPostMessage(HWND hWnd,
                                       RunFromWindowThreadProc_t proc,
                                       PVOID procParam) {
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };

    const UINT message = RunFromWindowThreadViaPostMessageMsg();

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = (MSG*)lParam;
                if (msg->message ==
                    g_runFromWindowThreadViaPostMessageMsg.load()) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)msg->lParam;
                    if (param) {
                        ForgetPendingPostHook(param->hook);
                        param->proc(param->procParam);
                        UnhookWindowsHookEx(param->hook);
                        delete param;
                        msg->lParam = 0;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{
        .proc = proc,
        .procParam = procParam,
        .hook = hook,
    };
    RememberPendingPostHook(hook);
    if (!PostMessage(hWnd, message, 0, (LPARAM)param)) {
        ForgetPendingPostHook(hook);
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }

    return true;
}

// Whether a window of this process is one the calendar is looked for in. The two targets
// name their window differently: ShellExperienceHost uses the CoreWindow of the flyout,
// ShellHost the ControlCenterWindow the newer builds moved it to.
bool IsCalendarWindowClass(PCWSTR className) {
    if (!className) {
        return false;
    }

    if (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
        _wcsicmp(className, L"ControlCenterWindow") == 0 ||
        wcsstr(className, L"CoreWindow") != nullptr ||
        wcsstr(className, L"ControlCenter") != nullptr) {
        return true;
    }

    return false;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    // Nothing is posted while the mod is being unloaded, for the same reason as in
    // OnWindowShown.
    if (g_shuttingDown) {
        return;
    }

    // The class name is only a string when the caller passed one: the same call also
    // accepts a numeric atom, which names no class to compare against.
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName || !IsCalendarWindowClass(lpClassName)) {
        return;
    }

    Wh_Log(L"Initializing - created calendar window: %08X via %S",
           (DWORD)(ULONG_PTR)hWnd, funcName);

    // Posted rather than sent, for the reason the ShellHost target always had: the window
    // exists before its XAML does, so this runs once the thread is free to run it. The
    // first pass of it usually finds nothing yet - the calendar is built when the flyout
    // is opened - which is what the sweep it arms is for.
    RunFromWindowThreadViaPostMessage(
        hWnd,
        [](PVOID) {
            InitializeForCurrentThread();
            OnCalendarOpenedOnCurrentThread();
        },
        nullptr);
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
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
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
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
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

// The window being shown is the moment the flyout is opened, and it is the signal both
// targets have in common: the newer builds moved the calendar into a window which is not
// a CoreWindow, so the activation events are not available there. A hide is ignored - the
// marks are not taken back when the flyout is closed, the shell repaints the cells itself.
void OnWindowShown(HWND hWnd) {
    // Nothing is posted while the mod is being unloaded: the post would have to be drained
    // or unhooked again, and the calendar is not marked after this point anyway.
    if (g_shuttingDown) {
        return;
    }

    DWORD dwProcessId = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return;
    }

    WCHAR szClassName[256];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
        !IsCalendarWindowClass(szClassName)) {
        return;
    }

    LogDebug(L"OnWindowShown: hWnd=%p, class=%s", hWnd, szClassName);

    // Posted, not sent: this runs inside window management, and blocking it on the UI
    // thread which is being told about its own window is a deadlock waiting to happen.
    RunFromWindowThreadViaPostMessage(
        hWnd, [](PVOID) { OnCalendarOpenedOnCurrentThread(); }, nullptr);
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

// The return value of ShowWindow says whether the window was visible before, not whether
// the call worked, so it is passed through and not tested.
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    const BOOL wasVisible = ShowWindow_Original(hWnd, nCmdShow);

    if (nCmdShow != SW_HIDE) {
        OnWindowShown(hWnd);
    }

    return wasVisible;
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    BOOL result = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    if ((uFlags & SWP_SHOWWINDOW) || !(uFlags & SWP_HIDEWINDOW)) {
        OnWindowShown(hWnd);
    }
    return result;
}

std::vector<HWND> GetCalendarWindows() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[256];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (IsCalendarWindowClass(szClassName)) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

void RefreshCalendarOnUiThreads() {
    for (HWND hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                SweepDayItems();
                // The shell rewrites the text of a cell after such a change as well, so
                // the watch is armed again rather than only used once.
                StartSweepTimer();
            },
            nullptr);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

// Installs one hook and says so when it does not work. A hook which silently does
// not get installed shows up much later, as "the mod does nothing", which is far
// harder to read than the line this writes. The name is a narrow string because
// GetProcAddress takes one, and %S prints it from this wide format.
//
// The hook, the original and the target all carry the same Prototype, so the compiler
// checks the signatures: the void* casts this had before let a hook whose signature had
// drifted from the function it replaces compile, and that mistake shows up as a crash
// inside the shell, not as an error here.
template <typename Prototype>
bool InstallHook(HMODULE module,
                 PCSTR functionName,
                 Prototype* hook,
                 Prototype** original) {
    if (!module) {
        Wh_Log(L"Initialization is incomplete: the module of %S is not loaded",
               functionName);
        return false;
    }

    Prototype* target =
        reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));
    if (!target) {
        Wh_Log(L"Initialization is incomplete: %S is not in its module",
               functionName);
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Initialization is incomplete: the hook of %S could not be set",
               functionName);
        return false;
    }

    return true;
}

// Why the hooks are set here: the window has to be hooked before it is created or shown,
// otherwise a window which existed before the mod was loaded, or one being made while it
// loads, is missed and never gets its holiday marks.
BOOL Wh_ModInit() {
    Wh_Log(L">");

    // A mod is loaded again after it was unloaded, so the flag which stops a refresh
    // request from starting a worker thread during an unload is cleared here.
    g_shuttingDown = false;

    g_target = Target::ShellExperienceHost;

    WCHAR moduleFilePath[MAX_PATH];
    switch (GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    LoadSettings();
    // No worker thread is started here: the thread is created by the first refresh
    // request which has a data source to fetch, so a mod which is left without a data
    // source never creates one (see RequestHolidayData).

    // Registered here rather than on the first post: this runs on one thread, before any
    // hook of this mod can post, so the message every other thread reads is written once
    // and does not change afterwards (see RunFromWindowThreadViaPostMessageMsg).
    RunFromWindowThreadViaPostMessageMsg();

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    InstallHook(user32Module, "CreateWindowInBand", CreateWindowInBand_Hook,
                &CreateWindowInBand_Original);
    InstallHook(user32Module, "CreateWindowInBandEx",
                CreateWindowInBandEx_Hook, &CreateWindowInBandEx_Original);
    // The flyout being shown is what says that the calendar is about to be looked at
    // again: the days of a month are drawn when it is opened, and the day cells of the
    // last time can be gone by then.
    InstallHook(user32Module, "ShowWindow", ShowWindow_Hook, &ShowWindow_Original);
    InstallHook(user32Module, "SetWindowPos", SetWindowPos_Hook, &SetWindowPos_Original);

    return TRUE;
}

// Initialised after the load: a window which is already there gets its initialization and
// its first pass here, so that a mod which is loaded after the window was created is not
// missed, and the data is fetched up front so that the calendar has content the first time
// it is opened.
void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Pre-calculate today's lunar date and save to lunar_today.txt immediately
    SYSTEMTIME st;
    GetLocalTime(&st);
    int32_t todayKey = MakeDateKey(st.wYear, st.wMonth, st.wDay);
    HolidayInfo todayInfo = CalculateVietnameseLunar(todayKey);
    if (!todayInfo.name.empty()) {
        int lDay = 0, lMonth = 0, lYear = 0, lLeap = 0;
        VietnameseLunar::convertSolar2Lunar(st.wDay, st.wMonth, st.wYear, lDay, lMonth, lYear, lLeap);
        std::wstring canChi = VietnameseLunar::getCanChiYear(lYear);
        wchar_t buf[256];
        swprintf_s(buf, L"%02d/%02d %s", lDay, lMonth, canChi.c_str());
        FILE* fToday = nullptr;
        if (_wfopen_s(&fToday, L"C:\\ProgramData\\Windhawk\\lunar_today.txt", L"w, ccs=UTF-8") == 0 && fToday) {
            fputws(buf, fToday);
            fclose(fToday);
            LogDebug(L"Wh_ModAfterInit: Updated lunar_today.txt -> %s", buf);
        }
    }

    for (auto hCoreWnd : GetCalendarWindows()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                InitializeForCurrentThread();
                OnCalendarOpenedOnCurrentThread();
            },
            nullptr);
    }

    // Fetch the data up front so that the calendar is populated the first time it is
    // opened. With no data source set - the default - this does nothing at all.
    RequestHolidayData();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Before anything else: a refresh request which arrives while the mod is being
    // unloaded must not start a worker thread again.
    g_shuttingDown = true;

    StopWorkerThread();

    // One thread at a time, and on the thread which took the window events: the day cells
    // are XAML objects of the thread they were found on, and the window events are
    // delegates into this module, which is about to be unmapped.
    for (auto hCoreWnd : GetCalendarWindows()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }

    // A post whose window is gone before its thread retrieved it is left with its hook
    // installed, and the procedure of that hook is in this module.
    UnhookPendingPostHooks();
}

// Changing the settings: only a changed data source clears the data which was fetched,
// because the other settings (colours, texts, whether to replace) are only a different
// way of drawing the same data, and the cells can be repainted from it. Dropping the
// data instead would make the calendar fall back to the lunar text until the next fetch.
// Data which is still inside the refresh interval is not fetched again either, so
// changing a colour does not cost a request.
// Emptying the address is a change like any other: it clears the data as well, so the
// marks are taken off the calendar and nothing is fetched again.
void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    const std::wstring previousUrl = GetSettings()->dataSourceUrl;
    LoadSettings();
    const std::shared_ptr<const Settings> settings = GetSettings();

    // The holidays of one data source are not the holidays of another, but the
    // data of the same source is kept: dropping it would make the calendar fall
    // back to the lunar text until the next fetch is done.
    if (settings->dataSourceUrl != previousUrl) {
        ResetHolidayData();
    }

    // Re-apply the new settings to the day items which are already on screen.
    RefreshCalendarOnUiThreads();

    // Data which is still fresh is not fetched again, so that changing a colour
    // doesn't cost a request.
    RequestHolidayData();
}
