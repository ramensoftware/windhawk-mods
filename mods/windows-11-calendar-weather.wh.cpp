// ==WindhawkMod==
// @id              windows-11-calendar-weather
// @name            Windows 11 Calendar Weather
// @description     Replace the Notification Center notification list with a compact Open-Meteo weather forecast card above the calendar
// @version         1.0.1
// @author          FranciscoMurias
// @github          https://github.com/FranciscoMurias
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -ladvapi32 -luuid
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Windows 11 Calendar Weather

![Screenshot](https://github.com/user-attachments/assets/1c453d01-eb84-4f71-a292-c6e6971641ed)

When you open the Windows 11 date and time flyout, this mod replaces the
notification list area with a compact weather forecast card while leaving the
calendar and focus-session controls unchanged.

## Layout

1. Weather forecast card (former notification area)
2. Small vertical gap (~12 XAML units)
3. Original Windows calendar card
4. Original focus-session controls

## Features

- Current conditions, hourly forecast, and multi-day forecast
- Open-Meteo data (no API key)
- Configurable location, units, refresh interval, and card height
- Optional calendar daylight bar (sunrise–sunset) above the month grid
- Optional remaining daylight time (beside, below, or instead of the total)
- Optional calendar-app button (left of expand/collapse) with a custom launch path
- Works with `ShellExperienceHost.exe` and newer `ShellHost.exe` (24H2+)
- Compatible alongside **Windows 11 Notification Center Styler** (avoids copying
  that mod's theme engine; uses a uniquely named injected root)
- Fully restores the notification list when the mod is disabled or unloaded

## Setup

1. Compile and enable the mod in Windhawk.
2. Set **Location name** and/or **Latitude** / **Longitude** (defaults are empty).
3. Open the taskbar clock. The weather card appears above the calendar.

Coordinates are preferred in Manual mode. Auto-detect uses the Windows
location API (may prompt for permission) and falls back to Manual values if
location is unavailable.

## Notes

- Weather is cached in memory and refreshed on a timer (default every 60 minutes).
- Opening the clock flyout reuses the cache unless it is older than the refresh interval.
- Offline failures keep the last successful forecast when available.

## Privacy

Forecast requests are sent to [Open-Meteo](https://open-meteo.com/) with the
configured (or auto-detected) latitude and longitude. No API key is used. Location
is not sent to any other service by this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- weather:
  - locationMode: manual
    $name: Location mode
    $description: Auto uses Windows location (may prompt once). Manual uses the name/coordinates below.
    $options:
      - manual: Manual
      - auto: Auto-detect
  - locationName: ""
    $name: Location name
    $description: Used in Manual mode (and as fallback label). Also used to geocode when coordinates are empty.
  - latitude: ""
    $name: Latitude
    $description: Manual mode decimal latitude (string)
  - longitude: ""
    $name: Longitude
    $description: Manual mode decimal longitude (string)
  - temperatureUnit: celsius
    $name: Temperature unit
    $options:
      - celsius: Celsius
      - fahrenheit: Fahrenheit
  - refreshMinutes: 60
    $name: Refresh interval (minutes)
    $description: How often to fetch fresh weather from Open-Meteo (default 60 = once per hour). Also refreshes when the flyout opens if the cache is older than this.
  - hourlyCount: 6
    $name: Hourly columns
    $description: Between 3 and 8
  - dailyCount: 5
    $name: Daily rows
    $description: Between 3 and 7
  - cardHeight: 390
    $name: Weather card max height
    $description: Caps the weather card height. The card sizes to its content and sits just above the calendar.
  - showLocation: true
    $name: Show location name
  $name: Weather
- daylight:
  - showCalendarDaylight: false
    $name: Show calendar daylight hours
    $description: Insert a sunrise–sunset daylight bar between the clock/date and the month calendar
  - daylightHoursLeft: off
    $name: Daylight hours left
    $description: Show remaining daylight next to the total sunlight duration (requires calendar daylight hours)
    $options:
      - off: Off
      - rightOf: To the right of total
      - below: Below total
      - insteadOf: Instead of total
  $name: Daylight meter
- calendar:
  - showCalendarAppButton: false
    $name: Show calendar app button
    $description: Place a small calendar button to the left of the expand/collapse control
  - calendarAppPath: ""
    $name: Calendar app path
    $description: "Full path to an executable, or a URI (for example outlookcal: or ms-calendar:). Required when the button is shown."
  - roundedDayMarkers: true
    $name: Rounded day markers
    $description: Use rounded rectangles for today and selected days instead of circles
  - calendarDayHighlightColor: "#777777"
    $name: Calendar day highlight
    $description: "Hex color for today and selected day markers (for example #777777 or #FF777777). System opacity is preserved. Used when rounded day markers are enabled."
  $name: Calendar tweaks
*/
// ==/WindhawkModSettings==

#include <xamlom.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <combaseapi.h>
#include <ocidl.h>
#include <roapi.h>
#include <shellapi.h>
#include <sddl.h>
#include <windows.h>
#include <winstring.h>

#include <winrt/Windows.Devices.Geolocation.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace std::string_view_literals;

namespace wf = winrt::Windows::Foundation;
namespace wdj = winrt::Windows::Data::Json;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;
namespace wuxa = winrt::Windows::UI::Xaml::Automation;
namespace wuxap = winrt::Windows::UI::Xaml::Automation::Peers;
namespace wut = winrt::Windows::UI::Text;
namespace ws = winrt::Windows::System;
namespace wuc = winrt::Windows::UI::Core;
namespace wdg = winrt::Windows::Devices::Geolocation;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuco = winrt::Windows::UI::Composition;

// Helper matching Windhawk string-setting unique_ptr pattern used by styler mods.
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

////////////////////////////////////////////////////////////////////////////////
// XAML diagnostics / Windhawk TAP infrastructure
// Adapted from windows-11-notification-center-styler.wh.cpp (necessary parts).

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<PCWSTR>(&GetCurrentModuleHandle),
                           &module)) {
        return nullptr;
    }
    return module;
}

#pragma region visualtreewatcher

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

   private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation relation,
        VisualElement element,
        VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle element,
        VisualElementState elementState,
        LPCWSTR context) noexcept override;

    wf::IInspectable FromHandle(InstanceHandle handle) {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(
            handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

#pragma endregion

#pragma region tap

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// Unique CLSID for this mod (must not collide with Notification Center Styler).
// {AE04D7BB-0E2B-41F3-9E1C-ECECAA7B0A2B}
static constexpr CLSID CLSID_WindhawkTAP = {
    0xae04d7bb,
    0x0e2b,
    0x41f3,
    {0x9e, 0x1c, 0xec, 0xec, 0xaa, 0x7b, 0x0a, 0x2b}};

class WindhawkTAP
    : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid,
                                      void** ppvSite) noexcept override;

   private:
    winrt::com_ptr<IUnknown> site;
};

HRESULT WindhawkTAP::SetSite(IUnknown* pUnkSite) try {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site) {
        FreeLibrary(GetCurrentModuleHandle());
        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }

    return S_OK;
} catch (...) {
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"WindhawkTAP::SetSite error %08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void** ppvSite) noexcept {
    return site.as(riid, ppvSite);
}

template <typename T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter,
                                             REFIID riid,
                                             void** ppvObject) override try {
        if (!pUnkOuter) {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        return CLASS_E_NOAGGREGATION;
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"SimpleFactory::CreateInstance error %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) _Use_decl_annotations_ STDAPI
DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try {
    if (rclsid == CLSID_WindhawkTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
} catch (...) {
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"DllGetClassObject error %08X", hr);
    return hr;
}

__declspec(dllexport) _Use_decl_annotations_ STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX =
    decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location))) {
        case 0:
        case ARRAYSIZE(location):
            return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux = LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!wux) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = E_FAIL;
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);
        hr = ixde(connectionName, GetCurrentProcessId(), L"", location,
                  CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }
    return hr;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Forward declarations

void HandleVisualTreeAdd(InstanceHandle handle,
                         wux::FrameworkElement const& element,
                         PCWSTR typeName);
void HandleVisualTreeRemove(InstanceHandle handle);
void InitializeForCurrentThread();
void UninitializeForCurrentThread();
void InitializeSettingsAndTap();
void UninitializeSettingsAndTap();
void LoadSettings();
void ApplyOrRestoreAllMounted();
void RequestWeatherRefresh(bool forceNetwork);
void EnsureRefreshTimer();
void EnsureDaylightTickTimer();
void StopRefreshTimer();
void UpdateAllWeatherUIs();
void ApplyOrRestoreAllDaylight();
void UpdateAllDaylightUIs();
void RefreshDaylightOnFlyoutShown();
void RegisterVisibilityRefresh(wux::UIElement const& element, int64_t& cookie);
bool MountDaylightIntoCalendarSection(InstanceHandle handle,
                                      wuxc::Grid const& section);
bool MountCalendarLaunchButton(InstanceHandle handle, wuxc::Grid const& section);
void ScheduleCalendarLaunchMountRetry(InstanceHandle handle,
                                      wuxc::Grid const& section,
                                      int attempt);
void ApplyOrRestoreAllCalendarLaunch();
void UnmountCalendarLaunchInstance(struct MountedCalendarLaunchInstance& instance);
void ApplyCalendarDayShapeForSection(wuxc::Grid const& section,
                                     int attempt = 0);
void ApplyAllCalendarDayShapes();
void RestoreAllCalendarDayShapes();

////////////////////////////////////////////////////////////////////////////////
// Target process

enum class Target {
    ShellExperienceHost,
    ShellHost,
    Explorer,
};

Target g_target = Target::ShellExperienceHost;

// Explorer-side broker: ShellExperienceHost (AppContainer) cannot reliably
// start Win32 apps. The flyout asks explorer.exe (also injected by this mod)
// to ShellExecute the path via WM_COPYDATA and/or a shared mapping.
//
// V5: same proven launch path as V2 (sync ShellExecute on WM_COPYDATA, path
// from payload). Named-object ACL must allow AppContainer opens.
constexpr wchar_t kCalLaunchMappingName[] =
    L"Local\\WindhawkCalWeatherLaunchPathV5";
constexpr wchar_t kCalLaunchEventName[] =
    L"Local\\WindhawkCalWeatherLaunchEventV5";
constexpr wchar_t kCalLaunchWindowClass[] = L"WindhawkCalWeatherLaunchWnd";
constexpr wchar_t kCalLaunchWindowTitle[] = L"WindhawkCalWeatherLaunch";
constexpr DWORD kCalLaunchMappingBytes = 4096;
constexpr UINT kCalLaunchCopyDataId = 0xC7A1;  // arbitrary non-zero token

HANDLE g_calLaunchMapping = nullptr;
HANDLE g_calLaunchEvent = nullptr;
HANDLE g_calLaunchStopEvent = nullptr;
HANDLE g_calLaunchThread = nullptr;
HWND g_calLaunchHwnd = nullptr;

void StopCalendarLaunchBroker();
bool StartCalendarLaunchBroker();
bool RequestLaunchViaExplorerBroker(std::wstring const& path);
void ExplorerBrokerLaunchPath(std::wstring const& path);

std::atomic<bool> g_initialized{false};
thread_local bool g_initializedForThread = false;
std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_refreshingWeatherChrome{false};
std::atomic<uint64_t> g_uiGeneration{1};

////////////////////////////////////////////////////////////////////////////////
// Settings

struct ModSettings {
    bool autoLocation = false;
    std::wstring locationName;
    double latitude = 0.0;
    double longitude = 0.0;
    bool coordinatesValid = false;
    std::wstring temperatureUnit = L"celsius";
    int refreshMinutes = 60;
    int hourlyCount = 6;
    int dailyCount = 5;
    int cardHeight = 390;
    bool showLocation = true;
    bool showCalendarDaylight = false;
    // off | rightOf | below | insteadOf
    std::wstring daylightHoursLeft = L"off";
    bool showCalendarAppButton = false;
    std::wstring calendarAppPath;
    bool roundedDayMarkers = true;
    // RGB only — alpha is taken from the calendar's existing day chrome.
    uint8_t calendarDayHighlightR = 0x77;
    uint8_t calendarDayHighlightG = 0x77;
    uint8_t calendarDayHighlightB = 0x77;
};

ModSettings g_settings;
std::mutex g_settingsMutex;

double ParseCoordinate(PCWSTR text, bool& ok) {
    ok = false;
    if (!text || !*text) {
        return 0.0;
    }
    wchar_t* end = nullptr;
    double value = wcstod(text, &end);
    if (end == text || !std::isfinite(value)) {
        return 0.0;
    }
    ok = true;
    return value;
}

int ClampInt(int value, int minValue, int maxValue) {
    return (std::max)(minValue, (std::min)(value, maxValue));
}

bool ParseHexColorRgb(PCWSTR text, uint8_t& r, uint8_t& g, uint8_t& b) {
    if (!text) {
        return false;
    }
    while (*text == L' ' || *text == L'\t') {
        ++text;
    }
    if (*text == L'#') {
        ++text;
    }
    auto hexVal = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') {
            return c - L'0';
        }
        if (c >= L'a' && c <= L'f') {
            return c - L'a' + 10;
        }
        if (c >= L'A' && c <= L'F') {
            return c - L'A' + 10;
        }
        return -1;
    };

    wchar_t buf[9]{};
    size_t n = 0;
    while (text[n] && n < 8) {
        if (hexVal(text[n]) < 0) {
            break;
        }
        buf[n] = text[n];
        ++n;
    }
    if (n != 3 && n != 6 && n != 8) {
        return false;
    }

    auto pair = [&](wchar_t hi, wchar_t lo) -> uint8_t {
        return static_cast<uint8_t>((hexVal(hi) << 4) | hexVal(lo));
    };

    if (n == 3) {
        // #RGB
        r = pair(buf[0], buf[0]);
        g = pair(buf[1], buf[1]);
        b = pair(buf[2], buf[2]);
        return true;
    }
    if (n == 6) {
        // #RRGGBB
        r = pair(buf[0], buf[1]);
        g = pair(buf[2], buf[3]);
        b = pair(buf[4], buf[5]);
        return true;
    }
    // #AARRGGBB — ignore alpha; opacity comes from system chrome.
    r = pair(buf[2], buf[3]);
    g = pair(buf[4], buf[5]);
    b = pair(buf[6], buf[7]);
    return true;
}

void LoadSettings() {
    ModSettings s;

    string_setting_unique_ptr locationMode(
        Wh_GetStringSetting(L"weather.locationMode"));
    s.autoLocation =
        locationMode && _wcsicmp(locationMode.get(), L"auto") == 0;

    string_setting_unique_ptr locationName(
        Wh_GetStringSetting(L"weather.locationName"));
    if (locationName && locationName.get()[0]) {
        s.locationName = locationName.get();
    }

    string_setting_unique_ptr latitudeText(
        Wh_GetStringSetting(L"weather.latitude"));
    string_setting_unique_ptr longitudeText(
        Wh_GetStringSetting(L"weather.longitude"));
    bool latOk = false;
    bool lonOk = false;
    s.latitude = ParseCoordinate(latitudeText ? latitudeText.get() : L"", latOk);
    s.longitude =
        ParseCoordinate(longitudeText ? longitudeText.get() : L"", lonOk);
    s.coordinatesValid = latOk && lonOk && s.latitude >= -90.0 &&
                         s.latitude <= 90.0 && s.longitude >= -180.0 &&
                         s.longitude <= 180.0;

    string_setting_unique_ptr unit(
        Wh_GetStringSetting(L"weather.temperatureUnit"));
    if (unit && _wcsicmp(unit.get(), L"fahrenheit") == 0) {
        s.temperatureUnit = L"fahrenheit";
    } else {
        s.temperatureUnit = L"celsius";
    }

    s.refreshMinutes =
        ClampInt(Wh_GetIntSetting(L"weather.refreshMinutes"), 15, 360);
    s.hourlyCount = ClampInt(Wh_GetIntSetting(L"weather.hourlyCount"), 3, 8);
    s.dailyCount = ClampInt(Wh_GetIntSetting(L"weather.dailyCount"), 3, 7);
    s.cardHeight = ClampInt(Wh_GetIntSetting(L"weather.cardHeight"), 300, 520);
    s.showLocation = Wh_GetIntSetting(L"weather.showLocation") != 0;
    s.showCalendarDaylight =
        Wh_GetIntSetting(L"daylight.showCalendarDaylight") != 0;

    string_setting_unique_ptr daylightLeft(
        Wh_GetStringSetting(L"daylight.daylightHoursLeft"));
    if (daylightLeft &&
        (_wcsicmp(daylightLeft.get(), L"rightOf") == 0 ||
         _wcsicmp(daylightLeft.get(), L"below") == 0 ||
         _wcsicmp(daylightLeft.get(), L"insteadOf") == 0)) {
        s.daylightHoursLeft = daylightLeft.get();
    } else {
        s.daylightHoursLeft = L"off";
    }

    s.showCalendarAppButton =
        Wh_GetIntSetting(L"calendar.showCalendarAppButton") != 0;
    string_setting_unique_ptr calendarAppPath(
        Wh_GetStringSetting(L"calendar.calendarAppPath"));
    if (calendarAppPath && calendarAppPath.get()[0]) {
        s.calendarAppPath = calendarAppPath.get();
        // Trim surrounding quotes from pasted paths.
        if (s.calendarAppPath.size() >= 2 && s.calendarAppPath.front() == L'"' &&
            s.calendarAppPath.back() == L'"') {
            s.calendarAppPath =
                s.calendarAppPath.substr(1, s.calendarAppPath.size() - 2);
        }
    }

    s.roundedDayMarkers =
        Wh_GetIntSetting(L"calendar.roundedDayMarkers") != 0;

    {
        string_setting_unique_ptr highlightColor(
            Wh_GetStringSetting(L"calendar.calendarDayHighlightColor"));
        uint8_t r = 0x77, g = 0x77, b = 0x77;
        if (highlightColor &&
            ParseHexColorRgb(highlightColor.get(), r, g, b)) {
            s.calendarDayHighlightR = r;
            s.calendarDayHighlightG = g;
            s.calendarDayHighlightB = b;
        }
    }

    {
        std::lock_guard lock(g_settingsMutex);
        g_settings = std::move(s);
    }

    Wh_Log(
        L"Settings loaded: mode=%s location=%s coordsValid=%d unit=%s "
        L"refresh=%d hourly=%d daily=%d height=%d daylight=%d (raw=%d) left=%s "
        L"calBtn=%d (raw=%d) calPath=%s roundedDays=%d (raw=%d) "
        L"dayHighlight=#%02X%02X%02X",
        g_settings.autoLocation ? L"auto" : L"manual",
        g_settings.locationName.c_str(), g_settings.coordinatesValid ? 1 : 0,
        g_settings.temperatureUnit.c_str(), g_settings.refreshMinutes,
        g_settings.hourlyCount, g_settings.dailyCount, g_settings.cardHeight,
        g_settings.showCalendarDaylight ? 1 : 0,
        Wh_GetIntSetting(L"daylight.showCalendarDaylight"),
        g_settings.daylightHoursLeft.c_str(),
        g_settings.showCalendarAppButton ? 1 : 0,
        Wh_GetIntSetting(L"calendar.showCalendarAppButton"),
        g_settings.calendarAppPath.empty() ? L"(empty)"
                                           : g_settings.calendarAppPath.c_str(),
        g_settings.roundedDayMarkers ? 1 : 0,
        Wh_GetIntSetting(L"calendar.roundedDayMarkers"),
        g_settings.calendarDayHighlightR, g_settings.calendarDayHighlightG,
        g_settings.calendarDayHighlightB);
}

ModSettings GetSettingsCopy() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

////////////////////////////////////////////////////////////////////////////////
// Weather model

enum class WeatherIconKind {
    ClearDay,
    ClearNight,
    PartlyCloudy,
    Cloudy,
    Fog,
    Drizzle,
    Rain,
    HeavyRain,
    Snow,
    HeavySnow,
    Thunderstorm,
    Unknown,
};

struct HourlyEntry {
    std::wstring hourLabel;
    double temperature = 0;
    int weatherCode = -1;
    bool isDay = true;
    // Civil time at the forecast location (from Open-Meteo local ISO).
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = -1;
};

struct DailyEntry {
    std::wstring weekdayLabel;
    double minTemp = 0;
    double maxTemp = 0;
    int weatherCode = -1;
    bool hasSunriseSunset = false;
    int sunriseMinute = 0;  // minutes from local midnight
    int sunsetMinute = 0;
};

struct ForecastData {
    bool valid = false;
    std::wstring locationDisplay;
    double currentTemp = 0;
    int currentCode = -1;
    bool currentIsDay = true;
    double todayMin = 0;
    double todayMax = 0;
    bool hasDaylight = false;
    int todaySunriseMinute = 0;
    int todaySunsetMinute = 0;
    int utcOffsetSeconds = 0;
    // Full Open-Meteo hourly series — display strip is re-sliced from this
    // whenever the flyout paints so the first column tracks the current hour
    // even if a network refresh has not completed yet.
    std::vector<HourlyEntry> hourlyAll;
    std::vector<HourlyEntry> hourly;
    std::vector<DailyEntry> daily;
    std::wstring temperatureUnit;
    // Wall clock — steady_clock does not advance across sleep, so wake would
    // treat multi-hour-old data as still fresh.
    std::chrono::system_clock::time_point fetchedAtWall{};
    // Local civil time at fetch — hour/day change means hourly strip is stale
    // even if still inside the refresh interval.
    int fetchedLocalYear = 0;
    int fetchedLocalMonth = 0;
    int fetchedLocalDay = 0;
    int fetchedLocalHour = -1;
};

struct ResolvedLocation {
    std::wstring displayName;
    double latitude = 0;
    double longitude = 0;
    bool valid = false;
};

std::mutex g_forecastMutex;
ForecastData g_forecast;
ResolvedLocation g_resolvedLocation;
std::wstring g_resolvedKey;
std::atomic<bool> g_fetchInProgress{false};
std::atomic<bool> g_forceRefreshPending{false};
std::atomic<uint64_t> g_fetchGeneration{0};
std::atomic<ULONGLONG> g_fetchStartedTick{0};
// Wall-clock start — GetTickCount64 freezes across sleep/standby, so a hung
// fetch can look "fresh" forever until we also check system_clock.
std::atomic<int64_t> g_fetchStartedWallMs{0};
std::atomic<ULONGLONG> g_lastResumeRefreshTick{0};
std::atomic<ULONGLONG> g_lastDaylightFlyoutRefreshTick{0};

PTP_TIMER g_refreshTimer = nullptr;
PTP_TIMER g_daylightTickTimer = nullptr;
DWORD g_refreshTimerPeriodMs = 0;
std::mutex g_timerMutex;
HANDLE g_suspendResumeNotify = nullptr;
HANDLE g_displayPowerNotify = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Mounted UI state

struct SavedProperty {
    wux::DependencyProperty property{nullptr};
    wf::IInspectable value{nullptr};
};

struct ChildVisibilityRecord {
    winrt::weak_ref<wux::UIElement> element;
    wux::Visibility original = wux::Visibility::Visible;
    double originalOpacity = 1.0;
    int64_t visibilityCookie = -1;
};

struct WeatherUiControls {
    // Strong refs: these are our controls (not shell-owned). Weak refs were
    // expiring for TextBlocks in ShellExperienceHost while panels stayed alive.
    wuxc::TextBlock locationText{nullptr};
    wuxc::TextBlock temperatureText{nullptr};
    wuxc::Grid conditionIconHost{nullptr};
    wuxc::TextBlock conditionText{nullptr};
    wuxc::TextBlock highLowText{nullptr};
    wuxc::Grid hourlyPanel{nullptr};
    wuxc::StackPanel dailyPanel{nullptr};
    wuxc::TextBlock statusText{nullptr};
    wuxc::StackPanel contentRoot{nullptr};
};

struct MountedWeatherInstance {
    InstanceHandle gridHandle = 0;
    DWORD uiThreadId = 0;
    winrt::weak_ref<wuxc::Grid> notificationGrid;
    winrt::weak_ref<wuxc::Border> weatherRoot;
    ws::DispatcherQueue dispatcherQueue{nullptr};
    wuc::CoreDispatcher coreDispatcher{nullptr};
    std::vector<ChildVisibilityRecord> childVisibility;
    std::vector<SavedProperty> savedProperties;
    WeatherUiControls ui;
    uint64_t generation = 0;
    wux::FrameworkElement::SizeChanged_revoker weatherSizeChangedRevoker{};
    wux::FrameworkElement::SizeChanged_revoker gridSizeChangedRevoker{};
    wux::FrameworkElement::ActualThemeChanged_revoker gridThemeChangedRevoker{};
    wux::FrameworkElement::ActualThemeChanged_revoker weatherThemeChangedRevoker{};
    wux::FrameworkElement::EffectiveViewportChanged_revoker
        weatherViewportRevoker{};
    int64_t backgroundChangedCookie = -1;
    int64_t shadowChangedCookie = -1;
    int64_t weatherVisibilityCookie = -1;
    int64_t gridVisibilityCookie = -1;

    MountedWeatherInstance() = default;
    MountedWeatherInstance(const MountedWeatherInstance&) = delete;
    MountedWeatherInstance& operator=(const MountedWeatherInstance&) = delete;
    MountedWeatherInstance(MountedWeatherInstance&&) = default;
    MountedWeatherInstance& operator=(MountedWeatherInstance&&) = default;
};

struct DaylightUiControls {
    wuxc::Grid nowMarkerHost{nullptr};
    wuxc::Grid trackHost{nullptr};
    wuxc::TextBlock sunriseText{nullptr};
    wuxc::StackPanel durationStack{nullptr};
    wuxc::TextBlock durationText{nullptr};
    wuxc::TextBlock remainingText{nullptr};
    wuxc::TextBlock sunsetText{nullptr};
};

struct MountedDaylightInstance {
    InstanceHandle sectionHandle = 0;
    DWORD uiThreadId = 0;
    winrt::weak_ref<wuxc::Grid> calendarSection;
    winrt::weak_ref<wux::FrameworkElement> daylightRoot;
    winrt::weak_ref<wux::FrameworkElement> clocksHost;
    ws::DispatcherQueue dispatcherQueue{nullptr};
    wuc::CoreDispatcher coreDispatcher{nullptr};
    DaylightUiControls ui;
    // UI-thread tick while the flyout stays open (hours-left / now marker).
    ws::DispatcherQueueTimer daylightTickTimer{nullptr};
    int64_t visibilityCookie = -1;
    uint64_t generation = 0;
    int insertedGridRow = -1;
    bool insertedRowDefinition = false;
    std::vector<ChildVisibilityRecord> hiddenClocks;
    bool clocksLayoutSaved = false;
    double clocksHeight = std::numeric_limits<double>::quiet_NaN();
    double clocksMinHeight = 0.0;
    double clocksMaxHeight = std::numeric_limits<double>::infinity();
    wux::Thickness clocksMargin{};

    MountedDaylightInstance() = default;
    MountedDaylightInstance(const MountedDaylightInstance&) = delete;
    MountedDaylightInstance& operator=(const MountedDaylightInstance&) = delete;
    MountedDaylightInstance(MountedDaylightInstance&&) = default;
    MountedDaylightInstance& operator=(MountedDaylightInstance&&) = default;
};

[[clang::no_destroy]] std::vector<MountedDaylightInstance> g_daylightMounted;

struct MountedCalendarLaunchInstance {
    InstanceHandle sectionHandle = 0;
    DWORD uiThreadId = 0;
    winrt::weak_ref<wuxc::Grid> calendarSection;
    winrt::weak_ref<wuxc::Panel> parent;
    winrt::weak_ref<wuxc::StackPanel> host;
    winrt::weak_ref<wuxc::Button> launchButton;
    winrt::weak_ref<wuxc::Button> expandButton;
    winrt::weak_ref<wuxc::Border> hoverFill;
    winrt::weak_ref<wuxc::Border> templateFill;
    winrt::weak_ref<wuxc::Border> templateStroke;
    winrt::weak_ref<wuxc::Border> templatePressed;
    ws::DispatcherQueue dispatcherQueue{nullptr};
    wuc::CoreDispatcher coreDispatcher{nullptr};
    wuxc::Button::Click_revoker clickRevoker;
    wux::UIElement::PointerEntered_revoker pointerEnteredRevoker;
    wux::UIElement::PointerPressed_revoker pointerPressedRevoker;
    wux::UIElement::PointerReleased_revoker pointerReleasedRevoker;
    wux::UIElement::PointerExited_revoker pointerExitedRevoker;
    wux::UIElement::Tapped_revoker tappedRevoker;
    int gridRow = 0;
    int gridColumn = 0;
    int gridRowSpan = 1;
    int gridColumnSpan = 1;
    wux::HorizontalAlignment expandHAlign = wux::HorizontalAlignment::Right;
    wux::VerticalAlignment expandVAlign = wux::VerticalAlignment::Top;
    wux::Thickness expandMargin{};

    MountedCalendarLaunchInstance() = default;
    MountedCalendarLaunchInstance(const MountedCalendarLaunchInstance&) =
        delete;
    MountedCalendarLaunchInstance& operator=(
        const MountedCalendarLaunchInstance&) = delete;
    MountedCalendarLaunchInstance(MountedCalendarLaunchInstance&&) = default;
    MountedCalendarLaunchInstance& operator=(MountedCalendarLaunchInstance&&) =
        default;
};

[[clang::no_destroy]] std::vector<MountedCalendarLaunchInstance>
    g_calendarLaunchMounted;
std::recursive_mutex g_mountMutex;
[[clang::no_destroy]] std::vector<MountedWeatherInstance> g_mounted;

// Match CalendarCenterGrid / Win11 flyout card radius.
constexpr double kWeatherCardCornerRadius = 8.0;

void SaveLocalProperty(wux::DependencyObject const& element,
                       wux::DependencyProperty const& property,
                       std::vector<SavedProperty>& out);

void ClearCompositionClip(wux::UIElement const& element) {
    try {
        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(element);
        visual.Clip(nullptr);
    } catch (...) {
    }
}

// Clip the weather Border to its corner radius so child content / parent fill
// cannot paint a square "box" through the rounded corners.
void ApplyCompositionRoundedClip(wux::UIElement const& element, float radius) {
    try {
        auto fe = element.try_as<wux::FrameworkElement>();
        if (!fe) {
            return;
        }
        float width = static_cast<float>(fe.ActualWidth());
        float height = static_cast<float>(fe.ActualHeight());
        if (width < 8.f || height < 8.f) {
            return;
        }

        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(element);
        auto compositor = visual.Compositor();

        wuco::CompositionRoundedRectangleGeometry geometry{nullptr};
        if (auto existingClip = visual.Clip()) {
            if (auto geoClip =
                    existingClip.try_as<wuco::CompositionGeometricClip>()) {
                geometry = geoClip.Geometry()
                               .try_as<wuco::CompositionRoundedRectangleGeometry>();
            }
        }
        if (!geometry) {
            geometry = compositor.CreateRoundedRectangleGeometry();
            auto clip = compositor.CreateGeometricClip();
            clip.Geometry(geometry);
            visual.Clip(clip);
        }

        // Expand 1px so anti-aliased top/side stroke isn't shaved by the clip
        // (reads as a persistent 1px cutoff of the card chrome).
        geometry.Offset(
            winrt::Windows::Foundation::Numerics::float2{-1.f, -1.f});
        geometry.CornerRadius(
            winrt::Windows::Foundation::Numerics::float2{radius, radius});
        geometry.Size(
            winrt::Windows::Foundation::Numerics::float2{width + 2.f,
                                                         height + 2.f});
    } catch (...) {
        Wh_Log(L"ApplyCompositionRoundedClip failed %08X", winrt::to_hresult());
    }
}

// Resolve a Grid dependency property via a throwaway Style Setter (same approach
// as Notification Center Styler; needed when the DP isn't in Windhawk headers).
wux::DependencyProperty ResolveGridProperty(PCWSTR propertyName) {
    static std::mutex cacheMutex;
    static std::unordered_map<std::wstring, wux::DependencyProperty> cache;
    {
        std::lock_guard lock(cacheMutex);
        auto it = cache.find(propertyName);
        if (it != cache.end()) {
            return it->second;
        }
    }

    wux::DependencyProperty resolved{nullptr};
    try {
        std::wstring xaml =
            L"<Style xmlns='http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/"
            L"xaml' TargetType='Grid'><Setter Property='";
        xaml += propertyName;
        if (wcscmp(propertyName, L"BorderBrush") == 0 ||
            wcscmp(propertyName, L"Background") == 0) {
            xaml += L"' Value='{x:Null}'/></Style>";
        } else {
            xaml += L"' Value='0'/></Style>";
        }
        auto style = wux::Markup::XamlReader::Load(xaml).as<wux::Style>();
        resolved = style.Setters().GetAt(0).as<wux::Setter>().Property();
    } catch (...) {
        Wh_Log(L"ResolveGridProperty(%s) failed %08X", propertyName,
               winrt::to_hresult());
    }
    if (resolved) {
        std::lock_guard lock(cacheMutex);
        cache.emplace(propertyName, resolved);
    }
    return resolved;
}

wuxc::Grid FindNamedGridNearby(wux::FrameworkElement const& from,
                               PCWSTR name) {
    try {
        wux::DependencyObject current = from;
        for (int depth = 0; depth < 8 && current; ++depth) {
            if (auto panel = current.try_as<wuxc::Panel>()) {
                for (auto const& child : panel.Children()) {
                    if (auto grid = child.try_as<wuxc::Grid>()) {
                        if (grid.Name() == name) {
                            return grid;
                        }
                    }
                }
            }
            current = wuxm::VisualTreeHelper::GetParent(current);
        }
    } catch (...) {
    }
    return nullptr;
}

wuxm::Brush FindFirstBackgroundBrush(wux::DependencyObject const& root,
                                     int depth) {
    if (!root || depth > 5) {
        return nullptr;
    }
    try {
        if (auto panel = root.try_as<wuxc::Panel>()) {
            if (auto bg = panel.Background()) {
                return bg;
            }
        }
        if (auto border = root.try_as<wuxc::Border>()) {
            if (auto bg = border.Background()) {
                return bg;
            }
        }
    } catch (...) {
    }
    try {
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; ++i) {
            if (auto brush = FindFirstBackgroundBrush(
                    wuxm::VisualTreeHelper::GetChild(root, i), depth + 1)) {
                return brush;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

// Content-driven height estimate. Must stay ABOVE real content height —
// NotificationCenterGrid is Bottom-aligned, so a tight MaxHeight clips the
// top edge of the card (the persistent 1–2px cutoff).
double EstimateWeatherContentHeight(ModSettings const& settings) {
    const double padding = 32.0;
    const double header = 100.0;
    const double dividers = 36.0;
    const double hourly = 80.0;
    const double dailyRow = 38.0;
    // Extra headroom so layout rounding / font metrics never clip chrome.
    const double headroom = 48.0;
    return padding + header + dividers + hourly +
           static_cast<double>(settings.dailyCount) * dailyRow + headroom;
}

// Walk up the visual tree and clear clips that can shave our card's top edge.
void ClearAncestorClips(wux::DependencyObject const& start, int maxDepth) {
    try {
        wux::DependencyObject current = start;
        for (int depth = 0; depth < maxDepth && current; ++depth) {
            if (auto element = current.try_as<wux::UIElement>()) {
                try {
                    element.Clip(nullptr);
                } catch (...) {
                }
                ClearCompositionClip(element);
            }
            current = wuxm::VisualTreeHelper::GetParent(current);
        }
    } catch (...) {
    }
}

std::atomic<int> g_insideLayoutEnforce{0};

// After the weather Border measures, raise MaxHeight if needed so Bottom
// alignment cannot clip the top chrome.
void EnsureGridFitsWeatherContent(wuxc::Grid const& grid,
                                  wuxc::Border const& weatherRoot) {
    try {
        if (!grid || !weatherRoot) {
            return;
        }
        const double content = weatherRoot.ActualHeight();
        if (content < 8.0) {
            return;
        }
        // Include grid margins and the card's own top inset (anti-clip margin).
        auto gridMargin = grid.Margin();
        auto rootMargin = weatherRoot.Margin();
        const double needed = content + gridMargin.Top + gridMargin.Bottom +
                              rootMargin.Top + rootMargin.Bottom + 2.0;
        const double currentMax = grid.MaxHeight();
        if (!std::isfinite(currentMax) || currentMax < needed) {
            grid.MaxHeight(needed);
        }
        grid.Clip(nullptr);
        ClearCompositionClip(grid);
        ClearAncestorClips(grid, 6);
    } catch (...) {
        Wh_Log(L"EnsureGridFitsWeatherContent failed %08X",
               winrt::to_hresult());
    }
}

// Shell notification updates often restore NotificationCenterGrid to Stretch
// (and/or a large Height/MinHeight) so the card jumps to the top of the star
// slot — huge gap above the calendar, top of the weather card clipped.
void EnforceWeatherNotificationLayout(wuxc::Grid const& grid,
                                      wuxc::Border const& weatherRoot) {
    if (!grid || g_insideLayoutEnforce.load() > 0) {
        return;
    }
    g_insideLayoutEnforce.fetch_add(1);
    try {
        bool drifted = false;
        try {
            if (grid.VerticalAlignment() !=
                wux::VerticalAlignment::Bottom) {
                grid.VerticalAlignment(wux::VerticalAlignment::Bottom);
                drifted = true;
            }
            auto heightLocal =
                grid.ReadLocalValue(wux::FrameworkElement::HeightProperty());
            if (heightLocal != wux::DependencyProperty::UnsetValue()) {
                grid.ClearValue(wux::FrameworkElement::HeightProperty());
                drifted = true;
            }
            auto minLocal = grid.ReadLocalValue(
                wux::FrameworkElement::MinHeightProperty());
            if (minLocal != wux::DependencyProperty::UnsetValue()) {
                grid.ClearValue(wux::FrameworkElement::MinHeightProperty());
                drifted = true;
            }
            auto margin = grid.Margin();
            if (margin.Top != 0.0 || margin.Bottom != 4.0) {
                margin.Top = 0;
                margin.Bottom = 4;
                grid.Margin(margin);
                drifted = true;
            }
            if (grid.Visibility() != wux::Visibility::Visible) {
                grid.Visibility(wux::Visibility::Visible);
                drifted = true;
            }
        } catch (...) {
            grid.ClearValue(wux::FrameworkElement::HeightProperty());
            grid.ClearValue(wux::FrameworkElement::MinHeightProperty());
            grid.VerticalAlignment(wux::VerticalAlignment::Bottom);
            drifted = true;
        }

        if (weatherRoot) {
            try {
                if (weatherRoot.VerticalAlignment() !=
                    wux::VerticalAlignment::Top) {
                    weatherRoot.VerticalAlignment(
                        wux::VerticalAlignment::Top);
                    drifted = true;
                }
            } catch (...) {
            }
            EnsureGridFitsWeatherContent(grid, weatherRoot);
        }

        if (drifted) {
            Wh_Log(L"Re-asserted weather grid layout after shell drift");
        }
    } catch (...) {
        Wh_Log(L"EnforceWeatherNotificationLayout failed %08X",
               winrt::to_hresult());
    }
    g_insideLayoutEnforce.fetch_sub(1);
}

void SavePropertyOnce(wux::DependencyObject const& element,
                      wux::DependencyProperty const& property,
                      std::vector<SavedProperty>& saved) {
    if (!property) {
        return;
    }
    for (auto const& item : saved) {
        if (item.property == property) {
            return;
        }
    }
    SaveLocalProperty(element, property, saved);
}

// Strip NotificationCenterGrid chrome entirely, then paint one calendar-matched
// Border (WindhawkWeatherRoot). The previous "square in a square" look was the
// Grid's own Border/Background still drawing behind an inset weather Border.
void StripNotificationGridChrome(wuxc::Grid const& notificationGrid,
                                 MountedWeatherInstance& instance);
void RefreshWeatherChromeForHandle(InstanceHandle handle);
void ApplyCalendarMatchedChrome(MountedWeatherInstance& instance,
                                wuxc::Grid const& notificationGrid,
                                wuxc::Border const& weatherRoot);
void CollapseNativeChildren(wuxc::Grid const& grid,
                            MountedWeatherInstance& instance);

// ThemeShadow belongs on the painted weather Border — not the transparent
// NotificationCenterGrid (that was the ghost under-card). Needs Z > 0.
void ApplyWeatherCardShadow(wuxc::Border const& weatherRoot,
                            wuxc::Grid const& notificationGrid) {
    try {
        float z = 32.f;
        try {
            if (auto calendar = FindNamedGridNearby(notificationGrid,
                                                    L"CalendarCenterGrid")) {
                auto ct = calendar.Translation();
                if (ct.z > 1.f) {
                    z = ct.z;
                }
            }
        } catch (...) {
        }

        weatherRoot.Shadow(wuxm::ThemeShadow());
        auto t = weatherRoot.Translation();
        weatherRoot.Translation(
            winrt::Windows::Foundation::Numerics::float3{t.x, t.y, z});
    } catch (...) {
        Wh_Log(L"ApplyWeatherCardShadow failed %08X", winrt::to_hresult());
    }
}

void StripNotificationGridChrome(wuxc::Grid const& notificationGrid,
                                 MountedWeatherInstance& instance) {
    static thread_local int insideStrip = 0;
    struct StripGuard {
        StripGuard() { ++insideStrip; }
        ~StripGuard() { --insideStrip; }
    };

    try {
        StripGuard guard;

        SavePropertyOnce(notificationGrid, wuxc::Panel::BackgroundProperty(),
                         instance.savedProperties);
        // Theme toggles re-apply an opaque/acrylic brush on this grid. Keep
        // forcing a local transparent value so only WindhawkWeatherRoot paints.
        notificationGrid.ClearValue(wuxc::Panel::BackgroundProperty());
        notificationGrid.Background(wuxm::SolidColorBrush(
            winrt::Windows::UI::Colors::Transparent()));

        if (auto prop = ResolveGridProperty(L"BorderBrush")) {
            SavePropertyOnce(notificationGrid, prop, instance.savedProperties);
            notificationGrid.ClearValue(prop);
        }
        if (auto prop = ResolveGridProperty(L"BorderThickness")) {
            SavePropertyOnce(notificationGrid, prop, instance.savedProperties);
            notificationGrid.SetValue(
                prop, winrt::box_value(wux::Thickness{0, 0, 0, 0}));
        }
        if (auto prop = ResolveGridProperty(L"CornerRadius")) {
            SavePropertyOnce(notificationGrid, prop, instance.savedProperties);
            notificationGrid.SetValue(
                prop, winrt::box_value(wux::CornerRadius{0, 0, 0, 0}));
        }
        if (auto prop = ResolveGridProperty(L"Padding")) {
            SavePropertyOnce(notificationGrid, prop, instance.savedProperties);
            notificationGrid.SetValue(
                prop, winrt::box_value(wux::Thickness{0, 0, 0, 0}));
        }

        SavePropertyOnce(notificationGrid, wux::UIElement::ClipProperty(),
                         instance.savedProperties);
        notificationGrid.Clip(nullptr);
        ClearCompositionClip(notificationGrid);

        // ThemeShadow on the notification grid is the visible "ghost card"
        // behind the weather Border after theme changes.
        try {
            SavePropertyOnce(notificationGrid, wux::UIElement::ShadowProperty(),
                             instance.savedProperties);
            notificationGrid.ClearValue(wux::UIElement::ShadowProperty());
            notificationGrid.Shadow(nullptr);
        } catch (...) {
        }

        // Catch shell restores of Background without a full theme change.
        if (instance.backgroundChangedCookie < 0) {
            instance.backgroundChangedCookie =
                notificationGrid.RegisterPropertyChangedCallback(
                    wuxc::Panel::BackgroundProperty(),
                    wux::DependencyPropertyChangedCallback(
                        [](wux::DependencyObject const& sender,
                           wux::DependencyProperty const&) {
                            if (insideStrip > 0 ||
                                g_refreshingWeatherChrome.load()) {
                                return;
                            }
                            auto grid = sender.try_as<wuxc::Grid>();
                            if (!grid) {
                                return;
                            }
                            try {
                                auto bg = grid.Background();
                                if (auto solid =
                                        bg
                                            ? bg.try_as<wuxm::SolidColorBrush>()
                                            : nullptr) {
                                    if (solid.Color().A == 0) {
                                        return;
                                    }
                                }
                                // Non-transparent background restored by shell.
                                grid.Background(wuxm::SolidColorBrush(
                                    winrt::Windows::UI::Colors::Transparent()));
                                try {
                                    grid.Shadow(nullptr);
                                } catch (...) {
                                }
                            } catch (...) {
                            }
                        }));
        }
        if (instance.shadowChangedCookie < 0) {
            instance.shadowChangedCookie =
                notificationGrid.RegisterPropertyChangedCallback(
                    wux::UIElement::ShadowProperty(),
                    wux::DependencyPropertyChangedCallback(
                        [](wux::DependencyObject const& sender,
                           wux::DependencyProperty const&) {
                            if (insideStrip > 0 ||
                                g_refreshingWeatherChrome.load()) {
                                return;
                            }
                            auto grid = sender.try_as<wuxc::Grid>();
                            if (!grid) {
                                return;
                            }
                            try {
                                if (grid.Shadow() == nullptr) {
                                    return;
                                }
                                grid.Shadow(nullptr);
                            } catch (...) {
                            }
                        }));
        }
    } catch (...) {
        Wh_Log(L"StripNotificationGridChrome failed %08X",
               winrt::to_hresult());
    }
}

void ApplyCalendarMatchedChrome(MountedWeatherInstance& instance,
                                wuxc::Grid const& notificationGrid,
                                wuxc::Border const& weatherRoot) {
    const wux::CornerRadius radius{
        kWeatherCardCornerRadius, kWeatherCardCornerRadius,
        kWeatherCardCornerRadius, kWeatherCardCornerRadius};

    StripNotificationGridChrome(notificationGrid, instance);

    wuxm::Brush background{nullptr};
    wuxm::Brush borderBrush{nullptr};
    wux::Thickness borderThickness{1, 1, 1, 1};
    wux::CornerRadius applied = radius;

    try {
        if (auto calendar =
                FindNamedGridNearby(notificationGrid, L"CalendarCenterGrid")) {
            background = calendar.Background();
            if (!background) {
                background = FindFirstBackgroundBrush(calendar, 0);
            }

            if (auto prop = ResolveGridProperty(L"BorderBrush")) {
                borderBrush = calendar.GetValue(prop).try_as<wuxm::Brush>();
            }
            if (auto prop = ResolveGridProperty(L"BorderThickness")) {
                try {
                    borderThickness = winrt::unbox_value<wux::Thickness>(
                        calendar.GetValue(prop));
                    if (borderThickness.Left <= 0 &&
                        borderThickness.Top <= 0 &&
                        borderThickness.Right <= 0 &&
                        borderThickness.Bottom <= 0) {
                        borderThickness = wux::Thickness{1, 1, 1, 1};
                    }
                } catch (...) {
                }
            }
            if (auto prop = ResolveGridProperty(L"CornerRadius")) {
                try {
                    auto cr = winrt::unbox_value<wux::CornerRadius>(
                        calendar.GetValue(prop));
                    double r = (std::max)(
                        (std::max)(cr.TopLeft, cr.TopRight),
                        (std::max)(cr.BottomLeft, cr.BottomRight));
                    if (r < 1.0) {
                        r = kWeatherCardCornerRadius;
                    }
                    applied = wux::CornerRadius{r, r, r, r};
                } catch (...) {
                }
            }
            Wh_Log(L"Matched calendar chrome brush=%d borderBrush=%d radius=%g",
                   background ? 1 : 0, borderBrush ? 1 : 0, applied.TopLeft);
        }
    } catch (...) {
        Wh_Log(L"Calendar chrome probe failed %08X", winrt::to_hresult());
    }

    if (!background) {
        try {
            auto resources = wux::Application::Current().Resources();
            if (auto value = resources.TryLookup(winrt::box_value(
                    L"AcrylicBackgroundFillColorDefaultBrush"))) {
                background = value.try_as<wuxm::Brush>();
            }
            if (!background) {
                if (auto value = resources.TryLookup(winrt::box_value(
                        L"LayerFillColorDefaultBrush"))) {
                    background = value.try_as<wuxm::Brush>();
                }
            }
        } catch (...) {
        }
    }
    if (!background) {
        background = wuxm::SolidColorBrush(
            winrt::Windows::UI::Color{220, 32, 32, 32});
    }
    if (!borderBrush) {
        borderBrush = wuxm::SolidColorBrush(
            winrt::Windows::UI::Color{48, 255, 255, 255});
    }

    try {
        weatherRoot.Background(background);
        weatherRoot.BorderBrush(borderBrush);
        weatherRoot.BorderThickness(borderThickness);
        weatherRoot.CornerRadius(applied);
        weatherRoot.Padding(wux::Thickness{12, 10, 12, 12});
        // Critical: parent slot often clips the top 1–3px of NotificationCenterGrid.
        // With a transparent grid, a small top margin on the card keeps the top
        // stroke fully below that clip line (padding alone cannot fix chrome clip).
        weatherRoot.Margin(wux::Thickness{0, 3, 0, 0});
        weatherRoot.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        weatherRoot.VerticalAlignment(wux::VerticalAlignment::Top);

        ClearCompositionClip(weatherRoot);
        ClearAncestorClips(notificationGrid, 8);
        ApplyWeatherCardShadow(weatherRoot, notificationGrid);

        auto onLayoutDrift =
            [gridWeak = winrt::make_weak(notificationGrid),
             rootWeak = winrt::make_weak(weatherRoot),
             handle = instance.gridHandle](wf::IInspectable const&,
                                           wux::SizeChangedEventArgs const&) {
                if (auto grid = gridWeak.get()) {
                    auto root = rootWeak.get();
                    // Notification arrivals resize this slot and often restore
                    // Stretch/Height — pin layout back before the gap appears.
                    EnforceWeatherNotificationLayout(grid, root);
                    try {
                        grid.Background(wuxm::SolidColorBrush(
                            winrt::Windows::UI::Colors::Transparent()));
                        grid.Shadow(nullptr);
                    } catch (...) {
                    }
                    try {
                        std::lock_guard lock(g_mountMutex);
                        for (auto& mounted : g_mounted) {
                            if (mounted.gridHandle == handle) {
                                CollapseNativeChildren(grid, mounted);
                                break;
                            }
                        }
                    } catch (...) {
                    }
                    // Opening the flyout often only resizes existing elements
                    // (no remount) — refresh weather hourly + daylight here.
                    RefreshDaylightOnFlyoutShown();
                }
            };

        instance.weatherSizeChangedRevoker = weatherRoot.SizeChanged(
            winrt::auto_revoke, onLayoutDrift);
        instance.gridSizeChangedRevoker = notificationGrid.SizeChanged(
            winrt::auto_revoke, onLayoutDrift);

        RegisterVisibilityRefresh(weatherRoot,
                                  instance.weatherVisibilityCookie);
        RegisterVisibilityRefresh(notificationGrid,
                                  instance.gridVisibilityCookie);

        // Popup open often keeps Visibility=Visible and only brings the
        // element into view — viewport change is the reliable "opened" signal.
        if (!instance.weatherViewportRevoker) {
            instance.weatherViewportRevoker = weatherRoot.EffectiveViewportChanged(
                winrt::auto_revoke,
                [](wux::FrameworkElement const&,
                   wux::EffectiveViewportChangedEventArgs const& args) {
                    try {
                        auto vp = args.EffectiveViewport();
                        if (vp.Width > 1.0 && vp.Height > 1.0) {
                            RefreshDaylightOnFlyoutShown();
                        }
                    } catch (...) {
                    }
                });
        }

        // System theme toggles re-theme NotificationCenterGrid (opaque acrylic
        // + shadow) under our weather Border. Re-strip and rematch brushes.
        if (!instance.gridThemeChangedRevoker) {
            const InstanceHandle handle = instance.gridHandle;
            auto enqueueChromeRefresh = [handle]() {
                try {
                    if (auto dq =
                            ws::DispatcherQueue::GetForCurrentThread()) {
                        dq.TryEnqueue(ws::DispatcherQueueHandler([handle]() {
                            RefreshWeatherChromeForHandle(handle);
                        }));
                        return;
                    }
                } catch (...) {
                }
                RefreshWeatherChromeForHandle(handle);
            };

            instance.gridThemeChangedRevoker =
                notificationGrid.ActualThemeChanged(
                    winrt::auto_revoke,
                    [enqueueChromeRefresh](wux::FrameworkElement const&,
                                           wf::IInspectable const&) {
                        Wh_Log(L"NotificationCenterGrid theme changed — "
                               L"refreshing weather chrome");
                        enqueueChromeRefresh();
                    });
            instance.weatherThemeChangedRevoker =
                weatherRoot.ActualThemeChanged(
                    winrt::auto_revoke,
                    [enqueueChromeRefresh](wux::FrameworkElement const&,
                                           wf::IInspectable const&) {
                        Wh_Log(L"Weather root theme changed — refreshing "
                               L"chrome");
                        enqueueChromeRefresh();
                    });
        }

        EnsureGridFitsWeatherContent(notificationGrid, weatherRoot);

        Wh_Log(L"Applied weather Border CornerRadius=%g (top-inset card)",
               applied.TopLeft);
    } catch (...) {
        Wh_Log(L"ApplyCalendarMatchedChrome failed %08X", winrt::to_hresult());
    }
}

void RefreshWeatherChromeForHandle(InstanceHandle handle) {
    if (g_shuttingDown.load()) {
        return;
    }
    if (g_refreshingWeatherChrome.exchange(true)) {
        return;
    }
    try {
        std::lock_guard lock(g_mountMutex);
        for (auto& mounted : g_mounted) {
            if (mounted.gridHandle != handle) {
                continue;
            }
            auto grid = mounted.notificationGrid.get();
            auto root = mounted.weatherRoot.get();
            if (grid && root) {
                ApplyCalendarMatchedChrome(mounted, grid, root);
            }
            break;
        }
    } catch (...) {
        Wh_Log(L"RefreshWeatherChromeForHandle failed %08X",
               winrt::to_hresult());
    }
    g_refreshingWeatherChrome.store(false);
}

////////////////////////////////////////////////////////////////////////////////
// VisualTreeWatcher implementation (needs HandleVisualTree*)

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
    : m_XamlDiagnostics(site.as<IXamlDiagnostics>()) {
    Wh_Log(L"Constructing VisualTreeWatcher");
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto* watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr =
                watcher->m_XamlDiagnostics.as<IVisualTreeService3>()
                    ->AdviseVisualTreeChange(watcher);
            watcher->Release();
            if (FAILED(hr)) {
                Wh_Log(L"AdviseVisualTreeChange error %08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
        AddRef();
        CloseHandle(thread);
    }
}

VisualTreeWatcher::~VisualTreeWatcher() {
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange() {
    Wh_Log(L"UnadviseVisualTreeChange");
    HRESULT hr = m_XamlDiagnostics.as<IVisualTreeService3>()
                     ->UnadviseVisualTreeChange(this);
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed %08X", hr);
    }
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(
    ParentChildRelation,
    VisualElement element,
    VisualMutationType mutationType) try {
    if (!g_initializedForThread || g_shuttingDown.load()) {
        return S_OK;
    }

    if (mutationType == Add) {
        const auto inspectable = FromHandle(element.Handle);
        auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
        if (frameworkElement) {
            HandleVisualTreeAdd(element.Handle, frameworkElement, element.Type);
        }
    } else if (mutationType == Remove) {
        HandleVisualTreeRemove(element.Handle);
    }

    return S_OK;
} catch (...) {
    Wh_Log(L"OnVisualTreeChange error %08X", winrt::to_hresult());
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle,
                                                 VisualElementState,
                                                 LPCWSTR) noexcept {
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Condition / icon mapping

PCWSTR ConditionLabelFromCode(int code) {
    switch (code) {
        case 0:
            return L"Clear";
        case 1:
            // WMO 1 is "Mainly clear" — distinct from 0, but same icon family.
            // Short label keeps the header tidy next to the condition icon.
            return L"Mostly clear";
        case 2:
            return L"Partly cloudy";
        case 3:
            return L"Overcast";
        case 45:
        case 48:
            return L"Fog";
        case 51:
        case 53:
        case 55:
        case 56:
        case 57:
            return L"Drizzle";
        case 61:
        case 63:
        case 80:
            return L"Rain";
        case 65:
        case 81:
        case 82:
            return L"Heavy rain";
        case 66:
        case 67:
            return L"Freezing rain";
        case 71:
        case 73:
            return L"Snow";
        case 75:
        case 77:
        case 85:
        case 86:
            return L"Snow showers";
        case 95:
            return L"Thunderstorm";
        case 96:
        case 99:
            return L"Thunderstorm with hail";
        default:
            return L"Unknown";
    }
}

WeatherIconKind IconKindFromCode(int code, bool isDay) {
    switch (code) {
        case 0:
        case 1:
            return isDay ? WeatherIconKind::ClearDay
                         : WeatherIconKind::ClearNight;
        case 2:
            return WeatherIconKind::PartlyCloudy;
        case 3:
            return WeatherIconKind::Cloudy;
        case 45:
        case 48:
            return WeatherIconKind::Fog;
        case 51:
        case 53:
        case 55:
        case 56:
        case 57:
            return WeatherIconKind::Drizzle;
        case 61:
        case 63:
        case 66:
        case 67:
        case 80:
            return WeatherIconKind::Rain;
        case 65:
        case 81:
        case 82:
            return WeatherIconKind::HeavyRain;
        case 71:
        case 73:
            return WeatherIconKind::Snow;
        case 75:
        case 77:
        case 85:
        case 86:
            return WeatherIconKind::HeavySnow;
        case 95:
        case 96:
        case 99:
            return WeatherIconKind::Thunderstorm;
        default:
            return WeatherIconKind::Unknown;
    }
}

// Original monochrome vector icons (24x24). Avoids Fluent/MDL2 private-use
// glyphs that are missing inside ShellExperienceHost / ShellHost.
PCWSTR WeatherIconPathData(WeatherIconKind kind) {
    switch (kind) {
        case WeatherIconKind::ClearDay:
            return L"M12,7.2A4.8,4.8 0 1 0 12.01,7.2Z M12,2.2 L12,4.2 M12,19.8 "
                   L"L12,21.8 M2.2,12 L4.2,12 M19.8,12 L21.8,12 M5.1,5.1 "
                   L"L6.5,6.5 M17.5,17.5 L18.9,18.9 M17.5,6.5 L18.9,5.1 "
                   L"M5.1,18.9 L6.5,17.5";
        case WeatherIconKind::ClearNight:
            return L"M14.2,3.2A8.2,8.2 0 1 0 20.5,14.8 6.4,6.4 0 1 1 14.2,3.2Z";
        case WeatherIconKind::PartlyCloudy:
            // Drawn as layered sun + cloud in MakeWeatherIcon.
            return L"";
        case WeatherIconKind::Cloudy:
            return L"M6.2,16.2h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
                   L"A3.5,3.5 0 0 0 6.2,16.2Z";
        case WeatherIconKind::Fog:
            return L"M5,9.2h14 M4.2,12.2h15.6 M5.5,15.2h13";
        case WeatherIconKind::Drizzle:
            return L"M6.2,11.2h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
                   L"A3.5,3.5 0 0 0 6.2,11.2Z M8.2,14.2 L7.2,17.2 M12,14.2 "
                   L"L11,17.2 M15.8,14.2 L14.8,17.2";
        case WeatherIconKind::Rain:
            return L"M6.2,10.5h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
                   L"A3.5,3.5 0 0 0 6.2,10.5Z M8,13.5 L6.8,17.8 M12,13.5 "
                   L"L10.8,17.8 M16,13.5 L14.8,17.8";
        case WeatherIconKind::HeavyRain:
            return L"M6.2,9.8h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
                   L"A3.5,3.5 0 0 0 6.2,9.8Z M7.2,12.6 L5.8,17.8 M10.4,12.6 "
                   L"L9,17.8 M13.6,12.6 L12.2,17.8 M16.8,12.6 L15.4,17.8";
        case WeatherIconKind::Snow:
            return L"M6.2,10.5h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
                   L"A3.5,3.5 0 0 0 6.2,10.5Z M8.2,14.2h0.1 M12,14.2h0.1 "
                   L"M15.8,14.2h0.1 M9.5,17h0.1 M14.2,17h0.1";
        case WeatherIconKind::HeavySnow:
            return L"M6.2,9.8h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
                   L"A3.5,3.5 0 0 0 6.2,9.8Z M7.5,13h0.1 M12,13h0.1 M16.5,13h0.1 "
                   L"M9,15.8h0.1 M14.5,15.8h0.1 M7.5,18.4h0.1 M12,18.4h0.1 "
                   L"M16.5,18.4h0.1";
        case WeatherIconKind::Thunderstorm:
            return L"M6.2,10.2h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
                   L"A3.5,3.5 0 0 0 6.2,10.2Z M13.2,11.2 L10.2,16.2h2.2 "
                   L"L9.8,21.2 14.8,14.8h-2.3z";
        default:
            return L"M12,4.5A7.5,7.5 0 1 0 12.01,4.5Z M12,9.2v4.2 M12,16.8h0.1";
    }
}

wuxc::Viewbox MakeWeatherIcon(WeatherIconKind kind,
                              double size,
                              wuxm::Brush const& brush) {
    wuxc::Canvas canvas;
    canvas.Width(24);
    canvas.Height(24);
    canvas.IsHitTestVisible(false);

    auto appendPath = [&](PCWSTR data, bool strokeOnly, double thickness) {
        try {
            std::wstring xaml =
                L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/"
                L"presentation' Data='";
            xaml += data;
            xaml += L"'/>";
            auto path =
                wux::Markup::XamlReader::Load(xaml).as<wuxs::Path>();
            if (strokeOnly) {
                path.Fill(nullptr);
                path.Stroke(brush);
                path.StrokeThickness(thickness);
                path.StrokeStartLineCap(wuxm::PenLineCap::Round);
                path.StrokeEndLineCap(wuxm::PenLineCap::Round);
                path.StrokeLineJoin(wuxm::PenLineJoin::Round);
            } else {
                path.Fill(brush);
                path.Stroke(brush);
                path.StrokeThickness(1.1);
                path.StrokeLineJoin(wuxm::PenLineJoin::Round);
            }
            canvas.Children().Append(path);
        } catch (...) {
            Wh_Log(L"Path icon load failed %08X", winrt::to_hresult());
        }
    };

    // Snow dots as small ellipses (path "h0.1" markers are unreliable).
    if (kind == WeatherIconKind::Snow || kind == WeatherIconKind::HeavySnow) {
        appendPath(
            L"M6.2,10.5h11.2a3.2,3.2 0 0 0 .25-6.35 4.6,4.6 0 0 0-8.7-1.5 "
            L"A3.5,3.5 0 0 0 6.2,10.5Z",
            false, 1.1);
        auto addDot = [&](double x, double y) {
            wuxs::Ellipse dot;
            dot.Width(1.8);
            dot.Height(1.8);
            dot.Fill(brush);
            wuxc::Canvas::SetLeft(dot, x);
            wuxc::Canvas::SetTop(dot, y);
            canvas.Children().Append(dot);
        };
        if (kind == WeatherIconKind::Snow) {
            addDot(7.6, 13.6);
            addDot(11.2, 13.6);
            addDot(14.8, 13.6);
            addDot(9.2, 16.6);
            addDot(13.4, 16.6);
        } else {
            addDot(6.8, 12.8);
            addDot(11.2, 12.8);
            addDot(15.6, 12.8);
            addDot(8.4, 15.4);
            addDot(13.6, 15.4);
            addDot(6.8, 17.8);
            addDot(11.2, 17.8);
            addDot(15.6, 17.8);
        }
    } else if (kind == WeatherIconKind::ClearDay) {
        // Keep rays inset from the 24x24 edges so a large header Viewbox
        // never paints into the card's top stroke.
        appendPath(L"M12,8.0A4.0,4.0 0 1 0 12.01,8.0Z", false, 1.1);
        appendPath(
            L"M12,3.6 L12,5.0 M12,19.0 L12,20.4 M3.6,12 L5.0,12 M19.0,12 "
            L"L20.4,12 M5.9,5.9 L7.0,7.0 M17.0,17.0 L18.1,18.1 M17.0,7.0 "
            L"L18.1,5.9 M5.9,18.1 L7.0,17.0",
            true, 1.5);
    } else if (kind == WeatherIconKind::PartlyCloudy) {
        // Solid sun + solid cloud with a real empty gap between them.
        // (Nested EvenOdd punches were drawing a donut hole in the sun.)
        appendPath(L"M18.0,3.4A2.55,2.55 0 1 0 18.01,3.4Z", false, 1.0);
        // Cloud top stays below the sun (~y 6) so acrylic shows through.
        appendPath(
            L"M3.5,18.8h12.8a3.1,3.1 0 0 0 .2-6.0 4.35,4.35 0 0 0-8.3-1.35 "
            L"A3.25,3.25 0 0 0 3.5,18.8Z",
            false, 1.05);
    } else if (kind == WeatherIconKind::Fog) {
        appendPath(L"M5,9.2h14 M4.2,12.2h15.6 M5.5,15.2h13", true, 1.8);
    } else if (kind == WeatherIconKind::Unknown) {
        appendPath(L"M12,4.5A7.5,7.5 0 1 0 12.01,4.5Z", true, 1.5);
        appendPath(L"M12,9.2v4.2", true, 1.6);
        wuxs::Ellipse tip;
        tip.Width(1.8);
        tip.Height(1.8);
        tip.Fill(brush);
        wuxc::Canvas::SetLeft(tip, 11.1);
        wuxc::Canvas::SetTop(tip, 16.4);
        canvas.Children().Append(tip);
    } else {
        appendPath(WeatherIconPathData(kind), false, 1.1);
    }

    wuxc::Viewbox viewbox;
    viewbox.Width(size);
    viewbox.Height(size);
    viewbox.Stretch(wuxm::Stretch::Uniform);
    viewbox.Child(canvas);
    viewbox.IsHitTestVisible(false);
    try {
        wuxa::AutomationProperties::SetAccessibilityView(
            viewbox, wuxap::AccessibilityView::Raw);
    } catch (...) {
    }
    return viewbox;
}

void SetWeatherIcon(wuxc::Grid const& host,
                    WeatherIconKind kind,
                    double size,
                    wuxm::Brush const& brush) {
    if (!host) {
        return;
    }
    host.Children().Clear();
    auto icon = MakeWeatherIcon(kind, size, brush);
    icon.HorizontalAlignment(wux::HorizontalAlignment::Center);
    icon.VerticalAlignment(wux::VerticalAlignment::Center);
    host.Children().Append(icon);
}

std::wstring FormatTemp(double value) {
    if (!std::isfinite(value)) {
        return L"--\u00B0";
    }
    wchar_t buffer[32];
    swprintf_s(buffer, L"%.0f\u00B0", value);
    return buffer;
}

std::wstring FormatHourLabel(int hour) {
    wchar_t buffer[16];
    swprintf_s(buffer, L"%d", hour);
    return buffer;
}

std::wstring UrlEncode(std::wstring_view input) {
    std::wstring out;
    out.reserve(input.size() * 3);
    for (wchar_t ch : input) {
        if ((ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z') ||
            (ch >= L'0' && ch <= L'9') || ch == L'-' || ch == L'_' ||
            ch == L'.' || ch == L'~') {
            out.push_back(ch);
        } else if (ch == L' ') {
            out.push_back(L'+');
        } else {
            char utf8[8];
            int len = WideCharToMultiByte(CP_UTF8, 0, &ch, 1, utf8,
                                          static_cast<int>(sizeof(utf8)),
                                          nullptr, nullptr);
            for (int i = 0; i < len; i++) {
                wchar_t hex[8];
                swprintf_s(hex, L"%%%02X", static_cast<unsigned char>(utf8[i]));
                out.append(hex);
            }
        }
    }
    return out;
}

std::wstring Utf8ToWide(std::string_view utf8) {
    if (utf8.empty()) {
        return {};
    }
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
                                   static_cast<int>(utf8.size()), nullptr, 0);
    if (size <= 0) {
        return {};
    }
    std::wstring wide(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()),
                        wide.data(), size);
    return wide;
}

bool ParseIsoLocalDateTime(std::wstring_view text,
                           SYSTEMTIME& outLocal,
                           bool& hasTime) {
    hasTime = false;
    // Formats: YYYY-MM-DD or YYYY-MM-DDTHH:MM or YYYY-MM-DDTHH:MM:SS
    if (text.size() < 10) {
        return false;
    }
    auto toInt = [](std::wstring_view s, int& v) -> bool {
        if (s.empty()) {
            return false;
        }
        std::wstring temp(s);
        wchar_t* end = nullptr;
        long value = wcstol(temp.c_str(), &end, 10);
        if (!end || end == temp.c_str()) {
            return false;
        }
        v = static_cast<int>(value);
        return true;
    };

    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    if (!toInt(text.substr(0, 4), year) || text[4] != L'-' ||
        !toInt(text.substr(5, 2), month) || text[7] != L'-' ||
        !toInt(text.substr(8, 2), day)) {
        return false;
    }

    ZeroMemory(&outLocal, sizeof(outLocal));
    outLocal.wYear = static_cast<WORD>(year);
    outLocal.wMonth = static_cast<WORD>(month);
    outLocal.wDay = static_cast<WORD>(day);

    if (text.size() >= 16 && text[10] == L'T') {
        if (!toInt(text.substr(11, 2), hour) || text[13] != L':' ||
            !toInt(text.substr(14, 2), minute)) {
            return false;
        }
        if (text.size() >= 19 && text[16] == L':') {
            toInt(text.substr(17, 2), second);
        }
        outLocal.wHour = static_cast<WORD>(hour);
        outLocal.wMinute = static_cast<WORD>(minute);
        outLocal.wSecond = static_cast<WORD>(second);
        hasTime = true;
    }
    return true;
}

ULARGE_INTEGER FileTimeToUlarge(const FILETIME& ft) {
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return u;
}

FILETIME GetForecastLocationNow(int utcOffsetSeconds) {
    SYSTEMTIME utc{};
    GetSystemTime(&utc);
    FILETIME utcFt{};
    SystemTimeToFileTime(&utc, &utcFt);
    ULARGE_INTEGER u = FileTimeToUlarge(utcFt);
    u.QuadPart += static_cast<LONGLONG>(utcOffsetSeconds) * 10000000LL;
    FILETIME localAsFt{};
    localAsFt.dwLowDateTime = u.LowPart;
    localAsFt.dwHighDateTime = u.HighPart;
    return localAsFt;
}

SYSTEMTIME GetForecastLocationSystemTime(int utcOffsetSeconds) {
    FILETIME nowLocal = GetForecastLocationNow(utcOffsetSeconds);
    SYSTEMTIME st{};
    FileTimeToSystemTime(&nowLocal, &st);
    return st;
}

// Compare civil Y-M-D-H (location-local). Returns <0, 0, >0.
int CompareCivilHour(int y, int m, int d, int h,
                     const SYSTEMTIME& now) {
    if (y != now.wYear) {
        return y < now.wYear ? -1 : 1;
    }
    if (m != now.wMonth) {
        return m < now.wMonth ? -1 : 1;
    }
    if (d != now.wDay) {
        return d < now.wDay ? -1 : 1;
    }
    if (h != now.wHour) {
        return h < now.wHour ? -1 : 1;
    }
    return 0;
}

size_t FindHourlyStartIndex(std::vector<HourlyEntry> const& all,
                            SYSTEMTIME const& nowLocal) {
    if (all.empty()) {
        return 0;
    }
    for (size_t i = 0; i < all.size(); i++) {
        if (all[i].hour < 0) {
            continue;
        }
        if (CompareCivilHour(all[i].year, all[i].month, all[i].day,
                             all[i].hour, nowLocal) >= 0) {
            return i;
        }
    }
    return all.size() - 1;
}

void ResliceHourlyForecast(ForecastData& data, int hourlyCount) {
    if (!data.valid) {
        data.hourly.clear();
        return;
    }
    // Copy before clearing data.hourly — when hourlyAll is empty we fall back
    // to the previous slice and must not invalidate that reference.
    std::vector<HourlyEntry> source =
        !data.hourlyAll.empty() ? data.hourlyAll : data.hourly;
    if (source.empty()) {
        data.hourly.clear();
        return;
    }

    SYSTEMTIME nowLocal = GetForecastLocationSystemTime(data.utcOffsetSeconds);
    const size_t start = FindHourlyStartIndex(source, nowLocal);
    const int prevFirstHour =
        data.hourly.empty() ? -1 : data.hourly.front().hour;
    data.hourly.clear();
    data.hourly.reserve(static_cast<size_t>((std::max)(hourlyCount, 0)));
    for (int n = 0; n < hourlyCount; n++) {
        const size_t idx = start + static_cast<size_t>(n);
        if (idx >= source.size()) {
            break;
        }
        data.hourly.push_back(source[idx]);
    }
    const int newFirstHour =
        data.hourly.empty() ? -1 : data.hourly.front().hour;
    if (newFirstHour != prevFirstHour) {
        Wh_Log(L"Hourly reslice: location now %04d-%02d-%02d %02d:xx → "
               L"start %s (%zu cols)",
               nowLocal.wYear, nowLocal.wMonth, nowLocal.wDay, nowLocal.wHour,
               data.hourly.empty() ? L"?"
                                   : data.hourly.front().hourLabel.c_str(),
               data.hourly.size());
    }
}

PCWSTR WeekdayAbbrev(WORD dayOfWeek) {
    static PCWSTR names[] = {L"Sun", L"Mon", L"Tue", L"Wed",
                             L"Thu", L"Fri", L"Sat"};
    if (dayOfWeek > 6) {
        return L"???";
    }
    return names[dayOfWeek];
}

wf::IInspectable TryFindThemeResource(wux::FrameworkElement const& element,
                                      PCWSTR key) {
    try {
        auto value = element.Resources().Lookup(winrt::box_value(key));
        if (value) {
            return value;
        }
    } catch (...) {
    }
    try {
        auto value =
            wux::Application::Current().Resources().Lookup(winrt::box_value(key));
        if (value) {
            return value;
        }
    } catch (...) {
    }
    return nullptr;
}

wuxm::Brush BrushOrFallback(wux::FrameworkElement const& element,
                            PCWSTR key,
                            winrt::Windows::UI::Color fallbackColor,
                            double opacity = 1.0) {
    try {
        if (auto value = TryFindThemeResource(element, key)) {
            if (auto brush = value.try_as<wuxm::Brush>()) {
                return brush;
            }
            if (auto color = value.try_as<wf::IReference<winrt::Windows::UI::Color>>()) {
                wuxm::SolidColorBrush solid(color.Value());
                solid.Opacity(opacity);
                return solid;
            }
        }
    } catch (...) {
    }
    wuxm::SolidColorBrush solid(fallbackColor);
    solid.Opacity(opacity);
    return solid;
}

////////////////////////////////////////////////////////////////////////////////
// Networking / parsing

std::optional<std::wstring> DownloadUrlUtf8AsWide(PCWSTR url) {
    if (g_shuttingDown.load()) {
        return std::nullopt;
    }

    const WH_URL_CONTENT* content = Wh_GetUrlContent(url, nullptr);
    if (!content) {
        Wh_Log(L"Wh_GetUrlContent failed for %s", url);
        return std::nullopt;
    }

    struct ContentGuard {
        const WH_URL_CONTENT* c;
        ~ContentGuard() {
            if (c) {
                Wh_FreeUrlContent(c);
            }
        }
    } guard{content};

    if (content->statusCode < 200 || content->statusCode >= 300) {
        Wh_Log(L"HTTP status %d for %s", content->statusCode, url);
        return std::nullopt;
    }
    if (!content->data || content->length == 0) {
        Wh_Log(L"Empty HTTP body for %s", url);
        return std::nullopt;
    }

    return Utf8ToWide(std::string_view(content->data, content->length));
}

bool JsonGetNumberArray(wdj::JsonObject const& obj,
                        PCWSTR key,
                        std::vector<double>& out) {
    out.clear();
    if (!obj.HasKey(key)) {
        return false;
    }
    auto arr = obj.GetNamedArray(key);
    out.reserve(arr.Size());
    for (uint32_t i = 0; i < arr.Size(); i++) {
        auto v = arr.GetAt(i);
        if (v.ValueType() == wdj::JsonValueType::Null) {
            out.push_back(std::numeric_limits<double>::quiet_NaN());
        } else {
            out.push_back(v.GetNumber());
        }
    }
    return true;
}

bool JsonGetIntArray(wdj::JsonObject const& obj,
                     PCWSTR key,
                     std::vector<int>& out) {
    out.clear();
    if (!obj.HasKey(key)) {
        return false;
    }
    auto arr = obj.GetNamedArray(key);
    out.reserve(arr.Size());
    for (uint32_t i = 0; i < arr.Size(); i++) {
        auto v = arr.GetAt(i);
        if (v.ValueType() == wdj::JsonValueType::Null) {
            out.push_back(-1);
        } else {
            out.push_back(static_cast<int>(v.GetNumber()));
        }
    }
    return true;
}

bool JsonGetStringArray(wdj::JsonObject const& obj,
                        PCWSTR key,
                        std::vector<std::wstring>& out) {
    out.clear();
    if (!obj.HasKey(key)) {
        return false;
    }
    auto arr = obj.GetNamedArray(key);
    out.reserve(arr.Size());
    for (uint32_t i = 0; i < arr.Size(); i++) {
        out.emplace_back(arr.GetAt(i).GetString().c_str());
    }
    return true;
}

std::optional<ResolvedLocation> GeocodeLocation(std::wstring const& name) {
    if (name.empty()) {
        return std::nullopt;
    }

    std::wstring url =
        L"https://geocoding-api.open-meteo.com/v1/search?name=" +
        UrlEncode(name) + L"&count=1&language=en&format=json";

    Wh_Log(L"Geocoding: %s", url.c_str());
    auto body = DownloadUrlUtf8AsWide(url.c_str());
    if (!body) {
        return std::nullopt;
    }

    try {
        auto root = wdj::JsonObject::Parse(*body);
        if (!root.HasKey(L"results")) {
            Wh_Log(L"Geocoding returned no results object");
            return std::nullopt;
        }
        auto results = root.GetNamedArray(L"results");
        if (results.Size() == 0) {
            Wh_Log(L"Geocoding returned empty results");
            return std::nullopt;
        }
        auto first = results.GetAt(0).GetObject();
        ResolvedLocation loc;
        loc.latitude = first.GetNamedNumber(L"latitude");
        loc.longitude = first.GetNamedNumber(L"longitude");
        if (first.HasKey(L"name")) {
            loc.displayName = first.GetNamedString(L"name").c_str();
        } else {
            loc.displayName = name;
        }
        if (first.HasKey(L"country_code")) {
            loc.displayName += L", ";
            loc.displayName += first.GetNamedString(L"country_code").c_str();
        }
        loc.valid = true;
        return loc;
    } catch (...) {
        Wh_Log(L"Geocode parse error %08X", winrt::to_hresult());
        return std::nullopt;
    }
}

std::optional<ForecastData> ParseForecastJson(std::wstring const& body,
                                              std::wstring const& displayName,
                                              std::wstring const& unit,
                                              int hourlyCount,
                                              int dailyCount) {
    try {
        auto root = wdj::JsonObject::Parse(body);
        ForecastData data;
        data.locationDisplay = displayName;
        data.temperatureUnit = unit;

        if (root.HasKey(L"utc_offset_seconds")) {
            data.utcOffsetSeconds =
                static_cast<int>(root.GetNamedNumber(L"utc_offset_seconds"));
        }

        if (!root.HasKey(L"current") || !root.HasKey(L"hourly") ||
            !root.HasKey(L"daily")) {
            Wh_Log(L"Forecast JSON missing required objects");
            return std::nullopt;
        }

        auto current = root.GetNamedObject(L"current");
        data.currentTemp = current.GetNamedNumber(L"temperature_2m");
        if (current.HasKey(L"weather_code")) {
            data.currentCode =
                static_cast<int>(current.GetNamedNumber(L"weather_code"));
        } else if (current.HasKey(L"weathercode")) {
            data.currentCode =
                static_cast<int>(current.GetNamedNumber(L"weathercode"));
        }
        if (current.HasKey(L"is_day")) {
            data.currentIsDay = current.GetNamedNumber(L"is_day") > 0.0;
        }

        auto hourly = root.GetNamedObject(L"hourly");
        std::vector<std::wstring> hourlyTimes;
        std::vector<double> hourlyTemps;
        std::vector<int> hourlyCodes;
        std::vector<int> hourlyIsDay;
        if (!JsonGetStringArray(hourly, L"time", hourlyTimes) ||
            !JsonGetNumberArray(hourly, L"temperature_2m", hourlyTemps)) {
            Wh_Log(L"Hourly arrays missing");
            return std::nullopt;
        }
        if (!JsonGetIntArray(hourly, L"weather_code", hourlyCodes) &&
            !JsonGetIntArray(hourly, L"weathercode", hourlyCodes)) {
            Wh_Log(L"Hourly weather_code missing");
            return std::nullopt;
        }
        JsonGetIntArray(hourly, L"is_day", hourlyIsDay);

        const size_t hourlySize =
            (std::min)({hourlyTimes.size(), hourlyTemps.size(),
                        hourlyCodes.size()});
        if (hourlySize == 0) {
            return std::nullopt;
        }

        data.hourlyAll.reserve(hourlySize);
        for (size_t i = 0; i < hourlySize; i++) {
            SYSTEMTIME st{};
            bool hasTime = false;
            if (!ParseIsoLocalDateTime(hourlyTimes[i], st, hasTime) ||
                !hasTime) {
                continue;
            }
            HourlyEntry entry;
            entry.year = st.wYear;
            entry.month = st.wMonth;
            entry.day = st.wDay;
            entry.hour = st.wHour;
            entry.hourLabel = FormatHourLabel(st.wHour);
            entry.temperature = hourlyTemps[i];
            entry.weatherCode = hourlyCodes[i];
            entry.isDay =
                (i < hourlyIsDay.size()) ? (hourlyIsDay[i] > 0) : true;
            data.hourlyAll.push_back(std::move(entry));
        }
        if (data.hourlyAll.empty()) {
            return std::nullopt;
        }

        ResliceHourlyForecast(data, hourlyCount);

        auto daily = root.GetNamedObject(L"daily");
        std::vector<std::wstring> dailyTimes;
        std::vector<double> dailyMin;
        std::vector<double> dailyMax;
        std::vector<int> dailyCodes;
        if (!JsonGetStringArray(daily, L"time", dailyTimes) ||
            !JsonGetNumberArray(daily, L"temperature_2m_min", dailyMin) ||
            !JsonGetNumberArray(daily, L"temperature_2m_max", dailyMax)) {
            Wh_Log(L"Daily arrays missing");
            return std::nullopt;
        }
        if (!JsonGetIntArray(daily, L"weather_code", dailyCodes) &&
            !JsonGetIntArray(daily, L"weathercode", dailyCodes)) {
            Wh_Log(L"Daily weather_code missing");
            return std::nullopt;
        }

        std::vector<std::wstring> dailySunrise;
        std::vector<std::wstring> dailySunset;
        JsonGetStringArray(daily, L"sunrise", dailySunrise);
        JsonGetStringArray(daily, L"sunset", dailySunset);

        const size_t dailySize =
            (std::min)({dailyTimes.size(), dailyMin.size(), dailyMax.size(),
                        dailyCodes.size()});
        if (dailySize == 0) {
            return std::nullopt;
        }

        const int takeDays =
            ClampInt(dailyCount, 1, static_cast<int>(dailySize));
        data.daily.reserve(static_cast<size_t>(takeDays));
        for (int i = 0; i < takeDays; i++) {
            SYSTEMTIME st{};
            bool hasTime = false;
            ParseIsoLocalDateTime(dailyTimes[static_cast<size_t>(i)], st,
                                  hasTime);
            FILETIME ft{};
            SystemTimeToFileTime(&st, &ft);
            SYSTEMTIME st2{};
            FileTimeToSystemTime(&ft, &st2);

            DailyEntry entry;
            entry.weekdayLabel = WeekdayAbbrev(st2.wDayOfWeek);
            entry.minTemp = dailyMin[static_cast<size_t>(i)];
            entry.maxTemp = dailyMax[static_cast<size_t>(i)];
            entry.weatherCode = dailyCodes[static_cast<size_t>(i)];

            if (static_cast<size_t>(i) < dailySunrise.size() &&
                static_cast<size_t>(i) < dailySunset.size()) {
                SYSTEMTIME riseSt{};
                SYSTEMTIME setSt{};
                bool riseHas = false;
                bool setHas = false;
                if (ParseIsoLocalDateTime(dailySunrise[static_cast<size_t>(i)],
                                          riseSt, riseHas) &&
                    riseHas &&
                    ParseIsoLocalDateTime(dailySunset[static_cast<size_t>(i)],
                                          setSt, setHas) &&
                    setHas) {
                    entry.hasSunriseSunset = true;
                    entry.sunriseMinute =
                        static_cast<int>(riseSt.wHour) * 60 + riseSt.wMinute;
                    entry.sunsetMinute =
                        static_cast<int>(setSt.wHour) * 60 + setSt.wMinute;
                }
            }

            data.daily.push_back(std::move(entry));
        }

        data.todayMin = data.daily.front().minTemp;
        data.todayMax = data.daily.front().maxTemp;
        if (data.daily.front().hasSunriseSunset) {
            data.hasDaylight = true;
            data.todaySunriseMinute = data.daily.front().sunriseMinute;
            data.todaySunsetMinute = data.daily.front().sunsetMinute;
        }
        data.valid = true;
        data.fetchedAtWall = std::chrono::system_clock::now();
        SYSTEMTIME localNow{};
        GetLocalTime(&localNow);
        data.fetchedLocalYear = localNow.wYear;
        data.fetchedLocalMonth = localNow.wMonth;
        data.fetchedLocalDay = localNow.wDay;
        data.fetchedLocalHour = localNow.wHour;
        return data;
    } catch (...) {
        Wh_Log(L"ParseForecastJson error %08X", winrt::to_hresult());
        return std::nullopt;
    }
}

std::optional<ResolvedLocation> TryWindowsGeolocation() {
    try {
        auto access =
            wdg::Geolocator::RequestAccessAsync().get();
        if (access != wdg::GeolocationAccessStatus::Allowed) {
            Wh_Log(L"Geolocation access not allowed (%d)",
                   static_cast<int>(access));
            return std::nullopt;
        }

        wdg::Geolocator geolocator;
        geolocator.DesiredAccuracy(wdg::PositionAccuracy::Default);
        auto position = geolocator.GetGeopositionAsync().get();
        if (!position) {
            return std::nullopt;
        }

        auto coord = position.Coordinate().Point().Position();
        ResolvedLocation loc;
        loc.latitude = coord.Latitude;
        loc.longitude = coord.Longitude;
        loc.displayName = L"Current location";
        loc.valid = true;

        // Best-effort reverse geocode for a friendly label.
        wchar_t latBuf[64];
        wchar_t lonBuf[64];
        swprintf_s(latBuf, L"%.4f", loc.latitude);
        swprintf_s(lonBuf, L"%.4f", loc.longitude);
        std::wstring reverseUrl =
            L"https://geocoding-api.open-meteo.com/v1/reverse?latitude=";
        reverseUrl += latBuf;
        reverseUrl += L"&longitude=";
        reverseUrl += lonBuf;
        reverseUrl += L"&language=en&format=json";
        if (auto body = DownloadUrlUtf8AsWide(reverseUrl.c_str())) {
            try {
                auto root = wdj::JsonObject::Parse(*body);
                if (root.HasKey(L"results")) {
                    auto results = root.GetNamedArray(L"results");
                    if (results.Size() > 0) {
                        auto first = results.GetAt(0).GetObject();
                        if (first.HasKey(L"name")) {
                            loc.displayName =
                                first.GetNamedString(L"name").c_str();
                            if (first.HasKey(L"country_code")) {
                                loc.displayName += L", ";
                                loc.displayName +=
                                    first.GetNamedString(L"country_code")
                                        .c_str();
                            }
                        }
                    }
                }
            } catch (...) {
                Wh_Log(L"Reverse geocode parse failed %08X",
                       winrt::to_hresult());
            }
        }

        Wh_Log(L"Auto location: %s (%.4f, %.4f)", loc.displayName.c_str(),
               loc.latitude, loc.longitude);
        return loc;
    } catch (...) {
        Wh_Log(L"TryWindowsGeolocation error %08X", winrt::to_hresult());
        return std::nullopt;
    }
}

bool ResolveCoordinates(ModSettings const& settings,
                        ResolvedLocation& outLocation) {
    if (settings.autoLocation) {
        std::wstring key = L"auto-geolocator";
        {
            std::lock_guard lock(g_forecastMutex);
            if (g_resolvedLocation.valid && g_resolvedKey == key) {
                outLocation = g_resolvedLocation;
                return true;
            }
        }

        if (auto detected = TryWindowsGeolocation()) {
            std::lock_guard lock(g_forecastMutex);
            g_resolvedLocation = *detected;
            g_resolvedKey = key;
            outLocation = g_resolvedLocation;
            return true;
        }

        Wh_Log(L"Auto location failed — falling back to manual settings");
    }

    if (settings.coordinatesValid) {
        outLocation.displayName = settings.locationName;
        outLocation.latitude = settings.latitude;
        outLocation.longitude = settings.longitude;
        outLocation.valid = true;
        return true;
    }

    std::wstring key = settings.locationName + L"|geocode";
    {
        std::lock_guard lock(g_forecastMutex);
        if (g_resolvedLocation.valid && g_resolvedKey == key) {
            outLocation = g_resolvedLocation;
            return true;
        }
    }

    auto geocoded = GeocodeLocation(settings.locationName);
    if (!geocoded) {
        return false;
    }

    {
        std::lock_guard lock(g_forecastMutex);
        g_resolvedLocation = *geocoded;
        g_resolvedKey = key;
        outLocation = g_resolvedLocation;
    }
    return true;
}

bool FetchForecastNetwork(uint64_t generation) {
    if (g_shuttingDown.load() || generation != g_fetchGeneration.load()) {
        return false;
    }

    auto settings = GetSettingsCopy();
    ResolvedLocation location;
    if (!ResolveCoordinates(settings, location)) {
        Wh_Log(L"Unable to resolve location");
        return false;
    }

    wchar_t latBuf[64];
    wchar_t lonBuf[64];
    swprintf_s(latBuf, L"%.4f", location.latitude);
    swprintf_s(lonBuf, L"%.4f", location.longitude);

    std::wstring url = L"https://api.open-meteo.com/v1/forecast?latitude=";
    url += latBuf;
    url += L"&longitude=";
    url += lonBuf;
    url += L"&current=temperature_2m,weather_code,is_day";
    url += L"&hourly=temperature_2m,weather_code,is_day";
    url += L"&daily=weather_code,temperature_2m_min,temperature_2m_max,"
           L"sunrise,sunset";
    url += L"&timezone=auto&forecast_days=7&temperature_unit=";
    url += settings.temperatureUnit;

    Wh_Log(L"Fetching weather: %s", url.c_str());
    auto body = DownloadUrlUtf8AsWide(url.c_str());
    if (!body) {
        return false;
    }
    if (g_shuttingDown.load() || generation != g_fetchGeneration.load()) {
        return false;
    }

    auto parsed =
        ParseForecastJson(*body, location.displayName, settings.temperatureUnit,
                          settings.hourlyCount, settings.dailyCount);
    if (!parsed) {
        return false;
    }
    if (g_shuttingDown.load() || generation != g_fetchGeneration.load()) {
        return false;
    }

    {
        std::lock_guard lock(g_forecastMutex);
        g_forecast = std::move(*parsed);
    }

    Wh_Log(L"Weather fetch succeeded for %s", location.displayName.c_str());
    return true;
}

void WeatherWorker(uint64_t generation) {
    struct ComInit {
        HRESULT hr;
        ComInit() {
            hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        }
        ~ComInit() {
            if (SUCCEEDED(hr)) {
                CoUninitialize();
            }
        }
    } com;

    bool previous = false;
    if (!g_fetchInProgress.compare_exchange_strong(previous, true)) {
        Wh_Log(L"Fetch already in progress");
        return;
    }
    g_fetchStartedTick.store(GetTickCount64());
    g_fetchStartedWallMs.store(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count());

    {
        struct ClearFlag {
            ~ClearFlag() {
                g_fetchInProgress = false;
                g_fetchStartedTick.store(0);
                g_fetchStartedWallMs.store(0);
            }
        } clearFlag;

        try {
            const bool ok = FetchForecastNetwork(generation);
            if (!ok) {
                Wh_Log(L"Weather fetch failed or superseded (gen=%llu)",
                       static_cast<unsigned long long>(generation));
            }
            // Always refresh UI so we leave the "Loading..." state on failure.
            UpdateAllWeatherUIs();
        } catch (...) {
            Wh_Log(L"WeatherWorker exception %08X", winrt::to_hresult());
            try {
                UpdateAllWeatherUIs();
            } catch (...) {
            }
        }
    }

    if (!g_shuttingDown.load() && g_forceRefreshPending.exchange(false)) {
        Wh_Log(L"Running queued forced weather refresh");
        RequestWeatherRefresh(true);
    }
}

bool IsForecastCacheStale(ForecastData const& forecast, int refreshMinutes) {
    if (!forecast.valid) {
        return true;
    }

    const auto age =
        std::chrono::system_clock::now() - forecast.fetchedAtWall;
    if (age < std::chrono::minutes(0)) {
        return true;
    }
    return age >= std::chrono::minutes(refreshMinutes);
}

void ClearStuckFetchLock() {
    if (!g_fetchInProgress.load()) {
        return;
    }
    const ULONGLONG started = g_fetchStartedTick.load();
    const ULONGLONG now = GetTickCount64();
    const int64_t startedWall = g_fetchStartedWallMs.load();
    const int64_t nowWall =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    // Tick count freezes across sleep; wall clock does not. Treat either
    // signal (or a missing start stamp) as a stuck latch.
    const bool stuckByTick =
        started == 0 || now - started > 90000ULL;
    const bool stuckByWall =
        startedWall == 0 || (nowWall - startedWall) > 90000;
    if (stuckByTick || stuckByWall) {
        Wh_Log(L"Clearing stuck weather fetch lock (tick age ms=%llu, wall "
               L"age ms=%lld)",
               started ? static_cast<unsigned long long>(now - started) : 0ULL,
               startedWall ? static_cast<long long>(nowWall - startedWall)
                           : 0LL);
        g_fetchInProgress.store(false);
        g_fetchStartedTick.store(0);
        g_fetchStartedWallMs.store(0);
    }
}

void RequestWeatherRefresh(bool forceNetwork) {
    if (g_shuttingDown.load()) {
        return;
    }

    ClearStuckFetchLock();

    // Coalesce: never bump the fetch generation while a worker is running —
    // that used to invalidate the in-flight result and leave the UI empty.
    if (g_fetchInProgress.load()) {
        if (forceNetwork) {
            g_forceRefreshPending.store(true);
        }
        Wh_Log(L"Weather refresh skipped — fetch already in progress%s",
               forceNetwork ? L" (queued)" : L"");
        // Still repaint daylight / cached card so the flyout is not frozen.
        if (!forceNetwork) {
            try {
                UpdateAllWeatherUIs();
            } catch (...) {
            }
        }
        return;
    }

    auto settings = GetSettingsCopy();
    bool needsFetch = forceNetwork;
    {
        std::lock_guard lock(g_forecastMutex);
        if (!g_forecast.valid) {
            needsFetch = true;
            Wh_Log(L"Weather refresh: no cached forecast");
        } else if (IsForecastCacheStale(g_forecast, settings.refreshMinutes)) {
            needsFetch = true;
            auto age =
                std::chrono::system_clock::now() - g_forecast.fetchedAtWall;
            auto ageMin =
                std::chrono::duration_cast<std::chrono::minutes>(age).count();
            Wh_Log(L"Weather refresh: cache stale (age %lld min, fetched "
                   L"local %04d-%02d-%02d %02d:xx)",
                   static_cast<long long>(ageMin), g_forecast.fetchedLocalYear,
                   g_forecast.fetchedLocalMonth, g_forecast.fetchedLocalDay,
                   g_forecast.fetchedLocalHour);
        } else if (forceNetwork) {
            Wh_Log(L"Weather refresh: forced");
        } else {
            Wh_Log(L"Weather refresh: using cache");
        }
        if (g_forecast.valid &&
            g_forecast.temperatureUnit != settings.temperatureUnit) {
            needsFetch = true;
        }
    }

    if (!needsFetch) {
        UpdateAllWeatherUIs();
        return;
    }

    // Paint immediately from the cached full hourly series (re-sliced to
    // "now") so the strip does not stay stuck on an old hour while the
    // network fetch runs — or if that fetch fails/hangs.
    UpdateAllWeatherUIs();

    uint64_t generation = ++g_fetchGeneration;
    if (!QueueUserWorkItem(
            [](PVOID param) -> DWORD {
                auto generation =
                    static_cast<uint64_t>(reinterpret_cast<uintptr_t>(param));
                WeatherWorker(generation);
                return 0;
            },
            reinterpret_cast<PVOID>(static_cast<uintptr_t>(generation)),
            WT_EXECUTEDEFAULT)) {
        Wh_Log(L"QueueUserWorkItem failed");
    }
}

FILETIME MakeRelativeDueTime(DWORD delayMs) {
    FILETIME due{};
    ULARGE_INTEGER u;
    // Thread-pool relative due times are negative 100ns intervals.
    u.QuadPart = 0ULL - (static_cast<ULONGLONG>(delayMs) * 10000ULL);
    due.dwLowDateTime = u.LowPart;
    due.dwHighDateTime = u.HighPart;
    return due;
}

void EnsureRefreshTimer() {
    if (g_shuttingDown.load()) {
        return;
    }

    auto settings = GetSettingsCopy();
    if (g_shuttingDown.load()) {
        return;
    }
    const DWORD periodMs =
        static_cast<DWORD>(settings.refreshMinutes) * 60U * 1000U;
    FILETIME due = MakeRelativeDueTime(periodMs);

    std::lock_guard lock(g_timerMutex);
    if (g_shuttingDown.load()) {
        return;
    }
    if (g_refreshTimer) {
        // Do not reset the countdown on every remount/init helper call —
        // that used to push the next hourly fetch out indefinitely.
        if (g_refreshTimerPeriodMs == periodMs) {
            return;
        }
        SetThreadpoolTimer(g_refreshTimer, &due, periodMs, 30 * 1000);
        g_refreshTimerPeriodMs = periodMs;
        Wh_Log(L"Refresh timer period updated to %d minutes",
               settings.refreshMinutes);
        return;
    }

    g_refreshTimer = CreateThreadpoolTimer(
        [](PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER) {
            if (g_shuttingDown.load()) {
                return;
            }
            Wh_Log(L"Hourly weather timer fired — fetching");
            RequestWeatherRefresh(true);
        },
        nullptr, nullptr);
    if (!g_refreshTimer) {
        Wh_Log(L"CreateThreadpoolTimer failed");
        return;
    }

    SetThreadpoolTimer(g_refreshTimer, &due, periodMs, 30 * 1000);
    g_refreshTimerPeriodMs = periodMs;
    Wh_Log(L"Refresh timer scheduled every %d minutes (first fire in %d min)",
           settings.refreshMinutes, settings.refreshMinutes);
}

void EnsureDaylightTickTimer() {
    if (g_shuttingDown.load()) {
        return;
    }
    if (!GetSettingsCopy().showCalendarDaylight) {
        return;
    }

    // Now-marker + "hours left" while the flyout stays open.
    constexpr DWORD kPeriodMs = 5U * 60U * 1000U;
    FILETIME due = MakeRelativeDueTime(kPeriodMs);

    std::lock_guard lock(g_timerMutex);
    if (g_shuttingDown.load()) {
        return;
    }
    if (g_daylightTickTimer) {
        // Do NOT reschedule — remount/flyout-open used to push due time out
        // forever so the tick never fired.
        return;
    }

    g_daylightTickTimer = CreateThreadpoolTimer(
        [](PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER) {
            if (g_shuttingDown.load()) {
                return;
            }
            Wh_Log(L"Daylight tick timer fired");
            try {
                // Same cadence as daylight: advance hourly columns from cache
                // and fetch if the wall-clock cache is stale.
                RequestWeatherRefresh(false);
            } catch (...) {
            }
            try {
                UpdateAllDaylightUIs();
            } catch (...) {
            }
        },
        nullptr, nullptr);
    if (!g_daylightTickTimer) {
        Wh_Log(L"CreateThreadpoolTimer (daylight tick) failed");
        return;
    }

    SetThreadpoolTimer(g_daylightTickTimer, &due, kPeriodMs, 30 * 1000);
    Wh_Log(L"Daylight tick timer scheduled every 5 minutes");
}

void StartDaylightDispatcherTick(MountedDaylightInstance& instance) {
    if (g_shuttingDown.load() || instance.daylightTickTimer) {
        return;
    }
    if (!instance.dispatcherQueue) {
        return;
    }
    try {
        auto timer = instance.dispatcherQueue.CreateTimer();
        timer.Interval(std::chrono::minutes(5));
        timer.IsRepeating(true);
        timer.Tick([](ws::DispatcherQueueTimer const&,
                      wf::IInspectable const&) {
            if (g_shuttingDown.load()) {
                return;
            }
            Wh_Log(L"Daylight DispatcherQueue tick");
            try {
                RequestWeatherRefresh(false);
            } catch (...) {
            }
            try {
                UpdateAllDaylightUIs();
            } catch (...) {
            }
        });
        timer.Start();
        instance.daylightTickTimer = timer;
        Wh_Log(L"Daylight DispatcherQueue timer started (5 min)");
    } catch (...) {
        Wh_Log(L"StartDaylightDispatcherTick failed %08X",
               winrt::to_hresult());
    }
}

void StopRefreshTimer() {
    // Take timer pointers under the lock, then wait/close OUTSIDE it.
    // Timer callbacks (daylight tick → RequestWeatherRefresh → EnsureRefreshTimer)
    // also take g_timerMutex; waiting while holding it deadlocks Uninit.
    PTP_TIMER refresh = nullptr;
    PTP_TIMER daylight = nullptr;
    {
        std::lock_guard lock(g_timerMutex);
        refresh = g_refreshTimer;
        daylight = g_daylightTickTimer;
        g_refreshTimer = nullptr;
        g_daylightTickTimer = nullptr;
        g_refreshTimerPeriodMs = 0;
    }
    if (refresh) {
        SetThreadpoolTimer(refresh, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(refresh, TRUE);
        CloseThreadpoolTimer(refresh);
        Wh_Log(L"Refresh timer stopped");
    }
    if (daylight) {
        SetThreadpoolTimer(daylight, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(daylight, TRUE);
        CloseThreadpoolTimer(daylight);
        Wh_Log(L"Daylight tick timer stopped");
    }
}

#ifndef DEVICE_NOTIFY_CALLBACK
#define DEVICE_NOTIFY_CALLBACK 2
#endif

// Display power state GUID (Connected Standby / modern sleep often skips APM).
static const GUID kGuidConsoleDisplayState = {
    0x6fe69556,
    0x704a,
    0x47a0,
    {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};

#ifndef PBT_POWERSETTINGCHANGE
#define PBT_POWERSETTINGCHANGE 0x8013
#endif

#ifndef POWERBROADCAST_SETTING_DEFINED_LOCAL
struct PowerBroadcastSettingLocal {
    GUID PowerSetting;
    DWORD DataLength;
    UCHAR Data[1];
};
#endif

// Local copy of DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS — avoids depending on
// which Windows SDK headers Windhawk's toolchain exposes.
struct SuspendResumeNotifyParams {
    ULONG(CALLBACK* Callback)(PVOID, ULONG, PVOID);
    PVOID Context;
};

void OnSystemResumeRefresh() {
    if (g_shuttingDown.load()) {
        return;
    }
    const ULONGLONG nowTick = GetTickCount64();
    const ULONGLONG last = g_lastResumeRefreshTick.load();
    if (last != 0 && nowTick - last < 5000ULL) {
        return;
    }
    g_lastResumeRefreshTick.store(nowTick);

    Wh_Log(L"System resume/display-on — forcing weather + daylight refresh");

    // Cancel any fetch that hung while the NIC stack was asleep, then force a
    // new one. Daylight uses wall-clock "now" so it can look correct while the
    // weather card still shows the pre-sleep hourly slice.
    ++g_fetchGeneration;
    g_fetchInProgress.store(false);
    g_fetchStartedTick.store(0);
    g_fetchStartedWallMs.store(0);
    g_forceRefreshPending.store(false);

    EnsureRefreshTimer();
    try {
        UpdateAllDaylightUIs();
    } catch (...) {
    }
    RequestWeatherRefresh(true);
}

ULONG CALLBACK SuspendResumeNotifyCallback(PVOID /*context*/,
                                           ULONG type,
                                           PVOID /*setting*/) {
    switch (type) {
        case PBT_APMRESUMEAUTOMATIC:
        case PBT_APMRESUMESUSPEND:
#ifdef PBT_APMRESUMECRITICAL
        case PBT_APMRESUMECRITICAL:
#endif
            OnSystemResumeRefresh();
            break;
        default:
            break;
    }
    return ERROR_SUCCESS;
}

ULONG CALLBACK DisplayPowerNotifyCallback(PVOID /*context*/,
                                          ULONG type,
                                          PVOID setting) {
    if (type != PBT_POWERSETTINGCHANGE || !setting) {
        return ERROR_SUCCESS;
    }
    auto* power = reinterpret_cast<PowerBroadcastSettingLocal*>(setting);
    if (!IsEqualGUID(power->PowerSetting, kGuidConsoleDisplayState) ||
        power->DataLength < sizeof(DWORD)) {
        return ERROR_SUCCESS;
    }
    const DWORD state = *reinterpret_cast<DWORD*>(power->Data);
    // 0 = off, 1 = on, 2 = dimmed. Refresh when the display turns on after
    // modern standby (which often skips classic APM resume notifications).
    if (state == 1) {
        OnSystemResumeRefresh();
    }
    return ERROR_SUCCESS;
}

void RegisterSuspendResumeRefresh() {
    if (!g_suspendResumeNotify) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            using RegisterFn = HANDLE(WINAPI*)(HANDLE, DWORD);
            auto registerFn = reinterpret_cast<RegisterFn>(
                GetProcAddress(user32, "RegisterSuspendResumeNotification"));
            if (registerFn) {
                SuspendResumeNotifyParams params{};
                params.Callback = SuspendResumeNotifyCallback;
                params.Context = nullptr;
                g_suspendResumeNotify =
                    registerFn(&params, DEVICE_NOTIFY_CALLBACK);
                if (g_suspendResumeNotify) {
                    Wh_Log(L"Registered suspend/resume weather refresh");
                } else {
                    Wh_Log(L"RegisterSuspendResumeNotification failed (%lu)",
                           GetLastError());
                }
            } else {
                Wh_Log(L"RegisterSuspendResumeNotification unavailable");
            }
        }
    }

    if (!g_displayPowerNotify) {
        HMODULE powrprof = LoadLibraryExW(L"powrprof.dll", nullptr,
                                          LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (powrprof) {
            using RegisterPowerFn = DWORD(WINAPI*)(LPCGUID, DWORD, HANDLE,
                                                   HANDLE*);
            auto registerPower = reinterpret_cast<RegisterPowerFn>(
                GetProcAddress(powrprof, "PowerSettingRegisterNotification"));
            if (registerPower) {
                SuspendResumeNotifyParams params{};
                params.Callback = DisplayPowerNotifyCallback;
                params.Context = nullptr;
                HANDLE handle = nullptr;
                const DWORD err = registerPower(&kGuidConsoleDisplayState,
                                                DEVICE_NOTIFY_CALLBACK,
                                                &params, &handle);
                if (err == ERROR_SUCCESS && handle) {
                    g_displayPowerNotify = handle;
                    Wh_Log(L"Registered display-on weather refresh");
                } else {
                    Wh_Log(L"PowerSettingRegisterNotification failed (%lu)",
                           err);
                }
            }
        }
    }
}

void UnregisterSuspendResumeRefresh() {
    if (g_suspendResumeNotify) {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            using UnregisterFn = BOOL(WINAPI*)(HANDLE);
            auto unregisterFn = reinterpret_cast<UnregisterFn>(
                GetProcAddress(user32, "UnregisterSuspendResumeNotification"));
            if (unregisterFn) {
                unregisterFn(g_suspendResumeNotify);
            }
        }
        g_suspendResumeNotify = nullptr;
    }

    if (g_displayPowerNotify) {
        HMODULE powrprof = GetModuleHandleW(L"powrprof.dll");
        if (powrprof) {
            using UnregisterPowerFn = DWORD(WINAPI*)(HANDLE);
            auto unregisterPower = reinterpret_cast<UnregisterPowerFn>(
                GetProcAddress(powrprof, "PowerSettingUnregisterNotification"));
            if (unregisterPower) {
                unregisterPower(g_displayPowerNotify);
            }
        }
        g_displayPowerNotify = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// UI construction / updates

void SaveLocalProperty(wux::DependencyObject const& element,
                       wux::DependencyProperty const& property,
                       std::vector<SavedProperty>& out) {
    try {
        SavedProperty saved;
        saved.property = property;
        saved.value = element.ReadLocalValue(property);
        out.push_back(std::move(saved));
    } catch (...) {
        Wh_Log(L"SaveLocalProperty error %08X", winrt::to_hresult());
    }
}

void RestoreLocalProperties(wux::DependencyObject const& element,
                            std::vector<SavedProperty> const& saved) {
    for (auto const& item : saved) {
        try {
            if (!item.value || item.value == wux::DependencyProperty::UnsetValue()) {
                element.ClearValue(item.property);
            } else {
                element.SetValue(item.property, item.value);
            }
        } catch (...) {
            Wh_Log(L"RestoreLocalProperties error %08X", winrt::to_hresult());
        }
    }
}

wuxc::Border MakeDivider(wux::FrameworkElement const& /*host*/) {
    wuxc::Border divider;
    divider.Height(1);
    divider.Margin(wux::Thickness{0, 6, 0, 6});
    divider.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    // Soft hairline — theme secondary brushes are too opaque on acrylic.
    wuxm::SolidColorBrush brush(
        winrt::Windows::UI::Color{255, 255, 255, 255});
    brush.Opacity(0.18);
    divider.Background(brush);
    return divider;
}

wuxc::Grid MakeTemperatureTrack(wux::FrameworkElement const& host,
                                double dayMin,
                                double dayMax,
                                double globalMin,
                                double globalMax) {
    double span = globalMax - globalMin;
    double leftRatio = 0;
    double rangeRatio = 1;
    double rightRatio = 0;
    if (span > 0.0001) {
        leftRatio = (dayMin - globalMin) / span;
        rangeRatio = (dayMax - dayMin) / span;
        rightRatio = 1.0 - leftRatio - rangeRatio;
    }
    leftRatio = (std::max)(0.0, (std::min)(1.0, leftRatio));
    rangeRatio = (std::max)(0.0, (std::min)(1.0, rangeRatio));
    rightRatio = (std::max)(0.0, (std::min)(1.0, rightRatio));
    double sum = leftRatio + rangeRatio + rightRatio;
    if (sum <= 0.0001) {
        leftRatio = 0;
        rangeRatio = 1;
        rightRatio = 0;
    } else {
        leftRatio /= sum;
        rangeRatio /= sum;
        rightRatio /= sum;
    }

    wuxc::Grid track;
    track.Height(5);
    track.VerticalAlignment(wux::VerticalAlignment::Center);
    track.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

    auto cols = track.ColumnDefinitions();
    wuxc::ColumnDefinition c0;
    c0.Width(wux::GridLength{leftRatio <= 0 ? 0.0001 : leftRatio,
                             wux::GridUnitType::Star});
    wuxc::ColumnDefinition c1;
    c1.Width(wux::GridLength{(std::max)(0.0001, rangeRatio),
                             wux::GridUnitType::Star});
    wuxc::ColumnDefinition c2;
    c2.Width(wux::GridLength{rightRatio <= 0 ? 0.0001 : rightRatio,
                             wux::GridUnitType::Star});
    cols.Append(c0);
    cols.Append(c1);
    cols.Append(c2);

    wuxc::Border background;
    wuxc::Grid::SetColumnSpan(background, 3);
    background.CornerRadius(wux::CornerRadius{2.5, 2.5, 2.5, 2.5});
    wuxm::SolidColorBrush trackBg(
        winrt::Windows::UI::Color{255, 255, 255, 255});
    trackBg.Opacity(0.18);
    background.Background(trackBg);
    track.Children().Append(background);

    wuxc::Border active;
    wuxc::Grid::SetColumn(active, 1);
    active.CornerRadius(wux::CornerRadius{2.5, 2.5, 2.5, 2.5});
    active.Background(BrushOrFallback(
        host, L"TextFillColorPrimaryBrush",
        winrt::Windows::UI::Color{255, 240, 240, 240}, 0.95));
    track.Children().Append(active);
    return track;
}

std::wstring FormatClockHm(int minuteOfDay) {
    minuteOfDay = ((minuteOfDay % 1440) + 1440) % 1440;
    wchar_t buffer[16];
    swprintf_s(buffer, L"%02d:%02d", minuteOfDay / 60, minuteOfDay % 60);
    return buffer;
}

std::wstring FormatDaylightDuration(int sunriseMinute, int sunsetMinute) {
    int span = sunsetMinute - sunriseMinute;
    if (span < 0) {
        span += 1440;
    }
    wchar_t buffer[32];
    swprintf_s(buffer, L"%dh %dm", span / 60, span % 60);
    return buffer;
}

std::wstring FormatDurationMinutes(int minutes) {
    if (minutes < 0) {
        minutes = 0;
    }
    wchar_t buffer[32];
    swprintf_s(buffer, L"%dh %dm", minutes / 60, minutes % 60);
    return buffer;
}

// Remaining daylight today: full span before sunrise, countdown until sunset
// during the day, zero after sunset.
int DaylightMinutesRemaining(int nowMinute,
                             int sunriseMinute,
                             int sunsetMinute) {
    nowMinute = ((nowMinute % 1440) + 1440) % 1440;
    sunriseMinute = ((sunriseMinute % 1440) + 1440) % 1440;
    sunsetMinute = ((sunsetMinute % 1440) + 1440) % 1440;
    if (sunsetMinute < sunriseMinute) {
        // Rare wrap — treat as no remaining.
        return 0;
    }
    if (nowMinute < sunriseMinute) {
        return sunsetMinute - sunriseMinute;
    }
    if (nowMinute >= sunsetMinute) {
        return 0;
    }
    return sunsetMinute - nowMinute;
}

wuxc::Viewbox MakeSunHalfIcon(bool sunrise,
                              double size,
                              wuxm::Brush const& brush) {
    // Tight canvas so Viewbox centers the glyph with adjacent text. A tall
    // 16x16 canvas left empty space under the sunrise arc and made it sit high.
    wuxc::Canvas canvas;
    canvas.Width(16);
    canvas.Height(10);
    canvas.IsHitTestVisible(false);

    try {
        // Sunrise: upper semicircle on a horizon. Sunset: lower (flipped).
        // Both packed into the same 16x10 box so optical centers match.
        std::wstring data =
            sunrise ? L"M2,8 A6,6 0 0 1 14,8 Z M1.5,8.5 L14.5,8.5"
                    : L"M2,2 A6,6 0 0 0 14,2 Z M1.5,1.5 L14.5,1.5";
        std::wstring xaml =
            L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation' Data='";
        xaml += data;
        xaml += L"'/>";
        auto path = wux::Markup::XamlReader::Load(xaml).as<wuxs::Path>();
        path.Fill(brush);
        path.Stroke(brush);
        path.StrokeThickness(1.2);
        path.StrokeStartLineCap(wuxm::PenLineCap::Round);
        path.StrokeEndLineCap(wuxm::PenLineCap::Round);
        canvas.Children().Append(path);
    } catch (...) {
    }

    wuxc::Viewbox viewbox;
    viewbox.Width(size);
    viewbox.Height(size * 0.72);
    viewbox.Stretch(wuxm::Stretch::Uniform);
    viewbox.VerticalAlignment(wux::VerticalAlignment::Center);
    viewbox.Child(canvas);
    viewbox.IsHitTestVisible(false);
    return viewbox;
}

wuxc::Viewbox MakeNowMarkerArrow(double size, wuxm::Brush const& brush) {
    wuxc::Canvas canvas;
    canvas.Width(10);
    canvas.Height(6);
    canvas.IsHitTestVisible(false);
    try {
        // Small filled triangle pointing down.
        std::wstring xaml =
            L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation' Data='M0,0 L10,0 L5,6 Z'/>";
        auto path = wux::Markup::XamlReader::Load(xaml).as<wuxs::Path>();
        path.Fill(brush);
        path.Stroke(nullptr);
        canvas.Children().Append(path);
    } catch (...) {
    }
    wuxc::Viewbox viewbox;
    viewbox.Width(size);
    viewbox.Height(size * 0.6);
    viewbox.Stretch(wuxm::Stretch::Uniform);
    viewbox.Child(canvas);
    viewbox.IsHitTestVisible(false);
    return viewbox;
}

wuxc::Grid MakeDaylightNowMarker(wux::FrameworkElement const& host,
                                 int nowMinute) {
    nowMinute = ((nowMinute % 1440) + 1440) % 1440;
    const double leftRatio = static_cast<double>(nowMinute) / 1440.0;
    const double rightRatio = 1.0 - leftRatio;

    wuxc::Grid row;
    row.Height(8);
    row.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

    // Two columns so the shared edge is exactly "now" on the 0–24h axis.
    wuxc::ColumnDefinition c0;
    c0.Width(wux::GridLength{leftRatio <= 0 ? 0.0001 : leftRatio,
                             wux::GridUnitType::Star});
    wuxc::ColumnDefinition c1;
    c1.Width(wux::GridLength{rightRatio <= 0 ? 0.0001 : rightRatio,
                             wux::GridUnitType::Star});
    row.ColumnDefinitions().Append(c0);
    row.ColumnDefinitions().Append(c1);

    auto brush = BrushOrFallback(
        host, L"TextFillColorPrimaryBrush",
        winrt::Windows::UI::Color{255, 240, 240, 240}, 0.95);
    auto arrow = MakeNowMarkerArrow(8, brush);
    arrow.HorizontalAlignment(wux::HorizontalAlignment::Right);
    arrow.VerticalAlignment(wux::VerticalAlignment::Bottom);
    // Center the triangle on the column boundary (half of ~8px width).
    arrow.Margin(wux::Thickness{0, 0, -4, 0});
    wuxc::Grid::SetColumn(arrow, 0);
    row.Children().Append(arrow);
    return row;
}

wux::FrameworkElement BuildDaylightRoot(DaylightUiControls& ui) {
    wuxc::StackPanel root;
    root.Name(L"WindhawkDaylightRoot");
    root.Spacing(4);
    root.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    root.VerticalAlignment(wux::VerticalAlignment::Top);
    // Sit snugly under the date in the clocks grid row.
    root.Margin(wux::Thickness{0, 4, 0, 8});

    wuxc::StackPanel barStack;
    barStack.Spacing(5);
    barStack.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

    wuxc::Grid nowMarkerHost;
    nowMarkerHost.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    nowMarkerHost.Height(8);
    ui.nowMarkerHost = nowMarkerHost;

    wuxc::Grid trackHost;
    trackHost.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    trackHost.Height(5);
    ui.trackHost = trackHost;

    barStack.Children().Append(nowMarkerHost);
    barStack.Children().Append(trackHost);

    wuxc::Grid infoRow;
    infoRow.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    wuxc::ColumnDefinition leftCol;
    leftCol.Width(wux::GridLength{1, wux::GridUnitType::Star});
    wuxc::ColumnDefinition midCol;
    midCol.Width(wux::GridLength{0, wux::GridUnitType::Auto});
    wuxc::ColumnDefinition rightCol;
    rightCol.Width(wux::GridLength{1, wux::GridUnitType::Star});
    infoRow.ColumnDefinitions().Append(leftCol);
    infoRow.ColumnDefinitions().Append(midCol);
    infoRow.ColumnDefinitions().Append(rightCol);

    auto primary = wuxm::SolidColorBrush(
        winrt::Windows::UI::Color{255, 240, 240, 240});

    wuxc::StackPanel sunriseStack;
    sunriseStack.Orientation(wuxc::Orientation::Horizontal);
    sunriseStack.Spacing(6);
    sunriseStack.HorizontalAlignment(wux::HorizontalAlignment::Left);
    sunriseStack.VerticalAlignment(wux::VerticalAlignment::Center);
    sunriseStack.Children().Append(MakeSunHalfIcon(true, 14, primary));

    wuxc::TextBlock sunriseText;
    sunriseText.FontSize(12);
    sunriseText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    sunriseText.VerticalAlignment(wux::VerticalAlignment::Center);
    sunriseText.Text(L"--:--");
    ui.sunriseText = sunriseText;
    sunriseStack.Children().Append(sunriseText);
    wuxc::Grid::SetColumn(sunriseStack, 0);

    wuxc::StackPanel durationStack;
    durationStack.Orientation(wuxc::Orientation::Vertical);
    durationStack.Spacing(1);
    durationStack.HorizontalAlignment(wux::HorizontalAlignment::Center);
    durationStack.VerticalAlignment(wux::VerticalAlignment::Center);
    ui.durationStack = durationStack;

    wuxc::TextBlock durationText;
    durationText.FontSize(12);
    durationText.FontWeight(wut::FontWeights::SemiBold());
    durationText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    durationText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    durationText.VerticalAlignment(wux::VerticalAlignment::Center);
    durationText.Text(L"--");
    ui.durationText = durationText;

    wuxc::TextBlock remainingText;
    remainingText.FontSize(11);
    remainingText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    remainingText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    remainingText.VerticalAlignment(wux::VerticalAlignment::Center);
    remainingText.Text(L"--");
    remainingText.Visibility(wux::Visibility::Collapsed);
    ui.remainingText = remainingText;

    durationStack.Children().Append(durationText);
    durationStack.Children().Append(remainingText);
    wuxc::Grid::SetColumn(durationStack, 1);

    wuxc::StackPanel sunsetStack;
    sunsetStack.Orientation(wuxc::Orientation::Horizontal);
    sunsetStack.Spacing(6);
    sunsetStack.HorizontalAlignment(wux::HorizontalAlignment::Right);
    sunsetStack.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock sunsetText;
    sunsetText.FontSize(12);
    sunsetText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    sunsetText.VerticalAlignment(wux::VerticalAlignment::Center);
    sunsetText.Text(L"--:--");
    ui.sunsetText = sunsetText;
    sunsetStack.Children().Append(MakeSunHalfIcon(false, 14, primary));
    sunsetStack.Children().Append(sunsetText);
    wuxc::Grid::SetColumn(sunsetStack, 2);

    infoRow.Children().Append(sunriseStack);
    infoRow.Children().Append(durationStack);
    infoRow.Children().Append(sunsetStack);

    root.Children().Append(barStack);
    root.Children().Append(infoRow);

    try {
        wuxa::AutomationProperties::SetName(root, L"Daylight hours");
    } catch (...) {
    }
    return root;
}

bool IsDaylightRootName(std::wstring_view name);
void RestoreHiddenElements(std::vector<ChildVisibilityRecord>& records);
void HideCalendarClocksContent(wuxc::Grid const& section,
                               MountedDaylightInstance& instance);
void EnsureCalendarStructuralVisible(wuxc::Grid const& section);

void ApplyDaylightToInstance(MountedDaylightInstance& instance,
                             ForecastData const& forecast) {
    try {
        auto root = instance.daylightRoot.get();
        if (!root) {
            return;
        }

        // Shell can re-materialize additional-clock text after first hide.
        if (auto section = instance.calendarSection.get()) {
            EnsureCalendarStructuralVisible(section);
            HideCalendarClocksContent(section, instance);
        }

        auto primary = BrushOrFallback(
            root, L"TextFillColorPrimaryBrush",
            winrt::Windows::UI::Color{255, 240, 240, 240}, 1.0);
        auto secondary = BrushOrFallback(
            root, L"TextFillColorSecondaryBrush",
            winrt::Windows::UI::Color{255, 200, 200, 200}, 1.0);

        const bool ok = forecast.valid && forecast.hasDaylight;

        int nowMinute = 0;
        try {
            FILETIME nowLocal =
                GetForecastLocationNow(forecast.utcOffsetSeconds);
            SYSTEMTIME st{};
            FileTimeToSystemTime(&nowLocal, &st);
            nowMinute = static_cast<int>(st.wHour) * 60 + st.wMinute;
        } catch (...) {
            SYSTEMTIME local{};
            GetLocalTime(&local);
            nowMinute = static_cast<int>(local.wHour) * 60 + local.wMinute;
        }

        if (instance.ui.nowMarkerHost) {
            instance.ui.nowMarkerHost.Children().Clear();
            if (forecast.valid) {
                auto marker = MakeDaylightNowMarker(root, nowMinute);
                instance.ui.nowMarkerHost.Children().Append(marker);
            }
        }
        if (instance.ui.trackHost) {
            instance.ui.trackHost.Children().Clear();
            if (ok) {
                auto track = MakeTemperatureTrack(
                    root, static_cast<double>(forecast.todaySunriseMinute),
                    static_cast<double>(forecast.todaySunsetMinute), 0.0,
                    1440.0);
                track.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
                instance.ui.trackHost.Children().Append(track);
            }
        }
        if (instance.ui.sunriseText) {
            instance.ui.sunriseText.Foreground(secondary);
            instance.ui.sunriseText.Text(
                ok ? FormatClockHm(forecast.todaySunriseMinute) : L"--:--");
        }
        if (instance.ui.sunsetText) {
            instance.ui.sunsetText.Foreground(secondary);
            instance.ui.sunsetText.Text(
                ok ? FormatClockHm(forecast.todaySunsetMinute) : L"--:--");
        }

        const auto settings = GetSettingsCopy();
        const std::wstring& leftMode = settings.daylightHoursLeft;
        const bool showRemaining = leftMode == L"rightOf" ||
                                   leftMode == L"below" ||
                                   leftMode == L"insteadOf";
        const bool showTotal = leftMode != L"insteadOf";

        if (instance.ui.durationStack) {
            if (leftMode == L"rightOf") {
                instance.ui.durationStack.Orientation(
                    wuxc::Orientation::Horizontal);
                instance.ui.durationStack.Spacing(8);
            } else {
                instance.ui.durationStack.Orientation(
                    wuxc::Orientation::Vertical);
                instance.ui.durationStack.Spacing(1);
            }
        }

        if (instance.ui.durationText) {
            instance.ui.durationText.Foreground(primary);
            instance.ui.durationText.FontWeight(wut::FontWeights::SemiBold());
            instance.ui.durationText.FontSize(12);
            instance.ui.durationText.Text(
                ok ? FormatDaylightDuration(forecast.todaySunriseMinute,
                                            forecast.todaySunsetMinute)
                   : L"--");
            instance.ui.durationText.Visibility(
                showTotal ? wux::Visibility::Visible
                          : wux::Visibility::Collapsed);
        }

        if (instance.ui.remainingText) {
            const int remainingMins =
                ok ? DaylightMinutesRemaining(nowMinute,
                                              forecast.todaySunriseMinute,
                                              forecast.todaySunsetMinute)
                   : 0;
            // Match total weight when replacing it; quieter when paired.
            if (leftMode == L"insteadOf") {
                instance.ui.remainingText.Foreground(primary);
                instance.ui.remainingText.FontWeight(
                    wut::FontWeights::SemiBold());
                instance.ui.remainingText.FontSize(12);
            } else {
                instance.ui.remainingText.Foreground(secondary);
                instance.ui.remainingText.FontWeight(
                    wut::FontWeights::Normal());
                instance.ui.remainingText.FontSize(11);
            }
            instance.ui.remainingText.Text(
                ok ? FormatDurationMinutes(remainingMins) : L"--");
            instance.ui.remainingText.Visibility(
                showRemaining ? wux::Visibility::Visible
                              : wux::Visibility::Collapsed);
            Wh_Log(L"Daylight remaining=%s mode=%s nowMinute=%d",
                   ok ? FormatDurationMinutes(remainingMins).c_str() : L"--",
                   leftMode.c_str(), nowMinute);
        }
    } catch (...) {
        Wh_Log(L"ApplyDaylightToInstance error %08X", winrt::to_hresult());
    }
}

void RefreshDaylightOnFlyoutShown() {
    if (g_shuttingDown.load()) {
        return;
    }
    const ULONGLONG nowTick = GetTickCount64();
    const ULONGLONG last = g_lastDaylightFlyoutRefreshTick.load();
    if (last != 0 && nowTick - last < 1500ULL) {
        return;
    }
    g_lastDaylightFlyoutRefreshTick.store(nowTick);

    Wh_Log(L"Flyout shown — refreshing weather + daylight");

    // Hourly columns are sliced at fetch time. If the local hour (or refresh
    // interval) moved on while the flyout stayed mounted, pull a new forecast.
    // Daylight below still updates immediately from cache while that runs.
    try {
        RequestWeatherRefresh(false);
    } catch (...) {
    }

    ForecastData forecast;
    {
        std::lock_guard lock(g_forecastMutex);
        forecast = g_forecast;
    }

    // Prefer applying on this thread when we already are on the XAML UI thread
    // (Visibility / SizeChanged callbacks).
    try {
        const DWORD tid = GetCurrentThreadId();
        bool applied = false;
        {
            std::lock_guard lock(g_mountMutex);
            for (auto& mounted : g_daylightMounted) {
                if (mounted.uiThreadId == tid || mounted.uiThreadId == 0) {
                    ApplyDaylightToInstance(mounted, forecast);
                    applied = true;
                }
            }
        }
        if (!applied) {
            UpdateAllDaylightUIs();
        }
    } catch (...) {
        try {
            UpdateAllDaylightUIs();
        } catch (...) {
        }
    }
}

void RegisterVisibilityRefresh(wux::UIElement const& element,
                               int64_t& cookie) {
    if (!element || cookie >= 0) {
        return;
    }
    try {
        cookie = element.RegisterPropertyChangedCallback(
            wux::UIElement::VisibilityProperty(),
            wux::DependencyPropertyChangedCallback(
                [](wux::DependencyObject const& sender,
                   wux::DependencyProperty const&) {
                    if (g_shuttingDown.load()) {
                        return;
                    }
                    try {
                        auto ui = sender.try_as<wux::UIElement>();
                        if (!ui ||
                            ui.Visibility() != wux::Visibility::Visible) {
                            return;
                        }
                        RefreshDaylightOnFlyoutShown();
                    } catch (...) {
                    }
                }));
    } catch (...) {
        cookie = -1;
    }
}

void UnmountDaylightInstance(MountedDaylightInstance& instance) {
    try {
        if (instance.daylightTickTimer) {
            try {
                instance.daylightTickTimer.Stop();
            } catch (...) {
            }
            instance.daylightTickTimer = nullptr;
        }
        if (instance.visibilityCookie >= 0) {
            try {
                if (auto root = instance.daylightRoot.get()) {
                    root.UnregisterPropertyChangedCallback(
                        wux::UIElement::VisibilityProperty(),
                        instance.visibilityCookie);
                }
            } catch (...) {
            }
            instance.visibilityCookie = -1;
        }

        auto section = instance.calendarSection.get();
        auto root = instance.daylightRoot.get();
        auto clocksHost = instance.clocksHost.get();

        if (section && root) {
            try {
                uint32_t index = 0;
                if (section.Children().IndexOf(root, index)) {
                    section.Children().RemoveAt(index);
                }
            } catch (...) {
            }
        } else if (section) {
            // Fallback: remove by name if weak root expired.
            try {
                auto children = section.Children();
                for (int32_t i = static_cast<int32_t>(children.Size()) - 1;
                     i >= 0; --i) {
                    auto child = children.GetAt(static_cast<uint32_t>(i));
                    if (auto fe = child.try_as<wux::FrameworkElement>()) {
                        if (IsDaylightRootName(fe.Name())) {
                            children.RemoveAt(static_cast<uint32_t>(i));
                        }
                    }
                }
            } catch (...) {
            }
        }

        // Restore world-clock layout + visibility after removing our root.
        if (clocksHost && instance.clocksLayoutSaved) {
            try {
                clocksHost.Height(instance.clocksHeight);
                clocksHost.MinHeight(instance.clocksMinHeight);
                clocksHost.MaxHeight(instance.clocksMaxHeight);
                clocksHost.Margin(instance.clocksMargin);
            } catch (...) {
            }
        }
        RestoreHiddenElements(instance.hiddenClocks);

        // Undo Grid.Row surgery only when we inserted a RowDefinition.
        if (section && instance.insertedRowDefinition &&
            instance.insertedGridRow >= 0) {
            try {
                for (auto const& child : section.Children()) {
                    auto fe = child.try_as<wux::FrameworkElement>();
                    if (!fe) {
                        continue;
                    }
                    const int row = wuxc::Grid::GetRow(fe);
                    if (row > instance.insertedGridRow) {
                        wuxc::Grid::SetRow(fe, row - 1);
                    }
                }
                if (instance.insertedGridRow <
                    static_cast<int>(section.RowDefinitions().Size())) {
                    section.RowDefinitions().RemoveAt(
                        static_cast<uint32_t>(instance.insertedGridRow));
                }
            } catch (...) {
                Wh_Log(L"Daylight row restore failed %08X",
                       winrt::to_hresult());
            }
        }
    } catch (...) {
        Wh_Log(L"UnmountDaylightInstance error %08X", winrt::to_hresult());
    }
    instance.daylightRoot = nullptr;
    instance.calendarSection = nullptr;
    instance.clocksHost = nullptr;
    instance.ui = {};
    instance.dispatcherQueue = nullptr;
    instance.coreDispatcher = nullptr;
    instance.uiThreadId = 0;
    instance.insertedGridRow = -1;
    instance.insertedRowDefinition = false;
    instance.clocksLayoutSaved = false;
}

constexpr PCWSTR kCalendarLaunchHostName = L"WindhawkCalendarLaunchHost";
constexpr PCWSTR kCalendarLaunchButtonName = L"WindhawkCalendarLaunchButton";

// Default Win11 day chrome is circular (radius ≈ half the cell). Modest
// radius → rounded rectangles.
// NOTE: Do not replace CalendarViewDayItemStyle — a custom Style without
// BasedOn breaks the shell calendar flyout. Do not override DayItemMargin
// (theme uses a top-only offset; uniform values add an uneven bottom gap).
// Shrink markers with RenderTransform scale (does not affect layout).
// Day numbers share that transform — do not compensate via DayItemFontSize /
// nested text scales (those approaches broke the flyout).
constexpr double kCalendarDayItemCornerRadius = 6.0;
// Theme default is typically 1; a small bump thickens the selected stroke.
constexpr double kCalendarDaySelectedBorderThickness = 2;
constexpr double kCalendarDayItemVisualScale = 0.98;

struct CalendarDayShapePatch {
    winrt::weak_ref<wuxc::CalendarView> calendar;
    std::vector<SavedProperty> savedProperties;
    wuxc::CalendarView::CalendarViewDayItemChanging_revoker
        dayItemChangingRevoker{};
};

[[clang::no_destroy]] std::vector<CalendarDayShapePatch>
    g_calendarDayShapePatches;

wux::DependencyProperty ResolveCalendarViewProperty(PCWSTR propertyName,
                                                    bool brushProperty) {
    static std::mutex cacheMutex;
    static std::unordered_map<std::wstring, wux::DependencyProperty> cache;
    const std::wstring key =
        std::wstring(propertyName) + (brushProperty ? L"#b" : L"#v");
    {
        std::lock_guard lock(cacheMutex);
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
    }

    wux::DependencyProperty resolved{nullptr};
    try {
        std::wstring xaml =
            L"<Style xmlns='http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/"
            L"xaml' TargetType='CalendarView'><Setter Property='";
        xaml += propertyName;
        if (brushProperty) {
            // xmlns:x is required for {x:Null}; without it brush DPs never
            // resolve and highlight colors silently fail to apply.
            xaml += L"' Value='{x:Null}'/></Style>";
        } else {
            xaml += L"' Value='0'/></Style>";
        }
        auto style = wux::Markup::XamlReader::Load(xaml).as<wux::Style>();
        resolved = style.Setters().GetAt(0).as<wux::Setter>().Property();
    } catch (...) {
        Wh_Log(L"ResolveCalendarViewProperty(%s) failed %08X", propertyName,
               winrt::to_hresult());
    }
    if (resolved) {
        std::lock_guard lock(cacheMutex);
        cache.emplace(key, resolved);
    }
    return resolved;
}

wuxc::CalendarView FindCalendarViewInSubtree(wux::DependencyObject const& root,
                                             int maxDepth) {
    if (!root || maxDepth < 0) {
        return nullptr;
    }
    try {
        if (auto calendar = root.try_as<wuxc::CalendarView>()) {
            return calendar;
        }
        try {
            auto className = winrt::get_class_name(root);
            if (className == L"Windows.UI.Xaml.Controls.CalendarView") {
                return root.try_as<wuxc::CalendarView>();
            }
        } catch (...) {
        }
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; ++i) {
            if (auto found = FindCalendarViewInSubtree(
                    wuxm::VisualTreeHelper::GetChild(root, i), maxDepth - 1)) {
                return found;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

bool SetCalendarItemCornerRadius(wuxc::CalendarView const& calendar,
                                 wux::CornerRadius const& radius,
                                 std::vector<SavedProperty>* saveInto) {
    if (!calendar) {
        return false;
    }
    // Prefer DependencyProperty resolve so we compile against older WinRT
    // projections that omit CalendarItemCornerRadius accessors.
    try {
        if (auto prop = ResolveCalendarViewProperty(L"CalendarItemCornerRadius",
                                                    false)) {
            if (saveInto) {
                SavePropertyOnce(calendar, prop, *saveInto);
            }
            calendar.SetValue(prop, winrt::box_value(radius));
            return true;
        }
    } catch (...) {
        Wh_Log(L"SetCalendarItemCornerRadius failed %08X",
               winrt::to_hresult());
    }
    return false;
}

bool ApplyCalendarDaySelectedBorderThickness(
    wuxc::CalendarView const& calendar,
    std::vector<SavedProperty>* saveInto) {
    if (!calendar) {
        return false;
    }
    try {
        if (auto prop = ResolveCalendarViewProperty(
                L"CalendarItemBorderThickness", false)) {
            if (saveInto) {
                SavePropertyOnce(calendar, prop, *saveInto);
            }
            const double t = kCalendarDaySelectedBorderThickness;
            calendar.SetValue(prop,
                              winrt::box_value(wux::Thickness{t, t, t, t}));
            return true;
        }
    } catch (...) {
        Wh_Log(L"ApplyCalendarDaySelectedBorderThickness failed %08X",
               winrt::to_hresult());
    }
    return false;
}

void ApplyDayItemVisualScale(wux::UIElement const& element) {
    if (!element) {
        return;
    }
    try {
        element.RenderTransformOrigin(wf::Point{0.5f, 0.5f});
        wuxm::ScaleTransform scale;
        scale.ScaleX(kCalendarDayItemVisualScale);
        scale.ScaleY(kCalendarDayItemVisualScale);
        element.RenderTransform(scale);
    } catch (...) {
    }
}

void ClearDayItemVisualScale(wux::UIElement const& element) {
    if (!element) {
        return;
    }
    try {
        element.ClearValue(wux::UIElement::RenderTransformProperty());
        element.ClearValue(wux::UIElement::RenderTransformOriginProperty());
    } catch (...) {
    }
}

bool IsCalendarViewDayItem(wux::DependencyObject const& node) {
    if (!node) {
        return false;
    }
    try {
        if (node.try_as<wuxc::CalendarViewDayItem>()) {
            return true;
        }
    } catch (...) {
    }
    try {
        auto className = winrt::get_class_name(node);
        return className == L"Windows.UI.Xaml.Controls.CalendarViewDayItem";
    } catch (...) {
    }
    return false;
}

void ForEachCalendarViewDayItem(
    wux::DependencyObject const& root,
    int maxDepth,
    std::function<void(wux::UIElement const&)> const& fn) {
    if (!root || maxDepth < 0 || !fn) {
        return;
    }
    try {
        if (IsCalendarViewDayItem(root)) {
            if (auto el = root.try_as<wux::UIElement>()) {
                fn(el);
            }
        }
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; ++i) {
            ForEachCalendarViewDayItem(
                wuxm::VisualTreeHelper::GetChild(root, i), maxDepth - 1, fn);
        }
    } catch (...) {
    }
}

void ScaleExistingCalendarDayItems(wuxc::CalendarView const& calendar) {
    ForEachCalendarViewDayItem(calendar, 16, [](wux::UIElement const& el) {
        ApplyDayItemVisualScale(el);
    });
}

void ClearScaledCalendarDayItems(wuxc::CalendarView const& calendar) {
    ForEachCalendarViewDayItem(calendar, 16, [](wux::UIElement const& el) {
        ClearDayItemVisualScale(el);
    });
}

void EnsureCalendarDayItemVisualScaleHook(wuxc::CalendarView const& calendar,
                                          CalendarDayShapePatch& patch) {
    if (!calendar) {
        return;
    }
    ScaleExistingCalendarDayItems(calendar);
    if (patch.dayItemChangingRevoker) {
        return;
    }
    try {
        patch.dayItemChangingRevoker = calendar.CalendarViewDayItemChanging(
            winrt::auto_revoke,
            [](wuxc::CalendarView const&,
               wuxc::CalendarViewDayItemChangingEventArgs const& args) {
                if (g_shuttingDown.load()) {
                    return;
                }
                try {
                    if (args.InRecycleQueue()) {
                        return;
                    }
                    if (auto item = args.Item()) {
                        ApplyDayItemVisualScale(item);
                    }
                } catch (...) {
                }
            });
    } catch (...) {
        Wh_Log(L"CalendarViewDayItemChanging hook failed %08X",
               winrt::to_hresult());
    }
}

void ClearCalendarDayChromeOverrides(wuxc::CalendarView const& calendar) {
    if (!calendar) {
        return;
    }
    // Clear only properties this mod sets. Also clear DayItemMargin /
    // CalendarViewDayItemStyle leftovers from older broken builds.
    static constexpr struct {
        PCWSTR name;
        bool brush;
    } kProps[] = {
        {L"CalendarItemCornerRadius", false},
        {L"CalendarItemBorderThickness", false},
        {L"DayItemFontSize", false},  // clear leftovers from broken compensation
        {L"DayItemMargin", false},
        {L"CalendarViewDayItemStyle", true},
        {L"TodayBackground", true},
        {L"SelectedBorderBrush", true},
        {L"SelectedHoverBorderBrush", true},
        {L"SelectedPressedBorderBrush", true},
    };
    for (auto const& item : kProps) {
        try {
            if (auto prop =
                    ResolveCalendarViewProperty(item.name, item.brush)) {
                calendar.ClearValue(prop);
            }
        } catch (...) {
        }
    }
}

uint8_t BrushAlphaOrFallback(wuxm::Brush const& brush, uint8_t fallback) {
    if (!brush) {
        return fallback;
    }
    try {
        if (auto solid = brush.try_as<wuxm::SolidColorBrush>()) {
            auto color = solid.Color();
            if (color.A > 0) {
                return color.A;
            }
            // Transparent solid — opacity may live on the brush.
            double opacity = solid.Opacity();
            if (opacity > 0.0 && opacity < 1.0) {
                return static_cast<uint8_t>(
                    (std::max)(0.0, (std::min)(1.0, opacity)) * 255.0 + 0.5);
            }
            if (color.A == 0 && opacity >= 1.0) {
                return fallback;
            }
            return color.A ? color.A : fallback;
        }
    } catch (...) {
    }
    try {
        double opacity = brush.Opacity();
        if (opacity > 0.0 && opacity <= 1.0) {
            return static_cast<uint8_t>(opacity * 255.0 + 0.5);
        }
    } catch (...) {
    }
    return fallback;
}

wuxm::Brush MakeCalendarDayHighlightBrush(uint8_t alpha) {
    auto settings = GetSettingsCopy();
    return wuxm::SolidColorBrush(winrt::Windows::UI::Color{
        alpha, settings.calendarDayHighlightR, settings.calendarDayHighlightG,
        settings.calendarDayHighlightB});
}

bool ApplyCalendarDayBrushProperty(wuxc::CalendarView const& calendar,
                                   PCWSTR propertyName,
                                   uint8_t fallbackAlpha,
                                   std::vector<SavedProperty>* saveInto) {
    if (!calendar) {
        return false;
    }
    try {
        auto prop = ResolveCalendarViewProperty(propertyName, true);
        if (!prop) {
            return false;
        }
        if (saveInto) {
            SavePropertyOnce(calendar, prop, *saveInto);
        }
        uint8_t alpha = fallbackAlpha;
        try {
            if (auto current = calendar.GetValue(prop)) {
                if (auto brush = current.try_as<wuxm::Brush>()) {
                    alpha = BrushAlphaOrFallback(brush, fallbackAlpha);
                }
            }
        } catch (...) {
        }
        // If we already overwrote with our solid brush, prefer the saved
        // original alpha so settings changes don't stack.
        if (saveInto) {
            for (auto const& saved : *saveInto) {
                if (saved.property == prop && saved.value) {
                    if (auto brush = saved.value.try_as<wuxm::Brush>()) {
                        alpha = BrushAlphaOrFallback(brush, fallbackAlpha);
                    }
                    break;
                }
            }
        }
        calendar.SetValue(prop, MakeCalendarDayHighlightBrush(alpha));
        return true;
    } catch (...) {
        Wh_Log(L"ApplyCalendarDayBrushProperty(%s) failed %08X", propertyName,
               winrt::to_hresult());
    }
    return false;
}

void ApplyCalendarDayHighlightColors(wuxc::CalendarView const& calendar,
                                     std::vector<SavedProperty>* saveInto) {
    // Today fill + selected chrome share one RGB; each keeps its own alpha.
    static constexpr PCWSTR kBrushProps[] = {
        L"TodayBackground",
        L"SelectedBorderBrush",
        L"SelectedHoverBorderBrush",
        L"SelectedPressedBorderBrush",
    };
    for (auto propName : kBrushProps) {
        ApplyCalendarDayBrushProperty(calendar, propName, 255, saveInto);
    }
}

void ApplyCalendarDayRoundedRects(wuxc::CalendarView const& calendar) {
    if (!calendar || g_shuttingDown.load()) {
        return;
    }
    if (!GetSettingsCopy().roundedDayMarkers) {
        return;
    }

    const wux::CornerRadius rounded{kCalendarDayItemCornerRadius,
                                    kCalendarDayItemCornerRadius,
                                    kCalendarDayItemCornerRadius,
                                    kCalendarDayItemCornerRadius};

    auto applyChrome = [&](CalendarDayShapePatch& patch) {
        if (!SetCalendarItemCornerRadius(calendar, rounded,
                                         &patch.savedProperties)) {
            Wh_Log(L"Could not set CalendarItemCornerRadius");
        }
        if (!ApplyCalendarDaySelectedBorderThickness(calendar,
                                                     &patch.savedProperties)) {
            Wh_Log(L"Could not set CalendarItemBorderThickness");
        }
        // Clear leftovers from older builds that broke the flyout / layout.
        try {
            if (auto prop =
                    ResolveCalendarViewProperty(L"DayItemMargin", false)) {
                calendar.ClearValue(prop);
            }
        } catch (...) {
        }
        try {
            if (auto prop = ResolveCalendarViewProperty(
                    L"CalendarViewDayItemStyle", true)) {
                calendar.ClearValue(prop);
            }
        } catch (...) {
        }
        try {
            if (auto prop =
                    ResolveCalendarViewProperty(L"DayItemFontSize", false)) {
                calendar.ClearValue(prop);
            }
        } catch (...) {
        }
        ApplyCalendarDayHighlightColors(calendar, &patch.savedProperties);
    };

    {
        std::lock_guard lock(g_mountMutex);
        for (auto& patch : g_calendarDayShapePatches) {
            if (auto existing = patch.calendar.get()) {
                if (existing == calendar) {
                    applyChrome(patch);
                    EnsureCalendarDayItemVisualScaleHook(calendar, patch);
                    return;
                }
            }
        }
    }

    CalendarDayShapePatch patch;
    patch.calendar = winrt::make_weak(calendar);
    applyChrome(patch);
    EnsureCalendarDayItemVisualScaleHook(calendar, patch);

    {
        std::lock_guard lock(g_mountMutex);
        g_calendarDayShapePatches.push_back(std::move(patch));
    }
    Wh_Log(L"Calendar day chrome radius=%g border=%g scale=%g "
           L"highlight=#%02X%02X%02X",
           kCalendarDayItemCornerRadius, kCalendarDaySelectedBorderThickness,
           kCalendarDayItemVisualScale,
           GetSettingsCopy().calendarDayHighlightR,
           GetSettingsCopy().calendarDayHighlightG,
           GetSettingsCopy().calendarDayHighlightB);
}

void ScheduleCalendarDayShapeRetry(wuxc::Grid const& section, int attempt);

void ApplyCalendarDayShapeForSection(wuxc::Grid const& section, int attempt) {
    if (!section || g_shuttingDown.load()) {
        return;
    }
    if (!GetSettingsCopy().roundedDayMarkers) {
        return;
    }
    try {
        if (auto calendar = FindCalendarViewInSubtree(section, 14)) {
            ApplyCalendarDayRoundedRects(calendar);
            return;
        }
        // CalendarView sometimes lives beside CalendarSection under the same
        // CalendarCenterGrid parent.
        if (auto parent = wuxm::VisualTreeHelper::GetParent(section)) {
            if (auto calendar = FindCalendarViewInSubtree(parent, 10)) {
                ApplyCalendarDayRoundedRects(calendar);
                return;
            }
        }
        if (attempt < 8) {
            ScheduleCalendarDayShapeRetry(section, attempt + 1);
        }
    } catch (...) {
        Wh_Log(L"ApplyCalendarDayShapeForSection error %08X",
               winrt::to_hresult());
    }
}

void ScheduleCalendarDayShapeRetry(wuxc::Grid const& section, int attempt) {
    if (!section || g_shuttingDown.load()) {
        return;
    }
    try {
        if (auto dq = ws::DispatcherQueue::GetForCurrentThread()) {
            auto timer = dq.CreateTimer();
            timer.Interval(std::chrono::milliseconds{
                (std::min)(40 * attempt, 300)});
            timer.IsRepeating(false);
            winrt::weak_ref<wuxc::Grid> weakSection = winrt::make_weak(section);
            timer.Tick([weakSection, attempt,
                        timer](wf::IInspectable const&,
                               wf::IInspectable const&) {
                try {
                    timer.Stop();
                } catch (...) {
                }
                if (g_shuttingDown.load()) {
                    return;
                }
                if (auto section = weakSection.get()) {
                    ApplyCalendarDayShapeForSection(section, attempt);
                }
            });
            timer.Start();
            return;
        }
    } catch (...) {
    }
}

void RestoreAllCalendarDayShapes() {
    std::vector<CalendarDayShapePatch> patches;
    {
        std::lock_guard lock(g_mountMutex);
        patches.swap(g_calendarDayShapePatches);
    }

    size_t restored = 0;
    for (auto& patch : patches) {
        try {
            patch.dayItemChangingRevoker = {};
            if (auto calendar = patch.calendar.get()) {
                ClearScaledCalendarDayItems(calendar);
                RestoreLocalProperties(calendar, patch.savedProperties);
                ClearCalendarDayChromeOverrides(calendar);
                ++restored;
            }
        } catch (...) {
            Wh_Log(L"RestoreAllCalendarDayShapes patch error %08X",
                   winrt::to_hresult());
        }
    }
    Wh_Log(L"Restored calendar day chrome on %zu view(s)", restored);
}

// Best-effort clear for calendars near known roots (no mount-list locking
// during XAML walks — that path deadlocked / timed out into off-thread UI).
void ClearCalendarDayChromeNearRoots(
    std::vector<winrt::weak_ref<wuxc::Grid>> const& roots) {
    for (auto const& weakRoot : roots) {
        try {
            auto root = weakRoot.get();
            if (!root) {
                continue;
            }
            if (auto calendar = FindCalendarViewInSubtree(root, 14)) {
                ClearCalendarDayChromeOverrides(calendar);
            }
            if (auto center =
                    FindNamedGridNearby(root, L"CalendarCenterGrid")) {
                if (auto calendar = FindCalendarViewInSubtree(center, 14)) {
                    ClearCalendarDayChromeOverrides(calendar);
                }
            }
            if (auto parent = wuxm::VisualTreeHelper::GetParent(root)) {
                if (auto calendar = FindCalendarViewInSubtree(parent, 10)) {
                    ClearCalendarDayChromeOverrides(calendar);
                }
            }
        } catch (...) {
        }
    }
}

wux::FrameworkElement FindNamedFrameworkElement(
    wux::DependencyObject const& root,
    PCWSTR name,
    int maxDepth = 10) {
    if (!root || maxDepth < 0 || !name || !*name) {
        return nullptr;
    }
    try {
        if (auto fe = root.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == name) {
                return fe;
            }
        }
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; ++i) {
            if (auto found = FindNamedFrameworkElement(
                    wuxm::VisualTreeHelper::GetChild(root, i), name,
                    maxDepth - 1)) {
                return found;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

void ApplyAllCalendarDayShapes() {
    if (g_shuttingDown.load()) {
        return;
    }
    if (!GetSettingsCopy().roundedDayMarkers) {
        RestoreAllCalendarDayShapes();
        return;
    }
    std::vector<wuxc::Grid> sections;
    {
        std::lock_guard lock(g_mountMutex);
        for (auto const& weather : g_mounted) {
            auto grid = weather.notificationGrid.get();
            if (!grid) {
                continue;
            }
            try {
                if (auto calendar =
                        FindNamedGridNearby(grid, L"CalendarCenterGrid")) {
                    if (auto section = FindNamedFrameworkElement(
                                           calendar, L"CalendarSection", 8)
                                           .try_as<wuxc::Grid>()) {
                        sections.push_back(section);
                    }
                }
            } catch (...) {
            }
        }
        for (auto const& daylight : g_daylightMounted) {
            if (auto section = daylight.calendarSection.get()) {
                sections.push_back(section);
            }
        }
        for (auto const& launch : g_calendarLaunchMounted) {
            if (auto section = launch.calendarSection.get()) {
                sections.push_back(section);
            }
        }
    }
    for (auto const& section : sections) {
        ApplyCalendarDayShapeForSection(section);
    }
}

int32_t IndexOfPanelChild(wuxc::Panel const& panel,
                          wux::UIElement const& child) {
    if (!panel || !child) {
        return -1;
    }
    try {
        auto children = panel.Children();
        const uint32_t size = children.Size();
        for (uint32_t i = 0; i < size; ++i) {
            if (children.GetAt(i) == child) {
                return static_cast<int32_t>(i);
            }
        }
    } catch (...) {
    }
    return -1;
}

std::wstring TrimCalendarAppPath(std::wstring path) {
    while (!path.empty() &&
           (path.front() == L' ' || path.front() == L'\t')) {
        path.erase(path.begin());
    }
    while (!path.empty() && (path.back() == L' ' || path.back() == L'\t')) {
        path.pop_back();
    }
    if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"') {
        path = path.substr(1, path.size() - 2);
    }
    return path;
}

bool LooksLikeLaunchUri(std::wstring const& path) {
    if (path.empty()) {
        return false;
    }
    // Protocols like outlookcal: / ms-calendar: — not a filesystem path.
    if (path.find(L'\\') != std::wstring::npos ||
        path.find(L'/') != std::wstring::npos) {
        return path.find(L"://") != std::wstring::npos;
    }
    const auto colon = path.find(L':');
    return colon != std::wstring::npos && colon > 1;
}

std::wstring ParentDirectory(std::wstring const& path) {
    const auto pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos || pos == 0) {
        return {};
    }
    return path.substr(0, pos);
}

bool LaunchViaUriBroker(std::wstring const& uriText) {
    try {
        wf::Uri uri(uriText);
        auto op = ws::Launcher::LaunchUriAsync(uri);
        op.Completed([](wf::IAsyncOperation<bool> const& asyncOp,
                        wf::AsyncStatus status) {
            try {
                if (status != wf::AsyncStatus::Completed || !asyncOp.GetResults()) {
                    Wh_Log(L"Launcher.LaunchUriAsync did not succeed");
                } else {
                    Wh_Log(L"Launcher.LaunchUriAsync succeeded");
                }
            } catch (...) {
                Wh_Log(L"Launcher.LaunchUriAsync completion error %08X",
                       winrt::to_hresult());
            }
        });
        return true;
    } catch (...) {
        Wh_Log(L"Launcher.LaunchUriAsync threw %08X", winrt::to_hresult());
        return false;
    }
}

std::wstring ResolveCalendarAppPath() {
    std::wstring path;
    try {
        string_setting_unique_ptr raw(
            Wh_GetStringSetting(L"calendar.calendarAppPath"));
        if (raw && raw.get()[0]) {
            path = TrimCalendarAppPath(raw.get());
        }
    } catch (...) {
    }
    if (path.empty()) {
        path = TrimCalendarAppPath(GetSettingsCopy().calendarAppPath);
    }
    return path;
}

void ExplorerBrokerLaunchPath(std::wstring const& path) {
    if (path.empty() || g_shuttingDown.load()) {
        return;
    }
    Wh_Log(L"Explorer broker launching: %s", path.c_str());
    AllowSetForegroundWindow(ASFW_ANY);

    const std::wstring directory = ParentDirectory(path);
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_DDEWAIT;
    sei.lpVerb = L"open";
    sei.lpFile = path.c_str();
    sei.lpDirectory = directory.empty() ? nullptr : directory.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) {
        const DWORD err = GetLastError();
        const INT_PTR fallback = reinterpret_cast<INT_PTR>(ShellExecuteW(
            nullptr, L"open", path.c_str(), nullptr,
            directory.empty() ? nullptr : directory.c_str(), SW_SHOWNORMAL));
        Wh_Log(L"Explorer broker ShellExecuteEx failed (%lu), ShellExecute=%lld",
               err, static_cast<long long>(fallback));
        return;
    }
    Wh_Log(L"Explorer broker ShellExecuteEx succeeded");
}

LRESULT CALLBACK CalendarLaunchWndProc(HWND hwnd,
                                       UINT msg,
                                       WPARAM wParam,
                                       LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        auto* cds = reinterpret_cast<COPYDATASTRUCT*>(lParam);
        if (!cds || cds->dwData != kCalLaunchCopyDataId || !cds->lpData ||
            cds->cbData < sizeof(wchar_t)) {
            return FALSE;
        }
        const size_t chars = cds->cbData / sizeof(wchar_t);
        auto* text = static_cast<const wchar_t*>(cds->lpData);
        std::wstring path;
        if (chars > 0) {
            // Prefer null-terminated payload; otherwise take raw chars.
            size_t len = chars;
            if (text[chars - 1] == L'\0') {
                len = chars - 1;
            }
            path.assign(text, len);
        }
        path = TrimCalendarAppPath(path);
        // Synchronous on the broker thread — this is the path that worked.
        ExplorerBrokerLaunchPath(path);
        return TRUE;
    }
    if (msg == WM_DESTROY) {
        if (g_calLaunchHwnd == hwnd) {
            g_calLaunchHwnd = nullptr;
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void BrokerLaunchFromSharedMapping() {
    if (!g_calLaunchMapping) {
        return;
    }
    void* view = MapViewOfFile(g_calLaunchMapping, FILE_MAP_READ, 0, 0,
                               kCalLaunchMappingBytes);
    if (!view) {
        return;
    }
    auto* bytes = static_cast<const uint8_t*>(view);
    uint32_t charCount = 0;
    memcpy(&charCount, bytes, sizeof(charCount));
    std::wstring path;
    if (charCount > 0 &&
        charCount < (kCalLaunchMappingBytes / sizeof(wchar_t)) - 4) {
        path.assign(
            reinterpret_cast<const wchar_t*>(bytes + sizeof(uint32_t)),
            charCount);
    }
    UnmapViewOfFile(view);
    path = TrimCalendarAppPath(path);
    ExplorerBrokerLaunchPath(path);
}

// SECURITY_ATTRIBUTES that AppContainer (ShellExperienceHost) can open.
bool MakeAppContainerAllowSa(SECURITY_ATTRIBUTES& sa, PSECURITY_DESCRIPTOR& sd) {
    sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    sd = nullptr;
    // WD = Everyone, AC = ALL APPLICATION PACKAGES (AppContainer).
    // Required for SEH to open the named mapping/event fallback; WM_COPYDATA
    // is preferred and does not use these objects.
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:(A;;GA;;;WD)(A;;GA;;;AC)", SDDL_REVISION_1, &sd, nullptr) ||
        !sd) {
        Wh_Log(L"Calendar launch SDDL failed (%lu)", GetLastError());
        return false;
    }
    sa.lpSecurityDescriptor = sd;
    return true;
}

DWORD WINAPI CalendarLaunchBrokerThread(LPVOID) {
    HMODULE processModule = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = CalendarLaunchWndProc;
    wc.hInstance = processModule;
    wc.lpszClassName = kCalLaunchWindowClass;
    RegisterClassExW(&wc);

    // Hidden top-level window (not HWND_MESSAGE) so FindWindow works from SEH.
    g_calLaunchHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW, kCalLaunchWindowClass, kCalLaunchWindowTitle, WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, processModule, nullptr);
    if (!g_calLaunchHwnd) {
        Wh_Log(L"Calendar launch broker window failed (%lu)", GetLastError());
    } else {
        Wh_Log(L"Calendar launch broker window ready");
    }

    HANDLE waits[2] = {g_calLaunchEvent, g_calLaunchStopEvent};
    while (waits[0] && waits[1] && !g_shuttingDown.load()) {
        const DWORD waited =
            MsgWaitForMultipleObjects(2, waits, FALSE, 500, QS_ALLINPUT);
        if (waited == WAIT_FAILED) {
            break;
        }
        if (waited == WAIT_OBJECT_0 + 1) {
            break;  // stop event
        }
        if (waited == WAIT_OBJECT_0) {
            BrokerLaunchFromSharedMapping();
        }

        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_shuttingDown.store(true);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_calLaunchHwnd) {
        DestroyWindow(g_calLaunchHwnd);
        g_calLaunchHwnd = nullptr;
    }
    UnregisterClassW(kCalLaunchWindowClass, processModule);
    return 0;
}

bool StartCalendarLaunchBroker() {
    if (g_calLaunchThread) {
        return true;
    }

    PSECURITY_DESCRIPTOR sd = nullptr;
    SECURITY_ATTRIBUTES sa{};
    SECURITY_ATTRIBUTES* saPtr = nullptr;
    if (MakeAppContainerAllowSa(sa, sd)) {
        saPtr = &sa;
    }

    g_calLaunchMapping =
        CreateFileMappingW(INVALID_HANDLE_VALUE, saPtr, PAGE_READWRITE, 0,
                           kCalLaunchMappingBytes, kCalLaunchMappingName);
    if (!g_calLaunchMapping) {
        Wh_Log(L"CreateFileMapping for calendar launch failed (%lu)",
               GetLastError());
        if (sd) {
            LocalFree(sd);
        }
        return false;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        Wh_Log(L"Calendar launch mapping already existed (ok)");
    }

    g_calLaunchEvent = CreateEventW(saPtr, FALSE, FALSE, kCalLaunchEventName);
    if (!g_calLaunchEvent) {
        Wh_Log(L"CreateEvent for calendar launch failed (%lu)", GetLastError());
        CloseHandle(g_calLaunchMapping);
        g_calLaunchMapping = nullptr;
        if (sd) {
            LocalFree(sd);
        }
        return false;
    }
    if (sd) {
        LocalFree(sd);
        sd = nullptr;
    }

    g_calLaunchStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_calLaunchStopEvent) {
        CloseHandle(g_calLaunchEvent);
        CloseHandle(g_calLaunchMapping);
        g_calLaunchEvent = nullptr;
        g_calLaunchMapping = nullptr;
        return false;
    }
    g_calLaunchThread =
        CreateThread(nullptr, 0, CalendarLaunchBrokerThread, nullptr, 0, nullptr);
    if (!g_calLaunchThread) {
        Wh_Log(L"Calendar launch broker thread failed (%lu)", GetLastError());
        StopCalendarLaunchBroker();
        return false;
    }
    Wh_Log(L"Calendar launch explorer broker started");
    return true;
}

void StopCalendarLaunchBroker() {
    if (g_calLaunchStopEvent) {
        SetEvent(g_calLaunchStopEvent);
    }
    if (g_calLaunchHwnd && IsWindow(g_calLaunchHwnd)) {
        PostMessageW(g_calLaunchHwnd, WM_CLOSE, 0, 0);
    }
    if (g_calLaunchThread) {
        const DWORD waited = WaitForSingleObject(g_calLaunchThread, 3000);
        if (waited != WAIT_OBJECT_0) {
            Wh_Log(L"Calendar launch broker thread wait result %lu", waited);
        }
        CloseHandle(g_calLaunchThread);
        g_calLaunchThread = nullptr;
    }
    g_calLaunchHwnd = nullptr;
    if (g_calLaunchStopEvent) {
        CloseHandle(g_calLaunchStopEvent);
        g_calLaunchStopEvent = nullptr;
    }
    if (g_calLaunchEvent) {
        CloseHandle(g_calLaunchEvent);
        g_calLaunchEvent = nullptr;
    }
    if (g_calLaunchMapping) {
        CloseHandle(g_calLaunchMapping);
        g_calLaunchMapping = nullptr;
    }
}

bool RequestLaunchViaExplorerWindow(std::wstring const& path) {
    HWND hwnd = FindWindowW(kCalLaunchWindowClass, kCalLaunchWindowTitle);
    if (!hwnd) {
        Wh_Log(L"Explorer broker window not found — is the mod loaded in "
               L"explorer.exe?");
        return false;
    }

    std::wstring payload = path;
    COPYDATASTRUCT cds{};
    cds.dwData = kCalLaunchCopyDataId;
    cds.cbData = static_cast<DWORD>((payload.size() + 1) * sizeof(wchar_t));
    cds.lpData = payload.data();

    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(hwnd, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&cds),
                             SMTO_ABORTIFHUNG | SMTO_NORMAL, 4000, &result)) {
        Wh_Log(L"Explorer broker WM_COPYDATA failed (%lu)", GetLastError());
        return false;
    }
    Wh_Log(L"Explorer broker WM_COPYDATA delivered for %s", path.c_str());
    return true;
}

bool RequestLaunchViaExplorerMapping(std::wstring const& path) {
    HANDLE mapping =
        OpenFileMappingW(FILE_MAP_WRITE, FALSE, kCalLaunchMappingName);
    if (!mapping) {
        Wh_Log(L"Explorer broker mapping unavailable (%lu)", GetLastError());
        return false;
    }
    void* view = MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0,
                               kCalLaunchMappingBytes);
    if (!view) {
        Wh_Log(L"Explorer broker MapViewOfFile write failed (%lu)",
               GetLastError());
        CloseHandle(mapping);
        return false;
    }

    const uint32_t charCount =
        static_cast<uint32_t>((std::min)(path.size(), static_cast<size_t>(
            (kCalLaunchMappingBytes - sizeof(uint32_t)) / sizeof(wchar_t) - 1)));
    auto* bytes = static_cast<uint8_t*>(view);
    memcpy(bytes, &charCount, sizeof(charCount));
    if (charCount > 0) {
        memcpy(bytes + sizeof(uint32_t), path.data(),
               charCount * sizeof(wchar_t));
        reinterpret_cast<wchar_t*>(bytes + sizeof(uint32_t))[charCount] = 0;
    }
    UnmapViewOfFile(view);
    CloseHandle(mapping);

    HANDLE event = OpenEventW(EVENT_MODIFY_STATE, FALSE, kCalLaunchEventName);
    if (!event) {
        Wh_Log(L"Explorer broker event unavailable (%lu)", GetLastError());
        return false;
    }
    const BOOL signaled = SetEvent(event);
    CloseHandle(event);
    if (!signaled) {
        Wh_Log(L"Explorer broker SetEvent failed (%lu)", GetLastError());
        return false;
    }
    Wh_Log(L"Explorer broker mapping signaled for %s", path.c_str());
    return true;
}

bool RequestLaunchViaExplorerBroker(std::wstring const& path) {
    // Prefer WM_COPYDATA — FindWindow works from AppContainer (same as tray).
    if (RequestLaunchViaExplorerWindow(path)) {
        return true;
    }
    return RequestLaunchViaExplorerMapping(path);
}

void LaunchCalendarAppWithPath(std::wstring path) {
    if (g_shuttingDown.load() || path.empty()) {
        return;
    }

    Wh_Log(L"Launching calendar app: %s", path.c_str());
    AllowSetForegroundWindow(ASFW_ANY);

    if (LooksLikeLaunchUri(path)) {
        if (!LaunchViaUriBroker(path)) {
            Wh_Log(L"URI launch failed for %s", path.c_str());
        }
        return;
    }

    if (!RequestLaunchViaExplorerBroker(path)) {
        Wh_Log(L"Explorer broker launch failed for %s — ensure the mod is "
               L"loaded in explorer.exe",
               path.c_str());
    }
}

std::atomic<ULONGLONG> g_lastCalendarLaunchTick{0};

void RequestCalendarAppLaunch() {
    if (g_shuttingDown.load()) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG prev = g_lastCalendarLaunchTick.load();
    if (prev != 0 && now - prev < 500) {
        return;
    }
    g_lastCalendarLaunchTick.store(now);

    const std::wstring path = ResolveCalendarAppPath();
    if (path.empty()) {
        Wh_Log(L"Calendar app launch skipped — path empty");
        return;
    }

    Wh_Log(L"Calendar launch requested: %s", path.c_str());
    LaunchCalendarAppWithPath(path);
}

wuxc::Viewbox MakeCalendarAppIcon(double size, wuxm::Brush const& brush) {
    // Simple calendar page outline — reads cleanly at ~12px control size.
    wuxc::Canvas canvas;
    canvas.Width(16);
    canvas.Height(16);
    canvas.IsHitTestVisible(false);

    auto appendPath = [&](PCWSTR data, bool strokeOnly, double thickness) {
        try {
            std::wstring xaml =
                L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/"
                L"presentation' Data='";
            xaml += data;
            xaml += L"'/>";
            auto path =
                wux::Markup::XamlReader::Load(xaml).as<wuxs::Path>();
            if (strokeOnly) {
                path.Fill(nullptr);
                path.Stroke(brush);
                path.StrokeThickness(thickness);
                path.StrokeLineJoin(wuxm::PenLineJoin::Round);
            } else {
                path.Fill(brush);
            }
            canvas.Children().Append(path);
        } catch (...) {
        }
    };

    appendPath(L"M3,2.5 h10 a1.5,1.5 0 0 1 1.5,1.5 v10 a1.5,1.5 0 0 1 -1.5,"
               L"1.5 h-10 a1.5,1.5 0 0 1 -1.5,-1.5 v-10 a1.5,1.5 0 0 1 1.5,"
               L"-1.5 Z",
               true, 1.25);
    appendPath(L"M2,5.5 h12", true, 1.2);
    appendPath(L"M5.5,1.2 v2.2 M10.5,1.2 v2.2", true, 1.3);

    wuxc::Viewbox box;
    box.Width(size);
    box.Height(size);
    box.Stretch(wuxm::Stretch::Uniform);
    box.Child(canvas);
    box.IsHitTestVisible(false);
    return box;
}

constexpr double kCalendarLaunchGap = 13.0;
constexpr double kCalendarLaunchSizeFallback = 28.0;
constexpr double kCalendarLaunchStrokeThickness = 0.5;
constexpr double kCalendarLaunchStrokeOpacity = 0.06;     // idle
constexpr double kCalendarLaunchStrokeOpacityHover = 0.12;  // hover
constexpr double kCalendarLaunchStrokeOpacityPressed = 0.13;
constexpr uint8_t kCalendarLaunchFillHover = 0x15;
// Dark overlay shown on top of fill while pressed.
constexpr uint8_t kCalendarLaunchFillPressedDark = 0x3C;

double ResolveExpandButtonSide(wuxc::Button const& expand) {
    auto usable = [](double v) {
        return !std::isnan(v) && v >= 8.0 && v < 80.0;
    };
    try {
        const double aw = expand.ActualWidth();
        const double ah = expand.ActualHeight();
        if (usable(aw) && usable(ah)) {
            return (std::min)(aw, ah);
        }
        if (usable(ah)) {
            return ah;
        }
        if (usable(aw)) {
            return aw;
        }
    } catch (...) {
    }
    try {
        if (usable(expand.Height())) {
            return expand.Height();
        }
        if (usable(expand.Width())) {
            return expand.Width();
        }
        if (usable(expand.MinHeight())) {
            return expand.MinHeight();
        }
        if (usable(expand.MinWidth())) {
            return expand.MinWidth();
        }
    } catch (...) {
    }
    return kCalendarLaunchSizeFallback;
}

wuxm::Brush MakeCalendarLaunchStrokeBrush(double opacity) {
    // Bake alpha into the color — Opacity on the brush can look harsh/wrong
    // on shell acrylic surfaces.
    const uint8_t a = static_cast<uint8_t>(
        (std::max)(0.0, (std::min)(1.0, opacity)) * 255.0 + 0.5);
    return wuxm::SolidColorBrush(
        winrt::Windows::UI::Color{a, 255, 255, 255});
}

wuxm::Brush MakeCalendarLaunchFillBrush(uint8_t alpha) {
    return wuxm::SolidColorBrush(
        winrt::Windows::UI::Color{alpha, 255, 255, 255});
}

wuxm::Brush MakeCalendarLaunchDarkFillBrush(uint8_t alpha) {
    return wuxm::SolidColorBrush(
        winrt::Windows::UI::Color{alpha, 0, 0, 0});
}

wuxm::Brush MakeTransparentHitBrush() {
    // Real Transparent brush (not null) so the element still receives hits.
    return wuxm::SolidColorBrush(
        winrt::Windows::UI::Color{0, 255, 255, 255});
}

// Strip default Button chrome — our Content grid owns fill + stroke + icon.
wux::Style GetCalendarLaunchButtonStyle() {
    constexpr int kStyleVersion = 7;
    static wux::Style style{nullptr};
    static int loadedVersion = 0;
    if (loadedVersion == kStyleVersion && style) {
        return style;
    }
    loadedVersion = kStyleVersion;
    try {
        // Background=Transparent on the presenter is required for hit-testing;
        // a presenter with null background lets clicks fall through.
        style =
            wux::Markup::XamlReader::Load(
                L"<Style xmlns='http://schemas.microsoft.com/winfx/2006/xaml/"
                L"presentation' TargetType='Button'>"
                L"<Setter Property='Background' Value='Transparent'/>"
                L"<Setter Property='BorderThickness' Value='0'/>"
                L"<Setter Property='Padding' Value='0'/>"
                L"<Setter Property='Template'>"
                L"<Setter.Value>"
                L"<ControlTemplate TargetType='Button'>"
                L"<ContentPresenter Background='Transparent' "
                L"HorizontalAlignment='Stretch' "
                L"VerticalAlignment='Stretch' "
                L"Content='{TemplateBinding Content}' "
                L"IsHitTestVisible='True'/>"
                L"</ControlTemplate>"
                L"</Setter.Value>"
                L"</Setter>"
                L"</Style>")
                .as<wux::Style>();
        Wh_Log(L"Calendar launch button style loaded (v%d)", kStyleVersion);
    } catch (...) {
        style = nullptr;
        loadedVersion = 0;
        Wh_Log(L"Calendar launch style load failed %08X", winrt::to_hresult());
    }
    return style;
}

wuxc::Border FindNamedBorderInSubtree(wux::DependencyObject const& root,
                                       PCWSTR name,
                                       int maxDepth) {
    if (!root || maxDepth < 0 || !name || !*name) {
        return nullptr;
    }
    try {
        if (auto border = root.try_as<wuxc::Border>()) {
            if (auto fe = border.try_as<wux::FrameworkElement>()) {
                if (fe.Name() == name) {
                    return border;
                }
            }
        }
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; ++i) {
            if (auto found = FindNamedBorderInSubtree(
                    wuxm::VisualTreeHelper::GetChild(root, i), name,
                    maxDepth - 1)) {
                return found;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

bool ResolveCalendarLaunchChromeParts(wuxc::Button const& button,
                                      wuxc::Border& fill,
                                      wuxc::Border& stroke,
                                      wuxc::Border& pressed) {
    fill = nullptr;
    stroke = nullptr;
    pressed = nullptr;
    if (!button) {
        return false;
    }
    try {
        // Chrome lives in Content (not the ControlTemplate).
        if (auto content = button.Content().try_as<wux::DependencyObject>()) {
            fill = FindNamedBorderInSubtree(content, L"WindhawkCalLaunchFill",
                                              4);
            stroke = FindNamedBorderInSubtree(
                content, L"WindhawkCalLaunchStroke", 4);
            pressed = FindNamedBorderInSubtree(
                content, L"WindhawkCalLaunchPressed", 4);
        }
    } catch (...) {
    }
    return fill != nullptr && stroke != nullptr && pressed != nullptr;
}

bool ResolveCalendarLaunchChromeParts(wuxc::Button const& button,
                                      wuxc::Border& fill,
                                      wuxc::Border& stroke) {
    wuxc::Border pressed{nullptr};
    return ResolveCalendarLaunchChromeParts(button, fill, stroke, pressed);
}

// Back-compat name used by older call sites.
bool ResolveCalendarLaunchTemplateParts(wuxc::Button const& button,
                                        wuxc::Border& fill,
                                        wuxc::Border& stroke) {
    return ResolveCalendarLaunchChromeParts(button, fill, stroke);
}

enum class CalendarLaunchVisualState { Normal, Hover, Pressed };

void ApplyCalendarLaunchVisualState(wuxc::Button const& button,
                                    wuxc::Border const& fill,
                                    wuxc::Border const& stroke,
                                    wuxc::Border const& pressed,
                                    CalendarLaunchVisualState state,
                                    wuxm::Brush const& idleBackground) {
    if (!fill || !stroke) {
        return;
    }
    try {
        switch (state) {
            case CalendarLaunchVisualState::Hover:
                fill.Background(
                    MakeCalendarLaunchFillBrush(kCalendarLaunchFillHover));
                stroke.BorderBrush(MakeCalendarLaunchStrokeBrush(
                    kCalendarLaunchStrokeOpacityHover));
                if (pressed) {
                    pressed.Visibility(wux::Visibility::Collapsed);
                }
                break;
            case CalendarLaunchVisualState::Pressed:
                // Keep hover/idle fill underneath; show dark overlay on top.
                fill.Background(
                    MakeCalendarLaunchFillBrush(kCalendarLaunchFillHover));
                stroke.BorderBrush(MakeCalendarLaunchStrokeBrush(
                    kCalendarLaunchStrokeOpacityPressed));
                if (pressed) {
                    pressed.Background(MakeCalendarLaunchDarkFillBrush(
                        kCalendarLaunchFillPressedDark));
                    pressed.Visibility(wux::Visibility::Visible);
                }
                break;
            case CalendarLaunchVisualState::Normal:
            default:
                fill.Background(idleBackground ? idleBackground
                                               : MakeCalendarLaunchFillBrush(0x10));
                stroke.BorderBrush(MakeCalendarLaunchStrokeBrush(
                    kCalendarLaunchStrokeOpacity));
                if (pressed) {
                    pressed.Visibility(wux::Visibility::Collapsed);
                }
                break;
        }
        stroke.BorderThickness(wux::Thickness{
            kCalendarLaunchStrokeThickness, kCalendarLaunchStrokeThickness,
            kCalendarLaunchStrokeThickness, kCalendarLaunchStrokeThickness});
        stroke.Visibility(wux::Visibility::Visible);
        wuxc::Canvas::SetZIndex(fill, 0);
        if (pressed) {
            wuxc::Canvas::SetZIndex(pressed, 1);
        }
        wuxc::Canvas::SetZIndex(stroke, 2);
    } catch (...) {
    }
    (void)button;
}

void BuildCalendarLaunchContentChrome(wuxc::Button const& expandButton,
                                      double side,
                                      wuxm::Brush const& idleBackground,
                                      wux::CornerRadius cornerRadius,
                                      wuxc::Border& outFill,
                                      wuxc::Border& outStroke,
                                      wuxc::Border& outPressed,
                                      wuxc::Grid& outRoot) {
    outFill = nullptr;
    outStroke = nullptr;
    outPressed = nullptr;
    outRoot = nullptr;

    wuxc::Grid root;
    root.Name(L"WindhawkCalLaunchChrome");
    root.Width(side);
    root.Height(side);
    // Transparent brush (not null) so the control receives pointer input.
    root.Background(MakeTransparentHitBrush());
    root.IsHitTestVisible(true);

    wuxc::Border fill;
    fill.Name(L"WindhawkCalLaunchFill");
    fill.Background(idleBackground ? idleBackground
                                   : MakeCalendarLaunchFillBrush(0x10));
    fill.CornerRadius(cornerRadius);
    fill.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    fill.VerticalAlignment(wux::VerticalAlignment::Stretch);
    // Fill is the hit target; overlays/icon sit above and ignore hits.
    fill.IsHitTestVisible(true);

    wuxc::Border pressed;
    pressed.Name(L"WindhawkCalLaunchPressed");
    pressed.Background(
        MakeCalendarLaunchDarkFillBrush(kCalendarLaunchFillPressedDark));
    pressed.CornerRadius(cornerRadius);
    pressed.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    pressed.VerticalAlignment(wux::VerticalAlignment::Stretch);
    pressed.Visibility(wux::Visibility::Collapsed);
    pressed.IsHitTestVisible(false);

    wuxc::Border stroke;
    stroke.Name(L"WindhawkCalLaunchStroke");
    stroke.Background(MakeTransparentHitBrush());
    stroke.BorderBrush(MakeCalendarLaunchStrokeBrush(kCalendarLaunchStrokeOpacity));
    stroke.BorderThickness(wux::Thickness{
        kCalendarLaunchStrokeThickness, kCalendarLaunchStrokeThickness,
        kCalendarLaunchStrokeThickness, kCalendarLaunchStrokeThickness});
    stroke.CornerRadius(cornerRadius);
    stroke.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    stroke.VerticalAlignment(wux::VerticalAlignment::Stretch);
    stroke.IsHitTestVisible(false);

    auto iconBrush = BrushOrFallback(
        expandButton, L"TextFillColorPrimaryBrush",
        winrt::Windows::UI::Color{255, 240, 240, 240}, 1.0);
    auto icon = MakeCalendarAppIcon((std::max)(10.0, side * 0.45), iconBrush);
    icon.HorizontalAlignment(wux::HorizontalAlignment::Center);
    icon.VerticalAlignment(wux::VerticalAlignment::Center);
    icon.IsHitTestVisible(false);

    wuxc::Canvas::SetZIndex(fill, 0);
    wuxc::Canvas::SetZIndex(pressed, 1);
    wuxc::Canvas::SetZIndex(stroke, 2);
    wuxc::Canvas::SetZIndex(icon, 3);

    root.Children().Append(fill);
    root.Children().Append(pressed);
    root.Children().Append(stroke);
    root.Children().Append(icon);

    outFill = fill;
    outStroke = stroke;
    outPressed = pressed;
    outRoot = root;
}

void ApplyCalendarLaunchButtonChrome(wuxc::Button const& button,
                                     wuxc::Button const& expandButton,
                                     wuxc::Border& outFill,
                                     wuxc::Border& outStroke,
                                     wuxc::Border& outPressed) {
    outFill = nullptr;
    outStroke = nullptr;
    outPressed = nullptr;
    if (!button || !expandButton) {
        return;
    }

    const double side = ResolveExpandButtonSide(expandButton);
    try {
        button.RequestedTheme(expandButton.RequestedTheme());
    } catch (...) {
    }

    button.Width(side);
    button.Height(side);
    button.MinWidth(side);
    button.MinHeight(side);
    button.MaxWidth(side);
    button.MaxHeight(side);
    button.Padding(wux::Thickness{0, 0, 0, 0});
    button.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
    button.VerticalContentAlignment(wux::VerticalAlignment::Stretch);
    button.IsHitTestVisible(true);
    button.IsTabStop(true);
    button.IsEnabled(true);
    try {
        button.ClickMode(wuxc::ClickMode::Release);
    } catch (...) {
    }

    wuxm::Brush background{nullptr};
    try {
        background = expandButton.Background();
    } catch (...) {
    }
    if (!background) {
        background = BrushOrFallback(
            expandButton, L"SubtleFillColorSecondaryBrush",
            winrt::Windows::UI::Color{255, 255, 255, 255}, 0.06);
    }
    if (!background) {
        background = MakeCalendarLaunchFillBrush(0x10);
    }

    if (auto style = GetCalendarLaunchButtonStyle()) {
        button.Style(style);
    }
    // Button itself stays transparent — chrome is entirely in Content.
    button.Background(MakeTransparentHitBrush());
    button.BorderThickness(wux::Thickness{0, 0, 0, 0});
    button.BorderBrush(nullptr);

    wux::CornerRadius corner{4, 4, 4, 4};
    try {
        corner = expandButton.CornerRadius();
    } catch (...) {
    }

    wuxc::Grid chromeRoot{nullptr};
    BuildCalendarLaunchContentChrome(expandButton, side, background, corner,
                                     outFill, outStroke, outPressed, chromeRoot);
    button.Content(chromeRoot);

    ApplyCalendarLaunchVisualState(button, outFill, outStroke, outPressed,
                                   CalendarLaunchVisualState::Normal,
                                   background);
    Wh_Log(L"Calendar launch content chrome applied (fill=%d stroke=%d "
           L"pressed=%d)",
           outFill ? 1 : 0, outStroke ? 1 : 0, outPressed ? 1 : 0);
}

void ApplyCalendarLaunchButtonChrome(wuxc::Button const& button,
                                     wuxc::Button const& expandButton,
                                     wuxc::Border& outFill,
                                     wuxc::Border& outStroke) {
    wuxc::Border pressed{nullptr};
    ApplyCalendarLaunchButtonChrome(button, expandButton, outFill, outStroke,
                                    pressed);
}

void ApplyCalendarLaunchButtonChrome(wuxc::Button const& button,
                                     wuxc::Button const& expandButton) {
    wuxc::Border fill{nullptr};
    wuxc::Border stroke{nullptr};
    ApplyCalendarLaunchButtonChrome(button, expandButton, fill, stroke);
}

wuxc::Button BuildCalendarLaunchButton(wuxc::Button const& expandButton,
                                       wuxc::Border& outHoverFill) {
    outHoverFill = nullptr;
    wuxc::Button button;
    button.Name(kCalendarLaunchButtonName);
    try {
        wuxa::AutomationProperties::SetName(button, L"Open calendar app");
    } catch (...) {
    }
    wuxc::Border fill{nullptr};
    wuxc::Border stroke{nullptr};
    ApplyCalendarLaunchButtonChrome(button, expandButton, fill, stroke);
    outHoverFill = fill;
    return button;
}

void WireCalendarLaunchClick(
    wuxc::Button const& button,
    wuxc::Border const& templateFill,
    wuxc::Border const& templateStroke,
    wuxc::Border const& templatePressed,
    wuxm::Brush const& idleBackground,
    wuxc::Button::Click_revoker& clickRevoker,
    wux::UIElement::PointerEntered_revoker& pointerEnteredRevoker,
    wux::UIElement::PointerPressed_revoker& pointerPressedRevoker,
    wux::UIElement::PointerReleased_revoker& pointerReleasedRevoker,
    wux::UIElement::PointerExited_revoker& pointerExitedRevoker,
    wux::UIElement::Tapped_revoker& tappedRevoker) {
    clickRevoker = {};
    pointerEnteredRevoker = {};
    pointerPressedRevoker = {};
    pointerReleasedRevoker = {};
    pointerExitedRevoker = {};
    tappedRevoker = {};
    if (!button) {
        return;
    }

    wuxc::Border fill = templateFill;
    wuxc::Border stroke = templateStroke;
    wuxc::Border pressed = templatePressed;
    if (!fill || !stroke || !pressed) {
        ResolveCalendarLaunchChromeParts(button, fill, stroke, pressed);
    }

    auto activate = [](PCWSTR via) {
        try {
            Wh_Log(L"Calendar launch button activated (%s)", via);
            RequestCalendarAppLaunch();
        } catch (...) {
            Wh_Log(L"RequestCalendarAppLaunch exception %08X",
                   winrt::to_hresult());
        }
    };

    winrt::weak_ref<wuxc::Button> weakButton = winrt::make_weak(button);
    winrt::weak_ref<wuxc::Border> weakFill = winrt::make_weak(fill);
    winrt::weak_ref<wuxc::Border> weakStroke = winrt::make_weak(stroke);
    winrt::weak_ref<wuxc::Border> weakPressed = winrt::make_weak(pressed);
    wuxm::Brush idle = idleBackground;
    if (!idle && fill) {
        try {
            idle = fill.Background();
        } catch (...) {
        }
    }
    // Prevent CapturePointer leave/enter from overwriting Pressed with Hover.
    auto pointerDown = std::make_shared<bool>(false);

    auto applyState = [weakButton, weakFill, weakStroke, weakPressed, idle](
                          CalendarLaunchVisualState state) {
        try {
            auto btn = weakButton.get();
            auto f = weakFill.get();
            auto s = weakStroke.get();
            auto p = weakPressed.get();
            if (!btn) {
                return;
            }
            if (!f || !s || !p) {
                ResolveCalendarLaunchChromeParts(btn, f, s, p);
            }
            if (!f || !s) {
                return;
            }
            ApplyCalendarLaunchVisualState(btn, f, s, p, state, idle);
        } catch (...) {
        }
    };

    auto endPress = [weakButton, applyState, pointerDown](bool assumeHover) {
        *pointerDown = false;
        try {
            if (auto btn = weakButton.get()) {
                btn.ReleasePointerCaptures();
            }
        } catch (...) {
        }
        applyState(assumeHover ? CalendarLaunchVisualState::Hover
                               : CalendarLaunchVisualState::Normal);
    };

    clickRevoker = button.Click(
        winrt::auto_revoke,
        [activate, endPress](wf::IInspectable const&,
                             wux::RoutedEventArgs const&) {
            endPress(true);
            activate(L"Click");
        });

    // Press on fill (hit target). Release/capture-lost must be on the button
    // because CapturePointer routes those events to the capture owner — wiring
    // release only on fill left the pressed overlay stuck after click.
    wux::UIElement pressTarget =
        fill ? fill.as<wux::UIElement>() : button.as<wux::UIElement>();

    pointerEnteredRevoker = button.PointerEntered(
        winrt::auto_revoke,
        [applyState, pointerDown](wf::IInspectable const&,
                                  wux::Input::PointerRoutedEventArgs const&) {
            if (*pointerDown) {
                return;
            }
            applyState(CalendarLaunchVisualState::Hover);
        });

    pointerExitedRevoker = button.PointerExited(
        winrt::auto_revoke,
        [applyState, pointerDown](wf::IInspectable const&,
                                  wux::Input::PointerRoutedEventArgs const&) {
            if (*pointerDown) {
                // Capture can synthesize Exited — keep pressed look until
                // release / capture lost.
                return;
            }
            applyState(CalendarLaunchVisualState::Normal);
        });

    pointerPressedRevoker = pressTarget.PointerPressed(
        winrt::auto_revoke,
        [weakButton, applyState, pointerDown](
            wf::IInspectable const&,
            wux::Input::PointerRoutedEventArgs const& args) {
            *pointerDown = true;
            applyState(CalendarLaunchVisualState::Pressed);
            try {
                if (auto btn = weakButton.get()) {
                    btn.CapturePointer(args.Pointer());
                }
            } catch (...) {
            }
        });

    pointerReleasedRevoker = button.PointerReleased(
        winrt::auto_revoke,
        [endPress](wf::IInspectable const&,
                   wux::Input::PointerRoutedEventArgs const&) {
            endPress(true);
        });

    tappedRevoker = button.Tapped(
        winrt::auto_revoke,
        [activate, endPress](wf::IInspectable const&,
                             wux::Input::TappedRoutedEventArgs const& args) {
            try {
                args.Handled(true);
            } catch (...) {
            }
            endPress(true);
            activate(L"Tapped");
        });
}

void RemoveUiElementFromParent(wux::UIElement const& element) {
    if (!element) {
        return;
    }
    try {
        auto parent =
            wuxm::VisualTreeHelper::GetParent(element).try_as<wuxc::Panel>();
        if (!parent) {
            return;
        }
        const int32_t index = IndexOfPanelChild(parent, element);
        if (index >= 0) {
            parent.Children().RemoveAt(static_cast<uint32_t>(index));
        }
    } catch (...) {
    }
}

void CollectNamedFrameworkElements(wux::DependencyObject const& root,
                                   PCWSTR name,
                                   std::vector<wux::FrameworkElement>& out,
                                   int maxDepth) {
    if (!root || maxDepth < 0 || !name || !*name) {
        return;
    }
    try {
        if (auto fe = root.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == name) {
                out.push_back(fe);
            }
        }
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; ++i) {
            CollectNamedFrameworkElements(
                wuxm::VisualTreeHelper::GetChild(root, i), name, out,
                maxDepth - 1);
        }
    } catch (...) {
    }
}

// Remount/retry paths used to leave multiple launch buttons in the same cell
// (looks like a thick/ghosted control). Always purge extras.
void RemoveCalendarLaunchButtons(wux::DependencyObject const& root,
                                 wux::UIElement const& keep = nullptr) {
    std::vector<wux::FrameworkElement> found;
    CollectNamedFrameworkElements(root, kCalendarLaunchButtonName, found, 12);
    for (auto const& fe : found) {
        try {
            if (keep && fe == keep) {
                continue;
            }
            RemoveUiElementFromParent(fe);
        } catch (...) {
        }
    }
}

// Unwrap WindhawkCalendarLaunchHost and restore ExpandCollapseButton to its
// original parent. ONLY call when disabling the feature, unloading, or when a
// host is broken. Remount must never unwrap a healthy host — that is what
// made both buttons vanish previously.
void UnwrapCalendarLaunchHost(wuxc::Grid const& section) {
    try {
        auto hostFe =
            FindNamedFrameworkElement(section, kCalendarLaunchHostName, 10);
        if (!hostFe) {
            return;
        }
        auto host = hostFe.try_as<wuxc::Panel>();
        auto parent =
            wuxm::VisualTreeHelper::GetParent(hostFe).try_as<wuxc::Panel>();
        if (!host || !parent) {
            return;
        }

        wuxc::Button expand{nullptr};
        auto children = host.Children();
        for (uint32_t i = 0; i < children.Size(); ++i) {
            if (auto btn = children.GetAt(i).try_as<wuxc::Button>()) {
                if (btn.Name() == L"ExpandCollapseButton") {
                    expand = btn;
                    children.RemoveAt(i);
                    break;
                }
            }
        }

        const int32_t hostIndex = IndexOfPanelChild(parent, hostFe);
        wux::Thickness hostMargin{};
        wux::HorizontalAlignment hostHAlign = wux::HorizontalAlignment::Right;
        wux::VerticalAlignment hostVAlign = wux::VerticalAlignment::Top;
        int row = 0, col = 0, rowSpan = 1, colSpan = 1;
        try {
            hostMargin = hostFe.Margin();
            hostHAlign = hostFe.HorizontalAlignment();
            hostVAlign = hostFe.VerticalAlignment();
            row = wuxc::Grid::GetRow(hostFe);
            col = wuxc::Grid::GetColumn(hostFe);
            rowSpan = wuxc::Grid::GetRowSpan(hostFe);
            colSpan = wuxc::Grid::GetColumnSpan(hostFe);
        } catch (...) {
        }

        if (hostIndex >= 0) {
            parent.Children().RemoveAt(static_cast<uint32_t>(hostIndex));
        }
        if (expand) {
            try {
                expand.RenderTransform(nullptr);
            } catch (...) {
            }
            expand.Margin(hostMargin);
            expand.HorizontalAlignment(hostHAlign);
            expand.VerticalAlignment(hostVAlign);
            wuxc::Grid::SetRow(expand, row);
            wuxc::Grid::SetColumn(expand, col);
            wuxc::Grid::SetRowSpan(expand, rowSpan);
            wuxc::Grid::SetColumnSpan(expand, colSpan);
            uint32_t insertAt = parent.Children().Size();
            if (hostIndex >= 0) {
                insertAt = static_cast<uint32_t>(hostIndex);
            }
            parent.Children().InsertAt(insertAt, expand);
            Wh_Log(L"Restored ExpandCollapseButton from launch host");
        }
    } catch (...) {
        Wh_Log(L"UnwrapCalendarLaunchHost error %08X", winrt::to_hresult());
    }
}

void ClearExpandLaunchTransform(wuxc::Button const& expand) {
    if (!expand) {
        return;
    }
    try {
        expand.RenderTransform(nullptr);
    } catch (...) {
    }
}

// Never clear ExpandCollapseButton.RenderTransform here — the shell uses it
// during minimize/expand animation. Clearing mid-animation causes jitter.
void ConfigureLaunchInHost(wuxc::Button const& launch,
                           wuxc::Button const& expand,
                           wuxc::Border& outFill,
                           wuxc::Border& outStroke,
                           wuxc::Border& outPressed) {
    outFill = nullptr;
    outStroke = nullptr;
    outPressed = nullptr;
    if (!launch || !expand) {
        return;
    }
    try {
        launch.RenderTransform(nullptr);
        ApplyCalendarLaunchButtonChrome(launch, expand, outFill, outStroke,
                                        outPressed);
        launch.HorizontalAlignment(wux::HorizontalAlignment::Center);
        launch.VerticalAlignment(wux::VerticalAlignment::Center);
        expand.VerticalAlignment(wux::VerticalAlignment::Center);
        // Gap between [launch][chevron] inside the horizontal StackPanel.
        launch.Margin(wux::Thickness{0, 0, kCalendarLaunchGap, 0});
    } catch (...) {
    }
}

void ConfigureLaunchInHost(wuxc::Button const& launch,
                           wuxc::Button const& expand,
                           wuxc::Border& outFill,
                           wuxc::Border& outStroke) {
    wuxc::Border pressed{nullptr};
    ConfigureLaunchInHost(launch, expand, outFill, outStroke, pressed);
}

void ConfigureLaunchInHost(wuxc::Button const& launch,
                           wuxc::Button const& expand) {
    wuxc::Border fill{nullptr};
    wuxc::Border stroke{nullptr};
    wuxc::Border pressed{nullptr};
    ConfigureLaunchInHost(launch, expand, fill, stroke, pressed);
}

void WireCalendarLaunchForInstance(wuxc::Button const& launch,
                                   MountedCalendarLaunchInstance& instance,
                                   wuxc::Border fill = nullptr,
                                   wuxc::Border stroke = nullptr,
                                   wuxc::Border pressed = nullptr) {
    if (!launch) {
        return;
    }
    if (!fill || !stroke || !pressed) {
        ResolveCalendarLaunchChromeParts(launch, fill, stroke, pressed);
    }
    instance.templateFill = winrt::make_weak(fill);
    instance.templateStroke = winrt::make_weak(stroke);
    instance.templatePressed = winrt::make_weak(pressed);
    instance.hoverFill = winrt::make_weak(fill);
    wuxm::Brush idle{nullptr};
    try {
        if (fill) {
            idle = fill.Background();
        }
    } catch (...) {
    }
    if (!idle) {
        idle = MakeCalendarLaunchFillBrush(0x10);
    }
    if (!fill || !stroke || !pressed) {
        Wh_Log(L"Calendar launch wire: missing chrome parts (fill=%d stroke=%d "
               L"pressed=%d)",
               fill ? 1 : 0, stroke ? 1 : 0, pressed ? 1 : 0);
    }
    WireCalendarLaunchClick(
        launch, fill, stroke, pressed, idle, instance.clickRevoker,
        instance.pointerEnteredRevoker, instance.pointerPressedRevoker,
        instance.pointerReleasedRevoker, instance.pointerExitedRevoker,
        instance.tappedRevoker);
}

bool TryGetHealthyLaunchHost(wuxc::Grid const& section,
                             wuxc::StackPanel& outHost,
                             wuxc::Button& outLaunch,
                             wuxc::Button& outExpand) {
    outHost = nullptr;
    outLaunch = nullptr;
    outExpand = nullptr;
    try {
        auto hostFe =
            FindNamedFrameworkElement(section, kCalendarLaunchHostName, 10);
        if (!hostFe) {
            return false;
        }
        auto host = hostFe.try_as<wuxc::StackPanel>();
        if (!host || host.Orientation() != wuxc::Orientation::Horizontal) {
            return false;
        }
        wuxc::Button launch{nullptr};
        wuxc::Button expand{nullptr};
        auto children = host.Children();
        for (uint32_t i = 0; i < children.Size(); ++i) {
            if (auto btn = children.GetAt(i).try_as<wuxc::Button>()) {
                if (btn.Name() == kCalendarLaunchButtonName) {
                    launch = btn;
                } else if (btn.Name() == L"ExpandCollapseButton") {
                    expand = btn;
                }
            }
        }
        if (!launch || !expand) {
            return false;
        }
        outHost = host;
        outLaunch = launch;
        outExpand = expand;
        return true;
    } catch (...) {
        return false;
    }
}

void UnmountCalendarLaunchInstance(MountedCalendarLaunchInstance& instance) {
    try {
        instance.clickRevoker = {};
        instance.pointerEnteredRevoker = {};
        instance.pointerPressedRevoker = {};
        instance.pointerReleasedRevoker = {};
        instance.pointerExitedRevoker = {};
        instance.tappedRevoker = {};

        if (auto expand = instance.expandButton.get()) {
            ClearExpandLaunchTransform(expand);
        }
        if (auto section = instance.calendarSection.get()) {
            // Unwrap restores the chevron; host (and launch) are removed.
            UnwrapCalendarLaunchHost(section);
            RemoveCalendarLaunchButtons(section);
        } else if (auto launch = instance.launchButton.get()) {
            RemoveUiElementFromParent(launch);
        }
    } catch (...) {
        Wh_Log(L"UnmountCalendarLaunchInstance error %08X",
               winrt::to_hresult());
    }
    instance.host = nullptr;
    instance.parent = nullptr;
    instance.launchButton = nullptr;
    instance.expandButton = nullptr;
    instance.hoverFill = nullptr;
    instance.templateFill = nullptr;
    instance.templateStroke = nullptr;
    instance.templatePressed = nullptr;
    instance.calendarSection = nullptr;
    instance.dispatcherQueue = nullptr;
    instance.coreDispatcher = nullptr;
}

// Reentrancy guard: wrapping ExpandCollapseButton emits Add/Remove that must
// not re-enter mount (that feedback loop stutters the minimize animation).
thread_local int g_insideCalendarLaunchMount = 0;

struct CalendarLaunchMountGuard {
    CalendarLaunchMountGuard() { ++g_insideCalendarLaunchMount; }
    ~CalendarLaunchMountGuard() { --g_insideCalendarLaunchMount; }
};

bool IsLaunchHostAlreadyTracked(wuxc::Grid const& section,
                                wuxc::Button const& launch,
                                wuxc::Button const& expand) {
    if (!section || !launch || !expand) {
        return false;
    }
    std::lock_guard lock(g_mountMutex);
    for (auto const& existing : g_calendarLaunchMounted) {
        auto mountedSection = existing.calendarSection.get();
        if (!(mountedSection && mountedSection == section)) {
            continue;
        }
        auto mountedLaunch = existing.launchButton.get();
        auto mountedExpand = existing.expandButton.get();
        if (mountedLaunch && mountedExpand && mountedLaunch == launch &&
            mountedExpand == expand) {
            return true;
        }
    }
    return false;
}

bool MountCalendarLaunchButton(InstanceHandle handle,
                               wuxc::Grid const& section) {
    if (g_shuttingDown.load() || !section) {
        return false;
    }
    if (g_insideCalendarLaunchMount > 0) {
        // Nested visual-tree callback during wrap — treat existing host as done.
        return FindNamedFrameworkElement(section, kCalendarLaunchHostName, 10) !=
               nullptr;
    }
    CalendarLaunchMountGuard mountGuard;

    auto settings = GetSettingsCopy();
    const bool wantButton =
        settings.showCalendarAppButton &&
        !TrimCalendarAppPath(settings.calendarAppPath).empty();

    if (!wantButton) {
        MountedCalendarLaunchInstance staleToUnmount;
        bool hasStale = false;
        {
            std::lock_guard lock(g_mountMutex);
            for (auto it = g_calendarLaunchMounted.begin();
                 it != g_calendarLaunchMounted.end(); ++it) {
                auto mountedSection = it->calendarSection.get();
                if (mountedSection && mountedSection == section) {
                    staleToUnmount = std::move(*it);
                    g_calendarLaunchMounted.erase(it);
                    hasStale = true;
                    break;
                }
            }
        }
        if (hasStale) {
            UnmountCalendarLaunchInstance(staleToUnmount);
        } else {
            UnwrapCalendarLaunchHost(section);
            RemoveCalendarLaunchButtons(section);
        }
        return false;
    }

    // Healthy wrap already in the tree: never unwrap / reconfigure. Rebuilds
    // and RenderTransform clears here made chevron minimize animation jitter.
    {
        wuxc::StackPanel host{nullptr};
        wuxc::Button launch{nullptr};
        wuxc::Button expand{nullptr};
        if (TryGetHealthyLaunchHost(section, host, launch, expand)) {
            if (IsLaunchHostAlreadyTracked(section, launch, expand)) {
                return true;
            }

            // Stale tracking (expired weak refs) — drop revokers only; keep host.
            {
                MountedCalendarLaunchInstance stale;
                bool hasStale = false;
                {
                    std::lock_guard lock(g_mountMutex);
                    for (auto it = g_calendarLaunchMounted.begin();
                         it != g_calendarLaunchMounted.end(); ++it) {
                        auto mountedSection = it->calendarSection.get();
                        if (mountedSection && mountedSection == section) {
                            stale = std::move(*it);
                            g_calendarLaunchMounted.erase(it);
                            hasStale = true;
                            break;
                        }
                    }
                }
                if (hasStale) {
                    stale.clickRevoker = {};
                    stale.pointerEnteredRevoker = {};
                    stale.pointerPressedRevoker = {};
                    stale.pointerReleasedRevoker = {};
                    stale.pointerExitedRevoker = {};
                    stale.tappedRevoker = {};
                }
            }

            // Adopt an orphan host left in the visual tree (one-time wire).
            wuxc::Border fill{nullptr};
            wuxc::Border stroke{nullptr};
            wuxc::Border pressed{nullptr};
            ConfigureLaunchInHost(launch, expand, fill, stroke, pressed);
            RemoveCalendarLaunchButtons(section, launch);

            MountedCalendarLaunchInstance instance;
            instance.sectionHandle = handle;
            instance.uiThreadId = GetCurrentThreadId();
            instance.calendarSection = winrt::make_weak(section);
            instance.host = winrt::make_weak(host);
            instance.launchButton = winrt::make_weak(launch);
            instance.expandButton = winrt::make_weak(expand);
            try {
                if (auto parent =
                        wuxm::VisualTreeHelper::GetParent(host)
                            .try_as<wuxc::Panel>()) {
                    instance.parent = winrt::make_weak(parent);
                }
                instance.dispatcherQueue =
                    ws::DispatcherQueue::GetForCurrentThread();
            } catch (...) {
            }
            try {
                if (auto coreWindow = wuc::CoreWindow::GetForCurrentThread()) {
                    instance.coreDispatcher = coreWindow.Dispatcher();
                }
            } catch (...) {
            }
            WireCalendarLaunchForInstance(launch, instance, fill, stroke,
                                          pressed);
            {
                std::lock_guard lock(g_mountMutex);
                g_calendarLaunchMounted.push_back(std::move(instance));
            }
            Wh_Log(L"Adopted existing calendar launch host");
            return true;
        }
    }

    // Broken / leftover host: unwrap once, then wrap fresh.
    if (FindNamedFrameworkElement(section, kCalendarLaunchHostName, 10)) {
        Wh_Log(L"Broken calendar launch host — unwrapping before remount");
        UnwrapCalendarLaunchHost(section);
    }

    // Drop sibling leftovers from the old margin-offset approach.
    RemoveCalendarLaunchButtons(section);

    MountedCalendarLaunchInstance staleToUnmount;
    bool hasStaleToUnmount = false;
    {
        std::lock_guard lock(g_mountMutex);
        for (auto it = g_calendarLaunchMounted.begin();
             it != g_calendarLaunchMounted.end(); ++it) {
            auto mountedSection = it->calendarSection.get();
            if (mountedSection && mountedSection == section) {
                staleToUnmount = std::move(*it);
                g_calendarLaunchMounted.erase(it);
                hasStaleToUnmount = true;
                break;
            }
        }
    }
    if (hasStaleToUnmount) {
        // Tree already cleaned above; just drop revokers/weak refs.
        staleToUnmount.clickRevoker = {};
        staleToUnmount.pointerEnteredRevoker = {};
        staleToUnmount.pointerPressedRevoker = {};
        staleToUnmount.pointerReleasedRevoker = {};
        staleToUnmount.pointerExitedRevoker = {};
        staleToUnmount.tappedRevoker = {};
        staleToUnmount.host = nullptr;
        staleToUnmount.parent = nullptr;
        staleToUnmount.launchButton = nullptr;
        staleToUnmount.expandButton = nullptr;
        staleToUnmount.hoverFill = nullptr;
        staleToUnmount.templateFill = nullptr;
        staleToUnmount.templateStroke = nullptr;
        staleToUnmount.templatePressed = nullptr;
        staleToUnmount.calendarSection = nullptr;
    }

    auto expandFe =
        FindNamedFrameworkElement(section, L"ExpandCollapseButton", 10);
    if (!expandFe) {
        Wh_Log(L"ExpandCollapseButton not found yet for calendar launch");
        return false;
    }
    auto expandButton = expandFe.try_as<wuxc::Button>();
    if (!expandButton) {
        return false;
    }

    auto parent = wuxm::VisualTreeHelper::GetParent(expandButton)
                      .try_as<wuxc::Panel>();
    if (!parent) {
        return false;
    }
    // Expand must not already be inside our host (broken path handled above).
    try {
        if (auto pfe = parent.try_as<wux::FrameworkElement>()) {
            if (pfe.Name() == kCalendarLaunchHostName) {
                UnwrapCalendarLaunchHost(section);
                expandFe = FindNamedFrameworkElement(
                    section, L"ExpandCollapseButton", 10);
                expandButton =
                    expandFe ? expandFe.try_as<wuxc::Button>() : nullptr;
                parent = expandButton
                             ? wuxm::VisualTreeHelper::GetParent(expandButton)
                                   .try_as<wuxc::Panel>()
                             : nullptr;
            }
        }
    } catch (...) {
    }
    if (!expandButton || !parent) {
        return false;
    }

    const int32_t expandIndex = IndexOfPanelChild(parent, expandButton);
    if (expandIndex < 0) {
        return false;
    }

    // Capture expand slot props — host inherits them so the cell can grow
    // leftward for both controls (narrow Grid cells cannot do this with margin).
    wux::Thickness expandMargin{};
    wux::HorizontalAlignment expandHAlign = wux::HorizontalAlignment::Right;
    wux::VerticalAlignment expandVAlign = wux::VerticalAlignment::Top;
    int row = 0, col = 0, rowSpan = 1, colSpan = 1;
    try {
        expandMargin = expandButton.Margin();
        expandHAlign = expandButton.HorizontalAlignment();
        expandVAlign = expandButton.VerticalAlignment();
        row = wuxc::Grid::GetRow(expandButton);
        col = wuxc::Grid::GetColumn(expandButton);
        rowSpan = wuxc::Grid::GetRowSpan(expandButton);
        colSpan = wuxc::Grid::GetColumnSpan(expandButton);
    } catch (...) {
    }

    wuxc::Border hoverFill{nullptr};
    wuxc::Border templateFill{nullptr};
    wuxc::Border templateStroke{nullptr};
    wuxc::Border templatePressed{nullptr};
    auto launchButton = BuildCalendarLaunchButton(expandButton, hoverFill);
    ConfigureLaunchInHost(launchButton, expandButton, templateFill,
                          templateStroke, templatePressed);
    if (!templateFill) {
        templateFill = hoverFill;
    }

    wuxc::StackPanel host;
    host.Name(kCalendarLaunchHostName);
    host.Orientation(wuxc::Orientation::Horizontal);
    host.HorizontalAlignment(expandHAlign);
    host.VerticalAlignment(expandVAlign);
    host.Margin(expandMargin);
    host.IsHitTestVisible(true);
    try {
        wuxc::Grid::SetRow(host, row);
        wuxc::Grid::SetColumn(host, col);
        wuxc::Grid::SetRowSpan(host, rowSpan);
        wuxc::Grid::SetColumnSpan(host, colSpan);
    } catch (...) {
    }

    // Expand lives inside the host; host owns the outer margin/alignment.
    // Do not clear ExpandCollapseButton.RenderTransform — shell collapse
    // animation depends on it.
    expandButton.Margin(wux::Thickness{0, 0, 0, 0});
    expandButton.HorizontalAlignment(wux::HorizontalAlignment::Center);

    MountedCalendarLaunchInstance instance;
    instance.sectionHandle = handle;
    instance.uiThreadId = GetCurrentThreadId();
    instance.calendarSection = winrt::make_weak(section);
    instance.parent = winrt::make_weak(parent);
    instance.host = winrt::make_weak(host);
    instance.expandButton = winrt::make_weak(expandButton);
    instance.launchButton = winrt::make_weak(launchButton);
    instance.hoverFill = winrt::make_weak(hoverFill);
    instance.templateFill = winrt::make_weak(templateFill);
    instance.templateStroke = winrt::make_weak(templateStroke);
    instance.templatePressed = winrt::make_weak(templatePressed);
    instance.gridRow = row;
    instance.gridColumn = col;
    instance.gridRowSpan = rowSpan;
    instance.gridColumnSpan = colSpan;
    instance.expandHAlign = expandHAlign;
    instance.expandVAlign = expandVAlign;
    instance.expandMargin = expandMargin;
    try {
        instance.dispatcherQueue = ws::DispatcherQueue::GetForCurrentThread();
    } catch (...) {
    }
    try {
        if (auto coreWindow = wuc::CoreWindow::GetForCurrentThread()) {
            instance.coreDispatcher = coreWindow.Dispatcher();
        }
    } catch (...) {
    }

    try {
        parent.Children().RemoveAt(static_cast<uint32_t>(expandIndex));
        host.Children().Append(launchButton);
        host.Children().Append(expandButton);
        parent.Children().InsertAt(static_cast<uint32_t>(expandIndex), host);
    } catch (...) {
        Wh_Log(L"Failed wrapping calendar launch host %08X",
               winrt::to_hresult());
        // Best-effort restore if we already removed expand.
        try {
            if (IndexOfPanelChild(parent, expandButton) < 0 &&
                IndexOfPanelChild(parent, host) < 0) {
                expandButton.Margin(expandMargin);
                expandButton.HorizontalAlignment(expandHAlign);
                expandButton.VerticalAlignment(expandVAlign);
                parent.Children().InsertAt(static_cast<uint32_t>(expandIndex),
                                           expandButton);
            }
        } catch (...) {
        }
        return false;
    }

    // Template parts resolve reliably only after the button is in the tree.
    ResolveCalendarLaunchChromeParts(launchButton, templateFill, templateStroke,
                                     templatePressed);
    instance.templateFill = winrt::make_weak(templateFill);
    instance.templateStroke = winrt::make_weak(templateStroke);
    instance.templatePressed = winrt::make_weak(templatePressed);
    instance.hoverFill = winrt::make_weak(templateFill);
    WireCalendarLaunchForInstance(launchButton, instance, templateFill,
                                  templateStroke, templatePressed);

    winrt::weak_ref<wuxc::Button> weakLaunch = instance.launchButton;
    winrt::weak_ref<wuxc::Button> weakExpand = instance.expandButton;
    try {
        if (auto dq = instance.dispatcherQueue
                          ? instance.dispatcherQueue
                          : ws::DispatcherQueue::GetForCurrentThread()) {
            dq.TryEnqueue(
                ws::DispatcherQueueHandler([weakLaunch, weakExpand]() {
                    try {
                        auto launch = weakLaunch.get();
                        auto expand = weakExpand.get();
                        if (!launch || !expand) {
                            return;
                        }
                        wuxc::Border fill{nullptr};
                        wuxc::Border stroke{nullptr};
                        wuxc::Border pressed{nullptr};
                        ConfigureLaunchInHost(launch, expand, fill, stroke,
                                              pressed);
                        std::lock_guard lock(g_mountMutex);
                        for (auto& mounted : g_calendarLaunchMounted) {
                            if (auto btn = mounted.launchButton.get()) {
                                if (btn == launch) {
                                    WireCalendarLaunchForInstance(
                                        launch, mounted, fill, stroke, pressed);
                                    break;
                                }
                            }
                        }
                    } catch (...) {
                    }
                }));
        }
    } catch (...) {
    }

    {
        std::lock_guard lock(g_mountMutex);
        g_calendarLaunchMounted.push_back(std::move(instance));
    }
    Wh_Log(L"Mounted calendar launch host (durable StackPanel wrap)");
    return true;
}


void ScheduleCalendarLaunchMountRetry(InstanceHandle handle,
                                      wuxc::Grid const& section,
                                      int attempt) {
    constexpr int kMaxAttempts = 12;
    if (g_shuttingDown.load() || attempt > kMaxAttempts) {
        return;
    }
    try {
        if (auto dq = ws::DispatcherQueue::GetForCurrentThread()) {
            auto timer = dq.CreateTimer();
            timer.Interval(std::chrono::milliseconds{
                (std::min)(50 * (attempt + 1), 400)});
            timer.IsRepeating(false);
            winrt::weak_ref<wuxc::Grid> weakSection = winrt::make_weak(section);
            timer.Tick([handle, weakSection, attempt,
                        timer](wf::IInspectable const&,
                               wf::IInspectable const&) {
                try {
                    timer.Stop();
                } catch (...) {
                }
                if (g_shuttingDown.load()) {
                    return;
                }
                auto section = weakSection.get();
                if (!section) {
                    return;
                }
                try {
                    if (!MountCalendarLaunchButton(handle, section)) {
                        auto settings = GetSettingsCopy();
                        if (settings.showCalendarAppButton &&
                            !TrimCalendarAppPath(settings.calendarAppPath)
                                 .empty()) {
                            ScheduleCalendarLaunchMountRetry(handle, section,
                                                             attempt + 1);
                        }
                    }
                } catch (...) {
                }
            });
            timer.Start();
            return;
        }
    } catch (...) {
    }
}

void ApplyOrRestoreAllCalendarLaunch() {
    auto settings = GetSettingsCopy();
    const bool wantButton =
        settings.showCalendarAppButton &&
        !TrimCalendarAppPath(settings.calendarAppPath).empty();

    std::vector<MountedCalendarLaunchInstance> toUnmount;
    {
        std::lock_guard lock(g_mountMutex);
        if (!wantButton) {
            toUnmount.swap(g_calendarLaunchMounted);
        }
    }
    for (auto& instance : toUnmount) {
        UnmountCalendarLaunchInstance(instance);
    }

    // Discover CalendarSection from weather mounts / existing daylight mounts.
    std::vector<std::pair<InstanceHandle, wuxc::Grid>> sections;
    {
        std::lock_guard lock(g_mountMutex);
        for (auto const& weather : g_mounted) {
            auto grid = weather.notificationGrid.get();
            if (!grid) {
                continue;
            }
            try {
                if (auto calendar =
                        FindNamedGridNearby(grid, L"CalendarCenterGrid")) {
                    if (auto section = FindNamedFrameworkElement(
                                           calendar, L"CalendarSection", 8)
                                           .try_as<wuxc::Grid>()) {
                        sections.emplace_back(weather.gridHandle, section);
                    }
                }
            } catch (...) {
            }
        }
        for (auto const& daylight : g_daylightMounted) {
            if (auto section = daylight.calendarSection.get()) {
                sections.emplace_back(daylight.sectionHandle, section);
            }
        }
    }

    // Only unwrap when the feature is off. Remount must keep a healthy host.
    if (!wantButton) {
        for (auto const& [handle, section] : sections) {
            (void)handle;
            UnwrapCalendarLaunchHost(section);
            RemoveCalendarLaunchButtons(section);
            try {
                if (auto expand = FindNamedFrameworkElement(
                        section, L"ExpandCollapseButton", 10)) {
                    if (auto btn = expand.try_as<wuxc::Button>()) {
                        ClearExpandLaunchTransform(btn);
                    }
                }
            } catch (...) {
            }
        }
        return;
    }

    for (auto const& [handle, section] : sections) {
        try {
            if (!MountCalendarLaunchButton(handle, section)) {
                ScheduleCalendarLaunchMountRetry(handle, section, 0);
            }
        } catch (...) {
            Wh_Log(L"ApplyOrRestoreAllCalendarLaunch error %08X",
                   winrt::to_hresult());
        }
    }
}

bool IsCalendarSection(wux::FrameworkElement const& element, PCWSTR typeName) {
    try {
        if (element.Name() != L"CalendarSection") {
            return false;
        }
        auto className = winrt::get_class_name(element);
        if (className != L"Windows.UI.Xaml.Controls.Grid") {
            if (typeName && wcsstr(typeName, L"Grid") == nullptr) {
                return false;
            }
        }
        return element.try_as<wuxc::Grid>() != nullptr;
    } catch (...) {
        return false;
    }
}

bool IsWorldClocksElementName(std::wstring_view name) {
    return name == L"ClocksSection" || name == L"Clocks" ||
           name == L"WorldClocks" || name == L"WorldClockList" ||
           name == L"AdditionalClocks" || name == L"ClockList";
}

bool IsDaylightRootName(std::wstring_view name) {
    return name == L"WindhawkDaylightRoot";
}

void RecordAndHideElement(wux::UIElement const& ui,
                          std::vector<ChildVisibilityRecord>& records) {
    if (!ui) {
        return;
    }
    for (auto const& existing : records) {
        if (auto el = existing.element.get()) {
            if (el == ui) {
                // Already recorded — keep forcing hidden.
                try {
                    ui.Visibility(wux::Visibility::Collapsed);
                    ui.Opacity(0.0);
                    ui.IsHitTestVisible(false);
                } catch (...) {
                }
                return;
            }
        }
    }

    ChildVisibilityRecord record;
    record.element = winrt::make_weak(ui);
    try {
        record.original = ui.Visibility();
    } catch (...) {
        record.original = wux::Visibility::Visible;
    }
    try {
        record.originalOpacity = ui.Opacity();
    } catch (...) {
        record.originalOpacity = 1.0;
    }
    records.push_back(record);
    try {
        ui.Visibility(wux::Visibility::Collapsed);
        ui.Opacity(0.0);
        ui.IsHitTestVisible(false);
    } catch (...) {
    }
}

void RestoreHiddenElements(std::vector<ChildVisibilityRecord>& records) {
    for (auto& record : records) {
        if (auto el = record.element.get()) {
            try {
                el.Visibility(record.original);
                el.Opacity(record.originalOpacity);
                el.IsHitTestVisible(true);
            } catch (...) {
            }
        }
    }
    records.clear();
}

void HideElementSubtree(wux::DependencyObject const& root,
                        std::vector<ChildVisibilityRecord>& records,
                        int depth = 0) {
    if (!root || depth > 12) {
        return;
    }
    if (auto ui = root.try_as<wux::UIElement>()) {
        // Never hide our injected daylight root if it ends up in this walk.
        if (auto fe = root.try_as<wux::FrameworkElement>()) {
            if (IsDaylightRootName(fe.Name())) {
                return;
            }
        }
        RecordAndHideElement(ui, records);
    }
    try {
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; i++) {
            HideElementSubtree(wuxm::VisualTreeHelper::GetChild(root, i),
                               records, depth + 1);
        }
    } catch (...) {
    }
}

void CollapseClocksHostLayout(wux::FrameworkElement const& clocksHost,
                              MountedDaylightInstance& instance) {
    try {
        if (!instance.clocksLayoutSaved) {
            instance.clocksHeight = clocksHost.Height();
            instance.clocksMinHeight = clocksHost.MinHeight();
            instance.clocksMaxHeight = clocksHost.MaxHeight();
            instance.clocksMargin = clocksHost.Margin();
            instance.clocksLayoutSaved = true;
        }
        // Shell may force Visibility back to Visible; zeroing layout keeps the
        // clocks band from reserving the old Warsaw/Lisbon height.
        clocksHost.Height(0.0);
        clocksHost.MinHeight(0.0);
        clocksHost.MaxHeight(0.0);
        clocksHost.Margin(wux::Thickness{0, 0, 0, 0});
    } catch (...) {
    }
}

void HideNativeClocksChildren(wux::FrameworkElement const& clocksHost,
                              MountedDaylightInstance& instance) {
    try {
        if (auto panel = clocksHost.try_as<wuxc::Panel>()) {
            for (auto const& child : panel.Children()) {
                auto fe = child.try_as<wux::FrameworkElement>();
                if (fe && IsDaylightRootName(fe.Name())) {
                    continue;
                }
                if (auto ui = child.try_as<wux::UIElement>()) {
                    RecordAndHideElement(ui, instance.hiddenClocks);
                    HideElementSubtree(child, instance.hiddenClocks);
                }
            }
            return;
        }

        // Fallback: visual-tree walk, skipping our root.
        const int32_t count =
            wuxm::VisualTreeHelper::GetChildrenCount(clocksHost);
        for (int32_t i = 0; i < count; i++) {
            auto child = wuxm::VisualTreeHelper::GetChild(clocksHost, i);
            if (auto fe = child.try_as<wux::FrameworkElement>()) {
                if (IsDaylightRootName(fe.Name())) {
                    continue;
                }
            }
            HideElementSubtree(child, instance.hiddenClocks);
        }
    } catch (...) {
        Wh_Log(L"HideNativeClocksChildren failed %08X", winrt::to_hresult());
    }
}

void HideCalendarClocksContent(wuxc::Grid const& section,
                               MountedDaylightInstance& instance) {
    // Only hide named world-clock hosts. Never hide by Grid.Row: after login
    // attached rows are often still all 0, which would also collapse
    // CalendarControlScrollViewer and leave a broken dark "square" on the
    // calendar until the mod is toggled.
    try {
        for (auto const& child : section.Children()) {
            auto fe = child.try_as<wux::FrameworkElement>();
            if (!fe) {
                continue;
            }
            const auto name = fe.Name();
            if (IsDaylightRootName(name) || !IsWorldClocksElementName(name)) {
                continue;
            }

            if (!instance.clocksHost.get()) {
                instance.clocksHost = winrt::make_weak(fe);
            }

            if (auto ui = child.try_as<wux::UIElement>()) {
                RecordAndHideElement(ui, instance.hiddenClocks);
                HideElementSubtree(fe, instance.hiddenClocks);
                CollapseClocksHostLayout(fe, instance);
                Wh_Log(L"Hidden calendar clocks content '%s' row=%d",
                       name.c_str(), wuxc::Grid::GetRow(fe));
            }
        }

        if (auto host = instance.clocksHost.get()) {
            if (auto ui = host.try_as<wux::UIElement>()) {
                RecordAndHideElement(ui, instance.hiddenClocks);
            }
            HideNativeClocksChildren(host, instance);
            CollapseClocksHostLayout(host, instance);
        }
    } catch (...) {
        Wh_Log(L"HideCalendarClocksContent failed %08X", winrt::to_hresult());
    }
}

void EnsureCalendarStructuralVisible(wuxc::Grid const& section) {
    // Repair calendar chrome if an earlier same-row hide (or shell race)
    // collapsed the month ScrollViewer / header.
    //
    // Do NOT force CalendarHeaderMinimizedOverlay visible — that border is
    // only for the collapsed-calendar state. Showing it while expanded paints
    // a wrong solid plate over the clock/daylight header until the user
    // toggles expand/collapse (which restores the shell's real visibility).
    try {
        wux::UIElement calendarScroll{nullptr};
        wux::UIElement minimizedOverlay{nullptr};

        for (auto const& child : section.Children()) {
            auto fe = child.try_as<wux::FrameworkElement>();
            if (!fe) {
                continue;
            }
            const auto name = fe.Name();
            if (IsWorldClocksElementName(name) || IsDaylightRootName(name)) {
                continue;
            }
            if (name == L"CalendarHeaderMinimizedOverlay") {
                minimizedOverlay = child.try_as<wux::UIElement>();
                continue;
            }
            if (name != L"CalendarControlScrollViewer" &&
                name != L"CalendarHeader" &&
                name != L"ExpandCollapseButton" &&
                name != L"FocusSessionControl") {
                continue;
            }
            if (auto ui = child.try_as<wux::UIElement>()) {
                if (name == L"CalendarControlScrollViewer") {
                    calendarScroll = ui;
                }
                if (ui.Visibility() != wux::Visibility::Visible) {
                    ui.Visibility(wux::Visibility::Visible);
                }
                if (ui.Opacity() < 0.5) {
                    ui.Opacity(1.0);
                }
            }
            try {
                // Undo accidental zero-height from a bad same-row collapse.
                if (fe.Height() == 0.0) {
                    fe.ClearValue(wux::FrameworkElement::HeightProperty());
                }
                if (fe.MinHeight() == 0.0 && fe.MaxHeight() == 0.0) {
                    fe.ClearValue(wux::FrameworkElement::MinHeightProperty());
                    fe.ClearValue(wux::FrameworkElement::MaxHeightProperty());
                }
            } catch (...) {
            }
        }

        // Month grid visible ⇒ header must not use the minimized overlay plate.
        if (calendarScroll && minimizedOverlay &&
            calendarScroll.Visibility() == wux::Visibility::Visible) {
            try {
                minimizedOverlay.Visibility(wux::Visibility::Collapsed);
            } catch (...) {
            }
        }
    } catch (...) {
    }
}

bool CalendarSectionReadyForDaylight(wuxc::Grid const& section,
                                     wux::FrameworkElement& clocksEl,
                                     wux::FrameworkElement& calendarScroll,
                                     wux::FrameworkElement& calendarHeader) {
    clocksEl = nullptr;
    calendarScroll = nullptr;
    calendarHeader = nullptr;

    int structuralCount = 0;
    int distinctRows = 0;
    bool rowSeen[16] = {};

    try {
        for (auto const& child : section.Children()) {
            auto fe = child.try_as<wux::FrameworkElement>();
            if (!fe) {
                continue;
            }
            const auto name = fe.Name();
            const int row = wuxc::Grid::GetRow(fe);
            const bool structural =
                IsWorldClocksElementName(name) ||
                name == L"CalendarControlScrollViewer" ||
                name == L"CalendarHeader" ||
                name == L"ExpandCollapseButton" ||
                name == L"FocusSessionControl" ||
                name == L"CalendarHeaderMinimizedOverlay";
            if (!structural) {
                continue;
            }
            ++structuralCount;
            if (row >= 0 && row < 16 && !rowSeen[row]) {
                rowSeen[row] = true;
                ++distinctRows;
            }
            if (!clocksEl && IsWorldClocksElementName(name)) {
                clocksEl = fe;
            } else if (name == L"CalendarControlScrollViewer") {
                calendarScroll = fe;
            } else if (name == L"CalendarHeader") {
                calendarHeader = fe;
            }
        }
    } catch (...) {
        return false;
    }

    if (!calendarScroll && !clocksEl && !calendarHeader) {
        return false;
    }

    // After login, Grid.Row is often still default 0 for every child. Mutating
    // layout then permanently misplaces the month chrome until remount.
    if (structuralCount >= 3 && distinctRows <= 1) {
        Wh_Log(L"CalendarSection not ready: %d structural children, %d "
               L"distinct rows",
               structuralCount, distinctRows);
        return false;
    }
    return true;
}

void ScheduleDaylightMountRetry(InstanceHandle handle,
                                wuxc::Grid const& section,
                                int attempt);

bool MountDaylightIntoCalendarSectionAttempt(InstanceHandle handle,
                                             wuxc::Grid const& section,
                                             int attempt);

bool MountDaylightIntoCalendarSection(InstanceHandle handle,
                                      wuxc::Grid const& section) {
    return MountDaylightIntoCalendarSectionAttempt(handle, section, 0);
}

bool MountDaylightIntoCalendarSectionAttempt(InstanceHandle handle,
                                             wuxc::Grid const& section,
                                             int attempt) {
    if (g_shuttingDown.load()) {
        return false;
    }

    auto settings = GetSettingsCopy();
    if (!settings.showCalendarDaylight) {
        return false;
    }

    for (auto const& child : section.Children()) {
        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (IsDaylightRootName(fe.Name())) {
                Wh_Log(L"WindhawkDaylightRoot already present — refreshing");
                EnsureCalendarStructuralVisible(section);
                try {
                    RefreshDaylightOnFlyoutShown();
                } catch (...) {
                }
                {
                    std::lock_guard lock(g_mountMutex);
                    for (auto& mounted : g_daylightMounted) {
                        StartDaylightDispatcherTick(mounted);
                    }
                }
                EnsureDaylightTickTimer();
                return true;
            }
        }
    }

    EnsureCalendarStructuralVisible(section);

    wux::FrameworkElement clocksEl{nullptr};
    wux::FrameworkElement calendarScroll{nullptr};
    wux::FrameworkElement calendarHeader{nullptr};
    if (!CalendarSectionReadyForDaylight(section, clocksEl, calendarScroll,
                                         calendarHeader)) {
        ScheduleDaylightMountRetry(handle, section, attempt + 1);
        return false;
    }

    try {
        for (auto const& child : section.Children()) {
            if (auto fe = child.try_as<wux::FrameworkElement>()) {
                Wh_Log(L"CalendarSection child '%s' Grid.Row=%d",
                       fe.Name().c_str(), wuxc::Grid::GetRow(fe));
            }
        }
    } catch (...) {
    }

    MountedDaylightInstance instance;
    instance.sectionHandle = handle;
    instance.uiThreadId = GetCurrentThreadId();
    instance.calendarSection = winrt::make_weak(section);
    instance.generation = g_uiGeneration.load();
    try {
        instance.dispatcherQueue = ws::DispatcherQueue::GetForCurrentThread();
    } catch (...) {
    }
    try {
        if (auto coreWindow = wuc::CoreWindow::GetForCurrentThread()) {
            instance.coreDispatcher = coreWindow.Dispatcher();
        }
    } catch (...) {
    }

    DaylightUiControls ui;
    auto root = BuildDaylightRoot(ui);
    instance.ui = ui;
    instance.daylightRoot = winrt::make_weak(root);

    try {
        const int columnSpan = (std::max)(
            1, static_cast<int>(section.ColumnDefinitions().Size()));

        if (clocksEl) {
            instance.clocksHost = winrt::make_weak(clocksEl);
            instance.insertedGridRow = wuxc::Grid::GetRow(clocksEl);
            instance.insertedRowDefinition = false;

            HideCalendarClocksContent(section, instance);
            wuxc::Grid::SetRow(root, instance.insertedGridRow);
            wuxc::Grid::SetColumnSpan(root, columnSpan);
            section.Children().Append(root);
            HideCalendarClocksContent(section, instance);
            EnsureCalendarStructuralVisible(section);
            Wh_Log(L"Mounted daylight over ClocksSection row %d",
                   instance.insertedGridRow);
        } else {
            int insertRow = -1;
            if (calendarScroll) {
                insertRow = wuxc::Grid::GetRow(calendarScroll);
            } else if (calendarHeader) {
                insertRow = wuxc::Grid::GetRow(calendarHeader) + 1;
            }
            if (insertRow < 0) {
                Wh_Log(L"CalendarSection: no insert point for daylight root");
                ScheduleDaylightMountRetry(handle, section, attempt + 1);
                return false;
            }

            for (auto const& child : section.Children()) {
                auto fe = child.try_as<wux::FrameworkElement>();
                if (!fe) {
                    continue;
                }
                const int row = wuxc::Grid::GetRow(fe);
                if (row >= insertRow) {
                    wuxc::Grid::SetRow(fe, row + 1);
                }
            }

            wuxc::RowDefinition daylightRow;
            daylightRow.Height(wux::GridLength{0, wux::GridUnitType::Auto});
            if (insertRow <=
                static_cast<int>(section.RowDefinitions().Size())) {
                section.RowDefinitions().InsertAt(
                    static_cast<uint32_t>(insertRow), daylightRow);
            } else {
                section.RowDefinitions().Append(daylightRow);
                insertRow =
                    static_cast<int>(section.RowDefinitions().Size()) - 1;
            }
            instance.insertedGridRow = insertRow;
            instance.insertedRowDefinition = true;

            wuxc::Grid::SetRow(root, instance.insertedGridRow);
            wuxc::Grid::SetColumnSpan(root, columnSpan);
            section.Children().Append(root);
            EnsureCalendarStructuralVisible(section);
            Wh_Log(L"Mounted daylight into new CalendarSection row %d",
                   insertRow);
        }
    } catch (...) {
        Wh_Log(L"Failed to insert daylight root %08X", winrt::to_hresult());
        RestoreHiddenElements(instance.hiddenClocks);
        EnsureCalendarStructuralVisible(section);
        return false;
    }

    {
        std::lock_guard lock(g_mountMutex);
        g_daylightMounted.push_back(std::move(instance));
        auto& mounted = g_daylightMounted.back();
        StartDaylightDispatcherTick(mounted);
        if (auto root = mounted.daylightRoot.get()) {
            RegisterVisibilityRefresh(root, mounted.visibilityCookie);
        }
    }

    ForecastData forecast;
    {
        std::lock_guard lock(g_forecastMutex);
        forecast = g_forecast;
    }
    {
        std::lock_guard lock(g_mountMutex);
        if (!g_daylightMounted.empty()) {
            ApplyDaylightToInstance(g_daylightMounted.back(), forecast);
        }
    }
    EnsureDaylightTickTimer();
    return true;
}

void ScheduleDaylightMountRetry(InstanceHandle handle,
                                wuxc::Grid const& section,
                                int attempt) {
    constexpr int kMaxAttempts = 12;
    if (g_shuttingDown.load() || attempt > kMaxAttempts) {
        Wh_Log(L"Daylight mount retry giving up (attempt %d)", attempt);
        EnsureCalendarStructuralVisible(section);
        return;
    }

    try {
        if (auto dq = ws::DispatcherQueue::GetForCurrentThread()) {
            auto timer = dq.CreateTimer();
            timer.Interval(std::chrono::milliseconds{
                (std::min)(50 * attempt, 400)});
            timer.IsRepeating(false);
            winrt::weak_ref<wuxc::Grid> weakSection = winrt::make_weak(section);
            timer.Tick([handle, weakSection, attempt,
                        timer](wf::IInspectable const&,
                               wf::IInspectable const&) {
                try {
                    timer.Stop();
                } catch (...) {
                }
                if (g_shuttingDown.load()) {
                    return;
                }
                if (auto section = weakSection.get()) {
                    MountDaylightIntoCalendarSectionAttempt(handle, section,
                                                            attempt);
                }
            });
            timer.Start();
            Wh_Log(L"Scheduled daylight mount retry %d", attempt);
            return;
        }
    } catch (...) {
        Wh_Log(L"DispatcherQueue timer retry failed %08X",
               winrt::to_hresult());
    }

    try {
        if (auto cw = wuc::CoreWindow::GetForCurrentThread()) {
            winrt::weak_ref<wuxc::Grid> weakSection = winrt::make_weak(section);
            cw.Dispatcher().RunAsync(
                wuc::CoreDispatcherPriority::Low,
                wuc::DispatchedHandler([handle, weakSection, attempt]() {
                    if (g_shuttingDown.load()) {
                        return;
                    }
                    if (auto section = weakSection.get()) {
                        MountDaylightIntoCalendarSectionAttempt(handle, section,
                                                                attempt);
                    }
                }));
        }
    } catch (...) {
    }
}

void UpdateAllDaylightUIs() {
    if (g_shuttingDown.load()) {
        return;
    }

    ForecastData forecast;
    {
        std::lock_guard lock(g_forecastMutex);
        forecast = g_forecast;
    }

    struct DispatchTarget {
        InstanceHandle handle = 0;
        DWORD uiThreadId = 0;
        ws::DispatcherQueue dispatcherQueue{nullptr};
        wuc::CoreDispatcher coreDispatcher{nullptr};
    };
    std::vector<DispatchTarget> snapshot;
    {
        std::lock_guard lock(g_mountMutex);
        snapshot.reserve(g_daylightMounted.size());
        for (auto const& mounted : g_daylightMounted) {
            snapshot.push_back(DispatchTarget{mounted.sectionHandle,
                                              mounted.uiThreadId,
                                              mounted.dispatcherQueue,
                                              mounted.coreDispatcher});
        }
    }

    if (snapshot.empty()) {
        return;
    }

    Wh_Log(L"UpdateAllDaylightUIs: %zu instance(s)", snapshot.size());

    auto applyOnUi = [forecast](InstanceHandle handle,
                                DWORD uiThreadId) mutable {
        if (g_shuttingDown.load()) {
            return;
        }
        try {
            const DWORD tid = GetCurrentThreadId();
            std::lock_guard lock(g_mountMutex);
            for (auto& mounted : g_daylightMounted) {
                // Prefer thread affinity; also accept handle match.
                if (mounted.uiThreadId == tid ||
                    (uiThreadId && mounted.uiThreadId == uiThreadId) ||
                    mounted.sectionHandle == handle) {
                    ApplyDaylightToInstance(mounted, forecast);
                }
            }
        } catch (...) {
            Wh_Log(L"Daylight UI apply error %08X", winrt::to_hresult());
        }
    };

    for (auto& target : snapshot) {
        try {
            // Prefer CoreDispatcher for ShellExperienceHost XAML.
            if (target.coreDispatcher) {
                target.coreDispatcher.RunAsync(
                    wuc::CoreDispatcherPriority::Normal,
                    wuc::DispatchedHandler(
                        [applyOnUi, handle = target.handle,
                         tid = target.uiThreadId]() mutable {
                            applyOnUi(handle, tid);
                        }));
            } else if (target.dispatcherQueue) {
                const bool queued =
                    target.dispatcherQueue.TryEnqueue(
                        ws::DispatcherQueueHandler(
                            [applyOnUi, handle = target.handle,
                             tid = target.uiThreadId]() mutable {
                                applyOnUi(handle, tid);
                            }));
                if (!queued) {
                    Wh_Log(L"Daylight UI TryEnqueue failed for handle %llu",
                           static_cast<unsigned long long>(target.handle));
                }
            } else {
                Wh_Log(L"Daylight UI: no dispatcher for handle %llu "
                       L"(skipping off-thread apply)",
                       static_cast<unsigned long long>(target.handle));
            }
        } catch (...) {
            Wh_Log(L"Dispatch daylight UI failed %08X", winrt::to_hresult());
        }
    }
}

void ApplyOrRestoreAllDaylight() {
    auto settings = GetSettingsCopy();
    std::vector<MountedDaylightInstance> toUnmount;
    {
        std::lock_guard lock(g_mountMutex);
        if (!settings.showCalendarDaylight) {
            toUnmount.swap(g_daylightMounted);
        } else {
            for (auto& mounted : g_daylightMounted) {
                mounted.generation = g_uiGeneration.load();
                if (auto root = mounted.daylightRoot.get()) {
                    root.Visibility(wux::Visibility::Visible);
                }
            }
        }
    }
    for (auto& instance : toUnmount) {
        UnmountDaylightInstance(instance);
    }

    if (settings.showCalendarDaylight) {
        // Try to discover CalendarSection from already-mounted weather grids.
        std::vector<wuxc::Grid> sections;
        {
            std::lock_guard lock(g_mountMutex);
            for (auto const& weather : g_mounted) {
                auto grid = weather.notificationGrid.get();
                if (!grid) {
                    continue;
                }
                try {
                    if (auto calendar = FindNamedGridNearby(
                            grid, L"CalendarCenterGrid")) {
                        std::vector<wux::DependencyObject> stack;
                        stack.push_back(calendar);
                        for (size_t i = 0; i < stack.size() && i < 64; i++) {
                            auto node = stack[i];
                            if (auto fe =
                                    node.try_as<wux::FrameworkElement>()) {
                                if (fe.Name() == L"CalendarSection") {
                                    if (auto section =
                                            fe.try_as<wuxc::Grid>()) {
                                        sections.push_back(section);
                                        continue;
                                    }
                                }
                            }
                            const int count =
                                wuxm::VisualTreeHelper::GetChildrenCount(node);
                            for (int c = 0; c < count; c++) {
                                stack.push_back(
                                    wuxm::VisualTreeHelper::GetChild(node, c));
                            }
                        }
                    }
                } catch (...) {
                }
            }
        }
        for (auto const& section : sections) {
            try {
                bool already = false;
                {
                    std::lock_guard lock(g_mountMutex);
                    for (auto const& mounted : g_daylightMounted) {
                        if (auto existing = mounted.calendarSection.get()) {
                            if (existing == section) {
                                already = true;
                                break;
                            }
                        }
                    }
                }
                if (!already) {
                    MountDaylightIntoCalendarSection(0, section);
                }
            } catch (...) {
            }
        }
        UpdateAllDaylightUIs();
    }
}

void ClearPanelChildren(wuxc::Panel const& panel) {
    if (panel) {
        panel.Children().Clear();
    }
}

void ApplyForecastToInstance(MountedWeatherInstance& instance,
                             ForecastData const& forecastIn,
                             ModSettings const& settings) {
    try {
        auto weatherRoot = instance.weatherRoot.get();
        if (!weatherRoot) {
            return;
        }

        // Re-slice hourly from the full cached series using "now" so the strip
        // advances with the clock even when a network refresh is delayed.
        ForecastData forecast = forecastIn;
        if (forecast.valid) {
            ResliceHourlyForecast(forecast, settings.hourlyCount);
        }

        auto primary = BrushOrFallback(
            weatherRoot, L"TextFillColorPrimaryBrush",
            winrt::Windows::UI::Color{255, 250, 250, 250});
        auto secondary = BrushOrFallback(
            weatherRoot, L"TextFillColorSecondaryBrush",
            winrt::Windows::UI::Color{255, 180, 180, 180});

        // Chrome is applied at mount; keep forecast updates resilient if chrome
        // tweaks throw (composition clip / theme probes must not block data).
        try {
            if (auto grid = instance.notificationGrid.get()) {
                ApplyCalendarMatchedChrome(instance, grid, weatherRoot);
            }
        } catch (...) {
            Wh_Log(L"ApplyCalendarMatchedChrome in forecast update failed %08X",
                   winrt::to_hresult());
        }

        if (instance.ui.locationText) {
            instance.ui.locationText.Visibility(
                settings.showLocation ? wux::Visibility::Visible
                                      : wux::Visibility::Collapsed);
            instance.ui.locationText.Text(
                forecast.valid ? winrt::hstring(forecast.locationDisplay)
                               : winrt::hstring(settings.locationName));
            instance.ui.locationText.Foreground(primary);
        }

        if (instance.ui.statusText) {
            if (!forecast.valid) {
                instance.ui.statusText.Text(
                    g_fetchInProgress.load() ? L"Loading weather..."
                                             : L"Weather unavailable");
                instance.ui.statusText.Visibility(wux::Visibility::Visible);
            } else {
                instance.ui.statusText.Visibility(wux::Visibility::Collapsed);
            }
        }

        if (instance.ui.contentRoot) {
            instance.ui.contentRoot.Opacity(forecast.valid ? 1.0 : 0.55);
        }

        if (!forecast.valid) {
            if (instance.ui.temperatureText) {
                instance.ui.temperatureText.Text(L"--\u00B0");
                instance.ui.temperatureText.Foreground(primary);
            }
            if (instance.ui.conditionText) {
                instance.ui.conditionText.Text(L"Unavailable");
                instance.ui.conditionText.FontWeight(wut::FontWeights::Bold());
                instance.ui.conditionText.Foreground(secondary);
            }
            if (instance.ui.highLowText) {
                instance.ui.highLowText.Text(L"--\u00B0  --\u00B0");
                instance.ui.highLowText.Foreground(secondary);
            }
            if (instance.ui.conditionIconHost) {
                SetWeatherIcon(instance.ui.conditionIconHost,
                               WeatherIconKind::Unknown, 36, primary);
            }
            if (instance.ui.hourlyPanel) {
                ClearPanelChildren(instance.ui.hourlyPanel);
            }
            if (instance.ui.dailyPanel) {
                ClearPanelChildren(instance.ui.dailyPanel);
            }
            return;
        }

        if (instance.ui.temperatureText) {
            instance.ui.temperatureText.Text(FormatTemp(forecast.currentTemp));
            instance.ui.temperatureText.Foreground(primary);
        }
        if (instance.ui.conditionText) {
            instance.ui.conditionText.Text(
                ConditionLabelFromCode(forecast.currentCode));
            instance.ui.conditionText.FontWeight(wut::FontWeights::Bold());
            instance.ui.conditionText.Foreground(secondary);
        }
        if (instance.ui.highLowText) {
            instance.ui.highLowText.Text(FormatTemp(forecast.todayMax) + L"  " +
                                         FormatTemp(forecast.todayMin));
            instance.ui.highLowText.Foreground(secondary);
        }
        if (instance.ui.conditionIconHost) {
            SetWeatherIcon(
                instance.ui.conditionIconHost,
                IconKindFromCode(forecast.currentCode, forecast.currentIsDay),
                36, primary);
        }

        if (instance.ui.hourlyPanel) {
            auto hourly = instance.ui.hourlyPanel;
            ClearPanelChildren(hourly);
            hourly.ColumnDefinitions().Clear();
            const int columns = static_cast<int>(forecast.hourly.size());
            for (int i = 0; i < columns; i++) {
                wuxc::ColumnDefinition col;
                col.Width(wux::GridLength{1, wux::GridUnitType::Star});
                hourly.ColumnDefinitions().Append(col);
            }
            for (int i = 0; i < columns; i++) {
                auto const& entry = forecast.hourly[static_cast<size_t>(i)];
                wuxc::StackPanel stack;
                stack.HorizontalAlignment(wux::HorizontalAlignment::Center);
                stack.VerticalAlignment(wux::VerticalAlignment::Center);
                stack.Spacing(4);

                wuxc::TextBlock hourText;
                hourText.Text(entry.hourLabel);
                hourText.FontSize(12);
                hourText.Foreground(secondary);
                hourText.HorizontalAlignment(wux::HorizontalAlignment::Center);

                wuxc::Grid hourIconHost;
                hourIconHost.Width(24);
                hourIconHost.Height(24);
                hourIconHost.HorizontalAlignment(
                    wux::HorizontalAlignment::Center);
                SetWeatherIcon(hourIconHost,
                               IconKindFromCode(entry.weatherCode, entry.isDay),
                               22, primary);

                wuxc::TextBlock tempText;
                tempText.Text(FormatTemp(entry.temperature));
                tempText.FontSize(13);
                tempText.FontWeight(wut::FontWeights::SemiBold());
                tempText.Foreground(primary);
                tempText.HorizontalAlignment(wux::HorizontalAlignment::Center);

                stack.Children().Append(hourText);
                stack.Children().Append(hourIconHost);
                stack.Children().Append(tempText);
                wuxc::Grid::SetColumn(stack, i);
                hourly.Children().Append(stack);
            }
        }

        if (instance.ui.dailyPanel) {
            auto daily = instance.ui.dailyPanel;
            ClearPanelChildren(daily);
            double globalMin = forecast.daily.front().minTemp;
            double globalMax = forecast.daily.front().maxTemp;
            for (auto const& day : forecast.daily) {
                globalMin = (std::min)(globalMin, day.minTemp);
                globalMax = (std::max)(globalMax, day.maxTemp);
            }

            for (auto const& day : forecast.daily) {
                wuxc::Grid row;
                row.Height(32);
                row.Margin(wux::Thickness{0, 1, 0, 1});
                row.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
                auto cols = row.ColumnDefinitions();

                wuxc::ColumnDefinition dayCol;
                dayCol.Width(wux::GridLength{44, wux::GridUnitType::Pixel});
                wuxc::ColumnDefinition iconCol;
                iconCol.Width(wux::GridLength{40, wux::GridUnitType::Pixel});
                wuxc::ColumnDefinition minCol;
                minCol.Width(wux::GridLength{36, wux::GridUnitType::Pixel});
                wuxc::ColumnDefinition graphCol;
                graphCol.Width(wux::GridLength{1, wux::GridUnitType::Star});
                wuxc::ColumnDefinition maxCol;
                maxCol.Width(wux::GridLength{36, wux::GridUnitType::Pixel});
                cols.Append(dayCol);
                cols.Append(iconCol);
                cols.Append(minCol);
                cols.Append(graphCol);
                cols.Append(maxCol);

                wuxc::TextBlock dayText;
                dayText.Text(day.weekdayLabel);
                dayText.FontSize(13);
                dayText.FontWeight(wut::FontWeights::SemiBold());
                dayText.Foreground(primary);
                dayText.VerticalAlignment(wux::VerticalAlignment::Center);
                wuxc::Grid::SetColumn(dayText, 0);

                wuxc::Grid dayIconHost;
                dayIconHost.Width(22);
                dayIconHost.Height(22);
                // Nudge left in the icon column (toward the day label).
                dayIconHost.HorizontalAlignment(wux::HorizontalAlignment::Left);
                dayIconHost.VerticalAlignment(wux::VerticalAlignment::Center);
                dayIconHost.Margin(wux::Thickness{2, 0, 0, 0});
                SetWeatherIcon(dayIconHost,
                               IconKindFromCode(day.weatherCode, true), 20,
                               primary);
                wuxc::Grid::SetColumn(dayIconHost, 1);

                wuxc::TextBlock minText;
                minText.Text(FormatTemp(day.minTemp));
                minText.FontSize(13);
                minText.Foreground(secondary);
                minText.HorizontalAlignment(wux::HorizontalAlignment::Right);
                minText.VerticalAlignment(wux::VerticalAlignment::Center);
                minText.Margin(wux::Thickness{0, 0, 8, 0});
                wuxc::Grid::SetColumn(minText, 2);

                auto track = MakeTemperatureTrack(weatherRoot, day.minTemp,
                                                  day.maxTemp, globalMin,
                                                  globalMax);
                track.Margin(wux::Thickness{4, 0, 8, 0});
                wuxc::Grid::SetColumn(track, 3);

                wuxc::TextBlock maxText;
                maxText.Text(FormatTemp(day.maxTemp));
                maxText.FontSize(13);
                maxText.FontWeight(wut::FontWeights::SemiBold());
                maxText.Foreground(primary);
                maxText.HorizontalAlignment(wux::HorizontalAlignment::Right);
                maxText.VerticalAlignment(wux::VerticalAlignment::Center);
                wuxc::Grid::SetColumn(maxText, 4);

                row.Children().Append(dayText);
                row.Children().Append(dayIconHost);
                row.Children().Append(minText);
                row.Children().Append(track);
                row.Children().Append(maxText);
                daily.Children().Append(row);
            }
        }

        try {
            std::wstring automation =
                forecast.locationDisplay + L", " +
                FormatTemp(forecast.currentTemp) + L", " +
                ConditionLabelFromCode(forecast.currentCode);
            wuxa::AutomationProperties::SetName(weatherRoot, automation);
        } catch (...) {
        }

        if (auto grid = instance.notificationGrid.get()) {
            EnsureGridFitsWeatherContent(grid, weatherRoot);
        }
    } catch (...) {
        Wh_Log(L"ApplyForecastToInstance error %08X", winrt::to_hresult());
    }
}

void UpdateAllWeatherUIs() {
    if (g_shuttingDown.load()) {
        return;
    }

    ForecastData forecast;
    {
        std::lock_guard lock(g_forecastMutex);
        forecast = g_forecast;
    }
    auto settings = GetSettingsCopy();

    struct DispatchTarget {
        InstanceHandle handle = 0;
        uint64_t generation = 0;
        ws::DispatcherQueue dispatcherQueue{nullptr};
        wuc::CoreDispatcher coreDispatcher{nullptr};
    };
    std::vector<DispatchTarget> snapshot;
    {
        std::lock_guard lock(g_mountMutex);
        snapshot.reserve(g_mounted.size());
        for (auto const& mounted : g_mounted) {
            snapshot.push_back(DispatchTarget{mounted.gridHandle,
                                              mounted.generation,
                                              mounted.dispatcherQueue,
                                              mounted.coreDispatcher});
        }
    }

    auto applyOnUi = [forecast, settings](InstanceHandle handle,
                                          uint64_t generation) mutable {
        if (g_shuttingDown.load()) {
            return;
        }
        try {
            std::lock_guard lock(g_mountMutex);
            for (auto& mounted : g_mounted) {
                if (mounted.gridHandle == handle &&
                    mounted.generation == generation) {
                    ApplyForecastToInstance(mounted, forecast, settings);
                    break;
                }
            }
        } catch (...) {
            Wh_Log(L"UI apply error %08X", winrt::to_hresult());
        }
    };

    for (auto& target : snapshot) {
        try {
            if (target.dispatcherQueue) {
                target.dispatcherQueue.TryEnqueue(ws::DispatcherQueueHandler(
                    [applyOnUi, handle = target.handle,
                     generation = target.generation]() mutable {
                        applyOnUi(handle, generation);
                    }));
            } else if (target.coreDispatcher) {
                target.coreDispatcher.RunAsync(
                    wuc::CoreDispatcherPriority::Normal,
                    wuc::DispatchedHandler([applyOnUi, handle = target.handle,
                                            generation = target.generation]() mutable {
                        applyOnUi(handle, generation);
                    }));
            } else {
                Wh_Log(L"No dispatcher for handle %llu",
                       static_cast<unsigned long long>(target.handle));
            }
        } catch (...) {
            Wh_Log(L"Dispatch weather UI failed %08X", winrt::to_hresult());
        }
    }

    UpdateAllDaylightUIs();
}

bool IsNotificationCenterGrid(wux::FrameworkElement const& element,
                              PCWSTR typeName) {
    try {
        if (element.Name() != L"NotificationCenterGrid") {
            return false;
        }
        auto className = winrt::get_class_name(element);
        if (className != L"Windows.UI.Xaml.Controls.Grid") {
            if (typeName && wcsstr(typeName, L"Grid") == nullptr) {
                return false;
            }
        }
        return element.try_as<wuxc::Grid>() != nullptr;
    } catch (...) {
        return false;
    }
}

wuxc::Border BuildWeatherRoot(WeatherUiControls& ui) {
    // Card chrome (acrylic, stroke, CornerRadius) is applied after mount by
    // copying CalendarCenterGrid — Border.CornerRadius reliably rounds its own
    // background, unlike Grid.CornerRadius with square child plates.
    wuxc::Border root;
    root.Name(L"WindhawkWeatherRoot");
    root.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    root.VerticalAlignment(wux::VerticalAlignment::Stretch);
    root.CornerRadius(wux::CornerRadius{
        kWeatherCardCornerRadius, kWeatherCardCornerRadius,
        kWeatherCardCornerRadius, kWeatherCardCornerRadius});
    root.Padding(wux::Thickness{12, 10, 12, 12});
    root.MinHeight(0);
    root.BorderThickness(wux::Thickness{1, 1, 1, 1});
    root.BorderBrush(wuxm::SolidColorBrush(
        winrt::Windows::UI::Color{48, 255, 255, 255}));
    root.Background(wuxm::SolidColorBrush(
        winrt::Windows::UI::Color{200, 32, 32, 32}));
    // Match ApplyCalendarMatchedChrome — keep chrome below parent top-clip.
    root.Margin(wux::Thickness{0, 3, 0, 0});

    wuxc::StackPanel outer;
    outer.Spacing(0);
    outer.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    outer.VerticalAlignment(wux::VerticalAlignment::Top);

    wuxc::TextBlock status;
    status.Text(L"Loading weather...");
    status.FontSize(13);
    status.Margin(wux::Thickness{0, 0, 0, 6});
    status.Visibility(wux::Visibility::Visible);
    ui.statusText = status;

    wuxc::StackPanel content;
    content.Spacing(0);
    content.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    ui.contentRoot = content;

    wuxc::Grid header;
    wuxc::ColumnDefinition leftCol;
    leftCol.Width(wux::GridLength{1, wux::GridUnitType::Star});
    wuxc::ColumnDefinition rightCol;
    rightCol.Width(wux::GridLength{0, wux::GridUnitType::Auto});
    header.ColumnDefinitions().Append(leftCol);
    header.ColumnDefinitions().Append(rightCol);

    wuxc::StackPanel leftStack;
    leftStack.Spacing(0);
    leftStack.VerticalAlignment(wux::VerticalAlignment::Top);

    wuxc::TextBlock location;
    location.FontSize(16);
    location.FontWeight(wut::FontWeights::SemiBold());
    location.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    location.TextWrapping(wux::TextWrapping::NoWrap);
    location.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    location.Text(L"");
    ui.locationText = location;

    wuxc::TextBlock temperature;
    temperature.FontSize(42);
    temperature.FontWeight(wut::FontWeights::Light());
    temperature.Margin(wux::Thickness{0, -4, 0, 0});
    temperature.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    temperature.Text(L"--\u00B0");
    ui.temperatureText = temperature;

    leftStack.Children().Append(location);
    leftStack.Children().Append(temperature);
    wuxc::Grid::SetColumn(leftStack, 0);

    // Right column: icon + condition + high/low as one stack, bottom-aligned
    // to the header so high/low shares a baseline with the large temperature
    // (matches the Berlin reference).
    wuxc::StackPanel rightStack;
    rightStack.Spacing(0);
    rightStack.HorizontalAlignment(wux::HorizontalAlignment::Right);
    rightStack.VerticalAlignment(wux::VerticalAlignment::Bottom);

    wuxc::Grid conditionIconHost;
    conditionIconHost.Width(36);
    conditionIconHost.Height(36);
    conditionIconHost.HorizontalAlignment(wux::HorizontalAlignment::Right);
    conditionIconHost.Margin(wux::Thickness{0, -2, 0, -2});
    ui.conditionIconHost = conditionIconHost;

    wuxc::TextBlock conditionText;
    conditionText.FontSize(13);
    conditionText.FontWeight(wut::FontWeights::Bold());
    conditionText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    conditionText.HorizontalAlignment(wux::HorizontalAlignment::Right);
    conditionText.Margin(wux::Thickness{0, 0, 0, 0});
    conditionText.Text(L"...");
    ui.conditionText = conditionText;

    wuxc::TextBlock highLowText;
    highLowText.FontSize(13);
    highLowText.FontWeight(wut::FontWeights::SemiBold());
    highLowText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    highLowText.HorizontalAlignment(wux::HorizontalAlignment::Right);
    highLowText.Text(L"--\u00B0  --\u00B0");
    ui.highLowText = highLowText;

    rightStack.Children().Append(conditionIconHost);
    rightStack.Children().Append(conditionText);
    rightStack.Children().Append(highLowText);
    wuxc::Grid::SetColumn(rightStack, 1);

    header.Children().Append(leftStack);
    header.Children().Append(rightStack);

    wuxc::Grid hourly;
    hourly.Height(72);
    hourly.Margin(wux::Thickness{0, 2, 0, 2});
    ui.hourlyPanel = hourly;

    wuxc::StackPanel daily;
    daily.Spacing(0);
    daily.Margin(wux::Thickness{0, 2, 0, 0});
    daily.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    ui.dailyPanel = daily;

    content.Children().Append(header);
    content.Children().Append(MakeDivider(root));
    content.Children().Append(hourly);
    content.Children().Append(MakeDivider(root));
    content.Children().Append(daily);

    outer.Children().Append(status);
    outer.Children().Append(content);
    root.Child(outer);

    try {
        wuxa::AutomationProperties::SetName(root, L"Weather forecast");
    } catch (...) {
    }

    return root;
}

bool IsWeatherInjectedRoot(wux::FrameworkElement const& fe) {
    try {
        return fe.Name() == L"WindhawkWeatherRoot";
    } catch (...) {
        return false;
    }
}

bool IsShellNotificationChromeName(std::wstring_view name) {
    return name == L"DoNotDisturbSubtext" || name == L"DoNotDisturbButton" ||
           name == L"NotificationCenterTopBanner" || name == L"ListContent" ||
           name == L"NotificationListView" ||
           name == L"FlexibleNormalToastView";
}

std::atomic<int> g_insideNativeCollapse{0};

void UnregisterChildVisibilityWatchers(MountedWeatherInstance& instance) {
    for (auto& record : instance.childVisibility) {
        if (record.visibilityCookie < 0) {
            continue;
        }
        if (auto element = record.element.get()) {
            try {
                element.UnregisterPropertyChangedCallback(
                    wux::UIElement::VisibilityProperty(),
                    record.visibilityCookie);
            } catch (...) {
            }
        }
        record.visibilityCookie = -1;
    }
}

// Shell re-shows DND / notification chrome after login or Focus Assist changes.
// Keep forcing Collapsed while weather owns the slot.
void SuppressNativeChild(wux::UIElement const& ui,
                         MountedWeatherInstance& instance) {
    if (!ui) {
        return;
    }
    try {
        if (auto fe = ui.try_as<wux::FrameworkElement>()) {
            if (IsWeatherInjectedRoot(fe)) {
                return;
            }
        }
    } catch (...) {
    }

    ChildVisibilityRecord* existing = nullptr;
    for (auto& record : instance.childVisibility) {
        if (auto el = record.element.get()) {
            if (el == ui) {
                existing = &record;
                break;
            }
        }
    }

    if (!existing) {
        ChildVisibilityRecord record;
        record.element = winrt::make_weak(ui);
        try {
            record.original = ui.Visibility();
        } catch (...) {
            record.original = wux::Visibility::Visible;
        }
        try {
            record.originalOpacity = ui.Opacity();
        } catch (...) {
            record.originalOpacity = 1.0;
        }
        instance.childVisibility.push_back(std::move(record));
        existing = &instance.childVisibility.back();
    }

    {
        g_insideNativeCollapse.fetch_add(1);
        try {
            ui.Visibility(wux::Visibility::Collapsed);
        } catch (...) {
        }
        g_insideNativeCollapse.fetch_sub(1);
    }

    if (existing->visibilityCookie < 0) {
        try {
            existing->visibilityCookie = ui.RegisterPropertyChangedCallback(
                wux::UIElement::VisibilityProperty(),
                wux::DependencyPropertyChangedCallback(
                    [](wux::DependencyObject const& sender,
                       wux::DependencyProperty const&) {
                        if (g_insideNativeCollapse.load() > 0 ||
                            g_shuttingDown.load()) {
                            return;
                        }
                        auto element = sender.try_as<wux::UIElement>();
                        if (!element) {
                            return;
                        }
                        try {
                            if (element.Visibility() ==
                                wux::Visibility::Collapsed) {
                                return;
                            }
                            if (auto fe =
                                    element.try_as<wux::FrameworkElement>()) {
                                if (IsWeatherInjectedRoot(fe)) {
                                    return;
                                }
                            }
                        } catch (...) {
                            return;
                        }
                        g_insideNativeCollapse.fetch_add(1);
                        try {
                            element.Visibility(wux::Visibility::Collapsed);
                            Wh_Log(L"Re-collapsed shell notification chrome "
                                   L"after Visibility restore");
                        } catch (...) {
                        }
                        g_insideNativeCollapse.fetch_sub(1);
                    }));
        } catch (...) {
            existing->visibilityCookie = -1;
        }
    }
}

void CollapseNamedShellChrome(wux::DependencyObject const& root,
                              MountedWeatherInstance& instance,
                              int depth = 0) {
    if (!root || depth > 12) {
        return;
    }
    try {
        if (auto fe = root.try_as<wux::FrameworkElement>()) {
            if (IsWeatherInjectedRoot(fe)) {
                return;
            }
            auto name = fe.Name();
            if (!name.empty() && IsShellNotificationChromeName(name)) {
                if (auto ui = fe.try_as<wux::UIElement>()) {
                    SuppressNativeChild(ui, instance);
                }
            }
        }
    } catch (...) {
    }

    try {
        const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int32_t i = 0; i < count; ++i) {
            CollapseNamedShellChrome(
                wuxm::VisualTreeHelper::GetChild(root, i), instance,
                depth + 1);
        }
    } catch (...) {
    }
}

void CollapseNativeChildren(wuxc::Grid const& grid,
                            MountedWeatherInstance& instance) {
    // Hide all native children — weather Border owns acrylic + rounded corners.
    // Also pin Visibility so Focus Assist / DND cannot re-expand the slot and
    // clip the weather header after a logoff/login.
    auto children = grid.Children();
    for (auto const& child : children) {
        auto fe = child.try_as<wux::FrameworkElement>();
        if (fe && IsWeatherInjectedRoot(fe)) {
            continue;
        }

        auto ui = child.try_as<wux::UIElement>();
        if (!ui) {
            continue;
        }
        SuppressNativeChild(ui, instance);
    }

    CollapseNamedShellChrome(grid, instance);

    // After logon, DoNotDisturbSubtext can sit as a sibling between
    // NotificationCenterGrid and CalendarCenterGrid.
    try {
        if (auto parent = wuxm::VisualTreeHelper::GetParent(grid)) {
            const int32_t count =
                wuxm::VisualTreeHelper::GetChildrenCount(parent);
            for (int32_t i = 0; i < count; ++i) {
                auto sibling = wuxm::VisualTreeHelper::GetChild(parent, i);
                if (auto fe = sibling.try_as<wux::FrameworkElement>()) {
                    auto name = fe.Name();
                    if (name == L"NotificationCenterGrid" ||
                        name == L"CalendarCenterGrid" ||
                        name == L"WindhawkWeatherRoot") {
                        continue;
                    }
                    if (IsShellNotificationChromeName(name)) {
                        if (auto ui = fe.try_as<wux::UIElement>()) {
                            SuppressNativeChild(ui, instance);
                        }
                    }
                }
                // Shallow scan only — avoid touching the calendar subtree.
                try {
                    const int32_t nested =
                        wuxm::VisualTreeHelper::GetChildrenCount(sibling);
                    for (int32_t j = 0; j < nested; ++j) {
                        auto child =
                            wuxm::VisualTreeHelper::GetChild(sibling, j);
                        if (auto fe = child.try_as<wux::FrameworkElement>()) {
                            if (IsShellNotificationChromeName(fe.Name())) {
                                if (auto ui = fe.try_as<wux::UIElement>()) {
                                    SuppressNativeChild(ui, instance);
                                }
                            }
                        }
                    }
                } catch (...) {
                }
            }
        }
    } catch (...) {
    }

    // New toasts resize the notification slot; re-pin content-sized Bottom
    // layout so the weather card cannot float to the top of a tall star row.
    try {
        EnforceWeatherNotificationLayout(grid, instance.weatherRoot.get());
    } catch (...) {
    }
}

void RestoreNativeChildren(MountedWeatherInstance& instance) {
    UnregisterChildVisibilityWatchers(instance);
    for (auto const& record : instance.childVisibility) {
        if (auto element = record.element.get()) {
            g_insideNativeCollapse.fetch_add(1);
            try {
                element.Visibility(record.original);
                element.Opacity(record.originalOpacity);
            } catch (...) {
            }
            g_insideNativeCollapse.fetch_sub(1);
        }
    }
    instance.childVisibility.clear();
}

void ForceShowNativeNotificationChildren(wuxc::Grid const& grid) {
    // Belt-and-suspenders: weak-ref restore can miss if the shell rebuilt
    // nodes, leaving Collapsed children and an empty acrylic slot.
    g_insideNativeCollapse.fetch_add(1);
    try {
        for (auto const& child : grid.Children()) {
            auto fe = child.try_as<wux::FrameworkElement>();
            if (fe && IsWeatherInjectedRoot(fe)) {
                continue;
            }
            if (auto ui = child.try_as<wux::UIElement>()) {
                ui.Visibility(wux::Visibility::Visible);
                if (ui.Opacity() <= 0.01) {
                    ui.Opacity(1.0);
                }
            }
        }
    } catch (...) {
    }
    g_insideNativeCollapse.fetch_sub(1);
}

void UnmountInstance(MountedWeatherInstance& instance) {
    Wh_Log(L"Unmounting weather instance handle=%llu thread=%u",
           static_cast<unsigned long long>(instance.gridHandle),
           instance.uiThreadId);
    try {
        auto grid = instance.notificationGrid.get();
        auto root = instance.weatherRoot.get();

        RestoreNativeChildren(instance);

        if (grid) {
            instance.weatherSizeChangedRevoker = {};
            instance.gridSizeChangedRevoker = {};
            instance.gridThemeChangedRevoker = {};
            instance.weatherThemeChangedRevoker = {};
            instance.weatherViewportRevoker = {};
            try {
                if (instance.backgroundChangedCookie >= 0) {
                    grid.UnregisterPropertyChangedCallback(
                        wuxc::Panel::BackgroundProperty(),
                        instance.backgroundChangedCookie);
                    instance.backgroundChangedCookie = -1;
                }
                if (instance.shadowChangedCookie >= 0) {
                    grid.UnregisterPropertyChangedCallback(
                        wux::UIElement::ShadowProperty(),
                        instance.shadowChangedCookie);
                    instance.shadowChangedCookie = -1;
                }
                if (instance.gridVisibilityCookie >= 0) {
                    grid.UnregisterPropertyChangedCallback(
                        wux::UIElement::VisibilityProperty(),
                        instance.gridVisibilityCookie);
                    instance.gridVisibilityCookie = -1;
                }
                if (instance.weatherVisibilityCookie >= 0 && root) {
                    root.UnregisterPropertyChangedCallback(
                        wux::UIElement::VisibilityProperty(),
                        instance.weatherVisibilityCookie);
                    instance.weatherVisibilityCookie = -1;
                }
            } catch (...) {
            }
            ClearCompositionClip(grid);

            try {
                auto children = grid.Children();
                for (int32_t i = static_cast<int32_t>(children.Size()) - 1;
                     i >= 0; --i) {
                    auto child = children.GetAt(static_cast<uint32_t>(i));
                    auto fe = child.try_as<wux::FrameworkElement>();
                    if (!fe) {
                        continue;
                    }
                    if (fe.Name() == L"WindhawkWeatherRoot" ||
                        (root && child == root)) {
                        ClearCompositionClip(fe);
                        children.RemoveAt(static_cast<uint32_t>(i));
                    }
                }
            } catch (...) {
                Wh_Log(L"Remove weather root error %08X", winrt::to_hresult());
            }

            RestoreLocalProperties(grid, instance.savedProperties);
            ForceShowNativeNotificationChildren(grid);
        } else if (root) {
            instance.weatherSizeChangedRevoker = {};
            instance.gridThemeChangedRevoker = {};
            instance.weatherThemeChangedRevoker = {};
            if (auto parent = root.Parent().try_as<wuxc::Panel>()) {
                uint32_t index = 0;
                if (parent.Children().IndexOf(root, index)) {
                    parent.Children().RemoveAt(index);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"UnmountInstance error %08X", winrt::to_hresult());
    }
    instance.weatherRoot = nullptr;
    instance.notificationGrid = nullptr;
    instance.ui = {};
    instance.savedProperties.clear();
    instance.dispatcherQueue = nullptr;
    instance.coreDispatcher = nullptr;
    instance.uiThreadId = 0;
}

void ConfigureNotificationGrid(wuxc::Grid const& grid,
                               MountedWeatherInstance& instance,
                               ModSettings const& settings) {
    instance.savedProperties.clear();
    SaveLocalProperty(grid, wux::FrameworkElement::HeightProperty(),
                      instance.savedProperties);
    SaveLocalProperty(grid, wux::FrameworkElement::MinHeightProperty(),
                      instance.savedProperties);
    SaveLocalProperty(grid, wux::FrameworkElement::MaxHeightProperty(),
                      instance.savedProperties);
    SaveLocalProperty(grid, wux::FrameworkElement::VerticalAlignmentProperty(),
                      instance.savedProperties);
    SaveLocalProperty(grid, wux::FrameworkElement::MarginProperty(),
                      instance.savedProperties);

    try {
        // Layout strategy (final):
        // - Do NOT force a large fixed Height. That either clips the top
        //   (Bottom align) or leaves a huge gap above the calendar (Top align).
        // - Let the weather Border size to its content; pin the grid to the
        //   bottom of the notification slot with a small gap above the calendar.
        // - Cap MaxHeight so we never overflow the flyout and clip corners.
        // Size to content; pin to bottom of the notification slot.
        // Do not oversize MaxHeight — a taller bottom-aligned child is what
        // gets its top edge clipped by the parent.
        const double contentHeight = EstimateWeatherContentHeight(settings);
        const double maxHeight = (std::max)(
            contentHeight,
            static_cast<double>(settings.cardHeight));

        grid.ClearValue(wux::FrameworkElement::HeightProperty());
        grid.ClearValue(wux::FrameworkElement::MinHeightProperty());
        grid.MaxHeight(maxHeight);
        grid.VerticalAlignment(wux::VerticalAlignment::Bottom);
        auto margin = grid.Margin();
        // Keep grid margins tight; the card itself has the anti-clip top inset.
        margin.Top = 0;
        margin.Bottom = 4;
        grid.Margin(margin);
        grid.Visibility(wux::Visibility::Visible);
        grid.Clip(nullptr);
        ClearCompositionClip(grid);
        ClearAncestorClips(grid, 8);
        // Same constraints SizeChanged will keep fighting for after toasts.
        if (auto root = instance.weatherRoot.get()) {
            EnforceWeatherNotificationLayout(grid, root);
        }

        Wh_Log(L"Weather layout: auto-height, max=%g, align=Bottom",
               maxHeight);
    } catch (...) {
        Wh_Log(L"ConfigureNotificationGrid error %08X", winrt::to_hresult());
    }
}

bool MountWeatherIntoGrid(InstanceHandle handle, wuxc::Grid const& grid) {
    if (g_shuttingDown.load()) {
        return false;
    }

    auto settings = GetSettingsCopy();

    // Idempotency: existing injected root?
    for (auto const& child : grid.Children()) {
        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == L"WindhawkWeatherRoot") {
                Wh_Log(L"WindhawkWeatherRoot already present");
                std::lock_guard lock(g_mountMutex);
                for (auto& mounted : g_mounted) {
                    if (mounted.gridHandle == handle) {
                        // Theme changes / Focus Assist can restore DND chrome
                        // and opaque NotificationCenterGrid while closed.
                        CollapseNativeChildren(grid, mounted);
                        if (auto border = fe.try_as<wuxc::Border>()) {
                            EnforceWeatherNotificationLayout(grid, border);
                            ApplyCalendarMatchedChrome(mounted, grid, border);
                        }
                        break;
                    }
                }
                // Flyout reopened without remounting: hourly slice + daylight
                // "now" both need a check (RefreshDaylight also requests weather).
                try {
                    RefreshDaylightOnFlyoutShown();
                } catch (...) {
                }
                try {
                    ApplyOrRestoreAllCalendarLaunch();
                } catch (...) {
                }
                try {
                    ApplyAllCalendarDayShapes();
                } catch (...) {
                }
                return true;
            }
        }
    }

    MountedWeatherInstance instance;
    instance.gridHandle = handle;
    instance.uiThreadId = GetCurrentThreadId();
    instance.notificationGrid = winrt::make_weak(grid);
    instance.generation = g_uiGeneration.load();
    try {
        instance.dispatcherQueue = ws::DispatcherQueue::GetForCurrentThread();
    } catch (...) {
        Wh_Log(L"DispatcherQueue::GetForCurrentThread failed");
    }
    try {
        if (auto coreWindow = wuc::CoreWindow::GetForCurrentThread()) {
            instance.coreDispatcher = coreWindow.Dispatcher();
        }
    } catch (...) {
        Wh_Log(L"CoreWindow::GetForCurrentThread failed");
    }

    WeatherUiControls ui;
    auto root = BuildWeatherRoot(ui);
    instance.ui = ui;
    instance.weatherRoot = winrt::make_weak(root);

    ConfigureNotificationGrid(grid, instance, settings);
    root.Visibility(wux::Visibility::Visible);

    try {
        grid.Children().Append(root);
    } catch (...) {
        Wh_Log(L"Failed to append weather root %08X", winrt::to_hresult());
        RestoreLocalProperties(grid, instance.savedProperties);
        return false;
    }

    CollapseNativeChildren(grid, instance);
    ApplyCalendarMatchedChrome(instance, grid, root);

    {
        std::lock_guard lock(g_mountMutex);
        g_mounted.push_back(std::move(instance));
    }

    Wh_Log(L"Mounted weather into NotificationCenterGrid");

    ForecastData forecast;
    {
        std::lock_guard lock(g_forecastMutex);
        forecast = g_forecast;
    }
    {
        // recursive_mutex: visual-tree callbacks may re-enter safely.
        std::lock_guard lock(g_mountMutex);
        if (!g_mounted.empty()) {
            ApplyForecastToInstance(g_mounted.back(), forecast, settings);
        }
    }
    RequestWeatherRefresh(false);
    EnsureRefreshTimer();
    try {
        ApplyOrRestoreAllCalendarLaunch();
    } catch (...) {
    }
    try {
        ApplyAllCalendarDayShapes();
    } catch (...) {
    }
    return true;
}

void ApplyOrRestoreAllMounted() {
    auto settings = GetSettingsCopy();
    bool needRefresh = false;
    {
        std::lock_guard lock(g_mountMutex);
        for (auto& mounted : g_mounted) {
            auto grid = mounted.notificationGrid.get();
            if (!grid) {
                continue;
            }
            try {
                if (auto root = mounted.weatherRoot.get()) {
                    root.Visibility(wux::Visibility::Visible);
                }
                if (mounted.savedProperties.empty()) {
                    ConfigureNotificationGrid(grid, mounted, settings);
                } else {
                    const double contentHeight =
                        EstimateWeatherContentHeight(settings);
                    const double maxHeight = (std::max)(
                        contentHeight,
                        static_cast<double>(settings.cardHeight));
                    grid.ClearValue(
                        wux::FrameworkElement::HeightProperty());
                    grid.ClearValue(
                        wux::FrameworkElement::MinHeightProperty());
                    grid.MaxHeight(maxHeight);
                    grid.VerticalAlignment(
                        wux::VerticalAlignment::Bottom);
                    auto margin = grid.Margin();
                    margin.Top = 0;
                    margin.Bottom = 4;
                    grid.Margin(margin);
                    grid.Clip(nullptr);
                    ClearCompositionClip(grid);
                    ClearAncestorClips(grid, 8);
                }
                CollapseNativeChildren(grid, mounted);
                if (auto root = mounted.weatherRoot.get()) {
                    ApplyCalendarMatchedChrome(mounted, grid, root);
                }
                needRefresh = true;
            } catch (...) {
                Wh_Log(L"ApplyOrRestoreAllMounted error %08X",
                       winrt::to_hresult());
            }
        }
    }
    if (needRefresh) {
        RequestWeatherRefresh(false);
    }
}

void CleanupMountedOnCurrentThread() {
    const DWORD threadId = GetCurrentThreadId();
    std::vector<MountedWeatherInstance> local;
    std::vector<MountedDaylightInstance> localDaylight;
    std::vector<MountedCalendarLaunchInstance> localLaunch;
    {
        std::lock_guard lock(g_mountMutex);
        for (auto it = g_mounted.begin(); it != g_mounted.end();) {
            if (it->uiThreadId == threadId || it->uiThreadId == 0) {
                local.push_back(std::move(*it));
                it = g_mounted.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = g_daylightMounted.begin();
             it != g_daylightMounted.end();) {
            if (it->uiThreadId == threadId || it->uiThreadId == 0) {
                localDaylight.push_back(std::move(*it));
                it = g_daylightMounted.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = g_calendarLaunchMounted.begin();
             it != g_calendarLaunchMounted.end();) {
            if (it->uiThreadId == threadId || it->uiThreadId == 0) {
                localLaunch.push_back(std::move(*it));
                it = g_calendarLaunchMounted.erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto& instance : local) {
        UnmountInstance(instance);
    }
    for (auto& instance : localDaylight) {
        UnmountDaylightInstance(instance);
    }
    for (auto& instance : localLaunch) {
        UnmountCalendarLaunchInstance(instance);
    }
}

// Run work on the UI dispatcher that owned the mount, waiting briefly so
// Wh_ModUninit does not drop mounts without restoring the notification list.
bool RunOnMountDispatcher(ws::DispatcherQueue const& dispatcherQueue,
                          wuc::CoreDispatcher const& coreDispatcher,
                          DWORD uiThreadId,
                          std::function<void()> const& fn) {
    if (uiThreadId != 0 && uiThreadId == GetCurrentThreadId()) {
        fn();
        return true;
    }

    // No dispatcher and not on the UI thread — do not touch XAML off-thread.
    if (!dispatcherQueue && !coreDispatcher) {
        Wh_Log(L"Dispatcher unmount skipped — no dispatcher available");
        return false;
    }

    HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!done) {
        Wh_Log(L"Dispatcher unmount skipped — CreateEvent failed");
        return false;
    }

    struct State {
        std::function<void()> fn;
        std::shared_ptr<void> doneHolder;
        std::atomic<bool> ran{false};
    };
    auto state = std::make_shared<State>();
    state->fn = fn;
    state->doneHolder = std::shared_ptr<void>(done, &::CloseHandle);

    auto invoke = [state]() {
        if (state->ran.exchange(true)) {
            return;
        }
        try {
            if (state->fn) {
                state->fn();
            }
        } catch (...) {
        }
        if (auto holder = state->doneHolder) {
            SetEvent(static_cast<HANDLE>(holder.get()));
        }
    };

    bool queued = false;
    try {
        if (dispatcherQueue) {
            queued = dispatcherQueue.TryEnqueue(
                ws::DispatcherQueueHandler(invoke));
        }
    } catch (...) {
        queued = false;
    }
    if (!queued) {
        try {
            if (coreDispatcher) {
                coreDispatcher.RunAsync(wuc::CoreDispatcherPriority::High,
                                        wuc::DispatchedHandler(invoke));
                queued = true;
            }
        } catch (...) {
            queued = false;
        }
    }

    if (!queued) {
        state->doneHolder.reset();
        Wh_Log(L"Dispatcher unmount skipped — enqueue failed");
        return false;
    }

    // Keep uninit snappy — never fall back to off-thread XAML after timeout.
    // Retain a local ref so CloseHandle cannot race WaitForSingleObject.
    auto waitHolder = state->doneHolder;
    const DWORD wait = WaitForSingleObject(done, 750);
    state->doneHolder.reset();
    waitHolder.reset();
    if (wait != WAIT_OBJECT_0 && !state->ran.load()) {
        Wh_Log(L"Dispatcher unmount timed out — abandoning (not off-thread)");
    }
    return state->ran.load();
}

void UnmountAllMountedSynchronously() {
    std::vector<MountedWeatherInstance> weather;
    std::vector<MountedDaylightInstance> daylight;
    std::vector<MountedCalendarLaunchInstance> launch;
    {
        std::lock_guard lock(g_mountMutex);
        weather.swap(g_mounted);
        daylight.swap(g_daylightMounted);
        launch.swap(g_calendarLaunchMounted);
    }

    std::vector<winrt::weak_ref<wuxc::Grid>> dayChromeRoots;
    ws::DispatcherQueue dq{nullptr};
    wuc::CoreDispatcher cd{nullptr};
    DWORD tid = 0;
    for (auto const& w : weather) {
        if (w.notificationGrid) {
            dayChromeRoots.push_back(w.notificationGrid);
        }
        if (!dq && !cd && (w.dispatcherQueue || w.coreDispatcher)) {
            dq = w.dispatcherQueue;
            cd = w.coreDispatcher;
            tid = w.uiThreadId;
        }
    }
    for (auto const& d : daylight) {
        if (d.calendarSection) {
            dayChromeRoots.push_back(d.calendarSection);
        }
        if (!dq && !cd && (d.dispatcherQueue || d.coreDispatcher)) {
            dq = d.dispatcherQueue;
            cd = d.coreDispatcher;
            tid = d.uiThreadId;
        }
    }

    // Restore day chrome on the UI thread only. Never fall back to inline
    // off-thread XAML (that hung/broke the clock flyout after timeouts).
    if (tid != 0 && tid == GetCurrentThreadId()) {
        try {
            RestoreAllCalendarDayShapes();
            ClearCalendarDayChromeNearRoots(dayChromeRoots);
        } catch (...) {
        }
    } else if (dq || cd) {
        HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        auto ran = std::make_shared<std::atomic<bool>>(false);
        auto work = [ran, dayChromeRoots]() {
            if (ran->exchange(true)) {
                return;
            }
            try {
                RestoreAllCalendarDayShapes();
                ClearCalendarDayChromeNearRoots(dayChromeRoots);
            } catch (...) {
            }
        };
        bool queued = false;
        try {
            if (dq) {
                queued = dq.TryEnqueue(ws::DispatcherQueueHandler([work, done]() {
                    work();
                    if (done) {
                        SetEvent(done);
                    }
                }));
            }
        } catch (...) {
            queued = false;
        }
        if (!queued) {
            try {
                if (cd) {
                    cd.RunAsync(wuc::CoreDispatcherPriority::High,
                                wuc::DispatchedHandler([work, done]() {
                                    work();
                                    if (done) {
                                        SetEvent(done);
                                    }
                                }));
                    queued = true;
                }
            } catch (...) {
                queued = false;
            }
        }
        if (queued && done) {
            WaitForSingleObject(done, 750);
        }
        if (done) {
            CloseHandle(done);
        }
        // If the dispatcher never ran, drop patches without touching XAML
        // off-thread — safer than corrupting the shell calendar.
        if (!ran->load()) {
            std::lock_guard lock(g_mountMutex);
            g_calendarDayShapePatches.clear();
            Wh_Log(L"Skipped day-chrome restore (dispatcher did not run)");
        }
    } else {
        std::lock_guard lock(g_mountMutex);
        g_calendarDayShapePatches.clear();
    }

    for (auto& instance : weather) {
        auto instanceDq = instance.dispatcherQueue;
        auto instanceCd = instance.coreDispatcher;
        auto instanceTid = instance.uiThreadId;
        RunOnMountDispatcher(instanceDq, instanceCd, instanceTid,
                             [&instance]() { UnmountInstance(instance); });
    }
    for (auto& instance : daylight) {
        auto instanceDq = instance.dispatcherQueue;
        auto instanceCd = instance.coreDispatcher;
        auto instanceTid = instance.uiThreadId;
        RunOnMountDispatcher(instanceDq, instanceCd, instanceTid, [&instance]() {
            UnmountDaylightInstance(instance);
        });
    }
    for (auto& instance : launch) {
        auto launchDq = instance.dispatcherQueue;
        auto launchCd = instance.coreDispatcher;
        auto launchTid = instance.uiThreadId;
        if (!launchDq && !launchCd) {
            launchDq = dq;
            launchCd = cd;
            if (!launchTid) {
                launchTid = tid;
            }
        }
        RunOnMountDispatcher(launchDq, launchCd, launchTid, [&instance]() {
            UnmountCalendarLaunchInstance(instance);
        });
    }
}

void HandleVisualTreeAdd(InstanceHandle handle,
                         wux::FrameworkElement const& element,
                         PCWSTR typeName) {
    try {
        if (IsNotificationCenterGrid(element, typeName)) {
            if (auto grid = element.try_as<wuxc::Grid>()) {
                Wh_Log(L"NotificationCenterGrid added — scheduling mount");
                // Defer mutation out of the visual-tree Add callback itself.
                bool scheduled = false;
                try {
                    if (auto dq = ws::DispatcherQueue::GetForCurrentThread()) {
                        scheduled = dq.TryEnqueue(
                            ws::DispatcherQueueHandler([handle, grid]() {
                                if (g_shuttingDown.load()) {
                                    return;
                                }
                                try {
                                    MountWeatherIntoGrid(handle, grid);
                                } catch (...) {
                                    Wh_Log(L"Deferred mount error %08X",
                                           winrt::to_hresult());
                                }
                            }));
                    }
                } catch (...) {
                }
                if (!scheduled) {
                    try {
                        if (auto cw = wuc::CoreWindow::GetForCurrentThread()) {
                            cw.Dispatcher().RunAsync(
                                wuc::CoreDispatcherPriority::Normal,
                                wuc::DispatchedHandler([handle, grid]() {
                                    if (g_shuttingDown.load()) {
                                        return;
                                    }
                                    try {
                                        MountWeatherIntoGrid(handle, grid);
                                    } catch (...) {
                                        Wh_Log(L"Deferred mount error %08X",
                                               winrt::to_hresult());
                                    }
                                }));
                            scheduled = true;
                        }
                    } catch (...) {
                    }
                }
                if (!scheduled) {
                    MountWeatherIntoGrid(handle, grid);
                }
            }
            return;
        }

        if (IsCalendarSection(element, typeName)) {
            if (auto section = element.try_as<wuxc::Grid>()) {
                Wh_Log(L"CalendarSection added — scheduling daylight / "
                       L"calendar-launch / day-shape");
                bool scheduled = false;
                try {
                    if (auto dq = ws::DispatcherQueue::GetForCurrentThread()) {
                        scheduled = dq.TryEnqueue(
                            ws::DispatcherQueueHandler([handle, section]() {
                                if (g_shuttingDown.load()) {
                                    return;
                                }
                                try {
                                    MountDaylightIntoCalendarSection(handle,
                                                                     section);
                                } catch (...) {
                                    Wh_Log(L"Deferred daylight mount error %08X",
                                           winrt::to_hresult());
                                }
                                try {
                                    if (!MountCalendarLaunchButton(handle,
                                                                   section)) {
                                        ScheduleCalendarLaunchMountRetry(
                                            handle, section, 0);
                                    }
                                } catch (...) {
                                    Wh_Log(L"Deferred calendar-launch mount "
                                           L"error %08X",
                                           winrt::to_hresult());
                                }
                                try {
                                    ApplyCalendarDayShapeForSection(section);
                                } catch (...) {
                                    Wh_Log(L"Deferred calendar day shape "
                                           L"error %08X",
                                           winrt::to_hresult());
                                }
                            }));
                    }
                } catch (...) {
                }
                if (!scheduled) {
                    try {
                        MountDaylightIntoCalendarSection(handle, section);
                    } catch (...) {
                    }
                    try {
                        if (!MountCalendarLaunchButton(handle, section)) {
                            ScheduleCalendarLaunchMountRetry(handle, section,
                                                             0);
                        }
                    } catch (...) {
                    }
                    try {
                        ApplyCalendarDayShapeForSection(section);
                    } catch (...) {
                    }
                }
            }
            return;
        }

        // Expand/collapse often materializes after CalendarSection — mount then.
        // Skip when the chevron already lives in our host (Add notifications
        // during minimize would otherwise remount and stutter the animation).
        try {
            if (element.Name() == L"ExpandCollapseButton") {
                bool alreadyHosted = false;
                try {
                    if (auto parentFe =
                            wuxm::VisualTreeHelper::GetParent(element)
                                .try_as<wux::FrameworkElement>()) {
                        alreadyHosted =
                            parentFe.Name() == kCalendarLaunchHostName;
                    }
                } catch (...) {
                }
                if (!alreadyHosted) {
                    wux::DependencyObject walk = element;
                    for (int depth = 0; depth < 10 && walk; ++depth) {
                        auto fe = walk.try_as<wux::FrameworkElement>();
                        if (fe && fe.Name() == L"CalendarSection") {
                            if (auto section = fe.try_as<wuxc::Grid>()) {
                                wuxc::StackPanel host{nullptr};
                                wuxc::Button launch{nullptr};
                                wuxc::Button expand{nullptr};
                                if (TryGetHealthyLaunchHost(section, host,
                                                            launch, expand)) {
                                    break;
                                }
                                // Defer — never mutate the tree inside the
                                // visual-tree Add callback.
                                bool scheduled = false;
                                try {
                                    if (auto dq =
                                            ws::DispatcherQueue::
                                                GetForCurrentThread()) {
                                        scheduled = dq.TryEnqueue(
                                            ws::DispatcherQueueHandler(
                                                [section]() {
                                                    if (g_shuttingDown
                                                            .load()) {
                                                        return;
                                                    }
                                                    try {
                                                        if (!MountCalendarLaunchButton(
                                                                0, section)) {
                                                            ScheduleCalendarLaunchMountRetry(
                                                                0, section, 0);
                                                        }
                                                    } catch (...) {
                                                    }
                                                }));
                                    }
                                } catch (...) {
                                }
                                if (!scheduled) {
                                    try {
                                        if (!MountCalendarLaunchButton(
                                                0, section)) {
                                            ScheduleCalendarLaunchMountRetry(
                                                0, section, 0);
                                        }
                                    } catch (...) {
                                    }
                                }
                            }
                            break;
                        }
                        walk = wuxm::VisualTreeHelper::GetParent(walk);
                    }
                }
            }
        } catch (...) {
        }

        // If a native child is re-added under a mounted notification grid,
        // collapse it so the weather card keeps the slot (DND banner after
        // logon is the common case).
        wuxc::Grid notificationParent{nullptr};
        try {
            auto name = element.Name();
            if (IsShellNotificationChromeName(name)) {
                wux::DependencyObject walk = element;
                for (int depth = 0; depth < 10 && walk; ++depth) {
                    auto fe = walk.try_as<wux::FrameworkElement>();
                    if (fe && fe.Name() == L"NotificationCenterGrid") {
                        notificationParent = fe.try_as<wuxc::Grid>();
                        break;
                    }
                    walk = wuxm::VisualTreeHelper::GetParent(walk);
                }
            }
        } catch (...) {
        }
        if (!notificationParent) {
            auto parent = element.Parent().try_as<wuxc::Grid>();
            if (parent && parent.Name() == L"NotificationCenterGrid") {
                notificationParent = parent;
            }
        }
        if (!notificationParent) {
            return;
        }
        if (element.Name() == L"WindhawkWeatherRoot") {
            return;
        }

        std::lock_guard lock(g_mountMutex);
        for (auto& mounted : g_mounted) {
            auto grid = mounted.notificationGrid.get();
            if (grid && grid == notificationParent) {
                CollapseNativeChildren(notificationParent, mounted);
                break;
            }
        }
    } catch (...) {
        Wh_Log(L"HandleVisualTreeAdd error %08X", winrt::to_hresult());
    }
}

void HandleVisualTreeRemove(InstanceHandle handle) {
    try {
        std::lock_guard lock(g_mountMutex);
        for (auto it = g_mounted.begin(); it != g_mounted.end(); ++it) {
            if (it->gridHandle == handle) {
                Wh_Log(L"NotificationCenterGrid removed, dropping instance");
                // Tree is going away; avoid touching dead elements.
                UnregisterChildVisibilityWatchers(*it);
                it->weatherRoot = nullptr;
                it->notificationGrid = nullptr;
                it->ui = {};
                it->childVisibility.clear();
                it->savedProperties.clear();
                g_mounted.erase(it);
                break;
            }
        }
        for (auto it = g_daylightMounted.begin();
             it != g_daylightMounted.end();) {
            bool drop = it->sectionHandle == handle;
            if (!drop) {
                try {
                    drop = !it->calendarSection.get();
                } catch (...) {
                    drop = true;
                }
            }
            if (drop) {
                Wh_Log(L"Dropping daylight instance");
                it->daylightRoot = nullptr;
                it->calendarSection = nullptr;
                it->ui = {};
                it = g_daylightMounted.erase(it);
                if (it == g_daylightMounted.end()) {
                    break;
                }
                // Continue pruning other dead entries; don't advance twice.
                continue;
            }
            ++it;
        }
        for (auto it = g_calendarLaunchMounted.begin();
             it != g_calendarLaunchMounted.end();) {
            bool drop = it->sectionHandle == handle;
            if (!drop) {
                try {
                    drop = !it->calendarSection.get() &&
                           !it->launchButton.get();
                } catch (...) {
                    drop = true;
                }
            }
            if (drop) {
                Wh_Log(L"Dropping calendar launch instance");
                it->clickRevoker = {};
                it->pointerEnteredRevoker = {};
                it->pointerPressedRevoker = {};
                it->pointerReleasedRevoker = {};
                it->pointerExitedRevoker = {};
                it->tappedRevoker = {};
                it->parent = nullptr;
                it->host = nullptr;
                it->launchButton = nullptr;
                it->expandButton = nullptr;
                it->hoverFill = nullptr;
                it->templateFill = nullptr;
                it->templateStroke = nullptr;
                it->templatePressed = nullptr;
                it->calendarSection = nullptr;
                it = g_calendarLaunchMounted.erase(it);
                continue;
            }
            ++it;
        }
    } catch (...) {
        Wh_Log(L"HandleVisualTreeRemove error %08X", winrt::to_hresult());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window-thread helpers

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                            cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

bool RunFromWindowThreadViaPostMessage(HWND hWnd,
                                       RunFromWindowThreadProc_t proc,
                                       PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsgViaPostMessage =
        RegisterWindowMessage(
            L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = reinterpret_cast<MSG*>(lParam);
                if (msg->message ==
                    runFromWindowThreadRegisteredMsgViaPostMessage) {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                            msg->lParam);
                    if (param) {
                        param->proc(param->procParam);
                        UnhookWindowsHookEx(param->hook);
                        delete param;
                        msg->lParam = 0;
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{proc, procParam, hook};
    if (!PostMessage(hWnd, runFromWindowThreadRegisteredMsgViaPostMessage, 0,
                     reinterpret_cast<LPARAM>(param))) {
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }
    return true;
}

std::vector<HWND> GetCoreWnds() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param{&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& param = *reinterpret_cast<ENUM_WINDOWS_PARAM*>(lParam);
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[64];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            switch (g_target) {
                case Target::Explorer:
                    break;
                case Target::ShellExperienceHost:
                    if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") ==
                        0) {
                        param.hWnds->push_back(hWnd);
                    }
                    break;
                case Target::ShellHost:
                    if (_wcsicmp(szClassName, L"ControlCenterWindow") == 0) {
                        param.hWnds->push_back(hWnd);
                    }
                    break;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));
    return hWnds;
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }
    Wh_Log(L"InitializeForCurrentThread %u", GetCurrentThreadId());
    g_initializedForThread = true;
}

void UninitializeForCurrentThread() {
    Wh_Log(L"UninitializeForCurrentThread %u", GetCurrentThreadId());
    CleanupMountedOnCurrentThread();
    g_initializedForThread = false;
}

void InitializeSettingsAndTap() {
    if (g_initialized.exchange(true)) {
        return;
    }
    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"InjectWindhawkTAP error %08X", hr);
        g_initialized = false;
    } else {
        Wh_Log(L"InjectWindhawkTAP succeeded");
    }
}

void UninitializeSettingsAndTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }
    g_initialized = false;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    BOOL bTextualClassName =
        ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    switch (g_target) {
        case Target::Explorer:
            break;
        case Target::ShellExperienceHost:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                Wh_Log(L"Initializing - Created core window: %08X via %S",
                       (DWORD)(ULONG_PTR)hWnd, funcName);
                InitializeForCurrentThread();
                InitializeSettingsAndTap();
            }
            break;
        case Target::ShellHost:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"ControlCenterWindow") == 0) {
                Wh_Log(L"Initializing - Created ControlCenterWindow: %08X via "
                       L"%S",
                       (DWORD)(ULONG_PTR)hWnd, funcName);
                RunFromWindowThreadViaPostMessage(
                    hWnd,
                    [](PVOID) {
                        InitializeForCurrentThread();
                        InitializeSettingsAndTap();
                    },
                    nullptr);
            }
            break;
    }
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    }
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
                                             LPCWSTR lpClassName,
                                             LPCWSTR lpWindowName,
                                             DWORD dwStyle,
                                             int X,
                                             int Y,
                                             int nWidth,
                                             int nHeight,
                                             HWND hWndParent,
                                             HMENU hMenu,
                                             HINSTANCE hInstance,
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
                                      LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName,
                                      DWORD dwStyle,
                                      int X,
                                      int Y,
                                      int nWidth,
                                      int nHeight,
                                      HWND hWndParent,
                                      HMENU hMenu,
                                      HINSTANCE hInstance,
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    }
    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L"> Windows 11 Calendar Weather init");
    g_shuttingDown = false;
    g_target = Target::ShellExperienceHost;

    WCHAR moduleFilePath[MAX_PATH];
    switch (GetModuleFileName(nullptr, moduleFilePath,
                              ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;
        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"explorer.exe") == 0) {
                    g_target = Target::Explorer;
                } else if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                } else if (_wcsicmp(moduleFileName,
                                   L"ShellExperienceHost.exe") != 0) {
                    Wh_Log(L"Unsupported module %s", moduleFileName);
                    return FALSE;
                }
            } else {
                Wh_Log(L"Unsupported module path");
                return FALSE;
            }
            break;
    }

    // explorer.exe only brokers calendar-app launches out of the flyout host.
    // Always start the broker — gating on settings left FindWindow failing
    // when explorer loaded before the path was visible to this process.
    if (g_target == Target::Explorer) {
        return StartCalendarLaunchBroker() ? TRUE : FALSE;
    }

    LoadSettings();

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        void* pCreateWindowInBand =
            (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
        void* pCreateWindowInBandEx =
            (void*)GetProcAddress(user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            Wh_SetFunctionHook(pCreateWindowInBandEx,
                               (void*)CreateWindowInBandEx_Hook,
                               (void**)&CreateWindowInBandEx_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"> Wh_ModAfterInit");
    if (g_target == Target::Explorer) {
        return;
    }
    bool initialize = false;
    for (auto hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }
    if (initialize) {
        InitializeSettingsAndTap();
    }
    // Timer only here — the first open/mount triggers the initial fetch.
    // Requesting refresh here raced with mount and could cancel the in-flight
    // result (generation bump), leaving the card stuck on "Loading...".
    EnsureRefreshTimer();
    EnsureDaylightTickTimer();
    RegisterSuspendResumeRefresh();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> Wh_ModSettingsChanged");
    if (g_shuttingDown.load() || g_target == Target::Explorer) {
        return;
    }

    ModSettings previous;
    {
        std::lock_guard lock(g_settingsMutex);
        previous = g_settings;
    }

    LoadSettings();
    auto current = GetSettingsCopy();

    bool locationChanged =
        previous.autoLocation != current.autoLocation ||
        previous.locationName != current.locationName ||
        previous.latitude != current.latitude ||
        previous.longitude != current.longitude ||
        previous.coordinatesValid != current.coordinatesValid ||
        previous.temperatureUnit != current.temperatureUnit ||
        previous.hourlyCount != current.hourlyCount ||
        previous.dailyCount != current.dailyCount;

    if (locationChanged) {
        {
            std::lock_guard lock(g_forecastMutex);
            g_forecast = {};
            if (previous.autoLocation != current.autoLocation ||
                previous.locationName != current.locationName ||
                previous.latitude != current.latitude ||
                previous.longitude != current.longitude ||
                previous.coordinatesValid != current.coordinatesValid) {
                g_resolvedLocation = {};
                g_resolvedKey.clear();
            }
        }
        ++g_fetchGeneration;
    }

    ++g_uiGeneration;
    EnsureRefreshTimer();
    EnsureDaylightTickTimer();

    auto coreWnds = GetCoreWnds();
    Wh_Log(L"Settings apply: daylight=%d roundedDays=%d calBtn=%d coreWnds=%zu",
           current.showCalendarDaylight ? 1 : 0,
           current.roundedDayMarkers ? 1 : 0,
           current.showCalendarAppButton ? 1 : 0, coreWnds.size());

    for (auto hCoreWnd : coreWnds) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                try {
                    ApplyOrRestoreAllMounted();
                    ApplyOrRestoreAllDaylight();
                    ApplyOrRestoreAllCalendarLaunch();
                    ApplyAllCalendarDayShapes();
                    ForecastData forecast;
                    {
                        std::lock_guard lock(g_forecastMutex);
                        forecast = g_forecast;
                    }
                    auto settings = GetSettingsCopy();
                    const DWORD threadId = GetCurrentThreadId();
                    std::lock_guard lock(g_mountMutex);
                    for (auto& mounted : g_mounted) {
                        if (mounted.uiThreadId == threadId ||
                            mounted.uiThreadId == 0) {
                            mounted.generation = g_uiGeneration.load();
                            ApplyForecastToInstance(mounted, forecast, settings);
                        }
                    }
                    for (auto& mounted : g_daylightMounted) {
                        if (mounted.uiThreadId == threadId ||
                            mounted.uiThreadId == 0) {
                            mounted.generation = g_uiGeneration.load();
                            ApplyDaylightToInstance(mounted, forecast);
                        }
                    }
                } catch (...) {
                    Wh_Log(L"Settings apply error %08X", winrt::to_hresult());
                }
            },
            nullptr);
    }

    RequestWeatherRefresh(locationChanged);
}

void Wh_ModUninit() {
    Wh_Log(L"> Wh_ModUninit");
    g_shuttingDown = true;

    if (g_target == Target::Explorer) {
        StopCalendarLaunchBroker();
        Wh_Log(L"Explorer calendar-launch broker stopped");
        return;
    }

    ++g_fetchGeneration;
    g_forceRefreshPending.store(false);

    UnregisterSuspendResumeRefresh();

    // Stops timers without holding g_timerMutex across the wait (see
    // StopRefreshTimer). Holding the mutex here used to deadlock Uninit when a
    // daylight/weather tick was already inside EnsureRefreshTimer.
    StopRefreshTimer();

    UninitializeSettingsAndTap();

    // Restore notification/calendar UI on the dispatcher that owns it before
    // we tear down. Async PostMessage alone often never runs in time and used
    // to leave an empty NotificationCenterGrid square behind.
    try {
        UnmountAllMountedSynchronously();
    } catch (...) {
        Wh_Log(L"UnmountAllMountedSynchronously error %08X",
               winrt::to_hresult());
    }

    for (auto hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Sync uninit for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        // Prefer synchronous cleanup so Visibility/chrome restore actually
        // lands before the mod DLL unloads. Settings changes already use this.
        if (!RunFromWindowThread(
                hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); },
                nullptr)) {
            RunFromWindowThreadViaPostMessage(
                hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); },
                nullptr);
        }
    }

    {
        std::lock_guard lock(g_mountMutex);
        if (!g_mounted.empty()) {
            Wh_Log(L"WARNING: force-unmounting %zu remaining mount(s)",
                   g_mounted.size());
            std::vector<MountedWeatherInstance> leftover;
            leftover.swap(g_mounted);
            for (auto& instance : leftover) {
                try {
                    UnmountInstance(instance);
                } catch (...) {
                }
            }
        }
        if (!g_daylightMounted.empty()) {
            Wh_Log(L"WARNING: force-unmounting %zu remaining daylight mount(s)",
                   g_daylightMounted.size());
            std::vector<MountedDaylightInstance> leftover;
            leftover.swap(g_daylightMounted);
            for (auto& instance : leftover) {
                try {
                    UnmountDaylightInstance(instance);
                } catch (...) {
                }
            }
        }
        if (!g_calendarLaunchMounted.empty()) {
            Wh_Log(L"WARNING: force-unmounting %zu remaining calendar-launch "
                   L"mount(s)",
                   g_calendarLaunchMounted.size());
            std::vector<MountedCalendarLaunchInstance> leftover;
            leftover.swap(g_calendarLaunchMounted);
            for (auto& instance : leftover) {
                try {
                    UnmountCalendarLaunchInstance(instance);
                } catch (...) {
                }
            }
        }
    }

    {
        std::lock_guard lock(g_forecastMutex);
        g_forecast = {};
        g_resolvedLocation = {};
        g_resolvedKey.clear();
    }

    Wh_Log(L"Windows 11 Calendar Weather uninitialized");
}
