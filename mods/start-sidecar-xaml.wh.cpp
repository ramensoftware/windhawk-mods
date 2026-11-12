// ==WindhawkMod==
// @id start-sidecar-xaml
// @name Start Sidecar - Embedded XAML
// @name:zh-CN 开始菜单卡片 - Start Sidecar
// @description Native XAML live cards inside the Windows 11 Start companion layout.
// @description:zh-CN 在 Windows 11 开始菜单中嵌入原生 XAML 卡片。
// @version 0.6.2
// @author Yizuka17
// @github https://github.com/Yizuka17
// @license MIT
// @include StartMenuExperienceHost.exe
// @architecture x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -luser32 -lshell32 -ladvapi32 -lwbemuuid -lpdh -lpowrprof -luuid -liphlpapi -lshcore -lshlwapi
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModSettings==
/*
- showOverview: true
  $name: "Show system overview"
  $name:zh-CN: "显示系统概览卡片"
- deviceTitle: ''
  $name: "Device name"
  $name:zh-CN: "设备名称"
  $description: "Leave blank to use the device model."
  $description:zh-CN: "留空自动读取设备型号。"
- overviewWidth: 252
  $name: "System overview width (DIP)"
  $name:zh-CN: "系统概览宽度（DIP）"
  $description: "Range: 200–340. Default: 252."
  $description:zh-CN: "范围 200–340，默认 252。"
- cardGap: -1
  $name: "System overview spacing (DIP)"
  $name:zh-CN: "系统概览间距（DIP）"
  $description: "Use -1 to match Start, or 0–24 for a custom gap."
  $description:zh-CN: "-1 跟随原生间距；0–24 指定间距。"
- showTime: true
  $name: "System overview: clock"
  $name:zh-CN: "系统概览：时间"
- showBattery: true
  $name: "System overview: battery"
  $name:zh-CN: "系统概览：电池"
- showMemory: true
  $name: "System overview: memory"
  $name:zh-CN: "系统概览：内存"
- showPerformance: true
  $name: "System overview: CPU / GPU"
  $name:zh-CN: "系统概览：CPU / GPU"
- showMedia: true
  $name: "Card: media controls"
  $name:zh-CN: "卡片：系统媒体控制（SMTC）"
- showQuickSettings: true
  $name: "Card: quick settings"
  $name:zh-CN: "卡片：快速设置"
- showNetwork: true
  $name: "Card: network status and speed"
  $name:zh-CN: "卡片：网络状态和速度"
- showStorage: true
  $name: "Card: system drive space"
  $name:zh-CN: "卡片：系统盘空间"
- showVolume: true
  $name: "Quick settings: volume"
  $name:zh-CN: "快速设置：音量"
- showBrightness: true
  $name: "Quick settings: built-in display brightness"
  $name:zh-CN: "快速设置：内置屏幕亮度"
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Start Sidecar - Embedded XAML

Adds native XAML cards beside and below the Windows 11 Start menu's Phone Link
companion layout. There is no separate overlay window or external layout file.
Configure individual cards, device title, overview width and spacing in this
mod's Settings tab. The cards follow Start's light and dark themes.
The interface uses English by default and also supports Simplified Chinese.
Cards outside the native menu reuse Start's shadow when available.
The mod makes no outbound requests.

Cards are packed into the actual free rectangles around Start and Phone Link.
The overview, media player, and smaller controls can use different sides or rows
above and below the menu. The native menu keeps its original position and size.
Card sizes adapt to the available space, including portrait displays and taskbars
at different edges. A scrollable section or an in-menu **Cards** button is used
only when the individual cards cannot fit. The layout updates when the display,
menu position, or Phone Link visibility changes.

Available cards:

- System overview: device model, time, battery, memory, CPU and GPU usage.
- Media: the current Windows SMTC session, artwork and playback controls.
- Quick settings: volume, mute and supported internal-display brightness.
- Network: active physical interface and sampled upload/download rates.
- Storage: system-drive usage and free space.

## Screenshot

Landscape layout captured on a Surface Pro 11, showing Start, Phone Link, and
the adjacent cards. Personal details are masked. This screenshot shows the
local prototype's weather card; the catalog version does not include weather.

![Landscape Start menu with Phone Link and Start Sidecar cards](https://raw.githubusercontent.com/Yizuka17/windhawk-mods/480656ad12ed9282f85c6fae9345b28a310a6d98/assets/start-sidecar/landscape.jpg)

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

#include <winsock2.h>
#include <windows.h>
#undef GetCurrentTime
#include <shellapi.h>
#include <windhawk_utils.h>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
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
#include <powrprof.h>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <winrt/Windows.Devices.Power.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.ApplicationModel.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <appmodel.h>
#include <tlhelp32.h>
#include <shobjidl.h>
#include <shcore.h>
#include <shlwapi.h>
#include <knownfolders.h>
#include <filesystem>
#include <fstream>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Text.h>
#include <functional>
#include <optional>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.System.h>

static const wchar_t* Tr(const wchar_t* english, const wchar_t* chinese) {
    static const bool simplifiedChinese = [] {
        const LANGID language = GetUserDefaultUILanguage();
        return PRIMARYLANGID(language) == LANG_CHINESE &&
               (SUBLANGID(language) == SUBLANG_CHINESE_SIMPLIFIED ||
                SUBLANGID(language) == SUBLANG_CHINESE_SINGAPORE);
    }();
    return simplifiedChinese ? chinese : english;
}

namespace svc {
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;
struct Config {
    bool overview = true, time = true, battery = true, memory = true,
         performance = true, media = true, quick = true, volume = true,
         brightness = true, network = true, storage = true;
    int overviewWidth = 252, cardGap = -1;
    std::wstring title;
};
struct Data {
    std::wstring device = Tr(L"This PC", L"此电脑"),
                 batteryDetail =
                     Tr(L"Battery estimate unavailable", L"预计剩余时间待估算"),
                 title = Tr(L"No media", L"暂无媒体"), artist, mediaError,
                 mediaSource = Tr(L"Media", L"媒体播放");
    std::wstring mediaApp,
        networkName = Tr(L"Reading network", L"正在读取网络"), networkState,
        storageName = Tr(L"System drive", L"系统盘");
    double download = -1, upload = -1, diskUsed = -1, diskTotal = -1,
           diskFree = -1;
    double cpu = -1, gpu = -1, volume = -1, brightness = -1;
    bool muted = false, playing = false, previous = false, next = false,
         toggle = false, brightnessReady = false;
    // Only encoded image bytes cross apartments; SMTC streams stay on the
    // worker.
    std::shared_ptr<const std::vector<uint8_t>> art, mediaLogo, mediaLogoLight,
        mediaLogoDark;
    uint64_t artRevision = 0;
};
inline std::shared_ptr<Config> config = std::make_shared<Config>();
inline std::mutex dataMutex;
inline Data data;
inline HANDLE stop = nullptr, wake = nullptr, mediaWake = nullptr;
[[clang::no_destroy]] inline std::optional<std::thread> worker, mediaWorker;
inline std::atomic<int> volumeRequest = -1, brightnessRequest = -1,
                        muteRequest = -1, mediaRequest = 0;
inline std::atomic<bool> visible = false;
inline std::atomic<unsigned> visibilityEpoch = 0;
inline void SetVisible(bool value) {
    if (visible.exchange(value) == value)
        return;
    ++visibilityEpoch;
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
    if (mediaWake)
        SetEvent(mediaWake);
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
struct WorkerApartment {
    WorkerApartment() { init_apartment(apartment_type::multi_threaded); }
    ~WorkerApartment() { uninit_apartment(); }
};
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
        return Tr(L"Unknown", L"时间待估算");
    int minutes = std::max(1, (int)std::round(seconds / 60));
    if (minutes < 60)
        return std::to_wstring(minutes) + Tr(L" min", L"分钟");
    return std::to_wstring(minutes / 60) + Tr(L" h ", L"小时") +
           (minutes % 60 ? std::to_wstring(minutes % 60) + Tr(L" min", L"分钟")
                         : L"");
}
inline std::wstring DeviceName() {
    wchar_t name[512] = {};
    DWORD bytes = sizeof(name);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS",
                     L"SystemProductName", RRF_RT_REG_SZ, nullptr, name,
                     &bytes) != ERROR_SUCCESS)
        return Tr(L"This PC", L"此电脑");
    std::wstring model = name;
    return model.empty() ? Tr(L"This PC", L"此电脑") : model;
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
        return Tr(L"Remaining: ", L"预计剩余 ") + Duration(state.EstimatedTime);
    if (power.ACLineStatus == 0 && power.BatteryLifeTime != DWORD(-1))
        return Tr(L"Remaining: ", L"预计剩余 ") +
               Duration(power.BatteryLifeTime);
    try {
        auto report =
            Windows::Devices::Power::Battery::AggregateBattery().GetReport();
        auto remaining = report.RemainingCapacityInMilliwattHours(),
             full = report.FullChargeCapacityInMilliwattHours(),
             rate = report.ChargeRateInMilliwatts();
        if (power.ACLineStatus == 1) {
            if (remaining && full && remaining.Value() >= full.Value() - 100)
                return Tr(L"Fully charged", L"已充满");
            if (remaining && full && rate && rate.Value() > 100)
                return Tr(L"Time to full: ", L"充满剩余（估算） ") +
                       Duration(3600.0 * (full.Value() - remaining.Value()) /
                                rate.Value());
            return Tr(L"Charge estimate unavailable", L"充满剩余时间待估算");
        }
        if (power.BatteryLifeTime != DWORD(-1))
            return Tr(L"Remaining: ", L"预计剩余 ") +
                   Duration(power.BatteryLifeTime);
        if (remaining && rate && rate.Value() < -100)
            return Tr(L"Remaining (est.): ", L"预计剩余（估算） ") +
                   Duration(3600.0 * remaining.Value() / -rate.Value());
    } catch (...) {
    }
    return power.ACLineStatus == 1
               ? Tr(L"Charge estimate unavailable", L"充满剩余时间待估算")
               : Tr(L"Battery estimate unavailable", L"预计剩余时间待估算");
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
                    ? Tr(L"Internet connected", L"已连接 Internet")
                    : Tr(L"Local network", L"本地网络");
        } else {
            next.networkName = Tr(L"Disconnected", L"未连接");
            next.networkState = Tr(L"No connection", L"无网络连接");
        }
    } catch (...) {
        next.networkName = Tr(L"Network unavailable", L"网络状态暂不可用");
    }
}
inline void ReadStorage(Data& next) {
    wchar_t drive[16]{};
    if (!GetEnvironmentVariableW(L"SystemDrive", drive, 16))
        lstrcpynW(drive, L"C:", 16);
    next.storageName = std::wstring(drive) + Tr(L" System drive", L" 系统盘");
    std::wstring root = std::wstring(drive) + L"\\";
    ULARGE_INTEGER free, total, available;
    if (GetDiskFreeSpaceExW(root.c_str(), &available, &total, &free)) {
        next.diskTotal = total.QuadPart / 1073741824.0;
        next.diskFree = free.QuadPart / 1073741824.0;
        next.diskUsed = next.diskTotal - next.diskFree;
    }
}
inline void MetricsWorker() {
    WorkerApartment apartment;
    Wmi wmi;
    NetworkSample network;
    com_ptr<IMMDeviceEnumerator> enumerator;
    com_ptr<IAudioEndpointVolume> endpoint;
    std::wstring endpointId;

    PDH_HQUERY gpuQuery = nullptr;
    PDH_HCOUNTER gpuCounter = nullptr;
    struct CloseQuery {
        PDH_HQUERY& query;
        ~CloseQuery() {
            if (query)
                PdhCloseQuery(query);
        }
    } closeQuery{gpuQuery};
    bool gpuInitialized = false;
    FILETIME previousIdle = {}, previousKernel = {}, previousUser = {};
    bool previousCpu = false;
    auto ft = [](FILETIME value) {
        return ((uint64_t)value.dwHighDateTime << 32) | value.dwLowDateTime;
    };
    uint64_t lastMetrics = 0, lastSlow = 0;
    unsigned lastEpoch = 0;
    {
        std::lock_guard lock(dataMutex);
        data.device = DeviceName();
    }
    while (!Stopped()) {
        HANDLE events[] = {stop, wake};
        if (!visible && volumeRequest < 0 && muteRequest < 0 &&
            brightnessRequest < 0) {
            WaitForMultipleObjects(2, events, FALSE, INFINITE);
            continue;
        }
        auto settings = std::atomic_load(&config);
        auto now = GetTickCount64();
        const auto epoch = visibilityEpoch.load();
        if (epoch != lastEpoch) {
            lastEpoch = epoch;
            lastMetrics = lastSlow = 0;
            previousCpu = false;
            network = {};
        }
        const bool sample = now - lastMetrics >= 1000;
        if (settings->quick && settings->volume &&
            (sample || volumeRequest >= 0 || muteRequest >= 0)) {
            if (!enumerator)
                CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                 CLSCTX_INPROC_SERVER,
                                 IID_PPV_ARGS(enumerator.put()));
            if (enumerator) {
                com_ptr<IMMDevice> device;
                if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(
                        eRender, eMultimedia, device.put()))) {
                    PWSTR id = nullptr;
                    if (SUCCEEDED(device->GetId(&id))) {
                        if (!endpoint || endpointId != id) {
                            endpoint = nullptr;
                            endpointId = id;
                            device->Activate(__uuidof(IAudioEndpointVolume),
                                             CLSCTX_INPROC_SERVER, nullptr,
                                             endpoint.put_void());
                        }
                        CoTaskMemFree(id);
                    }
                } else {
                    endpoint = nullptr;
                    endpointId.clear();
                }
            }
        } else if (!settings->quick || !settings->volume) {
            endpoint = nullptr;
            endpointId.clear();
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
        if (request >= 0 && settings->quick && settings->brightness &&
            wmi.SetBrightness(request)) {
            std::lock_guard lock(dataMutex);
            data.brightness = request;
            data.brightnessReady = true;
            lastSlow = now;
        }
        if (visible && sample) {
            lastMetrics = now;
            Data next = Snapshot();
            if (settings->network)
                network.Read(next);
            if (settings->overview && settings->performance) {
                if (!gpuInitialized) {
                    gpuInitialized = true;
                    if (PdhOpenQueryW(nullptr, 0, &gpuQuery) == ERROR_SUCCESS &&
                        PdhAddEnglishCounterW(
                            gpuQuery,
                            L"\\GPU Engine(*)\\Utilization Percentage", 0,
                            &gpuCounter) != ERROR_SUCCESS)
                        gpuCounter = nullptr;
                }
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
            next.volume = -1;
            if (endpoint) {
                float volume;
                BOOL mute = FALSE;
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
                if (settings->overview && settings->battery)
                    next.batteryDetail = BatteryDetail();
                if (settings->quick && settings->brightness) {
                    int value = wmi.ReadBrightness();
                    next.brightness = value;
                    next.brightnessReady = value >= 0;
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
        if (WaitForMultipleObjects(2, events, FALSE, visible ? 1000 : 0) ==
            WAIT_OBJECT_0)
            break;
    }
}
inline void MediaWorker();
inline bool InitializeEvents(HANDLE stopEvent) {
    stop = stopEvent;
    wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    mediaWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    return wake && mediaWake;
}
inline void Start() {
    mediaWorker.emplace([] {
        try {
            MediaWorker();
        } catch (...) {
            std::lock_guard lock(dataMutex);
            data.mediaError = Tr(L"Media service unavailable", L"媒体服务异常");
        }
    });
    worker.emplace([] {
        try {
            MetricsWorker();
        } catch (...) {
            Wh_Log(L"System metrics worker stopped after an error");
        }
    });
}
inline void Stop() {
    if (stop)
        SetEvent(stop);
    if (wake)
        SetEvent(wake);
    if (mediaWake)
        SetEvent(mediaWake);
    if (mediaWorker) {
        mediaWorker->join();
        mediaWorker.reset();
    }
    if (worker) {
        worker->join();
        worker.reset();
    }
    if (wake)
        CloseHandle(wake);
    if (mediaWake)
        CloseHandle(mediaWake);
    mediaWake = nullptr;
    wake = nullptr;
    stop = nullptr;
}
}  // namespace svc

namespace svc {
static std::wstring Lower(std::wstring text) {
    std::transform(text.begin(), text.end(), text.begin(), ::towlower);
    return text;
}
static std::wstring ExecutableName(std::wstring const& app) {
    auto lower = Lower(app);
    if (lower == L"neteasenext")
        return L"NetEaseNEXT.exe";
    if (lower == L"cloudmusic" || lower == L"netease")
        return L"cloudmusic.exe";
    if (lower == L"spotify")
        return L"spotify.exe";
    if (lower == L"qqmusic")
        return L"qqmusic.exe";
    if (lower == L"msedge")
        return L"msedge.exe";
    if (lower == L"chrome" || lower == L"google chrome")
        return L"chrome.exe";
    if (lower == L"firefox")
        return L"firefox.exe";
    if (lower == L"foobar2000")
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
// Resolve short SMTC identities through the running app's registered package.
// Some packaged desktop players publish an executable name instead of an AUMID.
static Windows::ApplicationModel::AppInfo MediaAppInfo(
    std::wstring const& source) {
    using Windows::ApplicationModel::AppInfo;
    AppInfo fallback{nullptr};
    try {
        fallback = AppInfo::GetFromAppUserModelId(source);
        if (fallback && fallback.Package())
            return fallback;
    } catch (...) {
    }
    for (auto const& process : SourceProcesses(source)) {
        HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                    process.first);
        if (!handle)
            continue;
        UINT32 length = 0;
        std::wstring identity;
        if (GetApplicationUserModelId(handle, &length, nullptr) ==
                ERROR_INSUFFICIENT_BUFFER &&
            length) {
            std::vector<wchar_t> text(length);
            if (GetApplicationUserModelId(handle, &length, text.data()) ==
                ERROR_SUCCESS)
                identity = text.data();
        }
        CloseHandle(handle);
        if (!identity.empty())
            try {
                auto app = AppInfo::GetFromAppUserModelId(identity);
                if (app)
                    return app;
            } catch (...) {
            }
    }
    return fallback;
}
static bool WithinPackage(std::filesystem::path const& root,
                          std::filesystem::path const& file) {
    std::error_code error;
    auto base = std::filesystem::weakly_canonical(root, error);
    if (error)
        return false;
    auto resolved = std::filesystem::weakly_canonical(file, error);
    if (error)
        return false;
    auto relative = resolved.lexically_relative(base);
    if (relative.empty() || relative.is_absolute())
        return false;
    for (auto const& part : relative)
        if (part == L"..")
            return false;
    return true;
}
static std::shared_ptr<const std::vector<uint8_t>> PackageBytes(
    std::filesystem::path const& root,
    std::filesystem::path const& file,
    size_t maximum) {
    if (!WithinPackage(root, file))
        return {};
    std::ifstream input(file, std::ios::binary | std::ios::ate);
    const auto size = input ? input.tellg() : std::streampos(-1);
    if (size <= 0 || uint64_t(size) > maximum)
        return {};
    auto bytes = std::make_shared<std::vector<uint8_t>>(size_t(size));
    input.seekg(0);
    if (!input.read(reinterpret_cast<char*>(bytes->data()),
                    std::streamsize(bytes->size())))
        return {};
    return bytes;
}
static std::pair<std::shared_ptr<const std::vector<uint8_t>>,
                 std::shared_ptr<const std::vector<uint8_t>>>
ThemedAppLogos(Windows::ApplicationModel::AppInfo const& app) {
    if (!app || !app.Package())
        return {};
    const std::filesystem::path root(
        app.Package().InstalledLocation().Path().c_str());
    auto bytes = PackageBytes(root, root / L"AppxManifest.xml", 512 * 1024);
    if (!bytes)
        return {};
    Windows::Data::Xml::Dom::XmlDocument manifest;
    DataWriter writer;
    writer.WriteBytes(*bytes);
    manifest.LoadXmlFromBuffer(writer.DetachBuffer());
    std::wstring logo;
    for (auto const& application :
         manifest.SelectNodes(L"//*[local-name()='Application']")) {
        auto id = application.Attributes().GetNamedItem(L"Id");
        if (!id || id.InnerText() != app.Id())
            continue;
        auto visual =
            application.SelectSingleNode(L"*[local-name()='VisualElements']");
        auto attribute =
            visual ? visual.Attributes().GetNamedItem(L"Square44x44Logo")
                   : nullptr;
        if (attribute)
            logo = attribute.InnerText();
        break;
    }
    if (logo.empty() || logo.find(L':') != std::wstring::npos)
        return {};
    const auto base = (root / std::filesystem::path(logo)).lexically_normal();
    if (!WithinPackage(root, base) ||
        Lower(base.extension().wstring()) != L".png")
        return {};
    const auto stem = Lower(base.stem().wstring());
    std::filesystem::path chosen[2];
    double best[2] = {INFINITY, INFINITY};
    std::error_code error;
    for (auto const& item :
         std::filesystem::directory_iterator(base.parent_path(), error)) {
        if (error)
            break;
        auto name = Lower(item.path().filename().wstring());
        if (!name.ends_with(L".png") ||
            !(name == stem + L".png" || name.starts_with(stem + L".")) ||
            name.find(L"contrast-") != std::wstring::npos)
            continue;
        double pixels = 44;
        auto target = name.find(L"targetsize-");
        auto scale = name.find(L"scale-");
        if (target != std::wstring::npos)
            pixels = std::wcstoul(name.c_str() + target + 11, nullptr, 10);
        else if (scale != std::wstring::npos)
            pixels *=
                std::wcstoul(name.c_str() + scale + 6, nullptr, 10) / 100.0;
        if (pixels <= 0 || pixels > 4096)
            continue;
        for (unsigned light = 0; light < 2; ++light) {
            const bool lightForm =
                name.find(L"altform-lightunplated") != std::wstring::npos;
            const bool darkForm =
                name.find(L"altform-unplated") != std::wstring::npos;
            const bool lightTheme =
                name.find(L"theme-light") != std::wstring::npos;
            const bool darkTheme =
                name.find(L"theme-dark") != std::wstring::npos;
            if ((light && (darkTheme || darkForm)) ||
                (!light && (lightTheme || lightForm)))
                continue;
            const double form = (light ? lightForm : darkForm)     ? 0
                                : (light ? lightTheme : darkTheme) ? 1
                                                                   : 2;
            // One sharp image also serves the larger no-artwork fallback.
            const double score =
                form * 10000 +
                (pixels >= 192 ? pixels - 192 : 1000 + 192 - pixels);
            if (score < best[light]) {
                best[light] = score;
                chosen[light] = item.path();
            }
        }
    }
    auto dark = chosen[0].empty()
                    ? nullptr
                    : PackageBytes(root, chosen[0], 8 * 1024 * 1024);
    auto light = chosen[1] == chosen[0] ? dark
                 : chosen[1].empty()
                     ? nullptr
                     : PackageBytes(root, chosen[1], 8 * 1024 * 1024);
    return {light, dark};
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
    if (app.find_first_of(L"\\/:") != std::wstring::npos)
        return context.window != nullptr;
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
    ~CoverRead() { Cancel(); }
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
    bool Poll(std::shared_ptr<const std::vector<uint8_t>>& destination,
              uint64_t& revision) {
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
                if (!bytes.empty() && (value != hash || !destination)) {
                    hash = value;
                    destination = std::make_shared<const std::vector<uint8_t>>(
                        std::move(bytes));
                    ++revision;
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
    WorkerApartment apartment;
    using Props = GlobalSystemMediaTransportControlsSessionMediaProperties;
    using PropsOperation = Windows::Foundation::IAsyncOperation<Props>;
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    GlobalSystemMediaTransportControlsSession watched{nullptr};
    PropsOperation properties{nullptr};
    CoverRead artwork, logo;
    ULONGLONG retry = 0, propertiesStarted = 0, metadataNext = 0,
              followUpUntil = 0;
    std::wstring source, artworkKey, logoIdentity;
    bool artForTrack = false;
    auto detach = [&] {
        watched = nullptr;
        artForTrack = false;
        if (properties) {
            try {
                properties.Cancel();
            } catch (...) {
            }
            properties = nullptr;
        }
        artwork.Cancel();
        artwork.hash = 0;
        logo.Cancel();
        logo.hash = 0;
    };
    while (!Stopped()) {
        auto settings = std::atomic_load(&config);
        auto now = GetTickCount64();
        HANDLE events[] = {stop, mediaWake};
        if (!settings->media || (!visible && mediaRequest == 0)) {
            detach();
            manager = nullptr;
            retry = 0;
            if (!settings->media)
                mediaRequest = 0;
            WaitForMultipleObjects(2, events, FALSE, INFINITE);
            continue;
        }
        if (!manager && (!retry || now - retry >= 5000)) {
            retry = now;
            try {
                manager =
                    Await(GlobalSystemMediaTransportControlsSessionManager::
                              RequestAsync());
            } catch (hresult_error const& ex) {
                std::lock_guard lock(dataMutex);
                data.mediaError = Tr(L"Media controls unavailable (",
                                     L"系统媒体接口不可用 (") +
                                  std::to_wstring((unsigned)ex.code()) + L")";
            }
        }
        Data next = Snapshot();
        GlobalSystemMediaTransportControlsSession session{nullptr};
        if (manager)
            try {
                session = manager.GetCurrentSession();
                next.mediaError.clear();
            } catch (...) {
                manager = nullptr;
            }
        try {
            if (session != watched || (!session && !next.mediaApp.empty())) {
                detach();
                watched = session;
                metadataNext = 0;
                artworkKey.clear();
                source = session ? std::wstring(session.SourceAppUserModelId())
                                 : L"";
                if (next.mediaApp != source) {
                    next.art.reset();
                    next.mediaLogo.reset();
                    next.mediaLogoLight.reset();
                    next.mediaLogoDark.reset();
                    ++next.artRevision;
                }
                next.mediaApp = source;
                if (watched) {
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
                        auto app = MediaAppInfo(source);
                        if (app) {
                            std::wstring identity(source);
                            if (auto package = app.Package())
                                identity =
                                    std::wstring(package.Id().FullName()) +
                                    L"!" + std::wstring(app.Id());
                            if (identity != logoIdentity) {
                                logoIdentity = std::move(identity);
                                logo.Cancel();
                                logo.hash = 0;
                                next.mediaLogo.reset();
                                next.mediaLogoLight.reset();
                                next.mediaLogoDark.reset();
                            }
                            auto display = app.DisplayInfo();
                            auto name = display.DisplayName();
                            if (!name.empty())
                                next.mediaSource = name;
                            if (!next.mediaLogo)
                                logo.Start(display.GetLogo({192, 192}));
                            if (!next.mediaLogoLight && !next.mediaLogoDark)
                                try {
                                    auto themed = ThemedAppLogos(app);
                                    next.mediaLogoLight =
                                        std::move(themed.first);
                                    next.mediaLogoDark =
                                        std::move(themed.second);
                                    if (next.mediaLogoLight ||
                                        next.mediaLogoDark)
                                        ++next.artRevision;
                                } catch (...) {
                                }
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
                if (!properties && now >= metadataNext) {
                    properties = watched.TryGetMediaPropertiesAsync();
                    propertiesStarted = now;
                    metadataNext = now + 1000;
                }
                if (properties) {
                    auto status = properties.Status();
                    if (status == Windows::Foundation::AsyncStatus::Completed) {
                        auto value = properties.GetResults();
                        properties = nullptr;
                        next.title = value.Title();
                        if (next.title.empty())
                            next.title = Tr(L"Untitled", L"未提供标题");
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
                            if (!artwork.Pending() &&
                                (!artForTrack || now < followUpUntil))
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
                if (artwork.Poll(next.art, next.artRevision))
                    artForTrack = true;
                logo.Poll(next.mediaLogo, next.artRevision);
            } else {
                next.title = Tr(L"No media playing", L"暂无媒体播放");
                next.artist = L"";
                next.mediaSource = Tr(L"Media", L"媒体播放");
                next.mediaApp.clear();
                next.playing = next.previous = next.next = next.toggle = false;
            }
        } catch (hresult_error const& ex) {
            next.mediaError =
                Tr(L"Media status unavailable (", L"媒体状态暂不可用 (") +
                std::to_wstring((unsigned)ex.code()) + L")";
        }
        {
            std::lock_guard lock(dataMutex);
            data.title = next.title;
            data.artist = next.artist;
            data.mediaSource = next.mediaSource;
            data.mediaApp = next.mediaApp;
            data.mediaError = next.mediaError;
            data.playing = next.playing;
            data.previous = next.previous;
            data.next = next.next;
            data.toggle = next.toggle;
            data.art = next.art;
            data.mediaLogo = next.mediaLogo;
            data.mediaLogoLight = next.mediaLogoLight;
            data.mediaLogoDark = next.mediaLogoDark;
            data.artRevision = next.artRevision;
        }
        if (WaitForMultipleObjects(2, events, FALSE, visible ? 200 : 0) ==
            WAIT_OBJECT_0)
            break;
    }
    detach();
}
}  // namespace svc

using namespace winrt;
namespace x = Windows::UI::Xaml;
namespace c = Windows::UI::Xaml::Controls;
namespace m = Windows::UI::Xaml::Media;
namespace f = Windows::Foundation;
namespace core = Windows::UI::Core;
static std::atomic<bool> g_unloading = false;
static std::atomic<HWND> g_hwnd;
static HANDLE g_startup, g_startupWake, g_stop;
static std::atomic<bool> g_uiReady = false;
static constexpr wchar_t kCallMessage[] = L"Local.StartSidecarXaml.UiCall.020";
[[clang::no_destroy]] static core::CoreWindow g_core{nullptr};
static event_token g_visible{}, g_activated{}, g_closed{}, g_sizeChanged{},
    g_updateTick{}, g_layoutTick{};
[[clang::no_destroy]] static x::DispatcherTimer g_update{nullptr},
    g_layoutTimer{nullptr};
[[clang::no_destroy]] static c::Border g_card{nullptr};
[[clang::no_destroy]] static c::Grid g_host{nullptr};
[[clang::no_destroy]] static c::Border g_shadowSource{nullptr};
[[clang::no_destroy]] static std::optional<std::vector<c::Border>>
    g_shadowCasters;
[[clang::no_destroy]] static std::optional<std::vector<c::Grid>> g_shadowMotion;
struct DetachedShadow {
    c::Grid surface;
    c::Grid motion;
};
[[clang::no_destroy]] static std::optional<std::vector<DetachedShadow>>
    g_detachedShadows;
[[clang::no_destroy]] static c::Canvas g_shadowLayer{nullptr};
[[clang::no_destroy]] static m::MatrixTransform g_shadowTransform{nullptr};
[[clang::no_destroy]] static std::optional<
    m::CompositionTarget::Rendering_revoker> g_shadowRendering;
static ULONGLONG g_shadowKeepUntil = 0;
static float g_shadowElevation = 0;
[[clang::no_destroy]] static c::TextBlock g_clock{nullptr}, g_date{nullptr},
    g_battery{nullptr}, g_power{nullptr}, g_memory{nullptr},
    g_available{nullptr};
[[clang::no_destroy]] static c::TextBlock g_title{nullptr},
    g_powerSource{nullptr}, g_cpu{nullptr}, g_gpu{nullptr};
static bool g_layoutReady = false;
[[clang::no_destroy]] static x::DependencyObject g_layoutRoot{nullptr};
[[clang::no_destroy]] static c::Grid g_compactLayer{nullptr};
[[clang::no_destroy]] static c::Border g_compactPanel{nullptr};
[[clang::no_destroy]] static c::Button g_compactButton{nullptr},
    g_compactBack{nullptr};
static int g_layoutAttempts = 0;
// Rectangular free-space packing. This has no dependency on screen orientation
// or taskbar alignment; the native menu and companion are fixed obstacles.
namespace packing {
struct Rect {
    double x = 0, y = 0, w = 0, h = 0;
    double Right() const { return x + w; }
    double Bottom() const { return y + h; }
    bool operator==(Rect const&) const = default;
};
struct Size {
    double w, h;
    bool compact = false;
};
struct Spec {
    std::vector<Size> sizes;
    double idealWidth = 0, minWidth = 0;
    bool overview = false;
};
struct Placement {
    Rect rect;
    bool compact = false;
};
inline bool Intersects(Rect const& a, Rect const& b) {
    return a.x < b.Right() - 0.01 && b.x < a.Right() - 0.01 &&
           a.y < b.Bottom() - 0.01 && b.y < a.Bottom() - 0.01;
}
inline bool Contains(Rect const& a, Rect const& b) {
    return b.x >= a.x - 0.01 && b.y >= a.y - 0.01 &&
           b.Right() <= a.Right() + 0.01 && b.Bottom() <= a.Bottom() + 0.01;
}
inline Rect Inflate(Rect r, double gap) {
    return {r.x - gap, r.y - gap, r.w + gap * 2, r.h + gap * 2};
}
inline Rect Union(Rect const& a, Rect const& b) {
    return {std::min(a.x, b.x), std::min(a.y, b.y),
            std::max(a.Right(), b.Right()) - std::min(a.x, b.x),
            std::max(a.Bottom(), b.Bottom()) - std::min(a.y, b.y)};
}
inline std::vector<Rect> Subtract(std::vector<Rect> const& free,
                                  Rect const& used) {
    std::vector<Rect> next;
    for (auto const& r : free) {
        if (!Intersects(r, used)) {
            next.push_back(r);
            continue;
        }
        if (used.x > r.x)
            next.push_back({r.x, r.y, used.x - r.x, r.h});
        if (used.Right() < r.Right())
            next.push_back({used.Right(), r.y, r.Right() - used.Right(), r.h});
        if (used.y > r.y)
            next.push_back({r.x, r.y, r.w, used.y - r.y});
        if (used.Bottom() < r.Bottom())
            next.push_back(
                {r.x, used.Bottom(), r.w, r.Bottom() - used.Bottom()});
    }
    for (size_t i = 0; i < next.size();) {
        bool redundant = next[i].w < 1 || next[i].h < 1;
        for (size_t j = 0; j < next.size() && !redundant; ++j)
            redundant = i != j && Contains(next[j], next[i]) &&
                        (next[j] != next[i] || j < i);
        if (redundant)
            next.erase(next.begin() + i);
        else
            ++i;
    }
    return next;
}
inline std::vector<Rect> Free(Rect const& bounds,
                              Rect const& obstacle,
                              double gap) {
    return Subtract({bounds}, Inflate(obstacle, gap));
}
struct State {
    std::vector<Rect> free;
    std::vector<Placement> placed;
    Rect hull;
    double cost = 0;
    double Rank() const { return cost + hull.w * hull.h * 0.0007; }
};
inline void FillRows(std::vector<Placement>& plan,
                     Rect const& bounds,
                     Rect const& native,
                     double gap) {
    Rect hull = native;
    for (auto const& p : plan)
        hull = Union(hull, p.rect);
    for (size_t i = 0; i < plan.size(); ++i) {
        if (plan[i].rect.h > 200)
            continue;
        std::vector<size_t> row;
        for (size_t j = 0; j < plan.size(); ++j)
            if (std::abs(plan[j].rect.y - plan[i].rect.y) < 0.5 &&
                std::abs(plan[j].rect.h - plan[i].rect.h) < 0.5)
                row.push_back(j);
        if (row.empty() || i != row.front())
            continue;
        std::sort(row.begin(), row.end(), [&](auto a, auto b) {
            return plan[a].rect.x < plan[b].rect.x;
        });
        Rect band{
            std::max(bounds.x, hull.x), plan[i].rect.y,
            std::min(bounds.Right(), hull.Right()) - std::max(bounds.x, hull.x),
            plan[i].rect.h};
        if (Intersects(band, Inflate(native, gap)))
            continue;
        bool clear = true;
        for (size_t j = 0; j < plan.size(); ++j)
            if (std::find(row.begin(), row.end(), j) == row.end() &&
                Intersects(band, Inflate(plan[j].rect, gap)))
                clear = false;
        double sum = 0;
        for (auto index : row)
            sum += plan[index].rect.w;
        const double room = band.w - gap * (row.size() - 1);
        if (!clear || room < sum || sum <= 0)
            continue;
        double x = band.x;
        for (auto index : row) {
            auto& r = plan[index].rect;
            r.x = x;
            r.w *= room / sum;
            x = r.Right() + gap;
        }
    }
}
inline std::vector<Placement> Solve(Rect const& bounds,
                                    Rect const& native,
                                    std::vector<Spec> const& specs,
                                    double gap) {
    if (bounds.w <= 0 || bounds.h <= 0)
        return {};
    std::vector<State> states{{Free(bounds, native, gap), {}, native, 0}};
    const bool flanks =
        native.x - bounds.x >= 230 && bounds.Right() - native.Right() >= 230;
    for (auto const& spec : specs) {
        std::vector<State> next;
        for (auto const& state : states) {
            std::vector<Placement> seen;
            for (auto const& space : state.free) {
                auto sizes = spec.sizes;
                if (!spec.overview && space.w >= spec.minWidth &&
                    space.h >= 164) {
                    const auto h = std::min(176.0, space.h);
                    for (double w : {spec.idealWidth, spec.minWidth,
                                     std::min(space.w, spec.idealWidth * 1.4)})
                        sizes.push_back({w, h});
                }
                for (auto const& size : sizes) {
                    if (size.w > space.w || size.h > space.h)
                        continue;
                    for (double x :
                         {space.x, space.Right() - size.w,
                          std::clamp(native.x, space.x, space.Right() - size.w),
                          std::clamp(native.Right() - size.w, space.x,
                                     space.Right() - size.w)})
                        for (double y :
                             {space.y, space.Bottom() - size.h,
                              std::clamp(native.y, space.y,
                                         space.Bottom() - size.h),
                              std::clamp(native.Bottom() - size.h, space.y,
                                         space.Bottom() - size.h)}) {
                            Rect rect{x, y, size.w, size.h};
                            if (std::any_of(seen.begin(), seen.end(),
                                            [&](auto const& p) {
                                                return p.rect == rect &&
                                                       p.compact ==
                                                           size.compact;
                                            }))
                                continue;
                            seen.push_back({rect, size.compact});
                            State candidate = state;
                            candidate.free =
                                Subtract(state.free, Inflate(rect, gap));
                            candidate.placed.push_back({rect, size.compact});
                            candidate.hull = Union(state.hull, rect);
                            const auto dx =
                                std::max({native.x - rect.Right(),
                                          rect.x - native.Right(), 0.0});
                            const auto dy =
                                std::max({native.y - rect.Bottom(),
                                          rect.y - native.Bottom(), 0.0});
                            candidate.cost += (dx + dy) * 0.3;
                            if (flanks && dy > 0)
                                candidate.cost += 32;
                            if (spec.overview)
                                candidate.cost += size.compact ? 35 : -60;
                            else
                                candidate.cost +=
                                    std::abs(size.w - spec.idealWidth) * 0.08;
                            next.push_back(std::move(candidate));
                        }
                }
            }
        }
        if (next.empty())
            return {};
        std::stable_sort(
            next.begin(), next.end(),
            [](auto const& a, auto const& b) { return a.Rank() < b.Rank(); });
        if (next.size() > 32)
            next.resize(32);
        states = std::move(next);
    }
    auto result = states.front().placed;
    FillRows(result, bounds, native, gap);
    return result;
}
}  // namespace packing

struct WorkArea {
    double width = 0, height = 0, left = 0, top = 0;
    bool operator==(WorkArea const&) const = default;
};
static WorkArea g_workArea;
struct NativeGeometry {
    packing::Rect menu, occupied;
    double hostX = 0, hostY = 0;
    bool phoneVisible = false;
    bool operator==(NativeGeometry const&) const = default;
};
static NativeGeometry g_nativeGeometry;
[[clang::no_destroy]] static c::Canvas g_cardsLayer{nullptr};
struct PackedCard {
    c::Border card;
    packing::Rect rect;
    bool overview;
};
[[clang::no_destroy]] static std::optional<std::vector<PackedCard>>
    g_packedCards;

struct NativeProperty {
    x::FrameworkElement element;
    x::DependencyProperty property;
    f::IInspectable value;
    x::Data::Binding binding{nullptr};
};
[[clang::no_destroy]] static std::optional<std::vector<NativeProperty>>
    g_nativeProperties;
[[clang::no_destroy]] static std::optional<std::vector<c::ColumnDefinition>>
    g_nativeColumns;
[[clang::no_destroy]] static std::optional<std::vector<c::RowDefinition>>
    g_nativeRows;
static void RestoreNativeBounds();

static WorkArea GetWorkArea() {
    const HWND hwnd = g_hwnd.load();
    MONITORINFO monitor{sizeof(monitor)};
    RECT client{};
    POINT origin{};
    const auto dpi = GetDpiForWindow(hwnd);
    if (!dpi || !GetClientRect(hwnd, &client) ||
        !ClientToScreen(hwnd, &origin) ||
        !GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                         &monitor))
        return {};
    const double scale = 96.0 / dpi;
    const auto left = std::max(0L, monitor.rcWork.left - origin.x);
    const auto top = std::max(0L, monitor.rcWork.top - origin.y);
    const auto right = std::min(client.right, monitor.rcWork.right - origin.x);
    const auto bottom =
        std::min(client.bottom, monitor.rcWork.bottom - origin.y);
    return {std::max(0L, right - left) * scale,
            std::max(0L, bottom - top) * scale, left * scale, top * scale};
}

[[clang::no_destroy]] static std::optional<std::vector<std::function<void()>>>
    g_cardEvents;
static void RemoveBottom();
static void UpdateBottom(svc::Data const&);
static void QueueLayout();

template <typename F>
static void GuardUi(F&& action) noexcept {
    try {
        action();
    } catch (hresult_error const& error) {
        Wh_Log(L"Start Sidecar XAML error %#x: %s", (unsigned)error.code(),
               error.message().c_str());
    } catch (...) {
        Wh_Log(L"Unexpected Start Sidecar UI error");
    }
}

template <typename Revoker>
static void Track(Revoker revoker) {
    auto subscription = std::make_shared<Revoker>(std::move(revoker));
    g_cardEvents->emplace_back([subscription] { subscription->revoke(); });
}

static void LabelControl(x::FrameworkElement const& control,
                         const wchar_t* text) {
    x::Automation::AutomationProperties::SetName(control, text);
    c::ToolTipService::SetToolTip(control, box_value(text));
}

static void BindSurfaceProperty(x::FrameworkElement const& target,
                                c::Border const& source,
                                x::DependencyProperty const& property,
                                const wchar_t* path) {
    x::Data::Binding binding;
    binding.Source(source);
    binding.Path(x::PropertyPath(path));
    target.SetBinding(property, binding);
}

static void ApplyNativeSurface(c::Border const& card, c::Border const& native) {
    // The native brush can be replaced on a theme change. A binding keeps both
    // the acrylic and the stroke in sync with the native surface.
    if (native.Background())
        BindSurfaceProperty(card, native, c::Border::BackgroundProperty(),
                            L"Background");
    if (native.BorderBrush())
        BindSurfaceProperty(card, native, c::Border::BorderBrushProperty(),
                            L"BorderBrush");
    card.BorderThickness(native.BorderThickness());
    card.CornerRadius(native.CornerRadius());
}

static void SyncShadowAnimation() {
    if (!g_shadowSource || !g_shadowTransform)
        return;
    auto parent =
        m::VisualTreeHelper::GetParent(g_shadowSource).try_as<x::UIElement>();
    if (!parent)
        return;
    const double width = std::max(1.0, g_shadowSource.ActualWidth());
    const double height = std::max(1.0, g_shadowSource.ActualHeight());
    auto transform = g_shadowSource.TransformToVisual(parent);
    const auto origin = transform.TransformPoint({0, 0});
    const auto across =
        transform.TransformPoint({static_cast<float>(width), 0});
    const auto down = transform.TransformPoint({0, static_cast<float>(height)});
    const auto offset = g_shadowSource.ActualOffset();
    m::Matrix matrix{
        (across.X - origin.X) / width, (across.Y - origin.Y) / width,
        (down.X - origin.X) / height,  (down.Y - origin.Y) / height,
        origin.X - offset.x,           origin.Y - offset.y};
    const auto old = g_shadowTransform.Matrix();
    if (std::abs(old.M11 - matrix.M11) > 0.00001 ||
        std::abs(old.M12 - matrix.M12) > 0.00001 ||
        std::abs(old.M21 - matrix.M21) > 0.00001 ||
        std::abs(old.M22 - matrix.M22) > 0.00001 ||
        std::abs(old.OffsetX - matrix.OffsetX) > 0.001 ||
        std::abs(old.OffsetY - matrix.OffsetY) > 0.001)
        g_shadowTransform.Matrix(matrix);
}

static void EnsureShadowAnimation() {
    if (!g_shadowTransform || g_shadowRendering ||
        (!svc::visible && GetTickCount64() >= g_shadowKeepUntil))
        return;
    // Track the actual shell animation, including private transition transforms
    // and interactive touch gestures, for as long as Start is visible. Keep a
    // short tail for the closing animation, then stop requesting render frames.
    g_shadowRendering.emplace(m::CompositionTarget::Rendering(
        auto_revoke, [](auto const&, auto const&) {
            GuardUi([] {
                SyncShadowAnimation();
                if (!svc::visible && GetTickCount64() >= g_shadowKeepUntil)
                    g_shadowRendering.reset();
            });
        }));
}

static c::Grid WrapSurface(c::Border const& card,
                           bool shadow = true,
                           bool detached = true) {
    c::Grid surface;
    if (shadow && g_shadowSource) {
        c::Grid motion;
        motion.IsHitTestVisible(false);
        c::Border caster;
        caster.Name(L"StartSidecarShadow");
        caster.IsHitTestVisible(false);
        caster.CornerRadius(card.CornerRadius());
        caster.Translation({0, 0, g_shadowSource.Translation().z});
        BindSurfaceProperty(caster, g_shadowSource,
                            x::UIElement::ShadowProperty(), L"Shadow");
        BindSurfaceProperty(caster, g_shadowSource,
                            x::UIElement::VisibilityProperty(), L"Visibility");
        motion.Children().Append(caster);
        if (!g_shadowTransform)
            g_shadowTransform = m::MatrixTransform();
        motion.RenderTransform(g_shadowTransform);
        BindSurfaceProperty(motion, g_shadowSource,
                            x::UIElement::OpacityProperty(), L"Opacity");
        if (detached) {
            if (!g_detachedShadows)
                g_detachedShadows.emplace();
            g_detachedShadows->push_back({surface, motion});
        } else {
            surface.Children().Append(motion);
        }
        if (!g_shadowMotion)
            g_shadowMotion.emplace();
        g_shadowMotion->push_back(motion);
        if (!g_shadowCasters)
            g_shadowCasters.emplace();
        g_shadowCasters->push_back(caster);
    }
    surface.Children().Append(card);
    return surface;
}

static x::FrameworkElement Resolve(x::DependencyObject root, std::wstring path);

static void SyncDetachedShadows() {
    if (!g_layoutReady || !g_layoutRoot || !g_detachedShadows)
        return;
    auto host = g_shadowLayer
                    ? g_shadowLayer.Parent().try_as<c::Grid>()
                    : Resolve(g_layoutRoot, L"AnimationRoot").try_as<c::Grid>();
    if (!host)
        return;
    if (!g_shadowLayer) {
        g_shadowLayer = c::Canvas();
        g_shadowLayer.Name(L"StartSidecarShadowLayer");
        g_shadowLayer.IsHitTestVisible(false);
        c::Canvas::SetZIndex(g_shadowLayer, -1);
        host.Children().InsertAt(0, g_shadowLayer);
        Track(host.LayoutUpdated(auto_revoke, [](auto const&, auto const&) {
            GuardUi([] { SyncDetachedShadows(); });
        }));
    }
    auto columns = std::max(1u, host.ColumnDefinitions().Size());
    auto rows = std::max(1u, host.RowDefinitions().Size());
    if (c::Grid::GetColumnSpan(g_shadowLayer) != static_cast<int>(columns))
        c::Grid::SetColumnSpan(g_shadowLayer, columns);
    if (c::Grid::GetRowSpan(g_shadowLayer) != static_cast<int>(rows))
        c::Grid::SetRowSpan(g_shadowLayer, rows);
    for (auto const& entry : *g_detachedShadows) {
        if (!entry.surface.Parent())
            continue;
        if (!entry.motion.Parent())
            g_shadowLayer.Children().Append(entry.motion);
        const auto point =
            entry.surface.TransformToVisual(host).TransformPoint({0, 0});
        if (c::Canvas::GetLeft(entry.motion) != point.X)
            c::Canvas::SetLeft(entry.motion, point.X);
        if (c::Canvas::GetTop(entry.motion) != point.Y)
            c::Canvas::SetTop(entry.motion, point.Y);
        if (entry.motion.Width() != entry.surface.ActualWidth())
            entry.motion.Width(entry.surface.ActualWidth());
        if (entry.motion.Height() != entry.surface.ActualHeight())
            entry.motion.Height(entry.surface.ActualHeight());
    }
}

static void SyncShadowElevation() {
    SyncShadowAnimation();
    EnsureShadowAnimation();
    SyncDetachedShadows();
    if (!g_shadowSource || !g_shadowCasters)
        return;
    float elevation = g_shadowSource.Translation().z;
    if (elevation == g_shadowElevation)
        return;
    g_shadowElevation = elevation;
    for (auto const& caster : *g_shadowCasters)
        caster.Translation({0, 0, elevation});
}
static void LoadSettings() {
    auto settings = std::make_shared<svc::Config>();
    settings->overview = Wh_GetIntSetting(L"showOverview") != 0;
    settings->time = Wh_GetIntSetting(L"showTime") != 0;
    settings->battery = Wh_GetIntSetting(L"showBattery") != 0;
    settings->memory = Wh_GetIntSetting(L"showMemory") != 0;
    settings->performance = Wh_GetIntSetting(L"showPerformance") != 0;
    settings->media = Wh_GetIntSetting(L"showMedia") != 0;
    settings->quick = Wh_GetIntSetting(L"showQuickSettings") != 0;
    settings->volume = Wh_GetIntSetting(L"showVolume") != 0;
    settings->brightness = Wh_GetIntSetting(L"showBrightness") != 0;
    settings->network = Wh_GetIntSetting(L"showNetwork") != 0;
    settings->storage = Wh_GetIntSetting(L"showStorage") != 0;
    int width = Wh_GetIntSetting(L"overviewWidth");
    settings->overviewWidth = width >= 200 && width <= 340 ? width : 252;
    settings->cardGap = std::clamp(Wh_GetIntSetting(L"cardGap"), -1, 24);
    settings->title = WindhawkUtils::StringSetting::make(L"deviceTitle").get();
    svc::Configure(settings);
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
static void RemoveCard() {
    g_layoutReady = false;
    g_layoutRoot = nullptr;
    g_shadowRendering.reset();
    g_shadowTransform = nullptr;
    GuardUi([] {
        if (g_update)
            g_update.Stop();
    });
    if (g_cardEvents) {
        for (auto const& unsubscribe : *g_cardEvents)
            GuardUi(unsubscribe);
        g_cardEvents.reset();
    }
    for (auto const& layer : {g_shadowLayer.try_as<x::FrameworkElement>(),
                              g_cardsLayer.try_as<x::FrameworkElement>(),
                              g_compactLayer.try_as<x::FrameworkElement>()}) {
        GuardUi([&] {
            if (layer)
                if (auto parent = layer.Parent().try_as<c::Panel>()) {
                    uint32_t index;
                    if (parent.Children().IndexOf(layer, index))
                        parent.Children().RemoveAt(index);
                }
        });
    }
    RestoreNativeBounds();
    RemoveBottom();
    g_shadowLayer = g_cardsLayer = nullptr;
    g_packedCards.reset();
    g_detachedShadows.reset();
    g_shadowMotion.reset();
    g_shadowCasters.reset();
    g_shadowSource = nullptr;
    g_shadowElevation = 0;
    g_compactBack = g_compactButton = nullptr;
    g_compactPanel = nullptr;
    g_compactLayer = nullptr;
    g_card = nullptr;
    g_host = nullptr;
    g_clock = g_date = g_battery = g_power = g_memory = g_available = nullptr;
    g_title = g_powerSource = g_cpu = g_gpu = nullptr;
}
static void UpdateData() {
    SyncShadowElevation();
    auto snapshot = svc::Snapshot();
    UpdateBottom(snapshot);
    if (!g_card || !g_clock)
        return;
    auto settings = std::atomic_load(&svc::config);
    g_title.Text(settings->title.empty() ? snapshot.device : settings->title);
    SYSTEMTIME now;
    GetLocalTime(&now);
    wchar_t clock[64]{}, date[96]{}, buffer[96];
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &now, nullptr,
                    clock, ARRAYSIZE(clock));
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &now, nullptr,
                    date, ARRAYSIZE(date), nullptr);
    g_clock.Text(clock);
    g_date.Text(date);
    SYSTEM_POWER_STATUS power = {};
    if (GetSystemPowerStatus(&power) && power.BatteryLifePercent != 255) {
        if (power.BatteryFlag & 128)
            g_battery.Text(Tr(L"No battery", L"无电池"));
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
        g_power.Text(Tr(L"Estimate unavailable", L"剩余时间待估算"));
    }
    MEMORYSTATUSEX memory = {};
    memory.dwLength = sizeof(memory);
    if (GlobalMemoryStatusEx(&memory)) {
        swprintf(buffer, 96, L"%u%%", memory.dwMemoryLoad);
        g_memory.Text(buffer);
        swprintf(buffer, 96,
                 Tr(L"Used %.1f / %.1f GB", L"已使用 %.1f / %.1f GB"),
                 (memory.ullTotalPhys - memory.ullAvailPhys) / 1073741824.0,
                 memory.ullTotalPhys / 1073741824.0);
        g_available.Text(buffer);
    } else {
        g_memory.Text(L"—");
        g_available.Text(Tr(L"Memory unavailable", L"内存状态未知"));
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
 BorderBrush="{ThemeResource SystemControlForegroundBaseLowBrush}" BorderThickness="1">
 <Grid>
  <Grid.RowDefinitions><RowDefinition Height="*"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
  <ScrollViewer x:Name="OverviewScroll" HorizontalScrollMode="Disabled" HorizontalScrollBarVisibility="Disabled" VerticalScrollBarVisibility="Auto">
  <StackPanel>
   <TextBlock x:Name="DeviceTitle" FontSize="20" FontWeight="SemiBold" TextTrimming="CharacterEllipsis" Margin="0,0,0,12"/>
   <Border x:Name="TimeTile" Background="{ThemeResource SystemControlBackgroundBaseLowBrush}" CornerRadius="10" Padding="10" Margin="0,0,0,8">
    <StackPanel><TextBlock x:Name="TimeLabel" FontSize="12" Opacity="0.7"/>
     <TextBlock x:Name="ClockValue" FontSize="34" FontWeight="SemiBold" TextTrimming="CharacterEllipsis" Margin="0,3,0,4"/>
     <TextBlock x:Name="DateValue" FontSize="12"/></StackPanel>
   </Border>
   <Border x:Name="BatteryTile" Background="{ThemeResource SystemControlBackgroundBaseLowBrush}" CornerRadius="10" Padding="10" Margin="0,0,0,8">
    <StackPanel><TextBlock x:Name="BatteryLabel" FontSize="12" Opacity="0.7"/>
     <StackPanel Orientation="Horizontal" Margin="0,4,0,2"><TextBlock x:Name="BatteryValue" FontSize="26" FontWeight="SemiBold"/><TextBlock x:Name="PowerSourceValue" FontSize="14" Margin="10,0,0,0" VerticalAlignment="Center" Opacity="0.75"/></StackPanel>
     <TextBlock x:Name="PowerValue" FontSize="12"/></StackPanel>
   </Border>
   <Border x:Name="MemoryTile" Background="{ThemeResource SystemControlBackgroundBaseLowBrush}" CornerRadius="10" Padding="10" Margin="0,0,0,8">
    <StackPanel><TextBlock x:Name="MemoryLabel" FontSize="12" Opacity="0.7"/>
     <TextBlock x:Name="MemoryValue" FontSize="26" FontWeight="SemiBold" Margin="0,4,0,2"/>
     <TextBlock x:Name="AvailableValue" FontSize="12"/></StackPanel>
   </Border>
   <Border x:Name="PerformanceTile" Background="{ThemeResource SystemControlBackgroundBaseLowBrush}" CornerRadius="10" Padding="10">
    <Grid><Grid.ColumnDefinitions><ColumnDefinition Width="*"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions><TextBlock x:Name="CpuValue" Text="CPU —" FontSize="17"/><TextBlock x:Name="GpuValue" Text="GPU —" Grid.Column="1" FontSize="17" HorizontalAlignment="Right"/></Grid>
   </Border>
  </StackPanel>
  </ScrollViewer>
  <Button x:Name="SettingsButton" Grid.Row="1" HorizontalAlignment="Stretch" Margin="0,10,0,0"/>
 </Grid>
</Border>)XAML";

namespace mediaCard {
// Layout and transport styling adapted from this machine's MediaPeek mod.
// Session selection continues to follow the system's current media session.
[[clang::no_destroy]] inline c::Grid view{nullptr};
[[clang::no_destroy]] inline std::shared_ptr<const std::vector<uint8_t>>
    shownLogo, shownArt;
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
  <TextBlock x:Name="SourceLabel" Grid.Column="1" FontSize="12" VerticalAlignment="Center"
   Foreground="{ThemeResource SystemControlForegroundBaseMediumBrush}" TextTrimming="CharacterEllipsis"/>
 </Grid>
 <StackPanel Grid.Row="2" Spacing="2" VerticalAlignment="Center">
   <TextBlock x:Name="TitleText" FontSize="14" FontWeight="SemiBold" MaxLines="1"
    Foreground="{ThemeResource SystemControlForegroundBaseHighBrush}" TextTrimming="CharacterEllipsis"/>
   <TextBlock x:Name="ArtistText" FontSize="12" MaxLines="1" TextTrimming="CharacterEllipsis"
    Foreground="{ThemeResource SystemControlForegroundBaseMediumBrush}"/>
 </StackPanel>
  <Button x:Name="CoverButton" Grid.Row="0" Grid.RowSpan="3" Grid.Column="1" Width="92" Height="92" HorizontalAlignment="Right" MinWidth="0" MinHeight="0" CornerRadius="3" Padding="0" BorderThickness="0" HorizontalContentAlignment="Stretch" VerticalContentAlignment="Stretch" Background="Transparent">
   <Grid Width="92" Height="92" UseLayoutRounding="True">
    <FontIcon x:Name="ArtFallback" Glyph="&#xE8D6;" FontFamily="Segoe Fluent Icons" FontSize="24"/>
    <Rectangle x:Name="PlayerAppIcon" Width="92" Height="92" RadiusX="3" RadiusY="3"/>
    <Rectangle x:Name="PlayerArt" Width="92" Height="92" RadiusX="3" RadiusY="3"/>
   </Grid>
  </Button>
 <StackPanel x:Name="TransportControls" Grid.Row="4" Grid.ColumnSpan="2" Orientation="Horizontal" Spacing="12" HorizontalAlignment="Center">
  <Button x:Name="PreviousButton" Style="{StaticResource TransportButton}">
   <FontIcon Glyph="&#xF8AC;" FontFamily="Segoe Fluent Icons" FontSize="16"/>
  </Button>
  <Button x:Name="PlayButton" Style="{StaticResource TransportButton}">
   <Grid Width="16" Height="16">
    <FontIcon x:Name="PlayGlyph" Glyph="&#xF5B0;" FontFamily="Segoe Fluent Icons" FontSize="16"/>
    <FontIcon x:Name="PauseGlyph" Glyph="&#xF8AE;" FontFamily="Segoe Fluent Icons" FontSize="16" Visibility="Collapsed"/>
   </Grid>
  </Button>
  <Button x:Name="NextButton" Style="{StaticResource TransportButton}">
   <FontIcon Glyph="&#xF8AD;" FontFamily="Segoe Fluent Icons" FontSize="16"/>
  </Button>
 </StackPanel>
</Grid>)XAML";
inline void Remove() {
    view = nullptr;
    shownLogo.reset();
    shownArt.reset();
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
    Track(view.ActualThemeChanged(auto_revoke, [](auto const&, auto const&) {
        GuardUi([] {
            if (view)
                Update(svc::Snapshot());
        });
    }));
    auto previous = Element(L"PreviousButton").as<c::Button>();
    auto play = Element(L"PlayButton").as<c::Button>();
    auto next = Element(L"NextButton").as<c::Button>();
    auto cover = Element(L"CoverButton").as<c::Button>();
    LabelControl(previous, Tr(L"Previous track", L"上一首"));
    LabelControl(play, Tr(L"Play or pause", L"播放／暂停"));
    LabelControl(next, Tr(L"Next track", L"下一首"));
    LabelControl(cover, Tr(L"Open source app", L"打开媒体来源应用"));
    Track(previous.Click(auto_revoke,
                         [](auto const&, auto const&) { svc::Media(1); }));
    Track(play.Click(auto_revoke, [](auto const&, auto const&) {
        GuardUi([] {
            auto state = svc::Snapshot();
            bool playing = GetTickCount64() < optimisticUntil
                               ? optimisticPlaying
                               : state.playing;
            optimisticPlaying = !playing;
            optimisticUntil = GetTickCount64() + 900;
            svc::Media(2);
            Update(state);
        });
    }));
    Track(next.Click(auto_revoke,
                     [](auto const&, auto const&) { svc::Media(3); }));
    Track(cover.Click(auto_revoke,
                      [](auto const&, auto const&) { svc::Media(4); }));
    return view;
}
inline void Image(const wchar_t* name,
                  std::shared_ptr<const std::vector<uint8_t>> const& bytes) {
    auto element = Element(name);
    m::Imaging::BitmapImage bitmap{nullptr};
    if (bytes)
        try {
            bitmap = m::Imaging::BitmapImage();
            com_ptr<::IStream> memory;
            memory.attach(
                SHCreateMemStream(bytes->data(), (UINT)bytes->size()));
            if (!memory)
                throw hresult_error(E_OUTOFMEMORY);
            svc::IRandomAccessStream stream{nullptr};
            check_hresult(CreateRandomAccessStreamOverStream(
                memory.get(), BSOS_DEFAULT, guid_of<svc::IRandomAccessStream>(),
                put_abi(stream)));
            bitmap.SetSourceAsync(stream);
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
    auto themed = view.ActualTheme() == x::ElementTheme::Light
                      ? data.mediaLogoLight
                      : data.mediaLogoDark;
    auto logo = themed ? themed : data.mediaLogo;
    if (shownLogo != logo) {
        shownLogo = logo;
        Image(L"HeaderLogo", logo);
        Image(L"PlayerAppIcon", logo);
    }
    if (shownArt != data.art) {
        shownArt = data.art;
        Image(L"PlayerArt", data.art);
    }
    Element(L"HeaderFallback")
        .Visibility(logo ? x::Visibility::Collapsed : x::Visibility::Visible);
    Element(L"ArtFallback")
        .Visibility(logo || data.art ? x::Visibility::Collapsed
                                     : x::Visibility::Visible);
    Element(L"PlayerAppIcon")
        .Visibility(data.art ? x::Visibility::Collapsed
                             : x::Visibility::Visible);
}
}  // namespace mediaCard

namespace cards {
[[clang::no_destroy]] inline c::TextBlock networkName{nullptr},
    networkState{nullptr}, download{nullptr}, upload{nullptr},
    storageName{nullptr}, storageValue{nullptr}, storageDetail{nullptr},
    storageFree{nullptr};
[[clang::no_destroy]] inline c::ProgressBar storageProgress{nullptr};
[[clang::no_destroy]] inline c::TextBlock volumeLabel{nullptr},
    brightnessLabel{nullptr};
[[clang::no_destroy]] inline c::Slider volume{nullptr}, brightness{nullptr};
[[clang::no_destroy]] inline c::Button mute{nullptr};
inline bool updating = false;
inline ULONGLONG volumeTouched = 0, brightnessTouched = 0;
inline void Remove() {
    mediaCard::Remove();
    networkName = nullptr;
    networkState = nullptr;
    download = nullptr;
    upload = nullptr;
    storageName = nullptr;
    storageValue = nullptr;
    storageDetail = nullptr;
    storageFree = nullptr;
    storageProgress = nullptr;
    volumeLabel = nullptr;
    brightnessLabel = nullptr;
    volume = nullptr;
    brightness = nullptr;
    mute = nullptr;
    updating = false;
}
inline c::Border Border(c::Border const& native) {
    static constexpr wchar_t markup[] =
        LR"XAML(
<Border xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 Padding="12,10,12,10" CornerRadius="8" BorderThickness="1"
 Background="{ThemeResource SystemControlBackgroundChromeMediumLowBrush}"
 BorderBrush="{ThemeResource SystemControlForegroundBaseLowBrush}"/>
)XAML";
    auto value = x::Markup::XamlReader::Load(markup).as<c::Border>();
    ApplyNativeSurface(value, native);
    return value;
}
inline c::TextBlock Text(const wchar_t* value, double size = 14) {
    c::TextBlock text;
    text.Text(value);
    text.FontSize(size);
    text.TextTrimming(x::TextTrimming::CharacterEllipsis);
    return text;
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
inline constexpr double kPanelHeight = 176;
inline constexpr double kGap = 4;

static unsigned ModuleCount(svc::Config const& settings) {
    return unsigned(settings.media) + unsigned(settings.network) +
           unsigned(settings.storage) +
           unsigned(settings.quick && (settings.volume || settings.brightness));
}

static bool HasCards(svc::Config const& settings) {
    return ModuleCount(settings) != 0;
}

template <typename Add>
static void Populate(c::Border const& native,
                     svc::Config const& settings,
                     Add&& add) {
    if (settings.media) {
        auto card = Border(native);
        card.Child(mediaCard::Build());
        add(card, 1.7);
    }
    if (settings.quick && (settings.volume || settings.brightness)) {
        auto card = Border(native);
        c::StackPanel content;
        auto heading = Text(Tr(L"Quick settings", L"快速设置"), 14);
        heading.FontWeight(Windows::UI::Text::FontWeights::SemiBold());
        c::Grid header;
        header.ColumnDefinitions().Append(c::ColumnDefinition());
        c::ColumnDefinition buttonColumn;
        buttonColumn.Width({1, x::GridUnitType::Auto});
        header.ColumnDefinitions().Append(buttonColumn);
        header.Children().Append(heading);
        if (settings.volume) {
            mute = c::Button();
            mute.Content(box_value(Tr(L"Mute", L"静音")));
            mute.FontSize(10);
            mute.Padding({6, 1, 6, 1});
            c::Grid::SetColumn(mute, 1);
            Track(mute.Click(auto_revoke, [](auto const&, auto const&) {
                GuardUi([] { svc::Mute(!svc::Snapshot().muted); });
            }));
            header.Children().Append(mute);
        }
        content.Children().Append(header);
        if (settings.volume) {
            volumeLabel = Text(Tr(L"Volume", L"音量"), 11);
            volumeLabel.Margin({0, 6, 0, 0});
            content.Children().Append(volumeLabel);
            volume = c::Slider();
            volume.Minimum(0);
            volume.Maximum(100);
            volume.StepFrequency(1);
            volume.Height(26);
            LabelControl(volume, Tr(L"Volume", L"音量"));
            Track(volume.ValueChanged(
                auto_revoke, [](auto const& sender, auto const&) {
                    GuardUi([&] {
                        if (updating)
                            return;
                        volumeTouched = GetTickCount64();
                        svc::Volume((int)std::round(
                            sender.template as<c::Slider>().Value()));
                    });
                }));
            content.Children().Append(volume);
        }
        if (settings.brightness) {
            brightnessLabel = Text(Tr(L"Brightness", L"亮度"), 11);
            brightnessLabel.Margin({0, 3, 0, 0});
            content.Children().Append(brightnessLabel);
            brightness = c::Slider();
            brightness.Minimum(1);
            brightness.Maximum(100);
            brightness.StepFrequency(1);
            brightness.Height(26);
            LabelControl(brightness, Tr(L"Brightness", L"亮度"));
            Track(brightness.ValueChanged(
                auto_revoke, [](auto const& sender, auto const&) {
                    GuardUi([&] {
                        if (updating)
                            return;
                        brightnessTouched = GetTickCount64();
                        svc::Brightness((int)std::round(
                            sender.template as<c::Slider>().Value()));
                    });
                }));
            content.Children().Append(brightness);
        }
        card.Child(content);
        add(card, 1.0);
    }
    if (settings.network) {
        auto card = Border(native);
        c::StackPanel info;
        info.Children().Append(Text(Tr(L"Network", L"网络"), 14));
        networkName = Text(Tr(L"Reading", L"正在读取"), 13);
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
    if (settings.storage) {
        auto card = Border(native);
        c::StackPanel info;
        storageName = Text(Tr(L"System drive", L"系统盘"), 14);
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
}

inline void Update(svc::Data const& data) {
    mediaCard::Update(data);
    wchar_t text[192];
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
            swprintf(buffer, 64, L"%.1f %ls", bytes, unit);
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
            swprintf(text, 192,
                     Tr(L"Used %.1f / %.1f GB", L"已用 %.1f / %.1f GB"),
                     data.diskUsed, data.diskTotal);
            storageDetail.Text(text);
            swprintf(text, 192, Tr(L"Free %.1f GB", L"可用 %.1f GB"),
                     data.diskFree);
            storageFree.Text(text);
        }
    }
    struct UpdateGuard {
        UpdateGuard() { updating = true; }
        ~UpdateGuard() { updating = false; }
    } guard;
    if (volume) {
        volume.IsEnabled(data.volume >= 0);
        if (data.volume >= 0) {
            if (GetTickCount64() - volumeTouched > 700)
                volume.Value(data.volume);
            swprintf(text, 192, Tr(L"Volume  %.0f%%%ls", L"音量  %.0f%%%ls"),
                     volume.Value(),
                     data.muted ? Tr(L" · Muted", L" · 已静音") : L"");
            volumeLabel.Text(text);
        } else
            volumeLabel.Text(Tr(L"Volume unavailable", L"音量暂不可用"));
    }
    if (brightness) {
        brightness.IsEnabled(data.brightnessReady);
        if (data.brightnessReady) {
            if (GetTickCount64() - brightnessTouched > 700)
                brightness.Value(data.brightness);
            swprintf(text, 192, Tr(L"Brightness  %.0f%%", L"亮度  %.0f%%"),
                     brightness.Value());
            brightnessLabel.Text(text);
        } else
            brightnessLabel.Text(
                Tr(L"Brightness unavailable", L"亮度暂不可用"));
    }
    if (mute) {
        mute.Content(box_value(data.muted ? Tr(L"Unmute", L"取消静音")
                                          : Tr(L"Mute", L"静音")));
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
static x::FrameworkElement OverviewElement(const wchar_t* name) {
    auto element = g_card.FindName(name).try_as<x::FrameworkElement>();
    if (!element)
        throw hresult_error(E_FAIL, L"Missing overview XAML element");
    return element;
}

static void WireOverview() {
    auto button = OverviewElement(L"SettingsButton").as<c::Button>();
    Track(button.Click(auto_revoke, [](auto const&, auto const&) {
        ShellExecuteW(nullptr, L"open", L"ms-settings:about", nullptr, nullptr,
                      SW_SHOWNORMAL);
    }));
}
static void BuildOverview(c::Border const& nativeBorder,
                          svc::Config const& settings,
                          bool wire = true) {
    g_card = x::Markup::XamlReader::Load(kMarkup).as<c::Border>();
    ApplyNativeSurface(g_card, nativeBorder);
    g_clock = OverviewElement(L"ClockValue").as<c::TextBlock>();
    g_date = OverviewElement(L"DateValue").as<c::TextBlock>();
    g_title = OverviewElement(L"DeviceTitle").as<c::TextBlock>();
    g_powerSource = OverviewElement(L"PowerSourceValue").as<c::TextBlock>();
    g_cpu = OverviewElement(L"CpuValue").as<c::TextBlock>();
    g_gpu = OverviewElement(L"GpuValue").as<c::TextBlock>();
    g_battery = OverviewElement(L"BatteryValue").as<c::TextBlock>();
    g_power = OverviewElement(L"PowerValue").as<c::TextBlock>();
    g_memory = OverviewElement(L"MemoryValue").as<c::TextBlock>();
    g_available = OverviewElement(L"AvailableValue").as<c::TextBlock>();
    OverviewElement(L"TimeLabel").as<c::TextBlock>().Text(Tr(L"Time", L"时间"));
    OverviewElement(L"BatteryLabel")
        .as<c::TextBlock>()
        .Text(Tr(L"Battery", L"电池"));
    OverviewElement(L"MemoryLabel")
        .as<c::TextBlock>()
        .Text(Tr(L"Memory", L"内存"));
    OverviewElement(L"TimeTile")
        .Visibility(settings.time ? x::Visibility::Visible
                                  : x::Visibility::Collapsed);
    OverviewElement(L"BatteryTile")
        .Visibility(settings.battery ? x::Visibility::Visible
                                     : x::Visibility::Collapsed);
    OverviewElement(L"MemoryTile")
        .Visibility(settings.memory ? x::Visibility::Visible
                                    : x::Visibility::Collapsed);
    OverviewElement(L"PerformanceTile")
        .Visibility(settings.performance ? x::Visibility::Visible
                                         : x::Visibility::Collapsed);
    auto button = OverviewElement(L"SettingsButton").as<c::Button>();
    button.Content(box_value(Tr(L"System settings", L"系统设置")));
    if (wire)
        WireOverview();
}

static void CompactOverview(double width) {
    std::vector<x::FrameworkElement> metrics;
    for (const wchar_t* name :
         {L"TimeTile", L"BatteryTile", L"MemoryTile", L"PerformanceTile"}) {
        auto element = OverviewElement(name);
        if (element.Visibility() == x::Visibility::Visible)
            metrics.push_back(element);
    }
    auto title = OverviewElement(L"DeviceTitle").as<c::TextBlock>();
    auto settings = OverviewElement(L"SettingsButton").as<c::Button>();
    auto detach = [](x::FrameworkElement const& element) {
        if (auto panel = element.Parent().try_as<c::Panel>()) {
            uint32_t index;
            if (panel.Children().IndexOf(element, index))
                panel.Children().RemoveAt(index);
        }
    };
    detach(title);
    detach(settings);
    for (auto const& metric : metrics)
        detach(metric);
    g_card.Child(nullptr);

    c::StackPanel content;
    content.Spacing(8);
    c::Grid header;
    header.ColumnSpacing(8);
    header.ColumnDefinitions().Append(c::ColumnDefinition());
    c::ColumnDefinition settingsColumn;
    settingsColumn.Width({1, x::GridUnitType::Auto});
    header.ColumnDefinitions().Append(settingsColumn);
    title.Margin({0, 0, 0, 0});
    title.VerticalAlignment(x::VerticalAlignment::Center);
    settings.Margin({0, 0, 0, 0});
    c::Grid::SetRow(settings, 0);
    c::Grid::SetColumn(settings, 1);
    header.Children().Append(title);
    if (width >= 330)
        header.Children().Append(settings);
    else
        c::Grid::SetColumn(settings, 0);
    content.Children().Append(header);

    c::Grid values;
    values.ColumnSpacing(6);
    values.RowSpacing(6);
    const unsigned columns = width >= 650 ? 4 : width >= 330 ? 2 : 1;
    for (unsigned i = 0; i < columns; ++i)
        values.ColumnDefinitions().Append(c::ColumnDefinition());
    for (unsigned i = 0; i < metrics.size(); ++i) {
        if (i % columns == 0) {
            c::RowDefinition row;
            row.Height({1, x::GridUnitType::Auto});
            values.RowDefinitions().Append(row);
        }
        metrics[i].Margin({0, 0, 0, 0});
        c::Grid::SetColumn(metrics[i], i % columns);
        c::Grid::SetRow(metrics[i], i / columns);
        values.Children().Append(metrics[i]);
    }
    content.Children().Append(values);
    if (width < 330)
        content.Children().Append(settings);
    g_card.ClearValue(x::FrameworkElement::WidthProperty());
    g_card.MinHeight(0);
    g_card.Padding({12, 12, 12, 12});
    g_card.HorizontalAlignment(x::HorizontalAlignment::Stretch);
    g_card.Child(content);
}

static c::ScrollViewer BuildStackedCards(c::Border const& native,
                                         svc::Config const& settings,
                                         double width,
                                         bool shadow = true) {
    c::StackPanel stack;
    stack.Spacing(6);
    stack.Margin({0, 0, 6, 0});
    if (settings.overview) {
        BuildOverview(native, settings);
        CompactOverview(width);
        stack.Children().Append(WrapSurface(g_card, shadow, false));
    }
    std::vector<c::Border> smallerCards;
    cards::Populate(
        native, settings, [&](c::Border const& card, double weight) {
            if (weight > 1.0) {
                card.Height(cards::kPanelHeight);
                stack.Children().Append(WrapSurface(card, shadow, false));
            } else {
                card.MinHeight(142);
                smallerCards.push_back(card);
            }
        });
    if (!smallerCards.empty()) {
        c::Grid grid;
        grid.ColumnSpacing(6);
        grid.RowSpacing(6);
        const unsigned columns =
            std::min<unsigned>(smallerCards.size(), width >= 590   ? 3
                                                    : width >= 370 ? 2
                                                                   : 1);
        for (unsigned i = 0; i < columns; ++i)
            grid.ColumnDefinitions().Append(c::ColumnDefinition());
        for (unsigned i = 0; i < smallerCards.size(); ++i) {
            if (i % columns == 0) {
                c::RowDefinition row;
                row.Height({1, x::GridUnitType::Auto});
                grid.RowDefinitions().Append(row);
            }
            auto surface = WrapSurface(smallerCards[i], shadow, false);
            c::Grid::SetColumn(surface, i % columns);
            c::Grid::SetRow(surface, i / columns);
            if (i + 1 == smallerCards.size())
                c::Grid::SetColumnSpan(surface, columns - i % columns);
            grid.Children().Append(surface);
        }
        stack.Children().Append(grid);
    }
    c::ScrollViewer scroll;
    scroll.HorizontalScrollMode(c::ScrollMode::Disabled);
    scroll.HorizontalScrollBarVisibility(c::ScrollBarVisibility::Disabled);
    scroll.VerticalScrollBarVisibility(c::ScrollBarVisibility::Auto);
    scroll.Content(stack);
    return scroll;
}

static void ShowCompactPanel(bool show, bool focus) {
    if (!g_compactPanel || !g_compactButton)
        return;
    g_compactPanel.Visibility(show ? x::Visibility::Visible
                                   : x::Visibility::Collapsed);
    g_compactButton.Visibility(show ? x::Visibility::Collapsed
                                    : x::Visibility::Visible);
    if (focus)
        (show ? g_compactBack : g_compactButton)
            .Focus(x::FocusState::Programmatic);
}

static void ResizeCompactLayer(c::Grid const& main, WorkArea work) {
    // The added layer must never take part in expanding Start's native frame.
    g_compactLayer.Width(std::min(main.ActualWidth(), work.width));
    g_compactLayer.Height(std::min(main.ActualHeight(), work.height));
}

static void BuildCompact(c::Grid const& main,
                         c::Border const& native,
                         svc::Config const& settings,
                         WorkArea work) {
    g_compactLayer = c::Grid();
    g_compactLayer.Name(L"StartSidecarCompactLayer");
    g_compactLayer.HorizontalAlignment(x::HorizontalAlignment::Center);
    g_compactLayer.VerticalAlignment(x::VerticalAlignment::Center);
    c::Grid::SetColumnSpan(g_compactLayer,
                           std::max(1u, main.ColumnDefinitions().Size()));
    c::Grid::SetRowSpan(g_compactLayer,
                        std::max(1u, main.RowDefinitions().Size()));
    c::Canvas::SetZIndex(g_compactLayer, 100);
    ResizeCompactLayer(main, work);

    g_compactPanel = cards::Border(native);
    g_compactPanel.Name(L"StartSidecarCompactPanel");
    g_compactPanel.Margin({6, 6, 6, 6});
    g_compactPanel.Padding({12, 10, 12, 10});
    g_compactPanel.TabFocusNavigation(x::Input::KeyboardNavigationMode::Cycle);
    c::Grid layout;
    c::RowDefinition top;
    top.Height({1, x::GridUnitType::Auto});
    layout.RowDefinitions().Append(top);
    layout.RowDefinitions().Append(c::RowDefinition());
    c::Grid header;
    header.Margin({0, 0, 0, 8});
    header.ColumnDefinitions().Append(c::ColumnDefinition());
    c::ColumnDefinition backColumn;
    backColumn.Width({1, x::GridUnitType::Auto});
    header.ColumnDefinitions().Append(backColumn);
    auto title = cards::Text(Tr(L"Start cards", L"开始菜单卡片"), 18);
    title.FontWeight(Windows::UI::Text::FontWeights::SemiBold());
    title.VerticalAlignment(x::VerticalAlignment::Center);
    header.Children().Append(title);
    g_compactBack = c::Button();
    g_compactBack.Content(box_value(Tr(L"Back to Start", L"返回开始菜单")));
    c::Grid::SetColumn(g_compactBack, 1);
    header.Children().Append(g_compactBack);
    layout.Children().Append(header);

    auto scroll = BuildStackedCards(
        native, settings, std::max(100.0, g_compactLayer.Width() - 40), false);
    c::Grid::SetRow(scroll, 1);
    layout.Children().Append(scroll);
    g_compactPanel.Child(layout);
    g_compactLayer.Children().Append(g_compactPanel);

    g_compactButton = c::Button();
    g_compactButton.Name(L"StartSidecarCompactButton");
    g_compactButton.Content(box_value(Tr(L"Cards", L"卡片")));
    g_compactButton.HorizontalAlignment(x::HorizontalAlignment::Center);
    g_compactButton.VerticalAlignment(x::VerticalAlignment::Bottom);
    g_compactButton.Margin({0, 0, 78, 12});
    g_compactLayer.Children().Append(g_compactButton);
    Track(g_compactButton.Click(auto_revoke, [](auto const&, auto const&) {
        GuardUi([] { ShowCompactPanel(true, true); });
    }));
    Track(g_compactBack.Click(auto_revoke, [](auto const&, auto const&) {
        GuardUi([] { ShowCompactPanel(false, true); });
    }));
    Track(
        g_compactPanel.KeyDown(auto_revoke, [](auto const&, auto const& args) {
            if (args.Key() == Windows::System::VirtualKey::Escape) {
                args.Handled(true);
                GuardUi([] { ShowCompactPanel(false, true); });
            }
        }));
    Track(
        main.SizeChanged(auto_revoke, [work](auto const& sender, auto const&) {
            GuardUi([&] {
                ResizeCompactLayer(sender.template as<c::Grid>(), work);
            });
        }));
    ShowCompactPanel(false, false);
    main.Children().Append(g_compactLayer);
}

static f::Point LayoutOrigin(x::DependencyObject element,
                             x::DependencyObject const& root) {
    f::Point origin{};
    for (unsigned depth = 0; element && element != root && depth < 64;
         ++depth) {
        if (auto ui = element.try_as<x::UIElement>()) {
            auto offset = ui.ActualOffset();
            origin.X += offset.x;
            origin.Y += offset.y;
        }
        element = m::VisualTreeHelper::GetParent(element);
    }
    return origin;
}
static NativeGeometry ReadNativeGeometry(x::DependencyObject const& root,
                                         c::Grid const& host,
                                         c::Grid const& main,
                                         x::FrameworkElement const& phone) {
    auto origin = LayoutOrigin(main, root);
    auto hostOrigin = LayoutOrigin(host, root);
    NativeGeometry geometry{
        {origin.X, origin.Y, main.ActualWidth(), main.ActualHeight()},
        {},
        hostOrigin.X,
        hostOrigin.Y,
        false};
    geometry.occupied = geometry.menu;
    if (phone && cards::Visible(phone) && phone.ActualWidth() > 1 &&
        phone.ActualHeight() > 1) {
        auto at = LayoutOrigin(phone, root);
        geometry.occupied = packing::Union(
            geometry.menu,
            {at.X, at.Y, phone.ActualWidth(), phone.ActualHeight()});
        geometry.phoneVisible = true;
    }
    return geometry;
}

static void SaveNative(x::FrameworkElement const& element,
                       x::DependencyProperty const& property) {
    for (auto const& saved : *g_nativeProperties)
        if (saved.element == element && saved.property == property)
            return;
    NativeProperty saved{element, property, element.ReadLocalValue(property)};
    if (auto expression = element.GetBindingExpression(property))
        saved.binding = expression.ParentBinding();
    g_nativeProperties->push_back(std::move(saved));
}
static void RestoreNativeBounds() {
    GuardUi([] {
        if (g_host && g_nativeColumns)
            for (auto const& column : *g_nativeColumns) {
                uint32_t index;
                if (g_host.ColumnDefinitions().IndexOf(column, index))
                    g_host.ColumnDefinitions().RemoveAt(index);
            }
        if (g_host && g_nativeRows)
            for (auto const& row : *g_nativeRows) {
                uint32_t index;
                if (g_host.RowDefinitions().IndexOf(row, index))
                    g_host.RowDefinitions().RemoveAt(index);
            }
    });
    if (g_nativeProperties)
        for (auto it = g_nativeProperties->rbegin();
             it != g_nativeProperties->rend(); ++it)
            GuardUi([&] {
                if (it->binding)
                    x::Data::BindingOperations::SetBinding(
                        it->element, it->property, it->binding);
                else if (it->value == x::DependencyProperty::UnsetValue())
                    it->element.ClearValue(it->property);
                else
                    it->element.SetValue(it->property, it->value);
            });
    g_nativeProperties.reset();
    g_nativeColumns.reset();
    g_nativeRows.reset();
}
static void ExpandHostBounds(x::DependencyObject const& root,
                             c::Grid const& host,
                             c::Grid const& main,
                             x::FrameworkElement const& phone,
                             std::vector<packing::Rect> const& cards) {
    auto frame = Resolve(root, L"FrameRoot");
    if (!frame)
        throw hresult_error(E_FAIL, L"Native frame is unavailable");
    g_host = host;
    g_nativeProperties.emplace();
    g_nativeColumns.emplace();
    g_nativeRows.emplace();
    const auto original = g_nativeGeometry;
    const double oldWidth = host.ActualWidth(), oldHeight = host.ActualHeight();
    const auto frameOffset = frame.ActualOffset();
    const double paddingWidth = std::max(0.0, frame.ActualWidth() - oldWidth);
    const double paddingHeight =
        std::max(0.0, frame.ActualHeight() - oldHeight);
    packing::Rect hull{original.hostX, original.hostY, oldWidth, oldHeight};
    for (auto const& rect : cards)
        hull = packing::Union(hull, rect);
    const double left = std::max(0.0, original.hostX - hull.x);
    const double top = std::max(0.0, original.hostY - hull.y);
    const double right =
        std::max(0.0, hull.Right() - original.hostX - oldWidth);
    const double bottom =
        std::max(0.0, hull.Bottom() - original.hostY - oldHeight);
    std::vector<x::FrameworkElement> nativeChildren;
    for (auto child : host.Children())
        if (auto element = child.try_as<x::FrameworkElement>())
            nativeChildren.push_back(element);
    auto column = [&](double size, bool first) {
        c::ColumnDefinition added;
        added.Width({size, x::GridUnitType::Pixel});
        if (first)
            host.ColumnDefinitions().InsertAt(0, added);
        else
            host.ColumnDefinitions().Append(added);
        g_nativeColumns->push_back(added);
    };
    auto row = [&](double size, bool first) {
        c::RowDefinition added;
        added.Height({size, x::GridUnitType::Pixel});
        if (first)
            host.RowDefinitions().InsertAt(0, added);
        else
            host.RowDefinitions().Append(added);
        g_nativeRows->push_back(added);
    };
    if (host.ColumnDefinitions().Size() == 0)
        column(oldWidth, false);
    if (host.RowDefinitions().Size() == 0)
        row(oldHeight, false);
    if (left > 0)
        column(left, true);
    if (right > 0)
        column(right, false);
    if (top > 0)
        row(top, true);
    if (bottom > 0)
        row(bottom, false);
    for (auto const& child : nativeChildren) {
        if (left > 0) {
            SaveNative(child, c::Grid::ColumnProperty());
            c::Grid::SetColumn(child, c::Grid::GetColumn(child) + 1);
        }
        if (top > 0) {
            SaveNative(child, c::Grid::RowProperty());
            c::Grid::SetRow(child, c::Grid::GetRow(child) + 1);
        }
    }
    for (auto const& element : {main.as<x::FrameworkElement>(), phone})
        if (element) {
            SaveNative(element, x::FrameworkElement::HeightProperty());
            element.Height(element.ActualHeight());
        }
    const double width = oldWidth + left + right;
    const double height = oldHeight + top + bottom;
    for (auto const& element : {host.as<x::FrameworkElement>(), frame}) {
        SaveNative(element, x::FrameworkElement::WidthProperty());
        SaveNative(element, x::FrameworkElement::HeightProperty());
        SaveNative(element, x::FrameworkElement::MaxWidthProperty());
        SaveNative(element, x::FrameworkElement::MaxHeightProperty());
        element.MaxWidth(INFINITY);
        element.MaxHeight(INFINITY);
    }
    host.Width(width);
    host.Height(height);
    frame.Width(width + paddingWidth);
    frame.Height(height + paddingHeight);
    SaveNative(frame, x::FrameworkElement::MarginProperty());
    SaveNative(frame, x::FrameworkElement::HorizontalAlignmentProperty());
    SaveNative(frame, x::FrameworkElement::VerticalAlignmentProperty());
    frame.HorizontalAlignment(x::HorizontalAlignment::Left);
    frame.VerticalAlignment(x::VerticalAlignment::Top);
    // Anchor the native menu; growth goes into the free regions, rather than
    // letting the larger frame recenter the native menu or activate a clip.
    frame.Margin({frameOffset.x - left, frameOffset.y - top, 0, 0});
    for (const wchar_t* name :
         {L"MaxHeightEnforcer", L"DropShadowDismissTarget"})
        if (auto enforcer = Resolve(root, name)) {
            SaveNative(enforcer, x::FrameworkElement::HeightProperty());
            SaveNative(enforcer, c::Grid::RowProperty());
            SaveNative(enforcer, c::Grid::RowSpanProperty());
            enforcer.Height(height + paddingHeight);
            c::Grid::SetRow(enforcer, 0);
            c::Grid::SetRowSpan(enforcer, host.RowDefinitions().Size());
        }
    if (auto element = root.try_as<x::FrameworkElement>())
        element.UpdateLayout();
    auto after = ReadNativeGeometry(root, host, main, phone);
    const double dx = original.menu.x - after.menu.x;
    const double dy = original.menu.y - after.menu.y;
    if (std::abs(dx) > 0.01 || std::abs(dy) > 0.01) {
        auto margin = frame.Margin();
        margin.Left += dx;
        margin.Top += dy;
        frame.Margin(margin);
        root.as<x::FrameworkElement>().UpdateLayout();
    }
    g_nativeGeometry = ReadNativeGeometry(root, host, main, phone);
}

static void CreateCardsLayer(c::Grid const& host) {
    g_host = host;
    g_cardsLayer = c::Canvas();
    g_cardsLayer.Name(L"StartSidecarCardsLayer");
    c::Grid::SetColumnSpan(g_cardsLayer,
                           std::max(1u, host.ColumnDefinitions().Size()));
    c::Grid::SetRowSpan(g_cardsLayer,
                        std::max(1u, host.RowDefinitions().Size()));
    // The host bounds include every card, while the native menu's own position
    // and size stay anchored. All host adjustments are restored on teardown.
    host.Children().Append(g_cardsLayer);
}
static void PlaceInCardsLayer(x::FrameworkElement const& element,
                              packing::Rect const& rect) {
    element.Width(rect.w);
    element.Height(rect.h);
    c::Canvas::SetLeft(element, rect.x - g_nativeGeometry.hostX);
    c::Canvas::SetTop(element, rect.y - g_nativeGeometry.hostY);
    g_cardsLayer.Children().Append(element);
}

static void RefreshLayout(x::UIElement const& root, WorkArea work) {
    try {
        auto main = Resolve(root, L"MainMenu").try_as<c::Grid>();
        auto host = Resolve(root, L"AnimationRoot").try_as<c::Grid>();
        auto phone = Resolve(root, L"RightCompanion");
        if (!main || !host || main.ActualWidth() < 100 ||
            main.ActualHeight() < 100)
            return;
        auto geometry = ReadNativeGeometry(root, host, main, phone);
        if (g_layoutReady && g_layoutRoot == root && g_workArea == work &&
            g_nativeGeometry == geometry && (!g_card || g_card.Parent()) &&
            (!g_cardsLayer || g_cardsLayer.Parent()) &&
            (!g_compactLayer || g_compactLayer.Parent())) {
            UpdateData();
            if (g_core && g_core.Visible() && g_update)
                g_update.Start();
            return;
        }
        RemoveCard();
        root.as<x::FrameworkElement>().UpdateLayout();
        geometry = ReadNativeGeometry(root, host, main, phone);
        g_layoutRoot = root;
        g_workArea = work;
        g_nativeGeometry = geometry;
        g_cardEvents.emplace();
        auto native =
            Resolve(root, L"MainMenu/AcrylicBorder").try_as<c::Border>();
        if (!native)
            return;
        for (const wchar_t* name :
             {L"StartDropShadow", L"RootGridDropShadow", L"DropShadow"})
            if (auto shadow = Find(host, name).try_as<c::Border>()) {
                g_shadowSource = shadow;
                break;
            }
        const auto settings = std::atomic_load(&svc::config);
        double gap = settings->cardGap;
        if (gap < 0) {
            gap =
                phone ? std::max(phone.Margin().Left, phone.Margin().Right) : 0;
            if (gap <= 0)
                gap = 4;
        }
        gap = std::clamp(gap, 0.0, 24.0);
        // Keep shadow falloff inside the popup's work area, on every side.
        const packing::Rect bounds{work.left + 12, work.top + 12,
                                   std::max(0.0, work.width - 48),
                                   std::max(0.0, work.height - 48)};
        auto free = packing::Free(bounds, geometry.occupied, gap);
        packing::Rect overflow;
        for (auto const& r : free)
            if (r.w >= 230 && r.h >= 164 && r.w * r.h > overflow.w * overflow.h)
                overflow = r;

        auto watch = [&] {
            Track(host.LayoutUpdated(
                auto_revoke,
                [root, host, main, phone](auto const&, auto const&) {
                    if (!g_layoutReady || !svc::visible)
                        return;
                    GuardUi([&] {
                        if (ReadNativeGeometry(root, host, main, phone) !=
                            g_nativeGeometry) {
                            g_layoutReady = false;
                            QueueLayout();
                        }
                    });
                }));
        };
        if ((!settings->overview && !cards::HasCards(*settings))) {
            g_layoutReady = true;
            watch();
            return;
        }
        if (overflow.w == 0) {
            BuildCompact(main, native, *settings, work);
        } else {
            std::vector<packing::Spec> specs;
            if (settings->overview) {
                BuildOverview(native, *settings, false);
                packing::Spec overview;
                overview.overview = true;
                overview.sizes.push_back(
                    {static_cast<double>(settings->overviewWidth),
                     std::max(500.0, geometry.menu.h), false});
                std::vector<double> widths{
                    static_cast<double>(settings->overviewWidth), 330, 650,
                    geometry.menu.w, bounds.w};
                for (auto const& space : free) {
                    const double width =
                        std::min<double>(settings->overviewWidth, space.w);
                    if (width >=
                            std::min(230.0, double(settings->overviewWidth)) &&
                        (space.Right() <= geometry.occupied.x ||
                         space.x >= geometry.occupied.Right()))
                        overview.sizes.push_back(
                            {width, std::max(500.0, geometry.menu.h), false});
                    if (space.w >= 230)
                        widths.push_back(space.w);
                }
                std::sort(widths.begin(), widths.end());
                widths.erase(std::unique(widths.begin(), widths.end()),
                             widths.end());
                for (double width : widths) {
                    if (width < 230 || width > bounds.w)
                        continue;
                    CompactOverview(width);
                    g_card.Width(width);
                    g_card.Measure({static_cast<float>(width), INFINITY});
                    overview.sizes.push_back(
                        {width, std::ceil(g_card.DesiredSize().Height), true});
                }
                specs.push_back(std::move(overview));
            }
            std::vector<std::pair<c::Border, double>> modules;
            cards::Populate(native, *settings,
                            [&](auto const& card, double weight) {
                                modules.emplace_back(card, weight);
                                const double ideal = weight > 1     ? 420
                                                     : weight == 1  ? 200
                                                     : weight > 0.7 ? 180
                                                                    : 150;
                                const double minimum = weight > 1     ? 240
                                                       : weight == 1  ? 180
                                                       : weight > 0.7 ? 170
                                                                      : 150;
                                specs.push_back({{}, ideal, minimum, false});
                            });
            auto plan = packing::Solve(bounds, geometry.occupied, specs, gap);
            std::vector<packing::Rect> extents;
            if (plan.size() == specs.size())
                for (auto const& item : plan)
                    extents.push_back(item.rect);
            else
                extents.push_back(overflow);
            ExpandHostBounds(root, host, main, phone, extents);
            CreateCardsLayer(host);
            if (plan.size() == specs.size()) {
                if (settings->overview) {
                    if (plan.front().compact) {
                        CompactOverview(plan.front().rect.w);
                        WireOverview();
                    } else {
                        BuildOverview(native, *settings);
                    }
                }
                g_packedCards.emplace();
                for (size_t i = 0; i < plan.size(); ++i) {
                    const bool overview = settings->overview && i == 0;
                    auto card =
                        overview
                            ? g_card
                            : modules[i - unsigned(settings->overview)].first;
                    auto const& rect = plan[i].rect;
                    if (!overview) {
                        auto padding = card.Padding();
                        padding.Top = padding.Bottom =
                            std::min(10.0, std::max(0.0, (rect.h - 152) / 2));
                        card.Padding(padding);
                    }
                    card.Width(rect.w);
                    card.MaxWidth(rect.w);
                    card.Height(rect.h);
                    if (auto child =
                            card.Child().try_as<x::FrameworkElement>()) {
                        auto padding = card.Padding(),
                             border = card.BorderThickness();
                        child.MaxWidth(std::max(
                            1.0, rect.w - padding.Left - padding.Right -
                                     border.Left - border.Right));
                    }
                    PlaceInCardsLayer(WrapSurface(card), rect);
                    g_packedCards->push_back({card, rect, overview});
                }
            } else {
                // A scrollable section is the fallback only after individual
                // cards could not be packed into the actual surrounding gaps.
                for (auto const& unsubscribe : *g_cardEvents)
                    GuardUi(unsubscribe);
                g_cardEvents->clear();
                cards::Remove();
                modules.clear();
                g_card = nullptr;
                auto scroll = BuildStackedCards(native, *settings, overflow.w);
                scroll.Name(L"StartSidecarOverflow");
                PlaceInCardsLayer(scroll, overflow);
            }
        }
        g_layoutReady = true;
        watch();
        UpdateData();
        if (g_update)
            g_update.Start();
    } catch (hresult_error const& error) {
        RemoveCard();
        Wh_Log(L"Start Sidecar layout error %#x: %s", (unsigned)error.code(),
               error.message().c_str());
    } catch (...) {
        RemoveCard();
        Wh_Log(L"Unexpected Start Sidecar layout error");
    }
}

static void EnsureLayout() {
    if (g_unloading || !svc::visible)
        return;
    if (auto window = x::Window::Current(); window && window.Content())
        RefreshLayout(window.Content(), GetWorkArea());
}
static void QueueLayout() {
    if (g_unloading || !svc::visible || !g_layoutTimer)
        return;
    g_layoutAttempts = 30;
    g_layoutTimer.Start();
}
static void ReleaseUi() {
    g_uiReady = false;
    svc::SetVisible(false);
    if (g_layoutTimer) {
        GuardUi([] { g_layoutTimer.Stop(); });
        GuardUi([] { g_layoutTimer.Tick(g_layoutTick); });
        g_layoutTimer = nullptr;
    }
    RemoveCard();
    if (g_update) {
        GuardUi([] { g_update.Tick(g_updateTick); });
        g_update = nullptr;
    }
    if (g_core) {
        GuardUi([] { g_core.VisibilityChanged(g_visible); });
        GuardUi([] { g_core.Activated(g_activated); });
        GuardUi([] { g_core.Closed(g_closed); });
        GuardUi([] { g_core.SizeChanged(g_sizeChanged); });
        g_core = nullptr;
    }
    g_visible = g_activated = g_closed = g_sizeChanged = g_updateTick =
        g_layoutTick = {};
    g_hwnd = nullptr;
}

static bool InitializeUi() {
    if (g_unloading)
        return true;
    try {
        auto window = x::Window::Current();
        auto cw = core::CoreWindow::GetForCurrentThread();
        if (!window || !window.Content() || !cw)
            return false;
        if (!g_uiReady) {
            g_core = cw;
            svc::SetVisible(cw.Visible());
            g_layoutTimer = x::DispatcherTimer();
            g_layoutTimer.Interval(std::chrono::milliseconds(75));
            g_layoutTick = g_layoutTimer.Tick([](auto const&, auto const&) {
                GuardUi([] {
                    g_layoutTimer.Stop();
                    if (g_unloading || !svc::visible)
                        return;
                    EnsureLayout();
                    if (!g_layoutReady && --g_layoutAttempts > 0)
                        g_layoutTimer.Start();
                });
            });
            g_update = x::DispatcherTimer();
            g_update.Interval(std::chrono::milliseconds(200));
            g_updateTick = g_update.Tick([](auto const&, auto const&) {
                if (!g_unloading && svc::visible)
                    GuardUi([] {
                        // This is only a cheap monitor/DPI check. The XAML tree
                        // is resolved again only when its available space
                        // changes.
                        if (GetWorkArea() != g_workArea)
                            QueueLayout();
                        else
                            UpdateData();
                    });
            });
            g_visible = cw.VisibilityChanged([](auto const&, auto const& args) {
                GuardUi([&] {
                    if (g_unloading)
                        return;
                    svc::SetVisible(args.Visible());
                    g_shadowKeepUntil = GetTickCount64() + 1000;
                    EnsureShadowAnimation();
                    if (args.Visible()) {
                        QueueLayout();
                    } else {
                        g_layoutTimer.Stop();
                        g_update.Stop();
                        ShowCompactPanel(false, false);
                    }
                });
            });
            g_activated = cw.Activated([](auto const&, auto const& args) {
                if (!g_unloading &&
                    args.WindowActivationState() !=
                        core::CoreWindowActivationState::Deactivated)
                    GuardUi(QueueLayout);
            });
            g_closed = cw.Closed([](auto const&, auto const&) {
                GuardUi(ReleaseUi);
                SetEvent(g_startupWake);
            });
            g_sizeChanged = cw.SizeChanged([](auto const&, auto const&) {
                if (!g_unloading && GetWorkArea() != g_workArea)
                    GuardUi(QueueLayout);
            });
            g_uiReady = true;
        }
        QueueLayout();
        return true;
    } catch (...) {
        ReleaseUi();
        return false;
    }
}
static void StartOnUi(HWND hwnd) {
    if (g_uiReady && g_hwnd != hwnd)
        return;
    g_hwnd = hwnd;
    InitializeUi();
    SetEvent(g_startupWake);
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
struct UiCallPacket {
    HWND window;
    UiCall function;
    void* param;
};
static std::mutex g_uiCallMutex;
static std::atomic<UiCallPacket*> g_pendingUiCall;
static bool OnUi(HWND hwnd, UiCall function, void* param) {
    DWORD thread = GetWindowThreadProcessId(hwnd, nullptr);
    if (!thread)
        return false;
    if (thread == GetCurrentThreadId()) {
        function(param);
        return true;
    }
    std::lock_guard lock(g_uiCallMutex);
    const UINT message = RegisterWindowMessageW(kCallMessage);
    if (!message)
        return false;
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                auto message = reinterpret_cast<const CWPSTRUCT*>(lp);
                auto packet = g_pendingUiCall.load();
                if (packet &&
                    message->lParam == reinterpret_cast<LPARAM>(packet) &&
                    message->hwnd == packet->window &&
                    message->message == RegisterWindowMessageW(kCallMessage)) {
                    g_pendingUiCall = nullptr;
                    GuardUi([&] { packet->function(packet->param); });
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        },
        nullptr, thread);
    if (!hook)
        return false;
    UiCallPacket packet{hwnd, function, param};
    g_pendingUiCall = &packet;
    SendMessageW(hwnd, message, 0, reinterpret_cast<LPARAM>(&packet));
    bool called = g_pendingUiCall.exchange(nullptr) == nullptr;
    UnhookWindowsHookEx(hook);
    return called;
}
BOOL Wh_ModInit() {
    g_unloading = false;
    LoadSettings();
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop)
        return FALSE;
    g_startupWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_startupWake || !svc::InitializeEvents(g_stop)) {
        svc::Stop();
        if (g_startupWake)
            CloseHandle(g_startupWake);
        CloseHandle(g_stop);
        g_startupWake = g_stop = nullptr;
        return FALSE;
    }
    HMODULE user = GetModuleHandleW(L"user32.dll");
    if (auto address = reinterpret_cast<Create_t>(
            GetProcAddress(user, "CreateWindowInBand")))
        WindhawkUtils::SetFunctionHook(address, HookCreate, &g_create);
    if (auto address = reinterpret_cast<CreateEx_t>(
            GetProcAddress(user, "CreateWindowInBandEx")))
        WindhawkUtils::SetFunctionHook(address, HookCreateEx, &g_createEx);
    return TRUE;
}
void Wh_ModAfterInit() {
    try {
        svc::Start();
    } catch (...) {
        Wh_Log(L"Could not start all Start Sidecar workers");
    }
    g_startup = CreateThread(
        nullptr, 0,
        [](void*) -> DWORD {
            while (!g_unloading) {
                if (!g_uiReady) {
                    if (HWND hwnd = FindWindowForStart()) {
                        OnUi(
                            hwnd,
                            [](void* arg) {
                                g_hwnd = static_cast<HWND>(arg);
                                InitializeUi();
                            },
                            hwnd);
                    }
                }
                HANDLE events[] = {g_stop, g_startupWake};
                if (WaitForMultipleObjects(2, events, FALSE,
                                           g_uiReady ? INFINITE : 250) ==
                    WAIT_OBJECT_0)
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
    if (auto hwnd = g_hwnd.load(); hwnd && IsWindow(hwnd))
        OnUi(hwnd, [](void*) { ReleaseUi(); }, nullptr);
    svc::Stop();
    CloseHandle(g_startupWake);
    CloseHandle(g_stop);
    g_startupWake = g_stop = nullptr;
}
void Wh_ModSettingsChanged() {
    LoadSettings();
    if (auto hwnd = g_hwnd.load(); hwnd && IsWindow(hwnd))
        OnUi(
            hwnd,
            [](void*) {
                g_layoutReady = false;
                QueueLayout();
            },
            nullptr);
}
