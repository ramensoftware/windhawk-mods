// ==WindhawkMod==
// @id              taskbar-system-info
// @name            Taskbar System Info
// @name:uk-UA      Системний монітор панелі завдань
// @description     A quiet two-column CPU, GPU, RAM and VRAM monitor with 60-second history graphs for the Windows 11 taskbar.
// @description:uk-UA Компактний монітор CPU, GPU, RAM і VRAM із 60-секундними графіками для панелі завдань Windows 11.
// @version         1.4.0
// @author          Yevhenii Starychenko
// @github          https://github.com/starychenko
// @homepage        https://github.com/starychenko/windhawk-taskbar-system-info
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lpdh -ldxgi -DWIN32_LEAN_AND_MEAN
// ==/WindhawkMod==

// Taskbar XAML discovery and window-thread marshaling are based on techniques
// from "Multirow taskbar for Windows 11" by Michael Maltsev (m417z):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp
// Native GPU temperature collection via D3DKMT follows Taskbar Clock
// Customization by Michael Maltsev (m417z):
// https://github.com/m417z/my-windhawk-mods/commit/861920df6380f4c13abec5d9226362c4725e8362
// Secondary-taskbar discovery is adapted from Taskbar Fluent Media Player by
// Salyts:
// https://github.com/Salyts/Taskbar-Fluent-Media-Player
// The first two projects are GPL-3.0; Taskbar Fluent Media Player is MIT.

/*
Taskbar Fluent Media Player MIT notice:

Copyright (c) 2026 Salyts

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ==WindhawkModReadme==
/*
# Taskbar System Info

A compact, click-through system monitor for the far-left free area of the
Windows 11 taskbar. It is designed for a quick administrator glance: current
values show what is happening now, while two restrained history traces reveal
whether a CPU or GPU spike is momentary or sustained.

![Taskbar System Info preview](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/taskbar-system-info.png)

The fixed two-column layout keeps every metric in a predictable place:

```text
CPU  10%  72°C  [60-second graph]    RAM   52%  16.7/32G
GPU   4%  56°C  [60-second graph]    VRAM   9%   2.1/24G
```

CPU and GPU history uses a fixed 0-100% scale. RAM and VRAM use thin capacity
bars. Fixed-width fields prevent the layout from shifting as values change.
Normal values remain monochrome; only warning and critical readings receive
color. Adaptive colors are enabled by default: normal text follows the native
taskbar foreground, while graphs and alerts switch between contrast-checked
light and dark palettes as the taskbar theme changes. Windows high-contrast mode
uses its system highlight colors. Disable the adaptive option to use the manual
color settings exactly. Network and disk activity are intentionally not
collected.

Unlike the performance placeholders in
[Taskbar Clock Customization](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-clock-customization.wh.cpp),
this mod does not alter the clock. It uses the free far-left taskbar area for a
stable 2x2 dashboard with rolling graphs, capacity bars and temperature alerts.

## Metrics

- CPU utilization from the Windows Processor Utility counter, matching the
  frequency-aware value used by Task Manager when available. Windows system
  time counters remain the compatibility fallback.
- RAM usage and capacity from Windows memory status.
- GPU utilization and dedicated or shared GPU-memory usage from Windows PDH
  counters.
- GPU-memory capacity and adapter identity from live D3DKMT enumeration, with
  DXGI as a compatibility fallback. Automatic mode uses shared GPU memory for
  integrated adapters, including common small dedicated carve-outs, and
  dedicated VRAM for discrete adapters. The memory type can also be forced.
- CPU and GPU temperatures from HWiNFO when available.
- GPU fallback from the Windows display-driver interface (D3DKMT).
- CPU fallback from Windows ACPI thermal zones exposed through PDH.

Metric collection runs on a worker thread. The taskbar UI thread only renders
completed snapshots and catches up with every sample that arrived while the UI
was busy. If a display-driver restart changes an adapter LUID or invalidates the
active performance counters, the mod refreshes the live adapter list and
rebuilds the counters automatically. This also covers successful but empty VRAM
results and uses exponential backoff while a parked GPU stays idle.

The adapter with the most dedicated VRAM is selected automatically. A partial
adapter-name filter is available for multi-GPU systems. GPU usage and VRAM are
matched to the selected live adapter by LUID. Duplicate stale adapters without
a driver name are ignored when a named adapter with the same capacity exists.
For an integrated GPU, the displayed capacity is the Windows shared-memory
limit rather than a physically reserved memory pool, so its percentage has
different semantics from a discrete GPU's dedicated VRAM. Small and fractional
capacity totals retain one decimal place.

## Temperature providers

The **Temperature source** setting provides these modes:

- **Automatic** fills CPU and GPU independently: HWiNFO shared memory first,
  then HWiNFO Gadget Registry, then Windows D3DKMT for a still-missing GPU
  reading and Windows thermal zones for a still-missing CPU reading.
- **HWiNFO automatic** uses only the two HWiNFO interfaces.
- **HWiNFO Shared Memory** uses only `Global\\HWiNFO_SENS_SM2`.
- **HWiNFO Gadget Registry** uses only
  `HKCU\\Software\\HWiNFO64\\VSB`.
- **Windows native** reads GPU temperature from the selected display driver via
  D3DKMT and CPU temperature from the same
  `\\Thermal Zone Information(*)\\Temperature` PDH source as Taskbar Clock
  Customization. It needs no third-party monitor. ACPI platform zones don't
  necessarily represent the CPU package sensor; the optional zone filter and
  average/hottest setting make this fallback explicit and controllable.
- **Disabled** skips temperature collection while keeping every other metric.

HWiNFO is optional and is not bundled with this mod. Shared-memory integration
targets HWiNFO 7.0 or newer, which permits full disclosure of the interface.
Temperature units are classified from HWiNFO's raw unit bytes, independently of
the Windows ANSI code page.
The free HWiNFO64 edition disables shared memory after 12 hours of continuous
use; HWiNFO64 Pro has no such limit. Gadget Registry is a separate HWiNFO
interface. Configure it under **Sensor Settings > HWiNFO Gadget** by enabling
**Report to Gadget** for the desired CPU and GPU temperature readings. If the
selected source is unavailable, temperatures are shown as `--°C`; all other
metrics continue to work. A single transient provider timeout keeps the last
good temperature for at most two samples, avoiding a one-tick `--°C` flicker
without hiding a provider that has actually disappeared. The active provider is
written to the Windhawk log only when it changes. Automatic HWiNFO GPU sensor
selection is also matched to the selected Windows adapter; a one-time diagnostic
explains when readings exist but no adapter name matches.

## Compatibility and placement

- Windows 11 64-bit. The widget can be placed on the primary or a secondary
  taskbar. x64 is hardware-tested; ARM64 is compilation-tested.
- Monitor 1 is always the primary display. Other monitors are ordered by their
  position in the virtual desktop and can differ from the numbers in Windows
  Display Settings. An unavailable or disconnected selection falls back to the
  primary taskbar automatically and moves back when the selected display returns.
- Centered taskbar icons are recommended.
- The widget uses the far-left taskbar area. Windows Widgets/weather or another
  left-side taskbar extension can occupy the same space; adjust the offset or
  disable the conflicting element if they overlap.
- Enable **Reserve space before the Start button** if the widget overlaps
  left-aligned taskbar buttons.
- The widget is native XAML inside the taskbar and can coexist with Taskbar
  Styler.

## Credits and license

Taskbar discovery and window-thread marshaling follow techniques from
[Multirow taskbar for Windows 11](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp)
by Michael Maltsev (`m417z`). Native GPU temperature collection follows his
[Taskbar Clock Customization implementation](https://github.com/m417z/my-windhawk-mods/commit/861920df6380f4c13abec5d9226362c4725e8362).
Secondary-taskbar discovery is adapted from
[Taskbar Fluent Media Player](https://github.com/Salyts/Taskbar-Fluent-Media-Player)
by Salyts.
Released under GPL-3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 410
  $name: Widget width
  $name:uk-UA: Ширина блока
  $description: "Allowed range: 330-800 pixels. A width of 380-450 works well with most display scales."
  $description:uk-UA: "Діапазон: 330-800 пікселів. Зазвичай добре підходить ширина 380-450."

- leftOffset: 10
  $name: Left offset
  $name:uk-UA: Відступ зліва
  $description: "Distance from the left taskbar edge, from 0 to 1000 pixels."
  $description:uk-UA: "Відстань від лівого краю панелі, від 0 до 1000 пікселів."

- monitor: 1
  $name: Taskbar monitor
  $name:uk-UA: Монітор панелі завдань
  $description: "Allowed range: 1-32. Monitor 1 is the primary display. Other monitors are ordered by their position in the virtual desktop and can differ from Windows Display Settings. An unavailable selection falls back to the primary taskbar."
  $description:uk-UA: "Діапазон: 1-32. Монітор 1 - основний. Інші впорядковані за розташуванням у віртуальному робочому столі, тому номери можуть відрізнятися від параметрів дисплея Windows. Якщо вибраний монітор недоступний, використовується основна панель."

- reserveSpace: false
  $name: Reserve space before the Start button
  $name:uk-UA: Резервувати місце перед кнопкою Пуск
  $description: "Usually not needed when Windows 11 taskbar icons are centered."
  $description:uk-UA: "Для центрованих значків Windows 11 зазвичай не потрібно."

- reserveGap: 8
  $name: Reserved space gap
  $name:uk-UA: Проміжок після блока
  $description: "Additional gap after the reserved widget area, from 0 to 100 pixels."
  $description:uk-UA: "Додатковий проміжок після зарезервованої області, від 0 до 100 пікселів."

- updateInterval: 1
  $name: Update interval
  $name:uk-UA: Інтервал оновлення
  $description: "Metric refresh interval, from 1 to 10 seconds."
  $description:uk-UA: "Від 1 до 10 секунд."

- historySeconds: 60
  $name: Graph history
  $name:uk-UA: Історія графіків
  $description: "CPU and GPU history window, from 15 to 180 seconds."
  $description:uk-UA: "Від 15 до 180 секунд."

- fontSize: 11
  $name: Font size
  $name:uk-UA: Розмір тексту
  $description: "From 9 to 13 pixels."
  $description:uk-UA: "Від 9 до 13 пікселів."

- fontFamily: "Segoe UI Variable Text"
  $name: Font family
  $name:uk-UA: Шрифт

- textColor: ""
  $name: Text color
  $name:uk-UA: Колір тексту
  $description: "#RRGGBB or #AARRGGBB. Leave empty to use the system color. Used when adaptive colors are disabled."
  $description:uk-UA: "#RRGGBB або #AARRGGBB. Порожнє значення використовує системний колір. Застосовується, коли адаптивні кольори вимкнені."

- adaptiveColors: true
  $name: Adapt colors to the taskbar theme
  $name:uk-UA: Адаптувати кольори до теми панелі
  $description: "Automatically uses contrasting light, dark or Windows high-contrast colors for text, graphs and alerts. Disable to use the manual colors below exactly."
  $description:uk-UA: "Автоматично добирає контрастні кольори тексту, графіків і попереджень для світлої, темної або висококонтрастної теми Windows. Вимкніть, щоб точно використовувати ручні кольори нижче."

- graphColor: "#78A8FF"
  $name: Graph and bar color
  $name:uk-UA: Колір графіків і смуг
  $description: "#RRGGBB or #AARRGGBB accent for CPU/GPU history and memory capacity bars. Used when adaptive colors are disabled. Invalid values use the default color."
  $description:uk-UA: "Акцент #RRGGBB або #AARRGGBB для історії CPU/GPU та смуг памяті. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

- warningColor: "#FFFFB900"
  $name: Warning color
  $name:uk-UA: Колір попередження
  $description: "#RRGGBB or #AARRGGBB. Used when adaptive colors are disabled. Invalid values use the default color."
  $description:uk-UA: "#RRGGBB або #AARRGGBB. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

- criticalColor: "#FFFF6B6B"
  $name: Critical color
  $name:uk-UA: Критичний колір
  $description: "#RRGGBB or #AARRGGBB. Used when adaptive colors are disabled. Invalid values use the default color."
  $description:uk-UA: "#RRGGBB або #AARRGGBB. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

- textOpacity: 96
  $name: Text opacity
  $name:uk-UA: Прозорість тексту
  $description: "From 0 to 100 percent."
  $description:uk-UA: "Від 0 до 100."

- cpuWarningTemp: 75
  $name: CPU temperature warning
  $name:uk-UA: Попередження температури CPU
  $description: "From 40 to 95 degrees Celsius."
  $description:uk-UA: "Від 40 до 95 градусів Цельсія."

- cpuCriticalTemp: 85
  $name: CPU critical temperature
  $name:uk-UA: Критична температура CPU
  $description: "From one degree above the warning threshold to 105 degrees Celsius."
  $description:uk-UA: "Від одного градуса вище порога попередження до 105 градусів Цельсія."

- gpuWarningTemp: 80
  $name: GPU temperature warning
  $name:uk-UA: Попередження температури GPU
  $description: "From 40 to 105 degrees Celsius."
  $description:uk-UA: "Від 40 до 105 градусів Цельсія."

- gpuCriticalTemp: 90
  $name: GPU critical temperature
  $name:uk-UA: Критична температура GPU
  $description: "From one degree above the warning threshold to 115 degrees Celsius."
  $description:uk-UA: "Від одного градуса вище порога попередження до 115 градусів Цельсія."

- memoryWarningPercent: 80
  $name: Memory usage warning
  $name:uk-UA: Попередження заповнення памяті
  $description: "From 50 to 98 percent."
  $description:uk-UA: "Від 50 до 98 відсотків."

- memoryCriticalPercent: 90
  $name: Critical memory usage
  $name:uk-UA: Критичне заповнення памяті
  $description: "From one percent above the warning threshold to 100 percent."
  $description:uk-UA: "Від одного відсотка вище порога попередження до 100 відсотків."

- gpuAdapter: ""
  $name: GPU adapter filter
  $name:uk-UA: Відеокарта
  $description: "Optional partial adapter name. Empty selects the adapter with the most dedicated VRAM."
  $description:uk-UA: "Необовязкова частина назви. Порожнє значення вибирає адаптер з найбільшим обсягом VRAM."

- gpuMemoryMode: auto
  $name: GPU memory type
  $name:uk-UA: Тип пам'яті GPU
  $description: "Automatic uses shared GPU memory for an integrated adapter and dedicated VRAM for a discrete adapter. The explicit modes override automatic detection."
  $description:uk-UA: "Автоматичний режим використовує спільну GPU-пам'ять для інтегрованого адаптера і виділену VRAM для дискретного. Явний режим перевизначає автоматичне визначення."
  $options:
  - auto: Automatic
  - dedicated: Dedicated VRAM
  - shared: Shared GPU memory
  $options:uk-UA:
  - auto: Автоматично
  - dedicated: Виділена VRAM
  - shared: Спільна GPU-пам'ять

- temperatureSource: auto
  $name: Temperature source
  $name:uk-UA: Джерело температури
  $description: "Automatic tries both HWiNFO interfaces, then Windows D3DKMT for a missing GPU reading and Windows thermal zones for a missing CPU reading."
  $description:uk-UA: "Автоматичний режим перевіряє обидва інтерфейси HWiNFO, а потім Windows D3DKMT для відсутньої температури GPU та системні термозони Windows для відсутньої температури CPU."
  $options:
  - auto: Automatic
  - hwinfoAuto: HWiNFO automatic
  - sharedMemory: HWiNFO Shared Memory
  - gadgetRegistry: HWiNFO Gadget Registry
  - windowsNative: Windows native (ACPI CPU + D3DKMT GPU)
  - disabled: Disabled
  $options:uk-UA:
  - auto: Автоматично
  - hwinfoAuto: HWiNFO автоматично
  - sharedMemory: HWiNFO Shared Memory
  - gadgetRegistry: HWiNFO Gadget Registry
  - windowsNative: Системні датчики Windows (ACPI CPU + D3DKMT GPU)
  - disabled: Вимкнено

- windowsThermalZoneFilter: ""
  $name: Windows thermal zone filter
  $name:uk-UA: Фільтр системної термозони Windows
  $description: "Optional partial PDH instance name. Empty uses every valid ACPI thermal zone. Applies to the CPU part of Windows native temperature collection."
  $description:uk-UA: "Необов'язкова частина назви екземпляра PDH. Порожнє значення використовує всі коректні термозони ACPI. Застосовується до CPU у системному режимі Windows."

- windowsThermalZoneAggregation: average
  $name: Windows thermal zone aggregation
  $name:uk-UA: Об'єднання системних термозон Windows
  $description: "Average matches Taskbar Clock Customization. Hottest is safer for alert-oriented monitoring."
  $description:uk-UA: "Середня відповідає Taskbar Clock Customization. Найгарячіша краще підходить для моніторингу попереджень."
  $options:
  - average: Average
  - hottest: Hottest
  $options:uk-UA:
  - average: Середня
  - hottest: Найгарячіша

- cpuTempSensor: ""
  $name: CPU temperature sensor filter
  $name:uk-UA: Датчик температури CPU
  $description: "Optional partial HWiNFO sensor name. Empty automatically selects CPU (Tctl/Tdie), CPU Die, or CPU Package."
  $description:uk-UA: "Необов'язкова частина назви HWiNFO. Порожнє значення автоматично вибирає CPU (Tctl/Tdie), CPU Die або CPU Package."

- gpuTempSensor: ""
  $name: GPU temperature sensor filter
  $name:uk-UA: Датчик температури GPU
  $description: "Optional partial HWiNFO sensor name. Empty automatically selects GPU Temperature."
  $description:uk-UA: "Необов'язкова частина назви HWiNFO. Порожнє значення автоматично вибирає GPU Temperature."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dxgi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using XamlPolyline = winrt::Windows::UI::Xaml::Shapes::Polyline;
using XamlRectangle = winrt::Windows::UI::Xaml::Shapes::Rectangle;

namespace {

constexpr wchar_t kWidgetName[] = L"WindhawkTaskbarSystemInfo";
constexpr double kWidgetHeight = 38.0;
constexpr double kRowHeight = 18.0;
constexpr double kRowGap = 2.0;
constexpr double kColumnGap = 14.0;
constexpr double kMetricLabelWidth = 31.0;
constexpr double kMetricUsageWidth = 42.0;
constexpr double kMetricTempWidth = 48.0;
constexpr double kGraphLeftGap = 8.0;
constexpr double kMemoryLabelWidth = 43.0;
constexpr double kMemoryPercentWidth = 38.0;
constexpr double kGraphHeight = 12.0;
constexpr double kGiB = 1024.0 * 1024.0 * 1024.0;
constexpr uint32_t kMaximumTaskbarViewHookAttempts = 3;
constexpr wchar_t kDefaultGraphColor[] = L"#78A8FF";
constexpr wchar_t kDefaultWarningColor[] = L"#FFFFB900";
constexpr wchar_t kDefaultCriticalColor[] = L"#FFFF6B6B";
constexpr wchar_t kLightGraphColor[] = L"#FF005FB8";
constexpr wchar_t kLightWarningColor[] = L"#FF8A4B00";
constexpr wchar_t kLightCriticalColor[] = L"#FFC42B1C";
constexpr uint32_t kHwInfoSignature = 0x53695748;  // "HWiS"
constexpr uint32_t kHwInfoTemperatureType = 1;

enum class TemperatureSource {
    Auto,
    HwInfoAuto,
    SharedMemory,
    GadgetRegistry,
    WindowsNative,
    Disabled,
};

enum class ThermalZoneAggregation {
    Average,
    Hottest,
};

enum class GpuMemoryMode {
    Auto,
    Dedicated,
    Shared,
};

enum class TemperatureProvider {
    None,
    HwInfoSharedMemory,
    HwInfoGadgetRegistry,
    WindowsD3dkmt,
    WindowsThermalZones,
};

struct ModSettings {
    std::wstring fontFamily;
    std::wstring textColor;
    std::wstring graphColor;
    std::wstring warningColor;
    std::wstring criticalColor;
    std::wstring gpuAdapter;
    std::wstring cpuTempSensor;
    std::wstring gpuTempSensor;
    std::wstring windowsThermalZoneFilter;
    TemperatureSource temperatureSource = TemperatureSource::Auto;
    ThermalZoneAggregation windowsThermalZoneAggregation =
        ThermalZoneAggregation::Average;
    GpuMemoryMode gpuMemoryMode = GpuMemoryMode::Auto;
    int width = 410;
    int leftOffset = 10;
    int monitor = 1;
    bool adaptiveColors = true;
    bool reserveSpace = false;
    int reserveGap = 8;
    int updateInterval = 1;
    int historySeconds = 60;
    int fontSize = 11;
    int textOpacity = 96;
    int cpuWarningTemp = 75;
    int cpuCriticalTemp = 85;
    int gpuWarningTemp = 80;
    int gpuCriticalTemp = 90;
    int memoryWarningPercent = 80;
    int memoryCriticalPercent = 90;
};

std::shared_ptr<const ModSettings> g_settings{
    std::make_shared<ModSettings>()};
std::mutex g_settingsMutex;
std::atomic<bool> g_unloading;
std::atomic<bool> g_uiTornDown;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<uint32_t> g_taskbarViewHookAttempts;
std::atomic<HWND> g_taskbarWindow{nullptr};
std::atomic<DWORD> g_taskbarThreadId{0};

[[clang::no_destroy]] Grid g_widget{nullptr};
[[clang::no_destroy]] Grid g_rootGrid{nullptr};
[[clang::no_destroy]] FrameworkElement g_taskItemsRepeater{nullptr};
double g_reservedMargin = 0.0;
std::optional<double> g_lastAppliedRepeaterMarginLeft;
double g_graphWidth = 96.0;
double g_memoryBarWidth = 120.0;
[[clang::no_destroy]] DispatcherTimer g_timer{nullptr};
event_token g_timerToken{};
event_token g_actualThemeChangedToken{};
std::chrono::steady_clock::time_point g_nextSystemColorCheck{};
std::chrono::steady_clock::time_point g_nextTaskbarPlacementCheck{};
[[clang::no_destroy]]
std::optional<std::list<FrameworkElement::Loaded_revoker>> g_loadedRevokers{
    std::in_place};

[[clang::no_destroy]] TextBlock g_cpuLabel{nullptr};
[[clang::no_destroy]] TextBlock g_cpuUsageText{nullptr};
[[clang::no_destroy]] TextBlock g_cpuTempText{nullptr};
[[clang::no_destroy]] TextBlock g_gpuLabel{nullptr};
[[clang::no_destroy]] TextBlock g_gpuUsageText{nullptr};
[[clang::no_destroy]] TextBlock g_gpuTempText{nullptr};
[[clang::no_destroy]] TextBlock g_ramLabel{nullptr};
[[clang::no_destroy]] TextBlock g_ramPercentText{nullptr};
[[clang::no_destroy]] TextBlock g_ramCapacityText{nullptr};
[[clang::no_destroy]] TextBlock g_vramLabel{nullptr};
[[clang::no_destroy]] TextBlock g_vramPercentText{nullptr};
[[clang::no_destroy]] TextBlock g_vramCapacityText{nullptr};
[[clang::no_destroy]] XamlPolyline g_cpuGraph{nullptr};
[[clang::no_destroy]] XamlPolyline g_gpuGraph{nullptr};
[[clang::no_destroy]] XamlRectangle g_ramTrack{nullptr};
[[clang::no_destroy]] XamlRectangle g_ramFill{nullptr};
[[clang::no_destroy]] XamlRectangle g_vramTrack{nullptr};
[[clang::no_destroy]] XamlRectangle g_vramFill{nullptr};
[[clang::no_destroy]] ColumnDefinition g_leftColumn{nullptr};
[[clang::no_destroy]] ColumnDefinition g_gapColumn{nullptr};
[[clang::no_destroy]] ColumnDefinition g_rightColumn{nullptr};
[[clang::no_destroy]] SolidColorBrush g_textBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_graphBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_warningBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_criticalBrush{nullptr};
ElementTheme g_cachedWidgetTheme = ElementTheme::Default;
bool g_themeBrushesInitialized = false;
bool g_cachedHighContrast = false;
COLORREF g_cachedHighlightColor = CLR_INVALID;
COLORREF g_cachedHotlightColor = CLR_INVALID;
COLORREF g_cachedGrayTextColor = CLR_INVALID;

std::deque<double> g_cpuHistory;
std::deque<double> g_gpuHistory;
int g_historyInterval = 0;
int g_historyWindow = 0;

PDH_HQUERY g_pdhQuery = nullptr;
PDH_HCOUNTER g_cpuUtilityCounter = nullptr;
PDH_HCOUNTER g_gpuCounter = nullptr;
PDH_HCOUNTER g_vramCounter = nullptr;
PDH_HCOUNTER g_sharedVramCounter = nullptr;
PDH_HCOUNTER g_thermalZoneCounter = nullptr;
std::chrono::steady_clock::time_point g_nextPdhCounterRetry{};
std::chrono::steady_clock::time_point g_nextPdhRecovery{};
std::chrono::steady_clock::time_point g_nextGpuIdentityCheck{};
uint32_t g_consecutivePdhReadFailures = 0;
bool g_hwInfoInvalidUnitLogged = false;
std::atomic<bool> g_hwInfoGpuAdapterMismatchLogged{false};

struct MetricsSnapshot {
    double cpu = 0.0;
    bool cpuAvailable = false;
    double ram = 0.0;
    double ramUsedGb = 0.0;
    double ramTotalGb = 0.0;
    bool ramAvailable = false;
    double gpu = 0.0;
    bool gpuAvailable = false;
    double vram = 0.0;
    double vramUsedGb = 0.0;
    double vramTotalGb = 0.0;
    bool vramAvailable = false;
    std::optional<double> cpuTemp;
    std::optional<double> gpuTemp;
    TemperatureProvider cpuTempProvider = TemperatureProvider::None;
    TemperatureProvider gpuTempProvider = TemperatureProvider::None;
};

std::mutex g_metricsMutex;
struct PublishedMetricsSnapshot {
    uint64_t sequence = 0;
    MetricsSnapshot snapshot;
};
std::deque<PublishedMetricsSnapshot> g_publishedMetrics;
uint64_t g_latestMetricsSequence = 0;
uint64_t g_lastRenderedMetricsSequence = 0;
constexpr size_t kMaximumPublishedMetrics = 256;

std::mutex g_metricsWorkerMutex;
std::atomic<bool> g_stopMetricsWorker{false};
HANDLE g_metricsWorkerWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_metricsWorker;

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

std::optional<Color> ParseColor(const std::wstring& value);

TemperatureSource ParseTemperatureSource(const std::wstring& value) {
    if (value == L"hwinfoAuto") {
        return TemperatureSource::HwInfoAuto;
    }
    if (value == L"sharedMemory") {
        return TemperatureSource::SharedMemory;
    }
    if (value == L"gadgetRegistry") {
        return TemperatureSource::GadgetRegistry;
    }
    if (value == L"windowsNative") {
        return TemperatureSource::WindowsNative;
    }
    if (value == L"disabled") {
        return TemperatureSource::Disabled;
    }
    return TemperatureSource::Auto;
}

ThermalZoneAggregation ParseThermalZoneAggregation(
    const std::wstring& value) {
    return value == L"hottest" ? ThermalZoneAggregation::Hottest
                                : ThermalZoneAggregation::Average;
}

GpuMemoryMode ParseGpuMemoryMode(const std::wstring& value) {
    if (value == L"dedicated") {
        return GpuMemoryMode::Dedicated;
    }
    if (value == L"shared") {
        return GpuMemoryMode::Shared;
    }
    return GpuMemoryMode::Auto;
}

void LoadSettings() {
    ModSettings settings;
    settings.fontFamily = GetStringSetting(L"fontFamily");
    settings.textColor = GetStringSetting(L"textColor");
    settings.graphColor = GetStringSetting(L"graphColor");
    settings.warningColor = GetStringSetting(L"warningColor");
    settings.criticalColor = GetStringSetting(L"criticalColor");
    settings.gpuAdapter = GetStringSetting(L"gpuAdapter");
    settings.gpuMemoryMode =
        ParseGpuMemoryMode(GetStringSetting(L"gpuMemoryMode"));
    settings.temperatureSource =
        ParseTemperatureSource(GetStringSetting(L"temperatureSource"));
    settings.cpuTempSensor = GetStringSetting(L"cpuTempSensor");
    settings.gpuTempSensor = GetStringSetting(L"gpuTempSensor");
    settings.windowsThermalZoneFilter =
        GetStringSetting(L"windowsThermalZoneFilter");
    settings.windowsThermalZoneAggregation = ParseThermalZoneAggregation(
        GetStringSetting(L"windowsThermalZoneAggregation"));
    settings.width = std::clamp(Wh_GetIntSetting(L"width"), 330, 800);
    settings.leftOffset = std::clamp(Wh_GetIntSetting(L"leftOffset"), 0, 1000);
    settings.monitor = std::clamp(Wh_GetIntSetting(L"monitor"), 1, 32);
    settings.adaptiveColors = Wh_GetIntSetting(L"adaptiveColors") != 0;
    settings.reserveSpace = Wh_GetIntSetting(L"reserveSpace") != 0;
    settings.reserveGap = std::clamp(Wh_GetIntSetting(L"reserveGap"), 0, 100);
    settings.updateInterval =
        std::clamp(Wh_GetIntSetting(L"updateInterval"), 1, 10);
    settings.historySeconds =
        std::clamp(Wh_GetIntSetting(L"historySeconds"), 15, 180);
    settings.fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 9, 13);
    settings.textOpacity =
        std::clamp(Wh_GetIntSetting(L"textOpacity"), 0, 100);
    settings.cpuWarningTemp =
        std::clamp(Wh_GetIntSetting(L"cpuWarningTemp"), 40, 95);
    settings.cpuCriticalTemp = std::clamp(
        Wh_GetIntSetting(L"cpuCriticalTemp"), settings.cpuWarningTemp + 1, 105);
    settings.gpuWarningTemp =
        std::clamp(Wh_GetIntSetting(L"gpuWarningTemp"), 40, 105);
    settings.gpuCriticalTemp = std::clamp(
        Wh_GetIntSetting(L"gpuCriticalTemp"), settings.gpuWarningTemp + 1, 115);
    settings.memoryWarningPercent =
        std::clamp(Wh_GetIntSetting(L"memoryWarningPercent"), 50, 98);
    settings.memoryCriticalPercent =
        std::clamp(Wh_GetIntSetting(L"memoryCriticalPercent"),
                   settings.memoryWarningPercent + 1, 100);

    if (settings.fontFamily.empty()) {
        settings.fontFamily = L"Segoe UI Variable Text";
    }
    if (!settings.textColor.empty() && !ParseColor(settings.textColor)) {
        Wh_Log(L"Invalid text color; using the system color");
        settings.textColor.clear();
    }
    if (!ParseColor(settings.graphColor)) {
        Wh_Log(L"Invalid graph color; using the default");
        settings.graphColor = kDefaultGraphColor;
    }
    if (!ParseColor(settings.warningColor)) {
        Wh_Log(L"Invalid warning color; using the default");
        settings.warningColor = kDefaultWarningColor;
    }
    if (!ParseColor(settings.criticalColor)) {
        Wh_Log(L"Invalid critical color; using the default");
        settings.criticalColor = kDefaultCriticalColor;
    }

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::make_shared<ModSettings>(std::move(settings));
    g_hwInfoGpuAdapterMismatchLogged = false;
}

std::shared_ptr<const ModSettings> CurrentSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
        return static_cast<wchar_t>(std::towlower(ch));
    });
    return value;
}

bool Contains(const std::wstring& text, const std::wstring& needle) {
    return needle.empty() || text.find(needle) != std::wstring::npos;
}

std::optional<std::wstring> ResolveGpuTemperatureAdapterName(
    const ModSettings& settings);

std::wstring FixedAnsiToWide(const char* value, size_t capacity) {
    size_t length = 0;
    while (length < capacity && value[length]) {
        length++;
    }
    if (!length) {
        return {};
    }

    int wideLength = MultiByteToWideChar(CP_ACP, 0, value,
                                         static_cast<int>(length), nullptr, 0);
    if (wideLength <= 0) {
        return {};
    }

    std::wstring result(wideLength, L'\0');
    MultiByteToWideChar(CP_ACP, 0, value, static_cast<int>(length),
                        result.data(), wideLength);
    return result;
}

int CpuTemperatureScore(const std::wstring& sensorName,
                        const std::wstring& label,
                        const std::wstring& preferred) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    std::wstring combined = sensor + L" " + reading;
    std::wstring preferredLower = ToLower(preferred);

    if (!preferredLower.empty()) {
        return Contains(combined, preferredLower) ? 10000 : -1;
    }

    bool gpuSensor = Contains(sensor, L"gpu") || Contains(sensor, L"nvidia") ||
                     Contains(sensor, L"radeon");
    bool cpuSensor =
        Contains(sensor, L"cpu") || Contains(sensor, L"processor") ||
        Contains(sensor, L"ryzen") || Contains(sensor, L"threadripper") ||
        Contains(sensor, L"epyc") || Contains(sensor, L"xeon") ||
        (Contains(sensor, L"intel") && !gpuSensor);
    if (!cpuSensor) {
        return -1;
    }

    if (Contains(reading, L"vrm") || Contains(reading, L"ccd") ||
        Contains(reading, L"iod") || Contains(reading, L"soc") ||
        Contains(reading, L"l3 cache")) {
        return -1;
    }

    if (Contains(reading, L"tctl/tdie")) {
        return 1000;
    }
    if (Contains(reading, L"cpu die (average)")) {
        return 950;
    }
    if (Contains(reading, L"cpu package")) {
        return 900;
    }
    if (Contains(reading, L"package temperature")) {
        return 850;
    }
    if (Contains(reading, L"cpu temperature")) {
        return 800;
    }
    if (Contains(reading, L"core temperatures")) {
        return 700;
    }
    if (Contains(reading, L"temperature")) {
        return 400;
    }

    // Gadget labels follow the selected HWiNFO UI language. A temperature
    // unit check is applied by the registry reader, so a reading that belongs
    // to a recognized CPU sensor remains a safe low-priority fallback even if
    // words such as "temperature" or "package" are localized.
    return 100;
}

std::wstring NormalizeAdapterIdentity(std::wstring value) {
    value = ToLower(std::move(value));
    for (wchar_t& character : value) {
        if (!std::iswalnum(character)) {
            character = L' ';
        }
    }
    std::wstring normalized;
    bool previousSpace = true;
    for (wchar_t character : value) {
        bool space = std::iswspace(character) != 0;
        if (space) {
            if (!previousSpace) {
                normalized.push_back(L' ');
            }
        } else {
            normalized.push_back(character);
        }
        previousSpace = space;
    }
    if (!normalized.empty() && normalized.back() == L' ') {
        normalized.pop_back();
    }

    // Windows adapter names often include trademark markers that HWiNFO omits
    // (for example "Intel(R)" or "Radeon(TM)"). Remove only these standalone
    // tokens so otherwise identical adapter names still match.
    std::wstring filtered;
    size_t start = 0;
    while (start < normalized.size()) {
        size_t end = normalized.find(L' ', start);
        if (end == std::wstring::npos) {
            end = normalized.size();
        }
        std::wstring_view token(normalized.data() + start, end - start);
        if (token != L"r" && token != L"tm") {
            if (!filtered.empty()) {
                filtered.push_back(L' ');
            }
            filtered.append(token);
        }
        start = end + 1;
    }
    return filtered;
}

std::vector<std::wstring> IdentityTokens(const std::wstring& value) {
    std::vector<std::wstring> tokens;
    size_t start = 0;
    while (start < value.size()) {
        size_t end = value.find(L' ', start);
        if (end == std::wstring::npos) {
            end = value.size();
        }
        if (end > start) {
            tokens.emplace_back(value.substr(start, end - start));
        }
        start = end + 1;
    }
    return tokens;
}

bool HasDigit(const std::wstring& value) {
    return std::any_of(value.begin(), value.end(), [](wchar_t character) {
        return std::iswdigit(character) != 0;
    });
}

int GpuAdapterIdentityScore(const std::wstring& sensorName,
                            const std::wstring& adapterName) {
    if (adapterName.empty()) {
        return 0;
    }
    std::wstring sensor = NormalizeAdapterIdentity(sensorName);
    std::wstring adapter = NormalizeAdapterIdentity(adapterName);
    if (adapter.empty()) {
        return 0;
    }
    if (Contains(sensor, adapter)) {
        return 5000;
    }

    auto sensorTokens = IdentityTokens(sensor);
    auto adapterTokens = IdentityTokens(adapter);
    int numericMatches = 0;
    int distinctiveMatches = 0;
    for (const std::wstring& token : adapterTokens) {
        bool matched = std::find(sensorTokens.begin(), sensorTokens.end(),
                                 token) != sensorTokens.end();
        if (!matched) {
            continue;
        }
        if (HasDigit(token)) {
            numericMatches++;
        } else if (token.size() >= 4 && token != L"graphics" &&
                   token != L"radeon" && token != L"geforce" &&
                   token != L"nvidia" && token != L"intel") {
            distinctiveMatches++;
        }
    }
    if (numericMatches) {
        return 4000 + numericMatches * 100 + distinctiveMatches * 10;
    }
    if (distinctiveMatches >= 2) {
        return 3000 + distinctiveMatches * 10;
    }
    return -1;
}

int GpuTemperatureScore(const std::wstring& sensorName,
                        const std::wstring& label,
                        const std::wstring& preferred,
                        const std::optional<std::wstring>& adapterName) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    std::wstring combined = sensor + L" " + reading;
    std::wstring preferredLower = ToLower(preferred);

    if (!preferredLower.empty()) {
        return Contains(combined, preferredLower) ? 10000 : -1;
    }

    if (!adapterName) {
        return -1;
    }

    int adapterScore = GpuAdapterIdentityScore(sensorName, *adapterName);
    if (adapterScore < 0) {
        return -1;
    }

    if (!Contains(sensor, L"gpu") && !Contains(sensor, L"nvidia") &&
        !Contains(sensor, L"radeon")) {
        return -1;
    }

    if (Contains(reading, L"hot spot") || Contains(reading, L"hotspot") ||
        Contains(reading, L"memory") || Contains(reading, L"vram")) {
        return -1;
    }

    if (reading == L"gpu temperature") {
        return adapterScore + 1000;
    }
    if (Contains(reading, L"gpu temperature")) {
        return adapterScore + 950;
    }
    if (Contains(reading, L"gpu core")) {
        return adapterScore + 900;
    }
    if (Contains(reading, L"temperature")) {
        return adapterScore + 500;
    }

    // Prefer the shortest localized label containing the stable GPU acronym.
    // This normally selects labels such as "GPU Temperature" over longer
    // hotspot, junction, or memory-temperature labels.
    if (Contains(reading, L"gpu")) {
        return adapterScore + 400 -
               static_cast<int>(std::min<size_t>(reading.size(), 200));
    }

    // As with CPU readings, the registry path validates the temperature unit
    // before this locale-independent fallback can be selected.
    return adapterScore + 100;
}

struct HwInfoTemperatureDiagnostics {
    bool gpuTemperatureReadingFound = false;
    bool gpuAdapterMatched = false;
};

bool IsGpuTemperatureReadingCandidate(const std::wstring& sensorName,
                                      const std::wstring& label) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    if (!Contains(sensor, L"gpu") && !Contains(sensor, L"nvidia") &&
        !Contains(sensor, L"radeon")) {
        return false;
    }
    return !Contains(reading, L"hot spot") &&
           !Contains(reading, L"hotspot") &&
           !Contains(reading, L"memory") && !Contains(reading, L"vram");
}

void RecordGpuTemperatureDiagnostic(
    HwInfoTemperatureDiagnostics& diagnostics,
    const std::wstring& sensorName,
    const std::wstring& label,
    const std::optional<std::wstring>& adapterName) {
    if (!IsGpuTemperatureReadingCandidate(sensorName, label)) {
        return;
    }
    diagnostics.gpuTemperatureReadingFound = true;
    if (adapterName &&
        GpuAdapterIdentityScore(sensorName, *adapterName) >= 0) {
        diagnostics.gpuAdapterMatched = true;
    }
}

// HWiNFO's published shared-memory layout explicitly uses one-byte packing.
#pragma pack(push, 1)
struct HwInfoHeader {
    uint32_t signature;
    uint32_t version;
    uint32_t revision;
    int64_t pollTime;
    uint32_t sensorOffset;
    uint32_t sensorStride;
    uint32_t sensorCount;
    uint32_t readingOffset;
    uint32_t readingStride;
    uint32_t readingCount;
    uint32_t pollingPeriod;
};

struct HwInfoSensorPrefix {
    uint32_t sensorId;
    uint32_t sensorInstance;
    char originalName[128];
    char userName[128];
};

struct HwInfoReadingPrefix {
    uint32_t readingType;
    uint32_t sensorIndex;
    uint32_t readingId;
    char originalLabel[128];
    char userLabel[128];
    char unit[16];
    double value;
};
#pragma pack(pop)

static_assert(sizeof(HwInfoHeader) == 48);
static_assert(offsetof(HwInfoHeader, pollTime) == 12);
static_assert(offsetof(HwInfoHeader, sensorOffset) == 20);
static_assert(sizeof(HwInfoSensorPrefix) == 264);
static_assert(offsetof(HwInfoReadingPrefix, value) == 284);
static_assert(sizeof(HwInfoReadingPrefix) == 292);

bool IsRangeValid(size_t totalSize,
                  uint32_t offset,
                  uint32_t stride,
                  uint32_t count,
                  size_t minimumStride) {
    if (stride < minimumStride || offset > totalSize) {
        return false;
    }
    size_t remaining = totalSize - offset;
    return count <= remaining / stride;
}

std::optional<double> NormalizeTemperature(double value,
                                           std::wstring unitOrFormattedValue) {
    if (!std::isfinite(value)) {
        return std::nullopt;
    }

    std::wstring unit = ToLower(unitOrFormattedValue);
    unit.erase(std::remove_if(unit.begin(), unit.end(), [](wchar_t character) {
                   return std::iswspace(character) != 0;
               }),
               unit.end());

    bool celsius = Contains(unit, L"\u00B0c") || Contains(unit, L"\u2103") ||
                   unit == L"c" || unit == L"celsius";
    bool fahrenheit =
        Contains(unit, L"\u00B0f") || Contains(unit, L"\u2109") ||
        unit == L"f" || unit == L"fahrenheit";
    if (celsius == fahrenheit) {
        return std::nullopt;
    }

    double celsiusValue =
        fahrenheit ? (value - 32.0) * 5.0 / 9.0 : value;
    if (!std::isfinite(celsiusValue) || celsiusValue < -50.0 ||
        celsiusValue > 200.0) {
        return std::nullopt;
    }
    return celsiusValue;
}

constexpr char HwInfoTemperatureUnit(const char* unit, size_t capacity) {
    char result = 0;
    for (size_t i = 0; i < capacity && unit[i]; i++) {
        char candidate = 0;
        if (unit[i] == 'C' || unit[i] == 'c') {
            candidate = 'C';
        } else if (unit[i] == 'F' || unit[i] == 'f') {
            candidate = 'F';
        }
        if (candidate) {
            if (result && result != candidate) {
                return 0;
            }
            result = candidate;
        }
    }
    return result;
}

constexpr char kHwInfoRawCelsiusUnit[] = {
    static_cast<char>(0xB0), 'C', 0};
constexpr char kHwInfoRawFahrenheitUnit[] = {
    static_cast<char>(0xB0), 'F', 0};
static_assert(HwInfoTemperatureUnit(kHwInfoRawCelsiusUnit,
                                    std::size(kHwInfoRawCelsiusUnit)) == 'C');
static_assert(HwInfoTemperatureUnit(kHwInfoRawFahrenheitUnit,
                                    std::size(kHwInfoRawFahrenheitUnit)) ==
              'F');

std::optional<double> NormalizeHwInfoTemperature(double value,
                                                 const char* unit,
                                                 size_t capacity) {
    // HWiNFO stores a raw single-byte degree sign followed by an ASCII unit
    // letter. Decoding the buffer through CP_ACP corrupts that sequence on
    // DBCS locales, so classify the ASCII letter directly from the bytes.
    char unitLetter = HwInfoTemperatureUnit(unit, capacity);
    if (!unitLetter) {
        return std::nullopt;
    }
    return NormalizeTemperature(value, unitLetter == 'F' ? L"F" : L"C");
}

void ReadHwInfoSharedMemory(MetricsSnapshot& snapshot,
                            const ModSettings& settings,
                            const std::optional<std::wstring>& gpuAdapterName,
                            HwInfoTemperatureDiagnostics& diagnostics) {
    HANDLE mapping = OpenFileMappingW(FILE_MAP_READ, FALSE,
                                      L"Global\\HWiNFO_SENS_SM2");
    if (!mapping) {
        return;
    }

    HANDLE mutex = OpenMutexW(SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE,
                              L"Global\\HWiNFO_SM2_MUTEX");
    bool mutexOwned = false;
    if (mutex) {
        DWORD waitResult = WaitForSingleObject(mutex, 50);
        if (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_ABANDONED) {
            mutexOwned = true;
        } else {
            CloseHandle(mutex);
            CloseHandle(mapping);
            return;
        }
    }

    void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    if (!view) {
        if (mutexOwned) {
            ReleaseMutex(mutex);
        }
        if (mutex) {
            CloseHandle(mutex);
        }
        CloseHandle(mapping);
        return;
    }

    std::vector<HwInfoSensorPrefix> sensors;
    std::vector<HwInfoReadingPrefix> readings;
    MEMORY_BASIC_INFORMATION memoryInfo{};
    if (VirtualQuery(view, &memoryInfo, sizeof(memoryInfo))) {
        size_t viewOffset = static_cast<const uint8_t*>(view) -
                            static_cast<const uint8_t*>(memoryInfo.BaseAddress);
        size_t mappedSize = viewOffset <= memoryInfo.RegionSize
                                ? memoryInfo.RegionSize - viewOffset
                                : 0;
        if (mappedSize >= sizeof(HwInfoHeader)) {
            // HWiNFO updates this mapping in place. Snapshot the range metadata
            // so every address below uses the same values that passed
            // validation even when the shared mutex isn't accessible from
            // Explorer.
            HwInfoHeader header{};
            std::memcpy(&header, view, sizeof(header));

            if (header.signature == kHwInfoSignature &&
                IsRangeValid(mappedSize, header.sensorOffset,
                             header.sensorStride, header.sensorCount,
                             sizeof(HwInfoSensorPrefix)) &&
                IsRangeValid(mappedSize, header.readingOffset,
                             header.readingStride, header.readingCount,
                             sizeof(HwInfoReadingPrefix))) {
                const auto* bytes = static_cast<const uint8_t*>(view);
                sensors.resize(header.sensorCount);
                readings.resize(header.readingCount);
                for (uint32_t i = 0; i < header.sensorCount; i++) {
                    const uint8_t* address =
                        bytes + header.sensorOffset +
                        static_cast<size_t>(i) * header.sensorStride;
                    std::memcpy(&sensors[i], address, sizeof(sensors[i]));
                }
                for (uint32_t i = 0; i < header.readingCount; i++) {
                    const uint8_t* address =
                        bytes + header.readingOffset +
                        static_cast<size_t>(i) * header.readingStride;
                    std::memcpy(&readings[i], address, sizeof(readings[i]));
                }
                if (!mutexOwned) {
                    HwInfoHeader verificationHeader{};
                    std::memcpy(&verificationHeader, view,
                                sizeof(verificationHeader));
                    if (std::memcmp(&header, &verificationHeader,
                                    sizeof(header)) != 0) {
                        // Explorer can occasionally see the mapping without
                        // access to HWiNFO's mutex. Discard a snapshot whose
                        // generation changed while the raw records were copied.
                        sensors.clear();
                        readings.clear();
                    }
                }
            }
        }
    }

    UnmapViewOfFile(view);
    if (mutexOwned) {
        ReleaseMutex(mutex);
    }
    if (mutex) {
        CloseHandle(mutex);
    }
    CloseHandle(mapping);

    // Keep HWiNFO's shared mutex only while copying the validated raw records.
    // String conversion, scoring and unit normalization don't need to block the
    // producer from refreshing its shared memory.
    int bestCpuScore = -1;
    int bestGpuScore = -1;
    bool sawTemperatureReading = false;
    bool sawSupportedTemperatureUnit = false;
    for (const HwInfoReadingPrefix& reading : readings) {
        if (reading.readingType != kHwInfoTemperatureType ||
            reading.sensorIndex >= sensors.size()) {
            continue;
        }
        sawTemperatureReading = true;

        auto value = NormalizeHwInfoTemperature(
            reading.value, reading.unit, std::size(reading.unit));
        if (!value) {
            continue;
        }
        sawSupportedTemperatureUnit = true;

        const HwInfoSensorPrefix& sensor = sensors[reading.sensorIndex];
        std::wstring sensorName =
            FixedAnsiToWide(sensor.originalName,
                            std::size(sensor.originalName));
        std::wstring label =
            FixedAnsiToWide(reading.originalLabel,
                            std::size(reading.originalLabel));
        RecordGpuTemperatureDiagnostic(diagnostics, sensorName, label,
                                       gpuAdapterName);

        int cpuScore = CpuTemperatureScore(
            sensorName, label, settings.cpuTempSensor);
        if (cpuScore > bestCpuScore) {
            bestCpuScore = cpuScore;
            snapshot.cpuTemp = *value;
            snapshot.cpuTempProvider = TemperatureProvider::HwInfoSharedMemory;
        }

        int gpuScore = GpuTemperatureScore(
            sensorName, label, settings.gpuTempSensor, gpuAdapterName);
        if (gpuScore > bestGpuScore) {
            bestGpuScore = gpuScore;
            snapshot.gpuTemp = *value;
            snapshot.gpuTempProvider = TemperatureProvider::HwInfoSharedMemory;
        }
    }

    if (sawTemperatureReading && !sawSupportedTemperatureUnit &&
        !g_hwInfoInvalidUnitLogged) {
        Wh_Log(L"HWiNFO temperature readings use an unsupported unit");
        g_hwInfoInvalidUnitLogged = true;
    }
}

std::optional<std::wstring> ReadRegistryString(HKEY key,
                                                const std::wstring& name) {
    DWORD type = 0;
    DWORD bytes = 0;
    LONG status = RegQueryValueExW(key, name.c_str(), nullptr, &type, nullptr,
                                   &bytes);
    if (status != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ) || bytes < sizeof(wchar_t)) {
        return std::nullopt;
    }

    std::vector<wchar_t> buffer(bytes / sizeof(wchar_t) + 1, L'\0');
    status = RegQueryValueExW(key, name.c_str(), nullptr, &type,
                              reinterpret_cast<BYTE*>(buffer.data()), &bytes);
    if (status != ERROR_SUCCESS) {
        return std::nullopt;
    }
    return std::wstring(buffer.data());
}

std::optional<double> ParseLocalizedDouble(std::wstring value) {
    std::replace(value.begin(), value.end(), L',', L'.');
    wchar_t* end = nullptr;
    double result = std::wcstod(value.c_str(), &end);
    if (end == value.c_str() || !std::isfinite(result)) {
        return std::nullopt;
    }
    return result;
}

std::optional<double> NormalizeRegistryTemperature(
    const std::wstring& rawValue,
    const std::wstring& formattedValue) {
    auto value = ParseLocalizedDouble(rawValue);
    if (!value) {
        return std::nullopt;
    }

    return NormalizeTemperature(*value, formattedValue);
}

void ReadHwInfoGadgetRegistry(MetricsSnapshot& snapshot,
                              const ModSettings& settings,
                              const std::optional<std::wstring>& gpuAdapterName,
                              HwInfoTemperatureDiagnostics& diagnostics) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\HWiNFO64\\VSB", 0,
                      KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        return;
    }

    int bestCpuScore = snapshot.cpuTemp ? 10000 : -1;
    int bestGpuScore = snapshot.gpuTemp ? 10000 : -1;
    int consecutiveMissing = 0;
    for (int i = 0; i < 1024; i++) {
        std::wstring suffix = std::to_wstring(i);
        auto sensor = ReadRegistryString(key, L"Sensor" + suffix);
        if (!sensor) {
            if (++consecutiveMissing >= 16) {
                break;
            }
            continue;
        }
        consecutiveMissing = 0;
        auto label = ReadRegistryString(key, L"Label" + suffix);
        auto rawValue = ReadRegistryString(key, L"ValueRaw" + suffix);
        auto formattedValue = ReadRegistryString(key, L"Value" + suffix);
        if (!label || !rawValue || !formattedValue) {
            continue;
        }
        auto value =
            NormalizeRegistryTemperature(*rawValue, *formattedValue);
        if (!value) {
            continue;
        }
        RecordGpuTemperatureDiagnostic(diagnostics, *sensor, *label,
                                       gpuAdapterName);

        int cpuScore =
            CpuTemperatureScore(*sensor, *label, settings.cpuTempSensor);
        if (cpuScore > bestCpuScore) {
            bestCpuScore = cpuScore;
            snapshot.cpuTemp = *value;
            snapshot.cpuTempProvider =
                TemperatureProvider::HwInfoGadgetRegistry;
        }

        int gpuScore =
            GpuTemperatureScore(*sensor, *label, settings.gpuTempSensor,
                                gpuAdapterName);
        if (gpuScore > bestGpuScore) {
            bestGpuScore = gpuScore;
            snapshot.gpuTemp = *value;
            snapshot.gpuTempProvider =
                TemperatureProvider::HwInfoGadgetRegistry;
        }
    }

    RegCloseKey(key);
}

void ReadWindowsThermalZones(MetricsSnapshot& snapshot,
                             const ModSettings& settings);
void ReadWindowsGpuTemperature(MetricsSnapshot& snapshot,
                               const ModSettings& settings);

void ReadHwInfoTemperatures(MetricsSnapshot& snapshot,
                            const ModSettings& settings,
                            const std::optional<std::wstring>& gpuAdapterName,
                            HwInfoTemperatureDiagnostics& diagnostics) {
    ReadHwInfoSharedMemory(snapshot, settings, gpuAdapterName, diagnostics);
    if (!snapshot.cpuTemp || !snapshot.gpuTemp) {
        ReadHwInfoGadgetRegistry(snapshot, settings, gpuAdapterName,
                                 diagnostics);
    }
}

void LogHwInfoGpuTemperatureMismatch(
    const MetricsSnapshot& snapshot,
    const ModSettings& settings,
    const std::optional<std::wstring>& gpuAdapterName,
    const HwInfoTemperatureDiagnostics& diagnostics) {
    if (snapshot.gpuTemp || !settings.gpuTempSensor.empty() ||
        !diagnostics.gpuTemperatureReadingFound ||
        diagnostics.gpuAdapterMatched ||
        g_hwInfoGpuAdapterMismatchLogged.exchange(true)) {
        return;
    }

    if (gpuAdapterName) {
        Wh_Log(L"HWiNFO GPU temperature readings found, but none matched "
               L"adapter '%s'; set the GPU temperature sensor filter to "
               L"select one explicitly",
               gpuAdapterName->c_str());
    } else {
        Wh_Log(L"HWiNFO GPU temperature readings found, but the selected "
               L"Windows GPU adapter could not be identified; set the GPU "
               L"temperature sensor filter to select one explicitly");
    }
}

void ReadTemperatures(MetricsSnapshot& snapshot,
                      const ModSettings& settings) {
    std::optional<std::wstring> gpuAdapterName;
    HwInfoTemperatureDiagnostics hwInfoDiagnostics;
    bool usedHwInfo = false;
    if (settings.temperatureSource != TemperatureSource::WindowsNative &&
        settings.temperatureSource != TemperatureSource::Disabled) {
        gpuAdapterName = ResolveGpuTemperatureAdapterName(settings);
    }
    switch (settings.temperatureSource) {
        case TemperatureSource::SharedMemory:
            usedHwInfo = true;
            ReadHwInfoSharedMemory(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            break;

        case TemperatureSource::GadgetRegistry:
            usedHwInfo = true;
            ReadHwInfoGadgetRegistry(snapshot, settings, gpuAdapterName,
                                     hwInfoDiagnostics);
            break;

        case TemperatureSource::WindowsNative:
            ReadWindowsGpuTemperature(snapshot, settings);
            ReadWindowsThermalZones(snapshot, settings);
            break;

        case TemperatureSource::Disabled:
            break;

        case TemperatureSource::HwInfoAuto:
            usedHwInfo = true;
            ReadHwInfoTemperatures(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            break;

        case TemperatureSource::Auto:
        default:
            usedHwInfo = true;
            ReadHwInfoTemperatures(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            if (!snapshot.gpuTemp) {
                ReadWindowsGpuTemperature(snapshot, settings);
            }
            if (!snapshot.cpuTemp) {
                ReadWindowsThermalZones(snapshot, settings);
            }
            break;
    }
    if (usedHwInfo) {
        LogHwInfoGpuTemperatureMismatch(snapshot, settings, gpuAdapterName,
                                        hwInfoDiagnostics);
    }
}

uint64_t FileTimeValue(const FILETIME& value) {
    ULARGE_INTEGER result{};
    result.LowPart = value.dwLowDateTime;
    result.HighPart = value.dwHighDateTime;
    return result.QuadPart;
}

std::optional<double> ReadCpuUsage() {
    static bool initialized = false;
    static uint64_t previousIdle = 0;
    static uint64_t previousKernel = 0;
    static uint64_t previousUser = 0;

    FILETIME idleTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return std::nullopt;
    }

    uint64_t idle = FileTimeValue(idleTime);
    uint64_t kernel = FileTimeValue(kernelTime);
    uint64_t user = FileTimeValue(userTime);
    if (!initialized) {
        initialized = true;
        previousIdle = idle;
        previousKernel = kernel;
        previousUser = user;
        return std::nullopt;
    }

    uint64_t idleDelta = idle - previousIdle;
    uint64_t kernelDelta = kernel - previousKernel;
    uint64_t userDelta = user - previousUser;
    previousIdle = idle;
    previousKernel = kernel;
    previousUser = user;

    uint64_t total = kernelDelta + userDelta;
    if (!total || idleDelta > total) {
        return std::nullopt;
    }
    return std::clamp(100.0 * static_cast<double>(total - idleDelta) /
                          static_cast<double>(total),
                      0.0, 100.0);
}

void ReadMemory(MetricsSnapshot& snapshot) {
    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);
    if (!GlobalMemoryStatusEx(&memory)) {
        return;
    }

    snapshot.ram = static_cast<double>(memory.dwMemoryLoad);
    snapshot.ramTotalGb = static_cast<double>(memory.ullTotalPhys) / kGiB;
    snapshot.ramUsedGb =
        static_cast<double>(memory.ullTotalPhys - memory.ullAvailPhys) / kGiB;
    snapshot.ramAvailable = true;
}

// D3DKMT exposes display-adapter performance data, including temperature,
// without requiring a third-party monitoring application. The declarations are
// kept local so the mod can build in Windhawk environments without d3dkmthk.h.
using D3DKMT_HANDLE = UINT32;

struct D3DKMT_OPENADAPTERFROMLUID {
    LUID AdapterLuid;
    D3DKMT_HANDLE hAdapter;
};

struct D3DKMT_CLOSEADAPTER {
    D3DKMT_HANDLE hAdapter;
};

struct D3DKMT_QUERYADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    UINT Type;
    void* pPrivateDriverData;
    UINT PrivateDriverDataSize;
};

struct D3DKMT_ADAPTER_PERFDATA {
    UINT PhysicalAdapterIndex;
    ULONGLONG MemoryFrequency;
    ULONGLONG MaxMemoryFrequency;
    ULONGLONG MaxMemoryFrequencyOC;
    ULONGLONG MemoryBandwidth;
    ULONGLONG PCIEBandwidth;
    ULONG FanRPM;
    ULONG Power;
    ULONG Temperature;
    UCHAR PowerStateOverride;
};

struct D3DKMT_ADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    LUID AdapterLuid;
    ULONG NumOfSources;
    BOOL bPresentMoveRegionsPreferred;
};

struct D3DKMT_ENUMADAPTERS2 {
    ULONG NumAdapters;
    D3DKMT_ADAPTERINFO* pAdapters;
};

struct D3DKMT_ADAPTERREGISTRYINFO {
    WCHAR AdapterString[MAX_PATH];
    WCHAR BiosString[MAX_PATH];
    WCHAR DacType[MAX_PATH];
    WCHAR ChipType[MAX_PATH];
};

struct D3DKMT_SEGMENTSIZEINFO {
    ULONGLONG DedicatedVideoMemorySize;
    ULONGLONG DedicatedSystemMemorySize;
    ULONGLONG SharedSystemMemorySize;
};

struct D3DKMT_ADAPTERTYPE {
    UINT Value;
};

constexpr UINT kAdapterRegistryInfoQueryType = 8;
constexpr UINT kAdapterSegmentSizeQueryType = 3;
constexpr UINT kAdapterTypeQueryType = 15;
constexpr UINT kAdapterPerfDataQueryType = 62;  // KMTQAITYPE_ADAPTERPERFDATA
constexpr ULONG kMaxD3dkmtAdapters = 16;
constexpr UINT kHybridIntegratedAdapterFlag = 1u << 5;

using D3DKMTEnumAdapters2_t = LONG(WINAPI*)(D3DKMT_ENUMADAPTERS2*);
using D3DKMTOpenAdapterFromLuid_t =
    LONG(WINAPI*)(D3DKMT_OPENADAPTERFROMLUID*);
using D3DKMTQueryAdapterInfo_t =
    LONG(WINAPI*)(D3DKMT_QUERYADAPTERINFO*);
using D3DKMTCloseAdapter_t =
    LONG(WINAPI*)(const D3DKMT_CLOSEADAPTER*);

D3DKMTEnumAdapters2_t g_d3dkmtEnumAdapters2 = nullptr;
D3DKMTOpenAdapterFromLuid_t g_d3dkmtOpenAdapterFromLuid = nullptr;
D3DKMTQueryAdapterInfo_t g_d3dkmtQueryAdapterInfo = nullptr;
D3DKMTCloseAdapter_t g_d3dkmtCloseAdapter = nullptr;

struct GpuAdapterInfo {
    std::wstring description;
    std::wstring luid;
    LUID luidValue{};
    uint64_t dedicatedVideoMemory = 0;
    uint64_t sharedSystemMemory = 0;
    bool integrated = false;
};

std::optional<std::wstring> g_cachedGpuAdapterFilter;
std::optional<GpuAdapterInfo> g_cachedGpuAdapterInfo;
bool g_cachedGpuAdapterResolved = false;
D3DKMT_HANDLE g_cachedD3dkmtAdapterHandle = 0;
LUID g_cachedD3dkmtAdapterLuid{};
uint32_t g_unchangedGpuIdentityChecks = 0;

bool SameLuid(const LUID& left, const LUID& right) {
    return left.HighPart == right.HighPart && left.LowPart == right.LowPart;
}

void CloseCachedD3dkmtAdapterHandle() {
    if (g_cachedD3dkmtAdapterHandle && g_d3dkmtCloseAdapter) {
        D3DKMT_CLOSEADAPTER closeAdapter{g_cachedD3dkmtAdapterHandle};
        g_d3dkmtCloseAdapter(&closeAdapter);
    }
    g_cachedD3dkmtAdapterHandle = 0;
    g_cachedD3dkmtAdapterLuid = {};
}

void InvalidateGpuAdapterCache() {
    CloseCachedD3dkmtAdapterHandle();
    g_cachedGpuAdapterFilter.reset();
    g_cachedGpuAdapterInfo.reset();
    g_cachedGpuAdapterResolved = false;
    g_nextGpuIdentityCheck = {};
    g_unchangedGpuIdentityChecks = 0;
}

std::wstring FormatAdapterLuid(const LUID& luid) {
    wchar_t buffer[32];
    swprintf(buffer, std::size(buffer), L"0x%08X_0x%08X",
             static_cast<DWORD>(luid.HighPart), luid.LowPart);
    return ToLower(buffer);
}

std::wstring FixedWideToString(const wchar_t* value, size_t capacity) {
    size_t length = 0;
    while (length < capacity && value[length]) {
        length++;
    }
    return std::wstring(value, length);
}

std::optional<GpuAdapterInfo> GetLiveD3dkmtAdapterInfo(
    const std::wstring& filterLower) {
    if (!g_d3dkmtEnumAdapters2 || !g_d3dkmtQueryAdapterInfo ||
        !g_d3dkmtCloseAdapter) {
        return std::nullopt;
    }

    D3DKMT_ADAPTERINFO adapters[kMaxD3dkmtAdapters]{};
    D3DKMT_ENUMADAPTERS2 enumeration{};
    enumeration.NumAdapters = std::size(adapters);
    enumeration.pAdapters = adapters;
    if (g_d3dkmtEnumAdapters2(&enumeration) != 0) {
        return std::nullopt;
    }

    std::optional<GpuAdapterInfo> selected;
    ULONG adapterCount = std::min<ULONG>(enumeration.NumAdapters,
                                         std::size(adapters));
    for (ULONG index = 0; index < adapterCount; index++) {
        const auto& adapter = adapters[index];

        D3DKMT_ADAPTERREGISTRYINFO registryInfo{};
        D3DKMT_QUERYADAPTERINFO registryQuery{};
        registryQuery.hAdapter = adapter.hAdapter;
        registryQuery.Type = kAdapterRegistryInfoQueryType;
        registryQuery.pPrivateDriverData = &registryInfo;
        registryQuery.PrivateDriverDataSize = sizeof(registryInfo);
        bool registryAvailable =
            g_d3dkmtQueryAdapterInfo(&registryQuery) == 0;

        D3DKMT_SEGMENTSIZEINFO segmentInfo{};
        D3DKMT_QUERYADAPTERINFO segmentQuery{};
        segmentQuery.hAdapter = adapter.hAdapter;
        segmentQuery.Type = kAdapterSegmentSizeQueryType;
        segmentQuery.pPrivateDriverData = &segmentInfo;
        segmentQuery.PrivateDriverDataSize = sizeof(segmentInfo);
        bool segmentsAvailable =
            g_d3dkmtQueryAdapterInfo(&segmentQuery) == 0;

        D3DKMT_ADAPTERTYPE adapterType{};
        D3DKMT_QUERYADAPTERINFO adapterTypeQuery{};
        adapterTypeQuery.hAdapter = adapter.hAdapter;
        adapterTypeQuery.Type = kAdapterTypeQueryType;
        adapterTypeQuery.pPrivateDriverData = &adapterType;
        adapterTypeQuery.PrivateDriverDataSize = sizeof(adapterType);
        bool adapterTypeAvailable =
            g_d3dkmtQueryAdapterInfo(&adapterTypeQuery) == 0;

        std::wstring description =
            registryAvailable
                ? FixedWideToString(registryInfo.AdapterString,
                                    std::size(registryInfo.AdapterString))
                : L"";
        GpuAdapterInfo candidate{
            description,
            FormatAdapterLuid(adapter.AdapterLuid),
            adapter.AdapterLuid,
            segmentsAvailable ? segmentInfo.DedicatedVideoMemorySize : 0,
            segmentsAvailable ? segmentInfo.SharedSystemMemorySize : 0,
            adapterTypeAvailable &&
                (adapterType.Value & kHybridIntegratedAdapterFlag) != 0,
        };

        bool matchesFilter =
            filterLower.empty() ||
            Contains(ToLower(candidate.description), filterLower);
        bool betterCandidate =
            !selected || candidate.dedicatedVideoMemory >
                             selected->dedicatedVideoMemory ||
            (candidate.dedicatedVideoMemory ==
                 selected->dedicatedVideoMemory &&
             !candidate.description.empty() &&
             selected->description.empty()) ||
            (candidate.dedicatedVideoMemory ==
                 selected->dedicatedVideoMemory &&
             candidate.description.empty() == selected->description.empty() &&
             candidate.sharedSystemMemory > selected->sharedSystemMemory);
        if (matchesFilter && betterCandidate) {
            selected = std::move(candidate);
        }
    }

    for (ULONG index = 0; index < adapterCount; index++) {
        if (adapters[index].hAdapter) {
            D3DKMT_CLOSEADAPTER closeAdapter{adapters[index].hAdapter};
            g_d3dkmtCloseAdapter(&closeAdapter);
        }
    }

    // Stale D3DKMT duplicates can retain the full memory sizes while losing
    // their registry identity. Prefer the DXGI compatibility path instead of
    // caching such an ambiguous adapter.
    return selected && !selected->description.empty() ? selected
                                                       : std::nullopt;
}

std::optional<GpuAdapterInfo> GetDxgiAdapterInfo(
    const std::wstring& filterLower) {
    com_ptr<IDXGIFactory> factory;
    if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.put())))) {
        return std::nullopt;
    }

    DXGI_ADAPTER_DESC selected{};
    bool found = false;
    for (UINT index = 0;; index++) {
        com_ptr<IDXGIAdapter> adapter;
        HRESULT result = factory->EnumAdapters(index, adapter.put());
        if (result == DXGI_ERROR_NOT_FOUND) {
            break;
        }
        if (FAILED(result)) {
            continue;
        }

        DXGI_ADAPTER_DESC description{};
        if (FAILED(adapter->GetDesc(&description))) {
            continue;
        }

        if (!filterLower.empty()) {
            if (Contains(ToLower(description.Description), filterLower)) {
                selected = description;
                found = true;
                break;
            }
        } else if (!found || description.DedicatedVideoMemory >
                                      selected.DedicatedVideoMemory) {
            selected = description;
            found = true;
        }
    }

    if (!found) {
        return std::nullopt;
    }

    return GpuAdapterInfo{
        selected.Description, FormatAdapterLuid(selected.AdapterLuid),
        selected.AdapterLuid,
        selected.DedicatedVideoMemory, selected.SharedSystemMemory, false};
}

std::optional<GpuAdapterInfo> ResolveCurrentGpuAdapterInfo(
    const std::wstring& filterLower,
    PCWSTR* provider = nullptr) {
    auto adapter = GetLiveD3dkmtAdapterInfo(filterLower);
    PCWSTR resolvedProvider = L"D3DKMT";
    if (!adapter) {
        adapter = GetDxgiAdapterInfo(filterLower);
        resolvedProvider = L"DXGI fallback";
    }
    if (provider) {
        *provider = resolvedProvider;
    }
    return adapter;
}

std::optional<GpuAdapterInfo> GetGpuAdapterInfo(
    const std::wstring& adapterFilter) {
    std::wstring filterLower = ToLower(adapterFilter);
    if (g_cachedGpuAdapterResolved &&
        g_cachedGpuAdapterFilter == filterLower) {
        return g_cachedGpuAdapterInfo;
    }

    if (g_cachedGpuAdapterFilter &&
        *g_cachedGpuAdapterFilter != filterLower) {
        CloseCachedD3dkmtAdapterHandle();
        g_nextGpuIdentityCheck = {};
        g_unchangedGpuIdentityChecks = 0;
    }

    g_cachedGpuAdapterFilter = filterLower;
    PCWSTR provider = nullptr;
    g_cachedGpuAdapterInfo =
        ResolveCurrentGpuAdapterInfo(filterLower, &provider);
    g_cachedGpuAdapterResolved = true;

    if (!g_cachedGpuAdapterInfo) {
        if (filterLower.empty()) {
            Wh_Log(L"No GPU adapter found");
        } else {
            Wh_Log(L"No GPU adapter matched: %s", filterLower.c_str());
        }
        return std::nullopt;
    }

    Wh_Log(L"Selected GPU (%s): %s, LUID %s, dedicated %.1f GiB, shared %.1f "
           L"GiB, integrated=%d",
           provider, g_cachedGpuAdapterInfo->description.c_str(),
           g_cachedGpuAdapterInfo->luid.c_str(),
           static_cast<double>(g_cachedGpuAdapterInfo->dedicatedVideoMemory) /
               kGiB,
           static_cast<double>(g_cachedGpuAdapterInfo->sharedSystemMemory) /
               kGiB,
           g_cachedGpuAdapterInfo->integrated);
    return g_cachedGpuAdapterInfo;
}

std::optional<std::wstring> ResolveGpuTemperatureAdapterName(
    const ModSettings& settings) {
    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (adapter) {
        return adapter->description;
    }
    if (settings.gpuAdapter.empty()) {
        // Preserve generic HWiNFO matching on systems where Windows adapter
        // identity isn't available at all. An explicit filter miss is different
        // and intentionally returns nullopt so another GPU isn't shown.
        return std::wstring{};
    }
    return std::nullopt;
}

bool HasGpuAdapterIdentityChanged(const GpuAdapterInfo& cachedAdapter,
                                  const std::wstring& adapterFilter) {
    auto now = std::chrono::steady_clock::now();
    if (now < g_nextGpuIdentityCheck) {
        return false;
    }
    auto currentAdapter =
        ResolveCurrentGpuAdapterInfo(ToLower(adapterFilter));
    if (currentAdapter && currentAdapter->luid != cachedAdapter.luid) {
        g_unchangedGpuIdentityChecks = 0;
        g_nextGpuIdentityCheck = {};
        return true;
    }

    // A parked or power-gated discrete GPU can temporarily resolve through a
    // different enumeration path. Back off repeated identity probes while the
    // cached adapter remains valid instead of re-enumerating every five seconds.
    uint32_t exponent = std::min<uint32_t>(g_unchangedGpuIdentityChecks, 6);
    auto delay = std::min(std::chrono::seconds(5u << exponent),
                          std::chrono::seconds(300));
    g_unchangedGpuIdentityChecks++;
    g_nextGpuIdentityCheck = now + delay;
    return false;
}

std::optional<D3DKMT_HANDLE> GetD3dkmtAdapterHandle(
    const GpuAdapterInfo& adapter) {
    if (g_cachedD3dkmtAdapterHandle &&
        SameLuid(g_cachedD3dkmtAdapterLuid, adapter.luidValue)) {
        return g_cachedD3dkmtAdapterHandle;
    }

    CloseCachedD3dkmtAdapterHandle();
    D3DKMT_OPENADAPTERFROMLUID openAdapter{};
    openAdapter.AdapterLuid = adapter.luidValue;
    if (g_d3dkmtOpenAdapterFromLuid(&openAdapter) != 0 ||
        !openAdapter.hAdapter) {
        return std::nullopt;
    }
    g_cachedD3dkmtAdapterHandle = openAdapter.hAdapter;
    g_cachedD3dkmtAdapterLuid = adapter.luidValue;
    return g_cachedD3dkmtAdapterHandle;
}

void ReadWindowsGpuTemperature(MetricsSnapshot& snapshot,
                               const ModSettings& settings) {
    if (snapshot.gpuTemp || !g_d3dkmtOpenAdapterFromLuid ||
        !g_d3dkmtQueryAdapterInfo || !g_d3dkmtCloseAdapter) {
        return;
    }

    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (!adapter) {
        return;
    }

    auto adapterHandle = GetD3dkmtAdapterHandle(*adapter);
    if (!adapterHandle) {
        return;
    }

    D3DKMT_ADAPTER_PERFDATA perfData{};
    D3DKMT_QUERYADAPTERINFO queryInfo{};
    queryInfo.hAdapter = *adapterHandle;
    queryInfo.Type = kAdapterPerfDataQueryType;
    queryInfo.pPrivateDriverData = &perfData;
    queryInfo.PrivateDriverDataSize = sizeof(perfData);

    LONG status = g_d3dkmtQueryAdapterInfo(&queryInfo);
    if (status != 0) {
        // A driver restart invalidates KMT handles. Reopen on the next sample.
        CloseCachedD3dkmtAdapterHandle();
    }

    // The driver reports tenths of a degree Celsius. Zero means unavailable;
    // reject values above 200 C as invalid driver data.
    if (status != 0 || perfData.Temperature == 0 ||
        perfData.Temperature > 2000) {
        return;
    }

    snapshot.gpuTemp = perfData.Temperature / 10.0;
    snapshot.gpuTempProvider = TemperatureProvider::WindowsD3dkmt;
}

bool MatchesGpuAdapter(const std::wstring& instance,
                       const std::optional<GpuAdapterInfo>& adapter) {
    return !adapter || Contains(ToLower(instance), adapter->luid);
}

void CloseMetricSources();

constexpr auto kPdhCounterRetryInterval = std::chrono::seconds(30);
constexpr auto kPdhRecoveryRetryDelay = std::chrono::seconds(1);
constexpr auto kPdhRecoveryCooldown = std::chrono::seconds(30);
constexpr uint32_t kPdhReadFailureThreshold = 3;

void ClosePdhQuery() {
    if (g_pdhQuery) {
        PdhCloseQuery(g_pdhQuery);
        g_pdhQuery = nullptr;
    }
    g_cpuUtilityCounter = nullptr;
    g_gpuCounter = nullptr;
    g_vramCounter = nullptr;
    g_sharedVramCounter = nullptr;
    g_thermalZoneCounter = nullptr;
}

void RecreatePdhSources(PCWSTR reason,
                        PDH_STATUS status,
                        std::chrono::steady_clock::time_point now) {
    if (status == ERROR_SUCCESS) {
        Wh_Log(L"Recreating GPU performance counters after %s", reason);
    } else {
        Wh_Log(L"Recreating GPU performance counters after %s: %08X",
               reason, status);
    }

    ClosePdhQuery();
    InvalidateGpuAdapterCache();
    g_consecutivePdhReadFailures = 0;
    g_nextPdhCounterRetry = now + kPdhRecoveryRetryDelay;
    g_nextPdhRecovery = now + kPdhRecoveryCooldown;
}

void RecordPdhReadFailure(PCWSTR reason,
                          PDH_STATUS status = ERROR_SUCCESS) {
    g_consecutivePdhReadFailures =
        std::min(g_consecutivePdhReadFailures + 1,
                 kPdhReadFailureThreshold);

    auto now = std::chrono::steady_clock::now();
    if (g_consecutivePdhReadFailures < kPdhReadFailureThreshold ||
        now < g_nextPdhRecovery) {
        return;
    }

    RecreatePdhSources(reason, status, now);
}

void RecoverFromGpuAdapterIdentityChange() {
    auto now = std::chrono::steady_clock::now();
    if (now < g_nextPdhRecovery) {
        return;
    }
    RecreatePdhSources(L"confirmed adapter LUID change", ERROR_SUCCESS, now);
}

void RecordPdhReadSuccess() {
    g_consecutivePdhReadFailures = 0;
}

bool AddPdhCounter(PDH_HCOUNTER& counter,
                   PCWSTR path,
                   PCWSTR description) {
    if (counter) {
        return false;
    }

    PDH_HCOUNTER newCounter = nullptr;
    PDH_STATUS status =
        PdhAddEnglishCounterW(g_pdhQuery, path, 0, &newCounter);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Adding the %s counter failed: %08X", description, status);
        return false;
    }

    counter = newCounter;
    return true;
}

bool NeedsWindowsThermalZones(const ModSettings& settings) {
    return settings.temperatureSource == TemperatureSource::Auto ||
           settings.temperatureSource == TemperatureSource::WindowsNative;
}

bool EnsurePdhQuery(const ModSettings& settings) {
    auto now = std::chrono::steady_clock::now();
    bool thermalZonesRequired = NeedsWindowsThermalZones(settings);
    if (!thermalZonesRequired && g_thermalZoneCounter) {
        PdhRemoveCounter(g_thermalZoneCounter);
        g_thermalZoneCounter = nullptr;
    }

    bool queryCreated = false;
    if (!g_pdhQuery) {
        if (now < g_nextPdhCounterRetry) {
            return false;
        }
        if (PdhOpenQueryW(nullptr, 0, &g_pdhQuery) != ERROR_SUCCESS) {
            g_pdhQuery = nullptr;
            g_nextPdhCounterRetry = now + kPdhCounterRetryInterval;
            return false;
        }
        queryCreated = true;
    } else if (g_cpuUtilityCounter && g_gpuCounter && g_vramCounter &&
               g_sharedVramCounter &&
               (!thermalZonesRequired || g_thermalZoneCounter)) {
        return false;
    }

    if (!queryCreated && now < g_nextPdhCounterRetry) {
        return false;
    }

    bool counterAdded = false;
    counterAdded |= AddPdhCounter(
        g_cpuUtilityCounter,
        L"\\Processor Information(_Total)\\% Processor Utility",
        L"CPU utility");
    counterAdded |= AddPdhCounter(
        g_gpuCounter, L"\\GPU Engine(*)\\Utilization Percentage", L"GPU usage");
    counterAdded |= AddPdhCounter(
        g_vramCounter, L"\\GPU Adapter Memory(*)\\Dedicated Usage",
        L"VRAM usage");
    counterAdded |= AddPdhCounter(
        g_sharedVramCounter, L"\\GPU Adapter Memory(*)\\Shared Usage",
        L"shared GPU-memory usage");
    if (thermalZonesRequired) {
        counterAdded |= AddPdhCounter(
            g_thermalZoneCounter,
            L"\\Thermal Zone Information(*)\\Temperature",
            L"Windows thermal-zone");
    }

    if (!g_cpuUtilityCounter && !g_gpuCounter && !g_vramCounter &&
        !g_sharedVramCounter &&
        (!thermalZonesRequired || !g_thermalZoneCounter)) {
        PdhCloseQuery(g_pdhQuery);
        g_pdhQuery = nullptr;
        g_nextPdhCounterRetry = now + kPdhCounterRetryInterval;
        return false;
    }

    if (!g_cpuUtilityCounter || !g_gpuCounter || !g_vramCounter ||
        !g_sharedVramCounter ||
        (thermalZonesRequired && !g_thermalZoneCounter)) {
        g_nextPdhCounterRetry = now + kPdhCounterRetryInterval;
    }

    if (queryCreated || counterAdded) {
        PDH_STATUS collectStatus = PdhCollectQueryData(g_pdhQuery);
        if (collectStatus != ERROR_SUCCESS) {
            Wh_Log(L"Initial metric counter collection failed: %08X",
                   collectStatus);
        }
    }
    return queryCreated || counterAdded;
}

PDH_STATUS ReadPdhArray(PDH_HCOUNTER counter,
                        std::vector<uint8_t>& buffer,
                        DWORD& itemCount) {
    if (!counter) {
        return PDH_CSTATUS_NO_COUNTER;
    }
    // GPU wildcard instances can appear or disappear between the sizing call
    // and the data call. PDH_MORE_DATA on a later call is a normal resize race,
    // not evidence that the provider or display driver is broken.
    constexpr int kMaxArrayReadAttempts = 4;
    DWORD bufferSize = 0;
    PDH_STATUS status = static_cast<PDH_STATUS>(PDH_MORE_DATA);
    for (int attempt = 0; attempt < kMaxArrayReadAttempts; attempt++) {
        itemCount = 0;
        auto* items = bufferSize
                          ? reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(
                                buffer.data())
                          : nullptr;
        status = PdhGetFormattedCounterArrayW(
            counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items);
        if (status == ERROR_SUCCESS) {
            if (!bufferSize) {
                buffer.clear();
                itemCount = 0;
            }
            return ERROR_SUCCESS;
        }
        if (status != static_cast<PDH_STATUS>(PDH_MORE_DATA) || !bufferSize) {
            return status;
        }
        buffer.resize(bufferSize);
    }
    return status;
}

bool IsHardPdhArrayFailure(PDH_STATUS status) {
    return status != ERROR_SUCCESS &&
           status != static_cast<PDH_STATUS>(PDH_NO_DATA) &&
           status != static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE);
}

void ReadWindowsThermalZones(MetricsSnapshot& snapshot,
                             const ModSettings& settings) {
    if (snapshot.cpuTemp || !g_thermalZoneCounter) {
        return;
    }

    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    if (ReadPdhArray(g_thermalZoneCounter, buffer, itemCount) !=
        ERROR_SUCCESS) {
        return;
    }

    std::wstring filter = ToLower(settings.windowsThermalZoneFilter);
    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    double aggregate = 0.0;
    size_t validCount = 0;

    for (DWORD i = 0; i < itemCount; i++) {
        const auto& item = items[i];
        const auto& value = item.FmtValue;
        if (value.CStatus != PDH_CSTATUS_VALID_DATA &&
            value.CStatus != PDH_CSTATUS_NEW_DATA) {
            continue;
        }

        std::wstring instance = item.szName ? ToLower(item.szName) : L"";
        if (!filter.empty() && !Contains(instance, filter)) {
            continue;
        }

        // This counter is reported in Kelvin. Match Taskbar Clock
        // Customization by rejecting dead zones below 200 K, and reject values
        // above the module's supported 200 °C ceiling as corrupt data.
        double kelvin = value.doubleValue;
        if (!std::isfinite(kelvin) || kelvin < 200.0 || kelvin > 473.15) {
            continue;
        }

        double celsius = kelvin - 273.15;
        if (settings.windowsThermalZoneAggregation ==
            ThermalZoneAggregation::Hottest) {
            aggregate = validCount ? std::max(aggregate, celsius) : celsius;
        } else {
            aggregate += celsius;
        }
        validCount++;
    }

    if (!validCount) {
        return;
    }

    if (settings.windowsThermalZoneAggregation ==
        ThermalZoneAggregation::Average) {
        aggregate /= validCount;
    }
    snapshot.cpuTemp = aggregate;
    snapshot.cpuTempProvider = TemperatureProvider::WindowsThermalZones;
}

std::optional<double> ReadGpuUsage(
    const std::optional<GpuAdapterInfo>& adapter,
    PDH_STATUS& readStatus) {
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    readStatus = ReadPdhArray(g_gpuCounter, buffer, itemCount);
    if (readStatus == static_cast<PDH_STATUS>(PDH_NO_DATA) ||
        readStatus == static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE)) {
        return 0.0;
    }
    if (readStatus != ERROR_SUCCESS) {
        return std::nullopt;
    }

    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    std::unordered_map<std::wstring, double> engineTotals;
    bool found = false;
    for (DWORD i = 0; i < itemCount; i++) {
        const auto& value = items[i].FmtValue;
        if ((value.CStatus != PDH_CSTATUS_VALID_DATA &&
             value.CStatus != PDH_CSTATUS_NEW_DATA) ||
            !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
            continue;
        }
        std::wstring instance = items[i].szName ? items[i].szName : L"";
        if (!MatchesGpuAdapter(instance, adapter)) {
            continue;
        }
        size_t luidPosition = instance.find(L"luid_");
        std::wstring engineKey =
            luidPosition == std::wstring::npos ? instance
                                                : instance.substr(luidPosition);
        engineTotals[engineKey] += value.doubleValue;
        found = true;
    }

    double busiestEngine = 0.0;
    for (const auto& [engine, usage] : engineTotals) {
        busiestEngine = std::max(busiestEngine, usage);
    }
    if (found) {
        return std::clamp(busiestEngine, 0.0, 100.0);
    }
    // A power-gated discrete GPU legitimately has no engine instances while an
    // integrated adapter is active. Adapter identity recovery is handled from
    // the memory counter below, so an otherwise healthy array means idle here.
    return 0.0;
}

std::optional<double> ReadCpuUtility() {
    if (!g_cpuUtilityCounter) {
        return std::nullopt;
    }
    PDH_FMT_COUNTERVALUE value{};
    PDH_STATUS status = PdhGetFormattedCounterValue(
        g_cpuUtilityCounter, PDH_FMT_DOUBLE, nullptr, &value);
    if (status != ERROR_SUCCESS ||
        (value.CStatus != PDH_CSTATUS_VALID_DATA &&
         value.CStatus != PDH_CSTATUS_NEW_DATA) ||
        !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
        return std::nullopt;
    }
    return std::clamp(value.doubleValue, 0.0, 100.0);
}

std::optional<double> ReadVramUsedBytes(
    PDH_HCOUNTER counter,
    const std::optional<GpuAdapterInfo>& adapter,
    PDH_STATUS& readStatus) {
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    readStatus = ReadPdhArray(counter, buffer, itemCount);
    if (readStatus != ERROR_SUCCESS) {
        return std::nullopt;
    }

    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    double total = 0.0;
    bool found = false;
    for (DWORD i = 0; i < itemCount; i++) {
        const auto& value = items[i].FmtValue;
        if ((value.CStatus != PDH_CSTATUS_VALID_DATA &&
             value.CStatus != PDH_CSTATUS_NEW_DATA) ||
            !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
            continue;
        }
        std::wstring instance = items[i].szName ? items[i].szName : L"";
        if (!MatchesGpuAdapter(instance, adapter)) {
            continue;
        }
        total += value.doubleValue;
        found = true;
    }
    return found ? std::optional<double>(total) : std::nullopt;
}

bool LooksLikeIntegratedGpu(const GpuAdapterInfo& adapter) {
    if (adapter.integrated || adapter.dedicatedVideoMemory == 0) {
        return true;
    }

    // Some WDDM drivers don't set HybridIntegrated but expose a small firmware
    // carve-out. Restrict the fallback to familiar integrated-family names so
    // an older low-memory discrete adapter isn't silently reclassified.
    constexpr uint64_t kMaximumIntegratedCarveout = 512ull * 1024 * 1024;
    if (adapter.dedicatedVideoMemory > kMaximumIntegratedCarveout ||
        adapter.sharedSystemMemory == 0) {
        return false;
    }
    std::wstring name = ToLower(adapter.description);
    return Contains(name, L"uhd graphics") || Contains(name, L"iris") ||
           Contains(name, L"radeon(tm) graphics") ||
           Contains(name, L"radeon graphics") || Contains(name, L"vega") ||
           Contains(name, L"integrated");
}

bool UseSharedGpuMemory(const GpuAdapterInfo& adapter,
                        const ModSettings& settings) {
    switch (settings.gpuMemoryMode) {
        case GpuMemoryMode::Dedicated:
            return false;
        case GpuMemoryMode::Shared:
            return true;
        case GpuMemoryMode::Auto:
        default:
            return adapter.sharedSystemMemory > 0 &&
                   LooksLikeIntegratedGpu(adapter);
    }
}

bool IsSoftPdhArrayAbsence(PDH_STATUS status) {
    return status == ERROR_SUCCESS ||
           status == static_cast<PDH_STATUS>(PDH_NO_DATA) ||
           status == static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE);
}

bool ReadPdhMetrics(MetricsSnapshot& snapshot, const ModSettings& settings) {
    if (EnsurePdhQuery(settings)) {
        // Rate counters need two samples. EnsurePdhQuery just established the
        // baseline, so publishing this same-tick snapshot would expose garbage
        // or briefly replace valid GPU values with unavailable placeholders.
        return false;
    }
    if (!g_pdhQuery) {
        return true;
    }

    PDH_STATUS collectStatus = PdhCollectQueryData(g_pdhQuery);
    if (collectStatus != ERROR_SUCCESS) {
        RecordPdhReadFailure(L"collection", collectStatus);
        return true;
    }

    if (auto cpuUtility = ReadCpuUtility()) {
        snapshot.cpu = *cpuUtility;
        snapshot.cpuAvailable = true;
    }

    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    bool explicitAdapterMissing = !settings.gpuAdapter.empty() && !adapter;
    PDH_STATUS gpuReadStatus = ERROR_SUCCESS;
    auto gpuUsage = explicitAdapterMissing
                        ? std::optional<double>{}
                        : ReadGpuUsage(adapter, gpuReadStatus);
    if (gpuUsage) {
        snapshot.gpu = *gpuUsage;
        snapshot.gpuAvailable = true;
    }
    uint64_t vramTotalBytes = 0;
    PDH_HCOUNTER vramCounter = nullptr;
    if (adapter) {
        if (UseSharedGpuMemory(*adapter, settings)) {
            vramTotalBytes = adapter->sharedSystemMemory;
            vramCounter = g_sharedVramCounter;
        } else {
            vramTotalBytes = adapter->dedicatedVideoMemory;
            vramCounter = g_vramCounter;
        }
    }
    PDH_STATUS vramReadStatus = ERROR_SUCCESS;
    auto vramUsedBytes = ReadVramUsedBytes(vramCounter, adapter,
                                           vramReadStatus);
    bool vramAvailable = vramTotalBytes > 0 && vramUsedBytes.has_value();
    if (vramAvailable) {
        snapshot.vramUsedGb = *vramUsedBytes / kGiB;
        snapshot.vramTotalGb = static_cast<double>(vramTotalBytes) / kGiB;
        snapshot.vram = std::clamp(
            snapshot.vramUsedGb / snapshot.vramTotalGb * 100.0, 0.0, 100.0);
        snapshot.vramAvailable = true;
    }

    bool hardReadFailure =
        (g_gpuCounter && IsHardPdhArrayFailure(gpuReadStatus)) ||
        (vramCounter && IsHardPdhArrayFailure(vramReadStatus));
    bool adapterSampleMissing = adapter && vramCounter && vramTotalBytes > 0 &&
                                !vramAvailable &&
                                IsSoftPdhArrayAbsence(vramReadStatus);
    bool adapterIdentityChanged =
        adapterSampleMissing &&
        HasGpuAdapterIdentityChanged(*adapter, settings.gpuAdapter);
    if (!adapterSampleMissing) {
        // A healthy matching sample should make the next future mismatch probe
        // immediately instead of inheriting an old parked-GPU backoff window.
        g_unchangedGpuIdentityChecks = 0;
        g_nextGpuIdentityCheck = {};
    }
    if (hardReadFailure) {
        RecordPdhReadFailure(L"counter read");
    } else if (adapterIdentityChanged) {
        RecoverFromGpuAdapterIdentityChange();
    } else {
        RecordPdhReadSuccess();
    }
    return true;
}

std::optional<MetricsSnapshot> CollectMetrics(const ModSettings& settings) {
    MetricsSnapshot snapshot;
    if (auto cpu = ReadCpuUsage()) {
        snapshot.cpu = *cpu;
        snapshot.cpuAvailable = true;
    }
    ReadMemory(snapshot);
    if (!ReadPdhMetrics(snapshot, settings)) {
        return std::nullopt;
    }
    ReadTemperatures(snapshot, settings);
    return snapshot;
}

PCWSTR TemperatureProviderName(TemperatureProvider provider) {
    switch (provider) {
        case TemperatureProvider::HwInfoSharedMemory:
            return L"HWiNFO Shared Memory";
        case TemperatureProvider::HwInfoGadgetRegistry:
            return L"HWiNFO Gadget Registry";
        case TemperatureProvider::WindowsD3dkmt:
            return L"Windows D3DKMT";
        case TemperatureProvider::WindowsThermalZones:
            return L"Windows thermal zones";
        case TemperatureProvider::None:
        default:
            return L"unavailable";
    }
}

constexpr uint32_t kTemperatureHoldoverSamples = 2;

struct TemperatureHoldover {
    std::optional<double> value;
    TemperatureProvider provider = TemperatureProvider::None;
    std::chrono::steady_clock::time_point capturedAt{};
    uint32_t missedSamples = 0;
};

void ApplyTemperatureHoldover(std::optional<double>& value,
                              TemperatureProvider& provider,
                              TemperatureHoldover& holdover,
                              const ModSettings& settings) {
    auto now = std::chrono::steady_clock::now();
    if (value) {
        holdover.value = value;
        holdover.provider = provider;
        holdover.capturedAt = now;
        holdover.missedSamples = 0;
        return;
    }

    auto maximumAge = std::chrono::seconds(
        settings.updateInterval * kTemperatureHoldoverSamples + 1);
    if (settings.temperatureSource != TemperatureSource::Disabled &&
        holdover.value &&
        holdover.missedSamples < kTemperatureHoldoverSamples &&
        now - holdover.capturedAt <= maximumAge) {
        value = holdover.value;
        provider = holdover.provider;
        holdover.missedSamples++;
        return;
    }

    holdover = {};
}

void PublishMetrics(MetricsSnapshot snapshot) {
    std::lock_guard lock(g_metricsMutex);
    g_latestMetricsSequence++;
    g_publishedMetrics.push_back(
        {g_latestMetricsSequence, std::move(snapshot)});
    while (g_publishedMetrics.size() > kMaximumPublishedMetrics) {
        g_publishedMetrics.pop_front();
    }
}

bool GetMetricsSince(uint64_t afterSequence,
                     MetricsSnapshot& latestSnapshot,
                     uint64_t& latestSequence,
                     std::vector<MetricsSnapshot>& newSnapshots) {
    std::lock_guard lock(g_metricsMutex);
    if (g_publishedMetrics.empty()) {
        return false;
    }
    latestSnapshot = g_publishedMetrics.back().snapshot;
    latestSequence = g_publishedMetrics.back().sequence;
    for (const PublishedMetricsSnapshot& published : g_publishedMetrics) {
        if (published.sequence > afterSequence) {
            newSnapshots.push_back(published.snapshot);
        }
    }
    return true;
}

void MetricsWorkerProc() {
    ReadCpuUsage();
    EnsurePdhQuery(*CurrentSettings());

    bool firstSample = true;
    bool waitFailureLogged = false;
    bool providersLogged = false;
    TemperatureProvider lastCpuProvider = TemperatureProvider::None;
    TemperatureProvider lastGpuProvider = TemperatureProvider::None;
    TemperatureHoldover cpuTemperatureHoldover;
    TemperatureHoldover gpuTemperatureHoldover;
    while (!g_stopMetricsWorker) {
        auto settings = CurrentSettings();
        DWORD waitMilliseconds =
            firstSample
                ? 250
                : static_cast<DWORD>(settings->updateInterval) * 1000;
        DWORD waitResult =
            WaitForSingleObject(g_metricsWorkerWakeEvent, waitMilliseconds);
        firstSample = false;

        if (g_stopMetricsWorker) {
            break;
        }
        if (waitResult == WAIT_FAILED) {
            if (!waitFailureLogged) {
                Wh_Log(L"Metrics worker wait failed: %u", GetLastError());
                waitFailureLogged = true;
            }
            // Keep the monitor alive even if its wake handle becomes invalid.
            // A short backoff prevents a hot loop; timeout-style collection can
            // continue until Explorer recreates the mod.
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else if (waitResult == WAIT_OBJECT_0) {
            // Settings changes wake the worker. Re-prime rate-based metrics and
            // the CPU delta baseline, then wait for a real sampling interval
            // instead of publishing a synthetic near-zero sample.
            settings = CurrentSettings();
            ReadCpuUsage();
            EnsurePdhQuery(*settings);
            cpuTemperatureHoldover = {};
            gpuTemperatureHoldover = {};
            continue;
        } else {
            waitFailureLogged = false;
        }

        settings = CurrentSettings();
        auto snapshot = CollectMetrics(*settings);
        if (!snapshot) {
            continue;
        }
        ApplyTemperatureHoldover(snapshot->cpuTemp,
                                 snapshot->cpuTempProvider,
                                 cpuTemperatureHoldover, *settings);
        ApplyTemperatureHoldover(snapshot->gpuTemp,
                                 snapshot->gpuTempProvider,
                                 gpuTemperatureHoldover, *settings);
        if (!providersLogged ||
            snapshot->cpuTempProvider != lastCpuProvider ||
            snapshot->gpuTempProvider != lastGpuProvider) {
            Wh_Log(L"Temperature providers: CPU=%s, GPU=%s",
                   TemperatureProviderName(snapshot->cpuTempProvider),
                   TemperatureProviderName(snapshot->gpuTempProvider));
            lastCpuProvider = snapshot->cpuTempProvider;
            lastGpuProvider = snapshot->gpuTempProvider;
            providersLogged = true;
        }
        PublishMetrics(std::move(*snapshot));
    }

    CloseMetricSources();
}

bool StartMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    if (g_metricsWorker) {
        return true;
    }
    if (g_unloading) {
        return false;
    }

    g_metricsWorkerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_metricsWorkerWakeEvent) {
        Wh_Log(L"Creating metrics worker event failed: %u", GetLastError());
        return false;
    }

    {
        std::lock_guard metricsLock(g_metricsMutex);
        g_publishedMetrics.clear();
        g_latestMetricsSequence = 0;
    }
    g_stopMetricsWorker = false;
    try {
        g_metricsWorker.emplace(MetricsWorkerProc);
    } catch (...) {
        CloseHandle(g_metricsWorkerWakeEvent);
        g_metricsWorkerWakeEvent = nullptr;
        Wh_Log(L"Starting metrics worker failed");
        return false;
    }
    return true;
}

void WakeMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    if (g_metricsWorkerWakeEvent) {
        SetEvent(g_metricsWorkerWakeEvent);
    }
}

void StopMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    g_stopMetricsWorker = true;
    if (g_metricsWorkerWakeEvent) {
        SetEvent(g_metricsWorkerWakeEvent);
    }
    if (g_metricsWorker) {
        if (g_metricsWorker->joinable()) {
            g_metricsWorker->join();
        }
        g_metricsWorker.reset();
    }
    if (g_metricsWorkerWakeEvent) {
        CloseHandle(g_metricsWorkerWakeEvent);
        g_metricsWorkerWakeEvent = nullptr;
    }

    std::lock_guard metricsLock(g_metricsMutex);
    g_publishedMetrics.clear();
    g_latestMetricsSequence = 0;
}

std::wstring FormatFixed(double value, int decimals) {
    wchar_t buffer[64];
    swprintf(buffer, std::size(buffer), decimals == 0 ? L"%.0f" : L"%.1f",
             value);
    return buffer;
}

std::wstring FormatPercent(double value) {
    wchar_t buffer[64];
    swprintf(buffer, std::size(buffer), L"%.0f%%",
             std::clamp(value, 0.0, 100.0));
    return buffer;
}

std::wstring FormatTemperature(const std::optional<double>& value) {
    return value ? FormatFixed(*value, 0) + L"°C" : L"--°C";
}

std::wstring FormatCapacity(double usedGb, double totalGb, bool available) {
    if (!available || !std::isfinite(usedGb) || !std::isfinite(totalGb) ||
        totalGb <= 0.0) {
        return L"--/--G";
    }
    double roundedTotalGb = std::round(totalGb);
    int totalDecimals =
        totalGb < 1.0 ||
                (totalGb < 4.0 && std::abs(totalGb - roundedTotalGb) >= 0.05)
            ? 1
            : 0;
    return FormatFixed(usedGb, 1) + L"/" +
           FormatFixed(totalGb, totalDecimals) + L"G";
}

enum class AlertLevel { Normal, Warning, Critical };

AlertLevel g_cpuTemperatureAlert = AlertLevel::Normal;
AlertLevel g_gpuTemperatureAlert = AlertLevel::Normal;
AlertLevel g_ramAlert = AlertLevel::Normal;
AlertLevel g_vramAlert = AlertLevel::Normal;

AlertLevel EvaluateAlert(double value,
                         double warning,
                         double critical,
                         AlertLevel previous,
                         double releaseMargin) {
    if (!std::isfinite(value)) {
        return AlertLevel::Normal;
    }
    if (value >= critical ||
        (previous == AlertLevel::Critical &&
         value >= critical - releaseMargin)) {
        return AlertLevel::Critical;
    }
    if (value >= warning ||
        (previous != AlertLevel::Normal && value >= warning - releaseMargin)) {
        return AlertLevel::Warning;
    }
    return AlertLevel::Normal;
}

std::optional<Color> ParseColor(const std::wstring& value) {
    std::wstring hex = value;
    if (!hex.empty() && hex.front() == L'#') {
        hex.erase(hex.begin());
    }
    if (hex.size() != 6 && hex.size() != 8) {
        return std::nullopt;
    }
    if (!std::all_of(hex.begin(), hex.end(), [](wchar_t character) {
            return std::iswxdigit(character) != 0;
        })) {
        return std::nullopt;
    }

    wchar_t* end = nullptr;
    unsigned long parsed = std::wcstoul(hex.c_str(), &end, 16);
    if (!end || *end) {
        return std::nullopt;
    }

    Color color{};
    if (hex.size() == 8) {
        color.A = static_cast<uint8_t>((parsed >> 24) & 0xFF);
    } else {
        color.A = 0xFF;
    }
    color.R = static_cast<uint8_t>((parsed >> 16) & 0xFF);
    color.G = static_cast<uint8_t>((parsed >> 8) & 0xFF);
    color.B = static_cast<uint8_t>(parsed & 0xFF);
    return color;
}

Color MakeColor(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) {
    Color color{};
    color.A = alpha;
    color.R = red;
    color.G = green;
    color.B = blue;
    return color;
}

SolidColorBrush BrushFromSetting(const std::wstring& value, Color fallback) {
    return SolidColorBrush(ParseColor(value).value_or(fallback));
}

ElementTheme ResolveWidgetTheme() {
    if (g_widget) {
        ElementTheme theme = g_widget.ActualTheme();
        if (theme != ElementTheme::Default) {
            return theme;
        }
    }
    Application application = Application::Current();
    return application && application.RequestedTheme() == ApplicationTheme::Light
               ? ElementTheme::Light
               : ElementTheme::Dark;
}

bool SystemColorsChanged() {
    HIGHCONTRASTW highContrast{};
    highContrast.cbSize = sizeof(highContrast);
    bool highContrastEnabled =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast),
                              &highContrast, 0) &&
        (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    return !g_themeBrushesInitialized ||
           highContrastEnabled != g_cachedHighContrast ||
           (highContrastEnabled &&
             (GetSysColor(COLOR_HIGHLIGHT) != g_cachedHighlightColor ||
              GetSysColor(COLOR_HOTLIGHT) != g_cachedHotlightColor ||
              GetSysColor(COLOR_GRAYTEXT) != g_cachedGrayTextColor));
}

Color ColorFromColorRef(COLORREF value) {
    return MakeColor(0xFF, GetRValue(value), GetGValue(value),
                     GetBValue(value));
}

void ApplyCachedBrushesToVisuals() {
    for (XamlPolyline graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Stroke(g_graphBrush);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Fill(g_graphBrush);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Fill(g_graphBrush);
        }
    }
}

void RefreshThemeBrushes(const ModSettings& settings) {
    ElementTheme theme = ResolveWidgetTheme();
    g_cachedWidgetTheme = theme;
    g_themeBrushesInitialized = true;
    HIGHCONTRASTW highContrast{};
    highContrast.cbSize = sizeof(highContrast);
    g_cachedHighContrast =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast),
                              &highContrast, 0) &&
        (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    g_cachedHighlightColor = GetSysColor(COLOR_HIGHLIGHT);
    g_cachedHotlightColor = GetSysColor(COLOR_HOTLIGHT);
    g_cachedGrayTextColor = GetSysColor(COLOR_GRAYTEXT);
    if (settings.adaptiveColors) {
        bool light = theme == ElementTheme::Light;
        g_textBrush = nullptr;
        if (g_cachedHighContrast) {
            g_graphBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedGrayTextColor));
            g_warningBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedHotlightColor));
            g_criticalBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedHighlightColor));
        } else {
            g_graphBrush = BrushFromSetting(
                light ? kLightGraphColor : kDefaultGraphColor,
                MakeColor(0xFF, 0x78, 0xA8, 0xFF));
            g_warningBrush = BrushFromSetting(
                light ? kLightWarningColor : kDefaultWarningColor,
                MakeColor(0xFF, 0xFF, 0xB9, 0x00));
            g_criticalBrush = BrushFromSetting(
                light ? kLightCriticalColor : kDefaultCriticalColor,
                MakeColor(0xFF, 0xFF, 0x6B, 0x6B));
        }
    } else {
        auto textColor = ParseColor(settings.textColor);
        g_textBrush = textColor ? SolidColorBrush(*textColor) : nullptr;
        g_graphBrush = BrushFromSetting(settings.graphColor,
                                        MakeColor(0xFF, 0x78, 0xA8, 0xFF));
        g_warningBrush = BrushFromSetting(settings.warningColor,
                                          MakeColor(0xFF, 0xFF, 0xB9, 0x00));
        g_criticalBrush = BrushFromSetting(
            settings.criticalColor, MakeColor(0xFF, 0xFF, 0x6B, 0x6B));
    }
    ApplyCachedBrushesToVisuals();
}

SolidColorBrush AlertBrush(AlertLevel alert) {
    if (alert == AlertLevel::Critical) {
        return g_criticalBrush;
    }
    if (alert == AlertLevel::Warning) {
        return g_warningBrush;
    }
    return g_graphBrush;
}

void SetTextForeground(TextBlock text, AlertLevel alert) {
    if (!text) {
        return;
    }

    SolidColorBrush brush = alert == AlertLevel::Normal
                                ? g_textBrush
                                : AlertBrush(alert);
    if (brush) {
        text.Foreground(brush);
    } else {
        text.ClearValue(TextBlock::ForegroundProperty());
    }
}

void SetTextIfChanged(TextBlock text, const std::wstring& value) {
    if (!text) {
        return;
    }
    hstring current = text.Text();
    if (current.size() != value.size() ||
        !std::equal(value.begin(), value.end(), current.begin())) {
        text.Text(value);
    }
}

size_t HistoryCapacity(const ModSettings& settings) {
    int intervals =
        (settings.historySeconds + settings.updateInterval - 1) /
        settings.updateInterval;
    return std::max<size_t>(2, static_cast<size_t>(intervals) + 1);
}

void AppendHistory(std::deque<double>& history,
                   double value,
                   size_t capacity) {
    history.push_back(std::clamp(value, 0.0, 100.0));
    while (history.size() > capacity) {
        history.pop_front();
    }
}

void UpdateSparkline(XamlPolyline graph,
                     const std::deque<double>& history,
                     size_t capacity) {
    if (!graph) {
        return;
    }

    auto points = graph.Points();
    points.Clear();
    if (history.size() < 2 || capacity < 2 || g_graphWidth <= 1.0) {
        graph.Visibility(Visibility::Collapsed);
        return;
    }

    constexpr double verticalPadding = 1.0;
    double usableHeight = kGraphHeight - verticalPadding * 2.0;
    double step = g_graphWidth / static_cast<double>(capacity - 1);
    double firstX =
        g_graphWidth - step * static_cast<double>(history.size() - 1);
    for (size_t i = 0; i < history.size(); i++) {
        double x = firstX + step * static_cast<double>(i);
        double y = verticalPadding +
                   (100.0 - std::clamp(history[i], 0.0, 100.0)) / 100.0 *
                       usableHeight;
        points.Append(Point{static_cast<float>(x), static_cast<float>(y)});
    }
    graph.Visibility(Visibility::Visible);
}

void UpdateMemoryBar(XamlRectangle fill,
                     double percent,
                     bool available,
                     AlertLevel alert) {
    if (!fill) {
        return;
    }
    fill.Width(available ? g_memoryBarWidth *
                               std::clamp(percent, 0.0, 100.0) / 100.0
                         : 0.0);
    fill.Fill(AlertBrush(alert));
}

template <typename F>
FrameworkElement FindChildRecursive(FrameworkElement element,
                                    F callback,
                                    int depth = 16) {
    if (!element || depth <= 0) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (callback(child)) {
            return child;
        }
        if (auto nested = FindChildRecursive(child, callback, depth - 1)) {
            return nested;
        }
    }
    return nullptr;
}

FrameworkElement FindDirectChildByName(FrameworkElement parent, PCWSTR name) {
    if (!parent) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (child && child.Name() == name) {
            return child;
        }
    }
    return nullptr;
}

void ApplyTextStyle(TextBlock text,
                    bool label,
                    const ModSettings& settings) {
    if (!text) {
        return;
    }
    text.FontFamily(Media::FontFamily(settings.fontFamily));
    text.FontSize(settings.fontSize);
    text.FontWeight(label ? Text::FontWeights::SemiBold()
                          : Text::FontWeights::Normal());
    double opacity = static_cast<double>(settings.textOpacity) / 100.0;
    text.Opacity(label ? opacity * 0.62 : opacity);
    text.TextWrapping(TextWrapping::NoWrap);
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    SetTextForeground(text, AlertLevel::Normal);
}

void ApplyWidgetGeometry(const ModSettings& settings) {
    if (!g_widget) {
        return;
    }

    double rightWidth =
        std::clamp(static_cast<double>(settings.width) * 0.38, 145.0, 170.0);
    double leftWidth = settings.width - kColumnGap - rightWidth;
    g_graphWidth = std::max(
        24.0, leftWidth - kMetricLabelWidth - kMetricUsageWidth -
                  kMetricTempWidth - kGraphLeftGap);
    g_memoryBarWidth = rightWidth;

    g_widget.Width(settings.width);
    g_widget.Height(kWidgetHeight);
    if (g_leftColumn) {
        g_leftColumn.Width(GridLength{leftWidth, GridUnitType::Pixel});
    }
    if (g_gapColumn) {
        g_gapColumn.Width(GridLength{kColumnGap, GridUnitType::Pixel});
    }
    if (g_rightColumn) {
        g_rightColumn.Width(GridLength{rightWidth, GridUnitType::Pixel});
    }
    for (XamlPolyline graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Width(g_graphWidth);
            graph.Height(kGraphHeight);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Width(g_memoryBarWidth);
        }
    }
}

void ApplyReservedSpace(const ModSettings& settings) {
    if (!g_taskItemsRepeater) {
        return;
    }

    Thickness margin = g_taskItemsRepeater.Margin();
    double baseLeft = margin.Left;
    if (g_lastAppliedRepeaterMarginLeft &&
        std::abs(margin.Left - *g_lastAppliedRepeaterMarginLeft) < 0.01) {
        baseLeft -= g_reservedMargin;
    } else if (g_reservedMargin != 0.0) {
        Wh_Log(L"Taskbar repeater margin changed externally; adopting it as "
               L"the new base");
    }
    g_reservedMargin = settings.reserveSpace
                           ? settings.leftOffset + settings.width +
                                 settings.reserveGap
                           : 0.0;
    margin.Left = baseLeft + g_reservedMargin;
    g_taskItemsRepeater.Margin(margin);
    if (g_reservedMargin != 0.0) {
        g_lastAppliedRepeaterMarginLeft = margin.Left;
    } else {
        g_lastAppliedRepeaterMarginLeft.reset();
    }
}

void ApplyWidgetSettings() {
    if (!g_widget) {
        return;
    }
    auto settingsSnapshot = CurrentSettings();
    const ModSettings& settings = *settingsSnapshot;
    RefreshThemeBrushes(settings);
    if (g_historyInterval != settings.updateInterval ||
        g_historyWindow != settings.historySeconds) {
        g_cpuHistory.clear();
        g_gpuHistory.clear();
        g_historyInterval = settings.updateInterval;
        g_historyWindow = settings.historySeconds;
    }
    ApplyWidgetGeometry(settings);
    g_widget.Margin(
        Thickness{static_cast<double>(settings.leftOffset), 0, 0, 0});
    g_widget.HorizontalAlignment(HorizontalAlignment::Left);
    g_widget.VerticalAlignment(VerticalAlignment::Center);
    g_widget.IsHitTestVisible(false);

    for (TextBlock label :
         {g_cpuLabel, g_gpuLabel, g_ramLabel, g_vramLabel}) {
        ApplyTextStyle(label, true, settings);
    }
    for (TextBlock value : {g_cpuUsageText, g_cpuTempText, g_gpuUsageText,
                            g_gpuTempText, g_ramPercentText,
                            g_ramCapacityText, g_vramPercentText,
                            g_vramCapacityText}) {
        ApplyTextStyle(value, false, settings);
    }

    for (XamlPolyline graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Stroke(g_graphBrush);
            graph.StrokeThickness(1.25);
            graph.StrokeStartLineCap(PenLineCap::Round);
            graph.StrokeEndLineCap(PenLineCap::Round);
            graph.StrokeLineJoin(PenLineJoin::Round);
            graph.Opacity(0.78);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Fill(g_graphBrush);
            track.Opacity(0.18);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Fill(g_graphBrush);
            fill.Opacity(0.76);
        }
    }

    size_t capacity = HistoryCapacity(settings);
    while (g_cpuHistory.size() > capacity) {
        g_cpuHistory.pop_front();
    }
    while (g_gpuHistory.size() > capacity) {
        g_gpuHistory.pop_front();
    }
    UpdateSparkline(g_cpuGraph, g_cpuHistory, capacity);
    UpdateSparkline(g_gpuGraph, g_gpuHistory, capacity);
    ApplyReservedSpace(settings);

    if (g_timer) {
        g_timer.Interval(std::chrono::milliseconds(250));
    }
}

void UpdateWidgetText(bool force = false) {
    if (!g_widget || g_unloading) {
        return;
    }
    MetricsSnapshot snapshot;
    uint64_t metricsSequence = 0;
    std::vector<MetricsSnapshot> newSnapshots;
    if (!GetMetricsSince(g_lastRenderedMetricsSequence, snapshot,
                         metricsSequence, newSnapshots)) {
        return;
    }
    bool hasNewSample = !newSnapshots.empty();
    if (!force && !hasNewSample) {
        return;
    }
    auto settingsSnapshot = CurrentSettings();
    const ModSettings& settings = *settingsSnapshot;

    g_cpuTemperatureAlert = snapshot.cpuTemp
                                ? EvaluateAlert(*snapshot.cpuTemp,
                                                settings.cpuWarningTemp,
                                                settings.cpuCriticalTemp,
                                                g_cpuTemperatureAlert, 3.0)
                                : AlertLevel::Normal;
    g_gpuTemperatureAlert = snapshot.gpuTemp
                                ? EvaluateAlert(*snapshot.gpuTemp,
                                                settings.gpuWarningTemp,
                                                settings.gpuCriticalTemp,
                                                g_gpuTemperatureAlert, 3.0)
                                : AlertLevel::Normal;
    g_ramAlert = snapshot.ramAvailable
                     ? EvaluateAlert(snapshot.ram,
                                     settings.memoryWarningPercent,
                                     settings.memoryCriticalPercent, g_ramAlert,
                                     3.0)
                     : AlertLevel::Normal;
    g_vramAlert =
        snapshot.vramAvailable
            ? EvaluateAlert(snapshot.vram, settings.memoryWarningPercent,
                            settings.memoryCriticalPercent, g_vramAlert, 3.0)
            : AlertLevel::Normal;

    SetTextIfChanged(g_cpuUsageText,
                     snapshot.cpuAvailable ? FormatPercent(snapshot.cpu)
                                           : L"--%");
    if (g_cpuTempText) {
        SetTextIfChanged(g_cpuTempText, FormatTemperature(snapshot.cpuTemp));
        SetTextForeground(g_cpuTempText, g_cpuTemperatureAlert);
    }
    SetTextIfChanged(g_gpuUsageText, snapshot.gpuAvailable
                                         ? FormatPercent(snapshot.gpu)
                                         : L"--%");
    if (g_gpuTempText) {
        SetTextIfChanged(g_gpuTempText, FormatTemperature(snapshot.gpuTemp));
        SetTextForeground(g_gpuTempText, g_gpuTemperatureAlert);
    }
    if (g_ramPercentText) {
        SetTextIfChanged(g_ramPercentText,
                         snapshot.ramAvailable ? FormatPercent(snapshot.ram)
                                               : L"--%");
        SetTextForeground(g_ramPercentText, g_ramAlert);
    }
    SetTextIfChanged(g_ramCapacityText,
                     FormatCapacity(snapshot.ramUsedGb, snapshot.ramTotalGb,
                                    snapshot.ramAvailable));
    if (g_vramPercentText) {
        SetTextIfChanged(g_vramPercentText, snapshot.vramAvailable
                                                ? FormatPercent(snapshot.vram)
                                                : L"--%");
        SetTextForeground(g_vramPercentText, g_vramAlert);
    }
    SetTextIfChanged(g_vramCapacityText,
                     FormatCapacity(snapshot.vramUsedGb, snapshot.vramTotalGb,
                                    snapshot.vramAvailable));

    if (hasNewSample) {
        size_t historyCapacity = HistoryCapacity(settings);
        for (const MetricsSnapshot& newSnapshot : newSnapshots) {
            if (newSnapshot.cpuAvailable) {
                AppendHistory(g_cpuHistory, newSnapshot.cpu, historyCapacity);
            }
            if (newSnapshot.gpuAvailable) {
                AppendHistory(g_gpuHistory, newSnapshot.gpu, historyCapacity);
            }
        }
        UpdateSparkline(g_cpuGraph, g_cpuHistory, historyCapacity);
        UpdateSparkline(g_gpuGraph, g_gpuHistory, historyCapacity);
        g_lastRenderedMetricsSequence = metricsSequence;
    }
    UpdateMemoryBar(g_ramFill, snapshot.ram, snapshot.ramAvailable, g_ramAlert);
    UpdateMemoryBar(g_vramFill, snapshot.vram, snapshot.vramAvailable,
                    g_vramAlert);
}

void EnsureConfiguredTaskbarPlacement();

void RefreshWidgetTheme() {
    if (!g_widget || g_unloading) {
        return;
    }
    RefreshThemeBrushes(*CurrentSettings());
    UpdateWidgetText(true);
}

void EnsureTimer() {
    if (g_timer) {
        return;
    }
    g_timer = DispatcherTimer();
    g_timer.Interval(std::chrono::milliseconds(250));
    auto now = std::chrono::steady_clock::now();
    g_nextSystemColorCheck = now + std::chrono::seconds(1);
    g_nextTaskbarPlacementCheck = now + std::chrono::seconds(1);
    g_timerToken = g_timer.Tick([](IInspectable const&, IInspectable const&) {
        try {
            bool force = false;
            auto now = std::chrono::steady_clock::now();
            if (now >= g_nextSystemColorCheck) {
                g_nextSystemColorCheck = now + std::chrono::seconds(1);
                if (SystemColorsChanged()) {
                    RefreshThemeBrushes(*CurrentSettings());
                    force = true;
                }
            }
            if (now >= g_nextTaskbarPlacementCheck) {
                g_nextTaskbarPlacementCheck = now + std::chrono::seconds(1);
                EnsureConfiguredTaskbarPlacement();
            }
            UpdateWidgetText(force);
        } catch (...) {
            HRESULT error = winrt::to_hresult();
            Wh_Log(L"Metrics update failed: %08X",
                   static_cast<unsigned>(error));
        }
    });
    g_timer.Start();
}

ColumnDefinition PixelColumn(double width) {
    ColumnDefinition column;
    column.Width(GridLength{width, GridUnitType::Pixel});
    return column;
}

RowDefinition PixelRow(double height) {
    RowDefinition row;
    row.Height(GridLength{height, GridUnitType::Pixel});
    return row;
}

TextBlock CreateCellText(PCWSTR name, TextAlignment alignment) {
    TextBlock text;
    text.Name(name);
    text.HorizontalAlignment(HorizontalAlignment::Stretch);
    text.VerticalAlignment(VerticalAlignment::Center);
    text.TextAlignment(alignment);
    text.TextWrapping(TextWrapping::NoWrap);
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    text.IsHitTestVisible(false);
    return text;
}

Grid CreateComputeRow(PCWSTR label,
                      PCWSTR prefix,
                      TextBlock& labelText,
                      TextBlock& usageText,
                      TextBlock& temperatureText,
                      XamlPolyline& graph) {
    Grid row;
    row.Height(kRowHeight);
    row.IsHitTestVisible(false);

    row.ColumnDefinitions().Append(PixelColumn(kMetricLabelWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMetricUsageWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMetricTempWidth));
    ColumnDefinition graphColumn;
    graphColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(graphColumn);

    std::wstring labelName = std::wstring(prefix) + L"Label";
    labelText = CreateCellText(labelName.c_str(), TextAlignment::Left);
    labelText.Text(label);

    std::wstring usageName = std::wstring(prefix) + L"Usage";
    usageText = CreateCellText(usageName.c_str(), TextAlignment::Right);
    usageText.Text(L"--%");

    std::wstring temperatureName = std::wstring(prefix) + L"Temperature";
    temperatureText =
        CreateCellText(temperatureName.c_str(), TextAlignment::Right);
    temperatureText.Text(L"--°C");

    graph = XamlPolyline();
    graph.Name((std::wstring(prefix) + L"History").c_str());
    graph.HorizontalAlignment(HorizontalAlignment::Left);
    graph.VerticalAlignment(VerticalAlignment::Center);
    graph.Margin(Thickness{kGraphLeftGap, 0, 0, 0});
    graph.Stretch(Stretch::None);
    graph.IsHitTestVisible(false);

    Grid::SetColumn(labelText, 0);
    Grid::SetColumn(usageText, 1);
    Grid::SetColumn(temperatureText, 2);
    Grid::SetColumn(graph, 3);
    row.Children().Append(labelText);
    row.Children().Append(usageText);
    row.Children().Append(temperatureText);
    row.Children().Append(graph);
    return row;
}

Grid CreateMemoryRow(PCWSTR label,
                     PCWSTR prefix,
                     TextBlock& labelText,
                     TextBlock& percentText,
                     TextBlock& capacityText,
                     XamlRectangle& track,
                     XamlRectangle& fill) {
    Grid row;
    row.Height(kRowHeight);
    row.IsHitTestVisible(false);

    row.ColumnDefinitions().Append(PixelColumn(kMemoryLabelWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMemoryPercentWidth));
    ColumnDefinition capacityColumn;
    capacityColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(capacityColumn);

    track = XamlRectangle();
    track.Name((std::wstring(prefix) + L"Track").c_str());
    track.Height(1.25);
    track.HorizontalAlignment(HorizontalAlignment::Left);
    track.VerticalAlignment(VerticalAlignment::Bottom);
    track.RadiusX(0.625);
    track.RadiusY(0.625);
    track.IsHitTestVisible(false);

    fill = XamlRectangle();
    fill.Name((std::wstring(prefix) + L"Fill").c_str());
    fill.Height(1.25);
    fill.HorizontalAlignment(HorizontalAlignment::Left);
    fill.VerticalAlignment(VerticalAlignment::Bottom);
    fill.RadiusX(0.625);
    fill.RadiusY(0.625);
    fill.IsHitTestVisible(false);

    std::wstring labelName = std::wstring(prefix) + L"Label";
    labelText = CreateCellText(labelName.c_str(), TextAlignment::Left);
    labelText.Text(label);

    std::wstring percentName = std::wstring(prefix) + L"Percent";
    percentText = CreateCellText(percentName.c_str(), TextAlignment::Right);
    percentText.Text(L"--%");

    std::wstring capacityName = std::wstring(prefix) + L"Capacity";
    capacityText = CreateCellText(capacityName.c_str(), TextAlignment::Right);
    capacityText.Text(L"--/--G");

    Grid::SetColumnSpan(track, 3);
    Grid::SetColumnSpan(fill, 3);
    Grid::SetColumn(labelText, 0);
    Grid::SetColumn(percentText, 1);
    Grid::SetColumn(capacityText, 2);
    row.Children().Append(track);
    row.Children().Append(fill);
    row.Children().Append(labelText);
    row.Children().Append(percentText);
    row.Children().Append(capacityText);
    return row;
}

void RemoveWidget() {
    if (g_timer) {
        g_timer.Stop();
        g_timer.Tick(g_timerToken);
        g_timer = nullptr;
        g_timerToken = {};
    }
    g_nextSystemColorCheck = {};
    g_nextTaskbarPlacementCheck = {};

    if (g_widget && g_actualThemeChangedToken.value) {
        try {
            g_widget.ActualThemeChanged(g_actualThemeChangedToken);
        } catch (...) {
            HRESULT error = winrt::to_hresult();
            Wh_Log(L"Removing taskbar theme handler failed: %08X",
                   static_cast<unsigned>(error));
        }
    }
    g_actualThemeChangedToken = {};

    if (g_taskItemsRepeater && g_reservedMargin != 0.0) {
        Thickness margin = g_taskItemsRepeater.Margin();
        if (g_lastAppliedRepeaterMarginLeft &&
            std::abs(margin.Left - *g_lastAppliedRepeaterMarginLeft) < 0.01) {
            margin.Left -= g_reservedMargin;
            g_taskItemsRepeater.Margin(margin);
        } else {
            Wh_Log(L"Taskbar repeater margin changed externally; leaving the "
                   L"external value intact");
        }
    }
    g_reservedMargin = 0.0;
    g_lastAppliedRepeaterMarginLeft.reset();

    if (g_rootGrid && g_widget) {
        uint32_t index = 0;
        if (g_rootGrid.Children().IndexOf(g_widget, index)) {
            g_rootGrid.Children().RemoveAt(index);
        }
    }

    g_widget = nullptr;
    g_rootGrid = nullptr;
    g_taskItemsRepeater = nullptr;
    g_cpuLabel = nullptr;
    g_cpuUsageText = nullptr;
    g_cpuTempText = nullptr;
    g_gpuLabel = nullptr;
    g_gpuUsageText = nullptr;
    g_gpuTempText = nullptr;
    g_ramLabel = nullptr;
    g_ramPercentText = nullptr;
    g_ramCapacityText = nullptr;
    g_vramLabel = nullptr;
    g_vramPercentText = nullptr;
    g_vramCapacityText = nullptr;
    g_cpuGraph = nullptr;
    g_gpuGraph = nullptr;
    g_ramTrack = nullptr;
    g_ramFill = nullptr;
    g_vramTrack = nullptr;
    g_vramFill = nullptr;
    g_leftColumn = nullptr;
    g_gapColumn = nullptr;
    g_rightColumn = nullptr;
    g_textBrush = nullptr;
    g_graphBrush = nullptr;
    g_warningBrush = nullptr;
    g_criticalBrush = nullptr;
    g_cachedWidgetTheme = ElementTheme::Default;
    g_themeBrushesInitialized = false;
    g_cachedHighContrast = false;
    g_cachedHighlightColor = CLR_INVALID;
    g_cachedHotlightColor = CLR_INVALID;
    g_cachedGrayTextColor = CLR_INVALID;
    g_cpuHistory.clear();
    g_gpuHistory.clear();
    g_lastRenderedMetricsSequence = 0;
    g_cpuTemperatureAlert = AlertLevel::Normal;
    g_gpuTemperatureAlert = AlertLevel::Normal;
    g_ramAlert = AlertLevel::Normal;
    g_vramAlert = AlertLevel::Normal;
}

bool InjectWidget(FrameworkElement taskbarFrame) {
    if (!taskbarFrame || g_unloading) {
        return false;
    }

    auto root = FindDirectChildByName(taskbarFrame, L"RootGrid").try_as<Grid>();
    if (!root) {
        Wh_Log(L"Taskbar RootGrid not found");
        return false;
    }

    auto children = root.Children();
    for (uint32_t index = 0; index < children.Size();) {
        auto element = children.GetAt(index).try_as<FrameworkElement>();
        if (!element || element.Name() != kWidgetName) {
            index++;
            continue;
        }

        uint32_t currentWidgetIndex = 0;
        if (g_widget && children.IndexOf(g_widget, currentWidgetIndex) &&
            currentWidgetIndex == index) {
            ApplyWidgetSettings();
            if (!StartMetricsWorker()) {
                Wh_Log(L"Metrics worker unavailable");
            }
            EnsureTimer();
            UpdateWidgetText(true);
            return true;
        }

        Wh_Log(L"Removing stale Taskbar System Info widget");
        children.RemoveAt(index);
    }

    RemoveWidget();

    Grid widget;
    widget.Name(kWidgetName);
    widget.IsHitTestVisible(false);
    Canvas::SetZIndex(widget, 10000);
    Grid::SetColumn(widget, 0);
    Grid::SetColumnSpan(widget,
                        std::max(1, static_cast<int>(root.ColumnDefinitions().Size())));

    g_leftColumn = ColumnDefinition();
    g_gapColumn = ColumnDefinition();
    g_rightColumn = ColumnDefinition();
    widget.ColumnDefinitions().Append(g_leftColumn);
    widget.ColumnDefinitions().Append(g_gapColumn);
    widget.ColumnDefinitions().Append(g_rightColumn);

    Grid leftPanel;
    leftPanel.IsHitTestVisible(false);
    leftPanel.RowDefinitions().Append(PixelRow(kRowHeight));
    leftPanel.RowDefinitions().Append(PixelRow(kRowGap));
    leftPanel.RowDefinitions().Append(PixelRow(kRowHeight));

    Grid cpuRow = CreateComputeRow(L"CPU", L"Cpu", g_cpuLabel,
                                   g_cpuUsageText, g_cpuTempText, g_cpuGraph);
    Grid gpuRow = CreateComputeRow(L"GPU", L"Gpu", g_gpuLabel,
                                   g_gpuUsageText, g_gpuTempText, g_gpuGraph);
    Grid::SetRow(cpuRow, 0);
    Grid::SetRow(gpuRow, 2);
    leftPanel.Children().Append(cpuRow);
    leftPanel.Children().Append(gpuRow);

    Grid rightPanel;
    rightPanel.IsHitTestVisible(false);
    rightPanel.RowDefinitions().Append(PixelRow(kRowHeight));
    rightPanel.RowDefinitions().Append(PixelRow(kRowGap));
    rightPanel.RowDefinitions().Append(PixelRow(kRowHeight));

    Grid ramRow = CreateMemoryRow(L"RAM", L"Ram", g_ramLabel,
                                  g_ramPercentText, g_ramCapacityText,
                                  g_ramTrack, g_ramFill);
    Grid vramRow = CreateMemoryRow(L"VRAM", L"Vram", g_vramLabel,
                                   g_vramPercentText, g_vramCapacityText,
                                   g_vramTrack, g_vramFill);
    Grid::SetRow(ramRow, 0);
    Grid::SetRow(vramRow, 2);
    rightPanel.Children().Append(ramRow);
    rightPanel.Children().Append(vramRow);

    Grid::SetColumn(leftPanel, 0);
    Grid::SetColumn(rightPanel, 2);
    widget.Children().Append(leftPanel);
    widget.Children().Append(rightPanel);
    root.Children().Append(widget);

    g_rootGrid = root;
    g_widget = widget;
    g_actualThemeChangedToken = g_widget.ActualThemeChanged(
        [](auto const&, auto const&) {
            try {
                RefreshWidgetTheme();
            } catch (...) {
                HRESULT error = winrt::to_hresult();
                Wh_Log(L"Taskbar theme update failed: %08X",
                       static_cast<unsigned>(error));
            }
        });
    g_taskItemsRepeater =
        FindDirectChildByName(root, L"TaskbarFrameRepeater");
    g_reservedMargin = 0.0;
    g_lastAppliedRepeaterMarginLeft.reset();

    ApplyWidgetSettings();
    if (!StartMetricsWorker()) {
        Wh_Log(L"Metrics worker unavailable");
    }
    EnsureTimer();
    UpdateWidgetText();
    Wh_Log(L"Taskbar System Info injected");
    return true;
}

using RunFromWindowThreadProc = void (*)(void*);

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc callback,
                         void* context) {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct CallbackContext {
        RunFromWindowThreadProc callback;
        void* context;
        bool invoked;
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        callback(context);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* messageData =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (messageData->message == message) {
                    auto* callbackContext =
                        reinterpret_cast<CallbackContext*>(messageData->lParam);
                    callbackContext->callback(callbackContext->context);
                    callbackContext->invoked = true;
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed for taskbar thread %u: %u", threadId,
               GetLastError());
        return false;
    }

    CallbackContext callbackContext{callback, context, false};
    SendMessageW(window, message, 0,
                 reinterpret_cast<LPARAM>(&callbackContext));
    UnhookWindowsHookEx(hook);
    if (!callbackContext.invoked) {
        Wh_Log(L"Taskbar thread dispatch failed for thread %u", threadId);
    }
    return callbackContext.invoked;
}

bool IsTaskbarWindowClass(HWND window, bool* secondary = nullptr) {
    WCHAR className[64];
    if (!window ||
        !GetClassNameW(window, className, std::size(className))) {
        return false;
    }

    bool isPrimary = _wcsicmp(className, L"Shell_TrayWnd") == 0;
    bool isSecondary =
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
    if (secondary) {
        *secondary = isSecondary;
    }
    return isPrimary || isSecondary;
}

bool IsCurrentProcessTaskbarWindow(HWND window) {
    DWORD processId = 0;
    return window && GetWindowThreadProcessId(window, &processId) != 0 &&
           processId == GetCurrentProcessId() &&
           IsTaskbarWindowClass(window);
}

struct DisplayMonitor {
    HMONITOR handle = nullptr;
    RECT bounds{};
    bool primary = false;
};

std::vector<DisplayMonitor> EnumerateDisplayMonitors() {
    std::vector<DisplayMonitor> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM context) -> BOOL {
            MONITORINFO info{};
            info.cbSize = sizeof(info);
            if (GetMonitorInfoW(monitor, &info)) {
                reinterpret_cast<std::vector<DisplayMonitor>*>(context)
                    ->push_back({monitor, info.rcMonitor,
                                 (info.dwFlags & MONITORINFOF_PRIMARY) != 0});
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));

    std::stable_sort(
        monitors.begin(), monitors.end(),
        [](const DisplayMonitor& left, const DisplayMonitor& right) {
            if (left.primary != right.primary) {
                return left.primary;
            }
            if (left.bounds.left != right.bounds.left) {
                return left.bounds.left < right.bounds.left;
            }
            if (left.bounds.top != right.bounds.top) {
                return left.bounds.top < right.bounds.top;
            }
            if (left.bounds.right != right.bounds.right) {
                return left.bounds.right < right.bounds.right;
            }
            if (left.bounds.bottom != right.bounds.bottom) {
                return left.bounds.bottom < right.bounds.bottom;
            }
            return reinterpret_cast<uintptr_t>(left.handle) <
                   reinterpret_cast<uintptr_t>(right.handle);
        });
    return monitors;
}

HWND FindTaskbarWindowForMonitor(HMONITOR monitor) {
    struct SearchContext {
        HMONITOR monitor;
        HWND result;
    } context{monitor, nullptr};

    EnumWindows(
        [](HWND window, LPARAM contextValue) -> BOOL {
            auto* context = reinterpret_cast<SearchContext*>(contextValue);
            if (IsCurrentProcessTaskbarWindow(window) &&
                MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) ==
                    context->monitor) {
                context->result = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.result;
}

HWND FindPrimaryTaskbarWindow() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM context) -> BOOL {
            DWORD processId = 0;
            WCHAR className[64];
            if (GetWindowThreadProcessId(window, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassNameW(window, className, std::size(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(context) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

HWND FindOnlyTaskbarWindow() {
    struct SearchContext {
        HWND window = nullptr;
        uint32_t count = 0;
    } context;
    EnumWindows(
        [](HWND window, LPARAM contextValue) -> BOOL {
            auto* context = reinterpret_cast<SearchContext*>(contextValue);
            if (!IsCurrentProcessTaskbarWindow(window)) {
                return TRUE;
            }
            context->window = window;
            context->count++;
            return context->count < 2;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.count == 1 ? context.window : nullptr;
}

HWND FindConfiguredTaskbarWindow(bool logFallback = true) {
    int monitorNumber = CurrentSettings()->monitor;
    auto monitors = EnumerateDisplayMonitors();
    if (monitorNumber <= static_cast<int>(monitors.size())) {
        if (HWND window =
                FindTaskbarWindowForMonitor(monitors[monitorNumber - 1].handle)) {
            return window;
        }
        if (logFallback) {
            Wh_Log(L"Monitor %d has no taskbar; using the primary taskbar",
                   monitorNumber);
        }
    } else {
        if (logFallback) {
            Wh_Log(L"Monitor %d is unavailable; using the primary taskbar",
                   monitorNumber);
        }
    }
    return FindPrimaryTaskbarWindow();
}

void RememberTaskbarWindow(HWND window) {
    if (!IsCurrentProcessTaskbarWindow(window)) {
        return;
    }
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (threadId) {
        g_taskbarWindow = window;
        g_taskbarThreadId = threadId;
    }
}

HWND FindRememberedTaskbarWindow() {
    HWND rememberedWindow = g_taskbarWindow.load();
    if (IsCurrentProcessTaskbarWindow(rememberedWindow)) {
        return rememberedWindow;
    }

    DWORD rememberedThreadId = g_taskbarThreadId.load();
    if (rememberedThreadId) {
        HWND threadWindow = nullptr;
        EnumThreadWindows(
            rememberedThreadId,
            [](HWND window, LPARAM context) -> BOOL {
                if (IsCurrentProcessTaskbarWindow(window)) {
                    *reinterpret_cast<HWND*>(context) = window;
                    return FALSE;
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&threadWindow));
        if (IsCurrentProcessTaskbarWindow(threadWindow)) {
            RememberTaskbarWindow(threadWindow);
            return threadWindow;
        }
    }
    return nullptr;
}

HWND FindAnyWindowOnTaskbarThread(HWND excludedWindow) {
    DWORD threadId = g_taskbarThreadId.load();
    if (!threadId) {
        return nullptr;
    }

    struct SearchContext {
        HWND excludedWindow;
        HWND result;
    } context{excludedWindow, nullptr};
    EnumThreadWindows(
        threadId,
        [](HWND window, LPARAM contextValue) -> BOOL {
            auto* context = reinterpret_cast<SearchContext*>(contextValue);
            DWORD processId = 0;
            if (window != context->excludedWindow &&
                GetWindowThreadProcessId(window, &processId) != 0 &&
                processId == GetCurrentProcessId()) {
                context->result = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.result;
}

using CTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CSecondaryTaskBand_GetTaskbarHost_t
    CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using RefCountBase_Decref_t = void(WINAPI*)(void* pThis);
RefCountBase_Decref_t RefCountBase_Decref_Original = nullptr;

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

bool IsReadableMemoryRange(const void* address, size_t size) {
    if (!address || size == 0) {
        return false;
    }

    MEMORY_BASIC_INFORMATION memory{};
    if (!VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }

    uintptr_t start = reinterpret_cast<uintptr_t>(address);
    uintptr_t regionStart =
        reinterpret_cast<uintptr_t>(memory.BaseAddress);
    uintptr_t regionEnd = regionStart + memory.RegionSize;
    return start >= regionStart && start <= regionEnd &&
           size <= regionEnd - start;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWindow) {
    bool isSecondary = false;
    if (!IsCurrentProcessTaskbarWindow(taskbarWindow) ||
        !IsTaskbarWindowClass(taskbarWindow, &isSecondary) ||
        !TaskbarHost_FrameHeight_Original || !RefCountBase_Decref_Original) {
        return nullptr;
    }

    auto getTaskbarHost = isSecondary
                              ? CSecondaryTaskBand_GetTaskbarHost_Original
                              : CTaskBand_GetTaskbarHost_Original;
    void* expectedVftable =
        isSecondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                    : CTaskBand_ITaskListWndSite_vftable;
    if (!getTaskbarHost || !expectedVftable) {
        Wh_Log(L"%s taskbar symbols unavailable",
               isSecondary ? L"Secondary" : L"Primary");
        return nullptr;
    }

    HWND taskBandWindow = isSecondary
                              ? FindWindowExW(taskbarWindow, nullptr, L"WorkerW",
                                              nullptr)
                              : reinterpret_cast<HWND>(GetPropW(
                                    taskbarWindow, L"TaskbandHWND"));
    if (!taskBandWindow) {
        Wh_Log(L"%s taskband host window not found",
               isSecondary ? L"Secondary" : L"Primary");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(
        GetWindowLongPtrW(taskBandWindow, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0;; i++) {
        if (!IsReadableMemoryRange(taskBandForSite, sizeof(void*))) {
            Wh_Log(L"Taskband site scan reached unreadable memory");
            return nullptr;
        }
        if (*reinterpret_cast<void**>(taskBandForSite) == expectedVftable) {
            break;
        }
        if (i == 20) {
            Wh_Log(L"Taskband site vftable not found");
            return nullptr;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        if (taskbarHostSharedPtr[1]) {
            RefCountBase_Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    size_t elementOffset = 0;
#if defined(_M_X64)
    const BYTE* code =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (IsReadableMemoryRange(code, 8) && code[0] == 0x48 &&
        code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        elementOffset = code[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        RefCountBase_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* code =
        reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (IsReadableMemoryRange(code, sizeof(DWORD) * 4) &&
        code[0] == 0xD503237F &&
        (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD &&
        (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (code[3] >> 12) & 0xFF;
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        RefCountBase_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* elementAddress =
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset;
    if (!IsReadableMemoryRange(elementAddress, sizeof(::IUnknown*))) {
        Wh_Log(L"Taskbar XAML element address is unreadable");
        RefCountBase_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    auto* elementUnknown =
        *reinterpret_cast<::IUnknown**>(elementAddress);
    if (!elementUnknown) {
        RefCountBase_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    elementUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                   winrt::put_abi(taskbarElement));
    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    RefCountBase_Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

struct ApplyWidgetContext {
    HWND taskbarWindow = nullptr;
    bool succeeded = false;
};

void ApplyToTaskbarWindow(void* contextValue) {
    auto* context = reinterpret_cast<ApplyWidgetContext*>(contextValue);
    HWND taskbarWindow = context->taskbarWindow;
    if (!IsCurrentProcessTaskbarWindow(taskbarWindow)) {
        return;
    }

    try {
        XamlRoot xamlRoot = GetTaskbarXamlRoot(taskbarWindow);
        if (!xamlRoot) {
            Wh_Log(L"GetTaskbarXamlRoot failed");
            return;
        }
        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        auto taskbarFrame = FindChildRecursive(content, [](FrameworkElement child) {
            return winrt::get_class_name(child) == L"Taskbar.TaskbarFrame";
        });
        if (taskbarFrame && InjectWidget(taskbarFrame)) {
            RememberTaskbarWindow(taskbarWindow);
            context->succeeded = true;
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Applying widget failed: %08X",
               static_cast<unsigned>(error));
    }
}

bool ApplyWidgetToTaskbarWindow(HWND taskbarWindow) {
    ApplyWidgetContext context{taskbarWindow, false};
    return RunFromWindowThread(taskbarWindow, ApplyToTaskbarWindow, &context) &&
           context.succeeded;
}

struct TaskbarProbeContext {
    HWND window = nullptr;
    bool ready = false;
};

void ProbeTaskbarWindow(void* contextValue) {
    auto* context = reinterpret_cast<TaskbarProbeContext*>(contextValue);
    try {
        XamlRoot xamlRoot = GetTaskbarXamlRoot(context->window);
        if (!xamlRoot) {
            return;
        }
        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        context->ready = static_cast<bool>(FindChildRecursive(
            content, [](FrameworkElement child) {
                return winrt::get_class_name(child) == L"Taskbar.TaskbarFrame";
            }));
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Probing taskbar XAML failed: %08X",
               static_cast<unsigned>(error));
    }
}

bool IsTaskbarReady(HWND window) {
    TaskbarProbeContext context{window, false};
    return RunFromWindowThread(window, ProbeTaskbarWindow, &context) &&
           context.ready;
}

struct RemoveWidgetForMoveContext {
    bool succeeded = false;
};

void RemoveWidgetForMove(void* contextValue) {
    auto* context =
        reinterpret_cast<RemoveWidgetForMoveContext*>(contextValue);
    try {
        RemoveWidget();
        context->succeeded = true;
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing widget before monitor switch failed: %08X",
               static_cast<unsigned>(error));
    }
}

void RemoveFromCurrentTaskbar(void*) {
    try {
        RemoveWidget();
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing widget failed: %08X",
               static_cast<unsigned>(error));
    }
    try {
        g_loadedRevokers.reset();
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing taskbar Loaded handlers failed: %08X",
               static_cast<unsigned>(error));
    }
    g_taskbarWindow = nullptr;
    g_taskbarThreadId = 0;
}

void ApplyOnTaskbarThread() {
    HWND targetWindow = FindConfiguredTaskbarWindow();
    if (!targetWindow) {
        Wh_Log(L"Taskbar window not found");
        return;
    }

    HWND currentWindow = FindRememberedTaskbarWindow();
    if (currentWindow != targetWindow && !IsTaskbarReady(targetWindow)) {
        HWND primaryWindow = FindPrimaryTaskbarWindow();
        if (!primaryWindow) {
            Wh_Log(L"Selected taskbar is not ready and no fallback exists");
            return;
        }
        if (targetWindow != primaryWindow) {
            Wh_Log(L"Selected taskbar is not ready; using the primary taskbar");
        }
        targetWindow = primaryWindow;
    }

    HWND removalWindow = currentWindow;
    if (!removalWindow && g_taskbarThreadId.load()) {
        removalWindow = FindAnyWindowOnTaskbarThread(nullptr);
    }
    bool widgetRemovedForMove = false;
    if (removalWindow && currentWindow != targetWindow) {
        RemoveWidgetForMoveContext removeContext;
        if (!RunFromWindowThread(removalWindow, RemoveWidgetForMove,
                                 &removeContext) ||
            !removeContext.succeeded) {
            Wh_Log(L"Removing widget from the previous monitor failed");
            return;
        }
        widgetRemovedForMove = true;
    }

    if (!ApplyWidgetToTaskbarWindow(targetWindow)) {
        Wh_Log(L"Applying widget on taskbar thread failed");
        if (widgetRemovedForMove &&
            IsCurrentProcessTaskbarWindow(currentWindow)) {
            Wh_Log(L"Restoring widget on the previous taskbar");
            if (!ApplyWidgetToTaskbarWindow(currentWindow)) {
                Wh_Log(L"Restoring widget on the previous taskbar failed");
            }
        }
    }
}

void EnsureConfiguredTaskbarPlacement() {
    if (!g_widget || g_unloading) {
        return;
    }
    HWND targetWindow = FindConfiguredTaskbarWindow(false);
    HWND rememberedWindow = g_taskbarWindow.load();
    if (!targetWindow ||
        (IsCurrentProcessTaskbarWindow(rememberedWindow) &&
         rememberedWindow == targetWindow)) {
        return;
    }

    Wh_Log(L"Taskbar topology changed; moving the widget");
    ApplyOnTaskbarThread();
}

void ApplyLoadedTaskbarFrame(FrameworkElement taskbarFrame) {
    if (!taskbarFrame || g_unloading) {
        return;
    }

    // The constructor hook already gives us the stable XAML element. Use it
    // directly whenever its taskbar is unambiguous, and keep the symbol-based
    // XAML-root lookup only for choosing between multiple monitor taskbars.
    HWND directWindow = FindOnlyTaskbarWindow();
    if (!directWindow && g_widget) {
        XamlRoot loadedRoot = taskbarFrame.XamlRoot();
        XamlRoot widgetRoot = g_widget.XamlRoot();
        HWND rememberedWindow = g_taskbarWindow.load();
        if (loadedRoot && widgetRoot &&
            winrt::get_abi(loadedRoot) == winrt::get_abi(widgetRoot) &&
            IsCurrentProcessTaskbarWindow(rememberedWindow)) {
            directWindow = rememberedWindow;
        }
    }

    if (directWindow && InjectWidget(taskbarFrame)) {
        RememberTaskbarWindow(directWindow);
        return;
    }
    ApplyOnTaskbarThread();
}

using TaskbarFrame_Constructor_t = void*(WINAPI*)(void* pThis);
TaskbarFrame_Constructor_t TaskbarFrame_Constructor_Original = nullptr;

void* WINAPI TaskbarFrame_Constructor_Hook(void* pThis) {
    void* result = TaskbarFrame_Constructor_Original(pThis);
    if (g_unloading || !g_loadedRevokers) {
        return result;
    }

    FrameworkElement taskbarFrame = nullptr;
    reinterpret_cast<::IUnknown**>(pThis)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarFrame));
    if (!taskbarFrame) {
        return result;
    }

    g_loadedRevokers->emplace_back();
    auto revoker = std::prev(g_loadedRevokers->end());
    *revoker = taskbarFrame.Loaded(
        winrt::auto_revoke_t{},
        [revoker](IInspectable const& sender, RoutedEventArgs const&) {
            if (!g_loadedRevokers) {
                return;
            }
            FrameworkElement loadedFrame = sender.try_as<FrameworkElement>();
            g_loadedRevokers->erase(revoker);
            if (g_unloading) {
                return;
            }
            try {
                ApplyLoadedTaskbarFrame(loadedFrame);
            } catch (...) {
                HRESULT error = winrt::to_hresult();
                Wh_Log(L"Loaded injection failed: %08X",
                       static_cast<unsigned>(error));
            }
        });
    return result;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &RefCountBase_Decref_Original},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      std::size(taskbarDllHooks));
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: __cdecl winrt::Taskbar::implementation::TaskbarFrame::TaskbarFrame(void))"},
        &TaskbarFrame_Constructor_Original,
        TaskbarFrame_Constructor_Hook,
    }};
    return WindhawkUtils::HookSymbols(module, hooks, std::size(hooks));
}

HMODULE GetTaskbarViewModule() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

bool TryHookTaskbarViewSymbols(HMODULE module, bool applyImmediately) {
    if (!module || g_taskbarViewDllLoaded) {
        return static_cast<bool>(g_taskbarViewDllLoaded);
    }

    if (g_taskbarViewHookAttempts >= kMaximumTaskbarViewHookAttempts ||
        g_taskbarViewDllLoaded.exchange(true)) {
        return static_cast<bool>(g_taskbarViewDllLoaded);
    }

    uint32_t attempt = ++g_taskbarViewHookAttempts;
    if (!HookTaskbarViewSymbols(module)) {
        Wh_Log(L"Taskbar.View symbol hook failed (attempt %u/%u)", attempt,
               kMaximumTaskbarViewHookAttempts);
        g_taskbarViewDllLoaded = false;
        return false;
    }
    if (applyImmediately) {
        Wh_ApplyHookOperations();
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (!g_taskbarViewDllLoaded &&
        g_taskbarViewHookAttempts < kMaximumTaskbarViewHookAttempts) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModule()) {
            TryHookTaskbarViewSymbols(taskbarViewModule, true);
        }
    }
    return module;
}

void CloseMetricSources() {
    ClosePdhQuery();
    InvalidateGpuAdapterCache();
    g_nextPdhCounterRetry = {};
    g_nextPdhRecovery = {};
    g_consecutivePdhReadFailures = 0;
}

bool TearDownTaskbarUi() {
    HWND taskbarWindow = FindRememberedTaskbarWindow();
    if (taskbarWindow &&
        RunFromWindowThread(taskbarWindow, RemoveFromCurrentTaskbar, nullptr)) {
        return true;
    }

    // The Shell_TrayWnd can disappear during Explorer teardown. Any surviving
    // window on its UI thread is sufficient: the WH_CALLWNDPROC hook runs the
    // callback, while SendMessage only wakes that thread.
    HWND fallbackWindow = FindAnyWindowOnTaskbarThread(taskbarWindow);
    if (!fallbackWindow) {
        // Injection can fail before g_taskbarWindow/g_taskbarThreadId are set,
        // while TaskbarFrame Loaded revokers are already pending. Every taskbar
        // shares Explorer's XAML UI thread, so the primary window is a safe
        // final marshaling target for revoking them before the DLL unloads.
        fallbackWindow = FindPrimaryTaskbarWindow();
    }
    return fallbackWindow && RunFromWindowThread(
                                 fallbackWindow, RemoveFromCurrentTaskbar,
                                 nullptr);
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_uiTornDown = false;
    if (HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll")) {
        g_d3dkmtEnumAdapters2 = reinterpret_cast<D3DKMTEnumAdapters2_t>(
            GetProcAddress(gdi32, "D3DKMTEnumAdapters2"));
        g_d3dkmtOpenAdapterFromLuid =
            reinterpret_cast<D3DKMTOpenAdapterFromLuid_t>(
                GetProcAddress(gdi32, "D3DKMTOpenAdapterFromLuid"));
        g_d3dkmtQueryAdapterInfo =
            reinterpret_cast<D3DKMTQueryAdapterInfo_t>(
                GetProcAddress(gdi32, "D3DKMTQueryAdapterInfo"));
        g_d3dkmtCloseAdapter = reinterpret_cast<D3DKMTCloseAdapter_t>(
            GetProcAddress(gdi32, "D3DKMTCloseAdapter"));
    }

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols unavailable");
        return FALSE;
    }

    if (HMODULE module = GetTaskbarViewModule()) {
        if (!TryHookTaskbarViewSymbols(module, false)) {
            Wh_Log(L"Taskbar.View symbols unavailable");
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBase) {
            kernelBase = GetModuleHandleW(L"kernel32.dll");
        }
        auto loadLibraryEx = kernelBase
                                 ? reinterpret_cast<LoadLibraryExW_t>(
                                       GetProcAddress(kernelBase,
                                                      "LoadLibraryExW"))
                                 : nullptr;
        if (!loadLibraryEx ||
            !WindhawkUtils::SetFunctionHook(loadLibraryEx, LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            return FALSE;
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE module = GetTaskbarViewModule()) {
            TryHookTaskbarViewSymbols(module, true);
        }
    }
    ApplyOnTaskbarThread();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    WakeMetricsWorker();
    ApplyOnTaskbarThread();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
    StopMetricsWorker();

    g_uiTornDown = TearDownTaskbarUi();
    if (!g_uiTornDown) {
        Wh_Log(L"Initial taskbar UI teardown failed; will retry");
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (!g_uiTornDown) {
        g_uiTornDown = TearDownTaskbarUi();
        if (!g_uiTornDown) {
            Wh_Log(L"Taskbar UI teardown retry failed");
        }
    }
    StopMetricsWorker();
    CloseMetricSources();
}
