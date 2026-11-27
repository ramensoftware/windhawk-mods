// ==WindhawkMod==
// @id              prayer-time-taskbar
// @name            Prayer Time Taskbar
// @description     Shows the current prayer period and the time left next to the Windows 11 taskbar clock (offline ISNA calculation)
// @version         0.1.0
// @author          VienJunaid
// @github          https://github.com/VienJunaid
// @homepage        https://github.com/VienJunaid/prayer-time-taskbar
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Development takes place here:
// https://github.com/VienJunaid/prayer-time-taskbar

// ==WindhawkModReadme==
/*
# Prayer Time Taskbar

Adds a small prayer indicator in front of the Windows 11 taskbar clock:

`🕌 Asr · 1:23 left   11:42 PM`

* The current prayer period you're in, and how long until that prayer's time
  ends.
* Between sunrise and Dhuhr there is no obligatory prayer, so it counts down
  *to* Dhuhr instead (`🕌 Dhuhr in 3:10`).
* All times are calculated on your PC using the ISNA method (Fajr 15°,
  Isha 15°). No internet, no API, no account.

## Setup

Open the **Settings** tab, enter your latitude and longitude (decimal degrees,
+ = North / East, − = South / West), and choose your Asr method. Use the
per-prayer offsets to match your local mosque's timetable.

## Template tokens

* `{icon}`: the icon setting
* `{prayer}`: the current prayer (or the upcoming one during a gap)
* `{next}`: the upcoming prayer
* `{time}`: time left, as `H:MM`

Windows 11 only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- latitude: "33.7490"
  $name: Latitude
  $description: Decimal degrees, + = North, − = South
- longitude: "-84.3880"
  $name: Longitude
  $description: Decimal degrees, + = East, − = West
- asrMethod: shafii
  $name: Asr method
  $options:
  - shafii: Shafi'i, Maliki, Hanbali (shadow factor 1)
  - hanafi: Hanafi (shadow factor 2)
- ishaEnd: fajr
  $name: Isha ends at
  $options:
  - fajr: Next Fajr
  - midnight: Islamic midnight (halfway between Maghrib and next Fajr)
- icon: "🕌"
  $name: Icon
  $description: Any emoji or character, e.g. ☪ or 🌙
- format: "{icon} {prayer} · {time} left"
  $name: Display template
- formatNoPrayer: "{icon} {next} in {time}"
  $name: Display template when no prayer is due
  $description: Used between sunrise and Dhuhr (and after Islamic midnight)
- offsetFajr: 0
  $name: Fajr offset (minutes)
- offsetDhuhr: 0
  $name: Dhuhr offset (minutes)
- offsetAsr: 0
  $name: Asr offset (minutes)
- offsetMaghrib: 0
  $name: Maghrib offset (minutes)
- offsetIsha: 0
  $name: Isha offset (minutes)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <cwchar>
#include <mutex>
#include <string>
#include <string_view>

#pragma region Settings

struct Settings {
    double latitude;
    double longitude;
    int asrFactor;
    bool ishaEndsAtMidnight;
    std::wstring icon;
    std::wstring format;
    std::wstring formatNoPrayer;
    int offsetFajr;
    int offsetDhuhr;
    int offsetAsr;
    int offsetMaghrib;
    int offsetIsha;
};

Settings g_settings;
std::mutex g_settingsMutex;

std::atomic<bool> g_unloading;

// Windhawk settings only support integer numbers, so coordinates are strings.
double ParseCoordinate(PCWSTR name, double limit, double fallback) {
    WindhawkUtils::StringSetting value = WindhawkUtils::StringSetting::make(name);
    PCWSTR str = value.get();
    PWSTR end = nullptr;
    double result = wcstod(str, &end);
    while (end && *end == L' ') {
        end++;
    }

    if (end == str || (end && *end) || !std::isfinite(result) ||
        std::fabs(result) > limit) {
        Wh_Log(L"Invalid %s: '%s', using %f", name, str, fallback);
        return fallback;
    }

    return result;
}

void LoadSettings() {
    Settings settings;

    settings.latitude = ParseCoordinate(L"latitude", 90.0, 33.7490);
    settings.longitude = ParseCoordinate(L"longitude", 180.0, -84.3880);

    {
        WindhawkUtils::StringSetting asrMethod =
            WindhawkUtils::StringSetting::make(L"asrMethod");
        settings.asrFactor = wcscmp(asrMethod.get(), L"hanafi") == 0 ? 2 : 1;
    }

    {
        WindhawkUtils::StringSetting ishaEnd =
            WindhawkUtils::StringSetting::make(L"ishaEnd");
        settings.ishaEndsAtMidnight = wcscmp(ishaEnd.get(), L"midnight") == 0;
    }

    settings.icon = WindhawkUtils::StringSetting::make(L"icon").get();
    settings.format = WindhawkUtils::StringSetting::make(L"format").get();
    settings.formatNoPrayer =
        WindhawkUtils::StringSetting::make(L"formatNoPrayer").get();

    settings.offsetFajr = Wh_GetIntSetting(L"offsetFajr");
    settings.offsetDhuhr = Wh_GetIntSetting(L"offsetDhuhr");
    settings.offsetAsr = Wh_GetIntSetting(L"offsetAsr");
    settings.offsetMaghrib = Wh_GetIntSetting(L"offsetMaghrib");
    settings.offsetIsha = Wh_GetIntSetting(L"offsetIsha");

    std::lock_guard<std::mutex> guard(g_settingsMutex);
    g_settings = std::move(settings);
}

#pragma endregion  // Settings

#pragma region PrayerCalc

// Pure math, no I/O. Formulas follow the PrayTimes.org method (ISNA
// parameters), see README.md. All angles are in degrees.

constexpr double kPi = 3.14159265358979323846;

constexpr double kFajrAngle = 15.0;
constexpr double kIshaAngle = 15.0;
constexpr double kRiseSetAngle = 0.833;

constexpr LONGLONG kTicksPerMinute = 60LL * 10000000LL;
constexpr LONGLONG kTicksPerDay = 24LL * 60LL * kTicksPerMinute;

double DSin(double d) {
    return sin(d * kPi / 180.0);
}

double DCos(double d) {
    return cos(d * kPi / 180.0);
}

double DTan(double d) {
    return tan(d * kPi / 180.0);
}

double DArcSin(double x) {
    return asin(x) * 180.0 / kPi;
}

double DArcCos(double x) {
    return acos(x) * 180.0 / kPi;
}

double DArcTan2(double y, double x) {
    return atan2(y, x) * 180.0 / kPi;
}

double DArcCot(double x) {
    return atan(1.0 / x) * 180.0 / kPi;
}

double FixRange(double a, double range) {
    a -= range * floor(a / range);
    return a < 0 ? a + range : a;
}

struct Date {
    int year;
    int month;
    int day;
};

double JulianDay(const Date& date) {
    int year = date.year;
    int month = date.month;
    if (month <= 2) {
        year -= 1;
        month += 12;
    }

    double a = floor(year / 100.0);
    double b = 2 - a + floor(a / 4.0);
    return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) +
           date.day + b - 1524.5;
}

struct SunPosition {
    double declination;
    double equationOfTime;  // Hours.
};

SunPosition ComputeSunPosition(double jd) {
    double d = jd - 2451545.0;
    double g = FixRange(357.529 + 0.98560028 * d, 360);
    double q = FixRange(280.459 + 0.98564736 * d, 360);
    double l = FixRange(q + 1.915 * DSin(g) + 0.020 * DSin(2 * g), 360);
    double e = 23.439 - 0.00000036 * d;

    double ra = FixRange(DArcTan2(DCos(e) * DSin(l), DCos(l)) / 15, 24);
    double eqt = q / 15 - ra;
    // Normalize to ±12h, q and RA can wrap around 0h on different days.
    eqt = FixRange(eqt + 12, 24) - 12;

    return {DArcSin(DSin(e) * DSin(l)), eqt};
}

// Absolute UTC times, as FILETIME ticks, rounded to the minute.
struct DayTimes {
    bool valid;
    LONGLONG fajr;
    LONGLONG sunrise;
    LONGLONG dhuhr;
    LONGLONG asr;
    LONGLONG maghrib;
    LONGLONG isha;
};

LONGLONG UtcMidnightTicks(const Date& date) {
    SYSTEMTIME st{};
    st.wYear = (WORD)date.year;
    st.wMonth = (WORD)date.month;
    st.wDay = (WORD)date.day;

    FILETIME ft;
    if (!SystemTimeToFileTime(&st, &ft)) {
        return 0;
    }

    return (LONGLONG)(((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime);
}

Date AddDays(const Date& date, int days) {
    LONGLONG ticks = UtcMidnightTicks(date) + days * kTicksPerDay;

    FILETIME ft;
    ft.dwLowDateTime = (DWORD)ticks;
    ft.dwHighDateTime = (DWORD)(ticks >> 32);

    SYSTEMTIME st;
    if (!FileTimeToSystemTime(&ft, &st)) {
        return date;
    }

    return {st.wYear, st.wMonth, st.wDay};
}

DayTimes ComputeDayTimes(const Date& date, const Settings& settings) {
    const double lat = settings.latitude;
    const double lng = settings.longitude;
    const double jDate = JulianDay(date) - lng / 360.0;

    // `t` is the approximate time of the event in hours, used to evaluate the
    // sun's position close to the event.
    auto midDay = [&](double t) {
        return 12 - ComputeSunPosition(jDate + t / 24).equationOfTime;
    };

    auto sunAngleTime = [&](double angle, double t, bool beforeNoon) {
        double decl = ComputeSunPosition(jDate + t / 24).declination;
        double hourAngle =
            DArcCos((-DSin(angle) - DSin(lat) * DSin(decl)) /
                    (DCos(lat) * DCos(decl))) /
            15;
        return midDay(t) + (beforeNoon ? -hourAngle : hourAngle);
    };

    auto asrTime = [&](double factor, double t) {
        double decl = ComputeSunPosition(jDate + t / 24).declination;
        double angle = -DArcCot(factor + DTan(std::fabs(lat - decl)));
        return sunAngleTime(angle, t, false);
    };

    // Local solar hours. NaN when the sun never reaches the angle.
    double fajr = sunAngleTime(kFajrAngle, 5, true);
    double sunrise = sunAngleTime(kRiseSetAngle, 6, true);
    double dhuhr = midDay(12);
    double asr = asrTime(settings.asrFactor, 13);
    double sunset = sunAngleTime(kRiseSetAngle, 18, false);
    double isha = sunAngleTime(kIshaAngle, 18, false);

    DayTimes result{};

    if (std::isnan(sunrise) || std::isnan(sunset) || std::isnan(asr)) {
        // Polar day or night, nothing sensible to show.
        return result;
    }

    // High latitudes: angle-based rule, the night portion is angle/60 of the
    // night.
    double night = 24 - (sunset - sunrise);
    if (std::isnan(fajr)) {
        fajr = sunrise - night * kFajrAngle / 60;
    }
    if (std::isnan(isha)) {
        isha = sunset + night * kIshaAngle / 60;
    }

    const LONGLONG midnightTicks = UtcMidnightTicks(date);
    auto toTicks = [&](double localSolarHours, int offsetMinutes) {
        double utcHours = localSolarHours - lng / 15;
        LONGLONG minutes = llround(utcHours * 60) + offsetMinutes;
        return midnightTicks + minutes * kTicksPerMinute;
    };

    result.valid = midnightTicks != 0;
    result.fajr = toTicks(fajr, settings.offsetFajr);
    result.sunrise = toTicks(sunrise, 0);
    result.dhuhr = toTicks(dhuhr, settings.offsetDhuhr);
    result.asr = toTicks(asr, settings.offsetAsr);
    result.maghrib = toTicks(sunset, settings.offsetMaghrib);
    result.isha = toTicks(isha, settings.offsetIsha);
    return result;
}

#pragma endregion  // PrayerCalc

#pragma region PeriodLogic

struct Period {
    LONGLONG start;
    LONGLONG end;
    PCWSTR prayer;  // nullptr for the gaps with no obligatory prayer.
    PCWSTR next;
};

struct CurrentPeriod {
    PCWSTR prayer;
    PCWSTR next;
    LONGLONG minutesLeft;
};

bool FindPeriod(const DayTimes& day,
                const DayTimes& nextDay,
                bool ishaEndsAtMidnight,
                LONGLONG now,
                CurrentPeriod* result) {
    if (!day.valid || !nextDay.valid) {
        return false;
    }

    LONGLONG midnight = day.maghrib + (nextDay.fajr - day.maghrib) / 2;
    // Keep Islamic midnight on a minute boundary, like the other times.
    midnight -= midnight % kTicksPerMinute;

    LONGLONG ishaEnd = ishaEndsAtMidnight ? midnight : nextDay.fajr;

    const Period periods[] = {
        {day.fajr, day.sunrise, L"Fajr", L"Dhuhr"},
        {day.sunrise, day.dhuhr, nullptr, L"Dhuhr"},
        {day.dhuhr, day.asr, L"Dhuhr", L"Asr"},
        {day.asr, day.maghrib, L"Asr", L"Maghrib"},
        {day.maghrib, day.isha, L"Maghrib", L"Isha"},
        {day.isha, ishaEnd, L"Isha", L"Fajr"},
        {ishaEnd, nextDay.fajr, nullptr, L"Fajr"},
    };

    for (const auto& period : periods) {
        if (now >= period.start && now < period.end) {
            result->prayer = period.prayer;
            result->next = period.next;
            result->minutesLeft = (period.end - now) / kTicksPerMinute;
            return true;
        }
    }

    return false;
}

bool GetCurrentPeriod(const Settings& settings, CurrentPeriod* result) {
    FILETIME nowFt;
    GetSystemTimeAsFileTime(&nowFt);
    LONGLONG now = (LONGLONG)(((ULONGLONG)nowFt.dwHighDateTime << 32) |
                              nowFt.dwLowDateTime);
    // The clock refreshes at (roughly) the start of each minute. Round to the
    // nearest minute so a refresh firing a few ms early or late shows the same
    // value.
    now = (now + kTicksPerMinute / 2) / kTicksPerMinute * kTicksPerMinute;

    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    Date today = {localTime.wYear, localTime.wMonth, localTime.wDay};

    // Times are computed per local date. Before today's Fajr we're still in
    // yesterday's Isha (or after its midnight).
    DayTimes yesterday = ComputeDayTimes(AddDays(today, -1), settings);
    DayTimes todayTimes = ComputeDayTimes(today, settings);
    DayTimes tomorrow = ComputeDayTimes(AddDays(today, 1), settings);

    return FindPeriod(yesterday, todayTimes, settings.ishaEndsAtMidnight, now,
                      result) ||
           FindPeriod(todayTimes, tomorrow, settings.ishaEndsAtMidnight, now,
                      result);
}

#pragma endregion  // PeriodLogic

#pragma region Formatter

void ReplaceAll(std::wstring& str, std::wstring_view from, std::wstring_view to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::wstring::npos) {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
}

bool BuildIndicatorText(std::wstring* text) {
    Settings settings;
    {
        std::lock_guard<std::mutex> guard(g_settingsMutex);
        settings = g_settings;
    }

    CurrentPeriod period;
    if (!GetCurrentPeriod(settings, &period)) {
        return false;
    }

    WCHAR time[32];
    swprintf_s(time, ARRAYSIZE(time), L"%d:%02d",
               (int)(period.minutesLeft / 60), (int)(period.minutesLeft % 60));

    *text = period.prayer ? settings.format : settings.formatNoPrayer;
    ReplaceAll(*text, L"{icon}", settings.icon);
    ReplaceAll(*text, L"{prayer}", period.prayer ? period.prayer : period.next);
    ReplaceAll(*text, L"{next}", period.next);
    ReplaceAll(*text, L"{time}", time);
    return true;
}

#pragma endregion  // Formatter

#pragma region ClockHook

// Same approach as the Taskbar Clock Customization mod by m417z: while the
// clock's data model refreshes its text, the time line is produced by
// GetTimeFormatEx, so we prepend our text to its output.

constexpr WCHAR kSeparator[] = L"   ";

thread_local bool g_inRefreshIcon;
thread_local bool g_inGetTimeToolTipString;

using ClockSystemTrayIconDataModel_RefreshIcon_t = void(WINAPI*)(
    LPVOID pThis,
    LPVOID  // SystemTrayTelemetry::ClockUpdate&
);
ClockSystemTrayIconDataModel_RefreshIcon_t
    ClockSystemTrayIconDataModel_RefreshIcon_Original;
ClockSystemTrayIconDataModel_RefreshIcon_t
    ClockSystemTrayIconDataModel2_RefreshIcon_Original;

void RefreshIcon_Hook_Impl(LPVOID pThis,
                           LPVOID param1,
                           ClockSystemTrayIconDataModel_RefreshIcon_t original) {
    g_inRefreshIcon = true;
    original(pThis, param1);
    g_inRefreshIcon = false;
}

void WINAPI ClockSystemTrayIconDataModel_RefreshIcon_Hook(LPVOID pThis,
                                                          LPVOID param1) {
    RefreshIcon_Hook_Impl(pThis, param1,
                          ClockSystemTrayIconDataModel_RefreshIcon_Original);
}

void WINAPI ClockSystemTrayIconDataModel2_RefreshIcon_Hook(LPVOID pThis,
                                                           LPVOID param1) {
    RefreshIcon_Hook_Impl(pThis, param1,
                          ClockSystemTrayIconDataModel2_RefreshIcon_Original);
}

// The tooltip is built inside RefreshIcon too and also calls GetTimeFormatEx,
// these hooks only exist to leave the tooltip alone.
using GetTimeToolTipString_t =
    LPVOID(WINAPI*)(LPVOID pThis, LPVOID, LPVOID, LPVOID, LPVOID);
GetTimeToolTipString_t ClockSystemTrayIconDataModel_GetTimeToolTipString_Original;
GetTimeToolTipString_t
    ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original;
GetTimeToolTipString_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original;
GetTimeToolTipString_t
    ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original;
GetTimeToolTipString_t
    ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original;

LPVOID GetTimeToolTipString_Hook_Impl(LPVOID pThis,
                                      LPVOID param1,
                                      LPVOID param2,
                                      LPVOID param3,
                                      LPVOID param4,
                                      GetTimeToolTipString_t original) {
    g_inGetTimeToolTipString = true;
    LPVOID ret = original(pThis, param1, param2, param3, param4);
    g_inGetTimeToolTipString = false;
    return ret;
}

LPVOID WINAPI
ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook(LPVOID pThis,
                                                       LPVOID param1,
                                                       LPVOID param2,
                                                       LPVOID param3,
                                                       LPVOID param4) {
    return GetTimeToolTipString_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel_GetTimeToolTipString_Original);
}

LPVOID WINAPI
ClockSystemTrayIconDataModel2_GetTimeToolTipString_Hook(LPVOID pThis,
                                                        LPVOID param1,
                                                        LPVOID param2,
                                                        LPVOID param3,
                                                        LPVOID param4) {
    return GetTimeToolTipString_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original);
}

LPVOID WINAPI
ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook(LPVOID pThis,
                                                        LPVOID param1,
                                                        LPVOID param2,
                                                        LPVOID param3,
                                                        LPVOID param4) {
    return GetTimeToolTipString_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original);
}

LPVOID WINAPI
ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Hook(LPVOID pThis,
                                                         LPVOID param1,
                                                         LPVOID param2,
                                                         LPVOID param3,
                                                         LPVOID param4) {
    return GetTimeToolTipString_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original);
}

LPVOID WINAPI
ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Hook(LPVOID pThis,
                                                         LPVOID param1,
                                                         LPVOID param2,
                                                         LPVOID param3,
                                                         LPVOID param4) {
    return GetTimeToolTipString_Hook_Impl(
        pThis, param1, param2, param3, param4,
        ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original);
}

using GetTimeFormatEx_t = decltype(&GetTimeFormatEx);
GetTimeFormatEx_t GetTimeFormatEx_Original;

int WINAPI GetTimeFormatEx_Hook(LPCWSTR lpLocaleName,
                                DWORD dwFlags,
                                CONST SYSTEMTIME* lpTime,
                                LPCWSTR lpFormat,
                                LPWSTR lpTimeStr,
                                int cchTime) {
    std::wstring text;
    if (!g_inRefreshIcon || g_inGetTimeToolTipString || g_unloading ||
        !BuildIndicatorText(&text)) {
        return GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime,
                                        lpFormat, lpTimeStr, cchTime);
    }

    WCHAR time[128];
    int timeLen = GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime,
                                           lpFormat, time, ARRAYSIZE(time));
    if (timeLen <= 0) {
        return GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime,
                                        lpFormat, lpTimeStr, cchTime);
    }

    text += kSeparator;
    text += time;

    int needed = (int)text.size() + 1;
    if (!cchTime) {
        // Leave some slack in case the text grows before the actual call, e.g.
        // 9:59 -> 10:00.
        return needed + 16;
    }

    if (cchTime < needed) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }

    wcscpy_s(lpTimeStr, cchTime, text.c_str());
    return needed;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    HRSRC resource =
        FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (!resource) {
        return nullptr;
    }

    HGLOBAL global = LoadResource(module, resource);
    if (!global) {
        return nullptr;
    }

    void* data = LockResource(global);
    if (!data) {
        return nullptr;
    }

    void* fixedFileInfo = nullptr;
    UINT len = 0;
    if (!VerQueryValue(data, L"\\", &fixedFileInfo, &len) || !len) {
        return nullptr;
    }

    return (VS_FIXEDFILEINFO*)fixedFileInfo;
}

bool IsWindows11() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr);
    if (!fixedFileInfo) {
        return false;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    Wh_Log(L"Explorer version: %u, build %u", major, build);

    return major == 10 && build >= 22000;
}

// The clock's data model lives in SystemTray.dll on recent builds, in
// Taskbar.View.dll before version 2604, and in ExplorerExtensions.dll on older
// builds.
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(module);
            WORD major =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!major || major >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %u", major);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookSystemTraySymbols(HMODULE module) {
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
            true,  // Added with feature flag 38762814.
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
            true,  // Added with feature flag 38762814.
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
            true,  // Added with feature flag 38762814.
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME *,struct _TIME_DYNAMIC_ZONE_INFORMATION *,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original,
            ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Hook,
            true,  // Until Windows 11 version 21H2.
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

std::atomic<bool> g_systemTrayModuleHooked;

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

// Touching the AdditionalClocks key makes the taskbar clock refresh right
// away, so settings changes show up without waiting for the next minute.
void RefreshClock() {
    constexpr WCHAR kTempValueName[] = L"_temp_windhawk_prayer-time-taskbar";

    HKEY key;
    LONG result =
        RegOpenKeyEx(HKEY_CURRENT_USER,
                     L"Control Panel\\TimeDate\\AdditionalClocks", 0,
                     KEY_WRITE, &key);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open AdditionalClocks key: %d", result);
        return;
    }

    if (RegSetValueEx(key, kTempValueName, 0, REG_SZ, (const BYTE*)L"",
                      sizeof(WCHAR)) != ERROR_SUCCESS) {
        Wh_Log(L"Failed to create temp value");
    } else if (RegDeleteValue(key, kTempValueName) != ERROR_SUCCESS) {
        Wh_Log(L"Failed to remove temp value");
    }

    RegCloseKey(key);
}

#pragma endregion  // ClockHook

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!IsWindows11()) {
        Wh_Log(L"Unsupported Windows version, Windows 11 is required");
        return FALSE;
    }

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");

    auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    // Use GetProcAddress, otherwise the kernel32.dll stub is hooked.
    auto pGetTimeFormatEx = (decltype(&GetTimeFormatEx))GetProcAddress(
        kernelBaseModule, "GetTimeFormatEx");
    if (!pGetTimeFormatEx) {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(pGetTimeFormatEx, GetTimeFormatEx_Hook,
                                   &GetTimeFormatEx_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The module might have been loaded between Wh_ModInit and the
    // LoadLibraryExW hook taking effect.
    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    RefreshClock();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;
    RefreshClock();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();
    RefreshClock();

    *bReload = FALSE;
    return TRUE;
}
