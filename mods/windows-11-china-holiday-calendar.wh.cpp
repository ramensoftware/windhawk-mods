// ==WindhawkMod==
// @id              windows-11-china-holiday-calendar
// @name            Windows 11 Native China Holiday Calendar
// @name:zh-CN      Windows 11 原生中国节假日日历
// @description     Automatically marks mainland China statutory holidays and make-up workdays in the native Windows 11 calendar, while preserving lunar/festival labels and selected-date header sync.
// @description:zh-CN 在 Windows 11 原生日历中自动获取并标记中国法定节假日与调休，保留农历/节日显示，并让顶部日期随点击日期同步。
// @version         1.0.1
// @author          Zep
// @github          https://github.com/Zeptol
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under GNU GPL v3.0.

// ==WindhawkModReadme==
/*
# Windows 11 原生中国节假日日历

作者：**Zep**（GitHub: [@zeptol](https://github.com/Zeptol)）

直接增强 Windows 11 任务栏右下角的**系统原生日历**，不替换窗口、不启动独立日历。

## 功能

- 自动获取中国大陆法定节假日与调休安排，无需每年手工修改代码；
- 法定休假日使用淡红色圆形底色，并显示小型 `休` 标记；
- 调休工作日使用淡暖色圆形底色，并显示小型 `班` 标记；
- 保留农历、传统节日和节气文字；
- 点击任意日期后，顶部日期与农历/节日信息会同步到所选日期；
- 日期数字本身仍由 Windows 原生样式绘制。

## 节假日数据

Mod 会按当前正在显示的年份自动联网读取 `NateScarlet/holiday-cn` 的年度 JSON 数据。
该项目根据国务院节假日公告自动更新，并提供 `休息日 / 调休工作日` 标记。

数据源：<https://github.com/NateScarlet/holiday-cn>

为了兼容跨年调休，Mod 会同时检查当前年份和下一年份的数据。数据在运行期间定期刷新；成功获取的数据会写入 Windhawk 本地缓存，重启后可先使用缓存并在后台刷新。网络不可用时，已缓存数据以及内置的 2026 数据仍可作为回退。

联网部分使用 Windhawk 1.5+ 提供的 `Wh_GetUrlContent` API，而不是直接从 `ShellExperienceHost.exe` 发起 WinINet 请求，从而避免打包系统进程的网络访问限制。

## 兼容性说明

本 Mod 使用 Windows XAML Diagnostics 访问系统原生 `CalendarView`。同一个 `ShellExperienceHost.exe` 进程同时只能有一个 XAML Diagnostics consumer，因此不要同时启用其它同样使用 XAML Diagnostics 修改通知中心/日历的 Mod。

XAML Diagnostics / VisualTreeWatcher 引导结构参考了 m417z 的 Windows 11 Notification Center Styler，并按照 GPL-3.0 要求保留相应许可与致谢。
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <xamlom.h>
#include <ocidl.h>
#include <combaseapi.h>

#include <atomic>
#include <cwchar>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// windows.h defines GetCurrentTime() as a macro for GetTickCount().
// C++/WinRT has methods named GetCurrentTime with parameters, so the macro
// must be removed after the Win32 headers and before the WinRT headers.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Markup.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wxc = winrt::Windows::UI::Xaml::Controls;
namespace wui = winrt::Windows::UI;
namespace wg = winrt::Windows::Globalization;
namespace wxmedia = winrt::Windows::UI::Xaml::Media;
namespace wxm = winrt::Windows::UI::Xaml::Markup;

// -----------------------------------------------------------------------------
// Holiday model
// -----------------------------------------------------------------------------

enum class DayKind {
    Normal,
    Holiday,
    MakeUpWorkday,
};

constexpr int DateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

// -----------------------------------------------------------------------------
// Automatic holiday data
// -----------------------------------------------------------------------------
//
// Data source: NateScarlet/holiday-cn
// https://github.com/NateScarlet/holiday-cn
//
// The repository's yearly JSON files are generated from State Council notices.
// Per the repository documentation, December dates can be affected by the next
// year's notice, so classification checks both <year>.json and <year+1>.json.

struct HolidayYearState {
    bool fetching = false;
    bool fetchedSuccessfully = false;
    bool cacheLoadAttempted = false;
    ULONGLONG lastAttemptTick = 0;
};

SRWLOCK g_holidayDataLock = SRWLOCK_INIT;
std::unordered_map<int, std::unordered_map<int, DayKind>> g_holidayDataBySourceYear;
std::unordered_map<int, HolidayYearState> g_holidayYearStates;

SRWLOCK g_fetchThreadsLock = SRWLOCK_INIT;
std::vector<HANDLE> g_fetchThreads;
std::atomic<bool> g_stopping{false};

constexpr ULONGLONG kSuccessfulRefreshMs = 12ULL * 60 * 60 * 1000;
constexpr ULONGLONG kFailedRetryMs = 15ULL * 60 * 1000;

void RefreshAllCalendarsAcrossThreads();

void InitializeBuiltIn2026Fallback() {
    std::unordered_map<int, DayKind> days;

    const int workdays[] = {
        20260104, 20260214, 20260228, 20260509, 20260920, 20261010,
    };
    for (int key : workdays) {
        days[key] = DayKind::MakeUpWorkday;
    }

    const int holidays[] = {
        20260101, 20260102, 20260103,
        20260215, 20260216, 20260217, 20260218, 20260219,
        20260220, 20260221, 20260222, 20260223,
        20260404, 20260405, 20260406,
        20260501, 20260502, 20260503, 20260504, 20260505,
        20260619, 20260620, 20260621,
        20260925, 20260926, 20260927,
        20261001, 20261002, 20261003, 20261004,
        20261005, 20261006, 20261007,
    };
    for (int key : holidays) {
        days[key] = DayKind::Holiday;
    }

    AcquireSRWLockExclusive(&g_holidayDataLock);
    g_holidayDataBySourceYear[2026] = std::move(days);
    // Deliberately don't mark 2026 as successfully fetched: the online source
    // should still refresh/override the fallback as soon as possible.
    ReleaseSRWLockExclusive(&g_holidayDataLock);
}

bool ParseIsoDateKey(std::string const& value, int* key) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') {
        return false;
    }

    auto digit = [](char c) { return c >= '0' && c <= '9'; };
    if (!digit(value[0]) || !digit(value[1]) || !digit(value[2]) ||
        !digit(value[3]) || !digit(value[5]) || !digit(value[6]) ||
        !digit(value[8]) || !digit(value[9])) {
        return false;
    }

    const int year = (value[0] - '0') * 1000 + (value[1] - '0') * 100 +
                     (value[2] - '0') * 10 + (value[3] - '0');
    const int month = (value[5] - '0') * 10 + (value[6] - '0');
    const int day = (value[8] - '0') * 10 + (value[9] - '0');
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }

    *key = DateKey(year, month, day);
    return true;
}

bool ParseHolidayJson(std::string const& json,
                      std::unordered_map<int, DayKind>* result) {
    result->clear();

    const size_t daysPos = json.find("\"days\"");
    if (daysPos == std::string::npos) {
        return false;
    }

    size_t pos = daysPos;
    while (true) {
        const size_t dateKeyPos = json.find("\"date\"", pos);
        if (dateKeyPos == std::string::npos) {
            break;
        }

        const size_t objectEnd = json.find('}', dateKeyPos);
        if (objectEnd == std::string::npos) {
            return false;
        }

        const size_t colon = json.find(':', dateKeyPos);
        const size_t quote1 = colon == std::string::npos
                                  ? std::string::npos
                                  : json.find('"', colon + 1);
        const size_t quote2 = quote1 == std::string::npos
                                  ? std::string::npos
                                  : json.find('"', quote1 + 1);
        if (quote1 == std::string::npos || quote2 == std::string::npos ||
            quote2 > objectEnd) {
            pos = objectEnd + 1;
            continue;
        }

        int date = 0;
        if (!ParseIsoDateKey(json.substr(quote1 + 1, quote2 - quote1 - 1),
                             &date)) {
            pos = objectEnd + 1;
            continue;
        }

        const size_t offKeyPos = json.find("\"isOffDay\"", quote2);
        if (offKeyPos == std::string::npos || offKeyPos > objectEnd) {
            pos = objectEnd + 1;
            continue;
        }

        const size_t offColon = json.find(':', offKeyPos);
        if (offColon == std::string::npos || offColon > objectEnd) {
            pos = objectEnd + 1;
            continue;
        }

        size_t valuePos = offColon + 1;
        while (valuePos < objectEnd &&
               (json[valuePos] == ' ' || json[valuePos] == '\t' ||
                json[valuePos] == '\r' || json[valuePos] == '\n')) {
            valuePos++;
        }

        if (json.compare(valuePos, 4, "true") == 0) {
            (*result)[date] = DayKind::Holiday;
        } else if (json.compare(valuePos, 5, "false") == 0) {
            (*result)[date] = DayKind::MakeUpWorkday;
        }

        pos = objectEnd + 1;
    }

    // An empty `days` array is valid (for example, the next year's file before
    // the State Council publishes its schedule), so presence of the field is
    // enough to consider the JSON valid.
    return true;
}

std::wstring CacheDataKey(int year) {
    return L"holiday-cache-data-" + std::to_wstring(year);
}

std::wstring CacheSizeKey(int year) {
    return L"holiday-cache-size-" + std::to_wstring(year);
}

bool SaveHolidayCache(int year, std::string const& json) {
    if (json.empty() || json.size() > 1024 * 1024) {
        return false;
    }

    const auto dataKey = CacheDataKey(year);
    const auto sizeKey = CacheSizeKey(year);

    if (!Wh_SetBinaryValue(dataKey.c_str(), json.data(), json.size())) {
        return false;
    }

    if (!Wh_SetIntValue(sizeKey.c_str(), static_cast<int>(json.size()))) {
        return false;
    }

    return true;
}

bool LoadHolidayCache(int year,
                      std::unordered_map<int, DayKind>* parsed) {
    const auto dataKey = CacheDataKey(year);
    const auto sizeKey = CacheSizeKey(year);

    const int storedSize = Wh_GetIntValue(sizeKey.c_str(), 0);
    if (storedSize <= 0 || storedSize > 1024 * 1024) {
        return false;
    }

    std::string json(static_cast<size_t>(storedSize), '\0');
    const size_t bytesRead =
        Wh_GetBinaryValue(dataKey.c_str(), json.data(), json.size());
    if (bytesRead != json.size()) {
        return false;
    }

    return ParseHolidayJson(json, parsed);
}

bool WindhawkGet(std::wstring const& url,
                 std::string* body,
                 int* statusCode) {
    body->clear();
    if (statusCode) {
        *statusCode = 0;
    }

    // Wh_GetUrlContent is the Windhawk-native networking API (Windhawk 1.5+).
    // Using it is more reliable for a mod injected into packaged system
    // processes such as ShellExperienceHost.exe than issuing WinINet requests
    // directly from that process.
    const WH_URL_CONTENT* content = Wh_GetUrlContent(url.c_str(), nullptr);
    if (!content) {
        return false;
    }

    const int status = content->statusCode;
    if (statusCode) {
        *statusCode = status;
    }

    bool ok = status == 200 && content->data != nullptr;
    if (ok) {
        body->assign(content->data, content->length);
    }

    Wh_FreeUrlContent(content);
    return ok;
}

bool DownloadHolidayYear(int year,
                         std::string* json,
                         wchar_t const** sourceName) {
    const std::wstring suffix = std::to_wstring(year) + L".json";

    struct Endpoint {
        wchar_t const* name;
        std::wstring url;
    };

    const Endpoint endpoints[] = {
        {
            L"jsDelivr",
            L"https://cdn.jsdelivr.net/gh/NateScarlet/holiday-cn@master/" +
                suffix,
        },
        {
            L"Fastly jsDelivr",
            L"https://fastly.jsdelivr.net/gh/NateScarlet/holiday-cn@master/" +
                suffix,
        },
        {
            L"GitHub Raw",
            L"https://raw.githubusercontent.com/NateScarlet/holiday-cn/master/" +
                suffix,
        },
    };

    for (auto const& endpoint : endpoints) {
        int status = 0;
        if (WindhawkGet(endpoint.url, json, &status)) {
            if (sourceName) {
                *sourceName = endpoint.name;
            }
            return true;
        }

        if (status != 0) {
            Wh_Log(L"Holiday data %d: %s returned HTTP %d",
                   year,
                   endpoint.name,
                   status);
        } else {
            Wh_Log(L"Holiday data %d: %s request failed",
                   year,
                   endpoint.name);
        }
    }

    if (sourceName) {
        *sourceName = L"";
    }
    return false;
}

struct HolidayFetchParam {
    int sourceYear;
};

DWORD WINAPI HolidayFetchThreadProc(LPVOID rawParam) {
    std::unique_ptr<HolidayFetchParam> param(
        static_cast<HolidayFetchParam*>(rawParam));
    const int sourceYear = param->sourceYear;

    std::string json;
    std::unordered_map<int, DayKind> parsed;
    wchar_t const* sourceName = L"";
    const bool downloaded =
        DownloadHolidayYear(sourceYear, &json, &sourceName);
    const bool parsedOk = downloaded && ParseHolidayJson(json, &parsed);

    AcquireSRWLockExclusive(&g_holidayDataLock);
    auto& state = g_holidayYearStates[sourceYear];
    state.fetching = false;
    if (parsedOk) {
        g_holidayDataBySourceYear[sourceYear] = parsed;
        state.fetchedSuccessfully = true;
    }
    ReleaseSRWLockExclusive(&g_holidayDataLock);

    if (parsedOk) {
        const bool cacheSaved = SaveHolidayCache(sourceYear, json);
        Wh_Log(L"Holiday data %d refreshed via %s (%u entries, cache=%s)",
               sourceYear,
               sourceName,
               static_cast<unsigned>(parsed.size()),
               cacheSaved ? L"saved" : L"not-saved");

        if (!g_stopping.load()) {
            RefreshAllCalendarsAcrossThreads();
        }
    } else {
        if (downloaded) {
            Wh_Log(L"Holiday data %d downloaded but JSON parsing failed; "
                   L"existing/cache/fallback data kept",
                   sourceYear);
        } else {
            Wh_Log(L"Holiday data %d refresh failed; "
                   L"existing/cache/fallback data kept",
                   sourceYear);
        }
    }

    return 0;
}

void EnsureHolidaySourceYearAsync(int sourceYear) {
    // holiday-cn currently starts at 2014. Avoid pointless network requests for
    // very old calendar pages or absurd future years.
    if (sourceYear < 2014 || sourceYear > 2100 || g_stopping.load()) {
        return;
    }

    bool shouldLoadCache = false;
    AcquireSRWLockExclusive(&g_holidayDataLock);
    {
        auto& state = g_holidayYearStates[sourceYear];
        if (!state.cacheLoadAttempted) {
            state.cacheLoadAttempted = true;
            shouldLoadCache = true;
        }
    }
    ReleaseSRWLockExclusive(&g_holidayDataLock);

    if (shouldLoadCache) {
        std::unordered_map<int, DayKind> cached;
        if (LoadHolidayCache(sourceYear, &cached)) {
            const auto cachedCount = cached.size();
            AcquireSRWLockExclusive(&g_holidayDataLock);
            g_holidayDataBySourceYear[sourceYear] = std::move(cached);
            ReleaseSRWLockExclusive(&g_holidayDataLock);

            Wh_Log(L"Holiday data %d loaded from persistent cache (%u entries)",
                   sourceYear,
                   static_cast<unsigned>(cachedCount));
        }
    }

    const ULONGLONG now = GetTickCount64();
    bool shouldStart = false;

    AcquireSRWLockExclusive(&g_holidayDataLock);
    auto& state = g_holidayYearStates[sourceYear];
    const ULONGLONG retryInterval = state.fetchedSuccessfully
                                        ? kSuccessfulRefreshMs
                                        : kFailedRetryMs;
    if (!state.fetching &&
        (state.lastAttemptTick == 0 ||
         now - state.lastAttemptTick >= retryInterval)) {
        state.fetching = true;
        state.lastAttemptTick = now;
        shouldStart = true;
    }
    ReleaseSRWLockExclusive(&g_holidayDataLock);

    if (!shouldStart) {
        return;
    }

    auto param = std::make_unique<HolidayFetchParam>();
    param->sourceYear = sourceYear;

    // Create suspended so Wh_ModUninit can't miss a just-created worker before
    // its handle is registered in g_fetchThreads.
    HANDLE thread = CreateThread(nullptr,
                                 0,
                                 HolidayFetchThreadProc,
                                 param.get(),
                                 CREATE_SUSPENDED,
                                 nullptr);
    if (!thread) {
        AcquireSRWLockExclusive(&g_holidayDataLock);
        g_holidayYearStates[sourceYear].fetching = false;
        ReleaseSRWLockExclusive(&g_holidayDataLock);
        return;
    }

    AcquireSRWLockExclusive(&g_fetchThreadsLock);
    if (g_stopping.load()) {
        ReleaseSRWLockExclusive(&g_fetchThreadsLock);
        CloseHandle(thread);
        AcquireSRWLockExclusive(&g_holidayDataLock);
        g_holidayYearStates[sourceYear].fetching = false;
        ReleaseSRWLockExclusive(&g_holidayDataLock);
        return;
    }

    g_fetchThreads.push_back(thread);
    param.release();
    ReleaseSRWLockExclusive(&g_fetchThreadsLock);
    ResumeThread(thread);
}

void EnsureHolidayDataForDate(int year) {
    EnsureHolidaySourceYearAsync(year);
    // holiday-cn documents that December dates can be affected by the following
    // year's State Council notice. Loading both also makes cross-year schedules
    // update automatically without a new mod release.
    EnsureHolidaySourceYearAsync(year + 1);
}

DayKind ClassifyDate(int year, int month, int day) {
    const int key = DateKey(year, month, day);
    DayKind result = DayKind::Normal;

    AcquireSRWLockShared(&g_holidayDataLock);

    // Prefer the following year's file if it contains this exact date, matching
    // holiday-cn's recommendation for cross-year adjustments.
    if (auto nextIt = g_holidayDataBySourceYear.find(year + 1);
        nextIt != g_holidayDataBySourceYear.end()) {
        if (auto dayIt = nextIt->second.find(key); dayIt != nextIt->second.end()) {
            result = dayIt->second;
        }
    }

    if (result == DayKind::Normal) {
        if (auto yearIt = g_holidayDataBySourceYear.find(year);
            yearIt != g_holidayDataBySourceYear.end()) {
            if (auto dayIt = yearIt->second.find(key);
                dayIt != yearIt->second.end()) {
                result = dayIt->second;
            }
        }
    }

    ReleaseSRWLockShared(&g_holidayDataLock);
    return result;
}

void StopHolidayWorkers() {
    g_stopping = true;

    std::vector<HANDLE> threads;
    AcquireSRWLockExclusive(&g_fetchThreadsLock);
    threads.swap(g_fetchThreads);
    ReleaseSRWLockExclusive(&g_fetchThreadsLock);

    for (HANDLE thread : threads) {
        if (thread) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
        }
    }
}

bool DateTimeToYmd(wf::DateTime const& dateTime, int* year, int* month, int* day) {
    // C++/WinRT DateTime uses the same 100 ns / 1601 epoch as FILETIME.
    // CalendarView dates are interpreted in the user's local time zone. Convert
    // through local time before taking Y/M/D so UTC+8 (and other zones) don't
    // accidentally classify the previous/next day.
    const auto rawTicks = dateTime.time_since_epoch().count();
    if (rawTicks < 0) {
        return false;
    }

    ULARGE_INTEGER ticks{};
    ticks.QuadPart = static_cast<ULONGLONG>(rawTicks);

    FILETIME ft{};
    ft.dwLowDateTime = ticks.LowPart;
    ft.dwHighDateTime = ticks.HighPart;

    SYSTEMTIME utc{};
    if (!FileTimeToSystemTime(&ft, &utc)) {
        return false;
    }

    SYSTEMTIME local{};
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &utc, &local)) {
        local = utc;
    }

    *year = local.wYear;
    *month = local.wMonth;
    *day = local.wDay;
    return true;
}

// -----------------------------------------------------------------------------
// Native CalendarView styling + selected-date header sync
// -----------------------------------------------------------------------------

thread_local std::unordered_map<std::wstring, wxc::ControlTemplate> g_dayTemplates;

wxc::ControlTemplate LoadTemplate(std::wstring const& xaml) {
    return wxm::XamlReader::Load(winrt::hstring{xaml}).as<wxc::ControlTemplate>();
}

std::wstring EscapeXamlText(std::wstring value) {
    std::wstring out;
    out.reserve(value.size() + 8);
    for (wchar_t ch : value) {
        switch (ch) {
            case L'&': out += L"&amp;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            case L'\"': out += L"&quot;"; break;
            default: out += ch; break;
        }
    }
    return out;
}

std::wstring SolarFestivalName(int month, int day);
std::wstring LunarFestivalName(int month, int day);
std::wstring LunarDayText(int day);

std::wstring SolarTermName2026(int year, int month, int day) {
    if (year != 2026) {
        return L"";
    }

    switch (DateKey(year, month, day)) {
        case 20260105: return L"小寒";
        case 20260120: return L"大寒";
        case 20260204: return L"立春";
        case 20260218: return L"雨水";
        case 20260305: return L"惊蛰";
        case 20260320: return L"春分";
        case 20260405: return L"清明";
        case 20260420: return L"谷雨";
        case 20260505: return L"立夏";
        case 20260521: return L"小满";
        case 20260605: return L"芒种";
        case 20260621: return L"夏至";
        case 20260707: return L"小暑";
        case 20260723: return L"大暑";
        case 20260807: return L"立秋";
        case 20260823: return L"处暑";
        case 20260907: return L"白露";
        case 20260923: return L"秋分";
        case 20261008: return L"寒露";
        case 20261023: return L"霜降";
        case 20261107: return L"立冬";
        case 20261122: return L"小雪";
        case 20261207: return L"大雪";
        case 20261222: return L"冬至";
    }

    return L"";
}

struct DaySecondaryInfo {
    std::wstring text;
    bool special = false;
};

DaySecondaryInfo GetDaySecondaryInfo(wf::DateTime const& dateTime) {
    SYSTEMTIME st{};

    const auto rawTicks = dateTime.time_since_epoch().count();
    if (rawTicks < 0) {
        return {};
    }

    ULARGE_INTEGER ticks{};
    ticks.QuadPart = static_cast<ULONGLONG>(rawTicks);
    FILETIME ft{};
    ft.dwLowDateTime = ticks.LowPart;
    ft.dwHighDateTime = ticks.HighPart;

    SYSTEMTIME utc{};
    if (!FileTimeToSystemTime(&ft, &utc)) {
        return {};
    }
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &utc, &st)) {
        st = utc;
    }

    if (auto solar = SolarFestivalName(st.wMonth, st.wDay); !solar.empty()) {
        return {solar, true};
    }

    if (auto term = SolarTermName2026(st.wYear, st.wMonth, st.wDay); !term.empty()) {
        return {term, true};
    }

    // 2026 Lunar New Year's Eve. Windows normally shows 除夕 instead of a
    // generic lunar day on this cell.
    if (DateKey(st.wYear, st.wMonth, st.wDay) == 20260216) {
        return {L"除夕", true};
    }

    try {
        wg::Calendar lunar;
        lunar.ChangeCalendarSystem(wg::CalendarIdentifiers::ChineseLunar());
        lunar.SetDateTime(dateTime);

        const int lunarMonth = lunar.Month();
        const int lunarDay = lunar.Day();

        if (auto festival = LunarFestivalName(lunarMonth, lunarDay);
            !festival.empty()) {
            return {festival, true};
        }

        return {LunarDayText(lunarDay), false};
    } catch (...) {
        return {};
    }
}

wxc::ControlTemplate DayTemplate(DayKind kind,
                                 std::wstring const& secondaryText,
                                 bool specialSecondary) {
    const wchar_t* kindKey = kind == DayKind::Holiday ? L"H" : L"W";
    std::wstring key = kindKey;
    key += specialSecondary ? L"|S|" : L"|N|";
    key += secondaryText;

    if (auto it = g_dayTemplates.find(key); it != g_dayTemplates.end()) {
        return it->second;
    }

    const wchar_t* circle =
        kind == DayKind::Holiday ? L"#22FF4D4F" : L"#18D99A35";
    const wchar_t* badge =
        kind == DayKind::Holiday ? L"#FFF04444" : L"#FF5D6B82";
    const wchar_t* badgeText =
        kind == DayKind::Holiday ? L"休" : L"班";

    // Keep generic lunar text quiet. Festival/solar-term text gets a little more
    // emphasis, but never a background of its own.
    const wchar_t* secondaryForeground = specialSecondary
        ? (kind == DayKind::Holiday ? L"#FFD94A4A" : L"#FF677386")
        : L"#FF777777";

    std::wstring xaml = LR"xaml(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
                 TargetType="CalendarViewDayItem">
    <Grid IsHitTestVisible="False">
        <!-- Full, soft status circle. It sits below the native Gregorian number. -->
        <Border Background=")xaml";
    xaml += circle;
    xaml += LR"xaml("
                Width="36"
                Height="36"
                CornerRadius="18"
                HorizontalAlignment="Center"
                VerticalAlignment="Center"/>

        <!-- Restore the line that ShellExperienceHost drops when a local template
             is applied. No background, no pill: just the lunar/festival/term text. -->
        <TextBlock Text=")xaml";
    xaml += EscapeXamlText(secondaryText);
    xaml += LR"xaml("
                   Foreground=")xaml";
    xaml += secondaryForeground;
    xaml += LR"xaml("
                   FontSize="9"
                   FontWeight="Normal"
                   HorizontalAlignment="Center"
                   VerticalAlignment="Bottom"
                   Margin="0,0,0,1"/>

        <!-- Plain text marker, intentionally inset so it isn't clipped. -->
        <TextBlock Text=")xaml";
    xaml += badgeText;
    xaml += LR"xaml("
                   Foreground=")xaml";
    xaml += badge;
    xaml += LR"xaml("
                   FontSize="8"
                   FontWeight="SemiBold"
                   HorizontalAlignment="Right"
                   VerticalAlignment="Top"
                   Margin="0,2,3,0"/>
    </Grid>
</ControlTemplate>)xaml";

    auto loaded = LoadTemplate(xaml);
    g_dayTemplates.emplace(key, loaded);
    return loaded;
}

thread_local std::vector<wxc::CalendarViewDayItem> g_touchedDayItems;

void RememberTouchedItem(wxc::CalendarViewDayItem const& item) {
    void* abi = winrt::get_abi(item);
    for (auto const& existing : g_touchedDayItems) {
        if (winrt::get_abi(existing) == abi) {
            return;
        }
    }
    g_touchedDayItems.push_back(item);
}

void RestoreTouchedItems() {
    for (auto const& item : g_touchedDayItems) {
        try {
            item.ClearValue(wxc::Control::TemplateProperty());
            item.ClearValue(wxc::Control::ForegroundProperty());
        } catch (...) {
        }
    }
    g_touchedDayItems.clear();
}

void ApplyDayStyle(wxc::CalendarViewDayItem const& item) {
    int year = 0;
    int month = 0;
    int day = 0;
    if (!DateTimeToYmd(item.Date(), &year, &month, &day)) {
        return;
    }

    EnsureHolidayDataForDate(year);
    const auto kind = ClassifyDate(year, month, day);
    RememberTouchedItem(item);

    if (kind == DayKind::Normal) {
        // CalendarView recycles containers. Remove all local values that we may
        // have attached when this container represented a holiday/workday.
        item.ClearValue(wxc::Control::TemplateProperty());
        item.ClearValue(wxc::Control::ForegroundProperty());
        return;
    }

    const auto secondary = GetDaySecondaryInfo(item.Date());
    item.Template(DayTemplate(kind, secondary.text, secondary.special));

    // Do not recolor the native Gregorian number. This keeps Windows' own hover,
    // selection and today states intact and matches the user's requested style.
    item.ClearValue(wxc::Control::ForegroundProperty());
}

void RefreshDayItemsRecursive(wux::DependencyObject const& root,
                              int depth = 0) {
    if (!root || depth > 32) {
        return;
    }

    if (auto dayItem = root.try_as<wxc::CalendarViewDayItem>()) {
        ApplyDayStyle(dayItem);
    }

    int count = 0;
    try {
        count = wxmedia::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        RefreshDayItemsRecursive(
            wxmedia::VisualTreeHelper::GetChild(root, i), depth + 1);
    }
}

// -----------------------------------------------------------------------------
// Header helpers. Windows' notification center normally keeps CalendarHeader
// pinned to today. We intentionally update it when CalendarView selection changes.
// -----------------------------------------------------------------------------

std::wstring WeekdayText(WORD dayOfWeek) {
    static constexpr wchar_t const* kWeekdays[] = {
        L"星期日", L"星期一", L"星期二", L"星期三",
        L"星期四", L"星期五", L"星期六",
    };
    return kWeekdays[dayOfWeek % 7];
}

bool DateTimeToLocalSystemTime(wf::DateTime const& dateTime, SYSTEMTIME* local) {
    const auto rawTicks = dateTime.time_since_epoch().count();
    if (rawTicks < 0) {
        return false;
    }

    ULARGE_INTEGER ticks{};
    ticks.QuadPart = static_cast<ULONGLONG>(rawTicks);

    FILETIME ft{};
    ft.dwLowDateTime = ticks.LowPart;
    ft.dwHighDateTime = ticks.HighPart;

    SYSTEMTIME utc{};
    if (!FileTimeToSystemTime(&ft, &utc)) {
        return false;
    }

    if (!SystemTimeToTzSpecificLocalTime(nullptr, &utc, local)) {
        *local = utc;
    }
    return true;
}

std::wstring PrimaryHeaderText(wf::DateTime const& dateTime) {
    SYSTEMTIME st{};
    if (!DateTimeToLocalSystemTime(dateTime, &st)) {
        return L"";
    }

    wchar_t buffer[64];
    swprintf_s(buffer,
               L"%u月%u日, %s",
               st.wMonth,
               st.wDay,
               WeekdayText(st.wDayOfWeek).c_str());
    return buffer;
}

std::wstring LunarMonthText(int month) {
    static constexpr wchar_t const* kMonths[] = {
        L"", L"正月", L"二月", L"三月", L"四月", L"五月", L"六月",
        L"七月", L"八月", L"九月", L"十月", L"冬月", L"腊月", L"十三月",
    };
    if (month >= 1 && month <= 13) {
        return kMonths[month];
    }
    return L"";
}

std::wstring LunarDayText(int day) {
    static constexpr wchar_t const* kDays[] = {
        L"", L"初一", L"初二", L"初三", L"初四", L"初五", L"初六", L"初七", L"初八", L"初九", L"初十",
        L"十一", L"十二", L"十三", L"十四", L"十五", L"十六", L"十七", L"十八", L"十九", L"二十",
        L"廿一", L"廿二", L"廿三", L"廿四", L"廿五", L"廿六", L"廿七", L"廿八", L"廿九", L"三十",
    };
    if (day >= 1 && day <= 30) {
        return kDays[day];
    }
    return L"";
}

std::wstring SolarFestivalName(int month, int day) {
    if (month == 1 && day == 1) return L"元旦";
    if (month == 5 && day == 1) return L"劳动节";
    if (month == 10 && day == 1) return L"国庆节";
    return L"";
}

std::wstring LunarFestivalName(int month, int day) {
    if (month == 1 && day == 1) return L"春节";
    if (month == 1 && day == 15) return L"元宵节";
    if (month == 5 && day == 5) return L"端午节";
    if (month == 7 && day == 7) return L"七夕";
    if (month == 7 && day == 15) return L"中元节";
    if (month == 8 && day == 15) return L"中秋节";
    if (month == 9 && day == 9) return L"重阳节";
    if (month == 12 && day == 8) return L"腊八节";
    return L"";
}

std::wstring SecondaryHeaderText(wf::DateTime const& dateTime) {
    SYSTEMTIME st{};
    if (!DateTimeToLocalSystemTime(dateTime, &st)) {
        return L"";
    }

    if (auto solar = SolarFestivalName(st.wMonth, st.wDay); !solar.empty()) {
        return solar;
    }

    if (auto term = SolarTermName2026(st.wYear, st.wMonth, st.wDay); !term.empty()) {
        return term;
    }

    if (DateKey(st.wYear, st.wMonth, st.wDay) == 20260216) {
        return L"除夕";
    }

    try {
        wg::Calendar lunar;
        lunar.ChangeCalendarSystem(wg::CalendarIdentifiers::ChineseLunar());
        lunar.SetDateTime(dateTime);

        const int lunarMonth = lunar.Month();
        const int lunarDay = lunar.Day();

        if (auto festival = LunarFestivalName(lunarMonth, lunarDay);
            !festival.empty()) {
            return festival;
        }

        return LunarMonthText(lunarMonth) + LunarDayText(lunarDay);
    } catch (...) {
        return L"";
    }
}

wux::DependencyObject FindNamedDescendant(wux::DependencyObject const& root,
                                          wchar_t const* name,
                                          int depth = 0) {
    if (!root || depth > 24) {
        return nullptr;
    }

    if (auto fe = root.try_as<wux::FrameworkElement>()) {
        if (fe.Name() == name) {
            return root;
        }
    }

    int count = 0;
    try {
        count = wxmedia::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        auto child = wxmedia::VisualTreeHelper::GetChild(root, i);
        if (auto found = FindNamedDescendant(child, name, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

void CollectTextBlocks(wux::DependencyObject const& root,
                       std::vector<wxc::TextBlock>* result,
                       int depth = 0) {
    if (!root || depth > 16) {
        return;
    }

    if (auto tb = root.try_as<wxc::TextBlock>()) {
        result->push_back(tb);
    }

    int count = 0;
    try {
        count = wxmedia::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        CollectTextBlocks(wxmedia::VisualTreeHelper::GetChild(root, i),
                          result,
                          depth + 1);
    }
}

wxc::Button FindDateButton(wux::DependencyObject const& root) {
    if (!root) {
        return nullptr;
    }

    if (auto button = root.try_as<wxc::Button>()) {
        auto name = button.Name();
        if (name == L"DateTextButtonWithClock" ||
            name == L"DateTextButtonWithClockOld") {
            return button;
        }
    }

    int count = 0;
    try {
        count = wxmedia::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        if (auto button = FindDateButton(
                wxmedia::VisualTreeHelper::GetChild(root, i))) {
            return button;
        }
    }

    return nullptr;
}

struct HeaderTargets {
    wxc::TextBlock primary{nullptr};
    wxc::TextBlock secondary{nullptr};
    wxc::Button dateButtonFallback{nullptr};

    winrt::hstring originalPrimary;
    winrt::hstring originalSecondary;
    wf::IInspectable originalButtonContent{nullptr};
    bool resolved = false;
};

void ResolveHeaderTargets(wxc::CalendarView const& calendar,
                          HeaderTargets* targets) {
    if (targets->resolved) {
        return;
    }

    wux::DependencyObject cursor = calendar;
    wux::DependencyObject calendarSection{nullptr};
    wux::DependencyObject highest = calendar;

    for (int i = 0; i < 16; i++) {
        auto parent = wxmedia::VisualTreeHelper::GetParent(cursor);
        if (!parent) {
            break;
        }
        highest = parent;
        cursor = parent;

        if (auto fe = cursor.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == L"CalendarSection") {
                calendarSection = cursor;
                break;
            }
        }
    }

    auto searchRoot = calendarSection ? calendarSection : highest;
    auto headerObject = FindNamedDescendant(searchRoot, L"CalendarHeader");
    if (!headerObject) {
        Wh_Log(L"CalendarHeader not found yet");
        return;
    }

    std::vector<wxc::TextBlock> textBlocks;
    CollectTextBlocks(headerObject, &textBlocks);

    for (auto const& tb : textBlocks) {
        auto text = tb.Text();
        auto name = tb.Name();
        Wh_Log(L"CalendarHeader TextBlock name='%s' text='%s'",
               name.c_str(),
               text.c_str());

        std::wstring value{text.c_str()};
        if (!targets->primary && value.find(L"星期") != std::wstring::npos) {
            targets->primary = tb;
            targets->originalPrimary = text;
            continue;
        }
    }

    // The additional-calendar/lunar line is normally the other short text block
    // in CalendarHeader. Prefer a compact non-time string containing Chinese date
    // characters, but keep a fallback to the first suitable non-empty line.
    for (auto const& tb : textBlocks) {
        if (targets->primary && winrt::get_abi(tb) == winrt::get_abi(targets->primary)) {
            continue;
        }

        std::wstring value{tb.Text().c_str()};
        if (value.empty() || value.find(L':') != std::wstring::npos ||
            value.find(L"开始") != std::wstring::npos) {
            continue;
        }

        const bool looksLunar =
            value.find(L'月') != std::wstring::npos ||
            value.find(L"节") != std::wstring::npos ||
            value.find(L"初") != std::wstring::npos ||
            value.find(L"廿") != std::wstring::npos;

        if (looksLunar) {
            targets->secondary = tb;
            targets->originalSecondary = tb.Text();
            break;
        }
    }

    if (!targets->primary) {
        targets->dateButtonFallback = FindDateButton(headerObject);
        if (targets->dateButtonFallback) {
            targets->originalButtonContent =
                targets->dateButtonFallback.Content();
        }
    }

    targets->resolved =
        targets->primary || targets->secondary || targets->dateButtonFallback;

    Wh_Log(L"CalendarHeader targets: primary=%d secondary=%d button=%d",
           targets->primary ? 1 : 0,
           targets->secondary ? 1 : 0,
           targets->dateButtonFallback ? 1 : 0);
}

void RestoreHeaderTargets(HeaderTargets* targets) {
    try {
        if (targets->primary) {
            targets->primary.Text(targets->originalPrimary);
        } else if (targets->dateButtonFallback &&
                   targets->originalButtonContent) {
            targets->dateButtonFallback.Content(targets->originalButtonContent);
        }

        if (targets->secondary) {
            targets->secondary.Text(targets->originalSecondary);
        }
    } catch (...) {
    }
}

struct CalendarSubscription {
    wxc::CalendarView calendar{nullptr};
    winrt::event_token dayItemChangingToken{};
    winrt::event_token selectedDatesChangedToken{};
    HeaderTargets header;
};

thread_local std::unordered_map<InstanceHandle, CalendarSubscription>
    g_calendarSubscriptions;

void RefreshAllCalendarsForCurrentThread() {
    for (auto const& [handle, subscription] : g_calendarSubscriptions) {
        try {
            RefreshDayItemsRecursive(subscription.calendar);
        } catch (...) {
        }
    }
}

void UpdateHeaderForDate(InstanceHandle handle,
                         wxc::CalendarView const& calendar,
                         wf::DateTime const& dateTime) {
    auto it = g_calendarSubscriptions.find(handle);
    if (it == g_calendarSubscriptions.end()) {
        return;
    }

    auto& header = it->second.header;
    ResolveHeaderTargets(calendar, &header);

    const auto primary = PrimaryHeaderText(dateTime);
    const auto secondary = SecondaryHeaderText(dateTime);

    try {
        if (header.primary) {
            header.primary.Text(primary);
        } else if (header.dateButtonFallback) {
            header.dateButtonFallback.Content(
                winrt::box_value(winrt::hstring{primary}));
        }

        if (header.secondary && !secondary.empty()) {
            header.secondary.Text(secondary);
        }

        Wh_Log(L"Selected date header -> '%s' / '%s'",
               primary.c_str(),
               secondary.c_str());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Header update failed: %08X %s",
               ex.code().value,
               ex.message().c_str());
    }
}

void RegisterCalendar(InstanceHandle handle, wxc::CalendarView const& calendar) {
    if (g_calendarSubscriptions.find(handle) != g_calendarSubscriptions.end()) {
        return;
    }

    Wh_Log(L"Found native CalendarView, handle=%llu", handle);

    CalendarSubscription subscription;
    subscription.calendar = calendar;

    subscription.dayItemChangingToken = calendar.CalendarViewDayItemChanging(
        [](wxc::CalendarView const&,
           wxc::CalendarViewDayItemChangingEventArgs const& args) {
            try {
                auto item = args.Item();
                if (item) {
                    ApplyDayStyle(item);
                }
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Calendar day styling failed: %08X %s",
                       ex.code().value,
                       ex.message().c_str());
            } catch (...) {
                Wh_Log(L"Calendar day styling failed: unknown exception");
            }
        });

    subscription.selectedDatesChangedToken = calendar.SelectedDatesChanged(
        [handle](wxc::CalendarView const& sender,
                 wxc::CalendarViewSelectedDatesChangedEventArgs const& args) {
            try {
                auto added = args.AddedDates();
                if (added.Size() > 0) {
                    UpdateHeaderForDate(handle, sender, added.GetAt(0));
                    return;
                }

                auto selected = sender.SelectedDates();
                if (selected.Size() > 0) {
                    UpdateHeaderForDate(handle, sender, selected.GetAt(0));
                }
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"SelectedDatesChanged failed: %08X %s",
                       ex.code().value,
                       ex.message().c_str());
            } catch (...) {
                Wh_Log(L"SelectedDatesChanged failed: unknown exception");
            }
        });

    g_calendarSubscriptions.emplace(handle, std::move(subscription));
}

void UnregisterCalendar(InstanceHandle handle) {
    auto it = g_calendarSubscriptions.find(handle);
    if (it == g_calendarSubscriptions.end()) {
        return;
    }

    RestoreHeaderTargets(&it->second.header);

    try {
        it->second.calendar.CalendarViewDayItemChanging(
            it->second.dayItemChangingToken);
    } catch (...) {
    }

    try {
        it->second.calendar.SelectedDatesChanged(
            it->second.selectedDatesChangedToken);
    } catch (...) {
    }

    g_calendarSubscriptions.erase(it);
}

void UnregisterAllCalendarsForCurrentThread() {
    for (auto& [handle, subscription] : g_calendarSubscriptions) {
        RestoreHeaderTargets(&subscription.header);

        try {
            subscription.calendar.CalendarViewDayItemChanging(
                subscription.dayItemChangingToken);
        } catch (...) {
        }

        try {
            subscription.calendar.SelectedDatesChanged(
                subscription.selectedDatesChangedToken);
        } catch (...) {
        }
    }
    g_calendarSubscriptions.clear();
    RestoreTouchedItems();
    g_dayTemplates.clear();
}

// -----------------------------------------------------------------------------
// XAML Diagnostics bootstrap.
// Based on the VisualTreeWatcher/TAP pattern used by m417z's
// "Windows 11 Notification Center Styler" (GPL-3.0).
// -----------------------------------------------------------------------------

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
            &module)) {
        return nullptr;
    }
    return module;
}

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_xamlDiagnostics(site.as<IXamlDiagnostics>()) {
        Wh_Log(L"Constructing VisualTreeWatcher");

        // Calling AdviseVisualTreeChange directly can occasionally hang while
        // XAML Diagnostics marshals to the UI thread. Use the same workaround
        // as the official Windhawk styler: advise from a short-lived thread.
        HANDLE thread = CreateThread(
            nullptr,
            0,
            [](LPVOID param) -> DWORD {
                auto watcher = reinterpret_cast<VisualTreeWatcher*>(param);
                HRESULT hr = watcher->m_xamlDiagnostics
                                 .as<IVisualTreeService3>()
                                 ->AdviseVisualTreeChange(watcher);
                watcher->Release();
                if (FAILED(hr)) {
                    Wh_Log(L"AdviseVisualTreeChange failed: %08X", hr);
                }
                return 0;
            },
            this,
            0,
            nullptr);

        if (thread) {
            AddRef();
            CloseHandle(thread);
        }
    }

    void UnadviseVisualTreeChange() {
        HRESULT hr = m_xamlDiagnostics.as<IVisualTreeService3>()
                         ->UnadviseVisualTreeChange(this);
        if (FAILED(hr)) {
            Wh_Log(L"UnadviseVisualTreeChange failed: %08X", hr);
        }
    }

private:
    wf::IInspectable TryFromHandle(InstanceHandle handle) {
        wf::IInspectable object;
        const HRESULT hr = m_xamlDiagnostics->GetIInspectableFromHandle(
            handle,
            reinterpret_cast<::IInspectable**>(winrt::put_abi(object)));

        // Visual-tree notifications race with XAML element destruction. A handle
        // can legitimately disappear before GetIInspectableFromHandle runs.
        // ERROR_NOT_FOUND (0x80070490) is therefore expected and not a failure.
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            return nullptr;
        }

        winrt::check_hresult(hr);
        return object;
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) override {
        try {
            if (mutationType == Add) {
                auto object = TryFromHandle(element.Handle);
                if (!object) {
                    return S_OK;
                }

                if (auto dayItem = object.try_as<wxc::CalendarViewDayItem>()) {
                    // AdviseVisualTreeChange also enumerates existing elements,
                    // so this styles the month that's already visible before we
                    // subscribed to CalendarViewDayItemChanging.
                    ApplyDayStyle(dayItem);
                } else if (auto calendar = object.try_as<wxc::CalendarView>()) {
                    RegisterCalendar(element.Handle, calendar);
                }
            } else if (mutationType == Remove) {
                UnregisterCalendar(element.Handle);
            }
        } catch (winrt::hresult_error const& ex) {
            // Don't propagate failures to XAML Diagnostics; doing so can stop
            // further visual-tree notifications.
            Wh_Log(L"OnVisualTreeChange error: %08X %s",
                   ex.code().value,
                   ex.message().c_str());
        } catch (...) {
            Wh_Log(L"OnVisualTreeChange error: unknown exception");
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle,
        VisualElementState,
        LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_xamlDiagnostics;
};

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {0CA40617-193C-4702-A32F-8D7129F7817F}
static constexpr CLSID CLSID_WindhawkHolidayTAP = {
    0x0ca40617,
    0x193c,
    0x4702,
    {0xa3, 0x2f, 0x8d, 0x71, 0x29, 0xf7, 0x81, 0x7f}};

class WindhawkHolidayTAP
    : public winrt::implements<WindhawkHolidayTAP,
                               IObjectWithSite,
                               winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* unknownSite) override {
        try {
            if (g_visualTreeWatcher) {
                g_visualTreeWatcher->UnadviseVisualTreeChange();
                g_visualTreeWatcher = nullptr;
            }

            m_site.copy_from(unknownSite);
            if (m_site) {
                // Balance the module refcount added by InitializeXamlDiagnosticsEx.
                FreeLibrary(GetCurrentModuleHandle());
                g_visualTreeWatcher =
                    winrt::make_self<VisualTreeWatcher>(m_site);
            }
            return S_OK;
        } catch (...) {
            const HRESULT hr = winrt::to_hresult();
            Wh_Log(L"WindhawkHolidayTAP::SetSite failed: %08X", hr);
            return hr;
        }
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** site) noexcept override {
        return m_site.as(riid, site);
    }

private:
    winrt::com_ptr<IUnknown> m_site;
};

template <class T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(
        IUnknown* outer,
        REFIID riid,
        void** object) override {
        try {
            if (outer) {
                return CLASS_E_NOAGGREGATION;
            }
            *object = nullptr;
            return winrt::make<T>().as(riid, object);
        } catch (...) {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) STDAPI DllGetClassObject(
    REFCLSID clsid,
    REFIID riid,
    LPVOID* object) {
    try {
        if (clsid != CLSID_WindhawkHolidayTAP) {
            return CLASS_E_CLASSNOTAVAILABLE;
        }
        *object = nullptr;
        return winrt::make<SimpleFactory<WindhawkHolidayTAP>>()
            .as(riid, object);
    } catch (...) {
        return winrt::to_hresult();
    }
}

__declspec(dllexport) STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX =
    decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkHolidayTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileNameW(module, location, ARRAYSIZE(location))) {
        case 0:
        case ARRAYSIZE(location):
            return HRESULT_FROM_WIN32(GetLastError());
    }

    HMODULE xamlModule = LoadLibraryExW(
        L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!xamlModule) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    auto initializeXamlDiagnosticsEx =
        reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
            GetProcAddress(xamlModule, "InitializeXamlDiagnosticsEx"));
    if (!initializeXamlDiagnosticsEx) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[64];
        wsprintfW(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = initializeXamlDiagnosticsEx(
            connectionName,
            GetCurrentProcessId(),
            L"",
            location,
            CLSID_WindhawkHolidayTAP,
            nullptr);

        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    return hr;
}

std::atomic<bool> g_tapInitialized{false};

void EnsureTapInitialized() {
    if (g_tapInitialized.exchange(true)) {
        return;
    }

    const HRESULT hr = InjectWindhawkHolidayTAP();
    if (FAILED(hr)) {
        g_tapInitialized = false;
        Wh_Log(L"InitializeXamlDiagnosticsEx failed: %08X", hr);
    } else {
        Wh_Log(L"XAML Diagnostics initialized");
    }
}

void UninitializeTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }
    g_tapInitialized = false;
}

// -----------------------------------------------------------------------------
// Run cleanup on the native calendar UI thread.
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc proc,
                         PVOID procParam) {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct Param {
        RunFromWindowThreadProc proc;
        PVOID procParam;
    };

    const DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto cwp = reinterpret_cast<CWPSTRUCT*>(lParam);
                if (cwp->message == message) {
                    auto param = reinterpret_cast<Param*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr,
        threadId);

    if (!hook) {
        return false;
    }

    Param param{proc, procParam};
    SendMessageW(window, message, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

std::vector<HWND> GetCoreWindows() {
    std::vector<HWND> windows;

    EnumWindows(
        [](HWND window, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            if (!GetWindowThreadProcessId(window, &processId) ||
                processId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[64]{};
            if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
                reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(window);
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));

    return windows;
}

void RefreshAllCalendarsAcrossThreads() {
    if (g_stopping.load()) {
        return;
    }

    for (HWND window : GetCoreWindows()) {
        RunFromWindowThread(
            window,
            [](PVOID) { RefreshAllCalendarsForCurrentThread(); },
            nullptr);
    }
}

// -----------------------------------------------------------------------------
// Catch creation of the ShellExperienceHost CoreWindow so the mod also works
// when ShellExperienceHost starts after Windhawk injects us.
// -----------------------------------------------------------------------------

bool IsCoreWindowClass(LPCWSTR className) {
    if (!className) {
        return false;
    }

    const BOOL textual =
        ((ULONG_PTR)className & ~(ULONG_PTR)0xffff) != 0;
    return textual &&
           _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0;
}

using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD,
    LPCWSTR,
    LPCWSTR,
    DWORD,
    int,
    int,
    int,
    int,
    HWND,
    HMENU,
    HINSTANCE,
    PVOID,
    DWORD);

CreateWindowInBand_t CreateWindowInBand_Original = nullptr;

HWND WINAPI CreateWindowInBand_Hook(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID param,
    DWORD band) {
    HWND window = CreateWindowInBand_Original(
        exStyle,
        className,
        windowName,
        style,
        x,
        y,
        width,
        height,
        parent,
        menu,
        instance,
        param,
        band);

    if (window && IsCoreWindowClass(className)) {
        EnsureTapInitialized();
    }

    return window;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(
    DWORD,
    LPCWSTR,
    LPCWSTR,
    DWORD,
    int,
    int,
    int,
    int,
    HWND,
    HMENU,
    HINSTANCE,
    PVOID,
    DWORD,
    DWORD);

CreateWindowInBandEx_t CreateWindowInBandEx_Original = nullptr;

HWND WINAPI CreateWindowInBandEx_Hook(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID param,
    DWORD band,
    DWORD typeFlags) {
    HWND window = CreateWindowInBandEx_Original(
        exStyle,
        className,
        windowName,
        style,
        x,
        y,
        width,
        height,
        parent,
        menu,
        instance,
        param,
        band,
        typeFlags);

    if (window && IsCoreWindowClass(className)) {
        EnsureTapInitialized();
    }

    return window;
}

// -----------------------------------------------------------------------------
// Windhawk entry points
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing native China holiday calendar v1.0.1");

    g_stopping = false;
    InitializeBuiltIn2026Fallback();

    HMODULE user32 =
        LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) {
        if (void* createWindowInBand =
                reinterpret_cast<void*>(
                    GetProcAddress(user32, "CreateWindowInBand"))) {
            Wh_SetFunctionHook(
                createWindowInBand,
                reinterpret_cast<void*>(CreateWindowInBand_Hook),
                reinterpret_cast<void**>(&CreateWindowInBand_Original));
        }

        if (void* createWindowInBandEx =
                reinterpret_cast<void*>(
                    GetProcAddress(user32, "CreateWindowInBandEx"))) {
            Wh_SetFunctionHook(
                createWindowInBandEx,
                reinterpret_cast<void*>(CreateWindowInBandEx_Hook),
                reinterpret_cast<void**>(&CreateWindowInBandEx_Original));
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!GetCoreWindows().empty()) {
        EnsureTapInitialized();
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing native China holiday calendar v1.0.1");

    // Finish background network workers before Windhawk unloads this DLL.
    StopHolidayWorkers();

    // Stop visual-tree callbacks before removing event handlers.
    UninitializeTap();

    for (HWND window : GetCoreWindows()) {
        RunFromWindowThread(
            window,
            [](PVOID) { UnregisterAllCalendarsForCurrentThread(); },
            nullptr);
    }
}
