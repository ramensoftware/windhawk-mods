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
- **100% Offline & Lightweight**: Embedded Hồ Ngọc Đức Vietnamese astronomical lunar algorithm runs completely in-memory with zero network requests and zero disk writes.
- **Authentic Lunar Dates**: Shows day and month (e.g. `17/9`, `4/9`). Leap months are clearly marked with an `N` suffix (e.g. `15/8N`).
- **Color Highlighting**: Highlights full moons (day 15), new moons (day 1), and traditional holidays in red (`#FF5A5A`).
- **Sexagenary Cycle Header**: Automatically replaces the Chinese lunar month header with full Vietnamese Can Chi year names (e.g. `4 Tháng 9 Năm Bính Ngọ`).
- **High-Contrast on Accent Circle**: Automatically renders today's date with high-contrast text on the system accent circle so it stays clear and legible.
- **Safe Lifecycle**: Preserves and cleanly restores all original text, colors, and headers when unloaded.

## How to activate
In order for Windows 11 to allocate the secondary subtitle line in the calendar flyout cells for this mod to display lunar dates:
1. Open Windows Settings (`Win + I`) > **Time & language** > **Date & time**.
2. Under **"Show additional calendars in the taskbar"**, select **"Simplified Chinese (Lunar)"**.
   *(This mod intercepts the text and replaces all Chinese characters with 100% Vietnamese lunar dates).*

---

# Lịch Âm Việt Nam cho Windows 11 (Tiếng Việt)

Hiển thị ngày Âm lịch Việt Nam, Mùng 1, Rằm và các dịp Lễ Tết truyền thống trực tiếp trên bảng Lịch (Notification Center Calendar Flyout) của Windows 11 khi nhấn vào đồng hồ ở Taskbar.

## Tính năng chính
- **Hoàn toàn Offline & Siêu nhẹ**: Tích hợp sẵn thuật toán thiên văn học Hồ Ngọc Đức chạy độc lập 100% trong bộ nhớ RAM, không gọi mạng bên ngoài, không ghi file rác.
- **Hiển thị ngày Âm lịch thuần túy**: Ngày/tháng âm (ví dụ: `17/9`, `4/9`...). Tháng nhuận được ký hiệu rõ ràng với chữ `N` (ví dụ `15/8N`).
- **Tô màu nổi bật**: Tự động tô màu đỏ (`#FF5A5A`) cho ngày Rằm (15), Mùng 1 (1) và các dịp Lễ Tết truyền thống.
- **Tiêu đề tháng Can Chi**: Tự động chuyển đổi tiêu đề tháng sang tiếng Việt đầy đủ Năm Can Chi: `4 Tháng 9 Năm Bính Ngọ`.
- **Tương phản cao ngày hôm nay**: Tự động đổi chữ màu đen tương phản trên nền tròn xanh ngày hiện tại để chữ không bị chìm.
- **An toàn & Sạch sẽ**: Lưu và khôi phục nguyên vẹn trạng thái chữ gốc của Windows khi tắt hoặc gỡ mod.

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
// The Chinese lunar calendar inspection structure follows "Chinese Holiday Calendar"
// by dcsmf, also GPL-3.0:
//
//   Copyright (C) 2026 dcsmf
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

#include <Unknwn.h>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <windhawk_utils.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

thread_local bool g_initializedForThread;

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
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

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring_view hex) {
    if (hex.starts_with(L"#")) {
        hex.remove_prefix(1);
    }
    if (hex.length() != 6 && hex.length() != 8) {
        return std::nullopt;
    }

    auto parseByte = [](std::wstring_view s) -> std::optional<uint8_t> {
        wchar_t* end = nullptr;
        std::wstring str(s);
        unsigned long val = std::wcstoul(str.c_str(), &end, 16);
        if (end != str.c_str() + str.length() || val > 255) {
            return std::nullopt;
        }
        return static_cast<uint8_t>(val);
    };

    uint8_t a = 255;
    size_t offset = 0;
    if (hex.length() == 8) {
        auto parsedA = parseByte(hex.substr(0, 2));
        if (!parsedA) return std::nullopt;
        a = *parsedA;
        offset = 2;
    }

    auto r = parseByte(hex.substr(offset, 2));
    auto g = parseByte(hex.substr(offset + 2, 2));
    auto b = parseByte(hex.substr(offset + 4, 2));

    if (!r || !g || !b) {
        return std::nullopt;
    }

    return winrt::Windows::UI::Color{a, *r, *g, *b};
}

void LoadSettings() {
    auto newSettings = std::make_shared<Settings>();
    newSettings->replaceText = Wh_GetIntSetting(L"replaceText");

    WindhawkUtils::StringSetting specialDayColorSetting(L"specialDayColor");
    if (specialDayColorSetting.get()) {
        auto col = ParseHexColor(specialDayColorSetting.get());
        if (col) newSettings->specialDayColor = *col;
    }

    WindhawkUtils::StringSetting normalDayColorSetting(L"normalDayColor");
    if (normalDayColorSetting.get()) {
        auto col = ParseHexColor(normalDayColorSetting.get());
        if (col) newSettings->normalDayColor = *col;
    }

    WindhawkUtils::StringSetting todayDayColorSetting(L"todayDayColor");
    if (todayDayColorSetting.get()) {
        auto col = ParseHexColor(todayDayColorSetting.get());
        if (col) newSettings->todayDayColor = *col;
    }

    WindhawkUtils::StringSetting todaySpecialDayColorSetting(L"todaySpecialDayColor");
    if (todaySpecialDayColorSetting.get()) {
        auto col = ParseHexColor(todaySpecialDayColorSetting.get());
        if (col) newSettings->todaySpecialDayColor = *col;
    }

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = newSettings;
}

////////////////////////////////////////////////////////////////////////////////
// Helpers

struct HolidayInfo {
    std::wstring name;
    bool isSpecial = false;
    std::wstring headerText;
    std::wstring fullDesc;
};

inline int32_t MakeDateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

bool IsNumberText(std::wstring const& text) {
    if (text.empty()) return false;
    for (wchar_t ch : text) {
        if (!iswdigit(ch)) return false;
    }
    return true;
}

std::wstring ElementText(wuxc::TextBlock const& tb) {
    try {
        return tb.Text().c_str();
    } catch (...) {
        return {};
    }
}

std::wstring ElementName(wux::FrameworkElement const& fe) {
    try {
        return fe.Name().c_str();
    } catch (...) {
        return {};
    }
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
    if (lLeap) {
        info.name = std::to_wstring(lDay) + L"/" + std::to_wstring(lMonth) + L"N";
    } else {
        info.name = std::to_wstring(lDay) + L"/" + std::to_wstring(lMonth);
    }
    
    // Mùng 1 & Rằm are special days in all months (including leap months)
    if (lDay == 1 || lDay == 15) {
        info.isSpecial = true;
    }

    // Lunar holidays only trigger on non-leap months
    if (!lLeap) {
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
    }

    // Solar Holidays
    if (m == 1 && d == 1) info.isSpecial = true;
    if (m == 4 && d == 30) info.isSpecial = true;
    if (m == 5 && d == 1) info.isSpecial = true;
    if (m == 9 && d == 2) info.isSpecial = true;

    std::wstring canChi = VietnameseLunar::getCanChiYear(lYear);
    std::wstring leapSuffix = lLeap ? L" (Nhuận)" : L"";
    info.headerText = std::to_wstring(lDay) + L" Tháng " + std::to_wstring(lMonth) + leapSuffix + L" Năm " + canChi;
    
    wchar_t descBuf[256];
    swprintf_s(descBuf, L"Ngày %d tháng %d%s âm lịch (%s)", lDay, lMonth, lLeap ? L" nhuận" : L"", canChi.c_str());
    info.fullDesc = descBuf;

    return info;
}

std::mutex g_lunarCacheMutex;
std::unordered_map<int32_t, HolidayInfo> g_lunarCache;

bool LookupHoliday(int32_t key, HolidayInfo& out) {
    std::lock_guard<std::mutex> lock(g_lunarCacheMutex);
    auto it = g_lunarCache.find(key);
    if (it != g_lunarCache.end()) {
        out = it->second;
        return true;
    }
    out = CalculateVietnameseLunar(key);
    g_lunarCache[key] = out;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Visual tree navigation

constexpr PCWSTR kLunarTextBlockName = L"LunarTextBlock";

struct FoundElement {
    wux::DependencyObject element;
    int depth;
};

void CollectDescendants(wux::DependencyObject const& root,
                        std::vector<FoundElement>& out,
                        int depth = 0) {
    int count = 0;
    try {
        count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        wux::DependencyObject child = nullptr;
        try {
            child = wuxm::VisualTreeHelper::GetChild(root, i);
        } catch (...) {
            continue;
        }

        if (child) {
            out.push_back({child, depth});
            CollectDescendants(child, out, depth + 1);
        }
    }
}

struct CellElements {
    wuxc::TextBlock labelTextBlock{nullptr};
    bool isAccentCircle{false};
};

CellElements FindCellElements(wuxc::CalendarViewDayItem const& item, void* preferredId, int32_t key) {
    CellElements result;
    SYSTEMTIME stToday;
    GetLocalTime(&stToday);
    int32_t todayKey = MakeDateKey(stToday.wYear, stToday.wMonth, stToday.wDay);
    if (key == todayKey) {
        result.isAccentCircle = true;
    }

    std::vector<FoundElement> descendants;
    CollectDescendants(item, descendants);

    std::vector<wuxc::TextBlock> candidates;
    for (const auto& found : descendants) {
        auto textBlock = found.element.try_as<wuxc::TextBlock>();
        if (!textBlock) continue;
        
        std::wstring text = ElementText(textBlock);
        if (IsNumberText(text)) {
            if (!result.isAccentCircle) {
                if (auto scb = textBlock.Foreground().try_as<wuxm::SolidColorBrush>()) {
                    auto c = scb.Color();
                    double luma = (0.299 * c.R + 0.587 * c.G + 0.114 * c.B) / 255.0;
                    if (luma < 0.35) {
                        result.isAccentCircle = true;
                    }
                }
            }
            continue;
        }
        candidates.push_back(textBlock);
    }

    if (candidates.empty()) return result;

    if (preferredId) {
        for (const auto& candidate : candidates) {
            if (winrt::get_abi(candidate) == preferredId) {
                result.labelTextBlock = candidate;
                return result;
            }
        }
    }

    for (const auto& candidate : candidates) {
        if (ElementName(candidate) == kLunarTextBlockName) {
            result.labelTextBlock = candidate;
            return result;
        }
    }

    wuxc::TextBlock best = nullptr;
    double bestFontSize = 0;
    bool bestHasText = false;
    for (const auto& candidate : candidates) {
        bool hasText = !ElementText(candidate).empty();
        double fontSize = 0;
        try {
            fontSize = candidate.FontSize();
        } catch (...) {}

        if (!best || (hasText != bestHasText && hasText) ||
            (hasText == bestHasText && fontSize < bestFontSize)) {
            best = candidate;
            bestFontSize = fontSize;
            bestHasText = hasText;
        }
    }

    result.labelTextBlock = best;
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Day cell state and application

struct DayCellState {
    winrt::weak_ref<wuxc::CalendarViewDayItem> dayItem;
    winrt::weak_ref<wuxc::TextBlock> label;
    int64_t textChangedToken = 0;
    std::wstring savedText;
    std::wstring appliedText;
    wuxm::Brush savedBrush{nullptr};
    wuxm::Brush ownBrush{nullptr};
    bool hadColor = false;
    bool hadText = false;
};

thread_local std::unordered_map<void*, DayCellState> t_cells;
thread_local bool t_applyWrote = false;

std::optional<int32_t> KeyFromDayItem(wuxc::CalendarViewDayItem const& item) {
    try {
        wf::DateTime dt = item.Date();
        auto seconds = dt.time_since_epoch().count() / 10000000LL - 11644473600LL;
        time_t t = static_cast<time_t>(seconds);
        struct tm tmBuf;
        if (gmtime_s(&tmBuf, &t) == 0) {
            return MakeDateKey(tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday);
        }
    } catch (...) {}
    return std::nullopt;
}

void SaveForeground(DayCellState& cell, wuxc::TextBlock const& label) {
    if (!cell.savedBrush) {
        try {
            cell.savedBrush = label.Foreground();
        } catch (...) {}
    }
}

void RestoreForeground(wuxc::TextBlock const& label, DayCellState& cell) {
    if (cell.savedBrush) {
        try {
            label.Foreground(cell.savedBrush);
        } catch (...) {}
        cell.savedBrush = nullptr;
    }
    cell.ownBrush = nullptr;
}

wuxm::Brush SetForegroundIfDifferent(wuxc::TextBlock const& label,
                                     winrt::Windows::UI::Color targetColor,
                                     wuxm::Brush const& currentBrush) {
    if (auto scb = currentBrush.try_as<wuxm::SolidColorBrush>()) {
        try {
            auto c = scb.Color();
            if (c.A == targetColor.A && c.R == targetColor.R &&
                c.G == targetColor.G && c.B == targetColor.B) {
                return currentBrush;
            }
        } catch (...) {}
    }

    try {
        wuxm::SolidColorBrush newBrush(targetColor);
        label.Foreground(newBrush);
        t_applyWrote = true;
        return newBrush;
    } catch (...) {
        return nullptr;
    }
}

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item) {
    auto maybeKey = KeyFromDayItem(item);
    if (!maybeKey) return;
    int32_t key = *maybeKey;

    void* itemId = winrt::get_abi(item);
    DayCellState& cell = t_cells[itemId];
    cell.dayItem = winrt::make_weak(item);

    void* preferredLabelId = nullptr;
    if (auto oldLabel = cell.label.get()) {
        preferredLabelId = winrt::get_abi(oldLabel);
    }

    CellElements elems = FindCellElements(item, preferredLabelId, key);
    auto label = elems.labelTextBlock;
    if (!label) return;

    if (cell.label.get() != label) {
        if (auto oldLabel = cell.label.get()) {
            if (cell.textChangedToken) {
                try {
                    oldLabel.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::TextProperty(), cell.textChangedToken);
                } catch (...) {}
                cell.textChangedToken = 0;
            }
            if (cell.hadColor) {
                RestoreForeground(oldLabel, cell);
                cell.hadColor = false;
            }
        }
        cell.label = winrt::make_weak(label);
        cell.savedText = ElementText(label);
        cell.appliedText.clear();

        try {
            cell.textChangedToken = label.RegisterPropertyChangedCallback(
                wuxc::TextBlock::TextProperty(),
                [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                    if (auto tb = sender.try_as<wuxc::TextBlock>()) {
                        void* labelId = winrt::get_abi(tb);
                        for (auto& [id, c] : t_cells) {
                            if (c.label.get() == tb) {
                                std::wstring current = ElementText(tb);
                                if (!c.appliedText.empty() && current != c.appliedText) {
                                    c.savedText = current;
                                }
                                break;
                            }
                        }
                    }
                });
        } catch (...) {}
    }

    HolidayInfo info;
    bool hasInfo = LookupHoliday(key, info);

    auto settings = GetSettings();
    std::wstring currentText = ElementText(label);
    std::wstring desired = hasInfo ? info.name : L"";

    if (settings->replaceText && hasInfo) {
        if (currentText != desired) {
            try {
                label.Text(desired);
                t_applyWrote = true;
            } catch (...) {}
        }
        cell.hadText = true;
    } else if (cell.hadText) {
        if (currentText == cell.appliedText && !cell.savedText.empty()) {
            try {
                label.Text(cell.savedText);
            } catch (...) {}
        }
        cell.hadText = false;
    }
    cell.appliedText = desired;

    if (hasInfo) {
        SaveForeground(cell, label);
        winrt::Windows::UI::Color targetColor;
        if (elems.isAccentCircle) {
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
// Lunar Header replacement

bool IsChineseLunarHeaderText(std::wstring const& text) {
    std::wstring value = text;
    if (value.rfind(L"农历", 0) == 0 || value.rfind(L"農曆", 0) == 0) {
        value.erase(0, 2);
    }

    const size_t monthEnd = value.find(L'月');
    if (monthEnd == std::wstring::npos || monthEnd == 0 || monthEnd > 2 ||
        monthEnd + 1 >= value.size()) {
        return false;
    }

    const std::wstring month = value.substr(0, monthEnd);
    const std::wstring monthChars = L"正一二三四五六七八九十冬腊臘";
    size_t monthStart = 0;
    if (month.size() == 2 && (month[0] == L'闰' || month[0] == L'閏')) {
        monthStart = 1;
    } else if (month.size() != 1) {
        return false;
    }
    if (monthChars.find(month[monthStart]) == std::wstring::npos) {
        return false;
    }

    const std::wstring day = value.substr(monthEnd + 1);
    if (day.size() == 2 && day[0] == L'初') {
        return std::wstring(L"一二三四五六七八九十").find(day[1]) != std::wstring::npos;
    }
    if (day == L"十" || day == L"二十" || day == L"三十") {
        return true;
    }
    if (day.size() == 2 && (day[0] == L'十' || day[0] == L'廿')) {
        return std::wstring(L"一二三四五六七八九").find(day[1]) != std::wstring::npos;
    }
    return false;
}

struct LunarHeaderState {
    winrt::weak_ref<wuxc::TextBlock> label;
    std::wstring savedText;
    std::wstring appliedText;
};

thread_local std::unordered_map<void*, LunarHeaderState> t_lunarHeaders;

constexpr size_t kMaxScannedElements = 16384;
constexpr int kMaxScanDepth = 96;

void ApplyLunarHeaderTextInTree(wux::DependencyObject const& root,
                                int32_t lunarDateKey,
                                size_t& budget,
                                int depth) {
    if (!root || budget == 0 || depth > kMaxScanDepth) return;
    --budget;

    if (root.try_as<wuxc::CalendarViewDayItem>()) return;

    if (auto textBlock = root.try_as<wuxc::TextBlock>()) {
        const std::wstring current = ElementText(textBlock);
        void* id = winrt::get_abi(textBlock);
        auto it = t_lunarHeaders.find(id);
        const bool tracked = it != t_lunarHeaders.end() && it->second.label.get() == textBlock;

        if (tracked && current == it->second.appliedText) {
            HolidayInfo hinfo;
            LookupHoliday(lunarDateKey, hinfo);
            const std::wstring desired = hinfo.headerText;
            if (current != desired) {
                try {
                    textBlock.Text(desired);
                    it->second.appliedText = desired;
                    t_applyWrote = true;
                } catch (...) {}
            }
        } else if (IsChineseLunarHeaderText(current)) {
            LunarHeaderState& state = t_lunarHeaders[id];
            if (!tracked) {
                state = {};
                state.label = winrt::make_weak(textBlock);
            }
            state.savedText = current;
            HolidayInfo hinfo;
            LookupHoliday(lunarDateKey, hinfo);
            const std::wstring desired = hinfo.headerText;
            try {
                textBlock.Text(desired);
                state.appliedText = desired;
                t_applyWrote = true;
            } catch (...) {}
        } else if (tracked) {
            t_lunarHeaders.erase(it);
        }
    }

    int count = 0;
    try {
        count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int childIndex = 0; childIndex < count && budget > 0; childIndex++) {
        wux::DependencyObject child = nullptr;
        try {
            child = wuxm::VisualTreeHelper::GetChild(root, childIndex);
        } catch (...) {
            continue;
        }
        ApplyLunarHeaderTextInTree(child, lunarDateKey, budget, depth + 1);
    }
}

void UninitializeLunarHeaderTextOnCurrentThread() {
    for (auto& entry : t_lunarHeaders) {
        auto label = entry.second.label.get();
        if (!label) continue;
        try {
            if (!entry.second.appliedText.empty() && ElementText(label) == entry.second.appliedText) {
                label.Text(entry.second.savedText);
            }
        } catch (...) {}
    }
    t_lunarHeaders.clear();
}

void ApplyLunarHeaderTextOnCurrentThread() {
    if (!GetSettings()->replaceText) {
        UninitializeLunarHeaderTextOnCurrentThread();
        return;
    }
    wux::Window window = nullptr;
    try {
        window = wux::Window::Current();
    } catch (...) {
        return;
    }
    if (!window) return;

    SYSTEMTIME today{};
    GetLocalTime(&today);
    const int32_t todayDate = MakeDateKey(today.wYear, today.wMonth, today.wDay);

    size_t budget = kMaxScannedElements;
    if (auto content = window.Content()) {
        ApplyLunarHeaderTextInTree(content, todayDate, budget, 0);
    }
    try {
        for (auto popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
            if (budget == 0) break;
            if (auto child = popup.Child()) {
                ApplyLunarHeaderTextInTree(child, todayDate, budget, 0);
            }
        }
    } catch (...) {}
}

////////////////////////////////////////////////////////////////////////////////
// Window Discovery and Sweep

thread_local std::vector<winrt::weak_ref<wuxc::CalendarViewDayItem>> t_trackedItems;
thread_local wuc::CoreDispatcher t_dispatcher{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer t_sweepTimer{nullptr};
thread_local int t_quietPasses = 0;
thread_local bool t_windowHooked = false;
thread_local bool t_subscribedEvents = false;
thread_local bool t_loggedNoWindow = false;

void TrackDayItem(wuxc::CalendarViewDayItem const& item) {
    for (const auto& tracked : t_trackedItems) {
        if (tracked.get() == item) return;
    }
    t_trackedItems.push_back(winrt::make_weak(item));
}

void CollectDayItemsFromElement(wux::DependencyObject const& root,
                                size_t& budget,
                                int depth) {
    if (!root || budget == 0 || depth > kMaxScanDepth) return;
    budget--;

    if (auto dayItem = root.try_as<wuxc::CalendarViewDayItem>()) {
        TrackDayItem(dayItem);
        return;
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
        for (auto popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
            if (budget == 0) break;
            if (auto child = popup.Child()) {
                CollectDayItemsFromElement(child, budget, 0);
            }
        }
    } catch (...) {}
}

void ApplyDayItemsOnCurrentThread() {
    CollectDayItemsOnCurrentThread();
    for (auto it = t_trackedItems.begin(); it != t_trackedItems.end();) {
        auto item = it->get();
        if (!item) {
            it = t_trackedItems.erase(it);
            continue;
        }
        ApplyToDayItem(item);
        ++it;
    }
}

void SweepDayItems() {
    t_applyWrote = false;
    ApplyDayItemsOnCurrentThread();
    ApplyLunarHeaderTextOnCurrentThread();
}

void StartSweepTimer();
void StopSweepTimer() {
    if (t_sweepTimer) {
        try {
            t_sweepTimer.Stop();
        } catch (...) {}
        t_sweepTimer = nullptr;
    }
}

void StartSweepTimer() {
    if (t_sweepTimer) return;
    try {
        auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (!queue) return;
        t_sweepTimer = queue.CreateTimer();
        t_sweepTimer.Interval(std::chrono::milliseconds(1000));
        t_quietPasses = 0;
        t_sweepTimer.Tick([](auto const& sender, auto const&) {
            SweepDayItems();
            if (t_applyWrote) {
                t_quietPasses = 0;
            } else {
                t_quietPasses++;
                if (t_quietPasses >= 5) {
                    try { sender.Stop(); } catch (...) {}
                    t_sweepTimer = nullptr;
                }
            }
        });
        t_sweepTimer.Start();
    } catch (...) {}
}

void HookWindowEvents(wuc::CoreWindow const& coreWindow) {
    if (t_windowHooked) return;
    t_windowHooked = true;

    try {
        coreWindow.Activated([](auto const&, wuc::WindowActivatedEventArgs const& args) {
            if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated) {
                SweepDayItems();
                StartSweepTimer();
            }
        });
        coreWindow.VisibilityChanged([](auto const&, wuc::VisibilityChangedEventArgs const& args) {
            if (args.Visible()) {
                SweepDayItems();
                StartSweepTimer();
            }
        });
    } catch (...) {}
}

void OnCalendarOpenedOnCurrentThread() {
    wux::Window window = nullptr;
    try {
        window = wux::Window::Current();
    } catch (...) {
        return;
    }
    if (!window) return;

    try {
        auto coreWindow = window.CoreWindow();
        if (coreWindow) HookWindowEvents(coreWindow);
    } catch (...) {}

    SweepDayItems();
    StartSweepTimer();
}

void UninitializeDayItemsOnCurrentThread() {
    for (auto& [id, cell] : t_cells) {
        if (auto label = cell.label.get()) {
            if (cell.textChangedToken) {
                try {
                    label.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::TextProperty(), cell.textChangedToken);
                } catch (...) {}
                cell.textChangedToken = 0;
            }
            if (!cell.savedText.empty() && ElementText(label) == cell.appliedText) {
                try { label.Text(cell.savedText); } catch (...) {}
            }
            if (cell.hadColor) {
                RestoreForeground(label, cell);
                cell.hadColor = false;
            }
        }
    }
    t_cells.clear();
}

void UninitializeForCurrentThread() {
    Wh_Log(L"Uninitializing on thread %u", GetCurrentThreadId());
    StopSweepTimer();
    UninitializeDayItemsOnCurrentThread();
    UninitializeLunarHeaderTextOnCurrentThread();
    t_trackedItems.clear();
    t_windowHooked = false;
    t_subscribedEvents = false;
    t_loggedNoWindow = false;
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) return;
    g_initializedForThread = true;
    try {
        wux::Window window = wux::Window::Current();
        if (window) {
            t_dispatcher = window.Dispatcher();
        }
    } catch (...) {}
}

////////////////////////////////////////////////////////////////////////////////
// Cross-thread execution helpers

UINT g_runFromWindowThreadMsg = 0;
UINT g_runFromWindowThreadViaPostMessageMsg = 0;

void RunFromWindowThread(HWND hWnd, void (*callback)(void*), void* param) {
    if (!hWnd || !IsWindow(hWnd)) return;
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == GetCurrentThreadId()) {
        callback(param);
        return;
    }
    SendMessage(hWnd, g_runFromWindowThreadMsg, (WPARAM)callback, (LPARAM)param);
}

UINT RunFromWindowThreadViaPostMessageMsg() {
    if (!g_runFromWindowThreadViaPostMessageMsg) {
        g_runFromWindowThreadViaPostMessageMsg =
            RegisterWindowMessage(L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);
    }
    return g_runFromWindowThreadViaPostMessageMsg;
}

struct PendingPostHook {
    HHOOK hHook;
    DWORD threadId;
};
std::mutex g_pendingPostHooksMutex;
std::vector<PendingPostHook> g_pendingPostHooks;

void UnhookPendingPostHooks() {
    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    for (auto& item : g_pendingPostHooks) {
        UnhookWindowsHookEx(item.hHook);
    }
    g_pendingPostHooks.clear();
}

void RunFromWindowThreadViaPostMessage(HWND hWnd, void (*callback)(void*), void* param) {
    if (!hWnd || !IsWindow(hWnd)) return;
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == GetCurrentThreadId()) {
        callback(param);
        return;
    }

    struct CallbackInfo {
        void (*cb)(void*);
        void* p;
        HHOOK hHook;
    };
    auto info = new CallbackInfo{callback, param, nullptr};

    info->hHook = SetWindowsHookEx(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            auto msg = reinterpret_cast<MSG*>(lParam);
            if (nCode >= 0 && msg && msg->message == RunFromWindowThreadViaPostMessageMsg()) {
                auto ci = reinterpret_cast<CallbackInfo*>(msg->lParam);
                if (ci) {
                    ci->cb(ci->p);
                    UnhookWindowsHookEx(ci->hHook);
                    {
                        std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
                        std::erase_if(g_pendingPostHooks, [h = ci->hHook](auto& item) {
                            return item.hHook == h;
                        });
                    }
                    delete ci;
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr,
        threadId);

    if (info->hHook) {
        {
            std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
            g_pendingPostHooks.push_back({info->hHook, threadId});
        }
        PostMessage(hWnd, RunFromWindowThreadViaPostMessageMsg(), 0, (LPARAM)info);
    } else {
        delete info;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window hooks

std::atomic<bool> g_shuttingDown = false;

bool IsCalendarWindowClass(PCWSTR className) {
    if (!className) return false;
    if (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
        _wcsicmp(className, L"ControlCenterWindow") == 0) {
        return true;
    }
    return false;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName) {
    if (g_shuttingDown) return;
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName && IsCalendarWindowClass(lpClassName)) {
        RunFromWindowThreadViaPostMessage(
            hWnd,
            [](void*) {
                InitializeForCurrentThread();
                OnCalendarOpenedOnCurrentThread();
            },
            nullptr);
    }
}

void OnWindowShown(HWND hWnd) {
    if (g_shuttingDown) return;
    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) > 0 &&
        IsCalendarWindowClass(szClassName)) {
        RunFromWindowThreadViaPostMessage(
            hWnd,
            [](void*) {
                InitializeForCurrentThread();
                OnCalendarOpenedOnCurrentThread();
            },
            nullptr);
    }
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
                                           LPVOID lpParam,
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
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName,
                                           dwStyle, X, Y, nWidth, nHeight,
                                           hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hWnd) OnWindowCreated(hWnd, lpClassName);
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
                                             LPVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlag);
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
                                      LPVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlag) {
    HWND hWnd = CreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName,
                                             dwStyle, X, Y, nWidth, nHeight,
                                             hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlag);
    if (hWnd) OnWindowCreated(hWnd, lpClassName);
    return hWnd;
}

using ShowWindow_t = BOOL(WINAPI*)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    BOOL wasVisible = ShowWindow_Original(hWnd, nCmdShow);
    if (nCmdShow != SW_HIDE) {
        OnWindowShown(hWnd);
    }
    return wasVisible;
}

std::vector<HWND> GetCalendarWindows() {
    std::vector<HWND> hWnds;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto out = reinterpret_cast<std::vector<HWND>*>(lParam);
            DWORD pid = 0;
            if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId()) {
                WCHAR szClassName[64];
                if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) > 0 &&
                    IsCalendarWindowClass(szClassName)) {
                    out->push_back(hWnd);
                }
            }
            return TRUE;
        },
        (LPARAM)&hWnds);
    return hWnds;
}

void RefreshCalendarOnUiThreads() {
    for (HWND hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(
            hCoreWnd,
            [](void*) {
                SweepDayItems();
                StartSweepTimer();
            },
            nullptr);
    }
}

template <typename Prototype>
bool InstallHook(HMODULE module,
                 PCSTR functionName,
                 Prototype* hook,
                 Prototype** original) {
    if (!module) return false;
    Prototype* target = reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));
    if (!target) return false;
    return WindhawkUtils::SetFunctionHook(target, hook, original);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

enum class Target { ShellExperienceHost, ShellHost };
Target g_target;

BOOL Wh_ModInit() {
    Wh_Log(L">");
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
    g_runFromWindowThreadMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    RunFromWindowThreadViaPostMessageMsg();

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    InstallHook(user32Module, "CreateWindowInBand", CreateWindowInBand_Hook,
                &CreateWindowInBand_Original);
    InstallHook(user32Module, "CreateWindowInBandEx",
                CreateWindowInBandEx_Hook, &CreateWindowInBandEx_Original);
    InstallHook(user32Module, "ShowWindow", ShowWindow_Hook, &ShowWindow_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    for (auto hCoreWnd : GetCalendarWindows()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd,
            [](void*) {
                InitializeForCurrentThread();
                OnCalendarOpenedOnCurrentThread();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_shuttingDown = true;

    for (auto hCoreWnd : GetCalendarWindows()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](void*) { UninitializeForCurrentThread(); }, nullptr);
    }

    UnhookPendingPostHooks();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    RefreshCalendarOnUiThreads();
}
