// ==WindhawkMod==
// @id              vietnamese-lunar-calendar
// @name            Vietnamese Lunar Calendar & Horoscope
// @description     Hiển thị Âm lịch trên Taskbar, Tử Vi trong Tooltip khi rê chuột vào đồng hồ, và Lịch Vạn Niên trên khay Lịch
// @version         2.0.1
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vietnamese Lunar Calendar & Horoscope

Bộ công cụ **Lịch Vạn Niên & Tử Vi Việt Nam** tích hợp toàn diện vào giao diện gốc của Windows 10, Windows 11, hoạt động độc lập mà **không cần bật Lịch phụ tiếng Trung** trong Cài đặt Windows.

Github: caotu2k5

## ✨ Tính năng chính

### 1. Đồng hồ Âm - Dương trên thanh Taskbar (`explorer.exe`)
* **Dòng trên:** Hiển thị Giờ:Phút, Thứ tiếng Việt và Ngày/Tháng Dương lịch (VD: `14:46 - T4, 14/10`).
* **Dòng dưới:** Hiển thị Ngày/Tháng Âm lịch (tự động nhận diện tháng Nhuận) kèm năm Can Chi (VD: `05/09 Bính Ngọ`).

### 2. Bảng Bát Tự & Tử Vi trong hộp Tooltip khi rê chuột vào đồng hồ
Khi di chuột lên đồng hồ góc phải Taskbar, hộp chú thích (Tooltip) sẽ tự động hiển thị:
* 📅 **Âm lịch chi tiết:** Ngày, Tháng, Năm Can Chi và tên Ngày lễ/Sự kiện trong năm.
* 🐉 **Bát tự (Tứ trụ):** Giờ Can Chi • Ngày Can Chi • Tháng Can Chi • Năm Can Chi.
* ☯ **Ngũ hành & Trực/Sao:** Mệnh ngày (30 Nạp âm), 12 Sao Hoàng Đạo / Hắc Đạo và Thập Nhị Trực.
* ⏰ **Giờ Hoàng Đạo:** 6 khung giờ tốt trong ngày để xuất hành, thực hiện việc quan trọng.
* 🔮 **Tử vi (Nên & Kỵ):** Gợi ý các việc cát lợi nên làm và các việc cần tránh trong ngày.
* 🧭 **Phong thủy ngày:** Tuổi xung khắc trong ngày và hướng xuất hành đón Hỷ Thần, Tài Thần.

### 3. Khay Lịch Âm Việt Nam & Đánh dấu Ngày Lễ (`ShellExperienceHost.exe` / `ShellHost.exe`)
* Tự động tạo và căn chỉnh dòng ngày Âm lịch bên dưới số ngày Dương lịch trên lưới lịch Windows 11 (kể cả khi chọn *"Không hiển thị các lịch khác"* trong Settings).
* Hiển thị dòng tiêu đề Âm lịch tiếng Việt trên Header (VD: `5 Tháng 9 Năm Bính Ngọ`).
* **Phân loại màu sắc ngày lễ chuẩn Lịch Bloc Việt Nam:**
  * 🔴 **Đỏ cả số Dương và chữ Âm:** Tết Dương lịch (1/1), Tết Nguyên Đán (Mùng 1–3, 29–30 Tết), Rằm tháng Giêng, Lễ Vu Lan (15/7 ÂL), Tết Trung Thu (15/8 ÂL).
  * 🔴 **Chỉ đỏ số Dương lịch:** Các ngày lễ Dương lịch Việt Nam & Quốc tế (14/2, 8/3, 26/3, 30/4, 1/5, 1/6, 2/9, 20/10, 20/11, 24/11, 22/12, 24–25/12) — giữ nguyên màu trung tính cho dòng Âm lịch bên dưới.
  * 🔴 **Chỉ đỏ dòng Âm lịch:** Giỗ Tổ Hùng Vương (10/3 ÂL), Lễ Phật Đản (15/4 ÂL), Tết Đoan Ngọ (5/5 ÂL), Ông Công Ông Táo (23/12 ÂL).

### 4. Trang Chi tiết Tử Vi khi Click vào từng ô ngày
* Khi bấm chọn một ngày bất kỳ trên khay lịch mở rộng, giao diện sẽ chuyển sang **Trang tra cứu Tử Vi & Can Chi** của riêng ngày hôm đó.
* Hỗ trợ nút **`←`** ở góc trên bên trái để quay lại lưới lịch tháng nhanh chóng.
*/
// ==/WindhawkModReadme==

#include <atomic>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

#undef GetCurrentTime

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.Globalization.h>

thread_local bool g_initializedForThread;

#pragma region winrt_hpp

#include <Unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
    namespace Windows {
        namespace Foundation {}
        namespace UI::Xaml {}
    }
}

namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;

#pragma endregion

#include <windhawk_utils.h>
#include <chrono>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

////////////////////////////////////////////////////////////////////////////////
// Settings
struct Settings {
    std::wstring dataSourceUrl;
    int refreshIntervalHours = 24;
    bool replaceText = true;
    std::wstring offDayText;
    std::wstring workdayText = L"Bù";
    winrt::Windows::UI::Color offDayColor{255, 78, 161, 255};
    winrt::Windows::UI::Color workdayColor{255, 255, 90, 90};
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings = std::make_shared<Settings>();

std::shared_ptr<const Settings> GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring ReadStringSetting(PCWSTR name) {
    const WindhawkUtils::StringSetting value = WindhawkUtils::StringSetting::make(name);
    return std::wstring(value.get());
}

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring const& text) {
    std::wstring value;
    for (wchar_t c : text) { if (!iswspace(c)) value += c; }
    if (!value.empty() && value[0] == L'#') value = value.substr(1);
    if (value.size() != 6 && value.size() != 8) return std::nullopt;

    auto hexDigit = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };

    uint8_t bytes[4] = {0xFF, 0x00, 0x00, 0x00};
    size_t offset = value.size() == 8 ? 0 : 1;
    for (size_t i = 0; i + 1 < value.size(); i += 2) {
        int hi = hexDigit(value[i]); int lo = hexDigit(value[i + 1]);
        if (hi < 0 || lo < 0) return std::nullopt;
        bytes[offset + i / 2] = static_cast<uint8_t>((hi << 4) | lo);
    }
    return winrt::Windows::UI::Color{bytes[0], bytes[1], bytes[2], bytes[3]};
}

winrt::Windows::UI::Color ReadColorSetting(PCWSTR name, winrt::Windows::UI::Color fallback) {
    if (auto color = ParseHexColor(ReadStringSetting(name))) return *color;
    return fallback;
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();
    settings->dataSourceUrl = ReadStringSetting(L"dataSourceUrl");
    settings->refreshIntervalHours = Wh_GetIntSetting(L"refreshIntervalHours");
    settings->replaceText = Wh_GetIntSetting(L"replaceText") != 0;
    settings->offDayText = ReadStringSetting(L"offDayText");
    settings->workdayText = ReadStringSetting(L"workdayText");
    settings->offDayColor = ReadColorSetting(L"offDayColor", Settings{}.offDayColor);
    settings->workdayColor = ReadColorSetting(L"workdayColor", Settings{}.workdayColor);

    if (settings->refreshIntervalHours < 1) settings->refreshIntervalHours = 1;
    { std::lock_guard<std::mutex> lock(g_settingsMutex); g_settings = settings; }
}

std::wstring TrimWhitespace(std::wstring const& text) {
    size_t begin = 0, end = text.size();
    while (begin < end && iswspace(text[begin])) begin++;
    while (end > begin && iswspace(text[end - 1])) end--;
    return text.substr(begin, end - begin);
}

std::wstring Utf8ToWide(const char* data, size_t length) {
    if (!data || !length) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(length), nullptr, 0);
    if (size <= 0) return {};
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(length), result.data(), size);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Date Logic
struct HolidayInfo { std::wstring name; bool isOffDay = false; };
using HolidayMap = std::unordered_map<int32_t, HolidayInfo>;

int32_t MakeDateKey(int year, int month, int day) { return year * 10000 + month * 100 + day; }
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
int32_t ParseDateKey(std::wstring const& text) {
    std::vector<int> parts; std::wstring current;
    for (wchar_t c : text) {
        if (c >= L'0' && c <= L'9') current += c;
        else if (!current.empty()) { parts.push_back((int)wcstol(current.c_str(), nullptr, 10)); current.clear(); }
    }
    if (!current.empty()) parts.push_back((int)wcstol(current.c_str(), nullptr, 10));

    int year = 0, month = 0, day = 0;
    if (!parts.empty() && parts[0] >= 10000101) {
        int value = parts[0]; year = value / 10000; month = (value / 100) % 100; day = value % 100;
    } else if (parts.size() >= 3) { year = parts[0]; month = parts[1]; day = parts[2]; } else return 0;

    if (!IsValidDate(year, month, day)) return 0;
    return MakeDateKey(year, month, day);
}
int32_t NextDateKey(int32_t key) {
    int year = key / 10000, month = (key / 100) % 100, day = key % 100;
    int days = DaysInMonth(year, month);
    if (days == 0) return 0;
    day++;
    if (day > days) { day = 1; month++; if (month > 12) { month = 1; year++; } }
    return MakeDateKey(year, month, day);
}

std::mutex g_holidaysMutex;
HolidayMap g_holidays;
bool LookupHoliday(int32_t key, HolidayInfo& out) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    auto it = g_holidays.find(key);
    if (it == g_holidays.end()) return false;
    out = it->second; return true;
}
void StoreHolidays(HolidayMap parsed) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex); g_holidays = std::move(parsed);
}

////////////////////////////////////////////////////////////////////////////////
// iCalendar parsing
std::vector<std::wstring> SplitIcsLines(std::wstring const& text) {
    std::vector<std::wstring> lines; std::wstring current;
    auto flush = [&]() {
        if (current.empty()) return;
        if ((current[0] == L' ' || current[0] == L'\t') && !lines.empty()) lines.back() += current.substr(1);
        else lines.push_back(current);
        current.clear();
    };
    for (size_t i = 0; i < text.size(); i++) {
        wchar_t c = text[i];
        if (c == L'\r' || c == L'\n') { flush(); if (c == L'\r' && i + 1 < text.size() && text[i + 1] == L'\n') i++; }
        else current += c;
    }
    flush(); return lines;
}

std::wstring UnescapeIcsText(std::wstring const& text) {
    std::wstring result;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] != L'\\' || i + 1 >= text.size()) { result += text[i]; continue; }
        switch (text[++i]) {
            case L'n': case L'N': result += L' '; break;
            case L'\\': result += L'\\'; break;
            case L';': result += L';'; break;
            case L',': result += L','; break;
            default: result += text[i]; break;
        }
    }
    return result;
}

bool ClassifyIcsSummary(std::wstring const& summary, std::wstring& name, bool& isOffDay) {
    std::wstring text = UnescapeIcsText(summary);
    isOffDay = true;
    struct Marker { const wchar_t* text; bool isOffDay; };
    static const Marker kMarkers[] = {
        {L"（补班）", false}, {L"(补班)", false}, {L"【补班】", false}, {L"[补班]", false},
        {L"（班）", false},   {L"(班)", false},   {L"【班】", false},   {L"[班]", false},
        {L"（休）", true},    {L"(休)", true},    {L"【休】", true},    {L"[休]", true},
        {L"（放假）", true},  {L"(放假)", true},
    };
    bool marked = false;
    for (const auto& marker : kMarkers) {
        size_t pos = text.find(marker.text);
        if (pos != std::wstring::npos) { text.erase(pos, wcslen(marker.text)); isOffDay = marker.isOffDay; marked = true; break; }
    }
    if (!marked) {
        std::wstring trimmed = TrimWhitespace(text);
        if (trimmed.size() >= 2) {
            wchar_t last = trimmed[trimmed.size() - 1]; wchar_t before = trimmed[trimmed.size() - 2];
            if ((last == L'休' || last == L'班') && (iswspace(before) || before == L'-' || before == L'/' || before == L'·' || before == L'—')) {
                isOffDay = last == L'休'; marked = true; text = trimmed.substr(0, trimmed.size() - 1);
            }
        }
    }
    if (!marked) return false;
    name = TrimWhitespace(text); return !name.empty();
}

std::atomic<bool> g_loggedRecurrence{false};
constexpr size_t kMaxHolidayDates = 20000;

bool ParseIcs(std::wstring const& text, HolidayMap& out) {
    auto lines = SplitIcsLines(text);
    bool inEvent = false, hasRecurrence = false, hitDateLimit = false, hasEnd = false;
    size_t droppedRanges = 0, added = 0; int32_t start = 0, end = 0;
    std::wstring summary;

    auto flushEvent = [&]() {
        if (!start) return;
        if (added >= kMaxHolidayDates) { hitDateLimit = true; return; }
        std::wstring name; bool isOffDay = true;
        if (!ClassifyIcsSummary(summary, name, isOffDay)) return;
        if (!hasEnd) end = NextDateKey(start);
        else if (end <= start) { droppedRanges++; return; }
        for (int32_t key = start; key && key < end; key = NextDateKey(key)) {
            if (added >= kMaxHolidayDates) { hitDateLimit = true; break; }
            HolidayInfo info; info.name = name; info.isOffDay = isOffDay;
            out[key] = std::move(info); added++;
        }
    };

    for (const auto& line : lines) {
        if (line == L"BEGIN:VEVENT") { inEvent = true; start = 0; end = 0; hasEnd = false; summary.clear(); continue; }
        if (line == L"END:VEVENT") { if (inEvent) flushEvent(); inEvent = false; continue; }
        if (!inEvent) continue;
        size_t colon = line.find(L':'); if (colon == std::wstring::npos) continue;
        std::wstring name = line.substr(0, colon); std::wstring value = line.substr(colon + 1);
        size_t semicolon = name.find(L';'); if (semicolon != std::wstring::npos) name = name.substr(0, semicolon);
        if (name == L"DTSTART") start = ParseDateKey(value);
        else if (name == L"DTEND") { end = ParseDateKey(value); hasEnd = true; }
        else if (name == L"SUMMARY") summary = value;
        else if (name == L"RRULE" || name == L"EXDATE" || name == L"RECURRENCE-ID") hasRecurrence = true;
    }
    return !out.empty();
}

bool ParseHolidayData(std::wstring const& body, HolidayMap& out) {
    if (body.find(L"BEGIN:VEVENT") == std::wstring::npos && body.find(L"BEGIN:VCALENDAR") == std::wstring::npos) return false;
    return ParseIcs(body, out);
}

////////////////////////////////////////////////////////////////////////////////
// Background fetching
void SweepDayItems();
void StartSweepTimer();
void RefreshCalendarOnUiThreads();

std::mutex g_workerThreadMutex;
std::atomic<HANDLE> g_hWorkerThread{nullptr};
std::atomic<HANDLE> g_hWorkEvent{nullptr};
std::atomic<HANDLE> g_hStopEvent{nullptr};
std::atomic<bool> g_fetchPending{false};
std::atomic<bool> g_shuttingDown{false};
std::mutex g_fetchStateMutex;
ULONGLONG g_lastFetchAttemptTick = 0;
bool g_haveData = false;
constexpr ULONGLONG kFetchRetryDelayMs = 10 * 60 * 1000;

bool FetchHolidayData() {
    const std::shared_ptr<const Settings> settings = GetSettings();
    { std::lock_guard<std::mutex> lock(g_fetchStateMutex); g_lastFetchAttemptTick = GetTickCount64(); }
    if (settings->dataSourceUrl.empty()) return false;
    const WH_URL_CONTENT* content = Wh_GetUrlContent(settings->dataSourceUrl.c_str(), nullptr);
    if (!content) return false;
    std::wstring body;
    bool ok = content->statusCode == 200 && content->data && content->length;
    if (ok) body = Utf8ToWide(content->data, content->length);
    Wh_FreeUrlContent(content);
    if (!ok || body.empty()) return false;
    HolidayMap parsed;
    if (!ParseHolidayData(body, parsed)) return false;
    StoreHolidays(std::move(parsed));
    { std::lock_guard<std::mutex> lock(g_fetchStateMutex); g_haveData = true; }
    return true;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    while (true) {
        HANDLE stopEvent = g_hStopEvent.load(); HANDLE workEvent = g_hWorkEvent.load();
        if (!stopEvent || !workEvent) break;
        HANDLE events[2] = {stopEvent, workEvent};
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0 || result != WAIT_OBJECT_0 + 1) break;
        g_fetchPending = false;
        if (FetchHolidayData()) RefreshCalendarOnUiThreads();
    }
    return 0;
}

void StartWorkerThread() {
    std::lock_guard<std::mutex> lock(g_workerThreadMutex);
    if (g_hWorkerThread.load() || g_shuttingDown) return;
    HANDLE workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!workEvent || !stopEvent) { if (workEvent) CloseHandle(workEvent); if (stopEvent) CloseHandle(stopEvent); return; }
    g_hWorkEvent.store(workEvent); g_hStopEvent.store(stopEvent);
    HANDLE thread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!thread) { g_hWorkEvent.store(nullptr); g_hStopEvent.store(nullptr); CloseHandle(workEvent); CloseHandle(stopEvent); return; }
    g_hWorkerThread.store(thread);
}

void StopWorkerThread() {
    HANDLE workerThread, workEvent, stopEvent;
    {
        std::lock_guard<std::mutex> lock(g_workerThreadMutex);
        workerThread = g_hWorkerThread.exchange(nullptr);
        workEvent = g_hWorkEvent.exchange(nullptr);
        stopEvent = g_hStopEvent.exchange(nullptr);
        if (stopEvent) SetEvent(stopEvent);
    }
    if (workerThread) { WaitForSingleObject(workerThread, INFINITE); CloseHandle(workerThread); }
    if (workEvent) CloseHandle(workEvent);
    if (stopEvent) CloseHandle(stopEvent);
}

void RequestHolidayData() {
    const std::shared_ptr<const Settings> settings = GetSettings();
    if (g_shuttingDown || settings->dataSourceUrl.empty()) return;
    if (!g_hWorkerThread.load()) StartWorkerThread();
    const ULONGLONG refreshInterval = static_cast<ULONGLONG>(settings->refreshIntervalHours) * 60ULL * 60ULL * 1000ULL;
    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        if (g_lastFetchAttemptTick) {
            ULONGLONG interval = g_haveData ? refreshInterval : kFetchRetryDelayMs;
            if (GetTickCount64() - g_lastFetchAttemptTick < interval) return;
        }
    }
    if (g_fetchPending.exchange(true)) return;
    if (HANDLE workEvent = g_hWorkEvent.load()) { if (SetEvent(workEvent)) return; }
    { std::lock_guard<std::mutex> lock(g_fetchStateMutex); g_lastFetchAttemptTick = GetTickCount64(); }
    g_fetchPending = false;
}

void ResetHolidayData() {
    { std::lock_guard<std::mutex> lock(g_fetchStateMutex); g_lastFetchAttemptTick = 0; g_haveData = false; }
    { std::lock_guard<std::mutex> lock(g_holidaysMutex); g_holidays.clear(); }
}

////////////////////////////////////////////////////////////////////////////////
// UI Logic & Vietnamese Lunar Conversion
int32_t DateKeyFromDateTime(wf::DateTime const& dateTime) {
    ULARGE_INTEGER uli; uli.QuadPart = static_cast<ULONGLONG>(dateTime.time_since_epoch().count());
    FILETIME fileTime; fileTime.dwLowDateTime = uli.LowPart; fileTime.dwHighDateTime = uli.HighPart;
    SYSTEMTIME systemTimeUtc{};
    if (!FileTimeToSystemTime(&fileTime, &systemTimeUtc)) return 0;
    SYSTEMTIME systemTimeLocal{};
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &systemTimeUtc, &systemTimeLocal)) return 0;
    auto ticksOf = [](SYSTEMTIME const& systemTime, ULONGLONG& ticks) -> bool {
        FILETIME fileTimeOfSystemTime{}; if (!SystemTimeToFileTime(&systemTime, &fileTimeOfSystemTime)) return false;
        ULARGE_INTEGER value; value.LowPart = fileTimeOfSystemTime.dwLowDateTime; value.HighPart = fileTimeOfSystemTime.dwHighDateTime;
        ticks = value.QuadPart; return true;
    };
    ULONGLONG localTicks = 0, utcTicks = 0;
    if (!ticksOf(systemTimeLocal, localTicks) || !ticksOf(systemTimeUtc, utcTicks)) return 0;
    const SYSTEMTIME& date = localTicks < utcTicks ? systemTimeUtc : systemTimeLocal;
    return MakeDateKey(date.wYear, date.wMonth, date.wDay);
}

// --- [BẮT ĐẦU: LOGIC ÂM LỊCH & TỬ VI VẠN NIÊN] ---
#define FORMATTED_BUFFER_SIZE 256

template <size_t N>
struct FormattedString {
    DWORD formatIndex = 0;
    WCHAR buffer[N]{};
};

DWORD g_formatIndex = 1;
FormattedString<FORMATTED_BUFFER_SIZE> g_lunarFullFormatted;
FormattedString<FORMATTED_BUFFER_SIZE> g_lunarCanChiOnly;
FormattedString<FORMATTED_BUFFER_SIZE> g_lunarDateOnly;
FormattedString<FORMATTED_BUFFER_SIZE> g_lunarHoroscopeShort;
FormattedString<2048> g_lunarHoroscopeFull;

const std::wstring CAN[] = { L"Canh", L"Tân", L"Nhâm", L"Quý", L"Giáp", L"Ất", L"Bính", L"Đinh", L"Mậu", L"Kỷ" };
const std::wstring CHI[] = { L"Thân", L"Dậu", L"Tuất", L"Hợi", L"Tý", L"Sửu", L"Dần", L"Mão", L"Thìn", L"Tỵ", L"Ngọ", L"Mùi" };

const PCWSTR CAN_10[] = { L"Giáp", L"Ất", L"Bính", L"Đinh", L"Mậu", L"Kỷ", L"Canh", L"Tân", L"Nhâm", L"Quý" };
const PCWSTR CHI_12[] = { L"Tý", L"Sửu", L"Dần", L"Mão", L"Thìn", L"Tỵ", L"Ngọ", L"Mùi", L"Thân", L"Dậu", L"Tuất", L"Hợi" };

std::wstring GetCanChiYear(int year) {
    return CAN[year % 10] + L" " + CHI[year % 12];
}

const int LEAP_MONTHS_100_YEARS[] = {
    // 2000 - 2009
    0, 4, 0, 0, 2, 0, 7, 0, 0, 5,
    // 2010 - 2019
    0, 0, 4, 0, 9, 0, 0, 6, 0, 0,
    // 2020 - 2029
    4, 0, 0, 2, 0, 6, 0, 0, 5, 0,
    // 2030 - 2039
    0, 3, 0, 11, 0, 0, 6, 0, 0, 5,
    // 2040 - 2049
    0, 0, 2, 0, 7, 0, 0, 5, 0, 0,
    // 2050 - 2059
    3, 0, 8, 0, 0, 6, 0, 0, 4, 0,
    // 2060 - 2069
    0, 3, 0, 7, 0, 0, 5, 0, 0, 4,
    // 2070 - 2079
    0, 8, 0, 0, 6, 0, 0, 4, 0, 0,
    // 2080 - 2089
    3, 0, 7, 0, 0, 5, 0, 0, 4, 0,
    // 2090 - 2099
    8, 0, 0, 6, 0, 0, 4, 0, 0, 3
};

int GetLeapMonthIndex(int lunarYear) {
    if (lunarYear < 2000 || lunarYear > 2099) {
        return 0;
    }
    return LEAP_MONTHS_100_YEARS[lunarYear - 2000];
}

struct LunarDateCache {
    WORD sYear = 0, sMonth = 0, sDay = 0;
    int lDay = 1, lMonth = 1, lYear = 2000;
    bool isLeap = false;
    std::wstring canChi;
} g_lunarCache;

void GetLunarDateWinRT(int& lDay, int& lMonth, int& lYear, bool& isLeap, std::wstring& canChi) {
    SYSTEMTIME st;
    GetLocalTime(&st);

    if (g_lunarCache.sYear == st.wYear &&
        g_lunarCache.sMonth == st.wMonth &&
        g_lunarCache.sDay == st.wDay &&
        !g_lunarCache.canChi.empty()) {
        lDay = g_lunarCache.lDay;
        lMonth = g_lunarCache.lMonth;
        lYear = g_lunarCache.lYear;
        isLeap = g_lunarCache.isLeap;
        canChi = g_lunarCache.canChi;
        return;
    }

    try {
        winrt::Windows::Globalization::Calendar cal;
        cal.ChangeCalendarSystem(winrt::Windows::Globalization::CalendarIdentifiers::ChineseLunar());
        cal.SetToNow();

        lDay = cal.Day();
        int rawMonth = cal.Month();

        lYear = st.wYear;
        if (rawMonth >= 10 && st.wMonth <= 2) {
            lYear--;
        }

        canChi = GetCanChiYear(lYear);
        int leapMonth = GetLeapMonthIndex(lYear);
        isLeap = false;

        if (leapMonth > 0) {
            if (rawMonth == leapMonth + 1) {
                lMonth = leapMonth;
                isLeap = true;
            } else if (rawMonth > leapMonth + 1) {
                lMonth = rawMonth - 1;
            } else {
                lMonth = rawMonth;
            }
        } else {
            lMonth = rawMonth;
        }

        g_lunarCache = { st.wYear, st.wMonth, st.wDay, lDay, lMonth, lYear, isLeap, canChi };
    } catch (...) {
        lDay = st.wDay; lMonth = st.wMonth; lYear = st.wYear; isLeap = false;
        canChi = L"";
    }
}

thread_local winrt::Windows::Globalization::Calendar t_gregorianCal{nullptr};
thread_local winrt::Windows::Globalization::Calendar t_lunarCal{nullptr};

void GetLunarDateWinRT_ForDate(int sYear, int sMonth, int sDay, int& lDay, int& lMonth, int& lYear, bool& isLeap, std::wstring& canChi) {
    try {
        if (!t_gregorianCal) {
            t_gregorianCal = winrt::Windows::Globalization::Calendar();
            t_gregorianCal.ChangeCalendarSystem(winrt::Windows::Globalization::CalendarIdentifiers::Gregorian());
        }
        if (!t_lunarCal) {
            t_lunarCal = winrt::Windows::Globalization::Calendar();
            t_lunarCal.ChangeCalendarSystem(winrt::Windows::Globalization::CalendarIdentifiers::ChineseLunar());
        }

        t_gregorianCal.Year(sYear);
        t_gregorianCal.Month(sMonth);
        t_gregorianCal.Day(sDay);
        t_lunarCal.SetDateTime(t_gregorianCal.GetDateTime());

        lDay = t_lunarCal.Day();
        int rawMonth = t_lunarCal.Month();

        lYear = sYear;
        if (rawMonth >= 10 && sMonth <= 2) {
            lYear--;
        }

        canChi = GetCanChiYear(lYear);
        int leapMonth = GetLeapMonthIndex(lYear);
        isLeap = false;

        if (leapMonth > 0) {
            if (rawMonth == leapMonth + 1) {
                lMonth = leapMonth;
                isLeap = true;
            } else if (rawMonth > leapMonth + 1) {
                lMonth = rawMonth - 1;
            } else {
                lMonth = rawMonth;
            }
        } else {
            lMonth = rawMonth;
        }
    } catch (...) {
        lDay = sDay; lMonth = sMonth; lYear = sYear; isLeap = false; canChi = L"";
    }
}

int CalculateJDN(int sDay, int sMonth, int sYear) {
    int a = (14 - sMonth) / 12;
    int y = sYear + 4800 - a;
    int m = sMonth + 12 * a - 3;
    return sDay + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

std::wstring GetVietnameseHolidayName(int sDay, int sMonth, int lDay, int lMonth, bool isLeap) {
    std::wstring names;
    auto addName = [&](PCWSTR n) {
        if (!names.empty()) names += L" • ";
        names += n;
    };

    if (sDay == 1 && sMonth == 1) addName(L"Tết Dương lịch");
    else if (sDay == 14 && sMonth == 2) addName(L"Lễ Tình nhân (Valentine)");
    else if (sDay == 8 && sMonth == 3) addName(L"Quốc tế Phụ nữ");
    else if (sDay == 26 && sMonth == 3) addName(L"Thành lập Đoàn TNCS HCM");
    else if (sDay == 30 && sMonth == 4) addName(L"Ngày Giải phóng miền Nam");
    else if (sDay == 1 && sMonth == 5) addName(L"Quốc tế Lao động");
    else if (sDay == 1 && sMonth == 6) addName(L"Quốc tế Thiếu nhi");
    else if (sDay == 2 && sMonth == 9) addName(L"Quốc khánh Việt Nam");
    else if (sDay == 20 && sMonth == 10) addName(L"Ngày Phụ nữ Việt Nam");
    else if (sDay == 20 && sMonth == 11) addName(L"Ngày Nhà giáo Việt Nam");
    else if (sDay == 24 && sMonth == 11) addName(L"Ngày Văn hóa Việt Nam");
    else if (sDay == 22 && sMonth == 12) addName(L"Thành lập QĐND Việt Nam");
    else if (sDay == 24 && sMonth == 12) addName(L"Đêm Giáng Sinh");
    else if (sDay == 25 && sMonth == 12) addName(L"Lễ Giáng Sinh (Noel)");

    if (!isLeap) {
        if (lDay == 1 && lMonth == 1) addName(L"Mùng 1 Tết Nguyên Đán");
        else if (lDay == 2 && lMonth == 1) addName(L"Mùng 2 Tết Nguyên Đán");
        else if (lDay == 3 && lMonth == 1) addName(L"Mùng 3 Tết Nguyên Đán");
        else if (lDay == 15 && lMonth == 1) addName(L"Tết Nguyên Tiêu (Rằm tháng Giêng)");
        else if (lDay == 3 && lMonth == 3) addName(L"Tết Hàn Thực");
        else if (lDay == 10 && lMonth == 3) addName(L"Giỗ Tổ Hùng Vương");
        else if (lDay == 15 && lMonth == 4) addName(L"Lễ Phật Đản");
        else if (lDay == 5 && lMonth == 5) addName(L"Tết Đoan Ngọ");
        else if (lDay == 15 && lMonth == 7) addName(L"Lễ Vu Lan (Rằm tháng Bảy)");
        else if (lDay == 15 && lMonth == 8) addName(L"Tết Trung Thu (Rằm tháng Tám)");
        else if (lDay == 23 && lMonth == 12) addName(L"Tiễn Ông Công Ông Táo");
        else if ((lDay == 29 || lDay == 30) && lMonth == 12) addName(L"Tất niên / Giao thừa");
        else if (lDay == 1) addName(L"Ngày Mùng 1 (Sóc)");
        else if (lDay == 15) addName(L"Ngày Rằm (Vọng)");
    }

    return names;
}

// Hiển thị trên Taskbar: "05/09 Bính Ngọ"
PCWSTR GetLunarFullFormatted() {
    if (g_lunarFullFormatted.formatIndex != g_formatIndex) {
        int lDay, lMonth, lYear;
        bool isLeap;
        std::wstring canChi;
        GetLunarDateWinRT(lDay, lMonth, lYear, isLeap, canChi);

        if (isLeap) {
            swprintf_s(g_lunarFullFormatted.buffer, L"%02d/%02d (Nhuận) %s", lDay, lMonth, canChi.c_str());
        } else {
            swprintf_s(g_lunarFullFormatted.buffer, L"%02d/%02d %s", lDay, lMonth, canChi.c_str());
        }
        g_lunarFullFormatted.formatIndex = g_formatIndex;
    }
    return g_lunarFullFormatted.buffer;
}

PCWSTR GetLunarCanChiOnly() {
    if (g_lunarCanChiOnly.formatIndex != g_formatIndex) {
        int lDay, lMonth, lYear;
        bool isLeap;
        std::wstring canChi;
        GetLunarDateWinRT(lDay, lMonth, lYear, isLeap, canChi);

        wcscpy_s(g_lunarCanChiOnly.buffer, canChi.c_str());
        g_lunarCanChiOnly.formatIndex = g_formatIndex;
    }
    return g_lunarCanChiOnly.buffer;
}

PCWSTR GetLunarDateOnly() {
    if (g_lunarDateOnly.formatIndex != g_formatIndex) {
        int lDay, lMonth, lYear;
        bool isLeap;
        std::wstring canChi;
        GetLunarDateWinRT(lDay, lMonth, lYear, isLeap, canChi);

        if (isLeap) {
            swprintf_s(g_lunarDateOnly.buffer, L"%02d/%02d (Nhuận)", lDay, lMonth);
        } else {
            swprintf_s(g_lunarDateOnly.buffer, L"%02d/%02d", lDay, lMonth);
        }
        g_lunarDateOnly.formatIndex = g_formatIndex;
    }
    return g_lunarDateOnly.buffer;
}

PCWSTR GetLunarHoroscopeShort() {
    if (g_lunarHoroscopeShort.formatIndex != g_formatIndex) {
        SYSTEMTIME st;
        GetLocalTime(&st);

        int lDay, lMonth, lYear;
        bool isLeap;
        std::wstring yearCanChi;
        GetLunarDateWinRT(lDay, lMonth, lYear, isLeap, yearCanChi);

        int jdn = CalculateJDN(st.wDay, st.wMonth, st.wYear);
        int canDayIdx = (jdn + 9) % 10;
        int chiDayIdx = (jdn + 1) % 12;

        static const PCWSTR kStarsShort[12] = {
            L"Thanh Long Hoàng Đạo", L"Minh Đường Hoàng Đạo", L"Thiên Hình Hắc Đạo", L"Chu Tước Hắc Đạo",
            L"Kim Quỹ Hoàng Đạo",    L"Kim Đường Hoàng Đạo",  L"Bạch Hổ Hắc Đạo",    L"Ngọc Đường Hoàng Đạo",
            L"Thiên Lao Hắc Đạo",    L"Nguyên Vũ Hắc Đạo",    L"Tư Mệnh Hoàng Đạo",  L"Câu Trận Hắc Đạo"
        };
        int thanhLongStartChi = ((lMonth - 1) % 6) * 2;
        int starIdx = (chiDayIdx - thanhLongStartChi + 12) % 12;

        swprintf_s(g_lunarHoroscopeShort.buffer, L"Ngày %s %s • %s",
                   CAN_10[canDayIdx], CHI_12[chiDayIdx], kStarsShort[starIdx]);
        g_lunarHoroscopeShort.formatIndex = g_formatIndex;
    }
    return g_lunarHoroscopeShort.buffer;
}

// Nội dung Âm lịch + Can Chi + Tử Vi hiển thị trong hộp Tooltip khi rê chuột vào đồng hồ Taskbar
PCWSTR GetLunarHoroscopeFull() {
    if (g_lunarHoroscopeFull.formatIndex != g_formatIndex) {
        SYSTEMTIME st;
        GetLocalTime(&st);

        int lDay, lMonth, lYear;
        bool isLeap;
        std::wstring yearCanChi;
        GetLunarDateWinRT(lDay, lMonth, lYear, isLeap, yearCanChi);

        int jdn = CalculateJDN(st.wDay, st.wMonth, st.wYear);

        int canDayIdx = (jdn + 9) % 10;
        int chiDayIdx = (jdn + 1) % 12;
        std::wstring dayCanChi = std::wstring(CAN_10[canDayIdx]) + L" " + CHI_12[chiDayIdx];

        int canMonthIdx = (lYear * 12 + lMonth + 3) % 10;
        int chiMonthIdx = (lMonth + 1) % 12;
        std::wstring monthCanChi = std::wstring(CAN_10[canMonthIdx]) + L" " + CHI_12[chiMonthIdx];

        int chiHourIdx = ((st.wHour + 1) / 2) % 12;
        int canHourIdx = ((canDayIdx % 5) * 2 + chiHourIdx) % 10;
        std::wstring hourCanChi = std::wstring(CAN_10[canHourIdx]) + L" " + CHI_12[chiHourIdx];

        static const PCWSTR kNapAm[30] = {
            L"Hải Trung Kim (Vàng trong biển)",       L"Lô Trung Hỏa (Lửa trong lò)",
            L"Đại Lâm Mộc (Gỗ rừng già)",             L"Lộ Bàng Thổ (Đất ven đường)",
            L"Kiếm Phong Kim (Vàng mũi kiếm)",        L"Sơn Đầu Hỏa (Lửa đầu núi)",
            L"Giản Hạ Thủy (Nước dưới khe)",          L"Thành Đầu Thổ (Đất tường thành)",
            L"Bạch Lạp Kim (Vàng chân đèn)",          L"Dương Liễu Mộc (Gỗ cây liễu)",
            L"Tuyền Trung Thủy (Nước trong suối)",    L"Ốc Thượng Thổ (Đất mái nhà)",
            L"Tích Lịch Hỏa (Lửa sấm sét)",           L"Tùng Bách Mộc (Gỗ tùng bách)",
            L"Trường Lưu Thủy (Nước chảy dài)",       L"Sa Trung Kim (Vàng trong cát)",
            L"Sơn Hạ Hỏa (Lửa dưới núi)",             L"Bình Địa Mộc (Gỗ đồng bằng)",
            L"Bích Thượng Thổ (Đất tò vò)",           L"Kim Bạch Kim (Vàng pha bạc)",
            L"Phúc Đăng Hỏa (Lửa đèn chụp)",          L"Thiên Hà Thủy (Nước ngân hà)",
            L"Đại Trạch Thổ (Đất nền nhà)",           L"Thoa Xuyến Kim (Vàng trang sức)",
            L"Tang Đố Mộc (Gỗ cây dâu)",              L"Đại Khê Thủy (Nước khe lớn)",
            L"Sa Trung Thổ (Đất pha cát)",            L"Thiên Thượng Hỏa (Lửa trên trời)",
            L"Thạch Lựu Mộc (Gỗ cây lựu)",            L"Đại Hải Thủy (Nước biển lớn)"
        };
        int hoaGiapIdx = (6 * canDayIdx - 5 * chiDayIdx + 60) % 60;
        PCWSTR napAmStr = kNapAm[hoaGiapIdx / 2];

        struct StarInfo { PCWSTR name; bool isHoangDao; };
        static const StarInfo kStars[12] = {
            { L"Thanh Long Hoàng Đạo", true },  { L"Minh Đường Hoàng Đạo", true },
            { L"Thiên Hình Hắc Đạo", false },   { L"Chu Tước Hắc Đạo", false },
            { L"Kim Quỹ Hoàng Đạo", true },     { L"Kim Đường Hoàng Đạo", true },
            { L"Bạch Hổ Hắc Đạo", false },      { L"Ngọc Đường Hoàng Đạo", true },
            { L"Thiên Lao Hắc Đạo", false },    { L"Nguyên Vũ Hắc Đạo", false },
            { L"Tư Mệnh Hoàng Đạo", true },     { L"Câu Trận Hắc Đạo", false }
        };
        int thanhLongStartChi = ((lMonth - 1) % 6) * 2;
        int starIdx = (chiDayIdx - thanhLongStartChi + 12) % 12;
        const StarInfo& star = kStars[starIdx];

        struct TrucInfo { PCWSTR name; PCWSTR goodFor; PCWSTR badFor; };
        static const TrucInfo kTruc[12] = {
            { L"Kiến",  L"Xuất hành, nhậm chức, cầu tài, ký kết",         L"Động thổ, đào đất, mở kho" },
            { L"Trừ",   L"Thanh tẩy, chữa bệnh, dọn dẹp, cầu bình an",    L"Khai trương lớn, đi xa" },
            { L"Mãn",   L"Cúng tế, cầu tài lộc, khai trương, mở kho",      L"Kiện tụng, tranh chấp" },
            { L"Bình",  L"Tu sửa, bàn bạc công việc, giao dịch, cưới hỏi", L"Khởi công lớn, thưa kiện" },
            { L"Định",  L"Ký hợp đồng, cầu danh, nhập học, mưu sự",        L"Xuất hành xa, kiện tụng" },
            { L"Chấp",  L"Xây dựng, tu tạo, lập khế ước, tuyển dụng",     L"Xuất tiền lớn, dọn nhà" },
            { L"Phá",   L"Phá dỡ, giải quyết việc tồn đọng, chữa bệnh",    L"Khai trương, cưới hỏi, ký kết" },
            { L"Nguy",  L"Tế tự, cầu an, nghỉ ngơi, cẩn trọng đi lại",     L"Mạo hiểm, khởi công, đi sông nước" },
            { L"Thành", L"Khai trương, nhập học, cưới hỏi, cầu tài lộc",   L"Kiện tụng, tranh cãi" },
            { L"Thu",   L"Thu hoạch, tích trữ, đòi nợ, cất giữ tài sản",   L"Khởi công mới, xuất hành xa" },
            { L"Khai",  L"Khai trương, động thổ, xuất hành, nhậm chức",    L"An táng, tranh chấp" },
            { L"Bế",    L"Đắp đê, tu bổ, cất giữ tài vật, lập kế hoạch",   L"Khai trương, xuất hành xa" }
        };
        int trucIdx = (chiDayIdx - chiMonthIdx + 12) % 12;
        const TrucInfo& truc = kTruc[trucIdx];

        static const PCWSTR kGioHoangDao[6] = {
            L"Tý (23-1h), Sửu (1-3h), Mão (5-7h), Ngọ (11-13h), Thân (15-17h), Dậu (17-19h)",
            L"Dần (3-5h), Mão (5-7h), Tỵ (9-11h), Thân (15-17h), Tuất (19-21h), Hợi (21-23h)",
            L"Tý (23-1h), Sửu (1-3h), Thìn (7-9h), Tỵ (9-11h), Mùi (13-15h), Tuất (19-21h)",
            L"Tý (23-1h), Dần (3-5h), Mão (5-7h), Ngọ (11-13h), Mùi (13-15h), Dậu (17-19h)",
            L"Dần (3-5h), Thìn (7-9h), Tỵ (9-11h), Thân (15-17h), Dậu (17-19h), Hợi (21-23h)",
            L"Sửu (1-3h), Thìn (7-9h), Ngọ (11-13h), Mùi (13-15h), Tuất (19-21h), Hợi (21-23h)"
        };
        PCWSTR gioTotStr = kGioHoangDao[chiDayIdx % 6];

        int xungChiIdx = (chiDayIdx + 6) % 12;
        int xungCan1 = (canDayIdx + 4) % 10;
        int xungCan2 = (canDayIdx + 6) % 10;
        std::wstring tuoiXung = std::wstring(CAN_10[xungCan1]) + L" " + CHI_12[xungChiIdx] +
                                L", " + CAN_10[xungCan2] + L" " + CHI_12[xungChiIdx];

        static const PCWSTR kHuongHyThan[10] = {
            L"Đông Bắc", L"Tây Bắc", L"Tây Nam", L"Chính Nam", L"Đông Nam",
            L"Đông Bắc", L"Tây Bắc", L"Tây Nam", L"Chính Nam", L"Đông Nam"
        };
        static const PCWSTR kHuongTaiThan[10] = {
            L"Đông Nam", L"Đông Nam", L"Chính Đông", L"Chính Đông", L"Chính Bắc",
            L"Chính Nam", L"Tây Nam", L"Tây Nam", L"Chính Tây", L"Tây Bắc"
        };

        std::wstring holidayStr = GetVietnameseHolidayName(st.wDay, st.wMonth, lDay, lMonth, isLeap);
        std::wstring holidayLine = holidayStr.empty() ? L"" : (L"\r\n🎉 Sự kiện: " + holidayStr);

        wchar_t lunarHeader[256];
        if (isLeap) {
            swprintf_s(lunarHeader, L"📅 Âm lịch: Ngày %02d Tháng %02d (Nhuận) Năm %s", lDay, lMonth, yearCanChi.c_str());
        } else {
            swprintf_s(lunarHeader, L"📅 Âm lịch: Ngày %02d Tháng %02d Năm %s (%02d/%02d/%d)", lDay, lMonth, yearCanChi.c_str(), lDay, lMonth, lYear);
        }

        swprintf_s(
            g_lunarHoroscopeFull.buffer,
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
            hourCanChi.c_str(),
            dayCanChi.c_str(),
            monthCanChi.c_str(),
            yearCanChi.c_str(),
            napAmStr,
            star.isHoangDao ? L"🌟" : L"⚪",
            star.name,
            star.isHoangDao ? L"Ngày Tốt" : L"Ngày Hắc Đạo",
            truc.name,
            gioTotStr,
            truc.goodFor,
            truc.badFor,
            tuoiXung.c_str(),
            kHuongHyThan[canDayIdx],
            kHuongTaiThan[canDayIdx]);

        g_lunarHoroscopeFull.formatIndex = g_formatIndex;
    }
    return g_lunarHoroscopeFull.buffer;
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

HoroscopePageData BuildHoroscopePageData(int32_t key) {
    HoroscopePageData out;
    int sYear = key / 10000;
    int sMonth = (key / 100) % 100;
    int sDay = key % 100;

    int lDay, lMonth, lYear;
    bool isLeap;
    std::wstring yearCanChi;
    GetLunarDateWinRT_ForDate(sYear, sMonth, sDay, lDay, lMonth, lYear, isLeap, yearCanChi);

    int jdn = CalculateJDN(sDay, sMonth, sYear);

    static const PCWSTR kWeekdaysFull[] = {
        L"Thứ Hai", L"Thứ Ba", L"Thứ Tư", L"Thứ Năm", L"Thứ Sáu", L"Thứ Bảy", L"Chủ Nhật"
    };
    PCWSTR weekdayStr = kWeekdaysFull[jdn % 7];

    int canDayIdx = (jdn + 9) % 10;
    int chiDayIdx = (jdn + 1) % 12;
    std::wstring dayCanChi = std::wstring(CAN_10[canDayIdx]) + L" " + CHI_12[chiDayIdx];

    int canMonthIdx = (lYear * 12 + lMonth + 3) % 10;
    int chiMonthIdx = (lMonth + 1) % 12;
    std::wstring monthCanChi = std::wstring(CAN_10[canMonthIdx]) + L" " + CHI_12[chiMonthIdx];

    static const PCWSTR kNapAm[30] = {
        L"Hải Trung Kim (Vàng trong biển)",       L"Lô Trung Hỏa (Lửa trong lò)",
        L"Đại Lâm Mộc (Gỗ rừng già)",             L"Lộ Bàng Thổ (Đất ven đường)",
        L"Kiếm Phong Kim (Vàng mũi kiếm)",        L"Sơn Đầu Hỏa (Lửa đầu núi)",
        L"Giản Hạ Thủy (Nước dưới khe)",          L"Thành Đầu Thổ (Đất tường thành)",
        L"Bạch Lạp Kim (Vàng chân đèn)",          L"Dương Liễu Mộc (Gỗ cây liễu)",
        L"Tuyền Trung Thủy (Nước trong suối)",    L"Ốc Thượng Thổ (Đất mái nhà)",
        L"Tích Lịch Hỏa (Lửa sấm sét)",           L"Tùng Bách Mộc (Gỗ tùng bách)",
        L"Trường Lưu Thủy (Nước chảy dài)",       L"Sa Trung Kim (Vàng trong cát)",
        L"Sơn Hạ Hỏa (Lửa dưới núi)",             L"Bình Địa Mộc (Gỗ đồng bằng)",
        L"Bích Thượng Thổ (Đất tò vò)",           L"Kim Bạch Kim (Vàng pha bạc)",
        L"Phúc Đăng Hỏa (Lửa đèn chụp)",          L"Thiên Hà Thủy (Nước ngân hà)",
        L"Đại Trạch Thổ (Đất nền nhà)",           L"Thoa Xuyến Kim (Vàng trang sức)",
        L"Tang Đố Mộc (Gỗ cây dâu)",              L"Đại Khê Thủy (Nước khe lớn)",
        L"Sa Trung Thổ (Đất pha cát)",            L"Thiên Thượng Hỏa (Lửa trên trời)",
        L"Thạch Lựu Mộc (Gỗ cây lựu)",            L"Đại Hải Thủy (Nước biển lớn)"
    };
    int hoaGiapIdx = (6 * canDayIdx - 5 * chiDayIdx + 60) % 60;
    PCWSTR napAmStr = kNapAm[hoaGiapIdx / 2];

    struct StarInfo { PCWSTR name; bool isHoangDao; };
    static const StarInfo kStars[12] = {
        { L"Thanh Long Hoàng Đạo", true },  { L"Minh Đường Hoàng Đạo", true },
        { L"Thiên Hình Hắc Đạo", false },   { L"Chu Tước Hắc Đạo", false },
        { L"Kim Quỹ Hoàng Đạo", true },     { L"Kim Đường Hoàng Đạo", true },
        { L"Bạch Hổ Hắc Đạo", false },      { L"Ngọc Đường Hoàng Đạo", true },
        { L"Thiên Lao Hắc Đạo", false },    { L"Nguyên Vũ Hắc Đạo", false },
        { L"Tư Mệnh Hoàng Đạo", true },     { L"Câu Trận Hắc Đạo", false }
    };
    int thanhLongStartChi = ((lMonth - 1) % 6) * 2;
    int starIdx = (chiDayIdx - thanhLongStartChi + 12) % 12;
    const StarInfo& star = kStars[starIdx];

    struct TrucInfo { PCWSTR name; PCWSTR goodFor; PCWSTR badFor; };
    static const TrucInfo kTruc[12] = {
        { L"Kiến",  L"Xuất hành, nhậm chức, cầu tài, ký kết",         L"Động thổ, đào đất, mở kho" },
        { L"Trừ",   L"Thanh tẩy, chữa bệnh, dọn dẹp, cầu bình an",    L"Khai trương lớn, đi xa" },
        { L"Mãn",   L"Cúng tế, cầu tài lộc, khai trương, mở kho",      L"Kiện tụng, tranh chấp" },
        { L"Bình",  L"Tu sửa, bàn bạc công việc, giao dịch, cưới hỏi", L"Khởi công lớn, thưa kiện" },
        { L"Định",  L"Ký hợp đồng, cầu danh, nhập học, mưu sự",        L"Xuất hành xa, kiện tụng" },
        { L"Chấp",  L"Xây dựng, tu tạo, lập khế ước, tuyển dụng",     L"Xuất tiền lớn, dọn nhà" },
        { L"Phá",   L"Phá dỡ, giải quyết việc tồn đọng, chữa bệnh",    L"Khai trương, cưới hỏi, ký kết" },
        { L"Nguy",  L"Tế tự, cầu an, nghỉ ngơi, cẩn trọng đi lại",     L"Mạo hiểm, khởi công, đi sông nước" },
        { L"Thành", L"Khai trương, nhập học, cưới hỏi, cầu tài lộc",   L"Kiện tụng, tranh cãi" },
        { L"Thu",   L"Thu hoạch, tích trữ, đòi nợ, cất giữ tài sản",   L"Khởi công mới, xuất hành xa" },
        { L"Khai",  L"Khai trương, động thổ, xuất hành, nhậm chức",    L"An táng, tranh chấp" },
        { L"Bế",    L"Đắp đê, tu bổ, cất giữ tài vật, lập kế hoạch",   L"Khai trương, xuất hành xa" }
    };
    int trucIdx = (chiDayIdx - chiMonthIdx + 12) % 12;
    const TrucInfo& truc = kTruc[trucIdx];

    static const PCWSTR kGioHoangDao[6] = {
        L"Tý (23-1h), Sửu (1-3h), Mão (5-7h), Ngọ (11-13h), Thân (15-17h), Dậu (17-19h)",
        L"Dần (3-5h), Mão (5-7h), Tỵ (9-11h), Thân (15-17h), Tuất (19-21h), Hợi (21-23h)",
        L"Tý (23-1h), Sửu (1-3h), Thìn (7-9h), Tỵ (9-11h), Mùi (13-15h), Tuất (19-21h)",
        L"Tý (23-1h), Dần (3-5h), Mão (5-7h), Ngọ (11-13h), Mùi (13-15h), Dậu (17-19h)",
        L"Dần (3-5h), Thìn (7-9h), Tỵ (9-11h), Thân (15-17h), Dậu (17-19h), Hợi (21-23h)",
        L"Sửu (1-3h), Thìn (7-9h), Ngọ (11-13h), Mùi (13-15h), Tuất (19-21h), Hợi (21-23h)"
    };
    PCWSTR gioTotStr = kGioHoangDao[chiDayIdx % 6];

    int xungChiIdx = (chiDayIdx + 6) % 12;
    int xungCan1 = (canDayIdx + 4) % 10;
    int xungCan2 = (canDayIdx + 6) % 10;
    std::wstring tuoiXung = std::wstring(CAN_10[xungCan1]) + L" " + CHI_12[xungChiIdx] +
                            L", " + CAN_10[xungCan2] + L" " + CHI_12[xungChiIdx];

    static const PCWSTR kHuongHyThan[10] = {
        L"Đông Bắc", L"Tây Bắc", L"Tây Nam", L"Chính Nam", L"Đông Nam",
        L"Đông Bắc", L"Tây Bắc", L"Tây Nam", L"Chính Nam", L"Đông Nam"
    };
    static const PCWSTR kHuongTaiThan[10] = {
        L"Đông Nam", L"Đông Nam", L"Chính Đông", L"Chính Đông", L"Chính Bắc",
        L"Chính Nam", L"Tây Nam", L"Tây Nam", L"Chính Tây", L"Tây Bắc"
    };

    wchar_t buf[512];
    swprintf_s(buf, L"%s, %02d/%02d/%d", weekdayStr, sDay, sMonth, sYear);
    out.headerSolar = buf;

    std::wstring specialName = GetVietnameseHolidayName(sDay, sMonth, lDay, lMonth, isLeap);
    HolidayInfo icsInfo;
    if (LookupHoliday(key, icsInfo) && !icsInfo.name.empty()) {
        if (!specialName.empty()) specialName += L" • ";
        specialName += icsInfo.name;
    }

    if (isLeap) {
        swprintf_s(buf, L"Âm lịch: %02d/%02d (Nhuận) Năm %s", lDay, lMonth, yearCanChi.c_str());
    } else {
        swprintf_s(buf, L"Âm lịch: %02d/%02d/%d (%s)", lDay, lMonth, lYear, yearCanChi.c_str());
    }
    out.headerLunar = buf;
    if (!specialName.empty()) {
        out.headerLunar += L"  •  🎉 " + specialName;
    }

    swprintf_s(buf, L"🐉 Ngày %s  •  Tháng %s  •  Năm %s",
               dayCanChi.c_str(), monthCanChi.c_str(), yearCanChi.c_str());
    out.lineCanChi = buf;

    swprintf_s(buf, L"☯ Mệnh: %s\n%s %s (%s)  •  Trực: %s",
               napAmStr,
               star.isHoangDao ? L"🌟" : L"⚪",
               star.name,
               star.isHoangDao ? L"Ngày Tốt" : L"Hắc Đạo",
               truc.name);
    out.lineMenhTruc = buf;

    swprintf_s(buf, L"⏰ Giờ Hoàng Đạo: %s", gioTotStr);
    out.lineGioHoangDao = buf;

    swprintf_s(buf, L"🔮 Tử vi (Nên): %s", truc.goodFor);
    out.lineTuViNen = buf;

    swprintf_s(buf, L"⛔ Việc Kỵ: %s", truc.badFor);
    out.lineTuViKy = buf;

    swprintf_s(buf, L"🧭 Tuổi xung: %s  •  Hỷ thần: %s, Tài thần: %s",
               tuoiXung.c_str(), kHuongHyThan[canDayIdx], kHuongTaiThan[canDayIdx]);
    out.lineXungHuong = buf;

    return out;
}
// --- [KẾT THÚC: LOGIC ÂM LỊCH & TỬ VI VẠN NIÊN] ---

// --- [PHÂN LOẠI MỨC ĐỘ LỄ VIỆT NAM] ---
enum class HolidayScope { None, SolarOnly, LunarOnly, Both };

HolidayScope GetVietnameseHolidayScope(int sDay, int sMonth, int lDay, int lMonth, bool isLeap) {
    HolidayScope scope = HolidayScope::None;

    if (sDay == 1 && sMonth == 1) scope = HolidayScope::Both;
    if (!isLeap) {
        if ((lDay >= 1 && lDay <= 3) && lMonth == 1) scope = HolidayScope::Both;
        if ((lDay == 29 || lDay == 30) && lMonth == 12) scope = HolidayScope::Both;
        if (lDay == 15 && (lMonth == 1 || lMonth == 7 || lMonth == 8)) scope = HolidayScope::Both;
    }
    if (scope == HolidayScope::Both) return scope;

    if ((sDay == 14 && sMonth == 2) || (sDay == 8 && sMonth == 3) ||
        (sDay == 26 && sMonth == 3) || (sDay == 30 && sMonth == 4) ||
        (sDay == 1 && sMonth == 5) || (sDay == 1 && sMonth == 6) ||
        (sDay == 2 && sMonth == 9) || (sDay == 20 && sMonth == 10) ||
        (sDay == 20 && sMonth == 11) || (sDay == 24 && sMonth == 11) ||
        (sDay == 22 && sMonth == 12) || (sDay == 24 && sMonth == 12) ||
        (sDay == 25 && sMonth == 12)) {
        return HolidayScope::SolarOnly;
    }

    if (!isLeap) {
        if ((lDay == 10 && lMonth == 3) || (lDay == 15 && lMonth == 4) ||
            (lDay == 5 && lMonth == 5) || (lDay == 23 && lMonth == 12)) {
            return HolidayScope::LunarOnly;
        }
    }

    return HolidayScope::None;
}
// -------------------------------------------------------------------------

constexpr PCWSTR kLunarTextBlockName = L"LunarTextBlock";
constexpr PCWSTR kCustomCellLunarName = L"CustomCellLunar";
constexpr PCWSTR kCustomHeaderLunarName = L"CustomHeaderLunar";
constexpr PCWSTR kCustomHoroscopePageName = L"CustomHoroscopePageOverlay";
struct FoundElement { wux::DependencyObject element{nullptr}; int depth = 0; };

void LogCalendarException(PCWSTR operation) noexcept {
    try {
        throw;
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"[VNCalendar] %s: HRESULT=0x%08X, %s", operation,
               static_cast<unsigned int>(error.code().value), error.message().c_str());
    } catch (...) {
        Wh_Log(L"[VNCalendar] %s: unknown exception", operation);
    }
}

void CollectDescendants(wux::DependencyObject const& root, std::vector<FoundElement>& out) {
    out.push_back({root, 0});
    for (size_t i = 0; i < out.size() && i < 256; i++) {
        wux::DependencyObject current = out[i].element; int depth = out[i].depth;
        if (!current || depth > 24) continue;
        int count = 0; try { count = wuxm::VisualTreeHelper::GetChildrenCount(current); } catch (...) { continue; }
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
    for (wchar_t c : text) { if (c < L'0' || c > L'9') return false; }
    return true;
}

bool DayItemHasContent(wuxc::CalendarViewDayItem const& item) {
    try { return wuxm::VisualTreeHelper::GetChildrenCount(item) > 0; } catch (...) {}
    return false;
}

thread_local bool t_applyWrote = false;
thread_local int32_t t_selectedHoroscopeKey = 0;
thread_local winrt::weak_ref<wuxc::CalendarView> t_calendarView{nullptr};
thread_local winrt::event_token t_selectedDatesChangedToken{};

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

void FindBothTextBlocks(wuxc::CalendarViewDayItem const& item, wuxc::TextBlock& solarOut, wuxc::TextBlock& lunarOut) {
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
                t_applyWrote = true;
            }
        } catch (...) {}
    }
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
    
    int64_t lunarLabelChangedToken = 0;
    std::wstring savedText;
    bool hasSavedText = false;
    std::wstring appliedText;
    
    ColorState lunarColor;
    ColorState solarColor;
    ColorState itemColor;
    wuxm::Brush normalForeground{nullptr};

    int32_t key = 0;
    int64_t dateChangedToken = 0;
    bool busy = false;
};

struct TrackedDayItem { void* id = nullptr; winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr}; };
constexpr size_t kMaxTrackedDayItems = 512;
thread_local std::unordered_map<void*, std::unique_ptr<DayCell>> t_cells;
thread_local std::vector<TrackedDayItem> t_dayItems;
thread_local std::unordered_map<void*, winrt::weak_ref<wuxc::CalendarViewDayItem>> t_labelOwners;
thread_local bool t_loggedMissingTextBlock = false;
thread_local bool t_loggedTrackLimit = false;
thread_local winrt::weak_ref<wuxc::TextBlock> t_headerLabel{nullptr};

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item);

void* CellId(wuxc::CalendarViewDayItem const& item) { return winrt::get_abi(item); }

DayCell& CellFor(wuxc::CalendarViewDayItem const& item) {
    std::unique_ptr<DayCell>& entry = t_cells[CellId(item)];
    if (entry && entry->item.get() != item) entry = nullptr;
    if (!entry) { entry = std::make_unique<DayCell>(); entry->item = winrt::make_weak(item); }
    return *entry;
}

void TrackDayItem(wuxc::CalendarViewDayItem const& item) {
    void* id = winrt::get_abi(item);
    for (auto it = t_dayItems.begin(); it != t_dayItems.end(); ++it) {
        if (it->id != id) continue;
        if (it->item.get()) { TrackedDayItem tracked = *it; t_dayItems.erase(it); t_dayItems.push_back(tracked); } 
        else { it->item = winrt::make_weak(item); }
        return;
    }
    if (t_dayItems.size() >= kMaxTrackedDayItems) t_dayItems.erase(t_dayItems.begin());
    t_dayItems.push_back({id, winrt::make_weak(item)});
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

wuxm::Brush SetForegroundIfDifferent(wuxc::TextBlock const& textBlock, winrt::Windows::UI::Color const& color, wuxm::Brush const& previousOwn) {
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

wuxm::Brush SetControlForegroundIfDifferent(wuxc::Control const& ctrl, winrt::Windows::UI::Color const& color, wuxm::Brush const& previousOwn) {
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
            state.savedForeground = brush; state.savedForegroundWasLocal = true;
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
            state.savedForeground = brush; state.savedForegroundWasLocal = true;
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
    if (auto item = it->second.get()) { ApplyToDayItem(item); StartSweepTimer(); }
}

void OpenHoroscopeForDateKey(int32_t key) {
    if (g_shuttingDown || !IsValidDate(key / 10000, (key / 100) % 100, key % 100)) return;
    if (key == t_selectedHoroscopeKey && t_horoscopePage.pageBorder.get() &&
        t_horoscopePage.renderedKey == key) return;

    Wh_Log(L"[VNCalendar] Open date: %d", key);
    t_selectedHoroscopeKey = key;
    UpdateHoroscopePageOnCurrentThread();
    StartSweepTimer();
}

void OnDayItemClicked(wuxc::CalendarViewDayItem const& item) {
    if (!item) return;
    try {
        int32_t key = DateKeyFromDateTime(item.Date());
        OpenHoroscopeForDateKey(key);
    } catch (...) { LogCalendarException(L"Day item tapped"); }
}

void EnsureCalendarViewSelectionHooked(wuxc::CalendarView const& calView) {
    if (!calView) return;
    if (t_calendarView.get() == calView && t_selectedDatesChangedToken.value != 0) return;

    if (auto prev = t_calendarView.get()) {
        if (t_selectedDatesChangedToken.value != 0) {
            try { prev.SelectedDatesChanged(t_selectedDatesChangedToken); } catch (...) {}
        }
    }
    t_selectedDatesChangedToken = {};
    t_calendarView = winrt::make_weak(calView);

    try {
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

bool IsOwnText(std::wstring const& text, Settings const& settings) {
    if (text.empty()) return false;
    if (text == settings.workdayText || text == settings.offDayText) return true;
    if (text.find(L"/") != std::wstring::npos) return true;
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    for (const auto& entry : g_holidays) { if (entry.second.name == text) return true; }
    return false;
}

std::wstring ExpandName(std::wstring const& text, std::wstring const& name) {
    if (text.empty()) return {};
    std::wstring result; size_t pos = 0;
    for (;;) {
        size_t found = text.find(L"{name}", pos);
        if (found == std::wstring::npos) { result.append(text, pos, std::wstring::npos); break; }
        result.append(text, pos, found - pos); result += name; pos = found + 6;
    }
    return result;
}

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item) {
    const std::shared_ptr<const Settings> settings = GetSettings();
    if (!item) return;
    int32_t key = DateKeyFromDateTime(item.Date());
    if (!key) return;

    DayCell& cell = CellFor(item);
    if (cell.busy) return;
    cell.busy = true; struct BusyGuard { bool& busy; ~BusyGuard() { busy = false; } } guard{cell.busy};

    if (cell.key != key) {
        cell.key = key;
        cell.savedText.clear();
        cell.appliedText.clear();
        cell.hasSavedText = false;
    }

    HolidayInfo info; const bool hasInfo = LookupHoliday(key, info);

    if (cell.dateChangedToken == 0) {
        try {
            cell.dateChangedToken = item.RegisterPropertyChangedCallback(
                wuxc::CalendarViewDayItem::DateProperty(),
                [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                    if (auto dayItem = sender.try_as<wuxc::CalendarViewDayItem>()) { ApplyToDayItem(dayItem); StartSweepTimer(); }
                });
        } catch (...) {}
    }

    wuxc::TextBlock solarLabel = nullptr;
    wuxc::TextBlock lunarLabel = nullptr;
    FindBothTextBlocks(item, solarLabel, lunarLabel);

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
        
        cell.savedText.clear(); cell.appliedText.clear(); cell.hasSavedText = false;
        cell.lunarColor = ColorState(); cell.solarColor = ColorState(); cell.normalForeground = nullptr;
        t_labelOwners[winrt::get_abi(lunarLabel)] = winrt::make_weak(item);

        try {
            cell.lunarLabelChangedToken = lunarLabel.RegisterPropertyChangedCallback(
                wuxc::TextBlock::TextProperty(), [](wux::DependencyObject const& sender, wux::DependencyProperty const&) { OnLabelTextChanged(sender); });
        } catch (...) {}
    }

    int year = key / 10000; int month = (key / 100) % 100; int day = key % 100;
    
    int lDay, lMonth, lYear; bool isLeap; std::wstring canChi;
    GetLunarDateWinRT_ForDate(year, month, day, lDay, lMonth, lYear, isLeap, canChi);
    
    wchar_t buffer[64];
    if (isLeap) swprintf_s(buffer, L"%d/%d (Nhuận)", lDay, lMonth);
    else swprintf_s(buffer, L"%d/%d", lDay, lMonth);
    std::wstring desired = buffer;

    HolidayScope scope = GetVietnameseHolidayScope(day, month, lDay, lMonth, isLeap);
    bool colorSolar = (scope == HolidayScope::Both || scope == HolidayScope::SolarOnly);
    bool colorLunar = (scope == HolidayScope::Both || scope == HolidayScope::LunarOnly);

    bool useIcsColorOnLunar = hasInfo && (scope == HolidayScope::None);
    if (hasInfo && settings->replaceText && scope != HolidayScope::SolarOnly) {
        if (info.isOffDay) desired = settings->offDayText.empty() ? info.name : ExpandName(settings->offDayText, info.name);
        else desired = ExpandName(settings->workdayText, info.name);
    }

    std::wstring current = ElementText(lunarLabel);

    if (!desired.empty() && current != desired) {
        if (current != cell.appliedText && !IsOwnText(current, *settings)) {
            cell.savedText = current; cell.hasSavedText = true;
        }
        try { lunarLabel.Text(desired); t_applyWrote = true; } catch (...) {}
    } else if (desired.empty() && !cell.appliedText.empty() && current == cell.appliedText) {
        try { if (cell.hasSavedText) { lunarLabel.Text(cell.savedText); t_applyWrote = true; } } catch (...) {}
        cell.hasSavedText = false;
    }
    cell.appliedText = desired;

    EnsureDualLineSpacing(item, solarLabel, lunarLabel, cell.savedText.empty());

    winrt::Windows::UI::Color redColor{255, 255, 90, 90}; 
    winrt::Windows::UI::Color icsColor = info.isOffDay ? settings->offDayColor : settings->workdayColor;

    if (!cell.normalForeground && !cell.lunarColor.hadColor) {
        try { cell.normalForeground = lunarLabel.Foreground(); } catch (...) {}
    }

    // 1. Ép màu Lịch Dương
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

    // 2. Ép màu Lịch Âm
    if (colorLunar || useIcsColorOnLunar) {
        SaveForeground(cell.lunarColor, lunarLabel);
        winrt::Windows::UI::Color targetColor = colorLunar ? redColor : icsColor;
        cell.lunarColor.ownBrush = SetForegroundIfDifferent(lunarLabel, targetColor, cell.lunarColor.ownBrush);
        cell.lunarColor.hadColor = true;
    } else if (colorSolar) {
        SaveForeground(cell.lunarColor, lunarLabel);
        winrt::Windows::UI::Color normalColor{255, 210, 210, 210};
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
// Trang Chi Tiết Tử Vi & Lịch Vạn Niên
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

wuxc::Border CreateLineSection(wuxc::TextBlock& textBlockOut, wuxm::Brush const& textBrush) {
    wuxc::Border rowBorder;
    rowBorder.Padding({6, 6, 6, 7});
    rowBorder.BorderThickness({0, 0, 0, 1});
    rowBorder.BorderBrush(wuxm::SolidColorBrush(winrt::Windows::UI::Color{38, 255, 255, 255}));

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

        wuxm::Brush fgBrush = wuxm::SolidColorBrush(winrt::Windows::UI::Color{255, 245, 245, 245});
        if (auto hdr = t_headerLabel.get()) {
            try { fgBrush = hdr.Foreground(); } catch (...) {}
        }

        wuxc::Border pageBorder;
        pageBorder.Name(kCustomHoroscopePageName);
        pageBorder.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        pageBorder.VerticalAlignment(wux::VerticalAlignment::Stretch);
        pageBorder.Padding({14, 10, 14, 10});
        pageBorder.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{22, 255, 255, 255}));
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

        // 1. Thanh Header trên cùng: [ ← ] + Tiêu đề Ngày Dương / Âm
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
        titleLunar.Foreground(wuxm::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 125, 125}));
        titleLunar.TextWrapping(wux::TextWrapping::Wrap);

        titleStack.Children().Append(titleSolar);
        titleStack.Children().Append(titleLunar);
        wuxc::Grid::SetColumn(titleStack, 1);
        topBar.Children().Append(titleStack);

        wuxc::Grid::SetRow(topBar, 0);
        pageLayout.Children().Append(topBar);

        // 2. Đường kẻ ngang phân cách
        wuxc::Border headerLine;
        headerLine.Height(1.0);
        headerLine.Margin({0, 0, 0, 4});
        headerLine.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{65, 255, 255, 255}));
        wuxc::Grid::SetRow(headerLine, 1);
        pageLayout.Children().Append(headerLine);

        // 3. Các dòng thông tin chi tiết Tử Vi & Can Chi
        wuxc::ScrollViewer scrollViewer;
        scrollViewer.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
        scrollViewer.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);

        wuxc::StackPanel listStack;
        wuxc::TextBlock tbCanChi{nullptr}, tbMenhTruc{nullptr}, tbGio{nullptr}, tbNen{nullptr}, tbKy{nullptr}, tbXung{nullptr};

        listStack.Children().Append(CreateLineSection(tbCanChi, fgBrush));
        listStack.Children().Append(CreateLineSection(tbMenhTruc, fgBrush));
        listStack.Children().Append(CreateLineSection(tbGio, fgBrush));
        listStack.Children().Append(CreateLineSection(tbNen, fgBrush));
        listStack.Children().Append(CreateLineSection(tbKy, fgBrush));

        wuxc::Border lastRow = CreateLineSection(tbXung, fgBrush);
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
        Wh_Log(L"[VNCalendar] Horoscope page created");
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
    if (t_selectedHoroscopeKey != 0) EnsureHoroscopePageCreated();

    auto hostGrid = t_horoscopePage.hostGrid.get();
    auto pageBorder = t_horoscopePage.pageBorder.get();
    if (!hostGrid || !pageBorder) return;
    if (t_horoscopePage.renderedKey == t_selectedHoroscopeKey) return;

    const bool showDetail = (t_selectedHoroscopeKey != 0);

    static thread_local bool loggedUpdateError = false;
    try {
        if (showDetail) {
            HoroscopePageData data = BuildHoroscopePageData(t_selectedHoroscopeKey);
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
// Quét và Ghi Đè Lên Tiêu Đề Header
void UpdateHeaderLabelOnCurrentThread() {
    auto window = wux::Window::Current(); if (!window) return;
    auto root = window.Content().try_as<wux::FrameworkElement>(); if (!root) return;
    wuxc::TextBlock headerLabel = t_headerLabel.get();
    wuxc::TextBlock solarDateHeader = nullptr;

    if (!headerLabel) {
        std::vector<wux::DependencyObject> queue;
        queue.push_back(root);
        size_t head = 0;

        while (head < queue.size() && queue.size() < 2000) {
            auto current = queue[head++];

            if (auto cv = current.try_as<wuxc::CalendarView>()) {
                EnsureCalendarViewSelectionHooked(cv);
                continue;
            }
            if (current.try_as<wuxc::CalendarViewDayItem>()) {
                continue;
            }

            if (auto textBlock = current.try_as<wuxc::TextBlock>()) {
                std::wstring name = ElementName(textBlock);
                std::wstring text;
                try { text = textBlock.Text().c_str(); } catch (...) {}

                if (name == kCustomHeaderLunarName ||
                    name.find(L"Lunar") != std::wstring::npos ||
                    name.find(L"Additional") != std::wstring::npos ||
                    name.find(L"Secondary") != std::wstring::npos ||
                    text.find(L"月") != std::wstring::npos ||
                    text.find(L"Năm ") != std::wstring::npos ||
                    text.find(L"Tý") != std::wstring::npos || text.find(L"Sửu") != std::wstring::npos ||
                    text.find(L"Dần") != std::wstring::npos || text.find(L"Mão") != std::wstring::npos ||
                    text.find(L"Thìn") != std::wstring::npos || text.find(L"Tỵ") != std::wstring::npos ||
                    text.find(L"Ngọ") != std::wstring::npos || text.find(L"Mùi") != std::wstring::npos ||
                    text.find(L"Thân") != std::wstring::npos || text.find(L"Dậu") != std::wstring::npos ||
                    text.find(L"Tuất") != std::wstring::npos || text.find(L"Hợi") != std::wstring::npos) {
                    headerLabel = textBlock;
                    t_headerLabel = winrt::make_weak(textBlock);
                    break;
                }

                if (!solarDateHeader &&
                    (text.find(L"Thứ") != std::wstring::npos ||
                     text.find(L"Chủ Nhật") != std::wstring::npos ||
                     text.find(L"Chủ nhật") != std::wstring::npos ||
                     text.find(L", ") != std::wstring::npos)) {
                    solarDateHeader = textBlock;
                }
            }

            int count = 0;
            try { count = wuxm::VisualTreeHelper::GetChildrenCount(current); } catch (...) {}
            for (int i = 0; i < count; i++) {
                try {
                    auto child = wuxm::VisualTreeHelper::GetChild(current, i);
                    if (child) queue.push_back(child);
                } catch (...) {}
            }
        }

        if (!headerLabel && solarDateHeader) {
            try {
                wux::DependencyObject parent = wuxm::VisualTreeHelper::GetParent(solarDateHeader);
                wuxc::Panel headerPanel = nullptr;
                wux::DependencyObject childInPanel = solarDateHeader;
                for (int depth = 0; depth < 4 && parent; depth++) {
                    if (auto panel = parent.try_as<wuxc::Panel>()) {
                        headerPanel = panel;
                        break;
                    }
                    childInPanel = parent;
                    parent = wuxm::VisualTreeHelper::GetParent(parent);
                }

                if (headerPanel) {
                    int count = wuxm::VisualTreeHelper::GetChildrenCount(headerPanel);
                    bool passedSolar = false;
                    for (int i = 0; i < count; i++) {
                        auto sibling = wuxm::VisualTreeHelper::GetChild(headerPanel, i);
                        if (sibling == childInPanel) {
                            passedSolar = true;
                            continue;
                        }
                        if (passedSolar) {
                            if (auto tb = sibling.try_as<wuxc::TextBlock>()) {
                                headerLabel = tb;
                                break;
                            }
                        }
                    }

                    if (!headerLabel) {
                        wuxc::TextBlock newHeaderLunar;
                        newHeaderLunar.Name(kCustomHeaderLunarName);
                        newHeaderLunar.FontSize(14.0);
                        newHeaderLunar.Opacity(0.85);
                        newHeaderLunar.Margin({0, 2, 0, 0});
                        try {
                            newHeaderLunar.Foreground(solarDateHeader.Foreground());
                        } catch (...) {}

                        headerPanel.Children().Append(newHeaderLunar);
                        headerLabel = newHeaderLunar;
                    }

                    if (headerLabel) {
                        t_headerLabel = winrt::make_weak(headerLabel);
                    }
                }
            } catch (...) {}
        }
    }

    if (headerLabel) {
        int lDay, lMonth, lYear;
        bool isLeap;
        std::wstring canChi;
        GetLunarDateWinRT(lDay, lMonth, lYear, isLeap, canChi);

        wchar_t buffer[128];
        if (isLeap) {
            swprintf_s(buffer, L"%d Tháng %d (Nhuận) Năm %s", lDay, lMonth, canChi.c_str());
        } else {
            swprintf_s(buffer, L"%d Tháng %d Năm %s", lDay, lMonth, canChi.c_str());
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
// Keeping the calendar up to date & Bắt tọa độ Click qua CoreWindow
thread_local winrt::Windows::System::DispatcherQueueTimer t_sweepTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker t_sweepTimerRevoker;
constexpr int kSweepIntervalMs = 100;
constexpr int kSweepQuietTicksToStop = 20;
thread_local int t_sweepQuietTicks = 0;

void StopSweepTimer() {
    t_sweepQuietTicks = 0;
    if (t_sweepTimer) { try { t_sweepTimer.Stop(); } catch (...) {} }
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
                winrt::auto_revoke, [](winrt::Windows::System::DispatcherQueueTimer const&, wf::IInspectable const&) { SweepDayItems(); });
        }
        if (!t_sweepTimer.IsRunning()) t_sweepTimer.Start();
    } catch (...) {}
}

void OnCoreWindowPointerReleased(wuc::PointerEventArgs const& args) {
    try {
        wf::Point pt = args.CurrentPoint().Position();

        if (t_selectedHoroscopeKey != 0) {
            if (auto backBtn = t_horoscopePage.backButton.get()) {
                float w = static_cast<float>(backBtn.ActualWidth());
                float h = static_cast<float>(backBtn.ActualHeight());
                if (w > 0 && h > 0) {
                    wf::Rect bounds = backBtn.TransformToVisual(nullptr).TransformBounds({0, 0, w, h});
                    if (pt.X >= bounds.X && pt.X <= bounds.X + bounds.Width &&
                        pt.Y >= bounds.Y && pt.Y <= bounds.Y + bounds.Height) {
                        t_selectedHoroscopeKey = 0;
                        UpdateHoroscopePageOnCurrentThread();
                        StartSweepTimer();
                    }
                }
            }
            return;
        }

        for (const auto& tracked : t_dayItems) {
            auto item = tracked.item.get();
            if (!item) continue;
            try {
                if (item.Visibility() != wux::Visibility::Visible) continue;
                float w = static_cast<float>(item.ActualWidth());
                float h = static_cast<float>(item.ActualHeight());
                if (w <= 0 || h <= 0) continue;

                wf::Rect bounds = item.TransformToVisual(nullptr).TransformBounds({0, 0, w, h});
                if (pt.X >= bounds.X && pt.X <= bounds.X + bounds.Width &&
                    pt.Y >= bounds.Y && pt.Y <= bounds.Y + bounds.Height) {
                    OnDayItemClicked(item);
                    break;
                }
            } catch (...) {}
        }
    } catch (...) {}
}

struct CoreWindowTriggers {
    wuc::CoreWindow coreWindow{nullptr};
    winrt::event_token activatedToken{};
    winrt::event_token visibilityChangedToken{};
    winrt::event_token pointerReleasedToken{};
};
thread_local CoreWindowTriggers t_windowTriggers;
constexpr ULONGLONG kCalendarOpenedDebounceMs = 500;
thread_local ULONGLONG t_lastCalendarOpenedTick = 0;
void OnCalendarOpenedOnCurrentThread();
void InitializeForCurrentThread();

void RegisterCalendarTriggersOnCurrentThread() {
    try {
        if (t_windowTriggers.coreWindow) return;
        auto coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) return;
        t_windowTriggers.coreWindow = coreWindow;
        t_windowTriggers.activatedToken = coreWindow.Activated(
            [](wuc::CoreWindow const&, wuc::WindowActivatedEventArgs const& args) {
                if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated) OnCalendarOpenedOnCurrentThread();
            });
        t_windowTriggers.visibilityChangedToken = coreWindow.VisibilityChanged(
            [](wuc::CoreWindow const&, wuc::VisibilityChangedEventArgs const& args) {
                if (args.Visible()) OnCalendarOpenedOnCurrentThread();
            });
        t_windowTriggers.pointerReleasedToken = coreWindow.PointerReleased(
            [](wuc::CoreWindow const&, wuc::PointerEventArgs const& args) {
                OnCoreWindowPointerReleased(args);
            });
    } catch (...) {}
}

void UnregisterCalendarTriggersOnCurrentThread() {
    try {
        auto coreWindow = t_windowTriggers.coreWindow;
        if (coreWindow) {
            if (t_windowTriggers.activatedToken.value != 0) coreWindow.Activated(t_windowTriggers.activatedToken);
            if (t_windowTriggers.visibilityChangedToken.value != 0) coreWindow.VisibilityChanged(t_windowTriggers.visibilityChangedToken);
            if (t_windowTriggers.pointerReleasedToken.value != 0) coreWindow.PointerReleased(t_windowTriggers.pointerReleasedToken);
        }
    } catch (...) {}
    t_windowTriggers = {};
}

thread_local wux::FrameworkElement t_layoutUpdatedElement{nullptr};
thread_local winrt::event_token t_layoutUpdatedToken{};
thread_local ULONGLONG t_layoutUpdatedStartTick = 0;
thread_local ULONGLONG t_lastLayoutUpdatedPassTick = 0;
constexpr ULONGLONG kLayoutUpdatedWatchMs = 5000;
constexpr ULONGLONG kLayoutUpdatedThrottleMs = 100;

void RevokeLayoutUpdatedWatch() {
    if (t_layoutUpdatedToken.value != 0) {
        auto element = t_layoutUpdatedElement;
        if (element) { try { element.LayoutUpdated(t_layoutUpdatedToken); } catch (...) {} }
    }
    t_layoutUpdatedToken = {}; t_layoutUpdatedElement = nullptr;
}

void OnCalendarLayoutUpdatedOnCurrentThread() {
    const ULONGLONG now = GetTickCount64();
    if (now - t_layoutUpdatedStartTick > kLayoutUpdatedWatchMs) { RevokeLayoutUpdatedWatch(); return; }
    if (now - t_lastLayoutUpdatedPassTick < kLayoutUpdatedThrottleMs) return;
    t_lastLayoutUpdatedPassTick = now;
    SweepDayItems();
    if (!t_dayItems.empty()) RevokeLayoutUpdatedWatch();
}

void WatchCalendarLayoutOnCurrentThread() {
    if (t_layoutUpdatedToken.value != 0) return;
    try {
        auto window = wux::Window::Current(); if (!window) return;
        auto root = window.Content().try_as<wux::FrameworkElement>(); if (!root) return;
        t_layoutUpdatedElement = root; t_layoutUpdatedStartTick = GetTickCount64(); t_lastLayoutUpdatedPassTick = 0;
        t_layoutUpdatedToken = root.LayoutUpdated([](wf::IInspectable const&, wf::IInspectable const&) { OnCalendarLayoutUpdatedOnCurrentThread(); });
    } catch (...) { RevokeLayoutUpdatedWatch(); }
}

constexpr size_t kMaxScannedElements = 16384;
constexpr int kMaxScanDepth = 96;
thread_local bool t_loggedNoWindow = false;

void CollectDayItemsFromElement(wux::DependencyObject const& root, size_t& budget, int depth) {
    if (!root || budget == 0 || depth > kMaxScanDepth) return;
    budget--;
    if (auto cv = root.try_as<wuxc::CalendarView>()) {
        EnsureCalendarViewSelectionHooked(cv);
    }
    if (auto dayItem = root.try_as<wuxc::CalendarViewDayItem>()) { TrackDayItem(dayItem); return; }
    int count = 0;
    try { count = wuxm::VisualTreeHelper::GetChildrenCount(root); } catch (...) { return; }
    for (int childIndex = 0; childIndex < count; childIndex++) {
        wux::DependencyObject child = nullptr;
        try { child = wuxm::VisualTreeHelper::GetChild(root, childIndex); } catch (...) { continue; }
        CollectDayItemsFromElement(child, budget, depth + 1);
    }
}

void CollectDayItemsOnCurrentThread() {
    wux::Window window = nullptr;
    try { window = wux::Window::Current(); } catch (...) { return; }
    if (!window) return;
    try {
        size_t budget = kMaxScannedElements;
        if (auto content = window.Content()) CollectDayItemsFromElement(content, budget, 0);
        for (auto popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
            if (budget == 0) break;
            if (auto child = popup.Child()) CollectDayItemsFromElement(child, budget, 0);
        }
    } catch (...) {}
}

void OnCalendarOpenedOnCurrentThread() {
    InitializeForCurrentThread();
    const ULONGLONG now = GetTickCount64();
    if (now - t_lastCalendarOpenedTick < kCalendarOpenedDebounceMs) return;
    t_lastCalendarOpenedTick = now;

    t_selectedHoroscopeKey = 0;

    RegisterCalendarTriggersOnCurrentThread();
    SweepDayItems();
    StartSweepTimer();
    WatchCalendarLayoutOnCurrentThread();
    RequestHolidayData();
}

void SweepDayItems() {
    t_applyWrote = false;
    InitializeForCurrentThread();
    RegisterCalendarTriggersOnCurrentThread();
    CollectDayItemsOnCurrentThread();

    UpdateHeaderLabelOnCurrentThread();

    for (size_t i = 0; i < t_dayItems.size();) {
        auto item = t_dayItems[i].item.get();
        if (!item) { t_dayItems.erase(t_dayItems.begin() + i); continue; }
        ApplyToDayItem(item);
        i++;
    }

    UpdateHoroscopePageOnCurrentThread();

    for (auto it = t_cells.begin(); it != t_cells.end();) {
        if (it->second->item.get()) ++it; else it = t_cells.erase(it);
    }
    for (auto it = t_labelOwners.begin(); it != t_labelOwners.end();) {
        if (it->second.get()) ++it; else it = t_labelOwners.erase(it);
    }

    if (t_applyWrote) t_sweepQuietTicks = 0;
    else if (++t_sweepQuietTicks >= kSweepQuietTicksToStop) StopSweepTimer();
}

void UninitializeDayItemsForCurrentThread() {
    if (auto calView = t_calendarView.get()) {
        if (t_selectedDatesChangedToken.value != 0) {
            try { calView.SelectedDatesChanged(t_selectedDatesChangedToken); } catch (...) {}
        }
    }
    t_selectedDatesChangedToken = {};
    t_calendarView = nullptr;

    for (auto& entry : t_cells) {
        DayCell& cell = *entry.second;
        auto item = cell.item.get();
        if (!item) continue;
        if (cell.dateChangedToken != 0) {
            try { item.UnregisterPropertyChangedCallback(wuxc::CalendarViewDayItem::DateProperty(), cell.dateChangedToken); } catch (...) {}
            cell.dateChangedToken = 0;
        }
        try {
            if (cell.itemColor.hadColor) RestoreControlForeground(item, cell.itemColor);
        } catch (...) {}
        
        if (auto lunarLabel = cell.lunarLabel.get()) {
            if (cell.lunarLabelChangedToken) {
                try { lunarLabel.UnregisterPropertyChangedCallback(wuxc::TextBlock::TextProperty(), cell.lunarLabelChangedToken); } catch (...) {}
                cell.lunarLabelChangedToken = 0;
            }
            try {
                lunarLabel.RenderTransform(nullptr);
                lunarLabel.ClearValue(wux::FrameworkElement::MarginProperty());
                if (cell.hasSavedText) lunarLabel.Text(cell.savedText);
                if (cell.lunarColor.hadColor) RestoreForeground(lunarLabel, cell.lunarColor);
            } catch (...) {}
        }
        
        if (auto solarLabel = cell.solarLabel.get()) {
            try {
                solarLabel.RenderTransform(nullptr);
                solarLabel.ClearValue(wux::FrameworkElement::MarginProperty());
                if (cell.solarColor.hadColor) RestoreForeground(solarLabel, cell.solarColor);
            } catch (...) {}
        }
    }

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
    
    t_headerLabel = nullptr; 
    
    t_sweepQuietTicks = 0;
    try { if (t_sweepTimer) t_sweepTimer.Stop(); } catch (...) {}
    t_sweepTimerRevoker.revoke();
    t_sweepTimer = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar Clock & Tooltip Horoscope (explorer.exe)

std::atomic<bool> g_systemTrayModuleHooked{false};
std::atomic<bool> g_tooltipSymbolActive{false};
thread_local bool t_inGetTimeToolTipString = false;
thread_local ULONGLONG t_lastLongDateTooltipTick = 0;

using GetTimeFormatEx_t = decltype(&GetTimeFormatEx);
GetTimeFormatEx_t GetTimeFormatEx_Original;

using GetDateFormatEx_t = decltype(&GetDateFormatEx);
GetDateFormatEx_t GetDateFormatEx_Original;

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

// Bơm toàn bộ nội dung Âm lịch + Can Chi + Tử Vi vào hộp Tooltip khi rê chuột vào đồng hồ
void AppendHoroscopeToToolTipString(LPVOID tooltipPtrPtr) {
    if (!tooltipPtrPtr || !*(void**)tooltipPtrPtr) return;

    g_formatIndex++;
    PCWSTR extraLine = GetLunarHoroscopeFull();
    if (!extraLine || !*extraLine) return;
    size_t extraLength = wcslen(extraLine);

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
           extraLine, sizeof(wchar_t) * extraLength);
    tooltipHeaderNew->length += static_cast<uint32_t>(extraLength);

    tooltipHeaderNew->buffer[tooltipHeaderNew->length] = L'\0';
    *(shared_hstring_header**)tooltipPtrPtr = tooltipHeaderNew;
}

LPVOID InvokeGetTimeToolTipStringHook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4,
    ClockSystemTrayIconDataModel_GetTimeToolTipString_t original) {
    g_tooltipSymbolActive = true;
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
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
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
    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (!module) module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandleW(L"ExplorerExtensions.dll");
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

bool IsCallStackFromSystemTray() {
    if (g_shuttingDown) return false;

    HMODULE hSysTray = GetModuleHandleW(L"SystemTray.dll");
    HMODULE hTaskbarView = GetModuleHandleW(L"Taskbar.View.dll");
    HMODULE hExpExt = GetModuleHandleW(L"ExplorerExtensions.dll");
    if (!hSysTray && !hTaskbarView && !hExpExt) return false;

    void* frames[8];
    WORD count = CaptureStackBackTrace(0, ARRAYSIZE(frames), frames, nullptr);
    for (WORD i = 0; i < count; i++) {
        HMODULE hMod = nullptr;
        if (GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(frames[i]), &hMod) &&
            hMod) {
            if ((hSysTray && hMod == hSysTray) ||
                (hTaskbarView && hMod == hTaskbarView) ||
                (hExpExt && hMod == hExpExt)) {
                return true;
            }
        }
    }
    return false;
}

int WINAPI GetTimeFormatEx_Hook(LPCWSTR lpLocaleName,
                                DWORD dwFlags,
                                CONST SYSTEMTIME* lpTime,
                                LPCWSTR lpFormat,
                                LPWSTR lpTimeStr,
                                int cchTime) {
    if (!t_inGetTimeToolTipString && IsCallStackFromSystemTray()) {
        if (!cchTime) {
            return FORMATTED_BUFFER_SIZE;
        }
        SYSTEMTIME st;
        GetLocalTime(&st);
        static const PCWSTR kWeekdays[] = {
            L"CN", L"T2", L"T3", L"T4", L"T5", L"T6", L"T7"
        };
        PCWSTR weekday = (st.wDayOfWeek < 7) ? kWeekdays[st.wDayOfWeek] : L"";
        int written = swprintf_s(lpTimeStr, cchTime, L"%02d:%02d - %s, %02d/%02d",
                                 st.wHour, st.wMinute, weekday, st.wDay, st.wMonth);
        if (written >= 0) {
            return written + 1;
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
    if (!t_inGetTimeToolTipString &&
        !(dwFlags & DATE_LONGDATE) &&
        (!lpFormat || (_wcsicmp(lpFormat, L"dddd") != 0 && _wcsicmp(lpFormat, L"ddd") != 0)) &&
        IsCallStackFromSystemTray()) {
        if (!cchDate) {
            return FORMATTED_BUFFER_SIZE;
        }
        g_formatIndex++;
        PCWSTR lunarStr = GetLunarFullFormatted();
        int written = swprintf_s(lpDateStr, cchDate, L"%s", lunarStr);
        if (written >= 0) {
            return written + 1;
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
// Process plumbing
enum class Target { Explorer, ShellExperienceHost, ShellHost, };
Target g_target = Target::ShellExperienceHost;
std::atomic<UINT> g_runFromWindowThreadViaPostMessageMsg{0};

UINT RunFromWindowThreadViaPostMessageMsg() {
    UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        message = RegisterWindowMessage(L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);
        g_runFromWindowThreadViaPostMessageMsg.store(message);
    }
    return message;
}

std::mutex g_pendingPostHooksMutex;
std::vector<HHOOK> g_pendingPostHooks;
void RememberPendingPostHook(HHOOK hook) { std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex); g_pendingPostHooks.push_back(hook); }
void ForgetPendingPostHook(HHOOK hook) {
    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    for (auto it = g_pendingPostHooks.begin(); it != g_pendingPostHooks.end(); ++it) {
        if (*it == hook) { g_pendingPostHooks.erase(it); break; }
    }
}
void UnhookPendingPostHooks() {
    std::vector<HHOOK> hooks;
    { std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex); hooks.swap(g_pendingPostHooks); }
    for (HHOOK hook : hooks) UnhookWindowsHookEx(hook);
}

void DrainPendingRunFromWindowThreadPostsOnCurrentThread() {
    const UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) return;
    MSG msg; while (PeekMessage(&msg, nullptr, message, message, PM_REMOVE)) {}
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
    static const UINT runFromWindowThreadRegisteredMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM { RunFromWindowThreadProc_t proc; PVOID procParam; };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;
    if (dwThreadId == GetCurrentThreadId()) { proc(procParam); return true; }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (nCode == HC_ACTION) {
            const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
            if (cwp->message == runFromWindowThreadRegisteredMsg) {
                RUN_FROM_WINDOW_THREAD_PARAM* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                param->proc(param->procParam);
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }, nullptr, dwThreadId);
    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param; param.proc = proc; param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

bool RunFromWindowThreadViaPostMessage(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    struct RUN_FROM_WINDOW_THREAD_PARAM { RunFromWindowThreadProc_t proc; PVOID procParam; HHOOK hook; };
    const UINT message = RunFromWindowThreadViaPostMessageMsg();
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;

    HHOOK hook = SetWindowsHookEx(WH_GETMESSAGE, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (nCode == HC_ACTION && wParam == PM_REMOVE) {
            MSG* msg = (MSG*)lParam;
            if (msg->message == g_runFromWindowThreadViaPostMessageMsg.load()) {
                auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)msg->lParam;
                if (param) { ForgetPendingPostHook(param->hook); param->proc(param->procParam); UnhookWindowsHookEx(param->hook); delete param; msg->lParam = 0; }
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }, nullptr, dwThreadId);
    if (!hook) return false;

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{ .proc = proc, .procParam = procParam, .hook = hook, };
    RememberPendingPostHook(hook);
    if (!PostMessage(hWnd, message, 0, (LPARAM)param)) {
        ForgetPendingPostHook(hook); UnhookWindowsHookEx(hook); delete param; return false;
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

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    if (g_shuttingDown) return;
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName || !IsCalendarWindowClass(lpClassName)) return;
    RunFromWindowThreadViaPostMessage(hWnd, [](PVOID) { InitializeForCurrentThread(); OnCalendarOpenedOnCurrentThread(); }, nullptr);
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam, DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) return hWnd;
    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) return hWnd;
    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

void OnWindowShown(HWND hWnd) {
    if (g_shuttingDown) return;
    DWORD dwProcessId = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return;
    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 || !IsCalendarWindowClass(szClassName)) return;
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
    struct ENUM_WINDOWS_PARAM { std::vector<HWND>* hWnds; };
    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;
        DWORD dwProcessId = 0;
        if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId()) return TRUE;
        WCHAR szClassName[32];
        if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) return TRUE;
        if (IsCalendarWindowClass(szClassName)) param.hWnds->push_back(hWnd);
        return TRUE;
    }, (LPARAM)&param);
    return hWnds;
}

void RefreshCalendarOnUiThreads() {
    for (HWND hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(hCoreWnd, [](PVOID) { SweepDayItems(); StartSweepTimer(); }, nullptr);
    }
}

template <typename Prototype>
bool InstallHook(HMODULE module, PCSTR functionName, Prototype* hook, Prototype** original) {
    if (!module) return false;
    Prototype* target = reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));
    if (!target) return false;
    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) return false;
    return true;
}

BOOL Wh_ModInit() {
    g_shuttingDown = false;
    g_target = Target::ShellExperienceHost;
    WCHAR moduleFilePath[MAX_PATH];
    switch (GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath): return FALSE;
        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"explorer.exe") == 0) {
                    g_target = Target::Explorer;
                } else if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else return FALSE;
            break;
    }

    if (g_target == Target::Explorer) {
        if (HMODULE sysTrayModule = GetSystemTrayModuleHandle()) {
            g_systemTrayModuleHooked = true;
            HookSystemTraySymbols(sysTrayModule);
        }

        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBaseModule) return FALSE;
        InstallHook(kernelBaseModule, "LoadLibraryExW", LoadLibraryExW_Hook, &LoadLibraryExW_Original);
        InstallHook(kernelBaseModule, "GetTimeFormatEx", GetTimeFormatEx_Hook, &GetTimeFormatEx_Original);
        InstallHook(kernelBaseModule, "GetDateFormatEx", GetDateFormatEx_Hook, &GetDateFormatEx_Original);
        return TRUE;
    }

    LoadSettings();
    RunFromWindowThreadViaPostMessageMsg();
    HMODULE user32Module = LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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
        RunFromWindowThread(hCoreWnd, [](PVOID) { InitializeForCurrentThread(); OnCalendarOpenedOnCurrentThread(); }, nullptr);
    }
    RequestHolidayData();
}

void Wh_ModUninit() {
    g_shuttingDown = true;
    if (g_target == Target::Explorer) {
        RefreshTaskbarClock();
        return;
    }
    StopWorkerThread();
    for (auto hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }
    UnhookPendingPostHooks();
}

void Wh_ModSettingsChanged() {
    if (g_target == Target::Explorer) {
        RefreshTaskbarClock();
        return;
    }
    const std::wstring previousUrl = GetSettings()->dataSourceUrl;
    LoadSettings();
    const std::shared_ptr<const Settings> settings = GetSettings();
    if (settings->dataSourceUrl != previousUrl) ResetHolidayData();
    RefreshCalendarOnUiThreads();
    RequestHolidayData();
}