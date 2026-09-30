// ==WindhawkMod==
// @id              vit-mess-menu-taskbar
// @name            VIT Mess Menu Taskbar Flyout
// @description     Shows the VIT Vellore hostel mess menu on the Windows 11 taskbar, with a native flyout for the full day's menu.
// @version         1.0.1
// @author          ashishkupadhyay
// @github          https://github.com/ashishkupadhyay
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lruntimeobject -luser32 -lwindowsapp -lwinhttp
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VIT Mess Menu Taskbar Flyout

Puts the VIT Vellore hostel mess menu into the Windows 11 taskbar.

A small button sits next to the system tray showing what is being served right
now, or how long until the next meal starts. Click it and a native-looking
flyout slides up with the full day's menu, split into Breakfast, Lunch, Snacks
and Dinner. Chevrons at the top let you browse to other days.

## The flyout open above the taskbar
![The flyout open above the taskbar on the left side](https://raw.githubusercontent.com/ashishkupadhyay/VIT-Mess-Menu-Taskbar-Flyout/refs/heads/main/docs/screenshots/hero.png)
![The flyout open above the taskbar on the right side](https://raw.githubusercontent.com/ashishkupadhyay/VIT-Mess-Menu-Taskbar-Flyout/main/docs/screenshots/flyout-next.png)

## What it does

- **Taskbar button** — `Idli • Vada • Khichdi…` while a meal is being served,
  `Lunch starts in 1 hr 20 min` between meals. Or icon only, in compact mode.
  The icon itself fills in while a meal is being served, so the state is
  visible at a glance either way.
- **Flyout** — all four meals, always expanded, with the current meal
  highlighted green and the upcoming meal highlighted yellow.
- **Automatic grouping** — items are sorted into Main Items, Bread & Sides,
  Dairy, Beverages and Dessert, and shown inline to keep the flyout compact.
- **Offline first** — the menu is cached on disk, so the flyout opens instantly
  and works without a network connection.

## Setup

Pick your **Hostel** and **Mess** in the settings, and that is it. The mod
downloads the right file from `messit.vinnovateit.com` by itself and keeps it
up to date. There is nothing to import and no files to manage.

The mod is built around VIT Vellore's messes and timings. If the site ever
moves, or another mess publishes its menu as JSON in the same shape, the
**Custom menu URL** setting points the mod there instead.

## Meal timings

These are the defaults, and they are the VIT Vellore timings. All five windows
are editable in the settings, so a mess that shifts a slot does not need a new
version of the mod:

| Meal | Mon-Fri | Sat & Sun |
| --- | --- | --- |
| Breakfast | 07:00 - 09:00 | 07:30 - 09:30 |
| Lunch | 12:30 - 14:30 | 12:30 - 14:30 |
| Snacks | 16:30 - 18:00 | 16:30 - 18:00 |
| Dinner | 19:00 - 21:00 | 19:00 - 21:00 |

## Updating

Each JSON file on the site covers one month. The current month is re-downloaded
about once a day, because the site sometimes revises a file after publishing
it. If the month is missing altogether, the mod retries every few hours until
the site publishes it. You can force a check at any time with the reload
button at the bottom of the flyout. The previous month's menu is never shown as
if it were the current one.

Cached menus are kept in Windhawk's own per-mod storage folder, which Windhawk
deletes when the mod is removed — so the mod leaves nothing behind. Every
hostel and mess you have looked at stays cached, so switching between them is
instant and works offline.

## Notes

Requires Windows 11 (22H2 or newer) — it hooks the XAML taskbar, which does not
exist on Windows 10.

On a multi-monitor setup, "Show on" chooses between the primary taskbar only
(the default) and every taskbar. Opening the flyout from a secondary taskbar
anchors it to that monitor.

The menu data comes from `messit.vinnovateit.com`, a third-party site this mod
does not control. If that site changes its data format or goes away, the mod
will report that no menu is available.

## Credits and licence

This mod is published under the GNU General Public License v3.0, because its
backdrop blur is derived from GPL-3.0 code.

- **Windows 11 Taskbar Styler** (GPL-3.0), whose `XamlBlurBrush` is itself
  derived from **TranslucentTB** (GPL-3.0) — the Composition backdrop-blur
  brush: the `IGraphicsEffectD2D1Interop` declaration, the effect object that
  describes a D2D Gaussian blur, and `CreateBackdropBrush` →
  `CreateEffectFactory` → `SetSourceParameter` behind a
  `XamlCompositionBrushBase`. This is the part that sets the licence above.
- **Taskbar AI Quota** and **Taskbar Fluent Media Player** (both MIT) — reaching
  the taskbar's XAML root through `CTaskBand::GetTaskbarHost` and the
  `TaskbarHost::FrameHeight` prologue, the system-tray column insert/remove, and
  the `RunFromWindowThread` helper.
- **Fluent UI System Icons** by Microsoft (MIT) — the "Food" glyph on the
  taskbar button, in its regular and filled variants.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- source:
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
  - url: ""
    $name: Custom menu URL
    $description: "Leave empty to use messit.vinnovateit.com. Otherwise the full URL of a JSON file in the same format. {hostel} and {mess} in the URL are replaced with the numbers chosen above, e.g. https://example.com/menu/hostel-{hostel}-mess-{mess}.json. Prefer https; a plain http URL is accepted for a server on your own network but is fetched unencrypted. Changing this clears the cached menus."
  $name: Menu source
  $description: Which mess's menu to show. This is the only thing that needs setting up.
- button:
  - mode: expanded
    $name: Content
    $description: Expanded shows the current meal or the next-meal countdown next to the icon. Compact shows only the icon, with the same text in the tooltip.
    $options:
    - expanded: Expanded
    - compact: Compact
  - filledIconWhenServing: true
    $name: Filled icon while a meal is being served
    $description: Switches the outline glyph to its filled variant for the duration of each serving window, so the state is visible from the icon alone. Useful in Compact mode.
  - position: tray_left
    $name: Position
    $description: The first sits in the taskbar's own area; the rest sit inside the system tray, next to the other tray icons.
    $options:
    - taskbar_left: Left edge of the taskbar
    - tray_left: Left of the system tray
    - input_left: Left of the input indicator (language switcher)
    - network_left: Left of the network, volume and battery icons
    - clock_left: Left of the clock
    - clock_right: Right of the clock
  - scope: primary
    $name: Show on
    $description: Which taskbars get the button on a multi-monitor setup.
    $options:
    - primary: The primary taskbar only
    - all: Every taskbar
  - maxLabelWidth: 180
    $name: Maximum label width
    $description: Longer text is truncated with an ellipsis. Pixels.
  - paddingLeft: 4
    $name: Spacing (left)
    $description: Gap in pixels to the left of the button, which also shifts the button to the right. Increase this to move clear of another mod occupying the same spot.
  - paddingRight: 4
    $name: Spacing (right)
    $description: Gap in pixels to the right of the button.
  - reserveSpace: true
    $name: 'Push the taskbar icons aside (only with "Left edge of the taskbar")'
    $description: Reserves the button's width plus its spacing before the taskbar icons, so they move out of the way instead of sitting underneath. Has no effect in the system-tray positions, where the tray lays the button out for us. Turn this off if another mod already manages that space.
  $name: Taskbar button
- flyout:
  - width: 380
    $name: Width
    $description: Pixels.
  - cornerRadius: 8
    $name: Corner radius
    $description: Pixels. The meal cards follow automatically, staying concentric with the flyout's own corners.
  - showSnacks: true
    $name: Show the Snacks card
    $description: Hides only the card. The countdown still knows when snacks are served.
  - backgroundMode: auto
    $name: Background
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
  $name: Flyout
- timings:
  - breakfast: "07:00-09:00"
    $name: Breakfast (Mon-Fri)
  - breakfastWeekend: "07:30-09:30"
    $name: Breakfast (Sat & Sun)
  - lunch: "12:30-14:30"
    $name: Lunch
  - snacks: "16:30-18:00"
    $name: Snacks
  - dinner: "19:00-21:00"
    $name: Dinner
  $name: Meal timings
  $description: "Serving windows as HH:MM-HH:MM, on a 24-hour clock. These drive the countdown and which card is highlighted, so correct them here if your mess changes a slot. The defaults are the VIT Vellore timings."
- grouping:
  - extraDessertItems: ""
    $name: Extra dessert items
    $description: "Comma-separated. The site sometimes lists desserts without a \"Sweet:\" or \"Fruits:\" label; the common ones are recognised already, and anything it starts listing that is not can be added here, e.g. Rasgulla, Mango. An entry matches a whole item or its last word, ignoring case."
  $name: Menu grouping
  $description: How items are sorted into Main Items, Bread & Sides, Dairy, Beverages and Dessert in the flyout.
- updates:
  - automatic: true
    $name: Check for new menus automatically
    $description: Re-downloads the current month about once a day, since the site sometimes revises a menu after publishing it, and keeps checking for a missing month every few hours. When off, the menu is only downloaded when you press the reload button in the flyout.
  $name: Updates
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
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
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
#include <cstdint>
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

// Stored as an enum rather than a std::wstring: g_settings is written by
// LoadSettings on the engine thread and read from the taskbar UI thread and the
// network worker. The scalar members are benign to race on, but a std::wstring
// can be read mid-reassignment.
enum class ButtonPosition {
    TaskbarLeft,
    TrayLeft,
    InputLeft,
    NetworkLeft,
    ClockLeft,
    ClockRight,
};

enum class TaskbarScope {
    Primary,
    All,
};

// A serving window, in seconds since local midnight.
struct MealWindow {
    int startSec;
    int endSec;
};

static constexpr int Hm(int hour, int minute) {
    return hour * 3600 + minute * 60;
}

struct ModSettings {
    int  hostel          = 1;       // 1 = men's, 2 = women's
    int  mess            = 2;       // 1 = special, 2 = veg, 3 = non-veg
    bool compact         = false;
    bool filledIconWhenServing = true;
    ButtonPosition position = ButtonPosition::TrayLeft;
    TaskbarScope taskbarScope = TaskbarScope::Primary;

    // Indexed by Meal. Defaults are the VIT Vellore windows; Breakfast has its
    // own weekend variant because that is the only one that shifts.
    MealWindow mealWeekday[4] = {
        {Hm(7, 0), Hm(9, 0)},      // Breakfast
        {Hm(12, 30), Hm(14, 30)},  // Lunch
        {Hm(16, 30), Hm(18, 0)},   // Snacks
        {Hm(19, 0), Hm(21, 0)},    // Dinner
    };
    MealWindow breakfastWeekend = {Hm(7, 30), Hm(9, 30)};

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

// hostel * 10 + mess, published as one word. The worker snapshots the pair
// while LoadSettings may be rewriting g_settings on another thread; read
// separately, a fetch could pair the new hostel with the old mess and cache
// that response under a file name it does not belong to.
static std::atomic<int> g_sourceKey{12};

static void CurrentSource(int& hostel, int& mess) {
    const int key = g_sourceKey.load();
    hostel = key / 10;
    mess = key % 10;
}

// The user's extra dessert keywords, already normalised (see NormalizeKey),
// each with its spaceless form precomputed so ClassifyItem does not redo it
// per item. Kept out of ModSettings for the reason given above: this is a
// list of strings, rewritten by LoadSettings while the taskbar thread may be
// in the middle of classifying a menu. Readers take a snapshot of the
// shared_ptr under the lock and never touch the vector itself while unlocked.
struct DessertKeyword {
    std::wstring normalized;  // "water melon"
    std::wstring compact;     // "watermelon"
};
static std::mutex g_userDessertKeywordsMutex;
static std::shared_ptr<const std::vector<DessertKeyword>> g_userDessertKeywords;

// The custom menu URL template, or empty for the built-in host. Same
// arrangement: written by LoadSettings, snapshotted by the network worker.
static std::mutex g_menuUrlMutex;
static std::shared_ptr<const std::wstring> g_menuUrlTemplate;

static std::wstring GetMenuUrlTemplate() {
    std::lock_guard<std::mutex> lock(g_menuUrlMutex);
    return g_menuUrlTemplate ? *g_menuUrlTemplate : std::wstring();
}

static std::wstring NormalizeKey(const std::wstring& text);
static std::wstring WithoutSpaces(const std::wstring& text);

// Wh_GetStringSetting never returns null -- it yields L"" when unset or on
// error -- so an empty test is all that is needed. StringSetting is RAII, so
// Wh_FreeStringSetting cannot be missed.
static std::wstring GetStringSetting(PCWSTR key, PCWSTR fallback) {
    WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(key);
    return value.get()[0] ? std::wstring(value.get()) : std::wstring(fallback);
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

// One "HH:MM" clock reading out of text[begin, end), tolerating spaces.
static bool ParseClock(const std::wstring& text, size_t begin, size_t end,
                       int& seconds) {
    while (begin < end && text[begin] == L' ') {
        begin++;
    }
    while (end > begin && text[end - 1] == L' ') {
        end--;
    }

    size_t colon = text.find(L':', begin);
    if (colon == std::wstring::npos || colon >= end || colon == begin ||
        colon + 1 == end) {
        return false;
    }

    int hours = 0;
    for (size_t i = begin; i < colon; i++) {
        if (!iswdigit(text[i])) {
            return false;
        }
        hours = hours * 10 + (text[i] - L'0');
        if (hours > 23) {
            return false;
        }
    }

    int minutes = 0;
    for (size_t i = colon + 1; i < end; i++) {
        if (!iswdigit(text[i])) {
            return false;
        }
        minutes = minutes * 10 + (text[i] - L'0');
        if (minutes > 59) {
            return false;
        }
    }

    seconds = hours * 3600 + minutes * 60;
    return true;
}

// "HH:MM-HH:MM". Rejects anything malformed, and anything that does not run
// forwards within one day -- the state machine assumes windows do not wrap past
// midnight, so a reversed range would silently never match.
static bool ParseTimeRange(const std::wstring& text, MealWindow& out) {
    size_t dash = text.find(L'-');
    if (dash == std::wstring::npos) {
        return false;
    }

    MealWindow parsed{};
    if (!ParseClock(text, 0, dash, parsed.startSec) ||
        !ParseClock(text, dash + 1, text.size(), parsed.endSec)) {
        return false;
    }
    if (parsed.endSec <= parsed.startSec) {
        return false;
    }

    out = parsed;
    return true;
}

// "Rasgulla, Mango, " -> {"rasgulla", "mango"}. Empty entries are dropped so a
// trailing comma cannot turn every item into a dessert.
static void LoadUserDessertKeywords() {
    std::wstring text = GetStringSetting(L"grouping.extraDessertItems", L"");
    auto keywords = std::make_shared<std::vector<DessertKeyword>>();

    size_t start = 0;
    while (start <= text.size()) {
        size_t comma = text.find(L',', start);
        std::wstring piece = NormalizeKey(text.substr(
            start, comma == std::wstring::npos ? std::wstring::npos
                                               : comma - start));
        if (!piece.empty()) {
            std::wstring compact = WithoutSpaces(piece);
            keywords->push_back({std::move(piece), std::move(compact)});
        }
        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }

    std::lock_guard<std::mutex> lock(g_userDessertKeywordsMutex);
    g_userDessertKeywords = std::move(keywords);
}

static void LoadMealWindow(PCWSTR key, PCWSTR fallback, MealWindow& target) {
    std::wstring text = GetStringSetting(key, fallback);
    if (ParseTimeRange(text, target)) {
        return;
    }
    Wh_Log(L"LoadSettings: could not parse %s \"%s\", using the default", key,
           text.c_str());
    ParseTimeRange(fallback, target);
}

// Settings are grouped into sections in the settings block above, and Windhawk
// addresses a grouped key as "section.key".
static void LoadSettings() {
    g_settings.hostel =
        (GetStringSetting(L"source.hostel", L"mens") == L"womens") ? 2 : 1;

    std::wstring mess = GetStringSetting(L"source.mess", L"veg");
    g_settings.mess = (mess == L"special") ? 1 : (mess == L"nonveg") ? 3 : 2;
    g_sourceKey.store(g_settings.hostel * 10 + g_settings.mess);

    {
        // Trimmed, so a stray space cannot turn a valid URL into a bad one.
        std::wstring url = GetStringSetting(L"source.url", L"");
        size_t first = url.find_first_not_of(L" \t\r\n");
        size_t last = url.find_last_not_of(L" \t\r\n");
        url = (first == std::wstring::npos)
                  ? std::wstring()
                  : url.substr(first, last - first + 1);
        auto shared = std::make_shared<const std::wstring>(std::move(url));
        std::lock_guard<std::mutex> lock(g_menuUrlMutex);
        g_menuUrlTemplate = std::move(shared);
    }

    g_settings.compact =
        (GetStringSetting(L"button.mode", L"expanded") == L"compact");
    g_settings.filledIconWhenServing =
        Wh_GetIntSetting(L"button.filledIconWhenServing") != 0;
    std::wstring position = GetStringSetting(L"button.position", L"tray_left");
    g_settings.position =
        (position == L"taskbar_left")   ? ButtonPosition::TaskbarLeft
        : (position == L"input_left")   ? ButtonPosition::InputLeft
        : (position == L"network_left") ? ButtonPosition::NetworkLeft
        : (position == L"clock_left")   ? ButtonPosition::ClockLeft
        : (position == L"clock_right")  ? ButtonPosition::ClockRight
                                        : ButtonPosition::TrayLeft;
    g_settings.taskbarScope =
        (GetStringSetting(L"button.scope", L"primary") == L"all")
            ? TaskbarScope::All
            : TaskbarScope::Primary;
    g_settings.maxLabelWidth =
        std::clamp(Wh_GetIntSetting(L"button.maxLabelWidth"), 40, 600);
    // Wide enough to slide the button across any monitor; the bound is only
    // here to stop a typo pushing it off-screen with no way back.
    g_settings.buttonPaddingLeft =
        std::clamp(Wh_GetIntSetting(L"button.paddingLeft"), 0, 4000);
    g_settings.buttonPaddingRight =
        std::clamp(Wh_GetIntSetting(L"button.paddingRight"), 0, 4000);
    g_settings.reserveTaskbarSpace =
        Wh_GetIntSetting(L"button.reserveSpace") != 0;

    g_settings.popupWidth =
        std::clamp(Wh_GetIntSetting(L"flyout.width"), 260, 900);
    g_settings.popupCornerRadius =
        std::clamp(Wh_GetIntSetting(L"flyout.cornerRadius"), 0, 32);
    g_settings.showSnacks = Wh_GetIntSetting(L"flyout.showSnacks") != 0;
    g_settings.customBackground =
        (GetStringSetting(L"flyout.backgroundMode", L"auto") == L"custom");
    std::wstring hexColor =
        GetStringSetting(L"flyout.backgroundColor", L"#80000000");
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
    g_settings.blurAmount =
        std::clamp(Wh_GetIntSetting(L"flyout.blurAmount"), 0, 100);

    LoadMealWindow(L"timings.breakfast", L"07:00-09:00",
                   g_settings.mealWeekday[0]);
    LoadMealWindow(L"timings.breakfastWeekend", L"07:30-09:30",
                   g_settings.breakfastWeekend);
    LoadMealWindow(L"timings.lunch", L"12:30-14:30", g_settings.mealWeekday[1]);
    LoadMealWindow(L"timings.snacks", L"16:30-18:00",
                   g_settings.mealWeekday[2]);
    LoadMealWindow(L"timings.dinner", L"19:00-21:00",
                   g_settings.mealWeekday[3]);

    LoadUserDessertKeywords();

    g_settings.autoUpdate = Wh_GetIntSetting(L"updates.automatic") != 0;
}

// ---------------------------------------------------------------------------
// Section 2: domain model
// ---------------------------------------------------------------------------

enum class Meal { Breakfast = 0, Lunch, Snacks, Dinner, Count };

static constexpr int kMealCount = (int)Meal::Count;

static const wchar_t* const kMealNames[kMealCount] = {L"Breakfast", L"Lunch",
                                                      L"Snacks", L"Dinner"};

// The taskbar icon: Fluent UI System Icons "Food" (ic_fluent_food_48_regular,
// MIT), as XAML path data so it renders through a PathIcon like the tray's
// own glyphs -- monochrome, theme-aware, no image decoding. "F1" selects the
// nonzero fill rule the SVG uses; XAML's default is even-odd.
// https://github.com/microsoft/fluentui-system-icons
static constexpr int kTaskbarIconCanvas = 48;
static const wchar_t* const kTaskbarIconData =
    L"F1 M7.97791 6.72626C8.23786 5.13494 9.61649 4 11.2028 4C12.0065 4 "
    L"12.7431 4.28759 13.3152 4.76548C13.914 4.28645 14.6735 4 15.5 4C16.3265 "
    L"4 17.086 4.28645 17.6848 4.76548C18.2569 4.28759 18.9935 4 19.7972 "
    L"4C21.3835 4 22.7621 5.13495 23.0221 6.72627C23.3899 8.97815 24 13.1284 "
    L"24 16C24 18.8478 22.5983 21.3683 20.4526 22.9087C19.8122 23.3685 19.5 "
    L"23.9239 19.5 24.3989C19.5 24.437 19.5011 24.4631 19.5035 24.4932C19.5912 "
    L"25.5888 20.5 36.9682 20.5 39C20.5 41.7614 18.2614 44 15.5 44C12.7386 44 "
    L"10.5 41.7614 10.5 39C10.5 36.9682 11.4088 25.5888 11.4965 24.4932C11.4989 "
    L"24.4631 11.5 24.437 11.5 24.3989C11.5 23.9239 11.1878 23.3685 10.5474 "
    L"22.9087C8.40173 21.3683 7 18.8478 7 16C7 13.1284 7.61005 8.97815 7.97791 "
    L"6.72626ZM19 16.75C19 17.4404 18.4404 18 17.75 18C17.0596 18 16.5 17.4404 "
    L"16.5 16.75V7.5C16.5 6.94772 16.0523 6.5 15.5 6.5C14.9477 6.5 14.5 "
    L"6.94772 14.5 7.5V16.75C14.5 17.4404 13.9404 18 13.25 18C12.5596 18 12 "
    L"17.4404 12 16.75V7.29725C12 6.85694 11.6431 6.5 11.2028 6.5C10.8154 6.5 "
    L"10.5034 6.77283 10.4452 7.12931C10.0747 9.3972 9.5 13.3587 9.5 16C9.5 "
    L"18.0086 10.4857 19.7869 12.0054 20.8779C13.0246 21.6096 14 22.8308 14 "
    L"24.3989C14 24.4955 13.9969 24.5886 13.9885 24.6928C13.8934 25.8804 13 "
    L"37.0998 13 39C13 40.3807 14.1193 41.5 15.5 41.5C16.8807 41.5 18 40.3807 "
    L"18 39C18 37.0998 17.1066 25.8804 17.0115 24.6928C17.0031 24.5886 17 "
    L"24.4955 17 24.3989C17 22.8308 17.9754 21.6096 18.9946 20.8779C20.5143 "
    L"19.7869 21.5 18.0086 21.5 16C21.5 13.3587 20.9253 9.3972 20.5548 "
    L"7.12931C20.4966 6.77283 20.1846 6.5 19.7972 6.5C19.3569 6.5 19 6.85694 "
    L"19 7.29725V16.75ZM36.5 6.53169V22.75C36.5 23.3401 36.6885 26.0805 "
    L"36.8952 29.0854L36.9093 29.2894C37.1889 33.3551 37.5 37.8909 37.5 "
    L"39C37.5 40.3807 36.3807 41.5 35 41.5C33.6193 41.5 32.5 40.3807 32.5 "
    L"39C32.5 38.0176 32.7453 34.014 32.9973 30.1803C33.1222 28.2802 33.2471 "
    L"26.4445 33.3408 25.0837C33.3876 24.4033 33.4267 23.8417 33.454 "
    L"23.4503L33.4969 22.8379C33.5213 22.492 33.4009 22.1515 33.1644 "
    L"21.8978C32.928 21.6441 32.5968 21.5 32.25 21.5H29.25C28.8358 21.5 28.5 "
    L"21.1642 28.5 20.75V15.25C28.5 10.6702 32.0186 6.91212 36.5 "
    L"6.53169ZM30.9097 24L30.8467 24.912C30.7529 26.2744 30.6278 28.1128 "
    L"30.5027 30.0164C30.2547 33.7899 30 37.9113 30 39C30 41.7614 32.2386 44 "
    L"35 44C37.7614 44 40 41.7614 40 39C40 37.7912 39.6846 33.2057 39.4123 "
    L"29.2478L39.4034 29.1178C39.1852 25.9458 39 23.2397 39 22.75V5.25C39 "
    L"4.55964 38.4404 4 37.75 4H37.25C31.0368 4 26 9.0368 26 15.25V20.75C26 "
    L"22.5449 27.4551 24 29.25 24H30.9097Z";

// The filled variant (ic_fluent_food_48_filled), shown while a meal is being
// served so the state is readable from the icon alone -- which is all there
// is to read in compact mode.
static const wchar_t* const kTaskbarIconFilledData =
    L"F1 M10.6139 4C9.96568 4 9.35301 4.46302 9.24717 5.17623C9.10659 6.1236 "
    L"8 13.6664 8 17C8 19.1964 8.94573 21.1737 10.4483 22.5436C11.1431 23.177 "
    L"11.5 23.8171 11.5 24.3858C11.5 24.4314 11.4986 24.4678 11.4954 "
    L"24.5078C11.4014 25.6815 10.5 36.9763 10.5 39C10.5 41.7614 12.7386 44 "
    L"15.5 44C18.2614 44 20.5 41.7614 20.5 39C20.5 36.9763 19.5986 25.6815 "
    L"19.5046 24.5078C19.5014 24.4678 19.5 24.4314 19.5 24.3858C19.5 23.8171 "
    L"19.8569 23.177 20.5517 22.5436C22.0543 21.1737 23 19.1964 23 17C23 "
    L"13.6657 21.8929 6.12023 21.7527 5.17555C21.6471 4.46372 21.0356 4 "
    L"20.3869 4H20.3688C19.5732 4 19.0017 4.65496 19.0017 5.37V15.88C19.0017 "
    L"16.4986 18.5003 17 17.8817 17C17.2632 17 16.7617 16.4986 16.7617 "
    L"15.88V5.37C16.7617 5.35279 16.7621 5.33575 16.7628 5.31888C16.7943 "
    L"4.60485 16.2251 4 15.5009 4C14.7766 4 14.2074 4.60485 14.2389 "
    L"5.31888C14.2396 5.33575 14.24 5.35279 14.24 5.37V15.88C14.24 16.4986 "
    L"13.7386 17 13.12 17C12.5014 17 12 16.4986 12 15.88V5.37C12 4.65496 "
    L"11.4285 4 10.6329 4H10.6139ZM30.9097 24L30.8467 24.912C30.7529 26.2744 "
    L"30.6278 28.1128 30.5027 30.0164C30.2547 33.7899 30 37.9113 30 39C30 "
    L"41.7614 32.2386 44 35 44C37.7614 44 40 41.7614 40 39C40 37.7912 39.6846 "
    L"33.2057 39.4123 29.2478L39.4034 29.1178C39.1852 25.9458 39 23.2397 39 "
    L"22.75V5.25C39 4.55964 38.4404 4 37.75 4H37.25C31.0368 4 26 9.0368 26 "
    L"15.25V20.75C26 22.5449 27.4551 24 29.25 24H30.9097Z";

enum class Group { Main = 0, BreadSides, Dairy, Beverages, Dessert, Count };

static constexpr int kGroupCount = (int)Group::Count;

static const wchar_t* const kGroupNames[kGroupCount] = {
    L"Main Items", L"Bread & Sides", L"Dairy", L"Beverages", L"Dessert"};

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
// The windows come from settings (VIT Vellore's by default), so nothing here
// may assume a fixed order or fixed times.
// ---------------------------------------------------------------------------

static_assert(kMealCount == 4,
              "ModSettings::mealWeekday is sized for four meals");

static MealWindow GetMealWindow(Meal meal, bool weekend) {
    if (meal == Meal::Breakfast && weekend) {
        return g_settings.breakfastWeekend;
    }
    return g_settings.mealWeekday[(int)meal];
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

    // The windows are all editable, so nothing guarantees they sit in array
    // order: take the earliest start still ahead, not the first one found.
    for (int i = 0; i < kMealCount; i++) {
        MealWindow window = GetMealWindow((Meal)i, weekendToday);
        if (window.startSec > nowSec &&
            (state.nextMeal < 0 ||
             window.startSec - nowSec < state.remainingSec)) {
            state.nextMeal = i;
            state.remainingSec = window.startSec - nowSec;
        }
    }
    if (state.nextMeal >= 0) {
        return state;
    }

    // Past the last meal of the day: count down to tomorrow's first one, using
    // tomorrow's weekday so the weekend breakfast window is honoured.
    const bool weekendTomorrow = IsWeekend(todayKey + 1);
    for (int i = 0; i < kMealCount; i++) {
        MealWindow window = GetMealWindow((Meal)i, weekendTomorrow);
        const int untilStart = (24 * 3600 - nowSec) + window.startSec;
        if (state.nextMeal < 0 || untilStart < state.remainingSec) {
            state.nextMeal = i;
            state.remainingSec = untilStart;
        }
    }
    state.nextIsTomorrow = true;
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

// The site cannot decide whether a compound is one word or two -- "Water
// Melon" and "Watermelon", "Butter milk" and "Buttermilk" have all appeared --
// so whole-item comparisons drop the spaces on both sides: the whole-item lists
// in ClassifyItem are written spaceless and compared against this.
static std::wstring WithoutSpaces(const std::wstring& text) {
    std::wstring result;
    result.reserve(text.size());
    for (wchar_t c : text) {
        if (c != L' ') {
            result.push_back(c);
        }
    }
    return result;
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
// "Cold Badam Milk", "Iced Lemon Tea" and "Nimbu Sharbat" land in Beverages.
static Group ClassifyItem(const std::wstring& item) {
    const std::wstring key = NormalizeKey(item);
    if (key.empty()) {
        return Group::Main;
    }
    // For the whole-item lists below, which are written spaceless.
    const std::wstring compactKey = WithoutSpaces(key);
    const std::wstring lastWord = LastWord(key);

    if (HasDessertLabel(key)) {
        return Group::Dessert;
    }

    // Some desserts arrive with no label at all -- "Assorted Ice Cream" is the
    // recurring one. Nothing savoury on this menu contains "ice cream", so a
    // plain substring test is safe here.
    if (key.find(L"ice cream") != std::wstring::npos) {
        return Group::Dessert;
    }

    // In September 2026 the site dropped the labels altogether and started
    // listing desserts bare -- "Gulab Jamun", "Jalebi", "Seasonal Fruit",
    // "Papaya". The label rule above stays in case they come back; this
    // catches the bare form with the same discipline as the other groups:
    // whole item or last word, never substring. So "Raw Banana Fry" and "Raw
    // Banana Bajji" stay main dishes while "Banana" does not, and "Sweet corn
    // chaat" is not a dessert.
    //
    // No cake here on purpose. Cake only ever shows up as the snack itself
    // ("Brownie Cake, Tea, Coffee, Milk"), and filing it under Dessert would
    // leave Main Items empty -- the taskbar button would then read "Tea •
    // Coffee • Milk" during snacks.
    static const wchar_t* const kFruits[] = {
        L"banana",   L"papaya",  L"watermelon",  L"muskmelon", L"grapes",
        L"apple",    L"orange",  L"pineapple",   L"guava",     L"mango",
        L"pomegranate", L"sapota", L"chikoo",    L"fruitsalad"};
    if (InList(compactKey, kFruits, ARRAYSIZE(kFruits))) {
        return Group::Dessert;
    }

    // "Seasonal Fruit", "Cut Fruits", "Bread Halwa", "Gulab Jamun", "Boondi
    // Laddu", "Dal Payasam", "Mysore Pak", "Water Melon".
    static const wchar_t* const kDessertTails[] = {
        L"fruit",   L"fruits",  L"halwa",   L"laddu",    L"ladoo",
        L"laddoo",  L"jamun",   L"jalebi",  L"kheer",    L"payasam",
        L"kesari",  L"burfi",   L"barfi",   L"rasgulla", L"rasmalai",
        L"badusha", L"jangri",  L"peda",    L"phirni",   L"kulfi",
        L"custard", L"pudding", L"pak",     L"rabri",    L"poli",
        L"melon"};
    if (InList(lastWord, kDessertTails, ARRAYSIZE(kDessertTails))) {
        return Group::Dessert;
    }

    // Whatever the user added in settings, for the next time the site changes
    // its mind. Snapshot the list under the lock; LoadSettings may be swapping
    // it on another thread.
    {
        std::shared_ptr<const std::vector<DessertKeyword>> userKeywords;
        {
            std::lock_guard<std::mutex> lock(g_userDessertKeywordsMutex);
            userKeywords = g_userDessertKeywords;
        }
        if (userKeywords) {
            for (const DessertKeyword& keyword : *userKeywords) {
                if (compactKey == keyword.compact ||
                    lastWord == keyword.normalized) {
                    return Group::Dessert;
                }
            }
        }
    }

    // Plain "Milk" is dairy; flavoured milks ("Rose Milk", "Cold Badam Milk")
    // fall through to Beverages on their last word.
    static const wchar_t* const kDairy[] = {L"curd", L"loosecurd",
                                            L"thickcurd", L"cupcurd",
                                            L"buttermilk", L"milk"};
    if (InList(compactKey, kDairy, ARRAYSIZE(kDairy))) {
        return Group::Dairy;
    }

    static const wchar_t* const kBeverageTails[] = {
        L"tea", L"coffee", L"milk", L"sharbat", L"juice", L"lassi", L"shake"};
    if (InList(lastWord, kBeverageTails, ARRAYSIZE(kBeverageTails))) {
        return Group::Beverages;
    }

    // "Bread, Butter, Jam" -- the breakfast sides, whether listed separately or
    // as one "Bread Butter Jam" / "BBJ" item.
    static const wchar_t* const kBreadSides[] = {L"bread", L"butter", L"jam",
                                                 L"breadbutterjam", L"bbj"};
    if (InList(compactKey, kBreadSides, ARRAYSIZE(kBreadSides))) {
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

    // DaysFromCivil happily normalises "2026-02-31" to 3 March, which would
    // then silently overwrite that day's menu. Round-trip and require the
    // same date back, so a malformed entry is skipped instead.
    int checkYear;
    unsigned checkMonth, checkDay;
    CivilFromDays(dayKey, checkYear, checkMonth, checkDay);
    return checkYear == year && checkMonth == (unsigned)month &&
           checkDay == (unsigned)day;
}

static bool ParseMenuJson(const std::wstring& json, ParsedMonth& out) {
    using namespace winrt::Windows::Data::Json;

    try {
        JsonObject root{nullptr};
        if (!JsonObject::TryParse(json, root) || !root) {
            return false;
        }

        // Null when "menu" is missing or not an array.
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
        }

        // Which month this file gets cached under. Taken from the most common
        // date in it rather than the first one: a file that opened with a
        // trailing day or two of the previous month would otherwise be filed
        // under the wrong name and become eligible for pruning a month early.
        std::map<int, int> monthCounts;
        for (auto& entry : out.days) {
            monthCounts[MonthKeyFromDayKey(entry.first)]++;
        }
        int bestCount = 0;
        for (auto& entry : monthCounts) {
            if (entry.second > bestCount) {
                bestCount = entry.second;
                out.monthKey = entry.first;
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

static void ReplaceAll(std::wstring& text, const wchar_t* token,
                       const std::wstring& value) {
    const size_t length = wcslen(token);
    for (size_t pos = text.find(token); pos != std::wstring::npos;
         pos = text.find(token, pos + value.size())) {
        text.replace(pos, length, value);
    }
}

// The URL to download for this hostel/mess: the custom template with its
// placeholders filled in, or the built-in messit.vinnovateit.com path.
static std::wstring ResolveMenuUrl(int hostel, int mess) {
    std::wstring url = GetMenuUrlTemplate();
    if (url.empty()) {
        url = std::wstring(L"https://") + kMenuHost + L"/menu-data/hostel-" +
              std::to_wstring(hostel) + L"-mess-" + std::to_wstring(mess) +
              L".json";
        return url;
    }
    ReplaceAll(url, L"{hostel}", std::to_wstring(hostel));
    ReplaceAll(url, L"{mess}", std::to_wstring(mess));
    return url;
}

static bool HttpGetJson(const std::wstring& url, std::string& out,
                        std::wstring& error) {
    out.clear();
    error.clear();

    // Split the URL up front so a typo in the custom setting fails with a
    // clear message rather than a connection error. Only http and https are
    // meaningful here.
    URL_COMPONENTS parts{};
    parts.dwStructSize = sizeof(parts);
    parts.dwSchemeLength = (DWORD)-1;
    parts.dwHostNameLength = (DWORD)-1;
    parts.dwUrlPathLength = (DWORD)-1;
    parts.dwExtraInfoLength = (DWORD)-1;
    if (!WinHttpCrackUrl(url.c_str(), (DWORD)url.size(), 0, &parts) ||
        !parts.lpszHostName || parts.dwHostNameLength == 0 ||
        (parts.nScheme != INTERNET_SCHEME_HTTPS &&
         parts.nScheme != INTERNET_SCHEME_HTTP)) {
        error = L"The menu URL is not valid";
        return false;
    }
    const bool secure = parts.nScheme == INTERNET_SCHEME_HTTPS;
    const std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    std::wstring path = parts.lpszUrlPath
                            ? std::wstring(parts.lpszUrlPath,
                                           parts.dwUrlPathLength)
                            : std::wstring();
    if (parts.lpszExtraInfo && parts.dwExtraInfoLength) {
        path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
    }
    if (path.empty()) {
        path = L"/";
    }

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

    HINTERNET connection =
        WinHttpConnect(session, host.c_str(), parts.nPort, 0);
    if (!connection) {
        error = L"Could not reach the server";
        WinHttpCloseHandle(session);
        return false;
    }

    HINTERNET request = WinHttpOpenRequest(
        connection, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, secure ? WINHTTP_FLAG_SECURE : 0);
    if (!request) {
        error = L"Could not create the request";
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        return false;
    }

    g_activeRequest.store(request);

    // The handle only becomes cancellable once it is published above, so an
    // unload that lands in the gap would otherwise have to wait out the WinHTTP
    // timeouts. Re-check now that it is visible.
    bool success = false;
    if (g_unloading) {
        error = L"Cancelled";
    } else if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        error = L"The request could not be sent";
    } else if (!WinHttpReceiveResponse(request, nullptr)) {
        error = L"No response from the server";
    } else {
        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        if (!WinHttpQueryHeaders(request,
                                 WINHTTP_QUERY_STATUS_CODE |
                                     WINHTTP_QUERY_FLAG_NUMBER,
                                 WINHTTP_HEADER_NAME_BY_INDEX, &status,
                                 &statusSize, WINHTTP_NO_HEADER_INDEX)) {
            error = L"The server's response could not be read";
        } else if (status != 200) {
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

// Our own "h<n>m<n>-YYYY-MM.json" names, and nothing else in the directory.
// Fills in the month the name carries.
static bool ParseCacheFileName(const std::wstring& name, int& monthKey) {
    // h1m2-2026-09.json is 17 characters.
    if (name.size() != 17 || name[0] != L'h' || name[2] != L'm' ||
        name[4] != L'-' || name[9] != L'-' || !iswdigit(name[1]) ||
        !iswdigit(name[3])) {
        return false;
    }
    int year = _wtoi(name.substr(5, 4).c_str());
    int month = _wtoi(name.substr(10, 2).c_str());
    if (year < 1970 || month < 1 || month > 12) {
        return false;
    }
    monthKey = year * 12 + month - 1;
    return true;
}

// Deletes every cached month older than `keepFromMonthKey`, for every
// hostel/mess -- not just the one selected. The other sources' files are kept
// on purpose, so switching between messes is instant and works offline, and
// this is the only place their old months would ever age out.
static void PruneOldCacheFiles(int keepFromMonthKey) {
    std::wstring directory = GetCacheDirectory();
    if (directory.empty()) {
        return;
    }

    std::wstring pattern = directory + L"\\h*m*-*.json";
    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        std::wstring name = findData.cFileName;
        int monthKey = 0;
        if (!ParseCacheFileName(name, monthKey)) {
            continue;
        }
        if (monthKey < keepFromMonthKey) {
            std::wstring full = directory + L"\\" + name;
            DeleteFileW(full.c_str());
            Wh_Log(L"PruneOldCacheFiles: removed %s", name.c_str());
        }
    } while (FindNextFileW(find, &findData));

    FindClose(find);
}

// Deletes every cached month, for every hostel/mess. Only for a change of
// menu URL: the file names carry the hostel/mess pair and not where the data
// came from, so nothing on disk can be trusted to match the new source.
static void PurgeCacheFiles() {
    std::wstring directory = GetCacheDirectory();
    if (directory.empty()) {
        return;
    }

    std::wstring pattern = directory + L"\\h*m*-*.json";
    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        std::wstring name = findData.cFileName;
        int monthKey = 0;
        if (!ParseCacheFileName(name, monthKey)) {
            continue;
        }
        std::wstring full = directory + L"\\" + name;
        DeleteFileW(full.c_str());
        Wh_Log(L"PurgeCacheFiles: removed %s", name.c_str());
    } while (FindNextFileW(find, &findData));

    FindClose(find);
}

// Loads every cached month for the configured hostel/mess into one map. The
// other sources' files stay on disk untouched, ready for a switch back.
static void LoadCacheFromDisk() {
    MenuStore store;
    CurrentSource(store.hostel, store.mess);

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

// True when the cached file for this month was written more than `maxAgeMs`
// ago, or does not exist. The file's own timestamp is used rather than an
// in-memory "last fetched" tick so an Explorer restart does not trigger a
// fresh download every time.
static bool CachedMonthOlderThan(int hostel, int mess, int monthKey,
                                 ULONGLONG maxAgeMs) {
    std::wstring directory = GetCacheDirectory();
    if (directory.empty()) {
        return true;
    }
    std::wstring path =
        directory + L"\\" + CacheFileName(hostel, mess, monthKey);

    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard,
                              &attributes)) {
        return true;
    }

    FILETIME nowFileTime{};
    GetSystemTimeAsFileTime(&nowFileTime);
    ULARGE_INTEGER now{}, written{};
    now.LowPart = nowFileTime.dwLowDateTime;
    now.HighPart = nowFileTime.dwHighDateTime;
    written.LowPart = attributes.ftLastWriteTime.dwLowDateTime;
    written.HighPart = attributes.ftLastWriteTime.dwHighDateTime;
    if (written.QuadPart > now.QuadPart) {
        return false;  // clock moved backwards; treat as fresh
    }
    // FILETIME is in 100 ns units.
    return (now.QuadPart - written.QuadPart) / 10000ULL > maxAgeMs;
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
// No [[clang::no_destroy]]: this holds no UI-thread-affine object, so its
// destructor is a plain heap free and is safe at process exit.
static std::map<UINT_PTR, std::shared_ptr<RunFromWindowThreadPayload>>
    g_runPayloads;
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
                auto it = g_runPayloads.find((UINT_PTR)message->lParam);
                if (it != g_runPayloads.end()) {
                    payload = it->second;
                    // Claimed under the lock, so the payload runs exactly once.
                    // Two overlapping calls install two hooks on the same
                    // thread and each hook proc sees both messages; without
                    // this, the second one would run the payload again.
                    g_runPayloads.erase(it);
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
// proc actually ran.
//
// `blockIndefinitely` is for the teardown path. Everywhere else a timeout is
// the safe choice, but at unload the alternative to waiting is unloading with
// the button, its Click delegate, the MessBlurBrush and the live timers all
// still pointing into an image Windhawk is about to unmap. A guaranteed slow
// unload beats a probabilistic Explorer crash.
static bool RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param,
                                bool blockIndefinitely = false) {
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
        g_runPayloads[id] = payload;
    }

    if (blockIndefinitely) {
        SendMessageW(hWnd, GetRunFromWindowThreadMessage(), 0, (LPARAM)id);
    } else {
        // SMTO_NOTIMEOUTIFNOTHUNG: a taskbar thread that is merely busy is
        // waited for; only one Windows classifies as hung is abandoned.
        // SMTO_BLOCK: no re-entrancy into this thread while it waits.
        DWORD_PTR result = 0;
        SendMessageTimeoutW(hWnd, GetRunFromWindowThreadMessage(), 0,
                            (LPARAM)id,
                            SMTO_BLOCK | SMTO_ABORTIFHUNG |
                                SMTO_NOTIMEOUTIFNOTHUNG,
                            10000, &result);
    }

    // The hook proc claims the payload under the lock and runs it with the
    // lock released, so a send that gives up after the claim leaves proc()
    // executing on the taskbar thread. Returning now would let the caller --
    // at unload, the image itself -- go away underneath it. If the id is
    // already gone from the map, that is exactly what happened: wait for the
    // run to finish rather than pretend it never started.
    bool claimed = false;
    if (!payload->ran.load()) {
        std::lock_guard<std::mutex> lock(g_runPayloadsMutex);
        claimed = g_runPayloads.erase(id) == 0;
    }
    if (claimed) {
        while (!payload->ran.load()) {
            Sleep(1);
        }
    }
    UnhookWindowsHookEx(hook);

    return payload->ran.load();
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

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Std_Ref_Decref_t = void(WINAPI*)(void*);

static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
static TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
static Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

// Secondary taskbars need their own band type. These two are resolved as
// optional symbols: if a Windows build ever drops or renames them, only
// "Show on: Every taskbar" degrades to the primary taskbar rather than the
// whole mod failing to load.
static CSecondaryTaskBand_GetTaskbarHost_t
    CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
static void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

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

static BOOL CALLBACK CollectTaskbarWndsProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    WCHAR className[32];
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId() ||
        !GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    auto* found = reinterpret_cast<std::vector<HWND>*>(lParam);
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        // The primary always leads, so callers that want only one taskbar can
        // take the front.
        found->insert(found->begin(), hWnd);
    } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        found->push_back(hWnd);
    }
    return TRUE;
}

// Every taskbar in this process: the primary first, then one per extra monitor.
static std::vector<HWND> EnumerateTaskbarWnds() {
    std::vector<HWND> found;
    EnumWindows(CollectTaskbarWndsProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    WCHAR className[32] = {};
    GetClassNameW(hTaskbarWnd, className, ARRAYSIZE(className));
    const bool isSecondary =
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;

    // A secondary taskbar hosts its band in a child WorkerW rather than
    // publishing it as a window property.
    HWND hTaskSwWnd =
        isSecondary ? FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
                    : (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        Wh_Log(L"GetTaskbarXamlRoot: taskband window not found");
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand) {
        Wh_Log(L"GetTaskbarXamlRoot: taskband pointer is null");
        return nullptr;
    }

    void* expectedVftable = isSecondary
                                ? CSecondaryTaskBand_ITaskListWndSite_vftable
                                : CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = isSecondary
                              ? CSecondaryTaskBand_GetTaskbarHost_Original
                              : CTaskBand_GetTaskbarHost_Original;
    if (!expectedVftable || !getTaskbarHost) {
        Wh_Log(L"GetTaskbarXamlRoot: %s symbols not resolved",
               isSecondary ? L"CSecondaryTaskBand" : L"CTaskBand");
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
// Guessing an offset here would hand a fabricated pointer to QueryInterface on
// an architecture nobody has validated. Fail the build instead.
#error "Unsupported architecture"
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

// An element already in the taskbar's tree, for asking what theme is actually
// in effect there. Defined with the UI state below.
static FrameworkElement ThemeProbeElement();

static bool IsLightTheme() {
    // Ask the tree first: another mod can set an explicit RequestedTheme on
    // the taskbar (Taskbar Styler themes do), which the registry knows nothing
    // about. The button's ActualThemeChanged handler already works this way.
    try {
        if (auto element = ThemeProbeElement()) {
            return element.ActualTheme() == ElementTheme::Light;
        }
    } catch (...) {
    }

    // Before the button exists, fall back to the OS setting.
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
//
// Two nested Borders, and the split matters. HitTarget fills the whole control
// and is painted Transparent -- which, unlike a null Background, does take part
// in hit testing -- so the button stays clickable across its entire area. Root
// sits inside it with `verticalInset` of margin and carries the highlight.
//
// Doing the inset with a Margin on the Button itself instead would shrink the
// control, and with it the hit area: the bottom few pixels of the taskbar would
// stop responding, so slamming the pointer into the screen edge would miss the
// button. Windows' own tray buttons take the full height and inset only the
// paint, which is what this reproduces.
static Style MakeSubtleButtonStyle(bool light, int verticalInset) {
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
        <Border x:Name="HitTarget" Background="Transparent">
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
          <Border x:Name="Root"
                  Background="{TemplateBinding Background}"
                  CornerRadius="4"
                  Margin="%INSET%"
                  Padding="{TemplateBinding Padding}">
            <ContentPresenter Content="{TemplateBinding Content}"
                              HorizontalAlignment="{TemplateBinding HorizontalContentAlignment}"
                              VerticalAlignment="{TemplateBinding VerticalContentAlignment}"/>
          </Border>
        </Border>
      </ControlTemplate>
    </Setter.Value>
  </Setter>
</Style>)XAML";

    std::wstring inset =
        L"0," + std::to_wstring(verticalInset) + L",0," +
        std::to_wstring(verticalInset);

    std::wstring xaml = kTemplate;
    ReplaceAll(xaml, L"%HOVER%", light ? L"#18000000" : L"#20FFFFFF");
    ReplaceAll(xaml, L"%PRESSED%", light ? L"#0C000000" : L"#12FFFFFF");
    ReplaceAll(xaml, L"%INSET%", inset);

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
//
// Two variants: the flyout's own buttons sit inside a padded surface and need
// no inset, while the taskbar button insets its highlight to match the height
// of Windows' tray buttons.
static constexpr int kTaskbarButtonInset = 4;

[[clang::no_destroy]] static Style g_subtleButtonStyle{nullptr};
static bool g_subtleButtonStyleIsLight = false;
[[clang::no_destroy]] static Style g_taskbarButtonStyle{nullptr};
static bool g_taskbarButtonStyleIsLight = false;

static Style GetSubtleButtonStyle(bool light) {
    if (!g_subtleButtonStyle || g_subtleButtonStyleIsLight != light) {
        g_subtleButtonStyle = MakeSubtleButtonStyle(light, 0);
        g_subtleButtonStyleIsLight = light;
    }
    return g_subtleButtonStyle;
}

static Style GetTaskbarButtonStyle(bool light) {
    if (!g_taskbarButtonStyle || g_taskbarButtonStyleIsLight != light) {
        g_taskbarButtonStyle =
            MakeSubtleButtonStyle(light, kTaskbarButtonInset);
        g_taskbarButtonStyleIsLight = light;
    }
    return g_taskbarButtonStyle;
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
// One record per taskbar we have injected into: just the primary, or every
// taskbar, depending on the "Show on" setting. Secondary taskbars live on the
// same Explorer UI thread as the primary, so this needs no synchronisation.
struct TaskbarEntry {
    HWND taskbarWnd = nullptr;
    Button button{nullptr};
    // Both glyphs live in the button at once and swap by Visibility: the
    // outline between meals, the filled one while a meal is being served.
    PathIcon icon{nullptr};
    PathIcon iconFilled{nullptr};
    TextBlock label{nullptr};
    Grid injectionParent{nullptr};
    // The tray column we added, or null when we appended without adding one
    // (taskbar-area positions). Held by identity rather than by index: the
    // tray's column list shifts as tray icons and other mods' columns come
    // and go, so an index recorded at injection time can point at somebody
    // else's column by the time we remove ours.
    ColumnDefinition injectedColumnDefinition{nullptr};
    // The taskbar's icon strip, when we are holding space open in front of it.
    FrameworkElement reservedElement{nullptr};
    Thickness reservedOriginalMargin{};
    bool hasReservedOriginalMargin = false;
    winrt::event_token sizeToken{};
    winrt::event_token themeToken{};
};

[[clang::no_destroy]] static std::optional<std::vector<TaskbarEntry>>
    g_taskbars{std::in_place};

static FrameworkElement ThemeProbeElement() {
    if (!g_taskbars) {
        return nullptr;
    }
    for (auto& entry : *g_taskbars) {
        if (entry.button) {
            return entry.button;
        }
    }
    return nullptr;
}

// The taskbar the open flyout belongs to, so its anchor maths use the right
// monitor rather than always the primary one.
static HWND g_flyoutTaskbarWnd = nullptr;
[[clang::no_destroy]] static Button g_flyoutAnchorButton{nullptr};

// The taskbar the flyout is being opened from, which on a multi-monitor setup
// is not necessarily the primary one.
static HWND FlyoutTaskbarWnd() {
    return g_flyoutTaskbarWnd ? g_flyoutTaskbarWnd : g_taskbarWnd.load();
}

[[clang::no_destroy]] static Flyout g_flyout{nullptr};
[[clang::no_destroy]] static Border g_flyoutRoot{nullptr};
[[clang::no_destroy]] static TextBlock g_headerDay{nullptr};
[[clang::no_destroy]] static TextBlock g_headerDate{nullptr};
[[clang::no_destroy]] static Button g_prevDayButton{nullptr};
[[clang::no_destroy]] static Button g_nextDayButton{nullptr};
[[clang::no_destroy]] static StackPanel g_cardsPanel{nullptr};
[[clang::no_destroy]] static Button g_reloadButton{nullptr};
[[clang::no_destroy]] static FontIcon g_reloadIcon{nullptr};
[[clang::no_destroy]] static ProgressRing g_reloadRing{nullptr};
[[clang::no_destroy]] static DispatcherTimer g_timer{nullptr};

// std::vector is not nullable, so clear() alone would leave the heap buffer for
// the CRT to free at shutdown. The optional wrapper gives a reset() that
// releases it while the mod is still mapped.
[[clang::no_destroy]] static std::optional<std::vector<TextBlock>>
    g_cardCountdowns{std::in_place};
// Plain ints, so no [[clang::no_destroy]] needed -- only the two vectors above
// hold XAML objects.
static std::vector<int> g_cardMeals;

// Every DispatcherTimer we start, so Wh_ModUninit can stop the pending ones.
// A timer that fires into an unmapped image crashes Explorer regardless of any
// g_unloading check inside the callback -- the crash is the call itself.
// Touched only from the taskbar UI thread, so it needs no lock.
//
// The token is stored alongside the timer because every Tick handler captures
// its own timer by value: timer -> delegate -> timer is a cycle that only the
// handler's own detach breaks. Stopping a timer that never fires would
// otherwise leak it, its delegate, and everything else the handler captured.
[[clang::no_destroy]] static std::optional<
    std::vector<std::pair<DispatcherTimer, winrt::event_token>>>
    g_liveTimers{std::in_place};

// The reveal animation's SizeChanged handler races its fallback timer and is
// only detached when one of them wins; if neither has by unload, detach here.
[[clang::no_destroy]] static Border g_revealTarget{nullptr};
static winrt::event_token g_revealSizeToken{};
// Both Storyboards, for the same reason: while one runs, XAML's timing
// manager roots it, and it holds the flyout tree -- and the Click delegates
// in it, whose code lives in this image -- alive. An unload inside the 250 ms
// would otherwise let the animation's completion release them into unmapped
// memory. Stop()ped and nulled in TearDownFlyout.
[[clang::no_destroy]] static Storyboard g_revealStoryboard{nullptr};
[[clang::no_destroy]] static Storyboard g_closingStoryboard{nullptr};

// The Flyout's own event registrations, revoked in TearDownFlyout before the
// Flyout itself is released, like every other handler the mod attaches.
static winrt::event_token g_flyoutOpenedToken{};
static winrt::event_token g_flyoutClosingToken{};
static winrt::event_token g_flyoutClosedToken{};

// Each of these checks the optional first: Wh_ModUninit reset()s it, and a
// stray callback arriving afterwards must not dereference an empty one.
static void TrackTimer(DispatcherTimer const& timer, winrt::event_token token) {
    if (!g_liveTimers) {
        return;
    }
    try {
        g_liveTimers->emplace_back(timer, token);
    } catch (...) {
    }
}

static void UntrackTimer(DispatcherTimer const& timer) {
    if (!g_liveTimers) {
        return;
    }
    try {
        auto& timers = *g_liveTimers;
        timers.erase(std::remove_if(timers.begin(), timers.end(),
                                    [&timer](auto const& entry) {
                                        return entry.first == timer;
                                    }),
                     timers.end());
    } catch (...) {
    }
}

static void StopAllTimers() {
    if (!g_liveTimers) {
        return;
    }
    try {
        for (auto& entry : *g_liveTimers) {
            try {
                entry.first.Stop();
                // Breaks the timer -> delegate -> timer cycle. Stop() alone
                // leaves the handler attached and the whole graph alive.
                if (entry.second.value) {
                    entry.first.Tick(entry.second);
                }
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
static void InjectWithRetry(int generation, int attempt = 0);

// Bumped every time the taskbar is (re)created. A retry chain started for an
// older taskbar carries its generation and gives up when it no longer matches,
// so two chains racing after a quick double restart cannot both inject -- which
// would otherwise leave the visible button unmanaged while the globals point at
// a dead tree.
static int g_injectGeneration = 0;

// Set when Wh_ModSettingsChanged could not reach the taskbar thread to
// re-apply the settings (the send was abandoned because the thread was
// classified as hung). The UI timer picks it up once the thread is responsive
// again, so the new settings are not silently ignored until the next change.
static std::atomic<bool> g_reapplySettingsPending{false};

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

    // Only the dish list needs the menu; the countdown does not. Reaching for
    // the store first and bailing out on a miss used to show "No menu" between
    // meals on first run or over an uncached month boundary, even though the
    // countdown was perfectly computable.
    if (state.currentMeal >= 0) {
        // The grouping work happens under the lock, against a reference rather
        // than a copy: copying DayMenu copies four std::wstrings every time.
        std::lock_guard<std::mutex> lock(g_dataMutex);
        auto it = g_store.days.find(todayKey);
        if (it != g_store.days.end()) {
            GroupedMenu grouped =
                GroupMenuItems(it->second.raw[state.currentMeal]);
            // Prefer the main dishes: "Idli • Vada" is a more useful glance
            // than "Tea • Coffee • Milk".
            for (int group = 0; group < kGroupCount; group++) {
                if (!grouped.groups[group].empty()) {
                    return JoinItems(grouped.groups[group], 4);
                }
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

// The entry whose button is `element`, or null. Handlers look themselves up
// this way rather than capturing a pointer, because the vector reallocates as
// taskbars come and go.
static TaskbarEntry* FindEntryForButton(FrameworkElement const& element) {
    if (!g_taskbars || !element) {
        return nullptr;
    }
    auto button = element.try_as<Button>();
    if (!button) {
        return nullptr;
    }
    for (auto& entry : *g_taskbars) {
        if (entry.button == button) {
            return &entry;
        }
    }
    return nullptr;
}

// Shows the filled glyph while a meal is being served, the outline otherwise.
// Setting-off means the outline always. Cheap enough to call on every label
// refresh; Visibility is only written when it actually changes.
static void ApplyIconState(TaskbarEntry& entry, bool serving) {
    if (!entry.icon || !entry.iconFilled) {
        return;
    }
    const bool filled = serving && g_settings.filledIconWhenServing;
    try {
        const Visibility outlineWanted =
            filled ? Visibility::Collapsed : Visibility::Visible;
        const Visibility filledWanted =
            filled ? Visibility::Visible : Visibility::Collapsed;
        if (entry.icon.Visibility() != outlineWanted) {
            entry.icon.Visibility(outlineWanted);
        }
        if (entry.iconFilled.Visibility() != filledWanted) {
            entry.iconFilled.Visibility(filledWanted);
        }
    } catch (...) {
    }
}

static void UpdateTaskbarLabel() {
    if (!g_taskbars || g_taskbars->empty()) {
        return;
    }
    try {
        MealState state = ComputeMealState();

        // Before the label cache's early-outs: the icon depends only on
        // whether a meal is in progress, and a freshly injected button starts
        // on the outline regardless of what the cache remembers.
        for (auto& entry : *g_taskbars) {
            ApplyIconState(entry, state.currentMeal >= 0);
        }

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

        // The text is the same on every taskbar, so it is computed once above
        // and only applied per entry here.
        std::wstring tooltip = text + L"\n" + HostelDisplayName() + L" • " +
                               MessDisplayName();
        auto boxedTooltip = winrt::box_value(winrt::hstring{tooltip});

        for (auto& entry : *g_taskbars) {
            // In compact mode there is no label, but the tooltip still carries
            // the full text -- that is the whole point of compact mode.
            if (entry.label) {
                entry.label.Text(text);
            }
            if (entry.button) {
                ToolTipService::SetToolTip(entry.button, boxedTooltip);
            }
        }
    } catch (...) {
        Wh_Log(L"UpdateTaskbarLabel: exception");
    }
}

static void ShowMessFlyout(FrameworkElement const& target);

// Geometry has no public parser in C++/WinRT, so the path mini-language goes
// through XamlReader. Null if that fails, which it should not with fixed data.
static PathIcon LoadPathIcon(const wchar_t* data) {
    static const wchar_t* kIconXaml =
        L"<PathIcon xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        L"presentation\" HorizontalAlignment=\"Left\" "
        L"VerticalAlignment=\"Top\" Data=\"%DATA%\"/>";
    std::wstring xaml = kIconXaml;
    ReplaceAll(xaml, L"%DATA%", data);
    try {
        return Markup::XamlReader::Load(xaml).try_as<PathIcon>();
    } catch (...) {
        Wh_Log(L"LoadPathIcon: XamlReader failed");
        return nullptr;
    }
}

// The 48-unit Fluent glyphs scaled to tray-icon size. Each PathIcon sits at
// its designed offset inside a 48x48 canvas and the Viewbox scales the canvas,
// so the glyph keeps the padding the icon set designed in rather than being
// stretched to its own bounds. Both variants are loaded once and overlaid;
// ApplyIconState picks which is visible, so a state change is one property
// set rather than another XamlReader pass. Returns the element to place;
// `icon` and `iconFilled` receive the PathIcons so their Foreground can
// follow the theme.
static FrameworkElement MakeTaskbarIcon(bool light, PathIcon& icon,
                                        PathIcon& iconFilled) {
    icon = LoadPathIcon(kTaskbarIconData);
    iconFilled = LoadPathIcon(kTaskbarIconFilledData);
    if (!icon) {
        // A text fallback beats an invisible button.
        iconFilled = nullptr;
        TextBlock fallback;
        fallback.Text(L"•");
        fallback.FontSize(14);
        fallback.Foreground(MakeBrush(TextPrimaryColor(light)));
        fallback.VerticalAlignment(VerticalAlignment::Center);
        return fallback;
    }
    icon.Foreground(MakeBrush(TextPrimaryColor(light)));

    Grid canvas;
    canvas.Width(kTaskbarIconCanvas);
    canvas.Height(kTaskbarIconCanvas);
    canvas.Children().Append(icon);
    if (iconFilled) {
        iconFilled.Foreground(MakeBrush(TextPrimaryColor(light)));
        iconFilled.Visibility(Visibility::Collapsed);
        canvas.Children().Append(iconFilled);
    }

    Viewbox viewbox;
    viewbox.Width(16);
    viewbox.Height(16);
    viewbox.Stretch(Stretch::Uniform);
    viewbox.VerticalAlignment(VerticalAlignment::Center);
    viewbox.Child(canvas);
    return viewbox;
}

// Fills entry.button, entry.icon, entry.iconFilled and entry.label. The entry must already be in
// g_taskbars, so the handlers below can find it again.
static void BuildTaskbarButton(bool light, TaskbarEntry& entry) {
    Button button;
    if (auto style = GetTaskbarButtonStyle(light)) {
        button.Style(style);
    }
    // No vertical margin: the button takes the tray's full height so it stays
    // clickable right down to the screen edge, the way the clock and the other
    // tray buttons do. The highlight is inset inside the template instead, so
    // it still matches their height (40px of a 48px taskbar).
    button.Padding({8, 0, 8, 0});
    button.Margin({(double)g_settings.buttonPaddingLeft, 0,
                   (double)g_settings.buttonPaddingRight, 0});
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.HorizontalAlignment(HorizontalAlignment::Center);

    StackPanel panel;
    panel.Orientation(Orientation::Horizontal);
    panel.VerticalAlignment(VerticalAlignment::Center);

    panel.Children().Append(
        MakeTaskbarIcon(light, entry.icon, entry.iconFilled));

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
        entry.label = label;
    } else {
        entry.label = nullptr;
    }

    button.Content(panel);

    // The tray's own buttons expose a name to screen readers; without one
    // this would be announced as an unnamed button.
    winrt::Windows::UI::Xaml::Automation::AutomationProperties::SetName(
        button, L"Mess menu");

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

    // The theme is baked into the label colour and the style's hover/pressed
    // colours, and the button outlives a theme switch -- without this, going
    // dark -> light leaves white text on a light taskbar, i.e. invisible. The
    // flyout is already fine because it is rebuilt on every open.
    entry.button = button;
    entry.themeToken = button.ActualThemeChanged(
        [](FrameworkElement const& sender,
           winrt::Windows::Foundation::IInspectable const&) {
            try {
                if (g_unloading) {
                    return;
                }
                // The event already carries the new theme, so there is no need
                // to go back to the registry for it.
                const bool nowLight =
                    sender.ActualTheme() == ElementTheme::Light;
                if (auto style = GetTaskbarButtonStyle(nowLight)) {
                    sender.as<Control>().Style(style);
                }
                if (auto* entry = FindEntryForButton(sender)) {
                    if (entry->icon) {
                        entry->icon.Foreground(
                            MakeBrush(TextPrimaryColor(nowLight)));
                    }
                    if (entry->iconFilled) {
                        entry->iconFilled.Foreground(
                            MakeBrush(TextPrimaryColor(nowLight)));
                    }
                    if (entry->label) {
                        entry->label.Foreground(
                            MakeBrush(TextPrimaryColor(nowLight)));
                    }
                }
            } catch (...) {
                Wh_Log(L"ActualThemeChanged: exception");
            }
        });
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
    const ButtonPosition position = g_settings.position;
    int columnCount = (int)trayGrid.ColumnDefinitions().Size();

    if (position == ButtonPosition::TrayLeft) {
        return 0;
    }
    if (position == ButtonPosition::InputLeft) {
        // NonActivatableStack holds the input indicator (the language
        // switcher) along with the microphone / location in-use badges. It is
        // a direct child of the tray grid even on a single-language system
        // where the indicator itself is hidden.
        int inputColumn =
            ColumnOfChildContaining(trayGrid, L"NonActivatableStack");
        if (inputColumn >= 0) {
            return inputColumn;
        }
        Wh_Log(L"ResolveInsertColumn: NonActivatableStack not found, "
               L"appending at the end of the tray");
    }
    if (position == ButtonPosition::NetworkLeft) {
        // ControlCenterButton is the network / volume / battery group.
        int networkColumn =
            ColumnOfChildContaining(trayGrid, L"ControlCenterButton");
        if (networkColumn >= 0) {
            return networkColumn;
        }
        Wh_Log(L"ResolveInsertColumn: ControlCenterButton not found, "
               L"appending at the end of the tray");
    }
    if (position == ButtonPosition::ClockLeft ||
        position == ButtonPosition::ClockRight) {
        int clockColumn = ColumnOfChildContaining(trayGrid,
                                                  L"NotificationCenterButton");
        if (clockColumn >= 0) {
            return position == ButtonPosition::ClockLeft ? clockColumn
                                                         : clockColumn + 1;
        }
        Wh_Log(L"ResolveInsertColumn: NotificationCenterButton not found, "
               L"appending at the end of the tray");
    }
    // Fallback: append after everything else. Indistinguishable from "Right
    // of the clock" on screen, which is why the misses above are logged.
    return columnCount;
}

static bool IsTaskbarAreaPosition() {
    return g_settings.position == ButtonPosition::TaskbarLeft;
}

// Holds the taskbar's icon strip clear of the button by widening its left
// margin. With centred icons this shrinks the space they centre within, so they
// stay centred and simply never reach far enough left to collide; with
// left-aligned icons it pushes them right. Driven by the button's SizeChanged,
// so it keeps up as the label text changes width.
static void UpdateReservedSpace(TaskbarEntry& entry) {
    if (!entry.reservedElement || !entry.button ||
        !entry.hasReservedOriginalMargin || g_unloading) {
        return;
    }
    try {
        double width = entry.button.ActualWidth();
        if (width <= 0.0) {
            return;
        }
        double wanted = entry.reservedOriginalMargin.Left + width +
                        (double)g_settings.buttonPaddingLeft +
                        (double)g_settings.buttonPaddingRight;

        auto margin = entry.reservedElement.Margin();
        if (std::abs(margin.Left - wanted) > 1.0) {
            margin.Left = wanted;
            entry.reservedElement.Margin(margin);
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

// Keeps the tray's children where they were after a column is inserted at
// `column`: anything at or past it moves right by one, and anything that
// starts before it but spans across it grows by one so it still covers the
// same columns.
static void ShiftGridChildrenForInsertedColumn(Grid const& grid, int column) {
    const uint32_t count = grid.Children().Size();
    for (uint32_t i = 0; i < count; i++) {
        auto child = grid.Children().GetAt(i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        const int childColumn = Grid::GetColumn(child);
        const int childSpan = Grid::GetColumnSpan(child);
        if (childColumn >= column) {
            Grid::SetColumn(child, childColumn + 1);
        } else if (childColumn + childSpan > column) {
            Grid::SetColumnSpan(child, childSpan + 1);
        }
    }
}

// The inverse, after the column at `column` has been removed.
static void ShiftGridChildrenForRemovedColumn(Grid const& grid, int column) {
    const uint32_t count = grid.Children().Size();
    for (uint32_t i = 0; i < count; i++) {
        auto child = grid.Children().GetAt(i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        const int childColumn = Grid::GetColumn(child);
        const int childSpan = Grid::GetColumnSpan(child);
        if (childColumn > column) {
            Grid::SetColumn(child, childColumn - 1);
        } else if (childColumn < column && childColumn + childSpan > column &&
                   childSpan > 1) {
            Grid::SetColumnSpan(child, childSpan - 1);
        }
    }
}

static void RemoveTaskbarButtonFrom(TaskbarEntry& entry) {
    // Give the taskbar its own layout back before anything else, so an
    // exception later cannot leave the icons permanently shoved aside.
    try {
        if (entry.button && entry.sizeToken.value) {
            entry.button.SizeChanged(entry.sizeToken);
        }
    } catch (...) {
    }
    entry.sizeToken = {};

    try {
        if (entry.button && entry.themeToken.value) {
            entry.button.ActualThemeChanged(entry.themeToken);
        }
    } catch (...) {
    }
    entry.themeToken = {};

    try {
        if (entry.reservedElement && entry.hasReservedOriginalMargin) {
            entry.reservedElement.Margin(entry.reservedOriginalMargin);
        }
    } catch (...) {
        Wh_Log(L"RemoveTaskbarButton: could not restore the icon strip margin");
    }
    entry.reservedElement = nullptr;
    entry.hasReservedOriginalMargin = false;

    try {
        if (entry.injectionParent && entry.button) {
            uint32_t index = 0;
            if (entry.injectionParent.Children().IndexOf(entry.button, index)) {
                entry.injectionParent.Children().RemoveAt(index);
            }
            // Find our column by identity, now, rather than trusting the
            // index we inserted at. If it is no longer in the list, whoever
            // removed it also owns the child shifts that went with it, so
            // there is nothing for us to undo.
            int ownedColumn = -1;
            if (entry.injectedColumnDefinition) {
                auto definitions = entry.injectionParent.ColumnDefinitions();
                const uint32_t count = definitions.Size();
                for (uint32_t i = 0; i < count; i++) {
                    if (definitions.GetAt(i) == entry.injectedColumnDefinition) {
                        ownedColumn = (int)i;
                        break;
                    }
                }
            }
            if (ownedColumn >= 0) {
                entry.injectionParent.ColumnDefinitions().RemoveAt(
                    (uint32_t)ownedColumn);
                ShiftGridChildrenForRemovedColumn(entry.injectionParent,
                                                  ownedColumn);
            }
        }
    } catch (...) {
        Wh_Log(L"RemoveTaskbarButton: exception");
    }

    entry.button = nullptr;
    entry.icon = nullptr;
    entry.iconFilled = nullptr;
    entry.label = nullptr;
    entry.injectionParent = nullptr;
    entry.injectedColumnDefinition = nullptr;
}

static void RemoveTaskbarButton() {
    if (!g_taskbars) {
        return;
    }
    for (auto& entry : *g_taskbars) {
        RemoveTaskbarButtonFrom(entry);
    }
    g_taskbars->clear();
    InvalidateLabelCache();
}

// Injects into one taskbar, appending its record to g_taskbars on success.
static bool InjectTaskbarButtonInto(HWND hWnd) {
    if (!hWnd || !g_taskbars) {
        return false;
    }

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

        // Pushed before the button is built so the handlers, which look
        // themselves up by button, can find their own record.
        g_taskbars->push_back(TaskbarEntry{});
        TaskbarEntry& entry = g_taskbars->back();
        entry.taskbarWnd = hWnd;

        const bool light = IsLightTheme();
        BuildTaskbarButton(light, entry);
        Button button = entry.button;

        // --- taskbar-area positions: sit in the taskbar's own grid ----------
        if (IsTaskbarAreaPosition()) {
            auto rootGrid = FindTaskbarRootGrid(root);
            if (!rootGrid) {
                Wh_Log(L"InjectTaskbarButton: taskbar RootGrid not found");
                RemoveTaskbarButtonFrom(entry);
                g_taskbars->pop_back();
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

            entry.injectionParent = rootGrid;
            entry.injectedColumnDefinition = nullptr;

            // Nothing lets us see where another mod has parked itself, so the
            // spacing settings stay the manual escape hatch. What we can do is
            // stop the taskbar's own icons from sitting underneath us.
            if (g_settings.reserveTaskbarSpace) {
                auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
                if (repeater) {
                    entry.reservedElement = repeater;
                    entry.reservedOriginalMargin = repeater.Margin();
                    entry.hasReservedOriginalMargin = true;
                    entry.sizeToken = button.SizeChanged(
                        [](winrt::Windows::Foundation::IInspectable const& sender,
                           SizeChangedEventArgs const&) {
                            if (auto* self = FindEntryForButton(
                                    sender.try_as<FrameworkElement>())) {
                                UpdateReservedSpace(*self);
                            }
                        });
                    UpdateReservedSpace(entry);
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
            ShiftGridChildrenForInsertedColumn(trayGrid, insertColumn);
        }

        Grid::SetColumn(button, insertColumn);
        trayGrid.Children().Append(button);

        entry.injectionParent = trayGrid;
        entry.injectedColumnDefinition = column;

        InvalidateLabelCache();
        UpdateTaskbarLabel();
        return true;
    } catch (...) {
        Wh_Log(L"InjectTaskbarButton: exception");
        // The half-built record must not linger: its button may already carry
        // handlers pointing into this image.
        if (g_taskbars && !g_taskbars->empty()) {
            TaskbarEntry& last = g_taskbars->back();
            if (last.taskbarWnd == hWnd && !last.injectionParent) {
                RemoveTaskbarButtonFrom(last);
                g_taskbars->pop_back();
            }
        }
        return false;
    }
}

// The taskbars this configuration should own a button on. The primary comes
// first, so a failure to enumerate secondaries still leaves it working.
static std::vector<HWND> TargetTaskbarWnds() {
    std::vector<HWND> all = EnumerateTaskbarWnds();
    if (g_settings.taskbarScope == TaskbarScope::All) {
        // Without the optional CSecondaryTaskBand symbols a secondary can
        // never be injected into. Leave them out here rather than let the
        // periodic reconcile below retry them -- and log the failure -- every
        // twenty seconds for the life of the session.
        if (CSecondaryTaskBand_GetTaskbarHost_Original &&
            CSecondaryTaskBand_ITaskListWndSite_vftable) {
            return all;
        }
        static bool warned = false;
        if (!warned && all.size() > 1) {
            warned = true;
            Wh_Log(L"TargetTaskbarWnds: CSecondaryTaskBand symbols not "
                   L"resolved, primary taskbar only");
        }
    }
    std::vector<HWND> primaryOnly;
    if (!all.empty()) {
        primaryOnly.push_back(all.front());
    }
    return primaryOnly;
}

// Injects into every taskbar that should have a button and does not yet.
// Returns true when they all have one.
static bool InjectTaskbarButton() {
    if (!g_taskbars) {
        return false;
    }

    std::vector<HWND> targets = TargetTaskbarWnds();
    if (targets.empty()) {
        Wh_Log(L"InjectTaskbarButton: no taskbar window found");
        return false;
    }

    g_taskbarWnd.store(targets.front());

    bool allInjected = true;
    for (HWND hWnd : targets) {
        bool present = false;
        for (auto& entry : *g_taskbars) {
            if (entry.taskbarWnd == hWnd && entry.button) {
                present = true;
                break;
            }
        }
        if (present) {
            continue;
        }
        if (!InjectTaskbarButtonInto(hWnd)) {
            allInjected = false;
        }
    }
    return allInjected;
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

    // Header row: meal name on the left, countdown on the right.
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

    auto title = MakeTextBlock(kMealNames[(int)meal], 14,
                               TextPrimaryColor(light), true);
    title.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetColumn(title, 0);
    header.Children().Append(title);

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

    g_cardCountdowns->push_back(countdown);
    g_cardMeals.push_back((int)meal);
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
        g_cardCountdowns->clear();
        g_cardMeals.clear();

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
                const wchar_t* subtitle =
                    g_fetching ? L"Checking now…"
                    : g_settings.autoUpdate
                        ? L"Checking periodically for updates…"
                        : L"Automatic checks are off — use the reload button "
                          L"below.";
                AddMessageBlock(L"Menu data not available for this month.",
                                subtitle, light);
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
        // On the button, not the glyph: the glyph is swapped for the progress
        // ring while a fetch runs, so a tooltip on it would vanish exactly when
        // there is something to say, and would only appear on the icon's own
        // few pixels the rest of the time.
        if (g_reloadButton) {
            ToolTipService::SetToolTip(
                g_reloadButton, winrt::box_value(winrt::hstring{tooltip}));
        }
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
            MonitorFromWindow(FlyoutTaskbarWnd(), MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (monitor && GetMonitorInfo(monitor, &info)) {
            // g_flyoutRoot is not assigned until this function returns, so ask
            // the taskbar button -- it is already in the tree.
            double scale = 1.0;
            if (g_flyoutAnchorButton) {
                if (auto xamlRoot = g_flyoutAnchorButton.XamlRoot()) {
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

    auto footerText = MakeTextBlock(
        HostelDisplayName() + L" • " + MessDisplayName(), 11,
        TextTertiaryColor(light));
    footerText.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetColumn(footerText, 0);
    footerRow.Children().Append(footerText);

    Button reloadButton;
    g_reloadButton = reloadButton;
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
    HWND hWnd = FlyoutTaskbarWnd();
    if (!hWnd) {
        return false;
    }
    RECT taskbarRect{};
    if (!GetWindowRect(hWnd, &taskbarRect)) {
        return false;
    }
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
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
    if (!g_cardCountdowns) {
        return;  // already reset by Wh_ModUninit
    }
    g_flyoutRoot = nullptr;
    g_headerDay = nullptr;
    g_headerDate = nullptr;
    g_prevDayButton = nullptr;
    g_nextDayButton = nullptr;
    g_cardsPanel = nullptr;
    g_reloadButton = nullptr;
    g_reloadIcon = nullptr;
    g_reloadRing = nullptr;
    g_cardCountdowns->clear();
    g_cardMeals.clear();
}

// One teardown for every path that discards the flyout: unload, settings
// change, and taskbar recreation.
//
// The order matters. Anything whose code lives in this image -- the closing
// Storyboard, the reveal handler, MessBlurBrush -- has to be released before
// the tree that owns it. Note that clearing the Flyout's Content is what
// actually releases the brush: by the time the flyout has been closed once,
// ClearFlyoutRefs has already nulled g_flyoutRoot, but the Flyout still owns
// clipHost -> content -> Background -> brush, so touching g_flyoutRoot alone
// would silently do nothing in the common case.
static void TearDownFlyout() {
    try {
        if (g_revealStoryboard) {
            g_revealStoryboard.Stop();
        }
    } catch (...) {
    }
    g_revealStoryboard = nullptr;

    try {
        if (g_closingStoryboard) {
            g_closingStoryboard.Stop();
        }
    } catch (...) {
    }
    g_closingStoryboard = nullptr;

    // The reveal handler detaches itself when it fires; if it has not fired, it
    // is still attached here.
    try {
        if (g_revealTarget && g_revealSizeToken.value) {
            g_revealTarget.SizeChanged(g_revealSizeToken);
        }
    } catch (...) {
    }
    g_revealSizeToken = {};
    g_revealTarget = nullptr;

    try {
        if (g_flyoutRoot) {
            g_flyoutRoot.Background(nullptr);
        }
    } catch (...) {
    }

    try {
        if (g_flyout) {
            if (g_flyoutOpen.load()) {
                // Tell the Closing handler this is our own Hide, so it does not
                // cancel it to run the close animation.
                g_flyoutClosingAnimInProgress.store(true);
                g_flyout.Hide();
            }
            g_flyout.Content(nullptr);
        }
    } catch (...) {
        g_flyoutClosingAnimInProgress.store(false);
    }

    // After Hide(), so its Closing and Closed still ran through the handlers.
    try {
        if (g_flyout) {
            if (g_flyoutOpenedToken.value) {
                g_flyout.Opened(g_flyoutOpenedToken);
            }
            if (g_flyoutClosingToken.value) {
                g_flyout.Closing(g_flyoutClosingToken);
            }
            if (g_flyoutClosedToken.value) {
                g_flyout.Closed(g_flyoutClosedToken);
            }
        }
    } catch (...) {
    }
    g_flyoutOpenedToken = {};
    g_flyoutClosingToken = {};
    g_flyoutClosedToken = {};

    g_flyout = nullptr;
    g_flyoutOpen = false;
    g_flyoutClosingAnimStarted.store(false);
    // If Hide() above was a no-op (the flyout was already mid-close), Closing
    // never ran to consume the flag, and the next flyout's first close would
    // skip its animation. Deterministic is better than usually-right.
    g_flyoutClosingAnimInProgress.store(false);
    g_flyoutTaskbarWnd = nullptr;
    g_flyoutAnchorButton = nullptr;
    ClearFlyoutRefs();
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

        // Release the previous flyout deterministically. Closed() clears the
        // child refs but not g_flyout itself, so without this the old Flyout --
        // and the clipHost -> content -> Background -> MessBlurBrush chain it
        // still owns -- would be dropped only by the assignment below, leaving
        // its release to whenever XAML lets go.
        TearDownFlyout();

        // Remember which taskbar this came from before anything measures a
        // monitor: on a multi-monitor setup the flyout must follow the button
        // that was clicked, not the primary taskbar.
        g_flyoutAnchorButton = target.try_as<Button>();
        if (auto* entry = FindEntryForButton(target)) {
            g_flyoutTaskbarWnd = entry->taskbarWnd;
        } else {
            g_flyoutTaskbarWnd = g_taskbarWnd.load();
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

        g_flyoutOpenedToken = flyout.Opened([content](auto const&, auto const&) {
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
                    g_revealStoryboard = storyboard;
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
            TrackTimer(fallback, *fallbackToken);
            fallback.Start();
        });

        g_flyoutClosingToken = flyout.Closing(
            [content](Primitives::FlyoutBase const& sender,
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
                TrackTimer(safety, *safetyToken);
                safety.Start();
                storyboard.Begin();
            } catch (...) {
                g_flyoutClosingAnimStarted.store(false);
            }
        });

        g_flyoutClosedToken = flyout.Closed([](auto const&, auto const&) {
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
                    HMONITOR monitor = MonitorFromWindow(
                        FlyoutTaskbarWnd(), MONITOR_DEFAULTTONEAREST);
                    MONITORINFO monitorInfo{};
                    monitorInfo.cbSize = sizeof(monitorInfo);
                    POINT origin{0, 0};
                    if (monitor && GetMonitorInfo(monitor, &monitorInfo) &&
                        ClientToScreen(FlyoutTaskbarWnd(), &origin)) {
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

// Monitors come and go at runtime -- docking a laptop, switching a display
// off -- and TrayUI::StartTaskbar does not re-fire for that. Without this, a
// removed monitor's entry would sit in g_taskbars forever holding references
// into a dead tree, and a newly attached one would never get a button.
//
// Runs from the UI timer, so a new taskbar is picked up within one idle tick.
// InjectTaskbarButton already skips taskbars that have a button, so it is
// only called when one is actually missing -- otherwise it would re-resolve
// the XAML root every tick for nothing.
static void ReconcileTaskbars() {
    if (!g_taskbars || g_unloading) {
        return;
    }

    // The timer ticks every second while the flyout is open; enumerating
    // windows that often is pointless, so hold this to the idle cadence.
    static ULONGLONG lastRunTick = 0;
    const ULONGLONG now = GetTickCount64();
    if (lastRunTick != 0 && now - lastRunTick < (ULONGLONG)kIdleTickMs) {
        return;
    }
    lastRunTick = now;

    try {
        // The flyout is anchored to a taskbar; if that one is gone, the
        // flyout's tree went with it.
        if (g_flyoutTaskbarWnd && !IsWindow(g_flyoutTaskbarWnd)) {
            TearDownFlyout();
        }

        auto& entries = *g_taskbars;
        bool removed = false;
        for (auto& entry : entries) {
            if (entry.taskbarWnd && !IsWindow(entry.taskbarWnd)) {
                // Revokes the tokens; the element removals fail harmlessly
                // against the dead tree, each inside its own try/catch.
                RemoveTaskbarButtonFrom(entry);
                removed = true;
            }
        }
        if (removed) {
            entries.erase(std::remove_if(entries.begin(), entries.end(),
                                         [](const TaskbarEntry& entry) {
                                             return !entry.button;
                                         }),
                          entries.end());
            InvalidateLabelCache();
        }

        for (HWND hWnd : TargetTaskbarWnds()) {
            bool present = false;
            for (auto& entry : entries) {
                if (entry.taskbarWnd == hWnd && entry.button) {
                    present = true;
                    break;
                }
            }
            if (!present) {
                InjectTaskbarButton();
                break;
            }
        }
    } catch (...) {
        Wh_Log(L"ReconcileTaskbars: exception");
    }
}

static void OnTimerTick() {
    if (g_unloading || !g_cardCountdowns) {
        return;
    }

    try {
        // A settings change that could not reach this thread at the time.
        if (g_reapplySettingsPending.exchange(false)) {
            Wh_Log(L"OnTimerTick: re-applying settings");
            TearDownFlyout();
            RemoveTaskbarButton();
            InjectWithRetry(++g_injectGeneration);
            return;
        }

        ApplyTimerInterval();
        ReconcileTaskbars();
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
            int meal = g_cardMeals[i];
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

// Guards the worker's lifecycle. StartNetThread is reachable from two threads
// at once (the engine thread via Wh_ModAfterInit, the taskbar thread via
// TrayUI::StartTaskbar), and losing that race would start a second worker whose
// handle is then overwritten -- leaving an unjoinable thread running after the
// image is unmapped. KickFetch takes it too, so it cannot signal an event that
// StopNetThread has just closed.
//
// StopNetThread deliberately drops the lock before joining: the worker can be
// blocked in SendMessageTimeoutW to the UI thread, and the UI thread can be in
// KickFetch waiting on this very mutex.
static std::mutex g_netMutex;
static HANDLE g_netThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_kickEvent = nullptr;

// Reloading the cache means parsing JSON, which means touching WinRT, so it has
// to happen on the worker thread (the only one whose apartment we control) and
// never on whichever thread Windhawk calls Wh_ModSettingsChanged from.
static std::atomic<bool> g_reloadCacheRequested{false};

// Set with g_reloadCacheRequested when the menu URL changes: every cached
// file came from the old source and none of them is trustworthy any more.
static std::atomic<bool> g_purgeCacheRequested{false};

static constexpr DWORD kIdleIntervalMs = 6 * 60 * 60 * 1000;   // 6 hours
static constexpr DWORD kFirstBackoffMs = 15 * 60 * 1000;       // 15 minutes

// The site revises a month's file after publishing it -- in September 2026 the
// breakfast sides and drinks were missing for the first week and added later.
// So a cached month is not final: re-download the current one once a day.
static constexpr ULONGLONG kRefreshAgeMs = 24ULL * 60 * 60 * 1000;

static void KickFetch() {
    std::lock_guard<std::mutex> lock(g_netMutex);
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
    int hostel, mess;
    CurrentSource(hostel, mess);

    g_fetching.store(true);
    NotifyUiDataChanged();

    std::string body;
    std::wstring error;
    bool ok = HttpGetJson(ResolveMenuUrl(hostel, mess), body, error);

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

        // Keep the previous, current and next month; drop anything older,
        // for every source.
        PruneOldCacheFiles(MonthKeyFromDayKey(TodayKey()) - 1);

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
    // Snapshot the handles: StopNetThread clears the globals under the lock
    // before it joins, so the worker must not read them afterwards. They are
    // set before CreateThread, so this is safe without the lock.
    HANDLE stopEvent = g_stopEvent;
    HANDLE kickEvent = g_kickEvent;

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

    // Tick of the last successful fetch, 0 until there has been one. The
    // file's age alone is not enough: near the end of a month the site can
    // already be serving the next month's file, so a refresh would write
    // 2026-10.json and leave 2026-09.json looking stale forever.
    ULONGLONG lastSuccessTick = 0;

    for (;;) {
        if (g_unloading) {
            break;
        }

        if (g_reloadCacheRequested.exchange(false)) {
            if (g_purgeCacheRequested.exchange(false)) {
                PurgeCacheFiles();
            }
            LoadCacheFromDisk();
            NotifyUiDataChanged();
        }

        const int todayKey = TodayKey();
        const bool covered = StoreCoversDay(todayKey);
        int hostel, mess;
        CurrentSource(hostel, mess);
        const bool refreshDue =
            covered && g_settings.autoUpdate &&
            (lastSuccessTick == 0 ||
             GetTickCount64() - lastSuccessTick > kRefreshAgeMs) &&
            CachedMonthOlderThan(hostel, mess, MonthKeyFromDayKey(todayKey),
                                 kRefreshAgeMs);
        DWORD waitMs = kIdleIntervalMs;

        if (forced || refreshDue || (!covered && g_settings.autoUpdate)) {
            bool satisfied = PerformFetch();
            if (satisfied) {
                lastSuccessTick = GetTickCount64();
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

        // With automatic checks off, a timed wake could only find nothing to
        // do: every fetch above is gated on the setting except a manual
        // reload, and that arrives on the kick event -- as does the setting
        // being turned back on (Wh_ModSettingsChanged kicks for it). So sleep
        // until one of them, rather than waking four times a day for a no-op.
        // This also keeps a failed manual reload from scheduling the automatic
        // retries the setting says are off.
        if (!g_settings.autoUpdate) {
            waitMs = INFINITE;
        }

        if (g_unloading) {
            break;
        }

        HANDLE handles[2] = {stopEvent, kickEvent};
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
            // Auto-reset event: the wait already cleared it.
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
    std::lock_guard<std::mutex> lock(g_netMutex);
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
    HANDLE thread = nullptr;
    HANDLE stopEvent = nullptr;
    HANDLE kickEvent = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_netMutex);
        thread = g_netThread;
        stopEvent = g_stopEvent;
        kickEvent = g_kickEvent;

        if (stopEvent) {
            SetEvent(stopEvent);
        }

        // Cleared under the lock so KickFetch stops touching them from now on;
        // the handles stay open until after the join below, because the worker
        // is still waiting on them.
        g_netThread = nullptr;
        g_stopEvent = nullptr;
        g_kickEvent = nullptr;
    }

    // Unblock a read that is already in flight.
    if (void* request = g_activeRequest.exchange(nullptr)) {
        WinHttpCloseHandle((HINTERNET)request);
    }

    if (thread) {
        // INFINITE, deliberately, and outside the lock. Windhawk unloads the
        // mod image as soon as Wh_ModUninit returns, so a worker still running
        // mod code after a timed-out wait would be executing unmapped memory.
        // The wait is already bounded by the stop event, the closed WinHTTP
        // handle and the g_unloading checks -- a timeout could only convert a
        // slow exit into a crash. Holding the lock here would deadlock against
        // a KickFetch on the UI thread that the worker is itself waiting on.
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
    }
    if (kickEvent) {
        CloseHandle(kickEvent);
    }
}

// ---------------------------------------------------------------------------
// Section 19: injection retry and the taskbar creation hook
// ---------------------------------------------------------------------------

// Retries until every targeted taskbar has a button. InjectTaskbarButton skips
// the ones already done, so a slow secondary taskbar never costs the primary
// its button -- which a remove-and-retry-everything loop would have made
// visibly flicker for up to five seconds.
static void InjectWithRetry(int generation, int attempt) {
    static constexpr int kMaxAttempts = 50;

    if (g_unloading || generation != g_injectGeneration) {
        return;
    }

    if (InjectTaskbarButton()) {
        StartUiTimer();
        return;
    }

    if (attempt >= kMaxAttempts) {
        Wh_Log(L"InjectWithRetry: gave up waiting for a taskbar's system tray");
        // Whatever did inject still needs its label kept current.
        StartUiTimer();
        return;
    }

    try {
        DispatcherTimer timer;
        timer.Interval(
            winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(100)});
        auto token = std::make_shared<winrt::event_token>();
        *token = timer.Tick(
            [timer, token, generation, attempt](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) mutable {
                try {
                    timer.Stop();
                    timer.Tick(*token);
                    UntrackTimer(timer);
                } catch (...) {
                }
                InjectWithRetry(generation, attempt + 1);
            });
        TrackTimer(timer, *token);
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
        StopUiTimer();
        TearDownFlyout();
        // RemoveTaskbarButton rather than nulling the globals by hand: the
        // button carries SizeChanged and ActualThemeChanged registrations whose
        // code lives in this image, and dropping the reference without revoking
        // them leaks a registration on every taskbar restart. It is safe
        // against the dead tree -- every step is inside its own try/catch.
        RemoveTaskbarButton();

        g_taskbarWnd.store(hWnd);

        // If the taskbar only appeared now, this is where the worker starts.
        StartNetThread();

        InjectWithRetry(g_injectGeneration);
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
        // Optional: only "Show on: Every taskbar" needs these, so a build that
        // lacks them should lose secondary taskbars, not the whole mod.
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable, nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original, nullptr, true},
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
    // No taskbar in this process (a folder window's Explorer) means nothing
    // to do at all -- neither the worker nor the button. The taskbar-creation
    // hook covers the case where the taskbar appears later.
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    g_taskbarWnd.store(hWnd);
    if (!hWnd) {
        return;
    }

    // Only the Explorer instance that owns the taskbar needs the menu.
    StartNetThread();

    RunFromWindowThread(
        hWnd,
        [](void*) {
            try {
                RemoveTaskbarButton();
                // Same retry chain as the taskbar-creation hook, so a
                // transient failure when the mod is enabled mid-session
                // does not leave it buttonless until a setting is touched.
                // InjectWithRetry starts the UI timer once it is done.
                InjectWithRetry(++g_injectGeneration);
            } catch (...) {
                Wh_Log(L"Wh_ModAfterInit: exception during injection");
            }
        },
        nullptr);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    const int oldHostel = g_settings.hostel;
    const int oldMess = g_settings.mess;
    const std::wstring oldUrl = GetMenuUrlTemplate();
    const bool oldAutoUpdate = g_settings.autoUpdate;

    LoadSettings();

    const bool urlChanged = (oldUrl != GetMenuUrlTemplate());
    const bool sourceChanged = urlChanged ||
        (oldHostel != g_settings.hostel) || (oldMess != g_settings.mess);
    // The worker sleeps indefinitely while automatic checks are off, so
    // turning them on has to wake it -- otherwise "Checking periodically for
    // updates…" would be shown with nothing actually scheduled.
    const bool autoUpdateTurnedOn = !oldAutoUpdate && g_settings.autoUpdate;

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        hWnd = g_taskbarWnd.load();
    }

    if (hWnd) {
        g_taskbarWnd.store(hWnd);
        const bool applied = RunFromWindowThread(
            hWnd,
            [](void*) {
                TearDownFlyout();

                try {
                    RemoveTaskbarButton();
                    // InjectWithRetry starts the UI timer when it is done --
                    // idempotently, so this also covers the case where the
                    // first injection failed and the timer was never created.
                    InjectWithRetry(++g_injectGeneration);
                } catch (...) {
                    Wh_Log(L"Wh_ModSettingsChanged: exception during re-inject");
                }
            },
            nullptr);
        if (!applied) {
            // The taskbar thread was not reachable; the UI timer will do it
            // once it is.
            Wh_Log(L"Wh_ModSettingsChanged: taskbar thread busy, deferring");
            g_reapplySettingsPending.store(true);
        }
    }

    if (sourceChanged) {
        // A different hostel/mess is a different file entirely: drop the loaded
        // menu now so the flyout cannot show the old mess's food, then let the
        // worker load that source's cached months (if it has been viewed
        // before, the flyout is populated again at once) and fetch. A
        // different URL invalidates the cache files as well, since they are
        // named by hostel/mess alone.
        {
            std::lock_guard<std::mutex> lock(g_dataMutex);
            g_store.days.clear();
            g_store.hostel = g_settings.hostel;
            g_store.mess = g_settings.mess;
            g_lastFetchError.clear();
            g_storeVersion.fetch_add(1);
        }
        if (urlChanged) {
            g_purgeCacheRequested.store(true);
        }
        g_reloadCacheRequested.store(true);
        KickFetch();
    } else if (autoUpdateTurnedOn) {
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

                TearDownFlyout();

                try {
                    RemoveTaskbarButton();
                } catch (...) {
                    Wh_Log(L"Wh_ModUninit: exception removing the button");
                }

                // The Styles come out of XamlReader::Load on this thread and are
                // XAML DependencyObjects, so they have to be released here
                // rather than on whichever thread runs Wh_ModUninit.
                g_subtleButtonStyle = nullptr;
                g_taskbarButtonStyle = nullptr;
            },
            nullptr,
            /*blockIndefinitely=*/true);

        if (!cleaned) {
            // Only reachable if the window died before the message landed.
            Wh_Log(L"Wh_ModUninit: UI-thread cleanup did not run");
        }
    }

    // Anything still holding a XAML object here would outlive the mod's code.
    g_flyout = nullptr;
    g_flyoutRoot = nullptr;
    g_revealTarget = nullptr;
    g_revealStoryboard = nullptr;
    g_closingStoryboard = nullptr;
    g_flyoutAnchorButton = nullptr;
    g_flyoutTaskbarWnd = nullptr;
    g_timer = nullptr;
    g_subtleButtonStyle = nullptr;
    g_taskbarButtonStyle = nullptr;
    ClearFlyoutRefs();

    // These carry [[clang::no_destroy]], so release their buffers by hand.
    g_liveTimers.reset();
    g_cardCountdowns.reset();
    g_taskbars.reset();
}
