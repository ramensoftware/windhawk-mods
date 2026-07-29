// ==WindhawkMod==
// @id              windows-11-calendar-weather
// @name            Windows 11 Calendar Weather
// @description     Replace the Notification Center notification list with a compact Open-Meteo weather forecast card above the calendar
// @version         1.0.0
// @author          FranciscoMurias
// @github          https://github.com/FranciscoMurias
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Windows 11 Calendar Weather

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
- Works with `ShellExperienceHost.exe` and newer `ShellHost.exe` (24H2+)
- Compatible alongside **Windows 11 Notification Center Styler** (avoids copying
  that mod's theme engine; uses a uniquely named injected root)
- Fully restores the notification list when the mod is disabled or unloaded

## Setup

1. Compile and enable the mod in Windhawk.
2. Set **Location name** and/or **Latitude** / **Longitude**.
3. Open the taskbar clock. The weather card appears above the calendar.

Coordinates are preferred in Manual mode. Auto-detect uses the Windows
location API (may prompt for permission) and falls back to Manual values if
location is unavailable.

## Notes

- Weather is cached in memory and refreshed on a timer (default every 60 minutes).
- Opening the clock flyout reuses the cache unless it is older than the refresh interval.
- Offline failures keep the last successful forecast when available.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- locationMode: manual
  $name: Location mode
  $description: Auto uses Windows location (may prompt once). Manual uses the name/coordinates below.
  $options:
  - manual: Manual
  - auto: Auto-detect
- locationName: Warsaw
  $name: Location name
  $description: Used in Manual mode (and as fallback label). Also used to geocode when coordinates are empty.
- latitude: "52.2297"
  $name: Latitude
  $description: Manual mode decimal latitude (string)
- longitude: "21.0122"
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
- showCalendarDaylight: false
  $name: Show calendar daylight hours
  $description: Insert a sunrise–sunset daylight bar between the clock/date and the month calendar
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
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#undef GetCurrentTime

#include <commctrl.h>
#include <combaseapi.h>
#include <ocidl.h>
#include <roapi.h>
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
#include <winrt/Windows.UI.Xaml.Hosting.h>
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
void StopRefreshTimer();
void UpdateAllWeatherUIs();
void ApplyOrRestoreAllDaylight();
void UpdateAllDaylightUIs();
bool MountDaylightIntoCalendarSection(InstanceHandle handle,
                                      wuxc::Grid const& section);

////////////////////////////////////////////////////////////////////////////////
// Target process

enum class Target {
    ShellExperienceHost,
    ShellHost,
};

Target g_target = Target::ShellExperienceHost;

std::atomic<bool> g_initialized{false};
thread_local bool g_initializedForThread = false;
std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_refreshingWeatherChrome{false};
std::atomic<uint64_t> g_uiGeneration{1};

////////////////////////////////////////////////////////////////////////////////
// Settings

struct ModSettings {
    bool autoLocation = false;
    std::wstring locationName = L"Warsaw";
    double latitude = 52.2297;
    double longitude = 21.0122;
    bool coordinatesValid = true;
    std::wstring temperatureUnit = L"celsius";
    int refreshMinutes = 60;
    int hourlyCount = 6;
    int dailyCount = 5;
    int cardHeight = 390;
    bool showLocation = true;
    bool showCalendarDaylight = false;
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

void LoadSettings() {
    ModSettings s;

    string_setting_unique_ptr locationMode(Wh_GetStringSetting(L"locationMode"));
    s.autoLocation =
        locationMode && _wcsicmp(locationMode.get(), L"auto") == 0;

    string_setting_unique_ptr locationName(Wh_GetStringSetting(L"locationName"));
    if (locationName && locationName.get()[0]) {
        s.locationName = locationName.get();
    }

    string_setting_unique_ptr latitudeText(Wh_GetStringSetting(L"latitude"));
    string_setting_unique_ptr longitudeText(Wh_GetStringSetting(L"longitude"));
    bool latOk = false;
    bool lonOk = false;
    s.latitude = ParseCoordinate(latitudeText ? latitudeText.get() : L"", latOk);
    s.longitude =
        ParseCoordinate(longitudeText ? longitudeText.get() : L"", lonOk);
    s.coordinatesValid = latOk && lonOk && s.latitude >= -90.0 &&
                         s.latitude <= 90.0 && s.longitude >= -180.0 &&
                         s.longitude <= 180.0;

    string_setting_unique_ptr unit(Wh_GetStringSetting(L"temperatureUnit"));
    if (unit && _wcsicmp(unit.get(), L"fahrenheit") == 0) {
        s.temperatureUnit = L"fahrenheit";
    } else {
        s.temperatureUnit = L"celsius";
    }

    s.refreshMinutes = ClampInt(Wh_GetIntSetting(L"refreshMinutes"), 15, 360);
    s.hourlyCount = ClampInt(Wh_GetIntSetting(L"hourlyCount"), 3, 8);
    s.dailyCount = ClampInt(Wh_GetIntSetting(L"dailyCount"), 3, 7);
    s.cardHeight = ClampInt(Wh_GetIntSetting(L"cardHeight"), 300, 520);
    s.showLocation = Wh_GetIntSetting(L"showLocation") != 0;
    s.showCalendarDaylight = Wh_GetIntSetting(L"showCalendarDaylight") != 0;

    {
        std::lock_guard lock(g_settingsMutex);
        g_settings = std::move(s);
    }

    Wh_Log(L"Settings loaded: mode=%s location=%s coordsValid=%d unit=%s "
           L"refresh=%d hourly=%d daily=%d height=%d daylight=%d",
           g_settings.autoLocation ? L"auto" : L"manual",
           g_settings.locationName.c_str(), g_settings.coordinatesValid ? 1 : 0,
           g_settings.temperatureUnit.c_str(), g_settings.refreshMinutes,
           g_settings.hourlyCount, g_settings.dailyCount, g_settings.cardHeight,
           g_settings.showCalendarDaylight ? 1 : 0);
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
    std::vector<HourlyEntry> hourly;
    std::vector<DailyEntry> daily;
    std::wstring temperatureUnit;
    std::chrono::steady_clock::time_point fetchedAt{};
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
std::atomic<uint64_t> g_fetchGeneration{0};

PTP_TIMER g_refreshTimer = nullptr;
std::mutex g_timerMutex;

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
    int64_t backgroundChangedCookie = -1;
    int64_t shadowChangedCookie = -1;

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
    wuxc::TextBlock durationText{nullptr};
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

std::vector<MountedDaylightInstance> g_daylightMounted;

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
        return style.Setters().GetAt(0).as<wux::Setter>().Property();
    } catch (...) {
        Wh_Log(L"ResolveGridProperty(%s) failed %08X", propertyName,
               winrt::to_hresult());
    }
    return nullptr;
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

        instance.gridSizeChangedRevoker = {};
        instance.weatherSizeChangedRevoker = weatherRoot.SizeChanged(
            winrt::auto_revoke,
            [gridWeak = winrt::make_weak(notificationGrid),
             rootWeak = winrt::make_weak(weatherRoot)](
                wf::IInspectable const&, wux::SizeChangedEventArgs const&) {
                if (auto grid = gridWeak.get()) {
                    if (auto root = rootWeak.get()) {
                        EnsureGridFitsWeatherContent(grid, root);
                        // Theme / layout passes can restore grid chrome —
                        // keep the under-card transparent (shadow stays on
                        // the weather Border, not this grid).
                        try {
                            grid.Background(wuxm::SolidColorBrush(
                                winrt::Windows::UI::Colors::Transparent()));
                            grid.Shadow(nullptr);
                        } catch (...) {
                        }
                    }
                }
            });

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

std::recursive_mutex g_mountMutex;
std::vector<MountedWeatherInstance> g_mounted;

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

FILETIME SystemTimeToFileTimeLocalAsUtc(const SYSTEMTIME& st) {
    FILETIME ft{};
    SystemTimeToFileTime(&st, &ft);
    return ft;
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

FILETIME FloorFileTimeToHour(FILETIME ft) {
    SYSTEMTIME st{};
    FileTimeToSystemTime(&ft, &st);
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;
    FILETIME out{};
    SystemTimeToFileTime(&st, &out);
    return out;
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

        FILETIME nowLocal = GetForecastLocationNow(data.utcOffsetSeconds);
        FILETIME nowHour = FloorFileTimeToHour(nowLocal);
        ULARGE_INTEGER nowU = FileTimeToUlarge(nowHour);

        size_t startIndex = hourlySize;
        for (size_t i = 0; i < hourlySize; i++) {
            SYSTEMTIME st{};
            bool hasTime = false;
            if (!ParseIsoLocalDateTime(hourlyTimes[i], st, hasTime) ||
                !hasTime) {
                continue;
            }
            FILETIME ft = SystemTimeToFileTimeLocalAsUtc(st);
            if (FileTimeToUlarge(ft).QuadPart >= nowU.QuadPart) {
                startIndex = i;
                break;
            }
        }
        if (startIndex >= hourlySize) {
            startIndex = hourlySize > 0 ? hourlySize - 1 : 0;
        }

        data.hourly.reserve(static_cast<size_t>(hourlyCount));
        for (int n = 0; n < hourlyCount; n++) {
            size_t idx = startIndex + static_cast<size_t>(n);
            if (idx >= hourlySize) {
                break;
            }
            SYSTEMTIME st{};
            bool hasTime = false;
            ParseIsoLocalDateTime(hourlyTimes[idx], st, hasTime);
            HourlyEntry entry;
            entry.hourLabel = FormatHourLabel(st.wHour);
            entry.temperature = hourlyTemps[idx];
            entry.weatherCode = hourlyCodes[idx];
            entry.isDay =
                (idx < hourlyIsDay.size()) ? (hourlyIsDay[idx] > 0) : true;
            data.hourly.push_back(std::move(entry));
        }

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
        data.fetchedAt = std::chrono::steady_clock::now();
        return data;
    } catch (...) {
        Wh_Log(L"ParseForecastJson error %08X", winrt::to_hresult());
        return std::nullopt;
    }
}

std::wstring MakeLocationKey(ModSettings const& s) {
    wchar_t buffer[256];
    swprintf_s(buffer, L"%s|%s|%.5f|%.5f|%s", s.autoLocation ? L"auto" : L"manual",
               s.locationName.c_str(), s.latitude, s.longitude,
               s.temperatureUnit.c_str());
    return buffer;
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

    struct ClearFlag {
        ~ClearFlag() { g_fetchInProgress = false; }
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

void RequestWeatherRefresh(bool forceNetwork) {
    if (g_shuttingDown.load()) {
        return;
    }

    // Coalesce: never bump the fetch generation while a worker is running —
    // that used to invalidate the in-flight result and leave the UI empty.
    if (g_fetchInProgress.load()) {
        Wh_Log(L"Weather refresh skipped — fetch already in progress");
        return;
    }

    auto settings = GetSettingsCopy();
    bool needsFetch = forceNetwork;
    {
        std::lock_guard lock(g_forecastMutex);
        if (!g_forecast.valid) {
            needsFetch = true;
            Wh_Log(L"Weather refresh: no cached forecast");
        } else {
            auto age = std::chrono::steady_clock::now() - g_forecast.fetchedAt;
            auto limit = std::chrono::minutes(settings.refreshMinutes);
            auto ageMin =
                std::chrono::duration_cast<std::chrono::minutes>(age).count();
            if (age >= limit) {
                needsFetch = true;
                Wh_Log(L"Weather refresh: cache age %lld min >= interval %d",
                       static_cast<long long>(ageMin), settings.refreshMinutes);
            } else if (forceNetwork) {
                Wh_Log(L"Weather refresh: forced (timer); cache age %lld min",
                       static_cast<long long>(ageMin));
            } else {
                Wh_Log(L"Weather refresh: using cache (age %lld min, interval %d)",
                       static_cast<long long>(ageMin), settings.refreshMinutes);
            }
            if (g_forecast.temperatureUnit != settings.temperatureUnit) {
                needsFetch = true;
            }
        }
    }

    if (!needsFetch) {
        UpdateAllWeatherUIs();
        return;
    }

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
    const DWORD periodMs =
        static_cast<DWORD>(settings.refreshMinutes) * 60U * 1000U;
    FILETIME due = MakeRelativeDueTime(periodMs);

    std::lock_guard lock(g_timerMutex);
    if (g_refreshTimer) {
        // Reschedule in place — never WaitForThreadpoolTimerCallbacks on a UI
        // thread (mount/settings paths), which can hang the shell.
        SetThreadpoolTimer(g_refreshTimer, &due, periodMs, 30 * 1000);
        Wh_Log(L"Refresh timer rescheduled every %d minutes",
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
    Wh_Log(L"Refresh timer scheduled every %d minutes (first fire in %d min)",
           settings.refreshMinutes, settings.refreshMinutes);
}

void StopRefreshTimer() {
    if (g_refreshTimer) {
        SetThreadpoolTimer(g_refreshTimer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(g_refreshTimer, TRUE);
        CloseThreadpoolTimer(g_refreshTimer);
        g_refreshTimer = nullptr;
        Wh_Log(L"Refresh timer stopped");
    }
}

void StopRefreshTimerLocked() {
    std::lock_guard lock(g_timerMutex);
    StopRefreshTimer();
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

wuxc::Viewbox MakeSunHalfIcon(bool sunrise,
                              double size,
                              wuxm::Brush const& brush) {
    wuxc::Canvas canvas;
    canvas.Width(16);
    canvas.Height(16);
    canvas.IsHitTestVisible(false);

    try {
        // Sunrise: upper semicircle on a horizon. Sunset: flipped (lower).
        std::wstring data =
            sunrise ? L"M2,10 A6,6 0 0 1 14,10 Z M1.5,10.5 L14.5,10.5"
                    : L"M2,6 A6,6 0 0 0 14,6 Z M1.5,5.5 L14.5,5.5";
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
    viewbox.Height(size);
    viewbox.Stretch(wuxm::Stretch::Uniform);
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
    sunriseText.Text(L"--:--");
    ui.sunriseText = sunriseText;
    sunriseStack.Children().Append(sunriseText);
    wuxc::Grid::SetColumn(sunriseStack, 0);

    wuxc::TextBlock durationText;
    durationText.FontSize(12);
    durationText.FontWeight(wut::FontWeights::SemiBold());
    durationText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    durationText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    durationText.VerticalAlignment(wux::VerticalAlignment::Center);
    durationText.Text(L"--");
    ui.durationText = durationText;
    wuxc::Grid::SetColumn(durationText, 1);

    wuxc::StackPanel sunsetStack;
    sunsetStack.Orientation(wuxc::Orientation::Horizontal);
    sunsetStack.Spacing(6);
    sunsetStack.HorizontalAlignment(wux::HorizontalAlignment::Right);
    sunsetStack.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock sunsetText;
    sunsetText.FontSize(12);
    sunsetText.FontFamily(wuxm::FontFamily(L"Segoe UI"));
    sunsetText.Text(L"--:--");
    ui.sunsetText = sunsetText;
    sunsetStack.Children().Append(MakeSunHalfIcon(false, 14, primary));
    sunsetStack.Children().Append(sunsetText);
    wuxc::Grid::SetColumn(sunsetStack, 2);

    infoRow.Children().Append(sunriseStack);
    infoRow.Children().Append(durationText);
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
        if (instance.ui.durationText) {
            instance.ui.durationText.Foreground(primary);
            instance.ui.durationText.Text(
                ok ? FormatDaylightDuration(forecast.todaySunriseMinute,
                                            forecast.todaySunsetMinute)
                   : L"--");
        }
    } catch (...) {
        Wh_Log(L"ApplyDaylightToInstance error %08X", winrt::to_hresult());
    }
}

void UnmountDaylightInstance(MountedDaylightInstance& instance) {
    try {
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
    try {
        for (auto const& child : section.Children()) {
            auto fe = child.try_as<wux::FrameworkElement>();
            if (!fe) {
                continue;
            }
            const auto name = fe.Name();
            if (IsWorldClocksElementName(name) || IsDaylightRootName(name)) {
                continue;
            }
            if (name != L"CalendarControlScrollViewer" &&
                name != L"CalendarHeader" &&
                name != L"CalendarHeaderMinimizedOverlay" &&
                name != L"ExpandCollapseButton" &&
                name != L"FocusSessionControl") {
                continue;
            }
            if (auto ui = child.try_as<wux::UIElement>()) {
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
                Wh_Log(L"WindhawkDaylightRoot already present");
                EnsureCalendarStructuralVisible(section);
                std::lock_guard lock(g_mountMutex);
                for (auto& mounted : g_daylightMounted) {
                    if (mounted.sectionHandle == handle) {
                        return true;
                    }
                }
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
        uint64_t generation = 0;
        ws::DispatcherQueue dispatcherQueue{nullptr};
        wuc::CoreDispatcher coreDispatcher{nullptr};
    };
    std::vector<DispatchTarget> snapshot;
    {
        std::lock_guard lock(g_mountMutex);
        snapshot.reserve(g_daylightMounted.size());
        for (auto const& mounted : g_daylightMounted) {
            snapshot.push_back(DispatchTarget{mounted.sectionHandle,
                                              mounted.generation,
                                              mounted.dispatcherQueue,
                                              mounted.coreDispatcher});
        }
    }

    auto applyOnUi = [forecast](InstanceHandle handle,
                                uint64_t generation) mutable {
        if (g_shuttingDown.load()) {
            return;
        }
        try {
            std::lock_guard lock(g_mountMutex);
            for (auto& mounted : g_daylightMounted) {
                if (mounted.sectionHandle == handle &&
                    mounted.generation == generation) {
                    ApplyDaylightToInstance(mounted, forecast);
                    break;
                }
            }
        } catch (...) {
            Wh_Log(L"Daylight UI apply error %08X", winrt::to_hresult());
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
                    wuc::DispatchedHandler(
                        [applyOnUi, handle = target.handle,
                         generation = target.generation]() mutable {
                            applyOnUi(handle, generation);
                        }));
            } else {
                applyOnUi(target.handle, target.generation);
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
                             ForecastData const& forecast,
                             ModSettings const& settings) {
    try {
        auto weatherRoot = instance.weatherRoot.get();
        if (!weatherRoot) {
            return;
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

void CollapseNativeChildren(wuxc::Grid const& grid,
                            MountedWeatherInstance& instance) {
    instance.childVisibility.clear();

    // Hide all native children — weather Border owns acrylic + rounded corners.
    auto children = grid.Children();
    for (auto const& child : children) {
        auto fe = child.try_as<wux::FrameworkElement>();
        if (fe && fe.Name() == L"WindhawkWeatherRoot") {
            continue;
        }

        auto ui = child.try_as<wux::UIElement>();
        if (!ui) {
            continue;
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
        instance.childVisibility.push_back(record);
        try {
            ui.Visibility(wux::Visibility::Collapsed);
        } catch (...) {
        }
    }
}

void RestoreNativeChildren(MountedWeatherInstance& instance) {
    for (auto const& record : instance.childVisibility) {
        if (auto element = record.element.get()) {
            try {
                element.Visibility(record.original);
                element.Opacity(record.originalOpacity);
            } catch (...) {
            }
        }
    }
    instance.childVisibility.clear();
}

void ForceShowNativeNotificationChildren(wuxc::Grid const& grid) {
    // Belt-and-suspenders: weak-ref restore can miss if the shell rebuilt
    // nodes, leaving Collapsed children and an empty acrylic slot.
    try {
        for (auto const& child : grid.Children()) {
            auto fe = child.try_as<wux::FrameworkElement>();
            if (fe && fe.Name() == L"WindhawkWeatherRoot") {
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
                        // Theme changes while the flyout was closed can restore
                        // opaque NotificationCenterGrid chrome; refresh it.
                        if (auto border = fe.try_as<wuxc::Border>()) {
                            ApplyCalendarMatchedChrome(mounted, grid, border);
                        }
                        return true;
                    }
                }
                Wh_Log(L"Warning: WindhawkWeatherRoot exists without registry "
                       L"entry");
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
    }
    for (auto& instance : local) {
        UnmountInstance(instance);
    }
    for (auto& instance : localDaylight) {
        UnmountDaylightInstance(instance);
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

    HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!done) {
        fn();
        return false;
    }

    struct State {
        std::function<void()> fn;
        HANDLE done = nullptr;
        std::atomic<bool> ran{false};
    };
    auto state = std::make_shared<State>();
    state->fn = fn;
    state->done = done;

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
        SetEvent(state->done);
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

    if (queued) {
        const DWORD wait = WaitForSingleObject(done, 2000);
        if (wait != WAIT_OBJECT_0 && !state->ran.load()) {
            Wh_Log(L"Dispatcher unmount timed out — trying inline");
            invoke();
        }
    } else {
        invoke();
    }
    CloseHandle(done);
    return state->ran.load();
}

void UnmountAllMountedSynchronously() {
    std::vector<MountedWeatherInstance> weather;
    std::vector<MountedDaylightInstance> daylight;
    {
        std::lock_guard lock(g_mountMutex);
        weather.swap(g_mounted);
        daylight.swap(g_daylightMounted);
    }

    for (auto& instance : weather) {
        auto dq = instance.dispatcherQueue;
        auto cd = instance.coreDispatcher;
        auto tid = instance.uiThreadId;
        RunOnMountDispatcher(dq, cd, tid, [&instance]() {
            UnmountInstance(instance);
        });
    }
    for (auto& instance : daylight) {
        auto dq = instance.dispatcherQueue;
        auto cd = instance.coreDispatcher;
        auto tid = instance.uiThreadId;
        RunOnMountDispatcher(dq, cd, tid, [&instance]() {
            UnmountDaylightInstance(instance);
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
                Wh_Log(L"CalendarSection added — scheduling daylight mount");
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
                            }));
                    }
                } catch (...) {
                }
                if (!scheduled) {
                    try {
                        MountDaylightIntoCalendarSection(handle, section);
                    } catch (...) {
                    }
                }
            }
            return;
        }

        // If a native child is re-added under a mounted notification grid,
        // collapse it so the weather card keeps the slot.
        auto parent = element.Parent().try_as<wuxc::Grid>();
        if (!parent || parent.Name() != L"NotificationCenterGrid") {
            return;
        }
        if (element.Name() == L"WindhawkWeatherRoot") {
            return;
        }

        std::lock_guard lock(g_mountMutex);
        for (auto& mounted : g_mounted) {
            auto grid = mounted.notificationGrid.get();
            if (grid && grid == parent) {
                auto ui = element.try_as<wux::UIElement>();
                if (!ui) {
                    break;
                }
                ChildVisibilityRecord record;
                record.element = winrt::make_weak(ui);
                try {
                    record.original = ui.Visibility();
                } catch (...) {
                    record.original = wux::Visibility::Visible;
                }
                mounted.childVisibility.push_back(record);
                ui.Visibility(wux::Visibility::Collapsed);
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
                if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"Unsupported module path");
                return FALSE;
            }
            break;
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
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> Wh_ModSettingsChanged");
    if (g_shuttingDown.load()) {
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

    for (auto hCoreWnd : GetCoreWnds()) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                try {
                    ApplyOrRestoreAllMounted();
                    ApplyOrRestoreAllDaylight();
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
    ++g_fetchGeneration;

    // Cancel timer; do not hang forever if a callback is stuck.
    {
        std::lock_guard lock(g_timerMutex);
        if (g_refreshTimer) {
            SetThreadpoolTimer(g_refreshTimer, nullptr, 0, 0);
            // Best-effort wait — callbacks check g_shuttingDown and return.
            WaitForThreadpoolTimerCallbacks(g_refreshTimer, TRUE);
            CloseThreadpoolTimer(g_refreshTimer);
            g_refreshTimer = nullptr;
            Wh_Log(L"Refresh timer stopped");
        }
    }

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
    }

    {
        std::lock_guard lock(g_forecastMutex);
        g_forecast = {};
        g_resolvedLocation = {};
        g_resolvedKey.clear();
    }

    Wh_Log(L"Windows 11 Calendar Weather uninitialized");
}
