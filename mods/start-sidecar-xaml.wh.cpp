// ==WindhawkMod==
// @id start-sidecar-xaml
// @name Start Sidecar - Embedded XAML
// @name:zh-CN 开始菜单卡片 - Start Sidecar
// @description Native XAML live cards inside the Windows 11 Start companion layout.
// @version 0.4.9
// @author Yizuka17
// @github https://github.com/Yizuka17
// @license MIT
// @include StartMenuExperienceHost.exe
// @architecture x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -luser32 -lshell32 -lcomctl32 -ladvapi32 -lwbemuuid -lpdh -lwinhttp -lpowrprof -luuid -liphlpapi
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModSettings==
/*
- showOverview: true
  $name: 显示系统概览卡片
- deviceTitle: ''
  $name: 设备名称
  $description: 留空自动读取设备型号。
- overviewWidth: 252
  $name: 系统概览宽度（DIP）
  $description: 范围 200–340，默认 252。
- cardGap: -1
  $name: 系统概览间距（DIP）
  $description: -1 跟随原生间距；0–24 指定间距。
- showTime: true
  $name: 系统概览：时间
- showBattery: true
  $name: 系统概览：电池
- showMemory: true
  $name: 系统概览：内存
- showPerformance: true
  $name: 系统概览：CPU / GPU
- showMedia: true
  $name: 下方模块：系统媒体控制（SMTC）
- showWeather: true
  $name: 下方模块：天气
- showQuickSettings: true
  $name: 下方模块：快速设置
- showNetwork: true
  $name: 下方模块：网络状态和速度
- showStorage: true
  $name: 下方模块：系统盘空间
- showVolume: true
  $name: 快速设置：音量
- showBrightness: true
  $name: 快速设置：内置屏幕亮度
- weatherCity: Shanghai
  $name: 天气地区
  $description: 按地区名查询；部分区县可通过经纬度精确指定。不读取设备定位。
- weatherProvider: open_meteo
  $name: 天气数据源
  $options:
  - open_meteo: Open-Meteo
  - custom: 自定义 JSON 接口（支持 MSN 连接器响应）
- weatherCustomUrl: ''
  $name: 自定义天气接口 URL
  $description: HTTPS JSON 接口。支持 MSN Weather 连接器、Open-Meteo 或 README 中的简单格式。
- weatherLatitude: ''
  $name: 天气纬度（可选）
- weatherLongitude: ''
  $name: 天气经度（可选）
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Start Sidecar - Embedded XAML

Adds native XAML cards beside and below the Windows 11 Start menu's Phone Link
companion layout. There is no separate overlay window or external layout file.
Configure individual cards, device title, overview width and spacing in this
mod's Settings tab. The card labels and settings currently use Simplified Chinese.

Available cards:

- System overview: device model, time, battery, memory, CPU and GPU usage.
- Media: the current Windows SMTC session, artwork and playback controls.
- Weather: configured city or coordinates; Celsius temperatures.
- Quick settings: volume, mute and supported internal-display brightness.
- Network: active physical interface and sampled upload/download rates.
- Storage: system-drive usage and free space.

## Compatibility

Experimental. Runtime-tested on Windows 11 Insider build 26340.9882 (x64).
Other builds and ARM64 runtime behavior have not been verified. This mod depends
on internal Start menu XAML names and the Phone Link companion layout being
available; unsupported layouts can leave the cards unavailable. Start menu
replacement and styling mods may conflict. This is not a Windows 10 Live Tile
host and does not load third-party tile providers.

GPU usage is sampled from Windows GPU engine performance counters; the busiest
engine can fluctuate or disagree with Task Manager's selected graph. Battery
estimates depend on firmware and Windows, with a capacity/rate fallback when
available. Brightness control requires a compatible built-in display. Media
controls and application activation depend on what the current player exposes.

## Weather and network requests

Weather is enabled by default, with Shanghai as the example city. Change the city
or disable the weather card in Settings. Device location is not requested.
The city is sent to Open-Meteo's geocoding service; coordinates are sent to its
forecast service, approximately every ten minutes (one minute after errors).
Configured coordinates bypass geocoding. These services receive the usual
network connection information, including the client's IP address.

Weather data: [Open-Meteo](https://open-meteo.com/),
[CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

A custom HTTPS JSON URL can be selected instead. That URL receives the requests;
do not share exported settings containing private URL tokens. Supported response
shapes include Open-Meteo's `current` object, an MSN connector-style object at
`responses.weather.current` (`temp`, `feels`, `rh`, `cap`), or a simple object:

```json
{"temperature": 21, "feels_like": 20, "humidity": 60, "condition": "Cloudy"}
```

There is no built-in MSN credential or direct MSN service integration.

## Credits

UI-thread initialization references the MIT-licensed
[Windows 11 Start Menu Buttons](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-11-start-menu-buttons.wh.cpp)
by Salyts. Media card design and activation logic were adapted from the
submitter's local MediaPeek prototype. Developed with ChatGPT assistance.
*/
// ==/WindhawkModReadme==
// clang-format on

/*
MIT License

Copyright (c) 2026 Yizuka17
Copyright (c) 2026 Salyts (adapted UI-thread initialization)

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

#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <wbemidl.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <winhttp.h>
#include <powrprof.h>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <sstream>
#include <winrt/Windows.Devices.Power.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <winrt/Windows.Networking.Connectivity.h>

namespace svc {
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;
using namespace Windows::Data::Json;
struct Config {
    bool overview = true, time = true, battery = true, memory = true,
         performance = true, media = true, weather = true, quick = true,
         volume = true, brightness = true, network = true, storage = true;
    int overviewWidth = 252, cardGap = -1;
    std::wstring title, city = L"Shanghai", latitude, longitude,
                        provider = L"open_meteo", customUrl;
};
struct Data {
    std::wstring device = L"此电脑", batteryDetail = L"预计剩余时间待估算",
                 title = L"暂无媒体", artist, mediaError,
                 mediaSource = L"媒体播放";
    std::wstring weatherCondition = L"天气加载中",
                 weatherSource = L"Open-Meteo", weatherError, weatherTime,
                 brightnessError;
    std::wstring mediaApp, networkName = L"正在读取网络", networkState,
                           storageName = L"系统盘";
    double download = -1, upload = -1, diskUsed = -1, diskTotal = -1,
           diskFree = -1;
    double cpu = -1, gpu = -1, volume = -1, brightness = -1, temperature = NAN,
           feels = NAN, humidity = NAN;
    bool muted = false, playing = false, previous = false, next = false,
         toggle = false, mediaReady = false, brightnessReady = false;
    IRandomAccessStream art{nullptr}, mediaLogo{nullptr};
    uint64_t artRevision = 0;
};
inline std::shared_ptr<Config> config = std::make_shared<Config>();
inline std::mutex dataMutex;
inline Data data;
inline HANDLE stop = nullptr, wake = nullptr, weatherWake = nullptr,
              mediaWake = nullptr;
inline std::thread worker, weatherWorker, mediaWorker;
inline std::atomic<int> volumeRequest = -1, brightnessRequest = -1,
                        muteRequest = -1, mediaRequest = 0;
inline std::atomic<bool> visible = true;
inline void SetVisible(bool value) {
    visible = value;
    if (wake)
        SetEvent(wake);
    if (mediaWake)
        SetEvent(mediaWake);
}
inline Data Snapshot() {
    std::lock_guard lock(dataMutex);
    return data;
}
inline void Configure(std::shared_ptr<Config> value) {
    std::atomic_store(&config, value);
    if (wake)
        SetEvent(wake);
    if (weatherWake)
        SetEvent(weatherWake);
}
inline void Volume(int value) {
    volumeRequest = std::clamp(value, 0, 100);
    if (wake)
        SetEvent(wake);
}
inline void Brightness(int value) {
    brightnessRequest = std::clamp(value, 1, 100);
    if (wake)
        SetEvent(wake);
}
inline void Mute(bool value) {
    muteRequest = value ? 1 : 0;
    if (wake)
        SetEvent(wake);
}
inline void Media(int action) {
    mediaRequest = action;
    if (mediaWake)
        SetEvent(mediaWake);
}
inline bool Stopped() {
    return stop && WaitForSingleObject(stop, 0) == WAIT_OBJECT_0;
}
template <class Async>
auto Await(Async const& operation) {
    auto deadline = GetTickCount64() + 4000;
    while (operation.Status() == Windows::Foundation::AsyncStatus::Started) {
        if (Stopped() || GetTickCount64() > deadline) {
            operation.Cancel();
            throw hresult_error(HRESULT_FROM_WIN32(ERROR_TIMEOUT));
        }
        WaitForSingleObject(stop, 10);
    }
    return operation.GetResults();
}
inline std::wstring Duration(double seconds) {
    if (!std::isfinite(seconds) || seconds < 0 || seconds > 7 * 86400)
        return L"时间待估算";
    int minutes = std::max(1, (int)std::round(seconds / 60));
    if (minutes < 60)
        return std::to_wstring(minutes) + L"分钟";
    return std::to_wstring(minutes / 60) + L"小时" +
           (minutes % 60 ? std::to_wstring(minutes % 60) + L"分钟" : L"");
}
inline std::wstring DeviceName() {
    wchar_t name[512] = {};
    DWORD bytes = sizeof(name);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS",
                     L"SystemProductName", RRF_RT_REG_SZ, nullptr, name,
                     &bytes) != ERROR_SUCCESS)
        return L"此电脑";
    std::wstring model = name;
    return model.empty() ? L"此电脑" : model;
}
struct Bstr {
    BSTR value;
    Bstr(const wchar_t* text) : value(SysAllocString(text)) {}
    ~Bstr() { SysFreeString(value); }
    operator BSTR() const { return value; }
};
struct Wmi {
    com_ptr<IWbemServices> service;
    std::wstring error;
    bool Connect() {
        if (service)
            return true;
        com_ptr<IWbemLocator> locator;
        HRESULT result =
            CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                             IID_PPV_ARGS(locator.put()));
        if (FAILED(result)) {
            error = L"locator " + std::to_wstring((unsigned)result);
            return false;
        }
        result =
            locator->ConnectServer(Bstr(L"ROOT\\WMI"), nullptr, nullptr,
                                   nullptr, 0, nullptr, nullptr, service.put());
        if (FAILED(result)) {
            error = L"connect " + std::to_wstring((unsigned)result);
            return false;
        }
        CoSetProxyBlanket(service.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                          nullptr, RPC_C_AUTHN_LEVEL_CALL,
                          RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        return true;
    }
    com_ptr<IWbemClassObject> Query(const wchar_t* query) {
        if (!Connect())
            return nullptr;
        com_ptr<IEnumWbemClassObject> values;
        HRESULT result = service->ExecQuery(
            Bstr(L"WQL"), Bstr(query),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,
            values.put());
        if (FAILED(result)) {
            error = L"query " + std::to_wstring((unsigned)result);
            return nullptr;
        }
        com_ptr<IWbemClassObject> object;
        ULONG count = 0;
        result = values->Next(1500, 1, object.put(), &count);
        if (FAILED(result) || !count) {
            error = L"next " + std::to_wstring((unsigned)result) + L" count=" +
                    std::to_wstring(count);
            return nullptr;
        }
        error.clear();
        return object;
    }
    int ReadBrightness() {
        auto object = Query(
            L"SELECT CurrentBrightness FROM WmiMonitorBrightness WHERE "
            L"Active=TRUE");
        if (!object)
            return -1;
        VARIANT value;
        VariantInit(&value);
        int result = -1;
        if (SUCCEEDED(object->Get(L"CurrentBrightness", 0, &value, nullptr,
                                  nullptr))) {
            if (value.vt == VT_UI1)
                result = value.bVal;
            else if (value.vt == VT_I4)
                result = value.lVal;
            else if (value.vt == VT_UI4)
                result = value.ulVal;
            else if (value.vt == VT_I2)
                result = value.iVal;
            else if (value.vt == VT_UI2)
                result = value.uiVal;
            else
                error = L"brightness VARIANT " + std::to_wstring(value.vt);
        }
        VariantClear(&value);
        return result;
    }
    bool SetBrightness(int percent) {
        auto object = Query(
            L"SELECT * FROM WmiMonitorBrightnessMethods WHERE Active=TRUE");
        if (!object)
            return false;
        VARIANT path;
        VariantInit(&path);
        if (FAILED(object->Get(L"__PATH", 0, &path, nullptr, nullptr)))
            return false;
        com_ptr<IWbemClassObject> type, definition, input, output;
        HRESULT result =
            service->GetObject(Bstr(L"WmiMonitorBrightnessMethods"), 0, nullptr,
                               type.put(), nullptr);
        if (SUCCEEDED(result))
            result = type->GetMethod(L"WmiSetBrightness", 0, definition.put(),
                                     nullptr);
        if (SUCCEEDED(result))
            result = definition->SpawnInstance(0, input.put());
        if (SUCCEEDED(result)) {
            VARIANT timeout, level;
            VariantInit(&timeout);
            VariantInit(&level);
            timeout.vt = VT_BSTR;
            timeout.bstrVal = SysAllocString(L"0");
            level.vt = VT_UI1;
            level.bVal = (BYTE)percent;
            result = input->Put(L"Timeout", 0, &timeout, 0);
            if (SUCCEEDED(result))
                result = input->Put(L"Brightness", 0, &level, 0);
            if (SUCCEEDED(result))
                result = service->ExecMethod(
                    path.bstrVal, Bstr(L"WmiSetBrightness"), 0, nullptr,
                    input.get(), output.put(), nullptr);
            VariantClear(&timeout);
            VariantClear(&level);
        }
        VariantClear(&path);
        return SUCCEEDED(result);
    }
};
inline std::wstring BatteryDetail() {
    SYSTEM_POWER_STATUS power = {};
    GetSystemPowerStatus(&power);
    SYSTEM_BATTERY_STATE state = {};
    if (CallNtPowerInformation(SystemBatteryState, nullptr, 0, &state,
                               sizeof(state)) == 0 &&
        state.BatteryPresent && state.Discharging &&
        state.EstimatedTime != DWORD(-1) && state.EstimatedTime > 0 &&
        state.EstimatedTime < 7 * 86400)
        return L"预计剩余 " + Duration(state.EstimatedTime);
    if (power.ACLineStatus == 0 && power.BatteryLifeTime != DWORD(-1))
        return L"预计剩余 " + Duration(power.BatteryLifeTime);
    try {
        auto report =
            Windows::Devices::Power::Battery::AggregateBattery().GetReport();
        auto remaining = report.RemainingCapacityInMilliwattHours(),
             full = report.FullChargeCapacityInMilliwattHours(),
             rate = report.ChargeRateInMilliwatts();
        if (power.ACLineStatus == 1) {
            if (remaining && full && remaining.Value() >= full.Value() - 100)
                return L"已充满";
            if (remaining && full && rate && rate.Value() > 100)
                return L"充满剩余（估算） " +
                       Duration(3600.0 * (full.Value() - remaining.Value()) /
                                rate.Value());
            return L"充满剩余时间待估算";
        }
        if (power.BatteryLifeTime != DWORD(-1))
            return L"预计剩余 " + Duration(power.BatteryLifeTime);
        if (remaining && rate && rate.Value() < -100)
            return L"预计剩余（估算） " +
                   Duration(3600.0 * remaining.Value() / -rate.Value());
    } catch (...) {
    }
    return power.ACLineStatus == 1 ? L"充满剩余时间待估算"
                                   : L"预计剩余时间待估算";
}
struct NetworkSample {
    std::map<ULONG64, std::pair<ULONG64, ULONG64>> previous;
    ULONGLONG last = 0;
    void Read(Data& next) {
        MIB_IF_TABLE2* table = nullptr;
        auto now = GetTickCount64();
        double seconds = last ? (now - last) / 1000.0 : 0;
        last = now;
        next.download = next.upload = 0;
        if (GetIfTable2(&table) != NO_ERROR) {
            next.download = next.upload = -1;
            return;
        }
        std::map<ULONG64, std::pair<ULONG64, ULONG64>> current;
        for (ULONG i = 0; i < table->NumEntries; i++) {
            auto const& row = table->Table[i];
            if (row.OperStatus != IfOperStatusUp ||
                !row.InterfaceAndOperStatusFlags.HardwareInterface)
                continue;
            auto key = row.InterfaceLuid.Value;
            current[key] = {row.InOctets, row.OutOctets};
            auto old = previous.find(key);
            if (old != previous.end() && seconds > 0 &&
                row.InOctets >= old->second.first &&
                row.OutOctets >= old->second.second) {
                next.download += (row.InOctets - old->second.first) / seconds;
                next.upload += (row.OutOctets - old->second.second) / seconds;
            }
        }
        FreeMibTable(table);
        previous = std::move(current);
    }
};
inline void ReadNetworkName(Data& next) {
    try {
        auto profile = Windows::Networking::Connectivity::NetworkInformation::
            GetInternetConnectionProfile();
        if (profile) {
            next.networkName = profile.ProfileName();
            next.networkState =
                profile.GetNetworkConnectivityLevel() ==
                        Windows::Networking::Connectivity::
                            NetworkConnectivityLevel::InternetAccess
                    ? L"已连接 Internet"
                    : L"本地网络";
        } else {
            next.networkName = L"未连接";
            next.networkState = L"无网络连接";
        }
    } catch (...) {
        next.networkName = L"网络状态暂不可用";
    }
}
inline void ReadStorage(Data& next) {
    wchar_t drive[16]{};
    if (!GetEnvironmentVariableW(L"SystemDrive", drive, 16))
        lstrcpynW(drive, L"C:", 16);
    next.storageName = std::wstring(drive) + L" 系统盘";
    std::wstring root = std::wstring(drive) + L"\\";
    ULARGE_INTEGER free, total, available;
    if (GetDiskFreeSpaceExW(root.c_str(), &available, &total, &free)) {
        next.diskTotal = total.QuadPart / 1073741824.0;
        next.diskFree = free.QuadPart / 1073741824.0;
        next.diskUsed = next.diskTotal - next.diskFree;
    }
}
inline void MetricsWorker() {
    init_apartment(apartment_type::multi_threaded);
    Wmi wmi;
    NetworkSample network;
    com_ptr<IMMDeviceEnumerator> enumerator;
    com_ptr<IAudioEndpointVolume> endpoint;

    PDH_HQUERY gpuQuery = nullptr;
    PDH_HCOUNTER gpuCounter = nullptr;
    if (PdhOpenQueryW(nullptr, 0, &gpuQuery) == ERROR_SUCCESS) {
        if (PdhAddEnglishCounterW(gpuQuery,
                                  L"\\GPU Engine(*)\\Utilization Percentage", 0,
                                  &gpuCounter) != ERROR_SUCCESS)
            gpuCounter = nullptr;
        PdhCollectQueryData(gpuQuery);
    }
    FILETIME previousIdle = {}, previousKernel = {}, previousUser = {};
    bool previousCpu = false;
    auto ft = [](FILETIME value) {
        return ((uint64_t)value.dwHighDateTime << 32) | value.dwLowDateTime;
    };
    std::wstring track;
    uint64_t artVersion = 0, lastMetrics = 0, lastSlow = 0, lastMediaRetry = 0;
    {
        std::lock_guard lock(dataMutex);
        data.device = DeviceName();
    }
    while (!Stopped()) {
        auto settings = std::atomic_load(&config);
        auto now = GetTickCount64();
        if (settings->volume && !endpoint) {
            if (!enumerator)
                CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                 CLSCTX_INPROC_SERVER,
                                 IID_PPV_ARGS(enumerator.put()));
            if (enumerator) {
                com_ptr<IMMDevice> device;
                if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(
                        eRender, eMultimedia, device.put())))
                    device->Activate(__uuidof(IAudioEndpointVolume),
                                     CLSCTX_INPROC_SERVER, nullptr,
                                     endpoint.put_void());
            }
        }
        int request = volumeRequest.exchange(-1);
        if (request >= 0 && endpoint &&
            SUCCEEDED(endpoint->SetMasterVolumeLevelScalar(request / 100.0f,
                                                           nullptr))) {
            std::lock_guard lock(dataMutex);
            data.volume = request;
        }
        request = muteRequest.exchange(-1);
        if (request >= 0 && endpoint)
            endpoint->SetMute(request != 0, nullptr);
        request = brightnessRequest.exchange(-1);
        if (request >= 0 && wmi.SetBrightness(request)) {
            std::lock_guard lock(dataMutex);
            data.brightness = request;
            data.brightnessReady = true;
            lastSlow = now;
        }
        if (now - lastMetrics >= 1000) {
            lastMetrics = now;
            Data next = Snapshot();
            if (settings->network)
                network.Read(next);
            if (settings->performance) {
                FILETIME idle, kernel, user;
                if (GetSystemTimes(&idle, &kernel, &user)) {
                    uint64_t total = (ft(kernel) - ft(previousKernel)) +
                                     (ft(user) - ft(previousUser));
                    uint64_t free = ft(idle) - ft(previousIdle);
                    if (previousCpu && total)
                        next.cpu = std::clamp(
                            100.0 * (1.0 - (double)free / total), 0.0, 100.0);
                    previousIdle = idle;
                    previousKernel = kernel;
                    previousUser = user;
                    previousCpu = true;
                }
                if (gpuCounter &&
                    PdhCollectQueryData(gpuQuery) == ERROR_SUCCESS) {
                    DWORD bytes = 0, count = 0;
                    PdhGetFormattedCounterArrayW(gpuCounter, PDH_FMT_DOUBLE,
                                                 &bytes, &count, nullptr);
                    if (bytes) {
                        std::vector<BYTE> buffer(bytes);
                        auto items =
                            (PDH_FMT_COUNTERVALUE_ITEM_W*)buffer.data();
                        if (PdhGetFormattedCounterArrayW(
                                gpuCounter, PDH_FMT_DOUBLE, &bytes, &count,
                                items) == ERROR_SUCCESS) {
                            std::map<std::wstring, double> engines;
                            for (DWORD i = 0; i < count; i++) {
                                if (items[i].FmtValue.CStatus !=
                                        PDH_CSTATUS_VALID_DATA &&
                                    items[i].FmtValue.CStatus !=
                                        PDH_CSTATUS_NEW_DATA)
                                    continue;
                                std::wstring name = items[i].szName;
                                auto start = name.find(L"luid_");
                                if (start != std::wstring::npos)
                                    engines[name.substr(start)] +=
                                        items[i].FmtValue.doubleValue;
                            }
                            double busy = 0;
                            for (auto const& item : engines)
                                busy = std::max(busy, item.second);
                            next.gpu = engines.empty()
                                           ? -1
                                           : std::clamp(busy, 0.0, 100.0);
                        }
                    }
                }
            }
            if (endpoint) {
                float volume;
                BOOL mute;
                if (SUCCEEDED(endpoint->GetMasterVolumeLevelScalar(&volume))) {
                    next.volume = volume * 100;
                    endpoint->GetMute(&mute);
                    next.muted = mute;
                } else
                    endpoint = nullptr;
            }
            if (now - lastSlow >= 4000) {
                lastSlow = now;
                if (settings->network)
                    ReadNetworkName(next);
                if (settings->storage)
                    ReadStorage(next);
                if (settings->battery)
                    next.batteryDetail = BatteryDetail();
                if (settings->brightness) {
                    int value = wmi.ReadBrightness();
                    next.brightness = value;
                    next.brightnessReady = value >= 0;
                    next.brightnessError = wmi.error;
                }
            }
            {
                std::lock_guard lock(dataMutex);
                data.cpu = next.cpu;
                data.gpu = next.gpu;
                data.volume = next.volume;
                data.muted = next.muted;
                data.brightness = next.brightness;
                data.brightnessReady = next.brightnessReady;
                data.brightnessError = next.brightnessError;
                data.batteryDetail = next.batteryDetail;
                data.networkName = next.networkName;
                data.networkState = next.networkState;
                data.download = next.download;
                data.upload = next.upload;
                data.diskUsed = next.diskUsed;
                data.diskTotal = next.diskTotal;
                data.diskFree = next.diskFree;
                data.storageName = next.storageName;
            }
        }
        HANDLE events[] = {stop, wake};
        if (WaitForMultipleObjects(2, events, FALSE, visible ? 1000 : 5000) ==
            WAIT_OBJECT_0)
            break;
    }
    if (gpuQuery)
        PdhCloseQuery(gpuQuery);
}
inline std::string Http(std::wstring const& url) {
    URL_COMPONENTS parts = {};
    parts.dwStructSize = sizeof(parts);
    parts.dwHostNameLength = DWORD(-1);
    parts.dwUrlPathLength = DWORD(-1);
    parts.dwExtraInfoLength = DWORD(-1);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts) ||
        parts.nScheme != INTERNET_SCHEME_HTTPS)
        throw std::runtime_error("HTTPS weather URL required");
    std::wstring host(parts.lpszHostName, parts.dwHostNameLength),
        path(parts.lpszUrlPath, parts.dwUrlPathLength);
    if (parts.dwExtraInfoLength)
        path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
    HINTERNET agent =
        WinHttpOpen(L"StartSidecar/0.3", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!agent)
        throw std::runtime_error("weather connection failed");
    WinHttpSetTimeouts(agent, 2000, 2000, 3000, 3000);
    auto connection = WinHttpConnect(agent, host.c_str(), parts.nPort, 0);
    auto request = connection
                       ? WinHttpOpenRequest(connection, L"GET", path.c_str(),
                                            nullptr, WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            WINHTTP_FLAG_SECURE)
                       : nullptr;
    std::string body;
    bool ok = request &&
              WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                 nullptr, 0, 0, 0) &&
              WinHttpReceiveResponse(request, nullptr);
    if (ok) {
        DWORD status = 0, length = sizeof(status);
        WinHttpQueryHeaders(
            request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &status, &length,
            WINHTTP_NO_HEADER_INDEX);
        ok = status == 200;
    }
    while (ok && !Stopped()) {
        DWORD available = 0;
        if (!WinHttpQueryDataAvailable(request, &available)) {
            ok = false;
            break;
        }
        if (!available)
            break;
        if (body.size() + available > 1024 * 1024) {
            ok = false;
            break;
        }
        size_t offset = body.size();
        body.resize(offset + available);
        DWORD read = 0;
        if (!WinHttpReadData(request, body.data() + offset, available, &read)) {
            ok = false;
            break;
        }
        body.resize(offset + read);
    }
    if (request)
        WinHttpCloseHandle(request);
    if (connection)
        WinHttpCloseHandle(connection);
    WinHttpCloseHandle(agent);
    if (!ok)
        throw std::runtime_error("weather request failed");
    return body;
}
inline std::wstring WeatherCode(int code) {
    if (code == 0)
        return L"晴";
    if (code <= 3)
        return L"多云";
    if (code == 45 || code == 48)
        return L"雾";
    if (code >= 51 && code <= 57)
        return L"毛毛雨";
    if (code >= 61 && code <= 67)
        return L"雨";
    if (code >= 71 && code <= 77)
        return L"雪";
    if (code >= 80 && code <= 82)
        return L"阵雨";
    if (code >= 85 && code <= 86)
        return L"阵雪";
    if (code >= 95)
        return L"雷雨";
    return L"天气信息";
}
inline std::wstring Encode(std::wstring const& value) {
    auto utf8 = to_string(value);
    std::wstring result;
    const wchar_t* hex = L"0123456789ABCDEF";
    for (unsigned char ch : utf8) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.')
            result.push_back(ch);
        else {
            result.push_back('%');
            result.push_back(hex[ch >> 4]);
            result.push_back(hex[ch & 15]);
        }
    }
    return result;
}
inline JsonObject Object(JsonObject const& root, const wchar_t* key) {
    auto value = root.GetNamedValue(key, JsonValue::CreateNullValue());
    if (value.ValueType() == JsonValueType::Object)
        return value.GetObject();
    if (value.ValueType() == JsonValueType::Array && value.GetArray().Size())
        return value.GetArray().GetObjectAt(0);
    return JsonObject();
}
inline void WeatherWorker() {
    init_apartment(apartment_type::multi_threaded);
    std::wstring lastCity, latitude, longitude;
    while (!Stopped()) {
        auto settings = std::atomic_load(&config);
        DWORD delay = 600000;
        if (settings->weather)
            try {
                std::wstring url = settings->customUrl;
                if (settings->provider != L"custom") {
                    if (!settings->latitude.empty() &&
                        !settings->longitude.empty()) {
                        latitude = settings->latitude;
                        longitude = settings->longitude;
                        lastCity = settings->city;
                    } else if (lastCity != settings->city || latitude.empty()) {
                        {
                            auto location = JsonObject::Parse(to_hstring(
                                Http(L"https://geocoding-api.open-meteo.com/v1/"
                                     L"search?name=" +
                                     Encode(settings->city) +
                                     L"&count=1&language=zh&format=json")));
                            auto values = location.GetNamedArray(L"results");
                            if (!values.Size())
                                throw std::runtime_error("city not found");
                            auto item = values.GetObjectAt(0);
                            latitude = to_hstring(std::to_string(
                                item.GetNamedNumber(L"latitude")));
                            longitude = to_hstring(std::to_string(
                                item.GetNamedNumber(L"longitude")));
                        }
                        lastCity = settings->city;
                    }
                    url = L"https://api.open-meteo.com/v1/forecast?latitude=" +
                          latitude + L"&longitude=" + longitude +
                          L"&current=temperature_2m,apparent_temperature,"
                          L"relative_humidity_2m,weather_code&timezone=auto&"
                          L"forecast_days=1";
                }
                if (url.empty())
                    throw std::runtime_error("missing weather URL");
                auto json = JsonObject::Parse(to_hstring(Http(url)));
                double temperature = NAN, feels = NAN, humidity = NAN;
                std::wstring condition, source;
                if (json.HasKey(L"current")) {
                    auto current = Object(json, L"current");
                    temperature =
                        current.GetNamedNumber(L"temperature_2m", NAN);
                    feels =
                        current.GetNamedNumber(L"apparent_temperature", NAN);
                    humidity =
                        current.GetNamedNumber(L"relative_humidity_2m", NAN);
                    condition = WeatherCode(
                        (int)current.GetNamedNumber(L"weather_code", 0));
                    source = L"Open-Meteo";
                } else if (json.HasKey(L"responses")) {
                    auto current =
                        Object(Object(Object(json, L"responses"), L"weather"),
                               L"current");
                    temperature = current.GetNamedNumber(L"temp", NAN);
                    feels = current.GetNamedNumber(L"feels", NAN);
                    humidity = current.GetNamedNumber(L"rh", NAN);
                    condition = current.GetNamedString(L"cap", L"天气信息");
                    source = L"MSN Weather";
                } else {
                    temperature = json.GetNamedNumber(L"temperature", NAN);
                    feels = json.GetNamedNumber(L"feels_like", NAN);
                    humidity = json.GetNamedNumber(L"humidity", NAN);
                    condition = json.GetNamedString(L"condition", L"天气信息");
                    source = json.GetNamedString(L"source", L"自定义数据源");
                }
                if (!std::isfinite(temperature))
                    throw std::runtime_error(
                        "weather response format unsupported");
                SYSTEMTIME now;
                GetLocalTime(&now);
                wchar_t time[32];
                swprintf(time, 32, L"%02u:%02u", now.wHour, now.wMinute);
                std::lock_guard lock(dataMutex);
                data.temperature = temperature;
                data.feels = feels;
                data.humidity = humidity;
                data.weatherCondition = condition;
                data.weatherSource = source;
                data.weatherTime = time;
                data.weatherError.clear();
            } catch (...) {
                delay = 60000;
                std::lock_guard lock(dataMutex);
                data.weatherError = L"天气暂不可用";
                if (!std::isfinite(data.temperature))
                    data.weatherCondition = L"天气暂不可用";
            }
        HANDLE events[] = {stop, weatherWake};
        if (WaitForMultipleObjects(2, events, FALSE, delay) == WAIT_OBJECT_0)
            break;
    }
}
inline void MediaWorker();
inline void Start(HANDLE stopEvent) {
    stop = stopEvent;
    wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    weatherWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    mediaWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    mediaWorker = std::thread([] {
        try {
            MediaWorker();
        } catch (...) {
            std::lock_guard lock(dataMutex);
            data.mediaError = L"媒体服务异常";
        }
    });
    worker = std::thread([] {
        try {
            MetricsWorker();
        } catch (hresult_error const& ex) {
            std::lock_guard lock(dataMutex);
            data.mediaError =
                L"系统数据服务异常 " + std::to_wstring((unsigned)ex.code());
        } catch (...) {
            std::lock_guard lock(dataMutex);
            data.mediaError = L"系统数据服务异常";
        }
    });
    weatherWorker = std::thread([] {
        try {
            WeatherWorker();
        } catch (...) {
            std::lock_guard lock(dataMutex);
            data.weatherError = L"天气服务不可用";
        }
    });
}
inline void Stop() {
    if (stop)
        SetEvent(stop);
    if (wake)
        SetEvent(wake);
    if (weatherWake)
        SetEvent(weatherWake);
    if (mediaWake)
        SetEvent(mediaWake);
    if (mediaWorker.joinable())
        mediaWorker.join();
    if (worker.joinable())
        worker.join();
    if (weatherWorker.joinable())
        weatherWorker.join();
    if (wake)
        CloseHandle(wake);
    if (weatherWake)
        CloseHandle(weatherWake);
    if (mediaWake)
        CloseHandle(mediaWake);
    mediaWake = nullptr;
    wake = nullptr;
    weatherWake = nullptr;
}
}  // namespace svc

#include <appmodel.h>
#include <tlhelp32.h>
#include <shobjidl.h>
#include <knownfolders.h>
#include <filesystem>
namespace svc {
static std::wstring Lower(std::wstring text) {
    std::transform(text.begin(), text.end(), text.begin(), ::towlower);
    return text;
}
static std::wstring ExecutableName(std::wstring const& app) {
    auto lower = Lower(app);
    if (lower.find(L"neteasenext") != std::wstring::npos)
        return L"NetEaseNEXT.exe";
    if (lower.find(L"cloudmusic") != std::wstring::npos ||
        lower.find(L"netease") != std::wstring::npos)
        return L"cloudmusic.exe";
    if (lower.find(L"spotify") != std::wstring::npos)
        return L"spotify.exe";
    if (lower.find(L"qqmusic") != std::wstring::npos)
        return L"qqmusic.exe";
    if (lower.find(L"msedge") != std::wstring::npos)
        return L"msedge.exe";
    if (lower.find(L"chrome") != std::wstring::npos)
        return L"chrome.exe";
    if (lower.find(L"firefox") != std::wstring::npos)
        return L"firefox.exe";
    if (lower.find(L"foobar") != std::wstring::npos)
        return L"foobar2000.exe";
    if (lower.size() > 4 && lower.ends_with(L".exe"))
        return std::filesystem::path(lower).filename().wstring();
    return L"";
}
static std::vector<std::pair<DWORD, std::wstring>> SourceProcesses(
    std::wstring const& app) {
    std::vector<std::pair<DWORD, std::wstring>> result;
    auto name = ExecutableName(app);
    bool packaged = app.find(L'!') != std::wstring::npos;
    if (name.empty() && !packaged)
        return result;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return result;
    PROCESSENTRY32W process{sizeof(process)};
    if (Process32FirstW(snapshot, &process))
        do {
            bool match =
                !name.empty() && _wcsicmp(process.szExeFile, name.c_str()) == 0;
            if (!match && !packaged)
                continue;
            HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                        FALSE, process.th32ProcessID);
            if (!handle)
                continue;
            if (packaged) {
                UINT32 length = 0;
                if (GetApplicationUserModelId(handle, &length, nullptr) ==
                        ERROR_INSUFFICIENT_BUFFER &&
                    length) {
                    std::vector<wchar_t> identity(length);
                    if (GetApplicationUserModelId(
                            handle, &length, identity.data()) == ERROR_SUCCESS)
                        match = _wcsicmp(identity.data(), app.c_str()) == 0;
                }
            }
            if (match) {
                wchar_t path[32768];
                DWORD length = ARRAYSIZE(path);
                if (QueryFullProcessImageNameW(handle, 0, path, &length))
                    result.emplace_back(process.th32ProcessID,
                                        std::wstring(path, length));
            }
            CloseHandle(handle);
        } while (Process32NextW(snapshot, &process));
    CloseHandle(snapshot);
    // Packaged WebView/helper processes can share the app identity. Put the
    // known main executable first for icon fallback and source activation.
    if (!name.empty())
        std::stable_partition(
            result.begin(), result.end(), [&](auto const& process) {
                return _wcsicmp(std::filesystem::path(process.second)
                                    .filename()
                                    .c_str(),
                                name.c_str()) == 0;
            });
    return result;
}
static bool OpenSource(std::wstring const& app) {
    if (app.empty())
        return false;
    auto processes = SourceProcesses(app);
    struct Context {
        std::vector<std::pair<DWORD, std::wstring>> const* processes;
        HWND window = nullptr;
    };
    Context context{&processes};
    EnumWindows(
        [](HWND window, LPARAM parameter) CALLBACK -> BOOL {
            auto& context = *reinterpret_cast<Context*>(parameter);
            if (!IsWindowVisible(window) || GetWindow(window, GW_OWNER) ||
                (GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_TOOLWINDOW))
                return TRUE;
            DWORD pid = 0;
            GetWindowThreadProcessId(window, &pid);
            for (auto const& item : *context.processes)
                if (item.first == pid) {
                    context.window = window;
                    return FALSE;
                }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    if (context.window) {
        ShowWindowAsync(context.window,
                        IsIconic(context.window) ? SW_RESTORE : SW_SHOW);
        if (SetForegroundWindow(context.window))
            return true;
    }
    if (app.find(L'!') != std::wstring::npos) {
        com_ptr<::IApplicationActivationManager> activation;
        if (SUCCEEDED(CoCreateInstance(CLSID_ApplicationActivationManager,
                                       nullptr, CLSCTX_INPROC_SERVER,
                                       IID_IApplicationActivationManager,
                                       activation.put_void()))) {
            DWORD pid = 0;
            if (SUCCEEDED(activation->ActivateApplication(app.c_str(), nullptr,
                                                          AO_NOERRORUI, &pid)))
                return true;
        }
    }
    com_ptr<::IShellItem> shellItem;
    if (SUCCEEDED(SHCreateItemInKnownFolder(FOLDERID_AppsFolder, 0, app.c_str(),
                                            IID_IShellItem,
                                            shellItem.put_void()))) {
        PIDLIST_ABSOLUTE id = nullptr;
        if (SUCCEEDED(SHGetIDListFromObject(shellItem.get(), &id))) {
            SHELLEXECUTEINFOW execute{sizeof(execute)};
            execute.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
            execute.lpIDList = id;
            execute.nShow = SW_SHOWNORMAL;
            bool ok = ShellExecuteExW(&execute) != FALSE;
            CoTaskMemFree(id);
            if (ok)
                return true;
        }
    }
    // Only launch a path obtained from an already-running source process.
    if (!processes.empty())
        return reinterpret_cast<INT_PTR>(ShellExecuteW(
                   nullptr, L"open", processes.front().second.c_str(), nullptr,
                   nullptr, SW_SHOWNORMAL)) > 32;
    return context.window != nullptr;
}

struct CoverRead {
    Windows::Foundation::IAsyncOperation<IRandomAccessStreamWithContentType>
        opening{nullptr};
    Windows::Foundation::IAsyncOperationWithProgress<IBuffer, uint32_t> reading{
        nullptr};
    IRandomAccessStream stream{nullptr};
    ULONGLONG deadline = 0;
    uint64_t hash = 0;
    bool Pending() const { return bool(opening) || bool(reading); }
    void Cancel() {
        if (opening)
            try {
                opening.Cancel();
            } catch (...) {
            }
        if (reading)
            try {
                reading.Cancel();
            } catch (...) {
            }
        opening = nullptr;
        reading = nullptr;
        stream = nullptr;
    }
    void Start(IRandomAccessStreamReference const& reference) {
        Cancel();
        if (reference) {
            opening = reference.OpenReadAsync();
            deadline = GetTickCount64() + 4000;
        }
    }
    bool Poll(Data& next) {
        if (Pending() && GetTickCount64() > deadline) {
            Cancel();
            return false;
        }
        if (opening) {
            auto status = opening.Status();
            if (status == Windows::Foundation::AsyncStatus::Completed) {
                stream = opening.GetResults();
                opening = nullptr;
                auto size = stream.Size();
                if (!size || size > 8 * 1024 * 1024) {
                    Cancel();
                    return false;
                }
                reading = stream.GetInputStreamAt(0).ReadAsync(
                    Buffer((uint32_t)size), (uint32_t)size,
                    InputStreamOptions::None);
            } else if (status != Windows::Foundation::AsyncStatus::Started) {
                Cancel();
                return false;
            }
        }
        if (reading) {
            auto status = reading.Status();
            if (status == Windows::Foundation::AsyncStatus::Completed) {
                auto buffer = reading.GetResults();
                reading = nullptr;
                std::vector<uint8_t> bytes(buffer.Length());
                DataReader::FromBuffer(buffer).ReadBytes(bytes);
                uint64_t value = 1469598103934665603ULL;
                for (auto byte : bytes) {
                    value ^= byte;
                    value *= 1099511628211ULL;
                }
                if (value != hash || !next.art) {
                    hash = value;
                    next.art = stream.CloneStream();
                    ++next.artRevision;
                }
                stream = nullptr;
                return true;
            } else if (status != Windows::Foundation::AsyncStatus::Started)
                Cancel();
        }
        return false;
    }
};
inline void MediaWorker() {
    init_apartment(apartment_type::multi_threaded);
    using Props = GlobalSystemMediaTransportControlsSessionMediaProperties;
    using PropsOperation = Windows::Foundation::IAsyncOperation<Props>;
    using ImageOperation = Windows::Foundation::IAsyncOperation<
        IRandomAccessStreamWithContentType>;
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    GlobalSystemMediaTransportControlsSession watched{nullptr};
    event_token managerEvent{}, playbackEvent{}, propertiesEvent{};
    auto dirty = std::make_shared<std::atomic<bool>>(true);
    auto signal = [dirty](auto const&, auto const&) {
        *dirty = true;
        if (mediaWake)
            SetEvent(mediaWake);
    };
    auto playbackSignal = [](auto const&, auto const&) {
        if (mediaWake)
            SetEvent(mediaWake);
    };
    PropsOperation properties{nullptr};
    ImageOperation logo{nullptr};
    CoverRead artwork;
    ULONGLONG retry = 0, propertiesStarted = 0, metadataNext = 0,
              followUpUntil = 0;
    std::wstring source, artworkKey;
    bool artForTrack = false;
    auto detach = [&] {
        if (watched) {
            try {
                watched.PlaybackInfoChanged(playbackEvent);
                watched.MediaPropertiesChanged(propertiesEvent);
            } catch (...) {
            }
            watched = nullptr;
        }
        if (properties) {
            try {
                properties.Cancel();
            } catch (...) {
            }
            properties = nullptr;
        }
        artwork.Cancel();
        artwork.hash = 0;
        if (logo) {
            try {
                logo.Cancel();
            } catch (...) {
            }
            logo = nullptr;
        }
    };
    while (!Stopped()) {
        auto settings = std::atomic_load(&config);
        auto now = GetTickCount64();
        if (!settings->media) {
            detach();
            HANDLE events[] = {stop, mediaWake};
            if (WaitForMultipleObjects(2, events, FALSE, 1000) == WAIT_OBJECT_0)
                break;
            continue;
        }
        if (!manager && now - retry >= 5000) {
            retry = now;
            try {
                manager =
                    Await(GlobalSystemMediaTransportControlsSessionManager::
                              RequestAsync());
                managerEvent = manager.CurrentSessionChanged(signal);
            } catch (hresult_error const& ex) {
                std::lock_guard lock(dataMutex);
                data.mediaError = L"系统媒体接口不可用 (" +
                                  std::to_wstring((unsigned)ex.code()) + L")";
            }
        }
        Data next = Snapshot();
        GlobalSystemMediaTransportControlsSession session{nullptr};
        if (manager)
            try {
                session = manager.GetCurrentSession();
                next.mediaReady = true;
                next.mediaError.clear();
            } catch (...) {
                try {
                    manager.CurrentSessionChanged(managerEvent);
                } catch (...) {
                }
                manager = nullptr;
            }
        try {
            if (session != watched) {
                detach();
                watched = session;
                *dirty = true;
                artworkKey.clear();
                source = session ? std::wstring(session.SourceAppUserModelId())
                                 : L"";
                next.mediaApp = source;
                next.art = nullptr;
                next.mediaLogo = nullptr;
                ++next.artRevision;
                if (watched) {
                    playbackEvent = watched.PlaybackInfoChanged(playbackSignal);
                    propertiesEvent = watched.MediaPropertiesChanged(signal);
                    next.mediaSource = source;
                    auto slash = next.mediaSource.find_last_of(L"\\/");
                    if (slash != std::wstring::npos)
                        next.mediaSource.erase(0, slash + 1);
                    if (next.mediaSource.size() > 4 &&
                        _wcsicmp(next.mediaSource.c_str() +
                                     next.mediaSource.size() - 4,
                                 L".exe") == 0)
                        next.mediaSource.resize(next.mediaSource.size() - 4);
                    try {
                        auto app = Windows::ApplicationModel::AppInfo::
                            GetFromAppUserModelId(source);
                        if (app) {
                            auto display = app.DisplayInfo();
                            auto name = display.DisplayName();
                            if (!name.empty())
                                next.mediaSource = name;
                            logo = display.GetLogo({32, 32}).OpenReadAsync();
                        }
                    } catch (...) {
                    }
                }
            }
            int command = mediaRequest.exchange(0);
            if (watched) {
                auto info = watched.GetPlaybackInfo();
                auto controls = info.Controls();
                next.playing =
                    info.PlaybackStatus() ==
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                        Playing;
                next.previous = controls.IsPreviousEnabled();
                next.next = controls.IsNextEnabled();
                next.toggle = controls.IsPlayPauseToggleEnabled() ||
                              (next.playing ? controls.IsPauseEnabled()
                                            : controls.IsPlayEnabled());
                if (command) {
                    Windows::Foundation::IAsyncOperation<bool> action{nullptr};
                    if (command == 1)
                        action = watched.TrySkipPreviousAsync();
                    else if (command == 2) {
                        if (controls.IsPlayPauseToggleEnabled())
                            action = watched.TryTogglePlayPauseAsync();
                        else if (next.playing)
                            action = watched.TryPauseAsync();
                        else
                            action = watched.TryPlayAsync();
                    } else if (command == 3)
                        action = watched.TrySkipNextAsync();
                    else if (command == 4)
                        OpenSource(source);
                }
                if (!properties &&
                    (dirty->exchange(false) || now >= metadataNext)) {
                    properties = watched.TryGetMediaPropertiesAsync();
                    propertiesStarted = now;
                    metadataNext = now + 5000;
                }
                if (properties) {
                    auto status = properties.Status();
                    if (status == Windows::Foundation::AsyncStatus::Completed) {
                        auto value = properties.GetResults();
                        properties = nullptr;
                        next.title = value.Title();
                        if (next.title.empty())
                            next.title = L"未提供标题";
                        next.artist = value.Artist();
                        auto key = source + L"|" + next.title + L"|" +
                                   next.artist + L"|" +
                                   std::wstring(value.AlbumTitle());
                        if (key != artworkKey) {
                            artworkKey = key;
                            artwork.Cancel();
                            artForTrack = false;
                            followUpUntil = now + 10000;
                            metadataNext = now + 500;
                        } else if (now < followUpUntil)
                            metadataNext = now + 1000;
                        if (value.Thumbnail()) {
                            if (!artwork.Pending())
                                artwork.Start(value.Thumbnail());
                        } else if (!artForTrack && now >= followUpUntil &&
                                   next.art) {
                            next.art = nullptr;
                            ++next.artRevision;
                        }
                    } else if (status !=
                                   Windows::Foundation::AsyncStatus::Started ||
                               now - propertiesStarted > 4000) {
                        try {
                            properties.Cancel();
                        } catch (...) {
                        }
                        properties = nullptr;
                    }
                }
                auto image = [&](ImageOperation& operation,
                                 IRandomAccessStream& destination) {
                    if (!operation)
                        return;
                    auto status = operation.Status();
                    if (status == Windows::Foundation::AsyncStatus::Completed) {
                        destination = operation.GetResults();
                        operation = nullptr;
                        ++next.artRevision;
                    } else if (status !=
                               Windows::Foundation::AsyncStatus::Started)
                        operation = nullptr;
                };
                if (artwork.Poll(next))
                    artForTrack = true;
                image(logo, next.mediaLogo);
            } else {
                next.title = L"暂无媒体播放";
                next.artist = L"";
                next.mediaSource = L"媒体播放";
                next.mediaApp.clear();
                next.playing = next.previous = next.next = next.toggle = false;
            }
        } catch (hresult_error const& ex) {
            next.mediaError = L"媒体状态暂不可用 (" +
                              std::to_wstring((unsigned)ex.code()) + L")";
        }
        {
            std::lock_guard lock(dataMutex);
            data.title = next.title;
            data.artist = next.artist;
            data.mediaSource = next.mediaSource;
            data.mediaApp = next.mediaApp;
            data.mediaReady = next.mediaReady;
            data.mediaError = next.mediaError;
            data.playing = next.playing;
            data.previous = next.previous;
            data.next = next.next;
            data.toggle = next.toggle;
            data.art = next.art;
            data.mediaLogo = next.mediaLogo;
            data.artRevision = next.artRevision;
        }
        HANDLE events[] = {stop, mediaWake};
        if (WaitForMultipleObjects(2, events, FALSE, visible ? 200 : 1000) ==
            WAIT_OBJECT_0)
            break;
    }
    detach();
    if (manager)
        try {
            manager.CurrentSessionChanged(managerEvent);
        } catch (...) {
        }
}
}  // namespace svc

using namespace winrt;
namespace x = Windows::UI::Xaml;
namespace c = Windows::UI::Xaml::Controls;
namespace m = Windows::UI::Xaml::Media;
namespace f = Windows::Foundation;
namespace core = Windows::UI::Core;
static std::atomic<bool> g_unloading = false;
static HWND g_hwnd;
static HANDLE g_startup, g_stop;
static constexpr UINT_PTR kReadyTimer = 0x53435841;
static constexpr wchar_t kCallMessage[] = L"Local.StartSidecarXaml.UiCall.020";
static core::CoreWindow g_core{nullptr};
static event_token g_visible{}, g_activated{};
static x::DispatcherTimer g_update{nullptr};
static c::Border g_card{nullptr};
static c::Grid g_host{nullptr};
static std::vector<c::ColumnDefinition> g_columns;
static std::vector<c::ColumnDefinition> g_addedColumns;
static c::TextBlock g_clock{nullptr}, g_date{nullptr}, g_battery{nullptr},
    g_power{nullptr}, g_memory{nullptr}, g_available{nullptr};
static c::TextBlock g_title{nullptr}, g_powerSource{nullptr}, g_cpu{nullptr},
    g_gpu{nullptr};
static bool g_layoutReady = false;
static x::DependencyObject g_layoutRoot{nullptr};

struct SavedProperty {
    x::FrameworkElement element{nullptr};
    x::DependencyProperty property{nullptr};
    f::IInspectable value{nullptr};
    x::Data::Binding binding{nullptr};
};
static std::vector<SavedProperty> g_saved;
static void RemoveBottom();
static void UpdateBottom(svc::Data const&);
static void BuildBottom(x::DependencyObject const&, c::Border const&);
static std::wstring StringSetting(const wchar_t* key) {
    auto value = Wh_GetStringSetting(key);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}
static void LoadSettings() {
    auto settings = std::make_shared<svc::Config>();
    settings->overview = Wh_GetIntSetting(L"showOverview") != 0;
    settings->time = Wh_GetIntSetting(L"showTime") != 0;
    settings->battery = Wh_GetIntSetting(L"showBattery") != 0;
    settings->memory = Wh_GetIntSetting(L"showMemory") != 0;
    settings->performance = Wh_GetIntSetting(L"showPerformance") != 0;
    settings->media = Wh_GetIntSetting(L"showMedia") != 0;
    settings->weather = Wh_GetIntSetting(L"showWeather") != 0;
    settings->quick = Wh_GetIntSetting(L"showQuickSettings") != 0;
    settings->volume = Wh_GetIntSetting(L"showVolume") != 0;
    settings->brightness = Wh_GetIntSetting(L"showBrightness") != 0;
    settings->network = Wh_GetIntSetting(L"showNetwork") != 0;
    settings->storage = Wh_GetIntSetting(L"showStorage") != 0;
    int width = Wh_GetIntSetting(L"overviewWidth");
    settings->overviewWidth = width >= 200 && width <= 340 ? width : 252;
    settings->cardGap = std::clamp(Wh_GetIntSetting(L"cardGap"), -1, 24);
    settings->title = StringSetting(L"deviceTitle");
    auto city = StringSetting(L"weatherCity");
    if (!city.empty())
        settings->city = city;
    settings->latitude = StringSetting(L"weatherLatitude");
    settings->longitude = StringSetting(L"weatherLongitude");
    auto provider = StringSetting(L"weatherProvider");
    if (!provider.empty())
        settings->provider = provider;
    settings->customUrl = StringSetting(L"weatherCustomUrl");
    svc::Configure(settings);
}

static void State(std::wstring const& value) {
    Wh_Log(L"%s", value.c_str());
}
static x::FrameworkElement Find(x::DependencyObject const& root,
                                std::wstring const& name,
                                int depth = 40) {
    if (!root || depth < 0)
        return nullptr;
    if (auto element = root.try_as<x::FrameworkElement>();
        element && element.Name() == name)
        return element;
    int count = m::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++)
        if (auto found =
                Find(m::VisualTreeHelper::GetChild(root, i), name, depth - 1))
            return found;
    return nullptr;
}
static x::FrameworkElement Resolve(x::DependencyObject root,
                                   std::wstring path) {
    size_t start = 0;
    for (;;) {
        size_t slash = path.find(L'/', start);
        auto result = Find(root, path.substr(start, slash == std::wstring::npos
                                                        ? slash
                                                        : slash - start));
        if (!result)
            return nullptr;
        if (slash == std::wstring::npos)
            return result;
        root = result;
        start = slash + 1;
    }
}
static std::wstring Number(double value) {
    if (std::isnan(value))
        return L"auto";
    wchar_t text[48];
    swprintf(text, 48, L"%.1f", value);
    return text;
}
static void Save(x::FrameworkElement const& element,
                 x::DependencyProperty const& property) {
    for (auto const& old : g_saved)
        if (old.element == element && old.property == property)
            return;
    SavedProperty saved;
    saved.element = element;
    saved.property = property;
    saved.value = element.ReadLocalValue(property);
    if (auto expression = element.GetBindingExpression(property))
        saved.binding = expression.ParentBinding();
    g_saved.push_back(saved);
}
static void SetWidth(x::FrameworkElement const& element, double value) {
    if (!element)
        return;
    Save(element, x::FrameworkElement::WidthProperty());
    Save(element, x::FrameworkElement::MaxWidthProperty());
    element.MaxWidth(INFINITY);
    element.Width(value);
}
static void RemoveCard() {
    g_layoutReady = false;
    g_layoutRoot = nullptr;
    try {
        if (g_update)
            g_update.Stop();
    } catch (...) {
    }
    RemoveBottom();
    try {
        if (g_host && g_card) {
            uint32_t at;
            if (g_host.Children().IndexOf(g_card, at))
                g_host.Children().RemoveAt(at);
        }
        if (g_host) {
            for (auto const& column : g_addedColumns) {
                uint32_t at;
                if (g_host.ColumnDefinitions().IndexOf(column, at))
                    g_host.ColumnDefinitions().RemoveAt(at);
            }
        }
    } catch (...) {
    }
    for (auto it = g_saved.rbegin(); it != g_saved.rend(); ++it)
        try {
            if (it->binding)
                x::Data::BindingOperations::SetBinding(
                    it->element, it->property, it->binding);
            else if (it->value == x::DependencyProperty::UnsetValue())
                it->element.ClearValue(it->property);
            else
                it->element.SetValue(it->property, it->value);
        } catch (...) {
        }
    g_saved.clear();
    g_addedColumns.clear();
    g_columns.clear();
    g_card = nullptr;
    g_host = nullptr;
    g_clock = nullptr;
    g_date = nullptr;
    g_battery = nullptr;
    g_power = nullptr;
    g_memory = nullptr;
    g_available = nullptr;
    g_title = nullptr;
    g_powerSource = nullptr;
    g_cpu = nullptr;
    g_gpu = nullptr;
}
static void UpdateData() {
    auto snapshot = svc::Snapshot();
    UpdateBottom(snapshot);
    if (!g_card || !g_clock)
        return;
    auto settings = std::atomic_load(&svc::config);
    g_title.Text(settings->title.empty() ? snapshot.device : settings->title);
    SYSTEMTIME now;
    GetLocalTime(&now);
    wchar_t clock[32], date[96], buffer[96];
    swprintf(clock, 32, L"%02u:%02u", now.wHour, now.wMinute);
    GetDateFormatEx(L"zh-CN", 0, &now, L"M月d日 dddd", date, 96, nullptr);
    g_clock.Text(clock);
    g_date.Text(date);
    SYSTEM_POWER_STATUS power = {};
    if (GetSystemPowerStatus(&power) && power.BatteryLifePercent != 255) {
        if (power.BatteryFlag & 128)
            g_battery.Text(L"无电池");
        else {
            swprintf(buffer, 96, L"%u%%", power.BatteryLifePercent);
            g_battery.Text(buffer);
        }
        g_powerSource.Text(power.ACLineStatus == 1   ? L"AC"
                           : power.ACLineStatus == 0 ? L"DC"
                                                     : L"—");
        g_power.Text(snapshot.batteryDetail);
    } else {
        g_battery.Text(L"—");
        g_powerSource.Text(L"—");
        g_power.Text(L"剩余时间待估算");
    }
    MEMORYSTATUSEX memory = {};
    memory.dwLength = sizeof(memory);
    if (GlobalMemoryStatusEx(&memory)) {
        swprintf(buffer, 96, L"%u%%", memory.dwMemoryLoad);
        g_memory.Text(buffer);
        swprintf(buffer, 96, L"已使用 %.1f / %.1f GB",
                 (memory.ullTotalPhys - memory.ullAvailPhys) / 1073741824.0,
                 memory.ullTotalPhys / 1073741824.0);
        g_available.Text(buffer);
    } else {
        g_memory.Text(L"—");
        g_available.Text(L"内存状态未知");
    }
    if (snapshot.cpu >= 0) {
        swprintf(buffer, 96, L"CPU %.0f%%", snapshot.cpu);
        g_cpu.Text(buffer);
    } else
        g_cpu.Text(L"CPU —");
    if (snapshot.gpu >= 0) {
        swprintf(buffer, 96, L"GPU %.0f%%", snapshot.gpu);
        g_gpu.Text(buffer);
    } else
        g_gpu.Text(L"GPU —");
}
static constexpr wchar_t kMarkup[] =
    LR"XAML(
<Border xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" x:Name="StartSidecarXaml"
 Width="252" MinHeight="500" Padding="18" CornerRadius="12"
 Background="{ThemeResource SystemControlBackgroundChromeMediumLowBrush}"
 BorderBrush="#287F7F7F" BorderThickness="1">
 <Grid>
  <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="*"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
  <StackPanel>
   <TextBlock x:Name="DeviceTitle" Text="此电脑" FontSize="20" FontWeight="SemiBold" Margin="0,0,0,18"/>
   <Border x:Name="TimeTile" Background="#254C87C1" CornerRadius="10" Padding="14" Margin="0,0,0,12">
    <StackPanel><TextBlock Text="时间" FontSize="12" Opacity="0.7"/>
     <TextBlock x:Name="ClockValue" FontSize="43" FontWeight="SemiBold" Margin="0,5,0,6"/>
     <TextBlock x:Name="DateValue" FontSize="12"/></StackPanel>
   </Border>
   <Border x:Name="BatteryTile" Background="#205A9D73" CornerRadius="10" Padding="14" Margin="0,0,0,12">
    <StackPanel><TextBlock Text="电池" FontSize="12" Opacity="0.7"/>
     <StackPanel Orientation="Horizontal" Margin="0,5,0,3"><TextBlock x:Name="BatteryValue" FontSize="29" FontWeight="SemiBold"/><TextBlock x:Name="PowerSourceValue" FontSize="14" Margin="10,0,0,0" VerticalAlignment="Center" Opacity="0.75"/></StackPanel>
     <TextBlock x:Name="PowerValue" FontSize="12"/></StackPanel>
   </Border>
   <Border x:Name="MemoryTile" Background="#257F60AD" CornerRadius="10" Padding="14" Margin="0,0,0,12">
    <StackPanel><TextBlock Text="内存" FontSize="12" Opacity="0.7"/>
     <TextBlock x:Name="MemoryValue" FontSize="29" FontWeight="SemiBold" Margin="0,5,0,3"/>
     <TextBlock x:Name="AvailableValue" FontSize="12"/></StackPanel>
   </Border>
   <Border x:Name="PerformanceTile" Background="#207F7F7F" CornerRadius="10" Padding="14">
    <Grid><Grid.ColumnDefinitions><ColumnDefinition Width="*"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions><TextBlock x:Name="CpuValue" Text="CPU —" FontSize="17"/><TextBlock x:Name="GpuValue" Text="GPU —" Grid.Column="1" FontSize="17" HorizontalAlignment="Right"/></Grid>
   </Border>
  </StackPanel>
  <Button x:Name="SettingsButton" Grid.Row="2" Content="系统设置" HorizontalAlignment="Stretch" Margin="0,16,0,0"/>
 </Grid>
</Border>)XAML";

#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

namespace mediaCard {
// Layout and transport styling adapted from this machine's MediaPeek mod.
// Session selection continues to follow the system's current media session.
inline c::Grid view{nullptr};
inline uint64_t imageRevision = UINT64_MAX;
inline ULONGLONG optimisticUntil = 0;
inline bool optimisticPlaying = false;

inline constexpr wchar_t markup[] =
    LR"XAML(
<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" Height="150" VerticalAlignment="Center">
 <Grid.Resources>
  <Style x:Key="TransportButton" TargetType="Button">
   <Setter Property="Width" Value="40"/><Setter Property="Height" Value="40"/>
   <Setter Property="Padding" Value="0"/><Setter Property="CornerRadius" Value="6"/>
   <Setter Property="BorderThickness" Value="0"/><Setter Property="Background" Value="Transparent"/>
   <Setter Property="Foreground" Value="{ThemeResource SystemControlForegroundBaseHighBrush}"/>
  </Style>
 </Grid.Resources>
 <Grid.RowDefinitions>
  <RowDefinition Height="24"/><RowDefinition Height="10"/><RowDefinition Height="58"/>
  <RowDefinition Height="18"/><RowDefinition Height="40"/>
 </Grid.RowDefinitions>
 <Grid.ColumnDefinitions><ColumnDefinition Width="*"/><ColumnDefinition Width="104"/></Grid.ColumnDefinitions>
 <Grid x:Name="MediaHeader" ColumnSpacing="8">
  <Grid.ColumnDefinitions><ColumnDefinition Width="16"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
  <FontIcon x:Name="HeaderFallback" Glyph="&#xE8D6;" FontFamily="Segoe Fluent Icons" FontSize="14"/>
  <Image x:Name="HeaderLogo" Width="16" Height="16"/>
  <TextBlock x:Name="SourceLabel" Grid.Column="1" Text="媒体播放" FontSize="12" VerticalAlignment="Center"
   Foreground="{ThemeResource SystemControlForegroundBaseMediumBrush}" TextTrimming="CharacterEllipsis"/>
 </Grid>
 <StackPanel Grid.Row="2" Spacing="2" VerticalAlignment="Center">
   <TextBlock x:Name="TitleText" Text="暂无媒体播放" FontSize="14" FontWeight="SemiBold" MaxLines="1"
    Foreground="{ThemeResource SystemControlForegroundBaseHighBrush}" TextTrimming="CharacterEllipsis"/>
   <TextBlock x:Name="ArtistText" FontSize="12" MaxLines="1" TextTrimming="CharacterEllipsis"
    Foreground="{ThemeResource SystemControlForegroundBaseMediumBrush}"/>
 </StackPanel>
  <Button x:Name="CoverButton" Grid.Row="0" Grid.RowSpan="3" Grid.Column="1" Width="92" Height="92" HorizontalAlignment="Right" MinWidth="0" MinHeight="0" CornerRadius="3" Padding="0" BorderThickness="0" HorizontalContentAlignment="Stretch" VerticalContentAlignment="Stretch" Background="Transparent" ToolTipService.ToolTip="打开媒体来源应用" AutomationProperties.Name="打开媒体来源应用">
   <Grid Width="92" Height="92" UseLayoutRounding="True">
    <FontIcon x:Name="ArtFallback" Glyph="&#xE8D6;" FontFamily="Segoe Fluent Icons" FontSize="24"/>
    <Rectangle x:Name="PlayerAppIcon" Width="92" Height="92" RadiusX="3" RadiusY="3"/>
    <Rectangle x:Name="PlayerArt" Width="92" Height="92" RadiusX="3" RadiusY="3"/>
   </Grid>
  </Button>
 <StackPanel x:Name="TransportControls" Grid.Row="4" Grid.ColumnSpan="2" Orientation="Horizontal" Spacing="12" HorizontalAlignment="Center">
  <Button x:Name="PreviousButton" Style="{StaticResource TransportButton}" ToolTipService.ToolTip="上一首" AutomationProperties.Name="上一首">
   <FontIcon Glyph="&#xF8AC;" FontFamily="Segoe Fluent Icons" FontSize="16"/>
  </Button>
  <Button x:Name="PlayButton" Style="{StaticResource TransportButton}" ToolTipService.ToolTip="播放／暂停" AutomationProperties.Name="播放／暂停">
   <Grid Width="16" Height="16">
    <FontIcon x:Name="PlayGlyph" Glyph="&#xF5B0;" FontFamily="Segoe Fluent Icons" FontSize="16"/>
    <FontIcon x:Name="PauseGlyph" Glyph="&#xF8AE;" FontFamily="Segoe Fluent Icons" FontSize="16" Visibility="Collapsed"/>
   </Grid>
  </Button>
  <Button x:Name="NextButton" Style="{StaticResource TransportButton}" ToolTipService.ToolTip="下一首" AutomationProperties.Name="下一首">
   <FontIcon Glyph="&#xF8AD;" FontFamily="Segoe Fluent Icons" FontSize="16"/>
  </Button>
 </StackPanel>
</Grid>)XAML";
inline void Remove() {
    view = nullptr;
    imageRevision = UINT64_MAX;
    optimisticUntil = 0;
}
inline x::FrameworkElement Element(const wchar_t* name) {
    auto element = view.FindName(name).try_as<x::FrameworkElement>();
    if (!element)
        throw hresult_error(E_FAIL, L"Missing MediaPeek XAML element");
    return element;
}
inline void Update(svc::Data const&);
inline c::Grid Build() {
    view = x::Markup::XamlReader::Load(markup).as<c::Grid>();
    Element(L"PreviousButton")
        .as<c::Button>()
        .Click([](auto const&, auto const&) { svc::Media(1); });
    Element(L"PlayButton").as<c::Button>().Click([](auto const&, auto const&) {
        auto state = svc::Snapshot();
        bool playing = GetTickCount64() < optimisticUntil ? optimisticPlaying
                                                          : state.playing;
        optimisticPlaying = !playing;
        optimisticUntil = GetTickCount64() + 900;
        svc::Media(2);
        Update(state);
    });
    Element(L"NextButton").as<c::Button>().Click([](auto const&, auto const&) {
        svc::Media(3);
    });
    Element(L"CoverButton").as<c::Button>().Click([](auto const&, auto const&) {
        svc::Media(4);
    });
    return view;
}
inline void Image(const wchar_t* name, svc::IRandomAccessStream const& stream) {
    auto element = Element(name);
    m::Imaging::BitmapImage bitmap{nullptr};
    if (stream)
        try {
            bitmap = m::Imaging::BitmapImage();
            bitmap.SetSourceAsync(stream.CloneStream());
        } catch (...) {
            bitmap = nullptr;
        }
    if (auto image = element.try_as<c::Image>())
        image.Source(bitmap);
    else if (auto shape =
                 element.try_as<Windows::UI::Xaml::Shapes::Rectangle>()) {
        if (bitmap) {
            m::ImageBrush brush;
            brush.Stretch(m::Stretch::Uniform);
            brush.ImageSource(bitmap);
            shape.Fill(brush);
        } else
            shape.Fill(nullptr);
    }
}
inline void Update(svc::Data const& data) {
    if (!view)
        return;

    Element(L"SourceLabel").as<c::TextBlock>().Text(data.mediaSource);
    Element(L"TitleText").as<c::TextBlock>().Text(data.title);
    Element(L"ArtistText")
        .as<c::TextBlock>()
        .Text(data.mediaError.empty() ? data.artist : data.mediaError);
    Element(L"PreviousButton").as<c::Button>().IsEnabled(data.previous);
    Element(L"PlayButton").as<c::Button>().IsEnabled(data.toggle);
    Element(L"NextButton").as<c::Button>().IsEnabled(data.next);
    Element(L"CoverButton").as<c::Button>().IsEnabled(!data.mediaApp.empty());
    bool playing =
        GetTickCount64() < optimisticUntil ? optimisticPlaying : data.playing;
    Element(L"PlayGlyph")
        .Visibility(playing ? x::Visibility::Collapsed
                            : x::Visibility::Visible);
    Element(L"PauseGlyph")
        .Visibility(playing ? x::Visibility::Visible
                            : x::Visibility::Collapsed);
    if (imageRevision != data.artRevision) {
        imageRevision = data.artRevision;
        Image(L"HeaderLogo", data.mediaLogo);
        Image(L"PlayerAppIcon", data.mediaLogo);
        Image(L"PlayerArt", data.art);
        Element(L"HeaderFallback")
            .Visibility(data.mediaLogo ? x::Visibility::Collapsed
                                       : x::Visibility::Visible);
        Element(L"ArtFallback")
            .Visibility(data.mediaLogo || data.art ? x::Visibility::Collapsed
                                                   : x::Visibility::Visible);
        Element(L"PlayerAppIcon")
            .Visibility(data.art ? x::Visibility::Collapsed
                                 : x::Visibility::Visible);
    }
}
}  // namespace mediaCard

#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Text.h>
namespace cards {
inline c::Border bottom{nullptr};
inline c::Grid parent{nullptr};
inline std::vector<c::RowDefinition> rows;
inline std::vector<c::Border> tiles;
inline std::vector<double> tileWeights;
inline c::Grid tileGrid{nullptr};
inline c::TextBlock networkName{nullptr}, networkState{nullptr},
    download{nullptr}, upload{nullptr}, storageName{nullptr},
    storageValue{nullptr}, storageDetail{nullptr}, storageFree{nullptr};
inline c::ProgressBar storageProgress{nullptr};
inline c::TextBlock mediaTitle{nullptr}, mediaArtist{nullptr},
    weatherCity{nullptr}, weatherValue{nullptr}, weatherDetail{nullptr},
    weatherSource{nullptr}, volumeLabel{nullptr}, brightnessLabel{nullptr};
inline c::Slider volume{nullptr}, brightness{nullptr};
inline c::Button previous{nullptr}, play{nullptr}, next{nullptr}, mute{nullptr};
inline c::Image cover{nullptr};
inline uint64_t artRevision = UINT64_MAX;
inline bool updating = false;
inline ULONGLONG volumeTouched = 0, brightnessTouched = 0;
inline void Remove() {
    mediaCard::Remove();
    tiles.clear();
    tileWeights.clear();
    tileGrid = nullptr;
    networkName = nullptr;
    networkState = nullptr;
    download = nullptr;
    upload = nullptr;
    storageName = nullptr;
    storageValue = nullptr;
    storageDetail = nullptr;
    storageFree = nullptr;
    storageProgress = nullptr;
    try {
        if (parent && bottom) {
            uint32_t index;
            if (parent.Children().IndexOf(bottom, index))
                parent.Children().RemoveAt(index);
        }
        if (parent)
            for (auto const& row : rows) {
                uint32_t index;
                if (parent.RowDefinitions().IndexOf(row, index))
                    parent.RowDefinitions().RemoveAt(index);
            }
    } catch (...) {
    }
    rows.clear();
    bottom = nullptr;
    parent = nullptr;
    mediaTitle = nullptr;
    mediaArtist = nullptr;
    weatherCity = nullptr;
    weatherValue = nullptr;
    weatherDetail = nullptr;
    weatherSource = nullptr;
    volumeLabel = nullptr;
    brightnessLabel = nullptr;
    volume = nullptr;
    brightness = nullptr;
    previous = nullptr;
    play = nullptr;
    next = nullptr;
    mute = nullptr;
    cover = nullptr;
    artRevision = UINT64_MAX;
}
inline c::Border Border(c::Border const& native) {
    c::Border value;
    value.Padding({12, 10, 12, 10});
    value.CornerRadius(native.CornerRadius());
    value.Background(native.Background());
    value.BorderBrush(native.BorderBrush());
    value.BorderThickness(native.BorderThickness());
    return value;
}
inline c::TextBlock Text(const wchar_t* value, double size = 14) {
    c::TextBlock text;
    text.Text(value);
    text.FontSize(size);
    text.TextTrimming(x::TextTrimming::CharacterEllipsis);
    return text;
}
inline c::Button IconButton(const wchar_t* glyph) {
    c::Button button;
    c::FontIcon icon;
    icon.FontFamily(m::FontFamily(L"Segoe MDL2 Assets"));
    icon.Glyph(glyph);
    icon.FontSize(17);
    button.Content(icon);
    button.Padding({10, 6, 10, 6});
    return button;
}
inline void Height(x::FrameworkElement const& element, double height) {
    Save(element, x::FrameworkElement::HeightProperty());
    element.Height(height);
}
inline bool Visible(x::DependencyObject node) {
    for (int i = 0; node && i < 8; i++) {
        if (auto element = node.try_as<x::FrameworkElement>()) {
            if (element.Visibility() == x::Visibility::Collapsed)
                return false;
            if (element.Name() == L"AnimationRoot")
                break;
        }
        node = m::VisualTreeHelper::GetParent(node);
    }
    return true;
}
inline void Reflow() {
    if (!bottom || !tileGrid || tiles.empty())
        return;
    auto root = x::Window::Current().Content();
    auto main = Resolve(root, L"MainMenu");
    if (!main)
        return;
    double width = main.ActualWidth();
    if (width < 100)
        width = main.Width();
    if (!std::isfinite(width) || width < 100)
        return;
    if (auto phone = Resolve(root, L"RightCompanionContainerGrid");
        phone && Visible(phone))
        width += phone.ActualWidth();
    if (g_card)
        width += g_card.Width() + g_card.Margin().Left;
    bottom.Width(width);
    tileGrid.Width(width);
    double totalWeight = 0;
    for (auto weight : tileWeights)
        totalWeight += weight;
    for (uint32_t i = 0; i < tiles.size(); i++) {
        double cell = width * tileWeights[i] / totalWeight;
        double cardWidth = std::max(60.0, cell - (i ? 4 : 0));
        tileGrid.ColumnDefinitions().GetAt(i).Width(
            {cell, x::GridUnitType::Pixel});
        tiles[i].Width(cardWidth);
        tiles[i].MaxWidth(cardWidth);
        if (auto child = tiles[i].Child().try_as<x::FrameworkElement>()) {
            auto padding = tiles[i].Padding(),
                 border = tiles[i].BorderThickness();
            double innerWidth =
                std::max(1.0, cardWidth - padding.Left - padding.Right -
                                  border.Left - border.Right);
            // Let Border arrange its child to the actual content slot,
            // including border thickness and layout rounding. Forcing
            // cardWidth-24 overflowed that slot and clipped the right edge of
            // the media artwork.
            child.Width(NAN);
            child.MaxWidth(innerWidth);
        }
    }
}
inline void Build(x::DependencyObject const& root, c::Border const& native) {
    auto settings = std::atomic_load(&svc::config);
    bool quick = settings->quick && (settings->volume || settings->brightness);
    if (!settings->media && !settings->weather && !quick &&
        !settings->network && !settings->storage) {
        return;
    }
    auto animation = Resolve(root, L"AnimationRoot").try_as<c::Grid>();
    auto main = Resolve(root, L"MainMenu");
    auto frame = Resolve(root, L"FrameRoot");
    if (!animation || !main || !frame) {
        return;
    }
    // Preserve the original menu and companion height. Extend the native
    // height enforcer as well so Start measures the added row as part of its
    // surface rather than clipping it at the old 750-DIP frame boundary.
    double oldHeight = main.ActualHeight();
    if (oldHeight < 100)
        oldHeight = 726;
    double panelHeight = 176, gap = 4,
           totalHeight = oldHeight + panelHeight + gap;
    double framePadding =
        std::max(0.0, frame.ActualHeight() - animation.ActualHeight());
    Height(main, oldHeight);
    if (auto phone = Resolve(root, L"RightCompanion"))
        Height(phone, oldHeight);
    parent = animation;
    if (animation.RowDefinitions().Size() == 0) {
        c::RowDefinition first;
        first.Height({oldHeight, x::GridUnitType::Pixel});
        animation.RowDefinitions().Append(first);
        rows.push_back(first);
    }
    auto row = animation.RowDefinitions().Size();
    c::RowDefinition added;
    added.Height({panelHeight + gap, x::GridUnitType::Pixel});
    animation.RowDefinitions().Append(added);
    rows.push_back(added);
    Height(animation, totalHeight);
    Save(frame, x::FrameworkElement::MaxHeightProperty());
    frame.MaxHeight(INFINITY);
    Height(frame, totalHeight + framePadding);
    if (auto enforcer = Resolve(root, L"MaxHeightEnforcer"))
        Height(enforcer, totalHeight + framePadding);
    bottom = c::Border();
    bottom.Name(L"StartSidecarBottomModules");
    bottom.Margin({0, gap, 0, 0});
    bottom.Height(panelHeight);
    bottom.HorizontalAlignment(x::HorizontalAlignment::Stretch);
    c::Grid grid;
    tileGrid = grid;
    bottom.Child(grid);
    int column = 0;
    auto add = [&](c::Border const& card, double weight = 1.0) {
        c::ColumnDefinition definition;
        definition.Width({200, x::GridUnitType::Pixel});
        grid.ColumnDefinitions().Append(definition);
        c::Grid::SetColumn(card, column);
        card.Margin({column ? 4.0 : 0, 0, 0, 0});
        grid.Children().Append(card);
        tiles.push_back(card);
        tileWeights.push_back(weight);
        column++;
    };
    if (settings->media) {
        auto card = Border(native);
        card.Child(mediaCard::Build());
        add(card, 1.7);
    }
    if (settings->weather) {
        auto card = Border(native);
        c::StackPanel info;
        weatherCity = Text(settings->city.c_str(), 13);
        weatherCity.FontWeight(Windows::UI::Text::FontWeights::SemiBold());
        weatherValue = Text(L"加载中", 27);
        weatherValue.Margin({0, 12, 0, 6});
        weatherDetail = Text(L"", 11);
        weatherDetail.TextWrapping(x::TextWrapping::WrapWholeWords);
        info.Children().Append(weatherCity);
        info.Children().Append(weatherValue);
        info.Children().Append(weatherDetail);
        card.Child(info);
        add(card, 0.9);
    }
    if (quick) {
        auto card = Border(native);
        c::StackPanel content;
        auto heading = Text(L"快速设置", 14);
        heading.FontWeight(Windows::UI::Text::FontWeights::SemiBold());
        c::Grid header;
        header.ColumnDefinitions().Append(c::ColumnDefinition());
        c::ColumnDefinition buttonColumn;
        buttonColumn.Width({1, x::GridUnitType::Auto});
        header.ColumnDefinitions().Append(buttonColumn);
        header.Children().Append(heading);
        if (settings->volume) {
            mute = c::Button();
            mute.Content(box_value(L"静音"));
            mute.FontSize(10);
            mute.Padding({6, 1, 6, 1});
            c::Grid::SetColumn(mute, 1);
            mute.Click([](auto const&, auto const&) {
                svc::Mute(!svc::Snapshot().muted);
            });
            header.Children().Append(mute);
        }
        content.Children().Append(header);
        if (settings->volume) {
            volumeLabel = Text(L"音量", 11);
            volumeLabel.Margin({0, 6, 0, 0});
            content.Children().Append(volumeLabel);
            volume = c::Slider();
            volume.Minimum(0);
            volume.Maximum(100);
            volume.StepFrequency(1);
            volume.Height(26);
            volume.ValueChanged([](auto const& sender, auto const&) {
                if (!updating) {
                    volumeTouched = GetTickCount64();
                    svc::Volume((int)std::round(
                        sender.template as<c::Slider>().Value()));
                }
            });
            content.Children().Append(volume);
        }
        if (settings->brightness) {
            brightnessLabel = Text(L"亮度", 11);
            brightnessLabel.Margin({0, 3, 0, 0});
            content.Children().Append(brightnessLabel);
            brightness = c::Slider();
            brightness.Minimum(1);
            brightness.Maximum(100);
            brightness.StepFrequency(1);
            brightness.Height(26);
            brightness.ValueChanged([](auto const& sender, auto const&) {
                if (!updating) {
                    brightnessTouched = GetTickCount64();
                    svc::Brightness((int)std::round(
                        sender.template as<c::Slider>().Value()));
                }
            });
            content.Children().Append(brightness);
        }
        card.Child(content);
        add(card);
    }
    if (settings->network) {
        auto card = Border(native);
        c::StackPanel info;
        info.Children().Append(Text(L"网络", 14));
        networkName = Text(L"正在读取", 13);
        networkName.Margin({0, 10, 0, 3});
        networkState = Text(L"", 11);
        networkState.Opacity(0.65);
        download = Text(L"↓ —", 16);
        download.Margin({0, 12, 0, 3});
        upload = Text(L"↑ —", 16);
        info.Children().Append(networkName);
        info.Children().Append(networkState);
        info.Children().Append(download);
        info.Children().Append(upload);
        card.Child(info);
        add(card, 0.7);
    }
    if (settings->storage) {
        auto card = Border(native);
        c::StackPanel info;
        storageName = Text(L"系统盘", 14);
        storageValue = Text(L"—", 29);
        storageValue.Margin({0, 9, 0, 6});
        storageProgress = c::ProgressBar();
        storageProgress.Maximum(100);
        storageProgress.Height(5);
        storageDetail = Text(L"", 11);
        storageDetail.Margin({0, 10, 0, 4});
        storageFree = Text(L"", 12);
        info.Children().Append(storageName);
        info.Children().Append(storageValue);
        info.Children().Append(storageProgress);
        info.Children().Append(storageDetail);
        info.Children().Append(storageFree);
        card.Child(info);
        add(card, 0.7);
    }
    c::Grid::SetColumn(bottom, c::Grid::GetColumn(main));
    c::Grid::SetColumnSpan(
        bottom, std::max(1u, animation.ColumnDefinitions().Size() -
                                 (uint32_t)c::Grid::GetColumn(main)));
    c::Grid::SetRow(bottom, row);
    animation.Children().Append(bottom);
    Reflow();
}
inline void Update(svc::Data const& data) {
    Reflow();
    mediaCard::Update(data);
    auto settings = std::atomic_load(&svc::config);
    wchar_t text[192];
    if (weatherValue) {
        weatherCity.Text(settings->city);
        if (std::isfinite(data.temperature)) {
            swprintf(text, 192, L"%.0f°  %s", data.temperature,
                     data.weatherCondition.c_str());
            weatherValue.Text(text);
            weatherValue.FontSize(27);
            std::wstring details;
            if (std::isfinite(data.feels)) {
                swprintf(text, 192, L"体感 %.0f°", data.feels);
                details = text;
            }
            if (std::isfinite(data.humidity)) {
                swprintf(text, 192, L"  湿度 %.0f%%", data.humidity);
                details += text;
            }
            weatherDetail.Text(details);
        } else {
            weatherValue.Text(data.weatherCondition);
            weatherValue.FontSize(18);
            weatherDetail.Text(L"稍后自动重试");
        }
    }
    if (networkName) {
        networkName.Text(data.networkName);
        networkState.Text(data.networkState);
        auto speed = [](double bytes) {
            if (bytes < 0)
                return std::wstring(L"—");
            const wchar_t* unit = L"B/s";
            if (bytes >= 1048576) {
                bytes /= 1048576;
                unit = L"MB/s";
            } else if (bytes >= 1024) {
                bytes /= 1024;
                unit = L"KB/s";
            }
            wchar_t buffer[64];
            swprintf(buffer, 64, L"%.1f %s", bytes, unit);
            return std::wstring(buffer);
        };
        download.Text(L"↓ " + speed(data.download));
        upload.Text(L"↑ " + speed(data.upload));
    }
    if (storageName) {
        storageName.Text(data.storageName);
        if (data.diskTotal > 0) {
            double percent = 100 * data.diskUsed / data.diskTotal;
            swprintf(text, 192, L"%.0f%%", percent);
            storageValue.Text(text);
            storageProgress.Value(percent);
            swprintf(text, 192, L"已用 %.1f / %.1f GB", data.diskUsed,
                     data.diskTotal);
            storageDetail.Text(text);
            swprintf(text, 192, L"可用 %.1f GB", data.diskFree);
            storageFree.Text(text);
        }
    }
    updating = true;
    if (volume) {
        volume.IsEnabled(data.volume >= 0);
        if (data.volume >= 0) {
            if (GetTickCount64() - volumeTouched > 700)
                volume.Value(data.volume);
            swprintf(text, 192, L"音量  %.0f%%%s", volume.Value(),
                     data.muted ? L" · 已静音" : L"");
            volumeLabel.Text(text);
        } else
            volumeLabel.Text(L"音量暂不可用");
    }
    if (brightness) {
        brightness.IsEnabled(data.brightnessReady);
        if (data.brightnessReady) {
            if (GetTickCount64() - brightnessTouched > 700)
                brightness.Value(data.brightness);
            swprintf(text, 192, L"亮度  %.0f%%", brightness.Value());
            brightnessLabel.Text(text);
        } else
            brightnessLabel.Text(L"亮度暂不可用");
    }
    updating = false;
    if (mute) {
        mute.Content(box_value(data.muted ? L"取消静音" : L"静音"));
        mute.IsEnabled(data.volume >= 0);
    }
}
}  // namespace cards

static void RemoveBottom() {
    cards::Remove();
}
static void UpdateBottom(svc::Data const& data) {
    cards::Update(data);
}
static void BuildBottom(x::DependencyObject const& root,
                        c::Border const& native) {
    cards::Build(root, native);
}

static void EnsureLayout() {
    if (g_unloading)
        return;
    try {
        auto window = x::Window::Current();
        if (!window || !window.Content())
            return;
        auto root = window.Content();
        if (g_layoutReady && g_layoutRoot == root &&
            (!g_card || g_card.Parent())) {
            UpdateData();
            if (g_core && g_core.Visible() && g_update)
                g_update.Start();
            return;
        }
        RemoveCard();
        if (auto element = root.try_as<x::FrameworkElement>())
            element.UpdateLayout();
        g_layoutRoot = root;
        auto settings = std::atomic_load(&svc::config);
        auto nativeBorder =
            Resolve(root, L"RightCompanion/CompanionRoot/AcrylicBorder")
                .try_as<c::Border>();
        if (!nativeBorder)
            nativeBorder =
                Resolve(root, L"MainMenu/AcrylicBorder").try_as<c::Border>();
        if (!nativeBorder) {
            State(L"waiting: native background border not ready");
            return;
        }
        if (!settings->overview) {
            BuildBottom(root, nativeBorder);
            g_layoutReady = true;
            UpdateData();
            if (g_update)
                g_update.Start();
            State(L"bottom XAML modules attached; overview disabled");
            return;
        }
        auto target = Resolve(root, L"AnimationRoot");
        auto host = target.try_as<c::Grid>();
        if (!host) {
            State(L"waiting: AnimationRoot Grid not ready");
            return;
        }
        double oldWidth = host.ActualWidth();
        if (oldWidth < 20)
            oldWidth = 864;
        double width = settings->overviewWidth;
        auto companion = Resolve(root, L"RightCompanion");
        auto companionContainer = Resolve(root, L"RightCompanionContainerGrid");
        double gap = settings->cardGap;
        if (gap < 0) {
            gap = companion ? std::max(companion.Margin().Left,
                                       companion.Margin().Right)
                            : 0;
            if (gap <= 0 && companion && companionContainer)
                gap =
                    companionContainer.ActualWidth() - companion.ActualWidth();
            gap = std::clamp(gap, 0.0, 24.0);
        } else
            gap = std::clamp(gap, 0.0, 24.0);
        g_card = x::Markup::XamlReader::Load(kMarkup).as<c::Border>();
        if (nativeBorder) {
            g_card.Background(nativeBorder.Background());
            g_card.BorderBrush(nativeBorder.BorderBrush());
            g_card.BorderThickness(nativeBorder.BorderThickness());
            g_card.CornerRadius(nativeBorder.CornerRadius());
        }
        g_card.Width(width);
        g_card.HorizontalAlignment(x::HorizontalAlignment::Left);
        g_card.Margin({gap, 0, 0, 0});
        g_card.VerticalAlignment(x::VerticalAlignment::Stretch);
        g_clock = Find(g_card, L"ClockValue").as<c::TextBlock>();
        g_date = Find(g_card, L"DateValue").as<c::TextBlock>();
        g_title = Find(g_card, L"DeviceTitle").as<c::TextBlock>();
        g_powerSource = Find(g_card, L"PowerSourceValue").as<c::TextBlock>();
        g_cpu = Find(g_card, L"CpuValue").as<c::TextBlock>();
        g_gpu = Find(g_card, L"GpuValue").as<c::TextBlock>();
        g_battery = Find(g_card, L"BatteryValue").as<c::TextBlock>();
        g_power = Find(g_card, L"PowerValue").as<c::TextBlock>();
        g_memory = Find(g_card, L"MemoryValue").as<c::TextBlock>();
        g_available = Find(g_card, L"AvailableValue").as<c::TextBlock>();
        Find(g_card, L"TimeTile")
            .Visibility(settings->time ? x::Visibility::Visible
                                       : x::Visibility::Collapsed);
        Find(g_card, L"BatteryTile")
            .Visibility(settings->battery ? x::Visibility::Visible
                                          : x::Visibility::Collapsed);
        Find(g_card, L"MemoryTile")
            .Visibility(settings->memory ? x::Visibility::Visible
                                         : x::Visibility::Collapsed);
        Find(g_card, L"PerformanceTile")
            .Visibility(settings->performance ? x::Visibility::Visible
                                              : x::Visibility::Collapsed);
        auto button = Find(g_card, L"SettingsButton").as<c::Button>();
        button.Click([](auto const&, auto const&) {
            ShellExecuteW(nullptr, L"open", L"ms-settings:about", nullptr,
                          nullptr, SW_SHOWNORMAL);
        });
        g_host = host;
        uint32_t existing = host.ColumnDefinitions().Size();
        if (existing == 0) {
            c::ColumnDefinition original;
            original.Width({oldWidth, x::GridUnitType::Pixel});
            host.ColumnDefinitions().Append(original);
            g_addedColumns.push_back(original);
            existing = 1;
        }
        c::ColumnDefinition added;
        added.Width({width + gap, x::GridUnitType::Pixel});
        host.ColumnDefinitions().Append(added);
        g_addedColumns.push_back(added);
        c::Grid::SetColumn(g_card, existing);
        c::Grid::SetRow(g_card, 0);
        host.Children().Append(g_card);
        BuildBottom(root, nativeBorder);
        g_layoutReady = true;
        UpdateData();
        if (g_update)
            g_update.Start();
        std::wstring ancestry = L"attached XAML card; host=" +
                                std::wstring(get_class_name(host)) + L"#" +
                                std::wstring(host.Name());
        ancestry += L"\r\nnative gap DIP=" + Number(gap);
        if (g_card.Background())
            ancestry += L"\r\nbackground=" +
                        std::wstring(get_class_name(g_card.Background()));
        auto radius = g_card.CornerRadius();
        ancestry += L"\r\ncorner radius=" + Number(radius.TopLeft) + L"," +
                    Number(radius.TopRight) + L"," +
                    Number(radius.BottomRight) + L"," +
                    Number(radius.BottomLeft);
        ancestry += L"\r\nancestry:\r\n";
        x::DependencyObject node = g_card;
        for (int depth = 0; node && depth < 20; depth++) {
            ancestry += std::wstring(get_class_name(node));
            if (auto e = node.try_as<x::FrameworkElement>())
                ancestry += L"#" + std::wstring(e.Name());
            ancestry += L"\r\n";
            node = m::VisualTreeHelper::GetParent(node);
        }
        State(ancestry);
    } catch (hresult_error const& error) {
        RemoveCard();
        State(L"XAML error " + std::to_wstring((unsigned)error.code()) + L": " +
              std::wstring(error.message()));
    } catch (...) {
        RemoveCard();
        State(L"unexpected XAML attachment error");
    }
}
static void QueueLayout() {
    if (g_unloading)
        return;
    try {
        if (auto dispatcher =
                Windows::System::DispatcherQueue::GetForCurrentThread())
            dispatcher.TryEnqueue(Windows::System::DispatcherQueuePriority::Low,
                                  [] {
                                      if (!g_unloading)
                                          EnsureLayout();
                                  });
    } catch (...) {
    }
}
static bool InitializeUi() {
    if (g_unloading)
        return true;
    try {
        auto window = x::Window::Current();
        auto cw = core::CoreWindow::GetForCurrentThread();
        if (!window || !window.Content() || !cw)
            return false;
        if (!g_core) {
            g_core = cw;
            svc::SetVisible(cw.Visible());
            g_update = x::DispatcherTimer();
            g_update.Interval(std::chrono::milliseconds(200));
            g_update.Tick([](auto const&, auto const&) {
                if (!g_unloading)
                    UpdateData();
            });
            g_visible = cw.VisibilityChanged([](auto const&, auto const& args) {
                if (g_unloading)
                    return;
                svc::SetVisible(args.Visible());
                if (args.Visible())
                    QueueLayout();
                else if (g_update)
                    g_update.Stop();
            });
            g_activated = cw.Activated([](auto const&, auto const& args) {
                if (!g_unloading &&
                    args.WindowActivationState() !=
                        core::CoreWindowActivationState::Deactivated)
                    QueueLayout();
            });
        }
        QueueLayout();
        return true;
    } catch (...) {
        return false;
    }
}
static void CALLBACK ReadyTimer(HWND hwnd, UINT, UINT_PTR id, DWORD) {
    if (g_unloading || InitializeUi())
        KillTimer(hwnd, id);
}
static void StartOnUi(HWND hwnd) {
    g_hwnd = hwnd;
    if (!InitializeUi())
        SetTimer(hwnd, kReadyTimer, 250, ReadyTimer);
}
using Create_t = HWND(WINAPI*)(DWORD,
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
                               void*,
                               DWORD);
using CreateEx_t = HWND(WINAPI*)(DWORD,
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
                                 void*,
                                 DWORD,
                                 DWORD);
static Create_t g_create;
static CreateEx_t g_createEx;
static bool CoreClass(LPCWSTR cls) {
    return (ULONG_PTR)cls > 0xFFFF &&
           _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}
static HWND WINAPI HookCreate(DWORD ex,
                              LPCWSTR cls,
                              LPCWSTR title,
                              DWORD style,
                              int a,
                              int b,
                              int w,
                              int h,
                              HWND parent,
                              HMENU menu,
                              HINSTANCE instance,
                              void* param,
                              DWORD band) {
    HWND hwnd = g_create(ex, cls, title, style, a, b, w, h, parent, menu,
                         instance, param, band);
    if (hwnd && CoreClass(cls) && !g_unloading)
        StartOnUi(hwnd);
    return hwnd;
}
static HWND WINAPI HookCreateEx(DWORD ex,
                                LPCWSTR cls,
                                LPCWSTR title,
                                DWORD style,
                                int a,
                                int b,
                                int w,
                                int h,
                                HWND parent,
                                HMENU menu,
                                HINSTANCE instance,
                                void* param,
                                DWORD band,
                                DWORD flags) {
    HWND hwnd = g_createEx(ex, cls, title, style, a, b, w, h, parent, menu,
                           instance, param, band, flags);
    if (hwnd && CoreClass(cls) && !g_unloading)
        StartOnUi(hwnd);
    return hwnd;
}
static HWND FindWindowForStart() {
    HWND candidate = nullptr;
    while ((candidate = FindWindowExW(
                nullptr, candidate, L"Windows.UI.Core.CoreWindow", nullptr))) {
        DWORD pid = 0;
        GetWindowThreadProcessId(candidate, &pid);
        if (pid == GetCurrentProcessId())
            return candidate;
    }
    return nullptr;
}
using UiCall = void (*)(void*);
static bool OnUi(HWND hwnd, UiCall function, void* param) {
    DWORD thread = GetWindowThreadProcessId(hwnd, nullptr);
    if (!thread)
        return false;
    if (thread == GetCurrentThreadId()) {
        function(param);
        return true;
    }
    struct Packet {
        UiCall function;
        void* param;
    };
    const UINT message = RegisterWindowMessageW(kCallMessage);
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                auto message = (CWPSTRUCT*)lp;
                if (message->message == RegisterWindowMessageW(kCallMessage)) {
                    auto packet = (Packet*)message->lParam;
                    packet->function(packet->param);
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        },
        nullptr, thread);
    if (!hook)
        return false;
    Packet packet{function, param};
    SendMessageW(hwnd, message, 0, (LPARAM)&packet);
    UnhookWindowsHookEx(hook);
    return true;
}
BOOL Wh_ModInit() {
    g_unloading = false;
    LoadSettings();
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop)
        return FALSE;
    HMODULE user = GetModuleHandleW(L"user32.dll");
    if (auto address = GetProcAddress(user, "CreateWindowInBand"))
        Wh_SetFunctionHook((void*)address, (void*)HookCreate,
                           (void**)&g_create);
    if (auto address = GetProcAddress(user, "CreateWindowInBandEx"))
        Wh_SetFunctionHook((void*)address, (void*)HookCreateEx,
                           (void**)&g_createEx);
    State(L"XAML module initialized; awaiting Start UI thread");
    return TRUE;
}
void Wh_ModAfterInit() {
    svc::Start(g_stop);
    g_startup = CreateThread(
        nullptr, 0,
        [](void*) -> DWORD {
            while (!g_unloading) {
                if (HWND hwnd = FindWindowForStart()) {
                    OnUi(hwnd, [](void* arg) { StartOnUi((HWND)arg); }, hwnd);
                    break;
                }
                if (WaitForSingleObject(g_stop, 500) == WAIT_OBJECT_0)
                    break;
            }
            return 0;
        },
        nullptr, 0, nullptr);
}
void Wh_ModUninit() {
    g_unloading = true;
    SetEvent(g_stop);
    if (g_startup) {
        WaitForSingleObject(g_startup, INFINITE);
        CloseHandle(g_startup);
        g_startup = nullptr;
    }
    if (g_hwnd && IsWindow(g_hwnd))
        OnUi(
            g_hwnd,
            [](void*) {
                KillTimer(g_hwnd, kReadyTimer);
                RemoveCard();
                if (g_core) {
                    try {
                        g_core.VisibilityChanged(g_visible);
                        g_core.Activated(g_activated);
                    } catch (...) {
                    }
                    g_core = nullptr;
                }
                g_update = nullptr;
            },
            nullptr);
    svc::Stop();
    CloseHandle(g_stop);
    g_stop = nullptr;
}
void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hwnd && IsWindow(g_hwnd))
        OnUi(
            g_hwnd,
            [](void*) {
                g_layoutReady = false;
                EnsureLayout();
            },
            nullptr);
}
