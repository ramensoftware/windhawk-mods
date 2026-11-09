// ==WindhawkMod==
// @id              gradual-night-light
// @name            Gradual Night Light
// @description     Gradually adjust Windows Night light strength using a daily schedule.
// @version         0.4.1
// @author          Fenryr
// @github          https://github.com/soldier93HUN
// @include         windhawk.exe
// @compilerOptions -fms-extensions -ladvapi32 -lgdi32 -lruntimeobject -lshell32 -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Gradual Night Light

Automatically adjust Windows' native Night light throughout the evening, with
an editable daily schedule and optional gradual transitions.

## Default schedule

| Setting | Time | Strength |
| --- | --- | --- |
| Stage 1 | 19:00 | 50 |
| Stage 2 | 20:00 | 60 |
| Stage 3 | 21:00 | 70 |
| Off time | 07:00 | Off |

## Settings

- **Off time**: the local time when Night light turns off each day.
- **Stages**: add or remove stages with Windhawk's list controls. Each stage has
  a time and a strength from 1 (weakest) to 100 (warmest). Three stages are
  provided by default; between 1 and 96 stages are supported.
- **Gradual transitions**: interpolate strength between stages. For example,
  the default schedule produces strength 55 at 19:30. Disable this option to
  change strength only at each stage's start time.

Times use the local 24-hour `HH:MM` format. Stages are sorted automatically and
the schedule repeats daily, including across midnight. All stage times must be
unique and different from Off time. Invalid settings are reported in the mod
log; an invalid edit keeps the last valid configuration. If saved settings are
invalid at startup, the tool stays running without applying a schedule until
valid settings are saved.

Night light switches off at Off time and stays off until the next stage. The
last stage holds its strength until Off time. To fade in from almost no warmth,
add a stage such as 18:30 with strength 1 before 19:00 with strength 50. Add
further stages to create a five-stage schedule or a gradual morning fade.

## Behavior and restoration

The scheduler runs in a dedicated Windhawk tool process. Every ten seconds it
applies the scheduled temperature directly through Windows' display pipeline.
It also checks after display changes, resume, and session unlock.

Settings and Quick Settings show Windows' own Night light configuration.
Windows continues running its own schedule in the background. Its transitions
can briefly take precedence until the next mod check. The mod pauses while
Windows previews its strength slider. Disabling the mod reapplies the
temperature requested by Windows' current settings.

The mod reads the current user's Night light settings and state from CloudStore.
It applies all scheduled changes at runtime without writing either record. If
Windhawk is terminated during a stage, the runtime tint may last until Windows
next applies Night light or the user signs out.

If you tested an earlier prerelease, disable it before upgrading so that version
can restore the settings it saved.

## Requirements and limitations

The mod uses internal Windows Night light display interfaces and the modern
CloudStore `Current` layout. The display driver must support Night light.
The tool launcher supports Windhawk 1.7.3 and newer. The runtime interfaces
were tested on Windows 11 build 26100, x64, with two displays. HDR and ARM64
have not been verified. If records are missing, open
**Settings > System > Display > Night light** once.

## Technical references

The parser and scheduler are independently implemented using the documented
research below. No external helper, script, or runtime download is required.

- [Night light registry format research](https://github.com/kvnxiao/win-nightlight-cli/blob/main/docs/nightlight-registry-format.md)
- [Microsoft Bond CompactBinary protocol](https://github.com/microsoft/bond/blob/master/cpp/inc/bond/protocol/compact_binary.h)
- [Official Windhawk tool mod launcher](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- OffTime: "07:00"
  $name: Off time (HH:MM)
  $description: Turn Night light off at this local time each day, until the next stage.
- Stages:
  - - Time: "19:00"
      $name: Time (HH:MM)
    - Strength: 50
      $name: Strength (1-100)
  - - Time: "20:00"
    - Strength: 60
  - - Time: "21:00"
    - Strength: 70
  $name: Stages
  $description: Add or remove stages using the list controls. Use 1-96 unique times, different from Off time. Stages are sorted automatically.
- GradualTransitions: true
  $name: Gradually change strength between times
  $description: Interpolate between stages. Night light switches off at Off time and stays off until the next stage.
*/
// ==/WindhawkModSettings==
// clang-format on

#include <asyncinfo.h>
#include <intrin.h>
#include <roapi.h>
#include <winstring.h>
#include <wrl/client.h>
#include <wil/resource.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace nightlight {
using Bytes = std::vector<uint8_t>;
constexpr size_t kMaxRecord = 65536;
constexpr uint8_t kBool = 2, kU64 = 6, kStruct = 10, kList = 11, kI8 = 14,
                  kI16 = 15, kI32 = 16;

void Require(bool condition, const char* message) {
    if (!condition)
        throw std::runtime_error(message);
}

// Decode the saved state to resume Windows' requested temperature on unload.
// Reject unsupported types, duplicate fields, and invalid record lengths.
struct Field {
    uint16_t id;
    uint8_t type;
    Bytes value;
};
using Fields = std::vector<Field>;

struct Reader {
    const Bytes& bytes;
    size_t pos = 0;
    uint8_t Byte() {
        Require(pos < bytes.size(), "Truncated Bond data");
        return bytes[pos++];
    }
    void Skip(size_t count) {
        Require(count <= bytes.size() - pos, "Invalid Bond length");
        pos += count;
    }
    uint64_t Varint() {
        uint64_t value = 0;
        for (unsigned shift = 0; shift < 70; shift += 7) {
            const auto b = Byte();
            Require(shift != 63 || b <= 1, "Bond varint overflow");
            value |= uint64_t(b & 127) << shift;
            if (!(b & 128))
                return value;
        }
        throw std::runtime_error("Invalid Bond varint");
    }
    void Value(uint8_t type, unsigned depth) {
        Require(depth < 32, "Bond nesting too deep");
        switch (type) {
            case 2:
                Require(Byte() <= 1, "Invalid Bond boolean");
                break;
            case 3:
            case 14:
                Skip(1);
                break;
            case 4:
            case 5:
            case 6:
            case 15:
            case 16:
            case 17:
                Varint();
                break;
            case 7:
                Skip(4);
                break;
            case 8:
                Skip(8);
                break;
            case 9:
            case 18: {
                auto n = Varint();
                Require(n <= kMaxRecord, "Bond string too large");
                Skip(static_cast<size_t>(n) * (type == 18 ? 2 : 1));
                break;
            }
            case 10:
                Struct(depth + 1);
                break;
            case 11:
            case 12:
            case 13: {
                const auto first = Byte();
                const auto second = type == 13 ? Byte() : 0;
                Require(first >= 2 && first <= 18 &&
                            (type != 13 || (second >= 2 && second <= 18)),
                        "Invalid Bond container type");
                const auto count = Varint();
                Require(count <= kMaxRecord, "Bond container too large");
                for (uint64_t i = 0; i < count; ++i) {
                    Value(first, depth + 1);
                    if (type == 13)
                        Value(second, depth + 1);
                }
                break;
            }
            default:
                throw std::runtime_error("Unsupported Bond type");
        }
    }
    Fields Struct(unsigned depth = 0) {
        Fields result;
        while (true) {
            const auto header = Byte();
            if (header == 0)
                return result;
            const auto type = static_cast<uint8_t>(header & 31);
            Require(type >= 2 && type <= 18, "Unsupported Bond field");
            uint16_t id = header >> 5;
            if (id == 6)
                id = Byte();
            else if (id == 7) {
                id = Byte();
                id |= uint16_t(Byte()) << 8;
            }
            Require(result.empty() || result.back().id < id,
                    "Unordered or duplicate Bond field");
            const auto start = pos;
            Value(type, depth);
            result.push_back(
                {id, type, Bytes(bytes.begin() + start, bytes.begin() + pos)});
        }
    }
};

uint64_t Number(const Bytes& value) {
    Reader r{value};
    auto n = r.Varint();
    Require(r.pos == value.size(), "Invalid numeric field");
    return n;
}
const Field* Find(const Fields& fields, uint16_t id) {
    auto it = std::find_if(fields.begin(), fields.end(),
                           [id](const Field& f) { return f.id == id; });
    return it == fields.end() ? nullptr : &*it;
}
const Field& Get(const Fields& fields, uint16_t id, uint8_t type) {
    const auto* f = Find(fields, id);
    Require(f && f->type == type, "Unexpected Night light schema");
    return *f;
}
Fields Parse(const Bytes& bytes, bool marshaled = false) {
    Require(bytes.size() <= kMaxRecord, "Night light record too large");
    Reader r{bytes};
    if (marshaled) {
        Require(r.Byte() == 0x43 && r.Byte() == 0x42 && r.Byte() == 1 &&
                    r.Byte() == 0,
                "Unsupported CloudStore format (expected Bond CB v1)");
    }
    auto fields = r.Struct();
    Require(r.pos == bytes.size(), "Trailing Bond data");
    return fields;
}
bool Bool(const Fields& f, uint16_t id, bool fallback = false) {
    if (!Find(f, id))
        return fallback;
    return Get(f, id, kBool).value.at(0) != 0;
}

Fields ReadPayload(const Bytes& bytes) {
    auto outer = Parse(bytes, true);
    auto metadata = Parse(Get(outer, 0, kStruct).value);
    Require(Bool(metadata, 0), "Invalid CloudStore metadata");
    auto container = Parse(Get(outer, 1, kStruct).value);
    Get(container, 0, kU64);
    auto wrapper = Parse(Get(container, 1, kStruct).value);
    const auto& list = Get(wrapper, 1, kList).value;
    Reader r{list};
    Require(r.Byte() == kI8, "Unexpected Night light payload type");
    auto n = r.Varint();
    Require(n == list.size() - r.pos, "Invalid Night light payload length");
    return Parse(Bytes(list.begin() + r.pos, list.end()), true);
}

bool IsOn(const Fields& fields) {
    Require(Bool(fields, 30, true),
            "Night light is unavailable for this display driver");
    if (!Find(fields, 0))
        return false;
    Require(Number(Get(fields, 0, kI32).value) == 0,
            "Unknown Night light state");
    return true;
}

struct Entry {
    int minute;
    int strength;
};
struct Config {
    std::vector<Entry> schedule;
    bool gradual;
};
int ParseTime(const std::wstring& value) {
    Require(value.size() == 5 && value[2] == L':' && value[0] >= L'0' &&
                value[0] <= L'9' && value[1] >= L'0' && value[1] <= L'9' &&
                value[3] >= L'0' && value[3] <= L'9' && value[4] >= L'0' &&
                value[4] <= L'9',
            "Times must use HH:MM");
    int h = (value[0] - L'0') * 10 + value[1] - L'0';
    int m = (value[3] - L'0') * 10 + value[4] - L'0';
    Require(h < 24 && m < 60, "Time is out of range");
    return h * 60 + m;
}
void ValidateSchedule(std::vector<Entry>& entries) {
    Require(!entries.empty() && entries.size() <= 97,
            "Use at most 96 stages and one off time");
    std::sort(entries.begin(), entries.end(),
              [](Entry a, Entry b) { return a.minute < b.minute; });
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        Require(e.minute >= 0 && e.minute < 1440 && e.strength >= 0 &&
                    e.strength <= 100,
                "Schedule time or strength is out of range");
        Require(i == 0 || entries[i - 1].minute != e.minute,
                "Schedule times must be unique");
    }
}
std::vector<Entry> BuildSchedule(std::vector<Entry> stages,
                                 const std::wstring& offTime) {
    Require(!stages.empty() && stages.size() <= 96,
            "Use between 1 and 96 stages");
    for (const auto& stage : stages)
        Require(stage.strength >= 1 && stage.strength <= 100,
                "Stage strength must be between 1 and 100");
    stages.push_back({ParseTime(offTime), 0});
    ValidateSchedule(stages);
    return stages;
}
double StrengthAt(const std::vector<Entry>& entries,
                  double minute,
                  bool gradual) {
    Require(!entries.empty() && minute >= 0 && minute < 1440,
            "Invalid schedule evaluation");
    auto next =
        std::upper_bound(entries.begin(), entries.end(), minute,
                         [](double m, const Entry& e) { return m < e.minute; });
    auto prev = next == entries.begin() ? entries.end() - 1 : next - 1;
    if (next == entries.end())
        next = entries.begin();
    if (!gradual || entries.size() == 1 || prev->strength == 0 ||
        next->strength == 0)
        return prev->strength;
    int duration = (next->minute - prev->minute + 1440) % 1440;
    double elapsed = minute - prev->minute;
    if (elapsed < 0)
        elapsed += 1440;
    return prev->strength +
           (next->strength - prev->strength) * elapsed / duration;
}
int Kelvin(double strength) {
    Require(std::isfinite(strength) && strength >= 0 && strength <= 100,
            "Invalid strength");
    return static_cast<int>(std::lround(6500 - 53 * strength));
}

std::wstring Key(bool settings) {
    std::wstring name = settings ? L"settings" : L"bluelightreductionstate";
    return std::wstring(
               L"Software\\Microsoft\\Windows\\CurrentVersion\\CloudStore\\Stor"
               L"e\\DefaultAccount\\Current\\") +
           L"default$windows.data.bluelightreduction." + name +
           L"\\windows.data.bluelightreduction." + name;
}
Bytes ReadRegistry(bool settings) {
    DWORD size = 0;
    auto key = Key(settings);
    auto error = RegGetValueW(HKEY_CURRENT_USER, key.c_str(), L"Data",
                              RRF_RT_REG_BINARY, nullptr, nullptr, &size);
    Require(error == ERROR_SUCCESS && size && size <= kMaxRecord,
            "Cannot read Night light registry record; open Night light in "
            "Windows Settings first");
    Bytes bytes(size);
    error = RegGetValueW(HKEY_CURRENT_USER, key.c_str(), L"Data",
                         RRF_RT_REG_BINARY, nullptr, bytes.data(), &size);
    Require(error == ERROR_SUCCESS, "Night light registry read failed");
    bytes.resize(size);
    return bytes;
}
struct NativeNightLight {
    int temperature;
    bool preview;
};

NativeNightLight DecodeNativeNightLight(const Bytes& settingsBytes,
                                       const Bytes& stateBytes) {
    auto settings = ReadPayload(settingsBytes);
    const bool on = IsOn(ReadPayload(stateBytes));
    const bool preview = Bool(settings, 70);
    const auto* field = Find(settings, 40);
    uint64_t raw = field ? Number(Get(settings, 40, kI16).value) : 13000;
    Require(!(raw & 1) && raw / 2 >= 1200 && raw / 2 <= 6500,
            "Invalid Night light temperature");
    return {on || preview ? static_cast<int>(raw / 2) : 6500, preview};
}

NativeNightLight ReadNativeNightLight() {
    return DecodeNativeNightLight(ReadRegistry(true), ReadRegistry(false));
}

// Private ABI used by Windows.Shell.BlueLightReduction.dll. The interface
// below includes the methods preceding the last one used by the mod.
struct DisplayEnhancement : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_IsNightLightCapable(BYTE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsNightLightOverridden(BYTE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE StartNightLightTransition(
        float kelvin, double durationMs, int transition) = 0;
    virtual HRESULT STDMETHODCALLTYPE add_IsNightLightOverriddenChanged(
        IUnknown* handler, INT64* token) = 0;
    virtual HRESULT STDMETHODCALLTYPE remove_IsNightLightOverriddenChanged(
        INT64 token) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnableModernNightLight(BYTE*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsAdaptiveColorCapable(BYTE*) = 0;
};

struct DisplayEnhancementOperation : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE put_Completed(IUnknown*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(IUnknown**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResults(DisplayEnhancement**) = 0;
};

struct DisplayEnhancementStatics : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE FromIdAsync(
        HSTRING id, DisplayEnhancementOperation**) = 0;
};

constexpr GUID kDisplayEnhancementStatics = {
    0x22771028, 0x1658, 0x4e5e,
    {0xab, 0x77, 0x30, 0x36, 0x91, 0xa8, 0x6f, 0xda}};
constexpr wchar_t kDisplayEnhancementClass[] =
    L"Windows.Internal.Graphics.Display.DisplayEnhancementManagement."
    L"DisplayEnhancementManagement";

using Microsoft::WRL::ComPtr;

using UniqueHString =
    wil::unique_any<HSTRING, decltype(&WindowsDeleteString), WindowsDeleteString>;

UniqueHString MakeHString(const std::wstring& value) {
    UniqueHString result;
    Require(SUCCEEDED(WindowsCreateString(
                value.c_str(), static_cast<UINT32>(value.size()), result.put())),
            "Cannot allocate a Windows display string");
    return result;
}

ComPtr<DisplayEnhancement> OpenDisplay(DisplayEnhancementStatics* statics,
                                       const wchar_t* devicePath) {
    auto path = MakeHString(devicePath);
    ComPtr<DisplayEnhancementOperation> operation;
    auto hr = statics->FromIdAsync(path.get(), operation.GetAddressOf());
    Require(SUCCEEDED(hr) && operation,
            "Cannot open the Windows display");
    ComPtr<IAsyncInfo> info;
    Require(SUCCEEDED(operation.As(&info)), "Cannot query display open status");
    const auto deadline = GetTickCount64() + 1500;
    for (;;) {
        AsyncStatus status;
        Require(SUCCEEDED(info->get_Status(&status)),
                "Cannot query display open status");
        if (status != AsyncStatus::Started)
            break;
        if (GetTickCount64() >= deadline) {
            info->Cancel();
            throw std::runtime_error("Opening the Windows display timed out");
        }
        Sleep(25);
    }
    ComPtr<DisplayEnhancement> display;
    Require(SUCCEEDED(operation->GetResults(display.GetAddressOf())) && display,
            "Cannot get the Windows display");
    return display;
}

std::vector<DISPLAYCONFIG_PATH_INFO> ActiveDisplayPaths() {
    for (int attempt = 0; attempt < 3; ++attempt) {
        UINT32 pathCount = 0, modeCount = 0;
        Require(GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                            &modeCount) == ERROR_SUCCESS,
                "Cannot size the active display list");
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
        auto result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                         paths.data(), &modeCount, modes.data(),
                                         nullptr);
        if (result == ERROR_INSUFFICIENT_BUFFER)
            continue;
        Require(result == ERROR_SUCCESS, "Cannot enumerate active displays");
        paths.resize(pathCount);
        return paths;
    }
    throw std::runtime_error("The display configuration is changing");
}

class DisplayControl {
    using SetDeviceTemperature = BOOL(WINAPI*)(HDC, float, DWORD, DWORD);

    struct Target {
        ComPtr<DisplayEnhancement> display;
        std::wstring source;
        bool capable;
    };

    std::vector<Target> targets_;
    wil::unique_hmodule mscms_;
    bool useDisplayService_ = false;
    bool applied_ = false;

    bool ApplyLegacy(const std::vector<Target>& targets, int kelvin) {
        if (!mscms_) {
            mscms_.reset(LoadLibraryExW(L"mscms.dll", nullptr,
                                       LOAD_LIBRARY_SEARCH_SYSTEM32));
        }
        Require(static_cast<bool>(mscms_),
                "Cannot load Windows color management");
        auto setTemperature = reinterpret_cast<SetDeviceTemperature>(
            GetProcAddress(mscms_.get(), "InternalSetDeviceTemperature"));
        Require(setTemperature,
                "The Windows Night light color API is unavailable");
        bool success = true;
        std::vector<std::wstring> appliedSources;
        for (const auto& target : targets) {
            if (!target.capable ||
                std::find(appliedSources.begin(), appliedSources.end(),
                          target.source) != appliedSources.end()) {
                continue;
            }
            wil::unique_hdc dc(CreateDCW(L"DISPLAY", target.source.c_str(),
                                         nullptr, nullptr));
            if (!dc ||
                !setTemperature(dc.get(), static_cast<float>(kelvin), 0, 0)) {
                success = false;
                continue;
            }
            applied_ = true;
            appliedSources.push_back(target.source);
        }
        return success && !appliedSources.empty();
    }

public:
    bool HasApplied() const { return applied_; }

    void Refresh() {
        auto name = MakeHString(kDisplayEnhancementClass);
        ComPtr<DisplayEnhancementStatics> statics;
        Require(SUCCEEDED(RoGetActivationFactory(
                    name.get(), kDisplayEnhancementStatics,
                    reinterpret_cast<void**>(statics.GetAddressOf()))) && statics,
                "The Windows display enhancement service is unavailable");
        std::vector<Target> targets;
        bool useDisplayService = useDisplayService_;
        for (const auto& path : ActiveDisplayPaths()) {
            DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
            target.header = {DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME,
                             sizeof(target), path.targetInfo.adapterId,
                             path.targetInfo.id};
            Require(DisplayConfigGetDeviceInfo(&target.header) == ERROR_SUCCESS,
                    "Cannot resolve an active display");
            if (!target.monitorDevicePath[0])
                continue;
            auto display = OpenDisplay(statics.Get(), target.monitorDevicePath);
            BYTE capable = 0, modern = 0, adaptive = 0;
            Require(SUCCEEDED(display->get_IsNightLightCapable(&capable)) &&
                        SUCCEEDED(display->get_EnableModernNightLight(&modern)) &&
                        SUCCEEDED(display->get_IsAdaptiveColorCapable(&adaptive)),
                    "Cannot read Windows Night light display capabilities");
            // Windows selects the service path for all displays if any
            // supports modern Night light or adaptive color.
            useDisplayService |= modern || adaptive;
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
            source.header = {DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME,
                             sizeof(source), path.sourceInfo.adapterId,
                             path.sourceInfo.id};
            Require(DisplayConfigGetDeviceInfo(&source.header) == ERROR_SUCCESS,
                    "Cannot resolve the display color device");
            targets.push_back({std::move(display), source.viewGdiDeviceName,
                               capable != 0});
        }
        Require(std::any_of(targets.begin(), targets.end(),
                            [](const Target& target) { return target.capable; }),
                "No active display supports Windows Night light");
        if (useDisplayService && !useDisplayService_ && applied_) {
            Require(ApplyLegacy(targets, 6500),
                    "Cannot clear the previous Night light display transform");
        }
        useDisplayService_ = useDisplayService;
        targets_ = std::move(targets);
    }

    void Apply(int kelvin) {
        if (!useDisplayService_) {
            Require(ApplyLegacy(targets_, kelvin),
                    "Cannot apply the Night light temperature to every display");
            return;
        }
        bool success = true;
        for (const auto& target : targets_) {
            if (!target.capable)
                continue;
            if (SUCCEEDED(target.display->StartNightLightTransition(
                    static_cast<float>(kelvin), 0, 0))) {
                applied_ = true;
            } else {
                success = false;
            }
        }
        Require(success,
                "Cannot apply the Night light temperature to every display");
    }
};

}  // namespace nightlight

#ifndef NIGHT_LIGHT_TEST
#include <shellapi.h>
#include <windhawk_utils.h>
#include <wtsapi32.h>
#include <mutex>

namespace {
using namespace nightlight;
std::mutex configMutex;
std::optional<Config>
    config;  // Empty until a valid saved schedule is available.
HANDLE stopEvent = nullptr, changedEvent = nullptr, worker = nullptr;
UINT taskbarCreatedMessage = 0;
bool refreshRequested =
    true;  // Only accessed by the worker and its window proc.

LRESULT CALLBACK EventWindowProc(HWND hwnd,
                                 UINT message,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    if (message == WM_QUERYENDSESSION)
        return TRUE;
    if (message == WM_ENDSESSION && wParam) {
        SetEvent(stopEvent);  // Best-effort cleanup; forced termination can
                              // still interrupt it.
        return 0;
    }
    if (message == WM_DISPLAYCHANGE || message == WM_TIMECHANGE ||
        (taskbarCreatedMessage && message == taskbarCreatedMessage) ||
        (message == WM_WTSSESSION_CHANGE && wParam == WTS_SESSION_UNLOCK) ||
        (message == WM_POWERBROADCAST && (wParam == PBT_APMRESUMEAUTOMATIC ||
                                          wParam == PBT_APMRESUMESUSPEND))) {
        refreshRequested = true;
        return message == WM_POWERBROADCAST ? TRUE : 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void WaitForWork(DWORD timeout) {
    HANDLE events[]{stopEvent, changedEvent};
    MsgWaitForMultipleObjectsEx(2, events, timeout, QS_ALLINPUT,
                                MWMO_INPUTAVAILABLE);
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

Config LoadConfig() {
    Config result;
    result.gradual = Wh_GetIntSetting(L"GradualTransitions") != 0;
    std::wstring offTime = WindhawkUtils::StringSetting::make(L"OffTime").get();
    std::vector<Entry> stages;
    for (int i = 0; i <= 96; ++i) {
        std::wstring time =
            WindhawkUtils::StringSetting::make(L"Stages[%d].Time", i).get();
        if (time.empty())
            break;
        stages.push_back(
            {ParseTime(time), Wh_GetIntSetting(L"Stages[%d].Strength", i)});
    }
    result.schedule = BuildSchedule(std::move(stages), offTime);
    return result;
}
DWORD WINAPI Run(void*) {
    const auto winrt = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(winrt)) {
        Wh_Log(L"Cannot initialize the display runtime (0x%08X)", winrt);
        return 0;
    }
    const auto uninitialize = wil::scope_exit([] { RoUninitialize(); });
    DisplayControl display;
    std::optional<int> restoreTemperature;
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(&EventWindowProc), &module);
    const wchar_t* className = L"WindhawkGradualNightLightEvents";
    WNDCLASSW wc{};
    wc.lpfnWndProc = EventWindowProc;
    wc.hInstance = module;
    wc.lpszClassName = className;
    bool registered = RegisterClassW(&wc) != 0;
    HWND eventWindow =
        registered ? CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                     className, L"", WS_POPUP, 0, 0, 0, 0,
                                     nullptr, nullptr, module, nullptr)
                   : nullptr;
    bool sessionRegistered =
        eventWindow &&
        WTSRegisterSessionNotification(eventWindow, NOTIFY_FOR_THIS_SESSION);
    if (!eventWindow)
        Wh_Log(L"Event window unavailable; scheduled changes still run");
    taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    refreshRequested = true;
    std::string lastError;
    while (WaitForSingleObject(stopEvent, 0) == WAIT_TIMEOUT) {
        try {
            std::optional<Config> current;
            {
                std::lock_guard lock(configMutex);
                current = config;
            }
            if (current) {
                const auto native = ReadNativeNightLight();
                if (!native.preview) {
                    SYSTEMTIME now;
                    GetLocalTime(&now);
                    double minute = now.wHour * 60 + now.wMinute +
                                    now.wSecond / 60.0;
                    const auto temperature = Kelvin(
                        StrengthAt(current->schedule, minute, current->gradual));
                    restoreTemperature = native.temperature;
                    if (refreshRequested) {
                        display.Refresh();
                        refreshRequested = false;
                    }
                    if (WaitForSingleObject(stopEvent, 0) == WAIT_TIMEOUT)
                        display.Apply(temperature);
                }
            }
            lastError.clear();
        } catch (const std::exception& e) {
            refreshRequested = true;
            if (lastError != e.what())
                Wh_Log(L"Night light: %S", e.what());
            lastError = e.what();
        }
        WaitForWork(10000);
    }
    if (display.HasApplied() && restoreTemperature) {
        try {
            restoreTemperature = ReadNativeNightLight().temperature;
        } catch (const std::exception& e) {
            Wh_Log(L"Using the last readable Windows Night light state: %S",
                   e.what());
        }
        try {
            display.Apply(*restoreTemperature);
        } catch (const std::exception& e) {
            Wh_Log(L"Night light runtime restoration failed: %S", e.what());
        }
    }
    if (sessionRegistered)
        WTSUnRegisterSessionNotification(eventWindow);
    if (eventWindow)
        DestroyWindow(eventWindow);
    if (registered)
        UnregisterClassW(className, module);
    return 0;
}
}  // namespace

BOOL WhTool_ModInit() {
    config.reset();
    try {
        config = LoadConfig();
    } catch (const std::exception& e) {
        Wh_Log(L"Invalid schedule; waiting for valid settings: %S", e.what());
    }
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    changedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (stopEvent && changedEvent)
        worker = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
    if (!worker) {
        if (stopEvent)
            CloseHandle(stopEvent);
        if (changedEvent)
            CloseHandle(changedEvent);
        stopEvent = changedEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}
void WhTool_ModSettingsChanged() {
    try {
        auto next = LoadConfig();
        {
            std::lock_guard lock(configMutex);
            config = std::move(next);
        }
        SetEvent(changedEvent);
    } catch (const std::exception& e) {
        Wh_Log(L"Invalid schedule; keeping the last valid schedule: %S",
               e.what());
    }
}
void WhTool_ModUninit() {
    if (worker) {
        SetEvent(stopEvent);
        WaitForSingleObject(worker, INFINITE);
    }
    if (worker)
        CloseHandle(worker);
    if (stopEvent)
        CloseHandle(stopEvent);
    if (changedEvent)
        CloseHandle(changedEvent);
    worker = stopEvent = changedEvent = nullptr;
}

// clang-format off
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
// clang-format on
#endif
