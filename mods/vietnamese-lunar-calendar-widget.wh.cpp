// ==WindhawkMod==
// @id              vietnamese-lunar-calendar-widget
// @name            Vietnamese Lunar Calendar Widget
// @name:vi-VN      Widget Lịch Âm Việt Nam & Tử Vi
// @description     Vietnamese lunar calendar and zodiac taskbar widget with an interactive horoscope popup
// @description:vi-VN Widget Âm lịch & Con Giáp trên Taskbar kèm popup Tử Vi có hiệu ứng nền mờ
// @version         1.0.15
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -lgdi32 -luser32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vietnamese Lunar Calendar Widget

Github: caotu2k5

A Vietnamese lunar calendar and zodiac widget for the primary Windows 11 taskbar,
with an interactive daily almanac and a list of Vietnamese holidays and
commemorations.

![Screenshot1](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-widget-resources/main/screenshot1.png)

Screenshot 1

![Screenshot2](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-widget-resources/main/screenshot2.png)

Screenshot 2

![Screenshot3](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-widget-resources/main/screenshot3.png)

Screenshot 3

## Features

### Taskbar widget

- **Vietnamese zodiac and Five Elements:** displays the day's zodiac animal,
  including the Cat for Mão, with a badge colored by its Nạp Âm element.
- **Lunar date:** shows the lunar day/month, leap-month marker and Can Chi year.
  The first line scales down when necessary to fit the widget width.
- **Rotating information:** the second line cycles through daily almanac
  information every **15 seconds**, including Can Chi, auspicious hours,
  suggested activities and directions.
- **Pinned holidays:** on holidays, commemorations, New Moon days and Full Moon
  days, the event name stays on the second line instead of rotating.
- **Tooltip:** hover over the widget to read the full text of both lines.

### Daily almanac and holiday popup

Click the widget to open or close the popup. Opening takes **300 ms** and closing
takes **240 ms**. Clicking again during closing reopens it.

The daily page includes the solar and lunar dates, day/month/hour Can Chi,
zodiac animal, Nạp Âm, auspicious or inauspicious day star, Trực, auspicious hours,
suggested activities, conflicting ages and travel directions.

- **◀ / ● / ▶:** previous day, today and next day.
- **List** (**Lễ** in Vietnamese): opens **13 lunar festival entries** and
  **37 solar holiday and commemoration entries**. Lunar New Year's Eve is shown
  on the actual last day of the lunar year, whether that is the 29th or 30th.
- **↩:** returns to the daily almanac.
- **Scrolling:** use the mouse wheel or vertical scrollbar to read all content.
  Navigation stays at the top. The popup remains **316 DIP** tall, shrinking only
  when the available screen space is smaller; holiday text does not enlarge it.
- **Dismissal:** click the widget again, click outside the popup or press **Esc**.

The popup follows the taskbar's light or dark theme and uses a Composition
HostBackdrop when available. If backdrop setup fails, it uses a solid background.

### Calendar and language

Lunar dates are calculated locally using the **Hồ Ngọc Đức algorithm with UTC+7**.
Calendar calculations and holiday lookup do not require an Internet connection.

English and Vietnamese labels are available. **Auto** uses Vietnamese on a
Vietnamese Windows UI and English otherwise. Traditional Can Chi and almanac
names retain their Vietnamese names.

## Settings

| Setting | Default | Description |
| --- | --- | --- |
| `language` | `auto` | System UI language, English (`en`) or Vietnamese (`vi`). |
| `enableTaskbarWidget` | `true` | Show or hide the widget. |
| `position` | `tray_left` | Widget placement; see the positions below. |
| `widgetLeftOffset` | `12` | Left offset for `root_left` only, in DIP; range 0–2500. |
| `widgetWidth` | `270` | Widget width in DIP; range 140–480. |
| `expandedWidth` | `540` | Popup width in DIP; range 320–640. |

Values outside the supported numeric ranges are clamped to the nearest limit.
DIP means device-independent pixels; Windows display scaling determines their
on-screen size.

Available positions:

- **`tray_left` — System tray, leftmost (default).**
- **`tray_before_clock` — System tray, before the clock.**
- **`tray_after_clock` — System tray, after the clock.**
- **`tray_right` — System tray, rightmost.**
- **`root_left` — Left side of the taskbar, as an overlay.**

Tray positions reserve space in the tray layout. The optional `root_left`
position overlays the taskbar and can cover Widgets, Start or pinned apps.
Adjust `widgetLeftOffset` to avoid those buttons, or choose a tray position.

## Usage and compatibility

1. Install and enable the mod from the **Windhawk catalog**.
2. Open the mod's **Settings** tab to choose a language, position and sizes.
3. Click the widget to browse the daily almanac or holiday list.

This mod requires the native Windows 11 XAML taskbar and currently adds the
widget to the **primary taskbar only**. Replacement taskbars are not supported.
It does not change the native taskbar clock, clock tooltip or Windows calendar
flyout.

If the widget does not appear, enable **Detailed debug logs** and inspect this
mod's taskbar initialization messages.

---

## Tiếng Việt

Widget hiển thị **Âm lịch, Con Giáp và Ngũ Hành** trên taskbar chính của Windows 11,
kèm popup xem thông tin Tử Vi trong ngày và danh sách ngày lễ, kỷ niệm Việt Nam.

### Tính năng

- **Con Giáp và Ngũ Hành:** hiển thị con giáp của ngày, trong đó Mão là Mèo;
  màu huy hiệu thay đổi theo Ngũ Hành Nạp Âm.
- **Dòng 1:** ngày/tháng Âm lịch, ghi chú tháng nhuận và năm Can Chi, ví dụ
  **Âm lịch: 08/09 năm Bính Ngọ**. Chữ được thu nhỏ khi cần để vừa chiều rộng.
- **Dòng 2:** luân chuyển thông tin Tử Vi mỗi **15 giây**. Khi có ngày lễ,
  kỷ niệm, Mùng 1 hoặc ngày Rằm, tên sự kiện được giữ cố định.
- **Tooltip:** di chuột lên widget để xem đầy đủ nội dung hai dòng.
- **Popup:** bấm widget để mở/đóng, với animation mở **300 ms**, đóng **240 ms**.
  Bấm lại khi đang đóng sẽ mở tiếp popup.
- **Thông tin trong ngày:** ngày Dương/Âm lịch, Can Chi ngày/tháng/giờ, Con Giáp,
  Nạp Âm, sao Hoàng Đạo/Hắc Đạo, Trực, Giờ Hoàng Đạo, việc Nên/Kỵ, tuổi xung
  và hướng xuất hành.
- **Điều hướng:** dùng **◀ / ● / ▶** để xem ngày trước, hôm nay và ngày sau.
  Nút **Lễ** mở danh sách **13 mục lễ Âm lịch** và **37 mục lễ, kỷ niệm Dương lịch**;
  nút **↩** trở về Tử Vi. Giao thừa được xác định theo ngày cuối năm Âm lịch thực tế.
- **Cuộn nội dung:** popup cao **316 DIP**, có thanh cuộn dọc và hàng nút cố định.
  Khung không phình lên khi có ngày lễ; chỉ thu nhỏ nếu màn hình không đủ chỗ.
  Cuộn chuột hoặc kéo thanh cuộn để đọc hết nội dung.
- **Đóng popup:** bấm lại widget, bấm ra ngoài hoặc nhấn **Esc**.

Popup theo giao diện sáng/tối của taskbar, dùng nền mờ **Composition HostBackdrop**
khi khả dụng và nền màu đặc nếu không thiết lập được hiệu ứng nền mờ.
Phần tính lịch dùng thuật toán **Hồ Ngọc Đức, UTC+7**, không cần kết nối Internet.

### Cài đặt và sử dụng

Cài và bật mod từ **danh mục Windhawk**, sau đó mở tab **Settings**:

| Cài đặt | Mặc định | Ý nghĩa |
| --- | --- | --- |
| `language` | `auto` | Theo ngôn ngữ Windows, tiếng Anh (`en`) hoặc tiếng Việt (`vi`). |
| `enableTaskbarWidget` | `true` | Bật hoặc ẩn widget. |
| `position` | `tray_left` | Vị trí widget trên taskbar. |
| `widgetLeftOffset` | `12` | Lề trái của vị trí `root_left`, từ 0–2500 DIP. |
| `widgetWidth` | `270` | Chiều rộng widget, từ 140–480 DIP. |
| `expandedWidth` | `540` | Chiều rộng popup, từ 320–640 DIP. |

Giá trị ngoài giới hạn được đưa về mức gần nhất. Kích thước DIP được điều chỉnh
theo tỉ lệ hiển thị của Windows.

Bốn vị trí khay hệ thống gồm **ngoài cùng bên trái** (`tray_left`, mặc định),
**trước đồng hồ** (`tray_before_clock`), **sau đồng hồ** (`tray_after_clock`) và
**ngoài cùng bên phải** (`tray_right`). Các vị trí này dành không gian riêng
cho widget trong khay hệ thống.

Vị trí **góc trái taskbar** (`root_left`) phủ lên taskbar và có thể che nút
Widgets, Start hoặc ứng dụng đã ghim. Hãy điều chỉnh `widgetLeftOffset` để tránh
che nút, hoặc chọn một vị trí trong khay hệ thống.

Chế độ **Auto** dùng tiếng Việt nếu giao diện Windows là tiếng Việt, và tiếng Anh
trong các trường hợp khác. Tên Can Chi và một số thuật ngữ Tử Vi giữ nguyên tiếng Việt.

Mod chỉ hỗ trợ **taskbar XAML gốc của Windows 11**, hiện hiển thị trên
**taskbar chính**, không sửa đồng hồ, tooltip đồng hồ hoặc khay Lịch của Windows.
Nếu widget chưa xuất hiện, bật **Detailed debug logs** để xem thông báo khởi tạo
taskbar của mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Display language
  $name:vi-VN: Ngôn ngữ hiển thị
  $options:
  - auto: System default (Auto)
  - en: English
  - vi: Tiếng Việt (Vietnamese)
  $options:vi-VN:
  - auto: Mặc định hệ thống (Tự động)
  - en: Tiếng Anh (English)
  - vi: Tiếng Việt
- enableTaskbarWidget: true
  $name: Enable taskbar Lunar & Zodiac widget
  $name:vi-VN: Bật Widget Âm lịch & Con Giáp trên thanh Taskbar
- position: tray_left
  $name: Widget position on taskbar
  $name:vi-VN: Vị trí Widget trên thanh Taskbar
  $options:
  - root_left: Left side of taskbar (RootGrid overlay)
  - tray_left: System tray - Leftmost
  - tray_before_clock: System tray - Before clock
  - tray_after_clock: System tray - After clock
  - tray_right: System tray - Rightmost
  $options:vi-VN:
  - root_left: Góc trái thanh Taskbar (Đè lên RootGrid)
  - tray_left: Khay hệ thống - Ngoài cùng bên trái
  - tray_before_clock: Khay hệ thống - Trước đồng hồ
  - tray_after_clock: Khay hệ thống - Sau đồng hồ
  - tray_right: Khay hệ thống - Ngoài cùng bên phải
- widgetLeftOffset: 12
  $name: Left offset when placed on left side of taskbar (DIP)
  $name:vi-VN: Khoảng cách lề trái khi đặt ở góc trái Taskbar (DIP)
  $description: Increase this value (e.g. 220) if you have a media player or Start button on the far left of the taskbar.
  $description:vi-VN: "Tăng giá trị này (ví dụ 220) nếu góc trái Taskbar của bạn đã có trình phát nhạc hoặc nút Widgets."
- widgetWidth: 270
  $name: Taskbar widget width (DIP)
  $name:vi-VN: Chiều rộng của Widget trên Taskbar (DIP)
  $description: "Allowed range: 140–480 DIP. Values outside this range are clamped."
  $description:vi-VN: "Giới hạn: 140–480 DIP. Giá trị ngoài giới hạn được đưa về mức gần nhất."
- expandedWidth: 540
  $name: Horoscope popup width (DIP)
  $name:vi-VN: Chiều rộng Popup Tử Vi (DIP)
  $description: "Allowed range: 320–640 DIP. Values outside this range are clamped."
  $description:vi-VN: "Giới hạn: 320–640 DIP. Giá trị ngoài giới hạn được đưa về mức gần nhất."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


#undef GetCurrentTime

#include <Unknwn.h>
#include <inspectable.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.System.h>
// Button.Click is inherited from ButtonBase; its projected definition is here.
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;
namespace wcomp = winrt::Windows::UI::Composition;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// COM interface for attaching DesktopWindowXamlSource to a Win32 HWND
struct IDesktopWindowXamlSourceNative : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE AttachToWindow(HWND parentWnd) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* hWnd) = 0;
};
__CRT_UUID_DECL(IDesktopWindowXamlSourceNative,
                0x3cbcf1bf, 0x2f76, 0x4e9c, 0x96, 0xab, 0xe8, 0x4b, 0x37, 0x97, 0x25, 0x54);

std::atomic<bool> g_shuttingDown{false};


////////////////////////////////////////////////////////////////////////////////
// Settings & Localization

enum class ModLanguage { Auto, English, Vietnamese };

struct ModSettings {
    ModLanguage language = ModLanguage::Auto;
    bool enableTaskbarWidget = true;
    std::wstring position = L"tray_left";
    int widgetLeftOffset = 12;
    int widgetWidth = 270;
    int expandedWidth = 540;
};

std::mutex g_settingsMutex;
ModSettings g_settings;

template <typename T>
T ClampVal(T val, T minVal, T maxVal) {
    return std::max(minVal, std::min(maxVal, val));
}

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

    WindhawkUtils::StringSetting pos = WindhawkUtils::StringSetting::make(L"position");
    if (*pos.get()) {
        s.position = pos.get();
    }

    s.enableTaskbarWidget = Wh_GetIntSetting(L"enableTaskbarWidget") != 0;
    s.widgetLeftOffset = ClampVal(Wh_GetIntSetting(L"widgetLeftOffset"), 0, 2500);
    s.widgetWidth = ClampVal(Wh_GetIntSetting(L"widgetWidth"), 140, 480);
    s.expandedWidth = ClampVal(Wh_GetIntSetting(L"expandedWidth"), 320, 640);

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

void OffsetSystemDateByDays(SYSTEMTIME& st, int deltaDays) {
    FILETIME ft{};
    if (!SystemTimeToFileTime(&st, &ft)) return;
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    LONGLONG ticksPerDay = 864000000000LL;
    uli.QuadPart = static_cast<ULONGLONG>(static_cast<LONGLONG>(uli.QuadPart) + deltaDays * ticksPerDay);
    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;
    FileTimeToSystemTime(&ft, &st);
}

////////////////////////////////////////////////////////////////////////////////
// Pure Arithmetic Vietnamese Lunar Calendar (Hồ Ngọc Đức Algorithm, UTC+7)

const PCWSTR CAN_10[] = {
    L"Giáp", L"Ất", L"Bính", L"Đinh", L"Mậu", L"Kỷ", L"Canh", L"Tân", L"Nhâm", L"Quý"
};
const PCWSTR CHI_12[] = {
    L"Tý", L"Sửu", L"Dần", L"Mão", L"Thìn", L"Tỵ", L"Ngọ", L"Mùi", L"Thân", L"Dậu", L"Tuất", L"Hợi"
};

// Vietnamese 12 Zodiac Animals (Mão = Cat 🐈)
const PCWSTR ZODIAC_EMOJI_12[] = {
    L"🐀", L"🐂", L"🐯", L"🐈", L"🐉", L"🐍", L"🐴", L"🐐", L"🐒", L"🐓", L"🐕", L"🐖"
};

const PCWSTR ZODIAC_VI_NAME_12[] = {
    L"Chuột", L"Trâu", L"Hổ", L"Mèo", L"Rồng", L"Rắn", L"Ngựa", L"Dê", L"Khỉ", L"Gà", L"Chó", L"Lợn"
};

const PCWSTR ZODIAC_EN_NAME_12[] = {
    L"Rat", L"Buffalo", L"Tiger", L"Cat", L"Dragon", L"Snake", L"Horse", L"Goat", L"Monkey", L"Rooster", L"Dog", L"Pig"
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

////////////////////////////////////////////////////////////////////////////////
// Holidays & Horoscope Data Tables

// One shared catalog for the compact widget, daily popup and holiday-list page.
// Sources (annual dates, not a leave / substitute-holiday schedule):
// https://doanthanhnien.vn/danh-muc/gioi-thieu-hoi-sinh-vien-viet-nam
// https://baochinhphu.vn/chuong-trinh-nghe-thuat-duong-len-phia-truoc-chao-mung-ngay-thanh-lap-dang-cong-san-viet-nam-102260203224622576.htm
// https://baochinhphu.vn/ky-niem-135-nam-ngay-sinh-chu-tich-ho-chi-minh-lanh-dao-dang-nha-nuoc-vao-lang-vieng-bac-10225051909201906.htm
// https://vnpa.moh.gov.vn/chuc-mung-ngay-thay-thuoc-viet-nam-27-02/
// https://cov.gov.vn/tin-tuc/ngay-sach-va-van-hoa-doc-viet-nam-168847.html
// https://baochinhphu.vn/ngay-bao-chi-cach-mang-viet-nam-21-6.html
// https://baochinhphu.vn/ngay-gia-dinh-viet-nam.html
// https://pbgdpl.cantho.gov.vn/ngay-truyen-thong-hoi-lien-hiep-thanh-nien-viet-nam-15-thang-10
// https://mattran.org.vn/hoat-dong/thong-bao-ve-viec-khong-to-chuc-tiep-khach-nhan-hoa-chuc-mung-nhan-dip-ky-niem-95-nam-ngay-truyen-thong-mat-tran-to-quoc-viet-nam-18111930-18112025-67598.html
// https://thanglong.chinhphu.vn/le-hoi-ky-niem-236-nam-chien-thang-ngoc-hoi-dong-da-103250202152726341.htm
// https://vi.wikipedia.org/wiki/C%C3%A1c_ng%C3%A0y_l%E1%BB%85_%E1%BB%9F_Vi%E1%BB%87t_Nam
struct HolidayEntry {
    int day; // 0 = last day of lunar year, which can be 29 or 30.
    int month;
    PCWSTR viName;
    PCWSTR enName;
};

static const HolidayEntry kSolarHolidays[] = {
    { 1,  1, L"Tết Dương lịch", L"New Year's Day" },
    { 9,  1, L"Ngày truyền thống Học sinh, Sinh viên Việt Nam", L"Vietnamese Students' Day" },
    { 3,  2, L"Ngày thành lập Đảng Cộng sản Việt Nam", L"Communist Party of Vietnam Founding Day" },
    { 14, 2, L"Lễ Tình nhân", L"Valentine's Day" },
    { 27, 2, L"Ngày Thầy thuốc Việt Nam", L"Vietnamese Doctors' Day" },
    { 3,  3, L"Ngày truyền thống Bộ đội Biên phòng", L"Vietnam Border Guard Day" },
    { 8,  3, L"Quốc tế Phụ nữ", L"International Women's Day" },
    { 26, 3, L"Ngày thành lập Đoàn TNCS Hồ Chí Minh", L"Ho Chi Minh Communist Youth Union Founding Day" },
    { 27, 3, L"Ngày Thể thao Việt Nam", L"Vietnam Sports Day" },
    { 21, 4, L"Ngày Sách và Văn hóa đọc Việt Nam", L"Vietnam Book and Reading Culture Day" },
    { 30, 4, L"Giải phóng miền Nam, thống nhất đất nước", L"Reunification Day" },
    { 1,  5, L"Quốc tế Lao động", L"International Workers' Day" },
    { 7,  5, L"Ngày chiến thắng Điện Biên Phủ", L"Dien Bien Phu Victory Day" },
    { 15, 5, L"Ngày thành lập Đội TNTP Hồ Chí Minh", L"Ho Chi Minh Young Pioneer Organization Founding Day" },
    { 19, 5, L"Ngày sinh Chủ tịch Hồ Chí Minh", L"President Ho Chi Minh's Birthday" },
    { 1,  6, L"Quốc tế Thiếu nhi", L"International Children's Day" },
    { 5,  6, L"Ngày Bác Hồ ra đi tìm đường cứu nước", L"Ho Chi Minh's Departure to Seek National Salvation" },
    { 21, 6, L"Ngày Báo chí Cách mạng Việt Nam", L"Vietnam Revolutionary Press Day" },
    { 28, 6, L"Ngày Gia đình Việt Nam", L"Vietnamese Family Day" },
    { 27, 7, L"Ngày Thương binh - Liệt sĩ", L"War Invalids and Martyrs' Day" },
    { 28, 7, L"Ngày thành lập Công đoàn Việt Nam", L"Vietnam Trade Union Founding Day" },
    { 19, 8, L"Cách mạng Tháng Tám thành công", L"August Revolution Day" },
    { 19, 8, L"Ngày truyền thống Công an nhân dân", L"People's Public Security Traditional Day" },
    { 2,  9, L"Quốc khánh Việt Nam", L"Vietnam National Day" },
    { 10,10, L"Ngày Giải phóng Thủ đô", L"Hanoi Liberation Day" },
    { 13,10, L"Ngày Doanh nhân Việt Nam", L"Vietnamese Entrepreneurs' Day" },
    { 15,10, L"Ngày truyền thống Hội Liên hiệp Thanh niên Việt Nam", L"Vietnam Youth Federation Traditional Day" },
    { 20,10, L"Ngày Phụ nữ Việt Nam", L"Vietnamese Women's Day" },
    { 9, 11, L"Ngày Pháp luật Việt Nam", L"Vietnam Law Day" },
    { 18,11, L"Ngày truyền thống Mặt trận Tổ quốc Việt Nam", L"Vietnam Fatherland Front Traditional Day" },
    { 20,11, L"Ngày Nhà giáo Việt Nam", L"Vietnamese Teachers' Day" },
    { 23,11, L"Ngày Di sản văn hóa Việt Nam", L"Vietnam Cultural Heritage Day" },
    { 24,11, L"Ngày Văn hóa Việt Nam", L"Vietnam Culture Day" },
    { 19,12, L"Ngày Toàn quốc kháng chiến", L"National Resistance Day" },
    { 22,12, L"Ngày thành lập Quân đội nhân dân Việt Nam", L"Vietnam People's Army Founding Day" },
    { 24,12, L"Đêm Giáng Sinh", L"Christmas Eve" },
    { 25,12, L"Lễ Giáng Sinh", L"Christmas Day" },
};

static const HolidayEntry kLunarHolidays[] = {
    { 1,  1, L"Mùng 1 Tết Nguyên Đán", L"Lunar New Year (Day 1)" },
    { 2,  1, L"Mùng 2 Tết Nguyên Đán", L"Lunar New Year (Day 2)" },
    { 3,  1, L"Mùng 3 Tết Nguyên Đán", L"Lunar New Year (Day 3)" },
    { 5,  1, L"Lễ hội Ngọc Hồi - Đống Đa", L"Ngoc Hoi - Dong Da Festival" },
    { 15, 1, L"Tết Nguyên Tiêu (Rằm tháng Giêng)", L"First Full Moon Festival" },
    { 3,  3, L"Tết Hàn Thực", L"Cold Food Festival" },
    { 10, 3, L"Giỗ Tổ Hùng Vương", L"Hung Kings' Commemoration Day" },
    { 15, 4, L"Lễ Phật Đản", L"Buddha's Birthday" },
    { 5,  5, L"Tết Đoan Ngọ", L"Mid-Year Festival" },
    { 15, 7, L"Lễ Vu Lan", L"Vu Lan Festival" },
    { 15, 8, L"Tết Trung Thu", L"Mid-Autumn Festival" },
    { 23,12, L"Ông Công Ông Táo", L"Kitchen Guardians Day" },
    { 0, 12, L"Tất niên / Giao thừa", L"Lunar New Year's Eve" },
};

bool IsVietnameseLunarYearEnd(int year, int month, int day,
                            int lunarDay, int lunarMonth, bool leap) {
    if (leap || lunarMonth != 12 || lunarDay < 29) return false;
    static const int monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int days = monthDays[month - 1];
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) ++days;
    if (++day > days) {
        day = 1;
        if (++month > 12) { month = 1; ++year; }
    }
    int nextDay, nextMonth, nextYear;
    bool nextLeap;
    std::wstring nextCanChi;
    GetVietnameseLunarDate(year, month, day, nextDay, nextMonth, nextYear, nextLeap, nextCanChi);
    return nextDay == 1 && nextMonth == 1 && !nextLeap;
}

std::wstring GetVietnameseHolidayNames(int sDay, int sMonth,
                                      int lDay, int lMonth, bool isLeap, bool vi,
                                      bool lunarYearEnd = false) {
    std::wstring names;
    auto addName = [&](PCWSTR name) {
        if (!names.empty()) names += L" • ";
        names += name;
    };
    for (const auto& entry : kSolarHolidays) {
        if (sDay == entry.day && sMonth == entry.month)
            addName(PickText(vi, entry.viName, entry.enName));
    }
    if (!isLeap) {
        bool matchedLunarFestival = false;
        for (const auto& entry : kLunarHolidays) {
            bool matches = entry.day == 0 ? lunarYearEnd
                : (lDay == entry.day && lMonth == entry.month);
            if (matches) {
                addName(PickText(vi, entry.viName, entry.enName));
                matchedLunarFestival = true;
            }
        }
        if (!matchedLunarFestival) {
            if (lDay == 1) addName(PickText(vi, L"Mùng 1 đầu tháng (Sóc)", L"New Moon Day (Mùng 1)"));
            else if (lDay == 15) addName(PickText(vi, L"Ngày Rằm (Vọng)", L"Full Moon Day (Rằm)"));
        }
    }
    return names;
}

std::wstring BuildHolidayCatalogText(bool vi) {
    std::wstring text = PickText(vi, L"LỄ TRUYỀN THỐNG · ÂM LỊCH\n\n",
                                    L"TRADITIONAL FESTIVALS · LUNAR CALENDAR\n\n");
    auto append = [&](HolidayEntry const& entry, bool lunar) {
        if (entry.day == 0) {
            text += PickText(vi, L"Ngày cuối tháng 12 ÂL (29 hoặc 30)",
                                L"Last day of lunar month 12 (29 or 30)");
        } else {
            if (entry.day < 10) text += L"0";
            text += std::to_wstring(entry.day) + L"/";
            if (entry.month < 10) text += L"0";
            text += std::to_wstring(entry.month);
            if (lunar) text += PickText(vi, L" ÂL", L" lunar");
        }
        text += L" — ";
        text += PickText(vi, entry.viName, entry.enName);
        text += L"\n\n";
    };
    for (const auto& entry : kLunarHolidays) append(entry, true);
    text += PickText(vi, L"NGÀY LỄ & KỶ NIỆM · DƯƠNG LỊCH\n\n",
                        L"HOLIDAYS & COMMEMORATIONS · SOLAR CALENDAR\n\n");
    for (const auto& entry : kSolarHolidays) append(entry, false);
    text += PickText(vi, L"Mùng 1 và ngày Rằm: hiển thị theo từng tháng âm lịch.\n"
                        L"Các lễ âm lịch không lặp lại trong tháng nhuận.",
                        L"New Moon and Full Moon days are shown each lunar month.\n"
                        L"Lunar festivals do not repeat in leap months.");
    return text;
}


enum class FiveElement { Metal, Wood, Water, Fire, Earth };

struct ComputedHoroscope {
    int sDay = 1, sMonth = 1, sYear = 2000;
    int lDay = 1, lMonth = 1, lYear = 2000;
    bool isLeap = false;
    int canDayIdx = 0;
    int chiDayIdx = 0;
    PCWSTR weekdayStr = L"";
    PCWSTR zodiacEmoji = L"🐉";
    PCWSTR zodiacAnimalName = L"";
    FiveElement element = FiveElement::Metal;
    std::wstring hourCanChi;
    std::wstring dayCanChi;
    std::wstring monthCanChi;
    std::wstring yearCanChi;
    PCWSTR napAmStr = L"";
    PCWSTR starName = L"";
    PCWSTR starShortName = L"";
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

    info.canDayIdx = (jdn + 9) % 10;
    info.chiDayIdx = (jdn + 1) % 12;
    info.dayCanChi = std::wstring(CAN_10[info.canDayIdx]) + L" " + CHI_12[info.chiDayIdx];
    info.zodiacEmoji = ZODIAC_EMOJI_12[info.chiDayIdx];
    info.zodiacAnimalName = vi ? ZODIAC_VI_NAME_12[info.chiDayIdx] : ZODIAC_EN_NAME_12[info.chiDayIdx];

    int canMonthIdx = (info.lYear * 12 + info.lMonth + 3) % 10;
    int chiMonthIdx = (info.lMonth + 1) % 12;
    info.monthCanChi = std::wstring(CAN_10[canMonthIdx]) + L" " + CHI_12[chiMonthIdx];

    int chiHourIdx = ((hour + 1) / 2) % 12;
    int canHourIdx = ((info.canDayIdx % 5) * 2 + chiHourIdx) % 10;
    info.hourCanChi = std::wstring(CAN_10[canHourIdx]) + L" " + CHI_12[chiHourIdx];

    struct NapAmEntry { PCWSTR vi; PCWSTR en; FiveElement elem; };
    static const NapAmEntry kNapAm[30] = {
        { L"Hải Trung Kim (Vàng trong biển)",    L"Hải Trung Kim (Sea Metal)",        FiveElement::Metal },
        { L"Lô Trung Hỏa (Lửa trong lò)",        L"Lô Trung Hỏa (Furnace Fire)",      FiveElement::Fire  },
        { L"Đại Lâm Mộc (Gỗ rừng già)",          L"Đại Lâm Mộc (Forest Wood)",        FiveElement::Wood  },
        { L"Lộ Bàng Thổ (Đất ven đường)",        L"Lộ Bàng Thổ (Roadside Earth)",     FiveElement::Earth },
        { L"Kiếm Phong Kim (Vàng mũi kiếm)",     L"Kiếm Phong Kim (Sword Metal)",     FiveElement::Metal },
        { L"Sơn Đầu Hỏa (Lửa đầu núi)",          L"Sơn Đầu Hỏa (Mountain Fire)",      FiveElement::Fire  },
        { L"Giản Hạ Thủy (Nước dưới khe)",       L"Giản Hạ Thủy (Stream Water)",      FiveElement::Water },
        { L"Thành Đầu Thổ (Đất tường thành)",    L"Thành Đầu Thổ (Fortress Earth)",   FiveElement::Earth },
        { L"Bạch Lạp Kim (Vàng chân đèn)",       L"Bạch Lạp Kim (Candlestick Metal)", FiveElement::Metal },
        { L"Dương Liễu Mộc (Gỗ cây liễu)",       L"Dương Liễu Mộc (Willow Wood)",     FiveElement::Wood  },
        { L"Tuyền Trung Thủy (Nước trong suối)", L"Tuyền Trung Thủy (Spring Water)",  FiveElement::Water },
        { L"Ốc Thượng Thổ (Đất mái nhà)",        L"Ốc Thượng Thổ (Roof Earth)",       FiveElement::Earth },
        { L"Tích Lịch Hỏa (Lửa sấm sét)",        L"Tích Lịch Hỏa (Thunderbolt Fire)", FiveElement::Fire  },
        { L"Tùng Bách Mộc (Gỗ tùng bách)",       L"Tùng Bách Mộc (Pine Wood)",        FiveElement::Wood  },
        { L"Trường Lưu Thủy (Nước chảy dài)",    L"Trường Lưu Thủy (River Water)",    FiveElement::Water },
        { L"Sa Trung Kim (Vàng trong cát)",      L"Sa Trung Kim (Sand Metal)",       FiveElement::Metal },
        { L"Sơn Hạ Hỏa (Lửa dưới núi)",          L"Sơn Hạ Hỏa (Foothill Fire)",       FiveElement::Fire  },
        { L"Bình Địa Mộc (Gỗ đồng bằng)",        L"Bình Địa Mộc (Plain Wood)",        FiveElement::Wood  },
        { L"Bích Thượng Thổ (Đất tò vò)",        L"Bích Thượng Thổ (Wall Earth)",     FiveElement::Earth },
        { L"Kim Bạch Kim (Vàng pha bạc)",        L"Kim Bạch Kim (Gold Foil Metal)",   FiveElement::Metal },
        { L"Phúc Đăng Hỏa (Lửa đèn chụp)",       L"Phúc Đăng Hỏa (Lamp Fire)",        FiveElement::Fire  },
        { L"Thiên Hà Thủy (Nước ngân hà)",       L"Thiên Hà Thủy (Celestial Water)",  FiveElement::Water },
        { L"Đại Trạch Thổ (Đất nền nhà)",        L"Đại Trạch Thổ (Marsh Earth)",      FiveElement::Earth },
        { L"Thoa Xuyến Kim (Vàng trang sức)",    L"Thoa Xuyến Kim (Jewelry Metal)",   FiveElement::Metal },
        { L"Tang Đố Mộc (Gỗ cây dâu)",           L"Tang Đố Mộc (Mulberry Wood)",      FiveElement::Wood  },
        { L"Đại Khê Thủy (Nước khe lớn)",        L"Đại Khê Thủy (Rapids Water)",      FiveElement::Water },
        { L"Sa Trung Thổ (Đất pha cát)",         L"Sa Trung Thổ (Sand Earth)",        FiveElement::Earth },
        { L"Thiên Thượng Hỏa (Lửa trên trời)",   L"Thiên Thượng Hỏa (Solar Fire)",    FiveElement::Fire  },
        { L"Thạch Lựu Mộc (Gỗ cây lựu)",         L"Thạch Lựu Mộc (Pomegranate Wood)", FiveElement::Wood  },
        { L"Đại Hải Thủy (Nước biển lớn)",       L"Đại Hải Thủy (Ocean Water)",       FiveElement::Water },
    };
    int hoaGiapIdx = (6 * info.canDayIdx - 5 * info.chiDayIdx + 60) % 60;
    info.napAmStr = PickText(vi, kNapAm[hoaGiapIdx / 2].vi, kNapAm[hoaGiapIdx / 2].en);
    info.element = kNapAm[hoaGiapIdx / 2].elem;

    struct StarInfo { PCWSTR fullName; PCWSTR shortName; bool isHoangDao; };
    static const StarInfo kStars[12] = {
        { L"Thanh Long Hoàng Đạo", L"Thanh Long (Hoàng Đạo)", true  },
        { L"Minh Đường Hoàng Đạo", L"Minh Đường (Hoàng Đạo)", true  },
        { L"Thiên Hình Hắc Đạo",   L"Thiên Hình (Hắc Đạo)",   false },
        { L"Chu Tước Hắc Đạo",     L"Chu Tước (Hắc Đạo)",     false },
        { L"Kim Quỹ Hoàng Đạo",    L"Kim Quỹ (Hoàng Đạo)",    true  },
        { L"Kim Đường Hoàng Đạo",  L"Kim Đường (Hoàng Đạo)",  true  },
        { L"Bạch Hổ Hắc Đạo",      L"Bạch Hổ (Hắc Đạo)",      false },
        { L"Ngọc Đường Hoàng Đạo", L"Ngọc Đường (Hoàng Đạo)", true  },
        { L"Thiên Lao Hắc Đạo",    L"Thiên Lao (Hắc Đạo)",    false },
        { L"Nguyên Vũ Hắc Đạo",    L"Nguyên Vũ (Hắc Đạo)",    false },
        { L"Tư Mệnh Hoàng Đạo",    L"Tư Mệnh (Hoàng Đạo)",    true  },
        { L"Câu Trận Hắc Đạo",     L"Câu Trận (Hắc Đạo)",     false }
    };
    int thanhLongStartChi = ((info.lMonth - 1) % 6) * 2;
    int starIdx = (info.chiDayIdx - thanhLongStartChi + 12) % 12;
    info.starName = kStars[starIdx].fullName;
    info.starShortName = kStars[starIdx].shortName;
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
    int trucIdx = (info.chiDayIdx - chiMonthIdx + 12) % 12;
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
    info.gioHoangDao = kGioHoangDao[info.chiDayIdx % 6];

    int xungChiIdx = (info.chiDayIdx + 6) % 12;
    int xungCan1 = (info.canDayIdx + 4) % 10;
    int xungCan2 = (info.canDayIdx + 6) % 10;
    info.tuoiXung = std::wstring(CAN_10[xungCan1]) + L" " + CHI_12[xungChiIdx] +
                    L", " + CAN_10[xungCan2] + L" " + CHI_12[xungChiIdx];

    struct BilingualDir { PCWSTR vi; PCWSTR en; };
    static const BilingualDir kHuongHyThan[10] = {
        { L"Đông Bắc", L"Northeast" }, { L"Tây Bắc",  L"Northwest" },
        { L"Tây Nam",  L"Southwest" }, { L"Chính Nam", L"South" },
        { L"Đông Nam", L"Southeast" }, { L"Đông Bắc", L"Northeast" },
        { L"Tây Bắc",  L"Northwest" }, { L"Tây Nam",  L"Southwest" },
        { L"Chính Nam", L"South" },    { L"Đông Nam", L"Southeast" },
    };
    static const BilingualDir kHuongTaiThan[10] = {
        { L"Đông Nam",  L"Southeast" }, { L"Đông Nam",  L"Southeast" },
        { L"Chính Đông", L"East" },     { L"Chính Đông", L"East" },
        { L"Chính Bắc", L"North" },     { L"Chính Nam", L"South" },
        { L"Tây Nam",   L"Southwest" }, { L"Tây Nam",   L"Southwest" },
        { L"Chính Tây", L"West" },      { L"Tây Bắc",   L"Northwest" },
    };
    info.huongHyThan = PickText(vi, kHuongHyThan[info.canDayIdx].vi, kHuongHyThan[info.canDayIdx].en);
    info.huongTaiThan = PickText(vi, kHuongTaiThan[info.canDayIdx].vi, kHuongTaiThan[info.canDayIdx].en);

    info.specialHoliday = GetVietnameseHolidayNames(
        sDay, sMonth, info.lDay, info.lMonth, info.isLeap, vi,
        IsVietnameseLunarYearEnd(sYear, sMonth, sDay, info.lDay, info.lMonth, info.isLeap));

    return info;
}

// Compact widget presentation: independent from XAML so timing can be tested.
constexpr uint64_t kHoroscopeRotationMs = 15000;

struct WidgetRotationState {
    bool initialized = false;
    int dateKey = 0;
    bool vietnamese = true;
    bool holiday = false;
    size_t rowCount = 0;
    uint64_t startedAt = 0;

    size_t Select(int today, bool vi, bool pinnedHoliday, size_t count, uint64_t now) {
        if (!initialized || dateKey != today || vietnamese != vi ||
            holiday != pinnedHoliday || rowCount != count || now < startedAt) {
            initialized = true;
            dateKey = today;
            vietnamese = vi;
            holiday = pinnedHoliday;
            rowCount = count;
            startedAt = now;
        }
        if (pinnedHoliday || count == 0) return 0;
        return static_cast<size_t>(((now - startedAt) / kHoroscopeRotationMs) % count);
    }
};

std::wstring FormatWidgetLunarHeader(ComputedHoroscope const& h, bool vi) {
    auto twoDigits = [](int n) { return (n < 10 ? L"0" : L"") + std::to_wstring(n); };
    std::wstring text = vi ? L"Âm lịch: " : L"Lunar: ";
    text += twoDigits(h.lDay) + L"/" + twoDigits(h.lMonth);
    if (h.isLeap) text += vi ? L" nhuận" : L" (leap)";
    text += vi ? L" năm " : L" year of ";
    return text + h.yearCanChi;
}

void AppendWidgetListRows(std::vector<std::wstring>& rows, PCWSTR prefix, PCWSTR list) {
    if (!list || !*list) return;
    std::wstring text = list;
    size_t start = 0;
    while (start < text.size()) {
        size_t end = text.find(L',', start);
        std::wstring item = TrimWhitespace(text.substr(start, end - start));
        if (!item.empty()) rows.push_back(std::wstring(prefix) + item);
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
}

std::vector<std::wstring> BuildWidgetHoroscopeRows(ComputedHoroscope const& h, bool vi) {
    std::vector<std::wstring> rows;
    rows.push_back(std::wstring(h.zodiacEmoji) + (vi ? L" Ngày " : L" Day ") + h.dayCanChi);
    rows.push_back(std::wstring(vi ? L"📅 Tháng " : L"📅 Month ") + h.monthCanChi);
    rows.push_back(std::wstring(vi ? L"🕒 Giờ " : L"🕒 Hour ") + h.hourCanChi);
    rows.push_back(std::wstring(vi ? L"☯ Mệnh: " : L"☯ Element: ") + h.napAmStr);
    rows.push_back(std::wstring(h.isHoangDao ? L"🌟 " : L"⚪ ") +
                   (vi ? h.starShortName : (h.isHoangDao ? L"Auspicious day" : L"Inauspicious day")));
    rows.push_back(std::wstring(vi ? L"📜 Trực: " : L"📜 Officer: ") + h.trucName);
    AppendWidgetListRows(rows, vi ? L"⏰ Giờ tốt: " : L"⏰ Good hour: ", h.gioHoangDao);
    AppendWidgetListRows(rows, vi ? L"🔮 Nên: " : L"🔮 Good for: ", h.goodFor);
    AppendWidgetListRows(rows, vi ? L"⛔ Kỵ: " : L"⛔ Avoid: ", h.badFor);
    AppendWidgetListRows(rows, vi ? L"🧭 Tuổi xung: " : L"🧭 Clash: ", h.tuoiXung.c_str());
    rows.push_back(std::wstring(vi ? L"🧭 Hỷ thần: " : L"🧭 Joy: ") + h.huongHyThan);
    rows.push_back(std::wstring(vi ? L"💰 Tài thần: " : L"💰 Wealth: ") + h.huongTaiThan);
    return rows;
}

std::wstring SelectWidgetSecondLine(ComputedHoroscope const& h, bool vi,
                                    uint64_t now, WidgetRotationState& state) {
    int today = MakeDateKey(h.sYear, h.sMonth, h.sDay);
    // Pin holiday and event names instead of rotating the almanac rows.
    if (!h.specialHoliday.empty()) {
        state.Select(today, vi, true, 1, now);
        return L"🎉 " + h.specialHoliday;
    }
    auto rows = BuildWidgetHoroscopeRows(h, vi);
    size_t index = state.Select(today, vi, false, rows.size(), now);
    return rows.empty() ? std::wstring{} : rows[index];
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar.dll XAML Injection & DesktopWindowXamlSource Popup (explorer.exe)

void* CTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t = void**(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using Std_Ref_Decref_t = void(WINAPI*)(void* pThis);
Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;

std::atomic<bool> g_taskbarDllHookAttempted{false};
std::atomic<bool> g_taskbarDllHooked{false};

void RequestTaskbarWidgetInjection();
using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    // The HWND can exist before its XAML tree. Queue work after startup returns.
    if (!g_shuttingDown) RequestTaskbarWidgetInjection();
}

// Called on the taskbar UI thread; repeated status messages are rate-limited.
void ReportWidgetStatus(PCWSTR status) {
    static std::wstring lastStatus;
    static ULONGLONG lastTime = 0;
    ULONGLONG now = GetTickCount64();
    if (lastStatus != status || now - lastTime >= 30000) {
        Wh_Log(L"%s", status);
        lastStatus = status;
        lastTime = now;
    }
}

bool HookTaskbarDllSymbols(HMODULE module) {
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const)",
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void) const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void) const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
            },
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const)",
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void) const)",
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )",
            },
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {
                LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))",
            },
            &Std_Ref_Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
            true, // Optional: CreateWindowExW and AfterInit also schedule injection.
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Taskbar.dll symbol resolution failed. Check symbol download/build compatibility.");
        return false;
    }
    Wh_Log(L"Taskbar.dll symbols ready; GetTaskbarHost=%p, FrameHeight=%p",
           reinterpret_cast<void*>(CTaskBand_GetTaskbarHost_Original),
           reinterpret_cast<void*>(TaskbarHost_FrameHeight_Original));
    return true;
}

void TryHookTaskbarDllSymbols(HMODULE module, bool applyHooks) {
    if (!module || g_shuttingDown || g_taskbarDllHookAttempted.exchange(true)) return;
    if (HookTaskbarDllSymbols(module)) {
        g_taskbarDllHooked = true;
        if (applyHooks) Wh_ApplyHookOperations();
    }
}

// 1. Find HWND of Shell_TrayWnd in current explorer.exe process
HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        DWORD pid = 0;
        wchar_t className[64]{};
        if (GetWindowThreadProcessId(hwnd, &pid) &&
            pid == GetCurrentProcessId() &&
            GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

// 2. Extract XamlRoot from Windows 11 Taskbar internal structures
wux::XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    if (!taskbarWnd || !g_taskbarDllHooked || !CTaskBand_ITaskListWndSite_vftable ||
        !CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original ||
        !Std_Ref_Decref_Original) {
        ReportWidgetStatus(L"Waiting for Taskbar.dll symbols (GetTaskbarHost/FrameHeight/_Decref).");
        return nullptr;
    }

    HWND taskSwWnd = reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) {
        ReportWidgetStatus(L"Waiting for TaskbandHWND.");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwWnd, 0));
    if (!taskBand) {
        ReportWidgetStatus(L"Waiting for the taskband object.");
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0; i < 20; ++i) {
        if (*reinterpret_cast<void**>(taskBandForSite) == CTaskBand_ITaskListWndSite_vftable) {
            break;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
        if (i == 19) {
            ReportWidgetStatus(L"ITaskListWndSite not found in taskband; unsupported layout.");
            return nullptr;
        }
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    struct HostReferenceGuard {
        void* controlBlock;
        ~HostReferenceGuard() {
            if (controlBlock) Std_Ref_Decref_Original(controlBlock);
        }
    } hostReferenceGuard{taskbarHostSharedPtr[1]};
    if (!taskbarHostSharedPtr[0]) {
        ReportWidgetStatus(L"Waiting for TaskbarHost.");
        return nullptr;
    }

    size_t elementOffset = 0;
    bool signatureMatched = false;
#if defined(_M_X64) || defined(__x86_64__)
    const BYTE* bytes = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
        bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 &&
        bytes[7] <= 0x7F) {
        elementOffset = bytes[7];
        signatureMatched = true;
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    const DWORD* words = reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (words[0] == 0xD503237F && (words[1] & 0xFFC07FFF) == 0xA9807BFD &&
        words[2] == 0x910003FD && (words[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (words[3] >> 12) & 0xFF;
        signatureMatched = true;
    }
#endif
    if (!signatureMatched) {
        // Never infer an object offset from leftover bytes after a detour jump.
        ReportWidgetStatus(L"Unsupported or hooked FrameHeight prologue; no guessed pointer dereference.");
        return nullptr;
    }

    auto elementUnknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset);
    wux::FrameworkElement taskbarElement{nullptr};
    if (elementUnknown) {
        HRESULT hr = elementUnknown->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                                    winrt::put_abi(taskbarElement));
        if (FAILED(hr)) {
            ReportWidgetStatus(L"TaskbarHost element does not expose FrameworkElement.");
            return nullptr;
        }
    }

    auto root = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (!root) ReportWidgetStatus(L"Waiting for the taskbar XamlRoot.");
    return root;
}

// 3. Traverse XAML visual tree to find element by Name and resolve injection target
wux::FrameworkElement FindVisualElement(wux::DependencyObject const& root,
                                        std::wstring_view value, bool byClass,
                                        int depth = 48) {
    if (!root || depth <= 0) {
        return nullptr;
    }
    try {
        if (auto element = root.try_as<wux::FrameworkElement>()) {
            if (byClass ? winrt::get_class_name(element) == value : element.Name() == value)
                return element;
        }
    } catch (...) {}

    int count = 0;
    try { count = wuxm::VisualTreeHelper::GetChildrenCount(root); } catch (...) { return nullptr; }
    for (int i = 0; i < count; ++i) {
        wux::DependencyObject child{nullptr};
        try {
            child = wuxm::VisualTreeHelper::GetChild(root, i);
        } catch (...) { continue; }
        if (!child) {
            continue;
        }
        // A non-FrameworkElement visual can still contain the tray. Traverse it.
        if (auto found = FindVisualElement(child, value, byClass, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

wux::FrameworkElement FindChildByName(wux::FrameworkElement const& root,
                                      std::wstring_view name, int depth = 48) {
    return FindVisualElement(root, name, false, depth);
}

wux::FrameworkElement FindTaskbarFrame(wux::FrameworkElement const& root, int depth = 8) {
    if (!root || depth <= 0) return nullptr;
    // Some builds identify the frame by runtime class, without the expected Name.
    if (winrt::get_class_name(root) == L"Taskbar.TaskbarFrame") return root;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i).try_as<wux::FrameworkElement>();
        if (auto frame = FindTaskbarFrame(child, depth - 1)) return frame;
    }
    return nullptr;
}

wuxc::Grid FindTaskbarRootGrid(wux::FrameworkElement const& root) {
    auto frame = FindTaskbarFrame(root);
    if (!frame) frame = FindChildByName(root, L"TaskbarFrame", 8);
    if (!frame) return nullptr;
    if (auto control = frame.try_as<wuxc::Control>()) control.ApplyTemplate();

    // Only use the frame's own grid, never a button's similarly named RootGrid.
    int count = wuxm::VisualTreeHelper::GetChildrenCount(frame);
    for (int i = 0; i < count; ++i) {
        auto grid = wuxm::VisualTreeHelper::GetChild(frame, i).try_as<wuxc::Grid>();
        if (grid && grid.Name() == L"RootGrid") return grid;
    }

    // Allow template wrappers, anchoring the search to the taskbar's repeater.
    auto repeater = FindChildByName(frame, L"TaskbarFrameRepeater", 8);
    wux::DependencyObject ancestor = repeater;
    for (int i = 0; ancestor && i < 8; ++i) {
        ancestor = wuxm::VisualTreeHelper::GetParent(ancestor);
        if (!ancestor || ancestor == frame) break;
        auto grid = ancestor.try_as<wuxc::Grid>();
        if (grid && grid.Name() == L"RootGrid") return grid;
    }
    return nullptr;
}

struct InjectionTarget {
    wuxc::Panel panel{nullptr};
    int column = -1;
    int childIndex = -1;
    bool overlay = false;
};

wux::FrameworkElement GetDirectPanelChild(wuxc::Panel const& panel,
                                           wux::FrameworkElement element) {
    wux::DependencyObject current = element;
    for (int i = 0; current && i < 32; ++i) {
        auto parent = wuxm::VisualTreeHelper::GetParent(current);
        if (parent == panel) return current.try_as<wux::FrameworkElement>();
        current = parent;
    }
    return nullptr;
}

int GetPanelChildIndex(wuxc::Panel const& panel, wux::FrameworkElement const& element) {
    auto child = GetDirectPanelChild(panel, element);
    uint32_t index = 0;
    if (child && panel.Children().IndexOf(child, index)) return static_cast<int>(index);
    return -1;
}

wux::FrameworkElement FindTrayClock(wuxc::Panel const& panel) {
    // Resolve the containing child, not a clock text block's stale Grid.Column.
    for (PCWSTR name : {L"NotificationCenterButton", L"ClockButton", L"ClockStack",
                        L"SecondaryClockStack", L"Clock"}) {
        auto anchor = FindChildByName(panel, name);
        auto child = GetDirectPanelChild(panel, anchor);
        if (child && child.Visibility() == wux::Visibility::Visible) return child;
    }
    return nullptr;
}

wuxc::Panel FindSystemTrayPanel(wux::FrameworkElement const& root) {
    auto named = FindChildByName(root, L"SystemTrayFrameGrid");
    if (named) {
        if (auto panel = named.try_as<wuxc::Panel>()) return panel;
    }
    auto frame = FindVisualElement(root, L"SystemTray.SystemTrayFrame", true);
    if (!frame) frame = FindChildByName(root, L"SystemTrayFrame");
    if (frame) {
        if (auto control = frame.try_as<wuxc::Control>()) control.ApplyTemplate();
        named = FindChildByName(frame, L"SystemTrayFrameGrid");
        if (named) {
            if (auto panel = named.try_as<wuxc::Panel>()) return panel;
        }
    }

    // Template names can change. Find the nearest layout panel shared by the
    // clock and another tray control; never pick an inner clock-content grid.
    auto scope = frame ? frame : root;
    auto clock = FindChildByName(scope, L"NotificationCenterButton");
    if (!clock) clock = FindChildByName(scope, L"ClockButton");
    if (!clock) clock = FindChildByName(scope, L"ClockStack");
    auto peer = FindChildByName(scope, L"ShowDesktopStack");
    if (!peer) peer = FindChildByName(scope, L"ControlCenterButton");
    wux::DependencyObject current = clock;
    for (int i = 0; current && peer && i < 32; ++i) {
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (!current || current == frame) break;
        auto panel = current.try_as<wuxc::Panel>();
        if (!panel) continue;
        if (!panel.try_as<wuxc::Grid>() && !panel.try_as<wuxc::StackPanel>()) continue;
        auto clockChild = GetDirectPanelChild(panel, clock);
        auto peerChild = GetDirectPanelChild(panel, peer);
        if (clockChild && peerChild && clockChild != peerChild) return panel;
    }
    return nullptr;
}

void LogTrayLookupFailure(wux::FrameworkElement const& root, PCWSTR position) {
    static ULONGLONG lastLogged = 0;
    auto now = GetTickCount64();
    if (lastLogged && now - lastLogged < 30000) return;
    lastLogged = now;
    Wh_Log(L"Target unavailable: position=%s; visual tree follows", position);
    int remaining = 100;
    auto dump = [&](auto&& self, wux::DependencyObject const& node, int depth) -> void {
        if (!node || depth > 12 || remaining-- <= 0) return;
        try {
            auto fe = node.try_as<wux::FrameworkElement>();
            Wh_Log(L"[Widget tree] depth=%d class=%s name=%s", depth,
                   winrt::get_class_name(node).c_str(), fe ? fe.Name().c_str() : L"");
            int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < count && remaining > 0; ++i)
                self(self, wuxm::VisualTreeHelper::GetChild(node, i), depth + 1);
        } catch (...) {}
    };
    dump(dump, root, 0);
}

InjectionTarget ResolveInjectionTarget(wux::FrameworkElement const& root,
                                       ModSettings const& settings) {
    if (settings.position != L"root_left") {
        auto panel = FindSystemTrayPanel(root);
        if (!panel) return {};

        // New Explorer builds retain the name SystemTrayFrameGrid but use a
        // StackPanel. Its child order drives layout; Grid.Column is ignored.
        if (auto stack = panel.try_as<wuxc::StackPanel>()) {
            int index = -1;
            if (settings.position == L"tray_left") {
                index = 0;
            } else if (settings.position == L"tray_right") {
                index = static_cast<int>(stack.Children().Size());
            } else if (settings.position == L"tray_before_clock" ||
                       settings.position == L"tray_after_clock") {
                index = GetPanelChildIndex(panel, FindTrayClock(panel));
                if (index >= 0 && settings.position == L"tray_after_clock") ++index;
            }
            if (index >= 0) return {panel, -1, index, false};
            return {};
        }

        if (auto trayGrid = panel.try_as<wuxc::Grid>()) {
            int col = -1;
            if (settings.position == L"tray_left") {
                col = 0;
            } else if (settings.position == L"tray_right") {
                col = std::max(1, static_cast<int>(trayGrid.ColumnDefinitions().Size()));
            } else if (settings.position == L"tray_before_clock" ||
                       settings.position == L"tray_after_clock") {
                if (auto clock = FindTrayClock(panel)) {
                    col = wuxc::Grid::GetColumn(clock);
                    if (settings.position == L"tray_after_clock")
                        col += std::max(1, wuxc::Grid::GetColumnSpan(clock));
                }
            }
            if (col >= 0) return {panel, col, -1, false};
        }
        return {}; // Wait for the requested tray target instead of silently moving left.
    }

    auto rootGrid = FindTaskbarRootGrid(root);
    if (rootGrid) {
        return {rootGrid, 0, -1, true};
    }

    return {};
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar Compact Widget & Expanded Popup State (Taskbar UI Thread)

constexpr PCWSTR kTaskbarWidgetElementName = L"WindhawkVnLunarTaskbarWidget";
constexpr PCWSTR kExpandedPopupWindowClass = L"Windhawk_VnLunarExpandedPopup_Class";
constexpr UINT_PTR kTaskbarEnsureTimerId   = 0x202601;

HWND g_taskbarWnd = nullptr;
bool g_taskbarTimerStarted = false;
bool g_ensuringTaskbarWidget = false;
// Release every strong UI reference on the taskbar thread during explicit
// teardown; never release these objects from CRT destructors at process exit.
[[clang::no_destroy]] wuxc::Panel g_injectedParentPanel{nullptr};
[[clang::no_destroy]] wuxc::ColumnDefinition g_injectedColumnDef{nullptr};
[[clang::no_destroy]] wuxc::ColumnDefinition g_injectedImplicitColumnDef{nullptr};
struct ShiftedTrayChild {
    winrt::weak_ref<wux::FrameworkElement> element;
    int column = 0;
    int span = 1;
    bool moved = false;
    bool widened = false;
};
// Contains only weak UI references; automatic destruction is safe at process exit.
std::vector<ShiftedTrayChild> g_shiftedTrayChildren;
[[clang::no_destroy]] wuxc::Border g_playerGrid{nullptr}; // The compact taskbar widget root border
[[clang::no_destroy]] wuxc::Border g_widgetBadgeBorder{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_widgetZodiacText{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_widgetLine1Text{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_widgetLine2Text{nullptr};
bool g_widgetHovered = false;
std::wstring g_currentInjectedPosition;
WidgetRotationState g_widgetRotation;

// Expanded Popup Window & XAML Host State
constexpr int kExpandedPopupHeightDip = 316;
constexpr UINT kPopupOpenDurationMs = 300;
constexpr UINT kPopupCloseDurationMs = 240;
constexpr UINT_PTR kPopupCloseTimerId = 0x202602;
bool g_popupClosing = false;
ULONGLONG g_popupCloseDeadline = 0;
HWND g_expandedPopup = nullptr;
HWND g_popupXamlChild = nullptr;
[[clang::no_destroy]] wuxh::DesktopWindowXamlSource g_popupXamlSource{nullptr};
bool g_popupBackdropDirty = true;
int g_popupBackdropTheme = -1;
[[clang::no_destroy]] wuxc::Border g_popupRootCard{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupBackdropLayer{nullptr};
[[clang::no_destroy]] wcomp::ContainerVisual g_popupBackdropVisual{nullptr};
[[clang::no_destroy]] wcomp::CompositionColorBrush g_popupTintBrush{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupBadgeBorder{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupZodiacText{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupTitleSolar{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupTitleLunar{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupHolidayBanner{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupHolidayText{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowBattu{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowMenhTruc{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowGioHoangDao{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowNenKy{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowXungHuong{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_popupHoroscopeScroll{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_popupHolidayScroll{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupHolidayListText{nullptr};
[[clang::no_destroy]] wuxc::Button g_popupHolidayListButton{nullptr};
bool g_popupShowingHolidays = false;
int g_popupHolidayListLanguage = -1;

RECT g_popupSourceRect{};
RECT g_popupFinalRect{};
RECT g_popupHostRect{};
RECT g_lastPopupWindowRect{};
bool g_popupOpensDown = false;
bool g_taskbarAtTop = false;
int g_popupDayOffset = 0;

double PopupDpiScale(HWND hwnd) {
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 96;
    if (dpi == 0) dpi = 96;
    return static_cast<double>(dpi) / 96.0;
}

int PopupDipToPx(double dip, HWND hwnd) {
    return static_cast<int>(std::lround(dip * PopupDpiScale(hwnd)));
}

bool TaskbarPopupOpensDown(RECT const& taskbar, MONITORINFO const& mi) {
    int monitorMidY = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
    int taskbarMidY = (taskbar.top + taskbar.bottom) / 2;
    return taskbarMidY < monitorMidY;
}

winrt::Windows::UI::Color GetFiveElementColor(FiveElement elem, bool isLightTheme) {
    switch (elem) {
        case FiveElement::Metal:
            return isLightTheme ? winrt::Windows::UI::Color{65, 200, 150, 20}
                                : winrt::Windows::UI::Color{85, 225, 180, 50};
        case FiveElement::Wood:
            return isLightTheme ? winrt::Windows::UI::Color{60, 35, 160, 85}
                                : winrt::Windows::UI::Color{85, 50, 190, 105};
        case FiveElement::Water:
            return isLightTheme ? winrt::Windows::UI::Color{60, 35, 125, 215}
                                : winrt::Windows::UI::Color{85, 55, 150, 240};
        case FiveElement::Fire:
            return isLightTheme ? winrt::Windows::UI::Color{60, 215, 65, 55}
                                : winrt::Windows::UI::Color{85, 235, 85, 70};
        case FiveElement::Earth:
        default:
            return isLightTheme ? winrt::Windows::UI::Color{65, 175, 115, 55}
                                : winrt::Windows::UI::Color{85, 195, 135, 75};
    }
}

bool IsElementLightTheme(wux::FrameworkElement const& fe) {
    if (fe) {
        try {
            return fe.ActualTheme() == wux::ElementTheme::Light;
        } catch (...) {}
    }
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme",
                     RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

void UpdateWidgetHoverVisual() {
    if (!g_playerGrid) return;
    try {
        bool light = IsElementLightTheme(g_playerGrid);
        bool popupActive = g_expandedPopup && IsWindowVisible(g_expandedPopup);
        winrt::Windows::UI::Color bg;
        winrt::Windows::UI::Color borderCol;
        if (g_widgetHovered || popupActive) {
            bg = light ? winrt::Windows::UI::Color{28, 0, 0, 0}
                       : winrt::Windows::UI::Color{34, 255, 255, 255};
            borderCol = light ? winrt::Windows::UI::Color{30, 0, 0, 0}
                              : winrt::Windows::UI::Color{40, 255, 255, 255};
        } else {
            // Use a transparent background when idle.
            bg = winrt::Windows::UI::Color{0, 0, 0, 0};
            borderCol = winrt::Windows::UI::Color{0, 0, 0, 0};
        }
        g_playerGrid.Background(wuxm::SolidColorBrush(bg));
        g_playerGrid.BorderBrush(wuxm::SolidColorBrush(borderCol));
    } catch (...) {}
}

void RefreshExpandedPopupData();

void RefreshTaskbarWidgetData() {
    if (!g_playerGrid) return;
    try {
        ModSettings settings = GetSettings();
        bool vi = UseVietnameseLabels(settings);
        bool light = IsElementLightTheme(g_playerGrid);

        SYSTEMTIME st{};
        GetLocalTime(&st);
        ComputedHoroscope h = ComputeHoroscopeForDate(st.wYear, st.wMonth, st.wDay, vi, st.wHour);

        g_playerGrid.Width(static_cast<double>(settings.widgetWidth));
        g_playerGrid.MaxWidth(static_cast<double>(settings.widgetWidth));
        if (settings.position == L"root_left") {
            g_playerGrid.Margin({static_cast<double>(settings.widgetLeftOffset), 0.0, 0.0, 0.0});
        } else {
            g_playerGrid.Margin({4.0, 0.0, 4.0, 0.0});
        }

        if (g_widgetBadgeBorder) {
            g_widgetBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(h.element, light)));
        }
        if (g_widgetZodiacText) {
            g_widgetZodiacText.Text(h.zodiacEmoji);
        }

        std::wstring line1 = FormatWidgetLunarHeader(h, vi);
        bool isHoliday = !h.specialHoliday.empty();
        std::wstring line2 = SelectWidgetSecondLine(h, vi, GetTickCount64(), g_widgetRotation);

        winrt::Windows::UI::Color primaryCol = light
            ? winrt::Windows::UI::Color{255, 22, 22, 24}
            : winrt::Windows::UI::Color{255, 245, 245, 248};
        winrt::Windows::UI::Color secondaryCol;
        if (isHoliday) {
            secondaryCol = light
                ? winrt::Windows::UI::Color{255, 196, 43, 28}
                : winrt::Windows::UI::Color{255, 255, 150, 135};
        } else {
            secondaryCol = light
                ? winrt::Windows::UI::Color{225, 65, 65, 70}
                : winrt::Windows::UI::Color{215, 205, 205, 212};
        }

        bool linesChanged = false;
        if (g_widgetLine1Text) {
            if (std::wstring_view(g_widgetLine1Text.Text()) != std::wstring_view(line1)) {
                g_widgetLine1Text.Text(line1);
                linesChanged = true;
            }
            g_widgetLine1Text.Foreground(wuxm::SolidColorBrush(primaryCol));
        }
        if (g_widgetLine2Text) {
            if (std::wstring_view(g_widgetLine2Text.Text()) != std::wstring_view(line2)) {
                g_widgetLine2Text.Text(line2);
                linesChanged = true;
            }
            g_widgetLine2Text.Foreground(wuxm::SolidColorBrush(secondaryCol));
        }
        if (linesChanged) {
            // Long names remain available in full without increasing taskbar width.
            wuxc::ToolTipService::SetToolTip(g_playerGrid,
                winrt::box_value(winrt::hstring{line1 + L"\n" + line2}));
        }

        UpdateWidgetHoverVisual();
        if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) RefreshExpandedPopupData();
    } catch (...) {}
}

// 4. Screen geometry & positioning for the Expanded Horoscope Popup
bool GetElementScreenRect(wux::FrameworkElement const& element, RECT& result) {
    if (!element || !g_taskbarWnd) {
        return false;
    }

    try {
        RECT taskbar{};
        GetWindowRect(g_taskbarWnd, &taskbar);
        auto transform = element.TransformToVisual(nullptr);
        auto point = transform.TransformPoint({0, 0});
        double scale = PopupDpiScale(g_taskbarWnd);
        result.left = taskbar.left + static_cast<int>(std::lround(point.X * scale));
        result.top = taskbar.top + static_cast<int>(std::lround(point.Y * scale));
        result.right = result.left +
                       static_cast<int>(std::lround(element.ActualWidth() * scale));
        result.bottom = result.top +
                        static_cast<int>(std::lround(element.ActualHeight() * scale));
        return result.right > result.left && result.bottom > result.top;
    } catch (...) {
        return false;
    }
}

void CapturePopupSourceGeometry(int& outCardWidthDip, int& outCardHeightDip) {
    ModSettings settings = GetSettings();
    RECT taskbar{};
    GetWindowRect(g_taskbarWnd, &taskbar);
    if (!GetElementScreenRect(g_playerGrid, g_popupSourceRect)) {
        int leftPx = PopupDipToPx(settings.widgetLeftOffset, g_taskbarWnd);
        g_popupSourceRect = {
            taskbar.left + leftPx, taskbar.top,
            taskbar.left + leftPx + PopupDipToPx(settings.widgetWidth, g_taskbarWnd),
            taskbar.bottom
        };
    }

    HMONITOR monitor = MonitorFromRect(&g_popupSourceRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        monitorInfo.rcMonitor = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
        monitorInfo.rcWork = monitorInfo.rcMonitor;
    }

    double dpiScale = PopupDpiScale(g_taskbarWnd);
    bool opensDown = TaskbarPopupOpensDown(taskbar, monitorInfo);
    g_popupOpensDown = opensDown;
    g_taskbarAtTop = opensDown;
    int popupGap = PopupDipToPx(opensDown ? 6.0 : 10.0, g_taskbarWnd);
    int edgePad = PopupDipToPx(12.0, g_taskbarWnd);

    int availableWidthPx = std::max(1L, monitorInfo.rcWork.right - monitorInfo.rcWork.left - 2 * edgePad);
    int availableHeightPx = opensDown
        ? std::max(1L, monitorInfo.rcWork.bottom - taskbar.bottom - popupGap - edgePad)
        : std::max(1L, taskbar.top - popupGap - monitorInfo.rcWork.top - edgePad);
    outCardWidthDip = std::min(settings.expandedWidth,
        std::max(1, static_cast<int>(std::floor(availableWidthPx / dpiScale))));
    // Independent of holiday text and selected page. Only shrink on a small screen.
    outCardHeightDip = std::min(kExpandedPopupHeightDip,
        std::max(1, static_cast<int>(std::floor(availableHeightPx / dpiScale))));

    int widthPx = PopupDipToPx(outCardWidthDip, g_taskbarWnd);
    int heightPx = PopupDipToPx(outCardHeightDip, g_taskbarWnd);
    int minLeft = monitorInfo.rcWork.left + edgePad;
    int maxLeft = std::max(minLeft, static_cast<int>(monitorInfo.rcWork.right) - edgePad - widthPx);
    int left = ClampVal(static_cast<int>(g_popupSourceRect.left), minLeft, maxLeft);
    int top = opensDown ? taskbar.bottom + popupGap : taskbar.top - popupGap - heightPx;
    g_popupFinalRect = {left, top, left + widthPx, top + heightPx};
    g_popupHostRect = g_popupFinalRect;
}

void PositionExpandedPopup() {
    if (!g_expandedPopup || !g_taskbarWnd) {
        return;
    }

    RECT hostRect = g_popupHostRect;
    int width = std::max(1L, hostRect.right - hostRect.left);
    int height = std::max(1L, hostRect.bottom - hostRect.top);

    if (!EqualRect(&hostRect, &g_lastPopupWindowRect)) {
        g_lastPopupWindowRect = hostRect;
        SetWindowPos(g_expandedPopup, HWND_TOPMOST,
                     hostRect.left, hostRect.top, width, height,
                     SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
    }
    if (g_popupXamlChild) {
        MoveWindow(g_popupXamlChild, 0, 0, width, height, TRUE);
    }
}

void ClearPopupCompositionBackdrop() {
    if (g_popupBackdropLayer) {
        try { wuxh::ElementCompositionPreview::SetElementChildVisual(g_popupBackdropLayer, nullptr); }
        catch (...) {}
    }
    g_popupBackdropVisual = nullptr;
    g_popupTintBrush = nullptr;
}

bool SetPopupCompositionBackdrop(bool light) {
    if (!g_popupBackdropLayer || !g_expandedPopup || !g_popupXamlChild) return false;
    try {
        // Paint the sampled desktop INSIDE the island, behind the content.
        // This does not require the island to reveal its parent HWND's pixels.
        BOOL enabled = TRUE;
        HRESULT hostHr = DwmSetWindowAttribute(g_expandedPopup, 17, &enabled, sizeof(enabled));
        HRESULT islandHr = DwmSetWindowAttribute(g_popupXamlChild, 17, &enabled, sizeof(enabled));
        if (FAILED(hostHr) && FAILED(islandHr)) return false;
        DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
        DwmSetWindowAttribute(g_expandedPopup, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
        BOOL dark = light ? FALSE : TRUE;
        DwmSetWindowAttribute(g_expandedPopup, 20, &dark, sizeof(dark));

        if (!g_popupBackdropVisual) {
            auto visual = wuxh::ElementCompositionPreview::GetElementVisual(g_popupBackdropLayer);
            auto compositor = visual.Compositor();
            auto container = compositor.CreateContainerVisual();
            container.RelativeSizeAdjustment({1.0f, 1.0f});
            auto backdrop = compositor.CreateSpriteVisual();
            backdrop.RelativeSizeAdjustment({1.0f, 1.0f});
            backdrop.Brush(compositor.CreateHostBackdropBrush());
            auto tint = compositor.CreateSpriteVisual();
            tint.RelativeSizeAdjustment({1.0f, 1.0f});
            auto tintBrush = compositor.CreateColorBrush();
            tint.Brush(tintBrush);
            container.Children().InsertAtBottom(backdrop);
            container.Children().InsertAtTop(tint);
            wuxh::ElementCompositionPreview::SetElementChildVisual(g_popupBackdropLayer, container);
            g_popupBackdropVisual = container;
            g_popupTintBrush = tintBrush;
        }
        g_popupTintBrush.Color(light ? winrt::Windows::UI::Color{110, 246, 246, 250}
                                    : winrt::Windows::UI::Color{100, 32, 33, 38});
        g_popupRootCard.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
        Wh_Log(L"Composition HostBackdrop attached: host=0x%08X, island=0x%08X",
               static_cast<unsigned int>(hostHr), static_cast<unsigned int>(islandHr));
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Composition failed: 0x%08X", static_cast<unsigned int>(e.code().value));
        ClearPopupCompositionBackdrop();
        return false;
    }
}

void ApplyPopupBackdrop(bool light) {
    if (!g_popupRootCard || !g_expandedPopup) return;
    if (!g_popupBackdropDirty && g_popupBackdropTheme == static_cast<int>(light)) return;

    if (!SetPopupCompositionBackdrop(light)) {
        ClearPopupCompositionBackdrop();
        auto fallback = light ? winrt::Windows::UI::Color{255, 246, 246, 250}
                              : winrt::Windows::UI::Color{255, 32, 33, 38};
        g_popupRootCard.Background(wuxm::SolidColorBrush(fallback));
        Wh_Log(L"Popup backdrop unavailable; using a solid background.");
    }
    g_popupBackdropTheme = static_cast<int>(light);
    g_popupBackdropDirty = false;
}

void ResetPopupScrollPosition(wuxc::ScrollViewer const& scroll) {
    if (!scroll) return;
    try {
        scroll.ChangeView(nullptr, winrt::box_value(0.0).as<wf::IReference<double>>(), nullptr, true);
    } catch (...) {}
}

void ShowPopupHolidayList(bool show) {
    g_popupShowingHolidays = show;
    if (g_popupHoroscopeScroll)
        g_popupHoroscopeScroll.Visibility(show ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    if (g_popupHolidayScroll)
        g_popupHolidayScroll.Visibility(show ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (g_popupHolidayListButton) {
        bool vi = UseVietnameseLabels(GetSettings());
        g_popupHolidayListButton.Content(winrt::box_value(
            show ? L"↩" : PickText(vi, L"Lễ", L"List")));
        wuxc::ToolTipService::SetToolTip(g_popupHolidayListButton, winrt::box_value(
            show ? PickText(vi, L"Trở về Tử Vi", L"Back to horoscope")
                 : PickText(vi, L"Ngày lễ & kỷ niệm Việt Nam", L"Vietnamese holidays and commemorations")));
    }
}

void ReleasePopupScrollControls() {
    g_popupHoroscopeScroll = nullptr;
    g_popupHolidayScroll = nullptr;
    g_popupHolidayListText = nullptr;
    g_popupHolidayListButton = nullptr;
    g_popupShowingHolidays = false;
    g_popupHolidayListLanguage = -1;
}

void RefreshExpandedPopupData() {
    if (!g_popupRootCard) return;
    try {
        ModSettings settings = GetSettings();
        bool vi = UseVietnameseLabels(settings);
        bool light = IsElementLightTheme(g_playerGrid);

        SYSTEMTIME st{};
        GetLocalTime(&st);
        if (g_popupDayOffset != 0) {
            OffsetSystemDateByDays(st, g_popupDayOffset);
        }
        ComputedHoroscope h = ComputeHoroscopeForDate(st.wYear, st.wMonth, st.wDay, vi, st.wHour);

        g_popupRootCard.RequestedTheme(light ? wux::ElementTheme::Light : wux::ElementTheme::Dark);

        winrt::Windows::UI::Color cardBorder = light
            ? winrt::Windows::UI::Color{40, 0, 0, 0}
            : winrt::Windows::UI::Color{45, 255, 255, 255};
        winrt::Windows::UI::Color textPrimary = light
            ? winrt::Windows::UI::Color{255, 20, 20, 24}
            : winrt::Windows::UI::Color{255, 245, 245, 248};
        winrt::Windows::UI::Color accentLunar = light
            ? winrt::Windows::UI::Color{255, 196, 43, 28}
            : winrt::Windows::UI::Color{255, 255, 145, 130};

        ApplyPopupBackdrop(light);
        g_popupRootCard.BorderBrush(wuxm::SolidColorBrush(cardBorder));

        if (g_popupBadgeBorder) {
            g_popupBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(h.element, light)));
        }
        if (g_popupZodiacText) {
            g_popupZodiacText.Text(h.zodiacEmoji);
        }

        WCHAR solarTitle[128]{};
        _snwprintf_s(solarTitle, _TRUNCATE, L"%s, %02d/%02d/%d",
                     h.weekdayStr, h.sDay, h.sMonth, h.sYear);
        if (g_popupTitleSolar) {
            g_popupTitleSolar.Text(solarTitle);
            g_popupTitleSolar.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        WCHAR lunarTitle[160]{};
        if (vi) {
            _snwprintf_s(lunarTitle, _TRUNCATE, L"Âm lịch: %02d/%02d%s • Năm %s",
                         h.lDay, h.lMonth, h.isLeap ? L" (Nhuận)" : L"", h.yearCanChi.c_str());
        } else {
            _snwprintf_s(lunarTitle, _TRUNCATE, L"Lunar: %02d/%02d%s • Year of %s",
                         h.lDay, h.lMonth, h.isLeap ? L" (Leap)" : L"", h.yearCanChi.c_str());
        }
        if (g_popupTitleLunar) {
            g_popupTitleLunar.Text(lunarTitle);
            g_popupTitleLunar.Foreground(wuxm::SolidColorBrush(accentLunar));
        }

        bool hasHoliday = !h.specialHoliday.empty();
        if (g_popupHolidayBanner && g_popupHolidayText) {
            if (hasHoliday) {
                g_popupHolidayText.Text(L"🎉 " + h.specialHoliday);
                g_popupHolidayText.Foreground(wuxm::SolidColorBrush(accentLunar));
                g_popupHolidayBanner.Visibility(wux::Visibility::Visible);
            } else {
                g_popupHolidayBanner.Visibility(wux::Visibility::Collapsed);
            }
        }

        WCHAR buf[512]{};
        if (g_popupRowBattu) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"%s Ngày %s (Con %s) • Tháng %s • Giờ %s",
                             h.zodiacEmoji, h.dayCanChi.c_str(), h.zodiacAnimalName,
                             h.monthCanChi.c_str(), h.hourCanChi.c_str());
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"%s Day %s (%s) • Month %s • Hour %s",
                             h.zodiacEmoji, h.dayCanChi.c_str(), h.zodiacAnimalName,
                             h.monthCanChi.c_str(), h.hourCanChi.c_str());
            }
            g_popupRowBattu.Text(buf);
            g_popupRowBattu.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowMenhTruc) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"☯ Mệnh: %s\n%s %s (%s) • Trực: %s",
                             h.napAmStr, h.isHoangDao ? L"🌟" : L"⚪",
                             h.starName, h.isHoangDao ? L"Ngày Tốt" : L"Hắc Đạo", h.trucName);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"☯ Element: %s\n%s %s (%s) • Officer: %s",
                             h.napAmStr, h.isHoangDao ? L"🌟" : L"⚪",
                             h.starName, h.isHoangDao ? L"Auspicious" : L"Inauspicious", h.trucName);
            }
            g_popupRowMenhTruc.Text(buf);
            g_popupRowMenhTruc.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowGioHoangDao) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"⏰ Giờ Hoàng Đạo: %s", h.gioHoangDao);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"⏰ Auspicious Hours: %s", h.gioHoangDao);
            }
            g_popupRowGioHoangDao.Text(buf);
            g_popupRowGioHoangDao.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowNenKy) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"🔮 Nên: %s\n⛔ Kỵ: %s", h.goodFor, h.badFor);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"🔮 Good for: %s\n⛔ Avoid: %s", h.goodFor, h.badFor);
            }
            g_popupRowNenKy.Text(buf);
            g_popupRowNenKy.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowXungHuong) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"🧭 Tuổi xung: %s • Xuất hành: %s (Hỷ), %s (Tài)",
                             h.tuoiXung.c_str(), h.huongHyThan, h.huongTaiThan);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"🧭 Clash: %s • Directions: %s (Joy), %s (Wealth)",
                             h.tuoiXung.c_str(), h.huongHyThan, h.huongTaiThan);
            }
            g_popupRowXungHuong.Text(buf);
            g_popupRowXungHuong.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupHolidayListText) {
            if (g_popupHolidayListLanguage != static_cast<int>(vi)) {
                g_popupHolidayListText.Text(BuildHolidayCatalogText(vi));
                g_popupHolidayListLanguage = static_cast<int>(vi);
            }
            g_popupHolidayListText.Foreground(wuxm::SolidColorBrush(textPrimary));
        }
        ShowPopupHolidayList(g_popupShowingHolidays);

        int cardW = 380, cardH = kExpandedPopupHeightDip;
        CapturePopupSourceGeometry(cardW, cardH);
        g_popupRootCard.Width(static_cast<double>(cardW));
        g_popupRootCard.Height(static_cast<double>(cardH));

        PositionExpandedPopup();
    } catch (...) {}
}

wuxc::Border CreatePopupRowSection(wuxc::TextBlock& tbOut, bool withBottomDivider = true) {
    wuxc::Border row;
    row.Padding({4, 5, 4, 6});
    if (withBottomDivider) {
        row.BorderThickness({0, 0, 0, 1});
        row.BorderBrush(wuxm::SolidColorBrush(winrt::Windows::UI::Color{28, 128, 128, 128}));
    }

    wuxc::TextBlock tb;
    tb.FontSize(12.2);
    tb.LineHeight(18.0);
    tb.TextWrapping(wux::TextWrapping::Wrap);
    row.Child(tb);
    tbOut = tb;
    return row;
}

void BeginCloseExpandedPopup(HWND hwnd);

bool InitializePopupXamlHost(HWND hwnd) {
    try {
        g_popupXamlSource = wuxh::DesktopWindowXamlSource();
        g_popupBackdropDirty = true;
        g_popupBackdropTheme = -1;
        auto native = g_popupXamlSource.as<IDesktopWindowXamlSourceNative>();
        winrt::check_hresult(native->AttachToWindow(hwnd));
        winrt::check_hresult(native->get_WindowHandle(&g_popupXamlChild));

        wuxc::Border rootCard;
        rootCard.CornerRadius({8, 8, 8, 8});
        rootCard.BorderThickness({1, 1, 1, 1});
        rootCard.Padding({14, 12, 14, 10});
        rootCard.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        rootCard.VerticalAlignment(wux::VerticalAlignment::Stretch);
        rootCard.KeyDown([](wf::IInspectable const&, wuxi::KeyRoutedEventArgs const& args) {
            if (args.Key() == winrt::Windows::System::VirtualKey::Escape) {
                args.Handled(true);
                BeginCloseExpandedPopup(g_expandedPopup);
            }
        });

        wuxc::Grid mainGrid;
        wuxc::RowDefinition headerRow;
        headerRow.Height(wux::GridLengthHelper::Auto());
        wuxc::RowDefinition dividerRow;
        dividerRow.Height(wux::GridLengthHelper::Auto());
        wuxc::RowDefinition bodyRow;
        bodyRow.Height(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
        mainGrid.RowDefinitions().Append(headerRow);
        mainGrid.RowDefinitions().Append(dividerRow);
        mainGrid.RowDefinitions().Append(bodyRow);

        wuxc::StackPanel bodyStack;
        bodyStack.Spacing(4.0);

        wuxc::Grid headerGrid;
        wuxc::ColumnDefinition colBadge;
        colBadge.Width(wux::GridLengthHelper::Auto());
        wuxc::ColumnDefinition colTitles;
        colTitles.Width(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
        wuxc::ColumnDefinition colNav;
        colNav.Width(wux::GridLengthHelper::Auto());
        headerGrid.ColumnDefinitions().Append(colBadge);
        headerGrid.ColumnDefinitions().Append(colTitles);
        headerGrid.ColumnDefinitions().Append(colNav);
        headerGrid.Margin({0, 0, 0, 6});

        wuxc::Border badgeBorder;
        badgeBorder.Width(44.0);
        badgeBorder.Height(44.0);
        badgeBorder.CornerRadius({10, 10, 10, 10});
        badgeBorder.Margin({0, 0, 12, 0});
        badgeBorder.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock zodiacTb;
        zodiacTb.FontSize(24.0);
        zodiacTb.HorizontalAlignment(wux::HorizontalAlignment::Center);
        zodiacTb.VerticalAlignment(wux::VerticalAlignment::Center);
        badgeBorder.Child(zodiacTb);
        wuxc::Grid::SetColumn(badgeBorder, 0);
        headerGrid.Children().Append(badgeBorder);

        wuxc::StackPanel titleStack;
        titleStack.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock titleSolar;
        titleSolar.FontSize(14.0);
        titleSolar.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        titleSolar.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        titleSolar.MaxLines(1);

        wuxc::TextBlock titleLunar;
        titleLunar.FontSize(12.0);
        titleLunar.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
        titleLunar.Margin({0, 2, 0, 0});
        titleLunar.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        titleLunar.MaxLines(1);

        titleStack.Children().Append(titleSolar);
        titleStack.Children().Append(titleLunar);
        wuxc::Grid::SetColumn(titleStack, 1);
        headerGrid.Children().Append(titleStack);

        wuxc::StackPanel navStack;
        navStack.Orientation(wuxc::Orientation::Horizontal);
        navStack.Spacing(4.0);
        navStack.VerticalAlignment(wux::VerticalAlignment::Center);
        navStack.Margin({8, 0, 0, 0});

        auto makeNavBtn = [](PCWSTR label, auto onClick) {
            wuxc::Button btn;
            btn.Content(winrt::box_value(label));
            btn.FontSize(11.5);
            btn.Padding({6, 4, 6, 5});
            btn.MinWidth(28.0);
            btn.CornerRadius({6, 6, 6, 6});
            btn.Click(onClick);
            return btn;
        };

        navStack.Children().Append(makeNavBtn(L"◀", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(false);
            g_popupDayOffset--;
            RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_popupHoroscopeScroll);
        }));
        navStack.Children().Append(makeNavBtn(L"●", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(false);
            g_popupDayOffset = 0;
            RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_popupHoroscopeScroll);
        }));
        navStack.Children().Append(makeNavBtn(L"▶", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(false);
            g_popupDayOffset++;
            RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_popupHoroscopeScroll);
        }));

        auto holidayListButton = makeNavBtn(L"Lễ", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(!g_popupShowingHolidays);
            RefreshExpandedPopupData();
        });
        navStack.Children().Append(holidayListButton);

        wuxc::Grid::SetColumn(navStack, 2);
        headerGrid.Children().Append(navStack);
        wuxc::Grid::SetRow(headerGrid, 0);
        mainGrid.Children().Append(headerGrid);

        wuxc::Border headerDiv;
        headerDiv.Height(1.0);
        headerDiv.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{35, 128, 128, 128}));
        headerDiv.Margin({0, 0, 0, 4});
        wuxc::Grid::SetRow(headerDiv, 1);
        mainGrid.Children().Append(headerDiv);

        wuxc::Border holidayBanner;
        holidayBanner.CornerRadius({6, 6, 6, 6});
        holidayBanner.Padding({8, 4, 8, 5});
        holidayBanner.Margin({0, 2, 0, 0});
        holidayBanner.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{30, 255, 100, 90}));
        holidayBanner.Visibility(wux::Visibility::Collapsed);

        wuxc::TextBlock holidayTb;
        holidayTb.FontSize(12.0);
        holidayTb.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        holidayTb.TextWrapping(wux::TextWrapping::Wrap);
        holidayBanner.Child(holidayTb);
        bodyStack.Children().Append(holidayBanner);

        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowBattu, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowMenhTruc, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowGioHoangDao, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowNenKy, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowXungHuong, false));

        // A ScrollViewer in the star row is measured with the remaining height.
        // A vertical StackPanel around it would measure infinity and prevent scrolling.
        auto makeScroll = []() {
            wuxc::ScrollViewer scroll;
            scroll.VerticalScrollMode(wuxc::ScrollMode::Enabled);
            scroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Visible);
            scroll.HorizontalScrollMode(wuxc::ScrollMode::Disabled);
            scroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
            scroll.ZoomMode(wuxc::ZoomMode::Disabled);
            scroll.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
            scroll.VerticalContentAlignment(wux::VerticalAlignment::Top);
            scroll.Padding({0, 0, 16, 4}); // Reserve space for the scrollbar thumb.
            scroll.IsTabStop(true);
            return scroll;
        };

        auto horoscopeScroll = makeScroll();
        horoscopeScroll.Content(bodyStack);
        wuxc::Grid::SetRow(horoscopeScroll, 2);
        mainGrid.Children().Append(horoscopeScroll);

        wuxc::TextBlock holidayListText;
        holidayListText.FontSize(12.2);
        holidayListText.LineHeight(19.0);
        holidayListText.TextWrapping(wux::TextWrapping::Wrap);
        holidayListText.Margin({4, 6, 4, 8});
        auto holidayScroll = makeScroll();
        holidayScroll.Content(holidayListText);
        holidayScroll.Visibility(wux::Visibility::Collapsed);
        wuxc::Grid::SetRow(holidayScroll, 2);
        mainGrid.Children().Append(holidayScroll);

        rootCard.Child(mainGrid);
        wuxc::Grid popupLayers;
        wuxc::Border backdropLayer;
        backdropLayer.IsHitTestVisible(false);
        popupLayers.Children().Append(backdropLayer);
        popupLayers.Children().Append(rootCard);
        g_popupXamlSource.Content(popupLayers);
        g_popupBackdropLayer = backdropLayer;

        g_popupRootCard = rootCard;
        g_popupBadgeBorder = badgeBorder;
        g_popupZodiacText = zodiacTb;
        g_popupTitleSolar = titleSolar;
        g_popupTitleLunar = titleLunar;
        g_popupHolidayBanner = holidayBanner;
        g_popupHolidayText = holidayTb;
        g_popupHoroscopeScroll = horoscopeScroll;
        g_popupHolidayScroll = holidayScroll;
        g_popupHolidayListText = holidayListText;
        g_popupHolidayListButton = holidayListButton;
        g_popupHolidayListLanguage = -1;
        ShowPopupHolidayList(false);

        if (g_popupXamlChild) {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            SetWindowPos(g_popupXamlChild, nullptr, 0, 0,
                         rc.right - rc.left, rc.bottom - rc.top,
                         SWP_SHOWWINDOW | SWP_NOACTIVATE);
        }
        return true;
    } catch (...) {
        Wh_Log(L"InitializePopupXamlHost failed");
        ReleasePopupScrollControls();
        ClearPopupCompositionBackdrop();
        g_popupBackdropLayer = nullptr;
        if (g_popupXamlSource) {
            try { g_popupXamlSource.Close(); } catch (...) {}
            g_popupXamlSource = nullptr;
        }
        g_popupXamlChild = nullptr;

        return false;
    }
}

// Animate the content without disconnecting the separate HostBackdrop visual.
// Replacing an animation starts from its current presentation value, so quick
// close/open clicks reverse smoothly instead of jumping back to a hidden card.
bool AnimatePopupTransition(bool opening, bool freshOpen = false) {
    if (!g_popupRootCard) return false;
    wcomp::Visual visual{nullptr};
    try {
        g_popupRootCard.UpdateLayout();
        visual = wuxh::ElementCompositionPreview::GetElementVisual(g_popupRootCard);
        auto compositor = visual.Compositor();
        auto easing = compositor.CreateCubicBezierEasingFunction(
            opening ? wf::Numerics::float2{0.16f, 1.0f} : wf::Numerics::float2{0.4f, 0.0f},
            opening ? wf::Numerics::float2{0.3f, 1.0f} : wf::Numerics::float2{1.0f, 1.0f});

        if (freshOpen) {
            visual.StopAnimation(L"Opacity");
            visual.StopAnimation(L"Scale");
            visual.Opacity(0.0f);
            visual.Scale({0.96f, 0.96f, 1.0f});
        }
        visual.CenterPoint({
            static_cast<float>(g_popupRootCard.ActualWidth() / 2.0),
            g_popupOpensDown ? 0.0f
                            : static_cast<float>(g_popupRootCard.ActualHeight()),
            0.0f
        });
        auto duration = std::chrono::milliseconds{
            opening ? kPopupOpenDurationMs : kPopupCloseDurationMs};
        auto fade = compositor.CreateScalarKeyFrameAnimation();
        fade.Duration(duration);
        fade.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
        fade.InsertKeyFrame(1.0f, opening ? 1.0f : 0.0f, easing);
        auto zoom = compositor.CreateVector3KeyFrameAnimation();
        zoom.Duration(duration);
        zoom.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
        zoom.InsertKeyFrame(1.0f, opening ? wf::Numerics::float3{1.0f, 1.0f, 1.0f}
                                        : wf::Numerics::float3{0.96f, 0.96f, 1.0f}, easing);
        visual.StartAnimation(L"Opacity", fade);
        visual.StartAnimation(L"Scale", zoom);
        return true;
    } catch (...) {
        if (visual) {
            try {
                visual.StopAnimation(L"Opacity");
                visual.StopAnimation(L"Scale");
                visual.Opacity(1.0f);
                visual.Scale({1.0f, 1.0f, 1.0f});
            } catch (...) {}
        }
        Wh_Log(L"Animation unavailable; using immediate visibility change.");
        return false;
    }
}

void CancelPopupClose(HWND hwnd) {
    if (hwnd) KillTimer(hwnd, kPopupCloseTimerId);
    g_popupClosing = false;
    g_popupCloseDeadline = 0;
}

void HideExpandedPopupImmediately(HWND hwnd) {
    CancelPopupClose(hwnd);
    if (hwnd && IsWindow(hwnd)) ShowWindow(hwnd, SW_HIDE);
    UpdateWidgetHoverVisual();
}

void BeginCloseExpandedPopup(HWND hwnd) {
    if (!hwnd || !IsWindowVisible(hwnd) || g_popupClosing) return;
    g_popupClosing = true;
    if (!AnimatePopupTransition(false)) {
        HideExpandedPopupImmediately(hwnd);
        return;
    }
    g_popupCloseDeadline = GetTickCount64() + kPopupCloseDurationMs;
    // Asynchronous: keep the XAML island alive for the closing transition.
    if (!SetTimer(hwnd, kPopupCloseTimerId, kPopupCloseDurationMs, nullptr)) {
        HideExpandedPopupImmediately(hwnd);
    }
}

LRESULT CALLBACK ExpandedPopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_THEMECHANGED:
        case WM_SETTINGCHANGE:
        case WM_DWMCOMPOSITIONCHANGED:
            g_popupBackdropDirty = true;
            if (IsWindowVisible(hwnd)) RefreshExpandedPopupData();
            break;

        case WM_SHOWWINDOW:
            if (!wParam) {
                CancelPopupClose(hwnd);

            } else g_popupBackdropDirty = true;
            break;

        case WM_TIMER:
            if (wParam == kPopupCloseTimerId) {
                // A queued timer can outlive KillTimer. The phase and deadline
                // also protect a reopened popup and a later closing transition.
                if (g_popupClosing && GetTickCount64() >= g_popupCloseDeadline)
                    HideExpandedPopupImmediately(hwnd);
                return 0;
            }
            break;

        case WM_NCDESTROY:
            CancelPopupClose(hwnd);
            break;

        case WM_SIZE:
            if (g_popupXamlChild) {
                MoveWindow(g_popupXamlChild, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            }
            return 0;

        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE) {
                POINT cursor{};
                RECT widgetRect{};
                const int primaryButton = GetSystemMetrics(SM_SWAPBUTTON)
                    ? VK_RBUTTON : VK_LBUTTON;
                const bool clickingWidget =
                    (GetAsyncKeyState(primaryButton) & 0x8000) != 0 &&
                    GetCursorPos(&cursor) &&
                    GetElementScreenRect(g_playerGrid, widgetRect) &&
                    PtInRect(&widgetRect, cursor);

                // Tapped will toggle a click on the widget. Hiding here first
                // would make the same click reopen the popup immediately.
                if (!clickingWidget) {
                    BeginCloseExpandedPopup(hwnd);
                }
            } else {
                g_popupBackdropDirty = true;
            }
            break; // Let DefWindowProc assign keyboard focus on activation.
        }

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                BeginCloseExpandedPopup(hwnd);
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ToggleExpandedPopup() {
    if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) {
        if (g_popupClosing) {
            CancelPopupClose(g_expandedPopup);
            AnimatePopupTransition(true);
            SetForegroundWindow(g_expandedPopup);
            g_popupBackdropDirty = true;
            RefreshExpandedPopupData();
            UpdateWidgetHoverVisual();
        } else {
            BeginCloseExpandedPopup(g_expandedPopup);
        }
        return;
    }

    if (!g_expandedPopup || !IsWindow(g_expandedPopup)) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = ExpandedPopupWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = kExpandedPopupWindowClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        // Replace a stale registration before assigning this module's WndProc.
        UnregisterClassW(kExpandedPopupWindowClass, wc.hInstance);
        if (!RegisterClassExW(&wc)) {
            Wh_Log(L"Popup class registration failed: %lu", GetLastError());
            return;
        }

        g_expandedPopup = CreateWindowExW(
            // Keep a redirected host surface for the popup's composition backdrop.
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            kExpandedPopupWindowClass,
            L"",
            WS_POPUP,
            0, 0, 540, kExpandedPopupHeightDip,
            g_taskbarWnd, nullptr, GetModuleHandleW(nullptr), nullptr);

        if (!g_expandedPopup || !InitializePopupXamlHost(g_expandedPopup)) {
            if (g_expandedPopup) {
                DestroyWindow(g_expandedPopup);
                g_expandedPopup = nullptr;
            }
            return;
        }
    }

    g_popupDayOffset = 0;
    ShowPopupHolidayList(false);
    g_lastPopupWindowRect = {};
    RefreshExpandedPopupData();

    ResetPopupScrollPosition(g_popupHoroscopeScroll);
    ResetPopupScrollPosition(g_popupHolidayScroll);
    CancelPopupClose(g_expandedPopup);
    AnimatePopupTransition(true, true);
    ShowWindow(g_expandedPopup, SW_SHOW);
    PositionExpandedPopup();
    SetForegroundWindow(g_expandedPopup);
    // Activation/visibility can reset Explorer-owned transient materials.
    // Apply again with a visible, sized HWND instead of caching the pre-show call.
    g_popupBackdropDirty = true;
    RefreshExpandedPopupData();
    UpdateWidgetHoverVisual();
}

void RemoveInjectedTaskbarWidget() {
    // Settings changes and unload must not leave a timer targeting this DLL.
    HideExpandedPopupImmediately(g_expandedPopup);

    if (g_injectedParentPanel) {
        try {
            uint32_t idx = 0;
            if (g_playerGrid && g_injectedParentPanel.Children().IndexOf(g_playerGrid, idx)) {
                g_injectedParentPanel.Children().RemoveAt(idx);
            }
            auto grid = g_injectedParentPanel.try_as<wuxc::Grid>();
            if (grid && g_injectedColumnDef) {
                uint32_t colIdx = 0;
                if (grid.ColumnDefinitions().IndexOf(g_injectedColumnDef, colIdx)) {
                    // Undo our insertion. Otherwise repeated position changes shift
                    // the native tray controls farther right on every update.
                    for (auto const& saved : g_shiftedTrayChildren) {
                        // Restore only properties that this mod successfully changed.
                        // Do not overwrite a later, different change by another mod.
                        if (auto element = saved.element.get()) {
                            if (saved.moved && wuxc::Grid::GetColumn(element) == saved.column + 1)
                                wuxc::Grid::SetColumn(element, saved.column);
                            if (saved.widened && wuxc::Grid::GetColumnSpan(element) == saved.span + 1)
                                wuxc::Grid::SetColumnSpan(element, saved.span);
                        }
                    }
                    grid.ColumnDefinitions().RemoveAt(colIdx);
                }
            }
            if (grid && g_injectedImplicitColumnDef) {
                uint32_t colIdx = 0;
                if (grid.ColumnDefinitions().IndexOf(g_injectedImplicitColumnDef, colIdx))
                    grid.ColumnDefinitions().RemoveAt(colIdx);
            }
        } catch (...) {}
    }

    g_playerGrid = nullptr;
    g_widgetBadgeBorder = nullptr;
    g_widgetZodiacText = nullptr;
    g_widgetLine1Text = nullptr;
    g_widgetLine2Text = nullptr;
    g_injectedParentPanel = nullptr;
    g_injectedColumnDef = nullptr;
    g_injectedImplicitColumnDef = nullptr;
    g_shiftedTrayChildren.clear();
    g_widgetHovered = false;
    g_currentInjectedPosition.clear();
    g_widgetRotation = {};
}

void DestroyTaskbarWidgetAndPopupOnThread() {
    if (g_taskbarWnd) {
        KillTimer(g_taskbarWnd, kTaskbarEnsureTimerId);
    }
    g_taskbarTimerStarted = false;
    RemoveInjectedTaskbarWidget();

    ClearPopupCompositionBackdrop();
    g_popupBackdropLayer = nullptr;
    if (g_popupXamlSource) {
        try { g_popupXamlSource.Close(); } catch (...) {}
        g_popupXamlSource = nullptr;
    }

    g_popupRootCard = nullptr;
    g_popupBadgeBorder = nullptr;
    g_popupZodiacText = nullptr;
    g_popupTitleSolar = nullptr;
    g_popupTitleLunar = nullptr;
    g_popupHolidayBanner = nullptr;
    g_popupHolidayText = nullptr;
    g_popupRowBattu = nullptr;
    g_popupRowMenhTruc = nullptr;
    g_popupRowGioHoangDao = nullptr;
    g_popupRowNenKy = nullptr;
    g_popupRowXungHuong = nullptr;
    ReleasePopupScrollControls();
    g_popupXamlChild = nullptr;

    if (g_expandedPopup && IsWindow(g_expandedPopup)) {
        DestroyWindow(g_expandedPopup);
        g_expandedPopup = nullptr;
    }
    // The class contains a WndProc in this DLL; don't retain it across mod reloads.
    UnregisterClassW(kExpandedPopupWindowClass, GetModuleHandleW(nullptr));
    g_taskbarWnd = nullptr;
}

wuxc::Border BuildTaskbarWidgetElement(ModSettings const& settings) {
    wuxc::Border rootBorder;
    rootBorder.Name(kTaskbarWidgetElementName);
    rootBorder.Height(38.0);
    rootBorder.Width(static_cast<double>(settings.widgetWidth));
    rootBorder.MaxWidth(static_cast<double>(settings.widgetWidth));
    rootBorder.CornerRadius({6, 6, 6, 6});
    rootBorder.BorderThickness({1, 1, 1, 1});
    rootBorder.Padding({6, 3, 10, 3});
    rootBorder.VerticalAlignment(wux::VerticalAlignment::Center);
    rootBorder.IsHitTestVisible(true);
    rootBorder.Visibility(wux::Visibility::Visible);
    rootBorder.Opacity(1.0);
    rootBorder.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));

    wuxc::Grid contentGrid;
    wuxc::ColumnDefinition colIcon;
    colIcon.Width(wux::GridLengthHelper::Auto());
    wuxc::ColumnDefinition colText;
    colText.Width(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
    contentGrid.ColumnDefinitions().Append(colIcon);
    contentGrid.ColumnDefinitions().Append(colText);

    wuxc::Border badgeBorder;
    badgeBorder.Width(28.0);
    badgeBorder.Height(28.0);
    badgeBorder.CornerRadius({6, 6, 6, 6});
    badgeBorder.Margin({0, 0, 8, 0});
    badgeBorder.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock zodiacTb;
    zodiacTb.FontSize(16.5);
    zodiacTb.HorizontalAlignment(wux::HorizontalAlignment::Center);
    zodiacTb.VerticalAlignment(wux::VerticalAlignment::Center);
    badgeBorder.Child(zodiacTb);

    wuxc::Grid::SetColumn(badgeBorder, 0);
    contentGrid.Children().Append(badgeBorder);

    wuxc::StackPanel textStack;
    textStack.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock line1Tb;
    line1Tb.FontSize(11.8);
    line1Tb.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    line1Tb.TextTrimming(wux::TextTrimming::None);
    line1Tb.MaxLines(1);

    // Keep the requested prefix/year visible at the existing widget width.
    // Only shrink this line when necessary; never enlarge small text.
    wuxc::Viewbox line1View;
    line1View.Stretch(wuxm::Stretch::Uniform);
    line1View.StretchDirection(wuxc::StretchDirection::DownOnly);
    line1View.HorizontalAlignment(wux::HorizontalAlignment::Left);
    line1View.Child(line1Tb);

    wuxc::TextBlock line2Tb;
    line2Tb.FontSize(10.8);
    line2Tb.Margin({0, 1, 0, 0});
    line2Tb.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    line2Tb.MaxLines(1);

    textStack.Children().Append(line1View);
    textStack.Children().Append(line2Tb);

    wuxc::Grid::SetColumn(textStack, 1);
    contentGrid.Children().Append(textStack);

    rootBorder.Child(contentGrid);

    rootBorder.PointerEntered([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const& args) {
        g_widgetHovered = true;
        UpdateWidgetHoverVisual();
        args.Handled(true);
    });

    rootBorder.PointerExited([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const& args) {
        g_widgetHovered = false;
        UpdateWidgetHoverVisual();
        args.Handled(true);
    });

    rootBorder.PointerPressed([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const& args) {
        args.Handled(true);
    });

    rootBorder.Tapped([](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& args) {
        if (!g_shuttingDown) ToggleExpandedPopup();
        args.Handled(true);
    });

    g_widgetBadgeBorder = badgeBorder;
    g_widgetZodiacText = zodiacTb;
    g_widgetLine1Text = line1Tb;
    g_widgetLine2Text = line2Tb;

    return rootBorder;
}

void EnsureTaskbarWidgetInjectedOnThread() {
    if (g_shuttingDown || g_ensuringTaskbarWidget) return;
    struct EnsureGuard {
        EnsureGuard() { g_ensuringTaskbarWidget = true; }
        ~EnsureGuard() { g_ensuringTaskbarWidget = false; }
    } ensureGuard;

    ModSettings settings = GetSettings();
    if (!settings.enableTaskbarWidget) {
        if (g_taskbarWnd) KillTimer(g_taskbarWnd, kTaskbarEnsureTimerId);
        g_taskbarTimerStarted = false;
        RemoveInjectedTaskbarWidget();
        ReportWidgetStatus(L"Disabled in settings.");
        return;
    }

    if (!g_taskbarWnd || !IsWindow(g_taskbarWnd)) {
        g_taskbarTimerStarted = false;
        g_taskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!g_taskbarWnd) return;
    }

    if (GetWindowThreadProcessId(g_taskbarWnd, nullptr) != GetCurrentThreadId()) {
        Wh_Log(L"Injection must run on the taskbar UI thread.");
        return;
    }
    if (!g_taskbarTimerStarted) {
        g_taskbarTimerStarted = SetTimer(g_taskbarWnd, kTaskbarEnsureTimerId, 1000,
            [](HWND, UINT, UINT_PTR, DWORD) {
                EnsureTaskbarWidgetInjectedOnThread();
            }) != 0;
        if (!g_taskbarTimerStarted) Wh_Log(L"SetTimer failed: %lu", GetLastError());
    }

    if (g_playerGrid && g_injectedParentPanel) {
        try {
            if (g_currentInjectedPosition != settings.position) {
                RemoveInjectedTaskbarWidget();
            } else {
                uint32_t idx = 0;
                if (g_injectedParentPanel.Children().IndexOf(g_playerGrid, idx) &&
                    g_playerGrid.XamlRoot() != nullptr) {
                    RefreshTaskbarWidgetData();
                    if (g_playerGrid.ActualWidth() <= 0 || g_playerGrid.ActualHeight() <= 0 ||
                        g_playerGrid.Visibility() != wux::Visibility::Visible) {
                        ReportWidgetStatus(L"Attached but has no visible size; check taskbar styles/layout.");
                    }
                    return;
                }
                RemoveInjectedTaskbarWidget();
            }
        } catch (...) {
            RemoveInjectedTaskbarWidget();
        }
    }

    try {
        wux::XamlRoot xamlRoot = GetTaskbarXamlRoot(g_taskbarWnd);
        if (!xamlRoot) return;

        auto rootElement = xamlRoot.Content().try_as<wux::FrameworkElement>();
        if (!rootElement) {
            ReportWidgetStatus(L"Waiting for XamlRoot.Content.");
            return;
        }

        if (auto oldWidget = FindChildByName(rootElement, kTaskbarWidgetElementName)) {
            if (auto oldParent = wuxm::VisualTreeHelper::GetParent(oldWidget).try_as<wuxc::Panel>()) {
                uint32_t oldIdx = 0;
                if (oldParent.Children().IndexOf(oldWidget, oldIdx)) {
                    oldParent.Children().RemoveAt(oldIdx);
                }
            }
        }

        InjectionTarget target = ResolveInjectionTarget(rootElement, settings);
        if (!target.panel) {
            ReportWidgetStatus(L"Waiting for the requested TaskbarFrame.RootGrid or system-tray target.");
            LogTrayLookupFailure(rootElement, settings.position.c_str());
            return;
        }

        wuxc::Border widget = BuildTaskbarWidgetElement(settings);
        // Set ownership before mutation so an exception can roll back insertion.
        g_playerGrid = widget;
        g_injectedParentPanel = target.panel;
        auto targetGrid = target.panel.try_as<wuxc::Grid>();

        if (target.overlay) {
            widget.HorizontalAlignment(wux::HorizontalAlignment::Left);
            widget.Margin({static_cast<double>(settings.widgetLeftOffset), 0.0, 0.0, 0.0});
            wuxc::Grid::SetColumn(widget, 0);
            wuxc::Grid::SetColumnSpan(widget, std::max(1, static_cast<int>(targetGrid.ColumnDefinitions().Size())));
            wuxc::Grid::SetRow(widget, 0);
            wuxc::Grid::SetRowSpan(widget, std::max(1, static_cast<int>(targetGrid.RowDefinitions().Size())));
            wuxc::Canvas::SetZIndex(widget, 1000);
            target.panel.Children().Append(widget);
            g_injectedColumnDef = nullptr;
        } else if (target.childIndex >= 0) {
            widget.HorizontalAlignment(wux::HorizontalAlignment::Center);
            widget.Margin({4.0, 0.0, 4.0, 0.0});
            int index = ClampVal(target.childIndex, 0, static_cast<int>(target.panel.Children().Size()));
            target.panel.Children().InsertAt(index, widget);
        } else {
            wuxc::ColumnDefinition colDef;
            colDef.Width(wux::GridLengthHelper::Auto());
            auto cols = targetGrid.ColumnDefinitions();
            if (cols.Size() == 0) {
                // Preserve the Grid's implicit original column before inserting ours.
                wuxc::ColumnDefinition implicitColumn;
                implicitColumn.Width(wux::GridLengthHelper::Auto());
                cols.Append(implicitColumn);
                g_injectedImplicitColumnDef = implicitColumn;
            }
            int insertCol = ClampVal(target.column, 0, static_cast<int>(cols.Size()));

            cols.InsertAt(insertCol, colDef);
            g_injectedColumnDef = colDef;
            for (auto child : target.panel.Children()) {
                // Children() yields UIElement; Grid's attached-property APIs
                // require FrameworkElement in Windows.UI.Xaml (UWP).
                auto element = child.try_as<wux::FrameworkElement>();
                if (!element) continue;
                int c = wuxc::Grid::GetColumn(element);
                int span = wuxc::Grid::GetColumnSpan(element);
                if (c >= insertCol || c + span > insertCol) {
                    g_shiftedTrayChildren.push_back({winrt::make_weak(element), c, span, false, false});
                    auto& saved = g_shiftedTrayChildren.back();
                    if (c >= insertCol) {
                        wuxc::Grid::SetColumn(element, c + 1);
                        saved.moved = true;
                    } else {
                        wuxc::Grid::SetColumnSpan(element, span + 1);
                        saved.widened = true;
                    }
                }
            }

            widget.HorizontalAlignment(wux::HorizontalAlignment::Center);
            widget.Margin({4.0, 0.0, 4.0, 0.0});
            wuxc::Grid::SetColumn(widget, insertCol);
            wuxc::Canvas::SetZIndex(widget, 1000);
            target.panel.Children().Append(widget);
        }

        g_playerGrid = widget;
        g_injectedParentPanel = target.panel;
        g_currentInjectedPosition = settings.position;

        RefreshTaskbarWidgetData();
        Wh_Log(L"Injected: position=%s, parent=%s, column=%d, childIndex=%d, width=%d DIP",
               settings.position.c_str(), winrt::get_class_name(target.panel).c_str(),
               target.column, target.childIndex, settings.widgetWidth);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Injection failed: 0x%08X - %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
        RemoveInjectedTaskbarWidget();
    } catch (...) {
        Wh_Log(L"Unexpected exception during injection.");
        RemoveInjectedTaskbarWidget();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Widget Lifecycle & Taskbar UI Thread Dispatch

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
HHOOK g_pendingPostHook = nullptr;
HWND g_pendingPostWindow = nullptr;

// Called on the taskbar thread, both when consuming the request and on unload.
void UnhookPendingPostHooks(bool discardMessage = true) {
    HHOOK hook;
    HWND window;
    {
        std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
        hook = std::exchange(g_pendingPostHook, nullptr);
        window = std::exchange(g_pendingPostWindow, nullptr);
    }
    if (hook) UnhookWindowsHookEx(hook);
    if (window && discardMessage) {
        MSG msg{};
        UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
        while (PeekMessageW(&msg, window, message, message, PM_REMOVE)) {}
    }
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

void RequestTaskbarWidgetInjection() {
    if (g_shuttingDown) return;
    HWND hwnd = FindCurrentProcessTaskbarWnd();
    if (!hwnd) return; // StartTaskbar/CreateWindowExW will retry when it exists.
    const UINT message = RunFromWindowThreadViaPostMessageMsg();
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId || !message) return;

    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    // Recheck under the lock so no producer can queue a request after teardown.
    if (g_shuttingDown || g_pendingPostHook) return;
    HHOOK hook = SetWindowsHookExW(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = reinterpret_cast<MSG*>(lParam);
                if (msg->message == g_runFromWindowThreadViaPostMessageMsg.load()) {
                    UnhookPendingPostHooks(false);
                    msg->message = WM_NULL;
                    msg->wParam = 0;
                    msg->lParam = 0;
                    if (!g_shuttingDown) EnsureTaskbarWidgetInjectedOnThread();
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        Wh_Log(L"Failed to install taskbar initialization hook: %lu", GetLastError());
        return;
    }
    g_pendingPostHook = hook;
    g_pendingPostWindow = hwnd;
    if (!PostMessageW(hwnd, message, 0, 0)) {
        DWORD error = GetLastError();
        UnhookWindowsHookEx(hook);
        g_pendingPostHook = nullptr;
        g_pendingPostWindow = nullptr;
        Wh_Log(L"Failed to queue taskbar initialization: %lu", error);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
                                DWORD style, int x, int y, int width, int height,
                                HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style,
                                        x, y, width, height, parent, menu, instance, param);
    if (hwnd && !g_shuttingDown) {
        // The class can be supplied as an ATOM, so inspect the created HWND.
        WCHAR actualClass[64]{};
        if (GetClassNameW(hwnd, actualClass, ARRAYSIZE(actualClass)) &&
            _wcsicmp(actualClass, L"Shell_TrayWnd") == 0) {
            RequestTaskbarWidgetInjection();
        }
    }
    return hwnd;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    if (!g_shuttingDown && GetModuleHandleW(L"Taskbar.dll") == module) {
        TryHookTaskbarDllSymbols(module, true);
    }
    return module;
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
    Wh_Log(L"Initializing taskbar widget.");
    g_shuttingDown = false;

    LoadSettings();
    RunFromWindowThreadViaPostMessageMsg();

    TryHookTaskbarDllSymbols(GetModuleHandleW(L"Taskbar.dll"), false);

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBaseModule) {
        Wh_Log(L"Failed to get kernelbase.dll handle");
        return FALSE;
    }
    InstallHook(kernelBaseModule, "LoadLibraryExW", LoadLibraryExW_Hook, &LoadLibraryExW_Original);

    HMODULE user32Module = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    InstallHook(user32Module, "CreateWindowExW", CreateWindowExW_Hook, &CreateWindowExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    TryHookTaskbarDllSymbols(GetModuleHandleW(L"Taskbar.dll"), true);

    if (HWND hTrayWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTrayWnd, [](PVOID) { EnsureTaskbarWidgetInjectedOnThread(); }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    // Detach widget/popup handlers and timers on their owning UI thread.
    g_shuttingDown = true;
    if (HWND hTrayWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTrayWnd,
            [](PVOID) {
                UnhookPendingPostHooks();
                DestroyTaskbarWidgetAndPopupOnThread();
            },
            nullptr);
    }
}



void Wh_ModSettingsChanged() {
    LoadSettings();
    if (HWND hTrayWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTrayWnd,
            [](PVOID) {
                EnsureTaskbarWidgetInjectedOnThread();
                if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) {
                    RefreshExpandedPopupData();
                }
            },
            nullptr);
    }
}
