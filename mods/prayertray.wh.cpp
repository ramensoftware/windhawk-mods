// ==WindhawkMod==
// @id prayertray
// @name PrayerTray
// @description Islamic prayer times on the taskbar, next to the clock.
// @version 2.0.12
// @author Omar Alhami (mar)
// @github https://github.com/n0tmar
// @homepage https://github.com/n0tmar/PrayerTray
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshlwapi -lwinhttp -lwinmm -lversion -lcomctl32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# PrayerTray

Shows the next Islamic prayer on the Windows taskbar so you can see it while
you work, without opening another app. Left-click opens today's times
(Fajr, Dhuhr, Asr, Maghrib, Isha). Works on Windows 10 and 11.

## Location

Uses Windows Location when it is allowed, otherwise IP geolocation.
You can also set city or coordinates in the mod settings.
Privacy > Location > allow desktop apps if you want Windows Location.

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
- showNotificationIcon: false
  $name: Notification icon
  $description: Optional icon in the notification area
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
  $description: Optional decimals; more precise than city alone
- manualLongitude: ""
  $name: Longitude
  $description: Optional decimals; more precise than city alone
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
#define WH_MOD_VERSION L"2.0.12"
#endif

#include <windhawk_utils.h>

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <future>
#include <list>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <dwmapi.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <winhttp.h>

#undef GetCurrentTime

#include <winrt/Windows.Devices.Geolocation.h>
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

constexpr wchar_t kColonRatio = L'\u2236';
constexpr wchar_t kLre = L'\u202A';
constexpr wchar_t kPdf = L'\u202C';
constexpr int kNotificationLeadMinutes = 1;
constexpr int kCurrentPrayerHighlightMinutes = 2;
constexpr int kCacheStaleHours = 6;
constexpr double kClusterDistanceKm = 80.0;
constexpr int kScheduleRefreshIntervalMs = 15 * 60 * 1000;
constexpr int kGeoAccessTimeoutMs = 8000;
constexpr int kRunFromWindowTimeoutMs = 3000;

enum class PrayerName { Fajr, Dhuhr, Asr, Maghrib, Isha, Count };

enum class TaskbarBackend { Win11Xaml, Win10Overlay };

enum class ContentMode { Countdown, Time, Smart };

struct ModSettings {
    bool showOnTaskbar = true;
    bool showNotificationIcon = false;
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
std::mutex g_scheduleMutex;
LocationInfo g_location;
PrayerSchedule g_currentSchedule;
PrayerSchedule g_nextSchedule;
bool g_hasCurrentSchedule = false;
bool g_hasNextSchedule = false;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_refreshRequested{true};
std::atomic<bool> g_refreshRunning{false};
std::atomic<bool> g_forceLocationRefresh{false};
std::atomic<bool> g_lastRefreshFailed{false};
std::atomic<bool> g_isFullscreen{false};
std::atomic<TaskbarBackend> g_backend{TaskbarBackend::Win11Xaml};
std::atomic<bool> g_isWin11{false};
std::atomic<int> g_uiTimerIntervalMs{500};

HWND g_messageHwnd = nullptr;
UINT_PTR g_uiTimer = 0;
UINT_PTR g_repositionTimer = 0;
UINT_PTR g_fullscreenTimer = 0;
UINT_PTR g_scheduleTimer = 0;
HWND g_win10OverlayHwnd = nullptr;
HWND g_flyoutHwnd = nullptr;
bool g_flyoutVisible = false;
HHOOK g_flyoutMouseHook = nullptr;
ULONGLONG g_flyoutSuppressDismissUntil = 0;
NOTIFYICONDATAW g_trayIcon{};
bool g_trayIconAdded = false;
HMENU g_contextMenu = nullptr;

std::unordered_set<std::wstring> g_playedNotificationKeys;
int g_notificationActiveDateKey = 0;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_slotInjected;
std::atomic<int> g_applySlotAttempts;
std::atomic<int> g_injectRetryMs{400};
std::atomic<ULONGLONG> g_lastMeasureInjectAttemptMs{0};
int g_lastUiTimerIntervalMs = -1;
bool g_hasOverlayPlacement = false;
int g_lastOverlayX = 0;
int g_lastOverlayY = 0;
int g_lastOverlayW = 0;
int g_lastOverlayH = 0;
bool g_lastOverlayShown = false;
using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement, &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;
std::list<FrameworkElementLoadedEventRevoker> g_loadedRevokers;
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
constexpr int kCmdToggleWidget = 4;
constexpr UINT WM_PT_REFRESH = WM_APP + 1;
constexpr UINT WM_PT_UI_TICK = WM_APP + 2;
constexpr UINT WM_PT_HIDE_FLYOUT = WM_APP + 3;


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
    TzSpecificLocalTimeToSystemTime(nullptr, &st, &utc);
    FILETIME ft{};
    SystemTimeToFileTime(&utc, &ft);
    ULARGE_INTEGER uli{};
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return (uli.QuadPart - 116444736000000000ULL) / 10000000ULL;
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
    wchar_t* end = nullptr;
    out = wcstod(text.c_str(), &end);
    return end && end != text.c_str();
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
    s.showNotificationIcon = Wh_GetIntSetting(L"showNotificationIcon") != 0;
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
    g_settings = std::move(s);
}

bool IsRtlLanguage() {
    return g_settings.language == L"ar" || g_settings.language == L"ur";
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
    RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                     reinterpret_cast<LPBYTE>(&value), &size);
    RegCloseKey(key);
    return value == 1;
}

std::wstring TaskbarTextColorHex() {
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
    std::wstring lang = g_settings.language;
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
    std::wstring text;
    if (g_settings.use24Hour) {
        wchar_t buf[16];
        swprintf(buf, 16, L"%02u%c%02u", time.wHour, kColonRatio, time.wMinute);
        text = buf;
    } else {
        int hour = time.wHour % 12;
        if (hour == 0) {
            hour = 12;
        }
        const std::wstring period =
            time.wHour < 12 ? Loc(L"TimePeriod_AM") : Loc(L"TimePeriod_PM");
        wchar_t buf[32];
        // Arabic/Urdu: period on the left of the digits (ص 4:21), not 4:21 ص.
        if (IsRtlLanguage()) {
            swprintf(buf, 32, L"%s %d%c%02u", period.c_str(), hour, kColonRatio,
                     time.wMinute);
        } else {
            swprintf(buf, 32, L"%d%c%02u %s", hour, kColonRatio, time.wMinute,
                     period.c_str());
        }
        text = buf;
    }
    return StabilizeCompactText(text);
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


std::optional<std::string> HttpGetUtf8(const std::wstring& url, bool useHttps = true) {
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
    const bool https = useHttps || uc.nScheme == INTERNET_SCHEME_HTTPS;
    HINTERNET session =
        WinHttpOpen(L"PrayerTray/2.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        return std::nullopt;
    }
    WinHttpSetTimeouts(session, 2000, 2000, 4000, 4000);
    HINTERNET connect = WinHttpConnect(session, host, uc.nPort, 0);
    if (!connect) {
        WinHttpCloseHandle(session);
        return std::nullopt;
    }
    std::wstring objectName = std::wstring(path) + extra;
    DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET request = WinHttpOpenRequest(
        connect, L"GET", objectName.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!request) {
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return std::nullopt;
    }
    if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request, nullptr)) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return std::nullopt;
    }
    std::string body;
    DWORD available = 0;
    do {
        if (!WinHttpQueryDataAvailable(request, &available)) {
            break;
        }
        if (available == 0) {
            break;
        }
        size_t offset = body.size();
        body.resize(offset + available);
        DWORD read = 0;
        if (!WinHttpReadData(request, body.data() + offset, available, &read)) {
            body.resize(offset);
            break;
        }
        body.resize(offset + read);
    } while (available > 0);
    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return body.empty() ? std::nullopt : std::optional<std::string>{std::move(body)};
}

std::optional<std::string_view> JsonFindValue(std::string_view json,
                                              std::string_view key) {
    std::string needle = std::string("\"") + std::string(key) + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string_view::npos) {
        return std::nullopt;
    }
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) {
        ++pos;
    }
    if (pos >= json.size()) {
        return std::nullopt;
    }
    if (json[pos] == '"') {
        ++pos;
        size_t end = pos;
        while (end < json.size() && json[end] != '"') {
            if (json[end] == '\\' && end + 1 < json.size()) {
                end += 2;
            } else {
                ++end;
            }
        }
        return json.substr(pos, end - pos);
    }
    size_t end = pos;
    while (end < json.size() && json[end] != ',' && json[end] != '}' &&
           json[end] != ']') {
        ++end;
    }
    return json.substr(pos, end - pos);
}

std::optional<double> JsonGetDouble(std::string_view json, std::string_view key) {
    if (auto val = JsonFindValue(json, key)) {
        try {
            return std::stod(std::string(*val));
        } catch (...) {
        }
    }
    return std::nullopt;
}

std::wstring JsonGetStringW(std::string_view json, std::string_view key) {
    if (auto val = JsonFindValue(json, key)) {
        return Utf8ToWide(*val);
    }
    return L"";
}

std::optional<std::string_view> JsonFindObject(std::string_view json,
                                               std::string_view key) {
    std::string needle = std::string("\"") + std::string(key) + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string_view::npos) {
        return std::nullopt;
    }
    pos += needle.size();
    while (pos < json.size() && json[pos] != '{') {
        ++pos;
    }
    if (pos >= json.size()) {
        return std::nullopt;
    }
    int depth = 0;
    size_t start = pos;
    for (; pos < json.size(); ++pos) {
        if (json[pos] == '{') {
            ++depth;
        } else if (json[pos] == '}') {
            --depth;
            if (depth == 0) {
                return json.substr(start, pos - start + 1);
            }
        }
    }
    return std::nullopt;
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
    return L"";
}

int ResolveCalculationMethod(const LocationInfo& loc) {
    if (g_settings.calculationMethod > 0) {
        return g_settings.calculationMethod;
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
    return earthRadiusKm * 2 * atan2(sqrt(h), sqrt(1 - h));
}

std::optional<IpCandidate> TryFetchIpApi() {
    auto body = HttpGetUtf8(L"http://ip-api.com/json/?fields=status,message,lat,lon,city,regionName,country,timezone", false);
    if (!body) return std::nullopt;
    if (JsonGetStringW(*body, "status") != L"success") return std::nullopt;
    IpCandidate c;
    c.provider = L"ip-api";
    c.weight = 2;
    c.latitude = JsonGetDouble(*body, "lat").value_or(0);
    c.longitude = JsonGetDouble(*body, "lon").value_or(0);
    c.city = JsonGetStringW(*body, "city");
    c.region = JsonGetStringW(*body, "regionName");
    c.country = JsonGetStringW(*body, "country");
    c.timezone = JsonGetStringW(*body, "timezone");
    return c;
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
    c.latitude = wcstod(loc.c_str(), nullptr);
    c.longitude = wcstod(loc.substr(comma + 1).c_str(), nullptr);
    c.city = JsonGetStringW(*body, "city");
    c.region = JsonGetStringW(*body, "region");
    c.country = ReadCountryNameFromCode(JsonGetStringW(*body, "country"));
    c.timezone = JsonGetStringW(*body, "timezone");
    return c;
}

std::optional<IpCandidate> TryFetchIpWho() {
    auto body = HttpGetUtf8(L"https://ipwho.is/");
    if (!body) return std::nullopt;
    auto success = JsonFindValue(*body, "success");
    if (!success || *success != "true") return std::nullopt;
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
    return c;
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
    // Prefer reverse-geocode city over IP label.
    result.city.clear();
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
    c.timezone = JsonGetStringW(*body, "timeZones");
    if (c.timezone.empty()) {
        if (auto zones = JsonFindObject(*body, "timeZones")) {
            (void)zones;
        }
        // freeipapi may return timeZones as an array; first quoted value is enough.
        auto pos = body->find("\"timeZones\"");
        if (pos != std::string::npos) {
            auto q1 = body->find('"', body->find('[', pos));
            if (q1 != std::string::npos) {
                auto q2 = body->find('"', q1 + 1);
                if (q2 != std::string::npos) {
                    c.timezone = Utf8ToWide(body->substr(q1 + 1, q2 - q1 - 1));
                }
            }
        }
    }
    return c;
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
    return c;
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
        return L"Mecca";
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
    if (result.timezone.empty()) {
        result.timezone = JsonGetStringW(*body, "localityInfo");  // ignored if wrong shape
        result.timezone.clear();
    }
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
    LocationInfo loc;
    loc.source = L"manual";
    loc.resolvedAtUtcMs = GetUtcNowMs();
    loc.city = FixCityName(Trim(g_settings.manualCity));
    loc.country = Trim(g_settings.manualCountry);
    loc.timezone = GetDefaultTimezoneForCountry(loc.country);
    double lat = 0, lon = 0;
    if (ParseDouble(g_settings.manualLatitude, lat) &&
        ParseDouble(g_settings.manualLongitude, lon)) {
        loc.latitude = lat;
        loc.longitude = lon;
        return loc;
    }
    if (!loc.city.empty() && !loc.country.empty()) {
        loc.source = L"manual-city";
        return loc;
    }
    loc.country = L"Unknown";
    return loc;
}

std::wstring JsonEscape(const std::wstring& value) {
    std::wstring out;
    out.reserve(value.size());
    for (wchar_t ch : value) {
        if (ch == L'\\' || ch == L'"') {
            out.push_back(L'\\');
            out.push_back(ch);
        } else if (ch == L'\n') {
            out += L"\\n";
        } else {
            out.push_back(ch);
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

void SaveLastLocation(const LocationInfo& loc) {
    const auto path = GetLastLocationPath();
    if (path.empty() || (loc.latitude == 0 && loc.longitude == 0 && loc.city.empty())) {
        return;
    }
    std::ofstream out(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!out) {
        return;
    }
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
}

std::optional<LocationInfo> LoadLastLocation() {
    const auto path = GetLastLocationPath();
    if (path.empty()) {
        return std::nullopt;
    }
    std::ifstream in(path.c_str(), std::ios::binary);
    if (!in) {
        return std::nullopt;
    }
    std::string json((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (json.empty()) {
        return std::nullopt;
    }
    LocationInfo loc;
    loc.latitude = JsonGetDouble(json, "Latitude").value_or(0);
    loc.longitude = JsonGetDouble(json, "Longitude").value_or(0);
    loc.city = FixCityName(JsonGetStringW(json, "City"));
    loc.region = JsonGetStringW(json, "Region");
    loc.country = JsonGetStringW(json, "Country");
    loc.timezone = JsonGetStringW(json, "Timezone");
    loc.source = L"last-known";
    loc.resolvedAtUtcMs = GetUtcNowMs();
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

std::optional<LocationInfo> TryWindowsLocation() {
    try {
        using namespace winrt::Windows::Devices::Geolocation;
        using namespace winrt::Windows::Foundation;
        using namespace std::chrono_literals;

        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
        } catch (winrt::hresult_error const&) {
        }

        // RequestAccessAsync has no built-in timeout and can block the refresh
        // pipeline forever if the consent prompt never resolves.
        auto accessPromise =
            std::make_shared<std::promise<GeolocationAccessStatus>>();
        auto accessFuture = accessPromise->get_future();
        std::thread([accessPromise]() {
            try {
                try {
                    winrt::init_apartment(winrt::apartment_type::multi_threaded);
                } catch (...) {
                }
                accessPromise->set_value(Geolocator::RequestAccessAsync().get());
            } catch (...) {
                try {
                    accessPromise->set_exception(std::current_exception());
                } catch (...) {
                }
            }
        }).detach();

        std::optional<GeolocationAccessStatus> access;
        if (accessFuture.wait_for(std::chrono::milliseconds(kGeoAccessTimeoutMs)) !=
            std::future_status::ready) {
            Wh_Log(L"Windows Location access timed out");
            return std::nullopt;
        }
        try {
            access = accessFuture.get();
        } catch (...) {
            Wh_Log(L"Windows Location access failed");
            return std::nullopt;
        }
        if (!access || *access != GeolocationAccessStatus::Allowed) {
            Wh_Log(L"Windows Location denied (%d)",
                   access ? static_cast<int>(*access) : -1);
            return std::nullopt;
        }

        Geolocator locator;
        const auto status = locator.LocationStatus();
        if (status == PositionStatus::Disabled ||
            status == PositionStatus::NotAvailable) {
            Wh_Log(L"Windows Location off (%d)",
                   static_cast<int>(status));
            return std::nullopt;
        }

        locator.DesiredAccuracy(PositionAccuracy::Default);
        try {
            locator.DesiredAccuracyInMeters(1000);
        } catch (...) {
        }

        const auto position =
            locator.GetGeopositionAsync(TimeSpan{30s}, TimeSpan{3s}).get();
        if (!position) {
            Wh_Log(L"Windows Location: no fix");
            return std::nullopt;
        }

        const auto coord = position.Coordinate().Point().Position();
        if (coord.Latitude == 0 && coord.Longitude == 0) {
            Wh_Log(L"Windows Location: empty coords");
            return std::nullopt;
        }

        LocationInfo loc =
            FillFromCoords({}, coord.Latitude, coord.Longitude, L"windows");
        Wh_Log(L"Location (windows): %s, %s (%.5f, %.5f)",
               loc.city.c_str(), loc.country.c_str(), loc.latitude, loc.longitude);
        return loc;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Windows Location failed: 0x%08X %s",
               static_cast<unsigned>(ex.code()), ex.message().c_str());
        return std::nullopt;
    } catch (...) {
        Wh_Log(L"Windows Location failed");
        return std::nullopt;
    }
}

LocationInfo TryIpLocation() {
    Wh_Log(L"Trying IP location");
    std::vector<std::future<std::optional<IpCandidate>>> jobs;
    jobs.push_back(std::async(std::launch::async, TryFetchIpApi));
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
    Wh_Log(L"Location (ip): %s, %s (%.4f, %.4f) [%s]",
           loc.city.c_str(), loc.country.c_str(), loc.latitude, loc.longitude,
           winner.provider.c_str());
    return loc;
}

LocationInfo ResolveLocation() {
    if (g_settings.useManualLocation) {
        auto loc = ResolveManualLocation();
        SaveLastLocation(loc);
        return loc;
    }

    // Prefer a known location immediately; refresh can replace it later.
    auto cachedLoc = LoadLastLocation();

    if (auto loc = TryWindowsLocation()) {
        SaveLastLocation(*loc);
        return *loc;
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
    outTime = outDate;
    outTime.wHour = static_cast<WORD>(wcstoul(clean.substr(0, colon).c_str(), nullptr, 10));
    outTime.wMinute = static_cast<WORD>(wcstoul(clean.substr(colon + 1).c_str(), nullptr, 10));
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
        auto val = JsonFindValue(timingsJson, key);
        if (!val) continue;
        PrayerEntry entry;
        entry.name = name;
        if (ParseTimeOnly(Utf8ToWide(*val), date, entry.time)) {
            schedule.prayers.push_back(entry);
        }
    }
    return schedule;
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
    std::wstring tz = loc.timezone.empty() ? L"notz" : SanitizeCachePart(loc.timezone);
    wchar_t path[512];
    swprintf(path, 512, L"%04d-%02d-%02d_%s_m%d_%s.json", year, month, day,
             key.c_str(), method, tz.c_str());
    return GetCacheFolder() + L"\\" + path;
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
               std::to_wstring(loc.longitude) + L"&method=" + std::to_wstring(method);
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
              std::to_wstring(loc.longitude) + L"&method=" + std::to_wstring(method);
        if (!loc.timezone.empty()) {
            url += L"&timezonestring=" + UrlEncode(loc.timezone);
        }
    }
    return url;
}

PrayerSchedule FetchScheduleFromApi(int year, int month, int day, LocationInfo loc) {
    int method = ResolveCalculationMethod(loc);
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
        return BuildScheduleFromTimingsJson(*timings, year, month, day, loc, tz);
    };
    try {
        auto url = BuildIslamicAppUrl(year, month, day, loc, method);
        auto body = HttpGetUtf8(url);
        if (!body) throw std::runtime_error("islamic.app failed");
        return parseResponse(*body);
    } catch (...) {
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
    std::ifstream in(path.c_str(), std::ios::binary);
    if (!in) return std::nullopt;
    std::string json((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (json.empty()) return std::nullopt;
    PrayerSchedule schedule;
    auto dateStr = JsonGetStringW(json, "Date");
    if (!dateStr.empty()) {
        schedule.year = _wtoi(dateStr.substr(0, 4).c_str());
        schedule.month = _wtoi(dateStr.substr(5, 2).c_str());
        schedule.day = _wtoi(dateStr.substr(8, 2).c_str());
    } else {
        schedule.year = year;
        schedule.month = month;
        schedule.day = day;
    }
    schedule.location = loc;
    schedule.fetchedAtUtcMs = GetUtcNowMs();
    if (auto locObj = JsonFindObject(json, "Location")) {
        schedule.location.city = JsonGetStringW(*locObj, "City");
        schedule.location.country = JsonGetStringW(*locObj, "Country");
        schedule.location.timezone = JsonGetStringW(*locObj, "Timezone");
        schedule.location.latitude = JsonGetDouble(*locObj, "Latitude").value_or(loc.latitude);
        schedule.location.longitude = JsonGetDouble(*locObj, "Longitude").value_or(loc.longitude);
    }
    if (auto prayersArr = json.find("\"Prayers\""); prayersArr != std::string::npos) {
        SYSTEMTIME date{};
        date.wYear = static_cast<WORD>(schedule.year);
        date.wMonth = static_cast<WORD>(schedule.month);
        date.wDay = static_cast<WORD>(schedule.day);
        for (int i = 0; i < static_cast<int>(PrayerName::Count); ++i) {
            auto name = static_cast<PrayerName>(i);
            std::string key = "\"Name\":" + std::to_string(static_cast<int>(name));
            (void)key;
        }
    }
    auto timings = JsonFindObject(json, "timings");
    if (timings) {
        return BuildScheduleFromTimingsJson(*timings, year, month, day, loc, loc.timezone);
    }
    for (int i = 0; i < static_cast<int>(PrayerName::Count); ++i) {
        auto name = static_cast<PrayerName>(i);
        std::string search = std::string("\"") + WideToUtf8(PrayerApiKey(name)) + "\"";
        if (json.find(search) != std::string::npos) {
            return BuildScheduleFromTimingsJson(json, year, month, day, loc, loc.timezone);
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
            st.wHour = static_cast<WORD>(_wtoi(timeText.substr(11, 2).c_str()));
            st.wMinute = static_cast<WORD>(_wtoi(timeText.substr(14, 2).c_str()));
            st.wSecond = 0;
            PrayerEntry entry;
            entry.name = static_cast<PrayerName>(nameIdx);
            entry.time = st;
            schedule.prayers.push_back(entry);
        }
        pos = end;
    }
    if (schedule.prayers.empty()) return std::nullopt;
    return schedule;
}

void SaveCache(const PrayerSchedule& schedule, int method) {
    auto path = BuildCachePath(schedule.year, schedule.month, schedule.day,
                              schedule.location, method);
    std::ofstream out(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!out) return;
    out << "{\n";
    wchar_t dateBuf[16];
    swprintf(dateBuf, 16, L"%04d-%02d-%02d", schedule.year, schedule.month, schedule.day);
    out << "  \"Date\": \"" << WideToUtf8(dateBuf) << "\",\n";
    out << "  \"FetchedAtUtc\": \"" << WideToUtf8(std::to_wstring(schedule.fetchedAtUtcMs)) << "\",\n";
    out << "  \"Location\": {\n";
    out << "    \"Latitude\": " << schedule.location.latitude << ",\n";
    out << "    \"Longitude\": " << schedule.location.longitude << ",\n";
    out << "    \"City\": \"" << WideToUtf8(schedule.location.city) << "\",\n";
    out << "    \"Country\": \"" << WideToUtf8(schedule.location.country) << "\",\n";
    out << "    \"Timezone\": \"" << WideToUtf8(schedule.location.timezone) << "\"\n";
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
}

PrayerSchedule GetScheduleForDate(int year, int month, int day, LocationInfo loc) {
    int method = ResolveCalculationMethod(loc);
    if (auto cached = LoadCache(year, month, day, loc, method)) {
        if (!IsCacheStale(*cached, year, month, day, loc)) {
            return *cached;
        }
    }
    auto schedule = FetchScheduleFromApi(year, month, day, loc);
    SaveCache(schedule, method);
    return schedule;
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
    DisplayContent content;
    content.color = TaskbarTextColorHex();
    if (!g_settings.showOnTaskbar || g_isFullscreen.load()) {
        content.visible = false;
        return content;
    }
    auto next = GetNextPrayer();
    const bool slotReady =
        g_backend.load() != TaskbarBackend::Win11Xaml || g_slotInjected.load();
    if (!next) {
        content.visible = true;
        if (g_lastRefreshFailed.load() && !g_refreshRunning.load()) {
            content.top = StabilizeCompactText(Loc(L"Unavailable"));
            content.bottom = Loc(L"Prayer");
        } else {
            content.top = StabilizeCompactText(L"...");
            content.bottom = Loc(L"Detecting");
        }
        g_uiTimerIntervalMs = slotReady ? 2000 : g_injectRetryMs.load();
        return content;
    }
    content.visible = true;
    content.bottom = GetPrayerName(next->name);
    switch (g_settings.contentMode) {
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

// Notification sound is downloaded once to %APPDATA%\PrayerTray\audio\
// from the GitHub repo (keeps this Windhawk single-file mod small).

constexpr unsigned kNotificationSoundMinBytes = 1000;

std::wstring GetNotificationSoundPath() {
    auto base = GetAppDataPrayerTrayPath();
    if (base.empty()) {
        return L"";
    }
    return base + L"\\audio\\allah-akbar.mp3";
}

bool EnsureNotificationSoundFile() {
    auto path = GetNotificationSoundPath();
    if (path.empty()) {
        Wh_Log(L"Notification sound path unavailable");
        return false;
    }
    auto base = GetAppDataPrayerTrayPath();
    CreateDirectoryW(base.c_str(), nullptr);
    CreateDirectoryW((base + L"\\audio").c_str(), nullptr);

    WIN32_FILE_ATTRIBUTE_DATA attrs{};
    if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attrs)) {
        ULARGE_INTEGER size{};
        size.HighPart = attrs.nFileSizeHigh;
        size.LowPart = attrs.nFileSizeLow;
        if (size.QuadPart >= kNotificationSoundMinBytes) {
            return true;
        }
    }

    auto body = HttpGetUtf8(
        L"https://raw.githubusercontent.com/n0tmar/PrayerTray/main/windhawk/audio/allah-akbar.mp3");
    if (!body || body->size() < kNotificationSoundMinBytes) {
        Wh_Log(L"Notification sound download failed");
        return false;
    }

    std::ofstream out(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!out) {
        Wh_Log(L"Notification sound write failed");
        return false;
    }
    out.write(body->data(), static_cast<std::streamsize>(body->size()));
    if (!out) {
        Wh_Log(L"Notification sound write incomplete");
        return false;
    }
    return true;
}

void PlayNotificationSound() {
    if (!EnsureNotificationSoundFile()) {
        return;
    }
    const auto path = GetNotificationSoundPath();
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
    // MCI volume is 0–1000; match the old app's ~12% level.
    mciSendStringW(L"setaudio prayertray_notify volume to 120", nullptr, 0,
                   nullptr);
    if (mciSendStringW(L"play prayertray_notify", nullptr, 0, nullptr) != 0) {
        Wh_Log(L"Notification sound play failed");
    }
}

void CheckNotifications() {
    if (!g_settings.playPrayerNotification) return;
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
        const int schedKey =
            DateKey(sched.year, sched.month, sched.day);
        for (const auto& p : sched.prayers) {
            ULONGLONG t = PrayerUnixSeconds(p);
            if (nowSec + leadSec < t) continue;
            if (nowSec >= t) continue;
            wchar_t key[64];
            swprintf(key, 64, L"%d:%d", schedKey, static_cast<int>(p.name));
            if (g_playedNotificationKeys.insert(key).second) {
                PlayNotificationSound();
                return true;
            }
        }
        return false;
    };
    if (consider(g_currentSchedule)) return;
    if (g_hasNextSchedule) consider(g_nextSchedule);
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
    SYSTEMTIME now = GetLocalNow();
    const int method = ResolveCalculationMethod(loc);
    auto today = LoadCache(now.wYear, now.wMonth, now.wDay, loc, method);
    if (!today || today->prayers.empty()) {
        return false;
    }

    SYSTEMTIME tomorrow = now;
    AddDays(tomorrow, 1);
    auto nextDay = LoadCache(tomorrow.wYear, tomorrow.wMonth, tomorrow.wDay, loc, method);

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

void RefreshSchedulesWorker() {
    if (g_refreshRunning.exchange(true)) return;
    try {
        LocationInfo loc;
        const bool fullLocation = g_forceLocationRefresh.exchange(false);

        if (g_settings.useManualLocation) {
            loc = ResolveManualLocation();
            SaveLastLocation(loc);
        } else if (!fullLocation) {
            if (auto last = LoadLastLocation()) {
                loc = *last;
            } else {
                loc = ResolveLocation();
            }
        } else {
            loc = ResolveLocation();
        }

        SYSTEMTIME now = GetLocalNow();
        auto today = GetScheduleForDate(now.wYear, now.wMonth, now.wDay, loc);
        SYSTEMTIME tomorrow = now;
        AddDays(tomorrow, 1);
        PrayerSchedule nextDay;
        try {
            nextDay = GetScheduleForDate(tomorrow.wYear, tomorrow.wMonth, tomorrow.wDay, loc);
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
    } catch (const std::exception& ex) {
        Wh_Log(L"Refresh failed: %S", ex.what());
        g_lastRefreshFailed = true;
    } catch (...) {
        Wh_Log(L"Refresh failed");
        g_lastRefreshFailed = true;
    }
    g_refreshRunning = false;
    g_refreshRequested = false;
    if (g_messageHwnd) {
        PostMessageW(g_messageHwnd, WM_PT_UI_TICK, 0, 0);
    }
}

void RequestRefresh(bool fullLocation = false) {
    if (fullLocation) {
        g_forceLocationRefresh = true;
    }
    g_refreshRequested = true;
    std::thread(RefreshSchedulesWorker).detach();
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
        if (!g_hasCurrentSchedule) {
            needsRefresh = g_lastRefreshFailed.load();
        } else {
            const int schedKey = DateKey(g_currentSchedule.year, g_currentSchedule.month,
                                         g_currentSchedule.day);
            needsRefresh = schedKey != todayKey;
        }
    }
    if (needsRefresh) {
        RequestRefresh(false);
    }
}

void OpenWindhawkSettings() {
    ShellExecuteW(nullptr, L"open", L"windhawk:", nullptr, nullptr, SW_SHOWNORMAL);
}

// Forward declarations
void DispatchCommand(int commandId);
void ShowFlyout();
void HideFlyout();
void ShowContextMenu(HWND anchor);
void ApplyEngineToBackends();
void EnsureWin10Overlay();
void RepositionWin10Overlay();
void UpdateTrayIcon();
void DestroyFlyout();
void DestroyWin10Overlay();
void RemoveTrayIcon();
HWND FindCurrentProcessTaskbarWnd();


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
    const auto now = GetTickCount64();
    if (g_lastCommandId.load() == commandId && now - g_lastCommandTick.load() < 250) {
        return;
    }
    g_lastCommandId = commandId;
    g_lastCommandTick = now;
    switch (commandId) {
        case kCmdShowFlyout:
            ShowFlyout();
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
            bool light = IsLightTheme();
            HBRUSH bg = CreateSolidBrush(light ? RGB(252, 252, 252) : RGB(32, 32, 32));
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, light ? RGB(26, 26, 26) : RGB(255, 255, 255));
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
            SetTextColor(hdc, light ? RGB(100, 100, 100) : RGB(180, 180, 180));
            DrawTextW(hdc, subtitle.c_str(), -1, &subRc, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
            auto entries = GetTodayEntriesWithStatus();
            int y = kFlyoutHeaderHeight;
            for (const auto& e : entries) {
                RECT rowRc{12, y, rc.right - 12, y + kFlyoutRowHeight};
                if (e.isCurrent || e.isNext) {
                    HBRUSH hl = CreateSolidBrush(light ? RGB(230, 240, 255) : RGB(50, 60, 80));
                    RECT fillRc{0, y, rc.right, y + kFlyoutRowHeight};
                    FillRect(hdc, &fillRc, hl);
                    DeleteObject(hl);
                }
                SetTextColor(hdc, light ? RGB(26, 26, 26) : RGB(255, 255, 255));
                std::wstring name = GetPrayerName(e.name);
                std::wstring time = FormatTime(e.time);
                RECT nameRc{16, y + 4, rc.right / 2, y + kFlyoutRowHeight};
                RECT timeRc{rc.right / 2, y + 4, rc.right - 16, y + kFlyoutRowHeight};
                DrawTextW(hdc, name.c_str(), -1, &nameRc, DT_LEFT | DT_SINGLELINE);
                DrawTextW(hdc, time.c_str(), -1, &timeRc, DT_RIGHT | DT_SINGLELINE);
                y += kFlyoutRowHeight;
            }
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
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = FlyoutWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = kFlyoutClass;
        wc.hbrBackground = nullptr;
        RegisterClassW(&wc);
        registered = true;
    }
    g_flyoutHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kFlyoutClass, L"",
        WS_POPUP | WS_BORDER, 0, 0, kFlyoutWidth,
        kFlyoutHeaderHeight + kFlyoutRowHeight * 5 + 8, g_messageHwnd, nullptr,
        GetModuleHandleW(nullptr), nullptr);
}

bool TryGetSlotAnchor(RECT& anchor) {
    const int slotW = 60, slotH = 40, gap = 4;
    HWND shell = FindCurrentProcessTaskbarWnd();
    if (!shell) return false;
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
    int left = anchor.right - kFlyoutWidth;
    int top = anchor.top - height - 8;
    HMONITOR mon = MonitorFromRect(&anchor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(mon, &mi);
    if (left < mi.rcWork.left + 8) left = mi.rcWork.left + 8;
    if (top < mi.rcWork.top + 8) top = mi.rcWork.top + 8;
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
    UninstallFlyoutDismissHook();
}

void DestroyFlyout() {
    HideFlyout();
    if (g_flyoutHwnd) {
        DestroyWindow(g_flyoutHwnd);
        g_flyoutHwnd = nullptr;
    }
}


void UpdateTrayIcon() {
    if (!g_messageHwnd) return;
    if (g_settings.showNotificationIcon) {
        if (!g_trayIconAdded) {
            ZeroMemory(&g_trayIcon, sizeof(g_trayIcon));
            g_trayIcon.cbSize = sizeof(g_trayIcon);
            g_trayIcon.hWnd = g_messageHwnd;
            g_trayIcon.uID = 1;
            g_trayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            g_trayIcon.uCallbackMessage = WM_APP + 10;
            g_trayIcon.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
            wcscpy_s(g_trayIcon.szTip, L"PrayerTray");
            Shell_NotifyIconW(NIM_ADD, &g_trayIcon);
            g_trayIconAdded = true;
        }
    } else {
        RemoveTrayIcon();
    }
}

void RemoveTrayIcon() {
    if (g_trayIconAdded) {
        Shell_NotifyIconW(NIM_DELETE, &g_trayIcon);
        g_trayIconAdded = false;
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
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = Win10OverlayProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = kOverlayClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_HAND);
        RegisterClassW(&wc);
        registered = true;
    }
    HWND shell = FindCurrentProcessTaskbarWnd();
    if (!shell) return;
    if (!g_win10OverlayHwnd) {
        g_win10OverlayHwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW, kOverlayClass, L"", WS_CHILD | WS_VISIBLE, 0, 0, 60,
            40, shell, nullptr, GetModuleHandleW(nullptr), nullptr);
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
        // Do not require XamlRoot() here — it can be null for a moment after
        // inject / during tray rebuild, and demoting then removes a live slot.
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
    button.Background(Brush(alpha, 255, 255, 255));
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
            if (col > slotColumn) Controls::Grid::SetColumn(child, col - 1);
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
    Controls::TextBlock bottomText;
    bottomText.Name(L"PrayerTrayBottom");
    bottomText.FontFamily(Media::FontFamily(L"Segoe UI Variable Text"));
    bottomText.FontSize(11.5);
    bottomText.TextAlignment(TextAlignment::Center);
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
            if (col >= slotColumn) Controls::Grid::SetColumn(child, col + 1);
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
    g_injectRetryMs = 400;
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
        TextAlignment="Center"/><TextBlock Name="PrayerTrayBottom"
        FontFamily="Segoe UI Variable Text" FontSize="12" TextAlignment="Center"/>
        </StackPanel></Button>)";
    auto slotButton = Markup::XamlReader::Load(xaml).try_as<Controls::Button>();
    if (!slotButton) return false;
    stackPanel.Children().Append(slotButton);
    g_slotRoot = slotButton;
    g_topTextBlock = FindTextBlockByName(slotButton, L"PrayerTrayTop");
    g_bottomTextBlock = FindTextBlockByName(slotButton, L"PrayerTrayBottom");
    g_slotInjected = true;
    g_hasAppliedContent = false;
    g_injectRetryMs = 400;
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

using RunFromWindowThreadProc_t = void(WINAPI*)(void*);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* param) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunFromWindowThreadProc_t proc; void* param; };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        try {
            proc(param);
        } catch (...) {
        }
        return true;
    }
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                static const UINT m = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == m) {
                    auto* p = reinterpret_cast<Param*>(cwp->lParam);
                    try {
                        p->proc(p->param);
                    } catch (...) {
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, tid);
    if (!hook) return false;
    Param p{proc, param};
    DWORD_PTR result = 0;
    // Do not use SMTO_ABORTIFHUNG — tray layout often looks busy and aborted updates.
    if (!SendMessageTimeoutW(hWnd, msg, 0, reinterpret_cast<LPARAM>(&p), SMTO_NORMAL,
                             kRunFromWindowTimeoutMs, &result)) {
        SendMessageW(hWnd, msg, 0, reinterpret_cast<LPARAM>(&p));
    }
    UnhookWindowsHookEx(hook);
    return true;
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

struct SlotApplyParam { DisplayContent content; };

void ApplyWin11FromThread(SlotApplyParam* param) {
    try {
        DisplayContent content = param ? param->content : FormatTaskbarDisplay();
        try {
            if (IsSlotRootAlive()) {
                ApplyWin11SlotContent(content);
                g_injectRetryMs = 400;
                return;
            }
        } catch (...) {
            g_slotInjected = false;
        }
        bool injected = false;
        for (auto it = g_observedElements.begin(); it != g_observedElements.end();) {
            auto element = it->get();
            if (!element) {
                it = g_observedElements.erase(it);
                continue;
            }
            try {
                if (auto xr = element.XamlRoot()) {
                    if (ApplyWin11Style(xr, content)) {
                        injected = true;
                        break;
                    }
                }
            } catch (...) {
            }
            ++it;
        }
        try {
            if (IsSlotRootAlive()) {
                ApplyWin11SlotContent(content);
                g_injectRetryMs = 400;
                return;
            }
        } catch (...) {
            g_slotInjected = false;
        }
        if (!injected) {
            const int next = std::min(3000, std::max(400, g_injectRetryMs.load() * 2));
            g_injectRetryMs = next;
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
    SlotApplyParam param{content};
    RunFromWindowThread(taskbar, [](void* p) { ApplyWin11FromThread(static_cast<SlotApplyParam*>(p)); }, &param);
}

void ApplyEngineToBackends() {
    if (g_backend.load() == TaskbarBackend::Win11Xaml) {
        TryApplyWin11Slot();
    } else {
        ApplyWin10Content(FormatTaskbarDisplay());
    }
}

void* CTaskBand_ITaskListWndSite_vftable;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;
void* TaskbarHost_FrameHeight_Original;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void*);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

using IconView_IconView_t = void*(WINAPI*)(void*);
IconView_IconView_t IconView_IconView_Original;
using SystemTrayFrame_OnApplyTemplate_t = void(WINAPI*)(void*);
SystemTrayFrame_OnApplyTemplate_t SystemTrayFrame_OnApplyTemplate_Original;
using SystemTrayFrame_MeasureOverride_t =
    winrt::Windows::Foundation::Size(WINAPI*)(void*, winrt::Windows::Foundation::Size const&);
SystemTrayFrame_MeasureOverride_t SystemTrayFrame_MeasureOverride_Original;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

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

void TryApplySlotFromElement(FrameworkElement element) {
    if (g_unloading || !element || g_backend.load() != TaskbarBackend::Win11Xaml) return;
    try {
        TrackObservedElement(element);
        if (IsSlotRootAlive()) {
            ApplyWin11SlotContent(FormatTaskbarDisplay());
            return;
        }
        if (auto xr = element.XamlRoot()) ApplyWin11Style(xr, FormatTaskbarDisplay());
    } catch (...) {
    }
}

void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);
    if (!g_unloading) TryApplySlotFromElement(TryGetFrameworkElementFromImplementation(pThis));
    return ret;
}

void WINAPI SystemTrayFrame_OnApplyTemplate_Hook(void* pThis) {
    SystemTrayFrame_OnApplyTemplate_Original(pThis);
    if (!g_unloading) TryApplySlotFromElement(TryGetFrameworkElementFromImplementation(pThis));
}

winrt::Windows::Foundation::Size WINAPI SystemTrayFrame_MeasureOverride_Hook(
    void* pThis, winrt::Windows::Foundation::Size const& availableSize) {
    auto result = SystemTrayFrame_MeasureOverride_Original(pThis, availableSize);
    // Measure runs constantly during tray layout. Never touch an already-injected
    // slot here, and debounce inject attempts so we don't fight layout.
    if (g_unloading || IsSlotRootAlive()) {
        return result;
    }
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG last = g_lastMeasureInjectAttemptMs.load();
    if (now - last < 1000) {
        return result;
    }
    g_lastMeasureInjectAttemptMs = now;
    TryApplySlotFromElement(TryGetFrameworkElementFromImplementation(pThis));
    return result;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandleW(L"Taskbar.View.dll");
    if (!m) m = GetModuleHandleW(L"ExplorerExtensions.dll");
    return m;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
         &IconView_IconView_Original, IconView_IconView_Hook, true},
        {{LR"(protected: virtual void __cdecl winrt::SystemTray::implementation::SystemTrayFrame::OnApplyTemplate(void))",
          LR"(public: virtual void __cdecl winrt::SystemTray::implementation::SystemTrayFrame::OnApplyTemplate(void))"},
         &SystemTrayFrame_OnApplyTemplate_Original, SystemTrayFrame_OnApplyTemplate_Hook, true},
        {{LR"(protected: virtual struct winrt::Windows::Foundation::Size __cdecl winrt::SystemTray::implementation::SystemTrayFrame::MeasureOverride(struct winrt::Windows::Foundation::Size const &))",
          LR"(public: virtual struct winrt::Windows::Foundation::Size __cdecl winrt::SystemTray::implementation::SystemTrayFrame::MeasureOverride(struct winrt::Windows::Foundation::Size const &))"},
         &SystemTrayFrame_MeasureOverride_Original, SystemTrayFrame_MeasureOverride_Hook, true},
    };
    return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR name) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", name);
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module) HandleLoadedModuleIfTaskbarView(module, name);
    return module;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &std__Ref_count_base__Decref_Original},
    };
    return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

void SelectBackend() {
    bool win11 = DetectWin11();
    g_isWin11 = win11;
    if (win11 && !g_settings.oldTaskbarOnWin11) {
        g_backend = TaskbarBackend::Win11Xaml;
    } else {
        g_backend = TaskbarBackend::Win10Overlay;
    }
    Wh_Log(L"Backend: %s", g_backend.load() == TaskbarBackend::Win11Xaml ? L"Win11" : L"Win10");
}


void RestartUiTimer() {
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
            } else if (wParam == 4) {
                RequestRefresh(false);
            }
            return 0;
        case WM_PT_UI_TICK:
        case WM_PT_REFRESH:
            ApplyEngineToBackends();
            return 0;
        case WM_PT_HIDE_FLYOUT:
            HideFlyout();
            return 0;
        case WM_APP + 10:
            switch (LOWORD(lParam)) {
                case WM_LBUTTONUP:
                    DispatchCommand(kCmdShowFlyout);
                    break;
                case WM_RBUTTONUP:
                    DispatchCommand(kCmdShowMenu);
                    break;
            }
            return 0;
        case WM_COMMAND:
            DispatchCommand(static_cast<int>(LOWORD(wParam)));
            return 0;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void EnsureMessageWindow() {
    if (g_messageHwnd) return;
    static bool reg = false;
    if (!reg) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = MessageWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"PrayerTrayMessageWnd";
        RegisterClassW(&wc);
        reg = true;
    }
    g_messageHwnd = CreateWindowExW(0, L"PrayerTrayMessageWnd", L"PrayerTray", 0, 0, 0, 0, 0,
                                    HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
}

void StartTimers() {
    EnsureMessageWindow();
    RestartUiTimer();
    g_repositionTimer = SetTimer(g_messageHwnd, 2, 2000, nullptr);
    g_fullscreenTimer = SetTimer(nullptr, 3, 500, FullscreenTimerProc);
    g_scheduleTimer =
        SetTimer(g_messageHwnd, 4, kScheduleRefreshIntervalMs, nullptr);
}

void StopTimers() {
    if (g_messageHwnd) {
        KillTimer(g_messageHwnd, 1);
        KillTimer(g_messageHwnd, 2);
        KillTimer(g_messageHwnd, 4);
    }
    g_uiTimer = 0;
    g_repositionTimer = 0;
    g_scheduleTimer = 0;
    g_lastUiTimerIntervalMs = -1;
    if (g_fullscreenTimer) {
        KillTimer(nullptr, g_fullscreenTimer);
        g_fullscreenTimer = 0;
    }
}

void ClearWin11SlotOnUiThread(void*) {
    try {
        if (g_slotParentGrid) {
            RemovePrayerTraySlotFromGrid(g_slotParentGrid);
        } else if (g_slotRoot) {
            g_slotRoot.Visibility(Visibility::Collapsed);
        }
    } catch (...) {
    }
    g_loadedRevokers.clear();
    g_observedElements.clear();
    g_slotRoot = nullptr;
    g_slotButton = nullptr;
    g_topTextBlock = nullptr;
    g_bottomTextBlock = nullptr;
    g_slotParentGrid = nullptr;
    g_slotInjected = false;
    g_hasAppliedContent = false;
}

void ShutdownUi() {
    StopTimers();
    HideFlyout();
    DestroyFlyout();
    DestroyWin10Overlay();
    RemoveTrayIcon();
    if (g_contextMenu) {
        DestroyMenu(g_contextMenu);
        g_contextMenu = nullptr;
    }
    if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(taskbar, ClearWin11SlotOnUiThread, nullptr);
    } else {
        try {
            ClearWin11SlotOnUiThread(nullptr);
        } catch (...) {
        }
    }
    if (g_messageHwnd) {
        DestroyWindow(g_messageHwnd);
        g_messageHwnd = nullptr;
    }
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    SelectBackend();
    EnsureMessageWindow();
    if (g_backend.load() == TaskbarBackend::Win11Xaml) {
        HookTaskbarDllSymbols();
        if (HMODULE m = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            HookTaskbarViewDllSymbols(m);
        } else {
            HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
            auto pLoad = reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kb, "LoadLibraryExW"));
            WindhawkUtils::SetFunctionHook(pLoad, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
        }
    }
    StartTimers();
    UpdateTrayIcon();
    ApplyEngineToBackends();
    if (BootstrapFromCache()) {
        ApplyEngineToBackends();
    }
    RequestRefresh();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (g_backend.load() == TaskbarBackend::Win11Xaml && !g_taskbarViewDllLoaded) {
        if (HMODULE m = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(m)) Wh_ApplyHookOperations();
            }
        }
    }
    ApplyEngineToBackends();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    ShutdownUi();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    const bool wasSoundEnabled = g_settings.playPrayerNotification;
    auto oldBackend = g_backend.load();
    LoadSettings();
    SelectBackend();
    if (oldBackend != g_backend.load()) {
        if (g_backend.load() == TaskbarBackend::Win10Overlay) {
            if (auto root = g_slotRoot) {
                try {
                    root.Visibility(Visibility::Collapsed);
                } catch (...) {
                }
            }
        } else {
            DestroyWin10Overlay();
        }
    }
    if (!wasSoundEnabled && g_settings.playPrayerNotification) {
        PlayNotificationSound();
    }
    UpdateTrayIcon();
    g_refreshRequested = true;
    RequestRefresh(true);
    ApplyEngineToBackends();
}
