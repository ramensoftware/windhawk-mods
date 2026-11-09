// ==WindhawkMod==
// @id              win7-desktop-widgets
// @name            Windows 7 Desktop Widgets
// @description     Authentic Windows 7 Clock, Calendar & CPU Meter desktop gadgets. Features 8 classic vector clock skins, expandable calendar, real-time dual CPU/RAM meters, and full multi-monitor support.
// @version         1.1.0
// @author          dekvas
// @github          https://github.com/dekvas
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -lshell32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Screenshot](https://i.ibb.co.com/Y7GmJqwc/windhawk-XU7-EJ31-Wg4.png) 

# Windows 7 Desktop Widgets

A faithful, lightweight, and modern recreation of the beloved Windows 7 desktop gadgets — bringing back the classic **Clock**, **Calendar**, and **CPU & RAM Meter** directly to your desktop on Windows 10 and Windows 11.

---

### What's Included:

- 🕒 **Classic Windows 7 Clock**:
  - All **8 original Windows 7 clock skins** (Traditional, Modern, Chronometer, Diner, Daisy, Novelty, Square, System).
  - Authentic options dialog (click the wrench icon on the hover bar or right-click -> **Options...**).
  - Optional smooth second hand, customizable clock label, and worldwide time zone selection.

- 📅 **Classic Windows 7 Calendar**:
  - Iconic spiral-bound desktop notebook.
  - Compact daily view showing today's date and day of the week.
  - Expandable monthly calendar grid (click the expand button or double-click the widget) with interactive date browsing.

- ⚡ **Classic Windows 7 CPU & RAM Meter**:
  - Dual analog speedometer gauges for real-time CPU and physical memory (RAM) usage.
  - Realistic damped needle physics with smooth acceleration and deceleration.
  - Integrated digital LCD percentage displays with crisp sub-pixel text.
  - Two authentic sizes: compact docked mode (`130×103`) and large undocked mode (`180×142`).

---

### Key Features & Advantages:

- 🎨 **100% Pure Vector GDI+ Rendering**:
  - Zero raster images or low-res photos! Every bezel, gradient, dial tick, glass reflection, and silicon chip is rendered natively in code using vector paths.
  - Remains razor-sharp on any monitor without blur or scaling artifacts.

- 🖥️ **Per-Monitor High-DPI & Multi-Monitor Support**:
  - Scales cleanly across mixed DPI environments (100%, 125%, 150%, 200%+).
  - Freely draggable across multiple monitors with automatic desktop edge clamping.

- 🚀 **Lightweight & High Performance**:
  - Runs in a dedicated, isolated Windhawk tool process — zero bloat and no heavy web frameworks (Electron/Chromium).
  - Animation timers automatically sleep when usage is steady, consuming negligible CPU and RAM in the background.

- 🖱️ **Authentic Windows 7 Hover UI**:
  - Hovering reveals the original Windows 7 side toolbar with Close, Resize/Options, and Drag buttons.
  - Rich right-click context menu to switch skins, toggle sizes, jump to today's date, or restore closed widgets.

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
  $description: Enable or disable the Windows 7 Clock widget on the desktop.

- showCalendar: true
  $name: Show Calendar Widget
  $description: Enable or disable the Windows 7 Calendar widget on the desktop.

- showCpuMeter: true
  $name: Show CPU Meter Widget
  $description: Enable or disable the Windows 7 CPU & RAM Meter widget on the desktop.

- clockSkin: skin1
  $name: Clock Skin (1 - 8)
  $description: Choose one of the 8 classic Windows 7 clock faces (can also be changed via the wrench button on the clock).
  $options:
  - skin1: 1 - Traditional
  - skin2: 2 - Modern
  - skin3: 3 - Chronometer
  - skin4: 4 - Diner
  - skin5: 5 - Daisy
  - skin6: 6 - Novelty
  - skin7: 7 - Square
  - skin8: 8 - System

- showSeconds: true
  $name: Show Second Hand
  $description: Display the red sweeping second hand on the clock.

- clockName: ""
  $name: Clock Name
  $description: Optional text displayed on the clock face.

- calendarExpanded: false
  $name: Calendar Expanded (Larger Size)
  $description: Show both the top monthly grid sheet and the bottom daily page.

- cpuMeterExpanded: false
  $name: CPU Meter Expanded (Larger Size)
  $description: Show the larger undocked version of the CPU & RAM Meter widget.

- widgetOpacity: 100
  $name: Widget Opacity (%)
  $description: Opacity from 20% to 100%.

- widgetScale: 100
  $name: Widget Scale (%)
  $description: Size scale in percent (e.g. 100, 125, 150).

- language: auto
  $name: Language
  $description: Widget interface language (Auto matches system language, or force English / Russian).
  $options:
  - auto: Auto (System Default)
  - en: English
  - ru: Russian
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>

using namespace Gdiplus;

namespace {

constexpr UINT_PTR kTickTimer = 1;
constexpr UINT_PTR kCpuPollTimer = 2;
constexpr UINT_PTR kCpuAnimTimer = 3;
constexpr UINT kSettingsChangedMessage = WM_APP + 1;
constexpr wchar_t kClockClassName[] = L"WindhawkWin7ClockWidget";
constexpr wchar_t kCalendarClassName[] = L"WindhawkWin7CalendarWidget";
constexpr wchar_t kCpuMeterClassName[] = L"WindhawkWin7CpuMeterWidget";
constexpr wchar_t kOptionsClassName[] = L"WindhawkWin7ClockOptionsDlg";

constexpr float kPi = 3.14159265358979323846f;
constexpr float kShadowPad = 6.0f;

enum WidgetId {
    WidgetClock = 0,
    WidgetCalendar = 1,
    WidgetCpuMeter = 2,
    WidgetCount = 3
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

std::atomic<HWND> g_clockHwnd{nullptr};
std::atomic<HWND> g_calendarHwnd{nullptr};
std::atomic<HWND> g_cpuHwnd{nullptr};
std::atomic<HWND> g_optionsHwnd{nullptr};
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
std::atomic<bool> g_unloading{false};
ULONG_PTR g_gdiplusToken = 0;

bool g_isRussian = false;
float g_userScale = 1.0f;
float g_clockScale = 1.0f;
float g_calendarScale = 1.0f;
float g_cpuScale = 1.0f;
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

// CPU Meter state
bool g_showCpuMeter = true;
bool g_cpuMeterExpanded = false;
PointF g_cpuPos = PointF(0, 0);
bool g_cpuTimesInitialized = false;
ULONGLONG g_prevIdleTime = 0;
ULONGLONG g_prevKernelTime = 0;
ULONGLONG g_prevUserTime = 0;
float g_targetCpuPct = 0.0f;
float g_targetRamPct = 0.0f;
float g_dispCpuPct = 0.0f;
float g_dispRamPct = 0.0f;
bool g_cpuAnimRunning = false;

// Interaction state
int g_hoveredWidget = -1;
int g_hoveredBtn = HoverBtnNone;
bool g_trackingMouse[WidgetCount] = {false, false, false};
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
    auto langSetting = WindhawkUtils::StringSetting::make(L"language");
    if (langSetting.get()) {
        if (wcscmp(langSetting.get(), L"ru") == 0) {
            g_isRussian = true;
            return;
        }
        if (wcscmp(langSetting.get(), L"en") == 0) {
            g_isRussian = false;
            return;
        }
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
    return MonthNameFull(month);
}

const wchar_t* MonthNameShort(int month) {
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
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);

    static GetDpiForWindow_t pGetDpiForWindow = []() -> GetDpiForWindow_t {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        return hUser32 ? reinterpret_cast<GetDpiForWindow_t>(
                             GetProcAddress(hUser32, "GetDpiForWindow"))
                       : nullptr;
    }();

    static GetDpiForMonitor_t pGetDpiForMonitor = []() -> GetDpiForMonitor_t {
        HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
        if (!hShcore) {
            hShcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        return hShcore ? reinterpret_cast<GetDpiForMonitor_t>(
                             GetProcAddress(hShcore, "GetDpiForMonitor"))
                       : nullptr;
    }();

    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (hMon && pGetDpiForMonitor) {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(pGetDpiForMonitor(hMon, 0 /*MDT_EFFECTIVE_DPI*/, &dpiX, &dpiY)) && dpiX > 0) {
            return dpiX / 96.0f;
        }
    }
    if (hwnd && IsWindow(hwnd) && pGetDpiForWindow) {
        UINT dpi = pGetDpiForWindow(hwnd);
        if (dpi > 0) return dpi / 96.0f;
    }
    HDC screenDc = GetDC(nullptr);
    int dpiX = 96;
    if (screenDc) {
        dpiX = GetDeviceCaps(screenDc, LOGPIXELSX);
        ReleaseDC(nullptr, screenDc);
    }
    return (dpiX > 0) ? (dpiX / 96.0f) : 1.0f;
}

HWND GetWidgetHwnd(int widgetId) {
    if (widgetId == WidgetClock) return g_clockHwnd.load();
    if (widgetId == WidgetCalendar) return g_calendarHwnd.load();
    return g_cpuHwnd.load();
}

bool IsWidgetVisibleFlag(int widgetId) {
    if (widgetId == WidgetClock) return g_showClock;
    if (widgetId == WidgetCalendar) return g_showCalendar;
    return g_showCpuMeter;
}

void SetWidgetVisibleFlag(int widgetId, bool visible) {
    if (widgetId == WidgetClock) g_showClock = visible;
    else if (widgetId == WidgetCalendar) g_showCalendar = visible;
    else if (widgetId == WidgetCpuMeter) g_showCpuMeter = visible;
}

PointF& GetWidgetPosRef(int widgetId) {
    if (widgetId == WidgetClock) return g_clockPos;
    if (widgetId == WidgetCalendar) return g_calendarPos;
    return g_cpuPos;
}

float GetWidgetScale(int widgetId) {
    if (widgetId == WidgetClock) return g_clockScale;
    if (widgetId == WidgetCalendar) return g_calendarScale;
    return g_cpuScale;
}

void UpdateEffectiveScale() {
    const float clockHalf = 71.0f * (g_clockScale > 0.1f ? g_clockScale : g_userScale);
    POINT clockPt = {
        static_cast<LONG>(std::round(g_clockPos.X + clockHalf)),
        static_cast<LONG>(std::round(g_clockPos.Y + clockHalf))
    };
    g_clockScale = g_userScale * GetMonitorDpiScaleForPoint(clockPt, g_clockHwnd.load());

    const float calHalf = 71.0f * (g_calendarScale > 0.1f ? g_calendarScale : g_userScale);
    POINT calPt = {
        static_cast<LONG>(std::round(g_calendarPos.X + calHalf)),
        static_cast<LONG>(std::round(g_calendarPos.Y + calHalf))
    };
    g_calendarScale = g_userScale * GetMonitorDpiScaleForPoint(calPt, g_calendarHwnd.load());

    const float cpuHalfX = (g_cpuMeterExpanded ? 96.0f : 71.0f) * (g_cpuScale > 0.1f ? g_cpuScale : g_userScale);
    const float cpuHalfY = (g_cpuMeterExpanded ? 77.0f : 58.0f) * (g_cpuScale > 0.1f ? g_cpuScale : g_userScale);
    POINT cpuPt = {
        static_cast<LONG>(std::round(g_cpuPos.X + cpuHalfX)),
        static_cast<LONG>(std::round(g_cpuPos.Y + cpuHalfY))
    };
    g_cpuScale = g_userScale * GetMonitorDpiScaleForPoint(cpuPt, g_cpuHwnd.load());
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

RectF SnapTextRect(float x, float y, float w, float h) {
    float rx = std::round(x);
    float ry = std::round(y);
    float rw = std::round(w);
    float rh = std::round(h);
    return RectF(rx, ry, std::max(1.0f, rw), std::max(1.0f, rh));
}

float SnapFontSize(float rawPx) {
    return std::max(8.0f, std::round(rawPx));
}

// Logical dimensions of widgets (at 100% scale, with kShadowPad = 6px padding):
// Clock:             130 x 130 body inside 166 x 144 window
// Calendar Small:    130 x 146 body inside 166 x 160 window
// Calendar Expanded: 130 x 290 body inside 166 x 304 window
// CPU Meter Small:   130 x 103 body inside 166 x 117 window
// CPU Meter Large:   180 x 142 body inside 216 x 156 window
// Hover bar width:   20px on the right edge of each widget

SizeF GetWidgetWindowSize(int widgetId) {
    const float s = GetWidgetScale(widgetId);
    if (widgetId == WidgetClock) {
        return SizeF(std::ceil((154.0f + kShadowPad * 2.0f) * s),
                     std::ceil((132.0f + kShadowPad * 2.0f) * s));
    } else if (widgetId == WidgetCalendar) {
        float h = g_calendarExpanded ? 292.0f : 148.0f;
        return SizeF(std::ceil((154.0f + kShadowPad * 2.0f) * s),
                     std::ceil((h + kShadowPad * 2.0f) * s));
    } else {
        float w = g_cpuMeterExpanded ? 204.0f : 154.0f;
        float h = g_cpuMeterExpanded ? 144.0f : 105.0f;
        return SizeF(std::ceil((w + kShadowPad * 2.0f) * s),
                     std::ceil((h + kShadowPad * 2.0f) * s));
    }
}

RectF GetWidgetLocalBodyRect(int widgetId) {
    const float s = GetWidgetScale(widgetId);
    const float pad = std::round(kShadowPad * s);
    if (widgetId == WidgetClock) {
        return RectF(pad, pad, 130.0f * s, 130.0f * s);
    } else if (widgetId == WidgetCalendar) {
        float h = g_calendarExpanded ? 290.0f : 146.0f;
        return RectF(pad, pad, 130.0f * s, h * s);
    } else {
        float w = g_cpuMeterExpanded ? 180.0f : 130.0f;
        float h = g_cpuMeterExpanded ? 142.0f : 103.0f;
        return RectF(pad, pad, w * s, h * s);
    }
}

RectF GetWidgetLocalHoverBarRect(int widgetId) {
    const float s = GetWidgetScale(widgetId);
    RectF body = GetWidgetLocalBodyRect(widgetId);
    float topOffset = (widgetId == WidgetCpuMeter) ? 5.0f * s : 4.0f * s;
    return RectF(body.X + body.Width + 1.0f * s, body.Y + topOffset, 20.0f * s, 56.0f * s);
}

RectF GetWidgetScreenBodyRect(int widgetId) {
    RectF local = GetWidgetLocalBodyRect(widgetId);
    PointF pos = GetWidgetPosRef(widgetId);
    return RectF(pos.X + local.X, pos.Y + local.Y, local.Width, local.Height);
}

void ClampWidgetToVirtualScreen(int widgetId, bool duringDrag = false) {
    RectF body = GetWidgetLocalBodyRect(widgetId);
    RectF bar = GetWidgetLocalHoverBarRect(widgetId);
    PointF& pos = GetWidgetPosRef(widgetId);

    RECT targetRc = {
        static_cast<LONG>(std::round(pos.X + body.X)),
        static_cast<LONG>(std::round(pos.Y + body.Y)),
        static_cast<LONG>(std::round(pos.X + body.X + body.Width)),
        static_cast<LONG>(std::round(pos.Y + body.Y + body.Height))
    };

    RECT bounds = GetFullVirtualScreenRect();
    if (!duringDrag) {
        HMONITOR hMon = MonitorFromRect(&targetRc, MONITOR_DEFAULTTONEAREST);
        if (hMon) {
            MONITORINFO mi = {};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(hMon, &mi)) {
                bounds = mi.rcWork;
            }
        }
    }

    float rightExtent = duringDrag ? (body.X + body.Width)
                                   : std::max(body.X + body.Width, bar.X + bar.Width);
    float minX = static_cast<float>(bounds.left) - body.X;
    float minY = static_cast<float>(bounds.top) - body.Y;
    float maxX = std::max(minX, static_cast<float>(bounds.right) - rightExtent);
    float maxY = std::max(minY, static_cast<float>(bounds.bottom) - (body.Y + body.Height));

    pos.X = ClampF(pos.X, minX, maxX);
    pos.Y = ClampF(pos.Y, minY, maxY);
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

// ── CPU & RAM Sampling + Smooth Needle Physics ───────────────────────────────

ULONGLONG FileTimeToUInt64(const FILETIME& ft) {
    return (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

void SampleCpuAndRamUsage(bool snapNeedlesImmediately = false) {
    // 1. Physical RAM usage
    MEMORYSTATUSEX mem = {};
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem) && mem.ullTotalPhys > 0) {
        double usedBytes = static_cast<double>(mem.ullTotalPhys - mem.ullAvailPhys);
        double ramPct = (usedBytes * 100.0) / static_cast<double>(mem.ullTotalPhys);
        g_targetRamPct = ClampF(static_cast<float>(ramPct), 0.0f, 100.0f);
    }

    // 2. System-wide CPU usage delta via GetSystemTimes
    FILETIME ftIdle = {}, ftKernel = {}, ftUser = {};
    if (GetSystemTimes(&ftIdle, &ftKernel, &ftUser)) {
        ULONGLONG idle = FileTimeToUInt64(ftIdle);
        ULONGLONG kernel = FileTimeToUInt64(ftKernel);
        ULONGLONG user = FileTimeToUInt64(ftUser);

        if (!g_cpuTimesInitialized) {
            g_prevIdleTime = idle;
            g_prevKernelTime = kernel;
            g_prevUserTime = user;
            g_targetCpuPct = 0.0f;
            g_cpuTimesInitialized = true;
        } else {
            ULONGLONG dIdle = idle - g_prevIdleTime;
            ULONGLONG dKernel = kernel - g_prevKernelTime;
            ULONGLONG dUser = user - g_prevUserTime;
            ULONGLONG dTotal = dKernel + dUser;

            g_prevIdleTime = idle;
            g_prevKernelTime = kernel;
            g_prevUserTime = user;

            if (dTotal > 0) {
                ULONGLONG dBusy = (dTotal > dIdle) ? (dTotal - dIdle) : 0ULL;
                double cpuPct = (static_cast<double>(dBusy) * 100.0) / static_cast<double>(dTotal);
                g_targetCpuPct = ClampF(static_cast<float>(cpuPct), 0.0f, 100.0f);
            }
        }
    }

    if (snapNeedlesImmediately) {
        g_dispCpuPct = g_targetCpuPct;
        g_dispRamPct = g_targetRamPct;
    } else if (HWND cpuHwnd = g_cpuHwnd.load(); cpuHwnd && IsWindow(cpuHwnd) && g_showCpuMeter) {
        if (std::fabs(g_dispCpuPct - g_targetCpuPct) > 0.15f ||
            std::fabs(g_dispRamPct - g_targetRamPct) > 0.15f) {
            if (!g_cpuAnimRunning) {
                SetTimer(cpuHwnd, kCpuAnimTimer, 33, nullptr);
                g_cpuAnimRunning = true;
            }
        }
    }
}

void SyncCpuMeterTimers() {
    HWND cpuHwnd = g_cpuHwnd.load();
    if (!cpuHwnd || !IsWindow(cpuHwnd)) return;

    if (g_showCpuMeter) {
        if (!g_cpuTimesInitialized) {
            SampleCpuAndRamUsage(true);
            // Quick initial delta after 250ms so CPU% populates almost immediately on launch
            SetTimer(cpuHwnd, kCpuPollTimer, 250, nullptr);
        } else {
            SetTimer(cpuHwnd, kCpuPollTimer, 1000, nullptr);
        }
    } else {
        KillTimer(cpuHwnd, kCpuPollTimer);
        if (g_cpuAnimRunning) {
            KillTimer(cpuHwnd, kCpuAnimTimer);
            g_cpuAnimRunning = false;
        }
        g_cpuTimesInitialized = false;
    }
}

// ── Persistence & Settings ───────────────────────────────────────────────────

void SaveStateToStorage() {
    wchar_t buf[128];
    swprintf_s(buf, L"%.1f,%.1f", g_clockPos.X, g_clockPos.Y);
    Wh_SetStringValue(L"clockPos", buf);

    swprintf_s(buf, L"%.1f,%.1f", g_calendarPos.X, g_calendarPos.Y);
    Wh_SetStringValue(L"calendarPos", buf);

    swprintf_s(buf, L"%.1f,%.1f", g_cpuPos.X, g_cpuPos.Y);
    Wh_SetStringValue(L"cpuPos", buf);

    Wh_SetIntValue(L"savedClockSkin", g_clockSkin);
    Wh_SetIntValue(L"savedShowSeconds", g_showSeconds ? 1 : 0);
    Wh_SetIntValue(L"savedClockTz", g_clockTzIndex);
    Wh_SetStringValue(L"savedClockName", g_clockName);
    Wh_SetIntValue(L"savedCalExpanded", g_calendarExpanded ? 1 : 0);
    Wh_SetIntValue(L"savedCpuExpanded", g_cpuMeterExpanded ? 1 : 0);
    Wh_SetIntValue(L"savedShowClock", g_showClock ? 1 : 0);
    Wh_SetIntValue(L"savedShowCalendar", g_showCalendar ? 1 : 0);
    Wh_SetIntValue(L"savedShowCpuMeter", g_showCpuMeter ? 1 : 0);
}

void ApplyDefaultPositions() {
    RECT workArea = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    POINT initPt = {workArea.right - 100, workArea.top + 100};
    const float s = g_userScale * GetMonitorDpiScaleForPoint(initPt);
    float rightEdge = static_cast<float>(workArea.right) - 161.0f * s;
    float topEdge = static_cast<float>(workArea.top) + 14.0f * s;

    g_clockPos = PointF(rightEdge, topEdge);
    g_calendarPos = PointF(rightEdge, topEdge + 148.0f * s);
    g_cpuPos = PointF(rightEdge, topEdge + 310.0f * s);
}

void AdjustCpuPosForSizeToggle(bool nowExpanded) {
    const float prevHalfW = (nowExpanded ? 71.0f : 96.0f);
    const float s = (g_cpuScale > 0.1f) ? g_cpuScale : g_userScale;
    POINT centerPt = {
        static_cast<LONG>(std::round(g_cpuPos.X + prevHalfW * s)),
        static_cast<LONG>(std::round(g_cpuPos.Y + 58.0f * s))
    };
    const float effS = g_userScale * GetMonitorDpiScaleForPoint(centerPt, g_cpuHwnd.load());

    HMONITOR hMon = MonitorFromPoint(centerPt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        float monMidX = (mi.rcWork.left + mi.rcWork.right) * 0.5f;
        if (static_cast<float>(centerPt.x) < monMidX) {
            // Left half of monitor: keep left edge anchored so repeated toggles never drift rightward
            return;
        }
    }
    // Right half of monitor: keep right edge & hover toolbar anchored
    g_cpuPos.X += nowExpanded ? (-50.0f * effS) : (50.0f * effS);
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
    bool cfgShowCpuMeter = Wh_GetIntSetting(L"showCpuMeter") != 0;

    int cfgSkin = 0;
    {
        auto skinSetting = WindhawkUtils::StringSetting::make(L"clockSkin");
        if (skinSetting.get()) {
            if (wcsncmp(skinSetting.get(), L"skin", 4) == 0 &&
                skinSetting.get()[4] >= L'1' && skinSetting.get()[4] <= L'8') {
                cfgSkin = skinSetting.get()[4] - L'1';
            }
        }
    }

    bool cfgShowSeconds = Wh_GetIntSetting(L"showSeconds") != 0;
    bool cfgCalExpanded = Wh_GetIntSetting(L"calendarExpanded") != 0;
    bool cfgCpuExpanded = Wh_GetIntSetting(L"cpuMeterExpanded") != 0;

    wchar_t cfgClockName[64] = L"";
    {
        auto nameSetting = WindhawkUtils::StringSetting::make(L"clockName");
        if (nameSetting.get()) {
            wcsncpy_s(cfgClockName, nameSetting.get(), _TRUNCATE);
        }
    }

    // Compare against the last known Windhawk settings so widget-side changes
    // survive unrelated edits in the Windhawk settings UI.
    int prevCfgShowClock = Wh_GetIntValue(L"prevCfgShowClock", -1);
    int prevCfgShowCal   = Wh_GetIntValue(L"prevCfgShowCalendar", -1);
    int prevCfgShowCpu   = Wh_GetIntValue(L"prevCfgShowCpuMeter", -1);
    int prevCfgSkin      = Wh_GetIntValue(L"prevCfgClockSkin", -1);
    int prevCfgSec       = Wh_GetIntValue(L"prevCfgShowSeconds", -1);
    int prevCfgExp       = Wh_GetIntValue(L"prevCfgCalExpanded", -1);
    int prevCfgCpuExp    = Wh_GetIntValue(L"prevCfgCpuExpanded", -1);
    wchar_t prevCfgName[64] = L"";
    bool hasPrevCfgName  = Wh_GetStringValue(L"prevCfgClockName", prevCfgName, ARRAYSIZE(prevCfgName)) > 0 ||
                           Wh_GetIntValue(L"prevCfgNameInit", 0) != 0;

    // Load saved runtime state first
    int savedShowClock = Wh_GetIntValue(L"savedShowClock", -1);
    g_showClock = (savedShowClock != -1) ? (savedShowClock != 0) : cfgShowClock;

    int savedShowCal = Wh_GetIntValue(L"savedShowCalendar", -1);
    g_showCalendar = (savedShowCal != -1) ? (savedShowCal != 0) : cfgShowCalendar;

    int savedShowCpu = Wh_GetIntValue(L"savedShowCpuMeter", -1);
    g_showCpuMeter = (savedShowCpu != -1) ? (savedShowCpu != 0) : cfgShowCpuMeter;

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

    int savedCpuExp = Wh_GetIntValue(L"savedCpuExpanded", -1);
    bool prevCpuExpandedRuntime = g_cpuMeterExpanded;
    g_cpuMeterExpanded = (savedCpuExp != -1) ? (savedCpuExp != 0) : cfgCpuExpanded;

    // Apply any Windhawk setting whose value changed since the last load,
    // including changes made while the mod wasn't running.
    if (prevCfgShowClock == -1 || prevCfgShowClock != (cfgShowClock ? 1 : 0)) {
        g_showClock = cfgShowClock;
    }
    if (prevCfgShowCal == -1 || prevCfgShowCal != (cfgShowCalendar ? 1 : 0)) {
        g_showCalendar = cfgShowCalendar;
    }
    if (prevCfgShowCpu == -1 || prevCfgShowCpu != (cfgShowCpuMeter ? 1 : 0)) {
        g_showCpuMeter = cfgShowCpuMeter;
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
    if (prevCfgCpuExp == -1 || prevCfgCpuExp != (cfgCpuExpanded ? 1 : 0)) {
        g_cpuMeterExpanded = cfgCpuExpanded;
    }

    if (!fromWindhawkSettingsChange) {
        // If all widgets were closed in the previous session, restore from settings
        // so the user isn't left with zero visible widgets on startup.
        if (!g_showClock && !g_showCalendar && !g_showCpuMeter &&
            (cfgShowClock || cfgShowCalendar || cfgShowCpuMeter)) {
            g_showClock = cfgShowClock;
            g_showCalendar = cfgShowCalendar;
            g_showCpuMeter = cfgShowCpuMeter;
        }
    }

    // Record current Windhawk settings snapshot & runtime state
    Wh_SetIntValue(L"prevCfgShowClock", cfgShowClock ? 1 : 0);
    Wh_SetIntValue(L"prevCfgShowCalendar", cfgShowCalendar ? 1 : 0);
    Wh_SetIntValue(L"prevCfgShowCpuMeter", cfgShowCpuMeter ? 1 : 0);
    Wh_SetIntValue(L"prevCfgClockSkin", cfgSkin);
    Wh_SetIntValue(L"prevCfgShowSeconds", cfgShowSeconds ? 1 : 0);
    Wh_SetIntValue(L"prevCfgCalExpanded", cfgCalExpanded ? 1 : 0);
    Wh_SetIntValue(L"prevCfgCpuExpanded", cfgCpuExpanded ? 1 : 0);
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
        if (Wh_GetStringValue(L"cpuPos", buf, ARRAYSIZE(buf)) > 0 &&
            swscanf_s(buf, L"%f,%f", &x, &y) == 2) {
            g_cpuPos = PointF(x, y);
            if (savedCpuExp != -1 && (savedCpuExp != 0) != g_cpuMeterExpanded) {
                AdjustCpuPosForSizeToggle(g_cpuMeterExpanded);
            }
        } else if (g_cpuMeterExpanded) {
            AdjustCpuPosForSizeToggle(true);
        }
        g_positionsInitialized = true;
    } else if (fromWindhawkSettingsChange && g_cpuMeterExpanded != prevCpuExpandedRuntime) {
        AdjustCpuPosForSizeToggle(g_cpuMeterExpanded);
    }

    UpdateEffectiveScale();
    ClampWidgetToVirtualScreen(WidgetClock);
    ClampWidgetToVirtualScreen(WidgetCalendar);
    ClampWidgetToVirtualScreen(WidgetCpuMeter);
    SyncCalendarWithTodayIfUnset();
    SyncCpuMeterTimers();
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
    // Middle: Options (Clock) or Larger/Smaller (Calendar & CPU Meter) (height 18)
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

    // Draw Middle Button: Wrench (Clock) or Larger/Smaller box (Calendar & CPU Meter)
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
            bool isExpanded = (widgetId == WidgetCalendar) ? g_calendarExpanded : g_cpuMeterExpanded;
            RectF boxRc(cx - 5.0f * s, cy - 4.5f * s, 10.0f * s, 9.0f * s);
            Pen boxPen(Color(245, 240, 248, 255), 1.3f * s);
            g.DrawRectangle(&boxPen, boxRc.X, boxRc.Y, boxRc.Width, boxRc.Height);

            Pen arrowPen(Color(255, 255, 255, 255), 1.4f * s);
            if (!isExpanded) {
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

// Vector stroke renderer for the authentic Windows 7 Traditional ('trad.png') numerals.
// In original trad.png, the digit '1' is a simple straight vertical bar with NO top hook
// and NO bottom serif, and all digits are tall, clean, warm-charcoal strokes.
void DrawTradDigitStroke(Graphics& g, int digit, float cx, float cy, float h, const Pen& pen) {
    float w = h * 0.42f;
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    float top = cy - hh;
    float bot = cy + hh;
    float left = cx - hw;
    float right = cx + hw;

    switch (digit) {
        case 0: {
            RectF rc(left, top, w, h);
            g.DrawEllipse(&pen, rc);
            break;
        }
        case 1: {
            g.DrawLine(&pen, cx, top, cx, bot);
            break;
        }
        case 2: {
            GraphicsPath p;
            p.AddArc(left, top, w, h * 0.48f, 190.0f, 195.0f);
            p.AddLine(PointF(right - w * 0.04f, cy - hh * 0.14f), PointF(left, bot));
            p.AddLine(PointF(left, bot), PointF(right, bot));
            g.DrawPath(&pen, &p);
            break;
        }
        case 3: {
            // Draw two separate open loops meeting cleanly at the middle waist (no connecting slash)
            g.DrawArc(&pen, left + w * 0.04f, top, w * 0.92f, h * 0.48f, 195.0f, 255.0f);
            g.DrawArc(&pen, left, cy - h * 0.02f, w, h * 0.52f, 270.0f, 255.0f);
            break;
        }
        case 4: {
            float barY = top + h * 0.68f;
            float stemX = left + w * 0.72f;
            g.DrawLine(&pen, stemX, bot, stemX, top);
            GraphicsPath p;
            p.AddLine(PointF(stemX, top), PointF(left - w * 0.06f, barY));
            p.AddLine(PointF(left - w * 0.06f, barY), PointF(right + w * 0.12f, barY));
            g.DrawPath(&pen, &p);
            break;
        }
        case 5: {
            GraphicsPath p;
            p.AddLine(PointF(right - w * 0.05f, top), PointF(left + w * 0.06f, top));
            p.AddLine(PointF(left + w * 0.06f, top), PointF(left + w * 0.02f, cy - hh * 0.06f));
            p.AddArc(left, cy - hh * 0.16f, w, h * 0.58f, 225.0f, 290.0f);
            g.DrawPath(&pen, &p);
            break;
        }
        case 6: {
            g.DrawEllipse(&pen, left, cy - hh * 0.08f, w, h * 0.54f);
            g.DrawArc(&pen, left, top, w, h, 145.0f, 150.0f);
            break;
        }
        case 7: {
            GraphicsPath p;
            p.AddLine(PointF(left, top), PointF(right, top));
            p.AddLine(PointF(right, top), PointF(left + w * 0.24f, bot));
            g.DrawPath(&pen, &p);
            break;
        }
        case 8: {
            g.DrawEllipse(&pen, left + w * 0.06f, top, w * 0.88f, h * 0.46f);
            g.DrawEllipse(&pen, left, cy - h * 0.04f, w, h * 0.54f);
            break;
        }
        case 9: {
            g.DrawEllipse(&pen, left, top, w, h * 0.54f);
            g.DrawArc(&pen, left, top, w, h, -35.0f, 150.0f);
            break;
        }
    }
}

void DrawTradNumber(Graphics& g, int num, float cx, float cy, float u, const Color& col) {
    float h = 11.2f * u;
    Pen pen(col, 1.75f * u);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    pen.SetLineJoin(LineJoinRound);

    if (num < 10) {
        DrawTradDigitStroke(g, num, cx, cy, h, pen);
    } else if (num == 10) {
        // In authentic trad.png, the outer '1' of '10' is slightly shorter at the top (h=9.6)
        // and aligned along the bottom so it follows the circular minute track!
        DrawTradDigitStroke(g, 1, cx - 3.6f * u, cy + 0.8f * u, 9.6f * u, pen);
        DrawTradDigitStroke(g, 0, cx + 1.8f * u, cy, h, pen);
    } else if (num == 11) {
        // In authentic trad.png, the left '1' of '11' is shorter at the top (h=9.6)
        DrawTradDigitStroke(g, 1, cx - 2.8f * u, cy + 0.8f * u, 9.6f * u, pen);
        DrawTradDigitStroke(g, 1, cx + 2.8f * u, cy, h, pen);
    } else if (num == 12) {
        DrawTradDigitStroke(g, 1, cx - 3.4f * u, cy, 10.6f * u, pen);
        DrawTradDigitStroke(g, 2, cx + 1.8f * u, cy, 10.6f * u, pen);
    }
}

// Helper to draw a rotated hand in local (130x130) units where -Y is 12 o'clock (up).
void FillRotatedHandPolygon(Graphics& g, float cx, float cy, float angleRad,
                            const PointF* localPts, int count, const Brush& brush,
                            float shadowDx = 0.0f, float shadowDy = 0.0f, BYTE shadowAlpha = 0) {
    // localPts has (x, y) with y < 0 pointing toward the tip.
    // Since angleRad = (deg - 90) * pi / 180, standard screen up (0, -1) maps to:
    // screenX = cx + x * (-sinA) + (-y) * cosA = cx - x * sinA - y * cosA
    // screenY = cy + x * cosA + (-y) * sinA    = cy + x * cosA - y * sinA
    float cosA = std::cos(angleRad);
    float sinA = std::sin(angleRad);
    PointF worldPts[12];
    PointF shPts[12];
    int n = std::min(count, 12);
    for (int i = 0; i < n; ++i) {
        float lx = localPts[i].X;
        float ly = localPts[i].Y;
        float wx = cx - lx * sinA - ly * cosA;
        float wy = cy + lx * cosA - ly * sinA;
        worldPts[i] = PointF(wx, wy);
        shPts[i] = PointF(wx + shadowDx, wy + shadowDy);
    }
    if (shadowAlpha > 0) {
        SolidBrush shBrush(Color(shadowAlpha, 0, 0, 0));
        g.FillPolygon(&shBrush, shPts, n);
    }
    g.FillPolygon(&brush, worldPts, n);
}

void RenderClockFaceAt(Graphics& g, float cx, float cy, float radius, int skinIndex,
                       const SYSTEMTIME& st, bool showSeconds, const wchar_t* customName,
                       bool drawDropShadow) {
    int skin = ((skinIndex % 8) + 8) % 8;

    // Unit conversion: in original 130x130 PNGs, outer bezel radius is 62.5px.
    const float u = radius / 62.5f;

    float sec = static_cast<float>(st.wSecond);
    float min = st.wMinute + sec / 60.0f;
    float hr  = (st.wHour % 12) + min / 60.0f;

    float hrAngle  = (hr  * 30.0f - 90.0f) * kPi / 180.0f;
    float minAngle = (min * 6.0f  - 90.0f) * kPi / 180.0f;
    float secAngle = (sec * 6.0f  - 90.0f) * kPi / 180.0f;

    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);

    // 1. Soft drop shadow (matched to each skin's silhouette)
    if (drawDropShadow && skin != 4 && skin != 5 && skin != 6) {
        for (int i = 4; i >= 1; --i) {
            float expand = i * 1.15f * u;
            float offsetY = i * 0.95f * u;
            SolidBrush shadowBrush(Color(static_cast<BYTE>(13 * (5 - i)), 0, 0, 0));
            g.FillEllipse(&shadowBrush,
                          cx - radius - expand,
                          cy - radius - expand + offsetY,
                          (radius + expand) * 2.0f,
                          (radius + expand) * 2.0f);
        }
    }

    switch (skin) {
        // ─────────────────────────────────────────────────────────────────────
        // SKIN 0: TRADITIONAL ('trad.png', 'trad_h/m/s/dot.png')
        // ─────────────────────────────────────────────────────────────────────
        case 0: {
            float rOuter = 62.5f * u;
            float rDial  = 55.5f * u;

            // Outer dark matte charcoal bezel (r = 55.5 .. 62.5)
            LinearGradientBrush bezelOuter(
                PointF(cx, cy - rOuter), PointF(cx, cy + rOuter),
                Color(255, 76, 75, 73), Color(255, 36, 36, 35));
            g.FillEllipse(&bezelOuter, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);

            float rBezelBody = 61.3f * u;
            LinearGradientBrush bezelBody(
                PointF(cx, cy - rBezelBody), PointF(cx, cy + rBezelBody),
                Color(255, 58, 57, 56), Color(255, 39, 39, 38));
            g.FillEllipse(&bezelBody, cx - rBezelBody, cy - rBezelBody, rBezelBody * 2.0f, rBezelBody * 2.0f);

            Pen outerCrispRim(Color(240, 26, 26, 25), 0.9f * u);
            g.DrawEllipse(&outerCrispRim, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);

            // Warm cream dial (r = 55.5): RGB(255,253,245) -> RGB(242,240,225) -> RGB(227,224,207)
            LinearGradientBrush creamDial(
                PointF(cx, cy - rDial), PointF(cx, cy + rDial),
                Color(255, 255, 253, 245), Color(255, 227, 224, 207));
            Color dialColors[3] = {
                Color(255, 255, 253, 245),
                Color(255, 242, 240, 225),
                Color(255, 227, 224, 207)
            };
            REAL dialPos[3] = {0.0f, 0.52f, 1.0f};
            creamDial.SetInterpolationColors(dialColors, dialPos, 3);
            g.FillEllipse(&creamDial, cx - rDial, cy - rDial, rDial * 2.0f, rDial * 2.0f);

            // Soft inner bezel shadow onto the edge of the cream dial (r = 51..55.5)
            for (int i = 0; i < 4; ++i) {
                float sr = rDial - i * 0.9f * u;
                Pen shPen(Color(static_cast<BYTE>(34 - i * 7), 45, 44, 40), 1.1f * u);
                g.DrawEllipse(&shPen, cx - sr, cy - sr, sr * 2.0f, sr * 2.0f);
            }

            // 12 inward-tapering hour wedges (r = 55.5 down to 50.0) and 48 circular minute dots (r = 52.8)
            SolidBrush hourTickBrush(Color(255, 36, 36, 35));
            SolidBrush minDotBrush(Color(245, 76, 75, 71));
            for (int i = 0; i < 60; ++i) {
                float a = (i * 6.0f - 90.0f) * kPi / 180.0f;
                if (i % 5 == 0) {
                    PointF wedge[4] = {
                        PointF(-1.15f * u, -55.4f * u),
                        PointF( 1.15f * u, -55.4f * u),
                        PointF( 0.72f * u, -49.8f * u),
                        PointF(-0.72f * u, -49.8f * u)
                    };
                    FillRotatedHandPolygon(g, cx, cy, a, wedge, 4, hourTickBrush);
                } else {
                    float dotR = 52.8f * u;
                    float dx = cx + std::cos(a) * dotR;
                    float dy = cy + std::sin(a) * dotR;
                    float rad = 0.92f * u;
                    g.FillEllipse(&minDotBrush, dx - rad, dy - rad, rad * 2.0f, rad * 2.0f);
                }
            }

            // Exact numeral positions measured from trad.png (relative to center 64.5, 64.5)
            static const float kTradNumPos[12][2] = {
                { 20.0f, -35.5f}, // 1
                { 34.0f, -23.0f}, // 2
                { 42.0f,  -1.5f}, // 3
                { 34.5f,  18.0f}, // 4
                { 19.0f,  33.0f}, // 5
                { -1.5f,  38.5f}, // 6
                {-20.5f,  34.2f}, // 7
                {-37.2f,  19.5f}, // 8
                {-42.8f,  -1.5f}, // 9
                {-36.2f, -21.2f}, // 10
                {-22.0f, -35.8f}, // 11
                { -1.2f, -40.5f}  // 12
            };
            Color numCol(255, 72, 71, 67);
            for (int n = 1; n <= 12; ++n) {
                float nx = cx + kTradNumPos[n - 1][0] * u;
                float ny = cy + kTradNumPos[n - 1][1] * u;
                DrawTradNumber(g, n, nx, ny, u, numCol);
            }

            // Optional custom clock name
            if (customName && customName[0] != L'\0') {
                FontFamily segoe(L"Segoe UI");
                Font nameFont(&segoe, SnapFontSize(8.5f * u), FontStyleRegular, UnitPixel);
                SolidBrush nameBrush(Color(215, 72, 71, 67));
                RectF nameRc = SnapTextRect(cx - 30.0f * u, cy + 12.0f * u, 60.0f * u, 13.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &nameBrush);
            }

            // Hour hand (measured from trad_h.png: NO tail behind center! Linear taper from 4.4px at pivot to 1.2px at tip dy=-40)
            SolidBrush handBrush(Color(255, 25, 24, 24));
            PointF hrPts[5] = {
                PointF(-2.2f * u,   0.0f),
                PointF(-0.65f * u, -39.5f * u),
                PointF( 0.0f,      -40.5f * u),
                PointF( 0.65f * u, -39.5f * u),
                PointF( 2.2f * u,   0.0f)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrPts, 5, handBrush, 1.1f * u, 1.6f * u, 65);

            // Minute hand (measured from trad_m.png: NO tail behind center! Linear taper to 1.1px at tip dy=-47.5)
            PointF minPts[5] = {
                PointF(-2.1f * u,   0.0f),
                PointF(-0.55f * u, -46.5f * u),
                PointF( 0.0f,      -47.5f * u),
                PointF( 0.55f * u, -46.5f * u),
                PointF( 2.1f * u,   0.0f)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minPts, 5, handBrush, 1.2f * u, 1.8f * u, 65);

            // Dark charcoal center hub (trad_dot.png: diameter 7px, r = 3.5px)
            float dotR = 3.4f * u;
            SolidBrush dotShadow(Color(80, 0, 0, 0));
            g.FillEllipse(&dotShadow, cx - dotR + 0.8f * u, cy - dotR + 1.0f * u, dotR * 2.0f, dotR * 2.0f);
            SolidBrush dotFill(Color(255, 26, 24, 24));
            g.FillEllipse(&dotFill, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
            Pen dotRim(Color(200, 75, 74, 74), 0.7f * u);
            g.DrawEllipse(&dotRim, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);

            // Second hand (measured from trad_s.png: RGB(188, 47, 55), tip dy=-48, tail dy=+14, red collar r=2.6)
            if (showSeconds) {
                SolidBrush secBrush(Color(255, 188, 47, 55));
                PointF secPts[4] = {
                    PointF(-1.15f * u,  14.0f * u),
                    PointF(-0.55f * u, -48.0f * u),
                    PointF( 0.55f * u, -48.0f * u),
                    PointF( 1.15f * u,  14.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, secPts, 4, secBrush, 1.0f * u, 1.5f * u, 55);

                float colR = 2.6f * u;
                g.FillEllipse(&secBrush, cx - colR, cy - colR, colR * 2.0f, colR * 2.0f);
                float innerPinR = 1.1f * u;
                g.FillEllipse(&dotFill, cx - innerPinR, cy - innerPinR, innerPinR * 2.0f, innerPinR * 2.0f);
            }
            break;
        }

        // ─────────────────────────────────────────────────────────────────────
        // SKIN 1: MODERN ('modern.png', 'modern_h/m/s/dot.png')
        // Brushed silver bezel, crimson red dial, 12 white baton markers with red dots
        // ─────────────────────────────────────────────────────────────────────
        case 1: {
            float rOuter = 62.5f * u;
            float rDial  = 55.5f * u;

            LinearGradientBrush silverBezel(
                PointF(cx - rOuter, cy - rOuter), PointF(cx + rOuter, cy + rOuter),
                Color(255, 252, 252, 252), Color(255, 136, 141, 145));
            g.FillEllipse(&silverBezel, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);
            Pen outerEdge(Color(220, 95, 100, 105), 0.9f * u);
            g.DrawEllipse(&outerEdge, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);

            // Crimson red dial: RGB(234,43,47) -> RGB(190,9,26)
            LinearGradientBrush redDial(
                PointF(cx, cy - rDial), PointF(cx, cy + rDial),
                Color(255, 234, 43, 47), Color(255, 185, 8, 24));
            g.FillEllipse(&redDial, cx - rDial, cy - rDial, rDial * 2.0f, rDial * 2.0f);

            // Deep dark-red inner bezel shadow (r = 51.5..55.5)
            for (int i = 0; i < 4; ++i) {
                float sr = rDial - i * 0.9f * u;
                Pen shPen(Color(static_cast<BYTE>(95 - i * 20), 90, 8, 12), 1.1f * u);
                g.DrawEllipse(&shPen, cx - sr, cy - sr, sr * 2.0f, sr * 2.0f);
            }

            // 12 white rectangular baton hour markers (r = 44.5..52.0, width 3.2px) + tiny red dot at r = 50.2
            SolidBrush batonBrush(Color(255, 246, 248, 250));
            SolidBrush redDotBrush(Color(255, 204, 24, 30));
            for (int i = 0; i < 12; ++i) {
                float a = (i * 30.0f - 90.0f) * kPi / 180.0f;
                PointF baton[4] = {
                    PointF(-1.55f * u, -52.0f * u),
                    PointF( 1.55f * u, -52.0f * u),
                    PointF( 1.55f * u, -44.5f * u),
                    PointF(-1.55f * u, -44.5f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, a, baton, 4, batonBrush, 0.7f * u, 1.0f * u, 50);

                float dotDist = 50.2f * u;
                float dx = cx + std::cos(a) * dotDist;
                float dy = cy + std::sin(a) * dotDist;
                float dr = 0.85f * u;
                g.FillEllipse(&redDotBrush, dx - dr, dy - dr, dr * 2.0f, dr * 2.0f);
            }

            if (customName && customName[0] != L'\0') {
                FontFamily segoe(L"Segoe UI");
                Font nameFont(&segoe, SnapFontSize(8.5f * u), FontStyleRegular, UnitPixel);
                SolidBrush nameBrush(Color(235, 255, 240, 240));
                RectF nameRc = SnapTextRect(cx - 30.0f * u, cy + 14.0f * u, 60.0f * u, 13.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &nameBrush);
            }

            // White rectangular baton hands (modern_h: -36..+13, w=4.6; modern_m: -46..+15, w=4.4)
            SolidBrush whiteHand(Color(255, 248, 250, 252));
            PointF hrPts[4] = {
                PointF(-2.2f * u,  13.0f * u),
                PointF(-2.2f * u, -36.0f * u),
                PointF( 2.2f * u, -36.0f * u),
                PointF( 2.2f * u,  13.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrPts, 4, whiteHand, 1.2f * u, 1.8f * u, 75);

            PointF minPts[4] = {
                PointF(-2.1f * u,  15.0f * u),
                PointF(-2.1f * u, -46.0f * u),
                PointF( 2.1f * u, -46.0f * u),
                PointF( 2.1f * u,  15.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minPts, 4, whiteHand, 1.2f * u, 1.8f * u, 75);

            // Salmon-orange second hand (modern_s: RGB(255,176,145), -50..+17)
            if (showSeconds) {
                SolidBrush secBrush(Color(255, 255, 176, 145));
                PointF secPts[4] = {
                    PointF(-0.95f * u,  17.0f * u),
                    PointF(-0.75f * u, -50.0f * u),
                    PointF( 0.75f * u, -50.0f * u),
                    PointF( 0.95f * u,  17.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, secPts, 4, secBrush, 1.0f * u, 1.4f * u, 60);
            }

            // Center dot: white outer ring (r=3.2) with salmon-red inner dot (r=1.8)
            float hubR = 3.2f * u;
            g.FillEllipse(&whiteHand, cx - hubR, cy - hubR, hubR * 2.0f, hubR * 2.0f);
            float inR = 1.8f * u;
            SolidBrush innerHub(Color(255, 240, 120, 95));
            g.FillEllipse(&innerHub, cx - inR, cy - inR, inR * 2.0f, inR * 2.0f);
            break;
        }

        // ─────────────────────────────────────────────────────────────────────
        // SKIN 2: CHRONOMETER ('cronometer.png', 'cronometer_h/m/s/dot.png')
        // Silver bezel, grey tachymeter ring, charcoal dial, 3 silver sub-dials
        // ─────────────────────────────────────────────────────────────────────
        case 2: {
            float rOuter = 62.5f * u;
            float rTach  = 55.5f * u;
            float rDial  = 49.0f * u;

            // Brushed silver bezel
            LinearGradientBrush silverBezel(
                PointF(cx - rOuter, cy - rOuter), PointF(cx + rOuter, cy + rOuter),
                Color(255, 248, 250, 252), Color(255, 118, 122, 126));
            g.FillEllipse(&silverBezel, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);
            Pen bezelEdge(Color(235, 85, 88, 92), 0.9f * u);
            g.DrawEllipse(&bezelEdge, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);

            // Grey tachymeter inner ring (r = 49..55.5)
            LinearGradientBrush tachRing(
                PointF(cx, cy - rTach), PointF(cx, cy + rTach),
                Color(255, 140, 143, 146), Color(255, 178, 182, 185));
            g.FillEllipse(&tachRing, cx - rTach, cy - rTach, rTach * 2.0f, rTach * 2.0f);

            // Tachymeter ticks around the grey ring (including red accent ticks at 1..3 o'clock)
            Pen tachBlackPen(Color(220, 25, 26, 28), 0.8f * u);
            Pen tachRedPen(Color(230, 195, 38, 42), 1.1f * u);
            for (int i = 0; i < 120; ++i) {
                if (i % 2 != 0 && i < 95) continue; // Dense 1/5s ticks near 10..12 o'clock
                float a = (i * 3.0f - 90.0f) * kPi / 180.0f;
                bool isRed = (i >= 10 && i <= 30 && (i % 10 == 0)) || (i == 75);
                bool isMajor = (i % 10 == 0);
                float r1 = isMajor ? 49.5f * u : 51.2f * u;
                float r2 = 54.8f * u;
                g.DrawLine(isRed ? &tachRedPen : &tachBlackPen,
                           cx + std::cos(a) * r1, cy + std::sin(a) * r1,
                           cx + std::cos(a) * r2, cy + std::sin(a) * r2);
            }

            // Dark charcoal main dial (r = 49.0)
            LinearGradientBrush darkDial(
                PointF(cx, cy - rDial), PointF(cx, cy + rDial),
                Color(255, 32, 33, 34), Color(255, 52, 54, 56));
            g.FillEllipse(&darkDial, cx - rDial, cy - rDial, rDial * 2.0f, rDial * 2.0f);

            // 3 silver sub-dials at (65, 36), (39, 57), (91, 57) -> offsets (0, -26.5), (-24.5, -6.5), (+24.5, -6.5)
            const float subOffsets[3][2] = {
                {  0.0f, -26.5f },
                {-24.5f,  -6.5f },
                { 24.5f,  -6.5f }
            };
            static const wchar_t* const kSubLabels[3][6] = {
                {L"30", L"5", L"10", L"15", L"20", L"25"},
                {L"60", L"10", L"20", L"30", L"40", L"50"},
                {L"12", L"2", L"4", L"6", L"8", L"10"}
            };
            FontFamily arialSub(L"Arial");
            Font subFont(&arialSub, 5.4f * u, FontStyleRegular, UnitPixel);
            SolidBrush subTextBrush(Color(235, 22, 24, 26));
            TextRenderingHint prevHint = g.GetTextRenderingHint();
            g.SetTextRenderingHint(TextRenderingHintAntiAlias);
            float subR = 12.6f * u;
            for (int sIdx = 0; sIdx < 3; ++sIdx) {
                float sx = cx + subOffsets[sIdx][0] * u;
                float sy = cy + subOffsets[sIdx][1] * u;
                LinearGradientBrush subBrush(
                    PointF(sx, sy - subR), PointF(sx, sy + subR),
                    Color(255, 185, 187, 190), Color(255, 220, 222, 224));
                g.FillEllipse(&subBrush, sx - subR, sy - subR, subR * 2.0f, subR * 2.0f);
                Pen subRim(Color(210, 20, 20, 20), 0.8f * u);
                g.DrawEllipse(&subRim, sx - subR, sy - subR, subR * 2.0f, subR * 2.0f);

                // Sub-dial ticks & tiny numerals (at 12, 4, 8 o'clock positions of each sub-dial for clean legibility)
                Pen subTick(Color(200, 35, 37, 40), 0.65f * u);
                for (int t = 0; t < 12; ++t) {
                    float ta = (t * 30.0f - 90.0f) * kPi / 180.0f;
                    g.DrawLine(&subTick,
                               sx + std::cos(ta) * (subR * 0.80f), sy + std::sin(ta) * (subR * 0.80f),
                               sx + std::cos(ta) * (subR * 0.96f), sy + std::sin(ta) * (subR * 0.96f));
                    if (t % 4 == 0) {
                        float tx = sx + std::cos(ta) * (subR * 0.46f);
                        float ty = sy + std::sin(ta) * (subR * 0.46f);
                        RectF trc(tx - 7.0f * u, ty - 4.5f * u, 14.0f * u, 9.0f * u);
                        g.DrawString(kSubLabels[sIdx][t / 2], -1, &subFont, trc, &sf, &subTextBrush);
                    }
                }
                // Mini sub-dial hand with hollow center ring
                float miniA = ((sIdx == 0 ? 45.0f : (sIdx == 1 ? 350.0f : 95.0f)) - 90.0f) * kPi / 180.0f;
                Pen miniHand(Color(245, 24, 26, 28), 1.25f * u);
                g.DrawLine(&miniHand, sx, sy, sx + std::cos(miniA) * (subR * 0.72f), sy + std::sin(miniA) * (subR * 0.72f));
                SolidBrush miniPin(Color(255, 32, 34, 36));
                g.FillEllipse(&miniPin, sx - 1.6f * u, sy - 1.6f * u, 3.2f * u, 3.2f * u);
                SolidBrush miniHole(Color(255, 205, 207, 210));
                g.FillEllipse(&miniHole, sx - 0.7f * u, sy - 0.7f * u, 1.4f * u, 1.4f * u);
            }
            g.SetTextRenderingHint(prevHint);

            // 12 crisp white outer hour ticks (r = 44.5..48.5) + 4 inner luminous bars at 11, 1, 4, 8 o'clock (r = 28.5..37.0)
            SolidBrush outerTickBrush(Color(240, 225, 232, 228));
            SolidBrush lumeBrush(Color(255, 214, 234, 218));
            for (int hIdx = 1; hIdx <= 12; ++hIdx) {
                float a = (hIdx * 30.0f - 90.0f) * kPi / 180.0f;
                PointF outerTick[4] = {
                    PointF(-0.7f * u, -48.5f * u),
                    PointF( 0.7f * u, -48.5f * u),
                    PointF( 0.7f * u, -44.2f * u),
                    PointF(-0.7f * u, -44.2f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, a, outerTick, 4, outerTickBrush);

                if (hIdx == 1 || hIdx == 4 || hIdx == 8 || hIdx == 11) {
                    PointF innerBaton[4] = {
                        PointF(-1.45f * u, -37.2f * u),
                        PointF( 1.45f * u, -37.2f * u),
                        PointF( 1.45f * u, -28.8f * u),
                        PointF(-1.45f * u, -28.8f * u)
                    };
                    FillRotatedHandPolygon(g, cx, cy, a, innerBaton, 4, lumeBrush);
                }
            }

            if (customName && customName[0] != L'\0') {
                FontFamily segoe(L"Segoe UI");
                Font nameFont(&segoe, SnapFontSize(8.0f * u), FontStyleRegular, UnitPixel);
                SolidBrush nameBrush(Color(225, 220, 225, 228));
                RectF nameRc = SnapTextRect(cx - 28.0f * u, cy + 16.0f * u, 56.0f * u, 12.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &nameBrush);
            }

            // Silver sword hands with dark inner slot (cronometer_h: -38..+8, cronometer_m: -43..+8)
            SolidBrush silverHand(Color(255, 215, 218, 220));
            SolidBrush slotBrush(Color(255, 55, 57, 60));
            PointF hrPts[5] = {
                PointF(-2.2f * u,   8.0f * u),
                PointF(-2.4f * u, -24.0f * u),
                PointF( 0.0f,     -38.0f * u),
                PointF( 2.4f * u, -24.0f * u),
                PointF( 2.2f * u,   8.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrPts, 5, silverHand, 1.2f * u, 1.6f * u, 80);
            PointF hrSlot[4] = {
                PointF(-0.7f * u,  -4.0f * u),
                PointF(-0.7f * u, -18.0f * u),
                PointF( 0.7f * u, -18.0f * u),
                PointF( 0.7f * u,  -4.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrSlot, 4, slotBrush);

            PointF minPts[5] = {
                PointF(-1.9f * u,   8.0f * u),
                PointF(-2.1f * u, -29.0f * u),
                PointF( 0.0f,     -43.5f * u),
                PointF( 2.1f * u, -29.0f * u),
                PointF( 1.9f * u,   8.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minPts, 5, silverHand, 1.2f * u, 1.8f * u, 80);
            PointF minSlot[4] = {
                PointF(-0.6f * u,  -4.0f * u),
                PointF(-0.6f * u, -21.0f * u),
                PointF( 0.6f * u, -21.0f * u),
                PointF( 0.6f * u,  -4.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minSlot, 4, slotBrush);

            // Second hand: dark shaft with red triangular arrow tip (cronometer_s.png)
            if (showSeconds) {
                SolidBrush darkSec(Color(255, 30, 31, 32));
                PointF secShaft[4] = {
                    PointF(-0.85f * u,  10.0f * u),
                    PointF(-0.65f * u, -40.0f * u),
                    PointF( 0.65f * u, -40.0f * u),
                    PointF( 0.85f * u,  10.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, secShaft, 4, darkSec, 1.0f * u, 1.4f * u, 60);

                SolidBrush redTip(Color(255, 192, 45, 52));
                PointF tipPts[3] = {
                    PointF(-2.1f * u, -40.0f * u),
                    PointF( 0.0f,     -48.0f * u),
                    PointF( 2.1f * u, -40.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, tipPts, 3, redTip);
            }

            float dotR = 3.2f * u;
            SolidBrush capBrush(Color(255, 42, 43, 45));
            g.FillEllipse(&capBrush, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
            Pen capRim(Color(220, 185, 188, 192), 0.8f * u);
            g.DrawEllipse(&capRim, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
            break;
        }

        // ─────────────────────────────────────────────────────────────────────
        // SKIN 3: DINER ('diner.png', 'diner_h/m/s/dot.png')
        // Black outer rim, glowing electric cyan-blue neon tube ring, deep navy dial
        // ─────────────────────────────────────────────────────────────────────
        case 3: {
            float rOuter = 62.5f * u;
            float rDial  = 47.0f * u;

            // Black outer case with subtle top highlight
            LinearGradientBrush blackCase(
                PointF(cx, cy - rOuter), PointF(cx, cy + rOuter),
                Color(255, 70, 74, 82), Color(255, 12, 14, 18));
            g.FillEllipse(&blackCase, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);

            float rInnerBlack = 59.0f * u;
            SolidBrush innerBlack(Color(255, 10, 12, 22));
            g.FillEllipse(&innerBlack, cx - rInnerBlack, cy - rInnerBlack, rInnerBlack * 2.0f, rInnerBlack * 2.0f);

            // Deep navy-blue inner dial (r = 47.0): RGB(11,25,170) -> RGB(10,18,77)
            LinearGradientBrush navyDial(
                PointF(cx, cy - rDial), PointF(cx, cy + rDial),
                Color(255, 15, 38, 195), Color(255, 10, 18, 77));
            g.FillEllipse(&navyDial, cx - rDial, cy - rDial, rDial * 2.0f, rDial * 2.0f);

            // Glowing electric blue/cyan neon tube (r = 52.0, with characteristic 6 o'clock inward bends)
            float rNeon = 52.0f * u;
            GraphicsPath neonPath;
            neonPath.AddLine(PointF(cx - 4.2f * u, cy + 47.0f * u), PointF(cx - 4.2f * u, cy + 51.8f * u));
            neonPath.AddArc(cx - rNeon, cy - rNeon, rNeon * 2.0f, rNeon * 2.0f, 95.0f, 350.0f);
            neonPath.AddLine(PointF(cx + 4.2f * u, cy + 51.8f * u), PointF(cx + 4.2f * u, cy + 47.0f * u));

            Pen neonGlowOuter(Color(115, 15, 95, 255), 8.5f * u);
            neonGlowOuter.SetStartCap(LineCapRound);
            neonGlowOuter.SetEndCap(LineCapRound);
            neonGlowOuter.SetLineJoin(LineJoinRound);
            g.DrawPath(&neonGlowOuter, &neonPath);

            Pen neonGlowMid(Color(215, 30, 175, 255), 5.0f * u);
            neonGlowMid.SetStartCap(LineCapRound);
            neonGlowMid.SetEndCap(LineCapRound);
            neonGlowMid.SetLineJoin(LineJoinRound);
            g.DrawPath(&neonGlowMid, &neonPath);

            Pen neonCore(Color(255, 175, 255, 255), 2.2f * u);
            neonCore.SetStartCap(LineCapRound);
            neonCore.SetEndCap(LineCapRound);
            neonCore.SetLineJoin(LineJoinRound);
            g.DrawPath(&neonCore, &neonPath);

            // White arc segments between numbers at r = 37.5
            float rNumRing = 37.2f * u;
            Pen whiteArcPen(Color(245, 235, 242, 255), 1.85f * u);
            whiteArcPen.SetStartCap(LineCapRound);
            whiteArcPen.SetEndCap(LineCapRound);
            for (int i = 0; i < 12; ++i) {
                float startDeg = i * 30.0f - 90.0f + 10.5f;
                float sweepDeg = 9.0f;
                g.DrawArc(&whiteArcPen, cx - rNumRing, cy - rNumRing, rNumRing * 2.0f, rNumRing * 2.0f, startDeg, sweepDeg);
            }

            // Bold white sans-serif numbers 1..12 around r = 37.2
            FontFamily arial(L"Arial");
            Font dinerFont(&arial, SnapFontSize(9.5f * u), FontStyleBold, UnitPixel);
            SolidBrush whiteText(Color(255, 250, 252, 255));
            for (int n = 1; n <= 12; ++n) {
                float a = (n * 30.0f - 90.0f) * kPi / 180.0f;
                float nx = cx + std::cos(a) * rNumRing;
                float ny = cy + std::sin(a) * rNumRing;
                wchar_t buf[8];
                swprintf_s(buf, L"%d", n);
                RectF rc = SnapTextRect(nx - 10.0f * u, ny - 6.5f * u, 20.0f * u, 13.0f * u);
                g.DrawString(buf, -1, &dinerFont, rc, &sf, &whiteText);
            }

            if (customName && customName[0] != L'\0') {
                Font nameFont(&arial, SnapFontSize(7.5f * u), FontStyleRegular, UnitPixel);
                SolidBrush nameBrush(Color(225, 190, 240, 255));
                RectF nameRc = SnapTextRect(cx - 26.0f * u, cy + 12.0f * u, 52.0f * u, 12.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &nameBrush);
            }

            // White tapered hands (diner_h: -31..0, diner_m: -39..0) + white center ring
            SolidBrush whiteHand(Color(255, 245, 248, 255));
            PointF hrPts[4] = {
                PointF(-2.1f * u,   0.0f),
                PointF(-0.8f * u, -31.0f * u),
                PointF( 0.8f * u, -31.0f * u),
                PointF( 2.1f * u,   0.0f)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrPts, 4, whiteHand, 1.1f * u, 1.5f * u, 70);

            PointF minPts[4] = {
                PointF(-2.0f * u,   0.0f),
                PointF(-0.7f * u, -39.0f * u),
                PointF( 0.7f * u, -39.0f * u),
                PointF( 2.0f * u,   0.0f)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minPts, 4, whiteHand, 1.1f * u, 1.5f * u, 70);

            float hubR = 4.2f * u;
            g.FillEllipse(&whiteHand, cx - hubR, cy - hubR, hubR * 2.0f, hubR * 2.0f);
            SolidBrush hubCenter(Color(255, 12, 24, 125));
            g.FillEllipse(&hubCenter, cx - 2.0f * u, cy - 2.0f * u, 4.0f * u, 4.0f * u);

            // Red second hand (diner_s: RGB(218,0,0), -34..+20)
            if (showSeconds) {
                SolidBrush redSec(Color(255, 224, 18, 18));
                PointF secPts[4] = {
                    PointF(-0.9f * u,  20.0f * u),
                    PointF(-0.6f * u, -34.0f * u),
                    PointF( 0.6f * u, -34.0f * u),
                    PointF( 0.9f * u,  20.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, secPts, 4, redSec, 0.9f * u, 1.3f * u, 60);
                g.FillEllipse(&redSec, cx - 2.3f * u, cy - 2.3f * u, 4.6f * u, 4.6f * u);
            }
            break;
        }

        // ─────────────────────────────────────────────────────────────────────
        // SKIN 4: DAISY / FLOWER ('flower.png', 'flower_h/m/s/dot.png')
        // Layered white daisy petals, spiky orange-gold center, floating teardrop second
        // ─────────────────────────────────────────────────────────────────────
        case 4: {
            // Draw 24 layered white daisy petals (back ring + front ring)
            for (int layer = 0; layer < 2; ++layer) {
                int petalCount = 12;
                float angleOffset = layer * 15.0f;
                for (int i = 0; i < petalCount; ++i) {
                    float deg = i * 30.0f + angleOffset;
                    GraphicsState stPetal = g.Save();
                    g.TranslateTransform(cx, cy);
                    g.RotateTransform(deg);

                    float pW = (layer == 0 ? 15.5f : 14.5f) * u;
                    float pH = (layer == 0 ? 48.0f : 49.5f) * u;
                    RectF petalRc(-pW * 0.5f, -61.0f * u, pW, pH);

                    if (drawDropShadow && layer == 0) {
                        SolidBrush pShadow(Color(28, 0, 0, 0));
                        g.FillEllipse(&pShadow, petalRc.X + 1.0f * u, petalRc.Y + 2.0f * u, petalRc.Width, petalRc.Height);
                    }

                    LinearGradientBrush petalGrad(
                        PointF(0.0f, -61.0f * u), PointF(0.0f, -12.0f * u),
                        layer == 0 ? Color(255, 224, 228, 238) : Color(255, 254, 254, 255),
                        layer == 0 ? Color(255, 198, 204, 218) : Color(255, 228, 232, 242));
                    g.FillEllipse(&petalGrad, petalRc);

                    Pen petalEdge(Color(140, 175, 182, 198), 0.7f * u);
                    g.DrawEllipse(&petalEdge, petalRc);
                    g.Restore(stPetal);
                }
            }

            // Subtle translucent white circular dial overlay (r = 46.5) as in flower.png
            float rGlass = 46.5f * u;
            SolidBrush dialOverlay(Color(95, 245, 247, 252));
            g.FillEllipse(&dialOverlay, cx - rGlass, cy - rGlass, rGlass * 2.0f, rGlass * 2.0f);
            Pen dialRim(Color(110, 255, 255, 255), 1.0f * u);
            g.DrawEllipse(&dialRim, cx - rGlass, cy - rGlass, rGlass * 2.0f, rGlass * 2.0f);

            // Spiky golden-orange flower center (r = 13.5)
            float rCenter = 13.5f * u;
            SolidBrush centerOuter(Color(255, 207, 80, 1));
            g.FillEllipse(&centerOuter, cx - rCenter, cy - rCenter, rCenter * 2.0f, rCenter * 2.0f);
            Pen spikePen(Color(255, 225, 110, 8), 1.2f * u);
            for (int i = 0; i < 36; ++i) {
                float a = (i * 10.0f) * kPi / 180.0f;
                g.DrawLine(&spikePen,
                           cx + std::cos(a) * (10.0f * u), cy + std::sin(a) * (10.0f * u),
                           cx + std::cos(a) * (15.0f * u), cy + std::sin(a) * (15.0f * u));
            }
            LinearGradientBrush centerInner(
                PointF(cx - 10.0f * u, cy - 10.0f * u), PointF(cx + 10.0f * u, cy + 10.0f * u),
                Color(255, 245, 175, 32), Color(255, 212, 92, 4));
            g.FillEllipse(&centerInner, cx - 10.5f * u, cy - 10.5f * u, 21.0f * u, 21.0f * u);

            // Orange numerals 12, 3, 6, 9 and orange dots at other hours (r = 38.5)
            FontFamily arial(L"Arial");
            Font flowerFont(&arial, SnapFontSize(12.5f * u), FontStyleBold, UnitPixel);
            SolidBrush orangeBrush(Color(255, 242, 133, 0));
            float rMarks = 38.5f * u;
            for (int n = 1; n <= 12; ++n) {
                float a = (n * 30.0f - 90.0f) * kPi / 180.0f;
                float mx = cx + std::cos(a) * rMarks;
                float my = cy + std::sin(a) * rMarks;
                if (n % 3 == 0) {
                    wchar_t buf[8];
                    swprintf_s(buf, L"%d", n);
                    RectF rc = SnapTextRect(mx - 12.0f * u, my - 8.0f * u, 24.0f * u, 16.0f * u);
                    g.DrawString(buf, -1, &flowerFont, rc, &sf, &orangeBrush);
                } else {
                    float dr = 1.9f * u;
                    g.FillEllipse(&orangeBrush, mx - dr, my - dr, dr * 2.0f, dr * 2.0f);
                }
            }

            if (customName && customName[0] != L'\0') {
                Font nameFont(&arial, SnapFontSize(7.5f * u), FontStyleBold, UnitPixel);
                RectF nameRc = SnapTextRect(cx - 26.0f * u, cy + 16.0f * u, 52.0f * u, 12.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &orangeBrush);
            }

            // Golden-yellow tapered hands (flower_h: -31..+3, flower_m: -39..+2, RGB(248,204,27))
            SolidBrush goldHand(Color(255, 248, 204, 27));
            PointF hrPts[4] = {
                PointF(-2.1f * u,   3.0f * u),
                PointF(-0.9f * u, -31.0f * u),
                PointF( 0.9f * u, -31.0f * u),
                PointF( 2.1f * u,   3.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrPts, 4, goldHand, 1.0f * u, 1.4f * u, 55);

            PointF minPts[4] = {
                PointF(-2.0f * u,   2.0f * u),
                PointF(-0.8f * u, -39.0f * u),
                PointF( 0.8f * u, -39.0f * u),
                PointF( 2.0f * u,   2.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minPts, 4, goldHand, 1.0f * u, 1.4f * u, 55);

            // Detached floating golden teardrop second indicator (flower_s: dy = -38..-30)
            if (showSeconds) {
                PointF dropPts[6] = {
                    PointF( 0.0f,     -38.5f * u),
                    PointF( 2.3f * u, -33.0f * u),
                    PointF( 1.6f * u, -30.0f * u),
                    PointF( 0.0f,     -29.2f * u),
                    PointF(-1.6f * u, -30.0f * u),
                    PointF(-2.3f * u, -33.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, dropPts, 6, goldHand, 0.8f * u, 1.2f * u, 50);
            }

            float dotR = 3.5f * u;
            g.FillEllipse(&goldHand, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
            break;
        }

        // ─────────────────────────────────────────────────────────────────────
        // SKIN 5: NOVELTY ('novelty.png', 'novelty_h/m/s/dot.png')
        // Pink furry cat-eared monster holding a sky-blue & white clock belly
        // ─────────────────────────────────────────────────────────────────────
        case 5: {
            // Monster tail on bottom-right (+38..+52, +38..+54) and bottom feet (-50..-32, +45..+58; +30..+46, +45..+58)
            SolidBrush basePink(Color(255, 224, 118, 188));
            Pen furEdge(Color(225, 168, 72, 138), 1.1f * u);

            GraphicsPath tailPath;
            tailPath.AddBezier(
                PointF(cx + 35.0f * u, cy + 44.0f * u),
                PointF(cx + 46.0f * u, cy + 44.0f * u),
                PointF(cx + 52.0f * u, cy + 36.0f * u),
                PointF(cx + 50.0f * u, cy + 31.0f * u));
            tailPath.AddBezier(
                PointF(cx + 50.0f * u, cy + 31.0f * u),
                PointF(cx + 51.0f * u, cy + 46.0f * u),
                PointF(cx + 44.0f * u, cy + 55.0f * u),
                PointF(cx + 32.0f * u, cy + 54.0f * u));
            tailPath.CloseFigure();
            g.FillPath(&basePink, &tailPath);
            g.DrawPath(&furEdge, &tailPath);

            // Left and right stubby feet at the bottom
            g.FillEllipse(&basePink, cx - 52.0f * u, cy + 43.0f * u, 24.0f * u, 14.0f * u);
            g.DrawEllipse(&furEdge,  cx - 52.0f * u, cy + 43.0f * u, 24.0f * u, 14.0f * u);
            g.FillEllipse(&basePink, cx + 26.0f * u, cy + 43.0f * u, 22.0f * u, 14.0f * u);
            g.DrawEllipse(&furEdge,  cx + 26.0f * u, cy + 43.0f * u, 22.0f * u, 14.0f * u);

            // Monster torso & long pointed cat/bat ears (measured from novelty.png: ear tips at (-47,-62) and (+47,-62), flat head y=-43)
            GraphicsPath monsterPath;
            monsterPath.AddBezier(
                PointF(cx - 43.0f * u, cy - 28.0f * u),
                PointF(cx - 50.0f * u, cy - 48.0f * u),
                PointF(cx - 51.0f * u, cy - 62.0f * u),
                PointF(cx - 46.5f * u, cy - 62.5f * u));
            monsterPath.AddBezier(
                PointF(cx - 46.5f * u, cy - 62.5f * u),
                PointF(cx - 39.0f * u, cy - 62.0f * u),
                PointF(cx - 26.0f * u, cy - 50.0f * u),
                PointF(cx - 18.5f * u, cy - 43.0f * u));
            monsterPath.AddLine(
                PointF(cx - 18.5f * u, cy - 43.0f * u),
                PointF(cx + 18.5f * u, cy - 43.0f * u));
            monsterPath.AddBezier(
                PointF(cx + 18.5f * u, cy - 43.0f * u),
                PointF(cx + 26.0f * u, cy - 50.0f * u),
                PointF(cx + 39.0f * u, cy - 62.0f * u),
                PointF(cx + 46.5f * u, cy - 62.5f * u));
            monsterPath.AddBezier(
                PointF(cx + 46.5f * u, cy - 62.5f * u),
                PointF(cx + 51.0f * u, cy - 62.0f * u),
                PointF(cx + 50.0f * u, cy - 48.0f * u),
                PointF(cx + 43.0f * u, cy - 28.0f * u));
            monsterPath.AddBezier(
                PointF(cx + 43.0f * u, cy - 28.0f * u),
                PointF(cx + 51.0f * u, cy -  5.0f * u),
                PointF(cx + 50.0f * u, cy + 42.0f * u),
                PointF(cx + 32.0f * u, cy + 54.0f * u));
            monsterPath.AddLine(
                PointF(cx + 32.0f * u, cy + 54.0f * u),
                PointF(cx - 32.0f * u, cy + 54.0f * u));
            monsterPath.AddBezier(
                PointF(cx - 32.0f * u, cy + 54.0f * u),
                PointF(cx - 50.0f * u, cy + 42.0f * u),
                PointF(cx - 51.0f * u, cy -  5.0f * u),
                PointF(cx - 43.0f * u, cy - 28.0f * u));
            monsterPath.CloseFigure();

            if (drawDropShadow) {
                SolidBrush mShadow(Color(48, 0, 0, 0));
                GraphicsState stSh = g.Save();
                g.TranslateTransform(1.2f * u, 2.2f * u);
                g.FillPath(&mShadow, &monsterPath);
                g.Restore(stSh);
            }

            LinearGradientBrush pinkFur(
                PointF(cx, cy - 62.0f * u), PointF(cx, cy + 56.0f * u),
                Color(255, 250, 178, 228), Color(255, 218, 112, 182));
            g.FillPath(&pinkFur, &monsterPath);
            g.DrawPath(&furEdge, &monsterPath);

            // Darker pink inner ear folds (measured from novelty.png)
            SolidBrush innerEarBrush(Color(215, 192, 68, 145));
            PointF earFoldL[3] = {
                PointF(cx - 45.0f * u, cy - 57.0f * u),
                PointF(cx - 31.0f * u, cy - 41.0f * u),
                PointF(cx - 40.5f * u, cy - 35.0f * u)
            };
            PointF earFoldR[3] = {
                PointF(cx + 45.0f * u, cy - 57.0f * u),
                PointF(cx + 31.0f * u, cy - 41.0f * u),
                PointF(cx + 40.5f * u, cy - 35.0f * u)
            };
            g.FillPolygon(&innerEarBrush, earFoldL, 3);
            g.FillPolygon(&innerEarBrush, earFoldR, 3);

            // Two glossy black oval eyes with double white specular highlights at (-13.5, -28.5) and (+12.5, -28.5)
            SolidBrush eyeBlack(Color(255, 18, 16, 20));
            SolidBrush eyeWhite(Color(255, 255, 255, 255));
            float eyeOffsets[2] = {-13.0f * u, 12.5f * u};
            for (int e = 0; e < 2; ++e) {
                float ex = cx + eyeOffsets[e];
                float ey = cy - 29.0f * u;
                g.FillEllipse(&eyeBlack, ex - 4.8f * u, ey - 7.2f * u, 9.6f * u, 14.4f * u);
                g.FillEllipse(&eyeWhite, ex - 2.4f * u, ey - 4.6f * u, 3.4f * u,  4.6f * u);
                g.FillEllipse(&eyeWhite, ex + 0.4f * u, ey + 1.2f * u, 2.0f * u,  2.2f * u);
            }

            // Wide horizontal grin crease at y = -15.5 and two white fangs at x = -29 and x = +28
            // (sticking out on either side of the top of the clock rim, exactly as in novelty.png!)
            Pen grinPen(Color(210, 165, 58, 125), 1.3f * u);
            g.DrawArc(&grinPen, cx - 35.0f * u, cy - 20.0f * u, 70.0f * u, 10.0f * u, 10.0f, 160.0f);

            PointF fangL[3] = {
                PointF(cx - 32.5f * u, cy - 15.0f * u),
                PointF(cx - 25.5f * u, cy - 14.0f * u),
                PointF(cx - 29.5f * u, cy -  6.5f * u)
            };
            PointF fangR[3] = {
                PointF(cx + 25.5f * u, cy - 14.0f * u),
                PointF(cx + 32.5f * u, cy - 15.0f * u),
                PointF(cx + 29.5f * u, cy -  6.5f * u)
            };
            g.FillPolygon(&eyeWhite, fangL, 3);
            g.DrawPolygon(&furEdge,  fangL, 3);
            g.FillPolygon(&eyeWhite, fangR, 3);
            g.DrawPolygon(&furEdge,  fangR, 3);

            // Clock belly centered at (cx, cy + 21.0 * u) with radius 42.0 * u
            float bellyCy = cy + 21.0f * u;
            float bellyR  = 42.0f * u;
            float dialR   = 36.0f * u;

            LinearGradientBrush whiteRim(
                PointF(cx, bellyCy - bellyR), PointF(cx, bellyCy + bellyR),
                Color(255, 255, 255, 255), Color(255, 212, 218, 228));
            g.FillEllipse(&whiteRim, cx - bellyR, bellyCy - bellyR, bellyR * 2.0f, bellyR * 2.0f);
            Pen bellyBorder(Color(210, 160, 166, 178), 1.0f * u);
            g.DrawEllipse(&bellyBorder, cx - bellyR, bellyCy - bellyR, bellyR * 2.0f, bellyR * 2.0f);

            LinearGradientBrush bellyDial(
                PointF(cx, bellyCy - dialR), PointF(cx, bellyCy + dialR),
                Color(255, 232, 236, 244), Color(255, 252, 253, 255));
            g.FillEllipse(&bellyDial, cx - dialR, bellyCy - dialR, dialR * 2.0f, dialR * 2.0f);

            // Sky-blue '12' and sky-blue hour ticks around belly dial
            SolidBrush skyBlueBrush(Color(255, 88, 158, 255));
            Pen skyTickPen(Color(255, 88, 158, 255), 2.2f * u);
            skyTickPen.SetStartCap(LineCapFlat);
            skyTickPen.SetEndCap(LineCapFlat);
            for (int n = 1; n <= 12; ++n) {
                if (n == 12) continue;
                float a = (n * 30.0f - 90.0f) * kPi / 180.0f;
                g.DrawLine(&skyTickPen,
                           cx + std::cos(a) * (27.5f * u), bellyCy + std::sin(a) * (27.5f * u),
                           cx + std::cos(a) * (33.0f * u), bellyCy + std::sin(a) * (33.0f * u));
            }
            FontFamily arial(L"Arial");
            Font novFont(&arial, SnapFontSize(11.5f * u), FontStyleBold, UnitPixel);
            RectF rc12 = SnapTextRect(cx - 12.0f * u, bellyCy - 34.0f * u, 24.0f * u, 14.0f * u);
            g.DrawString(L"12", -1, &novFont, rc12, &sf, &skyBlueBrush);

            if (customName && customName[0] != L'\0') {
                Font nameFont(&arial, SnapFontSize(6.5f * u), FontStyleBold, UnitPixel);
                RectF nameRc = SnapTextRect(cx - 22.0f * u, bellyCy + 8.0f * u, 44.0f * u, 11.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &skyBlueBrush);
            }

            // Pink 3-clawed paws gripping the left and right edges of the clock
            SolidBrush pawBrush(Color(255, 236, 134, 204));
            for (int c = -1; c <= 1; ++c) {
                float cyOff = bellyCy - 6.0f * u + c * 6.0f * u;
                g.FillEllipse(&pawBrush, cx - 54.0f * u, cyOff - 4.0f * u, 15.0f * u, 8.5f * u);
                g.DrawEllipse(&furEdge,  cx - 54.0f * u, cyOff - 4.0f * u, 15.0f * u, 8.5f * u);
                g.FillEllipse(&pawBrush, cx + 39.0f * u, cyOff - 4.0f * u, 15.0f * u, 8.5f * u);
                g.DrawEllipse(&furEdge,  cx + 39.0f * u, cyOff - 4.0f * u, 15.0f * u, 8.5f * u);
            }

            // Hands pivoted at bellyCy!
            // Hour hand: sky-blue (-25..+14), Minute hand: hot-pink (-30..+14), Second hand: yellow (-35..+14)
            PointF hrPts[4] = {
                PointF(-2.1f * u,  13.0f * u),
                PointF(-1.5f * u, -25.0f * u),
                PointF( 1.5f * u, -25.0f * u),
                PointF( 2.1f * u,  13.0f * u)
            };
            FillRotatedHandPolygon(g, cx, bellyCy, hrAngle, hrPts, 4, skyBlueBrush, 0.9f * u, 1.3f * u, 55);

            SolidBrush hotPinkBrush(Color(255, 246, 58, 116));
            PointF minPts[4] = {
                PointF(-1.9f * u,  14.0f * u),
                PointF(-1.3f * u, -30.0f * u),
                PointF( 1.3f * u, -30.0f * u),
                PointF( 1.9f * u,  14.0f * u)
            };
            FillRotatedHandPolygon(g, cx, bellyCy, minAngle, minPts, 4, hotPinkBrush, 0.9f * u, 1.3f * u, 55);

            SolidBrush yellowBrush(Color(255, 253, 203, 40));
            if (showSeconds) {
                PointF secPts[4] = {
                    PointF(-0.9f * u,  14.0f * u),
                    PointF(-0.6f * u, -34.0f * u),
                    PointF( 0.6f * u, -34.0f * u),
                    PointF( 0.9f * u,  14.0f * u)
                };
                FillRotatedHandPolygon(g, cx, bellyCy, secAngle, secPts, 4, yellowBrush, 0.8f * u, 1.1f * u, 45);
            }
            float dotR = 3.2f * u;
            g.FillEllipse(&yellowBrush, cx - dotR, bellyCy - dotR, dotR * 2.0f, dotR * 2.0f);
            break;
        }

        // ─────────────────────────────────────────────────────────────────────
        // SKIN 6: SQUARE ('square.png', 'square_h/m/s/dot.png')
        // Brushed steel rounded-square frame, metallic silver dial, thick dark baton hands
        // ─────────────────────────────────────────────────────────────────────
        case 6: {
            float halfOuter = 58.0f * u; // 116x116 square in 130x130 (x = 7..123)
            float halfInner = 51.0f * u; // 102x102 inner recess (x = 14..116)
            RectF outerSq(cx - halfOuter, cy - halfOuter, halfOuter * 2.0f, halfOuter * 2.0f);
            RectF innerSq(cx - halfInner, cy - halfInner, halfInner * 2.0f, halfInner * 2.0f);

            if (drawDropShadow) {
                for (int i = 4; i >= 1; --i) {
                    float exp = i * 1.2f * u;
                    SolidBrush sh(Color(static_cast<BYTE>(14 * (5 - i)), 0, 0, 0));
                    FillRoundRect(g, RectF(outerSq.X - exp, outerSq.Y - exp + i * 1.0f * u,
                                           outerSq.Width + exp * 2.0f, outerSq.Height + exp * 2.0f), 6.0f * u, sh);
                }
            }

            // Brushed steel outer square bezel
            LinearGradientBrush steelFrame(
                PointF(outerSq.X, outerSq.Y), PointF(outerSq.X + outerSq.Width, outerSq.Y + outerSq.Height),
                Color(255, 232, 228, 225), Color(255, 92, 88, 86));
            FillRoundRect(g, outerSq, 5.5f * u, steelFrame);
            Pen outerPen(Color(235, 65, 62, 60), 0.9f * u);
            DrawRoundRect(g, outerSq, 5.5f * u, outerPen);

            // Dark inner recess border
            Pen recessPen(Color(230, 52, 48, 46), 1.6f * u);
            DrawRoundRect(g, innerSq, 3.0f * u, recessPen);

            // Metallic silver square dial with diagonal sheen
            LinearGradientBrush silverDial(
                PointF(innerSq.X, innerSq.Y), PointF(innerSq.X + innerSq.Width, innerSq.Y + innerSq.Height),
                Color(255, 228, 222, 218), Color(255, 148, 144, 142));
            Color sheenCols[3] = {
                Color(255, 205, 198, 194),
                Color(255, 236, 230, 226),
                Color(255, 146, 142, 140)
            };
            REAL sheenPos[3] = {0.0f, 0.42f, 1.0f};
            silverDial.SetInterpolationColors(sheenCols, sheenPos, 3);
            FillRoundRect(g, innerSq, 2.5f * u, silverDial);

            // Circular tick track (r = 47.0): 12 thick dark-charcoal rounded-capsule hour bars + 48 minute ticks
            SolidBrush charcoalBrush(Color(255, 42, 36, 36));
            Pen hourCapPen(Color(255, 42, 36, 36), 4.1f * u);
            hourCapPen.SetStartCap(LineCapRound);
            hourCapPen.SetEndCap(LineCapRound);
            Pen minCapPen(Color(255, 42, 36, 36), 1.3f * u);
            minCapPen.SetStartCap(LineCapRound);
            minCapPen.SetEndCap(LineCapRound);
            for (int i = 0; i < 60; ++i) {
                float a = (i * 6.0f - 90.0f) * kPi / 180.0f;
                if (i % 5 == 0) {
                    g.DrawLine(&hourCapPen,
                               cx + std::cos(a) * (36.8f * u), cy + std::sin(a) * (36.8f * u),
                               cx + std::cos(a) * (45.2f * u), cy + std::sin(a) * (45.2f * u));
                } else {
                    g.DrawLine(&minCapPen,
                               cx + std::cos(a) * (41.5f * u), cy + std::sin(a) * (41.5f * u),
                               cx + std::cos(a) * (46.5f * u), cy + std::sin(a) * (46.5f * u));
                }
            }

            if (customName && customName[0] != L'\0') {
                FontFamily segoe(L"Segoe UI");
                Font nameFont(&segoe, SnapFontSize(8.5f * u), FontStyleRegular, UnitPixel);
                RectF nameRc = SnapTextRect(cx - 30.0f * u, cy + 13.0f * u, 60.0f * u, 13.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &charcoalBrush);
            }

            // Wide dark blunt baton hands (square_h: -43..+17, w=6.0; square_m: -54..+18, w=4.6)
            PointF hrPts[4] = {
                PointF(-2.9f * u,  17.0f * u),
                PointF(-2.9f * u, -43.0f * u),
                PointF( 2.9f * u, -43.0f * u),
                PointF( 2.9f * u,  17.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrPts, 4, charcoalBrush, 1.2f * u, 1.8f * u, 70);

            PointF minPts[4] = {
                PointF(-2.2f * u,  18.0f * u),
                PointF(-2.2f * u, -52.0f * u),
                PointF( 2.2f * u, -52.0f * u),
                PointF( 2.2f * u,  18.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minPts, 4, charcoalBrush, 1.2f * u, 1.8f * u, 70);

            if (showSeconds) {
                PointF secPts[4] = {
                    PointF(-0.65f * u,  20.0f * u),
                    PointF(-0.65f * u, -54.0f * u),
                    PointF( 0.65f * u, -54.0f * u),
                    PointF( 0.65f * u,  20.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, secPts, 4, charcoalBrush, 1.0f * u, 1.4f * u, 55);
            }

            float dotR = 3.0f * u;
            g.FillEllipse(&charcoalBrush, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
            break;
        }

        // ─────────────────────────────────────────────────────────────────────
        // SKIN 7: SYSTEM ('system.png', 'system_h/m/s/dot.png')
        // Multi-layered silver bezel with deep top inner shadow, mint-white dial, thin hands
        // ─────────────────────────────────────────────────────────────────────
        case 7: {
            float rOuter = 62.5f * u;
            float rInnerBezel = 53.5f * u;
            float rDial  = 49.5f * u;

            // Outer multi-layered silver bezel
            LinearGradientBrush outerSilver(
                PointF(cx, cy - rOuter), PointF(cx, cy + rOuter),
                Color(255, 240, 242, 245), Color(255, 118, 122, 128));
            g.FillEllipse(&outerSilver, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);
            Pen outerEdge(Color(230, 82, 86, 92), 0.9f * u);
            g.DrawEllipse(&outerEdge, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);

            // Inner recessed cylindrical bezel wall (dark top shadow, bright bottom reflection)
            LinearGradientBrush recessWall(
                PointF(cx, cy - rInnerBezel), PointF(cx, cy + rInnerBezel),
                Color(255, 82, 84, 90), Color(255, 248, 250, 252));
            g.FillEllipse(&recessWall, cx - rInnerBezel, cy - rInnerBezel, rInnerBezel * 2.0f, rInnerBezel * 2.0f);

            // Cool mint-white dial (r = 49.5): RGB(236,249,243) -> RGB(186,190,188)
            LinearGradientBrush mintDial(
                PointF(cx, cy - rDial), PointF(cx, cy + rDial),
                Color(255, 236, 249, 243), Color(255, 186, 190, 188));
            g.FillEllipse(&mintDial, cx - rDial, cy - rDial, rDial * 2.0f, rDial * 2.0f);

            // Top inner cylindrical shadow cast onto the dial
            GraphicsPath shadowCrescent;
            shadowCrescent.AddArc(cx - rDial, cy - rDial, rDial * 2.0f, rDial * 2.0f, 180.0f, 180.0f);
            shadowCrescent.AddArc(cx - rDial * 1.02f, cy - rDial * 0.86f, rDial * 2.04f, rDial * 1.86f, 0.0f, -180.0f);
            shadowCrescent.CloseFigure();
            SolidBrush topCastShadow(Color(55, 40, 45, 50));
            g.FillPath(&topCastShadow, &shadowCrescent);

            // 12 dark hour dashes (r = 43.0..48.5) + 48 embossed minute dots (r = 46.5)
            SolidBrush dashBrush(Color(255, 58, 60, 62));
            SolidBrush dotDark(Color(210, 105, 110, 112));
            SolidBrush dotLight(Color(220, 252, 255, 254));
            for (int i = 0; i < 60; ++i) {
                float a = (i * 6.0f - 90.0f) * kPi / 180.0f;
                if (i % 5 == 0) {
                    PointF dash[4] = {
                        PointF(-0.95f * u, -48.5f * u),
                        PointF( 0.95f * u, -48.5f * u),
                        PointF( 0.95f * u, -43.0f * u),
                        PointF(-0.95f * u, -43.0f * u)
                    };
                    FillRotatedHandPolygon(g, cx, cy, a, dash, 4, dashBrush);
                } else {
                    float dR = 46.5f * u;
                    float dx = cx + std::cos(a) * dR;
                    float dy = cy + std::sin(a) * dR;
                    float pr = 0.85f * u;
                    g.FillEllipse(&dotLight, dx - pr + 0.3f * u, dy - pr + 0.4f * u, pr * 2.0f, pr * 2.0f);
                    g.FillEllipse(&dotDark,  dx - pr,            dy - pr,            pr * 2.0f, pr * 2.0f);
                }
            }

            if (customName && customName[0] != L'\0') {
                FontFamily segoe(L"Segoe UI");
                Font nameFont(&segoe, SnapFontSize(8.0f * u), FontStyleRegular, UnitPixel);
                RectF nameRc = SnapTextRect(cx - 28.0f * u, cy + 12.0f * u, 56.0f * u, 12.0f * u);
                g.DrawString(customName, -1, &nameFont, nameRc, &sf, &dashBrush);
            }

            // Thin black baton hands with long counterweights (system_h: -36..+21, system_m: -48..+17)
            SolidBrush blackHand(Color(255, 28, 30, 32));
            PointF hrPts[4] = {
                PointF(-0.85f * u,  21.0f * u),
                PointF(-0.85f * u, -36.0f * u),
                PointF( 0.85f * u, -36.0f * u),
                PointF( 0.85f * u,  21.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, hrAngle, hrPts, 4, blackHand, 1.0f * u, 1.5f * u, 60);

            PointF minPts[4] = {
                PointF(-0.80f * u,  17.0f * u),
                PointF(-0.80f * u, -48.0f * u),
                PointF( 0.80f * u, -48.0f * u),
                PointF( 0.80f * u,  17.0f * u)
            };
            FillRotatedHandPolygon(g, cx, cy, minAngle, minPts, 4, blackHand, 1.0f * u, 1.5f * u, 60);

            // Crimson red second hand (system_s: -48..+30)
            if (showSeconds) {
                SolidBrush redSec(Color(255, 188, 47, 55));
                PointF secPts[4] = {
                    PointF(-0.65f * u,  30.0f * u),
                    PointF(-0.55f * u, -48.0f * u),
                    PointF( 0.55f * u, -48.0f * u),
                    PointF( 0.65f * u,  30.0f * u)
                };
                FillRotatedHandPolygon(g, cx, cy, secAngle, secPts, 4, redSec, 0.9f * u, 1.3f * u, 50);
            }

            float dotR = 2.4f * u;
            g.FillEllipse(&blackHand, cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
            break;
        }
    }
}

void DrawClockGadget(Graphics& g) {
    if (!g_showClock) return;
    RectF rc = GetWidgetLocalBodyRect(WidgetClock);
    float cx = rc.X + rc.Width * 0.5f;
    float cy = rc.Y + rc.Height * 0.5f;
    float radius = rc.Width * (62.5f / 130.0f);

    SYSTEMTIME st = GetClockTime();
    RenderClockFaceAt(g, cx, cy, radius, g_clockSkin, st, g_showSeconds, g_clockName, true);
    DrawWin7HoverToolbar(g, WidgetClock);
}

// ── Authentic Windows 7 Calendar Gadget Renderer ─────────────────────────────

void DrawSpiralRings(Graphics& g, float x, float yTop, float s, bool isDoublePage) {
    // Authentic Windows 7 rings-desk.png (117x17) / rings-dock.png (117x13):
    // 11 twin-wire units spaced every 11px across x = 6..122 (in 130px widget coords).
    // Each unit is 7px wide:
    //   dx = 0..6: dark punched slot holes at top (if double) and bottom (orange page)
    //   dx = 1..2: left metallic vertical wire
    //   dx = 3:    center dark groove between the twin wires
    //   dx = 4..5: right metallic vertical wire
    //   dx = 6:    cast drop shadow on the right side
    for (int i = 0; i < 11; ++i) {
        float ux = x + (3.0f + i * 11.0f) * s;
        float ringH = (isDoublePage ? 14.0f : 11.0f) * s;
        float uy = isDoublePage ? (yTop - 7.0f * s) : (yTop - 3.5f * s);

        // Punched paper holes (dark brown-black slots on both sheets when expanded)
        SolidBrush holeBrush(Color(240, 32, 16, 10));
        if (isDoublePage) {
            g.FillRectangle(&holeBrush, ux + 0.3f * s, uy - 1.5f * s, 6.2f * s, 3.6f * s);
        }
        g.FillRectangle(&holeBrush, ux + 0.3f * s, uy + ringH - 2.2f * s, 6.2f * s, 3.4f * s);

        // Cast shadow to the bottom-right of the twin wire unit
        SolidBrush castShadow(Color(105, 25, 10, 5));
        g.FillRectangle(&castShadow, ux + 5.2f * s, uy + 1.2f * s, 1.8f * s, ringH);

        // Center dark separation groove (dx = 3)
        SolidBrush grooveBrush(Color(225, 48, 49, 52));
        g.FillRectangle(&grooveBrush, ux + 2.7f * s, uy, 1.3f * s, ringH);

        // Two parallel vertical metallic wires (left wire at dx=1..2.7, right wire at dx=3.9..5.6)
        for (int w = 0; w < 2; ++w) {
            float wx = ux + (w == 0 ? 1.0f : 3.9f) * s;
            RectF wireRc(wx, uy, 1.75f * s, ringH);

            LinearGradientBrush wireGrad(
                PointF(wireRc.X, wireRc.Y),
                PointF(wireRc.X, wireRc.Y + wireRc.Height),
                Color(255, 245, 245, 246),
                Color(255, 68, 69, 72));
            Color blendCols[4] = {
                Color(255, 118, 120, 124),
                Color(255, 248, 249, 250),
                Color(255, 175, 177, 180),
                Color(255, 62, 63, 66)
            };
            REAL blendPos[4] = {0.0f, 0.35f, 0.72f, 1.0f};
            wireGrad.SetInterpolationColors(blendCols, blendPos, 4);
            g.FillRectangle(&wireGrad, wireRc);
        }
    }
}

void DrawOrangeDayPage(Graphics& g, const RectF& pageRc, float s) {
    // 1. Soft drop shadow under the orange card
    for (int i = 4; i >= 1; --i) {
        float exp = i * 1.15f * s;
        SolidBrush sh(Color(static_cast<BYTE>(15 * (5 - i)), 0, 0, 0));
        FillRoundRect(g, RectF(pageRc.X - exp, pageRc.Y - exp + i * 1.0f * s,
                               pageRc.Width + exp * 2.0f, pageRc.Height + exp * 2.0f), 5.5f * s, sh);
    }

    // 2. Authentic right & bottom stacked cardboard/paper edge from calendar_single_orange.png:
    // Olive-tan paper stack edge RGB(134, 125, 84) + warm dark brown base rim
    FillRoundRect(g, RectF(pageRc.X + 1.2f * s, pageRc.Y + 2.2f * s, pageRc.Width, pageRc.Height),
                  4.5f * s, SolidBrush(Color(255, 112, 96, 58)));
    FillRoundRect(g, RectF(pageRc.X + 0.6f * s, pageRc.Y + 1.1f * s, pageRc.Width, pageRc.Height),
                  4.5f * s, SolidBrush(Color(255, 154, 138, 88)));

    // 3. Main Windows 7 Orange diagonal gradient (measured from calendar_single_orange.png:
    // top-left RGB(252, 101, 0) -> center RGB(224, 90, 0) -> bottom-right RGB(198, 78, 0))
    LinearGradientBrush orangeBrush(
        PointF(pageRc.X, pageRc.Y),
        PointF(pageRc.X + pageRc.Width, pageRc.Y + pageRc.Height),
        Color(255, 252, 102, 0),
        Color(255, 198, 76, 0));
    FillRoundRect(g, pageRc, 4.0f * s, orangeBrush);

    // Top & left inner highlight line RGB(247, 137, 64)
    RectF innerHiRc(pageRc.X + 0.8f * s, pageRc.Y + 0.8f * s, pageRc.Width - 1.6f * s, pageRc.Height - 1.6f * s);
    Pen innerHiPen(Color(150, 255, 155, 80), 0.9f * s);
    DrawRoundRect(g, innerHiRc, 3.5f * s, innerHiPen);

    // Crisp outer border
    Pen orangeBorder(Color(235, 158, 52, 0), 0.9f * s);
    DrawRoundRect(g, pageRc, 4.0f * s, orangeBorder);

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

    // Crisp ClearType text rendering on integer-snapped pixel boundaries
    TextRenderingHint oldHint = g.GetTextRenderingHint();
    PixelOffsetMode oldOffset = g.GetPixelOffsetMode();
    g.SetPixelOffsetMode(PixelOffsetModeDefault);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    FontFamily segoe(L"Segoe UI");
    StringFormat sf(StringFormat::GenericTypographic());
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);

    SolidBrush whiteBrush(Color(255, 255, 255, 255));

    wchar_t monthYearStr[64];
    swprintf_s(monthYearStr, L"%s %d", MonthNameGenitiveOrFull(dispMonth), dispYear);

    const wchar_t* dowStr = DayOfWeekName(dispDow);

    wchar_t dayNumStr[16];
    swprintf_s(dayNumStr, L"%d", dispDay);

    Font smallFont(&segoe, SnapFontSize(12.0f * s), FontStyleRegular, UnitPixel);
    Font bigDayFont(&segoe, SnapFontSize(56.0f * s), FontStyleRegular, UnitPixel);

    const wchar_t* topLine = g_isRussian ? dowStr : monthYearStr;
    const wchar_t* botLine = g_isRussian ? monthYearStr : dowStr;

    RectF topRc = SnapTextRect(pageRc.X + 4.0f * s, pageRc.Y + 12.0f * s, pageRc.Width - 8.0f * s, 20.0f * s);
    RectF midRc = SnapTextRect(pageRc.X + 2.0f * s, pageRc.Y + 29.0f * s, pageRc.Width - 4.0f * s, 72.0f * s);
    RectF botRc = SnapTextRect(pageRc.X + 4.0f * s, pageRc.Y + 102.0f * s, pageRc.Width - 8.0f * s, 20.0f * s);

    // No fractional drop-shadow on small strings so ClearType subpixel edges remain 100% razor-sharp!
    g.DrawString(topLine, -1, &smallFont, topRc, &sf, &whiteBrush);
    g.DrawString(dayNumStr, -1, &bigDayFont, midRc, &sf, &whiteBrush);
    g.DrawString(botLine, -1, &smallFont, botRc, &sf, &whiteBrush);

    g.SetPixelOffsetMode(oldOffset);
    g.SetTextRenderingHint(oldHint);
}

void DrawTopWhiteMonthSheet(Graphics& g, const RectF& sheetRc, float s) {
    // 1. Soft shadow under the top white card
    for (int i = 3; i >= 1; --i) {
        float exp = i * 1.1f * s;
        SolidBrush sh(Color(static_cast<BYTE>(15 * (4 - i)), 0, 0, 0));
        FillRoundRect(g, RectF(sheetRc.X - exp, sheetRc.Y - exp + i * 0.9f * s,
                               sheetRc.Width + exp * 2.0f, sheetRc.Height + exp * 2.0f), 5.0f * s, sh);
    }

    // 2. Right & bottom stacked paper edge (matching calendar_double_orange.png top sheet)
    FillRoundRect(g, RectF(sheetRc.X + 1.0f * s, sheetRc.Y + 1.5f * s, sheetRc.Width, sheetRc.Height),
                  4.5f * s, SolidBrush(Color(255, 145, 142, 132)));

    // 3. Diagonal off-white paper gradient (top-left RGB(255,255,255) -> bottom-right RGB(214,218,225))
    LinearGradientBrush paperBrush(
        PointF(sheetRc.X, sheetRc.Y),
        PointF(sheetRc.X + sheetRc.Width, sheetRc.Y + sheetRc.Height),
        Color(255, 255, 255, 255),
        Color(255, 214, 218, 225));
    FillRoundRect(g, sheetRc, 4.0f * s, paperBrush);

    Pen borderPen(Color(215, 155, 160, 168), 0.9f * s);
    DrawRoundRect(g, sheetRc, 4.0f * s, borderPen);

    // 4. Left / Right navigation arrows with authentic circular hover glow (bPrev-hot.png / bNext-hot.png)
    float arrY = std::round(sheetRc.Y + 13.0f * s);
    float leftCx  = std::round(sheetRc.X + 12.0f * s);
    float rightCx = std::round(sheetRc.X + sheetRc.Width - 12.0f * s);

    auto drawNavArrow = [&](float cx, float cy, bool isLeft, bool hovered) {
        if (hovered) {
            float glowR = 7.0f * s;
            RectF glowRc(cx - glowR, cy - glowR, glowR * 2.0f, glowR * 2.0f);
            LinearGradientBrush glowGrad(
                PointF(glowRc.X, glowRc.Y), PointF(glowRc.X, glowRc.Y + glowRc.Height),
                Color(240, 252, 253, 255), Color(240, 202, 212, 226));
            g.FillEllipse(&glowGrad, glowRc);
            Pen glowRim(Color(210, 158, 168, 182), 0.9f * s);
            g.DrawEllipse(&glowRim, glowRc);
        }
        SolidBrush arrBrush(hovered ? Color(255, 42, 48, 58) : Color(255, 132, 136, 144));
        if (isLeft) {
            PointF pts[3] = {
                PointF(cx - 2.5f * s, cy),
                PointF(cx + 2.2f * s, cy - 3.8f * s),
                PointF(cx + 2.2f * s, cy + 3.8f * s)
            };
            g.FillPolygon(&arrBrush, pts, 3);
        } else {
            PointF pts[3] = {
                PointF(cx + 2.5f * s, cy),
                PointF(cx - 2.2f * s, cy - 3.8f * s),
                PointF(cx - 2.2f * s, cy + 3.8f * s)
            };
            g.FillPolygon(&arrBrush, pts, 3);
        }
    };

    drawNavArrow(leftCx,  arrY, true,  g_hoveredBtn == CalBtnPrevMonth);
    drawNavArrow(rightCx, arrY, false, g_hoveredBtn == CalBtnNextMonth);

    // 5. Crisp ClearType text rendering for month title, weekday headers, and day numbers
    TextRenderingHint oldHint = g.GetTextRenderingHint();
    PixelOffsetMode oldOffset = g.GetPixelOffsetMode();
    g.SetPixelOffsetMode(PixelOffsetModeDefault);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    FontFamily segoe(L"Segoe UI");
    StringFormat sf(StringFormat::GenericTypographic());
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    sf.SetFormatFlags(StringFormatFlagsNoWrap);

    // Header: [<] "Sep 08" / "окт 26" [>]
    wchar_t headerBuf[32];
    swprintf_s(headerBuf, L"%s %02d", MonthNameShort(g_calViewMonth), g_calViewYear % 100);

    Font headerFont(&segoe, SnapFontSize(11.0f * s), FontStyleRegular, UnitPixel);
    SolidBrush headerBrush(Color(255, 28, 32, 38));
    RectF headerRc = SnapTextRect(sheetRc.X + 20.0f * s, sheetRc.Y + 5.0f * s, sheetRc.Width - 40.0f * s, 16.0f * s);
    g.DrawString(headerBuf, -1, &headerFont, headerRc, &sf, &headerBrush);

    // Weekday headers (П В С Ч П С В / S M T W T F S)
    static const wchar_t* kDowRu[7] = {L"П", L"В", L"С", L"Ч", L"П", L"С", L"В"};
    static const wchar_t* kDowEn[7] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};

    SYSTEMTIME today;
    GetLocalTime(&today);

    // Snap grid origin and column width to integer pixels so every column has identical crisp alignment
    float gridLeft = std::round(sheetRc.X + 5.0f * s);
    float gridRight = std::round(sheetRc.X + sheetRc.Width - 5.0f * s);
    float gridWidth = gridRight - gridLeft;
    float colW = gridWidth / 7.0f;
    float dowY = std::round(sheetRc.Y + 22.0f * s);
    float startY = std::round(sheetRc.Y + 37.0f * s);
    float rowH = std::max(11.0f, std::round(14.5f * s));

    // Crisp 1px vertical column separator lines
    Pen colSepPen(Color(110, 198, 202, 208), 1.0f);
    for (int col = 1; col < 7; ++col) {
        float lx = std::round(gridLeft + col * colW);
        g.DrawLine(&colSepPen, lx, dowY + 1.0f, lx, startY + rowH * 6.0f - 2.0f);
    }

    Font dowFont(&segoe, SnapFontSize(10.0f * s), FontStyleBold, UnitPixel);
    for (int col = 0; col < 7; ++col) {
        int colDow = g_isRussian ? ((col + 1) % 7) : col;
        bool isCurrentDow = (today.wYear == g_calViewYear && today.wMonth == g_calViewMonth && today.wDayOfWeek == colDow);
        SolidBrush dowBrush(isCurrentDow ? Color(255, 228, 92, 12) : Color(255, 112, 116, 122));
        float cLeft = std::round(gridLeft + col * colW);
        float cRight = std::round(gridLeft + (col + 1) * colW);
        RectF cellRc(cLeft, dowY, cRight - cLeft, std::round(14.0f * s));
        g.DrawString(g_isRussian ? kDowRu[col] : kDowEn[col], -1, &dowFont, cellRc, &sf, &dowBrush);
    }

    // Days grid (6 rows x 7 columns)
    int firstDow = DayOfWeekForDate(g_calViewYear, g_calViewMonth, 1);
    int leadCount = g_isRussian ? ((firstDow + 6) % 7) : firstDow;

    int prevMonth = (g_calViewMonth == 1) ? 12 : (g_calViewMonth - 1);
    int prevYear  = (g_calViewMonth == 1) ? (g_calViewYear - 1) : g_calViewYear;
    int daysInPrev = DaysInMonth(prevYear, prevMonth);
    int daysInCurr = DaysInMonth(g_calViewYear, g_calViewMonth);

    Font numFont(&segoe, SnapFontSize(10.0f * s), FontStyleRegular, UnitPixel);
    Font numBoldFont(&segoe, SnapFontSize(10.0f * s), FontStyleBold, UnitPixel);

    SolidBrush currMonthBrush(Color(255, 20, 24, 30));
    SolidBrush otherMonthBrush(Color(255, 170, 175, 182));
    SolidBrush whiteText(Color(255, 255, 255, 255));

    for (int cell = 0; cell < 42; ++cell) {
        int row = cell / 7;
        int col = cell % 7;
        float cLeft = std::round(gridLeft + col * colW);
        float cRight = std::round(gridLeft + (col + 1) * colW);
        float rTop = std::round(startY + row * rowH);
        RectF cellRc(cLeft, rTop, cRight - cLeft, rowH);

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
                // Authentic bg-today.png highlight box
                RectF hiRc(cellRc.X + 1.0f, cellRc.Y + 1.0f, cellRc.Width - 2.0f, cellRc.Height - 2.0f);
                SolidBrush orangeBox(Color(255, 232, 98, 14));
                g.FillRectangle(&orangeBox, hiRc);
                Pen orangeRim(Color(255, 185, 62, 4), 1.0f);
                g.DrawRectangle(&orangeRim, hiRc.X, hiRc.Y, hiRc.Width, hiRc.Height);
                g.DrawString(label, -1, &numBoldFont, cellRc, &sf, &whiteText);
            } else if (isSelected) {
                RectF hiRc(cellRc.X + 1.0f, cellRc.Y + 1.0f, cellRc.Width - 2.0f, cellRc.Height - 2.0f);
                Pen selRim(Color(255, 232, 98, 14), 1.0f);
                g.DrawRectangle(&selRim, hiRc.X, hiRc.Y, hiRc.Width, hiRc.Height);
                g.DrawString(label, -1, &numBoldFont, cellRc, &sf, &currMonthBrush);
            } else {
                g.DrawString(label, -1, &numFont, cellRc, &sf, &currMonthBrush);
            }
        } else {
            g.DrawString(label, -1, &numFont, cellRc, &sf, &otherMonthBrush);
        }
    }

    g.SetPixelOffsetMode(oldOffset);
    g.SetTextRenderingHint(oldHint);
}

void DrawCalendarGadget(Graphics& g) {
    if (!g_showCalendar) return;

    const float s = GetWidgetScale(WidgetCalendar);
    RectF body = GetWidgetLocalBodyRect(WidgetCalendar);

    if (!g_calendarExpanded) {
        // Collapsed: Single orange page (123x127 in 130x146 body) with spiral rings at the top
        RectF orangeRc(std::round(body.X + 3.0f * s), std::round(body.Y + 8.0f * s),
                       std::round(123.0f * s), std::round(129.0f * s));
        DrawOrangeDayPage(g, orangeRc, s);
        DrawSpiralRings(g, orangeRc.X, orangeRc.Y, s, false);
    } else {
        // Expanded: Top white month sheet + Bottom orange day page joined by spiral rings
        RectF topSheetRc(std::round(body.X + 3.0f * s), std::round(body.Y + 4.0f * s),
                         std::round(123.0f * s), std::round(134.0f * s));
        RectF botOrangeRc(std::round(body.X + 3.0f * s), std::round(body.Y + 143.0f * s),
                          std::round(123.0f * s), std::round(129.0f * s));

        DrawTopWhiteMonthSheet(g, topSheetRc, s);
        DrawOrangeDayPage(g, botOrangeRc, s);
        DrawSpiralRings(g, botOrangeRc.X, std::round(body.Y + 140.5f * s), s, true);
    }

    DrawWin7HoverToolbar(g, WidgetCalendar);
}

// ── Authentic Windows 7 CPU & RAM Meter Renderer (Small & Large) ─────────────

struct CpuMeterLayout {
    float cpuCx, cpuCy, cpuOuterR, cpuInnerR;
    float cpuHubCx, cpuHubCy, cpuHubR;
    RectF cpuLcd;
    float cpuLcdRadius, cpuFontPx;
    bool cpuHasStopPin;
    float cpuStopPinR;
    float cpuNeedleTipY, cpuNeedleShoulderY, cpuNeedleBladeHalfW;
    float cpuNeedleStepOutY, cpuNeedleStepInY, cpuNeedleBaseY, cpuNeedleBaseHalfW;

    float ramCx, ramCy, ramOuterR, ramInnerR;
    float ramHubCx, ramHubCy, ramHubR, ramHubClipBottomY;
    RectF ramLcd;
    float ramLcdRadius, ramFontPx;
    float ramStopPinR;
    float ramNeedlePivotCx, ramNeedlePivotCy;
    float ramNeedleTipY, ramNeedleShoulderY, ramNeedleBladeHalfW;
    float ramNeedleStepOutY, ramNeedleStepInY, ramNeedleBaseY, ramNeedleBaseHalfW;
};

CpuMeterLayout GetCpuMeterLayout(bool expanded) {
    if (!expanded) {
        // Sub-pixel measurements from back.png (130x103), dialdot.png, dial.png, dial_sml.png
        return {
            42.0f, 57.5f, 39.5f, 33.8f,
            42.0f, 57.5f, 14.0f,
            RectF(30.5f, 78.5f, 23.0f, 12.0f), 1.6f, 9.0f,
            true, 2.0f,
            -27.0f, -23.5f, 2.85f, -8.5f, -7.0f, 0.0f, 1.95f,

            95.0f, 34.0f, 29.8f, 25.0f,
            95.0f, 35.5f, 11.0f, 44.2f,
            RectF(83.5f, 44.8f, 23.5f, 11.0f), 1.5f, 8.5f,
            1.85f,
            95.0f, 34.5f,
            -18.5f, -14.8f, 2.80f, -1.5f, 0.0f, 7.5f, 1.90f
        };
    } else {
        // Sub-pixel measurements from back_lrg.png, dialdot_lrg.png, dial_lrg.png, dial_lrg_sml.png
        // (shifted by (-8.0, -4.0) so the 172x132 body sits centered in 180x142)
        return {
            60.5f, 79.5f, 55.5f, 47.8f,
            60.8f, 79.5f, 15.0f,
            RectF(44.8f, 103.0f, 32.0f, 14.8f), 1.8f, 11.0f,
            true, 2.55f,
            -32.5f, -28.0f, 3.85f, -9.5f, -8.0f, 0.0f, 2.85f,

            134.5f, 47.0f, 42.0f, 35.8f,
            134.8f, 47.0f, 13.0f, 999.0f,
            RectF(119.8f, 62.0f, 30.0f, 13.0f), 1.7f, 10.0f,
            2.30f,
            134.5f, 47.0f,
            -24.5f, -20.2f, 3.80f, -1.5f, 0.0f, 8.0f, 2.80f
        };
    }
}

void ApplyWin7StudioChromeStops(LinearGradientBrush& brush) {
    // 11-stop vertical studio-chrome reflection measured every 15 deg from back_lrg.png
    const Color cols[11] = {
        Color(255,  88,  96,  99),
        Color(255, 138, 144, 147),
        Color(255, 210, 212, 213),
        Color(255, 252, 252, 252),
        Color(255, 164, 168, 169),
        Color(255,  74,  81,  84),
        Color(255, 155, 159, 160),
        Color(255, 250, 250, 250),
        Color(255, 216, 218, 219),
        Color(255, 148, 153, 156),
        Color(255,  95, 102, 105)
    };
    const REAL pos[11] = {
        0.00f, 0.07f, 0.15f, 0.25f, 0.37f, 0.50f, 0.63f, 0.75f, 0.85f, 0.93f, 1.00f
    };
    brush.SetInterpolationColors(cols, pos, 11);
}

Color EvalWarningZoneColor(float deg, bool isCpuDial, BYTE* outAlpha) {
    auto lerpByte = [](int a, int b, float t) -> BYTE {
        t = ClampF(t, 0.0f, 1.0f);
        return static_cast<BYTE>(std::round(a + (b - a) * t));
    };

    if (isCpuDial) {
        // CPU warning zone: soft cream-yellow at +13 deg -> rich amber (+50) -> orange (+82) -> coral-red (+125)
        if (deg < 12.0f || deg > 129.0f) {
            *outAlpha = 0;
            return Color(0, 255, 255, 255);
        }
        BYTE a = 255;
        if (deg < 28.0f) {
            a = lerpByte(0, 255, (deg - 12.0f) / 16.0f);
        } else if (deg > 122.0f) {
            a = lerpByte(255, 0, (deg - 122.0f) / 7.0f);
        }
        *outAlpha = a;

        if (deg <= 52.0f) {
            float t = (deg - 12.0f) / 40.0f;
            return Color(a, 255, lerpByte(238, 204, t), lerpByte(185, 116, t));
        } else if (deg <= 86.0f) {
            float t = (deg - 52.0f) / 34.0f;
            return Color(a, 255, lerpByte(204, 122, t), lerpByte(116, 66, t));
        } else {
            float t = (deg - 86.0f) / 32.0f;
            return Color(a, 255, lerpByte(122, 74, t), lerpByte(66, 56, t));
        }
    } else {
        // RAM warning zone: starts around +56 deg -> amber at +88 deg -> orange -> coral-red
        if (deg < 54.0f || deg > 129.0f) {
            *outAlpha = 0;
            return Color(0, 255, 255, 255);
        }
        BYTE a = 255;
        if (deg < 70.0f) {
            a = lerpByte(0, 255, (deg - 54.0f) / 16.0f);
        } else if (deg > 122.0f) {
            a = lerpByte(255, 0, (deg - 122.0f) / 7.0f);
        }
        *outAlpha = a;

        if (deg <= 88.0f) {
            float t = (deg - 54.0f) / 34.0f;
            return Color(a, 255, lerpByte(234, 204, t), lerpByte(175, 118, t));
        } else if (deg <= 104.0f) {
            float t = (deg - 88.0f) / 16.0f;
            return Color(a, 255, lerpByte(204, 124, t), lerpByte(118, 66, t));
        } else {
            float t = (deg - 104.0f) / 18.0f;
            return Color(a, 255, lerpByte(124, 74, t), lerpByte(66, 56, t));
        }
    }
}

void DrawCpuMeterDialInterior(Graphics& g, float cx, float cy, float innerR, float s,
                              bool isCpuDial, bool expanded,
                              bool hasStopPin, float stopPinR,
                              const RectF& lcdRc, float lcdCornerR, float fontPx, int pctValue) {
    GraphicsState st = g.Save();

    GraphicsPath dialClip;
    dialClip.AddEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);
    g.SetClip(&dialClip);

    // 1. Crisp porcelain-white dial base
    SolidBrush whiteFace(Color(255, 255, 255, 255));
    g.FillEllipse(&whiteFace, cx - innerR - 1.0f, cy - innerR - 1.0f, (innerR + 1.0f) * 2.0f, (innerR + 1.0f) * 2.0f);

    // 2. Smoothly feathered Yellow -> Orange -> Coral-Red warning zone (0.68..1.00 * innerR)
    const float startDeg = isCpuDial ? 12.0f : 54.0f;
    const float stepDeg = 1.5f;
    RectF pieRc(cx - innerR * 1.03f, cy - innerR * 1.03f, innerR * 2.06f, innerR * 2.06f);
    for (float deg = startDeg; deg < 129.0f; deg += stepDeg) {
        BYTE alpha = 0;
        Color c = EvalWarningZoneColor(deg + stepDeg * 0.5f, isCpuDial, &alpha);
        if (alpha > 0) {
            SolidBrush wb(c);
            g.FillPie(&wb, pieRc, deg - 90.0f - 0.25f, stepDeg + 0.55f);
        }
    }

    // Smooth radial feather from vivid warning rim into pure porcelain-white center disc
    {
        GraphicsPath innerWhitePath;
        float cutR = innerR * 0.745f;
        innerWhitePath.AddEllipse(cx - cutR, cy - cutR, cutR * 2.0f, cutR * 2.0f);
        PathGradientBrush pgb(&innerWhitePath);
        pgb.SetCenterPoint(PointF(cx, cy));
        pgb.SetCenterColor(Color(255, 255, 255, 255));
        const Color fadeCols[4] = {
            Color(0,   255, 255, 255),
            Color(135, 255, 255, 255),
            Color(255, 255, 255, 255),
            Color(255, 255, 255, 255)
        };
        REAL fadePos[4] = {0.0f, 0.08f, 0.18f, 1.0f};
        pgb.SetInterpolationColors(fadeCols, fadePos, 4);
        g.FillPath(&pgb, &innerWhitePath);
    }

    // 3. CPU 0% solid maroon block (-133.8 deg to -123.3 deg, sitting cleanly below the -120 deg 0% tick)
    if (isCpuDial) {
        float rOut = innerR * 0.992f;
        float rIn  = innerR * 0.720f;
        float a0 = -133.8f - 90.0f;
        float sweep = 10.5f;
        GraphicsPath zeroBlock;
        zeroBlock.AddArc(cx - rOut, cy - rOut, rOut * 2.0f, rOut * 2.0f, a0, sweep);
        zeroBlock.AddArc(cx - rIn,  cy - rIn,  rIn  * 2.0f, rIn  * 2.0f, a0 + sweep, -sweep);
        zeroBlock.CloseFigure();
        SolidBrush zeroBrush(Color(255, 179, 61, 60));
        g.FillPath(&zeroBrush, &zeroBlock);
    }

    // 4. 11 inward-tapering sharp maroon scale ticks (-120 deg to +120 deg in 24 deg steps)
    {
        SolidBrush tickBrush(Color(255, 156, 30, 30));
        float rOut = innerR * 0.992f;
        float rIn  = innerR * 0.715f;
        float baseHalfW = (isCpuDial ? (expanded ? 1.12f : 0.90f) : (expanded ? 0.92f : 0.76f)) * s;
        float tipHalfW  = 0.14f * s;

        for (int i = 0; i <= 10; ++i) {
            float deg = -120.0f + i * 24.0f;
            float rad = (deg - 90.0f) * kPi / 180.0f;
            PointF tickPts[4] = {
                PointF(-baseHalfW, -rOut),
                PointF( baseHalfW, -rOut),
                PointF( tipHalfW,  -rIn),
                PointF(-tipHalfW,  -rIn)
            };
            FillRotatedHandPolygon(g, cx, cy, rad, tickPts, 4, tickBrush);
        }
    }

    // 5. Authentic soft recessed 3D inner-bezel shadow around the white dial face
    {
        GraphicsPath rimShadowPath;
        float shR = innerR + 0.8f * s;
        rimShadowPath.AddEllipse(cx - shR, cy - shR, shR * 2.0f, shR * 2.0f);
        PathGradientBrush rimPgb(&rimShadowPath);
        rimPgb.SetCenterPoint(PointF(cx + 0.8f * s, cy + 1.4f * s));
        rimPgb.SetCenterColor(Color(0, 72, 75, 78));
        const Color rimCols[5] = {
            Color(125, 68, 70, 74),
            Color(52,  82, 85, 88),
            Color(14,  92, 95, 98),
            Color(0,   92, 95, 98),
            Color(0,   92, 95, 98)
        };
        REAL rimPos[5] = {0.0f, 0.05f, 0.12f, 0.20f, 1.0f};
        rimPgb.SetInterpolationColors(rimCols, rimPos, 5);
        g.FillPath(&rimPgb, &rimShadowPath);
    }

    // 6. Light pearl-silver mechanical needle stop-pin at -121 deg (covered at 0%, revealed above ~8%)
    if (hasStopPin && stopPinR > 0.0f) {
        float pinRad = (-121.0f - 90.0f) * kPi / 180.0f;
        float pinDist = innerR * 0.555f;
        float px = cx + std::cos(pinRad) * pinDist;
        float py = cy + std::sin(pinRad) * pinDist;
        float pr = stopPinR * s;

        SolidBrush pinSh(Color(36, 0, 0, 0));
        g.FillEllipse(&pinSh, px - pr + 0.35f * s, py - pr + 0.65f * s, pr * 2.0f, pr * 2.0f);

        LinearGradientBrush pinGrad(
            PointF(px - pr * 0.5f, py - pr), PointF(px + pr * 0.5f, py + pr),
            Color(255, 236, 239, 243), Color(255, 196, 199, 204));
        g.FillEllipse(&pinGrad, px - pr, py - pr, pr * 2.0f, pr * 2.0f);

        Pen pinRim(Color(175, 172, 175, 180), 0.65f * s);
        g.DrawEllipse(&pinRim, px - pr, py - pr, pr * 2.0f, pr * 2.0f);
    }

    // 7. Exact 5-stop smoky-glass black digital LCD box measured from back.png / back_lrg.png
    {
        RectF bevelRc(lcdRc.X - 0.45f * s, lcdRc.Y + 0.55f * s, lcdRc.Width + 0.9f * s, lcdRc.Height);
        Pen lightBevel(Color(165, 232, 234, 236), 0.80f * s);
        DrawRoundRect(g, bevelRc, lcdCornerR * s, lightBevel);

        LinearGradientBrush lcdGrad(
            PointF(lcdRc.X, lcdRc.Y),
            PointF(lcdRc.X + lcdRc.Width, lcdRc.Y + lcdRc.Height),
            Color(255, 85, 84, 84),
            Color(255, 2, 2, 2));
        const Color lcdCols[5] = {
            Color(255,  85,  84,  84),
            Color(255, 119, 118, 118),
            Color(255,  76,  75,  75),
            Color(255,  27,  26,  26),
            Color(255,   2,   2,   2)
        };
        const REAL lcdPos[5] = {0.00f, 0.25f, 0.50f, 0.75f, 1.00f};
        lcdGrad.SetInterpolationColors(lcdCols, lcdPos, 5);
        FillRoundRect(g, lcdRc, lcdCornerR * s, lcdGrad);

        Pen lcdBorder(Color(235, 36, 37, 39), 0.85f * s);
        DrawRoundRect(g, lcdRc, lcdCornerR * s, lcdBorder);

        wchar_t pctText[16];
        int clampedPct = std::max(0, std::min(100, pctValue));
        swprintf_s(pctText, L"%d%%", clampedPct);

        float effFontPx = fontPx * s;
        if (clampedPct >= 100 && !expanded) {
            effFontPx *= 0.88f;
        }
        float snappedPx = std::max(6.0f, std::round(effFontPx));

        TextRenderingHint oldHint = g.GetTextRenderingHint();
        PixelOffsetMode oldOffset = g.GetPixelOffsetMode();
        g.SetPixelOffsetMode(PixelOffsetModeDefault);
        g.SetTextRenderingHint(snappedPx >= 8.0f ? TextRenderingHintClearTypeGridFit
                                                 : TextRenderingHintAntiAliasGridFit);

        FontFamily segoe(L"Segoe UI");
        Font lcdFont(&segoe, snappedPx, FontStyleRegular, UnitPixel);
        StringFormat sf(StringFormat::GenericTypographic());
        sf.SetAlignment(StringAlignmentCenter);
        sf.SetLineAlignment(StringAlignmentCenter);
        sf.SetFormatFlags(StringFormatFlagsNoWrap);

        SolidBrush lcdWhite(Color(255, 248, 250, 252));
        RectF textRc = SnapTextRect(lcdRc.X - 1.0f * s, lcdRc.Y, lcdRc.Width + 2.0f * s, lcdRc.Height + 0.4f * s);
        g.DrawString(pctText, -1, &lcdFont, textRc, &sf, &lcdWhite);

        g.SetPixelOffsetMode(oldOffset);
        g.SetTextRenderingHint(oldHint);
    }

    g.Restore(st);

    // Subtle medium-slate inner bezel seam + bottom highlight arc
    Pen innerLip(Color(215, 116, 120, 124), 0.90f * s);
    g.DrawEllipse(&innerLip, cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);

    Pen bottomRimLight(Color(145, 244, 246, 248), 0.85f * s);
    g.DrawArc(&bottomRimLight, cx - innerR - 0.8f * s, cy - innerR - 0.8f * s,
              (innerR + 0.8f * s) * 2.0f, (innerR + 0.8f * s) * 2.0f, 20.0f, 140.0f);
}

void DrawCpuMeterNeedle(Graphics& g, float dialCx, float dialCy, float innerR,
                        float pivotCx, float pivotCy, float pct, float s,
                        float tipY, float shoulderY, float bladeHalfW,
                        float stepOutY, float stepInY, float baseY, float baseHalfW) {
    GraphicsState st = g.Save();
    GraphicsPath dialClip;
    dialClip.AddEllipse(dialCx - innerR + 0.6f * s, dialCy - innerR + 0.6f * s,
                        (innerR - 0.6f * s) * 2.0f, (innerR - 0.6f * s) * 2.0f);
    g.SetClip(&dialClip);

    float clamped = ClampF(pct, 0.0f, 100.0f);
    float deg = -120.0f + clamped * 2.4f;
    float rad = (deg - 90.0f) * kPi / 180.0f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    // Exact 8-point stepped parallel blade + short chevron tip from dial*.png pixel scans
    const float tipHalfW = 0.55f * s;
    PointF localPts[8] = {
        PointF(-tipHalfW,       tipY * s),
        PointF( tipHalfW,       tipY * s),
        PointF( bladeHalfW * s, shoulderY * s),
        PointF( bladeHalfW * s, stepOutY * s),
        PointF( baseHalfW * s,  stepInY * s),
        PointF( baseHalfW * s,  baseY * s),
        PointF(-baseHalfW * s,  baseY * s),
        PointF(-baseHalfW * s,  stepInY * s)
    };
    // Add symmetric left step-out & left shoulder in 9-point polygon
    PointF fullLocal[9] = {
        localPts[0], localPts[1], localPts[2], localPts[3],
        localPts[4], localPts[5], localPts[6], localPts[7],
        PointF(-bladeHalfW * s, stepOutY * s)
    };
    // Wait: connect (-baseHalfW, stepInY) -> (-bladeHalfW, stepOutY) -> (-bladeHalfW, shoulderY) -> (-tipHalfW, tipY)
    PointF polyLocal[10] = {
        PointF(-tipHalfW,       tipY * s),
        PointF( tipHalfW,       tipY * s),
        PointF( bladeHalfW * s, shoulderY * s),
        PointF( bladeHalfW * s, stepOutY * s),
        PointF( baseHalfW * s,  stepInY * s),
        PointF( baseHalfW * s,  baseY * s),
        PointF(-baseHalfW * s,  baseY * s),
        PointF(-baseHalfW * s,  stepInY * s),
        PointF(-bladeHalfW * s, stepOutY * s),
        PointF(-bladeHalfW * s, shoulderY * s)
    };
    (void)fullLocal;

    PointF worldPts[10];
    PointF shPts[10];
    for (int i = 0; i < 10; ++i) {
        float lx = polyLocal[i].X;
        float ly = polyLocal[i].Y;
        float wx = pivotCx - lx * sinA - ly * cosA;
        float wy = pivotCy + lx * cosA - ly * sinA;
        worldPts[i] = PointF(wx, wy);
        shPts[i] = PointF(wx + 0.45f * s, wy + 0.85f * s);
    }

    SolidBrush shBrush(Color(32, 0, 0, 0));
    g.FillPolygon(&shBrush, shPts, 10);

    PointF tipWorld(pivotCx - (tipY * s - 1.0f * s) * cosA,
                    pivotCy - (tipY * s - 1.0f * s) * sinA);
    PointF baseWorld(pivotCx - (baseY * s + 1.0f * s) * cosA,
                     pivotCy - (baseY * s + 1.0f * s) * sinA);
    LinearGradientBrush needleGrad(tipWorld, baseWorld,
                                   Color(255, 214, 4, 4),
                                   Color(255, 154, 1, 1));
    g.FillPolygon(&needleGrad, worldPts, 10);

    g.Restore(st);
}

void DrawCpuMeterHubCap(Graphics& g, float hubCx, float hubCy, float hubR, float s,
                        bool isCpuChip, bool expanded, float clipBottomY, float bodyOriginY) {
    GraphicsState st = g.Save();

    if (clipBottomY < 500.0f) {
        float clipMaxY = bodyOriginY + clipBottomY * s;
        RectF clipRc(hubCx - hubR - 4.0f * s, hubCy - hubR - 4.0f * s,
                     (hubR + 4.0f * s) * 2.0f, (clipMaxY - (hubCy - hubR - 4.0f * s)));
        g.SetClip(clipRc);
    }

    // 1. Soft drop shadow beneath glass hub button
    for (int i = 3; i >= 1; --i) {
        float ex = i * 0.48f * s;
        SolidBrush sh(Color(static_cast<BYTE>(15 * (4 - i)), 0, 0, 0));
        g.FillEllipse(&sh, hubCx - hubR - ex, hubCy - hubR - ex + i * 0.55f * s,
                      (hubR + ex) * 2.0f, (hubR + ex) * 2.0f);
    }

    // 2. Soft pearl-silver matte button body (matching dialdot.png / dialdot_lrg.png)
    LinearGradientBrush domeGrad(
        PointF(hubCx - hubR * 0.4f, hubCy - hubR),
        PointF(hubCx + hubR * 0.4f, hubCy + hubR),
        Color(255, 249, 250, 251),
        Color(255, 204, 207, 211));
    g.FillEllipse(&domeGrad, hubCx - hubR, hubCy - hubR, hubR * 2.0f, hubR * 2.0f);

    // 3. Detailed Silicon Chip Icon (exact pixel match to dialdot.png / dialdot_lrg.png)
    if (isCpuChip) {
        float pkgSz = (expanded ? 15.2f : 13.6f) * s;
        float ix = std::round(hubCx - pkgSz * 0.5f);
        float iy = std::round(hubCy - pkgSz * 0.5f - 0.3f * s);

        // Subtle package drop shadow on the silver dome
        SolidBrush pkgSh(Color(45, 0, 0, 0));
        g.FillRectangle(&pkgSh, ix + 0.6f * s, iy + 0.8f * s, pkgSz, pkgSz);

        // Square dark charcoal PGA processor substrate
        RectF pkgRc(ix, iy, pkgSz, pkgSz);
        LinearGradientBrush pcbGrad(
            PointF(ix, iy), PointF(ix + pkgSz, iy + pkgSz),
            Color(255, 86, 89, 93), Color(255, 38, 40, 43));
        FillRoundRect(g, pkgRc, 0.9f * s, pcbGrad);
        Pen pcbRim(Color(255, 42, 44, 47), 0.7f * s);
        DrawRoundRect(g, pkgRc, 0.9f * s, pcbRim);

        // Bright gold contact pins along right and bottom inner border + corner bevel dot
        SolidBrush goldPin(Color(255, 232, 182, 54));
        for (int p = 0; p < 5; ++p) {
            float off = (1.5f + p * (expanded ? 2.35f : 2.08f)) * s;
            g.FillRectangle(&goldPin, ix + pkgSz - 1.7f * s, iy + off, 1.15f * s, 1.10f * s);
            g.FillRectangle(&goldPin, ix + off, iy + pkgSz - 1.7f * s, 1.10f * s, 1.15f * s);
        }
        g.FillRectangle(&goldPin, ix + pkgSz - 2.9f * s, iy + pkgSz - 2.9f * s, 1.35f * s, 1.35f * s);

        // Inner metallic silver IHS heatspreader square (shifted toward top-left to expose dark trench + gold pins)
        float ihsSz = pkgSz * 0.64f;
        RectF ihsRc(ix + 1.4f * s, iy + 1.4f * s, ihsSz, ihsSz);
        LinearGradientBrush ihsGrad(
            PointF(ihsRc.X, ihsRc.Y), PointF(ihsRc.X + ihsRc.Width, ihsRc.Y + ihsRc.Height),
            Color(255, 240, 242, 244), Color(255, 162, 166, 170));
        FillRoundRect(g, ihsRc, 0.6f * s, ihsGrad);
        Pen ihsRim(Color(200, 72, 75, 79), 0.6f * s);
        DrawRoundRect(g, ihsRc, 0.6f * s, ihsRim);
    } else {
        // Bold vertical RAM DIP IC with crisp white-silver inner outline, 6x2 horizontal leads & top notch
        float chipW = (expanded ? 9.6f : 8.4f) * s;
        float chipH = (expanded ? 15.0f : 12.6f) * s;
        float cyShift = (clipBottomY < 500.0f) ? (-1.7f * s) : (0.0f);
        float rx = std::round(hubCx - chipW * 0.5f);
        float ry = std::round(hubCy - chipH * 0.5f + cyShift);
        float pinW = (expanded ? 2.1f : 1.85f) * s;

        // Subtle dark backing pad behind pins so the 6 bright silver leads stand out crisply
        RectF padRc(rx - pinW, ry + 0.8f * s, chipW + pinW * 2.0f, chipH - 1.6f * s);
        SolidBrush padDark(Color(220, 56, 58, 62));
        FillRoundRect(g, padRc, 0.7f * s, padDark);

        // 6 pairs of crisp bright-silver horizontal IC leads
        SolidBrush pinBright(Color(255, 240, 242, 245));
        float pinH = (expanded ? 1.15f : 0.98f) * s;
        float pinStart = ry + (expanded ? 1.6f : 1.3f) * s;
        float pinStep = (chipH - (expanded ? 3.2f : 2.6f) * s - pinH) / 5.0f;
        for (int p = 0; p < 6; ++p) {
            float py = pinStart + p * pinStep;
            g.FillRectangle(&pinBright, rx - pinW + 0.15f * s, py, pinW, pinH);
            g.FillRectangle(&pinBright, rx + chipW - 0.15f * s, py, pinW, pinH);
        }

        // Dark charcoal IC package body
        RectF icRc(rx, ry, chipW, chipH);
        LinearGradientBrush icGrad(
            PointF(rx, ry), PointF(rx + chipW, ry + chipH),
            Color(255, 62, 65, 69), Color(255, 30, 32, 35));
        FillRoundRect(g, icRc, 0.8f * s, icGrad);

        // Crisp light-silver inner rectangular border frame (signature detail of dialdot_lrg.png!)
        RectF innerFrame(rx + 1.3f * s, ry + 1.3f * s, chipW - 2.6f * s, chipH - 2.6f * s);
        Pen framePen(Color(230, 212, 215, 220), 0.82f * s);
        g.DrawRectangle(&framePen, innerFrame.X, innerFrame.Y, innerFrame.Width, innerFrame.Height);

        // Top half-circle orientation notch interrupting top edge
        float notchR = (expanded ? 1.55f : 1.35f) * s;
        SolidBrush notchFill(Color(255, 48, 50, 54));
        g.FillPie(&notchFill, hubCx - notchR, ry - notchR * 0.4f, notchR * 2.0f, notchR * 2.0f, 0.0f, 180.0f);
        Pen notchRim(Color(230, 212, 215, 220), 0.80f * s);
        g.DrawArc(&notchRim, hubCx - notchR, ry - notchR * 0.4f, notchR * 2.0f, notchR * 2.0f, 10.0f, 160.0f);

        // Two subtle internal die bars inside the dark core
        SolidBrush dieMark(Color(175, 138, 142, 148));
        g.FillRectangle(&dieMark, hubCx - 0.85f * s, ry + chipH * 0.34f, 1.7f * s, 0.9f * s);
        g.FillRectangle(&dieMark, hubCx - 0.85f * s, ry + chipH * 0.58f, 1.7f * s, 0.9f * s);
    }

    // 4. Subtle diagonal curved glass specular highlight across upper-left of the dome
    {
        GraphicsState glareSt = g.Save();
        GraphicsPath hubClip;
        float innerGlassR = hubR - 0.7f * s;
        hubClip.AddEllipse(hubCx - innerGlassR, hubCy - innerGlassR, innerGlassR * 2.0f, innerGlassR * 2.0f);
        g.SetClip(&hubClip, CombineModeIntersect);

        LinearGradientBrush glareGrad(
            PointF(hubCx - hubR * 0.85f, hubCy - hubR * 0.85f),
            PointF(hubCx + hubR * 0.10f, hubCy + hubR * 0.10f),
            Color(82, 255, 255, 255),
            Color(0, 255, 255, 255));
        g.FillEllipse(&glareGrad,
                      hubCx - hubR * 1.35f,
                      hubCy - hubR * 1.35f,
                      hubR * 2.05f,
                      hubR * 1.80f);
        g.Restore(glareSt);
    }

    // 5. Delicate light outer & inner rim (soft silver bevel, no harsh dark ring)
    Pen innerHighlight(Color(195, 255, 255, 255), 0.75f * s);
    g.DrawEllipse(&innerHighlight, hubCx - hubR + 0.65f * s, hubCy - hubR + 0.65f * s,
                  (hubR - 0.65f * s) * 2.0f, (hubR - 0.65f * s) * 2.0f);

    Pen outerRim(Color(180, 172, 176, 180), 0.75f * s);
    g.DrawEllipse(&outerRim, hubCx - hubR, hubCy - hubR, hubR * 2.0f, hubR * 2.0f);

    if (clipBottomY < 500.0f) {
        float clipY = bodyOriginY + clipBottomY * s;
        float dy = clipY - hubCy;
        if (std::fabs(dy) < hubR) {
            float halfChord = std::sqrt(hubR * hubR - dy * dy);
            Pen flatEdge(Color(185, 168, 172, 176), 0.75f * s);
            g.DrawLine(&flatEdge, hubCx - halfChord, clipY - 0.35f * s, hubCx + halfChord, clipY - 0.35f * s);
        }
    }

    g.Restore(st);
}

void BuildCpuRamBridgePath(GraphicsPath& path, float cpuCx, float cpuCy, float cpuOuterR,
                           float ramCx, float ramCy, float ramOuterR) {
    // Authentic tapered sidecar sleeve wrapping around RAM bezel and tucking BEHIND the foreground CPU bezel!
    // Exact upper tangent normal (-122 deg, slope -32 deg) & lower tangent normal (+80 deg, slope -10 deg)
    // measured directly from back.png and back_lrg.png.
    (void)cpuOuterR;
    const float dx = ramCx - cpuCx;
    const float dy = ramCy - cpuCy;
    const float dist = std::sqrt(dx * dx + dy * dy);

    const float upperDeg = -122.0f;
    const float lowerDeg = 80.0f;
    const float upperRad = upperDeg * kPi / 180.0f;
    const float lowerRad = lowerDeg * kPi / 180.0f;

    PointF topRam(ramCx + std::cos(upperRad) * ramOuterR,
                  ramCy + std::sin(upperRad) * ramOuterR);
    PointF botRam(ramCx + std::cos(lowerRad) * ramOuterR,
                  ramCy + std::sin(lowerRad) * ramOuterR);

    // Tangent vectors pointing down-left behind the foreground CPU circle
    const float upTanX =  std::sin(upperRad); // -0.8480
    const float upTanY = -std::cos(upperRad); // +0.5299
    PointF topHidden(topRam.X + upTanX * (dist * 0.85f),
                     topRam.Y + upTanY * (dist * 0.85f));

    const float loTanX = -std::sin(lowerRad); // -0.9848
    const float loTanY =  std::cos(lowerRad); // +0.1736
    PointF botHidden(botRam.X + loTanX * (dist * 0.82f),
                     botRam.Y + loTanY * (dist * 0.82f));

    const float sweepDeg = lowerDeg - upperDeg; // 202.0 deg around top, right, and bottom of RAM

    path.StartFigure();
    path.AddLine(topHidden, topRam);
    path.AddArc(ramCx - ramOuterR, ramCy - ramOuterR, ramOuterR * 2.0f, ramOuterR * 2.0f,
                upperDeg, sweepDeg);
    path.AddLine(botRam, botHidden);
    path.CloseFigure();
}

void DrawCpuMeterGadget(Graphics& g) {
    if (!g_showCpuMeter) return;

    const float s = GetWidgetScale(WidgetCpuMeter);
    const RectF body = GetWidgetLocalBodyRect(WidgetCpuMeter);
    const CpuMeterLayout L = GetCpuMeterLayout(g_cpuMeterExpanded);

    const float cpuCx = body.X + L.cpuCx * s;
    const float cpuCy = body.Y + L.cpuCy * s;
    const float cpuOuterR = L.cpuOuterR * s;
    const float cpuInnerR = L.cpuInnerR * s;

    const float ramCx = body.X + L.ramCx * s;
    const float ramCy = body.Y + L.ramCy * s;
    const float ramOuterR = L.ramOuterR * s;
    const float ramInnerR = L.ramInnerR * s;

    // 1. Soft multi-layer desktop drop shadow behind both circles & sidecar bridge
    for (int i = 4; i >= 1; --i) {
        float ex = i * 1.10f * s;
        float dy = i * 1.00f * s;
        SolidBrush shBrush(Color(static_cast<BYTE>(14 * (5 - i)), 0, 0, 0));

        GraphicsPath shPath;
        BuildCpuRamBridgePath(shPath, cpuCx, cpuCy + dy, cpuOuterR + ex,
                              ramCx, ramCy + dy, ramOuterR + ex);
        g.FillPath(&shBrush, &shPath);
        g.FillEllipse(&shBrush, cpuCx - cpuOuterR - ex, cpuCy - cpuOuterR - ex + dy,
                      (cpuOuterR + ex) * 2.0f, (cpuOuterR + ex) * 2.0f);
    }

    // 2. Background Housing: Tapered Silver Sidecar Sleeve + Right RAM Studio-Chrome Bezel
    GraphicsPath bridgeAndRam;
    BuildCpuRamBridgePath(bridgeAndRam, cpuCx, cpuCy, cpuOuterR, ramCx, ramCy, ramOuterR);
    {
        LinearGradientBrush ramChrome(
            PointF(ramCx, ramCy - ramOuterR), PointF(ramCx, ramCy + ramOuterR * 1.06f),
            Color(255, 88, 96, 99), Color(255, 95, 102, 105));
        ramChrome.SetWrapMode(WrapModeTileFlipXY);
        ApplyWin7StudioChromeStops(ramChrome);
        g.FillPath(&ramChrome, &bridgeAndRam);

        // Bright diagonal specular highlight on the upper-left sidecar shoulder (matching back_lrg.png)
        {
            GraphicsState brSt = g.Save();
            g.SetClip(&bridgeAndRam);
            float shCx = cpuCx + (ramCx - cpuCx) * 0.48f;
            float shCy = ramCy - ramOuterR * 0.36f;
            GraphicsPath sheenPath;
            sheenPath.AddEllipse(shCx - ramOuterR * 0.65f, shCy - ramOuterR * 0.45f,
                                 ramOuterR * 1.30f, ramOuterR * 0.90f);
            PathGradientBrush sheenPgb(&sheenPath);
            sheenPgb.SetCenterColor(Color(175, 255, 255, 255));
            Color surCol[1] = {Color(0, 255, 255, 255)};
            INT surCnt = 1;
            sheenPgb.SetSurroundColors(surCol, &surCnt);
            g.FillPath(&sheenPgb, &sheenPath);
            g.Restore(brSt);
        }

        Pen outerGunmetal(Color(230, 44, 48, 52), 1.0f * s);
        g.DrawPath(&outerGunmetal, &bridgeAndRam);

        GraphicsPath innerBridge;
        BuildCpuRamBridgePath(innerBridge, cpuCx, cpuCy, cpuOuterR - 0.95f * s,
                              ramCx, ramCy, ramOuterR - 0.95f * s);
        Pen innerSpec(Color(105, 255, 255, 255), 0.80f * s);
        g.DrawPath(&innerSpec, &innerBridge);
    }

    // 3. RAM Dial Interior, Needle, and Center Hub Cap (drawn BEFORE foreground CPU bezel!)
    {
        RectF ramLcd(body.X + L.ramLcd.X * s, body.Y + L.ramLcd.Y * s,
                     L.ramLcd.Width * s, L.ramLcd.Height * s);
        int ramPctInt = static_cast<int>(std::round(g_targetRamPct));

        DrawCpuMeterDialInterior(g, ramCx, ramCy, ramInnerR, s,
                                 false, g_cpuMeterExpanded,
                                 true, L.ramStopPinR,
                                 ramLcd, L.ramLcdRadius, L.ramFontPx, ramPctInt);

        DrawCpuMeterNeedle(g, ramCx, ramCy, ramInnerR,
                           body.X + L.ramNeedlePivotCx * s, body.Y + L.ramNeedlePivotCy * s,
                           g_dispRamPct, s,
                           L.ramNeedleTipY, L.ramNeedleShoulderY, L.ramNeedleBladeHalfW,
                           L.ramNeedleStepOutY, L.ramNeedleStepInY, L.ramNeedleBaseY, L.ramNeedleBaseHalfW);

        DrawCpuMeterHubCap(g, body.X + L.ramHubCx * s, body.Y + L.ramHubCy * s, L.ramHubR * s, s,
                           false, g_cpuMeterExpanded, L.ramHubClipBottomY, body.Y);
    }

    // 4. 3D Contact Shadow cast by foreground CPU bezel exclusively onto the RAM sidecar housing
    {
        GraphicsState ovSt = g.Save();
        g.SetClip(&bridgeAndRam);
        for (int i = 4; i >= 1; --i) {
            float ex = i * 1.35f * s;
            SolidBrush overlapSh(Color(static_cast<BYTE>(22 * (5 - i)), 0, 0, 0));
            g.FillEllipse(&overlapSh,
                          cpuCx - cpuOuterR - ex * 0.2f + i * 0.95f * s,
                          cpuCy - cpuOuterR - ex * 0.3f + i * 0.45f * s,
                          (cpuOuterR + ex * 0.4f) * 2.0f,
                          (cpuOuterR + ex * 0.4f) * 2.0f);
        }
        g.Restore(ovSt);
    }

    // 5. Foreground Unbroken 360-Degree CPU Studio-Chrome Bezel
    {
        LinearGradientBrush cpuChrome(
            PointF(cpuCx, cpuCy - cpuOuterR), PointF(cpuCx, cpuCy + cpuOuterR),
            Color(255, 88, 96, 99), Color(255, 95, 102, 105));
        ApplyWin7StudioChromeStops(cpuChrome);
        g.FillEllipse(&cpuChrome, cpuCx - cpuOuterR, cpuCy - cpuOuterR,
                      cpuOuterR * 2.0f, cpuOuterR * 2.0f);

        Pen outerGunmetal(Color(235, 42, 46, 50), 1.05f * s);
        g.DrawEllipse(&outerGunmetal, cpuCx - cpuOuterR, cpuCy - cpuOuterR,
                      cpuOuterR * 2.0f, cpuOuterR * 2.0f);

        Pen innerSpec(Color(115, 255, 255, 255), 0.85f * s);
        g.DrawEllipse(&innerSpec, cpuCx - cpuOuterR + 0.95f * s, cpuCy - cpuOuterR + 0.95f * s,
                      (cpuOuterR - 0.95f * s) * 2.0f, (cpuOuterR - 0.95f * s) * 2.0f);
    }

    // 6. CPU Dial Interior, Needle, and Center Hub Cap
    {
        RectF cpuLcd(body.X + L.cpuLcd.X * s, body.Y + L.cpuLcd.Y * s,
                     L.cpuLcd.Width * s, L.cpuLcd.Height * s);
        int cpuPctInt = static_cast<int>(std::round(g_targetCpuPct));

        DrawCpuMeterDialInterior(g, cpuCx, cpuCy, cpuInnerR, s,
                                 true, g_cpuMeterExpanded,
                                 L.cpuHasStopPin, L.cpuStopPinR,
                                 cpuLcd, L.cpuLcdRadius, L.cpuFontPx, cpuPctInt);

        DrawCpuMeterNeedle(g, cpuCx, cpuCy, cpuInnerR,
                           cpuCx, cpuCy,
                           g_dispCpuPct, s,
                           L.cpuNeedleTipY, L.cpuNeedleShoulderY, L.cpuNeedleBladeHalfW,
                           L.cpuNeedleStepOutY, L.cpuNeedleStepInY, L.cpuNeedleBaseY, L.cpuNeedleBaseHalfW);

        DrawCpuMeterHubCap(g, body.X + L.cpuHubCx * s, body.Y + L.cpuHubCy * s, L.cpuHubR * s, s,
                           true, g_cpuMeterExpanded, 999.0f, body.Y);
    }

    DrawWin7HoverToolbar(g, WidgetCpuMeter);
}

// ── Hit-Testing & Hover Mechanics (Per-Widget Local Coordinates) ─────────────

int HitTestWidgetLocal(int widgetId, PointF localPt, int* outButtonId, int* outDayNum) {
    if (outButtonId) *outButtonId = HoverBtnNone;
    if (outDayNum) *outDayNum = 0;

    if (!IsWidgetVisibleFlag(widgetId)) return -1;

    const float s = GetWidgetScale(widgetId);
    RectF body = GetWidgetLocalBodyRect(widgetId);

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

    // Bridge the narrow gap between the right side of the widget body and the hover toolbar
    // so moving the cursor from a round clock or RAM dial to the toolbar never drops hover state.
    RectF bridgeRc(body.X + body.Width * 0.5f, bar.Y, (bar.X - (body.X + body.Width * 0.5f)) + 2.0f * s, bar.Height);
    if (g_hoveredWidget == widgetId && PtInRectF(bridgeRc, localPt)) {
        return widgetId;
    }

    if (!PtInRectF(body, localPt)) return -1;

    if (widgetId == WidgetClock) {
        if (g_clockSkin == 5 || g_clockSkin == 6) {
            // Novelty monster and Square clock fill the rectangular body area
            return WidgetClock;
        }
        float cx = body.X + body.Width * 0.5f;
        float cy = body.Y + body.Height * 0.5f;
        float dx = localPt.X - cx;
        float dy = localPt.Y - cy;
        float r = body.Width * 0.49f;
        if (dx * dx + dy * dy <= r * r) {
            return WidgetClock;
        }
        return -1;
    }

    if (widgetId == WidgetCalendar) {
        if (g_calendarExpanded) {
            RectF topSheetRc(std::round(body.X + 3.0f * s), std::round(body.Y + 4.0f * s),
                             std::round(123.0f * s), std::round(134.0f * s));
            RectF prevRc(topSheetRc.X + 3.0f * s, topSheetRc.Y + 4.0f * s, 18.0f * s, 18.0f * s);
            RectF nextRc(topSheetRc.X + topSheetRc.Width - 21.0f * s, topSheetRc.Y + 4.0f * s, 18.0f * s, 18.0f * s);
            if (PtInRectF(prevRc, localPt)) {
                if (outButtonId) *outButtonId = CalBtnPrevMonth;
                return WidgetCalendar;
            }
            if (PtInRectF(nextRc, localPt)) {
                if (outButtonId) *outButtonId = CalBtnNextMonth;
                return WidgetCalendar;
            }

            // Check day cells in the monthly grid (matching DrawTopWhiteMonthSheet coordinates)
            float gridLeft = std::round(topSheetRc.X + 5.0f * s);
            float gridRight = std::round(topSheetRc.X + topSheetRc.Width - 5.0f * s);
            float gridWidth = gridRight - gridLeft;
            float colW = gridWidth / 7.0f;
            float startY = std::round(topSheetRc.Y + 37.0f * s);
            float rowH = std::max(11.0f, std::round(14.5f * s));
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

    if (widgetId == WidgetCpuMeter) {
        const CpuMeterLayout L = GetCpuMeterLayout(g_cpuMeterExpanded);
        float dxCpu = localPt.X - (body.X + L.cpuCx * s);
        float dyCpu = localPt.Y - (body.Y + L.cpuCy * s);
        float rCpu  = (L.cpuOuterR + 1.0f) * s;
        if (dxCpu * dxCpu + dyCpu * dyCpu <= rCpu * rCpu) {
            return WidgetCpuMeter;
        }
        float dxRam = localPt.X - (body.X + L.ramCx * s);
        float dyRam = localPt.Y - (body.Y + L.ramCy * s);
        float rRam  = (L.ramOuterR + 1.0f) * s;
        if (dxRam * dxRam + dyRam * dyRam <= rRam * rRam) {
            return WidgetCpuMeter;
        }
        // Exact trapezoidal hit-test for the tapered silver sidecar bridge between CPU and RAM
        const float cpuCenterX = body.X + L.cpuCx * s;
        const float ramCenterX = body.X + L.ramCx * s;
        if (localPt.X >= cpuCenterX && localPt.X <= ramCenterX + L.ramOuterR * 0.25f * s) {
            const float upperRad = -122.0f * kPi / 180.0f;
            const float lowerRad =   80.0f * kPi / 180.0f;
            const float topRamX = ramCenterX + std::cos(upperRad) * (L.ramOuterR * s);
            const float topRamY = (body.Y + L.ramCy * s) + std::sin(upperRad) * (L.ramOuterR * s);
            const float botRamX = ramCenterX + std::cos(lowerRad) * (L.ramOuterR * s);
            const float botRamY = (body.Y + L.ramCy * s) + std::sin(lowerRad) * (L.ramOuterR * s);

            const float upTanX =  std::sin(upperRad);
            const float upTanY = -std::cos(upperRad);
            const float loTanX = -std::sin(lowerRad);
            const float loTanY =  std::cos(lowerRad);

            const float topYAtX = topRamY + (localPt.X - topRamX) * (upTanY / upTanX);
            const float botYAtX = botRamY + (localPt.X - botRamX) * (loTanY / loTanX);
            if (localPt.Y >= topYAtX - 1.0f * s && localPt.Y <= botYAtX + 1.0f * s) {
                return WidgetCpuMeter;
            }
        }
        return -1;
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
    HWND hwnd = GetWidgetHwnd(widgetId);
    if (!hwnd || !IsWindow(hwnd)) return;

    if (!IsWidgetVisibleFlag(widgetId)) {
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

        // Only fill the hover toolbar & bridge area with alpha=1 WHILE the widget is hovered,
        // so unhovered widgets have pure alpha=0 outside their body for 100% OS click-through.
        if (g_hoveredWidget == widgetId) {
            RectF body = GetWidgetLocalBodyRect(widgetId);
            RectF bar = GetWidgetLocalHoverBarRect(widgetId);
            RectF hitZone(body.X + body.Width * 0.5f, bar.Y,
                          (bar.X + bar.Width) - (body.X + body.Width * 0.5f), bar.Height);
            SolidBrush hitBrush(Color(1, 0, 0, 0));
            g.FillRectangle(&hitBrush, hitZone);
        }

        if (widgetId == WidgetClock) {
            DrawClockGadget(g);
        } else if (widgetId == WidgetCalendar) {
            DrawCalendarGadget(g);
        } else if (widgetId == WidgetCpuMeter) {
            DrawCpuMeterGadget(g);
        }
    }

    PointF wPos = GetWidgetPosRef(widgetId);
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
    RenderWidget(WidgetCpuMeter);
}

void ToggleCpuMeterExpanded() {
    g_cpuMeterExpanded = !g_cpuMeterExpanded;
    AdjustCpuPosForSizeToggle(g_cpuMeterExpanded);
    UpdateEffectiveScale();
    ClampWidgetToVirtualScreen(WidgetCpuMeter);
    SaveStateToStorage();
    RenderWidget(WidgetCpuMeter);
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
    HWND hBtnOk = nullptr;
    HWND hBtnCancel = nullptr;
    HFONT hFont = nullptr;
    HBRUSH hBgBrush = nullptr;
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
    POINT clockPt = {static_cast<LONG>(g_clockPos.X + 71.0f), static_cast<LONG>(g_clockPos.Y + 71.0f)};
    return GetMonitorDpiScaleForPoint(clockPt, g_clockHwnd.load());
}

RectF GetOptionsPreviewBoxRect() {
    const float ds = GetOptionsDpiScale(g_optionsHwnd.load());
    return RectF(18.0f * ds, 44.0f * ds, 244.0f * ds, 168.0f * ds);
}

RectF GetOptionsPrevArrowRect() {
    const float ds = GetOptionsDpiScale(g_optionsHwnd.load());
    return RectF(94.0f * ds, 180.0f * ds, 22.0f * ds, 22.0f * ds);
}

RectF GetOptionsNextArrowRect() {
    const float ds = GetOptionsDpiScale(g_optionsHwnd.load());
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
        Font titleFont(&segoe, SnapFontSize(14.0f * ds), FontStyleRegular, UnitPixel);
        SolidBrush titleBrush(Color(255, 255, 255, 255));
        StringFormat sfNear;
        sfNear.SetAlignment(StringAlignmentNear);
        sfNear.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(g_isRussian ? L"Часы" : L"Clock", -1, &titleFont,
                     SnapTextRect(14.0f * ds, 0.0f, 200.0f * ds, 34.0f * ds), &sfNear, &titleBrush);

        // Preview Box with white background and subtle gray frame
        RectF previewRc = GetOptionsPreviewBoxRect();
        SolidBrush whiteBrush(Color(255, 255, 255, 255));
        g.FillRectangle(&whiteBrush, previewRc);
        Pen framePen(Color(255, 210, 214, 220), 1.0f);
        g.DrawRectangle(&framePen, previewRc.X, previewRc.Y, previewRc.Width, previewRc.Height);
        g.DrawLine(&framePen, previewRc.X, std::round(172.0f * ds), previewRc.X + previewRc.Width, std::round(172.0f * ds));

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
        Font labelFont(&segoe, SnapFontSize(12.0f * ds), FontStyleRegular, UnitPixel);
        SolidBrush darkText(Color(255, 30, 34, 40));

        wchar_t pageStr[32];
        if (g_isRussian) {
            swprintf_s(pageStr, L"%d из 8", g_optState.tempSkin + 1);
        } else {
            swprintf_s(pageStr, L"%d of 8", g_optState.tempSkin + 1);
        }
        g.DrawString(pageStr, -1, &labelFont, SnapTextRect(116.0f * ds, 178.0f * ds, 48.0f * ds, 24.0f * ds), &sfCenter, &darkText);

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
                     SnapTextRect(18.0f * ds, 220.0f * ds, 240.0f * ds, 18.0f * ds), &sfNear, &darkText);

        g.DrawString(g_isRussian ? L"Часовой пояс:" : L"Time zone:", -1, &labelFont,
                     SnapTextRect(18.0f * ds, 268.0f * ds, 240.0f * ds, 18.0f * ds), &sfNear, &darkText);
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
            if (!g_optState.hBgBrush) {
                g_optState.hBgBrush = CreateSolidBrush(RGB(245, 246, 248));
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

            g_optState.hBtnOk = CreateWindowExW(
                0, L"BUTTON", L"OK",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                S(102), S(356), S(76), S(25), hwnd, reinterpret_cast<HMENU>(IDOK), nullptr, nullptr);
            SendMessageW(g_optState.hBtnOk, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

            g_optState.hBtnCancel = CreateWindowExW(
                0, L"BUTTON", g_isRussian ? L"Отмена" : L"Cancel",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                S(186), S(356), S(76), S(25), hwnd, reinterpret_cast<HMENU>(IDCANCEL), nullptr, nullptr);
            SendMessageW(g_optState.hBtnCancel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

            SetTimer(hwnd, 1, 500, nullptr);
            return 0;
        }

        case WM_DPICHANGED: {
            UINT newDpi = HIWORD(wParam);
            float newDs = (newDpi > 0) ? (newDpi / 96.0f) : GetOptionsDpiScale(hwnd);
            g_optState.dpiScale = newDs;
            auto NS = [newDs](int v) -> int { return static_cast<int>(std::round(v * newDs)); };

            const RECT* suggested = reinterpret_cast<const RECT*>(lParam);
            if (suggested) {
                SetWindowPos(hwnd, nullptr,
                             suggested->left, suggested->top,
                             suggested->right - suggested->left,
                             suggested->bottom - suggested->top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }

            HFONT newFont = CreateFontW(-NS(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            if (newFont) {
                HWND ctrls[] = {
                    g_optState.hEditName, g_optState.hComboTz,
                    g_optState.hCheckSec, g_optState.hBtnOk, g_optState.hBtnCancel
                };
                for (HWND c : ctrls) {
                    if (c) SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(newFont), TRUE);
                }
                if (g_optState.hFont) DeleteObject(g_optState.hFont);
                g_optState.hFont = newFont;
            }

            if (g_optState.hEditName)
                SetWindowPos(g_optState.hEditName, nullptr, NS(18), NS(240), NS(244), NS(22), SWP_NOZORDER | SWP_NOACTIVATE);
            if (g_optState.hComboTz)
                SetWindowPos(g_optState.hComboTz, nullptr, NS(18), NS(288), NS(244), NS(240), SWP_NOZORDER | SWP_NOACTIVATE);
            if (g_optState.hCheckSec)
                SetWindowPos(g_optState.hCheckSec, nullptr, NS(18), NS(320), NS(244), NS(20), SWP_NOZORDER | SWP_NOACTIVATE);
            if (g_optState.hBtnOk)
                SetWindowPos(g_optState.hBtnOk, nullptr, NS(102), NS(356), NS(76), NS(25), SWP_NOZORDER | SWP_NOACTIVATE);
            if (g_optState.hBtnCancel)
                SetWindowPos(g_optState.hBtnCancel, nullptr, NS(186), NS(356), NS(76), NS(25), SWP_NOZORDER | SWP_NOACTIVATE);

            InvalidateRect(hwnd, nullptr, FALSE);
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
            SetBkColor(hdcStatic, RGB(245, 246, 248));
            return reinterpret_cast<INT_PTR>(
                g_optState.hBgBrush ? g_optState.hBgBrush : GetSysColorBrush(COLOR_BTNFACE));
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
            if (g_optState.hBgBrush) {
                DeleteObject(g_optState.hBgBrush);
                g_optState.hBgBrush = nullptr;
            }
            g_optionsHwnd.store(nullptr);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void OpenClockOptionsDialog() {
    HWND existing = g_optionsHwnd.load();
    if (existing && IsWindow(existing)) {
        SetForegroundWindow(existing);
        return;
    }

    RectF clockRc = GetWidgetScreenBodyRect(WidgetClock);
    POINT clockCenter = {
        static_cast<LONG>(clockRc.X + clockRc.Width * 0.5f),
        static_cast<LONG>(clockRc.Y + clockRc.Height * 0.5f)
    };
    const float ds = GetMonitorDpiScaleForPoint(clockCenter, g_clockHwnd.load());
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

    HWND hDlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        kOptionsClassName,
        g_isRussian ? L"Часы — Параметры" : L"Clock Options",
        WS_POPUP | WS_VISIBLE | WS_SYSMENU,
        x, y, dlgW, dlgH,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    g_optionsHwnd.store(hDlg);

    if (hDlg) {
        ShowWindow(hDlg, SW_SHOWNORMAL);
        SetForegroundWindow(hDlg);
    }
}

// ── Context Menu (Right-click on Clock, Calendar, or CPU Meter) ──────────────

void ShowGadgetContextMenu(HWND hwnd, int widgetId, POINT screenPt) {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    enum MenuCmd {
        CmdOptions = 1,
        CmdSecondHand,
        CmdToggleCalSize,
        CmdToggleCpuSize,
        CmdJumpToday,
        CmdShowClock,
        CmdShowCalendar,
        CmdShowCpuMeter,
        CmdCloseGadget,
        CmdSkinBase = 100
    };

    static const wchar_t* kSkinNamesRu[8] = {
        L"1 - Классический",
        L"2 - Современный",
        L"3 - Хронометр",
        L"4 - Неоновый",
        L"5 - Ромашка",
        L"6 - Монстрик",
        L"7 - Металлический квадрат",
        L"8 - Системный"
    };
    static const wchar_t* kSkinNamesEn[8] = {
        L"1 - Traditional",
        L"2 - Modern",
        L"3 - Chronometer",
        L"4 - Diner",
        L"5 - Daisy",
        L"6 - Novelty",
        L"7 - Square",
        L"8 - System"
    };

    if (widgetId == WidgetClock) {
        AppendMenuW(hMenu, MF_STRING | MF_DEFAULT, CmdOptions,
                    g_isRussian ? L"Параметры..." : L"Options...");

        HMENU hSkinMenu = CreatePopupMenu();
        for (int i = 0; i < 8; ++i) {
            AppendMenuW(hSkinMenu, MF_STRING | (g_clockSkin == i ? MF_CHECKED : 0),
                        CmdSkinBase + i, g_isRussian ? kSkinNamesRu[i] : kSkinNamesEn[i]);
        }
        AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSkinMenu),
                    g_isRussian ? L"Скин часов" : L"Clock Skin");

        AppendMenuW(hMenu, MF_STRING | (g_showSeconds ? MF_CHECKED : 0), CmdSecondHand,
                    g_isRussian ? L"Показывать секундную стрелку" : L"Show second hand");
    } else if (widgetId == WidgetCalendar) {
        AppendMenuW(hMenu, MF_STRING | MF_DEFAULT, CmdToggleCalSize,
                    g_calendarExpanded
                        ? (g_isRussian ? L"Меньший размер" : L"Smaller size")
                        : (g_isRussian ? L"Больший размер" : L"Larger size"));
        AppendMenuW(hMenu, MF_STRING, CmdJumpToday,
                    g_isRussian ? L"Перейти к сегодняшней дате" : L"Go to Today");
    } else if (widgetId == WidgetCpuMeter) {
        AppendMenuW(hMenu, MF_STRING | MF_DEFAULT, CmdToggleCpuSize,
                    g_cpuMeterExpanded
                        ? (g_isRussian ? L"Меньший размер" : L"Smaller size")
                        : (g_isRussian ? L"Больший размер" : L"Larger size"));
    }

    bool needRestoreSep = true;
    auto appendRestoreItem = [&](int cmdId, const wchar_t* labelRu, const wchar_t* labelEn) {
        if (needRestoreSep) {
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            needRestoreSep = false;
        }
        AppendMenuW(hMenu, MF_STRING, cmdId, g_isRussian ? labelRu : labelEn);
    };

    if (widgetId != WidgetClock && !g_showClock) {
        appendRestoreItem(CmdShowClock, L"Показать Часы", L"Show Clock Gadget");
    }
    if (widgetId != WidgetCalendar && !g_showCalendar) {
        appendRestoreItem(CmdShowCalendar, L"Показать Календарь", L"Show Calendar Gadget");
    }
    if (widgetId != WidgetCpuMeter && !g_showCpuMeter) {
        appendRestoreItem(CmdShowCpuMeter, L"Показать Индикатор ЦП", L"Show CPU Meter Gadget");
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
        RenderWidget(WidgetClock);
    } else if (cmd == CmdSecondHand) {
        g_showSeconds = !g_showSeconds;
        SaveStateToStorage();
        RenderWidget(WidgetClock);
    } else if (cmd == CmdToggleCalSize) {
        g_calendarExpanded = !g_calendarExpanded;
        ClampWidgetToVirtualScreen(WidgetCalendar);
        SaveStateToStorage();
        RenderWidget(WidgetCalendar);
    } else if (cmd == CmdToggleCpuSize) {
        ToggleCpuMeterExpanded();
    } else if (cmd == CmdJumpToday) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        g_calViewYear = st.wYear;
        g_calViewMonth = st.wMonth;
        g_calSelectedYear = st.wYear;
        g_calSelectedMonth = st.wMonth;
        g_calSelectedDay = st.wDay;
        RenderWidget(WidgetCalendar);
    } else if (cmd == CmdShowClock) {
        g_showClock = true;
        SaveStateToStorage();
        RenderWidget(WidgetClock);
    } else if (cmd == CmdShowCalendar) {
        g_showCalendar = true;
        SaveStateToStorage();
        RenderWidget(WidgetCalendar);
    } else if (cmd == CmdShowCpuMeter) {
        g_showCpuMeter = true;
        SyncCpuMeterTimers();
        SaveStateToStorage();
        RenderWidget(WidgetCpuMeter);
    } else if (cmd == CmdCloseGadget) {
        SetWidgetVisibleFlag(widgetId, false);
        if (widgetId == WidgetCpuMeter) {
            SyncCpuMeterTimers();
        }
        g_hoveredWidget = -1;
        g_hoveredBtn = HoverBtnNone;
        SaveStateToStorage();
        RenderWidget(widgetId);
    }
}

// ── Per-Widget Window Procedure ──────────────────────────────────────────────

void ScheduleNextClockTick(HWND hwnd) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    UINT delay = 1005U - std::min<UINT>(999U, st.wMilliseconds);
    SetTimer(hwnd, kTickTimer, std::max<UINT>(50U, delay), nullptr);
}

int GetWidgetIdFromHwnd(HWND hwnd) {
    if (hwnd == g_clockHwnd.load()) return WidgetClock;
    if (hwnd == g_calendarHwnd.load()) return WidgetCalendar;
    if (hwnd == g_cpuHwnd.load()) return WidgetCpuMeter;
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
                g_clockHwnd.store(hwnd);
                ScheduleNextClockTick(hwnd);
            } else if (widgetId == WidgetCalendar) {
                g_calendarHwnd.store(hwnd);
            } else if (widgetId == WidgetCpuMeter) {
                g_cpuHwnd.store(hwnd);
                SyncCpuMeterTimers();
            }
            RenderWidget(widgetId);
            return 0;
        }

        case WM_TIMER: {
            if (wParam == kTickTimer) {
                ScheduleNextClockTick(hwnd);
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
            if (wParam == kCpuPollTimer) {
                if (!g_showCpuMeter) {
                    KillTimer(hwnd, kCpuPollTimer);
                    return 0;
                }
                SetTimer(hwnd, kCpuPollTimer, 1000, nullptr);
                SampleCpuAndRamUsage(false);
                RenderWidget(WidgetCpuMeter);
                return 0;
            }
            if (wParam == kCpuAnimTimer) {
                if (!g_showCpuMeter) {
                    KillTimer(hwnd, kCpuAnimTimer);
                    g_cpuAnimRunning = false;
                    return 0;
                }
                float dCpu = g_targetCpuPct - g_dispCpuPct;
                float dRam = g_targetRamPct - g_dispRamPct;
                if (std::fabs(dCpu) <= 0.12f && std::fabs(dRam) <= 0.12f) {
                    g_dispCpuPct = g_targetCpuPct;
                    g_dispRamPct = g_targetRamPct;
                    KillTimer(hwnd, kCpuAnimTimer);
                    g_cpuAnimRunning = false;
                } else {
                    g_dispCpuPct += dCpu * 0.24f;
                    g_dispRamPct += dRam * 0.24f;
                }
                RenderWidget(WidgetCpuMeter);
                return 0;
            }
            return 0;
        }

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED: {
            UpdateEffectiveScale();
            ClampWidgetToVirtualScreen(WidgetClock);
            ClampWidgetToVirtualScreen(WidgetCalendar);
            ClampWidgetToVirtualScreen(WidgetCpuMeter);
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
            bool shouldBeVisible = IsWidgetVisibleFlag(widgetId);
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
                SetWidgetVisibleFlag(widgetId, false);
                if (widgetId == WidgetCpuMeter) {
                    SyncCpuMeterTimers();
                }
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
                } else if (widgetId == WidgetCpuMeter) {
                    ToggleCpuMeterExpanded();
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
            PointF currentPos = GetWidgetPosRef(widgetId);
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
                PointF& posRef = GetWidgetPosRef(widgetId);
                posRef = PointF(pt.X - g_dragOffset.X, pt.Y - g_dragOffset.Y);
                ClampWidgetToVirtualScreen(widgetId, true);

                // Move the window BEFORE querying DPI so GetDpiForWindow reflects the new monitor
                SetWindowPos(hwnd, HWND_BOTTOM,
                             static_cast<int>(std::round(posRef.X)),
                             static_cast<int>(std::round(posRef.Y)),
                             0, 0,
                             SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

                UpdateEffectiveScale();
                float newScale = GetWidgetScale(widgetId);
                if (std::fabs(newScale - oldScale) > 0.001f && oldScale > 0.01f) {
                    float ratio = newScale / oldScale;
                    g_dragOffset.X *= ratio;
                    g_dragOffset.Y *= ratio;
                    posRef = PointF(pt.X - g_dragOffset.X, pt.Y - g_dragOffset.Y);
                    ClampWidgetToVirtualScreen(widgetId, true);
                    RenderWidget(widgetId);
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
                UpdateEffectiveScale();
                ClampWidgetToVirtualScreen(dragged, false);
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
            if (hit == WidgetCpuMeter && btn == HoverBtnNone) {
                ToggleCpuMeterExpanded();
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
            int dragged = g_dragWidget;
            bool wasMoved = g_dragMoved;
            g_dragWidget = -1;
            g_dragMoved = false;
            if (wasMoved && dragged >= 0) {
                UpdateEffectiveScale();
                ClampWidgetToVirtualScreen(dragged, false);
                SaveStateToStorage();
            }
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
                HWND optHwnd = g_optionsHwnd.load();
                if (optHwnd && IsWindow(optHwnd)) {
                    DestroyWindow(optHwnd);
                }
                DestroyRenderSurface(WidgetClock);
                g_clockHwnd.store(nullptr);
                PostQuitMessage(0);
            } else if (widgetId == WidgetCalendar) {
                DestroyRenderSurface(WidgetCalendar);
                g_calendarHwnd.store(nullptr);
            } else if (widgetId == WidgetCpuMeter) {
                KillTimer(hwnd, kCpuPollTimer);
                KillTimer(hwnd, kCpuAnimTimer);
                g_cpuAnimRunning = false;
                DestroyRenderSurface(WidgetCpuMeter);
                g_cpuHwnd.store(nullptr);
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

    WNDCLASSEXW wcCpu = {};
    wcCpu.cbSize = sizeof(wcCpu);
    wcCpu.style = CS_DBLCLKS;
    wcCpu.hInstance = instance;
    wcCpu.lpfnWndProc = WidgetWndProc;
    wcCpu.lpszClassName = kCpuMeterClassName;
    wcCpu.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wcCpu);

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
    HWND hClock = CreateWindowExW(
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
    g_clockHwnd.store(hClock);

    SizeF calSz = GetWidgetWindowSize(WidgetCalendar);
    HWND hCal = CreateWindowExW(
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
    g_calendarHwnd.store(hCal);

    SizeF cpuSz = GetWidgetWindowSize(WidgetCpuMeter);
    HWND hCpu = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kCpuMeterClassName,
        L"Windows 7 CPU Meter Widget",
        WS_POPUP,
        static_cast<int>(std::round(g_cpuPos.X)),
        static_cast<int>(std::round(g_cpuPos.Y)),
        static_cast<int>(cpuSz.Width),
        static_cast<int>(cpuSz.Height),
        nullptr, nullptr, instance,
        reinterpret_cast<LPVOID>(static_cast<INT_PTR>(WidgetCpuMeter)));
    g_cpuHwnd.store(hCpu);

    Render();

    MSG msg;
    while (!g_unloading.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        HWND optHwnd = g_optionsHwnd.load();
        if (optHwnd && IsDialogMessageW(optHwnd, &msg)) {
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (HWND optHwnd = g_optionsHwnd.load(); optHwnd && IsWindow(optHwnd)) DestroyWindow(optHwnd);
    if (HWND cpuHwnd = g_cpuHwnd.load(); cpuHwnd && IsWindow(cpuHwnd)) DestroyWindow(cpuHwnd);
    if (HWND calHwnd = g_calendarHwnd.load(); calHwnd && IsWindow(calHwnd)) DestroyWindow(calHwnd);
    if (HWND clkHwnd = g_clockHwnd.load(); clkHwnd && IsWindow(clkHwnd)) DestroyWindow(clkHwnd);
    UnregisterClassW(kOptionsClassName, instance);
    UnregisterClassW(kCpuMeterClassName, instance);
    UnregisterClassW(kCalendarClassName, instance);
    UnregisterClassW(kClockClassName, instance);
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

} // namespace

// ── Windhawk Tool Mod Entry Points ───────────────────────────────────────────

BOOL WhTool_ModInit() {
    g_unloading.store(false);
    LoadStateAndSettings(false);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModUninit() {
    g_unloading.store(true);
    if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    HWND target = g_clockHwnd.load();
    if (!target) target = g_calendarHwnd.load();
    if (!target) target = g_cpuHwnd.load();
    if (target) {
        PostMessageW(target, kSettingsChangedMessage, 0, 0);
    }
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
