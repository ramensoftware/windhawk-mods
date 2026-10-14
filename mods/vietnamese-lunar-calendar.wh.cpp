// ==WindhawkMod==
// @id              vietnamese-lunar-calendar
// @name            Vietnamese Lunar Calendar & Horoscope
// @name:vi-VN      Lịch Âm Việt Nam & Tử Vi Vạn Niên
// @description     Displays the Vietnamese Lunar Calendar, traditional holidays, Sexagenary cycle (Can Chi), and daily Horoscope (Tử Vi) on the Windows 11 taskbar clock, tooltip, and calendar flyout
// @description:vi-VN Hiển thị Âm lịch Việt Nam, ngày lễ truyền thống, Bát tự Can Chi và Tử Vi hàng ngày trên đồng hồ Taskbar, hộp chú thích và khay Lịch Windows 11
// @version         1.0.0
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vietnamese Lunar Calendar & Horoscope

Integrates the **Vietnamese Lunar Calendar (Lịch Vạn Niên, UTC+7)** and **Daily Horoscope (Tử Vi)** directly into the Windows 11 taskbar clock, clock tooltip, and calendar flyout without requiring Windows auxiliary calendar settings.

Only **Windows 11** is supported.

![Taskbar clock and horoscope tooltip](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/screenshot1.png)

Taskbar clock with lunar date and detailed horoscope tooltip

![Calendar flyout with lunar dates and horoscope page](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/screenshot2.png)

Calendar flyout with Vietnamese lunar dates, holidays, and interactive horoscope view

## Features

1. **Taskbar Clock (`explorer.exe`)**
   * Preserves your system time format (12/24-hour clock and seconds) while appending the weekday and solar date on the top line (e.g., `14:46 - T4, 14/10`) and displaying the Vietnamese lunar date with the Sexagenary year on the bottom line (e.g., `05/09 Bính Ngọ`).
   * Can be toggled off or switched to preserve the default top line in the mod settings.
2. **Detailed Horoscope Tooltip (`explorer.exe`)**
   * Hovering over the taskbar clock appends a full Vietnamese almanac breakdown:
     * Lunar date, Sexagenary year, and special Vietnamese holidays/events.
     * Four Pillars (Bát Tự): Hour, Day, Month, and Year stems and branches (Can Chi).
     * Five Elements destiny (Ngũ Hành Nạp Âm), 12 Auspicious/Inauspicious stars (Hoàng Đạo / Hắc Đạo), and 12 Day Officers (Thập Nhị Trực).
     * Six Auspicious Hours (Giờ Hoàng Đạo) of the day.
     * Daily recommendations (Good for / Avoid), conflicting zodiac signs (Tuổi xung), and auspicious travel directions (Hướng xuất hành).
3. **Calendar Flyout Grid & Header (`ShellExperienceHost.exe` / `ShellHost.exe`)**
   * Displays the full Vietnamese lunar date in the calendar header (`CalendarHeader`) and beneath each solar day cell, even when *"Don't show additional calendars"* is selected in Windows Settings.
   * Color-codes traditional Vietnamese solar holidays, lunar holidays, and major festivals (Tết Nguyên Đán, Rằm tháng Giêng, Giỗ Tổ Hùng Vương, Vu Lan, Trung Thu, etc.).
4. **Interactive Horoscope Detail View**
   * Clicking any date cell in the calendar flyout opens a dedicated almanac and horoscope page for that specific date, with a back button (`←`) to return to the month grid.
5. **Bilingual Support (English & Vietnamese)**
   * Automatically follows your Windows UI language (Vietnamese on `vi-VN` systems, English otherwise) or can be manually selected in settings.

## Compatibility Note

Because this mod customizes the taskbar clock text, clock tooltip, and calendar flyout day cells, enabling those respective features alongside **Taskbar Clock Customization** or **Chinese Holiday Calendar** will cause the mods to overwrite each other's text. You can disable individual features in this mod's settings to use them side by side.

## Credits

* Astronomical Vietnamese lunar calendar calculation based on **Hồ Ngọc Đức**'s UTC+7 algorithm.
* Calendar flyout injection plumbing adapted from [Chinese Holiday Calendar](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/chinese-holiday-calendar.wh.cpp) by **dcsmf**.
* Taskbar clock and tooltip hooks adapted from [Taskbar Clock Customization](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-clock-customization.wh.cpp) by **m417z**.

---

# Lịch Âm Việt Nam & Tử Vi Vạn Niên (Tiếng Việt)

Tích hợp **Lịch Vạn Niên (Âm lịch Việt Nam chuẩn múi giờ UTC+7)** và **Tử Vi hàng ngày** trực tiếp vào đồng hồ Taskbar, hộp chú thích (Tooltip) và khay Lịch của **Windows 11** mà không cần bật lịch phụ tiếng Trung trong Cài đặt Windows.

## Tính năng chính

1. **Đồng hồ Âm - Dương trên thanh Taskbar:** Giữ nguyên định dạng giờ của hệ thống (12h/24h và hiển thị giây), kết hợp Thứ + Ngày/Tháng Dương lịch ở dòng trên và `Ngày/Tháng Âm lịch + Năm Can Chi` ở dòng dưới.
2. **Bảng Bát Tự & Tử Vi trong hộp chú thích (Tooltip):** Khi rê chuột vào đồng hồ góc phải, hộp chú thích hiển thị đầy đủ ngày Âm lịch, Bát tự (Giờ, Ngày, Tháng, Năm Can Chi), Mệnh ngày (Nạp âm), Sao Hoàng Đạo/Hắc Đạo, Thập Nhị Trực, 6 Giờ Hoàng Đạo, Việc Nên/Kỵ và Hướng xuất hành.
3. **Khay Lịch Vạn Niên & Đánh dấu Ngày Lễ:** Tự động hiển thị ngày Âm lịch dưới từng ô ngày Dương lịch và tô màu đỏ theo đúng chuẩn Lịch Bloc Việt Nam (Lễ Dương lịch, Lễ Âm lịch, Tết Nguyên Đán, Rằm/Mùng 1).
4. **Tra cứu Tử Vi theo ngày bất kỳ:** Click vào một ô ngày bất kỳ trên khay lịch để mở trang chi tiết Bát Tự & Tử Vi của ngày đó (bấm nút `←` để quay lại lưới lịch).
5. **Tùy chỉnh linh hoạt:** Từng tính năng (Đồng hồ Taskbar, Tooltip, Khay lịch, Trang Tử Vi) và ngôn ngữ hiển thị đều có thể bật/tắt riêng trong phần Cài đặt của mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Display language
  $name:vi-VN: Ngôn ngữ hiển thị
  $description: Language used for labels, weekdays, and horoscope descriptions.
  $description:vi-VN: Ngôn ngữ dùng cho các nhãn, thứ trong tuần và nội dung tử vi.
  $options:
  - auto: System default (Auto)
  - en: English
  - vi: Tiếng Việt (Vietnamese)
  $options:vi-VN:
  - auto: Mặc định hệ thống (Tự động)
  - en: Tiếng Anh (English)
  - vi: Tiếng Việt
- enableTaskbarClock: true
  $name: Show lunar date on taskbar clock
  $name:vi-VN: Hiển thị Âm lịch trên đồng hồ Taskbar
  $description: Display the lunar date on the taskbar clock. Disable this option if you use Taskbar Clock Customization.
  $description:vi-VN: Hiển thị ngày Âm lịch trên đồng hồ Taskbar. Hãy tắt tùy chọn này nếu bạn dùng chung với mod Taskbar Clock Customization.
- taskbarClockSolarOnTop: true
  $name: Show weekday and solar date on top clock line
  $name:vi-VN: Gộp Thứ và Ngày/Tháng Dương lịch lên dòng trên đồng hồ
  $description: When enabled, shows "Time - Weekday, DD/MM" on the top line and the lunar date on the bottom line. When disabled, keeps the top line as Time only and shows "Solar date • Lunar date" on the bottom line.
  $description:vi-VN: Khi bật, dòng trên hiển thị "Giờ - Thứ, Ngày/Tháng" và dòng dưới hiển thị Âm lịch. Khi tắt, dòng trên chỉ hiện Giờ và dòng dưới hiện "Dương lịch • Âm lịch".
- enableTaskbarTooltip: true
  $name: Show horoscope & almanac in clock tooltip
  $name:vi-VN: Hiển thị Tử Vi & Bát Tự trong hộp chú thích (Tooltip) đồng hồ
- enableCalendarFlyout: true
  $name: Show lunar dates & holidays in calendar flyout
  $name:vi-VN: Hiển thị Âm lịch & ngày lễ trong khay Lịch
- enableHoroscopePage: true
  $name: Open horoscope detail page when clicking a date
  $name:vi-VN: Mở trang chi tiết Tử Vi khi bấm vào ngày trên lịch
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std::string_view_literals;

#undef GetCurrentTime

#include <Unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

thread_local bool g_initializedForThread = false;
std::atomic<bool> g_shuttingDown{false};

#define FORMATTED_BUFFER_SIZE 256

////////////////////////////////////////////////////////////////////////////////
// Settings & Localization

enum class ModLanguage { Auto, English, Vietnamese };

struct ModSettings {
    ModLanguage language = ModLanguage::Auto;
    bool enableTaskbarClock = true;
    bool taskbarClockSolarOnTop = true;
    bool enableTaskbarTooltip = true;
    bool enableCalendarFlyout = true;
    bool enableHoroscopePage = true;
};

std::mutex g_settingsMutex;
ModSettings g_settings;

void LoadSettings() {
    ModSettings s;
    WindhawkUtils::StringSetting lang = WindhawkUtils::StringSetting::make(L"language");
    if (_wcsicmp(lang.get(), L"vi") == 0) {
        s.language = ModLanguage::Vietnamese;
    } else if (_wcsicmp(lang.get(), L"en") == 0) {
        s.language = ModLanguage::English;
    } else {
        s.language = ModLanguage::Auto;
    }

    s.enableTaskbarClock = Wh_GetIntSetting(L"enableTaskbarClock") != 0;
    s.taskbarClockSolarOnTop = Wh_GetIntSetting(L"taskbarClockSolarOnTop") != 0;
    s.enableTaskbarTooltip = Wh_GetIntSetting(L"enableTaskbarTooltip") != 0;
    s.enableCalendarFlyout = Wh_GetIntSetting(L"enableCalendarFlyout") != 0;
    s.enableHoroscopePage = Wh_GetIntSetting(L"enableHoroscopePage") != 0;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = s;
}

ModSettings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

bool UseVietnameseLabels(ModSettings const& settings) {
    if (settings.language == ModLanguage::Vietnamese) return true;
    if (settings.language == ModLanguage::English) return false;
    LANGID langId = GetUserDefaultUILanguage();
    return PRIMARYLANGID(langId) == LANG_VIETNAMESE;
}

PCWSTR PickText(bool vi, PCWSTR viText, PCWSTR enText) {
    return vi ? viText : enText;
}

////////////////////////////////////////////////////////////////////////////////
// String & Date Utilities

std::wstring TrimWhitespace(std::wstring const& text) {
    size_t begin = 0, end = text.size();
    while (begin < end && iswspace(text[begin])) begin++;
    while (end > begin && iswspace(text[end - 1])) end--;
    return text.substr(begin, end - begin);
}

int32_t MakeDateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

int DaysInMonth(int year, int month) {
    if (month < 1 || month > 12) return 0;
    static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) return 29;
    return kDays[month - 1];
}

bool IsValidDate(int year, int month, int day) {
    if (year < 1900 || year > 9999) return false;
    return day >= 1 && day <= DaysInMonth(year, month);
}

int32_t DateKeyFromDateTime(wf::DateTime const& dateTime) {
    ULARGE_INTEGER uli;
    uli.QuadPart = static_cast<ULONGLONG>(dateTime.time_since_epoch().count());
    FILETIME fileTime;
    fileTime.dwLowDateTime = uli.LowPart;
    fileTime.dwHighDateTime = uli.HighPart;
    SYSTEMTIME systemTimeUtc{};
    if (!FileTimeToSystemTime(&fileTime, &systemTimeUtc)) return 0;
    SYSTEMTIME systemTimeLocal{};
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &systemTimeUtc, &systemTimeLocal)) return 0;

    auto ticksOf = [](SYSTEMTIME const& systemTime, ULONGLONG& ticks) -> bool {
        FILETIME ft{};
        if (!SystemTimeToFileTime(&systemTime, &ft)) return false;
        ULARGE_INTEGER value;
        value.LowPart = ft.dwLowDateTime;
        value.HighPart = ft.dwHighDateTime;
        ticks = value.QuadPart;
        return true;
    };

    ULONGLONG localTicks = 0, utcTicks = 0;
    if (!ticksOf(systemTimeLocal, localTicks) || !ticksOf(systemTimeUtc, utcTicks)) return 0;
    const SYSTEMTIME& date = localTicks < utcTicks ? systemTimeUtc : systemTimeLocal;
    return MakeDateKey(date.wYear, date.wMonth, date.wDay);
}

////////////////////////////////////////////////////////////////////////////////
// Pure Arithmetic Vietnamese Lunar Calendar (Hồ Ngọc Đức Algorithm, UTC+7)
// Eliminates WinRT Calendar objects and fixes UTC+7 vs UTC+8 new-moon differences.

const PCWSTR CAN_10[] = {
    L"Giáp", L"Ất", L"Bính", L"Đinh", L"Mậu", L"Kỷ", L"Canh", L"Tân", L"Nhâm", L"Quý"
};
const PCWSTR CHI_12[] = {
    L"Tý", L"Sửu", L"Dần", L"Mão", L"Thìn", L"Tỵ", L"Ngọ", L"Mùi", L"Thân", L"Dậu", L"Tuất", L"Hợi"
};

std::wstring GetCanChiYear(int year) {
    int canIdx = ((year + 6) % 10 + 10) % 10;
    int chiIdx = ((year + 8) % 12 + 12) % 12;
    return std::wstring(CAN_10[canIdx]) + L" " + CHI_12[chiIdx];
}

int CalculateJDN(int sDay, int sMonth, int sYear) {
    int a = (14 - sMonth) / 12;
    int y = sYear + 4800 - a;
    int m = sMonth + 12 * a - 3;
    return sDay + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

constexpr double kPi = 3.14159265358979323846;

int IntFloor(double d) {
    return static_cast<int>(std::floor(d));
}

double NewMoonJDN(int k) {
    double T = k / 1236.85;
    double T2 = T * T;
    double T3 = T2 * T;
    double dr = kPi / 180.0;
    double Jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * T2 - 0.000000155 * T3;
    Jd1 += 0.00033 * std::sin((166.56 + 132.87 * T - 0.009173 * T2) * dr);
    double M = 359.2242 + 29.10535608 * k - 0.0000333 * T2 - 0.00000347 * T3;
    double Mpr = 306.0253 + 385.81691806 * k + 0.0107306 * T2 + 0.00001236 * T3;
    double F = 21.2964 + 390.67050646 * k - 0.0016528 * T2 - 0.00000239 * T3;
    double C1 = (0.1734 - 0.000393 * T) * std::sin(M * dr) + 0.0021 * std::sin(2 * dr * M);
    C1 = C1 - 0.4068 * std::sin(Mpr * dr) + 0.0161 * std::sin(dr * 2 * Mpr);
    C1 = C1 - 0.0004 * std::sin(dr * 3 * Mpr);
    C1 = C1 + 0.0104 * std::sin(dr * 2 * F) - 0.0051 * std::sin(dr * (M + Mpr));
    C1 = C1 - 0.0074 * std::sin(dr * (M - Mpr)) + 0.0004 * std::sin(dr * (2 * F + M));
    C1 = C1 - 0.0004 * std::sin(dr * (2 * F - M)) - 0.0006 * std::sin(dr * (2 * F + Mpr));
    C1 = C1 + 0.0010 * std::sin(dr * (2 * F - Mpr)) + 0.0005 * std::sin(dr * (2 * Mpr + M));
    double deltat;
    if (T < -11) {
        deltat = 0.001 + 0.000839 * T + 0.0002261 * T2 - 0.00000845 * T3 - 0.000000081 * T * T3;
    } else {
        deltat = -0.000278 + 0.000265 * T + 0.000262 * T2;
    }
    return Jd1 + C1 - deltat;
}

int GetNewMoonDay(int k, double timeZone) {
    return IntFloor(NewMoonJDN(k) + 0.5 + timeZone / 24.0);
}

double SunLongitudeRadians(double jdn) {
    double T = (jdn - 2451545.0) / 36525.0;
    double T2 = T * T;
    double dr = kPi / 180.0;
    double M = 357.52910 + 35999.05030 * T - 0.0001559 * T2 - 0.00000048 * T * T2;
    double L0 = 280.46645 + 36000.76983 * T + 0.0003032 * T2;
    double DL = (1.914600 - 0.004817 * T - 0.000014 * T2) * std::sin(dr * M);
    DL += (0.019993 - 0.000101 * T) * std::sin(dr * 2 * M) + 0.000290 * std::sin(dr * 3 * M);
    double L = (L0 + DL) * dr;
    L -= kPi * 2 * IntFloor(L / (kPi * 2));
    return L;
}

int GetSunLongitudeSector(int dayNumber, double timeZone) {
    return IntFloor(SunLongitudeRadians(dayNumber - 0.5 - timeZone / 24.0) / kPi * 6.0);
}

int GetLunarMonth11(int yy, double timeZone) {
    int off = CalculateJDN(31, 12, yy) - 2415021;
    int k = IntFloor(off / 29.530588853);
    int nm = GetNewMoonDay(k, timeZone);
    int sunLong = GetSunLongitudeSector(nm, timeZone);
    if (sunLong >= 9) {
        nm = GetNewMoonDay(k - 1, timeZone);
    }
    return nm;
}

int GetLeapMonthOffset(int a11, double timeZone) {
    int k = IntFloor((a11 - 2415021.076998695) / 29.530588853 + 0.5);
    int last = 0;
    int i = 1;
    int arc = GetSunLongitudeSector(GetNewMoonDay(k + i, timeZone), timeZone);
    do {
        last = arc;
        i++;
        arc = GetSunLongitudeSector(GetNewMoonDay(k + i, timeZone), timeZone);
    } while (arc != last && i < 14);
    return i - 1;
}

void GetVietnameseLunarDate(int sYear, int sMonth, int sDay,
                            int& lDay, int& lMonth, int& lYear,
                            bool& isLeap, std::wstring& canChi) {
    constexpr double kVietnamTimeZone = 7.0;
    int dayNumber = CalculateJDN(sDay, sMonth, sYear);
    int k = IntFloor((dayNumber - 2415021.076998695) / 29.530588853);
    int monthStart = GetNewMoonDay(k + 1, kVietnamTimeZone);
    if (monthStart > dayNumber) {
        monthStart = GetNewMoonDay(k, kVietnamTimeZone);
    }
    int a11 = GetLunarMonth11(sYear, kVietnamTimeZone);
    int b11 = a11;
    if (a11 >= monthStart) {
        lYear = sYear;
        a11 = GetLunarMonth11(sYear - 1, kVietnamTimeZone);
    } else {
        lYear = sYear + 1;
        b11 = GetLunarMonth11(sYear + 1, kVietnamTimeZone);
    }
    lDay = dayNumber - monthStart + 1;
    int diff = IntFloor((monthStart - a11) / 29.0);
    isLeap = false;
    lMonth = diff + 11;
    if (b11 - a11 > 365) {
        int leapMonthDiff = GetLeapMonthOffset(a11, kVietnamTimeZone);
        if (diff >= leapMonthDiff) {
            lMonth = diff + 10;
            if (diff == leapMonthDiff) {
                isLeap = true;
            }
        }
    }
    if (lMonth > 12) {
        lMonth -= 12;
    }
    if (lMonth >= 11 && diff < 4) {
        lYear -= 1;
    }
    canChi = GetCanChiYear(lYear);
}

void GetLunarDateToday(int& lDay, int& lMonth, int& lYear, bool& isLeap, std::wstring& canChi) {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    GetVietnameseLunarDate(st.wYear, st.wMonth, st.wDay, lDay, lMonth, lYear, isLeap, canChi);
}

std::wstring FormatTaskbarLunarDate(int sYear, int sMonth, int sDay, bool vi) {
    int lDay, lMonth, lYear;
    bool isLeap;
    std::wstring canChi;
    GetVietnameseLunarDate(sYear, sMonth, sDay, lDay, lMonth, lYear, isLeap, canChi);

    WCHAR buffer[128]{};
    if (isLeap) {
        _snwprintf_s(buffer, _TRUNCATE, L"%02d/%02d (%s) %s",
                     lDay, lMonth, PickText(vi, L"Nhuận", L"Leap"), canChi.c_str());
    } else {
        _snwprintf_s(buffer, _TRUNCATE, L"%02d/%02d %s", lDay, lMonth, canChi.c_str());
    }
    return buffer;
}

////////////////////////////////////////////////////////////////////////////////
// Holidays & Horoscope Data Tables (Bilingual Vietnamese & English)

enum class HolidayScope { None, SolarOnly, LunarOnly, Both };

struct VietnameseHolidayResult {
    HolidayScope scope = HolidayScope::None;
    std::wstring names;
};

VietnameseHolidayResult EvaluateVietnameseHoliday(int sDay, int sMonth,
                                                  int lDay, int lMonth,
                                                  bool isLeap, bool vi) {
    VietnameseHolidayResult res;
    bool highlightSolar = false;
    bool highlightLunar = false;

    auto addName = [&](PCWSTR name) {
        if (!res.names.empty()) res.names += L" • ";
        res.names += name;
    };

    struct HolidayEntry {
        int day;
        int month;
        HolidayScope scope;
        PCWSTR viName;
        PCWSTR enName;
    };

    static const HolidayEntry kSolarHolidays[] = {
        { 1,  1,  HolidayScope::Both,      L"Tết Dương lịch",             L"New Year's Day" },
        { 14, 2,  HolidayScope::SolarOnly, L"Lễ Tình nhân (Valentine)",   L"Valentine's Day" },
        { 8,  3,  HolidayScope::SolarOnly, L"Quốc tế Phụ nữ",             L"International Women's Day" },
        { 26, 3,  HolidayScope::SolarOnly, L"Thành lập Đoàn TNCS HCM",    L"Youth Union Foundation Day" },
        { 30, 4,  HolidayScope::SolarOnly, L"Ngày Giải phóng miền Nam",   L"Reunification Day" },
        { 1,  5,  HolidayScope::SolarOnly, L"Quốc tế Lao động",           L"International Workers' Day" },
        { 1,  6,  HolidayScope::SolarOnly, L"Quốc tế Thiếu nhi",          L"International Children's Day" },
        { 2,  9,  HolidayScope::SolarOnly, L"Quốc khánh Việt Nam",        L"Vietnam National Day" },
        { 20, 10, HolidayScope::SolarOnly, L"Ngày Phụ nữ Việt Nam",       L"Vietnamese Women's Day" },
        { 20, 11, HolidayScope::SolarOnly, L"Ngày Nhà giáo Việt Nam",     L"Vietnamese Teachers' Day" },
        { 24, 11, HolidayScope::SolarOnly, L"Ngày Văn hóa Việt Nam",      L"Vietnam Cultural Heritage Day" },
        { 22, 12, HolidayScope::SolarOnly, L"Thành lập QĐND Việt Nam",    L"People's Army Foundation Day" },
        { 24, 12, HolidayScope::SolarOnly, L"Đêm Giáng Sinh",             L"Christmas Eve" },
        { 25, 12, HolidayScope::SolarOnly, L"Lễ Giáng Sinh (Noel)",       L"Christmas Day" },
    };

    static const HolidayEntry kLunarHolidays[] = {
        { 1,  1,  HolidayScope::Both,      L"Mùng 1 Tết Nguyên Đán",              L"Lunar New Year (Tết - Day 1)" },
        { 2,  1,  HolidayScope::Both,      L"Mùng 2 Tết Nguyên Đán",              L"Lunar New Year (Tết - Day 2)" },
        { 3,  1,  HolidayScope::Both,      L"Mùng 3 Tết Nguyên Đán",              L"Lunar New Year (Tết - Day 3)" },
        { 15, 1,  HolidayScope::Both,      L"Tết Nguyên Tiêu (Rằm tháng Giêng)",  L"First Full Moon Festival (Tết Nguyên Tiêu)" },
        { 3,  3,  HolidayScope::None,      L"Tết Hàn Thực",                       L"Cold Food Festival (Tết Hàn Thực)" },
        { 10, 3,  HolidayScope::LunarOnly, L"Giỗ Tổ Hùng Vương",                  L"Hùng Kings' Commemoration Day" },
        { 15, 4,  HolidayScope::LunarOnly, L"Lễ Phật Đản",                        L"Buddha's Birthday (Lễ Phật Đản)" },
        { 5,  5,  HolidayScope::LunarOnly, L"Tết Đoan Ngọ",                       L"Mid-Year Festival (Tết Đoan Ngọ)" },
        { 15, 7,  HolidayScope::Both,      L"Lễ Vu Lan (Rằm tháng Bảy)",          L"Vu Lan Festival (Ghost Festival)" },
        { 15, 8,  HolidayScope::Both,      L"Tết Trung Thu (Rằm tháng Tám)",      L"Mid-Autumn Festival (Tết Trung Thu)" },
        { 23, 12, HolidayScope::LunarOnly, L"Tiễn Ông Công Ông Táo",              L"Kitchen Guardians Day (Ông Công Ông Táo)" },
        { 29, 12, HolidayScope::Both,      L"Tất niên / Giao thừa",               L"Lunar New Year's Eve (Tất niên)" },
        { 30, 12, HolidayScope::Both,      L"Tất niên / Giao thừa",               L"Lunar New Year's Eve (Giao thừa)" },
    };

    for (const auto& entry : kSolarHolidays) {
        if (sDay == entry.day && sMonth == entry.month) {
            addName(PickText(vi, entry.viName, entry.enName));
            if (entry.scope == HolidayScope::Both || entry.scope == HolidayScope::SolarOnly) highlightSolar = true;
            if (entry.scope == HolidayScope::Both || entry.scope == HolidayScope::LunarOnly) highlightLunar = true;
        }
    }

    if (!isLeap) {
        bool matchedLunarFestival = false;
        for (const auto& entry : kLunarHolidays) {
            if (lDay == entry.day && lMonth == entry.month) {
                addName(PickText(vi, entry.viName, entry.enName));
                matchedLunarFestival = true;
                if (entry.scope == HolidayScope::Both || entry.scope == HolidayScope::SolarOnly) highlightSolar = true;
                if (entry.scope == HolidayScope::Both || entry.scope == HolidayScope::LunarOnly) highlightLunar = true;
            }
        }
        if (!matchedLunarFestival) {
            if (lDay == 1) addName(PickText(vi, L"Ngày Mùng 1 (Sóc)", L"New Moon Day (Mùng 1)"));
            else if (lDay == 15) addName(PickText(vi, L"Ngày Rằm (Vọng)", L"Full Moon Day (Ngày Rằm)"));
        }
    }

    if (highlightSolar && highlightLunar) res.scope = HolidayScope::Both;
    else if (highlightSolar) res.scope = HolidayScope::SolarOnly;
    else if (highlightLunar) res.scope = HolidayScope::LunarOnly;
    else res.scope = HolidayScope::None;

    return res;
}

struct ComputedHoroscope {
    int sDay = 1, sMonth = 1, sYear = 2000;
    int lDay = 1, lMonth = 1, lYear = 2000;
    bool isLeap = false;
    PCWSTR weekdayStr = L"";
    std::wstring hourCanChi;
    std::wstring dayCanChi;
    std::wstring monthCanChi;
    std::wstring yearCanChi;
    PCWSTR napAmStr = L"";
    PCWSTR starName = L"";
    bool isHoangDao = false;
    PCWSTR trucName = L"";
    PCWSTR goodFor = L"";
    PCWSTR badFor = L"";
    PCWSTR gioHoangDao = L"";
    std::wstring tuoiXung;
    PCWSTR huongHyThan = L"";
    PCWSTR huongTaiThan = L"";
    std::wstring specialHoliday;
};

ComputedHoroscope ComputeHoroscopeForDate(int sYear, int sMonth, int sDay, bool vi, int hour = 0) {
    ComputedHoroscope info;
    info.sYear = sYear;
    info.sMonth = sMonth;
    info.sDay = sDay;

    GetVietnameseLunarDate(sYear, sMonth, sDay,
                           info.lDay, info.lMonth, info.lYear,
                           info.isLeap, info.yearCanChi);

    int jdn = CalculateJDN(sDay, sMonth, sYear);

    static const PCWSTR kWeekdaysVi[] = {
        L"Thứ Hai", L"Thứ Ba", L"Thứ Tư", L"Thứ Năm", L"Thứ Sáu", L"Thứ Bảy", L"Chủ Nhật"
    };
    static const PCWSTR kWeekdaysEn[] = {
        L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday", L"Sunday"
    };
    info.weekdayStr = vi ? kWeekdaysVi[jdn % 7] : kWeekdaysEn[jdn % 7];

    int canDayIdx = (jdn + 9) % 10;
    int chiDayIdx = (jdn + 1) % 12;
    info.dayCanChi = std::wstring(CAN_10[canDayIdx]) + L" " + CHI_12[chiDayIdx];

    int canMonthIdx = (info.lYear * 12 + info.lMonth + 3) % 10;
    int chiMonthIdx = (info.lMonth + 1) % 12;
    info.monthCanChi = std::wstring(CAN_10[canMonthIdx]) + L" " + CHI_12[chiMonthIdx];

    int chiHourIdx = ((hour + 1) / 2) % 12;
    int canHourIdx = ((canDayIdx % 5) * 2 + chiHourIdx) % 10;
    info.hourCanChi = std::wstring(CAN_10[canHourIdx]) + L" " + CHI_12[chiHourIdx];

    struct BilingualText { PCWSTR vi; PCWSTR en; };
    static const BilingualText kNapAm[30] = {
        { L"Hải Trung Kim (Vàng trong biển)",    L"Hải Trung Kim (Sea Metal)" },
        { L"Lô Trung Hỏa (Lửa trong lò)",        L"Lô Trung Hỏa (Furnace Fire)" },
        { L"Đại Lâm Mộc (Gỗ rừng già)",          L"Đại Lâm Mộc (Forest Wood)" },
        { L"Lộ Bàng Thổ (Đất ven đường)",        L"Lộ Bàng Thổ (Roadside Earth)" },
        { L"Kiếm Phong Kim (Vàng mũi kiếm)",     L"Kiếm Phong Kim (Sword Metal)" },
        { L"Sơn Đầu Hỏa (Lửa đầu núi)",          L"Sơn Đầu Hỏa (Mountain Fire)" },
        { L"Giản Hạ Thủy (Nước dưới khe)",       L"Giản Hạ Thủy (Stream Water)" },
        { L"Thành Đầu Thổ (Đất tường thành)",    L"Thành Đầu Thổ (Fortress Earth)" },
        { L"Bạch Lạp Kim (Vàng chân đèn)",       L"Bạch Lạp Kim (Candlestick Metal)" },
        { L"Dương Liễu Mộc (Gỗ cây liễu)",       L"Dương Liễu Mộc (Willow Wood)" },
        { L"Tuyền Trung Thủy (Nước trong suối)", L"Tuyền Trung Thủy (Spring Water)" },
        { L"Ốc Thượng Thổ (Đất mái nhà)",        L"Ốc Thượng Thổ (Roof Earth)" },
        { L"Tích Lịch Hỏa (Lửa sấm sét)",        L"Tích Lịch Hỏa (Thunderbolt Fire)" },
        { L"Tùng Bách Mộc (Gỗ tùng bách)",       L"Tùng Bách Mộc (Pine Wood)" },
        { L"Trường Lưu Thủy (Nước chảy dài)",    L"Trường Lưu Thủy (River Water)" },
        { L"Sa Trung Kim (Vàng trong cát)",      L"Sa Trung Kim (Sand Metal)" },
        { L"Sơn Hạ Hỏa (Lửa dưới núi)",          L"Sơn Hạ Hỏa (Foothill Fire)" },
        { L"Bình Địa Mộc (Gỗ đồng bằng)",        L"Bình Địa Mộc (Plain Wood)" },
        { L"Bích Thượng Thổ (Đất tò vò)",        L"Bích Thượng Thổ (Wall Earth)" },
        { L"Kim Bạch Kim (Vàng pha bạc)",        L"Kim Bạch Kim (Gold Foil Metal)" },
        { L"Phúc Đăng Hỏa (Lửa đèn chụp)",       L"Phúc Đăng Hỏa (Lamp Fire)" },
        { L"Thiên Hà Thủy (Nước ngân hà)",       L"Thiên Hà Thủy (Celestial Water)" },
        { L"Đại Trạch Thổ (Đất nền nhà)",        L"Đại Trạch Thổ (Marsh Earth)" },
        { L"Thoa Xuyến Kim (Vàng trang sức)",    L"Thoa Xuyến Kim (Jewelry Metal)" },
        { L"Tang Đố Mộc (Gỗ cây dâu)",           L"Tang Đố Mộc (Mulberry Wood)" },
        { L"Đại Khê Thủy (Nước khe lớn)",        L"Đại Khê Thủy (Rapids Water)" },
        { L"Sa Trung Thổ (Đất pha cát)",         L"Sa Trung Thổ (Sand Earth)" },
        { L"Thiên Thượng Hỏa (Lửa trên trời)",   L"Thiên Thượng Hỏa (Solar Fire)" },
        { L"Thạch Lựu Mộc (Gỗ cây lựu)",         L"Thạch Lựu Mộc (Pomegranate Wood)" },
        { L"Đại Hải Thủy (Nước biển lớn)",       L"Đại Hải Thủy (Ocean Water)" },
    };
    int hoaGiapIdx = (6 * canDayIdx - 5 * chiDayIdx + 60) % 60;
    info.napAmStr = PickText(vi, kNapAm[hoaGiapIdx / 2].vi, kNapAm[hoaGiapIdx / 2].en);

    struct StarInfo { PCWSTR name; bool isHoangDao; };
    static const StarInfo kStars[12] = {
        { L"Thanh Long Hoàng Đạo", true },  { L"Minh Đường Hoàng Đạo", true },
        { L"Thiên Hình Hắc Đạo", false },   { L"Chu Tước Hắc Đạo", false },
        { L"Kim Quỹ Hoàng Đạo", true },     { L"Kim Đường Hoàng Đạo", true },
        { L"Bạch Hổ Hắc Đạo", false },      { L"Ngọc Đường Hoàng Đạo", true },
        { L"Thiên Lao Hắc Đạo", false },    { L"Nguyên Vũ Hắc Đạo", false },
        { L"Tư Mệnh Hoàng Đạo", true },     { L"Câu Trận Hắc Đạo", false }
    };
    int thanhLongStartChi = ((info.lMonth - 1) % 6) * 2;
    int starIdx = (chiDayIdx - thanhLongStartChi + 12) % 12;
    info.starName = kStars[starIdx].name;
    info.isHoangDao = kStars[starIdx].isHoangDao;

    struct TrucInfo {
        PCWSTR name;
        PCWSTR goodVi;
        PCWSTR badVi;
        PCWSTR goodEn;
        PCWSTR badEn;
    };
    static const TrucInfo kTruc[12] = {
        { L"Kiến",  L"Xuất hành, nhậm chức, cầu tài, ký kết",         L"Động thổ, đào đất, mở kho",
                    L"Travel, starting a role, signing contracts",     L"Groundbreaking, excavation" },
        { L"Trừ",   L"Thanh tẩy, chữa bệnh, dọn dẹp, cầu bình an",    L"Khai trương lớn, đi xa",
                    L"Cleansing, medical treatment, decluttering",     L"Grand openings, long journeys" },
        { L"Mãn",   L"Cúng tế, cầu tài lộc, khai trương, mở kho",      L"Kiện tụng, tranh chấp",
                    L"Prayers, wealth seeking, business opening",      L"Litigation, disputes" },
        { L"Bình",  L"Tu sửa, bàn bạc công việc, giao dịch, cưới hỏi", L"Khởi công lớn, thưa kiện",
                    L"Repairs, negotiations, trading, weddings",       L"Major construction, lawsuits" },
        { L"Định",  L"Ký hợp đồng, cầu danh, nhập học, mưu sự",        L"Xuất hành xa, kiện tụng",
                    L"Contracts, enrollment, long-term planning",      L"Long-distance travel, lawsuits" },
        { L"Chấp",  L"Xây dựng, tu tạo, lập khế ước, tuyển dụng",     L"Xuất tiền lớn, dọn nhà",
                    L"Building, renovations, hiring, agreements",      L"Large expenses, moving house" },
        { L"Phá",   L"Phá dỡ, giải quyết việc tồn đọng, chữa bệnh",    L"Khai trương, cưới hỏi, ký kết",
                    L"Demolition, resolving backlog, medical care",    L"Grand openings, weddings, contracts" },
        { L"Nguy",  L"Tế tự, cầu an, nghỉ ngơi, cẩn trọng đi lại",     L"Mạo hiểm, khởi công, đi sông nước",
                    L"Prayers for peace, rest, cautious travel",       L"High-risk activities, construction" },
        { L"Thành", L"Khai trương, nhập học, cưới hỏi, cầu tài lộc",   L"Kiện tụng, tranh cãi",
                    L"Grand openings, study, weddings, investments",   L"Litigation, arguments" },
        { L"Thu",   L"Thu hoạch, tích trữ, đòi nợ, cất giữ tài sản",   L"Khởi công mới, xuất hành xa",
                    L"Harvesting, saving, collecting debts",           L"Starting new projects, long trips" },
        { L"Khai",  L"Khai trương, động thổ, xuất hành, nhậm chức",    L"An táng, tranh chấp",
                    L"Grand openings, groundbreaking, travel",         L"Burials, disputes" },
        { L"Bế",    L"Đắp đê, tu bổ, cất giữ tài vật, lập kế hoạch",   L"Khai trương, xuất hành xa",
                    L"Maintenance, saving assets, planning",           L"Grand openings, long journeys" },
    };
    int trucIdx = (chiDayIdx - chiMonthIdx + 12) % 12;
    info.trucName = kTruc[trucIdx].name;
    info.goodFor = PickText(vi, kTruc[trucIdx].goodVi, kTruc[trucIdx].goodEn);
    info.badFor = PickText(vi, kTruc[trucIdx].badVi, kTruc[trucIdx].badEn);

    static const PCWSTR kGioHoangDao[6] = {
        L"Tý (23-1h), Sửu (1-3h), Mão (5-7h), Ngọ (11-13h), Thân (15-17h), Dậu (17-19h)",
        L"Dần (3-5h), Mão (5-7h), Tỵ (9-11h), Thân (15-17h), Tuất (19-21h), Hợi (21-23h)",
        L"Tý (23-1h), Sửu (1-3h), Thìn (7-9h), Tỵ (9-11h), Mùi (13-15h), Tuất (19-21h)",
        L"Tý (23-1h), Dần (3-5h), Mão (5-7h), Ngọ (11-13h), Mùi (13-15h), Dậu (17-19h)",
        L"Dần (3-5h), Thìn (7-9h), Tỵ (9-11h), Thân (15-17h), Dậu (17-19h), Hợi (21-23h)",
        L"Sửu (1-3h), Thìn (7-9h), Ngọ (11-13h), Mùi (13-15h), Tuất (19-21h), Hợi (21-23h)"
    };
    info.gioHoangDao = kGioHoangDao[chiDayIdx % 6];

    int xungChiIdx = (chiDayIdx + 6) % 12;
    int xungCan1 = (canDayIdx + 4) % 10;
    int xungCan2 = (canDayIdx + 6) % 10;
    info.tuoiXung = std::wstring(CAN_10[xungCan1]) + L" " + CHI_12[xungChiIdx] +
                    L", " + CAN_10[xungCan2] + L" " + CHI_12[xungChiIdx];

    static const BilingualText kHuongHyThan[10] = {
        { L"Đông Bắc", L"Northeast" }, { L"Tây Bắc",  L"Northwest" },
        { L"Tây Nam",  L"Southwest" }, { L"Chính Nam", L"South" },
        { L"Đông Nam", L"Southeast" }, { L"Đông Bắc", L"Northeast" },
        { L"Tây Bắc",  L"Northwest" }, { L"Tây Nam",  L"Southwest" },
        { L"Chính Nam", L"South" },    { L"Đông Nam", L"Southeast" },
    };
    static const BilingualText kHuongTaiThan[10] = {
        { L"Đông Nam",  L"Southeast" }, { L"Đông Nam",  L"Southeast" },
        { L"Chính Đông", L"East" },     { L"Chính Đông", L"East" },
        { L"Chính Bắc", L"North" },     { L"Chính Nam", L"South" },
        { L"Tây Nam",   L"Southwest" }, { L"Tây Nam",   L"Southwest" },
        { L"Chính Tây", L"West" },      { L"Tây Bắc",   L"Northwest" },
    };
    info.huongHyThan = PickText(vi, kHuongHyThan[canDayIdx].vi, kHuongHyThan[canDayIdx].en);
    info.huongTaiThan = PickText(vi, kHuongTaiThan[canDayIdx].vi, kHuongTaiThan[canDayIdx].en);

    info.specialHoliday = EvaluateVietnameseHoliday(
        sDay, sMonth, info.lDay, info.lMonth, info.isLeap, vi).names;

    return info;
}

std::wstring FormatTooltipHoroscope(ModSettings const& settings) {
    bool vi = UseVietnameseLabels(settings);
    SYSTEMTIME st{};
    GetLocalTime(&st);
    ComputedHoroscope h = ComputeHoroscopeForDate(st.wYear, st.wMonth, st.wDay, vi, st.wHour);

    WCHAR lunarHeader[256]{};
    if (vi) {
        if (h.isLeap) {
            _snwprintf_s(lunarHeader, _TRUNCATE,
                         L"📅 Âm lịch: Ngày %02d Tháng %02d (Nhuận) Năm %s",
                         h.lDay, h.lMonth, h.yearCanChi.c_str());
        } else {
            _snwprintf_s(lunarHeader, _TRUNCATE,
                         L"📅 Âm lịch: Ngày %02d Tháng %02d Năm %s (%02d/%02d/%d)",
                         h.lDay, h.lMonth, h.yearCanChi.c_str(), h.lDay, h.lMonth, h.lYear);
        }
    } else {
        if (h.isLeap) {
            _snwprintf_s(lunarHeader, _TRUNCATE,
                         L"📅 Lunar Date: %02d/%02d/%d (Leap Month, Year of %s)",
                         h.lDay, h.lMonth, h.lYear, h.yearCanChi.c_str());
        } else {
            _snwprintf_s(lunarHeader, _TRUNCATE,
                         L"📅 Lunar Date: %02d/%02d/%d (Year of %s)",
                         h.lDay, h.lMonth, h.lYear, h.yearCanChi.c_str());
        }
    }

    std::wstring holidayLine;
    if (!h.specialHoliday.empty()) {
        holidayLine = std::wstring(vi ? L"\r\n🎉 Sự kiện: " : L"\r\n🎉 Event: ") + h.specialHoliday;
    }

    WCHAR buffer[2048]{};
    if (vi) {
        _snwprintf_s(
            buffer, _TRUNCATE,
            L"%s%s"
            L"\r\n🐉 Bát tự: Giờ %s • Ngày %s • Tháng %s • Năm %s"
            L"\r\n☯ Mệnh ngày: %s"
            L"\r\n%s %s (%s) • Trực: %s"
            L"\r\n⏰ Giờ Hoàng Đạo: %s"
            L"\r\n🔮 Tử vi (Nên): %s"
            L"\r\n⛔ Việc Kỵ: %s"
            L"\r\n🧭 Tuổi xung: %s • Xuất hành: %s (Hỷ), %s (Tài)",
            lunarHeader,
            holidayLine.c_str(),
            h.hourCanChi.c_str(),
            h.dayCanChi.c_str(),
            h.monthCanChi.c_str(),
            h.yearCanChi.c_str(),
            h.napAmStr,
            h.isHoangDao ? L"🌟" : L"⚪",
            h.starName,
            h.isHoangDao ? L"Ngày Tốt" : L"Ngày Hắc Đạo",
            h.trucName,
            h.gioHoangDao,
            h.goodFor,
            h.badFor,
            h.tuoiXung.c_str(),
            h.huongHyThan,
            h.huongTaiThan);
    } else {
        _snwprintf_s(
            buffer, _TRUNCATE,
            L"%s%s"
            L"\r\n🐉 Four Pillars: Hour %s • Day %s • Month %s • Year %s"
            L"\r\n☯ Element: %s"
            L"\r\n%s %s (%s) • Officer (Trực): %s"
            L"\r\n⏰ Auspicious Hours: %s"
            L"\r\n🔮 Good for: %s"
            L"\r\n⛔ Avoid: %s"
            L"\r\n🧭 Clash: %s • Directions: %s (Joy), %s (Wealth)",
            lunarHeader,
            holidayLine.c_str(),
            h.hourCanChi.c_str(),
            h.dayCanChi.c_str(),
            h.monthCanChi.c_str(),
            h.yearCanChi.c_str(),
            h.napAmStr,
            h.isHoangDao ? L"🌟" : L"⚪",
            h.starName,
            h.isHoangDao ? L"Auspicious" : L"Inauspicious",
            h.trucName,
            h.gioHoangDao,
            h.goodFor,
            h.badFor,
            h.tuoiXung.c_str(),
            h.huongHyThan,
            h.huongTaiThan);
    }

    return buffer;
}

struct HoroscopePageData {
    std::wstring headerSolar;
    std::wstring headerLunar;
    std::wstring lineCanChi;
    std::wstring lineMenhTruc;
    std::wstring lineGioHoangDao;
    std::wstring lineTuViNen;
    std::wstring lineTuViKy;
    std::wstring lineXungHuong;
};

HoroscopePageData BuildHoroscopePageData(int32_t key, bool vi) {
    HoroscopePageData out;
    int sYear = key / 10000;
    int sMonth = (key / 100) % 100;
    int sDay = key % 100;

    ComputedHoroscope h = ComputeHoroscopeForDate(sYear, sMonth, sDay, vi);

    WCHAR buf[512]{};
    _snwprintf_s(buf, _TRUNCATE, L"%s, %02d/%02d/%d", h.weekdayStr, sDay, sMonth, sYear);
    out.headerSolar = buf;

    if (vi) {
        if (h.isLeap) {
            _snwprintf_s(buf, _TRUNCATE, L"Âm lịch: %02d/%02d (Nhuận) Năm %s",
                         h.lDay, h.lMonth, h.yearCanChi.c_str());
        } else {
            _snwprintf_s(buf, _TRUNCATE, L"Âm lịch: %02d/%02d/%d (%s)",
                         h.lDay, h.lMonth, h.lYear, h.yearCanChi.c_str());
        }
    } else {
        if (h.isLeap) {
            _snwprintf_s(buf, _TRUNCATE, L"Lunar: %02d/%02d/%d (Leap, Year of %s)",
                         h.lDay, h.lMonth, h.lYear, h.yearCanChi.c_str());
        } else {
            _snwprintf_s(buf, _TRUNCATE, L"Lunar: %02d/%02d/%d (Year of %s)",
                         h.lDay, h.lMonth, h.lYear, h.yearCanChi.c_str());
        }
    }
    out.headerLunar = buf;
    if (!h.specialHoliday.empty()) {
        out.headerLunar += L"  •  🎉 " + h.specialHoliday;
    }

    if (vi) {
        _snwprintf_s(buf, _TRUNCATE, L"🐉 Ngày %s  •  Tháng %s  •  Năm %s",
                     h.dayCanChi.c_str(), h.monthCanChi.c_str(), h.yearCanChi.c_str());
        out.lineCanChi = buf;

        _snwprintf_s(buf, _TRUNCATE, L"☯ Mệnh: %s\n%s %s (%s)  •  Trực: %s",
                     h.napAmStr,
                     h.isHoangDao ? L"🌟" : L"⚪",
                     h.starName,
                     h.isHoangDao ? L"Ngày Tốt" : L"Hắc Đạo",
                     h.trucName);
        out.lineMenhTruc = buf;

        _snwprintf_s(buf, _TRUNCATE, L"⏰ Giờ Hoàng Đạo: %s", h.gioHoangDao);
        out.lineGioHoangDao = buf;

        _snwprintf_s(buf, _TRUNCATE, L"🔮 Tử vi (Nên): %s", h.goodFor);
        out.lineTuViNen = buf;

        _snwprintf_s(buf, _TRUNCATE, L"⛔ Việc Kỵ: %s", h.badFor);
        out.lineTuViKy = buf;

        _snwprintf_s(buf, _TRUNCATE, L"🧭 Tuổi xung: %s  •  Hỷ thần: %s, Tài thần: %s",
                     h.tuoiXung.c_str(), h.huongHyThan, h.huongTaiThan);
        out.lineXungHuong = buf;
    } else {
        _snwprintf_s(buf, _TRUNCATE, L"🐉 Day %s  •  Month %s  •  Year %s",
                     h.dayCanChi.c_str(), h.monthCanChi.c_str(), h.yearCanChi.c_str());
        out.lineCanChi = buf;

        _snwprintf_s(buf, _TRUNCATE, L"☯ Element: %s\n%s %s (%s)  •  Officer (Trực): %s",
                     h.napAmStr,
                     h.isHoangDao ? L"🌟" : L"⚪",
                     h.starName,
                     h.isHoangDao ? L"Auspicious" : L"Inauspicious",
                     h.trucName);
        out.lineMenhTruc = buf;

        _snwprintf_s(buf, _TRUNCATE, L"⏰ Auspicious Hours: %s", h.gioHoangDao);
        out.lineGioHoangDao = buf;

        _snwprintf_s(buf, _TRUNCATE, L"🔮 Good for: %s", h.goodFor);
        out.lineTuViNen = buf;

        _snwprintf_s(buf, _TRUNCATE, L"⛔ Avoid: %s", h.badFor);
        out.lineTuViKy = buf;

        _snwprintf_s(buf, _TRUNCATE, L"🧭 Clash: %s  •  Directions: %s (Joy), %s (Wealth)",
                     h.tuoiXung.c_str(), h.huongHyThan, h.huongTaiThan);
        out.lineXungHuong = buf;
    }

    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Calendar Flyout XAML Manipulation

constexpr PCWSTR kLunarTextBlockName = L"LunarTextBlock";
constexpr PCWSTR kCustomCellLunarName = L"CustomCellLunar";
constexpr PCWSTR kCustomHeaderLunarName = L"CustomHeaderLunar";
constexpr PCWSTR kCustomHoroscopePageName = L"CustomHoroscopePageOverlay";

struct FoundElement {
    wux::DependencyObject element{nullptr};
    int depth = 0;
};

void LogCalendarException(PCWSTR operation) noexcept {
    try {
        throw;
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"%s: HRESULT=0x%08X, %s", operation,
               static_cast<unsigned int>(error.code().value), error.message().c_str());
    } catch (...) {
        Wh_Log(L"%s: unknown exception", operation);
    }
}

void CollectDescendants(wux::DependencyObject const& root, std::vector<FoundElement>& out) {
    out.push_back({root, 0});
    for (size_t i = 0; i < out.size() && i < 256; i++) {
        wux::DependencyObject current = out[i].element;
        int depth = out[i].depth;
        if (!current || depth > 24) continue;
        int count = 0;
        try { count = wuxm::VisualTreeHelper::GetChildrenCount(current); } catch (...) { continue; }
        for (int childIndex = 0; childIndex < count; childIndex++) {
            wux::DependencyObject child = nullptr;
            try { child = wuxm::VisualTreeHelper::GetChild(current, childIndex); } catch (...) { continue; }
            if (!child) continue;
            out.push_back({child, depth + 1});
        }
    }
}

std::wstring ElementName(wux::DependencyObject const& element) {
    if (auto frameworkElement = element.try_as<wux::FrameworkElement>()) {
        try { return frameworkElement.Name().c_str(); } catch (...) {}
    }
    return {};
}

std::wstring ElementText(wuxc::TextBlock const& textBlock) {
    try { return TrimWhitespace(textBlock.Text().c_str()); } catch (...) { return {}; }
}

bool IsNumberText(std::wstring const& text) {
    if (text.empty()) return false;
    for (wchar_t c : text) {
        if (c < L'0' || c > L'9') return false;
    }
    return true;
}

thread_local bool t_applyWrote = false;
thread_local int32_t t_selectedHoroscopeKey = 0;
thread_local winrt::weak_ref<wuxc::CalendarView> t_calendarView{nullptr};
thread_local winrt::event_token t_selectedDatesChangedToken{};
thread_local wuxc::CalendarViewSelectionMode t_savedSelectionMode =
    wuxc::CalendarViewSelectionMode::None;
thread_local bool t_hasSavedSelectionMode = false;

struct HeaderState {
    winrt::weak_ref<wuxc::Panel> headerPanel{nullptr};
    winrt::weak_ref<wuxc::TextBlock> headerLabel{nullptr};
    bool createdByMod = false;
    bool hasSavedState = false;
    std::wstring savedText;
    wux::Visibility savedVisibility = wux::Visibility::Visible;
};
thread_local HeaderState t_headerState;

struct HiddenCalendarChild {
    winrt::weak_ref<wux::UIElement> element{nullptr};
    double opacity = 1.0;
    bool hitTestVisible = true;
};

struct HoroscopePageControls {
    winrt::weak_ref<wuxc::Grid> hostGrid{nullptr};
    winrt::weak_ref<wuxc::Border> pageBorder{nullptr};
    winrt::weak_ref<wuxc::Button> backButton{nullptr};
    winrt::event_token backClickToken{};
    std::vector<HiddenCalendarChild> hiddenChildren;
    int32_t renderedKey = 0;
    winrt::weak_ref<wuxc::TextBlock> titleSolar{nullptr};
    winrt::weak_ref<wuxc::TextBlock> titleLunar{nullptr};
    winrt::weak_ref<wuxc::TextBlock> rowCanChi{nullptr};
    winrt::weak_ref<wuxc::TextBlock> rowMenhTruc{nullptr};
    winrt::weak_ref<wuxc::TextBlock> rowGioHoangDao{nullptr};
    winrt::weak_ref<wuxc::TextBlock> rowTuViNen{nullptr};
    winrt::weak_ref<wuxc::TextBlock> rowTuViKy{nullptr};
    winrt::weak_ref<wuxc::TextBlock> rowXungHuong{nullptr};
};
thread_local HoroscopePageControls t_horoscopePage;

void SweepDayItems();
void StartSweepTimer();
void UpdateHoroscopePageOnCurrentThread();
void OpenHoroscopeForDateKey(int32_t key);

void EnsureDualLineSpacing(wuxc::CalendarViewDayItem const& item,
                           wuxc::TextBlock const& solarOut,
                           wuxc::TextBlock const& lunarOut,
                           bool forceTransform) {
    if (!solarOut || !lunarOut) return;
    try {
        solarOut.ClearValue(wux::FrameworkElement::MarginProperty());
        lunarOut.ClearValue(wux::FrameworkElement::MarginProperty());

        double currentSolarShift = 0.0;
        double currentLunarShift = 0.0;
        if (auto st = solarOut.RenderTransform().try_as<wuxm::TranslateTransform>()) {
            currentSolarShift = st.Y();
        }
        if (auto lt = lunarOut.RenderTransform().try_as<wuxm::TranslateTransform>()) {
            currentLunarShift = lt.Y();
        }

        bool needShift = forceTransform;
        double targetSolarShift = -5.5;
        double targetLunarShift = 3.5;

        try {
            wf::Point ptSolar = solarOut.TransformToVisual(item).TransformPoint({0, 0});
            wf::Point ptLunar = lunarOut.TransformToVisual(item).TransformPoint({0, 0});
            double baseSolarY = ptSolar.Y - currentSolarShift;
            double baseLunarY = ptLunar.Y - currentLunarShift;
            double baseDeltaY = baseLunarY - baseSolarY;

            if (baseDeltaY > 0.1 && baseDeltaY < 16.5) {
                needShift = true;
                double extra = 18.0 - baseDeltaY;
                targetSolarShift = -extra * 0.65;
                targetLunarShift = extra * 0.35;
            } else if (baseDeltaY >= 16.5) {
                needShift = false;
            }
        } catch (...) {}

        if (needShift) {
            if (std::abs(currentSolarShift - targetSolarShift) > 0.2) {
                wuxm::TranslateTransform solarTransform;
                solarTransform.Y(targetSolarShift);
                solarOut.RenderTransform(solarTransform);
            }
            if (std::abs(currentLunarShift - targetLunarShift) > 0.2) {
                wuxm::TranslateTransform lunarTransform;
                lunarTransform.Y(targetLunarShift);
                lunarOut.RenderTransform(lunarTransform);
            }
        }
    } catch (...) {}
}

struct ColorState {
    wuxm::Brush savedForeground{nullptr};
    bool hasSavedForeground = false;
    bool savedForegroundWasLocal = false;
    wuxm::Brush ownBrush{nullptr};
    bool hadColor = false;
};

struct DayCell {
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
    winrt::weak_ref<wuxc::TextBlock> lunarLabel{nullptr};
    winrt::weak_ref<wuxc::TextBlock> solarLabel{nullptr};
    winrt::weak_ref<wuxc::Panel> createdParentPanel{nullptr};
    bool createdByMod = false;

    int64_t lunarLabelChangedToken = 0;
    std::wstring savedText;
    bool hasSavedText = false;
    wux::Visibility savedVisibility = wux::Visibility::Visible;
    bool hasSavedVisibility = false;
    std::wstring appliedText;

    ColorState lunarColor;
    ColorState solarColor;
    ColorState itemColor;
    wuxm::Brush normalForeground{nullptr};

    int32_t key = 0;
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

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item);

void* CellId(wuxc::CalendarViewDayItem const& item) {
    return winrt::get_abi(item);
}

DayCell& CellFor(wuxc::CalendarViewDayItem const& item) {
    std::unique_ptr<DayCell>& entry = t_cells[CellId(item)];
    if (entry && entry->item.get() != item) entry = nullptr;
    if (!entry) {
        entry = std::make_unique<DayCell>();
        entry->item = winrt::make_weak(item);
    }
    return *entry;
}

void TrackDayItem(wuxc::CalendarViewDayItem const& item) {
    void* id = winrt::get_abi(item);
    for (auto it = t_dayItems.begin(); it != t_dayItems.end(); ++it) {
        if (it->id != id) continue;
        if (it->item.get()) {
            TrackedDayItem tracked = *it;
            t_dayItems.erase(it);
            t_dayItems.push_back(tracked);
        } else {
            it->item = winrt::make_weak(item);
        }
        return;
    }
    if (t_dayItems.size() >= kMaxTrackedDayItems) {
        t_dayItems.erase(t_dayItems.begin());
    }
    t_dayItems.push_back({id, winrt::make_weak(item)});
}

void FindOrCreateLunarTextBlock(wuxc::CalendarViewDayItem const& item,
                                DayCell& cell,
                                wuxc::TextBlock& solarOut,
                                wuxc::TextBlock& lunarOut) {
    solarOut = nullptr;
    lunarOut = nullptr;
    wuxc::Panel containerPanel = nullptr;

    std::vector<FoundElement> descendants;
    CollectDescendants(item, descendants);

    for (const auto& found : descendants) {
        if (!containerPanel) {
            if (auto panel = found.element.try_as<wuxc::Panel>()) {
                containerPanel = panel;
            }
        }

        auto tb = found.element.try_as<wuxc::TextBlock>();
        if (!tb) continue;

        std::wstring name = ElementName(tb);
        std::wstring text = ElementText(tb);

        if (name == kLunarTextBlockName || name == kCustomCellLunarName) {
            lunarOut = tb;
            continue;
        }

        if (IsNumberText(text)) {
            if (!solarOut) solarOut = tb;
            continue;
        }

        if (!lunarOut && !text.empty()) {
            lunarOut = tb;
        }
    }

    if (lunarOut) {
        if (!cell.createdByMod && ElementName(lunarOut) != kCustomCellLunarName) {
            if (!cell.hasSavedVisibility) {
                try {
                    cell.savedVisibility = lunarOut.Visibility();
                    cell.hasSavedVisibility = true;
                } catch (...) {}
            }
            if (!cell.hasSavedText) {
                try {
                    cell.savedText = lunarOut.Text().c_str();
                    cell.hasSavedText = true;
                } catch (...) {}
            }
        }
        try {
            if (lunarOut.Visibility() != wux::Visibility::Visible) {
                lunarOut.Visibility(wux::Visibility::Visible);
                t_applyWrote = true;
            }
        } catch (...) {}
        return;
    }

    if (!lunarOut && solarOut) {
        try {
            wuxc::Panel parentPanel = nullptr;
            if (auto parent = wuxm::VisualTreeHelper::GetParent(solarOut)) {
                parentPanel = parent.try_as<wuxc::Panel>();
            }
            if (!parentPanel) parentPanel = containerPanel;

            if (parentPanel) {
                wuxc::TextBlock newLunar;
                newLunar.Name(kCustomCellLunarName);
                newLunar.FontSize(10.0);
                newLunar.HorizontalAlignment(wux::HorizontalAlignment::Center);
                newLunar.VerticalAlignment(wux::VerticalAlignment::Bottom);
                newLunar.Opacity(0.8);

                parentPanel.Children().Append(newLunar);
                lunarOut = newLunar;
                cell.createdByMod = true;
                cell.createdParentPanel = winrt::make_weak(parentPanel);
                t_applyWrote = true;
            }
        } catch (...) {}
    }
}

bool SameBrush(wuxm::Brush const& left, wuxm::Brush const& right) {
    return left && right && winrt::get_abi(left) == winrt::get_abi(right);
}

bool IsOwnBrush(wuxc::TextBlock const& textBlock, wuxm::Brush const& ownBrush) {
    if (!ownBrush) return false;
    try { return SameBrush(textBlock.Foreground(), ownBrush); } catch (...) { return false; }
}

bool IsOwnControlBrush(wuxc::Control const& ctrl, wuxm::Brush const& ownBrush) {
    if (!ownBrush) return false;
    try { return SameBrush(ctrl.Foreground(), ownBrush); } catch (...) { return false; }
}

wuxm::Brush SetForegroundIfDifferent(wuxc::TextBlock const& textBlock,
                                     winrt::Windows::UI::Color const& color,
                                     wuxm::Brush const& previousOwn) {
    try {
        if (auto current = textBlock.Foreground().try_as<wuxm::SolidColorBrush>()) {
            if (current.Color() == color) return SameBrush(current, previousOwn) ? previousOwn : nullptr;
        }
        wuxm::SolidColorBrush brush(color);
        textBlock.Foreground(brush);
        t_applyWrote = true;
        return brush;
    } catch (...) { return nullptr; }
}

wuxm::Brush SetControlForegroundIfDifferent(wuxc::Control const& ctrl,
                                            winrt::Windows::UI::Color const& color,
                                            wuxm::Brush const& previousOwn) {
    try {
        if (auto current = ctrl.Foreground().try_as<wuxm::SolidColorBrush>()) {
            if (current.Color() == color) return SameBrush(current, previousOwn) ? previousOwn : nullptr;
        }
        wuxm::SolidColorBrush brush(color);
        ctrl.Foreground(brush);
        t_applyWrote = true;
        return brush;
    } catch (...) { return nullptr; }
}

void SaveForeground(ColorState& state, wuxc::TextBlock const& label) {
    if (state.hasSavedForeground) return;
    try {
        if (IsOwnBrush(label, state.ownBrush)) return;
        wf::IInspectable value = label.ReadLocalValue(wuxc::TextBlock::ForegroundProperty());
        if (auto brush = value.try_as<wuxm::Brush>()) {
            state.savedForeground = brush;
            state.savedForegroundWasLocal = true;
        }
        state.hasSavedForeground = true;
    } catch (...) {}
}

void SaveControlForeground(ColorState& state, wuxc::Control const& ctrl) {
    if (state.hasSavedForeground) return;
    try {
        if (IsOwnControlBrush(ctrl, state.ownBrush)) return;
        wf::IInspectable value = ctrl.ReadLocalValue(wuxc::Control::ForegroundProperty());
        if (auto brush = value.try_as<wuxm::Brush>()) {
            state.savedForeground = brush;
            state.savedForegroundWasLocal = true;
        }
        state.hasSavedForeground = true;
    } catch (...) {}
}

void RestoreForeground(wuxc::TextBlock const& label, ColorState& state) {
    try {
        if (!IsOwnBrush(label, state.ownBrush)) return;
        if (state.savedForegroundWasLocal && state.savedForeground) label.Foreground(state.savedForeground);
        else label.ClearValue(wuxc::TextBlock::ForegroundProperty());
        t_applyWrote = true;
    } catch (...) {}
    state.ownBrush = nullptr;
    state.hasSavedForeground = false;
}

void RestoreControlForeground(wuxc::Control const& ctrl, ColorState& state) {
    try {
        if (!IsOwnControlBrush(ctrl, state.ownBrush)) return;
        if (state.savedForegroundWasLocal && state.savedForeground) ctrl.Foreground(state.savedForeground);
        else ctrl.ClearValue(wuxc::Control::ForegroundProperty());
        t_applyWrote = true;
    } catch (...) {}
    state.ownBrush = nullptr;
    state.hasSavedForeground = false;
}

void OnLabelTextChanged(wux::DependencyObject const& sender) {
    auto it = t_labelOwners.find(winrt::get_abi(sender));
    if (it == t_labelOwners.end()) return;
    if (auto item = it->second.get()) {
        ApplyToDayItem(item);
        StartSweepTimer();
    }
}

void OpenHoroscopeForDateKey(int32_t key) {
    ModSettings settings = GetSettings();
    if (!settings.enableCalendarFlyout || !settings.enableHoroscopePage) return;
    if (g_shuttingDown || !IsValidDate(key / 10000, (key / 100) % 100, key % 100)) return;
    if (key == t_selectedHoroscopeKey && t_horoscopePage.pageBorder.get() &&
        t_horoscopePage.renderedKey == key) return;

    t_selectedHoroscopeKey = key;
    UpdateHoroscopePageOnCurrentThread();
    StartSweepTimer();
}

void EnsureCalendarViewSelectionHooked(wuxc::CalendarView const& calView) {
    if (!calView) return;
    ModSettings settings = GetSettings();
    if (!settings.enableHoroscopePage) {
        if (t_selectedDatesChangedToken.value != 0) {
            try { calView.SelectedDatesChanged(t_selectedDatesChangedToken); } catch (...) {}
            t_selectedDatesChangedToken = {};
        }
        if (t_hasSavedSelectionMode) {
            try { calView.SelectionMode(t_savedSelectionMode); } catch (...) {}
            t_hasSavedSelectionMode = false;
        }
        return;
    }

    if (t_calendarView.get() == calView && t_selectedDatesChangedToken.value != 0) return;

    if (auto prev = t_calendarView.get()) {
        if (t_selectedDatesChangedToken.value != 0) {
            try { prev.SelectedDatesChanged(t_selectedDatesChangedToken); } catch (...) {}
        }
    }
    t_selectedDatesChangedToken = {};
    t_calendarView = winrt::make_weak(calView);

    try {
        if (!t_hasSavedSelectionMode) {
            t_savedSelectionMode = calView.SelectionMode();
            t_hasSavedSelectionMode = true;
        }
        if (calView.SelectionMode() == wuxc::CalendarViewSelectionMode::None) {
            calView.SelectionMode(wuxc::CalendarViewSelectionMode::Single);
        }
        t_selectedDatesChangedToken = calView.SelectedDatesChanged(
            [](wuxc::CalendarView const&, wuxc::CalendarViewSelectedDatesChangedEventArgs const& args) {
                try {
                    wf::DateTime dt{};
                    bool hasDate = false;
                    if (args.AddedDates().Size() > 0) {
                        dt = args.AddedDates().GetAt(0);
                        hasDate = true;
                    } else if (args.RemovedDates().Size() > 0) {
                        dt = args.RemovedDates().GetAt(0);
                        hasDate = true;
                    }
                    if (hasDate) {
                        int32_t key = DateKeyFromDateTime(dt);
                        OpenHoroscopeForDateKey(key);
                    }
                } catch (...) { LogCalendarException(L"SelectedDatesChanged"); }
            });
    } catch (...) { LogCalendarException(L"Register SelectedDatesChanged"); }
}

bool IsOwnLunarCellText(std::wstring const& text) {
    return !text.empty() && text.find(L'/') != std::wstring::npos;
}

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item) {
    ModSettings settings = GetSettings();
    if (!settings.enableCalendarFlyout || !item) return;

    int32_t key = DateKeyFromDateTime(item.Date());
    if (!key) return;

    DayCell& cell = CellFor(item);
    if (cell.busy) return;
    cell.busy = true;
    struct BusyGuard { bool& busy; ~BusyGuard() { busy = false; } } guard{cell.busy};

    if (cell.key != key) {
        cell.key = key;
        cell.appliedText.clear();
        if (!cell.createdByMod) {
            cell.savedText.clear();
            cell.hasSavedText = false;
        }
    }

    if (cell.dateChangedToken == 0) {
        try {
            cell.dateChangedToken = item.RegisterPropertyChangedCallback(
                wuxc::CalendarViewDayItem::DateProperty(),
                [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                    if (auto dayItem = sender.try_as<wuxc::CalendarViewDayItem>()) {
                        ApplyToDayItem(dayItem);
                        StartSweepTimer();
                    }
                });
        } catch (...) {}
    }

    wuxc::TextBlock solarLabel = nullptr;
    wuxc::TextBlock lunarLabel = nullptr;
    FindOrCreateLunarTextBlock(item, cell, solarLabel, lunarLabel);

    if (!lunarLabel) return;

    if (cell.lunarLabel.get() != lunarLabel) {
        if (auto previous = cell.lunarLabel.get()) {
            if (cell.lunarLabelChangedToken) {
                try { previous.UnregisterPropertyChangedCallback(wuxc::TextBlock::TextProperty(), cell.lunarLabelChangedToken); } catch (...) {}
            }
            t_labelOwners.erase(winrt::get_abi(previous));
        }
        cell.lunarLabelChangedToken = 0;
        cell.lunarLabel = winrt::make_weak(lunarLabel);
        if (solarLabel) cell.solarLabel = winrt::make_weak(solarLabel);

        cell.appliedText.clear();
        cell.lunarColor = ColorState();
        cell.solarColor = ColorState();
        cell.normalForeground = nullptr;
        t_labelOwners[winrt::get_abi(lunarLabel)] = winrt::make_weak(item);

        try {
            cell.lunarLabelChangedToken = lunarLabel.RegisterPropertyChangedCallback(
                wuxc::TextBlock::TextProperty(),
                [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                    OnLabelTextChanged(sender);
                });
        } catch (...) {}
    }

    int year = key / 10000;
    int month = (key / 100) % 100;
    int day = key % 100;

    int lDay, lMonth, lYear;
    bool isLeap;
    std::wstring canChi;
    GetVietnameseLunarDate(year, month, day, lDay, lMonth, lYear, isLeap, canChi);

    bool vi = UseVietnameseLabels(settings);
    WCHAR buffer[64]{};
    if (isLeap) {
        _snwprintf_s(buffer, _TRUNCATE, L"%d/%d (%s)",
                     lDay, lMonth, PickText(vi, L"Nhuận", L"Leap"));
    } else {
        _snwprintf_s(buffer, _TRUNCATE, L"%d/%d", lDay, lMonth);
    }
    std::wstring desired = buffer;

    HolidayScope scope = EvaluateVietnameseHoliday(day, month, lDay, lMonth, isLeap, vi).scope;
    bool colorSolar = (scope == HolidayScope::Both || scope == HolidayScope::SolarOnly);
    bool colorLunar = (scope == HolidayScope::Both || scope == HolidayScope::LunarOnly);

    std::wstring current = ElementText(lunarLabel);
    if (!desired.empty() && current != desired) {
        if (!cell.createdByMod && current != cell.appliedText && !IsOwnLunarCellText(current)) {
            cell.savedText = current;
            cell.hasSavedText = true;
        }
        try {
            lunarLabel.Text(desired);
            t_applyWrote = true;
        } catch (...) {}
    }
    cell.appliedText = desired;

    EnsureDualLineSpacing(item, solarLabel, lunarLabel, cell.createdByMod || cell.savedText.empty());

    winrt::Windows::UI::Color redColor{255, 255, 90, 90};

    if (!cell.normalForeground && !cell.lunarColor.hadColor) {
        try { cell.normalForeground = lunarLabel.Foreground(); } catch (...) {}
    }

    if (colorSolar) {
        SaveControlForeground(cell.itemColor, item);
        cell.itemColor.ownBrush = SetControlForegroundIfDifferent(item, redColor, cell.itemColor.ownBrush);
        cell.itemColor.hadColor = true;

        if (solarLabel) {
            SaveForeground(cell.solarColor, solarLabel);
            cell.solarColor.ownBrush = SetForegroundIfDifferent(solarLabel, redColor, cell.solarColor.ownBrush);
            cell.solarColor.hadColor = true;
        }
    } else {
        if (cell.itemColor.hadColor) {
            RestoreControlForeground(item, cell.itemColor);
            cell.itemColor.hadColor = false;
        }
        if (solarLabel && cell.solarColor.hadColor) {
            RestoreForeground(solarLabel, cell.solarColor);
            cell.solarColor.hadColor = false;
        }
    }

    if (colorLunar) {
        SaveForeground(cell.lunarColor, lunarLabel);
        cell.lunarColor.ownBrush = SetForegroundIfDifferent(lunarLabel, redColor, cell.lunarColor.ownBrush);
        cell.lunarColor.hadColor = true;
    } else if (colorSolar) {
        SaveForeground(cell.lunarColor, lunarLabel);
        winrt::Windows::UI::Color normalColor{255, 180, 180, 180};
        if (cell.normalForeground) {
            if (auto scb = cell.normalForeground.try_as<wuxm::SolidColorBrush>()) {
                if (scb.Color() != redColor) normalColor = scb.Color();
            }
        }
        cell.lunarColor.ownBrush = SetForegroundIfDifferent(lunarLabel, normalColor, cell.lunarColor.ownBrush);
        cell.lunarColor.hadColor = true;
    } else if (cell.lunarColor.hadColor) {
        RestoreForeground(lunarLabel, cell.lunarColor);
        cell.lunarColor.hadColor = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Horoscope Detail Overlay Page

wuxc::Grid FindCalendarRootGrid() {
    if (auto existing = t_horoscopePage.hostGrid.get()) {
        return existing;
    }

    wuxc::CalendarView calView = t_calendarView.get();

    if (!calView) {
        for (const auto& tracked : t_dayItems) {
            auto item = tracked.item.get();
            if (!item) continue;

            wux::DependencyObject curr = item;
            for (int depth = 0; depth < 16 && curr; depth++) {
                if (auto cv = curr.try_as<wuxc::CalendarView>()) {
                    calView = cv;
                    break;
                }
                try { curr = wuxm::VisualTreeHelper::GetParent(curr); } catch (...) { break; }
            }
            if (calView) break;
        }
    }

    if (calView) {
        EnsureCalendarViewSelectionHooked(calView);

        std::vector<FoundElement> descendants;
        CollectDescendants(calView, descendants);
        for (const auto& found : descendants) {
            if (auto g = found.element.try_as<wuxc::Grid>()) {
                t_horoscopePage.hostGrid = winrt::make_weak(g);
                return g;
            }
        }
    }
    return nullptr;
}

wuxc::Border CreateLineSection(wuxc::TextBlock& textBlockOut,
                               wuxm::Brush const& textBrush,
                               winrt::Windows::UI::Color const& dividerColor) {
    wuxc::Border rowBorder;
    rowBorder.Padding({6, 6, 6, 7});
    rowBorder.BorderThickness({0, 0, 0, 1});
    rowBorder.BorderBrush(wuxm::SolidColorBrush(dividerColor));

    wuxc::TextBlock tb;
    tb.FontSize(12.5);
    tb.LineHeight(18.5);
    tb.TextWrapping(wux::TextWrapping::Wrap);
    if (textBrush) {
        try { tb.Foreground(textBrush); } catch (...) {}
    }

    rowBorder.Child(tb);
    textBlockOut = tb;
    return rowBorder;
}

void EnsureHoroscopePageCreated() {
    if (t_horoscopePage.pageBorder.get()) return;

    wuxc::Grid hostGrid = FindCalendarRootGrid();
    if (!hostGrid) return;

    static thread_local bool loggedCreateError = false;
    try {
        int count = wuxm::VisualTreeHelper::GetChildrenCount(hostGrid);
        for (int i = 0; i < count; i++) {
            auto child = wuxm::VisualTreeHelper::GetChild(hostGrid, i);
            if (ElementName(child) == kCustomHoroscopePageName) {
                if (auto oldBorder = child.try_as<wuxc::Border>()) {
                    uint32_t idx = 0;
                    if (hostGrid.Children().IndexOf(oldBorder, idx)) {
                        hostGrid.Children().RemoveAt(idx);
                    }
                }
                break;
            }
        }

        bool isLightTheme = false;
        if (auto fe = hostGrid.try_as<wux::FrameworkElement>()) {
            try {
                isLightTheme = (fe.ActualTheme() == wux::ElementTheme::Light);
            } catch (...) {}
        }

        wuxm::Brush fgBrush = wuxm::SolidColorBrush(
            isLightTheme ? winrt::Windows::UI::Color{255, 24, 24, 24}
                         : winrt::Windows::UI::Color{255, 245, 245, 245});
        if (auto hdr = t_headerState.headerLabel.get()) {
            try { fgBrush = hdr.Foreground(); } catch (...) {}
        }

        winrt::Windows::UI::Color pageBgColor =
            isLightTheme ? winrt::Windows::UI::Color{18, 0, 0, 0}
                         : winrt::Windows::UI::Color{22, 255, 255, 255};
        winrt::Windows::UI::Color dividerColor =
            isLightTheme ? winrt::Windows::UI::Color{38, 0, 0, 0}
                         : winrt::Windows::UI::Color{38, 255, 255, 255};
        winrt::Windows::UI::Color accentLunarColor =
            isLightTheme ? winrt::Windows::UI::Color{255, 196, 43, 28}
                         : winrt::Windows::UI::Color{255, 255, 125, 125};

        wuxc::Border pageBorder;
        pageBorder.Name(kCustomHoroscopePageName);
        pageBorder.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        pageBorder.VerticalAlignment(wux::VerticalAlignment::Stretch);
        pageBorder.Padding({14, 10, 14, 10});
        pageBorder.Background(wuxm::SolidColorBrush(pageBgColor));
        pageBorder.CornerRadius({8, 8, 8, 8});
        pageBorder.Visibility(wux::Visibility::Collapsed);

        wuxc::Grid::SetRow(pageBorder, 0);
        wuxc::Grid::SetRowSpan(pageBorder, 10);
        wuxc::Grid::SetColumn(pageBorder, 0);
        wuxc::Grid::SetColumnSpan(pageBorder, 10);
        wuxc::Canvas::SetZIndex(pageBorder, 999);

        wuxc::Grid pageLayout;
        wuxc::RowDefinition rowHeader;
        rowHeader.Height(wux::GridLengthHelper::Auto());
        wuxc::RowDefinition rowDivider;
        rowDivider.Height(wux::GridLengthHelper::Auto());
        wuxc::RowDefinition rowBody;
        rowBody.Height(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
        pageLayout.RowDefinitions().Append(rowHeader);
        pageLayout.RowDefinitions().Append(rowDivider);
        pageLayout.RowDefinitions().Append(rowBody);

        wuxc::Grid topBar;
        wuxc::ColumnDefinition colBack;
        colBack.Width(wux::GridLengthHelper::Auto());
        wuxc::ColumnDefinition colTitles;
        colTitles.Width(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
        topBar.ColumnDefinitions().Append(colBack);
        topBar.ColumnDefinitions().Append(colTitles);
        topBar.Margin({0, 0, 0, 8});

        wuxc::Button backBtn;
        backBtn.Content(winrt::box_value(L"←"));
        backBtn.FontSize(16.0);
        backBtn.Padding({12, 4, 12, 6});
        backBtn.Margin({0, 0, 12, 0});
        backBtn.VerticalAlignment(wux::VerticalAlignment::Center);
        backBtn.CornerRadius({6, 6, 6, 6});

        t_horoscopePage.backClickToken = backBtn.Click(
            [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                t_selectedHoroscopeKey = 0;
                UpdateHoroscopePageOnCurrentThread();
            });

        wuxc::Grid::SetColumn(backBtn, 0);
        topBar.Children().Append(backBtn);

        wuxc::StackPanel titleStack;
        titleStack.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock titleSolar;
        titleSolar.FontSize(14.5);
        titleSolar.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        titleSolar.TextWrapping(wux::TextWrapping::Wrap);
        try { titleSolar.Foreground(fgBrush); } catch (...) {}

        wuxc::TextBlock titleLunar;
        titleLunar.FontSize(12.5);
        titleLunar.Margin({0, 2, 0, 0});
        titleLunar.Foreground(wuxm::SolidColorBrush(accentLunarColor));
        titleLunar.TextWrapping(wux::TextWrapping::Wrap);

        titleStack.Children().Append(titleSolar);
        titleStack.Children().Append(titleLunar);
        wuxc::Grid::SetColumn(titleStack, 1);
        topBar.Children().Append(titleStack);

        wuxc::Grid::SetRow(topBar, 0);
        pageLayout.Children().Append(topBar);

        wuxc::Border headerLine;
        headerLine.Height(1.0);
        headerLine.Margin({0, 0, 0, 4});
        headerLine.Background(wuxm::SolidColorBrush(dividerColor));
        wuxc::Grid::SetRow(headerLine, 1);
        pageLayout.Children().Append(headerLine);

        wuxc::ScrollViewer scrollViewer;
        scrollViewer.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
        scrollViewer.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);

        wuxc::StackPanel listStack;
        wuxc::TextBlock tbCanChi{nullptr}, tbMenhTruc{nullptr}, tbGio{nullptr},
                        tbNen{nullptr}, tbKy{nullptr}, tbXung{nullptr};

        listStack.Children().Append(CreateLineSection(tbCanChi, fgBrush, dividerColor));
        listStack.Children().Append(CreateLineSection(tbMenhTruc, fgBrush, dividerColor));
        listStack.Children().Append(CreateLineSection(tbGio, fgBrush, dividerColor));
        listStack.Children().Append(CreateLineSection(tbNen, fgBrush, dividerColor));
        listStack.Children().Append(CreateLineSection(tbKy, fgBrush, dividerColor));

        wuxc::Border lastRow = CreateLineSection(tbXung, fgBrush, dividerColor);
        lastRow.BorderThickness({0, 0, 0, 0});
        listStack.Children().Append(lastRow);

        scrollViewer.Content(listStack);
        wuxc::Grid::SetRow(scrollViewer, 2);
        pageLayout.Children().Append(scrollViewer);

        pageBorder.Child(pageLayout);
        hostGrid.Children().Append(pageBorder);

        t_horoscopePage.pageBorder = winrt::make_weak(pageBorder);
        t_horoscopePage.backButton = winrt::make_weak(backBtn);
        t_horoscopePage.titleSolar = winrt::make_weak(titleSolar);
        t_horoscopePage.titleLunar = winrt::make_weak(titleLunar);
        t_horoscopePage.rowCanChi = winrt::make_weak(tbCanChi);
        t_horoscopePage.rowMenhTruc = winrt::make_weak(tbMenhTruc);
        t_horoscopePage.rowGioHoangDao = winrt::make_weak(tbGio);
        t_horoscopePage.rowTuViNen = winrt::make_weak(tbNen);
        t_horoscopePage.rowTuViKy = winrt::make_weak(tbKy);
        t_horoscopePage.rowXungHuong = winrt::make_weak(tbXung);
        t_horoscopePage.renderedKey = 0;
        loggedCreateError = false;
    } catch (...) {
        if (!loggedCreateError) {
            LogCalendarException(L"Create horoscope page");
            loggedCreateError = true;
        }
    }
}

void RestoreHoroscopeCalendarChildren() {
    for (const auto& saved : t_horoscopePage.hiddenChildren) {
        if (auto ui = saved.element.get()) {
            try {
                ui.Opacity(saved.opacity);
                ui.IsHitTestVisible(saved.hitTestVisible);
            } catch (...) { LogCalendarException(L"Restore calendar child"); }
        }
    }
    t_horoscopePage.hiddenChildren.clear();
}

void UpdateHoroscopePageOnCurrentThread() {
    ModSettings settings = GetSettings();
    if (!settings.enableCalendarFlyout || !settings.enableHoroscopePage) {
        t_selectedHoroscopeKey = 0;
    }

    if (t_selectedHoroscopeKey != 0) EnsureHoroscopePageCreated();

    auto hostGrid = t_horoscopePage.hostGrid.get();
    auto pageBorder = t_horoscopePage.pageBorder.get();
    if (!hostGrid || !pageBorder) return;
    if (t_horoscopePage.renderedKey == t_selectedHoroscopeKey) return;

    const bool showDetail = (t_selectedHoroscopeKey != 0);

    static thread_local bool loggedUpdateError = false;
    try {
        if (showDetail) {
            bool vi = UseVietnameseLabels(settings);
            HoroscopePageData data = BuildHoroscopePageData(t_selectedHoroscopeKey, vi);
            if (auto tb = t_horoscopePage.titleSolar.get()) tb.Text(data.headerSolar);
            if (auto tb = t_horoscopePage.titleLunar.get()) tb.Text(data.headerLunar);
            if (auto tb = t_horoscopePage.rowCanChi.get()) tb.Text(data.lineCanChi);
            if (auto tb = t_horoscopePage.rowMenhTruc.get()) tb.Text(data.lineMenhTruc);
            if (auto tb = t_horoscopePage.rowGioHoangDao.get()) tb.Text(data.lineGioHoangDao);
            if (auto tb = t_horoscopePage.rowTuViNen.get()) tb.Text(data.lineTuViNen);
            if (auto tb = t_horoscopePage.rowTuViKy.get()) tb.Text(data.lineTuViKy);
            if (auto tb = t_horoscopePage.rowXungHuong.get()) tb.Text(data.lineXungHuong);
        }

        if (showDetail && t_horoscopePage.hiddenChildren.empty()) {
            int count = wuxm::VisualTreeHelper::GetChildrenCount(hostGrid);
            for (int i = 0; i < count; i++) {
                auto child = wuxm::VisualTreeHelper::GetChild(hostGrid, i);
                if (!child || child == pageBorder) continue;
                if (auto ui = child.try_as<wux::UIElement>()) {
                    t_horoscopePage.hiddenChildren.push_back(
                        {winrt::make_weak(ui), ui.Opacity(), ui.IsHitTestVisible()});
                    ui.Opacity(0.0);
                    ui.IsHitTestVisible(false);
                }
            }
        } else if (!showDetail) {
            RestoreHoroscopeCalendarChildren();
        }

        pageBorder.Visibility(showDetail ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        pageBorder.IsHitTestVisible(showDetail);
        t_horoscopePage.renderedKey = t_selectedHoroscopeKey;
        t_applyWrote = true;
        loggedUpdateError = false;
    } catch (...) {
        if (!loggedUpdateError) {
            LogCalendarException(L"Update horoscope page");
            loggedUpdateError = true;
        }
        RestoreHoroscopeCalendarChildren();
        try { pageBorder.Visibility(wux::Visibility::Collapsed); } catch (...) {}
        t_horoscopePage.renderedKey = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Scoped Calendar Header Lookup (Grid#CalendarCenterGrid > StackPanel#CalendarHeader)

void UpdateHeaderLabelOnCurrentThread() {
    ModSettings settings = GetSettings();
    if (!settings.enableCalendarFlyout) return;

    wuxc::Panel headerPanel = t_headerState.headerPanel.get();
    if (!headerPanel) return;

    wuxc::TextBlock headerLabel = t_headerState.headerLabel.get();

    if (!headerLabel) {
        try {
            wuxc::TextBlock primaryButtonTextBlock = nullptr;
            int count = wuxm::VisualTreeHelper::GetChildrenCount(headerPanel);
            for (int i = 0; i < count; i++) {
                auto child = wuxm::VisualTreeHelper::GetChild(headerPanel, i);
                if (!child) continue;

                if (auto tb = child.try_as<wuxc::TextBlock>()) {
                    headerLabel = tb;
                    t_headerState.createdByMod = (ElementName(tb) == kCustomHeaderLunarName);
                    break;
                }

                if (!primaryButtonTextBlock) {
                    std::vector<FoundElement> sub;
                    CollectDescendants(child, sub);
                    for (const auto& f : sub) {
                        if (auto innerTb = f.element.try_as<wuxc::TextBlock>()) {
                            primaryButtonTextBlock = innerTb;
                            break;
                        }
                    }
                }
            }

            if (headerLabel && !t_headerState.createdByMod && !t_headerState.hasSavedState) {
                try {
                    t_headerState.savedText = headerLabel.Text().c_str();
                    t_headerState.savedVisibility = headerLabel.Visibility();
                    t_headerState.hasSavedState = true;
                } catch (...) {}
            }

            if (!headerLabel) {
                wuxc::TextBlock newHeaderLunar;
                newHeaderLunar.Name(kCustomHeaderLunarName);
                newHeaderLunar.FontSize(13.5);
                newHeaderLunar.Opacity(0.85);
                newHeaderLunar.Margin({12, 0, 12, 6});
                if (primaryButtonTextBlock) {
                    try {
                        newHeaderLunar.Foreground(primaryButtonTextBlock.Foreground());
                    } catch (...) {}
                }

                headerPanel.Children().Append(newHeaderLunar);
                headerLabel = newHeaderLunar;
                t_headerState.createdByMod = true;
            }

            if (headerLabel) {
                t_headerState.headerLabel = winrt::make_weak(headerLabel);
            }
        } catch (...) {}
    }

    if (headerLabel) {
        int lDay, lMonth, lYear;
        bool isLeap;
        std::wstring canChi;
        GetLunarDateToday(lDay, lMonth, lYear, isLeap, canChi);

        bool vi = UseVietnameseLabels(settings);
        WCHAR buffer[128]{};
        if (vi) {
            if (isLeap) {
                _snwprintf_s(buffer, _TRUNCATE, L"%d Tháng %d (Nhuận) Năm %s",
                             lDay, lMonth, canChi.c_str());
            } else {
                _snwprintf_s(buffer, _TRUNCATE, L"%d Tháng %d Năm %s",
                             lDay, lMonth, canChi.c_str());
            }
        } else {
            if (isLeap) {
                _snwprintf_s(buffer, _TRUNCATE, L"Lunar: Day %d, Month %d (Leap), Year of %s",
                             lDay, lMonth, canChi.c_str());
            } else {
                _snwprintf_s(buffer, _TRUNCATE, L"Lunar: Day %d, Month %d, Year of %s",
                             lDay, lMonth, canChi.c_str());
            }
        }

        std::wstring desired = buffer;
        std::wstring current;
        try { current = headerLabel.Text().c_str(); } catch (...) {}

        try {
            if (headerLabel.Visibility() != wux::Visibility::Visible) {
                headerLabel.Visibility(wux::Visibility::Visible);
                t_applyWrote = true;
            }
        } catch (...) {}

        if (current != desired) {
            try {
                headerLabel.Text(desired);
                t_applyWrote = true;
            } catch (...) {}
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Keeping the Calendar Flyout Up to Date

thread_local winrt::Windows::System::DispatcherQueueTimer t_sweepTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker t_sweepTimerRevoker;
constexpr int kSweepIntervalMs = 1000;
constexpr int kSweepQuietTicksToStop = 5;
thread_local int t_sweepQuietTicks = 0;

void StopSweepTimer() {
    t_sweepQuietTicks = 0;
    if (t_sweepTimer) {
        try { t_sweepTimer.Stop(); } catch (...) {}
    }
}

void StartSweepTimer() {
    t_sweepQuietTicks = 0;
    try {
        if (!t_sweepTimer) {
            auto dispatcherQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) return;
            t_sweepTimer = dispatcherQueue.CreateTimer();
            t_sweepTimer.Interval(std::chrono::milliseconds{kSweepIntervalMs});
            t_sweepTimerRevoker = t_sweepTimer.Tick(
                winrt::auto_revoke,
                [](winrt::Windows::System::DispatcherQueueTimer const&, wf::IInspectable const&) {
                    SweepDayItems();
                });
        }
        if (!t_sweepTimer.IsRunning()) t_sweepTimer.Start();
    } catch (...) {}
}

struct CoreWindowTriggers {
    wuc::CoreWindow coreWindow{nullptr};
    winrt::event_token activatedToken{};
    winrt::event_token visibilityChangedToken{};
};
thread_local CoreWindowTriggers t_windowTriggers;
constexpr ULONGLONG kCalendarOpenedDebounceMs = 500;
thread_local ULONGLONG t_lastCalendarOpenedTick = 0;

void OnCalendarOpenedOnCurrentThread();
void InitializeForCurrentThread();
void UninitializeDayItemsForCurrentThread();

void RegisterCalendarTriggersOnCurrentThread() {
    try {
        if (t_windowTriggers.coreWindow) return;
        auto coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) return;
        t_windowTriggers.coreWindow = coreWindow;
        t_windowTriggers.activatedToken = coreWindow.Activated(
            [](wuc::CoreWindow const&, wuc::WindowActivatedEventArgs const& args) {
                if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated) {
                    OnCalendarOpenedOnCurrentThread();
                }
            });
        t_windowTriggers.visibilityChangedToken = coreWindow.VisibilityChanged(
            [](wuc::CoreWindow const&, wuc::VisibilityChangedEventArgs const& args) {
                if (args.Visible()) OnCalendarOpenedOnCurrentThread();
            });
    } catch (...) {}
}

void UnregisterCalendarTriggersOnCurrentThread() {
    try {
        auto coreWindow = t_windowTriggers.coreWindow;
        if (coreWindow) {
            if (t_windowTriggers.activatedToken.value != 0) coreWindow.Activated(t_windowTriggers.activatedToken);
            if (t_windowTriggers.visibilityChangedToken.value != 0) coreWindow.VisibilityChanged(t_windowTriggers.visibilityChangedToken);
        }
    } catch (...) {}
    t_windowTriggers = {};
}

thread_local wux::FrameworkElement t_layoutUpdatedElement{nullptr};
thread_local winrt::event_token t_layoutUpdatedToken{};
thread_local ULONGLONG t_layoutUpdatedStartTick = 0;
thread_local ULONGLONG t_lastLayoutUpdatedPassTick = 0;
constexpr ULONGLONG kLayoutUpdatedWatchMs = 5000;
constexpr ULONGLONG kLayoutUpdatedThrottleMs = 150;

void RevokeLayoutUpdatedWatch() {
    if (t_layoutUpdatedToken.value != 0) {
        auto element = t_layoutUpdatedElement;
        if (element) {
            try { element.LayoutUpdated(t_layoutUpdatedToken); } catch (...) {}
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
    if (now - t_lastLayoutUpdatedPassTick < kLayoutUpdatedThrottleMs) return;
    t_lastLayoutUpdatedPassTick = now;
    SweepDayItems();
    if (!t_dayItems.empty()) RevokeLayoutUpdatedWatch();
}

void WatchCalendarLayoutOnCurrentThread() {
    if (t_layoutUpdatedToken.value != 0) return;
    try {
        auto window = wux::Window::Current();
        if (!window) return;
        auto root = window.Content().try_as<wux::FrameworkElement>();
        if (!root) return;
        t_layoutUpdatedElement = root;
        t_layoutUpdatedStartTick = GetTickCount64();
        t_lastLayoutUpdatedPassTick = 0;
        t_layoutUpdatedToken = root.LayoutUpdated(
            [](wf::IInspectable const&, wf::IInspectable const&) {
                OnCalendarLayoutUpdatedOnCurrentThread();
            });
    } catch (...) { RevokeLayoutUpdatedWatch(); }
}

constexpr size_t kMaxScannedElements = 16384;
constexpr int kMaxScanDepth = 96;

void CollectDayItemsFromElement(wux::DependencyObject const& root,
                                size_t& budget,
                                int depth,
                                bool insideCalendarCenter) {
    if (!root || budget == 0 || depth > kMaxScanDepth) return;
    budget--;

    std::wstring name = ElementName(root);
    if (name == L"NotificationCenterGrid") return;
    if (name == L"CalendarCenterGrid") insideCalendarCenter = true;

    if (insideCalendarCenter && name == L"CalendarHeader") {
        if (auto panel = root.try_as<wuxc::Panel>()) {
            t_headerState.headerPanel = winrt::make_weak(panel);
        }
    }

    if (auto cv = root.try_as<wuxc::CalendarView>()) {
        EnsureCalendarViewSelectionHooked(cv);
    }

    if (auto dayItem = root.try_as<wuxc::CalendarViewDayItem>()) {
        TrackDayItem(dayItem);
        return;
    }

    int count = 0;
    try { count = wuxm::VisualTreeHelper::GetChildrenCount(root); } catch (...) { return; }
    for (int childIndex = 0; childIndex < count; childIndex++) {
        wux::DependencyObject child = nullptr;
        try { child = wuxm::VisualTreeHelper::GetChild(root, childIndex); } catch (...) { continue; }
        CollectDayItemsFromElement(child, budget, depth + 1, insideCalendarCenter);
    }
}

void CollectDayItemsOnCurrentThread() {
    wux::Window window = nullptr;
    try { window = wux::Window::Current(); } catch (...) { return; }
    if (!window) return;
    try {
        size_t budget = kMaxScannedElements;
        if (auto content = window.Content()) {
            CollectDayItemsFromElement(content, budget, 0, false);
        }
        for (auto popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
            if (budget == 0) break;
            if (auto child = popup.Child()) {
                CollectDayItemsFromElement(child, budget, 0, false);
            }
        }
    } catch (...) {}
}

void OnCalendarOpenedOnCurrentThread() {
    InitializeForCurrentThread();
    const ULONGLONG now = GetTickCount64();
    if (now - t_lastCalendarOpenedTick < kCalendarOpenedDebounceMs) return;
    t_lastCalendarOpenedTick = now;

    t_selectedHoroscopeKey = 0;

    if (!GetSettings().enableCalendarFlyout) return;

    RegisterCalendarTriggersOnCurrentThread();
    SweepDayItems();
    StartSweepTimer();
    WatchCalendarLayoutOnCurrentThread();
}

void SweepDayItems() {
    ModSettings settings = GetSettings();
    if (!settings.enableCalendarFlyout) {
        UninitializeDayItemsForCurrentThread();
        return;
    }

    t_applyWrote = false;
    InitializeForCurrentThread();
    RegisterCalendarTriggersOnCurrentThread();
    CollectDayItemsOnCurrentThread();

    UpdateHeaderLabelOnCurrentThread();

    for (size_t i = 0; i < t_dayItems.size();) {
        auto item = t_dayItems[i].item.get();
        if (!item) {
            t_dayItems.erase(t_dayItems.begin() + i);
            continue;
        }
        ApplyToDayItem(item);
        i++;
    }

    UpdateHoroscopePageOnCurrentThread();

    for (auto it = t_cells.begin(); it != t_cells.end();) {
        if (it->second->item.get()) ++it;
        else it = t_cells.erase(it);
    }
    for (auto it = t_labelOwners.begin(); it != t_labelOwners.end();) {
        if (it->second.get()) ++it;
        else it = t_labelOwners.erase(it);
    }

    if (t_applyWrote) t_sweepQuietTicks = 0;
    else if (++t_sweepQuietTicks >= kSweepQuietTicksToStop) StopSweepTimer();
}

void UninitializeDayItemsForCurrentThread() {
    if (auto calView = t_calendarView.get()) {
        if (t_selectedDatesChangedToken.value != 0) {
            try { calView.SelectedDatesChanged(t_selectedDatesChangedToken); } catch (...) {}
        }
        if (t_hasSavedSelectionMode) {
            try { calView.SelectionMode(t_savedSelectionMode); } catch (...) {}
        }
    }
    t_selectedDatesChangedToken = {};
    t_hasSavedSelectionMode = false;
    t_calendarView = nullptr;

    for (auto& entry : t_cells) {
        DayCell& cell = *entry.second;
        auto item = cell.item.get();
        if (item) {
            if (cell.dateChangedToken != 0) {
                try { item.UnregisterPropertyChangedCallback(wuxc::CalendarViewDayItem::DateProperty(), cell.dateChangedToken); } catch (...) {}
                cell.dateChangedToken = 0;
            }
            try {
                if (cell.itemColor.hadColor) RestoreControlForeground(item, cell.itemColor);
            } catch (...) {}
        }

        if (auto lunarLabel = cell.lunarLabel.get()) {
            if (cell.lunarLabelChangedToken) {
                try { lunarLabel.UnregisterPropertyChangedCallback(wuxc::TextBlock::TextProperty(), cell.lunarLabelChangedToken); } catch (...) {}
                cell.lunarLabelChangedToken = 0;
            }
            if (cell.createdByMod) {
                if (auto parentPanel = cell.createdParentPanel.get()) {
                    try {
                        uint32_t idx = 0;
                        if (parentPanel.Children().IndexOf(lunarLabel, idx)) {
                            parentPanel.Children().RemoveAt(idx);
                        }
                    } catch (...) {}
                }
            } else {
                try {
                    lunarLabel.RenderTransform(nullptr);
                    lunarLabel.ClearValue(wux::FrameworkElement::MarginProperty());
                    if (cell.hasSavedText) lunarLabel.Text(cell.savedText);
                    if (cell.hasSavedVisibility) lunarLabel.Visibility(cell.savedVisibility);
                    if (cell.lunarColor.hadColor) RestoreForeground(lunarLabel, cell.lunarColor);
                } catch (...) {}
            }
        }

        if (auto solarLabel = cell.solarLabel.get()) {
            try {
                solarLabel.RenderTransform(nullptr);
                solarLabel.ClearValue(wux::FrameworkElement::MarginProperty());
                if (cell.solarColor.hadColor) RestoreForeground(solarLabel, cell.solarColor);
            } catch (...) {}
        }
    }

    if (auto headerLabel = t_headerState.headerLabel.get()) {
        if (t_headerState.createdByMod) {
            if (auto headerPanel = t_headerState.headerPanel.get()) {
                try {
                    uint32_t idx = 0;
                    if (headerPanel.Children().IndexOf(headerLabel, idx)) {
                        headerPanel.Children().RemoveAt(idx);
                    }
                } catch (...) {}
            }
        } else if (t_headerState.hasSavedState) {
            try {
                headerLabel.Text(t_headerState.savedText);
                headerLabel.Visibility(t_headerState.savedVisibility);
            } catch (...) {}
        }
    }
    t_headerState = {};

    RestoreHoroscopeCalendarChildren();
    if (auto backButton = t_horoscopePage.backButton.get()) {
        if (t_horoscopePage.backClickToken.value != 0) {
            try { backButton.Click(t_horoscopePage.backClickToken); } catch (...) {}
        }
    }
    if (auto hostGrid = t_horoscopePage.hostGrid.get()) {
        try {
            if (auto pageBorder = t_horoscopePage.pageBorder.get()) {
                uint32_t idx = 0;
                if (hostGrid.Children().IndexOf(pageBorder, idx)) {
                    hostGrid.Children().RemoveAt(idx);
                }
            }
        } catch (...) {}
    }
    t_horoscopePage = {};
    t_selectedHoroscopeKey = 0;

    decltype(t_cells)().swap(t_cells);
    decltype(t_dayItems)().swap(t_dayItems);
    decltype(t_labelOwners)().swap(t_labelOwners);

    t_sweepQuietTicks = 0;
    try { if (t_sweepTimer) t_sweepTimer.Stop(); } catch (...) {}
    t_sweepTimerRevoker.revoke();
    t_sweepTimer = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar Clock & Tooltip Horoscope (explorer.exe)

std::atomic<bool> g_systemTrayModuleHooked{false};
thread_local bool t_inRefreshIcon = false;
thread_local bool t_inGetTimeToolTipString = false;

using GetTimeFormatEx_t = decltype(&GetTimeFormatEx);
GetTimeFormatEx_t GetTimeFormatEx_Original;

using GetDateFormatEx_t = decltype(&GetDateFormatEx);
GetDateFormatEx_t GetDateFormatEx_Original;

using ClockSystemTrayIconDataModel_RefreshIcon_t = void(WINAPI*)(LPVOID pThis, LPVOID param1);
ClockSystemTrayIconDataModel_RefreshIcon_t ClockSystemTrayIconDataModel_RefreshIcon_Original;
ClockSystemTrayIconDataModel_RefreshIcon_t ClockSystemTrayIconDataModel2_RefreshIcon_Original;

using ClockSystemTrayIconDataModel_GetTimeToolTipString_t =
    LPVOID(WINAPI*)(LPVOID pThis, LPVOID, LPVOID, LPVOID, LPVOID);
ClockSystemTrayIconDataModel_GetTimeToolTipString_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString_Original;
ClockSystemTrayIconDataModel_GetTimeToolTipString_t
    ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original;
ClockSystemTrayIconDataModel_GetTimeToolTipString_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original;
ClockSystemTrayIconDataModel_GetTimeToolTipString_t
    ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original;
ClockSystemTrayIconDataModel_GetTimeToolTipString_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString_Legacy_Original;

void WINAPI ClockSystemTrayIconDataModel_RefreshIcon_Hook(LPVOID pThis, LPVOID param1) {
    t_inRefreshIcon = true;
    ClockSystemTrayIconDataModel_RefreshIcon_Original(pThis, param1);
    t_inRefreshIcon = false;
}

void WINAPI ClockSystemTrayIconDataModel2_RefreshIcon_Hook(LPVOID pThis, LPVOID param1) {
    t_inRefreshIcon = true;
    ClockSystemTrayIconDataModel2_RefreshIcon_Original(pThis, param1);
    t_inRefreshIcon = false;
}

void AppendHoroscopeToToolTipString(LPVOID tooltipPtrPtr) {
    if (!tooltipPtrPtr || !*(void**)tooltipPtrPtr) return;

    ModSettings settings = GetSettings();
    if (!settings.enableTaskbarTooltip) return;

    std::wstring extraLine = FormatTooltipHoroscope(settings);
    if (extraLine.empty()) return;
    size_t extraLength = extraLine.size();

    auto separator = L"\r\n\r\n"sv;

    struct hstring_header {
        uint32_t flags;
        uint32_t length;
        uint32_t padding1;
        uint32_t padding2;
        wchar_t const* ptr;
    };

    struct shared_hstring_header : hstring_header {
        int32_t count;
        wchar_t buffer[1];
    };

    shared_hstring_header* tooltipHeader = *(shared_hstring_header**)tooltipPtrPtr;
    if (!tooltipHeader) return;

    uint64_t bytesRequired = sizeof(shared_hstring_header) +
                             sizeof(wchar_t) * (tooltipHeader->length + separator.length() + extraLength);

    shared_hstring_header* tooltipHeaderNew =
        (shared_hstring_header*)HeapReAlloc(GetProcessHeap(), 0, tooltipHeader, bytesRequired);
    if (!tooltipHeaderNew) return;

    tooltipHeaderNew->ptr = tooltipHeaderNew->buffer;

    memcpy(tooltipHeaderNew->buffer + tooltipHeaderNew->length,
           separator.data(), sizeof(wchar_t) * separator.length());
    tooltipHeaderNew->length += static_cast<uint32_t>(separator.length());

    memcpy(tooltipHeaderNew->buffer + tooltipHeaderNew->length,
           extraLine.data(), sizeof(wchar_t) * extraLength);
    tooltipHeaderNew->length += static_cast<uint32_t>(extraLength);

    tooltipHeaderNew->buffer[tooltipHeaderNew->length] = L'\0';
    *(shared_hstring_header**)tooltipPtrPtr = tooltipHeaderNew;
}

LPVOID InvokeGetTimeToolTipStringHook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4,
    ClockSystemTrayIconDataModel_GetTimeToolTipString_t original) {
    t_inGetTimeToolTipString = true;
    LPVOID ret = original(pThis, p1, p2, p3, p4);
    AppendHoroscopeToToolTipString(ret);
    t_inGetTimeToolTipString = false;
    return ret;
}

LPVOID WINAPI ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    return InvokeGetTimeToolTipStringHook(
        pThis, p1, p2, p3, p4, ClockSystemTrayIconDataModel_GetTimeToolTipString_Original);
}

LPVOID WINAPI ClockSystemTrayIconDataModel2_GetTimeToolTipString_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    return InvokeGetTimeToolTipStringHook(
        pThis, p1, p2, p3, p4, ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original);
}

LPVOID WINAPI ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    return InvokeGetTimeToolTipStringHook(
        pThis, p1, p2, p3, p4, ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original);
}

LPVOID WINAPI ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    return InvokeGetTimeToolTipStringHook(
        pThis, p1, p2, p3, p4, ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original);
}

LPVOID WINAPI ClockSystemTrayIconDataModel_GetTimeToolTipString_Legacy_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    return InvokeGetTimeToolTipStringHook(
        pThis, p1, p2, p3, p4, ClockSystemTrayIconDataModel_GetTimeToolTipString_Legacy_Original);
}

bool HookSystemTraySymbols(HMODULE module) {
    if (!module) return false;

    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_RefreshIcon_Original,
            ClockSystemTrayIconDataModel_RefreshIcon_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel2_RefreshIcon_Original,
            ClockSystemTrayIconDataModel2_RefreshIcon_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_GetTimeToolTipString_Original,
            ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original,
            ClockSystemTrayIconDataModel2_GetTimeToolTipString_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original,
            ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original,
            ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME *,struct _TIME_DYNAMIC_ZONE_INFORMATION *,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_GetTimeToolTipString_Legacy_Original,
            ClockSystemTrayIconDataModel_GetTimeToolTipString_Legacy_Hook,
            true,
        },
    };
    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for system tray module");
        return false;
    }
    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return static_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandleW(L"Taskbar.View.dll");
        if (module) {
            VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor = fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module) {
        if (!g_systemTrayModuleHooked.exchange(true)) {
            if (HookSystemTraySymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }
    return module;
}

int WINAPI GetTimeFormatEx_Hook(LPCWSTR lpLocaleName,
                                DWORD dwFlags,
                                CONST SYSTEMTIME* lpTime,
                                LPCWSTR lpFormat,
                                LPWSTR lpTimeStr,
                                int cchTime) {
    if (t_inRefreshIcon && !t_inGetTimeToolTipString) {
        ModSettings settings = GetSettings();
        if (settings.enableTaskbarClock && settings.taskbarClockSolarOnTop) {
            if (!cchTime) {
                return FORMATTED_BUFFER_SIZE;
            }

            // Preserve user's 12/24-hour setting and seconds via GetTimeFormatEx_Original
            WCHAR origTime[128]{};
            GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime, lpFormat,
                                     origTime, ARRAYSIZE(origTime));

            SYSTEMTIME st{};
            if (lpTime) {
                st = *lpTime;
            } else {
                GetLocalTime(&st);
            }

            WCHAR weekdayBuf[32]{};
            if (UseVietnameseLabels(settings)) {
                static const PCWSTR kWeekdaysVi[] = {
                    L"CN", L"T2", L"T3", L"T4", L"T5", L"T6", L"T7"
                };
                wcscpy_s(weekdayBuf, (st.wDayOfWeek < 7) ? kWeekdaysVi[st.wDayOfWeek] : L"");
            } else {
                GetDateFormatEx_Original(lpLocaleName, DATE_AUTOLAYOUT, &st, L"ddd",
                                         weekdayBuf, ARRAYSIZE(weekdayBuf), nullptr);
            }

            _snwprintf_s(lpTimeStr, cchTime, _TRUNCATE, L"%s - %s, %02d/%02d",
                         origTime, weekdayBuf, st.wDay, st.wMonth);
            return static_cast<int>(wcslen(lpTimeStr)) + 1;
        }
    }
    return GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime, lpFormat, lpTimeStr, cchTime);
}

int WINAPI GetDateFormatEx_Hook(LPCWSTR lpLocaleName,
                                DWORD dwFlags,
                                CONST SYSTEMTIME* lpDate,
                                LPCWSTR lpFormat,
                                LPWSTR lpDateStr,
                                int cchDate,
                                LPCWSTR lpCalendar) {
    if (t_inRefreshIcon && !t_inGetTimeToolTipString && !(dwFlags & DATE_LONGDATE)) {
        ModSettings settings = GetSettings();
        if (settings.enableTaskbarClock) {
            if (!cchDate) {
                return FORMATTED_BUFFER_SIZE;
            }

            SYSTEMTIME st{};
            if (lpDate) {
                st = *lpDate;
            } else {
                GetLocalTime(&st);
            }

            bool vi = UseVietnameseLabels(settings);
            std::wstring lunarStr = FormatTaskbarLunarDate(st.wYear, st.wMonth, st.wDay, vi);

            if (settings.taskbarClockSolarOnTop) {
                _snwprintf_s(lpDateStr, cchDate, _TRUNCATE, L"%s", lunarStr.c_str());
            } else {
                // Keep the user's locale short date and append the lunar date
                WCHAR origDate[128]{};
                GetDateFormatEx_Original(lpLocaleName, dwFlags, lpDate, lpFormat,
                                         origDate, ARRAYSIZE(origDate), lpCalendar);
                _snwprintf_s(lpDateStr, cchDate, _TRUNCATE, L"%s • %s",
                             origDate, lunarStr.c_str());
            }
            return static_cast<int>(wcslen(lpDateStr)) + 1;
        }
    }
    return GetDateFormatEx_Original(lpLocaleName, dwFlags, lpDate, lpFormat, lpDateStr, cchDate, lpCalendar);
}

void RefreshTaskbarClock() {
    constexpr WCHAR kTempValueName[] = L"_temp_windhawk_vn_lunar_clock";
    HKEY hSubKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Control Panel\\TimeDate\\AdditionalClocks", 0,
                      KEY_WRITE, &hSubKey) == ERROR_SUCCESS) {
        if (RegSetValueExW(hSubKey, kTempValueName, 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(L""),
                           sizeof(WCHAR)) == ERROR_SUCCESS) {
            RegDeleteValueW(hSubKey, kTempValueName);
        }
        RegCloseKey(hSubKey);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Process Plumbing

enum class Target { Explorer, ShellExperienceHost, ShellHost };
Target g_target = Target::ShellExperienceHost;
std::atomic<UINT> g_runFromWindowThreadViaPostMessageMsg{0};

UINT RunFromWindowThreadViaPostMessageMsg() {
    UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        message = RegisterWindowMessageW(L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);
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
    for (HHOOK hook : hooks) UnhookWindowsHookEx(hook);
}

void DrainPendingRunFromWindowThreadPostsOnCurrentThread() {
    const UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) return;
    MSG msg;
    while (PeekMessageW(&msg, nullptr, message, message, PM_REMOVE)) {}
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) return;
    g_initializedForThread = true;
    RegisterCalendarTriggersOnCurrentThread();
}

void UninitializeForCurrentThread() {
    DrainPendingRunFromWindowThreadPostsOnCurrentThread();
    if (!g_initializedForThread) return;
    UnregisterCalendarTriggersOnCurrentThread();
    RevokeLayoutUpdatedWatch();
    UninitializeDayItemsForCurrentThread();
    g_initializedForThread = false;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;
    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

bool RunFromWindowThreadViaPostMessage(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };
    const UINT message = RunFromWindowThreadViaPostMessageMsg();
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;

    HHOOK hook = SetWindowsHookExW(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = reinterpret_cast<MSG*>(lParam);
                if (msg->message == g_runFromWindowThreadViaPostMessageMsg.load()) {
                    auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(msg->lParam);
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
    if (!hook) return false;

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{proc, procParam, hook};
    RememberPendingPostHook(hook);
    if (!PostMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(param))) {
        ForgetPendingPostHook(hook);
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }
    return true;
}

bool IsCalendarWindowClass(PCWSTR className) {
    switch (g_target) {
        case Target::ShellExperienceHost: return _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0;
        case Target::ShellHost: return _wcsicmp(className, L"ControlCenterWindow") == 0;
        default: return false;
    }
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName) {
    if (g_shuttingDown) return;
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName || !IsCalendarWindowClass(lpClassName)) return;
    RunFromWindowThreadViaPostMessage(
        hWnd,
        [](PVOID) {
            InitializeForCurrentThread();
            OnCalendarOpenedOnCurrentThread();
        },
        nullptr);
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                           DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                           HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                           PVOID lpParam, DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                    PVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                            X, Y, nWidth, nHeight, hWndParent, hMenu,
                                            hInstance, lpParam, dwBand);
    if (hWnd) OnWindowCreated(hWnd, lpClassName);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                             DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                             HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                             PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                      DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                      HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                      PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                              X, Y, nWidth, nHeight, hWndParent, hMenu,
                                              hInstance, lpParam, dwBand, dwTypeFlags);
    if (hWnd) OnWindowCreated(hWnd, lpClassName);
    return hWnd;
}

void OnWindowShown(HWND hWnd) {
    if (g_shuttingDown) return;
    DWORD dwProcessId = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return;
    WCHAR szClassName[32];
    if (GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 || !IsCalendarWindowClass(szClassName)) return;
    RunFromWindowThreadViaPostMessage(hWnd, [](PVOID) { OnCalendarOpenedOnCurrentThread(); }, nullptr);
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    const BOOL wasVisible = ShowWindow_Original(hWnd, nCmdShow);
    if (nCmdShow != SW_HIDE) OnWindowShown(hWnd);
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
            auto& param = *reinterpret_cast<ENUM_WINDOWS_PARAM*>(lParam);
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return TRUE;
            WCHAR szClassName[32];
            if (GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) return TRUE;
            if (IsCalendarWindowClass(szClassName)) param.hWnds->push_back(hWnd);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));
    return hWnds;
}

template <typename Prototype>
bool InstallHook(HMODULE module, PCSTR functionName, Prototype* hook, Prototype** original) {
    if (!module) {
        Wh_Log(L"Module is null when hooking %S", functionName);
        return false;
    }
    Prototype* target = reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));
    if (!target) {
        Wh_Log(L"Failed to resolve %S", functionName);
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Failed to set function hook for %S", functionName);
        return false;
    }
    return true;
}

BOOL Wh_ModInit() {
    g_shuttingDown = false;
    g_target = Target::ShellExperienceHost;
    WCHAR moduleFilePath[MAX_PATH];
    switch (GetModuleFileNameW(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return FALSE;
        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"explorer.exe") == 0) {
                    g_target = Target::Explorer;
                } else if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                return FALSE;
            }
            break;
    }

    LoadSettings();

    if (g_target == Target::Explorer) {
        if (HMODULE sysTrayModule = GetSystemTrayModuleHandle()) {
            g_systemTrayModuleHooked = true;
            HookSystemTraySymbols(sysTrayModule);
        }

        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBaseModule) {
            Wh_Log(L"Failed to get kernelbase.dll handle");
            return FALSE;
        }
        InstallHook(kernelBaseModule, "LoadLibraryExW", LoadLibraryExW_Hook, &LoadLibraryExW_Original);
        InstallHook(kernelBaseModule, "GetTimeFormatEx", GetTimeFormatEx_Hook, &GetTimeFormatEx_Original);
        InstallHook(kernelBaseModule, "GetDateFormatEx", GetDateFormatEx_Hook, &GetDateFormatEx_Original);
        return TRUE;
    }

    RunFromWindowThreadViaPostMessageMsg();
    HMODULE user32Module = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    InstallHook(user32Module, "CreateWindowInBand", CreateWindowInBand_Hook, &CreateWindowInBand_Original);
    InstallHook(user32Module, "CreateWindowInBandEx", CreateWindowInBandEx_Hook, &CreateWindowInBandEx_Original);
    InstallHook(user32Module, "ShowWindow", ShowWindow_Hook, &ShowWindow_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_target == Target::Explorer) {
        if (!g_systemTrayModuleHooked) {
            if (HMODULE sysTrayModule = GetSystemTrayModuleHandle()) {
                if (!g_systemTrayModuleHooked.exchange(true)) {
                    if (HookSystemTraySymbols(sysTrayModule)) {
                        Wh_ApplyHookOperations();
                    }
                }
            }
        }
        RefreshTaskbarClock();
        return;
    }
    for (auto hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                InitializeForCurrentThread();
                OnCalendarOpenedOnCurrentThread();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    g_shuttingDown = true;
    if (g_target == Target::Explorer) {
        RefreshTaskbarClock();
        return;
    }
    for (auto hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }
    UnhookPendingPostHooks();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_target == Target::Explorer) {
        RefreshTaskbarClock();
        return;
    }
    for (auto hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                if (!GetSettings().enableCalendarFlyout) {
                    UninitializeDayItemsForCurrentThread();
                } else {
                    t_horoscopePage.renderedKey = -1;
                    SweepDayItems();
                    StartSweepTimer();
                }
            },
            nullptr);
    }
}
