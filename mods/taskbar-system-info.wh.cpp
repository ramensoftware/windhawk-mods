// ==WindhawkMod==
// @id              taskbar-system-info
// @name            Taskbar System Info
// @name:uk-UA      Системний монітор панелі завдань
// @description     A quiet two-column CPU, GPU, RAM and VRAM monitor with 60-second history graphs for the Windows 11 taskbar.
// @description:uk-UA Компактний монітор CPU, GPU, RAM і VRAM із 60-секундними графіками для панелі завдань Windows 11.
// @version         1.0.0
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
// Both projects are distributed under the GNU General Public License v3.0.

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
color. Network and disk activity are intentionally not collected.

Unlike the performance placeholders in
[Taskbar Clock Customization](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-clock-customization.wh.cpp),
this mod does not alter the clock. It uses the free far-left taskbar area for a
stable 2x2 dashboard with rolling graphs, capacity bars and temperature alerts.

## Metrics

- CPU utilization from Windows system time counters.
- RAM usage and capacity from Windows memory status.
- GPU utilization and dedicated VRAM usage from Windows PDH counters.
- Dedicated VRAM capacity and adapter identity from DXGI.
- CPU and GPU temperatures from HWiNFO when available.

Metric collection runs on a worker thread. The taskbar UI thread only renders
the latest completed snapshot.

The adapter with the most dedicated VRAM is selected automatically. A partial
adapter-name filter is available for multi-GPU systems. GPU usage and VRAM are
matched to the selected DXGI adapter by LUID.

## Optional HWiNFO temperatures

Temperatures are read from HWiNFO in this order:

1. HWiNFO shared memory (`Global\\HWiNFO_SENS_SM2`).
2. HWiNFO Gadget registry (`HKCU\\Software\\HWiNFO64\\VSB`).

HWiNFO is optional and is not bundled with this mod. Shared-memory integration
targets HWiNFO 7.0 or newer, which permits full disclosure of the interface.
The free HWiNFO64 edition disables shared memory after 12 hours of continuous
use; HWiNFO64 Pro has no such limit. The Gadget fallback can be configured
under **Sensor Settings > HWiNFO Gadget** by enabling **Report to Gadget** for
the desired CPU and GPU temperature readings. If neither source is available,
temperatures are shown as `--°C`; all other metrics continue to work.

## Compatibility and placement

- Windows 11 64-bit, primary taskbar. x64 is hardware-tested; ARM64 is
  compilation-tested.
- Centered taskbar icons are recommended.
- Enable **Reserve space before the Start button** if the widget overlaps
  left-aligned taskbar buttons.
- The widget is native XAML inside the taskbar and can coexist with Taskbar
  Styler.

## Credits and license

Taskbar discovery and window-thread marshaling follow techniques from
[Multirow taskbar for Windows 11](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp)
by Michael Maltsev (`m417z`). Released under GPL-3.0.
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

- reserveSpace: false
  $name: Reserve space before the Start button
  $name:uk-UA: Резервувати місце перед кнопкою Пуск
  $description: "Usually not needed when Windows 11 taskbar icons are centered."
  $description:uk-UA: "Для центрованих значків Windows 11 зазвичай не потрібно."

- reserveGap: 8
  $name: Reserved space gap
  $name:uk-UA: Проміжок після блока

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
  $description: "#RRGGBB or #AARRGGBB. Leave empty to use the system color."
  $description:uk-UA: "#RRGGBB або #AARRGGBB. Порожнє значення використовує системний колір."

- graphColor: "#78A8FF"
  $name: Graph and bar color
  $name:uk-UA: Колір графіків і смуг
  $description: "Accent color for CPU/GPU history and memory capacity bars."
  $description:uk-UA: "Стриманий акцент для історії CPU/GPU та смуг памяті."

- warningColor: "#FFFFB900"
  $name: Warning color
  $name:uk-UA: Колір попередження

- criticalColor: "#FFFF6B6B"
  $name: Critical color
  $name:uk-UA: Критичний колір

- textOpacity: 96
  $name: Text opacity
  $name:uk-UA: Прозорість тексту
  $description: "From 0 to 100 percent."
  $description:uk-UA: "Від 0 до 100."

- cpuWarningTemp: 75
  $name: CPU temperature warning
  $name:uk-UA: Попередження температури CPU

- cpuCriticalTemp: 85
  $name: CPU critical temperature
  $name:uk-UA: Критична температура CPU

- gpuWarningTemp: 80
  $name: GPU temperature warning
  $name:uk-UA: Попередження температури GPU

- gpuCriticalTemp: 90
  $name: GPU critical temperature
  $name:uk-UA: Критична температура GPU

- memoryWarningPercent: 80
  $name: Memory usage warning
  $name:uk-UA: Попередження заповнення памяті

- memoryCriticalPercent: 90
  $name: Critical memory usage
  $name:uk-UA: Критичне заповнення памяті

- gpuAdapter: ""
  $name: GPU adapter filter
  $name:uk-UA: Відеокарта
  $description: "Optional partial adapter name. Empty selects the adapter with the most dedicated VRAM."
  $description:uk-UA: "Необовязкова частина назви. Порожнє значення вибирає адаптер з найбільшим обсягом VRAM."

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
#include <mutex>
#include <optional>
#include <string>
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
constexpr uint32_t kHwInfoSignature = 0x53695748;  // "HWiS"
constexpr uint32_t kHwInfoTemperatureType = 1;

struct ModSettings {
    std::wstring fontFamily;
    std::wstring textColor;
    std::wstring graphColor;
    std::wstring warningColor;
    std::wstring criticalColor;
    std::wstring gpuAdapter;
    std::wstring cpuTempSensor;
    std::wstring gpuTempSensor;
    int width = 410;
    int leftOffset = 10;
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

ModSettings g_settings;
std::mutex g_settingsMutex;
std::atomic<bool> g_unloading;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<HWND> g_taskbarWindow{nullptr};
std::atomic<DWORD> g_taskbarThreadId{0};

[[clang::no_destroy]] Grid g_widget{nullptr};
[[clang::no_destroy]] Grid g_rootGrid{nullptr};
[[clang::no_destroy]] FrameworkElement g_taskItemsRepeater{nullptr};
double g_reservedMargin = 0.0;
double g_graphWidth = 96.0;
double g_memoryBarWidth = 120.0;
[[clang::no_destroy]] DispatcherTimer g_timer{nullptr};
event_token g_timerToken{};
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

std::deque<double> g_cpuHistory;
std::deque<double> g_gpuHistory;
int g_historyInterval = 0;
int g_historyWindow = 0;

PDH_HQUERY g_pdhQuery = nullptr;
PDH_HCOUNTER g_gpuCounter = nullptr;
PDH_HCOUNTER g_vramCounter = nullptr;

struct MetricsSnapshot {
    double cpu = 0.0;
    double ram = 0.0;
    double ramUsedGb = 0.0;
    double ramTotalGb = 0.0;
    double gpu = 0.0;
    double vram = 0.0;
    double vramUsedGb = 0.0;
    double vramTotalGb = 0.0;
    bool vramAvailable = false;
    std::optional<double> cpuTemp;
    std::optional<double> gpuTemp;
};

std::mutex g_metricsMutex;
MetricsSnapshot g_latestMetrics;
uint64_t g_latestMetricsSequence = 0;
bool g_latestMetricsAvailable = false;
uint64_t g_lastRenderedMetricsSequence = 0;

std::mutex g_metricsWorkerMutex;
std::atomic<bool> g_stopMetricsWorker{false};
HANDLE g_metricsWorkerWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_metricsWorker;

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

void LoadSettings() {
    ModSettings settings;
    settings.fontFamily = GetStringSetting(L"fontFamily");
    settings.textColor = GetStringSetting(L"textColor");
    settings.graphColor = GetStringSetting(L"graphColor");
    settings.warningColor = GetStringSetting(L"warningColor");
    settings.criticalColor = GetStringSetting(L"criticalColor");
    settings.gpuAdapter = GetStringSetting(L"gpuAdapter");
    settings.cpuTempSensor = GetStringSetting(L"cpuTempSensor");
    settings.gpuTempSensor = GetStringSetting(L"gpuTempSensor");
    settings.width = std::clamp(Wh_GetIntSetting(L"width"), 330, 800);
    settings.leftOffset = std::clamp(Wh_GetIntSetting(L"leftOffset"), 0, 1000);
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
    if (settings.graphColor.empty()) {
        settings.graphColor = L"#78A8FF";
    }
    if (settings.warningColor.empty()) {
        settings.warningColor = L"#FFFFB900";
    }
    if (settings.criticalColor.empty()) {
        settings.criticalColor = L"#FFFF6B6B";
    }

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(settings);
}

ModSettings CurrentSettings() {
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

    if (!Contains(sensor, L"cpu") && !Contains(sensor, L"ryzen") &&
        !Contains(sensor, L"processor")) {
        return -1;
    }

    if (Contains(reading, L"vrm") || Contains(reading, L"ccd") ||
        Contains(reading, L"iod") || Contains(reading, L"soc") ||
        Contains(reading, L"l3 cache")) {
        return -1;
    }

    if (Contains(reading, L"cpu (tctl/tdie)")) {
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
    return -1;
}

int GpuTemperatureScore(const std::wstring& sensorName,
                        const std::wstring& label,
                        const std::wstring& preferred) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    std::wstring combined = sensor + L" " + reading;
    std::wstring preferredLower = ToLower(preferred);

    if (!preferredLower.empty()) {
        return Contains(combined, preferredLower) ? 10000 : -1;
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
        return 1000;
    }
    if (Contains(reading, L"gpu temperature")) {
        return 950;
    }
    if (Contains(reading, L"gpu core")) {
        return 900;
    }
    if (Contains(reading, L"temperature")) {
        return 500;
    }
    return -1;
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

bool ReadHwInfoSharedMemory(MetricsSnapshot& snapshot,
                            const ModSettings& settings) {
    HANDLE mapping = OpenFileMappingW(FILE_MAP_READ, FALSE,
                                      L"Global\\HWiNFO_SENS_SM2");
    if (!mapping) {
        return false;
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
            return false;
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
        return false;
    }

    bool available = false;
    MEMORY_BASIC_INFORMATION memoryInfo{};
    if (VirtualQuery(view, &memoryInfo, sizeof(memoryInfo)) &&
        memoryInfo.RegionSize >= sizeof(HwInfoHeader)) {
        const auto* header = static_cast<const HwInfoHeader*>(view);
        size_t viewOffset = static_cast<const uint8_t*>(view) -
                            static_cast<const uint8_t*>(memoryInfo.BaseAddress);
        size_t mappedSize = memoryInfo.RegionSize - viewOffset;

        if (header->signature == kHwInfoSignature &&
            IsRangeValid(mappedSize, header->sensorOffset,
                         header->sensorStride, header->sensorCount,
                         sizeof(HwInfoSensorPrefix)) &&
            IsRangeValid(mappedSize, header->readingOffset,
                         header->readingStride, header->readingCount,
                         sizeof(HwInfoReadingPrefix))) {
            int bestCpuScore = -1;
            int bestGpuScore = -1;
            const auto* bytes = static_cast<const uint8_t*>(view);

            for (uint32_t i = 0; i < header->readingCount; i++) {
                HwInfoReadingPrefix reading{};
                const uint8_t* readingAddress =
                    bytes + header->readingOffset +
                    static_cast<size_t>(i) * header->readingStride;
                std::memcpy(&reading, readingAddress, sizeof(reading));

                if (reading.readingType != kHwInfoTemperatureType ||
                    reading.sensorIndex >= header->sensorCount ||
                    !std::isfinite(reading.value) || reading.value < -50.0 ||
                    reading.value > 200.0) {
                    continue;
                }

                HwInfoSensorPrefix sensor{};
                const uint8_t* sensorAddress =
                    bytes + header->sensorOffset +
                    static_cast<size_t>(reading.sensorIndex) *
                        header->sensorStride;
                std::memcpy(&sensor, sensorAddress, sizeof(sensor));

                std::wstring sensorName =
                    FixedAnsiToWide(sensor.originalName,
                                    std::size(sensor.originalName));
                std::wstring label =
                    FixedAnsiToWide(reading.originalLabel,
                                    std::size(reading.originalLabel));

                int cpuScore = CpuTemperatureScore(
                    sensorName, label, settings.cpuTempSensor);
                if (cpuScore > bestCpuScore) {
                    bestCpuScore = cpuScore;
                    snapshot.cpuTemp = reading.value;
                }

                int gpuScore = GpuTemperatureScore(
                    sensorName, label, settings.gpuTempSensor);
                if (gpuScore > bestGpuScore) {
                    bestGpuScore = gpuScore;
                    snapshot.gpuTemp = reading.value;
                }
            }

            available = true;
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
    return available;
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

bool ReadHwInfoGadgetRegistry(MetricsSnapshot& snapshot,
                              const ModSettings& settings) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\HWiNFO64\\VSB", 0,
                      KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        return false;
    }

    int bestCpuScore = snapshot.cpuTemp ? 10000 : -1;
    int bestGpuScore = snapshot.gpuTemp ? 10000 : -1;
    bool foundAny = false;

    for (int i = 0; i < 1024; i++) {
        std::wstring suffix = std::to_wstring(i);
        auto sensor = ReadRegistryString(key, L"Sensor" + suffix);
        if (!sensor) {
            break;
        }
        auto label = ReadRegistryString(key, L"Label" + suffix);
        auto rawValue = ReadRegistryString(key, L"ValueRaw" + suffix);
        if (!label || !rawValue) {
            continue;
        }
        auto value = ParseLocalizedDouble(*rawValue);
        if (!value || *value < -50.0 || *value > 200.0) {
            continue;
        }

        foundAny = true;
        int cpuScore =
            CpuTemperatureScore(*sensor, *label, settings.cpuTempSensor);
        if (cpuScore > bestCpuScore) {
            bestCpuScore = cpuScore;
            snapshot.cpuTemp = *value;
        }

        int gpuScore =
            GpuTemperatureScore(*sensor, *label, settings.gpuTempSensor);
        if (gpuScore > bestGpuScore) {
            bestGpuScore = gpuScore;
            snapshot.gpuTemp = *value;
        }
    }

    RegCloseKey(key);
    return foundAny;
}

void ReadTemperatures(MetricsSnapshot& snapshot,
                      const ModSettings& settings) {
    ReadHwInfoSharedMemory(snapshot, settings);
    if (!snapshot.cpuTemp || !snapshot.gpuTemp) {
        ReadHwInfoGadgetRegistry(snapshot, settings);
    }
}

uint64_t FileTimeValue(const FILETIME& value) {
    ULARGE_INTEGER result{};
    result.LowPart = value.dwLowDateTime;
    result.HighPart = value.dwHighDateTime;
    return result.QuadPart;
}

double ReadCpuUsage() {
    static bool initialized = false;
    static uint64_t previousIdle = 0;
    static uint64_t previousKernel = 0;
    static uint64_t previousUser = 0;

    FILETIME idleTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return 0.0;
    }

    uint64_t idle = FileTimeValue(idleTime);
    uint64_t kernel = FileTimeValue(kernelTime);
    uint64_t user = FileTimeValue(userTime);
    if (!initialized) {
        initialized = true;
        previousIdle = idle;
        previousKernel = kernel;
        previousUser = user;
        return 0.0;
    }

    uint64_t idleDelta = idle - previousIdle;
    uint64_t kernelDelta = kernel - previousKernel;
    uint64_t userDelta = user - previousUser;
    previousIdle = idle;
    previousKernel = kernel;
    previousUser = user;

    uint64_t total = kernelDelta + userDelta;
    if (!total || idleDelta > total) {
        return 0.0;
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

    constexpr double gib = 1024.0 * 1024.0 * 1024.0;
    snapshot.ram = static_cast<double>(memory.dwMemoryLoad);
    snapshot.ramTotalGb = static_cast<double>(memory.ullTotalPhys) / gib;
    snapshot.ramUsedGb =
        static_cast<double>(memory.ullTotalPhys - memory.ullAvailPhys) / gib;
}

constexpr double kGiB = 1024.0 * 1024.0 * 1024.0;

struct DxgiAdapterInfo {
    std::wstring description;
    std::wstring luid;
    uint64_t dedicatedVideoMemory = 0;
};

std::optional<DxgiAdapterInfo> GetDxgiAdapterInfo(
    const std::wstring& adapterFilter) {
    static std::optional<std::wstring> cachedFilter;
    static std::optional<DxgiAdapterInfo> cachedInfo;

    std::wstring filterLower = ToLower(adapterFilter);
    if (cachedInfo && cachedFilter == filterLower) {
        return cachedInfo;
    }

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

    wchar_t luid[32];
    swprintf(luid, std::size(luid), L"0x%08X_0x%08X",
             static_cast<DWORD>(selected.AdapterLuid.HighPart),
             selected.AdapterLuid.LowPart);
    cachedFilter = filterLower;
    cachedInfo = DxgiAdapterInfo{selected.Description, ToLower(luid),
                                  selected.DedicatedVideoMemory};
    Wh_Log(L"Selected GPU: %s, LUID %s, VRAM %.1f GiB",
           cachedInfo->description.c_str(), cachedInfo->luid.c_str(),
           static_cast<double>(cachedInfo->dedicatedVideoMemory) / kGiB);
    return cachedInfo;
}

bool MatchesGpuAdapter(const std::wstring& instance,
                       const std::optional<DxgiAdapterInfo>& adapter) {
    return !adapter || Contains(ToLower(instance), adapter->luid);
}

void CloseMetricSources();

void EnsurePdhQuery() {
    if (g_pdhQuery) {
        return;
    }

    if (PdhOpenQueryW(nullptr, 0, &g_pdhQuery) != ERROR_SUCCESS) {
        g_pdhQuery = nullptr;
        return;
    }

    PDH_STATUS gpuStatus = PdhAddEnglishCounterW(
        g_pdhQuery, L"\\GPU Engine(*)\\Utilization Percentage", 0,
        &g_gpuCounter);
    if (gpuStatus != ERROR_SUCCESS) {
        g_gpuCounter = nullptr;
        Wh_Log(L"Adding the GPU usage counter failed: %08X", gpuStatus);
    }

    PDH_STATUS vramStatus = PdhAddEnglishCounterW(
        g_pdhQuery, L"\\GPU Adapter Memory(*)\\Dedicated Usage", 0,
        &g_vramCounter);
    if (vramStatus != ERROR_SUCCESS) {
        g_vramCounter = nullptr;
        Wh_Log(L"Adding the VRAM usage counter failed: %08X", vramStatus);
    }

    if (!g_gpuCounter && !g_vramCounter) {
        PdhCloseQuery(g_pdhQuery);
        g_pdhQuery = nullptr;
        return;
    }

    PDH_STATUS collectStatus = PdhCollectQueryData(g_pdhQuery);
    if (collectStatus != ERROR_SUCCESS) {
        Wh_Log(L"Initial GPU counter collection failed: %08X", collectStatus);
    }
}

bool ReadPdhArray(PDH_HCOUNTER counter,
                  std::vector<uint8_t>& buffer,
                  DWORD& itemCount) {
    if (!counter) {
        return false;
    }
    DWORD bufferSize = 0;
    PDH_STATUS status = PdhGetFormattedCounterArrayW(
        counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, nullptr);
    if (status != static_cast<PDH_STATUS>(PDH_MORE_DATA) || !bufferSize) {
        return false;
    }

    buffer.resize(bufferSize);
    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    return PdhGetFormattedCounterArrayW(counter, PDH_FMT_DOUBLE, &bufferSize,
                                        &itemCount, items) == ERROR_SUCCESS;
}

double ReadGpuUsage(const std::optional<DxgiAdapterInfo>& adapter) {
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    if (!ReadPdhArray(g_gpuCounter, buffer, itemCount)) {
        return 0.0;
    }

    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    std::unordered_map<std::wstring, double> engineTotals;
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
    }

    double busiestEngine = 0.0;
    for (const auto& [engine, usage] : engineTotals) {
        busiestEngine = std::max(busiestEngine, usage);
    }
    return std::clamp(busiestEngine, 0.0, 100.0);
}

std::optional<double> ReadVramUsedBytes(
    const std::optional<DxgiAdapterInfo>& adapter) {
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    if (!ReadPdhArray(g_vramCounter, buffer, itemCount)) {
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

void ReadPdhMetrics(MetricsSnapshot& snapshot, const ModSettings& settings) {
    EnsurePdhQuery();
    if (!g_pdhQuery || PdhCollectQueryData(g_pdhQuery) != ERROR_SUCCESS) {
        return;
    }

    auto adapter = GetDxgiAdapterInfo(settings.gpuAdapter);
    snapshot.gpu = ReadGpuUsage(adapter);
    auto vramUsedBytes = ReadVramUsedBytes(adapter);
    if (adapter && adapter->dedicatedVideoMemory > 0 && vramUsedBytes) {
        snapshot.vramUsedGb = *vramUsedBytes / kGiB;
        snapshot.vramTotalGb =
            static_cast<double>(adapter->dedicatedVideoMemory) / kGiB;
        snapshot.vram = std::clamp(
            snapshot.vramUsedGb / snapshot.vramTotalGb * 100.0, 0.0, 100.0);
        snapshot.vramAvailable = true;
    }
}

MetricsSnapshot CollectMetrics(const ModSettings& settings) {
    MetricsSnapshot snapshot;
    snapshot.cpu = ReadCpuUsage();
    ReadMemory(snapshot);
    ReadPdhMetrics(snapshot, settings);
    ReadTemperatures(snapshot, settings);
    return snapshot;
}

void PublishMetrics(MetricsSnapshot snapshot) {
    std::lock_guard lock(g_metricsMutex);
    g_latestMetrics = std::move(snapshot);
    g_latestMetricsSequence++;
    g_latestMetricsAvailable = true;
}

bool GetLatestMetrics(MetricsSnapshot& snapshot, uint64_t& sequence) {
    std::lock_guard lock(g_metricsMutex);
    if (!g_latestMetricsAvailable) {
        return false;
    }
    snapshot = g_latestMetrics;
    sequence = g_latestMetricsSequence;
    return true;
}

void MetricsWorkerProc() {
    ReadCpuUsage();
    EnsurePdhQuery();

    bool firstSample = true;
    while (!g_stopMetricsWorker) {
        ModSettings settings = CurrentSettings();
        DWORD waitMilliseconds =
            firstSample ? 250 : static_cast<DWORD>(settings.updateInterval) * 1000;
        DWORD waitResult =
            WaitForSingleObject(g_metricsWorkerWakeEvent, waitMilliseconds);
        firstSample = false;

        if (g_stopMetricsWorker) {
            break;
        }
        if (waitResult == WAIT_FAILED) {
            Wh_Log(L"Metrics worker wait failed: %u", GetLastError());
            break;
        }

        settings = CurrentSettings();
        PublishMetrics(CollectMetrics(settings));
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
        g_latestMetrics = {};
        g_latestMetricsSequence = 0;
        g_latestMetricsAvailable = false;
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
    g_latestMetrics = {};
    g_latestMetricsSequence = 0;
    g_latestMetricsAvailable = false;
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
    wchar_t buffer[64];
    swprintf(buffer, std::size(buffer), L"%.1f/%.0fG", usedGb,
             std::round(totalGb));
    return buffer;
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

void SetTextForeground(TextBlock text,
                       AlertLevel alert,
                       const ModSettings& settings) {
    if (!text) {
        return;
    }

    std::optional<Color> color;
    if (alert == AlertLevel::Critical) {
        color = ParseColor(settings.criticalColor);
    } else if (alert == AlertLevel::Warning) {
        color = ParseColor(settings.warningColor);
    } else {
        color = ParseColor(settings.textColor);
    }

    if (color) {
        text.Foreground(SolidColorBrush(*color));
    } else {
        text.ClearValue(TextBlock::ForegroundProperty());
    }
}

SolidColorBrush AlertBrush(AlertLevel alert, const ModSettings& settings) {
    if (alert == AlertLevel::Critical) {
        return BrushFromSetting(settings.criticalColor,
                                MakeColor(0xFF, 0xFF, 0x6B, 0x6B));
    }
    if (alert == AlertLevel::Warning) {
        return BrushFromSetting(settings.warningColor,
                                MakeColor(0xFF, 0xFF, 0xB9, 0x00));
    }
    return BrushFromSetting(settings.graphColor,
                            MakeColor(0xFF, 0x78, 0xA8, 0xFF));
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
                     AlertLevel alert,
                     const ModSettings& settings) {
    if (!fill) {
        return;
    }
    fill.Width(available ? g_memoryBarWidth *
                               std::clamp(percent, 0.0, 100.0) / 100.0
                         : 0.0);
    fill.Fill(AlertBrush(alert, settings));
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
    text.TextTrimming(TextTrimming::None);
    SetTextForeground(text, AlertLevel::Normal, settings);
}

void ApplyWidgetGeometry(const ModSettings& settings) {
    if (!g_widget) {
        return;
    }

    double rightWidth =
        std::clamp(static_cast<double>(settings.width) * 0.38, 145.0, 170.0);
    double leftWidth = settings.width - kColumnGap - rightWidth;
    g_graphWidth = std::max(
        48.0, leftWidth - kMetricLabelWidth - kMetricUsageWidth -
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
    margin.Left -= g_reservedMargin;
    g_reservedMargin = settings.reserveSpace
                           ? settings.leftOffset + settings.width +
                                 settings.reserveGap
                           : 0.0;
    margin.Left += g_reservedMargin;
    g_taskItemsRepeater.Margin(margin);
}

void ApplyWidgetSettings() {
    if (!g_widget) {
        return;
    }
    ModSettings settings = CurrentSettings();
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

    SolidColorBrush graphBrush = AlertBrush(AlertLevel::Normal, settings);
    for (XamlPolyline graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Stroke(graphBrush);
            graph.StrokeThickness(1.25);
            graph.StrokeStartLineCap(PenLineCap::Round);
            graph.StrokeEndLineCap(PenLineCap::Round);
            graph.StrokeLineJoin(PenLineJoin::Round);
            graph.Opacity(0.78);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Fill(graphBrush);
            track.Opacity(0.18);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Fill(graphBrush);
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
        g_timer.Interval(std::chrono::seconds(settings.updateInterval));
    }
}

void UpdateWidgetText() {
    if (!g_widget || g_unloading) {
        return;
    }
    ModSettings settings = CurrentSettings();
    MetricsSnapshot snapshot;
    uint64_t metricsSequence = 0;
    if (!GetLatestMetrics(snapshot, metricsSequence)) {
        return;
    }

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
    g_ramAlert = EvaluateAlert(snapshot.ram, settings.memoryWarningPercent,
                               settings.memoryCriticalPercent, g_ramAlert, 3.0);
    g_vramAlert =
        snapshot.vramAvailable
            ? EvaluateAlert(snapshot.vram, settings.memoryWarningPercent,
                            settings.memoryCriticalPercent, g_vramAlert, 3.0)
            : AlertLevel::Normal;

    if (g_cpuUsageText) {
        g_cpuUsageText.Text(FormatPercent(snapshot.cpu));
    }
    if (g_cpuTempText) {
        g_cpuTempText.Text(FormatTemperature(snapshot.cpuTemp));
        SetTextForeground(g_cpuTempText, g_cpuTemperatureAlert, settings);
    }
    if (g_gpuUsageText) {
        g_gpuUsageText.Text(FormatPercent(snapshot.gpu));
    }
    if (g_gpuTempText) {
        g_gpuTempText.Text(FormatTemperature(snapshot.gpuTemp));
        SetTextForeground(g_gpuTempText, g_gpuTemperatureAlert, settings);
    }
    if (g_ramPercentText) {
        g_ramPercentText.Text(FormatPercent(snapshot.ram));
        SetTextForeground(g_ramPercentText, g_ramAlert, settings);
    }
    if (g_ramCapacityText) {
        g_ramCapacityText.Text(FormatCapacity(snapshot.ramUsedGb,
                                              snapshot.ramTotalGb, true));
    }
    if (g_vramPercentText) {
        g_vramPercentText.Text(snapshot.vramAvailable
                                   ? FormatPercent(snapshot.vram)
                                   : L"--%");
        SetTextForeground(g_vramPercentText, g_vramAlert, settings);
    }
    if (g_vramCapacityText) {
        g_vramCapacityText.Text(
            FormatCapacity(snapshot.vramUsedGb, snapshot.vramTotalGb,
                           snapshot.vramAvailable));
    }

    if (metricsSequence != g_lastRenderedMetricsSequence) {
        size_t historyCapacity = HistoryCapacity(settings);
        AppendHistory(g_cpuHistory, snapshot.cpu, historyCapacity);
        AppendHistory(g_gpuHistory, snapshot.gpu, historyCapacity);
        UpdateSparkline(g_cpuGraph, g_cpuHistory, historyCapacity);
        UpdateSparkline(g_gpuGraph, g_gpuHistory, historyCapacity);
        g_lastRenderedMetricsSequence = metricsSequence;
    }
    UpdateMemoryBar(g_ramFill, snapshot.ram, true, g_ramAlert, settings);
    UpdateMemoryBar(g_vramFill, snapshot.vram, snapshot.vramAvailable,
                    g_vramAlert, settings);
}

void EnsureTimer() {
    if (g_timer) {
        return;
    }
    ModSettings settings = CurrentSettings();
    g_timer = DispatcherTimer();
    g_timer.Interval(std::chrono::seconds(settings.updateInterval));
    g_timerToken = g_timer.Tick([](IInspectable const&, IInspectable const&) {
        try {
            UpdateWidgetText();
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
    text.TextTrimming(TextTrimming::None);
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

    if (g_taskItemsRepeater && g_reservedMargin != 0.0) {
        Thickness margin = g_taskItemsRepeater.Margin();
        margin.Left -= g_reservedMargin;
        g_taskItemsRepeater.Margin(margin);
    }
    g_reservedMargin = 0.0;

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
            UpdateWidgetText();
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
    g_taskItemsRepeater =
        FindDirectChildByName(root, L"TaskbarFrameRepeater");
    g_reservedMargin = 0.0;

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

HWND FindCurrentProcessTaskbarWindow() {
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

bool IsCurrentProcessWindow(HWND window) {
    DWORD processId = 0;
    WCHAR className[64];
    return window && IsWindow(window) &&
           GetWindowThreadProcessId(window, &processId) != 0 &&
           processId == GetCurrentProcessId() &&
           GetClassNameW(window, className, std::size(className)) != 0 &&
           _wcsicmp(className, L"Shell_TrayWnd") == 0;
}

void RememberTaskbarWindow(HWND window) {
    if (!IsCurrentProcessWindow(window)) {
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
    if (IsCurrentProcessWindow(rememberedWindow)) {
        return rememberedWindow;
    }

    DWORD rememberedThreadId = g_taskbarThreadId.load();
    if (rememberedThreadId) {
        HWND threadWindow = nullptr;
        EnumThreadWindows(
            rememberedThreadId,
            [](HWND window, LPARAM context) -> BOOL {
                WCHAR className[64];
                if (GetClassNameW(window, className, std::size(className)) &&
                    _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                    *reinterpret_cast<HWND*>(context) = window;
                    return FALSE;
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&threadWindow));
        if (IsCurrentProcessWindow(threadWindow)) {
            RememberTaskbarWindow(threadWindow);
            return threadWindow;
        }
    }

    HWND currentWindow = FindCurrentProcessTaskbarWindow();
    if (currentWindow) {
        RememberTaskbarWindow(currentWindow);
    }
    return currentWindow;
}

using CTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using RefCountBase_Decref_t = void(WINAPI*)(void* pThis);
RefCountBase_Decref_t RefCountBase_Decref_Original = nullptr;

void* CTaskBand_ITaskListWndSite_vftable = nullptr;

XamlRoot GetTaskbarXamlRoot(HWND taskbarWindow) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original || !RefCountBase_Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND taskBandWindow =
        reinterpret_cast<HWND>(GetPropW(taskbarWindow, L"TaskbandHWND"));
    if (!taskBandWindow) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(
        GetWindowLongPtrW(taskBandWindow, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0;
         *reinterpret_cast<void**>(taskBandForSite) !=
         CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
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
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[3] == 0x28 &&
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
    if (code[0] == 0xD503237F &&
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

    auto* elementUnknown = *reinterpret_cast<::IUnknown**>(
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset);
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

void ApplyToCurrentTaskbar(void*) {
    HWND taskbarWindow = FindCurrentProcessTaskbarWindow();
    if (!taskbarWindow) {
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
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Applying widget failed: %08X",
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
    HWND taskbarWindow = FindRememberedTaskbarWindow();
    if (!taskbarWindow) {
        Wh_Log(L"Taskbar window not found");
        return;
    }
    if (!RunFromWindowThread(taskbarWindow, ApplyToCurrentTaskbar, nullptr)) {
        Wh_Log(L"Applying widget on taskbar thread failed");
    }
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
        [revoker](IInspectable const&, RoutedEventArgs const&) {
            if (!g_loadedRevokers) {
                return;
            }
            g_loadedRevokers->erase(revoker);
            if (g_unloading) {
                return;
            }
            try {
                ApplyToCurrentTaskbar(nullptr);
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
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
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

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module && !g_taskbarViewDllLoaded && GetTaskbarViewModule() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        if (HookTaskbarViewSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    return module;
}

void CloseMetricSources() {
    if (g_pdhQuery) {
        PdhCloseQuery(g_pdhQuery);
        g_pdhQuery = nullptr;
        g_gpuCounter = nullptr;
        g_vramCounter = nullptr;
    }
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols unavailable");
        return FALSE;
    }

    if (HMODULE module = GetTaskbarViewModule()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewSymbols(module)) {
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
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE module = GetTaskbarViewModule()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewSymbols(module)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    ApplyOnTaskbarThread();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    WakeMetricsWorker();
    ApplyOnTaskbarThread();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    StopMetricsWorker();

    HWND taskbarWindow = FindRememberedTaskbarWindow();
    if (!taskbarWindow) {
        Wh_Log(L"Taskbar window unavailable during teardown");
    } else if (!RunFromWindowThread(taskbarWindow, RemoveFromCurrentTaskbar,
                                    nullptr)) {
        Wh_Log(L"Taskbar UI teardown failed");
    }
}

void Wh_ModUninit() {
    StopMetricsWorker();
    CloseMetricSources();
}
