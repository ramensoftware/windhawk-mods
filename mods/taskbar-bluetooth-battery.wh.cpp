// ==WindhawkMod==
// @id              taskbar-bluetooth-battery
// @name            Taskbar Bluetooth Device Battery Level
// @description     Displays connected Bluetooth devices and their battery levels on the Windows 11 taskbar system tray
// @version         1.0.6
// @author          Vicente Higino
// @github          https://github.com/vicente-higino
// @homepage        https://github.com/vicente-higino/taskbar-bluetooth-battery
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lcfgmgr32 -lshell32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Bluetooth Device Battery Level

> **⚠️ AI Disclaimer:** This Windhawk mod was created and developed with the assistance of AI (Antigravity / Google DeepMind). While tested on Windows 11, please review the code and use at your own discretion.

Displays currently connected Bluetooth devices and their battery percentages
directly on the Windows 11 taskbar system tray using a native XAML widget.

## Features
- **Classic Bluetooth & BLE Support**: Automatically detects connected Classic
  Bluetooth (Hands-Free / AVRCP) and Bluetooth Low Energy (GATT Battery Service)
  devices via Windows Configuration Manager (`CfgMgr32`) and WinRT
  `BluetoothLEDevice`.
- **Smart Device Category Icons**: Uses native Windows 11 `Segoe Fluent Icons`
  glyphs for headphones, headsets, speakers, mice, keyboards, controllers,
  pens/styluses, phones, watches, and generic Bluetooth devices.
- **Rich Hover Tooltip**: Hovering over the widget displays the full friendly
  name, category, and battery percentage of every connected Bluetooth device.
- **Battery Level Color Coding**: Automatically highlights low (`<= 20%`) and
  critical (`<= 10%`) battery levels while respecting Windows 11 Light and Dark
  taskbar themes.
- **Event-Driven Updates**: Immediately updates when Bluetooth devices connect
  or disconnect via PnP Configuration Manager notifications, paired with a
  lightweight background polling timer.
- **Quick Actions**: Clicking the widget opens the Windows 11 Quick Settings
  Bluetooth flyout (`ms-controlcenter:bluetooth`), Windows Bluetooth Settings
  (`ms-settings:bluetooth`), or triggers an immediate battery refresh.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayFormat: iconAndPercentage
  $name: Inline display format
  $description: Choose what information is shown for each device on the taskbar.
  $options:
  - iconAndPercentage: Device category icon + percentage (e.g. 🎧 85%)
  - nameAndPercentage: Device name + percentage (e.g. WH-1000XM5 85%)
  - iconNameAndPercentage: Device icon + name + percentage (e.g. 🎧 WH-1000XM5 85%)
  - batteryIconAndPercentage: Dynamic battery icon + percentage (e.g. 🔋 85%)
- multiDeviceMode: all
  $name: Multiple connected devices mode
  $description: How to display when multiple Bluetooth devices are connected.
  $options:
  - all: Show all connected devices side-by-side (up to Max devices)
  - lowest: Show only the device with the lowest battery inline (all in tooltip)
- maxDevices: 5
  $name: Maximum inline devices
  $description: Maximum number of Bluetooth devices displayed side-by-side on the taskbar.
- maxNameLength: 12
  $name: Maximum inline device name length
  $description: Maximum number of characters when displaying device names inline (0 = unlimited).
- position: beforeControlCenter
  $name: Taskbar tray position
  $description: Where to place the Bluetooth battery widget in the system tray area.
  $options:
  - beforeControlCenter: Left of Quick Settings (Wi-Fi / Volume / Battery)
  - afterControlCenter: Between Quick Settings and Clock
  - beforeTrayIcons: Left of system tray icons
  - insideControlCenter: Inside Quick Settings button
- whenDisconnected: hide
  $name: When no Bluetooth device is connected
  $description: Whether to hide the widget or show a placeholder icon when no Bluetooth devices with battery info are connected.
  $options:
  - hide: Hide widget automatically
  - showPlaceholder: Show dimmed Bluetooth icon (--%)
- colorCoding: lowOnly
  $name: Battery level color coding
  $description: Colorize battery text/icons based on remaining battery level.
  $options:
  - lowOnly: Only highlight low (<= 20% amber) and critical (<= 10% red) battery
  - always: Always color-code (green > 20%, amber <= 20%, red <= 10%)
  - none: Always use default taskbar text color
- clickAction: openQuickSettings
  $name: Click action
  $description: Action performed when clicking the Bluetooth battery widget on the taskbar.
  $options:
  - openQuickSettings: Open Quick Settings Bluetooth flyout (ms-controlcenter:bluetooth)
  - openBluetoothSettings: Open Windows Bluetooth Settings (ms-settings:bluetooth)
  - refresh: Force immediate battery refresh
  - none: None
- showOnSecondaryTaskbars: true
  $name: Show on secondary monitor taskbars
  $description: Display the Bluetooth battery widget on secondary monitors in multi-monitor setups.
- refreshIntervalSeconds: 15
  $name: Refresh interval (seconds)
  $description: How often to poll battery levels in the background (in addition to instant connect/disconnect events).
- fontSize: 12
  $name: Font size
  $description: Font size (in pixels) for the battery percentage text and icons.
- itemSpacing: 8
  $name: Spacing between devices (pixels)
  $description: Horizontal spacing between multiple connected Bluetooth device items.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>
#include <cfgmgr32.h>
#include <commctrl.h>
#include <devpkey.h>
#include <propkey.h>
#include <shellapi.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

// ---------------------------------------------------------------------------
// Settings definitions
// ---------------------------------------------------------------------------

enum class DisplayFormat {
    iconAndPercentage,
    nameAndPercentage,
    iconNameAndPercentage,
    batteryIconAndPercentage,
};

enum class MultiDeviceMode {
    all,
    lowest,
};

enum class TrayPosition {
    beforeControlCenter,
    afterControlCenter,
    beforeTrayIcons,
    insideControlCenter,
};

enum class WhenDisconnected {
    hide,
    showPlaceholder,
};

enum class ColorCoding {
    lowOnly,
    always,
    none,
};

enum class ClickAction {
    openBluetoothSettings,
    openQuickSettings,
    refresh,
    none,
};

struct ModSettings {
    DisplayFormat displayFormat = DisplayFormat::iconAndPercentage;
    MultiDeviceMode multiDeviceMode = MultiDeviceMode::all;
    int maxDevices = 5;
    int maxNameLength = 12;
    TrayPosition position = TrayPosition::beforeControlCenter;
    WhenDisconnected whenDisconnected = WhenDisconnected::hide;
    ColorCoding colorCoding = ColorCoding::lowOnly;
    ClickAction clickAction = ClickAction::openBluetoothSettings;
    bool showOnSecondaryTaskbars = true;
    int refreshIntervalSeconds = 15;
    int fontSize = 12;
    int itemSpacing = 8;
} g_settings;

// ---------------------------------------------------------------------------
// Device property keys & Classic Bluetooth API definitions
// ---------------------------------------------------------------------------

DEFINE_DEVPROPKEY(DEVPKEY_Bluetooth_Battery,
                  0x104ea319,
                  0x6ee2,
                  0x4701,
                  0xbd,
                  0x47,
                  0x8d,
                  0xdb,
                  0xf4,
                  0x25,
                  0xbb,
                  0xe5,
                  2);

DEFINE_DEVPROPKEY(DEVPKEY_Bluetooth_DeviceAddress,
                  0x2bd67d8b,
                  0x8beb,
                  0x48d5,
                  0x87,
                  0xe0,
                  0x6c,
                  0xda,
                  0x34,
                  0x28,
                  0x04,
                  0x0a,
                  1);

DEFINE_DEVPROPKEY(DEVPKEY_Bluetooth_ClassOfDevice,
                  0x2bd67d8b,
                  0x8beb,
                  0x48d5,
                  0x87,
                  0xe0,
                  0x6c,
                  0xda,
                  0x34,
                  0x28,
                  0x04,
                  0x0a,
                  2);

DEFINE_DEVPROPKEY(DEVPKEY_Aep_IsConnected,
                  0xa35996ab,
                  0x11cf,
                  0x4935,
                  0x8b,
                  0x61,
                  0xa6,
                  0x76,
                  0x10,
                  0x81,
                  0xec,
                  0xdf,
                  4);

DEFINE_DEVPROPKEY(DEVPKEY_DeviceContainer_Category_Local,
                  0x78c34fc8,
                  0x104a,
                  0x4aca,
                  0x9e,
                  0xa4,
                  0x52,
                  0x4d,
                  0x52,
                  0x99,
                  0x6e,
                  0x57,
                  90);

using BTH_ADDR_LOCAL = ULONGLONG;

struct BLUETOOTH_DEVICE_SEARCH_PARAMS_LOCAL {
    DWORD dwSize;
    BOOL fReturnAuthenticated;
    BOOL fReturnRemembered;
    BOOL fReturnUnknown;
    BOOL fReturnConnected;
    BOOL fIssueInquiry;
    UCHAR cTimeoutMultiplier;
    HANDLE hRadio;
};

struct BLUETOOTH_DEVICE_INFO_LOCAL {
    DWORD dwSize;
    BTH_ADDR_LOCAL Address;
    ULONG ulClassofDevice;
    BOOL fConnected;
    BOOL fRemembered;
    BOOL fAuthenticated;
    SYSTEMTIME stLastSeen;
    SYSTEMTIME stLastUsed;
    WCHAR szName[248];
};

using HBLUETOOTH_DEVICE_FIND_LOCAL = HANDLE;
using BluetoothFindFirstDevice_t = HBLUETOOTH_DEVICE_FIND_LOCAL(WINAPI*)(
    const BLUETOOTH_DEVICE_SEARCH_PARAMS_LOCAL* pbtsp,
    BLUETOOTH_DEVICE_INFO_LOCAL* pbtdi);
using BluetoothFindNextDevice_t = BOOL(WINAPI*)(
    HBLUETOOTH_DEVICE_FIND_LOCAL hFind,
    BLUETOOTH_DEVICE_INFO_LOCAL* pbtdi);
using BluetoothFindDeviceClose_t = BOOL(WINAPI*)(
    HBLUETOOTH_DEVICE_FIND_LOCAL hFind);

// ---------------------------------------------------------------------------
// Data models
// ---------------------------------------------------------------------------

enum class DeviceCategory {
    Headphones,
    Headset,
    Speaker,
    Mouse,
    Keyboard,
    Gamepad,
    Pen,
    Phone,
    Watch,
    GenericBluetooth,
};

struct BluetoothDeviceBatteryInfo {
    std::wstring id;
    std::wstring name;
    int batteryPercent = -1;
    DeviceCategory category = DeviceCategory::GenericBluetooth;
    bool isBle = false;

    bool operator==(const BluetoothDeviceBatteryInfo& other) const {
        return id == other.id && name == other.name &&
               batteryPercent == other.batteryPercent &&
               category == other.category && isBle == other.isBle;
    }
};

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

std::atomic<bool> g_systemTrayModuleHooked{false};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_taskbarSubclassed{false};
std::atomic<bool> g_isNonTaskbarProcess{false};
std::atomic<uint64_t> g_devicesGeneration{1};
thread_local bool g_inLayoutUpdate = false;

HWND g_hTaskbarWnd = nullptr;
UINT g_msgUpdateWidgets = 0;

std::mutex g_devicesMutex;
std::vector<BluetoothDeviceBatteryInfo> g_connectedDevices;

HANDLE g_workerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
HCMNOTIFICATION g_cmNotificationHandle = nullptr;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

struct InjectedWidgetState {
    winrt::weak_ref<FrameworkElement> systemTrayFrame;
    winrt::weak_ref<Controls::Panel> parentPanel;
    winrt::weak_ref<Controls::Border> widgetElement;
    std::vector<winrt::weak_ref<Controls::ColumnDefinition>> addedColumns;
    int insertedColumnIndex = -1;
    uint64_t renderedGeneration = 0;
};

[[clang::no_destroy]] std::optional<std::vector<InjectedWidgetState>>
    g_injectedWidgets{std::in_place};

struct OrientationStatesSubscription {
    winrt::weak_ref<VisualStateGroup> group;
    winrt::event_token currentStateChangedToken{};
};

[[clang::no_destroy]] std::optional<std::vector<OrientationStatesSubscription>>
    g_orientationSubscriptions{std::in_place};

constexpr PCWSTR kWidgetElementName = L"WindhawkBluetoothBatteryWidget";

// ---------------------------------------------------------------------------
// Helper utilities
// ---------------------------------------------------------------------------

std::wstring ToLower(std::wstring_view input) {
    std::wstring out(input);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](wchar_t c) { return std::towlower(c); });
    return out;
}

bool ContainsIgnoreCase(std::wstring_view haystack, std::wstring_view needle) {
    auto h = ToLower(haystack);
    auto n = ToLower(needle);
    return h.find(n) != std::wstring::npos;
}

bool IsGenericProfileName(std::wstring_view name) {
    if (name.empty()) {
        return true;
    }
    return ContainsIgnoreCase(name, L"hands-free") ||
           ContainsIgnoreCase(name, L"avrcp") ||
           ContainsIgnoreCase(name, L"generic attribute") ||
           ContainsIgnoreCase(name, L"generic access") ||
           ContainsIgnoreCase(name, L"device information") ||
           ContainsIgnoreCase(name, L"battery service") ||
           ContainsIgnoreCase(name, L"bluetooth le device") ||
           ContainsIgnoreCase(name, L"bluetooth peripheral") ||
           ContainsIgnoreCase(name, L"hid-compliant") ||
           ContainsIgnoreCase(name, L"bluetooth hid device") ||
           ContainsIgnoreCase(name, L"standard serial over bluetooth");
}

PCWSTR GetCategoryGlyph(DeviceCategory category) {
    switch (category) {
        case DeviceCategory::Headphones:
            return L"\uE7F6";
        case DeviceCategory::Headset:
            return L"\uE95B";
        case DeviceCategory::Speaker:
            return L"\uE7F5";
        case DeviceCategory::Mouse:
            return L"\uE962";
        case DeviceCategory::Keyboard:
            return L"\uE765";
        case DeviceCategory::Gamepad:
            return L"\uE7FC";
        case DeviceCategory::Pen:
            return L"\uEDC6";
        case DeviceCategory::Phone:
            return L"\uE8EA";
        case DeviceCategory::Watch:
            return L"\uE916";
        case DeviceCategory::GenericBluetooth:
        default:
            return L"\uE702";
    }
}

PCWSTR GetCategoryDisplayName(DeviceCategory category) {
    switch (category) {
        case DeviceCategory::Headphones:
            return L"Headphones";
        case DeviceCategory::Headset:
            return L"Headset";
        case DeviceCategory::Speaker:
            return L"Speaker";
        case DeviceCategory::Mouse:
            return L"Mouse";
        case DeviceCategory::Keyboard:
            return L"Keyboard";
        case DeviceCategory::Gamepad:
            return L"Controller";
        case DeviceCategory::Pen:
            return L"Pen";
        case DeviceCategory::Phone:
            return L"Phone";
        case DeviceCategory::Watch:
            return L"Watch";
        case DeviceCategory::GenericBluetooth:
        default:
            return L"Bluetooth Device";
    }
}

PCWSTR GetBatteryLevelGlyph(int batteryPercent) {
    int clamped = std::clamp(batteryPercent, 0, 100);
    int bucket = (clamped + 5) / 10;
    static const PCWSTR kBatteryGlyphs[11] = {
        L"\uEBA0", L"\uEBA1", L"\uEBA2", L"\uEBA3", L"\uEBA4", L"\uEBA5",
        L"\uEBA6", L"\uEBA7", L"\uEBA8", L"\uEBA9", L"\uEBAA",
    };
    return kBatteryGlyphs[bucket];
}

DeviceCategory ClassifyFromClassOfDevice(ULONG cod) {
    if (cod == 0) {
        return DeviceCategory::GenericBluetooth;
    }

    ULONG major = (cod >> 8) & 0x1F;
    ULONG minor = (cod >> 2) & 0x3F;

    switch (major) {
        case 0x02:
            return DeviceCategory::Phone;

        case 0x04:
            switch (minor) {
                case 0x01:
                case 0x02:
                    return DeviceCategory::Headset;
                case 0x06:
                    return DeviceCategory::Headphones;
                case 0x05:
                case 0x07:
                case 0x08:
                case 0x0A:
                    return DeviceCategory::Speaker;
                default:
                    return DeviceCategory::Headphones;
            }

        case 0x05: {
            ULONG peripheralType = (minor >> 4) & 0x03;
            ULONG subType = minor & 0x0F;
            if (peripheralType == 0x01 || peripheralType == 0x03) {
                return DeviceCategory::Keyboard;
            } else if (peripheralType == 0x02) {
                return DeviceCategory::Mouse;
            }
            if (subType == 0x01 || subType == 0x02) {
                return DeviceCategory::Gamepad;
            } else if (subType == 0x05) {
                return DeviceCategory::Pen;
            }
            break;
        }

        case 0x07:
            if (minor == 0x01) {
                return DeviceCategory::Watch;
            }
            break;
    }

    return DeviceCategory::GenericBluetooth;
}

DeviceCategory ClassifyFromBleAppearance(uint16_t appearanceCategory) {
    switch (appearanceCategory) {
        case 0x0001:
            return DeviceCategory::Phone;
        case 0x0003:
            return DeviceCategory::Watch;
        case 0x0021:
            return DeviceCategory::Speaker;
        case 0x0025:
            return DeviceCategory::Headphones;
        default:
            break;
    }
    return DeviceCategory::GenericBluetooth;
}

DeviceCategory ClassifyFromBleAppearanceRaw(uint16_t rawAppearance) {
    switch (rawAppearance) {
        case 961:
            return DeviceCategory::Keyboard;
        case 962:
            return DeviceCategory::Mouse;
        case 963:
        case 964:
            return DeviceCategory::Gamepad;
        case 965:
        case 967:
            return DeviceCategory::Pen;
        default:
            break;
    }
    return ClassifyFromBleAppearance(rawAppearance >> 6);
}

DeviceCategory ClassifyFromNameAndCategoryString(
    std::wstring_view name,
    std::wstring_view containerCategory) {
    if (!containerCategory.empty()) {
        if (ContainsIgnoreCase(containerCategory, L"Mouse")) {
            return DeviceCategory::Mouse;
        }
        if (ContainsIgnoreCase(containerCategory, L"Keyboard")) {
            return DeviceCategory::Keyboard;
        }
        if (ContainsIgnoreCase(containerCategory, L"Gaming")) {
            return DeviceCategory::Gamepad;
        }
        if (ContainsIgnoreCase(containerCategory, L"Pen")) {
            return DeviceCategory::Pen;
        }
        if (ContainsIgnoreCase(containerCategory, L"Headset")) {
            return DeviceCategory::Headset;
        }
        if (ContainsIgnoreCase(containerCategory, L"Headphone") ||
            ContainsIgnoreCase(containerCategory, L"Earbud")) {
            return DeviceCategory::Headphones;
        }
        if (ContainsIgnoreCase(containerCategory, L"Speaker")) {
            return DeviceCategory::Speaker;
        }
        if (ContainsIgnoreCase(containerCategory, L"Audio") ||
            ContainsIgnoreCase(containerCategory, L"Multimedia")) {
            return DeviceCategory::Headphones;
        }
    }

    if (ContainsIgnoreCase(name, L"mouse") ||
        ContainsIgnoreCase(name, L"mx master") ||
        ContainsIgnoreCase(name, L"mx anywhere") ||
        ContainsIgnoreCase(name, L"deathadder") ||
        ContainsIgnoreCase(name, L"basilisk") ||
        ContainsIgnoreCase(name, L"orochi") ||
        ContainsIgnoreCase(name, L"trackpad")) {
        return DeviceCategory::Mouse;
    }
    if (ContainsIgnoreCase(name, L"keyboard") ||
        ContainsIgnoreCase(name, L"keychron") ||
        ContainsIgnoreCase(name, L"mx keys") ||
        ContainsIgnoreCase(name, L"nuphy") ||
        ContainsIgnoreCase(name, L"hhkb")) {
        return DeviceCategory::Keyboard;
    }
    if (ContainsIgnoreCase(name, L"controller") ||
        ContainsIgnoreCase(name, L"gamepad") ||
        ContainsIgnoreCase(name, L"xbox") ||
        ContainsIgnoreCase(name, L"dualsense") ||
        ContainsIgnoreCase(name, L"dualshock") ||
        ContainsIgnoreCase(name, L"joy-con") ||
        ContainsIgnoreCase(name, L"8bitdo")) {
        return DeviceCategory::Gamepad;
    }
    if (ContainsIgnoreCase(name, L"pen") ||
        ContainsIgnoreCase(name, L"pencil") ||
        ContainsIgnoreCase(name, L"stylus")) {
        return DeviceCategory::Pen;
    }
    if (ContainsIgnoreCase(name, L"speaker") ||
        ContainsIgnoreCase(name, L"soundlink") ||
        ContainsIgnoreCase(name, L"flip ") ||
        ContainsIgnoreCase(name, L"charge ") ||
        ContainsIgnoreCase(name, L"boom")) {
        return DeviceCategory::Speaker;
    }
    if (ContainsIgnoreCase(name, L"headset")) {
        return DeviceCategory::Headset;
    }
    if (ContainsIgnoreCase(name, L"airpods") ||
        ContainsIgnoreCase(name, L"buds") ||
        ContainsIgnoreCase(name, L"headphone") ||
        ContainsIgnoreCase(name, L"earphone") ||
        ContainsIgnoreCase(name, L"earbud") ||
        ContainsIgnoreCase(name, L"wh-") ||
        ContainsIgnoreCase(name, L"wf-") ||
        ContainsIgnoreCase(name, L"quietcomfort") ||
        ContainsIgnoreCase(name, L"momentum") ||
        ContainsIgnoreCase(name, L"beats")) {
        return DeviceCategory::Headphones;
    }
    if (ContainsIgnoreCase(name, L"watch")) {
        return DeviceCategory::Watch;
    }
    if (ContainsIgnoreCase(name, L"iphone") ||
        ContainsIgnoreCase(name, L"galaxy") ||
        ContainsIgnoreCase(name, L"pixel")) {
        return DeviceCategory::Phone;
    }

    return DeviceCategory::GenericBluetooth;
}

// ---------------------------------------------------------------------------
// DevNode Property Helpers
// ---------------------------------------------------------------------------

std::optional<BYTE> GetDevNodeByteProperty(DEVINST devInst,
                                           const DEVPROPKEY& key) {
    DEVPROPTYPE propType = 0;
    BYTE value = 0;
    ULONG size = sizeof(value);
    CONFIGRET cr =
        CM_Get_DevNode_PropertyW(devInst, &key, &propType, &value, &size, 0);
    if (cr == CR_SUCCESS &&
        (propType == DEVPROP_TYPE_BYTE || propType == DEVPROP_TYPE_SBYTE)) {
        return value;
    }
    return std::nullopt;
}

std::optional<bool> GetDevNodeBoolProperty(DEVINST devInst,
                                           const DEVPROPKEY& key) {
    DEVPROPTYPE propType = 0;
    DEVPROP_BOOLEAN value = DEVPROP_FALSE;
    ULONG size = sizeof(value);
    CONFIGRET cr = CM_Get_DevNode_PropertyW(devInst, &key, &propType,
                                            (PBYTE)&value, &size, 0);
    if (cr == CR_SUCCESS && propType == DEVPROP_TYPE_BOOLEAN) {
        return value != DEVPROP_FALSE;
    }
    return std::nullopt;
}

std::optional<uint32_t> GetDevNodeUInt32Property(DEVINST devInst,
                                                 const DEVPROPKEY& key) {
    DEVPROPTYPE propType = 0;
    uint32_t value = 0;
    ULONG size = sizeof(value);
    CONFIGRET cr = CM_Get_DevNode_PropertyW(devInst, &key, &propType,
                                            (PBYTE)&value, &size, 0);
    if (cr == CR_SUCCESS && propType == DEVPROP_TYPE_UINT32) {
        return value;
    }
    return std::nullopt;
}

std::optional<GUID> GetDevNodeGuidProperty(DEVINST devInst,
                                           const DEVPROPKEY& key) {
    DEVPROPTYPE propType = 0;
    GUID value{};
    ULONG size = sizeof(value);
    CONFIGRET cr = CM_Get_DevNode_PropertyW(devInst, &key, &propType,
                                            (PBYTE)&value, &size, 0);
    if (cr == CR_SUCCESS && propType == DEVPROP_TYPE_GUID) {
        return value;
    }
    return std::nullopt;
}

std::wstring GetDevNodeStringProperty(DEVINST devInst, const DEVPROPKEY& key) {
    DEVPROPTYPE propType = 0;
    ULONG size = 0;
    CONFIGRET cr =
        CM_Get_DevNode_PropertyW(devInst, &key, &propType, nullptr, &size, 0);
    if (cr != CR_BUFFER_SMALL || size == 0 ||
        (propType != DEVPROP_TYPE_STRING &&
         propType != DEVPROP_TYPE_STRING_LIST)) {
        return {};
    }

    std::vector<BYTE> buffer(size);
    cr = CM_Get_DevNode_PropertyW(devInst, &key, &propType, buffer.data(),
                                  &size, 0);
    if (cr != CR_SUCCESS) {
        return {};
    }

    const wchar_t* wstr = reinterpret_cast<const wchar_t*>(buffer.data());
    size_t maxChars = size / sizeof(wchar_t);
    if (propType == DEVPROP_TYPE_STRING_LIST) {
        std::wstring result;
        size_t pos = 0;
        while (pos < maxChars && wstr[pos] != L'\0') {
            std::wstring_view part(&wstr[pos]);
            if (!result.empty()) {
                result += L' ';
            }
            result += part;
            pos += part.length() + 1;
        }
        return result;
    }

    size_t len = wcsnlen(wstr, maxChars);
    return std::wstring(wstr, len);
}

std::optional<ULONGLONG> ExtractBluetoothMacAddress(
    std::wstring_view instanceId,
    std::wstring_view addressProp) {
    if (addressProp.length() == 12) {
        wchar_t* endPtr = nullptr;
        ULONGLONG addr = std::wcstoull(std::wstring(addressProp).c_str(),
                                       &endPtr, 16);
        if (endPtr && *endPtr == L'\0' && addr != 0) {
            return addr;
        }
    }

    auto isHex12 = [](std::wstring_view sv) {
        if (sv.length() != 12) {
            return false;
        }
        for (wchar_t c : sv) {
            if (!std::iswxdigit(c)) {
                return false;
            }
        }
        return true;
    };

    std::wstring upper(instanceId);
    for (wchar_t& c : upper) {
        c = std::towupper(c);
    }

    size_t devPos = upper.find(L"DEV_");
    if (devPos != std::wstring::npos && devPos + 16 <= upper.length()) {
        std::wstring_view candidate(upper.data() + devPos + 4, 12);
        if (isHex12(candidate)) {
            ULONGLONG addr =
                std::wcstoull(std::wstring(candidate).c_str(), nullptr, 16);
            if (addr != 0) {
                return addr;
            }
        }
    }

    for (size_t i = 0; i + 13 <= upper.length(); ++i) {
        if (upper[i] == L'_' || upper[i] == L'&') {
            std::wstring_view candidate(upper.data() + i + 1, 12);
            bool boundaryAfter =
                (i + 13 == upper.length()) || !std::iswxdigit(upper[i + 13]);
            if (boundaryAfter && isHex12(candidate)) {
                ULONGLONG addr =
                    std::wcstoull(std::wstring(candidate).c_str(), nullptr, 16);
                if (addr != 0) {
                    return addr;
                }
            }
        }
    }

    return std::nullopt;
}

std::wstring GuidToStringKey(const GUID& g) {
    wchar_t buf[64]{};
    swprintf_s(
        buf,
        L"%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2],
        g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
    return buf;
}

// ---------------------------------------------------------------------------
// Bluetooth Battery Enumeration
// ---------------------------------------------------------------------------

struct ClassicBtDeviceRecord {
    ULONGLONG address = 0;
    std::wstring name;
    ULONG classOfDevice = 0;
    bool isConnected = false;
};

std::unordered_map<ULONGLONG, ClassicBtDeviceRecord>
QueryClassicBluetoothDevices() {
    std::unordered_map<ULONGLONG, ClassicBtDeviceRecord> result;

    static HMODULE hBtApis = LoadLibraryExW(
        L"BluetoothApis.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hBtApis) {
        return result;
    }

    static auto pFindFirst =
        reinterpret_cast<BluetoothFindFirstDevice_t>(
            GetProcAddress(hBtApis, "BluetoothFindFirstDevice"));
    static auto pFindNext =
        reinterpret_cast<BluetoothFindNextDevice_t>(
            GetProcAddress(hBtApis, "BluetoothFindNextDevice"));
    static auto pFindClose =
        reinterpret_cast<BluetoothFindDeviceClose_t>(
            GetProcAddress(hBtApis, "BluetoothFindDeviceClose"));

    if (!pFindFirst || !pFindNext || !pFindClose) {
        return result;
    }

    BLUETOOTH_DEVICE_SEARCH_PARAMS_LOCAL searchParams{};
    searchParams.dwSize = sizeof(searchParams);
    searchParams.fReturnAuthenticated = TRUE;
    searchParams.fReturnRemembered = TRUE;
    searchParams.fReturnUnknown = FALSE;
    searchParams.fReturnConnected = TRUE;
    searchParams.fIssueInquiry = FALSE;
    searchParams.cTimeoutMultiplier = 0;
    searchParams.hRadio = nullptr;

    BLUETOOTH_DEVICE_INFO_LOCAL deviceInfo{};
    deviceInfo.dwSize = sizeof(deviceInfo);

    HBLUETOOTH_DEVICE_FIND_LOCAL hFind = pFindFirst(&searchParams, &deviceInfo);
    if (hFind) {
        do {
            if (deviceInfo.Address != 0) {
                ClassicBtDeviceRecord rec;
                rec.address = deviceInfo.Address;
                rec.name = deviceInfo.szName;
                rec.classOfDevice = deviceInfo.ulClassofDevice;
                rec.isConnected = (deviceInfo.fConnected != FALSE);
                result[rec.address] = std::move(rec);
            }
            ZeroMemory(&deviceInfo, sizeof(deviceInfo));
            deviceInfo.dwSize = sizeof(deviceInfo);
        } while (pFindNext(hFind, &deviceInfo));

        pFindClose(hFind);
    }

    return result;
}

struct ContainerAggregation {
    std::wstring containerKey;
    std::wstring classicRootName;
    std::wstring bleRootName;
    std::wstring bestFriendlyName;
    std::wstring fallbackName;
    std::wstring categoryString;
    std::optional<int> batteryPercent;
    std::optional<ULONGLONG> classicBtAddress;
    std::optional<ULONGLONG> bleBtAddress;
    ULONG classOfDevice = 0;
    bool hasClassicRootNode = false;
    bool classicRootConnected = false;
    bool hasBleRootNode = false;
    bool bleRootConnected = false;
    bool anyStartedDevNode = false;
    std::optional<bool> aepIsConnected;
};

std::vector<BluetoothDeviceBatteryInfo> DiscoverConnectedBluetoothBatteries() {
    auto classicDevices = QueryClassicBluetoothDevices();

    ULONG listChars = 0;
    CONFIGRET cr = CM_Get_Device_ID_List_SizeW(
        &listChars, nullptr, CM_GETIDLIST_FILTER_NONE);
    if (cr != CR_SUCCESS || listChars <= 1) {
        return {};
    }

    std::vector<wchar_t> idListBuffer(listChars);
    cr = CM_Get_Device_ID_ListW(nullptr, idListBuffer.data(), listChars,
                                CM_GETIDLIST_FILTER_NONE);
    if (cr != CR_SUCCESS) {
        return {};
    }

    std::map<std::wstring, ContainerAggregation> containers;

    const wchar_t* curr = idListBuffer.data();
    while (*curr != L'\0') {
        std::wstring_view instanceId(curr);
        curr += instanceId.length() + 1;

        bool isBthEnum =
            _wcsnicmp(instanceId.data(), L"BTHENUM\\", 8) == 0;
        bool isBthLe =
            _wcsnicmp(instanceId.data(), L"BTHLE\\", 6) == 0;
        bool isBthLeDevice =
            _wcsnicmp(instanceId.data(), L"BTHLEDEVICE\\", 12) == 0;

        if (!isBthEnum && !isBthLe && !isBthLeDevice) {
            continue;
        }

        DEVINST devInst = 0;
        if (CM_Locate_DevNodeW(&devInst, const_cast<DEVINSTID_W>(instanceId.data()),
                               CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) {
            continue;
        }

        auto containerGuidOpt =
            GetDevNodeGuidProperty(devInst, DEVPKEY_Device_ContainerId);
        if (!containerGuidOpt) {
            continue;
        }

        GUID zeroGuid{};
        if (memcmp(&(*containerGuidOpt), &zeroGuid, sizeof(GUID)) == 0) {
            continue;
        }

        std::wstring containerKey = GuidToStringKey(*containerGuidOpt);
        auto& agg = containers[containerKey];
        agg.containerKey = containerKey;

        bool isClassicRoot =
            isBthEnum && ContainsIgnoreCase(instanceId, L"\\Dev_");
        bool isBleRoot =
            isBthLe && ContainsIgnoreCase(instanceId, L"\\Dev_");

        ULONG devStatus = 0;
        ULONG devProblem = 0;
        bool nodeConnectedAndStarted = false;
        if (CM_Get_DevNode_Status(&devStatus, &devProblem, devInst, 0) ==
            CR_SUCCESS) {
            nodeConnectedAndStarted =
                ((devStatus & DN_STARTED) != 0) &&
                ((devStatus & DN_DEVICE_DISCONNECTED) == 0);
        }

        if (nodeConnectedAndStarted) {
            agg.anyStartedDevNode = true;
        }

        if (isClassicRoot) {
            agg.hasClassicRootNode = true;
            if (nodeConnectedAndStarted) {
                agg.classicRootConnected = true;
            }
        }
        if (isBleRoot) {
            agg.hasBleRootNode = true;
            if (nodeConnectedAndStarted) {
                agg.bleRootConnected = true;
            }
        }

        if (isClassicRoot || isBleRoot) {
            if (auto aepConn =
                    GetDevNodeBoolProperty(devInst, DEVPKEY_Aep_IsConnected)) {
                if (*aepConn) {
                    agg.aepIsConnected = true;
                } else if (!agg.aepIsConnected.has_value()) {
                    agg.aepIsConnected = false;
                }
            }
        }

        if (auto batteryByte =
                GetDevNodeByteProperty(devInst, DEVPKEY_Bluetooth_Battery)) {
            if (*batteryByte <= 100) {
                agg.batteryPercent = static_cast<int>(*batteryByte);
            }
        }

        if (auto codOpt = GetDevNodeUInt32Property(
                devInst, DEVPKEY_Bluetooth_ClassOfDevice)) {
            if (*codOpt != 0) {
                agg.classOfDevice = *codOpt;
            }
        }

        std::wstring addrStr =
            GetDevNodeStringProperty(devInst, DEVPKEY_Bluetooth_DeviceAddress);
        if (auto macOpt = ExtractBluetoothMacAddress(instanceId, addrStr)) {
            if (isBthEnum) {
                agg.classicBtAddress = *macOpt;
            } else {
                agg.bleBtAddress = *macOpt;
            }
        }

        std::wstring catStr = GetDevNodeStringProperty(
            devInst, DEVPKEY_DeviceContainer_Category_Local);
        if (!catStr.empty() && agg.categoryString.empty()) {
            agg.categoryString = std::move(catStr);
        }

        std::wstring friendlyName =
            GetDevNodeStringProperty(devInst, DEVPKEY_Device_FriendlyName);
        if (friendlyName.empty()) {
            friendlyName =
                GetDevNodeStringProperty(devInst, DEVPKEY_Device_DeviceDesc);
        }

        if (!friendlyName.empty()) {
            if (isClassicRoot && !IsGenericProfileName(friendlyName)) {
                agg.classicRootName = friendlyName;
            } else if (isBleRoot && !IsGenericProfileName(friendlyName)) {
                agg.bleRootName = friendlyName;
            } else if (agg.bestFriendlyName.empty() &&
                       !IsGenericProfileName(friendlyName)) {
                agg.bestFriendlyName = friendlyName;
            } else if (agg.fallbackName.empty()) {
                agg.fallbackName = friendlyName;
            }
        }
    }

    std::vector<BluetoothDeviceBatteryInfo> connectedList;

    for (auto& [key, agg] : containers) {
        bool isConnected = agg.classicRootConnected || agg.bleRootConnected;
        uint16_t bleAppearanceRaw = 0;

        if (agg.classicBtAddress) {
            auto it = classicDevices.find(*agg.classicBtAddress);
            if (it != classicDevices.end()) {
                if (it->second.isConnected) {
                    isConnected = true;
                }
                if (agg.classicRootName.empty() && !it->second.name.empty()) {
                    agg.classicRootName = it->second.name;
                }
                if (agg.classOfDevice == 0 && it->second.classOfDevice != 0) {
                    agg.classOfDevice = it->second.classOfDevice;
                }
            }
        }

        if (agg.bleBtAddress &&
            (agg.bleRootConnected ||
             (!agg.hasBleRootNode && !agg.hasClassicRootNode))) {
            try {
                using namespace winrt::Windows::Devices::Bluetooth;
                using namespace winrt::Windows::Devices::Bluetooth::
                    GenericAttributeProfile;

                auto bleOp =
                    BluetoothLEDevice::FromBluetoothAddressAsync(*agg.bleBtAddress);
                auto bleDevice = bleOp.get();
                if (bleDevice) {
                    if (bleDevice.ConnectionStatus() ==
                        BluetoothConnectionStatus::Connected) {
                        isConnected = true;
                    }

                    if (agg.bleRootName.empty()) {
                        std::wstring winrtName(bleDevice.Name());
                        if (!winrtName.empty() &&
                            !IsGenericProfileName(winrtName)) {
                            agg.bleRootName = std::move(winrtName);
                        }
                    }

                    if (auto appearance = bleDevice.Appearance()) {
                        bleAppearanceRaw = appearance.RawValue();
                    }

                    if (isConnected && !agg.batteryPercent) {
                        auto servicesResult =
                            bleDevice
                                .GetGattServicesForUuidAsync(
                                    GattServiceUuids::Battery(),
                                    BluetoothCacheMode::Cached)
                                .get();
                        if (servicesResult.Status() ==
                                GattCommunicationStatus::Success &&
                            servicesResult.Services().Size() > 0) {
                            auto service = servicesResult.Services().GetAt(0);
                            auto charsResult =
                                service
                                    .GetCharacteristicsForUuidAsync(
                                        GattCharacteristicUuids::BatteryLevel(),
                                        BluetoothCacheMode::Cached)
                                    .get();
                            if (charsResult.Status() ==
                                    GattCommunicationStatus::Success &&
                                charsResult.Characteristics().Size() > 0) {
                                auto characteristic =
                                    charsResult.Characteristics().GetAt(0);
                                auto readResult =
                                    characteristic
                                        .ReadValueAsync(
                                            BluetoothCacheMode::Cached)
                                        .get();
                                if (readResult.Status() ==
                                        GattCommunicationStatus::Success &&
                                    readResult.Value() &&
                                    readResult.Value().Length() >= 1) {
                                    auto reader = winrt::Windows::Storage::
                                        Streams::DataReader::FromBuffer(
                                            readResult.Value());
                                    uint8_t level = reader.ReadByte();
                                    if (level <= 100) {
                                        agg.batteryPercent = level;
                                    }
                                }
                            }
                        }
                    }
                }
            } catch (...) {
            }
        }

        if (!isConnected && !agg.hasClassicRootNode && !agg.hasBleRootNode) {
            if (agg.aepIsConnected.has_value()) {
                isConnected = *agg.aepIsConnected;
            } else {
                isConnected = agg.anyStartedDevNode;
            }
        }

        if (!isConnected || !agg.batteryPercent.has_value()) {
            continue;
        }

        std::wstring displayName;
        if (!agg.classicRootName.empty()) {
            displayName = agg.classicRootName;
        } else if (!agg.bleRootName.empty()) {
            displayName = agg.bleRootName;
            if (_wcsnicmp(displayName.c_str(), L"LE_", 3) == 0 &&
                displayName.length() > 3) {
                displayName = displayName.substr(3);
            }
        } else if (!agg.bestFriendlyName.empty()) {
            displayName = agg.bestFriendlyName;
        } else {
            displayName = agg.fallbackName;
        }

        if (displayName.empty()) {
            displayName = L"Bluetooth Device";
        }

        DeviceCategory category = DeviceCategory::GenericBluetooth;
        if (agg.classOfDevice != 0) {
            category = ClassifyFromClassOfDevice(agg.classOfDevice);
        }
        if (category == DeviceCategory::GenericBluetooth &&
            bleAppearanceRaw != 0) {
            category = ClassifyFromBleAppearanceRaw(bleAppearanceRaw);
        }
        if (category == DeviceCategory::GenericBluetooth) {
            category = ClassifyFromNameAndCategoryString(displayName,
                                                         agg.categoryString);
        }

        BluetoothDeviceBatteryInfo info;
        info.id = agg.containerKey;
        info.name = std::move(displayName);
        info.batteryPercent = *agg.batteryPercent;
        info.category = category;
        info.isBle = agg.hasBleRootNode && !agg.hasClassicRootNode;
        connectedList.push_back(std::move(info));
    }

    std::sort(connectedList.begin(), connectedList.end(),
              [](const BluetoothDeviceBatteryInfo& a,
                 const BluetoothDeviceBatteryInfo& b) {
                  int cmp = _wcsicmp(a.name.c_str(), b.name.c_str());
                  if (cmp != 0) {
                      return cmp < 0;
                  }
                  return a.id < b.id;
              });

    return connectedList;
}

// ---------------------------------------------------------------------------
// Taskbar Visual Tree & XAML Helpers
// ---------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByNameRecursive(DependencyObject root,
                                          PCWSTR name,
                                          int depth = 0) {
    if (!root || depth > 8) {
        return nullptr;
    }

    if (auto fe = root.try_as<FrameworkElement>()) {
        if (fe.Name() == name) {
            return fe;
        }
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(root, i);
        if (auto found = FindChildByNameRecursive(child, name, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

FrameworkElement GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    auto curr = element;
    while (curr) {
        curr = Media::VisualTreeHelper::GetParent(curr)
                   .try_as<FrameworkElement>();
        if (curr && winrt::get_class_name(curr) == className) {
            return curr;
        }
    }
    return nullptr;
}

void CollectSystemTrayFramesRecursive(DependencyObject root,
                                      std::vector<FrameworkElement>& outFrames,
                                      int depth = 0) {
    if (!root || depth > 16) {
        return;
    }

    if (auto fe = root.try_as<FrameworkElement>()) {
        if (winrt::get_class_name(fe) == L"SystemTray.SystemTrayFrame") {
            bool alreadyAdded = false;
            for (const auto& existing : outFrames) {
                if (existing == fe) {
                    alreadyAdded = true;
                    break;
                }
            }
            if (!alreadyAdded) {
                outFrames.push_back(fe);
            }
            return;
        }
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(root, i);
        CollectSystemTrayFramesRecursive(child, outFrames, depth + 1);
    }

    if (auto cc = root.try_as<Controls::ContentControl>()) {
        if (auto contentObj = cc.Content().try_as<DependencyObject>()) {
            CollectSystemTrayFramesRecursive(contentObj, outFrames, depth + 1);
        }
    }
}

VisualStateGroup GetOrientationStatesGroup(
    FrameworkElement systemTrayFramePanel) {
    for (const auto& group :
         VisualStateManager::GetVisualStateGroups(systemTrayFramePanel)) {
        if (group.Name() == L"OrientationStates") {
            return group;
        }
    }
    return nullptr;
}

bool IsVerticalTrayPanel(FrameworkElement systemTrayFramePanel,
                         VisualStateGroup orientationStates) {
    if (auto sp = systemTrayFramePanel.try_as<Controls::StackPanel>()) {
        if (sp.Orientation() == Controls::Orientation::Vertical) {
            return true;
        }
    }
    auto currentState =
        orientationStates ? orientationStates.CurrentState() : nullptr;
    return currentState && currentState.Name() == L"VerticalOrientation";
}

bool IsDarkTheme(FrameworkElement element) {
    try {
        auto curr = element;
        while (curr) {
            auto actualTheme = curr.ActualTheme();
            if (actualTheme == ElementTheme::Dark) {
                return true;
            }
            if (actualTheme == ElementTheme::Light) {
                return false;
            }
            curr = Media::VisualTreeHelper::GetParent(curr)
                       .try_as<FrameworkElement>();
        }
        return Application::Current().RequestedTheme() ==
               ApplicationTheme::Dark;
    } catch (...) {
        return true;
    }
}

Media::Brush FindExistingTrayTextForeground(FrameworkElement root,
                                            int depth = 0) {
    if (!root || depth > 9 || root.Name() == kWidgetElementName) {
        return nullptr;
    }

    if (auto tb = root.try_as<Controls::TextBlock>()) {
        auto name = tb.Name();
        if (name == L"TimeInnerTextBlock" || name == L"DateInnerTextBlock" ||
            name == L"InnerTextBlock") {
            if (auto fg = tb.Foreground()) {
                return fg;
            }
        }
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(root, i)
                         .try_as<FrameworkElement>();
        if (auto found = FindExistingTrayTextForeground(child, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

Media::Brush GetDeviceForegroundBrush(FrameworkElement contextElement,
                                      FrameworkElement trayRootElement,
                                      int batteryPercent) {
    bool dark = IsDarkTheme(trayRootElement ? trayRootElement : contextElement);

    if (batteryPercent >= 0 && g_settings.colorCoding != ColorCoding::none) {
        if (batteryPercent <= 10) {
            winrt::Windows::UI::Color c =
                dark ? winrt::Windows::UI::Color{255, 255, 153, 164}
                     : winrt::Windows::UI::Color{255, 196, 43, 28};
            return Media::SolidColorBrush(c);
        }
        if (batteryPercent <= 20) {
            winrt::Windows::UI::Color c =
                dark ? winrt::Windows::UI::Color{255, 252, 225, 0}
                     : winrt::Windows::UI::Color{255, 157, 93, 0};
            return Media::SolidColorBrush(c);
        }
        if (g_settings.colorCoding == ColorCoding::always) {
            winrt::Windows::UI::Color c =
                dark ? winrt::Windows::UI::Color{255, 108, 203, 95}
                     : winrt::Windows::UI::Color{255, 15, 123, 15};
            return Media::SolidColorBrush(c);
        }
    }

    if (trayRootElement) {
        if (auto existingBrush =
                FindExistingTrayTextForeground(trayRootElement)) {
            return existingBrush;
        }
    }

    winrt::Windows::UI::Color fallbackColor =
        dark ? winrt::Windows::UI::Color{255, 255, 255, 255}
             : winrt::Windows::UI::Color{228, 0, 0, 0};
    return Media::SolidColorBrush(fallbackColor);
}

std::wstring FormatShortDeviceName(std::wstring_view name) {
    if (g_settings.maxNameLength <= 0 ||
        static_cast<int>(name.length()) <= g_settings.maxNameLength) {
        return std::wstring(name);
    }
    std::wstring truncated(
        name.substr(0, static_cast<size_t>(g_settings.maxNameLength)));
    truncated += L"\u2026";
    return truncated;
}

void LaunchUriOnStaThread(PCWSTR uri) {
    std::wstring uriCopy(uri);
    std::thread([uriCopy = std::move(uriCopy)]() {
        HRESULT hrInit = CoInitializeEx(
            nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = L"open";
        sei.lpFile = uriCopy.c_str();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
        if (SUCCEEDED(hrInit)) {
            CoUninitialize();
        }
    }).detach();
}

void HandleWidgetClick(FrameworkElement) {
    static std::atomic<ULONGLONG> s_lastClickTick{0};
    ULONGLONG now = GetTickCount64();
    ULONGLONG prev = s_lastClickTick.load();
    if (now - prev < 250) {
        return;
    }
    s_lastClickTick.store(now);

    switch (g_settings.clickAction) {
        case ClickAction::openQuickSettings:
            LaunchUriOnStaThread(L"ms-controlcenter:bluetooth");
            break;

        case ClickAction::openBluetoothSettings:
            LaunchUriOnStaThread(L"ms-settings:bluetooth");
            break;

        case ClickAction::refresh:
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
            break;

        case ClickAction::none:
        default:
            break;
    }
}

void PopulateBluetoothWidgetContent(Controls::Border widgetBorder,
                                    FrameworkElement trayRootElement,
                                    bool verticalTaskbar) {
    std::vector<BluetoothDeviceBatteryInfo> devices;
    {
        std::lock_guard<std::mutex> lock(g_devicesMutex);
        devices = g_connectedDevices;
    }

    if (devices.empty() &&
        g_settings.whenDisconnected == WhenDisconnected::hide) {
        widgetBorder.Visibility(Visibility::Collapsed);
        return;
    }

    widgetBorder.Visibility(Visibility::Visible);

    std::vector<BluetoothDeviceBatteryInfo> displayDevices = devices;
    if (g_settings.multiDeviceMode == MultiDeviceMode::lowest &&
        displayDevices.size() > 1) {
        auto lowestIt = std::min_element(
            displayDevices.begin(), displayDevices.end(),
            [](const BluetoothDeviceBatteryInfo& a,
               const BluetoothDeviceBatteryInfo& b) {
                return a.batteryPercent < b.batteryPercent;
            });
        BluetoothDeviceBatteryInfo lowest = *lowestIt;
        displayDevices.clear();
        displayDevices.push_back(std::move(lowest));
    } else if (g_settings.maxDevices > 0 &&
               static_cast<int>(displayDevices.size()) >
                   g_settings.maxDevices) {
        displayDevices.resize(static_cast<size_t>(g_settings.maxDevices));
    }

    Controls::StackPanel itemsPanel;
    itemsPanel.Orientation(verticalTaskbar ? Controls::Orientation::Vertical
                                           : Controls::Orientation::Horizontal);
    itemsPanel.Spacing(static_cast<double>(g_settings.itemSpacing));
    itemsPanel.VerticalAlignment(VerticalAlignment::Center);
    itemsPanel.HorizontalAlignment(HorizontalAlignment::Center);

    double fontSize = static_cast<double>(g_settings.fontSize);
    Media::FontFamily iconFontFamily(L"Segoe Fluent Icons");

    if (displayDevices.empty()) {
        Controls::StackPanel itemStack;
        itemStack.Orientation(Controls::Orientation::Horizontal);
        itemStack.Spacing(4.0);
        itemStack.VerticalAlignment(VerticalAlignment::Center);

        auto fgBrush =
            GetDeviceForegroundBrush(widgetBorder, trayRootElement, -1);

        Controls::TextBlock iconTb;
        iconTb.FontFamily(iconFontFamily);
        iconTb.FontSize(fontSize + 2.0);
        iconTb.Text(L"\uE702");
        iconTb.VerticalAlignment(VerticalAlignment::Center);
        iconTb.Foreground(fgBrush);
        iconTb.Opacity(0.6);

        Controls::TextBlock textTb;
        textTb.FontSize(fontSize);
        textTb.Text(L"--%");
        textTb.VerticalAlignment(VerticalAlignment::Center);
        textTb.Foreground(fgBrush);
        textTb.Opacity(0.6);

        itemStack.Children().Append(iconTb);
        itemStack.Children().Append(textTb);
        itemsPanel.Children().Append(itemStack);
    } else {
        for (const auto& dev : displayDevices) {
            Controls::StackPanel itemStack;
            itemStack.Orientation(Controls::Orientation::Horizontal);
            itemStack.Spacing(4.0);
            itemStack.VerticalAlignment(VerticalAlignment::Center);

            auto fgBrush = GetDeviceForegroundBrush(
                widgetBorder, trayRootElement, dev.batteryPercent);

            bool showIcon =
                (g_settings.displayFormat == DisplayFormat::iconAndPercentage ||
                 g_settings.displayFormat ==
                     DisplayFormat::iconNameAndPercentage ||
                 g_settings.displayFormat ==
                     DisplayFormat::batteryIconAndPercentage);

            if (showIcon) {
                Controls::TextBlock iconTb;
                iconTb.FontFamily(iconFontFamily);
                iconTb.FontSize(fontSize + 2.0);
                iconTb.Text(g_settings.displayFormat ==
                                    DisplayFormat::batteryIconAndPercentage
                                ? GetBatteryLevelGlyph(dev.batteryPercent)
                                : GetCategoryGlyph(dev.category));
                iconTb.VerticalAlignment(VerticalAlignment::Center);
                iconTb.Foreground(fgBrush);
                itemStack.Children().Append(iconTb);
            }

            std::wstring labelText;
            if (g_settings.displayFormat == DisplayFormat::nameAndPercentage ||
                g_settings.displayFormat ==
                    DisplayFormat::iconNameAndPercentage) {
                labelText = FormatShortDeviceName(dev.name);
                labelText += L" ";
            }
            labelText += std::to_wstring(dev.batteryPercent) + L"%";

            Controls::TextBlock textTb;
            textTb.FontSize(fontSize);
            textTb.Text(labelText);
            textTb.VerticalAlignment(VerticalAlignment::Center);
            textTb.Foreground(fgBrush);
            itemStack.Children().Append(textTb);

            itemsPanel.Children().Append(itemStack);
        }
    }

    itemsPanel.IsHitTestVisible(false);
    widgetBorder.Child(itemsPanel);

    std::wstring tooltipText;
    if (devices.empty()) {
        tooltipText = L"Bluetooth: No connected devices reporting battery";
    } else {
        for (size_t i = 0; i < devices.size(); ++i) {
            if (i > 0) {
                tooltipText += L"\n";
            }
            const auto& dev = devices[i];
            tooltipText += dev.name + L" (" +
                           GetCategoryDisplayName(dev.category) + L"): " +
                           std::to_wstring(dev.batteryPercent) + L"%";
        }
    }

    Controls::ToolTipService::SetToolTip(widgetBorder,
                                         winrt::box_value(tooltipText));
    Automation::AutomationProperties::SetName(widgetBorder, tooltipText);
}

Controls::Border CreateBluetoothWidgetElement(bool isInsideControlCenter) {
    // Alpha=1 ensures layered XAML Island InputSite hit-testing registers
    // clicks across the entire pill while remaining visually transparent.
    const winrt::Windows::UI::Color kRestBgColor{1, 128, 128, 128};

    Controls::Border border;
    border.Name(kWidgetElementName);
    border.IsHitTestVisible(true);
    border.VerticalAlignment(VerticalAlignment::Stretch);
    border.HorizontalAlignment(HorizontalAlignment::Center);
    border.Padding(isInsideControlCenter ? Thickness{6, 0, 2, 0}
                                         : Thickness{8, 0, 8, 0});
    border.Margin(isInsideControlCenter ? Thickness{0, 0, 0, 0}
                                        : Thickness{2, 4, 2, 4});
    border.CornerRadius(CornerRadius{4.0, 4.0, 4.0, 4.0});
    border.Background(Media::SolidColorBrush(kRestBgColor));

    if (!isInsideControlCenter) {
        border.PointerEntered(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               Input::PointerRoutedEventArgs const&) {
                if (auto b = sender.try_as<Controls::Border>()) {
                    bool dark = IsDarkTheme(b);
                    winrt::Windows::UI::Color hoverColor =
                        dark ? winrt::Windows::UI::Color{18, 255, 255, 255}
                             : winrt::Windows::UI::Color{14, 0, 0, 0};
                    b.Background(Media::SolidColorBrush(hoverColor));
                }
            });

        border.PointerExited(
            [kRestBgColor](
                winrt::Windows::Foundation::IInspectable const& sender,
                Input::PointerRoutedEventArgs const&) {
                if (auto b = sender.try_as<Controls::Border>()) {
                    b.Background(Media::SolidColorBrush(kRestBgColor));
                }
            });

        border.PointerPressed(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               Input::PointerRoutedEventArgs const& e) {
                if (g_settings.clickAction == ClickAction::none) {
                    return;
                }
                if (auto b = sender.try_as<Controls::Border>()) {
                    auto pt = e.GetCurrentPoint(b);
                    if (pt && pt.Properties().IsLeftButtonPressed()) {
                        e.Handled(true);
                        b.CapturePointer(e.Pointer());
                        bool dark = IsDarkTheme(b);
                        winrt::Windows::UI::Color pressColor =
                            dark ? winrt::Windows::UI::Color{30, 255, 255, 255}
                                 : winrt::Windows::UI::Color{24, 0, 0, 0};
                        b.Background(Media::SolidColorBrush(pressColor));
                    }
                }
            });

        border.PointerReleased(
            [kRestBgColor](
                winrt::Windows::Foundation::IInspectable const& sender,
                Input::PointerRoutedEventArgs const& e) {
                if (g_settings.clickAction == ClickAction::none) {
                    return;
                }
                if (auto b = sender.try_as<Controls::Border>()) {
                    bool hadCapture = false;
                    if (auto captures = b.PointerCaptures()) {
                        for (const auto& cap : captures) {
                            if (cap.PointerId() == e.Pointer().PointerId()) {
                                hadCapture = true;
                                break;
                            }
                        }
                    }
                    b.ReleasePointerCapture(e.Pointer());

                    auto pt = e.GetCurrentPoint(b);
                    auto pos = pt ? pt.Position()
                                  : winrt::Windows::Foundation::Point{-1, -1};
                    bool inside = pos.X >= 0 && pos.Y >= 0 &&
                                  pos.X <= b.ActualWidth() &&
                                  pos.Y <= b.ActualHeight();

                    if (inside) {
                        bool dark = IsDarkTheme(b);
                        winrt::Windows::UI::Color hoverColor =
                            dark ? winrt::Windows::UI::Color{18, 255, 255, 255}
                                 : winrt::Windows::UI::Color{14, 0, 0, 0};
                        b.Background(Media::SolidColorBrush(hoverColor));
                    } else {
                        b.Background(Media::SolidColorBrush(kRestBgColor));
                    }

                    if (hadCapture && inside) {
                        e.Handled(true);
                        HandleWidgetClick(b);
                    }
                }
            });

        border.PointerCaptureLost(
            [kRestBgColor](
                winrt::Windows::Foundation::IInspectable const& sender,
                Input::PointerRoutedEventArgs const&) {
                if (auto b = sender.try_as<Controls::Border>()) {
                    b.Background(Media::SolidColorBrush(kRestBgColor));
                }
            });

        border.Tapped(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               Input::TappedRoutedEventArgs const& e) {
                if (g_settings.clickAction != ClickAction::none) {
                    if (auto fe = sender.try_as<FrameworkElement>()) {
                        e.Handled(true);
                        HandleWidgetClick(fe);
                    }
                }
            });
    } else {
        border.Tapped(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               Input::TappedRoutedEventArgs const& e) {
                if (g_settings.clickAction ==
                        ClickAction::openBluetoothSettings ||
                    g_settings.clickAction == ClickAction::refresh) {
                    if (auto fe = sender.try_as<FrameworkElement>()) {
                        e.Handled(true);
                        HandleWidgetClick(fe);
                    }
                }
            });
    }

    border.ActualThemeChanged(
        [](FrameworkElement const& sender,
           winrt::Windows::Foundation::IInspectable const&) {
            if (g_unloading) {
                return;
            }
            if (auto b = sender.try_as<Controls::Border>()) {
                auto parent = Media::VisualTreeHelper::GetParent(b)
                                  .try_as<FrameworkElement>();
                PopulateBluetoothWidgetContent(b, parent, false);
            }
        });

    return border;
}

void RemoveAllInjectedWidgets() {
    if (!g_injectedWidgets) {
        return;
    }

    g_inLayoutUpdate = true;
    for (auto& state : *g_injectedWidgets) {
        auto parent = state.parentPanel.get();
        auto widget = state.widgetElement.get();

        if (parent && widget) {
            uint32_t index = 0;
            if (parent.Children().IndexOf(widget, index)) {
                parent.Children().RemoveAt(index);
            }

            if (state.insertedColumnIndex >= 0) {
                if (auto grid = parent.try_as<Controls::Grid>()) {
                    for (const auto& child : grid.Children()) {
                        if (auto fe = child.try_as<FrameworkElement>()) {
                            int col = Controls::Grid::GetColumn(fe);
                            int span = Controls::Grid::GetColumnSpan(fe);
                            if (col > state.insertedColumnIndex) {
                                Controls::Grid::SetColumn(fe, col - 1);
                            } else if (span > 1 &&
                                       col <= state.insertedColumnIndex &&
                                       col + span > state.insertedColumnIndex) {
                                Controls::Grid::SetColumnSpan(fe, span - 1);
                            }
                        }
                    }

                    for (const auto& colRef : state.addedColumns) {
                        if (auto addedCol = colRef.get()) {
                            uint32_t colIdx = 0;
                            if (grid.ColumnDefinitions().IndexOf(addedCol,
                                                                 colIdx)) {
                                grid.ColumnDefinitions().RemoveAt(colIdx);
                            }
                        }
                    }
                }
            }

            parent.InvalidateMeasure();
        }
        if (auto trayFrame = state.systemTrayFrame.get()) {
            trayFrame.InvalidateMeasure();
        }
    }
    g_inLayoutUpdate = false;

    g_injectedWidgets->clear();
}

bool ApplyBluetoothWidgetToSystemTrayFrame(FrameworkElement systemTrayFrame,
                                           bool forceUpdate);
bool ApplyBluetoothWidgetToXamlRoot(XamlRoot xamlRoot, bool forceUpdate);

void SubscribeToOrientationChanges(FrameworkElement systemTrayFrame,
                                   VisualStateGroup orientationStates) {
    if (!g_orientationSubscriptions) {
        return;
    }

    std::erase_if(*g_orientationSubscriptions,
                  [](const OrientationStatesSubscription& s) {
                      return !s.group.get();
                  });

    for (const auto& sub : *g_orientationSubscriptions) {
        if (sub.group.get() == orientationStates) {
            return;
        }
    }

    auto trayWeak = winrt::make_weak(systemTrayFrame);
    g_orientationSubscriptions->push_back({
        .group = orientationStates,
        .currentStateChangedToken = orientationStates.CurrentStateChanged(
            [trayWeak](winrt::Windows::Foundation::IInspectable const&,
                       VisualStateChangedEventArgs const&) {
                if (g_unloading) {
                    return;
                }
                if (auto tray = trayWeak.get()) {
                    ApplyBluetoothWidgetToSystemTrayFrame(tray, true);
                }
            }),
    });
}

void InvalidateTrayAncestors(FrameworkElement systemTrayFrame) {
    auto curr = systemTrayFrame;
    for (int i = 0; i < 6 && curr; ++i) {
        curr.InvalidateMeasure();
        curr.InvalidateArrange();
        curr = Media::VisualTreeHelper::GetParent(curr)
                   .try_as<FrameworkElement>();
    }
}

UIElement GetDirectChildContaining(Controls::Panel parentPanel,
                                   FrameworkElement descendant) {
    if (!parentPanel || !descendant) {
        return nullptr;
    }
    DependencyObject curr = descendant;
    while (curr) {
        auto p = Media::VisualTreeHelper::GetParent(curr);
        if (!p) {
            return nullptr;
        }
        if (p == parentPanel) {
            return curr.try_as<UIElement>();
        }
        curr = p;
    }
    return nullptr;
}

bool ApplyBluetoothWidgetToSystemTrayFrame(FrameworkElement systemTrayFrame,
                                           bool forceUpdate) {
    if (g_unloading || g_inLayoutUpdate || !systemTrayFrame ||
        !g_injectedWidgets) {
        return false;
    }

    // Note: Despite being named "SystemTrayFrameGrid", on newer Windows 11 builds
    // (e.g. Build 26300) this element is a Windows.UI.Xaml.Controls.StackPanel,
    // while on older Windows 11 builds it is a Windows.UI.Xaml.Controls.Grid.
    // Both derive from Controls::Panel.
    auto trayContainerElement =
        FindChildByNameRecursive(systemTrayFrame, L"SystemTrayFrameGrid");
    if (!trayContainerElement) {
        return false;
    }

    auto systemTrayFramePanel = trayContainerElement.try_as<Controls::Panel>();
    if (!systemTrayFramePanel) {
        Wh_Log(L"SystemTrayFrameGrid is not a Panel: %s",
               winrt::get_class_name(trayContainerElement).c_str());
        return false;
    }

    std::erase_if(*g_injectedWidgets, [](const InjectedWidgetState& s) {
        return !s.widgetElement.get() || !s.parentPanel.get();
    });

    uint64_t currentGen = g_devicesGeneration.load();

    for (auto& state : *g_injectedWidgets) {
        if (state.systemTrayFrame.get() == systemTrayFrame) {
            if (!forceUpdate && state.renderedGeneration == currentGen) {
                return true;
            }
            auto widget = state.widgetElement.get();
            if (widget) {
                g_inLayoutUpdate = true;
                auto orientationStates =
                    GetOrientationStatesGroup(systemTrayFramePanel);
                bool vertical = IsVerticalTrayPanel(systemTrayFramePanel,
                                                    orientationStates);
                PopulateBluetoothWidgetContent(widget, systemTrayFramePanel,
                                               vertical);
                state.renderedGeneration = currentGen;
                InvalidateTrayAncestors(systemTrayFrame);
                g_inLayoutUpdate = false;
                return true;
            }
        }
    }

    g_inLayoutUpdate = true;

    auto orientationStates = GetOrientationStatesGroup(systemTrayFramePanel);
    if (orientationStates) {
        SubscribeToOrientationChanges(systemTrayFrame, orientationStates);
    }
    bool vertical =
        IsVerticalTrayPanel(systemTrayFramePanel, orientationStates);

    if (g_settings.position == TrayPosition::insideControlCenter) {
        FrameworkElement controlCenterButton =
            FindChildByNameRecursive(systemTrayFramePanel,
                                     L"ControlCenterButton");
        FrameworkElement buttonRootFe =
            controlCenterButton
                ? EnumChildElements(controlCenterButton,
                                    [](FrameworkElement c) {
                                        return !!c.try_as<Controls::Panel>();
                                    })
                : nullptr;
        auto buttonRootPanel =
            buttonRootFe ? buttonRootFe.try_as<Controls::Panel>() : nullptr;
        if (!buttonRootPanel) {
            g_inLayoutUpdate = false;
            return false;
        }

        auto widget = CreateBluetoothWidgetElement(true);
        std::vector<winrt::weak_ref<Controls::ColumnDefinition>> addedCols;
        int insertedCol = -1;

        if (auto buttonGrid = buttonRootPanel.try_as<Controls::Grid>()) {
            if (buttonGrid.ColumnDefinitions().Size() == 0) {
                Controls::ColumnDefinition c0;
                c0.Width(GridLength{1.0, GridUnitType::Auto});
                Controls::ColumnDefinition c1;
                c1.Width(GridLength{1.0, GridUnitType::Auto});
                buttonGrid.ColumnDefinitions().Append(c0);
                buttonGrid.ColumnDefinitions().Append(c1);
                addedCols.push_back(c0);
                addedCols.push_back(c1);
            } else {
                Controls::ColumnDefinition c0;
                c0.Width(GridLength{1.0, GridUnitType::Auto});
                buttonGrid.ColumnDefinitions().InsertAt(0, c0);
                addedCols.push_back(c0);
            }

            for (const auto& child : buttonGrid.Children()) {
                if (auto fe = child.try_as<FrameworkElement>()) {
                    int c = Controls::Grid::GetColumn(fe);
                    Controls::Grid::SetColumn(fe, c + 1);
                }
            }

            Controls::Grid::SetColumn(widget, 0);
            buttonGrid.Children().Append(widget);
            insertedCol = 0;
        } else {
            buttonRootPanel.Children().InsertAt(0, widget);
        }

        PopulateBluetoothWidgetContent(widget, systemTrayFramePanel, vertical);

        g_injectedWidgets->push_back({
            .systemTrayFrame = systemTrayFrame,
            .parentPanel = buttonRootPanel,
            .widgetElement = widget,
            .addedColumns = std::move(addedCols),
            .insertedColumnIndex = insertedCol,
            .renderedGeneration = currentGen,
        });

        InvalidateTrayAncestors(systemTrayFrame);
        g_inLayoutUpdate = false;
        return true;
    }

    PCWSTR anchorName = L"ControlCenterButton";
    bool insertAfter = false;

    switch (g_settings.position) {
        case TrayPosition::beforeControlCenter:
            anchorName = L"ControlCenterButton";
            insertAfter = false;
            break;
        case TrayPosition::afterControlCenter:
            anchorName = L"ControlCenterButton";
            insertAfter = true;
            break;
        case TrayPosition::beforeTrayIcons:
            anchorName = L"MainStack";
            insertAfter = false;
            break;
        default:
            break;
    }

    FrameworkElement anchor =
        FindChildByNameRecursive(systemTrayFramePanel, anchorName);
    if (!anchor) {
        anchor = FindChildByNameRecursive(systemTrayFramePanel,
                                          L"ControlCenterButton");
    }
    if (!anchor) {
        anchor = FindChildByNameRecursive(systemTrayFramePanel,
                                          L"NotificationCenterButton");
    }

    auto widget = CreateBluetoothWidgetElement(false);
    std::vector<winrt::weak_ref<Controls::ColumnDefinition>> addedCols;
    int insertedCol = -1;

    if (auto trayGrid = systemTrayFramePanel.try_as<Controls::Grid>()) {
        // Older Windows 11 builds where SystemTrayFrameGrid is a Grid
        uint32_t existingCols = trayGrid.ColumnDefinitions().Size();
        int insertCol = 0;

        if (anchor && existingCols > 0) {
            int anchorCol = Controls::Grid::GetColumn(anchor);
            insertCol = insertAfter ? (anchorCol + 1) : anchorCol;
            insertCol =
                std::clamp(insertCol, 0, static_cast<int>(existingCols));

            Controls::ColumnDefinition newColDef;
            newColDef.Width(GridLength{1.0, GridUnitType::Auto});
            trayGrid.ColumnDefinitions().InsertAt(
                static_cast<uint32_t>(insertCol), newColDef);
            addedCols.push_back(newColDef);

            for (const auto& child : trayGrid.Children()) {
                if (auto fe = child.try_as<FrameworkElement>()) {
                    int c = Controls::Grid::GetColumn(fe);
                    int span = Controls::Grid::GetColumnSpan(fe);
                    if (c >= insertCol) {
                        Controls::Grid::SetColumn(fe, c + 1);
                    } else if (span > 1 && c < insertCol &&
                               c + span > insertCol) {
                        Controls::Grid::SetColumnSpan(fe, span + 1);
                    }
                }
            }

            Controls::Grid::SetColumn(widget, insertCol);
            insertedCol = insertCol;
        }

        if (anchor) {
            Controls::Grid::SetRow(widget, Controls::Grid::GetRow(anchor));
            int rowSpan = Controls::Grid::GetRowSpan(anchor);
            if (rowSpan > 1) {
                Controls::Grid::SetRowSpan(widget, rowSpan);
            }
        }

        trayGrid.Children().Append(widget);
    } else {
        // Newer Windows 11 builds where SystemTrayFrameGrid is a StackPanel
        uint32_t totalChildren = systemTrayFramePanel.Children().Size();
        uint32_t insertIndex = totalChildren;

        if (UIElement directAnchor =
                GetDirectChildContaining(systemTrayFramePanel, anchor)) {
            uint32_t anchorIdx = 0;
            if (systemTrayFramePanel.Children().IndexOf(directAnchor,
                                                        anchorIdx)) {
                insertIndex = insertAfter ? (anchorIdx + 1) : anchorIdx;
                if (insertIndex > totalChildren) {
                    insertIndex = totalChildren;
                }
            }
        }

        systemTrayFramePanel.Children().InsertAt(insertIndex, widget);
        Wh_Log(L"Inserted Bluetooth battery widget into StackPanel at index %u",
               insertIndex);
    }

    PopulateBluetoothWidgetContent(widget, systemTrayFramePanel, vertical);

    g_injectedWidgets->push_back({
        .systemTrayFrame = systemTrayFrame,
        .parentPanel = systemTrayFramePanel,
        .widgetElement = widget,
        .addedColumns = std::move(addedCols),
        .insertedColumnIndex = insertedCol,
        .renderedGeneration = currentGen,
    });

    InvalidateTrayAncestors(systemTrayFrame);
    g_inLayoutUpdate = false;
    return true;
}

bool ApplyBluetoothWidgetToXamlRoot(XamlRoot xamlRoot, bool forceUpdate) {
    if (g_unloading || !xamlRoot) {
        return false;
    }

    auto rootContent = xamlRoot.Content();
    if (!rootContent) {
        return false;
    }

    std::vector<FrameworkElement> frames;
    CollectSystemTrayFramesRecursive(rootContent, frames);
    if (frames.empty()) {
        return false;
    }

    bool anyApplied = false;
    for (auto& frame : frames) {
        if (ApplyBluetoothWidgetToSystemTrayFrame(frame, forceUpdate)) {
            anyApplied = true;
        }
    }

    return anyApplied;
}

// ---------------------------------------------------------------------------
// Taskbar Host & Window Subclass Dispatch
// ---------------------------------------------------------------------------

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

void* TaskbarHost_FrameHeight_Original = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t
    CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original = nullptr;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        }
    }
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(),
            winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (std__Ref_count_base__Decref_Original && taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetPropW(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
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

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        FindWindowExW(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

void PerformUiThreadWidgetUpdate(bool recreate) {
    if (g_unloading || recreate) {
        if (g_unloading && g_autoRevokerList) {
            g_autoRevokerList->clear();
        }

        if (g_orientationSubscriptions && (g_unloading || recreate)) {
            for (const auto& sub : *g_orientationSubscriptions) {
                if (auto group = sub.group.get()) {
                    group.CurrentStateChanged(sub.currentStateChangedToken);
                }
            }
            g_orientationSubscriptions->clear();
        }

        RemoveAllInjectedWidgets();
    }

    if (g_unloading) {
        g_autoRevokerList.reset();
        g_injectedWidgets.reset();
        g_orientationSubscriptions.reset();
        return;
    }

    if (g_injectedWidgets) {
        std::vector<FrameworkElement> liveFrames;
        for (const auto& state : *g_injectedWidgets) {
            if (auto frame = state.systemTrayFrame.get()) {
                liveFrames.push_back(frame);
            }
        }
        for (auto& frame : liveFrames) {
            try {
                ApplyBluetoothWidgetToSystemTrayFrame(frame, true);
            } catch (...) {
            }
        }
    }

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
            WCHAR className[32];
            if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (g_settings.showOnSecondaryTaskbars &&
                       _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (xamlRoot) {
                try {
                    ApplyBluetoothWidgetToXamlRoot(xamlRoot, true);
                } catch (...) {
                    HRESULT hr = winrt::to_hresult();
                    Wh_Log(L"Exception in ApplyBluetoothWidgetToXamlRoot: %08X",
                           hr);
                }
            }
            return TRUE;
        },
        0);
}

LRESULT CALLBACK TaskbarWndSubclassProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR) {
    if (uMsg == g_msgUpdateWidgets && g_msgUpdateWidgets != 0) {
        PerformUiThreadWidgetUpdate(wParam != 0);
        return 0;
    }

    if (uMsg == WM_DEVICECHANGE && !g_unloading && g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool EnsureTaskbarSubclassed() {
    if (g_isNonTaskbarProcess.load()) {
        return false;
    }

    if (g_taskbarSubclassed.load()) {
        return true;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    g_hTaskbarWnd = hTaskbarWnd;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hTaskbarWnd, TaskbarWndSubclassProc, 0)) {
        g_taskbarSubclassed = true;
        return true;
    }

    return false;
}

void RequestUiWidgetUpdate(bool recreateLayout, bool synchronous) {
    if (g_isNonTaskbarProcess.load()) {
        return;
    }

    if (!EnsureTaskbarSubclassed() || !g_hTaskbarWnd ||
        g_msgUpdateWidgets == 0) {
        return;
    }

    if (synchronous) {
        SendMessageW(g_hTaskbarWnd, g_msgUpdateWidgets,
                     recreateLayout ? 1 : 0, 0);
    } else {
        PostMessageW(g_hTaskbarWnd, g_msgUpdateWidgets,
                     recreateLayout ? 1 : 0, 0);
    }
}

// ---------------------------------------------------------------------------
// SystemTray Hooks
// ---------------------------------------------------------------------------

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original = nullptr;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);

    if (g_unloading || !g_autoRevokerList) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const&) {
            if (g_autoRevokerList) {
                g_autoRevokerList->erase(autoRevokerIt);
            }

            if (g_unloading) {
                return;
            }

            auto element = sender.try_as<FrameworkElement>();
            if (!element) {
                return;
            }

            try {
                if (auto trayFrame = GetParentElementByClassName(
                        element, L"SystemTray.SystemTrayFrame")) {
                    ApplyBluetoothWidgetToSystemTrayFrame(trayFrame, false);
                } else if (auto xamlRoot = element.XamlRoot()) {
                    ApplyBluetoothWidgetToXamlRoot(xamlRoot, false);
                }
            } catch (...) {
            }
        });

    return ret;
}

using SystemTrayFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
SystemTrayFrame_MeasureOverride_t SystemTrayFrame_MeasureOverride_Original =
    nullptr;
int WINAPI SystemTrayFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    int ret = SystemTrayFrame_MeasureOverride_Original(pThis, size, resultSize);

    if (!g_unloading && !g_inLayoutUpdate) {
        try {
            FrameworkElement systemTrayFrame = nullptr;
            ((IUnknown*)pThis)
                ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                 winrt::put_abi(systemTrayFrame));
            if (systemTrayFrame && systemTrayFrame.IsLoaded()) {
                ApplyBluetoothWidgetToSystemTrayFrame(systemTrayFrame, false);
            }
        } catch (...) {
        }
    }

    return ret;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &SystemTrayFrame_MeasureOverride_Original,
            SystemTrayFrame_MeasureOverride_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for SystemTray module");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
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
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandleW(L"Taskbar.View.dll");
        if (module) {
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
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }
    return module;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
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
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// ---------------------------------------------------------------------------
// Background Monitor Thread & PnP Notification
// ---------------------------------------------------------------------------

DWORD CALLBACK OnPnPDeviceNotification(HCMNOTIFICATION,
                                       PVOID,
                                       CM_NOTIFY_ACTION action,
                                       PCM_NOTIFY_EVENT_DATA,
                                       DWORD) {
    switch (action) {
        case CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL:
        case CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL:
        case CM_NOTIFY_ACTION_DEVICECUSTOMEVENT:
            if (!g_unloading && g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
            break;
        default:
            break;
    }
    return ERROR_SUCCESS;
}

DWORD WINAPI BluetoothMonitorThreadProc(LPVOID) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    bool firstRun = true;
    while (!g_unloading) {
        auto discovered = DiscoverConnectedBluetoothBatteries();

        bool changed = false;
        {
            std::lock_guard<std::mutex> lock(g_devicesMutex);
            if (firstRun || discovered != g_connectedDevices) {
                g_connectedDevices = std::move(discovered);
                g_devicesGeneration.fetch_add(1);
                changed = true;
                firstRun = false;
            }
        }

        if (changed && !g_unloading) {
            Wh_Log(L"Bluetooth devices updated: %zu connected",
                   g_connectedDevices.size());
            RequestUiWidgetUpdate(false, false);
        }

        if (g_unloading) {
            break;
        }

        DWORD waitMs =
            static_cast<DWORD>(std::clamp(g_settings.refreshIntervalSeconds, 3,
                                          3600)) *
            1000;
        WaitForSingleObject(g_wakeEvent, waitMs);
    }

    winrt::uninit_apartment();
    return 0;
}

// ---------------------------------------------------------------------------
// Settings Loader & Windhawk Lifecycle Entrypoints
// ---------------------------------------------------------------------------

void LoadSettings() {
    PCWSTR displayFormat = Wh_GetStringSetting(L"displayFormat");
    g_settings.displayFormat = DisplayFormat::iconAndPercentage;
    if (wcscmp(displayFormat, L"nameAndPercentage") == 0) {
        g_settings.displayFormat = DisplayFormat::nameAndPercentage;
    } else if (wcscmp(displayFormat, L"iconNameAndPercentage") == 0) {
        g_settings.displayFormat = DisplayFormat::iconNameAndPercentage;
    } else if (wcscmp(displayFormat, L"batteryIconAndPercentage") == 0) {
        g_settings.displayFormat = DisplayFormat::batteryIconAndPercentage;
    }
    Wh_FreeStringSetting(displayFormat);

    PCWSTR multiDeviceMode = Wh_GetStringSetting(L"multiDeviceMode");
    g_settings.multiDeviceMode = MultiDeviceMode::all;
    if (wcscmp(multiDeviceMode, L"lowest") == 0) {
        g_settings.multiDeviceMode = MultiDeviceMode::lowest;
    }
    Wh_FreeStringSetting(multiDeviceMode);

    g_settings.maxDevices = std::clamp(Wh_GetIntSetting(L"maxDevices"), 1, 16);
    g_settings.maxNameLength =
        std::clamp(Wh_GetIntSetting(L"maxNameLength"), 0, 64);

    PCWSTR position = Wh_GetStringSetting(L"position");
    g_settings.position = TrayPosition::beforeControlCenter;
    if (wcscmp(position, L"afterControlCenter") == 0) {
        g_settings.position = TrayPosition::afterControlCenter;
    } else if (wcscmp(position, L"beforeTrayIcons") == 0) {
        g_settings.position = TrayPosition::beforeTrayIcons;
    } else if (wcscmp(position, L"insideControlCenter") == 0) {
        g_settings.position = TrayPosition::insideControlCenter;
    }
    Wh_FreeStringSetting(position);

    PCWSTR whenDisconnected = Wh_GetStringSetting(L"whenDisconnected");
    g_settings.whenDisconnected = WhenDisconnected::hide;
    if (wcscmp(whenDisconnected, L"showPlaceholder") == 0) {
        g_settings.whenDisconnected = WhenDisconnected::showPlaceholder;
    }
    Wh_FreeStringSetting(whenDisconnected);

    PCWSTR colorCoding = Wh_GetStringSetting(L"colorCoding");
    g_settings.colorCoding = ColorCoding::lowOnly;
    if (wcscmp(colorCoding, L"always") == 0) {
        g_settings.colorCoding = ColorCoding::always;
    } else if (wcscmp(colorCoding, L"none") == 0) {
        g_settings.colorCoding = ColorCoding::none;
    }
    Wh_FreeStringSetting(colorCoding);

    PCWSTR clickAction = Wh_GetStringSetting(L"clickAction");
    g_settings.clickAction = ClickAction::openQuickSettings;
    if (wcscmp(clickAction, L"openBluetoothSettings") == 0) {
        g_settings.clickAction = ClickAction::openBluetoothSettings;
    } else if (wcscmp(clickAction, L"refresh") == 0) {
        g_settings.clickAction = ClickAction::refresh;
    } else if (wcscmp(clickAction, L"none") == 0) {
        g_settings.clickAction = ClickAction::none;
    }
    Wh_FreeStringSetting(clickAction);

    g_settings.showOnSecondaryTaskbars =
        Wh_GetIntSetting(L"showOnSecondaryTaskbars") != 0;
    g_settings.refreshIntervalSeconds =
        std::clamp(Wh_GetIntSetting(L"refreshIntervalSeconds"), 3, 3600);
    g_settings.fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 8, 28);
    g_settings.itemSpacing =
        std::clamp(Wh_GetIntSetting(L"itemSpacing"), 0, 32);
}

BOOL Wh_ModInit() {
    g_msgUpdateWidgets =
        RegisterWindowMessageW(L"Windhawk_BtBattery_Update_" WH_MOD_ID);

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        HWND globalTray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (globalTray) {
            DWORD trayPid = 0;
            GetWindowThreadProcessId(globalTray, &trayPid);
            if (trayPid != 0 && trayPid != GetCurrentProcessId()) {
                g_isNonTaskbarProcess = true;
                return;
            }
        }
    } else {
        EnsureTaskbarSubclassed();
    }

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    CM_NOTIFY_FILTER notifyFilter{};
    notifyFilter.cbSize = sizeof(notifyFilter);
    notifyFilter.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE;
    notifyFilter.Flags = CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES;
    CM_Register_Notification(&notifyFilter, nullptr, OnPnPDeviceNotification,
                             &g_cmNotificationHandle);

    g_workerThread = CreateThread(nullptr, 0, BluetoothMonitorThreadProc,
                                  nullptr, 0, nullptr);
}

void Wh_ModBeforeUninit() {
    g_unloading = true;

    if (g_cmNotificationHandle) {
        CM_Unregister_Notification(g_cmNotificationHandle);
        g_cmNotificationHandle = nullptr;
    }

    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }

    if (g_taskbarSubclassed.load() && g_hTaskbarWnd) {
        SendMessageW(g_hTaskbarWnd, g_msgUpdateWidgets, 1, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hTaskbarWnd, TaskbarWndSubclassProc);
        g_taskbarSubclassed = false;
    }
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
    if (g_isNonTaskbarProcess.load()) {
        return;
    }

    LoadSettings();
    g_devicesGeneration.fetch_add(1);

    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }

    RequestUiWidgetUpdate(true, true);
}
