// ==WindhawkMod==
// @id              lunar-calendar-native-vietnam
// @name            Vietnamese Lunar & Holiday Calendar
// @name:vi-VN      Lịch Âm & Ngày Lễ Việt Nam
// @description     Displays Vietnamese lunar dates directly in the Windows 11 calendar flyout, with holiday, weekend, and custom event highlighting.
// @description:vi-VN Hiển thị lịch âm trực tiếp trên ô lịch Windows 11, bôi đỏ ngày lễ, cuối tuần, mùng 1, rằm và sự kiện quan trọng.
// @version         1.0.2
// @author          Mhale
// @github          https://github.com/PhTrien
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vietnamese Lunar & Holiday Calendar / Lịch Âm & Ngày Lễ Việt Nam

![Screenshot](https://raw.githubusercontent.com/PhTrien/Native-Lunar-Calendar-Vietnam/main/img/Screenshot.png)

## English

This mod integrates the Vietnamese lunar calendar directly into the Windows 11 taskbar calendar flyout.

### Features
* **Preserves the native Windows 11 design language:** Integrates cleanly into the system calendar flyout without external floating overlays or heavy UI modifications.
* **Full compatibility:** Works smoothly alongside **Windows 11 Notification Center Styler**, **Start Menu Styler**, and **Taskbar Styler**.
* **Accurate Hồ Ngọc Đức lunar algorithm (UTC+7):** Accurately computes new moon days, solar terms, leap months, and lunar new year transitions.
* **Holiday & festival names:** Shows short names of traditional Vietnamese holidays and international observances (Tết, Giỗ Tổ, 30/4, 2/9, Christmas, etc.).
* **Custom events:** Add personal anniversaries, birthdays, and reminders (by solar or lunar date) directly in Settings.
* **Weekend & special day highlighting:** Highlights Saturdays, Sundays, New Moon (1st), and Full Moon (15th) days.
* **Modern today indicator:** Optional subtle translucent rounded square indicator for the current date.

### Requirements & Setup
For the most seamless experience, turn on secondary calendars in Windows:
* Go to **Settings > Time & language > Date & time**.
* Under **Show additional calendars in the taskbar**, select **Simplified Chinese (Lunar)** or **Traditional Chinese (Lunar)**.
* Windows will provide the secondary calendar label in each day cell, which this mod automatically replaces with the Vietnamese lunar date, festivals, and events!

---

## Tiếng Việt

Bản mod tích hợp lịch âm Việt Nam trực tiếp vào bảng lịch hệ thống Windows 11.

### Tính năng nổi bật
* **Hòa nhập với phong cách Windows 11:** Tích hợp liền mạch vào giao diện lịch Windows 11 gốc, không dùng cửa sổ nổi hay can thiệp nặng nề. Các tùy chọn làm nổi bật như viền bo góc ngày hiện tại hay bôi đỏ thứ cuối tuần có thể tùy chỉnh hoặc tắt trong Cài đặt.
* **Tương thích hoàn toàn:** Hoạt động mượt mà cùng lúc với **Windows 11 Notification Center Styler**, **Start Menu Styler** và **Taskbar Styler**.
* **Thuật toán âm lịch Hồ Ngọc Đức chuẩn xác (UTC+7):** Tính đúng ngày Sóc, điểm Hoàng kinh mặt trời, tháng nhuận và chuyển năm âm lịch.
* **Bôi đỏ & Tên ngày Lễ:** Hiển thị tên rút gọn các dịp lễ Tết truyền thống Việt Nam và ngày lễ quốc tế.
* **Sự kiện quan trọng cá nhân:** Cho phép tạo danh sách ngày kỷ niệm, sinh nhật theo cả Dương lịch lẫn Âm lịch ngay trong Settings!
* **Bôi đỏ ngày Cuối tuần, Mùng 1 và Rằm (15 âm):** Giúp theo dõi ngày sóc vọng thuận tiện.

### Yêu cầu & Hướng dẫn cài đặt
Để mod hoạt động tối ưu và ổn định nhất, khuyến khích bật lịch phụ trong Windows:
* Mở **Settings > Time & language > Date & time**.
* Tại mục **Show additional calendars in the taskbar**, chọn **Simplified Chinese (Lunar)** hoặc **Traditional Chinese (Lunar)**.
* Windows sẽ tạo sẵn nhãn lịch phụ trong ô ngày, mod sẽ tự động thay thế phần chữ này bằng ngày âm lịch Việt Nam và tên ngày lễ!

---

## Credits / Lời cảm ơn

This mod is based on the calendar plumbing and UI hooks from:
* [Chinese Holiday Calendar](https://github.com/ramensoftware/windhawk-mods/tree/main/mods/chinese-holiday-calendar) by m417z (GPL-3.0-only)
* [Agenda in Calendar View](https://github.com/ramensoftware/windhawk-mods/tree/main/mods/agenda-in-calendar-view) by lonfro (GPL-3.0-only)
* [Windows 11 Notification Center Styler](https://github.com/ramensoftware/windhawk-mods/tree/main/mods/windows-11-notification-center-styler) by m417z (GPL-3.0-only)
* Vietnamese lunar calendar calculation algorithm by [Hồ Ngọc Đức](https://www.informatik.uni-leipzig.de/~duc/amlich/).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showLunarDate: true
  $name: "Show Vietnamese lunar calendar"
  $description: "Display Vietnamese lunar date under each solar day number."
  $name:vi-VN: "Hiển thị âm lịch Việt Nam"
  $description:vi-VN: "Hiển thị ngày âm lịch bên dưới mỗi số ngày dương lịch."
- useTransparentRoundedToday: true
  $name: "Current day: Modern rounded transparent square"
  $description: "Replace the solid accent circle with a subtle transparent rounded square."
  $name:vi-VN: "Ngày hiện tại: Hình vuông bo góc trong suốt"
  $description:vi-VN: "Thay thế hình tròn xanh đặc bằng hình vuông bo góc trong suốt hiện đại."
- showHolidayName: true
  $name: "Show holiday name instead of lunar date"
  $description: "Display the holiday or festival name instead of the numeric lunar day."
  $name:vi-VN: "Hiển thị tên ngày lễ thay vì số ngày âm"
  $description:vi-VN: "Hiển thị tên ngày lễ (ví dụ Phụ nữ VN, Tết Tây,...) để tránh nhầm số 20/10 dương với âm."
- highlightHolidays: true
  $name: "Highlight holidays and festivals"
  $description: "Highlight traditional Vietnamese holidays and major international days in red."
  $name:vi-VN: "Bôi đỏ ngày Lễ Tết"
  $description:vi-VN: "Tô màu đỏ và hiển thị tên các ngày lễ truyền thống (Tết, Giỗ Tổ, 30/4, 2/9, Giáng sinh,...)."
- highlightLunarSpecial: false
  $name: "Highlight 1st and 15th lunar days"
  $description: "Highlight the new moon (1st) and full moon (15th) of each lunar month."
  $name:vi-VN: "Bôi đỏ ngày Mùng 1 và Rằm (15 âm lịch)"
  $description:vi-VN: "Làm nổi bật ngày Mùng 1 và ngày 15 âm lịch hàng tháng."
- highlightWeekendHeader: true
  $name: "Highlight weekend header (Sat, Sun)"
  $description: "Highlight Saturday and Sunday columns in the weekday header."
  $name:vi-VN: "Bôi đỏ thứ cuối tuần (Thứ 7, CN)"
  $description:vi-VN: "Bôi đỏ chữ B và C trên thanh thứ để dễ dàng nhận biết cột cuối tuần."
- highlightWeekends: false
  $name: "Highlight weekend day numbers"
  $description: "Highlight Saturday and Sunday day numbers in the calendar grid (disabled by default)."
  $name:vi-VN: "Bôi đỏ ngày cuối tuần trong lịch"
  $description:vi-VN: "Làm nổi bật các con số ngày Thứ Bảy và Chủ Nhật trong bảng ngày (mặc định tắt)."
- holidayColorHex: "#FF4D4F"
  $name: "Highlight color (HEX)"
  $description: "Color code used to highlight holidays, weekends, special lunar days, and events."
  $name:vi-VN: "Màu sắc bôi đỏ (Mã HEX)"
  $description:vi-VN: "Mã màu dùng để làm nổi bật ngày lễ, cuối tuần, mùng 1, rằm và sự kiện (mặc định #FF4D4F)."
- lunarFontSize: 12
  $name: "Lunar date font size"
  $description: "Font size of the lunar date text below the day number (default 12, 7-14)."
  $name:vi-VN: "Cỡ chữ ngày âm lịch"
  $description:vi-VN: "Cỡ chữ hiển thị ngày âm bên dưới số ngày dương (mặc định 12, từ 7 đến 14)."
- customEvents:
  - - date: "14/03"
      $name: "Date (DD/MM or DD/MM/YYYY)"
      $name:vi-VN: "Ngày (DD/MM hoặc DD/MM/YYYY)"
    - name: "Pi Day"
      $name: "Event name"
      $name:vi-VN: "Tên sự kiện"
    - isLunar: false
      $name: "Lunar calendar"
      $name:vi-VN: "Tính theo Âm lịch"
  $name: "Important custom events"
  $description: "Add anniversaries, birthdays, or special events (e.g., 14/03 or 15/08/2026)."
  $name:vi-VN: "Danh sách sự kiện quan trọng"
  $description:vi-VN: "Thêm các ngày kỷ niệm, sinh nhật (Ví dụ: 14/03 hoặc 15/08/2026)."
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <Unknwn.h>
#include <winrt/base.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <windhawk_utils.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuv = winrt::Windows::UI::ViewManagement;

thread_local bool g_initializedForThread = false;

////////////////////////////////////////////////////////////////////////////////
// Settings & Custom Events

struct CustomEvent {
    int day = 0;
    int month = 0;
    int year = 0; // 0 = lặp lại hàng năm
    bool isLunar = false;
    std::wstring name;
};

struct Settings {
    bool showLunarDate = true;
    bool useTransparentRoundedToday = true;
    bool showHolidayName = true;
    bool highlightWeekendHeader = true;
    bool highlightWeekends = false;
    bool highlightLunarSpecial = false;
    bool highlightHolidays = true;
    int lunarFontSize = 12;
    winrt::Windows::UI::Color holidayColor{255, 255, 77, 79};  // #FF4D4F
    std::vector<CustomEvent> customEvents;
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings = std::make_shared<Settings>();

std::shared_ptr<const Settings> GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring ReadStringSetting(PCWSTR name) {
    const WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(name);
    return std::wstring(value.get());
}

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
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };

    uint8_t bytes[4] = {0xFF, 0x00, 0x00, 0x00};
    size_t offset = (value.size() == 8) ? 0 : 1;
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

void ParseCustomEventDate(std::wstring const& dateStr, int& outDay, int& outMonth, int& outYear) {
    outDay = 0;
    outMonth = 0;
    outYear = 0;
    int parts[3] = {0, 0, 0};
    int count = 0;
    int cur = 0;
    bool hasDigits = false;

    for (wchar_t c : dateStr) {
        if (c >= L'0' && c <= L'9') {
            cur = cur * 10 + (c - L'0');
            hasDigits = true;
        } else if (c == L'/' || c == L'-' || c == L'.') {
            if (hasDigits && count < 3) {
                parts[count++] = cur;
                cur = 0;
                hasDigits = false;
            }
        }
    }
    if (hasDigits && count < 3) {
        parts[count++] = cur;
    }

    if (count == 2) {
        outDay = parts[0];
        outMonth = parts[1];
        outYear = 0;
    } else if (count == 3) {
        outDay = parts[0];
        outMonth = parts[1];
        outYear = parts[2];
    }
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();

    settings->showLunarDate = Wh_GetIntSetting(L"showLunarDate") != 0;
    settings->useTransparentRoundedToday = Wh_GetIntSetting(L"useTransparentRoundedToday") != 0;
    settings->showHolidayName = Wh_GetIntSetting(L"showHolidayName") != 0;
    settings->highlightHolidays = Wh_GetIntSetting(L"highlightHolidays") != 0;
    settings->highlightLunarSpecial = Wh_GetIntSetting(L"highlightLunarSpecial") != 0;
    settings->highlightWeekendHeader = Wh_GetIntSetting(L"highlightWeekendHeader") != 0;
    settings->highlightWeekends = Wh_GetIntSetting(L"highlightWeekends") != 0;

    settings->lunarFontSize = Wh_GetIntSetting(L"lunarFontSize");
    if (settings->lunarFontSize < 7 || settings->lunarFontSize > 14) {
        settings->lunarFontSize = 12;
    }

    if (auto c = ParseHexColor(ReadStringSetting(L"holidayColorHex"))) {
        settings->holidayColor = *c;
    }

    // Đọc danh sách sự kiện tùy biến
    for (int i = 0;; i++) {
        auto date = WindhawkUtils::StringSetting::make(
            L"customEvents[%d].date", i);
        if (!*date.get()) {
            break;
        }

        auto name = WindhawkUtils::StringSetting::make(
            L"customEvents[%d].name", i);
        int isLunar = Wh_GetIntSetting(L"customEvents[%d].isLunar", i);

        CustomEvent evt;
        evt.isLunar = (isLunar != 0);
        evt.name = *name.get() ? name.get() : L"Event";
        ParseCustomEventDate(date.get(), evt.day, evt.month, evt.year);

        if (evt.day >= 1 && evt.day <= 31 && evt.month >= 1 && evt.month <= 12) {
            settings->customEvents.push_back(std::move(evt));
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = settings;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Helpers

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

int32_t MakeDateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

////////////////////////////////////////////////////////////////////////////////
// Thuật toán Âm lịch Hồ Ngọc Đức (UTC+7)

constexpr double kPi = 3.14159265358979323846;
constexpr double kVietnamTimeZone = 7.0;

int IntFloor(double value) {
    return static_cast<int>(std::floor(value));
}

int JulianDayFromDate(int day, int month, int year) {
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    if (jd < 2299161) {
        jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - 32083;
    }
    return jd;
}

int NewMoonDay(int k, double timeZone) {
    double t = k / 1236.85;
    double t2 = t * t;
    double t3 = t2 * t;
    double dr = kPi / 180.0;
    double jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * t2 - 0.000000155 * t3;
    jd1 += 0.00033 * std::sin((166.56 + 132.87 * t - 0.009173 * t2) * dr);
    double m = 359.2242 + 29.10535608 * k - 0.0000333 * t2 - 0.00000347 * t3;
    double mPrime = 306.0253 + 385.81691806 * k + 0.0107306 * t2 + 0.00001236 * t3;
    double f = 21.2964 + 390.67050646 * k - 0.0016528 * t2 - 0.00000239 * t3;
    double c1 = (0.1734 - 0.000393 * t) * std::sin(m * dr) +
                0.0021 * std::sin(2 * dr * m) - 0.4068 * std::sin(mPrime * dr) +
                0.0161 * std::sin(2 * dr * mPrime) - 0.0004 * std::sin(3 * dr * mPrime) +
                0.0104 * std::sin(2 * dr * f) - 0.0051 * std::sin(dr * (m + mPrime)) -
                0.0074 * std::sin(dr * (m - mPrime)) + 0.0004 * std::sin(dr * (2 * f + m)) -
                0.0004 * std::sin(dr * (2 * f - m)) - 0.0006 * std::sin(dr * (2 * f + mPrime)) +
                0.0010 * std::sin(dr * (2 * f - mPrime)) + 0.0005 * std::sin(dr * (2 * mPrime + m));
    double deltaT = t < -11
        ? 0.001 + 0.000839 * t + 0.0002261 * t2 - 0.00000845 * t3 - 0.000000081 * t * t3
        : -0.000278 + 0.000265 * t + 0.000262 * t2;
    return IntFloor(jd1 + c1 - deltaT + 0.5 + timeZone / 24.0);
}

int SunLongitudeSector(int dayNumber, double timeZone) {
    double t = (dayNumber - 2451545.5 - timeZone / 24.0) / 36525.0;
    double t2 = t * t;
    double dr = kPi / 180.0;
    double m = 357.52910 + 35999.05030 * t - 0.0001559 * t2 - 0.00000048 * t * t2;
    double l0 = 280.46645 + 36000.76983 * t + 0.0003032 * t2;
    double delta = (1.914600 - 0.004817 * t - 0.000014 * t2) * std::sin(dr * m);
    delta += (0.019993 - 0.000101 * t) * std::sin(2 * dr * m) + 0.000290 * std::sin(3 * dr * m);
    double longitude = (l0 + delta) * dr;
    longitude -= kPi * 2 * IntFloor(longitude / (kPi * 2));
    return IntFloor(longitude / kPi * 6);
}

int LunarMonth11(int year, double timeZone) {
    int off = JulianDayFromDate(31, 12, year) - 2415021;
    int k = IntFloor(off / 29.530588853);
    int nm = NewMoonDay(k, timeZone);
    if (SunLongitudeSector(nm, timeZone) >= 9) {
        nm = NewMoonDay(k - 1, timeZone);
    }
    return nm;
}

int LeapMonthOffset(int a11, double timeZone) {
    int k = IntFloor(0.5 + (a11 - 2415021.076998695) / 29.530588853);
    int last = 0;
    int i = 1;
    int arc = SunLongitudeSector(NewMoonDay(k + i, timeZone), timeZone);
    do {
        last = arc;
        ++i;
        arc = SunLongitudeSector(NewMoonDay(k + i, timeZone), timeZone);
    } while (arc != last && i < 14);
    return i - 1;
}

struct VietnameseLunarDateInfo {
    int day = 0;
    int month = 0;
    int year = 0;
    bool leapMonth = false;
};

bool VietnameseLunarDate(int dayNumber, int solarYear,
                         VietnameseLunarDateInfo& lunar) {
    int k = IntFloor((dayNumber - 2415021.076998695) / 29.530588853);
    int monthStart = NewMoonDay(k + 1, kVietnamTimeZone);
    if (monthStart > dayNumber) {
        monthStart = NewMoonDay(k, kVietnamTimeZone);
    }
    int a11 = LunarMonth11(solarYear, kVietnamTimeZone);
    int b11 = a11;
    const bool beforeLunarMonth11 = a11 >= monthStart;
    if (beforeLunarMonth11) {
        a11 = LunarMonth11(solarYear - 1, kVietnamTimeZone);
    } else {
        b11 = LunarMonth11(solarYear + 1, kVietnamTimeZone);
    }
    lunar.day = dayNumber - monthStart + 1;
    int diff = IntFloor((monthStart - a11) / 29.0);
    lunar.month = diff + 11;
    lunar.year = beforeLunarMonth11 ? solarYear : solarYear + 1;
    if (b11 - a11 > 365) {
        int leapOff = LeapMonthOffset(a11, kVietnamTimeZone);
        if (diff >= leapOff) {
            lunar.month = diff + 10;
        }
        if (diff == leapOff) {
            lunar.leapMonth = true;
        }
    }
    if (lunar.month > 12) {
        lunar.month -= 12;
    }
    if (lunar.month >= 11 && diff < 4) {
        lunar.year--;
    }
    return lunar.day >= 1 && lunar.day <= 30 && lunar.month >= 1 &&
           lunar.month <= 12 && lunar.year >= 1;
}

struct VietnameseHoliday {
    PCWSTR shortLabel;
    PCWSTR fullLabel;
};

std::optional<VietnameseHoliday> GetVietnameseHoliday(
    int dayNumber, int solarDay, int solarMonth, int solarYear,
    VietnameseLunarDateInfo const& lunar) {
    if (!lunar.leapMonth) {
        if (lunar.month == 1 && lunar.day == 1) return VietnameseHoliday{L"Tết", L"Mùng 1 Tết Nguyên Đán"};
        if (lunar.month == 1 && lunar.day == 2) return VietnameseHoliday{L"Mùng 2", L"Mùng 2 Tết Nguyên Đán"};
        if (lunar.month == 1 && lunar.day == 3) return VietnameseHoliday{L"Mùng 3", L"Mùng 3 Tết Nguyên Đán"};
        if (lunar.month == 1 && lunar.day == 4) return VietnameseHoliday{L"Mùng 4", L"Mùng 4 Tết Nguyên Đán"};
        if (lunar.month == 1 && lunar.day == 15) return VietnameseHoliday{L"Rằm T1", L"Tết Nguyên Tiêu (Rằm tháng Giêng)"};
        if (lunar.month == 3 && lunar.day == 3) return VietnameseHoliday{L"Hàn thực", L"Tết Hàn Thực (3/3 ÂL)"};
        if (lunar.month == 3 && lunar.day == 10) return VietnameseHoliday{L"Giỗ Tổ", L"Giỗ Tổ Hùng Vương (10/3 ÂL)"};
        if (lunar.month == 4 && lunar.day == 15) return VietnameseHoliday{L"Phật Đản", L"Lễ Phật Đản (15/4 ÂL)"};
        if (lunar.month == 5 && lunar.day == 5) return VietnameseHoliday{L"Đoan Ngọ", L"Tết Đoan Ngọ (5/5 ÂL)"};
        if (lunar.month == 7 && lunar.day == 15) return VietnameseHoliday{L"Vu Lan", L"Lễ Vu Lan (Rằm tháng 7)"};
        if (lunar.month == 8 && lunar.day == 15) return VietnameseHoliday{L"Trung thu", L"Tết Trung Thu (Rằm tháng 8)"};
        if (lunar.month == 12 && lunar.day == 23) return VietnameseHoliday{L"Ông Táo", L"Tiễn Ông Công Ông Táo (23/12 ÂL)"};
        if (lunar.month == 12 && (lunar.day == 29 || lunar.day == 30)) {
            // Chỉ hiển thị "Tất niên" vào đúng ngày cuối cùng của năm âm lịch (ngày 29 hoặc 30 tùy tháng thiếu hay đủ)
            VietnameseLunarDateInfo nextLunar;
            if (VietnameseLunarDate(dayNumber + 1, solarYear, nextLunar)) {
                if (nextLunar.month == 1 && nextLunar.day == 1) {
                    return VietnameseHoliday{L"Tất niên", L"Tất Niên"};
                }
            }
        }
    }

    // Ngày lễ Dương lịch - Dùng tên chữ rõ ràng, tránh nhầm thành ngày âm lịch
    if (solarMonth == 1 && solarDay == 1) return VietnameseHoliday{L"Tết Tây", L"Tết Dương Lịch"};
    if (solarMonth == 2 && solarDay == 14) return VietnameseHoliday{L"Valentine", L"Lễ Tình Nhân"};
    if (solarMonth == 3 && solarDay == 8) return VietnameseHoliday{L"Quốc tế PN", L"Quốc tế Phụ nữ"};
    if (solarMonth == 4 && solarDay == 30) return VietnameseHoliday{L"Giải Phóng", L"Ngày Giải phóng miền Nam 30/4"};
    if (solarMonth == 5 && solarDay == 1) return VietnameseHoliday{L"Quốc tế LĐ", L"Quốc tế Lao động 1/5"};
    if (solarMonth == 9 && solarDay == 2) return VietnameseHoliday{L"Quốc Khánh", L"Quốc khánh 2/9"};
    if (solarMonth == 10 && solarDay == 20) return VietnameseHoliday{L"Phụ nữ VN", L"Ngày Phụ nữ Việt Nam 20/10"};
    if (solarMonth == 11 && solarDay == 20) return VietnameseHoliday{L"Nhà giáo VN", L"Ngày Nhà giáo Việt Nam 20/11"};
    if (solarMonth == 12 && solarDay == 24) return VietnameseHoliday{L"Đêm Noel", L"Đêm Giáng Sinh"};
    if (solarMonth == 12 && solarDay == 25) return VietnameseHoliday{L"Giáng Sinh", L"Lễ Giáng Sinh"};

    return std::nullopt;
}

struct DayDisplayInfo {
    std::wstring label;
    bool shouldHighlight = false;
};

DayDisplayInfo GetDayDisplayInfo(int32_t dateKey, Settings const& settings) {
    DayDisplayInfo out;

    int year = dateKey / 10000;
    int month = (dateKey / 100) % 100;
    int day = dateKey % 100;
    int dayNumber = JulianDayFromDate(day, month, year);
    VietnameseLunarDateInfo lunar;
    if (!VietnameseLunarDate(dayNumber, year, lunar)) {
        return out;
    }

    int dayOfWeek = (dayNumber + 1) % 7; // 0 = Chủ Nhật, 6 = Thứ Bảy
    bool isWeekend = (dayOfWeek == 0 || dayOfWeek == 6);
    bool isLunarSpecial = (lunar.day == 1 || lunar.day == 15);

    // Mặc định: ngày 1 âm hoặc mùng 1 dương thì hiện Ngày/Tháng (thêm N nếu nhuận)
    if (settings.showLunarDate) {
        if (lunar.day == 1 || day == 1) {
            out.label = std::to_wstring(lunar.day) + L"/" + std::to_wstring(lunar.month);
            if (lunar.leapMonth) {
                out.label += L"N";
            }
        } else {
            out.label = std::to_wstring(lunar.day);
        }
    }

    // 1. Kiểm tra ngày lễ truyền thống & quốc tế
    if (settings.highlightHolidays) {
        if (auto holiday = GetVietnameseHoliday(dayNumber, day, month, year, lunar)) {
            out.shouldHighlight = true;
            if (settings.showHolidayName && holiday->shortLabel && holiday->shortLabel[0] != L'\0') {
                out.label = holiday->shortLabel;
            }
        }
    }

    // 2. Kiểm tra sự kiện quan trọng cá nhân (Sự kiện người dùng có độ ưu tiên cao nhất, đè lên ngày lễ)
    for (const auto& evt : settings.customEvents) {
        bool match = false;
        if (evt.isLunar) {
            if (!lunar.leapMonth && evt.day == lunar.day && evt.month == lunar.month &&
                (evt.year == 0 || evt.year == lunar.year)) {
                match = true;
            }
        } else {
            if (evt.day == day && evt.month == month &&
                (evt.year == 0 || evt.year == year)) {
                match = true;
            }
        }
        if (match) {
            out.shouldHighlight = true;
            if (!evt.name.empty()) {
                out.label = evt.name;
            }
            break;
        }
    }

    // 3. Kiểm tra cuối tuần (Thứ 7, CN)
    if (settings.highlightWeekends && isWeekend) {
        out.shouldHighlight = true;
    }

    // 4. Kiểm tra Mùng 1 và Rằm (15 âm)
    if (settings.highlightLunarSpecial && isLunarSpecial) {
        out.shouldHighlight = true;
    }

    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Visual Tree Helpers & Day Cell Styling

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

constexpr PCWSTR kLunarTextBlockName = L"WindhawkLunarText";

struct FoundElement {
    wux::DependencyObject element{nullptr};
    int depth = 0;
};

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

// Tìm chính xác cặp TextBlock (số ngày dương và chữ âm lịch) trong ô ngày,
// đồng thời dọn dẹp triệt để bất kỳ TextBlock chèn trùng lặp (kLunarTextBlockName) nào.
void FindDayCellTextBlocks(wuxc::CalendarViewDayItem const& item,
                           int solarDay,
                           wuxc::TextBlock& outDayNum,
                           wuxc::TextBlock& outLabel) {
    outDayNum = nullptr;
    outLabel = nullptr;

    std::vector<FoundElement> descendants;
    CollectDescendants(item, descendants);

    wuxc::TextBlock dayNumTb = nullptr;
    wuxc::TextBlock nativeLunarTb = nullptr;
    std::vector<wuxc::TextBlock> insertedLunarTbs;
    std::vector<wuxc::TextBlock> otherTbs;

    for (const auto& found : descendants) {
        if (auto tb = found.element.try_as<wuxc::TextBlock>()) {
            std::wstring name = ElementName(tb);
            if (name == L"DayNumberTextBlock") {
                dayNumTb = tb;
            } else if (name == L"LunarTextBlock") {
                nativeLunarTb = tb;
            } else if (name == kLunarTextBlockName) {
                insertedLunarTbs.push_back(tb);
            } else {
                otherTbs.push_back(tb);
            }
        }
    }

    // 1. Nếu ô đã có LunarTextBlock chính thức của hệ thống Windows:
    // Xóa ngay lập tức bất kỳ TextBlock chèn trùng (kLunarTextBlockName) nào để tránh lồng số!
    if (nativeLunarTb) {
        for (auto& inserted : insertedLunarTbs) {
            try {
                if (auto parent = wuxm::VisualTreeHelper::GetParent(inserted)) {
                    if (auto panel = parent.try_as<wuxc::Panel>()) {
                        uint32_t idx = 0;
                        if (panel.Children().IndexOf(inserted, idx)) {
                            panel.Children().RemoveAt(idx);
                        }
                    }
                }
            } catch (...) {}
        }
        insertedLunarTbs.clear();
        outLabel = nativeLunarTb;
    } else if (!insertedLunarTbs.empty()) {
        // Nếu không có native LunarTextBlock nhưng có kLunarTextBlockName:
        // Giữ lại đúng 1 cái duy nhất, xóa các cái thừa
        outLabel = insertedLunarTbs[0];
        try {
            outLabel.Visibility(wux::Visibility::Visible);
        } catch (...) {}

        for (size_t i = 1; i < insertedLunarTbs.size(); i++) {
            try {
                if (auto parent = wuxm::VisualTreeHelper::GetParent(insertedLunarTbs[i])) {
                    if (auto panel = parent.try_as<wuxc::Panel>()) {
                        uint32_t idx = 0;
                        if (panel.Children().IndexOf(insertedLunarTbs[i], idx)) {
                            panel.Children().RemoveAt(idx);
                        }
                    }
                }
            } catch (...) {}
        }
    }

    // 2. Xác định dayNumTb nếu chưa tìm thấy theo tên DayNumberTextBlock
    if (!dayNumTb) {
        std::wstring expectedDayStr = std::to_wstring(solarDay);
        for (auto it = otherTbs.begin(); it != otherTbs.end(); ++it) {
            if (ElementText(*it) == expectedDayStr) {
                dayNumTb = *it;
                otherTbs.erase(it);
                break;
            }
        }
    }
    if (!dayNumTb && !otherTbs.empty()) {
        size_t bestIdx = 0;
        double maxFontSize = 0;
        for (size_t i = 0; i < otherTbs.size(); i++) {
            double fs = 0;
            try { fs = otherTbs[i].FontSize(); } catch (...) {}
            if (fs > maxFontSize) {
                maxFontSize = fs;
                bestIdx = i;
            }
        }
        dayNumTb = otherTbs[bestIdx];
        otherTbs.erase(otherTbs.begin() + bestIdx);
    }

    // 3. Nếu vẫn chưa có outLabel nhưng còn TextBlock phụ khác trong cell:
    if (!outLabel && !otherTbs.empty()) {
        outLabel = otherTbs[0];
    }

    // 4. Fallback: chỉ khi ô hoàn toàn chỉ có 1 TextBlock duy nhất (số ngày dương)
    // thì mới tạo mới TextBlock kLunarTextBlockName bên trong Panel con của ô
    if (!outLabel && dayNumTb) {
        wuxc::Panel hostPanel = nullptr;
        try {
            auto parent = wuxm::VisualTreeHelper::GetParent(dayNumTb);
            while (parent && parent != item) {
                if (auto p = parent.try_as<wuxc::Panel>()) {
                    hostPanel = p;
                    break;
                }
                parent = wuxm::VisualTreeHelper::GetParent(parent);
            }
        } catch (...) {}

        if (!hostPanel) {
            for (const auto& found : descendants) {
                if (found.element != item) {
                    if (auto p = found.element.try_as<wuxc::Panel>()) {
                        hostPanel = p;
                        break;
                    }
                }
            }
        }

        if (hostPanel) {
            try {
                auto newTb = wuxc::TextBlock();
                newTb.Name(kLunarTextBlockName);
                newTb.IsHitTestVisible(false);
                newTb.HorizontalAlignment(wux::HorizontalAlignment::Center);
                newTb.VerticalAlignment(wux::VerticalAlignment::Bottom);
                newTb.TextAlignment(wux::TextAlignment::Center);
                newTb.Margin(wux::Thickness{0, 0, 0, 2});

                try {
                    int row = wuxc::Grid::GetRow(dayNumTb);
                    int col = wuxc::Grid::GetColumn(dayNumTb);
                    wuxc::Grid::SetRow(newTb, row);
                    wuxc::Grid::SetColumn(newTb, col);
                } catch (...) {}

                try {
                    if (auto fg = dayNumTb.Foreground()) {
                        newTb.Foreground(fg);
                    }
                } catch (...) {}

                hostPanel.Children().Append(newTb);
                outLabel = newTb;
            } catch (...) {}
        }
    }

    outDayNum = dayNumTb;
}

struct ColorLocker {
    winrt::weak_ref<wuxc::TextBlock> textBlock{nullptr};
    int64_t token = 0;
    winrt::Windows::UI::Color targetColor{};
    wuxm::Brush originalBrush{nullptr};
    bool hasOriginalBrush = false;
    bool active = false;
};
thread_local std::unordered_map<void*, ColorLocker> t_colorLockers;

bool IsCustomHighlightColor(winrt::Windows::UI::Color const& c) {
    auto settings = GetSettings();
    if (c == settings->holidayColor) {
        return true;
    }
    if (c.R > 220 && c.G < 100 && c.B < 100) {
        return true;
    }
    return false;
}

void LockTextBlockColorStable(wuxc::TextBlock const& tb, winrt::Windows::UI::Color const& color) {
    if (!tb) {
        return;
    }
    try {
        void* id = winrt::get_abi(tb);
        winrt::Windows::UI::Color targetCol{color.A, color.R, color.G, color.B};

        auto& locker = t_colorLockers[id];
        locker.textBlock = winrt::make_weak(tb);

        if (!locker.hasOriginalBrush) {
            auto currentBrush = tb.Foreground();
            if (auto scb = currentBrush.try_as<wuxm::SolidColorBrush>()) {
                if (!IsCustomHighlightColor(scb.Color())) {
                    locker.originalBrush = currentBrush;
                    locker.hasOriginalBrush = true;
                }
            } else if (currentBrush) {
                locker.originalBrush = currentBrush;
                locker.hasOriginalBrush = true;
            }
        }

        locker.targetColor = targetCol;

        auto curBrush = tb.Foreground().try_as<wuxm::SolidColorBrush>();
        if (!curBrush || curBrush.Color() != targetCol) {
            tb.Foreground(wuxm::SolidColorBrush(targetCol));
        }

        if (!locker.active) {
            locker.active = true;
            locker.token = tb.RegisterPropertyChangedCallback(
                wuxc::TextBlock::ForegroundProperty(),
                [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                    if (auto t = sender.try_as<wuxc::TextBlock>()) {
                        void* sid = winrt::get_abi(t);
                        auto it = t_colorLockers.find(sid);
                        if (it != t_colorLockers.end()) {
                            if (it->second.active) {
                                auto cb = t.Foreground().try_as<wuxm::SolidColorBrush>();
                                if (!cb || cb.Color() != it->second.targetColor) {
                                    t.Foreground(wuxm::SolidColorBrush(it->second.targetColor));
                                }
                            } else {
                                auto cb = t.Foreground().try_as<wuxm::SolidColorBrush>();
                                if (cb && !IsCustomHighlightColor(cb.Color())) {
                                    it->second.originalBrush = cb;
                                    it->second.hasOriginalBrush = true;
                                }
                            }
                        }
                    }
                });
        }
    } catch (...) {
    }
}

void UnlockTextBlockColor(wuxc::TextBlock const& tb) {
    if (!tb) {
        return;
    }
    try {
        void* id = winrt::get_abi(tb);
        auto it = t_colorLockers.find(id);
        if (it != t_colorLockers.end()) {
            if (it->second.active) {
                it->second.active = false;
                if (it->second.token != 0) {
                    try {
                        tb.UnregisterPropertyChangedCallback(
                            wuxc::TextBlock::ForegroundProperty(), it->second.token);
                    } catch (...) {
                    }
                    it->second.token = 0;
                }
            }
        }

        auto curBrush = tb.Foreground().try_as<wuxm::SolidColorBrush>();
        if (curBrush && IsCustomHighlightColor(curBrush.Color())) {
            tb.ClearValue(wuxc::TextBlock::ForegroundProperty());

            auto afterClear = tb.Foreground().try_as<wuxm::SolidColorBrush>();
            if (!afterClear || IsCustomHighlightColor(afterClear.Color())) {
                if (it != t_colorLockers.end() && it->second.hasOriginalBrush && it->second.originalBrush) {
                    tb.Foreground(it->second.originalBrush);
                } else {
                    winrt::Windows::UI::Color fgColor{255, 255, 255, 255};
                    try {
                        fgColor = wuv::UISettings().GetColorValue(wuv::UIColorType::Foreground);
                    } catch (...) {}
                    tb.Foreground(wuxm::SolidColorBrush(fgColor));
                }
            }
        }
    } catch (...) {
    }
}

////////////////////////////////////////////////////////////////////////////////
// Quản lý trạng thái từng ô ngày (CalendarViewDayItem)

struct DayCell {
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
    winrt::weak_ref<wuxc::TextBlock> dayNumberLabel{nullptr};

    winrt::weak_ref<wuxc::TextBlock> label{nullptr};
    void* labelId = nullptr;
    int64_t labelChangedToken = 0;
    double savedFontSize = 0;
    bool hasSavedFontSize = false;

    int32_t key = 0;
    int32_t lastStyledKey = 0;
    bool lastIsToday = false;

    std::wstring savedText;
    bool hasSavedText = false;
    std::wstring appliedText;
    int64_t dateChangedToken = 0;
    bool busy = false;
};

struct TrackedDayItem {
    void* id = nullptr;
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
};

constexpr size_t kMaxTrackedDayItems = 512;

thread_local std::unordered_map<void*, std::unique_ptr<DayCell>> t_cells;
thread_local std::vector<TrackedDayItem> t_dayItems;
thread_local std::unordered_map<void*, winrt::weak_ref<wuxc::CalendarViewDayItem>> t_labelOwners;
thread_local winrt::weak_ref<wuxc::CalendarView> t_calendarView{nullptr};

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item);
void StartSweepTimer();

void* CellId(wuxc::CalendarViewDayItem const& item) {
    return winrt::get_abi(item);
}

DayCell& CellFor(wuxc::CalendarViewDayItem const& item) {
    std::unique_ptr<DayCell>& entry = t_cells[CellId(item)];
    if (entry && entry->item.get() != item) {
        entry = nullptr;
    }
    if (!entry) {
        entry = std::make_unique<DayCell>();
        entry->item = winrt::make_weak(item);
    }
    return *entry;
}

void TrackDayItem(wuxc::CalendarViewDayItem const& item) {
    void* id = winrt::get_abi(item);

    for (auto it = t_dayItems.begin(); it != t_dayItems.end(); ++it) {
        if (it->id != id) {
            continue;
        }
        if (!it->item.get()) {
            it->item = winrt::make_weak(item);
        }
        return;
    }

    if (t_dayItems.size() >= kMaxTrackedDayItems) {
        t_dayItems.erase(t_dayItems.begin());
    }
    t_dayItems.push_back({id, winrt::make_weak(item)});
}

void OnLabelTextChanged(wux::DependencyObject const& sender) {
    auto it = t_labelOwners.find(winrt::get_abi(sender));
    if (it == t_labelOwners.end()) {
        return;
    }
    if (auto item = it->second.get()) {
        StartSweepTimer();
        ApplyToDayItem(item);
    }
}

// Bộ đệm cọ vẽ (SolidColorBrush) dùng chung để tránh cấp phát mới liên tục
struct CachedBrushes {
    winrt::Windows::UI::Color accentColor{};
    winrt::Windows::UI::Color foregroundColor{};
    wuxm::SolidColorBrush todayBgBrush{nullptr};
    wuxm::SolidColorBrush todayHoverBgBrush{nullptr};
    wuxm::SolidColorBrush todayPressedBgBrush{nullptr};
    wuxm::SolidColorBrush todayBorderBrush{nullptr};
    wuxm::SolidColorBrush foregroundBrush{nullptr};
    bool valid = false;
};
thread_local CachedBrushes t_cachedBrushes;

void UpdateCachedBrushesIfNeeded() {
    winrt::Windows::UI::Color accentColor{255, 0, 120, 215};
    winrt::Windows::UI::Color fgColor{255, 255, 255, 255};
    try {
        auto uiSettings = wuv::UISettings();
        accentColor = uiSettings.GetColorValue(wuv::UIColorType::Accent);
        fgColor = uiSettings.GetColorValue(wuv::UIColorType::Foreground);
    } catch (...) {}

    if (!t_cachedBrushes.valid ||
        t_cachedBrushes.accentColor != accentColor ||
        t_cachedBrushes.foregroundColor != fgColor) {
        t_cachedBrushes.accentColor = accentColor;
        t_cachedBrushes.foregroundColor = fgColor;

        winrt::Windows::UI::Color todayBgColor{65, accentColor.R, accentColor.G, accentColor.B};
        winrt::Windows::UI::Color todayHoverBgColor{105, accentColor.R, accentColor.G, accentColor.B};
        winrt::Windows::UI::Color todayPressedBgColor{145, accentColor.R, accentColor.G, accentColor.B};
        winrt::Windows::UI::Color todayBorderColor{180, accentColor.R, accentColor.G, accentColor.B};

        t_cachedBrushes.todayBgBrush = wuxm::SolidColorBrush(todayBgColor);
        t_cachedBrushes.todayHoverBgBrush = wuxm::SolidColorBrush(todayHoverBgColor);
        t_cachedBrushes.todayPressedBgBrush = wuxm::SolidColorBrush(todayPressedBgColor);
        t_cachedBrushes.todayBorderBrush = wuxm::SolidColorBrush(todayBorderColor);
        t_cachedBrushes.foregroundBrush = wuxm::SolidColorBrush(fgColor);
        t_cachedBrushes.valid = true;
    }
}

thread_local bool t_applyWrote = false;

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

    if (cell.key != key) {
        if (auto prevDayNum = cell.dayNumberLabel.get()) {
            UnlockTextBlockColor(prevDayNum);
        }
        if (auto prevLabel = cell.label.get()) {
            UnlockTextBlockColor(prevLabel);
        }
        cell.key = key;
        cell.lastStyledKey = 0;
        cell.savedText.clear();
        cell.appliedText.clear();
        cell.hasSavedText = false;
        cell.dayNumberLabel = nullptr;
        cell.label = nullptr;
    }

    if (cell.dateChangedToken == 0) {
        try {
            cell.dateChangedToken = item.RegisterPropertyChangedCallback(
                wuxc::CalendarViewDayItem::DateProperty(),
                [](wux::DependencyObject const& sender,
                   wux::DependencyProperty const&) {
                    if (auto dayItem = sender.try_as<wuxc::CalendarViewDayItem>()) {
                        StartSweepTimer();
                        ApplyToDayItem(dayItem);
                    }
                });
        } catch (...) {
        }
    }

    SYSTEMTIME now{};
    GetLocalTime(&now);
    const bool isToday = (key == MakeDateKey(now.wYear, now.wMonth, now.wDay));

    const DayDisplayInfo displayInfo = GetDayDisplayInfo(key, *settings);
    const bool shouldColorRed = displayInfo.shouldHighlight && !isToday;

    auto cachedDayNum = cell.dayNumberLabel.get();
    auto cachedLabel = cell.label.get();

    // Tìm và giải quyết chính xác cặp TextBlock của ô ngày.
    // Nếu chưa có hoặc nhãn hiện tại là kLunarTextBlockName (có nguy cơ bị chèn đè lên LunarTextBlock gốc),
    // luôn quét lại để ưu tiên LunarTextBlock gốc và dọn dẹp phần tử trùng lặp.
    if (!cachedDayNum || !cachedLabel || ElementName(cachedLabel) == kLunarTextBlockName) {
        wuxc::TextBlock dayNum = nullptr;
        wuxc::TextBlock label = nullptr;
        FindDayCellTextBlocks(item, key % 100, dayNum, label);
        if (dayNum) {
            cachedDayNum = dayNum;
            cell.dayNumberLabel = winrt::make_weak(dayNum);
        }
        if (label) {
            cachedLabel = label;
        }
    }

    // Nhẹ nhàng chỉnh số ngày dương nhích lên trên một chút để chừa chỗ cho chữ âm lịch bên dưới
    if (cachedDayNum) {
        try {
            if (settings->showLunarDate) {
                cachedDayNum.VerticalAlignment(wux::VerticalAlignment::Top);
                cachedDayNum.Margin(wux::Thickness{0, 2, 0, 0});
            } else {
                cachedDayNum.ClearValue(wux::FrameworkElement::VerticalAlignmentProperty());
                cachedDayNum.ClearValue(wux::FrameworkElement::MarginProperty());
            }
        } catch (...) {
        }
    }

    wuxc::TextBlock label = cachedLabel;
    if (!label) {
        return;
    }

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
        cell.savedFontSize = label.FontSize();
        cell.hasSavedFontSize = true;
        cell.savedText.clear();
        cell.appliedText.clear();
        cell.hasSavedText = false;
        t_labelOwners[cell.labelId] = winrt::make_weak(item);

        try {
            cell.labelChangedToken = label.RegisterPropertyChangedCallback(
                wuxc::TextBlock::TextProperty(),
                [](wux::DependencyObject const& sender,
                   wux::DependencyProperty const&) {
                    OnLabelTextChanged(sender);
                });
        } catch (...) {
        }
    }

    const std::wstring& desired = displayInfo.label;

    try {
        label.TextTrimming(wux::TextTrimming::None);
        label.TextWrapping(wux::TextWrapping::NoWrap);
        label.HorizontalAlignment(wux::HorizontalAlignment::Center);
        label.VerticalAlignment(wux::VerticalAlignment::Bottom);
        label.TextAlignment(wux::TextAlignment::Center);
        label.Margin(wux::Thickness{0, 0, 0, 2});

        double wantedFontSize = settings->showLunarDate
                                    ? static_cast<double>(settings->lunarFontSize)
                                    : cell.savedFontSize;
        if (displayInfo.shouldHighlight && desired.size() >= 7) {
            wantedFontSize = 7.0;
        }
        if (std::fabs(label.FontSize() - wantedFontSize) > 0.1) {
            label.FontSize(wantedFontSize);
            t_applyWrote = true;
        }
    } catch (...) {
    }

    std::wstring current = ElementText(label);

    if (!desired.empty() && current != desired) {
        if (current != cell.appliedText) {
            cell.savedText = current;
            cell.hasSavedText = true;
        }
        try {
            label.Text(desired);
            t_applyWrote = true;
        } catch (...) {
        }
    } else if (desired.empty() && !cell.appliedText.empty() && current == cell.appliedText) {
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

    UpdateCachedBrushesIfNeeded();

    // 3. Đổi màu chữ và tạo hình vuông bo góc trong suốt cho ngày hôm nay
    if (isToday) {
        UnlockTextBlockColor(cachedDayNum);
        UnlockTextBlockColor(label);
        try {
            label.Foreground(t_cachedBrushes.foregroundBrush);
            label.Opacity(0.9);
        } catch (...) {}

        if (settings->useTransparentRoundedToday && (!cell.lastIsToday || cell.lastStyledKey != key)) {
            std::vector<FoundElement> descendants;
            CollectDescendants(item, descendants);
            for (const auto& found : descendants) {
                if (auto b = found.element.try_as<wuxc::Border>()) {
                    try {
                        b.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
                        b.BorderBrush(t_cachedBrushes.todayBorderBrush);
                        b.BorderThickness(wux::Thickness{1.2, 1.2, 1.2, 1.2});
                        t_applyWrote = true;
                    } catch (...) {}
                }
            }
        }
    } else {
        if (cell.lastIsToday) {
            std::vector<FoundElement> descendants;
            CollectDescendants(item, descendants);
            for (const auto& found : descendants) {
                if (auto b = found.element.try_as<wuxc::Border>()) {
                    try {
                        b.ClearValue(wuxc::Border::CornerRadiusProperty());
                        b.ClearValue(wuxc::Border::BorderBrushProperty());
                        b.ClearValue(wuxc::Border::BorderThicknessProperty());
                        t_applyWrote = true;
                    } catch (...) {}
                }
            }
        }

        if (shouldColorRed) {
            if (cachedDayNum) LockTextBlockColorStable(cachedDayNum, settings->holidayColor);
            LockTextBlockColorStable(label, settings->holidayColor);
            try {
                label.Opacity(1.0);
            } catch (...) {}
        } else {
            if (cachedDayNum) UnlockTextBlockColor(cachedDayNum);
            UnlockTextBlockColor(label);
            try {
                if (cachedDayNum) {
                    if (auto fg = cachedDayNum.Foreground()) {
                        label.Foreground(fg);
                    }
                }
                label.Opacity(0.65);
            } catch (...) {}
        }
    }

    cell.lastStyledKey = key;
    cell.lastIsToday = isToday;
}

////////////////////////////////////////////////////////////////////////////////
// Visual Tree Sweep

thread_local winrt::Windows::System::DispatcherQueueTimer t_sweepTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker t_sweepTimerRevoker;

constexpr int kSweepIntervalMs = 500;
constexpr int kSweepQuietTicksToStop = 5;

thread_local int t_sweepQuietTicks = 0;

void SweepDayItems(bool forceFullTreeScan = false);

void StopSweepTimer() {
    t_sweepQuietTicks = 0;
    if (t_sweepTimer) {
        try {
            t_sweepTimer.Stop();
        } catch (...) {
        }
    }
}

void StartSweepTimer() {
    t_sweepQuietTicks = 0;
    try {
        if (!t_sweepTimer) {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
                return;
            }

            t_sweepTimer = dispatcherQueue.CreateTimer();
            t_sweepTimer.Interval(std::chrono::milliseconds{kSweepIntervalMs});
            t_sweepTimerRevoker = t_sweepTimer.Tick(
                winrt::auto_revoke,
                [](winrt::Windows::System::DispatcherQueueTimer const&,
                   wf::IInspectable const&) { SweepDayItems(false); });
        }

        if (!t_sweepTimer.IsRunning()) {
            t_sweepTimer.Start();
        }
    } catch (...) {
    }
}

constexpr size_t kMaxScannedElements = 16384;
constexpr int kMaxScanDepth = 96;

void CollectDayItemsFromElement(wux::DependencyObject const& root,
                                size_t& budget,
                                int depth) {
    if (!root || budget == 0 || depth > kMaxScanDepth) {
        return;
    }
    budget--;

    if (auto dayItem = root.try_as<wuxc::CalendarViewDayItem>()) {
        TrackDayItem(dayItem);
        return;
    }

    if (!t_calendarView.get()) {
        if (auto cv = root.try_as<wuxc::CalendarView>()) {
            t_calendarView = winrt::make_weak(cv);
        }
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

void CollectDayItemsOnCurrentThread() {
    wux::Window window = nullptr;
    try {
        window = wux::Window::Current();
    } catch (...) {
        return;
    }

    if (!window) {
        return;
    }

    try {
        size_t budget = kMaxScannedElements;

        if (auto content = window.Content()) {
            CollectDayItemsFromElement(content, budget, 0);
        }

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

void StyleWeekDayNames(wuxc::CalendarView const& cv, Settings const& settings) {
    if (!cv) {
        return;
    }

    std::vector<FoundElement> descendants;
    CollectDescendants(cv, descendants);

    wuxc::Grid weekDayGrid = nullptr;
    for (const auto& found : descendants) {
        if (auto grid = found.element.try_as<wuxc::Grid>()) {
            if (ElementName(grid) == L"WeekDayNames") {
                weekDayGrid = grid;
                break;
            }
        }
    }

    if (!weekDayGrid) {
        return;
    }

    int firstDay = 1; // Mặc định thứ Hai (Monday)
    try {
        firstDay = static_cast<int>(cv.FirstDayOfWeek());
    } catch (...) {
    }

    int childCount = 0;
    try {
        childCount = weekDayGrid.Children().Size();
    } catch (...) {
        return;
    }

    for (int i = 0; i < childCount; i++) {
        try {
            auto child = weekDayGrid.Children().GetAt(i);
            if (auto tb = child.try_as<wuxc::TextBlock>()) {
                int col = wuxc::Grid::GetColumn(tb);
                // 0 = Sunday, 6 = Saturday
                int dayOfWeek = (firstDay + col) % 7;
                bool isWeekend = (dayOfWeek == 0 || dayOfWeek == 6);

                if (settings.highlightWeekendHeader && isWeekend) {
                    LockTextBlockColorStable(tb, settings.holidayColor);
                } else {
                    UnlockTextBlockColor(tb);
                }
            }
        } catch (...) {
        }
    }
}

thread_local bool t_calendarViewStyled = false;
thread_local bool t_lastStyledTransparentToday = false;
thread_local winrt::Windows::UI::Color t_lastCalendarAccentColor{};

void StyleCalendarView(wuxc::CalendarView const& cv, Settings const& settings) {
    if (!cv) {
        return;
    }
    try {
        StyleWeekDayNames(cv, settings);

        UpdateCachedBrushesIfNeeded();

        if (settings.useTransparentRoundedToday) {
            if (!t_calendarViewStyled || !t_lastStyledTransparentToday ||
                t_lastCalendarAccentColor != t_cachedBrushes.accentColor) {
                cv.CalendarItemCornerRadius(wux::CornerRadius{6, 6, 6, 6});
                cv.TodayBackground(t_cachedBrushes.todayBgBrush);
                cv.TodayHoverBackground(t_cachedBrushes.todayHoverBgBrush);
                cv.TodayPressedBackground(t_cachedBrushes.todayPressedBgBrush);
                cv.TodayForeground(t_cachedBrushes.foregroundBrush);

                t_calendarViewStyled = true;
                t_lastStyledTransparentToday = true;
                t_lastCalendarAccentColor = t_cachedBrushes.accentColor;
                t_applyWrote = true;
            }
        } else {
            if (t_calendarViewStyled && t_lastStyledTransparentToday) {
                cv.ClearValue(wuxc::CalendarView::CalendarItemCornerRadiusProperty());
                cv.ClearValue(wuxc::CalendarView::TodayBackgroundProperty());
                cv.ClearValue(wuxc::CalendarView::TodayHoverBackgroundProperty());
                cv.ClearValue(wuxc::CalendarView::TodayPressedBackgroundProperty());
                cv.ClearValue(wuxc::CalendarView::TodayForegroundProperty());

                t_calendarViewStyled = true;
                t_lastStyledTransparentToday = false;
                t_applyWrote = true;
            }
        }
    } catch (...) {
    }
}

void InitializeForCurrentThread();

void SweepDayItems(bool forceFullTreeScan) {
    InitializeForCurrentThread();
    t_applyWrote = false;

    const auto settings = GetSettings();

    if (forceFullTreeScan || t_dayItems.size() < 42 || !t_calendarView.get()) {
        CollectDayItemsOnCurrentThread();
    } else if (auto calView = t_calendarView.get()) {
        size_t budget = 1024;
        CollectDayItemsFromElement(calView, budget, 0);
    }

    if (auto calView = t_calendarView.get()) {
        StyleCalendarView(calView, *settings);
    }

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

    for (auto it = t_colorLockers.begin(); it != t_colorLockers.end();) {
        if (it->second.textBlock.get()) {
            ++it;
        } else {
            it = t_colorLockers.erase(it);
        }
    }

    if (t_applyWrote) {
        t_sweepQuietTicks = 0;
    } else if (++t_sweepQuietTicks >= kSweepQuietTicksToStop) {
        StopSweepTimer();
    }
}

void OnCalendarOpenedOnCurrentThread() {
    InitializeForCurrentThread();
    StartSweepTimer();
    SweepDayItems(true);
}

void UninitializeDayItemsForCurrentThread() {
    // 1. Dọn dẹp trạng thái các ô ngày đã được style
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

        if (auto dayNumber = cell.dayNumberLabel.get()) {
            UnlockTextBlockColor(dayNumber);
            try {
                dayNumber.ClearValue(wux::FrameworkElement::VerticalAlignmentProperty());
                dayNumber.ClearValue(wux::FrameworkElement::MarginProperty());
            } catch (...) {}
        }

        if (auto label = cell.label.get()) {
            UnlockTextBlockColor(label);
            if (cell.labelChangedToken) {
                try {
                    label.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::TextProperty(), cell.labelChangedToken);
                } catch (...) {
                }
                cell.labelChangedToken = 0;
            }
            try {
                label.ClearValue(wux::FrameworkElement::HorizontalAlignmentProperty());
                label.ClearValue(wux::FrameworkElement::VerticalAlignmentProperty());
                label.ClearValue(wuxc::TextBlock::TextAlignmentProperty());
                label.ClearValue(wux::FrameworkElement::MarginProperty());
                label.ClearValue(wuxc::TextBlock::TextTrimmingProperty());
                label.ClearValue(wuxc::TextBlock::TextWrappingProperty());
                label.ClearValue(wux::UIElement::OpacityProperty());
                label.ClearValue(wuxc::TextBlock::ForegroundProperty());

                if (ElementName(label) == kLunarTextBlockName) {
                    if (auto parent = wuxm::VisualTreeHelper::GetParent(label)) {
                        if (auto panel = parent.try_as<wuxc::Panel>()) {
                            uint32_t index = 0;
                            if (panel.Children().IndexOf(label, index)) {
                                panel.Children().RemoveAt(index);
                            }
                        }
                    }
                } else if (cell.hasSavedText) {
                    label.Text(cell.savedText);
                }
                if (cell.hasSavedFontSize) {
                    label.FontSize(cell.savedFontSize);
                }
            } catch (...) {
            }
        }

        // Hoàn nguyên viền hình vuông bo góc nếu có
        std::vector<FoundElement> descendants;
        CollectDescendants(item, descendants);
        for (const auto& found : descendants) {
            if (auto b = found.element.try_as<wuxc::Border>()) {
                try {
                    b.ClearValue(wuxc::Border::CornerRadiusProperty());
                    b.ClearValue(wuxc::Border::BorderBrushProperty());
                    b.ClearValue(wuxc::Border::BorderThicknessProperty());
                } catch (...) {}
            }
        }
    }

    // 2. Mở khóa màu các thứ trên thanh WeekDayNames trước khi xóa bộ nhớ locker
    if (auto calView = t_calendarView.get()) {
        try {
            std::vector<FoundElement> descendants;
            CollectDescendants(calView, descendants);
            for (const auto& found : descendants) {
                if (auto grid = found.element.try_as<wuxc::Grid>()) {
                    if (ElementName(grid) == L"WeekDayNames") {
                        int childCount = grid.Children().Size();
                        for (int i = 0; i < childCount; i++) {
                            if (auto tb = grid.Children().GetAt(i).try_as<wuxc::TextBlock>()) {
                                UnlockTextBlockColor(tb);
                            }
                        }
                        break;
                    }
                }
            }
        } catch (...) {}

        try {
            calView.ClearValue(wuxc::CalendarView::CalendarItemCornerRadiusProperty());
            calView.ClearValue(wuxc::CalendarView::TodayBackgroundProperty());
            calView.ClearValue(wuxc::CalendarView::TodayHoverBackgroundProperty());
            calView.ClearValue(wuxc::CalendarView::TodayPressedBackgroundProperty());
            calView.ClearValue(wuxc::CalendarView::TodayForegroundProperty());
        } catch (...) {}
    }

    // 3. Hủy đăng ký callback Foreground trên tất cả ColorLocker còn hoạt động
    for (auto& [id, locker] : t_colorLockers) {
        if (locker.active && locker.token) {
            if (auto tb = locker.textBlock.get()) {
                try {
                    tb.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::ForegroundProperty(), locker.token);
                } catch (...) {}
            }
            locker.active = false;
            locker.token = 0;
        }
    }

    decltype(t_cells)().swap(t_cells);
    decltype(t_dayItems)().swap(t_dayItems);
    decltype(t_labelOwners)().swap(t_labelOwners);
    decltype(t_colorLockers)().swap(t_colorLockers);

    t_calendarViewStyled = false;
    t_lastStyledTransparentToday = false;
    t_cachedBrushes = {};

    StopSweepTimer();
    t_sweepTimerRevoker.revoke();
    t_sweepTimer = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Process plumbing

std::atomic<bool> g_shuttingDown{false};
std::atomic<UINT> g_runFromWindowThreadViaPostMessageMsg{0};

UINT RunFromWindowThreadViaPostMessageMsg() {
    UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        message = RegisterWindowMessage(
            L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);
        g_runFromWindowThreadViaPostMessageMsg.store(message);
    }
    return message;
}

std::mutex g_pendingPostHooksMutex;
std::vector<HHOOK> g_pendingPostHooks;

void RememberPendingPostHook(HHOOK hook) {
    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    g_pendingPostHooks.push_back(hook);
}

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
        UnhookWindowsHookEx(hook);
    }
}

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
}

void UninitializeForCurrentThread() {
    DrainPendingRunFromWindowThreadPostsOnCurrentThread();

    if (!g_initializedForThread) {
        return;
    }

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

bool IsCalendarWindowClass(PCWSTR className) {
    if (!className) {
        return false;
    }
    return (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
            _wcsicmp(className, L"ControlCenterWindow") == 0);
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName) {
    if (g_shuttingDown) {
        return;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName || !IsCalendarWindowClass(lpClassName)) {
        return;
    }

    RunFromWindowThreadViaPostMessage(
        hWnd,
        [](PVOID) {
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
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName);
    }
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
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName);
    }
    return hWnd;
}

void OnWindowShown(HWND hWnd) {
    if (g_shuttingDown) {
        return;
    }

    DWORD dwProcessId = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return;
    }

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
        !IsCalendarWindowClass(szClassName)) {
        return;
    }

    RunFromWindowThreadViaPostMessage(
        hWnd, [](PVOID) { OnCalendarOpenedOnCurrentThread(); }, nullptr);
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    const BOOL wasVisible = ShowWindow_Original(hWnd, nCmdShow);
    if (nCmdShow != SW_HIDE) {
        OnWindowShown(hWnd);
    }
    return wasVisible;
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

            WCHAR szClassName[64];
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
                StartSweepTimer();
                SweepDayItems(true);
            },
            nullptr);
    }
}

template <typename Prototype>
bool InstallHook(HMODULE module,
                 PCSTR functionName,
                 Prototype* hook,
                 Prototype** original) {
    if (!module) {
        Wh_Log(L"InstallHook: module is null for %S", functionName);
        return false;
    }
    Prototype* target =
        reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));
    if (!target) {
        Wh_Log(L"InstallHook: GetProcAddress failed for %S", functionName);
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"InstallHook: SetFunctionHook failed for %S", functionName);
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Mod Lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L"> Wh_ModInit Native Lunar Calendar");

    g_shuttingDown = false;

    LoadSettings();
    RunFromWindowThreadViaPostMessageMsg();

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    InstallHook(user32Module, "CreateWindowInBand", CreateWindowInBand_Hook,
                &CreateWindowInBand_Original);
    InstallHook(user32Module, "CreateWindowInBandEx",
                CreateWindowInBandEx_Hook, &CreateWindowInBandEx_Original);
    InstallHook(user32Module, "ShowWindow", ShowWindow_Hook, &ShowWindow_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"> Wh_ModAfterInit Native Lunar Calendar");

    for (auto hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                OnCalendarOpenedOnCurrentThread();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"> Wh_ModUninit Native Lunar Calendar");
    g_shuttingDown = true;

    for (auto hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }

    UnhookPendingPostHooks();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> Wh_ModSettingsChanged");
    LoadSettings();
    RefreshCalendarOnUiThreads();
}