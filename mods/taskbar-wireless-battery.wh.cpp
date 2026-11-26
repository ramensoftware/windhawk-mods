// ==WindhawkMod==
// @id              taskbar-wireless-battery
// @name            Wireless Battery Indicator
// @description     Shows the battery level of wireless mice and controllers (Razer, Xbox/XInput) next to the system tray
// @version         1.0.0
// @author          simonzk27
// @github          https://github.com/simonzk27
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lhid -lsetupapi -lshcore -lshlwapi -lversion -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Wireless Battery Indicator

Shows the battery level of your wireless devices right next to the system tray
on the Windows 11 taskbar.

![Mouse and controller next to the system tray](https://raw.githubusercontent.com/simonzk27/windhawk-mod-assets/main/taskbar-wireless-battery/tray.png)

Hover a device to see its details:

![Mouse tooltip](https://raw.githubusercontent.com/simonzk27/windhawk-mod-assets/main/taskbar-wireless-battery/tooltip-mouse.png)

![Controller tooltip](https://raw.githubusercontent.com/simonzk27/windhawk-mod-assets/main/taskbar-wireless-battery/tooltip-controller.png)

Click the indicator to open a panel with every device, its level, charging
state, estimated time remaining and last reading:

![Devices panel](https://raw.githubusercontent.com/simonzk27/windhawk-mod-assets/main/taskbar-wireless-battery/panel.png)

## Supported devices

| Device | Battery | Charging | Tested |
|---|---|---|---|
| Razer mice and keyboards (HyperSpeed dongle or cable) | exact % | ✅ | DeathAdder V3 Pro |
| Bluetooth devices whose battery Windows shows (headphones, earbuds, keyboards, mice…) | exact % | — | Redmi Buds 6 Active |
| PlayStation DualShock 4 and DualSense (USB) | % | ✅ | DualShock 4 |
| PlayStation controllers over Bluetooth | % | ✅ | — (see notes) |
| Xbox and other XInput controllers | exact % when Windows reports it, otherwise 4 levels | ✅ | Wired Xbox Series controller |
| Logitech HID++ devices (Lightspeed, Unifying, Bolt receivers or cable) | % or estimated from voltage | ✅ | — (not tested on real hardware) |

Wired controllers without batteries are shown with a plug icon.

## Features

- **Estimated time remaining**: from how fast the level changes, e.g. "About
  14 h left" or "Full in about 40 min". It needs about 10–20 minutes of
  readings and is kept across Explorer restarts. Shown in the tooltip and the
  panel, or next to the percentage.
- **Devices panel** on click, or open Bluetooth settings instead.
- Icon per device type (mouse, keyboard, headphones, controller), lightning bolt
  while charging, red below the low battery threshold.
- Primary taskbar only or all taskbars, left or right of the hidden icons
  arrow.

## Customization

Font, font size, bold, text color, icon size, display mode (icon + %, icon
only, % only, battery bar), spacing, low battery threshold and color.

Per device (matched by name) you can rename it, hide it, or use your own PNG or
SVG icon.

## Notes

- Windows 11 only.
- Razer and Logitech devices are read every 60 seconds by default
  (configurable), Bluetooth devices and PlayStation controllers every 15
  seconds, XInput controllers every 5 seconds.
- Bluetooth PlayStation controllers are only read after another app (for
  example Steam) has switched them to full reports. The mod doesn't do that
  itself, because it breaks DirectInput for other apps until the controller is
  turned off.
- Razer Synapse and Logitech G HUB send the same kind of requests to the
  devices. Running them at the same time can occasionally cause a missed
  reading, which is retried on the next refresh.
- Compatible with Windows 11 Taskbar Styler: the indicator is a `StackPanel`
  named `BatteryWidget` and can be styled with the target
  `StackPanel#BatteryWidget`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: leftOfChevron
  $name: Position
  $options:
  - leftOfChevron: Left of the hidden icons arrow
  - rightOfChevron: Right of the hidden icons arrow
- displayMode: iconAndPercent
  $name: Display mode
  $options:
  - iconAndPercent: Icon and percentage
  - iconOnly: Icon only
  - percentOnly: Percentage only
  - batteryBar: Icon and battery bar
- iconSize: 16
  $name: Icon size
- fontFamily: Segoe UI Variable Text
  $name: Font
- fontSize: 12
  $name: Font size
- bold: false
  $name: Bold text
- textColor: ""
  $name: Text color
  $description: Empty to follow the taskbar theme. Format #RRGGBB or #AARRGGBB
- lowBatteryThreshold: 20
  $name: Low battery threshold (%)
- lowBatteryColor: "#FFE81123"
  $name: Low battery color
- showChargingIndicator: true
  $name: Show charging indicator
- hideDisconnected: true
  $name: Hide disconnected devices
  $description: When disabled, devices that stop responding stay visible (dimmed) with their last known level
- pollInterval: 60
  $name: Razer battery refresh interval (seconds)
  $description: Controllers are checked every 5 seconds regardless of this value
- deviceSpacing: 8
  $name: Spacing between devices (px)
- showOnAllTaskbars: false
  $name: Show on all taskbars
  $description: By default the indicator is shown only on the taskbar that has the notification area
- timeEstimate: tooltip
  $name: Estimated time remaining
  $description: Estimated from how fast the level changes. Needs about 20 minutes of readings
  $options:
  - tooltip: In the tooltip and the panel
  - inline: Next to the percentage
  - off: Off
- clickAction: panel
  $name: Click action
  $options:
  - panel: Show the devices panel
  - bluetoothSettings: Open Bluetooth settings
  - none: Nothing
- devices:
  - - match: ""
      $name: Device name contains
    - displayName: ""
      $name: Display name
    - customIcon: ""
      $name: Custom icon
      $description: Full path to a PNG or SVG file
    - hide: false
      $name: Hide this device
  $name: Per-device settings
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <hidsdi.h>
#include <setupapi.h>
#include <shcore.h>
#include <shlwapi.h>
#include <xinput.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Power.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

enum class DisplayMode { IconAndPercent, IconOnly, PercentOnly, BatteryBar };

enum class Position { LeftOfChevron, RightOfChevron };

enum class TimeEstimateMode { Tooltip, Inline, Off };

enum class ClickAction { Panel, BluetoothSettings, None };

struct DeviceOverride {
    std::wstring match;
    std::wstring displayName;
    std::wstring customIcon;
    bool hide = false;
};

struct Settings {
    Position position = Position::LeftOfChevron;
    DisplayMode displayMode = DisplayMode::IconAndPercent;
    int iconSize = 16;
    std::wstring fontFamily = L"Segoe UI Variable Text";
    int fontSize = 12;
    bool bold = false;
    std::wstring textColor;
    int lowBatteryThreshold = 20;
    std::wstring lowBatteryColor = L"#FFE81123";
    bool showChargingIndicator = true;
    bool hideDisconnected = true;
    int pollInterval = 60;
    int deviceSpacing = 8;
    bool showOnAllTaskbars = false;
    TimeEstimateMode timeEstimate = TimeEstimateMode::Tooltip;
    ClickAction clickAction = ClickAction::Panel;
    std::vector<DeviceOverride> devices;
};

enum class DeviceKind { Mouse, Gamepad, Headphones, Keyboard, Generic };

struct DeviceState {
    std::wstring key;
    std::wstring name;
    DeviceKind kind = DeviceKind::Mouse;
    int percent = -1;
    bool approximate = false;
    bool charging = false;
    bool wiredNoBattery = false;
    bool stale = false;
    ULONGLONG lastSeen = 0;
    std::wstring updated;
    int estimateMinutes = -1;
    double precisePercent = -1;

    bool SameDisplay(const DeviceState& o) const {
        return key == o.key && name == o.name && kind == o.kind &&
               percent == o.percent && approximate == o.approximate &&
               charging == o.charging && wiredNoBattery == o.wiredNoBattery &&
               stale == o.stale && updated == o.updated &&
               estimateMinutes == o.estimateMinutes;
    }
};

struct WidgetInstance {
    winrt::weak_ref<wux::FrameworkElement> frame;
    winrt::weak_ref<wuxc::Panel> container;
    wuxc::StackPanel widget{nullptr};
    wuxc::ColumnDefinition addedColumn{nullptr};
    std::vector<std::pair<winrt::weak_ref<wux::FrameworkElement>, int>> shiftedChildren;
    void* frameAbi = nullptr;
};

std::mutex g_settingsMutex;
Settings g_settings;
std::atomic<int> g_settingsVersion{1};

std::mutex g_devicesMutex;
std::vector<DeviceState> g_devices;
std::atomic<int> g_dataVersion{1};

// Released on the taskbar thread in Wh_ModBeforeUninit.
[[clang::no_destroy]] std::optional<std::vector<WidgetInstance>> g_instances{std::in_place};
[[clang::no_destroy]] wux::DispatcherTimer g_uiTimer{nullptr};
winrt::event_token g_uiTimerToken{};
int g_renderedDataVersion = 0;
int g_renderedSettingsVersion = 0;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_needScan{true};
bool g_inEnsure = false;

HANDLE g_pollThread = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_refreshEvent = nullptr;

std::atomic<bool> g_systemTrayHooked{false};

Settings GetSettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

std::atomic<bool> g_showOnAllTaskbars{false};
std::atomic<Position> g_position{Position::LeftOfChevron};

std::wstring ReadStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

void LoadSettings() {
    Settings s;

    s.position = ReadStringSetting(L"position") == L"rightOfChevron"
                     ? Position::RightOfChevron
                     : Position::LeftOfChevron;

    std::wstring mode = ReadStringSetting(L"displayMode");
    if (mode == L"iconOnly") {
        s.displayMode = DisplayMode::IconOnly;
    } else if (mode == L"percentOnly") {
        s.displayMode = DisplayMode::PercentOnly;
    } else if (mode == L"batteryBar") {
        s.displayMode = DisplayMode::BatteryBar;
    } else {
        s.displayMode = DisplayMode::IconAndPercent;
    }

    s.iconSize = std::clamp(Wh_GetIntSetting(L"iconSize"), 8, 48);
    s.fontFamily = ReadStringSetting(L"fontFamily");
    if (s.fontFamily.empty()) {
        s.fontFamily = L"Segoe UI Variable Text";
    }
    s.fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 6, 40);
    s.bold = Wh_GetIntSetting(L"bold") != 0;
    s.textColor = ReadStringSetting(L"textColor");
    s.lowBatteryThreshold = std::clamp(Wh_GetIntSetting(L"lowBatteryThreshold"), 0, 100);
    s.lowBatteryColor = ReadStringSetting(L"lowBatteryColor");
    s.showChargingIndicator = Wh_GetIntSetting(L"showChargingIndicator") != 0;
    s.hideDisconnected = Wh_GetIntSetting(L"hideDisconnected") != 0;
    s.pollInterval = std::clamp(Wh_GetIntSetting(L"pollInterval"), 10, 3600);
    s.deviceSpacing = std::clamp(Wh_GetIntSetting(L"deviceSpacing"), 0, 64);
    s.showOnAllTaskbars = Wh_GetIntSetting(L"showOnAllTaskbars") != 0;

    std::wstring estimate = ReadStringSetting(L"timeEstimate");
    s.timeEstimate = estimate == L"inline" ? TimeEstimateMode::Inline
                     : estimate == L"off"  ? TimeEstimateMode::Off
                                            : TimeEstimateMode::Tooltip;
    std::wstring click = ReadStringSetting(L"clickAction");
    s.clickAction = click == L"bluetoothSettings" ? ClickAction::BluetoothSettings
                    : click == L"none"            ? ClickAction::None
                                                   : ClickAction::Panel;

    for (int i = 0; i < 32; i++) {
        std::wstring matchStr =
            WindhawkUtils::StringSetting::make(L"devices[%d].match", i).get();
        if (matchStr.empty()) {
            continue;
        }
        DeviceOverride o;
        o.match = ToLower(matchStr);
        o.displayName =
            WindhawkUtils::StringSetting::make(L"devices[%d].displayName", i)
                .get();
        o.customIcon =
            WindhawkUtils::StringSetting::make(L"devices[%d].customIcon", i)
                .get();
        o.hide = Wh_GetIntSetting(L"devices[%d].hide", i) != 0;
        s.devices.push_back(std::move(o));
    }

    g_showOnAllTaskbars = s.showOnAllTaskbars;
    g_position = s.position;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

#pragma region razer

#pragma pack(push, 1)
struct RazerReport {
    unsigned char reportId;
    unsigned char status;
    unsigned char transactionId;
    unsigned short remainingPackets;
    unsigned char protocolType;
    unsigned char dataSize;
    unsigned char commandClass;
    unsigned char commandId;
    unsigned char arguments[80];
    unsigned char crc;
    unsigned char reserved;
};
#pragma pack(pop)

static_assert(sizeof(RazerReport) == 91);

struct RazerModel {
    USHORT pid;
    PCWSTR name;
    bool wired;
};

const RazerModel kRazerModels[] = {
    {0x00B6, L"Razer DeathAdder V3 Pro", true},
    {0x00B7, L"Razer DeathAdder V3 Pro", false},
    {0x00B2, L"Razer DeathAdder V3 HyperSpeed", false},
    {0x00AA, L"Razer Basilisk V3 Pro", true},
    {0x00AB, L"Razer Basilisk V3 Pro", false},
    {0x00C0, L"Razer Viper V3 Pro", true},
    {0x00C1, L"Razer Viper V3 Pro", false},
    {0x007C, L"Razer DeathAdder V2 Pro", true},
    {0x007D, L"Razer DeathAdder V2 Pro", false},
    {0x0083, L"Razer Basilisk X HyperSpeed", false},
    {0x0088, L"Razer Basilisk Ultimate", false},
    {0x008F, L"Razer Naga Pro", true},
    {0x0090, L"Razer Naga Pro", false},
    {0x007A, L"Razer Viper Ultimate", true},
    {0x007B, L"Razer Viper Ultimate", false},
    {0x00A5, L"Razer Viper V2 Pro", true},
    {0x00A6, L"Razer Viper V2 Pro", false},
};

const RazerModel* FindRazerModel(USHORT pid) {
    for (const auto& m : kRazerModels) {
        if (m.pid == pid) {
            return &m;
        }
    }
    return nullptr;
}

unsigned char RazerCrc(const RazerReport& r) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(&r) + 1;
    unsigned char crc = 0;
    for (int i = 2; i < 88; i++) {
        crc ^= p[i];
    }
    return crc;
}

bool RazerQuery(HANDLE h,
                unsigned char txId,
                unsigned char cls,
                unsigned char id,
                RazerReport& out) {
    RazerReport req{};
    req.transactionId = txId;
    req.dataSize = 0x02;
    req.commandClass = cls;
    req.commandId = id;
    req.crc = RazerCrc(req);

    for (int attempt = 0; attempt < 3; attempt++) {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            return false;
        }
        if (!HidD_SetFeature(h, &req, sizeof(req))) {
            return false;
        }
        Sleep(60);
        memset(&out, 0, sizeof(out));
        if (!HidD_GetFeature(h, &out, sizeof(out))) {
            return false;
        }
        if (out.status == 0x02 && out.commandClass == cls &&
            out.commandId == id) {
            return true;
        }
        if (out.status == 0x01) {
            Sleep(100);
            continue;
        }
        return false;
    }
    return false;
}

struct RazerReading {
    USHORT pid;
    std::wstring name;
    DeviceKind kind;
    bool wired;
    bool ok;
    int percent;
    double precisePercent;
    bool charging;
};

std::vector<RazerReading> PollRazer() {
    std::vector<RazerReading> readings;

    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO set = SetupDiGetClassDevsW(&hidGuid, nullptr, nullptr,
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (set == INVALID_HANDLE_VALUE) {
        return readings;
    }

    SP_DEVICE_INTERFACE_DATA ifData{sizeof(ifData)};
    for (DWORD i = 0;
         SetupDiEnumDeviceInterfaces(set, nullptr, &hidGuid, i, &ifData);
         i++) {
        DWORD needed = 0;
        SetupDiGetDeviceInterfaceDetailW(set, &ifData, nullptr, 0, &needed,
                                         nullptr);
        if (!needed) {
            continue;
        }
        std::vector<BYTE> buf(needed);
        auto detail =
            reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buf.data());
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (!SetupDiGetDeviceInterfaceDetailW(set, &ifData, detail, needed,
                                              nullptr, nullptr)) {
            continue;
        }

        std::wstring lower = ToLower(detail->DevicePath);
        if (lower.find(L"vid_1532") == std::wstring::npos) {
            continue;
        }

        HANDLE h = CreateFileW(detail->DevicePath, 0,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                               OPEN_EXISTING, 0, nullptr);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }

        HIDD_ATTRIBUTES attrs{sizeof(attrs)};
        HIDP_CAPS caps{};
        PHIDP_PREPARSED_DATA pp = nullptr;
        bool capsOk = false;
        if (HidD_GetAttributes(h, &attrs) && HidD_GetPreparsedData(h, &pp)) {
            capsOk = HidP_GetCaps(pp, &caps) == HIDP_STATUS_SUCCESS;
            HidD_FreePreparsedData(pp);
        }

        bool alreadyRead = std::any_of(
            readings.begin(), readings.end(),
            [&](const RazerReading& r) { return r.pid == attrs.ProductID && r.ok; });
        if (capsOk && !alreadyRead && caps.UsagePage == 0x01 &&
            (caps.Usage == 0x02 || caps.Usage == 0x06) &&
            caps.FeatureReportByteLength == sizeof(RazerReport)) {
            const RazerModel* model = FindRazerModel(attrs.ProductID);
            RazerReading reading{};
            reading.pid = attrs.ProductID;
            reading.kind = !model && caps.Usage == 0x06 ? DeviceKind::Keyboard
                                                        : DeviceKind::Mouse;
            reading.name = model ? model->name
                         : reading.kind == DeviceKind::Keyboard
                             ? L"Razer Keyboard"
                             : L"Razer Mouse";
            reading.wired = model ? model->wired : false;

            const unsigned char txIds[] = {0x1F, 0x3F, 0xFF};
            RazerReport r{};
            for (unsigned char tx : txIds) {
                if (RazerQuery(h, tx, 0x07, 0x80, r)) {
                    reading.ok = true;
                    reading.percent = (r.arguments[1] * 100 + 127) / 255;
                    reading.precisePercent = r.arguments[1] * 100.0 / 255.0;
                    if (RazerQuery(h, tx, 0x07, 0x84, r)) {
                        reading.charging = r.arguments[1] != 0;
                    }
                    break;
                }
            }
            readings.push_back(std::move(reading));
        }

        CloseHandle(h);
    }

    SetupDiDestroyDeviceInfoList(set);
    return readings;
}

#pragma endregion

#pragma region xinput

struct XInputReading {
    int index;
    bool wiredNoBattery;
    int percent;
};

using XInputGetState_t = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
using XInputGetBatteryInformation_t = DWORD(WINAPI*)(DWORD,
                                                      BYTE,
                                                      XINPUT_BATTERY_INFORMATION*);

HMODULE g_xinputModule = nullptr;
XInputGetState_t g_xinputGetState = nullptr;
XInputGetBatteryInformation_t g_xinputGetBatteryInformation = nullptr;
int g_lastXusbCount = -1;
ULONGLONG g_lastXInputReload = 0;

const GUID kXusbInterfaceGuid = {0xEC87F1E3,
                                 0xC13B,
                                 0x4100,
                                 {0xB5, 0xF7, 0x8B, 0x84, 0xD5, 0x42, 0x60, 0xCB}};

void UnloadXInput() {
    g_xinputGetState = nullptr;
    g_xinputGetBatteryInformation = nullptr;
    if (g_xinputModule) {
        FreeLibrary(g_xinputModule);
        g_xinputModule = nullptr;
    }
}

bool LoadXInput() {
    if (g_xinputModule) {
        return true;
    }
    g_xinputModule = LoadLibraryExW(L"xinput1_4.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_xinputModule) {
        return false;
    }
    g_xinputGetState = (XInputGetState_t)GetProcAddress(g_xinputModule,
                                                        "XInputGetState");
    g_xinputGetBatteryInformation = (XInputGetBatteryInformation_t)GetProcAddress(
        g_xinputModule, "XInputGetBatteryInformation");
    if (!g_xinputGetState || !g_xinputGetBatteryInformation) {
        UnloadXInput();
        return false;
    }
    g_lastXInputReload = GetTickCount64();
    return true;
}

int g_usbXusbCount = 0;

int CountXusbInterfaces() {
    HDEVINFO set = SetupDiGetClassDevsW(&kXusbInterfaceGuid, nullptr, nullptr,
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (set == INVALID_HANDLE_VALUE) {
        g_usbXusbCount = 0;
        return 0;
    }
    int count = 0;
    int usbCount = 0;
    SP_DEVICE_INTERFACE_DATA ifData{sizeof(ifData)};
    while (SetupDiEnumDeviceInterfaces(set, nullptr, &kXusbInterfaceGuid, count,
                                       &ifData)) {
        count++;
        DWORD needed = 0;
        SetupDiGetDeviceInterfaceDetailW(set, &ifData, nullptr, 0, &needed,
                                         nullptr);
        if (!needed) {
            continue;
        }
        std::vector<BYTE> buf(needed);
        auto detail =
            reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buf.data());
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (SetupDiGetDeviceInterfaceDetailW(set, &ifData, detail, needed,
                                             nullptr, nullptr)) {
            std::wstring path = ToLower(detail->DevicePath);
            if (path.find(L"\\\\?\\usb#") == 0 ||
                path.find(L"vid_045e&pid_02ff") != std::wstring::npos) {
                usbCount++;
            }
        }
    }
    SetupDiDestroyDeviceInfoList(set);
    g_usbXusbCount = usbCount;
    return count;
}

BYTE g_lastBatteryType[XUSER_MAX_COUNT] = {0xFE, 0xFE, 0xFE, 0xFE};
BYTE g_lastBatteryLevel[XUSER_MAX_COUNT] = {0xFE, 0xFE, 0xFE, 0xFE};

int CountXInputConnected() {
    int count = 0;
    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
        XINPUT_STATE state{};
        if (g_xinputGetState(i, &state) == ERROR_SUCCESS) {
            count++;
        }
    }
    return count;
}

std::vector<XInputReading> PollXInput() {
    std::vector<XInputReading> readings;

    int xusbCount = CountXusbInterfaces();
    if (xusbCount != g_lastXusbCount) {
        Wh_Log(L"XUSB interfaces: %d -> %d, reloading XInput", g_lastXusbCount,
               xusbCount);
        g_lastXusbCount = xusbCount;
        UnloadXInput();
        if (xusbCount > 0 && WaitForSingleObject(g_stopEvent, 750) ==
                                 WAIT_OBJECT_0) {
            return readings;
        }
    }

    if (!LoadXInput()) {
        return readings;
    }

    static int mismatchReloadedForCount = -1;
    if (xusbCount > CountXInputConnected() &&
        mismatchReloadedForCount != xusbCount &&
        GetTickCount64() - g_lastXInputReload > 15000) {
        mismatchReloadedForCount = xusbCount;
        Wh_Log(L"XInput sees fewer controllers than present, reloading");
        UnloadXInput();
        if (!LoadXInput()) {
            return readings;
        }
    }

    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
        XINPUT_STATE state{};
        if (g_xinputGetState(i, &state) != ERROR_SUCCESS) {
            continue;
        }
        XINPUT_BATTERY_INFORMATION battery{};
        if (g_xinputGetBatteryInformation(i, BATTERY_DEVTYPE_GAMEPAD,
                                          &battery) != ERROR_SUCCESS) {
            battery.BatteryType = BATTERY_TYPE_UNKNOWN;
        }

        if (battery.BatteryType != g_lastBatteryType[i] ||
            battery.BatteryLevel != g_lastBatteryLevel[i]) {
            Wh_Log(L"XInput %u battery type=%u level=%u (usb xusb=%d)", i,
                   battery.BatteryType, battery.BatteryLevel, g_usbXusbCount);
            g_lastBatteryType[i] = battery.BatteryType;
            g_lastBatteryLevel[i] = battery.BatteryLevel;
        }

        XInputReading reading{(int)i, false, -1};
        switch (battery.BatteryType) {
            case BATTERY_TYPE_WIRED:
                reading.wiredNoBattery = true;
                break;
            case BATTERY_TYPE_ALKALINE:
            case BATTERY_TYPE_NIMH:
                switch (battery.BatteryLevel) {
                    case BATTERY_LEVEL_EMPTY:
                        reading.percent = 5;
                        break;
                    case BATTERY_LEVEL_LOW:
                        reading.percent = 30;
                        break;
                    case BATTERY_LEVEL_MEDIUM:
                        reading.percent = 65;
                        break;
                    case BATTERY_LEVEL_FULL:
                        reading.percent = 100;
                        break;
                }
                break;
            default:
                if (g_usbXusbCount > 0) {
                    reading.wiredNoBattery = true;
                }
                break;
        }
        readings.push_back(reading);
    }
    return readings;
}

#pragma endregion

#pragma region bluetooth

const DEVPROPKEY kBatteryLevelKey = {
    {0x104EA319, 0x6EE2, 0x4701, {0xBD, 0x47, 0x8D, 0xDB, 0xF4, 0x25, 0xBB, 0xE5}},
    2};
const DEVPROPKEY kIsConnectedKey = {
    {0x83DA6326, 0x97A6, 0x4088, {0x94, 0x53, 0xA1, 0x92, 0x3F, 0x57, 0x3B, 0x29}},
    15};
const DEVPROPKEY kClassOfDeviceKey = {
    {0x2BD67D8B, 0x8BEB, 0x48D5, {0x87, 0xE0, 0x6C, 0xDA, 0x34, 0x28, 0x04, 0x0A}},
    10};
const DEVPROPKEY kClassOfDeviceAltKey = {
    {0x2BD67D8B, 0x8BEB, 0x48D5, {0x87, 0xE0, 0x6C, 0xDA, 0x34, 0x28, 0x04, 0x0A}},
    4};
const DEVPROPKEY kFriendlyNameKey = {
    {0xA45C254E, 0xDF1C, 0x4EFD, {0x80, 0x20, 0x67, 0xD1, 0x46, 0xA8, 0x50, 0xE0}},
    14};

struct BluetoothReading {
    std::wstring address;
    std::wstring name;
    DeviceKind kind;
    int percent;
};

std::wstring ExtractBluetoothAddress(const std::wstring& instanceId) {
    std::wstring upper = instanceId;
    for (auto& c : upper) {
        c = (wchar_t)towupper(c);
    }
    std::wstring result;
    for (size_t i = 0; i + 12 <= upper.size(); i++) {
        bool hex = true;
        for (size_t j = 0; j < 12 && hex; j++) {
            hex = iswxdigit(upper[i + j]) != 0;
        }
        bool boundaryBefore = i == 0 || !iswxdigit(upper[i - 1]);
        bool boundaryAfter = i + 12 == upper.size() || !iswxdigit(upper[i + 12]);
        if (hex && boundaryBefore && boundaryAfter) {
            result = upper.substr(i, 12);
        }
    }
    return result;
}

DeviceKind KindFromClassOfDevice(DWORD cod, const std::wstring& name) {
    if (cod) {
        DWORD major = (cod >> 8) & 0x1F;
        DWORD minor = (cod >> 2) & 0x3F;
        if (major == 0x04) {
            return DeviceKind::Headphones;
        }
        if (major == 0x05) {
            if (minor & 0x10) {
                return DeviceKind::Keyboard;
            }
            if (minor & 0x20) {
                return DeviceKind::Mouse;
            }
            DWORD sub = minor & 0x0F;
            if (sub == 0x01 || sub == 0x02) {
                return DeviceKind::Gamepad;
            }
        }
    }
    std::wstring lower = ToLower(name);
    const std::pair<PCWSTR, DeviceKind> keywords[] = {
        {L"buds", DeviceKind::Headphones},  {L"headset", DeviceKind::Headphones},
        {L"headphone", DeviceKind::Headphones}, {L"earbud", DeviceKind::Headphones},
        {L"airpods", DeviceKind::Headphones}, {L"keyboard", DeviceKind::Keyboard},
        {L"teclado", DeviceKind::Keyboard}, {L"mouse", DeviceKind::Mouse},
        {L"controller", DeviceKind::Gamepad}, {L"gamepad", DeviceKind::Gamepad},
    };
    for (const auto& [word, kind] : keywords) {
        if (lower.find(word) != std::wstring::npos) {
            return kind;
        }
    }
    return DeviceKind::Generic;
}

bool GetDevProp(HDEVINFO set,
                SP_DEVINFO_DATA* data,
                const DEVPROPKEY& key,
                std::vector<BYTE>& out,
                DEVPROPTYPE* type) {
    DWORD size = 0;
    DEVPROPTYPE t = 0;
    SetupDiGetDevicePropertyW(set, data, &key, &t, nullptr, 0, &size, 0);
    if (!size) {
        return false;
    }
    out.resize(size);
    if (!SetupDiGetDevicePropertyW(set, data, &key, &t, out.data(), size,
                                   &size, 0)) {
        return false;
    }
    if (type) {
        *type = t;
    }
    return true;
}

std::vector<BluetoothReading> PollBluetooth(
    std::vector<std::wstring>* connectedAddresses) {
    struct DeviceNode {
        std::wstring name;
        bool connected = false;
        DWORD cod = 0;
    };
    std::vector<std::pair<std::wstring, DeviceNode>> deviceNodes;
    std::vector<std::pair<std::wstring, int>> batteries;

    HDEVINFO set = SetupDiGetClassDevsW(nullptr, nullptr, nullptr,
                                        DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (set == INVALID_HANDLE_VALUE) {
        return {};
    }

    SP_DEVINFO_DATA data{sizeof(data)};
    WCHAR instanceId[512];
    std::vector<BYTE> buf;
    for (DWORD i = 0; SetupDiEnumDeviceInfo(set, i, &data); i++) {
        if (!SetupDiGetDeviceInstanceIdW(set, &data, instanceId,
                                         ARRAYSIZE(instanceId), nullptr)) {
            continue;
        }
        std::wstring id = instanceId;
        if (_wcsnicmp(instanceId, L"BTH", 3) != 0) {
            continue;
        }

        bool isDeviceNode = _wcsnicmp(instanceId, L"BTHENUM\\DEV_", 12) == 0 ||
                            _wcsnicmp(instanceId, L"BTHLE\\DEV_", 10) == 0;
        if (isDeviceNode) {
            DeviceNode node;
            DEVPROPTYPE type = 0;
            if (GetDevProp(set, &data, kFriendlyNameKey, buf, &type) &&
                type == DEVPROP_TYPE_STRING) {
                node.name = reinterpret_cast<const wchar_t*>(buf.data());
            }
            if (GetDevProp(set, &data, kIsConnectedKey, buf, &type) &&
                type == DEVPROP_TYPE_BOOLEAN && !buf.empty()) {
                node.connected = buf[0] != 0;
            }
            if ((GetDevProp(set, &data, kClassOfDeviceKey, buf, &type) ||
                 GetDevProp(set, &data, kClassOfDeviceAltKey, buf, &type)) &&
                type == DEVPROP_TYPE_UINT32 && buf.size() >= 4) {
                node.cod = *reinterpret_cast<const DWORD*>(buf.data());
            }
            deviceNodes.emplace_back(ExtractBluetoothAddress(id),
                                     std::move(node));
        }

        DEVPROPTYPE type = 0;
        if (GetDevProp(set, &data, kBatteryLevelKey, buf, &type) &&
            type == DEVPROP_TYPE_BYTE && !buf.empty() && buf[0] <= 100) {
            std::wstring address = ExtractBluetoothAddress(id);
            if (!address.empty()) {
                batteries.emplace_back(address, (int)buf[0]);
            }
        }
    }
    SetupDiDestroyDeviceInfoList(set);

    std::vector<BluetoothReading> readings;
    for (const auto& [address, percent] : batteries) {
        if (std::any_of(readings.begin(), readings.end(),
                        [&](const BluetoothReading& r) {
                            return r.address == address;
                        })) {
            continue;
        }
        auto it = std::find_if(
            deviceNodes.begin(), deviceNodes.end(),
            [&](const auto& n) { return n.first == address; });
        if (it == deviceNodes.end() || !it->second.connected) {
            continue;
        }
        BluetoothReading r;
        r.address = address;
        r.name = it->second.name.empty() ? L"Bluetooth device" : it->second.name;
        r.kind = KindFromClassOfDevice(it->second.cod, r.name);
        r.percent = percent;
        readings.push_back(std::move(r));
    }

    if (connectedAddresses) {
        connectedAddresses->clear();
        for (const auto& [address, node] : deviceNodes) {
            if (node.connected && !address.empty()) {
                connectedAddresses->push_back(address);
            }
        }
    }
    return readings;
}

struct BluetoothCacheEntry {
    BluetoothReading reading;
    ULONGLONG lastSeen;
};

std::vector<BluetoothCacheEntry> g_bluetoothCache;

// Some headsets close the profile that reports the battery while idle, so the
// level disappears for a while although the device is still connected. Keep
// the last level until the device actually disconnects.
void UpdateBluetoothCache(const std::vector<BluetoothReading>& readings,
                          const std::vector<std::wstring>& connected) {
    ULONGLONG now = GetTickCount64();
    for (const auto& r : readings) {
        auto it = std::find_if(g_bluetoothCache.begin(), g_bluetoothCache.end(),
                               [&](const BluetoothCacheEntry& e) {
                                   return e.reading.address == r.address;
                               });
        if (it != g_bluetoothCache.end()) {
            it->reading = r;
            it->lastSeen = now;
        } else {
            g_bluetoothCache.push_back({r, now});
        }
    }

    constexpr ULONGLONG kMaxAgeMs = 30 * 60 * 1000;
    g_bluetoothCache.erase(
        std::remove_if(g_bluetoothCache.begin(), g_bluetoothCache.end(),
                       [&](const BluetoothCacheEntry& e) {
                           bool stillConnected =
                               std::find(connected.begin(), connected.end(),
                                         e.reading.address) != connected.end();
                           bool remove =
                               !stillConnected || now - e.lastSeen > kMaxAgeMs;
                           if (remove) {
                               Wh_Log(L"Bluetooth %s removed (connected=%d)",
                                      e.reading.name.c_str(), stillConnected);
                           }
                           return remove;
                       }),
        g_bluetoothCache.end());
}

#pragma endregion

#pragma region playstation

struct PlayStationReading {
    std::wstring key;
    std::wstring name;
    int percent;
    bool charging;
};

PCWSTR PlayStationModelName(USHORT pid, bool* isDualSense) {
    *isDualSense = false;
    switch (pid) {
        case 0x0CE6:
            *isDualSense = true;
            return L"DualSense";
        case 0x0DF2:
            *isDualSense = true;
            return L"DualSense Edge";
        case 0x05C4:
        case 0x09CC:
            return L"DualShock 4";
        case 0x0BA0:
            return L"DualShock 4 (Wireless Adapter)";
    }
    return nullptr;
}

bool ReadInputReport(HANDLE h, std::vector<BYTE>& buf, DWORD timeoutMs) {
    OVERLAPPED ov{};
    ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) {
        return false;
    }
    DWORD read = 0;
    bool ok = ReadFile(h, buf.data(), (DWORD)buf.size(), &read, &ov);
    if (!ok && GetLastError() == ERROR_IO_PENDING) {
        HANDLE handles[] = {ov.hEvent, g_stopEvent};
        DWORD wait = WaitForMultipleObjects(2, handles, FALSE, timeoutMs);
        if (wait == WAIT_OBJECT_0) {
            ok = GetOverlappedResult(h, &ov, &read, FALSE);
        } else {
            CancelIoEx(h, &ov);
            GetOverlappedResult(h, &ov, &read, TRUE);
            ok = false;
        }
    }
    CloseHandle(ov.hEvent);
    if (ok) {
        buf.resize(read);
    }
    return ok && read > 0;
}

bool ParsePlayStationReport(const std::vector<BYTE>& r,
                            bool isDualSense,
                            bool usb,
                            int* percent,
                            bool* charging) {
    if (r.empty()) {
        return false;
    }
    if (isDualSense) {
        size_t offset = 0;
        if (usb && r[0] == 0x01 && r.size() >= 54) {
            offset = 53;
        } else if (!usb && r[0] == 0x31 && r.size() >= 55) {
            offset = 54;
        } else {
            return false;
        }
        BYTE status = r[offset];
        int level = status & 0x0F;
        int state = status >> 4;
        if (state > 2) {
            return false;
        }
        *percent = std::min(100, level * 10 + 5);
        if (state == 2) {
            *percent = 100;
        }
        *charging = state == 1;
        return true;
    }

    size_t offset = 0;
    if (usb && r[0] == 0x01 && r.size() >= 31) {
        offset = 30;
    } else if (!usb && r[0] == 0x11 && r.size() >= 33) {
        offset = 32;
    } else {
        return false;
    }
    BYTE status = r[offset];
    int level = status & 0x0F;
    bool cable = (status & 0x10) != 0;
    int max = cable ? 11 : 8;
    *percent = std::min(100, level * 100 / max);
    *charging = cable && level < 11;
    return true;
}

std::vector<PlayStationReading> PollPlayStation() {
    std::vector<PlayStationReading> readings;

    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO set = SetupDiGetClassDevsW(&hidGuid, nullptr, nullptr,
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (set == INVALID_HANDLE_VALUE) {
        return readings;
    }

    SP_DEVICE_INTERFACE_DATA ifData{sizeof(ifData)};
    for (DWORD i = 0;
         SetupDiEnumDeviceInterfaces(set, nullptr, &hidGuid, i, &ifData);
         i++) {
        DWORD needed = 0;
        SetupDiGetDeviceInterfaceDetailW(set, &ifData, nullptr, 0, &needed,
                                         nullptr);
        if (!needed) {
            continue;
        }
        std::vector<BYTE> detailBuf(needed);
        auto detail =
            reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(detailBuf.data());
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (!SetupDiGetDeviceInterfaceDetailW(set, &ifData, detail, needed,
                                              nullptr, nullptr)) {
            continue;
        }
        std::wstring path = ToLower(detail->DevicePath);
        if (path.find(L"vid_054c") == std::wstring::npos &&
            path.find(L"054c_pid") == std::wstring::npos) {
            continue;
        }

        HANDLE h = CreateFileW(detail->DevicePath, GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                               OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }

        HIDD_ATTRIBUTES attrs{sizeof(attrs)};
        HIDP_CAPS caps{};
        PHIDP_PREPARSED_DATA pp = nullptr;
        bool capsOk = false;
        if (HidD_GetAttributes(h, &attrs) && HidD_GetPreparsedData(h, &pp)) {
            capsOk = HidP_GetCaps(pp, &caps) == HIDP_STATUS_SUCCESS;
            HidD_FreePreparsedData(pp);
        }

        bool isDualSense = false;
        PCWSTR model = capsOk ? PlayStationModelName(attrs.ProductID, &isDualSense)
                              : nullptr;
        if (model && caps.UsagePage == 0x01 && caps.Usage == 0x05 &&
            caps.InputReportByteLength > 0) {
            bool usb = caps.InputReportByteLength == 64;
            int percent = -1;
            bool charging = false;
            bool parsed = false;
            for (int attempt = 0; attempt < 4 && !parsed; attempt++) {
                std::vector<BYTE> report(caps.InputReportByteLength);
                if (!ReadInputReport(h, report, 250)) {
                    break;
                }
                parsed = ParsePlayStationReport(report, isDualSense, usb,
                                                &percent, &charging);
            }
            static std::wstring lastLogged;
            WCHAR logLine[160];
            _snwprintf_s(logLine, _TRUNCATE,
                         L"PlayStation pid=%04X usb=%d inputLen=%u parsed=%d "
                         L"percent=%d charging=%d",
                         attrs.ProductID, usb, caps.InputReportByteLength,
                         parsed, percent, charging);
            if (lastLogged != logLine) {
                Wh_Log(L"%s", logLine);
                lastLogged = logLine;
            }
            if (parsed) {
                PlayStationReading r;
                r.key = L"ps:" + path;
                r.name = model;
                r.percent = percent;
                r.charging = charging;
                readings.push_back(std::move(r));
            }
        }

        CloseHandle(h);
    }

    SetupDiDestroyDeviceInfoList(set);
    return readings;
}

#pragma endregion

#pragma region logitech

struct LogitechReading {
    std::wstring key;
    std::wstring name;
    DeviceKind kind;
    int percent;
    bool charging;
    bool approximate;
};

bool WriteOutputReport(HANDLE h, const BYTE* data, DWORD size) {
    OVERLAPPED ov{};
    ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) {
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(h, data, size, &written, &ov);
    if (!ok && GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(ov.hEvent, 500) == WAIT_OBJECT_0) {
            ok = GetOverlappedResult(h, &ov, &written, FALSE);
        } else {
            CancelIoEx(h, &ov);
            GetOverlappedResult(h, &ov, &written, TRUE);
            ok = false;
        }
    }
    CloseHandle(ov.hEvent);
    return ok;
}

constexpr BYTE kHidppSoftwareId = 0x0A;

bool HidppRequest(HANDLE h,
                  DWORD inputLength,
                  BYTE deviceIndex,
                  BYTE featureIndex,
                  BYTE function,
                  std::initializer_list<BYTE> params,
                  BYTE response[20]) {
    BYTE request[20]{};
    request[0] = 0x11;
    request[1] = deviceIndex;
    request[2] = featureIndex;
    request[3] = (BYTE)((function << 4) | kHidppSoftwareId);
    size_t i = 4;
    for (BYTE p : params) {
        if (i < sizeof(request)) {
            request[i++] = p;
        }
    }
    if (!WriteOutputReport(h, request, sizeof(request))) {
        return false;
    }

    ULONGLONG deadline = GetTickCount64() + 400;
    while (GetTickCount64() < deadline) {
        std::vector<BYTE> buf(std::max<DWORD>(inputLength, 20));
        DWORD remaining = (DWORD)(deadline - GetTickCount64());
        if (!ReadInputReport(h, buf, std::max<DWORD>(remaining, 1))) {
            return false;
        }
        if (buf.size() < 20 || buf[0] != 0x11 || buf[1] != deviceIndex) {
            continue;
        }
        if (buf[2] == 0xFF && buf[3] == featureIndex) {
            return false;
        }
        if (buf[2] == featureIndex && buf[3] == request[3]) {
            memcpy(response, buf.data(), 20);
            return true;
        }
    }
    return false;
}

BYTE HidppFeatureIndex(HANDLE h, DWORD inputLength, BYTE deviceIndex, WORD featureId) {
    BYTE r[20];
    if (!HidppRequest(h, inputLength, deviceIndex, 0x00, 0,
                      {(BYTE)(featureId >> 8), (BYTE)(featureId & 0xFF)}, r)) {
        return 0;
    }
    return r[4];
}

bool ReadLogitechDevice(HANDLE h,
                        DWORD inputLength,
                        BYTE deviceIndex,
                        LogitechReading& out) {
    BYTE r[20];
    out.percent = -1;
    out.charging = false;
    out.approximate = false;

    if (BYTE idx = HidppFeatureIndex(h, inputLength, deviceIndex, 0x1004)) {
        if (HidppRequest(h, inputLength, deviceIndex, idx, 1, {}, r)) {
            int soc = r[4];
            BYTE levels = r[5];
            BYTE status = r[6];
            if (soc > 0 && soc <= 100) {
                out.percent = soc;
            } else if (levels) {
                out.percent = (levels & 0x08) ? 100
                              : (levels & 0x04) ? 60
                              : (levels & 0x02) ? 20
                                                : 5;
                out.approximate = true;
            }
            out.charging = status == 1 || status == 2;
            if (status == 3) {
                out.percent = 100;
            }
        }
    } else if (BYTE idx = HidppFeatureIndex(h, inputLength, deviceIndex, 0x1000)) {
        if (HidppRequest(h, inputLength, deviceIndex, idx, 0, {}, r)) {
            if (r[4] <= 100) {
                out.percent = r[4];
            }
            BYTE status = r[6];
            out.charging = status == 1 || status == 2 || status == 4;
            if (status == 3) {
                out.percent = 100;
            }
        }
    } else if (BYTE idx = HidppFeatureIndex(h, inputLength, deviceIndex, 0x1001)) {
        if (HidppRequest(h, inputLength, deviceIndex, idx, 0, {}, r)) {
            int millivolts = (r[4] << 8) | r[5];
            if (millivolts > 0) {
                out.percent = std::clamp((millivolts - 3500) * 100 / 650, 0, 100);
                out.approximate = true;
            }
            out.charging = (r[6] & 0x80) != 0;
        }
    } else {
        return false;
    }

    if (out.percent < 0) {
        return false;
    }

    out.kind = DeviceKind::Generic;
    out.name = L"Logitech device";
    if (BYTE idx = HidppFeatureIndex(h, inputLength, deviceIndex, 0x0005)) {
        if (HidppRequest(h, inputLength, deviceIndex, idx, 0, {}, r)) {
            int length = std::min<int>(r[4], 64);
            std::string name;
            while ((int)name.size() < length) {
                if (!HidppRequest(h, inputLength, deviceIndex, idx, 1,
                                  {(BYTE)name.size()}, r)) {
                    break;
                }
                for (int i = 4; i < 20 && (int)name.size() < length; i++) {
                    if (!r[i]) {
                        break;
                    }
                    name.push_back((char)r[i]);
                }
                if (r[4] == 0) {
                    break;
                }
            }
            if (!name.empty()) {
                out.name = L"Logitech " + std::wstring(name.begin(), name.end());
            }
        }
        if (HidppRequest(h, inputLength, deviceIndex, idx, 2, {}, r)) {
            switch (r[4]) {
                case 0:
                case 2:
                    out.kind = DeviceKind::Keyboard;
                    break;
                case 3:
                case 4:
                case 5:
                    out.kind = DeviceKind::Mouse;
                    break;
                case 8:
                    out.kind = DeviceKind::Headphones;
                    break;
                case 11:
                case 12:
                    out.kind = DeviceKind::Gamepad;
                    break;
            }
        }
    }
    return true;
}

std::vector<LogitechReading> PollLogitech() {
    std::vector<LogitechReading> readings;

    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO set = SetupDiGetClassDevsW(&hidGuid, nullptr, nullptr,
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (set == INVALID_HANDLE_VALUE) {
        return readings;
    }

    SP_DEVICE_INTERFACE_DATA ifData{sizeof(ifData)};
    for (DWORD i = 0;
         SetupDiEnumDeviceInterfaces(set, nullptr, &hidGuid, i, &ifData);
         i++) {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            break;
        }
        DWORD needed = 0;
        SetupDiGetDeviceInterfaceDetailW(set, &ifData, nullptr, 0, &needed,
                                         nullptr);
        if (!needed) {
            continue;
        }
        std::vector<BYTE> detailBuf(needed);
        auto detail =
            reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(detailBuf.data());
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (!SetupDiGetDeviceInterfaceDetailW(set, &ifData, detail, needed,
                                              nullptr, nullptr)) {
            continue;
        }
        std::wstring path = ToLower(detail->DevicePath);
        if (path.find(L"vid_046d") == std::wstring::npos) {
            continue;
        }

        HANDLE h = CreateFileW(detail->DevicePath, GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                               OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }

        HIDP_CAPS caps{};
        PHIDP_PREPARSED_DATA pp = nullptr;
        bool capsOk = false;
        if (HidD_GetPreparsedData(h, &pp)) {
            capsOk = HidP_GetCaps(pp, &caps) == HIDP_STATUS_SUCCESS;
            HidD_FreePreparsedData(pp);
        }

        if (capsOk && caps.UsagePage == 0xFF00 && caps.Usage == 0x0002 &&
            caps.OutputReportByteLength >= 20) {
            const BYTE deviceIndexes[] = {0xFF, 1, 2, 3, 4, 5, 6};
            for (BYTE deviceIndex : deviceIndexes) {
                LogitechReading reading;
                if (ReadLogitechDevice(h, caps.InputReportByteLength,
                                       deviceIndex, reading)) {
                    reading.key = L"logi:" + path + L":" +
                                  std::to_wstring(deviceIndex);
                    Wh_Log(L"Logitech %s index=%u percent=%d charging=%d",
                           reading.name.c_str(), deviceIndex, reading.percent,
                           reading.charging);
                    readings.push_back(std::move(reading));
                    if (deviceIndex == 0xFF) {
                        break;
                    }
                }
            }
        }

        CloseHandle(h);
    }

    SetupDiDestroyDeviceInfoList(set);
    return readings;
}

#pragma endregion

#pragma region gaming_input
struct GamingInputBattery {
    int percent;
    bool charging;
};

std::vector<GamingInputBattery> PollGamingInputBatteries() {
    std::vector<GamingInputBattery> result;
    try {
        auto gamepads = winrt::Windows::Gaming::Input::Gamepad::Gamepads();
        for (auto const& gamepad : gamepads) {
            auto report = gamepad.TryGetBatteryReport();
            if (!report) {
                continue;
            }
            auto status = report.Status();
            if (status == winrt::Windows::System::Power::BatteryStatus::NotPresent) {
                continue;
            }
            auto full = report.FullChargeCapacityInMilliwattHours();
            auto remaining = report.RemainingCapacityInMilliwattHours();
            if (!full || !remaining || full.Value() <= 0) {
                continue;
            }
            int percent = std::clamp(remaining.Value() * 100 / full.Value(), 0, 100);
            result.push_back(
                {percent,
                 status == winrt::Windows::System::Power::BatteryStatus::Charging});
        }
    } catch (...) {
        Wh_Log(L"Windows.Gaming.Input failed: %08X", winrt::to_hresult().value);
    }
    return result;
}

#pragma endregion

#pragma region polling
std::vector<DeviceState> g_razerCache;

void PublishDevices(std::vector<DeviceState> devices) {
    std::sort(devices.begin(), devices.end(),
              [](const DeviceState& a, const DeviceState& b) {
                  return a.key < b.key;
              });

    std::lock_guard<std::mutex> lock(g_devicesMutex);
    bool same = devices.size() == g_devices.size();
    for (size_t i = 0; same && i < devices.size(); i++) {
        same = devices[i].SameDisplay(g_devices[i]);
    }
    if (!same) {
        Wh_Log(L"Devices changed (%zu)", devices.size());
        for (const auto& d : devices) {
            Wh_Log(L"  %s: percent=%d approx=%d charging=%d wired=%d stale=%d",
                   d.name.c_str(), d.percent, d.approximate, d.charging,
                   d.wiredNoBattery, d.stale);
        }
        g_devices = std::move(devices);
        g_dataVersion++;
    }
}

std::wstring CurrentTimeString() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR buf[64];
    if (!GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr,
                         buf, ARRAYSIZE(buf))) {
        return L"";
    }
    return buf;
}

void UpdateRazerCache(const std::vector<RazerReading>& readings,
                      bool hideDisconnected) {
    ULONGLONG now = GetTickCount64();
    std::vector<std::wstring> seenNames;

    for (const auto& r : readings) {
        Wh_Log(L"Razer pid=%04X wired=%d ok=%d percent=%d charging=%d", r.pid,
               r.wired, r.ok, r.percent, r.charging);
    }

    for (const auto& r : readings) {
        if (std::find(seenNames.begin(), seenNames.end(), r.name) !=
            seenNames.end()) {
            continue;
        }

        const RazerReading* best = nullptr;
        for (const auto& c : readings) {
            if (c.name != r.name || !c.ok) {
                continue;
            }
            if (!best || (c.wired && !best->wired)) {
                best = &c;
            }
        }

        if (!best) {
            continue;
        }
        seenNames.push_back(r.name);

        std::wstring key = L"razer:" + r.name;
        auto it = std::find_if(g_razerCache.begin(), g_razerCache.end(),
                               [&](const DeviceState& d) { return d.key == key; });
        DeviceState state;
        state.key = key;
        state.name = r.name;
        state.kind = best->kind;
        state.percent = best->percent;
        state.precisePercent = best->precisePercent;
        state.charging = false;
        for (const auto& c : readings) {
            if (c.name == r.name && c.ok &&
                (c.charging || (c.wired && c.percent < 100))) {
                state.charging = true;
            }
        }
        state.lastSeen = now;
        state.updated = CurrentTimeString();
        if (it != g_razerCache.end()) {
            *it = state;
        } else {
            g_razerCache.push_back(state);
        }
    }

    constexpr ULONGLONG kGraceMs = 10 * 60 * 1000;
    for (auto it = g_razerCache.begin(); it != g_razerCache.end();) {
        bool seen = std::find(seenNames.begin(), seenNames.end(), it->name) !=
                    seenNames.end();
        if (seen) {
            it->stale = false;
            ++it;
            continue;
        }
        if (now - it->lastSeen < kGraceMs) {
            it->stale = false;
            ++it;
            continue;
        }
        if (hideDisconnected) {
            it = g_razerCache.erase(it);
        } else {
            it->stale = true;
            it->charging = false;
            ++it;
        }
    }
}

constexpr int kEstimating = -2;

struct LevelHistory {
    std::wstring key;
    bool charging;
    double anchorPercent;
    long long anchorTime;
    double lastPercent;
};

std::vector<LevelHistory> g_levelHistory;
bool g_levelHistoryLoaded = false;

long long UnixSecondsNow() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER u{{ft.dwLowDateTime, ft.dwHighDateTime}};
    return (long long)(u.QuadPart / 10000000ULL) - 11644473600LL;
}

void LoadLevelHistory() {
    g_levelHistoryLoaded = true;
    WCHAR buf[4096];
    if (!Wh_GetStringValue(L"levelHistory", buf, ARRAYSIZE(buf))) {
        return;
    }
    long long now = UnixSecondsNow();
    std::wstring all = buf;
    size_t pos = 0;
    while (pos < all.size()) {
        size_t end = all.find(L'\n', pos);
        if (end == std::wstring::npos) {
            end = all.size();
        }
        std::wstring line = all.substr(pos, end - pos);
        pos = end + 1;
        size_t a = line.rfind(L'|');
        size_t b = a == std::wstring::npos ? a : line.rfind(L'|', a - 1);
        size_t c = b == std::wstring::npos ? b : line.rfind(L'|', b - 1);
        if (c == std::wstring::npos) {
            continue;
        }
        LevelHistory h;
        h.key = line.substr(0, c);
        h.charging = line.substr(c + 1, b - c - 1) == L"1";
        h.anchorPercent = _wtof(line.substr(b + 1, a - b - 1).c_str());
        h.anchorTime = _wtoi64(line.substr(a + 1).c_str());
        h.lastPercent = h.anchorPercent;
        if (now - h.anchorTime < 24 * 3600 && now >= h.anchorTime) {
            g_levelHistory.push_back(h);
        }
    }
}

void SaveLevelHistory() {
    std::wstring all;
    for (const auto& h : g_levelHistory) {
        WCHAR line[600];
        _snwprintf_s(line, _TRUNCATE, L"%s|%d|%.2f|%lld\n", h.key.c_str(),
                     h.charging ? 1 : 0, h.anchorPercent, h.anchorTime);
        if (all.size() + wcslen(line) >= 4000) {
            break;
        }
        all += line;
    }
    Wh_SetStringValue(L"levelHistory", all.c_str());
}

int RoundEstimate(int minutes) {
    if (minutes < 60) {
        return std::max(5, (minutes + 2) / 5 * 5);
    }
    if (minutes < 600) {
        return (minutes + 7) / 15 * 15;
    }
    return (minutes + 30) / 60 * 60;
}

// Estimates the time until empty (or until full while charging) from the
// level change since the last time the charging state changed. The anchor is
// saved, so the estimate survives mod and Explorer restarts.
void ApplyEstimates(std::vector<DeviceState>& devices) {
    if (!g_levelHistoryLoaded) {
        LoadLevelHistory();
    }
    long long now = UnixSecondsNow();
    bool changed = false;
    for (auto& d : devices) {
        if (d.percent < 0 || d.approximate || d.wiredNoBattery || d.stale) {
            continue;
        }
        double level = d.precisePercent >= 0 ? d.precisePercent : d.percent;
        auto it = std::find_if(g_levelHistory.begin(), g_levelHistory.end(),
                               [&](const LevelHistory& h) { return h.key == d.key; });
        if (it == g_levelHistory.end()) {
            g_levelHistory.push_back({d.key, d.charging, level, now, level});
            d.estimateMinutes = kEstimating;
            changed = true;
            continue;
        }
        bool wrongDirection = d.charging ? level < it->lastPercent - 0.5
                                         : level > it->lastPercent + 0.5;
        if (it->charging != d.charging || wrongDirection) {
            *it = {d.key, d.charging, level, now, level};
            d.estimateMinutes = kEstimating;
            changed = true;
            continue;
        }
        it->lastPercent = level;

        double delta = std::abs(level - it->anchorPercent);
        double elapsedMinutes = (now - it->anchorTime) / 60.0;
        d.estimateMinutes = kEstimating;
        if (delta < 1.0 || elapsedMinutes < 10) {
            continue;
        }
        double minutesPerPercent = elapsedMinutes / delta;
        double remaining = d.charging ? 100 - level : level;
        double estimate = remaining * minutesPerPercent;
        if (estimate <= 0 || estimate > 99 * 60) {
            continue;
        }
        d.estimateMinutes = RoundEstimate((int)estimate);
    }
    if (changed) {
        SaveLevelHistory();
    }
}
std::wstring FormatDuration(int minutes) {
    if (minutes < 60) {
        return std::to_wstring(minutes) + L" min";
    }
    int hours = minutes / 60;
    int rest = minutes % 60;
    if (hours >= 10 || rest == 0) {
        return std::to_wstring(hours) + L" h";
    }
    return std::to_wstring(hours) + L" h " + std::to_wstring(rest) + L" min";
}

std::wstring EstimateText(const DeviceState& d) {
    if (d.estimateMinutes == kEstimating) {
        return L"Estimating time remaining\u2026";
    }
    if (d.estimateMinutes < 0) {
        return L"";
    }
    return d.charging ? L"Full in about " + FormatDuration(d.estimateMinutes)
                      : L"About " + FormatDuration(d.estimateMinutes) + L" left";
}

DWORD WINAPI PollThreadProc(LPVOID) {    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    } catch (...) {
    }

    ULONGLONG lastRazerPoll = 0;
    ULONGLONG lastBluetoothPoll = 0;
    std::vector<PlayStationReading> playStation;
    std::vector<LogitechReading> logitech;
    bool firstRun = true;

    while (true) {
        Settings settings = GetSettingsCopy();
        ULONGLONG now = GetTickCount64();

        if (firstRun ||
            now - lastRazerPoll >= (ULONGLONG)settings.pollInterval * 1000) {
            auto razer = PollRazer();
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                break;
            }
            UpdateRazerCache(razer, settings.hideDisconnected);
            logitech = PollLogitech();
            lastRazerPoll = now;
        }

        if (firstRun || now - lastBluetoothPoll >= 15000) {
            std::vector<std::wstring> connectedBluetooth;
            auto bluetooth = PollBluetooth(&connectedBluetooth);
            UpdateBluetoothCache(bluetooth, connectedBluetooth);
            playStation = PollPlayStation();
            lastBluetoothPoll = now;
        }
        firstRun = false;

        std::vector<DeviceState> devices = g_razerCache;

        for (const auto& l : logitech) {
            DeviceState state;
            state.key = l.key;
            state.name = l.name;
            state.kind = l.kind;
            state.percent = l.percent;
            state.approximate = l.approximate;
            state.charging = l.charging;
            state.lastSeen = now;
            devices.push_back(std::move(state));
        }

        for (const auto& p : playStation) {
            DeviceState state;
            state.key = p.key;
            state.name = p.name;
            state.kind = DeviceKind::Gamepad;
            state.percent = p.percent;
            state.charging = p.charging;
            state.lastSeen = now;
            devices.push_back(std::move(state));
        }

        for (const auto& entry : g_bluetoothCache) {
            const auto& b = entry.reading;
            DeviceState state;
            state.key = L"bt:" + b.address;
            state.name = b.name;
            state.kind = b.kind;
            state.percent = b.percent;
            state.lastSeen = now;
            devices.push_back(std::move(state));
        }

        auto xinputReadings = PollXInput();
        auto gamingInputBatteries = PollGamingInputBatteries();
        size_t nextGamingInput = 0;
        for (const auto& x : xinputReadings) {
            DeviceState state;
            state.key = L"xinput:" + std::to_wstring(x.index);
            state.name = x.index == 0
                             ? std::wstring(L"Xbox Controller")
                             : L"Xbox Controller " + std::to_wstring(x.index + 1);
            state.kind = DeviceKind::Gamepad;
            state.percent = x.percent;
            state.approximate = x.percent >= 0;
            state.wiredNoBattery = x.wiredNoBattery;
            if (!x.wiredNoBattery &&
                nextGamingInput < gamingInputBatteries.size()) {
                const auto& exact = gamingInputBatteries[nextGamingInput++];
                state.percent = exact.percent;
                state.charging = exact.charging;
                state.approximate = false;
            }
            state.lastSeen = now;
            devices.push_back(std::move(state));
        }

        ApplyEstimates(devices);
        PublishDevices(std::move(devices));

        HANDLE handles[] = {g_stopEvent, g_refreshEvent};
        DWORD wait = WaitForMultipleObjects(2, handles, FALSE, 5000);
        if (wait == WAIT_OBJECT_0) {
            break;
        }
        if (wait == WAIT_OBJECT_0 + 1) {
            firstRun = true;
        }
    }

    UnloadXInput();
    if (apartmentInitialized) {
        winrt::uninit_apartment();
    }
    return 0;
}

#pragma endregion

#pragma region ui

std::optional<winrt::Windows::UI::Color> ParseColor(std::wstring s) {
    s.erase(std::remove_if(s.begin(), s.end(), iswspace), s.end());
    if (s.empty() || s[0] != L'#') {
        return std::nullopt;
    }
    s = s.substr(1);
    if (s.size() != 6 && s.size() != 8) {
        return std::nullopt;
    }
    for (wchar_t c : s) {
        if (!iswxdigit(c)) {
            return std::nullopt;
        }
    }
    unsigned long v = wcstoul(s.c_str(), nullptr, 16);
    winrt::Windows::UI::Color c{};
    if (s.size() == 6) {
        c.A = 0xFF;
        c.R = (v >> 16) & 0xFF;
        c.G = (v >> 8) & 0xFF;
        c.B = v & 0xFF;
    } else {
        c.A = (v >> 24) & 0xFF;
        c.R = (v >> 16) & 0xFF;
        c.G = (v >> 8) & 0xFF;
        c.B = v & 0xFF;
    }
    return c;
}

const DeviceOverride* FindOverride(const Settings& settings,
                                   const DeviceState& device) {
    std::wstring lowerName = ToLower(device.name);
    for (const auto& o : settings.devices) {
        if (lowerName.find(o.match) != std::wstring::npos) {
            return &o;
        }
    }
    return nullptr;
}

wuxm::ImageSource LoadImageFromFile(const std::wstring& path) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Cannot open icon %s", path.c_str());
        return nullptr;
    }
    LARGE_INTEGER size{};
    std::vector<BYTE> bytes;
    if (GetFileSizeEx(f, &size) && size.QuadPart > 0 &&
        size.QuadPart < 8 * 1024 * 1024) {
        bytes.resize((size_t)size.QuadPart);
        DWORD read = 0;
        if (!ReadFile(f, bytes.data(), (DWORD)bytes.size(), &read, nullptr) ||
            read != bytes.size()) {
            bytes.clear();
        }
    }
    CloseHandle(f);
    if (bytes.empty()) {
        return nullptr;
    }

    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream(bytes.data(), (UINT)bytes.size()));
    if (!stream) {
        return nullptr;
    }
    winrt::Windows::Storage::Streams::IRandomAccessStream ras{nullptr};
    if (FAILED(CreateRandomAccessStreamOverStream(
            stream.get(), BSOS_DEFAULT,
            winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(),
            winrt::put_abi(ras)))) {
        return nullptr;
    }

    std::wstring lower = ToLower(path);
    if (lower.size() >= 4 && lower.compare(lower.size() - 4, 4, L".svg") == 0) {
        wuxm::Imaging::SvgImageSource svg;
        svg.SetSourceAsync(ras);
        return svg;
    }
    wuxm::Imaging::BitmapImage bitmap;
    bitmap.SetSource(ras);
    return bitmap;
}

wuxc::FontIcon MakeGlyph(PCWSTR glyph, double size) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    icon.Glyph(glyph);
    icon.FontSize(size);
    icon.VerticalAlignment(wux::VerticalAlignment::Center);
    return icon;
}

wux::UIElement BuildDeviceItem(const Settings& settings,
                               const DeviceState& device,
                               const DeviceOverride* override,
                               wuxm::Brush const& defaultBrush) {
    wuxc::StackPanel item;
    item.Orientation(wuxc::Orientation::Horizontal);
    item.VerticalAlignment(wux::VerticalAlignment::Center);
    item.Spacing(3);
    item.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));

    std::wstring name = device.name;
    if (override && !override->displayName.empty()) {
        name = override->displayName;
    }

    bool low = device.percent >= 0 &&
               device.percent <= settings.lowBatteryThreshold &&
               !device.charging;

    wuxm::Brush textBrush{nullptr};
    if (auto c = ParseColor(settings.textColor)) {
        textBrush = wuxm::SolidColorBrush(*c);
    }
    wuxm::Brush lowBrush{nullptr};
    if (low) {
        auto c = ParseColor(settings.lowBatteryColor);
        lowBrush = wuxm::SolidColorBrush(
            c ? *c : winrt::Windows::UI::Color{0xFF, 0xE8, 0x11, 0x23});
    }
    wuxm::Brush effectiveBrush = lowBrush ? lowBrush : textBrush;

    auto applyForeground = [&](wuxc::FontIcon const& icon) {
        if (effectiveBrush) {
            icon.Foreground(effectiveBrush);
        }
    };

    bool showIcon = settings.displayMode != DisplayMode::PercentOnly;
    bool showText = settings.displayMode == DisplayMode::IconAndPercent ||
                    settings.displayMode == DisplayMode::PercentOnly;
    bool showBar = settings.displayMode == DisplayMode::BatteryBar;

    if (showIcon) {
        wux::UIElement iconElement{nullptr};
        if (override && !override->customIcon.empty()) {
            if (auto source = LoadImageFromFile(override->customIcon)) {
                wuxc::Image image;
                image.Source(source);
                image.Width(settings.iconSize);
                image.Height(settings.iconSize);
                image.VerticalAlignment(wux::VerticalAlignment::Center);
                iconElement = image;
            }
        }
        if (!iconElement) {
            PCWSTR glyphCode = L"\uE702";
            switch (device.kind) {
                case DeviceKind::Mouse:
                    glyphCode = L"\uE962";
                    break;
                case DeviceKind::Gamepad:
                    glyphCode = L"\uE7FC";
                    break;
                case DeviceKind::Headphones:
                    glyphCode = L"\uE7F6";
                    break;
                case DeviceKind::Keyboard:
                    glyphCode = L"\uE765";
                    break;
                case DeviceKind::Generic:
                    break;
            }
            auto glyph = MakeGlyph(glyphCode, settings.iconSize);
            applyForeground(glyph);
            iconElement = glyph;
        }
        item.Children().Append(iconElement);
    }

    if (device.percent >= 0 && showText) {
        wuxc::TextBlock text;
        std::wstring value = std::to_wstring(device.percent) + L"%";
        if (device.approximate) {
            value = L"~" + value;
        }
        if (settings.timeEstimate == TimeEstimateMode::Inline &&
            device.estimateMinutes >= 0) {
            value += L" " + FormatDuration(device.estimateMinutes);
        }
        text.Text(value);
        text.FontFamily(wuxm::FontFamily(settings.fontFamily));
        text.FontSize(settings.fontSize);
        if (settings.bold) {
            text.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        }
        text.VerticalAlignment(wux::VerticalAlignment::Center);
        if (effectiveBrush) {
            text.Foreground(effectiveBrush);
        }
        item.Children().Append(text);
    }

    if (device.percent >= 0 && showBar) {
        wuxm::Brush barBrush = effectiveBrush ? effectiveBrush : defaultBrush;
        double barWidth = std::max(14.0, settings.iconSize * 1.4);
        double barHeight = std::max(7.0, settings.iconSize * 0.65);

        wuxc::Border outer;
        outer.Width(barWidth);
        outer.Height(barHeight);
        outer.BorderThickness(wux::Thickness{1, 1, 1, 1});
        outer.CornerRadius(wux::CornerRadius{2, 2, 2, 2});
        outer.Padding(wux::Thickness{1, 1, 1, 1});
        outer.VerticalAlignment(wux::VerticalAlignment::Center);
        if (barBrush) {
            outer.BorderBrush(barBrush);
        }

        wuxc::Border fill;
        fill.HorizontalAlignment(wux::HorizontalAlignment::Left);
        fill.Width(std::max(1.0, (barWidth - 4) * device.percent / 100.0));
        fill.CornerRadius(wux::CornerRadius{1, 1, 1, 1});
        if (barBrush) {
            fill.Background(barBrush);
        }
        outer.Child(fill);
        item.Children().Append(outer);
    }

    if (device.wiredNoBattery) {
        auto plug = MakeGlyph(L"\uECF0", settings.iconSize * 0.8);
        applyForeground(plug);
        item.Children().Append(plug);
    } else if (device.charging && settings.showChargingIndicator) {
        auto bolt = MakeGlyph(L"\uE945", settings.iconSize * 0.75);
        applyForeground(bolt);
        item.Children().Append(bolt);
    }

    if (device.stale) {
        item.Opacity(0.5);
    }

    std::wstring tooltip = name;
    if (device.wiredNoBattery) {
        tooltip += L"\nWired (no battery)";
    } else if (device.percent >= 0) {
        tooltip += L"\nBattery: ";
        if (device.approximate) {
            tooltip += L"about ";
        }
        tooltip += std::to_wstring(device.percent) + L"%";
        if (device.charging) {
            tooltip += L" (charging)";
        }
    } else {
        tooltip += L"\nBattery level unavailable";
    }
    if (device.stale) {
        tooltip += L"\nNot responding, showing last known level";
    }
    if (settings.timeEstimate != TimeEstimateMode::Off &&
        device.estimateMinutes != -1) {
        tooltip += L"\n" + EstimateText(device);
    }
    if (!device.updated.empty()) {
        tooltip += L"\nLast reading: " + device.updated;
    }
    wuxc::ToolTipService::SetToolTip(item, winrt::box_value(winrt::hstring(tooltip)));

    return item;
}

PCWSTR GlyphForKind(DeviceKind kind) {
    switch (kind) {
        case DeviceKind::Mouse:
            return L"\uE962";
        case DeviceKind::Gamepad:
            return L"\uE7FC";
        case DeviceKind::Headphones:
            return L"\uE7F6";
        case DeviceKind::Keyboard:
            return L"\uE765";
        case DeviceKind::Generic:
            break;
    }
    return L"\uE702";
}

// Released on the taskbar thread in Wh_ModBeforeUninit.
[[clang::no_destroy]] winrt::weak_ref<wuxc::Flyout> g_openFlyout;

void OpenBluetoothSettings() {
    ShellExecuteW(nullptr, L"open", L"ms-settings:bluetooth", nullptr, nullptr,
                  SW_SHOWNORMAL);
}

wuxc::TextBlock MakePanelText(const std::wstring& text,
                              double size,
                              bool semibold,
                              double opacity) {
    wuxc::TextBlock block;
    block.Text(text);
    block.FontSize(size);
    if (semibold) {
        block.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    }
    block.Opacity(opacity);
    block.TextWrapping(wux::TextWrapping::NoWrap);
    return block;
}

void ShowDevicePanel(wux::FrameworkElement const& anchor) {
    std::vector<DeviceState> devices;
    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        devices = g_devices;
    }
    Settings settings = GetSettingsCopy();

    wuxc::StackPanel root;
    root.Spacing(12);
    root.MinWidth(280);
    root.Children().Append(MakePanelText(L"Wireless devices", 15, true, 1.0));

    int shown = 0;
    for (const auto& device : devices) {
        const DeviceOverride* o = FindOverride(settings, device);
        if (o && o->hide) {
            continue;
        }
        shown++;
        std::wstring name =
            o && !o->displayName.empty() ? o->displayName : device.name;

        wuxc::StackPanel row;
        row.Orientation(wuxc::Orientation::Horizontal);
        row.Spacing(12);
        if (device.stale) {
            row.Opacity(0.6);
        }

        auto icon = MakeGlyph(GlyphForKind(device.kind), 22);
        icon.VerticalAlignment(wux::VerticalAlignment::Top);
        icon.Margin(wux::Thickness{0, 2, 0, 0});
        row.Children().Append(icon);

        wuxc::StackPanel details;
        details.Spacing(2);
        details.Children().Append(MakePanelText(name, 13, true, 1.0));

        std::wstring status;
        if (device.wiredNoBattery) {
            status = L"Wired (no battery)";
        } else if (device.percent >= 0) {
            status = (device.approximate ? L"About " : L"") +
                     std::to_wstring(device.percent) + L"%";
            if (device.charging) {
                status += L" \u00B7 Charging";
            }
        } else {
            status = L"Battery level unavailable";
        }
        details.Children().Append(MakePanelText(status, 12, false, 0.85));

        if (device.percent >= 0 && !device.wiredNoBattery) {
            wuxc::ProgressBar bar;
            bar.Minimum(0);
            bar.Maximum(100);
            bar.Value(device.percent);
            bar.Width(200);
            bar.HorizontalAlignment(wux::HorizontalAlignment::Left);
            bool low = device.percent <= settings.lowBatteryThreshold &&
                       !device.charging;
            if (low) {
                auto c = ParseColor(settings.lowBatteryColor);
                bar.Foreground(wuxm::SolidColorBrush(
                    c ? *c : winrt::Windows::UI::Color{0xFF, 0xE8, 0x11, 0x23}));
            }
            details.Children().Append(bar);
        }

        if (settings.timeEstimate != TimeEstimateMode::Off &&
            device.estimateMinutes != -1) {
            details.Children().Append(
                MakePanelText(EstimateText(device), 12, false, 0.85));
        }
        if (device.stale) {
            details.Children().Append(MakePanelText(
                L"Not responding, showing last known level", 11, false, 0.7));
        }
        if (!device.updated.empty()) {
            details.Children().Append(MakePanelText(
                L"Last reading: " + device.updated, 11, false, 0.7));
        }

        row.Children().Append(details);
        root.Children().Append(row);
    }

    if (!shown) {
        root.Children().Append(
            MakePanelText(L"No devices detected", 12, false, 0.85));
    }

    wuxc::Flyout flyout;
    wuxc::Button button;
    button.Content(winrt::box_value(L"Bluetooth settings"));
    button.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    auto weakFlyout = winrt::make_weak(flyout);
    button.Click([weakFlyout](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        OpenBluetoothSettings();
        if (auto f = weakFlyout.get()) {
            f.Hide();
        }
    });
    root.Children().Append(button);

    flyout.Content(root);
    flyout.Placement(wuxc::Primitives::FlyoutPlacementMode::Top);
    flyout.ShouldConstrainToRootBounds(false);
    flyout.ShowAt(anchor);
    g_openFlyout = weakFlyout;
}

void HideDevicePanel() {
    if (auto flyout = g_openFlyout.get()) {
        try {
            flyout.Hide();
        } catch (...) {
        }
    }
    g_openFlyout = nullptr;
}

void OnWidgetTapped(wf::IInspectable const& sender,
                    wux::Input::TappedRoutedEventArgs const& args) {
    if (g_unloading) {
        return;
    }
    switch (GetSettingsCopy().clickAction) {
        case ClickAction::Panel:
            if (auto anchor = sender.try_as<wux::FrameworkElement>()) {
                args.Handled(true);
                ShowDevicePanel(anchor);
            }
            break;
        case ClickAction::BluetoothSettings:
            args.Handled(true);
            OpenBluetoothSettings();
            break;
        case ClickAction::None:
            break;
    }
}

void RenderInstance(WidgetInstance& inst,                    const Settings& settings,
                    const std::vector<DeviceState>& devices) {
    if (!inst.widget) {
        return;
    }

    wuxm::Brush defaultBrush{nullptr};
    if (auto frame = inst.frame.get()) {
        if (auto control = frame.try_as<wuxc::Control>()) {
            defaultBrush = control.Foreground();
        }
    }

    auto children = inst.widget.Children();
    children.Clear();
    inst.widget.Spacing(settings.deviceSpacing);

    for (const auto& device : devices) {
        const DeviceOverride* o = FindOverride(settings, device);
        if (o && o->hide) {
            continue;
        }
        try {
            children.Append(BuildDeviceItem(settings, device, o, defaultBrush));
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"BuildDeviceItem failed: %08X", e.code().value);
        }
    }

    inst.widget.Visibility(children.Size() > 0 ? wux::Visibility::Visible
                                               : wux::Visibility::Collapsed);
}

void RenderAll(bool force) {
    int dataVersion = g_dataVersion.load();
    int settingsVersion = g_settingsVersion.load();
    if (!g_instances) {
        return;
    }
    if (!force && dataVersion == g_renderedDataVersion &&
        settingsVersion == g_renderedSettingsVersion) {
        return;
    }

    std::vector<DeviceState> devices;
    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        devices = g_devices;
    }
    Settings settings = GetSettingsCopy();

    for (auto& inst : *g_instances) {
        try {
            RenderInstance(inst, settings, devices);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"RenderInstance failed: %08X", e.code().value);
        }
    }

    g_renderedDataVersion = dataVersion;
    g_renderedSettingsVersion = settingsVersion;
}

void RetryPendingFrames();

void EnsureUiTimer() {
    if (g_uiTimer) {
        return;
    }
    g_uiTimer = wux::DispatcherTimer();
    g_uiTimer.Interval(std::chrono::milliseconds(1000));
    g_uiTimerToken = g_uiTimer.Tick([](wf::IInspectable const&,
                                       wf::IInspectable const&) {
        if (g_unloading) {
            return;
        }
        try {
            if (g_needScan) {
                RetryPendingFrames();
            }
            RenderAll(false);
        } catch (...) {
            Wh_Log(L"RenderAll failed: %08X", winrt::to_hresult().value);
        }
    });
    g_uiTimer.Start();
}

void StopUiTimer() {
    if (!g_uiTimer) {
        return;
    }
    try {
        g_uiTimer.Stop();
        g_uiTimer.Tick(g_uiTimerToken);
    } catch (...) {
    }
    g_uiTimer = nullptr;
}

wux::FrameworkElement FindDescendantByName(wux::DependencyObject const& root,
                                           PCWSTR name,
                                           int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == name) {
                return fe;
            }
        }
        if (auto found = FindDescendantByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

enum class TrayKind { Unknown, Primary, Secondary };

struct KnownFrame {
    winrt::weak_ref<wux::FrameworkElement> frame;
    void* frameAbi = nullptr;
    TrayKind kind = TrayKind::Unknown;
    ULONGLONG lastDetectTick = 0;
    TrayKind lastGuess = TrayKind::Unknown;
};

std::vector<KnownFrame> g_knownFrames;

TrayKind TrayKindFromWindow(HWND hWnd) {
    if (!hWnd) {
        return TrayKind::Unknown;
    }
    HWND root = GetAncestor(hWnd, GA_ROOT);
    WCHAR className[64];
    if (!root || !GetClassName(root, className, ARRAYSIZE(className))) {
        return TrayKind::Unknown;
    }
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return TrayKind::Primary;
    }
    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return TrayKind::Secondary;
    }
    return TrayKind::Unknown;
}

TrayKind TrayKindFromPoint(POINT pt) {
    struct Param {
        POINT pt;
        TrayKind kind;
    } param{pt, TrayKind::Unknown};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto p = reinterpret_cast<Param*>(lParam);
            DWORD pid = 0;
            if (!GetWindowThreadProcessId(hWnd, &pid) ||
                pid != GetCurrentProcessId()) {
                return TRUE;
            }
            RECT rc;
            if (!GetWindowRect(hWnd, &rc) || !PtInRect(&rc, p->pt)) {
                return TRUE;
            }
            TrayKind kind = TrayKindFromWindow(hWnd);
            if (kind != TrayKind::Unknown) {
                p->kind = kind;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));

    return param.kind;
}

TrayKind DetectTrayKindFromLayout(wux::FrameworkElement const& frame) {
    try {
        auto peer = wux::Automation::Peers::FrameworkElementAutomationPeer::
            CreatePeerForElement(frame);
        if (!peer) {
            return TrayKind::Unknown;
        }
        wf::Rect rect = peer.GetBoundingRectangle();
        if (rect.Width <= 0 || rect.Height <= 0) {
            return TrayKind::Unknown;
        }
        POINT center{(LONG)(rect.X + rect.Width / 2),
                     (LONG)(rect.Y + rect.Height / 2)};
        TrayKind kind = TrayKindFromPoint(center);
        Wh_Log(L"Tray rect %.0f,%.0f %.0fx%.0f -> kind %d", rect.X, rect.Y,
               rect.Width, rect.Height, (int)kind);
        return kind;
    } catch (...) {
        return TrayKind::Unknown;
    }
}

KnownFrame& RememberFrame(wux::FrameworkElement const& frame) {
    g_knownFrames.erase(
        std::remove_if(g_knownFrames.begin(), g_knownFrames.end(),
                       [](const KnownFrame& k) { return !k.frame.get(); }),
        g_knownFrames.end());

    void* abi = winrt::get_abi(frame);
    for (auto& k : g_knownFrames) {
        if (k.frameAbi == abi) {
            return k;
        }
    }
    KnownFrame k;
    k.frame = winrt::make_weak(frame);
    k.frameAbi = abi;
    g_knownFrames.push_back(k);
    return g_knownFrames.back();
}

TrayKind DetectTrayKindFromContent(wuxc::Panel const& container) {
    if (!container || container.ActualWidth() <= 0) {
        return TrayKind::Unknown;
    }
    for (auto child : container.Children()) {
        auto fe = child.try_as<wux::FrameworkElement>();
        if (!fe || fe.Name() != L"ControlCenterButton") {
            continue;
        }
        bool hasContent = fe.Visibility() == wux::Visibility::Visible &&
                          fe.ActualWidth() > 0;
        return hasContent ? TrayKind::Primary : TrayKind::Secondary;
    }
    return TrayKind::Unknown;
}

// Returns the confirmed kind (from the window under the tray or under the
// cursor) when known, otherwise a best guess from the tray content, which is
// re-evaluated on every call. `confirmed` tells which one it is.
TrayKind GetTrayKind(wux::FrameworkElement const& frame,
                     wuxc::Panel const& container,
                     bool* confirmed = nullptr) {
    KnownFrame& known = RememberFrame(frame);
    ULONGLONG now = GetTickCount64();
    if (known.kind == TrayKind::Unknown && now - known.lastDetectTick >= 500) {
        known.lastDetectTick = now;
        known.kind = DetectTrayKindFromLayout(frame);
    }
    if (confirmed) {
        *confirmed = known.kind != TrayKind::Unknown;
    }
    if (known.kind != TrayKind::Unknown) {
        return known.kind;
    }
    TrayKind guess = DetectTrayKindFromContent(container);
    if (guess != known.lastGuess) {
        Wh_Log(L"Tray kind from content: %d (was %d)", (int)guess,
               (int)known.lastGuess);
        known.lastGuess = guess;
    }
    return guess;
}

void SetTrayKindFromCursor(wux::FrameworkElement const& frame) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }
    TrayKind kind = TrayKindFromWindow(WindowFromPoint(pt));
    if (kind == TrayKind::Unknown) {
        return;
    }
    KnownFrame& known = RememberFrame(frame);
    if (known.kind != kind) {
        Wh_Log(L"Tray kind from cursor: %d (was %d)", (int)kind,
               (int)known.kind);
        known.kind = kind;
    }
}

int FindChildIndexByName(wuxc::Panel const& container, PCWSTR name) {
    auto children = container.Children();
    for (uint32_t i = 0; i < children.Size(); i++) {
        if (auto fe = children.GetAt(i).try_as<wux::FrameworkElement>()) {
            if (fe.Name() == name) {
                return (int)i;
            }
        }
    }
    return -1;
}

void LogContainer(wuxc::Panel const& container) {
    Wh_Log(L"Container class: %s", winrt::get_class_name(container).c_str());
    for (auto child : container.Children()) {
        auto fe = child.try_as<wux::FrameworkElement>();
        if (!fe) {
            continue;
        }
        Wh_Log(L"  child %s #%s column=%d visible=%d",
               winrt::get_class_name(fe).c_str(), fe.Name().c_str(),
               wuxc::Grid::GetColumn(fe),
               fe.Visibility() == wux::Visibility::Visible);
    }
}

void RemoveInstance(WidgetInstance& inst) {
    auto container = inst.container.get();
    if (container && inst.widget) {
        auto children = container.Children();
        uint32_t index = 0;
        if (children.IndexOf(inst.widget, index)) {
            children.RemoveAt(index);
        }
        if (auto grid = container.try_as<wuxc::Grid>()) {
            if (inst.addedColumn) {
                auto columns = grid.ColumnDefinitions();
                uint32_t colIndex = 0;
                if (columns.IndexOf(inst.addedColumn, colIndex)) {
                    columns.RemoveAt(colIndex);
                }
            }
            for (auto& [weakChild, originalColumn] : inst.shiftedChildren) {
                if (auto child = weakChild.get()) {
                    wuxc::Grid::SetColumn(child, originalColumn);
                }
            }
        }
    }
    inst.shiftedChildren.clear();
    inst.addedColumn = nullptr;
    inst.widget = nullptr;
}

bool InsertWidget(WidgetInstance& inst,
                  wuxc::Panel const& container,
                  Position position) {
    wuxc::StackPanel widget;
    widget.Name(L"BatteryWidget");
    widget.Tapped(OnWidgetTapped);
    widget.Orientation(wuxc::Orientation::Horizontal);
    widget.VerticalAlignment(wux::VerticalAlignment::Center);
    widget.Margin(wux::Thickness{6, 0, 6, 0});

    if (auto grid = container.try_as<wuxc::Grid>()) {
        auto columns = grid.ColumnDefinitions();
        if (columns.Size() == 0) {
            Wh_Log(L"Tray grid has no columns, skipping");
            return false;
        }
        for (auto child : container.Children()) {
            if (auto fe = child.try_as<wux::FrameworkElement>()) {
                int column = wuxc::Grid::GetColumn(fe);
                inst.shiftedChildren.emplace_back(winrt::make_weak(fe), column);
                wuxc::Grid::SetColumn(fe, column + 1);
            }
        }
        wuxc::ColumnDefinition column;
        column.Width(wux::GridLength{0, wux::GridUnitType::Auto});
        columns.InsertAt(0, column);
        inst.addedColumn = column;
        wuxc::Grid::SetColumn(widget, 0);
        container.Children().Append(widget);
    } else {
        int index = FindChildIndexByName(container, L"NotifyIconStack");
        if (index < 0) {
            index = 0;
        } else if (position == Position::RightOfChevron) {
            index++;
        }
        container.Children().InsertAt((uint32_t)index, widget);
    }

    inst.widget = widget;
    inst.container = winrt::make_weak(container);
    return true;
}

void EnsureWidgetForFrame(wux::FrameworkElement const& frame) {
    if (g_unloading || g_inEnsure || !frame || !g_instances) {
        return;
    }
    g_inEnsure = true;

    try {
        void* frameAbi = winrt::get_abi(frame);

        g_instances->erase(
            std::remove_if(g_instances->begin(), g_instances->end(),
                           [](WidgetInstance& inst) {
                               return !inst.frame.get();
                           }),
            g_instances->end());

        auto it = std::find_if(g_instances->begin(), g_instances->end(),
                               [&](const WidgetInstance& inst) {
                                   return inst.frameAbi == frameAbi;
                               });

        auto containerElement =
            FindDescendantByName(frame, L"SystemTrayFrameGrid", 4);
        auto container = containerElement
                             ? containerElement.try_as<wuxc::Panel>()
                             : nullptr;

        if (it != g_instances->end()) {
            bool confirmedKind = false;
            TrayKind currentKind =
                container ? GetTrayKind(frame, container, &confirmedKind)
                          : TrayKind::Unknown;
            bool allowed = g_showOnAllTaskbars || !confirmedKind ||
                           currentKind == TrayKind::Primary;
            bool valid = it->widget && container &&
                         it->container.get() == container &&
                         it->widget.Parent() == container;
            if (valid && !allowed) {
                Wh_Log(L"Removing widget from non-primary taskbar");
                RemoveInstance(*it);
                g_instances->erase(it);
                g_inEnsure = false;
                return;
            }
            if (valid) {
                g_inEnsure = false;
                return;
            }
            RemoveInstance(*it);
            g_instances->erase(it);
        }

        if (!container) {
            g_inEnsure = false;
            return;
        }

        TrayKind kind = GetTrayKind(frame, container);
        EnsureUiTimer();
        if (!g_showOnAllTaskbars && kind != TrayKind::Primary) {
            g_inEnsure = false;
            return;
        }

        LogContainer(container);

        WidgetInstance inst;
        inst.frame = winrt::make_weak(frame);
        inst.frameAbi = frameAbi;
        if (InsertWidget(inst, container, g_position)) {
            g_instances->push_back(std::move(inst));
            EnsureUiTimer();
            RenderAll(true);
            if (kind == TrayKind::Primary) {
                g_needScan = false;
            }
            Wh_Log(L"Widget inserted (kind=%d)", (int)kind);
        }
    } catch (...) {
        Wh_Log(L"EnsureWidgetForFrame failed: %08X", winrt::to_hresult().value);
    }

    g_inEnsure = false;
}

void RetryPendingFrames() {
    if (g_unloading || g_inEnsure) {
        return;
    }
    std::vector<wux::FrameworkElement> frames;
    for (auto& known : g_knownFrames) {
        if (auto frame = known.frame.get()) {
            frames.push_back(frame);
        }
    }
    for (auto& frame : frames) {
        EnsureWidgetForFrame(frame);
    }
}

wux::FrameworkElement FindAncestorTrayFrame(wux::FrameworkElement const& element) {
    wux::DependencyObject current = element;
    for (int i = 0; i < 20 && current; i++) {
        if (auto fe = current.try_as<wux::FrameworkElement>()) {
            if (winrt::get_class_name(fe) == L"SystemTray.SystemTrayFrame") {
                return fe;
            }
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

wux::FrameworkElement ElementFromThis(void* pThis) {
    wux::FrameworkElement element{nullptr};
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                         winrt::put_abi(element));
    return element;
}

void RemoveAllWidgets() {
    if (!g_instances) {
        return;
    }
    for (auto& inst : *g_instances) {
        try {
            RemoveInstance(inst);
        } catch (...) {
            Wh_Log(L"RemoveInstance failed: %08X", winrt::to_hresult().value);
        }
    }
    g_instances->clear();
}

#pragma endregion

#pragma region hooks

HWND FindCurrentProcessTaskbarWnd();

std::atomic<DWORD> g_taskbarThreadId{0};
std::atomic<bool> g_loggedForeignThread{false};

bool IsTaskbarThread() {
    DWORD threadId = g_taskbarThreadId.load();
    if (!threadId) {
        HWND hTaskbar = FindCurrentProcessTaskbarWnd();
        if (!hTaskbar) {
            return false;
        }
        threadId = GetWindowThreadProcessId(hTaskbar, nullptr);
        g_taskbarThreadId = threadId;
    }
    if (GetCurrentThreadId() == threadId) {
        return true;
    }
    if (!g_loggedForeignThread.exchange(true)) {
        Wh_Log(L"Skipping tray on thread %u (taskbar thread is %u)",
               GetCurrentThreadId(), threadId);
    }
    return false;
}

using ArrangeOverride_t = int(WINAPI*)(void* pThis,
                                       wf::Size finalSize,
                                       wf::Size* result);
ArrangeOverride_t SystemTrayFrame_ArrangeOverride_Original;
ArrangeOverride_t IconView_ArrangeOverride_Original;

using OnPointerEntered_t = int(WINAPI*)(void* pThis, void* args);
OnPointerEntered_t SystemTrayFrame_OnPointerEntered_Original;

int WINAPI SystemTrayFrame_ArrangeOverride_Hook(void* pThis,
                                                wf::Size finalSize,
                                                wf::Size* result) {
    int ret = SystemTrayFrame_ArrangeOverride_Original(pThis, finalSize, result);
    if (!g_unloading && !g_inEnsure && IsTaskbarThread()) {
        try {
            EnsureWidgetForFrame(ElementFromThis(pThis));
        } catch (...) {
        }
    }
    return ret;
}

int WINAPI IconView_ArrangeOverride_Hook(void* pThis,
                                         wf::Size finalSize,
                                         wf::Size* result) {
    int ret = IconView_ArrangeOverride_Original(pThis, finalSize, result);
    if (g_needScan && !g_unloading && !g_inEnsure && IsTaskbarThread()) {
        try {
            if (auto element = ElementFromThis(pThis)) {
                if (auto frame = FindAncestorTrayFrame(element)) {
                    EnsureWidgetForFrame(frame);
                }
            }
        } catch (...) {
        }
    }
    return ret;
}

int WINAPI SystemTrayFrame_OnPointerEntered_Hook(void* pThis, void* args) {
    int ret = SystemTrayFrame_OnPointerEntered_Original(pThis, args);
    if (!g_unloading && !g_inEnsure && IsTaskbarThread()) {
        try {
            if (auto frame = ElementFromThis(pThis)) {
                SetTrayKindFromCursor(frame);
                EnsureWidgetForFrame(frame);
            }
        } catch (...) {
        }
    }
    return ret;
}

using IconView_PropertyChanged_t = void(WINAPI*)(void* pThis,
                                                 void* sender,
                                                 void* args);
IconView_PropertyChanged_t IconView_OnIconViewModelPropertyChanged_Original;
IconView_PropertyChanged_t IconView_OnContentViewModelPropertyChanged_Original;

void ScanFromIconViewImpl(void* pThis) {
    if (!g_needScan || g_unloading || g_inEnsure || !IsTaskbarThread()) {
        return;
    }
    try {
        wux::FrameworkElement element{nullptr};
        ((IUnknown**)pThis)[1]->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(element));
        if (element) {
            if (auto frame = FindAncestorTrayFrame(element)) {
                EnsureWidgetForFrame(frame);
            }
        }
    } catch (...) {
    }
}

void WINAPI IconView_OnIconViewModelPropertyChanged_Hook(void* pThis,
                                                         void* sender,
                                                         void* args) {
    IconView_OnIconViewModelPropertyChanged_Original(pThis, sender, args);
    ScanFromIconViewImpl(pThis);
}

void WINAPI IconView_OnContentViewModelPropertyChanged_Hook(void* pThis,
                                                            void* sender,
                                                            void* args) {
    IconView_OnContentViewModelPropertyChanged_Original(pThis, sender, args);
    ScanFromIconViewImpl(pThis);
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::ArrangeOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &SystemTrayFrame_ArrangeOverride_Original,
            SystemTrayFrame_ArrangeOverride_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::ArrangeOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &IconView_ArrangeOverride_Original,
            IconView_ArrangeOverride_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerEntered(void *))"},
            &SystemTrayFrame_OnPointerEntered_Original,
            SystemTrayFrame_OnPointerEntered_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::IconView::OnIconViewModelPropertyChanged(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Windows::Foundation::IInspectable const &))"},
            &IconView_OnIconViewModelPropertyChanged_Original,
            IconView_OnIconViewModelPropertyChanged_Hook,
            true,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::SystemTray::implementation::IconView::OnContentViewModelPropertyChanged(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Windows::Foundation::IInspectable const &))"},
            &IconView_OnContentViewModelPropertyChanged_Original,
            IconView_OnContentViewModelPropertyChanged_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, systemTrayHooks,
                                      ARRAYSIZE(systemTrayHooks));
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002, the SystemTray types
            // moved into SystemTray.dll.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void HookSystemTrayModuleIfLoaded(HMODULE candidate, bool applyNow) {
    if (g_systemTrayHooked) {
        return;
    }
    HMODULE module = GetSystemTrayModuleHandle();
    if (!module || (candidate && candidate != module)) {
        return;
    }
    if (g_systemTrayHooked.exchange(true)) {
        return;
    }
    WCHAR path[MAX_PATH];
    if (GetModuleFileName(module, path, ARRAYSIZE(path))) {
        Wh_Log(L"Hooking system tray module %s", path);
    }
    if (!HookSystemTraySymbols(module)) {
        Wh_Log(L"HookSymbols failed");
        return;
    }
    if (applyNow) {
        Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_systemTrayHooked) {
        HookSystemTrayModuleIfLoaded(module, true);
    }
    return module;
}

#pragma endregion

#pragma region thread_helpers

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
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

#pragma endregion

#pragma region taskbar_xaml_root

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

bool g_taskbarSymbolsResolved = false;

bool ResolveTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"taskbar.dll symbols not found, using layout hooks only");
        return false;
    }
    return CTaskBand_ITaskListWndSite_vftable &&
           CTaskBand_GetTaskbarHost_Original &&
           TaskbarHost_FrameHeight_Original &&
           std__Ref_count_base__Decref_Original;
}

wux::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!g_taskbarSymbolsResolved) {
        return nullptr;
    }

    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;
    }
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }

    wux::XamlRoot result{nullptr};
    if (taskbarHostSharedPtr[0]) {
        auto* taskbarElementIUnknown =
            *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                          taskbarElementIUnknownOffset);
        wux::FrameworkElement taskbarElement{nullptr};
        if (taskbarElementIUnknown) {
            taskbarElementIUnknown->QueryInterface(
                winrt::guid_of<wux::FrameworkElement>(),
                winrt::put_abi(taskbarElement));
        }
        if (taskbarElement) {
            result = taskbarElement.XamlRoot();
        }
    }

    if (taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

wux::FrameworkElement FindDescendantByClassName(wux::DependencyObject const& root,
                                                PCWSTR className,
                                                int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (winrt::get_class_name(fe) == className) {
                return fe;
            }
        }
        if (auto found = FindDescendantByClassName(child, className, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

void InsertIntoPrimaryTaskbarNow() {
    HWND hTaskbar = FindCurrentProcessTaskbarWnd();
    if (!hTaskbar || !g_taskbarSymbolsResolved) {
        return;
    }
    RunFromWindowThread(
        hTaskbar,
        [](PVOID param) {
            try {
                auto xamlRoot = GetTaskbarXamlRoot((HWND)param);
                if (!xamlRoot) {
                    Wh_Log(L"Getting taskbar XamlRoot failed");
                    return;
                }
                auto content = xamlRoot.Content();
                auto frame = FindDescendantByClassName(
                    content, L"SystemTray.SystemTrayFrame", 40);
                if (!frame) {
                    Wh_Log(L"System tray frame not found in taskbar XamlRoot");
                    return;
                }
                RememberFrame(frame).kind = TrayKind::Primary;
                EnsureWidgetForFrame(frame);
            } catch (...) {
                Wh_Log(L"InsertIntoPrimaryTaskbarNow failed: %08X",
                       winrt::to_hresult().value);
            }
        },
        hTaskbar);
}

#pragma endregion

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_refreshEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_refreshEvent) {
        return FALSE;
    }

    if (HMODULE module = GetSystemTrayModuleHandle()) {
        g_systemTrayHooked = true;
        WCHAR path[MAX_PATH];
        if (GetModuleFileName(module, path, ARRAYSIZE(path))) {
            Wh_Log(L"Hooking system tray module %s", path);
        }
        if (!HookSystemTraySymbols(module)) {
            Wh_Log(L"HookSymbols failed");
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(
            kernelBase, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    g_taskbarSymbolsResolved = ResolveTaskbarDllSymbols();

    g_pollThread = CreateThread(nullptr, 0, PollThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HookSystemTrayModuleIfLoaded(nullptr, true);
    InsertIntoPrimaryTaskbarNow();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_pollThread) {
        WaitForSingleObject(g_pollThread, INFINITE);
        CloseHandle(g_pollThread);
        g_pollThread = nullptr;
    }

    if (HWND hTaskbar = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTaskbar,
            [](PVOID) {
                HideDevicePanel();
                StopUiTimer();
                RemoveAllWidgets();
                g_instances.reset();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_refreshEvent) {
        CloseHandle(g_refreshEvent);
        g_refreshEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    g_settingsVersion++;
    g_needScan = true;
    if (g_refreshEvent) {
        SetEvent(g_refreshEvent);
    }

    if (HWND hTaskbar = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTaskbar,
            [](PVOID) {
                try {
                    RemoveAllWidgets();
                    std::vector<wux::FrameworkElement> frames;
                    for (auto& known : g_knownFrames) {
                        if (auto frame = known.frame.get()) {
                            frames.push_back(frame);
                        }
                    }
                    for (auto& frame : frames) {
                        EnsureWidgetForFrame(frame);
                    }
                    RenderAll(true);
                } catch (...) {
                    Wh_Log(L"Settings apply failed: %08X",
                           winrt::to_hresult().value);
                }
            },
            nullptr);
    }
}
