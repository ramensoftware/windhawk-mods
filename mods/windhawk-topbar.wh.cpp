// ==WindhawkMod==
// @id              windhawk-topbar
// @name            TopBar for Windows
// @donateUrl       https://www.patreon.com/WasiXGamer/join
// @description     A working TopBar with Flyouts for Windows through Windhawk.
// @version         1.2.0
// @author          WasiXGamer
// @github          https://github.com/wasixgamer
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32 -ldwmapi -ladvapi32 -luser32 -lshcore -lcomctl32 -lpdh -lpsapi -ldxgi -lwinhttp -ld2d1
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# TopBar For Windhawk
## Note: 
### Settings for this mod were shifted to a separate settings app. It can be accessed either by settings icon in the topbar, OR from the context menus. 
![TopBar screenshot](https://i.imgur.com/4vheIy6.png)
![Flyouts screenshot](https://i.imgur.com/Ihjh1bC.png)
Adds a **TopBar** at top of your screen with multiple customizations, hosted by a
dedicated explorer.exe tool process.

# Support my Work:
[![Patreon](https://i.imgur.com/JJ0TluA.png)](https://www.patreon.com/WasiXGamer/join)

## Themes
Themes are collections of styles that can be selected from the **Theme** dropdown in the mod settings. The following themes are available:

| Theme | Preview |
|-------|---------|
| [GreenBar](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/GreenBar) | [![GreenBar](https://raw.githubusercontent.com/wasixgamer/windhawk-topbar-styling-guide/main/Themes/GreenBar/screenshot.png)](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/GreenBar) |
| [NoIslands](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/NoIslands) | [![NoIslands](https://raw.githubusercontent.com/wasixgamer/windhawk-topbar-styling-guide/main/Themes/NoIslands/screenshot.png)](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/NoIslands) |
| [OS27 GoldenGate](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/OS27%20GoldenGate) | [![OS27 GoldenGate](https://raw.githubusercontent.com/wasixgamer/windhawk-topbar-styling-guide/main/Themes/OS27%20GoldenGate/screenshot.png)](https://github.com/wasixgamer/windhawk-topbar-styling-guide/tree/main/Themes/OS27%20GoldenGate) |

More themes, stylings, etc can be found and contributed from:
**[Windhawk TopBar Styling Guide](https://github.com/wasixgamer/windhawk-topbar-styling-guide)**

## Features

- **Task list** — window icons, titles, click-to-activate, double-click maximize
- **Control centre** — Display (brightness, Dark Mode), Sound (volume, per-app mixer, device picker, media controls), Wi-Fi (scan/connect), Bluetooth (connect/disconnect), Battery, Weather, Recycle Bin, Resource Monitor (CPU/RAM/GPU)
- **Full styling** via Control styles
- **Background translucency** tinting for TopBar, BlurBehind for Flyouts and context menus.

## Weather data

The weather widget is powered by **[Open-Meteo](https://open-meteo.com/)** — a
free, no-API-key weather service. The city search sends the text you type to
`geocoding-api.open-meteo.com`; once a location is picked, the bar sends its
latitude/longitude to `api.open-meteo.com` for the forecast. No account, no
API key, no personal data — only the coordinates (or search string) you
provided.

## Process model

The bar runs in its own Explorer tool process (`explorer.exe -tool-mod windhawk-topbar`).
A mutex keeps one bar alive. XAML Islands require a real Explorer host.

## Styling

Every element is targetable from **Control styles**, using the plain name, a bare
class name (`Button`), `ClassName#Name`, or a parent chain (`StackPanel > TextBlock`).
`*` matches any intermediate parents, and `:root >` requires a root element.

Every element can also be filtered with `[N]` for the Nth child of its parent,
`[Prop=Value]` for a local property value, and `@CommonStates` for elements that
expose a VisualStateGroup. Style values can use `$name` constants,
`Prop@VisualState=Value` for state-specific values, and `Prop=>VarName` /
`{{VarName}}` for captures and substitutions.

### WindhawkBlur

Styles can apply a Windhawk blur brush with XAML: `<WindhawkBlur String="value" />`.
Usable on any brush-typed property (`Background`, `Fill`, `BorderBrush`, ...).

Usage Examples:
`Background:=<WindhawkBlur BlurAmount="18" />`
`Background:=<WindhawkBlur BlurAmount="18" TintColor="#80000000" />`
`Background:=<WindhawkBlur BlurAmount="18" TintColor="#80000000" TintOpacity="0.6" />`

`TintColor` and `TintOpacity` are optional; leave both off for a plain blur with no tint.

### Discovering targets

Run **[UWPSpy](https://github.com/m417z/UWPSpy/releases/)** on the TopBar's
`explorer.exe` process. Ctrl+D shortcut works on the topbar elements but for Flyouts, you will have to scroll down the targets in `Windows.UI.Xaml.PopupRoot`.

| Name | What it is |
|------|------------|
| `TopBarRoot` | Root `Grid` spanning the whole bar |
| `LeftPanel` | Left strip holding Start and Search |
| `StartButton` / `StartIcon` | Start button and its logo |
| `SearchButton` / `SearchIcon` | Search button (opens native Search) |
| `TaskListPanel` / `TaskButton` | Task strip, and every task button |
| `TaskButtonIcon` / `TaskButtonText` | Icon and label inside a task button |
| `DisplayButton` `SoundButton` `WifiButton` `BluetoothButton`  `BatteryButton` | ControlCenter buttons |
| `ClockButton` / `ClockText` | Date/time |
| `ResourceButton` | Battery button |
| `WeatherButton` | Weather button and its temperature label |
| `RecycleBinButton` / `RecycleBinIcon` | Recycle Bin button and its glyph |

### Keyboard shortcuts

Enable **Enable keyboard shortcuts** in the mod settings to register the
following global hotkeys. They're mostly useful for opening a flyout while
inspecting it with UWPSpy in Sticky mode.

| Hotkey | Action |
|--------|--------|
| Ctrl+Alt+0 | Toggle Resource Monitor flyout |
| Ctrl+Alt+1 | Toggle Display flyout |
| Ctrl+Alt+2 | Toggle Sound flyout |
| Ctrl+Alt+3 | Toggle Wi-Fi flyout |
| Ctrl+Alt+4 | Toggle Bluetooth flyout |
| Ctrl+Alt+5 | Toggle Battery flyout |
| Ctrl+Alt+6 | Show Start button context menu |
| Ctrl+Alt+7 | Show Task list context menu |
| Ctrl+Alt+8 | Toggle Weather flyout |
| Ctrl+Alt+9 | Toggle Recycle Bin flyout |

## Global transparency, Tint, and Blur

The tint color is configured in **Global background color** and Tint Opacity is configured in: **Global Tint opacity**.
It is applied on the top bar, flyouts, and all context menus. 
Any style rule that sets `Background:=<WindhawkBlur .../>` on one of those elements overrides the one configured in Mod Settings.

## Known limitations

- Live Wallpapers are NOT supported and topbar background will use default windows wallpaper instead of live wallpaper.


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: None
  $name: Theme
  $options:
  - None: No theme
  - GreenBar: GreenBar
  - NoIslands: NoIslands
  - OS27 GoldenGate: OS27 GoldenGate
  $description: >-
    Select a TopBar theme.
- monitorIndex: 0
  $name: Monitor
  $description: 0 or 1 = primary monitor. 2+ selects the corresponding secondary monitor.
- controlStyles:
  - - target: ""
      $name: Target
    - styles: [""]
      $name: Styles
  $name: Control styles
  $description: >-
    Chose targets Either from the given list in Readme, OR spy through UWPSpy.
- styleConstants: [""]
  $name: Style constants
  $description: name=value pairs referenced in styles as $name.
- enableHotkeys: false
  $name: Enable keyboard shortcuts
  $description: >-
    Register Ctrl+Alt+0..9 as global hotkeys for opening flyouts and context
    menus. Off by default; Ctrl+Alt+digit is a common app binding.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <dwmapi.h>

#include <shellscalingapi.h>
#include <objbase.h>
#include <shlobj.h>
#include <shobjidl.h>


#undef GetCurrentTime

#include <propsys.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <wbemidl.h>
#include <uiautomation.h>
#include <wlanapi.h>
#include <bluetoothapis.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <winhttp.h>
#include <pdh.h>
#include <psapi.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <tlhelp32.h>
#include <powerbase.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <windows.graphics.effects.h>
#include <initguid.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Data.Json.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#if __has_include(<winrt/Windows.Media.Control.h>)
#define TOPBAR_HAS_MEDIA_CONTROL 1
#include <winrt/Windows.Media.Control.h>
#else
#define TOPBAR_HAS_MEDIA_CONTROL 0
#endif

#if __has_include(<winrt/Windows.Devices.Radios.h>)
#define TOPBAR_HAS_RADIOS 1
#include <winrt/Windows.Devices.Radios.h>
#else
#define TOPBAR_HAS_RADIOS 0
#endif

#if __has_include(<winrt/Windows.Devices.Bluetooth.h>)
#define TOPBAR_HAS_BLUETOOTH_LE 1
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Foundation.Collections.h>
#else
#define TOPBAR_HAS_BLUETOOTH_LE 0
#endif

#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <process.h>

#include <vector>
#include <deque>
#include <limits>

using namespace winrt::Windows::UI::Xaml;



namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wf = winrt::Windows::Foundation;
namespace wui = winrt::Windows::UI;

// ============================================================================
// Resource usage (CPU, RAM, GPU)
// ============================================================================
namespace resource {
    struct Usage {
        int cpu = 0;
        int ram = 0;
        int gpu = 0;
        bool cpuAvailable = false;
        bool gpuAvailable = false;
    };

    struct DetailedInfo {
        // CPU
        int cpuClockMHz = 0;
        int cpuCores = 0;
        int cpuThreads = 0;
        // RAM
        int ramSpeedMHz = 0;
        uint64_t virtualMemoryTotal = 0;
        uint64_t virtualMemoryUsed = 0;
        std::wstring ramType;
        std::wstring ramManufacturer;
        std::wstring ramPartNumber;
        uint64_t ramCapacity = 0;
        // GPU
        std::wstring gpuName;
        uint64_t vramTotal = 0;
        uint64_t vramUsed = 0;
    };

    // CPU usage via PDH (Performance Data Helper)
    // We use a single counter for total processor time.
    PDH_HQUERY g_cpuQuery = nullptr;
    PDH_HCOUNTER g_cpuCounter = nullptr;

    // GPU usage via PDH "GPU Engine" counters
    // We'll aggregate all instances by expanding wildcards into individual counters.
    PDH_HQUERY g_gpuQuery = nullptr;
    std::vector<PDH_HCOUNTER> g_gpuCounters;
    bool g_gpuQueryInitialized = false;
    bool g_gpuSecondPollDone = false;

    // (GPU Adapter Memory counters are handled by GetVramUsed() with its own query)

    // Expands a wildcard PDH path into specific localized counters (from m417z's approach)
    std::vector<std::wstring> ExpandGpuWildcard(PCWSTR wildcard_path) {
        std::vector<std::wstring> paths;
        PDH_HQUERY temp_query;
        PDH_HCOUNTER temp_counter;
        if (PdhOpenQuery(nullptr, 0, &temp_query) != ERROR_SUCCESS) return paths;
        if (PdhAddEnglishCounter(temp_query, wildcard_path, 0, &temp_counter) != ERROR_SUCCESS) {
            PdhCloseQuery(temp_query);
            return paths;
        }

        DWORD buffer_size = 0;
        PdhGetCounterInfo(temp_counter, FALSE, &buffer_size, nullptr);
        std::vector<BYTE> buffer(buffer_size);
        PDH_COUNTER_INFO* counter_info = reinterpret_cast<PDH_COUNTER_INFO*>(buffer.data());
        if (PdhGetCounterInfo(temp_counter, FALSE, &buffer_size, counter_info) != ERROR_SUCCESS) {
            PdhCloseQuery(temp_query);
            return paths;
        }

        buffer_size = 0;
        PdhExpandWildCardPath(nullptr, counter_info->szFullPath, nullptr, &buffer_size, 0);
        std::vector<WCHAR> path_buffer(buffer_size);
        if (PdhExpandWildCardPath(nullptr, counter_info->szFullPath, path_buffer.data(), &buffer_size, 0) == ERROR_SUCCESS) {
            WCHAR* p = path_buffer.data();
            while (*p) {
                paths.emplace_back(p);
                p += wcslen(p) + 1;
            }
        }
        PdhCloseQuery(temp_query);
        return paths;
    }

    // RAM usage via GlobalMemoryStatusEx
    std::mutex g_resourceInitMutex;

    void Initialize() {
        std::lock_guard<std::mutex> lock(g_resourceInitMutex);
        // Initialize CPU counter
        if (!g_cpuQuery) {
            if (PdhOpenQuery(nullptr, 0, &g_cpuQuery) == ERROR_SUCCESS) {
                if (PdhAddEnglishCounter(g_cpuQuery, L"\\Processor(_Total)\\% Processor Time", 0, &g_cpuCounter) != ERROR_SUCCESS) {
                    PdhCloseQuery(g_cpuQuery);
                    g_cpuQuery = nullptr;
                }
            }
        }

        // Initialize GPU Engine counters (Summing all instances)
        if (!g_gpuQueryInitialized) {
            if (PdhOpenQuery(nullptr, 0, &g_gpuQuery) == ERROR_SUCCESS) {
                auto paths = ExpandGpuWildcard(L"\\GPU Engine(*)\\Utilization Percentage");
                for (const auto& path : paths) {
                    PDH_HCOUNTER counter;
                    if (PdhAddCounter(g_gpuQuery, path.c_str(), 0, &counter) == ERROR_SUCCESS) {
                        g_gpuCounters.push_back(counter);
                    }
                }
                g_gpuQueryInitialized = true;
            } else {
                g_gpuQueryInitialized = true;
            }
        }

        // (GPU Adapter Memory counters are handled by GetVramUsed() with its own query)
    }

    int GetCpu() {
        if (!g_cpuQuery || !g_cpuCounter) return -1;
        PDH_FMT_COUNTERVALUE value;
        if (PdhCollectQueryData(g_cpuQuery) != ERROR_SUCCESS) return -1;
        if (PdhGetFormattedCounterValue(g_cpuCounter, PDH_FMT_DOUBLE, nullptr, &value) != ERROR_SUCCESS) return -1;
        return static_cast<int>(value.doubleValue + 0.5);
    }

    int GetRam() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(memInfo);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return static_cast<int>(memInfo.dwMemoryLoad);
        }
        return -1;
    }

    int GetGpu() {
        if (!g_gpuQuery || g_gpuCounters.empty()) return -1;
        PDH_FMT_COUNTERVALUE value;
        if (PdhCollectQueryData(g_gpuQuery) != ERROR_SUCCESS) return -1;
        if (!g_gpuSecondPollDone) {
            Sleep(100); // Wait a tiny bit to allow the counter to compute a baseline
            PdhCollectQueryData(g_gpuQuery);
            g_gpuSecondPollDone = true;
        }
        // \GPU Engine(*)\Utilization Percentage reports one value per engine
        // (3D, Copy, VideoDecode, ...) per adapter, and each is already a
        // percentage. Report the busiest engine, matching Task Manager.
        double usage = 0;
        for (auto counter : g_gpuCounters) {
            if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value) == ERROR_SUCCESS) {
                if (value.CStatus == ERROR_SUCCESS) {
                    if (value.doubleValue > usage) usage = value.doubleValue;
                }
            }
        }
        if (usage < 0) usage = 0;
        if (usage > 100) usage = 100;
        return static_cast<int>(usage + 0.5);
    }

    Usage GetUsage() {
        std::lock_guard<std::mutex> lock(g_resourceInitMutex);
        Usage usage;
        usage.cpu = GetCpu();
        usage.ram = GetRam();
        usage.gpu = GetGpu();
        usage.cpuAvailable = (usage.cpu >= 0);
        usage.gpuAvailable = (usage.gpu >= 0);
        return usage;
    }

    // CPU clock speed via PDH
    int GetCpuClockMHz() {
        static PDH_HQUERY query = nullptr;
        static PDH_HCOUNTER counter = nullptr;
        if (!query) {
            if (PdhOpenQuery(nullptr, 0, &query) != ERROR_SUCCESS) return 0;
            if (PdhAddEnglishCounter(query, L"\\Processor Information(_Total)\\Processor Frequency", 0, &counter) != ERROR_SUCCESS) {
                PdhCloseQuery(query);
                query = nullptr;
                return 0;
            }
        }
        PDH_FMT_COUNTERVALUE value;
        if (PdhCollectQueryData(query) != ERROR_SUCCESS) return 0;
        if (PdhGetFormattedCounterValue(counter, PDH_FMT_LONG, nullptr, &value) != ERROR_SUCCESS) return 0;
        return static_cast<int>(value.longValue);
    }

    int GetCpuCoreCount() {
        DWORD length = 0;
        GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &length);
        if (length == 0) {
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            return si.dwNumberOfProcessors;
        }
        std::vector<BYTE> buffer(length);
        if (!GetLogicalProcessorInformationEx(
                RelationProcessorCore,
                reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer.data()),
                &length)) {
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            return si.dwNumberOfProcessors;
        }
        DWORD count = 0;
        DWORD offset = 0;
        while (offset < length) {
            auto* entry = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(
                buffer.data() + offset);
            if (entry->Size == 0) break;
            if (entry->Relationship == RelationProcessorCore) {
                count++;
            }
            offset += entry->Size;
        }
        return count > 0 ? static_cast<int>(count) : 1;
    }

    int GetCpuThreadCount() {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwNumberOfProcessors;
    }

    int GetRamSpeedMHz() {
        static int cached = -1;
        if (cached >= 0) return cached;
        // Use WMI (root\cimv2 Win32_PhysicalMemory)
        static const CLSID kCLSID_WbemLocator = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        static const IID kIID_IWbemLocator = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        winrt::com_ptr<IWbemLocator> locator;
        if (FAILED(CoCreateInstance(kCLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                                    kIID_IWbemLocator, locator.put_void()))) return 0;
        BSTR ns = SysAllocString(L"root\\cimv2");
        winrt::com_ptr<IWbemServices> services;
        if (FAILED(locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0, nullptr, nullptr,
                                             services.put()))) {
            SysFreeString(ns);
            return 0;
        }
        SysFreeString(ns);
        CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        BSTR query = SysAllocString(L"SELECT Speed FROM Win32_PhysicalMemory");
        BSTR lang = SysAllocString(L"WQL");
        winrt::com_ptr<IEnumWbemClassObject> enumerator;
        if (SUCCEEDED(services->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                          nullptr, enumerator.put()))) {
            IWbemClassObject* obj = nullptr;
            ULONG returned = 0;
            int speed = 0;
            while (SUCCEEDED(enumerator->Next(2000, 1, &obj, &returned)) && returned) {
                VARIANT v;
                VariantInit(&v);
                if (SUCCEEDED(obj->Get(L"Speed", 0, &v, nullptr, nullptr))) {
                    if (v.vt == VT_I4) speed = v.lVal;
                }
                VariantClear(&v);
                obj->Release();
                if (speed > 0) break;
            }
            cached = speed;
        }
        SysFreeString(query);
        SysFreeString(lang);
        return cached > 0 ? cached : 0;
    }

    uint64_t GetVirtualMemoryTotal() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(memInfo);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return memInfo.ullTotalPageFile;
        }
        return 0;
    }

    // Gets all GPU names for the dropdown
    std::vector<std::wstring> GetAllGpuNames() {
        std::vector<std::wstring> names;
        static const CLSID kCLSID_WbemLocator = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        static const IID kIID_IWbemLocator = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        winrt::com_ptr<IWbemLocator> locator;
        if (FAILED(CoCreateInstance(kCLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                                    kIID_IWbemLocator, locator.put_void()))) return names;
        BSTR ns = SysAllocString(L"root\\cimv2");
        winrt::com_ptr<IWbemServices> services;
        if (FAILED(locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0, nullptr, nullptr,
                                             services.put()))) {
            SysFreeString(ns);
            return names;
        }
        SysFreeString(ns);
        CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        BSTR query = SysAllocString(L"SELECT Name FROM Win32_VideoController");
        BSTR lang = SysAllocString(L"WQL");
        winrt::com_ptr<IEnumWbemClassObject> enumerator;
        if (SUCCEEDED(services->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                          nullptr, enumerator.put()))) {
            IWbemClassObject* obj = nullptr;
            ULONG returned = 0;
            while (SUCCEEDED(enumerator->Next(2000, 1, &obj, &returned)) && returned) {
                VARIANT v;
                VariantInit(&v);
                if (SUCCEEDED(obj->Get(L"Name", 0, &v, nullptr, nullptr))) {
                    if (v.vt == VT_BSTR) names.push_back(v.bstrVal);
                }
                VariantClear(&v);
                obj->Release();
            }
        }
        SysFreeString(query);
        SysFreeString(lang);
        return names;
    }

    // Gets CPU full name
    std::wstring GetCpuFullName() {
        static std::wstring name;
        if (!name.empty()) return name;
        static const CLSID kCLSID_WbemLocator = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        static const IID kIID_IWbemLocator = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        winrt::com_ptr<IWbemLocator> locator;
        if (FAILED(CoCreateInstance(kCLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                                    kIID_IWbemLocator, locator.put_void()))) return L"";
        BSTR ns = SysAllocString(L"root\\cimv2");
        winrt::com_ptr<IWbemServices> services;
        if (FAILED(locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0, nullptr, nullptr,
                                             services.put()))) {
            SysFreeString(ns);
            return L"";
        }
        SysFreeString(ns);
        CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        BSTR query = SysAllocString(L"SELECT Name FROM Win32_Processor");
        BSTR lang = SysAllocString(L"WQL");
        winrt::com_ptr<IEnumWbemClassObject> enumerator;
        if (SUCCEEDED(services->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                          nullptr, enumerator.put()))) {
            IWbemClassObject* obj = nullptr;
            ULONG returned = 0;
            if (SUCCEEDED(enumerator->Next(2000, 1, &obj, &returned)) && returned) {
                VARIANT v;
                VariantInit(&v);
                if (SUCCEEDED(obj->Get(L"Name", 0, &v, nullptr, nullptr))) {
                    if (v.vt == VT_BSTR) name = v.bstrVal;
                }
                VariantClear(&v);
                obj->Release();
            }
        }
        SysFreeString(query);
        SysFreeString(lang);
        return name;
    }

    // Gets RAM type (e.g., DDR4, DDR5), total capacity, and module name
    struct RamInfo {
        std::wstring type;
        std::wstring manufacturer;
        std::wstring partNumber;
        uint64_t totalCapacity = 0;
    };

    RamInfo GetRamInfo() {
        static RamInfo cached;
        static bool cachedInitialized = false;
        if (cachedInitialized) {
            return cached;
        }
        RamInfo info;
        static const CLSID kCLSID_WbemLocator = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        static const IID kIID_IWbemLocator = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
        winrt::com_ptr<IWbemLocator> locator;
        if (FAILED(CoCreateInstance(kCLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                                    kIID_IWbemLocator, locator.put_void()))) return info;
        BSTR ns = SysAllocString(L"root\\cimv2");
        winrt::com_ptr<IWbemServices> services;
        if (FAILED(locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0, nullptr, nullptr,
                                             services.put()))) {
            SysFreeString(ns);
            return info;
        }
        SysFreeString(ns);
        CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        BSTR query = SysAllocString(L"SELECT SMBIOSMemoryType, Capacity, Manufacturer, PartNumber FROM Win32_PhysicalMemory");
        BSTR lang = SysAllocString(L"WQL");
        winrt::com_ptr<IEnumWbemClassObject> enumerator;
        if (SUCCEEDED(services->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                          nullptr, enumerator.put()))) {
            IWbemClassObject* obj = nullptr;
            ULONG returned = 0;
            while (SUCCEEDED(enumerator->Next(2000, 1, &obj, &returned)) && returned) {
                VARIANT vType, vCap, vManufacturer, vPartNumber;
                VariantInit(&vType);
                VariantInit(&vCap);
                VariantInit(&vManufacturer);
                VariantInit(&vPartNumber);
                if (SUCCEEDED(obj->Get(L"SMBIOSMemoryType", 0, &vType, nullptr, nullptr)) && vType.vt == VT_I2) {
                    switch (vType.iVal) {
                        case 20: info.type = L"DDR"; break;
                        case 21: info.type = L"DDR2"; break;
                        case 24: info.type = L"DDR3"; break;
                        case 26: info.type = L"DDR4"; break;
                        case 34: info.type = L"DDR5"; break;
                        default: info.type = L"RAM"; break;
                    }
                }
                if (SUCCEEDED(obj->Get(L"Capacity", 0, &vCap, nullptr, nullptr)) && vCap.vt == VT_UI8) {
                    info.totalCapacity += vCap.ullVal;
                }
                if (SUCCEEDED(obj->Get(L"Manufacturer", 0, &vManufacturer, nullptr, nullptr)) && vManufacturer.vt == VT_BSTR) {
                    info.manufacturer = vManufacturer.bstrVal;
                }
                if (SUCCEEDED(obj->Get(L"PartNumber", 0, &vPartNumber, nullptr, nullptr)) && vPartNumber.vt == VT_BSTR) {
                    info.partNumber = vPartNumber.bstrVal;
                }
                VariantClear(&vType);
                VariantClear(&vCap);
                VariantClear(&vManufacturer);
                VariantClear(&vPartNumber);
                obj->Release();
            }
        }
        SysFreeString(query);
        SysFreeString(lang);
        
        // Fallback: if WMI capacity query failed, get total physical memory
        if (info.totalCapacity == 0) {
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(memInfo);
            if (GlobalMemoryStatusEx(&memInfo)) {
                info.totalCapacity = memInfo.ullTotalPhys;
            }
        }
        cached = info;
        cachedInitialized = true;
        return info;
    }

    // Live CPU frequency using NtPowerInformation
    // PROCESSOR_POWER_INFORMATION is not reliably declared in the toolchain's headers,
    // so it's defined manually. This matches the Windows API definition exactly.
    struct PROCESSOR_POWER_INFORMATION {
        ULONG Number;
        ULONG MaxMhz;
        ULONG CurrentMhz;
        ULONG MhzLimit;
        ULONG MaxIdleState;
        ULONG CurrentIdleState;
    };
    int GetCurrentCpuFrequencyMHz() {
        using CallNtPowerInformation_t = LONG(WINAPI*)(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);
        HMODULE hPowrProf = GetModuleHandle(L"powrprof.dll");
        if (!hPowrProf) hPowrProf = LoadLibraryEx(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!hPowrProf) return 0;
        auto pCall = (CallNtPowerInformation_t)GetProcAddress(hPowrProf, "CallNtPowerInformation");
        if (!pCall) return 0;

        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD numProcessors = si.dwNumberOfProcessors;
        if (numProcessors == 0) return 0;

        std::vector<PROCESSOR_POWER_INFORMATION> info(numProcessors);
        ULONG result = pCall(ProcessorInformation, nullptr, 0, info.data(), (ULONG)(sizeof(PROCESSOR_POWER_INFORMATION) * numProcessors));
        if (result != 0) return 0;

        ULONGLONG total = 0;
        for (DWORD i = 0; i < numProcessors; i++) {
            total += info[i].CurrentMhz;
        }
        return (int)(total / numProcessors);
    }

    // Virtual memory used (Total - Available)
    uint64_t GetVirtualMemoryUsed() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(memInfo);
        if (GlobalMemoryStatusEx(&memInfo)) {
            return memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
        }
        return 0;
    }

    std::wstring GetGpuName(int gpuIndex = 0) {
        static std::vector<std::wstring> cachedNames;
        static bool initialized = false;
        if (!initialized) {
            cachedNames = GetAllGpuNames();
            initialized = true;
        }
        if (gpuIndex >= 0 && gpuIndex < (int)cachedNames.size()) {
            return cachedNames[gpuIndex];
        }
        return L"";
    }

    // GPU VRAM total via DXGI (with GPU index)
    uint64_t GetVramTotal(int gpuIndex = 0) {
        static std::vector<uint64_t> cachedVram;
        static bool initialized = false;
        if (!initialized) {
            winrt::com_ptr<IDXGIFactory1> factory;
            if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)factory.put()))) {
                initialized = true;
                return 0;
            }
            for (UINT i = 0;; ++i) {
                winrt::com_ptr<IDXGIAdapter1> adapter;
                if (factory->EnumAdapters1(i, adapter.put()) == DXGI_ERROR_NOT_FOUND) break;
                DXGI_ADAPTER_DESC1 desc;
                if (SUCCEEDED(adapter->GetDesc1(&desc))) {
                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
                    cachedVram.push_back(desc.DedicatedVideoMemory);
                }
            }
            initialized = true;
        }
        if (gpuIndex >= 0 && gpuIndex < (int)cachedVram.size()) {
            return cachedVram[gpuIndex];
        }
        return 0;
    }

    // --- Helper to extract LUID from a PDH instance name ---
    std::wstring ExtractGpuLuidFromInstance(std::wstring_view instance) {
        auto luid_pos = instance.find(L"luid_");
        if (luid_pos == std::wstring_view::npos) return L"";
        auto luid_start = luid_pos + 5;
        auto phys_pos = instance.find(L"_phys_", luid_start);
        if (phys_pos == std::wstring_view::npos) return L"";
        return std::wstring(instance.substr(luid_start, phys_pos - luid_start));
    }

    // Helper to get the exact LUID of the selected GPU (by index)
    std::wstring GetGpuLuidForIndex(int gpuIndex) {
        winrt::com_ptr<IDXGIFactory1> factory;
        if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)factory.put()))) return L"";
        int current = 0;
        for (UINT i = 0;; ++i) {
            winrt::com_ptr<IDXGIAdapter1> adapter;
            if (factory->EnumAdapters1(i, adapter.put()) == DXGI_ERROR_NOT_FOUND) break;
            DXGI_ADAPTER_DESC1 desc;
            if (SUCCEEDED(adapter->GetDesc1(&desc))) {
                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
                if (current == gpuIndex) {
                    WCHAR luid_str[32];
                    swprintf_s(luid_str, L"0x%08X_0x%08X", desc.AdapterLuid.HighPart, desc.AdapterLuid.LowPart);
                    return luid_str;
                }
                current++;
            }
        }
        return L"";
    }

    // GPU VRAM used via PDH (Performance Data Helper)
    // Sums only the expanded instances matching the selected GPU (by LUID)
    uint64_t GetVramUsed(int gpuIndex = 0) {
        static int lastGpuIndex = -1;
        static PDH_HQUERY vramQuery = nullptr;
        static std::vector<PDH_HCOUNTER> vramCounters;

        // If we switched GPU tabs, re-create the query to filter to the new GPU
        if (lastGpuIndex != gpuIndex) {
            if (vramQuery) {
                PdhCloseQuery(vramQuery);
                vramQuery = nullptr;
                vramCounters.clear();
            }

            if (PdhOpenQuery(nullptr, 0, &vramQuery) == ERROR_SUCCESS) {
                auto paths = ExpandGpuWildcard(L"\\GPU Adapter Memory(*)\\Dedicated Usage");
                std::wstring targetLuid = GetGpuLuidForIndex(gpuIndex);

                for (const auto& path : paths) {
                    // Extract the instance name from the path (e.g., "luid_0x..._0x..._phys_0")
                    auto start = path.find(L'(');
                    auto end = path.rfind(L')');
                    std::wstring instance;
                    if (start != std::wstring::npos && end != std::wstring::npos) {
                        instance = path.substr(start + 1, end - start - 1);
                    }

                    // Extract LUID and match it to the selected GPU
                    std::wstring luid = ExtractGpuLuidFromInstance(instance);
                    if (!targetLuid.empty() && luid == targetLuid) {
                        PDH_HCOUNTER c;
                        if (PdhAddCounter(vramQuery, path.c_str(), 0, &c) == ERROR_SUCCESS) {
                            vramCounters.push_back(c);
                        }
                    }
                }
                PdhCollectQueryData(vramQuery); // Initial baseline poll
            }
            lastGpuIndex = gpuIndex;
        }

        if (!vramQuery || vramCounters.empty()) return 0;
        if (PdhCollectQueryData(vramQuery) != ERROR_SUCCESS) return 0;

        uint64_t total = 0;
        PDH_FMT_COUNTERVALUE value;
        for (auto c : vramCounters) {
            if (PdhGetFormattedCounterValue(c, PDH_FMT_LARGE, nullptr, &value) == ERROR_SUCCESS) {
                if (value.CStatus == ERROR_SUCCESS) {
                    total += static_cast<uint64_t>(value.largeValue);
                }
            }
        }
        return total;
    }

    DetailedInfo GetDetailedInfo(int gpuIndex = 0) {
        std::lock_guard<std::mutex> lock(g_resourceInitMutex);
        DetailedInfo info;
        info.cpuClockMHz = GetCurrentCpuFrequencyMHz(); // live frequency
        if (info.cpuClockMHz <= 0) info.cpuClockMHz = GetCpuClockMHz(); // fallback if API fails
        info.cpuCores = GetCpuCoreCount();
        info.cpuThreads = GetCpuThreadCount();
        // processCount is not displayed; omitted to avoid per-second toolhelp snapshot
        info.ramSpeedMHz = GetRamSpeedMHz();
        info.virtualMemoryTotal = GetVirtualMemoryTotal();
        info.virtualMemoryUsed = GetVirtualMemoryUsed(); // live used
        RamInfo ramInfo = GetRamInfo();
        info.ramType = ramInfo.type;
        info.ramManufacturer = ramInfo.manufacturer;
        info.ramPartNumber = ramInfo.partNumber;
        info.ramCapacity = ramInfo.totalCapacity;
        info.gpuName = GetGpuName(gpuIndex);
        info.vramTotal = GetVramTotal(gpuIndex);
        info.vramUsed = GetVramUsed(gpuIndex);
        return info;
    }

    void Cleanup() {
        if (g_cpuQuery) {
            PdhCloseQuery(g_cpuQuery);
            g_cpuQuery = nullptr;
            g_cpuCounter = nullptr;
        }
        if (g_gpuQuery) {
            PdhCloseQuery(g_gpuQuery);
            g_gpuQuery = nullptr;
            g_gpuCounters.clear();
        }
        // (g_vramQuery is no longer used; see GetVramUsed())
    }
}
// IXamlSourceTransparency – not projected in standard headers, so declare manually.
MIDL_INTERFACE("06636c29-5a17-458d-8ea2-2422d997a922")
IXamlSourceTransparency : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_IsBackgroundTransparent(BOOL* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsBackgroundTransparent(BOOL value) = 0;
};
// Returns the system accent color, falling back to default blue if it fails.
wui::Color GetSystemAccentColor() {
    try {
        auto settings = winrt::Windows::UI::ViewManagement::UISettings();
        auto c = settings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        const double factor = 0.8;
        uint8_t r = static_cast<uint8_t>(c.R * factor);
        uint8_t g = static_cast<uint8_t>(c.G * factor);
        uint8_t b = static_cast<uint8_t>(c.B * factor);
        return wui::ColorHelper::FromArgb(c.A, r, g, b);
    } catch (...) {
        return wui::ColorHelper::FromArgb(255, 0, 120, 212);
    }
}
// ============================================================================
// Forward declarations
// ============================================================================

void RefreshTaskList(bool forceIconRegeneration);
void RefreshApplicationButtons();
void RunOnUiThread(std::function<void()> work);


void ApplyAllControlStyles();
void ApplyVisibilitySettings();
FrameworkElement BuildTopBarContent();
void PositionAppBar(HWND hwnd, int heightPx);
wuxc::ControlTemplate BuildFlyoutShellTemplate(bool isMenu);
extern wuxc::ControlTemplate g_flyoutShellTemplate;
extern wuxc::ControlTemplate g_menuShellTemplate;
void BuildTaskContextMenu();
void BuildStartContextMenu();
std::wstring FormatClockText();
void PopulateDisplayPanel();
void PopulateSoundPanel();
void PopulateWifiPanel();
void PopulateBluetoothPanel();
void RefreshBluetoothRadioState();
void PopulateBatteryPanel();
void PopulateWeatherPanel();
void RefreshWeatherButtonContent();
void PopulateRecycleBinPanel();
void PopulateSettingsPanel();
void ApplyBlurToAllOpenPopups();
void StripInheritedIslandBackgrounds();
void ApplyWindowBackdrop(HWND hwnd);
void ApplyModernWindowChrome(HWND hwnd);
std::wstring ReadTrayOrder();

void LoadSettings();
double GetBarDpiScale();
void UpdateResourceButton();
void OpenTopBarSettingsWindow();
void CloseTopBarSettingsWindow();

// ============================================================================
// Settings
// ============================================================================

struct ControlStyleRule {
    std::wstring target;
    std::vector<std::wstring> styles;
};

struct {
    int barHeightDip = 40;
    std::wstring topBarBackgroundColor = L"#000000";
    int topBarBackgroundOpacity = 25;
    int monitorIndex = 0;
    int cornerRadius = 6;
    bool showStartButton = true;
    bool showSearchButton = true;
    std::wstring centerContent = L"applicationButtons";
    int taskButtonWidth = 100;
    int taskIconSize = 20;
    std::wstring taskButtonContent = L"textOnly";
    bool showDisplayButton = true;
    bool showSoundButton = true;
    bool showWifiButton = true;
    bool showBluetoothButton = true;
    bool showBatteryButton = true;
    bool showSettingsButton = true;
    bool showWeatherButton = true;

    std::wstring leftItems = L"StartButton,SearchButton";
    std::wstring centerItems = L"ResourceButton,WeatherButton,SettingsButton";
    std::wstring rightItems = L"DisplayButton,SoundButton,WifiButton,BluetoothButton,BatteryButton,RecycleBinButton,ClockButton";

    bool showCpuUsage = true;
    bool showRamUsage = true;
    bool showGpuUsage = true;
    bool enableHotkeys = false;
    bool showClock = true;
    std::wstring timeFormat = L"🕑hh:mm tt";
    bool showDate = true;
    std::wstring dateFormat = L"📅ddd, MMM dd";
    std::wstring iconColor = L"#FFFFFF";
    std::wstring fontFamily;
    bool fontBold = false;
    int globalBlurAmount = 6;
    std::wstring trayOrder;

    std::wstring weatherLocationName;
    std::wstring weatherLatitude;
    std::wstring weatherLongitude;
} g_settings;

struct WeatherState {
    bool valid = false;
    double temperature = 0.0;
    double tempMin = 0.0;
    double tempMax = 0.0;
    int weatherCode = 0;
    std::wstring locationName;
    std::vector<std::pair<std::wstring, double>> hourlyTemps;
    std::vector<int> hourlyCodes;
    std::vector<int> hourlyRain;
    std::vector<std::wstring> hourlyTimes;
    ULONGLONG lastFetchTick = 0;
};
WeatherState g_weatherState;

struct WeatherSearchResult {
    std::wstring name;
    std::wstring admin1;
    std::wstring country;
    std::wstring lat;
    std::wstring lon;
};

bool g_weatherLocationPickerActive = false;
std::wstring g_weatherSearchQuery;
std::vector<WeatherSearchResult> g_weatherSearchResults;
std::atomic<int> g_weatherSearchToken{0};
[[clang::no_destroy]] DispatcherTimer g_weatherSearchDebounceTimer{nullptr};

bool g_recycleBinConfirming = false;

struct RecycleBinState {
    uint64_t sizeBytes = 0;
    uint64_t itemCount = 0;
    bool valid = false;
};
RecycleBinState g_recycleBinState;

std::vector<std::wstring> g_trayOrder;
const std::vector<std::wstring> kDefaultTrayOrder = {
    L"WeatherButton", L"DisplayButton", L"SoundButton", L"WifiButton",
    L"BluetoothButton", L"BatteryButton", L"ResourceButton",
    L"RecycleBinButton", L"ClockButton"};

// Every element the UI Alignment page / leftItems-centerItems-rightItems
// storage keys can move between panels. Superset of kDefaultTrayOrder —
// Start, Search and Settings are also placeable but don't get the tray
// right-click reorder menu.
const std::vector<std::wstring> kMovableItems = {
    L"StartButton", L"SearchButton", L"SettingsButton",
    L"WeatherButton", L"DisplayButton", L"SoundButton", L"WifiButton",
    L"BluetoothButton", L"BatteryButton", L"ResourceButton",
    L"RecycleBinButton", L"ClockButton"};

[[clang::no_destroy]] wuxc::StackPanel g_trayPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_centerPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_clockPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_leftPanel{nullptr};
[[clang::no_destroy]] wuxc::Button g_settingsButton{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_settingsFlyout{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_settingsPanel{nullptr};

std::vector<std::pair<std::wstring, std::wstring>> g_styleConstants;
std::vector<ControlStyleRule> g_controlStyleRules;

const std::vector<ControlStyleRule>& BuiltInStyles() {
    static const std::vector<ControlStyleRule> styles = {
        {L"TopBarRoot", {L"Margin=3,2", L"CornerRadius=6"}},
        {L"StartButton", {L"Width=35", L"Margin=8,2,2,2", L"Background:=#15ffffff"}},
        {L"SearchButton", {L"Background:=#15ffffff", L"Width=35", L"Margin=4,2,4,2"}},
        {L"SearchIcon", {L"Width=20", L"Height=20"}},
        {L"TaskButton",
         {L"Background:=#15ffffff", L"Margin=3,4,3,4", L"Foreground=white"}},
        {L"ClockText", {L"Foreground=white", L"FontSize=14"}},
        {L"ClockButton", {L"Background:=#15ffffff", L"Margin=3,4,6,4"}},
        {L"DisplayButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"SoundButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"WifiButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"BluetoothButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"ResourceButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"BatteryButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"WeatherButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"RecycleBinButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"SettingsButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"AppTitleButton", {L"Background:=#15ffffff", L"Margin=5,4"}},
        {L"WifiHeaderToggle", {L"Width=50"}},
        {L"BluetoothHeaderToggle", {L"Width=50"}},
    };
    return styles;
}
const std::vector<ControlStyleRule> g_themeGreenBarStyles = {
    {L"TopBarRoot", {L"Background:=#102A27", L"IconColor=#7FD1C4"}},
    {L"WeatherButton", {L"Background:=#27403C"}},
    {L"RecycleBinButton", {L"Background:=#27403C"}},
    {L"SettingsButton", {L"Background:=#27403C"}},
    {L"WifiHeaderToggle", {L"Width=50"}},
    {L"BluetoothHeaderToggle", {L"Width=50"}},
    {L"StartButton", {L"Background:=#27403C"}},
    {L"SearchButton", {L"Background:=#27403C"}},
    {L"ClockButton", {L"Background:=#27403C"}},
    {L"DisplayButton", {L"Background:=#27403C"}},
    {L"SoundButton", {L"Background:=#27403C"}},
    {L"WifiButton", {L"Background:=#27403C"}},
    {L"BluetoothButton", {L"Background:=#27403C"}},
    {L"ResourceButton", {L"Background:=#27403C"}},
    {L"BatteryButton", {L"Background:=#27403C"}},
    {L"TaskButton", {L"Background:=#27403C"}},
    {L"AppTitleButton", {L"Background:=#27403C"}},
    {L"Canvas > FlyoutPresenter > Grid > Border#PART_BackgroundBorder",
     {L"IconColor=#7FD1C4", L"Background:=#1B2E2B"}},
    {L"FlyoutBlurHost", {L"Background:=transparent"}},
    {L"Canvas > MenuFlyoutPresenter > Grid > Border#PART_BackgroundBorder",
     {L"Background:=#1B2E2B"}},
};

const std::vector<ControlStyleRule> g_themeNoIslandsStyles = {
    {L"TopBarRoot", {L"Margin=0", L"CornerRadius=0"}},
    {L"WeatherButton", {L"Background:=transparent"}},
    {L"RecycleBinButton", {L"Background:=transparent"}},
    {L"SettingsButton", {L"Background:=transparent"}},
    {L"StartButton", {L"Background:=transparent"}},
    {L"SearchButton", {L"Background:=transparent"}},
    {L"ClockButton", {L"Background:=transparent"}},
    {L"DisplayButton", {L"Background:=transparent"}},
    {L"SoundButton", {L"Background:=transparent"}},
    {L"WifiButton", {L"Background:=transparent"}},
    {L"BluetoothButton", {L"Background:=transparent"}},
    {L"BatteryButton", {L"Background:=transparent"}},
    {L"ResourceButton", {L"Background:=transparent"}},
    {L"TaskButton", {L"Background:=transparent"}},
    {L"AppTitleButton", {L"Background:=transparent"}},
};

const std::vector<ControlStyleRule> g_themeOS27GoldenGateStyles = {
    {L"Grid#BarRoot > Grid#TopBarRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"3\" />",
        L"BorderThickness=1",
        L"CornerRadius=14",
        L"Margin=6,4,6,0"}},
    {L"Canvas > FlyoutPresenter > Grid > Border#PART_BackgroundBorder", {
        L"CornerRadius=20",
        L"BorderThickness=1"}},
    {L"FlyoutBlurHost", {
        L"Background:=<WindhawkBlur BlurAmount=\"3\" />"}},
    {L"Canvas > MenuFlyoutPresenter > Grid > Border#PART_BackgroundBorder", {
        L"CornerRadius=20",
        L"Background:=<WindhawkBlur BlurAmount=\"3\" />",
        L"BorderThickness=1"}},
    {L"StartButton, SearchButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"16\" TintColor=\"#20FFFFFF\" TintOpacity=\"0.2\" />",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.50,1.17\" EndPoint=\"0.50,-0.17\"><GradientStop Offset=\"0.09\" Color=\"#C9FFFFFF\"/><GradientStop Offset=\"0.46\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.49\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.91\" Color=\"#A8FFFFFF\"/></LinearGradientBrush>",
        L"BorderThickness=1",
        L"CornerRadius=10",
        L"Margin=5,3,3,3"}},
    {L"TaskButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"16\" TintColor=\"#20FFFFFF\" TintOpacity=\"0.2\" />",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.50,1.17\" EndPoint=\"0.50,-0.17\"><GradientStop Offset=\"0.09\" Color=\"#C9FFFFFF\"/><GradientStop Offset=\"0.46\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.49\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.91\" Color=\"#A8FFFFFF\"/></LinearGradientBrush>",
        L"BorderThickness=1",
        L"CornerRadius=10",
        L"Margin=3,3,3,3"}},
    {L"Button#AppTitleButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"16\" TintColor=\"#20FFFFFF\" TintOpacity=\"0.2\" />",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.50,1.17\" EndPoint=\"0.50,-0.17\"><GradientStop Offset=\"0.09\" Color=\"#C9FFFFFF\"/><GradientStop Offset=\"0.46\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.49\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.91\" Color=\"#A8FFFFFF\"/></LinearGradientBrush>",
        L"BorderThickness=1",
        L"CornerRadius=10",
        L"Margin=3,3,3,3"}},
    {L"DisplayButton, SoundButton, WifiButton, BluetoothButton, ResourceButton, BatteryButton, WeatherButton, RecycleBinButton, SettingsButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"16\" TintColor=\"#20FFFFFF\" TintOpacity=\"0.2\" />",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.50,1.17\" EndPoint=\"0.50,-0.17\"><GradientStop Offset=\"0.09\" Color=\"#C9FFFFFF\"/><GradientStop Offset=\"0.46\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.49\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.91\" Color=\"#A8FFFFFF\"/></LinearGradientBrush>",
        L"BorderThickness=1",
        L"CornerRadius=10",
        L"Margin=3,3,3,3"}},
    {L"ClockButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"16\" TintColor=\"#20FFFFFF\" TintOpacity=\"0.2\" />",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.50,1.17\" EndPoint=\"0.50,-0.17\"><GradientStop Offset=\"0.09\" Color=\"#C9FFFFFF\"/><GradientStop Offset=\"0.46\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.49\" Color=\"#001C1C1C\"/><GradientStop Offset=\"0.91\" Color=\"#A8FFFFFF\"/></LinearGradientBrush>",
        L"BorderThickness=1",
        L"CornerRadius=10",
        L"Margin=3,3,3,3"}},
};

// Global variable to hold the currently selected theme's styles
std::vector<ControlStyleRule> g_themeStyleRules;

// ============================================================================
// Globals
// ============================================================================


HANDLE g_topBarThread;
DWORD g_topBarThreadId;
HANDLE g_stopEvent = nullptr;  // Stop event for clean shutdown
HMODULE g_modModule = nullptr;

HWND g_topBarHwnd;
HWND g_islandHwnd;
[[clang::no_destroy]] wuxc::Grid g_wallpaperLayer{nullptr};  // Store the wallpaper layer for updates

int g_barHeightPx = 40;
double g_dpiScale = 1.0;

[[clang::no_destroy]] wuxh::WindowsXamlManager g_xamlManager{nullptr};
[[clang::no_destroy]] wuxh::DesktopWindowXamlSource g_desktopSource{nullptr};
[[clang::no_destroy]] winrt::Windows::System::DispatcherQueue g_uiDispatcherQueue{nullptr};

[[clang::no_destroy]] DispatcherTimer g_clockTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_blurRefreshTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_taskRefreshTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_taskListTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_taskClickTimer{nullptr};
HWND g_taskClickPendingHwnd = nullptr;
[[clang::no_destroy]] DispatcherTimer g_wifiAutoRefreshTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_bluetoothAutoRefreshTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_restoreTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_resourceTimer{nullptr};
[[clang::no_destroy]] wuxc::Button g_resourceButton{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_resourceFlyout{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_resourcePanel{nullptr};
[[clang::no_destroy]] DispatcherTimer g_resourceFlyoutTimer{nullptr};
[[clang::no_destroy]] wuxc::Canvas g_graphCanvas{nullptr};
[[clang::no_destroy]] winrt::Windows::UI::Xaml::Shapes::Polyline g_graphLine{nullptr};
[[clang::no_destroy]] winrt::Windows::UI::Xaml::Shapes::Polygon g_graphFill{nullptr};
[[clang::no_destroy]] wuxc::Grid g_statsGrid{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statLabel0{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statValue0{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statLabel1{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statValue1{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statLabel2{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statValue2{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statLabel3{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_statValue3{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_statCell0{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_statCell1{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_statCell2{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_statCell3{nullptr};
// Holds strong XAML references. The attribute prevents the destructor from
// running on the shutdown thread; the teardown block releases the elements on
// the UI thread instead.
[[clang::no_destroy]] std::vector<wuxc::Button> g_tabButtons;

// Info island globals
[[clang::no_destroy]] wuxc::TextBlock g_infoCpuName{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_infoRamName{nullptr};
[[clang::no_destroy]] wuxc::ComboBox g_infoGpuCombo{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_infoCpuLabel{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_infoRamLabel{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_infoGpuLabel{nullptr};
std::vector<std::wstring> g_gpuNames;
int g_selectedGpuIndex = 0;
bool g_selectedGpuIndexLoaded = false;
std::deque<float> g_cpuHistory;
std::deque<float> g_ramHistory;
std::map<int, std::deque<float>> g_gpuHistory;
int g_currentTab = 0; // 0=CPU, 1=RAM, 2=GPU

[[clang::no_destroy]] std::map<std::wstring, FrameworkElement> g_namedElements;
[[clang::no_destroy]] FrameworkElement g_rootElement{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_taskListPanel{nullptr};
[[clang::no_destroy]] std::vector<HWND> g_stableWindowOrder;
ULONGLONG g_lastDoubleTapTick = 0;
[[clang::no_destroy]] std::map<HWND, wuxc::Button> g_taskButtonsByHwnd;
[[clang::no_destroy]] std::map<HWND, std::wstring> g_taskButtonLastTitle;
// Helper to resize task buttons so they fit within the available width
void AdjustTaskButtonWidths() {
    static bool s_adjusting = false;
    if (s_adjusting) return;
    if (!g_taskListPanel) return;
    if (g_taskListPanel.Children().Size() == 0) return;
    s_adjusting = true;
    struct Guard {
        ~Guard() { s_adjusting = false; }
    } guard;

    // Determine available width for the task list column.
    double availableWidth = 0.0;
    if (g_rootElement) {
        double rootWidth = g_rootElement.ActualWidth();
        double leftWidth = 0.0, rightWidth = 0.0;
        auto leftIt = g_namedElements.find(L"LeftPanel");
        if (leftIt != g_namedElements.end()) leftWidth = leftIt->second.ActualWidth();
        auto rightIt = g_namedElements.find(L"TrayPanel");
        if (rightIt != g_namedElements.end()) rightWidth = rightIt->second.ActualWidth();
        availableWidth = rootWidth - leftWidth - rightWidth;
    } else {
        availableWidth = g_taskListPanel.ActualWidth(); // fallback
    }
    if (availableWidth <= 0) return;

    // Measure natural width of each button (its desired size without explicit width).
    double sumNaturalWidths = 0.0;
    double sumMargins = 0.0;
    std::vector<double> naturalWidths;
    for (auto&& child : g_taskListPanel.Children()) {
        if (auto button = child.try_as<wuxc::Button>()) {
            button.ClearValue(FrameworkElement::WidthProperty());
            button.Measure(winrt::Windows::Foundation::Size{
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max()
            });
            double natural = button.DesiredSize().Width;
            naturalWidths.push_back(natural);
            sumNaturalWidths += natural;
            sumMargins += (button.Margin().Left + button.Margin().Right);
        }
    }
    if (sumNaturalWidths <= 0) return;

    double spaceForWidths = availableWidth - sumMargins;
    if (spaceForWidths <= 0) return;

    if (sumNaturalWidths <= spaceForWidths) {
        // All buttons fit – clear any explicit width, keep auto sizing.
        for (auto&& child : g_taskListPanel.Children()) {
            if (auto button = child.try_as<wuxc::Button>()) {
                button.ClearValue(FrameworkElement::WidthProperty());
            }
        }
        return;
    }

    // Need to shrink – scale all button widths proportionally.
    double scale = spaceForWidths / sumNaturalWidths;
    size_t idx = 0;
    for (auto&& child : g_taskListPanel.Children()) {
        if (auto button = child.try_as<wuxc::Button>()) {
            double newWidth = naturalWidths[idx] * scale;
            newWidth = std::max(20.0, newWidth); // minimum width for usability
            button.Width(newWidth);
            idx++;
        }
    }
}

[[clang::no_destroy]] wuxc::MenuFlyout g_taskContextMenu{nullptr};
[[clang::no_destroy]] wuxc::MenuFlyoutItem g_taskMenuToggleItem{nullptr};
[[clang::no_destroy]] HWND g_contextMenuTargetHwnd;
[[clang::no_destroy]] HWND g_appTitleContextTarget;
[[clang::no_destroy]] wuxc::MenuFlyout g_startContextMenu{nullptr};
[[clang::no_destroy]] wuxc::MenuFlyout g_appTitleContextMenu{nullptr};

// Foreground tracking. Clicking a task button activates the bar itself, so
// GetForegroundWindow() can never equal the clicked window by the time the
// handler runs -- which is exactly why "click to minimize" never fired. A
// global EVENT_SYSTEM_FOREGROUND hook records the last real foreground window
// instead, ignoring anything owned by this process.
HWINEVENTHOOK g_windowEventHook;
HWINEVENTHOOK g_windowEventNameHook;
HWND g_lastForegroundHwnd;
HWINEVENTHOOK g_foregroundHook = nullptr;
bool g_fullScreenAppActive = false;   
bool g_allowHide = false;             

wui::Color g_iconTintColor{0, 255, 255, 255};
double g_iconTintOpacity = 0.0;
std::optional<wui::Color> g_iconColorOverride;

constexpr UINT WM_APPBAR_CALLBACK = WM_APP + 0x137;
constexpr UINT_PTR kAppBarInitTimerId = 1;
constexpr PCWSTR kWindowClassName = L"WindhawkTopBarWnd";

// Hotkey IDs for opening control flyouts.
constexpr int HOTKEY_ID_DISPLAY = 1;
constexpr int HOTKEY_ID_SOUND = 2;
constexpr int HOTKEY_ID_WIFI = 3;
constexpr int HOTKEY_ID_BLUETOOTH = 4;
constexpr int HOTKEY_ID_RESOURCE = 0;
constexpr int HOTKEY_ID_BATTERY = 5;

constexpr int HOTKEY_ID_START_MENU = 6;
constexpr int HOTKEY_ID_TASK_MENU = 7;
constexpr int HOTKEY_ID_WEATHER = 8;
constexpr int HOTKEY_ID_RECYCLEBIN = 9;
UINT g_taskbarCreatedMsg = 0;
bool g_appBarRegistered = false;


// ============================================================================
// String helpers
// ============================================================================

std::wstring GetStringSettingCopy(PCWSTR name) {
    {
        wchar_t buf[2048] = {0};
        size_t n = Wh_GetStringValue(name, buf, ARRAYSIZE(buf));
        if (n > 0) return std::wstring(buf, n);
    }
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

static const std::map<std::wstring, int>& IntDefaults() {
    static const std::map<std::wstring, int> kDefaults = {
        { L"barHeight", 40 },
        { L"cornerRadius", 6 },
        { L"topBarBackgroundOpacity", 25 },
        { L"globalBlurAmount", 6 },
        { L"taskButtonWidth", 100 },
        { L"taskIconSize", 20 },
        { L"showStartButton", 1 },
        { L"showSearchButton", 1 },
        { L"showDisplayButton", 1 },
        { L"showSoundButton", 1 },
        { L"showWifiButton", 1 },
        { L"showBluetoothButton", 1 },
        { L"showBatteryButton", 1 },
        { L"showSettingsButton", 1 },
        { L"showWeatherButton", 1 },
        { L"showClock", 1 },
        { L"showDate", 1 },
        { L"showCpuUsage", 1 },
        { L"showRamUsage", 1 },
        { L"showGpuUsage", 1 },
        { L"fontBold", 0 },
    };
    return kDefaults;
}

static const std::map<std::wstring, std::wstring>& StringDefaults() {
    static const std::map<std::wstring, std::wstring> kDefaults = {
        { L"topBarBackgroundColor", L"#000000" },
        { L"iconColor", L"#FFFFFF" },
        { L"timeFormat", L"\U0001F551hh:mm tt" },
        { L"dateFormat", L"\U0001F4C5ddd, MMM dd" },
    };
    return kDefaults;
}

int ReadIntSetting(PCWSTR name, int fallback = 0) {
    {
        wchar_t buf[64] = {0};
        size_t n = Wh_GetStringValue(name, buf, ARRAYSIZE(buf));
        if (n > 0) {
            try { return std::stoi(buf); } catch (...) {}
        }
    }
    int v = Wh_GetIntSetting(name);
    if (v != 0) return v;
    if (fallback != 0) return fallback;
    auto it = IntDefaults().find(name);
    if (it != IntDefaults().end()) return it->second;
    return 0;
}

// Array settings are addressed with a format string ("controlStyles[%d].target"),
// which Wh_GetStringSetting takes variadically.
template <typename... Args>
std::wstring GetStringSettingCopy(PCWSTR name, Args... args) {
    PCWSTR value = Wh_GetStringSetting(name, args...);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

// Reads a setting from the mod-editor schema ONLY, ignoring Wh storage.
// Use for schema-only keys (theme, monitorIndex) that the in-app settings
// window never writes — so a stale storage value from an older build can't
// silently mask the value picked in the mod editor.
std::wstring GetModEditorStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring TrimWs(std::wstring_view s) {
    size_t b = s.find_first_not_of(L" \t\r\n");
    if (b == std::wstring_view::npos) {
        return L"";
    }
    size_t e = s.find_last_not_of(L" \t\r\n");
    return std::wstring(s.substr(b, e - b + 1));
}

std::wstring EscapeXmlAttr(std::wstring_view s) {
    std::wstring out;
    out.reserve(s.size());
    for (wchar_t c : s) {
        switch (c) {
            case L'&': out += L"&amp;"; break;
            case L'"': out += L"&quot;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            default: out.push_back(c);
        }
    }
    return out;
}

std::wstring ToLowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring ApplyStyleConstants(std::wstring_view value) {
    std::wstring result;
    size_t lastPos = 0;
    size_t findPos;
    while ((findPos = value.find(L'$', lastPos)) != std::wstring_view::npos) {
        result.append(value, lastPos, findPos - lastPos);

        const std::pair<std::wstring, std::wstring>* match = nullptr;
        for (const auto& c : g_styleConstants) {
            if (value.substr(findPos + 1, c.first.size()) == c.first) {
                if (!match || c.first.size() > match->first.size()) {
                    match = &c;
                }
            }
        }

        if (match) {
            result += match->second;
            lastPos = findPos + 1 + match->first.size();
        } else {
            result += L'$';
            lastPos = findPos + 1;
        }
    }
    result.append(value.substr(lastPos));
    return result;
}

// ============================================================================
// Control style engine
// ============================================================================

struct TreeElementMatcher {
    std::wstring className;
    std::wstring name;
    std::wstring visualStateGroupName;
    std::vector<std::pair<std::wstring, std::wstring>> propertyFilters;
    int oneBasedIndex = 0;
    bool bareIdentifier = false;
    bool wildcard = false;
    bool rootRequired = false;
};

std::wstring ShortClassName(winrt::hstring const& fullName) {
    std::wstring_view view(fullName);
    auto dotPos = view.rfind(L'.');
    return std::wstring(dotPos != std::wstring_view::npos ? view.substr(dotPos + 1) : view);
}

TreeElementMatcher ParseMatcherPart(std::wstring_view part) {
    TreeElementMatcher m;
    std::wstring_view s = part;
    if (TrimWs(s) == L"*") {
        m.wildcard = true;
        return m;
    }

    size_t first = s.find_first_of(L"#@[");
    std::wstring typePart(TrimWs(s.substr(0, first)));
    if (first == std::wstring_view::npos) {
        m.name = typePart;
        m.bareIdentifier = true;
        return m;
    }
    m.className = typePart;

    while (first != std::wstring_view::npos) {
        size_t next = s.find_first_of(L"#@[", first + 1);
        std::wstring_view chunk = s.substr(first + 1,
            next == std::wstring_view::npos ? std::wstring_view::npos : next - first - 1);
        switch (s[first]) {
            case L'#':
                m.name = std::wstring(TrimWs(chunk));
                break;
            case L'@':
                m.visualStateGroupName = std::wstring(TrimWs(chunk));
                break;
            case L'[': {
                std::wstring rule(TrimWs(chunk));
                if (!rule.empty() && rule.back() == L']') rule.pop_back();
                if (!rule.empty()) {
                    bool allDigits =
                        rule.find_first_not_of(L"0123456789") == std::wstring::npos;
                    if (allDigits) {
                        try { m.oneBasedIndex = std::stoi(rule); } catch (...) {}
                    } else {
                        auto eq = rule.find(L'=');
                        if (eq != std::wstring::npos) {
                            m.propertyFilters.emplace_back(
                                std::wstring(TrimWs(rule.substr(0, eq))),
                                std::wstring(TrimWs(rule.substr(eq + 1))));
                        }
                    }
                }
                break;
            }
        }
        first = next;
    }
    if (m.className.empty() && !m.name.empty()) {
        m.bareIdentifier = true;
    }
    return m;
}

std::vector<TreeElementMatcher> ParseTargetChain(std::wstring_view target) {
    std::vector<TreeElementMatcher> result;
    size_t pos = 0;
    bool rootRequired = false;
    // Check the whole string before splitting.
    if (target.starts_with(L":root > ")) {
        rootRequired = true;
        target = target.substr(8); // remove ":root > "
    }
    while (pos <= target.size()) {
        size_t arrow = target.find(L" > ", pos);
        auto partSv = target.substr(
            pos, arrow == std::wstring_view::npos ? std::wstring_view::npos : arrow - pos);
        std::wstring trimmed = TrimWs(partSv);
        if (rootRequired) {
            auto matcher = ParseMatcherPart(trimmed);
            matcher.rootRequired = true;
            result.push_back(std::move(matcher));
            rootRequired = false; // only first matcher is root‑required
        } else {
            result.push_back(ParseMatcherPart(trimmed));
        }
        if (arrow == std::wstring_view::npos) {
            break;
        }
        pos = arrow + 3;
    }
    return result;
}

thread_local std::map<std::pair<std::wstring, std::wstring>, DependencyProperty> g_propNameCache;

DependencyProperty ResolvePropertyByName(std::wstring const& shortTypeName,
                                         std::wstring const& propName) {
    auto key = std::make_pair(shortTypeName, propName);
    auto it = g_propNameCache.find(key);
    if (it != g_propNameCache.end()) return it->second;

    std::wstring xaml =
        L"<ResourceDictionary "
        L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
        L"<Style TargetType=\"" + EscapeXmlAttr(shortTypeName) + L"\">"
        L"<Setter Property=\"" + EscapeXmlAttr(propName) + L"\" Value=\"{x:Null}\" />"
        L"</Style></ResourceDictionary>";
    DependencyProperty dp{nullptr};
    try {
        auto dict = Markup::XamlReader::Load(xaml).as<ResourceDictionary>();
        auto [k, s] = dict.First().Current();
        auto style = s.as<Style>();
        dp = style.Setters().GetAt(0).as<Setter>().Property();
    } catch (...) {}
    g_propNameCache.emplace(key, dp);
    return dp;
}

bool TestTreeMatcher(FrameworkElement const& element, TreeElementMatcher const& m) {
    if (m.wildcard) {
        return true;
    }
    
    // ':root' constraint: the element must have no parent FrameworkElement
    if (m.rootRequired) {
        DependencyObject parent = wuxm::VisualTreeHelper::GetParent(element);
        if (parent) {
            return false;
        }
    }
    
    std::wstring elementName(element.Name());
    auto className = winrt::get_class_name(element);
    std::wstring shortName = ShortClassName(className);

    if (m.bareIdentifier) {
        if (!m.name.empty() && elementName == m.name) return true;
        return shortName == m.name || std::wstring(className) == m.name;
    }

    if (!m.className.empty()) {
        std::wstring want = m.className;
        if (want.starts_with(L"muxc:"))
            want = L"Microsoft.UI.Xaml.Controls." + want.substr(5);
        else if (want.starts_with(L"taskbar:"))
            want = L"Taskbar." + want.substr(8);
        else if (want.starts_with(L"systemtray:"))
            want = L"SystemTray." + want.substr(11);
        else if (want.starts_with(L"udk:"))
            want = L"WindowsUdk.UI.Shell." + want.substr(4);
        if (want.find(L'.') == std::wstring::npos) {
            if (shortName != want && std::wstring(className) != want) return false;
        } else {
            if (std::wstring(className) != want && shortName != want) return false;
        }
    }
    if (!m.name.empty() && elementName != m.name) return false;

    if (m.oneBasedIndex > 0) {
        auto parent = wuxm::VisualTreeHelper::GetParent(element);
        if (!parent) return false;
        int idx = m.oneBasedIndex - 1;
        if (idx < 0 || idx >= wuxm::VisualTreeHelper::GetChildrenCount(parent) ||
            wuxm::VisualTreeHelper::GetChild(parent, idx) != element) {
            return false;
        }
    }

    for (auto const& [propName, want] : m.propertyFilters) {
        DependencyProperty dp = ResolvePropertyByName(shortName, propName);
        if (!dp) return false;
        auto actual = element.ReadLocalValue(dp);
        if (!actual || actual == DependencyProperty::UnsetValue()) return false;
        std::wstring actualStr;
        try {
            if (auto pv = actual.try_as<wf::IPropertyValue>()) {
                if (pv.Type() == wf::PropertyType::String) {
                    actualStr = pv.GetString().c_str();
                } else if (pv.Type() == wf::PropertyType::Boolean) {
                    actualStr = pv.GetBoolean() ? L"True" : L"False";
                } else if (pv.Type() == wf::PropertyType::Double) {
                    wchar_t b[64]; swprintf_s(b, L"%g", pv.GetDouble());
                    actualStr = b;
                } else if (pv.Type() == wf::PropertyType::Int32) {
                    actualStr = std::to_wstring(pv.GetInt32());
                } else if (pv.Type() == wf::PropertyType::UInt32) {
                    actualStr = std::to_wstring(pv.GetUInt32());
                }
            }
        } catch (...) {}
        if (actualStr != want) return false;
    }

    return true;
}

bool MatchesAncestorChain(FrameworkElement const& element,
                          std::vector<TreeElementMatcher> const& chain) {
    if (chain.size() <= 1) return true;
    DependencyObject current = element;
    int chainIndex = static_cast<int>(chain.size()) - 2;
    while (chainIndex >= 0) {
        // Wildcard: skip any number of ancestors to match the next matcher
        if (chain[chainIndex].wildcard) {
            int nextIdx = chainIndex - 1; // the matcher after the wildcard (since chain is reversed)
            if (nextIdx < 0) {
                return true; // leading wildcard: no further ancestor constraints
            }
            bool matched = false;
            while (current) {
                auto fe = current.try_as<FrameworkElement>();
                if (fe && TestTreeMatcher(fe, chain[nextIdx])) {
                    // found a matching ancestor; move current to this ancestor
                    chainIndex = nextIdx - 1;
                    matched = true;
                    break;
                }
                current = wuxm::VisualTreeHelper::GetParent(current);
            }
            if (!matched) return false;
            continue;
        }
        // Normal matcher: walk one parent up
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (!current) return false;
        auto fe = current.try_as<FrameworkElement>();
        if (!fe || !TestTreeMatcher(fe, chain[chainIndex])) return false;
        chainIndex--;
    }
    return true;
}

void CollectMatchingElements(DependencyObject const& node,
                             std::vector<TreeElementMatcher> const& chain,
                             std::vector<FrameworkElement>& results) {
    if (!node) {
        return;
    }
    try {
        if (auto fe = node.try_as<FrameworkElement>()) {
            if (TestTreeMatcher(fe, chain.back()) && MatchesAncestorChain(fe, chain)) {
                results.push_back(fe);
            }
        }
        int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
        for (int i = 0; i < count; i++) {
            CollectMatchingElements(wuxm::VisualTreeHelper::GetChild(node, i), chain, results);
        }
    } catch (...) {
    }
}

// Flyout and context menu content lives in separate popup trees, not under the
// main root, so name-only matches are also checked against them.
[[clang::no_destroy]] std::vector<FrameworkElement> g_detachedStyleRoots;

std::vector<FrameworkElement> ResolveGeneralTarget(const std::wstring& target) {
    std::vector<FrameworkElement> results;
    auto chain = ParseTargetChain(target);
    if (chain.empty()) {
        return results;
    }

    if (g_rootElement) {
        CollectMatchingElements(g_rootElement, chain, results);
    }
    for (auto& root : g_detachedStyleRoots) {
        if (root) {
            CollectMatchingElements(root, chain, results);
        }
    }

    if (chain.size() == 1 && !chain.back().name.empty()) {
        auto tryMenu = [&](wuxc::MenuFlyout const& menu) {
            if (!menu) {
                return;
            }
            for (auto const& item : menu.Items()) {
                if (auto fe = item.try_as<FrameworkElement>()) {
                    if (TestTreeMatcher(fe, chain.back())) {
                        results.push_back(fe);
                    }
                }
                if (auto sub = item.try_as<wuxc::MenuFlyoutSubItem>()) {
                    for (auto const& subItem : sub.Items()) {
                        if (auto subFe = subItem.try_as<FrameworkElement>()) {
                            if (TestTreeMatcher(subFe, chain.back())) {
                                results.push_back(subFe);
                            }
                        }
                    }
                }
            }
        };
        tryMenu(g_taskContextMenu);
        tryMenu(g_startContextMenu);
    }

    return results;
}

struct WindhawkBlurParams {
    float blurAmount = 5.0f;
    wui::Color tint{0, 0, 0, 0};
    std::optional<uint8_t> tintOpacity;
};

std::optional<WindhawkBlurParams> ParseWindhawkBlurValue(std::wstring_view value);

wuxm::XamlCompositionBrushBase CreateWindhawkBlurBrush(
    UIElement const& element, WindhawkBlurParams const& params);

bool ElementIsInVisualState(FrameworkElement const& element, std::wstring const& stateName);

bool TryParseHexColor(const std::wstring& text, wui::Color* outColor);
bool TryParseNamedColor(const std::wstring& text, wui::Color* outColor);
void ApplyIconColorToElement(FrameworkElement element, const std::wstring& value);

struct StyleCaptureRegistry {
    std::map<std::wstring, std::map<std::wstring, std::wstring>> values;
};
[[clang::no_destroy]] StyleCaptureRegistry g_styleCaptures;

std::wstring FormatDoubleInvariant(double d);
bool TryParseNumberInvariant(std::wstring_view s, double* out);
std::wstring GetStyleCaptureKey(winrt::Windows::UI::Xaml::XamlRoot const& root);
std::wstring ExpandStyleVariables(std::wstring_view input, XamlRoot const& root);

void ApplySingleStyleToElement(FrameworkElement element, const std::wstring& rule) {
    std::wstring ruleWithConstants = ApplyStyleConstants(rule);
    std::wstring trimmedRule = TrimWs(ruleWithConstants);
    if (trimmedRule.empty() || trimmedRule.starts_with(L"//")) {
        return;
    }

    size_t eqPos = trimmedRule.find(L'=');
    if (eqPos == std::wstring::npos) {
        Wh_Log(L"Bad style syntax (missing '='): %s", trimmedRule.c_str());
        return;
    }

    std::wstring propPart = trimmedRule.substr(0, eqPos);
    std::wstring valuePart = TrimWs(trimmedRule.substr(eqPos + 1));

    if (!valuePart.empty() && valuePart.front() == L'>') {
        std::wstring varName = TrimWs(valuePart.substr(1));
        std::wstring propName = TrimWs(propPart);
        if (!varName.empty() && !propName.empty()) {
            auto root = element.XamlRoot();
            if (root) {
                DependencyProperty dp = ResolvePropertyByName(
                    ShortClassName(winrt::get_class_name(element)), propName);
                if (dp) {
                    auto read = element.ReadLocalValue(dp);
                    std::wstring str;
                    try {
                        if (auto pv = read.try_as<wf::IPropertyValue>()) {
                            if (pv.Type() == wf::PropertyType::String)
                                str = pv.GetString().c_str();
                            else if (pv.Type() == wf::PropertyType::Boolean)
                                str = pv.GetBoolean() ? L"1" : L"0";
                            else if (pv.Type() == wf::PropertyType::Double) {
                                str = FormatDoubleInvariant(pv.GetDouble());
                            } else if (pv.Type() == wf::PropertyType::Int32) {
                                str = std::to_wstring(pv.GetInt32());
                            } else if (pv.Type() == wf::PropertyType::UInt32) {
                                str = std::to_wstring(pv.GetUInt32());
                            }
                        }
                    } catch (...) {}
                    g_styleCaptures.values[GetStyleCaptureKey(root)][varName] = str;
                    try {
                        element.RegisterPropertyChangedCallback(dp,
                            [varName, dp](DependencyObject sender, DependencyProperty) {
                                auto fe = sender.try_as<FrameworkElement>();
                                if (!fe) return;
                                auto root = fe.XamlRoot();
                                if (!root) return;
                                auto read = fe.ReadLocalValue(dp);
                                std::wstring s;
                                try {
                                    if (auto pv = read.try_as<wf::IPropertyValue>()) {
                                        if (pv.Type() == wf::PropertyType::Double)
                                            s = FormatDoubleInvariant(pv.GetDouble());
                                        else if (pv.Type() == wf::PropertyType::Int32)
                                            s = std::to_wstring(pv.GetInt32());
                                        else if (pv.Type() == wf::PropertyType::UInt32)
                                            s = std::to_wstring(pv.GetUInt32());
                                        else if (pv.Type() == wf::PropertyType::Boolean)
                                            s = pv.GetBoolean() ? L"1" : L"0";
                                        else if (pv.Type() == wf::PropertyType::String)
                                            s = pv.GetString().c_str();
                                    }
                                } catch (...) {}
                                g_styleCaptures.values[GetStyleCaptureKey(root)][varName] = s;
                            });
                    } catch (...) {}
                }
            }
        }
        return;
    }

    if (auto root = element.XamlRoot()) {
        valuePart = ExpandStyleVariables(valuePart, root);
    }

    bool isXamlValue = false;
    std::wstring trimmedProp = TrimWs(propPart);
    if (!trimmedProp.empty() && trimmedProp.back() == L':') {
        isXamlValue = true;
        trimmedProp.pop_back();
        trimmedProp = TrimWs(trimmedProp);
    }
    if (trimmedProp.empty()) {
        Wh_Log(L"Bad style syntax (empty property): %s", trimmedRule.c_str());
        return;
    }

    auto className = winrt::get_class_name(element);
    std::wstring_view classNameView(className);
    auto dotPos = classNameView.rfind(L'.');
    std::wstring shortTypeName(dotPos != std::wstring_view::npos ? classNameView.substr(dotPos + 1)
                                                                 : classNameView);

    if (isXamlValue && valuePart.starts_with(L"<WindhawkBlur ")) {
        if (auto blur = ParseWindhawkBlurValue(valuePart)) {
            if (auto ui = element.try_as<UIElement>()) {
                std::wstring probeXaml =
                    L"<ResourceDictionary "
                    L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
                    L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
                    L"<Style TargetType=\"" + shortTypeName + L"\">"
                    L"<Setter Property=\"" + trimmedProp + L"\">"
                    L"<Setter.Value><SolidColorBrush Color=\"Transparent\"/></Setter.Value>"
                    L"</Setter></Style></ResourceDictionary>";
                try {
                    auto dict = Markup::XamlReader::Load(probeXaml).as<ResourceDictionary>();
                    auto [key, styleObj] = dict.First().Current();
                    auto style = styleObj.as<Style>();
                    auto setter = style.Setters().GetAt(0).as<Setter>();
                    auto brush = CreateWindhawkBlurBrush(ui, *blur);
                    element.SetValue(setter.Property(), brush);
                    return;
                } catch (winrt::hresult_error const& ex) {
                    Wh_Log(L"WindhawkBlur apply failed: %08X",
                           static_cast<unsigned int>(ex.code().value));
                }
            }
        }
    }

    std::wstring visualState;
    auto atPos = trimmedProp.find(L'@');
    if (atPos != std::wstring::npos) {
        visualState = TrimWs(trimmedProp.substr(atPos + 1));
        trimmedProp = TrimWs(trimmedProp.substr(0, atPos));
    }
    if (!visualState.empty() && !ElementIsInVisualState(element, visualState)) {
        return;
    }

    if (!isXamlValue && trimmedProp == L"IconColor") {
        wui::Color parsed{};
        if (TryParseHexColor(valuePart, &parsed) || TryParseNamedColor(valuePart, &parsed)) {
            if (std::wstring_view(element.Name()) == L"TopBarRoot") {
                g_iconColorOverride = parsed;
            }
        }
        ApplyIconColorToElement(element, valuePart);
        return;
    }

    if (isXamlValue) {
        std::wstring_view v(valuePart);
        if (v.starts_with(L"<WindhawkBlur")) {
            if (auto blur = ParseWindhawkBlurValue(v)) {
                if (auto ui = element.try_as<UIElement>()) {
                    std::wstring probe =
                        L"<ResourceDictionary "
                        L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
                        L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
                        L"<Style TargetType=\"" + shortTypeName + L"\">"
                        L"<Setter Property=\"" + trimmedProp + L"\">"
                        L"<Setter.Value><SolidColorBrush Color=\"Transparent\"/></Setter.Value>"
                        L"</Setter></Style></ResourceDictionary>";
                    try {
                        auto d = Markup::XamlReader::Load(probe).as<ResourceDictionary>();
                        auto [k, so] = d.First().Current();
                        auto st = so.as<Style>();
                        auto set = st.Setters().GetAt(0).as<Setter>();
                        auto brush = CreateWindhawkBlurBrush(ui, *blur);
                        element.SetValue(set.Property(), brush);
                        return;
                    } catch (winrt::hresult_error const& ex) {
                        Wh_Log(L"WindhawkBlur apply failed: %08X",
                               static_cast<unsigned int>(ex.code().value));
                    }
                }
            }
        }
    }

    std::wstring setterXaml = L"<Setter Property=\"" + trimmedProp + L"\"";
    if (!isXamlValue) {
        setterXaml += L" Value=\"" + EscapeXmlAttr(valuePart) + L"\" />";
    } else if (valuePart.empty()) {
        setterXaml += L" Value=\"{x:Null}\" />";
    } else {
        setterXaml += L"><Setter.Value>" + valuePart + L"</Setter.Value></Setter>";
    }

    std::wstring styleXaml =
        L"<ResourceDictionary "
        L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
        L"<Style TargetType=\"" +
        shortTypeName + L"\">" + setterXaml + L"</Style></ResourceDictionary>";

    try {
        auto dict = Markup::XamlReader::Load(styleXaml).as<ResourceDictionary>();
        auto [key, styleObj] = dict.First().Current();
        auto style = styleObj.as<Style>();
        auto setter = style.Setters().GetAt(0).as<Setter>();
        auto value = setter.Value();
        if (value == DependencyProperty::UnsetValue()) {
            element.ClearValue(setter.Property());
        } else {
            element.SetValue(setter.Property(), value);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Style apply failed (%s): %08X", trimmedRule.c_str(),
               static_cast<unsigned int>(ex.code().value));
    } catch (std::exception const& ex) {
        Wh_Log(L"Style apply failed (%s): %S", trimmedRule.c_str(), ex.what());
    }
}

bool TryParseHexColor(const std::wstring& text, wui::Color* outColor) {
    std::wstring hex = TrimWs(text);
    if (!hex.empty() && hex.front() == L'#') {
        hex.erase(0, 1);
    }
    if (hex.size() != 6 && hex.size() != 8) {
        return false;
    }
    try {
        uint32_t value = std::stoul(hex, nullptr, 16);
        uint8_t a = hex.size() == 8 ? static_cast<uint8_t>((value >> 24) & 0xFF) : 255;
        uint8_t r = static_cast<uint8_t>((value >> 16) & 0xFF);
        uint8_t g = static_cast<uint8_t>((value >> 8) & 0xFF);
        uint8_t b = static_cast<uint8_t>(value & 0xFF);
        *outColor = wui::ColorHelper::FromArgb(a, r, g, b);
        return true;
    } catch (...) {
        return false;
    }
}
// Parses a color name like "red", "blue", etc.
bool TryParseNamedColor(const std::wstring& text, wui::Color* outColor) {
    std::wstring lower = ToLowerCopy(text);
    if (lower == L"black") { *outColor = wui::ColorHelper::FromArgb(255,0,0,0); return true; }
    if (lower == L"white") { *outColor = wui::ColorHelper::FromArgb(255,255,255,255); return true; }
    if (lower == L"red")   { *outColor = wui::ColorHelper::FromArgb(255,255,0,0);   return true; }
    if (lower == L"green") { *outColor = wui::ColorHelper::FromArgb(255,0,128,0);   return true; }
    if (lower == L"blue")  { *outColor = wui::ColorHelper::FromArgb(255,0,0,255);   return true; }
    if (lower == L"yellow"){ *outColor = wui::ColorHelper::FromArgb(255,255,255,0); return true; }
    if (lower == L"orange"){ *outColor = wui::ColorHelper::FromArgb(255,255,165,0); return true; }
    if (lower == L"gray" || lower == L"grey") { *outColor = wui::ColorHelper::FromArgb(255,128,128,128); return true; }
    if (lower == L"purple"){ *outColor = wui::ColorHelper::FromArgb(255,128,0,128); return true; }
    if (lower == L"pink")  { *outColor = wui::ColorHelper::FromArgb(255,255,192,203); return true; }
    // Add more if needed...
    return false;
}

// Combines hex/name parsing with opacity
bool ParseBarColor(const std::wstring& text, int opacity, wui::Color* outColor) {
    wui::Color baseColor{};
    if (text.size() == 7 && text.front() == L'#') { // #RRGGBB
        if (!TryParseHexColor(text, &baseColor)) return false;
    } else if (text.size() == 9 && text.front() == L'#') { // #AARRGGBB
        if (!TryParseHexColor(text, &baseColor)) return false;
    } else {
        if (!TryParseNamedColor(text, &baseColor)) return false;
    }
    // Combine with opacity (0-100)
    uint8_t alpha = static_cast<uint8_t>((opacity * 255) / 100);
    *outColor = wui::ColorHelper::FromArgb(alpha, baseColor.R, baseColor.G, baseColor.B);
    return true;
}
// RefreshTaskList() ends by re-applying styles, and applying an IconTint* style
// asks for a refresh -- so without this guard the two would call each other
// forever.
bool g_applyingStyles = false;

std::wstring FormatDoubleInvariant(double d) {
    wchar_t buf[64];
    swprintf_s(buf, L"%.10g", d);
    return buf;
}

bool TryParseNumberInvariant(std::wstring_view s, double* out) {
    try {
        size_t used = 0;
        std::wstring tmp(s);
        double v = std::stod(tmp, &used);
        if (used != tmp.size()) return false;
        *out = v;
        return true;
    } catch (...) { return false; }
}

class StyleExprEval {
public:
    StyleExprEval(std::wstring_view t, XamlRoot const& root)
        : m_text(t), m_root(root) {}
    std::wstring Evaluate() {
        m_pos = 0; Skip();
        double v = ParseExpr();
        return FormatDoubleInvariant(v);
    }
private:
    void Skip() { while (m_pos < m_text.size() && iswspace(m_text[m_pos])) m_pos++; }
    bool Eat(wchar_t c) { Skip(); if (m_pos < m_text.size() && m_text[m_pos] == c) { m_pos++; return true; } return false; }
    double ParseExpr() { return ParseTernary(); }
    double ParseTernary() {
        double c = ParseEquality();
        if (Eat(L'?')) {
            double a = ParseExpr();
            Eat(L':');
            double b = ParseTernary();
            return c != 0.0 ? a : b;
        }
        return c;
    }
    double ParseEquality() {
        double a = ParseRel();
        while (true) {
            if (m_text.size() - m_pos >= 2 && m_text.compare(m_pos, 2, L"==") == 0) { m_pos += 2; double b = ParseRel(); a = (a == b) ? 1.0 : 0.0; }
            else if (m_text.size() - m_pos >= 2 && m_text.compare(m_pos, 2, L"!=") == 0) { m_pos += 2; double b = ParseRel(); a = (a != b) ? 1.0 : 0.0; }
            else break;
        }
        return a;
    }
    double ParseRel() {
        double a = ParseAdd();
        while (true) {
            if (m_text.size() - m_pos >= 2 && m_text.compare(m_pos, 2, L"<=") == 0) { m_pos += 2; a = (a <= ParseAdd()) ? 1.0 : 0.0; }
            else if (m_text.size() - m_pos >= 2 && m_text.compare(m_pos, 2, L">=") == 0) { m_pos += 2; a = (a >= ParseAdd()) ? 1.0 : 0.0; }
            else if (m_pos < m_text.size() && m_text[m_pos] == L'<') { m_pos++; a = (a < ParseAdd()) ? 1.0 : 0.0; }
            else if (m_pos < m_text.size() && m_text[m_pos] == L'>') { m_pos++; a = (a > ParseAdd()) ? 1.0 : 0.0; }
            else break;
        }
        return a;
    }
    double ParseAdd() {
        double a = ParseMul();
        while (true) {
            Skip();
            if (Eat(L'+')) a += ParseMul();
            else if (Eat(L'-')) a -= ParseMul();
            else break;
        }
        return a;
    }
    double ParseMul() {
        double a = ParseUnary();
        while (true) {
            Skip();
            if (Eat(L'*')) a *= ParseUnary();
            else if (Eat(L'/')) { double d = ParseUnary(); if (d != 0.0) a /= d; }
            else break;
        }
        return a;
    }
    double ParseUnary() {
        Skip();
        if (Eat(L'-')) return -ParseUnary();
        if (Eat(L'+')) return ParseUnary();
        return ParsePrim();
    }
    double ParsePrim() {
        Skip();
        if (Eat(L'(')) { double v = ParseExpr(); Eat(L')'); return v; }
        if (m_pos < m_text.size() && (iswdigit(m_text[m_pos]) || m_text[m_pos] == L'.')) {
            size_t st = m_pos;
            while (m_pos < m_text.size() && (iswdigit(m_text[m_pos]) || m_text[m_pos] == L'.')) m_pos++;
            double v = 0;
            if (TryParseNumberInvariant(m_text.substr(st, m_pos - st), &v)) return v;
            return 0;
        }
        if (m_pos < m_text.size() && (iswalpha(m_text[m_pos]) || m_text[m_pos] == L'_')) {
            size_t st = m_pos;
            while (m_pos < m_text.size() && (iswalnum(m_text[m_pos]) || m_text[m_pos] == L'_')) m_pos++;
            std::wstring name(m_text.substr(st, m_pos - st));
            Skip();
            if (m_pos < m_text.size() && m_text[m_pos] == L'(') {
                m_pos++;
                double a = ParseExpr();
                Eat(L',');
                double b = ParseExpr();
                Eat(L')');
                if (name == L"min") return a < b ? a : b;
                if (name == L"max") return a > b ? a : b;
                return 0;
            }
            auto rootIt = g_styleCaptures.values.find(GetStyleCaptureKey(m_root));
            if (rootIt != g_styleCaptures.values.end()) {
                auto vIt = rootIt->second.find(name);
                if (vIt != rootIt->second.end()) {
                    double v = 0;
                    if (TryParseNumberInvariant(vIt->second, &v)) return v;
                }
            }
            return 0;
        }
        return 0;
    }

    std::wstring_view m_text;
    XamlRoot m_root;
    size_t m_pos = 0;
};

std::wstring ExpandStyleVariables(std::wstring_view input, XamlRoot const& root) {
    std::wstring result(input);
    size_t pos = 0;
    while (true) {
        size_t open = result.find(L"{{", pos);
        if (open == std::wstring::npos) break;
        size_t close = result.find(L"}}", open + 2);
        if (close == std::wstring::npos) break;
        std::wstring_view body(result.data() + open + 2, close - open - 2);
        std::wstring expanded;
        bool isBare = body.find_first_of(L"+-*/<>=?!(),") == std::wstring_view::npos;
        if (isBare) {
            std::wstring name(TrimWs(body));
            auto rootIt = g_styleCaptures.values.find(GetStyleCaptureKey(root));
            if (rootIt != g_styleCaptures.values.end()) {
                auto vIt = rootIt->second.find(name);
                if (vIt != rootIt->second.end()) expanded = vIt->second;
            }
        } else {
            try {
                StyleExprEval eval(body, root);
                expanded = eval.Evaluate();
            } catch (...) {}
        }
        result.replace(open, close + 2 - open, expanded);
        pos = open + expanded.size();
    }
    return result;
}

std::wstring GetStyleCaptureKey(winrt::Windows::UI::Xaml::XamlRoot const& root) {
    if (!root) return L"";
    wchar_t buf[32];
    swprintf_s(buf, L"%p", winrt::get_abi(root));
    return buf;
}

void ApplyRuleList(const std::vector<ControlStyleRule>& rules) {
    for (const auto& rule : rules) {
        std::wstring ruleTarget = TrimWs(rule.target);
        if (ruleTarget.empty() || ruleTarget.starts_with(L"//")) {
            continue;
        }

        if (ruleTarget == L"TaskButton") {
            for (const auto& style : rule.styles) {
                std::wstring trimmed = TrimWs(style);
                if (trimmed.empty() || trimmed.starts_with(L"//")) {
                    continue;
                }
                size_t eq = trimmed.find(L'=');
                if (eq == std::wstring::npos) {
                    continue;
                }
                std::wstring prop = TrimWs(trimmed.substr(0, eq));
                std::wstring value = TrimWs(trimmed.substr(eq + 1));

                if (prop == L"IconTintColor") {
                    wui::Color color;
                    if (TryParseHexColor(value, &color)) {
                        g_iconTintColor = color;
                    }
                    continue;
                }
                if (prop == L"IconTintOpacity") {
                    try {
                        g_iconTintOpacity = std::clamp(std::stod(value), 0.0, 1.0);
                    } catch (...) {
                    }
                    continue;
                }

                for (auto& [hwnd, button] : g_taskButtonsByHwnd) {
                    ApplySingleStyleToElement(button, style);
                }
            }
            continue;
        }

        size_t start = 0;
        while (start <= ruleTarget.size()) {
            size_t comma = ruleTarget.find(L',', start);
            std::wstring name = TrimWs(ruleTarget.substr(
                start, comma == std::wstring::npos ? std::wstring::npos : comma - start));
            if (!name.empty()) {
                auto it = g_namedElements.find(name);
                bool applied = false;
                if (it != g_namedElements.end()) {
                    for (const auto& style : rule.styles) {
                        ApplySingleStyleToElement(it->second, style);
                    }
                    applied = true;
                }
                auto matches = ResolveGeneralTarget(name);
                for (auto& element : matches) {
                    if (applied && it != g_namedElements.end() && element == it->second) {
                        continue;
                    }
                    for (const auto& style : rule.styles) {
                        ApplySingleStyleToElement(element, style);
                    }
                    applied = true;
                }
                if (!applied) {
                    Wh_Log(L"Unknown control style target: %s", name.c_str());
                }
            }
            if (comma == std::wstring::npos) {
                break;
            }
            start = comma + 1;
        }
    }
}

void ApplyAllControlStyles() {
    if (g_applyingStyles) {
        return;
    }
    g_applyingStyles = true;
    struct Guard {
        ~Guard() { g_applyingStyles = false; }
    } guard;

    // Compared by value at the end rather than "a tint rule was seen", so a
    // rule that re-states the current tint doesn't trigger a pointless icon
    // rebuild on every styling pass.
    const wui::Color previousTintColor = g_iconTintColor;
    const double previousTintOpacity = g_iconTintOpacity;

    // Built-ins always applied.
    ApplyRuleList(BuiltInStyles());
    // Theme styles applied after built-ins, before user custom styles.
    ApplyRuleList(g_themeStyleRules);
    ApplyRuleList(g_controlStyleRules);

    bool tintChanged = previousTintColor.A != g_iconTintColor.A ||
                       previousTintColor.R != g_iconTintColor.R ||
                       previousTintColor.G != g_iconTintColor.G ||
                       previousTintColor.B != g_iconTintColor.B ||
                       previousTintOpacity != g_iconTintOpacity;
    if (tintChanged) {
        g_applyingStyles = false;
        RefreshTaskList(/*forceIconRegeneration=*/true);
    }
}

void ApplyVisibilitySettings() {
    auto setVis = [](PCWSTR name, bool visible) {
        auto it = g_namedElements.find(name);
        if (it != g_namedElements.end()) {
            it->second.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
        }
    };
    setVis(L"StartButton", g_settings.showStartButton);
    setVis(L"SearchButton", g_settings.showSearchButton);
    setVis(L"TaskListPanel", g_settings.centerContent != L"none");
    setVis(L"DisplayButton", g_settings.showDisplayButton);
    setVis(L"SoundButton", g_settings.showSoundButton);
    setVis(L"WifiButton", g_settings.showWifiButton);
    setVis(L"BluetoothButton", g_settings.showBluetoothButton);
    
    setVis(L"BatteryButton", g_settings.showBatteryButton);
    setVis(L"SettingsButton", g_settings.showSettingsButton);
    setVis(L"WeatherButton", g_settings.showWeatherButton);
    setVis(L"ResourceButton", (g_settings.showCpuUsage || g_settings.showRamUsage || g_settings.showGpuUsage));
    setVis(L"ClockButton", g_settings.showClock || g_settings.showDate);
}

// Uses Windows' own native date/time format-picture tokens, so token
// substitution, locale awareness, and pass-through of anything that isn't a
// recognized format letter -- including emoji -- all come for free.
std::wstring FormatClockText() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    std::wstring result;

    if (g_settings.showDate && !g_settings.dateFormat.empty()) {
        wchar_t buf[128]{};
        if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, g_settings.dateFormat.c_str(), buf,
                            ARRAYSIZE(buf), nullptr)) {
            result += buf;
        }
    }
    if (g_settings.showClock && !g_settings.timeFormat.empty()) {
        if (!result.empty()) {
            result += L"  ";
        }
        wchar_t buf[128]{};
        if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, g_settings.timeFormat.c_str(), buf,
                            ARRAYSIZE(buf))) {
            result += buf;
        }
    }
    return result;
}

// ============================================================================
// Vector icon library
// ============================================================================

std::wstring GetEffectiveIconColorString() {
    if (g_iconColorOverride) {
        wchar_t buf[16];
        swprintf_s(buf, L"#%02X%02X%02X",
                   g_iconColorOverride->R,
                   g_iconColorOverride->G,
                   g_iconColorOverride->B);
        return buf;
    }
    return g_settings.iconColor.empty() ? L"#FFFFFF" : g_settings.iconColor;
}

namespace icons {

constexpr PCWSTR kSearchOutline =
    LR"(M57 52 C56.19046875 52.72832031 55.3809375 53.45664062 54.546875 54.20703125 C40.10361222 67.81702887 30.64664849 86.15644954 25 105 C24.6803125 106.06347656 24.360625 107.12695312 24.03125 108.22265625 C16.58663689 138.2423693 23.68076575 168.81926969 38.80615234 195.05957031 C41.71591037 199.78896843 45.29875959 203.88454135 49 208 C49.91652344 209.12921875 49.91652344 209.12921875 50.8515625 210.28125 C68.5131584 231.62547653 96.08358271 243.50615679 123.125 246.6875 C124.74869154 246.81038809 126.37391538 246.9144166 128 247 C128.83917969 247.05285156 129.67835938 247.10570312 130.54296875 247.16015625 C154.16645853 247.92650359 177.39880592 241.06746272 197 228 C201.5066721 227.99008184 203.70652636 230.24535331 206.72486877 233.24940491 C207.31154831 233.85360764 207.89822784 234.45781036 208.50268555 235.08032227 C209.13418518 235.71445038 209.76568481 236.34857849 210.4163208 237.00192261 C212.49913494 239.09770777 214.56764156 241.20706587 216.63671875 243.31640625 C218.08107742 244.77330497 219.52647068 246.22917867 220.97285461 247.68406677 C224.7765582 251.51437308 228.56896585 255.35565517 232.35827637 259.20019531 C238.43205221 265.36023463 244.51935924 271.50683234 250.61310768 277.64710808 C252.74399785 279.79851473 254.86797457 281.95657939 256.99137878 284.1153717 C258.28918977 285.42683165 259.58721759 286.73807707 260.88549805 288.04907227 C261.47825027 288.65529922 262.0710025 289.26152618 262.68171692 289.88612366 C264.08356728 291.29580791 265.53702333 292.65385981 267 294 C267.66 294 268.32 294 269 294 C269.20625 294.53109375 269.4125 295.0621875 269.625 295.609375 C271.57961192 299.00773436 274.22123717 301.51055928 277 304.25 C277.5465625 304.80429687 278.093125 305.35859375 278.65625 305.9296875 C281.32600768 308.99215775 281.32600768 308.99215775 285 310 C285.2475 310.5775 285.495 311.155 285.75 311.75 C287.18299988 314.32939979 288.78424676 316.06121592 291 318 C291.66 318 292.32 318 293 318 C293.2475 318.5775 293.495 319.155 293.75 319.75 C295.18299988 322.32939979 296.78424676 324.06121592 299 326 C299.66 326 300.32 326 301 326 C301.37318359 326.89138672 301.37318359 326.89138672 301.75390625 327.80078125 C303.18065713 330.31884001 304.60427079 331.49732092 306.9375 333.1875 C307.62714844 333.69667969 308.31679688 334.20585937 309.02734375 334.73046875 C313.67685724 337.72272991 318.66572217 337.54609076 324 337 C328.74720408 335.71053453 332.06387778 333.44772735 335 329.5 C337.88172003 324.21848582 338.09197728 318.84438543 337 313 C333.47379409 305.24701748 327.05944819 299.87011477 320.69140625 294.46484375 C317.13837637 291.38773935 314.0090352 288.03609172 311 284.4375 C308.56804854 281.55772117 305.99445392 279.30342609 303 277 C300.13833918 274.28509102 297.52553788 271.46039445 295 268.4375 C292.56804854 265.55772117 289.99445392 263.30342609 287 261 C284.48184682 258.60468356 282.1919593 256.24768942 280.0625 253.5 C277.65085484 250.57679375 274.9327108 248.39404964 272 246 C269.35491029 243.48393905 266.97653101 240.97082234 264.6875 238.125 C262.82989036 235.85762352 260.88289666 233.97572183 258.640625 232.08984375 C253.1355835 227.32085761 247.94619112 222.24091526 242.77026367 217.12036133 C241.62448863 215.9912938 240.47335579 214.86763957 239.31713867 213.74926758 C237.62752384 212.11439446 235.95494105 210.46368647 234.28515625 208.80859375 C233.76856949 208.31467636 233.25198273 207.82075897 232.71974182 207.31187439 C229.46140139 204.03396808 228.10342777 201.64830539 228 197 C228.63330078 195.05639648 228.63330078 195.05639648 229.6953125 193.37109375 C230.07921143 192.73574707 230.46311035 192.10040039 230.85864258 191.44580078 C231.27654053 190.78306152 231.69443848 190.12032227 232.125 189.4375 C246.65329549 164.92538453 250.82932914 134.7979171 243.78320312 107.10742188 C238.87571286 89.22649712 231.0840638 74.13264973 219 60 C218.28457031 59.08541016 218.28457031 59.08541016 217.5546875 58.15234375 C201.51340633 38.03199393 175.29984029 27.00163468 150.71484375 22.3828125 C115.59304879 18.76353184 83.01428234 27.91921162 57 52 Z)";

constexpr PCWSTR kSearchLens =
    LR"(M0 0 C16.93975128 14.36920693 28.26770469 33.59992875 31.51196289 55.73510742 C33.52749766 81.02050274 28.39560109 104.20401079 11.93774414 123.94213867 C11.13723633 124.86381836 10.33672852 125.78549805 9.51196289 126.73510742 C8.54323242 127.8546582 8.54323242 127.8546582 7.55493164 128.99682617 C-6.43622666 143.98945686 -28.05664112 154.22232888 -48.50756836 154.97338867 C-74.68429045 155.47604695 -97.03823145 148.6076491 -116.66381836 130.58666992 C-133.25860681 113.74313732 -142.24229311 91.24241269 -142.86303711 67.73510742 C-142.1704908 42.54373556 -132.21994474 21.51955403 -114.48803711 3.73510742 C-113.85510742 3.07897461 -113.22217773 2.4228418 -112.57006836 1.74682617 C-82.51997673 -27.10922208 -31.27019387 -25.48510426 0 0 Z)";

// 24x24 viewport for everything below.
constexpr PCWSTR kBrightnessStroke =
    L"M8 12 A4 4 0 1 1 16 12 A4 4 0 1 1 8 12 Z "
    L"M12 1.6 L12 4.1 M12 19.9 L12 22.4 M1.6 12 L4.1 12 M19.9 12 L22.4 12 "
    L"M4.9 4.9 L6.7 6.7 M17.3 17.3 L19.1 19.1 M19.1 4.9 L17.3 6.7 M6.7 17.3 L4.9 19.1";

constexpr PCWSTR kSpeakerFill = L"M4 9.2 L7.6 9.2 L12.4 5 L12.4 19 L7.6 14.8 L4 14.8 Z";
constexpr PCWSTR kSpeakerWaves =
    L"M15.4 9.4 A3.6 3.6 0 0 1 15.4 14.6 M18 6.9 A7.2 7.2 0 0 1 18 17.1";
constexpr PCWSTR kSpeakerMuted = L"M15.8 9.8 L20.6 14.6 M20.6 9.8 L15.8 14.6";

constexpr PCWSTR kWifiStroke =
    L"M2.6 8.7 A13.4 13.4 0 0 1 21.4 8.7 M5.8 12.1 A8.9 8.9 0 0 1 18.2 12.1 "
    L"M9 15.4 A4.4 4.4 0 0 1 15 15.4";
constexpr PCWSTR kBluetoothStroke = L"M7.2 7.6 L16.8 16.4 L12 21 L12 3 L16.8 7.6 L7.2 16.4";

constexpr PCWSTR kChevronUp = L"M6.5 14.5 L12 9 L17.5 14.5";
constexpr PCWSTR kChevronDown = L"M6.5 9.5 L12 15 L17.5 9.5";

constexpr PCWSTR kMoonFill =
    L"M12.5 3 C8 3.7 4.5 7.6 4.5 12.3 C4.5 17.5 8.7 21.7 13.9 21.7 "
    L"C17.5 21.7 20.6 19.6 22 16.6 C21 17 19.9 17.2 18.7 17.2 "
    L"C14 17.2 10.2 13.4 10.2 8.7 C10.2 6.6 11 4.6 12.5 3 Z";



constexpr PCWSTR kLockFill = L"M6.6 10.6 L17.4 10.6 L17.4 20.4 L6.6 20.4 Z";
constexpr PCWSTR kLockShackle = L"M9.2 10.6 L9.2 7.7 A2.8 2.8 0 0 1 14.8 7.7 L14.8 10.6";

constexpr PCWSTR kPlayFill = L"M8 5 L18.5 12 L8 19 Z";
constexpr PCWSTR kPauseFill = L"M8 5 L11 5 L11 19 L8 19 Z M13 5 L16 5 L16 19 L13 19 Z";
constexpr PCWSTR kPrevFill = L"M17 5 L17 19 L7.5 12 Z M6 5 L8 5 L8 19 L6 19 Z";
constexpr PCWSTR kNextFill = L"M7 5 L7 19 L16.5 12 Z M16 5 L18 5 L18 19 L16 19 Z";

constexpr PCWSTR kHeadphoneStroke = L"M4.6 15.2 L4.6 12 A7.4 7.4 0 0 1 19.4 12 L19.4 15.2";
constexpr PCWSTR kHeadphoneFill =
    L"M3 14.4 L6.6 14.4 L6.6 20.2 L3 20.2 Z M17.4 14.4 L21 14.4 L21 20.2 L17.4 20.2 Z";

constexpr PCWSTR kAppFill = L"M7 5 L17 5 C18.1 5 19 5.9 19 7 L19 17 C19 18.1 18.1 19 17 19 L7 19 C5.9 19 5 18.1 5 17 L5 7 C5 5.9 5.9 5 7 5 Z M9 9 L9 15 L15 15 L15 9 Z";

constexpr PCWSTR kWeatherSunFill =
    L"M12 6 A6 6 0 1 1 11.99 6 Z";
constexpr PCWSTR kWeatherSunRays =
    L"M12 1.4 L12 3.4 M12 20.6 L12 22.6 M1.4 12 L3.4 12 M20.6 12 L22.6 12 "
    L"M4.5 4.5 L5.9 5.9 M18.1 18.1 L19.5 19.5 "
    L"M19.5 4.5 L18.1 5.9 M5.9 18.1 L4.5 19.5";
constexpr PCWSTR kWeatherCloudFill =
    L"M7 18 A3.6 3.6 0 0 1 7.2 10.8 A5 5 0 0 1 16.6 9.6 "
    L"A3.8 3.8 0 0 1 16.8 18 Z";
constexpr PCWSTR kWeatherSmallCloudFill =
    L"M6 13 A3 3 0 0 1 6.2 6 A4.2 4.2 0 0 1 14.8 5 "
    L"A3.2 3.2 0 0 1 15 13 Z";
constexpr PCWSTR kWeatherRainDrops =
    L"M8 15 L7 19 M12 15 L11 19 M16 15 L15 19";
constexpr PCWSTR kWeatherSnowFlakes =
    L"M7.5 15.5 L9.5 17.5 M9.5 15.5 L7.5 17.5 "
    L"M14.5 15.5 L16.5 17.5 M16.5 15.5 L14.5 17.5";
constexpr PCWSTR kWeatherBolt =
    L"M12.5 14 L10.5 17 L12.5 17 L11 19.5";
constexpr PCWSTR kChevronLeft = L"M14.5 6.5 L9 12 L14.5 17.5";
constexpr PCWSTR kChevronRight = L"M9.5 6.5 L15 12 L9.5 17.5";

constexpr PCWSTR kRecycleBinBody =
    L"M6.5 7 L17.5 7 L16.6 20.5 L7.4 20.5 Z";
constexpr PCWSTR kRecycleBinLid =
    L"M4.5 6 L19.5 6 M9.8 6 L9.8 4 L14.2 4 L14.2 6 "
    L"M10.2 10 L10.2 18 M13.8 10 L13.8 18";
constexpr PCWSTR kCpuChipStroke =
    L"M7 5 L17 5 A2 2 0 0 1 19 7 L19 17 A2 2 0 0 1 17 19 L7 19 "
    L"A2 2 0 0 1 5 17 L5 7 A2 2 0 0 1 7 5 Z "
    L"M9 9 L15 9 L15 15 L9 15 Z "
    L"M8 3 L8 5 M12 3 L12 5 M16 3 L16 5 "
    L"M8 19 L8 21 M12 19 L12 21 M16 19 L16 21 "
    L"M3 8 L5 8 M3 12 L5 12 M3 16 L5 16 "
    L"M19 8 L21 8 M19 12 L21 12 M19 16 L21 16";

constexpr PCWSTR kRamStickStroke =
    L"M3 3.5 L21 3.5 L21 16 L3 16 Z "
    L"M5 6 L7.5 6 L7.5 13.5 L5 13.5 Z "
    L"M9 6 L11.5 6 L11.5 13.5 L9 13.5 Z "
    L"M13 6 L15.5 6 L15.5 13.5 L13 13.5 Z "
    L"M17 6 L19.5 6 L19.5 13.5 L17 13.5 Z "
    L"M4.5 16 L4.5 20.5 M7 16 L7 20.5 M9.5 16 L9.5 20.5 "
    L"M12 16 L12 20.5 M14.5 16 L14.5 20.5 M17 16 L17 20.5 "
    L"M19.5 16 L19.5 20.5";

constexpr PCWSTR kGpuCardStroke =
    L"M2 3 L2 20 "
    L"M3 5 L17 5 L21 8 L21 17 L3 17 Z "
    L"M8 11 A2.5 2.5 0 1 1 13 11 A2.5 2.5 0 1 1 8 11 Z "
    L"M15 11 A2.5 2.5 0 1 1 20 11 A2.5 2.5 0 1 1 15 11 Z";
constexpr PCWSTR kCheckStroke = L"M5 12.5 L10 17.5 L19 6.5";

}  // namespace icons
// Windows 11 Start logo
FrameworkElement BuildWindows11StartIcon(double displaySize) {
    std::wstring brush = GetEffectiveIconColorString();
    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" + std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        L"<Grid Width=\"24\" Height=\"24\">"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"" + brush + L"\" HorizontalAlignment=\"Left\" VerticalAlignment=\"Top\" Margin=\"1,1,0,0\"/>"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"" + brush + L"\" HorizontalAlignment=\"Right\" VerticalAlignment=\"Top\" Margin=\"0,1,1,0\"/>"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"" + brush + L"\" HorizontalAlignment=\"Left\" VerticalAlignment=\"Bottom\" Margin=\"1,0,0,1\"/>"
        L"<Rectangle Width=\"10\" Height=\"10\" RadiusX=\"2\" RadiusY=\"2\" Fill=\"" + brush + L"\" HorizontalAlignment=\"Right\" VerticalAlignment=\"Bottom\" Margin=\"0,0,1,1\"/>"
        L"</Grid></Viewbox>";

    try {
        auto element = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
        element.Name(L"StartIcon");
        return element;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build Windows 11 start icon: %08X",
               static_cast<unsigned int>(ex.code().value));
        return nullptr;
    }
}

FrameworkElement BuildVectorIcon(PCWSTR name,
                                 std::wstring_view fillData,
                                 std::wstring_view strokeData,
                                 double viewport,
                                 double displaySize,
                                 double thickness = 1.7,
                                 PCWSTR brushOverride = nullptr) {
    std::wstring brush = brushOverride ? brushOverride : GetEffectiveIconColorString();

    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" +
        std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        L"<Grid Width=\"" + std::to_wstring(viewport) + L"\" Height=\"" +
        std::to_wstring(viewport) + L"\">";

    if (!fillData.empty()) {
        xaml += L"<Path Data=\"" + EscapeXmlAttr(fillData) + L"\" Fill=\"" + brush + L"\"/>";
    }
    if (!strokeData.empty()) {
        xaml += L"<Path Data=\"" + EscapeXmlAttr(strokeData) + L"\" Stroke=\"" + brush +
                L"\" StrokeThickness=\"" + std::to_wstring(thickness) +
                L"\" StrokeStartLineCap=\"Round\" StrokeEndLineCap=\"Round\" "
                L"StrokeLineJoin=\"Round\"/>";
    }

    xaml += L"</Grid></Viewbox>";

    try {
        auto element = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
        if (name && *name) {
            element.Name(name);
        }
        return element;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build vector icon %s: %08X", name ? name : L"(unnamed)",
               static_cast<unsigned int>(ex.code().value));
        return nullptr;
    }
}

FrameworkElement BuildSearchIcon(double displaySize) {
    std::wstring brush = GetEffectiveIconColorString();

    std::wstring pathsXaml =
        L"<Grid xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\">"
        L"<Path Data=\"" + EscapeXmlAttr(icons::kSearchOutline) + L"\" />"
        L"<Path Data=\"" + EscapeXmlAttr(icons::kSearchLens) + L"\" />"
        L"</Grid>";

    std::wstring viewboxXaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" +
        std::to_wstring(displaySize) + L"\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        L"<Grid Width=\"360\" Height=\"360\">"
        L"<Path Fill=\"" + brush + L"\" />"
        L"</Grid>"
        L"</Viewbox>";

    try {
        auto tempGrid = Markup::XamlReader::Load(pathsXaml).as<wuxc::Grid>();
        auto p1 = tempGrid.Children().GetAt(0).as<winrt::Windows::UI::Xaml::Shapes::Path>();
        auto p2 = tempGrid.Children().GetAt(1).as<winrt::Windows::UI::Xaml::Shapes::Path>();

        // Cleared from the throwaway paths so the geometries can be re-parented.
        auto p1Geom = p1.Data();
        p1.Data(nullptr);
        auto p2Geom = p2.Data();
        p2.Data(nullptr);

        wuxm::GeometryGroup group;
        group.FillRule(wuxm::FillRule::EvenOdd);
        group.Children().Append(p1Geom);

        wuxm::TranslateTransform transform;
        transform.X(189.488037109375);
        transform.Y(66.264892578125);
        p2Geom.Transform(transform);
        group.Children().Append(p2Geom);

        auto viewbox = Markup::XamlReader::Load(viewboxXaml).as<wuxc::Viewbox>();
        auto finalGrid = viewbox.Child().as<wuxc::Grid>();
        auto searchPath =
            finalGrid.Children().GetAt(0).as<winrt::Windows::UI::Xaml::Shapes::Path>();
        searchPath.Data(group);
        viewbox.Name(L"SearchIcon");
        return viewbox;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build search icon: %08X",
               static_cast<unsigned int>(ex.code().value));
        return nullptr;
    }
}

// ============================================================================
// Battery icon builder
// ============================================================================

FrameworkElement BuildBatteryIcon(double displaySize, int percentage, bool charging) {
    percentage = std::clamp(percentage, 0, 100);

    // Indicator colour depends only on the charge level; charging is shown by
    // the bolt to the left, not by a colour change.
    std::wstring indicatorColor;
    std::wstring textColor;
    if (percentage < 20) {
        indicatorColor = L"#FF3B30";   // red
        textColor = L"#000000";
    } else if (percentage < 40) {
        indicatorColor = L"#FF9500";   // orange
        textColor = L"#000000";
    } else if (percentage < 60) {
        indicatorColor = L"#FFCC00";   // yellow
        textColor = L"#000000";
    } else if (percentage < 80) {
        indicatorColor = L"#121212";   // black
        textColor = L"#FFFFFF";
    } else {
        indicatorColor = L"#34C759";   // green
        textColor = L"#000000";
    }

    const double shellWidth = 99.4277;
    const double shellHeight = 55.6796;

    // Indicator width grows with the charge level; a small minimum keeps it
    // visible at 1-2%.
    double indicatorWidth = shellWidth * percentage / 100.0;
    if (indicatorWidth < 6.0) indicatorWidth = 6.0;

    // Outer grid holds the shell + terminal nub. A nested grid the exact width
    // of the shell is used as the text container so the digits are centred on
    // the shell, not on the shell+nub combination (which is where the text
    // used to drift right).
    std::wstring batteryXaml;
    batteryXaml += L"<Grid Width=\"108.809\" Height=\"55.6796\">";

    // Terminal nub, drawn first so the shell overlaps its left edge.
    batteryXaml += L"<Border Width=\"5.404\" Height=\"11.931\" "
                   L"CornerRadius=\"0,3,3,0\" Background=\"#D1D1D1\" "
                   L"HorizontalAlignment=\"Right\" VerticalAlignment=\"Center\"/>";

    // Shell-area grid: everything that should visually sit "inside the shell".
    batteryXaml += L"<Grid Width=\"99.4277\" Height=\"55.6796\" "
                   L"HorizontalAlignment=\"Left\" VerticalAlignment=\"Center\">";
    batteryXaml += L"<Border Width=\"99.4277\" Height=\"55.6796\" "
                   L"CornerRadius=\"15.9085\" Background=\"#D1D1D1\"/>";
    batteryXaml += L"<Border Width=\"" + std::to_wstring(indicatorWidth) + L"\" "
                   L"Height=\"55.6796\" CornerRadius=\"15.9085\" "
                   L"Background=\"" + indicatorColor + L"\" "
                   L"HorizontalAlignment=\"Left\"/>";
    batteryXaml += L"<TextBlock Text=\"" + std::to_wstring(percentage) + L"\" "
                   L"Foreground=\"" + textColor + L"\" FontWeight=\"Bold\" "
                   L"FontSize=\"50\" FontFamily=\"Segoe UI Variable Display, Segoe UI\" "
                   L"HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\" "
                   L"TextAlignment=\"Center\" Margin=\"0\" Padding=\"0\">"
                   L"<TextBlock.RenderTransform>"
                   L"<TranslateTransform Y=\"-2\"/>"
                   L"</TextBlock.RenderTransform>"
                   L"</TextBlock>";
    batteryXaml += L"</Grid>";
    batteryXaml += L"</Grid>";

    std::wstring contentXaml;
    if (charging) {
        contentXaml += L"<StackPanel Orientation=\"Horizontal\" Spacing=\"2\" "
                       L"HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\">";
        contentXaml += L"<Path Data=\"M0 18.3378L14.2119 0L11.9196 13.4478H24.1448L4.43166 32.8553L10.6971 18.3378H0Z\" "
                       L"Fill=\"#FFFFFF\" Width=\"36\" Height=\"47\" Stretch=\"Uniform\" "
                       L"VerticalAlignment=\"Center\"/>";
        contentXaml += batteryXaml;
        contentXaml += L"</StackPanel>";
    } else {
        contentXaml = batteryXaml;
    }

    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Height=\"" + std::to_wstring(displaySize) + L"\">"
        + contentXaml + L"</Viewbox>";

    try {
        auto element = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
        element.Name(L"BatteryIcon");
        return element;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build battery icon: %08X", static_cast<unsigned int>(ex.code().value));
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

// ============================================================================
// Shared WinUI-flavoured building blocks
// ============================================================================
wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush(wui::ColorHelper::FromArgb(a, r, g, b));
}

CornerRadius MakeCorner(double radius) {
    return CornerRadius{radius, radius, radius, radius};
}

// Every clickable surface in the bar and in the flyouts goes through this, so
// corner radius, padding and the transparent-until-hover treatment stay
// identical everywhere.
wuxc::Button MakeGhostButton(PCWSTR name, double cornerRadius) {
    wuxc::Button button;
    if (name && *name) {
        button.Name(name);
    }
    button.Background(MakeBrush(0, 255, 255, 255));
    button.BorderThickness(Thickness{0, 0, 0, 0});
    button.CornerRadius(MakeCorner(cornerRadius));
    button.Padding(Thickness{8, 4, 8, 4});
    button.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    button.VerticalContentAlignment(VerticalAlignment::Center);
    return button;
}

wuxc::TextBlock MakeText(PCWSTR name, std::wstring_view text, double size, bool bold = false,
                         double opacity = 1.0) {
    wuxc::TextBlock block;
    if (name && *name) {
        block.Name(name);
    }
    block.Text(winrt::hstring(text));
    block.FontSize(size);

    // Global font family / weight chosen in the settings window.
    if (!g_settings.fontFamily.empty()) {
        try {
            block.FontFamily(wuxm::FontFamily(winrt::hstring(g_settings.fontFamily)));
        } catch (...) {}
    }
    if (bold || g_settings.fontBold) {
        block.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    }
    block.Opacity(opacity);
    block.VerticalAlignment(VerticalAlignment::Center);
    block.TextTrimming(TextTrimming::CharacterEllipsis);
    return block;
}

wuxc::Border MakeDivider() {
    wuxc::Border border;
    border.Name(L"FlyoutDivider");
    border.Height(1);
    border.Background(MakeBrush(28, 255, 255, 255));
    border.Margin(Thickness{0, 6, 0, 6});
    return border;
}

// ============================================================================
// task icons
//
// ============================================================================

bool ProcessImagePathForWindow(HWND hwnd, std::wstring* imagePath) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (ok) {
        *imagePath = path;
    }
    return ok;
}

using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);

PrivateExtractIconsW_t GetPrivateExtractIcons() {
    static PrivateExtractIconsW_t proc = reinterpret_cast<PrivateExtractIconsW_t>(
        GetProcAddress(GetModuleHandle(L"user32.dll"), "PrivateExtractIconsW"));
    return proc;
}

// Returns an icon the caller owns and must DestroyIcon.
HICON ExtractCrispWindowIcon(HWND hwnd, UINT sizePx) {
    if (sizePx == 0) {
        sizePx = 16;
    }

    std::wstring exePath;
    if (ProcessImagePathForWindow(hwnd, &exePath) && !exePath.empty()) {
        if (auto extract = GetPrivateExtractIcons()) {
            HICON icon = nullptr;
            UINT iconId = 0;
            if (extract(exePath.c_str(), 0, static_cast<int>(sizePx), static_cast<int>(sizePx),
                        &icon, &iconId, 1, 0) == 1 &&
                icon) {
                return icon;
            }
        }
    }

    DWORD_PTR result = 0;
    auto fromMessage = [&](WPARAM which) -> HICON {
        if (SendMessageTimeout(hwnd, WM_GETICON, which, 0, SMTO_ABORTIFHUNG, 100, &result) &&
            result) {
            return CopyIcon(reinterpret_cast<HICON>(result));
        }
        return nullptr;
    };

    if (HICON icon = fromMessage(ICON_BIG)) {
        return icon;
    }
    if (auto classIcon = reinterpret_cast<HICON>(GetClassLongPtr(hwnd, GCLP_HICON))) {
        return CopyIcon(classIcon);
    }
    if (HICON icon = fromMessage(ICON_SMALL2)) {
        return icon;
    }
    if (HICON icon = fromMessage(ICON_SMALL)) {
        return icon;
    }
    if (auto classIcon = reinterpret_cast<HICON>(GetClassLongPtr(hwnd, GCLP_HICONSM))) {
        return CopyIcon(classIcon);
    }
    return nullptr;
}

wuxm::Imaging::BitmapImage HIconToBitmapImage(HICON hIcon, UINT size) {
    if (!hIcon || !size) {
        return nullptr;
    }

    HDC screenDc = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screenDc);
    ReleaseDC(nullptr, screenDc);
    if (!dc) {
        return nullptr;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = static_cast<LONG>(size);
    bmi.bmiHeader.biHeight = -static_cast<LONG>(size);  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) {
        DeleteDC(dc);
        return nullptr;
    }

    DWORD dataSize = static_cast<DWORD>(size) * static_cast<DWORD>(size) * 4;

    HGDIOBJ old = SelectObject(dc, dib);
    ZeroMemory(bits, dataSize);  // fully transparent baseline, no dark fringe
    DrawIconEx(dc, 0, 0, hIcon, static_cast<int>(size), static_cast<int>(size), 0, nullptr,
               DI_NORMAL);

    std::vector<uint8_t> pixels(dataSize);
    memcpy(pixels.data(), bits, dataSize);

    SelectObject(dc, old);
    DeleteObject(dib);
    DeleteDC(dc);

    bool hasAlpha = false;
    for (DWORD i = 3; i < dataSize; i += 4) {
        if (pixels[i] != 0) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        for (DWORD i = 0; i + 3 < dataSize; i += 4) {
            bool black = pixels[i] < 4 && pixels[i + 1] < 4 && pixels[i + 2] < 4;
            pixels[i + 3] = black ? 0 : 255;
        }
    } else {
        for (DWORD i = 0; i + 3 < dataSize; i += 4) {
            uint8_t a = pixels[i + 3];
            if (a > 0 && a < 255) {
                pixels[i + 0] = static_cast<uint8_t>(pixels[i + 0] * a / 255);
                pixels[i + 1] = static_cast<uint8_t>(pixels[i + 1] * a / 255);
                pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * a / 255);
            }
        }
    }

    // Tint priority: a global IconColor override wins over everything, then
    // the per-task IconTint* styles, then no tint.
    double tintOpacity = 0.0;
    wui::Color tintColor = g_iconTintColor;
    if (g_iconColorOverride) {
        tintColor = *g_iconColorOverride;
        tintOpacity = 1.0;
    } else if (g_iconTintOpacity > 0.0) {
        tintOpacity = std::clamp(g_iconTintOpacity, 0.0, 1.0);
    }

    if (tintOpacity > 0.0) {
        double t = tintOpacity;
        double tb = tintColor.B;
        double tg = tintColor.G;
        double tr = tintColor.R;
        for (DWORD i = 0; i + 3 < dataSize; i += 4) {
            // BGRA byte order.
            pixels[i + 0] = static_cast<uint8_t>(pixels[i + 0] * (1.0 - t) + tb * t);
            pixels[i + 1] = static_cast<uint8_t>(pixels[i + 1] * (1.0 - t) + tg * t);
            pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * (1.0 - t) + tr * t);
        }
    }

    BITMAPFILEHEADER fileHeader{};
    fileHeader.bfType = 0x4D42;  // "BM"
    fileHeader.bfSize = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                                           dataSize);
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER infoHeader = bmi.bmiHeader;
    infoHeader.biSizeImage = dataSize;

    try {
        winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
        winrt::Windows::Storage::Streams::DataWriter writer(stream);
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader)));
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&infoHeader), sizeof(infoHeader)));
        writer.WriteBytes(pixels);
        writer.StoreAsync().get();
        writer.DetachStream();
        stream.Seek(0);

        wuxm::Imaging::BitmapImage bitmapImage;
        // Fire-and-forget: blocking on .get() here would risk this UI thread
        // waiting on its own dispatcher to pump the completion.
        bitmapImage.SetSourceAsync(stream);
        return bitmapImage;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Icon conversion failed: %08X", static_cast<unsigned int>(ex.code().value));
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

wuxm::Imaging::BitmapImage GetWindowIconBitmap(HWND hwnd, UINT physicalSize) {
    HICON icon = ExtractCrispWindowIcon(hwnd, physicalSize);
    if (!icon) {
        return nullptr;
    }
    auto bitmap = HIconToBitmapImage(icon, physicalSize);
    DestroyIcon(icon);
    return bitmap;
}

// ============================================================================
// task list
// ============================================================================

// Helper to determine if a window is a Windows shell surface (Start, Search,
// Task View, Action Center, Quick Settings). The owning process name is stable
// across UI languages, unlike the window titles, so match on that.
bool IsStartOrSearchWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok) {
        return false;
    }
    std::wstring exe = path;
    size_t slash = exe.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        exe.erase(0, slash + 1);
    }
    std::wstring lowerExe = ToLowerCopy(exe);
    return lowerExe == L"startmenuexperiencehost.exe" ||
           lowerExe == L"searchhost.exe" ||
           lowerExe == L"searchapp.exe" ||
           lowerExe == L"searchui.exe" ||
           lowerExe == L"shellexperiencehost.exe";
}


bool IsTaskbarEligibleWindow(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }
    if (hwnd == g_topBarHwnd || hwnd == g_islandHwnd) {
        return false;
    }
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return false;
    }
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }
    if (GetWindowTextLength(hwnd) == 0) {
        return false;
    }

    // Skip Start menu / Search overlay windows
    if (IsStartOrSearchWindow(hwnd)) {
        return false;
    }

    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    return true;
}

void ForceForegroundWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    HWND foreground = GetForegroundWindow();
    if (foreground == hwnd) {
        return;
    }

    DWORD foregroundThread = foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThread = GetCurrentThreadId();

    bool attachedForeground =
        foregroundThread && foregroundThread != currentThread &&
        AttachThreadInput(currentThread, foregroundThread, TRUE) != FALSE;
    bool attachedTarget = targetThread && targetThread != currentThread &&
                          targetThread != foregroundThread &&
                          AttachThreadInput(currentThread, targetThread, TRUE) != FALSE;

    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);
    SetActiveWindow(hwnd);

    if (attachedTarget) {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }
    if (attachedForeground) {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }
}


void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                  LONG idChild, DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF || !hwnd) {
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        return;
    }
    g_lastForegroundHwnd = hwnd;

    if (g_settings.centerContent == L"applicationButtons") {
        RunOnUiThread([] {
            try { RefreshApplicationButtons(); } catch (...) {}
        });
    }

    // Check if the foreground window is the desktop
    wchar_t className[256] = {0};
    if (GetClassName(hwnd, className, ARRAYSIZE(className))) {
        if ((wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0)) {
            // When desktop is shown, always clear fullscreen flag
            g_fullScreenAppActive = false;

            // Restore the top bar immediately (minimized, hidden, or cloaked)
            if (g_topBarHwnd) {
                if (IsIconic(g_topBarHwnd)) {
                    ShowWindow(g_topBarHwnd, SW_RESTORE);
                }
                if (!IsWindowVisible(g_topBarHwnd)) {
                    ShowWindow(g_topBarHwnd, SW_SHOWNOACTIVATE);
                }
                BOOL cloaked = FALSE;
                if (SUCCEEDED(DwmGetWindowAttribute(g_topBarHwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) {
                    DwmSetWindowAttribute(g_topBarHwnd, DWMWA_CLOAK, FALSE, sizeof(BOOL));
                }
                // Force topmost so the desktop can't cover the bar
                SetWindowPos(g_topBarHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                // Reposition
                PositionAppBar(g_topBarHwnd, g_barHeightPx);
            }
        }
    }
}

void ActivateOrMinimizeWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    if (hwnd == g_lastForegroundHwnd && !IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_MINIMIZE);
        g_lastForegroundHwnd = nullptr;
        return;
    }
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
    ForceForegroundWindow(hwnd);
    g_lastForegroundHwnd = hwnd;
}

void ToggleMaximizeWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
    ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE);
    ForceForegroundWindow(hwnd);
}

void CloseWindowGracefully(HWND hwnd) {
    if (IsWindow(hwnd)) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
}

FrameworkElement BuildTaskButtonContent(HWND hwnd, const std::wstring& title) {
    wuxc::Grid content;
    content.Name(L"TaskButtonContent");
    content.VerticalAlignment(VerticalAlignment::Center);

    bool wantIcon = g_settings.taskButtonContent != L"textOnly";
    bool wantText = g_settings.taskButtonContent != L"iconOnly";

    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width(GridLength{0, GridUnitType::Auto});
    wuxc::ColumnDefinition textColumn;
    textColumn.Width(GridLength{1, GridUnitType::Star});
    content.ColumnDefinitions().Append(iconColumn);
    content.ColumnDefinitions().Append(textColumn);

    double iconDip = std::max(12, g_settings.taskIconSize);

    if (wantIcon) {
        // Extracted at the exact physical pixel size the image will occupy, so
        // there is no upscale step at all.
        UINT physicalSize =
            static_cast<UINT>(std::lround(iconDip * std::max(1.0, g_dpiScale)));
        if (auto bitmapImage = GetWindowIconBitmap(hwnd, physicalSize)) {
            wuxc::Image img;
            img.Name(L"TaskButtonIcon");
            img.Source(bitmapImage);
            img.Width(iconDip);
            img.Height(iconDip);
            img.Stretch(wuxm::Stretch::Uniform);
            img.VerticalAlignment(VerticalAlignment::Center);
            if (wantText) {
                img.Margin(Thickness{0, 0, 8, 0});
            }
            wuxc::Grid::SetColumn(img, 0);
            content.Children().Append(img);
        }
    }

    if (wantText) {
        auto text = MakeText(L"TaskButtonText", title, 12.5);
        wuxc::Grid::SetColumn(text, 1);
        // No MaxWidth on text; the button's own width will clip it.
        text.ClearValue(FrameworkElement::MaxWidthProperty());
        content.Children().Append(text);
    } else {
        textColumn.Width(GridLength{0, GridUnitType::Pixel});
    }

    return content;
}

// Single click activates immediately, double-click maximizes.
// No delay: Tapped fires instantly, DoubleTapped also fires (after the second tap)
// but we accept that the window will be activated once on the first tap.
void ActivateTaskWindow(HWND hwnd) {
    if (IsWindow(hwnd)) {
        ActivateOrMinimizeWindow(hwnd);
    }
}

wuxc::Button CreateTaskButton(HWND hwnd, const std::wstring& title) {
    auto button = MakeGhostButton(L"TaskButton", g_settings.cornerRadius);
    button.Content(BuildTaskButtonContent(hwnd, title));
    button.MaxWidth(g_settings.taskButtonWidth);  // max width from settings
    button.ClearValue(FrameworkElement::WidthProperty()); // auto width by default
    // Stretch, not Center: the Start and Search buttons fill the bar height and
    // let their Margin do the insetting, so a centred task button ended up
    // shorter than them with dead space above and below. Stretching makes the
    // same "Margin=3,2,3,2" mean the same thing here as it does there, at any
    // bar height, without hard-coding a Height.
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.Margin(Thickness{3, 2, 3, 2});
    button.Padding(Thickness{8, 0, 8, 0});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    button.Tag(winrt::box_value(reinterpret_cast<int64_t>(hwnd)));

    // The first Tapped of a double-tap would minimize the window before
    // DoubleTapped fires, and the trailing Tapped raised by WinUI after
    // DoubleTapped would do the same again. Both are handled here: the first
    // Tapped defers the single-tap action so it can be cancelled, and the
    // trailing one is discarded via the timestamp set in DoubleTapped.
    // The event is marked handled so it does not bubble up to the bar root,
    // whose own DoubleTapped handler toggles the last foreground window.
    button.Tapped([](wf::IInspectable const& sender, Input::TappedRoutedEventArgs const& args) {
        try {
            args.Handled(true);
            if (g_lastDoubleTapTick != 0 &&
                GetTickCount64() - g_lastDoubleTapTick < 400) {
                return;
            }
            auto btn = sender.as<wuxc::Button>();
            HWND hwnd = reinterpret_cast<HWND>(winrt::unbox_value<int64_t>(btn.Tag()));
            if (!IsWindow(hwnd)) {
                return;
            }
            // Defer the single-tap action just long enough for a second tap to
            // arrive and cancel it. GetDoubleClickTime() matches the interval
            // the framework itself uses to decide between single and double tap.
            g_taskClickPendingHwnd = hwnd;
            if (!g_taskClickTimer) {
                g_taskClickTimer = DispatcherTimer();
                g_taskClickTimer.Interval(
                    std::chrono::milliseconds(GetDoubleClickTime()));
                g_taskClickTimer.Tick(
                    [](wf::IInspectable const&, wf::IInspectable const&) {
                        g_taskClickTimer.Stop();
                        HWND pending = g_taskClickPendingHwnd;
                        g_taskClickPendingHwnd = nullptr;
                        if (pending) {
                            ActivateTaskWindow(pending);
                        }
                    });
            }
            g_taskClickTimer.Stop();
            g_taskClickTimer.Start();
        } catch (...) {
        }
    });

    button.DoubleTapped(
        [](wf::IInspectable const& sender, Input::DoubleTappedRoutedEventArgs const& args) {
            try {
                // Mark handled so the event does not bubble to the bar root's
                // DoubleTapped, which toggles the last foreground window and
                // would otherwise affect a second window (or the same one
                // twice, flipping it back).
                args.Handled(true);
                // Cancel the deferred single-tap action before it runs the
                // minimize animation, and stamp the time so any trailing Tapped
                // raised by the framework for this same gesture is discarded.
                g_lastDoubleTapTick = GetTickCount64();
                if (g_taskClickTimer) {
                    g_taskClickTimer.Stop();
                }
                g_taskClickPendingHwnd = nullptr;
                auto btn = sender.as<wuxc::Button>();
                HWND hwnd = reinterpret_cast<HWND>(winrt::unbox_value<int64_t>(btn.Tag()));
                if (!IsWindow(hwnd)) {
                    return;
                }
                ToggleMaximizeWindow(hwnd);
            } catch (...) {
            }
        });

    button.RightTapped(
        [](wf::IInspectable const& sender, Input::RightTappedRoutedEventArgs const&) {
            try {
                if (!g_taskContextMenu) {
                    return;
                }
                auto btn = sender.as<wuxc::Button>();
                auto tagValue = winrt::unbox_value<int64_t>(btn.Tag());
                g_contextMenuTargetHwnd = reinterpret_cast<HWND>(tagValue);
                if (g_taskMenuToggleItem) {
                    g_taskMenuToggleItem.Text(
                        IsZoomed(g_contextMenuTargetHwnd) ? L"Restore" : L"Maximize");
                }
                g_taskContextMenu.ShowAt(btn);
            } catch (...) {
            }
        });

    return button;
}

void UpdateTaskButtonState(wuxc::Button button, HWND hwnd, const std::wstring& title) {
    button.Content(BuildTaskButtonContent(hwnd, title));
}
DWORD GetRealAppPid(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return 0;

    wchar_t exePath[MAX_PATH]{};
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return pid;
    DWORD size = ARRAYSIZE(exePath);
    QueryFullProcessImageNameW(process, 0, exePath, &size);
    CloseHandle(process);

    std::wstring exeName = exePath;
    size_t slash = exeName.find_last_of(L"\\/");
    if (slash != std::wstring::npos) exeName.erase(0, slash + 1);

    if (_wcsicmp(exeName.c_str(), L"ApplicationFrameHost.exe") == 0) {
        struct ChildSearch { DWORD pid = 0; };
        ChildSearch search;
        EnumChildWindows(hwnd, [](HWND child, LPARAM lp) -> BOOL {
            auto* s = reinterpret_cast<ChildSearch*>(lp);
            wchar_t cls[128]{};
            GetClassName(child, cls, ARRAYSIZE(cls));
            if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                GetWindowThreadProcessId(child, &s->pid);
                return FALSE;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&search));
        if (search.pid) return search.pid;
    }
    return pid;
}

bool FileDescriptionMatchesExe(const std::wstring& desc, const std::wstring& exeBase) {
    std::wstring d = ToLowerCopy(desc);
    std::wstring e = ToLowerCopy(exeBase);
    if (d.empty() || e.empty()) return false;
    if (d.find(e) != std::wstring::npos) return true;
    if (e.find(d) != std::wstring::npos) return true;
    size_t maxLen = (std::min)(d.size(), e.size());
    for (size_t len = maxLen; len >= 4; len--) {
        for (size_t i = 0; i + len <= e.size(); i++) {
            if (d.find(e.substr(i, len)) != std::wstring::npos) return true;
        }
    }
    return false;
}

std::wstring ResolveShortcutTarget(const std::wstring& lnkPath) {
    winrt::com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(link.put())))) {
        return L"";
    }
    winrt::com_ptr<IPersistFile> file;
    if (FAILED(link->QueryInterface(IID_PPV_ARGS(file.put())))) return L"";
    if (FAILED(file->Load(lnkPath.c_str(), STGM_READ))) return L"";
    wchar_t target[MAX_PATH]{};
    if (FAILED(link->GetPath(target, ARRAYSIZE(target), nullptr, SLGP_UNCPRIORITY))) {
        return L"";
    }
    return target;
}

std::wstring FindStartMenuShortcutName(const std::wstring& exePath) {
    static std::mutex s_cacheMutex;
    static std::map<std::wstring, std::wstring> s_cache;

    std::wstring key = ToLowerCopy(exePath);
    {
        std::lock_guard<std::mutex> lock(s_cacheMutex);
        auto cached = s_cache.find(key);
        if (cached != s_cache.end()) return cached->second;
    }

    std::wstring exeBase = exePath;
    size_t slash = exeBase.find_last_of(L"\\/");
    if (slash != std::wstring::npos) exeBase.erase(0, slash + 1);
    size_t dot = exeBase.find_last_of(L'.');
    if (dot != std::wstring::npos) exeBase.resize(dot);
    std::wstring exeBaseLower = ToLowerCopy(exeBase);

    static const GUID kProgramsFolder =
        {0xA77F5D77, 0x2E2B, 0x44C3, {0xA6, 0xA2, 0xAB, 0xA6, 0x01, 0x05, 0x4A, 0x51}};
    static const GUID kCommonProgramsFolder =
        {0x0139D44E, 0x6AFE, 0x49F2, {0x86, 0x90, 0x3D, 0xAF, 0xCA, 0xE6, 0xFF, 0xB8}};

    std::wstring exactMatch;
    std::wstring fuzzyMatch;

    const GUID* folders[] = { &kProgramsFolder, &kCommonProgramsFolder };
    for (const GUID* id : folders) {
        PWSTR folder = nullptr;
        if (FAILED(SHGetKnownFolderPath(*id, 0, nullptr, &folder))) continue;

        std::vector<std::wstring> stack;
        stack.push_back(folder);
        CoTaskMemFree(folder);

        while (!stack.empty()) {
            std::wstring dir = stack.back();
            stack.pop_back();

            WIN32_FIND_DATAW fd{};
            HANDLE find = FindFirstFileW((dir + L"\\*").c_str(), &fd);
            if (find == INVALID_HANDLE_VALUE) continue;
            do {
                if (wcscmp(fd.cFileName, L".") == 0 ||
                    wcscmp(fd.cFileName, L"..") == 0) continue;
                std::wstring full = dir + L"\\" + fd.cFileName;
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    stack.push_back(full);
                    continue;
                }
                std::wstring name = fd.cFileName;
                if (name.size() <= 4) continue;
                if (_wcsicmp(name.c_str() + name.size() - 4, L".lnk") != 0) continue;

                std::wstring shortName = name.substr(0, name.size() - 4);

                if (exactMatch.empty()) {
                    std::wstring target = ResolveShortcutTarget(full);
                    if (!target.empty() &&
                        _wcsicmp(target.c_str(), exePath.c_str()) == 0) {
                        exactMatch = shortName;
                    }
                }
                if (fuzzyMatch.empty() &&
                    ToLowerCopy(shortName) == exeBaseLower) {
                    fuzzyMatch = shortName;
                }
            } while (FindNextFileW(find, &fd));
            FindClose(find);
        }
    }

    std::wstring result = !exactMatch.empty() ? exactMatch : fuzzyMatch;
    if (!result.empty()) {
        std::lock_guard<std::mutex> lock(s_cacheMutex);
        s_cache[key] = result;
    }
    return result;
}

std::wstring GetApplicationDisplayName(HWND hwnd) {
    wchar_t titleBuf[512]{};
    GetWindowText(hwnd, titleBuf, ARRAYSIZE(titleBuf));
    std::wstring title = TrimWs(titleBuf);

    if (!title.empty()) {
        // A bare file path with no app-name suffix: fall back to the
        // executable's own name, which is what a taskbar tooltip shows.
        if (title.find(L'\\') != std::wstring::npos ||
            title.find(L'/') != std::wstring::npos) {
            bool hasSeparator = false;
            for (const wchar_t* sep : { L" - ", L" — ", L" – ", L" | " }) {
                if (title.rfind(sep) != std::wstring::npos) {
                    hasSeparator = true;
                    break;
                }
            }
            if (!hasSeparator) {
                size_t slash = title.find_last_of(L"\\/");
                if (slash != std::wstring::npos && slash + 1 < title.size()) {
                    title = title.substr(slash + 1);
                }
            }
        }

        static const wchar_t* seps[] = { L" - ", L" — ", L" – ", L" | " };
        size_t bestPos = std::wstring::npos;
        std::wstring bestSuffix;
        for (const wchar_t* sep : seps) {
            size_t sepLen = wcslen(sep);
            size_t pos = title.rfind(sep);
            if (pos != std::wstring::npos && pos > 0 &&
                pos + sepLen < title.size()) {
                if (bestPos == std::wstring::npos || pos > bestPos) {
                    bestPos = pos;
                    bestSuffix = TrimWs(title.substr(pos + sepLen));
                }
            }
        }
        if (!bestSuffix.empty()) return bestSuffix;
        return title;
    }

    DWORD pid = GetRealAppPid(hwnd);
    if (!pid) return L"Application";

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return L"Application";

    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok) return L"Application";

    std::wstring exeBase = path;
    size_t slash = exeBase.find_last_of(L"\\/");
    if (slash != std::wstring::npos) exeBase.erase(0, slash + 1);
    size_t dot = exeBase.find_last_of(L'.');
    if (dot != std::wstring::npos) exeBase.resize(dot);

    static HMODULE versionDll =
        LoadLibraryEx(L"version.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (versionDll) {
        using GetFileVersionInfoSizeW_t = DWORD(WINAPI*)(LPCWSTR, LPDWORD);
        using GetFileVersionInfoW_t = BOOL(WINAPI*)(LPCWSTR, DWORD, DWORD, LPVOID);
        using VerQueryValueW_t = BOOL(WINAPI*)(LPCVOID, LPCWSTR, LPVOID*, PUINT);
        auto pSize = (GetFileVersionInfoSizeW_t)GetProcAddress(versionDll, "GetFileVersionInfoSizeW");
        auto pInfo = (GetFileVersionInfoW_t)GetProcAddress(versionDll, "GetFileVersionInfoW");
        auto pQuery = (VerQueryValueW_t)GetProcAddress(versionDll, "VerQueryValueW");
        if (pSize && pInfo && pQuery) {
            DWORD dummy = 0;
            DWORD verSize = pSize(path, &dummy);
            if (verSize > 0) {
                std::vector<BYTE> buffer(verSize);
                if (pInfo(path, 0, verSize, buffer.data())) {
                    struct LANGANDCODEPAGE { WORD wLanguage; WORD wCodePage; } *lpTranslate;
                    UINT cbTranslate = 0;
                    if (pQuery(buffer.data(), L"\\VarFileInfo\\Translation",
                               (LPVOID*)&lpTranslate, &cbTranslate) &&
                        cbTranslate >= sizeof(LANGANDCODEPAGE)) {
                        wchar_t subBlock[64];
                        swprintf_s(subBlock,
                                   L"\\StringFileInfo\\%04x%04x\\FileDescription",
                                   lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
                        LPWSTR desc = nullptr;
                        UINT descLen = 0;
                        if (pQuery(buffer.data(), subBlock, (LPVOID*)&desc, &descLen) &&
                            descLen > 0 && desc) {
                            std::wstring result = TrimWs(desc);
                            if (!result.empty() &&
                                FileDescriptionMatchesExe(result, exeBase)) {
                                return result;
                            }
                        }
                    }
                }
            }
        }
    }

    if (exeBase.empty()) return L"Application";
    exeBase[0] = static_cast<wchar_t>(towupper(exeBase[0]));
    return exeBase;
}

// Last (target HWND, display title) actually rendered into the application-
// name panel. File-scope so BuildTopBarContent() can reset it whenever the
// panel is rebuilt — otherwise the early-exit below skips population and the
// fresh, empty panel stays blank.
static HWND         g_lastAppTitleTarget = nullptr;
static std::wstring g_lastAppTitleText;

void RefreshApplicationButtons() {
    if (!g_taskListPanel) {
        return;
    }

    HWND target = g_lastForegroundHwnd;
    if (!target || !IsWindow(target)) {
        HWND fg = GetForegroundWindow();
        if (fg) {
            DWORD pid = 0;
            GetWindowThreadProcessId(fg, &pid);
            target = (pid && pid != GetCurrentProcessId()) ? fg : nullptr;
        } else {
            target = nullptr;
        }
    }

    if (!target) {
        if (g_lastAppTitleTarget != nullptr) {
            g_taskListPanel.Children().Clear();
            g_lastAppTitleTarget = nullptr;
            g_lastAppTitleText.clear();
        }
        return;
    }

    std::wstring title = GetApplicationDisplayName(target);

    if (target == g_lastAppTitleTarget && title == g_lastAppTitleText) {
        return;
    }
    g_lastAppTitleTarget = target;
    g_lastAppTitleText = title;

    g_taskListPanel.Children().Clear();

    auto titleBtn = MakeGhostButton(L"AppTitleButton", g_settings.cornerRadius);
    titleBtn.VerticalAlignment(VerticalAlignment::Stretch);
    titleBtn.Margin(Thickness{3, 4, 3, 4});
    titleBtn.Padding(Thickness{10, 0, 10, 0});
    titleBtn.HorizontalContentAlignment(HorizontalAlignment::Center);
    titleBtn.MaxWidth(360.0);

    // Clip the content to the button's own bounds. XAML Buttons don't clip
    // their content by default, so without this a long app name spills past
    // MaxWidth and overlaps whatever is rendered to the right of it.
    auto clipGeometry = wuxm::RectangleGeometry();
    titleBtn.Clip(clipGeometry);
    titleBtn.SizeChanged([clipGeometry](auto&& sender, auto&&) {
        try {
            auto btn = sender.template as<wuxc::Button>();
            clipGeometry.Rect(winrt::Windows::Foundation::Rect{
                0.0f, 0.0f,
                static_cast<float>(btn.ActualWidth()),
                static_cast<float>(btn.ActualHeight())});
        } catch (...) {}
    });

    auto titleText = MakeText(L"AppTitleText", title, 12.5, true);
    titleText.MaxWidth(340.0);
    titleBtn.Content(titleText);
    HWND capturedTarget = target;
    titleBtn.Tag(winrt::box_value(reinterpret_cast<int64_t>(capturedTarget)));
    titleBtn.Click([capturedTarget](auto&&, auto&&) {
        if (IsWindow(capturedTarget)) {
            ForceForegroundWindow(capturedTarget);
        }
    });
    titleBtn.RightTapped([capturedTarget](wf::IInspectable const& sender,
                                          Input::RightTappedRoutedEventArgs const& args) {
        try {
            args.Handled(true);
            g_appTitleContextTarget = capturedTarget;
            if (g_appTitleContextMenu) {
                g_appTitleContextMenu.ShowAt(sender.as<FrameworkElement>());
            }
        } catch (...) {
        }
    });
    g_taskListPanel.Children().Append(titleBtn);

    ApplyAllControlStyles();
}

void RefreshTaskList(bool forceIconRegeneration) {
    if (!g_taskListPanel) {
        return;
    }

    if (g_settings.centerContent == L"applicationButtons") {
        RefreshApplicationButtons();
        return;
    }
    if (g_settings.centerContent == L"none") {
        if (g_taskListPanel.Children().Size() > 0) {
            g_taskListPanel.Children().Clear();
            g_taskButtonsByHwnd.clear();
            g_taskButtonLastTitle.clear();
            g_stableWindowOrder.clear();
        }
        return;
    }

    std::vector<HWND> currentWindows;
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto* list = reinterpret_cast<std::vector<HWND>*>(lParam);
            if (IsTaskbarEligibleWindow(hwnd)) {
                list->push_back(hwnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&currentWindows));

    std::set<HWND> currentSet(currentWindows.begin(), currentWindows.end());

    bool treeChanged = forceIconRegeneration;

    for (auto it = g_stableWindowOrder.begin(); it != g_stableWindowOrder.end();) {
        if (!currentSet.count(*it)) {
            auto btnIt = g_taskButtonsByHwnd.find(*it);
            if (btnIt != g_taskButtonsByHwnd.end()) {
                uint32_t index;
                if (g_taskListPanel.Children().IndexOf(btnIt->second, index)) {
                    g_taskListPanel.Children().RemoveAt(index);
                }
                g_taskButtonsByHwnd.erase(btnIt);
            }
            g_taskButtonLastTitle.erase(*it);
            it = g_stableWindowOrder.erase(it);
            treeChanged = true;
        } else {
            ++it;
        }
    }

    for (HWND hwnd : currentWindows) {
        wchar_t titleBuf[256]{};
        GetWindowText(hwnd, titleBuf, ARRAYSIZE(titleBuf));
        std::wstring title = titleBuf;

        auto btnIt = g_taskButtonsByHwnd.find(hwnd);
        if (btnIt == g_taskButtonsByHwnd.end()) {
            auto button = CreateTaskButton(hwnd, title);
            g_taskListPanel.Children().Append(button);
            g_taskButtonsByHwnd.insert_or_assign(hwnd, button);
            g_taskButtonLastTitle.insert_or_assign(hwnd, title);
            g_stableWindowOrder.push_back(hwnd);
            treeChanged = true;
            continue;
        }

        auto lastTitleIt = g_taskButtonLastTitle.find(hwnd);
        bool titleChanged =
            lastTitleIt == g_taskButtonLastTitle.end() || lastTitleIt->second != title;
        if (forceIconRegeneration || titleChanged) {
            UpdateTaskButtonState(btnIt->second, hwnd, title);
            g_taskButtonLastTitle.insert_or_assign(hwnd, title);
        }
    }

    // NEW: Adjust button widths to fit the available space
    AdjustTaskButtonWidths();

    if (treeChanged) {
        ApplyAllControlStyles();
    }
}
// Diffing refresh: keeps the same Button object at the same panel position for
// every window that's still open, only appending new windows at the end and
// removing closed ones. EnumWindows returns Z-order, which changes every time
// the user clicks between windows -- rebuilding from scratch each tick made the
// whole list visibly reorder itself on every refresh.

// ============================================================================
// Audio subsystem
// ============================================================================

// Declared by hand rather than pulled from functiondiscoverykeys_devpkey.h,
// which isn't reliably present in this toolchain's headers.
static const PROPERTYKEY kPkeyDeviceFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};

// IPolicyConfig is how every third-party audio switcher changes the default
// endpoint -- there is no public API for it. Declared by hand because it ships
// in no SDK header. Only SetDefaultEndpoint is actually called; the earlier
// vtable slots must still be declared so the layout matches, and their unused
// parameters are typed as void* to avoid dragging in mmreg.h.
static const CLSID kCLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const IID kIID_IPolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};

struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, INT64*, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                       PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId, ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

namespace audio {

struct SessionInfo {
    std::wstring name;
    int volume = 100;
    bool isSystemSounds = false;
    winrt::com_ptr<ISimpleAudioVolume> control;
};

struct OutputDevice {
    std::wstring id;
    std::wstring name;
    bool isDefault = false;
};

winrt::com_ptr<IMMDeviceEnumerator> DeviceEnumerator() {
    winrt::com_ptr<IMMDeviceEnumerator> enumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), enumerator.put_void()))) {
        return nullptr;
    }
    return enumerator;
}

winrt::com_ptr<IMMDevice> DefaultRenderDevice() {
    auto enumerator = DeviceEnumerator();
    if (!enumerator) {
        return nullptr;
    }
    winrt::com_ptr<IMMDevice> device;
    if (FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, device.put()))) {
        return nullptr;
    }
    return device;
}

std::wstring DeviceFriendlyName(IMMDevice* device) {
    if (!device) {
        return L"";
    }
    winrt::com_ptr<IPropertyStore> store;
    if (FAILED(device->OpenPropertyStore(STGM_READ, store.put()))) {
        return L"";
    }
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring name;
    if (SUCCEEDED(store->GetValue(kPkeyDeviceFriendlyName, &value)) && value.vt == VT_LPWSTR &&
        value.pwszVal) {
        name = value.pwszVal;
    }
    PropVariantClear(&value);
    return name;
}

std::wstring DeviceId(IMMDevice* device) {
    if (!device) {
        return L"";
    }
    LPWSTR id = nullptr;
    if (FAILED(device->GetId(&id)) || !id) {
        return L"";
    }
    std::wstring result = id;
    CoTaskMemFree(id);
    return result;
}

// Cached: the wheel handler hits this on every notch, and re-running
// CoCreateInstance + Activate each time is enough latency to make scrolling
// feel sticky. Dropped whenever a call fails, which covers the default endpoint
// changing underneath us.
[[clang::no_destroy]] winrt::com_ptr<IAudioEndpointVolume> g_cachedEndpointVolume;
std::wstring g_cachedEndpointDeviceId;
std::mutex g_endpointMutex;

void InvalidateEndpointCache() {
    std::lock_guard<std::mutex> lock(g_endpointMutex);
    g_cachedEndpointVolume = nullptr;
    g_cachedEndpointDeviceId.clear();
}

winrt::com_ptr<IAudioEndpointVolume> EndpointVolume() {
    auto device = DefaultRenderDevice();
    if (!device) {
        return nullptr;
    }
    std::wstring deviceId = DeviceId(device.get());

    std::lock_guard<std::mutex> lock(g_endpointMutex);
    if (g_cachedEndpointVolume && g_cachedEndpointDeviceId == deviceId) {
        return g_cachedEndpointVolume;
    }

    winrt::com_ptr<IAudioEndpointVolume> volume;
    if (FAILED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                                volume.put_void()))) {
        return nullptr;
    }
    g_cachedEndpointVolume = volume;
    g_cachedEndpointDeviceId = deviceId;
    return volume;
}

int GetMasterVolume() {
    auto volume = EndpointVolume();
    if (!volume) {
        return 0;
    }
    float level = 0.0f;
    if (FAILED(volume->GetMasterVolumeLevelScalar(&level))) {
        return 0;
    }
    return std::clamp(static_cast<int>(level * 100.0f + 0.5f), 0, 100);
}

void SetMasterVolume(int percent) {
    auto volume = EndpointVolume();
    if (!volume) {
        return;
    }
    float level = std::clamp(percent, 0, 100) / 100.0f;
    volume->SetMasterVolumeLevelScalar(level, nullptr);
    if (percent > 0) {
        volume->SetMute(FALSE, nullptr);
    }
}

bool GetMasterMute() {
    auto volume = EndpointVolume();
    if (!volume) {
        return false;
    }
    BOOL muted = FALSE;
    volume->GetMute(&muted);
    return muted != FALSE;
}

std::vector<OutputDevice> EnumerateOutputDevices() {
    std::vector<OutputDevice> devices;
    auto enumerator = DeviceEnumerator();
    if (!enumerator) {
        return devices;
    }

    std::wstring defaultId;
    {
        winrt::com_ptr<IMMDevice> defaultDevice;
        if (SUCCEEDED(
                enumerator->GetDefaultAudioEndpoint(eRender, eConsole, defaultDevice.put()))) {
            defaultId = DeviceId(defaultDevice.get());
        }
    }

    winrt::com_ptr<IMMDeviceCollection> collection;
    if (FAILED(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, collection.put()))) {
        return devices;
    }

    UINT count = 0;
    collection->GetCount(&count);
    for (UINT i = 0; i < count; i++) {
        winrt::com_ptr<IMMDevice> device;
        if (FAILED(collection->Item(i, device.put())) || !device) {
            continue;
        }
        OutputDevice info;
        info.id = DeviceId(device.get());
        info.name = DeviceFriendlyName(device.get());
        if (info.id.empty() || info.name.empty()) {
            continue;
        }
        info.isDefault = !defaultId.empty() && info.id == defaultId;
        devices.push_back(std::move(info));
    }

    std::sort(devices.begin(), devices.end(),
              [](const OutputDevice& a, const OutputDevice& b) {
                  if (a.isDefault != b.isDefault) {
                      return a.isDefault;
                  }
                  return ToLowerCopy(a.name) < ToLowerCopy(b.name);
              });
    return devices;
}

bool SetDefaultOutputDevice(const std::wstring& deviceId) {
    if (deviceId.empty()) {
        return false;
    }
    winrt::com_ptr<IPolicyConfig> policy;
    if (FAILED(CoCreateInstance(kCLSID_PolicyConfigClient, nullptr, CLSCTX_ALL,
                                kIID_IPolicyConfig, policy.put_void())) ||
        !policy) {
        Wh_Log(L"IPolicyConfig unavailable; cannot switch the default output device");
        return false;
    }
    // All three roles, otherwise communication apps keep the old endpoint.
    bool ok = SUCCEEDED(policy->SetDefaultEndpoint(deviceId.c_str(), eConsole));
    policy->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
    policy->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
    InvalidateEndpointCache();
    return ok;
}

std::wstring ProcessDisplayName(DWORD pid) {
    if (!pid) {
        return L"";
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return L"";
    }
    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok) {
        return L"";
    }
    std::wstring name = path;
    size_t slash = name.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        name.erase(0, slash + 1);
    }
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        name.resize(dot);
    }
    return name;
}

std::vector<SessionInfo> EnumerateSessions() {
    std::vector<SessionInfo> sessions;

    auto device = DefaultRenderDevice();
    if (!device) {
        return sessions;
    }

    winrt::com_ptr<IAudioSessionManager2> manager;
    if (FAILED(device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
                                manager.put_void()))) {
        return sessions;
    }

    winrt::com_ptr<IAudioSessionEnumerator> enumerator;
    if (FAILED(manager->GetSessionEnumerator(enumerator.put()))) {
        return sessions;
    }

    int count = 0;
    if (FAILED(enumerator->GetCount(&count))) {
        return sessions;
    }

    for (int i = 0; i < count; i++) {
        winrt::com_ptr<IAudioSessionControl> control;
        if (FAILED(enumerator->GetSession(i, control.put()))) {
            continue;
        }
        auto control2 = control.try_as<IAudioSessionControl2>();
        if (!control2) {
            continue;
        }

        AudioSessionState state = AudioSessionStateExpired;
        if (SUCCEEDED(control->GetState(&state)) && state == AudioSessionStateExpired) {
            continue;
        }

        SessionInfo info;
        info.isSystemSounds = control2->IsSystemSoundsSession() == S_OK;

        LPWSTR displayName = nullptr;
        if (SUCCEEDED(control->GetDisplayName(&displayName)) && displayName && *displayName) {
            info.name = displayName;
        }
        if (displayName) {
            CoTaskMemFree(displayName);
        }

        if (info.isSystemSounds) {
            info.name = L"System Sounds";
        } else if (info.name.empty()) {
            DWORD pid = 0;
            control2->GetProcessId(&pid);
            info.name = ProcessDisplayName(pid);
        }
        if (info.name.empty()) {
            continue;
        }

        auto simpleVolume = control.try_as<ISimpleAudioVolume>();
        if (!simpleVolume) {
            continue;
        }
        float level = 1.0f;
        simpleVolume->GetMasterVolume(&level);
        info.volume = std::clamp(static_cast<int>(level * 100.0f + 0.5f), 0, 100);
        info.control = simpleVolume;

        sessions.push_back(std::move(info));
    }

    // Stable ordering: system sounds first, then alphabetical, so the list
    // doesn't shuffle every time the flyout is opened.
    std::sort(sessions.begin(), sessions.end(),
              [](const SessionInfo& a, const SessionInfo& b) {
                  if (a.isSystemSounds != b.isSystemSounds) {
                      return a.isSystemSounds;
                  }
                  return ToLowerCopy(a.name) < ToLowerCopy(b.name);
              });

    return sessions;
}

}  // namespace audio

// ============================================================================
// Brightness subsystem
//
// Internal laptop panels answer to WMI (root\WMI, WmiSetBrightness); external
// monitors generally don't, but most answer DDC/CI through dxva2's
// SetMonitorBrightness. Both are tried, cheapest-first, and which one worked is
// remembered so the slider doesn't re-probe on every drag.
// ============================================================================

namespace brightness {

static const CLSID kCLSID_WbemLocator = {
    0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
static const IID kIID_IWbemLocator = {
    0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};

enum class Backend { Unknown, Wmi, Ddc, None };
Backend g_backend = Backend::Unknown;
std::mutex g_brightnessMutex;

[[clang::no_destroy]] winrt::com_ptr<IWbemServices> g_cachedWmiServices;

winrt::com_ptr<IWbemServices> ConnectWmi() {
    winrt::com_ptr<IWbemLocator> locator;
    if (FAILED(CoCreateInstance(kCLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                                kIID_IWbemLocator, locator.put_void()))) {
        return nullptr;
    }

    BSTR nameSpace = SysAllocString(L"root\\WMI");
    winrt::com_ptr<IWbemServices> services;
    HRESULT hr = locator->ConnectServer(nameSpace, nullptr, nullptr, nullptr, 0, nullptr,
                                        nullptr, services.put());
    SysFreeString(nameSpace);
    if (FAILED(hr)) {
        return nullptr;
    }

    CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    return services;
}

bool WmiGet(int* outPercent) {
    auto services = ConnectWmi();
    if (!services) {
        return false;
    }

    BSTR language = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM WmiMonitorBrightness");
    winrt::com_ptr<IEnumWbemClassObject> enumerator;
    HRESULT hr = services->ExecQuery(language, query,
                                     WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                     nullptr, enumerator.put());
    SysFreeString(query);
    SysFreeString(language);
    if (FAILED(hr) || !enumerator) {
        return false;
    }

    IWbemClassObject* raw = nullptr;
    ULONG returned = 0;
    if (FAILED(enumerator->Next(2000, 1, &raw, &returned)) || returned == 0 || !raw) {
        return false;
    }
    winrt::com_ptr<IWbemClassObject> object;
    object.attach(raw);

    VARIANT value;
    VariantInit(&value);
    bool ok = false;
    if (SUCCEEDED(object->Get(L"CurrentBrightness", 0, &value, nullptr, nullptr))) {
        if (value.vt == VT_UI1) {
            *outPercent = value.bVal;
            ok = true;
        } else if (value.vt == VT_I4) {
            *outPercent = value.lVal;
            ok = true;
        }
    }
    VariantClear(&value);
    return ok;
}

bool WmiSet(int percent) {
    auto services = ConnectWmi();
    if (!services) {
        return false;
    }

    BSTR language = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM WmiMonitorBrightnessMethods");
    winrt::com_ptr<IEnumWbemClassObject> enumerator;
    HRESULT hr = services->ExecQuery(language, query,
                                     WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                     nullptr, enumerator.put());
    SysFreeString(query);
    SysFreeString(language);
    if (FAILED(hr) || !enumerator) {
        return false;
    }

    IWbemClassObject* raw = nullptr;
    ULONG returned = 0;
    if (FAILED(enumerator->Next(2000, 1, &raw, &returned)) || returned == 0 || !raw) {
        return false;
    }
    winrt::com_ptr<IWbemClassObject> instance;
    instance.attach(raw);

    VARIANT pathValue;
    VariantInit(&pathValue);
    if (FAILED(instance->Get(L"__PATH", 0, &pathValue, nullptr, nullptr)) ||
        pathValue.vt != VT_BSTR) {
        VariantClear(&pathValue);
        return false;
    }

    bool ok = false;
    BSTR className = SysAllocString(L"WmiMonitorBrightnessMethods");
    BSTR methodName = SysAllocString(L"WmiSetBrightness");
    winrt::com_ptr<IWbemClassObject> classDef;
    if (SUCCEEDED(services->GetObject(className, 0, nullptr, classDef.put(), nullptr)) &&
        classDef) {
        winrt::com_ptr<IWbemClassObject> inParamsDef;
        if (SUCCEEDED(classDef->GetMethod(methodName, 0, inParamsDef.put(), nullptr)) &&
            inParamsDef) {
            winrt::com_ptr<IWbemClassObject> inParams;
            if (SUCCEEDED(inParamsDef->SpawnInstance(0, inParams.put())) && inParams) {
                VARIANT timeout;
                VariantInit(&timeout);
                timeout.vt = VT_I4;
                timeout.lVal = 0;
                inParams->Put(L"Timeout", 0, &timeout, 0);
                VariantClear(&timeout);

                VARIANT level;
                VariantInit(&level);
                level.vt = VT_UI1;
                level.bVal = static_cast<BYTE>(std::clamp(percent, 0, 100));
                inParams->Put(L"Brightness", 0, &level, 0);
                VariantClear(&level);

                ok = SUCCEEDED(services->ExecMethod(pathValue.bstrVal, methodName, 0, nullptr,
                                                    inParams.get(), nullptr, nullptr));
            }
        }
    }
    SysFreeString(methodName);
    SysFreeString(className);
    VariantClear(&pathValue);
    return ok;
}

// dxva2 is loaded dynamically so a missing import library can never break the
// build; these entry points are also absent on some server SKUs.
using GetNumberOfPhysicalMonitors_t = BOOL(WINAPI*)(HMONITOR, LPDWORD);
using GetPhysicalMonitors_t = BOOL(WINAPI*)(HMONITOR, DWORD, LPPHYSICAL_MONITOR);
using DestroyPhysicalMonitors_t = BOOL(WINAPI*)(DWORD, LPPHYSICAL_MONITOR);
using GetMonitorBrightness_t = BOOL(WINAPI*)(HANDLE, LPDWORD, LPDWORD, LPDWORD);
using SetMonitorBrightness_t = BOOL(WINAPI*)(HANDLE, DWORD);

struct Dxva2 {
    HMODULE module = nullptr;
    GetNumberOfPhysicalMonitors_t getCount = nullptr;
    GetPhysicalMonitors_t getMonitors = nullptr;
    DestroyPhysicalMonitors_t destroy = nullptr;
    GetMonitorBrightness_t get = nullptr;
    SetMonitorBrightness_t set = nullptr;
    bool valid() const { return getCount && getMonitors && destroy && get && set; }
};

const Dxva2& GetDxva2() {
    static Dxva2 api = [] {
        Dxva2 result;
        result.module = LoadLibraryEx(L"dxva2.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!result.module) {
            return result;
        }
        result.getCount = reinterpret_cast<GetNumberOfPhysicalMonitors_t>(
            GetProcAddress(result.module, "GetNumberOfPhysicalMonitorsFromHMONITOR"));
        result.getMonitors = reinterpret_cast<GetPhysicalMonitors_t>(
            GetProcAddress(result.module, "GetPhysicalMonitorsFromHMONITOR"));
        result.destroy = reinterpret_cast<DestroyPhysicalMonitors_t>(
            GetProcAddress(result.module, "DestroyPhysicalMonitors"));
        result.get = reinterpret_cast<GetMonitorBrightness_t>(
            GetProcAddress(result.module, "GetMonitorBrightness"));
        result.set = reinterpret_cast<SetMonitorBrightness_t>(
            GetProcAddress(result.module, "SetMonitorBrightness"));
        return result;
    }();
    return api;
}

HMONITOR BarMonitor();  // defined with the monitor helpers below

bool DdcForEach(const std::function<bool(HANDLE)>& callback) {
    const Dxva2& api = GetDxva2();
    if (!api.valid()) {
        return false;
    }
    HMONITOR monitor = BarMonitor();
    if (!monitor) {
        return false;
    }
    DWORD count = 0;
    if (!api.getCount(monitor, &count) || count == 0) {
        return false;
    }
    std::vector<PHYSICAL_MONITOR> monitors(count);
    if (!api.getMonitors(monitor, count, monitors.data())) {
        return false;
    }
    bool any = false;
    for (auto& physical : monitors) {
        if (callback(physical.hPhysicalMonitor)) {
            any = true;
        }
    }
    api.destroy(count, monitors.data());
    return any;
}

bool DdcGet(int* outPercent) {
    const Dxva2& api = GetDxva2();
    int found = -1;
    DdcForEach([&](HANDLE handle) {
        DWORD minimum = 0, current = 0, maximum = 0;
        if (api.get(handle, &minimum, &current, &maximum) && maximum > minimum) {
            found = static_cast<int>((current - minimum) * 100 / (maximum - minimum));
            return true;
        }
        return false;
    });
    if (found < 0) {
        return false;
    }
    *outPercent = std::clamp(found, 0, 100);
    return true;
}

bool DdcSet(int percent) {
    const Dxva2& api = GetDxva2();
    return DdcForEach([&](HANDLE handle) {
        DWORD minimum = 0, current = 0, maximum = 0;
        if (!api.get(handle, &minimum, &current, &maximum) || maximum <= minimum) {
            return false;
        }
        DWORD target = minimum + (maximum - minimum) * std::clamp(percent, 0, 100) / 100;
        return api.set(handle, target) != FALSE;
    });
}

// Last value we read or wrote. Reading brightness is a full WMI query, far too
// slow to do once per wheel notch, so the wheel path works off this.
int g_lastKnown = -1;

int Get() {
    std::lock_guard<std::mutex> lock(g_brightnessMutex);
    int percent = 0;
    if (g_backend == Backend::Unknown || g_backend == Backend::Wmi) {
        if (WmiGet(&percent)) {
            g_backend = Backend::Wmi;
            g_lastKnown = std::clamp(percent, 0, 100);
            return g_lastKnown;
        }
    }
    if (g_backend == Backend::Unknown || g_backend == Backend::Ddc) {
        if (DdcGet(&percent)) {
            g_backend = Backend::Ddc;
            g_lastKnown = std::clamp(percent, 0, 100);
            return g_lastKnown;
        }
    }
    if (g_backend == Backend::Unknown) {
        g_backend = Backend::None;
    }
    return 50;
}

int GetFast() {
    {
        std::lock_guard<std::mutex> lock(g_brightnessMutex);
        if (g_lastKnown >= 0) return g_lastKnown;
    }
    return Get(); // Get() will lock and update the cache
}

void Set(int percent) {
    std::lock_guard<std::mutex> lock(g_brightnessMutex);
    percent = std::clamp(percent, 0, 100);
    g_lastKnown = percent;
    switch (g_backend) {
        case Backend::Wmi:
            if (!WmiSet(percent)) {
                DdcSet(percent);
            }
            return;
        case Backend::Ddc:
            DdcSet(percent);
            return;
        case Backend::None:
            return;
        case Backend::Unknown:
        default:
            if (WmiSet(percent)) {
                g_backend = Backend::Wmi;
            } else if (DdcSet(percent)) {
                g_backend = Backend::Ddc;
            } else {
                g_backend = Backend::None;
            }
            return;
    }
}

bool Available() {
    if (g_backend == Backend::Unknown) {
        Get();
    }
    return g_backend != Backend::None;
}

}  // namespace brightness

// ============================================================================
// Dark mode
// ============================================================================

bool IsAppsDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValue(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                    L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) !=
        ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

void SetAppsDarkMode(bool dark) {
    HKEY key = nullptr;
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
                       nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
        return;
    }
    DWORD value = dark ? 0 : 1;
    RegSetValueEx(key, L"AppsUseLightTheme", 0, REG_DWORD,
                  reinterpret_cast<const BYTE*>(&value), sizeof(value));
    RegSetValueEx(key, L"SystemUsesLightTheme", 0, REG_DWORD,
                  reinterpret_cast<const BYTE*>(&value), sizeof(value));
    RegCloseKey(key);
    // Broadcast so already-running apps repaint, the same notification the
    // Settings app sends.
    DWORD_PTR result = 0;
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                       reinterpret_cast<LPARAM>(L"ImmersiveColorSet"), SMTO_ABORTIFHUNG, 200,
                       &result);
}


// ============================================================================
// Wi-Fi subsystem
//
// wlanapi.dll is resolved at runtime rather than linked, so a toolchain without
// the import library still builds and a machine with no WLAN service degrades
// to "Wi-Fi unavailable" instead of failing to load the mod.
// ============================================================================

namespace wifi {

using WlanOpenHandle_t = DWORD(WINAPI*)(DWORD, PVOID, PDWORD, PHANDLE);
using WlanCloseHandle_t = DWORD(WINAPI*)(HANDLE, PVOID);
using WlanEnumInterfaces_t = DWORD(WINAPI*)(HANDLE, PVOID, PWLAN_INTERFACE_INFO_LIST*);
using WlanQueryInterface_t = DWORD(WINAPI*)(HANDLE, const GUID*, WLAN_INTF_OPCODE, PVOID,
                                            PDWORD, PVOID*, PWLAN_OPCODE_VALUE_TYPE);
using WlanSetInterface_t = DWORD(WINAPI*)(HANDLE, const GUID*, WLAN_INTF_OPCODE, DWORD,
                                          const PVOID, PVOID);
using WlanGetAvailableNetworkList_t = DWORD(WINAPI*)(HANDLE, const GUID*, DWORD, PVOID,
                                                     PWLAN_AVAILABLE_NETWORK_LIST*);
using WlanScan_t = DWORD(WINAPI*)(HANDLE, const GUID*, const PDOT11_SSID,
                                  const PWLAN_RAW_DATA, PVOID);
using WlanConnect_t = DWORD(WINAPI*)(HANDLE, const GUID*, const PWLAN_CONNECTION_PARAMETERS,
                                     PVOID);
using WlanDisconnect_t = DWORD(WINAPI*)(HANDLE, const GUID*, PVOID);
using WlanSetProfile_t = DWORD(WINAPI*)(HANDLE, const GUID*, DWORD, LPCWSTR, LPCWSTR, BOOL,
                                        PVOID, DWORD*);
using WlanDeleteProfile_t = DWORD(WINAPI*)(HANDLE, const GUID*, LPCWSTR, PVOID);
using WlanFreeMemory_t = VOID(WINAPI*)(PVOID);

struct Api {
    HMODULE module = nullptr;
    WlanOpenHandle_t open = nullptr;
    WlanCloseHandle_t close = nullptr;
    WlanEnumInterfaces_t enumInterfaces = nullptr;
    WlanQueryInterface_t query = nullptr;
    WlanSetInterface_t setInterface = nullptr;
    WlanGetAvailableNetworkList_t getNetworks = nullptr;
    WlanScan_t scan = nullptr;
    WlanConnect_t connect = nullptr;
    WlanDisconnect_t disconnect = nullptr;
    WlanSetProfile_t setProfile = nullptr;
    WlanDeleteProfile_t deleteProfile = nullptr;
    WlanFreeMemory_t freeMemory = nullptr;

    bool valid() const {
        return open && close && enumInterfaces && query && getNetworks && connect &&
               disconnect && freeMemory;
    }
};

const Api& GetApi() {
    static Api api = [] {
        Api result;
        result.module = LoadLibraryEx(L"wlanapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!result.module) {
            return result;
        }
        auto bind = [&](auto& target, const char* name) {
            target = reinterpret_cast<std::decay_t<decltype(target)>>(
                GetProcAddress(result.module, name));
        };
        bind(result.open, "WlanOpenHandle");
        bind(result.close, "WlanCloseHandle");
        bind(result.enumInterfaces, "WlanEnumInterfaces");
        bind(result.query, "WlanQueryInterface");
        bind(result.setInterface, "WlanSetInterface");
        bind(result.getNetworks, "WlanGetAvailableNetworkList");
        bind(result.scan, "WlanScan");
        bind(result.connect, "WlanConnect");
        bind(result.disconnect, "WlanDisconnect");
        bind(result.setProfile, "WlanSetProfile");
        bind(result.deleteProfile, "WlanDeleteProfile");
        bind(result.freeMemory, "WlanFreeMemory");
        return result;
    }();
    return api;
}

// RAII around the client handle plus the first usable interface GUID. Every
// operation opens its own handle -- these calls are infrequent and a long-lived
// handle would have to survive the WLAN service restarting.
struct Session {
    const Api& api = GetApi();
    HANDLE handle = nullptr;
    GUID interfaceGuid{};
    bool haveInterface = false;

    Session() {
        if (!api.valid()) {
            return;
        }
        DWORD negotiated = 0;
        if (api.open(2, nullptr, &negotiated, &handle) != ERROR_SUCCESS) {
            handle = nullptr;
            return;
        }
        PWLAN_INTERFACE_INFO_LIST list = nullptr;
        if (api.enumInterfaces(handle, nullptr, &list) == ERROR_SUCCESS && list) {
            for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
                interfaceGuid = list->InterfaceInfo[i].InterfaceGuid;
                haveInterface = true;
                if (list->InterfaceInfo[i].isState == wlan_interface_state_connected) {
                    break;  // prefer the connected adapter when there are several
                }
            }
        }
        if (list) {
            api.freeMemory(list);
        }
    }

    ~Session() {
        if (handle) {
            api.close(handle, nullptr);
        }
    }

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    bool ok() const { return handle && haveInterface; }
};

struct Network {
    std::wstring ssid;
    std::wstring profileName;
    int signal = 0;
    bool secured = false;
    bool connected = false;
    bool hasProfile = false;
    DOT11_AUTH_ALGORITHM authAlgorithm = DOT11_AUTH_ALGO_80211_OPEN;
    DOT11_CIPHER_ALGORITHM cipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
};

struct Status {
    bool available = false;
    bool radioOn = false;
    bool connected = false;
    std::wstring ssid;
    int signal = 0;
};

std::wstring SsidToString(const DOT11_SSID& ssid) {
    if (ssid.uSSIDLength == 0) {
        return L"";
    }
    int length = static_cast<int>(ssid.uSSIDLength);
    int needed = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(ssid.ucSSID),
                                     length, nullptr, 0);
    if (needed <= 0) {
        return L"";
    }
    std::wstring result(needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(ssid.ucSSID), length,
                        result.data(), needed);
    return result;
}

bool StringToSsid(const std::wstring& text, DOT11_SSID* out) {
    *out = {};
    int written = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
                                      reinterpret_cast<char*>(out->ucSSID),
                                      sizeof(out->ucSSID), nullptr, nullptr);
    if (written <= 0) {
        return false;
    }
    out->uSSIDLength = static_cast<ULONG>(written);
    return true;
}

bool RadioIsOn(const Session& session) {
    if (!session.ok() || !session.api.query) {
        return false;
    }
    DWORD size = 0;
    PWLAN_RADIO_STATE state = nullptr;
    if (session.api.query(session.handle, &session.interfaceGuid, wlan_intf_opcode_radio_state,
                          nullptr, &size, reinterpret_cast<PVOID*>(&state), nullptr) !=
            ERROR_SUCCESS ||
        !state) {
        return false;
    }
    bool on = false;
    for (DWORD i = 0; i < state->dwNumberOfPhys; i++) {
        const auto& phy = state->PhyRadioState[i];
        if (phy.dot11SoftwareRadioState == dot11_radio_state_on &&
            phy.dot11HardwareRadioState != dot11_radio_state_off) {
            on = true;
            break;
        }
    }
    session.api.freeMemory(state);
    return on;
}

bool SetRadio(bool on) {
    Session session;
    if (!session.ok() || !session.api.setInterface) {
        return false;
    }
    WLAN_PHY_RADIO_STATE state{};
    state.dwPhyIndex = 0;
    state.dot11SoftwareRadioState = on ? dot11_radio_state_on : dot11_radio_state_off;
    return session.api.setInterface(session.handle, &session.interfaceGuid,
                                    wlan_intf_opcode_radio_state, sizeof(state), &state,
                                    nullptr) == ERROR_SUCCESS;
}

Status GetStatus() {
    Status status;
    Session session;
    if (!session.ok()) {
        return status;
    }
    status.available = true;
    status.radioOn = RadioIsOn(session);

    DWORD size = 0;
    PWLAN_CONNECTION_ATTRIBUTES attributes = nullptr;
    if (session.api.query(session.handle, &session.interfaceGuid,
                          wlan_intf_opcode_current_connection, nullptr, &size,
                          reinterpret_cast<PVOID*>(&attributes), nullptr) == ERROR_SUCCESS &&
        attributes) {
        if (attributes->isState == wlan_interface_state_connected) {
            status.connected = true;
            status.ssid = SsidToString(attributes->wlanAssociationAttributes.dot11Ssid);
            status.signal =
                static_cast<int>(attributes->wlanAssociationAttributes.wlanSignalQuality);
        }
        session.api.freeMemory(attributes);
    }
    return status;
}

// Kicks the adapter into scanning. Results arrive asynchronously, so callers
// re-read the network list a moment later rather than expecting fresh data
// straight after this returns.
void RequestScan() {
    Session session;
    if (!session.ok() || !session.api.scan) {
        return;
    }
    session.api.scan(session.handle, &session.interfaceGuid, nullptr, nullptr, nullptr);
}

std::vector<Network> EnumerateNetworks() {
    std::vector<Network> networks;
    Session session;
    if (!session.ok()) {
        return networks;
    }

    PWLAN_AVAILABLE_NETWORK_LIST list = nullptr;
    if (session.api.getNetworks(session.handle, &session.interfaceGuid,
                                WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES,
                                nullptr, &list) != ERROR_SUCCESS ||
        !list) {
        return networks;
    }

    // The adapter reports one entry per (SSID, profile) pair, so the same
    // network can appear several times. Keep the strongest, and let any entry
    // that is connected or has a profile contribute those flags.
    std::map<std::wstring, Network> best;
    for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
        const WLAN_AVAILABLE_NETWORK& entry = list->Network[i];
        std::wstring ssid = SsidToString(entry.dot11Ssid);
        if (ssid.empty()) {
            continue;  // hidden network with no usable name
        }

        Network network;
        network.ssid = ssid;
        network.profileName = entry.strProfileName;
        network.signal = static_cast<int>(entry.wlanSignalQuality);
        network.secured = entry.bSecurityEnabled != FALSE;
        network.connected = (entry.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) != 0;
        network.hasProfile = (entry.dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE) != 0;
        network.authAlgorithm = entry.dot11DefaultAuthAlgorithm;
        network.cipherAlgorithm = entry.dot11DefaultCipherAlgorithm;

        auto found = best.find(ssid);
        if (found == best.end()) {
            best.emplace(ssid, std::move(network));
        } else {
            Network& existing = found->second;
            existing.signal = (std::max)(existing.signal, network.signal);
            existing.connected = existing.connected || network.connected;
            if (network.hasProfile && !existing.hasProfile) {
                existing.hasProfile = true;
                existing.profileName = network.profileName;
            }
        }
    }
    session.api.freeMemory(list);

    networks.reserve(best.size());
    for (auto& pair : best) {
        networks.push_back(std::move(pair.second));
    }

    // Connected first, then by signal, so the list reads top-down like the
    // Windows flyout does.
    std::sort(networks.begin(), networks.end(), [](const Network& a, const Network& b) {
        if (a.connected != b.connected) {
            return a.connected;
        }
        return a.signal > b.signal;
    });
    return networks;
}

std::wstring BuildProfileXml(const Network& network, const std::wstring& password) {
    std::wstring authentication = L"open";
    std::wstring encryption = L"none";
    bool usePassword = false;

    switch (network.authAlgorithm) {
        case DOT11_AUTH_ALGO_RSNA_PSK:
            authentication = L"WPA2PSK";
            encryption = network.cipherAlgorithm == DOT11_CIPHER_ALGO_TKIP ? L"TKIP" : L"AES";
            usePassword = true;
            break;
        case DOT11_AUTH_ALGO_WPA_PSK:
        case DOT11_AUTH_ALGO_WPA_NONE:
            authentication = L"WPAPSK";
            encryption = network.cipherAlgorithm == DOT11_CIPHER_ALGO_CCMP ? L"AES" : L"TKIP";
            usePassword = true;
            break;
        default:
            if (network.secured) {
                // WPA3-SAE and anything else unrecognised: WPA2PSK is the
                // widest-compatibility guess, and transition-mode APs accept it.
                authentication = L"WPA2PSK";
                encryption = L"AES";
                usePassword = true;
            }
            break;
    }

    std::wstring ssid = EscapeXmlAttr(network.ssid);
    std::wstring xml;
    xml += L"<?xml version=\"1.0\"?>";
    xml += L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">";
    xml += L"<name>" + ssid + L"</name>";
    xml += L"<SSIDConfig><SSID><name>" + ssid + L"</name></SSID></SSIDConfig>";
    xml += L"<connectionType>ESS</connectionType>";
    xml += L"<connectionMode>auto</connectionMode>";
    xml += L"<MSM><security><authEncryption>";
    xml += L"<authentication>" + authentication + L"</authentication>";
    xml += L"<encryption>" + encryption + L"</encryption>";
    xml += L"<useOneX>false</useOneX>";
    xml += L"</authEncryption>";
    if (usePassword) {
        xml += L"<sharedKey><keyType>passPhrase</keyType><protected>false</protected>";
        xml += L"<keyMaterial>" + EscapeXmlAttr(password) + L"</keyMaterial></sharedKey>";
    }
    xml += L"</security></MSM></WLANProfile>";
    return xml;
}

// A stored profile is enough on its own; otherwise one is written first from
// the network's advertised auth/cipher plus the password the user typed.
bool Connect(const Network& network, const std::wstring& password) {
    Session session;
    if (!session.ok()) {
        return false;
    }

    std::wstring profileName = network.hasProfile && !network.profileName.empty()
                                   ? network.profileName
                                   : network.ssid;

    if (!network.hasProfile) {
        if (!session.api.setProfile) {
            return false;
        }
        std::wstring xml = BuildProfileXml(network, password);
        DWORD reason = 0;
        DWORD result = session.api.setProfile(session.handle, &session.interfaceGuid, 0,
                                              xml.c_str(), nullptr, TRUE, nullptr, &reason);
        if (result != ERROR_SUCCESS) {
            Wh_Log(L"WlanSetProfile failed for %s: %u (reason %u)", network.ssid.c_str(),
                   result, reason);
            return false;
        }
        profileName = network.ssid;
    }

    DOT11_SSID ssid{};
    if (!StringToSsid(network.ssid, &ssid)) {
        return false;
    }

    WLAN_CONNECTION_PARAMETERS parameters{};
    parameters.wlanConnectionMode = wlan_connection_mode_profile;
    parameters.strProfile = profileName.c_str();
    parameters.pDot11Ssid = &ssid;
    parameters.dwFlags = 0;
    parameters.dot11BssType = dot11_BSS_type_infrastructure;

    DWORD result =
        session.api.connect(session.handle, &session.interfaceGuid, &parameters, nullptr);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"WlanConnect failed for %s: %u", network.ssid.c_str(), result);
        return false;
    }
    return true;
}

bool Disconnect() {
    Session session;
    if (!session.ok()) {
        return false;
    }
    return session.api.disconnect(session.handle, &session.interfaceGuid, nullptr) ==
           ERROR_SUCCESS;
}

// Used when a saved password turns out to be wrong: without this the stale
// profile keeps being reused and the password prompt never appears again.
bool ForgetProfile(const std::wstring& profileName) {
    Session session;
    if (!session.ok() || !session.api.deleteProfile || profileName.empty()) {
        return false;
    }
    return session.api.deleteProfile(session.handle, &session.interfaceGuid,
                                     profileName.c_str(), nullptr) == ERROR_SUCCESS;
}

}  // namespace wifi

// ============================================================================
// Bluetooth subsystem
//
// bthprops.cpl carries the Bluetooth API surface; like wlanapi it is resolved
// at runtime. Connecting is done by turning a device's installed services on,
// which is what "connect" means for the HID/audio profiles that make up almost
// everything a user has paired.
// ============================================================================

namespace bluetooth {

bool g_bluetoothRadioOn = false;
std::mutex g_bluetoothRadioMutex;

using BluetoothFindFirstRadio_t = HANDLE(WINAPI*)(const BLUETOOTH_FIND_RADIO_PARAMS*, HANDLE*);
using BluetoothFindNextRadio_t = BOOL(WINAPI*)(HANDLE, HANDLE*);
using BluetoothFindRadioClose_t = BOOL(WINAPI*)(HANDLE);
using BluetoothFindFirstDevice_t = HANDLE(WINAPI*)(const BLUETOOTH_DEVICE_SEARCH_PARAMS*,
                                                   BLUETOOTH_DEVICE_INFO*);
using BluetoothFindNextDevice_t = BOOL(WINAPI*)(HANDLE, BLUETOOTH_DEVICE_INFO*);
using BluetoothFindDeviceClose_t = BOOL(WINAPI*)(HANDLE);
using BluetoothEnumerateInstalledServices_t = DWORD(WINAPI*)(HANDLE,
                                                             const BLUETOOTH_DEVICE_INFO*,
                                                             DWORD*, GUID*);
using BluetoothSetServiceState_t = DWORD(WINAPI*)(HANDLE, const BLUETOOTH_DEVICE_INFO*,
                                                  const GUID*, DWORD);
using BluetoothGetRadioInfo_t = DWORD(WINAPI*)(HANDLE, PBLUETOOTH_RADIO_INFO);

struct Api {
    HMODULE module = nullptr;
    BluetoothFindFirstRadio_t findFirstRadio = nullptr;
    BluetoothFindNextRadio_t findNextRadio = nullptr;
    BluetoothFindRadioClose_t findRadioClose = nullptr;
    BluetoothFindFirstDevice_t findFirstDevice = nullptr;
    BluetoothFindNextDevice_t findNextDevice = nullptr;
    BluetoothFindDeviceClose_t findDeviceClose = nullptr;
    BluetoothEnumerateInstalledServices_t enumerateServices = nullptr;
    BluetoothSetServiceState_t setServiceState = nullptr;
    BluetoothGetRadioInfo_t getRadioInfo = nullptr;

    bool valid() const {
        return findFirstRadio && findRadioClose && findFirstDevice && findNextDevice &&
               findDeviceClose;
    }
};

const Api& GetApi() {
    static Api api = [] {
        Api result;
        // bthprops.cpl is the documented home for these exports; irprops.cpl is
        // the older name still present on some builds.
        result.module = LoadLibraryEx(L"bthprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!result.module) {
            result.module =
                LoadLibraryEx(L"irprops.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        if (!result.module) {
            return result;
        }
        auto bind = [&](auto& target, const char* name) {
            target = reinterpret_cast<std::decay_t<decltype(target)>>(
                GetProcAddress(result.module, name));
        };
        bind(result.findFirstRadio, "BluetoothFindFirstRadio");
        bind(result.findNextRadio, "BluetoothFindNextRadio");
        bind(result.findRadioClose, "BluetoothFindRadioClose");
        bind(result.findFirstDevice, "BluetoothFindFirstDevice");
        bind(result.findNextDevice, "BluetoothFindNextDevice");
        bind(result.findDeviceClose, "BluetoothFindDeviceClose");
        bind(result.enumerateServices, "BluetoothEnumerateInstalledServices");
        bind(result.setServiceState, "BluetoothSetServiceState");
        bind(result.getRadioInfo, "BluetoothGetRadioInfo");
        return result;
    }();
    return api;
}

struct RadioHandle {
    const Api& api = GetApi();
    HANDLE find = nullptr;
    HANDLE radio = nullptr;

    RadioHandle() {
        if (!api.valid()) {
            return;
        }
        BLUETOOTH_FIND_RADIO_PARAMS params{sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
        find = api.findFirstRadio(&params, &radio);
        if (!find) {
            radio = nullptr;
        }
    }

    ~RadioHandle() {
        if (radio) {
            CloseHandle(radio);
        }
        if (find) {
            api.findRadioClose(find);
        }
    }

    RadioHandle(const RadioHandle&) = delete;
    RadioHandle& operator=(const RadioHandle&) = delete;

    bool ok() const { return radio != nullptr; }
};

struct Device {
    BLUETOOTH_ADDRESS address{};
    std::wstring name;
    bool connected = false;
    bool paired = false;
    ULONG classOfDevice = 0;
    int batteryPercent = -1; // -1 = unknown
};

bool IsRadioOn() {
    std::lock_guard<std::mutex> lock(g_bluetoothRadioMutex);
    return g_bluetoothRadioOn;
}

bool Available() {
    // Only check if the API exists, NOT if the radio handle exists.
    // This keeps the toggle enabled when the radio is off, so you can turn it back on.
    return GetApi().valid();
}

bool SetRadio(bool on) {
#if TOPBAR_HAS_RADIOS
    try {
        using namespace winrt::Windows::Devices::Radios;
        auto access = Radio::RequestAccessAsync().get();
        if (access != RadioAccessStatus::Allowed) {
            Wh_Log(L"Radio access denied; cannot toggle Bluetooth");
            return false;
        }
        auto radios = Radio::GetRadiosAsync().get();
        for (auto&& radio : radios) {
            if (radio.Kind() != RadioKind::Bluetooth) {
                continue;
            }
            radio.SetStateAsync(on ? RadioState::On : RadioState::Off).get();
            return true;
        }
    } catch (const winrt::hresult_error& error) {
        Wh_Log(L"Bluetooth radio toggle failed: %s", error.message().c_str());
    } catch (...) {
    }
#else
    (void)on;
#endif
    return false;
}

// Helper: Query battery percentage for a Bluetooth LE device.
// Returns -1 if unknown or not available.
int GetBatteryPercent(const BLUETOOTH_ADDRESS& address) {
#if TOPBAR_HAS_BLUETOOTH_LE
    try {
        using namespace winrt::Windows::Devices::Bluetooth;
        using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;

        auto device = BluetoothLEDevice::FromBluetoothAddressAsync(address.ullLong).get();
        if (!device) return -1;

        auto servicesResult = device.GetGattServicesAsync().get();
        if (servicesResult.Status() != GattCommunicationStatus::Success) return -1;

        auto services = servicesResult.Services();
        for (auto&& service : services) {
            if (service.Uuid() == GattServiceUuids::Battery()) {
                auto characteristicsResult = service.GetCharacteristicsAsync().get();
                if (characteristicsResult.Status() != GattCommunicationStatus::Success) continue;
                auto characteristics = characteristicsResult.Characteristics();
                for (auto&& characteristic : characteristics) {
                    if (characteristic.Uuid() == GattCharacteristicUuids::BatteryLevel()) {
                        auto readResult = characteristic.ReadValueAsync().get();
                        if (readResult.Status() != GattCommunicationStatus::Success) continue;
                        auto value = readResult.Value();
                        auto buffer = value.as<winrt::Windows::Storage::Streams::IBuffer>();
                        uint8_t percent = 0;
                        winrt::Windows::Storage::Streams::DataReader reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer);
                        reader.ReadBytes(winrt::array_view<uint8_t>(&percent, 1));
                        return static_cast<int>(percent);
                    }
                }
            }
        }
    } catch (...) {}
#endif
    return -1;
}
std::vector<Device> Enumerate(bool includeUnpaired) {
    Wh_Log(L"Enumerate called (includeUnpaired=%d)", includeUnpaired ? 1 : 0);
    std::vector<Device> devices;
    const Api& api = GetApi();
    RadioHandle radio;
    if (!radio.ok()) {
        Wh_Log(L"Radio handle invalid");
        return devices;
    }

    BLUETOOTH_DEVICE_SEARCH_PARAMS params{};
    params.dwSize = sizeof(params);
    params.fReturnAuthenticated = TRUE;
    params.fReturnRemembered = TRUE;
    params.fReturnConnected = TRUE;
    params.fReturnUnknown = includeUnpaired ? TRUE : FALSE;
    params.fIssueInquiry = includeUnpaired ? TRUE : FALSE;
    params.cTimeoutMultiplier = includeUnpaired ? 3 : 0;  // ~4 s when inquiring
    params.hRadio = radio.radio;

    BLUETOOTH_DEVICE_INFO info{};
    info.dwSize = sizeof(info);

    HANDLE find = api.findFirstDevice(&params, &info);
    if (!find) {
        return devices;
    }

    do {
        Device device;
        device.address = info.Address;
        device.name = info.szName;
        device.connected = info.fConnected != FALSE;
        device.paired = info.fAuthenticated != FALSE;  // Only authenticated devices are truly paired
        device.classOfDevice = info.ulClassofDevice;
        if (device.name.empty()) {
            continue;  // skip devices with no name
        }
        if (device.connected) {
            device.batteryPercent = GetBatteryPercent(device.address);
        }
        devices.push_back(std::move(device));

        info = {};
        info.dwSize = sizeof(info);
    } while (api.findNextDevice(find, &info));

    api.findDeviceClose(find);

    // If the classic Bluetooth API returned nothing, try WinRT (works for some
    // Bluetooth LE‑only adapters).
    if (devices.empty() && includeUnpaired) {
        Wh_Log(L"Classic enumeration returned 0 devices; trying WinRT fallback...");
#if TOPBAR_HAS_BLUETOOTH_LE
        try {
            using namespace winrt::Windows::Devices::Enumeration;
            using namespace winrt::Windows::Devices::Bluetooth;

            // Query for Bluetooth devices (paired + discovered)
            auto deviceInfoCollection = DeviceInformation::FindAllAsync(
                BluetoothDevice::GetDeviceSelector()).get();

            for (auto const& deviceInfo : deviceInfoCollection) {
                Device device;
                device.name = deviceInfo.Name().c_str();
                if (device.name.empty()) continue;

                // Extract Bluetooth address from the device ID (format:
                // "Bluetooth#BluetoothLE&Dev_XX:XX:XX:XX:XX:XX...")
                std::wstring id = deviceInfo.Id().c_str();
                auto pos = id.find(L"Dev_");
                if (pos != std::wstring::npos) {
                    auto addr = id.substr(pos + 4);
                    // Convert colon-separated hex to ULONGLONG
                    std::wstring hexAddr;
                    for (auto c : addr) if (c != L':') hexAddr.push_back(c);
                    device.address.ullLong = std::stoull(hexAddr, nullptr, 16);
                }

                // Check if it's paired or connected
                device.paired = deviceInfo.Pairing().IsPaired();
                device.connected = false; // we can't easily determine from here

                if (device.connected) {
                    device.batteryPercent = GetBatteryPercent(device.address);
                }
                if (device.name.empty() || device.address.ullLong == 0) continue;

                devices.push_back(std::move(device));
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"WinRT Bluetooth enumeration failed: %s", e.message().c_str());
        } catch (...) {
            Wh_Log(L"WinRT Bluetooth enumeration unknown error");
        }
#endif
    }

    // Connected first, then paired, then discovered -- matching how the flyout
    // groups them.
    std::sort(devices.begin(), devices.end(), [](const Device& a, const Device& b) {
        if (a.connected != b.connected) return a.connected;
        if (a.paired != b.paired) return a.paired;
        return ToLowerCopy(a.name) < ToLowerCopy(b.name);
    });
    return devices;
}

// Attempts to pair with a discovered Bluetooth device using WinRT.

bool PairDevice(const BLUETOOTH_ADDRESS& address) {
    Wh_Log(L"PairDevice called for address 0x%llX", address.ullLong);
#if TOPBAR_HAS_BLUETOOTH_LE
    try {
        using namespace winrt::Windows::Devices::Bluetooth;
        using namespace winrt::Windows::Devices::Enumeration;
        using namespace winrt::Windows::Foundation;

        auto device = BluetoothDevice::FromBluetoothAddressAsync(address.ullLong).get();
        if (!device) {
            Wh_Log(L"BluetoothDevice not found");
            return false;
        }
        auto deviceInfo = DeviceInformation::CreateFromIdAsync(device.DeviceId()).get();
        if (!deviceInfo) {
            Wh_Log(L"DeviceInformation not found");
            return false;
        }

        // Get custom pairing interface
        auto customPairing = deviceInfo.Pairing().Custom();
        if (!customPairing) {
            Wh_Log(L"Custom pairing not supported");
            return false;
        }

        // Run pairing in a separate task with a 30‑second timeout
        auto future = std::async(std::launch::async, [customPairing]() {
            // Attach event handler to accept the pairing request
            auto pairingRequestedToken = customPairing.PairingRequested(
                [](DeviceInformationCustomPairing const&, DevicePairingRequestedEventArgs const& args) {
                    // Accept the pairing request (for devices that require a PIN,
                    // you would use args.Accept(pin) after getting the PIN)
                    args.Accept();
                });

            try {
                auto result = customPairing.PairAsync(DevicePairingKinds::ConfirmOnly).get();
                customPairing.PairingRequested(pairingRequestedToken); // unregister
                return result.Status() == DevicePairingResultStatus::Paired ||
                       result.Status() == DevicePairingResultStatus::AlreadyPaired;
            } catch (...) {
                customPairing.PairingRequested(pairingRequestedToken);
                return false;
            }
        });

        if (future.wait_for(std::chrono::seconds(30)) == std::future_status::ready) {
            bool result = future.get();
            Wh_Log(L"Pairing result: %d", result ? 1 : 0);
            return result;
        } else {
            Wh_Log(L"Pairing timed out");
            return false;
        }
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"PairDevice exception: %s", ex.message().c_str());
    } catch (...) {
        Wh_Log(L"PairDevice unknown exception");
    }
#endif
    return false;
}

bool FindDeviceInfo(const BLUETOOTH_ADDRESS& address, HANDLE radioHandle,
                    BLUETOOTH_DEVICE_INFO* out) {
    const Api& api = GetApi();
    BLUETOOTH_DEVICE_SEARCH_PARAMS params{};
    params.dwSize = sizeof(params);
    params.fReturnAuthenticated = TRUE;
    params.fReturnRemembered = TRUE;
    params.fReturnConnected = TRUE;
    params.fReturnUnknown = TRUE;
    params.fIssueInquiry = FALSE;
    params.cTimeoutMultiplier = 0;
    params.hRadio = radioHandle;

    BLUETOOTH_DEVICE_INFO info{};
    info.dwSize = sizeof(info);
    HANDLE find = api.findFirstDevice(&params, &info);
    if (!find) {
        return false;
    }

    bool found = false;
    do {
        if (info.Address.ullLong == address.ullLong) {
            *out = info;
            found = true;
            break;
        }
        info = {};
        info.dwSize = sizeof(info);
    } while (api.findNextDevice(find, &info));

    api.findDeviceClose(find);
    return found;
}

bool SetConnected(const BLUETOOTH_ADDRESS& address, bool connect) {
    const Api& api = GetApi();
    if (!api.enumerateServices || !api.setServiceState) {
        return false;
    }
    RadioHandle radio;
    if (!radio.ok()) {
        return false;
    }

    BLUETOOTH_DEVICE_INFO info{};
    if (!FindDeviceInfo(address, radio.radio, &info)) {
        return false;
    }

    DWORD serviceCount = 0;
    DWORD result = api.enumerateServices(radio.radio, &info, &serviceCount, nullptr);
    if (serviceCount == 0) {
        Wh_Log(L"No installed services for %s (0x%08X); nothing to toggle", info.szName,
               result);
        return false;
    }

    std::vector<GUID> services(serviceCount);
    if (api.enumerateServices(radio.radio, &info, &serviceCount, services.data()) !=
        ERROR_SUCCESS) {
        return false;
    }
    services.resize(serviceCount);

    bool any = false;
    if (connect) {
        for (const GUID& service : services) {
            api.setServiceState(radio.radio, &info, &service, BLUETOOTH_SERVICE_DISABLE);
        }
        Sleep(200);
        for (const GUID& service : services) {
            if (api.setServiceState(radio.radio, &info, &service, BLUETOOTH_SERVICE_ENABLE) == ERROR_SUCCESS) {
                any = true;
            }
        }
    } else {
        for (const GUID& service : services) {
            if (api.setServiceState(radio.radio, &info, &service, BLUETOOTH_SERVICE_DISABLE) == ERROR_SUCCESS) {
                any = true;
            }
        }
    }
    return any;
}

}  // namespace bluetooth

// ============================================================================
// WindhawkBlur: XAML composition blur brush
// ============================================================================

namespace wuc = winrt::Windows::UI::Composition;
namespace wge = winrt::Windows::Graphics::Effects;
namespace awge = ABI::Windows::Graphics::Effects;

template <>
inline constexpr winrt::guid winrt::impl::guid_v<
    winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>};

namespace ABI {
namespace Windows {
namespace Graphics {
namespace Effects {

typedef interface IGraphicsEffectD2D1Interop IGraphicsEffectD2D1Interop;

typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING {
    GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
} GRAPHICS_EFFECT_PROPERTY_MAPPING;

#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown,
                       "2FC57384-A068-44D7-A331-30982FCF7177")
{
    STDMETHOD(GetEffectId)(_Out_ GUID * id) PURE;
    STDMETHOD(GetNamedPropertyMapping)(
        LPCWSTR name, _Out_ UINT* index,
        _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING * mapping) PURE;
    STDMETHOD(GetPropertyCount)(_Out_ UINT * count) PURE;
    STDMETHOD(GetProperty)(
        UINT index,
        _Outptr_ winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>**
            value) PURE;
    STDMETHOD(GetSource)(UINT index, _Outptr_ IGraphicsEffectSource * *source)
        PURE;
    STDMETHOD(GetSourceCount)(_Out_ UINT * count) PURE;
};

}  // namespace Effects
}  // namespace Graphics
}  // namespace Windows
}  // namespace ABI

template <>
inline constexpr winrt::guid winrt::impl::guid_v<
    ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7,
    {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

struct CompositeEffect
    : winrt::implements<CompositeEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1Composite;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        if (std::wstring_view(name) == L"Mode") {
            *index = D2D1_COMPOSITE_PROP_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>**
            value) noexcept override try {
        if (!value) return E_INVALIDARG;
        if (index == D2D1_COMPOSITE_PROP_MODE) {
            *value = wf::PropertyValue::CreateUInt32((UINT32)Mode)
                         .as<winrt::impl::abi_t<
                             winrt::Windows::Foundation::IPropertyValue>>()
                         .detach();
            return S_OK;
        }
        return E_BOUNDS;
    } catch (...) { return winrt::to_hresult(); }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index,
        awge::IGraphicsEffectSource** source) noexcept override try {
        if (!source) return E_INVALIDARG;
        winrt::copy_to_abi(Sources.at(index),
                           *reinterpret_cast<void**>(source));
        return S_OK;
    } catch (...) { return winrt::to_hresult(); }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = static_cast<UINT>(Sources.size());
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }
    std::vector<wge::IGraphicsEffectSource> Sources;
    D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
   private:
    winrt::hstring m_name = L"CompositeEffect";
};

struct FloodEffect
    : winrt::implements<FloodEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1Flood;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        if (std::wstring_view(name) == L"Color") {
            *index = D2D1_FLOOD_PROP_COLOR;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>**
            value) noexcept override try {
        if (!value) return E_INVALIDARG;
        if (index == D2D1_FLOOD_PROP_COLOR) {
            *value = wf::PropertyValue::CreateSingleArray({
                Color.R / 255.0f,
                Color.G / 255.0f,
                Color.B / 255.0f,
                Color.A / 255.0f,
            }).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>()
                .detach();
            return S_OK;
        }
        return E_BOUNDS;
    } catch (...) { return winrt::to_hresult(); }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT, awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) return E_INVALIDARG;
        return E_BOUNDS;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 0;
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }
    winrt::Windows::UI::Color Color{};
   private:
    winrt::hstring m_name = L"FloodEffect";
};

struct GaussianBlurEffect
    : winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1GaussianBlur;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        std::wstring_view nameView(name);
        if (nameView == L"BlurAmount") {
            *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        if (nameView == L"Optimization") {
            *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        if (nameView == L"BorderMode") {
            *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>**
            value) noexcept override try {
        if (!value) return E_INVALIDARG;
        switch (index) {
            case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION:
                *value = wf::PropertyValue::CreateSingle(BlurAmount)
                             .as<winrt::impl::abi_t<
                                 winrt::Windows::Foundation::IPropertyValue>>()
                             .detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION:
                *value = wf::PropertyValue::CreateUInt32((UINT32)Optimization)
                             .as<winrt::impl::abi_t<
                                 winrt::Windows::Foundation::IPropertyValue>>()
                             .detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE:
                *value = wf::PropertyValue::CreateUInt32((UINT32)BorderMode)
                             .as<winrt::impl::abi_t<
                                 winrt::Windows::Foundation::IPropertyValue>>()
                             .detach();
                break;
            default:
                return E_BOUNDS;
        }
        return S_OK;
    } catch (...) { return winrt::to_hresult(); }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index,
        awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) return E_INVALIDARG;
        if (index == 0) {
            winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
            return S_OK;
        }
        return E_BOUNDS;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }
    wge::IGraphicsEffectSource Source{nullptr};
    float BlurAmount = 3.0f;
    DWORD Optimization = 1;
    D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;
   private:
    winrt::hstring m_name = L"GaussianBlurEffect";
};

std::vector<winrt::weak_ref<wuxm::XamlCompositionBrushBase>> g_liveBlurBrushes;

class XamlBlurBrush : public wuxm::XamlCompositionBrushBaseT<XamlBlurBrush> {
   public:
    XamlBlurBrush(UIElement element, float blurAmount, wui::Color tint,
                  uint8_t tintOpacity)
        : m_compositor(wuxh::ElementCompositionPreview::GetElementVisual(element)
                           .Compositor()),
          m_blurAmount(blurAmount),
          m_tint(tint) {
        m_tint.A = tintOpacity;
        try {
            wuxm::XamlCompositionBrushBase base = *this;
            g_liveBlurBrushes.push_back(winrt::make_weak(base));
        } catch (...) {
        }
    }

    ~XamlBlurBrush() {}

    void OnConnected() {
        if (!CompositionBrush()) {
            CompositionBrush(CreateEffectBrush());
        }
    }

    void OnDisconnected() {
        if (auto brush = CompositionBrush()) {
            brush.Close();
            CompositionBrush(nullptr);
        }
    }

    void Rebuild() {
        OnDisconnected();
        OnConnected();
    }

   private:
    wuc::CompositionBrush CreateEffectBrush() {
        auto backdropBrush = m_compositor.CreateBackdropBrush();

        auto blurEffect = winrt::make_self<GaussianBlurEffect>();
        blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
        blurEffect->BlurAmount = m_blurAmount;
        blurEffect->Name(L"BlurEffect");

        auto floodEffect = winrt::make_self<FloodEffect>();
        floodEffect->Color = m_tint;
        floodEffect->Name(L"FloodEffect");

        auto compositeEffect = winrt::make_self<CompositeEffect>();
        compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
        compositeEffect->Sources.push_back(*blurEffect);
        compositeEffect->Sources.push_back(*floodEffect);

        auto factory = m_compositor.CreateEffectFactory(*compositeEffect);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"backdrop", backdropBrush);
        return brush;
    }

    wuc::Compositor m_compositor;
    float m_blurAmount;
    wui::Color m_tint;
};

std::optional<WindhawkBlurParams> ParseWindhawkBlurValue(std::wstring_view value) {
    std::wstring s(value);
    if (s.size() < 15) return std::nullopt;
    if (s.compare(0, 13, L"<WindhawkBlur") != 0) return std::nullopt;
    size_t close = s.rfind(L"/>");
    if (close == std::wstring::npos) return std::nullopt;
    std::wstring body = s.substr(13, close - 13);

    WindhawkBlurParams p;
    size_t i = 0;
    while (i < body.size()) {
        while (i < body.size() && (body[i] == L' ' || body[i] == L'\t')) i++;
        if (i >= body.size()) break;
        size_t eq = body.find(L'=', i);
        if (eq == std::wstring::npos) break;
        std::wstring key = TrimWs(body.substr(i, eq - i));
        size_t q1 = body.find(L'"', eq);
        if (q1 == std::wstring::npos) break;
        size_t q2 = body.find(L'"', q1 + 1);
        if (q2 == std::wstring::npos) break;
        std::wstring val = body.substr(q1 + 1, q2 - q1 - 1);
        i = q2 + 1;

        if (key == L"BlurAmount") {
            try { p.blurAmount = std::stof(val); } catch (...) {}
        } else if (key == L"TintColor") {
            wui::Color c{};
            if (TryParseHexColor(val, &c)) p.tint = c;
        } else if (key == L"TintOpacity") {
            try {
                float op = std::stof(val);
                if (op < 0.0f) op = 0.0f;
                if (op > 1.0f) op = 1.0f;
                p.tintOpacity = static_cast<uint8_t>(op * 255.0f);
            } catch (...) {}
        }
    }
    return p;
}

wuxm::XamlCompositionBrushBase CreateWindhawkBlurBrush(
    UIElement const& element, WindhawkBlurParams const& params) {
    bool tintRequested = params.tintOpacity.has_value() || params.tint.A != 0 ||
                         params.tint.R != 0 || params.tint.G != 0 || params.tint.B != 0;
    uint8_t alpha = tintRequested ? params.tintOpacity.value_or(params.tint.A) : 0;
    return winrt::make<XamlBlurBrush>(element, params.blurAmount, params.tint, alpha);
}

bool ElementIsInVisualState(FrameworkElement const& element, std::wstring const& stateName) {
    try {
        auto groups = VisualStateManager::GetVisualStateGroups(element);
        for (auto const& g : groups) {
            auto cur = g.CurrentState();
            if (cur && std::wstring(cur.Name()) == stateName) return true;
        }
    } catch (...) {}
    return false;
}

void ApplyIconColorToElement(FrameworkElement element, const std::wstring& value) {
    wui::Color color{};
    if (!TryParseHexColor(value, &color) && !TryParseNamedColor(value, &color)) {
        Wh_Log(L"Bad IconColor value: %s", value.c_str());
        return;
    }
    auto brush = MakeBrush(color.A, color.R, color.G, color.B);

    using namespace winrt::Windows::UI::Xaml::Shapes;

    std::function<void(DependencyObject)> walk = [&](DependencyObject node) {
        if (!node) return;
        try {
            // Controls whose template draws its own Shape-based visuals
            // (ToggleSwitch track + knob, Slider track + thumb, ProgressRing,
            // ProgressBar, ScrollBar). Recoloring their internals overwrites
            // state-driven fills -- in the ToggleSwitch case the walk paints
            // a solid IconColor rectangle straight over the whole switch.
            // None of them contain vector icons in this mod, so skipping
            // their subtree is safe.
            if (node.try_as<wuxc::ToggleSwitch>() ||
                node.try_as<wuxc::Slider>() ||
                node.try_as<wuxc::ProgressRing>() ||
                node.try_as<wuxc::ProgressBar>() ||
                node.try_as<wuxc::Primitives::ScrollBar>()) {
                return;
            }
            // Path, Rectangle, Ellipse, Line, Polygon, Polyline all derive
            // from Shape, so one check covers every vector glyph the mod
            // builds, including the Start logo which is four Rectangles.
            if (auto shape = node.try_as<Shape>()) {
                if (shape.Fill() != nullptr) shape.Fill(brush);
                if (shape.Stroke() != nullptr) shape.Stroke(brush);
            } else if (auto fi = node.try_as<wuxc::FontIcon>()) {
                fi.Foreground(brush);
            } else if (auto text = node.try_as<wuxc::TextBlock>()) {
                // FontIcon's template renders the glyph through an internal
                // TextBlock. Only recolor that one -- a standalone TextBlock
                // is a label and follows the text color, not the icon color.
                if (text.Parent().try_as<wuxc::FontIcon>()) {
                    text.Foreground(brush);
                }
            }
            int n = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < n; i++) {
                walk(wuxm::VisualTreeHelper::GetChild(node, i));
            }
        } catch (...) {}
    };
    walk(element);
}

// ============================================================================
// Flyout infrastructure
// ============================================================================

constexpr double kFlyoutCorner = 12.0;
constexpr double kTileCorner = 12.0;
constexpr double kRowCorner = 12.0;
constexpr double kPanelWidth = 340.0;

[[clang::no_destroy]] wuxc::Button g_displayButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_soundButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_wifiButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_bluetoothButton{nullptr};


// Helper to toggle a flyout open/closed.
void ToggleFlyout(wuxc::Flyout const& flyout, wuxc::Button const& button) {
    if (!flyout || !button) return;
    if (flyout.IsOpen()) {
        flyout.Hide();
    } else {
        flyout.ShowAt(button);
    }
}

[[clang::no_destroy]] wuxc::Flyout g_displayFlyout{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_soundFlyout{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_wifiFlyout{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_bluetoothFlyout{nullptr};

[[clang::no_destroy]] DispatcherTimer g_volumeRevertTimer{nullptr};
[[clang::no_destroy]] DispatcherTimer g_brightnessRevertTimer{nullptr};

[[clang::no_destroy]] wuxc::StackPanel g_displayPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_soundPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_wifiPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_bluetoothPanel{nullptr};

[[clang::no_destroy]] wuxc::Button g_batteryButton{nullptr};
[[clang::no_destroy]] wuxc::Flyout g_batteryFlyout{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_batteryPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_weatherPanel{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_recycleBinPanel{nullptr};
// Set while a panel is writing its own controls, so the ValueChanged /Toggled
// handlers that XAML raises during construction don't get mistaken for the user
// actually moving something.
bool g_populatingPanel = false;

// Non-zero once the mod starts tearing down; background workers check it before
// touching XAML or dispatching back to the UI thread.
volatile LONG g_shuttingDown = 0;



// (Crash flag mechanism removed – no registry key needed)



// Which panel sections the user has expanded. Kept outside the panels because
// the panels are rebuilt wholesale on every refresh.
bool g_appMixerExpanded = false;
bool g_outputDevicesExpanded = false;

// Wi-Fi password entry state: when set, the Wi-Fi panel draws the password view
// for this network instead of the network list.
bool g_wifiPasswordPrompt = false;
wifi::Network g_wifiPromptNetwork;
std::wstring g_wifiPromptError;

std::vector<bluetooth::Device> g_bluetoothDevices;




std::vector<wifi::Network> g_wifiNetworks;
std::atomic<bool> g_wifiScanning{false};
std::atomic<bool> g_bluetoothScanning{false};
std::atomic<int> g_bluetoothConnectingState{0};
std::atomic<unsigned long long> g_bluetoothConnectingAddress{0};
std::atomic<unsigned long long> g_pairingDeviceAddress{0};
std::wstring g_wifiConnectingSSID;


// bool g_bluetoothRadioOn = false;   // moved inside bluetooth namespace
// std::mutex g_bluetoothRadioMutex;  // moved inside bluetooth namespace

void RunOnUiThread(std::function<void()> work) {
    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) != 0) {
        return;
    }
    auto guarded = [work = std::move(work)]() {
        if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) != 0) {
            return;
        }
        try {
            work();
        } catch (...) {
        }
    };

    if (g_uiDispatcherQueue) {
        g_uiDispatcherQueue.TryEnqueue(guarded);
        return;
    }

    // WindowsXamlManager normally provides a DispatcherQueue on the island
    // thread, but if it didn't, the XAML CoreDispatcher reaches the same thread.
    if (g_rootElement) {
        try {
            g_rootElement.Dispatcher().RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, guarded);
        } catch (...) {
        }
    }
}

// Wi-Fi scans and Bluetooth inquiry block for seconds; running them on the UI
// thread would freeze the whole bar. Each worker owns its own apartment, and
// Wh_ModUninit waits for the count to drain.
std::atomic<int> g_backgroundJobs{0};
std::vector<HANDLE> g_workerThreads;
std::mutex g_workerThreadsMutex;

void RunInBackground(std::function<void()> work) {
    if (InterlockedCompareExchange(&g_shuttingDown, 0, 0) != 0) {
        return;
    }
    // Reap finished threads to avoid accumulating handles
    {
        std::lock_guard<std::mutex> lock(g_workerThreadsMutex);
        for (auto it = g_workerThreads.begin(); it != g_workerThreads.end();) {
            if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
                CloseHandle(*it);
                it = g_workerThreads.erase(it);
            } else {
                ++it;
            }
        }
    }
    g_backgroundJobs.fetch_add(1);
    auto* workPtr = new std::function<void()>(std::move(work));
    HANDLE threadHandle = (HANDLE)_beginthreadex(nullptr, 0, [](void* param) -> unsigned int {
        auto* workPtr = static_cast<std::function<void()>*>(param);
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        try {
            (*workPtr)();
        } catch (...) {
        }
        if (SUCCEEDED(hr)) {
            CoUninitialize();
        }
        g_backgroundJobs.fetch_sub(1);
        delete workPtr;
        return 0;
    }, workPtr, 0, nullptr);
    if (threadHandle) {
        std::lock_guard<std::mutex> lock(g_workerThreadsMutex);
        g_workerThreads.push_back(threadHandle);
    }
}

std::mutex g_volumeCoalesceMutex;
int g_pendingVolumeValue = -1;
bool g_volumeWorkerRunning = false;

void SetMasterVolumeCoalesced(int value) {
    {
        std::lock_guard<std::mutex> lock(g_volumeCoalesceMutex);
        g_pendingVolumeValue = value;
        if (g_volumeWorkerRunning) {
            return;
        }
        g_volumeWorkerRunning = true;
    }
    RunInBackground([] {
        for (;;) {
            int v;
            {
                std::lock_guard<std::mutex> lock(g_volumeCoalesceMutex);
                if (g_pendingVolumeValue < 0) {
                    g_volumeWorkerRunning = false;
                    return;
                }
                v = g_pendingVolumeValue;
                g_pendingVolumeValue = -1;
            }
            audio::SetMasterVolume(v);
        }
    });
}

std::mutex g_brightnessCoalesceMutex;
int g_pendingBrightnessValue = -1;
bool g_brightnessWorkerRunning = false;

void SetBrightnessCoalesced(int value) {
    {
        std::lock_guard<std::mutex> lock(g_brightnessCoalesceMutex);
        g_pendingBrightnessValue = value;
        if (g_brightnessWorkerRunning) {
            return;
        }
        g_brightnessWorkerRunning = true;
    }
    RunInBackground([] {
        for (;;) {
            int v;
            {
                std::lock_guard<std::mutex> lock(g_brightnessCoalesceMutex);
                if (g_pendingBrightnessValue < 0) {
                    g_brightnessWorkerRunning = false;
                    return;
                }
                v = g_pendingBrightnessValue;
                g_pendingBrightnessValue = -1;
            }
            brightness::Set(v);
        }
    });
}

// Panels tear themselves down from inside their own click handlers, which would
// destroy the very button that is still dispatching. Rebuilding on the next
// dispatcher turn avoids that.
void RepopulateLater(const std::function<void()>& populate) {
    RunOnUiThread(populate);
}

wuxm::SolidColorBrush FlyoutBackgroundBrush() {
    // Flyouts are fully transparent – only the blur shows through.
    return wuxm::SolidColorBrush(wui::ColorHelper::FromArgb(0, 0, 0, 0));
}

wuxc::ControlTemplate g_flyoutShellTemplate{nullptr};
wuxc::ControlTemplate g_menuShellTemplate{nullptr};

wuxc::ControlTemplate BuildFlyoutShellTemplate(bool isMenu) {
    static const wchar_t* kFlyoutShell = LR"XAML(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
                 TargetType="FlyoutPresenter">
  <Grid Background="Transparent">
    <Grid.ChildrenTransitions>
      <TransitionCollection>
        <PopupThemeTransition/>
      </TransitionCollection>
    </Grid.ChildrenTransitions>
    <Border x:Name="PART_BackgroundBorder"
            Background="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=Background}"
            BorderBrush="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=BorderBrush}"
            BorderThickness="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=BorderThickness}"
            CornerRadius="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=CornerRadius}">
      <ContentPresenter Content="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=Content}"
                        ContentTemplate="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=ContentTemplate}"
                        ContentTransitions="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=ContentTransitions}"
                        Padding="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=Padding}"
                        HorizontalContentAlignment="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=HorizontalContentAlignment}"
                        VerticalContentAlignment="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=VerticalContentAlignment}"/>
    </Border>
  </Grid>
</ControlTemplate>)XAML";
    static const wchar_t* kMenuShell = LR"XAML(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
                 TargetType="MenuFlyoutPresenter">
  <Grid Background="Transparent">
    <Grid.ChildrenTransitions>
      <TransitionCollection>
        <PopupThemeTransition/>
      </TransitionCollection>
    </Grid.ChildrenTransitions>
    <Border x:Name="PART_BackgroundBorder"
            Background="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=Background}"
            BorderBrush="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=BorderBrush}"
            BorderThickness="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=BorderThickness}"
            CornerRadius="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=CornerRadius}">
      <ItemsPresenter Padding="{Binding RelativeSource={RelativeSource TemplatedParent}, Path=Padding}"/>
    </Border>
  </Grid>
</ControlTemplate>)XAML";
    try {
        return Markup::XamlReader::Load(isMenu ? kMenuShell : kFlyoutShell)
            .as<wuxc::ControlTemplate>();
    } catch (...) {
        return wuxc::ControlTemplate{nullptr};
    }
}

Style MakeFlyoutPresenterStyle(double width) {
    Style style(winrt::xaml_typename<wuxc::FlyoutPresenter>());
    auto setters = style.Setters();
    if (!g_settings.fontFamily.empty()) {
        try {
            setters.Append(Setter(wuxc::Control::FontFamilyProperty(),
                                  winrt::box_value(wuxm::FontFamily(winrt::hstring(g_settings.fontFamily)))));
        } catch (...) {}
    }
    if (g_settings.fontBold) {
        setters.Append(Setter(wuxc::Control::FontWeightProperty(),
                              winrt::box_value(winrt::Windows::UI::Text::FontWeights::Bold())));
    }
    setters.Append(Setter(wuxc::Control::BackgroundProperty(),
                          winrt::box_value(FlyoutBackgroundBrush())));
    setters.Append(Setter(wuxc::Control::BorderBrushProperty(),
                          winrt::box_value(MakeBrush(0x30, 0xFF, 0xFF, 0xFF))));
    setters.Append(Setter(wuxc::Control::BorderThicknessProperty(),
                          winrt::box_value(Thickness{1, 1, 1, 1})));
    setters.Append(Setter(wuxc::Control::CornerRadiusProperty(),
                          winrt::box_value(MakeCorner(kFlyoutCorner))));
    setters.Append(
        Setter(wuxc::Control::PaddingProperty(), winrt::box_value(Thickness{0, 0, 0, 0})));
    setters.Append(Setter(FrameworkElement::MinWidthProperty(), winrt::box_value(width)));
    setters.Append(Setter(FrameworkElement::MaxWidthProperty(), winrt::box_value(width)));
    setters.Append(Setter(wuxc::ScrollViewer::HorizontalScrollBarVisibilityProperty(),
                          winrt::box_value(wuxc::ScrollBarVisibility::Disabled)));
    if (auto tmpl = BuildFlyoutShellTemplate(false)) {
        setters.Append(Setter(wuxc::Control::TemplateProperty(), winrt::box_value(tmpl)));
    }
    return style;
}

// The panel body is wrapped in a ScrollViewer so a long network or tray list
// stays inside a sane height instead of running off the bottom of the screen.
wuxc::Flyout MakeControlFlyout(PCWSTR name, wuxc::StackPanel& contentOut) {
    wuxc::StackPanel panel;
    panel.Name(name);
    panel.Spacing(2);
    contentOut = panel;
    g_detachedStyleRoots.push_back(panel);  // Register panel so tree targets can find it

    wuxc::ScrollViewer scroller;
    scroller.Content(panel);
    scroller.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    scroller.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
    scroller.MaxHeight(560);

    wuxc::Border blurHost;
    blurHost.Name(L"FlyoutBlurHost");
    blurHost.CornerRadius(MakeCorner(kFlyoutCorner));
    blurHost.Padding(Thickness{12, 12, 12, 12});
    blurHost.HorizontalAlignment(HorizontalAlignment::Stretch);
    blurHost.VerticalAlignment(VerticalAlignment::Stretch);
    blurHost.Child(scroller);

    wuxc::Flyout flyout;
    flyout.Content(blurHost);
    flyout.Placement(wuxc::Primitives::FlyoutPlacementMode::Bottom);
    flyout.FlyoutPresenterStyle(MakeFlyoutPresenterStyle(kPanelWidth));

    // Without this the panel is clipped to the bar-height island. (Credit: the
    // user's fix.)
    flyout.ShouldConstrainToRootBounds(false);

    // When the flyout opens, register the topmost visual root (like FlyoutPresenter)
    // so targets like `FlyoutPresenter` or `FlyoutPresenter > Grid` also match.
    flyout.Opened([flyout](auto&&, auto&&) {
        try {
            auto content = flyout.Content().try_as<FrameworkElement>();
            if (content) {
                wuxc::FlyoutPresenter presenter{nullptr};
                DependencyObject current = content;
                while (auto parent = wuxm::VisualTreeHelper::GetParent(current)) {
                    if (auto p = parent.try_as<wuxc::FlyoutPresenter>()) {
                        presenter = p;
                        break;
                    }
                    current = parent;
                }

                if (presenter) {
                    if (auto blurHost = content.try_as<wuxc::Border>()) {
                        if (blurHost.Background() == nullptr) {
                            wui::Color tintColor;
                            if (!ParseBarColor(g_settings.topBarBackgroundColor,
                                               g_settings.topBarBackgroundOpacity,
                                               &tintColor)) {
                                tintColor = wui::ColorHelper::FromArgb(128, 0, 0, 0);
                            }
                            auto blurBrush = winrt::make<XamlBlurBrush>(
                                blurHost.as<UIElement>(),
                                static_cast<float>(g_settings.globalBlurAmount),
                                tintColor, tintColor.A);
                            blurHost.Background(blurBrush);
                        }
                    }
                    presenter.Background(MakeBrush(0, 0, 0, 0));

                    if (std::find(g_detachedStyleRoots.begin(),
                                  g_detachedStyleRoots.end(),
                                  presenter.as<FrameworkElement>()) ==
                        g_detachedStyleRoots.end()) {
                        g_detachedStyleRoots.push_back(
                            presenter.as<FrameworkElement>());
                    }

                    auto presenterRef = presenter;
                    auto clearShell = [presenterRef]() {
                        try {
                            auto transparent = MakeBrush(0, 0, 0, 0);
                            std::function<void(DependencyObject)> walk =
                                [&](DependencyObject node) {
                                    if (!node) return;
                                    if (auto cp =
                                            node.try_as<wuxc::ContentPresenter>()) {
                                        try { cp.Background(transparent); }
                                        catch (...) {}
                                        return;
                                    }
                                    if (auto panel = node.try_as<wuxc::Panel>()) {
                                        try { panel.Background(transparent); }
                                        catch (...) {}
                                    } else if (auto border =
                                                   node.try_as<wuxc::Border>()) {
                                        try { border.Background(transparent); }
                                        catch (...) {}
                                    } else if (auto ctrl =
                                                   node.try_as<wuxc::Control>()) {
                                        try { ctrl.Background(transparent); }
                                        catch (...) {}
                                    }
                                    int n = 0;
                                    try {
                                        n = wuxm::VisualTreeHelper::GetChildrenCount(node);
                                    } catch (...) { return; }
                                    for (int i = 0; i < n; i++) {
                                        try {
                                            walk(wuxm::VisualTreeHelper::GetChild(node, i));
                                        } catch (...) {}
                                    }
                                };
                            int n = 0;
                            try {
                                n = wuxm::VisualTreeHelper::GetChildrenCount(presenterRef);
                            } catch (...) { return; }
                            for (int i = 0; i < n; i++) {
                                try {
                                    walk(wuxm::VisualTreeHelper::GetChild(presenterRef, i));
                                } catch (...) {}
                            }
                        } catch (...) {}
                    };
                    if (g_uiDispatcherQueue) {
                        g_uiDispatcherQueue.TryEnqueue(clearShell);
                    } else {
                        clearShell();
                    }

                    auto isOurBlur = [](wf::IInspectable const& obj) {
                        return obj &&
                               obj.try_as<wuxm::XamlCompositionBrushBase>() !=
                                   nullptr;
                    };
                    std::function<void(DependencyObject)> clearInner =
                        [&](DependencyObject node) {
                            if (auto panel = node.try_as<wuxc::Panel>()) {
                                if (panel.Background() &&
                                    !isOurBlur(panel.Background())) {
                                    panel.Background(MakeBrush(0, 0, 0, 0));
                                }
                            } else if (auto border = node.try_as<wuxc::Border>()) {
                                if (border.Background() &&
                                    !isOurBlur(border.Background())) {
                                    border.Background(MakeBrush(0, 0, 0, 0));
                                }
                            } else if (auto cp =
                                           node.try_as<wuxc::ContentPresenter>()) {
                                if (cp.Background() &&
                                    !isOurBlur(cp.Background())) {
                                    cp.Background(MakeBrush(0, 0, 0, 0));
                                }
                            } else if (auto ctrl = node.try_as<wuxc::Control>()) {
                                if (ctrl.Background() &&
                                    !isOurBlur(ctrl.Background())) {
                                    ctrl.Background(MakeBrush(0, 0, 0, 0));
                                }
                            }
                            if (node.try_as<wuxc::ScrollViewer>()) return;
                            int childCount =
                                wuxm::VisualTreeHelper::GetChildrenCount(node);
                            for (int i = 0; i < childCount; i++) {
                                clearInner(
                                    wuxm::VisualTreeHelper::GetChild(node, i));
                            }
                        };
                    int childCount =
                        wuxm::VisualTreeHelper::GetChildrenCount(presenter);
                    for (int i = 0; i < childCount; i++) {
                        clearInner(wuxm::VisualTreeHelper::GetChild(presenter, i));
                    }

                    auto root = presenter.as<FrameworkElement>();
                    if (std::find(g_detachedStyleRoots.begin(),
                                  g_detachedStyleRoots.end(),
                                  root) == g_detachedStyleRoots.end()) {
                        g_detachedStyleRoots.push_back(root);
                    }
                }
            }
            ApplyBlurToAllOpenPopups();
            ApplyAllControlStyles();
        } catch (...) {
        }
    });

    return flyout;
}

wuxc::TextBlock MakePanelTitle(std::wstring_view text) {
    auto title = MakeText(L"FlyoutTitle", text, 15, true);
    title.Margin(Thickness{4, 2, 4, 6});
    return title;
}

// A full-width row: [icon] [label (+ optional sublabel)] [trailing element].
// Every list entry in every panel is built from this so the columns line up
// across the Display, Sound, Wi-Fi, Bluetooth and tray panels.
wuxc::Grid MakeRowContent(FrameworkElement leading,
                          std::wstring_view primary,
                          std::wstring_view secondary,
                          FrameworkElement trailing) {
    wuxc::Grid grid;
    grid.ColumnSpacing(12);

    wuxc::ColumnDefinition leadingColumn;
    leadingColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(leadingColumn);

    wuxc::ColumnDefinition textColumn;
    textColumn.Width(GridLength{1, GridUnitType::Star});
    grid.ColumnDefinitions().Append(textColumn);

    wuxc::ColumnDefinition trailingColumn;
    trailingColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(trailingColumn);

    if (leading) {
        leading.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(leading, 0);
        grid.Children().Append(leading);
    }

    wuxc::StackPanel textStack;
    textStack.VerticalAlignment(VerticalAlignment::Center);
    textStack.Children().Append(MakeText(nullptr, primary, 13));
    if (!secondary.empty()) {
        auto sub = MakeText(nullptr, secondary, 11, false, 0.6);
        textStack.Children().Append(sub);
    }
    wuxc::Grid::SetColumn(textStack, 1);
    grid.Children().Append(textStack);

    if (trailing) {
        trailing.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(trailing, 2);
        grid.Children().Append(trailing);
    }
    return grid;
}

// Clickable list row. Same corner radius and padding everywhere, so Wi-Fi
// networks, Bluetooth devices, output devices and tray items are visually one
// family.
wuxc::Button MakeListRow(FrameworkElement leading,
                         std::wstring_view primary,
                         std::wstring_view secondary,
                         FrameworkElement trailing,
                         std::function<void()> onClick) {
    auto button = MakeGhostButton(L"FlyoutListRow", kRowCorner);
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{10, 8, 10, 8});
    button.Content(MakeRowContent(leading, primary, secondary, trailing));
    if (onClick) {
        button.Click([onClick = std::move(onClick)](auto&&, auto&&) { onClick(); });
    }
    return button;
}

// Slider row: icon on the left, slider filling the width, live percentage on
// the right. onChanged only fires for real user movement.
wuxc::Grid MakeSliderRow(FrameworkElement icon,
                         int value,
                         std::function<void(int)> onChanged,
                         wuxc::Slider* sliderOut = nullptr) {
    wuxc::Grid grid;
    grid.ColumnSpacing(10);

    wuxc::ColumnDefinition iconColumn;
    iconColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(iconColumn);

    wuxc::ColumnDefinition sliderColumn;
    sliderColumn.Width(GridLength{1, GridUnitType::Star});
    grid.ColumnDefinitions().Append(sliderColumn);

    wuxc::ColumnDefinition valueColumn;
    valueColumn.Width(GridLength{0, GridUnitType::Auto});
    grid.ColumnDefinitions().Append(valueColumn);

    if (icon) {
        icon.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(icon, 0);
        grid.Children().Append(icon);
    }

    auto readout = MakeText(nullptr, std::to_wstring(value), 12, false, 0.75);
    readout.MinWidth(30);
    readout.TextAlignment(TextAlignment::Right);

    wuxc::Slider slider;
    slider.Minimum(0);
    slider.Maximum(100);
    slider.Value(value);
    slider.StepFrequency(1);
    slider.VerticalAlignment(VerticalAlignment::Center);
    slider.Margin(Thickness{0, 0, 0, 0});
    // The default tooltip repeats what the readout already shows, and it draws
    // outside the island where it can't be clipped correctly.
    slider.ThumbToolTipValueConverter(nullptr);
    slider.ValueChanged([onChanged = std::move(onChanged), readout](auto&&, auto&& args) {
        int newValue = static_cast<int>(args.NewValue() + 0.5);
        readout.Text(winrt::hstring(std::to_wstring(newValue)));
        if (g_populatingPanel) {
            return;
        }
        if (onChanged) {
            onChanged(newValue);
        }
    });
    wuxc::Grid::SetColumn(slider, 1);
    grid.Children().Append(slider);

    wuxc::Grid::SetColumn(readout, 2);
    grid.Children().Append(readout);

    if (sliderOut) {
        *sliderOut = slider;
    }
    return grid;
}

// Header + collapsible body, hand-built: the system XAML used by islands has no
// Expander control.
struct Expander {
    wuxc::StackPanel root{nullptr};
    wuxc::StackPanel body{nullptr};
};

Expander MakeExpander(std::wstring_view title,
                      std::wstring_view subtitle,
                      bool* expandedFlag,
                      const std::function<void()>& onToggled) {
    Expander expander;

    wuxc::StackPanel root;
    root.Spacing(2);

    auto chevron = BuildVectorIcon(nullptr, L"",
                                   *expandedFlag ? icons::kChevronUp : icons::kChevronDown, 24,
                                   14, 1.8);

    auto header = MakeGhostButton(L"ExpanderHeader", kRowCorner);
    header.HorizontalAlignment(HorizontalAlignment::Stretch);
    header.Padding(Thickness{10, 8, 10, 8});
    header.Content(MakeRowContent(nullptr, title, subtitle, chevron));
    header.Click([expandedFlag, onToggled](auto&&, auto&&) {
        *expandedFlag = !*expandedFlag;
        if (onToggled) {
            onToggled();
        }
    });
    root.Children().Append(header);

    wuxc::StackPanel body;
    body.Spacing(2);
    body.Margin(Thickness{8, 2, 0, 2});
    body.Visibility(*expandedFlag ? Visibility::Visible : Visibility::Collapsed);
    root.Children().Append(body);

    expander.root = root;
    expander.body = body;
    return expander;
}

// ============================================================================
// Weather subsystem
// ============================================================================

namespace weather {

std::wstring DescribeCode(int code) {
    if (code == 0) return L"Clear sky";
    if (code == 1) return L"Mainly clear";
    if (code == 2) return L"Partly cloudy";
    if (code == 3) return L"Overcast";
    if (code == 45 || code == 48) return L"Fog";
    if (code >= 51 && code <= 57) return L"Drizzle";
    if (code >= 61 && code <= 67) return L"Rain";
    if (code >= 71 && code <= 77) return L"Snow";
    if (code >= 80 && code <= 82) return L"Rain showers";
    if (code >= 85 && code <= 86) return L"Snow showers";
    if (code >= 95) return L"Thunderstorm";
    return L"Weather";
}

std::wstring UrlEncode(const std::wstring& value) {
    // Convert to UTF-8 first, then percent-encode byte-by-byte, so non-ASCII
    // city names ("Kraków", "München") reach Open-Meteo intact.
    if (value.empty()) return L"";
    int needed = WideCharToMultiByte(CP_UTF8, 0, value.c_str(),
                                     static_cast<int>(value.size()),
                                     nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return L"";
    std::string utf8(needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.c_str(),
                        static_cast<int>(value.size()),
                        utf8.data(), needed, nullptr, nullptr);

    static const char kHex[] = "0123456789ABCDEF";
    std::wstring out;
    out.reserve(utf8.size() * 3);
    for (unsigned char b : utf8) {
        if ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') ||
            (b >= '0' && b <= '9') ||
            b == '-' || b == '_' || b == '.' || b == '~') {
            out.push_back(static_cast<wchar_t>(b));
        } else if (b == ' ') {
            out += L"%20";
        } else {
            wchar_t buf[4];
            buf[0] = L'%';
            buf[1] = kHex[(b >> 4) & 0xF];
            buf[2] = kHex[b & 0xF];
            buf[3] = 0;
            out += buf;
        }
    }
    return out;
}

// ----------------------------------------------------------------------------
// HTTP via WinHTTP
//
// WinRT's HttpClient is unusable in an unpackaged host process like this tool
// mod's explorer.exe: it silently fails because the process has no package
// identity / capabilities declared, and the failure is delivered as an
// exception inside the async completion rather than to the caller. WinHTTP is
// the plain Win32 HTTP stack and works in any process without ceremony.
// ----------------------------------------------------------------------------

std::string HttpGet(const std::wstring& url) {
    std::string body;

    HINTERNET session = WinHttpOpen(
        L"Windhawk-TopBar/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        Wh_Log(L"[Weather] WinHttpOpen failed (%lu)", GetLastError());
        return body;
    }

    URL_COMPONENTS urlComp{};
    urlComp.dwStructSize = sizeof(urlComp);
    wchar_t hostName[256]{};
    wchar_t urlPath[1024]{};
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = ARRAYSIZE(hostName);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = ARRAYSIZE(urlPath);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &urlComp)) {
        Wh_Log(L"[Weather] WinHttpCrackUrl failed (%lu) for %s",
               GetLastError(), url.c_str());
        WinHttpCloseHandle(session);
        return body;
    }

    HINTERNET connect = WinHttpConnect(session, hostName, urlComp.nPort, 0);
    if (!connect) {
        Wh_Log(L"[Weather] WinHttpConnect failed (%lu)", GetLastError());
        WinHttpCloseHandle(session);
        return body;
    }

    DWORD flags =
        (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET request = WinHttpOpenRequest(
        connect, L"GET", urlPath, nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!request) {
        Wh_Log(L"[Weather] WinHttpOpenRequest failed (%lu)", GetLastError());
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return body;
    }

    BOOL ok = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                 WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (ok) {
        ok = WinHttpReceiveResponse(request, nullptr);
    }
    if (ok) {
        DWORD avail = 0;
        do {
            avail = 0;
            if (!WinHttpQueryDataAvailable(request, &avail) || avail == 0) {
                break;
            }
            std::vector<char> buf(avail);
            DWORD read = 0;
            if (!WinHttpReadData(request, buf.data(), avail, &read)) {
                break;
            }
            body.append(buf.data(), read);
        } while (avail > 0);
    } else {
        Wh_Log(L"[Weather] HTTP request failed (%lu) url=%s", GetLastError(),
               url.c_str());
    }

    Wh_Log(L"[Weather] HTTP response: %d bytes from %s", (int)body.size(),
           url.c_str());

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);
    return body;
}

std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) {
        return L"";
    }
    int needed = MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
                                     static_cast<int>(utf8.size()), nullptr, 0);
    if (needed <= 0) {
        return L"";
    }
    std::wstring result(needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
                        static_cast<int>(utf8.size()), result.data(), needed);
    return result;
}

bool Geocode(const std::wstring& query,
             std::wstring* outName,
             std::wstring* outLat,
             std::wstring* outLon) {
    std::wstring url =
        L"https://geocoding-api.open-meteo.com/v1/search?name=" +
        UrlEncode(query) + L"&count=1&language=en&format=json";
    std::string body = HttpGet(url);
    if (body.empty()) {
        Wh_Log(L"[Weather] Geocode: empty response for '%s'",
               query.c_str());
        return false;
    }
    try {
        using namespace winrt::Windows::Data::Json;
        auto json = JsonObject::Parse(winrt::hstring(Utf8ToWide(body)));
        auto results = json.GetNamedArray(L"results", nullptr);
        if (!results || results.Size() == 0) {
            Wh_Log(L"[Weather] Geocode: no results for '%s'",
                   query.c_str());
            return false;
        }
        auto first = results.GetObjectAt(0);
        *outName = std::wstring(first.GetNamedString(L"name", L""));
        wchar_t latBuf[32];
        wchar_t lonBuf[32];
        swprintf_s(latBuf, L"%.4f", first.GetNamedNumber(L"latitude", 0));
        swprintf_s(lonBuf, L"%.4f", first.GetNamedNumber(L"longitude", 0));
        *outLat = latBuf;
        *outLon = lonBuf;
        return true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"[Weather] Geocode JSON parse failed: %s",
               ex.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"[Weather] Geocode failed with unknown error");
        return false;
    }
}

void Fetch(double lat, double lon, WeatherState* out) {
    wchar_t url[768];
    swprintf_s(url,
               L"https://api.open-meteo.com/v1/forecast?"
               L"latitude=%.4f&longitude=%.4f&"
               L"current=temperature_2m,weather_code&"
               L"hourly=temperature_2m,weather_code,precipitation_probability&"
               L"daily=temperature_2m_max,temperature_2m_min&"
               L"timezone=auto&forecast_days=2",
               lat, lon);
    std::string body = HttpGet(url);
    if (body.empty()) {
        Wh_Log(L"[Weather] Fetch: empty response");
        out->valid = false;
        return;
    }
    try {
        using namespace winrt::Windows::Data::Json;
        auto json = JsonObject::Parse(winrt::hstring(Utf8ToWide(body)));

        if (auto current = json.GetNamedObject(L"current", nullptr)) {
            out->temperature = current.GetNamedNumber(L"temperature_2m", 0);
            out->weatherCode =
                static_cast<int>(current.GetNamedNumber(L"weather_code", 0));
        }

        if (auto daily = json.GetNamedObject(L"daily", nullptr)) {
            auto maxs = daily.GetNamedArray(L"temperature_2m_max", nullptr);
            auto mins = daily.GetNamedArray(L"temperature_2m_min", nullptr);
            if (maxs && maxs.Size() > 0) out->tempMax = maxs.GetNumberAt(0);
            if (mins && mins.Size() > 0) out->tempMin = mins.GetNumberAt(0);
        }

        if (auto hourly = json.GetNamedObject(L"hourly", nullptr)) {
            auto times = hourly.GetNamedArray(L"time", nullptr);
            auto temps = hourly.GetNamedArray(L"temperature_2m", nullptr);
            auto codes = hourly.GetNamedArray(L"weather_code", nullptr);
            auto rain = hourly.GetNamedArray(L"precipitation_probability", nullptr);
            if (times && temps && codes) {
                out->hourlyTimes.clear();
                out->hourlyTemps.clear();
                out->hourlyCodes.clear();
                out->hourlyRain.clear();
                uint32_t limit = (std::min)(times.Size(), 24u);
                for (uint32_t i = 0; i < limit; i++) {
                    std::wstring t{times.GetStringAt(i)};
                    if (t.size() >= 16) {
                        t = t.substr(11, 5);
                    }
                    out->hourlyTimes.push_back(t);
                    out->hourlyTemps.emplace_back(t, temps.GetNumberAt(i));
                    out->hourlyCodes.push_back(
                        static_cast<int>(codes.GetNumberAt(i)));
                    if (rain && i < rain.Size()) {
                        out->hourlyRain.push_back(
                            static_cast<int>(rain.GetNumberAt(i)));
                    } else {
                        out->hourlyRain.push_back(0);
                    }
                }
            }
        }
        out->valid = true;
        out->lastFetchTick = GetTickCount64();
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"[Weather] Fetch JSON parse failed: %s",
               ex.message().c_str());
        out->valid = false;
    } catch (...) {
        Wh_Log(L"[Weather] Fetch failed with unknown error");
        out->valid = false;
    }
}

void EnsureFresh() {
    // Must be called on the UI thread. Reads g_settings by value before
    // dispatching, and only writes g_weatherState via RunOnUiThread, so
    // the worker never races LoadSettings() or the UI reader.
    std::wstring latStr = g_settings.weatherLatitude;
    std::wstring lonStr = g_settings.weatherLongitude;
    std::wstring locName = g_settings.weatherLocationName;
    if (latStr.empty() || lonStr.empty()) return;

    double lat = _wtof(latStr.c_str());
    double lon = _wtof(lonStr.c_str());
    RunInBackground([lat, lon, locName] {
        WeatherState fresh;
        Fetch(lat, lon, &fresh);
        fresh.locationName = locName;
        RunOnUiThread([fresh = std::move(fresh)]() mutable {
            g_weatherState = std::move(fresh);
            RefreshWeatherButtonContent();
            PopulateWeatherPanel();
        });
    });
}

std::vector<WeatherSearchResult> GeocodeMulti(const std::wstring& query) {
    std::vector<WeatherSearchResult> out;
    std::wstring url =
        L"https://geocoding-api.open-meteo.com/v1/search?name=" +
        UrlEncode(query) + L"&count=15&language=en&format=json";
    Wh_Log(L"[Weather] GeocodeMulti query='%s' url='%s'", query.c_str(),
           url.c_str());
    std::string body = HttpGet(url);
    if (body.empty()) {
        return out;
    }
    try {
        using namespace winrt::Windows::Data::Json;
        auto json = JsonObject::Parse(winrt::hstring(Utf8ToWide(body)));
        auto results = json.GetNamedArray(L"results", nullptr);
        if (!results) {
            return out;
        }
        for (uint32_t i = 0; i < results.Size(); i++) {
            auto obj = results.GetObjectAt(i);
            WeatherSearchResult city;
            city.name = std::wstring(obj.GetNamedString(L"name", L""));
            city.admin1 = std::wstring(obj.GetNamedString(L"admin1", L""));
            city.country = std::wstring(obj.GetNamedString(L"country", L""));
            wchar_t latBuf[32];
            wchar_t lonBuf[32];
            swprintf_s(latBuf, L"%.4f", obj.GetNamedNumber(L"latitude", 0));
            swprintf_s(lonBuf, L"%.4f", obj.GetNamedNumber(L"longitude", 0));
            city.lat = latBuf;
            city.lon = lonBuf;
            if (!city.name.empty()) {
                out.push_back(std::move(city));
            }
        }
    } catch (...) {
    }
    return out;
}

std::wstring FormatHour12(const std::wstring& time24) {
    if (time24.size() < 5) return time24;
    int hour = _wtoi(time24.substr(0, 2).c_str());
    const wchar_t* ampm = (hour < 12) ? L"AM" : L"PM";
    int h12 = hour % 12;
    if (h12 == 0) h12 = 12;
    wchar_t buf[16];
    swprintf_s(buf, L"%d %s", h12, ampm);
    return buf;
}

FrameworkElement BuildWeatherIcon(double size, int code) {
    // Clear / mainly clear: sun only.
    if (code == 0 || code == 1) {
        return BuildVectorIcon(nullptr, icons::kWeatherSunFill,
                               icons::kWeatherSunRays, 24, size, 1.5);
    }
    // Partly cloudy / overcast / fog: plain cloud. Avoiding the sun-behind-
    // cloud overlay keeps the small size legible; the two shapes overlapped
    // badly at 20 DIP.
    if (code == 2 || code == 3 || code == 45 || code == 48) {
        return BuildVectorIcon(nullptr, icons::kWeatherCloudFill,
                               L"", 24, size, 1.5);
    }
    // Drizzle / rain / rain showers.
    if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) {
        return BuildVectorIcon(nullptr, icons::kWeatherSmallCloudFill,
                               icons::kWeatherRainDrops, 24, size, 1.5);
    }
    // Snow / snow showers.
    if ((code >= 71 && code <= 77) || (code >= 85 && code <= 86)) {
        return BuildVectorIcon(nullptr, icons::kWeatherSmallCloudFill,
                               icons::kWeatherSnowFlakes, 24, size, 1.5);
    }
    // Thunderstorm.
    if (code >= 95) {
        return BuildVectorIcon(nullptr, icons::kWeatherSmallCloudFill,
                               icons::kWeatherBolt, 24, size, 1.5);
    }
    return BuildVectorIcon(nullptr, icons::kWeatherCloudFill,
                           L"", 24, size, 1.5);
}

}  // namespace weather

// ============================================================================
// Recycle bin subsystem
// ============================================================================

namespace recyclebin {

void Refresh() {
    try {
        SHQUERYRBINFO info{};
        info.cbSize = sizeof(info);
        if (SUCCEEDED(SHQueryRecycleBin(nullptr, &info))) {
            g_recycleBinState.sizeBytes =
                static_cast<uint64_t>(info.i64Size);
            g_recycleBinState.itemCount =
                static_cast<uint64_t>(info.i64NumItems);
            g_recycleBinState.valid = true;
        }
    } catch (...) {
    }
}

std::wstring FormatBytes(uint64_t bytes) {
    const wchar_t* units[] = {L"B", L"KB", L"MB", L"GB", L"TB"};
    double value = static_cast<double>(bytes);
    int unit = 0;
    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        unit++;
    }
    wchar_t buf[64];
    swprintf_s(buf, L"%.1f %s", value, units[unit]);
    return buf;
}

bool Empty() {
    HRESULT hr = SHEmptyRecycleBin(
        nullptr, nullptr,
        SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
    return SUCCEEDED(hr);
}

}  // namespace recyclebin

wuxc::TextBlock MakeStatusText(std::wstring_view text) {
    auto block = MakeText(nullptr, text, 12, false, 0.6);
    block.Margin(Thickness{10, 8, 10, 8});
    block.TextWrapping(TextWrapping::Wrap);
    return block;
}

// A plain "open the real Settings page" footer link. Only used where Windows
// genuinely owns the setting; the panels no longer punt the actual controls to
// Settings.
wuxc::Button MakeSettingsLink(std::wstring_view label, PCWSTR uri) {
    auto button = MakeGhostButton(L"FlyoutFooterLink", kRowCorner);
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{10, 7, 10, 7});
    auto text = MakeText(nullptr, label, 12, false, 0.75);
    button.Content(text);
    std::wstring target = uri;
    button.Click([target](auto&&, auto&&) {
        ShellExecute(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    });
    return button;
}

// ============================================================================
// Display panel
// ============================================================================

void PopulateDisplayPanel() {
    if (!g_displayPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_displayPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"Display"));

    if (brightness::Available()) {
        auto icon = BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6);
        children.Append(MakeSliderRow(icon, brightness::Get(),
                                      [](int value) { SetBrightnessCoalesced(value); }));
    } else {
        children.Append(MakeStatusText(L"Brightness control isn't available on this display."));
    }

    children.Append(MakeDivider());

    bool darkMode = IsAppsDarkMode();

    auto darkTile = MakeGhostButton(L"QuickToggleTile", kTileCorner);
    darkTile.HorizontalAlignment(HorizontalAlignment::Stretch);
    darkTile.Padding(Thickness{8, 6, 8, 6});
    darkTile.Background(darkMode ? MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));

    wuxc::StackPanel darkStack;
    darkStack.Orientation(wuxc::Orientation::Horizontal);
    darkStack.Spacing(8);
    if (auto icon = BuildVectorIcon(nullptr, icons::kMoonFill, L"", 24, 16, 1.7, L"#FFFFFF")) {
        icon.VerticalAlignment(VerticalAlignment::Center);
        darkStack.Children().Append(icon);
    }
    darkStack.Children().Append(MakeText(nullptr, L"Dark mode", 12));
    darkTile.Content(darkStack);
    darkTile.Click([darkMode](auto&&, auto&&) {
        SetAppsDarkMode(!darkMode);
        RepopulateLater(PopulateDisplayPanel);
    });

    // Put them side-by-side (2 columns)
    children.Append(darkTile);

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Display settings", L"ms-settings:display"));
}

// ============================================================================
// Sound panel
// ============================================================================

FrameworkElement BuildSpeakerIcon(double size, int volume, bool muted) {
    std::wstring stroke;
    if (muted || volume == 0) {
        stroke = icons::kSpeakerMuted;
    } else if (volume < 55) {
        // One arc below about half volume, two above -- the same visual cue the
        // system tray icon uses.
        stroke = L"M15.4 9.4 A3.6 3.6 0 0 1 15.4 14.6";
    } else {
        stroke = icons::kSpeakerWaves;
    }
    return BuildVectorIcon(nullptr, icons::kSpeakerFill, stroke, 24, size, 1.6);
}

void RefreshSoundButtonIcon() {
    if (!g_soundButton) {
        return;
    }
    if (auto icon = BuildSpeakerIcon(18, audio::GetMasterVolume(), audio::GetMasterMute())) {
        g_soundButton.Content(icon);
    }
}

// ----------------------------------------------------------------------------
// Now-playing card
//
// Every call into the media session manager is asynchronous and has to be
// awaited, which cannot be done from the UI thread without deadlocking the
// apartment. So the state is fetched on a worker and pushed back through the
// dispatcher, and the transport buttons do the same in reverse.
// ----------------------------------------------------------------------------

[[clang::no_destroy]] wuxc::StackPanel g_mediaContainer{nullptr};

#if TOPBAR_HAS_MEDIA_CONTROL

namespace media {

using namespace winrt::Windows::Media::Control;

struct Snapshot {
    bool valid = false;
    std::wstring title;
    std::wstring artist;
    bool playing = false;
    bool canGoPrevious = false;
    bool canGoNext = false;
};

Snapshot Read() {
    Snapshot snapshot;
    try {
        auto manager =
            GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        if (!manager) {
            return snapshot;
        }
        auto session = manager.GetCurrentSession();
        if (!session) {
            return snapshot;
        }

        auto properties = session.TryGetMediaPropertiesAsync().get();
        if (properties) {
            snapshot.title = properties.Title();
            snapshot.artist = properties.Artist();
        }

        auto info = session.GetPlaybackInfo();
        if (info) {
            snapshot.playing = info.PlaybackStatus() ==
                               GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            if (auto controls = info.Controls()) {
                snapshot.canGoPrevious = controls.IsPreviousEnabled();
                snapshot.canGoNext = controls.IsNextEnabled();
            }
        }

        snapshot.valid = !snapshot.title.empty();
    } catch (...) {
    }
    return snapshot;
}

enum class Command { PlayPause, Previous, Next };

void Send(Command command) {
    RunInBackground([command] {
        try {
            auto manager =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (!manager) {
                return;
            }
            auto session = manager.GetCurrentSession();
            if (!session) {
                return;
            }
            switch (command) {
                case Command::PlayPause:
                    session.TryTogglePlayPauseAsync().get();
                    break;
                case Command::Previous:
                    session.TrySkipPreviousAsync().get();
                    break;
                case Command::Next:
                    session.TrySkipNextAsync().get();
                    break;
            }
        } catch (...) {
        }
    });
}

}  // namespace media

wuxc::Button MakeTransportButton(std::wstring_view glyph, bool enabled,
                                 std::function<void()> onClick) {
    auto button = MakeGhostButton(L"MediaTransportButton", kTileCorner);
    button.Padding(Thickness{8, 6, 8, 6});
    button.IsEnabled(enabled);
    button.Content(BuildVectorIcon(nullptr, glyph, L"", 24, 16));
    if (onClick) {
        button.Click([onClick = std::move(onClick)](auto&&, auto&&) { onClick(); });
    }
    return button;
}

void FillMediaCard(const media::Snapshot& snapshot) {
    if (!g_mediaContainer) {
        return;
    }
    auto children = g_mediaContainer.Children();
    children.Clear();

    if (!snapshot.valid) {
        g_mediaContainer.Visibility(Visibility::Collapsed);
        return;
    }
    g_mediaContainer.Visibility(Visibility::Visible);

    wuxc::StackPanel transport;
    transport.Orientation(wuxc::Orientation::Horizontal);
    transport.Spacing(2);
    transport.Children().Append(MakeTransportButton(
        icons::kPrevFill, snapshot.canGoPrevious,
        [] { media::Send(media::Command::Previous); }));
    transport.Children().Append(MakeTransportButton(
        snapshot.playing ? icons::kPauseFill : icons::kPlayFill, true, [] {
            media::Send(media::Command::PlayPause);
            // The status flips a moment after the command lands.
            RunInBackground([] {
                Sleep(400);
                auto refreshed = media::Read();
                RunOnUiThread([refreshed] { FillMediaCard(refreshed); });
            });
        }));
    transport.Children().Append(MakeTransportButton(
        icons::kNextFill, snapshot.canGoNext, [] { media::Send(media::Command::Next); }));

    auto icon = BuildVectorIcon(nullptr, icons::kHeadphoneFill, icons::kHeadphoneStroke, 24,
                                18, 1.6);
    auto row = MakeRowContent(icon, snapshot.title, snapshot.artist, transport);
    row.Margin(Thickness{10, 4, 6, 4});
    children.Append(row);
    
}

void RefreshMediaCard() {
    RunInBackground([] {
        auto snapshot = media::Read();
        RunOnUiThread([snapshot] { FillMediaCard(snapshot); });
    });
}

#else

void RefreshMediaCard() {}

#endif  // TOPBAR_HAS_MEDIA_CONTROL

void PopulateSoundPanel() {
    if (!g_soundPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_soundPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"Sound"));

    // Starts collapsed and empty; the worker fills it in and reveals it only if
    // something is actually playing.
    g_mediaContainer = wuxc::StackPanel();
    g_mediaContainer.Name(L"MediaCard");
    g_mediaContainer.Visibility(Visibility::Collapsed);
    children.Append(g_mediaContainer);
    RefreshMediaCard();

    int masterVolume = audio::GetMasterVolume();
    bool masterMuted = audio::GetMasterMute();

    // The speaker glyph doubles as the mute button.
    auto muteButton = MakeGhostButton(L"SoundMuteButton", kTileCorner);
    muteButton.Padding(Thickness{6, 6, 6, 6});
    muteButton.Content(BuildSpeakerIcon(18, masterVolume, masterMuted));
    muteButton.Click([masterMuted](auto&&, auto&&) {
        if (auto volume = audio::EndpointVolume()) {
            volume->SetMute(masterMuted ? FALSE : TRUE, nullptr);
        }
        RefreshSoundButtonIcon();
        RepopulateLater(PopulateSoundPanel);
    });

    children.Append(MakeSliderRow(muteButton, masterVolume, [](int value) {
        SetMasterVolumeCoalesced(value);
        RunOnUiThread([] { RefreshSoundButtonIcon(); });
    }));

    children.Append(MakeDivider());

    // Output devices: an expander showing the current default, listing every
    // active endpoint when opened. Tapping one switches to it -- the old build
    // sent the user to the Settings app instead.
    {
        auto devices = audio::EnumerateOutputDevices();
        std::wstring currentName = L"No output device";
        for (const auto& device : devices) {
            if (device.isDefault) {
                currentName = device.name;
                break;
            }
        }

        auto expander = MakeExpander(L"Output device", currentName, &g_outputDevicesExpanded,
                                     [] { RepopulateLater(PopulateSoundPanel); });

        if (devices.empty()) {
            expander.body.Children().Append(MakeStatusText(L"No output devices found."));
        }
        for (const auto& device : devices) {
            FrameworkElement check{nullptr};
            if (device.isDefault) {
                check = BuildVectorIcon(nullptr, L"", icons::kCheckStroke, 24, 15, 2.0);
            }
            auto icon = BuildVectorIcon(nullptr, icons::kHeadphoneFill,
                                        icons::kHeadphoneStroke, 24, 17, 1.6);
            std::wstring deviceId = device.id;
            expander.body.Children().Append(
                MakeListRow(icon, device.name, L"", check, [deviceId] {
                    if (!audio::SetDefaultOutputDevice(deviceId)) {
                        Wh_Log(L"Failed to set the default output device");
                    }
                    RefreshSoundButtonIcon();
                    RepopulateLater(PopulateSoundPanel);
                }));
        }
        children.Append(expander.root);
    }

    // Per-app mixer, collapsed into one expandable section as requested.
    // (Spatial Audio has been dropped entirely.)
    {
        auto sessions = audio::EnumerateSessions();
        std::wstring subtitle =
            sessions.empty()
                ? std::wstring(L"Nothing is playing")
                : std::to_wstring(sessions.size()) +
                      (sessions.size() == 1 ? L" app" : L" apps");

        auto expander = MakeExpander(L"App volume mixer", subtitle, &g_appMixerExpanded,
                                     [] { RepopulateLater(PopulateSoundPanel); });

        if (sessions.empty()) {
            expander.body.Children().Append(
                MakeStatusText(L"No apps are currently playing audio."));
        }
        for (const auto& session : sessions) {
            wuxc::StackPanel row;
            row.Spacing(0);
            row.Margin(Thickness{10, 4, 10, 4});
            row.Children().Append(MakeText(nullptr, session.name, 12, false, 0.85));

            auto icon = BuildVectorIcon(
                nullptr, session.isSystemSounds ? icons::kSpeakerFill : icons::kAppFill, L"",
                24, 15, 1.5);

            // The control interface is captured so the slider writes straight
            // to that session without re-enumerating on every drag.
            auto control = session.control;
            row.Children().Append(MakeSliderRow(icon, session.volume, [control](int value) {
                if (control) {
                    control->SetMasterVolume(std::clamp(value, 0, 100) / 100.0f, nullptr);
                    if (value > 0) {
                        control->SetMute(FALSE, nullptr);
                    }
                }
            }));
            expander.body.Children().Append(row);
        }
        children.Append(expander.root);
    }

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Sound settings", L"ms-settings:sound"));
}

// ============================================================================
// Wi-Fi panel
// ============================================================================

// Four-bar signal glyph, drawn by dropping arcs off the top of the full icon.
FrameworkElement BuildWifiIcon(double size, int signal, bool connected) {
    (void)signal; // unused now
    (void)connected; // unused now

    // Corrected path (scientific notation replaced with decimal)
    std::wstring wifiPath =
        L"M61.5917 17.7222C79.4636 17.723 96.6521 24.3489 109.605 36.2305C110.58 37.1478 112.139 37.1362 113.1 36.2045"
        L"L122.423 27.1255C122.909 26.653 123.181 26.0129 123.177 25.3469C123.173 24.6809 122.894 24.0439 122.402 23.5769"
        L"C88.4054 -7.85896 34.7726 -7.85896 0.776469 23.5769C0.284012 24.0436 0.00459724 24.6804 0.0000562216 25.3463"
        L"C-0.00448479 26.0123 0.266222 26.6526 0.752273 27.1255L10.0785 36.2045C11.0385 37.1376 12.5987 37.1492 13.5734 36.2305"
        L"C26.5276 24.3481 43.7181 17.7222 61.5917 17.7222Z"
        L"M61.5676 48.0483C71.321 48.0477 80.7264 51.7249 87.9562 58.3656C88.934 59.308 90.4744 59.2876 91.4276 58.3195"
        L"L100.678 48.8392C101.165 48.342 101.435 47.6674 101.428 46.9664C101.421 46.2654 101.137 45.5965 100.64 45.1094"
        L"C78.6243 24.3363 44.5296 24.3363 22.5135 45.1094C22.0162 45.5965 21.7324 46.2657 21.7259 46.9669"
        L"C21.7194 47.6681 21.9906 48.3427 22.4788 48.8392L31.7262 58.3195C32.6795 59.2876 34.2198 59.308 35.1977 58.3656"
        L"C42.4227 51.7293 51.8206 48.0524 61.5676 48.0483Z"
        L"M79.7076 68.1222C79.7214 68.8792 79.4551 69.6091 78.9715 70.1395L63.3304 87.7786C62.8719 88.297 62.2468 88.5888 61.5946 88.5888"
        L"C60.9423 88.5888 60.3172 88.297 59.8587 87.7786L44.215 70.1395C43.7317 69.6087 43.4659 68.8786 43.4802 68.1216"
        L"C43.4946 67.3645 43.7878 66.6477 44.2907 66.1402C54.2797 56.6989 68.9094 56.6989 78.8984 66.1402"
        L"C79.401 66.6481 79.6938 67.3652 79.7076 68.1222Z";

    std::wstring brush = GetEffectiveIconColorString();

    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Stretch=\"Uniform\" Width=\"" + std::to_wstring(size) + L"\" Height=\"" + std::to_wstring(size) + L"\">"
        L"<Grid Width=\"124\" Height=\"89\">"
        L"<Path Data=\"" + EscapeXmlAttr(wifiPath) + L"\" Fill=\"" + brush + L"\"/>"
        L"</Grid></Viewbox>";

    try {
        auto element = Markup::XamlReader::Load(xaml).as<FrameworkElement>();
        element.Name(L"WifiIcon");
        return element;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to build wifi icon: %08X", static_cast<unsigned int>(ex.code().value));
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

void RefreshWifiButtonIcon() {
    if (!g_wifiButton) {
        return;
    }
    auto status = wifi::GetStatus();
    if (auto icon = BuildWifiIcon(18, status.signal, status.connected && status.radioOn)) {
        g_wifiButton.Content(icon);
    }
}

// Kicks off a scan and re-reads the list a moment later. WlanScan is
// asynchronous, so reading the list immediately would return the same stale
// results the user is complaining about.
void StartWifiScan() {
    if (g_wifiScanning) {
        return;
    }
    g_wifiScanning = true;
    RunInBackground([] {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
        wifi::RequestScan();
        // The driver reports results over the next few seconds; this is the
        // interval Windows' own flyout waits before redrawing.
        WaitForSingleObject(g_stopEvent, 4000);
        auto networks = wifi::EnumerateNetworks();
        RunOnUiThread([networks = std::move(networks)]() mutable {
            g_wifiNetworks = std::move(networks);
            g_wifiScanning = false;
            PopulateWifiPanel();
        });
    });
}

void ConnectToWifi(const wifi::Network& network, const std::wstring& password) {
    RunInBackground([network, password] {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
        bool ok = wifi::Connect(network, password);
        // Association takes a moment; re-read afterwards so the row shows the
        // real outcome rather than an optimistic "Connected".
        WaitForSingleObject(g_stopEvent, ok ? 2500 : 300);

        auto networks = wifi::EnumerateNetworks();
        bool connected = false;
        for (const auto& candidate : networks) {
            if (candidate.ssid == network.ssid && candidate.connected) {
                connected = true;
                break;
            }
        }

        if (!connected && !network.hasProfile) {
            wifi::ForgetProfile(network.ssid);
        }

        RunOnUiThread([networks = std::move(networks), connected, network]() mutable {
            g_wifiNetworks = std::move(networks);
            g_wifiPasswordPrompt = false;
            g_wifiConnectingSSID.clear();
            if (!connected) {
                g_wifiPromptError = L"Couldn't connect to " + network.ssid + L".";
            }
            RefreshWifiButtonIcon();
            PopulateWifiPanel();
        });
    });
}

void BuildWifiPasswordView(const wf::Collections::IVector<UIElement>& children) {
    children.Append(MakePanelTitle(L"Connect to " + g_wifiPromptNetwork.ssid));

    if (!g_wifiPromptError.empty()) {
        auto error = MakeText(nullptr, g_wifiPromptError, 12, false, 0.9);
        error.Foreground(MakeBrush(0xFF, 0xFF, 0x8A, 0x80));
        error.TextWrapping(TextWrapping::Wrap);
        error.Margin(Thickness{10, 0, 10, 6});
        children.Append(error);
    }

    children.Append(MakeStatusText(L"Enter the network security key."));

    wuxc::PasswordBox passwordBox;
    passwordBox.Name(L"WifiPasswordBox");
    passwordBox.PlaceholderText(L"Password");
    passwordBox.Margin(Thickness{10, 0, 10, 8});
    passwordBox.CornerRadius(MakeCorner(kRowCorner));
    passwordBox.Background(MakeBrush(0xFF, 0x30, 0x30, 0x30)); // Dark background
    passwordBox.Foreground(MakeBrush(0xFF, 0xFF, 0xFF, 0xFF)); // White text
    children.Append(passwordBox);

    auto submit = [passwordBox] {
        std::wstring password{passwordBox.Password()};
        g_wifiPromptError.clear();
        ConnectToWifi(g_wifiPromptNetwork, password);
    };

    // Enter should connect, same as the system flyout.
    passwordBox.KeyDown([submit](auto&&, Input::KeyRoutedEventArgs const& args) {
        if (args.Key() == winrt::Windows::System::VirtualKey::Enter) {
            args.Handled(true);
            submit();
        }
    });

    wuxc::Grid buttons;
    buttons.ColumnSpacing(8);
    buttons.Margin(Thickness{10, 0, 10, 4});
    for (int i = 0; i < 2; i++) {
        wuxc::ColumnDefinition column;
        column.Width(GridLength{1, GridUnitType::Star});
        buttons.ColumnDefinitions().Append(column);
    }

    auto connect = MakeGhostButton(L"WifiConnectButton", kRowCorner);
    connect.Background(MakeBrush(0xFF, 0x4C, 0x8E, 0xE0));
    connect.Padding(Thickness{12, 8, 12, 8});
    connect.HorizontalAlignment(HorizontalAlignment::Stretch);
    connect.HorizontalContentAlignment(HorizontalAlignment::Center);
    connect.Content(MakeText(nullptr, L"Connect", 12));
    connect.Click([submit](auto&&, auto&&) { submit(); });
    wuxc::Grid::SetColumn(connect, 0);
    buttons.Children().Append(connect);

    auto cancel = MakeGhostButton(L"WifiCancelButton", kRowCorner);
    cancel.Background(MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
    cancel.Padding(Thickness{12, 8, 12, 8});
    cancel.HorizontalAlignment(HorizontalAlignment::Stretch);
    cancel.HorizontalContentAlignment(HorizontalAlignment::Center);
    cancel.Content(MakeText(nullptr, L"Cancel", 12));
    cancel.Click([](auto&&, auto&&) {
        g_wifiPasswordPrompt = false;
        g_wifiPromptError.clear();
        RepopulateLater(PopulateWifiPanel);
    });
    wuxc::Grid::SetColumn(cancel, 1);
    buttons.Children().Append(cancel);

    children.Append(buttons);

    // Focus can only be taken once the box is actually in the tree.
    passwordBox.Loaded([passwordBox](auto&&, auto&&) {
        passwordBox.Focus(FocusState::Programmatic);
    });
}

void PopulateWifiPanel() {
    if (!g_wifiPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_wifiPanel.Children();
    children.Clear();

    if (g_wifiPasswordPrompt) {
        BuildWifiPasswordView(children);
        return;
    }

    auto status = wifi::GetStatus();

    // 2-Column Header: Left (Title + Spinner) | Right (Refresh + Toggle)
    wuxc::Grid headerGrid;
    headerGrid.Margin(Thickness{4, 2, 0, 6});
    headerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);

    // Column 0: Title + Spinner
    wuxc::ColumnDefinition leftColumn;
    leftColumn.Width(GridLength{1, GridUnitType::Star});
    headerGrid.ColumnDefinitions().Append(leftColumn);

    // Column 1: Right controls (auto width, pushed far right)
    wuxc::ColumnDefinition rightColumn;
    rightColumn.Width(GridLength{0, GridUnitType::Auto});
    headerGrid.ColumnDefinitions().Append(rightColumn);

    // Left stack: title + spinner
    wuxc::StackPanel leftStack;
    leftStack.Orientation(wuxc::Orientation::Horizontal);
    leftStack.Spacing(8);
    leftStack.VerticalAlignment(VerticalAlignment::Center);

    auto titleBlock = MakeText(L"FlyoutTitle", L"Wi-Fi", 15, true);
    leftStack.Children().Append(titleBlock);

    wuxc::ProgressRing wifiProgress;
    wifiProgress.Name(L"WifiProgressRing");
    wifiProgress.IsActive(true);
    wifiProgress.Width(14);
    wifiProgress.Height(14);
    wifiProgress.VerticalAlignment(VerticalAlignment::Center);
    wifiProgress.Foreground(MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B));
    wifiProgress.Opacity(g_wifiScanning ? 1.0 : 0.3);
    leftStack.Children().Append(wifiProgress);

    wuxc::Grid::SetColumn(leftStack, 0);
    headerGrid.Children().Append(leftStack);

    // Right stack: refresh button + toggle
    wuxc::StackPanel rightStack;
    rightStack.Orientation(wuxc::Orientation::Horizontal);
    rightStack.Spacing(4);
    rightStack.VerticalAlignment(VerticalAlignment::Center);

    // Refresh button (manual scan trigger)
    auto refreshButton = MakeGhostButton(L"WifiRefreshButton", kRowCorner);
    refreshButton.Padding(Thickness{6, 6, 6, 6});
    refreshButton.Content(BuildVectorIcon(nullptr, L"M12 20c-2.21665 0 -4.10415 -0.77915 -5.6625 -2.3375C4.779165 16.10415 4 14.21665 4 12c0 -2.21665 0.779165 -4.10415 2.3375 -5.6625C7.89585 4.779165 9.78335 4 12 4c1.41665 0 2.65835 0.2875 3.725 0.8625 1.06665 0.575 1.99165 1.3625 2.775 2.3625V4h1.5v6.35H13.65v-1.5h4.2c-0.63335 -1 -1.44165 -1.80835 -2.425 -2.425C14.44165 5.80835 13.3 5.5 12 5.5c-1.81665 0 -3.35415 0.62915 -4.6125 1.8875C6.12915 8.64585 5.5 10.18335 5.5 12c0 1.81665 0.62915 3.35415 1.8875 4.6125C8.64585 17.87085 10.18335 18.5 12 18.5c1.38335 0 2.65 -0.39585 3.8 -1.1875s1.95 -1.8375 2.4 -3.1375h1.55c-0.48335 1.75 -1.44165 3.15835 -2.875 4.225C15.44165 19.46665 13.81665 20 12 20Z", L"", 24, 20, 1.7));
    refreshButton.Click([](auto&&, auto&&) {
        StartWifiScan();
    });
    rightStack.Children().Append(refreshButton);

    // Toggle switch
    wuxc::ToggleSwitch toggle;
    toggle.Name(L"WifiHeaderToggle");
    g_namedElements.insert_or_assign(L"WifiHeaderToggle", toggle);
    toggle.OnContent(winrt::box_value(L""));
    toggle.OffContent(winrt::box_value(L""));
    toggle.MinWidth(50);
    toggle.Width(50);
    toggle.HorizontalAlignment(HorizontalAlignment::Right);
    toggle.HorizontalContentAlignment(HorizontalAlignment::Right);
    toggle.VerticalAlignment(VerticalAlignment::Center);
    toggle.Margin(Thickness{0, 0, 4, 0});
    toggle.IsOn(status.radioOn);
    toggle.Toggled([](auto&& sender, auto&&) {
        if (g_populatingPanel) return;
        bool on = sender.template as<wuxc::ToggleSwitch>().IsOn();
        RunInBackground([on] {
            wifi::SetRadio(on);
            WaitForSingleObject(g_stopEvent, 600);
            auto networks = on ? wifi::EnumerateNetworks() : std::vector<wifi::Network>{};
            RunOnUiThread([networks = std::move(networks)]() mutable {
                g_wifiNetworks = std::move(networks);
                RefreshWifiButtonIcon();
                PopulateWifiPanel();
            });
        });
    });
    rightStack.Children().Append(toggle);

    wuxc::Grid::SetColumn(rightStack, 1);
    headerGrid.Children().Append(rightStack);

    children.Append(headerGrid);

    if (!status.available) {
        children.Append(MakeStatusText(L"No Wi-Fi adapter was found on this PC."));
        return;
    }
    if (!status.radioOn) {
        children.Append(MakeStatusText(L"Wi-Fi is off."));
        return;
    }

    if (!g_wifiPromptError.empty()) {
        auto error = MakeText(nullptr, g_wifiPromptError, 12, false, 0.9);
        error.Foreground(MakeBrush(0xFF, 0xFF, 0x8A, 0x80));
        error.TextWrapping(TextWrapping::Wrap);
        error.Margin(Thickness{10, 0, 10, 4});
        children.Append(error);
    }

    if (g_wifiNetworks.empty()) {
        children.Append(MakeStatusText(g_wifiScanning ? L"Scanning for networks…"
                                                      : L"No networks found."));
    }

    for (const auto& network : g_wifiNetworks) {
        auto icon = BuildWifiIcon(18, network.signal, true);
        std::wstring subtitle;
        if (network.ssid == g_wifiConnectingSSID) {
            subtitle = L"Connecting...";
        } else if (network.connected) {
            subtitle = L"Connected";
        } else if (network.hasProfile) {
            subtitle = network.secured ? L"Saved, secured" : L"Saved";
        } else if (network.secured) {
            subtitle = L"Secured";
        } else {
            subtitle = L"Open";
        }

        FrameworkElement trailing{nullptr};
        if (network.ssid == g_wifiConnectingSSID) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 15, 1.7); // use a generic loading dot? or just no trailing
        } else if (network.connected) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kCheckStroke, 24, 15, 2.0);
        } else if (network.secured) {
            trailing = BuildVectorIcon(nullptr, icons::kLockFill, icons::kLockShackle, 24, 14, 1.5);
        }

        wifi::Network captured = network;
        children.Append(MakeListRow(icon, network.ssid, subtitle, trailing, [captured] {
            g_wifiPromptError.clear();
            if (captured.connected) {
                RunInBackground([] {
                    wifi::Disconnect();
                    WaitForSingleObject(g_stopEvent, 800);
                    auto networks = wifi::EnumerateNetworks();
                    RunOnUiThread([networks = std::move(networks)]() mutable {
                        g_wifiNetworks = std::move(networks);
                        RefreshWifiButtonIcon();
                        PopulateWifiPanel();
                    });
                });
                return;
            }
            if (captured.secured && !captured.hasProfile) {
                g_wifiPasswordPrompt = true;
                g_wifiPromptNetwork = captured;
                RepopulateLater(PopulateWifiPanel);
                return;
            }
            
            // Set connecting status
            g_wifiConnectingSSID = captured.ssid;
            RepopulateLater(PopulateWifiPanel); // Show "Connecting..."
            
            ConnectToWifi(captured, L"");
        }));
    }

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Network settings", L"ms-settings:network-wifi"));
    // Apply any styles for dynamically created elements (like the Wi-Fi toggle).
    ApplyAllControlStyles();
}

// ============================================================================
// Bluetooth panel
// ============================================================================

void RefreshBluetoothButtonIcon() {
    if (!g_bluetoothButton) {
        return;
    }
    if (auto icon = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 18, 1.8)) {
        icon.Opacity(bluetooth::IsRadioOn() ? 1.0 : 0.4);
        g_bluetoothButton.Content(icon);
    }
}

// Paired devices come back fast; unpaired ones need an inquiry that blocks for
// seconds, which is why "no new devices at the bottom" was the old behaviour --
// nothing ever asked for them.
void StartBluetoothScan(bool includeUnpaired) {
    if (g_bluetoothScanning) {
        Wh_Log(L"Bluetooth scan already in progress");
        return;
    }
    // Make sure the radio is actually on before scanning
    RefreshBluetoothRadioState();
    if (!bluetooth::IsRadioOn()) {
        Wh_Log(L"Bluetooth radio is off, cannot scan");
        return;
    }

    g_bluetoothScanning = true;
    Wh_Log(L"Starting Bluetooth scan (includeUnpaired=%d)", includeUnpaired ? 1 : 0);
    RunInBackground([includeUnpaired] {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            Wh_Log(L"Bluetooth scan cancelled (stop event)");
            g_bluetoothScanning = false;
            return;
        }
        auto devices = bluetooth::Enumerate(includeUnpaired);
        Wh_Log(L"Bluetooth scan found %d devices", (int)devices.size());
        RunOnUiThread([devices = std::move(devices)]() mutable {
            g_bluetoothDevices = std::move(devices);
            g_bluetoothScanning = false;
            PopulateBluetoothPanel();
        });
    });
}

void RefreshBluetoothRadioState() {
    RunInBackground([] {
        bool on = false;
#if TOPBAR_HAS_RADIOS
        try {
            using namespace winrt::Windows::Devices::Radios;
            auto access = Radio::RequestAccessAsync().get();
            if (access == RadioAccessStatus::Allowed) {
                auto radios = Radio::GetRadiosAsync().get();
                for (auto&& radio : radios) {
                    if (radio.Kind() == RadioKind::Bluetooth) {
                        on = (radio.State() == RadioState::On);
                        break;
                    }
                }
            }
        } catch (...) {}
#endif
        {
            std::lock_guard<std::mutex> lock(bluetooth::g_bluetoothRadioMutex);
            bluetooth::g_bluetoothRadioOn = on;
        }
        RunOnUiThread([] {
            RefreshBluetoothButtonIcon();
            if (g_bluetoothFlyout && g_bluetoothFlyout.IsOpen()) {
                PopulateBluetoothPanel();
            }
        });
    });
}

void PopulateBluetoothPanel() {
    if (!g_bluetoothPanel) {
        return;
    }

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_bluetoothPanel.Children();
    children.Clear();

    bool available = bluetooth::Available();
    bool radioOn = available && bluetooth::IsRadioOn();

    // 2-Column Header: Left (Title + Spinner) | Right (Toggle)
    wuxc::Grid headerGrid;
    headerGrid.Margin(Thickness{4, 2, 0, 6});
    headerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);

    // Column 0: Title + Spinner
    wuxc::ColumnDefinition leftColumn;
    leftColumn.Width(GridLength{1, GridUnitType::Star});
    headerGrid.ColumnDefinitions().Append(leftColumn);

    // Column 1: Toggle (auto width, pushed far right)
    wuxc::ColumnDefinition rightColumn;
    rightColumn.Width(GridLength{0, GridUnitType::Auto});
    headerGrid.ColumnDefinitions().Append(rightColumn);

    // Left stack: title + spinner
    wuxc::StackPanel leftStack;
    leftStack.Orientation(wuxc::Orientation::Horizontal);
    leftStack.Spacing(8);
    leftStack.VerticalAlignment(VerticalAlignment::Center);

    auto titleBlock = MakeText(L"FlyoutTitle", L"Bluetooth", 15, true);
    leftStack.Children().Append(titleBlock);

    wuxc::ProgressRing btProgress;
    btProgress.Name(L"BluetoothProgressRing");
    btProgress.IsActive(true);
    btProgress.Width(14);
    btProgress.Height(14);
    btProgress.VerticalAlignment(VerticalAlignment::Center);
    btProgress.Foreground(MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B));
    btProgress.Opacity(g_bluetoothScanning ? 1.0 : 0.3);
    leftStack.Children().Append(btProgress);

    wuxc::Grid::SetColumn(leftStack, 0);
    headerGrid.Children().Append(leftStack);

    // Right side: refresh button + toggle (stacked horizontally)
    wuxc::StackPanel rightStack;
    rightStack.Orientation(wuxc::Orientation::Horizontal);
    rightStack.Spacing(4);
    rightStack.VerticalAlignment(VerticalAlignment::Center);

    // Refresh button (manual scan trigger)
    auto refreshButton = MakeGhostButton(L"BluetoothRefreshButton", kRowCorner);
    refreshButton.Padding(Thickness{6, 6, 6, 6});
    refreshButton.Content(BuildVectorIcon(nullptr, L"M12 20c-2.21665 0 -4.10415 -0.77915 -5.6625 -2.3375C4.779165 16.10415 4 14.21665 4 12c0 -2.21665 0.779165 -4.10415 2.3375 -5.6625C7.89585 4.779165 9.78335 4 12 4c1.41665 0 2.65835 0.2875 3.725 0.8625 1.06665 0.575 1.99165 1.3625 2.775 2.3625V4h1.5v6.35H13.65v-1.5h4.2c-0.63335 -1 -1.44165 -1.80835 -2.425 -2.425C14.44165 5.80835 13.3 5.5 12 5.5c-1.81665 0 -3.35415 0.62915 -4.6125 1.8875C6.12915 8.64585 5.5 10.18335 5.5 12c0 1.81665 0.62915 3.35415 1.8875 4.6125C8.64585 17.87085 10.18335 18.5 12 18.5c1.38335 0 2.65 -0.39585 3.8 -1.1875s1.95 -1.8375 2.4 -3.1375h1.55c-0.48335 1.75 -1.44165 3.15835 -2.875 4.225C15.44165 19.46665 13.81665 20 12 20Z", L"", 24, 20, 1.7));
    refreshButton.Click([](auto&&, auto&&) {
        StartBluetoothScan(true);
    });
    rightStack.Children().Append(refreshButton);

    // Toggle switch
    wuxc::ToggleSwitch toggle;
    toggle.Name(L"BluetoothHeaderToggle");
    g_namedElements.insert_or_assign(L"BluetoothHeaderToggle", toggle);
    toggle.OnContent(winrt::box_value(L""));
    toggle.OffContent(winrt::box_value(L""));
    toggle.MinWidth(50);
    toggle.Width(50);
    toggle.HorizontalAlignment(HorizontalAlignment::Right);
    toggle.HorizontalContentAlignment(HorizontalAlignment::Right);
    toggle.VerticalAlignment(VerticalAlignment::Center);
    toggle.Margin(Thickness{0, 0, 4, 0});
    toggle.IsOn(radioOn);
    toggle.IsEnabled(true);
    toggle.Toggled([](auto&& sender, auto&&) {
        if (g_populatingPanel) return;
        bool on = sender.template as<wuxc::ToggleSwitch>().IsOn();
        RunInBackground([on] {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return;
            bluetooth::SetRadio(on);
            WaitForSingleObject(g_stopEvent, 600);
            auto devices = on ? bluetooth::Enumerate(false)
                              : std::vector<bluetooth::Device>{};
            RunOnUiThread([devices = std::move(devices)]() mutable {
                g_bluetoothDevices = std::move(devices);
                RefreshBluetoothButtonIcon();
                PopulateBluetoothPanel();
                RefreshBluetoothRadioState();
            });
        });
    });
    rightStack.Children().Append(toggle);

    wuxc::Grid::SetColumn(rightStack, 1);
    headerGrid.Children().Append(rightStack);

    children.Append(headerGrid);

    if (!available) {
        children.Append(MakeStatusText(L"No Bluetooth radio was found on this PC."));
        return;
    }
    if (!radioOn) {
        children.Append(MakeStatusText(L"Bluetooth is off."));
        return;
    }





    if (g_bluetoothDevices.empty()) {
        children.Append(MakeStatusText(L"No paired devices."));
    }

    for (const auto& device : g_bluetoothDevices) {
        auto icon = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 17, 1.7);
        if (icon && !device.connected) {
            icon.Opacity(0.55);
        }

        std::wstring subtitle;
        // Check if this device is currently pairing
        if (device.address.ullLong == g_pairingDeviceAddress) {
            subtitle = L"Pairing...";
        } else if (device.address.ullLong == g_bluetoothConnectingAddress && g_bluetoothConnectingState == 1) {
            subtitle = L"Connecting...";
        } else if (device.address.ullLong == g_bluetoothConnectingAddress && g_bluetoothConnectingState == 2) {
            subtitle = L"Disconnecting...";
        } else if (device.connected) {
            subtitle = L"Connected";
        } else if (device.paired) {
            subtitle = L"Paired";
        } else {
            subtitle = L"Available";
        }
        // Append battery percentage if known
        if (device.batteryPercent >= 0) {
            subtitle += L" · " + std::to_wstring(device.batteryPercent) + L"% battery";
        }

        FrameworkElement trailing{nullptr};
        if (device.address.ullLong == g_bluetoothConnectingAddress) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 15, 1.7);
        } else if (device.connected) {
            trailing = BuildVectorIcon(nullptr, L"", icons::kCheckStroke, 24, 15, 2.0);
        }

        bluetooth::Device captured = device;
        children.Append(MakeListRow(icon, device.name, subtitle, trailing, [captured] {


            if (!captured.paired) {
                RunInBackground([captured] {
                    g_pairingDeviceAddress = captured.address.ullLong;
                    RunOnUiThread([] { PopulateBluetoothPanel(); }); // show "Pairing..." immediately

                    Wh_Log(L"Clicking unpaired device: %s", captured.name.c_str());
                    bool paired = bluetooth::PairDevice(captured.address);
                    Wh_Log(L"PairDevice returned: %d", paired ? 1 : 0);

                    RunOnUiThread([paired, captured]() mutable {
                        g_pairingDeviceAddress = 0;

                        if (paired) {
                            for (auto& device : g_bluetoothDevices) {
                                if (device.address.ullLong == captured.address.ullLong) {
                                    device.paired = true;
                                    break;
                                }
                            }
                        }
                        PopulateBluetoothPanel();
                    });
                });
                return;
            }

            bool connect = !captured.connected;
            
            // Set state: 1 for Connect, 2 for Disconnect
            g_bluetoothConnectingState = connect ? 1 : 2;
            g_bluetoothConnectingAddress = captured.address.ullLong;
            RepopulateLater(PopulateBluetoothPanel);
            
            RunInBackground([captured, connect] {
                bluetooth::SetConnected(captured.address, connect);
                WaitForSingleObject(g_stopEvent, 2500);
                auto devices = bluetooth::Enumerate(false);
                RunOnUiThread([devices = std::move(devices)]() mutable {
                    g_bluetoothDevices = std::move(devices);
                    g_bluetoothConnectingAddress = 0;
                    g_bluetoothConnectingState = 0; // Reset state
                    RefreshBluetoothButtonIcon();
                    PopulateBluetoothPanel();
                });
            });
        }));
    }

    // (Scan button removed – scanning is now automatic or via header button)

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Bluetooth settings", L"ms-settings:bluetooth"));
    // Apply any styles for dynamically created elements (like the Bluetooth toggle).
    ApplyAllControlStyles();
}

// ============================================================================
// Tray panel
// ============================================================================

// Turns raw BGRA into a BitmapImage the same way HIconToBitmapImage does, via a
// BMP in an in-memory stream.
wuxm::Imaging::BitmapImage Bgra32ToBitmapImage(const std::vector<uint8_t>& pixels, int width,
                                               int height) {
    if (pixels.empty() || width <= 0 || height <= 0) {
        return nullptr;
    }

    BITMAPINFOHEADER infoHeader{};
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = width;
    infoHeader.biHeight = -height;  // top-down
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = static_cast<DWORD>(pixels.size());

    BITMAPFILEHEADER fileHeader{};
    fileHeader.bfType = 0x4D42;  // "BM"
    fileHeader.bfSize = static_cast<DWORD>(sizeof(fileHeader) + sizeof(infoHeader) +
                                           pixels.size());
    fileHeader.bfOffBits = sizeof(fileHeader) + sizeof(infoHeader);

    try {
        winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
        winrt::Windows::Storage::Streams::DataWriter writer(stream);
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader)));
        writer.WriteBytes(winrt::array_view<const uint8_t>(
            reinterpret_cast<const uint8_t*>(&infoHeader), sizeof(infoHeader)));
        writer.WriteBytes(pixels);
        writer.StoreAsync().get();
        writer.DetachStream();
        stream.Seek(0);

        wuxm::Imaging::BitmapImage bitmap;
        bitmap.SetSourceAsync(stream);
        return bitmap;
    } catch (...) {
        return nullptr;
    }
}

// There is no API that hands over another process's notification icon bitmap,
// so the icon is read back off the screen where the real taskbar is already
// drawing it. Anything occluded or off-screen simply produces a blank capture,
// which is detected and falls back to a generic glyph.
wuxm::Imaging::BitmapImage CaptureScreenRect(const RECT& bounds) {
    int width = bounds.right - bounds.left;
    int height = bounds.bottom - bounds.top;
    if (width <= 0 || height <= 0 || width > 256 || height > 256) {
        return nullptr;
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }
    HDC memoryDc = CreateCompatibleDC(screenDc);
    if (!memoryDc) {
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(memoryDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) {
        DeleteDC(memoryDc);
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    HGDIOBJ old = SelectObject(memoryDc, dib);
    BOOL copied = BitBlt(memoryDc, 0, 0, width, height, screenDc, bounds.left, bounds.top,
                         SRCCOPY);

    std::vector<uint8_t> pixels;
    if (copied) {
        size_t dataSize = static_cast<size_t>(width) * height * 4;
        pixels.resize(dataSize);
        memcpy(pixels.data(), bits, dataSize);
    }

    SelectObject(memoryDc, old);
    DeleteObject(dib);
    DeleteDC(memoryDc);
    ReleaseDC(nullptr, screenDc);

    if (pixels.empty()) {
        return nullptr;
    }

    // BitBlt gives no alpha channel, and the taskbar behind the icon is a solid
    // colour. Treat the most common colour as the background and knock it out,
    // so the icon sits on the panel rather than on a grey chip.
    std::map<uint32_t, int> histogram;
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        uint32_t key = (static_cast<uint32_t>(pixels[i]) << 16) |
                       (static_cast<uint32_t>(pixels[i + 1]) << 8) |
                       static_cast<uint32_t>(pixels[i + 2]);
        histogram[key]++;
    }
    uint32_t background = 0;
    int backgroundCount = 0;
    for (const auto& entry : histogram) {
        if (entry.second > backgroundCount) {
            background = entry.first;
            backgroundCount = entry.second;
        }
    }

    int totalPixels = width * height;
    // A capture that is almost entirely one colour is an occluded or empty
    // slot, not an icon.
    if (backgroundCount > totalPixels * 9 / 10) {
        return nullptr;
    }

    uint8_t backgroundB = static_cast<uint8_t>((background >> 16) & 0xFF);
    uint8_t backgroundG = static_cast<uint8_t>((background >> 8) & 0xFF);
    uint8_t backgroundR = static_cast<uint8_t>(background & 0xFF);
    for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
        int deltaB = std::abs(static_cast<int>(pixels[i]) - backgroundB);
        int deltaG = std::abs(static_cast<int>(pixels[i + 1]) - backgroundG);
        int deltaR = std::abs(static_cast<int>(pixels[i + 2]) - backgroundR);
        bool isBackground = deltaB + deltaG + deltaR < 24;
        pixels[i + 3] = isBackground ? 0 : 255;
        if (isBackground) {
            pixels[i] = pixels[i + 1] = pixels[i + 2] = 0;  // premultiplied
        }
    }

    return Bgra32ToBitmapImage(pixels, width, height);
}


struct BatteryInfo {
    int percentage = 0;
    int health = 100;   // percentage of design capacity
    bool charging = false;
    bool powerSaving = false;
    bool present = true; // true if a battery exists
};



// Helper: Query battery health via WMI (Win32_Battery)
int QueryBatteryHealth() {
    static int cachedHealth = -1;
    if (cachedHealth >= 0) return cachedHealth;

    int health = 100;
    ULONGLONG designCap = 0;
    ULONGLONG fullCap = 0;

    static const CLSID kCLSID_WbemLocator = {
        0x4590f811, 0x1d3a, 0x11d0,
        {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
    static const IID kIID_IWbemLocator = {
        0xdc12a687, 0x737f, 0x11cf,
        {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};

    auto extract = [](const VARIANT& v) -> ULONGLONG {
        switch (v.vt) {
            case VT_UI1: return v.bVal;
            case VT_UI2: return v.uiVal;
            case VT_UI4: return v.ulVal;
            case VT_UI8: return v.ullVal;
            case VT_I1:  return (ULONGLONG)v.cVal;
            case VT_I2:  return (ULONGLONG)v.iVal;
            case VT_I4:  return (ULONGLONG)v.lVal;
            case VT_I8:  return (ULONGLONG)v.llVal;
            default:     return 0;
        }
    };

    auto queryOne = [&](IWbemServices* services, PCWSTR cls, PCWSTR prop) -> ULONGLONG {
        std::wstring q = L"SELECT ";
        q += prop;
        q += L" FROM ";
        q += cls;
        BSTR lang = SysAllocString(L"WQL");
        BSTR query = SysAllocString(q.c_str());
        winrt::com_ptr<IEnumWbemClassObject> enumerator;
        HRESULT r = services->ExecQuery(
            lang, query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr, enumerator.put());
        SysFreeString(query);
        SysFreeString(lang);
        if (FAILED(r) || !enumerator) return 0;

        IWbemClassObject* obj = nullptr;
        ULONG returned = 0;
        if (SUCCEEDED(enumerator->Next(2000, 1, &obj, &returned)) && returned && obj) {
            VARIANT v;
            VariantInit(&v);
            ULONGLONG result = 0;
            if (SUCCEEDED(obj->Get(prop, 0, &v, nullptr, nullptr))) {
                result = extract(v);
            }
            VariantClear(&v);
            obj->Release();
            return result;
        }
        return 0;
    };

    try {
        winrt::com_ptr<IWbemLocator> locator;
        if (SUCCEEDED(CoCreateInstance(kCLSID_WbemLocator, nullptr,
                                       CLSCTX_INPROC_SERVER, kIID_IWbemLocator,
                                       locator.put_void()))) {
            BSTR ns = SysAllocString(L"root\\WMI");
            winrt::com_ptr<IWbemServices> services;
            HRESULT hr = locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0,
                                                nullptr, nullptr, services.put());
            SysFreeString(ns);
            if (SUCCEEDED(hr) && services) {
                CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                                  nullptr, RPC_C_AUTHN_LEVEL_CALL,
                                  RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
                designCap = queryOne(services.get(), L"BatteryStaticData",
                                     L"DesignedCapacity");
                fullCap = queryOne(services.get(), L"BatteryFullChargedCapacity",
                                   L"FullChargedCapacity");
            }
        }
    } catch (...) {}

    if (designCap == 0 || fullCap == 0) {
        try {
            winrt::com_ptr<IWbemLocator> locator;
            if (SUCCEEDED(CoCreateInstance(kCLSID_WbemLocator, nullptr,
                                           CLSCTX_INPROC_SERVER, kIID_IWbemLocator,
                                           locator.put_void()))) {
                BSTR ns = SysAllocString(L"root\\cimv2");
                winrt::com_ptr<IWbemServices> services;
                HRESULT hr = locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0,
                                                    nullptr, nullptr, services.put());
                SysFreeString(ns);
                if (SUCCEEDED(hr) && services) {
                    CoSetProxyBlanket(services.get(), RPC_C_AUTHN_WINNT,
                                      RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
                                      RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
                    BSTR query = SysAllocString(
                        L"SELECT FullChargeCapacity, DesignCapacity FROM Win32_Battery");
                    BSTR lang = SysAllocString(L"WQL");
                    winrt::com_ptr<IEnumWbemClassObject> enumerator;
                    if (SUCCEEDED(services->ExecQuery(
                            lang, query,
                            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                            nullptr, enumerator.put()))) {
                        IWbemClassObject* obj = nullptr;
                        ULONG returned = 0;
                        if (SUCCEEDED(enumerator->Next(2000, 1, &obj, &returned)) &&
                            returned && obj) {
                            VARIANT full, design;
                            VariantInit(&full);
                            VariantInit(&design);
                            if (SUCCEEDED(obj->Get(L"FullChargeCapacity", 0, &full,
                                                   nullptr, nullptr)) &&
                                SUCCEEDED(obj->Get(L"DesignCapacity", 0, &design,
                                                   nullptr, nullptr))) {
                                fullCap   = extract(full);
                                designCap = extract(design);
                            }
                            VariantClear(&full);
                            VariantClear(&design);
                            obj->Release();
                        }
                    }
                    SysFreeString(query);
                    SysFreeString(lang);
                }
            }
        } catch (...) {}
    }

    if (designCap > 0 && fullCap > 0) {
        health = static_cast<int>((fullCap * 100ULL) / designCap);
        if (health > 100) health = 100;
        if (health < 0) health = 0;
    }
    cachedHealth = health;
    return health;
}

BatteryInfo GetBatteryInfo() {
    BatteryInfo info;
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus)) {
        if (powerStatus.BatteryLifePercent == 255 || (powerStatus.BatteryFlag & 128)) {
            info.present = false;
            info.percentage = 100;
            info.charging = false;
        } else {
            info.percentage = powerStatus.BatteryLifePercent;
            info.charging = (powerStatus.ACLineStatus == 1);
        }
    } else {
        info.present = false;
    }

    info.powerSaving = false;
    HKEY key = nullptr;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Power\\SystemSettings",
                     0, KEY_READ, &key) == ERROR_SUCCESS) {
        DWORD value = 0, size = sizeof(value);
        if (RegQueryValueEx(key, L"PowerSavingMode", nullptr, nullptr, (BYTE*)&value, &size) == ERROR_SUCCESS)
            info.powerSaving = value != 0;
        RegCloseKey(key);
    }

    if (info.present) {
        info.health = QueryBatteryHealth();
    }
    return info;
}

void UpdateBatteryButton() {
    if (!g_batteryButton) return;
    BatteryInfo info = GetBatteryInfo();
    static int s_lastPercentage = -1;
    static bool s_lastCharging = false;
    static bool s_initialized = false;
    if (s_initialized && info.percentage == s_lastPercentage &&
        info.charging == s_lastCharging) {
        return;
    }
    s_lastPercentage = info.percentage;
    s_lastCharging = info.charging;
    s_initialized = true;
    auto batteryIcon = BuildBatteryIcon(20, info.percentage, info.charging);
    if (batteryIcon) {
        batteryIcon.VerticalAlignment(VerticalAlignment::Center);
        g_batteryButton.Content(batteryIcon);
    }
}

void ApplyResourceUsageToButton(const resource::Usage& usage) {
    if (!g_resourceButton) return;

    // Horizontal row of [icon][label] pairs so the tray button matches the
    // visual language of the CPU / RAM / GPU tabs inside its own flyout.
    wuxc::StackPanel row;
    row.Orientation(wuxc::Orientation::Horizontal);
    row.Spacing(0);
    row.VerticalAlignment(VerticalAlignment::Center);
    row.HorizontalAlignment(HorizontalAlignment::Center);

    bool any = false;

    auto appendMetric = [&](PCWSTR iconStroke, const std::wstring& label) {
        if (any) {
            // Inter-group spacer
            auto spacer = MakeText(nullptr, L" ", 12);
            spacer.MinWidth(8);
            row.Children().Append(spacer);
        }
        if (auto icon = BuildVectorIcon(nullptr, L"", iconStroke, 24, 14, 1.7)) {
            icon.VerticalAlignment(VerticalAlignment::Center);
            icon.Margin(Thickness{0, 0, 3, 0});
            row.Children().Append(icon);
        }
        auto text = MakeText(nullptr, label, 12);
        text.VerticalAlignment(VerticalAlignment::Center);
        row.Children().Append(text);
        any = true;
    };

    if (g_settings.showCpuUsage) {
        std::wstring label = L"CPU: ";
        label += usage.cpuAvailable ? (std::to_wstring(usage.cpu) + L"%") : L"-";
        appendMetric(icons::kCpuChipStroke, label);
    }
    if (g_settings.showRamUsage) {
        appendMetric(icons::kRamStickStroke,
                     L"RAM: " + std::to_wstring(usage.ram) + L"%");
    }
    if (g_settings.showGpuUsage) {
        std::wstring label = L"GPU: ";
        label += usage.gpuAvailable ? (std::to_wstring(usage.gpu) + L"%") : L"-";
        appendMetric(icons::kGpuCardStroke, label);
    }

    if (!any) {
        g_resourceButton.Visibility(Visibility::Collapsed);
        return;
    }

    g_resourceButton.Visibility(Visibility::Visible);
    g_resourceButton.Content(row);
}

void UpdateResourceButton() {
    if (!g_resourceButton) return;
    RunInBackground([] {
        resource::Initialize();
        auto usage = resource::GetUsage();
        RunOnUiThread([usage] {
            try {
                ApplyResourceUsageToButton(usage);
            } catch (...) {
            }
        });
    });
}

// Update the graph and stats based on current tab
void ApplyResourceFlyoutContentUI(const resource::Usage& usage,
                                  const resource::DetailedInfo& detailed) {
    try {
    // Update Info Island - show only the currently selected component and update it
    if (g_infoCpuLabel && g_infoCpuName && g_infoRamLabel && g_infoRamName && g_infoGpuLabel && g_infoGpuCombo) {
        if (g_currentTab == 0) {
            g_infoCpuLabel.Visibility(Visibility::Visible);
            g_infoCpuName.Visibility(Visibility::Visible);
            g_infoRamLabel.Visibility(Visibility::Collapsed);
            g_infoRamName.Visibility(Visibility::Collapsed);
            g_infoGpuLabel.Visibility(Visibility::Collapsed);
            g_infoGpuCombo.Visibility(Visibility::Collapsed);

            std::wstring cpuName = resource::GetCpuFullName();
            // Strip the hardcoded base frequency from the name string
            size_t atPos = cpuName.find_last_of(L'@');
            if (atPos != std::wstring::npos) {
                cpuName = cpuName.substr(0, atPos);
            }
            while (!cpuName.empty() && cpuName.back() == L' ') cpuName.pop_back();
            // Append the current fluctuating speed
            cpuName += L" @ " + std::to_wstring(detailed.cpuClockMHz) + L" MHz";
            g_infoCpuName.Text(winrt::hstring(cpuName));
        } else if (g_currentTab == 1) {
            g_infoCpuLabel.Visibility(Visibility::Collapsed);
            g_infoCpuName.Visibility(Visibility::Collapsed);
            g_infoRamLabel.Visibility(Visibility::Visible);
            g_infoRamName.Visibility(Visibility::Visible);
            g_infoGpuLabel.Visibility(Visibility::Collapsed);
            g_infoGpuCombo.Visibility(Visibility::Collapsed);

            std::wstring current = g_infoRamName.Text().c_str();
            if (current.empty() || current == L"Loading...") {
                std::wstring ramText;
                if (!detailed.ramManufacturer.empty()) {
                    ramText = detailed.ramManufacturer;
                }
                if (!detailed.ramPartNumber.empty()) {
                    if (!ramText.empty()) ramText += L" ";
                    ramText += detailed.ramPartNumber;
                }
                if (ramText.empty()) {
                    double ramGB = static_cast<double>(detailed.ramCapacity) / (1024.0 * 1024.0 * 1024.0);
                    wchar_t ramBuffer[32];
                    swprintf_s(ramBuffer, L"%.1f GB", ramGB);
                    ramText = ramBuffer;
                }
                if (!ramText.empty()) g_infoRamName.Text(winrt::hstring(ramText));
            }
        } else if (g_currentTab == 2) {
            g_infoCpuLabel.Visibility(Visibility::Collapsed);
            g_infoCpuName.Visibility(Visibility::Collapsed);
            g_infoRamLabel.Visibility(Visibility::Collapsed);
            g_infoRamName.Visibility(Visibility::Collapsed);
            g_infoGpuLabel.Visibility(Visibility::Visible);
            g_infoGpuCombo.Visibility(Visibility::Visible);

            if (g_infoGpuCombo.Items().Size() == 0) {
                if (!g_selectedGpuIndexLoaded) {
                    g_selectedGpuIndex = Wh_GetIntValue(L"selectedGpuIndex", 0);
                    g_selectedGpuIndexLoaded = true;
                }
                g_gpuNames = resource::GetAllGpuNames();
                for (const auto& name : g_gpuNames) {
                    wuxc::ComboBoxItem item;
                    item.Content(winrt::box_value(winrt::hstring(name)));
                    g_infoGpuCombo.Items().Append(item);
                }
                if (g_gpuNames.size() > 0) {
                    if (g_selectedGpuIndex < 0 || g_selectedGpuIndex >= (int)g_gpuNames.size()) {
                        g_selectedGpuIndex = 0;
                    }
                    bool prev = g_populatingPanel;
                    g_populatingPanel = true;
                    g_infoGpuCombo.SelectedIndex(g_selectedGpuIndex);
                    g_populatingPanel = prev;
                }
            }
        }
    }

    // Add to history (max 60 points)
    const size_t maxSamples = 60;
    if (g_currentTab == 0) {
        if (usage.cpuAvailable) g_cpuHistory.push_back(static_cast<float>(usage.cpu));
        else g_cpuHistory.push_back(0);
        if (g_cpuHistory.size() > maxSamples) g_cpuHistory.pop_front();
    } else if (g_currentTab == 1) {
        g_ramHistory.push_back(static_cast<float>(usage.ram));
        if (g_ramHistory.size() > maxSamples) g_ramHistory.pop_front();
    } else if (g_currentTab == 2) {
        auto& hist = g_gpuHistory[g_selectedGpuIndex];
        if (usage.gpuAvailable) hist.push_back(static_cast<float>(usage.gpu));
        else hist.push_back(0);
        if (hist.size() > maxSamples) hist.pop_front();
    }

    // Set graph colors based on current tab
    wui::Color lineColor;
    wui::Color fillColor;
    if (g_currentTab == 0) {
        // CPU: #34C759 line, #288A5D fill
        lineColor = wui::ColorHelper::FromArgb(255, 0x34, 0xC7, 0x59);
        fillColor = wui::ColorHelper::FromArgb(80, 0x28, 0x8A, 0x5D);
    } else if (g_currentTab == 1) {
        // RAM: #5C9EFA line, darker fill
        lineColor = wui::ColorHelper::FromArgb(255, 0x5C, 0x9E, 0xFA);
        fillColor = wui::ColorHelper::FromArgb(80, 0x3E, 0x6F, 0x9E);
    } else if (g_currentTab == 2) {
        // GPU: Fluent red #E81123 line, darker red fill
        lineColor = wui::ColorHelper::FromArgb(255, 0xE8, 0x11, 0x23);
        fillColor = wui::ColorHelper::FromArgb(80, 0x80, 0x00, 0x00);
    }
    g_graphLine.Stroke(MakeBrush(lineColor.A, lineColor.R, lineColor.G, lineColor.B));
    g_graphFill.Fill(MakeBrush(fillColor.A, fillColor.R, fillColor.G, fillColor.B));

    // Draw graph (line + fill)
    auto& history = (g_currentTab == 0) ? g_cpuHistory
                   : (g_currentTab == 1) ? g_ramHistory
                                         : g_gpuHistory[g_selectedGpuIndex];
    
    auto linePoints = g_graphLine.Points();
    auto fillPoints = g_graphFill.Points();
    linePoints.Clear();
    fillPoints.Clear();

    if (history.size() >= 2) {
        double width = g_graphCanvas.ActualWidth();
        double height = g_graphCanvas.ActualHeight();
        if (width <= 0 || height <= 0) width = 300, height = 150;
        double minVal = 0, maxVal = 100;
        for (float v : history) {
            if (v < minVal) minVal = v;
            if (v > maxVal) maxVal = v;
        }
        if (maxVal - minVal < 1) maxVal = minVal + 1;

        // Add fill bottom-left point
        fillPoints.Append(winrt::Windows::Foundation::Point{0, static_cast<float>(height)});
        
        size_t count = history.size();
        for (size_t i = 0; i < count; ++i) {
            double x = width * i / (count - 1);
            double y = height - (height * (history[i] - minVal) / (maxVal - minVal));
            auto point = winrt::Windows::Foundation::Point{static_cast<float>(x), static_cast<float>(y)};
            linePoints.Append(point);
            fillPoints.Append(point);
        }
        
        // Add fill bottom-right point
        fillPoints.Append(winrt::Windows::Foundation::Point{static_cast<float>(width), static_cast<float>(height)});
    }

    // Update stats grid (2x2)
    std::wstring label1, value1, label2, value2, label3, value3, label4, value4;
    if (g_currentTab == 0) {
        label1 = L"CPU Usage"; value1 = std::to_wstring(usage.cpu) + L"%";
        label2 = L"Clock Speed"; value2 = std::to_wstring(detailed.cpuClockMHz) + L" MHz";
        label3 = L"Cores"; value3 = std::to_wstring(detailed.cpuCores);
        label4 = L"Threads"; value4 = std::to_wstring(detailed.cpuThreads);
    } else if (g_currentTab == 1) {
        label1 = L"RAM Usage"; value1 = std::to_wstring(usage.ram) + L"%";
        label2 = L"RAM Capacity"; 
        double ramGB = static_cast<double>(detailed.ramCapacity) / (1024.0 * 1024.0 * 1024.0);
        wchar_t ramBuffer[32];
        swprintf_s(ramBuffer, L"%.1f GB", ramGB);
        value2 = ramBuffer;
        label3 = L"Speed"; value3 = std::to_wstring(detailed.ramSpeedMHz) + L" MHz";
        label4 = L"Virtual Mem"; value4 = std::to_wstring(detailed.virtualMemoryUsed / (1024*1024)) + L" MB";
    } else if (g_currentTab == 2) {
        label1 = L"GPU Usage"; value1 = std::to_wstring(usage.gpu) + L"%";
        label2 = L"VRAM"; 
        double vramUsedGB = static_cast<double>(detailed.vramUsed) / (1024.0 * 1024.0 * 1024.0);
        double vramTotalGB = static_cast<double>(detailed.vramTotal) / (1024.0 * 1024.0 * 1024.0);
        wchar_t vramBuffer[64];
        swprintf_s(vramBuffer, L"%.1f / %.1f GB", vramUsedGB, vramTotalGB);
        value2 = vramBuffer;
        label3 = L""; value3 = L"";
        label4 = L""; value4 = L"";
    }

    // Collapse the bottom row for the GPU tab to remove the empty space
    if (g_currentTab == 2) {
        g_statCell2.Visibility(Visibility::Collapsed);
        g_statCell3.Visibility(Visibility::Collapsed);
    } else {
        g_statCell2.Visibility(Visibility::Visible);
        g_statCell3.Visibility(Visibility::Visible);
    }

    // Update text blocks
    if (g_statLabel0) g_statLabel0.Text(winrt::hstring(label1));
    if (g_statValue0) g_statValue0.Text(winrt::hstring(value1));
    if (g_statLabel1) g_statLabel1.Text(winrt::hstring(label2));
    if (g_statValue1) g_statValue1.Text(winrt::hstring(value2));
    if (g_statLabel2) g_statLabel2.Text(winrt::hstring(label3));
    if (g_statValue2) g_statValue2.Text(winrt::hstring(value3));
    if (g_statLabel3) g_statLabel3.Text(winrt::hstring(label4));
    if (g_statValue3) g_statValue3.Text(winrt::hstring(value4));

    // Highlight active tab - WinUI pill style with graph color tint
    wui::Color tabBorderColor;
    wui::Color tabBackgroundColor;
    if (g_currentTab == 0) {
        // CPU graph colors: line #34C759, fill #288A5D
        tabBorderColor = wui::ColorHelper::FromArgb(255, 0x34, 0xC7, 0x59);
        tabBackgroundColor = wui::ColorHelper::FromArgb(80, 0x28, 0x8A, 0x5D);
    } else if (g_currentTab == 1) {
        // RAM graph colors: line #5C9EFA, fill #3E6F9E
        tabBorderColor = wui::ColorHelper::FromArgb(255, 0x5C, 0x9E, 0xFA);
        tabBackgroundColor = wui::ColorHelper::FromArgb(80, 0x3E, 0x6F, 0x9E);
    } else {
        // GPU graph colors: line #E81123, fill #800000
        tabBorderColor = wui::ColorHelper::FromArgb(255, 0xE8, 0x11, 0x23);
        tabBackgroundColor = wui::ColorHelper::FromArgb(80, 0x80, 0x00, 0x00);
    }

    for (size_t i = 0; i < g_tabButtons.size(); i++) {
        if (!g_tabButtons[i]) continue;
        if (static_cast<int>(i) == g_currentTab) {
            g_tabButtons[i].Background(MakeBrush(tabBackgroundColor.A, tabBackgroundColor.R, tabBackgroundColor.G, tabBackgroundColor.B));
            g_tabButtons[i].BorderBrush(MakeBrush(tabBorderColor.A, tabBorderColor.R, tabBorderColor.G, tabBorderColor.B));
            g_tabButtons[i].BorderThickness(Thickness{2, 2, 2, 2});
        } else {
            g_tabButtons[i].Background(MakeBrush(0, 0, 0, 0));
            g_tabButtons[i].BorderBrush(MakeBrush(0, 0, 0, 0));
            g_tabButtons[i].BorderThickness(Thickness{0, 0, 0, 0});
        }
    }

        } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"ApplyResourceFlyoutContentUI failed: %s", ex.message().c_str());
        } catch (...) {
        Wh_Log(L"ApplyResourceFlyoutContentUI failed (unknown error)");
        }
    }

void UpdateResourceFlyoutContent() {
    if (!g_graphCanvas || !g_graphLine || !g_graphFill || !g_statsGrid) return;
    int gpuIndex = g_selectedGpuIndex;
    RunInBackground([gpuIndex] {
        resource::Initialize();
        auto usage = resource::GetUsage();
        auto detailed = resource::GetDetailedInfo(gpuIndex);
        RunOnUiThread([usage, detailed] {
            try {
                ApplyResourceFlyoutContentUI(usage, detailed);
            } catch (...) {
            }
        });
    });
}

// Populate the resource flyout (with tabs, graph, stats)
void PopulateResourceFlyout() {
    if (!g_resourcePanel) return;
    try {

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_resourcePanel.Children();
    children.Clear();

    // Title
    children.Append(MakePanelTitle(L"Resource Monitor"));

    // Tab buttons
    wuxc::StackPanel tabPanel;
    tabPanel.Orientation(wuxc::Orientation::Horizontal);
    tabPanel.Spacing(4);
    tabPanel.Margin(Thickness{0, 0, 0, 8});

    auto makeTabButton = [&](PCWSTR name, int tabIndex,
                             PCWSTR iconStroke) -> wuxc::Button {
        auto btn = MakeGhostButton(name, kRowCorner);
        wuxc::StackPanel content;
        content.Orientation(wuxc::Orientation::Horizontal);
        content.Spacing(6);
        content.HorizontalAlignment(HorizontalAlignment::Center);
        content.VerticalAlignment(VerticalAlignment::Center);
        if (auto icon = BuildVectorIcon(nullptr, L"", iconStroke, 24, 20, 1.8)) {
            icon.VerticalAlignment(VerticalAlignment::Center);
            content.Children().Append(icon);
        }
        auto text = MakeText(nullptr, name, 14);
        text.VerticalAlignment(VerticalAlignment::Center);
        content.Children().Append(text);
        btn.Content(content);
        btn.Padding(Thickness{10, 6, 10, 6});
        btn.HorizontalAlignment(HorizontalAlignment::Stretch);
        btn.Width(90);
        btn.Click([tabIndex](auto&&, auto&&) {
            g_currentTab = tabIndex;
            UpdateResourceFlyoutContent();
        });
        return btn;
    };

    auto cpuTab = makeTabButton(L"CPU", 0, icons::kCpuChipStroke);
    auto ramTab = makeTabButton(L"RAM", 1, icons::kRamStickStroke);
    auto gpuTab = makeTabButton(L"GPU", 2, icons::kGpuCardStroke);
    tabPanel.Children().Append(cpuTab);
    tabPanel.Children().Append(ramTab);
    tabPanel.Children().Append(gpuTab);
    children.Append(tabPanel);

    // Store tab buttons for highlighting
    g_tabButtons.clear();
    g_tabButtons.push_back(cpuTab);
    g_tabButtons.push_back(ramTab);
    g_tabButtons.push_back(gpuTab);

    // Info Island (component details)
    wuxc::Border infoIsland;
    infoIsland.Name(L"InfoIsland");
    infoIsland.Background(MakeBrush(0x15, 0xFF, 0xFF, 0xFF));
    infoIsland.CornerRadius(MakeCorner(6));
    infoIsland.BorderBrush(MakeBrush(0x20, 0xFF, 0xFF, 0xFF));
    infoIsland.BorderThickness(Thickness{1, 1, 1, 1});
    infoIsland.Margin(Thickness{0, 0, 0, 8});
    infoIsland.Padding(Thickness{10, 8, 10, 8});
    infoIsland.HorizontalAlignment(HorizontalAlignment::Stretch);
    infoIsland.VerticalAlignment(VerticalAlignment::Top); // Prevent it from stretching to fill the ScrollViewer

    wuxc::StackPanel infoStack;
    infoStack.Spacing(4);
    infoStack.VerticalAlignment(VerticalAlignment::Center); // Centers the text inside the Border

    // CPU Name
    g_infoCpuLabel = MakeText(nullptr, L"CPU", 10, false, 0.6);
    g_infoCpuName = MakeText(nullptr, L"Loading...", 13, true);
    infoStack.Children().Append(g_infoCpuLabel);
    infoStack.Children().Append(g_infoCpuName);

    // RAM Name
    g_infoRamLabel = MakeText(nullptr, L"Memory", 10, false, 0.6);
    g_infoRamName = MakeText(nullptr, L"Loading...", 13, true);
    infoStack.Children().Append(g_infoRamLabel);
    infoStack.Children().Append(g_infoRamName);

    // GPU Combo
    g_infoGpuLabel = MakeText(nullptr, L"GPU", 10, false, 0.6);
    g_infoGpuCombo = wuxc::ComboBox();
    g_infoGpuCombo.Name(L"GpuSelector");
    g_infoGpuCombo.HorizontalAlignment(HorizontalAlignment::Stretch);
    g_infoGpuCombo.Margin(Thickness{0, 0, 0, 0});
    g_infoGpuCombo.Padding(Thickness{8, 4, 8, 4});
    g_infoGpuCombo.Background(MakeBrush(0x30, 0xFF, 0xFF, 0xFF));
    g_infoGpuCombo.Foreground(MakeBrush(0xFF, 0xFF, 0xFF, 0xFF));
    g_infoGpuCombo.CornerRadius(MakeCorner(4));

    // Drop-down changed handler
    g_infoGpuCombo.SelectionChanged([](auto&& sender, auto&&) {
        if (g_populatingPanel) return;
        auto combo = sender.template as<wuxc::ComboBox>();
        int index = combo.SelectedIndex();
        if (index >= 0 && index != g_selectedGpuIndex) {
            g_selectedGpuIndex = index;
            Wh_SetIntValue(L"selectedGpuIndex", index);
            UpdateResourceFlyoutContent();
        }
    });

    infoStack.Children().Append(g_infoGpuLabel);
    infoStack.Children().Append(g_infoGpuCombo);

    infoIsland.Child(infoStack);
    children.Append(infoIsland);

    // Content area: graph + stats
    wuxc::Grid contentGrid;
    contentGrid.RowDefinitions().Append(wuxc::RowDefinition()); // graph row
    contentGrid.RowDefinitions().Append(wuxc::RowDefinition()); // stats row

    // Graph Canvas (wrapped in Border for rounded corners)
    wuxc::Canvas graphCanvas;
    graphCanvas.Height(150);
    graphCanvas.HorizontalAlignment(HorizontalAlignment::Stretch);
    wuxc::Border graphBorder;
    graphBorder.Height(150);
    graphBorder.Background(MakeBrush(0x10, 0xFF, 0xFF, 0xFF));
    graphBorder.CornerRadius(MakeCorner(6));
    graphBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
    graphBorder.Child(graphCanvas);

    // Graph fill (Polygon – behind the line)
    winrt::Windows::UI::Xaml::Shapes::Polygon fill;
    fill.Stretch(wuxm::Stretch::None);
    fill.Points().Clear();
    graphCanvas.Children().Append(fill);

    // Graph line (on top of fill)
    winrt::Windows::UI::Xaml::Shapes::Polyline line;
    line.Stroke(MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B));
    line.StrokeThickness(2);
    line.Stretch(wuxm::Stretch::None);
    graphCanvas.Children().Append(line);

    contentGrid.Children().Append(graphBorder);
    wuxc::Grid::SetRow(graphBorder, 0);

    // Stats grid (2x2, Task Manager style)
    wuxc::Grid statsGrid;
    statsGrid.Margin(Thickness{10, 8, 10, 8});
    statsGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
    statsGrid.VerticalAlignment(VerticalAlignment::Stretch);
    statsGrid.RowSpacing(8);
    statsGrid.ColumnSpacing(12);
    
    for (int i = 0; i < 2; i++) {
        statsGrid.RowDefinitions().Append(wuxc::RowDefinition());
        statsGrid.ColumnDefinitions().Append(wuxc::ColumnDefinition());
    }

    // Helper to create a stat cell (centered)
    auto makeStatCell = [&](int row, int col, PCWSTR labelText, wuxc::TextBlock& labelOut, wuxc::TextBlock& valueOut, wuxc::StackPanel& cellOut) {
        wuxc::StackPanel cell;
        cell.Spacing(2);
        cell.HorizontalAlignment(HorizontalAlignment::Center);
        auto label = MakeText(nullptr, labelText, 11, false, 0.6);
        label.HorizontalAlignment(HorizontalAlignment::Center);
        label.TextAlignment(TextAlignment::Center);
        auto value = MakeText(nullptr, L"-", 20, true);
        value.HorizontalAlignment(HorizontalAlignment::Center);
        value.TextAlignment(TextAlignment::Center);
        labelOut = label;
        valueOut = value;
        cell.Children().Append(label);
        cell.Children().Append(value);
        wuxc::Grid::SetRow(cell, row);
        wuxc::Grid::SetColumn(cell, col);
        statsGrid.Children().Append(cell);
        
        cellOut = cell; // Store the cell container
    };

    makeStatCell(0, 0, L"CPU Usage", g_statLabel0, g_statValue0, g_statCell0);
    makeStatCell(0, 1, L"Clock Speed", g_statLabel1, g_statValue1, g_statCell1);
    makeStatCell(1, 0, L"Cores", g_statLabel2, g_statValue2, g_statCell2);
    makeStatCell(1, 1, L"Threads", g_statLabel3, g_statValue3, g_statCell3);

    contentGrid.Children().Append(statsGrid);
    wuxc::Grid::SetRow(statsGrid, 1);

    children.Append(contentGrid);

    // Store references for updates
    g_graphCanvas = graphCanvas;
    g_graphLine = line;
    g_graphFill = fill;
    g_statsGrid = statsGrid;

    // Update content immediately
    UpdateResourceFlyoutContent();
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"PopulateResourceFlyout failed: %s", ex.message().c_str());
    } catch (...) {
        Wh_Log(L"PopulateResourceFlyout failed (unknown error)");
    }
}

void PopulateWeatherPanel();

// Rebuilds the tray button's content: [icon] [status] [temp°C]
void RefreshWeatherButtonContent() {
    auto it = g_namedElements.find(L"WeatherButtonContent");
    if (it == g_namedElements.end()) {
        return;
    }
    auto stack = it->second.try_as<wuxc::StackPanel>();
    if (!stack) {
        return;
    }
    stack.Children().Clear();

    int code = g_weatherState.valid ? g_weatherState.weatherCode : 0;
    auto icon = weather::BuildWeatherIcon(15, code);
    if (icon) {
        icon.VerticalAlignment(VerticalAlignment::Center);
        icon.Margin(Thickness{0, 0, 6, 0});
        stack.Children().Append(icon);
    }

    std::wstring status = g_weatherState.valid
                              ? weather::DescribeCode(code)
                              : L"Weather";
    auto statusTb = MakeText(nullptr, status, 12);
    statusTb.Margin(Thickness{0, 0, 8, 0});
    stack.Children().Append(statusTb);

    wchar_t buf[32];
    if (g_weatherState.valid) {
        swprintf_s(buf, L"%.0f°C", g_weatherState.temperature);
    } else {
        wcscpy_s(buf, L"--°C");
    }
    stack.Children().Append(MakeText(nullptr, buf, 12, true));
}

void RebuildWeatherSearchResultsPanel() {
    auto it = g_namedElements.find(L"WeatherSearchResults");
    if (it == g_namedElements.end()) return;
    auto panel = it->second.try_as<wuxc::StackPanel>();
    if (!panel) return;

    // Preserve the search box's caret across the rebuild. On some systems
    // a XAML TextBox drops its SelectionStart when its sibling panel
    // reflows, which made the caret jump to position 0 mid-typing.
    int savedCaret = -1;
    auto boxIt = g_namedElements.find(L"WeatherSearchBox");
    if (boxIt != g_namedElements.end()) {
        if (auto box = boxIt->second.try_as<wuxc::TextBox>()) {
            try {
                if (box.FocusState() != FocusState::Unfocused) {
                    savedCaret = box.SelectionStart();
                }
            } catch (...) {}
        }
    }

    panel.Children().Clear();
    if (g_weatherSearchResults.empty()) {
        if (!g_weatherSearchQuery.empty()) {
            panel.Children().Append(MakeStatusText(L"No cities found."));
        }
    } else {
        for (const auto& city : g_weatherSearchResults) {
            std::wstring secondary = city.admin1;
            if (!city.country.empty()) {
                if (!secondary.empty()) secondary += L", ";
                secondary += city.country;
            }
            WeatherSearchResult captured = city;
            panel.Children().Append(
                MakeListRow(nullptr, city.name, secondary, nullptr, [captured] {
                    g_settings.weatherLocationName = captured.name;
                    g_settings.weatherLatitude = captured.lat;
                    g_settings.weatherLongitude = captured.lon;
                    Wh_SetStringValue(L"weatherLocationName", captured.name.c_str());
                    Wh_SetStringValue(L"weatherLatitude", captured.lat.c_str());
                    Wh_SetStringValue(L"weatherLongitude", captured.lon.c_str());
                    g_weatherLocationPickerActive = false;
                    g_weatherSearchResults.clear();
                    g_weatherSearchQuery.clear();
                    RunInBackground([] {
                        weather::EnsureFresh();
                        RunOnUiThread([] {
                            RefreshWeatherButtonContent();
                            PopulateWeatherPanel();
                        });
                    });
                }));
        }
    }

    if (savedCaret >= 0) {
        auto boxIt2 = g_namedElements.find(L"WeatherSearchBox");
        if (boxIt2 != g_namedElements.end()) {
            if (auto box = boxIt2->second.try_as<wuxc::TextBox>()) {
                try { box.SelectionStart(savedCaret); } catch (...) {}
            }
        }
    }
}

void BuildWeatherLocationPicker(const wf::Collections::IVector<UIElement>& children) {
    children.Append(MakePanelTitle(L"Change Location"));

    wuxc::TextBox searchBox;
    searchBox.Name(L"WeatherSearchBox");
    searchBox.PlaceholderText(L"Type a city name (e.g. Warsaw)");
    searchBox.Text(g_weatherSearchQuery);
    // Setting Text() resets the caret to position 0. Restoring it to the
    // end keeps the caret where a typing user expects it after a rebuild.
    if (!g_weatherSearchQuery.empty()) {
        try { searchBox.SelectionStart(g_weatherSearchQuery.size()); } catch (...) {}
    }
    searchBox.Margin(Thickness{0, 0, 0, 6});
    searchBox.CornerRadius(MakeCorner(kRowCorner));
    children.Append(searchBox);
    g_namedElements.insert_or_assign(L"WeatherSearchBox", searchBox);

    wuxc::StackPanel resultsPanel;
    resultsPanel.Name(L"WeatherSearchResults");
    resultsPanel.Spacing(2);
    children.Append(resultsPanel);
    g_namedElements.insert_or_assign(L"WeatherSearchResults", resultsPanel);

    RebuildWeatherSearchResultsPanel();

    searchBox.TextChanged([](auto&& sender, auto&&) {
        try {
            auto box = sender.template as<wuxc::TextBox>();
            std::wstring query{box.Text()};
            g_weatherSearchQuery = query;
            if (query.empty()) {
                g_weatherSearchResults.clear();
                RebuildWeatherSearchResultsPanel();
                return;
            }
            // Debounce: only fire the geocoding request 400 ms after the
            // last keystroke, so typing "Amsterdam" issues one request
            // instead of nine against the public Open-Meteo API.
            if (!g_weatherSearchDebounceTimer) {
                g_weatherSearchDebounceTimer = DispatcherTimer();
                g_weatherSearchDebounceTimer.Interval(
                    std::chrono::milliseconds(400));
                g_weatherSearchDebounceTimer.Tick(
                    [](wf::IInspectable const&, wf::IInspectable const&) {
                        g_weatherSearchDebounceTimer.Stop();
                        std::wstring q = g_weatherSearchQuery;
                        if (q.empty()) return;
                        int token = g_weatherSearchToken.load();
                        RunInBackground([q, token] {
                            Wh_Log(L"[Weather] Searching: '%s' (token %d)",
                                   q.c_str(), token);
                            auto results = weather::GeocodeMulti(q);
                            Wh_Log(L"[Weather] Results: %d for '%s' (token %d)",
                                   (int)results.size(), q.c_str(), token);
                            RunOnUiThread([results = std::move(results),
                                           token, q]() mutable {
                                if (g_weatherSearchToken.load() != token) {
                                    Wh_Log(L"[Weather] Discarding stale response for '%s'",
                                           q.c_str());
                                    return;
                                }
                                g_weatherSearchResults = std::move(results);
                                RebuildWeatherSearchResultsPanel();
                            });
                        });
                    });
            }
            g_weatherSearchToken.fetch_add(1);
            g_weatherSearchDebounceTimer.Stop();
            g_weatherSearchDebounceTimer.Start();
        } catch (...) {
        }
    });

    if (!g_settings.weatherLocationName.empty()) {
        auto cancelBtn = MakeGhostButton(L"WeatherPickerCancel", kRowCorner);
        cancelBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
        cancelBtn.Padding(Thickness{10, 8, 10, 8});
        cancelBtn.Content(MakeText(nullptr, L"Cancel", 12));
        cancelBtn.Click([](auto&&, auto&&) {
            g_weatherLocationPickerActive = false;
            g_weatherSearchResults.clear();
            g_weatherSearchQuery.clear();
            RepopulateLater(PopulateWeatherPanel);
        });
        children.Append(cancelBtn);
    }

    searchBox.Loaded([searchBox](auto&&, auto&&) {
        searchBox.Focus(FocusState::Programmatic);
    });
}

void BuildWeatherView(const wf::Collections::IVector<UIElement>& children) {
    // Header: city + big temp on the left, centered icon + condition + min/max
    // on the right.
    wuxc::Grid header;

    wuxc::ColumnDefinition leftCol;
    leftCol.Width(GridLength{1, GridUnitType::Star});
    header.ColumnDefinitions().Append(leftCol);

    wuxc::ColumnDefinition rightCol;
    rightCol.Width(GridLength{0, GridUnitType::Auto});
    header.ColumnDefinitions().Append(rightCol);

    wuxc::StackPanel leftStack;
    leftStack.Spacing(2);

    auto cityText = MakeText(L"WeatherCityName",
                             g_settings.weatherLocationName, 18, true);
    leftStack.Children().Append(cityText);

    wchar_t tempBuf[32];
    swprintf_s(tempBuf, L"%.0f°", g_weatherState.temperature);
    auto tempText = MakeText(L"WeatherCurrentTemp", tempBuf, 40, true);
    tempText.Margin(Thickness{0, 0, 0, 0});
    leftStack.Children().Append(tempText);

    wuxc::Grid::SetColumn(leftStack, 0);
    header.Children().Append(leftStack);

    // Right block: icon on the left, condition stacked above min/max on the
    // right of the icon.
    wuxc::StackPanel rightStack;
    rightStack.Orientation(wuxc::Orientation::Horizontal);
    rightStack.Spacing(8);
    rightStack.HorizontalAlignment(HorizontalAlignment::Right);
    rightStack.VerticalAlignment(VerticalAlignment::Center);

    auto bigIcon = weather::BuildWeatherIcon(34, g_weatherState.weatherCode);
    if (bigIcon) {
        bigIcon.VerticalAlignment(VerticalAlignment::Center);
        rightStack.Children().Append(bigIcon);
    }

    wuxc::StackPanel rightTextStack;
    rightTextStack.Spacing(2);
    rightTextStack.VerticalAlignment(VerticalAlignment::Center);

    auto cond = MakeText(L"WeatherDesc",
                         weather::DescribeCode(g_weatherState.weatherCode),
                         13, false, 0.9);
    cond.HorizontalAlignment(HorizontalAlignment::Left);
    rightTextStack.Children().Append(cond);

    wchar_t minMaxBuf[64];
    swprintf_s(minMaxBuf, L"Min: %.0f°C  Max: %.0f°C",
               g_weatherState.tempMin, g_weatherState.tempMax);
    auto minMax = MakeText(L"WeatherMinMax", minMaxBuf, 11, false, 0.65);
    minMax.HorizontalAlignment(HorizontalAlignment::Left);
    rightTextStack.Children().Append(minMax);

    rightStack.Children().Append(rightTextStack);

    wuxc::Grid::SetColumn(rightStack, 1);
    header.Children().Append(rightStack);

    children.Append(header);
    children.Append(MakeDivider());

    // Hourly strip: [<] [scrollable cells] [>]
    // Cell width + spacing chosen so exactly 5 fit at the default 340 DIP
    // panel: 5*48 + 4*4 = 256, which is precisely the width left once the
    // two 24-DIP chevron buttons and their 6-DIP gaps are subtracted.
    wuxc::StackPanel hourlyRow;
    hourlyRow.Orientation(wuxc::Orientation::Horizontal);
    hourlyRow.Spacing(4);

    // The Open-Meteo response gives up to 24 hourly entries per day; show
    // all of them. The ScrollViewer below is what constrains the visible
    // width — the rows scroll horizontally inside a fixed viewport.
    size_t limit = g_weatherState.hourlyTimes.size();
    if (limit > 24) limit = 24;

    for (size_t i = 0; i < limit; i++) {
        wuxc::StackPanel cell;
        cell.Width(48);
        cell.Spacing(3);
        cell.HorizontalAlignment(HorizontalAlignment::Center);

        std::wstring t12 = weather::FormatHour12(g_weatherState.hourlyTimes[i]);
        auto time = MakeText(nullptr, t12, 11, false, 0.7);
        time.HorizontalAlignment(HorizontalAlignment::Center);
        cell.Children().Append(time);

        auto hourIcon = weather::BuildWeatherIcon(20, g_weatherState.hourlyCodes[i]);
        if (hourIcon) {
            hourIcon.HorizontalAlignment(HorizontalAlignment::Center);
            cell.Children().Append(hourIcon);
        }

        wchar_t tBuf[16];
        swprintf_s(tBuf, L"%.0f°", g_weatherState.hourlyTemps[i].second);
        auto tv = MakeText(nullptr, tBuf, 14, true);
        tv.HorizontalAlignment(HorizontalAlignment::Center);
        cell.Children().Append(tv);

        int rain = (i < g_weatherState.hourlyRain.size())
                       ? g_weatherState.hourlyRain[i]
                       : 0;
        wchar_t rBuf[16];
        swprintf_s(rBuf, L"%d%%", rain);
        auto rv = MakeText(nullptr, rBuf, 10, false, 0.55);
        rv.HorizontalAlignment(HorizontalAlignment::Center);
        cell.Children().Append(rv);

        hourlyRow.Children().Append(cell);
    }

    wuxc::ScrollViewer hourlyScroller;
    hourlyScroller.HorizontalScrollBarVisibility(
        wuxc::ScrollBarVisibility::Hidden);
    hourlyScroller.VerticalScrollBarVisibility(
        wuxc::ScrollBarVisibility::Disabled);
    hourlyScroller.HorizontalScrollMode(wuxc::ScrollMode::Enabled);
    hourlyScroller.VerticalScrollMode(wuxc::ScrollMode::Disabled);
    // Snap scrolling to cell boundaries. MandatorySingle means any scroll --
    // wheel, touch or our own ChangeView calls -- always settles with exactly
    // one cell flush against the left edge, never half a card.
    hourlyScroller.HorizontalSnapPointsType(
        wuxc::SnapPointsType::MandatorySingle);
    hourlyScroller.HorizontalSnapPointsAlignment(
        wuxc::Primitives::SnapPointsAlignment::Near);
    hourlyScroller.Content(hourlyRow);
    hourlyScroller.HorizontalAlignment(HorizontalAlignment::Stretch);
    // Center so the scroller reports the cells' natural height, which the row
    // then uses to size the two side buttons.
    hourlyScroller.VerticalAlignment(VerticalAlignment::Center);

    auto scrollLeft = MakeGhostButton(L"WeatherScrollLeft", kTileCorner);
    scrollLeft.Width(24);
    scrollLeft.Padding(Thickness{0, 0, 0, 0});
    scrollLeft.Background(MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
    scrollLeft.BorderBrush(MakeBrush(0x20, 0xFF, 0xFF, 0xFF));
    scrollLeft.BorderThickness(Thickness{1, 1, 1, 1});
    scrollLeft.HorizontalContentAlignment(HorizontalAlignment::Center);
    scrollLeft.VerticalContentAlignment(VerticalAlignment::Center);
    // Stretch so the button takes the height the hourly cells established
    // for the row.
    scrollLeft.VerticalAlignment(VerticalAlignment::Stretch);
    scrollLeft.Content(BuildVectorIcon(nullptr, L"", icons::kChevronLeft, 24, 14, 1.8));
    scrollLeft.Click([hourlyScroller, hourlyRow](auto&&, auto&&) {
        constexpr double kCellStep = 52.0;
        double current = hourlyScroller.HorizontalOffset();
        int currentIndex = static_cast<int>(std::round(current / kCellStep));
        int targetIndex = std::max(0, currentIndex - 5);
        if (targetIndex < static_cast<int>(hourlyRow.Children().Size())) {
            auto cell = hourlyRow.Children().GetAt(targetIndex).as<FrameworkElement>();
            BringIntoViewOptions options;
            options.HorizontalAlignmentRatio(0.0);
            options.AnimationDesired(true);
            cell.StartBringIntoView(options);
        }
    });

    auto scrollRight = MakeGhostButton(L"WeatherScrollRight", kTileCorner);
    scrollRight.Width(24);
    scrollRight.Padding(Thickness{0, 0, 0, 0});
    scrollRight.Background(MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
    scrollRight.BorderBrush(MakeBrush(0x20, 0xFF, 0xFF, 0xFF));
    scrollRight.BorderThickness(Thickness{1, 1, 1, 1});
    scrollRight.HorizontalContentAlignment(HorizontalAlignment::Center);
    scrollRight.VerticalContentAlignment(VerticalAlignment::Center);
    scrollRight.VerticalAlignment(VerticalAlignment::Stretch);
    scrollRight.Content(BuildVectorIcon(nullptr, L"", icons::kChevronRight, 24, 14, 1.8));
    scrollRight.Click([hourlyScroller, hourlyRow](auto&&, auto&&) {
        constexpr double kCellStep = 52.0;
        double current = hourlyScroller.HorizontalOffset();
        int currentIndex = static_cast<int>(std::round(current / kCellStep));
        int targetIndex = currentIndex + 5;
        int childCount = static_cast<int>(hourlyRow.Children().Size());
        if (targetIndex >= childCount) {
            targetIndex = childCount - 1;
        }
        auto cell = hourlyRow.Children().GetAt(targetIndex).as<FrameworkElement>();
        BringIntoViewOptions options;
        options.HorizontalAlignmentRatio(0.0);
        options.AnimationDesired(true);
        cell.StartBringIntoView(options);
    });

    wuxc::Grid scrollRow;
    scrollRow.ColumnSpacing(6);
    // Explicit Auto/Star/Auto: side buttons keep their 28 DIP width and the
    // middle column absorbs whatever is left for the scroller.
    wuxc::ColumnDefinition leftBtnCol;
    leftBtnCol.Width(GridLength{0, GridUnitType::Auto});
    scrollRow.ColumnDefinitions().Append(leftBtnCol);
    wuxc::ColumnDefinition scrollerCol;
    scrollerCol.Width(GridLength{1, GridUnitType::Star});
    scrollRow.ColumnDefinitions().Append(scrollerCol);
    wuxc::ColumnDefinition rightBtnCol;
    rightBtnCol.Width(GridLength{0, GridUnitType::Auto});
    scrollRow.ColumnDefinitions().Append(rightBtnCol);

    wuxc::Grid::SetColumn(scrollLeft, 0);
    scrollRow.Children().Append(scrollLeft);
    wuxc::Grid::SetColumn(hourlyScroller, 1);
    scrollRow.Children().Append(hourlyScroller);
    wuxc::Grid::SetColumn(scrollRight, 2);
    scrollRow.Children().Append(scrollRight);

    children.Append(scrollRow);

    children.Append(MakeDivider());

    auto changeBtn = MakeGhostButton(L"WeatherChangeLocation", kRowCorner);
    changeBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
    changeBtn.Padding(Thickness{10, 8, 10, 8});
    changeBtn.Content(MakeText(nullptr, L"Change location", 12));
    changeBtn.Click([](auto&&, auto&&) {
        g_weatherLocationPickerActive = true;
        g_weatherSearchResults.clear();
        g_weatherSearchQuery.clear();
        RepopulateLater(PopulateWeatherPanel);
    });
    children.Append(changeBtn);
}

void PopulateWeatherPanel() {
    if (!g_weatherPanel) return;

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_weatherPanel.Children();
    children.Clear();

    const bool hasLocation =
        !g_settings.weatherLocationName.empty() &&
        !g_settings.weatherLatitude.empty() &&
        !g_settings.weatherLongitude.empty();

    if (g_weatherLocationPickerActive || !hasLocation) {
        BuildWeatherLocationPicker(children);
    } else {
        BuildWeatherView(children);
    }

    ApplyAllControlStyles();
}

void PopulateRecycleBinPanel() {
    if (!g_recycleBinPanel) return;

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    recyclebin::Refresh();

    auto children = g_recycleBinPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"Recycle Bin"));

    if (!g_recycleBinState.valid) {
        children.Append(MakeStatusText(L"Recycle Bin is not available."));
        return;
    }

    uint64_t bytes = g_recycleBinState.sizeBytes;
    uint64_t count = g_recycleBinState.itemCount;

    auto bigText = MakeText(L"RecycleBinSizeText",
                            recyclebin::FormatBytes(bytes), 32, true);
    bigText.HorizontalAlignment(HorizontalAlignment::Center);
    bigText.Margin(Thickness{0, 8, 0, 2});
    children.Append(bigText);

    auto countText = MakeText(
        L"RecycleBinCountText",
        std::to_wstring(count) + (count == 1 ? L" item" : L" items"), 13,
        false, 0.7);
    countText.HorizontalAlignment(HorizontalAlignment::Center);
    countText.Margin(Thickness{0, 0, 0, 10});
    children.Append(countText);

    if (g_recycleBinConfirming) {
        auto warn = MakeText(L"RecycleBinWarning",
                             L"Permanently delete all items in the Recycle Bin?",
                             12, false, 0.9);
        warn.Foreground(MakeBrush(0xFF, 0xFF, 0x8A, 0x80));
        warn.TextWrapping(TextWrapping::Wrap);
        warn.HorizontalAlignment(HorizontalAlignment::Center);
        warn.Margin(Thickness{0, 0, 0, 8});
        children.Append(warn);

        wuxc::Grid buttons;
        buttons.ColumnSpacing(8);
        for (int i = 0; i < 2; i++) {
            wuxc::ColumnDefinition col;
            col.Width(GridLength{1, GridUnitType::Star});
            buttons.ColumnDefinitions().Append(col);
        }

        auto confirm = MakeGhostButton(L"RecycleBinConfirmButton", kRowCorner);
        confirm.Background(MakeBrush(0xFF, 0xC4, 0x2B, 0x1C));
        confirm.Padding(Thickness{12, 8, 12, 8});
        confirm.HorizontalAlignment(HorizontalAlignment::Stretch);
        confirm.HorizontalContentAlignment(HorizontalAlignment::Center);
        confirm.Content(MakeText(nullptr, L"Empty", 12));
        confirm.Click([](auto&&, auto&&) {
            recyclebin::Empty();
            g_recycleBinConfirming = false;
            RepopulateLater(PopulateRecycleBinPanel);
        });
        wuxc::Grid::SetColumn(confirm, 0);
        buttons.Children().Append(confirm);

        auto cancel = MakeGhostButton(L"RecycleBinCancelButton", kRowCorner);
        cancel.Background(MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
        cancel.Padding(Thickness{12, 8, 12, 8});
        cancel.HorizontalAlignment(HorizontalAlignment::Stretch);
        cancel.HorizontalContentAlignment(HorizontalAlignment::Center);
        cancel.Content(MakeText(nullptr, L"Cancel", 12));
        cancel.Click([](auto&&, auto&&) {
            g_recycleBinConfirming = false;
            RepopulateLater(PopulateRecycleBinPanel);
        });
        wuxc::Grid::SetColumn(cancel, 1);
        buttons.Children().Append(cancel);

        children.Append(buttons);
    } else {
        auto emptyButton = MakeGhostButton(L"RecycleBinEmptyButton", kRowCorner);
        emptyButton.HorizontalAlignment(HorizontalAlignment::Stretch);
        emptyButton.Padding(Thickness{12, 10, 12, 10});
        emptyButton.Content(MakeText(nullptr, L"Empty Recycle Bin", 12));
        emptyButton.Click([](auto&&, auto&&) {
            g_recycleBinConfirming = true;
            RepopulateLater(PopulateRecycleBinPanel);
        });
        children.Append(emptyButton);
    }

    children.Append(MakeDivider());
    children.Append(MakeSettingsLink(L"Storage settings",
                                     L"ms-settings:storagesense"));
}

void PopulateBatteryPanel() {
    if (!g_batteryPanel) return;

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_batteryPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"Battery"));

    BatteryInfo info = GetBatteryInfo();

    // Percentage display
    auto percentageText = MakeText(L"BatteryPercentage", std::to_wstring(info.percentage) + L"%", 32, true);
    percentageText.HorizontalAlignment(HorizontalAlignment::Center);
    percentageText.Margin(Thickness{0, 8, 0, 4});
    children.Append(percentageText);

    // Health
    auto healthText = MakeText(L"BatteryHealth", L"Battery health: " + std::to_wstring(info.health) + L"%", 13, false, 0.7);
    healthText.HorizontalAlignment(HorizontalAlignment::Center);
    healthText.Margin(Thickness{0, 0, 0, 8});
    children.Append(healthText);




}

// ============================================================================
// Context menus
//
// The system MenuFlyoutPresenter draws square corners and gives MenuFlyoutItem
// and MenuFlyoutSubItem *different* pointer-over brushes, which is why the
// submenu highlighted in a different colour from the rest. Both are fixed by
// overriding the theme resources rather than by retemplating: the overrides go
// into the application resource dictionary so they also reach the submenu
// popups, which are separate visual trees and would otherwise keep the
// defaults.
// ============================================================================

constexpr double kMenuCorner = 12.0;
constexpr double kMenuItemCorner = 4.0;

void InstallGlobalMenuResources() {
    try {
        auto application = Application::Current();
        if (!application) {
            return;
        }
        auto resources = application.Resources();

        auto hover = MakeBrush(0x30, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);
        auto pressed = MakeBrush(0x20, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);

        auto set = [&](PCWSTR key, wf::IInspectable const& value) {
            auto boxedKey = winrt::box_value(winrt::hstring(key));
            if (resources.HasKey(boxedKey)) {
                resources.Remove(boxedKey);
            }
            resources.Insert(boxedKey, value);
        };

        // Rounds the presenter itself -- the default MenuFlyoutPresenter
        // template binds its corner radius to this.
        set(L"OverlayCornerRadius", winrt::box_value(MakeCorner(kMenuCorner)));

        // Transparent backgrounds for every presenter. The XAML presenter
        // template's inner Border binds its Background to these *theme
        // resource names* rather than to the presenter's own Background
        // property, so setting presenter.Background(blurBrush) is not enough:
        // the theme-resource tint is drawn on top of the blur brush, which is
        // the extra tint users see even at global tint opacity 0. Overriding
        // the resource names here removes that extra layer; the Windhawk blur
        // brush (with its own tint) is then the only thing visible.
        set(L"MenuFlyoutPresenterBackground", FlyoutBackgroundBrush());
        set(L"MenuFlyoutPresenterBorderBrush", MakeBrush(0x30, 0xFF, 0xFF, 0xFF));
        set(L"FlyoutPresenterBackground", FlyoutBackgroundBrush());
        set(L"FlyoutPresenterBorderBrush", MakeBrush(0x30, 0xFF, 0xFF, 0xFF));

        // The pair that made the submenu look different from everything else.
        set(L"MenuFlyoutItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutSubItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundSubMenuOpened", hover);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Could not install menu resources: %08X",
               static_cast<unsigned int>(ex.code().value));
    } catch (...) {
    }
}

Style MakeMenuPresenterStyle() {
    Style style(winrt::xaml_typename<wuxc::MenuFlyoutPresenter>());
    auto setters = style.Setters();
    if (!g_settings.fontFamily.empty()) {
        try {
            setters.Append(Setter(wuxc::Control::FontFamilyProperty(),
                                  winrt::box_value(wuxm::FontFamily(winrt::hstring(g_settings.fontFamily)))));
        } catch (...) {}
    }
    if (g_settings.fontBold) {
        setters.Append(Setter(wuxc::Control::FontWeightProperty(),
                              winrt::box_value(winrt::Windows::UI::Text::FontWeights::Bold())));
    }
    setters.Append(Setter(wuxc::Control::BackgroundProperty(),
                          winrt::box_value(FlyoutBackgroundBrush())));
    setters.Append(Setter(wuxc::Control::BorderBrushProperty(),
                          winrt::box_value(MakeBrush(0x30, 0xFF, 0xFF, 0xFF))));
    setters.Append(Setter(wuxc::Control::BorderThicknessProperty(),
                          winrt::box_value(Thickness{1, 1, 1, 1})));
    setters.Append(Setter(wuxc::Control::CornerRadiusProperty(),
                          winrt::box_value(MakeCorner(kMenuCorner))));
    setters.Append(
        Setter(wuxc::Control::PaddingProperty(), winrt::box_value(Thickness{4, 4, 4, 4})));
    setters.Append(Setter(FrameworkElement::MinWidthProperty(), winrt::box_value(200.0)));
    if (auto tmpl = BuildFlyoutShellTemplate(true)) {
        setters.Append(Setter(wuxc::Control::TemplateProperty(), winrt::box_value(tmpl)));
    }
    return style;
}

// Per-item overrides as well as the global ones: an item's own resource
// dictionary is checked first, so this holds even if a future Windows build
// renames or re-scopes the theme resources.
void ApplyMenuItemLook(wuxc::MenuFlyoutItemBase const& item) {
    try {
        auto hover = MakeBrush(0x24, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);
        auto pressed = MakeBrush(0x14, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B);
        auto resources = item.Resources();
        auto set = [&](PCWSTR key, wf::IInspectable const& value) {
            auto boxedKey = winrt::box_value(winrt::hstring(key));
            if (resources.HasKey(boxedKey)) {
                resources.Remove(boxedKey);
            }
            resources.Insert(boxedKey, value);
        };
        set(L"MenuFlyoutItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundPointerOver", hover);
        set(L"MenuFlyoutSubItemBackgroundPressed", pressed);
        set(L"MenuFlyoutSubItemBackgroundSubMenuOpened", hover);
        set(L"OverlayCornerRadius", winrt::box_value(MakeCorner(kMenuCorner)));

        if (auto control = item.try_as<wuxc::Control>()) {
            control.CornerRadius(MakeCorner(kMenuItemCorner));
            control.Padding(Thickness{12, 7, 12, 7});
        }
    } catch (...) {
    }
}

wuxc::MenuFlyoutItem MakeMenuItem(std::wstring_view text, std::function<void()> onClick) {
    wuxc::MenuFlyoutItem item;
    item.Text(winrt::hstring(text));
    if (!g_settings.fontFamily.empty()) {
        try {
            item.FontFamily(wuxm::FontFamily(winrt::hstring(g_settings.fontFamily)));
        } catch (...) {}
    }
    if (g_settings.fontBold) {
        item.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    }
    ApplyMenuItemLook(item);
    if (onClick) {
        item.Click([onClick = std::move(onClick)](auto&&, auto&&) {
            try {
                onClick();
            } catch (...) {
            }
        });
    }
    item.Loaded([](wf::IInspectable const& sender, RoutedEventArgs const&) {
        auto fe = sender.as<FrameworkElement>();
        DependencyObject current = fe;
        while (auto parent = wuxm::VisualTreeHelper::GetParent(current)) {
            if (auto presenter = parent.try_as<wuxc::MenuFlyoutPresenter>()) {
                if (g_menuShellTemplate && presenter.Template() != g_menuShellTemplate) {
                    presenter.Template(g_menuShellTemplate);
                }
                wui::Color tintColor;
                if (!ParseBarColor(g_settings.topBarBackgroundColor,
                                   g_settings.topBarBackgroundOpacity,
                                   &tintColor)) {
                    tintColor = wui::ColorHelper::FromArgb(128, 0, 0, 0);
                }
                auto isOurBlur = [](wf::IInspectable const& obj) {
                    return obj && obj.try_as<wuxm::XamlCompositionBrushBase>() != nullptr;
                };
                auto blurBrush = winrt::make<XamlBlurBrush>(
                    presenter.as<UIElement>(),
                    static_cast<float>(g_settings.globalBlurAmount),
                    tintColor, tintColor.A);
                if (!isOurBlur(presenter.Background())) {
                    presenter.Background(blurBrush);
                }
                try {
                    if (auto shellBorder =
                            presenter.FindName(L"PART_BackgroundBorder")
                                .try_as<wuxc::Border>()) {
                        if (!isOurBlur(shellBorder.Background())) {
                            shellBorder.Background(blurBrush);
                        }
                    }
                } catch (...) {}
                auto presenterRoot = presenter.as<FrameworkElement>();
                if (std::find(g_detachedStyleRoots.begin(),
                              g_detachedStyleRoots.end(),
                              presenterRoot) == g_detachedStyleRoots.end()) {
                    g_detachedStyleRoots.push_back(presenterRoot);
                }
                RunOnUiThread([] {
                    try { ApplyAllControlStyles(); } catch (...) {}
                });
                break;
            }
            current = parent;
        }
    });
    return item;
}

wuxc::MenuFlyoutSubItem MakeMenuSubItem(std::wstring_view text) {
    wuxc::MenuFlyoutSubItem item;
    item.Text(winrt::hstring(text));
    if (!g_settings.fontFamily.empty()) {
        try {
            item.FontFamily(wuxm::FontFamily(winrt::hstring(g_settings.fontFamily)));
        } catch (...) {}
    }
    if (g_settings.fontBold) {
        item.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    }
    ApplyMenuItemLook(item);
    item.PointerEntered([](auto&&, auto&&) {
        ApplyBlurToAllOpenPopups();
    });
    item.Loaded([](wf::IInspectable const& sender, RoutedEventArgs const&) {
        auto fe = sender.as<FrameworkElement>();
        DependencyObject current = fe;
        while (auto parent = wuxm::VisualTreeHelper::GetParent(current)) {
            if (auto presenter = parent.try_as<wuxc::MenuFlyoutPresenter>()) {
                if (g_menuShellTemplate && presenter.Template() != g_menuShellTemplate) {
                    presenter.Template(g_menuShellTemplate);
                }
                wui::Color tintColor;
                if (!ParseBarColor(g_settings.topBarBackgroundColor,
                                   g_settings.topBarBackgroundOpacity,
                                   &tintColor)) {
                    tintColor = wui::ColorHelper::FromArgb(128, 0, 0, 0);
                }
                auto isOurBlur = [](wf::IInspectable const& obj) {
                    return obj && obj.try_as<wuxm::XamlCompositionBrushBase>() != nullptr;
                };
                auto blurBrush = winrt::make<XamlBlurBrush>(
                    presenter.as<UIElement>(),
                    static_cast<float>(g_settings.globalBlurAmount),
                    tintColor, tintColor.A);
                if (!isOurBlur(presenter.Background())) {
                    presenter.Background(blurBrush);
                }
                try {
                    if (auto shellBorder =
                            presenter.FindName(L"PART_BackgroundBorder")
                                .try_as<wuxc::Border>()) {
                        if (!isOurBlur(shellBorder.Background())) {
                            shellBorder.Background(blurBrush);
                        }
                    }
                } catch (...) {}
                auto presenterRoot = presenter.as<FrameworkElement>();
                if (std::find(g_detachedStyleRoots.begin(),
                              g_detachedStyleRoots.end(),
                              presenterRoot) == g_detachedStyleRoots.end()) {
                    g_detachedStyleRoots.push_back(presenterRoot);
                }
                RunOnUiThread([] {
                    try { ApplyAllControlStyles(); } catch (...) {}
                });
                break;
            }
            current = parent;
        }
    });
    return item;
}

wuxc::MenuFlyoutSeparator MakeMenuSeparator() {
    wuxc::MenuFlyoutSeparator separator;
    return separator;
}

void StyleMenuFlyout(wuxc::MenuFlyout const& menu) {
    menu.MenuFlyoutPresenterStyle(MakeMenuPresenterStyle());
    // Same reason as the control flyouts: the XAML root is bar-height, so a menu
    // constrained to it would be clipped away entirely.
    menu.ShouldConstrainToRootBounds(false);
    menu.Opened([](auto&&, auto&&) {
        ApplyBlurToAllOpenPopups();
    });
}

// ----------------------------------------------------------------------------
// Shell actions used by the Start menu
// ----------------------------------------------------------------------------

void RunShellCommand(PCWSTR file, PCWSTR parameters = nullptr, bool hidden = false) {
    ShellExecute(nullptr, L"open", file, parameters, nullptr,
                 hidden ? SW_HIDE : SW_SHOWNORMAL);
}

// Synthesizes a Win+key chord. Used for the shell surfaces that have no command
// line at all -- Search and Run are only reachable this way.
void SendWinKeyChord(WORD key) {
    INPUT input[4]{};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_LWIN;
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = key;
    input[2].type = INPUT_KEYBOARD;
    input[2].ki.wVk = key;
    input[2].ki.dwFlags = KEYEVENTF_KEYUP;
    input[3].type = INPUT_KEYBOARD;
    input[3].ki.wVk = VK_LWIN;
    input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
}

void ShowDesktop() {
    // 419 is the shell's "toggle desktop" command; it's what Win+D posts.
    if (HWND tray = FindWindow(L"Shell_TrayWnd", nullptr)) {
        PostMessage(tray, WM_COMMAND, 419, 0);
        return;
    }
    SendWinKeyChord('D');
}

void SuspendSystem() {
    using SetSuspendState_t = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
    HMODULE module = LoadLibraryEx(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return;
    }
    auto setSuspendState =
        reinterpret_cast<SetSuspendState_t>(GetProcAddress(module, "SetSuspendState"));
    if (setSuspendState) {
        setSuspendState(FALSE /* hibernate */, FALSE /* force */, FALSE /* wake events */);
    }
    FreeLibrary(module);
}

void BuildStartContextMenu() {
    wuxc::MenuFlyout menu;
    StyleMenuFlyout(menu);

    auto items = menu.Items();
    items.Append(MakeMenuItem(L"TopBar settings…", [] { OpenTopBarSettingsWindow(); }));
    items.Append(MakeMenuSeparator());
    items.Append(MakeMenuItem(L"Task Manager", [] { RunShellCommand(L"taskmgr.exe"); }));
    items.Append(MakeMenuItem(L"Settings", [] { RunShellCommand(L"ms-settings:"); }));
    items.Append(MakeMenuItem(L"File Explorer", [] { RunShellCommand(L"explorer.exe"); }));
    items.Append(MakeMenuItem(L"Search", [] { SendWinKeyChord('S'); }));
    items.Append(MakeMenuItem(L"Run", [] { SendWinKeyChord('R'); }));

    items.Append(MakeMenuSeparator());

    auto powerItem = MakeMenuSubItem(L"Shut down or sign out");
    auto powerItems = powerItem.Items();
    powerItems.Append(
        MakeMenuItem(L"Sign out", [] { RunShellCommand(L"shutdown.exe", L"/l", true); }));
    powerItems.Append(MakeMenuItem(L"Sleep", [] { SuspendSystem(); }));
    powerItems.Append(MakeMenuItem(
        L"Shut down", [] { RunShellCommand(L"shutdown.exe", L"/s /t 0", true); }));
    powerItems.Append(
        MakeMenuItem(L"Restart", [] { RunShellCommand(L"shutdown.exe", L"/r /t 0", true); }));
    items.Append(powerItem);

    items.Append(MakeMenuSeparator());
    items.Append(MakeMenuItem(L"Desktop", [] { ShowDesktop(); }));

    g_startContextMenu = menu;
}

void OpenFileLocationForApp(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    DWORD pid = GetRealAppPid(hwnd);
    if (!pid) return;

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return;

    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok || !path[0]) return;

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(path, nullptr, &pidl, 0, nullptr)) && pidl) {
        SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        CoTaskMemFree(pidl);
        return;
    }

    std::wstring folder = path;
    size_t slash = folder.find_last_of(L"\\/");
    if (slash != std::wstring::npos) folder.resize(slash);
    ShellExecute(nullptr, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void ShowFilePropertiesForApp(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    DWORD pid = GetRealAppPid(hwnd);
    if (!pid) return;

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return;

    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok || !path[0]) return;

    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"properties";
    sei.lpFile = path;
    sei.nShow = SW_SHOW;
    ShellExecuteExW(&sei);
}

void BuildTaskContextMenu() {
    wuxc::MenuFlyout menu;
    StyleMenuFlyout(menu);

    auto items = menu.Items();

    g_taskMenuToggleItem = MakeMenuItem(L"Maximize", [] {
        if (g_contextMenuTargetHwnd) {
            ToggleMaximizeWindow(g_contextMenuTargetHwnd);
        }
    });
    items.Append(g_taskMenuToggleItem);

    items.Append(MakeMenuItem(L"Minimize", [] {
        if (g_contextMenuTargetHwnd) {
            ShowWindow(g_contextMenuTargetHwnd, SW_MINIMIZE);
        }
    }));

    items.Append(MakeMenuItem(L"Bring to front", [] {
        if (g_contextMenuTargetHwnd) {
            ForceForegroundWindow(g_contextMenuTargetHwnd);
        }
    }));

    items.Append(MakeMenuSeparator());

    items.Append(MakeMenuItem(L"Open file location", [] {
        if (g_contextMenuTargetHwnd) {
            OpenFileLocationForApp(g_contextMenuTargetHwnd);
        }
    }));

    items.Append(MakeMenuItem(L"Properties", [] {
        if (g_contextMenuTargetHwnd) {
            ShowFilePropertiesForApp(g_contextMenuTargetHwnd);
        }
    }));

    items.Append(MakeMenuSeparator());

    items.Append(MakeMenuItem(L"Close", [] {
        if (g_contextMenuTargetHwnd) {
            CloseWindowGracefully(g_contextMenuTargetHwnd);
        }
    }));

    g_taskContextMenu = menu;
}

void BuildAppTitleContextMenu() {
    wuxc::MenuFlyout menu;
    StyleMenuFlyout(menu);

    auto items = menu.Items();

    items.Append(MakeMenuItem(L"Open file location", [] {
        if (g_appTitleContextTarget) {
            OpenFileLocationForApp(g_appTitleContextTarget);
        }
    }));

    items.Append(MakeMenuItem(L"Properties", [] {
        if (g_appTitleContextTarget) {
            ShowFilePropertiesForApp(g_appTitleContextTarget);
        }
    }));

    g_appTitleContextMenu = menu;
}

// ============================================================================
// Battery info
// ============================================================================



void RegisterNamed(PCWSTR name, FrameworkElement const& element) {
    if (element && name && *name) {
        g_namedElements.insert_or_assign(name, element);
    }
}
// Gets the current desktop wallpaper and returns an ImageBrush from it.
// Returns an empty brush if wallpaper is missing or fails to load.
wuxm::ImageBrush GetWallpaperBrush() {
    wuxm::ImageBrush brush;

    wchar_t wallpaperPath[MAX_PATH] = {0};
    if (SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, wallpaperPath, 0) && wallpaperPath[0]) {
        std::wstring uriPath = wallpaperPath;
        std::replace(uriPath.begin(), uriPath.end(), L'\\', L'/');
        // Cache-busting query so WinRT re-decodes when the file content changes
        // at the same path (e.g. TranscodedWallpaper).
        wchar_t suffix[32];
        swprintf_s(suffix, L"?v=%llu", static_cast<unsigned long long>(GetTickCount64()));
        uriPath = L"file:///" + uriPath + suffix;

        try {
            wuxm::Imaging::BitmapImage bitmap;
            bitmap.UriSource(wf::Uri(winrt::hstring(uriPath)));
            brush.ImageSource(bitmap);
            brush.Stretch(wuxm::Stretch::UniformToFill);
            brush.AlignmentX(wuxm::AlignmentX::Left);
            brush.AlignmentY(wuxm::AlignmentY::Top);
        } catch (...) {
        }
    }
    return brush;
}

// Checks if the wallpaper has changed and updates the background
void UpdateWallpaperIfChanged() {
    if (!g_wallpaperLayer) return;

    wchar_t wallpaperPath[MAX_PATH] = {0};
    if (!SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, wallpaperPath, 0) || !wallpaperPath[0]) {
        return;
    }
    std::wstring currentPath = wallpaperPath;
    unsigned long long stamp = 0;
    WIN32_FILE_ATTRIBUTE_DATA fad{};
    if (GetFileAttributesExW(currentPath.c_str(), GetFileExInfoStandard, &fad)) {
        stamp = (static_cast<unsigned long long>(fad.ftLastWriteTime.dwHighDateTime) << 32) |
                fad.ftLastWriteTime.dwLowDateTime;
    }
    static std::wstring s_lastPath;
    static unsigned long long s_lastStamp = 0;
    if (currentPath != s_lastPath || stamp != s_lastStamp) {
        s_lastPath = currentPath;
        s_lastStamp = stamp;
        g_wallpaperLayer.Background(GetWallpaperBrush());
        Wh_Log(L"TopBar: Wallpaper updated: %s", currentPath.c_str());
    }
}
// Wheel over the Display and Sound buttons adjusts brightness and volume in
// place. Both read through the cached fast path so a fast scroll doesn't queue
// up a WMI query or a device activation per notch.
void RefreshDisplayButtonIcon() {
    if (!g_displayButton) return;
    auto icon = BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6);
    g_displayButton.Content(icon);
}

void ShowVolumePercent(int percent) {
    if (!g_soundButton) return;
    auto text = MakeText(nullptr, std::to_wstring(percent) + L"%", 16, true);
    text.HorizontalAlignment(HorizontalAlignment::Center);
    text.VerticalAlignment(VerticalAlignment::Center);
    g_soundButton.Content(text);
}

void ShowBrightnessPercent(int percent) {
    if (!g_displayButton) return;
    auto text = MakeText(nullptr, std::to_wstring(percent) + L"%", 16, true);
    text.HorizontalAlignment(HorizontalAlignment::Center);
    text.VerticalAlignment(VerticalAlignment::Center);
    g_displayButton.Content(text);
}

void AttachWheelHandler(wuxc::Button const& button, bool isVolume) {
    button.PointerWheelChanged(
        [isVolume](wf::IInspectable const& sender, Input::PointerRoutedEventArgs const& args) {
            try {
                auto point = args.GetCurrentPoint(sender.as<UIElement>());
                int delta = point.Properties().MouseWheelDelta();
                if (delta == 0) return;

                int step = isVolume ? 6 : 4;
                int direction = delta > 0 ? 1 : -1;

                if (isVolume) {
                    int newVolume = std::clamp(audio::GetMasterVolume() + step * direction, 0, 100);
                    SetMasterVolumeCoalesced(newVolume);
                    ShowVolumePercent(newVolume);

                    if (!g_volumeRevertTimer) {
                        g_volumeRevertTimer = DispatcherTimer();
                        g_volumeRevertTimer.Interval(std::chrono::milliseconds(700));
                        g_volumeRevertTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
                            g_volumeRevertTimer.Stop();
                            RefreshSoundButtonIcon();
                        });
                    }
                    g_volumeRevertTimer.Stop();
                    g_volumeRevertTimer.Start();
                } else {
                    int newBrightness = std::clamp(brightness::GetFast() + step * direction, 0, 100);
                    SetBrightnessCoalesced(newBrightness);
                    ShowBrightnessPercent(newBrightness);

                    if (!g_brightnessRevertTimer) {
                        g_brightnessRevertTimer = DispatcherTimer();
                        g_brightnessRevertTimer.Interval(std::chrono::milliseconds(700));
                        g_brightnessRevertTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
                            g_brightnessRevertTimer.Stop();
                            RefreshDisplayButtonIcon();
                        });
                    }
                    g_brightnessRevertTimer.Stop();
                    g_brightnessRevertTimer.Start();
                }

                args.Handled(true);
            } catch (...) {}
        });
}

// One of the five control-centre buttons. They share a size, a corner radius
// and a flyout shape so the right-hand cluster reads as a single unit.
wuxc::Button MakeControlButton(PCWSTR name,
                               FrameworkElement icon,
                               wuxc::Flyout const& flyout,
                               const std::function<void()>& onOpening) {
    auto button = MakeGhostButton(name, g_settings.cornerRadius);
    button.Content(icon);
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.Margin(Thickness{5, 4, 5, 4});
    button.Padding(Thickness{7, 0, 7, 0});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    if (flyout) {
        button.Flyout(flyout);
        if (onOpening) {
            // Populate on open rather than up front: enumerating devices,
            // sessions and tray icons is far too expensive to do on every
            // settings change.
            flyout.Opening([onOpening](auto&&, auto&&) {
                try {
                    onOpening();
                    // Apply styles to any dynamically created elements.
                    ApplyAllControlStyles();
                } catch (...) {
                }
            });
        }
    }
    RegisterNamed(name, button);
    return button;
}
std::wstring ReadTrayOrder() {
    std::vector<wchar_t> buffer(1024);
    size_t length = Wh_GetStringValue(L"trayOrder", buffer.data(), buffer.size());
    return std::wstring(buffer.data(), length);
}

void WriteTrayOrder(const std::wstring& value) {
    Wh_SetStringValue(L"trayOrder", value.c_str());
}

void ApplyTrayOrderToPanel() {
    if (!g_trayPanel) {
        return;
    }
    std::map<std::wstring, UIElement> byName;
    for (auto&& child : g_trayPanel.Children()) {
        if (auto fe = child.try_as<FrameworkElement>()) {
            std::wstring childName(fe.Name());
            if (!childName.empty()) {
                byName.insert_or_assign(childName, child);
            }
        }
    }
    g_trayPanel.Children().Clear();
    for (const auto& childName : g_trayOrder) {
        auto found = byName.find(childName);
        if (found != byName.end()) {
            g_trayPanel.Children().Append(found->second);
            byName.erase(found);
        }
    }
    for (auto& leftover : byName) {
        g_trayPanel.Children().Append(leftover.second);
    }
}

// Forward declarations — the real definitions live further down, next to
// PopulateSettingsPanel.
std::vector<std::wstring> SplitItemList(const std::wstring& csv);
static void InsertCanonical(std::wstring& csv, const std::wstring& item);

// Items that must stay adjacent and ordered as one block. A member of a
// group can only swap with another member of the SAME group; an item
// outside the group can hop OVER the block but never land inside it.
static const std::vector<std::vector<std::wstring>> kAtomicGroups = {
    { L"StartButton", L"SearchButton" },
    { L"DisplayButton", L"SoundButton", L"WifiButton",
      L"BluetoothButton", L"BatteryButton", L"RecycleBinButton" },
};

static int FindAtomicGroupOf(const std::wstring& item) {
    for (size_t gi = 0; gi < kAtomicGroups.size(); gi++) {
        for (const auto& m : kAtomicGroups[gi]) {
            if (m == item) return static_cast<int>(gi);
        }
    }
    return -1;
}

// Which panel is this element currently placed in? Reads storage, matches
// BuildTopBarContent()'s placement pass.
static const wchar_t* GetPanelOfItem(const std::wstring& item) {
    auto contains = [&](const std::wstring& csv) {
        auto v = SplitItemList(csv);
        return std::find(v.begin(), v.end(), item) != v.end();
    };
    if (contains(g_settings.leftItems))   return L"left";
    if (contains(g_settings.centerItems)) return L"center";
    if (contains(g_settings.rightItems))  return L"right";
    return nullptr;
}

// Right-click Move Left/Right. Strictly within-panel — use the UI Alignment
// page to move items between panels.
//
//  * Grouped item (CC block / Start+Search): only swaps with the IMMEDIATELY
//    adjacent member of the same group. Blocked otherwise.
//  * Ungrouped item (Weather, Resource, Clock, Settings): skips over any
//    atomic-group run in the requested direction and swaps with the next
//    ungrouped item found. Never lands inside a group.
void MoveItemWithinPanels(const std::wstring& item, int direction) {
    const wchar_t* panelName = GetPanelOfItem(item);
    if (!panelName) return;

    std::wstring* csv = nullptr;
    if (wcscmp(panelName, L"left") == 0)        csv = &g_settings.leftItems;
    else if (wcscmp(panelName, L"center") == 0) csv = &g_settings.centerItems;
    else if (wcscmp(panelName, L"right") == 0)  csv = &g_settings.rightItems;
    if (!csv) return;

    auto list = SplitItemList(*csv);
    auto it = std::find(list.begin(), list.end(), item);
    if (it == list.end()) return;
    int idx = static_cast<int>(it - list.begin());
    int n   = static_cast<int>(list.size());

    int myGroup = FindAtomicGroupOf(item);

    if (myGroup >= 0) {
        // Grouped item: only swap with an immediately-adjacent member of
        // the same group. Keeps the group contiguous.
        int target = idx + direction;
        if (target < 0 || target >= n) return;
        if (FindAtomicGroupOf(list[target]) != myGroup) return;
        std::swap(list[idx], list[target]);
    } else {
        // Ungrouped item: hop over any adjacent atomic group and swap with
        // whatever sits on the other side. If we run off the edge of the
        // panel while skipping groups, clamp to the boundary — the item
        // still jumps cleanly past the block.
        if (direction < 0) {
            if (idx == 0) return;
            int target = idx - 1;
            while (target >= 0) {
                int g = FindAtomicGroupOf(list[target]);
                if (g < 0) break;
                while (target >= 0 && FindAtomicGroupOf(list[target]) == g) target--;
            }
            if (target < 0) target = 0;
            if (target >= idx) return;
            std::wstring moved = list[idx];
            for (int k = idx; k > target; k--) list[k] = list[k - 1];
            list[target] = moved;
        } else {
            if (idx == n - 1) return;
            int target = idx + 1;
            while (target < n) {
                int g = FindAtomicGroupOf(list[target]);
                if (g < 0) break;
                while (target < n && FindAtomicGroupOf(list[target]) == g) target++;
            }
            if (target >= n) target = n - 1;
            if (target <= idx) return;
            std::wstring moved = list[idx];
            for (int k = idx; k < target; k++) list[k] = list[k + 1];
            list[target] = moved;
        }
    }

    std::wstring out;
    for (int i = 0; i < n; i++) { if (i) out += L","; out += list[i]; }
    *csv = out;

    Wh_SetStringValue(L"leftItems",   g_settings.leftItems.c_str());
    Wh_SetStringValue(L"centerItems", g_settings.centerItems.c_str());
    Wh_SetStringValue(L"rightItems",  g_settings.rightItems.c_str());

    RunOnUiThread([] {
        if (!g_topBarHwnd) return;
        LoadSettings();
        g_dpiScale = GetBarDpiScale();
        g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);
        auto content = BuildTopBarContent();
        g_desktopSource.Content(content);
        BuildStartContextMenu();
        BuildTaskContextMenu();
        ApplyAllControlStyles();
        ApplyVisibilitySettings();
        UpdateResourceButton();
        StripInheritedIslandBackgrounds();
        PositionAppBar(g_topBarHwnd, g_barHeightPx);
        RefreshTaskList(true);
    });
}

// Right-click menu for every movable topbar item. Two items on one row with
// arrow glyphs; hops across panels when the item is already at the edge.
void AttachTrayReorderMenu(wuxc::Button const& button, std::wstring name) {
    button.RightTapped([name](wf::IInspectable const& sender,
                              Input::RightTappedRoutedEventArgs const& args) {
        try {
            args.Handled(true);
            auto fe = sender.as<FrameworkElement>();

            auto makeBtn = [](bool isLeft) {
                auto b = MakeGhostButton(nullptr, 4);
                b.MinWidth(110);
                b.Padding(Thickness{ 14, 10, 14, 10 });
                b.HorizontalAlignment(HorizontalAlignment::Stretch);
                b.HorizontalContentAlignment(HorizontalAlignment::Center);
                b.Background(MakeBrush(0x30, 0xFF, 0xFF, 0xFF));   // translucent, sits over blur

                wuxc::StackPanel c;
                c.Orientation(wuxc::Orientation::Horizontal);
                c.Spacing(8);
                c.VerticalAlignment(VerticalAlignment::Center);
                c.HorizontalAlignment(HorizontalAlignment::Center);

                if (isLeft) {
                    wuxc::FontIcon fi; fi.Glyph(L"\uE76B"); fi.FontSize(14);
                    fi.Foreground(MakeBrush(0xFF, 0xFF, 0xFF, 0xFF));
                    c.Children().Append(fi);
                }
                auto t = MakeText(nullptr, isLeft ? L"Left" : L"Right", 13);
                t.Foreground(MakeBrush(0xFF, 0xFF, 0xFF, 0xFF));
                c.Children().Append(t);
                if (!isLeft) {
                    wuxc::FontIcon fi; fi.Glyph(L"\uE76C"); fi.FontSize(14);
                    fi.Foreground(MakeBrush(0xFF, 0xFF, 0xFF, 0xFF));
                    c.Children().Append(fi);
                }
                b.Content(c);
                return b;
            };

            auto leftBtn = makeBtn(true);
            auto rightBtn = makeBtn(false);

            auto rowGrid = wuxc::Grid();
            wuxc::ColumnDefinition c1; c1.Width(GridLength{ 1, GridUnitType::Star });
            wuxc::ColumnDefinition c2; c2.Width(GridLength{ 1, GridUnitType::Star });
            rowGrid.ColumnDefinitions().Append(c1);
            rowGrid.ColumnDefinitions().Append(c2);
            rowGrid.ColumnSpacing(6);
            wuxc::Grid::SetColumn(leftBtn, 0);
            wuxc::Grid::SetColumn(rightBtn, 1);
            rowGrid.Children().Append(leftBtn);
            rowGrid.Children().Append(rightBtn);

            auto wrap = wuxc::Border();
            // Low-alpha tint so the DWM blur underneath shows through — the
            // popup host HWND gets ACCENT_ENABLE_BLURBEHIND applied on open.
            wrap.Background(MakeBrush(0x40, 0x20, 0x20, 0x20));
            wrap.BorderBrush(MakeBrush(0x30, 0xFF, 0xFF, 0xFF));
            wrap.BorderThickness(Thickness{ 1, 1, 1, 1 });
            wrap.CornerRadius(MakeCorner(8));
            wrap.Padding(Thickness{ 6, 6, 6, 6 });
            wrap.Child(rowGrid);

            auto flyout = wuxc::Flyout();
            flyout.Content(wrap);
            flyout.ShouldConstrainToRootBounds(false);
            {
                Style fpStyle(winrt::xaml_typename<wuxc::FlyoutPresenter>());
                fpStyle.Setters().Append(Setter(wuxc::Control::BackgroundProperty(),
                    winrt::box_value(MakeBrush(0x00, 0, 0, 0))));
                fpStyle.Setters().Append(Setter(wuxc::Control::BorderThicknessProperty(),
                    winrt::box_value(Thickness{ 0, 0, 0, 0 })));
                fpStyle.Setters().Append(Setter(wuxc::Control::PaddingProperty(),
                    winrt::box_value(Thickness{ 0, 0, 0, 0 })));
                if (auto tmpl = BuildFlyoutShellTemplate(false)) {
                    fpStyle.Setters().Append(Setter(wuxc::Control::TemplateProperty(),
                        winrt::box_value(tmpl)));
                }
                flyout.FlyoutPresenterStyle(fpStyle);
            }
            {
                auto wrapRef = wrap;
                flyout.Opened([wrapRef](auto&&, auto&&) {
                    try {
                        DependencyObject current = wrapRef;
                        while (auto parent = wuxm::VisualTreeHelper::GetParent(current)) {
                            if (auto presenter = parent.try_as<wuxc::FlyoutPresenter>()) {
                                wui::Color tintColor;
                                if (!ParseBarColor(g_settings.topBarBackgroundColor,
                                                   g_settings.topBarBackgroundOpacity,
                                                   &tintColor)) {
                                    tintColor = wui::ColorHelper::FromArgb(128, 0, 0, 0);
                                }
                                auto blurBrush = winrt::make<XamlBlurBrush>(
                                    presenter.as<UIElement>(),
                                    static_cast<float>(g_settings.globalBlurAmount),
                                    tintColor, tintColor.A);
                                presenter.Background(blurBrush);
                                break;
                            }
                            current = parent;
                        }
                    } catch (...) {}
                    ApplyBlurToAllOpenPopups();
                });
            }

            auto flyoutRef = std::make_shared<wuxc::Flyout>(flyout);
            leftBtn.Click([name, flyoutRef](auto&&, auto&&) {
                MoveItemWithinPanels(name, -1);
                try { flyoutRef->Hide(); } catch (...) {}
            });
            rightBtn.Click([name, flyoutRef](auto&&, auto&&) {
                MoveItemWithinPanels(name, +1);
                try { flyoutRef->Hide(); } catch (...) {}
            });

            wuxc::Primitives::FlyoutShowOptions opts;
            opts.Position(args.GetPosition(fe));
            opts.Placement(wuxc::Primitives::FlyoutPlacementMode::Bottom);
            flyout.ShowAt(fe, opts);
        } catch (...) {
        }
    });
}

void EnsureAutoRefreshTimers() {
    if (!g_wifiAutoRefreshTimer) {
        g_wifiAutoRefreshTimer = DispatcherTimer();
        g_wifiAutoRefreshTimer.Interval(std::chrono::seconds(30));
        g_wifiAutoRefreshTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            if (!g_wifiFlyout || !g_wifiFlyout.IsOpen()) {
                g_wifiAutoRefreshTimer.Stop();
                return;
            }
            // Don't refresh while typing password, otherwise it clears the box!
            if (g_wifiPasswordPrompt) {
                return;
            }
            if (!g_wifiScanning) {
                StartWifiScan();
            }
        });
    }
    if (!g_bluetoothAutoRefreshTimer) {
        g_bluetoothAutoRefreshTimer = DispatcherTimer();
        g_bluetoothAutoRefreshTimer.Interval(std::chrono::seconds(15)); // more frequent
        g_bluetoothAutoRefreshTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            if (!g_bluetoothFlyout || !g_bluetoothFlyout.IsOpen()) {
                g_bluetoothAutoRefreshTimer.Stop();
                return;
            }
            if (!g_bluetoothScanning) {
                StartBluetoothScan(true);
            }
        });
    }
}



std::vector<std::wstring> SplitItemList(const std::wstring& csv) {
    std::vector<std::wstring> out;
    size_t pos = 0;
    while (pos <= csv.size()) {
        size_t comma = csv.find(L',', pos);
        std::wstring token = TrimWs(csv.substr(
            pos, comma == std::wstring::npos ? std::wstring::npos : comma - pos));
        if (!token.empty()) out.push_back(token);
        if (comma == std::wstring::npos) break;
        pos = comma + 1;
    }
    return out;
}

void RemoveItemFromAll(std::wstring& csv, const std::wstring& item) {
    auto items = SplitItemList(csv);
    std::wstring result;
    for (auto& s : items) {
        if (s == item) continue;
        if (!result.empty()) result += L",";
        result += s;
    }
    csv = result;
}

std::wstring FindSectionOf(const std::wstring& item) {
    auto has = [](const std::wstring& csv, const std::wstring& it) {
        auto v = SplitItemList(csv);
        return std::find(v.begin(), v.end(), it) != v.end();
    };
    if (has(g_settings.leftItems, item))   return L"left";
    if (has(g_settings.centerItems, item)) return L"center";
    if (has(g_settings.rightItems, item))  return L"right";
    return L"";
}

void ReorderInSection(const std::wstring& section, const std::wstring& item, int direction) {
    std::wstring* target = nullptr;
    if (section == L"left")        target = &g_settings.leftItems;
    else if (section == L"center") target = &g_settings.centerItems;
    else if (section == L"right")  target = &g_settings.rightItems;
    if (!target) return;

    auto list = SplitItemList(*target);
    auto it = std::find(list.begin(), list.end(), item);
    if (it == list.end()) return;

    int idx = static_cast<int>(it - list.begin());
    int newIdx = idx + direction;
    if (newIdx < 0 || newIdx >= static_cast<int>(list.size())) return;
    std::swap(list[idx], list[newIdx]);

    std::wstring rebuilt;
    for (auto& s : list) {
        if (!rebuilt.empty()) rebuilt += L",";
        rebuilt += s;
    }
    *target = rebuilt;
    Wh_SetStringValue(L"leftItems",   g_settings.leftItems.c_str());
    Wh_SetStringValue(L"centerItems", g_settings.centerItems.c_str());
    Wh_SetStringValue(L"rightItems",  g_settings.rightItems.c_str());
}

// Canonical left-to-right priority per item, so when an item returns to a
// panel it lands where the user expects (Start/Search leftmost, Clock/Weather
// rightmost, ...) instead of at the far edge.
static int CanonicalItemRank(const std::wstring& item) {
    static const wchar_t* kOrder[] = {
        L"StartButton", L"SearchButton",
        L"ResourceButton", L"WeatherButton",
        L"DisplayButton", L"SoundButton", L"WifiButton",
        L"BluetoothButton", L"BatteryButton", L"RecycleBinButton",
        L"ClockButton", L"SettingsButton",
    };
    for (size_t i = 0; i < ARRAYSIZE(kOrder); i++) {
        if (item == kOrder[i]) return static_cast<int>(i);
    }
    return 1000;   // unknown items go last
}

// Insert `item` into a comma-separated list at the position that keeps
// canonical order — used when an item is added to a panel via UI Alignment.
static void InsertCanonical(std::wstring& csv, const std::wstring& item) {
    auto list = SplitItemList(csv);
    int rank = CanonicalItemRank(item);
    size_t pos = list.size();
    for (size_t i = 0; i < list.size(); i++) {
        if (CanonicalItemRank(list[i]) > rank) { pos = i; break; }
    }
    list.insert(list.begin() + pos, item);
    std::wstring out;
    for (size_t i = 0; i < list.size(); i++) {
        if (i) out += L",";
        out += list[i];
    }
    csv = out;
}

void MoveItemToSection(const std::wstring& item, const std::wstring& section) {
    RemoveItemFromAll(g_settings.leftItems, item);
    RemoveItemFromAll(g_settings.centerItems, item);
    RemoveItemFromAll(g_settings.rightItems, item);
    if (section == L"left") {
        InsertCanonical(g_settings.leftItems, item);
    } else if (section == L"center") {
        InsertCanonical(g_settings.centerItems, item);
    } else if (section == L"right") {
        InsertCanonical(g_settings.rightItems, item);
    }
    Wh_SetStringValue(L"leftItems", g_settings.leftItems.c_str());
    Wh_SetStringValue(L"centerItems", g_settings.centerItems.c_str());
    Wh_SetStringValue(L"rightItems", g_settings.rightItems.c_str());
}

void PopulateSettingsPanel() {
    if (!g_settingsPanel) return;

    g_populatingPanel = true;
    struct Guard {
        ~Guard() { g_populatingPanel = false; }
    } guard;

    auto children = g_settingsPanel.Children();
    children.Clear();

    children.Append(MakePanelTitle(L"TopBar Settings"));
    children.Append(MakeStatusText(L"Tap L / C / R to move whole groups."));

    struct SettingsGroup {
        std::wstring label;
        std::vector<std::wstring> members;
    };

    const std::vector<SettingsGroup> groups = {
        {L"Start",          {L"StartButton"}},
        {L"Search",         {L"SearchButton"}},
        {L"Resources",      {L"ResourceButton"}},
        {L"Weather",        {L"WeatherButton"}},
        {L"Control Center", {L"DisplayButton", L"SoundButton", L"WifiButton",
                             L"BluetoothButton", L"BatteryButton",
                             L"RecycleBinButton", L"SettingsButton"}},
        {L"Clock",          {L"ClockButton"}},
    };

    auto allInSection = [](const std::vector<std::wstring>& members,
                           const std::wstring& csv) {
        auto list = SplitItemList(csv);
        for (const auto& m : members) {
            if (std::find(list.begin(), list.end(), m) == list.end()) return false;
        }
        return true;
    };

    for (const auto& g : groups) {
        wuxc::Grid row;
        row.ColumnSpacing(3);
        row.Margin(Thickness{0, 4, 0, 4});

        wuxc::ColumnDefinition labelCol;
        labelCol.Width(GridLength{1, GridUnitType::Star});
        row.ColumnDefinitions().Append(labelCol);
        for (int i = 0; i < 3; i++) {
            wuxc::ColumnDefinition col;
            col.Width(GridLength{0, GridUnitType::Auto});
            row.ColumnDefinitions().Append(col);
        }

        auto labelTb = MakeText(nullptr, g.label, 13);
        labelTb.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(labelTb, 0);
        row.Children().Append(labelTb);

        std::wstring current;
        if      (allInSection(g.members, g_settings.leftItems))   current = L"left";
        else if (allInSection(g.members, g_settings.centerItems)) current = L"center";
        else if (allInSection(g.members, g_settings.rightItems))  current = L"right";

        const wchar_t* btnLabels[3] = {L"L", L"C", L"R"};
        const wchar_t* btnIds[3]    = {L"left", L"center", L"right"};

        for (int i = 0; i < 3; i++) {
            auto btn = MakeGhostButton(nullptr, kRowCorner);
            btn.Width(32);
            btn.Padding(Thickness{0, 4, 0, 4});
            btn.HorizontalContentAlignment(HorizontalAlignment::Center);
            btn.Content(MakeText(nullptr, btnLabels[i], 12, true));

            if (current == btnIds[i]) {
                auto a = GetSystemAccentColor();
                btn.Background(MakeBrush(0xFF, a.R, a.G, a.B));
            } else {
                btn.Background(MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
            }

            std::vector<std::wstring> membersCopy = g.members;
            std::wstring targetCopy = btnIds[i];
            btn.Click([membersCopy, targetCopy](auto&&, auto&&) {
                for (const auto& m : membersCopy) {
                    MoveItemToSection(m, targetCopy);
                }
                RepopulateLater(PopulateSettingsPanel);
            });
            wuxc::Grid::SetColumn(btn, i + 1);
            row.Children().Append(btn);
        }

        children.Append(row);
    }

    children.Append(MakeDivider());

    auto applyBtn = MakeGhostButton(L"ApplyLayoutButton", kRowCorner);
    applyBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
    applyBtn.Padding(Thickness{10, 8, 10, 8});
    applyBtn.Content(MakeText(nullptr, L"Apply layout now", 12));
    applyBtn.Click([](auto&&, auto&&) {
        auto content = BuildTopBarContent();
        g_desktopSource.Content(content);
        ApplyAllControlStyles();
        ApplyVisibilitySettings();
        StripInheritedIslandBackgrounds();
        RepopulateLater(PopulateSettingsPanel);
    });
    children.Append(applyBtn);

    auto resetBtn = MakeGhostButton(L"ResetLayoutButton", kRowCorner);
    resetBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
    resetBtn.Padding(Thickness{10, 8, 10, 8});
    resetBtn.Content(MakeText(nullptr, L"Reset to defaults", 12));
    resetBtn.Click([](auto&&, auto&&) {
        g_settings.leftItems   = L"StartButton,SearchButton";
        g_settings.centerItems = L"ResourceButton,WeatherButton,SettingsButton";
        g_settings.rightItems  = L"DisplayButton,SoundButton,WifiButton,BluetoothButton,"
                                 L"BatteryButton,RecycleBinButton,ClockButton";
        Wh_SetStringValue(L"leftItems",   g_settings.leftItems.c_str());
        Wh_SetStringValue(L"centerItems", g_settings.centerItems.c_str());
        Wh_SetStringValue(L"rightItems",  g_settings.rightItems.c_str());

        auto content = BuildTopBarContent();
        g_desktopSource.Content(content);
        ApplyAllControlStyles();
        ApplyVisibilitySettings();
        StripInheritedIslandBackgrounds();
        RepopulateLater(PopulateSettingsPanel);
    });
    children.Append(resetBtn);

    ApplyAllControlStyles();
}

namespace topbar_settings_window {

namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wux  = winrt::Windows::UI::Xaml;
namespace wut  = winrt::Windows::UI::Text;

static HWND s_hwnd = nullptr;
static HWND s_islandHwnd = nullptr;
[[clang::no_destroy]] static wuxh::DesktopWindowXamlSource s_xamlSource{ nullptr };
[[clang::no_destroy]] static winrt::com_ptr<IDesktopWindowXamlSourceNative2> s_native2;
[[clang::no_destroy]] static wuxc::StackPanel s_contentPanel{ nullptr };
static int s_currentPage = 0;
[[clang::no_destroy]] static wux::DispatcherTimer s_applyTimer{ nullptr };
static const wchar_t* kClassName = L"TopBarSettingsWnd";

wuxm::SolidColorBrush MakeSolid(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush(wui::ColorHelper::FromArgb(a, r, g, b));
}
wuxc::TextBlock MakeLabel(const std::wstring& text, double size = 13, bool bold = false, double opacity = 1.0) {
    wuxc::TextBlock tb;
    tb.Text(winrt::hstring(text));
    tb.FontSize(size);
    if (bold) tb.FontWeight(wut::FontWeights::SemiBold());
    tb.Opacity(opacity);
    tb.VerticalAlignment(VerticalAlignment::Center);
    return tb;
}

void ScheduleReload() {
    if (!s_applyTimer) {
        s_applyTimer = wux::DispatcherTimer();
        s_applyTimer.Interval(std::chrono::milliseconds(250));
        s_applyTimer.Tick([](auto&&, auto&&) {
            s_applyTimer.Stop();
            if (!g_topBarHwnd || !g_desktopSource) return;
            LoadSettings();
            g_dpiScale = GetBarDpiScale();
            g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);
            InstallGlobalMenuResources();
            auto content = BuildTopBarContent();
            g_desktopSource.Content(content);
            BuildStartContextMenu();
            BuildTaskContextMenu();
            ApplyAllControlStyles();
            ApplyVisibilitySettings();
            UpdateResourceButton();
            StripInheritedIslandBackgrounds();
            PositionAppBar(g_topBarHwnd, g_barHeightPx);
            RefreshTaskList(true);
        });
    }
    s_applyTimer.Stop();
    s_applyTimer.Start();
}

void WriteInt(PCWSTR key, int value) {
    Wh_SetStringValue(key, std::to_wstring(value).c_str());
    ScheduleReload();
}
void WriteStr(PCWSTR key, const std::wstring& value) {
    Wh_SetStringValue(key, value.c_str());
    ScheduleReload();
}

wuxc::Border MakeCard() {
    wuxc::Border b;
    b.CornerRadius(CornerRadius{ 8, 8, 8, 8 });
    b.Background(MakeSolid(0x20, 0xFF, 0xFF, 0xFF));   // semi-transparent so blur shows through
    b.BorderBrush(MakeSolid(0x28, 0xFF, 0xFF, 0xFF));
    b.BorderThickness(Thickness{ 1, 1, 1, 1 });
    b.Margin(Thickness{ 0, 3, 0, 3 });
    return b;
}

wuxc::Grid MakeRowShell(const std::wstring& label, FrameworkElement control,
                        const std::wstring& desc = L"") {
    wuxc::Grid g;
    g.Padding(Thickness{ 14, 10, 14, 10 });
    wuxc::ColumnDefinition c1;
    c1.Width(GridLength{ 1, GridUnitType::Star });
    wuxc::ColumnDefinition c2;
    c2.Width(GridLength{ 0, GridUnitType::Auto });
    g.ColumnDefinitions().Append(c1);
    g.ColumnDefinitions().Append(c2);

    wuxc::StackPanel left;
    left.VerticalAlignment(VerticalAlignment::Center);
    left.Children().Append(MakeLabel(label, 13));
    if (!desc.empty()) left.Children().Append(MakeLabel(desc, 11, false, 0.55));
    wuxc::Grid::SetColumn(left, 0);
    g.Children().Append(left);

    if (control) {
        control.VerticalAlignment(VerticalAlignment::Center);
        wuxc::Grid::SetColumn(control, 1);
        g.Children().Append(control);
    }
    return g;
}

// Forward declarations — the actual definitions live further down the
// namespace, right above AddTextRow.
static void StyleLightTextBox(wuxc::TextBox const& tb);
static void StyleLightComboBox(wuxc::ComboBox const& cb);

// Non-zero while at least one color-picker flyout is open. ScheduleReload is
// deferred while this is set: a rebuild tears down every open flyout,
// including the one whose slider is currently dragging — that orphans the
// OS pointer capture and the slider keeps tracking the cursor even after
// the user releases the button.
static std::atomic<int> g_openColorPickers{0};
static std::atomic<bool> g_pendingColorReload{false};

static void RequestColorReload() {
    if (g_openColorPickers.load() > 0) {
        g_pendingColorReload = true;
    } else {
        ScheduleReload();
    }
}

// Immediate topbar background apply — bypasses ScheduleReload's debounce so
// the change is visible on the live bar the instant the user picks a colour.
static void ApplyTopBarBackgroundNow() {
    std::wstring colorStr = GetStringSettingCopy(L"topBarBackgroundColor");
    if (colorStr.empty()) colorStr = L"#000000";
    int opacity = ReadIntSetting(L"topBarBackgroundOpacity", 50);

    wui::Color tintColor;
    if (!ParseBarColor(colorStr, opacity, &tintColor)) {
        tintColor = wui::ColorHelper::FromArgb(128, 0, 0, 0);
    }
    auto it = g_namedElements.find(L"TopBarRoot");
    if (it == g_namedElements.end()) return;
    if (auto grid = it->second.try_as<wuxc::Grid>()) {
        grid.Background(wuxm::SolidColorBrush(tintColor));
    }
}

void AddColorRow(wuxc::StackPanel& panel, const std::wstring& label, PCWSTR key,
                 const std::wstring& desc = L"") {
    std::wstring currentText = GetStringSettingCopy(key);
    if (currentText.empty()) {
        auto it = StringDefaults().find(key);
        if (it != StringDefaults().end()) currentText = it->second;
    }
    wui::Color current = wui::ColorHelper::FromArgb(255, 0, 0, 0);
    if (!TryParseHexColor(currentText, &current)) {
        TryParseNamedColor(currentText, &current);
    }
    std::wstring keyStr = key;

    // Rounded swatch — 8px corner on a 28×28 box reads clearly as rounded.
    auto swatch = MakeGhostButton(nullptr, 8);
    swatch.Width(28);
    swatch.Height(28);
    swatch.MinWidth(28);
    swatch.Padding(Thickness{ 0, 0, 0, 0 });
    swatch.Background(wuxm::SolidColorBrush(current));

    // Editable hex textbox, sits to the right of the swatch.
    wuxc::TextBox tb;
    tb.Text(currentText);
    tb.MinWidth(120);
    tb.MaxWidth(180);
    tb.CornerRadius(CornerRadius{ 4, 4, 4, 4 });
    StyleLightTextBox(tb);

    // Shared state — the "current" colour, kept in sync across picker,
    // slider, and textbox. All three write through the same commit path.
    auto curColor = std::make_shared<wui::Color>(current);
    auto commitColor = [keyStr, swatch, tb, curColor](const wui::Color& c) {
        *curColor = c;
        wchar_t buf[16];
        swprintf_s(buf, L"#%02X%02X%02X", c.R, c.G, c.B);
        std::wstring hex = buf;
        swatch.Background(wuxm::SolidColorBrush(c));
        tb.Text(hex);
        Wh_SetStringValue(keyStr.c_str(), hex.c_str());
        // Bugfix: previously the picker only wrote storage + ScheduleReload,
        // which (a) debounces for 250 ms and (b) triggers a full topbar
        // rebuild — neither of which the user perceives as immediate. Direct
        // apply for topBarBackgroundColor makes the change visible instantly.
        if (keyStr == L"topBarBackgroundColor") {
            ApplyTopBarBackgroundNow();
        }
        RequestColorReload();
    };

    // --- Compact dark ColorPicker ----------------------------------------
    // Wheel only (no RGB channel inputs, no built-in value slider — we add
    // our own lightness slider below, which handles both directions).
    wuxc::ColorPicker picker;
    picker.IsAlphaEnabled(false);
    picker.IsAlphaSliderVisible(false);
    picker.IsAlphaTextInputVisible(false);
    // The picker's main square is Hue × Saturation — it can only go toward
    // white. Darkness is a SEPARATE Value slider; keep it visible or black
    // is unreachable.
    picker.IsColorSliderVisible(true);
    picker.IsColorChannelTextInputVisible(false);
    picker.IsHexInputVisible(true);
    picker.IsMoreButtonVisible(false);
    picker.RequestedTheme(wux::ElementTheme::Dark);
    picker.Width(280);
    picker.Color(current);

    // Recursively release every pointer capture held anywhere in a subtree.
    // Called when the picker's internal slider got a PointerPressed but its
    // PointerReleased was delivered to the wrong HWND (cursor released
    // outside the popup) — without this the slider keeps tracking the
    // cursor forever.
    auto releaseCapturesRecursive = std::make_shared<std::function<void(wux::UIElement const&)>>();
    *releaseCapturesRecursive = [releaseCapturesRecursive](wux::UIElement const& node) {
        if (!node) return;
        try { node.ReleasePointerCaptures(); } catch (...) {}
        try {
            int n = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < n; i++) {
                if (auto child = wuxm::VisualTreeHelper::GetChild(node, i).try_as<wux::UIElement>()) {
                    (*releaseCapturesRecursive)(child);
                }
            }
        } catch (...) {}
    };

    // Picker → commit. Value comparison, not a guard flag, so a single stray
    // event doesn't drop a real update.
    picker.ColorChanged([picker, curColor, commitColor, releaseCapturesRecursive](auto&&, auto&&) {
        wui::Color c = picker.Color();
        if (c == *curColor) return;
        commitColor(c);
        // If the physical button is no longer held, no capture should be
        // outstanding. Release any that survived a release-outside-the-popup.
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
            (*releaseCapturesRecursive)(picker);
        }
    });

    // Same cleanup on any pointer movement over the picker — fires even when
    // the child slider is not receiving events because its capture is stuck.
    picker.PointerMoved([picker, releaseCapturesRecursive](
        auto&&, wux::Input::PointerRoutedEventArgs const& e) {
        try {
            auto pp = e.GetCurrentPoint(picker);
            if (!pp.Properties().IsLeftButtonPressed()) {
                (*releaseCapturesRecursive)(picker);
            }
        } catch (...) {}
    });

    // Textbox → picker + commit.
    auto commitHex = [picker, curColor, commitColor](const std::wstring& v) {
        wui::Color c{ 255, 0, 0, 0 };
        if (!TryParseHexColor(v, &c)) {
            if (!TryParseNamedColor(v, &c)) return;
        }
        if (c == *curColor) return;
        if (picker.Color() != c) picker.Color(c);
        commitColor(c);
    };
    tb.LostFocus([commitHex](auto&& sender, auto&&) {
        commitHex(std::wstring{ sender.template as<wuxc::TextBox>().Text() });
    });
    tb.KeyDown([commitHex](auto&& sender, auto&& e) {
        if (e.Key() == winrt::Windows::System::VirtualKey::Enter) {
            commitHex(std::wstring{ sender.template as<wuxc::TextBox>().Text() });
        }
    });

    wuxc::StackPanel pickerColumn;
    pickerColumn.Orientation(wuxc::Orientation::Vertical);
    pickerColumn.Spacing(6);
    pickerColumn.Children().Append(picker);

    wuxc::Border wrap;
    wrap.Background(MakeSolid(0xFF, 0x1F, 0x1F, 0x1F));
    wrap.BorderBrush(MakeSolid(0xFF, 0x40, 0x40, 0x40));
    wrap.BorderThickness(Thickness{ 1, 1, 1, 1 });
    wrap.CornerRadius(CornerRadius{ 8, 8, 8, 8 });
    wrap.Padding(Thickness{ 10, 10, 10, 10 });
    wrap.RequestedTheme(wux::ElementTheme::Dark);
    wrap.Child(pickerColumn);

    wuxc::Flyout flyout;
    flyout.Content(wrap);
    flyout.ShouldConstrainToRootBounds(false);
    {
        Style fpStyle(winrt::xaml_typename<wuxc::FlyoutPresenter>());
        fpStyle.Setters().Append(Setter(wuxc::Control::BackgroundProperty(),
            winrt::box_value(MakeSolid(0x00, 0, 0, 0))));
        fpStyle.Setters().Append(Setter(wuxc::Control::BorderThicknessProperty(),
            winrt::box_value(Thickness{ 0, 0, 0, 0 })));
        fpStyle.Setters().Append(Setter(wuxc::Control::PaddingProperty(),
            winrt::box_value(Thickness{ 0, 0, 0, 0 })));
        flyout.FlyoutPresenterStyle(fpStyle);
    }
    // Track whether a pointer button is currently held inside the picker.
    // Any attempt to close the flyout while this is true is cancelled: a
    // light-dismissal that fires mid-drag orphans the internal slider's OS
    // pointer capture, so it keeps tracking the cursor even after the user
    // releases outside the popup.
    auto pickerDragging = std::make_shared<bool>(false);
    auto markDown = [pickerDragging](auto&&, auto&&) { *pickerDragging = true;  };
    auto markUp   = [pickerDragging](auto&&, auto&&) { *pickerDragging = false; };
    picker.AddHandler(wux::UIElement::PointerPressedEvent(),
        winrt::box_value(wux::Input::PointerEventHandler(markDown)), true);
    picker.AddHandler(wux::UIElement::PointerReleasedEvent(),
        winrt::box_value(wux::Input::PointerEventHandler(markUp)), true);
    picker.AddHandler(wux::UIElement::PointerCaptureLostEvent(),
        winrt::box_value(wux::Input::PointerEventHandler(markUp)), true);

    flyout.Opened([](auto&&, auto&&) {
        g_openColorPickers.fetch_add(1);
        ApplyBlurToAllOpenPopups();
    });
    flyout.Closing([pickerDragging](
        wuxc::Primitives::FlyoutBase const&,
        wuxc::Primitives::FlyoutBaseClosingEventArgs const& e) {
        // Light-dismiss while a slider is being dragged would kill its
        // capture — refuse to close until the button is released.
        if (*pickerDragging) e.Cancel(true);
    });
    flyout.Closed([](auto&&, auto&&) {
        // Only reload once the last open picker closes, so a mid-drag
        // rebuild can't destroy the popup whose slider is still captured.
        if (g_openColorPickers.fetch_sub(1) == 1 &&
            g_pendingColorReload.exchange(false)) {
            ScheduleReload();
        }
    });
    swatch.Flyout(flyout);

    // [■ swatch]  [ hex textbox ]
    wuxc::StackPanel inline_;
    inline_.Orientation(wuxc::Orientation::Horizontal);
    inline_.Spacing(8);
    inline_.VerticalAlignment(VerticalAlignment::Center);
    inline_.Children().Append(swatch);
    inline_.Children().Append(tb);

    auto card = MakeCard();
    card.Child(MakeRowShell(label, inline_, desc));
    panel.Children().Append(card);
}

// Shared styling for every text-style input in this window: light surface,
// dark text — matches the topbar's own flyout textboxes.
static void StyleLightTextBox(wuxc::TextBox const& tb) {
    auto bg   = MakeSolid(0xFF, 0x2D, 0x2D, 0x2D);
    auto bgH  = MakeSolid(0xFF, 0x33, 0x33, 0x33);
    auto bgF  = MakeSolid(0xFF, 0x1F, 0x1F, 0x1F);
    auto fg   = MakeSolid(0xFF, 0xFF, 0xFF, 0xFF);
    auto brd  = MakeSolid(0xFF, 0x55, 0x55, 0x55);
    auto brdH = MakeSolid(0xFF, 0x77, 0x77, 0x77);
    auto brdF = MakeSolid(0xFF, 0x00, 0x78, 0xD4);
    auto ph   = MakeSolid(0xFF, 0x88, 0x88, 0x88);

    tb.Background(bg);
    tb.Foreground(fg);
    tb.BorderBrush(brd);
    tb.PlaceholderForeground(ph);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlBackground")), bg);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlBackgroundPointerOver")), bgH);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlBackgroundFocused")), bgF);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlForeground")), fg);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlForegroundPointerOver")), fg);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlForegroundFocused")), fg);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlBorderBrush")), brd);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlBorderBrushPointerOver")), brdH);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlBorderBrushFocused")), brdF);
    tb.Resources().Insert(winrt::box_value(winrt::hstring(L"TextControlPlaceholderForeground")), ph);
}

static void StyleLightComboBox(wuxc::ComboBox const& cb) {
    auto bg   = MakeSolid(0xFF, 0x2D, 0x2D, 0x2D);
    auto bgH  = MakeSolid(0xFF, 0x33, 0x33, 0x33);
    auto fg   = MakeSolid(0xFF, 0xFF, 0xFF, 0xFF);
    auto brd  = MakeSolid(0xFF, 0x55, 0x55, 0x55);
    auto brdH = MakeSolid(0xFF, 0x77, 0x77, 0x77);
    auto brdF = MakeSolid(0xFF, 0x00, 0x78, 0xD4);
    auto ddBg = MakeSolid(0xFF, 0x20, 0x20, 0x20);

    cb.Background(bg);
    cb.Foreground(fg);
    cb.BorderBrush(brd);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxBackground")), bg);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxBackgroundPointerOver")), bgH);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxBackgroundFocused")), bg);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxForeground")), fg);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxForegroundPointerOver")), fg);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxForegroundFocused")), fg);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxBorderBrush")), brd);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxBorderBrushPointerOver")), brdH);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxBorderBrushFocused")), brdF);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxDropDownBackground")), ddBg);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxItemForeground")), fg);
    cb.Resources().Insert(winrt::box_value(winrt::hstring(L"ComboBoxItemForegroundPointerOver")), fg);
}

void AddTextRow(wuxc::StackPanel& panel, const std::wstring& label, PCWSTR key,
                const std::wstring& desc = L"") {
    std::wstring initial = GetStringSettingCopy(key);
    if (initial.empty()) {
        auto it = StringDefaults().find(key);
        if (it != StringDefaults().end()) initial = it->second;
    }
    wuxc::TextBox tb;
    tb.Text(initial);
    tb.MinWidth(230);
    tb.CornerRadius(CornerRadius{ 4, 4, 4, 4 });
    StyleLightTextBox(tb);
    std::wstring k = key;
    tb.LostFocus([k](auto&& sender, auto&&) {
        auto t = sender.template as<wuxc::TextBox>();
        WriteStr(k.c_str(), std::wstring(t.Text()));
    });
    auto card = MakeCard();
    card.Child(MakeRowShell(label, tb, desc));
    panel.Children().Append(card);
}

void AddIntRow(wuxc::StackPanel& panel, const std::wstring& label, PCWSTR key,
               const std::wstring& desc = L"") {
    int v = ReadIntSetting(key);
    wuxc::TextBox tb;
    tb.Text(std::to_wstring(v));
    tb.MinWidth(90);
    tb.CornerRadius(CornerRadius{ 4, 4, 4, 4 });
    StyleLightTextBox(tb);
    std::wstring k = key;
    tb.LostFocus([k](auto&& sender, auto&&) {
        auto t = sender.template as<wuxc::TextBox>();
        try { WriteInt(k.c_str(), std::stoi(std::wstring(t.Text()))); } catch (...) {}
    });
    auto card = MakeCard();
    card.Child(MakeRowShell(label, tb, desc));
    panel.Children().Append(card);
}

void AddSliderRow(wuxc::StackPanel& panel, const std::wstring& label, PCWSTR key,
                  int lo, int hi, const std::wstring& desc = L"") {
    int current = std::clamp(ReadIntSetting(key), lo, hi);
    wuxc::StackPanel wrap;
    wrap.Orientation(wuxc::Orientation::Horizontal);
    wrap.Spacing(8);
    wrap.VerticalAlignment(VerticalAlignment::Center);

    wuxc::Slider s;
    s.Minimum(lo);
    s.Maximum(hi);
    s.Value(current);
    s.StepFrequency(1);
    s.Width(200);
    s.VerticalAlignment(VerticalAlignment::Center);

    auto readout = MakeLabel(std::to_wstring(current), 13);
    readout.MinWidth(40);
    readout.TextAlignment(TextAlignment::Center);

    std::wstring k = key;
    s.ValueChanged([readout](auto&& sender, auto&&) {
        auto sl = sender.template as<wuxc::Slider>();
        int v = static_cast<int>(sl.Value() + 0.5);
        readout.Text(winrt::hstring(std::to_wstring(v)));
    });

    // --- Phantom-drag fix ------------------------------------------------
    // A XAML island can lose the WM_LBUTTONUP that should end a slider drag
    // when the pointer is released outside the island's HWND: the slider
    // keeps its pointer capture and follows the mouse indefinitely.
    //
    // Fix: track our own drag state and, on every pointer move, check the
    // *physical* left-button state via GetAsyncKeyState. If the button is
    // already up, the release was swallowed somewhere — force-release the
    // capture and commit.
    //
    // Every handler goes through AddHandler(..., handledEventsToo=true).
    // wuxc::Slider's own template marks PointerMoved / PointerReleased as
    // Handled for its thumb logic, and a plain .PointerMoved(handler) never
    // fires because the event has already been consumed by the time it
    // would bubble to us.
    auto dragging = std::make_shared<bool>(false);

    s.AddHandler(UIElement::PointerPressedEvent(), winrt::box_value(
        winrt::Windows::UI::Xaml::Input::PointerEventHandler(
        [dragging](auto const&,
                   winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
            *dragging = true;
        })), true);

    auto endDrag = [k, s, dragging]() mutable {
        if (!*dragging) return;
        *dragging = false;
        try { s.ReleasePointerCaptures(); } catch (...) {}
        WriteInt(k.c_str(), static_cast<int>(s.Value() + 0.5));
    };

    s.AddHandler(UIElement::PointerReleasedEvent(), winrt::box_value(
        winrt::Windows::UI::Xaml::Input::PointerEventHandler(
        [endDrag](auto const&,
                  winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) mutable {
            endDrag();
        })), true);

    s.AddHandler(UIElement::PointerCaptureLostEvent(), winrt::box_value(
        winrt::Windows::UI::Xaml::Input::PointerEventHandler(
        [endDrag](auto const&,
                  winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) mutable {
            endDrag();
        })), true);

    s.AddHandler(UIElement::PointerMovedEvent(), winrt::box_value(
        winrt::Windows::UI::Xaml::Input::PointerEventHandler(
        [dragging, endDrag](auto const&,
                            winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) mutable {
            if (!*dragging) return;
            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
                endDrag();
            }
        })), true);

    wrap.Children().Append(s);
    wrap.Children().Append(readout);
    auto card = MakeCard();
    card.Child(MakeRowShell(label, wrap, desc));
    panel.Children().Append(card);
}

void AddBoolRow(wuxc::StackPanel& panel, const std::wstring& label, PCWSTR key,
                const std::wstring& desc = L"") {
    bool current = ReadIntSetting(key) != 0;
    wuxc::ToggleSwitch t;
    t.OnContent(winrt::box_value(L""));
    t.OffContent(winrt::box_value(L""));
    t.IsOn(current);
    t.MinWidth(0);
    t.Width(50);
    std::wstring k = key;
    t.Toggled([k](auto&& sender, auto&&) {
        auto ts = sender.template as<wuxc::ToggleSwitch>();
        WriteInt(k.c_str(), ts.IsOn() ? 1 : 0);
    });
    auto card = MakeCard();
    card.Child(MakeRowShell(label, t, desc));
    panel.Children().Append(card);
}

void AddComboRow(wuxc::StackPanel& panel, const std::wstring& label, PCWSTR key,
                 const std::vector<std::pair<std::wstring, std::wstring>>& options,
                 const std::wstring& desc = L"") {
    std::wstring current = GetStringSettingCopy(key);
    wuxc::ComboBox cb;
    cb.MinWidth(230);
    StyleLightComboBox(cb);
    int selected = 0;
    for (size_t i = 0; i < options.size(); i++) {
        wuxc::ComboBoxItem item;
        item.Content(winrt::box_value(winrt::hstring(options[i].second)));
        cb.Items().Append(item);
        if (options[i].first == current) selected = static_cast<int>(i);
    }
    cb.SelectedIndex(selected);
    std::wstring k = key;
    auto opts = options;
    cb.SelectionChanged([k, opts](auto&& sender, auto&&) {
        auto c = sender.template as<wuxc::ComboBox>();
        int idx = c.SelectedIndex();
        if (idx >= 0 && idx < static_cast<int>(opts.size()))
            WriteStr(k.c_str(), opts[idx].first);
    });
    auto card = MakeCard();
    card.Child(MakeRowShell(label, cb, desc));
    panel.Children().Append(card);
}

wuxc::TextBlock MakeHeading(const std::wstring& text, double size = 16) {
    auto tb = MakeLabel(text, size, true);
    tb.Margin(Thickness{ 0, 14, 0, 6 });
    return tb;
}

void BuildAppearancePage(wuxc::StackPanel& p) {
    p.Children().Append(MakeHeading(L"Appearance", 22));

    p.Children().Append(MakeHeading(L"Bar"));
    AddSliderRow(p, L"TopBar height (px)", L"barHeight", 20, 120);
    AddSliderRow(p, L"Corner radius", L"cornerRadius", 0, 30);
    AddColorRow(p,  L"Global Background Tint", L"topBarBackgroundColor",
                L"Color of the tint over the wallpaper.");
    AddSliderRow(p, L"Global Tint Opacity", L"topBarBackgroundOpacity", 0, 100);
    AddColorRow(p,  L"Icon colour", L"iconColor",
                L"Applies to all drawn vector icons, including the Start logo.");
    AddSliderRow(p, L"Global Blur Amount", L"globalBlurAmount", 1, 50,
                 L"Strength of the Windhawk blur behind the bar background.");

    p.Children().Append(MakeHeading(L"Font"));
    AddComboRow(p, L"Font family", L"fontFamily", {
        { L"",                  L"(Default)" },
        { L"Segoe UI Variable", L"Segoe UI Variable" },
        { L"Segoe UI",          L"Segoe UI" },
        { L"Arial",             L"Arial" },
        { L"Calibri",           L"Calibri" },
        { L"Consolas",          L"Consolas" },
        { L"Tahoma",            L"Tahoma" },
        { L"Verdana",           L"Verdana" },
    }, L"Applies to every label and icon in the topbar.");
    AddBoolRow(p, L"Bold text", L"fontBold");

    p.Children().Append(MakeHeading(L"Clock & Date"));
    AddTextRow(p, L"Time format", L"timeFormat",
               L"Windows tokens: h, hh, H, HH, m, mm, s, ss, t, tt.");
    AddTextRow(p, L"Date format", L"dateFormat",
               L"Windows tokens: d, dd, ddd, dddd, M, MM, MMM, MMMM, y, yy, yyyy.");
}

void BuildLayoutPage(wuxc::StackPanel& p) {
    p.Children().Append(MakeHeading(L"Layout", 22));

    // Top of the page: how the center strip is filled
    AddComboRow(p, L"Center content", L"centerContent", {
        { L"applicationButtons", L"Application name" },
        { L"taskList",           L"Task list" },
        { L"none",               L"None" },
    });

    p.Children().Append(MakeHeading(L"Task Buttons"));
    AddSliderRow(p, L"Task button width (DIP)", L"taskButtonWidth", 40, 250);
    AddSliderRow(p, L"Task icon size (DIP)",    L"taskIconSize",    12, 40);
    AddComboRow(p, L"Task button content", L"taskButtonContent", {
        { L"iconAndText", L"Icon and text" },
        { L"iconOnly",    L"Icon only" },
        { L"textOnly",    L"Text only" },
    });

}

void BuildButtonsPage(wuxc::StackPanel& p) {
    p.Children().Append(MakeHeading(L"Buttons", 22));

    p.Children().Append(MakeHeading(L"System buttons"));
    AddBoolRow(p, L"Show Start button",  L"showStartButton");
    AddBoolRow(p, L"Show Search button", L"showSearchButton");

    p.Children().Append(MakeHeading(L"Control Center"));
    AddBoolRow(p, L"Show Display button",   L"showDisplayButton");
    AddBoolRow(p, L"Show Sound button",     L"showSoundButton");
    AddBoolRow(p, L"Show Wi-Fi button",     L"showWifiButton");
    AddBoolRow(p, L"Show Bluetooth button", L"showBluetoothButton");
    AddBoolRow(p, L"Show Battery button",   L"showBatteryButton");
    AddBoolRow(p, L"Show Settings button",  L"showSettingsButton");
    AddBoolRow(p, L"Show Weather button",   L"showWeatherButton");

    p.Children().Append(MakeHeading(L"Clock & Date"));
    AddBoolRow(p, L"Show clock", L"showClock");
    AddBoolRow(p, L"Show date",  L"showDate");

    p.Children().Append(MakeHeading(L"Resource Monitor"));
    AddBoolRow(p, L"Show CPU usage", L"showCpuUsage");
    AddBoolRow(p, L"Show RAM usage", L"showRamUsage");
    AddBoolRow(p, L"Show GPU usage", L"showGpuUsage");
}

static std::wstring BuildSettingsJson() {
    using namespace winrt::Windows::Data::Json;
    JsonObject obj;

    auto addStr = [&](PCWSTR k) {
        obj.SetNamedValue(k, JsonValue::CreateStringValue(GetStringSettingCopy(k)));
    };
    auto addInt = [&](PCWSTR k) {
        obj.SetNamedValue(k, JsonValue::CreateNumberValue(ReadIntSetting(k)));
    };

    addInt(L"barHeight");
    addInt(L"cornerRadius");
    addStr(L"topBarBackgroundColor");
    addInt(L"topBarBackgroundOpacity");
    addStr(L"iconColor");
    addStr(L"fontFamily");
    addInt(L"fontBold");
    addStr(L"timeFormat");
    addStr(L"dateFormat");
    addInt(L"showClock");
    addInt(L"showDate");
    addStr(L"leftItems");
    addStr(L"centerItems");
    addStr(L"rightItems");
    addInt(L"taskButtonWidth");
    addInt(L"taskIconSize");
    addStr(L"taskButtonContent");
    addStr(L"centerContent");
    addInt(L"showStartButton");
    addInt(L"showSearchButton");
    addInt(L"showDisplayButton");
    addInt(L"showSoundButton");
    addInt(L"showWifiButton");
    addInt(L"showBluetoothButton");
    addInt(L"showBatteryButton");
    addInt(L"showWeatherButton");
    addInt(L"showCpuUsage");
    addInt(L"showRamUsage");
    addInt(L"showGpuUsage");

    return std::wstring(obj.Stringify());
}



void BuildAboutPage(wuxc::StackPanel& p) {
    p.Children().Append(MakeHeading(L"TopBar for Windows", 22));
    p.Children().Append(MakeLabel(L"Version 1.2.0", 13, false, 0.65));
    auto info = MakeLabel(
        L"Mod Created By WasiXGamer. Settings App inspired by Taskbar Fluent Media Player mod by Slayts, Windhawk Blur imported from Windows 11 Taskbar Styler by m417z.",
        12, false, 0.8);
    info.TextWrapping(TextWrapping::Wrap);
    info.MaxWidth(420);
    info.Margin(Thickness{ 0, 6, 0, 20 });
    p.Children().Append(info);

    p.Children().Append(MakeHeading(L"Resources", 16));
    auto guide = MakeGhostButton(nullptr, 6);
    guide.HorizontalAlignment(HorizontalAlignment::Stretch);
    guide.Padding(Thickness{ 14, 10, 14, 10 });
    guide.Content(MakeLabel(L"Windhawk TopBar Styling Guide", 13));
    guide.Click([](auto&&, auto&&) {
        ShellExecute(nullptr, L"open",
            L"https://github.com/wasixgamer/windhawk-topbar-styling-guide",
            nullptr, nullptr, SW_SHOWNORMAL);
    });
    p.Children().Append(guide);

}

void ShowPage(int idx);   // forward decl (defined with kPages, below)

void BuildUiAlignmentPage(wuxc::StackPanel& p) {
    p.Children().Append(MakeHeading(L"UI Alignment", 22));
    auto help = MakeLabel(
        L"Tap Left / Center / Right to move each item between the three panels.",
        12, false, 0.6);
    help.TextWrapping(TextWrapping::Wrap);
    help.MaxWidth(420);
    p.Children().Append(help);

    struct Tile {
        const wchar_t* label;
        std::vector<std::wstring> members;
    };
    const std::vector<Tile> tiles = {
        { L"Start",     { L"StartButton" } },
        { L"Search",    { L"SearchButton" } },
        { L"Resources", { L"ResourceButton" } },
        { L"Weather",   { L"WeatherButton" } },
        { L"Clock",     { L"ClockButton" } },
        { L"Settings",  { L"SettingsButton" } },
        { L"Control Panel buttons",
          { L"DisplayButton", L"SoundButton", L"WifiButton",
            L"BluetoothButton", L"BatteryButton", L"RecycleBinButton" } },
    };

    auto allInSection = [](const std::vector<std::wstring>& members,
                           const std::wstring& csv) {
        auto list = SplitItemList(csv);
        for (const auto& m : members) {
            if (std::find(list.begin(), list.end(), m) == list.end()) return false;
        }
        return true;
    };

    for (const auto& t : tiles) {
        auto card = MakeCard();
        wuxc::Grid row;
        row.Padding(Thickness{ 14, 10, 14, 10 });
        wuxc::ColumnDefinition c1;
        c1.Width(GridLength{ 1, GridUnitType::Star });
        row.ColumnDefinitions().Append(c1);
        for (int i = 0; i < 3; i++) {
            wuxc::ColumnDefinition col;
            col.Width(GridLength{ 0, GridUnitType::Auto });
            row.ColumnDefinitions().Append(col);
        }
        auto label = MakeLabel(t.label, 13);
        wuxc::Grid::SetColumn(label, 0);
        row.Children().Append(label);

        std::wstring current;
        if      (allInSection(t.members, g_settings.leftItems))   current = L"left";
        else if (allInSection(t.members, g_settings.centerItems)) current = L"center";
        else if (allInSection(t.members, g_settings.rightItems))  current = L"right";

        const wchar_t* btnLabels[3] = { L"Left", L"Center", L"Right" };
        const wchar_t* btnIds[3]    = { L"left", L"center", L"right" };
        for (int i = 0; i < 3; i++) {
            auto btn = MakeGhostButton(nullptr, 6);
            btn.Width(68);
            btn.Padding(Thickness{ 0, 6, 0, 6 });
            btn.Margin(Thickness{ 4, 0, 0, 0 });
            btn.HorizontalContentAlignment(HorizontalAlignment::Center);
            btn.Content(MakeLabel(btnLabels[i], 12, true));
            if (current == btnIds[i]) {
                auto a = GetSystemAccentColor();
                btn.Background(MakeSolid(0xFF, a.R, a.G, a.B));
            } else {
                btn.Background(MakeSolid(0x18, 0xFF, 0xFF, 0xFF));
            }
            std::vector<std::wstring> membersCopy = t.members;
            std::wstring targetCopy = btnIds[i];
            btn.Click([membersCopy, targetCopy](auto&&, auto&&) {
                for (const auto& m : membersCopy) {
                    MoveItemToSection(m, targetCopy);
                }
                ScheduleReload();
                ShowPage(s_currentPage);   // refresh tile highlights
            });
            wuxc::Grid::SetColumn(btn, i + 1);
            row.Children().Append(btn);
        }
        card.Child(row);
        p.Children().Append(card);
    }
}

// ============================================================================
// Import / Export helpers
// ============================================================================

// Build the export payload with the user-provided config name embedded under
// the reserved "_name" key. Parsing on import strips that key back out.
static std::wstring BuildSettingsJsonWithName(const std::wstring& name) {
    using namespace winrt::Windows::Data::Json;
    std::wstring base = BuildSettingsJson();
    try {
        JsonObject obj = JsonObject::Parse(winrt::hstring(base));
        obj.SetNamedValue(L"_name", JsonValue::CreateStringValue(winrt::hstring(name)));
        return std::wstring(obj.Stringify());
    } catch (...) {
        return base;
    }
}

// Parse a JSON payload into (key, value) pairs, skipping the reserved "_name"
// key. Nothing is written to storage here — that only happens once the user
// confirms.
static bool ParseSettingsJson(
        const std::wstring& json,
        std::wstring* outName,
        std::vector<std::pair<std::wstring, std::wstring>>* outPairs) {
    using namespace winrt::Windows::Data::Json;
    try {
        auto obj = JsonObject::Parse(winrt::hstring(json));
        if (outName) {
            *outName = std::wstring(obj.GetNamedString(L"_name", L""));
        }
        for (auto it = obj.First(); it.HasCurrent(); it.MoveNext()) {
            auto pair = it.Current();
            std::wstring key(pair.Key());
            if (key == L"_name") continue;
            auto v = pair.Value();
            std::wstring value;
            switch (v.ValueType()) {
                case JsonValueType::String:  value = std::wstring(v.GetString()); break;
                case JsonValueType::Number:  value = std::to_wstring((int)v.GetNumber()); break;
                case JsonValueType::Boolean: value = v.GetBoolean() ? L"1" : L"0"; break;
                default: continue;
            }
            outPairs->emplace_back(std::move(key), std::move(value));
        }
        return true;
    } catch (...) {
        return false;
    }
}

static void ApplySettingsPairs(
        const std::vector<std::pair<std::wstring, std::wstring>>& pairs) {
    for (auto& kv : pairs) {
        Wh_SetStringValue(kv.first.c_str(), kv.second.c_str());
    }
    ScheduleReload();
}

// IFileSaveDialog / IFileOpenDialog CLSIDs. Declared by hand because the
// toolchain doesn't link uuid.lib (same reason kCLSID_PolicyConfigClient and
// kCLSID_WbemLocator are hand-written elsewhere in this file).
static const CLSID kCLSID_FileSaveDialog = {
    0xC0B4E2F3, 0xBA21, 0x4773, { 0x8D, 0xBA, 0x33, 0x5E, 0xC9, 0x46, 0xEB, 0x8B }};
static const CLSID kCLSID_FileOpenDialog = {
    0xDC1C5A9C, 0xE88A, 0x4DDE, { 0xA5, 0xA1, 0x60, 0xF8, 0x2A, 0x20, 0xAE, 0xF7 }};

// "Save as" file dialog. Returns "" if the user cancels.
static std::wstring PromptSaveFilePath(HWND owner, const std::wstring& suggested) {
    winrt::com_ptr<IFileSaveDialog> dlg;
    if (FAILED(CoCreateInstance(kCLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dlg.put())))) {
        return {};
    }
    DWORD opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_OVERWRITEPROMPT | FOS_FORCEFILESYSTEM);
    const COMDLG_FILTERSPEC filter[] = { { L"JSON config", L"*.json" } };
    dlg->SetFileTypes(1, filter);
    dlg->SetDefaultExtension(L"json");
    dlg->SetFileName(suggested.c_str());
    if (FAILED(dlg->Show(owner))) return {};
    winrt::com_ptr<IShellItem> item;
    if (FAILED(dlg->GetResult(item.put()))) return {};
    PWSTR path = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) || !path) return {};
    std::wstring result = path;
    CoTaskMemFree(path);
    return result;
}

// "Open" file dialog. Returns "" if the user cancels.
static std::wstring PromptOpenFilePath(HWND owner) {
    winrt::com_ptr<IFileOpenDialog> dlg;
    if (FAILED(CoCreateInstance(kCLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dlg.put())))) {
        return {};
    }
    DWORD opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM);
    const COMDLG_FILTERSPEC filter[] = { { L"JSON config", L"*.json" } };
    dlg->SetFileTypes(1, filter);
    dlg->SetDefaultExtension(L"json");
    if (FAILED(dlg->Show(owner))) return {};
    winrt::com_ptr<IShellItem> item;
    if (FAILED(dlg->GetResult(item.put()))) return {};
    PWSTR path = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) || !path) return {};
    std::wstring result = path;
    CoTaskMemFree(path);
    return result;
}

// Read a whole file as UTF-8 and decode to wide. Empty on failure.
static std::wstring ReadFileUtf8(const std::wstring& path) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return {};
    DWORD sz = GetFileSize(h, nullptr);
    std::string utf8(sz ? sz : 0, '\0');
    DWORD read = 0;
    if (sz) ReadFile(h, utf8.data(), sz, &read, nullptr);
    CloseHandle(h);
    utf8.resize(read);
    if (utf8.empty()) return {};

    // Skip a UTF-8 BOM if present.
    size_t start = 0;
    if (utf8.size() >= 3 &&
        (unsigned char)utf8[0] == 0xEF &&
        (unsigned char)utf8[1] == 0xBB &&
        (unsigned char)utf8[2] == 0xBF) {
        start = 3;
    }

    int needed = MultiByteToWideChar(CP_UTF8, 0, utf8.data() + start,
                                     (int)(utf8.size() - start), nullptr, 0);
    if (needed <= 0) return {};
    std::wstring wide(needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data() + start,
                        (int)(utf8.size() - start), wide.data(), needed);
    return wide;
}

// Write a UTF-8 JSON string to disk. Returns false on failure.
static bool WriteFileUtf8(const std::wstring& path, const std::wstring& content) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    int needed = WideCharToMultiByte(CP_UTF8, 0, content.c_str(),
                                     (int)content.size(), nullptr, 0, nullptr, nullptr);
    if (needed <= 0) { CloseHandle(h); return false; }
    std::string utf8(needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0, content.c_str(), (int)content.size(),
                        utf8.data(), needed, nullptr, nullptr);
    DWORD written = 0;
    BOOL ok = WriteFile(h, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
    CloseHandle(h);
    return ok != FALSE;
}

// Copy a wide string to the clipboard.
static void CopyWideToClipboard(HWND owner, const std::wstring& text) {
    if (!OpenClipboard(owner)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (mem) {
        void* dst = GlobalLock(mem);
        if (dst) {
            memcpy(dst, text.c_str(), bytes);
            GlobalUnlock(mem);
            if (!SetClipboardData(CF_UNICODETEXT, mem)) GlobalFree(mem);
        } else {
            GlobalFree(mem);
        }
    }
    CloseClipboard();
}

// Show the "Do you want to apply this config?" prompt and only commit if the
// user says Yes.
static void ConfirmAndApplyImportedConfig(
        const std::wstring& name,
        std::vector<std::pair<std::wstring, std::wstring>> pairs) {
    std::wstring shown = name.empty() ? std::wstring(L"(unnamed)") : name;
    std::wstring prompt =
        L"Do you want to apply the config:\n\n\"" + shown + L"\"";
    if (MessageBoxW(s_hwnd, prompt.c_str(), L"Apply config?",
                    MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }
    ApplySettingsPairs(pairs);
}

void BuildImportExportPage(wuxc::StackPanel& p) {
    p.Children().Append(MakeHeading(L"Import / Export Config", 22));
    auto help = MakeLabel(
        L"Save or restore the entire TopBar configuration. Exports embed the "
        L"config name below so you can identify them later.",
        12, false, 0.6);
    help.TextWrapping(TextWrapping::Wrap);
    help.MaxWidth(480);
    p.Children().Append(help);

    // Transient status line. Reused by every action button on this page —
    // "Copied to clipboard", "Exported to <file>", "Config applied", etc.
    // Cleared automatically a few seconds after it appears.
    auto status = MakeLabel(L"", 12, true);
    status.Foreground(MakeSolid(0xFF, 0x4C, 0xAF, 0x50));   // Win11 success green
    status.Opacity(0.0);
    status.Margin(Thickness{ 4, 14, 0, 0 });

    auto statusTimer = std::make_shared<wux::DispatcherTimer>();
    statusTimer->Interval(std::chrono::seconds(3));
    auto statusToken = std::make_shared<winrt::event_token>();
    *statusToken = statusTimer->Tick([status, statusTimer, statusToken](
            auto&&, auto&&) {
        statusTimer->Stop();
        status.Opacity(0.0);
    });

    auto showStatus = [status, statusTimer](const std::wstring& text) {
        status.Text(winrt::hstring(text));
        status.Opacity(1.0);
        statusTimer->Stop();
        statusTimer->Start();
    };

    // Reusable row for a status message that outlives the initial page
    // rebuild — captured by reference in the action lambdas below.
    auto setStatus = [showStatus](const std::wstring& text) {
        showStatus(text);
    };

    // --- Config name ------------------------------------------------------
    p.Children().Append(MakeHeading(L"Config name"));
    {
        wuxc::TextBox nameBox;
        nameBox.Text(GetStringSettingCopy(L"importExportConfigName"));
        nameBox.MinWidth(320);
        nameBox.CornerRadius(CornerRadius{ 4, 4, 4, 4 });
        StyleLightTextBox(nameBox);
        nameBox.LostFocus([](auto&& sender, auto&&) {
            auto t = sender.template as<wuxc::TextBox>();
            Wh_SetStringValue(L"importExportConfigName", std::wstring(t.Text()).c_str());
        });
        auto card = MakeCard();
        card.Child(MakeRowShell(L"Name", nameBox,
            L"Added to the exported JSON as \"_name\"."));
        p.Children().Append(card);
    }

    // --- Export -----------------------------------------------------------
    p.Children().Append(MakeHeading(L"Export"));

    auto mkActionBtn = [](const std::wstring& label, std::function<void()> onClick) {
        auto b = MakeGhostButton(nullptr, 6);
        b.HorizontalAlignment(HorizontalAlignment::Stretch);
        b.Padding(Thickness{ 14, 10, 14, 10 });
        b.HorizontalContentAlignment(HorizontalAlignment::Left);
        b.Background(MakeSolid(0x18, 0xFF, 0xFF, 0xFF));
        b.Content(MakeLabel(label, 13));
        b.Click([onClick](auto&&, auto&&) { onClick(); });
        return b;
    };

    p.Children().Append(mkActionBtn(L"Copy config to clipboard",
        [setStatus] {
            std::wstring name = GetStringSettingCopy(L"importExportConfigName");
            if (name.empty()) name = L"TopBar Config";
            CopyWideToClipboard(s_hwnd, BuildSettingsJsonWithName(name));
            setStatus(L"✓ Copied config \"" + name + L"\" to clipboard");
        }));

    {
        auto b = mkActionBtn(L"Export as .JSON…", [setStatus] {
            std::wstring suggested = GetStringSettingCopy(L"importExportConfigName");
            if (suggested.empty()) suggested = L"TopBar Config";
            // Sanitise a bit so the name is usable as a filename.
            for (auto& c : suggested) {
                if (c == L'\\' || c == L'/' || c == L':' || c == L'*' ||
                    c == L'?' || c == L'"' || c == L'<' || c == L'>' || c == L'|') {
                    c = L'_';
                }
            }
            suggested += L".json";

            std::wstring path = PromptSaveFilePath(s_hwnd, suggested);
            if (path.empty()) return;

            std::wstring name = GetStringSettingCopy(L"importExportConfigName");
            if (name.empty()) name = L"TopBar Config";
            std::wstring json = BuildSettingsJsonWithName(name);
            if (!WriteFileUtf8(path, json)) {
                MessageBoxW(s_hwnd, L"Couldn't write the file.",
                            L"TopBar Settings", MB_OK | MB_ICONERROR);
                return;
            }
            setStatus(L"✓ Exported to " + path);
        });
        b.Margin(Thickness{ 0, 6, 0, 0 });
        p.Children().Append(b);
    }

    // --- Import -----------------------------------------------------------
    p.Children().Append(MakeHeading(L"Import"));

    p.Children().Append(mkActionBtn(L"Import .JSON…", [setStatus] {
        std::wstring path = PromptOpenFilePath(s_hwnd);
        if (path.empty()) return;
        std::wstring json = ReadFileUtf8(path);
        if (json.empty()) {
            MessageBoxW(s_hwnd, L"Couldn't read the file, or it was empty.",
                        L"TopBar Settings", MB_OK | MB_ICONERROR);
            return;
        }
        std::wstring name;
        std::vector<std::pair<std::wstring, std::wstring>> pairs;
        if (!ParseSettingsJson(json, &name, &pairs)) {
            MessageBoxW(s_hwnd, L"That file doesn't contain valid TopBar JSON.",
                        L"TopBar Settings", MB_OK | MB_ICONERROR);
            return;
        }
        ConfirmAndApplyImportedConfig(name, std::move(pairs));
        setStatus(L"✓ Applied config \"" +
                  (name.empty() ? std::wstring(L"(unnamed)") : name) + L"\"");
    }));

    {
        auto b = mkActionBtn(L"Paste config from clipboard", [setStatus] {
            if (!OpenClipboard(s_hwnd)) {
                setStatus(L"✗ Could not open clipboard");
                return;
            }
            HANDLE h = GetClipboardData(CF_UNICODETEXT);
            if (!h) { CloseClipboard(); setStatus(L"✗ Clipboard is empty"); return; }
            auto ptr = static_cast<const wchar_t*>(GlobalLock(h));
            if (!ptr) { CloseClipboard(); setStatus(L"✗ Could not read clipboard"); return; }
            std::wstring json = ptr;
            GlobalUnlock(h);
            CloseClipboard();

            std::wstring name;
            std::vector<std::pair<std::wstring, std::wstring>> pairs;
            if (!ParseSettingsJson(json, &name, &pairs)) {
                MessageBoxW(s_hwnd, L"Clipboard doesn't contain valid TopBar JSON.",
                            L"TopBar Settings", MB_OK | MB_ICONERROR);
                return;
            }
            ConfirmAndApplyImportedConfig(name, std::move(pairs));
            setStatus(L"✓ Applied config \"" +
                      (name.empty() ? std::wstring(L"(unnamed)") : name) + L"\"");
        });
        b.Margin(Thickness{ 0, 6, 0, 0 });
        p.Children().Append(b);
    }

    p.Children().Append(status);
}

struct PageDef {
    const wchar_t* label;
    const wchar_t* glyph;
    void (*build)(wuxc::StackPanel&);
};
static const PageDef kPages[] = {
    { L"Appearance",          L"\uE790", BuildAppearancePage },       // Color
    { L"Layout",              L"\uE8A9", BuildLayoutPage },           // ViewAll
    { L"UI Alignment",        L"\uE71D", BuildUiAlignmentPage },      // GridView
    { L"Buttons",             L"\uE7C4", BuildButtonsPage },          // Button
    { L"Import / Export",     L"\uE8B5", BuildImportExportPage },     // Save
    { L"About",               L"\uE946", BuildAboutPage },            // Info
};
static constexpr int kPageCount = ARRAYSIZE(kPages);

void ShowPage(int idx) {
    if (idx < 0 || idx >= kPageCount) return;
    if (!s_contentPanel) return;
    s_currentPage = idx;
    s_contentPanel.Children().Clear();
    kPages[idx].build(s_contentPanel);
}

wuxc::Grid BuildWindowContent() {
    wuxc::Grid root;
    root.Background(MakeSolid(0xFF, 0x14, 0x14, 0x14));
    root.RequestedTheme(wux::ElementTheme::Dark);

    wuxc::NavigationView nav;
    nav.Name(L"SettingsNav");
    nav.IsBackButtonVisible(wuxc::NavigationViewBackButtonVisible::Collapsed);
    nav.IsSettingsVisible(false);
    nav.IsPaneToggleButtonVisible(false);
    nav.PaneDisplayMode(wuxc::NavigationViewPaneDisplayMode::Left);
    nav.OpenPaneLength(220);
    nav.IsPaneOpen(true);
    nav.SelectionFollowsFocus(wuxc::NavigationViewSelectionFollowsFocus::Disabled);

    // Translucent pane so the DWM blur shows through (Fix 3)
    nav.Resources().Insert(winrt::box_value(winrt::hstring(L"NavigationViewDefaultPaneBackground")),
                           MakeSolid(0x00, 0x00, 0x00, 0x00));
    nav.Resources().Insert(winrt::box_value(winrt::hstring(L"NavigationViewExpandedPaneBackground")),
                           MakeSolid(0x00, 0x00, 0x00, 0x00));
    nav.Resources().Insert(winrt::box_value(winrt::hstring(L"NavigationViewContentBackground")),
                           MakeSolid(0x00, 0x00, 0x00, 0x00));
    nav.Resources().Insert(winrt::box_value(winrt::hstring(L"NavigationViewContentGridBorderBrush")),
                           MakeSolid(0x00, 0x00, 0x00, 0x00));
    nav.Resources().Insert(winrt::box_value(winrt::hstring(L"NavigationViewContentGridBorderThickness")),
                           winrt::box_value(Thickness{ 0, 0, 0, 0 }));

    for (int i = 0; i < kPageCount; i++) {
        wuxc::NavigationViewItem item;
        item.Content(winrt::box_value(winrt::hstring(kPages[i].label)));
        wuxc::FontIcon icon;
        icon.Glyph(winrt::hstring(kPages[i].glyph));
        item.Icon(icon);
        item.Tag(winrt::box_value(static_cast<int32_t>(i)));
        nav.MenuItems().Append(item);
    }

    wuxc::ScrollViewer scroll;
    scroll.HorizontalScrollMode(wuxc::ScrollMode::Disabled);
    scroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    s_contentPanel = wuxc::StackPanel();
    s_contentPanel.Padding(Thickness{ 28, 20, 28, 28 });
    s_contentPanel.Spacing(4);
    s_contentPanel.HorizontalAlignment(HorizontalAlignment::Stretch);
    scroll.Content(s_contentPanel);
    nav.Content(scroll);

    nav.SelectionChanged([](wuxc::NavigationView const& nv, auto const&) {
        try {
            auto item = nv.SelectedItem().try_as<wuxc::NavigationViewItem>();
            if (!item) return;
            int idx = winrt::unbox_value<int32_t>(item.Tag());
            ShowPage(idx);
        } catch (...) {}
    });

    // Nav pane footer: just the Reset button. Export/Import now live on their
    // own page (see BuildImportExportPage).
    {
        wuxc::StackPanel footer;
        footer.Spacing(4);
        footer.Margin(Thickness{ 8, 8, 8, 12 });

        auto resetBtn = MakeGhostButton(nullptr, 6);
        resetBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
        resetBtn.Padding(Thickness{ 12, 8, 12, 8 });
        resetBtn.HorizontalContentAlignment(HorizontalAlignment::Left);
        {
            wuxc::StackPanel resetContent;
            resetContent.Orientation(wuxc::Orientation::Horizontal);
            resetContent.Spacing(10);
            resetContent.VerticalAlignment(VerticalAlignment::Center);

            wuxc::FontIcon resetIcon;
            resetIcon.Glyph(L"\uE777");
            resetIcon.FontSize(14);
            resetIcon.VerticalAlignment(VerticalAlignment::Center);
            resetContent.Children().Append(resetIcon);

            resetContent.Children().Append(MakeLabel(L"Reset config…", 12));

            resetBtn.Content(resetContent);
        }
        resetBtn.Click([](auto&&, auto&&) {
            if (MessageBoxW(s_hwnd,
                    L"Reset all TopBar settings to their defaults?",
                    L"TopBar Settings",
                    MB_YESNO | MB_ICONWARNING) != IDYES) return;
            const wchar_t* keys[] = {
                L"barHeight", L"cornerRadius", L"topBarBackgroundColor",
                L"topBarBackgroundOpacity", L"iconColor", L"fontFamily", L"fontBold",
                L"timeFormat", L"dateFormat", L"showClock", L"showDate",
                L"leftItems", L"centerItems", L"rightItems",
                L"taskButtonWidth", L"taskIconSize", L"taskButtonContent",
                L"centerContent", L"showStartButton", L"showSearchButton",
                L"showDisplayButton", L"showSoundButton", L"showWifiButton",
                L"showBluetoothButton", L"showBatteryButton",
                L"showWeatherButton",
                L"showCpuUsage", L"showRamUsage", L"showGpuUsage",
            };
            for (auto* k : keys) Wh_SetStringValue(k, L"");
            ScheduleReload();
        });
        footer.Children().Append(resetBtn);

        auto donateBtn = MakeGhostButton(nullptr, 6);
        donateBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
        donateBtn.Padding(Thickness{ 12, 8, 12, 8 });
        donateBtn.HorizontalContentAlignment(HorizontalAlignment::Center);
        donateBtn.Margin(Thickness{ 0, 6, 0, 0 });

        wui::Color accent = GetSystemAccentColor();
        double r = accent.R / 255.0;
        double g = accent.G / 255.0;
        double b = accent.B / 255.0;
        double brightness =
            std::sqrt(0.299 * r * r + 0.587 * g * g + 0.114 * b * b);
        wui::Color textColor = (brightness > 0.5)
            ? wui::ColorHelper::FromArgb(255, 0, 0, 0)
            : wui::ColorHelper::FromArgb(255, 255, 255, 255);

        auto accentBrush = wuxm::SolidColorBrush(accent);
        donateBtn.Background(accentBrush);
        donateBtn.BorderThickness(Thickness{ 0, 0, 0, 0 });

        // Same brush in every pointer state — no hover or press color shift.
        // The default Button template binds these to theme resources, so
        // overriding each one is what actually holds the color flat.
        donateBtn.Resources().Insert(
            winrt::box_value(winrt::hstring(L"ButtonBackground")),
            accentBrush);
        donateBtn.Resources().Insert(
            winrt::box_value(winrt::hstring(L"ButtonBackgroundPointerOver")),
            accentBrush);
        donateBtn.Resources().Insert(
            winrt::box_value(winrt::hstring(L"ButtonBackgroundPressed")),
            accentBrush);
        donateBtn.Resources().Insert(
            winrt::box_value(winrt::hstring(L"ButtonBackgroundDisabled")),
            accentBrush);
        donateBtn.Resources().Insert(
            winrt::box_value(winrt::hstring(L"ButtonBorderBrush")),
            accentBrush);
        donateBtn.Resources().Insert(
            winrt::box_value(winrt::hstring(L"ButtonBorderBrushPointerOver")),
            accentBrush);
        donateBtn.Resources().Insert(
            winrt::box_value(winrt::hstring(L"ButtonBorderBrushPressed")),
            accentBrush);

        auto donateLabel = MakeLabel(L"Support on Patreon", 13, true);
        donateLabel.Foreground(wuxm::SolidColorBrush(textColor));
        donateLabel.HorizontalAlignment(HorizontalAlignment::Center);
        donateLabel.TextAlignment(TextAlignment::Center);
        donateBtn.Content(donateLabel);
        donateBtn.Click([](auto&&, auto&&) {
            ShellExecute(nullptr, L"open",
                L"https://www.patreon.com/WasiXGamer/join",
                nullptr, nullptr, SW_SHOWNORMAL);
        });
        footer.Children().Append(donateBtn);

        // Social row: [Discord] [YouTube] side by side, below Patreon.
        // Fill-only vector paths (24x24 viewport). Swap the two URLs for the
        // real invite/channel if these guesses are wrong.
        constexpr PCWSTR kDiscordPath =
            L"M20.32 4.37a16.1 16.1 0 0 0-4.02-1.24.06.06 0 0 0-.06.03c-.17.31-.37.72-.51 1.04"
            L"a14.85 14.85 0 0 0-4.47 0 10.2 10.2 0 0 0-.52-1.04.06.06 0 0 0-.06-.03 16.1 16.1 0 0 0"
            L"-4.02 1.24.06.06 0 0 0-.03.02C4.11 9.09 3.31 13.66 3.7 18.18a.07.07 0 0 0 .03.05c1.69"
            L" 1.24 3.33 2 4.94 2.5a.07.07 0 0 0 .07-.03c.38-.52.72-1.07 1.01-1.64a.06.06 0 0 0-.03"
            L"-.09c-.54-.2-1.05-.45-1.55-.73a.06.06 0 0 1-.01-.11c.1-.08.21-.16.31-.24a.06.06 0 0 1"
            L".07-.01c3.25 1.48 6.77 1.48 9.98 0a.06.06 0 0 1 .07.01c.1.08.2.16.31.24a.06.06 0 0 1"
            L"-.01.11c-.49.29-1.01.53-1.55.73a.06.06 0 0 0-.03.09c.3.58.64 1.12 1.01 1.64a.07.07 0"
            L" 0 0 .07.03c1.62-.5 3.26-1.26 4.95-2.5a.07.07 0 0 0 .03-.05c.46-5.22-.77-9.76-3.27-13"
            L".79a.05.05 0 0 0-.02-.02zM10.34 15.35c-.98 0-1.78-.89-1.78-1.99s.79-2 1.78-2c1 0 1.8"
            L".9 1.78 2 0 1.1-.79 1.99-1.78 1.99zm5.32 0c-.98 0-1.78-.89-1.78-1.99s.79-2 1.78-2c1"
            L" 0 1.8.9 1.78 2 0 1.1-.78 1.99-1.78 1.99z";

        constexpr PCWSTR kYoutubePath =
            L"M23.5 6.19a3.02 3.02 0 0 0-2.12-2.14C19.5 3.55 12 3.55 12 3.55s-7.5 0-9.38.5A3.02 3"
            L".02 0 0 0 .5 6.19C0 8.08 0 12 0 12s0 3.92.5 5.81a3.02 3.02 0 0 0 2.12 2.14c1.88.5 9"
            L".38.5 9.38.5s7.5 0 9.38-.5a3.02 3.02 0 0 0 2.12-2.14C24 15.92 24 12 24 12s0-3.92-.5-"
            L"5.81zM9.55 15.57V8.43L15.82 12l-6.27 3.57z";

        auto makeSocialBtn = [](PCWSTR fillPath, const std::wstring& label,
                                PCWSTR url) {
            auto btn = MakeGhostButton(nullptr, 6);
            btn.HorizontalAlignment(HorizontalAlignment::Stretch);
            btn.Padding(Thickness{ 10, 8, 10, 8 });
            btn.HorizontalContentAlignment(HorizontalAlignment::Center);
            btn.Background(MakeSolid(0x18, 0xFF, 0xFF, 0xFF));
            btn.BorderThickness(Thickness{ 0, 0, 0, 0 });

            wuxc::StackPanel row;
            row.Orientation(wuxc::Orientation::Horizontal);
            row.Spacing(8);
            row.VerticalAlignment(VerticalAlignment::Center);
            row.HorizontalAlignment(HorizontalAlignment::Center);

            if (auto icon = BuildVectorIcon(nullptr, fillPath, L"", 24, 16, 1.5)) {
                icon.VerticalAlignment(VerticalAlignment::Center);
                row.Children().Append(icon);
            }
            row.Children().Append(MakeLabel(label, 12, true));
            btn.Content(row);

            std::wstring urlStr = url;
            btn.Click([urlStr](auto&&, auto&&) {
                ShellExecute(nullptr, L"open", urlStr.c_str(),
                             nullptr, nullptr, SW_SHOWNORMAL);
            });
            return btn;
        };

        auto socialGrid = wuxc::Grid();
        socialGrid.ColumnSpacing(6);
        socialGrid.Margin(Thickness{ 0, 6, 0, 0 });
        wuxc::ColumnDefinition sc1; sc1.Width(GridLength{ 1, GridUnitType::Star });
        wuxc::ColumnDefinition sc2; sc2.Width(GridLength{ 1, GridUnitType::Star });
        socialGrid.ColumnDefinitions().Append(sc1);
        socialGrid.ColumnDefinitions().Append(sc2);

        auto discordBtn = makeSocialBtn(kDiscordPath, L"Discord",
                                        L"https://discord.gg/BTQYJSpUgX");
        auto ytBtn = makeSocialBtn(kYoutubePath, L"YouTube",
                                   L"https://www.youtube.com/@WasiCustomization");
        wuxc::Grid::SetColumn(discordBtn, 0);
        wuxc::Grid::SetColumn(ytBtn, 1);
        socialGrid.Children().Append(discordBtn);
        socialGrid.Children().Append(ytBtn);

        footer.Children().Append(socialGrid);

        nav.PaneFooter(footer);
    }

    root.Children().Append(nav);

    if (nav.MenuItems().Size() > 0) {
        nav.SelectedItem(nav.MenuItems().GetAt(0));
    }
    return root;
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_SIZE:
            if (s_islandHwnd) MoveWindow(s_islandHwnd, 0, 0, LOWORD(l), HIWORD(l), TRUE);
            return 0;
        case WM_SETFOCUS:
            if (s_islandHwnd) SetFocus(s_islandHwnd);
            return 0;
        case WM_CLOSE:
            DestroyWindow(h);
            return 0;
        case WM_DESTROY:
            try { if (s_xamlSource) s_xamlSource.Close(); } catch (...) {}
            s_xamlSource = nullptr;
            s_native2 = nullptr;
            s_islandHwnd = nullptr;
            s_contentPanel = nullptr;
            s_hwnd = nullptr;
            // Intentionally NOT PostQuitMessage — the topbar thread owns the loop.
            return 0;
    }
    return DefWindowProc(h, m, w, l);
}

HICON g_settingsLargeIcon = nullptr;
HICON g_settingsSmallIcon = nullptr;

// The modern Settings icon lives inside the immersivecontrolpanel AppX
// package, not in SystemSettings.exe anymore, so the exe's resources give the
// old blue gear. Ask the shell for the AppsFolder entry instead. Falls back
// to the classic imageres.dll gear only if the shell query fails.
HICON LoadSettingsIconFromShell(int cx, int cy) {
    constexpr PCWSTR kSettingsAumid =
        L"shell:AppsFolder\\windows.immersivecontrolpanel_cw5n1h2txyewy"
        L"!microsoft.windows.immersivecontrolpanel";

    winrt::com_ptr<IShellItemImageFactory> factory;
    if (SUCCEEDED(SHCreateItemFromParsingName(kSettingsAumid, nullptr,
                                              IID_PPV_ARGS(factory.put()))) &&
        factory) {
        HBITMAP bmp = nullptr;
        SIZE sz{cx, cy};
        if (SUCCEEDED(factory->GetImage(sz, SIIGBF_ICONONLY, &bmp)) && bmp) {
            ICONINFO ii{};
            ii.fIcon = TRUE;
            ii.hbmColor = bmp;
            ii.hbmMask = CreateBitmap(cx, cy, 1, 1, nullptr);
            HICON icon = CreateIconIndirect(&ii);
            DeleteObject(bmp);
            if (ii.hbmMask) DeleteObject(ii.hbmMask);
            if (icon) return icon;
        }
    }

    HICON icon = nullptr;
    ExtractIconExW(L"imageres.dll", -114, &icon, nullptr, 1);
    if (!icon) {
        SHSTOCKICONINFO sii{};
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_SETTINGS, SHGSI_ICON | SHGSI_LARGEICON, &sii))) {
            icon = sii.hIcon;
        }
    }
    return icon;
}

void EnsureSettingsWindowIcons() {
    if (g_settingsLargeIcon && g_settingsSmallIcon) return;

    int large = GetSystemMetrics(SM_CXICON);
    int small = GetSystemMetrics(SM_CXSMICON);
    if (large <= 0) large = 32;
    if (small <= 0) small = 16;

    if (!g_settingsLargeIcon) g_settingsLargeIcon = LoadSettingsIconFromShell(large, large);
    if (!g_settingsSmallIcon) g_settingsSmallIcon = LoadSettingsIconFromShell(small, small);

    // Last-ditch fallback: if the shell icon is unavailable, fall back to the
    // stock question mark so the window still has *an* icon.
    if (!g_settingsLargeIcon) {
        SHSTOCKICONINFO sii{};
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON | SHGSI_LARGEICON, &sii))) {
            g_settingsLargeIcon = sii.hIcon;
        }
    }
    if (!g_settingsSmallIcon) g_settingsSmallIcon = g_settingsLargeIcon;
}

void CreateWindowOnCurrentThread() {
    if (s_hwnd && IsWindow(s_hwnd)) {
        ApplyModernWindowChrome(s_hwnd);
        ShowWindow(s_hwnd, IsIconic(s_hwnd) ? SW_RESTORE : SW_SHOW);
        SetForegroundWindow(s_hwnd);
        return;
    }

    EnsureSettingsWindowIcons();

    WNDCLASSEX wc{ sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    // The window procedure lives in the mod image, not in explorer.exe, so
    // register the class against the mod module (matching the topbar's own
    // class) to avoid a dangling lpfnWndProc if this ever runs without an
    // immediate ExitProcess.
    wc.hInstance = g_modModule ? g_modModule : GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = g_settingsLargeIcon;
    wc.hIconSm = g_settingsSmallIcon;
    wc.lpszClassName = kClassName;
    RegisterClassEx(&wc);

    int dpi = 96;
    if (HDC hdc = GetDC(nullptr)) {
        dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(nullptr, hdc);
    }
    int W = static_cast<int>(1000 * dpi / 96.0);
    int H = static_cast<int>(720 * dpi / 96.0);
    int x = (GetSystemMetrics(SM_CXSCREEN) - W) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - H) / 2;

    s_hwnd = CreateWindowEx(WS_EX_APPWINDOW, kClassName, L"TopBar Settings",
        WS_OVERLAPPEDWINDOW, x, y, W, H,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!s_hwnd) {
        Wh_Log(L"TopBar Settings: CreateWindowEx failed: %u", GetLastError());
        return;
    }

    // The window class is registered only once, so its icons won't be
    // re-read on a second open. Setting them per window keeps the taskbar
    // and alt-tab preview correct every time.
    if (g_settingsLargeIcon) {
        SendMessage(s_hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_settingsLargeIcon);
    }
    if (g_settingsSmallIcon) {
        SendMessage(s_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_settingsSmallIcon);
    }

    try {
        s_xamlSource = wuxh::DesktopWindowXamlSource();
        auto native = s_xamlSource.as<IDesktopWindowXamlSourceNative>();
        winrt::check_hresult(native->AttachToWindow(s_hwnd));
        winrt::check_hresult(native->get_WindowHandle(&s_islandHwnd));
        RECT rc;
        GetClientRect(s_hwnd, &rc);
        SetWindowPos(s_islandHwnd, nullptr, 0, 0, rc.right, rc.bottom,
                     SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
        s_xamlSource.Content(BuildWindowContent());
        s_native2 = s_xamlSource.try_as<IDesktopWindowXamlSourceNative2>();
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"TopBar Settings: XAML init failed: %08X - %s",
               static_cast<unsigned>(ex.code().value), ex.message().c_str());
        try { if (s_xamlSource) s_xamlSource.Close(); } catch (...) {}
        s_xamlSource = nullptr;
        s_islandHwnd = nullptr;
        DestroyWindow(s_hwnd);
        s_hwnd = nullptr;
        return;
    } catch (...) {
        try { if (s_xamlSource) s_xamlSource.Close(); } catch (...) {}
        s_xamlSource = nullptr;
        s_islandHwnd = nullptr;
        DestroyWindow(s_hwnd);
        s_hwnd = nullptr;
        return;
    }

    ShowWindow(s_hwnd, SW_SHOW);
    UpdateWindow(s_hwnd);
    // Applied after the first ShowWindow so the initial theme pass doesn't
    // overwrite them.
    ApplyModernWindowChrome(s_hwnd);
    // Re-assert once DWM has fully created the frame.
    SetWindowPos(s_hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
    ApplyModernWindowChrome(s_hwnd);
    SetForegroundWindow(s_hwnd);
}

bool PreTranslateSettingsMessage(MSG* msg) {
    if (!s_native2) return false;
    BOOL processed = FALSE;
    try { s_native2->PreTranslateMessage(msg, &processed); } catch (...) {}
    return processed != FALSE;
}

void DestroySettingsWindowNow() {
    // Stop the debounce timer on the UI thread before tearing anything else
    // down — it's UI-thread-affine and must not be released from the
    // shutdown thread.
    if (s_applyTimer) {
        s_applyTimer.Stop();
        s_applyTimer = nullptr;
    }
    if (s_hwnd && IsWindow(s_hwnd)) {
        DestroyWindow(s_hwnd);
    }
    if (g_modModule) {
        UnregisterClass(kClassName, g_modModule);
    }
}

} // namespace topbar_settings_window

void OpenTopBarSettingsWindow() {
    RunOnUiThread([] {
        topbar_settings_window::CreateWindowOnCurrentThread();
    });
}

void CloseTopBarSettingsWindow() {
    if (topbar_settings_window::s_hwnd &&
        IsWindow(topbar_settings_window::s_hwnd)) {
        PostMessage(topbar_settings_window::s_hwnd, WM_CLOSE, 0, 0);
    }
}

FrameworkElement BuildTopBarContent() {
    
    g_namedElements.clear();
    g_taskButtonsByHwnd.clear();
    g_taskButtonLastTitle.clear();
    g_stableWindowOrder.clear();
    g_detachedStyleRoots.clear();  // Clear old flyout roots before rebuilding
    g_lastAppTitleTarget = nullptr;
    g_lastAppTitleText.clear();

    // Outer root grid that holds everything (background + interactive bar)
    wuxc::Grid barRoot;
    barRoot.Name(L"BarRoot");
    barRoot.HorizontalAlignment(HorizontalAlignment::Stretch);
    barRoot.VerticalAlignment(VerticalAlignment::Stretch);
    barRoot.Background(nullptr); // Transparent so the blur shows through
    // Wallpaper layer (added first so it's behind everything)
    wuxm::ImageBrush wallpaperBrush = GetWallpaperBrush();
    {
        wuxc::Grid wallpaperLayer;
        wallpaperLayer.Name(L"WallpaperLayer");
        wallpaperLayer.Background(wallpaperBrush);
        wallpaperLayer.IsHitTestVisible(false); // don't block clicks
        wallpaperLayer.HorizontalAlignment(HorizontalAlignment::Stretch);
        wallpaperLayer.VerticalAlignment(VerticalAlignment::Stretch);
        wallpaperLayer.Opacity(1.0);
        barRoot.Children().Append(wallpaperLayer);
        g_wallpaperLayer = wallpaperLayer; // Save reference
    }
    // Interactive content grid (This is TopBarRoot and gets the blur)
    wuxc::Grid root;
    root.Name(L"TopBarRoot");
    root.HorizontalAlignment(HorizontalAlignment::Stretch);
    root.VerticalAlignment(VerticalAlignment::Stretch);

    // Font is applied per-label inside MakeText — Grid (Panel) has no font
    // properties, so it can't be set once on the root.

    // Parse the user's color and opacity
    wui::Color tintColor;
    if (!ParseBarColor(g_settings.topBarBackgroundColor, g_settings.topBarBackgroundOpacity, &tintColor)) {
        tintColor = wui::ColorHelper::FromArgb(128, 0, 0, 0); // default: black 50%
    }

    root.Loaded([root, tintColor](wf::IInspectable const&, RoutedEventArgs const&) {
        if (root.Background() != nullptr) {
            return;
        }
        auto blurBrush = winrt::make<XamlBlurBrush>(
            root.as<UIElement>(),
            static_cast<float>(g_settings.globalBlurAmount),
            tintColor,
            tintColor.A);
        root.Background(blurBrush);
    });

    for (int i = 0; i < 4; i++) {
        wuxc::ColumnDefinition column;
        column.Width(GridLength{i == 1 ? 1.0 : 0.0,
                                i == 1 ? GridUnitType::Star : GridUnitType::Auto});
        root.ColumnDefinitions().Append(column);
    }

    // Add the interactive root on top of the wallpaper layer
    barRoot.Children().Append(root);

    // Double-clicking empty bar space maximizes or restores the window that was
    // last in the foreground. The bar itself takes focus on click, so the live
    // foreground window is useless here -- the WinEvent hook's record is used
    // instead.
    root.DoubleTapped([](auto&&, Input::DoubleTappedRoutedEventArgs const& args) {
        args.Handled(true);
        if (g_lastForegroundHwnd && IsWindow(g_lastForegroundHwnd)) {
            ToggleMaximizeWindow(g_lastForegroundHwnd);
        }
    });

    root.Loaded([](auto&&, auto&&) {
        RunOnUiThread([] {
            try {
                std::function<void(DependencyObject)> hookAll = [&](DependencyObject node) {
                    if (!node) return;
                    try {
                        if (auto fe = node.try_as<FrameworkElement>()) {
                            auto groups = VisualStateManager::GetVisualStateGroups(fe);
                            for (auto const& g : groups) {
                                if (g) {
                                    g.CurrentStateChanged([](auto&&, auto&&) {
                                        ApplyAllControlStyles();
                                    });
                                }
                            }
                        }
                        int n = wuxm::VisualTreeHelper::GetChildrenCount(node);
                        for (int i = 0; i < n; i++) {
                            hookAll(wuxm::VisualTreeHelper::GetChild(node, i));
                        }
                    } catch (...) {}
                };
                if (g_rootElement) hookAll(g_rootElement);
            } catch (...) {}
        });
    });

    root.RightTapped([](wf::IInspectable const& sender, Input::RightTappedRoutedEventArgs const& args) {
        args.Handled(true);
        auto menu = wuxc::MenuFlyout();
        StyleMenuFlyout(menu);
        menu.Items().Append(MakeMenuItem(L"TopBar settings…", [] {
            OpenTopBarSettingsWindow();
        }));
        menu.Items().Append(MakeMenuItem(L"Reload TopBar", [] {
            RunOnUiThread([] {
                if (!g_topBarHwnd) return;
                LoadSettings();
                g_dpiScale = GetBarDpiScale();
                g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);
                auto content = BuildTopBarContent();
                g_desktopSource.Content(content);
                BuildStartContextMenu();
                BuildTaskContextMenu();
                ApplyAllControlStyles();
                ApplyVisibilitySettings();
                UpdateResourceButton();
                StripInheritedIslandBackgrounds();
                PositionAppBar(g_topBarHwnd, g_barHeightPx);
                RefreshTaskList(true);
            });
        }));
        auto fe = sender.as<FrameworkElement>();
        auto pt = args.GetPosition(fe);
        menu.ShowAt(fe, pt);   // anchor at the cursor, not the element's corner
    });

    // ---- Left cluster: Start, Search --------------------------------------
    wuxc::StackPanel leftPanel;
    leftPanel.Name(L"LeftPanel");
    leftPanel.Orientation(wuxc::Orientation::Horizontal);
    leftPanel.VerticalAlignment(VerticalAlignment::Stretch);

    {
        auto startButton = MakeGhostButton(L"StartButton", g_settings.cornerRadius);
        startButton.VerticalAlignment(VerticalAlignment::Stretch);
        startButton.Margin(Thickness{8, 2, 2, 2});
        startButton.Padding(Thickness{0, 0, 0, 0});
        startButton.HorizontalContentAlignment(HorizontalAlignment::Center);
        if (auto icon = BuildWindows11StartIcon(20)) {
          startButton.Content(icon);
          RegisterNamed(L"StartIcon", icon);
    }
        startButton.Click([](auto&&, auto&&) { SendWinKeyChord(VK_LWIN); });
        startButton.RightTapped(
            [](wf::IInspectable const& sender, Input::RightTappedRoutedEventArgs const& args) {
                args.Handled(true);
                if (g_startContextMenu) {
                    g_startContextMenu.ShowAt(sender.as<FrameworkElement>());
                }
            });
        RegisterNamed(L"StartButton", startButton);
        leftPanel.Children().Append(startButton);
    }

    {
        // The old search *box* never worked and isn't wanted; this is a plain
        // button that opens the real Windows search.
        auto searchButton = MakeGhostButton(L"SearchButton", g_settings.cornerRadius);
        searchButton.VerticalAlignment(VerticalAlignment::Stretch);
        searchButton.Margin(Thickness{4, 2, 4, 2});
        searchButton.Padding(Thickness{0, 0, 0, 0});
        searchButton.HorizontalContentAlignment(HorizontalAlignment::Center);
        if (auto icon = BuildSearchIcon(20)) {
            searchButton.Content(icon);
            RegisterNamed(L"SearchIcon", icon);
        }
        searchButton.Click([](auto&&, auto&&) { SendWinKeyChord('S'); });
        RegisterNamed(L"SearchButton", searchButton);
        leftPanel.Children().Append(searchButton);
    }

    wuxc::Grid::SetColumn(leftPanel, 0);
    root.Children().Append(leftPanel);
    RegisterNamed(L"LeftPanel", leftPanel);
    g_leftPanel = leftPanel;

    // ---- Middle: the task list --------------------------------------------
    wuxc::StackPanel taskPanel;
    taskPanel.Name(L"TaskListPanel");
    taskPanel.Orientation(wuxc::Orientation::Horizontal);
    // Stretch so the task buttons inherit the full bar height and their margins
    // mean the same thing as the Start and Search margins do.
    taskPanel.VerticalAlignment(VerticalAlignment::Stretch);
    taskPanel.HorizontalAlignment(HorizontalAlignment::Left);
    wuxc::Grid::SetColumn(taskPanel, 1);
    root.Children().Append(taskPanel);
    g_taskListPanel = taskPanel;
g_taskListPanel.SizeChanged([](auto&&, auto&&) {
    AdjustTaskButtonWidths();
});
    RegisterNamed(L"TaskListPanel", taskPanel);

    wuxc::StackPanel centerPanel;
    centerPanel.Name(L"CenterPanel");
    centerPanel.Orientation(wuxc::Orientation::Horizontal);
    centerPanel.VerticalAlignment(VerticalAlignment::Stretch);
    centerPanel.HorizontalAlignment(HorizontalAlignment::Center);
    g_centerPanel = centerPanel;
    wuxc::Grid::SetColumnSpan(centerPanel, 4);
    root.Children().Append(centerPanel);
    RegisterNamed(L"CenterPanel", centerPanel);

    wuxc::StackPanel clockPanel;
    clockPanel.Name(L"ClockPanel");
    clockPanel.Orientation(wuxc::Orientation::Horizontal);
    clockPanel.VerticalAlignment(VerticalAlignment::Stretch);
    g_clockPanel = clockPanel;
    g_clockPanel.HorizontalAlignment(HorizontalAlignment::Center);
    wuxc::Grid::SetColumn(clockPanel, 3);
    root.Children().Append(clockPanel);
    RegisterNamed(L"ClockPanel", clockPanel);

    wuxc::StackPanel rightPanel;
    rightPanel.Name(L"ControlCenterPanel");
    rightPanel.Orientation(wuxc::Orientation::Horizontal);
    rightPanel.VerticalAlignment(VerticalAlignment::Stretch);

    g_displayFlyout = MakeControlFlyout(L"DisplayFlyoutRoot", g_displayPanel);
    g_soundFlyout = MakeControlFlyout(L"SoundFlyoutRoot", g_soundPanel);
    g_wifiFlyout = MakeControlFlyout(L"WifiFlyoutRoot", g_wifiPanel);
    g_bluetoothFlyout = MakeControlFlyout(L"BluetoothFlyoutRoot", g_bluetoothPanel);
    
    g_batteryFlyout = MakeControlFlyout(L"BatteryFlyoutRoot", g_batteryPanel);

    g_wifiFlyout.Closed([](auto&&, auto&&) {
        if (g_wifiAutoRefreshTimer) {
            g_wifiAutoRefreshTimer.Stop();
        }
    });
    g_bluetoothFlyout.Closed([](auto&&, auto&&) {
        if (g_bluetoothAutoRefreshTimer) {
            g_bluetoothAutoRefreshTimer.Stop();
        }
    });

    g_displayButton = MakeControlButton(
        L"DisplayButton", BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6),
        g_displayFlyout, [] {
            // Show loading placeholder immediately
            if (g_displayPanel) {
                g_populatingPanel = true;
                auto children = g_displayPanel.Children();
                children.Clear();
                children.Append(MakePanelTitle(L"Display"));
                children.Append(MakeStatusText(L"Loading…"));
                g_populatingPanel = false;
            }
            // Fetch brightness in background
            RunInBackground([] {
                bool available = brightness::Available();
                int brightnessValue = brightness::Get();
                bool darkMode = IsAppsDarkMode();
                RunOnUiThread([available, brightnessValue, darkMode]() {
                    if (!g_displayPanel) return;
                    g_populatingPanel = true;
                    auto children = g_displayPanel.Children();
                    children.Clear();
                    children.Append(MakePanelTitle(L"Display"));

                    if (available) {
                        auto icon = BuildVectorIcon(nullptr, L"", icons::kBrightnessStroke, 24, 18, 1.6);
                        children.Append(MakeSliderRow(icon, brightnessValue,
                            [](int value) { SetBrightnessCoalesced(value); }));
                    } else {
                        children.Append(MakeStatusText(L"Brightness control isn't available on this display."));
                    }

                    children.Append(MakeDivider());

                    auto darkTile = MakeGhostButton(L"QuickToggleTile", kTileCorner);
                    darkTile.HorizontalAlignment(HorizontalAlignment::Stretch);
                    darkTile.Padding(Thickness{8, 6, 8, 6});
                    darkTile.Background(darkMode ? MakeBrush(0xFF, GetSystemAccentColor().R, GetSystemAccentColor().G, GetSystemAccentColor().B)
                                                 : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
                    wuxc::StackPanel darkStack;
                    darkStack.Orientation(wuxc::Orientation::Horizontal);
                    darkStack.Spacing(8);
                    if (auto icon = BuildVectorIcon(nullptr, icons::kMoonFill, L"", 24, 16, 1.7, L"#FFFFFF")) {
                        icon.VerticalAlignment(VerticalAlignment::Center);
                        darkStack.Children().Append(icon);
                    }
                    darkStack.Children().Append(MakeText(nullptr, L"Dark mode", 12));
                    darkTile.Content(darkStack);
                    darkTile.Click([darkMode](auto&&, auto&&) {
                        SetAppsDarkMode(!darkMode);
                        RepopulateLater(PopulateDisplayPanel);
                    });
                    children.Append(darkTile);

                    children.Append(MakeDivider());
                    children.Append(MakeSettingsLink(L"Display settings", L"ms-settings:display"));
                    g_populatingPanel = false;
                });
            });
        });

    AttachWheelHandler(g_displayButton, false);
    rightPanel.Children().Append(g_displayButton);

    g_soundButton = MakeControlButton(
        L"SoundButton", BuildSpeakerIcon(18, audio::GetMasterVolume(), audio::GetMasterMute()),
        g_soundFlyout, [] { PopulateSoundPanel(); });

    AttachWheelHandler(g_soundButton, true);
    rightPanel.Children().Append(g_soundButton);

    {
        auto status = wifi::GetStatus();

g_wifiButton = MakeControlButton(
    L"WifiButton", BuildWifiIcon(18, status.signal, status.connected),
    g_wifiFlyout, [] {
        // Show loading
        if (g_wifiPanel) {
            g_populatingPanel = true;
            auto children = g_wifiPanel.Children();
            children.Clear();
            children.Append(MakePanelTitle(L"Wi-Fi"));
            children.Append(MakeStatusText(L"Loading…"));
            g_populatingPanel = false;
        }
        // Fetch in background
        RunInBackground([] {
            auto status = wifi::GetStatus();
            auto networks = wifi::EnumerateNetworks();
RunOnUiThread([status, networks = std::move(networks)]() mutable {
    g_wifiNetworks = std::move(networks);
    g_wifiPasswordPrompt = false;
    g_wifiPromptError.clear();
    PopulateWifiPanel();
    StartWifiScan();
    EnsureAutoRefreshTimers();
    if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer.Start();
});
        });
    });


        rightPanel.Children().Append(g_wifiButton);
    }

    {
        auto icon = BuildVectorIcon(nullptr, L"", icons::kBluetoothStroke, 24, 18, 1.8);

    g_bluetoothButton = MakeControlButton(L"BluetoothButton", icon, g_bluetoothFlyout, [] {
        // Show loading
        if (g_bluetoothPanel) {
            g_populatingPanel = true;
            auto children = g_bluetoothPanel.Children();
            children.Clear();
            children.Append(MakePanelTitle(L"Bluetooth"));
            children.Append(MakeStatusText(L"Loading…"));
            g_populatingPanel = false;
        }
        RefreshBluetoothRadioState(); // update radio state in background
        // First load paired devices (fast, no scan)
        RunInBackground([] {
            auto devices = bluetooth::Enumerate(false); // no inquiry, instant
            RunOnUiThread([devices = std::move(devices)]() mutable {
                g_bluetoothDevices = std::move(devices);
                PopulateBluetoothPanel();
                // Do a single scan for new devices (no auto-refresh loop)
                StartBluetoothScan(true);
            });
        });
    });

        rightPanel.Children().Append(g_bluetoothButton);
    }

    // Battery button. The percentage is drawn inside the icon, so no separate
    // text label is added next to it.
    {
        BatteryInfo info = GetBatteryInfo();
        auto batteryIcon = BuildBatteryIcon(20, info.percentage, info.charging);
        if (batteryIcon) {
            batteryIcon.VerticalAlignment(VerticalAlignment::Center);
        }
        g_batteryButton = MakeControlButton(L"BatteryButton", batteryIcon, g_batteryFlyout,
                                            [] { PopulateBatteryPanel(); });
        rightPanel.Children().Append(g_batteryButton);
    }

    // Resource usage button (CPU/RAM/GPU)
    {
        auto resourceButton = MakeGhostButton(L"ResourceButton", g_settings.cornerRadius);
        resourceButton.VerticalAlignment(VerticalAlignment::Stretch);
        resourceButton.Margin(Thickness{5, 4, 5, 4});
        resourceButton.Padding(Thickness{7, 0, 7, 0});
        resourceButton.HorizontalContentAlignment(HorizontalAlignment::Center);
        {
            int metrics = (g_settings.showCpuUsage ? 1 : 0) +
                          (g_settings.showRamUsage ? 1 : 0) +
                          (g_settings.showGpuUsage ? 1 : 0);
            if (metrics < 1) metrics = 1;
            resourceButton.Width(metrics * 84.0 + 10.0);
        }
        // Placeholder text; will be updated periodically
        resourceButton.Content(MakeText(nullptr, L"CPU: 0% RAM: 0% GPU: 0%", 12));

        // Create the flyout with toggles
        g_resourceFlyout = MakeControlFlyout(L"ResourceFlyoutRoot", g_resourcePanel);

g_resourceFlyout.Opening([](auto&&, auto&&) {
    try {
        PopulateResourceFlyout();
        ApplyAllControlStyles();
        if (!g_resourceFlyoutTimer) {
            g_resourceFlyoutTimer = DispatcherTimer();
            g_resourceFlyoutTimer.Interval(std::chrono::seconds(1));
            g_resourceFlyoutTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
                try {
                    UpdateResourceFlyoutContent();
                } catch (...) {}
            });
        }
        g_resourceFlyoutTimer.Start();
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Resource flyout opening failed: %s", ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Resource flyout opening failed (unknown error)");
    }
});

        g_resourceFlyout.Closed([](auto&&, auto&&) {
            if (g_resourceFlyoutTimer) g_resourceFlyoutTimer.Stop();
        });
resourceButton.Flyout(g_resourceFlyout);
resourceButton.Click([](auto&&, auto&&) {
    if (g_resourceFlyout && !g_resourceFlyout.IsOpen()) {
        g_resourceFlyout.ShowAt(g_resourceButton);
    }
});

g_resourceButton = resourceButton;
RegisterNamed(L"ResourceButton", resourceButton);
g_centerPanel.Children().Append(resourceButton);
    }



    {
        auto clockButton = MakeGhostButton(L"ClockButton", g_settings.cornerRadius);
        clockButton.VerticalAlignment(VerticalAlignment::Stretch);
        clockButton.Margin(Thickness{3, 2, 6, 2});
        clockButton.Padding(Thickness{10, 0, 10, 0});
        clockButton.HorizontalContentAlignment(HorizontalAlignment::Center);

        auto clockText = MakeText(L"ClockText", FormatClockText(), 14);
        clockText.TextAlignment(TextAlignment::Center);
        clockText.LineHeight(15);
        clockButton.Content(clockText);
        clockButton.Click([](auto&&, auto&&) {
            RunShellCommand(L"ms-actioncenter:");
        });

        RegisterNamed(L"ClockText", clockText);
        RegisterNamed(L"ClockButton", clockButton);
        g_clockPanel.Children().Append(clockButton);
    }

    // ---- Weather button ---------------------------------------------------
    {
        auto weatherIcon =
            BuildVectorIcon(L"WeatherIcon", icons::kWeatherCloudFill,
                            L"", 24, 20, 1.6);
        auto weatherButton = MakeGhostButton(L"WeatherButton", g_settings.cornerRadius);
        weatherButton.VerticalAlignment(VerticalAlignment::Stretch);
        weatherButton.Margin(Thickness{5, 4, 5, 4});
        weatherButton.Padding(Thickness{8, 0, 8, 0});
        weatherButton.HorizontalContentAlignment(HorizontalAlignment::Center);
        wuxc::StackPanel weatherContent;
        weatherContent.Name(L"WeatherButtonContent");
        weatherContent.Orientation(wuxc::Orientation::Horizontal);
        weatherContent.Spacing(0);
        weatherContent.VerticalAlignment(VerticalAlignment::Center);
        weatherButton.Content(weatherContent);
        RegisterNamed(L"WeatherButtonContent", weatherContent);
        RefreshWeatherButtonContent();

        wuxc::Flyout weatherFlyout;
        weatherFlyout = MakeControlFlyout(L"WeatherFlyoutRoot", g_weatherPanel);
        weatherFlyout.Opening([](auto&&, auto&&) {
            PopulateWeatherPanel();
            if (!g_settings.weatherLatitude.empty() &&
                !g_settings.weatherLongitude.empty()) {
                weather::EnsureFresh();
            }
        });
        weatherFlyout.Closed([](auto&&, auto&&) {
            // Reset picker state so the flyout reopens on the weather card.
            if (!g_settings.weatherLocationName.empty()) {
                g_weatherLocationPickerActive = false;
            }
            g_weatherSearchResults.clear();
            g_weatherSearchQuery.clear();
        });
        weatherButton.Flyout(weatherFlyout);

        RegisterNamed(L"WeatherButton", weatherButton);
        g_centerPanel.Children().Append(weatherButton);

        // Kick off an initial background fetch when we have a saved location.
        if (!g_settings.weatherLatitude.empty() &&
            !g_settings.weatherLongitude.empty()) {
            weather::EnsureFresh();
        }
    }

    // ---- Recycle bin button ----------------------------------------------
    {
        auto binIcon = BuildVectorIcon(L"RecycleBinIcon",
                                       icons::kRecycleBinBody,
                                       icons::kRecycleBinLid, 24, 18, 1.6);
        auto binButton = MakeControlButton(
            L"RecycleBinButton", binIcon, wuxc::Flyout{nullptr},
            nullptr);
        wuxc::Flyout binFlyout =
            MakeControlFlyout(L"RecycleBinFlyoutRoot", g_recycleBinPanel);
        binButton.Flyout(binFlyout);
        binFlyout.Opening([](auto&&, auto&&) { PopulateRecycleBinPanel(); });
        binFlyout.Closed([](auto&&, auto&&) {
            g_recycleBinConfirming = false;
        });
        rightPanel.Children().Append(binButton);
    }

    {
        auto gearIcon = BuildVectorIcon(nullptr, L"",
            L"M19.14 12.94c.04-.3.06-.61.06-.94 0-.32-.02-.64-.06-.94l2.03-1.58c.18-.14.23-.41.12-.61l-1.92-3.32c-.12-.22-.37-.29-.59-.22l-2.39.96c-.5-.38-1.03-.7-1.62-.94l-.36-2.54c-.04-.24-.24-.41-.48-.41h-3.84c-.24 0-.43.17-.47.41l-.36 2.54c-.59.24-1.13.57-1.62.94l-2.39-.96c-.22-.08-.47 0-.59.22L2.74 8.87c-.12.21-.08.47.12.61l2.03 1.58c-.04.3-.06.62-.06.94s.02.64.06.94l-2.03 1.58c-.18.14-.23.41-.12.61l1.92 3.32c.12.22.37.29.59.22l2.39-.96c.5.38 1.03.7 1.62.94l.36 2.54c.04.24.24.41.48.41h3.84c.24 0 .43-.17.47-.41l.36-2.54c.59-.24 1.13-.56 1.62-.94l2.39.96c.22.08.47 0 .59-.22l1.92-3.32c.12-.22.07-.47-.12-.61l-2.01-1.58zM12 15.6c-1.98 0-3.6-1.62-3.6-3.6s1.62-3.6 3.6-3.6 3.6 1.62 3.6 3.6-1.62 3.6-3.6 3.6z",
            24, 18, 1.7);
        // Plain ghost button — separate from the Control Center cluster so it
        // doesn't inherit its hover/flyout styling. Placement is entirely
        // driven by leftItems/centerItems/rightItems.
        auto settingsButton = MakeGhostButton(L"SettingsButton", g_settings.cornerRadius);
        settingsButton.VerticalAlignment(VerticalAlignment::Stretch);
        settingsButton.Margin(Thickness{5, 4, 5, 4});
        settingsButton.Padding(Thickness{7, 0, 7, 0});
        settingsButton.HorizontalContentAlignment(HorizontalAlignment::Center);
        settingsButton.Content(gearIcon);
        settingsButton.Click([](auto&&, auto&&) {
            OpenTopBarSettingsWindow();
        });
        RegisterNamed(L"SettingsButton", settingsButton);
        g_settingsButton = settingsButton;
        // Default to the right panel; the placement pass may relocate it.
        rightPanel.Children().Append(settingsButton);
    }

    g_trayPanel = rightPanel;

    {
        std::map<std::wstring, UIElement> pool;
        auto harvest = [&](wuxc::StackPanel panel) {
            std::vector<UIElement> take;
            for (auto&& child : panel.Children()) {
                if (auto fe = child.try_as<FrameworkElement>()) {
                    std::wstring n(fe.Name());
                    if (!n.empty() &&
                        std::find(kMovableItems.begin(), kMovableItems.end(), n) !=
                            kMovableItems.end()) {
                        pool.insert_or_assign(n, child);
                        take.push_back(child);
                    }
                }
            }
            for (auto& c : take) {
                uint32_t idx;
                if (panel.Children().IndexOf(c, idx)) panel.Children().RemoveAt(idx);
            }
        };
        harvest(g_leftPanel);
        harvest(g_centerPanel);
        harvest(g_clockPanel);
        harvest(g_trayPanel);

        // Place exactly as stored. The storage is now authoritative — user
        // moves via right-click Move Left/Right or the UI Alignment page are
        // honored verbatim, no post-pass reshuffle.
        auto place = [&](wuxc::StackPanel panel, const std::wstring& csv) {
            if (!panel) return;
            for (const auto& name : SplitItemList(csv)) {
                auto it = pool.find(name);
                if (it != pool.end()) {
                    panel.Children().Append(it->second);
                    pool.erase(it);
                }
            }
        };
        place(g_leftPanel,   g_settings.leftItems);
        place(g_centerPanel, g_settings.centerItems);
        place(g_trayPanel,   g_settings.rightItems);

        for (auto& leftover : pool) {
            if (g_trayPanel) g_trayPanel.Children().Append(leftover.second);
        }
    }

    // Every movable button gets the right-click Move Left/Right menu,
    // regardless of which panel the placement pass put it in.
    auto attachMenusToPanel = [&](wuxc::StackPanel panel) {
        if (!panel) return;
        for (auto&& child : panel.Children()) {
            if (auto button = child.try_as<wuxc::Button>()) {
                std::wstring childName(button.Name());
                if (!childName.empty() &&
                    std::find(kMovableItems.begin(), kMovableItems.end(), childName) !=
                        kMovableItems.end()) {
                    AttachTrayReorderMenu(button, childName);
                }
            }
        }
    };
    attachMenusToPanel(g_leftPanel);
    attachMenusToPanel(g_centerPanel);
    attachMenusToPanel(g_clockPanel);
    attachMenusToPanel(g_trayPanel);

    wuxc::Grid::SetColumn(rightPanel, 2);
    root.Children().Append(rightPanel);
    RegisterNamed(L"ControlCenterPanel", rightPanel);
    RegisterNamed(L"TrayPanel", rightPanel);
    RegisterNamed(L"TopBarRoot", root);

    g_rootElement = barRoot;   // Outer root includes wallpaper
    return barRoot;
}

// ============================================================================
// Monitors and the AppBar reservation
// ============================================================================

struct MonitorEnumState {
    int wanted = 0;
    int seen = 0;
    HMONITOR result = nullptr;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
    auto* state = reinterpret_cast<MonitorEnumState*>(lParam);
    state->seen++;
    if (state->seen == state->wanted) {
        state->result = monitor;
        return FALSE;
    }
    return TRUE;
}

HMONITOR GetBarMonitor() {
    if (g_settings.monitorIndex > 0) {
        MonitorEnumState state;
        state.wanted = g_settings.monitorIndex;
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                            reinterpret_cast<LPARAM>(&state));
        if (state.result) {
            return state.result;
        }
    }
    POINT origin{0, 0};
    return MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
}

// brightness::BarMonitor was forward-declared inside that namespace so the DDC
// path could target the display the bar is actually on.
HMONITOR brightness::BarMonitor() {
    return GetBarMonitor();
}

RECT GetBarMonitorRect() {
    MONITORINFO info{sizeof(MONITORINFO)};
    HMONITOR monitor = GetBarMonitor();
    if (monitor && GetMonitorInfo(monitor, &info)) {
        return info.rcMonitor;
    }
    RECT fallback{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    return fallback;
}

double GetBarDpiScale() {
    UINT dpiX = 96, dpiY = 96;
    if (HMONITOR monitor = GetBarMonitor()) {
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            return dpiX / 96.0;
        }
    }
    return 1.0;
}

// Reserves the strip at the top of the work area so maximized windows stop
// below the bar instead of underneath it.
void PositionAppBar(HWND hwnd, int heightPx) {
    RECT monitorRect = GetBarMonitorRect();

    APPBARDATA data{};
    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    data.uEdge = ABE_TOP;
    data.rc.left = monitorRect.left;
    data.rc.right = monitorRect.right;
    data.rc.top = monitorRect.top;
    data.rc.bottom = monitorRect.top + heightPx;

    SHAppBarMessage(ABM_QUERYPOS, &data);
    data.rc.bottom = data.rc.top + heightPx;
    SHAppBarMessage(ABM_SETPOS, &data);

    SetWindowPos(hwnd, nullptr, data.rc.left, data.rc.top, data.rc.right - data.rc.left,
                 data.rc.bottom - data.rc.top, SWP_NOZORDER | SWP_NOACTIVATE);

    if (g_islandHwnd) {
        SetWindowPos(g_islandHwnd, nullptr, 0, 0, data.rc.right - data.rc.left,
                     data.rc.bottom - data.rc.top, SWP_NOZORDER | SWP_SHOWWINDOW);
    }
}

void RegisterAppBar(HWND hwnd) {
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    data.uCallbackMessage = WM_APPBAR_CALLBACK;
    if (SHAppBarMessage(ABM_NEW, &data)) {
        g_appBarRegistered = true;
    }
}

void UnregisterAppBar(HWND hwnd) {
    if (!g_appBarRegistered) {
        return;
    }
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    data.hWnd = hwnd;
    SHAppBarMessage(ABM_REMOVE, &data);
    g_appBarRegistered = false;
}

// ============================================================================
// Background Blur and rounded corners
// ============================================================================

// DWM window corner preference (Windows 11 build 22000+)
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

void ApplyRoundedCornersToWindow(HWND hwnd) {
    (void)hwnd;
}

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;  // ABGR
    DWORD AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19,
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute() {
    static SetWindowCompositionAttribute_t function = [] {
        HMODULE module = GetModuleHandle(L"user32.dll");
        return module ? reinterpret_cast<SetWindowCompositionAttribute_t>(
                            GetProcAddress(module, "SetWindowCompositionAttribute"))
                      : nullptr;
    }();
    return function;
}



void ApplyBlurToWindow(HWND hwnd) {
    (void)hwnd;
}

void ApplyWindowBackdrop(HWND hwnd) {
    ApplyBlurToWindow(hwnd);
}

// Modern Windows 11 chrome for the settings window. Everything here goes
// through DwmSetWindowAttribute — the DWM compositor draws the frame, dark
// titlebar, rounded corners and Mica backdrop itself.
//
// IMPORTANT: do NOT call ApplyBlurToWindow / SetWindowCompositionAttribute
// (the WCA_ACCENT_POLICY / ACCENT_ENABLE_BLURBEHIND path) on this window.
// That API switches the window into the legacy compositing mode, which is
// exactly what causes DWM to serve the classic Windows 7 "Aero Glass" frame
// (blue tint, puffy top corners, square system buttons).
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMWA_NCRENDERING_POLICY
#define DWMWA_NCRENDERING_POLICY 2
#endif

void ApplyModernWindowChrome(HWND hwnd) {
    //   1. dark titlebar
    //   2. (Win11) rounded corners — ignored on Win10
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    DWORD corner = 2;   // DWMWCP_ROUND
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
}



// Popup HWNDs that have already had the DWM accent blur applied. The accent
// policy must be set exactly once per popup HWND: re-applying it every time a
// flyout opens makes the compositor fight with XAML's own background rendering,
// which is what caused a user-set FlyoutPresenter background to flicker or
// vanish. Setting it a single time keeps the blur and stops the flicker.
std::set<HWND> g_blurredPopupHwnds;

void ApplyBlurToAllOpenPopups() {
    // Drop dead HWNDs so the set doesn't grow unbounded across flyout rebuilds.
    for (auto it = g_blurredPopupHwnds.begin(); it != g_blurredPopupHwnds.end();) {
        if (!IsWindow(*it)) {
            it = g_blurredPopupHwnds.erase(it);
        } else {
            ++it;
        }
    }

    EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId() || hwnd == g_topBarHwnd) {
            return TRUE;
        }
        // Never touch the settings window or any of its child HWNDs — the
        // legacy ACCENT_POLICY path flips DWM back to Aero compositing.
        if (topbar_settings_window::s_hwnd && IsWindow(topbar_settings_window::s_hwnd) &&
            (hwnd == topbar_settings_window::s_hwnd ||
             GetAncestor(hwnd, GA_ROOTOWNER) == topbar_settings_window::s_hwnd)) {
            return TRUE;
        }
        wchar_t className[256] = {0};
        GetClassName(hwnd, className, ARRAYSIZE(className));
        if (!(wcsstr(className, L"Popup") || wcsstr(className, L"Xaml") ||
              wcsstr(className, L"Menu") || wcsstr(className, L"Flyout") ||
              wcsstr(className, L"ToolWindow"))) {
            return TRUE;
        }
        if (g_blurredPopupHwnds.insert(hwnd).second) {
            ApplyBlurToWindow(hwnd);
            ApplyRoundedCornersToWindow(hwnd);
        }
        return TRUE;
    }, 0);
}

// The island inserts its own root above whatever content we set, and those
// elements carry a theme background. Walking up from our root and clearing
// every background we find is what lets the configured translucency actually
// reach the desktop. The count is logged deliberately: if the black plate is
// still there, the number here says whether the walk found anything at all.
void StripInheritedIslandBackgrounds() {
    if (!g_rootElement) {
        return;
    }

    int cleared = 0;
    int visited = 0;
    auto transparent = MakeBrush(0, 0, 0, 0);

    DependencyObject current = g_rootElement;
    while (current) {
        DependencyObject parent{nullptr};
        try {
            parent = wuxm::VisualTreeHelper::GetParent(current);
        } catch (...) {
            break;
        }
        if (!parent) {
            break;
        }
        visited++;

        try {
            if (auto panel = parent.try_as<wuxc::Panel>()) {
                if (panel.Background()) {
                    panel.Background(transparent);
                    cleared++;
                }
            } else if (auto border = parent.try_as<wuxc::Border>()) {
                if (border.Background()) {
                    border.Background(transparent);
                    cleared++;
                }
            } else if (auto presenter = parent.try_as<wuxc::ContentPresenter>()) {
                if (presenter.Background()) {
                    presenter.Background(transparent);
                    cleared++;
                }
            } else if (auto control = parent.try_as<wuxc::Control>()) {
                if (control.Background()) {
                    control.Background(transparent);
                    cleared++;
                }
            }
        } catch (...) {
        }

        current = parent;
    }

    Wh_Log(L"Island background strip: visited %d ancestor(s), cleared %d background(s)",
           visited, cleared);
}

// ============================================================================
// Host window
// ============================================================================

void CALLBACK WindowEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (event == EVENT_OBJECT_CREATE || event == EVENT_OBJECT_DESTROY ||
        event == EVENT_OBJECT_NAMECHANGE || event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE) {
        RunOnUiThread([] {
            if (!g_taskRefreshTimer) {
                g_taskRefreshTimer = DispatcherTimer();
                g_taskRefreshTimer.Interval(std::chrono::milliseconds(200));
                g_taskRefreshTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
                    g_taskRefreshTimer.Stop();
                    RefreshTaskList(false);
                });
            }
            g_taskRefreshTimer.Stop();
            g_taskRefreshTimer.Start();
        });
    }
}

void UpdateClockText() {
    auto it = g_namedElements.find(L"ClockText");
    if (it == g_namedElements.end()) {
        return;
    }
    if (auto text = it->second.try_as<wuxc::TextBlock>()) {
        text.Text(winrt::hstring(FormatClockText()));
    }
}

LRESULT CALLBACK TopBarWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        // Explorer restarted and dropped every AppBar registration with it.
        g_appBarRegistered = false;
        RegisterAppBar(hwnd);
        PositionAppBar(hwnd, g_barHeightPx);
        return 0;
    }

    switch (message) {
        case WM_ERASEBKGND:
            // Never let DefWindowProc paint the class brush; that plate is
            // exactly the black rectangle behind the bar.
            return 1;

        case WM_APPBAR_CALLBACK:
            switch (wParam) {
                case ABN_POSCHANGED:
                    PositionAppBar(hwnd, g_barHeightPx);
                    break;
                case ABN_FULLSCREENAPP:
                    {
                        // lParam carries TRUE when a full-screen app is opening
                        // and FALSE when it is closing. Win+D (Show Desktop)
                        // makes the shell incorrectly report TRUE even though
                        // it's really the desktop being shown. Verify the
                        // foreground window is actually a fullscreen app
                        // covering the whole monitor.
                        bool isFullscreen = (lParam != 0);
                        if (isFullscreen) {
                            HWND fg = GetForegroundWindow();
                            bool verified = false;
                            if (fg) {
                                wchar_t cls[256] = {0};
                                GetClassName(fg, cls, ARRAYSIZE(cls));
                                if (wcscmp(cls, L"Progman") != 0 &&
                                    wcscmp(cls, L"WorkerW") != 0) {
                                    RECT fgRect{};
                                    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
                                    MONITORINFO mi{};
                                    mi.cbSize = sizeof(mi);
                                    if (GetWindowRect(fg, &fgRect) &&
                                        GetMonitorInfo(mon, &mi)) {
                                        if (fgRect.left <= mi.rcMonitor.left &&
                                            fgRect.top <= mi.rcMonitor.top &&
                                            fgRect.right >= mi.rcMonitor.right &&
                                            fgRect.bottom >= mi.rcMonitor.bottom) {
                                            verified = true;
                                        }
                                    }
                                }
                            }
                            isFullscreen = verified;
                        }
                        g_fullScreenAppActive = isFullscreen;
                        SetWindowPos(hwnd,
                                     isFullscreen ? HWND_NOTOPMOST : HWND_TOPMOST,
                                     0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    }
                    break;
            }
            return 0;

        case WM_SIZE:
            // If we get minimized, restore immediately (unless shutting down).
            // Never allow the shell to minimize the bar via Win+D.
            if (wParam == SIZE_MINIMIZED && !g_allowHide) {
                ShowWindow(hwnd, SW_RESTORE);
                ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                return 0;
            }
            if (g_islandHwnd) {
                SetWindowPos(g_islandHwnd, nullptr, 0, 0, LOWORD(lParam), HIWORD(lParam),
                             SWP_NOZORDER | SWP_SHOWWINDOW);
            }
            return 0;

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            g_dpiScale = GetBarDpiScale();
            g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);
            PositionAppBar(hwnd, g_barHeightPx);
            RefreshTaskList(true);
        RefreshBluetoothRadioState(); // initial radio state
            return 0;

        case WM_SETTINGCHANGE:
            // Theme switches change what the tray and the panels should look
            // like; the panels rebuild themselves when next opened, so only the
            // clock needs touching here.
            UpdateClockText();

            // Reload the wallpaper when Windows tells us the wallpaper changed.
            // The lParam will be "Wallpaper" (case-sensitive? usually it's "Wallpaper").
            if (lParam && wcscmp(reinterpret_cast<PCWSTR>(lParam), L"Wallpaper") == 0) {
                if (g_wallpaperLayer) {
                    g_wallpaperLayer.Background(GetWallpaperBrush());
                    Wh_Log(L"TopBar: Wallpaper background updated.");
                }
            }
            return 0;

        case WM_HOTKEY:
            switch (wParam) {
                case HOTKEY_ID_DISPLAY:
                    ToggleFlyout(g_displayFlyout, g_displayButton);
                    break;
                case HOTKEY_ID_SOUND:
                    ToggleFlyout(g_soundFlyout, g_soundButton);
                    break;
                case HOTKEY_ID_WIFI:
                    ToggleFlyout(g_wifiFlyout, g_wifiButton);
                    break;
                case HOTKEY_ID_BLUETOOTH:
                    ToggleFlyout(g_bluetoothFlyout, g_bluetoothButton);
                    break;
                case HOTKEY_ID_RESOURCE:
                    ToggleFlyout(g_resourceFlyout, g_resourceButton);
                    break;
                case HOTKEY_ID_BATTERY:
                    ToggleFlyout(g_batteryFlyout, g_batteryButton);
                    break;

                case HOTKEY_ID_START_MENU:
                    if (g_startContextMenu) {
                        auto it = g_namedElements.find(L"StartButton");
                        if (it != g_namedElements.end()) {
                            g_startContextMenu.ShowAt(it->second.as<FrameworkElement>());
                        }
                    }
                    break;
                case HOTKEY_ID_TASK_MENU:
                    if (g_taskContextMenu) {
                        auto it = g_namedElements.find(L"TaskListPanel");
                        if (it != g_namedElements.end()) {
                            g_taskContextMenu.ShowAt(it->second.as<FrameworkElement>());
                        }
                    }
                    break;
                case HOTKEY_ID_WEATHER:
                    if (auto it = g_namedElements.find(L"WeatherButton");
                        it != g_namedElements.end()) {
                        if (auto button = it->second.try_as<wuxc::Button>()) {
                            if (auto flyout = button.Flyout()) {
                                ToggleFlyout(flyout.try_as<wuxc::Flyout>(), button);
                            }
                        }
                    }
                    break;
                case HOTKEY_ID_RECYCLEBIN:
                    if (auto it = g_namedElements.find(L"RecycleBinButton");
                        it != g_namedElements.end()) {
                        if (auto button = it->second.try_as<wuxc::Button>()) {
                            if (auto flyout = button.Flyout()) {
                                ToggleFlyout(flyout.try_as<wuxc::Flyout>(), button);
                            }
                        }
                    }
                    break;
            }
            return 0;

        case WM_TIMER:
            if (wParam == kAppBarInitTimerId) {
                KillTimer(hwnd, kAppBarInitTimerId);
                PositionAppBar(hwnd, g_barHeightPx);
                return 0;
            }
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == SC_MINIMIZE) {
                return 0;   // block minimization
            }
            break;



        case WM_SHOWWINDOW:
            if (wParam == FALSE && !g_allowHide && !g_fullScreenAppActive) {
                ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, FALSE, sizeof(BOOL));
                // Immediately reapply topmost
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                return 0;
            }
            break;

        case WM_DESTROY:
            UnregisterHotKey(hwnd, HOTKEY_ID_DISPLAY);
            UnregisterHotKey(hwnd, HOTKEY_ID_SOUND);
            UnregisterHotKey(hwnd, HOTKEY_ID_WIFI);
            UnregisterHotKey(hwnd, HOTKEY_ID_BLUETOOTH);
            UnregisterHotKey(hwnd, HOTKEY_ID_RESOURCE);
            UnregisterHotKey(hwnd, HOTKEY_ID_BATTERY);
            UnregisterHotKey(hwnd, HOTKEY_ID_WEATHER);
            UnregisterHotKey(hwnd, HOTKEY_ID_RECYCLEBIN);
            UnregisterHotKey(hwnd, HOTKEY_ID_START_MENU);
            UnregisterHotKey(hwnd, HOTKEY_ID_TASK_MENU);
            UnregisterAppBar(hwnd);
            PostQuitMessage(0);
            return 0;

        case WM_WINDOWPOSCHANGING:
            {
                WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
                // Block hide whenever not shutting down. Fullscreen apps should
                // cover the bar via z-order, never hide it.
                if ((wp->flags & SWP_HIDEWINDOW) && !g_allowHide) {
                    wp->flags &= ~SWP_HIDEWINDOW; // cancel the hide
                }
            }
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void ForegroundEventProcInstall() {
    if (g_foregroundHook) {
        return;
    }
    g_foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                       nullptr, ForegroundEventProc, 0, 0,
                                       WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_foregroundHook) {
        Wh_Log(L"SetWinEventHook failed; click-to-minimize will not work");
    }

    // Additional hook for window creation/destruction/rename to refresh task list
    g_windowEventHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE,
                                        nullptr, WindowEventProc, 0, 0,
                                        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    g_windowEventNameHook = SetWinEventHook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE,
                                            nullptr, WindowEventProc, 0, 0,
                                            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

void ShowElementTargetUnderCursor() {
    POINT cursor;
    if (!GetCursorPos(&cursor)) return;

    HWND hWndUnderCursor = WindowFromPoint(cursor);
    if (!hWndUnderCursor) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWndUnderCursor, &pid);
    if (pid != GetCurrentProcessId()) {
        Wh_Log(L"[Ctrl+D] cursor over a foreign window");
        return;
    }

    POINT clientPt = cursor;
    ScreenToClient(hWndUnderCursor, &clientPt);
    winrt::Windows::Foundation::Point pt{(float)clientPt.x, (float)clientPt.y};

    FrameworkElement hit{nullptr};

    auto tryRoot = [&](FrameworkElement const& root) {
        if (hit || !root) return;
        try {
            for (auto const& el : wuxm::VisualTreeHelper::FindElementsInHostCoordinates(pt, root)) {
                if (auto fe = el.try_as<FrameworkElement>()) {
                    hit = fe;
                    return;
                }
            }
        } catch (...) {}
    };

    // Flyouts are on top of the topbar; search them newest-first so the
    // element the user is actually pointing at wins.
    for (auto it = g_detachedStyleRoots.rbegin();
         it != g_detachedStyleRoots.rend() && !hit; ++it) {
        tryRoot(*it);
    }
    if (!hit && g_rootElement) {
        tryRoot(g_rootElement.try_as<FrameworkElement>());
    }

    if (!hit) {
        Wh_Log(L"[Ctrl+D] no XAML element under cursor");
        return;
    }

    std::wstring fullClass(winrt::get_class_name(hit));
    std::wstring shortClass = fullClass;
    if (auto dot = fullClass.rfind(L'.'); dot != std::wstring::npos) {
        shortClass = fullClass.substr(dot + 1);
    }
    std::wstring elemName(hit.Name());

    std::wstring target = shortClass;
    if (!elemName.empty()) target += L"#" + elemName;

    Wh_Log(L"[Ctrl+D] %s", target.c_str());

    try {
        auto tb = MakeText(nullptr, target, 13, true);
        auto border = wuxc::Border();
        border.Background(MakeBrush(0xE0, 0x20, 0x20, 0x20));
        border.BorderBrush(MakeBrush(0xFF, 0x60, 0x60, 0x60));
        border.BorderThickness(Thickness{1, 1, 1, 1});
        border.Padding(Thickness{10, 6, 10, 6});
        border.CornerRadius(MakeCorner(6));
        border.Child(tb);

        static wuxc::Flyout s_targetFlyout{nullptr};
        if (s_targetFlyout && s_targetFlyout.IsOpen()) {
            s_targetFlyout.Hide();
        }
        s_targetFlyout = wuxc::Flyout();
        s_targetFlyout.Content(border);
        s_targetFlyout.ShouldConstrainToRootBounds(false);
        s_targetFlyout.ShowAt(hit);
    } catch (...) {}
}

DWORD WINAPI TopBarThreadProc(LPVOID) {
    Wh_Log(L"TopBar: TopBarThreadProc started.");
    try {
        // No delay – start immediately.

        winrt::init_apartment(winrt::apartment_type::single_threaded);

        WNDCLASSEX windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = TopBarWndProc;
        HMODULE modModule = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCWSTR)&TopBarWndProc, &modModule);
        windowClass.hInstance = modModule;
        g_modModule = modModule;
        windowClass.lpszClassName = kWindowClassName;
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        // No class brush at all: with one, DefWindowProc paints an opaque plate
        // before the island ever draws.
        windowClass.hbrBackground = nullptr;

        // If the class still exists from a previous failed unload, unregister it first.
        if (g_modModule) {
            UnregisterClass(kWindowClassName, g_modModule);
        }

        (void)RegisterClassEx(&windowClass);

        g_dpiScale = GetBarDpiScale();
        g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);
        RECT monitorRect = GetBarMonitorRect();

        g_topBarHwnd = CreateWindowEx(
            WS_EX_TOOLWINDOW, kWindowClassName, L"Windhawk Top Bar", WS_POPUP, monitorRect.left,
            monitorRect.top, monitorRect.right - monitorRect.left, g_barHeightPx, nullptr, nullptr,
            windowClass.hInstance, nullptr);
        if (!g_topBarHwnd) {
            Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
            // Clean up the window class registration
            if (g_modModule) {
                UnregisterClass(kWindowClassName, g_modModule);
                g_modModule = nullptr;
            }
            return 1;
        }

        ApplyWindowBackdrop(g_topBarHwnd);

        try {
            g_xamlManager = wuxh::WindowsXamlManager::InitializeForCurrentThread();
            // Force dark theme for the whole app
            try {
                auto app = Application::Current();
                if (app) {
                    app.RequestedTheme(ApplicationTheme::Dark);
                }
            } catch (...) {}
            g_desktopSource = wuxh::DesktopWindowXamlSource();

            auto native = g_desktopSource.as<IDesktopWindowXamlSourceNative>();
            winrt::check_hresult(native->AttachToWindow(g_topBarHwnd));
            winrt::check_hresult(native->get_WindowHandle(&g_islandHwnd));

            g_menuShellTemplate = BuildFlyoutShellTemplate(true);
        } catch (winrt::hresult_error const& ex) {
            // The usual cause is a host process whose manifest has no
            // <maxversiontested>; XAML Islands refuse to initialize there. This is
            // why the bar has to be hosted by a real explorer.exe.
            Wh_Log(L"XAML Islands failed to initialize: %08X - %s",
                   static_cast<unsigned int>(ex.code().value), ex.message().c_str());
            DestroyWindow(g_topBarHwnd);
            g_topBarHwnd = nullptr;
            // Clean up window class
            if (g_modModule) {
                UnregisterClass(kWindowClassName, g_modModule);
                g_modModule = nullptr;
            }
            return 1;
        }

        g_uiDispatcherQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (!g_uiDispatcherQueue) {
            Wh_Log(L"No DispatcherQueue on the UI thread; falling back to the XAML dispatcher");
        }

        SetWindowPos(g_islandHwnd, nullptr, 0, 0, monitorRect.right - monitorRect.left,
                     g_barHeightPx, SWP_NOZORDER | SWP_SHOWWINDOW);

        // Menu resources first: the flyouts and menus built below pick them up as
        // they are created.
        InstallGlobalMenuResources();

        auto content = BuildTopBarContent();
        g_desktopSource.Content(content);
        
        // Make the XAML island background transparent so the system blur shows through.
        auto xamlSourceUnknown = g_desktopSource.as<::IUnknown>();
        winrt::com_ptr<IXamlSourceTransparency> transparency;
        const IID kIID_IXamlSourceTransparency = {0x06636c29, 0x5a17, 0x458d, {0x8e, 0xa2, 0x24, 0x22, 0xd9, 0x97, 0xa9, 0x22}};
        if (SUCCEEDED(xamlSourceUnknown->QueryInterface(kIID_IXamlSourceTransparency, transparency.put_void())) && transparency) {
            transparency->put_IsBackgroundTransparent(TRUE);
        } else {
            Wh_Log(L"Failed to set IXamlSourceTransparency.IsBackgroundTransparent");
        }

        // After BuildTopBarContent, which clears g_namedElements -- building the
        // menus earlier would lose their registrations.
        BuildStartContextMenu();
        BuildTaskContextMenu();
        BuildAppTitleContextMenu();

        ApplyVisibilitySettings();
        ApplyAllControlStyles();
        UpdateResourceButton();

        // Once the content is live, clear whatever opaque roots the island put
        // above it.
        StripInheritedIslandBackgrounds();

        // Last-writer-wins: re-apply user styles one dispatcher turn later,
        // after every `Loaded` handler that was queued during the tree build
        // has run. Without this the built-in blur handlers occasionally stomp
        // on a user WindhawkBlur that was set by ApplyAllControlStyles above.
        RunOnUiThread([] {
            try { ApplyAllControlStyles(); } catch (...) {}
        });

        ShowWindow(g_topBarHwnd, SW_SHOWNOACTIVATE);
        // Set topmost so the bar stays above the desktop when Win+D is pressed.
        // When a fullscreen app starts, ABN_FULLSCREENAPP will remove topmost.
        SetWindowPos(g_topBarHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        UpdateWindow(g_topBarHwnd);
        // Re-apply backdrop after window becomes visible (fixes blur on top bar)
        ApplyWindowBackdrop(g_topBarHwnd);
    // Register global hotkeys (Ctrl+Alt+1..5) to open control flyouts.
        // Hotkeys are disabled by default. Ctrl+Alt+digit is a common app binding.
        // Uncomment the lines below to re-enable them.
        if (g_settings.enableHotkeys) {
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_DISPLAY, MOD_CONTROL | MOD_ALT, '1');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_SOUND, MOD_CONTROL | MOD_ALT, '2');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_WIFI, MOD_CONTROL | MOD_ALT, '3');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_BLUETOOTH, MOD_CONTROL | MOD_ALT, '4');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_RESOURCE, MOD_CONTROL | MOD_ALT, '0');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_BATTERY, MOD_CONTROL | MOD_ALT, '5');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_START_MENU, MOD_CONTROL | MOD_ALT, '6');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_TASK_MENU, MOD_CONTROL | MOD_ALT, '7');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_WEATHER, MOD_CONTROL | MOD_ALT, '8');
            RegisterHotKey(g_topBarHwnd, HOTKEY_ID_RECYCLEBIN, MOD_CONTROL | MOD_ALT, '9');
        }
        RegisterAppBar(g_topBarHwnd);
        // A moment later, so the shell has finished its own start-up layout pass
        // and doesn't immediately overwrite our reservation.
        SetTimer(g_topBarHwnd, kAppBarInitTimerId, 800, nullptr);

        ForegroundEventProcInstall();

        g_clockTimer = DispatcherTimer();
        g_clockTimer.Interval(std::chrono::seconds(1));
        g_clockTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                UpdateClockText();
                UpdateBatteryButton();
                UpdateWallpaperIfChanged();
            } catch (...) {
            }
        });
        g_clockTimer.Start();

        g_blurRefreshTimer = DispatcherTimer();
        g_blurRefreshTimer.Interval(std::chrono::seconds(2));
        g_blurRefreshTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                g_liveBlurBrushes.erase(
                    std::remove_if(g_liveBlurBrushes.begin(),
                                   g_liveBlurBrushes.end(),
                                   [](auto const& ref) {
                                       return !ref.get();
                                   }),
                    g_liveBlurBrushes.end());
                for (auto& ref : g_liveBlurBrushes) {
                    if (auto base = ref.get()) {
                        if (auto brush = base.try_as<XamlBlurBrush>()) {
                            brush->Rebuild();
                        }
                    }
                }
            } catch (...) {
            }
        });
        g_blurRefreshTimer.Start();

        g_taskListTimer = DispatcherTimer();
        g_taskListTimer.Interval(std::chrono::milliseconds(30000)); // 30s fallback
        g_taskListTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                RefreshTaskList(false);
            } catch (...) {
            }
        });
        g_taskListTimer.Start();

        // Resource usage timer (updates every 1 second)
        g_resourceTimer = DispatcherTimer();
        g_resourceTimer.Interval(std::chrono::seconds(1));
        g_resourceTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                UpdateResourceButton();
            } catch (...) {
            }
        });
        g_resourceTimer.Start();

        // Timer to restore the top bar if it gets minimized/hidden/cloaked by Show Desktop
        // Timer to restore the top bar if it gets minimized/hidden/cloaked by Show Desktop
        g_restoreTimer = DispatcherTimer();
        g_restoreTimer.Interval(std::chrono::milliseconds(1000));

        g_restoreTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                if (!g_topBarHwnd || g_allowHide) {
                    return;
                }

                // Always ensure visible.
                if (IsIconic(g_topBarHwnd)) {
                    ShowWindow(g_topBarHwnd, SW_RESTORE);
                }
                BOOL cloaked = FALSE;
                if (SUCCEEDED(DwmGetWindowAttribute(g_topBarHwnd, DWMWA_CLOAKED,
                                                    &cloaked, sizeof(cloaked))) &&
                    cloaked) {
                    DwmSetWindowAttribute(g_topBarHwnd, DWMWA_CLOAK, FALSE, sizeof(BOOL));
                }
                if (!IsWindowVisible(g_topBarHwnd)) {
                    ShowWindow(g_topBarHwnd, SW_SHOWNOACTIVATE);
                }

                // Classify the current foreground window.
                HWND fg = GetForegroundWindow();
                bool desktopFg = false;
                bool fullscreenFg = false;
                if (fg && fg != g_topBarHwnd) {
                    wchar_t cls[256] = {0};
                    GetClassName(fg, cls, ARRAYSIZE(cls));
                    desktopFg = (wcscmp(cls, L"Progman") == 0 ||
                                 wcscmp(cls, L"WorkerW") == 0);
                    if (!desktopFg) {
                        RECT r{};
                        HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
                        MONITORINFO mi{};
                        mi.cbSize = sizeof(mi);
                        if (GetWindowRect(fg, &r) && GetMonitorInfo(mon, &mi)) {
                            if (r.left   <= mi.rcMonitor.left &&
                                r.top    <= mi.rcMonitor.top &&
                                r.right  >= mi.rcMonitor.right &&
                                r.bottom >= mi.rcMonitor.bottom) {
                                fullscreenFg = true;
                            }
                        }
                    }
                }

                // Only touch z-order when the classification actually changed:
                // calling SetWindowPos once a second while a full-screen game
                // is foreground can kick it out of exclusive full-screen.
                static HWND s_lastRestoreFg = nullptr;
                static int s_lastRestoreClass = -1; // 0 normal, 1 desktop, 2 fullscreen
                int currentClass = fullscreenFg ? 2 : (desktopFg ? 1 : 0);
                bool classChanged =
                    (fg != s_lastRestoreFg || currentClass != s_lastRestoreClass);
                s_lastRestoreFg = fg;
                s_lastRestoreClass = currentClass;

                if (classChanged) {
                    if (fullscreenFg) {
                        // Place the bar DIRECTLY BEHIND the fullscreen window.
                        // Works whether the fullscreen app is topmost or not.
                        g_fullScreenAppActive = true;
                        SetWindowPos(g_topBarHwnd, fg, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                                     SWP_NOOWNERZORDER);
                    } else if (desktopFg) {
                        // Win+D: re-stack above the desktop (which is itself topmost).
                        g_fullScreenAppActive = false;
                        SetWindowPos(g_topBarHwnd, HWND_BOTTOM, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        SetWindowPos(g_topBarHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                    } else {
                        // Normal window foreground: keep ourselves above it.
                        g_fullScreenAppActive = false;
                        SetWindowPos(g_topBarHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    }
                }

                // Reposition if drifted.
                RECT wanted = GetBarMonitorRect();
                wanted.bottom = wanted.top + g_barHeightPx;
                RECT current{};
                GetWindowRect(g_topBarHwnd, &current);
                if (current.left != wanted.left || current.top != wanted.top ||
                    current.right != wanted.right || current.bottom != wanted.bottom) {
                    PositionAppBar(g_topBarHwnd, g_barHeightPx);
                }
            } catch (...) {
            }
        });

        g_restoreTimer.Start();

        // Wallpaper updates are handled by WM_SETTINGCHANGE – no timer needed.

        RefreshTaskList(true);

        // PreTranslateMessage is what gives the island keyboard input -- without it
        // the Wi-Fi password box would never see a keystroke.
        winrt::com_ptr<IDesktopWindowXamlSourceNative2> native2;
        try {
            native2 = g_desktopSource.as<IDesktopWindowXamlSourceNative2>();
        } catch (...) {
        }

        MSG message;
        while (GetMessage(&message, nullptr, 0, 0)) {
            BOOL processed = FALSE;
            if (native2) {
                native2->PreTranslateMessage(&message, &processed);
            }
            if (!processed && topbar_settings_window::PreTranslateSettingsMessage(&message)) {
                processed = TRUE;
            }
            if (!processed) {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }

        // Message loop ended — tear the settings window down on this same thread
        // (its WndProc runs here too, so this is safe and synchronous).
        topbar_settings_window::DestroySettingsWindowNow();

        // The topbar has been closed. Stop the foreground hook first (same thread).
        if (g_foregroundHook) {
            UnhookWinEvent(g_foregroundHook);
            g_foregroundHook = nullptr;
        }
        if (g_windowEventHook) {
            UnhookWinEvent(g_windowEventHook);
            g_windowEventHook = nullptr;
        }
        if (g_windowEventNameHook) {
            UnhookWinEvent(g_windowEventNameHook);
            g_windowEventNameHook = nullptr;
        }

        // The topbar has been closed. Stop all timers before the DLL unloads.
        if (g_clockTimer) g_clockTimer.Stop();
        if (g_blurRefreshTimer) g_blurRefreshTimer.Stop();
        if (g_taskRefreshTimer) g_taskRefreshTimer.Stop();
        if (g_taskClickTimer) g_taskClickTimer.Stop();
        if (g_taskListTimer) g_taskListTimer.Stop();
        if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer.Stop();
        if (g_bluetoothAutoRefreshTimer) g_bluetoothAutoRefreshTimer.Stop();
        if (g_volumeRevertTimer) g_volumeRevertTimer.Stop();
        if (g_brightnessRevertTimer) g_brightnessRevertTimer.Stop();
        if (g_resourceTimer) g_resourceTimer.Stop();
        if (g_resourceFlyoutTimer) g_resourceFlyoutTimer.Stop();
        if (g_restoreTimer) g_restoreTimer.Stop();
        if (g_restoreTimer) g_restoreTimer = nullptr;
        g_allowHide = true;  // allow hiding during final teardown
        // Release XAML and COM objects on this thread (before it exits)
        if (g_clockTimer) g_clockTimer = nullptr;
        if (g_blurRefreshTimer) g_blurRefreshTimer = nullptr;
        g_liveBlurBrushes.clear();
        if (g_taskRefreshTimer) g_taskRefreshTimer = nullptr;
        if (g_taskClickTimer) g_taskClickTimer = nullptr;
        g_taskClickPendingHwnd = nullptr;
        g_lastDoubleTapTick = 0;

        if (g_wifiAutoRefreshTimer) g_wifiAutoRefreshTimer = nullptr;
        if (g_bluetoothAutoRefreshTimer) g_bluetoothAutoRefreshTimer = nullptr;
        if (g_volumeRevertTimer) g_volumeRevertTimer = nullptr;
        if (g_brightnessRevertTimer) g_brightnessRevertTimer = nullptr;
        if (g_resourceTimer) g_resourceTimer = nullptr;
        if (g_resourceFlyoutTimer) g_resourceFlyoutTimer = nullptr;
        if (g_weatherSearchDebounceTimer) {
            g_weatherSearchDebounceTimer.Stop();
            g_weatherSearchDebounceTimer = nullptr;
        }
        if (g_weatherSearchDebounceTimer) {
            g_weatherSearchDebounceTimer.Stop();
            g_weatherSearchDebounceTimer = nullptr;
        }

        // Release wallpaper layer and other no_destroy globals
        if (g_wallpaperLayer) g_wallpaperLayer = nullptr;
        if (g_displayFlyout) g_displayFlyout = nullptr;
                if (g_resourceFlyout) g_resourceFlyout = nullptr;
        if (g_displayButton) g_displayButton = nullptr;
        
        if (g_resourceButton) g_resourceButton = nullptr;
        if (g_displayPanel) g_displayPanel = nullptr;
        
        if (g_resourcePanel) g_resourcePanel = nullptr;

        if (g_rootElement) {
            try {
                g_rootElement = nullptr;
            } catch (...) {}
        }
        if (g_desktopSource) {
            try {
                g_desktopSource.Content(nullptr);
            } catch (...) {}
            g_desktopSource = nullptr;
        }
        g_xamlManager = nullptr;
        g_uiDispatcherQueue = nullptr;
        g_taskListPanel = nullptr;

        // Clear maps of XAML refs
        g_namedElements.clear();
        g_taskButtonsByHwnd.clear();
        g_taskButtonLastTitle.clear();
        g_detachedStyleRoots.clear();
        g_taskContextMenu = nullptr;
        g_startContextMenu = nullptr;
        g_taskMenuToggleItem = nullptr;
        g_mediaContainer = nullptr;
        g_tabButtons.clear();

        // Release COM pointers
        audio::g_cachedEndpointVolume = nullptr;
        brightness::g_cachedWmiServices = nullptr;
        

        // Unregister the window class (now safe because thread exits)
        if (g_modModule) {
            UnregisterClass(kWindowClassName, g_modModule);
            g_modModule = nullptr;
        }

        return 0;
    } catch (...) {
        Wh_Log(L"TopBarThreadProc crashed with a C++ exception.");
        return 1;
    }
}

// ============================================================================
// Settings
// ============================================================================

void LoadSettings() {
    // Strings: GetStringSettingCopy is already storage-first.
    // Ints: ReadIntSetting is now storage-first with an explicit fallback
    // matching what the old schema used to declare.
    g_settings.barHeightDip = ReadIntSetting(L"barHeight", 40);
    if (g_settings.barHeightDip < 20) {
        g_settings.barHeightDip = 40;
    }
    
    // Schema-kept keys: Wh_GetIntSetting only.
    g_settings.monitorIndex = Wh_GetIntSetting(L"monitorIndex");

    g_settings.cornerRadius = ReadIntSetting(L"cornerRadius", 6);
    g_settings.topBarBackgroundColor = GetStringSettingCopy(L"topBarBackgroundColor");
    if (g_settings.topBarBackgroundColor.empty()) {
        g_settings.topBarBackgroundColor = L"#000000";
    }
    g_settings.topBarBackgroundOpacity = ReadIntSetting(L"topBarBackgroundOpacity", 25);
    g_settings.showStartButton = ReadIntSetting(L"showStartButton", 1) != 0;
    g_settings.showSearchButton = ReadIntSetting(L"showSearchButton", 1) != 0;
    g_settings.centerContent = GetStringSettingCopy(L"centerContent");
    if (g_settings.centerContent.empty()) {
        g_settings.centerContent = L"applicationButtons";
    }
    g_settings.taskButtonWidth = ReadIntSetting(L"taskButtonWidth", 100);
    if (g_settings.taskButtonWidth < 40) {
        g_settings.taskButtonWidth = 100;
    }
    g_settings.taskIconSize = ReadIntSetting(L"taskIconSize", 20);
    if (g_settings.taskIconSize < 8) {
        g_settings.taskIconSize = 20;
    }
    g_settings.taskButtonContent = GetStringSettingCopy(L"taskButtonContent");
    if (g_settings.taskButtonContent.empty()) {
        g_settings.taskButtonContent = L"textOnly";
    }
    g_settings.showDisplayButton = ReadIntSetting(L"showDisplayButton", 1) != 0;
    g_settings.showSoundButton = ReadIntSetting(L"showSoundButton", 1) != 0;
    g_settings.showWifiButton = ReadIntSetting(L"showWifiButton", 1) != 0;
    g_settings.showBluetoothButton = ReadIntSetting(L"showBluetoothButton", 1) != 0;
    g_settings.showBatteryButton = ReadIntSetting(L"showBatteryButton", 1) != 0;
    g_settings.showSettingsButton = ReadIntSetting(L"showSettingsButton", 1) != 0;
    g_settings.showWeatherButton = ReadIntSetting(L"showWeatherButton", 1) != 0;

    g_settings.leftItems = GetStringSettingCopy(L"leftItems");
    g_settings.centerItems = GetStringSettingCopy(L"centerItems");
    g_settings.rightItems = GetStringSettingCopy(L"rightItems");
    if (g_settings.leftItems.empty()) g_settings.leftItems = L"StartButton,SearchButton";
    if (g_settings.centerItems.empty()) g_settings.centerItems = L"ResourceButton,WeatherButton,SettingsButton";
    if (g_settings.rightItems.empty()) g_settings.rightItems = L"DisplayButton,SoundButton,WifiButton,BluetoothButton,BatteryButton,RecycleBinButton,ClockButton";

    g_settings.showCpuUsage = ReadIntSetting(L"showCpuUsage", 1) != 0;
    g_settings.showRamUsage = ReadIntSetting(L"showRamUsage", 1) != 0;
    g_settings.showGpuUsage = ReadIntSetting(L"showGpuUsage", 1) != 0;
    g_settings.enableHotkeys = Wh_GetIntSetting(L"enableHotkeys") != 0;
    g_settings.showClock = ReadIntSetting(L"showClock", 1) != 0;
    g_settings.timeFormat = GetStringSettingCopy(L"timeFormat");
    if (g_settings.timeFormat.empty()) {
        g_settings.timeFormat = L"🕑hh:mm tt";
    }
    g_settings.showDate = ReadIntSetting(L"showDate", 1) != 0;
    g_settings.dateFormat = GetStringSettingCopy(L"dateFormat");
    if (g_settings.dateFormat.empty()) {
        g_settings.dateFormat = L"📅ddd, MMM dd";
    }
    g_settings.iconColor = GetStringSettingCopy(L"iconColor");
    if (g_settings.iconColor.empty()) {
        g_settings.iconColor = L"#FFFFFF";
    }
    g_iconColorOverride.reset();
    g_settings.fontFamily = GetStringSettingCopy(L"fontFamily");
    g_settings.fontBold = ReadIntSetting(L"fontBold", 0) != 0;
    g_settings.globalBlurAmount = ReadIntSetting(L"globalBlurAmount", 6);
    if (g_settings.globalBlurAmount < 1) g_settings.globalBlurAmount = 1;
    if (g_settings.globalBlurAmount > 50) g_settings.globalBlurAmount = 50;

    {
        std::vector<wchar_t> buf(256);
        size_t len = Wh_GetStringValue(L"weatherLocationName", buf.data(), buf.size());
        g_settings.weatherLocationName.assign(buf.data(), len);
    }
    {
        std::vector<wchar_t> buf(64);
        size_t len = Wh_GetStringValue(L"weatherLatitude", buf.data(), buf.size());
        g_settings.weatherLatitude.assign(buf.data(), len);
    }
    {
        std::vector<wchar_t> buf(64);
        size_t len = Wh_GetStringValue(L"weatherLongitude", buf.data(), buf.size());
        g_settings.weatherLongitude.assign(buf.data(), len);
    }

    g_settings.trayOrder = ReadTrayOrder();
    g_trayOrder.clear();
    {
        size_t pos = 0;
        while (pos <= g_settings.trayOrder.size()) {
            size_t comma = g_settings.trayOrder.find(L',', pos);
            std::wstring token = TrimWs(g_settings.trayOrder.substr(
                pos, comma == std::wstring::npos ? std::wstring::npos : comma - pos));
            if (!token.empty()) {
                g_trayOrder.push_back(token);
            }
            if (comma == std::wstring::npos) {
                break;
            }
            pos = comma + 1;
        }
    }
    for (const auto& known : kDefaultTrayOrder) {
        if (std::find(g_trayOrder.begin(), g_trayOrder.end(), known) == g_trayOrder.end()) {
            g_trayOrder.push_back(known);
        }
    }

    g_styleConstants.clear();
    for (int i = 0;; i++) {
        std::wstring entry = GetStringSettingCopy(L"styleConstants[%d]", i);
        if (entry.empty()) {
            break;
        }
        size_t equals = entry.find(L'=');
        if (equals == std::wstring::npos) {
            continue;
        }
        std::wstring name = TrimWs(entry.substr(0, equals));
        std::wstring value = TrimWs(entry.substr(equals + 1));
        if (!name.empty()) {
            g_styleConstants.emplace_back(std::move(name), std::move(value));
        }
    }

    g_controlStyleRules.clear();
    for (int i = 0;; i++) {
        std::wstring target = GetStringSettingCopy(L"controlStyles[%d].target", i);
        if (target.empty()) {
            break;
        }
        ControlStyleRule rule;
        rule.target = TrimWs(target);
        for (int j = 0;; j++) {
            std::wstring style = GetStringSettingCopy(L"controlStyles[%d].styles[%d]", i, j);
            if (style.empty()) {
                break;
            }
            rule.styles.push_back(TrimWs(style));
        }
        if (!rule.styles.empty()) {
            g_controlStyleRules.push_back(std::move(rule));
        }
    }

    // Load selected theme.
    // Read straight from the mod editor — the settings window has no Theme
    // combo, but an earlier build wrote a stale value into Wh storage, which
    // the storage-first read would happily return instead of the mod value.
    g_themeStyleRules.clear();
    std::wstring theme = GetModEditorStringSetting(L"theme");
    if (theme == L"GreenBar") {
        g_themeStyleRules = g_themeGreenBarStyles;
    } else if (theme == L"NoIslands") {
        g_themeStyleRules = g_themeNoIslandsStyles;
    } else if (theme == L"OS27 GoldenGate") {
        g_themeStyleRules = g_themeOS27GoldenGateStyles;
    }
}

// ============================================================================
// Tool-mod plumbing
//
// The bar lives in its own process: Windhawk relaunches the host executable
// with "-tool-mod <id>", the real entry point is hooked out, and this mod owns
// the whole process. The host has to stay explorer.exe -- XAML Islands refuse
// to initialize in a process whose manifest lacks <maxversiontested>, which is
// what made the bar silently fail to appear when the host was changed.
// ============================================================================



BOOL WhTool_ModInit() {
    Wh_Log(L"TopBar: WhTool_ModInit called.");
    LoadSettings();

    g_taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

    // Create the stop event for clean shutdown
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);



    g_topBarThread = CreateThread(nullptr, 0, TopBarThreadProc, nullptr, 0, &g_topBarThreadId);
    if (!g_topBarThread) {
        Wh_Log(L"Failed to create the top bar thread: %u", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (!g_uiDispatcherQueue) {
        // Settings will be loaded on next start.
        return;
    }
    RunOnUiThread([] {
        LoadSettings();
        if (!g_topBarHwnd) {
            return;
        }
        g_dpiScale = GetBarDpiScale();
        g_barHeightPx = static_cast<int>(g_settings.barHeightDip * g_dpiScale + 0.5);

        // The whole tree is rebuilt: corner radius, icon colour and the task
        // button layout are all baked in at construction time.
        InstallGlobalMenuResources();
        auto content = BuildTopBarContent();
        g_desktopSource.Content(content);
        BuildStartContextMenu();
        BuildTaskContextMenu();
        BuildAppTitleContextMenu();
        ApplyAllControlStyles();
        ApplyVisibilitySettings();
        UpdateResourceButton();
        StripInheritedIslandBackgrounds();
        ApplyWindowBackdrop(g_topBarHwnd);
        PositionAppBar(g_topBarHwnd, g_barHeightPx);

        // Re-register hotkeys if the setting changed
        if (g_topBarHwnd) {
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_DISPLAY);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_SOUND);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_WIFI);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_BLUETOOTH);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_RESOURCE);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_BATTERY);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_START_MENU);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_TASK_MENU);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_WEATHER);
            UnregisterHotKey(g_topBarHwnd, HOTKEY_ID_RECYCLEBIN);
            if (g_settings.enableHotkeys) {
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_DISPLAY, MOD_CONTROL | MOD_ALT, '1');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_SOUND, MOD_CONTROL | MOD_ALT, '2');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_WIFI, MOD_CONTROL | MOD_ALT, '3');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_BLUETOOTH, MOD_CONTROL | MOD_ALT, '4');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_RESOURCE, MOD_CONTROL | MOD_ALT, '0');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_BATTERY, MOD_CONTROL | MOD_ALT, '5');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_START_MENU, MOD_CONTROL | MOD_ALT, '6');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_TASK_MENU, MOD_CONTROL | MOD_ALT, '7');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_WEATHER, MOD_CONTROL | MOD_ALT, '8');
                RegisterHotKey(g_topBarHwnd, HOTKEY_ID_RECYCLEBIN, MOD_CONTROL | MOD_ALT, '9');
            }
        }
        RefreshTaskList(true);
    });
}

void WhTool_ModUninit() {
    InterlockedExchange(&g_shuttingDown, 1);
    g_allowHide = true;   // allow the bar to hide during shutdown

    // Close the settings window (if open) before the topbar's message loop
    // exits, so its DestroyWindow runs on the correct thread.
    CloseTopBarSettingsWindow();

    // Signal the stop event so background workers can exit early.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Stop the UI thread first.
    if (g_topBarHwnd) {
        PostMessage(g_topBarHwnd, WM_CLOSE, 0, 0);
    }
    if (g_topBarThreadId) {
        PostThreadMessage(g_topBarThreadId, WM_QUIT, 0, 0);
    }

    // Wait for all worker threads to finish (up to 10 seconds).
    std::vector<HANDLE> handles;
    {
        std::lock_guard<std::mutex> lock(g_workerThreadsMutex);
        handles = g_workerThreads;
    }
    if (!handles.empty()) {
        const size_t kMaxWaitObjects = 64;
        for (size_t offset = 0; offset < handles.size(); offset += kMaxWaitObjects) {
            size_t batchCount = std::min<size_t>(handles.size() - offset, kMaxWaitObjects);
            WaitForMultipleObjects(static_cast<DWORD>(batchCount), handles.data() + offset, TRUE, 10000);
        }
    }
    // Close all worker handles
    for (HANDLE h : handles) {
        CloseHandle(h);
    }
    g_workerThreads.clear();

    // Wait for the main UI thread to exit.
    if (g_topBarThread) {
        WaitForSingleObject(g_topBarThread, INFINITE);
        CloseHandle(g_topBarThread);
        g_topBarThread = nullptr;
    }

    // Now it's safe to close the stop event (all workers have exited).
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
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