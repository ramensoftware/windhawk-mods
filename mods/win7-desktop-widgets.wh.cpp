// ==WindhawkMod==
// @id              win7-desktop-widgets
// @name            Windows 7 Desktop Widgets
// @description     Authentic Windows 7 Clock & Calendar desktop widgets with 8 skins, options dialog, expandable notebook and multi-monitor support.
// @description:ru  Аутентичные виджеты Часов и Календаря из Windows 7 с 8 скинами, окном настроек, раскрывающимся блокнотом и поддержкой всех мониторов.
// @version         1.0.0
// @author          dekvas
// @github          https://github.com/dekvas
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -lshell32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Screenshot](https://i.imgur.com/u7F6iG9.png) 
![Screenshot](https://i.imgur.com/cQDjOws.png)
![Screenshot](https://i.imgur.com/IdDURoO.png) 

# Windows 7 Desktop Widgets

I really missed the classic Windows 7 desktop widgets and couldn't find a proper, lightweight version anywhere without bloat, broken scaling, or missing multi-monitor support. I love how simple and handy they were, so I made this accurate recreation. Enjoy!

### What's included:
- **Classic Windows 7 Clock**: All 8 original skins, second hand toggle, clock label, time zone picker, and the authentic desktop options window (click the wrench icon on the widget hover bar or right-click -> **Options...**).
- **Classic Windows 7 Calendar**: Spiral-bound notebook with an expandable monthly grid (click the expand icon on the hover bar or double-click the calendar), date picking, and smooth navigation.
- **Clean & Fast**: Each widget runs in its own lightweight window with native vector GDI+ rendering (no external images needed), per-monitor High-DPI scaling (125%/150%+), and full multi-monitor drag support.

---

☕ **Support / Donations:**
If you enjoy this mod and want to support its author:
[Support on Boosty](https://boosty.to/dekvas/single-payment/donation/838774/target?share=target_link)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showClock: true
  $name: Show Clock Widget
  $name:ru: Показывать виджет Часов
  $description: Enable or disable the Windows 7 Clock widget on the desktop.
  $description:ru: Включить или скрыть виджет часов на рабочем столе.

- showCalendar: true
  $name: Show Calendar Widget
  $name:ru: Показывать виджет Календаря
  $description: Enable or disable the Windows 7 Calendar widget on the desktop.
  $description:ru: Включить или скрыть виджет календаря на рабочем столе.

- clockSkin: skin1
  $name: Clock Skin (1 - 8)
  $name:ru: Скин часов (1 - 8)
  $description: Choose one of the 8 classic Windows 7 clock faces (can also be changed via the wrench button on the clock).
  $description:ru: Выберите один из 8 классических циферблатов Windows 7 (также меняется через значок гаечного ключа на часах).
  $options:
  - skin1: 1 - Traditional
  - skin2: 2 - Modern Dark
  - skin3: 3 - Silver & White
  - skin4: 4 - Sky & Pastel
  - skin5: 5 - Ruby Red
  - skin6: 6 - Roman Steel
  - skin7: 7 - Emerald Gold
  - skin8: 8 - Aero Glass
  $options:ru:
  - skin1: 1 - Классический
  - skin2: 2 - Современный тёмный
  - skin3: 3 - Серебристый
  - skin4: 4 - Небесный
  - skin5: 5 - Рубиновый
  - skin6: 6 - Стальной с римскими цифрами
  - skin7: 7 - Изумрудный
  - skin8: 8 - Прозрачное стекло

- showSeconds: true
  $name: Show Second Hand
  $name:ru: Показывать секундную стрелку
  $description: Display the red sweeping second hand on the clock.
  $description:ru: Отображать секундную стрелку на часах.

- clockName: ""
  $name: Clock Name
  $name:ru: Имя часов
  $description: Optional text displayed on the clock face.
  $description:ru: Текст, отображаемый прямо на циферблате часов.

- calendarExpanded: false
  $name: Calendar Expanded (Larger Size)
  $name:ru: Развёрнутый календарь (Крупный размер)
  $description: Show both the top monthly grid sheet and the bottom daily page.
  $description:ru: Показывать верхний лист с сеткой месяца и нижний лист с текущим числом.

- widgetOpacity: 100
  $name: Widget Opacity (%)
  $name:ru: Непрозрачность виджетов (%)
  $description: Opacity from 20% to 100%.
  $description:ru: Прозрачность виджетов от 20% до 100%.

- widgetScale: 100
  $name: Widget Scale (%)
  $name:ru: Масштаб виджетов (%)
  $description: Size scale in percent (e.g. 100, 125, 150).
  $description:ru: Масштаб размера в процентах (например 100, 125, 150).

- language: auto
  $name: Language
  $name:ru: Язык интерфейса
  $options:
  - auto: Auto (System Default)
  - en: English
  - ru: Russian
  $options:ru:
  - auto: Авто (По умолчанию системы)
  - en: Английский (English)
  - ru: Русский
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <shellapi.h>

#include <algorithm>
#include <cmath>
#include <cwchar>

using namespace Gdiplus;

namespace {

constexpr UINT_PTR kTickTimer = 1;
constexpr UINT kSettingsChangedMessage = WM_APP + 1;
constexpr wchar_t kClockClassName[] = L"WindhawkWin7ClockWidget";
constexpr wchar_t kCalendarClassName[] = L"WindhawkWin7CalendarWidget";
constexpr wchar_t kOptionsClassName[] = L"WindhawkWin7ClockOptionsDlg";

constexpr float kPi = 3.14159265358979323846f;

enum WidgetId {
    WidgetClock = 0,
    WidgetCalendar = 1,
    WidgetCount = 2
};

enum HoverButtonId {
    HoverBtnNone = 0,
    HoverBtnClose = 1,
    HoverBtnSizeOrOpts = 2,
    HoverBtnDrag = 3,
    CalBtnPrevMonth = 4,
    CalBtnNextMonth = 5,
    CalBtnDayCell = 6
};

struct TimeZoneEntry {
    int biasMinutes; // Offset from UTC in minutes (positive = East of UTC, or special Local)
    bool isLocal;
    const wchar_t* nameRu;
    const wchar_t* nameEn;
};

const TimeZoneEntry kTimeZones[] = {
    {0,    true,  L"Текущее время на компьютере",          L"Current computer time"},
    {-720, false, L"(UTC-12:00) Линия перемены дат",       L"(UTC-12:00) International Date Line West"},
    {-600, false, L"(UTC-10:00) Гавайи",                   L"(UTC-10:00) Hawaii"},
    {-480, false, L"(UTC-08:00) Тихоокеанское время",      L"(UTC-08:00) Pacific Time (US & Canada)"},
    {-420, false, L"(UTC-07:00) Горное время",             L"(UTC-07:00) Mountain Time (US & Canada)"},
    {-360, false, L"(UTC-06:00) Центральное время",        L"(UTC-06:00) Central Time (US & Canada)"},
    {-300, false, L"(UTC-05:00) Восточное время",          L"(UTC-05:00) Eastern Time (US & Canada)"},
    {-180, false, L"(UTC-03:00) Бразилиа, Буэнос-Айрес",   L"(UTC-03:00) Brasilia, Buenos Aires"},
    {0,    false, L"(UTC+00:00) Лондон, Дублин, Лиссабон", L"(UTC+00:00) London, Dublin, Lisbon"},
    {60,   false, L"(UTC+01:00) Берлин, Париж, Рим",       L"(UTC+01:00) Berlin, Paris, Rome"},
    {120,  false, L"(UTC+02:00) Калининград, Киев, Афины", L"(UTC+02:00) Kaliningrad, Kyiv, Athens"},
    {180,  false, L"(UTC+03:00) Москва, Санкт-Петербург",  L"(UTC+03:00) Moscow, St. Petersburg"},
    {240,  false, L"(UTC+04:00) Самара, Тбилиси, Дубай",   L"(UTC+04:00) Samara, Tbilisi, Dubai"},
    {300,  false, L"(UTC+05:00) Екатеринбург, Ташкент",    L"(UTC+05:00) Yekaterinburg, Tashkent"},
    {360,  false, L"(UTC+06:00) Омск, Алматы, Астана",     L"(UTC+06:00) Omsk, Almaty, Astana"},
    {420,  false, L"(UTC+07:00) Новосибирск, Красноярск",  L"(UTC+07:00) Novosibirsk, Krasnoyarsk"},
    {480,  false, L"(UTC+08:00) Иркутск, Пекин, Сингапур", L"(UTC+08:00) Irkutsk, Beijing, Singapore"},
    {540,  false, L"(UTC+09:00) Якутск, Токио, Сеул",      L"(UTC+09:00) Yakutsk, Tokyo, Seoul"},
    {600,  false, L"(UTC+10:00) Владивосток, Сидней",      L"(UTC+10:00) Vladivostok, Sydney"},
    {660,  false, L"(UTC+11:00) Магадан, Сахалин",         L"(UTC+11:00) Magadan, Sakhalin"},
    {720,  false, L"(UTC+12:00) Камчатка, Окленд",         L"(UTC+12:00) Kamchatka, Auckland"},
};

// ── Global State ─────────────────────────────────────────────────────────────

HWND g_clockHwnd = nullptr;
HWND g_calendarHwnd = nullptr;
HWND g_optionsHwnd = nullptr;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
volatile bool g_unloading = false;
ULONG_PTR g_gdiplusToken = 0;

bool g_isRussian = false;
float g_userScale = 1.0f;
float g_clockScale = 1.0f;
float g_calendarScale = 1.0f;
BYTE g_masterOpacity = 255;

// Clock state
bool g_showClock = true;
int g_clockSkin = 0; // 0..7
bool g_showSeconds = true;
wchar_t g_clockName[64] = L"";
int g_clockTzIndex = 0;
PointF g_clockPos = PointF(0, 0); // In virtual screen coordinates

// Calendar state
bool g_showCalendar = true;
bool g_calendarExpanded = false;
PointF g_calendarPos = PointF(0, 0);
int g_calViewYear = 0;
int g_calViewMonth = 0; // 1..12
int g_calSelectedDay = 0;
int g_calSelectedMonth = 0;
int g_calSelectedYear = 0;
int g_lastKnownTodayYear = 0;
int g_lastKnownTodayMonth = 0;
int g_lastKnownTodayDay = 0;
int g_lastRenderedMinute = -1;

// Interaction state
int g_hoveredWidget = -1;
int g_hoveredBtn = HoverBtnNone;
bool g_trackingMouse[WidgetCount] = {false, false};
int g_dragWidget = -1;
PointF g_dragOffset = PointF(0, 0);
PointF g_dragStartScreen = PointF(0, 0);
bool g_dragMoved = false;
bool g_positionsInitialized = false;

struct RenderSurface {
    HDC memDc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    int width = 0;
    int height = 0;
};

RenderSurface g_surfaces[WidgetCount];

// ── Localization ─────────────────────────────────────────────────────────────

void DetectLanguage() {
    PCWSTR langSetting = Wh_GetStringSetting(L"language");
    if (langSetting) {
        if (wcscmp(langSetting, L"ru") == 0) {
            g_isRussian = true;
            Wh_FreeStringSetting(langSetting);
            return;
        }
        if (wcscmp(langSetting, L"en") == 0) {
            g_isRussian = false;
            Wh_FreeStringSetting(langSetting);
            return;
        }
        Wh_FreeStringSetting(langSetting);
    }
    LANGID langId = GetUserDefaultUILanguage();
    WORD primary = PRIMARYLANGID(langId);
    g_isRussian = (primary == LANG_RUSSIAN);
}

const wchar_t* MonthNameFull(int month) {
    static const wchar_t* kRu[] = {
        L"Январь", L"Февраль", L"Март", L"Апрель", L"Май", L"Июнь",
        L"Июль", L"Август", L"Сентябрь", L"Октябрь", L"Ноябрь", L"Декабрь"
    };
    static const wchar_t* kEn[] = {
        L"January", L"February", L"March", L"April", L"May", L"June",
        L"July", L"August", L"September", L"October", L"November", L"December"
    };
    int idx = std::max(1, std::min(12, month)) - 1;
    return g_isRussian ? kRu[idx] : kEn[idx];
}

const wchar_t* MonthNameGenitiveOrFull(int month) {
    // On the Russian screenshot of Win7 Calendar: "Октябрь 2013" (Nominative!)
    return MonthNameFull(month);
}

const wchar_t* MonthNameShort(int month) {
    // Notice on the Russian screenshot: lowercase "июн 10", on English: "Sep 08"
    static const wchar_t* kRu[] = {
        L"янв", L"фев", L"мар", L"апр", L"май", L"июн",
        L"июл", L"авг", L"сен", L"окт", L"ноя", L"дек"
    };
    static const wchar_t* kEn[] = {
        L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
        L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"
    };
    int idx = std::max(1, std::min(12, month)) - 1;
    return g_isRussian ? kRu[idx] : kEn[idx];
}

const wchar_t* DayOfWeekName(int dowSundayZero) {
    // Notice on the Russian screenshot: lowercase "среда", on English: "Friday"
    static const wchar_t* kRu[] = {
        L"воскресенье", L"понедельник", L"вторник", L"среда",
        L"четверг", L"пятница", L"суббота"
    };
    static const wchar_t* kEn[] = {
        L"Sunday", L"Monday", L"Tuesday", L"Wednesday",
        L"Thursday", L"Friday", L"Saturday"
    };
    int idx = ((dowSundayZero % 7) + 7) % 7;
    return g_isRussian ? kRu[idx] : kEn[idx];
}

// ── DPI Scaling & Virtual Screen Helpers ─────────────────────────────────────

float GetMonitorDpiScaleForPoint(POINT pt, HWND hwnd = nullptr) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hwnd && IsWindow(hwnd) && hUser32) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto pGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
            GetProcAddress(hUser32, "GetDpiForWindow"));
        if (pGetDpiForWindow) {
            UINT dpi = pGetDpiForWindow(hwnd);
            if (dpi > 0) return dpi / 96.0f;
        }
    }
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (hMon) {
        HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
        if (!hShcore) hShcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hShcore) {
            using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
            auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
                GetProcAddress(hShcore, "GetDpiForMonitor"));
            if (pGetDpiForMonitor) {
                UINT dpiX = 96, dpiY = 96;
                if (SUCCEEDED(pGetDpiForMonitor(hMon, 0 /*MDT_EFFECTIVE_DPI*/, &dpiX, &dpiY)) && dpiX > 0) {
                    return dpiX / 96.0f;
                }
            }
        }
    }
    HDC screenDc = GetDC(nullptr);
    int dpiX = 96;
    if (screenDc) {
        dpiX = GetDeviceCaps(screenDc, LOGPIXELSX);
        ReleaseDC(nullptr, screenDc);
    }
    return (dpiX > 0) ? (dpiX / 96.0f) : 1.0f;
}

float GetWidgetScale(int widgetId) {
    return (widgetId == WidgetClock) ? g_clockScale : g_calendarScale;
}

void UpdateEffectiveScale() {
    POINT clockPt = {
        static_cast<LONG>(g_clockPos.X + 65.0f),
        static_cast<LONG>(g_clockPos.Y + 65.0f)
    };
    g_clockScale = g_userScale * GetMonitorDpiScaleForPoint(clockPt, g_clockHwnd);

    POINT calPt = {
        static_cast<LONG>(g_calendarPos.X + 65.0f),
        static_cast<LONG>(g_calendarPos.Y + 65.0f)
    };
    g_calendarScale = g_userScale * GetMonitorDpiScaleForPoint(calPt, g_calendarHwnd);
}

RECT GetFullVirtualScreenRect() {
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (w <= 0 || h <= 0) {
        w = GetSystemMetrics(SM_CXSCREEN);
        h = GetSystemMetrics(SM_CYSCREEN);
        x = 0;
        y = 0;
    }
    return {x, y, x + w, y + h};
}

float ClampF(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

bool PtInRectF(const RectF& rc, PointF pt) {
    return pt.X >= rc.X && pt.X <= rc.X + rc.Width &&
           pt.Y >= rc.Y && pt.Y <= rc.Y + rc.Height;
}

// Logical dimensions of widgets (at 100% scale):
// Clock: 130 x 130
// Calendar Collapsed: 130 x 146
// Calendar Expanded:  130 x 290
// Hover bar width: 20px on the right edge of each widget

SizeF GetWidgetWindowSize(int widgetId) {
    const float s = GetWidgetScale(widgetId);
    if (widgetId == WidgetClock) {
        return SizeF(std::ceil(154.0f * s), std::ceil(132.0f * s));
    } else {
        float h = g_calendarExpanded ? 292.0f : 148.0f;
        return SizeF(std::ceil(154.0f * s), std::ceil(h * s));
    }
}

RectF GetWidgetLocalBodyRect(int widgetId) {
    const float s = GetWidgetScale(widgetId);
    if (widgetId == WidgetClock) {
        return RectF(0.0f, 0.0f, 130.0f * s, 130.0f * s);
    } else {
        float h = g_calendarExpanded ? 290.0f : 146.0f;
        return RectF(0.0f, 0.0f, 130.0f * s, h * s);
    }
}

RectF GetWidgetLocalHoverBarRect(int widgetId) {
    const float s = GetWidgetScale(widgetId);
    RectF body = GetWidgetLocalBodyRect(widgetId);
    return RectF(body.X + body.Width + 1.0f * s, body.Y + 4.0f * s, 20.0f * s, 56.0f * s);
}

RectF GetWidgetScreenBodyRect(int widgetId) {
    RectF local = GetWidgetLocalBodyRect(widgetId);
    PointF pos = (widgetId == WidgetClock) ? g_clockPos : g_calendarPos;
    return RectF(pos.X, pos.Y, local.Width, local.Height);
}

void ClampWidgetToVirtualScreen(int widgetId) {
    RECT vr = GetFullVirtualScreenRect();
    RectF body = GetWidgetLocalBodyRect(widgetId);
    float minX = static_cast<float>(vr.left);
    float minY = static_cast<float>(vr.top);
    float maxX = std::max(minX, static_cast<float>(vr.right) - body.Width);
    float maxY = std::max(minY, static_cast<float>(vr.bottom) - body.Height);

    if (widgetId == WidgetClock) {
        g_clockPos.X = ClampF(g_clockPos.X, minX, maxX);
        g_clockPos.Y = ClampF(g_clockPos.Y, minY, maxY);
    } else {
        g_calendarPos.X = ClampF(g_calendarPos.X, minX, maxX);
        g_calendarPos.Y = ClampF(g_calendarPos.Y, minY, maxY);
    }
}

// ── Time & Date Calculations ─────────────────────────────────────────────────

SYSTEMTIME GetClockTimeForTz(int tzIndex) {
    if (tzIndex <= 0 || tzIndex >= static_cast<int>(ARRAYSIZE(kTimeZones)) ||
        kTimeZones[tzIndex].isLocal) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        return st;
    }
    SYSTEMTIME utc;
    GetSystemTime(&utc);
    FILETIME ft;
    SystemTimeToFileTime(&utc, &ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    LONGLONG offsetTicks = static_cast<LONGLONG>(kTimeZones[tzIndex].biasMinutes) * 60LL * 10000000LL;
    uli.QuadPart += offsetTicks;
    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;
    SYSTEMTIME target;
    FileTimeToSystemTime(&ft, &target);
    return target;
}

SYSTEMTIME GetClockTime() {
    return GetClockTimeForTz(g_clockTzIndex);
}

int DaysInMonth(int year, int month) {
    static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) return 30;
    if (month == 2) {
        bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        return leap ? 29 : 28;
    }
    return kDays[month - 1];
}

int DayOfWeekForDate(int year, int month, int day) {
    SYSTEMTIME st = {};
    st.wYear = static_cast<WORD>(year);
    st.wMonth = static_cast<WORD>(month);
    st.wDay = static_cast<WORD>(day);
    FILETIME ft = {};
    SystemTimeToFileTime(&st, &ft);
    FileTimeToSystemTime(&ft, &st);
    return st.wDayOfWeek; // 0 = Sunday .. 6 = Saturday
}

void SyncCalendarWithTodayIfUnset() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    if (g_lastKnownTodayYear == 0) {
        g_lastKnownTodayYear = st.wYear;
        g_lastKnownTodayMonth = st.wMonth;
        g_lastKnownTodayDay = st.wDay;
    }
    if (g_calViewYear == 0 || g_calViewMonth == 0) {
        g_calViewYear = st.wYear;
        g_calViewMonth = st.wMonth;
    }
    if (g_calSelectedYear == 0) {
        g_calSelectedYear = st.wYear;
        g_calSelectedMonth = st.wMonth;
        g_calSelectedDay = st.wDay;
    }
}

bool CheckAndAdvanceMidnightRollover() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    if (g_lastKnownTodayYear == 0) {
        SyncCalendarWithTodayIfUnset();
        return true;
    }
    if (st.wYear != g_lastKnownTodayYear ||
        st.wMonth != g_lastKnownTodayMonth ||
        st.wDay != g_lastKnownTodayDay) {
        // If user had the previous "today" selected, automatically advance to the new day
        if (g_calSelectedYear == g_lastKnownTodayYear &&
            g_calSelectedMonth == g_lastKnownTodayMonth &&
            g_calSelectedDay == g_lastKnownTodayDay) {
            g_calSelectedYear = st.wYear;
            g_calSelectedMonth = st.wMonth;
            g_calSelectedDay = st.wDay;
        }
        if (g_calViewYear == g_lastKnownTodayYear &&
            g_calViewMonth == g_lastKnownTodayMonth) {
            g_calViewYear = st.wYear;
            g_calViewMonth = st.wMonth;
        }
        g_lastKnownTodayYear = st.wYear;
        g_lastKnownTodayMonth = st.wMonth;
        g_lastKnownTodayDay = st.wDay;
        return true;
    }
    return false;
}

// ── Persistence & Settings ───────────────────────────────────────────────────

void SaveStateToStorage() {
    wchar_t buf[128];
    swprintf_s(buf, L"%.1f,%.1f", g_clockPos.X, g_clockPos.Y);
    Wh_SetStringValue(L"clockPos", buf);

    swprintf_s(buf, L"%.1f,%.1f", g_calendarPos.X, g_calendarPos.Y);
    Wh_SetStringValue(L"calendarPos", buf);

    Wh_SetIntValue(L"savedClockSkin", g_clockSkin);
    Wh_SetIntValue(L"savedShowSeconds", g_showSeconds ? 1 : 0);
    Wh_SetIntValue(L"savedClockTz", g_clockTzIndex);
    Wh_SetStringValue(L"savedClockName", g_clockName);
    Wh_SetIntValue(L"savedCalExpanded", g_calendarExpanded ? 1 : 0);
    Wh_SetIntValue(L"savedShowClock", g_showClock ? 1 : 0);
    Wh_SetIntValue(L"savedShowCalendar", g_showCalendar ? 1 : 0);
}

void ApplyDefaultPositions() {
    RECT workArea = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    POINT initPt = {workArea.right - 100, workArea.top + 100};
    const float s = g_userScale * GetMonitorDpiScaleForPoint(initPt);
    float rightEdge = static_cast<float>(workArea.right) - 155.0f * s;
    float topEdge = static_cast<float>(workArea.top) + 20.0f * s;

    g_clockPos = PointF(rightEdge, topEdge);
    g_calendarPos = PointF(rightEdge, topEdge + 145.0f * s);
}

void LoadStateAndSettings(bool fromWindhawkSettingsChange) {
    DetectLanguage();

    int scalePct = Wh_GetIntSetting(L"widgetScale");
    if (scalePct < 50 || scalePct > 300) scalePct = 100;
    g_userScale = scalePct / 100.0f;

    int opacityPct = Wh_GetIntSetting(L"widgetOpacity");
    if (opacityPct < 20 || opacityPct > 100) opacityPct = 100;
    g_masterOpacity = static_cast<BYTE>(std::round((opacityPct / 100.0f) * 255.0f));

    bool cfgShowClock = Wh_GetIntSetting(L"showClock") != 0;
    bool cfgShowCalendar = Wh_GetIntSetting(L"showCalendar") != 0;

    int cfgSkin = 0;
    PCWSTR skinSetting = Wh_GetStringSetting(L"clockSkin");
    if (skinSetting) {
        if (wcsncmp(skinSetting, L"skin", 4) == 0 && skinSetting[4] >= L'1' && skinSetting[4] <= L'8') {
            cfgSkin = skinSetting[4] - L'1';
        }
        Wh_FreeStringSetting(skinSetting);
    }

    bool cfgShowSeconds = Wh_GetIntSetting(L"showSeconds") != 0;
    bool cfgCalExpanded = Wh_GetIntSetting(L"calendarExpanded") != 0;

    wchar_t cfgClockName[64] = L"";
    PCWSTR nameSetting = Wh_GetStringSetting(L"clockName");
    if (nameSetting) {
        wcsncpy_s(cfgClockName, nameSetting, _TRUNCATE);
        Wh_FreeStringSetting(nameSetting);
    }

    // Compare against the last known Windhawk settings so widget-side changes
    // survive unrelated edits in the Windhawk settings UI.
    int prevCfgShowClock = Wh_GetIntValue(L"prevCfgShowClock", -1);
    int prevCfgShowCal   = Wh_GetIntValue(L"prevCfgShowCalendar", -1);
    int prevCfgSkin      = Wh_GetIntValue(L"prevCfgClockSkin", -1);
    int prevCfgSec       = Wh_GetIntValue(L"prevCfgShowSeconds", -1);
    int prevCfgExp       = Wh_GetIntValue(L"prevCfgCalExpanded", -1);
    wchar_t prevCfgName[64] = L"";
    bool hasPrevCfgName  = Wh_GetStringValue(L"prevCfgClockName", prevCfgName, ARRAYSIZE(prevCfgName)) > 0 ||
                           Wh_GetIntValue(L"prevCfgNameInit", 0) != 0;

    // Load saved runtime state first
    int savedShowClock = Wh_GetIntValue(L"savedShowClock", -1);
    g_showClock = (savedShowClock != -1) ? (savedShowClock != 0) : cfgShowClock;

    int savedShowCal = Wh_GetIntValue(L"savedShowCalendar", -1);
    g_showCalendar = (savedShowCal != -1) ? (savedShowCal != 0) : cfgShowCalendar;

    int savedSkin = Wh_GetIntValue(L"savedClockSkin", -1);
    g_clockSkin = (savedSkin >= 0 && savedSkin <= 7) ? savedSkin : cfgSkin;

    int savedSec = Wh_GetIntValue(L"savedShowSeconds", -1);
    g_showSeconds = (savedSec != -1) ? (savedSec != 0) : cfgShowSeconds;

    int savedTz = Wh_GetIntValue(L"savedClockTz", -1);
    if (savedTz >= 0 && savedTz < static_cast<int>(ARRAYSIZE(kTimeZones))) {
        g_clockTzIndex = savedTz;
    }

    wchar_t savedName[64] = {};
    if (Wh_GetStringValue(L"savedClockName", savedName, ARRAYSIZE(savedName)) > 0 ||
        Wh_GetIntValue(L"savedClockNameInit", 0) != 0) {
        wcsncpy_s(g_clockName, savedName, _TRUNCATE);
    } else {
        wcsncpy_s(g_clockName, cfgClockName, _TRUNCATE);
    }

    int savedExp = Wh_GetIntValue(L"savedCalExpanded", -1);
    g_calendarExpanded = (savedExp != -1) ? (savedExp != 0) : cfgCalExpanded;

    // Apply any Windhawk setting whose value changed since the last load,
    // including changes made while the mod wasn't running.
    if (prevCfgShowClock == -1 || prevCfgShowClock != (cfgShowClock ? 1 : 0)) {
        g_showClock = cfgShowClock;
    }
    if (prevCfgShowCal == -1 || prevCfgShowCal != (cfgShowCalendar ? 1 : 0)) {
        g_showCalendar = cfgShowCalendar;
    }
    if (prevCfgSkin == -1 || prevCfgSkin != cfgSkin) {
        g_clockSkin = cfgSkin;
    }
    if (prevCfgSec == -1 || prevCfgSec != (cfgShowSeconds ? 1 : 0)) {
        g_showSeconds = cfgShowSeconds;
    }
    if (!hasPrevCfgName || wcscmp(prevCfgName, cfgClockName) != 0) {
        wcsncpy_s(g_clockName, cfgClockName, _TRUNCATE);
    }
    if (prevCfgExp == -1 || prevCfgExp != (cfgCalExpanded ? 1 : 0)) {
        g_calendarExpanded = cfgCalExpanded;
    }

    if (!fromWindhawkSettingsChange) {
        // If both widgets were closed in the previous session, restore from settings
        // so the user isn't left with zero visible widgets on startup.
        if (!g_showClock && !g_showCalendar && (cfgShowClock || cfgShowCalendar)) {
            g_showClock = cfgShowClock;
            g_showCalendar = cfgShowCalendar;
        }
    }

    // Record current Windhawk settings snapshot & runtime state
    Wh_SetIntValue(L"prevCfgShowClock", cfgShowClock ? 1 : 0);
    Wh_SetIntValue(L"prevCfgShowCalendar", cfgShowCalendar ? 1 : 0);
    Wh_SetIntValue(L"prevCfgClockSkin", cfgSkin);
    Wh_SetIntValue(L"prevCfgShowSeconds", cfgShowSeconds ? 1 : 0);
    Wh_SetIntValue(L"prevCfgCalExpanded", cfgCalExpanded ? 1 : 0);
    Wh_SetStringValue(L"prevCfgClockName", cfgClockName);
    Wh_SetIntValue(L"prevCfgNameInit", 1);
    Wh_SetIntValue(L"savedClockNameInit", 1);

    if (!g_positionsInitialized) {
        ApplyDefaultPositions();
        wchar_t buf[64];
        float x = 0, y = 0;
        if (Wh_GetStringValue(L"clockPos", buf, ARRAYSIZE(buf)) > 0 &&
            swscanf_s(buf, L"%f,%f", &x, &y) == 2) {
            g_clockPos = PointF(x, y);
        }
        if (Wh_GetStringValue(L"calendarPos", buf, ARRAYSIZE(buf)) > 0 &&
            swscanf_s(buf, L"%f,%f", &x, &y) == 2) {
            g_calendarPos = PointF(x, y);
        }
        g_positionsInitialized = true;
    }

    UpdateEffectiveScale();
    ClampWidgetToVirtualScreen(WidgetClock);
    ClampWidgetToVirtualScreen(WidgetCalendar);
    SyncCalendarWithTodayIfUnset();
    SaveStateToStorage();
}

// ── GDI+ Drawing Primitives ──────────────────────────────────────────────────

void AddRoundedRectPath(GraphicsPath& path, const RectF& rc, float r) {
    if (r <= 0.0f) {
        path.AddRectangle(rc);
        return;
    }
    float d = std::min({r * 2.0f, rc.Width, rc.Height});
    path.AddArc(rc.X, rc.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rc.X + rc.Width - d, rc.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rc.X + rc.Width - d, rc.Y + rc.Height - d, d, d, 0.0f, 90.0f);
    path.AddArc(rc.X, rc.Y + rc.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

void FillRoundRect(Graphics& g, const RectF& rc, float r, const Brush& brush) {
    GraphicsPath path;
    AddRoundedRectPath(path, rc, r);
    g.FillPath(&brush, &path);
}

void DrawRoundRect(Graphics& g, const RectF& rc, float r, const Pen& pen) {
    GraphicsPath path;
    AddRoundedRectPath(path, rc, r);
    g.DrawPath(&pen, &path);
}

// ── Windows 7 Hover Control Bar (Close [X], Larger/Options, Drag Dots) ───────

void DrawWin7HoverToolbar(Graphics& g, int widgetId) {
    if (g_hoveredWidget != widgetId || g_dragWidget >= 0) return;

    const float s = GetWidgetScale(widgetId);
    RectF bar = GetWidgetLocalHoverBarRect(widgetId);

    // Outer dark translucent glass pill
    LinearGradientBrush bgBrush(
        PointF(bar.X, bar.Y), PointF(bar.X + bar.Width, bar.Y),
        Color(210, 35, 42, 52), Color(230, 15, 20, 28));
    FillRoundRect(g, bar, 3.5f * s, bgBrush);

    Pen outerBorder(Color(210, 10, 14, 20), 1.0f * s);
    DrawRoundRect(g, bar, 3.5f * s, outerBorder);

    RectF innerBorderRc(bar.X + 1.0f * s, bar.Y + 1.0f * s, bar.Width - 2.0f * s, bar.Height - 2.0f * s);
    Pen innerGlass(Color(95, 255, 255, 255), 1.0f * s);
    DrawRoundRect(g, innerBorderRc, 2.5f * s, innerGlass);

    // 3 zones:
    // Top: Close [X] (height 18)
    // Middle: Options (Clock) or Larger/Smaller (Calendar) (height 18)
    // Bottom: Drag grip dots (height 20)
    RectF btnCloseRc(bar.X + 2.0f * s, bar.Y + 2.0f * s, bar.Width - 4.0f * s, 17.0f * s);
    RectF btnMidRc(bar.X + 2.0f * s, bar.Y + 20.0f * s, bar.Width - 4.0f * s, 17.0f * s);
    RectF gripRc(bar.X + 2.0f * s, bar.Y + 38.0f * s, bar.Width - 4.0f * s, 16.0f * s);

    // Dividers
    Pen divDark(Color(180, 12, 16, 22), 1.0f * s);
    Pen divLight(Color(65, 255, 255, 255), 1.0f * s);
    g.DrawLine(&divDark, bar.X + 2.0f * s, bar.Y + 19.5f * s, bar.X + bar.Width - 2.0f * s, bar.Y + 19.5f * s);
    g.DrawLine(&divLight, bar.X + 2.0f * s, bar.Y + 20.5f * s, bar.X + bar.Width - 2.0f * s, bar.Y + 20.5f * s);
    g.DrawLine(&divDark, bar.X + 2.0f * s, bar.Y + 37.5f * s, bar.X + bar.Width - 2.0f * s, bar.Y + 37.5f * s);
    g.DrawLine(&divLight, bar.X + 2.0f * s, bar.Y + 38.5f * s, bar.X + bar.Width - 2.0f * s, bar.Y + 38.5f * s);

    // Hover highlights
    if (g_hoveredBtn == HoverBtnClose) {
        LinearGradientBrush redGlow(
            PointF(btnCloseRc.X, btnCloseRc.Y),
            PointF(btnCloseRc.X, btnCloseRc.Y + btnCloseRc.Height),
            Color(235, 235, 115, 105), Color(235, 185, 30, 25));
        FillRoundRect(g, btnCloseRc, 2.0f * s, redGlow);
        Pen redRim(Color(220, 255, 180, 170), 1.0f * s);
        DrawRoundRect(g, btnCloseRc, 2.0f * s, redRim);
    } else if (g_hoveredBtn == HoverBtnSizeOrOpts) {
        LinearGradientBrush blueGlow(
            PointF(btnMidRc.X, btnMidRc.Y),
            PointF(btnMidRc.X, btnMidRc.Y + btnMidRc.Height),
            Color(235, 110, 195, 250), Color(235, 25, 115, 200));
        FillRoundRect(g, btnMidRc, 2.0f * s, blueGlow);
        Pen blueRim(Color(220, 180, 230, 255), 1.0f * s);
        DrawRoundRect(g, btnMidRc, 2.0f * s, blueRim);
    }

    // Draw [X] icon
    {
        float cx = btnCloseRc.X + btnCloseRc.Width * 0.5f;
        float cy = btnCloseRc.Y + btnCloseRc.Height * 0.5f;
        float d = 3.6f * s;
        Pen shadowPen(Color(160, 0, 0, 0), 2.2f * s);
        shadowPen.SetStartCap(LineCapRound);
        shadowPen.SetEndCap(LineCapRound);
        g.DrawLine(&shadowPen, cx - d, cy - d + 0.8f * s, cx + d, cy + d + 0.8f * s);
        g.DrawLine(&shadowPen, cx + d, cy - d + 0.8f * s, cx - d, cy + d + 0.8f * s);

        Pen xPen(Color(245, 245, 248, 255), 1.8f * s);
        xPen.SetStartCap(LineCapRound);
        xPen.SetEndCap(LineCapRound);
        g.DrawLine(&xPen, cx - d, cy - d, cx + d, cy + d);
        g.DrawLine(&xPen, cx + d, cy - d, cx - d, cy + d);
    }

    // Draw Middle Button: Wrench (Clock) or Maximize/Restore box (Calendar)
    {
        float cx = btnMidRc.X + btnMidRc.Width * 0.5f;
        float cy = btnMidRc.Y + btnMidRc.Height * 0.5f;
        if (widgetId == WidgetClock) {
            // Wrench icon
            Pen wrenchShadow(Color(150, 0, 0, 0), 2.2f * s);
            wrenchShadow.SetStartCap(LineCapRound);
            wrenchShadow.SetEndCap(LineCapRound);
            g.DrawLine(&wrenchShadow, cx - 3.5f * s, cy + 3.5f * s, cx + 2.0f * s, cy - 2.0f * s);

            Pen wrenchPen(Color(245, 240, 246, 255), 1.8f * s);
            wrenchPen.SetStartCap(LineCapRound);
            wrenchPen.SetEndCap(LineCapRound);
            g.DrawLine(&wrenchPen, cx - 3.5f * s, cy + 3.5f * s, cx + 1.8f * s, cy - 1.8f * s);
            g.DrawArc(&wrenchPen, cx - 0.5f * s, cy - 5.0f * s, 5.2f * s, 5.2f * s, 200.0f, 250.0f);
        } else {
            // Larger / Smaller size icon (exact match to user's screenshot #1!)
            RectF boxRc(cx - 5.0f * s, cy - 4.5f * s, 10.0f * s, 9.0f * s);
            Pen boxPen(Color(245, 240, 248, 255), 1.3f * s);
            g.DrawRectangle(&boxPen, boxRc.X, boxRc.Y, boxRc.Width, boxRc.Height);

            Pen arrowPen(Color(255, 255, 255, 255), 1.4f * s);
            if (!g_calendarExpanded) {
                // Diagonal arrow pointing top-right (Larger size)
                g.DrawLine(&arrowPen, cx - 2.5f * s, cy + 2.0f * s, cx + 2.5f * s, cy - 2.0f * s);
                g.DrawLine(&arrowPen, cx + 2.5f * s, cy - 2.0f * s, cx - 0.5f * s, cy - 2.0f * s);
                g.DrawLine(&arrowPen, cx + 2.5f * s, cy - 2.0f * s, cx + 2.5f * s, cy + 1.0f * s);
            } else {
                // Diagonal arrow pointing bottom-left (Smaller size)
                g.DrawLine(&arrowPen, cx + 2.5f * s, cy - 2.0f * s, cx - 2.5f * s, cy + 2.0f * s);
                g.DrawLine(&arrowPen, cx - 2.5f * s, cy + 2.0f * s, cx + 0.5f * s, cy + 2.0f * s);
                g.DrawLine(&arrowPen, cx - 2.5f * s, cy + 2.0f * s, cx - 2.5f * s, cy - 1.0f * s);
            }
        }
    }

    // Draw Drag Grip Dots (3x4 grid of tiny embossed dots)
    {
        SolidBrush dotLight(Color(200, 210, 220, 235));
        SolidBrush dotDark(Color(160, 10, 14, 20));
        float startX = gripRc.X + (gripRc.Width - 8.0f * s) * 0.5f;
        float startY = gripRc.Y + 3.0f * s;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 3; ++col) {
                float dx = startX + col * 3.5f * s;
                float dy = startY + row * 3.0f * s;
                g.FillRectangle(&dotDark, dx + 0.6f * s, dy + 0.6f * s, 1.5f * s, 1.5f * s);
                g.FillRectangle(&dotLight, dx, dy, 1.5f * s, 1.5f * s);
            }
        }
    }
}

// ── Authentic Windows 7 Clock Renderer (All 8 Skins) ─────────────────────────

void RenderClockFaceAt(Graphics& g, float cx, float cy, float radius, int skinIndex,
                       const SYSTEMTIME& st, bool showSeconds, const wchar_t* customName,
                       bool drawDropShadow) {
    // 1. Soft drop shadow under the clock
    if (drawDropShadow) {
        for (int i = 5; i >= 1; --i) {
            float expand = i * (radius * 0.028f);
            float offsetY = i * (radius * 0.022f);
            SolidBrush shadowBrush(Color(static_cast<BYTE>(14 * (6 - i)), 0, 0, 0));
            g.FillEllipse(&shadowBrush,
                          cx - radius - expand,
                          cy - radius - expand + offsetY,
                          (radius + expand) * 2.0f,
                          (radius + expand) * 2.0f);
        }
    }

    int skin = ((skinIndex % 8) + 8) % 8;

    // Outer Bezel / Case (in original Win7 Traditional clock, the bezel is sleek and thin: 88.5% face)
    float faceRadius = radius * 0.885f;

    switch (skin) {
        case 0: { // 1: Traditional (Exact Win7 default clock from screenshots!)
            // Dark charcoal metallic outer bezel
            LinearGradientBrush outerRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(255, 88, 90, 94), Color(255, 18, 19, 21));
            g.FillEllipse(&outerRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            float rMid = radius * 0.945f;
            LinearGradientBrush innerRim(
                PointF(cx, cy - rMid), PointF(cx, cy + rMid),
                Color(255, 22, 23, 26), Color(255, 58, 60, 64));
            g.FillEllipse(&innerRim, cx - rMid, cy - rMid, rMid * 2.0f, rMid * 2.0f);

            // Crisp outer edge ring
            Pen darkEdge(Color(235, 12, 13, 15), radius * 0.016f);
            g.DrawEllipse(&darkEdge, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            // Subtle top-left bevel highlight on the black frame
            Pen topBevel(Color(115, 190, 195, 205), radius * 0.018f);
            g.DrawArc(&topBevel, cx - radius * 0.965f, cy - radius * 0.965f,
                      radius * 1.93f, radius * 1.93f, 200.0f, 130.0f);

            // Warm off-white / cream face (#F7F6EC -> #EDEBD8) exactly like Win7 Traditional
            LinearGradientBrush creamFace(
                PointF(cx, cy - faceRadius), PointF(cx, cy + faceRadius),
                Color(255, 249, 248, 238), Color(255, 236, 233, 214));
            g.FillEllipse(&creamFace, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);
            break;
        }
        case 1: { // 2: Modern Dark (Sleek black chronograph with red accents)
            LinearGradientBrush outerRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(255, 110, 115, 122), Color(255, 28, 30, 35));
            g.FillEllipse(&outerRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            LinearGradientBrush darkFace(
                PointF(cx, cy - faceRadius), PointF(cx, cy + faceRadius),
                Color(255, 38, 41, 46), Color(255, 14, 16, 20));
            g.FillEllipse(&darkFace, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);

            Pen redRing(Color(200, 210, 40, 35), radius * 0.02f);
            float rr = faceRadius * 0.94f;
            g.DrawEllipse(&redRing, cx - rr, cy - rr, rr * 2.0f, rr * 2.0f);
            break;
        }
        case 2: { // 3: Silver & Crisp White
            LinearGradientBrush silverRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(255, 245, 248, 252), Color(255, 130, 138, 148));
            g.FillEllipse(&silverRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            Pen rimBorder(Color(255, 95, 102, 112), radius * 0.02f);
            g.DrawEllipse(&rimBorder, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            LinearGradientBrush whiteFace(
                PointF(cx, cy - faceRadius), PointF(cx, cy + faceRadius),
                Color(255, 255, 255, 255), Color(255, 235, 240, 246));
            g.FillEllipse(&whiteFace, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);
            break;
        }
        case 3: { // 4: Sky & Pastel (Spring blue)
            LinearGradientBrush skyRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(255, 135, 205, 245), Color(255, 45, 125, 185));
            g.FillEllipse(&skyRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            LinearGradientBrush skyFace(
                PointF(cx, cy - faceRadius), PointF(cx, cy + faceRadius),
                Color(255, 242, 250, 255), Color(255, 205, 232, 250));
            g.FillEllipse(&skyFace, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);
            break;
        }
        case 4: { // 5: Ruby Red
            LinearGradientBrush rubyRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(255, 225, 65, 65), Color(255, 105, 12, 18));
            g.FillEllipse(&rubyRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            LinearGradientBrush rubyFace(
                PointF(cx, cy - faceRadius), PointF(cx, cy + faceRadius),
                Color(255, 145, 20, 26), Color(255, 72, 8, 12));
            g.FillEllipse(&rubyFace, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);
            break;
        }
        case 5: { // 6: Roman Brushed Steel
            LinearGradientBrush steelRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(255, 195, 202, 210), Color(255, 75, 82, 92));
            g.FillEllipse(&steelRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            LinearGradientBrush steelFace(
                PointF(cx - faceRadius, cy - faceRadius), PointF(cx + faceRadius, cy + faceRadius),
                Color(255, 225, 230, 236), Color(255, 175, 182, 192));
            g.FillEllipse(&steelFace, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);
            break;
        }
        case 6: { // 7: Emerald & Gold
            LinearGradientBrush goldRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(255, 245, 215, 125), Color(255, 155, 115, 32));
            g.FillEllipse(&goldRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            LinearGradientBrush emeraldFace(
                PointF(cx, cy - faceRadius), PointF(cx, cy + faceRadius),
                Color(255, 22, 95, 68), Color(255, 8, 45, 32));
            g.FillEllipse(&emeraldFace, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);
            break;
        }
        case 7: { // 8: Aero Clear Glass
            LinearGradientBrush glassRim(
                PointF(cx - radius, cy - radius), PointF(cx + radius, cy + radius),
                Color(190, 240, 248, 255), Color(130, 120, 150, 185));
            g.FillEllipse(&glassRim, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            SolidBrush glassInner(Color(85, 20, 32, 48));
            g.FillEllipse(&glassInner, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);

            Pen glassEdge(Color(210, 255, 255, 255), radius * 0.022f);
            g.DrawEllipse(&glassEdge, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);
            break;
        }
    }

    // Inner bevel shadow around the face
    Pen innerShadow(Color(115, 0, 0, 0), radius * 0.024f);
    g.DrawEllipse(&innerShadow, cx - faceRadius, cy - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);

    // Determine tick and numeral colors per skin
    bool darkDial = (skin == 1 || skin == 4 || skin == 6 || skin == 7);
    Color tickMajorColor = darkDial ? Color(245, 245, 245, 245) : Color(255, 36, 37, 40);
    Color tickMinorColor = darkDial ? Color(175, 210, 215, 220) : Color(215, 82, 84, 88);
    Color numColor       = darkDial ? Color(250, 250, 250, 245) : Color(255, 38, 40, 44);
    if (skin == 6) { // Emerald Gold
        tickMajorColor = Color(255, 248, 220, 140);
        numColor = Color(255, 252, 230, 160);
    }

    // 2. Draw 60 minute/hour ticks (in original Win7 Traditional clock, ticks start right from the inner edge of the bezel!)
    Pen majorPen(tickMajorColor, radius * 0.028f);
    Pen minorPen(tickMinorColor, radius * 0.015f);
    for (int i = 0; i < 60; ++i) {
        float angle = (i * 6.0f - 90.0f) * kPi / 180.0f;
        bool isHour = (i % 5 == 0);
        float outerR = faceRadius * 0.99f;
        float innerR = isHour ? (faceRadius * 0.855f) : (faceRadius * 0.915f);
        Pen& pen = isHour ? majorPen : minorPen;
        g.DrawLine(&pen,
                   cx + std::cos(angle) * innerR, cy + std::sin(angle) * innerR,
                   cx + std::cos(angle) * outerR, cy + std::sin(angle) * outerR);
    }

    // 3. Draw Numerals (1..12 or Roman)
    // In original Win7 Traditional clock, numerals are tall, condensed medium-weight sans-serif (Arial/Franklin style, not overly thick)
    FontFamily fontFamily(skin == 5 ? L"Times New Roman" : (skin == 0 ? L"Arial" : L"Segoe UI"));
    float fontSize = faceRadius * (skin == 5 ? 0.21f : (skin == 0 ? 0.245f : 0.23f));
    int fontStyle = (skin == 0) ? FontStyleRegular : FontStyleBold;
    Font numFont(&fontFamily, fontSize, fontStyle, UnitPixel);
    SolidBrush numBrush(numColor);
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);

    static const wchar_t* kRoman[] = {
        L"I", L"II", L"III", L"IV", L"V", L"VI",
        L"VII", L"VIII", L"IX", L"X", L"XI", L"XII"
    };

    float numRadius = faceRadius * 0.695f;
    for (int n = 1; n <= 12; ++n) {
        float angle = (n * 30.0f - 90.0f) * kPi / 180.0f;
        float nx = cx + std::cos(angle) * numRadius;
        float ny = cy + std::sin(angle) * numRadius;
        wchar_t buf[8];
        if (skin == 5) {
            wcscpy_s(buf, kRoman[n - 1]);
        } else {
            swprintf_s(buf, L"%d", n);
        }
        if (skin == 0) {
            // Slight horizontal condensation (0.86x width) to match the tall, narrow numerals in Win7 Traditional clock
            GraphicsState state = g.Save();
            g.TranslateTransform(nx, ny);
            g.ScaleTransform(0.86f, 1.05f);
            RectF localRc(-faceRadius * 0.28f, -faceRadius * 0.20f,
                          faceRadius * 0.56f, faceRadius * 0.40f);
            g.DrawString(buf, -1, &numFont, localRc, &sf, &numBrush);
            g.Restore(state);
        } else {
            RectF textRc(nx - faceRadius * 0.25f, ny - faceRadius * 0.18f,
                         faceRadius * 0.50f, faceRadius * 0.36f);
            g.DrawString(buf, -1, &numFont, textRc, &sf, &numBrush);
        }
    }

    // 4. Optional Clock Name on the dial (above 6 o'clock, just like Win7)
    if (customName && customName[0] != L'\0') {
        Font nameFont(&fontFamily, faceRadius * 0.145f, FontStyleRegular, UnitPixel);
        SolidBrush nameBrush(darkDial ? Color(210, 230, 235, 240) : Color(210, 60, 64, 70));
        RectF nameRc(cx - faceRadius * 0.55f, cy + faceRadius * 0.22f,
                     faceRadius * 1.10f, faceRadius * 0.22f);
        g.DrawString(customName, -1, &nameFont, nameRc, &sf, &nameBrush);
    }

    // 5. Clock Hands (Authentic Win7 sword/diamond-tapered hands + drop shadow + red second hand with tail)
    float sec = st.wSecond + st.wMilliseconds / 1000.0f;
    float min = st.wMinute + sec / 60.0f;
    float hr  = (st.wHour % 12) + min / 60.0f;

    float hrAngle  = (hr  * 30.0f - 90.0f) * kPi / 180.0f;
    float minAngle = (min * 6.0f  - 90.0f) * kPi / 180.0f;
    float secAngle = (st.wSecond * 6.0f - 90.0f) * kPi / 180.0f;

    Color handFillColor = darkDial ? Color(255, 242, 244, 248) : Color(255, 20, 22, 25);
    if (skin == 6) handFillColor = Color(255, 248, 224, 150);

    auto drawSwordHand = [&](float angle, float length, float tailLen, float pivotHalfW, float midHalfW, float tipHalfW) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        float perpX = -sinA;
        float perpY = cosA;
        float midDist = length * 0.28f;

        // 6-point sword/lozenge polygon hand (slightly wider past the center hub, tapering to a sharp tip)
        PointF pts[6] = {
            PointF(cx - cosA * tailLen + perpX * (pivotHalfW * 0.75f), cy - sinA * tailLen + perpY * (pivotHalfW * 0.75f)),
            PointF(cx + cosA * midDist + perpX * midHalfW,             cy + sinA * midDist + perpY * midHalfW),
            PointF(cx + cosA * length  + perpX * tipHalfW,             cy + sinA * length  + perpY * tipHalfW),
            PointF(cx + cosA * length  - perpX * tipHalfW,             cy + sinA * length  - perpY * tipHalfW),
            PointF(cx + cosA * midDist - perpX * midHalfW,             cy + sinA * midDist - perpY * midHalfW),
            PointF(cx - cosA * tailLen - perpX * (pivotHalfW * 0.75f), cy - sinA * tailLen - perpY * (pivotHalfW * 0.75f)),
        };
        PointF shadowPts[6];
        for (int i = 0; i < 6; ++i) {
            shadowPts[i] = PointF(pts[i].X + radius * 0.022f, pts[i].Y + radius * 0.032f);
        }
        SolidBrush shadowBrush(Color(75, 0, 0, 0));
        g.FillPolygon(&shadowBrush, shadowPts, 6);

        SolidBrush handBrush(handFillColor);
        g.FillPolygon(&handBrush, pts, 6);
    };

    // Hour hand (reaches just inside the numerals, like at 5 o'clock on screenshot)
    drawSwordHand(hrAngle, faceRadius * 0.56f, faceRadius * 0.12f, radius * 0.036f, radius * 0.048f, radius * 0.008f);
    // Minute hand (reaches almost to the outer tick marks, like at 12 o'clock on screenshot)
    drawSwordHand(minAngle, faceRadius * 0.83f, faceRadius * 0.14f, radius * 0.028f, radius * 0.038f, radius * 0.006f);

    // Center cap with metallic silver ring + dark center (exact match to screenshot!)
    float outerHubR = radius * 0.062f;
    SolidBrush hubShadow(Color(90, 0, 0, 0));
    g.FillEllipse(&hubShadow, cx - outerHubR + radius * 0.015f, cy - outerHubR + radius * 0.02f, outerHubR * 2.0f, outerHubR * 2.0f);

    SolidBrush outerHubBrush(handFillColor);
    g.FillEllipse(&outerHubBrush, cx - outerHubR, cy - outerHubR, outerHubR * 2.0f, outerHubR * 2.0f);

    // Second hand (Authentic crimson red with counterweight tail)
    if (showSeconds) {
        float cosS = std::cos(secAngle);
        float sinS = std::sin(secAngle);
        float secLen = faceRadius * 0.85f;
        float secTail = faceRadius * 0.22f;

        Pen secShadow(Color(75, 0, 0, 0), radius * 0.020f);
        g.DrawLine(&secShadow,
                   cx - cosS * secTail + radius * 0.018f, cy - sinS * secTail + radius * 0.025f,
                   cx + cosS * secLen  + radius * 0.018f, cy + sinS * secLen  + radius * 0.025f);

        Pen secPen(Color(255, 202, 30, 36), radius * 0.018f);
        secPen.SetStartCap(LineCapRound);
        secPen.SetEndCap(LineCapRound);
        g.DrawLine(&secPen,
                   cx - cosS * secTail, cy - sinS * secTail,
                   cx + cosS * secLen,  cy + sinS * secLen);

        float redCapR = radius * 0.038f;
        SolidBrush redCap(Color(255, 202, 30, 36));
        g.FillEllipse(&redCap, cx - redCapR, cy - redCapR, redCapR * 2.0f, redCapR * 2.0f);
    }

    // Center light silver/white pin dot (visible right in the center of the hands on screenshot)
    float pinR = radius * 0.022f;
    SolidBrush pinBrush(Color(255, 232, 235, 240));
    g.FillEllipse(&pinBrush, cx - pinR, cy - pinR, pinR * 2.0f, pinR * 2.0f);

    // 6. Subtle top glass reflection (gentle so it doesn't wash out the cream dial)
    GraphicsPath glarePath;
    float gr = faceRadius * 0.96f;
    glarePath.AddArc(cx - gr, cy - gr, gr * 2.0f, gr * 2.0f, 200.0f, 140.0f);
    glarePath.AddArc(cx - gr * 1.08f, cy - gr * 0.22f, gr * 2.16f, gr * 1.10f, 335.0f, -130.0f);
    glarePath.CloseFigure();

    LinearGradientBrush glareBrush(
        PointF(cx, cy - gr), PointF(cx, cy),
        Color(skin == 0 ? 45 : 75, 255, 255, 255), Color(0, 255, 255, 255));
    g.FillPath(&glareBrush, &glarePath);
}

void DrawClockGadget(Graphics& g) {
    if (!g_showClock) return;
    RectF rc = GetWidgetLocalBodyRect(WidgetClock);
    float cx = rc.X + rc.Width * 0.5f;
    float cy = rc.Y + rc.Height * 0.5f;
    float radius = rc.Width * 0.465f;

    SYSTEMTIME st = GetClockTime();
    RenderClockFaceAt(g, cx, cy, radius, g_clockSkin, st, g_showSeconds, g_clockName, true);
    DrawWin7HoverToolbar(g, WidgetClock);
}

// ── Authentic Windows 7 Calendar Gadget Renderer ─────────────────────────────

void DrawSpiralRings(Graphics& g, float x, float yCenter, float width, float s) {
    // 11 twin-wire metallic binder rings across the top of the orange page
    const int ringCount = 11;
    float marginX = 11.0f * s;
    float usableW = width - marginX * 2.0f;
    float step = usableW / (ringCount - 1);
    float ringH = 12.0f * s;
    float wireW = 2.2f * s;

    for (int i = 0; i < ringCount; ++i) {
        float rx = x + marginX + i * step;

        // Punched dark holes in the paper
        SolidBrush holeBrush(Color(220, 35, 18, 8));
        if (g_calendarExpanded) {
            // Top hole in white sheet
            g.FillEllipse(&holeBrush, rx - 3.2f * s, yCenter - ringH * 0.52f - 1.5f * s, 6.4f * s, 3.6f * s);
        }
        // Bottom hole in orange sheet
        g.FillEllipse(&holeBrush, rx - 3.2f * s, yCenter + ringH * 0.42f - 1.5f * s, 6.4f * s, 3.6f * s);

        // Twin metallic wires per slot
        for (int w = 0; w < 2; ++w) {
            float wx = rx + (w == 0 ? -1.4f * s : 0.8f * s);
            RectF wireRc(wx, yCenter - ringH * 0.5f, wireW, ringH);

            // Drop shadow of wire
            SolidBrush wireShadow(Color(110, 0, 0, 0));
            g.FillRectangle(&wireShadow, wireRc.X + 0.8f * s, wireRc.Y + 1.0f * s, wireRc.Width, wireRc.Height);

            LinearGradientBrush metalBrush(
                PointF(wireRc.X, wireRc.Y),
                PointF(wireRc.X, wireRc.Y + wireRc.Height),
                Color(255, 245, 248, 252),
                Color(255, 95, 102, 110));
            Color blendColors[3] = {
                Color(255, 130, 136, 145),
                Color(255, 250, 252, 255),
                Color(255, 75, 80, 88)
            };
            REAL blendPos[3] = {0.0f, 0.45f, 1.0f};
            metalBrush.SetInterpolationColors(blendColors, blendPos, 3);
            FillRoundRect(g, wireRc, 1.0f * s, metalBrush);
        }
    }
}

void DrawOrangeDayPage(Graphics& g, const RectF& pageRc, float s) {
    // Soft shadow under the orange card
    for (int i = 4; i >= 1; --i) {
        float exp = i * 1.4f * s;
        SolidBrush sh(Color(static_cast<BYTE>(18 * (5 - i)), 0, 0, 0));
        FillRoundRect(g, RectF(pageRc.X - exp, pageRc.Y - exp + i * 1.2f * s,
                               pageRc.Width + exp * 2.0f, pageRc.Height + exp * 2.0f), 6.0f * s, sh);
    }

    // Bottom multi-page paper stack edge (gives the realistic thick pad look)
    FillRoundRect(g, RectF(pageRc.X + 1.5f * s, pageRc.Y + 3.0f * s, pageRc.Width - 3.0f * s, pageRc.Height),
                  5.0f * s, SolidBrush(Color(255, 140, 45, 5)));
    FillRoundRect(g, RectF(pageRc.X + 0.8f * s, pageRc.Y + 1.5f * s, pageRc.Width - 1.6f * s, pageRc.Height),
                  5.0f * s, SolidBrush(Color(255, 185, 65, 10)));

    // Main vibrant Windows 7 Orange gradient (clean, uniform top-to-bottom without any horizontal band!)
    LinearGradientBrush orangeBrush(
        PointF(pageRc.X, pageRc.Y),
        PointF(pageRc.X, pageRc.Y + pageRc.Height),
        Color(255, 240, 102, 14),
        Color(255, 194, 52, 2));
    FillRoundRect(g, pageRc, 4.5f * s, orangeBrush);

    // Crisp outer border
    Pen orangeBorder(Color(235, 150, 38, 0), 1.0f * s);
    DrawRoundRect(g, pageRc, 4.5f * s, orangeBorder);

    // Determine date to display on the orange card
    SYSTEMTIME st;
    GetLocalTime(&st);
    int dispYear = st.wYear;
    int dispMonth = st.wMonth;
    int dispDay = st.wDay;
    int dispDow = st.wDayOfWeek;

    if (g_calSelectedYear > 0 && g_calSelectedMonth > 0 && g_calSelectedDay > 0) {
        dispYear = g_calSelectedYear;
        dispMonth = g_calSelectedMonth;
        dispDay = g_calSelectedDay;
        dispDow = DayOfWeekForDate(dispYear, dispMonth, dispDay);
    }

    FontFamily segoe(L"Segoe UI");
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);

    SolidBrush whiteBrush(Color(255, 255, 255, 255));
    SolidBrush shadowTextBrush(Color(75, 100, 20, 0));

    wchar_t monthYearStr[64];
    swprintf_s(monthYearStr, L"%s %d", MonthNameGenitiveOrFull(dispMonth), dispYear);

    const wchar_t* dowStr = DayOfWeekName(dispDow);

    wchar_t dayNumStr[16];
    swprintf_s(dayNumStr, L"%d", dispDay);

    Font smallFont(&segoe, 12.0f * s, FontStyleRegular, UnitPixel);
    Font bigDayFont(&segoe, 58.0f * s, FontStyleRegular, UnitPixel);

    // Notice on user's screenshots:
    // In Russian Win7 (screenshot 2): Top = day of week ("среда"), Middle = "30", Bottom = "Октябрь 2013"
    // In English Win7 (screenshot 1): Top = "September 2008", Middle = "12", Bottom = "Friday"
    const wchar_t* topLine = g_isRussian ? dowStr : monthYearStr;
    const wchar_t* botLine = g_isRussian ? monthYearStr : dowStr;

    RectF topRc(pageRc.X + 4.0f * s, pageRc.Y + 12.0f * s, pageRc.Width - 8.0f * s, 20.0f * s);
    RectF midRc(pageRc.X + 2.0f * s, pageRc.Y + 30.0f * s, pageRc.Width - 4.0f * s, 74.0f * s);
    RectF botRc(pageRc.X + 4.0f * s, pageRc.Y + 106.0f * s, pageRc.Width - 8.0f * s, 20.0f * s);

    // Subtle text shadows
    g.DrawString(topLine, -1, &smallFont, RectF(topRc.X, topRc.Y + 1.0f * s, topRc.Width, topRc.Height), &sf, &shadowTextBrush);
    g.DrawString(topLine, -1, &smallFont, topRc, &sf, &whiteBrush);

    g.DrawString(dayNumStr, -1, &bigDayFont, RectF(midRc.X, midRc.Y + 1.5f * s, midRc.Width, midRc.Height), &sf, &shadowTextBrush);
    g.DrawString(dayNumStr, -1, &bigDayFont, midRc, &sf, &whiteBrush);

    g.DrawString(botLine, -1, &smallFont, RectF(botRc.X, botRc.Y + 1.0f * s, botRc.Width, botRc.Height), &sf, &shadowTextBrush);
    g.DrawString(botLine, -1, &smallFont, botRc, &sf, &whiteBrush);
}

void DrawTopWhiteMonthSheet(Graphics& g, const RectF& sheetRc, float s) {
    // Soft shadow under the top white card
    for (int i = 3; i >= 1; --i) {
        float exp = i * 1.2f * s;
        SolidBrush sh(Color(static_cast<BYTE>(16 * (4 - i)), 0, 0, 0));
        FillRoundRect(g, RectF(sheetRc.X - exp, sheetRc.Y - exp + i * 1.0f * s,
                               sheetRc.Width + exp * 2.0f, sheetRc.Height + exp * 2.0f), 6.0f * s, sh);
    }

    // Off-white paper gradient (matches screenshot #1 right & screenshot #2 bottom)
    LinearGradientBrush paperBrush(
        PointF(sheetRc.X, sheetRc.Y),
        PointF(sheetRc.X, sheetRc.Y + sheetRc.Height),
        Color(255, 250, 251, 252),
        Color(255, 224, 227, 232));
    FillRoundRect(g, sheetRc, 5.0f * s, paperBrush);

    Pen borderPen(Color(210, 150, 156, 165), 1.0f * s);
    DrawRoundRect(g, sheetRc, 5.0f * s, borderPen);

    FontFamily segoe(L"Segoe UI");
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);

    // Header: [<] "Sep 08" / "Окт 26" [>]
    wchar_t headerBuf[32];
    swprintf_s(headerBuf, L"%s %02d", MonthNameShort(g_calViewMonth), g_calViewYear % 100);

    Font headerFont(&segoe, 11.0f * s, FontStyleRegular, UnitPixel);
    SolidBrush headerBrush(Color(255, 30, 34, 40));
    RectF headerRc(sheetRc.X + 20.0f * s, sheetRc.Y + 6.0f * s, sheetRc.Width - 40.0f * s, 16.0f * s);
    g.DrawString(headerBuf, -1, &headerFont, headerRc, &sf, &headerBrush);

    // Left / Right triangular arrows
    Color leftArrColor = (g_hoveredBtn == CalBtnPrevMonth) ? Color(255, 230, 95, 15) : Color(255, 125, 130, 138);
    Color rightArrColor = (g_hoveredBtn == CalBtnNextMonth) ? Color(255, 230, 95, 15) : Color(255, 125, 130, 138);
    SolidBrush leftBrush(leftArrColor);
    SolidBrush rightBrush(rightArrColor);

    float arrY = sheetRc.Y + 14.0f * s;
    PointF leftPts[3] = {
        PointF(sheetRc.X + 9.0f * s,  arrY),
        PointF(sheetRc.X + 14.5f * s, arrY - 4.0f * s),
        PointF(sheetRc.X + 14.5f * s, arrY + 4.0f * s)
    };
    PointF rightPts[3] = {
        PointF(sheetRc.X + sheetRc.Width - 9.0f * s,  arrY),
        PointF(sheetRc.X + sheetRc.Width - 14.5f * s, arrY - 4.0f * s),
        PointF(sheetRc.X + sheetRc.Width - 14.5f * s, arrY + 4.0f * s)
    };
    g.FillPolygon(&leftBrush, leftPts, 3);
    g.FillPolygon(&rightBrush, rightPts, 3);

    // Weekday headers
    // Russian starts Monday with single capital letters (П В С Ч П С В — exact match to screenshot!),
    // English starts Sunday (S M T W T F S)
    static const wchar_t* kDowRu[7] = {L"П", L"В", L"С", L"Ч", L"П", L"С", L"В"};
    static const wchar_t* kDowEn[7] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};

    SYSTEMTIME today;
    GetLocalTime(&today);

    float gridLeft = sheetRc.X + 5.0f * s;
    float gridWidth = sheetRc.Width - 10.0f * s;
    float colW = gridWidth / 7.0f;
    float dowY = sheetRc.Y + 24.0f * s;
    float startY = sheetRc.Y + 38.5f * s;
    float rowH = 14.5f * s;

    // Subtle vertical column separator lines (just like on the original Win7 Calendar screenshots!)
    Pen colSepPen(Color(115, 200, 204, 210), 1.0f);
    for (int col = 1; col < 7; ++col) {
        float lx = std::round(gridLeft + col * colW);
        g.DrawLine(&colSepPen, lx, dowY + 1.0f * s, lx, startY + rowH * 6.0f - 2.0f * s);
    }

    Font dowFont(&segoe, 10.0f * s, FontStyleBold, UnitPixel);
    for (int col = 0; col < 7; ++col) {
        int colDow = g_isRussian ? ((col + 1) % 7) : col;
        bool isCurrentDow = (today.wYear == g_calViewYear && today.wMonth == g_calViewMonth && today.wDayOfWeek == colDow);
        SolidBrush dowBrush(isCurrentDow ? Color(255, 228, 92, 12) : Color(255, 118, 122, 128));
        RectF cellRc(gridLeft + col * colW, dowY, colW, 13.5f * s);
        g.DrawString(g_isRussian ? kDowRu[col] : kDowEn[col], -1, &dowFont, cellRc, &sf, &dowBrush);
    }

    // Days grid (6 rows x 7 columns, right-aligned numbers inside each column like original Win7 Calendar)
    int firstDow = DayOfWeekForDate(g_calViewYear, g_calViewMonth, 1);
    int leadCount = g_isRussian ? ((firstDow + 6) % 7) : firstDow;

    int prevMonth = (g_calViewMonth == 1) ? 12 : (g_calViewMonth - 1);
    int prevYear  = (g_calViewMonth == 1) ? (g_calViewYear - 1) : g_calViewYear;
    int daysInPrev = DaysInMonth(prevYear, prevMonth);
    int daysInCurr = DaysInMonth(g_calViewYear, g_calViewMonth);

    Font numFont(&segoe, 10.0f * s, FontStyleRegular, UnitPixel);
    Font numBoldFont(&segoe, 10.0f * s, FontStyleBold, UnitPixel);

    SolidBrush currMonthBrush(Color(255, 20, 24, 30));
    SolidBrush otherMonthBrush(Color(255, 175, 180, 188));
    SolidBrush whiteText(Color(255, 255, 255, 255));

    for (int cell = 0; cell < 42; ++cell) {
        int row = cell / 7;
        int col = cell % 7;
        RectF cellRc(gridLeft + col * colW, startY + row * rowH, colW, rowH);

        int dayNum = 0;
        bool isCurrentMonth = false;
        if (cell < leadCount) {
            dayNum = daysInPrev - leadCount + 1 + cell;
        } else if (cell < leadCount + daysInCurr) {
            dayNum = cell - leadCount + 1;
            isCurrentMonth = true;
        } else {
            dayNum = cell - (leadCount + daysInCurr) + 1;
        }

        wchar_t label[8];
        swprintf_s(label, L"%d", dayNum);

        if (isCurrentMonth) {
            bool isToday = (today.wYear == g_calViewYear && today.wMonth == g_calViewMonth && today.wDay == dayNum);
            bool isSelected = (g_calSelectedYear == g_calViewYear && g_calSelectedMonth == g_calViewMonth && g_calSelectedDay == dayNum);

            if (isToday) {
                // Solid orange highlight box for Today (just like "12" on screenshot #1 and "30" on screenshot #2)
                RectF hiRc(cellRc.X + 0.5f * s, cellRc.Y + 0.5f * s, cellRc.Width - 1.0f * s, cellRc.Height - 1.0f * s);
                SolidBrush orangeBox(Color(255, 232, 102, 18));
                g.FillRectangle(&orangeBox, hiRc);
                Pen orangeRim(Color(255, 185, 65, 5), 1.0f * s);
                g.DrawRectangle(&orangeRim, hiRc.X, hiRc.Y, hiRc.Width, hiRc.Height);
                g.DrawString(label, -1, &numBoldFont, cellRc, &sf, &whiteText);
            } else if (isSelected) {
                RectF hiRc(cellRc.X + 0.5f * s, cellRc.Y + 0.5f * s, cellRc.Width - 1.0f * s, cellRc.Height - 1.0f * s);
                Pen selRim(Color(255, 232, 102, 18), 1.2f * s);
                g.DrawRectangle(&selRim, hiRc.X, hiRc.Y, hiRc.Width, hiRc.Height);
                g.DrawString(label, -1, &numBoldFont, cellRc, &sf, &currMonthBrush);
            } else {
                g.DrawString(label, -1, &numFont, cellRc, &sf, &currMonthBrush);
            }
        } else {
            g.DrawString(label, -1, &numFont, cellRc, &sf, &otherMonthBrush);
        }
    }
}

void DrawCalendarGadget(Graphics& g) {
    if (!g_showCalendar) return;

    const float s = GetWidgetScale(WidgetCalendar);
    RectF body = GetWidgetLocalBodyRect(WidgetCalendar);

    if (!g_calendarExpanded) {
        // Collapsed: Single orange page with spiral rings at the top
        RectF orangeRc(body.X + 4.0f * s, body.Y + 8.0f * s, 122.0f * s, 134.0f * s);
        DrawOrangeDayPage(g, orangeRc, s);
        DrawSpiralRings(g, orangeRc.X, orangeRc.Y + 1.0f * s, orangeRc.Width, s);
    } else {
        // Expanded: Top white month sheet + Bottom orange day page joined by spiral rings
        RectF topSheetRc(body.X + 4.0f * s, body.Y + 4.0f * s, 122.0f * s, 136.0f * s);
        RectF botOrangeRc(body.X + 4.0f * s, body.Y + 145.0f * s, 122.0f * s, 136.0f * s);

        DrawTopWhiteMonthSheet(g, topSheetRc, s);
        DrawOrangeDayPage(g, botOrangeRc, s);
        DrawSpiralRings(g, botOrangeRc.X, body.Y + 142.5f * s, botOrangeRc.Width, s);
    }

    DrawWin7HoverToolbar(g, WidgetCalendar);
}

// ── Hit-Testing & Hover Mechanics (Per-Widget Local Coordinates) ─────────────

int HitTestWidgetLocal(int widgetId, PointF localPt, int* outButtonId, int* outDayNum) {
    if (outButtonId) *outButtonId = HoverBtnNone;
    if (outDayNum) *outDayNum = 0;

    if (widgetId == WidgetClock && !g_showClock) return -1;
    if (widgetId == WidgetCalendar && !g_showCalendar) return -1;

    const float s = GetWidgetScale(widgetId);

    // Check hover toolbar buttons first
    RectF bar = GetWidgetLocalHoverBarRect(widgetId);
    if (PtInRectF(bar, localPt)) {
        RectF btnCloseRc(bar.X, bar.Y, bar.Width, 19.5f * s);
        RectF btnMidRc(bar.X, bar.Y + 19.5f * s, bar.Width, 18.0f * s);
        if (PtInRectF(btnCloseRc, localPt)) {
            if (outButtonId) *outButtonId = HoverBtnClose;
        } else if (PtInRectF(btnMidRc, localPt)) {
            if (outButtonId) *outButtonId = HoverBtnSizeOrOpts;
        } else {
            if (outButtonId) *outButtonId = HoverBtnDrag;
        }
        return widgetId;
    }

    RectF body = GetWidgetLocalBodyRect(widgetId);
    if (!PtInRectF(body, localPt)) return -1;

    if (widgetId == WidgetClock) {
        float cx = body.X + body.Width * 0.5f;
        float cy = body.Y + body.Height * 0.5f;
        float dx = localPt.X - cx;
        float dy = localPt.Y - cy;
        float r = body.Width * 0.48f;
        if (dx * dx + dy * dy <= r * r) {
            return WidgetClock;
        }
        return -1;
    }

    if (widgetId == WidgetCalendar) {
        if (g_calendarExpanded) {
            RectF topSheetRc(body.X + 4.0f * s, body.Y + 4.0f * s, 122.0f * s, 136.0f * s);
            RectF prevRc(topSheetRc.X + 4.0f * s, topSheetRc.Y + 4.0f * s, 18.0f * s, 18.0f * s);
            RectF nextRc(topSheetRc.X + topSheetRc.Width - 22.0f * s, topSheetRc.Y + 4.0f * s, 18.0f * s, 18.0f * s);
            if (PtInRectF(prevRc, localPt)) {
                if (outButtonId) *outButtonId = CalBtnPrevMonth;
                return WidgetCalendar;
            }
            if (PtInRectF(nextRc, localPt)) {
                if (outButtonId) *outButtonId = CalBtnNextMonth;
                return WidgetCalendar;
            }

            // Check day cells in the monthly grid
            float gridLeft = topSheetRc.X + 5.0f * s;
            float gridWidth = topSheetRc.Width - 10.0f * s;
            float colW = gridWidth / 7.0f;
            float startY = topSheetRc.Y + 38.5f * s;
            float rowH = 14.5f * s;
            RectF gridRc(gridLeft, startY, gridWidth, rowH * 6.0f);
            if (PtInRectF(gridRc, localPt)) {
                int col = std::min(6, std::max(0, static_cast<int>((localPt.X - gridLeft) / colW)));
                int row = std::min(5, std::max(0, static_cast<int>((localPt.Y - startY) / rowH)));
                int cell = row * 7 + col;
                int firstDow = DayOfWeekForDate(g_calViewYear, g_calViewMonth, 1);
                int leadCount = g_isRussian ? ((firstDow + 6) % 7) : firstDow;
                int daysInCurr = DaysInMonth(g_calViewYear, g_calViewMonth);
                if (cell >= leadCount && cell < leadCount + daysInCurr) {
                    if (outButtonId) *outButtonId = CalBtnDayCell;
                    if (outDayNum) *outDayNum = cell - leadCount + 1;
                }
            }
        }
        return WidgetCalendar;
    }

    return -1;
}

// ── Per-Widget Layered Window Surface & Render ───────────────────────────────

void DestroyRenderSurface(int widgetId) {
    RenderSurface& surf = g_surfaces[widgetId];
    if (surf.memDc) {
        if (surf.oldBitmap)
            SelectObject(surf.memDc, surf.oldBitmap);
        if (surf.bitmap)
            DeleteObject(surf.bitmap);
        DeleteDC(surf.memDc);
    }
    surf = {};
}

bool EnsureRenderSurface(int widgetId, HDC screenDc, int width, int height) {
    RenderSurface& surf = g_surfaces[widgetId];
    if (surf.memDc && surf.bitmap &&
        surf.width == width && surf.height == height) {
        return true;
    }
    DestroyRenderSurface(widgetId);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    surf.memDc = CreateCompatibleDC(screenDc);
    if (!surf.memDc) return false;
    surf.bitmap = CreateDIBSection(screenDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!surf.bitmap) {
        DestroyRenderSurface(widgetId);
        return false;
    }
    surf.oldBitmap = SelectObject(surf.memDc, surf.bitmap);
    surf.width = width;
    surf.height = height;
    return true;
}

void RenderWidget(int widgetId) {
    HWND hwnd = (widgetId == WidgetClock) ? g_clockHwnd : g_calendarHwnd;
    if (!hwnd || !IsWindow(hwnd)) return;

    bool visible = (widgetId == WidgetClock) ? g_showClock : g_showCalendar;
    if (!visible) {
        if (IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
        }
        return;
    }

    UpdateEffectiveScale();
    SizeF winSz = GetWidgetWindowSize(widgetId);
    int w = std::max(1, static_cast<int>(winSz.Width));
    int h = std::max(1, static_cast<int>(winSz.Height));

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) return;
    if (!EnsureRenderSurface(widgetId, screenDc, w, h)) {
        ReleaseDC(nullptr, screenDc);
        return;
    }

    RenderSurface& surf = g_surfaces[widgetId];
    {
        Graphics g(surf.memDc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
        g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        g.Clear(Color(0, 0, 0, 0));

        // Fill the hover toolbar area with alpha=1 when hidden so it still receives mouse hover events
        if (g_hoveredWidget != widgetId) {
            RectF bar = GetWidgetLocalHoverBarRect(widgetId);
            SolidBrush hitBrush(Color(1, 0, 0, 0));
            g.FillRectangle(&hitBrush, bar);
        }

        if (widgetId == WidgetClock) {
            DrawClockGadget(g);
        } else {
            DrawCalendarGadget(g);
        }
    }

    PointF wPos = (widgetId == WidgetClock) ? g_clockPos : g_calendarPos;
    SIZE size = {w, h};
    POINT src = {0, 0};
    POINT pos = {static_cast<LONG>(std::round(wPos.X)), static_cast<LONG>(std::round(wPos.Y))};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, g_masterOpacity, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, screenDc, &pos, &size,
                        surf.memDc, &src, 0, &blend, ULW_ALPHA);

    if (!IsWindowVisible(hwnd)) {
        SetWindowPos(hwnd, HWND_BOTTOM, pos.x, pos.y, w, h,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    }

    ReleaseDC(nullptr, screenDc);
}

void Render() {
    RenderWidget(WidgetClock);
    RenderWidget(WidgetCalendar);
}

// ── Authentic Windows 7 Clock Options Dialog (matching Screenshot #2) ────────

struct ClockOptionsState {
    int tempSkin = 0;
    bool tempShowSeconds = true;
    int tempTzIndex = 0;
    wchar_t tempName[64] = {};
    HWND hEditName = nullptr;
    HWND hComboTz = nullptr;
    HWND hCheckSec = nullptr;
    HFONT hFont = nullptr;
    float dpiScale = 1.0f;
    bool hoverPrev = false;
    bool hoverNext = false;
};

ClockOptionsState g_optState;

constexpr int kIdEditName    = 103;
constexpr int kIdComboTz     = 104;
constexpr int kIdCheckSec    = 105;

float GetOptionsDpiScale(HWND hwnd = nullptr) {
    if (hwnd && IsWindow(hwnd)) {
        POINT pt = {0, 0};
        ClientToScreen(hwnd, &pt);
        return GetMonitorDpiScaleForPoint(pt, hwnd);
    }
    if (g_optState.dpiScale > 0.0f) return g_optState.dpiScale;
    POINT clockPt = {static_cast<LONG>(g_clockPos.X + 65.0f), static_cast<LONG>(g_clockPos.Y + 65.0f)};
    return GetMonitorDpiScaleForPoint(clockPt, g_clockHwnd);
}

RectF GetOptionsPreviewBoxRect() {
    const float ds = GetOptionsDpiScale(g_optionsHwnd);
    return RectF(18.0f * ds, 44.0f * ds, 244.0f * ds, 168.0f * ds);
}

RectF GetOptionsPrevArrowRect() {
    const float ds = GetOptionsDpiScale(g_optionsHwnd);
    return RectF(94.0f * ds, 180.0f * ds, 22.0f * ds, 22.0f * ds);
}

RectF GetOptionsNextArrowRect() {
    const float ds = GetOptionsDpiScale(g_optionsHwnd);
    return RectF(164.0f * ds, 180.0f * ds, 22.0f * ds, 22.0f * ds);
}

void PaintClockOptionsDialog(HWND hwnd, HDC hdc) {
    const float ds = GetOptionsDpiScale(hwnd);
    RECT clientRc;
    GetClientRect(hwnd, &clientRc);
    int w = clientRc.right - clientRc.left;
    int h = clientRc.bottom - clientRc.top;

    HDC memDc = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, w, h);
    HGDIOBJ oldBmp = SelectObject(memDc, memBmp);

    {
        Graphics g(memDc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

        // Dialog background (classic Win7 Aero dialog light gray-white)
        SolidBrush bgBrush(Color(255, 245, 246, 248));
        g.FillRectangle(&bgBrush, 0, 0, w, h);

        // Top Aero Glass dark-blue header band (matching "Часы" header on Screenshot #2)
        int headerH = static_cast<int>(std::round(34.0f * ds));
        LinearGradientBrush headerBrush(
            PointF(0.0f, 0.0f), PointF(0.0f, 34.0f * ds),
            Color(255, 38, 60, 82), Color(255, 18, 36, 54));
        g.FillRectangle(&headerBrush, 0, 0, w, headerH);

        FontFamily segoe(L"Segoe UI");
        Font titleFont(&segoe, 14.0f * ds, FontStyleRegular, UnitPixel);
        SolidBrush titleBrush(Color(255, 255, 255, 255));
        StringFormat sfNear;
        sfNear.SetAlignment(StringAlignmentNear);
        sfNear.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(g_isRussian ? L"Часы" : L"Clock", -1, &titleFont,
                     RectF(14.0f * ds, 0.0f, 200.0f * ds, 34.0f * ds), &sfNear, &titleBrush);

        // Preview Box with white background and subtle gray frame
        RectF previewRc = GetOptionsPreviewBoxRect();
        SolidBrush whiteBrush(Color(255, 255, 255, 255));
        g.FillRectangle(&whiteBrush, previewRc);
        Pen framePen(Color(255, 210, 214, 220), 1.0f);
        g.DrawRectangle(&framePen, previewRc.X, previewRc.Y, previewRc.Width, previewRc.Height);
        g.DrawLine(&framePen, previewRc.X, 172.0f * ds, previewRc.X + previewRc.Width, 172.0f * ds);

        // Render live preview of the selected clock face
        SYSTEMTIME st = GetClockTimeForTz(g_optState.tempTzIndex);
        RenderClockFaceAt(g,
                          previewRc.X + previewRc.Width * 0.5f,
                          previewRc.Y + 64.0f * ds,
                          52.0f * ds,
                          g_optState.tempSkin,
                          st,
                          g_optState.tempShowSeconds,
                          g_optState.tempName,
                          true);

        // "< 1 из 8 >" pager controls
        StringFormat sfCenter;
        sfCenter.SetAlignment(StringAlignmentCenter);
        sfCenter.SetLineAlignment(StringAlignmentCenter);
        Font labelFont(&segoe, 12.0f * ds, FontStyleRegular, UnitPixel);
        SolidBrush darkText(Color(255, 30, 34, 40));

        wchar_t pageStr[32];
        if (g_isRussian) {
            swprintf_s(pageStr, L"%d из 8", g_optState.tempSkin + 1);
        } else {
            swprintf_s(pageStr, L"%d of 8", g_optState.tempSkin + 1);
        }
        g.DrawString(pageStr, -1, &labelFont, RectF(116.0f * ds, 178.0f * ds, 48.0f * ds, 24.0f * ds), &sfCenter, &darkText);

        // Draw circular Prev / Next buttons
        auto drawNavCircle = [&](RectF rc, bool isLeft, bool hovered) {
            float cx = rc.X + rc.Width * 0.5f;
            float cy = rc.Y + rc.Height * 0.5f;
            LinearGradientBrush btnGrad(
                PointF(rc.X, rc.Y), PointF(rc.X, rc.Y + rc.Height),
                hovered ? Color(255, 225, 242, 255) : Color(255, 248, 249, 251),
                hovered ? Color(255, 165, 210, 248) : Color(255, 212, 216, 222));
            g.FillEllipse(&btnGrad, rc);
            Pen btnPen(hovered ? Color(255, 60, 140, 210) : Color(255, 155, 162, 172), 1.0f);
            g.DrawEllipse(&btnPen, rc);

            SolidBrush arrowBrush(Color(255, 45, 55, 70));
            if (isLeft) {
                PointF pts[3] = {
                    PointF(cx - 3.0f * ds, cy),
                    PointF(cx + 2.0f * ds, cy - 4.0f * ds),
                    PointF(cx + 2.0f * ds, cy + 4.0f * ds)
                };
                g.FillPolygon(&arrowBrush, pts, 3);
            } else {
                PointF pts[3] = {
                    PointF(cx + 3.0f * ds, cy),
                    PointF(cx - 2.0f * ds, cy - 4.0f * ds),
                    PointF(cx - 2.0f * ds, cy + 4.0f * ds)
                };
                g.FillPolygon(&arrowBrush, pts, 3);
            }
        };

        drawNavCircle(GetOptionsPrevArrowRect(), true, g_optState.hoverPrev);
        drawNavCircle(GetOptionsNextArrowRect(), false, g_optState.hoverNext);

        // Labels for Clock Name and Time Zone
        g.DrawString(g_isRussian ? L"Имя часов:" : L"Clock name:", -1, &labelFont,
                     RectF(18.0f * ds, 220.0f * ds, 240.0f * ds, 18.0f * ds), &sfNear, &darkText);

        g.DrawString(g_isRussian ? L"Часовой пояс:" : L"Time zone:", -1, &labelFont,
                     RectF(18.0f * ds, 268.0f * ds, 240.0f * ds, 18.0f * ds), &sfNear, &darkText);
    }

    BitBlt(hdc, 0, 0, w, h, memDc, 0, 0, SRCCOPY);
    SelectObject(memDc, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDc);
}

LRESULT CALLBACK OptionsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    const float ds = GetOptionsDpiScale(hwnd);
    auto S = [ds](int v) -> int { return static_cast<int>(std::round(v * ds)); };

    switch (msg) {
        case WM_CREATE: {
            g_optState.tempSkin = g_clockSkin;
            g_optState.tempShowSeconds = g_showSeconds;
            g_optState.tempTzIndex = g_clockTzIndex;
            wcsncpy_s(g_optState.tempName, g_clockName, _TRUNCATE);

            if (g_optState.hFont) {
                DeleteObject(g_optState.hFont);
                g_optState.hFont = nullptr;
            }
            HFONT hFont = CreateFontW(-S(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_optState.hFont = hFont;

            g_optState.hEditName = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", g_optState.tempName,
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                S(18), S(240), S(244), S(22), hwnd, reinterpret_cast<HMENU>(kIdEditName), nullptr, nullptr);
            SendMessageW(g_optState.hEditName, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
            SendMessageW(g_optState.hEditName, EM_SETLIMITTEXT, 24, 0);

            g_optState.hComboTz = CreateWindowExW(
                0, L"COMBOBOX", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
                S(18), S(288), S(244), S(240), hwnd, reinterpret_cast<HMENU>(kIdComboTz), nullptr, nullptr);
            SendMessageW(g_optState.hComboTz, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

            for (size_t i = 0; i < ARRAYSIZE(kTimeZones); ++i) {
                const wchar_t* label = g_isRussian ? kTimeZones[i].nameRu : kTimeZones[i].nameEn;
                SendMessageW(g_optState.hComboTz, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label));
            }
            SendMessageW(g_optState.hComboTz, CB_SETCURSEL, g_optState.tempTzIndex, 0);

            g_optState.hCheckSec = CreateWindowExW(
                0, L"BUTTON", g_isRussian ? L"Показывать секундную стрелку" : L"Show the second hand",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
                S(18), S(320), S(244), S(20), hwnd, reinterpret_cast<HMENU>(kIdCheckSec), nullptr, nullptr);
            SendMessageW(g_optState.hCheckSec, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
            SendMessageW(g_optState.hCheckSec, BM_SETCHECK,
                         g_optState.tempShowSeconds ? BST_CHECKED : BST_UNCHECKED, 0);

            HWND hBtnOk = CreateWindowExW(
                0, L"BUTTON", L"OK",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                S(102), S(356), S(76), S(25), hwnd, reinterpret_cast<HMENU>(IDOK), nullptr, nullptr);
            SendMessageW(hBtnOk, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

            HWND hBtnCancel = CreateWindowExW(
                0, L"BUTTON", g_isRussian ? L"Отмена" : L"Cancel",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                S(186), S(356), S(76), S(25), hwnd, reinterpret_cast<HMENU>(IDCANCEL), nullptr, nullptr);
            SendMessageW(hBtnCancel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

            SetTimer(hwnd, 1, 500, nullptr);
            return 0;
        }

        case WM_TIMER: {
            RECT previewRc = {S(18), S(44), S(262), S(172)};
            InvalidateRect(hwnd, &previewRc, FALSE);
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = reinterpret_cast<HDC>(wParam);
            SetBkMode(hdcStatic, TRANSPARENT);
            return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_BTNFACE));
        }

        case WM_MOUSEMOVE: {
            PointF pt( static_cast<float>(GET_X_LPARAM(lParam)),
                       static_cast<float>(GET_Y_LPARAM(lParam)) );
            bool prevH = PtInRectF(GetOptionsPrevArrowRect(), pt);
            bool nextH = PtInRectF(GetOptionsNextArrowRect(), pt);
            if (prevH != g_optState.hoverPrev || nextH != g_optState.hoverNext) {
                g_optState.hoverPrev = prevH;
                g_optState.hoverNext = nextH;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            PointF pt( static_cast<float>(GET_X_LPARAM(lParam)),
                       static_cast<float>(GET_Y_LPARAM(lParam)) );
            if (PtInRectF(GetOptionsPrevArrowRect(), pt)) {
                g_optState.tempSkin = (g_optState.tempSkin + 7) % 8;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRectF(GetOptionsNextArrowRect(), pt)) {
                g_optState.tempSkin = (g_optState.tempSkin + 1) % 8;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            // Allow dragging the dialog by its top header bar
            if (pt.Y < 34.0f * ds) {
                ReleaseCapture();
                SendMessageW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                return 0;
            }
            return 0;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);
            if (id == kIdEditName && code == EN_CHANGE) {
                GetWindowTextW(g_optState.hEditName, g_optState.tempName, ARRAYSIZE(g_optState.tempName));
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (id == kIdCheckSec && code == BN_CLICKED) {
                g_optState.tempShowSeconds =
                    (SendMessageW(g_optState.hCheckSec, BM_GETCHECK, 0, 0) == BST_CHECKED);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (id == kIdComboTz && code == CBN_SELCHANGE) {
                int selTz = static_cast<int>(SendMessageW(g_optState.hComboTz, CB_GETCURSEL, 0, 0));
                if (selTz >= 0 && selTz < static_cast<int>(ARRAYSIZE(kTimeZones))) {
                    g_optState.tempTzIndex = selTz;
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (id == IDOK) {
                GetWindowTextW(g_optState.hEditName, g_clockName, ARRAYSIZE(g_clockName));
                g_clockSkin = g_optState.tempSkin;
                g_showSeconds = (SendMessageW(g_optState.hCheckSec, BM_GETCHECK, 0, 0) == BST_CHECKED);
                int selTz = static_cast<int>(SendMessageW(g_optState.hComboTz, CB_GETCURSEL, 0, 0));
                if (selTz >= 0 && selTz < static_cast<int>(ARRAYSIZE(kTimeZones))) {
                    g_clockTzIndex = selTz;
                }
                SaveStateToStorage();
                Render();
                DestroyWindow(hwnd);
                return 0;
            }
            if (id == IDCANCEL) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            PaintClockOptionsDialog(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            if (g_optState.hFont) {
                DeleteObject(g_optState.hFont);
                g_optState.hFont = nullptr;
            }
            g_optionsHwnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void OpenClockOptionsDialog() {
    if (g_optionsHwnd && IsWindow(g_optionsHwnd)) {
        SetForegroundWindow(g_optionsHwnd);
        return;
    }

    RectF clockRc = GetWidgetScreenBodyRect(WidgetClock);
    POINT clockCenter = {
        static_cast<LONG>(clockRc.X + clockRc.Width * 0.5f),
        static_cast<LONG>(clockRc.Y + clockRc.Height * 0.5f)
    };
    const float ds = GetMonitorDpiScaleForPoint(clockCenter, g_clockHwnd);
    g_optState.dpiScale = ds;

    int dlgW = static_cast<int>(std::round(282.0f * ds));
    int dlgH = static_cast<int>(std::round(400.0f * ds));

    // Place dialog near the clock widget, clamped to the monitor containing the clock
    HMONITOR hMon = MonitorFromPoint(clockCenter, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(hMon, &mi);

    int x = static_cast<int>(clockRc.X) - dlgW - 12;
    if (x < mi.rcWork.left + 10) {
        x = static_cast<int>(clockRc.X + clockRc.Width) + 28;
    }
    int y = static_cast<int>(clockRc.Y);
    if (y + dlgH > mi.rcWork.bottom - 10) {
        y = mi.rcWork.bottom - dlgH - 10;
    }
    if (y < mi.rcWork.top + 10) {
        y = mi.rcWork.top + 10;
    }

    g_optionsHwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        kOptionsClassName,
        g_isRussian ? L"Часы — Параметры" : L"Clock Options",
        WS_POPUP | WS_VISIBLE | WS_SYSMENU,
        x, y, dlgW, dlgH,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (g_optionsHwnd) {
        ShowWindow(g_optionsHwnd, SW_SHOWNORMAL);
        SetForegroundWindow(g_optionsHwnd);
    }
}

// ── Context Menu (Right-click on Clock or Calendar) ──────────────────────────

void ShowGadgetContextMenu(HWND hwnd, int widgetId, POINT screenPt) {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    enum MenuCmd {
        CmdOptions = 1,
        CmdSecondHand,
        CmdToggleCalSize,
        CmdJumpToday,
        CmdShowOtherGadget,
        CmdCloseGadget,
        CmdSkinBase = 100
    };

    if (widgetId == WidgetClock) {
        AppendMenuW(hMenu, MF_STRING | MF_DEFAULT, CmdOptions,
                    g_isRussian ? L"Параметры..." : L"Options...");

        HMENU hSkinMenu = CreatePopupMenu();
        for (int i = 0; i < 8; ++i) {
            wchar_t skinLabel[32];
            swprintf_s(skinLabel, g_isRussian ? L"Циферблат %d" : L"Clock Face %d", i + 1);
            AppendMenuW(hSkinMenu, MF_STRING | (g_clockSkin == i ? MF_CHECKED : 0),
                        CmdSkinBase + i, skinLabel);
        }
        AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSkinMenu),
                    g_isRussian ? L"Скин часов" : L"Clock Skin");

        AppendMenuW(hMenu, MF_STRING | (g_showSeconds ? MF_CHECKED : 0), CmdSecondHand,
                    g_isRussian ? L"Показывать секундную стрелку" : L"Show second hand");

        if (!g_showCalendar) {
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu, MF_STRING, CmdShowOtherGadget,
                        g_isRussian ? L"Показать Календарь" : L"Show Calendar Gadget");
        }
    } else if (widgetId == WidgetCalendar) {
        AppendMenuW(hMenu, MF_STRING | MF_DEFAULT, CmdToggleCalSize,
                    g_calendarExpanded
                        ? (g_isRussian ? L"Меньший размер" : L"Smaller size")
                        : (g_isRussian ? L"Больший размер" : L"Larger size"));
        AppendMenuW(hMenu, MF_STRING, CmdJumpToday,
                    g_isRussian ? L"Перейти к сегодняшней дате" : L"Go to Today");

        if (!g_showClock) {
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu, MF_STRING, CmdShowOtherGadget,
                        g_isRussian ? L"Показать Часы" : L"Show Clock Gadget");
        }
    }

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, CmdCloseGadget,
                g_isRussian ? L"Закрыть гаджет" : L"Close gadget");

    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                             screenPt.x, screenPt.y, 0, hwnd, nullptr);
    DestroyMenu(hMenu);
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (cmd == CmdOptions) {
        OpenClockOptionsDialog();
    } else if (cmd >= CmdSkinBase && cmd < CmdSkinBase + 8) {
        g_clockSkin = cmd - CmdSkinBase;
        SaveStateToStorage();
        Render();
    } else if (cmd == CmdSecondHand) {
        g_showSeconds = !g_showSeconds;
        SaveStateToStorage();
        Render();
    } else if (cmd == CmdToggleCalSize) {
        g_calendarExpanded = !g_calendarExpanded;
        ClampWidgetToVirtualScreen(WidgetCalendar);
        SaveStateToStorage();
        Render();
    } else if (cmd == CmdJumpToday) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        g_calViewYear = st.wYear;
        g_calViewMonth = st.wMonth;
        g_calSelectedYear = st.wYear;
        g_calSelectedMonth = st.wMonth;
        g_calSelectedDay = st.wDay;
        Render();
    } else if (cmd == CmdShowOtherGadget) {
        if (widgetId == WidgetClock) g_showCalendar = true;
        else g_showClock = true;
        SaveStateToStorage();
        Render();
    } else if (cmd == CmdCloseGadget) {
        if (widgetId == WidgetClock) g_showClock = false;
        else g_showCalendar = false;
        g_hoveredWidget = -1;
        SaveStateToStorage();
        Render();
    }
}

// ── Per-Widget Window Procedure ──────────────────────────────────────────────

int GetWidgetIdFromHwnd(HWND hwnd) {
    if (hwnd == g_clockHwnd) return WidgetClock;
    if (hwnd == g_calendarHwnd) return WidgetCalendar;
    return static_cast<int>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}

LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int widgetId = GetWidgetIdFromHwnd(hwnd);

    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            widgetId = static_cast<int>(reinterpret_cast< INT_PTR >(cs->lpCreateParams));
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, widgetId);
            if (widgetId == WidgetClock) {
                g_clockHwnd = hwnd;
                SetTimer(hwnd, kTickTimer, 1000, nullptr);
            } else {
                g_calendarHwnd = hwnd;
            }
            RenderWidget(widgetId);
            return 0;
        }

        case WM_TIMER: {
            if (wParam == kTickTimer) {
                bool dayRolledOver = CheckAndAdvanceMidnightRollover();
                SYSTEMTIME st = GetClockTime();
                if (g_showSeconds || static_cast<int>(st.wMinute) != g_lastRenderedMinute) {
                    g_lastRenderedMinute = st.wMinute;
                    RenderWidget(WidgetClock);
                }
                if (dayRolledOver) {
                    RenderWidget(WidgetCalendar);
                }
                return 0;
            }
            return 0;
        }

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED: {
            UpdateEffectiveScale();
            ClampWidgetToVirtualScreen(WidgetClock);
            ClampWidgetToVirtualScreen(WidgetCalendar);
            Render();
            return 0;
        }

        case kSettingsChangedMessage: {
            LoadStateAndSettings(true);
            Render();
            return 0;
        }

        case WM_WINDOWPOSCHANGING: {
            // Prevent Win+D (Show Desktop) from hiding visible desktop widgets
            WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            bool shouldBeVisible = (widgetId == WidgetClock) ? g_showClock : g_showCalendar;
            if (wp && shouldBeVisible && (wp->flags & SWP_HIDEWINDOW)) {
                wp->flags &= ~SWP_HIDEWINDOW;
            }
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

        case WM_NCHITTEST: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &pt);
            int btn = HoverBtnNone;
            int hit = HitTestWidgetLocal(
                widgetId,
                PointF(static_cast<float>(pt.x), static_cast<float>(pt.y)),
                &btn, nullptr);
            return (hit >= 0) ? HTCLIENT : HTTRANSPARENT;
        }

        case WM_SETCURSOR: {
            if (g_hoveredWidget == widgetId) {
                if (g_hoveredBtn == HoverBtnClose || g_hoveredBtn == HoverBtnSizeOrOpts ||
                    g_hoveredBtn == CalBtnPrevMonth || g_hoveredBtn == CalBtnNextMonth ||
                    g_hoveredBtn == CalBtnDayCell) {
                    SetCursor(LoadCursor(nullptr, IDC_HAND));
                    return TRUE;
                }
                if (g_dragWidget == widgetId || g_hoveredBtn == HoverBtnDrag) {
                    SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                    return TRUE;
                }
            }
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return TRUE;
        }

        case WM_LBUTTONDOWN: {
            PointF localPt(static_cast<float>(GET_X_LPARAM(lParam)),
                           static_cast<float>(GET_Y_LPARAM(lParam)));
            int btn = HoverBtnNone;
            int dayNum = 0;
            int hit = HitTestWidgetLocal(widgetId, localPt, &btn, &dayNum);

            if (hit < 0) return 0;

            // Handle immediate button clicks
            if (btn == HoverBtnClose) {
                if (widgetId == WidgetClock) g_showClock = false;
                else if (widgetId == WidgetCalendar) g_showCalendar = false;
                g_hoveredWidget = -1;
                g_hoveredBtn = HoverBtnNone;
                SaveStateToStorage();
                RenderWidget(widgetId);
                return 0;
            }
            if (btn == HoverBtnSizeOrOpts) {
                if (widgetId == WidgetClock) {
                    OpenClockOptionsDialog();
                } else if (widgetId == WidgetCalendar) {
                    g_calendarExpanded = !g_calendarExpanded;
                    ClampWidgetToVirtualScreen(WidgetCalendar);
                    SaveStateToStorage();
                    RenderWidget(WidgetCalendar);
                }
                return 0;
            }
            if (widgetId == WidgetCalendar && btn == CalBtnPrevMonth) {
                if (--g_calViewMonth < 1) {
                    g_calViewMonth = 12;
                    --g_calViewYear;
                }
                RenderWidget(WidgetCalendar);
                return 0;
            }
            if (widgetId == WidgetCalendar && btn == CalBtnNextMonth) {
                if (++g_calViewMonth > 12) {
                    g_calViewMonth = 1;
                    ++g_calViewYear;
                }
                RenderWidget(WidgetCalendar);
                return 0;
            }
            if (widgetId == WidgetCalendar && btn == CalBtnDayCell && dayNum > 0) {
                g_calSelectedYear = g_calViewYear;
                g_calSelectedMonth = g_calViewMonth;
                g_calSelectedDay = dayNum;
                RenderWidget(WidgetCalendar);
                return 0;
            }

            // Start dragging the widget across any monitor
            POINT screenPt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ClientToScreen(hwnd, &screenPt);
            g_dragWidget = widgetId;
            g_dragStartScreen = PointF(static_cast<float>(screenPt.x), static_cast<float>(screenPt.y));
            g_dragMoved = false;
            PointF currentPos = (widgetId == WidgetClock) ? g_clockPos : g_calendarPos;
            g_dragOffset = PointF(g_dragStartScreen.X - currentPos.X, g_dragStartScreen.Y - currentPos.Y);
            SetCapture(hwnd);
            RenderWidget(widgetId);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!g_trackingMouse[widgetId]) {
                TRACKMOUSEEVENT tme = {};
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
                g_trackingMouse[widgetId] = true;
            }

            if (g_dragWidget == widgetId && (wParam & MK_LBUTTON)) {
                POINT screenPt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ClientToScreen(hwnd, &screenPt);
                PointF pt(static_cast<float>(screenPt.x), static_cast<float>(screenPt.y));
                if (std::fabs(pt.X - g_dragStartScreen.X) > 2.0f ||
                    std::fabs(pt.Y - g_dragStartScreen.Y) > 2.0f) {
                    g_dragMoved = true;
                }
                float oldScale = GetWidgetScale(widgetId);
                if (widgetId == WidgetClock) {
                    g_clockPos = PointF(pt.X - g_dragOffset.X, pt.Y - g_dragOffset.Y);
                    ClampWidgetToVirtualScreen(WidgetClock);
                } else {
                    g_calendarPos = PointF(pt.X - g_dragOffset.X, pt.Y - g_dragOffset.Y);
                    ClampWidgetToVirtualScreen(WidgetCalendar);
                }
                UpdateEffectiveScale();
                float newScale = GetWidgetScale(widgetId);
                if ( std::fabs(newScale - oldScale) > 0.001f ) {
                    RenderWidget(widgetId);
                } else {
                    PointF wPos = (widgetId == WidgetClock) ? g_clockPos : g_calendarPos;
                    SetWindowPos(hwnd, HWND_BOTTOM,
                                 static_cast<int>(std::round(wPos.X)),
                                 static_cast<int>(std::round(wPos.Y)),
                                 0, 0,
                                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
                }
                return 0;
            }

            PointF localPt(static_cast<float>(GET_X_LPARAM(lParam)),
                           static_cast<float>(GET_Y_LPARAM(lParam)));
            int btn = HoverBtnNone;
            int hit = HitTestWidgetLocal(widgetId, localPt, &btn, nullptr);
            int prevHoveredWidget = g_hoveredWidget;
            int prevHoveredBtn = g_hoveredBtn;

            if (hit >= 0) {
                g_hoveredWidget = widgetId;
                g_hoveredBtn = btn;
            } else if (g_hoveredWidget == widgetId) {
                g_hoveredWidget = -1;
                g_hoveredBtn = HoverBtnNone;
            }

            if (g_hoveredWidget != prevHoveredWidget || g_hoveredBtn != prevHoveredBtn) {
                if (prevHoveredWidget >= 0 && prevHoveredWidget != widgetId) {
                    RenderWidget(prevHoveredWidget);
                }
                RenderWidget(widgetId);
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            g_trackingMouse[widgetId] = false;
            if (g_hoveredWidget == widgetId) {
                g_hoveredWidget = -1;
                g_hoveredBtn = HoverBtnNone;
                RenderWidget(widgetId);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            bool wasMoved = g_dragMoved;
            int dragged = g_dragWidget;
            g_dragWidget = -1;
            g_dragMoved = false;
            ReleaseCapture();
            if (wasMoved && dragged >= 0) {
                SaveStateToStorage();
            }
            RenderWidget(widgetId);
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            PointF localPt(static_cast<float>(GET_X_LPARAM(lParam)),
                           static_cast<float>(GET_Y_LPARAM(lParam)));
            int btn = HoverBtnNone;
            int hit = HitTestWidgetLocal(widgetId, localPt, &btn, nullptr);
            if (hit == WidgetCalendar && btn == HoverBtnNone) {
                SYSTEMTIME st;
                GetLocalTime(&st);
                g_calViewYear = st.wYear;
                g_calViewMonth = st.wMonth;
                g_calSelectedYear = st.wYear;
                g_calSelectedMonth = st.wMonth;
                g_calSelectedDay = st.wDay;
                g_calendarExpanded = !g_calendarExpanded;
                ClampWidgetToVirtualScreen(WidgetCalendar);
                SaveStateToStorage();
                RenderWidget(WidgetCalendar);
                return 0;
            }
            if (hit == WidgetClock && btn == HoverBtnNone) {
                OpenClockOptionsDialog();
                return 0;
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            PointF localPt(static_cast<float>(GET_X_LPARAM(lParam)),
                           static_cast<float>(GET_Y_LPARAM(lParam)));
            int btn = HoverBtnNone;
            int hit = HitTestWidgetLocal(widgetId, localPt, &btn, nullptr);
            if (hit >= 0) {
                POINT menuPt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ClientToScreen(hwnd, &menuPt);
                ShowGadgetContextMenu(hwnd, widgetId, menuPt);
            }
            return 0;
        }

        case WM_CAPTURECHANGED: {
            if (g_dragMoved && g_dragWidget >= 0) {
                SaveStateToStorage();
            }
            g_dragWidget = -1;
            g_dragMoved = false;
            RenderWidget(widgetId);
            return 0;
        }

        case WM_QUERYENDSESSION:
            SaveStateToStorage();
            return TRUE;

        case WM_ENDSESSION:
            if (wParam) SaveStateToStorage();
            break;

        case WM_DESTROY:
            SaveStateToStorage();
            if (widgetId == WidgetClock) {
                KillTimer(hwnd, kTickTimer);
                if (g_optionsHwnd && IsWindow(g_optionsHwnd)) {
                    DestroyWindow(g_optionsHwnd);
                }
                DestroyRenderSurface(WidgetClock);
                g_clockHwnd = nullptr;
                PostQuitMessage(0);
            } else if (widgetId == WidgetCalendar) {
                DestroyRenderSurface(WidgetCalendar);
                g_calendarHwnd = nullptr;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ── UI Thread ────────────────────────────────────────────────────────────────

DWORD WINAPI UiThreadProc(LPVOID) {
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wcClock = {};
    wcClock.cbSize = sizeof(wcClock);
    wcClock.style = CS_DBLCLKS;
    wcClock.hInstance = instance;
    wcClock.lpfnWndProc = WidgetWndProc;
    wcClock.lpszClassName = kClockClassName;
    wcClock.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wcClock);

    WNDCLASSEXW wcCal = {};
    wcCal.cbSize = sizeof(wcCal);
    wcCal.style = CS_DBLCLKS;
    wcCal.hInstance = instance;
    wcCal.lpfnWndProc = WidgetWndProc;
    wcCal.lpszClassName = kCalendarClassName;
    wcCal.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wcCal);

    WNDCLASSEXW wcOpt = {};
    wcOpt.cbSize = sizeof(wcOpt);
    wcOpt.style = CS_HREDRAW | CS_VREDRAW;
    wcOpt.hInstance = instance;
    wcOpt.lpfnWndProc = OptionsWndProc;
    wcOpt.lpszClassName = kOptionsClassName;
    wcOpt.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcOpt.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
    RegisterClassExW(&wcOpt);

    SizeF clockSz = GetWidgetWindowSize(WidgetClock);
    g_clockHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClockClassName,
        L"Windows 7 Clock Widget",
        WS_POPUP,
        static_cast<int>(std::round(g_clockPos.X)),
        static_cast<int>(std::round(g_clockPos.Y)),
        static_cast<int>(clockSz.Width),
        static_cast<int>(clockSz.Height),
        nullptr, nullptr, instance,
        reinterpret_cast<LPVOID>(static_cast<INT_PTR>(WidgetClock)));

    SizeF calSz = GetWidgetWindowSize(WidgetCalendar);
    g_calendarHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kCalendarClassName,
        L"Windows 7 Calendar Widget",
        WS_POPUP,
        static_cast<int>(std::round(g_calendarPos.X)),
        static_cast<int>(std::round(g_calendarPos.Y)),
        static_cast<int>(calSz.Width),
        static_cast<int>(calSz.Height),
        nullptr, nullptr, instance,
        reinterpret_cast<LPVOID>(static_cast<INT_PTR>(WidgetCalendar)));

    Render();

    MSG msg;
    while (!g_unloading && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (g_optionsHwnd && IsDialogMessageW(g_optionsHwnd, &msg)) {
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_optionsHwnd && IsWindow(g_optionsHwnd)) DestroyWindow(g_optionsHwnd);
    if (g_calendarHwnd && IsWindow(g_calendarHwnd)) DestroyWindow(g_calendarHwnd);
    if (g_clockHwnd && IsWindow(g_clockHwnd)) DestroyWindow(g_clockHwnd);
    UnregisterClassW(kOptionsClassName, instance);
    UnregisterClassW(kCalendarClassName, instance);
    UnregisterClassW(kClockClassName, instance);
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

} // namespace

// ── Windhawk Tool Mod Entry Points ───────────────────────────────────────────

BOOL WhTool_ModInit() {
    g_unloading = false;
    LoadStateAndSettings(false);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModUninit() {
    g_unloading = true;
    if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    if (g_clockHwnd) PostMessageW(g_clockHwnd, kSettingsChangedMessage, 0, 0);
    else LoadStateAndSettings(true);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
