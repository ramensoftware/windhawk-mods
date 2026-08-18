// ==WindhawkMod==
// @id prayertray
// @name PrayerTray
// @description Islamic prayer times on the taskbar, next to the clock.
// @version 3.1.4
// @author Omar Alhami (mar)
// @github https://github.com/n0tmar
// @homepage https://github.com/n0tmar/PrayerTray
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lwindowsapp -lshlwapi -lshell32 -ladvapi32 -luser32 -lwinhttp -lwinmm -lversion -lcomctl32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# PrayerTray

Shows the next Islamic prayer on the Windows taskbar so you can see it while
you work, without opening another app. Left-click toggles today's times
(Fajr, Dhuhr, Asr, Maghrib, Isha). Works on Windows 10 and 11.

## Location

Uses HTTPS IP geolocation with a cached offline fallback. IP location can be
affected by VPNs and ISP routing; manual coordinates are the reliable option
when an exact city is required.

## Source

https://github.com/n0tmar/PrayerTray

## Support

If you love this, you can support me on Patreon :)
https://www.patreon.com/n0tmar
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showOnTaskbar: true
  $name: Show on taskbar
  $description: Show prayer text beside the clock
- playPrayerNotification: false
  $name: Prayer notification sound
  $description: Soft Allahu Akbar clip 1 minute before prayer time; plays a preview when enabled
- useManualLocation: false
  $name: Manual location
  $description: Ignore auto location and use the fields below
- manualCity: ""
  $name: City
  $description: Used when manual location is on
- manualCountry: ""
  $name: Country
  $description: Used when manual location is on
- manualLatitude: ""
  $name: Latitude
  $description: Decimal degrees; enter both latitude and longitude
- manualLongitude: ""
  $name: Longitude
  $description: Decimal degrees; enter both latitude and longitude
- calculationMethod: 0
  $name: Calculation method
  $description: Leave at 0 for your country's default. Advanced users can set an Aladhan method number.
- taskbarContentMode: smart
  $name: Taskbar text
  $description: Countdown, prayer clock time, or auto
  $options:
  - countdown: Countdown
  - time: Prayer time
  - smart: Auto
- timeFormat: 12h
  $name: Time format
  $description: 12-hour or 24-hour
  $options:
  - 12h: 12-hour
  - 24h: 24-hour
- language: en
  $name: Language
  $description: Prayer names and labels
  $options:
  - en: English
  - ar: Arabic
  - fr: French
  - ur: Urdu
  - tr: Turkish
  - id: Indonesian
- oldTaskbarOnWin11: false
  $name: Old taskbar on Windows 11
  $description: Use the Win10-style overlay (ExplorerPatcher)
*/
// ==/WindhawkModSettings==

#ifndef WH_MOD_ID
#define WH_MOD_ID L"prayertray"
#endif

#ifndef WH_MOD_VERSION
#define WH_MOD_VERSION L"3.1.4"
#endif

#include <windhawk_utils.h>

#include <atomic>
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <future>
#include <functional>
#include <iomanip>
#include <initializer_list>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <dwmapi.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <winhttp.h>

#undef GetCurrentTime

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

namespace {

// A normal colon has the Unicode bidi class needed to keep hour:minute in
// order inside Arabic and Urdu text. U+2236 (ratio) is visually similar but
// can be reordered as a neutral character by GDI.
constexpr wchar_t kTimeColon = L':';
constexpr wchar_t kLre = L'\u202A';
constexpr wchar_t kPdf = L'\u202C';
constexpr int kNotificationLeadMinutes = 1;
constexpr int kCurrentPrayerHighlightMinutes = 2;
constexpr int kCacheStaleHours = 6;
constexpr int kLocationRefreshHours = 24;
constexpr double kClusterDistanceKm = 80.0;
constexpr int kScheduleRefreshIntervalMs = 15 * 60 * 1000;
constexpr int kRunFromWindowTimeoutMs = 3000;
constexpr wchar_t kNotificationSoundUrl[] =
    L"https://raw.githubusercontent.com/n0tmar/PrayerTray/main/windhawk/audio/allah-akbar.mp3";
constexpr size_t kNotificationSoundMinBytes = 4096;
constexpr size_t kNotificationSoundMaxBytes = 1024 * 1024;

enum class PrayerName { Fajr, Dhuhr, Asr, Maghrib, Isha, Count };

enum class TaskbarBackend { Win11Xaml, Win10Overlay };

enum class ContentMode { Countdown, Time, Smart };

struct ModSettings {
    bool showOnTaskbar = true;
    bool playPrayerNotification = false;
    bool useManualLocation = false;
    std::wstring manualCity;
    std::wstring manualCountry;
    std::wstring manualLatitude;
    std::wstring manualLongitude;
    int calculationMethod = 0;
    ContentMode contentMode = ContentMode::Smart;
    bool use24Hour = false;
    std::wstring language = L"en";
    bool oldTaskbarOnWin11 = false;
};

struct LocationInfo {
    double latitude = 0;
    double longitude = 0;
    std::wstring city;
    std::wstring region;
    std::wstring country;
    std::wstring timezone;
    std::wstring source = L"ip";
    ULONGLONG resolvedAtUtcMs = 0;
};

struct PrayerEntry {
    PrayerName name = PrayerName::Fajr;
    SYSTEMTIME time{};
    bool isNext = false;
    bool isCurrent = false;
};

struct PrayerSchedule {
    int year = 0;
    int month = 0;
    int day = 0;
    LocationInfo location;
    std::vector<PrayerEntry> prayers;
    ULONGLONG fetchedAtUtcMs = 0;
};

struct NextPrayerInfo {
    PrayerName name = PrayerName::Fajr;
    SYSTEMTIME time{};
    ULONGLONG secondsUntil = 0;
};

struct DisplayContent {
    bool visible = false;
    std::wstring top;
    std::wstring bottom;
    std::wstring color = L"FFFFFF";
};

struct IpCandidate {
    std::wstring provider;
    int weight = 0;
    double latitude = 0;
    double longitude = 0;
    std::wstring city;
    std::wstring region;
    std::wstring country;
    std::wstring timezone;
};

ModSettings g_settings;
std::mutex g_settingsMutex;
std::mutex g_scheduleMutex;
LocationInfo g_location;
PrayerSchedule g_currentSchedule;
PrayerSchedule g_nextSchedule;
bool g_hasCurrentSchedule = false;
bool g_hasNextSchedule = false;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_refreshRequested{false};
std::atomic<bool> g_refreshRunning{false};
std::atomic<bool> g_forceLocationRefresh{false};
std::atomic<bool> g_lastRefreshFailed{false};
std::atomic<bool> g_isFullscreen{false};
std::atomic<TaskbarBackend> g_backend{TaskbarBackend::Win11Xaml};
std::atomic<bool> g_isWin11{false};
std::atomic<int> g_uiTimerIntervalMs{500};

HANDLE g_stopEvent = nullptr;
HANDLE g_refreshWakeEvent = nullptr;
HANDLE g_refreshThread = nullptr;
HANDLE g_uiReadyEvent = nullptr;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
HMODULE g_moduleInstance = nullptr;
std::atomic<bool> g_soundPreviewRequested{false};

HWND g_messageHwnd = nullptr;
UINT_PTR g_uiTimer = 0;
UINT_PTR g_repositionTimer = 0;
UINT_PTR g_fullscreenTimer = 0;
HWND g_win10OverlayHwnd = nullptr;
HWND g_flyoutHwnd = nullptr;
bool g_flyoutVisible = false;
RECT g_flyoutAnchorRect{};
bool g_hasFlyoutAnchor = false;
HHOOK g_flyoutMouseHook = nullptr;
ULONGLONG g_flyoutSuppressDismissUntil = 0;
HMENU g_contextMenu = nullptr;
bool g_messageClassRegistered = false;
bool g_flyoutClassRegistered = false;
bool g_overlayClassRegistered = false;

std::unordered_set<std::wstring> g_playedNotificationKeys;
int g_notificationActiveDateKey = 0;

std::atomic<bool> g_systemTrayModuleHooked{false};
std::atomic<bool> g_slotInjected;
std::atomic<int> g_applySlotAttempts;
std::atomic<int> g_injectRetryMs{100};
std::atomic<ULONGLONG> g_nextInjectionFailureLogMs{0};
int g_lastUiTimerIntervalMs = -1;
bool g_hasOverlayPlacement = false;
int g_lastOverlayX = 0;
int g_lastOverlayY = 0;
int g_lastOverlayW = 0;
int g_lastOverlayH = 0;
bool g_lastOverlayShown = false;
std::list<winrt::weak_ref<FrameworkElement>> g_observedElements;
Controls::TextBlock g_topTextBlock{nullptr};
Controls::TextBlock g_bottomTextBlock{nullptr};
Controls::Button g_slotButton{nullptr};
FrameworkElement g_slotRoot{nullptr};
Controls::Grid g_slotParentGrid{nullptr};
std::atomic<bool> g_slotPointerOver;
std::atomic<bool> g_slotPointerPressed;
std::atomic<bool> g_slotRightPressed;
std::atomic<int> g_lastCommandId;
std::atomic<ULONGLONG> g_lastCommandTick;
bool g_hasAppliedContent;
bool g_lastAppliedVisible;
std::wstring g_lastAppliedTop;
std::wstring g_lastAppliedBottom;
std::wstring g_lastAppliedColor;

constexpr int kCmdShowFlyout = 1;
constexpr int kCmdShowMenu = 2;
constexpr int kCmdRefreshLocation = 3;
constexpr UINT WM_PT_UI_TICK = WM_APP + 1;
constexpr UINT WM_PT_HIDE_FLYOUT = WM_APP + 2;
constexpr UINT WM_PT_SETTINGS_CHANGED = WM_APP + 3;
constexpr UINT WM_PT_AFTER_INIT = WM_APP + 4;
constexpr UINT WM_PT_PLAY_SOUND = WM_APP + 5;

using WindowThreadAction = std::function<void()>;
bool RunFromWindowThread(HWND hWnd, WindowThreadAction action,
                         DWORD timeoutMs = kRunFromWindowTimeoutMs);
HWND FindCurrentProcessTaskbarWnd();
constexpr UINT WM_PT_SHUTDOWN = WM_APP + 6;


typedef LONG(WINAPI* RtlGetVersion_t)(PRTL_OSVERSIONINFOW);

bool DetectWin11() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        return false;
    }
    auto rtlGetVersion =
        reinterpret_cast<RtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion"));
    if (!rtlGetVersion) {
        return false;
    }
    RTL_OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof(info);
    if (rtlGetVersion(&info) != 0) {
        return false;
    }
    Wh_Log(L"OS %u.%u build %u", info.dwMajorVersion, info.dwMinorVersion,
           info.dwBuildNumber);
    return info.dwMajorVersion >= 10 && info.dwBuildNumber >= 22000;
}

ULONGLONG GetUtcNowMs() {
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER uli{};
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart / 10000;
}

SYSTEMTIME GetLocalNow() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    return st;
}

int DateKey(const SYSTEMTIME& st) {
    return st.wYear * 10000 + st.wMonth * 100 + st.wDay;
}

int DateKey(int y, int m, int d) {
    return y * 10000 + m * 100 + d;
}

bool SameDate(const SYSTEMTIME& a, int y, int m, int d) {
    return a.wYear == y && a.wMonth == m && a.wDay == d;
}

ULONGLONG SystemTimeToUnixSeconds(const SYSTEMTIME& st) {
    SYSTEMTIME utc{};
    if (!TzSpecificLocalTimeToSystemTime(nullptr, &st, &utc)) return 0;
    FILETIME ft{};
    if (!SystemTimeToFileTime(&utc, &ft)) return 0;
    ULARGE_INTEGER uli{};
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    constexpr ULONGLONG epoch = 116444736000000000ULL;
    if (uli.QuadPart < epoch) return 0;
    return (uli.QuadPart - epoch) / 10000000ULL;
}

ULONGLONG LocalNowUnixSeconds() {
    SYSTEMTIME now = GetLocalNow();
    return SystemTimeToUnixSeconds(now);
}

ULONGLONG PrayerUnixSeconds(const PrayerEntry& prayer) {
    return SystemTimeToUnixSeconds(prayer.time);
}

std::wstring Utf8ToWide(std::string_view value) {
    if (value.empty()) {
        return {};
    }
    int size = MultiByteToWideChar(CP_UTF8, 0, value.data(),
                                   static_cast<int>(value.size()), nullptr, 0);
    if (size <= 0) {
        std::wstring fallback;
        fallback.reserve(value.size());
        for (unsigned char ch : value) {
            fallback.push_back(static_cast<wchar_t>(ch));
        }
        return fallback;
    }
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.data(),
                        static_cast<int>(value.size()), result.data(), size);
    return result;
}

std::string WideToUtf8(std::wstring_view value) {
    if (value.empty()) {
        return {};
    }
    int size = WideCharToMultiByte(CP_UTF8, 0, value.data(),
                                   static_cast<int>(value.size()), nullptr, 0,
                                   nullptr, nullptr);
    if (size <= 0) {
        return {};
    }
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.data(),
                        static_cast<int>(value.size()), result.data(), size,
                        nullptr, nullptr);
    return result;
}

std::wstring Trim(std::wstring s) {
    auto notSpace = [](wchar_t c) { return !iswspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

std::wstring ToUpper(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), towupper);
    return s;
}

std::wstring NormalizeCountry(std::wstring country) {
    country = Trim(country);
    std::wstring result;
    result.reserve(country.size());
    for (wchar_t ch : country) {
        if (ch >= 0x0300 && ch <= 0x036F) {
            continue;
        }
        result.push_back(ch);
    }
    return ToUpper(result);
}

std::wstring GetAppDataPrayerTrayPath() {
    wchar_t appData[MAX_PATH]{};
    DWORD len = GetEnvironmentVariableW(L"APPDATA", appData, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return L"";
    }
    return std::wstring(appData) + L"\\PrayerTray";
}

std::wstring GetCacheFolder() {
    auto base = GetAppDataPrayerTrayPath();
    if (base.empty()) {
        return L"";
    }
    std::wstring path = base + L"\\cache";
    CreateDirectoryW(base.c_str(), nullptr);
    CreateDirectoryW(path.c_str(), nullptr);
    return path;
}

std::wstring SanitizeCachePart(std::wstring value) {
    value = Trim(value);
    std::wstring out;
    out.reserve(value.size());
    for (wchar_t ch : value) {
        if (ch == L'<' || ch == L'>' || ch == L':' || ch == L'"' ||
            ch == L'/' || ch == L'\\' || ch == L'|' || ch == L'?' ||
            ch == L'*' || iswspace(ch)) {
            out.push_back(L'_');
        } else {
            out.push_back(ch);
        }
    }
    return out.empty() ? L"unknown" : out;
}

std::wstring UrlEncodeUtf8(std::string_view value) {
    std::wstring encoded;
    for (unsigned char ch : value) {
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
            (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' ||
            ch == '.' || ch == '~') {
            encoded.push_back(static_cast<wchar_t>(ch));
        } else {
            wchar_t buf[8];
            swprintf(buf, 8, L"%%%02X", ch);
            encoded += buf;
        }
    }
    return encoded;
}

std::wstring UrlEncode(const std::wstring& value) {
    return UrlEncodeUtf8(WideToUtf8(value));
}

bool ParseDouble(const std::wstring& text, double& out) {
    const std::wstring clean = Trim(text);
    if (clean.empty()) {
        return false;
    }
    wchar_t* end = nullptr;
    errno = 0;
    out = wcstod(clean.c_str(), &end);
    return errno != ERANGE && end && end != clean.c_str() && *end == L'\0' &&
           std::isfinite(out);
}

ContentMode ParseContentMode(const std::wstring& mode) {
    if (mode == L"time") {
        return ContentMode::Time;
    }
    if (mode == L"countdown") {
        return ContentMode::Countdown;
    }
    return ContentMode::Smart;
}

void LoadSettings() {
    auto takeString = [](PCWSTR key) -> std::wstring {
        PCWSTR value = Wh_GetStringSetting(key);
        std::wstring result = value ? value : L"";
        if (value) {
            Wh_FreeStringSetting(value);
        }
        return result;
    };

    ModSettings s;
    s.showOnTaskbar = Wh_GetIntSetting(L"showOnTaskbar") != 0;
    s.playPrayerNotification = Wh_GetIntSetting(L"playPrayerNotification") != 0;
    s.useManualLocation = Wh_GetIntSetting(L"useManualLocation") != 0;
    s.manualCity = takeString(L"manualCity");
    s.manualCountry = takeString(L"manualCountry");
    s.manualLatitude = takeString(L"manualLatitude");
    s.manualLongitude = takeString(L"manualLongitude");
    s.calculationMethod = static_cast<int>(Wh_GetIntSetting(L"calculationMethod"));
    s.contentMode = ParseContentMode(takeString(L"taskbarContentMode"));
    s.use24Hour = takeString(L"timeFormat") == L"24h";
    s.language = takeString(L"language");
    if (s.language.empty()) {
        s.language = L"en";
    }
    s.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11") != 0;
    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(s);
}

ModSettings GetSettingsSnapshot() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

bool IsRtlLanguage() {
    const auto settings = GetSettingsSnapshot();
    return settings.language == L"ar" || settings.language == L"ur";
}

bool IsLightTheme() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                      0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }
    DWORD value = 0;
    DWORD size = sizeof(value);
    LSTATUS status = RegQueryValueExW(
        key, L"SystemUsesLightTheme", nullptr, nullptr,
        reinterpret_cast<LPBYTE>(&value), &size);
    if (status != ERROR_SUCCESS) {
        size = sizeof(value);
        RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                         reinterpret_cast<LPBYTE>(&value), &size);
    }
    RegCloseKey(key);
    return value == 1;
}

struct TaskbarPalette {
    COLORREF background = RGB(32, 32, 32);
    COLORREF foreground = RGB(255, 255, 255);
    COLORREF secondary = RGB(180, 180, 180);
    COLORREF highlight = RGB(50, 50, 50);
    COLORREF border = RGB(96, 96, 96);
};

int ColorLuminance(COLORREF color) {
    return (299 * GetRValue(color) + 587 * GetGValue(color) +
            114 * GetBValue(color)) /
           1000;
}

COLORREF BlendColor(COLORREF from, COLORREF to, double amount) {
    auto blend = [amount](BYTE a, BYTE b) {
        return static_cast<BYTE>(std::clamp(
            std::lround(a + (b - a) * amount), 0L, 255L));
    };
    return RGB(blend(GetRValue(from), GetRValue(to)),
               blend(GetGValue(from), GetGValue(to)),
               blend(GetBValue(from), GetBValue(to)));
}

std::optional<COLORREF> SampleTaskbarBackground() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    RECT rect{};
    if (!taskbar || !GetWindowRect(taskbar, &rect) ||
        rect.right <= rect.left || rect.bottom <= rect.top) {
        return std::nullopt;
    }

    HDC screen = GetDC(nullptr);
    if (!screen) return std::nullopt;

    std::vector<int> reds;
    std::vector<int> greens;
    std::vector<int> blues;
    auto sample = [&](int x, int y) {
        x = std::clamp(x, static_cast<int>(rect.left) + 2,
                       static_cast<int>(rect.right) - 3);
        y = std::clamp(y, static_cast<int>(rect.top) + 2,
                       static_cast<int>(rect.bottom) - 3);
        const COLORREF color = GetPixel(screen, x, y);
        if (color != CLR_INVALID) {
            reds.push_back(GetRValue(color));
            greens.push_back(GetGValue(color));
            blues.push_back(GetBValue(color));
        }
    };

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if (width >= height) {
        for (int xPercent : {3, 12, 25, 40, 60, 75, 88, 97}) {
            for (int yPercent : {25, 50, 75}) {
                sample(rect.left + width * xPercent / 100,
                       rect.top + height * yPercent / 100);
            }
        }
    } else {
        for (int yPercent : {3, 12, 25, 40, 60, 75, 88, 97}) {
            for (int xPercent : {25, 50, 75}) {
                sample(rect.left + width * xPercent / 100,
                       rect.top + height * yPercent / 100);
            }
        }
    }
    ReleaseDC(nullptr, screen);
    if (reds.empty()) return std::nullopt;

    auto median = [](std::vector<int>& values) {
        const auto middle = values.begin() + values.size() / 2;
        std::nth_element(values.begin(), middle, values.end());
        return *middle;
    };
    return RGB(median(reds), median(greens), median(blues));
}

TaskbarPalette GetTaskbarPalette() {
    static std::mutex cacheMutex;
    static TaskbarPalette cached;
    static ULONGLONG sampledAt = 0;
    std::lock_guard lock(cacheMutex);
    const ULONGLONG now = GetTickCount64();
    if (sampledAt && now - sampledAt < 750) return cached;

    TaskbarPalette palette;
    palette.background = SampleTaskbarBackground().value_or(
        IsLightTheme() ? RGB(252, 252, 252) : RGB(32, 32, 32));
    const bool light = ColorLuminance(palette.background) >= 150;
    palette.foreground = light ? RGB(26, 26, 26) : RGB(255, 255, 255);
    palette.secondary = BlendColor(palette.background, palette.foreground, 0.65);
    palette.highlight = BlendColor(palette.background, palette.foreground,
                                   light ? 0.09 : 0.18);
    palette.border = BlendColor(palette.background, palette.foreground,
                                light ? 0.22 : 0.30);
    cached = palette;
    sampledAt = now;
    return cached;
}

std::wstring TaskbarTextColorHex() {
    // Keep screen capture out of Explorer's startup and taskbar render loop.
    // The flyout samples the taskbar only after the user explicitly opens it.
    return IsLightTheme() ? L"1A1A1A" : L"FFFFFF";
}


using LocTable = std::unordered_map<std::wstring, std::wstring>;

const LocTable& GetLocTable() {
    static const std::unordered_map<std::wstring, LocTable> all = {
        {L"en",
         {{L"Prayer", L"Prayer"}, {L"Unavailable", L"Unavailable"},
          {L"UnknownLocation", L"Unknown location"}, {L"Detecting", L"Detecting..."},
          {L"TimePeriod_AM", L"AM"}, {L"TimePeriod_PM", L"PM"},
          {L"Prayer_Fajr", L"Fajr"}, {L"Prayer_Dhuhr", L"Dhuhr"},
          {L"Prayer_Asr", L"Asr"}, {L"Prayer_Maghrib", L"Maghrib"},
          {L"Prayer_Isha", L"Isha"}, {L"Tray_TodaySchedule", L"Today's schedule"},
          {L"Tray_RefreshLocation", L"Refresh location"},
          {L"Tray_Settings", L"Open Windhawk"},
          {L"Tray_ShowWidget", L"Show taskbar item"},
          {L"Tray_HideWidget", L"Hide taskbar item"},
          {L"PrayerTimes", L"Prayer times"}, {L"Location_Format", L"Location: {0}"},
          {L"Countdown_Seconds", L"{0}s"}, {L"Countdown_Minutes", L"{0}m"},
          {L"Countdown_Hours", L"{0}h"}, {L"Countdown_HoursMinutes", L"{0}h {1}m"}}},
        {L"ar",
         {{L"Prayer", L"صلاة"}, {L"Unavailable", L"غير متاح"},
          {L"UnknownLocation", L"موقع غير معروف"}, {L"Detecting", L"جارٍ تحديد الموقع..."},
          {L"TimePeriod_AM", L"ص"}, {L"TimePeriod_PM", L"م"},
          {L"Prayer_Fajr", L"الفجر"}, {L"Prayer_Dhuhr", L"الظهر"},
          {L"Prayer_Asr", L"العصر"}, {L"Prayer_Maghrib", L"المغرب"},
          {L"Prayer_Isha", L"العشاء"}, {L"Tray_TodaySchedule", L"جدول اليوم"},
          {L"Tray_RefreshLocation", L"تحديث الموقع"},
          {L"Tray_Settings", L"فتح Windhawk"},
          {L"Tray_ShowWidget", L"إظهار عنصر شريط المهام"},
          {L"Tray_HideWidget", L"إخفاء عنصر شريط المهام"},
          {L"PrayerTimes", L"مواقيت الصلاة"}, {L"Location_Format", L"الموقع: {0}"},
          {L"Countdown_Seconds", L"{0} ث"}, {L"Countdown_Minutes", L"{0} د"},
          {L"Countdown_Hours", L"{0} س"}, {L"Countdown_HoursMinutes", L"{0} س {1} د"}}},
        {L"fr",
         {{L"Prayer", L"Prière"}, {L"Unavailable", L"Indisponible"},
          {L"UnknownLocation", L"Position inconnue"},
          {L"Detecting", L"Recherche de la position..."},
          {L"TimePeriod_AM", L"AM"}, {L"TimePeriod_PM", L"PM"},
          {L"Prayer_Fajr", L"Fajr"}, {L"Prayer_Dhuhr", L"Dhuhr"},
          {L"Prayer_Asr", L"Asr"}, {L"Prayer_Maghrib", L"Maghrib"},
          {L"Prayer_Isha", L"Isha"}, {L"Tray_TodaySchedule", L"Horaire du jour"},
          {L"Tray_RefreshLocation", L"Actualiser la position"},
          {L"Tray_Settings", L"Ouvrir Windhawk"},
          {L"Tray_ShowWidget", L"Afficher dans la barre des tâches"},
          {L"Tray_HideWidget", L"Masquer dans la barre des tâches"},
          {L"PrayerTimes", L"Horaires de prière"}, {L"Location_Format", L"Position : {0}"},
          {L"Countdown_Seconds", L"{0}s"}, {L"Countdown_Minutes", L"{0}m"},
          {L"Countdown_Hours", L"{0}h"}, {L"Countdown_HoursMinutes", L"{0}h {1}m"}}},
        {L"ur",
         {{L"Prayer", L"نماز"}, {L"Unavailable", L"دستیاب نہیں"},
          {L"UnknownLocation", L"نامعلوم مقام"}, {L"Detecting", L"مقام تلاش ہو رہا ہے..."},
          {L"TimePeriod_AM", L"ق.ظ"}, {L"TimePeriod_PM", L"ب.ظ"},
          {L"Prayer_Fajr", L"فجر"}, {L"Prayer_Dhuhr", L"ظہر"},
          {L"Prayer_Asr", L"عصر"}, {L"Prayer_Maghrib", L"مغرب"},
          {L"Prayer_Isha", L"عشاء"}, {L"Tray_TodaySchedule", L"آج کا شیڈول"},
          {L"Tray_RefreshLocation", L"مقام تازہ کریں"},
          {L"Tray_Settings", L"Windhawk کھولیں"},
          {L"Tray_ShowWidget", L"ٹاسک بار آئٹم دکھائیں"},
          {L"Tray_HideWidget", L"ٹاسک بار آئٹم چھپائیں"},
          {L"PrayerTimes", L"نماز کے اوقات"}, {L"Location_Format", L"مقام: {0}"},
          {L"Countdown_Seconds", L"{0} س"}, {L"Countdown_Minutes", L"{0} م"},
          {L"Countdown_Hours", L"{0} گ"}, {L"Countdown_HoursMinutes", L"{0} گ {1} م"}}},
        {L"tr",
         {{L"Prayer", L"Namaz"}, {L"Unavailable", L"Kullanılamıyor"},
          {L"UnknownLocation", L"Bilinmeyen konum"}, {L"Detecting", L"Konum aranıyor..."},
          {L"TimePeriod_AM", L"ÖÖ"}, {L"TimePeriod_PM", L"ÖS"},
          {L"Prayer_Fajr", L"İmsak"}, {L"Prayer_Dhuhr", L"Öğle"},
          {L"Prayer_Asr", L"İkindi"}, {L"Prayer_Maghrib", L"Akşam"},
          {L"Prayer_Isha", L"Yatsı"}, {L"Tray_TodaySchedule", L"Bugünün programı"},
          {L"Tray_RefreshLocation", L"Konumu yenile"},
          {L"Tray_Settings", L"Windhawk'ı aç"},
          {L"Tray_ShowWidget", L"Görev çubuğu öğesini göster"},
          {L"Tray_HideWidget", L"Görev çubuğu öğesini gizle"},
          {L"PrayerTimes", L"Namaz vakitleri"}, {L"Location_Format", L"Konum: {0}"},
          {L"Countdown_Seconds", L"{0} sn"}, {L"Countdown_Minutes", L"{0} dk"},
          {L"Countdown_Hours", L"{0} sa"}, {L"Countdown_HoursMinutes", L"{0} sa {1} dk"}}},
        {L"id",
         {{L"Prayer", L"Shalat"}, {L"Unavailable", L"Tidak tersedia"},
          {L"UnknownLocation", L"Lokasi tidak diketahui"},
          {L"Detecting", L"Mencari lokasi..."},
          {L"TimePeriod_AM", L"AM"}, {L"TimePeriod_PM", L"PM"},
          {L"Prayer_Fajr", L"Subuh"}, {L"Prayer_Dhuhr", L"Dzuhur"},
          {L"Prayer_Asr", L"Ashar"}, {L"Prayer_Maghrib", L"Maghrib"},
          {L"Prayer_Isha", L"Isya"}, {L"Tray_TodaySchedule", L"Jadwal hari ini"},
          {L"Tray_RefreshLocation", L"Segarkan lokasi"},
          {L"Tray_Settings", L"Buka Windhawk"},
          {L"Tray_ShowWidget", L"Tampilkan item bilah tugas"},
          {L"Tray_HideWidget", L"Sembunyikan item bilah tugas"},
          {L"PrayerTimes", L"Jadwal shalat"}, {L"Location_Format", L"Lokasi: {0}"},
          {L"Countdown_Seconds", L"{0} d"}, {L"Countdown_Minutes", L"{0} m"},
          {L"Countdown_Hours", L"{0} j"}, {L"Countdown_HoursMinutes", L"{0} j {1} m"}}},
    };
    std::wstring lang = GetSettingsSnapshot().language;
    for (auto& ch : lang) {
        if (ch >= L'A' && ch <= L'Z') {
            ch = static_cast<wchar_t>(ch - L'A' + L'a');
        }
    }
    auto it = all.find(lang);
    if (it != all.end()) {
        return it->second;
    }
    return all.at(L"en");
}

std::wstring Loc(const std::wstring& key) {
    const auto& table = GetLocTable();
    auto it = table.find(key);
    return it != table.end() ? it->second : key;
}

std::wstring LocFormat(const std::wstring& key, int a, int b = 0) {
    std::wstring fmt = Loc(key);
    size_t pos = fmt.find(L"{0}");
    if (pos != std::wstring::npos) {
        fmt.replace(pos, 3, std::to_wstring(a));
    }
    pos = fmt.find(L"{1}");
    if (pos != std::wstring::npos) {
        fmt.replace(pos, 3, std::to_wstring(b));
    }
    return fmt;
}

const wchar_t* PrayerNameKey(PrayerName name) {
    switch (name) {
        case PrayerName::Fajr:
            return L"Prayer_Fajr";
        case PrayerName::Dhuhr:
            return L"Prayer_Dhuhr";
        case PrayerName::Asr:
            return L"Prayer_Asr";
        case PrayerName::Maghrib:
            return L"Prayer_Maghrib";
        case PrayerName::Isha:
            return L"Prayer_Isha";
        default:
            return L"Prayer";
    }
}

const wchar_t* PrayerApiKey(PrayerName name) {
    switch (name) {
        case PrayerName::Fajr:
            return L"Fajr";
        case PrayerName::Dhuhr:
            return L"Dhuhr";
        case PrayerName::Asr:
            return L"Asr";
        case PrayerName::Maghrib:
            return L"Maghrib";
        case PrayerName::Isha:
            return L"Isha";
        default:
            return L"";
    }
}

std::wstring GetPrayerName(PrayerName name) {
    return Loc(PrayerNameKey(name));
}

std::wstring StabilizeCompactText(const std::wstring& text) {
    if (IsRtlLanguage()) {
        return std::wstring(1, kLre) + text + kPdf;
    }
    return text;
}

std::wstring FormatTime(const SYSTEMTIME& time) {
    const auto settings = GetSettingsSnapshot();
    std::wstring numericTime;
    if (settings.use24Hour) {
        wchar_t buf[16];
        swprintf(buf, 16, L"%02u%c%02u", time.wHour, kTimeColon, time.wMinute);
        numericTime = buf;
    } else {
        int hour = time.wHour % 12;
        if (hour == 0) {
            hour = 12;
        }
        const std::wstring period =
            time.wHour < 12 ? Loc(L"TimePeriod_AM") : Loc(L"TimePeriod_PM");
        wchar_t buf[16];
        swprintf(buf, 16, L"%d%c%02u", hour, kTimeColon, time.wMinute);
        numericTime = buf;
        if (IsRtlLanguage()) {
            // Isolate only hour:minute as LTR. Keeping the Arabic/Urdu period
            // outside the embedding prevents GDI from reversing 4:23 to 23:4.
            return std::wstring(1, kLre) + numericTime + kPdf + L" " + period;
        }
        return numericTime + L" " + period;
    }
    return IsRtlLanguage()
               ? std::wstring(1, kLre) + numericTime + kPdf
               : numericTime;
}

std::wstring FormatCountdown(ULONGLONG seconds) {
    std::wstring text;
    if (seconds < 60) {
        int sec = static_cast<int>(seconds);
        text = LocFormat(L"Countdown_Seconds", sec);
    } else if (seconds < 3600) {
        text = LocFormat(L"Countdown_Minutes", static_cast<int>(seconds / 60));
    } else {
        int hours = static_cast<int>(seconds / 3600);
        int mins = static_cast<int>((seconds % 3600) / 60);
        text = mins == 0 ? LocFormat(L"Countdown_Hours", hours)
                         : LocFormat(L"Countdown_HoursMinutes", hours, mins);
    }
    return StabilizeCompactText(text);
}

std::wstring LocationDisplayName(const LocationInfo& loc) {
    if (!loc.city.empty() && !loc.country.empty()) {
        return loc.city + L", " + loc.country;
    }
    if (!loc.city.empty()) {
        return loc.city;
    }
    if (!loc.country.empty()) {
        return loc.country;
    }
    if (!loc.region.empty()) {
        return loc.region;
    }
    return Loc(L"UnknownLocation");
}


struct WinHttpHandle {
    HINTERNET value = nullptr;
    WinHttpHandle() = default;
    explicit WinHttpHandle(HINTERNET value) : value(value) {}
    ~WinHttpHandle() {
        if (value) WinHttpCloseHandle(value);
    }
    WinHttpHandle(const WinHttpHandle&) = delete;
    WinHttpHandle& operator=(const WinHttpHandle&) = delete;
    operator HINTERNET() const { return value; }
};

bool StopRequested() {
    return g_unloading.load() ||
           (g_stopEvent && WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0);
}

std::optional<std::string> HttpGetUtf8(const std::wstring& url,
                                       size_t maxBytes = 1024 * 1024) {
    if (StopRequested()) return std::nullopt;
    URL_COMPONENTS uc{};
    uc.dwStructSize = sizeof(uc);
    wchar_t host[256]{};
    wchar_t path[2048]{};
    wchar_t extra[1024]{};
    uc.lpszHostName = host;
    uc.dwHostNameLength = ARRAYSIZE(host);
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = ARRAYSIZE(path);
    uc.lpszExtraInfo = extra;
    uc.dwExtraInfoLength = ARRAYSIZE(extra);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &uc)) {
        return std::nullopt;
    }
    if (uc.nScheme != INTERNET_SCHEME_HTTPS) {
        Wh_Log(L"Blocked non-HTTPS request");
        return std::nullopt;
    }
    WinHttpHandle session(WinHttpOpen(
        L"PrayerTray/3.1.4 (+https://github.com/n0tmar/PrayerTray)",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session) {
        return std::nullopt;
    }
    WinHttpSetTimeouts(session, 1500, 1500, 3000, 3000);
    WinHttpHandle connect(WinHttpConnect(session, host, uc.nPort, 0));
    if (!connect) {
        return std::nullopt;
    }
    std::wstring objectName = std::wstring(path) + extra;
    WinHttpHandle request(WinHttpOpenRequest(
        connect, L"GET", objectName.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE));
    if (!request) {
        return std::nullopt;
    }
    const wchar_t headers[] = L"Accept: */*\r\n";
    if (!WinHttpSendRequest(request, headers, static_cast<DWORD>(-1L),
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request, nullptr)) {
        Wh_Log(L"HTTPS request failed for %s (%u)", host, GetLastError());
        return std::nullopt;
    }
    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    if (!WinHttpQueryHeaders(request,
                             WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                             WINHTTP_NO_HEADER_INDEX) ||
        status < 200 || status >= 300) {
        Wh_Log(L"HTTP %u for %s", status, host);
        return std::nullopt;
    }
    std::string body;
    DWORD available = 0;
    do {
        if (!WinHttpQueryDataAvailable(request, &available)) {
            return std::nullopt;
        }
        if (available == 0) {
            break;
        }
        if (available > maxBytes || body.size() > maxBytes - available) {
            Wh_Log(L"HTTP response too large from %s", host);
            return std::nullopt;
        }
        size_t offset = body.size();
        body.resize(offset + available);
        DWORD read = 0;
        if (!WinHttpReadData(request, body.data() + offset, available, &read)) {
            body.resize(offset);
            return std::nullopt;
        }
        body.resize(offset + read);
    } while (available > 0);
    return body.empty() ? std::nullopt : std::optional<std::string>{std::move(body)};
}

std::optional<winrt::Windows::Data::Json::JsonObject> ParseJsonObject(
    std::string_view json) {
    try {
        return winrt::Windows::Data::Json::JsonObject::Parse(Utf8ToWide(json));
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<double> JsonGetDouble(std::string_view json, std::string_view key) {
    auto object = ParseJsonObject(json);
    if (!object) return std::nullopt;
    try {
        const auto name = Utf8ToWide(key);
        auto value = object->GetNamedValue(name);
        if (value.ValueType() == winrt::Windows::Data::Json::JsonValueType::Number)
            return value.GetNumber();
        if (value.ValueType() == winrt::Windows::Data::Json::JsonValueType::String) {
            double parsed = 0;
            if (ParseDouble(value.GetString().c_str(), parsed)) return parsed;
        }
    } catch (...) {
    }
    return std::nullopt;
}

std::wstring JsonGetStringW(std::string_view json, std::string_view key) {
    auto object = ParseJsonObject(json);
    if (!object) return L"";
    try {
        auto value = object->GetNamedValue(Utf8ToWide(key));
        if (value.ValueType() == winrt::Windows::Data::Json::JsonValueType::String)
            return value.GetString().c_str();
    } catch (...) {
    }
    return L"";
}

std::optional<bool> JsonGetBool(std::string_view json, std::string_view key) {
    auto object = ParseJsonObject(json);
    if (!object) return std::nullopt;
    try {
        auto value = object->GetNamedValue(Utf8ToWide(key));
        if (value.ValueType() == winrt::Windows::Data::Json::JsonValueType::Boolean)
            return value.GetBoolean();
    } catch (...) {
    }
    return std::nullopt;
}

std::optional<std::string> JsonFindObject(std::string_view json,
                                          std::string_view key) {
    auto object = ParseJsonObject(json);
    if (!object) return std::nullopt;
    try {
        auto child = object->GetNamedObject(Utf8ToWide(key));
        return WideToUtf8(child.Stringify().c_str());
    } catch (...) {
        return std::nullopt;
    }
}

std::wstring JsonGetFirstArrayString(std::string_view json,
                                     std::string_view key) {
    auto object = ParseJsonObject(json);
    if (!object) return L"";
    try {
        auto array = object->GetNamedArray(Utf8ToWide(key));
        if (array.Size() &&
            array.GetAt(0).ValueType() ==
                winrt::Windows::Data::Json::JsonValueType::String) {
            return array.GetStringAt(0).c_str();
        }
    } catch (...) {
    }
    return L"";
}

int GetDefaultMethodForCountry(const std::wstring& country) {
    const auto n = NormalizeCountry(country);
    if (n == L"UNITED STATES" || n == L"USA" || n == L"CANADA") return 2;
    if (n == L"SAUDI ARABIA") return 4;
    if (n == L"EGYPT") return 5;
    if (n == L"IRAN") return 7;
    if (n == L"KUWAIT") return 9;
    if (n == L"QATAR") return 10;
    if (n == L"UNITED ARAB EMIRATES" || n == L"UAE") return 8;
    if (n == L"TURKEY" || n == L"TURKIYE") return 13;
    if (n == L"INDONESIA") return 20;
    if (n == L"MALAYSIA") return 17;
    if (n == L"PAKISTAN" || n == L"BANGLADESH" || n == L"INDIA") return 1;
    return 3;
}

std::wstring GetCountryCode(const std::wstring& country) {
    const auto n = NormalizeCountry(country);
    if (n == L"SAUDI ARABIA") return L"SA";
    if (n == L"UNITED ARAB EMIRATES" || n == L"UAE") return L"AE";
    if (n == L"EGYPT") return L"EG";
    if (n == L"UNITED STATES" || n == L"USA") return L"US";
    if (n == L"UNITED KINGDOM") return L"GB";
    if (n == L"PAKISTAN") return L"PK";
    if (n == L"INDIA") return L"IN";
    if (n == L"TURKEY" || n == L"TURKIYE") return L"TR";
    if (n == L"INDONESIA") return L"ID";
    if (n == L"MALAYSIA") return L"MY";
    if (n == L"KUWAIT") return L"KW";
    if (n == L"QATAR") return L"QA";
    if (n == L"CANADA") return L"CA";
    if (n == L"BANGLADESH") return L"BD";
    if (n == L"IRAN") return L"IR";
    return L"";
}

std::wstring GetDefaultTimezoneForCountry(const std::wstring& country) {
    const auto n = NormalizeCountry(country);
    if (n == L"SAUDI ARABIA") return L"Asia/Riyadh";
    if (n == L"UNITED ARAB EMIRATES" || n == L"UAE") return L"Asia/Dubai";
    if (n == L"EGYPT") return L"Africa/Cairo";
    if (n == L"KUWAIT") return L"Asia/Kuwait";
    if (n == L"QATAR") return L"Asia/Qatar";
    if (n == L"TURKEY" || n == L"TURKIYE") return L"Europe/Istanbul";
    if (n == L"INDONESIA") return L"Asia/Jakarta";
    if (n == L"MALAYSIA") return L"Asia/Kuala_Lumpur";
    if (n == L"PAKISTAN") return L"Asia/Karachi";
    if (n == L"INDIA") return L"Asia/Kolkata";
    if (n == L"BANGLADESH") return L"Asia/Dhaka";
    if (n == L"IRAN") return L"Asia/Tehran";
    return L"";
}

int ResolveCalculationMethod(const LocationInfo& loc,
                             const ModSettings& settings) {
    if (settings.calculationMethod > 0) {
        return settings.calculationMethod;
    }
    return GetDefaultMethodForCountry(loc.country);
}

double DistanceKm(const IpCandidate& a, const IpCandidate& b) {
    const double earthRadiusKm = 6371.0;
    auto deg2rad = [](double d) { return d * 3.141592653589793 / 180.0; };
    double lat1 = deg2rad(a.latitude);
    double lat2 = deg2rad(b.latitude);
    double dLat = deg2rad(b.latitude - a.latitude);
    double dLon = deg2rad(b.longitude - a.longitude);
    double sinLat = sin(dLat / 2);
    double sinLon = sin(dLon / 2);
    double h = sinLat * sinLat + cos(lat1) * cos(lat2) * sinLon * sinLon;
    h = (std::clamp)(h, 0.0, 1.0);
    return earthRadiusKm * 2 * atan2(sqrt(h), sqrt(1 - h));
}

bool IsValidCoordinate(double latitude, double longitude) {
    return std::isfinite(latitude) && std::isfinite(longitude) &&
           latitude >= -90.0 && latitude <= 90.0 && longitude >= -180.0 &&
           longitude <= 180.0 && !(latitude == 0.0 && longitude == 0.0);
}

std::optional<IpCandidate> ValidateCandidate(IpCandidate candidate) {
    if (!IsValidCoordinate(candidate.latitude, candidate.longitude)) {
        return std::nullopt;
    }
    return candidate;
}

std::wstring ReadCountryNameFromCode(const std::wstring& code) {
    const auto c = ToUpper(code);
    if (c == L"SA") return L"Saudi Arabia";
    if (c == L"AE") return L"United Arab Emirates";
    if (c == L"EG") return L"Egypt";
    if (c == L"US") return L"United States";
    if (c == L"GB") return L"United Kingdom";
    if (c == L"PK") return L"Pakistan";
    if (c == L"IN") return L"India";
    if (c == L"TR") return L"Turkey";
    if (c == L"ID") return L"Indonesia";
    if (c == L"MY") return L"Malaysia";
    if (c == L"KW") return L"Kuwait";
    if (c == L"QA") return L"Qatar";
    if (c == L"CA") return L"Canada";
    if (c == L"BD") return L"Bangladesh";
    if (c == L"IR") return L"Iran";
    return code;
}

std::optional<IpCandidate> TryFetchIpInfo() {
    auto body = HttpGetUtf8(L"https://ipinfo.io/json");
    if (!body) return std::nullopt;
    auto loc = JsonGetStringW(*body, "loc");
    if (loc.empty()) return std::nullopt;
    auto comma = loc.find(L',');
    if (comma == std::wstring::npos) return std::nullopt;
    IpCandidate c;
    c.provider = L"ipinfo";
    c.weight = 3;
    if (!ParseDouble(loc.substr(0, comma), c.latitude) ||
        !ParseDouble(loc.substr(comma + 1), c.longitude)) {
        return std::nullopt;
    }
    c.city = JsonGetStringW(*body, "city");
    c.region = JsonGetStringW(*body, "region");
    c.country = ReadCountryNameFromCode(JsonGetStringW(*body, "country"));
    c.timezone = JsonGetStringW(*body, "timezone");
    return ValidateCandidate(std::move(c));
}

std::optional<IpCandidate> TryFetchIpWho() {
    auto body = HttpGetUtf8(L"https://ipwho.is/");
    if (!body) return std::nullopt;
    if (!JsonGetBool(*body, "success").value_or(false)) return std::nullopt;
    IpCandidate c;
    c.provider = L"ipwho";
    c.weight = 1;
    c.latitude = JsonGetDouble(*body, "latitude").value_or(0);
    c.longitude = JsonGetDouble(*body, "longitude").value_or(0);
    c.city = JsonGetStringW(*body, "city");
    c.region = JsonGetStringW(*body, "region");
    c.country = JsonGetStringW(*body, "country");
    if (auto tzObj = JsonFindObject(*body, "timezone")) {
        c.timezone = JsonGetStringW(*tzObj, "id");
    }
    return ValidateCandidate(std::move(c));
}

IpCandidate SelectBestCandidate(std::vector<IpCandidate>& candidates) {
    std::vector<std::vector<size_t>> clusters;
    for (size_t i = 0; i < candidates.size(); ++i) {
        bool matched = false;
        for (auto& cluster : clusters) {
            if (DistanceKm(candidates[i], candidates[cluster[0]]) <= kClusterDistanceKm) {
                cluster.push_back(i);
                matched = true;
                break;
            }
        }
        if (!matched) {
            clusters.push_back({i});
        }
    }

    auto clusterTightness = [&](const std::vector<size_t>& cluster) {
        double maxDistance = 0;
        for (size_t i = 0; i < cluster.size(); ++i) {
            for (size_t j = i + 1; j < cluster.size(); ++j) {
                maxDistance = (std::max)(
                    maxDistance, DistanceKm(candidates[cluster[i]], candidates[cluster[j]]));
            }
        }
        return maxDistance;
    };

    auto best = clusters.begin();
    for (auto it = clusters.begin(); it != clusters.end(); ++it) {
        if (it->size() > best->size()) {
            best = it;
            continue;
        }
        if (it->size() < best->size()) {
            continue;
        }
        const double tightA = clusterTightness(*it);
        const double tightB = clusterTightness(*best);
        if (tightA < tightB) {
            best = it;
            continue;
        }
        if (tightA > tightB) {
            continue;
        }
        int weightA = 0, weightB = 0;
        for (size_t idx : *it) weightA += candidates[idx].weight;
        for (size_t idx : *best) weightB += candidates[idx].weight;
        if (weightA > weightB) best = it;
    }

    double lat = 0, lon = 0;
    int weight = 0;
    std::wstring providers;
    const IpCandidate* bestNamed = &candidates[(*best)[0]];
    for (size_t idx : *best) {
        lat += candidates[idx].latitude;
        lon += candidates[idx].longitude;
        weight += candidates[idx].weight;
        if (candidates[idx].weight > bestNamed->weight) {
            bestNamed = &candidates[idx];
        }
        if (!providers.empty()) providers += L"+";
        providers += candidates[idx].provider;
    }
    lat /= best->size();
    lon /= best->size();
    IpCandidate result;
    result.provider = providers;
    result.weight = weight;
    result.latitude = lat;
    result.longitude = lon;
    // FillFromCoords prefers reverse-geocode names and retains this as fallback.
    result.city = bestNamed->city;
    result.region = bestNamed->region;
    result.country = bestNamed->country;
    result.timezone = bestNamed->timezone;
    return result;
}

std::optional<IpCandidate> TryFetchFreeIpApi() {
    auto body = HttpGetUtf8(L"https://free.freeipapi.com/api/json");
    if (!body) return std::nullopt;
    IpCandidate c;
    c.provider = L"freeipapi";
    c.weight = 2;
    c.latitude = JsonGetDouble(*body, "latitude").value_or(0);
    c.longitude = JsonGetDouble(*body, "longitude").value_or(0);
    if (c.latitude == 0 && c.longitude == 0) return std::nullopt;
    c.city = JsonGetStringW(*body, "cityName");
    c.region = JsonGetStringW(*body, "regionName");
    c.country = JsonGetStringW(*body, "countryName");
    c.timezone = JsonGetFirstArrayString(*body, "timeZones");
    return ValidateCandidate(std::move(c));
}

std::optional<IpCandidate> TryFetchGeoJs() {
    auto body = HttpGetUtf8(L"https://get.geojs.io/v1/ip/geo.json");
    if (!body) return std::nullopt;
    IpCandidate c;
    c.provider = L"geojs";
    c.weight = 3;
    c.latitude = JsonGetDouble(*body, "latitude").value_or(0);
    c.longitude = JsonGetDouble(*body, "longitude").value_or(0);
    if (c.latitude == 0 && c.longitude == 0) {
        // geojs sometimes returns strings
        auto latStr = JsonGetStringW(*body, "latitude");
        auto lonStr = JsonGetStringW(*body, "longitude");
        double lat = 0, lon = 0;
        if (!ParseDouble(latStr, lat) || !ParseDouble(lonStr, lon)) return std::nullopt;
        c.latitude = lat;
        c.longitude = lon;
    }
    c.city = JsonGetStringW(*body, "city");
    c.region = JsonGetStringW(*body, "region");
    c.country = JsonGetStringW(*body, "country");
    return ValidateCandidate(std::move(c));
}

struct ReverseGeocodeResult {
    std::wstring city;
    std::wstring region;
    std::wstring country;
    std::wstring timezone;
};

std::wstring FixCityName(std::wstring city) {
    auto upper = ToUpper(city);
    if (upper == L"MAKKAH" || upper == L"MAKKAH AL MUKARRAMAH" ||
        upper == L"MECCA" || upper == L"MEKKAH") {
        return L"Makkah";
    }
    if (upper == L"AL TAIF" || upper == L"AT TAIF" || upper == L"TA'IF") {
        return L"Taif";
    }
    return city;
}

std::optional<ReverseGeocodeResult> TryReverseGeocode(double latitude, double longitude) {
    wchar_t url[512];
    swprintf(url, 512,
             L"https://api.bigdatacloud.net/data/reverse-geocode-client?latitude=%.6f&longitude=%.6f&localityLanguage=en",
             latitude, longitude);
    auto body = HttpGetUtf8(url);
    if (!body) return std::nullopt;

    ReverseGeocodeResult result;
    result.city = JsonGetStringW(*body, "locality");
    if (result.city.empty()) {
        result.city = JsonGetStringW(*body, "city");
    }
    result.region = JsonGetStringW(*body, "principalSubdivision");
    result.country = JsonGetStringW(*body, "countryName");
    result.timezone = JsonGetStringW(*body, "ianaTimeId");
    if (result.city.empty() && result.country.empty()) {
        return std::nullopt;
    }
    result.city = FixCityName(result.city);
    return result;
}

std::wstring FirstNonEmpty(std::initializer_list<std::wstring> values) {
    for (const auto& value : values) {
        if (!Trim(value).empty()) {
            return value;
        }
    }
    return L"";
}

LocationInfo ResolveManualLocation() {
    const auto settings = GetSettingsSnapshot();
    LocationInfo loc;
    loc.source = L"manual";
    loc.resolvedAtUtcMs = GetUtcNowMs();
    loc.city = FixCityName(Trim(settings.manualCity));
    loc.country = Trim(settings.manualCountry);
    loc.timezone = GetDefaultTimezoneForCountry(loc.country);
    double lat = 0, lon = 0;
    const bool hasLatitude = !Trim(settings.manualLatitude).empty();
    const bool hasLongitude = !Trim(settings.manualLongitude).empty();
    if (hasLatitude || hasLongitude) {
        if (!ParseDouble(settings.manualLatitude, lat) ||
            !ParseDouble(settings.manualLongitude, lon) || lat < -90.0 ||
            lat > 90.0 || lon < -180.0 || lon > 180.0) {
            throw std::runtime_error("manual coordinates are invalid");
        }
        loc.latitude = lat;
        loc.longitude = lon;
        return loc;
    }
    if (!loc.city.empty() && !loc.country.empty()) {
        loc.source = L"manual-city";
        return loc;
    }
    throw std::runtime_error("manual city and country are required");
}

std::wstring JsonEscape(const std::wstring& value) {
    std::wstring out;
    out.reserve(value.size());
    for (wchar_t ch : value) {
        switch (ch) {
            case L'\\': out += L"\\\\"; break;
            case L'"': out += L"\\\""; break;
            case L'\b': out += L"\\b"; break;
            case L'\f': out += L"\\f"; break;
            case L'\n': out += L"\\n"; break;
            case L'\r': out += L"\\r"; break;
            case L'\t': out += L"\\t"; break;
            default:
                if (ch < 0x20) {
                    wchar_t escaped[7];
                    swprintf(escaped, ARRAYSIZE(escaped), L"\\u%04X",
                             static_cast<unsigned>(ch));
                    out += escaped;
                } else {
                    out.push_back(ch);
                }
                break;
        }
    }
    return out;
}

std::wstring GetLastLocationPath() {
    auto base = GetAppDataPrayerTrayPath();
    if (base.empty()) {
        return L"";
    }
    CreateDirectoryW(base.c_str(), nullptr);
    return base + L"\\last-location.json";
}

bool WriteTextFileAtomically(const std::wstring& path, const std::string& content) {
    if (path.empty()) return false;
    const std::wstring temp = path + L".tmp." + std::to_wstring(GetCurrentProcessId()) +
                              L"." + std::to_wstring(GetCurrentThreadId());
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    const bool wrote = content.size() <= MAXDWORD &&
                       WriteFile(file, content.data(),
                                 static_cast<DWORD>(content.size()), &written,
                                 nullptr) &&
                       written == content.size() && FlushFileBuffers(file);
    CloseHandle(file);
    if (!wrote || !MoveFileExW(temp.c_str(), path.c_str(),
                               MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(temp.c_str());
        return false;
    }
    return true;
}

std::optional<std::string> ReadTextFile(const std::wstring& path,
                                        size_t maxBytes = 1024 * 1024) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) return std::nullopt;
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 ||
        static_cast<ULONGLONG>(size.QuadPart) > maxBytes ||
        size.QuadPart > MAXDWORD) {
        CloseHandle(file);
        return std::nullopt;
    }
    std::string data(static_cast<size_t>(size.QuadPart), '\0');
    DWORD bytesRead = 0;
    const bool read = ReadFile(file, data.data(),
                               static_cast<DWORD>(data.size()), &bytesRead,
                               nullptr) &&
                      bytesRead == data.size();
    CloseHandle(file);
    if (!read) {
        return std::nullopt;
    }
    return data;
}

void SaveLastLocation(const LocationInfo& loc) {
    const auto path = GetLastLocationPath();
    if (path.empty() || (loc.latitude == 0 && loc.longitude == 0 && loc.city.empty())) {
        return;
    }
    std::ostringstream out;
    out << std::setprecision(15);
    out << "{\n";
    out << "  \"Latitude\": " << loc.latitude << ",\n";
    out << "  \"Longitude\": " << loc.longitude << ",\n";
    out << "  \"City\": \"" << WideToUtf8(JsonEscape(loc.city)) << "\",\n";
    out << "  \"Region\": \"" << WideToUtf8(JsonEscape(loc.region)) << "\",\n";
    out << "  \"Country\": \"" << WideToUtf8(JsonEscape(loc.country)) << "\",\n";
    out << "  \"Timezone\": \"" << WideToUtf8(JsonEscape(loc.timezone)) << "\",\n";
    out << "  \"Source\": \"" << WideToUtf8(JsonEscape(loc.source)) << "\",\n";
    out << "  \"ResolvedAtUtcMs\": " << loc.resolvedAtUtcMs << "\n";
    out << "}\n";
    if (!WriteTextFileAtomically(path, out.str())) {
        Wh_Log(L"Failed to save location cache");
    }
}

std::optional<LocationInfo> LoadLastLocation() {
    const auto path = GetLastLocationPath();
    if (path.empty()) {
        return std::nullopt;
    }
    auto contents = ReadTextFile(path, 64 * 1024);
    if (!contents) return std::nullopt;
    const std::string& json = *contents;
    LocationInfo loc;
    loc.latitude = JsonGetDouble(json, "Latitude").value_or(0);
    loc.longitude = JsonGetDouble(json, "Longitude").value_or(0);
    loc.city = FixCityName(JsonGetStringW(json, "City"));
    loc.region = JsonGetStringW(json, "Region");
    loc.country = JsonGetStringW(json, "Country");
    loc.timezone = JsonGetStringW(json, "Timezone");
    loc.source = JsonGetStringW(json, "Source");
    if (loc.source.empty()) loc.source = L"last-known";
    loc.resolvedAtUtcMs = static_cast<ULONGLONG>(
        JsonGetDouble(json, "ResolvedAtUtcMs").value_or(0));
    if (loc.latitude == 0 && loc.longitude == 0 && loc.city.empty()) {
        return std::nullopt;
    }
    return loc;
}

LocationInfo FillFromCoords(LocationInfo loc,
                                      double latitude,
                                      double longitude,
                                      const std::wstring& source,
                                      const std::wstring& fallbackCity = L"",
                                      const std::wstring& fallbackRegion = L"",
                                      const std::wstring& fallbackCountry = L"",
                                      const std::wstring& fallbackTimezone = L"") {
    loc.latitude = latitude;
    loc.longitude = longitude;
    loc.source = source;
    loc.resolvedAtUtcMs = GetUtcNowMs();

    if (auto reverse = TryReverseGeocode(latitude, longitude)) {
        loc.city = FirstNonEmpty({reverse->city, fallbackCity});
        loc.region = FirstNonEmpty({reverse->region, fallbackRegion});
        loc.country = FirstNonEmpty({reverse->country, fallbackCountry});
        loc.timezone = FirstNonEmpty({reverse->timezone, fallbackTimezone});
    } else {
        loc.city = fallbackCity;
        loc.region = fallbackRegion;
        loc.country = fallbackCountry;
        loc.timezone = fallbackTimezone;
    }
    if (loc.timezone.empty()) {
        loc.timezone = GetDefaultTimezoneForCountry(loc.country);
    }
    loc.city = FixCityName(loc.city);
    return loc;
}

LocationInfo TryIpLocation() {
    Wh_Log(L"Trying IP location");
    std::vector<std::future<std::optional<IpCandidate>>> jobs;
    jobs.push_back(std::async(std::launch::async, TryFetchIpInfo));
    jobs.push_back(std::async(std::launch::async, TryFetchIpWho));
    jobs.push_back(std::async(std::launch::async, TryFetchFreeIpApi));
    jobs.push_back(std::async(std::launch::async, TryFetchGeoJs));

    std::vector<IpCandidate> candidates;
    for (auto& job : jobs) {
        try {
            if (auto c = job.get()) {
                candidates.push_back(*c);
            }
        } catch (...) {
        }
    }
    if (candidates.empty()) {
        throw std::runtime_error("all IP providers failed");
    }
    auto winner = SelectBestCandidate(candidates);
    LocationInfo loc = FillFromCoords(
        {}, winner.latitude, winner.longitude, L"ip", winner.city, winner.region,
        winner.country, winner.timezone);
    Wh_Log(L"Location (ip): %s, %s [%s]", loc.city.c_str(),
           loc.country.c_str(), winner.provider.c_str());
    return loc;
}

LocationInfo ResolveLocation() {
    const auto settings = GetSettingsSnapshot();
    if (settings.useManualLocation) {
        auto loc = ResolveManualLocation();
        SaveLastLocation(loc);
        return loc;
    }

    // Prefer a known location immediately; refresh can replace it later.
    auto cachedLoc = LoadLastLocation();
    if (cachedLoc && cachedLoc->source.starts_with(L"manual")) {
        cachedLoc.reset();
    }

    try {
        auto loc = TryIpLocation();
        SaveLastLocation(loc);
        return loc;
    } catch (const std::exception& ex) {
        Wh_Log(L"IP location failed: %S", ex.what());
    } catch (...) {
        Wh_Log(L"IP location failed");
    }

    if (cachedLoc) {
        Wh_Log(L"Location (cached): %s, %s", cachedLoc->city.c_str(),
               cachedLoc->country.c_str());
        return *cachedLoc;
    }

    throw std::runtime_error("location unavailable");
}

bool ParseTimeOnly(const std::wstring& text, SYSTEMTIME& outDate, SYSTEMTIME& outTime) {
    std::wstring clean = text;
    auto space = clean.find(L' ');
    if (space != std::wstring::npos) {
        clean = clean.substr(0, space);
    }
    auto colon = clean.find(L':');
    if (colon == std::wstring::npos) return false;
    const auto secondColon = clean.find(L':', colon + 1);
    const std::wstring minuteText =
        secondColon == std::wstring::npos
            ? clean.substr(colon + 1)
            : clean.substr(colon + 1, secondColon - colon - 1);
    double hourValue = 0;
    double minuteValue = 0;
    if (!ParseDouble(clean.substr(0, colon), hourValue) ||
        !ParseDouble(minuteText, minuteValue) ||
        std::floor(hourValue) != hourValue ||
        std::floor(minuteValue) != minuteValue || hourValue < 0 ||
        hourValue > 23 || minuteValue < 0 || minuteValue > 59) {
        return false;
    }
    if (secondColon != std::wstring::npos) {
        double seconds = 0;
        if (!ParseDouble(clean.substr(secondColon + 1), seconds) ||
            std::floor(seconds) != seconds || seconds < 0 || seconds > 59) {
            return false;
        }
    }
    outTime = outDate;
    outTime.wHour = static_cast<WORD>(hourValue);
    outTime.wMinute = static_cast<WORD>(minuteValue);
    outTime.wSecond = 0;
    outTime.wMilliseconds = 0;
    return true;
}

PrayerSchedule BuildScheduleFromTimingsJson(std::string_view timingsJson,
                                            int year, int month, int day,
                                            LocationInfo location,
                                            const std::wstring& timezone) {
    PrayerSchedule schedule;
    schedule.year = year;
    schedule.month = month;
    schedule.day = day;
    schedule.location = location;
    schedule.fetchedAtUtcMs = GetUtcNowMs();
    if (!timezone.empty()) {
        schedule.location.timezone = timezone;
    }
    SYSTEMTIME date{};
    date.wYear = static_cast<WORD>(year);
    date.wMonth = static_cast<WORD>(month);
    date.wDay = static_cast<WORD>(day);
    for (int i = 0; i < static_cast<int>(PrayerName::Count); ++i) {
        auto name = static_cast<PrayerName>(i);
        std::string key = WideToUtf8(PrayerApiKey(name));
        auto value = JsonGetStringW(timingsJson, key);
        if (value.empty()) continue;
        PrayerEntry entry;
        entry.name = name;
        if (ParseTimeOnly(value, date, entry.time)) {
            schedule.prayers.push_back(entry);
        }
    }
    return schedule;
}

bool IsCompleteSchedule(const PrayerSchedule& schedule) {
    if (schedule.prayers.size() != static_cast<size_t>(PrayerName::Count)) {
        return false;
    }
    bool seen[static_cast<size_t>(PrayerName::Count)]{};
    for (const auto& prayer : schedule.prayers) {
        const auto index = static_cast<size_t>(prayer.name);
        if (index >= static_cast<size_t>(PrayerName::Count) || seen[index]) {
            return false;
        }
        seen[index] = true;
    }
    return true;
}

std::wstring BuildCachePath(int year, int month, int day, const LocationInfo& loc,
                            int method) {
    std::wstring key;
    if (loc.latitude != 0 || loc.longitude != 0) {
        wchar_t buf[64];
        swprintf(buf, 64, L"%.4f_%.4f", loc.latitude, loc.longitude);
        key = buf;
    } else {
        key = loc.city + L"_" + loc.country;
    }
    key = SanitizeCachePart(key);
    wchar_t path[512];
    swprintf(path, 512, L"%04d-%02d-%02d_%s_m%d.json", year, month, day,
             key.c_str(), method);
    const auto folder = GetCacheFolder();
    return folder.empty() ? L"" : folder + L"\\" + path;
}

std::wstring BuildIslamicAppUrl(int year, int month, int day, const LocationInfo& loc,
                                int method) {
    SYSTEMTIME now = GetLocalNow();
    std::wstring dateSegment;
    if (year == now.wYear && month == now.wMonth && day == now.wDay) {
        dateSegment = L"today";
    } else {
        wchar_t buf[16];
        swprintf(buf, 16, L"%02d-%02d-%04d", day, month, year);
        dateSegment = buf;
    }
    std::wstring url = L"https://api.islamic.app/v1/timings/" + dateSegment + L"?";
    if (loc.source == L"manual-city") {
        auto cc = GetCountryCode(loc.country);
        url += L"city=" + UrlEncode(loc.city) + L"&country=" + cc + L"&method=" +
               std::to_wstring(method);
    } else {
        url += L"latitude=" + std::to_wstring(loc.latitude) + L"&longitude=" +
               std::to_wstring(loc.longitude) + L"&method=" +
               std::to_wstring(method);
        if (!loc.timezone.empty()) {
            url += L"&timezone=" + UrlEncode(loc.timezone);
        }
    }
    return url;
}

std::wstring BuildAladhanUrl(int year, int month, int day, const LocationInfo& loc,
                             int method) {
    wchar_t dateSeg[16];
    swprintf(dateSeg, 16, L"%02d-%02d-%04d", day, month, year);
    std::wstring url;
    if (loc.source == L"manual-city") {
        url = L"https://api.aladhan.com/v1/timingsByCity/" + std::wstring(dateSeg) +
              L"?city=" + UrlEncode(loc.city) + L"&country=" + UrlEncode(loc.country) +
              L"&method=" + std::to_wstring(method);
    } else {
        url = L"https://api.aladhan.com/v1/timings/" + std::wstring(dateSeg) +
              L"?latitude=" + std::to_wstring(loc.latitude) + L"&longitude=" +
              std::to_wstring(loc.longitude) + L"&method=" +
              std::to_wstring(method);
        if (!loc.timezone.empty()) {
            url += L"&timezonestring=" + UrlEncode(loc.timezone);
        }
    }
    return url;
}

PrayerSchedule FetchScheduleFromApi(int year, int month, int day,
                                    LocationInfo loc, int method) {
    auto parseResponse = [&](const std::string& body) -> PrayerSchedule {
        auto data = JsonFindObject(body, "data");
        if (!data) throw std::runtime_error("no data");
        auto timings = JsonFindObject(*data, "timings");
        if (!timings) throw std::runtime_error("no timings");
        std::wstring tz = loc.timezone;
        if (auto meta = JsonFindObject(*data, "meta")) {
            auto tzVal = JsonGetStringW(*meta, "timezone");
            if (!tzVal.empty()) tz = tzVal;
        }
        auto schedule =
            BuildScheduleFromTimingsJson(*timings, year, month, day, loc, tz);
        if (!IsCompleteSchedule(schedule)) {
            throw std::runtime_error("incomplete prayer timings");
        }
        return schedule;
    };
    try {
        auto url = BuildIslamicAppUrl(year, month, day, loc, method);
        auto body = HttpGetUtf8(url);
        if (!body) throw std::runtime_error("islamic.app failed");
        return parseResponse(*body);
    } catch (...) {
        Wh_Log(L"Primary prayer API failed; trying Aladhan");
        auto url = BuildAladhanUrl(year, month, day, loc, method);
        auto body = HttpGetUtf8(url);
        if (!body) throw std::runtime_error("aladhan failed");
        return parseResponse(*body);
    }
}

bool IsCacheStale(const PrayerSchedule& schedule, int y, int m, int d,
                  const LocationInfo& loc) {
    if (schedule.year != y || schedule.month != m || schedule.day != d) return true;
    if (!loc.timezone.empty() && !schedule.location.timezone.empty() &&
        _wcsicmp(loc.timezone.c_str(), schedule.location.timezone.c_str()) != 0) {
        return true;
    }
    SYSTEMTIME now = GetLocalNow();
    if (y == now.wYear && m == now.wMonth && d == now.wDay) {
        ULONGLONG ageMs = GetUtcNowMs() - schedule.fetchedAtUtcMs;
        return ageMs > static_cast<ULONGLONG>(kCacheStaleHours) * 3600000ULL;
    }
    return false;
}

std::optional<PrayerSchedule> LoadCache(int year, int month, int day,
                                        const LocationInfo& loc, int method) {
    auto path = BuildCachePath(year, month, day, loc, method);
    auto contents = ReadTextFile(path, 256 * 1024);
    if (!contents) return std::nullopt;
    const std::string& json = *contents;
    PrayerSchedule schedule;
    auto dateStr = JsonGetStringW(json, "Date");
    if (!dateStr.empty()) {
        if (dateStr.size() < 10 || dateStr[4] != L'-' ||
            dateStr[7] != L'-') {
            return std::nullopt;
        }
        schedule.year = _wtoi(dateStr.substr(0, 4).c_str());
        schedule.month = _wtoi(dateStr.substr(5, 2).c_str());
        schedule.day = _wtoi(dateStr.substr(8, 2).c_str());
    } else {
        schedule.year = year;
        schedule.month = month;
        schedule.day = day;
    }
    schedule.location = loc;
    schedule.fetchedAtUtcMs = static_cast<ULONGLONG>(
        JsonGetDouble(json, "FetchedAtUtcMs").value_or(0));
    if (!schedule.fetchedAtUtcMs) {
        // Old cache format: force a refresh instead of pretending it is new.
        schedule.fetchedAtUtcMs = 1;
    }
    if (auto locObj = JsonFindObject(json, "Location")) {
        schedule.location.city = JsonGetStringW(*locObj, "City");
        schedule.location.country = JsonGetStringW(*locObj, "Country");
        schedule.location.timezone = JsonGetStringW(*locObj, "Timezone");
        schedule.location.latitude = JsonGetDouble(*locObj, "Latitude").value_or(loc.latitude);
        schedule.location.longitude = JsonGetDouble(*locObj, "Longitude").value_or(loc.longitude);
    }
    auto timings = JsonFindObject(json, "timings");
    if (timings) {
        auto parsed = BuildScheduleFromTimingsJson(*timings, year, month, day,
                                                   schedule.location,
                                                   schedule.location.timezone);
        if (!IsCompleteSchedule(parsed)) {
            return std::nullopt;
        }
        parsed.fetchedAtUtcMs = schedule.fetchedAtUtcMs;
        return parsed;
    }
    for (int i = 0; i < static_cast<int>(PrayerName::Count); ++i) {
        auto name = static_cast<PrayerName>(i);
        std::string search = std::string("\"") + WideToUtf8(PrayerApiKey(name)) + "\"";
        if (json.find(search) != std::string::npos) {
            auto parsed = BuildScheduleFromTimingsJson(
                json, year, month, day, schedule.location,
                schedule.location.timezone);
            if (!IsCompleteSchedule(parsed)) {
                return std::nullopt;
            }
            parsed.fetchedAtUtcMs = schedule.fetchedAtUtcMs;
            return parsed;
        }
    }
    SYSTEMTIME date{};
    date.wYear = static_cast<WORD>(year);
    date.wMonth = static_cast<WORD>(month);
    date.wDay = static_cast<WORD>(day);
    size_t pos = 0;
    while ((pos = json.find("\"Name\":", pos)) != std::string::npos) {
        pos += 7;
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '"')) ++pos;
        int nameIdx = atoi(json.c_str() + pos);
        if (nameIdx < 0 || nameIdx >= static_cast<int>(PrayerName::Count)) {
            return std::nullopt;
        }
        pos = json.find("\"Time\":", pos);
        if (pos == std::string::npos) break;
        pos += 7;
        while (pos < json.size() && json[pos] != '"') ++pos;
        if (pos >= json.size()) break;
        ++pos;
        size_t end = json.find('"', pos);
        if (end == std::string::npos) break;
        std::wstring timeText = Utf8ToWide(json.substr(pos, end - pos));
        if (timeText.size() >= 16) {
            SYSTEMTIME st = date;
            if (!ParseTimeOnly(timeText.substr(11), date, st)) {
                return std::nullopt;
            }
            PrayerEntry entry;
            entry.name = static_cast<PrayerName>(nameIdx);
            entry.time = st;
            schedule.prayers.push_back(entry);
        }
        pos = end;
    }
    if (!IsCompleteSchedule(schedule)) {
        return std::nullopt;
    }
    return schedule;
}

void SaveCache(const PrayerSchedule& schedule, int method,
               const LocationInfo& cacheLocation) {
    auto path = BuildCachePath(schedule.year, schedule.month, schedule.day,
                              cacheLocation, method);
    if (path.empty()) return;
    std::ostringstream out;
    out << std::setprecision(15);
    out << "{\n";
    wchar_t dateBuf[16];
    swprintf(dateBuf, 16, L"%04d-%02d-%02d", schedule.year, schedule.month, schedule.day);
    out << "  \"Date\": \"" << WideToUtf8(dateBuf) << "\",\n";
    out << "  \"FetchedAtUtcMs\": " << schedule.fetchedAtUtcMs << ",\n";
    out << "  \"Location\": {\n";
    out << "    \"Latitude\": " << schedule.location.latitude << ",\n";
    out << "    \"Longitude\": " << schedule.location.longitude << ",\n";
    out << "    \"City\": \"" << WideToUtf8(JsonEscape(schedule.location.city)) << "\",\n";
    out << "    \"Country\": \"" << WideToUtf8(JsonEscape(schedule.location.country)) << "\",\n";
    out << "    \"Timezone\": \"" << WideToUtf8(JsonEscape(schedule.location.timezone)) << "\"\n";
    out << "  },\n  \"timings\": {\n";
    for (size_t i = 0; i < schedule.prayers.size(); ++i) {
        const auto& p = schedule.prayers[i];
        char timeBuf[16];
        snprintf(timeBuf, sizeof(timeBuf), "%02u:%02u:00", p.time.wHour, p.time.wMinute);
        out << "    \"" << WideToUtf8(PrayerApiKey(p.name)) << "\": \"" << timeBuf << "\"";
        if (i + 1 < schedule.prayers.size()) out << ",";
        out << "\n";
    }
    out << "  }\n}\n";
    if (!WriteTextFileAtomically(path, out.str())) {
        Wh_Log(L"Failed to save prayer cache");
    }
}

PrayerSchedule GetScheduleForDate(int year, int month, int day,
                                  LocationInfo loc, int method) {
    auto cached = LoadCache(year, month, day, loc, method);
    if (cached) {
        if (!IsCacheStale(*cached, year, month, day, loc)) {
            return *cached;
        }
    }
    try {
        auto schedule = FetchScheduleFromApi(year, month, day, loc, method);
        SaveCache(schedule, method, loc);
        return schedule;
    } catch (...) {
        if (cached && IsCompleteSchedule(*cached)) {
            Wh_Log(L"Using stale prayer cache for %04d-%02d-%02d", year,
                   month, day);
            return *cached;
        }
        throw;
    }
}

void AddDays(SYSTEMTIME& st, int days) {
    FILETIME ft{};
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER uli{};
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    uli.QuadPart += static_cast<ULONGLONG>(days) * 864000000000ULL;
    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;
    FileTimeToSystemTime(&ft, &st);
}

std::optional<NextPrayerInfo> GetNextPrayer() {
    std::lock_guard lock(g_scheduleMutex);
    if (!g_hasCurrentSchedule) return std::nullopt;
    ULONGLONG nowSec = LocalNowUnixSeconds();
    NextPrayerInfo best{};
    ULONGLONG bestSec = 0;
    bool found = false;
    auto consider = [&](const PrayerSchedule& sched) {
        for (const auto& p : sched.prayers) {
            ULONGLONG t = PrayerUnixSeconds(p);
            if (!t) continue;
            if (t >= nowSec) {
                if (!found || t < bestSec) {
                    best.name = p.name;
                    best.time = p.time;
                    best.secondsUntil = t - nowSec;
                    bestSec = t;
                    found = true;
                }
            }
        }
    };
    consider(g_currentSchedule);
    if (g_hasNextSchedule) consider(g_nextSchedule);
    return found ? std::optional<NextPrayerInfo>{best} : std::nullopt;
}

std::vector<PrayerEntry> GetTodayEntriesWithStatus() {
    std::lock_guard lock(g_scheduleMutex);
    std::vector<PrayerEntry> result;
    if (!g_hasCurrentSchedule) return result;
    ULONGLONG nowSec = LocalNowUnixSeconds();
    const PrayerEntry* current = nullptr;
    for (const auto& p : g_currentSchedule.prayers) {
        ULONGLONG t = PrayerUnixSeconds(p);
        if (!t) continue;
        if (nowSec >= t &&
            nowSec < t + kCurrentPrayerHighlightMinutes * 60) {
            current = &p;
            break;
        }
    }
    NextPrayerInfo nextInfo{};
    bool hasNext = false;
    auto considerNext = [&](const PrayerSchedule& sched) {
        for (const auto& p : sched.prayers) {
            ULONGLONG t = PrayerUnixSeconds(p);
            if (!t) continue;
            if (t >= nowSec) {
                if (!hasNext || t < SystemTimeToUnixSeconds(nextInfo.time)) {
                    nextInfo.name = p.name;
                    nextInfo.time = p.time;
                    nextInfo.secondsUntil = t - nowSec;
                    hasNext = true;
                }
            }
        }
    };
    considerNext(g_currentSchedule);
    if (g_hasNextSchedule) considerNext(g_nextSchedule);
    for (auto p : g_currentSchedule.prayers) {
        p.isCurrent = current && current->name == p.name &&
                       current->time.wHour == p.time.wHour &&
                       current->time.wMinute == p.time.wMinute;
        if (hasNext && !current) {
            p.isNext = p.name == nextInfo.name &&
                       p.time.wHour == nextInfo.time.wHour &&
                       p.time.wMinute == nextInfo.time.wMinute;
        }
        result.push_back(p);
    }
    return result;
}

DisplayContent FormatTaskbarDisplay() {
    const auto settings = GetSettingsSnapshot();
    DisplayContent content;
    content.color = TaskbarTextColorHex();
    if (!settings.showOnTaskbar || g_isFullscreen.load()) {
        content.visible = false;
        return content;
    }
    auto next = GetNextPrayer();
    const bool slotReady =
        g_backend.load() != TaskbarBackend::Win11Xaml || g_slotInjected.load();
    if (!next) {
        content.visible = true;
        if (g_lastRefreshFailed.load() && !g_refreshRunning.load()) {
            content.top = StabilizeCompactText(L"--");
            content.bottom = Loc(L"Prayer");
        } else {
            content.top = StabilizeCompactText(L"...");
            content.bottom = Loc(L"Prayer");
        }
        g_uiTimerIntervalMs = slotReady ? 2000 : g_injectRetryMs.load();
        return content;
    }
    content.visible = true;
    content.bottom = GetPrayerName(next->name);
    switch (settings.contentMode) {
        case ContentMode::Time:
            content.top = FormatTime(next->time);
            break;
        case ContentMode::Smart:
            if (next->secondsUntil > 3600) {
                content.top = FormatTime(next->time);
            } else {
                content.top = FormatCountdown(next->secondsUntil);
            }
            break;
        default:
            content.top = FormatCountdown(next->secondsUntil);
            break;
    }
    int intervalMs = 30000;
    if (next->secondsUntil <= 3600) intervalMs = 1000;
    else if (next->secondsUntil <= 6 * 3600) intervalMs = 15000;
    if (!slotReady) {
        intervalMs = g_injectRetryMs.load();
    }
    g_uiTimerIntervalMs = intervalMs;
    return content;
}

std::wstring GetNotificationSoundPath() {
    const auto base = GetAppDataPrayerTrayPath();
    return base.empty() ? L"" : base + L"\\audio\\allah-akbar.mp3";
}

bool LooksLikeMp3(std::string_view data) {
    if (data.size() < kNotificationSoundMinBytes ||
        data.size() > kNotificationSoundMaxBytes) {
        return false;
    }
    if (data.size() >= 3 && data[0] == 'I' && data[1] == 'D' &&
        data[2] == '3') {
        return true;
    }
    const auto b0 = static_cast<unsigned char>(data[0]);
    const auto b1 = static_cast<unsigned char>(data[1]);
    return b0 == 0xFF && (b1 & 0xE0) == 0xE0;
}

bool EnsureNotificationSoundFile() {
    const auto path = GetNotificationSoundPath();
    if (path.empty()) {
        return false;
    }
    const auto base = GetAppDataPrayerTrayPath();
    CreateDirectoryW(base.c_str(), nullptr);
    CreateDirectoryW((base + L"\\audio").c_str(), nullptr);

    WIN32_FILE_ATTRIBUTE_DATA attrs{};
    if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attrs)) {
        ULARGE_INTEGER size{};
        size.HighPart = attrs.nFileSizeHigh;
        size.LowPart = attrs.nFileSizeLow;
        if (size.QuadPart >= kNotificationSoundMinBytes &&
            size.QuadPart <= kNotificationSoundMaxBytes) {
            auto existing =
                ReadTextFile(path, kNotificationSoundMaxBytes);
            if (existing && LooksLikeMp3(*existing)) return true;
        }
    }

    const auto body =
        HttpGetUtf8(kNotificationSoundUrl, kNotificationSoundMaxBytes);
    if (!body || !LooksLikeMp3(*body)) {
        Wh_Log(L"Notification sound download failed");
        return false;
    }

    if (!WriteTextFileAtomically(path, *body)) {
        Wh_Log(L"Notification sound write failed");
        return false;
    }
    return true;
}

void PlayNotificationSound() {
    const auto path = GetNotificationSoundPath();
    if (path.empty() || GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return;
    }
    mciSendStringW(L"close prayertray_notify", nullptr, 0, nullptr);
    std::wstring openCmd =
        L"open \"" + path + L"\" type mpegvideo alias prayertray_notify";
    if (mciSendStringW(openCmd.c_str(), nullptr, 0, nullptr) != 0) {
        openCmd = L"open \"" + path + L"\" alias prayertray_notify";
        if (mciSendStringW(openCmd.c_str(), nullptr, 0, nullptr) != 0) {
            Wh_Log(L"Notification sound open failed");
            return;
        }
    }
    mciSendStringW(L"setaudio prayertray_notify volume to 120", nullptr, 0, nullptr);
    if (mciSendStringW(L"play prayertray_notify", nullptr, 0, nullptr) != 0) {
        Wh_Log(L"Notification sound play failed");
    }
}

void CheckNotifications() {
    if (!GetSettingsSnapshot().playPrayerNotification) return;
    bool shouldPlay = false;
    {
        std::lock_guard lock(g_scheduleMutex);
        if (!g_hasCurrentSchedule) return;
        int dateKey = DateKey(g_currentSchedule.year, g_currentSchedule.month,
                              g_currentSchedule.day);
        if (g_notificationActiveDateKey != dateKey) {
            g_notificationActiveDateKey = dateKey;
            g_playedNotificationKeys.clear();
        }
        ULONGLONG nowSec = LocalNowUnixSeconds();
        const ULONGLONG leadSec =
            static_cast<ULONGLONG>(kNotificationLeadMinutes * 60);
        auto consider = [&](const PrayerSchedule& sched) {
            const int schedKey = DateKey(sched.year, sched.month, sched.day);
            for (const auto& p : sched.prayers) {
                ULONGLONG t = PrayerUnixSeconds(p);
                if (!t) continue;
                if (nowSec + leadSec < t || nowSec >= t) continue;
                wchar_t key[64];
                swprintf(key, 64, L"%d:%d", schedKey,
                         static_cast<int>(p.name));
                if (g_playedNotificationKeys.insert(key).second) {
                    return true;
                }
            }
            return false;
        };
        shouldPlay = consider(g_currentSchedule) ||
                     (g_hasNextSchedule && consider(g_nextSchedule));
    }
    if (shouldPlay) PlayNotificationSound();
}

bool IsForegroundWindowFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == GetDesktopWindow() || fg == GetShellWindow()) {
        return false;
    }

    // Desktop / shell surfaces cover the monitor but are not fullscreen apps.
    wchar_t className[64]{};
    if (GetClassNameW(fg, className, ARRAYSIZE(className))) {
        if (_wcsicmp(className, L"Progman") == 0 ||
            _wcsicmp(className, L"WorkerW") == 0 ||
            _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
            _wcsicmp(className, L"DV2ControlHost") == 0 ||
            _wcsicmp(className, L"ForegroundStaging") == 0 ||
            _wcsicmp(className, L"ApplicationManager_DesktopShellWindow") == 0) {
            return false;
        }
    }

    // Ignore cloaked UWP shell hosts.
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(fg, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    RECT windowRect{};
    if (!GetWindowRect(fg, &windowRect)) {
        return false;
    }
    HMONITOR monitor = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) {
        return false;
    }
    const int tol = 4;
    const RECT& mr = mi.rcMonitor;
    if (abs(windowRect.left - mr.left) > tol) return false;
    if (abs(windowRect.top - mr.top) > tol) return false;
    if (abs(windowRect.right - mr.right) > tol) return false;
    if (abs(windowRect.bottom - mr.bottom) > tol) return false;
    return true;
}

bool BootstrapFromCache() {
    auto locOpt = LoadLastLocation();
    if (!locOpt) {
        return false;
    }

    LocationInfo loc = *locOpt;
    const auto settings = GetSettingsSnapshot();
    if (settings.useManualLocation) {
        try {
            loc = ResolveManualLocation();
        } catch (...) {
            return false;
        }
    } else if (loc.source.starts_with(L"manual")) {
        return false;
    }
    SYSTEMTIME now = GetLocalNow();
    const int method = ResolveCalculationMethod(loc, settings);
    auto today = LoadCache(now.wYear, now.wMonth, now.wDay, loc, method);
    if (!today || today->prayers.empty()) {
        return false;
    }

    SYSTEMTIME tomorrow = now;
    AddDays(tomorrow, 1);
    auto nextDay = LoadCache(tomorrow.wYear, tomorrow.wMonth, tomorrow.wDay,
                             loc, method);

    {
        std::lock_guard lock(g_scheduleMutex);
        g_location = loc;
        g_currentSchedule = *today;
        g_hasCurrentSchedule = true;
        if (nextDay && !nextDay->prayers.empty()) {
            g_nextSchedule = *nextDay;
            g_hasNextSchedule = true;
        } else {
            g_hasNextSchedule = false;
        }
    }
    Wh_Log(L"Loaded cached schedule for %s", loc.city.c_str());
    return true;
}

bool RefreshSchedulesOnce(bool fullLocation) {
    bool success = false;
    try {
        LocationInfo loc;
        const auto settings = GetSettingsSnapshot();
        if (settings.useManualLocation) {
            loc = ResolveManualLocation();
            SaveLastLocation(loc);
        } else if (!fullLocation) {
            if (auto last = LoadLastLocation()) {
                const ULONGLONG now = GetUtcNowMs();
                const ULONGLONG maxAge =
                    static_cast<ULONGLONG>(kLocationRefreshHours) * 3600000ULL;
                if (!last->source.starts_with(L"manual") &&
                    last->resolvedAtUtcMs && now >= last->resolvedAtUtcMs &&
                    now - last->resolvedAtUtcMs <= maxAge) {
                    loc = *last;
                } else {
                    loc = ResolveLocation();
                }
            } else {
                loc = ResolveLocation();
            }
        } else {
            loc = ResolveLocation();
        }

        SYSTEMTIME now = GetLocalNow();
        const int method = ResolveCalculationMethod(loc, settings);
        auto today = GetScheduleForDate(now.wYear, now.wMonth, now.wDay, loc,
                                        method);
        SYSTEMTIME tomorrow = now;
        AddDays(tomorrow, 1);
        PrayerSchedule nextDay;
        try {
            nextDay = GetScheduleForDate(tomorrow.wYear, tomorrow.wMonth,
                                         tomorrow.wDay, loc, method);
        } catch (...) {
        }
        {
            std::lock_guard lock(g_scheduleMutex);
            g_location = loc;
            g_currentSchedule = today;
            g_hasCurrentSchedule = !today.prayers.empty();
            g_nextSchedule = nextDay;
            g_hasNextSchedule = !nextDay.prayers.empty();
        }
        g_lastRefreshFailed = !g_hasCurrentSchedule;
        success = g_hasCurrentSchedule;
    } catch (const std::exception& ex) {
        Wh_Log(L"Refresh failed: %S", ex.what());
        g_lastRefreshFailed = true;
    } catch (...) {
        Wh_Log(L"Refresh failed");
        g_lastRefreshFailed = true;
    }
    if (g_messageHwnd) {
        PostMessageW(g_messageHwnd, WM_PT_UI_TICK, 0, 0);
    }
    return success;
}

DWORD WINAPI RefreshThreadProc(void*) {
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    } catch (...) {
    }

    DWORD waitMs = INFINITE;
    DWORD failureDelayMs = 60000;
    HANDLE handles[] = {g_stopEvent, g_refreshWakeEvent};
    while (!StopRequested()) {
        DWORD wait = WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE,
                                            waitMs);
        if (wait == WAIT_OBJECT_0 || StopRequested()) break;
        if (wait != WAIT_OBJECT_0 + 1 && wait != WAIT_TIMEOUT) {
            Wh_Log(L"Refresh worker wait failed (%u)", GetLastError());
            break;
        }

        bool success = false;
        do {
            g_refreshRequested = false;
            const bool fullLocation = g_forceLocationRefresh.exchange(false);

            // Handle an enable-preview before any location or prayer API work.
            // A cached clip therefore plays immediately after Apply, while a
            // first-time download isn't delayed by the schedule refresh.
            const auto settings = GetSettingsSnapshot();
            const bool previewRequested =
                g_soundPreviewRequested.exchange(false);
            bool soundReady = true;
            if (settings.playPrayerNotification) {
                soundReady = EnsureNotificationSoundFile();
            }
            if (previewRequested && soundReady && g_messageHwnd) {
                PostMessageW(g_messageHwnd, WM_PT_PLAY_SOUND, 0, 0);
            }

            g_refreshRunning = true;
            success = RefreshSchedulesOnce(fullLocation);
            g_refreshRunning = false;
        } while (!StopRequested() &&
                 (g_refreshRequested.exchange(false) ||
                  g_forceLocationRefresh.load()));

        if (success) {
            failureDelayMs = 60000;
            waitMs = kScheduleRefreshIntervalMs;
        } else {
            waitMs = failureDelayMs;
            failureDelayMs = (std::min)(failureDelayMs * 2, 15UL * 60UL * 1000UL);
        }
    }
    g_refreshRunning = false;
    if (apartmentInitialized) winrt::uninit_apartment();
    return 0;
}

bool StartRefreshWorker() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_refreshWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_refreshWakeEvent) {
        if (g_refreshWakeEvent) CloseHandle(g_refreshWakeEvent);
        if (g_stopEvent) CloseHandle(g_stopEvent);
        g_refreshWakeEvent = nullptr;
        g_stopEvent = nullptr;
        return false;
    }
    g_refreshThread =
        CreateThread(nullptr, 0, RefreshThreadProc, nullptr, 0, nullptr);
    if (!g_refreshThread) {
        CloseHandle(g_refreshWakeEvent);
        CloseHandle(g_stopEvent);
        g_refreshWakeEvent = nullptr;
        g_stopEvent = nullptr;
        return false;
    }
    return true;
}

void StopRefreshWorker() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_refreshWakeEvent) SetEvent(g_refreshWakeEvent);
    if (g_refreshThread) {
        WaitForSingleObject(g_refreshThread, INFINITE);
        CloseHandle(g_refreshThread);
        g_refreshThread = nullptr;
    }
    if (g_refreshWakeEvent) {
        CloseHandle(g_refreshWakeEvent);
        g_refreshWakeEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void RequestRefresh(bool fullLocation = false) {
    if (fullLocation) {
        g_forceLocationRefresh = true;
    }
    g_refreshRequested = true;
    if (g_refreshWakeEvent) SetEvent(g_refreshWakeEvent);
}

void MaybeRequestScheduleRefresh() {
    if (g_refreshRunning.load() || g_refreshRequested.load()) {
        return;
    }
    SYSTEMTIME now = GetLocalNow();
    const int todayKey = DateKey(now.wYear, now.wMonth, now.wDay);
    bool needsRefresh = false;
    {
        std::lock_guard lock(g_scheduleMutex);
        if (g_hasCurrentSchedule) {
            const int schedKey = DateKey(g_currentSchedule.year, g_currentSchedule.month,
                                         g_currentSchedule.day);
            needsRefresh = schedKey != todayKey;
        }
    }
    if (needsRefresh) {
        RequestRefresh(false);
    }
}

std::wstring FindWindhawkExePath() {
    wchar_t buffer[MAX_PATH]{};

    // Default install locations.
    const wchar_t* candidates[] = {
        L"%ProgramFiles%\\Windhawk\\windhawk.exe",
        L"%ProgramFiles(x86)%\\Windhawk\\windhawk.exe",
        L"%LocalAppData%\\Programs\\Windhawk\\windhawk.exe",
    };
    for (const wchar_t* pattern : candidates) {
        DWORD n = ExpandEnvironmentStringsW(pattern, buffer, MAX_PATH);
        if (n > 0 && n < MAX_PATH && GetFileAttributesW(buffer) != INVALID_FILE_ATTRIBUTES) {
            return buffer;
        }
    }

    // Uninstall registry key often has the install folder.
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Windhawk",
                      0, KEY_READ | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS) {
        DWORD type = 0;
        DWORD size = sizeof(buffer);
        if (RegQueryValueExW(key, L"InstallLocation", nullptr, &type,
                             reinterpret_cast<LPBYTE>(buffer), &size) == ERROR_SUCCESS &&
            type == REG_SZ) {
            std::wstring path = buffer;
            if (!path.empty() && path.back() != L'\\') {
                path += L'\\';
            }
            path += L"windhawk.exe";
            if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
                RegCloseKey(key);
                return path;
            }
        }
        RegCloseKey(key);
    }
    return L"";
}

void OpenWindhawkSettings() {
    // Do NOT use windhawk: — that opens the code editor (VSCodium), not the
    // mod manager. Launch Windhawk.exe for the real UI.
    const auto exe = FindWindhawkExePath();
    if (!exe.empty()) {
        ShellExecuteW(nullptr, L"open", exe.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    Wh_Log(L"Windhawk.exe not found");
}

// Forward declarations
void DispatchCommand(int commandId);
void ShowFlyout();
void HideFlyout();
void ShowContextMenu(HWND anchor);
void ApplyEngineToBackends();
void EnsureWin10Overlay();
void RepositionWin10Overlay();
void DestroyFlyout();
void DestroyWin10Overlay();
void StopTimers();

void EnsureContextMenu() {
    if (g_contextMenu) return;
    g_contextMenu = CreatePopupMenu();
}

void ShowContextMenu(HWND anchor) {
    EnsureContextMenu();
    while (GetMenuItemCount(g_contextMenu) > 0) {
        DeleteMenu(g_contextMenu, 0, MF_BYPOSITION);
    }
    AppendMenuW(g_contextMenu, MF_STRING, kCmdShowFlyout, Loc(L"Tray_TodaySchedule").c_str());
    AppendMenuW(g_contextMenu, MF_STRING, kCmdRefreshLocation, Loc(L"Tray_RefreshLocation").c_str());
    AppendMenuW(g_contextMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(g_contextMenu, MF_STRING, kCmdShowMenu + 100, Loc(L"Tray_Settings").c_str());
    POINT pt{};
    GetCursorPos(&pt);
    // Message-only windows are unreliable TrackPopupMenu owners on Win11.
    HWND owner = FindCurrentProcessTaskbarWnd();
    if (!owner) {
        owner = anchor;
    }
    if (!owner) {
        owner = g_messageHwnd;
    }
    if (!owner) {
        return;
    }
    SetForegroundWindow(owner);
    const UINT cmd = TrackPopupMenu(g_contextMenu, TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                    pt.x, pt.y, 0, owner, nullptr);
    PostMessageW(owner, WM_NULL, 0, 0);
    if (cmd) {
        DispatchCommand(static_cast<int>(cmd));
    }
}

void DispatchCommand(int commandId) {
    if (g_uiThreadId && GetCurrentThreadId() != g_uiThreadId) {
        if (g_messageHwnd) {
            PostMessageW(g_messageHwnd, WM_COMMAND,
                         static_cast<WPARAM>(commandId), 0);
        }
        return;
    }
    const auto now = GetTickCount64();
    if (commandId != kCmdShowFlyout &&
        g_lastCommandId.load() == commandId &&
        now - g_lastCommandTick.load() < 250) {
        return;
    }
    g_lastCommandId = commandId;
    g_lastCommandTick = now;
    switch (commandId) {
        case kCmdShowFlyout:
            if (g_flyoutVisible) {
                HideFlyout();
            } else {
                ShowFlyout();
            }
            break;
        case kCmdShowMenu:
            ShowContextMenu(g_win10OverlayHwnd ? g_win10OverlayHwnd
                                               : FindCurrentProcessTaskbarWnd());
            break;
        case kCmdRefreshLocation:
            RequestRefresh(true);
            break;
        case kCmdShowMenu + 100:
            OpenWindhawkSettings();
            break;
        default:
            break;
    }
}


constexpr wchar_t kFlyoutClass[] = L"PrayerTrayFlyoutWnd";
constexpr int kFlyoutWidth = 220;
constexpr int kFlyoutRowHeight = 28;
constexpr int kFlyoutHeaderHeight = 52;
constexpr int kFlyoutOpenGraceMs = 300;

void HideFlyout();
void UninstallFlyoutDismissHook();
void InstallFlyoutDismissHook();

LRESULT CALLBACK FlyoutMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && g_flyoutVisible && g_flyoutHwnd &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
         wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN ||
         wParam == WM_NCRBUTTONDOWN)) {
        if (GetTickCount64() >= g_flyoutSuppressDismissUntil) {
            const auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            RECT rc{};
            if (GetWindowRect(g_flyoutHwnd, &rc)) {
                POINT pt{info->pt.x, info->pt.y};
                if (!PtInRect(&rc, pt)) {
                    // A slot click is handled by DispatchCommand as a native
                    // toggle. Don't enqueue an outside-dismiss first, which
                    // would otherwise hide and immediately reopen the flyout.
                    if ((wParam == WM_LBUTTONDOWN ||
                         wParam == WM_NCLBUTTONDOWN) &&
                        g_hasFlyoutAnchor &&
                        PtInRect(&g_flyoutAnchorRect, pt)) {
                        return CallNextHookEx(g_flyoutMouseHook, nCode, wParam,
                                              lParam);
                    }
                    if (g_messageHwnd) {
                        PostMessageW(g_messageHwnd, WM_PT_HIDE_FLYOUT, 0, 0);
                    }
                }
            }
        }
    }
    return CallNextHookEx(g_flyoutMouseHook, nCode, wParam, lParam);
}

void UninstallFlyoutDismissHook() {
    if (g_flyoutMouseHook) {
        UnhookWindowsHookEx(g_flyoutMouseHook);
        g_flyoutMouseHook = nullptr;
    }
}

void InstallFlyoutDismissHook() {
    if (g_flyoutMouseHook) {
        return;
    }
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&FlyoutMouseHookProc), &module);
    g_flyoutMouseHook =
        SetWindowsHookExW(WH_MOUSE_LL, FlyoutMouseHookProc, module, 0);
    if (!g_flyoutMouseHook) {
        Wh_Log(L"Failed to install flyout dismiss mouse hook (%u)", GetLastError());
    }
}

LRESULT CALLBACK FlyoutWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                SetCursor(LoadCursorW(nullptr, IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                HideFlyout();
            }
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                HideFlyout();
                return 0;
            }
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc{};
            GetClientRect(hWnd, &rc);
            const TaskbarPalette palette = GetTaskbarPalette();
            HBRUSH bg = CreateSolidBrush(palette.background);
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, palette.foreground);
            HFONT font = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                     CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                     DEFAULT_PITCH, L"Segoe UI");
            HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
            LocationInfo loc;
            {
                std::lock_guard lock(g_scheduleMutex);
                loc = g_location;
            }
            std::wstring title = Loc(L"PrayerTimes");
            std::wstring subtitle = LocationDisplayName(loc);
            RECT titleRc{12, 8, rc.right - 12, 28};
            DrawTextW(hdc, title.c_str(), -1, &titleRc, DT_LEFT | DT_SINGLELINE);
            RECT subRc{12, 28, rc.right - 12, 48};
            SetTextColor(hdc, palette.secondary);
            DrawTextW(hdc, subtitle.c_str(), -1, &subRc, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
            auto entries = GetTodayEntriesWithStatus();
            int y = kFlyoutHeaderHeight;
            for (const auto& e : entries) {
                RECT rowRc{12, y, rc.right - 12, y + kFlyoutRowHeight};
                if (e.isCurrent || e.isNext) {
                    HBRUSH hl = CreateSolidBrush(palette.highlight);
                    RECT fillRc{0, y, rc.right, y + kFlyoutRowHeight};
                    FillRect(hdc, &fillRc, hl);
                    DeleteObject(hl);
                }
                SetTextColor(hdc, palette.foreground);
                std::wstring name = GetPrayerName(e.name);
                std::wstring time = FormatTime(e.time);
                RECT nameRc{16, y + 4, rc.right / 2, y + kFlyoutRowHeight};
                RECT timeRc{rc.right / 2, y + 4, rc.right - 16, y + kFlyoutRowHeight};
                DrawTextW(hdc, name.c_str(), -1, &nameRc, DT_LEFT | DT_SINGLELINE);
                DrawTextW(hdc, time.c_str(), -1, &timeRc, DT_RIGHT | DT_SINGLELINE);
                y += kFlyoutRowHeight;
            }
            HBRUSH border = CreateSolidBrush(palette.border);
            FrameRect(hdc, &rc, border);
            DeleteObject(border);
            SelectObject(hdc, oldFont);
            DeleteObject(font);
            EndPaint(hWnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void EnsureFlyoutWindow() {
    if (g_flyoutHwnd) return;
    if (!g_flyoutClassRegistered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = FlyoutWndProc;
        wc.hInstance = g_moduleInstance;
        wc.lpszClassName = kFlyoutClass;
        wc.hbrBackground = nullptr;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Flyout class registration failed (%u)", GetLastError());
            return;
        }
        g_flyoutClassRegistered = true;
    }
    g_flyoutHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kFlyoutClass, L"",
        WS_POPUP, 0, 0, kFlyoutWidth,
        kFlyoutHeaderHeight + kFlyoutRowHeight * 5 + 8, g_messageHwnd, nullptr,
        g_moduleInstance, nullptr);
    if (!g_flyoutHwnd) {
        Wh_Log(L"Flyout window creation failed (%u)", GetLastError());
    }
}

bool TryGetSlotAnchor(RECT& anchor) {
    if (g_backend.load() == TaskbarBackend::Win10Overlay &&
        g_win10OverlayHwnd && GetWindowRect(g_win10OverlayHwnd, &anchor)) {
        return true;
    }

    const int slotW = 60, slotH = 40, gap = 4;
    HWND shell = FindCurrentProcessTaskbarWnd();
    if (!shell) return false;

    if (g_backend.load() == TaskbarBackend::Win11Xaml && g_slotInjected.load()) {
        struct AnchorState {
            RECT rect{};
            std::atomic<bool> valid{false};
        };
        auto state = std::make_shared<AnchorState>();
        const bool ran = RunFromWindowThread(shell, [state, shell]() {
            try {
                auto slot = g_slotRoot;
                if (!slot) return;
                auto xamlRoot = slot.XamlRoot();
                auto content = xamlRoot
                                   ? xamlRoot.Content().try_as<UIElement>()
                                   : nullptr;
                if (!content || slot.ActualWidth() <= 0 ||
                    slot.ActualHeight() <= 0) {
                    return;
                }
                const auto transform = slot.TransformToVisual(content);
                const auto topLeft = transform.TransformPoint({0, 0});
                const auto bottomRight = transform.TransformPoint(
                    {static_cast<float>(slot.ActualWidth()),
                     static_cast<float>(slot.ActualHeight())});
                const double scale = xamlRoot.RasterizationScale();
                POINT origin{0, 0};
                if (!ClientToScreen(shell, &origin)) return;
                RECT measured{
                    origin.x + static_cast<LONG>(std::lround(topLeft.X * scale)),
                    origin.y + static_cast<LONG>(std::lround(topLeft.Y * scale)),
                    origin.x + static_cast<LONG>(std::lround(bottomRight.X * scale)),
                    origin.y + static_cast<LONG>(std::lround(bottomRight.Y * scale))};
                if (measured.right > measured.left &&
                    measured.bottom > measured.top) {
                    state->rect = measured;
                    state->valid.store(true, std::memory_order_release);
                }
            } catch (...) {
            }
        });
        if (ran && state->valid.load(std::memory_order_acquire)) {
            anchor = state->rect;
            return true;
        }
    }

    // Compatibility fallback for classic taskbars or a tray that is rebuilding.
    HWND trayNotify = FindWindowExW(shell, nullptr, L"TrayNotifyWnd", nullptr);
    RECT notifyRect{};
    if (trayNotify && GetWindowRect(trayNotify, &notifyRect)) {
        HWND clock = FindWindowExW(trayNotify, nullptr, L"TrayClockWClass", nullptr);
        if (!clock) clock = FindWindowExW(trayNotify, nullptr, L"ClockButton", nullptr);
        int clockLeft = notifyRect.right - 26 - 60;
        if (clock && GetWindowRect(clock, &anchor)) {
            clockLeft = anchor.left;
        }
        anchor.left = clockLeft - gap - slotW + 12;
        anchor.top = notifyRect.top + (notifyRect.bottom - notifyRect.top - slotH) / 2;
        anchor.right = anchor.left + slotW;
        anchor.bottom = anchor.top + slotH;
        return true;
    }
    return false;
}

void ShowFlyout() {
    EnsureFlyoutWindow();
    if (!g_flyoutHwnd) return;
    RECT anchor{};
    if (!TryGetSlotAnchor(anchor)) {
        anchor = {GetSystemMetrics(SM_CXSCREEN) - 200, GetSystemMetrics(SM_CYSCREEN) - 48, 0, 0};
        anchor.right = anchor.left + 60;
        anchor.bottom = anchor.top + 40;
    }
    int height = kFlyoutHeaderHeight + kFlyoutRowHeight * 5 + 8;
    HMONITOR mon = MonitorFromRect(&anchor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(mon, &mi)) return;

    g_flyoutAnchorRect = anchor;
    g_hasFlyoutAnchor = true;

    constexpr int gap = 8;
    constexpr int margin = 8;
    const int centerX = anchor.left + (anchor.right - anchor.left) / 2;
    const int centerY = anchor.top + (anchor.bottom - anchor.top) / 2;
    const int distanceTop = centerY - mi.rcMonitor.top;
    const int distanceBottom = mi.rcMonitor.bottom - centerY;
    const int distanceLeft = centerX - mi.rcMonitor.left;
    const int distanceRight = mi.rcMonitor.right - centerX;
    const int nearestEdge = (std::min)({distanceTop, distanceBottom,
                                       distanceLeft, distanceRight});

    int left = centerX - kFlyoutWidth / 2;
    int top = anchor.top - height - gap;
    if (nearestEdge == distanceTop) {
        top = anchor.bottom + gap;
    } else if (nearestEdge == distanceLeft) {
        left = anchor.right + gap;
        top = centerY - height / 2;
    } else if (nearestEdge == distanceRight) {
        left = anchor.left - kFlyoutWidth - gap;
        top = centerY - height / 2;
    }

    left = std::clamp(left, static_cast<int>(mi.rcWork.left) + margin,
                      static_cast<int>(mi.rcWork.right) - kFlyoutWidth - margin);
    top = std::clamp(top, static_cast<int>(mi.rcWork.top) + margin,
                     static_cast<int>(mi.rcWork.bottom) - height - margin);
    SetWindowPos(g_flyoutHwnd, HWND_TOPMOST, left, top, kFlyoutWidth, height,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    InvalidateRect(g_flyoutHwnd, nullptr, TRUE);
    g_flyoutVisible = true;
    g_flyoutSuppressDismissUntil = GetTickCount64() + kFlyoutOpenGraceMs;
    InstallFlyoutDismissHook();
}

void HideFlyout() {
    if (g_flyoutHwnd && g_flyoutVisible) {
        ShowWindow(g_flyoutHwnd, SW_HIDE);
        g_flyoutVisible = false;
    }
    g_hasFlyoutAnchor = false;
    UninstallFlyoutDismissHook();
}

void DestroyFlyout() {
    HideFlyout();
    if (g_flyoutHwnd) {
        DestroyWindow(g_flyoutHwnd);
        g_flyoutHwnd = nullptr;
    }
    if (g_flyoutClassRegistered) {
        UnregisterClassW(kFlyoutClass, g_moduleInstance);
        g_flyoutClassRegistered = false;
    }
}


constexpr wchar_t kOverlayClass[] = L"PrayerTrayOverlayWnd";
std::wstring g_win10Top;
std::wstring g_win10Bottom;
bool g_win10Visible = false;
COLORREF g_win10TextColor = RGB(255, 255, 255);

LRESULT CALLBACK Win10OverlayProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONUP:
            DispatchCommand(kCmdShowFlyout);
            return 0;
        case WM_RBUTTONUP:
            DispatchCommand(kCmdShowMenu);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc{};
            GetClientRect(hWnd, &rc);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, g_win10TextColor);
            HFONT font = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                     CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                     DEFAULT_PITCH, L"Segoe UI");
            HFONT old = static_cast<HFONT>(SelectObject(hdc, font));
            if (g_win10Visible) {
                RECT topRc = rc;
                topRc.bottom = (rc.top + rc.bottom) / 2;
                RECT botRc = rc;
                botRc.top = topRc.bottom;
                DrawTextW(hdc, g_win10Top.c_str(), -1, &topRc,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                DrawTextW(hdc, g_win10Bottom.c_str(), -1, &botRc,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            }
            SelectObject(hdc, old);
            DeleteObject(font);
            EndPaint(hWnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void EnsureWin10Overlay() {
    if (g_backend.load() != TaskbarBackend::Win10Overlay) return;
    if (!g_overlayClassRegistered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = Win10OverlayProc;
        wc.hInstance = g_moduleInstance;
        wc.lpszClassName = kOverlayClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_HAND);
        if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Overlay class registration failed (%u)", GetLastError());
            return;
        }
        g_overlayClassRegistered = true;
    }
    HWND shell = FindCurrentProcessTaskbarWnd();
    if (!shell) return;
    if (!g_win10OverlayHwnd) {
        g_win10OverlayHwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW, kOverlayClass, L"", WS_CHILD | WS_VISIBLE, 0, 0, 60,
            40, shell, nullptr, g_moduleInstance, nullptr);
        if (!g_win10OverlayHwnd) {
            Wh_Log(L"Overlay window creation failed (%u)", GetLastError());
            return;
        }
    }
    RepositionWin10Overlay();
}

void RepositionWin10Overlay() {
    if (!g_win10OverlayHwnd) return;
    HWND shell = FindCurrentProcessTaskbarWnd();
    if (!shell) return;
    const int w = 60, h = 40, gap = 4;
    int cx = 0, cy = 0;
    HWND trayNotify = FindWindowExW(shell, nullptr, L"TrayNotifyWnd", nullptr);
    RECT notifyRect{};
    if (trayNotify && GetWindowRect(trayNotify, &notifyRect)) {
        HWND clock = FindWindowExW(trayNotify, nullptr, L"TrayClockWClass", nullptr);
        if (!clock) clock = FindWindowExW(trayNotify, nullptr, L"ClockButton", nullptr);
        int clockLeft = notifyRect.right - 26 - 60;
        if (clock) {
            RECT cr{};
            GetWindowRect(clock, &cr);
            clockLeft = cr.left;
        }
        POINT pt{clockLeft - gap - w + 12,
                 notifyRect.top + (notifyRect.bottom - notifyRect.top - h) / 2};
        ScreenToClient(shell, &pt);
        cx = pt.x;
        cy = pt.y;
    } else {
        ShowWindow(g_win10OverlayHwnd, SW_HIDE);
        g_lastOverlayShown = false;
        return;
    }
    const bool show = g_win10Visible;
    if (g_hasOverlayPlacement && g_lastOverlayShown == show &&
        g_lastOverlayX == cx && g_lastOverlayY == cy && g_lastOverlayW == w &&
        g_lastOverlayH == h) {
        return;
    }
    ShowWindow(g_win10OverlayHwnd, show ? SW_SHOW : SW_HIDE);
    SetWindowPos(g_win10OverlayHwnd, HWND_TOP, cx, cy, w, h, SWP_NOACTIVATE);
    g_hasOverlayPlacement = true;
    g_lastOverlayShown = show;
    g_lastOverlayX = cx;
    g_lastOverlayY = cy;
    g_lastOverlayW = w;
    g_lastOverlayH = h;
}

void DestroyWin10Overlay() {
    if (g_win10OverlayHwnd) {
        DestroyWindow(g_win10OverlayHwnd);
        g_win10OverlayHwnd = nullptr;
    }
    g_hasOverlayPlacement = false;
    if (g_overlayClassRegistered) {
        UnregisterClassW(kOverlayClass, g_moduleInstance);
        g_overlayClassRegistered = false;
    }
}

void ApplyWin10Content(const DisplayContent& content) {
    const bool visibilityChanged = g_win10Visible != content.visible;
    g_win10Top = content.top;
    g_win10Bottom = content.bottom;
    g_win10Visible = content.visible;
    if (content.color == L"1A1A1A") g_win10TextColor = RGB(26, 26, 26);
    else g_win10TextColor = RGB(255, 255, 255);
    EnsureWin10Overlay();
    if (g_win10OverlayHwnd) {
        InvalidateRect(g_win10OverlayHwnd, nullptr, FALSE);
        if (visibilityChanged || !g_hasOverlayPlacement) {
            RepositionWin10Overlay();
        }
    }
}


Media::SolidColorBrush Brush(uint8_t alpha, uint8_t red, uint8_t green,
                             uint8_t blue) {
    return Media::SolidColorBrush({alpha, red, green, blue});
}

winrt::Windows::UI::Color ParseColor(const std::wstring& color) {
    std::wstring hex = color;
    if (hex.starts_with(L"#")) hex = hex.substr(1);
    if (hex.size() != 6) return winrt::Windows::UI::Color{255, 255, 255, 255};
    auto parseByte = [&hex](size_t offset) -> uint8_t {
        wchar_t buf[3] = {hex[offset], hex[offset + 1], 0};
        return static_cast<uint8_t>(wcstoul(buf, nullptr, 16));
    };
    return winrt::Windows::UI::Color{255, parseByte(0), parseByte(2), parseByte(4)};
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int n = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < n; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (child.Name() == name) return child;
        if (auto match = FindChildByName(child, name)) return match;
    }
    return nullptr;
}

Controls::TextBlock FindTextBlockByName(FrameworkElement element, PCWSTR name) {
    if (auto named = element.FindName(name).try_as<Controls::TextBlock>()) return named;
    return FindChildByName(element, name).try_as<Controls::TextBlock>();
}

FrameworkElement FindChildByClassName(FrameworkElement element, PCWSTR className) {
    if (winrt::get_class_name(element) == className) return element;
    int n = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < n; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (auto match = FindChildByClassName(child, className)) return match;
    }
    return nullptr;
}

FrameworkElement FindMainStackIconStackPanel(FrameworkElement mainStack) {
    FrameworkElement child = mainStack;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        return child;
    }
    return nullptr;
}

FrameworkElement FindDirectChildByName(Controls::Panel panel, PCWSTR name) {
    for (uint32_t i = 0; i < panel.Children().Size(); i++) {
        auto child = panel.Children().GetAt(i).try_as<FrameworkElement>();
        if (child && child.Name() == name) return child;
    }
    return nullptr;
}

Controls::Grid FindTrayGridFromAnchor(FrameworkElement root) {
    FrameworkElement anchor = FindChildByName(root, L"NotificationCenterButton");
    if (!anchor) anchor = FindChildByName(root, L"ControlCenterButton");
    if (!anchor) return nullptr;
    DependencyObject current = anchor;
    while (current) {
        current = Media::VisualTreeHelper::GetParent(current);
        if (!current) break;
        if (auto grid = current.try_as<Controls::Grid>()) {
            if (FindDirectChildByName(grid, L"NotificationCenterButton") ||
                FindDirectChildByName(grid, L"ControlCenterButton")) {
                return grid;
            }
        }
    }
    return nullptr;
}

FrameworkElement ResolveMainStack(FrameworkElement root) {
    if (root.Name() == L"MainStack" || winrt::get_class_name(root) == L"SystemTray.Stack") {
        return root;
    }
    return FindChildByName(root, L"MainStack");
}

bool IsSlotRootAlive() {
    if (!g_slotInjected) return false;
    try {
        if (!g_slotRoot || winrt::get_abi(g_slotRoot) == nullptr) {
            g_slotInjected = false;
            return false;
        }
        if (!g_slotRoot.XamlRoot() ||
            !Media::VisualTreeHelper::GetParent(g_slotRoot)) {
            g_slotRoot = nullptr;
            g_slotButton = nullptr;
            g_topTextBlock = nullptr;
            g_bottomTextBlock = nullptr;
            g_slotParentGrid = nullptr;
            g_slotInjected = false;
            g_hasAppliedContent = false;
            return false;
        }
        return true;
    } catch (...) {
        g_slotInjected = false;
        return false;
    }
}

void ApplySlotButtonVisualState() {
    auto button = g_slotButton;
    if (!button) return;
    uint8_t alpha = 0;
    if (g_slotPointerPressed.load()) alpha = 0x3A;
    else if (g_slotPointerOver.load()) alpha = 0x26;
    const uint8_t channel = IsLightTheme() ? 0 : 255;
    button.Background(Brush(alpha, channel, channel, channel));
}

void ResetSlotButtonVisualState() {
    g_slotPointerOver = false;
    g_slotPointerPressed = false;
    g_slotRightPressed = false;
    ApplySlotButtonVisualState();
}

void ApplyWin11SlotContent(const DisplayContent& content) {
    auto top = g_topTextBlock;
    auto bottom = g_bottomTextBlock;
    auto root = g_slotRoot;
    if (!top || !bottom || !root) {
        g_slotInjected = false;
        return;
    }
    const bool show = content.visible && (!content.top.empty() || !content.bottom.empty());
    const bool changed = !g_hasAppliedContent || g_lastAppliedVisible != show ||
                         g_lastAppliedTop != content.top ||
                         g_lastAppliedBottom != content.bottom ||
                         g_lastAppliedColor != content.color;
    // Prefer opacity over Collapsed so tray columns do not reflow/flicker.
    if (!g_hasAppliedContent || g_lastAppliedVisible != show) {
        root.Visibility(Visibility::Visible);
        root.Opacity(show ? 1.0 : 0.0);
        root.IsHitTestVisible(show);
    }
    if (!show) {
        if (changed) {
            g_hasAppliedContent = true;
            g_lastAppliedVisible = false;
            g_lastAppliedTop = content.top;
            g_lastAppliedBottom = content.bottom;
            g_lastAppliedColor = content.color;
        }
        return;
    }
    // Always keep the button interactive when content is shown (recovers from
    // a previous hide that left IsHitTestVisible false).
    root.Visibility(Visibility::Visible);
    root.Opacity(1.0);
    root.IsHitTestVisible(true);
    if (!changed) return;
    const auto brush = Media::SolidColorBrush(ParseColor(content.color));
    top.Text(content.top);
    top.FlowDirection(FlowDirection::LeftToRight);
    top.Foreground(brush);
    bottom.Text(content.bottom);
    bottom.FlowDirection(FlowDirection::LeftToRight);
    bottom.Foreground(brush);
    g_hasAppliedContent = true;
    g_lastAppliedVisible = true;
    g_lastAppliedTop = content.top;
    g_lastAppliedBottom = content.bottom;
    g_lastAppliedColor = content.color;
}

void AttachSlotInteractions(FrameworkElement slotRoot) {
    auto button = slotRoot.try_as<Controls::Button>();
    if (!button) return;
    if (auto existing = g_slotButton) {
        if (winrt::get_abi(existing) == winrt::get_abi(button)) return;
    }
    g_slotButton = button;
    ResetSlotButtonVisualState();
    button.PointerEntered([](auto&, Input::PointerRoutedEventArgs const&) {
        g_slotPointerOver = true;
        ApplySlotButtonVisualState();
    });
    button.PointerExited([](auto&, Input::PointerRoutedEventArgs const&) {
        g_slotPointerOver = false;
        g_slotPointerPressed = false;
        g_slotRightPressed = false;
        ApplySlotButtonVisualState();
    });
    button.Click([](auto&, RoutedEventArgs const&) { DispatchCommand(kCmdShowFlyout); });
    button.RightTapped([](auto&, Input::RightTappedRoutedEventArgs const& args) {
        g_slotRightPressed = false;
        DispatchCommand(kCmdShowMenu);
        args.Handled(true);
    });
    button.PointerPressed([](winrt::Windows::Foundation::IInspectable const& sender,
                             Input::PointerRoutedEventArgs const& args) {
        auto element = sender.try_as<UIElement>();
        if (!element) return;
        auto props = args.GetCurrentPoint(element).Properties();
        if (props.IsLeftButtonPressed()) {
            g_slotPointerOver = true;
            g_slotPointerPressed = true;
            ApplySlotButtonVisualState();
        }
        // Do not mark right-button as Handled here — that suppresses RightTapped.
        if (props.IsRightButtonPressed()) {
            g_slotPointerOver = true;
            g_slotRightPressed = true;
            ApplySlotButtonVisualState();
        }
    });
    button.PointerReleased([](auto&, Input::PointerRoutedEventArgs const& args) {
        g_slotPointerPressed = false;
        ApplySlotButtonVisualState();
        // Fallback if RightTapped is eaten by the tray host.
        if (g_slotRightPressed.exchange(false)) {
            DispatchCommand(kCmdShowMenu);
            args.Handled(true);
        }
    });
}

void RemovePrayerTraySlotFromGrid(Controls::Grid trayGrid) {
    auto children = trayGrid.Children();
    int slotColumn = -1;
    for (uint32_t i = 0; i < children.Size(); i++) {
        auto child = children.GetAt(i).try_as<FrameworkElement>();
        if (child && child.Name() == L"PrayerTraySlotRoot") {
            slotColumn = Controls::Grid::GetColumn(child);
            children.RemoveAt(i);
            break;
        }
    }
    if (slotColumn >= 0 && slotColumn < static_cast<int>(trayGrid.ColumnDefinitions().Size())) {
        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();
            if (!child) continue;
            int col = Controls::Grid::GetColumn(child);
            int span = Controls::Grid::GetColumnSpan(child);
            if (col > slotColumn) {
                Controls::Grid::SetColumn(child, col - 1);
            } else if (col < slotColumn && col + span > slotColumn && span > 1) {
                Controls::Grid::SetColumnSpan(child, span - 1);
            }
        }
        trayGrid.ColumnDefinitions().RemoveAt(slotColumn);
    }
    g_slotRoot = nullptr;
    g_slotButton = nullptr;
    g_topTextBlock = nullptr;
    g_bottomTextBlock = nullptr;
    g_slotParentGrid = nullptr;
    g_slotInjected = false;
    g_hasAppliedContent = false;
}

int ResolveTraySlotColumn(Controls::Grid trayGrid) {
    if (auto clock = FindDirectChildByName(trayGrid, L"NotificationCenterButton")) {
        return Controls::Grid::GetColumn(clock);
    }
    if (auto cc = FindDirectChildByName(trayGrid, L"ControlCenterButton")) {
        return Controls::Grid::GetColumn(cc) + 1;
    }
    return -1;
}

bool EnsurePrayerTraySlotInTrayGrid(Controls::Grid trayGrid) {
    if (IsSlotRootAlive() && g_slotParentGrid &&
        winrt::get_abi(g_slotParentGrid) == winrt::get_abi(trayGrid)) {
        AttachSlotInteractions(g_slotRoot);
        return true;
    }
    while (FindDirectChildByName(trayGrid, L"PrayerTraySlotRoot")) {
        RemovePrayerTraySlotFromGrid(trayGrid);
    }
    int slotColumn = ResolveTraySlotColumn(trayGrid);
    if (slotColumn < 0) return false;
    slotColumn = std::clamp(slotColumn, 0, static_cast<int>(trayGrid.ColumnDefinitions().Size()));
    Controls::Button slotButton;
    slotButton.Name(L"PrayerTraySlotRoot");
    slotButton.Background(Brush(0, 255, 255, 255));
    slotButton.BorderBrush(Brush(0, 255, 255, 255));
    slotButton.BorderThickness({0, 0, 0, 0});
    slotButton.Height(40);
    slotButton.MinWidth(52);
    slotButton.Padding({7, 0, 7, 0});
    slotButton.Margin({2, 0, 2, 0});
    slotButton.VerticalAlignment(VerticalAlignment::Center);
    slotButton.HorizontalAlignment(HorizontalAlignment::Center);
    Controls::StackPanel slotStack;
    slotStack.Orientation(Controls::Orientation::Vertical);
    slotStack.IsHitTestVisible(false);
    Controls::TextBlock topText;
    topText.Name(L"PrayerTrayTop");
    topText.FontFamily(Media::FontFamily(L"Segoe UI Variable Text"));
    topText.FontSize(11.5);
    topText.FlowDirection(FlowDirection::LeftToRight);
    topText.TextAlignment(TextAlignment::Center);
    topText.MaxWidth(92);
    topText.TextTrimming(TextTrimming::CharacterEllipsis);
    Controls::TextBlock bottomText;
    bottomText.Name(L"PrayerTrayBottom");
    bottomText.FontFamily(Media::FontFamily(L"Segoe UI Variable Text"));
    bottomText.FontSize(11.5);
    bottomText.TextAlignment(TextAlignment::Center);
    bottomText.MaxWidth(92);
    bottomText.TextTrimming(TextTrimming::CharacterEllipsis);
    slotStack.Children().Append(topText);
    slotStack.Children().Append(bottomText);
    slotButton.Content(slotStack);
    FrameworkElement slotElement = slotButton;
    Controls::ColumnDefinition column;
    column.Width({1.0, GridUnitType::Auto});
    if (slotColumn >= static_cast<int>(trayGrid.ColumnDefinitions().Size())) {
        trayGrid.ColumnDefinitions().Append(column);
    } else {
        trayGrid.ColumnDefinitions().InsertAt(slotColumn, column);
        for (uint32_t i = 0; i < trayGrid.Children().Size(); i++) {
            auto child = trayGrid.Children().GetAt(i).try_as<FrameworkElement>();
            if (!child) continue;
            int col = Controls::Grid::GetColumn(child);
            int span = Controls::Grid::GetColumnSpan(child);
            if (col >= slotColumn) {
                Controls::Grid::SetColumn(child, col + 1);
            } else if (col < slotColumn && col + span > slotColumn) {
                Controls::Grid::SetColumnSpan(child, span + 1);
            }
        }
    }
    Controls::Grid::SetColumn(slotElement, slotColumn);
    trayGrid.Children().Append(slotElement);
    g_slotRoot = slotElement;
    g_topTextBlock = topText;
    g_bottomTextBlock = bottomText;
    g_slotParentGrid = trayGrid;
    g_slotInjected = true;
    g_hasAppliedContent = false;
    g_injectRetryMs = 100;
    AttachSlotInteractions(slotElement);
    return true;
}

bool EnsurePrayerTraySlot(FrameworkElement mainStack) {
    if (IsSlotRootAlive()) {
        AttachSlotInteractions(g_slotRoot);
        return true;
    }
    auto stackPanelElement = FindMainStackIconStackPanel(mainStack);
    if (!stackPanelElement) return false;
    auto stackPanel = stackPanelElement.try_as<Controls::StackPanel>();
    if (!stackPanel) return false;
    PCWSTR xaml = LR"(<Button xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        Name="PrayerTraySlotRoot" Background="Transparent" BorderThickness="0"
        Padding="4,0,6,0" Margin="4,0,6,0" VerticalAlignment="Center">
        <StackPanel Orientation="Vertical"><TextBlock Name="PrayerTrayTop"
        FontFamily="Segoe UI Variable Text" FontSize="12" FlowDirection="LeftToRight"
        MaxWidth="92" TextTrimming="CharacterEllipsis" TextAlignment="Center"/><TextBlock Name="PrayerTrayBottom"
        FontFamily="Segoe UI Variable Text" FontSize="12" MaxWidth="92"
        TextTrimming="CharacterEllipsis" TextAlignment="Center"/>
        </StackPanel></Button>)";
    auto slotButton = Markup::XamlReader::Load(xaml).try_as<Controls::Button>();
    if (!slotButton) return false;
    stackPanel.Children().Append(slotButton);
    g_slotRoot = slotButton;
    g_topTextBlock = FindTextBlockByName(slotButton, L"PrayerTrayTop");
    g_bottomTextBlock = FindTextBlockByName(slotButton, L"PrayerTrayBottom");
    g_slotInjected = true;
    g_hasAppliedContent = false;
    g_injectRetryMs = 100;
    AttachSlotInteractions(slotButton);
    return true;
}

void TrackObservedElement(FrameworkElement element) {
    if (!element) return;
    for (auto it = g_observedElements.begin(); it != g_observedElements.end();) {
        if (!it->get()) it = g_observedElements.erase(it);
        else if (winrt::get_abi(it->get()) == winrt::get_abi(element)) return;
        else ++it;
    }
    g_observedElements.emplace_back(element);
    while (g_observedElements.size() > 64) g_observedElements.pop_front();
}

bool ApplyWin11Style(XamlRoot xamlRoot, const DisplayContent& content) {
    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (!child) return false;
    if (IsSlotRootAlive()) {
        ApplyWin11SlotContent(content);
        return true;
    }
    if (auto frameGrid = FindChildByName(child, L"SystemTrayFrameGrid")) {
        if (auto trayGrid = frameGrid.try_as<Controls::Grid>()) {
            if (EnsurePrayerTraySlotInTrayGrid(trayGrid)) {
                ApplyWin11SlotContent(content);
                return true;
            }
        }
    }
    if (auto trayGrid = FindTrayGridFromAnchor(child)) {
        if (EnsurePrayerTraySlotInTrayGrid(trayGrid)) {
            ApplyWin11SlotContent(content);
            return true;
        }
    }
    if (auto mainStack = ResolveMainStack(child)) {
        if (EnsurePrayerTraySlot(mainStack)) {
            ApplyWin11SlotContent(content);
            return true;
        }
    }
    return false;
}

bool RunFromWindowThread(HWND hWnd, WindowThreadAction action,
                         DWORD timeoutMs) {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Payload {
        WindowThreadAction action;
        std::atomic<bool> ran{false};
        explicit Payload(WindowThreadAction value) : action(std::move(value)) {}
    };
    using PayloadRef = std::shared_ptr<Payload>;

    const DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId || !action) return false;
    if (threadId == GetCurrentThreadId()) {
        try {
            action();
            return true;
        } catch (...) {
            return false;
        }
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                static const UINT runMessage = RegisterWindowMessageW(
                    L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == runMessage) {
                    std::unique_ptr<PayloadRef> holder(
                        reinterpret_cast<PayloadRef*>(cwp->lParam));
                    PayloadRef payload = *holder;
                    try {
                        payload->action();
                    } catch (...) {
                    }
                    payload->ran.store(true, std::memory_order_release);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    PayloadRef payload = std::make_shared<Payload>(std::move(action));
    auto* holder = new PayloadRef(payload);
    DWORD_PTR ignored = 0;
    const bool sent =
        SendMessageTimeoutW(hWnd, message, 0,
                            reinterpret_cast<LPARAM>(holder),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, timeoutMs,
                            &ignored) != 0;
    const bool ran = payload->ran.load(std::memory_order_acquire);
    if (!sent) {
        // A timed-out send can still be consumed by Windows. The hook owns the
        // heap holder in that case, which avoids a use-after-free during unload.
        UnhookWindowsHookEx(hook);
        return false;
    }
    if (!ran) delete holder;
    UnhookWindowsHookEx(hook);
    return ran;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid;
            WCHAR cls[32];
            if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, cls, 32) && _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void*);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original = nullptr;

HRESULT TryGetTaskbarElementAbi(HWND taskbar, void** result) {
    if (!result) return E_POINTER;
    *result = nullptr;
    if (!taskbar || !CTaskBand_ITaskListWndSite_vftable ||
        !CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        return E_NOINTERFACE;
    }

    HWND taskSwitch =
        reinterpret_cast<HWND>(GetPropW(taskbar, L"TaskbandHWND"));
    if (!taskSwitch) return E_HANDLE;
    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtrW(taskSwitch, 0));
    if (!taskBand) return E_POINTER;

    void* taskBandSite = taskBand;
    for (int index = 0;
         *reinterpret_cast<void**>(taskBandSite) !=
         CTaskBand_ITaskListWndSite_vftable;
         ++index) {
        if (index == 20) return E_NOINTERFACE;
        taskBandSite = reinterpret_cast<void**>(taskBandSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandSite, taskbarHostSharedPtr);
    auto cleanup = [&]() {
        if (taskbarHostSharedPtr[1]) {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
            taskbarHostSharedPtr[1] = nullptr;
        }
    };
    if (!taskbarHostSharedPtr[0]) {
        cleanup();
        return E_POINTER;
    }

    const BYTE* bytes =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (bytes[0] != 0x48 || bytes[1] != 0x83 || bytes[2] != 0xEC ||
        bytes[4] != 0x48 || bytes[5] != 0x83 || bytes[6] != 0xC1 ||
        bytes[7] > 0x7F) {
        cleanup();
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    }
    const size_t elementOffset = bytes[7];
    auto* elementUnknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset);
    if (!elementUnknown) {
        cleanup();
        return E_POINTER;
    }

    const HRESULT hr = elementUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), result);
    cleanup();
    return hr;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbar) {
    void* elementAbi = nullptr;
    if (FAILED(TryGetTaskbarElementAbi(taskbar, &elementAbi)) ||
        !elementAbi) {
        return nullptr;
    }
    FrameworkElement element{nullptr};
    winrt::attach_abi(element, elementAbi);
    return element ? element.XamlRoot() : nullptr;
}

struct SlotApplyParam {
    HWND taskbar = nullptr;
    DisplayContent content;
};

void ApplyWin11FromThread(const SlotApplyParam& param) {
    try {
        const DisplayContent& content = param.content;
        try {
            if (IsSlotRootAlive()) {
                ApplyWin11SlotContent(content);
                g_injectRetryMs = 100;
                return;
            }
        } catch (...) {
            g_slotInjected = false;
        }
        bool injected = false;
        try {
            if (auto xamlRoot = GetTaskbarXamlRoot(param.taskbar)) {
                injected = ApplyWin11Style(xamlRoot, content);
            }
        } catch (...) {
        }
        for (auto it = g_observedElements.begin(); it != g_observedElements.end();) {
            if (injected) break;
            auto element = it->get();
            if (!element) {
                it = g_observedElements.erase(it);
                continue;
            }
            try {
                if (auto xr = element.XamlRoot()) {
                    injected = ApplyWin11Style(xr, content);
                    break;
                }
            } catch (...) {
            }
            ++it;
        }
        try {
            if (IsSlotRootAlive()) {
                ApplyWin11SlotContent(content);
                g_injectRetryMs = 100;
                return;
            }
        } catch (...) {
            g_slotInjected = false;
        }
        if (!injected) {
            // Stay aggressive at first (100→200→400), then back off to 1.5s max.
            const int cur = g_injectRetryMs.load();
            g_injectRetryMs = std::min(1500, std::max(100, cur * 2));
            const ULONGLONG now = GetTickCount64();
            ULONGLONG next = g_nextInjectionFailureLogMs.load();
            if (now >= next &&
                g_nextInjectionFailureLogMs.compare_exchange_strong(
                    next, now + 30000)) {
                Wh_Log(L"Win11 tray slot not ready (attempt %d, observed %zu)",
                       g_applySlotAttempts.load(), g_observedElements.size());
            }
        }
    } catch (...) {
        g_slotInjected = false;
    }
}

void TryApplyWin11Slot() {
    if (g_backend.load() != TaskbarBackend::Win11Xaml) return;
    ++g_applySlotAttempts;
    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (!taskbar) return;
    DisplayContent content = FormatTaskbarDisplay();
    SlotApplyParam param{taskbar, content};
    if (!RunFromWindowThread(taskbar,
                             [param]() { ApplyWin11FromThread(param); })) {
        const int current = g_injectRetryMs.load();
        g_injectRetryMs = (std::min)(1500, (std::max)(100, current * 2));
    }
}

void ApplyEngineToBackends() {
    if (g_backend.load() == TaskbarBackend::Win11Xaml) {
        TryApplyWin11Slot();
    } else {
        ApplyWin10Content(FormatTaskbarDisplay());
    }
}

using IconView_IconView_t = void*(WINAPI*)(void*);
IconView_IconView_t IconView_IconView_Original = nullptr;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

FrameworkElement TryGetFrameworkElementFromImplementation(void* pThis) {
    if (!pThis) return nullptr;
    FrameworkElement element = nullptr;
    try {
        auto object = reinterpret_cast<IUnknown**>(pThis)[1];
        if (object) {
            object->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
        }
    } catch (...) {
    }
    return element;
}

void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);
    if (!g_unloading.load()) {
        // Constructors run before the visual tree is ready. Keep only a weak
        // reference and let the owned UI retry loop apply after layout settles.
        if (auto element = TryGetFrameworkElementFromImplementation(pThis)) {
            TrackObservedElement(element);
        }
        if (g_messageHwnd) {
            PostMessageW(g_messageHwnd, WM_PT_UI_TICK, 0, 0);
        }
    }
    return ret;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    void* fixedInfo = nullptr;
    UINT length = 0;
    HRSRC resource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (!resource) return nullptr;
    HGLOBAL loaded = LoadResource(module, resource);
    if (!loaded) return nullptr;
    void* data = LockResource(loaded);
    if (!data || !VerQueryValueW(data, L"\\", &fixedInfo, &length) ||
        length < sizeof(VS_FIXEDFILEINFO)) {
        return nullptr;
    }
    return static_cast<VS_FIXEDFILEINFO*>(fixedInfo);
}

HMODULE GetSystemTrayModuleHandle() {
    if (HMODULE module = GetModuleHandleW(L"SystemTray.dll")) {
        return module;
    }
    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
        const auto* version = GetModuleVersionInfo(module);
        const WORD major = version ? HIWORD(version->dwFileVersionMS) : 0;
        if (major && major < 2604) return module;
        return nullptr;
    }
    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
         &IconView_IconView_Original, IconView_IconView_Hook},
    };
    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"System tray symbol hook failed");
        return false;
    }
    return true;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR name) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded system tray module: %s", name ? name : L"(unknown)");
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        } else {
            g_systemTrayModuleHooked = false;
        }
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module) HandleLoadedModuleIfSystemTray(module, name);
    return module;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"taskbar.dll load failed (%u)", GetLastError());
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &std__Ref_count_base__Decref_Original},
    };
    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

void SelectBackend() {
    const bool win11 = DetectWin11();
    const auto settings = GetSettingsSnapshot();
    g_isWin11 = win11;
    if (win11 && !settings.oldTaskbarOnWin11) {
        g_backend = TaskbarBackend::Win11Xaml;
    } else {
        g_backend = TaskbarBackend::Win10Overlay;
    }
    Wh_Log(L"Backend: %s", g_backend.load() == TaskbarBackend::Win11Xaml ? L"Win11" : L"Win10");
}


void RestartUiTimer() {
    if (!g_messageHwnd) return;
    const int intervalMs = g_uiTimerIntervalMs.load();
    if (g_uiTimer && g_lastUiTimerIntervalMs == intervalMs) {
        return;
    }
    g_uiTimer = SetTimer(g_messageHwnd, 1, intervalMs, nullptr);
    g_lastUiTimerIntervalMs = intervalMs;
}

VOID CALLBACK FullscreenTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    bool fs = IsForegroundWindowFullscreen();
    if (fs != g_isFullscreen.load()) {
        g_isFullscreen = fs;
        ApplyEngineToBackends();
    }
}

void ClearWin11Slot() {
    try {
        if (g_slotParentGrid) {
            RemovePrayerTraySlotFromGrid(g_slotParentGrid);
        } else if (g_slotRoot) {
            g_slotRoot.Visibility(Visibility::Collapsed);
        }
    } catch (...) {
    }
    g_observedElements.clear();
    g_slotRoot = nullptr;
    g_slotButton = nullptr;
    g_topTextBlock = nullptr;
    g_bottomTextBlock = nullptr;
    g_slotParentGrid = nullptr;
    g_slotInjected = false;
    g_hasAppliedContent = false;
}

void ShutdownOwnedUiResources() {
    StopTimers();
    HideFlyout();
    DestroyFlyout();
    DestroyWin10Overlay();
    if (g_contextMenu) {
        DestroyMenu(g_contextMenu);
        g_contextMenu = nullptr;
    }
    if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
        if (!RunFromWindowThread(taskbar, ClearWin11Slot, 10000)) {
            Wh_Log(L"Timed out while removing the Win11 taskbar slot");
        }
    } else {
        ClearWin11Slot();
    }
}

LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == 1) {
                MaybeRequestScheduleRefresh();
                ApplyEngineToBackends();
                CheckNotifications();
                RestartUiTimer();
            } else if (wParam == 2) {
                RepositionWin10Overlay();
            } else if (wParam == 5 || wParam == 6 || wParam == 7) {
                // One-shot startup inject retries.
                KillTimer(hWnd, wParam);
                ApplyEngineToBackends();
            }
            return 0;
        case WM_PT_UI_TICK:
            ApplyEngineToBackends();
            return 0;
        case WM_PT_HIDE_FLYOUT:
            HideFlyout();
            return 0;
        case WM_PT_AFTER_INIT:
            ApplyEngineToBackends();
            SetTimer(hWnd, 5, 150, nullptr);
            SetTimer(hWnd, 6, 400, nullptr);
            SetTimer(hWnd, 7, 900, nullptr);
            return 0;
        case WM_PT_PLAY_SOUND:
            PlayNotificationSound();
            return 0;
        case WM_PT_SETTINGS_CHANGED: {
            // Settings can rebuild the tray XAML. Close the transient flyout
            // first so it never presents a stale or apparently busy surface.
            HideFlyout();
            const auto oldBackend = static_cast<TaskbarBackend>(wParam);
            const auto newBackend = g_backend.load();
            if (oldBackend != newBackend) {
                if (newBackend == TaskbarBackend::Win10Overlay) {
                    if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
                        RunFromWindowThread(
                            taskbar,
                            []() {
                                try {
                                    if (g_slotParentGrid) {
                                        RemovePrayerTraySlotFromGrid(
                                            g_slotParentGrid);
                                    } else if (g_slotRoot) {
                                        g_slotRoot.Visibility(
                                            Visibility::Collapsed);
                                    }
                                } catch (...) {
                                }
                            });
                    }
                } else {
                    DestroyWin10Overlay();
                }
            }
            ApplyEngineToBackends();
            return 0;
        }
        case WM_PT_SHUTDOWN:
            ShutdownOwnedUiResources();
            g_messageHwnd = nullptr;
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            DispatchCommand(static_cast<int>(LOWORD(wParam)));
            return 0;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool EnsureMessageWindow() {
    if (g_messageHwnd) return true;
    if (!g_messageClassRegistered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = MessageWndProc;
        wc.hInstance = g_moduleInstance;
        wc.lpszClassName = L"PrayerTrayMessageWnd";
        if (!RegisterClassW(&wc) &&
            GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Message window class registration failed (%u)",
                   GetLastError());
            return false;
        }
        g_messageClassRegistered = true;
    }
    g_messageHwnd = CreateWindowExW(0, L"PrayerTrayMessageWnd", L"PrayerTray", 0, 0, 0, 0, 0,
                                    HWND_MESSAGE, nullptr, g_moduleInstance, nullptr);
    if (!g_messageHwnd) {
        Wh_Log(L"Message window creation failed (%u)", GetLastError());
        return false;
    }
    return true;
}

void StartTimers() {
    RestartUiTimer();
    g_repositionTimer = SetTimer(g_messageHwnd, 2, 2000, nullptr);
    g_fullscreenTimer = SetTimer(nullptr, 3, 500, FullscreenTimerProc);
}

void StopTimers() {
    if (g_messageHwnd) {
        KillTimer(g_messageHwnd, 1);
        KillTimer(g_messageHwnd, 2);
        KillTimer(g_messageHwnd, 5);
        KillTimer(g_messageHwnd, 6);
        KillTimer(g_messageHwnd, 7);
    }
    g_uiTimer = 0;
    g_repositionTimer = 0;
    g_lastUiTimerIntervalMs = -1;
    if (g_fullscreenTimer) {
        KillTimer(nullptr, g_fullscreenTimer);
        g_fullscreenTimer = 0;
    }
}

DWORD WINAPI UiThreadProc(void*) {
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    } catch (...) {
    }
    g_uiThreadId = GetCurrentThreadId();
    const bool ready = EnsureMessageWindow();
    if (ready) {
        StartTimers();
        ApplyEngineToBackends();
    }
    if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
    if (!ready) {
        if (g_messageClassRegistered) {
            UnregisterClassW(L"PrayerTrayMessageWnd", g_moduleInstance);
            g_messageClassRegistered = false;
        }
        g_uiThreadId = 0;
        if (apartmentInitialized) winrt::uninit_apartment();
        return 1;
    }

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (g_messageHwnd) {
        ShutdownOwnedUiResources();
        DestroyWindow(g_messageHwnd);
        g_messageHwnd = nullptr;
    }
    if (g_messageClassRegistered) {
        UnregisterClassW(L"PrayerTrayMessageWnd", g_moduleInstance);
        g_messageClassRegistered = false;
    }
    g_uiThreadId = 0;
    if (apartmentInitialized) winrt::uninit_apartment();
    return 0;
}

bool StartUiThread() {
    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_uiReadyEvent) return false;
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread) {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
        return false;
    }
    if (WaitForSingleObject(g_uiReadyEvent, INFINITE) != WAIT_OBJECT_0 ||
        !g_messageHwnd) {
        if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        CloseHandle(g_uiReadyEvent);
        g_uiThread = nullptr;
        g_uiReadyEvent = nullptr;
        return false;
    }
    return true;
}

void StopUiThread() {
    if (g_messageHwnd) {
        if (!PostMessageW(g_messageHwnd, WM_PT_SHUTDOWN, 0, 0) &&
            g_uiThreadId) {
            PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        }
    } else if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_uiReadyEvent) {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
    }
}

bool IsShellExplorerProcess() {
    const HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        // Explorer can create the shell window slightly after process startup.
        return true;
    }
    DWORD shellProcessId = 0;
    GetWindowThreadProcessId(shellWindow, &shellProcessId);
    return shellProcessId == GetCurrentProcessId();
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"PrayerTray 3.1.4 initializing");
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<LPCWSTR>(&UiThreadProc),
                      &g_moduleInstance);
    if (!g_moduleInstance || !IsShellExplorerProcess()) {
        Wh_Log(L"Skipping non-shell explorer.exe process");
        return FALSE;
    }

    LoadSettings();
    SelectBackend();

    // Install Win11 hooks even when the compatibility overlay is currently
    // selected, so changing that setting doesn't require restarting Explorer.
    if (g_isWin11.load()) {
        if (!HookTaskbarDllSymbols()) {
            Wh_Log(L"taskbar.dll symbols unavailable; using IconView fallback");
        }
        bool systemTrayHookReady = false;
        if (HMODULE module = GetSystemTrayModuleHandle()) {
            systemTrayHookReady = HookSystemTraySymbols(module);
            g_systemTrayModuleHooked = systemTrayHookReady;
            if (!systemTrayHookReady) {
                Wh_Log(L"System tray hooks unavailable; watching for the current module");
            }
        }
        if (!systemTrayHookReady) {
            HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
            auto loadLibraryEx = kernelBase
                                     ? reinterpret_cast<LoadLibraryExW_t>(
                                           GetProcAddress(kernelBase,
                                                          "LoadLibraryExW"))
                                     : nullptr;
            if (loadLibraryEx) {
                WindhawkUtils::SetFunctionHook(loadLibraryEx,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
            } else {
                Wh_Log(L"LoadLibraryExW hook unavailable");
            }
        }
    }

    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    } catch (...) {
        // Explorer may already own this thread's COM apartment.
    }
    try {
        BootstrapFromCache();
    } catch (...) {
        Wh_Log(L"Cache bootstrap failed");
    }
    if (apartmentInitialized) winrt::uninit_apartment();
    g_injectRetryMs = 100;
    g_uiTimerIntervalMs = 100;
    if (!StartUiThread()) {
        Wh_Log(L"UI thread startup failed");
        return FALSE;
    }
    if (!StartRefreshWorker()) {
        Wh_Log(L"Refresh worker startup failed");
        StopUiThread();
        return FALSE;
    }
    RequestRefresh(false);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (g_isWin11.load() && !g_systemTrayModuleHooked.load()) {
        if (HMODULE module = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (HookSystemTraySymbols(module)) {
                    Wh_ApplyHookOperations();
                } else {
                    g_systemTrayModuleHooked = false;
                }
            }
        }
    }
    if (g_messageHwnd) {
        PostMessageW(g_messageHwnd, WM_PT_AFTER_INIT, 0, 0);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    StopRefreshWorker();
    StopUiThread();
    mciSendStringW(L"close prayertray_notify", nullptr, 0, nullptr);
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    const auto oldBackend = g_backend.load();
    LoadSettings();
    SelectBackend();
    const auto newSettings = GetSettingsSnapshot();
    // Windhawk invokes this callback after Save settings. Preview on every
    // save while the option is enabled, not only on a false -> true change.
    if (newSettings.playPrayerNotification) {
        g_soundPreviewRequested = true;
    }
    RequestRefresh(true);
    if (g_messageHwnd) {
        PostMessageW(g_messageHwnd, WM_PT_SETTINGS_CHANGED,
                     static_cast<WPARAM>(oldBackend), 0);
    }
}
