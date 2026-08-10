// ==WindhawkMod==
// @id              vit-mess-menu-taskbar
// @name            VIT Mess Menu Taskbar Flyout
// @description     Shows the VIT Vellore hostel mess menu on the Windows 11 taskbar, with a native flyout for the full day's menu.
// @version         1.0.0
// @author          ashishkupadhyay
// @github          https://github.com/ashishkupadhyay
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -luser32 -lwindowsapp -lwinhttp
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mess Menu Taskbar Flyout

Puts the VIT Vellore hostel mess menu into the Windows 11 taskbar.

A small button sits next to the system tray showing what is being served right
now, or how long until the next meal starts. Click it and a native-looking
flyout slides up with the full day's menu, split into Breakfast, Lunch, Snacks
and Dinner. Chevrons at the top let you browse to other days.

## What it does

- **Taskbar button** — `Idli - Vada - Khichdi...` while a meal is being served,
  `Lunch starts in 1 hr 20 min` between meals. Or icon only, in compact mode.
- **Flyout** — all four meals, always expanded, with the current meal
  highlighted green and the upcoming meal highlighted yellow.
- **Automatic grouping** — items are sorted into Main Items, Bread & Sides,
  Dairy, Drinks and Dessert, and shown inline to keep the flyout compact.
- **Offline first** — the menu is cached on disk, so the flyout opens instantly
  and works without a network connection.

## Setup

Pick your **Hostel** and **Mess** in the settings, and that is it. The mod
downloads the right file from `messit.vinnovateit.com` by itself and keeps it
up to date. There is nothing to import and no files to manage.

## Meal timings

These are the VIT Vellore timings and are built into the mod:

| Meal | Mon-Fri | Sat & Sun |
| --- | --- | --- |
| Breakfast | 07:00 - 09:00 | 07:30 - 09:30 |
| Lunch | 12:30 - 14:30 | 12:30 - 14:30 |
| Snacks | 16:30 - 18:00 | 16:30 - 18:00 |
| Dinner | 19:00 - 21:00 | 19:00 - 21:00 |

## Updating

Each JSON file on the site covers one month. If the current month is already
cached, nothing is downloaded. If it is missing, the mod retries every few
hours until the site publishes it, and you can force a check with the reload
button at the bottom of the flyout. The previous month's menu is never shown as
if it were the current one.

Cached files live in `%LOCALAPPDATA%\Windhawk\MessMenu`.

## Notes

Requires Windows 11 (22H2 or newer) — it hooks the XAML taskbar, which does not
exist on Windows 10.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hostel: mens
  $name: Hostel
  $options:
  - mens: Men's Hostel (Hostel 1)
  - womens: Women's Hostel (Hostel 2)
- mess: veg
  $name: Mess
  $options:
  - special: Special (Mess 1)
  - veg: Veg (Mess 2)
  - nonveg: Non-Veg (Mess 3)
- buttonMode: expanded
  $name: Taskbar button
  $description: Expanded shows the current meal or the next-meal countdown. Compact shows only the icon.
  $options:
  - expanded: Expanded
  - compact: Compact
- position: tray_left
  $name: Button position
  $description: The first sits in the taskbar's own area; the rest sit inside the system tray, next to the other tray icons.
  $options:
  - taskbar_left: Left edge of the taskbar
  - tray_left: Left of the system tray
  - clock_left: Left of the clock
  - clock_right: Right of the clock
- buttonPaddingLeft: 4
  $name: Button spacing (left)
  $description: Gap in pixels to the left of the button, which also shifts the button to the right. Increase this to move clear of another mod occupying the same spot.
- buttonPaddingRight: 4
  $name: Button spacing (right)
  $description: Gap in pixels to the right of the button.
- reserveTaskbarSpace: true
  $name: Push the taskbar icons aside
  $description: Left edge position only. Reserves the button's width plus its spacing before the taskbar icons, so they move out of the way instead of sitting underneath. Turn this off if another mod already manages that space.
- maxLabelWidth: 180
  $name: Maximum label width
  $description: Longer text is truncated with an ellipsis. Pixels.
- popupWidth: 380
  $name: Flyout width
  $description: Pixels.
- popupCornerRadius: 8
  $name: Flyout corner radius
  $description: Pixels. The meal cards follow automatically, staying concentric with the flyout's own corners.
- showSnacks: true
  $name: Show the Snacks card
- backgroundMode: auto
  $name: Flyout background
  $description: Match Windows follows the built-in Windows 11 flyout styling and ignores the two settings below. Use Custom to match a Taskbar Styler theme instead.
  $options:
  - auto: Match Windows 11
  - custom: Custom colour and blur
- backgroundColor: "#80000000"
  $name: Custom background colour
  $description: "Only used when the background is set to Custom. Hex with the alpha first: #AARRGGBB, or #RRGGBB for fully opaque. The default #80000000 is the Tinted Glass taskbar theme's colour."
- blurAmount: 18
  $name: Custom blur amount
  $description: Only used when the background is set to Custom. Blur radius in pixels, on the same scale Taskbar Styler themes use. The default 18 is the Tinted Glass taskbar theme's value. Set to 0 for a flat surface with no blur.
- autoUpdate: true
  $name: Check for new menus automatically
  $description: When off, the menu is only downloaded when you press the reload button in the flyout.

*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Graphics.Effects.h>

#include <windows.h>
#include <winhttp.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Older MinGW winhttp.h revisions predate some of these.
#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif
#ifndef WINHTTP_OPTION_DECOMPRESSION
#define WINHTTP_OPTION_DECOMPRESSION 118
#endif
#ifndef WINHTTP_DECOMPRESSION_FLAG_ALL
#define WINHTTP_DECOMPRESSION_FLAG_ALL 0x00000003
#endif
#ifndef WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2
#define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 0x00000800
#endif

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Animation;

// ---------------------------------------------------------------------------
// Section 1: settings
// ---------------------------------------------------------------------------

struct ModSettings {
    int  hostel          = 1;       // 1 = men's, 2 = women's
    int  mess            = 2;       // 1 = special, 2 = veg, 3 = non-veg
    bool compact         = false;
    std::wstring position = L"tray_left";
    int  buttonPaddingLeft  = 4;
    int  buttonPaddingRight = 4;
    bool reserveTaskbarSpace = true;
    int  maxLabelWidth   = 180;
    int  popupWidth      = 380;
    int  popupCornerRadius = 8;
    bool showSnacks      = true;
    bool customBackground = false;
    BYTE bgA = 0x80, bgR = 0, bgG = 0, bgB = 0;
    int  blurAmount      = 18;
    bool autoUpdate      = true;
};

static ModSettings g_settings;

// Wh_GetStringSetting never returns null -- it yields L"" when unset or on
// error -- so an empty test is all that is needed. StringSetting is RAII, so
// Wh_FreeStringSetting cannot be missed.
static std::wstring GetStringSetting(PCWSTR key, PCWSTR fallback) {
    WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(key);
    return *value ? std::wstring(value.get()) : std::wstring(fallback);
}

// Accepts #AARRGGBB and #RRGGBB, with or without the leading '#', in either
// case. Six digits mean fully opaque. Returns false on anything malformed so
// the caller can fall back to the default rather than to an invisible flyout.
static bool ParseHexColor(const std::wstring& text, BYTE& a, BYTE& r, BYTE& g,
                          BYTE& b) {
    std::wstring digits;
    for (wchar_t c : text) {
        if (c == L'#' || c == L' ' || c == L'\t') {
            continue;
        }
        if (!iswxdigit(c)) {
            return false;
        }
        digits.push_back(c);
    }

    if (digits.size() != 6 && digits.size() != 8) {
        return false;
    }

    auto nibble = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') {
            return c - L'0';
        }
        if (c >= L'a' && c <= L'f') {
            return c - L'a' + 10;
        }
        return c - L'A' + 10;
    };
    auto byteAt = [&](size_t i) -> BYTE {
        return (BYTE)((nibble(digits[i]) << 4) | nibble(digits[i + 1]));
    };

    size_t offset = 0;
    if (digits.size() == 8) {
        a = byteAt(0);
        offset = 2;
    } else {
        a = 0xFF;
    }
    r = byteAt(offset);
    g = byteAt(offset + 2);
    b = byteAt(offset + 4);
    return true;
}

static void LoadSettings() {
    g_settings.hostel = (GetStringSetting(L"hostel", L"mens") == L"womens") ? 2 : 1;

    std::wstring mess = GetStringSetting(L"mess", L"veg");
    g_settings.mess = (mess == L"special") ? 1 : (mess == L"nonveg") ? 3 : 2;

    g_settings.compact = (GetStringSetting(L"buttonMode", L"expanded") == L"compact");
    g_settings.position = GetStringSetting(L"position", L"tray_left");
    // Wide enough to slide the button across any monitor; the bound is only
    // here to stop a typo pushing it off-screen with no way back.
    g_settings.buttonPaddingLeft =
        std::clamp(Wh_GetIntSetting(L"buttonPaddingLeft"), 0, 4000);
    g_settings.buttonPaddingRight =
        std::clamp(Wh_GetIntSetting(L"buttonPaddingRight"), 0, 4000);
    g_settings.reserveTaskbarSpace =
        Wh_GetIntSetting(L"reserveTaskbarSpace") != 0;

    g_settings.maxLabelWidth = std::clamp(Wh_GetIntSetting(L"maxLabelWidth"), 40, 600);
    g_settings.popupWidth = std::clamp(Wh_GetIntSetting(L"popupWidth"), 260, 900);
    g_settings.popupCornerRadius =
        std::clamp(Wh_GetIntSetting(L"popupCornerRadius"), 0, 32);
    g_settings.showSnacks = Wh_GetIntSetting(L"showSnacks") != 0;

    g_settings.customBackground =
        (GetStringSetting(L"backgroundMode", L"auto") == L"custom");
    std::wstring hexColor = GetStringSetting(L"backgroundColor", L"#80000000");
    if (!ParseHexColor(hexColor, g_settings.bgA, g_settings.bgR, g_settings.bgG,
                       g_settings.bgB)) {
        Wh_Log(L"LoadSettings: could not parse backgroundColor \"%s\", "
               L"using the default",
               hexColor.c_str());
        g_settings.bgA = 0x80;
        g_settings.bgR = 0;
        g_settings.bgG = 0;
        g_settings.bgB = 0;
    }

    g_settings.blurAmount = std::clamp(Wh_GetIntSetting(L"blurAmount"), 0, 100);
    g_settings.autoUpdate = Wh_GetIntSetting(L"autoUpdate") != 0;
}

// ---------------------------------------------------------------------------
// Section 2: domain model
// ---------------------------------------------------------------------------

enum class Meal { Breakfast = 0, Lunch, Snacks, Dinner, Count };

static constexpr int kMealCount = (int)Meal::Count;

static const wchar_t* const kMealNames[kMealCount] = {L"Breakfast", L"Lunch",
                                                      L"Snacks", L"Dinner"};

// Breakfast, Lunch, Snacks, Dinner.
static const wchar_t* const kMealEmoji[kMealCount] = {
    L"\U0001F373", L"\U0001F35B", L"☕", L"\U0001F319"};

static const wchar_t* const kTaskbarEmoji = L"\U0001F37D";

enum class Group { Main = 0, BreadSides, Dairy, Drinks, Dessert, Count };

static constexpr int kGroupCount = (int)Group::Count;

static const wchar_t* const kGroupNames[kGroupCount] = {
    L"Main Items", L"Bread & Sides", L"Dairy", L"Drinks", L"Dessert"};

struct DayMenu {
    std::wstring raw[kMealCount];
};

struct MenuStore {
    int hostel = 0;
    int mess = 0;
    std::map<int, DayMenu> days;  // key = days since 1970-01-01
};

static std::mutex g_dataMutex;
static MenuStore g_store;
static std::wstring g_lastFetchError;

// Bumped on every mutation of g_store, so the taskbar label's cache knows when
// the menu behind it changed.
static std::atomic<uint32_t> g_storeVersion{0};

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_fetching{false};

// ---------------------------------------------------------------------------
// Section 3: date helpers
//
// Hand-rolled civil-date arithmetic (Howard Hinnant's algorithms) rather than
// the C++20 <chrono> calendar types, so the mod does not depend on how
// complete the bundled libstdc++ calendar support happens to be.
// ---------------------------------------------------------------------------

static int DaysFromCivil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int)doe - 719468;
}

static void CivilFromDays(int z, int& y, unsigned& m, unsigned& d) {
    z += 719468;
    const int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = (unsigned)(z - era * 146097);
    const unsigned yoe =
        (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const int yr = (int)yoe + era * 400;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    d = doy - (153 * mp + 2) / 5 + 1;
    m = mp + (mp < 10 ? 3 : -9);
    y = yr + (m <= 2);
}

// 0 = Sunday .. 6 = Saturday.
static int WeekdayFromDays(int z) {
    return (unsigned)(z >= -4 ? (z + 4) % 7 : (z + 5) % 7 + 6);
}

static bool IsWeekend(int dayKey) {
    int weekday = WeekdayFromDays(dayKey);
    return weekday == 0 || weekday == 6;
}

static const wchar_t* const kWeekdayNames[7] = {
    L"Sunday", L"Monday", L"Tuesday", L"Wednesday",
    L"Thursday", L"Friday", L"Saturday"};

static const wchar_t* const kMonthNames[12] = {
    L"January", L"February", L"March",     L"April",   L"May",      L"June",
    L"July",    L"August",   L"September", L"October", L"November", L"December"};

static int TodayKey() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    return DaysFromCivil(st.wYear, st.wMonth, st.wDay);
}

// Seconds elapsed since local midnight.
static int NowSeconds() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    return st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
}

// yyyy * 12 + (mm - 1), so month arithmetic is plain integer arithmetic.
static int MonthKeyFromDayKey(int dayKey) {
    int y;
    unsigned m, d;
    CivilFromDays(dayKey, y, m, d);
    return y * 12 + (int)m - 1;
}

static void MonthKeyToParts(int monthKey, int& year, unsigned& month) {
    year = monthKey / 12;
    month = (unsigned)(monthKey % 12) + 1;
}

static std::wstring FormatLongDate(int dayKey) {
    int y;
    unsigned m, d;
    CivilFromDays(dayKey, y, m, d);
    return std::to_wstring(d) + L" " + kMonthNames[m - 1] + L" " +
           std::to_wstring(y);
}

// ---------------------------------------------------------------------------
// Section 4: meal windows and the "what is happening now" state machine
//
// VIT Vellore timings, deliberately hard-coded: this mod targets one campus.
// ---------------------------------------------------------------------------

struct MealWindow {
    int startSec;
    int endSec;
};

static constexpr int Hm(int hour, int minute) {
    return hour * 3600 + minute * 60;
}

static MealWindow GetMealWindow(Meal meal, bool weekend) {
    switch (meal) {
        case Meal::Breakfast:
            return weekend ? MealWindow{Hm(7, 30), Hm(9, 30)}
                           : MealWindow{Hm(7, 0), Hm(9, 0)};
        case Meal::Lunch:
            return MealWindow{Hm(12, 30), Hm(14, 30)};
        case Meal::Snacks:
            return MealWindow{Hm(16, 30), Hm(18, 0)};
        case Meal::Dinner:
        default:
            return MealWindow{Hm(19, 0), Hm(21, 0)};
    }
}

// "Show the Snacks card" hides a card; it does not change when snacks are
// served. The state machine below therefore always considers all four meals --
// otherwise, with the card hidden, the button would read "Dinner starts in
// 1 hr 20 min" at 17:00 while snacks were actually being served.
static bool MealCardVisible(Meal meal) {
    return meal != Meal::Snacks || g_settings.showSnacks;
}

struct MealState {
    int  currentMeal = -1;   // index into kMealNames, or -1
    int  nextMeal = -1;      // index into kMealNames, or -1
    int  remainingSec = 0;   // to the end of current, or the start of next
    bool nextIsTomorrow = false;
};

static MealState ComputeMealState() {
    MealState state;

    const int todayKey = TodayKey();
    const int nowSec = NowSeconds();
    const bool weekendToday = IsWeekend(todayKey);

    for (int i = 0; i < kMealCount; i++) {
        MealWindow window = GetMealWindow((Meal)i, weekendToday);
        if (nowSec >= window.startSec && nowSec < window.endSec) {
            state.currentMeal = i;
            state.remainingSec = window.endSec - nowSec;
            return state;
        }
    }

    for (int i = 0; i < kMealCount; i++) {
        MealWindow window = GetMealWindow((Meal)i, weekendToday);
        if (window.startSec > nowSec) {
            state.nextMeal = i;
            state.remainingSec = window.startSec - nowSec;
            return state;
        }
    }

    // Past the last meal of the day: count down to tomorrow's breakfast, using
    // tomorrow's weekday to pick the right breakfast window.
    const bool weekendTomorrow = IsWeekend(todayKey + 1);
    MealWindow breakfast = GetMealWindow(Meal::Breakfast, weekendTomorrow);
    state.nextMeal = (int)Meal::Breakfast;
    state.nextIsTomorrow = true;
    state.remainingSec = (24 * 3600 - nowSec) + breakfast.startSec;
    return state;
}

// "1 hr 20 min", "2 hr", "42 min", "<1 min" -- one formatter, used by both the
// taskbar button and the flyout cards so they can never disagree.
static std::wstring FormatCountdown(int seconds) {
    if (seconds <= 0) {
        return L"<1 min";
    }
    int minutes = (seconds + 59) / 60;  // seconds > 0, so minutes >= 1
    if (minutes < 60) {
        return std::to_wstring(minutes) + L" min";
    }
    int hours = minutes / 60;
    int rest = minutes % 60;
    if (rest == 0) {
        return std::to_wstring(hours) + L" hr";
    }
    return std::to_wstring(hours) + L" hr " + std::to_wstring(rest) + L" min";
}

// ---------------------------------------------------------------------------
// Section 5: grouping engine
// ---------------------------------------------------------------------------

static std::wstring Trim(const std::wstring& text) {
    size_t first = text.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return L"";
    }
    size_t last = text.find_last_not_of(L" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Lowercased, with runs of whitespace collapsed to one space.
static std::wstring NormalizeKey(const std::wstring& text) {
    std::wstring result;
    result.reserve(text.size());
    bool pendingSpace = false;
    for (wchar_t c : Trim(text)) {
        if (c == L' ' || c == L'\t') {
            pendingSpace = true;
            continue;
        }
        if (pendingSpace && !result.empty()) {
            result.push_back(L' ');
        }
        pendingSpace = false;
        result.push_back((wchar_t)towlower(c));
    }
    return result;
}

static std::wstring LastWord(const std::wstring& normalized) {
    size_t space = normalized.find_last_of(L' ');
    return space == std::wstring::npos ? normalized
                                       : normalized.substr(space + 1);
}

static bool InList(const std::wstring& value, const wchar_t* const* list,
                   int count) {
    for (int i = 0; i < count; i++) {
        if (value == list[i]) {
            return true;
        }
    }
    return false;
}

// True for "Sweet: Badusha", "Sweet : Badusha", "Fruits: Grapes",
// "Fruit : Banana" -- the site is inconsistent about the space before the colon,
// so match the label and then skip any spaces before requiring the colon.
static bool HasDessertLabel(const std::wstring& key) {
    static const wchar_t* const kLabels[] = {L"sweet", L"sweets", L"fruit",
                                             L"fruits", L"dessert"};
    for (const wchar_t* label : kLabels) {
        size_t length = wcslen(label);
        if (key.size() <= length || key.compare(0, length, label) != 0) {
            continue;
        }
        size_t i = length;
        while (i < key.size() && key[i] == L' ') {
            i++;
        }
        if (i < key.size() && key[i] == L':') {
            return true;
        }
    }
    return false;
}

// Whole-item and last-word matching, never substring matching: a substring test
// on "curd" files "Curd Rice" as dairy, and the last-word rule is what makes
// "Cold Badam Milk", "Iced Lemon Tea" and "Nimbu Sharbat" land in Drinks.
static Group ClassifyItem(const std::wstring& item) {
    const std::wstring key = NormalizeKey(item);
    if (key.empty()) {
        return Group::Main;
    }

    if (HasDessertLabel(key)) {
        return Group::Dessert;
    }

    // Some desserts arrive with no label at all -- "Assorted Ice Cream" is the
    // recurring one. Nothing savoury on this menu contains "ice cream", so a
    // plain substring test is safe here.
    if (key.find(L"ice cream") != std::wstring::npos) {
        return Group::Dessert;
    }

    static const wchar_t* const kDairy[] = {L"curd", L"loose curd",
                                            L"thick curd", L"butter milk",
                                            L"buttermilk"};
    if (InList(key, kDairy, ARRAYSIZE(kDairy))) {
        return Group::Dairy;
    }

    static const wchar_t* const kDrinkTails[] = {L"tea",     L"coffee",
                                                 L"milk",    L"sharbat",
                                                 L"juice",   L"lassi",
                                                 L"shake",   L"buttermilk"};
    if (InList(LastWord(key), kDrinkTails, ARRAYSIZE(kDrinkTails))) {
        return Group::Drinks;
    }

    static const wchar_t* const kBreadSides[] = {L"bread", L"butter", L"jam"};
    if (InList(key, kBreadSides, ARRAYSIZE(kBreadSides))) {
        return Group::BreadSides;
    }

    return Group::Main;
}

struct GroupedMenu {
    std::vector<std::wstring> groups[kGroupCount];
    bool empty = true;
};

static GroupedMenu GroupMenuItems(const std::wstring& raw) {
    GroupedMenu grouped;
    size_t start = 0;
    while (start <= raw.size()) {
        size_t comma = raw.find(L',', start);
        std::wstring piece = Trim(raw.substr(
            start, comma == std::wstring::npos ? std::wstring::npos
                                               : comma - start));
        if (!piece.empty()) {
            grouped.groups[(int)ClassifyItem(piece)].push_back(piece);
            grouped.empty = false;
        }
        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }
    return grouped;
}

static std::wstring JoinItems(const std::vector<std::wstring>& items,
                              size_t limit = 0) {
    std::wstring result;
    size_t count = (limit == 0) ? items.size() : std::min(limit, items.size());
    for (size_t i = 0; i < count; i++) {
        if (i) {
            result += L" • ";
        }
        result += items[i];
    }
    if (limit != 0 && items.size() > count) {
        result += L"…";
    }
    return result;
}

// ---------------------------------------------------------------------------
// Section 6: cache paths and file I/O
// ---------------------------------------------------------------------------

// Windhawk's own per-mod storage directory. Using it rather than a folder of
// our own under %LOCALAPPDATA% means Windhawk deletes the cached menus when the
// mod is removed, so the mod leaves nothing behind.
static std::wstring GetCacheDirectory() {
    WCHAR path[MAX_PATH];
    size_t length = Wh_GetModStoragePath(path, ARRAYSIZE(path));
    if (length == 0 || length >= ARRAYSIZE(path)) {
        return L"";
    }
    return std::wstring(path, length);
}

static bool EnsureCacheDirectory(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }
    if (CreateDirectoryW(path.c_str(), nullptr)) {
        return true;
    }
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

static std::wstring CacheFilePrefix(int hostel, int mess) {
    return L"h" + std::to_wstring(hostel) + L"m" + std::to_wstring(mess) + L"-";
}

static std::wstring CacheFileName(int hostel, int mess, int monthKey) {
    int year;
    unsigned month;
    MonthKeyToParts(monthKey, year, month);
    wchar_t buffer[64];
    swprintf(buffer, ARRAYSIZE(buffer), L"%04d-%02u.json", year, month);
    return CacheFilePrefix(hostel, mess) + buffer;
}

static bool ReadWholeFile(const std::wstring& path, std::string& out) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 ||
        size.QuadPart > 8 * 1024 * 1024) {
        CloseHandle(file);
        return false;
    }

    out.resize((size_t)size.QuadPart);
    DWORD read = 0;
    BOOL ok = ReadFile(file, out.data(), (DWORD)out.size(), &read, nullptr);
    CloseHandle(file);
    if (!ok || read != out.size()) {
        out.clear();
        return false;
    }
    return true;
}

static bool WriteWholeFile(const std::wstring& path, const std::string& data) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(file, data.data(), (DWORD)data.size(), &written,
                        nullptr);
    CloseHandle(file);
    return ok && written == data.size();
}

static std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) {
        return L"";
    }
    // Skip a UTF-8 BOM if the server or an editor left one behind.
    const char* data = utf8.data();
    int size = (int)utf8.size();
    if (size >= 3 && (unsigned char)data[0] == 0xEF &&
        (unsigned char)data[1] == 0xBB && (unsigned char)data[2] == 0xBF) {
        data += 3;
        size -= 3;
    }
    if (size <= 0) {
        return L"";
    }
    int needed = MultiByteToWideChar(CP_UTF8, 0, data, size, nullptr, 0);
    if (needed <= 0) {
        return L"";
    }
    std::wstring result((size_t)needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, data, size, result.data(), needed);
    return result;
}

// ---------------------------------------------------------------------------
// Section 7: JSON parsing (Windows.Data.Json -- no third-party dependency)
// ---------------------------------------------------------------------------

struct ParsedMonth {
    int monthKey = -1;
    std::map<int, DayMenu> days;
};

static bool ParseIsoDate(const std::wstring& text, int& dayKey) {
    if (text.size() < 10) {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        bool shouldBeDash = (i == 4 || i == 7);
        if (shouldBeDash != (text[i] == L'-')) {
            return false;
        }
        if (!shouldBeDash && !iswdigit(text[i])) {
            return false;
        }
    }
    int year = _wtoi(text.substr(0, 4).c_str());
    int month = _wtoi(text.substr(5, 2).c_str());
    int day = _wtoi(text.substr(8, 2).c_str());
    if (year < 1970 || year > 2200 || month < 1 || month > 12 || day < 1 ||
        day > 31) {
        return false;
    }
    dayKey = DaysFromCivil(year, (unsigned)month, (unsigned)day);
    return true;
}

static bool ParseMenuJson(const std::wstring& json, ParsedMonth& out) {
    using namespace winrt::Windows::Data::Json;

    try {
        JsonObject root{nullptr};
        if (!JsonObject::TryParse(json, root) || !root) {
            return false;
        }
        if (!root.HasKey(L"menu")) {
            return false;
        }

        JsonArray dayArray = root.GetNamedArray(L"menu", nullptr);
        if (!dayArray) {
            return false;
        }

        for (uint32_t i = 0; i < dayArray.Size(); i++) {
            JsonObject dayObject{nullptr};
            try {
                dayObject = dayArray.GetObjectAt(i);
            } catch (...) {
                continue;
            }
            if (!dayObject || !dayObject.HasKey(L"date")) {
                continue;
            }

            std::wstring dateText = dayObject.GetNamedString(L"date", L"").c_str();
            int dayKey = 0;
            if (!ParseIsoDate(dateText, dayKey)) {
                continue;
            }

            DayMenu day;
            JsonArray mealArray = dayObject.GetNamedArray(L"menu", nullptr);
            if (mealArray) {
                for (uint32_t j = 0; j < mealArray.Size(); j++) {
                    JsonObject mealObject{nullptr};
                    try {
                        mealObject = mealArray.GetObjectAt(j);
                    } catch (...) {
                        continue;
                    }
                    if (!mealObject) {
                        continue;
                    }
                    int type = (int)mealObject.GetNamedNumber(L"type", 0);
                    if (type < 1 || type > kMealCount) {
                        continue;
                    }
                    day.raw[type - 1] =
                        mealObject.GetNamedString(L"menu", L"").c_str();
                }
            }

            out.days[dayKey] = std::move(day);
            if (out.monthKey < 0) {
                out.monthKey = MonthKeyFromDayKey(dayKey);
            }
        }
    } catch (...) {
        Wh_Log(L"ParseMenuJson: exception while parsing");
        return false;
    }

    return out.monthKey >= 0 && !out.days.empty();
}

// ---------------------------------------------------------------------------
// Section 8: HTTP (WinHTTP)
// ---------------------------------------------------------------------------

static constexpr wchar_t kMenuHost[] = L"messit.vinnovateit.com";
static constexpr DWORD kMaxResponseBytes = 2 * 1024 * 1024;

// Kept so Wh_ModUninit can abort a request that is blocked in the middle of a
// read: closing the handle makes the blocking call return immediately.
static std::atomic<void*> g_activeRequest{nullptr};

static std::wstring BuildMenuPath(int hostel, int mess) {
    return L"/menu-data/hostel-" + std::to_wstring(hostel) + L"-mess-" +
           std::to_wstring(mess) + L".json";
}

static bool HttpGetJson(const std::wstring& path, std::string& out,
                        std::wstring& error) {
    out.clear();
    error.clear();

    HINTERNET session = WinHttpOpen(L"MessMenuWindhawkMod/1.0",
                                    WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                    WINHTTP_NO_PROXY_NAME,
                                    WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        error = L"Could not start a network session";
        return false;
    }

    WinHttpSetTimeouts(session, 10000, 10000, 20000, 20000);

    DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
#ifdef WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3
    protocols |= WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
#endif
    WinHttpSetOption(session, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols,
                     sizeof(protocols));

    DWORD decompression = WINHTTP_DECOMPRESSION_FLAG_ALL;
    WinHttpSetOption(session, WINHTTP_OPTION_DECOMPRESSION, &decompression,
                     sizeof(decompression));

    HINTERNET connection = WinHttpConnect(session, kMenuHost,
                                          INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!connection) {
        error = L"Could not reach the server";
        WinHttpCloseHandle(session);
        return false;
    }

    HINTERNET request = WinHttpOpenRequest(
        connection, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!request) {
        error = L"Could not create the request";
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        return false;
    }

    g_activeRequest.store(request);

    bool success = false;
    if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        error = L"The request could not be sent";
    } else if (!WinHttpReceiveResponse(request, nullptr)) {
        error = L"No response from the server";
    } else {
        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        WinHttpQueryHeaders(request,
                            WINHTTP_QUERY_STATUS_CODE |
                                WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                            WINHTTP_NO_HEADER_INDEX);
        if (status != 200) {
            error = L"The server returned HTTP " + std::to_wstring(status);
        } else {
            success = true;
            char buffer[16384];
            for (;;) {
                if (g_unloading) {
                    error = L"Cancelled";
                    success = false;
                    break;
                }
                DWORD read = 0;
                if (!WinHttpReadData(request, buffer, sizeof(buffer), &read)) {
                    error = L"The download was interrupted";
                    success = false;
                    break;
                }
                if (read == 0) {
                    break;
                }
                if (out.size() + read > kMaxResponseBytes) {
                    error = L"The menu file is unexpectedly large";
                    success = false;
                    break;
                }
                out.append(buffer, read);
            }
            if (success && out.empty()) {
                error = L"The server returned an empty file";
                success = false;
            }
        }
    }

    // Exchange rather than store: if the mod is unloading, StopNetThread may
    // have taken the handle already and closed it to abort us. Exactly one of
    // the two closes it.
    if (void* owned = g_activeRequest.exchange(nullptr)) {
        WinHttpCloseHandle((HINTERNET)owned);
    }
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);

    if (!success) {
        out.clear();
    }
    return success;
}

// ---------------------------------------------------------------------------
// Section 9: cache load, merge and pruning
// ---------------------------------------------------------------------------

static void PruneOldCacheFiles(int hostel, int mess, int keepFromMonthKey) {
    std::wstring directory = GetCacheDirectory();
    if (directory.empty()) {
        return;
    }

    std::wstring pattern = directory + L"\\" + CacheFilePrefix(hostel, mess) +
                           L"*.json";
    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    const std::wstring prefix = CacheFilePrefix(hostel, mess);
    do {
        std::wstring name = findData.cFileName;
        if (name.size() < prefix.size() + 12) {
            continue;
        }
        int year = _wtoi(name.substr(prefix.size(), 4).c_str());
        int month = _wtoi(name.substr(prefix.size() + 5, 2).c_str());
        if (year < 1970 || month < 1 || month > 12) {
            continue;
        }
        int monthKey = year * 12 + month - 1;
        if (monthKey < keepFromMonthKey) {
            std::wstring full = directory + L"\\" + name;
            DeleteFileW(full.c_str());
            Wh_Log(L"PruneOldCacheFiles: removed %s", name.c_str());
        }
    } while (FindNextFileW(find, &findData));

    FindClose(find);
}

// Loads every cached month for the configured hostel/mess into one map.
static void LoadCacheFromDisk() {
    MenuStore store;
    store.hostel = g_settings.hostel;
    store.mess = g_settings.mess;

    std::wstring directory = GetCacheDirectory();
    if (!directory.empty()) {
        std::wstring pattern = directory + L"\\" +
                               CacheFilePrefix(store.hostel, store.mess) +
                               L"*.json";
        WIN32_FIND_DATAW findData{};
        HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
        if (find != INVALID_HANDLE_VALUE) {
            do {
                std::wstring full = directory + L"\\" + findData.cFileName;
                std::string bytes;
                if (!ReadWholeFile(full, bytes)) {
                    continue;
                }
                ParsedMonth parsed;
                if (!ParseMenuJson(Utf8ToWide(bytes), parsed)) {
                    Wh_Log(L"LoadCacheFromDisk: could not parse %s",
                         findData.cFileName);
                    continue;
                }
                for (auto& entry : parsed.days) {
                    store.days[entry.first] = entry.second;
                }
            } while (FindNextFileW(find, &findData));
            FindClose(find);
        }
    }

    Wh_Log(L"LoadCacheFromDisk: %d days loaded for hostel %d mess %d",
         (int)store.days.size(), store.hostel, store.mess);

    std::lock_guard<std::mutex> lock(g_dataMutex);
    g_store = std::move(store);
    g_storeVersion.fetch_add(1);
}

static bool StoreCoversDay(int dayKey) {
    std::lock_guard<std::mutex> lock(g_dataMutex);
    return g_store.days.find(dayKey) != g_store.days.end();
}

static bool StoreCoversMonth(int monthKey) {
    std::lock_guard<std::mutex> lock(g_dataMutex);
    for (auto& entry : g_store.days) {
        if (MonthKeyFromDayKey(entry.first) == monthKey) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Section 10: XAML helpers
// ---------------------------------------------------------------------------

using WindowThreadProc = void (*)(void*);

struct RunFromWindowThreadPayload {
    WindowThreadProc proc = nullptr;
    void* param = nullptr;
    std::atomic<bool> ran{false};
};

// The payload is owned here rather than on the caller's stack, and addressed by
// id. If SendMessageTimeoutW gives up but the message is dispatched afterwards,
// the id is already gone from the map and the late dispatch is a no-op -- where
// a stack address would have been a dangling read into a dead frame.
static std::mutex g_runPayloadsMutex;
[[clang::no_destroy]] static std::optional<
    std::map<UINT_PTR, std::shared_ptr<RunFromWindowThreadPayload>>>
    g_runPayloads{std::in_place};
static std::atomic<UINT_PTR> g_nextRunPayloadId{1};

static UINT GetRunFromWindowThreadMessage() {
    static const UINT kMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return kMsg;
}

static LRESULT CALLBACK RunFromWindowThreadHookProc(int code, WPARAM wParam,
                                                    LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (message->message == GetRunFromWindowThreadMessage()) {
            std::shared_ptr<RunFromWindowThreadPayload> payload;
            {
                std::lock_guard<std::mutex> lock(g_runPayloadsMutex);
                if (g_runPayloads) {
                    auto it = g_runPayloads->find((UINT_PTR)message->lParam);
                    if (it != g_runPayloads->end()) {
                        payload = it->second;
                    }
                }
            }
            // Released the lock first: proc may call back in.
            if (payload) {
                payload->proc(payload->param);
                payload->ran.store(true);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Runs proc on the thread that owns hWnd. A message hook catches the sent
// message, so no window subclassing or extra window is needed. Returns whether
// proc actually ran -- callers, Wh_ModUninit above all, need to know.
static bool RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param) {
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHookProc,
                                   nullptr, threadId);
    if (!hook) {
        return false;
    }

    auto payload = std::make_shared<RunFromWindowThreadPayload>();
    payload->proc = proc;
    payload->param = param;

    const UINT_PTR id = g_nextRunPayloadId.fetch_add(1);
    {
        std::lock_guard<std::mutex> lock(g_runPayloadsMutex);
        (*g_runPayloads)[id] = payload;
    }

    DWORD_PTR result = 0;
    LRESULT sent =
        SendMessageTimeoutW(hWnd, GetRunFromWindowThreadMessage(), 0,
                            (LPARAM)id, SMTO_ABORTIFHUNG, 10000, &result);

    {
        std::lock_guard<std::mutex> lock(g_runPayloadsMutex);
        g_runPayloads->erase(id);
    }
    UnhookWindowsHookEx(hook);

    return sent != 0 && payload->ran.load();
}

static bool IsReadableMemoryRange(const void* address, size_t size) {
    MEMORY_BASIC_INFORMATION info{};
    if (!VirtualQuery(address, &info, sizeof(info))) {
        return false;
    }
    if (info.State != MEM_COMMIT) {
        return false;
    }
    if (info.Protect & (PAGE_NOACCESS | PAGE_GUARD)) {
        return false;
    }
    size_t available = (size_t)((const BYTE*)info.BaseAddress + info.RegionSize -
                                (const BYTE*)address);
    return available >= size;
}

static FrameworkElement FindChildByName(FrameworkElement const& root,
                                        std::wstring_view name,
                                        int depth = 32) {
    if (!root || depth == 0) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
        if (auto found = FindChildByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

static FrameworkElement FindChildByClassName(FrameworkElement const& root,
                                             const wchar_t* className,
                                             int depth = 32) {
    if (!root || depth == 0) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (winrt::get_class_name(child) == className) {
            return child;
        }
        if (auto found = FindChildByClassName(child, className, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Section 11: taskbar XAML root (via taskbar.dll internals)
// ---------------------------------------------------------------------------

// Only the primary taskbar (Shell_TrayWnd) is targeted, so the CSecondaryTaskBand
// symbols are deliberately not resolved: they are non-optional entries in the
// hook array, and failing to resolve them would take the whole mod down for a
// code path that cannot run.
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Std_Ref_Decref_t = void(WINAPI*)(void*);

static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
static TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
static Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

// Written from the taskbar thread, read from the network worker.
static std::atomic<HWND> g_taskbarWnd{nullptr};

static BOOL CALLBACK FindTaskbarWndEnumProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    WCHAR className[32];
    if (GetWindowThreadProcessId(hWnd, &processId) &&
        processId == GetCurrentProcessId() &&
        GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbarWnd = nullptr;
    EnumWindows(FindTaskbarWndEnumProc, reinterpret_cast<LPARAM>(&taskbarWnd));
    return taskbarWnd;
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        Wh_Log(L"GetTaskbarXamlRoot: taskband window not found");
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand) {
        Wh_Log(L"GetTaskbarXamlRoot: taskband pointer is null");
        return nullptr;
    }

    void* expectedVftable = CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = CTaskBand_GetTaskbarHost_Original;
    if (!expectedVftable || !getTaskbarHost) {
        Wh_Log(L"GetTaskbarXamlRoot: symbols not resolved");
        return nullptr;
    }

    void* site = taskBand;
    constexpr int kMaxSlots = 20;
    for (int i = 0;; i++) {
        if (!IsReadableMemoryRange(site, sizeof(void*))) {
            Wh_Log(L"GetTaskbarXamlRoot: unreadable memory at slot %d", i);
            return nullptr;
        }
        if (*(void**)site == expectedVftable) {
            break;
        }
        if (i == kMaxSlots) {
            Wh_Log(L"GetTaskbarXamlRoot: vftable not found in %d slots",
                   kMaxSlots);
            return nullptr;
        }
        site = (void**)site + 1;
    }

    void* hostSharedPtr[2]{};
    getTaskbarHost(site, hostSharedPtr);
    if (!hostSharedPtr[0]) {
        Wh_Log(L"GetTaskbarXamlRoot: TaskbarHost is empty");
        if (hostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(hostSharedPtr[1]);
        }
        return nullptr;
    }

    // Recover the offset of the embedded IUnknown by reading the prologue of
    // TaskbarHost::FrameHeight, which loads it.
    size_t elementOffset = 0;
    bool recognized = false;
#if defined(_M_X64) || defined(__x86_64__)
    {
        const BYTE* code = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (IsReadableMemoryRange(code, 8) && code[0] == 0x48 &&
            code[1] == 0x83 && code[2] == 0xEC && code[4] == 0x48 &&
            code[5] == 0x83 && code[6] == 0xC1 && code[7] <= 0x7F) {
            elementOffset = code[7];
            recognized = true;
        }
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    {
        const DWORD* code = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (IsReadableMemoryRange(code, sizeof(DWORD) * 4) &&
            code[0] == 0xD503237F && (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
            code[2] == 0x910003FD && (code[3] & 0xFFF00FE0) == 0xF8400C00) {
            elementOffset = (code[3] >> 12) & 0xFF;
            recognized = true;
        }
    }
#else
    elementOffset = 0x10;
    recognized = true;
#endif

    if (!recognized ||
        !IsReadableMemoryRange((BYTE*)hostSharedPtr[0] + elementOffset,
                               sizeof(IUnknown*))) {
        Wh_Log(L"GetTaskbarXamlRoot: FrameHeight pattern not recognized");
        if (hostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(hostSharedPtr[1]);
        }
        return nullptr;
    }

    auto* elementUnknown =
        *(IUnknown**)((BYTE*)hostSharedPtr[0] + elementOffset);
    if (!elementUnknown) {
        if (hostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(hostSharedPtr[1]);
        }
        return nullptr;
    }

    FrameworkElement element{nullptr};
    HRESULT hr = elementUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    auto result = element ? element.XamlRoot() : nullptr;
    if (hostSharedPtr[1] && Std_Ref_Decref_Original) {
        Std_Ref_Decref_Original(hostSharedPtr[1]);
    }
    return SUCCEEDED(hr) ? result : nullptr;
}

// ---------------------------------------------------------------------------
// Section 12: theme and brushes
// ---------------------------------------------------------------------------

static bool IsLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

static winrt::Windows::UI::Color MakeColor(BYTE a, BYTE r, BYTE g, BYTE b) {
    return winrt::Windows::UI::Color{a, r, g, b};
}

static SolidColorBrush MakeBrush(winrt::Windows::UI::Color color) {
    return SolidColorBrush(color);
}

static winrt::Windows::UI::Color TextPrimaryColor(bool light) {
    return light ? MakeColor(0xFF, 0x1A, 0x1A, 0x1A)
                 : MakeColor(0xFF, 0xFF, 0xFF, 0xFF);
}

static winrt::Windows::UI::Color TextSecondaryColor(bool light) {
    return light ? MakeColor(0x9E, 0x00, 0x00, 0x00)
                 : MakeColor(0xB0, 0xFF, 0xFF, 0xFF);
}

static winrt::Windows::UI::Color TextTertiaryColor(bool light) {
    return light ? MakeColor(0x73, 0x00, 0x00, 0x00)
                 : MakeColor(0x8A, 0xFF, 0xFF, 0xFF);
}

static winrt::Windows::UI::Color SeparatorColor(bool light) {
    return light ? MakeColor(0x1A, 0x00, 0x00, 0x00)
                 : MakeColor(0x1F, 0xFF, 0xFF, 0xFF);
}

// Windows' own acrylic surfaces blur at roughly 30px; using the same radius is
// what makes "Match Windows 11" read as a built-in flyout.
static constexpr int kNativeBlurAmount = 30;

// The tint Windows itself uses for flyout surfaces, read from Explorer's live
// resource dictionary so it tracks the OS theme instead of being guessed here.
//
// Windows composites acrylic with a luminosity blend that a flat tint cannot
// reproduce exactly, so the brush's luminosity opacity is used as the tint's
// alpha -- close in practice. The clamp keeps the result readable if a future
// Windows build reports something unexpected.
static winrt::Windows::UI::Color NativeFlyoutTint(bool light) {
    static const wchar_t* const kResourceKeys[] = {
        L"AcrylicBackgroundFillColorDefaultBrush",
        L"AcrylicInAppFillColorDefaultBrush",
        L"SystemControlAcrylicElementBrush",
        L"SystemControlAcrylicWindowBrush",
    };

    try {
        auto application = Application::Current();
        if (application) {
            auto resources = application.Resources();
            for (const wchar_t* key : kResourceKeys) {
                auto boxedKey = winrt::box_value(winrt::hstring{key});
                if (!resources.HasKey(boxedKey)) {
                    continue;
                }
                auto value = resources.Lookup(boxedKey);

                if (auto acrylic = value.try_as<AcrylicBrush>()) {
                    double opacity = acrylic.TintOpacity();
                    if (auto luminosity = acrylic.TintLuminosityOpacity()) {
                        opacity = luminosity.Value();
                    }
                    int alpha = (int)(opacity * 255.0 + 0.5);
                    alpha = std::clamp(alpha, 0x99, 0xE6);
                    auto tint = acrylic.TintColor();
                    return MakeColor((BYTE)alpha, tint.R, tint.G, tint.B);
                }
                if (auto solid = value.try_as<SolidColorBrush>()) {
                    return solid.Color();
                }
            }
        }
    } catch (...) {
    }

    // Windows 11's published acrylic defaults, for when the lookup comes up
    // empty.
    return light ? MakeColor(0xD9, 0xFC, 0xFC, 0xFC)
                 : MakeColor(0xD9, 0x2C, 0x2C, 0x2C);
}

// ---------------------------------------------------------------------------
// Section 12b: backdrop blur
//
// AcrylicBrush exposes no blur radius (only TintColor / TintOpacity /
// TintLuminosityOpacity), and its HostBackdrop source renders nothing at all
// inside this flyout. So the backdrop is built as a Composition effect graph
// instead, following Windhawk Taskbar Styler's XamlBlurBrush:
//
//     Compositor.CreateBackdropBrush()  ->  D2D1GaussianBlur  ->  CompositionBrush
//
// Two details that matter, both learned the hard way:
//   * CreateBackdropBrush, *not* CreateHostBackdropBrush. The host variant is
//     pre-blurred by DWM at a fixed radius and ignores the effect graph, which
//     is why every blur value used to look identical.
//   * The result is exposed as a XamlCompositionBrushBase, not as a sprite
//     visual, so XAML clips it to the Border's CornerRadius for free.
//
// Describing a D2D effect to the compositor needs IGraphicsEffectD2D1Interop
// from windows.graphics.effects.interop.h, which Windhawk's toolchain does not
// ship, so it is declared here.
// ---------------------------------------------------------------------------

// CLSID_D2D1GaussianBlur.
static constexpr GUID kGaussianBlurEffectId = {
    0x1FEB6D69,
    0x2FE6,
    0x4AC9,
    {0x8C, 0x58, 0x1D, 0x7F, 0x93, 0xE7, 0xA6, 0xA5}};

// D2D1_GAUSSIANBLUR_PROP_*
static constexpr UINT kBlurPropStandardDeviation = 0;
static constexpr UINT kBlurPropOptimization = 1;
static constexpr UINT kBlurPropBorderMode = 2;

// D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED, D2D1_BORDER_MODE_SOFT.
static constexpr UINT32 kBlurOptimizationBalanced = 1;
static constexpr UINT32 kBorderModeSoft = 0;

// GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT.
static constexpr UINT kPropertyMappingDirect = 1;

// Out-parameters are void** so the ABI types never have to be named; detach_abi
// below hands back the correct interface pointer.
struct IGraphicsEffectD2D1Interop : ::IUnknown {
    virtual HRESULT __stdcall GetEffectId(GUID* id) = 0;
    virtual HRESULT __stdcall GetNamedPropertyMapping(LPCWSTR name, UINT* index,
                                                      UINT* mapping) = 0;
    virtual HRESULT __stdcall GetPropertyCount(UINT* count) = 0;
    virtual HRESULT __stdcall GetProperty(UINT index, void** value) = 0;
    virtual HRESULT __stdcall GetSource(UINT index, void** source) = 0;
    virtual HRESULT __stdcall GetSourceCount(UINT* count) = 0;
};

// This toolchain is MinGW-flavoured, so the IID goes on with __CRT_UUID_DECL;
// clang silently ignores __declspec(uuid(...)) here and winrt::guid_of then
// fails to compile.
// {2FC57384-A068-44D7-A331-30982FCF7177}
__CRT_UUID_DECL(IGraphicsEffectD2D1Interop, 0x2FC57384, 0xA068, 0x44D7, 0xA3,
                0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77)

struct GaussianBlurEffect
    : winrt::implements<GaussianBlurEffect,
                        winrt::Windows::Graphics::Effects::IGraphicsEffect,
                        winrt::Windows::Graphics::Effects::IGraphicsEffectSource,
                        IGraphicsEffectD2D1Interop> {
    winrt::Windows::Graphics::Effects::IGraphicsEffectSource Source{nullptr};
    float BlurAmount = 3.0f;

    // IGraphicsEffect
    winrt::hstring Name() const noexcept { return m_name; }
    void Name(winrt::hstring const& value) noexcept { m_name = value; }

    // IGraphicsEffectD2D1Interop
    HRESULT __stdcall GetEffectId(GUID* id) noexcept final {
        if (!id) {
            return E_INVALIDARG;
        }
        *id = kGaussianBlurEffectId;
        return S_OK;
    }

    HRESULT __stdcall GetNamedPropertyMapping(LPCWSTR name, UINT* index,
                                              UINT* mapping) noexcept final {
        if (!name || !index || !mapping) {
            return E_INVALIDARG;
        }
        if (_wcsicmp(name, L"BlurAmount") == 0) {
            *index = kBlurPropStandardDeviation;
            *mapping = kPropertyMappingDirect;
            return S_OK;
        }
        if (_wcsicmp(name, L"Optimization") == 0) {
            *index = kBlurPropOptimization;
            *mapping = kPropertyMappingDirect;
            return S_OK;
        }
        if (_wcsicmp(name, L"BorderMode") == 0) {
            *index = kBlurPropBorderMode;
            *mapping = kPropertyMappingDirect;
            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT __stdcall GetPropertyCount(UINT* count) noexcept final {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 3;
        return S_OK;
    }

    HRESULT __stdcall GetProperty(UINT index, void** value) noexcept final {
        if (!value) {
            return E_INVALIDARG;
        }
        *value = nullptr;
        try {
            using winrt::Windows::Foundation::IPropertyValue;
            using winrt::Windows::Foundation::PropertyValue;

            IPropertyValue property{nullptr};
            switch (index) {
                case kBlurPropStandardDeviation:
                    property = PropertyValue::CreateSingle(BlurAmount)
                                   .as<IPropertyValue>();
                    break;
                case kBlurPropOptimization:
                    property =
                        PropertyValue::CreateUInt32(kBlurOptimizationBalanced)
                            .as<IPropertyValue>();
                    break;
                case kBlurPropBorderMode:
                    property = PropertyValue::CreateUInt32(kBorderModeSoft)
                                   .as<IPropertyValue>();
                    break;
                default:
                    return E_INVALIDARG;
            }
            *value = winrt::detach_abi(property);
            return S_OK;
        } catch (...) {
            return E_FAIL;
        }
    }

    HRESULT __stdcall GetSource(UINT index, void** source) noexcept final {
        if (!source) {
            return E_INVALIDARG;
        }
        *source = nullptr;
        if (index != 0) {
            return E_INVALIDARG;
        }
        auto copy = Source;
        *source = winrt::detach_abi(copy);
        return S_OK;
    }

    HRESULT __stdcall GetSourceCount(UINT* count) noexcept final {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 1;
        return S_OK;
    }

   private:
    winrt::hstring m_name{L"MessMenuBlur"};
};

// A XAML brush that paints a blurred copy of whatever sits behind it. Assign it
// to a Border's Background like any other brush.
class MessBlurBrush : public XamlCompositionBrushBaseT<MessBlurBrush> {
   public:
    MessBlurBrush(UIElement const& element, float blurAmount,
                  winrt::Windows::UI::Color fallbackColor)
        : m_compositor(winrt::Windows::UI::Xaml::Hosting::
                           ElementCompositionPreview::GetElementVisual(element)
                               .Compositor()),
          m_blurAmount(blurAmount),
          m_fallbackColor(fallbackColor) {}

    void OnConnected() {
        if (CompositionBrush()) {
            return;
        }
        try {
            CompositionBrush(CreateEffectBrush());
        } catch (...) {
            Wh_Log(L"MessBlurBrush: effect unavailable, using a solid brush");
            try {
                CompositionBrush(m_compositor.CreateColorBrush(m_fallbackColor));
            } catch (...) {
            }
        }
    }

    void OnDisconnected() {
        try {
            if (auto brush = CompositionBrush()) {
                brush.Close();
                CompositionBrush(nullptr);
            }
        } catch (...) {
        }
    }

   private:
    winrt::Windows::UI::Composition::CompositionBrush CreateEffectBrush() {
        using namespace winrt::Windows::UI::Composition;

        auto backdrop = m_compositor.CreateBackdropBrush();

        auto blur = winrt::make_self<GaussianBlurEffect>();
        blur->Source = CompositionEffectSourceParameter(L"backdrop");
        blur->BlurAmount = m_blurAmount;

        auto factory = m_compositor.CreateEffectFactory(*blur);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"backdrop", backdrop);
        return brush;
    }

    winrt::Windows::UI::Composition::Compositor m_compositor;
    float m_blurAmount;
    winrt::Windows::UI::Color m_fallbackColor;
};

// A minimal "subtle" button template, so our buttons pick up the same quiet
// hover treatment the surrounding taskbar buttons use.
static Style MakeSubtleButtonStyle(bool light) {
    static const wchar_t* kTemplate =
        LR"XAML(<Style TargetType="Button"
       xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
       xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">
  <Setter Property="Background" Value="Transparent"/>
  <Setter Property="BorderThickness" Value="0"/>
  <Setter Property="Padding" Value="0"/>
  <Setter Property="MinWidth" Value="0"/>
  <Setter Property="MinHeight" Value="0"/>
  <Setter Property="UseSystemFocusVisuals" Value="False"/>
  <Setter Property="HorizontalContentAlignment" Value="Stretch"/>
  <Setter Property="VerticalContentAlignment" Value="Center"/>
  <Setter Property="Template">
    <Setter.Value>
      <ControlTemplate TargetType="Button">
        <Border x:Name="Root"
                Background="{TemplateBinding Background}"
                CornerRadius="4"
                Padding="{TemplateBinding Padding}">
          <VisualStateManager.VisualStateGroups>
            <VisualStateGroup x:Name="CommonStates">
              <VisualState x:Name="Normal"/>
              <VisualState x:Name="PointerOver">
                <VisualState.Setters>
                  <Setter Target="Root.Background" Value="%HOVER%"/>
                </VisualState.Setters>
              </VisualState>
              <VisualState x:Name="Pressed">
                <VisualState.Setters>
                  <Setter Target="Root.Background" Value="%PRESSED%"/>
                </VisualState.Setters>
              </VisualState>
              <VisualState x:Name="Disabled">
                <VisualState.Setters>
                  <Setter Target="Root.Opacity" Value="0.35"/>
                </VisualState.Setters>
              </VisualState>
            </VisualStateGroup>
          </VisualStateManager.VisualStateGroups>
          <ContentPresenter Content="{TemplateBinding Content}"
                            HorizontalAlignment="{TemplateBinding HorizontalContentAlignment}"
                            VerticalAlignment="{TemplateBinding VerticalContentAlignment}"/>
        </Border>
      </ControlTemplate>
    </Setter.Value>
  </Setter>
</Style>)XAML";

    std::wstring xaml = kTemplate;
    auto replace = [&xaml](const wchar_t* token, const wchar_t* value) {
        size_t pos = xaml.find(token);
        if (pos != std::wstring::npos) {
            xaml.replace(pos, wcslen(token), value);
        }
    };
    replace(L"%HOVER%", light ? L"#18000000" : L"#20FFFFFF");
    replace(L"%PRESSED%", light ? L"#0C000000" : L"#12FFFFFF");

    try {
        return Markup::XamlReader::Load(xaml).try_as<Style>();
    } catch (...) {
        Wh_Log(L"MakeSubtleButtonStyle: XamlReader failed");
        return nullptr;
    }
}

// Parsing the template is the most expensive thing in building the flyout, and
// it was running three times per open. One instance per theme is enough -- a
// Style is immutable once applied and is happily shared between controls.
[[clang::no_destroy]] static Style g_subtleButtonStyle{nullptr};
static bool g_subtleButtonStyleIsLight = false;

static Style GetSubtleButtonStyle(bool light) {
    if (!g_subtleButtonStyle || g_subtleButtonStyleIsLight != light) {
        g_subtleButtonStyle = MakeSubtleButtonStyle(light);
        g_subtleButtonStyleIsLight = light;
    }
    return g_subtleButtonStyle;
}

// ---------------------------------------------------------------------------
// Section 13: UI state
// ---------------------------------------------------------------------------

// Every global holding a XAML object carries [[clang::no_destroy]]. Wh_ModUninit
// clears them on a normal unload, but it does not run when explorer.exe itself
// terminates (restart, sign-out, reboot) -- there the CRT would run these
// destructors alone on the shutdown thread, releasing UI-thread-affine XAML
// objects after the XAML core is gone.
// https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown
[[clang::no_destroy]] static Button g_taskbarButton{nullptr};
[[clang::no_destroy]] static TextBlock g_taskbarLabel{nullptr};
[[clang::no_destroy]] static Grid g_injectionParent{nullptr};
// -1 means we appended without adding a column (taskbar-area positions).
static int g_injectedColumn = -1;
// The taskbar's icon strip, when we are holding space open in front of it.
[[clang::no_destroy]] static FrameworkElement g_reservedElement{nullptr};
static Thickness g_reservedOriginalMargin{};
static bool g_hasReservedOriginalMargin = false;
static winrt::event_token g_buttonSizeToken{};

[[clang::no_destroy]] static Flyout g_flyout{nullptr};
[[clang::no_destroy]] static Border g_flyoutRoot{nullptr};
[[clang::no_destroy]] static TextBlock g_headerDay{nullptr};
[[clang::no_destroy]] static TextBlock g_headerDate{nullptr};
[[clang::no_destroy]] static Button g_prevDayButton{nullptr};
[[clang::no_destroy]] static Button g_nextDayButton{nullptr};
[[clang::no_destroy]] static StackPanel g_cardsPanel{nullptr};
[[clang::no_destroy]] static TextBlock g_footerText{nullptr};
[[clang::no_destroy]] static FontIcon g_reloadIcon{nullptr};
[[clang::no_destroy]] static ProgressRing g_reloadRing{nullptr};
[[clang::no_destroy]] static DispatcherTimer g_timer{nullptr};

// std::vector is not nullable, so clear() alone would leave the heap buffer for
// the CRT to free at shutdown. The optional wrapper gives a reset() that
// releases it while the mod is still mapped.
[[clang::no_destroy]] static std::optional<std::vector<TextBlock>>
    g_cardCountdowns{std::in_place};
[[clang::no_destroy]] static std::optional<std::vector<Border>> g_cardBorders{
    std::in_place};
[[clang::no_destroy]] static std::optional<std::vector<int>> g_cardMeals{
    std::in_place};

// Every DispatcherTimer we start, so Wh_ModUninit can stop the pending ones.
// A timer that fires into an unmapped image crashes Explorer regardless of any
// g_unloading check inside the callback -- the crash is the call itself.
// Touched only from the taskbar UI thread, so it needs no lock.
[[clang::no_destroy]] static std::optional<std::vector<DispatcherTimer>>
    g_liveTimers{std::in_place};

// The reveal animation's SizeChanged handler races its fallback timer and is
// only detached when one of them wins; if neither has by unload, detach here.
[[clang::no_destroy]] static Border g_revealTarget{nullptr};
static winrt::event_token g_revealSizeToken{};
[[clang::no_destroy]] static Storyboard g_closingStoryboard{nullptr};

// Each of these checks the optional first: Wh_ModUninit reset()s it, and a
// stray callback arriving afterwards must not dereference an empty one.
static void TrackTimer(DispatcherTimer const& timer) {
    if (!g_liveTimers) {
        return;
    }
    try {
        g_liveTimers->push_back(timer);
    } catch (...) {
    }
}

static void UntrackTimer(DispatcherTimer const& timer) {
    if (!g_liveTimers) {
        return;
    }
    try {
        auto& timers = *g_liveTimers;
        timers.erase(std::remove(timers.begin(), timers.end(), timer),
                     timers.end());
    } catch (...) {
    }
}

static void StopAllTimers() {
    if (!g_liveTimers) {
        return;
    }
    try {
        for (auto& timer : *g_liveTimers) {
            try {
                timer.Stop();
            } catch (...) {
            }
        }
        g_liveTimers->clear();
    } catch (...) {
    }
}

static std::atomic<bool> g_flyoutOpen{false};
static std::atomic<bool> g_flyoutClosingAnimStarted{false};
static std::atomic<bool> g_flyoutClosingAnimInProgress{false};
static int g_dayOffset = 0;
static double g_flyoutAnimSign = 1.0;

static std::wstring g_lastLabelText;
static long long g_lastLabelKey = LLONG_MIN;
static int g_lastRenderedStateKey = INT_MIN;

static void InvalidateLabelCache() {
    g_lastLabelText.clear();
    g_lastLabelKey = LLONG_MIN;
}

static void RenderFlyoutPage();
static void UpdateTaskbarLabel();
static void KickFetch();
static void ApplyTimerInterval();

static std::wstring HostelDisplayName() {
    return g_settings.hostel == 2 ? L"Women's Hostel" : L"Men's Hostel";
}

static std::wstring MessDisplayName() {
    switch (g_settings.mess) {
        case 1:
            return L"Special Mess";
        case 3:
            return L"Non-Veg Mess";
        default:
            return L"Veg Mess";
    }
}

// ---------------------------------------------------------------------------
// Section 14: taskbar button
// ---------------------------------------------------------------------------

static std::wstring ComputeButtonLabel(const MealState& state) {
    const int todayKey = TodayKey();

    // The grouping work happens under the lock, against a reference rather than
    // a copy: copying DayMenu means copying four std::wstrings every time.
    std::lock_guard<std::mutex> lock(g_dataMutex);
    auto it = g_store.days.find(todayKey);
    if (it == g_store.days.end()) {
        return L"No menu";
    }
    const DayMenu& day = it->second;

    if (state.currentMeal >= 0) {
        GroupedMenu grouped = GroupMenuItems(day.raw[state.currentMeal]);
        // Prefer the main dishes: "Idli - Vada" is a more useful glance than
        // "Tea - Coffee - Milk".
        for (int group = 0; group < kGroupCount; group++) {
            if (!grouped.groups[group].empty()) {
                return JoinItems(grouped.groups[group], 4);
            }
        }
        return std::wstring(kMealNames[state.currentMeal]) + L" is being served";
    }

    if (state.nextMeal >= 0) {
        return std::wstring(kMealNames[state.nextMeal]) + L" starts in " +
               FormatCountdown(state.remainingSec);
    }

    return L"No menu";
}

static void UpdateTaskbarLabel() {
    if (!g_taskbarButton) {
        return;
    }
    try {
        MealState state = ComputeMealState();

        // Everything the label depends on, as one cheap integer, so the split /
        // classify / join work only runs when the text can actually have
        // changed. During a meal the label is a dish list, which does not move
        // with the clock at all -- so the countdown is left out of the key
        // there, and the label is then recomputed only when the meal or the
        // cached menu changes.
        const long long minutePart =
            (state.currentMeal >= 0) ? 0 : ((state.remainingSec + 59) / 60);
        const long long key =
            (long long)g_storeVersion.load() * 1000000LL +
            (long long)(state.currentMeal + 1) * 100000LL +
            (long long)(state.nextMeal + 1) * 10000LL +
            (state.nextIsTomorrow ? 5000LL : 0LL) + minutePart;

        if (key == g_lastLabelKey) {
            return;
        }
        g_lastLabelKey = key;

        std::wstring text = ComputeButtonLabel(state);
        if (text == g_lastLabelText) {
            return;
        }
        g_lastLabelText = text;

        // In compact mode there is no label, but the tooltip still carries the
        // full text -- that is the whole point of compact mode.
        if (g_taskbarLabel) {
            g_taskbarLabel.Text(text);
        }

        std::wstring tooltip = text + L"\n" + HostelDisplayName() + L" • " +
                               MessDisplayName();
        ToolTipService::SetToolTip(g_taskbarButton,
                                   winrt::box_value(winrt::hstring{tooltip}));
    } catch (...) {
        Wh_Log(L"UpdateTaskbarLabel: exception");
    }
}

static void ShowMessFlyout(FrameworkElement const& target);

static Button BuildTaskbarButton(bool light) {
    Button button;
    if (auto style = GetSubtleButtonStyle(light)) {
        button.Style(style);
    }
    // Stretch to the tray's full height so the hover/pressed fill matches the
    // neighbouring taskbar buttons instead of hugging the text. The small
    // vertical margin is the inset Windows itself leaves around tray buttons.
    button.Padding({8, 0, 8, 0});
    button.Margin({(double)g_settings.buttonPaddingLeft, 6,
                   (double)g_settings.buttonPaddingRight, 6});
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.HorizontalAlignment(HorizontalAlignment::Center);

    StackPanel panel;
    panel.Orientation(Orientation::Horizontal);
    panel.VerticalAlignment(VerticalAlignment::Center);

    TextBlock icon;
    icon.Text(kTaskbarEmoji);
    icon.FontFamily(FontFamily(L"Segoe UI Emoji"));
    icon.FontSize(14);
    icon.VerticalAlignment(VerticalAlignment::Center);
    panel.Children().Append(icon);

    if (!g_settings.compact) {
        TextBlock label;
        label.FontFamily(FontFamily(L"Segoe UI Variable Text, Segoe UI"));
        label.FontSize(12);
        label.Foreground(MakeBrush(TextPrimaryColor(light)));
        label.VerticalAlignment(VerticalAlignment::Center);
        label.Margin({6, 0, 0, 0});
        label.TextWrapping(TextWrapping::NoWrap);
        label.TextTrimming(TextTrimming::CharacterEllipsis);
        label.MaxWidth((double)g_settings.maxLabelWidth);
        panel.Children().Append(label);
        g_taskbarLabel = label;
    } else {
        g_taskbarLabel = nullptr;
    }

    button.Content(panel);
    button.Click([](winrt::Windows::Foundation::IInspectable const& sender,
                    RoutedEventArgs const&) {
        try {
            if (auto element = sender.try_as<FrameworkElement>()) {
                ShowMessFlyout(element);
            }
        } catch (...) {
            Wh_Log(L"Taskbar button click: exception");
        }
    });

    return button;
}

// Column of the direct child of `grid` that contains an element named `name`.
static int ColumnOfChildContaining(Grid const& grid, const wchar_t* name) {
    uint32_t count = grid.Children().Size();
    for (uint32_t i = 0; i < count; i++) {
        auto child = grid.Children().GetAt(i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name || FindChildByName(child, name, 8)) {
            return Grid::GetColumn(child);
        }
    }
    return -1;
}

static int ResolveInsertColumn(Grid const& trayGrid) {
    const std::wstring& position = g_settings.position;
    int columnCount = (int)trayGrid.ColumnDefinitions().Size();

    if (position == L"tray_left") {
        return 0;
    }
    if (position == L"clock_left" || position == L"clock_right") {
        int clockColumn = ColumnOfChildContaining(trayGrid,
                                                  L"NotificationCenterButton");
        if (clockColumn >= 0) {
            return position == L"clock_left" ? clockColumn : clockColumn + 1;
        }
        // Clock not found -- fall through and append at the end.
    }
    // Fallback: append after everything else.
    return columnCount;
}

static bool IsTaskbarAreaPosition() {
    return g_settings.position == L"taskbar_left";
}

// Holds the taskbar's icon strip clear of the button by widening its left
// margin. With centred icons this shrinks the space they centre within, so they
// stay centred and simply never reach far enough left to collide; with
// left-aligned icons it pushes them right. Driven by the button's SizeChanged,
// so it keeps up as the label text changes width.
static void UpdateReservedSpace() {
    if (!g_reservedElement || !g_taskbarButton || !g_hasReservedOriginalMargin ||
        g_unloading) {
        return;
    }
    try {
        double width = g_taskbarButton.ActualWidth();
        if (width <= 0.0) {
            return;
        }
        double wanted = g_reservedOriginalMargin.Left + width +
                        (double)g_settings.buttonPaddingLeft +
                        (double)g_settings.buttonPaddingRight;

        auto margin = g_reservedElement.Margin();
        if (std::abs(margin.Left - wanted) > 1.0) {
            margin.Left = wanted;
            g_reservedElement.Margin(margin);
        }
    } catch (...) {
    }
}

// Taskbar.TaskbarFrame > Grid#RootGrid -- the grid that spans the whole
// taskbar, as opposed to the system tray's own grid.
static Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    FrameworkElement frame = root;
    if (winrt::get_class_name(root) != L"Taskbar.TaskbarFrame") {
        frame = FindChildByClassName(root, L"Taskbar.TaskbarFrame");
    }
    if (!frame) {
        return nullptr;
    }
    auto rootGrid = FindChildByName(frame, L"RootGrid");
    return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
}

static void RemoveTaskbarButton() {
    // Give the taskbar its own layout back before anything else, so an
    // exception later cannot leave the icons permanently shoved aside.
    try {
        if (g_taskbarButton && g_buttonSizeToken.value) {
            g_taskbarButton.SizeChanged(g_buttonSizeToken);
        }
    } catch (...) {
    }
    g_buttonSizeToken = {};

    try {
        if (g_reservedElement && g_hasReservedOriginalMargin) {
            g_reservedElement.Margin(g_reservedOriginalMargin);
        }
    } catch (...) {
        Wh_Log(L"RemoveTaskbarButton: could not restore the icon strip margin");
    }
    g_reservedElement = nullptr;
    g_hasReservedOriginalMargin = false;

    try {
        if (g_injectionParent && g_taskbarButton) {
            uint32_t index = 0;
            if (g_injectionParent.Children().IndexOf(g_taskbarButton, index)) {
                g_injectionParent.Children().RemoveAt(index);
            }
            if (g_injectedColumn >= 0 &&
                g_injectedColumn <
                    (int)g_injectionParent.ColumnDefinitions().Size()) {
                g_injectionParent.ColumnDefinitions().RemoveAt(
                    (uint32_t)g_injectedColumn);
                uint32_t count = g_injectionParent.Children().Size();
                for (uint32_t i = 0; i < count; i++) {
                    auto child =
                        g_injectionParent.Children().GetAt(i).try_as<FrameworkElement>();
                    if (!child) {
                        continue;
                    }
                    int column = Grid::GetColumn(child);
                    if (column > g_injectedColumn) {
                        Grid::SetColumn(child, column - 1);
                    }
                }
            }
        }
    } catch (...) {
        Wh_Log(L"RemoveTaskbarButton: exception");
    }

    g_taskbarButton = nullptr;
    g_taskbarLabel = nullptr;
    g_injectionParent = nullptr;
    g_injectedColumn = -1;
    InvalidateLabelCache();
}

static bool InjectTaskbarButton() {
    HWND hWnd = g_taskbarWnd.load() ? g_taskbarWnd.load()
                                    : FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"InjectTaskbarButton: taskbar window not found");
        return false;
    }
    g_taskbarWnd.store(hWnd);

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            return false;
        }
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) {
            return false;
        }

        auto trayFrame = FindChildByClassName(root, L"SystemTray.SystemTrayFrame");
        if (!trayFrame) {
            return false;
        }
        auto trayGridElement = FindChildByName(trayFrame, L"SystemTrayFrameGrid");
        auto trayGrid = trayGridElement ? trayGridElement.try_as<Grid>() : nullptr;
        if (!trayGrid) {
            return false;
        }

        const bool light = IsLightTheme();
        Button button = BuildTaskbarButton(light);

        // --- taskbar-area positions: sit in the taskbar's own grid ----------
        if (IsTaskbarAreaPosition()) {
            auto rootGrid = FindTaskbarRootGrid(root);
            if (!rootGrid) {
                Wh_Log(L"InjectTaskbarButton: taskbar RootGrid not found");
                return false;
            }

            button.HorizontalAlignment(HorizontalAlignment::Left);

            // RootGrid may be columned; span it so the alignment above is
            // measured against the whole taskbar rather than one column.
            int columnCount = (int)rootGrid.ColumnDefinitions().Size();
            if (columnCount > 1) {
                Grid::SetColumn(button, 0);
                Grid::SetColumnSpan(button, columnCount);
            }

            rootGrid.Children().Append(button);

            g_taskbarButton = button;
            g_injectionParent = rootGrid;
            g_injectedColumn = -1;

            // Nothing lets us see where another mod has parked itself, so the
            // spacing settings stay the manual escape hatch. What we can do is
            // stop the taskbar's own icons from sitting underneath us.
            if (g_settings.reserveTaskbarSpace) {
                auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
                if (repeater) {
                    g_reservedElement = repeater;
                    g_reservedOriginalMargin = repeater.Margin();
                    g_hasReservedOriginalMargin = true;
                    g_buttonSizeToken = button.SizeChanged(
                        [](winrt::Windows::Foundation::IInspectable const&,
                           SizeChangedEventArgs const&) {
                            UpdateReservedSpace();
                        });
                    UpdateReservedSpace();
                } else {
                    Wh_Log(L"InjectTaskbarButton: TaskbarFrameRepeater not "
                           L"found, cannot reserve space");
                }
            }

            InvalidateLabelCache();
            UpdateTaskbarLabel();
            return true;
        }

        // --- system-tray positions: insert a column into the tray grid ------
        int insertColumn = std::clamp(ResolveInsertColumn(trayGrid), 0,
                                      (int)trayGrid.ColumnDefinitions().Size());

        ColumnDefinition column;
        column.Width({1.0, GridUnitType::Auto});
        if (insertColumn >= (int)trayGrid.ColumnDefinitions().Size()) {
            trayGrid.ColumnDefinitions().Append(column);
        } else {
            trayGrid.ColumnDefinitions().InsertAt((uint32_t)insertColumn, column);
            uint32_t count = trayGrid.Children().Size();
            for (uint32_t i = 0; i < count; i++) {
                auto child = trayGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (!child) {
                    continue;
                }
                int childColumn = Grid::GetColumn(child);
                if (childColumn >= insertColumn) {
                    Grid::SetColumn(child, childColumn + 1);
                }
            }
        }

        Grid::SetColumn(button, insertColumn);
        trayGrid.Children().Append(button);

        g_taskbarButton = button;
        g_injectionParent = trayGrid;
        g_injectedColumn = insertColumn;

        InvalidateLabelCache();
        UpdateTaskbarLabel();
        return true;
    } catch (...) {
        Wh_Log(L"InjectTaskbarButton: exception");
        g_taskbarButton = nullptr;
        g_injectionParent = nullptr;
        g_injectedColumn = -1;
        return false;
    }
}

// ---------------------------------------------------------------------------
// Section 15: flyout content
// ---------------------------------------------------------------------------

// Horizontal gap between the flyout's inner edge and a meal card's edge. Keep
// this in step with the root Border's left/right padding in BuildFlyoutContent.
static constexpr double kCardInset = 14.0;

static double PopupCornerRadius() {
    return (double)g_settings.popupCornerRadius;
}

// Concentric corners: for two nested rounded rectangles to look like they share
// a centre, the inner radius is the outer radius minus the gap between them.
// At small outer radii that goes negative, so fall back to halving -- which is
// the Fluent convention for a card inside a surface. Whichever is larger wins,
// so the result is monotonic and collapses to 0 when the flyout is square.
static double CardCornerRadius() {
    const double outer = PopupCornerRadius();
    if (outer <= 0.0) {
        return 0.0;
    }
    return std::max(outer - kCardInset, outer * 0.5);
}

static TextBlock MakeTextBlock(const std::wstring& text, double fontSize,
                               winrt::Windows::UI::Color color,
                               bool semiBold = false) {
    TextBlock block;
    block.Text(text);
    block.FontSize(fontSize);
    block.FontFamily(FontFamily(L"Segoe UI Variable Text, Segoe UI"));
    block.Foreground(MakeBrush(color));
    if (semiBold) {
        block.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    }
    return block;
}

static Border MakeSeparator(bool light) {
    Border line;
    line.Height(1);
    line.Background(MakeBrush(SeparatorColor(light)));
    line.Margin({0, 6, 0, 6});
    return line;
}

static Border BuildMealCard(Meal meal, const DayMenu& day, bool light,
                            int highlight,  // 0 none, 1 current, 2 next
                            const std::wstring& countdownText) {
    Border card;
    const double cardRadius = CardCornerRadius();
    card.CornerRadius({cardRadius, cardRadius, cardRadius, cardRadius});
    card.Padding({12, 10, 12, 10});
    card.Margin({0, 3, 0, 3});
    card.BorderThickness({1, 1, 1, 1});

    if (highlight == 1) {
        card.Background(MakeBrush(MakeColor(0x2E, 0x4C, 0xAF, 0x50)));
        card.BorderBrush(MakeBrush(MakeColor(0x73, 0x4C, 0xAF, 0x50)));
    } else if (highlight == 2) {
        card.Background(MakeBrush(MakeColor(0x29, 0xFF, 0xC1, 0x07)));
        card.BorderBrush(MakeBrush(MakeColor(0x66, 0xFF, 0xC1, 0x07)));
    } else {
        card.Background(MakeBrush(light ? MakeColor(0x0A, 0x00, 0x00, 0x00)
                                        : MakeColor(0x0D, 0xFF, 0xFF, 0xFF)));
        card.BorderBrush(MakeBrush(MakeColor(0x00, 0x00, 0x00, 0x00)));
    }

    StackPanel content;

    // Header row: emoji + name on the left, countdown on the right.
    Grid header;
    header.ColumnDefinitions().Append([] {
        ColumnDefinition definition;
        definition.Width({1.0, GridUnitType::Auto});
        return definition;
    }());
    header.ColumnDefinitions().Append([] {
        ColumnDefinition definition;
        definition.Width({1.0, GridUnitType::Star});
        return definition;
    }());

    StackPanel titlePanel;
    titlePanel.Orientation(Orientation::Horizontal);
    titlePanel.VerticalAlignment(VerticalAlignment::Center);

    TextBlock emoji;
    emoji.Text(kMealEmoji[(int)meal]);
    emoji.FontFamily(FontFamily(L"Segoe UI Emoji"));
    emoji.FontSize(14);
    emoji.VerticalAlignment(VerticalAlignment::Center);
    titlePanel.Children().Append(emoji);

    auto title = MakeTextBlock(kMealNames[(int)meal], 14,
                               TextPrimaryColor(light), true);
    title.Margin({8, 0, 0, 0});
    title.VerticalAlignment(VerticalAlignment::Center);
    titlePanel.Children().Append(title);

    Grid::SetColumn(titlePanel, 0);
    header.Children().Append(titlePanel);

    auto countdown = MakeTextBlock(countdownText, 11, TextTertiaryColor(light));
    countdown.HorizontalAlignment(HorizontalAlignment::Right);
    countdown.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetColumn(countdown, 1);
    header.Children().Append(countdown);

    content.Children().Append(header);

    GroupedMenu grouped = GroupMenuItems(day.raw[(int)meal]);
    if (grouped.empty) {
        auto empty = MakeTextBlock(L"Not listed", 12, TextTertiaryColor(light));
        empty.Margin({0, 6, 0, 0});
        content.Children().Append(empty);
    } else {
        for (int group = 0; group < kGroupCount; group++) {
            if (grouped.groups[group].empty()) {
                continue;
            }
            auto caption = MakeTextBlock(kGroupNames[group], 10,
                                         TextTertiaryColor(light));
            caption.Margin({0, 8, 0, 2});
            caption.CharacterSpacing(60);
            content.Children().Append(caption);

            auto items = MakeTextBlock(JoinItems(grouped.groups[group]), 12,
                                       TextSecondaryColor(light));
            items.TextWrapping(TextWrapping::Wrap);
            items.LineHeight(17);
            content.Children().Append(items);
        }
    }

    card.Child(content);

    g_cardBorders->push_back(card);
    g_cardCountdowns->push_back(countdown);
    g_cardMeals->push_back((int)meal);
    return card;
}

static void AddMessageBlock(const std::wstring& title,
                            const std::wstring& subtitle, bool light) {
    StackPanel panel;
    panel.Margin({0, 28, 0, 28});
    panel.HorizontalAlignment(HorizontalAlignment::Center);

    auto titleBlock = MakeTextBlock(title, 13, TextPrimaryColor(light), true);
    titleBlock.TextWrapping(TextWrapping::Wrap);
    titleBlock.TextAlignment(TextAlignment::Center);
    panel.Children().Append(titleBlock);

    if (!subtitle.empty()) {
        auto subtitleBlock =
            MakeTextBlock(subtitle, 11, TextTertiaryColor(light));
        subtitleBlock.TextWrapping(TextWrapping::Wrap);
        subtitleBlock.TextAlignment(TextAlignment::Center);
        subtitleBlock.Margin({0, 6, 0, 0});
        panel.Children().Append(subtitleBlock);
    }

    g_cardsPanel.Children().Append(panel);
}

// A cheap fingerprint of everything that affects the rendered page, so the 1 s
// timer can tell "only the countdown moved" from "the whole page must change".
static int ComputeStateKey(const MealState& state) {
    return (TodayKey() * 512) + (state.currentMeal + 1) * 32 +
           (state.nextMeal + 1) * 2 + (state.nextIsTomorrow ? 1 : 0);
}

static void RenderFlyoutPage() {
    if (!g_cardsPanel || !g_headerDay || !g_headerDate) {
        return;
    }

    try {
        const bool light = IsLightTheme();
        const int todayKey = TodayKey();
        const int dayKey = todayKey + g_dayOffset;

        g_headerDay.Text(g_dayOffset == 0
                             ? L"Today"
                             : kWeekdayNames[WeekdayFromDays(dayKey)]);
        g_headerDate.Text(FormatLongDate(dayKey));

        g_cardsPanel.Children().Clear();
        g_cardBorders->clear();
        g_cardCountdowns->clear();
        g_cardMeals->clear();

        DayMenu day;
        bool haveDay = false;
        int minKey = 0;
        int maxKey = 0;
        bool haveRange = false;
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            auto it = g_store.days.find(dayKey);
            if (it != g_store.days.end()) {
                day = it->second;
                haveDay = true;
            }
            if (!g_store.days.empty()) {
                minKey = g_store.days.begin()->first;
                maxKey = g_store.days.rbegin()->first;
                haveRange = true;
            }
        }

        if (g_prevDayButton) {
            g_prevDayButton.IsEnabled(haveRange && dayKey > minKey);
        }
        if (g_nextDayButton) {
            g_nextDayButton.IsEnabled(haveRange && dayKey < maxKey);
        }

        if (!haveDay) {
            if (!StoreCoversMonth(MonthKeyFromDayKey(dayKey))) {
                AddMessageBlock(L"Menu data not available for this month.",
                                g_fetching ? L"Checking now…"
                                           : L"Checking periodically for "
                                             L"updates…",
                                light);
            } else {
                AddMessageBlock(L"No menu for this date.", L"", light);
            }
            g_lastRenderedStateKey = INT_MIN;
            return;
        }

        MealState state = ComputeMealState();
        const bool isToday = (g_dayOffset == 0);

        for (int i = 0; i < kMealCount; i++) {
            if (!MealCardVisible((Meal)i)) {
                continue;
            }

            int highlight = 0;
            std::wstring countdown;
            if (isToday) {
                if (state.currentMeal == i) {
                    highlight = 1;
                    countdown = L"Ends in " + FormatCountdown(state.remainingSec);
                } else if (state.nextMeal == i && !state.nextIsTomorrow) {
                    highlight = 2;
                    countdown =
                        L"Starts in " + FormatCountdown(state.remainingSec);
                }
            }

            g_cardsPanel.Children().Append(
                BuildMealCard((Meal)i, day, light, highlight, countdown));
        }

        g_lastRenderedStateKey = isToday ? ComputeStateKey(state) : INT_MIN;
    } catch (...) {
        Wh_Log(L"RenderFlyoutPage: exception");
    }
}

static void UpdateReloadIndicator() {
    if (!g_reloadIcon || !g_reloadRing) {
        return;
    }
    try {
        bool busy = g_fetching.load();
        g_reloadIcon.Visibility(busy ? Visibility::Collapsed
                                     : Visibility::Visible);
        g_reloadRing.Visibility(busy ? Visibility::Visible
                                     : Visibility::Collapsed);
        g_reloadRing.IsActive(busy);

        std::wstring tooltip = L"Check for a new menu";
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            if (!busy && !g_lastFetchError.empty()) {
                tooltip = L"Last check failed: " + g_lastFetchError;
            }
        }
        ToolTipService::SetToolTip(g_reloadIcon,
                                   winrt::box_value(winrt::hstring{tooltip}));
    } catch (...) {
    }
}

static Border BuildFlyoutContent() {
    const bool light = IsLightTheme();

    Border root;
    root.Width((double)g_settings.popupWidth);
    const double popupRadius = PopupCornerRadius();
    root.CornerRadius({popupRadius, popupRadius, popupRadius, popupRadius});
    root.BorderThickness({1, 1, 1, 1});
    root.BorderBrush(MakeBrush(light ? MakeColor(0x24, 0x00, 0x00, 0x00)
                                     : MakeColor(0x24, 0xFF, 0xFF, 0xFF)));

    // "Match Windows 11" ignores both the custom colour and the custom blur
    // amount by design: the tint comes from Explorer's own acrylic resource and
    // the radius is Windows' own, so the flyout follows the OS rather than
    // whatever those two settings happen to hold.
    const bool custom = g_settings.customBackground;
    const winrt::Windows::UI::Color tintColor =
        custom ? MakeColor(g_settings.bgA, g_settings.bgR, g_settings.bgG,
                           g_settings.bgB)
               : NativeFlyoutTint(light);
    const int blurAmount = custom ? g_settings.blurAmount : kNativeBlurAmount;
    bool blurAttached = false;

    if (blurAmount > 0) {
        try {
            auto blurBrush = winrt::make_self<MessBlurBrush>(
                root, (float)blurAmount,
                MakeColor(0xF2, tintColor.R, tintColor.G, tintColor.B));
            root.Background(blurBrush.as<Brush>());
            blurAttached = true;
        } catch (...) {
            Wh_Log(L"BuildFlyoutContent: could not create the blur brush");
        }
    }
    if (!blurAttached) {
        root.Background(MakeBrush(tintColor));
    }

    Grid layout;
    layout.RowDefinitions().Append([] {
        RowDefinition definition;
        definition.Height({1.0, GridUnitType::Auto});
        return definition;
    }());
    layout.RowDefinitions().Append([] {
        RowDefinition definition;
        definition.Height({1.0, GridUnitType::Star});
        return definition;
    }());
    layout.RowDefinitions().Append([] {
        RowDefinition definition;
        definition.Height({1.0, GridUnitType::Auto});
        return definition;
    }());

    Style buttonStyle = GetSubtleButtonStyle(light);

    // --- header ---------------------------------------------------------
    StackPanel header;

    Grid navRow;
    for (int i = 0; i < 3; i++) {
        ColumnDefinition definition;
        definition.Width(i == 1 ? GridLength{1.0, GridUnitType::Star}
                                : GridLength{1.0, GridUnitType::Auto});
        navRow.ColumnDefinitions().Append(definition);
    }

    auto makeChevron = [&](const wchar_t* glyph, int delta) {
        Button button;
        if (buttonStyle) {
            button.Style(buttonStyle);
        }
        button.Padding({8, 4, 8, 4});
        FontIcon icon;
        icon.Glyph(glyph);
        icon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
        icon.FontSize(12);
        icon.Foreground(MakeBrush(TextSecondaryColor(light)));
        button.Content(icon);
        button.Click([delta](winrt::Windows::Foundation::IInspectable const&,
                             RoutedEventArgs const&) {
            g_dayOffset += delta;
            RenderFlyoutPage();
        });
        return button;
    };

    g_prevDayButton = makeChevron(L"", -1);
    Grid::SetColumn(g_prevDayButton, 0);
    navRow.Children().Append(g_prevDayButton);

    g_headerDay = MakeTextBlock(L"Today", 16, TextPrimaryColor(light), true);
    g_headerDay.HorizontalAlignment(HorizontalAlignment::Center);
    g_headerDay.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetColumn(g_headerDay, 1);
    navRow.Children().Append(g_headerDay);

    g_nextDayButton = makeChevron(L"", 1);
    Grid::SetColumn(g_nextDayButton, 2);
    navRow.Children().Append(g_nextDayButton);

    header.Children().Append(navRow);

    g_headerDate = MakeTextBlock(L"", 12, TextTertiaryColor(light));
    g_headerDate.HorizontalAlignment(HorizontalAlignment::Center);
    g_headerDate.Margin({0, 1, 0, 0});
    header.Children().Append(g_headerDate);

    header.Children().Append(MakeSeparator(light));

    Grid::SetRow(header, 0);
    layout.Children().Append(header);

    // --- scrollable body -------------------------------------------------
    ScrollViewer scroller;
    scroller.VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    scroller.HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
    scroller.HorizontalScrollMode(ScrollMode::Disabled);

    double maxHeight = 520.0;
    try {
        HMONITOR monitor =
            MonitorFromWindow(g_taskbarWnd.load(), MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (monitor && GetMonitorInfo(monitor, &info)) {
            // g_flyoutRoot is not assigned until this function returns, so ask
            // the taskbar button -- it is already in the tree.
            double scale = 1.0;
            if (g_taskbarButton) {
                if (auto xamlRoot = g_taskbarButton.XamlRoot()) {
                    scale = xamlRoot.RasterizationScale();
                }
            }
            if (scale <= 0.0) {
                scale = 1.0;
            }
            double workHeight = (info.rcWork.bottom - info.rcWork.top) / scale;
            maxHeight = std::clamp(workHeight * 0.7, 240.0, 900.0);
        }
    } catch (...) {
    }
    scroller.MaxHeight(maxHeight);

    g_cardsPanel = StackPanel();
    scroller.Content(g_cardsPanel);
    Grid::SetRow(scroller, 1);
    layout.Children().Append(scroller);

    // --- footer ----------------------------------------------------------
    StackPanel footer;
    footer.Children().Append(MakeSeparator(light));

    Grid footerRow;
    footerRow.ColumnDefinitions().Append([] {
        ColumnDefinition definition;
        definition.Width({1.0, GridUnitType::Star});
        return definition;
    }());
    footerRow.ColumnDefinitions().Append([] {
        ColumnDefinition definition;
        definition.Width({1.0, GridUnitType::Auto});
        return definition;
    }());

    g_footerText = MakeTextBlock(
        HostelDisplayName() + L" • " + MessDisplayName(), 11,
        TextTertiaryColor(light));
    g_footerText.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetColumn(g_footerText, 0);
    footerRow.Children().Append(g_footerText);

    Button reloadButton;
    if (buttonStyle) {
        reloadButton.Style(buttonStyle);
    }
    reloadButton.Padding({8, 4, 8, 4});

    Grid reloadContent;
    g_reloadIcon = FontIcon();
    g_reloadIcon.Glyph(L"");
    g_reloadIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    g_reloadIcon.FontSize(12);
    g_reloadIcon.Foreground(MakeBrush(TextSecondaryColor(light)));
    reloadContent.Children().Append(g_reloadIcon);

    g_reloadRing = ProgressRing();
    g_reloadRing.Width(14);
    g_reloadRing.Height(14);
    g_reloadRing.IsActive(false);
    g_reloadRing.Visibility(Visibility::Collapsed);
    reloadContent.Children().Append(g_reloadRing);

    reloadButton.Content(reloadContent);
    reloadButton.Click([](winrt::Windows::Foundation::IInspectable const&,
                          RoutedEventArgs const&) {
        KickFetch();
        UpdateReloadIndicator();
    });
    ToolTipService::SetToolTip(
        reloadButton, winrt::box_value(winrt::hstring{L"Check for a new menu"}));

    Grid::SetColumn(reloadButton, 1);
    footerRow.Children().Append(reloadButton);

    footer.Children().Append(footerRow);
    Grid::SetRow(footer, 2);
    layout.Children().Append(footer);

    // The tint rides on its own Border so it sits above the blurred backdrop
    // but below the content, and carries the padding.
    Border surface;
    surface.CornerRadius({popupRadius, popupRadius, popupRadius, popupRadius});
    surface.Padding({kCardInset, 12, kCardInset, 10});
    if (blurAttached) {
        surface.Background(MakeBrush(tintColor));
    }
    surface.Child(layout);

    root.Child(surface);
    return root;
}

// ---------------------------------------------------------------------------
// Section 16: flyout show / animate / dismiss
// ---------------------------------------------------------------------------

// Gap between the settled flyout and the taskbar edge. Applied as a margin
// inside the flyout, so the slide still starts at the taskbar edge itself.
static constexpr double kTaskbarGap = 12.0;

static bool IsTaskbarAtTop() {
    if (!g_taskbarWnd.load()) {
        return false;
    }
    RECT taskbarRect{};
    if (!GetWindowRect(g_taskbarWnd.load(), &taskbarRect)) {
        return false;
    }
    HMONITOR monitor = MonitorFromWindow(g_taskbarWnd.load(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfo(monitor, &info)) {
        return false;
    }
    int distanceToTop = taskbarRect.top - info.rcMonitor.top;
    int distanceToBottom = info.rcMonitor.bottom - taskbarRect.bottom;
    return distanceToTop < distanceToBottom;
}

static void ClearFlyoutRefs() {
    if (!g_cardBorders) {
        return;  // already reset by Wh_ModUninit
    }
    g_flyoutRoot = nullptr;
    g_headerDay = nullptr;
    g_headerDate = nullptr;
    g_prevDayButton = nullptr;
    g_nextDayButton = nullptr;
    g_cardsPanel = nullptr;
    g_footerText = nullptr;
    g_reloadIcon = nullptr;
    g_reloadRing = nullptr;
    g_cardBorders->clear();
    g_cardCountdowns->clear();
    g_cardMeals->clear();
}

static void ShowMessFlyout(FrameworkElement const& target) {
    if (!target || g_unloading) {
        return;
    }

    try {
        if (g_flyoutOpen && g_flyout) {
            g_flyout.Hide();
            return;
        }

        g_dayOffset = 0;

        Flyout flyout;
        Border content = BuildFlyoutContent();
        g_flyoutRoot = content;

        Grid clipHost;
        clipHost.HorizontalAlignment(HorizontalAlignment::Stretch);
        clipHost.VerticalAlignment(VerticalAlignment::Stretch);
        clipHost.Children().Append(content);
        flyout.Content(clipHost);

        // Strip the presenter's own chrome so only our Border is visible.
        Style presenterStyle(winrt::xaml_typename<FlyoutPresenter>());
        auto addSetter = [&presenterStyle](DependencyProperty property,
                                           winrt::Windows::Foundation::IInspectable value) {
            presenterStyle.Setters().Append(Setter(property, value));
        };
        addSetter(Control::BackgroundProperty(),
                  winrt::box_value(SolidColorBrush(
                      winrt::Windows::UI::Colors::Transparent())));
        addSetter(Control::BorderBrushProperty(),
                  winrt::box_value(SolidColorBrush(
                      winrt::Windows::UI::Colors::Transparent())));
        addSetter(Control::BorderThicknessProperty(),
                  winrt::box_value(Thickness{0, 0, 0, 0}));
        addSetter(Control::PaddingProperty(),
                  winrt::box_value(Thickness{0, 0, 0, 0}));
        addSetter(FrameworkElement::MarginProperty(),
                  winrt::box_value(Thickness{0, 0, 0, 0}));
        addSetter(FrameworkElement::MaxWidthProperty(),
                  winrt::box_value(10000.0));
        addSetter(FrameworkElement::MaxHeightProperty(),
                  winrt::box_value(10000.0));
        flyout.FlyoutPresenterStyle(presenterStyle);

        flyout.Opened([content](auto const&, auto const&) {
            g_flyoutOpen = true;
            g_flyoutClosingAnimStarted.store(false);

            try {
                RenderFlyoutPage();
                UpdateReloadIndicator();
                // Countdowns are on screen now, so speed the tick up at once
                // rather than waiting out the current idle interval.
                ApplyTimerInterval();
            } catch (...) {
            }

            // XAML gives the presenter its own drop shadow; ours is the Border,
            // so drop theirs to avoid a doubled edge.
            try {
                FlyoutPresenter presenter{nullptr};
                auto node = content.as<DependencyObject>();
                for (int i = 0; i < 12 && node; i++) {
                    if (auto candidate = node.try_as<FlyoutPresenter>()) {
                        presenter = candidate;
                        break;
                    }
                    node = VisualTreeHelper::GetParent(node);
                }
                if (presenter) {
                    int count = VisualTreeHelper::GetChildrenCount(presenter);
                    for (int i = 0; i < count; i++) {
                        auto child = VisualTreeHelper::GetChild(presenter, i);
                        if (auto border = child.try_as<Border>()) {
                            border.Shadow(nullptr);
                            break;
                        }
                    }
                }
            } catch (...) {
            }

            content.Opacity(0.0);
            auto transform = content.RenderTransform().try_as<CompositeTransform>();
            if (!transform) {
                transform = CompositeTransform();
                content.RenderTransform(transform);
            }
            transform.TranslateY(0.0);

            auto fired = std::make_shared<bool>(false);
            auto reveal = [content, transform, fired]() mutable {
                if (*fired) {
                    return;
                }
                *fired = true;
                try {
                    double height = content.ActualHeight();
                    if (height <= 0) {
                        height = 420.0;
                    }
                    // Include the margin, otherwise a sliver of the content
                    // stays visible past the flyout's edge at the start.
                    double start = (height + kTaskbarGap) * g_flyoutAnimSign;
                    transform.TranslateY(start);
                    content.Opacity(1.0);

                    Storyboard storyboard;
                    DoubleAnimation animation;
                    animation.From(start);
                    animation.To(0.0);
                    animation.Duration(DurationHelper::FromTimeSpan(
                        std::chrono::milliseconds(250)));
                    CircleEase ease;
                    ease.EasingMode(EasingMode::EaseOut);
                    animation.EasingFunction(ease);
                    Storyboard::SetTarget(animation, content);
                    Storyboard::SetTargetProperty(
                        animation,
                        L"(UIElement.RenderTransform).(CompositeTransform."
                        L"TranslateY)");
                    storyboard.Children().Append(animation);
                    storyboard.Begin();
                } catch (...) {
                }
            };

            // Prefer the first real layout pass; fall back on a short timer,
            // because a re-shown flyout may not raise SizeChanged at all.
            // Both are recorded globally so Wh_ModUninit can cancel whichever
            // has not fired yet.
            g_revealTarget = content;
            g_revealSizeToken = content.SizeChanged(
                [content, reveal](
                    winrt::Windows::Foundation::IInspectable const&,
                    SizeChangedEventArgs const&) mutable {
                    try {
                        if (g_revealSizeToken.value) {
                            content.SizeChanged(g_revealSizeToken);
                            g_revealSizeToken = {};
                            g_revealTarget = nullptr;
                        }
                    } catch (...) {
                    }
                    reveal();
                });

            DispatcherTimer fallback;
            fallback.Interval(winrt::Windows::Foundation::TimeSpan{
                std::chrono::milliseconds(80)});
            auto fallbackToken = std::make_shared<winrt::event_token>();
            *fallbackToken = fallback.Tick(
                [fallback, fallbackToken, reveal](
                    winrt::Windows::Foundation::IInspectable const&,
                    winrt::Windows::Foundation::IInspectable const&) mutable {
                    try {
                        fallback.Stop();
                        fallback.Tick(*fallbackToken);
                        UntrackTimer(fallback);
                    } catch (...) {
                    }
                    reveal();
                });
            TrackTimer(fallback);
            fallback.Start();
        });

        flyout.Closing([content](Primitives::FlyoutBase const& sender,
                                 Primitives::FlyoutBaseClosingEventArgs const& args) {
            if (g_unloading) {
                return;
            }
            if (g_flyoutClosingAnimInProgress.exchange(false)) {
                return;  // this is our own Hide() at the end of the animation
            }
            if (g_flyoutClosingAnimStarted.load()) {
                args.Cancel(true);
                return;
            }

            try {
                auto transform =
                    content.RenderTransform().try_as<CompositeTransform>();
                if (!transform) {
                    return;
                }
                args.Cancel(true);

                double height = content.ActualHeight();
                if (height <= 0) {
                    height = 420.0;
                }
                double end = (height + kTaskbarGap + 8.0) * g_flyoutAnimSign;

                Storyboard storyboard;
                DoubleAnimation animation;
                animation.To(end);
                animation.Duration(DurationHelper::FromTimeSpan(
                    std::chrono::milliseconds(250)));
                CircleEase ease;
                ease.EasingMode(EasingMode::EaseIn);
                animation.EasingFunction(ease);
                Storyboard::SetTarget(animation, content);
                Storyboard::SetTargetProperty(
                    animation,
                    L"(UIElement.RenderTransform).(CompositeTransform."
                    L"TranslateY)");
                storyboard.Children().Append(animation);
                g_closingStoryboard = storyboard;

                g_flyoutClosingAnimStarted.store(true);
                auto hide = [sender]() {
                    g_flyoutClosingAnimStarted.store(false);
                    try {
                        g_flyoutClosingAnimInProgress.store(true);
                        sender.Hide();
                    } catch (...) {
                        g_flyoutClosingAnimInProgress.store(false);
                    }
                };
                storyboard.Completed(
                    [hide](auto const&, auto const&) { hide(); });

                DispatcherTimer safety;
                safety.Interval(winrt::Windows::Foundation::TimeSpan{
                    std::chrono::milliseconds(350)});
                auto safetyToken = std::make_shared<winrt::event_token>();
                *safetyToken = safety.Tick(
                    [safety, safetyToken, hide](
                        winrt::Windows::Foundation::IInspectable const&,
                        winrt::Windows::Foundation::IInspectable const&) mutable {
                        try {
                            safety.Stop();
                            safety.Tick(*safetyToken);
                            UntrackTimer(safety);
                        } catch (...) {
                        }
                        if (g_flyoutClosingAnimStarted.load()) {
                            hide();
                        }
                    });
                TrackTimer(safety);
                safety.Start();
                storyboard.Begin();
            } catch (...) {
                g_flyoutClosingAnimStarted.store(false);
            }
        });

        flyout.Closed([](auto const&, auto const&) {
            g_flyoutOpen = false;
            g_flyoutClosingAnimStarted.store(false);
            g_dayOffset = 0;
            ClearFlyoutRefs();
            ApplyTimerInterval();
        });

        // Anchor above the button (or below it, for a top taskbar).
        const bool atTop = IsTaskbarAtTop();
        g_flyoutAnimSign = atTop ? -1.0 : 1.0;

        // The flyout's own bounds run all the way to the taskbar edge and clip
        // the sliding content; this margin is what keeps the settled flyout
        // clear of it.
        content.Margin(atTop ? Thickness{0, kTaskbarGap, 0, 0}
                             : Thickness{0, 0, 0, kTaskbarGap});

        Primitives::FlyoutPlacementMode placement =
            atTop ? Primitives::FlyoutPlacementMode::Bottom
                  : Primitives::FlyoutPlacementMode::Top;
        winrt::Windows::Foundation::Point anchor{0.f, 0.f};
        FrameworkElement showTarget = target;

        try {
            if (auto xamlRoot = target.XamlRoot()) {
                if (auto rootContent = xamlRoot.Content().try_as<FrameworkElement>()) {
                    showTarget = rootContent;
                    try {
                        flyout.OverlayInputPassThroughElement(rootContent);
                    } catch (...) {
                    }
                    auto transform = target.TransformToVisual(rootContent);
                    auto point = transform.TransformPoint({0.f, 0.f});

                    // Follow the button horizontally, but keep the whole flyout
                    // on the monitor: centring on a button parked at either end
                    // of the taskbar would otherwise push it off-screen, which
                    // is what made every button position look the same.
                    const double width = (double)g_settings.popupWidth;
                    double centreX =
                        point.X + (double)target.ActualWidth() * 0.5;

                    double workLeft = 0.0;
                    double workRight = (double)rootContent.ActualWidth();
                    HMONITOR monitor =
                        MonitorFromWindow(g_taskbarWnd.load(), MONITOR_DEFAULTTONEAREST);
                    MONITORINFO monitorInfo{};
                    monitorInfo.cbSize = sizeof(monitorInfo);
                    POINT origin{0, 0};
                    if (monitor && GetMonitorInfo(monitor, &monitorInfo) &&
                        ClientToScreen(g_taskbarWnd.load(), &origin)) {
                        double scale = xamlRoot.RasterizationScale();
                        if (scale <= 0.0) {
                            scale = 1.0;
                        }
                        workLeft = (monitorInfo.rcWork.left - origin.x) / scale;
                        workRight = (monitorInfo.rcWork.right - origin.x) / scale;
                    }

                    constexpr double kEdgeMargin = 8.0;
                    double left = centreX - width * 0.5;
                    double maxLeft = workRight - kEdgeMargin - width;
                    double minLeft = workLeft + kEdgeMargin;
                    if (maxLeft < minLeft) {
                        maxLeft = minLeft;  // flyout wider than the work area
                    }
                    left = std::clamp(left, minLeft, maxLeft);

                    // Anchor vertically on the taskbar's own edge, not the
                    // button's, so the flyout emerges from exactly that line.
                    // The resting gap comes from the content's margin instead
                    // (set above), so the reveal wipes out of the taskbar edge
                    // while the settled flyout still clears it.
                    anchor = {(float)(left + width * 0.5),
                              atTop ? (float)rootContent.ActualHeight() : 0.0f};
                }
            }
        } catch (...) {
            Wh_Log(L"ShowMessFlyout: could not compute the anchor");
        }

        try {
            flyout.ShouldConstrainToRootBounds(false);
            flyout.Placement(placement);
        } catch (...) {
        }

        g_flyout = flyout;

        Primitives::FlyoutShowOptions options;
        options.Placement(placement);
        options.Position(anchor);
        flyout.ShowAt(showTarget, options);
    } catch (...) {
        Wh_Log(L"ShowMessFlyout: exception");
    }
}

// ---------------------------------------------------------------------------
// Section 17: UI timer
// ---------------------------------------------------------------------------

static int g_lastSeenDayKey = 0;

// A per-second tick is only needed while a live countdown is on screen, which
// means the flyout open on today's page. The taskbar label alone has minute
// granularity, so the rest of the time a much slower tick is indistinguishable.
static constexpr int kFastTickMs = 1000;
static constexpr int kIdleTickMs = 20000;
static int g_timerIntervalMs = kFastTickMs;

static void ApplyTimerInterval() {
    if (!g_timer) {
        return;
    }
    const bool countdownVisible = g_flyoutOpen.load() && g_dayOffset == 0;
    const int wanted = countdownVisible ? kFastTickMs : kIdleTickMs;
    if (wanted == g_timerIntervalMs) {
        return;
    }
    try {
        g_timer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(wanted)});
        g_timerIntervalMs = wanted;
    } catch (...) {
    }
}

static void OnTimerTick() {
    if (g_unloading || !g_cardCountdowns || !g_cardMeals) {
        return;
    }

    try {
        ApplyTimerInterval();
        UpdateTaskbarLabel();

        const int todayKey = TodayKey();
        if (todayKey != g_lastSeenDayKey) {
            g_lastSeenDayKey = todayKey;
            // A new day, possibly a new month: make sure we have its data.
            if (g_settings.autoUpdate && !StoreCoversDay(todayKey)) {
                KickFetch();
            }
            if (g_flyoutOpen) {
                RenderFlyoutPage();
            }
            return;
        }

        if (!g_flyoutOpen || g_dayOffset != 0 || g_cardCountdowns->empty()) {
            return;
        }

        MealState state = ComputeMealState();
        int stateKey = ComputeStateKey(state);
        if (stateKey != g_lastRenderedStateKey) {
            // A meal started or ended: highlights move, so rebuild.
            RenderFlyoutPage();
            return;
        }

        // Same state, only the countdown moved. Touch just that text.
        for (size_t i = 0; i < g_cardCountdowns->size(); i++) {
            int meal = (*g_cardMeals)[i];
            std::wstring text;
            if (state.currentMeal == meal) {
                text = L"Ends in " + FormatCountdown(state.remainingSec);
            } else if (state.nextMeal == meal && !state.nextIsTomorrow) {
                text = L"Starts in " + FormatCountdown(state.remainingSec);
            }
            if ((*g_cardCountdowns)[i].Text() != text) {
                (*g_cardCountdowns)[i].Text(text);
            }
        }
    } catch (...) {
        Wh_Log(L"OnTimerTick: exception");
    }
}

static void StartUiTimer() {
    if (g_timer) {
        return;
    }
    try {
        g_lastSeenDayKey = TodayKey();
        g_timer = DispatcherTimer();
        g_timer.Interval(
            winrt::Windows::Foundation::TimeSpan{std::chrono::seconds(1)});
        g_timer.Tick([](winrt::Windows::Foundation::IInspectable const&,
                        winrt::Windows::Foundation::IInspectable const&) {
            OnTimerTick();
        });
        g_timer.Start();
    } catch (...) {
        Wh_Log(L"StartUiTimer: exception");
        g_timer = nullptr;
    }
}

static void StopUiTimer() {
    if (!g_timer) {
        return;
    }
    try {
        g_timer.Stop();
    } catch (...) {
    }
    g_timer = nullptr;
}

// ---------------------------------------------------------------------------
// Section 18: network worker
// ---------------------------------------------------------------------------

static HANDLE g_netThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_kickEvent = nullptr;

// Reloading the cache means parsing JSON, which means touching WinRT, so it has
// to happen on the worker thread (the only one whose apartment we control) and
// never on whichever thread Windhawk calls Wh_ModSettingsChanged from.
static std::atomic<bool> g_reloadCacheRequested{false};

static constexpr DWORD kIdleIntervalMs = 6 * 60 * 60 * 1000;   // 6 hours
static constexpr DWORD kFirstBackoffMs = 15 * 60 * 1000;       // 15 minutes

static void KickFetch() {
    if (g_kickEvent) {
        SetEvent(g_kickEvent);
    }
}

static void NotifyUiDataChanged() {
    HWND hWnd = g_taskbarWnd.load();
    if (!hWnd || g_unloading) {
        return;
    }
    RunFromWindowThread(
        hWnd,
        [](void*) {
            try {
                InvalidateLabelCache();
                UpdateTaskbarLabel();
                UpdateReloadIndicator();
                if (g_flyoutOpen) {
                    RenderFlyoutPage();
                }
            } catch (...) {
            }
        },
        nullptr);
}

// Returns true when the merged cache now covers today.
static bool PerformFetch() {
    const int hostel = g_settings.hostel;
    const int mess = g_settings.mess;

    g_fetching.store(true);
    NotifyUiDataChanged();

    std::string body;
    std::wstring error;
    bool ok = HttpGetJson(BuildMenuPath(hostel, mess), body, error);

    ParsedMonth parsed;
    if (ok) {
        if (!ParseMenuJson(Utf8ToWide(body), parsed)) {
            ok = false;
            error = L"The menu file could not be read";
        }
    }

    if (ok) {
        std::wstring directory = GetCacheDirectory();
        if (EnsureCacheDirectory(directory)) {
            std::wstring path =
                directory + L"\\" + CacheFileName(hostel, mess, parsed.monthKey);
            if (!WriteWholeFile(path, body)) {
                Wh_Log(L"PerformFetch: could not write %s", path.c_str());
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            if (g_store.hostel != hostel || g_store.mess != mess) {
                g_store.days.clear();
                g_store.hostel = hostel;
                g_store.mess = mess;
            }
            for (auto& entry : parsed.days) {
                g_store.days[entry.first] = entry.second;
            }
            g_lastFetchError.clear();
            g_storeVersion.fetch_add(1);
        }

        // Keep the previous, current and next month; drop anything older.
        PruneOldCacheFiles(hostel, mess, MonthKeyFromDayKey(TodayKey()) - 1);

        int year;
        unsigned month;
        MonthKeyToParts(parsed.monthKey, year, month);
        Wh_Log(L"PerformFetch: got %04d-%02u with %d days", year, month,
             (int)parsed.days.size());
    } else {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        g_lastFetchError = error;
        Wh_Log(L"PerformFetch failed: %s", error.c_str());
    }

    g_fetching.store(false);
    NotifyUiDataChanged();

    return ok && StoreCoversDay(TodayKey());
}

static DWORD WINAPI NetThreadProc(void*) {
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    } catch (...) {
        Wh_Log(L"NetThreadProc: could not initialise the apartment");
    }

    LoadCacheFromDisk();
    NotifyUiDataChanged();

    DWORD backoffMs = kFirstBackoffMs;
    bool forced = false;

    for (;;) {
        if (g_unloading) {
            break;
        }

        if (g_reloadCacheRequested.exchange(false)) {
            LoadCacheFromDisk();
            NotifyUiDataChanged();
        }

        const int todayKey = TodayKey();
        const bool covered = StoreCoversDay(todayKey);
        DWORD waitMs = kIdleIntervalMs;

        if (forced || (!covered && g_settings.autoUpdate)) {
            bool satisfied = PerformFetch();
            if (satisfied) {
                backoffMs = kFirstBackoffMs;
                waitMs = kIdleIntervalMs;
            } else {
                // Either the network failed, or the site is still serving the
                // previous month. Keep the cache either way and try later.
                std::wstring error;
                {
                    std::lock_guard<std::mutex> lock(g_dataMutex);
                    error = g_lastFetchError;
                }
                if (error.empty()) {
                    waitMs = kIdleIntervalMs;   // stale month: flat 6 h
                    backoffMs = kFirstBackoffMs;
                } else {
                    waitMs = backoffMs;
                    backoffMs = std::min<DWORD>(backoffMs * 2, kIdleIntervalMs);
                }
            }
            forced = false;
        } else if (covered) {
            waitMs = kIdleIntervalMs;
        }

        if (g_unloading) {
            break;
        }

        HANDLE handles[2] = {g_stopEvent, g_kickEvent};
        DWORD result = WaitForMultipleObjects(2, handles, FALSE, waitMs);
        if (result == WAIT_FAILED) {
            // Otherwise the loop would spin at 100% CPU on a bad handle.
            Wh_Log(L"NetThreadProc: wait failed, stopping");
            break;
        }
        if (result == WAIT_OBJECT_0) {
            break;
        }
        if (result == WAIT_OBJECT_0 + 1) {
            ResetEvent(g_kickEvent);
            forced = true;          // a manual reload ignores the backoff
            backoffMs = kFirstBackoffMs;
        }
    }

    if (apartmentInitialized) {
        try {
            winrt::uninit_apartment();
        } catch (...) {
        }
    }
    return 0;
}

// Idempotent: called from Wh_ModAfterInit when a taskbar is already present,
// and from the taskbar creation hook otherwise. Without the gate, every
// explorer.exe would run a worker and download the menu -- with "launch folder
// windows in a separate process" enabled that is one per Explorer window.
static void StartNetThread() {
    if (g_netThread) {
        return;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_kickEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_kickEvent) {
        Wh_Log(L"StartNetThread: could not create the events");
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        if (g_kickEvent) {
            CloseHandle(g_kickEvent);
            g_kickEvent = nullptr;
        }
        return;
    }

    g_netThread = CreateThread(nullptr, 0, NetThreadProc, nullptr, 0, nullptr);
    if (!g_netThread) {
        Wh_Log(L"StartNetThread: could not start the worker");
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        CloseHandle(g_kickEvent);
        g_kickEvent = nullptr;
    }
}

static void StopNetThread() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Unblock a read that is already in flight.
    void* request = g_activeRequest.exchange(nullptr);
    if (request) {
        WinHttpCloseHandle((HINTERNET)request);
    }

    if (g_netThread) {
        // INFINITE, deliberately. Windhawk unloads the mod image as soon as
        // Wh_ModUninit returns, so a worker still running mod code after a
        // timed-out wait would be executing unmapped memory. The wait is
        // already bounded by the stop event, the closed WinHTTP handle and the
        // g_unloading checks -- a timeout here could only convert a slow exit
        // into a crash.
        WaitForSingleObject(g_netThread, INFINITE);
        CloseHandle(g_netThread);
        g_netThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_kickEvent) {
        CloseHandle(g_kickEvent);
        g_kickEvent = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Section 19: injection retry and the taskbar creation hook
// ---------------------------------------------------------------------------

// Bumped every time the taskbar is (re)created. A retry chain started for an
// older taskbar carries its generation and gives up when it no longer matches,
// so two chains racing after a quick double restart cannot both inject -- which
// would otherwise leave the visible button unmanaged while the globals point at
// a dead tree.
static int g_injectGeneration = 0;

static void InjectWithRetry(FrameworkElement rootContent, int generation,
                            int attempt = 0) {
    static constexpr int kMaxAttempts = 50;

    if (g_unloading || generation != g_injectGeneration) {
        return;
    }

    auto trayFrame = rootContent
                         ? FindChildByClassName(rootContent,
                                                L"SystemTray.SystemTrayFrame")
                         : nullptr;
    if (trayFrame && FindChildByName(trayFrame, L"SystemTrayFrameGrid")) {
        RemoveTaskbarButton();
        InjectTaskbarButton();
        StartUiTimer();
        return;
    }

    if (attempt >= kMaxAttempts) {
        Wh_Log(L"InjectWithRetry: system tray never appeared");
        return;
    }

    try {
        DispatcherTimer timer;
        timer.Interval(
            winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(100)});
        auto token = std::make_shared<winrt::event_token>();
        *token = timer.Tick(
            [timer, token, rootContent, generation, attempt](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) mutable {
                try {
                    timer.Stop();
                    timer.Tick(*token);
                    UntrackTimer(timer);
                } catch (...) {
                }
                InjectWithRetry(rootContent, generation, attempt + 1);
            });
        TrackTimer(timer);
        timer.Start();
    } catch (...) {
        Wh_Log(L"InjectWithRetry: could not schedule a retry");
    }
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);

    if (g_unloading) {
        return;
    }

    try {
        HWND hWnd = FindCurrentProcessTaskbarWnd();
        if (!hWnd) {
            Wh_Log(L"TrayUI_StartTaskbar_Hook: taskbar window not found");
            return;
        }

        // The old tree is gone; drop every reference into it before rebuilding,
        // and retire any retry chain still running for the previous taskbar.
        g_injectGeneration++;
        StopAllTimers();
        g_taskbarButton = nullptr;
        g_taskbarLabel = nullptr;
        g_injectionParent = nullptr;
        g_injectedColumn = -1;
        g_flyout = nullptr;
        g_flyoutOpen = false;
        ClearFlyoutRefs();
        StopUiTimer();

        g_taskbarWnd.store(hWnd);

        // If the taskbar only appeared now, this is where the worker starts.
        StartNetThread();

        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            return;
        }
        auto rootContent = xamlRoot.Content().try_as<FrameworkElement>();
        if (!rootContent) {
            return;
        }
        InjectWithRetry(rootContent, g_injectGeneration);
    } catch (...) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: exception");
    }
}

static bool HookTaskbarSymbols() {
    HMODULE taskbarModule = LoadLibraryExW(L"taskbar.dll", nullptr,
                                           LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarModule) {
        Wh_Log(L"HookTaskbarSymbols: taskbar.dll not found");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
    };

    return WindhawkUtils::HookSymbols(taskbarModule, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

// ---------------------------------------------------------------------------
// Section 20: mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_unloading = false;
    g_taskbarWnd.store(nullptr);

    LoadSettings();

    if (!HookTaskbarSymbols()) {
        Wh_Log(L"Wh_ModInit: taskbar symbol hooks failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    g_taskbarWnd.store(FindCurrentProcessTaskbarWnd());

    if (g_taskbarWnd.load()) {
        // Only the Explorer instance that owns the taskbar needs the menu.
        StartNetThread();
    }

    if (g_taskbarWnd.load()) {
        RunFromWindowThread(
            g_taskbarWnd.load(),
            [](void*) {
                try {
                    RemoveTaskbarButton();
                    InjectTaskbarButton();
                    StartUiTimer();
                } catch (...) {
                    Wh_Log(L"Wh_ModAfterInit: exception during injection");
                }
            },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    const int oldHostel = g_settings.hostel;
    const int oldMess = g_settings.mess;

    LoadSettings();

    const bool sourceChanged =
        (oldHostel != g_settings.hostel) || (oldMess != g_settings.mess);

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        hWnd = g_taskbarWnd.load();
    }

    if (hWnd) {
        g_taskbarWnd.store(hWnd);
        RunFromWindowThread(
            hWnd,
            [](void*) {
                try {
                    if (g_flyout && g_flyoutOpen) {
                        g_flyout.Hide();
                    }
                } catch (...) {
                }
                g_flyout = nullptr;
                g_flyoutOpen = false;
                ClearFlyoutRefs();

                try {
                    RemoveTaskbarButton();
                    InjectTaskbarButton();
                } catch (...) {
                    Wh_Log(L"Wh_ModSettingsChanged: exception during re-inject");
                }
            },
            nullptr);
    }

    if (sourceChanged) {
        // A different hostel/mess is a different file entirely: drop the loaded
        // menu now so the flyout cannot show the old mess's food, then let the
        // worker reload the cache and fetch.
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_store.days.clear();
            g_store.hostel = g_settings.hostel;
            g_store.mess = g_settings.mess;
            g_lastFetchError.clear();
            g_storeVersion.fetch_add(1);
        }
        g_reloadCacheRequested.store(true);
        KickFetch();
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;

    StopNetThread();

    if (g_taskbarWnd.load()) {
        bool cleaned = RunFromWindowThread(
            g_taskbarWnd.load(),
            [](void*) {
                // Order matters. Everything that could still call into this
                // image has to be cancelled before the references are dropped,
                // because the image is unmapped the moment Wh_ModUninit
                // returns and a pending callback would then run in freed
                // memory -- a g_unloading check inside it cannot help, since
                // the crash is the call itself.
                try {
                    StopUiTimer();
                    StopAllTimers();
                } catch (...) {
                }

                // The reveal handler normally detaches itself when it fires;
                // if it has not fired yet, it is still attached here.
                try {
                    if (g_revealTarget && g_revealSizeToken.value) {
                        g_revealTarget.SizeChanged(g_revealSizeToken);
                    }
                } catch (...) {
                }
                g_revealSizeToken = {};
                g_revealTarget = nullptr;

                try {
                    if (g_closingStoryboard) {
                        g_closingStoryboard.Stop();
                    }
                } catch (...) {
                }
                g_closingStoryboard = nullptr;

                // MessBlurBrush is defined in this image and lives on the
                // flyout Border's Background. Hide() does not tear the popup's
                // visual tree down synchronously, so drop the brush now to run
                // its OnDisconnected while the code still exists.
                try {
                    if (g_flyoutRoot) {
                        g_flyoutRoot.Background(nullptr);
                    }
                } catch (...) {
                }

                try {
                    if (g_flyout && g_flyoutOpen) {
                        g_flyout.Hide();
                    }
                } catch (...) {
                }
                g_flyout = nullptr;
                g_flyoutOpen = false;
                ClearFlyoutRefs();

                try {
                    RemoveTaskbarButton();
                } catch (...) {
                    Wh_Log(L"Wh_ModUninit: exception removing the button");
                }
            },
            nullptr);

        if (!cleaned) {
            // The UI thread never ran our cleanup, so the button and its
            // handlers are still live in a taskbar whose mod is about to
            // vanish. Nothing safe can be done about it from this thread.
            Wh_Log(L"Wh_ModUninit: UI-thread cleanup did not run");
        }
    }

    // Anything still holding a XAML object here would outlive the mod's code.
    g_flyout = nullptr;
    g_flyoutRoot = nullptr;
    g_revealTarget = nullptr;
    g_closingStoryboard = nullptr;
    g_taskbarButton = nullptr;
    g_taskbarLabel = nullptr;
    g_injectionParent = nullptr;
    g_reservedElement = nullptr;
    g_timer = nullptr;
    g_subtleButtonStyle = nullptr;
    ClearFlyoutRefs();

    // These carry [[clang::no_destroy]], so release their buffers by hand.
    g_liveTimers.reset();
    g_cardCountdowns.reset();
    g_cardBorders.reset();
    g_cardMeals.reset();
    {
        std::lock_guard<std::mutex> lock(g_runPayloadsMutex);
        g_runPayloads.reset();
    }
}
